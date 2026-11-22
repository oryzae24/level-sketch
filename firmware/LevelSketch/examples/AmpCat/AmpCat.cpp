#include "SketchViews.h"
#include "CatDrawing.h"
using namespace LevelSketch;
using namespace SketchViews;

// 猫メーターの作品コード。160.0f が上限感度、100 が最大の上昇量です。
// 測定値は変更せず、受け取った f.rmsCounts を好きな動きに変換します。
void SketchViews::paintAmpCat(const Frame& f, bool entered) {
  static CatPose previous={0,0,255,false};
  int rise=f.idle ? 0 : int(100*sqrt(constrain(f.rmsCounts/160.0f,0.0f,1.0f)));
  CatPose next={88,int16_t((224-rise)&~1),uint8_t(f.idle || f.timeMs%3100<170),false};
  if (entered) {
    previous.frame=255;
    display.drawFastHLine(42,282,156,PAPER);
    display.setTextSize(2); display.setTextColor(PAPER,INK); display.setCursor(48,295);
    display.print(F("PLAY TO LIFT"));
  }
  drawCatChanged(&BLACK_CAT_FACE[0][0],CAT_FACE_W,CAT_FACE_H,next,previous,INK);
  previous=next;
}
