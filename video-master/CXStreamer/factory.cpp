#include "factory.h"
namespace CXS{
    static std::map<std::string,FactoryDescriptor*>& getFactoryMap(){
        static std::map<std::string,FactoryDescriptor*>* map = nullptr;
        if(map == nullptr)
            map = new  std::map<std::string,FactoryDescriptor*>;
        return *map;
    }
    static std::list<ElementDescriptor*>& getElemDescList(){
        static std::list<ElementDescriptor*>* list = nullptr;
        if(list == nullptr)
            list = new  std::list<ElementDescriptor*>;
        return *list;
    }
    bool Factory::isInit = false;
    Factory::Factory()
    {

    }

    int Factory::registerFactory(FactoryDescriptor* descriptor)
    {
        std::map<std::string,FactoryDescriptor*>& factoryMap = getFactoryMap();
        if(descriptor->init && 0 != descriptor->init())
        {
            CXS_PRT("factory(%s) register failure\n",descriptor->factoryName);
            return -1;
        }
        factoryMap.insert(std::pair< std::string, FactoryDescriptor*>(descriptor->factoryName, descriptor));
        return 0;
    }
    Factory* Factory::getFactoryInstanceByName(const char* factoryName)
    {
        std::map<std::string,FactoryDescriptor*>& factoryMap = getFactoryMap();
        if(factoryMap[factoryName] != nullptr)
            return factoryMap[factoryName]->getFactoryInstance();

        return  nullptr;
    }


    void Factory::addElementDesc(ElementDescriptor* descriptor)
    {
        std::list<ElementDescriptor*>& elemDescList = getElemDescList();
        elemDescList.push_back(descriptor);
    }
    void Factory::registerElement(ElementDescriptor* descriptor)
    {
        if(descriptor->init && 0 != descriptor->init())
        {
            CXS_PRT("element(%s) register failure\n",descriptor->elementClassName);
            return;
        }
        elemDescGetFuncPtrMap.insert(std::pair< std::string, ElementDescriptor*>(descriptor->elementClassName, descriptor));
    }
    int Factory::init()
    {
        if(isInit == true)
            return 0;
        std::list<ElementDescriptor*>& elemDescList = getElemDescList();
        for(auto it = elemDescList.begin();it != elemDescList.end();it++)
        {
            Factory::getFactoryInstanceByName((*it)->factoryName)->registerElement(*it);
        }
        isInit = true;
        return 0;
    }
}
