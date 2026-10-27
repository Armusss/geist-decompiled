// u_8003B98C.cpp: stream load manager: handle table, file streaming and channel release (0x8003B98C-0x8003CC90).
// Names are descriptive guesses; the code is matched against the retail DOL.
// Function and data names are descriptive guesses based on what the code does.

typedef signed char s8;
typedef unsigned char u8;
typedef short s16;
typedef unsigned short u16;
typedef int s32;
typedef unsigned long u32;

// Handle encoding: bits 0-11 index, bits 12-14 channel, bit 15 sign.
// Entries live in lbl_8040DF00 (stride 0x114); lbl_804D4324 is the
// per-index "used" bitmap.
struct HandleEntry {
    u8 channel;         // low 3 bits compared against the handle's channel
    u8 pad0[3];
    s32 value;
    u8 pad1[0x114 - 8];
};

extern HandleEntry lbl_8040DF00[];
extern u32 lbl_804D4324[1];

// Returns the value stored for a valid channel handle, otherwise 0.
// fn_8003B98C
extern "C" s32 fn_8003B98C(s16 handle)
{
    s32 index;

    if (handle == -1)
        return 0;
    index = handle & 0xFFF;
    if (index >= 1)
        return 0;
    if ((lbl_804D4324[index / 32] & (1 << (handle & 0x1F))) == 0)
        return 0;
    if (((handle >> 12) & 7) != (lbl_8040DF00[index].channel & 7))
        return 0;
    return lbl_8040DF00[index].value;
}

// Marks the entry behind a valid channel handle as used (value 1).
// fn_8003BA00
extern "C" void fn_8003BA00(s16 handle)
{
    s32 index;

    if (handle == -1)
        return;
    index = handle & 0xFFF;
    if (index >= 1)
        return;
    if ((lbl_804D4324[index / 32] & (1 << (handle & 0x1F))) == 0)
        return;
    if (((handle >> 12) & 7) == (lbl_8040DF00[index].channel & 7))
        lbl_8040DF00[index].value = 1;
}

// fn_8003BA70 (placeholder name kept: called from other units as fn_8003BA70).
// Checks that a handle's index is live in the bitmap and that the handle's
// channel bits agree with the channel stored in the table entry.
extern "C" void fn_8003BA70(s16 handle)
{
    s32 index;

    if (handle == -1)
        return;
    index = handle & 0xFFF;
    if (index >= 1)
        return;
    if ((lbl_804D4324[index / 32] & (1 << (handle & 0x1F))) == 0)
        return;
    if (((handle >> 12) & 7) != (lbl_8040DF00[index].channel & 7))
        return;
}

// ---- Streamed load queue -------------------------------------------------
// A fixed table of load slots is walked every frame; each slot pulls blocks
// from an async file and pushes them into a queue.

struct AsyncFile;

// Async read from a file into a buffer; returns 1 when a block was read,
// 0 when nothing was available (async mode) and anything else at end of file.
int AsyncRead(AsyncFile* file, void* buffer, int size, int count, int blocking);
// Non-zero when the queue cannot accept more data.
int QueueFull(u32 queue);
// Appends a block of data to a queue; returns 1 when it was accepted.
int QueuePush(unsigned long queue, const void* data, int count, unsigned long flags);
// FreeHandleChannel (fn_8003CA3C): releases the handle channel of the load
// queue.
int FreeHandleChannel(s16 handle);

struct LoadEntry {
    s32 done;                       // +0x00
    s32 progress;                   // +0x04
    s32 total;                      // +0x08
    s32 unknown0c;                  // +0x0c
    u32 queue;                      // +0x10
    s32 remaining;                  // +0x14
    void (*doneCallback)(s32, s32); // +0x18
    AsyncFile* file;                // +0x1c
    s32 unknown20;                  // +0x20
};

struct LoadState {
    u8 channel;      // +0x00
    s8 async;        // +0x01
    s16 pending;     // +0x02
    u8 unknown04[8];
    s32 completed;   // +0x0c
    s32 processed;   // +0x10
    u8 unknown14[8];
    s32 limit;       // +0x1c
    s32 count;       // +0x20
    u8 unknown24[8];
    LoadEntry entries[1]; // +0x2c
};

extern double lbl_804D61F0;
extern double lbl_804D61F8;
extern double lbl_804D6200;

