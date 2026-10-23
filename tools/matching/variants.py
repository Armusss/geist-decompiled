#!/usr/bin/env python3
"""variants.py SRC FUNC ADDR SIZE VARIANTS.py: try source variants of one function. VARIANTS.py defines a list
V of (name, old_text, new_text); each variant replaces old_text by new_text in SRC, counts mdiff's `!!` lines for
FUNC, then SRC is restored. Prints one line per variant. Use it for brute force: generate many plausible,
ordinary source shapes (declaration orders, statement orders, inline helpers, types) and keep the best one.
Every variant must still be source the original programmer could have written (see CONTRIBUTING.md)."""
import os, subprocess, sys
HERE = os.path.dirname(os.path.abspath(__file__))
src, fn, addr, size, vf = sys.argv[1:6]
orig = open(src).read()
ns = {}
exec(open(vf).read(), ns)
try:
    for name, old, new in ns['V']:
        if old not in orig:
            print(name, 'OLD TEXT NOT FOUND'); continue
        open(src, 'w').write(orig.replace(old, new, 1))
        o = subprocess.run([sys.executable, os.path.join(HERE, 'mdiff.py'), src, fn, addr, size],
                           capture_output=True, text=True)
        print(name, 'BUILDFAIL' if o.returncode == 2 else o.stdout.count('!!'))
finally:
    open(src, 'w').write(orig)
