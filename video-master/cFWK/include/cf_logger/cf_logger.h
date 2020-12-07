#ifndef CF_LOGGER_H
#define CF_LOGGER_H
#include "cf_iostream/cf_iostream.h"

#ifdef __cplusplus
extern "C"{
#endif
typedef enum cf_log_level{
    CF_LOG_QUIET,
    CF_LOG_ERROR,
    CF_LOG_DEBUG,
    CF_LOG_WARN,
    CF_LOG_INFO
}cf_log_level;
typedef struct cf_logger cf_logger;
//extern struct cf_logger* cf_logger_root();
//logger == NULL 则操作全局logger对象
void cf_log( cf_logger* logger,cf_log_level level,const char* format,...);
void cf_log_add_out( cf_logger* logger,cf_iostream* out);
//返回旧的level
cf_log_level cf_log_set_level( cf_logger* logger,cf_log_level level);
//会自动释放out资源
void cf_log_remove_out( cf_logger* logger,cf_iostream* out);

#ifdef __cplusplus
}
#endif

#endif