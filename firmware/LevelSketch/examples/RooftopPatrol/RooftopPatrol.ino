#include "SketchViews.h"

// 屋上の絵と動きだけを確認するための全画面プレビュー。
void setup() {
  LevelSketch::display.begin(4000000);
  LevelSketch::display.setRotation(0);
}

void loop() {
  static bool firstFrame = true;
  static uint32_t lastDraw = 0;
  uint32_t now = millis();
  if (!firstFrame && now - lastDraw < 90) return;
  lastDraw = now;

  LevelSketch::Frame frame{};
  frame.timeMs = now;
  frame.idle = true;
  SketchViews::paintRooftopPatrol(frame, firstFrame);
  firstFrame = false;
}