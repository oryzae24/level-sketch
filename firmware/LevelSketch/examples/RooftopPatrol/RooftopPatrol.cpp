#include "SketchViews.h"
#include "CatDrawing.h"
using namespace LevelSketch;
using namespace SketchViews;
// 待機作品のコード。city() が背景、paintRooftopPatrol() が動きを担当します。
// このファイルを編集して、同じ測定・操作の仕組みで別の待機作品を作れます。
namespace {
constexpr uint16_t NIGHT=0x0864, WALL=0x1906, WALL2=0x2107, PINK=0xF418, BLUE=0x4CDD, PURPLE=0x939B;
const uint8_t BUILDINGS[][3] PROGMEM={{0,123,32},{35,97,34},{73,115,27},{103,79,42},{149,125,25},{178,93,36},{218,128,22}};
void city() {
  display.fillScreen(NIGHT);
  for(uint8_t i=0;i<12;++i)display.fillRect((uint16_t(i)*53+19)%226+7,45+(uint16_t(i)*29)%47,2,2,PURPLE);
  display.fillCircle(186,58,13,AMBER);display.fillCircle(191,53,12,NIGHT);
  for(uint8_t i=0;i<7;++i) {
    uint8_t x=pgm_read_byte(&BUILDINGS[i][0]),y=pgm_read_byte(&BUILDINGS[i][1]),w=pgm_read_byte(&BUILDINGS[i][2]);
    display.fillRect(x,y,w,166-y,i&1?WALL2:WALL);
    display.drawFastHLine(x,y,w,i&1?PURPLE:BLUE);
    for(uint8_t r=0;r<(160-y)/12;++r)for(uint8_t col=0;col<(w-5)/9;++col) {
      uint8_t id=i*19+r*3+col;
      uint16_t colour=id%7<3?(id%3==0?AMBER:id%3==1?PINK:MINT):WALL2;
      display.fillRect(x+5+col*9,y+7+r*12,3,5,colour);
    }
  }
  display.setTextWrap(false);display.setTextSize(1);
  display.fillRect(10,138,31,14,PINK);display.setTextColor(NIGHT,PINK);display.setCursor(14,141);display.print(F("LIVE"));
  display.fillRect(108,108,31,13,MINT);display.setTextColor(NIGHT,MINT);display.setCursor(116,111);display.print(F("ON"));
  // 猫の移動範囲（y=174〜237）は単色。
  display.fillRect(0,238,240,5,PURPLE);display.fillRect(0,243,240,13,WALL2);
  for(uint8_t i=0;i<10;++i)display.drawFastHLine(i*26,250,16,PURPLE);
  display.setTextColor(PAPER,NIGHT);display.setTextSize(2);
  display.setCursor(48,16);display.print(F("LEVEL SKETCH"));
  display.setCursor(30,276);display.print(F("ROOFTOP PATROL"));
  display.setTextSize(1);display.setTextColor(MINT,NIGHT);display.setCursor(84,305);display.print(F("CAT PATROL"));
}
}
void SketchViews::paintRooftopPatrol(const Frame& f,bool entered) {
  static unsigned long epoch=0;
  static uint16_t lastTwinkle=65535;
  static CatPose previous={0,0,255,false};
  if(entered){city();epoch=f.timeMs;lastTwinkle=65535;previous.frame=255;}
  unsigned long age=f.timeMs-epoch;
  uint16_t phase=age%10000;
  bool mirrored=(age/10000)&1;
  uint8_t offset=(uint32_t(min(phase,uint16_t(6000)))*108/6000)&~1;
  // 横向きに歩き、斜めから正面へ振り向き、再び横を向いて目を閉じて伸びる。
  uint8_t pose=phase<6000?(age/160)%4:phase<6300?6:phase<7300?4:
               phase<7600?6:phase<9000?5:phase<9300?6:4;
  CatPose next={int16_t(mirrored?126-offset:18+offset),174,pose,mirrored};
  drawCatChanged(&BLACK_CAT_WALK[0][0],CAT_WALK_W,CAT_WALK_H,next,previous,NIGHT);
  previous=next;
  uint16_t sparkle=age/600;
  if(sparkle!=lastTwinkle) {
    display.fillRect(27,51,2,2,sparkle&1?PINK:PURPLE);
    display.fillRect(157,43,2,2,sparkle&1?PURPLE:AMBER);
    display.fillRect(40,104,3,5,sparkle%3?AMBER:WALL2);
    lastTwinkle=sparkle;
  }
}
