// u_8000F2F0.cpp: texture table and render-state cache (0x8000F2F0-0x80012E04).
// - A table of textures (0x24 bytes each, ids are 12-bit indices with chained alternates) backed
//   by disc streaming slots (0x18 bytes each); load / free / prefetch walk the id chain and poke
//   the stream state machine.
// - A small cache of the current texture, fog / tint / blend settings that is flushed to GX by
//   the big frame-state setup function at the end.
// Names are descriptive guesses; unmatched against retail so far.

#include <dolphin/gx.h>

// Functions called from other units under their placeholder names (C linkage).
extern "C" void fn_8000F2F0(int* resident, int* total);
extern "C" void fn_8000F550(short* ids, int n);
extern "C" void fn_8000F60C(int id);
extern "C" void fn_8000F7E4(int id, char prio);
extern "C" void fn_8000F874(int id);
extern "C" int fn_8000F904(int id);
extern "C" int fn_8000F990(int id);
extern "C" void fn_8000FBFC(int on);
extern "C" int fn_8000FECC(int id);
extern "C" void fn_8000FF28(int on);
extern "C" void fn_8000FF7C();
extern "C" u32 fn_80010000(int id);
extern "C" void fn_800102C8(int id, u32 v);
extern "C" u32 fn_800102F8(int id);
extern "C" int fn_80010330(int id, int alpha);
extern "C" int fn_80010448(int id, int alpha);

struct StreamEntry;

// Disc streaming slot (0x18 bytes).
struct TexSlot {
    int tex;            // 0x00
    int pending;        // 0x04
    short state;        // 0x08: 0 free, 1 queued, 2 requested, 3 releasing
    short pad0A;
    int pad0C;
    int size;           // 0x10
    int pad14;
};

// Loaded streaming resource: the stream entry is embedded at +8 (priority at +10).
struct TexRes {
    int tex;            // 0x00: texture data once bound
    int data;           // 0x04: raw data, set when the stream finishes
    short pad08;
    char prio;          // 0x0A
    char pad0B;
};

// Texture table entry (0x24 bytes).
struct TexEntry {
    short kind;         // 0x00
    short unk02;        // 0x02 (returned by fn_8000FECC)
    short width;        // 0x04
    short height;       // 0x06
    short link;         // 0x08: next id in the chain
    short count;        // 0x0A: mip / frame count, also read as a short by fn_8000F904
    u32 flags;          // 0x0C
    u32 flags2;         // 0x10
    char pad14[8];
    TexRes* res;        // 0x1C
    int unk20;          // 0x20
};

// Mangled C++ callees.
void StreamRead(int pri, StreamEntry* e, void (*done)(int, void*, void*), void* arg);
void StreamFree(void* p);
void GunStateTimerTick8017BAD0(void* p);
void GunStateStep8017BBBC(void* p);
void GunStateFinish8017BC5C(void* p);
void GunBlockClear8017BCD0(void* p);
int GunBlockClear8017BD20(void* p);

extern TexEntry* lbl_804D4130;
extern int lbl_804D4134;
extern TexSlot* lbl_804D4138;
extern int lbl_804D413C;
extern int lbl_804D4140;
extern int lbl_804D4144;
extern int lbl_804D2608;
extern u32 lbl_804D414C;
extern int lbl_804D25C8;
extern int lbl_804D261C;
extern int lbl_804D2604;
extern int lbl_804D25CC;

extern "C" {
void fn_801D8FC8(void);
void fn_80008C00(const char* msg);
void fn_800891C8(void);
void fn_8010D024(void);
void fn_8010CF00(void);
// TODO (not drafted yet, 0x2318 bytes at 0x80010AEC): flush the cached render state to GX
void fn_80010AEC(void);
void fn_80096390(int a, int b);
void fn_8010EF3C(void);
void fn_8010EF00(void);
void fn_801060C4(double f, u32* color);
void fn_801D8A68(int a);
float fn_802A2080(void);
}
void GfxFbCommit();
void GfxResetTev();
int FileAddHandler(int type, int (*fn)(void*, int), void* arg);
extern GXTexObj lbl_803DA920;
extern int lbl_804D2610;
void GfxGetViewportRect(short* out);
void* fn_80115A34(int a, int b, short* rect);


void TextureSlotDone(int result, void* arg, void* slotp);

