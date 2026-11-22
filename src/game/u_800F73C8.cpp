// u_800F73C8.cpp (0x800F6CFC-0x800F77F8): the DVD music stream player: volume/command/list API (fn_800F6CFC..
// fn_800F7350, called by u_800DFE3C and Cube) and the stream callbacks (finished, shutdown, trigger, address done,
// stop, prepared, error). Split from the old u_800F73C8 (0x800F73C8-0x800FDDE4). Names are descriptive guesses;
// matched against retail.
// Boundary evidence (2026-11-22): fn_800F6CFC..fn_800F7350 (formerly the end of u_800F396C) use only the stream
// state in .sbss 0x804D4B58-0x804D4B88 and .bss 0x80457E50-0x80457F70 that the callbacks use, and the callbacks
// are their DVD/AI callbacks; the weather code before 0x800F6CFC uses .sbss 0x804D4B00-0x804D4B54 and .bss
// 0x80457BD0-0x80457C4C instead. The effect code after 0x800F77F8 starts its own data.


#include "game/types.h"
#include "game/string.h"
#include "game/msg.h"

typedef unsigned long u32;
typedef unsigned short u16;
typedef unsigned char u8;

#define NULL 0

struct StreamNode;
struct EffectKind;
struct EffectInst;
struct Emitter;
struct Vector;
struct CamObj;
struct Shake;
struct GameType;
struct Minigame;

// Functions called from other units under their placeholder names.
extern "C" float fn_800F7920(u32 row, u32 key);
extern "C" int fn_800F7994(u32 row, u32 key);
extern "C" void fn_800F7A90(u32 row);
extern "C" int fn_800F7AF0(u32 emitter, u32 row);
extern "C" int fn_800F7C64(u32 emitter, int row);
extern "C" int fn_800F7D28(u32 emitter, u32 row);
extern "C" int fn_800F80DC(u32 handle);
extern "C" int fn_800F8130(u32 handle, int i);
extern "C" int fn_800F81EC(u32 handle);
extern "C" int fn_800F825C(u32 handle, u32 row, int n);
extern "C" void fn_800F86A0(u32 handle);
extern "C" u32 fn_800F8A30(int user);
extern "C" u32 fn_800F8AF4(u32 row);
extern "C" void fn_800F8ED0(void);
extern "C" void fn_800F9468(int on);
extern "C" int fn_800F94C8(u32 handle);
extern "C" void fn_800F94F8(u32 handle, short v);
extern "C" void fn_800F9578(u32 handle, int v);
extern "C" float fn_800F95E4(u32 handle);
extern "C" int fn_800F9614(u32 handle);
extern "C" void fn_800F96E0(u32 handle);
extern "C" u32 fn_800F9724(int owner, int id);
extern "C" u32 fn_800FC3B8(int id);
extern "C" int fn_800FC764(u32 who, int arg);
extern "C" char* fn_800FD334(int i);
extern "C" int fn_800FD3DC(void);
extern "C" int fn_800FD3EC(void);
extern "C" char* fn_800FD3FC(int i);
extern "C" char* fn_800FD424(int i);
extern "C" const char* fn_800FD44C(int c);
extern "C" const char* fn_800FD5C4(int c);
extern "C" int fn_800FD6EC(int i);
extern "C" void fn_800FD924(int arg);
extern "C" void fn_800FD998(void);
extern "C" void fn_800FDA10(void);
extern "C" void fn_800FDA90(void);
extern "C" void fn_800FDC48(void);
extern "C" void fn_800FDC78(void);

extern "C" int SendMsg(int to, int msg, void* data);

static inline void SetVarInt(int target, u32 hash, int value) {
    QueryMsg m;
    m.hash = hash;
    *(int*)&m.result = value;
    m.unk8 = 0;
    SendMsg(target, 0x41D, &m);
}

static inline int GetVarInt(int target, u32 hash) {
    QueryMsg m;
    *(int*)&m.result = 0;
    m.hash = hash;
    m.unk8 = 0;
    SendMsg(target, 0x41E, &m);
    return *(int*)&m.result;
}

extern unsigned char __upper_map[256];
inline int toupper(int c) { return (c == -1) ? -1 : (int)__upper_map[(unsigned char)c]; }

// ---- stream list -------------------------------------------------------------------------

struct DVDCommandBlock {
    u8 data[0x30];
};

struct StreamNode {
    StreamNode* prev;       // 0x00
    StreamNode* next;       // 0x04
    const char* path;       // 0x08
    u32 events;             // 0x0C: which events invoke the callback
    void (*callback)(u32 event); // 0x10: gets the event bit
    u8 file[1];             // 0x14: DVDFileInfo
};

