#ifndef CF_ITERATOR_H
#define CF_ITERATOR_H
#include <stdbool.h>

#ifdef __cplusplus
extern "C"{
#endif

typedef struct cf_iterator cf_iterator;
struct cf_iterator_vt
{
    bool(*is_end)(struct cf_iterator*);
    void (*next)(struct cf_iterator*);
    void* (*get)(struct cf_iterator*);
    void (*remove)(struct cf_iterator*);
};

struct cf_iterator {
    const struct cf_iterator_vt* m_vt;
    void* m_container;
    void* m_priv;
};

bool cf_iterator_is_end(struct cf_iterator* iter);
void cf_iterator_next(struct cf_iterator*);
void* cf_iterator_get(struct cf_iterator*);
void cf_iterator_remove(struct cf_iterator* iter);

#ifdef __cplusplus
}
#endif

#endif//CF_ITERATOR_H