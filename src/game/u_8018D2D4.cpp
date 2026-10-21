// u_8018D2D4.cpp: ghost shot / ghost projectile effects ("Ghost_apart", "Ghost_impact",
// "Ghost_fire", "Ghost_fire2"). A pool of 64 effect instances is updated every frame, spawned
// with two variants of an initialiser, and drawn as billboards/sprites.
// Names are descriptive guesses; the code is matched against the retail DOL.

typedef unsigned char u8;
typedef unsigned short u16;
typedef unsigned long u32;

struct Vec3 {
    float x, y, z;
};

struct Vec4 {
    float x, y, z, w;
};

struct GhostShot {
    u8 unk000[0x204];
    u8 unk204[0x204];
    Vec3 v408;          // 0x408
    Vec3 v414;          // 0x414
    Vec3 v420;          // 0x420
    Vec3 v42c;          // 0x42c
    Vec3 v438;          // 0x438
    Vec3 v444;          // 0x444
    Vec3 v450;          // 0x450
    Vec3 v45c;          // 0x45c
    Vec3 v468;          // 0x468
    float f474;         // 0x474
    float f478;         // 0x478
    Vec4 c47c;          // 0x47c..0x48b
    Vec4 c48c;          // 0x48c..0x49b
    int i49c;           // 0x49c
    int state;          // 0x4a0
    int i4a4;           // 0x4a4
    float f4a8;         // 0x4a8
    float f4ac;         // 0x4ac
    float f4b0;         // 0x4b0
    float f4b4;         // 0x4b4
    int asset;          // 0x4b8
    int count;          // 0x4bc
    int i4c0;           // 0x4c0
    int i4c4;           // 0x4c4
    int i4c8;           // 0x4c8
    int i4cc;           // 0x4cc
    int i4d0;           // 0x4d0
    float f4d4;         // 0x4d4
    int i4d8;           // 0x4d8
};

static u32 sShotActive[3];              // 0x804958D0
static GhostShot* sShots[65];           // 0x804958DC

// helpers shared with the effect/draw code (addresses in comments)
void* fn_80024834(void* list, int size, void* mask, int count); // 0x80024834
void fn_80024794(void* obj, void* list, void* mask, int count);  // 0x80024794
int AssetFind(u32 a, u32 b);                                    // 0x8007EB34
extern "C" int rand(void);                                      // 0x802F2CFC
float fn_800A1394(float x);                                     // 0x800A1394
float fn_800A1418(float x);                                     // 0x800A1418
void* fn_8016F120(void* fx, int* pos);                          // 0x8016F120
void* FxFind(const char* name);                                 // 0x80170834
float fn_80100EB0(float f, void* a, void* b, void* c, void* d, void* out); // 0x80100EB0
void fn_8013A3D4(void);                                         // 0x8013A3D4
void fn_8013A834(void* pos, float a, float b, u8 r, u8 g, u8 b2, u8 al); // 0x8013A834
void fn_8013AC68(int asset, int mode);                          // 0x8013AC68
void fn_8013B208(void* obj, u8 r, u8 g, u8 b, u8 a);            // 0x8013B208
void fn_8013B5D4(void);                                         // 0x8013B5D4
void fn_8013B6F8(float f, int a, int b);                        // 0x8013B6F8
void fn_8013C76C(void* obj, void* pos);                         // 0x8013C76C
void fn_8013C7D4(void* obj, void* dst, int n, int, int, int, int, int, int, int, int); // 0x8013C7D4

void DrawFlush(void);                                           // 0x8013C030
void DrawVertex(u8, u8, u8, u8, float, float, float, float);    // 0x8013C034
void DrawSetTexture(int tex);                                   // 0x8013C074
void fn_8013C0F4(int tex, u8, u8, u8, u8, float, float, float, float, float, float, float, float); // 0x8013C0F4
void Draw2DEnd(void);                                           // 0x8013C6C4
void Draw2DBegin(void);                                         // 0x8013C6F0

