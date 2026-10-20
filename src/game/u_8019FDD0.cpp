// Reflection rendering helpers (0x8019FDD0-0x801A0F2C): accumulate mirror time, handle the
// reflection actor's message, set up the reflection texture matrix, finish/restore the reflection
// pass, render one reflection entry (with and without a second actor) and update the camera state.
// Names are descriptive guesses; the code is matched against the retail DOL.

typedef unsigned char u8;
typedef unsigned long u32;

struct Vec3 {
    float x, y, z;
};

struct Mtx33 {
    float m[3][3];
};

struct Mtx34 {
    float m[3][4];
};

struct Actor {
    int matrix[12];        // 0x00: 3x3 basis (1/4096) then translation
    u8 pad30[5];           // 0x30
    char flag35;           // 0x35
    u8 pad36[0x12];        // 0x36
    int unk48;             // 0x48
};

struct ReflectEntry {
    int active;            // 0x00
    int lastValue;         // 0x04
    int id;                // 0x08
    Actor* actor;          // 0x0C
    Actor* actor2;         // 0x10
    int msgTarget;         // 0x14
    Mtx34 texMtx;          // 0x18
    u8 pad48[0x18];        // 0x48
};

struct ReflectState {
    Mtx34 view;            // 0x00
    Mtx34 proj;            // 0x30
    Mtx34 cam;             // 0x60
    Vec3 target;           // 0x90
    ReflectEntry entries[8]; // 0x9C
};

struct Camera {
    Mtx34 view;            // 0x00
    Mtx34 proj;            // 0x30
};

struct NotifyMsg {
    int unk0;
    int unk4;
    int unk8;
    int unkC;
};

extern "C" {
int lbl_804D5160;
int lbl_804D514C;
int lbl_804D5148;
int lbl_804D5150;
int lbl_804D5154;
float lbl_804D5158;
int lbl_804D515C;
int lbl_804D308C;
float lbl_804D3088;
int lbl_804D4084;
int lbl_804D2568;
int lbl_804D256C;
ReflectState lbl_80495CE8;
Vec3 lbl_80495D78;      // aliases lbl_80495CE8.target
Mtx34 lbl_80449068;

int SendMsg(int to, int msg, void* data);

void fn_80009170(int* pos, int* end, int r, int g, int b);
void fn_800091D4(Mtx34* a, Mtx34* b, Mtx33* out, int count);
void fn_80009848(float fov, Mtx34* texMtx, Mtx34* mtx);
void fn_80009E44(int a, int b);
void fn_8004320C(Mtx34* src, Mtx34* dst);
void fn_8009F7A8(Mtx34* mtx, Vec3* up);
void fn_800A0094(Vec3* out, Vec3* in);
void fn_800A0100(Mtx34* mtx);
void fn_800A0168(Mtx34* out, Mtx34* in);
void* fn_8000F990(int id);
void* fn_800C5DD8(void);
int fn_800C5E74(int a);
void fn_800C5E9C(int a);
void fn_800C6200(void* buf);
void* fn_800C6448(void);
void fn_800C6664(int a);
void fn_800C6834(int a);
void fn_8009863C(float f, int a);
void fn_8010F7EC(void);
void fn_80115F28(int a);

void GXLoadTexMtxImm(Mtx34* mtx, u32 id, u32 type);
void GXSetNumTexGens(u8 count);
void GXSetTexCoordGen2(int coord, int func, int src, int mtx, int normalize, int postMtx);
void GXSetTexCopySrc(int left, int top, int width, int height);
void GXSetTexCopyDst(int width, int height, int format, int mipmap);
void GXSetZMode(int compare, int func, int update);
void* GXGetTexObjData(void* tex);
void GXCopyTex(void* data, int clear);
void GXInitTexObjLOD(void* tex, int minFilt, int magFilt, float minLod, float maxLod,
                     float lodBias, int biasClamp, int edgeLod, int maxAniso);
void GXPixModeSync(void);
void GXInvalidateTexAll(void);
void GXSetViewport(float left, float top, float width, float height, float nearZ, float farZ);
void GXSetScissor(int left, int top, int width, int height);

void PSMTXReflect(Mtx34* out, Vec3* pos, Vec3* normal);
void PSMTXConcat(Mtx34* a, Mtx34* b, Mtx34* out);
}

