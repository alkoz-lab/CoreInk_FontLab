#!/usr/bin/env python3
"""Persistent capture of a full-font-set character dump streamed by CaptureFontsAutomated.ino.

Usage:
    python capture_screenshots.py COM5 [--baud 115200] [--out-dir captures]

Start this script first, then press the CoreInk's top button. The device will
automatically walk every built-in font, streaming one BMP per filled page. This
script saves each BMP, sends an ACK so the device can continue, and prints
progress as files arrive. It exits automatically when the device signals
ALL_DONE (after the last page of the last font).
"""
import argparse
import re
import sys
import time
from pathlib import Path

import serial

BEGIN_MARKER = b"BEGIN_BMP"
END_MARKER = b"END_BMP"
NAME_PREFIX = b"NAME:"
DONE_MARKER = b"ALL_DONE"
BMP_HEADER_SIZE = 54


def read_line(ser: serial.Serial) -> bytes:
    return ser.readline().strip()


def sanitize_filename(name: str) -> str:
    name = name.strip()
    name = name.removeprefix("NAME:")
    name = name.replace("\\", "/")
    cleaned = re.sub(r"[^A-Za-z0-9._/-]+", "_", name)
    cleaned = cleaned.replace("/", "_")
    cleaned = cleaned.strip("_")
    if not cleaned:
        cleaned = "screenshot"
    if not cleaned.lower().endswith(".bmp"):
        cleaned += ".bmp"
    return cleaned


def read_exact(ser: serial.Serial, size: int) -> bytes:
    data = bytearray()
    while len(data) < size:
        chunk = ser.read(size - len(data))
        if not chunk:
            raise TimeoutError(f"Timed out reading {size - len(data)} more bytes")
        data.extend(chunk)
    return bytes(data)


def bmp_data_size(header: bytes) -> int:
    file_size = int.from_bytes(header[2:6], "little")
    return file_size - BMP_HEADER_SIZE


def capture_one_page(ser: serial.Serial, name: str, out_dir: Path) -> Path:
    header = read_exact(ser, BMP_HEADER_SIZE)
    data_size = bmp_data_size(header)
    pixel_data = read_exact(ser, data_size)

    line = read_line(ser)
    while line != END_MARKER:
        if line:
            print(line.decode(errors="replace"))
        line = read_line(ser)

    out_path = out_dir / f"{sanitize_filename(name)}"
    with open(out_path, "wb") as f:
        f.write(header)
        f.write(pixel_data)
    return out_path


def open_serial(port: str, baud: int) -> serial.Serial:
    ser = serial.Serial(port, baud, timeout=20, dsrdtr=False, rtscts=False)
    ser.dtr = False
    ser.rts = False
    time.sleep(0.25)
    ser.reset_input_buffer()
    ser.reset_output_buffer()
    return ser


def run(port: str, baud: int, out_dir: Path) -> None:
    out_dir.mkdir(parents=True, exist_ok=True)
    file_count = 0

    with open_serial(port, baud) as ser:
        ser.write(b"READY\n")
        print(f"Listening on {port} @ {baud}. Ready to start capture. Waiting for device...")
        pending_name = None

        while True:
            line = read_line(ser)
            if not line:
                continue

            if line.startswith(NAME_PREFIX):
                pending_name = line[len(NAME_PREFIX):].decode(errors="replace").strip()
                continue

            if line == BEGIN_MARKER:
                name = pending_name or f"screenshot_{file_count + 1}.bmp"
                pending_name = None
                out_path = capture_one_page(ser, name, out_dir)
                size = out_path.stat().st_size
                file_count += 1
                print(f"Saved {out_path} ({size} bytes) [total: {file_count} file(s)]")
                ser.write(b"ACK\n")
                continue

            if line == DONE_MARKER:
                print(f"Capture complete. {file_count} file(s) saved to {out_dir}/")
                return

            print(line.decode(errors="replace"))


def main() -> int:
    parser = argparse.ArgumentParser(description=__doc__)
    parser.add_argument("port", help="Serial port, e.g. COM5")
    parser.add_argument("--baud", type=int, default=115200)
    parser.add_argument("--out-dir", default="captures", help="Directory to save BMP files into")
    args = parser.parse_args()

    try:
        run(args.port, args.baud, Path(args.out_dir))
    except (TimeoutError, serial.SerialException) as exc:
        print(f"Error: {exc}", file=sys.stderr)
        return 1
    except KeyboardInterrupt:
        print("\nInterrupted.")
        return 1
    return 0


if __name__ == "__main__":
    sys.exit(main())