float fn_800E1020(Vec3* v);                                     // 0x800E1020
void fn_800E1098(float f, void* a, void* b);                    // 0x800E1098
int fn_80015B64(int a, void* b, void* c);                       // 0x80015B64
extern "C" int SendMsg(int to, int msg, void* data);            // 0x800265D4
void* fn_800C9414(void);                                        // 0x800C9414
int VarEventInt(int a, u32 ev);                                 // 0x800DEF3C
void fn_800FF294(void);                                         // 0x800FF294
void fn_80190018(short v);                                      // 0x80190018
struct Actor;
Actor* GetPlayer(void);                                         // 0x80049948
void PlayerHud(Actor* a);                                       // 0x80049928

extern int lbl_804D512C;
extern short lbl_804D5122;
extern float gAspectScale;                                      // 0x804D2520

static const float sGhostTimeScale = 1.5f;

int GhostShotSpawnBurst(float a, float b, float c, int d, Vec3* p5, Vec3* p6, Vec3* p7,
                        Vec3* p8, Vec4* p9, Vec4* p10, int k, int l, int m, int n, Vec3* opt);
int GhostShotSpawn(float a, float b, float c, int d, Vec3* p5, Vec3* p6, int state, Vec3* p8,
                   Vec4* p9, Vec4* p10, int k, int l, int m, int n, Vec3* opt);

// 0x8018D2D4
void GhostShotDrawAll(void) {
    int i;

    for (i = 0; i < 64; i++) {
        if (sShotActive[i / 32] & (1 << (i & 31))) {
            int col;

            fn_8013B6F8((float)sShots[i]->i4c0, -4, 0);
            if (sShots[i]->c47c.x == 0.0f) {
                col = 0xaa;
            } else {
                col = 0xff;
            }
            fn_8013B208(sShots[i], col, col, col, col);
            if (sShots[i]->i4d0 != 0) {
                fn_8013B208(sShots[i], col, col, col, col);
            }
            fn_8013B5D4();
            fn_8013B6F8((float)sShots[i]->i4c4, -4, 0);
            fn_8013B208(sShots[i]->unk204, col, col, col, col);
            fn_8013B5D4();

            {
                GhostShot* o = sShots[i];
                Vec3 pos;
                float alpha;

                fn_80100EB0(o->f4a8, &o->v408, &o->v414, &o->v420, &o->v438, &pos);
                if (o->count == 0) {
                    if (o->i4d0 != 0) {
                        alpha = 300.0f;
                    } else {
                        alpha = 112.5f;
                    }
                } else if (o->count <= 20) {
                    alpha = 112.5f + 375.0f * (float)o->count / 20.0f;
                } else {
                    alpha = 487.5f * ((40.0f - (float)o->count) / 40.0f);
                }

                fn_8013AC68(o->asset, 0);
                fn_8013A834(&pos, alpha, 0.0f,
                            (u8)(int)(o->c47c.x * (float)col),
                            (u8)(int)(o->c47c.y * (float)col),
                            (u8)(int)(o->c47c.z * (float)col),
                            (u8)(int)(o->c47c.w * (float)col));
                if (o->f478 > 0.0f) {
                    {
                        float t = o->f478 / o->f474;

                        fn_8013A834(&o->v45c, 121.875f, 0.0f,
                                    (u8)(int)(255.0f * (o->c47c.x * t)),
                                    (u8)(int)(255.0f * (o->c47c.y * t)),
                                    (u8)(int)(255.0f * (o->c47c.z * t)),
                                    (u8)(int)((float)col * t));
                    }
                    {
                        float t = o->f478 / o->f474;

                        fn_8013A834(&o->v45c, 48.75f, 0.0f,
                                    (u8)(int)(255.0f * (o->c47c.x * t)),
                                    (u8)(int)(255.0f * (o->c47c.y * t)),
                                    (u8)(int)(255.0f * (o->c47c.z * t)),
                                    (u8)(int)((float)col * t));
                    }
                }
                fn_8013A3D4();
                fn_8013AC68(o->asset, 0);
                if (o->count > 0 && i % 3 == 0) {
                    fn_8013A834(&pos, alpha * 0.5f, 0.0f, col, col, col, col / 2);
                } else if (o->count == 0) {
                    fn_8013A834(&pos, alpha * 0.5f, 0.0f, col, col, col, col);
                }
                fn_8013A3D4();
            }
        }
    }
}

