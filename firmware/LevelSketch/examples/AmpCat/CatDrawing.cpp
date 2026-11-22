#include "CatDrawing.h"
using namespace LevelSketch;
namespace {
uint8_t pixel(const uint8_t* data,uint8_t w,uint8_t h,const SketchViews::CatPose& pose,int x,int y) {
  if(pose.frame==255 || x<pose.x || y<pose.y || x>=pose.x+2*w || y>=pose.y+2*h) return 0;
  uint8_t col=(x-pose.x)/2, row=(y-pose.y)/2;
  if(pose.mirrored) col=w-1-col;
  uint16_t index=uint16_t(row)*w+col;
  uint8_t packed=pgm_read_byte(data+uint16_t(pose.frame)*(uint16_t(w)*h/4)+index/4);
  return (packed>>(6-2*(col&3)))&3;
}
}
void SketchViews::drawCatChanged(const uint8_t* images,uint8_t w,uint8_t h,
                                  const CatPose& next,const CatPose& previous,uint16_t background) {
  if(next.x==previous.x && next.y==previous.y && next.frame==previous.frame && next.mirrored==previous.mirrored) return;
  bool oldValid=previous.frame!=255;
  int left=oldValid?min(next.x,previous.x):next.x;
  int top=oldValid?min(next.y,previous.y):next.y;
  int right=(oldValid?max(next.x,previous.x):next.x)+2*w;
  int bottom=(oldValid?max(next.y,previous.y):next.y)+2*h;
  display.startWrite();
  for(int y=top;y<bottom;y+=2) {
    int run=-1; uint8_t colour=0;
    for(int x=left;x<=right;x+=2) {
      uint8_t before=x<right?pixel(images,w,h,previous,x,y):0;
      uint8_t after=x<right?pixel(images,w,h,next,x,y):0;
      bool changed=x<right && before!=after;
      if(run>=0 && (!changed || after!=colour)) {
        uint16_t rgb=colour?pgm_read_word(&BLACK_CAT_PALETTE[colour]):background;
        display.writeFillRect(run,y,x-run,2,rgb);run=-1;
      }
      if(changed && run<0){run=x;colour=after;}
    }
  }
  display.endWrite();
}
