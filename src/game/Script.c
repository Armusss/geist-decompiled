// Script.c: the script virtual machine.  A script is an array of 12-byte function records; the code
// is a stream of 32-bit instruction words.  The low 5 bits of a word select one of 32 handlers
// (jump table at 0x80398148); each handler receives (instruction, vm) and returns 0 to continue,
// 1 to leave the current function, or a value above 0x40000000 for an error.
// (0x80083360-0x80085CDC)
// Names are descriptive guesses; struct layouts are partial and derived from the retail asm.

typedef unsigned char u8;
typedef signed char s8;
typedef unsigned short u16;
typedef signed short s16;
typedef unsigned long u32;
typedef signed long s32;
typedef float f32;

// Script machine state.  Integer registers start at 0 (r1 is the stack pointer, r3 the result),
// float registers start at 0x60.
typedef struct ScriptVM {
    /*0x00*/ int r[24];
    /*0x60*/ f32 f[8];
    /*0x80*/ int pc;
    /*0x84*/ u8 unk84[0x94 - 0x84];
    /*0x94*/ int functionCount;
    /*0x98*/ u8* functions;
    /*0x9C*/ u8 unk9C[0x20AC - 0x9C];
    /*0x20AC*/ int currentFunction;
} ScriptVM;

// Instruction fields.
#define INS_SUB(i) (((i) >> 5) & 7)
#define INS_OFF(i) ((int)((i) & 0xFFFF0000) >> 16)

extern void* memset(void* dst, int val, u32 n);
extern void fn_800050B4(void);

// 0x800834C0: stack adjust (sub 0 = release n bytes of stack and clear them, otherwise grow)
int ScriptOpStack(int ins, ScriptVM* vm) {
    int n = (u32)ins >> 16;
    if (INS_SUB(ins) == 0) {
        if (n != 0) {
            vm->r[1] -= n;
            memset((void*)vm->r[1], 0, n);
        }
    } else {
        vm->r[1] += n;
    }
    vm->pc += 4;
    return 0;
}

// 0x800839A8: conditional branch comparing two float registers (sub = comparison)
int ScriptOpBranchFloat(int ins, ScriptVM* vm) {
    int sub = INS_SUB(ins);
    f32 a = *(f32*)((u8*)vm + (((u32)ins >> 6) & 0x3C) + 0x60);
    f32 b = *(f32*)((u8*)vm + (((u32)ins >> 10) & 0x3C) + 0x60);
    int taken;
    switch (sub) {
    case 0: taken = (u8)(a == b); break;
    case 1: taken = (u8)(a != b); break;
    case 2: taken = (u8)(a < b); break;
    case 3: taken = (u8)(a > b); break;
    case 4: taken = (u8)(a <= b); break;
    case 5: taken = (u8)(a >= b); break;
    default: return 0x40000001;
    }
    if (taken)
        vm->pc += INS_OFF(ins);
    else
        vm->pc += 4;
    return 0;
}

// 0x80083AA8: conditional branch comparing two integer registers as unsigned values
int ScriptOpBranchUnsigned(int ins, ScriptVM* vm) {
    int sub = INS_SUB(ins);
    u32 a = *(u32*)((u8*)vm + (((u32)ins >> 6) & 0x3C));
    u32 b = *(u32*)((u8*)vm + (((u32)ins >> 10) & 0x3C));
    u32 taken;
    switch (sub) {
    case 0: taken = (u8)(a == b); break;
    case 1: taken = (u8)(a != b); break;
    case 2: taken = (u8)(a < b); break;
    case 3: taken = (u8)(a > b); break;
    case 4: taken = (u8)(a <= b); break;
    case 5: taken = (u8)(a >= b); break;
    default: return 0x40000001;
    }
    if (taken)
        vm->pc += INS_OFF(ins);
    else
        vm->pc += 4;
    return 0;
}

// 0x80083BAC: conditional branch comparing two integer registers as signed values
int ScriptOpBranchSigned(int ins, ScriptVM* vm) {
    int sub = INS_SUB(ins);
    s32 a = *(s32*)((u8*)vm + (((u32)ins >> 6) & 0x3C));
    s32 b = *(s32*)((u8*)vm + (((u32)ins >> 10) & 0x3C));
    int taken;
    switch (sub) {
    case 0: taken = (u8)(a == b); break;
    case 1: taken = (u8)(a != b); break;
    case 2: taken = (u8)(a < b); break;
    case 3: taken = (u8)(a > b); break;
    case 4: taken = (u8)(a <= b); break;
    case 5: taken = (u8)(a >= b); break;
    default: return 0x40000001;
    }
    if (taken)
        vm->pc += INS_OFF(ins);
    else
        vm->pc += 4;
    return 0;
}

