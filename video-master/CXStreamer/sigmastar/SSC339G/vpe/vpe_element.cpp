#include <string.h>
#include "../../../factory.h"
#include "../hielement.h"
#include "zstd.h"
#include "st_common.h"
#include "st_vif.h"
#include "st_vpe.h"
#include "st_venc.h"
#include <sys/time.h>
#include "mi_divp.h"

namespace CXS
{
#define ELEMENT_CLASS_NAME "vpe"

    class Vpe : public HiElement
    {
        typedef struct ty_resolution
        {
            MI_U32 width;
            MI_U32 height;
        } TY_RESOLUTION;

        Vpe() : HiElement(ELEMENT_CLASS_NAME)
        {
            setAttr("SECOND_VPE_CHNID", "2");
        }

    public:
        static Element *createInstance()
        {
            Vpe *elem = new Vpe();
            return elem;
        }
        int startSelf()
        {
            int ret = 0;
            MI_VIF_DEV vifDev;
            MI_VPE_CHANNEL vpechn;
            MI_VENC_CHN VencChn;
            MI_U32 DivpChn = 0;
            /************************************************
            Step3:  init VPE (create one VPE)
            *************************************************/
            ST_VPE_ChannelInfo_T stVpeChannelInfo;
            ST_VPE_PortInfo_T stVpePortInfo;

            MI_SYS_PixelFormat_e pixelFormat;
            MI_VPE_SensorChannel_e eBindSensorId;
            std::string tmpSensorId;
            std::string tmpResolution;
            // std::string scaleResolution;
            TY_RESOLUTION maxResolution;
            TY_RESOLUTION vpeResolution;
            TY_RESOLUTION scaleResolution;



            vpechn = atoi(this->getAttr("vpeChn", "0").c_str());
            pixelFormat = (MI_SYS_PixelFormat_e)atoi(this->getAttr("pixelFormat", "35").c_str());
            tmpSensorId = this->getAttr("sensorId", "0");

            tmpResolution = this->getAttr("resolution", "FHD");
            if (tmpResolution == "4K")
            {
                maxResolution.width = 3840;
                maxResolution.height = 2160;
                vpeResolution.width = 3840;
                vpeResolution.height = 2160;
            }
            else if (tmpResolution == "FHD")
            {
                maxResolution.width = 3840;
                maxResolution.height = 2160;
                vpeResolution.width = 1920;
                vpeResolution.height = 1080;
            }
            else if (tmpResolution == "HD")
            {
                maxResolution.width = 1920;
                maxResolution.height = 1080;
                vpeResolution.width = 1280;
                vpeResolution.height = 720;
            }

            tmpResolution = this->getAttr("scale", "FHD");
            if(tmpResolution == "4K")
            {
                scaleResolution.width = 3840;
                scaleResolution.height = 2160;
            }
            else if(tmpResolution == "FHD")
            {
                scaleResolution.width = 1920;
                scaleResolution.height = 1080;
            }
            else if(tmpResolution == "HD")
            {
                scaleResolution.width = 1280;
                scaleResolution.height = 720;
            }
            if (tmpSensorId == "0")
            {
                eBindSensorId = E_MI_VPE_SENSOR0;
            }
            else if (tmpSensorId == "1")
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
            STCHECKRESULT(ST_Vpe_StartPort(0, &stVpePortInfo));

            std::string mode = this->getAttr("ReGropMode", "0");
            if(mode != "No-Regroup")
            {
                MI_DIVP_ChnAttr_t stDivpChnAttr;
                memset(&stDivpChnAttr, 0x00, sizeof(MI_DIVP_ChnAttr_t));

                stDivpChnAttr.bHorMirror = FALSE;
                stDivpChnAttr.bVerMirror = FALSE;
                stDivpChnAttr.eDiType = E_MI_DIVP_DI_TYPE_OFF;
                stDivpChnAttr.eRotateType = E_MI_SYS_ROTATE_NONE;
                stDivpChnAttr.eTnrLevel = E_MI_DIVP_TNR_LEVEL_OFF;
                stDivpChnAttr.stCropRect.u16X = 0;
                stDivpChnAttr.stCropRect.u16Y = 0;
                stDivpChnAttr.stCropRect.u16Width = 0;
                stDivpChnAttr.stCropRect.u16Height = 0;
                stDivpChnAttr.u32MaxWidth = maxResolution.width;
                stDivpChnAttr.u32MaxHeight = maxResolution.height;

                ExecFunc(MI_DIVP_CreateChn(DivpChn, &stDivpChnAttr), MI_SUCCESS);
                MI_DIVP_OutputPortAttr_t stDivpOutputPortAttr;
                stDivpOutputPortAttr.eCompMode = E_MI_SYS_COMPRESS_MODE_NONE;
                stDivpOutputPortAttr.ePixelFormat = E_MI_SYS_PIXEL_FRAME_YUV_SEMIPLANAR_420;
                stDivpOutputPortAttr.u32Width = scaleResolution.width;
                stDivpOutputPortAttr.u32Height = vpeResolution.height;
                MI_DIVP_SetOutputPortAttr(DivpChn, &stDivpOutputPortAttr);
                ExecFunc(MI_DIVP_StartChn(DivpChn), MI_SUCCESS);
            }
            


            return ret;
        }
        int linkTo(Element *elem)
        {
            HiElement *hiElem = dynamic_cast<HiElement *>(elem);
            MI_S32 ret = 0;
            ST_Sys_BindInfo_T stBindInfo;
            MI_U32 u32VencDevId = 0xff;
            MI_VIF_DEV vifDev;
            MI_VPE_CHANNEL vpechn;
            MI_VENC_CHN VencChn;

            if (strcmp(hiElem->getClassName(), "venc") == 0)
            {
                vifDev = atoi(this->getAttr("vifDev", "0").c_str());
                vpechn = atoi(this->getAttr("vpeChn", "0").c_str());
                VencChn = atoi(this->getAttr("vencChn", "0").c_str());
                std::string mode = this->getAttr("ReGropMode", "0");
                MI_VENC_GetChnDevid(VencChn, &u32VencDevId);
                if (mode == "No-Regroup")
                {
                    memset(&stBindInfo, 0x0, sizeof(ST_Sys_BindInfo_T));
                    stBindInfo.stSrcChnPort.eModId = E_MI_MODULE_ID_VPE;
                    stBindInfo.stSrcChnPort.u32DevId = 0;
                    stBindInfo.stSrcChnPort.u32ChnId = vpechn;
                    stBindInfo.stSrcChnPort.u32PortId = 0;

                    stBindInfo.stDstChnPort.eModId = E_MI_MODULE_ID_VENC;
                    stBindInfo.stDstChnPort.u32DevId = u32VencDevId;
                    stBindInfo.stDstChnPort.u32ChnId = VencChn;
                    stBindInfo.stDstChnPort.u32PortId = 0;

                    stBindInfo.u32SrcFrmrate = 20;
                    stBindInfo.u32DstFrmrate = 20;
                    stBindInfo.eBindType = E_MI_SYS_BIND_TYPE_FRAME_BASE;
                    ret = ST_Sys_Bind(&stBindInfo);
                }
                else
                {
                    // ST_Sys_BindInfo_T DIVPstBindInfo;
                    // memset(&DIVPstBindInfo, 0x00, sizeof(ST_Sys_BindInfo_T));
                    // DIVPstBindInfo.stSrcChnPort.eModId = E_MI_MODULE_ID_VPE;
                    // DIVPstBindInfo.stSrcChnPort.u32DevId = 0;
                    // DIVPstBindInfo.stSrcChnPort.u32ChnId = 0;
                    // DIVPstBindInfo.stSrcChnPort.u32PortId = 0;

                    // DIVPstBindInfo.stDstChnPort.eModId = E_MI_MODULE_ID_DIVP;
                    // DIVPstBindInfo.stDstChnPort.u32DevId = 0;
                    // DIVPstBindInfo.stDstChnPort.u32ChnId = 0;
                    // DIVPstBindInfo.stDstChnPort.u32PortId = 0;
                    // DIVPstBindInfo.eBindType = E_MI_SYS_BIND_TYPE_FRAME_BASE;

                    // DIVPstBindInfo.u32SrcFrmrate = 30;
                    // DIVPstBindInfo.u32DstFrmrate = 30;
                    // ST_Sys_Bind(&DIVPstBindInfo);   
                    printf("\n\n\n\n\n\n");
                    memset(&stBindInfo, 0x0, sizeof(ST_Sys_BindInfo_T));
                    stBindInfo.stSrcChnPort.eModId = E_MI_MODULE_ID_DIVP;
                    stBindInfo.stSrcChnPort.u32DevId = 0;
                    stBindInfo.stSrcChnPort.u32ChnId = 0;
                    stBindInfo.stSrcChnPort.u32PortId = 0;

                    stBindInfo.stDstChnPort.eModId = E_MI_MODULE_ID_VENC;
                    stBindInfo.stDstChnPort.u32DevId = u32VencDevId;
                    stBindInfo.stDstChnPort.u32ChnId = VencChn;
                    stBindInfo.stDstChnPort.u32PortId = 0;

                    stBindInfo.u32SrcFrmrate = 20;
                    stBindInfo.u32DstFrmrate = 20;
                    stBindInfo.eBindType = E_MI_SYS_BIND_TYPE_FRAME_BASE;
                    ret = ST_Sys_Bind(&stBindInfo);
                                    
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
        {"SSC339G",
         ELEMENT_CLASS_NAME,
         init,
         deInit,
         Vpe::createInstance},
    };
    REGISTER_ELEMENT(elementDescriptors, sizeof(elementDescriptors) / sizeof(elementDescriptors[0]));
} // namespace CXS
