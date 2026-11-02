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
    /*0x84*/ u8* code;
    /*0x88*/ u8 unk88[0x94 - 0x88];
    /*0x94*/ int functionCount;
    /*0x98*/ u8* functions;
    /*0x9C*/ u8 unk9C[0x20AC - 0x9C];
    /*0x20AC*/ int currentFunction;
} ScriptVM;

// One 12-byte function record of a script.
typedef struct ScriptFunc {
    /*0x00*/ int field00;
    /*0x04*/ int entry;
    /*0x08*/ int field08;
} ScriptFunc;

typedef int (*ScriptOpFn)(int ins, ScriptVM* vm);

extern ScriptVM lbl_8043CAF0;       // the running machine
extern ScriptOpFn lbl_80398148[32]; // handler table
extern int lbl_804D4670;            // pending error code
extern int lbl_804D46A0;            // executed instruction counter
extern int lbl_804D46A4;            // return address of the running call
extern int lbl_804D29DC;            // call depth
extern int lbl_8044069C[0x40];      // call stack of function indices

// Instruction fields.
#define INS_SUB(i) (((i) >> 5) & 7)
#define INS_OFF(i) ((int)((i) & 0xFFFF0000) >> 16)

extern void* memset(void* dst, int val, u32 n);
extern void fn_800050B4(void);

// 0x80083360: run a script function to completion; returns the function's result (r3) or -1
int ScriptRun(ScriptFunc* func, ScriptVM* vm) {
    int savedPc;
    int state = -1;
    int savedFunc;
    int savedRet;
    int ret = lbl_804D4670;
    int res;
    if (ret > 0x40000000)
        return ret;
    savedPc = vm->pc;
    savedRet = lbl_804D46A4;
    lbl_804D46A4 = savedPc + 4;
    savedFunc = vm->currentFunction;
    vm->pc = func->entry;
    vm->currentFunction = (u32)((u8*)func - vm->functions) / 12;
    lbl_804D29DC++;
    if (lbl_804D29DC < 0x40)
        lbl_8044069C[lbl_804D29DC] = vm->currentFunction;
    do {
        if (state >= 0) {
            switch (state) {
            case 3:
                break;
            case 4:
            case 5:
                state = -1;
                break;
            }
        }
        res = lbl_80398148[*(u32*)(lbl_8043CAF0.code + lbl_8043CAF0.pc) & 0x1F](
            *(u32*)(lbl_8043CAF0.code + lbl_8043CAF0.pc), vm);
        if (res > 0x40000000)
            return -1;
        if (state >= 0 && res == 1 && state != 0) {
            vm->pc = savedPc;
            state = 2;
        }
        lbl_804D46A0++;
    } while (res == 0);
    if (lbl_804D29DC >= 0)
        lbl_804D29DC--;
    vm->pc = savedPc;
    vm->currentFunction = savedFunc;
    lbl_804D46A4 = savedRet;
    return vm->r[3];
}

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

// 0x80083F88: store a register to memory at base register + signed 16-bit offset
// (sub 0/1 = byte, 2/3 = halfword, 4/6 = word; sub 6 stores a float register)
int ScriptOpStore(int ins, ScriptVM* vm) {
    u32 sub;
    u32 val;
    u8* addr;
    int err;
    int idx = ((u32)ins >> 12) & 0xF;
    if (INS_SUB(ins) == 6)
        val = *(u32*)((u8*)vm + idx * 4 + 0x60);
    else
        val = *(u32*)((u8*)vm + idx * 4);
    sub = ((u32)ins >> 5) & 6;
    addr = (u8*)(*(int*)((u8*)vm + (((u32)ins >> 8) & 0xF) * 4) + (ins >> 16));
    err = 0;
    switch (sub) {
    case 0:
        *addr = val;
        break;
    case 2:
        if ((u32)addr & 1) {
            err = 0x40000004;
        } else {
            *(u16*)addr = val;
        }
        break;
    case 4:
    case 6:
        if ((u32)addr & 3) {
            err = 0x40000004;
        } else {
            *(u32*)addr = val;
        }
        break;
    }
    if (err == 0)
        vm->pc += 4;
    return err;
}

// 0x80084050: store a register through a pointer register (addresses with bit 28 set are stack-relative)
int ScriptOpStorePtr(int ins, ScriptVM* vm) {
    u32 sub;
    u8* addr = *(u8**)((u8*)vm + (((u32)ins >> 10) & 0x3C));
    u32 val = *(u32*)((u8*)vm + (((u32)ins >> 6) & 0x3C));
    int err;
    if ((u32)addr & 0x10000000)
        addr = (u8*)(((u32)addr & 0xFFFFFFF) + vm->r[1]);
    sub = ((u32)ins >> 5) & 6;
    err = 0;
    switch (sub) {
    case 0:
        *addr = val;
        break;
    case 2:
        if ((u32)addr & 1) {
            err = 0x40000004;
        } else {
            *(u16*)addr = val;
        }
        break;
    case 4:
    case 6:
        if ((u32)addr & 3) {
            err = 0x40000004;
        } else {
            *(u32*)addr = val;
        }
        break;
    }
    if (err == 0)
        vm->pc += 4;
    return err;
}

// 0x80084100
int ScriptOpNop(int ins, ScriptVM* vm) {
    return 0;
}

