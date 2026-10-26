// u_8016A774.cpp: the seamless-level-transition (SLT) manager (0x8016A774-0x8016C244).
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

extern "C" {
int sprintf(char* dst, const char* format, ...);
int SendMsg(int to, int msg, void* data);                               // 0x800265D4
extern char __upper_map[];                                              // 0x803C6B08

int fn_800059D0(int a);                                                 // 0x800059D0
void fn_800070C0(void* p);                                              // 0x800070C0
void fn_80007118(void* p);                                              // 0x80007118
void fn_80026660(int msg, int (*handler)(int, int, void*), int arg);    // 0x80026660
void fn_8003BA70(s16 id, u32 arg);                                      // 0x8003BA70
void fn_8003C308(int id);                                               // 0x8003C308
int fn_8003C38C(int id, char* name, u32 bind, int count, void* subs, u32 arg, int unk); // 0x8003C38C
int fn_8003C48C(char* name, u32 bind, int count, void* subs, u32 arg, int flags, int id); // 0x8003C48C
void fn_80081008(u32 handle);                                           // 0x80081008
void fn_80081D0C(char* name, int a, int b, int c);                      // 0x80081D0C
int fn_80082874(int id, u32 handle, u32 key);                           // 0x80082874
void fn_800E0554(float a);                                              // 0x800E0554
void fn_800E05A4(s16 id, u32 arg);                                      // 0x800E05A4
void fn_800E05A8(int id);                                               // 0x800E05A8
int fn_800E05E0(int id, u32 handle, u32 flags);                         // 0x800E05E0
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
    void* prev;  // 0x08
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
u32 sLoadedMask;   // 0x804D4F20  one bit per in-use slot
u32 sInputLock;    // 0x804D4F24
u32 sHandleSeq;    // 0x804D4F28
s8 sLBoxOn;        // 0x804D4F2C

int sFontId = -1;      // 0x804D2FB0
int sCurrentSlot = -1; // 0x804D2FB4
int sAssetA = -1;      // 0x804D2FB8
int sAssetB = -1;      // 0x804D2FBC

SltGroup sGroups[2];            // 0x8046E3E8
SltSlot sSlots[2];              // 0x8046E400
SltEffectList sEffects = { 0, 0, 0, 0, 0, 0 }; // 0x803A7C60

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
int SltRelease(u32 handle);
void SltDrawEntry(int slot);
int SltFileHandler(void* fileData, int size, void* out);
void SltEffectKillAll(void);
void SltEffectUpdate(float dt);
void SltAssetInit(void);
void SltEffectCleanup(void);

// 0x8016A774
static inline int SltFindEntry(SltGroup* grp, u32 key) {
    int i;

    for (i = 0; i < grp->file->count; i++) {
        if (grp->entries[i].key == key) {
            return i;
        }
    }
    return -1;
}

static inline void SltUnloadEach(void) {
    SltIdMsg unload;
    int i;

    i = 0;
    unload.unk0 = 0;
    for (; i < 2; i++) {
        if (sLoadedMask & (1 << i)) {
            unload.id = sSlots[i].handle;
            SltUnload(&unload);
        }
    }
}

static inline int SltQueryRumble(SltLoadMsg* m) {
    int found;
    int i;

    found = 0;
    if (m == 0) {
        return found;
    }
    for (i = 0; i < 2; i++) {
        if (sSlots[i].ptr != 0) {
            for (int j = 0; j < sSlots[i].ptr->count; j++) {
                if (sSlots[i].ptr->subs[j].type == 'R') {
                    found = 1;
                }
            }
        }
        if (found == 1) {
            break;
        }
    }
    if (found == 0) {
        m->id = 0xFFFFFFFF;
    } else {
        m->id = Fn800816B0();
        m->key = sSlots[i].unk14;
    }
    return 1;
}

static inline int SltFadeOut(SltLoadMsg* m) {
    u32 arg = m->key;
    u32 id = m->id;

    if (id != 0xFFFFFFFF) {
        int slot = (id >> 16) & 0xF;
        if (slot < 0 || slot >= 2) {
            DebugPrintf("SLTFadeOut: Warning - invalid sltId\n");
            return 0;
        }
        if ((id >> 20) != sSlots[slot].stamp) {
            DebugPrintf("SLTFadeOut: Warning - invalid timestamp\n");
            return 0;
        }
        id = sSlots[slot].id;
    }
    fn_8003BA70(id, arg);
    fn_800E05A4(id, arg);
    return 1;
}

int SltMsgHandler(int actor, int msg, void* data) {
    int result = 1;

    switch (msg) {
    case 0x3004:
        result = SltCreate((SltCreateMsg*)data, 0);
        break;
    case 0x3007:
        result = SltUnload((SltIdMsg*)data);
        break;
    case 0x3008:
        SltUnloadEach();
        break;
    case 0x3009:
        result = SltLoad((SltLoadMsg*)data);
        break;
    case 0x300A:
        result = SltQueryRumble((SltLoadMsg*)data);
        break;
    case 0x300C:
        result = SltFadeOut((SltLoadMsg*)data);
        break;
    default:
        result = 0;
        break;
    }
    return result;
}

// 0x8016A9B8
int SltUpdate(void) {
    int pressed = 0;
    int result = 0;

    if (sInputLock == 0) {
        if (PadPressed(0, 0, 2) == 2) {
            pressed = 1;
        }
    }
    if (pressed) {
        for (int i = 0; i < 2; i++) {
            if (sSlots[i].ptr != 0 && pressed) {
                if (fn_802810BC(sSlots[i].unk20) != 0) {
                    if ((sSlots[i].flags & 0x200) == 0 && (sSlots[i].flags & 0x400) == 0) {
                        for (int j = 0; j < sSlots[i].ptr->count; j++) {
                            if (sSlots[i].ptr->subs[j].type == 'R') {
                                if ((sSlots[i].flags & 4) == 0 && (sSlots[i].flags & 0x80) == 0) {
                                    if (Fn800816B0() > 0) {
                                        result = 1;
                                        sSlots[i].flags |= 0x200;
                                        fn_80281124(sSlots[i].unk20);
                                    }
                                }
                            }
                        }
                    }
                }
            }
        }
        if (result != 0) {
            ActorPlayAnim(0x4E000000, 0x1284E9A1, 0x2000000);
        }
    } else {
        if (fn_800059D0(0x15) != 2) {
            sInputLock = 0;
        }
    }

    for (int i = 0; i < 2; i++) {
        if (sSlots[i].ptr != 0) {
            if (sSlots[i].flags & 0x400) {
                SltIdMsg unload;
                sInputLock = 1;
                unload.unk0 = 0;
                for (int k = 0; k < 2; k++) {
                    if (sLoadedMask & (1 << k)) {
                        unload.id = sSlots[k].handle;
                        SltUnload(&unload);
                    }
                }
                sSlots[i].flags &= ~0x200;
                sSlots[i].flags &= ~0x400;
            } else if (result == 0 && (sSlots[i].flags & 0x200)) {
                sSlots[i].flags &= ~0x200;
                sSlots[i].flags |= 0x400;
            } else {
                for (int j = 0; j < sSlots[i].ptr->count; j++) {
                    if (sSlots[i].ptr->subs[j].type == 'R') {
                        if (Fn800816B0() > sSlots[i].unk14) {
                            u32 a = sSlots[i].unk24;
                            u32 b = sSlots[i].unk2C;
                            u32 c = sSlots[i].unk20;
                            fn_80081008(a);
                            sSlots[i].unk10 += sSlots[i].unk18;
                            sSlots[i].unk14 = sSlots[i].unk1C;
                            sSlots[i].unk20 = sSlots[i].unk24;
                            sSlots[i].unk24 = 0;
                            sSlots[i].unk18 = 0;
                            sSlots[i].unk1C = 0;
                            if (b != 0xFFFFFFFF) {
                                SltAnimMsg m;
                                m.a = c;
                                m.b = a;
                                SendMsg(b, 0x3006, &m);
                            }
                        }
                        break;
                    }
                }
            }
        }
    }
    return result;
}

// 0x8016AC90
void SltGroupReset(void) {
    sGroups[0].valid = 0;
    sGroups[1].valid = 0;
}

// 0x8016ACA4
void SltInit(void) {
    sSlots[0].ptr = 0;
    sSlots[0].id = -1;
    sSlots[0].stamp = 0;
    sSlots[0].flags = 0;
    sSlots[0].handle = 0;
    sSlots[0].unk10 = 0;
    sSlots[0].unk14 = 0;
    sSlots[0].unk18 = 0;
    sSlots[0].unk1C = 0;
    sSlots[0].unk20 = 0;
    sSlots[0].unk24 = 0;
    sSlots[0].unk28 = 0;
    sSlots[0].unk2C = -1;
    sSlots[0].unk30 = 0;

    sSlots[1].ptr = 0;
    sSlots[1].id = -1;
    sSlots[1].stamp = 0;
    sSlots[1].flags = 0;
    sSlots[1].handle = 0;
    sSlots[1].unk10 = 0;
    sSlots[1].unk14 = 0;
    sSlots[1].unk18 = 0;
    sSlots[1].unk1C = 0;
    sSlots[1].unk20 = 0;
    sSlots[1].unk24 = 0;
    sSlots[1].unk28 = 0;
    sSlots[1].unk2C = -1;
    sGroups[0].valid = 0;
    sGroups[1].valid = 0;
    sSlots[1].unk30 = 0;

    sHandleSeq = 0;
    sLoadedMask = 0;

    FileAddHandler(0x18, SltFileHandler, &sGroups[0]);
    FileAddHandler(0x19, SltFileHandler, &sGroups[1]);
    fn_80026660(0x2D, SltMsgHandler, 0);
    sFontId = -1;
}

// 0x8016ADA4
void SltUnloadAll(void) {
    SltIdMsg msg;

    msg.unk0 = 0;
    for (int i = 0; i < 2; i++) {
        if (sLoadedMask & (1 << i)) {
            msg.id = sSlots[i].handle;
            SltUnload(&msg);
        }
    }
}

// 0x8016AE20
int SltUnload(SltIdMsg* msg) {
    int group;

    if (msg == 0) {
        return 0;
    }
    group = (msg->id >> 12) & 0xF;
    if (!sGroups || sGroups[group].file == 0) {
        return 0;
    }

    char bufC[0x100];
    char bufD[0x100];
    char path[0x400];
    SltIdMsg out;

    LevelLoadNames(sGroups[group].file->name, 0, 0, bufD, bufC);
    sprintf(path, "/%s/%s%s", bufD, bufD, bufC);
    if (msg == 0) {
        return 0;
    }

    u32 id = msg->id;
    int slot = (id >> 16) & 0xF;
    int stamp = id >> 20;
    if (stamp != sSlots[slot].stamp) {
        return 0;
    }
    if (sSlots[slot].flags & 2) {
        return 0;
    }

    fn_8003C308(sSlots[slot].id);

    if (sLBoxOn == 1) {
        sLBoxOn = 0;
        if (sFontId != -1) {
            FontRelease(sFontId);
            sFontId = FontLoad(1, "LBox Off", 8, 0, 0, 0);
        }
    }

    if (sSlots[slot].ptr != 0) {
        for (int i = 0; i < sSlots[slot].ptr->count; i++) {
            switch (sSlots[slot].ptr->subs[i].type) {
            case 'R':
                fn_80081D0C(path, sSlots[slot].ptr->subs[i].unk4,
                            sSlots[slot].ptr->subs[i].unk1, 1);
                break;
            case 'M':
                break;
            case 'A':
                fn_800E05A8(sSlots[slot].id);
                break;
            }
        }
    }

    int target = sSlots[slot].unk2C;
    int arg = sSlots[slot].unk30;
    sSlots[slot].ptr = 0;
    sLoadedMask &= ~(1 << slot);
    sSlots[slot].id = -1;
    sSlots[slot].stamp = 0;
    sSlots[slot].flags = 0;
    sSlots[slot].handle = 0;
    sSlots[slot].unk10 = 0;
    sSlots[slot].unk14 = 0;
    sSlots[slot].unk18 = 0;
    sSlots[slot].unk1C = 0;
    sSlots[slot].unk20 = 0;
    sSlots[slot].unk24 = 0;
    sSlots[slot].unk28 = 0;
    sSlots[slot].unk2C = -1;
    sSlots[slot].unk30 = 0;
    sSlots[slot].id = -1;
    out.id = arg;
    sSlots[slot].ptr = 0;
    if (target != -1) {
        SendMsg(target, 0x3005, &out);
    }
    return 1;
}

// 0x8016B094
int SltLoad(SltLoadMsg* msg) {
    int group = (msg->id >> 12) & 0xF;

    fn_802A1FA0(msg);
    if (!sGroups || sGroups[group].file == 0) {
        return 0;
    }

    char bufB[0x100];
    char bufD[0x100];
    char bufC[0x100];
    char path[0x400];
    int unkArg;

    LevelLoadNames(sGroups[group].file->name, &unkArg, bufB, bufC, bufD);
    sprintf(path, "/%s/%s%s", bufC, bufC, bufD);
    if (msg == 0) {
        return 0;
    }

    int slot = (msg->id >> 16) & 0xF;
    if ((msg->id >> 20) != sSlots[slot].stamp) {
        return 0;
    }

    for (int g = 0; g < 2; g++) {
        if (!sGroups[g].valid) {
            continue;
        }
        int index = SltFindEntry(&sGroups[g], msg->key);
        if (index == -1) {
            continue;
        }

        SltEntry* entry = &sGroups[g].entries[index];
        if (fn_8003C38C(sSlots[slot].id, path, entry->bind, entry->count, entry->subs,
                        entry->arg, 0) == -1) {
            return 0;
        }
        for (int i = 0; i < entry->count; i++) {
            if (entry->subs[i].type == 'R') {
                sSlots[slot].unk24 = msg->key;
                sSlots[slot].unk18 = entry->subs[i].unk2;
                sSlots[slot].unk1C = entry->subs[i].unk6;
                break;
            }
        }
        return 1;
    }
    return 1;
}

// 0x8016B2BC
int SltCreate(SltCreateMsg* msg, int* index) {
    int result = 0;

    if (msg == 0) {
        return 0;
    }

    int slot;
    if (index != 0) {
        slot = *index;
    } else {
        if (sCurrentSlot >= 0) {
            if (msg->key == sSlots[sCurrentSlot].unk20 && (msg->flags & 0x80) == 0) {
                result = 0x100;
                slot = 0;
            } else {
                fn_8003C308(sSlots[sCurrentSlot].id);
                slot = 2;
                sLoadedMask &= ~1;
            }
            sCurrentSlot = -1;
        } else {
            slot = 2;
        }
        if (slot == 2) {
            slot = 0;
            for (int n = 2; n != 0; n--) {
                if ((sLoadedMask & (1 << slot)) == 0) {
                    sLoadedMask |= 1 << slot;
                    break;
                }
                slot++;
            }
        }
        if (slot == 2) {
            return -1;
        }
    }

    sSlots[slot].unk28 = 0;

    u32 flags = msg->flags;
    if (flags & 0x80) {
        result |= 0x80;
    }
    if (flags & 1) {
        result |= 1;
    }
    if (flags & 8) {
        result |= 8;
    }
    if (flags & 0x10) {
        result |= 0x10;
    }
    if (flags & 0x40) {
        sLBoxOn = 1;
    }

    for (int g = 0; g < 2; g++) {
        if (!sGroups[g].valid) {
            continue;
        }
        int found = -1;
        for (int k = 0; k < sGroups[g].file->count; k++) {
            if (sGroups[g].entries[k].key == msg->key) {
                found = k;
                break;
            }
        }
        if (found == -1) {
            continue;
        }

        if ((result & 0x100) == 0) {
            sHandleSeq = (sHandleSeq + 1) & 0xFFF;
        }
        u32 handle = (sHandleSeq << 20) | (slot << 16) | (g << 12) | found;
        msg->handle = handle;
        sSlots[slot].flags = msg->flags;
        sSlots[slot].handle = handle;
        sSlots[slot].stamp = sHandleSeq;
        sSlots[slot].ptr = &sGroups[g].entries[found];
        if (SltLoadData(sSlots[slot].ptr, g, msg->handle, result) != 0) {
            if ((result & 0x100) == 0) {
                sSlots[slot].unk20 = 0;
                sSlots[slot].unk24 = 0;
                sSlots[slot].unk10 = 0;
                sSlots[slot].unk18 = 0;
                sSlots[slot].unk1C = 0;
                SltEntry* entry = sSlots[slot].ptr;
                for (int i = 0; i < entry->count; i++) {
                    if (entry->subs[i].type == 'R') {
                        sSlots[slot].unk20 = msg->key;
                        sSlots[slot].unk10 = entry->subs[i].unk2;
                        sSlots[slot].unk14 = entry->subs[i].unk6;
                    }
                }
            }
            sSlots[slot].unk2C = msg->notify;
            sSlots[slot].unk30 = msg->notifyArg;
            if (msg->flags & 0x20) {
                if (sFontId != -1) {
                    FontRelease(sFontId);
                    sFontId = -1;
                }
                sFontId = FontLoad(1, "LBox On", 8, 0, 0, 0);
            }
            return msg->handle;
        }
    }

    sSlots[slot].id = -1;
    sLoadedMask &= ~(1 << slot);
    sSlots[slot].ptr = 0;
    if (msg->notify != -1) {
        SltIdMsg m;
        m.id = msg->notifyArg;
        SendMsg(msg->notify, 0x3005, &m);
    }
    return 0;
}

// 0x8016B688
int SltLoadIndex(int index, int arg) {
    if (!sGroups || sGroups[0].file == 0) {
        return 0;
    }
    if (index < sGroups[0].file->count) {
        SltCreateMsg msg;
        msg.key = sGroups[0].entries[index].key;
        msg.flags = arg;
        msg.notify = -1;
        msg.unk0 = 0;
        msg.notifyArg = 0;
        SltCreate(&msg, 0);
        return 1;
    }
    return 0;
}

// 0x8016B710
int SltLoadData(SltEntry* entry, int group, u32 handle, u32 flags) {
    int sltId = -1;

    if (!sGroups || sGroups[group].file == 0) {
        return 0;
    }

    char bufC[0x100];
    char bufD[0x100];
    char bufB[0x100];
    char path[0x400];
    int unkArg;

    LevelLoadNames(sGroups[group].file->name, &unkArg, bufB, bufC, bufD);
    sprintf(path, "/%s/%s%s", bufC, bufC, bufD);

    u8 data = 0;
    if (flags & 1) {
        data |= 1;
    }
    if (flags & 8) {
        data |= 2;
    }
    if (flags & 0x10) {
        data |= 4;
    }
    u32 mask80 = flags & 0x80;
    if (mask80) {
        data |= 8;
    }
    if (flags & 0x100) {
        data |= 0x10;
        sltId = sSlots[0].id;
    }

    int id = fn_8003C48C(path, entry->bind, entry->count, entry->subs, entry->arg, data, sltId);
    if ((s16)id == -1) {
        return 0;
    }

    int i;
    for (i = 0; i < entry->count; i++) {
        SltSub* sub = &entry->subs[i];
        switch (sub->type) {
        case 'A':
            if (fn_800E05E0(id, handle, mask80) == 0) {
                fn_8003C308(id);
                return 0;
            }
            {
                u32 hash = 0;
                for (char* p = "01B_BossIntro"; *p != 0; p++) {
                    u32 c = ((s8)*p == -1) ? 0xFFFFFFFF : (u8)__upper_map[(u8)*p];
                    hash = hash * 0x1F + (c & 0xFF);
                }
                float f = 0.0f;
                if (entry->key == hash % 0xFFFFFFFF) {
                    f = 500.0f;
                }
                fn_800E0554(f);
            }
            break;
        case 'R':
            if (mask80 == 0 && fn_80082874(id, handle, entry->key) == 0) {
                fn_8003C308(id);
                return 0;
            }
            break;
        }
    }

    int slot = (i >> 16) & 0xF;
    sSlots[slot].id = id;
    if (mask80) {
        sCurrentSlot = slot;
    }
    return 1;
}

// 0x8016B998
int SltRelease(int handle) {
    if (handle == -1) {
        return 0;
    }
    int slot = (handle >> 16) & 0xF;
    if (slot < 0 || slot >= 2) {
        return 0;
    }
    if (sSlots[slot].unk24 != 0) {
        return 0;
    }

    sSlots[slot].unk28++;
    fn_80281124(sSlots[slot].unk20);
    if (sSlots[slot].id != -1 && sSlots[slot].unk28 == sSlots[slot].ptr->count) {
        fn_8003C308(sSlots[slot].id);

        if (sLBoxOn == 1) {
            sLBoxOn = 0;
            if (sFontId != -1) {
                FontRelease(sFontId);
                sFontId = FontLoad(1, "LBox Off", 8, 0, 0, 0);
            }
        }

        for (int i = 0; i < sSlots[slot].ptr->count; i++) {
            switch (sSlots[slot].ptr->subs[i].type) {
            case 'R':
                SltDrawEntry(slot);
                break;
            case 'M':
                break;
            case 'A':
                fn_800E05A8(sSlots[slot].id);
                break;
            }
        }

        int target = sSlots[slot].unk2C;
        int arg = sSlots[slot].unk30;
        sSlots[slot].ptr = 0;
        sLoadedMask &= ~(1 << slot);
        sSlots[slot].id = -1;
        sSlots[slot].stamp = 0;
        sSlots[slot].flags = 0;
        sSlots[slot].handle = 0;
        sSlots[slot].unk10 = 0;
        sSlots[slot].unk14 = 0;
        sSlots[slot].unk18 = 0;
        sSlots[slot].unk1C = 0;
        sSlots[slot].unk20 = 0;
        sSlots[slot].unk24 = 0;
        sSlots[slot].unk28 = 0;
        sSlots[slot].unk2C = -1;
        sSlots[slot].unk30 = 0;
        sSlots[slot].id = -1;
        SltIdMsg m;
        m.id = arg;
        sSlots[slot].ptr = 0;
        if (target != -1) {
            SendMsg(target, 0x3005, &m);
        }
        return 1;
    }
    return 0;
}

// 0x8016BBB0
void SltDrawEntry(int slot) {
    u32 handle = sSlots[slot].handle;
    if (!sGroups || sGroups[(handle >> 12) & 0xF].file == 0) {
        return;
    }

    int unkArg;
    char bufB[0x100];
    char bufC[0x100];
    char bufD[0x100];
    char path[0x400];

    LevelLoadNames(sGroups[(handle >> 12) & 0xF].file->name, &unkArg, bufB, bufD, bufC);
    sprintf(path, "/%s/%s%s", bufD, bufD, bufC);
    for (int i = 0; i < sSlots[slot].ptr->count; i++) {
        switch (sSlots[slot].ptr->subs[i].type) {
        case 'R':
            fn_80081D0C(path, sSlots[slot].ptr->subs[i].unk4, sSlots[slot].ptr->subs[i].unk1, 0);
            break;
        }
    }
}

// 0x8016BCC0
int SltFileHandler(void* fileData, int size, void* out) {
    SltGroup* group = (SltGroup*)out;

    group->valid = 0;
    group->file = (size > 0) ? (SltFile*)fileData : 0;
    int extra = size - 0x88;
    fn_800070C0(group->file);
    fn_800070C0(&group->file->count);
    if (group->file->type != 4) {
        return 0;
    }
    if (group->file->count > 0) {
        group->entries = (extra > 0 && (u32)group->file->count * 0x34 != 0)
                             ? (SltEntry*)((u8*)fileData + 0x88)
                             : 0;
        extra -= group->file->count * 0x34;
        for (int i = 0; i < group->file->count; i++) {
            fn_800070C0(&group->entries[i].key);
            fn_800070C0(&group->entries[i].bind);
            fn_800070C0(&group->entries[i].arg);
            for (int j = 0; j < 3; j++) {
                fn_80007118(&group->entries[i].subs[j].unk2);
                fn_80007118(&group->entries[i].subs[j].unk4);
                fn_80007118(&group->entries[i].subs[j].unk6);
                fn_80007118(&group->entries[i].subs[j].unkA);
            }
        }
        if (extra != 0) {
            return 0;
        }
    }
    group->valid = 1;
    return 1;
}

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