extern "C" {
void AISetStreamPlayState(int state);
void AISetStreamTrigger(int t);
void AISetStreamVolLeft(u8 v);
void AISetStreamVolRight(u8 v);
void AIResetStreamSampleCount(void);
void* AIRegisterStreamCallback(void (*cb)(int));
int DVDOpen(const char* path, void* file);
int DVDCancelStreamAsync(DVDCommandBlock* b, void* cb);
int OSDisableInterrupts(void);
int OSRestoreInterrupts(int level);
int DVDClose(void* file);
int DVDGetStreamPlayAddrAsync(DVDCommandBlock* b, void* cb);
int DVDGetStreamErrorStatusAsync(DVDCommandBlock* b, void* cb);
int DVDPrepareStreamAsync(void* file, int a, int b, void* cb);
int DVDStopStreamAtEndAsync(DVDCommandBlock* b, void* cb);
}

extern StreamNode* lbl_804D4B58;  // current entry
extern StreamNode* lbl_804D4B5C;  // list head
extern StreamNode* lbl_804D4B60;  // list tail
extern volatile u32 lbl_804D4B64;  // stream state (0 idle, 1 playing, 2 paused, 3 busy)
extern volatile u32 lbl_804D4B68;  // play mode (0 once, 1 loop list, 2 repeat entry)
extern volatile int lbl_804D4B6C;  // stream position
extern volatile int lbl_804D4B70;  // trigger
extern volatile u8 lbl_804D4B74;  // volume left
extern volatile u8 lbl_804D4B75;  // volume right
extern volatile u32 lbl_804D4B78;  // set while the stream system shuts down
extern volatile u32 lbl_804D4B7C;  // end reached
extern void (*lbl_804D4B80)(void);    // 0x804D4B80
extern DVDCommandBlock lbl_80457E80;  // stop command block
extern DVDCommandBlock lbl_80457E50;  // play command block
extern DVDCommandBlock lbl_80457EE0;  // address command block
extern DVDCommandBlock lbl_80457EB0;  // error command block
extern DVDCommandBlock lbl_80457F10;  // shutdown cancel command block
extern DVDCommandBlock lbl_80457F40;  // stop cancel command block

void StreamFinished(void);
void MusicStreamShutdown(void);
void StreamSetTrigger(int offset);
void StreamStop(void);
void StreamPrepared(void);
void StreamErrorDone(unsigned char status);
void StreamAddrDone(u32 pos);

// Calls the current entry's callback when it asked for event `bit`.
static inline void StreamNotify(u32 bit) {
    if (lbl_804D4B58 != NULL) {
        u32 ev;
        if (lbl_804D4B58->callback != NULL && (ev = lbl_804D4B58->events & bit) != 0) {
            lbl_804D4B58->callback(ev);
        }
    }
}

// Calls an entry's callback when it asked for event `bit`.
static inline void StreamNodeNotify(StreamNode* n, u32 bit) {
    u32 ev;
    if (n != NULL && n->callback != NULL && (ev = n->events & bit) != 0) {
        n->callback(ev);
    }
}

// Closes every stream entry, resets the list and runs the pending shutdown callback.
static inline void StreamCloseAll(void) {
    StreamNode* n;
    AISetStreamPlayState(0);
    for (n = lbl_804D4B5C; n != NULL; n = n->next) {
        DVDClose(n->file);
    }
    lbl_804D4B5C = NULL;
    lbl_804D4B60 = NULL;
    lbl_804D4B58 = NULL;
    lbl_804D4B64 = 0;
    if (lbl_804D4B80 != NULL) {
        lbl_804D4B80();
        lbl_804D4B80 = NULL;
    }
    lbl_804D4B64 = 0;
    lbl_804D4B78 = 0;
}

// Sets the stream volumes (applied at once while playing).
extern "C" void fn_800F6CFC(u8 left, u8 right) {
    lbl_804D4B74 = left;
    lbl_804D4B75 = right;
    if (lbl_804D4B64 == 1) {
        AISetStreamVolLeft(left);
        AISetStreamVolRight(right);
    }
}

// The current entry.
extern "C" StreamNode* fn_800F6D44(void) { return lbl_804D4B58; }

// The stream state.
extern "C" u32 fn_800F6D4C(void) { return lbl_804D4B64; }

