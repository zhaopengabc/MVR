#ifndef GLOBAL_H
#define GLOBAL_H
#include "configer.h"
#define CONFIG_VIDEO_FILE "/tmp/video_config.json"
class Global{
public:
    Configer conf;
    Global():conf(CONFIG_VIDEO_FILE){

    }
}global;

#endif // GLOBAL_H
