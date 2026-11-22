#include "LevelSketch.h"
#include <EEPROM.h>
#include <util/atomic.h>
#include <avr/interrupt.h>
#include <math.h>
#include <stdlib.h>
#include <string.h>

#if !defined(__AVR_ATmega328P__) || F_CPU != 16000000UL
#error Select the 16 MHz ATmega328P Arduino Uno (Rev3).
#endif

namespace {
constexpr uint16_t BLOCK = 256;
constexpr uint8_t SLOTS = 32, CAL_BLOCKS = 38;  // 画面切替に備えてキューに余裕を持たせる。校正時間は1.012秒。
// ADCの各ブロックに適用する係数は 1 - exp(-26.624 ms / 時定数)。
constexpr float SLOW_ALPHA = 0.08492f, FAST_ALPHA = 0.35837f;
float slowAlpha=SLOW_ALPHA, fastAlpha=FAST_ALPHA;
float idleActivityCounts=LevelSketch::IDLE_ACTIVITY_COUNTS;
float responseAlpha(uint16_t milliseconds) {
  // 初期設定では以前のスケッチと同じ係数を使う。
  if (milliseconds==300) return SLOW_ALPHA;
  if (milliseconds==60) return FAST_ALPHA;
  return milliseconds ? 1.0f-expf(-26.624f/milliseconds) : 1.0f;
}
constexpr int8_t DEFAULT_REF_DBV = -20;
constexpr float GATE = 3.0f;                       // 単位はADCカウントの実効値。電源5 Vのとき約15 mV。
struct Block { uint32_t sum, squares; uint16_t low, high, seq; };
volatile Block queue[SLOTS];
volatile uint8_t head, tail;
volatile uint16_t dropped;

struct Preferences {
  uint16_t magic;
  float voltsPerCount, referenceDbV;  // v1では2つ目のfloatに基準の生カウントを保存していた。
  uint16_t crc;
};
Preferences prefs{};
uint16_t checksum(const Preferences& value) {
  uint16_t crc = 0xffff;
  const uint8_t* bytes = reinterpret_cast<const uint8_t*>(&value);
  for (size_t i=0; i<offsetof(Preferences,crc); ++i) {
    crc ^= bytes[i];
    for (uint8_t bit=0; bit<8; ++bit) crc = (crc & 1) ? (crc >> 1) ^ 0xa001 : crc >> 1;
  }
  return crc;
}
void save() { prefs.magic=0x4c32; prefs.crc=checksum(prefs); EEPROM.put(0,prefs); }
void loadPreferences() {
  EEPROM.get(0,prefs);
  bool validScale=prefs.voltsPerCount==0 ||
      (prefs.voltsPerCount>=0.0035f && prefs.voltsPerCount<=0.0065f);
  if (prefs.crc==checksum(prefs) && validScale) {
    if (prefs.magic==0x4c32 && prefs.referenceDbV>=-36 && prefs.referenceDbV<=0) return;
    if (prefs.magic==0x4c31) {
      float counts=prefs.referenceDbV;
      prefs.referenceDbV=DEFAULT_REF_DBV;
      if (counts>0 && counts<=600 && prefs.voltsPerCount>0) {
        float db=round(20*log10(counts*prefs.voltsPerCount));
        prefs.referenceDbV=constrain(db,-36.0f,0.0f);
      }
      save();
      return;
    }
  }
  prefs.voltsPerCount=0;
  prefs.referenceDbV=DEFAULT_REF_DBV;
}
bool calibrated() { return prefs.voltsPerCount > 0; }

struct Button {
  uint8_t pin;
  bool raw=false, down=false, held=false;
  unsigned long changed=0, pressed=0;
  // 押下で3、短押しを離すと1、長押し成立時に一度だけ2を返す。
  uint8_t read(unsigned long now) {
    bool state = digitalRead(pin) == LOW;
    if (state != raw) { raw=state; changed=now; }
    if (raw != down && now-changed >= 25) {
      down=raw;
      if (down) { pressed=now; held=false; return 3; }
      else if (!held) return 1;
    }
    if (down && !held && now-pressed >= 700) { held=true; return 2; }
    return 0;
  }
  explicit Button(uint8_t p):pin(p){}
};
Button mode(LevelSketch::MODE_PIN), speed(LevelSketch::SPEED_PIN), view(LevelSketch::VIEW_PIN);
size_t pageIndex=0;
LevelSketch::Frame frame{};
bool editing=false, relative=true, repaint=true, hadFrame=false, pendingAttack=false;
LevelSketch::Painter idlePainter=nullptr;
uint32_t idleTimeoutMs=LevelSketch::IDLE_AFTER_MS;
bool showingIdle=false;
bool idlePainterChanged=false;
bool manualIdle=false, manualIdleArmed=false, viewGestureEligible=false;
unsigned long lastControl=0;
int8_t editDb=DEFAULT_REF_DBV;
unsigned long lastActive=0, lastClip=0, lastDraw=0, lastLog=0;
uint16_t attackSeq=0;
bool clipSeen=false, haveAttack=false;
float slowRms=0;
float needlePower=0;
bool calibrating=false;
uint8_t collected=0;
uint16_t previousSeq=0, armSeq=0;
unsigned long armed=0, noticeAt=0;
float powerTotal=0, calibrationVolts=0;
const __FlashStringHelper* notice=nullptr;

void configure(const LevelSketch::Config& config) {
  idleTimeoutMs=config.idleTimeoutMs;
  idleActivityCounts=isfinite(config.idleThresholdCounts) && config.idleThresholdCounts>0 ?
      config.idleThresholdCounts : LevelSketch::IDLE_ACTIVITY_COUNTS;
  slowAlpha=responseAlpha(config.slowResponseMs);
  fastAlpha=responseAlpha(config.fastResponseMs);
  relative=config.startRelative;
  frame.fast=config.startFast;
}

void message(const __FlashStringHelper* s) {
  notice=s; noticeAt=millis(); repaint=true;
  Serial.print(F("# ")); Serial.println(s);
}
void cancelCalibration() { calibrating=false; collected=0; powerTotal=0; }
void startCalibration(unsigned long now) {
  calibrating=true; collected=0; powerTotal=0; armed=now;
  // 校正指示より前にキューへ入ったブロックを読み飛ばす。
  armSeq=previousSeq;
  ATOMIC_BLOCK(ATOMIC_RESTORESTATE) {
    if (head!=tail) armSeq=queue[(head+SLOTS-1)%SLOTS].seq;
  }
  message(F("CAL TONE"));
}
void calibrate(const Block& b, float power, bool clipped) {
  if (!calibrating || int16_t(b.seq-armSeq)<=0) return;
  if (clipped) {
    cancelCalibration(); message(F("CAL CLIPPED")); return;
  }
  if (collected && uint16_t(b.seq-previousSeq)!=1) {
    // このブロックから連続区間を取り直す。期限は最初の校正開始時点から数える。
    collected=0;
    powerTotal=0;
  }
  if (!collected && power < GATE*GATE) return;
  powerTotal+=power;
  if (++collected<CAL_BLOCKS) return;
  float counts=sqrt(powerTotal/CAL_BLOCKS);
  if (counts<GATE) message(F("TOO QUIET"));
  else {
    float scale=calibrationVolts/counts;
    if (scale<0.0035f || scale>0.0065f) message(F("CHECK mV"));
    else { prefs.voltsPerCount=scale; save(); message(F("CAL SAVED")); }
  }
  cancelCalibration();
}
void process(const Block& b, unsigned long now) {
  float mean=b.sum/float(BLOCK);
  float power=fmax(0,b.squares/float(BLOCK)-mean*mean);
  frame.rmsCounts=sqrt(power);
  frame.peakCounts=fmax(b.high-mean,mean-b.low);
  bool nearRail=b.low<=16 || b.high>=1007;
  if (nearRail) { lastClip=now; clipSeen=true; }
  calibrate(b,power,nearRail);
  // 欠落後は次に取得したブロックから平滑化を再開する。
  if (hadFrame && uint16_t(b.seq-previousSeq)!=1) needlePower=power;
  else needlePower+=(frame.fast ? fastAlpha : slowAlpha)*(power-needlePower);
  previousSeq=b.seq;
  if (frame.rmsCounts>=idleActivityCounts) {
    lastActive=now;
    if (manualIdleArmed) manualIdle=false;
  } else if (manualIdle) manualIdleArmed=true;
  if (frame.rmsCounts>=GATE) {
    if (frame.rmsCounts>fmax(GATE,slowRms*1.65f) &&
        (!haveAttack || uint16_t(b.seq-attackSeq)>=4)) {
      attackSeq=b.seq; haveAttack=true; pendingAttack=true;
      frame.attackPeakCounts=frame.peakCounts;
    }
  }
  if (haveAttack && uint16_t(b.seq-attackSeq)<6)
    frame.attackPeakCounts=fmax(frame.attackPeakCounts,frame.peakCounts);
  // 長い休止中の16ビット連番の周回に備え、先に追跡を終了する。
  if (haveAttack && uint16_t(b.seq-attackSeq)>=6) haveAttack=false;
  slowRms=slowRms*0.75f+frame.rmsCounts*0.25f;
  frame.clipped=clipSeen && now-lastClip<400;
  frame.absoluteDbV=calibrated() && frame.rmsCounts>=GATE ?
      20*log10(frame.rmsCounts*prefs.voltsPerCount) : NAN;
  frame.relativeDb=frame.absoluteDbV-prefs.referenceDbV;
  frame.idle=manualIdle || now-lastActive>=idleTimeoutMs;
  hadFrame=true;
}
bool dequeue(Block& b) {
  bool available=false;
  ATOMIC_BLOCK(ATOMIC_RESTORESTATE) {
    if (tail!=head) {
      const volatile Block& s=queue[tail];
      b.sum=s.sum; b.squares=s.squares; b.low=s.low; b.high=s.high; b.seq=s.seq;
      tail=(tail+1)%SLOTS; available=true;
    }
    frame.dropped=dropped;
  }
  return available;
}
void buttons(unsigned long now, size_t pageCount) {
  uint8_t m=mode.read(now), s=speed.read(now);
  uint8_t v=view.read(now);
  if(m || s || v) lastControl=now;
  if (manualIdle && (m || s || v==3 || v==1)) {
    manualIdle=false;
    frame.idle=now-lastActive>=idleTimeoutMs;
  }
  bool canView=!editing && !calibrating && m!=2;
  if (v==3) viewGestureEligible=canView;
  if (!canView) viewGestureEligible=false;
  // 短押しと待機への長押しを区別するため、離すまで待つ。
  if (v==1 && viewGestureEligible && pageCount>1) {
    if (++pageIndex>=pageCount) pageIndex=0;
    repaint=true;
  }
  if (v==2 && viewGestureEligible && idlePainter && !notice && !m && !s) {
    manualIdle=true;
    // 待機に入った時点の音を除き、無音後の次の音で復帰する。
    manualIdleArmed=frame.rmsCounts<idleActivityCounts;
    frame.idle=true;
  }
  if (v==1 || v==2) viewGestureEligible=false;
  if (m==2) {
    cancelCalibration();
    if (editing) {
      prefs.referenceDbV=editDb;
      save(); editing=false; relative=true; message(F("REF SAVED"));
    } else if (calibrated()) {
      editDb=int8_t(prefs.referenceDbV);
      editing=true; repaint=true;
    } else message(F("CAL FIRST"));
  } else if (editing) {
    if (m==1 && editDb>-36) { --editDb; repaint=true; }
    if (s==3 && editDb<0) { ++editDb; repaint=true; }
  } else {
    if (m==1) { relative=!relative; repaint=true; }
    if (s==3) { frame.fast=!frame.fast; repaint=true; }
  }
}
void serialCommands(unsigned long now) {
  static char line[20]; static uint8_t len=0; static bool invalid=false;
  while (Serial.available()) {
    char ch=Serial.read();
    if (ch=='\r') continue;
    if (ch!='\n') {
      if (len<sizeof(line)-1) line[len++]=ch; else invalid=true;
      continue;
    }
    line[len]='\0';
    if (!invalid && strncmp(line,"CAL ",4)==0) {
      char* end; long mv=strtol(line+4,&end,10);
      if (*end=='\0' && mv>=50 && mv<=1000 && !editing) {
        calibrationVolts=mv/1000.0f; startCalibration(now);
      } else Serial.println(F("# CAL 50..1000 mVrms; exit edit mode"));
    } else if (len || invalid) Serial.println(F("# command: CAL 100"));
    len=0; invalid=false;
  }
}
void telemetry(unsigned long now) {
  if (now-lastLog<150 || Serial.availableForWrite()<60) return;
  lastLog=now;
  Serial.print(F("M,")); Serial.print(now); Serial.print(',');
  Serial.print(frame.rmsCounts,1); Serial.print(','); Serial.print(frame.peakCounts,1);
  Serial.print(','); Serial.print(frame.absoluteDbV,1); Serial.print(',');
  Serial.print(frame.relativeDb,1); Serial.print(','); Serial.print(frame.clipped);
  Serial.print(','); Serial.println(frame.dropped);
}

void header(bool force) {
  using namespace LevelSketch;
  static int lastValue=32767;
  static char lastDigits[6]={};
  static uint8_t lastStatus=255;
  float db=relative ? frame.relativeDb : frame.absoluteDbV;
  int value=editing ? editDb*10 : (isfinite(db) ? int(round(db*10)) : -32768);
  uint8_t status=editing?1:calibrating?(collected?6:2):frame.clipped?3:notice?4:0;
  if (force) {
    display.fillRect(8,8,220,20,INK);
    display.setTextColor(MINT,INK); display.setTextSize(2); display.setCursor(10,10);
    if (editing) display.print(F("SET REF / dBV"));
    else {
      display.print(relative ? F("REL / dB ") : F("ABS / dBV "));
      display.print(frame.fast ? F("FAST") : F("SLOW"));
    }
  }
  if (force || value!=lastValue) {
    lastValue=value;
    char digits[8];
    char* end=digits;
    if (value==-32768) { *end++='-'; *end++='-'; }
    else {
      if (value<0) *end++='-';
      else if (value>0) *end++='+';
      unsigned int magnitude=value<0 ? -value : value;
      utoa(magnitude/10,end,10);
      end+=strlen(end);
      *end++='.'; *end++='0'+magnitude%10;
    }
    while (end<digits+6) *end++=' ';
    // 内蔵フォントは背景も上書きするため、変化した文字だけを更新する。
    // 桁数が減ったときは末尾を空白で埋める。
    for (uint8_t i=0; i<6; ++i) {
      if (force || digits[i]!=lastDigits[i]) {
        display.drawChar(10+24*i,39,digits[i],PAPER,INK,4);
        lastDigits[i]=digits[i];
      }
    }
  }
  if (!force && status==lastStatus) return;
  lastStatus=status;
  display.fillRect(8,89,224,18,INK);
  display.setTextSize(2); display.setTextColor(AMBER,INK); display.setCursor(10,90);
  if (editing) display.print(F("MODE -   SPEED +"));
  else if (calibrating) display.print(collected ? F("CAL MEASURE") : F("CAL TONE"));
  else if (frame.clipped) display.print(F("INPUT LIMIT"));
  else if (notice) display.print(notice);
  else if (!calibrated()) display.print(F("CAL FIRST"));
}
} // namespace