// fn_8003BAD0 (placeholder name kept: called from other units as fn_8003BAD0).
// Pumps the loading slots; the argument is the frame time.
extern "C" void fn_8003BAD0(float dt)
{
    LoadState* state = (LoadState*)lbl_8040DF00;
    u8 buffer[0x800];
    s32 half;
    double fastRate;
    double slowRate;
    s32 i;

    half = (s32)dt / 2;
    fastRate = lbl_804D61F0 * ((double)half - lbl_804D6200);
    slowRate = lbl_804D61F8 * ((double)half - lbl_804D6200);

    if ((lbl_804D4324[0] & 1) == 0)
        return;
    if (state->async != 0 && state->pending == 0)
        return;
    if (state->processed >= state->limit)
        return;

    for (i = 0; i < state->count; i++) {
        LoadEntry* entry = &state->entries[i];
        s32 budget;
        s32 read;

        if (state->async != 0) {
            if (entry->remaining == 0)
                continue;
            budget = half;
        } else if (i == 0) {
            budget = (s32)fastRate;
        } else {
            s32 n = (s32)slowRate;
            budget = n > 1 ? n : 1;
        }

        read = 1;
        while (entry->done == 0 && budget != 0 && read != 0) {
            if (QueueFull(entry->queue))
                break;
            read = AsyncRead(entry->file, buffer, 0x800, 1, state->async == 0);
            if (state->async != 0 && read == 0)
                continue;
            if (read != 1) {
                // End of file: tell every slot and drop the channel.
                // (The original reuses the outer slot counter here.)
                for (i = 0; i < state->count; i++) {
                    if (state->entries[i].doneCallback != 0)
                        state->entries[i].doneCallback(0, 1);
                }
                FreeHandleChannel((s16)((state->channel & 7) << 12));
                break;
            }
            QueuePush(entry->queue, buffer, 1, 0);
            entry->progress++;
            state->completed++;
            if (entry->progress >= entry->total) {
                entry->done = 1;
                if (entry->doneCallback != 0)
                    entry->doneCallback(0, 1);
            }
            budget--;
            state->processed++;
            if (state->async != 0 && state->pending > 0) {
                state->pending--;
                entry->remaining--;
            }
        }
    }
}

// ---- Load-manager reset --------------------------------------------------
// Reset/init view of the load manager at lbl_8040DF00: a small header followed
// by the per-slot state.  The handle fields hold queue channel handles and are
// -1 while the slot is free.  (Layout is a guess from the retail offsets.)
struct LoadSlotView {
    s32 handle;   // +0x00
    s32 count;    // +0x04
    s32 total;    // +0x08
    s32 state;    // +0x0c
    s32 arg;      // +0x10
    s32 handle2;  // +0x14
    s32 queue;    // +0x18
    s32 field1c;  // +0x1c
    s32 field20;  // +0x20
};

struct LoadManagerView {
    u8 channel;            // +0x00
    s8 async;              // +0x01
    s16 pending;           // +0x02
    s32 field04;           // +0x04
    s32 field08;           // +0x08
    s32 completed;         // +0x0c
    s32 processed;         // +0x10
    s32 field14;           // +0x14
    s32 field18;           // +0x18
    s32 limit;             // +0x1c
    s32 count;             // +0x20
    s32 field24;           // +0x24
    LoadSlotView slots[7]; // +0x28
};

extern u8 lbl_803F9A00[];
extern s32 lbl_804D4328;
extern "C" void* memset(void* dest, int value, unsigned long count);

// True when the handle's index bit is set in the global handle bitmap.
static inline int IsHandleSlotUsed(s16 handle)
{
    return (lbl_804D4324[handle / 32] & (1 << (handle & 0x1F))) != 0;
}

