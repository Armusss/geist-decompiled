// Save game (0x800DE0C4-0x800DEC54): the 0x420-byte "Geist Save Data" block with three save slots.
// Commands arrive as message 0x7106; the block is written to the memory card (Store mode 2) and kept
// in ARAM (mode 1). Each slot stores a level name, a best level and extra data blocks gathered from
// other systems with message 0x7100 and handed back with 0x7101.
// Names are descriptive guesses; the code is matched against the retail DOL.

typedef unsigned long u32;
typedef signed short s16;
typedef unsigned char u8;

extern "C" {
char* strcpy(char* dst, const char* src);
char* strchr(const char* s, int c);
char* strrchr(const char* s, int c);
void* memset(void* dst, int val, u32 n);
void* memcpy(void* dst, const void* src, u32 n);

// Callees in other files, not named yet.
int fn_802A1D04(void);
int fn_8014FF10(const char* level);
void fn_800259C8(void* p, int size, int arg);
void fn_800FE724(int arg);
void fn_80026660(int id, int (*handler)(int, int, void*), int arg);
extern int lbl_804D5828;
extern int lbl_804D5860;
}

extern "C" int SendMsg(int target, int msg, void* data);
void HudMessage(int id, const char* text, float r, float g, float b);
int StoreSave(int mode, const char* name, int size, u32 flags, void* src);
int StoreLoad(int mode, const char* name, int size, u32 flags, void* dst);
void StoreCardUnmount(void);
int StoreInit(int code);

#define SAVE_VALID 0x80000000

struct SaveBlocks {
    int unk0;
    int used;
    int count;
    u8 padC[4];
    u8 data[0x100];
};

struct SaveSlot {
    char name[0x10];
    int bestLevel;
    u8 pad14[0xC];
    SaveBlocks blk;
};

struct SaveData {
    u32 flags;
    u8 pad4[0xC];
    int hdr10;
    int hdr14;
    int hdr18;
    int hdr1C;
    int hdr20;
    int hdr24;
    int hdr28;
    u8 options[0x8];
    s16 statA[3];
    s16 statB[3];
    u8 pad40[0x50];
    SaveSlot slots[3];
};

struct SaveMsg {
    int unk0;
    void* data;
    u8 pad8[8];
};

struct SaveBlockMsg {
    int unk0;
    int size;
    u8* buffer;
    int length;
    u32 flag;
    u8 pad14[0xC];
};

struct SaveSendMsg {
    int unk0;
    int unk4;
    u8* data;
    int length;
    u8 pad10[0x10];
};

struct SaveCmdMsg {
    int unk0;
    int cmd;
    int arg;
};

static SaveData sSave;
static char sSaveTitle[0x20];
static char sSavePath[0x100];

u32 sSaveKeys[2] = {0x63000000, 0x02000000};

static int sSaveSlot;
static float sSaveTimer;
static int sSaveBusy;
static int sSaveLoaded;

int SaveCommand(int cmd, int arg);
int SaveTryWrite(int mode, SaveData* save, u32 flags);

inline int SaveFlush(int mode, u32 flags) {
    int result;

    if (sSave.flags & SAVE_VALID) {
        sSave.flags &= ~SAVE_VALID;
        result = StoreSave(mode, "Geist Save Data", 0x420, flags, &sSave);
        sSave.flags |= SAVE_VALID;
        return result;
    }
    return 0;
}

void SaveUpdate(float dt) {
    if (sSaveTimer > 0.0f) {
        sSaveTimer -= dt;
        if (sSaveTimer <= 0.0f) {
            if (SaveFlush(2, 0x10) == 1) {
                fn_800FE724(0);
                HudMessage(0, "Save Complete", 1.0f, 1.0f, 1.0f);
            }
        }
    }
}

void SaveSetSlotName(int slot, const char* name) {
    if (slot >= 0 && slot < 3) {
        strcpy(sSaveTitle, name);
        strcpy(sSave.slots[slot].name, name);
    }
}

bool SaveIsNewBest(void) {
    int level;

    if (fn_802A1D04()) {
        return 0;
    }
    level = fn_8014FF10(sSave.slots[sSaveSlot].name);
    return level >= sSave.slots[sSaveSlot].bestLevel;
}

int SaveGetBestLevel(int slot) {
    if (slot >= 0 && slot < 3) {
        return sSave.slots[slot].bestLevel;
    }
    return 0;
}

const char* SaveGetSlotName(int slot) {
    if (slot >= 0 && slot < 3) {
        return sSave.slots[slot].name;
    }
    return "xXx";
}

