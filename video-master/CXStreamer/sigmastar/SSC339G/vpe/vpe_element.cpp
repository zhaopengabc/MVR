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
        typedef struct ty_resolution
        {
            MI_U32 width;
            MI_U32 height;
        }TY_RESOLUTION;
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
            MI_VPE_SensorChannel_e eBindSensorId;
            std::string tmpSensorId;
            std::string tmpResolution;
            TY_RESOLUTION maxResolution;
            TY_RESOLUTION vpeResolution;


            vpechn = atoi(this->getAttr("vpeChn","0").c_str());
            pixelFormat = (MI_SYS_PixelFormat_e)atoi(this->getAttr("pixelFormat","35").c_str());
            tmpSensorId =  this->getAttr("sensorId","0");

            tmpResolution = this->getAttr("relolution","FHD");
            if(tmpResolution == "FHD")
            {
                maxResolution.width = 3840;
                maxResolution.height = 2160;
                vpeResolution.width = 1920;
                vpeResolution.height = 1080;
            }
            else if(tmpResolution == "HD")
            {
                maxResolution.width = 1920;
                maxResolution.height = 1080;
                vpeResolution.width = 1280;
                vpeResolution.height = 720;
            }
            
            if(tmpSensorId == "0")
            {
                eBindSensorId = E_MI_VPE_SENSOR0;
            }
            else if(tmpSensorId == "1")
            {
                eBindSensorId = E_MI_VPE_SENSOR1;
            }
                memset(&stVpeChannelInfo, 0, sizeof(ST_VPE_ChannelInfo_T));
                stVpeChannelInfo.u16VpeMaxW = maxResolution.width;
                stVpeChannelInfo.u16VpeMaxH = maxResolution.height;
                stVpeChannelInfo.u32X = 0;
                stVpeChannelInfo.u32Y = 0;
                stVpeChannelInfo.u16VpeCropW = 0;
                stVpeChannelInfo.u16VpeCropH = 0;
                stVpeChannelInfo.eRunningMode = E_MI_VPE_RUN_CAM_MODE;
                stVpeChannelInfo.eFormat = pixelFormat;
                stVpeChannelInfo.eHDRtype = E_MI_VPE_HDR_TYPE_OFF;
                stVpeChannelInfo.eBindSensorId = eBindSensorId;

                ST_Vpe_CreateChannel(vpechn, &stVpeChannelInfo);
                ST_Vpe_StartChannel(vpechn);

                 memset(&stVpePortInfo, 0, sizeof(ST_VPE_PortInfo_T));

                stVpePortInfo.DepVpeChannel = vpechn;
                stVpePortInfo.u16OutputWidth = vpeResolution.width;
                stVpePortInfo.u16OutputHeight = vpeResolution.height;
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
                    vpechn = atoi(this->getAttr("vpeChn","0").c_str());
                    VencChn = atoi(this->getAttr("vencChn","0").c_str());

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