// fn_8003BDA4: resets the streamed-load manager, releasing the queue's
// channel handle first when it is still live, then advances the channel.
void ResetLoadManager(void)
{
    LoadManagerView* mgr = (LoadManagerView*)lbl_8040DF00;

    if (IsHandleSlotUsed(0))
        FreeHandleChannel((s16)((mgr->channel & 7) << 12));

    mgr->processed = 0;
    mgr->completed = 0;
    mgr->async = 0;
    mgr->pending = 0;
    mgr->field04 = 0;
    mgr->field14 = 0;
    mgr->field18 = 0;
    mgr->limit = 0;
    mgr->count = 0;
    mgr->field24 = 0;

    mgr->slots[0].handle = -1;
    mgr->slots[0].count = 0;
    mgr->slots[0].total = 0;
    mgr->slots[0].state = 0;
    mgr->slots[0].handle2 = -1;
    mgr->slots[0].queue = 0;

    mgr->slots[1].handle = -1;
    mgr->slots[1].count = 0;
    mgr->slots[1].total = 0;
    mgr->slots[1].state = 0;
    mgr->slots[1].handle2 = -1;
    mgr->slots[1].queue = 0;

    mgr->slots[2].handle = -1;
    mgr->slots[2].count = 0;
    mgr->slots[2].total = 0;
    mgr->slots[2].state = 0;
    mgr->slots[2].handle2 = -1;
    mgr->slots[2].queue = 0;

    mgr->slots[3].handle = 0;
    mgr->slots[3].count = 0;
    mgr->slots[3].total = 0;
    mgr->slots[3].state = 0;
    mgr->slots[3].arg = 0;
    mgr->slots[3].handle2 = -1;
    mgr->slots[3].queue = 0;
    mgr->slots[3].field1c = 0;
    mgr->slots[3].field20 = 0;

    mgr->slots[4].count = -1;
    mgr->slots[4].total = 0;
    mgr->slots[4].handle2 = -1;
    mgr->slots[4].queue = 0;
    mgr->slots[4].field1c = 0;
    mgr->slots[4].field20 = 0;

    mgr->slots[5].count = -1;
    mgr->slots[5].total = 0;
    mgr->slots[5].handle2 = -1;
    mgr->slots[5].queue = 0;
    mgr->slots[5].field1c = 0;
    mgr->slots[5].field20 = 0;

    mgr->slots[6].count = -1;
    mgr->slots[6].total = 0;

    mgr->channel = (mgr->channel + 1) & 7;
}

// fn_8003BEF0: initialises the load manager: clears the queue buffer block and
// the manager state, and forgets the global handle bookkeeping.
int InitLoadManager(void)
{
    LoadManagerView* mgr = (LoadManagerView*)lbl_8040DF00;

    memset(lbl_803F9A00, 0, 0x40C0);

    mgr->processed = 0;
    mgr->completed = 0;
    mgr->async = 0;
    mgr->pending = 0;
    mgr->field04 = 0;
    mgr->field14 = 0;
    mgr->field18 = 0;
    mgr->limit = 0;
    mgr->count = 0;
    mgr->field24 = 0;

    mgr->slots[0].handle = -1;
    mgr->slots[0].count = 0;
    mgr->slots[0].total = 0;
    mgr->slots[0].state = 0;
    mgr->slots[0].handle2 = -1;
    mgr->slots[0].queue = 0;

    mgr->slots[1].handle = -1;
    mgr->slots[1].count = 0;
    mgr->slots[1].total = 0;
    mgr->slots[1].state = 0;
    mgr->slots[1].handle2 = -1;
    mgr->slots[1].queue = 0;

    mgr->slots[2].handle = -1;
    mgr->slots[2].count = 0;
    mgr->slots[2].total = 0;
    mgr->slots[2].state = 0;
    mgr->slots[2].handle2 = -1;
    mgr->slots[2].queue = 0;

    mgr->slots[3].handle = 0;
    mgr->slots[3].count = 0;
    mgr->slots[3].total = 0;
    mgr->slots[3].state = 0;
    mgr->slots[3].arg = 0;
    mgr->slots[3].handle2 = -1;
    mgr->slots[3].queue = 0;
    mgr->slots[3].field1c = 0;
    mgr->slots[3].field20 = 0;

    mgr->slots[4].count = -1;
    mgr->slots[4].total = 0;
    mgr->slots[4].handle2 = -1;
    mgr->slots[4].queue = 0;
    mgr->slots[4].field1c = 0;
    mgr->slots[4].field20 = 0;

    mgr->slots[5].count = -1;
    mgr->slots[5].total = 0;
    mgr->slots[5].handle2 = -1;
    mgr->slots[5].queue = 0;
    mgr->slots[5].field1c = 0;
    mgr->slots[5].field20 = 0;

    mgr->slots[6].count = -1;
    mgr->slots[6].total = 0;

    lbl_804D4328 = 0;
    lbl_804D4324[0] = 0;

    return 1;
}

// ---- Streamed file loading ----------------------------------------------
// Two fixed stream slots live at lbl_803F9A00; each owns a 0x2000-byte read
// window and streams fixed 0x800-byte packets into a queue.

