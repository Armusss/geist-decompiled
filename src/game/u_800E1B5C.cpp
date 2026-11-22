// u_800E1B5C.cpp (0x800E1B5C-0x800E2414): controller rumble (PADControlMotor per channel); the first of six files in the old u_800E1B5C range
// (0x800E1B5C-0x800E9418): this one, u_800E2414 (AUG effect), u_800E39F4 (Bullet effect), u_800E3EC4 (Pistol
// effect), u_800E48A0 (actor update/default message) and u_800E7E20. Data: .bss 0x80455780 (four channels),
// .sdata 0x804D2C28 (channel mask), the switch table 0x8039C3A8 (fn_800E1EA0).
// Boundary evidence for the end: own .sdata2 pool 0x804D7190-0x804D71A0 (0.0f 0x804D7190, double 0x804D7198);
// the AUG file has its own 0.0f at 0x804D71A0 (scan cut exactly at 0x800E2414, the AUG reset in the AUG
// descriptor).
// Names are descriptive guesses; the code is matched against the retail DOL.

#include <dolphin/types.h>

extern "C" void PADControlMotor(int chan, u32 command);

// One rumble channel per controller (4 x 0x24 bytes at .bss 0x80455780).
struct Rumble {
    int active;     // 0x00
    float length;   // 0x04
    float time;     // 0x08
    float phase;    // 0x0C
    int a;          // 0x10
    int b;          // 0x14
    int c;          // 0x18
    int d;          // 0x1C
    int enabled;    // 0x20
};

Rumble gRumble[4];
u32 gRumbleMask = 0xF;   // channels with rumble allowed