// Total and resident streaming memory.
void fn_8000F2F0(int* resident, int* total) {
    int r = 0;
    int t = 0;
    TexSlot* s = lbl_804D4138;
    int n = lbl_804D413C;

    if (n > 0) {
        do {
            t += s->size;
            if (s->state == 1 || s->state == 4) {
                r += s->size;
            }
            s++;
            n--;
        } while (n != 0);
    }
    if (resident != 0) {
        *resident = r;
    }
    if (total != 0) {
        *total = t;
    }
}

// Queue every slot that was requested.
void TextureFlushRequests() {
    if (lbl_804D4140 != 0 && lbl_804D2608 != 0) {
        lbl_804D4140 = 0;
        for (int i = 0; i < lbl_804D413C; i++) {
            TexSlot* s = &lbl_804D4138[i];
            if (s->state == 2) {
                s->state = 1;
                StreamRead(2, (StreamEntry*)&s->state, TextureSlotDone, s);
            }
        }
    }
}

// Release every slot marked for release.
void TextureFlushReleases() {
    if (lbl_804D4144 != 0 && lbl_804D2608 != 0) {
        lbl_804D4144 = 0;
        for (int i = 0; i < lbl_804D413C; i++) {
            TexSlot* s = &lbl_804D4138[i];
            if (s->state == 3) {
                if (s->pending == 0) {
                    lbl_804D4144 = 1;
                } else {
                    s->state = 0;
                    StreamFree((void*)s->pending);
                    s->tex = 0;
                    s->pending = 0;
                }
            }
        }
    }
}

// Stream completion callback.
void TextureSlotDone(int result, void* arg, void* slotp) {
    TexSlot* s = (TexSlot*)slotp;

    if (result < 1) {
        if (result < 0) {
            int st = GunBlockClear8017BD20(&s->state);
            if (st == 2 || st == 1) {
                if (s->pending == 0) {
                    StreamRead(2, (StreamEntry*)&s->state, TextureSlotDone, s);
                }
                s->state = 1;
            } else {
                s->tex = 0;
                s->pending = 0;
            }
        } else {
            GunBlockClear8017BCD0(&s->state);
            s->tex = 0;
            s->pending = 0;
        }
    } else {
        s->tex = 0;
        s->pending = (int)arg;
    }
}

// Mark a list of texture ids (and their chains) for release.
void fn_8000F550(short* ids, int n) {
    for (int i = 0; i < n; i++) {
        int idx = *ids & 0xFFF;
        lbl_804D4144 = 1;
        int v = *ids;
        if (idx != 0xFFF) {
            v = idx;
        }
        while (v > -1 && v < lbl_804D4134) {
            TexEntry* e = &lbl_804D4130[v];
            GunStateTimerTick8017BAD0((char*)e->res + 8);
            v = e->link;
            idx = v & 0xFFF;
            if (idx != 0xFFF) {
                v = idx;
            }
        }
        ids++;
    }
}

void fn_8000F60C(int id) {
    lbl_804D4144 = 1;
    if ((id & 0xFFF) != 0xFFF) {
        id = id & 0xFFF;
    }
    while (id > -1 && id < lbl_804D4134) {
        TexEntry* e = &lbl_804D4130[id];
        GunStateTimerTick8017BAD0((char*)e->res + 8);
        id = e->link;
        int idx = id & 0xFFF;
        if (idx != 0xFFF) {
            id = idx;
        }
    }
}

void TexturePrefetchIds(short* ids, int n) {
    for (int i = 0; i < n; i++) {
        int idx = *ids & 0xFFF;
        lbl_804D4140 = 1;
        int v = *ids;
        if (idx != 0xFFF) {
            v = idx;
        }
        while (v > -1 && v < lbl_804D4134) {
            TexEntry* e = &lbl_804D4130[v];
            TexRes* r = e->res;
            r->prio = 1;
            GunStateStep8017BBBC((char*)r + 8);
            v = e->link;
            idx = v & 0xFFF;
            if (idx != 0xFFF) {
                v = idx;
            }
        }
        ids++;
    }
}

void TextureLoad(short* ids, int n) {
    for (int i = 0; i < n; i++) {
        int idx = *ids & 0xFFF;
        lbl_804D4140 = 1;
        int v = *ids;
        if (idx != 0xFFF) {
            v = idx;
        }
        while (v > -1 && v < lbl_804D4134) {
            TexEntry* e = &lbl_804D4130[v];
            TexRes* r = e->res;
            r->prio = 0;
            GunStateStep8017BBBC((char*)r + 8);
            v = e->link;
            idx = v & 0xFFF;
            if (idx != 0xFFF) {
                v = idx;
            }
        }
        ids++;
    }
}

