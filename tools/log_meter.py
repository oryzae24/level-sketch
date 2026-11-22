"""Save the Level Sketch USB telemetry. Install pyserial and close the IDE serial monitor."""
import argparse
import csv
from pathlib import Path

import serial

parser = argparse.ArgumentParser(description="Level SketchのUSBログをCSVに記録する（Ctrl+Cで終了）")
parser.add_argument("port", help="例 COM4")
parser.add_argument("output", type=Path, help="新規CSVファイル")
args = parser.parse_args()
with serial.Serial(args.port, 115200, timeout=1) as device, args.output.open("x", newline="", encoding="utf-8") as out:
    writer = csv.writer(out)
    writer.writerow(["ms", "rms_counts", "peak_counts", "abs_dBV", "rel_dB", "limit", "dropped"])
    try:
        while True:
            fields = device.readline().decode("ascii", errors="replace").strip().split(",")
            if len(fields) == 8 and fields[0] == "M":
                writer.writerow(fields[1:])
                out.flush()
    except KeyboardInterrupt:
        pass
