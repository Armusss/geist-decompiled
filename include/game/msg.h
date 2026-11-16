#ifndef GAME_MSG_H
#define GAME_MSG_H

#include "game/string.h"

// Message payloads passed to actor message handlers (`handler(msg, data)` at actor +0x30, and the
// four-argument type handlers `msg(actor, msg, data, arg)`).
// Names are descriptive guesses; numbered structs are named after their message id.
//
// Every payload starts with the same three words: the sender (or 0), a key/hash, and a spare word. The
// reply starts at +0x0C. Evidence for the sizes is the stack frame of the Matching senders: the query
// payload with a String reply is 0x20 bytes (0x0C + sizeof(String) 0x14), and the plain reply block of
// message 0x400 is also 0x20 bytes (docs/matching-tricks.md, HomingMsg).

struct Actor;

// Reply of a name query: wrapping the String in a struct gives the double null check retail has in the
// inlined destructor (docs/matching-tricks.md, helper-game-b).
struct QueryResult {
    String name;                               // 0x00
};

// Name/hash query (0x20 bytes). 40 units declared this layout, 25 of them Matching.
struct QueryMsg {
    int unk0;                                  // 0x00
    unsigned long hash;                        // 0x04
    int unk8;                                  // 0x08
    QueryResult result;                        // 0x0C
};

// Plain 0x20-byte reply block: a pointer result followed by words the senders do not read.
struct MsgResult {
    void* p;                                   // 0x00
    char pad[0x1C];                            // 0x04
};

// Message 0x400: state query sent by behaviours (0x20 bytes).
struct Msg400 {
    Actor* actor;                              // 0x00
    int ownerId;                               // 0x04
    int unk08;                                 // 0x08
    int state;                                 // 0x0C
    float f10;                                 // 0x10
    float f14;                                 // 0x14
    int flags;                                 // 0x18
    int unk1C;                                 // 0x1C
};

// Message 0x409: play an animation.
struct Msg409 {
    Actor* actor;                              // 0x00
    int unk04;                                 // 0x04
    int anim;                                  // 0x08
    int arg;                                   // 0x0C
};

// Message 0x40A: animation query.
struct Msg40A {
    Actor* actor;                              // 0x00
    int unk04;                                 // 0x04
    unsigned long anim;                        // 0x08
    int result;                                // 0x0C
};

// Message 0x41D: keyed command with an integer argument.
struct Msg41D {
    Actor* actor;                              // 0x00
    unsigned long cmd;                         // 0x04
    int unk08;                                 // 0x08
    int arg;                                   // 0x0C
};

// Message 0x41E: keyed query with a text reply (0x20 bytes).
struct Msg41E {
    Actor* actor;                              // 0x00
    unsigned long cmd;                         // 0x04
    int unk08;                                 // 0x08
    Buffer result;                             // 0x0C
};

#endif