void fn_8000F7E4(int id, char prio) {
    lbl_804D4140 = 1;
    if ((id & 0xFFF) != 0xFFF) {
        id = id & 0xFFF;
    }
    while (id > -1 && id < lbl_804D4134) {
        TexEntry* e = &lbl_804D4130[id];
        TexRes* r = e->res;
        r->prio = prio;
        GunStateStep8017BBBC((char*)r + 8);
        id = e->link;
        int idx = id & 0xFFF;
        if (idx != 0xFFF) {
            id = idx;
        }
    }
}

void fn_8000F874(int id) {
    lbl_804D4140 = 1;
    if ((id & 0xFFF) != 0xFFF) {
        id = id & 0xFFF;
    }
    while (id > -1 && id < lbl_804D4134) {
        TexEntry* e = &lbl_804D4130[id];
        TexRes* r = e->res;
        r->prio = 0;
        GunStateStep8017BBBC((char*)r + 8);
        id = e->link;
        int idx = id & 0xFFF;
        if (idx != 0xFFF) {
            id = idx;
        }
    }
}

int fn_8000F904(int id) {
    if ((id & 0xFFF) != 0xFFF) {
        id = id & 0xFFF;
    }
    if (id > -1 && id < lbl_804D4134) {
        return lbl_804D4130[id].count;
    }
    return 0;
}

void GfxLoadTexObj(GXTexObj* obj, int map) {
    lbl_804D25C8 = -10000;
    fn_801D8FC8();
    GXLoadTexObj(obj, (GXTexMapID)map);
}

// Bind a texture's GX object, building it from the streamed data on first use.
int fn_8000F990(int id) {
    if ((id & 0xFFF) != 0xFFF) {
        id = id & 0xFFF;
    }
    if (id > -1 && id < lbl_804D4134) {
        TexEntry* e = &lbl_804D4130[id];
        TexRes* r = e->res;
        bool ok;

        if (r->tex == 0) {
            int data = r->data;
            if (data == 0) {
                ok = false;
            } else {
                r->tex = data;
                u32 fl = e->flags;
                GXTexFmt fmt;
                if ((fl & 0x40) == 0) {
                    if ((fl & 0x20000) == 0) {
                        if ((fl & 2) == 0) {
                            if ((fl & 1) == 0) {
                                fmt = GX_TF_RGBA8;
                            } else {
                                fmt = GX_TF_CMPR;
                            }
                        } else if ((fl & 0x100) == 0) {
                            fmt = GX_TF_I8;
                        } else {
                            fmt = GX_TF_IA8;
                        }
                    } else {
                        fmt = GX_TF_RGB565;
                        e->flags = fl | 0x52000;
                    }
                } else {
                    fmt = GX_TF_I4;
                }
                fl = e->flags;
                GXTexWrapMode wrapS = (GXTexWrapMode)(((fl >> 13) & 1) ^ 1);
                GXTexWrapMode wrapT = (GXTexWrapMode)(((fl >> 18) & 1) ^ 1);
                GXTexObj* obj = (GXTexObj*)data;
                if ((fl & 8) == 0) {
                    if ((fl & 4) == 0 || lbl_804D261C == 0) {
                        GXInitTexObj(obj, (char*)data + 0x20, e->width, e->height, fmt, wrapS, wrapT, 0);
                    } else {
                        GXInitTexObj(obj, (char*)data + 0x20, e->width, e->height, fmt, wrapS, wrapT, 1);
                        GXInitTexObjLOD(obj, GX_LIN_MIP_LIN, GX_LINEAR, 0.0f, 2.0f, -0.5f, GX_FALSE, GX_FALSE,
                                        GX_ANISO_1);
                    }
                } else {
                    short w = e->width;
                    short h = e->height;
                    short n = e->count;
                    u32 stride = GXGetTexBufferSize(w, h, fmt, 0, 0);
                    char* img = (char*)data + n * 0x20;
                    for (int i = 0; i < n; i++) {
                        GXInitTexObj(obj, img, w, h, fmt, wrapS, wrapT, 0);
                        img += stride;
                        obj++;
                    }
                }
                ok = true;
            }
        } else {
            ok = true;
        }
        if (ok) {
            return lbl_804D4130[id].res->tex;
        }
    }
    return 0;
}

