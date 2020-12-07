#ifndef RTSPH264ONDEMANDSERVER_H
#define RTSPH264ONDEMANDSERVER_H
#include <RTSPServer.h>
#include "VideoPipeServerMediaSubsession.h"
namespace CXS{
    class RTSPOnDemandServer : public RTSPServer
    {
    public:
        enum Format{
            H264 = VideoPipeServerMediaSubsession::H264,
            H265 = VideoPipeServerMediaSubsession::H265
        } mFormat;
    private:
        VideoPipeServerMediaSubsession* mSubSersion;
        RTSPOnDemandServer(Format format);
    public:
        static RTSPOnDemandServer* createNew(Format format){ return new RTSPOnDemandServer(format);}
        int pushData(uint8_t* buff,int len);
        int establishAndRunServer(uint16_t);
    };
}
#endif // RTSPH264ONDEMANDSERVER_H
