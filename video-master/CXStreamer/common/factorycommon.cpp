#include <string.h>
#include "../factory.h"


namespace CXS{
    class FactoryCommon : public Factory
    {
        FactoryCommon()
        {
        }
        ~FactoryCommon()
        {
        }

    public:
        DECLARE_FACTORY_GET_INSTANCE(FactoryCommon)

    };
    static int init()
    {
        return 0;
    }
    static void deInit()
    {
    }
    static struct FactoryDescriptor factoryDescriptor  = {
        "common",
         init,
        deInit,
        FactoryCommon::getInstance
    };

    REGISTER_FACTORY(&factoryDescriptor);

}
