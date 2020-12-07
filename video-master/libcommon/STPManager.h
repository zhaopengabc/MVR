#ifndef  STPMANAGER_H
#define  STPMANAGER_H

#include<stdint.h>

#include<stdio.h>
#include<stdlib.h>  
#include<string.h>     
#include<unistd.h>      
#include<sys/types.h>    
#include<sys/socket.h>     
#include<arpa/inet.h>    
#include<netinet/in.h> 
#include<json/json.h>
#include "json_util.h"
#include<sstream>

#define STP_MAX_PKG_SIZE    (60*1024)

class STPRemotePoint{    
    struct sockaddr_in m_addr;
    int m_len;
public:
    STPRemotePoint(){
    }
    STPRemotePoint(const char* ip_addr,uint16_t port){
        m_addr.sin_family = AF_INET;
        m_addr.sin_addr.s_addr = inet_addr(ip_addr);
        m_addr.sin_port = htons(port);
        m_len = sizeof(m_addr);
    }
    friend class STPManager;
};

class STPManager{
    int m_socket;
    const int m_timeout_ms;
    int m_request_seq;
    int m_response_seq;
    bool m_is_server;
    STPManager(int socket,bool is_server = false):m_socket(socket),m_timeout_ms(3000),m_request_seq(0),m_response_seq(0),m_is_server(is_server){
        struct timeval timeout = {m_timeout_ms/1000,m_timeout_ms%1000 };
        setsockopt(socket,SOL_SOCKET,SO_RCVTIMEO,(char *)&timeout,sizeof(struct timeval));
    }
    static STPManager* createInstance(bool is_server,uint16_t port = 0,const char* multicast_addr = nullptr){
        int err;
        int socket_fd;
        socket_fd = socket(AF_INET, SOCK_DGRAM, 0);         /*建立套接字*/
        if (socket_fd == -1)
        {
            perror("socket()");
            goto err1;
        }
        if(port != 0)
        {
            struct sockaddr_in local_addr;
            memset(&local_addr, 0, sizeof(local_addr));
            local_addr.sin_family = AF_INET;
            local_addr.sin_addr.s_addr = htonl(INADDR_ANY);
            local_addr.sin_port = htons(port);

            /*绑定socket*/
            err = bind(socket_fd,(struct sockaddr*)&local_addr, sizeof(local_addr)) ;
            if(err < 0)
            {
                perror("bind()");
                goto err2;
            }
        }
        else
        {
            struct timeval timeout = {3,0 };
            setsockopt(socket_fd,SOL_SOCKET,SO_RCVTIMEO,(char *)&timeout,sizeof(struct timeval));
        }

        /*设置回环许可*/
#if 0
        int loop = 1;
        err = setsockopt(socket_fd,IPPROTO_IP, IP_MULTICAST_LOOP,&loop, sizeof(loop));
        if(err < 0)
        {
            perror("setsockopt():IP_MULTICAST_LOOP");
            goto err2;
        }
#endif

        if( multicast_addr != nullptr){
            struct ip_mreq mreq;                                /*加入多播组*/
            mreq.imr_multiaddr.s_addr = inet_addr(multicast_addr);  /*多播地址*/
            mreq.imr_interface.s_addr = htonl(INADDR_ANY);      /*网络接口为默认*/
            /*将本机加入多播组*/
            err = setsockopt(socket_fd, IPPROTO_IP, IP_ADD_MEMBERSHIP,&mreq, sizeof(mreq));
            if (err < 0)
            {
                perror("setsockopt():IP_ADD_MEMBERSHIP");
                goto err2;
            }
        }
        return new STPManager(socket_fd,is_server);
err2:
        close(socket_fd);
err1:
        return nullptr;
    }
public:
    static STPManager* createServerInstance(uint16_t port = 0,const char* multicast_addr = nullptr){
        return createInstance(true,port,multicast_addr);
    }
    static STPManager* createClientInstance(uint16_t port = 0,const char* multicast_addr = nullptr){
        return createInstance(false,port,multicast_addr);
    }
    int recv(Json::Value& jsonObj,STPRemotePoint* remote = nullptr){
        struct sockaddr_in addr_client;
        int len=sizeof(addr_client);
        char recv_buf[STP_MAX_PKG_SIZE];
        bool res = false;
        while(true){
            int recv_num = recvfrom(m_socket, recv_buf, sizeof(recv_buf), 0, (struct sockaddr *)&addr_client, (socklen_t *)&len);
    //        printf("recv from recv_num=%d\n",recv_num);
            if(recv_num > 0){
                printf("recv str=%s\n",recv_buf);
                res =  JsonUtil::ParseJson(recv_buf,&jsonObj);
                if(res && remote != nullptr)
                {
                    remote->m_addr = addr_client;
                    remote->m_len = len;
                }
                if(res == true ){
                    if(m_is_server == false && jsonObj["__meta__"]["response"]["seq"] != m_request_seq)
                    {
                        res = false;
                        continue;
                    }
                    else if(m_is_server == true)
                    {
                        m_response_seq = jsonObj["__meta__"]["request"]["seq"].asInt();

                    }
                }
            }
            break;
        }
        return res == false ? -1 : 0;
    }
    int recv(Json::Value& jsonObj,int timeout_ms){
        struct timeval timeout = {timeout_ms/1000,timeout_ms%1000*1000 };
        setsockopt(m_socket,SOL_SOCKET,SO_RCVTIMEO,(char *)&timeout,sizeof(struct timeval));
        int res = recv(jsonObj);
        timeout = {m_timeout_ms/1000,m_timeout_ms%1000*1000 };
        setsockopt(m_socket,SOL_SOCKET,SO_RCVTIMEO,(char *)&timeout,sizeof(struct timeval));
        return res;
    }
    /*************************
     *
     * return:
     *  -1 failured
     *  -2 json string to length
     **************************/
    int send( Json::Value& jsonObj,const STPRemotePoint *remote ){
        if(remote == nullptr)
            return -1;


        if(m_is_server)
        {
            jsonObj["__meta__"]=Json::Value();
            jsonObj["__meta__"]["response"] = Json::Value();
            jsonObj["__meta__"]["response"]["seq"] = m_response_seq;
        }
        else
        {
            jsonObj["__meta__"]=Json::Value();
            jsonObj["__meta__"]["request"] = Json::Value();
            jsonObj["__meta__"]["request"]["seq"] = ++m_request_seq;
        }

        std::string str = JsonUtil::JsonToString(jsonObj);

        int res = sendto(m_socket,str.c_str(),str.length()+1,0,(const struct sockaddr*)&remote->m_addr,remote->m_len);
        printf("send to str=%s ,len=%d, res=%d\n",str.c_str(),(int)str.length()+1,res);
        if(res <= 0)
            return -1;
        else
            return 0;
    }
    int send( Json::Value& jsonObj,const STPRemotePoint& remote ){
        return send(jsonObj,&remote);
    }
    ~STPManager(){
        close(m_socket);
        m_socket = 0;
    }

};
     

#endif//  STPMANAGER_H