void fn_8000FBFC(int on) {
    if (on != 0) {
        lbl_804D414C |= 8;
    } else {
        lbl_804D414C &= ~8;
    }
}

void GfxCaptureFrame(double f, int on, u8 r, u8 g, u8 b, u8 a) {
    if (on == 0) {
        lbl_804D414C &= ~4;
    } else {
        u32 color;
        short rect[4];

        fn_80096390(0xFF, 0);
        GfxFbCommit();
        GfxGetViewportRect(rect);
        GXLoadTexObj((GXTexObj*)fn_80115A34(0, 1, rect), GX_TEXMAP0);
        fn_801D8A68(1);
        color = (r << 24) | (g << 16) | (b << 8) | a;
        fn_8010EF3C();
        fn_8010EF00();
        u32 c2 = color;
        fn_801060C4(f, &c2);
        lbl_804D25C8 = -2;
        lbl_804D414C |= 4;
    }
}

extern int lbl_804D4080;
extern int lbl_804D25D8;
extern int lbl_804D25DC;
extern int lbl_804D25D0;

void GfxSetFogA(int v) {
    if (lbl_804D4080 != 0) {
        v = 0;
    }
    if (lbl_804D25D8 != v) {
        lbl_804D25D8 = v;
        lbl_804D25C8 = -2;
    }
}

void GfxSetFogB(int v) {
    if (lbl_804D4080 != 0) {
        v = 0;
    }
    if (lbl_804D25DC != v) {
        lbl_804D25DC = v;
        lbl_804D25C8 = -2;
    }
}

void GfxSetFogC(int v) {
    if (lbl_804D4080 != 0) {
        v = 0;
    }
    if (lbl_804D25D0 != v) {
        lbl_804D25D0 = v;
        lbl_804D25C8 = -2;
    }
}

int GfxFlag1Set() {
    return (-lbl_804D414C | lbl_804D414C) >> 31;
}

extern u8 lbl_804D2614[8];

void GfxSetTint(int slot, u8* c) {
    if (slot < 0) {
        return;
    }
    if (slot > 1) {
        return;
    }
    lbl_804D2614[slot * 4] = c[0];
    lbl_804D2614[slot * 4 + 1] = c[1];
    lbl_804D2614[slot * 4 + 2] = c[2];
    lbl_804D2614[slot * 4 + 3] = c[3];
}

extern u8 lbl_804D260C[4];

void GfxSetBlendColor(u8 r, u8 g, u8 b, char a, int mode) {
    lbl_804D260C[0] = r;
    lbl_804D260C[1] = g;
    lbl_804D260C[2] = b;
    lbl_804D260C[3] = a;
    if (mode != -3) {
        if (mode < -3) {
            if (mode == -5) {
                lbl_804D2610 = 4;
                return;
            }
            if (mode > -6) {
                lbl_804D2610 = 2;
                return;
            }
        } else if (mode < -1) {
            lbl_804D2610 = 2;
            return;
        }
    }
    lbl_804D2610 = 1;
    if (a != -1) {
        lbl_804D2610 = 8;
    }
}

// Select the current texture (or none) for the next draw.
void GfxSelectTexture(int id) {
    if ((id & 0xFFF) != 0xFFF) {
        id = id & 0xFFF;
    }
    if (id < 0 || id >= lbl_804D4134 || lbl_804D2604 == 0) {
        lbl_804D25CC = -1;
        lbl_804D25C8 = -10000;
        fn_801D8FC8();
    } else if (lbl_804D25CC != id) {
        lbl_804D25C8 = -10000;
        lbl_804D25CC = id;
        fn_801D8FC8();
    }
}

int fn_8000FECC(int id) {
    if ((id & 0xFFF) != 0xFFF) {
        id = id & 0xFFF;
    }
    if (id > -1 && id < lbl_804D4134) {
        return lbl_804D4130[id].unk02;
    }
    return 1;
}

void GfxSetFlag2(int on) {
    if (on != 0) {
        lbl_804D414C |= 2;
    } else {
        lbl_804D414C &= ~2;
    }
}

void fn_8000FF28(int on) {
    if (on != 0) {
        lbl_804D414C |= 0x10;
    } else {
        lbl_804D414C &= ~0x10;
    }
}

void GfxSetFlag10B(int on) {
    if (on != 0) {
        lbl_804D414C |= 0x10;
    } else {
        lbl_804D414C &= ~0x10;
    }
}

