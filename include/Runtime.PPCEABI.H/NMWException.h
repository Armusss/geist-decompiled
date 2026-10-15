#ifndef NMWEXCEPTION_H
#define NMWEXCEPTION_H

#include "__ppc_eabi_linker.h"

#ifdef __cplusplus
extern "C" {
#endif

typedef short vbase_ctor_arg_type;
typedef char local_cond_type;

typedef struct CatchInfo {
    void* location;
    void* typeinfo;
    void* dtor;
    void* sublocation;
    long pointercopy;
    void* stacktop;
} CatchInfo;

typedef struct DestructorChain {
    struct DestructorChain* next;
    void* destructor;
    void* object;
} DestructorChain;

void __unregister_fragment(int fragmentID);
int __register_fragment(struct __eti_init_info* info, char* TOC);
void* __register_global_object(void* object, void* destructor, void* regmem);
void __destroy_global_chain(void);
void __end__catch(CatchInfo* catchinfo);
void __throw(char* throwtype, void* location, void* dtor);
void __unexpected(CatchInfo* catchinfo);

#ifdef __cplusplus
}

char __throw_catch_compare(const char* throwtype, const char* catchtype, long* offset_result);

namespace std {
void unexpected();
void terminate();
} // namespace std

using std::terminate;
using std::unexpected;
#endif

#endif /* NMWEXCEPTION_H */
