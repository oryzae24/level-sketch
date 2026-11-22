#pragma once
#include <LevelSketch.h>

// このスケッチの作品一覧。作品を追加するときはここにも宣言します。
namespace SketchViews {
void paintNeedle(const LevelSketch::Frame& frame, bool entered);
void paintAmpCat(const LevelSketch::Frame& frame, bool entered);
void paintRooftopPatrol(const LevelSketch::Frame& frame, bool entered);
}
