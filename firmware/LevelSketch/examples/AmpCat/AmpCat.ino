#include <LevelSketch.h>
#include "SketchViews.h"
using namespace LevelSketch;
using namespace SketchViews;

// 共通設定。猫の感度・絵柄・動きは各作品の.cppで編集します。
constexpr uint32_t IDLE_TIMEOUT_MS = 60UL * 1000UL;
constexpr float IDLE_THRESHOLD_COUNTS = 5.0f;
constexpr uint16_t SLOW_RESPONSE_MS = 300;
constexpr uint16_t FAST_RESPONSE_MS = 60;
constexpr bool START_IN_RELATIVE = true;
constexpr bool START_IN_FAST = false;

void setup() {
  Config config;
  config.idleTimeoutMs = IDLE_TIMEOUT_MS;
  config.idleThresholdCounts = IDLE_THRESHOLD_COUNTS;
  config.slowResponseMs = SLOW_RESPONSE_MS;
  config.fastResponseMs = FAST_RESPONSE_MS;
  config.startRelative = START_IN_RELATIVE;
  config.startFast = START_IN_FAST;
  LevelSketch::begin(config);
  // この作例では猫メーターを表示し続けます。
}
void loop() { LevelSketch::update(paintAmpCat); }
