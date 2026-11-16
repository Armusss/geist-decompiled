// obstack.cpp: Open Dynamics Engine (ODE) 0.5 "ode/src/obstack.cpp" (0x80277290-0x802775AC).
// ODE is Copyright (C) 2001-2003 Russell L. Smith, used under its BSD license.
// Written as ODE wrote it (with the dObStack / dBase declarations from obstack.h and common.h).
// Built with -inline deferred: the functions come out in reverse source order.

typedef unsigned long size_t;
typedef unsigned long intP;

#define dOBSTACK_ARENA_SIZE 8192
#define EFFICIENT_ALIGNMENT 16
#define dEFFICIENT_SIZE(x) ((((x)-1) | (EFFICIENT_ALIGNMENT - 1)) + 1)

void* dAlloc(size_t size);
void dFree(void* ptr, size_t size);
void dDebug(int num, const char* msg, ...);

class dBase {
public:
    void* operator new(size_t size) { return dAlloc(size); }
    void operator delete(void* ptr, size_t size) { dFree(ptr, size); }
    void* operator new[](size_t size) { return dAlloc(size); }
    void operator delete[](void* ptr, size_t size) { dFree(ptr, size); }
};

struct dObStack : public dBase {
    dObStack();
    ~dObStack();

    void* alloc(int num_bytes);
    void freeAll();
    void* rewind();
    void* next(int num_bytes);

private:
    struct Arena {
        Arena* next;  // next arena in linked list
        int used;     // total number of bytes used in this arena, counting
    };                // this header

    Arena* first;  // head of the arena linked list. 0 if no arenas yet
    Arena* last;   // arena where blocks are currently being allocated

    // used for iterator
    Arena* current_arena;
    int current_ofs;
};

#define MAX_ALLOC_SIZE ((size_t)(dOBSTACK_ARENA_SIZE - sizeof(Arena) - EFFICIENT_ALIGNMENT + 1))

// we use malloc and free directly here because
// we want to guarantee that the alignment is EFFICIENT_ALIGNMENT

#define ROUND_UP_OFFSET_TO_EFFICIENT_SIZE(arena, ofs) \
    ofs = (size_t)(dEFFICIENT_SIZE(((intP)(arena)) + ofs) - ((intP)(arena)));

dObStack::dObStack()
{
    first = 0;
    last = 0;
    current_arena = 0;
    current_ofs = 0;
}

dObStack::~dObStack()
{
    // free all arenas
    Arena *a, *nexta;
    a = first;
    while (a) {
        nexta = a->next;
        dFree(a, dOBSTACK_ARENA_SIZE);
        a = nexta;
    }
}

void* dObStack::alloc(int num_bytes)
{
    if ((size_t)num_bytes > MAX_ALLOC_SIZE) dDebug(0, "num_bytes too large");

    // allocate or move to a new arena if necessary
    if (!first) {
        // allocate the first arena if necessary
        first = last = (Arena*)dAlloc(dOBSTACK_ARENA_SIZE);
        first->next = 0;
        first->used = sizeof(Arena);
        ROUND_UP_OFFSET_TO_EFFICIENT_SIZE(first, first->used);
    } else {
        // we already have one or more arenas, see if a new arena must be used
        if ((last->used + num_bytes) > dOBSTACK_ARENA_SIZE) {
            if (!last->next) {
                last->next = (Arena*)dAlloc(dOBSTACK_ARENA_SIZE);
                last->next->next = 0;
            }
            last = last->next;
            last->used = sizeof(Arena);
            ROUND_UP_OFFSET_TO_EFFICIENT_SIZE(last, last->used);
        }
    }

    // allocate an area in the arena
    char* c = ((char*)last) + last->used;
    last->used += num_bytes;
    ROUND_UP_OFFSET_TO_EFFICIENT_SIZE(last, last->used);
    return c;
}

void dObStack::freeAll()
{
    last = first;
    if (first) {
        first->used = sizeof(Arena);
        ROUND_UP_OFFSET_TO_EFFICIENT_SIZE(first, first->used);
    }
}

void* dObStack::rewind()
{
    current_arena = first;
    current_ofs = sizeof(Arena);
    if (current_arena) {
        ROUND_UP_OFFSET_TO_EFFICIENT_SIZE(current_arena, current_ofs)
        return ((char*)current_arena) + current_ofs;
    } else
        return 0;
}

void* dObStack::next(int num_bytes)
{
    // this functions like alloc, except that no new storage is ever
    // allocated
    if (!current_arena) return 0;
    current_ofs += num_bytes;
    ROUND_UP_OFFSET_TO_EFFICIENT_SIZE(current_arena, current_ofs);
    if (current_ofs >= current_arena->used) {
        current_arena = current_arena->next;
        if (!current_arena) return 0;
        current_ofs = sizeof(Arena);
        ROUND_UP_OFFSET_TO_EFFICIENT_SIZE(current_arena, current_ofs);
    }
    return ((char*)current_arena) + current_ofs;
}
