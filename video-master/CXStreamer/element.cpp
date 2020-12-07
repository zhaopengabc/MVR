#include "element.h"
namespace CXS{
    Element::Element(const char* className)
    {
        mClassName = className;
        m_state = Init;
    }

    Element::~Element()
    {

    }
}
