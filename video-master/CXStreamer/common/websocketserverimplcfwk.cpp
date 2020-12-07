#include "websocketserverimplcfwk.h"
#include "cf_threadpool/cf_threadpool.h"
#include "cf_allocator/cf_allocator_simple.h"
#include "cf_json/cf_json.h"
#include <stdio.h>
#include <algorithm>

WebSocketServerImplcFWK::WebSocketServerImplcFWK(uint16_t port)
{
    pthread_mutex_init(&cli_list_mutex,NULL);
    ws_server = cf_websocket_server_create(port);
    cf_websocket_server_set_user_data(ws_server,this);
    on_max_pending_changed = nullptr;
}
void WebSocketServerImplcFWK::start(void){
    cf_threadpool_run(ws_run,this);
}
WebSocketServerImplcFWK::~WebSocketServerImplcFWK(){
    pthread_mutex_destroy(&cli_list_mutex);
}
void WebSocketServerImplcFWK::on_new_websocket(cf_websocket_server* ws_server,cf_websocket* cli){
    WebSocketServerImplcFWK* _this = static_cast<WebSocketServerImplcFWK*>(cf_websocket_server_get_user_data( ws_server)) ;
    video_cli* v_cli = (video_cli*)cf_allocator_simple_alloc(sizeof(video_cli));
    v_cli->ws = cli;
    v_cli->ws_server_impl = _this;
    cf_websocket_set_user_data(cli,v_cli);
    pthread_mutex_lock(&_this->cli_list_mutex);
    _this->cli_list.push_back(v_cli);
    pthread_mutex_unlock(&_this->cli_list_mutex);
}
void WebSocketServerImplcFWK::on_disconnect(cf_websocket_server* ws_server,cf_websocket* cli){
    WebSocketServerImplcFWK* _this = static_cast<WebSocketServerImplcFWK*>(cf_websocket_server_get_user_data( ws_server)) ;
    video_cli* v_cli = (video_cli*)cf_websocket_get_user_data(cli);
    pthread_mutex_lock(&_this->cli_list_mutex);
    if(v_cli){
        _this->cli_list.remove(v_cli);
        cf_allocator_simple_free(v_cli);

    }

    pthread_mutex_unlock(&_this->cli_list_mutex);    //这里把v_cli的释放和list的remove一并保护
}
void WebSocketServerImplcFWK::on_cli_read(cf_websocket* cli,const char* buf,uint64_t n){
    video_cli* v_cli = static_cast<video_cli*>(cf_websocket_get_user_data(cli));
    WebSocketServerImplcFWK* _this = v_cli->ws_server_impl;

    cf_json* json = cf_json_load(buf);
    cf_json_print(json);


    if(cf_json_contains(json,"pendingH264pkg")){
        v_cli->pending_h264 = cf_json_get_int(json,"pendingH264pkg",NULL);
        //printf("WebSocketServerImplcFWK->on_max_pending_changed=%p\n",_this->on_max_pending_changed);
        if(_this->on_max_pending_changed != nullptr){
            auto max = (*std::max_element(_this->cli_list.begin(),_this->cli_list.end(),[](video_cli* c1, video_cli* c2) -> bool { return c1->pending_h264 < c2->pending_h264; }))->pending_h264;
            if(  max != _this->m_max_pending || max == 0){
                _this->m_max_pending = max;
                _this->on_max_pending_changed(_this->on_max_pending_changed_user_data,reinterpret_cast<void*>(max) );
            }
        }
    }
    cf_json_destroy_object(json);
}
void WebSocketServerImplcFWK::ws_run(void* d){
    WebSocketServerImplcFWK* _this = static_cast<WebSocketServerImplcFWK*>(d) ;
    cf_websocket_server_set_on_connect_callback(_this->ws_server,on_new_websocket);
    cf_websocket_server_set_on_disconnect_callback(_this->ws_server,on_disconnect);
    cf_websocket_server_set_on_read_text_callback(_this->ws_server,on_cli_read);
    cf_websocket_server_run(_this->ws_server);
}
int WebSocketServerImplcFWK::pushData(uint8_t* buff,int n){
    pthread_mutex_lock(&cli_list_mutex);
    if(cli_list.size() > 10)
    {
        auto v_ws = cli_list.rbegin();
        v_ws++;
        v_ws++;
        for(;v_ws != cli_list.rend();v_ws++){
            (*v_ws)->pending_h264 = 0;
        }
    }
    auto v_ws_e = cli_list.rend();
    if(cli_list.size() > 2){
        v_ws_e = cli_list.rbegin();
        v_ws_e++;
        v_ws_e++;
    }

    for(auto v_ws = cli_list.rbegin();v_ws != v_ws_e;v_ws++){
        video_cli* cli = (*v_ws);
        cf_websocket_write_binary(cli->ws, buff,n);
    }
    pthread_mutex_unlock(&cli_list_mutex);
    return 0;
}
void WebSocketServerImplcFWK::setMaxPendingChangedCallback(void (*on_max_pending_changed)(void* user_data,void* para),void* user_data){
    this->on_max_pending_changed = on_max_pending_changed;
    this->on_max_pending_changed_user_data = user_data;

}
void WebSocketServerImplcFWK::setLayoutCfgCallback(void (*on_layout_cfg)(void* user_data,void* para),void* user_data){

}
