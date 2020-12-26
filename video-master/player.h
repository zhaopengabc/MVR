#ifndef PLAYER_H
#define PLAYER_H
#include "player.h"
#include <CXStreamer/factory.h>
class Player
{
    CXS::Element *mElemHead1;
    CXS::Element *mElemHead2;
    Player();

private:
    struct Input
    {
        uint16_t width = 3840;
        uint16_t height = 2160;
        uint8_t format = 0; // 0:NV12 1:NV16
    };
    struct RateCtl
    {
        uint8_t RateCtrlMode = 3; // 0:CONST_QP 1:CBR 2:VBR 3:LOW_LATENCY
        uint16_t FrameRate = 60;
        uint32_t BitRate = 25000;
        uint32_t MaxBitRate = 25000;
        int16_t SliceQP = -1; // 0-51 -1:AUTO
        float CPBSize = 1;    //
        float InitialDelay = 0.5;
        bool ScnChgResilience = false;
        uint16_t MaxPictureSize = 1000;
    };
    struct Gop
    {
        uint8_t mode = 0; //0:DEFAULT_GOP, 1:LOW_DELAY_P, 2:LOW_DELAY_B, 3:PYRAMIDAL_GOP
        uint16_t lenght = 120;
        uint8_t numB = 0;
    };
    /* 
profile 
  0:HEVC_MONO10
  1:HEVC_MONO
  2:HEVC_MAIN_422_10_INTRA
  3:HEVC_MAIN_422_10
  4:HEVC_MAIN_422
  5:HEVC_MAIN_INTRA
  6:HEVC_MAIN_STILL
  7:HEVC_MAIN10_INTRA
  8:HEVC_MAIN10
  9:HEVC_MAIN
  10:AVC_BASELINE
  11:AVC_C_BASELINE
  12:AVC_MAIN
  13:AVC_HIGH10_INTRA
  14:AVC_HIGH10
  15:AVC_HIGH_422_INTRA
  16:AVC_HIGH_422
  17:AVC_HIGH
  18:AVC_C_HIGH
  19:AVC_PROG_HIGH
  20:XAVC_HIGH10_INTRA_CBG
  21:XAVC_HIGH10_INTRA_VBR
  22:XAVC_HIGH_422_INTRA_CBG
  23:XAVC_HIGH_422_INTRA_VBR
  24:XAVC_LONG_GOP_MAIN_MP4
  25:XAVC_LONG_GOP_HIGH_MP4
  26:XAVC_LONG_GOP_HIGH_MXF
  27:XAVC_LONG_GOP_HIGH_422_MXF
*/

    struct Setting
    {
        uint8_t profile = 12;
        float level = 5.2;
        uint8_t tier = 0;       //0:MAIN_TIER, 1:HIGH_TIER
        uint8_t chromaMode = 0; //0:CHROMA_4_2_0, 1:CHROMA_4_2_2
        uint8_t bitDepth = 8;
        uint8_t numSlices = 4;
        uint8_t QPCtrlMode = 1;  //0:UNIFORM_QP, 1:AUTO_QP, 2:LOAD_QP
        uint8_t scalingList = 0; //0:FLAT, 1:DEFAULT, 2:CUSTOM
        uint8_t entropyMode = 1; //0:MODE_CAVLC, 1:MODE_CABAC
        bool loopFilter = true;
        bool cacheLevel2 = true;
    };

    struct Nova_EnocderCfg
    {
        Gop gopData;
        Setting settingData;
        Input inputData;
        RateCtl rateCtlData;
    };

public:
    static Player *getInstance(int chn1, int chn2, uint16_t port1, uint16_t port2, uint8_t protocol = 0);
    typedef int(*callback)(uint8_t * ,size_t);
    static int RTSPCallBack_0(uint8_t *framedata, size_t datalen);
    static int RTSPCallBack_1(uint8_t *framedata, size_t datalen);

    static void *NovaEncoderInit(int channel,Nova_EnocderCfg nova_cfg, callback rtsp_push);
    // static CXS::Element *NovaEncoderInit(Nova_EnocderCfg nova_cfg);

    static int NovaEncoderDestroy();
    int start();
};

#endif // PLAYER_H
