#include "../element.h"
#include "../factory.h"
#include "hielement.h"

#include <string.h>
namespace CXS{
    #define ELEMENT_CLASS_NAME  "vi"
    class VIHi3798MV200 : public HiElement
    {
        HI_UNF_VI_ATTR_S mAttr;
        HI_UNF_VI_E mViPort;
        VIHi3798MV200():HiElement(ELEMENT_CLASS_NAME){
            mViPort = HI_UNF_VI_PORT0;
            mAttr.bVirtual = HI_TRUE;
            mAttr.u32BufNum = 6;
            mOutPads.push_back(Pad(this));


        }
    public:
        static Element* createInstance()
        {
            VIHi3798MV200* vi = new VIHi3798MV200();
            HI_S32 ret = HI_UNF_VI_Create (vi->mViPort,&vi->mAttr,&vi->mHandle);
            if(ret != HI_SUCCESS){
                delete vi;
                vi = nullptr;
            }
            return vi;
        }
        int startSelf(){
            HI_S32 ret = HI_SUCCESS;
            ret = HI_UNF_VI_SetAttr ( mHandle,&mAttr);

            if(HI_SUCCESS != ret)
                return false;
            return HI_SUCCESS == HI_UNF_VI_Start(mHandle) ? 0 : -1;
        }
        int linkTo(Element* elem){
            HiElement* hiElem = dynamic_cast<HiElement*>(elem);
            HI_S32 ret;
            if(strcmp(hiElem->getClassName(),"window") == 0)
            {
                ret = HI_UNF_VO_AttachWindow(hiElem->getHandle(),mHandle);
            }
            return ret == HI_SUCCESS ? 0 : -1 ;
        }
        int setAttr(const char* attrName,bool attrVal)
        {
            if(strcmp(attrName,"Virtual") == 0 )
            {
                if(attrVal)
                {
                    mViPort = HI_UNF_VI_PORT0;
                    mAttr.bVirtual = HI_TRUE;
                    mAttr.u32BufNum = 6;
                }
                else
                {
                    mAttr.bVirtual = HI_FALSE;
                }
                HI_S32 ret = HI_UNF_VI_SetAttr ( mHandle,&mAttr);
                return ret ==HI_SUCCESS ? 0:-1;
            }
            else
                return -1;
        }
    };
    static int init()
    {
        return HI_SUCCESS == HI_UNF_VI_Init() ? 0:-1;
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
            .createElementInstance = VIHi3798MV200::createInstance
        },
    };
    //REGISTER_ELEMENT(elementDescriptors,sizeof(elementDescriptors)/sizeof(elementDescriptors[0]));
}
