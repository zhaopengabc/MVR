#include <string.h>
#include "../../factory.h"
#include "../hielement.h"
#include "../cxs_hi_comm.h"
#include "hi3531dv100_lib.h"
#include "zstd.h"
#include"mpi_venc.h"
#include <iostream>
extern "C"
{
extern void SetH264Processor(void (*processor)(int vencChn,unsigned char* buffer,int len));
}

namespace CXS{
    #define ELEMENT_CLASS_NAME  "venc"
    class VEncHi3531DV100 : public HiElement
    {
    //        std::string m_mode;
    //        std::string m_payload;
    //        int m_frame_rate;
    //        int m_src_frame_rate;
        static std::vector<VEncHi3531DV100*> arr_VEncHi3531DV100;
        VEncHi3531DV100():HiElement(ELEMENT_CLASS_NAME){
            for(int i =0;i < 32;i++){
                if(arr_VEncHi3531DV100[i] == nullptr){
                    mHandle = i;
                    break;
                }
            }
            arr_VEncHi3531DV100[mHandle] = this;
            setAttr("VENC-CHN",mHandle);
            setAttr("VENC-SRC-FRAMERATE",60);
            setAttr("VENC-FRAMERATE",15);
            setAttr("VENC-MODE","HD1080");
            setAttr("VENC-PAYLOAD","h264");
        }

        struct Buffer{
            uint8_t* buffer;
            size_t len;
        };
        static void h264_processor(int vencChn,unsigned char* buffer,int len)
        {
            std::cout<<"arr_VEncHi3531DV100.size="<<arr_VEncHi3531DV100.size()<<std::endl;
            std::cout<<"arr_VEncHi3531DV100["<<vencChn<<"]"<<arr_VEncHi3531DV100[vencChn]<<std::endl;
            VEncHi3531DV100* venc = arr_VEncHi3531DV100[vencChn];
            struct Buffer buf = {buffer,(size_t)len};
            venc->pushNext(&buf);
        }
    public:
        static Element* createInstance()
        {
            static bool inited = false;
            if(!inited)
            {
                inited = true;
                SetH264Processor(h264_processor);
            }
            VEncHi3531DV100* elem = new VEncHi3531DV100();
            return elem;
        }
        static Element* findVencByHandle(int handle)
        {
            if(handle < (int)arr_VEncHi3531DV100.size() && arr_VEncHi3531DV100[handle] != nullptr)
                return arr_VEncHi3531DV100[handle];
            else
                return nullptr;
        }

        int startSelf(){
            int ret = HI_FAILURE;
            HI_S32 handle = getAttr("VENC-CHN",-1);
            if(handle == -1)
                return ret;
            if(handle != mHandle){
                if(arr_VEncHi3531DV100[handle] != nullptr)
                    return ret;
                arr_VEncHi3531DV100[mHandle] = nullptr;
                arr_VEncHi3531DV100[handle] = this;
                mHandle = handle;
            }
            ret = hi3531dv100_venc_start ( mHandle);
            if(ret == 0)
            {
                CXS_PRT("VENC(%d) start ok\n",mHandle);
            }
            return ret;
        }
        int linkTo(Element* elem){
            z_unused(elem);
            return  0  ;
        }
        int pushData(void* data){
            if(m_state == Starting){
               HI_MPI_VENC_SendFrame(mHandle, (VIDEO_FRAME_INFO_S*)data,10);
            }
            return HI_SUCCESS;
        }

    };
    Element* findVencByHandle(int handle){
        return VEncHi3531DV100::findVencByHandle(handle);
    }

    std::vector<VEncHi3531DV100*> VEncHi3531DV100::arr_VEncHi3531DV100 = std::vector<VEncHi3531DV100*>(32);

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
             VEncHi3531DV100::createInstance
        },
    };
    REGISTER_ELEMENT(elementDescriptors,sizeof(elementDescriptors)/sizeof(elementDescriptors[0]));
}