// Stream command: 0 stop, 1 play/resume, 2 pause, 4 prepare and stop.
extern "C" void fn_800F6D54(int cmd) {
    if (lbl_804D4B64 == cmd || lbl_804D4B64 == 3) {
        return;
    }
    switch (cmd) {
    case 0:
        if (lbl_804D4B58 != NULL) {
            lbl_804D4B64 = 3;
            AISetStreamVolLeft(0);
            AISetStreamVolRight(0);
            AISetStreamPlayState(0);
            DVDCancelStreamAsync(&lbl_80457F40, (void*)StreamFinished);
        }
        break;
    case 1:
        if (lbl_804D4B64 == 2) {
            AISetStreamVolLeft(lbl_804D4B74);
            AISetStreamVolRight(lbl_804D4B75);
            AIResetStreamSampleCount();
            AISetStreamTrigger(lbl_804D4B70);
            AISetStreamPlayState(1);
            lbl_804D4B64 = 1;
            if (lbl_804D4B58 != NULL) {
                StreamNodeNotify(lbl_804D4B58, 1);
            }
        } else if (lbl_804D4B58 != NULL) {
            lbl_804D4B64 = 3;
            DVDPrepareStreamAsync(lbl_804D4B58->file, 0, 0, (void*)StreamPrepared);
        } else {
            lbl_804D4B64 = 1;
        }
        lbl_804D4B7C = 0;
        break;
    case 4:
        if (lbl_804D4B64 == 0) {
            if (lbl_804D4B58 != NULL) {
                lbl_804D4B64 = 3;
                DVDPrepareStreamAsync(lbl_804D4B58->file, 0, 0, (void*)StreamStop);
            }
            lbl_804D4B7C = 0;
        }
        break;
    case 2:
        AISetStreamPlayState(0);
        if (lbl_804D4B64 == 1) {
            lbl_804D4B64 = 2;
        }
        StreamNotify(4);
        break;
    }
}

// Sets the play mode.
extern "C" void fn_800F6F34(u32 mode) { lbl_804D4B68 = mode; }

// Shuts the stream down (cancelling a playing stream first) and then calls `done`.
extern "C" void fn_800F6F3C(void (*done)(void)) {
    if (lbl_804D4B64 != 3) {
        u32 state = lbl_804D4B64;
        lbl_804D4B80 = done;
        lbl_804D4B64 = 3;
        if (state == 1) {
            DVDCancelStreamAsync(&lbl_80457F10, (void*)MusicStreamShutdown);
        } else {
            StreamCloseAll();
        }
    }
}

// Removes an entry from the list (2: it is the current one).
extern "C" int fn_800F6FFC(StreamNode* n) {
    int level;

    if (n == lbl_804D4B58) {
        return 2;
    }
    level = OSDisableInterrupts();
    DVDClose(n->file);
    if (n == lbl_804D4B5C && n == lbl_804D4B60) {
        lbl_804D4B5C = NULL;
        lbl_804D4B60 = NULL;
        OSRestoreInterrupts(level);
        return 0;
    }
    if (n == lbl_804D4B5C) {
        lbl_804D4B5C = n->next;
        lbl_804D4B5C->prev = NULL;
        if (lbl_804D4B68 == 1) {
            lbl_804D4B60->next = lbl_804D4B5C;
        }
        OSRestoreInterrupts(level);
        return 0;
    }
    if (n == lbl_804D4B60) {
        lbl_804D4B60 = n->prev;
        lbl_804D4B60->next = NULL;
        if (lbl_804D4B68 == 1) {
            lbl_804D4B60->next = lbl_804D4B5C;
        }
        OSRestoreInterrupts(level);
        return 0;
    }
    n->prev->next = n->next;
    n->next->prev = n->prev;
    OSRestoreInterrupts(level);
    return 0;
}

// Opens `path` into entry `n` and appends it to the list (1: the file does not open).
extern "C" int fn_800F7130(const char* path, StreamNode* n, u32 events, void (*callback)(u32)) {
    u32 start = 0;
    int level;

    if (!DVDOpen(path, n->file)) {
        return 1;
    }
    level = OSDisableInterrupts();
    n->path = path;
    n->events = events;
    n->callback = callback;
    if (lbl_804D4B5C == NULL) {
        lbl_804D4B5C = n;
        lbl_804D4B60 = n;
        n->prev = NULL;
        n->next = NULL;
        if (lbl_804D4B64 == 1) {
            start = 1;
        }
    } else {
        lbl_804D4B60->next = n;
        n->prev = lbl_804D4B60;
        lbl_804D4B60 = n;
        n->next = NULL;
    }
    if (lbl_804D4B58 == NULL) {
        lbl_804D4B58 = n;
    }
    OSRestoreInterrupts(level);
    StreamNodeNotify(n, 8);
    if (start) {
        lbl_804D4B64 = 3;
        DVDPrepareStreamAsync(lbl_804D4B58->file, 0, 0, (void*)StreamPrepared);
    }
    return 0;
}

