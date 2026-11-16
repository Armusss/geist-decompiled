// u_8016BE58.cpp: SLT effect list and its assets (0x8016BE58-0x8016C244), split from u_8016A774.cpp (the
// SLT loader): it owns its own .data list, .sdata asset ids and .sdata2 constants (2900.0f, 100.0f).
// Two SLT slots each bind a level-load request to an entry of a loaded SLT file and run the
// entry's sub-entries (A = asset, R = resident/effect, M = model). A small effect list of
// 0x34-byte nodes lives next to it and is stepped by SltEffectUpdate.
// Names are descriptive guesses; the code is matched against the retail DOL.

typedef unsigned long u32;
typedef unsigned char u8;
typedef signed char s8;
typedef unsigned short u16;
typedef short s16;

struct Actor;

void DebugPrintf(const char* format, ...);                              // 0x8000781C
void FileAddHandler(int id, int (*handler)(void*, int, void*), void* arg); // 0x8001950C
int FontLoad(int id, const char* name, int a, int b, int c, int d);     // 0x8007F004
void FontRelease(int font);                                             // 0x8007EEA4
int AssetFind(u32 bank, u32 name);                                      // 0x8007EB34
void TextureLoad(short* ids, int count);                                // 0x8000F738
void LevelLoadNames(const char* name, int* a, char* b, char* c, char* d); // 0x80015A44
int ActorPlayAnim(int actor, u32 anim, int arg);                        // 0x800C5654
void EmitterDestroy(int handle);                                        // 0x800D3694
void ActorRender(Actor* actor);                                         // 0x800C7CE8
int PadPressed(int pad, int a, int button);                             // 0x800DC930
int Fn800816B0(void);                                                   // 0x800816B0
void AnimTrackSelect(int id, int offset);                               // 0x80081008
void SAMSetStartDelay(float delay);                                     // 0x800E0554
int SAMPlay(int track, u32 doneArg, long preload);                       // 0x800E05E0
void SAMNop(s16 id, u32 arg);                                           // 0x800E05A4
void SAMReset(int id);                                                  // 0x800E05A8

extern "C" {
int sprintf(char* dst, const char* format, ...);
int SendMsg(int to, int msg, void* data);                               // 0x800265D4
extern unsigned char __upper_map[];                                              // 0x803C6B08

int fn_800059D0(int a);                                                 // 0x800059D0
void fn_800070C0(void* p);                                              // 0x800070C0
void fn_80007118(void* p);                                              // 0x80007118
void fn_80026660(int msg, int (*handler)(int, int, void*), int arg);    // 0x80026660
void fn_8003BA70(s16 id, u32 arg);                                      // 0x8003BA70
void fn_8003C308(int id);                                               // 0x8003C308
int fn_8003C38C(int id, char* name, u32 bind, int count, void* subs, u32 arg, int unk); // 0x8003C38C
int fn_8003C48C(char* name, u32 bind, int count, void* subs, u32 arg, int flags, int id); // 0x8003C48C
void fn_80081D0C(char* name, int a, int b, int c);                      // 0x80081D0C
int fn_80082874(int id, u32 handle, u32 key);                           // 0x80082874
int fn_802810BC(u32 key);                                               // 0x802810BC
void fn_80281124(u32 key);                                              // 0x80281124
void fn_802A1FA0(void* p);                                              // 0x802A1FA0

void* fn_80187108(void* list, void* it);                                // 0x80187108
int fn_80187148(void* list, void* it);                                  // 0x80187148
void fn_80187188(void* list, void* it);                                 // 0x80187188
void fn_801871E0(void* list, void* it);                                 // 0x801871E0
void* fn_80187214(void* list, void* node);                              // 0x80187214
void fn_8018721C(void* list, void* node);                               // 0x8018721C
}
void EmitterSetEnabled(u32 handle, int a);

