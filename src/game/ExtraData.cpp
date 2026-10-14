// ExtraData.cpp: optional 0x70-byte extra block hung off an object (0x8023040C-0x80230504).
// Used from many places (models, actors, effects) to free, read and copy the block.
// Names are descriptive guesses; the code is matched against the retail DOL.

typedef unsigned long u32;

extern "C" void* memcpy(void* dst, const void* src, u32 n);

void* VAlloc(int size);                                   // 0x8009DA68
void VFree(void* p);                                      // 0x8009DA3C

struct ExtraHolder {
    char pad0[0x18];
    int id;
    void* extra;
};

void ExtraFree(ExtraHolder* h) {
    if (h && h->extra) {
        VFree(h->extra);
        h->extra = 0;
    }
}

int ExtraGetId(ExtraHolder* h) {
    if (h == 0) {
        return 0;
    } else {
        return h->id;
    }
}

int ExtraCopy(ExtraHolder* dst, ExtraHolder* src) {
    if (!dst || !src) {
        return 0;
    }
    if (src->extra) {
        if (!dst->extra) {
            dst->extra = VAlloc(0x70);
        }
        memcpy(dst->extra, src->extra, 0x70);
    } else if (dst->extra) {
        VFree(dst->extra);
        dst->extra = 0;
    }
    return 1;
}