void GfxSetFlag1(int on) {
    if (on != 0) {
        lbl_804D414C |= 1;
    } else {
        lbl_804D414C &= ~1;
    }
}

// Invalidate the cached render state.
void fn_8000FF7C() {
    lbl_804D25C8 = -10000;
    fn_801D8FC8();
}

extern int lbl_804D4148;

void GfxAdvanceTimer(float f) {
    float s = fn_802A2080();
    lbl_804D25C8 = -1;
    lbl_804D4148 += (int)(f * s);
    fn_801D8FC8();
}

u32 fn_80010000(int id) {
    if ((id & 0xFFF) != 0xFFF) {
        id = id & 0xFFF;
    }
    if (id < lbl_804D4134) {
        return lbl_804D4130[id].flags;
    }
    return 0;
}

void TextureReleaseAll() {
    lbl_804D4144 = 1;
    for (int i = 0; i < lbl_804D413C; i++) {
        GunStateFinish8017BC5C(&lbl_804D4138[i].state);
    }
    TextureFlushReleases();
}

void fn_800102C8(int id, u32 v) {
    if ((id & 0xFFF) != 0xFFF) {
        id = id & 0xFFF;
    }
    if (lbl_804D4134 <= id) {
        return;
    }
    lbl_804D4130[id].flags2 = v;
}

u32 fn_800102F8(int id) {
    if ((id & 0xFFF) != 0xFFF) {
        id = id & 0xFFF;
    }
    if (id < lbl_804D4134) {
        return lbl_804D4130[id].flags2;
    }
    return 0;
}

// TODO (not drafted yet, 0x1B4 bytes at 0x80010114): init the texture system (file handler, 8x8 default texture)

// Simpler variant of fn_80010448: always flushes the cached state.
int fn_80010330(int id, int alpha) {
    if ((id & 0xFFF) != 0xFFF) {
        id = id & 0xFFF;
    }
    if (lbl_804D4130 == 0) {
        lbl_804D25C8 = -3;
        fn_801D8A68(alpha != 0 ? 8 : 1);
        fn_8010D024();
        return 1;
    }
    if ((lbl_804D414C & 8) == 0) {
        fn_80010AEC();
        if ((lbl_804D4130[id].flags2 & 0x40000) == 0) {
            return 1;
        }
        return 8;
    }
    return 0;
}

// Select the render state for a texture id; returns the GX blend mode to use.
int fn_80010448(int id, int alpha) {
    if (lbl_804D4130 == 0) {
        lbl_804D25C8 = -3;
        fn_801D8A68(alpha != 0 ? 8 : 1);
        fn_8010D024();
        return 1;
    }
    int idx = id;
    if ((id & 0xFFF) != 0xFFF) {
        idx = id & 0xFFF;
    }
    if (lbl_804D414C != 0) {
        if ((lbl_804D414C & 0x10) != 0) {
            if (idx > -1 && idx < lbl_804D4134 && (lbl_804D4130[idx].flags & 0x8E00) != 0) {
                return 0;
            }
            lbl_804D25C8 = -2;
            return 1;
        }
        if ((lbl_804D414C & 8) != 0) {
            if (idx > -1 && idx < lbl_804D4134) {
                TexEntry* e = &lbl_804D4130[idx];
                if ((e->flags2 & 0x4000) == 0 && e->width != 0) {
                    if ((e->flags & 0xF00) != 0) {
                        return 0;
                    }
                    short k = lbl_804D4130[idx].kind;
                    if (k != 5 && k != 1) {
                        return 1;
                    }
                    return 0;
                }
            }
            return 0;
        }
        if ((lbl_804D414C & 4) != 0) {
            if (idx > -1 && idx < lbl_804D4134) {
                TexEntry* e = &lbl_804D4130[idx];
                if ((e->flags & 0x8E00) != 0) {
                    return 0;
                }
                if ((e->flags2 & 0x4000) == 0) {
                    return 1;
                }
            }
            return 0;
        }
        if ((lbl_804D414C & 1) != 0) {
            if (idx > -1 && idx < lbl_804D4134 && (lbl_804D4130[idx].flags & 0x8E00) != 0) {
                return 0;
            }
            lbl_804D25C8 = -2;
            fn_801D8A68(lbl_804D2610);
            GXColor col = *(GXColor*)lbl_804D260C;
            GXSetTevKColor(GX_KCOLOR0, col);
            fn_8010CF00();
            return 8;
        }
        if ((lbl_804D414C & 2) != 0) {
            if (idx > -1 && idx < lbl_804D4134 && (lbl_804D4130[idx].flags & 0x8E00) != 0) {
                return 0;
            }
            lbl_804D25C8 = -2;
            fn_801D8A68(2);
            fn_8010D024();
            return 8;
        }
    }
    if (idx > -1 && idx < lbl_804D4134) {
        TexEntry* e = &lbl_804D4130[idx];
        if ((e->flags2 & 0x4000) == 0 && e->width != 0) {
            if ((e->flags & 0xF00) != 0) {
                return 2;
            }
            if (e->kind == 5 || e->kind == 1) {
                return 2;
            }
            if (e->unk02 > 2) {
                return 2;
            }
            fn_80010AEC();
            if ((lbl_804D4130[idx].flags2 & 0x40000) != 0) {
                return 8;
            }
            return 1;
        }
    }
    if (id == -3) {
        if (lbl_804D25C8 != -3) {
            lbl_804D25C8 = -3;
            fn_801D8A68(alpha != 0 ? 8 : 1);
            fn_8010D024();
        }
        return 1;
    }
    if (id < -3) {
        if (id == -5) {
            if (lbl_804D25C8 != -5) {
                lbl_804D25C8 = -5;
                fn_801D8A68(4);
                fn_8010D024();
            }
            return 1;
        }
        if (id > -6) {
            if (lbl_804D25C8 != -4) {
                lbl_804D25C8 = -4;
                fn_801D8A68(2);
                fn_8010D024();
            }
            return 1;
        }
    } else {
        if (id == -1) {
            if (lbl_804D25C8 != -1) {
                lbl_804D25C8 = -1;
                GXLoadTexObj(&lbl_803DA920, GX_TEXMAP0);
                fn_801D8A68(8);
                GfxResetTev();
            }
            return 1;
        }
        if (id < -1) {
            lbl_804D25C8 = -2;
            fn_801D8A68(2);
            fn_8010D024();
            return 1;
        }
    }
    return 0;
}

