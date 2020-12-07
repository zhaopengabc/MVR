#ifndef PLAYER_H
#define PLAYER_H
#include "player.h"
#include <CXStreamer/factory.h>
class Player
{
    CXS::Element* mElemHead1;
    CXS::Element* mElemHead2;
    Player();
public:
    static Player* getInstance(int chn1,int chn2,uint16_t port1,uint16_t port2,uint8_t protocol = 0);

    int start();
};

#endif // PLAYER_H
