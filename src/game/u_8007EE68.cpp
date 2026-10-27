// u_8007EE68.cpp: the font system (0x8007EE68-0x80080714).
// A font is created from one of the built-in font scripts (the table below), runs 0x1C-byte
// bytecode instructions and appends 0x18-byte render nodes to one of eight per-layer queues.
// Names are descriptive guesses; the code is matched against the retail DOL.

typedef unsigned long u32;
typedef unsigned char u8;

struct Font;
struct FontDesc;
struct FontNode;

typedef int (*FontCallback)(int, int, void*);

extern "C" {
void* memset(void* dst, int val, unsigned long n);
void* memcpy(void* dst, const void* src, unsigned long n);
int stricmp(const char* a, const char* b);
int SendMsg(int to, int msg, void* data);

void fn_80026660(int msg, int (*cb)(void*, int, void*), int arg);
int fn_80215C04(void);
void fn_80098FB4(int a, int b);
void fn_80098C2C(int a, int b, int c);
void fn_800988A8(int a, float b);
void fn_8009851C(int r, int g, int b);
void fn_800983FC(int r, int g, int b);
void fn_80098244(int r, int g, int b);
void fn_80096D5C(float a);
void fn_800967B8(int a);
void fn_80096584(int a);
void fn_80096B30(int r, int g, int b);
void fn_8009877C(void* v, int a, float b, float c);
void fn_800980F8(int a, float b);
void fn_80094E20(float a, float b, float c);
float fn_800A1394(float a);
int fn_800A1624(int a);
int fn_802A1DAC(void);
}

void* GetPlayer(void);
int Rand(void);

struct FontInst {
    float time; // 0x00
    int op;     // 0x04
    int a;      // 0x08
    int b;      // 0x0C
    int c;      // 0x10
    int d;      // 0x14
    int e;      // 0x18
};

union FontValue {
    int i;
    float f;
};

struct FontNode {
    int op;          // 0x00
    int id;          // 0x04
    u8 r;            // 0x08
    u8 g;            // 0x09
    u8 b;            // 0x0A
    u8 pad0B;        // 0x0B
    FontValue x;     // 0x0C
    FontValue y;     // 0x10
    FontNode* next;  // 0x14
};

struct FontQueue {
    FontNode* head; // 0x00
    FontNode* tail; // 0x04
};

struct Font {
    int layer;         // 0x00
    int id;            // 0x04
    FontInst* code;    // 0x08
    int pc;            // 0x0C
    int size;          // 0x10
    float scale;       // 0x14
    int p18;           // 0x18
    int p1C;           // 0x1C
    int p20;           // 0x20
    int p24;           // 0x24
    int p28;           // 0x28
    FontCallback cb;   // 0x2C
    void* cbData;      // 0x30
    int target;        // 0x34
    int unk38;         // 0x38
    int next;          // 0x3C
    int prev;          // 0x40
};

struct FontDesc {
    int unk0;          // 0x00
    int target;        // 0x04
    int unk8;          // 0x08
    int layer;         // 0x0C
    int id;            // 0x10
    const char* name;  // 0x14
    float scale;       // 0x18
    int a;             // 0x1C
    int b;             // 0x20
    int c;             // 0x24
    int d;             // 0x28
    int e;             // 0x2C
    int handle;        // 0x30
};

struct FontDef {
    const char* name;
    void* data;
    u32 size;
};

struct FontPool {
    u8 code[16][0x540];
    FontNode nodes[32];
};