// Slice of the file buffer: valid only if both the remaining size and the count are positive.
static inline void* Slice(void* p, int avail, int n) {
    if (avail > 0 && n > 0) {
        return p;
    }
    return 0;
}

// File handler for the texture bank: header (version 7), entry table, slot table, extra data.
int TextureFileLoad(void* file, int size) {
    int* hdr = (int*)file;
    if (size < 1) {
        hdr = 0;
    }
    char* body = (char*)file + 0x10;
    if (hdr[0] < 7) {
        fn_80008C00("Texture version mismatch (old data)");
        return 0;
    }
    if (hdr[0] > 7) {
        fn_80008C00("Texture version mismatch (old code)");
        return 0;
    }
    lbl_804D4134 = hdr[1];
    lbl_804D4130 = (TexEntry*)Slice(body, size - 0x10, lbl_804D4134 * 0x24);
    int rest = size - 0x10 - lbl_804D4134 * 0x24;
    char* slots = body + lbl_804D4134 * 0x24;
    lbl_804D413C = hdr[2];
    lbl_804D4138 = (TexSlot*)Slice(slots, rest, lbl_804D413C * 0x18);
    rest -= lbl_804D413C * 0x18;
    lbl_804D4138->state = 4;
    char* extra = 0;
    if (hdr[3] != 0) {
        extra = (char*)Slice(slots + lbl_804D413C * 0x18, rest, hdr[3]);
    }
    for (int i = 0; i < lbl_804D4134; i++) {
        TexEntry* e = &lbl_804D4130[i];
        e->unk20 = (int)extra + e->unk20;
        e->res = (TexRes*)((char*)lbl_804D4138 + (int)e->res * 0x18);
    }
    for (int i = 0; i < lbl_804D4134; i++) {
        TexEntry* e = &lbl_804D4130[i];
        if (e->width != 0) {
            int v = e->link;
            if ((v & 0xFFF) != 0xFFF) {
                v = v & 0xFFF;
            }
            while (v != -1) {
                TexEntry* c = &lbl_804D4130[v];
                if (c->kind == 1) {
                    e->flags |= 0x100;
                }
                if (c->unk02 != 1) {
                    e->unk02 = c->unk02;
                }
                if ((c->flags & 0x600) != 0) {
                    c->flags |= 0x20;
                }
                v = c->link;
                if ((v & 0xFFF) != 0xFFF) {
                    v = v & 0xFFF;
                }
            }
        }
    }
    fn_800891C8();
    return 1;
}
