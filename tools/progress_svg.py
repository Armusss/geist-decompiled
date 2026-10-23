#!/usr/bin/env python3
"""Draw docs/progress.svg: one square per translation unit, green when it is verified byte-identical C,
orange when it is partly matched C, grey when it is still assembly.

Usage: python3 tools/progress_svg.py   (run from the repo root after `python3 configure.py && ninja`)
A unit is drawn green only when all three hold:
  1. configure.py marks it Matching;
  2. build.ninja links main.elf from its compiled C object (build/GITE01/src/...), not from the original asm;
  3. build/GITE01/main.dol has the SHA-1 in config/GITE01/build.sha1 (the original DOL).
Together these prove the unit's C produces exactly the original bytes at its address. (objdiff's per-unit
data percentages can read below 100% for such units because they count alignment padding and unnamed
compiler symbols; the linked DOL is the authority.) If the DOL check fails, every square is drawn grey.
A unit that is not green is drawn orange ("partly matched") when objdiff's build/GITE01/report.json shows at
least one of its functions at 100% (run `ninja build/GITE01/report.json` first); the tooltip gives the count."""
import hashlib
import json
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

GREEN, ORANGE, GREY, BG, FG, MUTED = "#2ea043", "#d29922", "#30363d", "#0d1117", "#ffffff", "#8b949e"
W, COLS, CELL, GAP = 1000, 48, 18, 2


def dol_verified():
    dol = os.path.join(ROOT, "build/GITE01/main.dol")
    want = open(os.path.join(ROOT, "config/GITE01/build.sha1")).read().split()[0]
    if not os.path.exists(dol):
        return False, want
    return hashlib.sha1(open(dol, "rb").read()).hexdigest() == want, want


def linked_from_c():
    lines = open(os.path.join(ROOT, "build.ninja")).read().split("\n")
    i = next(k for k, l in enumerate(lines) if l.startswith("build build/GITE01/main.elf: link"))
    buf = ""
    while True:
        buf += " " + lines[i].rstrip("$")
        if not lines[i].endswith("$"):
            break
        i += 1
    return {x for x in buf.split("|")[0].split() if x.startswith("build/GITE01/src/") and x.endswith(".o")}


def units():
    cfg = open(os.path.join(ROOT, "configure.py")).read()
    ok, _ = dol_verified()
    c_objs = linked_from_c()
    out = []
    for m in re.finditer(r'Object\((Matching|NonMatching|Equivalent), "([^"]+)"', cfg):
        obj = "build/GITE01/src/" + os.path.splitext(m[2])[0] + ".o"
        out.append((m[2], ok and m[1] == "Matching" and obj in c_objs))
    return out


def partial_counts():
    """{unit path prefix without extension: (matched functions, total functions)} from objdiff's report."""
    path = os.path.join(ROOT, "build/GITE01/report.json")
    if not os.path.exists(path):
        return {}
    out = {}
    for u in json.load(open(path)).get("units", []):
        fs = u.get("functions", [])
        if fs:
            name = u["name"].split("/", 1)[1] if "/" in u["name"] else u["name"]
            out[name] = (sum(1 for f in fs if f.get("fuzzy_match_percent", 0) >= 100), len(fs))
    return out


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
        for i, (st, tip) in enumerate(items):
            r, c = divmod(i, COLS)
            x, yy = 8 + c * (CELL + GAP), y0 + r * (CELL + GAP)
            svg.append(f'<rect x="{x}" y="{yy}" width="{CELL}" height="{CELL}" rx="2" fill="{GREEN if st == 'g' else ORANGE if st == 'p' else GREY}">'
                       f'<title>{html.escape(tip)}</title></rect>')
        return y0 + ((len(items) + COLS - 1) // COLS) * (CELL + GAP)

    pc = partial_counts()

    def item(p, ok):
        if ok:
            return ("g", f"{p} - matching")
        m, n = pc.get(os.path.splitext(p)[0], (0, 0))
        if m > 0:
            return ("p", f"{p} - partial: {m}/{n} functions match")
        return ("a", f"{p} - asm")

    lib_total = sum(1 for p, _ in us if any(p.startswith(g) for _, g in GROUPS))
    lib_done = sum(1 for p, ok in us if ok and any(p.startswith(g) for _, g in GROUPS))
    y = 26
    text(8, y, f"Geist (GITE01) matching decompilation — libraries: {lib_done}/{lib_total} files "
               f"({100 * lib_done / lib_total:.1f}%) byte-identical", 16, weight="bold")
    y += 14
    for label, prefix in GROUPS:
        items = [item(p, ok) for p, ok in us if p.startswith(prefix)]
        done = sum(st == "g" for st, _ in items)
        y += 20
        text(8, y, f"{label}: {done}/{len(items)}", 13, MUTED)
        y = squares(items, y + 6)

    # game code: one square per (provisional) game file
    gitems = [item(p, ok) for p, ok in us if p.startswith('game/')]
    gdone = sum(st == "g" for st, _ in gitems)
    gpart = sum(st == "p" for st, _ in gitems)
    y += 20
    text(8, y, f"Game code: {gdone}/{len(gitems)} files matching, {gpart} partly matched "
               f"(provisional file boundaries)", 13, MUTED)
    y = squares(gitems, y + 6)
    y += 22
    svg.append(f'<rect x="8" y="{y - 11}" width="12" height="12" rx="2" fill="{GREEN}"/>')
    text(26, y, "byte-identical C", 12, MUTED)
    svg.append(f'<rect x="150" y="{y - 11}" width="12" height="12" rx="2" fill="{ORANGE}"/>')
    text(168, y, "partly matched C", 12, MUTED)
    svg.append(f'<rect x="300" y="{y - 11}" width="12" height="12" rx="2" fill="{GREY}"/>')
    text(318, y, "still assembly", 12, MUTED)
    ok, want = dol_verified()
    text(440, y, (f"verified: built main.dol SHA-1 {want[:12]}… = original" if ok
                  else "NOT verified: built main.dol does not match the original SHA-1"), 12,
         GREEN if ok else "#f85149")
    y += 12

    out = [f'<svg xmlns="http://www.w3.org/2000/svg" viewBox="0 0 {W} {y}" width="{W}" height="{y}">',
           f'<rect width="{W}" height="{y}" fill="{BG}"/>', *svg, "</svg>"]
    os.makedirs(os.path.join(ROOT, "docs"), exist_ok=True)
    path = os.path.join(ROOT, "docs/progress.svg")
    open(path, "w").write("\n".join(out) + "\n")
    print(f"wrote {path}: libraries {lib_done}/{lib_total}, game {gdone}/{len(gitems)} files, {gpart} partly matched")


if __name__ == "__main__":
    main()
