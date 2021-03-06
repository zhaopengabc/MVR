#include <string.h>
#include "../../../factory.h"
#include "../hielement.h"
#include <pthread.h>
#include <iostream>
#include <unistd.h>
#include "st_common.h"
#include "st_vif.h"
#include "st_vpe.h"
#include "st_venc.h"
#include <sys/time.h>

using namespace std;
#define max 100
string to_String(int n)
{
    int m = n;
    char s[max];
    char ss[max];
    int i = 0, j = 0;
    if (n < 0) // 处理负数
    {
        m = 0 - m;
        j = 1;
        ss[0] = '-';
    }
    while (m > 0)
    {
        s[i++] = m % 10 + '0';
        m /= 10;
    }
    s[i] = '\0';
    i = i - 1;
    while (i >= 0)
    {
        ss[j++] = s[i--];
    }
    ss[j] = '\0';
    return ss;
}

namespace CXS
{
#define ELEMENT_CLASS_NAME "vif"
#define MAX_FRAME_NUM 8
#define MAX_LAYER_NUMS 128

    pthread_t pthreadId;

    typedef struct resolution
    {
        int width;
        int height;
    } TY_RESOLUTION;

    class vif : public HiElement
    {
        vif() : HiElement(ELEMENT_CLASS_NAME)
        {
        }
        
        typedef struct webPara
        {
            int dst_horizontal_blks;
            int dst_vertical_blks;
            int layer_layout_tab[MAX_LAYER_NUMS];
        } TY_WEB_PARA;

