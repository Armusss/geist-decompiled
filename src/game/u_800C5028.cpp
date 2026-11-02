// u_800C5028.cpp: the actor core: animation-state table lookup, actor message helpers,
// actor rendering, collision hooks and the actor type registry.
// Names are descriptive guesses; the code is matched against the retail DOL.

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

// A sorted table entry: an animation key and the list of states that use it.
struct AnimTableEntry {
    u32 key;      // 0x00
    u32* states;  // 0x04
    int count;    // 0x08
};

// A message queued for later delivery (a pooled singly linked list).
struct PendingMsg {
    int arg0;          // 0x00
    int target;        // 0x04
    int arg1;          // 0x08
    PendingMsg* next;  // 0x0c
};

extern AnimTableEntry* lbl_804D4898;  // sorted animation table
extern int lbl_804D489C;              // entries in the table
extern int lbl_804D48A0;
extern PendingMsg* lbl_804D4890;      // queued messages
extern PendingMsg* lbl_804D4894;      // free pool
extern PendingMsg lbl_8044F600[64];   // message pool

// Binary search of the animation table: 1 if the state list of `key` holds `state`, 0 if not,
// -1 if the key is missing.
static inline int AnimTableFind(u32 key, u32 state, int unused, AnimTableEntry* table, int lo, int hi,
                                int found)
{
    while (lo <= hi && found == -1) {
        int mid = (lo + hi) / 2;
        u32 k = table[mid].key;

        if (k == key) {
            found = mid;
        } else if (key < k) {
            hi = mid - 1;
        } else {
            lo = mid + 1;
        }
    }
    if (found >= 0) {
        int i;

        table = table + found;
        for (i = 0; i < table->count; i++) {
            if (state == table->states[i]) {
                return 1;
            }
        }
        return 0;
    }
    return -1;
}

// 0x800C5028: does an animation key allow a state?
int ActorAnimState(int key, u32 state)
{
    AnimTableEntry* table = lbl_804D4898;
    int lo = 0;
    int hi = lbl_804D489C;
    int found = -1;

    if (table == 0) {
        return -1;
    }
    return AnimTableFind(key, state, 0, table, lo, hi, found);
}

// TODO (not drafted yet, 0x4 bytes at 0x800C50EC): actor/game routine
extern "C" void fn_800C50EC();

// TODO (not drafted yet, 0x4 bytes at 0x800C50F0): actor/game routine
extern "C" void fn_800C50F0();

// TODO (not drafted yet, 0x9C bytes at 0x800C50F4): actor/game routine
extern "C" void fn_800C50F4();

// TODO (not drafted yet, 0x1C bytes at 0x800C5190): actor/game routine
extern "C" void fn_800C5190();

// TODO (not drafted yet, 0x4 bytes at 0x800C51AC): actor/game routine
extern "C" void fn_800C51AC();

// TODO (not drafted yet, 0x30 bytes at 0x800C51B0): actor/game routine
extern "C" void fn_800C51B0();

// TODO (not drafted yet, 0x30 bytes at 0x800C51E0): actor/game routine
extern "C" void fn_800C51E0();

// TODO (not drafted yet, 0x444 bytes at 0x800C5210): actor/game routine
extern "C" void fn_800C5210();

// TODO (not drafted yet, 0x3C bytes at 0x800C5654): actor/game routine
extern "C" void fn_800C5654();

// TODO (not drafted yet, 0xC bytes at 0x800C5690): actor/game routine
extern "C" void fn_800C5690();

// TODO (not drafted yet, 0x88 bytes at 0x800C569C): actor/game routine
extern "C" void fn_800C569C();

// TODO (not drafted yet, 0xC bytes at 0x800C5724): actor/game routine
extern "C" void fn_800C5724();

// TODO (not drafted yet, 0x168 bytes at 0x800C5730): actor/game routine
extern "C" void fn_800C5730();

// TODO (not drafted yet, 0x88 bytes at 0x800C5898): actor/game routine
extern "C" void fn_800C5898();

// TODO (not drafted yet, 0x124 bytes at 0x800C5920): actor/game routine
extern "C" void fn_800C5920();

// TODO (not drafted yet, 0x128 bytes at 0x800C5A44): actor/game routine
extern "C" void fn_800C5A44();

// TODO (not drafted yet, 0x26C bytes at 0x800C5B6C): actor/game routine
extern "C" void fn_800C5B6C();