// 0x80083CB8: jump (sub 1 = to the address in a register, otherwise relative)
int ScriptOpJump(int ins, ScriptVM* vm) {
    if (INS_SUB(ins) == 1)
        vm->pc = *(int*)((u8*)vm + (((u32)ins >> 14) & 0x3FFFC));
    else
        vm->pc += INS_OFF(ins);
    return 0;
}

typedef union FloatBits {
    f32 f;
    u32 u;
} FloatBits;

// 0x80083CF0: float multiply (sub 0) / divide (sub 2): f[dst] = f[b] op f[a]
int ScriptOpFloatMul(int ins, ScriptVM* vm) {
    f32 b = *(f32*)((u8*)vm + (((u32)ins >> 10) & 0x3C) + 0x60);
    int sub = INS_SUB(ins);
    f32 a = *(f32*)((u8*)vm + (((u32)ins >> 6) & 0x3C) + 0x60);
    int dst = ((u32)ins >> 16) & 0xF;
    FloatBits res;
    res.f = b;
    switch (sub) {
    case 0: res.f = b * a; break;
    case 2: res.f = a / b; break;
    default: return 0x40000001;
    }
    ((u32*)vm->f)[dst] = res.u;
    vm->pc += 4;
    return 0;
}

// 0x80083D88: integer multiply / divide / remainder
int ScriptOpIntMul(int ins, ScriptVM* vm) {
    int sub = INS_SUB(ins);
    u32 a = *(u32*)((u8*)vm + (((u32)ins >> 6) & 0x3C));
    u32 b = *(u32*)((u8*)vm + (((u32)ins >> 10) & 0x3C));
    int dst = ((u32)ins >> 16) & 0xF;
    u32 res;
    switch (sub) {
    case 0: res = b * a; break;
    case 2: if ((s32)b != 0) res = (s32)a / (s32)b; else res = 0; break;
    case 3: if ((s32)b != 0) res = a / b; else res = 0; break;
    case 4: res = a - (s32)a / (s32)b * b; break;
    case 5: res = a - a / b * b; break;
    default: return 0x40000001;
    }
    *(u32*)((u8*)vm + dst * 4) = res;
    vm->pc += 4;
    return 0;
}

// 0x80083E54: float add (sub 0) / subtract (sub 3)
int ScriptOpFloatAdd(int ins, ScriptVM* vm) {
    f32 b = *(f32*)((u8*)vm + (((u32)ins >> 10) & 0x3C) + 0x60);
    int sub = INS_SUB(ins);
    f32 a = *(f32*)((u8*)vm + (((u32)ins >> 6) & 0x3C) + 0x60);
    int dst = ((u32)ins >> 16) & 0xF;
    FloatBits res;
    res.f = b;
    switch (sub) {
    case 0: res.f = a + b; break;
    case 3: res.f = a - b; break;
    }
    ((u32*)vm->f)[dst] = res.u;
    vm->pc += 4;
    return 0;
}

// 0x80083ED0: integer add / subtract (sub 2 = add a signed 16-bit immediate)
int ScriptOpIntAdd(int ins, ScriptVM* vm) {
    int sub = INS_SUB(ins);
    int a = *(int*)((u8*)vm + (((u32)ins >> 6) & 0x3C));
    int b = *(int*)((u8*)vm + (((u32)ins >> 10) & 0x3C));
    int dstA = ((u32)ins >> 12) & 0xF;
    int dst = ((u32)ins >> 16) & 0xF;
    switch (sub) {
    case 2:
        b = a + (s16)((int)(ins & 0xFFFF0000) >> 16);
        dst = dstA;
        break;
    case 0:
    case 1:
        b = a + b;
        break;
    case 3:
    case 4:
        b = a - b;
        break;
    }
    *(int*)((u8*)vm + dst * 4) = b;
    vm->pc += 4;
    return 0;
}

// 0x80083F68: load an upper-half immediate into a register
int ScriptOpLoadHigh(int ins, ScriptVM* vm) {
    *(u32*)((u8*)vm + (((u32)ins >> 10) & 0x3C)) = ins & 0xFFFF0000;
    vm->pc += 4;
    return 0;
}
