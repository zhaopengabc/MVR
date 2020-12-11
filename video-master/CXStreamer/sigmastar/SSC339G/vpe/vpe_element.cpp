#include <string.h>
#include "../../../factory.h"
#include "../hielement.h"
#include "zstd.h"
#include "st_common.h"
#include "st_vif.h"
#include "st_vpe.h"
#include "st_venc.h"


namespace CXS{
    #define ELEMENT_CLASS_NAME  "vpe"
    #define MAX_VPE_CHN 1

    class Vpe : public HiElement
    {
        struct ST_Stream_Attr_T
        {
            MI_VIF_DEV s32vifDev;
            ST_Sys_Input_E enInput;
            MI_U32     u32InputChn;
            MI_U32     u32InputPort;
            MI_VENC_CHN vencChn;
            MI_VENC_ModType_e eType;
            MI_U32    u32Width;
            MI_U32     u32Height;
            MI_VPE_SensorChannel_e eBindSensorId;
            MI_SYS_PixelFormat_e pixelFormat;
        };
       struct ST_Stream_Attr_T gVpeAttr[MAX_VPE_CHN];

        Vpe():HiElement(ELEMENT_CLASS_NAME)
        {
            setAttr("SECOND_VPE_CHNID","2");
        }
    public:
        static Element* createInstance()
        {
            Vpe* elem = new Vpe();
            return elem;
        }
        int startSelf()
        {
            int ret = 0;
            MI_VIF_DEV vifDev;
            MI_VPE_CHANNEL vpechn;
            MI_VENC_CHN VencChn;
            /************************************************
            Step3:  init VPE (create one VPE)
            *************************************************/
            ST_VPE_ChannelInfo_T stVpeChannelInfo;
            ST_VPE_PortInfo_T stVpePortInfo;

            MI_SYS_PixelFormat_e pixelFormat;

            gVpeAttr[0].s32vifDev = 0;
            gVpeAttr[0].enInput = ST_Sys_Input_VPE;
            gVpeAttr[0].u32InputChn = 0;
            gVpeAttr[0].u32InputPort = 0;
            gVpeAttr[0].vencChn = 0;
            gVpeAttr[0].eType = E_MI_VENC_MODTYPE_H264E;
            gVpeAttr[0].u32Width = 1920;
            gVpeAttr[0].u32Height = 1080;
            gVpeAttr[0].eBindSensorId = E_MI_VPE_SENSOR0;
            gVpeAttr[0].pixelFormat = (MI_SYS_PixelFormat_e)35;


            gVpeAttr[1].s32vifDev = 2;
            gVpeAttr[1].enInput =ST_Sys_Input_VPE;
            gVpeAttr[1].u32InputChn = 2;
            gVpeAttr[1].u32InputPort = 0;
            gVpeAttr[1].vencChn = 2;
            gVpeAttr[1].eType = E_MI_VENC_MODTYPE_H264E;
            gVpeAttr[1].u32Width = 1280;
            gVpeAttr[1].u32Height = 720;
            // gVpeAttr[1].u32Width = 3840;
            // gVpeAttr[1].u32Height = 2160;
            gVpeAttr[1].eBindSensorId = E_MI_VPE_SENSOR1;
            gVpeAttr[1].pixelFormat = (MI_SYS_PixelFormat_e)44;

            for(int i=0;i<MAX_VPE_CHN;i++)
            {
                vpechn = gVpeAttr[i].s32vifDev;
                pixelFormat = gVpeAttr[i].pixelFormat;
                
                memset(&stVpeChannelInfo, 0, sizeof(ST_VPE_ChannelInfo_T));
                // stVpeChannelInfo.u16VpeMaxW = 1920;// this data get frome camera ????
                // stVpeChannelInfo.u16VpeMaxH = 1080;// this data get frome camera ????
                stVpeChannelInfo.u16VpeMaxW = 3840;// this data get frome camera ????
                stVpeChannelInfo.u16VpeMaxH = 2160;// this data get frome camera ????
                stVpeChannelInfo.u32X = 0;
                stVpeChannelInfo.u32Y = 0;
                stVpeChannelInfo.u16VpeCropW = 0;
                stVpeChannelInfo.u16VpeCropH = 0;
                stVpeChannelInfo.eRunningMode = E_MI_VPE_RUN_CAM_MODE;
                stVpeChannelInfo.eFormat = pixelFormat;
                stVpeChannelInfo.eHDRtype = E_MI_VPE_HDR_TYPE_OFF;
                stVpeChannelInfo.eBindSensorId = gVpeAttr[i].eBindSensorId;

                ST_Vpe_CreateChannel(vpechn, &stVpeChannelInfo);
                ST_Vpe_StartChannel(vpechn);

                 memset(&stVpePortInfo, 0, sizeof(ST_VPE_PortInfo_T));

                stVpePortInfo.DepVpeChannel = vpechn;
                stVpePortInfo.u16OutputWidth = gVpeAttr[i].u32Width;
                stVpePortInfo.u16OutputHeight = gVpeAttr[i].u32Height;
                stVpePortInfo.ePixelFormat = E_MI_SYS_PIXEL_FRAME_YUV_SEMIPLANAR_420;
                stVpePortInfo.eCompressMode = E_MI_SYS_COMPRESS_MODE_NONE;
                STCHECKRESULT(ST_Vpe_StartPort(0 , &stVpePortInfo));

                printf("\n\n\n\n ========VPE init ============\n");
                printf("vpechn : %d \n",vpechn);
                printf("stVpeChannelInfo.u16VpeMaxW : %d \n",stVpeChannelInfo.u16VpeMaxW);
                printf("stVpeChannelInfo.u16VpeMaxH : %d \n",stVpeChannelInfo.u16VpeMaxH);
                printf("stVpeChannelInfo.u32X : %d \n",stVpeChannelInfo.u32X);
                printf("stVpeChannelInfo.u32Y : %d \n",stVpeChannelInfo.u32Y);
                printf("stVpeChannelInfo.u16VpeCropW : %d \n",stVpeChannelInfo.u16VpeCropW);
                printf("stVpeChannelInfo.u16VpeCropH : %d \n",stVpeChannelInfo.u16VpeCropH);
                printf("stVpeChannelInfo.eRunningMode : %d \n",stVpeChannelInfo.eRunningMode);
                printf("stVpeChannelInfo.eFormat : %d \n",stVpeChannelInfo.eFormat);
                printf("stVpeChannelInfo.eHDRtype : %d \n",stVpeChannelInfo.eHDRtype);
                printf("stVpeChannelInfo.eBindSensorId : %d \n",stVpeChannelInfo.eBindSensorId);
            }
            return ret;
        }
        int linkTo(Element* elem)
        {
            HiElement* hiElem = dynamic_cast<HiElement*>(elem);
            MI_S32 ret = 0;
            ST_Sys_BindInfo_T stBindInfo;
            MI_U32 u32VencDevId = 0xff;
            MI_VIF_DEV vifDev;
            MI_VPE_CHANNEL vpechn;
            MI_VENC_CHN VencChn;
            
            if(strcmp(hiElem->getClassName(),"venc") == 0)
            {
                for(int i = 0;i<MAX_VPE_CHN;i++)
                {
                    vifDev = gVpeAttr[i].s32vifDev;
                    vpechn = vifDev;
                    VencChn = gVpeAttr[i].vencChn;

                    ExecFunc(MI_VENC_GetChnDevid(VencChn, &u32VencDevId), MI_SUCCESS);
                    memset(&stBindInfo, 0x0, sizeof(ST_Sys_BindInfo_T));
                    stBindInfo.stSrcChnPort.eModId = E_MI_MODULE_ID_VPE;
                    stBindInfo.stSrcChnPort.u32DevId = 0;
                    stBindInfo.stSrcChnPort.u32ChnId = vpechn;
                    stBindInfo.stSrcChnPort.u32PortId = 0;

                    stBindInfo.stDstChnPort.eModId = E_MI_MODULE_ID_VENC;
                    stBindInfo.stDstChnPort.u32DevId = u32VencDevId;
                    stBindInfo.stDstChnPort.u32ChnId = VencChn;
                    stBindInfo.stDstChnPort.u32PortId = 0;

                    stBindInfo.u32SrcFrmrate = 30;
                    stBindInfo.u32DstFrmrate = 30;
                    stBindInfo.eBindType = E_MI_SYS_BIND_TYPE_FRAME_BASE;
                    ret = ST_Sys_Bind(&stBindInfo);

                    printf("\n\n\n\n\n ================== VPE bind VENC ===============");
                    printf("stBindInfo.stSrcChnPort.eModId : %d \n",stBindInfo.stSrcChnPort.eModId);
                    printf("stBindInfo.stSrcChnPort.u32DevId : %d \n",stBindInfo.stSrcChnPort.u32DevId);
                    printf("stBindInfo.stSrcChnPort.u32ChnId : %d \n",stBindInfo.stSrcChnPort.u32ChnId);
                    printf("stBindInfo.stSrcChnPort.u32PortId : %d \n",stBindInfo.stSrcChnPort.u32PortId);
                    printf("stBindInfo.stDstChnPort.eModId : %d \n",stBindInfo.stDstChnPort.eModId);
                    printf("stBindInfo.stDstChnPort.u32DevId : %d \n",stBindInfo.stDstChnPort.u32DevId);
                    printf("stBindInfo.stDstChnPort.u32ChnId : %d \n",stBindInfo.stDstChnPort.u32ChnId);
                    printf("stBindInfo.stDstChnPort.u32PortId : %d \n",stBindInfo.stDstChnPort.u32PortId);
                    printf("stBindInfo.u32SrcFrmrate : %d \n",stBindInfo.u32SrcFrmrate);
                    printf("stBindInfo.u32DstFrmrate : %d \n",stBindInfo.u32DstFrmrate);
                    printf("stBindInfo.eBindType : %d \n",stBindInfo.eBindType);
                }                
            }
            return ret;
        }
    };
    static int init()
    {
        return MI_SUCCESS;
    }
    static void deInit()
    {

    }
    static ElementDescriptor elementDescriptors[] = {
        {
             "SSC339G",
             ELEMENT_CLASS_NAME,
             init,
             deInit,
             Vpe::createInstance
        },
    };
    REGISTER_ELEMENT(elementDescriptors,sizeof(elementDescriptors)/sizeof(elementDescriptors[0]));
}
