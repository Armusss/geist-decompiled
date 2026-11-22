// u_801E0850.cpp (0x801E0850-0x801E092C): the effect-module fan-out hooks (reset / per-frame update / load /
// unload of the trail, crouch and timer modules and two others), called from AppMain and u_80153C10.
// Boundary evidence: fn_801E08CC calls the trail texture preload fn_801DE7A0 (0x3C bytes) out of line; built in
// one file with it, CodeWarrior auto-inlines that call (tried), so the hooks are not in the trail file. They
// read no data; the Lightning file u_801E092C starts with its own .bss / .sdata / .sdata2.
// Names are descriptive guesses.

void CrouchNop();
void CrouchAdvancePhases(float dt);
void CrouchInit();
void CrouchResetPhases();
void TimerModuleReset();
void TimerModuleUpdate(float dt);
void TimerModuleLoad();
void TimerModuleUnload();
extern "C" {
void fn_801DEB6C(void);
void fn_801DECA8(float dt);
void fn_801DE7A0(void);
void fn_801DED94(void);
void fn_801E3AD4(void);
void fn_801E5D5C(void);
void fn_801E5D60(float dt);
void fn_801E3BBC(float dt);
void fn_801E5DD4(void);
void fn_801E3A88(void);
void fn_801E3E14(void);
void fn_801E5E44(void);
}

//@@801E0850
// Effect-module hooks: reset, per-frame update, load and unload of the trail, crouch and timer modules
// (called from AppMain).
extern "C" void fn_801E0850(void) {
    fn_801DEB6C();
    fn_801E3AD4();
    fn_801E5D5C();
    CrouchNop();
    TimerModuleReset();
}
//@@end

//@@801E0880
extern "C" void fn_801E0880(float dt) {
    fn_801E5D60(dt);
    fn_801DECA8(dt);
    fn_801E3BBC(dt);
    CrouchAdvancePhases(dt);
    TimerModuleUpdate(dt);
}
//@@end

//@@801E08CC
extern "C" void fn_801E08CC(void) {
    fn_801E5DD4();
    fn_801DE7A0();
    fn_801E3A88();
    CrouchInit();
    TimerModuleLoad();
}
//@@end

//@@801E08FC
extern "C" void fn_801E08FC(void) {
    fn_801DED94();
    fn_801E3E14();
    fn_801E5E44();
    CrouchResetPhases();
    TimerModuleUnload();
}
//@@end
