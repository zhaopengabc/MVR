#ifndef CF_HASH_H
#define CF_HASH_H

#include <stdlib.h>
#include <stdbool.h>

#ifdef __cplusplus
extern "C"{
#endif

typedef struct cf_hash cf_hash;
size_t cf_hash_str_hash(void* key);
bool cf_hash_str_equal(void* key1,void* key2);
struct cf_hash* cf_hash_create(size_t (*hash_func)(void*),bool (*key_equal_func)(void*,void*),void (*key_free_func)(void*),void (*value_free_func)(void*));
void cf_hash_delete(struct cf_hash* hash);
void cf_hash_insert(struct cf_hash* hash,void* key,void* value);
void* cf_hash_get(struct cf_hash* hash, void* key,int* error);

#ifdef __cplusplus
}
#endif
#endif//CF_HASH_H