    private:
        static int reorganizeYUV(MI_SYS_FrameData_t *inFrameData, MI_SYS_FrameData_t *outFrameData, TY_WEB_PARA *webPara)
        {
            int layerId = -1;
            int vi_w = 3840;
            int vi_h = 2160;
            int blk_w = vi_w / webPara->dst_horizontal_blks;
            int blk_h = vi_h / webPara->dst_vertical_blks;
            for (int i = 0; i < webPara->dst_horizontal_blks * webPara->dst_vertical_blks; i++)
            {
                layerId = webPara->layer_layout_tab[i];
                if (layerId != -1)
                {
                    uint8_t *src_y = (uint8_t *)inFrameData->pVirAddr[0] + layerId / webPara->dst_horizontal_blks * blk_h * vi_w + layerId % webPara->dst_horizontal_blks * blk_w;
                    uint8_t *dst_y = (uint8_t *)outFrameData->pVirAddr[0] + i / webPara->dst_horizontal_blks * blk_h * vi_w + i % webPara->dst_horizontal_blks * blk_w;
                    uint8_t *src_uv = (uint8_t *)inFrameData->pVirAddr[1] + layerId / webPara->dst_horizontal_blks * blk_h / 2 * vi_w + layerId % webPara->dst_horizontal_blks * blk_w;
                    uint8_t *dst_uv = (uint8_t *)outFrameData->pVirAddr[1] + i / webPara->dst_horizontal_blks * blk_h / 2 * vi_w + i % webPara->dst_horizontal_blks * blk_w;

                    for (int y = 0; y < blk_h; y++)
                    {
                        memcpy(dst_y, src_y, blk_w);
                        memcpy(dst_uv, src_uv, blk_w);
                        src_y += vi_w;
                        dst_y += vi_w;
                        if (y % 2 == 1)
                        {
                            src_uv += vi_w;
                            dst_uv += vi_w;
                        }
                    }
                }
            }

            return 0;
        }
        static void *YUVdataProcess(void *para)
        {
            MI_U32 ret = 0;
            MI_S32 s32Fd = 0;

            vif *VifElement;
            VifElement = (vif *)para;

            MI_VIF_DEV vifDev = atoi(VifElement->getAttr("vifDev", "0").c_str());
            MI_VIF_CHN vifChn = vifDev * 4;
            MI_VPE_CHANNEL vpechn = vifDev;
            MI_VENC_CHN vencChn = 0;
            MI_U32 u32VencDevId = 0;
            MI_SYS_ChnPort_t stChnPort;
            MI_SYS_ChnPort_t stVencChnInput;
            struct timeval TimeoutVal;
            MI_SYS_BufInfo_t stBufInfo;
            MI_SYS_BUF_HANDLE hHandle;
            MI_SYS_BufConf_t stVencBufConf;
            MI_SYS_BufInfo_t stVencBufInfo;
            MI_SYS_BUF_HANDLE hVencHandle;
            struct timeval stTv;
            TY_RESOLUTION vencResolution;
            std::string tmpResolution;
            fd_set read_fds;

            tmpResolution = VifElement->getAttr("resolution", "FHD");
            if (tmpResolution == "4K")
            {
                vencResolution.width = 3840;
                vencResolution.height = 2160;
            }
            else if (tmpResolution == "FHD")
            {
                vencResolution.width = 1920;
                vencResolution.height = 1080;
            }
            else if (tmpResolution == "HD")
            {
                vencResolution.width = 1280;
                vencResolution.height = 720;
            }

            memset(&stChnPort, 0x0, sizeof(MI_SYS_ChnPort_t));
            stChnPort.eModId = E_MI_MODULE_ID_VPE;
            stChnPort.u32DevId = vifDev;
            stChnPort.u32ChnId = vpechn;
            stChnPort.u32PortId = 0;

            ret = MI_SYS_SetChnOutputPortDepth(&stChnPort, 2, 3);
            if (MI_SUCCESS != ret)
            {
                ST_ERR("MI_SYS_SetChnOutputPortDepth err:%x, chn:%d,port:%d\n", ret, stChnPort.u32ChnId, stChnPort.u32PortId);
                return NULL;
            }
            ret = MI_SYS_GetFd(&stChnPort, &s32Fd);
            if (MI_SUCCESS != ret)
            {
                ST_ERR("MI_SYS_GetFd 0, error, %X\n", ret);
                return NULL;
            }

            MI_VENC_GetChnDevid(vencChn, &u32VencDevId);
            memset(&stVencChnInput, 0, sizeof(MI_SYS_ChnPort_t));
            stVencChnInput.eModId = E_MI_MODULE_ID_DIVP;
            stVencChnInput.u32DevId = u32VencDevId;
            stVencChnInput.u32ChnId = vencChn;
            stVencChnInput.u32PortId = 0;

            while (1)
            {
                FD_ZERO(&read_fds);
                FD_SET(s32Fd, &read_fds);
                TimeoutVal.tv_sec = 1;
                TimeoutVal.tv_usec = 0;
                ret = select(s32Fd + 1, &read_fds, NULL, NULL, &TimeoutVal);
                if (ret < 0)
                {
                    ST_ERR("select failed!\n"); // usleep(10 * 1000); continue;
                    continue;
                }
                else if (ret == 0)
                {
                    ST_ERR("get vif frame time out\n"); //usleep(10 * 1000);
                    continue;
                }
                else
                {
                    if (FD_ISSET(s32Fd, &read_fds))
                    {
                        memset(&stBufInfo, 0, sizeof(MI_SYS_BufInfo_t));
                        ret = MI_SYS_ChnOutputPortGetBuf(&stChnPort, &stBufInfo, &hHandle);
                        if (ret != MI_SUCCESS)
                        {
                            MI_SYS_ChnOutputPortPutBuf(hHandle);
                            continue;
                        }
                        else
                        {
                            memset(&stVencBufConf, 0, sizeof(MI_SYS_BufConf_t));
                            stVencBufConf.eBufType = E_MI_SYS_BUFDATA_FRAME;
                            gettimeofday(&stTv, NULL);
                            stVencBufConf.u64TargetPts = stTv.tv_sec * 1000000 + stTv.tv_usec;
                            stVencBufConf.stFrameCfg.eFormat = E_MI_SYS_PIXEL_FRAME_YUV_SEMIPLANAR_420;
                            stVencBufConf.stFrameCfg.eFrameScanMode = E_MI_SYS_FRAME_SCAN_MODE_PROGRESSIVE;
                            stVencBufConf.stFrameCfg.u16Width = vencResolution.width;
                            stVencBufConf.stFrameCfg.u16Height = vencResolution.height;
                            // size = stBufInfo.stFrameData.u32BufSize;

                            ret = MI_SYS_ChnInputPortGetBuf(&stVencChnInput, &stVencBufConf, &stVencBufInfo, &hVencHandle, 0);
                            if (ret == 0)
                            {
                                TY_WEB_PARA webPara;
                                int layNum;
                                int i = 0;
                                memset(&webPara, 0, sizeof(TY_WEB_PARA));
                                webPara.dst_horizontal_blks = atoi(VifElement->getAttr("dst_horizontal_blks", "0").c_str());
                                webPara.dst_vertical_blks = atoi(VifElement->getAttr("dst_vertical_blks", "0").c_str());
                                layNum = atoi(VifElement->getAttr("layNum", "0").c_str());
                                for (int i = 0; i < layNum; i++)
                                {
                                    std::string tabId = "tabId";
                                    std::string tab = tabId + to_String(i);
                                    webPara.layer_layout_tab[i] = atoi(VifElement->getAttr(tab.c_str(), "0").c_str());
                                }

                                reorganizeYUV(&(stBufInfo.stFrameData), &(stVencBufInfo.stFrameData), &webPara);
                            }
                            // memcpy(stVencBufInfo.stFrameData.pVirAddr[0],stBufInfo.stFrameData.pVirAddr[0],stBufInfo.stFrameData.u32BufSize);
                            MI_SYS_ChnInputPortPutBuf(hVencHandle, &stVencBufInfo, FALSE);
                            MI_SYS_ChnOutputPortPutBuf(hHandle);
                        }
                    }
                }
            }
        }

