#include <string.h>
#include <iostream>
#include "../factory.h"
#include "../element.h"
#include "RTSPOnDemandServer.h"
#include "RTSPH264PassiveServer.h"
#include <pthread.h>
namespace CXS{
    #define ELEMENT_CLASS_NAME  "rtsp-server"
    class LiveRTSPServer : public Element
    {
        struct h264_buff_t{
            uint8_t* buff;
            int len;
            h264_buff_t(uint8_t* buff,int len){
                this->buff = (uint8_t*)malloc(len);
                memcpy(this->buff,buff,len);
                this->len = len;
            }
            ~h264_buff_t(){
                free(this->buff);
            }
        };
        std::string mPayload;
        RTSPServer *mRtspServer;
        uint16_t mPort;
        std::list<h264_buff_t*> mPushBuffList;
        pthread_mutex_t mPushBuffListMutex;
        pthread_t mPushThread;
        bool mPushThreadIsRun;
        LiveRTSPServer():Element(ELEMENT_CLASS_NAME){
            setAttr("payload","h264");
            setAttr("isConnected",false);
            mRtspServer = NULL;
            mPushThreadIsRun = true;
            pthread_mutex_init(&mPushBuffListMutex,NULL);
            pthread_create(&mPushThread,0,mPushThreadFunc,this);
            pthread_detach(mPushThread);
        }
        ~LiveRTSPServer(){
            mPushThreadIsRun = false;
        }

        static void* mPushThreadFunc(void* arg){
            LiveRTSPServer* server = (LiveRTSPServer*)arg;
            while(server->mPushThreadIsRun){
                pthread_mutex_lock(&server->mPushBuffListMutex);
                if(server->mPushBuffList.size()>0){
                    h264_buff_t* h264_buff = server->mPushBuffList.front();
                    server->mPushBuffList.pop_front();
                    pthread_mutex_unlock(&server->mPushBuffListMutex);
                    if(server->mRtspServer)
                        server->mRtspServer->pushData(h264_buff->buff,h264_buff->len);
                    delete h264_buff;
                }
                else
                {
                    pthread_mutex_unlock(&server->mPushBuffListMutex);
                    usleep(5000);
                }

            }

            pthread_mutex_lock(&server->mPushBuffListMutex);
            while(server->mPushBuffList.size()>0){
                h264_buff_t* h264_buff = server->mPushBuffList.front();
                server->mPushBuffList.pop_front();
                delete h264_buff;
            }
            pthread_mutex_unlock(&server->mPushBuffListMutex);
            return NULL;
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
            pthread_mutex_lock(&mPushBuffListMutex);
            if(mPushBuffList.size()<10)
            {
                mPushBuffList.push_back(new h264_buff_t(buff,len));
                setAttr("isConnected",true);
            }
            else{//无客户端连接
                //printf("LiveRTSPServer::pushData package is too much\n");
                setAttr("isConnected",false);
            }
            pthread_mutex_unlock(&mPushBuffListMutex);
            return 0;
        }

        static Element* createInstance()
        {
            LiveRTSPServer* elem = new LiveRTSPServer();
            return elem;
        }


        int startSelf(){
            std::string payload = getAttr("payload","h264");
            if(payload == "h264")
            {
                printf("rtsp payload is h264\n");
                mRtspServer = RTSPOnDemandServer::createNew(RTSPOnDemandServer::H264);
            }
            else{
                printf("rtsp payload is h265\n");
                std::cout<<"===="<<payload<<std::endl;
                mRtspServer = RTSPOnDemandServer::createNew(RTSPOnDemandServer::H265);
            }

            uint16_t port = getAttr("port",0);
            mRtspServer->setServPort(port);

            mRtspServer->start();
            return 0;
        }
        int linkTo(Element* elem){
            z_unused(elem);
            return 0;
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
             LiveRTSPServer::createInstance
        },
    };
    REGISTER_ELEMENT(elementDescriptors,sizeof(elementDescriptors)/sizeof(elementDescriptors[0]));
}
