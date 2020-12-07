#include "CXLog.h"
#include <stdio.h>
#include <stdarg.h>
#include <time.h>

#define LOG_STR_MAX_SIZE    4096

CXLog::CXLog()
{

}
void CXLog::to_log_str(const char*file,const char* func,int line,const char* format,char* output_str,int output_str_size,va_list args)
{
    char content[1024];
    time_t now;
    struct tm *tm_now;
    time(&now);
    tm_now = localtime(&now);
    vsnprintf(content,sizeof(content),format, args);
    snprintf(output_str,output_str_size,"%d-%02d-%02d %02d:%02d:%02d %s:%s(%d): %s \n", tm_now->tm_year+1900, tm_now->tm_mon+1, tm_now->tm_mday,
              tm_now->tm_hour, tm_now->tm_min, tm_now->tm_sec,
              file,func,line,content);
    return;
}
void CXLog::info(const char*file,const char* func,int line,const char* format,...)
{
    char output[LOG_STR_MAX_SIZE];
    va_list args;
    va_start(args,format);
    to_log_str(file,func,line,format,output,sizeof(output),args);
    va_end(args);

    printf(output);
    return;
}
void CXLog::dbg(const char*file,const char* func,int line,const char* format,...)
{
    char output[LOG_STR_MAX_SIZE];
    va_list args;
    va_start(args,format);
    to_log_str(file,func,line,format,output,sizeof(output),args);
    va_end(args);

    printf(output);
    return;
}
CXLog* CXLog::getDefaultInstance()
{
    static CXLog* instance = new CXLog;
    return instance;
}