    public:
        static Element *createInstance()
        {
            vif *vi = new vif();
            return vi;
        }
        int startSelf()
        {
            
            int s32Ret = 0;
            MI_SNR_PADInfo_t stPad0Info;
            MI_SNR_PlaneInfo_t stSnrPlane0Info;
            MI_VIF_DEV vifDev = atoi(this->getAttr("vifDev", "0").c_str());
            MI_VIF_CHN vifChn = vifDev * 4;
            MI_VPE_CHANNEL vpechn = vifDev;
            MI_SNR_PAD_ID_e eSnrPadId = (MI_SNR_PAD_ID_e)atoi(this->getAttr("eSnrPad", "0").c_str());

            memset(&stPad0Info, 0x0, sizeof(MI_SNR_PADInfo_t));
            MI_SNR_SetPlaneMode(eSnrPadId, FALSE);
            MI_SNR_SetRes(eSnrPadId, 0);
            MI_SNR_Enable(eSnrPadId);
            MI_SNR_GetPadInfo(eSnrPadId, &stPad0Info);
            MI_SNR_GetPlaneInfo(eSnrPadId, 0, &stSnrPlane0Info);

            MI_VIF_DevAttr_t stDevAttr;
            memset(&stDevAttr, 0x0, sizeof(MI_VIF_DevAttr_t));
            stDevAttr.eIntfMode = stPad0Info.eIntfMode;
            stDevAttr.eWorkMode = E_MI_VIF_WORK_MODE_RGB_FRAMEMODE;
            stDevAttr.eHDRType = E_MI_VIF_HDR_TYPE_OFF;
            if (stDevAttr.eIntfMode == E_MI_VIF_MODE_MIPI)
            {
                stDevAttr.eDataSeq = stPad0Info.unIntfAttr.stMipiAttr.eDataYUVOrder;
            }
            else
            {
                stDevAttr.eDataSeq = E_MI_VIF_INPUT_DATA_YUYV;
            }

            stDevAttr.eBitOrder = E_MI_VIF_BITORDER_NORMAL;

            MI_VIF_SetDevAttr(vifDev, &stDevAttr);
            MI_VIF_EnableDev(vifDev);
            
            MI_U32 u32InputPort = atoi(this->getAttr("u32InputPort", "0").c_str());
            MI_U32 u32CapWidth = 0, u32CapHeight = 0;
            MI_VIF_FrameRate_e eFrameRate;
            MI_SYS_PixelFormat_e ePixFormat;

            u32CapWidth = stSnrPlane0Info.stCapRect.u16Width;
            u32CapHeight = stSnrPlane0Info.stCapRect.u16Height;
            eFrameRate = E_MI_VIF_FRAMERATE_FULL;
            ePixFormat = (MI_SYS_PixelFormat_e)RGB_BAYER_PIXEL(stSnrPlane0Info.ePixPrecision, stSnrPlane0Info.eBayerId);

            ST_VIF_PortInfo_T stVifPortInfoInfo;
            memset(&stVifPortInfoInfo, 0, sizeof(ST_VIF_PortInfo_T));
            stVifPortInfoInfo.u32RectX = stSnrPlane0Info.stCapRect.u16X;
            stVifPortInfoInfo.u32RectY = stSnrPlane0Info.stCapRect.u16Y;
            stVifPortInfoInfo.u32RectWidth = u32CapWidth;
            stVifPortInfoInfo.u32RectHeight = u32CapHeight;
            stVifPortInfoInfo.u32DestWidth = u32CapWidth;
            stVifPortInfoInfo.u32DestHeight = u32CapHeight;
            stVifPortInfoInfo.eFrameRate = eFrameRate;
            stVifPortInfoInfo.ePixFormat = ePixFormat; //E_MI_SYS_PIXEL_FRAME_RGB_BAYER_12BPP_GR;
            ST_Vif_CreatePort(vifChn, u32InputPort, &stVifPortInfoInfo);
            ST_Vif_StartPort(0, vifChn, u32InputPort);

            MI_ModuleId_e eVifModeId = E_MI_MODULE_ID_VIF;
            MI_U8 u8MmaHeap[128] = "mma_heap_name0";
            s32Ret = MI_SYS_SetChnMMAConf(eVifModeId, 0, vifChn, u8MmaHeap);
            

            return s32Ret;
        }
        int linkTo(Element *elem)
        {
            MI_U32 ret = MI_SUCCESS;
            MI_U32 i = 0;
            MI_VIF_DEV vifDev = 0;
            MI_VIF_CHN vifChn = 0;
            MI_VPE_CHANNEL vpechn = 0;
            MI_VENC_CHN vencChn = 0;
            MI_U32 u32VencDevId = 0;
            ST_Sys_BindInfo_T stBindInfo;

            HiElement *hiElem = dynamic_cast<HiElement *>(elem);
            vifDev = atoi(this->getAttr("vifDev", "0").c_str());
            vifChn = vifDev * 4;
            vpechn = vifDev;
            vencChn = atoi(this->getAttr("vencChn", "0").c_str());

            if (strcmp(hiElem->getClassName(), "vpe") == 0)
            {
                /************************************************
                    Step1: bind VIF->VPE
                *************************************************/
                memset(&stBindInfo, 0x0, sizeof(ST_Sys_BindInfo_T));
                stBindInfo.stSrcChnPort.eModId = E_MI_MODULE_ID_VIF;
                stBindInfo.stSrcChnPort.u32DevId = vifDev;
                stBindInfo.stSrcChnPort.u32ChnId = vifChn;
                stBindInfo.stSrcChnPort.u32PortId = 0;

                stBindInfo.stDstChnPort.eModId = E_MI_MODULE_ID_VPE;
                stBindInfo.stDstChnPort.u32DevId = 0; // why always 0 ???
                stBindInfo.stDstChnPort.u32ChnId = vpechn;
                stBindInfo.stDstChnPort.u32PortId = 0;

                stBindInfo.u32SrcFrmrate = 20;
                stBindInfo.u32DstFrmrate = 20;
                stBindInfo.eBindType = E_MI_SYS_BIND_TYPE_FRAME_BASE;
                STCHECKRESULT(ST_Sys_Bind(&stBindInfo));

                std::string mode = this->getAttr("mode", "0");

                if (mode == "regroup")
                {
                    pthread_create(&pthreadId, NULL, YUVdataProcess, (void *)this);
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
        pthread_join(pthreadId, NULL);
    }
    static ElementDescriptor elementDescriptors[] = {
        {"SSC339G",
         ELEMENT_CLASS_NAME,
         init,
         deInit,
         vif::createInstance},
    };
    REGISTER_ELEMENT(elementDescriptors, sizeof(elementDescriptors) / sizeof(elementDescriptors[0]));
} // namespace CXS
