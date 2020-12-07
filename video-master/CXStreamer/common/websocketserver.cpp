#include <string.h>
#include "../factory.h"
#include "../element.h"

#include <thread>
#include <mutex>
#include <map>
#include <list>
#include <vector>
#include <stdint.h>
#include "httpserver.h"
//#include "websocketserverimplqt.h"
#include "websocketserverimplcfwk.h"
#include "zstd.h"
namespace CXS{
    //WebsocketServer类作为Element体系下实现websocket转发的子类Element， WebsocketServerInf作为websocket通信功能的抽象，具体通信功能由WebsocketServerInf的子类实现;
    //与业务相关的逻辑应收敛于WebsocketServer，然而这里的实现却把业务相关代码(分析处理客户端未决h264包数)柔和进了WebsocketServerInf子类，致使这里代码颇为凌乱，应该改进。
    #define ELEMENT_CLASS_NAME  "ws-server"
    class WebsocketServer : public Element
    {

        WebsocketServerInf* mWebsocketServer;
        void (*m_eventHandler)(void* user_data,void* event_para);
        void (*m_layout_cfg_Handler)(void* user_data,void* event_para);
        void* m_user_data;
        WebsocketServer():Element(ELEMENT_CLASS_NAME){

            m_eventHandler = nullptr;
            m_layout_cfg_Handler = nullptr;
            m_user_data = nullptr;
            static HttpServer* httpServer = nullptr;
            if(httpServer == nullptr)
                httpServer = new HttpServer(8085);
            setAttr("port",8081);
        }
        ~WebsocketServer(){
            if(mWebsocketServer)
            {
                delete mWebsocketServer;
                mWebsocketServer = nullptr;
            }
        }

    public:
        struct Buffer{
            uint8_t* buffer;
            size_t len;
        };
        int pushData(void* data)
        {
            struct Buffer* buf = (struct Buffer*)data;
            uint8_t* buff = buf->buffer;
            size_t len = buf->len;
            return mWebsocketServer->pushData(buff,len);
        }

        static Element* createInstance()
        {
            WebsocketServer* elem = new WebsocketServer();
            return elem;
        }

        int startSelf(){
            //mWebsocketServer = new WebsocketServerQt(mPort);
            //mPort = 8800;
            uint16_t port = getAttr("port",0);
            mWebsocketServer = new WebSocketServerImplcFWK(port);
            mWebsocketServer->start();
            uint64_t ptr = getAttr("max_cli_pending_changed",(uint64_t)nullptr);;
            m_eventHandler = (void (*)(void* ,void* ))ptr;
            m_user_data = (void*)getAttr("max_cli_pending_user_data",(uint64_t)nullptr);
            if(m_eventHandler != nullptr)
            {
                mWebsocketServer->setMaxPendingChangedCallback(m_eventHandler,m_user_data);
            }
            m_layout_cfg_Handler = (void (*)(void* ,void* ))getAttr("layout_cfg",(void*)nullptr);
            if(m_layout_cfg_Handler != nullptr){
                mWebsocketServer->setLayoutCfgCallback(m_layout_cfg_Handler,NULL);
            }
            return 0;
        }
        int linkTo(Element* elem){
            z_unused(elem);
            return  -1  ;
        }
    };

    static int init()
    {
        return 0;
    }
    static void deInit()
    {

    }
    static ElementDescriptor elementDescriptors[] = {
        {
             "common",
             ELEMENT_CLASS_NAME,
             init,
             deInit,
             WebsocketServer::createInstance
        },
    };
    REGISTER_ELEMENT(elementDescriptors,sizeof(elementDescriptors)/sizeof(elementDescriptors[0]));
}
