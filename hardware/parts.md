# 部品表 · 机上試作1台

2026-11-10に秋月電子通商の通販ページを確認した税込価格。袋売りの部品は最小販売単位で計算した。ギター、ケーブル、UNO R3、USBケーブル、830穴ブレッドボード、ジャンパー線、ペダル用安定化9 V DC電源（センターマイナス、1出力250 mA以上を推奨）は手持ち想定。はんだ、工具、送料とケースは別枠。

| 部品 | 試作に使う数 | 購入単位と金額 | 秋月販売コード |
|---|---:|---:|---|
| LCD　MSP2807／ILI9341。2.8型、320×240、SPI | 1 | 1枚 1,450円 | [116265](https://akizukidenshi.com/catalog/g/g116265/) |
| U2　AE-LLCNV8-DIP20／FXMA108 | 1 | 1組 250円 | [117062](https://akizukidenshi.com/catalog/g/g117062/) |
| J1・J2　MJ-185LP、6.3 mmモノラル・絶縁型 | 2 | 180円 × 2 = 360円 | [108694](https://akizukidenshi.com/catalog/g/g108694/) |
| U1　MCP6002-E/P、DIP-8 | 1 | 1個 80円 | [130011](https://akizukidenshi.com/catalog/g/g130011/) |
| R1　100 kΩ、測定枝の直列抵抗 | 1 | 100本袋 100円 | [116104](https://akizukidenshi.com/catalog/g/g116104/) |
| R2　10 MΩ、入力バイアス | 1 | 100本袋 180円 | [114295](https://akizukidenshi.com/catalog/g/g114295/) |
| R3・R4　10 kΩ、中点用 | 2 | 100本袋 100円 | [116103](https://akizukidenshi.com/catalog/g/g116103/) |
| R5　3.3 kΩ、ADC前ローパス | 1 | 100本袋 170円 | [116332](https://akizukidenshi.com/catalog/g/g116332/) |
| C1・C3　100 nF、入力結合・U1電源バイパス | 2 | 20円 × 2 = 40円 | [114597](https://akizukidenshi.com/catalog/g/g114597/) |
| C2　100 µF、中点平滑。＋を分圧点、−をGND | 1 | 1個 10円 | [117877](https://akizukidenshi.com/catalog/g/g117877/) |
| C4　15 nF、ADC前ローパス | 1 | 1個 15円 | [114593](https://akizukidenshi.com/catalog/g/g114593/) |
| S1 MODE・S2 SPEED・S3 VIEW　手で押すモーメンタリータクト | 3 | 10円 × 3 = 30円 | [103653](https://akizukidenshi.com/catalog/g/g103653/) |
| J3　MJ-14、パネル絶縁DCジャック、外径5.5／内径2.1 mm | 1 | 1個 70円 | [106342](https://akizukidenshi.com/catalog/g/g106342/) |
| D1　1N5819、直列逆極性保護、40 V／1 A、帯側をVINへ | 1 | 10本入1袋 100円 | [117244](https://akizukidenshi.com/catalog/g/g117244/) |
| **9 V／USB対応の机上試作合計** | | **2,955円** | 確認時はいずれも通販在庫あり |

表示部1,700円、音声・計測・操作部1,085円、ペダル電源入力170円。J3のパネル穴はφ8.2 mm。外側が＋9 Vなので、取り付け後に外側接点と金属ケースの絶縁を確認する。机上用タクトは指で操作する。[J3の製品図](https://akizukidenshi.com/goodsaffix/MJ14_MJ15.pdf)

**ボードへの取り付け**は、ブレッドボードと液晶を固定し、MODE・SPEED・VIEWを指で押せる配置にする。筐体の内寸候補は幅100 × 奥行160 × 高さ50 mm以上。液晶基板50 × 86 mmとUNO約53 × 69 mm、左右のジャックの奥行、J3とUSBプラグの干渉を紙型で確認する。本体のセンタープラスDCジャックにはケースに開口を設けず、外から挿せるDC入力をJ3に統一する。
