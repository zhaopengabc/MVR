#ifndef CF_LIST_H
#define CF_LIST_H

#include <stdbool.h>
#include <stddef.h>
#include "cf_iterator.h"

#ifdef __cplusplus
extern "C"{
#endif
typedef struct cf_list cf_list;
typedef void(*cf_list_free_f)(void*);
cf_list* cf_list_create(cf_list_free_f );
void cf_list_clear(cf_list* list);
void cf_list_destroy(cf_list* list);
bool cf_list_is_empty(const cf_list*);
void cf_list_push(cf_list*,void*);
void cf_list_remove_item(cf_list*,void*);
void* cf_list_take_front(cf_list*);
size_t cf_list_length(const cf_list*);
cf_iterator cf_list_begin(cf_list* list);

#ifdef __cplusplus
}
#endif

#endif//CF_LIST_H