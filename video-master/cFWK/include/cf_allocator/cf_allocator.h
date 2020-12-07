#include <stddef.h>

struct cf_allocator;
void* cf_allocator_alloc(struct cf_allocator* alloctor,size_t size);
void cf_allocator_free(struct cf_allocator* alloctor,void* mem);