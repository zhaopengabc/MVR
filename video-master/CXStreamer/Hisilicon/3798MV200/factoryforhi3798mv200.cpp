#include "../factory.h"
#include "hi_common.h"
namespace CXS{
    class FactoryForHi3798MV200 : public Factory
    {
        FactoryForHi3798MV200(){
        }
        ~FactoryForHi3798MV200(){
        }

    public:
        DECLARE_FACTORY_GET_INSTANCE(FactoryForHi3798MV200)

    };
    static int init()
    {
        HI_S32 ret = HI_SYS_Init();
        HI_SYS_VERSION_S version;
        HI_SYS_GetVersion(&version);
        DEBUG("HI aVersion=%s\n",version.aVersion);
        DEBUG("HI BootVersion =%s\n",version.BootVersion );
        DEBUG("HI enChipTypeHardWare =%x\n",version.enChipTypeHardWare );
        DEBUG("HI enChipTypeSoft =%x\n",version.enChipTypeSoft );
        DEBUG("HI enChipVersion =%x\n",version.enChipVersion );
        return HI_SUCCESS == ret ? 0:-1;
    }
    static void deInit()
    {
        HI_SYS_DeInit();
    }

    static FactoryDescriptor factoryDescriptor{
        .factoryName = "hi3798mv200",
        .init = init,
        .deInit = deInit,
        .getFactoryInstance = FactoryForHi3798MV200::getInstance
    };
    REGISTER_FACTORY(&factoryDescriptor);

}