ISR(ADC_vect) {
  static uint32_t sum=0,squares=0;
  static uint16_t low=1023,high=0,seq=0;
  static uint8_t n=0;
  uint16_t x=ADC;
  sum+=x; squares+=uint32_t(x)*x;
  if (x<low) low=x;
  if (x>high) high=x;
  if (++n) return;
  uint8_t next=(head+1)%SLOTS;
  if (next==tail) { tail=(tail+1)%SLOTS; ++dropped; }
  volatile Block& b=queue[head];
  b.sum=sum; b.squares=squares; b.low=low; b.high=high; b.seq=++seq;
  head=next; sum=squares=0; low=1023; high=0;
}

namespace LevelSketch {
Adafruit_ILI9341 display(LCD_CS,LCD_DC,LCD_RESET);
void begin() {
  const Config defaults;
  begin(defaults);
}
void begin(const Config& config) {
  configure(config);
  Serial.begin(115200); pinMode(MODE_PIN,INPUT_PULLUP); pinMode(SPEED_PIN,INPUT_PULLUP);
  pinMode(VIEW_PIN,INPUT_PULLUP);
  loadPreferences();
  display.begin(4000000); display.setRotation(0); display.fillScreen(INK);
  display.setTextWrap(false);
  ADMUX=_BV(REFS0);                       // 基準電圧はAVcc、入力はADC0。
  ADCSRB=0; DIDR0 |= _BV(ADC0D);
  ADCSRA=_BV(ADEN)|_BV(ADATE)|_BV(ADIE)|_BV(ADIF)|_BV(ADPS2)|_BV(ADPS1)|_BV(ADPS0);
  ADCSRA |= _BV(ADSC);
  lastActive=millis();
  Serial.println(F("# M,ms,rms_counts,peak_counts,abs_dBV,rel_dB,limit,dropped"));
  Serial.println(F("# Level Sketch 0.5.1"));
}
void setIdlePainter(Painter paint) {
  if(idlePainter!=paint) idlePainterChanged=true;
  idlePainter=paint;
  if(!paint) manualIdle=false;
}
void setIdleTimeoutMs(uint32_t timeoutMs) { idleTimeoutMs=timeoutMs; }
void update(Painter paint) { updatePages(&paint,1); }
void updatePages(const Painter* pages, size_t count) {
  unsigned long now=millis();
  Block block;
  while (dequeue(block)) process(block,now);
  serialCommands(now); buttons(now,count);
  if (calibrating && now-armed>6000) { cancelCalibration(); message(F("CAL TIMEOUT")); }
  if (notice && now-noticeAt>1600) { notice=nullptr; repaint=true; }
  if (editing || calibrating || notice) manualIdle=false;
  if (!hadFrame) return;
  frame.timeMs=now; frame.relative=relative; telemetry(now);
  if (now-lastDraw<90) return;
  lastDraw=now;
  bool wantsIdle=idlePainter && !editing && !calibrating && !notice &&
                 (manualIdle || (frame.idle && now-lastControl>=idleTimeoutMs));
  if(wantsIdle) {
    bool entered=!showingIdle || idlePainterChanged;
    idlePainterChanged=false;
    showingIdle=true;
    if(entered) Serial.println(F("# IDLE"));
    idlePainter(frame,entered);
    repaint=true;
    return;
  }
  if(showingIdle) {
    showingIdle=false;
    Serial.println(F("# METER"));
    display.fillScreen(INK);
    repaint=true;
    if(calibrating) header(true);
  }
  if (calibrating) {
    // 全面の再描画中にADCキューが満杯になるため、ここでは見出しだけを更新する。
    header(false);
    return;
  }
  frame.needleDb=calibrated() && needlePower>=GATE*GATE ?
      10*log10(needlePower*prefs.voltsPerCount*prefs.voltsPerCount) : NAN;
  if (relative) frame.needleDb-=prefs.referenceDbV;
  static bool prevIdle=false;
  bool entered=repaint || prevIdle!=frame.idle;
  frame.attack=pendingAttack; pendingAttack=false;
  header(repaint);
  if (entered) display.fillRect(0,VIEW_TOP,240,320-VIEW_TOP,INK);
  if (pageIndex>=count) pageIndex=0;
  if (count) pages[pageIndex](frame,entered);
  prevIdle=frame.idle; repaint=false;
}
}
