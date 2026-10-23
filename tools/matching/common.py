#!/usr/bin/env python3
"""Shared helpers for the matching tools: read the original main.dol's .text sections, read a compiled
object's functions with relocated fields masked, and disassemble with the project's binutils.
GM (the repo root) defaults to this file's repo; set the GM environment variable to point at another checkout."""
import os, struct, subprocess, sys, tempfile

GM = os.environ.get('GM') or os.path.dirname(os.path.dirname(os.path.dirname(os.path.abspath(__file__))))
DOL = os.path.join(GM, 'orig/GITE01/sys/main.dol')

def dol_text():
    d = open(DOL, 'rb').read()
    offs, addrs, sizes = (struct.unpack_from('>7I', d, o) for o in (0, 0x48, 0x90))
    return [(a, d[o:o + s]) for o, a, s in zip(offs, addrs, sizes) if s]

LOCALS = {}

def elf_funcs(path):
    e = open(path, 'rb').read()
    shoff, = struct.unpack_from('>I', e, 0x20); shnum, shstr = struct.unpack_from('>HH', e, 0x30)
    sh = [struct.unpack_from('>10I', e, shoff + 40 * i) for i in range(shnum)]
    names = e[sh[shstr][4]:]
    nm = lambda o, base: base[o:base.index(b'\0', o)].decode()
    secs = {i: nm(s[0], names) for i, s in enumerate(sh)}
    symtab = next(i for i, s in enumerate(sh) if s[1] == 2)
    strtab = sh[sh[symtab][6]]
    strs = e[strtab[4]:strtab[4] + strtab[5]]
    syms = []; local_names = set()
    for k in range(sh[symtab][5] // 16):
        n, val, size, info, other, shndx = struct.unpack_from('>IIIBBH', e, sh[symtab][4] + 16 * k)
        syms.append((nm(n, strs), val, size, info & 0xf, shndx))
        if info >> 4 == 0: local_names.add(nm(n, strs))
    out = []
    for i, s in enumerate(sh):
        if secs[i] != '.text' or s[1] != 1:
            continue
        data = bytearray(e[s[4]:s[4] + s[5]])
        mask = bytearray(b'\xff' * len(data))
        for j, r in enumerate(sh):
            if r[1] == 4 and r[7] == i:           # SHT_RELA for this section
                for q in range(r[5] // 12):
                    off, info, _ = struct.unpack_from('>IIi', e, r[4] + 12 * q)
                    t = info & 0xff
                    w = off & ~3
                    if t in (10,):                    # R_PPC_REL24: keep opcode, mask target
                        mask[w:w + 4] = b'\xfc\x00\x00\x03'
                    elif t in (4, 5, 6, 109):         # ADDR16_LO/HI/HA, EMB_SDA21
                        mask[w + 2:w + 4] = b'\x00\x00'
                        if t == 109: mask[w + 1] = 0xe0   # SDA21 also picks the base register
                    else:
                        mask[w:w + 4] = b'\x00\x00\x00\x00'
        for (n, val, size, typ, shndx) in sorted(syms, key=lambda x: x[1]):
            if shndx == i and typ == 2 and size:
                out.append((n, bytes(data[val:val + size]), bytes(mask[val:val + size])))
                LOCALS[n] = n in local_names
    return out

def find(code, mask, texts):
    hits = []
    first = next((k for k in range(0, len(code), 4) if mask[k:k + 4] == b'\xff\xff\xff\xff'), None)
    if first is None: return hits
    key = code[first:first + 4]
    for base, t in texts:
        i = t.find(key)
        while i != -1:
            st = i - first
            if st >= 0 and st + len(code) <= len(t) and all((t[st + k] & mask[k]) == (code[k] & mask[k]) for k in range(len(code))):
                hits.append(base + st)
            i = t.find(key, i + 4)
    return hits

def disasm(words, base):
    import tempfile as tf
    od = os.path.join(GM, 'build/binutils/powerpc-eabi-objdump')
    with tf.NamedTemporaryFile(suffix='.bin') as f:
        f.write(words); f.flush()
        r = subprocess.run([od, '-D', '-b', 'binary', '-m', 'powerpc:common', '-EB', f'--adjust-vma={base:#x}', f.name], capture_output=True, text=True)
    return [l.split('\t', 2)[-1].strip() for l in r.stdout.splitlines() if l.strip().endswith(':') is False and '\t' in l]

