#ifndef HIELEMENT_H
#define HIELEMENT_H
#include "../element.h"
// #include "hi_common.h"
//#include "hi_unf_vi.h"
//#include "hi_unf_vo.h"
#include<string>
namespace CXS{
    class HiElement : public Element
    {
    protected:
        int mHandle;
        HiElement(const char* className);
    public:
        int getHandle();
        //const char*getClassName();
    };
}
#endif // HIELEMENT_H
