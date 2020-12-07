#ifndef CF_IOSTREAM
#define CF_IOSTREAM
typedef struct cf_iostream cf_iostream;

#define CF_IOS_READ 0x01
#define CF_IOS_WRITE 0x02

#ifdef __cplusplus
extern "C"{
#endif

typedef struct cf_iostream_vt{
    // 返回写入字节数, 负数表示异常
    int (*writeln)( cf_iostream* stream,const char* str);
    void (*close)(cf_iostream* stream);
    void (*destroy)(cf_iostream* stream);
}cf_iostream_vt;


typedef struct cf_iostream {
    const cf_iostream_vt* m_vt;
}cf_iostream;


cf_iostream* cf_iostream_from_std_out();
cf_iostream* cf_iostream_from_std_err();
int cf_iostream_writeln( cf_iostream* stream,const char* str);
void cf_iostream_close(cf_iostream* stream);
void cf_iostream_destroy(cf_iostream* stream);

#ifdef __cplusplus
}
#endif

#endif//CF_IOSTREAM