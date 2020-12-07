#include "hielement.h"
#include "hi_unf_disp.h"
#include "../factory.h"
#include "../cxstreamer.h"
namespace CXS{
    class DispHi3798MV200 : public HiElement
    {
		#define ELEMENT_CLASS_NAME  "disp"
        DispHi3798MV200():HiElement(ELEMENT_CLASS_NAME){
            mHandle = HI_UNF_DISPLAY1;
            mIntf.enIntfType =HI_UNF_DISP_INTF_TYPE_HDMI;
            mIntf.unIntf.enHdmi = HI_UNF_HDMI_ID_0 ;
            mInPads.push_back(Pad(this));

        }
        ~DispHi3798MV200(){
            HI_UNF_DISP_Close ( (HI_UNF_DISP_E)  mHandle   );
        }

        HI_UNF_DISP_INTF_S  mIntf;
    public:
        static Element* createInstance()
        {
            DispHi3798MV200* elem= new DispHi3798MV200();
            HI_S32 ret= HI_UNF_DISP_AttachIntf((HI_UNF_DISP_E )elem->mHandle,&elem->mIntf,1);
            if(ret != HI_SUCCESS)
            {
                DEBUG("disp create ret=%d\n",ret);
                delete elem;
                elem = nullptr;
            }
            return elem;
        }

        int startSelf(){

            return HI_SUCCESS == HI_UNF_DISP_Open((HI_UNF_DISP_E)mHandle);
        }
    };
    static int init()
    {
        return HI_SUCCESS == HI_UNF_DISP_Init() ? 0: -1;
    }
    static void deInit()
    {
        HI_UNF_DISP_DeInit();
    }
    static ElementDescriptor elementDescriptors[] = {
        {
            .factoryName = "hi3798mv200",
            .elementClassName = ELEMENT_CLASS_NAME,
            .init = init,
            .deInit = deInit,
            .createElementInstance = DispHi3798MV200::createInstance
        },
    };
    //REGISTER_ELEMENT(elementDescriptors,sizeof(elementDescriptors)/sizeof(elementDescriptors[0]));
}
