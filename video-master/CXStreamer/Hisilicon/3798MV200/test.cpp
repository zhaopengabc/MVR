
#include "hi_unf_disp.h"
#include "hi_unf_mce.h"

#include "test.h"
#include "../element.h"
#include "../factory.h"
#include "hielement.h"
#include "hi_unf_avplay.h"
//#include "hi_adp.h"
#include <string.h>
namespace CXS{
#define ELEMENT_CLASS_NAME  "test"

    class Test : public HiElement
    {
        HI_UNF_VI_ATTR_S mAttr;
        HI_UNF_VI_E mViPort;

        HI_HANDLE hWindow;
        HI_UNF_WINDOW_ATTR_S mAttrWin;

        HI_UNF_DISP_E mDispChannel;
        HI_UNF_DISP_INTF_S  mIntf;
    public:
        Test():HiElement(ELEMENT_CLASS_NAME){
            memset(&mAttr,0,sizeof(mAttr));
            memset(&mAttrWin,0,sizeof(mAttrWin));
            memset(&mIntf,0,sizeof(mIntf));
            mViPort = HI_UNF_VI_PORT0;
            //mAttr.bVirtual = HI_TRUE;
            //mAttr.u32BufNum = 4;
            mAttr.bVirtual = HI_TRUE;
//            mAttr.stInputRect.s32X = 0;
//            mAttr.stInputRect.s32Y = 0;
//            mAttr.stInputRect.s32Width  = 640;
//            mAttr.stInputRect.s32Height = 480;
//            mAttr.enVideoFormat = HI_UNF_FORMAT_YUV_PACKAGE_YUYV;//HI_UNF_FORMAT_YUV_SEMIPLANAR_420;
//            mAttr.enBufMgmtMode = HI_UNF_VI_BUF_ALLOC;
            mAttr.u32BufNum = 6;

            mAttrWin.enDisp = HI_UNF_DISPLAY1;
            mAttrWin.enVideoFormat = HI_UNF_FORMAT_YUV_PACKAGE_YUYV;

            mDispChannel = HI_UNF_DISPLAY1;
            mIntf.enIntfType =HI_UNF_DISP_INTF_TYPE_HDMI;
            mIntf.unIntf.enHdmi = HI_UNF_HDMI_ID_0 ;

        }
        static HI_S32 clear_mce()
        {
            HI_S32                  Ret;
            HI_UNF_MCE_STOPPARM_S   stStop;

            Ret = HI_UNF_MCE_Init(HI_NULL);
            if (HI_SUCCESS != Ret)
            {
                printf("call HI_UNF_MCE_Init failed, Ret=%#x!\n", Ret);
                return Ret;
            }

            Ret = HI_UNF_MCE_ClearLogo();
            if (HI_SUCCESS != Ret)
            {
                printf("call HI_UNF_MCE_ClearLogo failed, Ret=%#x!\n", Ret);
                return Ret;
            }

            stStop.enStopMode = HI_UNF_AVPLAY_STOP_MODE_STILL;
            stStop.enCtrlMode = HI_UNF_MCE_PLAYCTRL_BY_TIME;
            stStop.u32PlayTimeMs = 0;
            Ret = HI_UNF_MCE_Stop(&stStop);
            if (HI_SUCCESS != Ret)
            {
                printf("call HI_UNF_MCE_Stop failed, Ret=%#x!\n", Ret);
                return Ret;
            }

            Ret = HI_UNF_MCE_Exit(HI_NULL);
            if (HI_SUCCESS != Ret)
            {
                printf("call HI_UNF_MCE_Exit failed, Ret=%#x!\n", Ret);
                return Ret;
            }

            HI_UNF_MCE_DeInit();
            return 0;
        }
        static void configHDMI()
        {
            HI_UNF_HDMI_DELAY_S stDelay;
            HI_UNF_HDMI_Init();
            HI_UNF_HDMI_GetDelay(HI_UNF_HDMI_ID_0,&stDelay);
            stDelay.bForceFmtDelay = HI_TRUE;
                stDelay.bForceMuteDelay = HI_TRUE;

            #if defined(CHIP_TYPE_hi3798mv200) || defined(CHIP_TYPE_hi3798mv300)
                stDelay.u32FmtDelay  = 1;
                stDelay.u32MuteDelay = 50;
            #else
                stDelay.u32FmtDelay = 500;
                stDelay.u32MuteDelay = 120;
            #endif
                HI_UNF_HDMI_SetDelay(HI_UNF_HDMI_ID_0,&stDelay);
                HI_UNF_HDMI_OPEN_PARA_S stOpenParam;
                stOpenParam.enDefaultMode = HI_UNF_HDMI_DEFAULT_ACTION_HDMI;//HI_UNF_HDMI_FORCE_NULL;
                HI_UNF_HDMI_Open(HI_UNF_HDMI_ID_0, &stOpenParam);

        }
        static int createWin(HI_RECT_S *pstWinRect,HI_HANDLE *phWin)
        {
            HI_S32 Ret;
                HI_UNF_WINDOW_ATTR_S   WinAttr;
                memset(&WinAttr, 0, sizeof(HI_UNF_WINDOW_ATTR_S));
                WinAttr.enDisp = HI_UNF_DISPLAY1;
                WinAttr.bVirtual = HI_FALSE;
                WinAttr.stWinAspectAttr.enAspectCvrs = HI_UNF_VO_ASPECT_CVRS_IGNORE;
                WinAttr.stWinAspectAttr.bUserDefAspectRatio = HI_FALSE;
                WinAttr.stWinAspectAttr.u32UserAspectWidth  = 0;
                WinAttr.stWinAspectAttr.u32UserAspectHeight = 0;
                WinAttr.bUseCropRect = HI_FALSE;
                WinAttr.stInputRect.s32X = 0;
                WinAttr.stInputRect.s32Y = 0;
                WinAttr.stInputRect.s32Width = 500;
                WinAttr.stInputRect.s32Height = 500;
                WinAttr.enVideoFormat = HI_UNF_FORMAT_YUV_PACKAGE_YUYV;
                if (HI_NULL == pstWinRect)
                {
                    memset(&WinAttr.stOutputRect, 0x0, sizeof(HI_RECT_S));
                }
                else
                {
                    memcpy(&WinAttr.stOutputRect,pstWinRect,sizeof(HI_RECT_S));
                }

                Ret = HI_UNF_VO_CreateWindow(&WinAttr, phWin);
                if (Ret != HI_SUCCESS)
                {
                    printf("call HI_UNF_VO_CreateWindow failed.\n");
                }
                return Ret;

        }
        static Element* createInstance()
        {
            HI_S32 ret;
            Test* e = new Test();

            clear_mce();
            HI_UNF_DISP_Init();
            ret= HI_UNF_DISP_AttachIntf(HI_UNF_DISPLAY1,&e->mIntf,1);
            if(ret != HI_SUCCESS){
                delete e;
                e = nullptr;
                goto err;
            }
            printf("222222222222222\n");
            HI_UNF_DISP_SetFormat(HI_UNF_DISPLAY1, HI_UNF_ENC_FMT_1080P_24);
            HI_UNF_DISP_BG_COLOR_S  BgColor;
            BgColor.u8Red   = 255;
                BgColor.u8Green = 0;
                BgColor.u8Blue  = 0;

                ret = HI_UNF_DISP_SetBgColor(HI_UNF_DISPLAY1, &BgColor);
                if(ret != HI_SUCCESS){
                    delete e;
                    e = nullptr;
                    goto err;
                }
            ret = HI_UNF_DISP_Open(HI_UNF_DISPLAY1);
            if(ret != HI_SUCCESS){
                delete e;
                e = nullptr;
                goto err;
            }
            configHDMI();

            HI_UNF_VO_Init(HI_UNF_VO_DEV_MODE_NORMAL);


            HI_RECT_S stRectViVo ;
            memset(&stRectViVo,0,sizeof(stRectViVo));
            stRectViVo.s32X = 640;
                stRectViVo.s32Y = 100;
                stRectViVo.s32Width  = 640;
                stRectViVo.s32Height = 480;
                ret = createWin(&stRectViVo, &e->hWindow);
                if(ret != HI_SUCCESS){
                    delete e;
                    e = nullptr;
                    goto err;
                }

                HI_UNF_VI_Init();
            printf("111111111111\n");
            ret = HI_UNF_VI_Create (e->mViPort,&e->mAttr,&e->mHandle);
            if(ret != HI_SUCCESS){
                delete e;
                e = nullptr;
                goto err;
            }

//                ret  = HIADP_AVPlay_RegADecLib();
//                if(ret != HI_SUCCESS){
//                    delete e;
//                    e = nullptr;
//                    goto err;
//                }
//                    ret = HI_UNF_AVPLAY_Init();
//                    if(ret != HI_SUCCESS){
//                        delete e;
//                        e = nullptr;
//                        goto err;
//                    }
//                    HI_UNF_AVPLAY_ATTR_S AvplayAttr;
//                    HI_UNF_AVPLAY_GetDefaultConfig(&AvplayAttr, HI_UNF_AVPLAY_STREAM_TYPE_ES);
//                    AvplayAttr.stStreamAttr.u32VidBufSize = 0x300000;
//                     ret = HI_UNF_AVPLAY_Create(&AvplayAttr, &e->mHandle);
//                     if (ret != HI_SUCCESS)
//                     {
//                         printf("call HI_UNF_AVPLAY_Create failed.\n");
//                         delete e;
//                         e = nullptr;
//                         goto err;
//                     }

//                     ret  = HI_UNF_AVPLAY_ChnOpen(e->mHandle, HI_UNF_AVPLAY_MEDIA_CHAN_VID, HI_NULL);
//                     if (HI_SUCCESS != ret)
//                     {
//                         printf("call HI_UNF_AVPLAY_ChnOpen failed.\n");
//                         delete e;
//                         e = nullptr;
//                         goto err;
//                     }






printf("444444444444\n");
            ret = HI_UNF_VO_AttachWindow(e->hWindow,e->mHandle);
            if(ret != HI_SUCCESS){
                delete e;
                e = nullptr;
                goto err;
            }

            ret = HI_UNF_VO_SetWindowEnable(e->hWindow,HI_TRUE);
            if(ret != HI_SUCCESS){
                delete e;
                e = nullptr;
            }

            ret = HI_UNF_VO_SetQuickOutputEnable(e->hWindow, HI_TRUE);
            if(ret != HI_SUCCESS){
                delete e;
                e = nullptr;
            }

//            HI_UNF_AVPLAY_LOW_DELAY_ATTR_S stLowDelay;
//            ret = HI_UNF_AVPLAY_GetAttr(e->mHandle, HI_UNF_AVPLAY_ATTR_ID_LOW_DELAY, &stLowDelay);
//                stLowDelay.bEnable = HI_TRUE;
//                ret = HI_UNF_AVPLAY_SetAttr(e->mHandle, HI_UNF_AVPLAY_ATTR_ID_LOW_DELAY, &stLowDelay);
//                HIAPI_RUN(HIADP_AVPlay_SetVdecAttr(hAvplay, enVdecFmt, HI_UNF_VCODEC_MODE_NORMAL), ret);


//                HI_UNF_AVPLAY_Start(e->mHandle, HI_UNF_AVPLAY_MEDIA_CHAN_VID, NULL);
//            printf("555555555555\n");
            ret = HI_UNF_VI_Start(e->mHandle);
            if(ret != HI_SUCCESS){
                delete e;
                e = nullptr;
                goto err;
            }
            //ret = HI_UNF_VO_ResetWindow(e->hWindow,HI_UNF_WINDOW_FREEZE_MODE_BLACK );
            if(ret != HI_SUCCESS){
                delete e;
                e = nullptr;
                goto err;
            }
            printf("66666666666\n");
           err:
            return e;
        }
    };
    static int init()
    {
//        HI_UNF_VI_Init();
//        HI_UNF_DISP_Init();
//        HI_UNF_VO_Init(HI_UNF_VO_DEV_MODE_NORMAL);
        return 0;
    }
    static void deInit()
    {
        HI_UNF_VI_DeInit();
    }
    static ElementDescriptor elementDescriptors[] = {
        {
            .factoryName = "hi3798mv200",
            .elementClassName = ELEMENT_CLASS_NAME,
            .init = init,
            .deInit = deInit,
            .createElementInstance = Test::createInstance
        },
    };
    REGISTER_ELEMENT(elementDescriptors,sizeof(elementDescriptors)/sizeof(elementDescriptors[0]));
}

