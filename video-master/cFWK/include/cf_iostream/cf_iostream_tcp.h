#ifndef CF_IOSTREAM_TCP_H
#define CF_IOSTREAM_TCP_H
#include <stdint.h>
#include "cf_iostream/cf_iostream.h"

#ifdef __cplusplus
extern "C"{
#endif

cf_iostream* cf_iostream_tcp_connect(const char* ipaddr,uint16_t port);

#ifdef __cplusplus
}
#endif

#endif//CF_IOSTREAM_TCP_H