#pragma once
#include <LevelSketch.h>
#include "BlackCatArt.h"
namespace SketchViews {
// 位置は偶数画素、画像は2倍で描く。フレーム255は非表示。
struct CatPose { int16_t x,y; uint8_t frame; bool mirrored; };
void drawCatChanged(const uint8_t* images, uint8_t width, uint8_t height,
                    const CatPose& next, const CatPose& previous, uint16_t background);
}
