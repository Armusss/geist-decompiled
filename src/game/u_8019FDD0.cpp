// Reflection rendering helpers (0x8019FDD0-0x801A0F2C): accumulate mirror time, handle the
// reflection actor's message, set up the reflection texture matrix, finish/restore the reflection
// pass, render one reflection entry (with and without a second actor) and update the camera state.
// Names are descriptive guesses; the code is matched against the retail DOL.

#include "game/types.h"

typedef unsigned char u8;
typedef unsigned long u32;

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

// Copy of the current view's camera object (fn_800C5DD8), 0x60 bytes.
struct Camera {
    PlaceF view;           // 0x00
    int unk30;             // 0x30
    float fov;             // 0x34
    float f38;             // 0x38
    float f3C;             // 0x3C
    float f40;             // 0x40
    float f44;             // 0x44
    float f48;             // 0x48
    int unk4C;             // 0x4C
    int unk50;             // 0x50
    int unk54;             // 0x54
    int unk58;             // 0x58
    int unk5C;             // 0x5C
};

struct NotifyMsg {
    int unk0;
    int unk4;
    int unk8;
    int unkC;
};

Camera lbl_80495CE8;
PlaceF lbl_80495D48;
Vec3 lbl_80495D78;
ReflectEntry lbl_80495D84[8];

