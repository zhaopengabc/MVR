#include <string.h>
#include "../../../factory.h"
#include "../hielement.h"
// #include "../cxs_hi_comm.h"
#include "zstd.h"
#include <iostream>

#include "st_common.h"
#include "st_vif.h"
#include "st_vpe.h"
#include "st_venc.h"

extern "C"
{
// extern void SetH264Processor(void (*processor)(int vencChn,unsigned char* buffer,int len));
}

namespace CXS{
    #define ELEMENT_CLASS_NAME  "venc"
    #define MAX_VPE_CHN 1

    struct Buffer
    {
        uint8_t* buffer;
        size_t len;
    };
    int getH264flag = 1;
    static void* getH264data(void *para);

    class VENCSSC339G : public HiElement
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

        static std::vector<VENCSSC339G*> arr_VENCSSC339G;
        VENCSSC339G():HiElement(ELEMENT_CLASS_NAME)
        {
            // for(int i =0;i < 32;i++){
            //     if(arr_VENCSSC339G[i] == nullptr){
            //         mHandle = i;
            //         break;
            //     }
            // }
            // arr_VENCSSC339G[mHandle] = this;
            // setAttr("VENC-CHN",mHandle);
            // setAttr("VENC-SRC-FRAMERATE",60);
            // setAttr("VENC-FRAMERATE",15);
            // setAttr("VENC-MODE","HD1080");
            // setAttr("VENC-PAYLOAD","h264");
            setAttr("FIRST_VENC-CHN","0");
            setAttr("SECOND_VENC-CHN","2");
        }
        struct ST_Stream_Attr_T gVencAttr[MAX_VPE_CHN];

    public:
        pthread_t pthreadId;
        
