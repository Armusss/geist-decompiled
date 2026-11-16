#ifndef GAME_EFFECT_H
#define GAME_EFFECT_H

// Effect objects (weapons, tools, emitters attached to an actor) and their type descriptors.
// Names are descriptive guesses.
//
// Evidence:
//   EffectObj  >=0x10 the callbacks read the per-type data at +0x0C and the emitter handle at +0x08
//                    (SpiritGunQuery/Msg/Update, HomingMsg, ExtinguisherUpdate, all Matching). The object is
//                    allocated by the effect system, not by these units, so its full size is not known.
//   EffectType 0x30  the descriptors are 0x30 bytes apart in .data and are addressed absolutely: a shorter
//                    declaration makes CodeWarrior use small-data addressing and breaks the Create functions
//                    (docs/matching-tricks.md, "Effect Create functions"). The lazy name hash is cached at +0x04.
//                    Callback signatures are those of the eight units that declared them (seven Matching).

struct EffectObj {
    int unk0;                                  // 0x00
    int unk4;                                  // 0x04
    int emitter;                               // 0x08
    void* data;                                // 0x0C
};

struct EffectType {
    const char* name;                                                     // 0x00
    unsigned long hash;                                                   // 0x04: 0 until first used
    int unk8[3];                                                          // 0x08
    void* (*create)(int a, int id);                                       // 0x14
    void (*destroy)(EffectObj* obj);                                      // 0x18
    int (*msg)(EffectObj* obj, int msg, int arg, void* data, float dt);   // 0x1C
    unsigned long (*query)(EffectObj* obj, int msg);                      // 0x20
    void (*update)(EffectObj* obj, float dt);                             // 0x24
    void (*reset)(EffectObj* obj);                                        // 0x28
    int unk2C;                                                            // 0x2C
};

#endif
