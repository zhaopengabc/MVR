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
}

namespace CXS
{
#define ELEMENT_CLASS_NAME "venc"
#define MAX_VPE_CHN 1

    struct Buffer
    {
        uint8_t *buffer;
        size_t len;
    };
    int getH264flag = 1;
    static void *getH264data(void *para);

    class VENCSSC339G : public HiElement
    {
        typedef struct ty_resolution
        {
            MI_U32 width;
            MI_U32 height;
        } TY_RESOLUTION;

        static std::vector<VENCSSC339G *> arr_VENCSSC339G;
        VENCSSC339G() : HiElement(ELEMENT_CLASS_NAME)
        {
        }

    public:
        pthread_t pthreadId;

        static Element *createInstance()
        {
            static bool inited = false;
            VENCSSC339G *elem = new VENCSSC339G();

            if (!inited)
            {
                inited = true;
            }
            return elem;
        }
        static void *getH264data(void *para)
        {
            MI_SYS_BufInfo_t stBufInfo;
            MI_S32 s32Ret = MI_SUCCESS;
            MI_U32 u32DevId = 0;
            MI_VENC_Stream_t stStream;
            MI_VENC_Pack_t stPack;
            MI_VENC_ChnStat_t stStat;
            VENCSSC339G *elem;
            uint8_t *data;
            size_t len;
            MI_VENC_CHN VencChn;

            elem = (VENCSSC339G *)para;
            for (int i = 0; i < MAX_VPE_CHN; i++)
            {
                VencChn = atoi(elem->getAttr("vencChn", "0").c_str());
                while (getH264flag)
                {
                    memset(&stBufInfo, 0x0, sizeof(MI_SYS_BufInfo_t));
                    memset(&stStream, 0, sizeof(stStream));
                    memset(&stPack, 0, sizeof(stPack));
                    stStream.pstPack = &stPack;
                    stStream.u32PackCount = 1;
                    s32Ret = MI_VENC_Query(VencChn, &stStat);
                    if (s32Ret != MI_SUCCESS || stStat.u32CurPacks == 0)
                    {
                        usleep(1000);
                        continue;
                    }
                    s32Ret = MI_VENC_GetStream(VencChn, &stStream, 400);
                    if (MI_SUCCESS == s32Ret)
                    {
                        len = stStream.pstPack[0].u32Len;
                        data = (uint8_t *)malloc(len);
                        memcpy(data, stStream.pstPack[0].pu8Addr, len);
                        // printf("len : %d \n",len);
                        struct Buffer buf = {data, (size_t)len};
                        elem->pushNext(&buf);

                        s32Ret = MI_VENC_ReleaseStream(VencChn, &stStream);
                        if (s32Ret != MI_SUCCESS)
                        {
                            ST_WARN("RELEASE venc buffer fail\n");
                        }
                    }
                    else
                    {
                        printf("s32Ret : 0x%x \n", s32Ret);
                    }
                    free(data);
                }
            }
            return 0;
        }
        int startSelf()
        {
            int ret = 0;
            MI_VENC_CHN VencChn;
            MI_U32 u32VencDevId = 0xff;
            /************************************************
            Step4:  init VENC
            *************************************************/
            MI_VENC_ChnAttr_t stChnAttr;
            std::string tmpResolution;
            TY_RESOLUTION maxResolution;
            TY_RESOLUTION vpeResolution;

            std::string compressionType;
            MI_VENC_RcMode_e eRcMode;
            MI_VENC_ModType_e eType;

            VencChn = atoi(this->getAttr("vencChn", "0").c_str());
            tmpResolution = this->getAttr("resolution", "0");
            printf("tmpResolution : %s \n",tmpResolution.c_str());
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
            compressionType = this->getAttr("compressionType", "H264");
            if (compressionType == "H264")
            {
                eRcMode = E_MI_VENC_RC_MODE_H264CBR;
                eType = E_MI_VENC_MODTYPE_H264E;
            }
            else if (compressionType == "H265")
            {
                eRcMode = E_MI_VENC_RC_MODE_H265CBR;
                eType = E_MI_VENC_MODTYPE_H265E;
            }

            memset(&stChnAttr, 0x0, sizeof(MI_VENC_ChnAttr_t));
            MI_S32 u32VenBitRate = 1024 * 1024 * 2;
            stChnAttr.stVeAttr.stAttrH264e.u32PicWidth = vpeResolution.width;
            stChnAttr.stVeAttr.stAttrH264e.u32PicHeight = vpeResolution.height;
            stChnAttr.stVeAttr.stAttrH264e.u32MaxPicWidth = maxResolution.width;
            stChnAttr.stVeAttr.stAttrH264e.u32MaxPicHeight = maxResolution.height;
            stChnAttr.stVeAttr.stAttrH264e.u32BFrameNum = 2;
            stChnAttr.stVeAttr.stAttrH264e.bByFrame = TRUE;

            stChnAttr.stRcAttr.eRcMode = eRcMode;
            stChnAttr.stRcAttr.stAttrH264Cbr.u32BitRate = u32VenBitRate;
            stChnAttr.stRcAttr.stAttrH264Cbr.u32FluctuateLevel = 0;
            stChnAttr.stRcAttr.stAttrH264Cbr.u32Gop = 30;
            stChnAttr.stRcAttr.stAttrH264Cbr.u32SrcFrmRateNum = 20;
            stChnAttr.stRcAttr.stAttrH264Cbr.u32SrcFrmRateDen = 1;
            stChnAttr.stRcAttr.stAttrH264Cbr.u32StatTime = 0;
            stChnAttr.stVeAttr.eType = eType;

            STCHECKRESULT(ST_Venc_CreateChannel(VencChn, &stChnAttr));
            STCHECKRESULT(ST_Venc_StartChannel(VencChn));
            pthread_create(&pthreadId, NULL, getH264data, (void *)this);

            return 0;
        }
        int linkTo(Element *elem)
        {
            z_unused(elem);

            return 0;
        }
        int pushData(void *data)
        {
            return MI_SUCCESS;
        }
    };

    std::vector<VENCSSC339G *> VENCSSC339G::arr_VENCSSC339G = std::vector<VENCSSC339G *>(32);

    static int init()
    {
        return MI_SUCCESS;
    }
    static void deInit()
    {
        getH264flag = 0;
    }
    static ElementDescriptor elementDescriptors[] = {
        {"SSC339G",
         ELEMENT_CLASS_NAME,
         init,
         deInit,
         VENCSSC339G::createInstance},
    };
    REGISTER_ELEMENT(elementDescriptors, sizeof(elementDescriptors) / sizeof(elementDescriptors[0]));
} // namespace CXS
