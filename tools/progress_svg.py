#!/usr/bin/env python3
"""Draw docs/progress.svg: one square per translation unit, green when it is byte-matching C, grey when it is
still assembly. Game code (not split into files yet) is drawn as one square per 16 KB of .text.

Usage: python3 tools/progress_svg.py   (run from the repo root after `python3 configure.py`)
A unit counts as matched when configure.py marks it Matching: those are only committed while
build/GITE01/main.dol still verifies byte-identical (SHA-1 in config/GITE01/build.sha1)."""
import os
import re
import html

ROOT = os.path.dirname(os.path.dirname(os.path.abspath(__file__)))
GAME_TEXT = (0x80005800, 0x802E4858)  # .text before the first library (MetroTRK); zlib inside is subtracted
GAME_CHUNK = 0x4000

GROUPS = [  # (label, path prefix)
    ("Dolphin SDK", "dolphin/"),
    ("MusyX", "musyx/"),
    ("zlib 1.1.4", "zlib/"),
    ("MSL C", "MSL_C/"),
    ("Runtime", "Runtime.PPCEABI.H/"),
    ("MetroTRK", "TRK_MINNOW_DOLPHIN/"),
]

GREEN, GREY, BG, FG, MUTED = "#2ea043", "#30363d", "#0d1117", "#ffffff", "#8b949e"
W, COLS, CELL, GAP = 1000, 48, 18, 2


def units():
    cfg = open(os.path.join(ROOT, "configure.py")).read()
    return [(m[2], m[1] == "Matching") for m in re.finditer(r'Object\((Matching|NonMatching|Equivalent), "([^"]+)"', cfg)]


def text_ranges():
    spl = open(os.path.join(ROOT, "config/GITE01/splits.txt")).read()
    out = {}
    for name, body in re.findall(r"^(\S[^\n]*):\n((?:\t[^\n]*\n)*)", spl, re.M):
        m = re.search(r"\.text\s+start:0x([0-9A-F]+) end:0x([0-9A-F]+)", body)
        if m:
            out[name] = int(m[1], 16), int(m[2], 16)
    return out


def main():
    us = units()
    tr = text_ranges()
    rows, y = [], 0
    svg = []

    def text(x, yy, s, size=14, fill=FG, weight="normal"):
        svg.append(f'<text x="{x}" y="{yy}" font-family="sans-serif" font-size="{size}" font-weight="{weight}" '
                   f'fill="{fill}">{html.escape(s)}</text>')

    def squares(items, y0):
        # items: list of (matched, tooltip); returns bottom y
        for i, (ok, tip) in enumerate(items):
            r, c = divmod(i, COLS)
            x, yy = 8 + c * (CELL + GAP), y0 + r * (CELL + GAP)
            svg.append(f'<rect x="{x}" y="{yy}" width="{CELL}" height="{CELL}" rx="2" fill="{GREEN if ok else GREY}">'
                       f'<title>{html.escape(tip)}</title></rect>')
        return y0 + ((len(items) + COLS - 1) // COLS) * (CELL + GAP)

    lib_total = sum(1 for p, _ in us if any(p.startswith(g) for _, g in GROUPS))
    lib_done = sum(1 for p, ok in us if ok and any(p.startswith(g) for _, g in GROUPS))
    y = 26
    text(8, y, f"Geist (GITE01) matching decompilation — libraries: {lib_done}/{lib_total} files "
               f"({100 * lib_done / lib_total:.1f}%) byte-identical", 16, weight="bold")
    y += 14
    for label, prefix in GROUPS:
        items = [(ok, f"{p} — {'matching' if ok else 'asm'}") for p, ok in us if p.startswith(prefix)]
        done = sum(ok for ok, _ in items)
        y += 20
        text(8, y, f"{label}: {done}/{len(items)}", 13, MUTED)
        y = squares(items, y + 6)

    # game code: everything in the game .text range that is not inside a library unit
    lo, hi = GAME_TEXT
    lib_in_game = sum(max(0, min(e, hi) - max(s, lo)) for n, (s, e) in tr.items()
                      if any(n.startswith(g) for _, g in GROUPS))
    game_bytes = hi - lo - lib_in_game
    game_done = sum(max(0, min(e, hi) - max(s, lo)) for n, (s, e) in tr.items()
                    if not any(n.startswith(g) for _, g in GROUPS) and (n, True) in [(p, ok) for p, ok in us])
    n = (game_bytes + GAME_CHUNK - 1) // GAME_CHUNK
    done_sq = game_done // GAME_CHUNK
    y += 20
    text(8, y, f"Game code: {100 * game_done / game_bytes:.1f}% ({game_done // 1024} / {game_bytes // 1024} KB "
               f"of .text; one square = 16 KB)", 13, MUTED)
    items = [(i < done_sq, f"game .text block {i + 1}/{n}") for i in range(n)]
    y = squares(items, y + 6)
    y += 22
    svg.append(f'<rect x="8" y="{y - 11}" width="12" height="12" rx="2" fill="{GREEN}"/>')
    text(26, y, "byte-identical C", 12, MUTED)
    svg.append(f'<rect x="150" y="{y - 11}" width="12" height="12" rx="2" fill="{GREY}"/>')
    text(168, y, "still assembly", 12, MUTED)
    y += 12

    out = [f'<svg xmlns="http://www.w3.org/2000/svg" viewBox="0 0 {W} {y}" width="{W}" height="{y}">',
           f'<rect width="{W}" height="{y}" fill="{BG}"/>', *svg, "</svg>"]
    os.makedirs(os.path.join(ROOT, "docs"), exist_ok=True)
    path = os.path.join(ROOT, "docs/progress.svg")
    open(path, "w").write("\n".join(out) + "\n")
    print(f"wrote {path}: libraries {lib_done}/{lib_total}, game {game_done}/{game_bytes} bytes")


if __name__ == "__main__":
    main()