// 0x8018D79C
int GhostShotUpdateAll(float dt) {
    float scale = 120.0f * dt;
    GhostShot** pp = sShots;
    int i;

    for (i = 0; i < 64; i++, pp++) {
        u32 bits = 1 << (i % 32);

        if (sShotActive[i / 32] & bits) {
            GhostShot* o = *pp;

            if (o->f478 > 0.0f) {
                float t = dt / 1000.0f;

                o->v45c.x += o->v468.x * t;
                o->v45c.y += o->v468.y * t;
                o->v45c.z += o->v468.z * t;
                o->f478 -= dt;
            }
            if (sShotActive[i / 32] & bits) {
                int state = o->state;
                float speed = o->f4ac * dt / 1000.0f;

                if (state == -1) {
                    Vec3 pos;
                    int n;

                    fn_80100EB0(o->f4a8, &o->v408, &o->v414, &o->v420, &o->v438, &pos);
                    n = (int)(scale / 1000.0f);
                    for (state = 0; state < n; state++) {
                        fn_8013C76C(o, &pos);
                        fn_8013C76C(o->unk204, &pos);
                        o->count++;
                    }
                    if (o->count > 39) {
                        fn_80024794(o, sShots, sShotActive, 64);
                    }
                } else if (state == -2) {
                    if (o->i4d0 == 0) {
                        o->v420.x = o->v42c.x + (2.0f * o->f4b0 * ((float)rand() / 32767.0f) - o->f4b0);
                        o->v420.y = o->v42c.y + (2.0f * o->f4b0 * ((float)rand() / 32767.0f) - o->f4b0);
                        o->v420.z = o->v42c.z + (2.0f * o->f4b0 * ((float)rand() / 32767.0f) - o->f4b0);
                    } else {
                        o->v420.x = o->v42c.x + (0.5f * o->f4b0 * ((float)rand() / 32767.0f)
                                                 - o->f4b0 * 0.25f);
                        o->v420.y = o->v42c.y + (0.5f * o->f4b0 * ((float)rand() / 32767.0f)
                                                 - o->f4b0 * 0.25f);
                        o->v420.z = o->v42c.z + (0.5f * o->f4b0 * ((float)rand() / 32767.0f)
                                                 - o->f4b0 * 0.25f);
                    }
                } else {
                    int msg[8];
                    int hit = SendMsg(state, 0x400, msg);

                    if (hit == 0) {
                        fn_80024794(o, sShots, sShotActive, 64);
                        continue;
                    }
                    if (o->i4d0 == 0) {
                        o->v420.x = (float)msg[0] + (2.0f * o->f4b0 * ((float)rand() / 32767.0f)
                                                   - o->f4b0);
                        o->v420.y = (float)msg[1] + (2.0f * o->f4b0 * ((float)rand() / 32767.0f)
                                                   - o->f4b0);
                        o->v420.z = (float)msg[2] + (2.0f * o->f4b0 * ((float)rand() / 32767.0f)
                                                   - o->f4b0);
                    } else {
                        o->v420.x = (float)msg[0] + (0.5f * o->f4b0 * ((float)rand() / 32767.0f)
                                                    - o->f4b0 * 0.25f);
                        o->v420.y = (float)msg[1] + (0.5f * o->f4b0 * ((float)rand() / 32767.0f)
                                                    - o->f4b0 * 0.25f);
                        o->v420.z = (float)msg[2] + (0.5f * o->f4b0 * ((float)rand() / 32767.0f)
                                                    - o->f4b0 * 0.25f);
                    }
                }
                {
                    Vec3 old = o->v450;
                    int done = 0;

                    while (speed > 1.1920929e-7f && o->f4a8 < 1.0f && done == 0) {
                        Vec3 pos;

                        o->f4a8 += 3.5f;
                        if (o->f4a8 > 1.0f) {
                            o->f4a8 = 1.0f;
                        }
                        fn_80100EB0(o->f4a8, &o->v408, &o->v414, &o->v420, &o->v438, &pos);
                        fn_8013C76C(o, &pos);
                        fn_8013C76C(o->unk204, &pos);
                        {
                            Vec3 delta;

                            delta.x = pos.x - old.x;
                            delta.y = pos.y - old.y;
                            delta.z = pos.z - old.z;
                            speed -= fn_800E1020(&delta);
                        }
                        if (o->state != -2) {
                            int ipos[3];

                            ipos[0] = (int)pos.x;
                            ipos[1] = (int)pos.y;
                            ipos[2] = (int)pos.z;
                            done = fn_80015B64(*(int*)(o->unk000 + 0x40), o->unk000, ipos);
                        }
                        old = pos;
                        if (o->state == -2) {
                            continue;
                        }
                        if (o->i4d0 == 0
                            || (o->v420.x - pos.x) * (o->v420.x - pos.x)
                                   + (o->v420.y - pos.y) * (o->v420.y - pos.y)
                                   + (o->v420.z - pos.z) * (o->v420.z - pos.z) > o->f4d4 * o->f4d4) {
                            continue;
                        }
                        if (o->state == -2) {
                            break;
                        }
                        {
                            int n2 = o->i4d8;
                            int j;

                            for (j = 0; j < n2; j++) {
                                Vec3 d;

                                d.x = 3.5f * ((float)rand() / 32767.0f) - 1.0f;
                                d.y = 3.5f * ((float)rand() / 32767.0f) - 1.0f;
                                d.z = (3.5f * ((float)rand() / 32767.0f) - 1.0f) / 2.5f;
                                fn_800E1098(1.3f * (1.0f + (float)rand() / 32767.0f), &d, &d);
                                {
                                    float sp = 1.3f * (1.0f + (float)rand() / 32767.0f);
                                    fn_800E1098(sp, &o->v444, &o->v444);
                                }
                                if (o->state == -2) {
                                    GhostShotSpawnBurst(o->f4ac,
                                                        (float)(10.0f + 90.0f / 4992.0f),
                                                        0.0f, o->i49c, &o->v444, &d, &o->v42c,
                                                        &o->v444, &o->c47c, &o->c48c, o->i4c8,
                                                        o->i4cc, 0, 0, 0);
                                } else {
                                    GhostShotSpawn(o->f4ac,
                                                   (float)(10.0f + 90.0f / 4992.0f),
                                                   0.0f, o->i49c, &o->v444, &d, o->state,
                                                   &o->v444, &o->c47c, &o->c48c, o->i4c8,
                                                   o->i4cc, 0, 0, 0);
                                }
                            }
                            {
                                int ip[3];

                                ip[0] = (int)pos.x;
                                ip[1] = (int)pos.y;
                                ip[2] = (int)pos.z;
                                fn_8016F120(FxFind("Ghost_apart"), ip);
                            }
                            {
                                int hitpos[3];

                                hitpos[0] = (int)o->v420.x;
                                hitpos[1] = (int)o->v420.y;
                                hitpos[2] = (int)o->v420.z;
                                fn_8016F120(FxFind("Ghost_impact"), hitpos);
                            }
                            fn_80024794(o, sShots, sShotActive, 64);
                            break;
                        }
                    }
                }
            }
        }
    }
    return 1;
}

