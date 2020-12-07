#ifndef CF_WEBSOCKET_SERVER_H
#define CF_WEBSOCKET_SERVER_H
#include <stdint.h>
#include <stddef.h>

#ifdef __cplusplus
extern "C"{
#endif

typedef struct cf_websocket_server cf_websocket_server;
typedef struct cf_websocket cf_websocket;
cf_websocket_server* cf_websocket_server_create(uint16_t port);
void cf_websocket_server_set_user_data(cf_websocket_server* ,void* );
void* cf_websocket_server_get_user_data(cf_websocket_server* );
int cf_websocket_server_run(cf_websocket_server* server);
int cf_websocket_write_text(cf_websocket* ,const char* buf,uint64_t n);
int cf_websocket_write_binary(cf_websocket* ,const uint8_t* buf,uint64_t n);
void cf_websocket_server_set_on_read_text_callback(cf_websocket_server* server,void (*on_cli_read_text)(cf_websocket*,const char*,uint64_t ));
void cf_websocket_server_set_on_read_binary_callback(cf_websocket_server* server,void (*on_cli_read)(cf_websocket*,const uint8_t*,uint64_t ));
void cf_websocket_server_set_on_connect_callback(cf_websocket_server* server,void (*on_new_websocket)(cf_websocket_server* ,cf_websocket* ));
void cf_websocket_server_set_on_disconnect_callback(cf_websocket_server* server,void (*on_disconnect)(cf_websocket_server* ,cf_websocket* ));

void cf_websocket_set_user_data(cf_websocket* ,void* );
void* cf_websocket_get_user_data(cf_websocket* );

#ifdef __cplusplus
}
#endif

#endif//CF_WEBSOCKET_SERVER_H
