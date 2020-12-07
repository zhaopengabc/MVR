#include <string.h>
#include "../../factory.h"
#include "../hielement.h"
#include "../cxs_hi_comm.h"
#include "hi3531dv100_lib.h"
#include"hi_config.h"
#include "common/sample_comm.h"
#include "zstd.h"
namespace CXS{
    #define ELEMENT_CLASS_NAME  "vo"
    class VOHi3531DV100 : public HiElement
    {
        VOHi3531DV100():HiElement(ELEMENT_CLASS_NAME){
        }
    public:
        static Element* createInstance()
        {
            VOHi3531DV100* elem = new VOHi3531DV100();
            return elem;
        }
        int startSelf(){
            int ret = HI_FAILURE;
            mHandle = getAttr("VO-DEV",0);
            ret = hi3531dv100_vo_start ( mHandle,1920,1080);
            if(ret == 0)
                CXS_PRT("VO(%d) start ok\n",mHandle);
            else{
                CXS_PRT("VO(%d) start failure\n",mHandle);
            }
            return ret;
        }
        int linkTo(Element* elem){
            z_unused(elem);
            HiElement* hiElem = dynamic_cast<HiElement*>(elem);
            HI_S32 ret = HI_FAILURE;
            if(strcmp(hiElem->getClassName(),"venc") == 0)
            {

                ret = SAMPLE_COMM_VENC_BindVo( mHandle ,  0,  hiElem->getHandle());
                if (HI_SUCCESS != ret)
                {
                         CXS_PRT("bind  Vo and venc failed!\n");
                }
                else
                {
                         CXS_PRT("bind  Vo and venc success!\n");
                }
            }

            return  ret;
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
             VOHi3531DV100::createInstance
        },
    };
    REGISTER_ELEMENT(elementDescriptors,sizeof(elementDescriptors)/sizeof(elementDescriptors[0]));
}
