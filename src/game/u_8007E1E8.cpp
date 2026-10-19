// Split-screen text helpers (0x8007E1E8-0x8007E8F8): scale a text size and place the debug text box
// for the current screen layout (1-6 views) and view index, project a world point to the screen,
// and send an object message 0xC00.
// Names are descriptive guesses; the code is matched against the retail DOL.

typedef unsigned char u8;

struct Vec3 {
    float x, y, z;
};

extern "C" {
// Callees in other files, not named yet.
int fn_800C5E6C(void);                       // current view index
int fn_802A1E90(void);                       // screen layout
void* fn_800C6448(void);                     // current camera
void fn_800A0030(Vec3* out, void* cam, Vec3* in); // into camera space
float fn_800097B4(void);                     // projection scale
}

extern "C" int SendMsg(int to, int msg, void* data);
void TextSetSize(int size);
void TextSetBox(int left, int right, int top, int bottom);
void TextSetMode(int mode);

#define MIN(a, b) ((a) < (b) ? (a) : (b))

inline int TextScaleSize(int size) {
    int view = fn_800C5E6C();

    switch (fn_802A1E90()) {
    case 1:
        size = 0.65f * size;
        break;
    case 4:
        if (view == 0) {
            size = 0.65f * size;
        } else if (view == 1) {
            size = 0.5f * size;
        } else if (view == 2) {
            size = 0.5f * size;
        }
        break;
    case 2:
    case 3:
    case 6:
        size = 0.5f * size;
        break;
    case 5:
        size = 0.65f * size;
        break;
    }
    return size;
}

int TextSizeScaled(int size) {
    size = TextScaleSize(size);
    TextSetSize(size);
    TextSetSize(size);
    return size;
}

int TextSizeScaledOnce(int size) {
    size = TextScaleSize(size);
    TextSetSize(size);
    return size;
}

void DebugTextPos(int x, int y, int mode) {
    int view = fn_800C5E6C();

    switch (fn_802A1E90()) {
    case 1:
        y /= 2;
        if (view == 0) {
            y += 240;
        }
        break;
    case 6:
        x /= 2;
        if (view == 1) {
            x += 320;
            y /= 2;
            y += 240;
        } else if (view == 2) {
            x += 320;
            y /= 2;
        }
        break;
    case 4:
        y /= 2;
        if (view == 0) {
            y += 240;
        } else if (view == 1) {
            x /= 2;
        } else if (view == 2) {
            x /= 2;
            x += 320;
        }
        break;
    case 2:
        y /= 2;
        if (view == 0) {
            y += 240;
            x /= 2;
            x += 160;
        } else if (view == 1) {
            x /= 2;
        } else if (view == 2) {
            x /= 2;
            x += 320;
        }
        break;
    case 3:
        x /= 2;
        y /= 2;
        if (view == 0) {
            y += 240;
        }
        if (view == 1) {
            y += 240;
            x += 320;
        } else if (view == 3) {
            x += 320;
        }
        break;
    case 5:
        x /= 2;
        if (view == 1) {
            x += 320;
        }
        break;
    }
    switch (mode) {
    case 0:
        TextSetBox(x, 640, 0, y);
        break;
    case 1:
        TextSetBox(0, x, 0, y);
        break;
    case 2:
        TextSetBox(x - MIN(x, 640 - x), x + MIN(x, 640 - x), 0, y);
        break;
    }
    TextSetMode(mode);
}

void WorldToScreen(Vec3* out, Vec3* in) {
    u8* cam = (u8*)fn_800C6448();
    Vec3 d;
    float s;

    d.x = in->x - *(float*)(cam + 0x24);
    d.y = in->y - *(float*)(cam + 0x28);
    d.z = in->z - *(float*)(cam + 0x2C);
    fn_800A0030(&d, cam, &d);
    s = fn_800097B4() / d.y;
    out->x = d.x * s + 320.0f;
    out->y = 480.0f - (-d.z * s + 240.0f);
    out->z = 0.0f;
}

struct ObjMsg {
    int unk0;
    int arg;
    int unk8;
    int unkC;
};

int ObjSendC00(void* obj, int arg) {
    ObjMsg m;

    m.arg = arg;
    if (obj) {
        return SendMsg(*(int*)((u8*)obj + 0x38), 0xC00, &m);
    }
    return 0;
}