// TODO (not drafted yet, 0x28 bytes at 0x800C5DD8): actor/game routine
extern "C" void fn_800C5DD8();

// TODO (not drafted yet, 0x5C bytes at 0x800C5E00): actor/game routine
extern "C" void fn_800C5E00();

// TODO (not drafted yet, 0x10 bytes at 0x800C5E5C): actor/game routine
extern "C" void fn_800C5E5C();

// TODO (not drafted yet, 0x8 bytes at 0x800C5E6C): actor/game routine
extern "C" void fn_800C5E6C();

// TODO (not drafted yet, 0x28 bytes at 0x800C5E74): actor/game routine
extern "C" void fn_800C5E74();

// TODO (not drafted yet, 0x118 bytes at 0x800C5E9C): actor/game routine
extern "C" void fn_800C5E9C();

// TODO (not drafted yet, 0x110 bytes at 0x800C5FB4): actor/game routine
extern "C" void fn_800C5FB4();

// TODO (not drafted yet, 0x13C bytes at 0x800C60C4): actor/game routine
extern "C" void fn_800C60C4();

// TODO (not drafted yet, 0x1A0 bytes at 0x800C6200): actor/game routine
extern "C" void fn_800C6200();

// TODO (not drafted yet, 0x38 bytes at 0x800C63A0): actor/game routine
extern "C" void fn_800C63A0();

// TODO (not drafted yet, 0x3C bytes at 0x800C63D8): actor/game routine
extern "C" void fn_800C63D8();

// TODO (not drafted yet, 0x34 bytes at 0x800C6414): actor/game routine
extern "C" void fn_800C6414();

// TODO (not drafted yet, 0x64 bytes at 0x800C6448): actor/game routine
extern "C" void fn_800C6448();

// TODO (not drafted yet, 0x38 bytes at 0x800C64AC): actor/game routine
extern "C" void fn_800C64AC();

// TODO (not drafted yet, 0x18 bytes at 0x800C64E4): actor/game routine
extern "C" void fn_800C64E4();

// TODO (not drafted yet, 0x14 bytes at 0x800C64FC): actor/game routine
extern "C" void fn_800C64FC();

// TODO (not drafted yet, 0x24 bytes at 0x800C6510): actor/game routine
extern "C" void fn_800C6510();

// TODO (not drafted yet, 0x8 bytes at 0x800C6534): actor/game routine
extern "C" void fn_800C6534();

// TODO (not drafted yet, 0x10 bytes at 0x800C653C): actor/game routine
extern "C" void fn_800C653C();

// TODO (not drafted yet, 0xC bytes at 0x800C654C): actor/game routine
extern "C" void fn_800C654C();

// TODO (not drafted yet, 0x8 bytes at 0x800C6558): actor/game routine
extern "C" void fn_800C6558();

// TODO (not drafted yet, 0x104 bytes at 0x800C6560): actor/game routine
extern "C" void fn_800C6560();

// TODO (not drafted yet, 0xFC bytes at 0x800C6664): actor/game routine
extern "C" void fn_800C6664();

// TODO (not drafted yet, 0xD4 bytes at 0x800C6760): actor/game routine
extern "C" void fn_800C6760();

// TODO (not drafted yet, 0xCC bytes at 0x800C6834): actor/game routine
extern "C" void fn_800C6834();

// TODO (not drafted yet, 0xB8 bytes at 0x800C6900): actor/game routine
extern "C" void fn_800C6900();

// TODO (not drafted yet, 0x130 bytes at 0x800C69B8): actor/game routine
extern "C" void fn_800C69B8();

// TODO (not drafted yet, 0x140 bytes at 0x800C6AE8): actor/game routine
extern "C" void fn_800C6AE8();

// TODO (not drafted yet, 0xE0 bytes at 0x800C6C28): actor/game routine
extern "C" void fn_800C6C28();

// TODO (not drafted yet, 0xCC bytes at 0x800C6D08): actor/game routine
extern "C" void fn_800C6D08();

// TODO (not drafted yet, 0x4 bytes at 0x800C6DD4): actor/game routine
extern "C" void fn_800C6DD4();

// TODO (not drafted yet, 0xB0 bytes at 0x800C6DD8): actor/game routine
extern "C" void fn_800C6DD8();

// TODO (not drafted yet, 0x4 bytes at 0x800C6E88): actor/game routine
extern "C" void fn_800C6E88();

