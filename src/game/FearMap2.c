// FearMap2.c: the in-game "Objectives" screen and its state machine (0x801FD994-0x80202AB8).
// Names are descriptive guesses; the code is matched against the retail DOL.

#include <dolphin/types.h>

struct Actor;

// Functions with C++ linkage elsewhere in the game (declared outside the C block below).
#ifdef __cplusplus
void VFree(void* p);                                            // 0x8009DA3C
int AssetFind(unsigned long a, unsigned long b);                // 0x8007EB34
void TextureLoad(short* ids, int count);                        // 0x8000F738
int FxFind(const char* name);                                   // 0x80170834
int ActorDefaultMsg(struct Actor* a, int msg, void* data, int arg);   // 0x800E4C64
extern "C" {
#endif

typedef struct ObjEntry {
    int id;             // 0x00
    int group;          // 0x04
    u8 unk8[0x10 - 8];
    float timer;        // 0x10
    u8 unk14[0x18 - 0x14];
    int state;          // 0x18
    int done;           // 0x1C
    int flag;           // 0x20
    u8 unk24[4];
} ObjEntry;             // size 0x28

typedef struct ObjFxRecord {
    float a;
    int b;
    float c;
    int handle;
} ObjFxRecord;

typedef struct ObjRef {
    u8 unk0[0x38];
    int id;             // 0x38
} ObjRef;

typedef struct MsgFlags {
    u8 unk0[0x18];
    u32 flags;          // 0x18
} MsgFlags;

typedef struct ObjMsgData {
    u8 unk0[8];
    u32 flags8;         // 0x08
    u8 unkC[4];
    ObjRef* ref10;      // 0x10
    u8 unk14[8];
    ObjRef* ref1C;      // 0x1C
    ObjRef* ref20;      // 0x20
} ObjMsgData;

typedef struct ObjActorData {
    u8 unk0[4];
    u32 flags;          // 0x04
} ObjActorData;

typedef struct ObjActor {
    u8 unk0[0xA4];
    ObjActorData* data; // 0xA4
} ObjActor;

typedef struct ObjState {
    int (*enter)(void);
    void (*exit)(void);
    int (*update)(float dt, int arg);
    int unkC;
} ObjState;

extern int gObjCount;           // 0x804D5374
extern int gObjState;           // 0x804D5378
extern float gObjTime;          // 0x804D537C
extern int gObjCursor;          // 0x804D5384
extern int gObjButtonUp;        // 0x804D5388
extern int gObjButtonDown;      // 0x804D538C
extern float gObjLo;            // 0x804D3380
extern float gObjHi;            // 0x804D3384
extern ObjEntry gObjEntries[];  // 0x8049D6C0
extern ObjState gObjStates[];   // 0x803BA090

void fn_80202534(void);
void fn_80202728(void);
int fn_80200670(int arg);
void fn_80201DC0(int index);
int SendMsg(int to, int msg, void* data);
void FxStart(int fx, int a, int b);
extern int gObjFxEnabled;       // 0x804D3390
extern int gObjFxActive;        // 0x804D5348
extern int gObjFxCount;         // 0x804D5344
extern ObjFxRecord gObjFxRecords[];   // 0x8049D2C0
extern int gObjTexture;         // 0x804D5330

// TODO (not drafted yet, 0x2F0 bytes at 0x801FD994): first function of the unit
// TODO (not drafted yet, 0x1B0 bytes at 0x801FDC84)
// TODO (not drafted yet, 0x104 bytes at 0x801FDE34)
// TODO (not drafted yet, 0xD0 bytes at 0x801FDF38)
// TODO (not drafted yet, 0x4 bytes at 0x801FE008)
// TODO (not drafted yet, 0x5C bytes at 0x801FE00C)
// TODO (not drafted yet, 0x268 bytes at 0x801FE068)
// TODO (not drafted yet, 0x7FC bytes at 0x801FE2D0)
// TODO (not drafted yet, 0x7E4 bytes at 0x801FEACC)
// TODO (not drafted yet, 0x6F4 bytes at 0x801FF2B0)
// TODO (not drafted yet, 0x3D4 bytes at 0x801FF9A4)
// TODO (not drafted yet, 0x8B8 bytes at 0x801FFD78)

// 0x80200630
void fn_80200630(void) {
    fn_80202534();
}

// 0x80200650
void fn_80200650(void) {
    fn_80202728();
}

// TODO (not drafted yet, 0x6B8 bytes at 0x80200670): draws the Objectives list
// TODO (not drafted yet, 0x2EC bytes at 0x80200D28)

// 0x80201014 advances the per-entry timers; returns 3 when every entry is ready, else the current state.
int ObjMenuAdvance(float dt) {
    float t = gObjTime;
    int n = gObjCount;
    ObjEntry* e = gObjEntries;
    int ok = 1;
    int i;
    float limit;
    for (i = 0; i < n; i++) {
        if (i != 0) {
            limit = (float)i * ((gObjHi - gObjLo) / (float)n - 1.0f);
        } else {
            limit = 0.0f;
        }
        if (t <= limit) {
            ok = 0;
        } else if (e->timer > 0.0f) {
            ok = 0;
            e->timer = e->timer - dt;
            if (e->timer < 0.0f) {
                e->timer = 0.0f;
            }
        }
        e++;
    }
    return ok ? 3 : gObjState;
}

// 0x802010F8 resets the menu clock and clears every entry's flag.
int ObjMenuClearFlags(void) {
    int i;
    gObjTime = 0.0f;
    for (i = 0; i < gObjCount; i++) {
        gObjEntries[i].flag = 0;
    }
    return 1;
}

// 0x802011A4 resets the menu clock.
int ObjMenuResetClock(void) {
    gObjTime = 0.0f;
    return 1;
}

// 0x802011B4 sets every entry's timer to the base delay and resets the clock.
int ObjMenuSetTimers(void) {
    int i;
    for (i = 0; i < gObjCount; i++) {
        gObjEntries[i].timer = gObjLo;
    }
    gObjTime = 0.0f;
    return 1;
}

// 0x80201260 runs the objectives state machine for one frame.
void fn_80201260(float dt, int arg, int closing) {   // ObjMenuUpdate; called from other units
    int next;
    gObjTime = gObjTime + dt;
    if (gObjState == 0 && gObjStates[1].enter() != 0) {
        gObjState = 1;
    }
    if (gObjState != 1 && closing != 0) {
        if (gObjStates[gObjState].exit != 0) {
            gObjStates[gObjState].exit();
        }
        if (gObjStates[1].enter != 0) {
            gObjStates[1].enter();
        }
        gObjState = 1;
    }
    if (gObjState == 1 && closing == 0) {
        if (gObjStates[1].exit != 0) {
            gObjStates[1].exit();
        }
        if (gObjStates[2].enter != 0) {
            gObjStates[2].enter();
        }
        gObjState = 2;
    }
    if (gObjStates[gObjState].update != 0) {
        next = gObjStates[gObjState].update(dt, arg);
        if (next != gObjState) {
            if (gObjStates[gObjState].exit != 0) {
                gObjStates[gObjState].exit();
            }
            gObjState = next;
            if (gObjStates[next].enter != 0) {
                gObjStates[next].enter();
            }
        }
    }
    fn_80200670(arg);
}

// TODO (not drafted yet, 0x30 bytes at 0x80201430): pad callback setting the up/down button flags

// 0x80201460 marks an objective (and the rest of its group) done or not done, starting the "new objective" effect.
void ObjSetState(int id, int on) {
    ObjEntry* e;
    ObjEntry* p;
    int i;
    int n;
    int found;
    int fx;
    ObjFxRecord* r;
    e = 0;
    for (i = 0; i < gObjCount; i++) {
        if (id == gObjEntries[i].id) {
            e = &gObjEntries[i];
            break;
        }
    }
    if (e != 0 && (e->state != on || e->done == on)) {
        if (e->group == 0x44FA364) {
            n = -1;
            for (i = 0; i < gObjCount; i++) {
                if (id == gObjEntries[i].id) {
                    n = i;
                    break;
                }
            }
            e->state = on;
            if (on != 0) {
                fn_80201DC0(n);
            }
            n = n + 1;
            for (p = &gObjEntries[n]; n < gObjCount && e->group != p->group; p++) {
                if (on != 0) {
                    fn_80201DC0(n);
                }
                p->state = on;
                n++;
            }
        } else {
            n = -1;
            for (i = 0; i < gObjCount; i++) {
                if (id == gObjEntries[i].id) {
                    n = i;
                    break;
                }
            }
            fn_80201DC0(n);
            e->done = (on == 0);
            e->state = on;
        }
        if (on != 0 && gObjFxEnabled != 0) {
            if (gObjFxActive == 0) {
                found = 0;
            } else {
                found = 1;
                if (gObjFxCount < 0x40) {
                    fx = AssetFind(0xBAA22E62, 0x2317C5B2);
                    for (i = 0; i < gObjFxCount; i++) {
                        if (fx == gObjFxRecords[i].handle) {
                            found = 0;
                            break;
                        }
                    }
                    if (found) {
                        r = &gObjFxRecords[gObjFxCount];
                        r->b = 1;
                        r->a = 1.0f;
                        r->c = 0.0f;
                        r->handle = fx;
                        gObjFxCount++;
                    }
                }
            }
            if (found) {
                FxStart(FxFind("obj_new"), 0, 0);
            }
        }
    }
}
// TODO (not drafted yet, 0x6E4 bytes at 0x802016DC)
// TODO (not drafted yet, 0x19C bytes at 0x80201DC0)
// TODO (not drafted yet, 0x64 bytes at 0x80201F5C)
// TODO (not drafted yet, 0x78 bytes at 0x80201FC0)

// 0x80202038 loads the objectives texture set and remembers its asset id.
void ObjLoadTextures(void) {
    short id[4];
    id[0] = AssetFind(0xF2E2BEB8, 0x3CC09F9A);
    TextureLoad(id, 1);
    gObjTexture = AssetFind(0xF2E2BEB8, 0x3CC09F9A);
}
// TODO (not drafted yet, 0x104 bytes at 0x80202090)
// TODO (not drafted yet, 0x3A0 bytes at 0x80202194)
// TODO (not drafted yet, 0x1F4 bytes at 0x80202534)
// TODO (not drafted yet, 0x1E8 bytes at 0x80202728)

// 0x80202910 objective actor messages: some are swallowed while the referenced object is in a given state.
int ObjActorMsg(struct Actor* actor, int msg, ObjMsgData* d, int arg) {
    MsgFlags s1;
    MsgFlags s2;
    MsgFlags s3;
    if (msg == 3) {
        if (d->ref10 == 0 || SendMsg(d->ref10->id, 0x400, &s3) == 0 || (s3.flags & 1) == 0 || (s3.flags & 2) != 0) {
            return 1;
        }
        return 0;
    }
    if (msg < 3) {
        if (msg == 1 && d->ref20 != 0 && SendMsg(d->ref20->id, 0x400, &s1) != 0 && (s1.flags & 1) != 0
            && (s1.flags & 2) == 0 && (d->flags8 & 0x4000000) != 0) {
            return 0;
        }
    } else if (msg < 5 && d->ref1C != 0 && SendMsg(d->ref1C->id, 0x400, &s2) != 0 && (s2.flags & 1) != 0
               && (s2.flags & 2) == 0) {
        return 0;
    }
    return ActorDefaultMsg(actor, msg, d, arg);
}

// 0x80202A68 returns bit 4 of the objective's data flags.
u32 ObjActorFlag10(ObjActor* a) {
    return a->data->flags & 0x10;
}

// 0x80202A78 frees an objective actor's data block.
void ObjActorFreeData(ObjActor* a) {
    if (a->data != 0) {
        VFree(a->data);
        a->data = 0;
    }
}

#ifdef __cplusplus
}
#endif