// 0x8018EAA0
int GhostShotFire(float a, float b, float c, int d, Vec3* p5, Vec3* p6, Vec3* p7, Vec3* p8,
                  Vec4* p9, Vec4* p10, int k, int l, int m, int n, Vec3* opt) {
    if (GhostShotSpawnBurst(a, b, c, d, p5, p6, p7, p8, p9, p10, k, l, m, n, opt)) {
        if (m == 0) {
            int pos[3];

            pos[0] = (int)p5->x;
            pos[1] = (int)p5->y;
            pos[2] = (int)p5->z;
            fn_8016F120(FxFind("Ghost_fire"), pos);
        } else {
            int pos2[3];

            pos2[0] = (int)p5->x;
            pos2[1] = (int)p5->y;
            pos2[2] = (int)p5->z;
            fn_8016F120(FxFind("Ghost_fire2"), pos2);
        }
        return 1;
    }
    return 0;
}

// 0x8018EBBC
int GhostShotFire2(float a, float b, float c, int d, Vec3* p5, Vec3* p6, int state, Vec3* p8,
                   Vec4* p9, Vec4* p10, int k, int l, int m, int n, Vec3* opt) {
    if (GhostShotSpawn(a, b, c, d, p5, p6, state, p8, p9, p10, k, l, m, n, opt)) {
        if (m == 0) {
            int pos[3];

            pos[0] = (int)p5->x;
            pos[1] = (int)p5->y;
            pos[2] = (int)p5->z;
            fn_8016F120(FxFind("Ghost_fire"), pos);
        } else {
            int pos2[3];

            pos2[0] = (int)p5->x;
            pos2[1] = (int)p5->y;
            pos2[2] = (int)p5->z;
            fn_8016F120(FxFind("Ghost_fire2"), pos2);
        }
        return 1;
    }
    return 0;
}