        static Element* createInstance()
        {
            static bool inited = false;
            VENCSSC339G* elem = new VENCSSC339G();

            if(!inited)
            {
                inited = true;
                // SetH264Processor(h264_processor);
            }
            return elem;
        }
        static void* getH264data(void *para)
        {
            MI_SYS_BufInfo_t stBufInfo;
            MI_S32 s32Ret = MI_SUCCESS;
            MI_U32 u32DevId = 0;
            MI_VENC_Stream_t stStream;
            MI_VENC_Pack_t stPack;
            MI_VENC_ChnStat_t stStat;
            VENCSSC339G* elem;
            uint8_t* data;
            size_t len;
            MI_VENC_CHN VencChn;

            
            elem = (VENCSSC339G*)para;
            for(int i = 0;i<MAX_VPE_CHN;i++)
            {
                VencChn = elem->gVencAttr[i].vencChn;
                // s32Ret = MI_VENC_GetChnDevid(VencChn, &u32DevId);
                // if(MI_SUCCESS != s32Ret)
                // {
                //     ST_INFO("MI_VENC_GetChnDevid %d error, %X\n", VencChn, s32Ret);
                // }
                printf("\n\n\n\n ======= get stream data ==========\n");
                printf("VencChn : %d\n",VencChn);
                while(getH264flag)
                {
                    memset(&stBufInfo, 0x0, sizeof(MI_SYS_BufInfo_t));
                    memset(&stStream, 0, sizeof(stStream));
                    memset(&stPack, 0, sizeof(stPack));
                    stStream.pstPack = &stPack;
                    stStream.u32PackCount = 1;
                    s32Ret = MI_VENC_Query(VencChn, &stStat);
                    // printf("stStat.u32CurPacks : %d \n",stStat.u32CurPacks);
                    if(s32Ret != MI_SUCCESS || stStat.u32CurPacks == 0)
                    {
                            usleep(1000);
                            continue;
                    }
                    s32Ret = MI_VENC_GetStream(VencChn, &stStream, 400);
                    if(MI_SUCCESS == s32Ret)
                    {
                        len = stStream.pstPack[0].u32Len;
                        data = (uint8_t* )malloc(len);
                        // printf("len : %d \n\n\n\n\n\n\n",len);
                        memcpy(data, stStream.pstPack[0].pu8Addr, len);

                        struct Buffer buf = {data,(size_t)len};
                        elem->pushNext(&buf);

                        s32Ret = MI_VENC_ReleaseStream(VencChn, &stStream);
                        if(s32Ret != MI_SUCCESS)
                        {
                            ST_WARN("RELEASE venc buffer fail\n");
                            printf("release VENC stream s32Ret : 0x%x \n",s32Ret);
                        }
                    }
                    else
                    {
                        printf("s32Ret : 0x%x \n",s32Ret);
                    }
                    free(data);
                }
                
            }
            return 0;    
        }
        int startSelf(){
            int ret = 0;
            MI_VENC_CHN VencChn;

            gVencAttr[0].s32vifDev = 0;
            gVencAttr[0].enInput = ST_Sys_Input_VPE;
            gVencAttr[0].u32InputChn = 0;
            gVencAttr[0].u32InputPort = 0;
            gVencAttr[0].vencChn = 0;
            gVencAttr[0].eType = E_MI_VENC_MODTYPE_H264E;
            // gVencAttr[0].u32Width = 1920;
            // gVencAttr[0].u32Height = 1080;
            gVencAttr[0].u32Width = 3840;
            gVencAttr[0].u32Height = 2160;
            gVencAttr[0].eBindSensorId = E_MI_VPE_SENSOR0;
            gVencAttr[0].pixelFormat = (MI_SYS_PixelFormat_e)35;

            gVencAttr[1].s32vifDev = 2;
            gVencAttr[1].enInput =ST_Sys_Input_VPE;
            gVencAttr[1].u32InputChn = 2;
            gVencAttr[1].u32InputPort = 0;
            gVencAttr[1].vencChn = 2;
            gVencAttr[1].eType = E_MI_VENC_MODTYPE_H264E;
            gVencAttr[1].u32Width = 1280;
            gVencAttr[1].u32Height = 720;


            MI_U32 u32VencDevId = 0xff;
            /************************************************
            Step4:  init VENC
            *************************************************/
            MI_VENC_ChnAttr_t stChnAttr;
            for(int i = 0;i<MAX_VPE_CHN;i++)
            {   
                VencChn = gVencAttr[i].vencChn;

                memset(&stChnAttr, 0x0, sizeof(MI_VENC_ChnAttr_t));
                MI_S32 u32VenBitRate = 1024 * 1024 * 2;
                stChnAttr.stVeAttr.stAttrH264e.u32PicWidth = gVencAttr[i].u32Width;
                stChnAttr.stVeAttr.stAttrH264e.u32PicHeight = gVencAttr[i].u32Height;
                stChnAttr.stVeAttr.stAttrH264e.u32MaxPicWidth = gVencAttr[i].u32Width;
                stChnAttr.stVeAttr.stAttrH264e.u32MaxPicHeight = gVencAttr[i].u32Height;
                stChnAttr.stVeAttr.stAttrH264e.u32BFrameNum = 2;
                stChnAttr.stVeAttr.stAttrH264e.bByFrame = TRUE;

                stChnAttr.stRcAttr.eRcMode = E_MI_VENC_RC_MODE_H264CBR;
                stChnAttr.stRcAttr.stAttrH264Cbr.u32BitRate = u32VenBitRate;
                stChnAttr.stRcAttr.stAttrH264Cbr.u32FluctuateLevel = 0;
                stChnAttr.stRcAttr.stAttrH264Cbr.u32Gop = 30;
                stChnAttr.stRcAttr.stAttrH264Cbr.u32SrcFrmRateNum = 30;
                stChnAttr.stRcAttr.stAttrH264Cbr.u32SrcFrmRateDen = 1;
                stChnAttr.stRcAttr.stAttrH264Cbr.u32StatTime = 0;
            
                stChnAttr.stVeAttr.eType = gVencAttr[i].eType;

                STCHECKRESULT(ST_Venc_CreateChannel(VencChn, &stChnAttr));
                STCHECKRESULT(ST_Venc_StartChannel(VencChn));

                printf("\n\n\n\n ============ init VENC =============\n");
                printf("stChnAttr.stVeAttr.stAttrH264e.u32PicWidth : %d \n",stChnAttr.stVeAttr.stAttrH264e.u32PicWidth);
                printf("stChnAttr.stVeAttr.stAttrH264e.u32PicHeight : %d \n",stChnAttr.stVeAttr.stAttrH264e.u32PicHeight);
                printf("stChnAttr.stVeAttr.stAttrH264e.u32MaxPicWidth : %d \n",stChnAttr.stVeAttr.stAttrH264e.u32MaxPicWidth);
                printf("stChnAttr.stVeAttr.stAttrH264e.u32MaxPicHeight : %d \n",stChnAttr.stVeAttr.stAttrH264e.u32MaxPicHeight);
                printf("stChnAttr.stVeAttr.stAttrH264e.u32BFrameNum : %d \n",stChnAttr.stVeAttr.stAttrH264e.u32BFrameNum );
                printf("stChnAttr.stVeAttr.stAttrH264e.bByFrame : %d \n",stChnAttr.stVeAttr.stAttrH264e.bByFrame);
                printf("stChnAttr.stRcAttr.eRcMode : %d \n",stChnAttr.stRcAttr.eRcMode);
                printf("stChnAttr.stRcAttr.stAttrH264Cbr.u32BitRate : %d \n",stChnAttr.stRcAttr.stAttrH264Cbr.u32BitRate);
                printf("stChnAttr.stRcAttr.stAttrH264Cbr.u32FluctuateLevel : %d \n",stChnAttr.stRcAttr.stAttrH264Cbr.u32FluctuateLevel);
                printf("stChnAttr.stRcAttr.stAttrH264Cbr.u32Gop : %d \n",stChnAttr.stRcAttr.stAttrH264Cbr.u32Gop);
                printf("stChnAttr.stRcAttr.stAttrH264Cbr.u32SrcFrmRateNum : %d \n",stChnAttr.stRcAttr.stAttrH264Cbr.u32SrcFrmRateNum);
                printf("stChnAttr.stRcAttr.stAttrH264Cbr.u32SrcFrmRateDen : %d \n",stChnAttr.stRcAttr.stAttrH264Cbr.u32SrcFrmRateDen);
                printf("stChnAttr.stRcAttr.stAttrH264Cbr.u32StatTime : %d \n",stChnAttr.stRcAttr.stAttrH264Cbr.u32StatTime);
                printf("stChnAttr.stVeAttr.eType : %d \n",stChnAttr.stVeAttr.eType);
            }
            pthread_create(&pthreadId,NULL,getH264data,(void *)this);

            return 0;
        }
        int linkTo(Element* elem)
        {
            z_unused(elem);

            return 0;
        }
        int pushData(void* data)
        {
            return MI_SUCCESS;
        }

    };
    // Element* findVencByHandle(int handle)
    // {
    //     return VENCSSC339G::findVencByHandle(handle);
    // }

    std::vector<VENCSSC339G*> VENCSSC339G::arr_VENCSSC339G= std::vector<VENCSSC339G*>(32);

    static int init()
    {
        return MI_SUCCESS;
    }
    static void deInit()
    {
        getH264flag = 0;
    }
    static ElementDescriptor elementDescriptors[] = {
        {
             "SSC339G",
             ELEMENT_CLASS_NAME,
             init,
             deInit,
             VENCSSC339G::createInstance
        },
    };
    REGISTER_ELEMENT(elementDescriptors,sizeof(elementDescriptors)/sizeof(elementDescriptors[0]));
}
