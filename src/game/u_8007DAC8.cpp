// Timers (0x8007DAC8-0x8007E1E8): a pool of 192 callback timers kept on a free list and a used list
// (doubly linked by 16-bit indices). Each frame the used timers count down (optionally scaled by a
// time factor) and fire their callback; a callback returns the next interval, or 0 to free the timer.
// Timer messages (0x3400-0x3406) create, adjust, free, fire and query timers for script objects.
// Names are descriptive guesses; the code is matched against the retail DOL.

extern "C" {
// Callees in other files, not named yet.
float fn_802A2080(void);  // time scale
void fn_80026660(int type, void* func, int arg);
}

extern "C" int SendMsg(int to, int msg, void* data);

typedef float (*TimerFunc)(void* user, int arg);

struct Timer {
    short next;
    short prev;
    float time;
    int scaled;
    TimerFunc func;
    void* user;
    int arg;
};

struct TimerMsg {
    int unk0;
    int a;
    int b;
    float c;
    int d;
    int e;
};

struct TimerReply {
    int unk0;
    int arg;
    float next;
};

static Timer sTimers[192];
static short sTimerFree;
static short sTimerUsed;
static float sTimerReply;

inline int TimerAlloc(void) {
    int i = sTimerFree;

    if (i == -1) {
        i = -1;
    } else {
        sTimerFree = sTimers[i].next;
        if (sTimerFree != -1) {
            sTimers[sTimerFree].prev = -1;
        }
        if (sTimerUsed != -1) {
            sTimers[sTimerUsed].prev = i;
        }
        sTimers[i].next = sTimerUsed;
        sTimerUsed = i;
    }
    return i;
}

inline void TimerRelease(int i) {
    sTimers[i].func = 0;
    if (sTimers[i].next != -1) {
        sTimers[sTimers[i].next].prev = sTimers[i].prev;
    }
    if (i != sTimerUsed) {
        sTimers[sTimers[i].prev].next = sTimers[i].next;
    } else {
        sTimerUsed = sTimers[i].next;
    }
    sTimers[i].next = sTimerFree;
    sTimers[i].prev = -1;
    if (sTimerFree != -1) {
        sTimers[sTimerFree].prev = i;
    }
    sTimerFree = i;
}

inline void TimerRestart(float* time, float next) {
    if (*time < 0.0f && *time > -next) {
        *time += next;
    } else {
        *time = next;
    }
}

inline void TimerRestartAt(int i, float r) {
    if (sTimers[i].time < 0.0f && sTimers[i].time > -r) {
        sTimers[i].time += r;
    } else {
        sTimers[i].time = r;
    }
}

int TimerUpdate(float dt) {
    TimerFunc* func;
    Timer* t;
    int i;
    int next;
    float* time;

    for (i = sTimerUsed; i != -1; i = next) {
        t = &sTimers[i];
        next = t->next;
        if (t->scaled) {
            t->time -= dt * fn_802A2080();
        } else {
            t->time -= dt;
        }
        time = &t->time;
        if (*time <= 0.0f) {
            float r;
            func = &t->func;
            r = (*func)(t->user, t->arg);
            if (r == 0.0f) {
                *func = 0;
                if (t->next != -1) {
                    sTimers[t->next].prev = t->prev;
                }
                if (i != sTimerUsed) {
                    sTimers[t->prev].next = t->next;
                } else {
                    sTimerUsed = t->next;
                }
                t->next = sTimerFree;
                t->prev = -1;
                if (sTimerFree != -1) {
                    sTimers[sTimerFree].prev = i;
                }
                sTimerFree = i;
            } else {
                TimerRestart(time, r);
            }
        }
    }
    return 1;
}

int TimerAdd(int scaled, float (*func)(void*), void* user, float delay) {
    int i = TimerAlloc();

    if (i != -1) {
        Timer* t = &sTimers[i];
        t->func = (TimerFunc)func;
        t->user = user;
        t->arg = 0;
        t->time = delay;
        t->scaled = scaled;
    }
    return i;
}

void TimerNop(void) {}

int TimerMsgHandler(int id, int msg, TimerMsg* data);

inline void TimerReset(void) {
    int i;

    for (i = 0; i < 192; i++) {
        sTimers[i].next = i + 1;
        sTimers[i].prev = i - 1;
        sTimers[i].func = 0;
    }
    sTimerFree = 0;
    sTimers[i - 1].next = -1;
    sTimerUsed = -1;
}

int TimerInit(void) {
    TimerReset();
    fn_80026660(0x29, (void*)TimerMsgHandler, 0);
    return 1;
}

float TimerMsgCallback(void* user, int arg);

inline float TimerFire(int i) {
    float left = 0.0f;
    if (sTimers[i].func) {
        float r;
        left = sTimers[i].time;
        r = sTimers[i].func(sTimers[i].user, sTimers[i].arg);
        if (r == 0.0f) {
            TimerRelease(i);
        } else {
            TimerRestart(&sTimers[i].time, r);
        }
    }
    return left;
}

inline int TimerCreate(float delay, TimerFunc func, int scaled, void* user, int arg) {
    int i = TimerAlloc();

    if (i != -1) {
        sTimers[i].func = func;
        sTimers[i].user = user;
        sTimers[i].arg = arg;
        sTimers[i].time = delay;
        sTimers[i].scaled = scaled;
    }
    return i;
}

int TimerMsgHandler(int id, int msg, TimerMsg* data) {
    switch (msg) {
    case 0x3400:
        data->d = TimerCreate(data->c, TimerMsgCallback, data->e, (void*)data->a, data->b);
        break;
    case 0x3401:
        if (0.0f != *(float*)&data->b) {
            TimerRestartAt(data->a, *(float*)&data->b);
        } else {
            sTimers[data->a].time += data->c;
        }
        break;
    case 0x3402:
        TimerRelease(data->a);
        break;
    case 0x3403:
        *(float*)&data->b = TimerFire(data->a);
        break;
    case 0x3404:
        *(float*)&data->b = sTimers[data->a].time;
        break;
    case 0x3406:
        sTimerReply = *(float*)&data->a;
        break;
    default:
        return 0;
    }
    return 1;
}

float TimerMsgCallback(void* user, int arg) {
    TimerReply m;

    m.unk0 = 0;
    m.arg = arg;
    m.next = 0.0f;
    sTimerReply = 0.0f;
    SendMsg((int)user, 0x3405, &m);
    if (m.next == 0.0f && sTimerReply != 0.0f) {
        return sTimerReply;
    }
    return m.next;
}