// TODO (not drafted yet, 0xCC bytes at 0x800C6E8C): actor/game routine
extern "C" void fn_800C6E8C();

// TODO (not drafted yet, 0x2C bytes at 0x800C6F58): actor/game routine
extern "C" void fn_800C6F58();

// TODO (not drafted yet, 0xA8 bytes at 0x800C6F84): actor/game routine
extern "C" void fn_800C6F84();

// TODO (not drafted yet, 0x14C bytes at 0x800C702C): actor/game routine
extern "C" void fn_800C702C();

// TODO (not drafted yet, 0xE4 bytes at 0x800C7178): actor/game routine
extern "C" void fn_800C7178();

// TODO (not drafted yet, 0x230 bytes at 0x800C725C): actor/game routine
extern "C" void fn_800C725C();

// TODO (not drafted yet, 0x464 bytes at 0x800C748C): actor/game routine
extern "C" void fn_800C748C();

// TODO (not drafted yet, 0x20 bytes at 0x800C78F0): actor/game routine
extern "C" void fn_800C78F0();

// TODO (not drafted yet, 0x20 bytes at 0x800C7910): actor/game routine
extern "C" void fn_800C7910();

// TODO (not drafted yet, 0x1E4 bytes at 0x800C7930): actor/game routine
extern "C" void fn_800C7930();

// TODO (not drafted yet, 0x48 bytes at 0x800C7B14): actor/game routine
extern "C" void fn_800C7B14();

// TODO (not drafted yet, 0x1C bytes at 0x800C7B5C): actor/game routine
extern "C" void fn_800C7B5C();

// TODO (not drafted yet, 0x170 bytes at 0x800C7B78): actor/game routine
extern "C" void fn_800C7B78();

// TODO (not drafted yet, 0x198 bytes at 0x800C7CE8): actor/game routine
extern "C" void fn_800C7CE8();

// TODO (not drafted yet, 0x318 bytes at 0x800C7E80): actor/game routine
extern "C" void fn_800C7E80();

// TODO (not drafted yet, 0x44 bytes at 0x800C8198): actor/game routine
extern "C" void fn_800C8198();

// TODO (not drafted yet, 0x1B8 bytes at 0x800C81DC): actor/game routine
extern "C" void fn_800C81DC();

// TODO (not drafted yet, 0x5C bytes at 0x800C8394): actor/game routine
extern "C" void fn_800C8394();

// TODO (not drafted yet, 0x4C bytes at 0x800C83F0): actor/game routine
extern "C" void fn_800C83F0();

// TODO (not drafted yet, 0x128 bytes at 0x800C843C): actor/game routine
extern "C" void fn_800C843C();

// TODO (not drafted yet, 0x3E0 bytes at 0x800C8564): actor/game routine
extern "C" void fn_800C8564();

// TODO (not drafted yet, 0x584 bytes at 0x800C8944): actor/game routine
extern "C" void fn_800C8944();

// TODO (not drafted yet, 0xA4 bytes at 0x800C8EC8): actor/game routine
extern "C" void fn_800C8EC8();

// TODO (not drafted yet, 0x4A8 bytes at 0x800C8F6C): actor/game routine
extern "C" void fn_800C8F6C();

// TODO (not drafted yet, 0x6C bytes at 0x800C9414): actor/game routine
extern "C" void fn_800C9414();

// TODO (not drafted yet, 0x64 bytes at 0x800C9480): actor/game routine
extern "C" void fn_800C9480();

// TODO (not drafted yet, 0x308 bytes at 0x800C94E4): actor/game routine
extern "C" void fn_800C94E4();

// TODO (not drafted yet, 0x44 bytes at 0x800C97EC): actor/game routine
extern "C" void fn_800C97EC();

// TODO (not drafted yet, 0x44 bytes at 0x800C9830): actor/game routine
extern "C" void fn_800C9830();

// TODO (not drafted yet, 0x98 bytes at 0x800C9874): actor/game routine
extern "C" void fn_800C9874();

// TODO (not drafted yet, 0x38 bytes at 0x800C990C): actor/game routine
extern "C" void fn_800C990C();

// TODO (not drafted yet, 0x24 bytes at 0x800C9944): actor/game routine
extern "C" void fn_800C9944();

// TODO (not drafted yet, 0x8 bytes at 0x800C9968): actor/game routine
extern "C" void fn_800C9968();

