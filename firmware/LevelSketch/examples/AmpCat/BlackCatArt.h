#pragma once
#include <Arduino.h>
#include <avr/pgmspace.h>
namespace SketchViews {
constexpr uint8_t CAT_FACE_W=32, CAT_FACE_H=28;
extern const uint8_t BLACK_CAT_FACE[2][224] PROGMEM;
extern const uint16_t BLACK_CAT_PALETTE[4] PROGMEM;
}
