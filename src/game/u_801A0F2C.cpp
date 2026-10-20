// u_801A0F2C.cpp: a mirror-object behaviour (0x801A0F2C-0x801A1D24).
// An eight-state behaviour (same state-machine helpers as NoBot/ThermoBeh) that spawns a
// coloured effect when its owner asks for one, plus the registry of mirror objects used by
// the reflection pass. The string "Warning: mirror object registered more than once" is
// the file's only string. Names are descriptive guesses; matched against the retail DOL.

typedef unsigned long u32;

// ---- shared game types (layouts taken from the retail code) ----

struct Actor;

struct Actor {
    char pad0[0x48];
    int unk48;
};

struct Owner;

struct Owner {
    char pad0[0x84];
    char fxSource[0x48];    // 0x84: anchor passed to the effect helpers
    int modelId;            // 0xCC
    char padD0[0x14];
    int nodeId;             // 0xE4
    char padE8[0x50];
    float dt;               // 0x138
    char pad13C[0x134];
    char conditions[0x67C]; // 0x270
    int unk8EC;             // 0x8EC
    int unk8F0;             // 0x8F0
};

struct Beh;
struct MirrorObjData;

typedef void (*BehStateFunc)(void* owner, void* beh);

struct BehState {
    BehStateFunc enter;
    BehStateFunc exit;
    BehStateFunc update;
};

struct MirrorObjData {
    int state;      // 0x00
    int next;       // 0x04
    u32 flags;      // 0x08
    int effect;     // 0x0C
    float timer;    // 0x10
};

struct Beh {
    char pad0[0x10];
    int state;      // 0x10
    char pad14[0x18];
    u32 flags;      // 0x2C
    char pad30[0x24];
    MirrorObjData* data; // 0x54
};

// One registered mirror object (.bss array in the reflection unit).
struct MirrorReg {
    int unk00;      // 0x00
    int model;      // 0x04
    int mesh;       // 0x08
    Actor* actor;   // 0x0C
    Actor* obj;     // 0x10
    int objId;      // 0x14
    char pad18[0x30];
    int pos[6];     // 0x48
};

extern BehState gMirrorObjStates[8];

extern MirrorReg lbl_80495D84[8];
extern int lbl_804D514C;
extern int lbl_804D5148;
extern int lbl_804D308C;
extern char lbl_80495CE8[];

extern "C" int fn_8003D988(float* out, void* src);
extern "C" int fn_800C69B8(void* obj, int a, int b);
extern "C" int fn_800BA7FC(int type);
extern "C" int fn_80027A58(int a, void* anchor, float* color, int delay, int b);
extern "C" void fn_800277B0(int handle, void* anchor, float* color, int* delay);
extern "C" void fn_80027690(int handle);
extern "C" void fn_80026660(int id, int (*fn)(int, int, void*), int arg);
extern "C" int rand(void);

void DebugPrintf(const char* fmt, ...);
void ActorFn80022D98(Actor* actor, int a);
void ModelNodeSetup(int a, int b, int c, int d);
int OwnerId(Owner* o);
int HasCondition(void* list, int which);
int Fn800F7DEC(int a, int b, int c, int d, void* e);
void Fn800F7F5C(int a, int b, int c, int d, void* e, float f);
int Fn80161FF4(Owner* o, void* buf, float f);
void Fn8016295C(Owner* o, Beh* b);
void BehUpdate(Owner* o, Beh* b);
void BehDone(Beh* b);
void BehReset(Beh* b);
void* VAlloc(int size);
void VFree(void* p);
int PickupScanMsg(int a, int b, void* c);

// ---- mirror object registry ----