// A sub-entry of an SLT entry (0x0C bytes).
struct SltSub {
    s8 type;   // 0x00  'A' asset, 'R' resident/effect
    s8 unk1;   // 0x01
    u16 unk2;  // 0x02
    u16 unk4;  // 0x04
    u16 unk6;  // 0x06
    u16 unk8;  // 0x08
    u16 unkA;  // 0x0A
};

// One entry of an SLT file (0x34 bytes).
struct SltEntry {
    u32 key;         // 0x00
    u32 bind;        // 0x04
    s8 count;        // 0x08
    char pad09[3];
    SltSub subs[3];  // 0x0C
    u32 arg;         // 0x30
};

// A loaded SLT file.
struct SltFile {
    int type;      // 0x00  must be 4
    int count;     // 0x04
    char name[1];  // 0x08
};

struct SltGroup {
    int valid;         // 0x00
    SltFile* file;     // 0x04
    SltEntry* entries; // 0x08
};

// One of the two SLT slots (0x34 bytes).
struct SltSlot {
    SltEntry* ptr;  // 0x00
    s16 id;         // 0x04
    u16 flags;      // 0x06
    u8 stamp;       // 0x08
    char pad09[3];
    u32 handle;     // 0x0C
    u32 unk10;      // 0x10
    u32 unk14;      // 0x14
    u32 unk18;      // 0x18
    u32 unk1C;      // 0x1C
    u32 unk20;      // 0x20
    int unk24;      // 0x24
    int unk28;      // 0x28
    int unk2C;      // 0x2C
    u32 unk30;      // 0x30
};

// Message payloads.
struct SltIdMsg {
    int unk0;   // 0x00
    u32 id;     // 0x04
    int unk8;   // 0x08
    int unkC;   // 0x0C
};

struct SltLoadMsg {
    int unk0;   // 0x00
    u32 id;     // 0x04
    u32 key;    // 0x08
    int unkC;   // 0x0C
};

struct SltCreateMsg {
    int unk0;    // 0x00
    u32 key;     // 0x04
    int notify;  // 0x08
    u32 notifyArg; // 0x0C
    u32 flags;   // 0x10
    u32 handle;  // 0x14
    u32 unk18;   // 0x18
    u32 unk1C;   // 0x1C
    u32 unk20;   // 0x20
    u32 unk24;   // 0x24
};

struct SltAnimMsg {
    int unk0;  // 0x00
    u32 a;     // 0x04
    u32 b;     // 0x08
};

// One node of the SLT effect list (0x34 bytes).
struct SltEffect {
    int state;     // 0x00
    int unk04;     // 0x04
    int unk08;     // 0x08
    int unk0C;     // 0x0C
    int unk10;     // 0x10
    int unk14;     // 0x14
    int unk18;     // 0x18
    int unk1C;     // 0x1C
    int unk20;     // 0x20
    int unk24;     // 0x24
    int unk28;     // 0x28
    float timer;   // 0x2C
    int unk30;     // 0x30
};

struct SltEffectIter {
    void* cur;   // 0x00
    void* next;  // 0x04
};

struct SltEffectList {
    void* head;  // 0x00
    void* tail;  // 0x04
    u32 count;   // 0x08
    int unk0C;   // 0x0C
    int unk10;   // 0x10
    int unk14;   // 0x14
};

// Unit data.

int sAssetA = -1;      // 0x804D2FB8
int sAssetB = -1;      // 0x804D2FBC


int SltMsgHandler(int actor, int msg, void* data);
int SltUpdate(void);
void SltGroupReset(void);
void SltInit(void);
void SltUnloadAll(void);
int SltUnload(SltIdMsg* msg);
int SltLoad(SltLoadMsg* msg);
int SltCreate(SltCreateMsg* msg, int* index);
int SltLoadIndex(int index, int arg);
int SltLoadData(SltEntry* entry, int group, u32 handle, u32 flags);
int SltRelease(int handle);
void SltDrawEntry(int slot);
int SltFileHandler(void* fileData, int size, void* out);
void SltEffectKillAll(void);
void SltEffectUpdate(float dt);
void SltAssetInit(void);
void SltEffectCleanup(void);