//@@800E1B5C
extern "C" void fn_800E1B5C(u32 mask);
void fn_800E1B5C(u32 mask) {
    int i;
    for (i = 0; i < 4; i++) {
        if ((gRumbleMask & (1 << i)) && !(mask & (1 << i))) {
            PADControlMotor(i, 0);
        }
    }
    gRumbleMask = mask;
}
//@@end
//@@800E1BD0
extern "C" u32 fn_800E1BD0(void);
u32 fn_800E1BD0(void) {
    return gRumbleMask;
}
//@@end
//@@800E1BD8
extern "C" int fn_800E1C04(int chan, int mode, int time, int a, int b, int c, int d);
extern "C" void fn_800E1BD8(int a, int b, int c, int d, int e);
void fn_800E1BD8(int a, int b, int c, int d, int e) {
    fn_800E1C04(a, b, c, d, d, e, e);
}
//@@end
//@@800E1C04
int fn_800E1C04(int chan, int mode, int time, int a, int b, int c, int d) {
    if (chan >= 4) {
        chan >>= 16;
    }
    if (chan < 0 || chan >= 4 || time < 0) {
        return 0;
    }
    if (gRumbleMask & (1 << chan)) {
        switch (mode) {
        case 0:
            gRumble[chan].active = 0;
            PADControlMotor(chan, 0);
            return 1;
        case 1:
            PADControlMotor(chan, 1);
            gRumble[chan].active = 2;
            gRumble[chan].time = time;
            return 1;
        case 2:
            PADControlMotor(chan, 1);
            gRumble[chan].active = 1;
            gRumble[chan].time = time;
            return 1;
        case 3:
            if (gRumble[chan].active != 3 && gRumble[chan].active != 4) {
                gRumble[chan].active = 3;
                gRumble[chan].phase = 0.0f;
            }
            gRumble[chan].a = a;
            gRumble[chan].b = b;
            gRumble[chan].c = c;
            gRumble[chan].d = d;
            gRumble[chan].length = time;
            gRumble[chan].time = time;
            return 1;
        case 4:
            if (gRumble[chan].active != 5 && gRumble[chan].active != 6) {
                gRumble[chan].active = 5;
                gRumble[chan].time = time;
                gRumble[chan].phase = 0.0f;
            }
            gRumble[chan].a = a;
            gRumble[chan].b = b;
            gRumble[chan].c = c;
            gRumble[chan].d = d;
            gRumble[chan].length = time;
            gRumble[chan].time = time;
            return 1;
        }
    }
    return 0;
}
//@@end
//@@800E1EA0
extern "C" void fn_800E1EA0(float dt);
void fn_800E1EA0(float dt) {
    int i;

    for (i = 0; i < 4; i++) {
        if (gRumble[i].enabled) {
            PADControlMotor(i, 0);
        } else {
            switch (gRumble[i].active) {
            case 0:
                break;
            case 1:
                gRumble[i].time -= dt;
            if (gRumble[i].time <= 0.0f) {
                PADControlMotor(i, 0);
                gRumble[i].active = 0;
                gRumble[i].time = 0.0f;
            }
                break;
            case 2:
                gRumble[i].time -= dt;
            if (gRumble[i].time <= 0.0f) {
                PADControlMotor(i, 2);
                gRumble[i].active = 0;
                gRumble[i].time = 0.0f;
            }
                break;
            case 4:
            gRumble[i].time -= dt;
            gRumble[i].phase -= dt;
            if (gRumble[i].time <= 0.0f) {
                PADControlMotor(i, 0);
                gRumble[i].active = 0;
                gRumble[i].time = 0.0f;
            } else if (gRumble[i].phase <= 0.0f) {
                PADControlMotor(i, 0);
                gRumble[i].active = 3;
                gRumble[i].phase = (float)gRumble[i].c * (gRumble[i].time / gRumble[i].length) + (float)gRumble[i].d * ((gRumble[i].length - gRumble[i].time) / gRumble[i].length);
            }
            break;
            case 3:
            gRumble[i].time -= dt;
            gRumble[i].phase -= dt;
            if (gRumble[i].time <= 0.0f) {
                PADControlMotor(i, 0);
                gRumble[i].active = 0;
                gRumble[i].time = 0.0f;
            } else if (gRumble[i].phase <= 0.0f) {
                PADControlMotor(i, 1);
                gRumble[i].active = 4;
                gRumble[i].phase = (float)gRumble[i].a * (gRumble[i].time / gRumble[i].length) + (float)gRumble[i].b * ((gRumble[i].length - gRumble[i].time) / gRumble[i].length);
            }
            break;
            case 6:
            gRumble[i].time -= dt;
            gRumble[i].phase -= dt;
            if (gRumble[i].time <= 0.0f) {
                PADControlMotor(i, 2);
                gRumble[i].active = 0;
                gRumble[i].time = 0.0f;
            } else if (gRumble[i].phase <= 0.0f) {
                PADControlMotor(i, 2);
                gRumble[i].active = 5;
                gRumble[i].phase = (float)gRumble[i].c * (gRumble[i].time / gRumble[i].length) + (float)gRumble[i].d * ((gRumble[i].length - gRumble[i].time) / gRumble[i].length);
            }
            break;
            case 5:
            gRumble[i].time -= dt;
            gRumble[i].phase -= dt;
            if (gRumble[i].time <= 0.0f) {
                PADControlMotor(i, 0);
                gRumble[i].active = 0;
                gRumble[i].time = 0.0f;
            } else if (gRumble[i].phase <= 0.0f) {
                PADControlMotor(i, 1);
                gRumble[i].active = 6;
                gRumble[i].phase = (float)gRumble[i].a * (gRumble[i].time / gRumble[i].length) + (float)gRumble[i].b * ((gRumble[i].length - gRumble[i].time) / gRumble[i].length);
            }
            break;
            default:
                PADControlMotor(i, 0);
                gRumble[i].active = 0;
                break;
            }
        }
    }
}
//@@end
//@@800E2300
extern "C" int fn_800E2300(int on);
int fn_800E2300(int on) {
    int i;
    for (i = 0; i < 4; i++) {
        gRumble[i].enabled = on;
        PADControlMotor(i, 0);
    }
    return 1;
}
//@@end
//@@800E2368
extern "C" void fn_800E2368(void);
void fn_800E2368(void) {
    int i;
    for (i = 0; i < 4; i++) {
        gRumble[i].active = 0;
        PADControlMotor(i, 0);
    }
}
//@@end
//@@800E23CC
extern "C" int fn_800E23CC(void);
int fn_800E23CC(void) {
    int i;
    for (i = 0; i < 4; i++) {
        gRumble[i].active = 0;
        gRumble[i].time = 0.0f;
        gRumble[i].enabled = 0;
    }
    return 1;
}
//@@end
