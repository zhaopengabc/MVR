#include <string.h>
#include "../../../factory.h"

// #include "../hi_comm_cfg.h"

#include "st_common.h"
#include "st_vif.h"
#include "st_vpe.h"
#include "st_venc.h"

namespace CXS{
    // struct ST_Stream_Attr_T
    // {
    //     ST_Sys_Input_E enInput;
    //     MI_U32     u32InputChn;
    //     MI_U32     u32InputPort;
    //     MI_VENC_CHN vencChn;
    //     MI_VENC_ModType_e eType;
    //     MI_U32    u32Width;
    //     MI_U32     u32Height;
    //     MI_U32 enFunc;
    //     const char    *pszStreamName;
    // };
    // typedef struct
    // {
    //     MI_VENC_CHN vencChn;
    //     MI_VENC_ModType_e enType;
    //     char szStreamName[64];

    //     MI_BOOL bWriteFile;
    //     int fd;
    //     char szDebugFile[128];
    // } ST_StreamInfo_T;

    // static struct ST_Stream_Attr_T g_stStreamAttr[6];

    // void ST_Flush(void)
    // {
    //     char c;

    //     while((c = getchar()) != '\n' && c != EOF);
    // }


    class SSC339GFactory : public Factory
    {
        SSC339GFactory()
        {

        }
        ~SSC339GFactory()
        {

        }

    public:
        DECLARE_FACTORY_GET_INSTANCE(SSC339GFactory)

    };
    static int init()
    {
        printf("factory init ....... \n");
        // STCHECKRESULT(ST_Sys_Init());

        return 0;
        // return 0 == s32Ret ? 0:-1;
    }
    static void deInit()
    {
        (ST_Sys_Exit());
    }
    static struct FactoryDescriptor factoryDescriptor  = 
    {
        "SSC339G",
        init,
        deInit,
        SSC339GFactory::getInstance
    };

    REGISTER_FACTORY(&factoryDescriptor);

}
