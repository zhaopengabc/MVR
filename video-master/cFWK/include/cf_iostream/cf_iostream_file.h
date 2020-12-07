#ifndef CF_IOSTREAM_FILE_H
#define CF_IOSTREAM_FILE_H
#include <stdint.h>
#ifdef __cplusplus
extern "C"{
#endif
#include "cf_iostream.h"

//flags 可选项: CF_IOS_READ |CF_IOS_WRITE
cf_iostream* cf_iostream_from_file(const char* file_name,uint32_t flags);

#ifdef __cplusplus
}
#endif

#endif//CF_IOSTREAM_FILE_H