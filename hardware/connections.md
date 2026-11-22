# 接続表 · Level Sketch

Fritzingプロジェクトから生成。830穴ボードの数字は列、A–EとF–Jは溝を挟む別の接点群。レールWは5 V、XとYはGND、Zは3.3 V。実物の赤青表示・途中の分断は導通で確認する。DIPの切り欠きは列番号の小さい側。ジャックとスイッチは線で引き出す。

J3は外径5.5／内径2.1 mm、安定化DC 9 V・センターマイナス。中心→GND、外側→D1のA、D1の帯側K→UNOのVIN。J3は使用接点の機能名で示す。USBはUNOのUSB-B端子を使う。

## ブレッドボードの挿入位置

| 部品 | 1番ピンから順に挿す穴 |
|---|---|
| R1  100k | 5J · 9J |
| C1  100nF | 9I · 11I |
| R2  10M | 11J · 15J |
| R3  10k | 24A · 28A |
| R4  10k | 28B · 32B |
| R5  3.3k | 19I · 23I |
| C2  100uF | 28C · 30C |
| C3  100nF | 16H · 18H |
| C4  15nF | 23J · 25J |
| MCP6002-E/P | 18E · 19E · 20E · 21E · 21F · 20F · 19F · 18F |
| FXMA108 / AE-LLCNV8-DIP20 | 36E · 37E · 38E · 39E · 40E · 41E · 42E · 43E · 44E · 45E · 45F · 44F · 43F · 42F · 41F · 40F · 39F · 38F · 37F · 36F |
| D1  1N5819 | 51J · 55J |

## ジャンパー配線

| ネット | 始点 | 終点 |
|---|---|---|
| 5V | UNO.5V | 4W |
| GND | UNO.GND | 6X |
| 3V3 | UNO.3V3 | 9Z |
| GND | 3X | 3Y |
| PEDAL_9V | J3 OUTER + | 51I |
| GND | J3 CENTER − | 58Y |
| VIN | 55H | UNO.VIN |
| TIP | J1.1 (TIP) | J2.1 (TIP) |
| GND | J1.2 (SLEEVE) | J2.2 (SLEEVE) |
| TIP | J1.1 (TIP) | 5H |
| GND | J2.2 (SLEEVE) | 5X |
| SENSE | 11H | 21G |
| MID_BUF | 15I | 18C |
| MID_BUF | 18D | 19D |
| 5V | 24D | 25W |
| GND | 32D | 31X |
| GND | 30D | 30X |
| DIV | 28D | 20C |
| GND | 16G | 16Y |
| 5V | 18I | 18W |
| GND | 21D | 21X |
| BUF | 19G | 20G |
| GND | 25I | 25Y |
| 3V3 | 36D | 36Z |
| 5V | 36G | 36W |
| GND | 45D | 45X |
| GND | 45G | 45Y |
| GND | 42G | 42Y |
| GND | 43G | 43Y |
| GND | 44G | 43J |
| A0 | 23H | UNO.A0 |
| CS_5V | UNO.D10 | 37J |
| CS_3V3 | 37A | LCD.3 (CS) |
| RST_5V | UNO.D8 | 38J |
| RST_3V3 | 38A | LCD.4 (RESET) |
| DC_5V | UNO.D9 | 39J |
| DC_3V3 | 39A | LCD.5 (DC) |
| MOSI_5V | UNO.D11 | 40J |
| MOSI_3V3 | 40A | LCD.6 (SDI) |
| SCK_5V | UNO.D13 | 41J |
| SCK_3V3 | 41A | LCD.7 (SCK) |
| 5V | 61W | LCD.1 (VCC) |
| GND | 61X | LCD.2 (GND) |
| 3V3 | 61Z | LCD.8 (LED) |
| 3V3 | 60Z | LCD.11 (T_CS) |
| MODE | UNO.D2 | S1.1 (NO) |
| GND | S1.2 (COM) | 52X |
| SPEED | UNO.D3 | S2.1 (NO) |
| GND | S2.2 (COM) | 53X |
| VIEW | UNO.D4 | S3.1 (NO) |
| GND | S3.2 (COM) | 54X |

## 回路図の同電位グループ

| ネット | 全端子（部品.ピン番号） |
|---|---|
| 5V | UNO.5V · U1.8 (VDD) · U2.20 (VB) · R3.1 (1) · C3.2 (2) · LCD.1 (VCC) |
| PEDAL_9V | J3 OUTER + · D1.1 (A) |
| VIN | D1.2 (K / band) · UNO.VIN |
| GND | UNO.GND · J3 CENTER − · J1.2 (SLEEVE) · J2.2 (SLEEVE) · U1.4 (VSS) · U2.10 (GND) · U2.11 (/OE) · U2.12 (B7) · U2.13 (B6) · U2.14 (B5) · R4.2 (2) · C2.2 (−) · C3.1 (1) · C4.2 (2) · LCD.2 (GND) · S1.2 (COM) · S2.2 (COM) · S3.2 (COM) |
| 3V3 | UNO.3V3 · U2.1 (VA) · LCD.8 (LED) · LCD.11 (T_CS) |
| TIP | J1.1 (TIP) · J2.1 (TIP) · R1.1 (1) |
| LIMIT | R1.2 (2) · C1.1 (1) |
| SENSE | C1.2 (2) · R2.1 (1) · U1.5 (+IN B) |
| MID_BUF | R2.2 (2) · U1.1 (OUT A) · U1.2 (−IN A) |
| DIV | R3.2 (2) · R4.1 (1) · C2.1 (+) · U1.3 (+IN A) |
| BUF | U1.6 (−IN B) · U1.7 (OUT B) · R5.1 (1) |
| A0 | R5.2 (2) · C4.1 (1) · UNO.A0 |
| CS_5V | UNO.D10 · U2.19 (B0) |
| CS_3V3 | U2.2 (A0) · LCD.3 (CS) |
| RST_5V | UNO.D8 · U2.18 (B1) |
| RST_3V3 | U2.3 (A1) · LCD.4 (RESET) |
| DC_5V | UNO.D9 · U2.17 (B2) |
| DC_3V3 | U2.4 (A2) · LCD.5 (DC) |
| MOSI_5V | UNO.D11 · U2.16 (B3) |
| MOSI_3V3 | U2.5 (A3) · LCD.6 (SDI) |
| SCK_5V | UNO.D13 · U2.15 (B4) |
| SCK_3V3 | U2.6 (A4) · LCD.7 (SCK) |
| MODE | UNO.D2 · S1.1 (NO) |
| SPEED | UNO.D3 · S2.1 (NO) |
| VIEW | UNO.D4 · S3.1 (NO) |

LCD 9・10・12～14番とSDスロットは未接続。U2のA5～A7は未接続、対応するB5～B7はGND。タクトの四足は内部の二端子をNO/COMとして表記。非押下で開放、押下で導通する二端子を使う。

Fritzing標準のUNO・ブレッドボード図形 © Fritzing contributors, CC BY-SA 3.0。この配線図とプロジェクトを同じ条件で提供する。https://creativecommons.org/licenses/by-sa/3.0/