extern char lbl_803436B7[];
extern char lbl_803436BF[];
extern char lbl_803436C8[];
extern char lbl_803436D2[];
extern char lbl_803436DD[];
extern char lbl_803436E5[];
extern char lbl_803436EE[];
extern char lbl_803436F7[];
extern char lbl_80343701[];
extern char lbl_8034370F[];
extern char lbl_8034371E[];
extern char lbl_80343726[];
extern char lbl_80343731[];
extern char lbl_80343739[];
extern char lbl_8034373E[];
extern char lbl_80343748[];
extern char lbl_80343757[];
extern char lbl_80343760[];
extern char lbl_8034376B[];
extern char lbl_80343775[];
extern char lbl_80343784[];
extern char lbl_80343790[];
extern char lbl_8034379D[];
extern char lbl_803437A6[];
extern char lbl_803437AE[];
extern char lbl_803437B5[];
extern char lbl_803437BE[];
extern char lbl_803437CB[];
extern char lbl_803437D7[];
extern char lbl_803437DF[];
extern char lbl_803437EB[];
extern char lbl_803437F5[];
extern char lbl_803437FF[];
extern char lbl_8034380B[];
extern char lbl_80343815[];
extern char lbl_8034381B[];
extern char lbl_80343826[];
extern char lbl_8034382D[];
extern char lbl_80343838[];
extern char lbl_80343842[];
extern char lbl_80343847[];

extern u8 lbl_80395278[];
extern u8 lbl_80395390[];
extern u8 lbl_80395438[];
extern u8 lbl_80395550[];
extern u8 lbl_80395F60[];
extern u8 lbl_80396008[];
extern u8 lbl_80396120[];
extern u8 lbl_803962C4[];
extern u8 lbl_803961C8[];
extern u8 lbl_803963DC[];
extern u8 lbl_80395CA4[];
extern u8 lbl_80395D4C[];
extern u8 lbl_80395E48[];
extern u8 lbl_80395EB8[];
extern u8 lbl_803955F8[];
extern u8 lbl_803956BC[];
extern u8 lbl_80395BE0[];
extern u8 lbl_8039595C[];
extern u8 lbl_80395A04[];
extern u8 lbl_80395B38[];
extern u8 lbl_803964F4[];
extern u8 lbl_803965D4[];
extern u8 lbl_803966B4[];
extern u8 lbl_803967CC[];
extern u8 lbl_80396874[];
extern u8 lbl_8039691C[];
extern u8 lbl_80396B68[];
extern u8 lbl_80396C80[];
extern u8 lbl_80396D28[];
extern u8 lbl_80396D44[];
extern u8 lbl_80396E40[];
extern u8 lbl_80396F04[];
extern u8 lbl_80396F90[];
extern u8 lbl_803970FC[];
extern u8 lbl_803976AC[];
extern u8 lbl_803973D4[];
extern u8 lbl_80397A64[];
extern u8 lbl_80397C94[];
extern u8 lbl_80397D3C[];
extern u8 lbl_80397BEC[];
extern u8 lbl_80397DE4[];