inline int toupper(int c) { return (c == -1) ? -1 : (int)__upper_map[(unsigned char)c]; }

inline u32 StrHash(const char* s) {
    int i;
    u32 h;

    if (s == 0) {
        return 0;
    }
    h = 0;
    for (i = 0; s[i] != 0; i++) {
        h = h * 31 + (u8)toupper(s[i]);
    }
    h %= 0xFFFFFFFF;
    return h;
}

SltEffectList sEffects = { 0, 0, 0, 0, 0, 0 }; // 0x803A7C60

// 0x8016BE58
void SltEffectKillAll(void) {
    while (sEffects.count != 0) {
        SltEffect* e = (SltEffect*)sEffects.head;
        if (e != 0) {
            SltEffect* p = (SltEffect*)fn_80187214(&sEffects, e);
            if (p != 0) {
                EmitterDestroy(p->unk04);
                EmitterDestroy(p->unk08);
                EmitterDestroy(p->unk24);
                ActorRender((Actor*)p->unk30);
            }
            fn_8018721C(&sEffects, e);
        }
    }
}

// 0x8016BEF8
void SltEffectUpdate(float dt) {
    SltEffectIter it;

    fn_801871E0(&sEffects, &it);
    while (fn_80187148(&sEffects, &it) != 0) {
        SltEffect* e = (SltEffect*)fn_80187108(&sEffects, &it);
        switch (e->state) {
        case 0:
            break;
        case 1:
            if (e->timer <= dt) {
                e->timer = 2900.0f;
                e->state = 2;
            } else {
                e->timer -= dt;
            }
            break;
        case 2:
            if (e->timer <= dt) {
                EmitterSetEnabled(e->unk04, 0);
                EmitterSetEnabled(e->unk08, 0);
                EmitterSetEnabled(e->unk0C, 0);
                EmitterSetEnabled(e->unk10, 0);
                EmitterSetEnabled(e->unk14, 0);
                EmitterSetEnabled(e->unk18, 0);
                EmitterSetEnabled(e->unk1C, 0);
                EmitterSetEnabled(e->unk20, 0);
                EmitterSetEnabled(e->unk24, 0);
                e->timer = 100.0f;
                e->state = 3;
            } else {
                e->timer -= dt;
            }
            break;
        case 3:
            if (e->timer <= dt) {
                e->state = 0;
            } else {
                e->timer -= dt;
            }
            break;
        }
        if (e->state == 0) {
            void* cur = it.cur;
            fn_80187188(&sEffects, &it);
            if (cur != 0) {
                SltEffect* p = (SltEffect*)fn_80187214(&sEffects, cur);
                if (p != 0) {
                    EmitterDestroy(p->unk04);
                    EmitterDestroy(p->unk08);
                    EmitterDestroy(p->unk24);
                    ActorRender((Actor*)p->unk30);
                }
                fn_8018721C(&sEffects, cur);
            }
        } else {
            fn_80187188(&sEffects, &it);
        }
    }
}

// 0x8016C120
void SltAssetInit(void) {
    sAssetB = AssetFind(0xF2E2BEB8, 0xCD678095);
    sAssetA = AssetFind(0xF2E2BEB8, 0x703C8B5F);
    s16 ids[2];
    ids[0] = (s16)sAssetB;
    ids[1] = (s16)sAssetA;
    TextureLoad(ids, 2);
    AssetFind(0x87DE5E6A, 0x555B96F3);
}

// 0x8016C198
void SltEffectCleanup(void) {
    if (sEffects.count != 0) {
        while (sEffects.count != 0) {
            SltEffect* e = (SltEffect*)sEffects.head;
            if (e != 0) {
                SltEffect* p = (SltEffect*)fn_80187214(&sEffects, e);
                if (p != 0) {
                    EmitterDestroy(p->unk04);
                    EmitterDestroy(p->unk08);
                    EmitterDestroy(p->unk24);
                    ActorRender((Actor*)p->unk30);
                }
                fn_8018721C(&sEffects, e);
            }
        }
    }
}
