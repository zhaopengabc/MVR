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
        

        return 0;
        // return 0 == s32Ret ? 0:-1;
    }
    static void deInit()
    {
        // MI_SNR_PAD_ID_e eSnrPadId = E_MI_SNR_PAD_ID_0;
        // MI_VIF_DEV vifDev = 0;
        // MI_VIF_CHN vifChn = 0;
        // MI_VENC_CHN VencChn = 0;
        // MI_U32 u32VencDevId = 0xff;
        // MI_VPE_CHANNEL vpechn = 0;
        // ST_Sys_BindInfo_T stBindInfo;
        // MI_U32 arraySize = ARRAY_SIZE(g_stStreamAttr);
        // ST_Stream_Attr_T *pstStreamAttr = g_stStreamAttr;
        // MI_U8 i=0;

        // for(i=0; i< arraySize; i++)
        // {
        //     printf("i %d, stream input chn %d, vpe chn %d \n", i, pstStreamAttr[i].u32InputChn, vpechn);
        //     if(pstStreamAttr[i].u32InputChn == vpechn)
        //     {
        //         VencChn = pstStreamAttr[i].vencChn;
        //         MI_VENC_GetChnDevid(VencChn, &u32VencDevId);

        //         memset(&stBindInfo, 0x0, sizeof(ST_Sys_BindInfo_T));
        //         stBindInfo.stSrcChnPort.eModId = E_MI_MODULE_ID_VPE;
        //         stBindInfo.stSrcChnPort.u32DevId = vifDev;
        //         stBindInfo.stSrcChnPort.u32ChnId = vpechn;
        //         stBindInfo.stSrcChnPort.u32PortId = pstStreamAttr[i].u32InputPort ;

        //         stBindInfo.stDstChnPort.eModId = E_MI_MODULE_ID_VENC;
        //         stBindInfo.stDstChnPort.u32DevId = u32VencDevId;
        //         stBindInfo.stDstChnPort.u32ChnId = VencChn;
        //         stBindInfo.stDstChnPort.u32PortId = 0;

        //         stBindInfo.u32SrcFrmrate = 30;
        //         stBindInfo.u32DstFrmrate = 30;
        //         (ST_Sys_UnBind(&stBindInfo));

        //         (ST_Vpe_StopPort(vpechn, pstStreamAttr[i].u32InputPort));

        //         (ST_Venc_StopChannel(VencChn));
        //         (ST_Venc_DestoryChannel(VencChn));
        //     }
        // }

        // memset(&stBindInfo, 0x0, sizeof(ST_Sys_BindInfo_T));
        // stBindInfo.stSrcChnPort.eModId = E_MI_MODULE_ID_VIF;
        // stBindInfo.stSrcChnPort.u32DevId = vifDev;
        // stBindInfo.stSrcChnPort.u32ChnId = vifChn;
        // stBindInfo.stSrcChnPort.u32PortId = 0 ;

        // stBindInfo.stDstChnPort.eModId = E_MI_MODULE_ID_VPE;
        // stBindInfo.stDstChnPort.u32DevId = vifDev;
        // stBindInfo.stDstChnPort.u32ChnId = vpechn;
        // stBindInfo.stDstChnPort.u32PortId = 0;

        // stBindInfo.u32SrcFrmrate = 30;
        // stBindInfo.u32DstFrmrate = 30;
        // (ST_Sys_UnBind(&stBindInfo));

        /************************************************
        // Step2:  destory VPE
        // *************************************************/
        // (ST_Vpe_StopChannel(vpechn));
        // (ST_Vpe_DestroyChannel(vpechn));

        // /************************************************
        // Step3:  destory VIF
        // *************************************************/
        // (ST_Vif_StopPort(vifChn, 0));
        // (ST_Vif_DisableDev(vifDev));

        // MI_SNR_Disable(eSnrPadId);

        /************************************************
        Step4:  destory SYS
        *************************************************/
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
