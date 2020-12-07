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


    class Vpe : public HiElement
    {
        Vpe():HiElement(ELEMENT_CLASS_NAME)
        {

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
            MI_VIF_DEV vifDev = 2;
            MI_VPE_CHANNEL vpechn = 2;
            MI_VENC_CHN VencChn = 2;
            /************************************************
            Step3:  init VPE (create one VPE)
            *************************************************/
            ST_VPE_ChannelInfo_T stVpeChannelInfo;

            printf("start vpe ========================\n");
            memset(&stVpeChannelInfo, 0, sizeof(ST_VPE_ChannelInfo_T));
            stVpeChannelInfo.u16VpeMaxW = 3840;
            stVpeChannelInfo.u16VpeMaxH = 2160;
            stVpeChannelInfo.u32X = 0;
            stVpeChannelInfo.u32Y = 0;
            stVpeChannelInfo.u16VpeCropW = 0;
            stVpeChannelInfo.u16VpeCropH = 0;
            stVpeChannelInfo.eRunningMode = E_MI_VPE_RUN_CAM_MODE;
            stVpeChannelInfo.eFormat = (MI_SYS_PixelFormat_e)35;
            stVpeChannelInfo.eHDRtype = E_MI_VPE_HDR_TYPE_OFF;

            stVpeChannelInfo.eBindSensorId = E_MI_VPE_SENSOR1;


            (ST_Vpe_CreateChannel(vpechn, &stVpeChannelInfo));
            (ST_Vpe_StartChannel(vpechn));

            printf("\n\n\n >>>>>>>>>>>>>>> VPE channel >>>>>>>>>>>>>>>>> \n");
            printf(" stVpeChannelInfo.u16VpeMaxH : %d \n",stVpeChannelInfo.u16VpeMaxH);
            printf("stVpeChannelInfo.u16VpeMaxW : %d \n",stVpeChannelInfo.u16VpeMaxW);
            printf("stVpeChannelInfo.eRunningMode : %d \n",stVpeChannelInfo.eRunningMode);
            printf("stVpeChannelInfo.eFormat : %d \n",stVpeChannelInfo.eFormat);
            printf("stVpeChannelInfo.eHDRtype : %d \n",stVpeChannelInfo.eHDRtype);
            printf("stVpeChannelInfo.eBindSensorId : %d\n",stVpeChannelInfo.eBindSensorId);


            ST_VPE_PortInfo_T stVpePortInfo;
            //MI_U16  u16VpePortWidth=1920, u16VpePortheight=1080;
            memset(&stVpePortInfo, 0, sizeof(ST_VPE_PortInfo_T));

            stVpePortInfo.DepVpeChannel = 0;
            stVpePortInfo.u16OutputWidth = 1920;
            stVpePortInfo.u16OutputHeight = 1080;
            stVpePortInfo.ePixelFormat = E_MI_SYS_PIXEL_FRAME_YUV_SEMIPLANAR_420;
            stVpePortInfo.eCompressMode = E_MI_SYS_COMPRESS_MODE_NONE;
            STCHECKRESULT(ST_Vpe_StartPort(vpechn , &stVpePortInfo));

            CXS_PRT("VPE(%d) start ok\n",mHandle);

            printf("\n\n\n >>>>>>>>>>>>>> VPE port info <<<<<<<<<<<<<<<<<<<<< \n");
            printf("stVpePortInfo.DepVpeChannel : %d \n",stVpePortInfo.DepVpeChannel);
            printf("stVpePortInfo.u16OutputWidth : %d \n",stVpePortInfo.u16OutputWidth);
            printf("stVpePortInfo.u16OutputHeight : %d \n",stVpePortInfo.u16OutputHeight);
            printf("stVpePortInfo.ePixelFormat : %d \n",stVpePortInfo.ePixelFormat);
            printf("stVpePortInfo.eCompressMode : %d \n",stVpePortInfo.eCompressMode);

            return ret;
        }
        int linkTo(Element* elem)
        {
            HiElement* hiElem = dynamic_cast<HiElement*>(elem);
            MI_S32 ret = 0;
            ST_Sys_BindInfo_T stBindInfo;
            MI_U32 u32VencDevId = 0xff;
            MI_VIF_DEV vifDev = 2;
            MI_VPE_CHANNEL vpechn = 2;
            MI_VENC_CHN VencChn = 2;

            if(strcmp(hiElem->getClassName(),"venc") == 0)
            {
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
                printf("ST_Sys_Bind ret : %d \n",ret);
                CXS_PRT("-------------------------vpe bind venc bind sucess \n");

                // printf("\n\n\n\n >>>>>>>>>>>>>> VENC -> VPE <<<<<<<<<<<<<<<<<<<<<<<<<< \n");
                // printf("stBindInfo.stSrcChnPort.eModId : %d \n",stBindInfo.stSrcChnPort.eModId);
                // printf("stBindInfo.stSrcChnPort.u32DevId : %d \n",stBindInfo.stSrcChnPort.u32DevId);
                // printf("stBindInfo.stSrcChnPort.u32ChnId : %d \n",stBindInfo.stSrcChnPort.u32ChnId);
                // printf("stBindInfo.stSrcChnPort.u32PortId : %d \n",stBindInfo.stSrcChnPort.u32PortId);
                // printf("stBindInfo.stDstChnPort.eModId : %d \n",stBindInfo.stDstChnPort.eModId);
                // printf("stBindInfo.stDstChnPort.u32DevId : %d \n",stBindInfo.stDstChnPort.u32DevId);
                // printf("stBindInfo.stDstChnPort.u32ChnId : %d \n",stBindInfo.stDstChnPort.u32ChnId);
                // printf("stBindInfo.stDstChnPort.u32PortId : %d \n",stBindInfo.stDstChnPort.u32PortId);
                // printf("stBindInfo.u32SrcFrmrate : %d \n",stBindInfo.u32SrcFrmrate);
                // printf("stBindInfo.u32DstFrmrate : %d \n",stBindInfo.u32DstFrmrate);
                // printf("stBindInfo.eBindType : %d \n",stBindInfo.eBindType);
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
