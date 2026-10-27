u_80221100 (sonnet-1 trial):
- GetTimeMsg/SetTimeMsg/BeesInit/BeesDestroy/__dt__7BeeList: matched (single-case switch; BeeBase/BeeList classes with inline dtors; operator new/delete on BeesState; void* state then typed local after null check).
- fn_80221100 (speed scale): 42 diffs, clamp shape (retail: bge+b, lo/hi loaded lazily) not found; tried ternary, if/else, >= forms.
- BeesSpawn: 23 diffs, locals td/sd sit 4 bytes off (retail td at 88, sd at 144), string pool offset (SPIN at +94, ours +71), fb hash/flag regs.
- BeesUpdate: drafted only (1110 diffs, size 0x1168 vs 0x115C); not worked on.
