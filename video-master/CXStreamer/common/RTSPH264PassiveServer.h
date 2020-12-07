#ifndef RTSPH264PASSIVESERVER_H
#define RTSPH264PASSIVESERVER_H
#include "RTSPServer.h"
namespace CXS {
    class RTSPH264PassiveServer : public RTSPServer
    {
        int mPipeId[2];
        FILE* mFid;
        UsageEnvironment* mEnv;
        RTPSink* mVideoSink;
        H264VideoStreamFramer* mVideoSource;
        RTSPH264PassiveServer();
        ~RTSPH264PassiveServer();
    public:
        H264VideoStreamFramer* getVideoSource(){return mVideoSource;}
        void setVideoSource(H264VideoStreamFramer* videoSource){ mVideoSource = videoSource;}
        FILE* getFid(){return mFid;}
        RTPSink* getVideoSink(){return mVideoSink;}
        UsageEnvironment* getEnv(){return mEnv;}
        static RTSPH264PassiveServer* createNew(){ return new RTSPH264PassiveServer;}
        int pushData(uint8_t* buff,int len);
        int establishAndRunServer(uint16_t);
    };
}


#endif // RTSPH264PASSIVESERVER_H
