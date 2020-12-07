#ifndef UTIL_H
#define UTIL_H
#include <sys/time.h>
#include <iostream>
#include <stdio.h>
#include "zsettings.h"
class Util{
public:

    static void print_array(const char* title, uint8_t* buffer,uint32_t len)
    {
        std::string print_buffer;
        char tmp_buffer[512];
        print_buffer+=title;
        print_buffer+="\r\n";
        for(uint32_t i = 0;i < len;i++)
        {
            sprintf(tmp_buffer,"%02x ",buffer[i]);
            print_buffer+=tmp_buffer;
            if( (i+1)%16 == 0)
            {
                print_buffer+="\r\n";
            }
        }
        print_buffer+="\r\n";
        std::cout<<print_buffer<<std::endl;

    }
    static long int gettime_ms()
    {
        static long int st = -1;
        struct timeval tv;
        gettimeofday(&tv,nullptr);
        if(st == -1)
            st = tv.tv_sec*1000+tv.tv_usec/1000;
        return tv.tv_sec*1000+tv.tv_usec/1000 - st;
    }

    static void get_device_SN(uint8_t* sn_buff){

        if(sn_buff == nullptr)
            return;

        ZSettings settings("/usr/Hseries/configs/sn");
        std::string sn_str = settings.getString("sn","0000000000000000");

        sscanf(sn_str.data(),"%02hhx%02hhx%02hhx%02hhx%02hhx%02hhx%02hhx%02hhx",&sn_buff[0],&sn_buff[1],&sn_buff[2],&sn_buff[3],&sn_buff[4],&sn_buff[5],&sn_buff[6],&sn_buff[7]);
    }

    static uint16_t get_local_model_id()
    {
    #ifdef DEV_TYPE_MVR
        return 0x7b47;
    #else
        return 0x7b05;
    #endif
    }


    static uint8_t get_local_slot_id()
    {
        int slot_id = -1;
        if(slot_id == -1){
            FILE *stream = nullptr;
            stream = popen("cat /dev/slot_id", "r");
            fscanf(stream,"%d",&slot_id);
            pclose(stream);
            slot_id -= 1;
        }
        return static_cast<uint8_t>(slot_id);
    }
};



#define COMM_LOG(format,...)  { \
    if( 1 ){\
        long int ms = Util::gettime_ms();\
        printf("%ld,%-3ld(s) File:" __FILE__ ",Line:%05d:" format ,ms/1000,ms%1000, __LINE__, ##__VA_ARGS__);\
    }\
}
#ifdef DEBUG_ON
#define COMM_DEBUG(format,...) { \
        long int ms = Util::gettime_ms();\
        printf("%ld,%-3ld(s) File:" __FILE__ ",Line:%05d:" format ,ms/1000,ms%1000, __LINE__, ##__VA_ARGS__);\
}    
#else
#define COMM_DEBUG(format,...)
#endif


#define COMM_WARNING(format,...) { \
    if( 1 ){\
        long int ms = Util::gettime_ms();\
        printf("%ld,%-3ld(s) File:" __FILE__ ",Line:%05d:" format ,ms/1000,ms%1000, __LINE__, ##__VA_ARGS__);\
    }\
}


#define COMM_ERROR(format,...) { \
    if( 1 ){\
        long int ms = Util::gettime_ms();\
        printf("%ld,%-3ld(s) File:" __FILE__ ",Line:%05d:" format ,ms/1000,ms%1000, __LINE__, ##__VA_ARGS__);\
    }\
}
#define INIT    __attribute((constructor))
#endif // UTIL_H
