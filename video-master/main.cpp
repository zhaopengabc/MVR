#include <iostream>
#include<unistd.h>
#include<stdlib.h>
//#include <QCoreApplication>
#include "cf_logger/cf_logger.h"
#include "zstd.h"
#include "CXStreamer/factory.h"
#include "player.h"

int main(int argc,  char *argv[])
{
    //QCoreApplication a(argc, argv);
    std::cout << "Hello World!" << std::endl;
    z_unused(argc);
    z_unused(argv);
    long chn1 = 0;
    long chn2 = 1;
    long protocol = 0;
    uint16_t port1 = 8081,port2 = 8082;
    if(argc >= 2)
        chn1 = strtol( argv[1],NULL,0);
    if(argc >= 3)
        chn2 = strtol( argv[2],NULL,0);

    if(argc >= 4)
        port1 = strtol( argv[3],NULL,0);

    if(argc >= 5)
        port2 = strtol( argv[4],NULL,0);
    if(argc >= 6)
        protocol = strtol( argv[5],NULL,0);

    Player * player = Player::getInstance(chn1,chn2,port1,port2,protocol);
    if(player != nullptr)
    {
        player->start();
    }
    else{
        cf_log(NULL,CF_LOG_ERROR,"create player failure.\n");
        return -1;
    }
    while(true){
        sleep(1);
    }
    return 0;
    //return a.exec();
}


