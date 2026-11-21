// u_80294548.cpp (0x80294548-0x80299F0C): the DemonCarrier host actor ("CarrierDemon_flame",
// "DemonCarrier_ShootSpikes", ..., bones BONE_RIGHTREARLEG01 ...) and its behaviour table. The old u_80294548
// range (0x80294548-0x8029C0AC) held four files; the other three are u_80299F0C.cpp (SpitAndSqueal effect),
// u_8029A5E4.cpp and u_8029B070.cpp. Boundary evidence for the end at 0x80299F0C:
// - .sdata2: this file's pool is 0x804DA650-0x804DA720 (0.0f at 0x804DA658, int-to-float double 0x804DA688);
//   the next file has its own 0.0f at 0x804DA720. fn_80299994..fn_80299E60 (the last functions here) read the behaviour table
//   0x803C3050, whose entries are this file's fn_802997DC..fn_8029994C.
// - .rodata: string pool 0x8034B688-0x8034B7C0 starting with the ghost-action name table (POSSESS..SCAN); the
//   next file's pool starts with its own copy of the table at 0x8034B7C0.
// - .data 0x803C2C08-0x803C30B0, .sdata 0x804D3810-0x804D3848, .sbss 0x804D5708-0x804D5718 and .bss
//   0x804A44F8-0x804A4598 are referenced only from this range.
// No duplicated constant inside the range, so it stays one file.
// Names are descriptive guesses; unmatched against retail unless noted.

#include <dolphin/types.h>

