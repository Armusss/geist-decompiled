// u_800CF2E0.cpp (0x800CF2E0-0x800D0254): a game subsystem with AppMain hooks (fn_800CF2E0..fn_800CF390; the
// last registers the message handler fn_800CF3C0 through fn_80026660), split from the old u_800C5028 range.
// Boundary evidence: starts after the previous file's inline list virtuals (0x800CF2E0); own .sdata2 pool from
// 0x804D6FE8 (0.0f, 1.0f duplicated); .data 0x8039B8F8-0x8039B960, .rodata 0x80344BA0, .sdata 0x804D2BB8.
// The AppMain hooks read no data and could be their own small file.
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


// TODO (not drafted yet, 0x4 bytes at 0x800CF2E0)
// fn_800CF2E0
// TODO (not drafted yet, 0x58 bytes at 0x800CF2E4)
// fn_800CF2E4
// TODO (not drafted yet, 0x54 bytes at 0x800CF33C)
// fn_800CF33C
// TODO (not drafted yet, 0x30 bytes at 0x800CF390)
// fn_800CF390
// TODO (not drafted yet, 0xF4 bytes at 0x800CF3C0)
// fn_800CF3C0
// TODO (not drafted yet, 0x460 bytes at 0x800CF4B4)
// fn_800CF4B4
// TODO (not drafted yet, 0x224 bytes at 0x800CF914)
// fn_800CF914
// TODO (not drafted yet, 0x40 bytes at 0x800CFB38)
// fn_800CFB38
// TODO (not drafted yet, 0x100 bytes at 0x800CFB78)
// fn_800CFB78
// TODO (not drafted yet, 0x8 bytes at 0x800CFC78)
// fn_800CFC78
// TODO (not drafted yet, 0x104 bytes at 0x800CFC80)
// fn_800CFC80
// TODO (not drafted yet, 0x120 bytes at 0x800CFD84)
// fn_800CFD84
// TODO (not drafted yet, 0x80 bytes at 0x800CFEA4)
// fn_800CFEA4
// TODO (not drafted yet, 0x8 bytes at 0x800CFF24)
// fn_800CFF24
// TODO (not drafted yet, 0x2C bytes at 0x800CFF2C)
// fn_800CFF2C
// TODO (not drafted yet, 0x140 bytes at 0x800CFF58)
// fn_800CFF58
// TODO (not drafted yet, 0x7C bytes at 0x800D0098)
// fn_800D0098
// TODO (not drafted yet, 0x24 bytes at 0x800D0114)
// fn_800D0114
// TODO (not drafted yet, 0xEC bytes at 0x800D0138)
// fn_800D0138
// TODO (not drafted yet, 0x8 bytes at 0x800D0224)
// fn_800D0224
// TODO (not drafted yet, 0x28 bytes at 0x800D022C)
// fn_800D022C