FontDef sFontDefs[41] = {
    { lbl_803436B7, lbl_80395278, 0x118 },
    { lbl_803436BF, lbl_80395390, 0xA8 },
    { lbl_803436C8, lbl_80395438, 0x118 },
    { lbl_803436D2, lbl_80395550, 0xA8 },
    { lbl_803436DD, lbl_80395F60, 0xA8 },
    { lbl_803436E5, lbl_80396008, 0x118 },
    { lbl_803436EE, lbl_80396120, 0xA8 },
    { lbl_803436F7, lbl_803962C4, 0x118 },
    { lbl_80343701, lbl_803961C8, 0xFC },
    { lbl_8034370F, lbl_803963DC, 0x118 },
    { lbl_8034371E, lbl_80395CA4, 0xA8 },
    { lbl_80343726, lbl_80395D4C, 0xFC },
    { lbl_80343731, lbl_80395E48, 0x70 },
    { lbl_80343739, lbl_80395EB8, 0xA8 },
    { lbl_8034373E, lbl_803955F8, 0xC4 },
    { lbl_80343748, lbl_803956BC, 0x2A0 },
    { lbl_80343757, lbl_80395BE0, 0xC4 },
    { lbl_80343760, lbl_8039595C, 0xA8 },
    { lbl_8034376B, lbl_80395A04, 0x134 },
    { lbl_80343775, lbl_80395B38, 0x134 },
    { lbl_80343784, lbl_803964F4, 0xE0 },
    { lbl_80343790, lbl_803965D4, 0xE0 },
    { lbl_8034379D, lbl_803966B4, 0x118 },
    { lbl_803437A6, lbl_803967CC, 0xA8 },
    { lbl_803437AE, lbl_80396874, 0xA8 },
    { lbl_803437B5, lbl_8039691C, 0x24C },
    { lbl_803437BE, lbl_80396B68, 0x118 },
    { lbl_803437CB, lbl_80396C80, 0xA8 },
    { lbl_803437D7, lbl_80396D28, 0x1C },
    { lbl_803437DF, lbl_80396D44, 0xFC },
    { lbl_803437EB, lbl_80396E40, 0xC4 },
    { lbl_803437F5, lbl_80396F04, 0x8C },
    { lbl_803437FF, lbl_80396F90, 0x16C },
    { lbl_8034380B, lbl_803970FC, 0x2D8 },
    { lbl_80343815, lbl_803976AC, 0x3B8 },
    { lbl_8034381B, lbl_803973D4, 0x2D8 },
    { lbl_80343826, lbl_80397A64, 0x188 },
    { lbl_8034382D, lbl_80397C94, 0xA8 },
    { lbl_80343838, lbl_80397D3C, 0xA8 },
    { lbl_80343842, lbl_80397BEC, 0xA8 },
    { lbl_80343847, lbl_80397DE4, 0xA8 },
};

Font sFonts[16];
u8 sFontCode[16][0x540];
FontNode sNodes[32];
FontQueue sQueues[8];

float sRedScale = 1.0f;
float sGreenScale = 1.0f;
float sBlueScale = 1.0f;

int sFreeFont;
int sFontList;
FontNode* sFreeNode;
FontQueue* sQueue;
FontNode* sNode;

inline void SetNode(int op, int x) {
    sNode->op = op;
    sNode->x.i = x;
}

inline void SetNode(int op, int x, int y) {
    sNode->op = op;
    sNode->x.i = x;
    sNode->y.i = y;
}

inline void SetNodeFloat(int op, float x) {
    sNode->op = op;
    sNode->x.f = x;
}

inline void SetNodeColor(int op, int r, int g, int b) {
    sNode->op = op;
    sNode->r = r;
    sNode->g = g;
    sNode->b = b;
}

inline void SetNodeColorScaled(int op, u8 r, u8 g, u8 b) {
    sNode->op = op;
    sNode->r = (u8)(r * sRedScale);
    sNode->g = (u8)(g * sGreenScale);
    sNode->b = (u8)(b * sBlueScale);
}

inline void SetNodeText(int op, float y, float x, int r, int g, int b) {
    sNode->op = op;
    sNode->y.f = y;
    sNode->x.f = x;
    sNode->r = r;
    sNode->g = g;
    sNode->b = b;
}

void fn_8007EE68(void) {}

void fn_8007EE6C(int index, int a, int b, int c, int d, int e) {
    if (index < 0) {
        return;
    }
    if (index >= 16) {
        return;
    }
    sFonts[index].p18 = a;
    sFonts[index].p1C = b;
    sFonts[index].p20 = c;
    sFonts[index].p24 = d;
    sFonts[index].p28 = e;
}

void FontRelease(int index) {
    if (index < 0) {
        return;
    }
    if (index >= 16) {
        return;
    }
    if (sFonts[index].code == 0) {
        return;
    }
    sFonts[index].code = 0;
    if (sFontList == index) {
        sFontList = sFonts[index].next;
    } else {
        sFonts[sFonts[index].prev].next = sFonts[index].next;
    }
    if (sFonts[index].next != -1) {
        sFonts[sFonts[index].next].prev = sFonts[index].prev;
    }
    sFonts[index].next = sFreeFont;
    sFreeFont = index;
}

int FontCreate(FontDesc* d, FontCallback cb, void* cbdata);

