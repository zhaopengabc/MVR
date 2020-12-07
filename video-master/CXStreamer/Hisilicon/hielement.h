#ifndef HIELEMENT_H
#define HIELEMENT_H
#include "../element.h"
#include "hi_common.h"
//#include "hi_unf_vi.h"
//#include "hi_unf_vo.h"
#include<string>
namespace CXS{
    class HiElement : public Element
    {
    protected:
        HI_S32 mHandle;
        HiElement(const char* className);
    public:
        HI_S32 getHandle();
        //const char*getClassName();
    };
}
#endif // HIELEMENT_H