extern "C" {
int lbl_804D5148;
int lbl_804D514C;
int lbl_804D5150;
int lbl_804D5154;
float lbl_804D5158;
int lbl_804D515C;
int lbl_804D5160;
float lbl_804D3088 = 0.005f;
int lbl_804D308C = -1;
extern int lbl_804D4084;
extern int lbl_804D2568;
extern int lbl_804D256C;
extern PlaceF lbl_80449068;

int SendMsg(int to, int msg, void* data);

void fn_80009170(int* pos, int* end, int r, int g, int b);
void fn_800091D4(Mtx34* a, Mtx34* b, Mtx33* out, int count);
void fn_80009848(float fov, Mtx34* texMtx, Mtx34* mtx);
void fn_80009E44(int a, int b);
void fn_8004320C(PlaceF* src, Mtx34* dst);
void fn_8009F7A8(PlaceF* mtx, Vec3* up);
void fn_800A0094(Vec3* out, Vec3* in);
void fn_800A0100(PlaceF* mtx);
void fn_800A0168(PlaceF* out, PlaceF* in);
void* fn_8000F990(int id);
Camera* fn_800C5DD8(void);
int fn_800C5E74(int a);
void fn_800C5E9C(int a);
void fn_800C6200(void* buf);
PlaceF* fn_800C6448(void);
void fn_800C6664(int a);
void fn_800C6834(int a);
void fn_8009863C(int a, float f, float g);
void fn_8010F7EC(void);

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
void GfxSetCopyFilter(int mode);
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
        if (id == lbl_80495D84[i].id && lbl_80495D84[i].active) {
            GXLoadTexMtxImm(&lbl_80495D84[i].texMtx, 0x1E, 0);
            GXSetNumTexGens(1);
            GXSetTexCoordGen2(0, 0, 0, 0x1E, 0, 0x7D);
            fn_8010F7EC();
            if (lbl_804D515C) {
                int pos[3];
                int end[3];

                pos[0] = lbl_80495D84[i].actor->matrix[9];
                pos[1] = lbl_80495D84[i].actor->matrix[10];
                pos[2] = lbl_80495D84[i].actor->matrix[11];
                end[0] = pos[0] + lbl_80495D84[i].actor->matrix[0] / 64;
                end[1] = pos[1] + lbl_80495D84[i].actor->matrix[1] / 64;
                end[2] = pos[2] + lbl_80495D84[i].actor->matrix[2] / 64;
                fn_80009170(pos, end, 255, 0, 0);

                end[0] = lbl_80495D84[i].actor->matrix[9] + lbl_80495D84[i].actor->matrix[3] / 64;
                end[1] = lbl_80495D84[i].actor->matrix[10] + lbl_80495D84[i].actor->matrix[4] / 64;
                end[2] = lbl_80495D84[i].actor->matrix[11] + lbl_80495D84[i].actor->matrix[5] / 64;
                fn_80009170(pos, end, 255, 255, 0);

                end[0] = lbl_80495D84[i].actor->matrix[9] + lbl_80495D84[i].actor->matrix[6] / 64;
                end[1] = lbl_80495D84[i].actor->matrix[10] + lbl_80495D84[i].actor->matrix[7] / 64;
                end[2] = lbl_80495D84[i].actor->matrix[11] + lbl_80495D84[i].actor->matrix[8] / 64;
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

    switch (lbl_80495D84[lbl_804D308C].actor->flag35) {
    case 1:
        if (lbl_804D3088) {
            fn_8009863C(0, (float)(lbl_804D5160 * 3), lbl_804D3088);
        }
        break;
    }

    tex = fn_8000F990(lbl_80495D84[lbl_804D308C].id);
    if (tex) {
        GfxSetCopyFilter(2);
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

    if (lbl_80495D84[lbl_804D308C].msgTarget != -1) {
        msg.unk4 = 0;
        SendMsg(lbl_80495D84[lbl_804D308C].msgTarget, 0xC00, &msg);
    }

    fn_800C6664(lbl_804D5148);
    fn_800C5E9C(0);
    lbl_804D308C = -1;
}

int ReflectRender(int index) {
    Mtx33 tmp;
    Mtx34 m180;
    Mtx34 refl;
    void* tex;

    tex = fn_8000F990(lbl_80495D84[index].id);
    if (lbl_80495D84[index].active && tex != 0) {

    lbl_804D4084 = 1;
    GXSetViewport(0.0f, 0.0f, 480.0f, 336.0f, 0.0f, 1.0f);
    GXSetScissor(0, 0, 480, 336);

    lbl_804D5150 = lbl_804D2568;
    lbl_804D5154 = lbl_804D256C;
    lbl_804D2568 = 480;
    lbl_804D256C = 336;

    if (lbl_80495D84[index].msgTarget == -1) {
        Vec3 pos;
        Vec3 n;
        Vec3 d;
        Vec3 up;
        PlaceF m1;
        PlaceF m2;
        PlaceF basis;

        GfxSetCullMode(1);
        fn_8004320C(&lbl_80495D48, &m180);

        pos.x = lbl_80495D84[index].actor->matrix[9];
        pos.y = lbl_80495D84[index].actor->matrix[10];
        pos.z = lbl_80495D84[index].actor->matrix[11];
        lbl_80495D78.x = pos.x;
        lbl_80495D78.y = pos.y;
        lbl_80495D78.z = pos.z;
        n.x = lbl_80495D84[index].actor->matrix[3];
        n.y = lbl_80495D84[index].actor->matrix[4];
        n.z = lbl_80495D84[index].actor->matrix[5];
        n.x *= 0.000244140625f;
        n.y *= 0.000244140625f;
        n.z *= 0.000244140625f;
        PSMTXReflect(&refl, &pos, &n);
        PSMTXConcat(&refl, &m180, &m180);

        lbl_80495CE8.view.rot.m[0][0] = m180.m[0][0];
        lbl_80495CE8.view.rot.m[0][1] = m180.m[0][1];
        lbl_80495CE8.view.rot.m[0][2] = m180.m[0][2];
        lbl_80495CE8.view.rot.m[1][0] = m180.m[1][0];
        lbl_80495CE8.view.rot.m[1][1] = m180.m[1][1];
        lbl_80495CE8.view.rot.m[1][2] = m180.m[1][2];
        lbl_80495CE8.view.rot.m[2][0] = m180.m[2][0];
        lbl_80495CE8.view.rot.m[2][1] = m180.m[2][1];
        lbl_80495CE8.view.rot.m[2][2] = m180.m[2][2];
        lbl_80495CE8.view.pos.x = m180.m[0][3];
        lbl_80495CE8.view.pos.y = m180.m[1][3];
        lbl_80495CE8.view.pos.z = m180.m[2][3];
        fn_800A0168(&lbl_80495CE8.view, &lbl_80495CE8.view);
        fn_800C5E74(0);
        fn_800C6834(lbl_804D5148);
        fn_800C5E9C(0);

        {
            Actor* a = lbl_80495D84[index].actor;
            float r00, r01, r02, r10, r11, r12, r20, r21, r22;
            float px, py, pz;
            r00 = a->matrix[0] * 0.000244140625f;
            r01 = a->matrix[3] * 0.000244140625f;
            r02 = a->matrix[6] * 0.000244140625f;
            r10 = a->matrix[1] * 0.000244140625f;
            r11 = a->matrix[4] * 0.000244140625f;
            r12 = a->matrix[7] * 0.000244140625f;
            r20 = a->matrix[2] * 0.000244140625f;
            r21 = a->matrix[5] * 0.000244140625f;
            r22 = a->matrix[8] * 0.000244140625f;
            px = a->matrix[9];
            py = a->matrix[10];
            pz = a->matrix[11];
            m180.m[0][0] = r00;
            m180.m[0][1] = r01;
            m180.m[0][2] = r02;
            m180.m[0][3] = px;
            m180.m[1][0] = r10;
            m180.m[1][1] = r11;
            m180.m[1][2] = r12;
            m180.m[1][3] = py;
            m180.m[2][0] = r20;
            m180.m[2][1] = r21;
            m180.m[2][2] = r22;
            m180.m[2][3] = pz;
        }
        fn_800091D4(&m180, &m180, &tmp, 1);
        fn_80009848(-lbl_804D5158, &lbl_80495D84[index].texMtx, &m180);

        if (lbl_80495D84[index].actor2 != 0) {
            ActorGetMatrix((Mtx34*)&m1, lbl_80495D84[index].actor);
            ActorGetMatrix((Mtx34*)&m2, lbl_80495D84[index].actor2);

            d.x = lbl_80495D48.pos.x - m1.pos.x;
            d.y = lbl_80495D48.pos.y - m1.pos.y;
            d.z = lbl_80495D48.pos.z - m1.pos.z;
            lbl_80449068 = m1;
            fn_800A0094(&d, &d);
            fn_800A0094(&up, (Vec3*)&lbl_80495D48.rot.m[1][0]);
            fn_800A0100(&m2);
            fn_800A0094(&d, &d);
            fn_800A0094(&up, &up);
            fn_8009F7A8(&basis, &up);
            basis.pos.x = m2.pos.x + d.x;
            basis.pos.y = m2.pos.y + d.y;
            basis.pos.z = m2.pos.z + d.z;
            fn_8004320C(&basis, &m180);

            pos.x = lbl_80495D84[index].actor2->matrix[9];
            pos.y = lbl_80495D84[index].actor2->matrix[10];
            pos.z = lbl_80495D84[index].actor2->matrix[11];
            lbl_80495D78.x = pos.x;
            lbl_80495D78.y = pos.y;
            lbl_80495D78.z = pos.z;
            n.x = lbl_80495D84[index].actor2->matrix[3];
            n.y = lbl_80495D84[index].actor2->matrix[4];
            n.z = lbl_80495D84[index].actor2->matrix[5];
            n.x *= 0.000244140625f;
            n.y *= 0.000244140625f;
            n.z *= 0.000244140625f;
            PSMTXReflect(&refl, &pos, &n);
            PSMTXConcat(&refl, &m180, &m180);

            lbl_80495CE8.view.rot.m[0][0] = m180.m[0][0];
            lbl_80495CE8.view.rot.m[0][1] = m180.m[0][1];
            lbl_80495CE8.view.rot.m[0][2] = m180.m[0][2];
            lbl_80495CE8.view.rot.m[1][0] = m180.m[1][0];
            lbl_80495CE8.view.rot.m[1][1] = m180.m[1][1];
            lbl_80495CE8.view.rot.m[1][2] = m180.m[1][2];
            lbl_80495CE8.view.rot.m[2][0] = m180.m[2][0];
            lbl_80495CE8.view.rot.m[2][1] = m180.m[2][1];
            lbl_80495CE8.view.rot.m[2][2] = m180.m[2][2];
            lbl_80495CE8.view.pos.x = m180.m[0][3];
            lbl_80495CE8.view.pos.y = m180.m[1][3];
            lbl_80495CE8.view.pos.z = m180.m[2][3];
            fn_800A0168(&lbl_80495CE8.view, &lbl_80495CE8.view);
            fn_800C5E9C(0);

            lbl_80495D78.x = lbl_80495D84[index].actor2->matrix[9];
            lbl_80495D78.y = lbl_80495D84[index].actor2->matrix[10];
            lbl_80495D78.z = lbl_80495D84[index].actor2->matrix[11];
        }
    } else {
        PlaceF m1;
        PlaceF m2;
        NotifyMsg msg;
        Vec3 d;
        Vec3 up;
        PlaceF* cam;

        ActorGetMatrix((Mtx34*)&m1, lbl_80495D84[index].actor);
        ActorGetMatrix((Mtx34*)&m2, lbl_80495D84[index].actor2);
        fn_800C5E74(0);
        msg.unk4 = 1;
        SendMsg(lbl_80495D84[index].msgTarget, 0xC00, &msg);

        cam = fn_800C6448();
        d.x = cam->pos.x - m1.pos.x;
        d.y = cam->pos.y - m1.pos.y;
        d.z = cam->pos.z - m1.pos.z;
        d.x *= 0.1f;
        d.y *= 0.1f;
        d.z *= 0.1f;
        lbl_80449068 = m2;
        fn_800A0094(&d, &d);
        fn_800A0094(&up, (Vec3*)&cam->rot.m[1][0]);
        fn_800A0100(&m1);
        fn_800A0094(&d, &d);
        fn_800A0094(&up, &up);
        fn_8009F7A8(&lbl_80495CE8.view, &up);
        lbl_80495CE8.view.pos.x = m1.pos.x + d.x;
        lbl_80495CE8.view.pos.y = m1.pos.y + d.y;
        lbl_80495CE8.view.pos.z = m1.pos.z + d.z;

        msg.unk4 = 0;
        SendMsg(lbl_80495D84[index].msgTarget, 0xC00, &msg);
        fn_800C6834(lbl_804D5148);
        fn_800C5E9C(0);

        {
            Actor* a = lbl_80495D84[index].actor;
            float r00, r01, r02, r10, r11, r12, r20, r21, r22;
            float px, py, pz;
            r00 = a->matrix[0] * 0.000244140625f;
            r01 = a->matrix[3] * 0.000244140625f;
            r02 = a->matrix[6] * 0.000244140625f;
            r10 = a->matrix[1] * 0.000244140625f;
            r11 = a->matrix[4] * 0.000244140625f;
            r12 = a->matrix[7] * 0.000244140625f;
            r20 = a->matrix[2] * 0.000244140625f;
            r21 = a->matrix[5] * 0.000244140625f;
            r22 = a->matrix[8] * 0.000244140625f;
            px = a->matrix[9];
            py = a->matrix[10];
            pz = a->matrix[11];
            m180.m[0][0] = r00;
            m180.m[0][1] = r01;
            m180.m[0][2] = r02;
            m180.m[0][3] = px;
            m180.m[1][0] = r10;
            m180.m[1][1] = r11;
            m180.m[1][2] = r12;
            m180.m[1][3] = py;
            m180.m[2][0] = r20;
            m180.m[2][1] = r21;
            m180.m[2][2] = r22;
            m180.m[2][3] = pz;
            m180.m[0][0] *= -1.0f;
            m180.m[0][1] *= -1.0f;
            m180.m[0][2] *= -1.0f;
        }
        fn_800091D4(&m180, &m180, &tmp, 1);
        fn_80009848(-lbl_804D5158, &lbl_80495D84[index].texMtx, &m180);

        msg.unk4 = 1;
        SendMsg(lbl_80495D84[index].msgTarget, 0xC00, &msg);
        fn_800C5E9C(0);

        lbl_80495D78.x = lbl_80495D84[index].actor2->matrix[9];
        lbl_80495D78.y = lbl_80495D84[index].actor2->matrix[10];
        lbl_80495D78.z = lbl_80495D84[index].actor2->matrix[11];
    }

    lbl_804D308C = index;
    return 1;
    }
    lbl_804D308C = -1;
    return 0;
}

int ReflectUpdateCamera(void) {
    int changed;
    int i;
    char buf[0x70];

    if (lbl_804D514C != 0) {
        changed = 0;
        lbl_80495CE8 = *fn_800C5DD8();
        lbl_80495CE8.fov += lbl_804D5158;
        fn_800C5E9C(0);
        fn_800C6200(buf);
        lbl_80495D48 = *fn_800C6448();

        for (i = 0; i < lbl_804D514C; i++) {
            if (lbl_80495D84[i].lastValue != lbl_80495D84[i].actor->unk48) {
                lbl_80495D84[i].lastValue = lbl_80495D84[i].actor->unk48;
                changed = 1;
                lbl_80495D84[i].active = 1;
            } else {
                lbl_80495D84[i].active = 0;
            }
        }
        if (!changed) {
            return 0;
        }
    }
    return lbl_804D514C;
}
