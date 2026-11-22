#include "SketchViews.h"
using namespace LevelSketch;
using namespace SketchViews;

void SketchViews::paintPickHistory(const Frame& f, bool entered) {
  static float peaks[8]={};
  static uint8_t count=0;
  static unsigned long started=0;
  bool redraw=entered;
  if (f.attack) {
    memmove(peaks,peaks+1,7*sizeof(float));
    peaks[7]=f.attackPeakCounts;
    if (count<8) ++count;
    started=f.timeMs; redraw=true;
  } else if (count && f.timeMs-started<180 && f.attackPeakCounts>peaks[7]) {
    peaks[7]=f.attackPeakCounts; redraw=true;
  }
  if (!redraw) return;
  display.setTextSize(2); display.setTextColor(MINT,INK); display.setCursor(10,133);
  display.print(F("LAST 8 ATTACKS"));
  for (uint8_t i=0; i<8; ++i) {
    int h=int(124*sqrt(constrain(peaks[i]/510.0f,0.0f,1.0f)));
    int x=12+27*i;
    display.fillRect(x,166,17,126,INK);
    if (h>0) display.fillRect(x,292-h,17,h,i==7 ? PAPER : MINT);
  }
  display.drawFastHLine(10,293,219,MINT);
}