int fn_8007EF48(int id, int layer, const char* name, int a, int b, int c, int d, int e, int cb,
                int cbdata) {
    FontDesc desc;

    desc.id = id;
    desc.layer = layer;
    desc.name = name;
    desc.scale = 0.0f;
    desc.a = a;
    desc.b = b;
    desc.c = c;
    desc.d = d;
    desc.e = e;
    return FontCreate(&desc, (FontCallback)cb, (void*)cbdata);
}

int fn_8007EFA4(int id, int layer, const char* name, int a, int b, int cb, int cbdata) {
    FontDesc desc;

    desc.target = -1;
    desc.id = id;
    desc.layer = layer;
    desc.name = name;
    desc.scale = 0.0f;
    desc.a = a;
    desc.b = b;
    desc.c = 0;
    desc.d = 0;
    desc.e = 0;
    return FontCreate(&desc, (FontCallback)cb, (void*)cbdata);
}

int FontLoad(int layer, const char* name, int a, int b, int cb, int cbdata) {
    FontDesc desc;

    desc.id = -1;
    desc.target = -1;
    desc.layer = layer;
    desc.name = name;
    desc.scale = 0.0f;
    desc.a = a;
    desc.b = b;
    desc.c = 0;
    desc.d = 0;
    desc.e = 0;
    return FontCreate(&desc, (FontCallback)cb, (void*)cbdata);
}

void FontRender(int id);

void fn_8007F064(int layer, int id) {
    GetPlayer();
    if (fn_80215C04() != 0) {
        return;
    }
    if (layer < 0) {
        return;
    }
    if (layer >= 8) {
        return;
    }
    sQueue = &sQueues[layer];
    FontRender(id);
}

int FontRun(Font* f, float dt);

void fn_8007F0D4(float dt) {
    FontNode* n;
    FontNode* prev;
    int i;

    prev = 0;
    for (i = 0; i < 32; i++) {
        sNodes[i].next = prev;
        prev = &sNodes[i];
    }
    sFreeNode = 0;
    sQueue = 0;
    sNode = 0;
    for (i = 0; i < 8; i++) {
        sQueues[i].head = 0;
        sQueues[i].tail = 0;
    }
    sFreeNode = prev;

    i = sFontList;
    while (i != -1) {
        int next = sFonts[i].next;
        sQueue = &sQueues[sFonts[i].layer];
        if (sFreeNode != 0) {
            n = sFreeNode;
            sFreeNode = n->next;
            n->next = 0;
            if (sQueue->tail != 0) {
                sQueue->tail->next = n;
                sQueue->tail = n;
            } else {
                sQueue->head = n;
                sQueue->tail = n;
            }
        } else {
            n = 0;
        }
        sNode = n;
        if (n != 0) {
            n->op = 9;
            sNode->id = sFonts[i].id;
            if (FontRun(&sFonts[i], dt) == 0) {
                FontRelease(i);
            }
        }
        i = next;
    }
}

int FontMsg(void* self, int msg, void* data);

void fn_8007F3A4(void) {
    int i;

    fn_80026660(0x3D, FontMsg, 0);
    memset(sFonts, 0, sizeof(sFonts));
    for (i = 0; i < 16; i++) {
        sFonts[i].next = i + 1;
        sFonts[i].prev = -1;
    }
    sFonts[15].next = -1;
    sFreeFont = 0;
    sFontList = -1;
}

int FontDone(int a, int b, void* user);

int FontMsg(void* self, int msg, void* data) {
    switch (msg) {
    case 0x6000: {
        FontDesc* d = (FontDesc*)data;
        if (d->target != -1) {
            d->handle = FontCreate(d, FontDone, 0);
            sFonts[d->handle].cbData = &sFonts[d->handle];
            sFonts[d->handle].unk38 = d->unk8;
        } else {
            d->handle = FontCreate(d, 0, 0);
        }
        break;
    }
    case 0x6001:
        FontRelease(((FontDesc*)data)->target);
        break;
    default:
        return 0;
    }
    return 1;
}

