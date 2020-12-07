#ifndef HTTPSERVER_H
#define HTTPSERVER_H
#include <thread>
#include "websocketserverinf.h"
#include <libwebsockets.h>
namespace CXS{
class HttpServer
{
    int mSockfd;
    int mLogs;
    struct lws_context_creation_info mInfo;

    pthread_t mThread;
public:
    static void* threadFunc(void* server);
    HttpServer(uint16_t port);
};
}
#endif // HTTPSERVER_H
