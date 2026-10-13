// Level variables database (0x800DED18-0x800DFE3C): a file type (handler 0x2D) holding groups of
// objects and variable definitions. Each group has its variable list (key, type, value index) and
// its objects (key, value array, data block). Values are ints, floats, hashes or string offsets
// (types 1 and 6), which are relocated against the string pool on load.
// Names are descriptive guesses; the code is matched against the retail DOL.

typedef unsigned long u32;
typedef unsigned char u8;

void FileFlushBuffer(void* dst, int size);
int FileAddHandler(int type, int (*handler)(void*, int, void*), void* arg);
int GameEvent(int event, int arg);

struct VarHeader {
    int version;
    int nGroups;
    int nVars;
    int nObjs;
    int nValues;
    int dataSize;
    int stringSize;
    u8 pad1C[4];
};

struct VarDef {
    u32 key;
    int type;
    int unk8;
    int index;
};

struct VarObj {
    u32 key;
    int nValues;
    int* values;
    int data;
};

struct VarGroup {
    u32 key;
    int nVars;
    VarDef* vars;
    int nObjs;
    VarObj* objs;
    u8 pad14[0xC];
};

static VarHeader* sVarHeader;
static VarGroup* sVarGroups;
static VarDef* sVarDefs;
static VarObj* sVarObjs;
static int* sVarValues;
static char* sVarData;
static char* sVarStrings;

inline VarGroup* VarFindGroup(u32 key) {
    int i;

    if (sVarHeader == 0) {
        return 0;
    }
    for (i = 0; i < sVarHeader->nGroups; i++) {
        if (sVarGroups[i].key == key) {
            return &sVarGroups[i];
        }
    }
    return 0;
}

inline VarDef* VarFindDef(VarGroup* group, u32 key) {
    int i;

    if (sVarHeader == 0) {
        return 0;
    }
    for (i = 0; i < group->nVars; i++) {
        if (group->vars[i].key == key) {
            return &group->vars[i];
        }
    }
    return 0;
}

inline int VarFind(u32 group, u32 obj, u32 var, VarObj** objOut, VarDef** varOut) {
    int i;
    int j;
    int k;

    if (sVarHeader == 0) {
        return 0;
    }
    for (i = 0; i < sVarHeader->nGroups; i++) {
        if (sVarGroups[i].key == group || group == 0) {
            for (j = 0; j < sVarGroups[i].nObjs; j++) {
                if (sVarGroups[i].objs[j].key == obj) {
                    for (k = 0; k < sVarGroups[i].nVars; k++) {
                        if (sVarGroups[i].vars[k].key == var) {
                            *objOut = &sVarGroups[i].objs[j];
                            *varOut = &sVarGroups[i].vars[k];
                            return 1;
                        }
                    }
                    break;
                }
            }
        }
    }
    return 0;
}

inline int* VarFindObjValue(u32 key, int index) {
    VarObj* objs;
    int i;

    if (sVarHeader == 0) {
        return 0;
    }
    objs = sVarObjs;
    for (i = 0; i < sVarHeader->nObjs; i++) {
        if (objs[i].key == key && index < objs[i].nValues) {
            return &objs[i].values[index];
        }
    }
    return 0;
}

inline float VarFloat(u32 group, u32 o, u32 var) {
    VarDef* def;
    VarObj* obj;

    if (VarFind(group, o, var, &obj, &def) && def->type == 4) {
        return ((float*)obj->values)[def->index];
    }
    return 0.0f;
}

inline int VarInt(u32 group, u32 o, u32 var) {
    VarDef* def;
    VarObj* obj;

    if (VarFind(group, o, var, &obj, &def) && def->type == 0) {
        return obj->values[def->index];
    }
    return -1;
}

inline int VarHash(u32 group, u32 o, u32 var) {
    VarDef* def;
    VarObj* obj;

    if (VarFind(group, o, var, &obj, &def) && def->type == 3) {
        return obj->values[def->index];
    }
    return -1;
}

void VarReset(void) {
    sVarGroups = 0;
    sVarDefs = 0;
    sVarObjs = 0;
    sVarValues = 0;
    sVarData = 0;
    sVarHeader = 0;
}