void GfxSetCullMode(int mode);
void ActorGetMatrix(Mtx34* out, Actor* actor);

void ReflectAddTime(float dt) {
    lbl_804D5160 += (int)dt;
}

void ReflectActorNotify(Actor* actor, int msg) {
    switch (msg) {
    case 0x21C5F0:
        actor->flag35 = 1;
        break;
    }
}

Vec3* ReflectGetTarget(void) {
    return &lbl_80495D78;
}

void ReflectSetupTexMtx(int id) {
    int i;

    for (i = 0; i < lbl_804D514C; i++) {
        ReflectEntry* entry = &lbl_80495CE8.entries[i];

        if (entry->id == id && entry->active) {
            GXLoadTexMtxImm(&entry->texMtx, 0x1E, 0);
            GXSetNumTexGens(1);
            GXSetTexCoordGen2(0, 0, 0, 0x1E, 0, 0x7D);
            fn_8010F7EC();
            if (lbl_804D515C) {
                int pos[3];
                int end[3];

                pos[0] = entry->actor->matrix[9];
                pos[1] = entry->actor->matrix[10];
                pos[2] = entry->actor->matrix[11];
                end[0] = pos[0] + entry->actor->matrix[0] / 64;
                end[1] = pos[1] + entry->actor->matrix[1] / 64;
                end[2] = pos[2] + entry->actor->matrix[2] / 64;
                fn_80009170(pos, end, 255, 0, 0);

                pos[0] = entry->actor->matrix[9];
                pos[1] = entry->actor->matrix[10];
                pos[2] = entry->actor->matrix[11];
                end[0] = pos[0] + entry->actor->matrix[3] / 64;
                end[1] = pos[1] + entry->actor->matrix[4] / 64;
                end[2] = pos[2] + entry->actor->matrix[5] / 64;
                fn_80009170(pos, end, 255, 255, 0);

                pos[0] = entry->actor->matrix[9];
                pos[1] = entry->actor->matrix[10];
                pos[2] = entry->actor->matrix[11];
                end[0] = pos[0] + entry->actor->matrix[6] / 64;
                end[1] = pos[1] + entry->actor->matrix[7] / 64;
                end[2] = pos[2] + entry->actor->matrix[8] / 64;
                fn_80009170(pos, end, 0, 0, 255);
            }
            break;
        }
    }
}

void ReflectEnd(void) {
    void* tex;
    NotifyMsg msg;

    if (lbl_804D308C == -1) return;

    lbl_804D4084 = 0;
    GfxSetCullMode(1);

    switch (lbl_80495CE8.entries[lbl_804D308C].actor->flag35) {
    case 1:
        if (lbl_804D3088 != 0.0f) {
            fn_8009863C((float)(lbl_804D5160 * 3), 0);
        }
        break;
    }

    tex = fn_8000F990(lbl_80495CE8.entries[lbl_804D308C].id);
    if (tex) {
        fn_80115F28(2);
        GXSetTexCopySrc(0, 0, 480, 336);
        GXSetTexCopyDst(480, 336, 4, 0);
        GXSetZMode(1, 7, 1);
        GXCopyTex(GXGetTexObjData(tex), 1);
        GXInitTexObjLOD(tex, 1, 1, 0.0f, 0.0f, 0.0f, 0, 0, 0);
        GXPixModeSync();
        GXInvalidateTexAll();
    }

    lbl_804D2568 = lbl_804D5150;
    lbl_804D256C = lbl_804D5154;
    fn_80009E44(0, 0);

    if (lbl_80495CE8.entries[lbl_804D308C].msgTarget != -1) {
        msg.unk4 = 0;
        SendMsg(lbl_80495CE8.entries[lbl_804D308C].msgTarget, 0xC00, &msg);
    }

    fn_800C6664(lbl_804D5148);
    fn_800C5E9C(0);
    lbl_804D308C = -1;
}