// 0x8018ECD8
int GhostShotSpawnBurst(float a, float b, float c, int d, Vec3* p5, Vec3* p6, Vec3* p7, Vec3* p8,
                        Vec4* p9, Vec4* p10, int k, int l, int m, int n, Vec3* opt) {
    GhostShot* o = (GhostShot*)fn_80024834(sShots, 0x4dc, sShotActive, 64);

    if (o == 0) {
        return 0;
    }
    o->v408 = *p5;
    o->v414 = *p6;
    o->v438 = *p8;
    o->v42c = *p7;
    o->v450 = *p5;
    o->state = -2;
    o->f4ac = a;
    o->f4b0 = b;
    o->i49c = d;
    o->c47c = *p9;
    o->c48c = *p10;
    o->i4d0 = m;
    o->f4d4 = c;
    o->i4d8 = n;
    if (opt != 0) {
        o->v444 = *opt;
    }
    o->i4c8 = k;
    o->i4cc = l;
    o->f4a8 = 0.0f;
    o->count = 0;
    o->asset = AssetFind(0xf2e2beb8, 0x2184ad04);
    if (m == 0) {
        o->i4c0 = (int)((float)rand() / 32767.0f * 7.0f) + 13;
        o->i4c4 = (int)((float)rand() / 32767.0f * 4.0f) + 1;
    } else {
        o->i4c0 = (int)((float)rand() / 32767.0f * 5.0f) + 17;
        o->i4c4 = (int)((float)rand() / 32767.0f * 2.0f) + 3;
    }
    o->f478 = 0.0f;
    fn_8013C7D4(o, o->unk000 + 0x24, 40,
                (int)(255.0f * p9->x), (int)(255.0f * p9->y),
                (int)(255.0f * p9->z), (int)(255.0f * p9->w),
                (int)(255.0f * p10->x), (int)(255.0f * p10->y),
                (int)(255.0f * p10->z), (int)(255.0f * p10->w));
    if (p9->x == 0.0f) {
        fn_8013C7D4(o->unk204, o->unk204 + 0x24, 40, 0xaa, 0xaa, 0xaa, 0xaa, 0xaa, 0xaa, 0xaa, 0);
    } else {
        fn_8013C7D4(o->unk204, o->unk204 + 0x24, 40, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0);
    }
    return 1;
}

// 0x8018F130
int GhostShotSpawn(float a, float b, float c, int d, Vec3* p5, Vec3* p6, int state, Vec3* p8,
                   Vec4* p9, Vec4* p10, int k, int l, int m, int n, Vec3* opt) {
    GhostShot* o = (GhostShot*)fn_80024834(sShots, 0x4dc, sShotActive, 64);

    if (o == 0) {
        return 0;
    }
    o->v408 = *p5;
    o->v414 = *p6;
    o->v438 = *p8;
    o->v450 = *p5;
    o->state = state;
    o->f4ac = a;
    o->f4b0 = b;
    o->i49c = d;
    o->c47c = *p9;
    o->c48c = *p10;
    o->i4d0 = m;
    o->f4d4 = c;
    o->i4d8 = n;
    if (opt != 0) {
        o->v444 = *opt;
    }
    o->i4c8 = k;
    o->i4cc = l;
    o->f4a8 = 0.0f;
    o->count = 0;
    o->asset = AssetFind(0xf2e2beb8, 0x2184ad04);
    if (m == 0) {
        o->i4c0 = (int)((float)rand() / 32767.0f * 7.0f) + 13;
        o->i4c4 = (int)((float)rand() / 32767.0f * 4.0f) + 1;
    } else {
        o->i4c0 = (int)((float)rand() / 32767.0f * 5.0f) + 17;
        o->i4c4 = (int)((float)rand() / 32767.0f * 2.0f) + 3;
    }
    o->f478 = 0.0f;
    fn_8013C7D4(o, o->unk000 + 0x24, 40,
                (int)(255.0f * p9->x), (int)(255.0f * p9->y),
                (int)(255.0f * p9->z), (int)(255.0f * p9->w),
                (int)(255.0f * p10->x), (int)(255.0f * p10->y),
                (int)(255.0f * p10->z), (int)(255.0f * p10->w));
    if (p9->x == 0.0f) {
        fn_8013C7D4(o->unk204, o->unk204 + 0x24, 40, 0xaa, 0xaa, 0xaa, 0xaa, 0xaa, 0xaa, 0xaa, 0);
    } else {
        fn_8013C7D4(o->unk204, o->unk204 + 0x24, 40, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0);
    }
    return 1;
}

