#ifndef ELEMENT_H
#define ELEMENT_H
#include <vector>
#include <map>
#include<sstream>
#include <stdint.h>
#include "cxstreamer.h"
#include "zstd.h"
namespace CXS{
    class Element
    {
        std::string mClassName;
        std::map<std::string,std::string> mAttrMap;
    protected:
        enum EState
        {
            Init,
            Starting,
            Stop
        }m_state;
        Element(const char* className);

    public:
        typedef int(*callback)(uint8_t * ,size_t);

        virtual int startSelf(){return true;}
        virtual int startSelfData(callback _getData){return true;}
        virtual int linkTo(Element* ){return 0;}

        std::vector<Element*> mNextElems;


        const char* getClassName()const { return mClassName.data(); }
        void printOutElem(){
            printf("%s(%p) mOutPads.size=%u:\n",this->getClassName(),this,mNextElems.size());
            for(uint32_t i =0;i < mNextElems.size();i++){
                printf("--%s\n",mNextElems[i]->getClassName());
            }
        }
        virtual ~Element();
        // virtual int getData(void* )
        // {
        //     return 0;
        // }
        virtual int pushData(void* )
        {
            return 0;
        }
        int pushNext(void* data)
        {
            for(uint32_t i =0;i < mNextElems.size();i++){
                Element* elem = mNextElems[i];
                elem->pushData(data);
            }
            return 0;
        }

        template<typename T>
        void setAttr(const char* attrName,T attrVal)
        {
            std::stringstream ss;
            ss<<attrVal;
            mAttrMap[attrName] = ss.str();
        }

        template<typename T>
        T getAttr(const char* attrName,T defaultVal){
            if(mAttrMap.count(attrName) == 0)
                return defaultVal;
            T val;
            std::stringstream ss(mAttrMap[attrName]);
            ss>>val;
            return val;
        }
        std::string getAttr(const char* attrName,const char* defaultVal)
        {
            if(mAttrMap.count(attrName) == 0)
                return defaultVal;
            std::string val;
            std::stringstream ss(mAttrMap[attrName]);
            ss>>val;
            return val;
        }
        int stop()
        {
            m_state = Stop;
            return 0;
        }
        int start()
        {
            printf("%s start\n",this->getClassName());
            if(m_state == Starting){
                return 0;
            }

            if(startSelf() == 0 )
            {
                // for(size_t i = 0;i < mNextElems.size();i++)
                // {

                //     DEBUG("----------linkedElem(%u) %s\n",(uint32_t)i,mNextElems[i]->getClassName());
                //     if(mNextElems[i]->start() != 0 || linkTo(mNextElems[i]) != 0){
                //         stop();
                //         return -1;
                //     }
                // }
                m_state = Starting;
            }
            return 0;
        }
        int link(Element* elem){
            printf("%s link to %s\n",this->getClassName(),elem->getClassName());
            mNextElems.push_back(elem);
            return 0;
        }
    };
}
#endif // ELEMENT_H
