// u_80161F40.cpp: the ghost-action behaviour registry (0x80161F40-0x80161FF4).
// A table of 16 behaviour types, each a name hash and five callbacks (implemented in the behaviour
// files), and a lookup that turns an action id into the behaviour's display name. Every entry's name is
// the empty string in retail.
// Names are descriptive guesses; the code is matched against the retail DOL.

typedef unsigned long u32;

// From a shared header: names of the ghost actions (the table itself is unused here).
static const char* sActionNames[] = {
    "POSSESS", "GHOSTACTION", "SLIP", "HIJACK", "HOSTACTION", "EXCHANGE", "H_HIJACK", "TALK", "SCAN",
};

// Growable character buffer (code at 0x8028B1E4-0x8028C5xx).
class Buffer {
public:
    Buffer();                                  // 0x8028BA9C
    virtual ~Buffer();                         // 0x8028B9C0
    virtual void Release(char* p);

    char* mData;
    int mLength;
    int mUnk0C;
    int mOwned;
};

class String : public Buffer {
public:
    String();                                  // 0x8028C500
    String(const char* s);                     // 0x8028C400
    ~String() {}
    virtual void Release(char* p);
};

struct Owner;
struct Beh;

// One behaviour type: name hash, callbacks (init, free, update, msg, query) and display name.
struct BehType {
    u32 hash;
    int (*init)(Owner* o, Beh* b);
    int (*free)(Owner* o, Beh* b);
    int (*update)(Owner* o, Beh* b);
    int (*msg)(Owner* o, Beh* b, int msg);
    int (*query)(Owner* o, Beh* b);
    const char* name;
};

int Fn800F8A98(unsigned long hash);  // 0x800F8A98
u32 HashMix(u32 h, u32 key);          // 0x800F78AC

// Behaviour callbacks that are already named (Credits.cpp, Thermostat.cpp).
int NoBotInit(Owner* o, Beh* b);
int NoBotFree(Owner* o, Beh* b);
int NoBotUpdate(Owner* o, Beh* b);
int NoBotMsg(Owner* o, Beh* b, int msg);
int NoBotQuery(Owner* o, Beh* b);
int ThermoBehInit(Owner* o, Beh* b);
int ThermoBehDestroy(Owner* o, Beh* b);
int ThermoBehUpdate(Owner* o, Beh* b);
int ThermoBehMsg(Owner* o, Beh* b, int msg);
int ThermoBehFn5(Owner* o, Beh* b);

// Behaviour callbacks in other files, not named yet.
extern "C" {
int fn_80161E84(Owner* o, Beh* b);
int fn_80161E28(Owner* o, Beh* b);
int fn_80161C44(Owner* o, Beh* b);
int fn_8016189C(Owner* o, Beh* b, int msg);
int fn_80161894(Owner* o, Beh* b);
int fn_80178470(Owner* o, Beh* b);
int fn_80178414(Owner* o, Beh* b);
int fn_80178270(Owner* o, Beh* b);
int fn_80177F80(Owner* o, Beh* b, int msg);
int fn_80177F78(Owner* o, Beh* b);
int fn_8017AEAC(Owner* o, Beh* b);
int fn_8017AE50(Owner* o, Beh* b);
int fn_8017AD38(Owner* o, Beh* b);
int fn_8017AB18(Owner* o, Beh* b, int msg);
int fn_8017AB10(Owner* o, Beh* b);
int fn_801A1C3C(Owner* o, Beh* b);
int fn_801A1BE0(Owner* o, Beh* b);
int fn_801A1A3C(Owner* o, Beh* b);
int fn_801A17D4(Owner* o, Beh* b, int msg);
int fn_801A17CC(Owner* o, Beh* b);
int fn_801791B0(Owner* o, Beh* b);
int fn_80179154(Owner* o, Beh* b);
int fn_80178FA0(Owner* o, Beh* b);
int fn_80178D54(Owner* o, Beh* b, int msg);
int fn_80178D4C(Owner* o, Beh* b);
int fn_801777E8(Owner* o, Beh* b);
int fn_801777B0(Owner* o, Beh* b);
int fn_80177694(Owner* o, Beh* b);
int fn_8017744C(Owner* o, Beh* b, int msg);
int fn_80177444(Owner* o, Beh* b);
int fn_801CF57C(Owner* o, Beh* b);
int fn_801CF520(Owner* o, Beh* b);
int fn_801CF3A4(Owner* o, Beh* b);
int fn_801CF108(Owner* o, Beh* b, int msg);
int fn_801CF100(Owner* o, Beh* b);
int fn_8022B964(Owner* o, Beh* b);
int fn_8022B908(Owner* o, Beh* b);
int fn_8022B714(Owner* o, Beh* b);
int fn_8022B310(Owner* o, Beh* b, int msg);
int fn_8022B308(Owner* o, Beh* b);
int fn_8017F614(Owner* o, Beh* b);
int fn_8017F5DC(Owner* o, Beh* b);
int fn_8017F504(Owner* o, Beh* b);
int fn_8017F188(Owner* o, Beh* b, int msg);
int fn_8017F130(Owner* o, Beh* b);
int fn_80299E60(Owner* o, Beh* b);
int fn_80299E28(Owner* o, Beh* b);
int fn_80299D50(Owner* o, Beh* b);
int fn_80299994(Owner* o, Beh* b, int msg);
int fn_8029998C(Owner* o, Beh* b);
int fn_80253360(Owner* o, Beh* b);
int fn_80253328(Owner* o, Beh* b);
int fn_802531B4(Owner* o, Beh* b);
int fn_80252F34(Owner* o, Beh* b, int msg);
int fn_80252F2C(Owner* o, Beh* b);
int fn_802E2DD0(Owner* o, Beh* b);
int fn_802E2D98(Owner* o, Beh* b);
int fn_802E2C30(Owner* o, Beh* b);
int fn_802E29F8(Owner* o, Beh* b, int msg);
int fn_802E29F0(Owner* o, Beh* b);
int fn_802D4288(Owner* o, Beh* b);
int fn_802D4250(Owner* o, Beh* b);
int fn_802D4178(Owner* o, Beh* b);
int fn_802D3A58(Owner* o, Beh* b, int msg);
int fn_802D3A50(Owner* o, Beh* b);
}

