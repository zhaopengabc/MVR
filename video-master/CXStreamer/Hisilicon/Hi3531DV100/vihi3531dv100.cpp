#include <string.h>
#include "../../factory.h"
#include "../hielement.h"
#include "../cxs_hi_comm.h"
#include "hi3531dv100_lib.h"
#include "common/sample_comm.h"
#include <pthread.h>
#include <iostream>
#include <unistd.h>
namespace CXS{
    #define ELEMENT_CLASS_NAME  "vi"
    class VIHi3531DV100 : public HiElement
    {
        pthread_t mFrameThread;
        VIHi3531DV100():HiElement(ELEMENT_CLASS_NAME){
            setAttr("VI-MODE","HD1080");
        }
        typedef struct UserData{
            HI_S32 mViChn;
            VIHi3531DV100* mViElem;
            UserData(HI_S32 viChn,VIHi3531DV100* viElem){
                mViChn = viChn;
                mViElem = viElem;
            }
        }UserData;

    public:


    private:
        static void* frameThread(void* data){
            UserData* ud = (UserData*)data;
            HI_S32 viChn = ud->mViChn;
            VIHi3531DV100* viElem = ud->mViElem;
#define MAX_FRAME_NUM 8
            VIDEO_FRAME_INFO_S stFrame[MAX_FRAME_NUM];

            uint64_t ptr = viElem->getAttr("ON-NEW-FRAME",(uint64_t)nullptr);
            void (*mNewFrameHandler)(VIDEO_FRAME_INFO_S* event_para) = (void (*)(VIDEO_FRAME_INFO_S* ))ptr;
            while(true){
                int frame_num = 0;
                while(frame_num < MAX_FRAME_NUM && HI_SUCCESS == HI_MPI_VI_GetFrame(viChn, &stFrame[frame_num],10)){
                    frame_num++;
                }
                if(frame_num == 0){
                    //usleep(1000);
                    continue;
                }
                int cur = frame_num-1;

                for(int i = 0;i < cur;i++){
                    HI_MPI_VI_ReleaseFrame(viChn,&stFrame[i]);
                }

                //stFrame.stVFrame.enPixelFormat == PIXEL_FORMAT_YUV_SEMIPLANAR_420;
                for(int i =0;i < 3;i++){
                    if(stFrame[cur].stVFrame.u32PhyAddr[i])
                        stFrame[cur].stVFrame.pVirAddr[i] = (uint8_t *)HI_MPI_SYS_Mmap(stFrame[cur].stVFrame.u32PhyAddr[i], stFrame[cur].stVFrame.u32Stride[i]*stFrame[cur].stVFrame.u32Height);
                }

                if(mNewFrameHandler){
                    mNewFrameHandler(&stFrame[cur]);
                }
                for(int i =0;i < 3;i++){
                    if(stFrame[cur].stVFrame.pVirAddr[i])
                        HI_MPI_SYS_Munmap((void*)stFrame[cur].stVFrame.pVirAddr[i], stFrame[cur].stVFrame.u32Stride[i]*stFrame[cur].stVFrame.u32Height);
                }
                HI_MPI_VI_ReleaseFrame(viChn,&stFrame[cur]);
            }
            return NULL;
        }
    public:
        static Element* createInstance()
        {
            VIHi3531DV100* vi = new VIHi3531DV100();
            return vi;
        }
        int startSelf(){
            int ret = HI_SUCCESS;
            mHandle = getAttr("VI-CHN",0);
            std::string mode = getAttr("VI-MODE","HD1080");
            int width = 1920;
            int height = 1080;
            if(mode == "UHD4K")
            {
                width = 3840;
                height = 2160;
            }
            ret = hi3531dv100_vi_start ( mHandle,width,height);
            if(ret == 0){
                CXS_PRT("VI(%d) start ok\n",mHandle);
                pthread_create(&mFrameThread,NULL,frameThread,(void*)new UserData(mHandle,this));
            }
            return ret;
        }
        int linkTo(Element* elem){
            HiElement* hiElem = dynamic_cast<HiElement*>(elem);
            HI_S32 ret = HI_SUCCESS;
            if(strcmp(hiElem->getClassName(),"vo") == 0)
            {
                ret = SAMPLE_COMM_VO_BindVi( hiElem->getHandle(),  0,  mHandle);
                if (HI_SUCCESS != ret)
                {
                         CXS_PRT("bind  VI and VO failed %x!\n",ret);
                }
            }
            else if(strcmp(hiElem->getClassName(),"venc") == 0)
            {
                ret = SAMPLE_COMM_VENC_BindVi(hiElem->getHandle(),  mHandle);
                if (HI_SUCCESS != ret)
                {
                         CXS_PRT("bind  Venc and vi failed!\n");
                }
            }
            else if(strcmp(hiElem->getClassName(),"vpss") == 0)
            {
                HI_S32 ViChn = mHandle;
                MPP_CHN_S stSrcChn;
                MPP_CHN_S stDestChn;
                memset(&stSrcChn,0,sizeof(MPP_CHN_S));
                memset(&stDestChn,0,sizeof(MPP_CHN_S));

                stSrcChn.enModId = HI_ID_VIU;
                stSrcChn.s32DevId = 0;
                stSrcChn.s32ChnId = ViChn;

                stDestChn.enModId = HI_ID_VPSS;
                stDestChn.s32DevId = hiElem->getHandle();
                stDestChn.s32ChnId = 0;

                ret = HI_MPI_SYS_Bind(&stSrcChn, &stDestChn);
                if (ret != HI_SUCCESS)
                {
                    CXS_PRT("vi binding vpss failed with %#x!\n", ret);
                    return HI_FAILURE;
                }
                else{
                    CXS_PRT("vi binding vpss success with %#x!\n", ret);
                }
            }
            return ret == HI_SUCCESS ? 0 : -1 ;
        }
    };
    static int init()
    {
        return HI_SUCCESS;
    }
    static void deInit()
    {

    }
    static ElementDescriptor elementDescriptors[] = {
        {
             "hi3531dv100",
             ELEMENT_CLASS_NAME,
             init,
             deInit,
             VIHi3531DV100::createInstance
        },
    };
    REGISTER_ELEMENT(elementDescriptors,sizeof(elementDescriptors)/sizeof(elementDescriptors[0]));
}
