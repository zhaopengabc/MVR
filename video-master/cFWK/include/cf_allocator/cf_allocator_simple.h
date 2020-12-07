#ifndef CF_ALLOCATOR_SIMPLE_H
#define CF_ALLOCATOR_SIMPLE_H
#include <stddef.h>

#ifdef __cplusplus
extern "C" {
#endif

void* cf_allocator_simple_alloc(size_t size);
void* cf_allocator_simple_clone(void* src,size_t size);
void* cf_allocator_simple_realloc(void* mem,size_t size);
void cf_allocator_simple_free(void* mem);
size_t cf_allocator_alloc_size();

#ifdef __cplusplus
}
#endif

#endif//CF_ALLOCATOR_SIMPLE_H