// The parts of the SDK DVD file-info block this unit touches; the block is
// 0x3c bytes and the file length sits at +0x34.
struct DVDFileInfo {
    u8 commandBlock[0x30];
    u32 start;       // +0x30
    u32 length;      // +0x34
    void* callback;  // +0x38
};

struct StreamSlot {
    u8 buffer[0x2000]; // +0x0000
    s32 bufStart;      // +0x2000
    s32 bufEnd;        // +0x2004
    s32 pos;           // +0x2008
    DVDFileInfo file;  // +0x200c
    s32 limit;         // +0x2048
    s32 inUse;         // +0x204c
};

// A queued block: a small header followed by the payload.
struct StreamPacket {
    u32 id;          // +0x0000
    u8 data[0x7FC];  // +0x0004
};

extern "C" void* memcpy(void* dest, const void* src, unsigned long count);
void DebugPrintf(const char* format, ...);
extern "C" int DVDOpen(const char* name, void* fileInfo);
extern "C" int DVDClose(void* fileInfo);
int FileReadSync(void* file, void* buffer, int length, int offset);

// Copies `size` bytes from the slot's current position into dest, sliding the
// 0x2000-byte window forward when the position falls outside it.  Returns 0
// when the window could not be refilled.
static inline int StreamRead(StreamSlot* slot, void* dest, s32 size)
{
    if (slot->pos < slot->bufStart || slot->pos + size >= slot->bufEnd) {
        slot->bufStart = slot->pos & 0xFFFFFFE0;
        slot->bufEnd = slot->bufStart + 0x2000;
        if (slot->bufEnd > slot->limit)
            slot->bufEnd = (slot->limit + 0x1F) & 0xFFFFFFE0;
        if (FileReadSync(&slot->file, slot, slot->bufEnd - slot->bufStart,
                         slot->bufStart) == 0) {
            slot->bufStart = 0;
            slot->bufEnd = 0;
            return 0;
        }
    }
    memcpy(dest, slot->buffer + (slot->pos - slot->bufStart), size);
    slot->pos += size;
    return 1;
}

// fn_8003C008: opens `name`, skips `block` x 0x800 bytes into it and pushes
// `count` packets onto the queue; returns 0, or a negative error on failure.
// fn_8003C008
extern "C" int fn_8003C008(int queue, int id, const char* name, int block, int count)
{
    StreamSlot* slots = (StreamSlot*)lbl_803F9A00;
    StreamSlot* slot;
    StreamPacket packet;
    s32 i;

    DebugPrintf("Opening Stream: %s", name);

    slot = 0;
    for (i = 0; i < 2; i++) {
        if (slots[i].inUse == 0 && DVDOpen(name, &slots[i].file) != 0) {
            slot = &slots[i];
            slot->inUse = 1;
            slot->bufStart = 0;
            slot->bufEnd = 0;
            slot->pos = 0;
            slot->limit = slot->file.length;
            break;
        }
    }
    if (slot == 0)
        return -1;

    slot->pos = block << 11;
    for (i = 0; i < count; i++) {
        if (StreamRead(slot, &packet.id, 4) == 0) {
            slot->inUse = 0;
            DVDClose(&slot->file);
            return -3;
        }
        if (packet.id != (u32)id) {
            slot->inUse = 0;
            DVDClose(&slot->file);
            return -4;
        }
        if (StreamRead(slot, packet.data, 0x7FC) == 0) {
            slot->inUse = 0;
            DVDClose(&slot->file);
            return -5;
        }
        if (QueuePush(queue, &packet, 1, 0) != 1) {
            slot->inUse = 0;
            DVDClose(&slot->file);
            return -6;
        }
    }
    slot->inUse = 0;
    DVDClose(&slot->file);
    return 0;
}

// fn_8003C308: when `handle` names a live channel of the stream queue,
// releases that channel.
// fn_8003C308
extern "C" void fn_8003C308(s16 handle)
{
    s32 index;

    if (handle == -1)
        return;
    index = handle & 0xFFF;
    if (index >= 1)
        return;
    if ((lbl_804D4324[index / 32] & (1 << (handle & 0x1F))) == 0)
        return;
    if (((handle >> 12) & 7) == (lbl_8040DF00[index].channel & 7))
        FreeHandleChannel(handle);
}

