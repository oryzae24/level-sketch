#pragma once
#include <Arduino.h>
#include <avr/pgmspace.h>
namespace SketchViews {
constexpr uint8_t CAT_WALK_W=48, CAT_WALK_H=32;
constexpr uint8_t CAT_WALK_FRAMES=7;
extern const uint8_t BLACK_CAT_WALK[CAT_WALK_FRAMES][384] PROGMEM;
extern const uint16_t BLACK_CAT_PALETTE[4] PROGMEM;
}
