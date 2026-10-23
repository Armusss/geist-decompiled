#!/bin/sh
# scan_fakes.sh [PATH...]: list forbidden fake-match patterns in game sources (default src/game). Exit 1 if any.
# Forbidden: calling functions through extern "C" declarations of mangled names, __declspec(section), any
# #pragma except a file-level `#pragma auto_inline off`, leftover draft-skeleton stubs, and Ghidra pseudocode
# artifacts (FUN_/DAT_ names, undefinedN types). `asm` blocks are listed for review, not rejected: real hardware
# ops (GQR setup, paired-single intrinsics) are fine; inline asm for ordinary code is not.
P=${*:-src/game}
bad=0
grep -rnE 'extern "C"[^;(]*\b[A-Za-z]\w*__F[A-Za-z0-9_]+|__declspec\(section|^// STUB |DRAFT SKELETON|\bFUN_8[0-9a-fA-F]{7}\b|\bDAT_8[0-9a-fA-F]{7}\b|\bundefined[1248]\b' $P && bad=1
grep -rnE '^\s*#pragma' $P | grep -vE '#pragma auto_inline off$' && bad=1
grep -rnwE 'asm' $P | sed 's/^/REVIEW asm: /'
exit $bad
