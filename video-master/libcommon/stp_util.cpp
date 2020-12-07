#include "STPManager.h"
#include "util.h"
void help(int argc,const char* argv[])
{
    printf("argc=%d\n",argc);
    printf("Usage:\n");
    printf("%s server\n",argv[0]);
    printf("%s onvif_list 8090 192.168.10.10\n",argv[0]);
    exit(0);
}
int main(int argc,const char* argv[]){
    if(argc < 2)
    {
        printf("1111\n");
        help(argc,argv);
    }
    if(strcmp(argv[1],"server") == 0)
    {
        STPManager* stp_server = STPManager::createServerInstance(8090);
        STPRemotePoint remote;
        Json::Value request_json;
        while(1)
        {
            if(stp_server->recv(request_json,&remote) == 0)
            {
                Json::Value response_json;
                response_json["cmd"] = "response";
                if(request_json["cmd"].asString() == "onvif_list")
                {
                    int ipc_cnt = 10;
                    for(int i =0;i < ipc_cnt;i++)
                    {
                        Json::Value ip_json;
                        char ip_str[128];
                        sprintf(ip_str,"192.168.10.%d",i);
                        ip_json["ip"]=ip_str;
                        response_json["data"] = Json::Value();
                        response_json["data"].append(ip_json);
                    }
                }
                else
                {
                    response_json["cmd"] = "unknow_cmd";
                }
                if(stp_server->send(response_json,&remote) < 0)
                {
                    COMM_DEBUG("server send response failure.\n");
                }
            }
        }
    }
    else
    {
        if(argc < 4){
            printf("2222\n");
            help(argc,argv);
        }
        STPManager* stp_client = STPManager::createClientInstance();
        Json::Value request_json;
        request_json["cmd"]=argv[1];
        uint16_t port = atoi(argv[2]);
        STPRemotePoint remote(argv[3],port);
        stp_client->send(request_json,&remote);
        Json::Value response_json;
        stp_client->recv(response_json);
        JsonUtil::PrintJson(response_json);
        delete stp_client;
        stp_client = nullptr;
    }
    return 0;
}