struct FontDoneMsg {
    int unk0;
    int unk4;
    int unk8;
    int unkC;
    int unk10;
    int reserved[3];
};

int FontDone(int a, int b, void* user) {
    Font* f;
    FontDoneMsg m;

    f = (Font*)user;
    m.unk0 = 0;
    m.unk4 = a;
    m.unk8 = b;
    m.unkC = f->unk38;
    m.unk10 = 0;
    SendMsg(f->target, 0x6002, &m);
    return m.unk10;
}

int FontCreate(FontDesc* d, FontCallback cb, void* cbdata) {
    int index;
    int cur;
    unsigned int i;

    if (d->layer < 0 && d->layer >= 8) {
        return -1;
    }
    for (i = 0; i < 41; i++) {
        if (stricmp(d->name, sFontDefs[i].name) == 0) {
            break;
        }
    }
    if (i == 41) {
        return -1;
    }
    if ((index = sFreeFont) == -1) {
        return -1;
    }
    sFreeFont = sFonts[index].next;
    sFonts[index].code = (FontInst*)sFontCode[index];
    sFonts[index].pc = 0;
    sFonts[index].layer = d->layer;
    memcpy(sFonts[index].code, sFontDefs[i].data, sFontDefs[i].size);
    sFonts[index].size = sFontDefs[i].size;
    sFonts[index].scale = d->scale;
    sFonts[index].p18 = d->a;
    sFonts[index].p1C = d->b;
    sFonts[index].p20 = d->c;
    sFonts[index].p24 = d->d;
    sFonts[index].p28 = d->e;
    sFonts[index].cb = cb;
    sFonts[index].cbData = cbdata;
    sFonts[index].target = d->target;
    sFonts[index].id = d->id;

    if (sFontList == -1) {
        sFontList = index;
        sFonts[index].prev = -1;
        sFonts[index].next = -1;
    } else {
        cur = sFontList;
        while (cur != -1) {
            if (sFonts[cur].layer < d->layer) {
                sFonts[index].prev = sFonts[cur].prev;
                if (sFonts[index].prev == -1) {
                    sFontList = index;
                } else {
                    sFonts[sFonts[index].prev].next = index;
                }
                sFonts[cur].prev = index;
                sFonts[index].next = cur;
                break;
            }
            if (sFonts[cur].next == -1) {
                sFonts[cur].next = index;
                sFonts[index].prev = cur;
                sFonts[index].next = -1;
                break;
            }
            cur = sFonts[cur].next;
        }
    }
    return index;
}