int VarGroupVarCount(u32 key) {
    int i;

    if (sVarHeader == 0) {
        return 0;
    }
    for (i = 0; i < sVarHeader->nGroups; i++) {
        if (sVarGroups[i].key == key) {
            return sVarGroups[i].nVars;
        }
    }
    return -1;
}

int VarGroupObjCount(u32 key) {
    int i;

    if (sVarHeader == 0) {
        return 0;
    }
    for (i = 0; i < sVarHeader->nGroups; i++) {
        if (sVarGroups[i].key == key) {
            return sVarGroups[i].nObjs;
        }
    }
    return 0;
}

float VarEventFloat(int event, u32 var) { return VarFloat(0, GameEvent(0xC0A77187, event), var); }

int VarEventInt(int event, u32 var) { return VarInt(0, GameEvent(0xC0A77187, event), var); }

int VarExists(u32 group, u32 obj, u32 var) {
    int i;
    int j;
    int k;

    if (group || obj || var) {
        for (i = 0; i < sVarHeader->nGroups; i++) {
            if (sVarGroups[i].key == group || group == 0) {
                VarGroup* g = &sVarGroups[i];
                for (j = 0; j < g->nObjs; j++) {
                    if (g->objs[j].key == obj || obj == 0) {
                        for (k = 0; k < g->nVars; k++) {
                            if (g->vars[k].key == var || var == 0) {
                                return 1;
                            }
                        }
                        break;
                    }
                }
                break;
            }
        }
    }
    return 0;
}

u32 VarObjKey(u32 key, int index) {
    VarGroup* group = VarFindGroup(key);

    if (group) {
        return group->objs[index].key;
    }
    return -1;
}

int VarGetHash(u32 key, int index, u32 var) {
    VarGroup* group = VarFindGroup(key);

    if (group) {
        VarDef* def = VarFindDef(group, var);
        if (def && def->type == 3) {
            VarObj* obj = &group->objs[index];
            return obj->values[def->index];
        }
    }
    return -1;
}

float VarGetFloat(u32 key, int index, u32 var) {
    VarGroup* group = VarFindGroup(key);

    if (group) {
        VarDef* def = VarFindDef(group, var);
        if (def && def->type == 4) {
            VarObj* obj = &group->objs[index];
            return ((float*)obj->values)[def->index];
        }
    }
    return 0.0f;
}

int Fn800DF3B8(u32 key, int index, u32 var) {
    VarGroup* group = VarFindGroup(key);

    if (group) {
        VarDef* def = VarFindDef(group, var);
        if (def && def->type == 0) {
            VarObj* obj = &group->objs[index];
            return obj->values[def->index];
        }
    }
    return -1;
}

int VarFindHash(u32 group, u32 obj, u32 var) { return VarHash(group, obj, var); }

float VarFindFloat(u32 group, u32 obj, u32 var) { return VarFloat(group, obj, var); }

int VarFindInt(u32 group, u32 obj, u32 var) { return VarInt(group, obj, var); }

int Fn800DF814(u32 key, int obj) {
    int i;
    int j;

    if (sVarHeader == 0) {
        return -1;
    }
    for (i = 0; i < sVarHeader->nGroups; i++) {
        if (sVarGroups[i].key == key) {
            VarGroup* g = &sVarGroups[i];
            for (j = 0; j < g->nObjs; j++) {
                if (g->objs[j].key == (u32)obj) {
                    return j;
                }
            }
            break;
        }
    }
    return -1;
}

int VarObjData(u32 key, int index) {
    int i;
    int result;

    if (sVarHeader == 0) {
        return 0;
    }
    result = 0;
    for (i = 0; i < sVarHeader->nGroups; i++) {
        if (sVarGroups[i].key == key) {
            if (index >= 0 && index < sVarGroups[i].nObjs) {
                result = sVarGroups[i].objs[index].data;
            }
            break;
        }
    }
    return result;
}

