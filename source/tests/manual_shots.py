#!/usr/bin/env python3
"""Every screen of the manual and the site, from the firmware built for the computer (tests/manual_shots.c):
   ../docs/img/NAME.png at 480 x 480. The working screens in RIBBON CLASSIC (orange), the themes in their own.
   python3 tests/manual_shots.py   (from source/)"""
import subprocess
from pathlib import Path
from PIL import Image

OUT = Path("../docs/img"); OUT.mkdir(parents=True, exist_ok=True)
RAW = Path("build/shots"); RAW.mkdir(parents=True, exist_ok=True)
EXE = Path("build/host/shots"); EXE.parent.mkdir(parents=True, exist_ok=True)
subprocess.run(["cc", "-O1", "-DKAWA_WASM", '-DAPP_VER="1.1"', "-Ifirmware/src", "-Ifirmware/gen", "-Iweb",
                "-w", "tests/manual_shots.c", "-o", str(EXE), "-lm"], check=True)
for f in RAW.glob("*.raw"):
    f.unlink()
subprocess.run([str(EXE)], check=True)
n = 0
for f in sorted(RAW.glob("*.raw")):
    b = f.read_bytes()
    im = Image.new("RGB", (240, 240))
    im.putdata([((v >> 11) * 255 // 31, ((v >> 5) & 63) * 255 // 63, (v & 31) * 255 // 31)
                for v in (b[i] | b[i + 1] << 8 for i in range(0, len(b), 2))])
    im.resize((480, 480), Image.NEAREST).save(OUT / f"{f.stem}.png", optimize=True)
    n += 1
print(n, "screens ->", OUT)