// ---- Async stream slot setup ---------------------------------------------
// The load manager keeps, per slot, a table of links.  fn_8003C38C fills those
// links for a live channel: each link copies an id out of the slot's source
// table and looks its value up in a caller-supplied name table.
// Names and layout are descriptive guesses.

struct StreamSource {          // 0x24 bytes, slot+0x28
    s32 id;                    // +0x00
    u8 pad04[0x20];
};

struct StreamLinkEntry {       // 0x24 bytes, slot+0xa8
    s32 id;                    // +0x00
    u8 pad04[8];
    s32 value;                 // +0x0c
    u8 pad10[0x14];
};

struct LinkNameEntry {         // 0x0c bytes
    s8 name;                   // +0x0
    u8 pad01;
    u16 value;                 // +0x2
    u8 pad04[8];
};

struct AsyncSlotView {
    u8 channel;                // +0x00
    u8 pad01[0x1f];            // +0x01
    s32 count;                 // +0x20
    u8 pad24[4];               // +0x24
    StreamSource sources[3];   // +0x28
    s32 field94;               // +0x94
    s32 argA;                  // +0x98
    s32 argB;                  // +0x9c
    s32 linkCount;             // +0xa0
    s32 fieldA4;               // +0xa4
    StreamLinkEntry links[3];  // +0xa8
};

// fn_8003C38C (placeholder name kept: called from other units as
// fn_8003C38C; descriptive name: LoadSlotSetup).
// Sets up the link table of the load slot named by `handle`.  The second
// parameter is not used by the retail code.
extern "C" int fn_8003C38C(s16 handle, int unused, int argA, int count,
                           const LinkNameEntry* names, int argB)
{
    AsyncSlotView* slot;
    s32 index;
    s32 i;
    s32 j;

    if (handle == -1)
        return -1;
    index = handle & 0xFFF;
    if (index >= 1)
        return -1;
    if ((lbl_804D4324[index / 32] & (1 << (handle & 0x1F))) == 0)
        return -1;
    if (((handle >> 12) & 7) != (lbl_8040DF00[index].channel & 7))
        return -1;

    slot = (AsyncSlotView*)&lbl_8040DF00[index];
    slot->argA = argA;
    slot->argB = argB;
    slot->linkCount = slot->count;
    for (i = 0; i < slot->linkCount; i++) {
        slot->links[i].id = slot->sources[i].id;
        slot->links[i].value = 0;
        for (j = 0; j < count; j++) {
            if (slot->links[i].id == names[j].name) {
                slot->links[i].value = names[j].value;
                break;
            }
        }
    }
    return 0;
}

// ---- Async stream opening ------------------------------------------------
// Opens the file streams described by an array of 0x0c-byte descriptors for a
// free load slot, then publishes the slot's channel handle.  Names, struct
// layout and callback types are descriptive guesses.

typedef void (*AsyncCallback)(int, int);

struct AsyncSource {            // 0x24 bytes at slot+0x28 + i*0x24
    s32 id;                     // +0x00
    s32 field04;                // +0x04
    s32 field08;                // +0x08
    s32 field0c;                // +0x0c
    s32 block;                  // +0x10
    s32 stream;                 // +0x14
    s32 mode;                   // +0x18
    AsyncCallback callback;     // +0x1c
    AsyncFile* file;            // +0x20
};

struct StreamDesc {             // 0x0c bytes
    s8 type;                    // +0x0  'R' or 'A'
    u8 pad01;
    u16 field02;                // +0x2
    u8 pad04[6];
    u16 block;                  // +0xa
};

extern u8 lbl_803FDAC0[];
extern u32 lbl_804D2680[];
extern s32 lbl_804D4320;

int BitsetFirstClear(u32* bits, int start, int limit);
int BitsetFirstSet(u32* bits, int start, int limit);
void BitClear(u32* bits, int index);
void QueueClear(unsigned long queue);
void Fn80080F90(void* owner, int arg);
int Fn80080F88(void);
void SAMStreamEvent(int arg0, int arg1);
int SAMGetStream(void);
int AsyncOpen(AsyncFile* file, const char* name, int isR);
void AsyncSeek(AsyncFile* file, int offset, int origin);
void AsyncClose(AsyncFile* file);