// TODO (not drafted yet, 0x78 bytes at 0x800C9970): actor/game routine
extern "C" void fn_800C9970();

// TODO (not drafted yet, 0x180 bytes at 0x800C99E8): actor/game routine
extern "C" void fn_800C99E8();

// TODO (not drafted yet, 0x88 bytes at 0x800C9B68): actor/game routine
extern "C" void fn_800C9B68();

// TODO (not drafted yet, 0x78 bytes at 0x800C9BF0): actor/game routine
extern "C" void fn_800C9BF0();

// TODO (not drafted yet, 0x6C bytes at 0x800C9C68): actor/game routine
extern "C" void fn_800C9C68();

// TODO (not drafted yet, 0x68 bytes at 0x800C9CD4): actor/game routine
extern "C" void fn_800C9CD4();

// TODO (not drafted yet, 0x194 bytes at 0x800C9D3C): actor/game routine
extern "C" void fn_800C9D3C();

// TODO (not drafted yet, 0x78 bytes at 0x800C9ED0): actor/game routine
extern "C" void fn_800C9ED0();

// TODO (not drafted yet, 0x78 bytes at 0x800C9F48): actor/game routine
extern "C" void fn_800C9F48();

// TODO (not drafted yet, 0xC0 bytes at 0x800C9FC0): actor/game routine
extern "C" void fn_800C9FC0();

// TODO (not drafted yet, 0x130 bytes at 0x800CA080): actor/game routine
extern "C" void fn_800CA080();

// TODO (not drafted yet, 0x998 bytes at 0x800CA1B0): actor/game routine
extern "C" void fn_800CA1B0();

// TODO (not drafted yet, 0xA0 bytes at 0x800CAB48): actor/game routine
extern "C" void fn_800CAB48();

// TODO (not drafted yet, 0x124 bytes at 0x800CABE8): actor/game routine
extern "C" void fn_800CABE8();

// TODO (not drafted yet, 0xD4 bytes at 0x800CAD0C): actor/game routine
extern "C" void fn_800CAD0C();

// TODO (not drafted yet, 0xADC bytes at 0x800CADE0): actor/game routine
extern "C" void fn_800CADE0();

// TODO (not drafted yet, 0x1E4 bytes at 0x800CB8BC): actor/game routine
extern "C" void fn_800CB8BC();

// TODO (not drafted yet, 0x650 bytes at 0x800CBAA0): actor/game routine
extern "C" void fn_800CBAA0();

// TODO (not drafted yet, 0x12C bytes at 0x800CC0F0): actor/game routine
extern "C" void fn_800CC0F0();

// TODO (not drafted yet, 0xB4 bytes at 0x800CC21C): actor/game routine
extern "C" void fn_800CC21C();

// TODO (not drafted yet, 0x38 bytes at 0x800CC2D0): actor/game routine
extern "C" void fn_800CC2D0();

// TODO (not drafted yet, 0x5C4 bytes at 0x800CC308): actor/game routine
extern "C" void fn_800CC308();

// TODO (not drafted yet, 0xB8 bytes at 0x800CC8CC): actor/game routine
extern "C" void fn_800CC8CC();

// TODO (not drafted yet, 0x104 bytes at 0x800CC984): actor/game routine
extern "C" void fn_800CC984();

// TODO (not drafted yet, 0x560 bytes at 0x800CCA88): actor/game routine
extern "C" void fn_800CCA88();

// TODO (not drafted yet, 0x5D0 bytes at 0x800CCFE8): actor/game routine
extern "C" void fn_800CCFE8();

// TODO (not drafted yet, 0x20 bytes at 0x800CD5B8): actor/game routine
extern "C" void fn_800CD5B8();

// TODO (not drafted yet, 0x608 bytes at 0x800CD5D8): actor/game routine
extern "C" void fn_800CD5D8();

// TODO (not drafted yet, 0x130 bytes at 0x800CDBE0): actor/game routine
extern "C" void fn_800CDBE0();

// TODO (not drafted yet, 0x618 bytes at 0x800CDD10): actor/game routine
extern "C" void fn_800CDD10();

// TODO (not drafted yet, 0x210 bytes at 0x800CE328): actor/game routine
extern "C" void fn_800CE328();

// TODO (not drafted yet, 0x224 bytes at 0x800CE538): actor/game routine
extern "C" void fn_800CE538();

