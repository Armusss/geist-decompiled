// u_800C99E8.cpp (0x800C99E8-0x800CF2E0): actor movement / physics helpers with two Buffer-derived list classes
// (vtables .data 0x8039B8B8 / 0x8039B8D8, element hooks fn_800CF1F0..fn_800CF2C0 emitted last as inline
// virtuals), split from the old u_800C5028 range.
// Boundary evidence: own .sdata2 pool 0x804D6F30-0x804D6FE8 (1.0f and int-to-float double duplicated from
// the previous file); it calls the camera getter fn_800C5E6C (8 bytes) out of line. End: the list classes'
// inline virtuals end at 0x800CF2E0; the next file's pool starts at 0x804D6FE8 with its own 0.0f.
// Data: .data 0x8039B7A0-0x8039B8F8, .rodata 0x80344B88-0x80344BA0, .sbss 0x804D48F8-0x804D4900.
// Names are descriptive guesses; unmatched against retail unless noted.

typedef unsigned long u32;
typedef signed long s32;
typedef unsigned char u8;

extern "C" {
void* memset(void* dst, int c, unsigned long n);
void qsort(void* base, unsigned long n, unsigned long size, int (*cmp)(const void*, const void*));
void fn_80026660(int msg, void* handler, int arg);
void fn_800933D0(int anim);
void fn_8009337C(int anim);
int fn_80084540(int to, int msg, void* data, void* result);
}
void AnimUnref(int anim);
void AnimRef(int anim);


// TODO (not drafted yet, 0x180 bytes at 0x800C99E8)
// fn_800C99E8
// TODO (not drafted yet, 0x88 bytes at 0x800C9B68)
// fn_800C9B68
// TODO (not drafted yet, 0x78 bytes at 0x800C9BF0)
// fn_800C9BF0
// TODO (not drafted yet, 0x6C bytes at 0x800C9C68)
// fn_800C9C68
// TODO (not drafted yet, 0x68 bytes at 0x800C9CD4)
// fn_800C9CD4
// TODO (not drafted yet, 0x194 bytes at 0x800C9D3C)
// fn_800C9D3C
// TODO (not drafted yet, 0x78 bytes at 0x800C9ED0)
// fn_800C9ED0
// TODO (not drafted yet, 0x78 bytes at 0x800C9F48)
// fn_800C9F48
// TODO (not drafted yet, 0xC0 bytes at 0x800C9FC0)
// fn_800C9FC0
// TODO (not drafted yet, 0x130 bytes at 0x800CA080)
// fn_800CA080
// TODO (not drafted yet, 0x998 bytes at 0x800CA1B0)
// fn_800CA1B0
// TODO (not drafted yet, 0xA0 bytes at 0x800CAB48)
// fn_800CAB48
// TODO (not drafted yet, 0x124 bytes at 0x800CABE8)
// fn_800CABE8
// TODO (not drafted yet, 0xD4 bytes at 0x800CAD0C)
// fn_800CAD0C
// TODO (not drafted yet, 0xADC bytes at 0x800CADE0)
// fn_800CADE0
// TODO (not drafted yet, 0x1E4 bytes at 0x800CB8BC)
// fn_800CB8BC
// TODO (not drafted yet, 0x650 bytes at 0x800CBAA0)
// fn_800CBAA0
// TODO (not drafted yet, 0x12C bytes at 0x800CC0F0)
// fn_800CC0F0
// TODO (not drafted yet, 0xB4 bytes at 0x800CC21C)
// fn_800CC21C
// TODO (not drafted yet, 0x38 bytes at 0x800CC2D0)
// fn_800CC2D0
// TODO (not drafted yet, 0x5C4 bytes at 0x800CC308)
// fn_800CC308
// TODO (not drafted yet, 0xB8 bytes at 0x800CC8CC)
// fn_800CC8CC
// TODO (not drafted yet, 0x104 bytes at 0x800CC984)
// fn_800CC984
// TODO (not drafted yet, 0x560 bytes at 0x800CCA88)
// fn_800CCA88
// TODO (not drafted yet, 0x5D0 bytes at 0x800CCFE8)
// fn_800CCFE8
// TODO (not drafted yet, 0x20 bytes at 0x800CD5B8)
// fn_800CD5B8
// TODO (not drafted yet, 0x608 bytes at 0x800CD5D8)
// fn_800CD5D8
// TODO (not drafted yet, 0x130 bytes at 0x800CDBE0)
// fn_800CDBE0
// TODO (not drafted yet, 0x618 bytes at 0x800CDD10)
// fn_800CDD10
// TODO (not drafted yet, 0x210 bytes at 0x800CE328)
// fn_800CE328
// TODO (not drafted yet, 0x224 bytes at 0x800CE538)
// fn_800CE538
// TODO (not drafted yet, 0x210 bytes at 0x800CE75C)
// fn_800CE75C
// TODO (not drafted yet, 0x218 bytes at 0x800CE96C)
// fn_800CE96C
// TODO (not drafted yet, 0x148 bytes at 0x800CEB84)
// fn_800CEB84
// TODO (not drafted yet, 0x13C bytes at 0x800CECCC)
// fn_800CECCC
// TODO (not drafted yet, 0xC0 bytes at 0x800CEE08)
// fn_800CEE08
// TODO (not drafted yet, 0x19C bytes at 0x800CEEC8)
// fn_800CEEC8
// TODO (not drafted yet, 0x18C bytes at 0x800CF064)
// fn_800CF064
// TODO (not drafted yet, 0x48 bytes at 0x800CF1F0)
// fn_800CF1F0
// TODO (not drafted yet, 0x60 bytes at 0x800CF238)
// fn_800CF238
// TODO (not drafted yet, 0x8 bytes at 0x800CF298)
// fn_800CF298
// TODO (not drafted yet, 0x20 bytes at 0x800CF2A0)
// fn_800CF2A0
// TODO (not drafted yet, 0x20 bytes at 0x800CF2C0)
// fn_800CF2C0
