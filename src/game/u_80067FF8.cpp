// A one-function unit (0x80067FF8-0x80068004): clears the word at an actor's +0x2F0 state block.
// Names are descriptive guesses; the code is matched against the retail DOL.

void ActorStateClear(int* state) { *state = 0; }
