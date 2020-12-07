#ifndef CF_VECTOR_H
#define CF_VECTOR_H
#include <stdlib.h>
#include "cf_iterator.h"

#ifdef __cplusplus
extern "C"{
#endif

struct cf_vector;
struct cf_vector* cf_vector_create(size_t elem_size,size_t count);
void* cf_vector_buffer(struct cf_vector* vector);
size_t cf_vector_length(struct cf_vector* vector);
int cf_vector_resize(struct cf_vector* vector,size_t count);
int cf_vector_append(struct cf_vector* vector,void* array,size_t count);
void cf_vector_delete(struct cf_vector* vector);

struct cf_iterator cf_vector_begin(struct cf_vector* v);

#ifdef __cplusplus
}
#endif

#endif//CF_VECTOR_H