// fn_8003C48C (placeholder name kept: called from other units as
// fn_8003C48C; descriptive name: OpenStreamAsync).
// Opens the streams described by `descs` for the first free load slot.
// `blockBase` is added to each descriptor's block number when seeking.
extern "C" int fn_8003C48C(const char* name, s32 blockBase, int count,
                           const StreamDesc* descs, s32 arg2, u32 flags,
                           s16 handle)
{
    LoadManagerView* mgr;
    AsyncSource* sources;
    s32 index;
    s32 i;
    int useSam;
    int stream;

    useSam = 0;

    mgr = (LoadManagerView*)lbl_8040DF00;
    if ((flags & 0x10) != 0 && (lbl_804D4324[0] & 1) != 0 &&
        handle == mgr->field08 && mgr->async != 0) {
        mgr->async = 0;
        return (s16)mgr->field08;
    }

    if (lbl_804D4328 != 0)
        return -1;

    index = BitsetFirstClear(lbl_804D4324, 1, 1);
    if (index == -1)
        return -1;

    mgr = (LoadManagerView*)&lbl_8040DF00[index];
    sources = (AsyncSource*)mgr->slots;

    mgr->processed = 0;
    mgr->completed = 0;
    mgr->pending = 0;
    mgr->field04 = 0;
    mgr->field14 = 0;
    mgr->field18 = 0;
    mgr->limit = 0;
    mgr->count = 0;
    mgr->field24 = 0;

    mgr->slots[0].handle = -1;
    mgr->slots[0].count = 0;
    mgr->slots[0].total = 0;
    mgr->slots[0].state = 0;
    mgr->slots[0].handle2 = -1;
    mgr->slots[0].queue = 0;

    mgr->slots[1].handle = -1;
    mgr->slots[1].count = 0;
    mgr->slots[1].total = 0;
    mgr->slots[1].state = 0;
    mgr->slots[1].handle2 = -1;
    mgr->slots[1].queue = 0;

    mgr->slots[2].handle = -1;
    mgr->slots[2].count = 0;
    mgr->slots[2].total = 0;
    mgr->slots[2].state = 0;
    mgr->slots[2].handle2 = -1;
    mgr->slots[2].queue = 0;

    mgr->slots[3].handle = 0;
    mgr->slots[3].count = 0;
    mgr->slots[3].total = 0;
    mgr->slots[3].state = 0;
    mgr->slots[3].arg = 0;
    mgr->slots[3].handle2 = -1;
    mgr->slots[3].queue = 0;
    mgr->slots[3].field1c = 0;
    mgr->slots[3].field20 = 0;

    mgr->slots[4].count = -1;
    mgr->slots[4].total = 0;
    mgr->slots[4].handle2 = -1;
    mgr->slots[4].queue = 0;
    mgr->slots[4].field1c = 0;
    mgr->slots[4].field20 = 0;

    mgr->slots[5].count = -1;
    mgr->slots[5].total = 0;
    mgr->slots[5].handle2 = -1;
    mgr->slots[5].queue = 0;
    mgr->slots[5].field1c = 0;
    mgr->slots[5].field20 = 0;

    mgr->slots[6].count = -1;
    mgr->slots[6].total = 0;

    if ((flags & 8) != 0) {
        mgr->async = 1;
        mgr->pending = 0x20;
    }

    for (i = 0; i < count; i++) {
        AsyncSource* src = &sources[i];
        const StreamDesc* desc = &descs[i];

        if (desc->type == 'R') {
            src->callback = (AsyncCallback)Fn80080F90;
            stream = Fn80080F88();
        } else if (desc->type == 'A') {
            if ((flags & 8) != 0)
                src->mode = 0x20;
            useSam = 1;
            src->callback = (AsyncCallback)SAMStreamEvent;
            stream = SAMGetStream();
        } else {
            return -1;
        }
        src->id = desc->type;
        src->field0c = desc->field02;
        src->stream = stream;
        src->block = desc->block;
        QueueClear(stream);
    }
    if (!useSam)
        mgr->field04 = 1;

    mgr->field18 = blockBase;
    mgr->limit = arg2;
    mgr->count = count;
    mgr->field14 = flags;

    for (i = 0; i < count; i++) {
        AsyncSource* src = &sources[i];
        AsyncFile* file;
        s32 streamSlot;
        int isR;

        isR = 0;
        if (src->id == 'R')
            isR = 1;
        DebugPrintf("Opening stream async: %s", name);
        streamSlot = BitsetFirstSet(lbl_804D2680, 1, 2);
        file = 0;
        if (streamSlot != -1) {
            BitClear(lbl_804D2680, streamSlot);
            file = (AsyncFile*)(lbl_803FDAC0 + streamSlot * 0x8220);
            if (AsyncOpen(file, name, isR) == 0)
                file = 0;
        }
        src->file = file;
        if (src->file != 0)
            AsyncSeek(src->file, (mgr->field18 + src->block) * 0x800, 0);
    }

    lbl_804D4328 = lbl_804D4328 + 1;
    lbl_804D4324[index / 32] =
        lbl_804D4324[index / 32] | (1 << (index & 0x1F));
    lbl_804D4320 = 0;
    mgr->channel = (mgr->channel + 1) & 7;
    mgr->field08 = (s16)(index | (mgr->channel << 12));
    return (s16)mgr->field08;
}