// TODO (not drafted yet, 0x210 bytes at 0x800CE75C): actor/game routine
extern "C" void fn_800CE75C();

// TODO (not drafted yet, 0x218 bytes at 0x800CE96C): actor/game routine
extern "C" void fn_800CE96C();

// TODO (not drafted yet, 0x148 bytes at 0x800CEB84): actor/game routine
extern "C" void fn_800CEB84();

// TODO (not drafted yet, 0x13C bytes at 0x800CECCC): actor/game routine
extern "C" void fn_800CECCC();

// TODO (not drafted yet, 0xC0 bytes at 0x800CEE08): actor/game routine
extern "C" void fn_800CEE08();

// TODO (not drafted yet, 0x19C bytes at 0x800CEEC8): actor/game routine
extern "C" void fn_800CEEC8();

// TODO (not drafted yet, 0x18C bytes at 0x800CF064): actor/game routine
extern "C" void fn_800CF064();

// TODO (not drafted yet, 0x48 bytes at 0x800CF1F0): actor/game routine
extern "C" void fn_800CF1F0();

// TODO (not drafted yet, 0x60 bytes at 0x800CF238): actor/game routine
extern "C" void fn_800CF238();

// TODO (not drafted yet, 0x8 bytes at 0x800CF298): actor/game routine
extern "C" void fn_800CF298();

// TODO (not drafted yet, 0x20 bytes at 0x800CF2A0): actor/game routine
extern "C" void fn_800CF2A0();

// TODO (not drafted yet, 0x20 bytes at 0x800CF2C0): actor/game routine
extern "C" void fn_800CF2C0();

// TODO (not drafted yet, 0x4 bytes at 0x800CF2E0): actor/game routine
extern "C" void fn_800CF2E0();

// TODO (not drafted yet, 0x58 bytes at 0x800CF2E4): actor/game routine
extern "C" void fn_800CF2E4();

// TODO (not drafted yet, 0x54 bytes at 0x800CF33C): actor/game routine
extern "C" void fn_800CF33C();

// TODO (not drafted yet, 0x30 bytes at 0x800CF390): actor/game routine
extern "C" void fn_800CF390();

// TODO (not drafted yet, 0xF4 bytes at 0x800CF3C0): actor/game routine
extern "C" void fn_800CF3C0();

// TODO (not drafted yet, 0x460 bytes at 0x800CF4B4): actor/game routine
extern "C" void fn_800CF4B4();

// TODO (not drafted yet, 0x224 bytes at 0x800CF914): actor/game routine
extern "C" void fn_800CF914();

// TODO (not drafted yet, 0x40 bytes at 0x800CFB38): actor/game routine
extern "C" void fn_800CFB38();

// TODO (not drafted yet, 0x100 bytes at 0x800CFB78): actor/game routine
extern "C" void fn_800CFB78();

// TODO (not drafted yet, 0x8 bytes at 0x800CFC78): actor/game routine
extern "C" void fn_800CFC78();

// TODO (not drafted yet, 0x104 bytes at 0x800CFC80): actor/game routine
extern "C" void fn_800CFC80();

// TODO (not drafted yet, 0x120 bytes at 0x800CFD84): actor/game routine
extern "C" void fn_800CFD84();

// TODO (not drafted yet, 0x80 bytes at 0x800CFEA4): actor/game routine
extern "C" void fn_800CFEA4();

// TODO (not drafted yet, 0x8 bytes at 0x800CFF24): actor/game routine
extern "C" void fn_800CFF24();

// TODO (not drafted yet, 0x2C bytes at 0x800CFF2C): actor/game routine
extern "C" void fn_800CFF2C();

// TODO (not drafted yet, 0x140 bytes at 0x800CFF58): actor/game routine
extern "C" void fn_800CFF58();

// TODO (not drafted yet, 0x7C bytes at 0x800D0098): actor/game routine
extern "C" void fn_800D0098();

// TODO (not drafted yet, 0x24 bytes at 0x800D0114): actor/game routine
extern "C" void fn_800D0114();

// TODO (not drafted yet, 0xEC bytes at 0x800D0138): actor/game routine
extern "C" void fn_800D0138();

// TODO (not drafted yet, 0x8 bytes at 0x800D0224): actor/game routine
extern "C" void fn_800D0224();

// TODO (not drafted yet, 0x28 bytes at 0x800D022C): actor/game routine
extern "C" void fn_800D022C();