int FontRun(Font* s, float dt) {
    FontInst* inst;
    FontInst* dst;
    FontInst* lo;
    FontInst* hi;
    int flag;
    int diff;
    float fdiff;

    inst = &s->code[s->pc];
    if (s->code == 0) {
        return 0;
    }
    switch (inst->op) {
    case 0:
        s->pc = inst->a;
        return FontRun(s, dt);
    case 1:
        if (s->cb != 0) {
            s->cb(0, 0, s->cbData);
        }
        return 0;
    case 8:
        dst = &s->code[inst->a];
        lo = &s->code[s->pc + 1];
        hi = &s->code[s->pc + 2];
        fdiff = hi->time - lo->time;
        if (fdiff != 0.0f) {
            dst->time = lo->time + (float)(Rand() % (int)fdiff);
        }
        diff = hi->a - lo->a;
        if (diff != 0) {
            dst->a = lo->a + Rand() % diff;
        }
        diff = hi->b - lo->b;
        if (diff != 0) {
            dst->b = lo->b + Rand() % diff;
        }
        diff = hi->c - lo->c;
        if (diff != 0) {
            dst->c = lo->c + Rand() % diff;
        }
        diff = hi->d - lo->d;
        if (diff != 0) {
            dst->d = lo->d + Rand() % diff;
        }
        diff = hi->e - lo->e;
        if (diff != 0) {
            dst->e = lo->e + Rand() % diff;
        }
        s->pc += 3;
        return FontRun(s, dt);
    case 5:
        dst = &s->code[inst->a];
        lo = &s->code[s->pc + 1];
        dst->time = lo->time;
        dst->a = lo->a;
        dst->b = lo->b;
        dst->c = lo->c;
        dst->d = lo->d;
        dst->e = lo->e;
        s->pc += 2;
        return FontRun(s, dt);
    case 11:
        dst = &s->code[inst->a];
        lo = &s->code[s->pc + 1];
        if (lo->time != 0.0f) {
            dst->time = s->scale * lo->time;
        }
        if (lo->a != 0) {
            dst->a = s->p18 * lo->a;
        }
        if (lo->b != 0) {
            dst->b = s->p1C * lo->b;
        }
        if (lo->c != 0) {
            dst->c = s->p20 * lo->c;
        }
        if (lo->d != 0) {
            dst->d = s->p24 * lo->d;
        }
        if (lo->e != 0) {
            dst->e = s->p28 * lo->e;
        }
        s->pc += 2;
        return FontRun(s, dt);
    case 2:
        lo = &s->code[s->pc - 1];
        hi = &s->code[s->pc + 1];
        flag = 0;
        if (hi->a == -1) {
            lo->a += inst->a;
            lo->a &= 0xFFF;
        } else {
            lo->a += inst->a;
            if (inst->a < 0) {
                if (lo->a < hi->a) {
                    flag = 1;
                }
            } else if (inst->a > 0) {
                if (lo->a > hi->a) {
                    flag = 1;
                }
            }
        }
        if (hi->b == -1) {
            lo->b += inst->b;
            lo->b &= 0xFFF;
        } else {
            lo->b += inst->b;
            if (inst->b < 0) {
                if (lo->b < hi->b) {
                    flag = 1;
                }
            } else if (inst->b > 0) {
                if (lo->b > hi->b) {
                    flag = 1;
                }
            }
        }
        lo->c += inst->c;
        if (inst->c < 0) {
            if (lo->c < hi->c) {
                flag = 1;
            }
        } else if (inst->c > 0) {
            if (lo->c > hi->c) {
                flag = 1;
            }
        }
        lo->d += inst->d;
        if (inst->d < 0) {
            if (lo->d < hi->d) {
                flag = 1;
            }
        } else if (inst->d > 0) {
            if (lo->d > hi->d) {
                flag = 1;
            }
        }
        lo->e += inst->e;
        if (inst->e < 0) {
            if (lo->e < hi->e) {
                flag = 1;
            }
        } else if (inst->e > 0) {
            if (lo->e > hi->e) {
                flag = 1;
            }
        }
        if (flag == 1) {
            s->pc += 2;
        } else {
            s->pc -= 1;
            lo->time = hi->time;
        }
        return FontRun(s, dt);
    case 4:
        if (s->cb != 0) {
            if (s->cb(1, 0, s->cbData) == 0) {
                return 0;
            }
            s->pc++;
            return FontRun(s, dt);
        }
        // fall through
    case 20:
        SetNode(0, inst->a);
        break;
    case 32:
        SetNode(0x0C, inst->a, inst->b);
        break;
    case 21:
        SetNode(1, inst->a, inst->b);
        break;
    case 22:
        SetNodeFloat(2, (float)inst->a / 4096.0f);
        break;
    case 31:
        SetNodeText(0x0B, (float)inst->a / 4096.0f, (float)inst->b / 4096.0f,
                    inst->c, inst->d, inst->e);
        break;
    case 29:
        SetNodeFloat(2, (float)inst->b *
                            ((float)fn_800A1624(inst->a) / 4096.0f) / 4096.0f);
        break;
    case 23:
        SetNodeColorScaled(3, inst->c, inst->d, inst->e);
        break;
    case 33:
        SetNodeColor(0x0D, inst->c, inst->d, inst->e);
        break;
    case 24:
        SetNodeColor(4, inst->c, inst->d, inst->e);
        break;
    case 34:
        SetNodeColor(0x0E, inst->c, inst->d, inst->e);
        break;
    case 35:
        SetNode(0x0F, inst->a, inst->b);
        break;
    case 30:
        SetNodeColor(0x0A, inst->c, inst->d, inst->e);
        break;
    case 25:
        SetNodeColor(5, inst->c, inst->d, inst->e);
        break;
    case 26:
        SetNodeFloat(6, (float)inst->a / 4096.0f);
        break;
    case 27:
        SetNode(7, inst->a);
        break;
    case 28:
        SetNode(8, inst->a);
        break;
    }

    inst->time -= dt;
    if (inst->time < 0.0f) {
        dt = -inst->time;
        inst->time = 0.0f;
        s->pc++;
        if (dt >= 1.1920929e-07f) {
            return FontRun(s, dt);
        }
    }
    return 1;
}

