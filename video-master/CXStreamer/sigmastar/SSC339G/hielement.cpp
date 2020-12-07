#include "hielement.h"
namespace CXS{
    HiElement::HiElement(const char* className):Element(className)
    {
        mHandle = 0;
    }
    int HiElement::getHandle()
    {
        return mHandle;
    }
}
