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

using namespace std;
 #define max 100
 string to_String(int n)
 {
      int m = n;
      char s[max];
     char ss[max];
     int i=0,j=0;
     if (n < 0)// 处理负数
     {
         m = 0 - m;
         j = 1;
         ss[0] = '-';
     }    
     while (m>0)
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

namespace CXS{
    #define ELEMENT_CLASS_NAME  "vif"
    #define MAX_FRAME_NUM 8
    #define MAX_DECODE_CH 1
    typedef struct resolution
    {
        int width;
        int height;
    }TY_RESOLUTION;
    typedef struct chnParam
    {
        int devId;
        int chnId;
        int portId;
        int vpeCh;
        MI_SNR_PAD_ID_e eSnrPad;
        TY_RESOLUTION resolution;
    }TY_CHN_PARM;
    typedef struct VIF_PARAM
    {
        TY_CHN_PARM firstChn;
        TY_CHN_PARM secondChn;
    }TY_VIF_PARAM;

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
        MI_U32 enFunc;
        const char    *pszStreamName;
        MI_SNR_PAD_ID_e eSnrPadId;
    };
    typedef struct
    {
        MI_VENC_CHN vencChn;
        MI_VENC_ModType_e enType;
        char szStreamName[64];

        MI_BOOL bWriteFile;
        int fd;
        char szDebugFile[128];
    } ST_StreamInfo_T;

    static struct ST_Stream_Attr_T g_stStreamAttr[MAX_DECODE_CH];

    void ST_Flush(void)
    {
        char c;

        while((c = getchar()) != '\n' && c != EOF);
    }
       
    static int StartVif()
    {
        MI_U32 s32Ret = -1;
        MI_U32 i = 0;
        MI_U32 u32CapWidth = 0, u32CapHeight = 0;
        MI_VIF_FrameRate_e eFrameRate;
        MI_SNR_PAD_ID_e eSnrPadId;
        MI_SNR_Res_t stRes;

        MI_VIF_DEV vifDev;
        MI_VIF_CHN vifChn;
        MI_VPE_CHANNEL vpechn;
        MI_U32     u32InputPort;

        MI_SNR_PADInfo_t  stPad0Info;
        MI_SNR_PlaneInfo_t stSnrPlane0Info;
        MI_VIF_DevAttr_t stDevAttr;

        g_stStreamAttr[0].s32vifDev = 0;
        g_stStreamAttr[0].enInput = ST_Sys_Input_VPE;
        g_stStreamAttr[0].u32InputChn = 0;
        g_stStreamAttr[0].u32InputPort = 0;
        g_stStreamAttr[0].vencChn = 0;
        g_stStreamAttr[0].eType = E_MI_VENC_MODTYPE_H264E;
        g_stStreamAttr[0].u32Width = 1920;
        g_stStreamAttr[0].u32Height = 1080;
        g_stStreamAttr[0].eSnrPadId = E_MI_SNR_PAD_ID_0;

        g_stStreamAttr[1].s32vifDev = 2;
        g_stStreamAttr[1].enInput =ST_Sys_Input_VPE;
        g_stStreamAttr[1].u32InputChn = 2;
        g_stStreamAttr[1].u32InputPort = 0;
        g_stStreamAttr[1].vencChn = 2;
        g_stStreamAttr[1].eType = E_MI_VENC_MODTYPE_H264E;
        g_stStreamAttr[1].u32Width = 1280;
        g_stStreamAttr[1].u32Height = 720;
        // g_stStreamAttr[1].u32Width = 3840;
        // g_stStreamAttr[1].u32Height = 2160;
        g_stStreamAttr[1].eSnrPadId = E_MI_SNR_PAD_ID_1;
        MI_SYS_PixelFormat_e ePixFormat;

        

        for(i=0;i<MAX_DECODE_CH;i++)
        {
            memset(&stPad0Info, 0x0, sizeof(MI_SNR_PADInfo_t));
            memset(&stRes, 0x0, sizeof(MI_SNR_Res_t));
            memset(&stDevAttr, 0x0, sizeof(MI_VIF_DevAttr_t));

            vifDev = g_stStreamAttr[i].s32vifDev;
            vifChn = vifDev * 4;
            vpechn = vifDev;
            u32InputPort = g_stStreamAttr[i].u32InputPort;
            eSnrPadId = g_stStreamAttr[i].eSnrPadId;


            MI_SNR_SetPlaneMode(eSnrPadId, FALSE);
            MI_SNR_SetRes(eSnrPadId,6);//0 1920*1080
            MI_SNR_Enable(eSnrPadId);
            MI_SNR_GetPadInfo(eSnrPadId, &stPad0Info);
            MI_SNR_GetPlaneInfo(eSnrPadId, 0, &stSnrPlane0Info);

            /************************************************
            Step1:  init SYS
            *************************************************/
            // STCHECKRESULT(ST_Sys_Init());

            /************************************************
            Step2:  init VIF
            *************************************************/
            stDevAttr.eIntfMode = stPad0Info.eIntfMode;
            stDevAttr.eWorkMode = E_MI_VIF_WORK_MODE_RGB_FRAMEMODE;
            stDevAttr.eHDRType = E_MI_VIF_HDR_TYPE_OFF;
            if(stDevAttr.eIntfMode == E_MI_VIF_MODE_MIPI)
            {
                stDevAttr.eDataSeq =stPad0Info.unIntfAttr.stMipiAttr.eDataYUVOrder;
            }
            else
            {
                stDevAttr.eDataSeq = E_MI_VIF_INPUT_DATA_YUYV;
            }

            stDevAttr.eBitOrder = E_MI_VIF_BITORDER_NORMAL; 
            
            MI_VIF_SetDevAttr(vifDev, &stDevAttr);
            MI_VIF_EnableDev(vifDev);

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
            stVifPortInfoInfo.ePixFormat = ePixFormat;//E_MI_SYS_PIXEL_FRAME_RGB_BAYER_12BPP_GR;
            ST_Vif_CreatePort(vifChn, u32InputPort, &stVifPortInfoInfo);
            ST_Vif_StartPort(0, vifChn, u32InputPort);

            MI_ModuleId_e eVifModeId = E_MI_MODULE_ID_VIF;
            MI_U8 u8MmaHeap[128] = "mma_heap_name0";
            s32Ret = MI_SYS_SetChnMMAConf(eVifModeId, 0, vifChn, u8MmaHeap);
            printf("\n\n\n\n =========init VIF ==============\n");
            printf("vifChn : %d \n",vifChn);
            printf("u32InputPort : %d \n",u32InputPort);
            printf("stVifPortInfoInfo.u32RectX : %d \n",stVifPortInfoInfo.u32RectX);
            printf("stVifPortInfoInfo.u32RectY : %d \n",stVifPortInfoInfo.u32RectY);
            printf("stVifPortInfoInfo.u32RectWidth : %d \n",stVifPortInfoInfo.u32RectWidth);
            printf("stVifPortInfoInfo.u32RectHeight : %d \n",stVifPortInfoInfo.u32RectHeight);
            printf("stVifPortInfoInfo.u32DestWidth : %d \n",stVifPortInfoInfo.u32DestWidth);
            printf("stVifPortInfoInfo.u32DestHeight : %d \n",stVifPortInfoInfo.u32DestHeight);
            printf("stVifPortInfoInfo.eFrameRate : %d \n",stVifPortInfoInfo.eFrameRate);
            printf("stVifPortInfoInfo.ePixFormat : %d \n",stVifPortInfoInfo.ePixFormat);
        }

        return s32Ret;
    }

    class vif : public HiElement
    {
        pthread_t mFrameThread;

        vif():HiElement(ELEMENT_CLASS_NAME)
        {            

        }
        typedef struct UserData
        {
            MI_U32 mViChn;
            vif* mViElem;
            UserData(MI_U32 viChn,vif* viElem)
            {
                mViChn = viChn;
                mViElem = viElem;
            }
        }UserData;


    private:
    //     static void* frameThread(void* data)
    //     {
    //         UserData* ud = (UserData*)data;
    //         HI_U32 viChn = ud->mViChn;
    //         vif* viElem = ud->mViElem;
    //         VIDEO_FRAME_INFO_S stFrame[MAX_FRAME_NUM];

    //         uint64_t ptr = viElem->getAttr("ON-NEW-FRAME",(uint64_t)nullptr);
    //         void (*mNewFrameHandler)(VIDEO_FRAME_INFO_S* event_para) = (void (*)(VIDEO_FRAME_INFO_S* ))ptr;
    //         while(true){
    //             int frame_num = 0;
    //             while(frame_num < MAX_FRAME_NUM && 0 == HI_MPI_VI_GetFrame(viChn, &stFrame[frame_num],10)){
    //                 frame_num++;
    //             }
    //             if(frame_num == 0){
    //                 //usleep(1000);
    //                 continue;
    //             }
    //             int cur = frame_num-1;

    //             for(int i = 0;i < cur;i++){
    //                 HI_MPI_VI_ReleaseFrame(viChn,&stFrame[i]);
    //             }

    //             //stFrame.stVFrame.enPixelFormat == PIXEL_FORMAT_YUV_SEMIPLANAR_420;
    //             for(int i =0;i < 3;i++){
    //                 if(stFrame[cur].stVFrame.u32PhyAddr[i])
    //                     stFrame[cur].stVFrame.pVirAddr[i] = (uint8_t *)HI_MPI_SYS_Mmap(stFrame[cur].stVFrame.u32PhyAddr[i], stFrame[cur].stVFrame.u32Stride[i]*stFrame[cur].stVFrame.u32Height);
    //             }

    //             if(mNewFrameHandler){
    //                 mNewFrameHandler(&stFrame[cur]);
    //             }
    //             for(int i =0;i < 3;i++){
    //                 if(stFrame[cur].stVFrame.pVirAddr[i])
    //                     HI_MPI_SYS_Munmap((void*)stFrame[cur].stVFrame.pVirAddr[i], stFrame[cur].stVFrame.u32Stride[i]*stFrame[cur].stVFrame.u32Height);
    //             }
    //             HI_MPI_VI_ReleaseFrame(viChn,&stFrame[cur]);
    //         }
    //         return NULL;
    //     }
 
    public:
        static Element* createInstance()
        {
            vif* vi = new vif();
            return vi;
        }
        int startSelf()
        {
            int s32Ret = 0;
            MI_SNR_PADInfo_t  stPad0Info;
            MI_SNR_PlaneInfo_t stSnrPlane0Info;
            MI_VIF_DEV vifDev = atoi(this->getAttr("vifDev","0").c_str());
            MI_VIF_CHN vifChn = vifDev*4;
            MI_VPE_CHANNEL vpechn = vifDev;
            // std::string padId;
            MI_SNR_PAD_ID_e eSnrPadId = (MI_SNR_PAD_ID_e)atoi(this->getAttr("eSnrPad","0").c_str());

            // padId = this->getAttr("eSnrPad","0");
            // if(padId == "0")
            // {
            //     eSnrPadId = E_MI_SNR_PAD_ID_0;
            // }
            // else if(padId == "1")
            // {
            //     eSnrPadId = E_MI_SNR_PAD_ID_1;
            // }

            memset(&stPad0Info, 0x0, sizeof(MI_SNR_PADInfo_t));
            MI_SNR_SetPlaneMode(eSnrPadId, FALSE);
            MI_SNR_SetRes(eSnrPadId,0);
            MI_SNR_Enable(eSnrPadId);
            MI_SNR_GetPadInfo(eSnrPadId, &stPad0Info);
            MI_SNR_GetPlaneInfo(eSnrPadId, 0, &stSnrPlane0Info);  

            MI_VIF_DevAttr_t stDevAttr;
            memset(&stDevAttr, 0x0, sizeof(MI_VIF_DevAttr_t));
            stDevAttr.eIntfMode = stPad0Info.eIntfMode;
            stDevAttr.eWorkMode = E_MI_VIF_WORK_MODE_RGB_FRAMEMODE;
            stDevAttr.eHDRType = E_MI_VIF_HDR_TYPE_OFF;
            if(stDevAttr.eIntfMode == E_MI_VIF_MODE_MIPI)
            {
                stDevAttr.eDataSeq =stPad0Info.unIntfAttr.stMipiAttr.eDataYUVOrder;
            }
            else
            {
                stDevAttr.eDataSeq = E_MI_VIF_INPUT_DATA_YUYV;
            }

            stDevAttr.eBitOrder = E_MI_VIF_BITORDER_NORMAL; 
            
            MI_VIF_SetDevAttr(vifDev, &stDevAttr);
            MI_VIF_EnableDev(vifDev);


            MI_U32 u32InputPort = atoi(this->getAttr("u32InputPort","0").c_str());
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
            stVifPortInfoInfo.ePixFormat = ePixFormat;//E_MI_SYS_PIXEL_FRAME_RGB_BAYER_12BPP_GR;
            ST_Vif_CreatePort(vifChn, u32InputPort, &stVifPortInfoInfo);
            ST_Vif_StartPort(0, vifChn, u32InputPort);

            MI_ModuleId_e eVifModeId = E_MI_MODULE_ID_VIF;
            MI_U8 u8MmaHeap[128] = "mma_heap_name0";
            s32Ret = MI_SYS_SetChnMMAConf(eVifModeId, 0, vifChn, u8MmaHeap);
            return s32Ret;
        }
        int linkTo(Element* elem)
        {
            MI_U32 ret = MI_SUCCESS;
            MI_U32 i = 0;
            MI_VIF_DEV vifDev;
            MI_VIF_CHN vifChn;
            MI_VPE_CHANNEL vpechn; 
            ST_Sys_BindInfo_T stBindInfo;

            HiElement* hiElem = dynamic_cast<HiElement*>(elem);
            vifDev = atoi(this->getAttr("vifDev","0").c_str());
            vifChn = vifDev*4;
            vpechn = vifDev;

            if(strcmp(hiElem->getClassName(),"vpe") == 0)
            {
                    /************************************************
                    Step1:  bind VIF->VPE
                    *************************************************/
                    memset(&stBindInfo, 0x0, sizeof(ST_Sys_BindInfo_T));
                    stBindInfo.stSrcChnPort.eModId = E_MI_MODULE_ID_VIF;
                    stBindInfo.stSrcChnPort.u32DevId = vifDev;
                    stBindInfo.stSrcChnPort.u32ChnId = vifChn;
                    stBindInfo.stSrcChnPort.u32PortId = 0;

                    stBindInfo.stDstChnPort.eModId = E_MI_MODULE_ID_VPE;
                    stBindInfo.stDstChnPort.u32DevId = 0;// why always 0 ???
                    stBindInfo.stDstChnPort.u32ChnId = vpechn;
                    stBindInfo.stDstChnPort.u32PortId = 0;

                    stBindInfo.u32SrcFrmrate = 30;
                    stBindInfo.u32DstFrmrate = 30;
                    stBindInfo.eBindType = E_MI_SYS_BIND_TYPE_FRAME_BASE;
                    STCHECKRESULT(ST_Sys_Bind(&stBindInfo));
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
             vif::createInstance
        },
    };
    REGISTER_ELEMENT(elementDescriptors,sizeof(elementDescriptors)/sizeof(elementDescriptors[0]));
}
