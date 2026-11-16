// memory.cpp: Open Dynamics Engine (ODE) 0.5 "ode/src/memory.cpp" (0x80277190-0x80277220).
// ODE is Copyright (C) 2001-2003 Russell L. Smith, used under its BSD license.
// Written as ODE wrote it. Built with -inline deferred (reverse source order); the dGet*Handler
// functions and dRealloc are removed by the linker.

typedef unsigned long size_t;

extern "C" {
void* malloc(size_t size);
void* realloc(void* ptr, size_t size);
void free(void* ptr);
}

typedef void* dAllocFunction(size_t size);
typedef void* dReallocFunction(void* ptr, size_t oldsize, size_t newsize);
typedef void dFreeFunction(void* ptr, size_t size);

static dAllocFunction* allocfn = 0;
static dReallocFunction* reallocfn = 0;
static dFreeFunction* freefn = 0;

void dSetAllocHandler(dAllocFunction* fn)
{
    allocfn = fn;
}

void dSetReallocHandler(dReallocFunction* fn)
{
    reallocfn = fn;
}

void dSetFreeHandler(dFreeFunction* fn)
{
    freefn = fn;
}

dAllocFunction* dGetAllocHandler()
{
    return allocfn;
}

dReallocFunction* dGetReallocHandler()
{
    return reallocfn;
}

dFreeFunction* dGetFreeHandler()
{
    return freefn;
}

void* dAlloc(size_t size)
{
    if (allocfn) return allocfn(size);
    else return malloc(size);
}

void* dRealloc(void* ptr, size_t oldsize, size_t newsize)
{
    if (reallocfn) return reallocfn(ptr, oldsize, newsize);
    else return realloc(ptr, newsize);
}

void dFree(void* ptr, size_t size)
{
    if (!ptr) return;
    if (freefn) freefn(ptr, size);
    else free(ptr);
}