int MirrorObjRegister(void* src, Actor* actor, Actor* obj, int objId) {
    float place[6];
    MirrorReg* e;
    int r;

    if (objId != -1 && obj == 0) {
        objId = -1;
    }
    if (lbl_804D514C < 8) {
        int i;

        for (i = 0; i < lbl_804D514C; i++) {
            if (lbl_80495D84[i].actor == actor) {
                DebugPrintf("Warning: mirror object registered more than once\n");
                return 1;
            }
        }
        r = fn_8003D988(place, src);
        if (r == -2) {
            return 1;
        }
        if (r == -1) {
            return 0;
        }
        e = &lbl_80495D84[lbl_804D514C];
        e->mesh = r;
        e->actor = actor;
        e->obj = obj;
        e->objId = objId;
        e->model = actor->unk48;
        e->pos[0] = (int)place[0];
        e->pos[1] = (int)place[1];
        e->pos[2] = (int)place[2];
        e->pos[3] = (int)place[3];
        e->pos[4] = (int)place[4];
        e->pos[5] = (int)place[5];
        lbl_804D514C++;
        if (obj) {
            ActorFn80022D98(obj, 0);
        }
    }
    return 1;
}

void MirrorObjInitModule(void) {
    lbl_804D514C = 0;
    lbl_804D308C = -1;
    lbl_804D5148 = fn_800C69B8(lbl_80495CE8, 0, 0);
}

// ---- state updates ----

void MirrorObjState7Update(void* owner, void* beh) {
    if (beh) {
        Owner* o = (Owner*)owner;
        Beh* b = (Beh*)beh;
        MirrorObjData* d = b->data;

        if ((d->flags & 0x20) && (d->flags & 0x10)) {
            d->next = 0;
        }
    }
}

void MirrorObjState6Update(void* owner, void* beh) {
    if (beh) {
        Owner* o = (Owner*)owner;
        Beh* b = (Beh*)beh;
        MirrorObjData* d = b->data;

        if ((d->flags & 0x20) && (d->flags & 0x10)) {
            d->next = 0;
        }
    }
}

void MirrorObjState5Update(void* owner, void* beh) {
    if (beh) {
        Owner* o = (Owner*)owner;
        Beh* b = (Beh*)beh;
        MirrorObjData* d = b->data;
        float color[4];
        int delay;
        int m;

        BehUpdate(o, b);
        Fn8016295C(o, b);
        if (d->flags & 1) {
            if (fn_800BA7FC(0x29) == 0 && (d->flags & 0x100) == 0) {
                delay = (int)(250.0f * (rand() / 32767.0f) + 800.0f);
                color[1] = 0.3f * (rand() / 32767.0f) + 0.3f;
                color[0] = 1.0f;
                color[2] = 0.0f;
                color[3] = 1.0f;
                if (d->effect == -1) {
                    d->effect = fn_80027A58(1, o->fxSource, color, delay, 9);
                    b->flags |= 0x100000;
                } else {
                    fn_800277B0(d->effect, o->fxSource, color, &delay);
                    b->flags &= ~0x100000;
                    fn_80027690(d->effect);
                    d->effect = -1;
                    d->flags |= 0x100;
                }
            }
            if (d->flags & 0x10) {
                m = Fn800F7DEC(o->unk8EC, o->unk8F0, 0xF829DB93, OwnerId(o), 0);
                switch (m) {
                case 0xC0BECF7E:
                    d->next = 7;
                    break;
                case 0x04A3E183:
                    d->next = 0;
                    break;
                case 0x11D2C025:
                case 0x3DA260F7:
                    break;
                }
            }
        }
    }
}