void SaveResetOptions(void) { fn_800259C8(sSave.options, 0x40, 1); }

int SaveCommand(int cmd, int arg) {
    SaveMsg msgLoad;
    SaveMsg msgCheck;
    SaveMsg msgTitle;
    SaveBlockMsg block;

    switch (cmd) {
    case 1: {
        char* name;
        int* hdr;

        if ((const char*)arg == 0) {
            name = 0;
        } else {
            char* p;
            strcpy(sSavePath, (const char*)arg);
            p = strchr(sSavePath, '.');
            if (p) {
                *p = 0;
            }
            p = strrchr(sSavePath, '/');
            if (p) {
                name = p + 1;
            } else {
                p = strrchr(sSavePath, '\\');
                if (p) {
                    name = p + 1;
                } else {
                    name = sSavePath;
                }
            }
        }

        hdr = &sSave.hdr10;
        msgLoad.data = hdr;
        if (SendMsg(0x4B000000, 0x7104, &msgLoad) == 0) {
            memset(&sSave, 0, sizeof(sSave));
            *hdr = 0;
            sSave.hdr14 = 0x32;
            sSave.hdr18 = 0;
            sSave.hdr1C = 0x32;
            sSave.hdr20 = 0x43;
            sSave.hdr28 = 0;
            sSave.hdr24 = 1;
        }
        SendMsg(0x63000000, 0x7104, &msgLoad);

        if (sSaveSlot >= 0 && name) {
            SaveSlot* slot = &sSave.slots[sSaveSlot];
            int level;
            int used;
            int i;
            u32* key;
            SaveBlocks* blk;
            u8* data;

            strcpy(slot->name, name);
            level = fn_8014FF10(slot->name);
            if (level > slot->bestLevel) {
                slot->bestLevel = level;
            }
            used = 0;
            data = slot->blk.data;
            slot->blk.count = used;
            i = used;
            blk = &slot->blk;
            key = sSaveKeys;
            slot->blk.unk0 = used;
            slot->blk.used = used;
            for (; i < 2; i++, key++) {
                int end = used + 8;
                u32* entry;

                block.buffer = data + end;
                block.size = 0x100 - end;
                entry = (u32*)(data + used);
                block.length = 0;
                block.flag = 1;
                if (SendMsg(*key, 0x7100, &block) && block.flag && block.length) {
                    end += block.length;
                    entry[1] = block.length;
                    entry[0] = *key;
                    if (end <= 0x100) {
                        used = end;
                        blk->count++;
                    }
                }
            }
            blk->used = used;
        }
        StoreCardUnmount();
        sSave.flags |= SAVE_VALID;
        return 1;
    }
    case 2:
        sSaveBusy = 1;
        msgTitle.data = sSaveTitle;
        SendMsg(0x38000000, 0x5600, &msgTitle);
        return 1;
    case 8:
        if (sSave.flags & SAVE_VALID) {
            msgCheck.data = &sSave.hdr10;
            SendMsg(0x4B000000, 0x7105, &msgCheck);
            return 1;
        }
        return 0;
    case 3:
        sSaveBusy = arg;
        return 1;
    case 6:
        if (arg == 1) {
            return SaveFlush(1, 0);
        }
        return SaveFlush(2, 0);
    case 13:
        sSaveTimer = 1000.0f;
        return 1;
    case 14:
        return SaveTryWrite(2, &sSave, 0);
    case 7: {
        int flags = arg;
        int mode = 2;
        if (flags == 1) {
            mode = 1;
        }
        if (mode == 2 && sSaveLoaded == 0) {
            mode = 1;
        }
        if (mode == 2) {
            StoreSave(1, "Geist Save Data", 0x420, 0x10, &sSave);
            switch (StoreLoad(2, "Geist Save Data", 0x420, flags, &sSave)) {
            case 1:
                sSave.flags |= SAVE_VALID;
                break;
            case 3:
                StoreLoad(1, "Geist Save Data", 0x420, 0x10, &sSave);
                sSaveLoaded = 1;
                break;
            default:
                StoreLoad(1, "Geist Save Data", 0x420, 0x10, &sSave);
                sSaveLoaded = 0;
                break;
            }
        } else if (StoreLoad(1, "Geist Save Data", 0x420, 0, &sSave) == 1) {
            sSave.flags |= SAVE_VALID;
        }
        return 1;
    }
    case 4:
        sSaveSlot = arg;
        return 1;
    case 5:
        return sSaveSlot;
    case 9:
        if (arg >= 0 && (sSave.flags & SAVE_VALID) && sSave.slots[arg].name[0]) {
            return 1;
        }
        return 0;
    case 10: {
        int slot = *(int*)arg;
        if ((sSave.flags & SAVE_VALID) && slot >= 0 && sSave.slots[slot].name[0]) {
            strcpy((char*)arg, sSave.slots[slot].name);
            return 1;
        }
        return 0;
    }
    case 12:
        if (sSave.flags & SAVE_VALID) {
            int src = arg & 0xFFFF;
            int dst = (u32)arg >> 16;
            if (sSave.slots[src].name[0]) {
                memcpy(&sSave.slots[dst], &sSave.slots[src], sizeof(SaveSlot));
                sSave.statA[dst] = sSave.statA[src];
                sSave.statB[dst] = sSave.statB[src];
                StoreCardUnmount();
                return 1;
            }
        }
        return 0;
    case 11:
        if (arg == -1) {
            memset(&sSave.slots[0], 0, sizeof(SaveSlot));
            memset(&sSave.slots[1], 0, sizeof(SaveSlot));
            memset(&sSave.slots[2], 0, sizeof(SaveSlot));
            sSave.statA[2] = 0;
            sSave.statA[1] = 0;
            sSave.statA[0] = 0;
            sSave.statB[2] = 0;
            sSave.statB[1] = 0;
            sSave.statB[0] = 0;
            StoreCardUnmount();
            return SaveFlush(1, 0);
        }
        if ((sSave.flags & SAVE_VALID) && sSave.slots[arg].name[0]) {
            memset(&sSave.slots[arg], 0, sizeof(SaveSlot));
            sSave.statA[arg] = 0;
            sSave.statB[arg] = 0;
            StoreCardUnmount();
            return 1;
        }
        return 0;
    }
    return 0;
}