// FreeHandleChannel (fn_8003CA3C): releases the stream queue channel named by
// `handle`.  Every async file the slot still owns is closed and its stream
// number returned to the global file bitmap, the slot's queues are cleared,
// and the load-manager state is reset.
int FreeHandleChannel(s16 handle)
{
    LoadManagerView* mgr;
    AsyncSource* sources;
    s32 index;
    s32 i;

    if (handle == -1)
        return 0;
    index = handle & 0xFFF;
    if (index >= 1)
        return 0;
    if ((lbl_804D4324[index / 32] & (1 << (handle & 0x1F))) == 0)
        return 0;
    mgr = (LoadManagerView*)&lbl_8040DF00[index];
    if (((handle >> 12) & 7) != (mgr->channel & 7))
        return 0;

    sources = (AsyncSource*)mgr->slots;
    for (i = 0; i < mgr->count; i++) {
        AsyncFile* file = sources[i].file;
        if (file != 0) {
            u32 stream = (u32)((u8*)file - lbl_803FDAC0) / 0x8220;
            AsyncClose(file);
            sources[i].file = 0;
            lbl_804D2680[stream / 32] = lbl_804D2680[stream / 32] |
                                        (1 << (stream & 0x1F));
        }
    }
    for (i = 0; i < mgr->count; i++) {
        if ((u32)sources[i].stream != 0xFFFFFFFF)
            QueueClear(sources[i].stream);
    }

    mgr->processed = 0;
    mgr->completed = 0;
    mgr->async = 0;
    mgr->pending = 0;
    mgr->field04 = 0;
    mgr->field14 = 0;
    mgr->field18 = 0;
    mgr->limit = 0;
    mgr->count = 0;
    mgr->field24 = 0;

    mgr->slots[0].handle = -1;
    mgr->slots[0].count = 0;
    mgr->slots[0].total = 0;
    mgr->slots[0].state = 0;
    mgr->slots[0].handle2 = -1;
    mgr->slots[0].queue = 0;

    mgr->slots[1].handle = -1;
    mgr->slots[1].count = 0;
    mgr->slots[1].total = 0;
    mgr->slots[1].state = 0;
    mgr->slots[1].handle2 = -1;
    mgr->slots[1].queue = 0;

    mgr->slots[2].handle = -1;
    mgr->slots[2].count = 0;
    mgr->slots[2].total = 0;
    mgr->slots[2].state = 0;
    mgr->slots[2].handle2 = -1;
    mgr->slots[2].queue = 0;

    mgr->slots[3].handle = 0;
    mgr->slots[3].count = 0;
    mgr->slots[3].total = 0;
    mgr->slots[3].state = 0;
    mgr->slots[3].arg = 0;
    mgr->slots[3].handle2 = -1;
    mgr->slots[3].queue = 0;
    mgr->slots[3].field1c = 0;
    mgr->slots[3].field20 = 0;

    mgr->slots[4].count = -1;
    mgr->slots[4].total = 0;
    mgr->slots[4].handle2 = -1;
    mgr->slots[4].queue = 0;
    mgr->slots[4].field1c = 0;
    mgr->slots[4].field20 = 0;

    mgr->slots[5].count = -1;
    mgr->slots[5].total = 0;
    mgr->slots[5].handle2 = -1;
    mgr->slots[5].queue = 0;
    mgr->slots[5].field1c = 0;
    mgr->slots[5].field20 = 0;

    mgr->slots[6].count = -1;
    mgr->slots[6].total = 0;

    mgr->channel = (mgr->channel + 1) & 7;
    BitClear(lbl_804D4324, index);
    lbl_804D4328 = lbl_804D4328 - 1;
    return 1;
}