void MirrorObjState4Update(void* owner, void* beh) {
    if (beh) {
        Owner* o = (Owner*)owner;
        Beh* b = (Beh*)beh;
        MirrorObjData* d = b->data;
        float color[4];
        int delay;
        int m;

        BehUpdate(o, b);
        Fn8016295C(o, b);
        if (d->flags & 1) {
            if (fn_800BA7FC(0x29) == 0 && (d->flags & 0x100) == 0) {
                delay = (int)(250.0f * (rand() / 32767.0f) + 800.0f);
                color[1] = 0.3f * (rand() / 32767.0f) + 0.3f;
                color[0] = 1.0f;
                color[2] = 0.0f;
                color[3] = 1.0f;
                if (d->effect == -1) {
                    d->effect = fn_80027A58(1, o->fxSource, color, delay, 9);
                    b->flags |= 0x100000;
                } else {
                    fn_800277B0(d->effect, o->fxSource, color, &delay);
                    b->flags &= ~0x100000;
                    fn_80027690(d->effect);
                    d->effect = -1;
                    d->flags |= 0x100;
                }
            }
            if (d->flags & 0x10) {
                m = Fn800F7DEC(o->unk8EC, o->unk8F0, 0x20EE36, OwnerId(o), 0);
                switch (m) {
                case 0xC0BECF7E:
                    d->next = 6;
                    break;
                case 0x04A3E183:
                    d->next = 0;
                    break;
                case 0x11D2C025:
                case 0x3DA260F7:
                    break;
                }
            }
        }
    }
}

void MirrorObjState0Update(void* owner, void* beh) {
    if (beh) {
        Owner* o = (Owner*)owner;
        Beh* b = (Beh*)beh;

        BehUpdate(o, b);
        Fn8016295C(o, b);
    }
}

// ---- state exits ----

void MirrorObjState7Exit(void* owner, void* beh) {}

void MirrorObjState6Exit(void* owner, void* beh) {}

void MirrorObjState5Exit(void* owner, void* beh) {
    if (beh) {
        MirrorObjData* d = ((Beh*)beh)->data;

        if (d->effect != -1) {
            fn_80027690(d->effect);
            d->effect = -1;
        }
    }
}

void MirrorObjState4Exit(void* owner, void* beh) {
    if (beh) {
        MirrorObjData* d = ((Beh*)beh)->data;

        if (d->effect != -1) {
            fn_80027690(d->effect);
            d->effect = -1;
        }
    }
}

void MirrorObjState0Exit(void* owner, void* beh) {}

// ---- state enters ----

void MirrorObjState7Enter(void* owner, void* beh) {
    if (beh) {
        Beh* b = (Beh*)beh;

        b->data->flags = 0;
        BehDone(b);
    }
}

void MirrorObjState6Enter(void* owner, void* beh) {
    if (beh) {
        Beh* b = (Beh*)beh;

        b->data->flags = 0;
        BehDone(b);
    }
}

void MirrorObjState5Enter(void* owner, void* beh) {
    if (beh) {
        Beh* b = (Beh*)beh;

        b->data->flags = 0;
        BehDone(b);
    }
}

void MirrorObjState4Enter(void* owner, void* beh) {
    if (beh) {
        Beh* b = (Beh*)beh;

        b->data->flags = 0;
        BehDone(b);
    }
}

void MirrorObjState0Enter(void* owner, void* beh) {
    if (beh) {
        Owner* o = (Owner*)owner;
        Beh* b = (Beh*)beh;

        b->data->flags = 0;
        BehDone(b);
        if (Fn800F7DEC(o->unk8EC, o->unk8F0, 0xE6B71BCC, OwnerId(o), 0) == 0x04A3E183) {
            b->flags |= 0x10;
        }
    }
}

// ---- behaviour callbacks (table in gMirrorObjStates) ----

int MirrorObjQuery(void* owner, void* beh) {
    return 0;
}

