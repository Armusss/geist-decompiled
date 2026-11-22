// u_800F73C8.cpp (0x800F73C8-0x800F77F8): the DVD music stream callbacks (finished, shutdown, trigger, address done, stop, prepared, error).
// Split from the old u_800F73C8 (0x800F73C8-0x800FDDE4). Names are descriptive guesses; matched against retail
// unless noted.
// Boundary note (2026-11-22, not applied: it moves u_800F396C's end): these functions share the stream state in
// .sbss 0x804D4B58-0x804D4B88 and .bss 0x80457E50-0x80457F70 with fn_800F6CFC..fn_800F7350 at the end of
// u_800F396C, so this range is the tail of that stream file; the effect code after it starts its own data.


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
    u8 pad00[4];
    StreamNode* next;       // 0x04
    u8 pad08[4];
    u32 events;             // 0x0C: which events invoke the callback
    void (*callback)(u32 event); // 0x10: gets the event bit
    u8 file[1];             // 0x14: DVDFileInfo
};

extern "C" {
void AISetStreamPlayState(int state);
void AISetStreamTrigger(int t);
void AISetStreamVolLeft(int v);
void AISetStreamVolRight(int v);
void AIResetStreamSampleCount(void);
int DVDClose(void* file);
int DVDGetStreamPlayAddrAsync(DVDCommandBlock* b, void* cb);
int DVDGetStreamErrorStatusAsync(DVDCommandBlock* b, void* cb);
int DVDPrepareStreamAsync(void* file, int a, int b, void* cb);
int DVDStopStreamAtEndAsync(DVDCommandBlock* b, void* cb);
}

extern StreamNode* lbl_804D4B58;  // current entry
extern StreamNode* lbl_804D4B5C;  // list head
extern int lbl_804D4B60;  // list tail
extern volatile int lbl_804D4B64;  // stream state
extern volatile int lbl_804D4B68;  // play mode
extern volatile int lbl_804D4B6C;  // stream position
extern int lbl_804D4B70;  // trigger
extern u8 lbl_804D4B74;  // volume left
extern u8 lbl_804D4B75;  // volume right
extern int lbl_804D4B78;  // stream word
extern volatile u32 lbl_804D4B7C;  // end reached
extern void (*lbl_804D4B80)(void);    // 0x804D4B80
extern DVDCommandBlock lbl_80457E80;  // stop command block
extern DVDCommandBlock lbl_80457E50;  // play command block
extern DVDCommandBlock lbl_80457EE0;  // address command block
extern DVDCommandBlock lbl_80457EB0;  // error command block

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

// Stream finished: notify (bit 2) and clear the state.
void StreamFinished(void) {
    StreamNotify(2);
    lbl_804D4B64 = 0;
}

// Closes every stream entry and resets the list.
void MusicStreamShutdown(void) {
    StreamNode* n;
    AISetStreamPlayState(0);
    for (n = lbl_804D4B5C; n != NULL; n = n->next) {
        DVDClose(n->file);
    }
    lbl_804D4B5C = NULL;
    lbl_804D4B60 = 0;
    lbl_804D4B58 = NULL;
    lbl_804D4B64 = 0;
    if (lbl_804D4B80 != NULL) {
        lbl_804D4B80();
        lbl_804D4B80 = NULL;
    }
    lbl_804D4B64 = 0;
    lbl_804D4B78 = 0;
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
