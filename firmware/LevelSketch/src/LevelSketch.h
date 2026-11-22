#pragma once
#include <Arduino.h>
#include <Adafruit_ILI9341.h>

namespace LevelSketch {
constexpr uint8_t MODE_PIN = 2, SPEED_PIN = 3, VIEW_PIN = 4;
constexpr uint8_t LCD_CS = 10, LCD_DC = 9, LCD_RESET = 8;
constexpr uint16_t PAPER = 0xCFAA, INK = 0x0924, MINT = 0x8E99, AMBER = 0xFBE9;
constexpr int16_t VIEW_TOP = 118;  // 縦画面のy=118〜319がスケッチの描画領域。
constexpr uint32_t IDLE_AFTER_MS = 60000UL; // Config::idleTimeoutMs の初期値。
constexpr float IDLE_ACTIVITY_COUNTS = 5.0f; // 待機判定専用のしきい値。

// スケッチ側の設定。校正値と保存した基準レベルはEEPROMで管理する。
struct Config {
  uint32_t idleTimeoutMs = IDLE_AFTER_MS;
  float idleThresholdCounts = IDLE_ACTIVITY_COUNTS;
  uint16_t slowResponseMs = 300;
  uint16_t fastResponseMs = 60;
  bool startRelative = true;
  bool startFast = false;
};

struct Frame {
  float rmsCounts, peakCounts;       // 直近256サンプルから直流成分を除いた値。
  float attackPeakCounts;            // 直近の立ち上がりから最初の6ブロックの最大値。
  float absoluteDbV, relativeDb;     // 平滑化前の値。未校正時またはしきい値未満ではNAN。
  float needleDb;                   // 電力で平滑化し、選択中の表示単位に換算した値。
  bool fast;                        // 設定されたSLOW／FASTの時定数を選ぶ。
  bool relative, clipped, idle;      // VIEW長押しによる待機中もidleになる。
  bool attack;                      // 前回の描画後に1回以上立ち上がりを検出した。
  uint16_t dropped;                 // 起動後に上書きされたADCブロック数。
  unsigned long timeMs;
};

extern Adafruit_ILI9341 display;
typedef void (*Painter)(const Frame& frame, bool entered);
void begin();
void begin(const Config& config);
// 全画面の待機作品を登録する。nullptrで無効。待ち時間を挟まず呼び出す。
void setIdlePainter(Painter paint);
// 最後の音・ボタン操作から待機作品を表示するまでの時間（ミリ秒）。
// setup()で指定する。uint32_tなので65秒を超える値も扱える。
void setIdleTimeoutMs(uint32_t timeoutMs);
void update(Painter paint);
void updatePages(const Painter* pages, size_t count);
template<size_t N>
void update(const Painter (&pages)[N]) { updatePages(pages, N); }
}
