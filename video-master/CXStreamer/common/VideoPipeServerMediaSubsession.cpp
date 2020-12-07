#include <unistd.h>
#include <fcntl.h>
#include <vector>
#include "VideoPipeServerMediaSubsession.h"
#include "H264VideoRTPSink.hh"
#include "H265VideoRTPSink.hh"
#include "GroupsockHelper.hh"
#include "MultiFramedRTPSink.hh"
#include "ByteStreamFileSource.hh"
#include "FramedPipeSource.h"
#include "H264VideoStreamFramer.hh"
#include "H265VideoStreamFramer.hh"
#include "CXLog/CXLog.h"
VideoPipeServerMediaSubsession::VideoPipeServerMediaSubsession(UsageEnvironment& env,  Boolean reuseFirstSource,Format format):OnDemandServerMediaSubsession(env, reuseFirstSource)
{
    mFormat = format;
    pipe(mPipeId);
    int flags = fcntl(mPipeId[0],F_GETFL);
    fcntl(mPipeId[0],F_SETFL,flags|O_NONBLOCK);
    //flags = fcntl(mPipeId[1],F_GETFL);
    //fcntl(mPipeId[1],F_SETFL,flags|O_NONBLOCK);
    mFid = fdopen(mPipeId[0],"rb");

    //zxr 可尝试设置组播ip
//    setServerAddressAndPortForSDP( 0/*232.0.27.173/20*/,
//                                  20786);
}
VideoPipeServerMediaSubsession*
VideoPipeServerMediaSubsession::createNew(UsageEnvironment& env,  Boolean reuseFirstSource,Format format)
{
    return new VideoPipeServerMediaSubsession(env,reuseFirstSource,format);
}
FramedSource* VideoPipeServerMediaSubsession::createNewStreamSource(unsigned /*clientSessionId*/,
                        unsigned& estBitrate) {
    estBitrate = 500; // kbps, estimate

    // Create the video source:

    FramedPipeSource* inputSource = FramedPipeSource::createNew(envir(),mFid);
    if (inputSource == NULL) return NULL;

    // Create a framer for the Video Elementary Stream:
    if(mFormat == H264)
        return H264VideoStreamFramer::createNew(envir(), inputSource);
    else
        return H265VideoStreamFramer::createNew(envir(), inputSource);

}
RTPSink* VideoPipeServerMediaSubsession::createNewRTPSink(Groupsock* rtpGroupsock,
                  unsigned char rtpPayloadTypeIfDynamic,
                  FramedSource* ) {
    increaseSendBufferTo(envir(),rtpGroupsock->socketNum(),500*1024);
    if(mFormat == H264)
        return H264VideoRTPSink::createNew(envir(), rtpGroupsock, rtpPayloadTypeIfDynamic);
    else
        return H265VideoRTPSink::createNew(envir(), rtpGroupsock, rtpPayloadTypeIfDynamic);
}
VideoPipeServerMediaSubsession::~VideoPipeServerMediaSubsession()
{
    ::close(mPipeId[0]);
    ::close(mPipeId[1]);
    mPipeId[0] = 0;
    mPipeId[1] = 0;
}
void VideoPipeServerMediaSubsession::pushPkg(uint8_t* buff,int len)
{
    //CXLOG_DBG("write h264");
    //printf("push package %dkb\n",len/1024);
    int off = 0;
    while(len > 0){
        int writed = write(mPipeId[1],buff+off,len);
        if(writed < 0){
            printf("VideoPipeServerMediaSubsession::pushPkg write package faluire\n");
            break;
        }
        len -= writed;
        off += writed;
    }

    //CXLOG_DBG("write h264 ok");
}

