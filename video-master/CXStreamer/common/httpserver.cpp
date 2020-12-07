#include "httpserver.h"
#include <stdint.h>
#include <arpa/inet.h>
#define LWS_PLUGIN_STATIC

#include "protocol_lws_minimal.c"
//#include "websocketserverimplqt.h"
namespace CXS{

//static int active_write(struct lws *wsi);

 static const struct lws_protocol_vhost_options wasm_mime = {
      NULL,               /* "next" pvo linked-list */
      NULL,               /* "child" pvo linked-list */
      ".wasm",             /* file suffix to match */
      "application/wasm"       /* mimetype to use */
  };

    static const struct lws_http_mount mount = {
        /* .mount_next */		NULL,		/* linked-list "next" */
        /* .mountpoint */		"/",		/* mountpoint URL */
        /* .origin */			"./mount-origin",  /* serve from dir */
        /* .def */			"index.html",	/* default filename */
        /* .protocol */			NULL,
        /* .cgienv */			NULL,
        /* .extra_mimetypes */		&wasm_mime,
        /* .interpret */		NULL,
        /* .cgi_timeout */		0,
        /* .cache_max_age */		0,
        /* .auth_mask */		0,
        /* .cache_reusable */		0,
        /* .cache_revalidate */		0,
        /* .cache_intermediaries */	0,
        /* .origin_protocol */		LWSMPRO_FILE,	/* files in a dir */
        /* .mountpoint_len */		1,		/* char count */
        /* .basic_auth_login_file */	NULL,
        {NULL,NULL}
    };
    static struct lws_protocols protocols[] = {
        { "http", lws_callback_http_dummy, 0, 0 ,0,NULL,0},
        LWS_PLUGIN_PROTOCOL_MINIMAL,
        { NULL, NULL, 0, 0,0,NULL,0 } /* terminator */
    };

    HttpServer::HttpServer(uint16_t port)
    {
        protocols[1].user = (void*)this;
        memset(&mInfo, 0, sizeof mInfo); /* otherwise uninitialized garbage */
        mInfo.port = port;
        mInfo.mounts = &mount;
        mInfo.protocols = protocols;
        mInfo.vhost_name = "localhost";
        mInfo.ws_ping_pong_interval = 10;
    //            mInfo.options =
    //                LWS_SERVER_OPTION_HTTP_HEADERS_SECURITY_BEST_PRACTICES_ENFORCE;

        mLogs = 0;


        lws_set_log_level(mLogs, NULL);
        lwsl_user("LWS minimal ws server | visit http://localhost:7681 (-s = use TLS / https)\n");
        printf("listen on port %d\n",mInfo.port);

        pthread_create(&mThread,0,threadFunc,this);


    }
    void* HttpServer::threadFunc(void* param)
    {
        HttpServer* sert = ( HttpServer*)param;
    //    if(mContext)
    //    {
    //        lws_context_destroy(mContext);
    //        mContext = nullptr;
    //    }
        struct lws_context * context = lws_create_context(&sert->mInfo);
        if (!context) {
            lwsl_err("lws init failed\n");
            return nullptr;
        }
        while(true)
        {
            //printf("lws_service...\n");
            lws_service(context,1000);
        }
        return nullptr;
    }

}