BehType gBehTypes[] = {
    { 0x0024BD87, 0, 0, 0, 0, 0, "" },
    { 0x0000FE93, fn_80161E84, fn_80161E28, fn_80161C44, fn_8016189C, fn_80161894, "" },
    { 0x7990FA64, fn_80178470, fn_80178414, fn_80178270, fn_80177F80, fn_80177F78, "" },
    { 0x806190E8, fn_8017AEAC, fn_8017AE50, fn_8017AD38, fn_8017AB18, fn_8017AB10, "" },
    { 0x00013DC9, fn_801A1C3C, fn_801A1BE0, fn_801A1A3C, fn_801A17D4, fn_801A17CC, "" },
    { 0xA602A7E6, fn_801791B0, fn_80179154, fn_80178FA0, fn_80178D54, fn_80178D4C, "" },
    { 0x5FC4CF74, fn_801777E8, fn_801777B0, fn_80177694, fn_8017744C, fn_80177444, "" },
    { 0x002E3FB5, fn_801CF57C, fn_801CF520, fn_801CF3A4, fn_801CF108, fn_801CF100, "" },
    { 0x00012971, fn_8022B964, fn_8022B908, fn_8022B714, fn_8022B310, fn_8022B308, "" },
    { 0xC8759B80, NoBotInit, NoBotFree, NoBotUpdate, NoBotMsg, NoBotQuery, "" },
    { 0x140AEB1E, fn_8017F614, fn_8017F5DC, fn_8017F504, fn_8017F188, fn_8017F130, "" },
    { 0x44B1D348, fn_80299E60, fn_80299E28, fn_80299D50, fn_80299994, fn_8029998C, "" },
    { 0x12DD2E2A, fn_80253360, fn_80253328, fn_802531B4, fn_80252F34, fn_80252F2C, "" },
    { 0x1BABAB25, ThermoBehInit, ThermoBehDestroy, ThermoBehUpdate, ThermoBehMsg, ThermoBehFn5, "" },
    { 0xF2BEA07D, fn_802E2DD0, fn_802E2D98, fn_802E2C30, fn_802E29F8, fn_802E29F0, "" },
    { 0x3194066B, fn_802D4288, fn_802D4250, fn_802D4178, fn_802D3A58, fn_802D3A50, "" },
};
int gBehTypeCount = 16;

inline int BehFind(u32 hash) {
    int i;

    for (i = 0; i < gBehTypeCount; i++) {
        if (gBehTypes[i].hash == hash) {
            return i;
        }
    }
    return -1;
}

String BehName(unsigned long id) {
    int i = BehFind(HashMix(Fn800F8A98(id), 0xCC330C14));

    if (i != -1) {
        return String(gBehTypes[i].name);
    }
    return String("");
}
