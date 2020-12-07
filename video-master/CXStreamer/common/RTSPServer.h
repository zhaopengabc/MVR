#ifndef RTSPH264SERVER_H
#define RTSPH264SERVER_H
#include <string.h>
#include "../factory.h"
#include "../element.h"

#include <pthread.h>
#include <mutex>
#include <map>
#include <list>
#include <vector>
#include <stdint.h>
#include "liveMedia.hh"
#include "BasicUsageEnvironment.hh"
#include "VideoPipeServerMediaSubsession.h"
#include "CXLog/CXLog.h"
#include "zstd.h"
#include <pthread.h>
#define ALLOW_SERVER_PORT_REUSE
namespace CXS {
    class RTSPServer
    {
        uint16_t mServPort;
        pthread_t mServerThread;
    public:
        RTSPServer();
        int start();
        void setServPort(uint16_t servPort){ mServPort = servPort;}
        uint16_t getServPort(){return mServPort;}
        virtual int pushData(uint8_t* buff,int len) = 0;
        virtual int establishAndRunServer(uint16_t) = 0;
    };
}


#endif // RTSPH264SERVER_H
