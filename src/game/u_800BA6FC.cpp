// u_800BA6FC.cpp: group system front end (0x800BA6FC-0x800BAA14): unaligned copy helpers, the selected
// group index, the 60-bit flag set and 13-entry table, and the init/update/shutdown fan-out. Split from
// u_800B1834.cpp: its data follows that file's and it has its own 0.01f and int-to-float constants.
// Names are descriptive guesses; the code is matched against the retail DOL.

typedef unsigned char u8;
typedef unsigned long u32;
typedef signed int    s32;

void TalkFree();
void TalkInit();

// Base Commit/Append (0x8028BC2C / 0x8028BC28) of the Buffer classes in u_8028A8D4.cpp, which the
// array overrides here call directly. Class and member names are guesses.
class ScreenEffectBuffer {
public:
    void Commit(int start, unsigned long size);
    void Append(unsigned char* data, unsigned long size);
};
extern "C" void NeoAgentPoolShutdown(void);
void NeoAgentSaveNotify();
void NeoAgentSystemCreate();
void NeoAgentSystemInit();
extern "C" void fn_80026660(int a, void* callback, int c);
extern "C" void fn_800BF428(void);
extern "C" void fn_800BF4B0(void);
extern "C" void fn_8029B7D0(void);
extern "C" void fn_8029B920(void);
extern "C" void fn_800BAA14(void);
int MsgObjectResolve(int a, int b, int* c);
extern "C" void fn_800B2BA4(void);
extern "C" void fn_800B4614(void);
extern "C" void fn_800B4EFC(void);
extern "C" void fn_800B98F8(void);
extern "C" void fn_800B9A8C(void);
extern "C" void fn_800B9824(void);
extern "C" void fn_800B985C(void);
void NeoAgentFrameUpdate(float value);
extern "C" void fn_800B9528(float value);
extern "C" void fn_800B4D94(float value);
extern "C" void fn_800B4414(float value);
extern "C" void fn_800B29FC(float value);
void TalkUpdate(float value);
extern "C" void fn_8029B85C(float value);
extern "C" void fn_800BA7AC(u8* begin, u8* end);
extern "C" void fn_800BA7CC(u8* begin, u8* end);

u32 lbl_8039B728[14] = {0};    // 13-entry global table
u32 lbl_804D2B90[2] = {0};     // 60-bit global flag set
int lbl_804D4840;              // selected group index

// Copy `size` bytes through the unaligned helper after clearing the tail.
extern "C" void fn_800BA6FC(void* dst, u8* src, u32 size)
{
    ((ScreenEffectBuffer*)dst)->Commit((int)src, size);
    fn_800BA7AC(src, src + (size & ~3u));
}

// Clear the aligned tail, then copy `size` bytes from `src` to `dst`.
extern "C" void fn_800BA744(void* dst, u8* src, u32 size)
{
    u32 start = (u32)src;

    fn_800BA7CC((u8*)start, (u8*)(start + (size & ~3u)));
    ((ScreenEffectBuffer*)dst)->Append((unsigned char*)src, size);
}

extern "C" int fn_800BA7A4(void)
{
    return 4;
}

extern "C" void fn_800BA7AC(u8* begin, u8* end)
{
    for (u8* p = begin; p < end; p += 4) {
    }
}

extern "C" void fn_800BA7CC(u8* begin, u8* end)
{
    for (u8* p = begin; p < end; p += 4) {
    }
}

extern "C" void fn_800BA7EC(int value)
{
    lbl_804D4840 = value;
}

extern "C" int fn_800BA7F4(void)
{
    return lbl_804D4840;
}

// Bit `index` of the 60-bit global flag set.
extern "C" u32 fn_800BA7FC(int index)
{
    if (index < 0 || index >= 0x3c) {
        return 0;
    }
    return lbl_804D2B90[index / 32] & (1 << (index % 32));
}

// Entry `index` of the 13-entry global table.
extern "C" u32 fn_800BA84C(int index)
{
    if (index < 0 || index >= 13) {
        return 0;
    }
    return lbl_8039B728[index];
}

// Store `value` into entry `index` of the 13-entry global table.
extern "C" void fn_800BA878(int index, u32 value)
{
    if (index < 0 || index >= 13) {
        return;
    }
    lbl_8039B728[index] = value;
}

// Reset the group tables and tear down the related subsystems.
extern "C" void fn_800BA8A0(void)
{
    lbl_804D4840 = 0;
    NeoAgentPoolShutdown();
    fn_800B9824();
    TalkFree();
    fn_800BF428();
    fn_8029B7D0();
}

// Scale the frame time by the global group count and drive all subsystems.
extern "C" void fn_800BA8D8(float time)
{
    float scaled = time * (0.01f * (float)(s32)lbl_8039B728[0x0b]);
    NeoAgentFrameUpdate(scaled);
    fn_800B9528(scaled);
    fn_800B4D94(scaled);
    fn_800B4414(scaled);
    fn_800B29FC(scaled);
    TalkUpdate(scaled);
    fn_8029B85C(scaled);
}

extern "C" void fn_800BA970(void)
{
    NeoAgentSaveNotify();
    fn_800B985C();
}

extern "C" void fn_800BA994(void)
{
    fn_800B98F8();
    NeoAgentSystemCreate();
    TalkInit();
}

// Initialise the group system and its subsystems.
extern "C" void fn_800BA9BC(void)
{
    lbl_804D4840 = 1;
    fn_80026660(0x27, (void*)MsgObjectResolve, 0);
    fn_800BAA14();
    fn_800BF4B0();
    fn_800B9A8C();
    NeoAgentSystemInit();
    fn_800B2BA4();
    fn_800B4EFC();
    fn_800B4614();
    fn_8029B920();
}