struct FontVec3 {
    float x, y, z;
};

void FontRender(int id) {
    FontNode* node;
    FontVec3 v;
    float cr;
    float cg;
    float cb;
    float frac;
    float val;

    for (node = sQueue->head; node != 0; node = node->next) {
        if (node->id != id) {
            continue;
        }
        switch (node->op) {
        case 12:
            fn_800980F8(0, (float)node->y.i / 2048.0f);
            fn_80094E20(0.1f * ((float)node->x.i / 22.0f), 0.0f,
                        (float)node->y.i / 4096.0f);
            break;
        case 0:
            fn_80098FB4(node->x.i, 0);
            break;
        case 1:
            fn_80098C2C(node->x.i, node->y.i, 0);
            break;
        case 2:
            fn_800988A8(0, node->x.f);
            break;
        case 11:
            v.x = ((float)node->r - 127.0f) / 255.0f;
            v.y = ((float)node->g - 127.0f) / 255.0f;
            v.z = ((float)node->b - 127.0f) / 255.0f;
            fn_8009877C(&v, 0, node->y.f, node->x.f);
            break;
        case 3:
            fn_8009851C(node->r, node->g, node->b);
            break;
        case 4:
            fn_800983FC(node->r, node->g, node->b);
            break;
        case 14:
            cr = fn_800A1394(90.0f * node->r / 255.0f);
            cg = fn_800A1394(90.0f * node->g / 255.0f);
            cb = fn_800A1394(90.0f * node->b / 255.0f);
            fn_80094E20(0.015 * cr, 0.015 * cr, 0.9f);
            fn_80094E20(0.01 * cr, 0.01 * cr, 0.95f);
            fn_80094E20(0.008 * cr, 0.008 * cr, 1.0f);
            fn_800983FC((u8)(cr * cr * 255.0f), (u8)(cg * cg * 255.0f),
                        (u8)(cb * cb * 255.0f));
            break;
        case 15:
            if (fn_802A1DAC() != 3) {
                frac = (float)node->x.i / (float)node->y.i;
                val = fn_800A1394(540.0f * frac);
                frac = (1.0f - frac) * val;
                fn_80094E20(0.015 * frac, 0.015 * frac, 0.9f);
                fn_80094E20(0.01 * frac, 0.01 * frac, 0.95f);
                fn_80094E20(0.008 * frac, 0.008 * frac, 1.0f);
            }
            break;
        case 13:
            fn_80096B30(node->r, node->g, node->b);
            break;
        case 5:
            fn_80098244(node->r, node->g, node->b);
            break;
        case 6:
            fn_80096D5C(node->x.f);
            break;
        case 7:
            fn_800967B8(node->x.i);
            break;
        case 8:
            fn_80096584(node->x.i);
            break;
        case 10:
            fn_8009851C(node->r, node->g, node->b);
            fn_800983FC(node->r, node->g, node->b);
            break;
        }
    }
}