// 0x8018F56C
void GhostShotDrawBeam(int tex, float t, float v0, float v1, float u0, float u1,
                       int x, int y, int w, int h, u8 r, u8 g, u8 b, u8 al) {
    float fx = (float)x;
    float fy = (float)y;
    float fw = (float)w;
    float fh = (float)h;
    float ang = 360.0f - 360.0f * t;
    float sn = fn_800A1418(ang);
    float cs = fn_800A1394(ang);
    float hw = fw * 0.5f;
    float hh = fh * 0.5f;
    float oV1 = hh - (1.0f - v1) * fh;
    float oV0 = hh - (1.0f - v0) * fh;
    float oU0 = hw - (1.0f - u0) * fw;
    float nV1 = -oV1;
    float nU0 = -oU0;
    float oU1 = hw - (1.0f - u1) * fw;
    float tu0 = 1.0f - u0;
    float tv0 = 1.0f - v0;

    Draw2DBegin();
    DrawSetTexture(tex);
    DrawVertex(r, g, b, al,
               fx + 1.33f * (oV0 * cs - nU0 * sn) / gAspectScale,
               fy + nU0 * cs + oV0 * sn,
               tu0, tv0);
    DrawVertex(r, g, b, al,
               fx + 1.33f * (nV1 * cs - nU0 * sn) / gAspectScale,
               fy + nU0 * cs + nV1 * sn,
               tu0, v1);
    DrawVertex(r, g, b, al,
               fx + 1.33f * (nV1 * cs - oU1 * sn) / gAspectScale,
               fy + oU1 * cs + nV1 * sn,
               u1, v1);
    DrawVertex(r, g, b, al,
               fx + 1.33f * (oV0 * cs - oU1 * sn) / gAspectScale,
               fy + oU1 * cs + oV0 * sn,
               u1, tv0);
    DrawFlush();
    Draw2DEnd();
}

// 0x8018F880
void GhostShotDrawSprite(int tex, int x, int y, int w, int h,
                         u8 r, u8 g, u8 b, u8 a,
                         float u0, float v0, float u1, float v1) {
    Draw2DBegin();

    float x0 = (float)x - 1.33f * ((float)w * 0.5f - (1.0f - u1) * (float)w) / gAspectScale;
    float x1 = (float)x + 1.33f * ((float)w * 0.5f - (1.0f - v1) * (float)w) / gAspectScale;
    float y0 = (float)y - ((float)h * 0.5f - (1.0f - v0) * (float)h);
    float y1 = (float)y + ((float)h * 0.5f - (1.0f - u0) * (float)h);

    fn_8013C0F4(tex, r, g, b, a, x0, y0, x1, y1, 1.0f - u1, 1.0f - u0, v1, v0);
    Draw2DEnd();
}

// 0x8018FA30 SoundPlay(unsigned long)
void SoundPlay(u32 id) {
    int v = 0;
    void* p = fn_800C9414();

    (void)id;
    if (p != 0) {
        v = *(int*)((char*)p + 0xc0);
    }
    lbl_804D512C = VarEventInt(v, 0xeaa273e0);
    if (lbl_804D512C == 0 || lbl_804D512C == -1) {
        lbl_804D512C = VarEventInt(v, 0x94001120);
    }
    lbl_804D5122 = VarEventInt(v, 0x7cce3f4d);
}

// 0x8018FAB4
int GhostShotExit(void) {
    fn_80190018(lbl_804D5122);
    PlayerHud(GetPlayer());
    fn_800FF294();
    return 1;
}
