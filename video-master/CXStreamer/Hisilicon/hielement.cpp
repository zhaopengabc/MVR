#include "hielement.h"
namespace CXS{
    HiElement::HiElement(const char* className):Element(className)
    {
        mHandle = 0;
    }
    HI_S32 HiElement::getHandle()
    {
        return mHandle;
    }
//    const char*HiElement::getClassName()
//    {
//        return hiClassName.data();
//    }
}
