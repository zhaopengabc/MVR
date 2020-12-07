#ifndef CXLOG_H
#define CXLOG_H
#include <stdarg.h>
class CXLog
{
    CXLog();

    void to_log_str(const char*file,const char* func,int line,const char* format,char* output_str,int output_str_size,va_list args);
public:
    static CXLog* getDefaultInstance();
    void info(const char*file,const char* func,int line,const char* format,...);
    void dbg(const char*file,const char* func,int line,const char* format,...);

};
#define CXLOG_INFO(format,args...)  CXLog::getDefaultInstance()->info(__FILE__,__func__,__LINE__,format, #args)
#define CXLOG_DBG(format,args...)  CXLog::getDefaultInstance()->dbg(__FILE__,__func__,__LINE__,format, #args)
#endif // CXLOG_H