// TODO (not drafted yet, 0x1C bytes at 0x80294548)
extern "C" void fn_80294548();
// TODO (not drafted yet, 0x758 bytes at 0x80294564)
extern "C" void fn_80294564();
// TODO (not drafted yet, 0x3D8 bytes at 0x80294CBC)
extern "C" void fn_80294CBC();
// TODO (not drafted yet, 0x8 bytes at 0x80295094)
extern "C" void fn_80295094();
// TODO (not drafted yet, 0x2FC bytes at 0x8029509C)
extern "C" void fn_8029509C();
// TODO (not drafted yet, 0x29C bytes at 0x80295398)
extern "C" void fn_80295398();
// TODO (not drafted yet, 0x4E8 bytes at 0x80295634)
extern "C" void fn_80295634();
// TODO (not drafted yet, 0x198 bytes at 0x80295B1C)
extern "C" void fn_80295B1C();
// TODO (not drafted yet, 0x4B4 bytes at 0x80295CB4)
extern "C" void fn_80295CB4();
// TODO (not drafted yet, 0x194 bytes at 0x80296168)
extern "C" void fn_80296168();
// TODO (not drafted yet, 0x14C bytes at 0x802962FC)
extern "C" void fn_802962FC();
// TODO (not drafted yet, 0x328 bytes at 0x80296448)
extern "C" void fn_80296448();
// TODO (not drafted yet, 0x6C bytes at 0x80296770)
extern "C" void fn_80296770();
// TODO (not drafted yet, 0x98 bytes at 0x802967DC)
extern "C" void fn_802967DC();
// TODO (not drafted yet, 0x7C bytes at 0x80296874)
extern "C" void fn_80296874();
// TODO (not drafted yet, 0x24 bytes at 0x802968F0)
extern "C" void fn_802968F0();
// TODO (not drafted yet, 0x40 bytes at 0x80296914)
extern "C" void fn_80296914();
// TODO (not drafted yet, 0x54 bytes at 0x80296954)
extern "C" void fn_80296954();
// TODO (not drafted yet, 0x68 bytes at 0x802969A8)
extern "C" void fn_802969A8();
// TODO (not drafted yet, 0x68 bytes at 0x80296A10)
extern "C" void fn_80296A10();
// TODO (not drafted yet, 0x13C bytes at 0x80296A78)
extern "C" void fn_80296A78();
// TODO (not drafted yet, 0x10C bytes at 0x80296BB4)
extern "C" void fn_80296BB4();
// TODO (not drafted yet, 0x84 bytes at 0x80296CC0)
extern "C" void fn_80296CC0();
// TODO (not drafted yet, 0xB8 bytes at 0x80296D44)
extern "C" void fn_80296D44();
// TODO (not drafted yet, 0xB8 bytes at 0x80296DFC)
extern "C" void fn_80296DFC();
// TODO (not drafted yet, 0x40 bytes at 0x80296EB4)
extern "C" void fn_80296EB4();
// TODO (not drafted yet, 0x174 bytes at 0x80296EF4)
extern "C" void fn_80296EF4();
// TODO (not drafted yet, 0x1A0 bytes at 0x80297068)
extern "C" void fn_80297068();
// TODO (not drafted yet, 0x104 bytes at 0x80297208)
extern "C" void fn_80297208();
// TODO (not drafted yet, 0xD4 bytes at 0x8029730C)
extern "C" void fn_8029730C();
// TODO (not drafted yet, 0x9C bytes at 0x802973E0)
extern "C" void fn_802973E0();
// TODO (not drafted yet, 0x98 bytes at 0x8029747C)
extern "C" void fn_8029747C();
// TODO (not drafted yet, 0x94 bytes at 0x80297514)
extern "C" void fn_80297514();
// TODO (not drafted yet, 0x70 bytes at 0x802975A8)
extern "C" void fn_802975A8();
// TODO (not drafted yet, 0x5C bytes at 0x80297618)
extern "C" void fn_80297618();
// TODO (not drafted yet, 0x5C bytes at 0x80297674)
extern "C" void fn_80297674();
// TODO (not drafted yet, 0x58 bytes at 0x802976D0)
extern "C" void fn_802976D0();
// TODO (not drafted yet, 0x80 bytes at 0x80297728)
extern "C" void fn_80297728();
// TODO (not drafted yet, 0x58 bytes at 0x802977A8)
extern "C" void fn_802977A8();
// TODO (not drafted yet, 0x58 bytes at 0x80297800)
extern "C" void fn_80297800();
// TODO (not drafted yet, 0x58 bytes at 0x80297858)
extern "C" void fn_80297858();
// TODO (not drafted yet, 0x58 bytes at 0x802978B0)
extern "C" void fn_802978B0();
// TODO (not drafted yet, 0x78 bytes at 0x80297908)
extern "C" void fn_80297908();
// TODO (not drafted yet, 0x60 bytes at 0x80297980)
extern "C" void fn_80297980();
// TODO (not drafted yet, 0x60 bytes at 0x802979E0)
extern "C" void fn_802979E0();
// TODO (not drafted yet, 0x5C bytes at 0x80297A40)
extern "C" void fn_80297A40();
// TODO (not drafted yet, 0x60 bytes at 0x80297A9C)
extern "C" void fn_80297A9C();
// TODO (not drafted yet, 0x58 bytes at 0x80297AFC)
extern "C" void fn_80297AFC();
// TODO (not drafted yet, 0xA0 bytes at 0x80297B54)
extern "C" void fn_80297B54();
// TODO (not drafted yet, 0xE8 bytes at 0x80297BF4)
extern "C" void fn_80297BF4();
// TODO (not drafted yet, 0x874 bytes at 0x80297CDC)
extern "C" void fn_80297CDC();
// TODO (not drafted yet, 0x1C8 bytes at 0x80298550)
extern "C" void fn_80298550();
// TODO (not drafted yet, 0x348 bytes at 0x80298718)
extern "C" void fn_80298718();
// TODO (not drafted yet, 0xB28 bytes at 0x80298A60)
extern "C" void fn_80298A60();
// TODO (not drafted yet, 0x228 bytes at 0x80299588)
extern "C" void fn_80299588();
// TODO (not drafted yet, 0x2C bytes at 0x802997B0)
extern "C" void fn_802997B0();
// TODO (not drafted yet, 0x4C bytes at 0x802997DC)
extern "C" void fn_802997DC();
// TODO (not drafted yet, 0x4C bytes at 0x80299828)
extern "C" void fn_80299828();
// TODO (not drafted yet, 0x4C bytes at 0x80299874)
extern "C" void fn_80299874();
// TODO (not drafted yet, 0x4 bytes at 0x802998C0)
extern "C" void fn_802998C0();
// TODO (not drafted yet, 0x4 bytes at 0x802998C4)
extern "C" void fn_802998C4();
// TODO (not drafted yet, 0x4 bytes at 0x802998C8)
extern "C" void fn_802998C8();
// TODO (not drafted yet, 0x40 bytes at 0x802998CC)
extern "C" void fn_802998CC();
// TODO (not drafted yet, 0x40 bytes at 0x8029990C)
extern "C" void fn_8029990C();
// TODO (not drafted yet, 0x40 bytes at 0x8029994C)
extern "C" void fn_8029994C();
// TODO (not drafted yet, 0x8 bytes at 0x8029998C)
extern "C" void fn_8029998C();
// TODO (not drafted yet, 0x3BC bytes at 0x80299994)
extern "C" void fn_80299994();
// TODO (not drafted yet, 0xD8 bytes at 0x80299D50)
extern "C" void fn_80299D50();
// TODO (not drafted yet, 0x38 bytes at 0x80299E28)
extern "C" void fn_80299E28();
// TODO (not drafted yet, 0xAC bytes at 0x80299E60)
extern "C" void fn_80299E60();
