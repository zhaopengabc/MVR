#ifndef WEBSOCKETSERVERIMPLCFWK_H
#define WEBSOCKETSERVERIMPLCFWK_H
#include "websocketserverinf.h"
#include "cf_websocket/cf_websocket_server.h"
#include <pthread.h>
#include <list>
class WebSocketServerImplcFWK;
typedef struct video_cli{
    cf_websocket* ws;
    WebSocketServerImplcFWK* ws_server_impl;
    uint32_t pending_h264;
} video_cli;

class WebSocketServerImplcFWK : public WebsocketServerInf
{
    cf_websocket_server *ws_server;
    void (*on_max_pending_changed)(void* user_data,void* para);
    void* on_max_pending_changed_user_data;
    uint32_t m_max_pending;//记录堆积h264包最多的客户端堆积的h264包数
    std::list<video_cli*> cli_list;
    pthread_mutex_t cli_list_mutex;
    static void on_new_websocket(cf_websocket_server* ,cf_websocket* );
    static void on_disconnect(cf_websocket_server* ,cf_websocket* );
    static void on_cli_read(cf_websocket*,const char*,uint64_t );
    static void ws_run(void*);
public:

    WebSocketServerImplcFWK(uint16_t port);
    void start(void);
    ~WebSocketServerImplcFWK();
    int pushData(uint8_t* buff,int len);
    void setMaxPendingChangedCallback(void (*on_max_pending_changed)(void* user_data,void* para),void* user_data);
    void setLayoutCfgCallback(void (*on_layout_cfg)(void* user_data,void* para),void* user_data);
};

#endif // WEBSOCKETSERVERIMPLCFWK_H