// 0x80084108: load from memory at base register + signed 16-bit offset (sub 6 loads a float register)
int ScriptOpLoad(int ins, ScriptVM* vm) {
    int sub = INS_SUB(ins);
    int dst = ((u32)ins >> 12) & 0xF;
    u8* addr = (u8*)(*(int*)((u8*)vm + (((u32)ins >> 6) & 0x3C)) + (s16)((int)(ins & 0xFFFF0000) >> 16));
    u32 val;
    int err;
    switch (sub) {
    case 0: val = *(s8*)addr; break;
    case 1: val = *(u8*)addr; break;
    case 2: val = *(s16*)addr; break;
    case 3: val = *(u16*)addr; break;
    case 4:
    case 6:
        if ((u32)addr & 3)
            return 0x40000003;
        val = *(u32*)addr;
        break;
    default: val = 0; break;
    }
    if (sub == 6)
        *(u32*)((u8*)vm + dst * 4 + 0x60) = val;
    else
        *(u32*)((u8*)vm + dst * 4) = val;
    vm->pc += 4;
    return 0;
}

// 0x800841CC: convert float to int (sub 0) or int to float (sub 1)
int ScriptOpConvert(int ins, ScriptVM* vm) {
    int sub = INS_SUB(ins);
    int src = ((u32)ins >> 8) & 0xF;
    int dst = ((u32)ins >> 12) & 0xF;
    FloatBits res;
    switch (sub) {
    case 0:
        vm->r[dst] = (int)vm->f[src];
        break;
    case 1:
        res.f = (f32)vm->r[src];
        *(u32*)&vm->f[dst] = res.u;
        break;
    default:
        return 0x40000001;
    }
    vm->pc += 4;
    return 0;
}

// 0x8008427C: shift left (sub 0), logical right (sub 1), arithmetic right (sub 2) by an immediate
int ScriptOpShift(int ins, ScriptVM* vm) {
    int sub = INS_SUB(ins);
    int dst;
    u32 val = *(u32*)((u8*)vm + (((u32)ins >> 6) & 0x3C));
    u32 n = (u32)ins >> 16;
    dst = ((u32)ins >> 12) & 0xF;
    switch (sub) {
    case 0: val = val << n; break;
    case 1: val = val >> n; break;
    case 2: val = (s32)val >> n; break;
    default: return 0x40000001;
    }
    vm->r[dst] = val;
    vm->pc += 4;
    return 0;
}

// 0x800842F4: shift by a register: left (sub 0), logical right (sub 1), arithmetic right (sub 2)
int ScriptOpShiftReg(int ins, ScriptVM* vm) {
    int sub = INS_SUB(ins);
    int dst;
    u32 val = *(u32*)((u8*)vm + (((u32)ins >> 6) & 0x3C));
    u32 n = *(u32*)((u8*)vm + (((u32)ins >> 10) & 0x3C));
    dst = ((u32)ins >> 16) & 0xF;
    switch (sub) {
    case 0: val = val << n; break;
    case 1: val = val >> n; break;
    case 2: val = (s32)val >> n; break;
    default: return 0x40000001;
    }
    vm->r[dst] = val;
    vm->pc += 4;
    return 0;
}

// 0x80084370: logical op with an immediate: and (sub 0), or (sub 1), xor (sub 2)
int ScriptOpLogicImm(int ins, ScriptVM* vm) {
    int sub = INS_SUB(ins);
    int dst;
    u32 val = *(u32*)((u8*)vm + (((u32)ins >> 6) & 0x3C));
    u32 imm = (u32)ins >> 16;
    dst = ((u32)ins >> 12) & 0xF;
    switch (sub) {
    case 0: val = val & imm; break;
    case 1: val = val | imm; break;
    case 2: val = val ^ imm; break;
    default: return 0x40000001;
    }
    vm->r[dst] = val;
    vm->pc += 4;
    return 0;
}

// 0x800843E8: logical op on two registers: and (sub 0), or (sub 1), xor (sub 2), not (sub 3)
int ScriptOpLogic(int ins, ScriptVM* vm) {
    int sub = INS_SUB(ins);
    u32 a = *(u32*)((u8*)vm + (((u32)ins >> 6) & 0x3C));
    u32 b = *(u32*)((u8*)vm + (((u32)ins >> 10) & 0x3C));
    switch (sub) {
    case 0: b = b & a; break;
    case 1: b = b | a; break;
    case 2: b = b ^ a; break;
    case 3: b = ~a; break;
    default: return 0x40000001;
    }
    vm->r[((u32)ins >> 16) & 0xF] = b;
    vm->pc += 4;
    return 0;
}

// 0x80084470: register move (sub 0 int, sub 1 float) and negate (sub 2 float, sub 3 int)
int ScriptOpMove(int ins, ScriptVM* vm) {
    int sub = INS_SUB(ins);
    int src = ((u32)ins >> 8) & 0xF;
    int dst = ((u32)ins >> 12) & 0xF;
    switch (sub) {
    case 0: vm->r[dst] = vm->r[src]; break;
    case 1: *(u32*)&vm->f[dst] = *(u32*)&vm->f[src]; break;
    case 2: vm->f[dst] = -vm->f[src]; break;
    case 3: vm->r[dst] = -vm->r[src]; break;
    default: return 0x40000001;
    }
    vm->pc += 4;
    return 0;
}

// 0x8008452C
int ScriptOpSkip(int ins, ScriptVM* vm) {
    vm->pc += 4;
    return 0;
}
