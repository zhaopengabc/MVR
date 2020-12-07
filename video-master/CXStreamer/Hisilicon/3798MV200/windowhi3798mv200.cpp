#include "hielement.h"
#include "../factory.h"
#include<string.h>
namespace CXS{
    #define ELEMENT_CLASS_NAME  "window"
    class WindowHi3798MV200 : public HiElement
    {
        HI_UNF_WINDOW_ATTR_S mAttr;

        WindowHi3798MV200():HiElement(ELEMENT_CLASS_NAME){
            mAttr.enDisp = (HI_UNF_DISP_E)1;
            mInPads.push_back(Pad(this));
            mOutPads.push_back(Pad(this));
        }
    public:
        static Element* createInstance()
        {
            WindowHi3798MV200* win = new WindowHi3798MV200();
            HI_S32 ret = HI_UNF_VO_CreateWindow  ( &win->mAttr,&win->mHandle);
            if(ret != HI_SUCCESS)
            {
                delete win;
                win = nullptr;
            }
            return win;
        }
        int startSelf(){
            HI_S32 ret = HI_SUCCESS;

//            if(HI_SUCCESS == HI_UNF_VO_SetWindowAttr( mHandle,&mAttr) &&
            if(1 ||
                    //HI_SUCCESS == HI_UNF_VO_AttachExternBuffer( mHandle,HI_UNF_BUFFER_ATTR_S *  pstBufAttr) &&
					HI_SUCCESS == HI_UNF_VO_SetWindowEnable(mHandle,HI_TRUE)
					)
                return 0;
            else
                return -1;
        }
        int linkTo(Element* elem)
        {
            HiElement* hiElem = dynamic_cast<HiElement*>(elem);
            HI_S32 ret = HI_SUCCESS;
            if(strcmp(hiElem->getClassName(),"disp") == 0)
            {
                mAttr.enDisp = (HI_UNF_DISP_E)hiElem->getHandle();
                printf("mAttr.enDisp %d\n",mAttr.enDisp);
                ret = HI_UNF_VO_SetWindowAttr(mHandle,&mAttr);
                printf("HI_UNF_VO_SetWindowAttr %d\n",ret);
            }
            return ret == HI_SUCCESS ? 0 : -1 ;
        }
    };
    static int init()
    {
//        printf("window init %d %d\n",HI_UNF_DISP_Init(),HI_UNF_VO_Init(HI_UNF_VO_DEV_MODE_NORMAL));
//        return -1;
        return HI_SUCCESS == HI_UNF_DISP_Init() && HI_SUCCESS == HI_UNF_VO_Init(HI_UNF_VO_DEV_MODE_NORMAL) ? 0 : -1;
    }
    static void deInit()
    {
        HI_UNF_VO_DeInit();
    }
    static ElementDescriptor elementDescriptors[] = {
        {
            .factoryName = "hi3798mv200",
            .elementClassName = ELEMENT_CLASS_NAME,
            .init = init,
            .deInit = deInit,
            .createElementInstance = WindowHi3798MV200::createInstance
        },
    };
    //REGISTER_ELEMENT(elementDescriptors,sizeof(elementDescriptors)/sizeof(elementDescriptors[0]));
}
