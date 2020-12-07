#ifndef FACTORY_H
#define FACTORY_H
#include<map>
#include<list>
#include<string>
#include "element.h"
#include "cxs_comm.h"
#include"cxstreamer.h"
namespace CXS{

    class Factory;

    /**************************************************************************
      * @brief  工厂描述结构
      * @note   提供工厂的名称、初始化方法、逆初始化方法和获得工厂实例方法
    **************************************************************************/
    struct FactoryDescriptor{
        const char* factoryName;
        /**************************************************************************
          * @brief  初始化方法
          * @return 0 成功;<0 失败代码
        **************************************************************************/
        int (*init)();
        void (*deInit)();
        Factory* (*getFactoryInstance)();
    };

    struct ElementDescriptor{
        const char* factoryName;
        const char* elementClassName;
        int (*init)();
        void (*deInit)();
        Element* (*createElementInstance)();
    };

    class Factory
    {
        static bool isInit;
        typedef Factory*(*GetFactoryInstanceFuncPtr)();
        typedef Element*(*CreateElementFuncPtr)();

        static void addElementDesc(ElementDescriptor* descriptor);
        void registerElement(ElementDescriptor* descriptor);
        std::map<std::string,ElementDescriptor*> elemDescGetFuncPtrMap;

    protected:

    public:
        static int registerFactory(FactoryDescriptor* descriptor);
        static int addElementDesc(ElementDescriptor* descriptors,int count){
            for(int i = 0;i < count;i++)
                Factory::addElementDesc(&descriptors[i]);
            return 0;
        }

        Factory();

        int init();
        virtual ~Factory(){}

        static Factory* getFactoryInstanceByName(const char* factoryName);
        Element* createElementByName(const char* elementClassName)
        {
            if(elemDescGetFuncPtrMap[elementClassName])
            {
                Element* elem = elemDescGetFuncPtrMap[elementClassName]->createElementInstance();
                printf("create element:%s(%p)\n",elementClassName,elem);
                return elem;
            }
            else
            {
                ERROR("Not found element:%s\n",elementClassName);
            }
            return  nullptr;
        }
    };

    #define DECLARE_FACTORY_GET_INSTANCE(FACTORY_CLASS)  \
    static Factory* getInstance()   \
    {   \
        static Factory* _instance = nullptr;\
        if(_instance == nullptr)\
        {\
            _instance = new FACTORY_CLASS;\
            _instance->init();  \
        }\
        return _instance;\
    }

    #define REGISTER_FACTORY(factoryDescriptor) static int tmpVar_factoryDescriptor = Factory::registerFactory(factoryDescriptor)
    #define REGISTER_ELEMENT(elementDescriptors,count) static int tmpVar_elementDescriptors = Factory::addElementDesc(elementDescriptors,count)

}

#endif // FACTORY_H
