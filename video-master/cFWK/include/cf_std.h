#ifndef CF_STD_H
#define CF_STD_H
#define cf_unused(expr) do { (void)(expr); } while (0)
#ifndef NULL
#define NULL ((void*)0)
#endif
#include <stdint.h>
#include <assert.h>

#define cf_assert(cond) assert(cond)
void cf_usleep(uint64_t ms);

#define CF_OK   0
#define CF_FAILURE   -1

#endif//CF_STD_H