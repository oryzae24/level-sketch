#include <LevelSketch.h>
#include "SketchViews.h"
using namespace LevelSketch;
using namespace SketchViews;

// ===== 共通設定：作品に合わせて、ここを変更します =====
constexpr uint32_t IDLE_TIMEOUT_MS = 60UL * 1000UL;
constexpr float IDLE_THRESHOLD_COUNTS = 5.0f; // 音ありと判断する大きさ（ADC RMS）
constexpr uint16_t SLOW_RESPONSE_MS = 300;
constexpr uint16_t FAST_RESPONSE_MS = 60;
constexpr bool START_IN_RELATIVE = true; // true: REL、false: ABS
constexpr bool START_IN_FAST = false;    // false: SLOW、true: FAST

// VIEW短押しで切り替える順番。長押し（約0.7秒）は待機作品へ。
// 描画は同じフォルダの各.cppで編集します。
const Painter pages[] = {paintNeedle, paintAmpCat};
const Painter idleArtwork = paintRooftopPatrol; // nullptr なら待機作品を使わない

void setup() {
  Config config;
  config.idleTimeoutMs = IDLE_TIMEOUT_MS;
  config.idleThresholdCounts = IDLE_THRESHOLD_COUNTS;
  config.slowResponseMs = SLOW_RESPONSE_MS;
  config.fastResponseMs = FAST_RESPONSE_MS;
  config.startRelative = START_IN_RELATIVE;
  config.startFast = START_IN_FAST;
  LevelSketch::begin(config);
  LevelSketch::setIdlePainter(idleArtwork);
}

void loop() { LevelSketch::update(pages); }