int MirrorObjMsg(void* owner, void* beh, int msg) {
    Owner* o;
    Beh* b;
    MirrorObjData* d;
    char buf[96];

    if (!beh) {
        return 0;
    }
    o = (Owner*)owner;
    b = (Beh*)beh;
    d = b->data;
    switch (msg) {
    case 1:
        if (d->state == 0 && d->timer <= 0.0f) {
            d->next = 4;
        }
        break;
    case 2:
        if (d->state == 0 && d->timer <= 0.0f) {
            d->next = 5;
        }
        break;
    case 3:
        if (d->state == 0) {
            d->next = 6;
        }
        break;
    case 4:
        if (d->state == 0) {
            d->next = 7;
        }
        break;
    case 0x1B83833F:
        d->flags |= 0x10;
        break;
    case 0xF829DB93:
    case 0x0020EE36:
        if (Fn800F7DEC(o->unk8EC, o->unk8F0, msg, OwnerId(o), 0) == 0x04A3E183) {
            d->flags |= 1;
            if (!Fn80161FF4(o, buf, 0.0f)) {
                return 0;
            }
            Fn800F7F5C(o->unk8EC, o->unk8F0, msg, OwnerId(o), buf, o->dt);
            d->flags &= ~0x100;
            d->timer = 7000.0f;
        }
        break;
    case 0xE6B71BCC:
    case 0x968CBE3D: {
        char buf2[96];

        if (!(d->flags & 0x20)) {
            if (!Fn80161FF4(o, buf2, 0.0f)) {
                return 0;
            }
            Fn800F7F5C(o->unk8EC, o->unk8F0, msg, OwnerId(o), buf2, o->dt);
            d->flags |= 0x20;
        }
        break;
    }
    }
    return 1;
}

int MirrorObjUpdate(void* owner, void* beh) {
    Owner* o;
    Beh* b;
    MirrorObjData* d;

    if (!beh) {
        return 0;
    }
    o = (Owner*)owner;
    b = (Beh*)beh;
    d = b->data;
    if (HasCondition(o->conditions, 9) || HasCondition(o->conditions, 10) ||
        HasCondition(o->conditions, 11)) {
        if (d->effect != -1) {
            fn_80027690(d->effect);
            d->effect = -1;
        }
        ModelNodeSetup(o->modelId, o->nodeId, 0, 1);
        d->flags = 0;
        d->timer = 0.0f;
        BehReset(b);
        BehDone(b);
    } else {
        if (d->timer >= 0.0f) {
            d->timer = d->timer - o->dt;
        }
        if (gMirrorObjStates[d->state].update) {
            gMirrorObjStates[d->state].update(owner, beh);
        }
        if (d->next != d->state) {
            if (gMirrorObjStates[d->state].exit) {
                gMirrorObjStates[d->state].exit(owner, beh);
            }
            d->state = d->next;
            b->state = d->next;
            if (gMirrorObjStates[d->state].enter) {
                gMirrorObjStates[d->state].enter(owner, beh);
            }
        }
    }
    return 1;
}

int MirrorObjDestroy(void* owner, void* beh) {
    Beh* b;
    MirrorObjData* d;

    if (!beh) {
        return 0;
    }
    b = (Beh*)beh;
    d = b->data;
    if (d->effect != -1) {
        fn_80027690(d->effect);
        d->effect = -1;
    }
    VFree(d);
    return 1;
}

int MirrorObjInit(void* owner, void* beh) {
    Beh* b;
    MirrorObjData* d;

    if (!beh) {
        return 0;
    }
    b = (Beh*)beh;
    d = (MirrorObjData*)VAlloc(sizeof(MirrorObjData));
    if (d == 0) {
        b->data = 0;
        return 0;
    }
    b->data = d;
    b->state = 0;
    d->state = 0;
    d->next = 0;
    d->effect = -1;
    d->flags = 0;
    d->timer = 0.0f;
    if (gMirrorObjStates[d->state].enter) {
        gMirrorObjStates[d->state].enter(owner, beh);
    }
    return 1;
}

void MirrorObjRegisterScan(void) {
    fn_80026660(0x55, PickupScanMsg, 0);
}

BehState gMirrorObjStates[8] = {
    { MirrorObjState0Enter, MirrorObjState0Exit, MirrorObjState0Update },
    { 0, 0, 0 },
    { 0, 0, 0 },
    { 0, 0, 0 },
    { MirrorObjState4Enter, MirrorObjState4Exit, MirrorObjState4Update },
    { MirrorObjState5Enter, MirrorObjState5Exit, MirrorObjState5Update },
    { MirrorObjState6Enter, MirrorObjState6Exit, MirrorObjState6Update },
    { MirrorObjState7Enter, MirrorObjState7Exit, MirrorObjState7Update },
};