void SaveClearBusy(void) { sSaveBusy = 0; }

void SaveRestoreBlocks(void) {
    if (sSaveBusy && sSaveSlot >= 0) {
        SaveSlot* slot = &sSave.slots[sSaveSlot];
        int offset;
        int i;
        u8* blocks;
        int* hdr;

        offset = 0;
        i = offset;
        hdr = &slot->blk.unk0;
        blocks = slot->blk.data;
        for (; i < hdr[2]; i++) {
            SaveSendMsg msg;
            u32* entry = (u32*)(blocks + offset);
            offset += 8;
            msg.data = blocks + offset;
            msg.length = entry[1];
            offset += entry[1];
            SendMsg(entry[0], 0x7101, &msg);
        }
    }
}

int SaveIsBusy(void) { return sSaveBusy; }

void SaveNop(void) {}

int SaveMsgHandler(int target, int msg, void* data);

int SaveRegister(void) {
    fn_80026660(0x50, SaveMsgHandler, 0);
    return 1;
}

void SaveInit(void) {
    sSaveSlot = -1;
    StoreInit(0x420);
}

int SaveTryWrite(int mode, SaveData* save, u32 flags) {
    SaveData tmp;
    int result;

    if (save->flags & SAVE_VALID) {
        lbl_804D5828 = 1;
        for (;;) {
            int r;
            save->flags &= ~SAVE_VALID;
            result = StoreSave(mode, "Geist Save Data", 0x420, flags, save);
            save->flags |= SAVE_VALID;
            if (result != 5) {
                break;
            }
            lbl_804D5860 = 1;
            r = StoreLoad(2, "Geist Save Data", 0x420, flags, &tmp);
            lbl_804D5860 = 0;
            if (r == 1) {
                tmp.hdr10 = save->hdr10;
                tmp.hdr14 = save->hdr14;
                tmp.hdr18 = save->hdr18;
                tmp.hdr1C = save->hdr1C;
                tmp.hdr20 = save->hdr20;
                tmp.hdr28 = save->hdr28;
                tmp.hdr24 = save->hdr24;
                memcpy(save, &tmp, sizeof(SaveData));
            } else if (r != 3) {
                break;
            }
        }
        lbl_804D5828 = 0;
        return result;
    }
    return 0;
}

int SaveMsgHandler(int target, int msg, void* data) {
    if (msg == 0x7106) {
        SaveCmdMsg* m = (SaveCmdMsg*)data;
        return SaveCommand(m->cmd, m->arg);
    }
    return 0;
}

void SaveGetOptions(int* dst, int n) {
    int i;

    for (i = 0; i < n; i++) {
        dst[i] = ((int*)&sSave)[i + 11];
    }
}