// Stops the stream system and waits until the shutdown has finished.
extern "C" void fn_800F724C(void) {
    AISetStreamVolLeft(0);
    AISetStreamVolRight(0);
    AIRegisterStreamCallback(NULL);
    AIResetStreamSampleCount();
    AISetStreamPlayState(0);
    lbl_804D4B78 = 1;
    if (lbl_804D4B64 != 3) {
        u32 state = lbl_804D4B64;
        lbl_804D4B64 = 3;
        lbl_804D4B80 = NULL;
        if (state == 1) {
            DVDCancelStreamAsync(&lbl_80457F10, (void*)MusicStreamShutdown);
        } else {
            StreamCloseAll();
        }
    }
    lbl_804D4B64 = 0;
    while (lbl_804D4B78 != 0) {
    }
}

// Initialises the stream player.
extern "C" void fn_800F7350(void) {
    lbl_804D4B64 = 0;
    lbl_804D4B68 = 0;
    lbl_804D4B6C = 0;
    lbl_804D4B70 = 48000;
    lbl_804D4B74 = 0xFF;
    lbl_804D4B58 = NULL;
    lbl_804D4B5C = NULL;
    lbl_804D4B60 = NULL;
    lbl_804D4B75 = 0xFF;
    AISetStreamVolLeft(0);
    AISetStreamVolRight(0);
    AIRegisterStreamCallback(StreamSetTrigger);
    AIResetStreamSampleCount();
    AISetStreamPlayState(0);
}

// Stream finished: notify (bit 2) and clear the state.
void StreamFinished(void) {
    StreamNotify(2);
    lbl_804D4B64 = 0;
}

// Closes every stream entry and resets the list.
void MusicStreamShutdown(void) {
    StreamCloseAll();
}

void StreamSetTrigger(int offset) {
    AISetStreamTrigger(offset + lbl_804D4B70);
    if (lbl_804D4B58 != NULL) {
        DVDGetStreamPlayAddrAsync(&lbl_80457EE0, (void*)StreamAddrDone);
    }
}

// Advances to the next entry (or stops/loops by mode) when the current one ends.
// Restarts the DVD stream on the current entry.
static inline void StreamRestart(void) {
    AISetStreamVolLeft(0);
    AISetStreamVolRight(0);
    AISetStreamPlayState(0);
    DVDPrepareStreamAsync(lbl_804D4B58->file, 0, 0, (void*)StreamPrepared);
}

void StreamAddrDone(u32 pos) {
    lbl_804D4B6C = pos;
    if (lbl_804D4B7C != 0) {
        lbl_804D4B7C = 0;
        StreamNotify(0x10);
        lbl_804D4B64 = 3;
        switch (lbl_804D4B68) {
        case 0:
            if (lbl_804D4B58 != NULL) {
                if (lbl_804D4B58->next != NULL) {
                    lbl_804D4B58 = lbl_804D4B58->next;
                    StreamRestart();
                } else {
                    lbl_804D4B58 = lbl_804D4B5C;
                    AISetStreamVolLeft(0);
                    AISetStreamVolRight(0);
                    AISetStreamPlayState(0);
                    lbl_804D4B64 = 0;
                }
            }
            break;
        case 1:
            if (lbl_804D4B58 != NULL) {
                if (lbl_804D4B58->next != NULL) {
                    lbl_804D4B58 = lbl_804D4B58->next;
                    StreamRestart();
                } else {
                    lbl_804D4B58 = lbl_804D4B5C;
                    StreamRestart();
                }
            }
            break;
        case 2:
            if (lbl_804D4B58 != NULL) {
                StreamRestart();
            }
            break;
        }
    } else {
        DVDGetStreamErrorStatusAsync(&lbl_80457EB0, (void*)StreamErrorDone);
    }
}

// Stream stopped: silence it and notify (bit 0x20).
void StreamStop(void) {
    AISetStreamVolLeft(0);
    AISetStreamVolRight(0);
    AISetStreamPlayState(0);
    DVDStopStreamAtEndAsync(&lbl_80457E80, NULL);
    lbl_804D4B64 = 2;
    StreamNotify(0x20);
}

// Stream prepared: start playback and notify (bit 1).
void StreamPrepared(void) {
    AISetStreamVolLeft(lbl_804D4B74);
    AISetStreamVolRight(lbl_804D4B75);
    AIResetStreamSampleCount();
    AISetStreamTrigger(lbl_804D4B70);
    AISetStreamPlayState(1);
    DVDStopStreamAtEndAsync(&lbl_80457E50, NULL);
    lbl_804D4B64 = 1;
    StreamNotify(1);
}

// DVD error status callback.
void StreamErrorDone(unsigned char status) {
    if (status != 0) {
        return;
    }
    lbl_804D4B7C = 1;
    lbl_804D4B6C = 0;
}
