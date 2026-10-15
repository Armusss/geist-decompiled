// A one-function unit (0x80085CDC-0x80085E50): builds a list of link infos at a position and hands
// them, with a blank descriptor, to the builder for one of the global link tables.
// Names are descriptive guesses; the code is matched against the retail DOL.

#include <dolphin/types.h>

struct LinkInfo {
    u8 pad[0x24];
    float x, y, z;
};

struct LinkDesc {
    int words[15];
};

struct LinkTable {
    int a;
    int count;
    float f;
};

extern "C" {
// Callees in other files, not named yet.
void* fn_800C6448(void);
void fn_8003F15C(int a, LinkInfo* links, int count, int arg, LinkDesc* desc, float f);
}

void LinkInfoInit(void* info);

extern LinkTable lbl_80440AA0[];

void LinkTableBuild(int index, int arg) {
    LinkDesc desc = {0};
    LinkInfo links[254];
    u8* obj = (u8*)fn_800C6448();
    float x = *(float*)(obj + 0x24);
    float y = *(float*)(obj + 0x28);
    float z = *(float*)(obj + 0x2C);
    int i;

    for (i = 0; i < lbl_80440AA0[index].count; i++) {
        LinkInfoInit(&links[i]);
        links[i].x = x;
        links[i].y = y;
        links[i].z = z;
    }
    fn_8003F15C(lbl_80440AA0[index].a, links, lbl_80440AA0[index].count, arg, &desc, lbl_80440AA0[index].f);
}