int VarObjValue(u32 key, int index) {
    int* p = VarFindObjValue(key, index);

    if (p) {
        return *p;
    }
    return -1;
}

int VarObjValue2(u32 key, int index) {
    int* p = VarFindObjValue(key, index);

    if (p) {
        return *p;
    }
    return -1;
}

int VarLoad(void* buffer, int size, void* arg);

void VarInit(void) {
    sVarGroups = 0;
    sVarDefs = 0;
    sVarObjs = 0;
    sVarValues = 0;
    sVarData = 0;
    sVarStrings = 0;
    FileAddHandler(0x2D, VarLoad, 0);
}

inline bool VarHasBlock(int size, int n) { return size > 0 && n > 0; }

int VarLoad(void* buffer, int size, void* arg) {
    int left = size;
    char* p = (char*)buffer;
    VarHeader* hdr;
    int n;
    int m;
    int i;

    FileFlushBuffer(p, sizeof(VarHeader));
    hdr = left > 0 ? (VarHeader*)p : 0;
    sVarHeader = hdr;
    left -= sizeof(VarHeader);
    p += sizeof(VarHeader);
    if (hdr->version == 3) {
        FileFlushBuffer(p, hdr->nGroups * sizeof(VarGroup) + hdr->nVars * sizeof(VarDef) + hdr->nObjs * sizeof(VarObj) +
                               hdr->nValues * sizeof(int));

        n = sVarHeader->nGroups * sizeof(VarGroup);
        sVarGroups = VarHasBlock(left, n) ? (VarGroup*)p : 0;
        left -= n;
        p += n;

        n = sVarHeader->nVars * sizeof(VarDef);
        sVarDefs = VarHasBlock(left, n) ? (VarDef*)p : 0;
        left -= n;
        p += n;

        n = sVarHeader->nObjs * sizeof(VarObj);
        sVarObjs = VarHasBlock(left, n) ? (VarObj*)p : 0;
        left -= n;
        p += n;

        n = sVarHeader->nValues * sizeof(int);
        sVarValues = VarHasBlock(left, n) ? (int*)p : 0;
        left -= n;
        p += n;

        n = sVarHeader->dataSize;
        sVarData = VarHasBlock(left, n) ? p : 0;
        p += n;
        left -= n;

        n = sVarHeader->stringSize;
        if (n > 0) {
            sVarStrings = VarHasBlock(left, n) ? p : 0;
        } else {
            sVarStrings = "";
        }

        // Retail reloads the pool pointers here instead of reusing the values just stored.
        {
            VarDef* defs = *(VarDef* volatile*)&sVarDefs;
            VarObj* objs = *(VarObj* volatile*)&sVarObjs;
            int* values = *(int* volatile*)&sVarValues;
            char* data = *(char* volatile*)&sVarData;
            char* strings = *(char* volatile*)&sVarStrings;
            int j;
            int k;

            for (i = 0; i < sVarHeader->nGroups; i++) {
                sVarGroups[i].vars = defs;
                defs += sVarGroups[i].nVars;
                sVarGroups[i].objs = objs;
                objs += sVarGroups[i].nObjs;
                for (j = 0; j < sVarGroups[i].nObjs; j++) {
                    int len;
                    sVarGroups[i].objs[j].values = values;
                    values += sVarGroups[i].nVars;
                    len = sVarGroups[i].objs[j].data;
                    sVarGroups[i].objs[j].data = (int)data;
                    data += len;
                }
            }

            for (i = 0; i < sVarHeader->nGroups; i++) {
                for (m = 0; m < sVarGroups[i].nObjs; m++) {
                    for (k = 0; k < sVarGroups[i].nVars; k++) {
                        int type = sVarGroups[i].vars[k].type;
                        int index = sVarGroups[i].vars[k].index;
                        if (type == 1) {
                            sVarGroups[i].objs[m].values[index] = (int)(strings + sVarGroups[i].objs[m].values[index]);
                        } else if (type == 6) {
                            sVarGroups[i].objs[m].values[index] = (int)(strings + sVarGroups[i].objs[m].values[index]);
                        }
                    }
                }
            }
        }
    }
    return 1;
}
