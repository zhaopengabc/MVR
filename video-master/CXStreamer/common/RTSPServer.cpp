#include "RTSPServer.h"
namespace CXS {
    RTSPServer::RTSPServer()
    {
        mServPort = 554;
        mServerThread = 0;
    }
    void* _start(void* thisServer){
        RTSPServer* server = static_cast<RTSPServer*>(thisServer);
        server->establishAndRunServer(server->getServPort());
        return nullptr;
    }
    int RTSPServer::start(){
        pthread_create(&mServerThread,nullptr,_start,this);
        pthread_detach(mServerThread);
        return 0;
    }
}
