#ifndef WEBSOCKETSERVERINF_H
#define WEBSOCKETSERVERINF_H
#include <stdint.h>
//该接口是为了屏蔽掉qtwebsocket实现细节，主要考虑未来可能采用其他websocket方案替换qtwebsocket方案
class WebsocketServerInf
{
public:
    WebsocketServerInf();
    virtual void start(void){}
    virtual ~WebsocketServerInf(){}
    virtual int pushData(uint8_t* buff,int len) = 0;
    virtual void setMaxPendingChangedCallback(void (*on_max_pending_changed)(void* user_data,void* para),void* user_data) = 0;
    virtual void setLayoutCfgCallback(void (*on_layout_cfg)(void* user_data,void* para),void* user_data) = 0;

};

#endif // WEBSOCKETSERVERINF_H
