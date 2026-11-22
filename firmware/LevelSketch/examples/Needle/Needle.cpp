#include "SketchViews.h"
using namespace LevelSketch;
using namespace SketchViews;

void SketchViews::paintNeedle(const Frame& f, bool entered) {
  static int16_t oldX=-1, oldY=0;
  static uint16_t oldColor=PAPER;
  int16_t nextX=-1, nextY=0;
  uint16_t color=f.clipped ? AMBER : PAPER;
  float db=f.needleDb;
  if (isfinite(db)) {
    float position=f.relative ? db/12.0f : (db+18)/18.0f;
    float angle=constrain(position,-1.0f,1.0f)*PI/3;
    nextX=120+sin(angle)*90; nextY=276-cos(angle)*90;
  }
  if (entered) {
    oldX=-1;
    // 針の長さは90画素で、半径95〜104の目盛りに重ならない。
    // 固定の目盛りは画面に入るときとリセット時だけ描く。
    for (uint8_t i=0; i<7; ++i) {
      float a=(int(i)-3)*PI/9;
      display.drawLine(120+sin(a)*95,276-cos(a)*95,120+sin(a)*104,276-cos(a)*104,
                       i==3 ? PAPER : MINT);
    }
    display.setTextSize(2); display.setTextColor(MINT,INK); display.setCursor(22,295);
    display.print(f.relative ? F("-12    0    +12") : F("-36   -18     0"));
  }
  bool moved=nextX!=oldX || (nextX>=0 && nextY!=oldY);
  if (!entered && !moved && (nextX<0 || color==oldColor)) return;
  // 座標を先に求め、1回のSPI通信で消去と描画を続けて行う。
  // 色だけが変わるときは針を同じ位置に上書きする。
  display.startWrite();
  if (moved && oldX>=0) display.writeLine(120,276,oldX,oldY,INK);
  if (nextX>=0) display.writeLine(120,276,nextX,nextY,color);
  display.endWrite();
  display.fillCircle(120,276,4,MINT);
  oldX=nextX; oldY=nextY; oldColor=color;
}