int ReflectRender(int index) {
    Mtx33 tmp;
    Mtx34 m180;
    Mtx34 refl;
    ReflectEntry* entry;
    void* tex;

    tex = fn_8000F990(lbl_80495CE8.entries[index].id);
    entry = &lbl_80495CE8.entries[index];
    if (!entry->active || tex == 0) {
        lbl_804D308C = -1;
        return 0;
    }

    lbl_804D4084 = 1;
    GXSetViewport(0.0f, 0.0f, 480.0f, 336.0f, 0.0f, 1.0f);
    GXSetScissor(0, 0, 480, 336);

    lbl_804D5150 = lbl_804D2568;
    lbl_804D5154 = lbl_804D256C;
    lbl_804D2568 = 480;
    lbl_804D256C = 336;

    if (entry->msgTarget == -1) {
        Vec3 pos;
        Vec3 n;

        GfxSetCullMode(1);
        fn_8004320C(&lbl_80495CE8.cam, &m180);

        pos.x = entry->actor->matrix[9];
        pos.y = entry->actor->matrix[10];
        pos.z = entry->actor->matrix[11];
        lbl_80495CE8.target = pos;
        n.x = entry->actor->matrix[0] * 0.000244140625f;
        n.y = entry->actor->matrix[1] * 0.000244140625f;
        n.z = entry->actor->matrix[2] * 0.000244140625f;
        PSMTXReflect(&refl, &pos, &n);
        PSMTXConcat(&refl, &m180, &m180);

        lbl_80495CE8.view = m180;
        fn_800A0168(&lbl_80495CE8.view, &lbl_80495CE8.view);
        fn_800C5E74(0);
        fn_800C6834(lbl_804D5148);
        fn_800C5E9C(0);

        if (entry->actor2 != 0) {
            Mtx34 m1;
            Mtx34 m2;
            Mtx34 basis;
            Vec3 d;
            Vec3 up;

            ActorGetMatrix(&m1, entry->actor);
            ActorGetMatrix(&m2, entry->actor2);

            d.x = lbl_80495CE8.cam.m[2][1] - m1.m[2][1];
            d.y = lbl_80495CE8.cam.m[2][2] - m1.m[2][2];
            d.z = lbl_80495CE8.cam.m[2][3] - m1.m[2][3];
            lbl_80449068 = m1;
            fn_800A0094(&d, &d);
            fn_800A0094(&up, (Vec3*)&lbl_80495CE8.cam.m[0][3]);
            fn_800A0100(&m2);
            fn_800A0094(&d, &d);
            fn_800A0094(&up, &up);
            fn_8009F7A8(&basis, &up);
            basis.m[2][1] = m2.m[2][1] + d.x;
            basis.m[2][2] = m2.m[2][2] + d.y;
            basis.m[2][3] = m2.m[2][3] + d.z;
            fn_8004320C(&basis, &m180);

            pos.x = entry->actor2->matrix[9];
            pos.y = entry->actor2->matrix[10];
            pos.z = entry->actor2->matrix[11];
            lbl_80495CE8.target = pos;
            n.x = entry->actor2->matrix[0] * 0.000244140625f;
            n.y = entry->actor2->matrix[1] * 0.000244140625f;
            n.z = entry->actor2->matrix[2] * 0.000244140625f;
            PSMTXReflect(&refl, &pos, &n);
            PSMTXConcat(&refl, &m180, &m180);

            lbl_80495CE8.view = m180;
            fn_800A0168(&lbl_80495CE8.view, &lbl_80495CE8.view);
            fn_800C5E9C(0);

            lbl_80495CE8.target.x = entry->actor2->matrix[9];
            lbl_80495CE8.target.y = entry->actor2->matrix[10];
            lbl_80495CE8.target.z = entry->actor2->matrix[11];
        }
    } else {
        Mtx34 m1;
        Mtx34 m2;
        NotifyMsg msg;
        Vec3 d;
        Vec3 up;
        Mtx34* cam;

        ActorGetMatrix(&m1, entry->actor);
        ActorGetMatrix(&m2, entry->actor2);
        fn_800C5E74(0);
        msg.unk4 = 1;
        SendMsg(entry->msgTarget, 0xC00, &msg);

        cam = (Mtx34*)fn_800C6448();
        d.x = cam->m[2][1] - m1.m[2][1];
        d.y = cam->m[2][2] - m1.m[2][2];
        d.z = cam->m[2][3] - m1.m[2][3];
        d.x *= 0.1f;
        d.y *= 0.1f;
        d.z *= 0.1f;
        lbl_80449068 = m2;
        fn_800A0094(&d, &d);
        fn_800A0094(&up, (Vec3*)&cam->m[0][3]);
        fn_800A0100(&m1);
        fn_800A0094(&d, &d);
        fn_800A0094(&up, &up);
        fn_8009F7A8(&lbl_80495CE8.view, &up);
        lbl_80495CE8.view.m[2][1] = m1.m[2][1] + d.x;
        lbl_80495CE8.view.m[2][2] = m1.m[2][2] + d.y;
        lbl_80495CE8.view.m[2][3] = m1.m[2][3] + d.z;

        msg.unk4 = 0;
        SendMsg(entry->msgTarget, 0xC00, &msg);
        fn_800C6834(lbl_804D5148);
        fn_800C5E9C(0);

        m180.m[0][0] = entry->actor->matrix[0] * 0.000244140625f * -1.0f;
        m180.m[0][1] = entry->actor->matrix[3] * 0.000244140625f * -1.0f;
        m180.m[0][2] = entry->actor->matrix[6] * 0.000244140625f * -1.0f;
        m180.m[0][3] = entry->actor->matrix[9];
        m180.m[1][0] = entry->actor->matrix[1] * 0.000244140625f;
        m180.m[1][1] = entry->actor->matrix[4] * 0.000244140625f;
        m180.m[1][2] = entry->actor->matrix[7] * 0.000244140625f;
        m180.m[1][3] = entry->actor->matrix[10];
        m180.m[2][0] = entry->actor->matrix[2] * 0.000244140625f;
        m180.m[2][1] = entry->actor->matrix[5] * 0.000244140625f;
        m180.m[2][2] = entry->actor->matrix[8] * 0.000244140625f;
        m180.m[2][3] = entry->actor->matrix[11];
        fn_800091D4(&m180, &m180, &tmp, 1);
        fn_80009848(-lbl_804D5158, &entry->texMtx, &m180);

        msg.unk4 = 1;
        SendMsg(entry->msgTarget, 0xC00, &msg);
        fn_800C5E9C(0);

        lbl_80495CE8.target.x = entry->actor2->matrix[9];
        lbl_80495CE8.target.y = entry->actor2->matrix[10];
        lbl_80495CE8.target.z = entry->actor2->matrix[11];
    }

    lbl_804D308C = index;
    return 1;
}

int ReflectUpdateCamera(void) {
    int changed;
    int i;
    char buf[0x70];

    if (lbl_804D514C != 0) {
        ReflectState* state = &lbl_80495CE8;
        Camera* cam;

        changed = 0;
        cam = (Camera*)fn_800C5DD8();
        state->view = cam->view;
        state->proj = cam->proj;
        state->proj.m[0][1] += lbl_804D5158;
        fn_800C5E9C(0);
        fn_800C6200(buf);
        state->cam = *(Mtx34*)fn_800C6448();

        for (i = 0; i < lbl_804D514C; i++) {
            ReflectEntry* entry = &state->entries[i];

            if (entry->lastValue != entry->actor->unk48) {
                entry->lastValue = entry->actor->unk48;
                changed = 1;
                entry->active = 1;
            } else {
                entry->active = 0;
            }
        }
        if (!changed) {
            return 0;
        }
    }
    return lbl_804D514C;
}
