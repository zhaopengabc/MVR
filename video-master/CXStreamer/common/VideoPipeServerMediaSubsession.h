#ifndef H264VIDEOPIPESERVERMEDIASUBSESSION_H
#define H264VIDEOPIPESERVERMEDIASUBSESSION_H
#include "FramedSource.hh"
#include "ServerMediaSession.hh"
#include "OnDemandServerMediaSubsession.hh"
#include <string>
//zxr OnDemandServerMediaSubsession为点播rtsp server 未必支持组播 OnDemandServerMediaSubsession::getStreamParameters函数强制返回isMulticast=False
// 组播参考官方testH264ViderStreamer例程
// PassiveServerMediaSubsession
class VideoPipeServerMediaSubsession : public OnDemandServerMediaSubsession
{
public:
    enum Format{
        H264,
        H265
    } mFormat;
protected:
    VideoPipeServerMediaSubsession(UsageEnvironment& env,  Boolean reuseFirstSource,Format format);
    virtual ~VideoPipeServerMediaSubsession();
protected: // new virtual functions, defined by all subclasses
  virtual FramedSource* createNewStreamSource(unsigned clientSessionId,
                          unsigned& estBitrate) ;
      // "estBitrate" is the stream's estimated bitrate, in kbps
  virtual RTPSink* createNewRTPSink(Groupsock* rtpGroupsock,
                    unsigned char rtpPayloadTypeIfDynamic,
                    FramedSource* inputSource) ;
public:
    static VideoPipeServerMediaSubsession*  createNew(UsageEnvironment& env,  Boolean reuseFirstSource,Format format = H264);
    void pushPkg(uint8_t* buff,int len);

private:
    int mPipeId[2];
    FILE* mFid;

};

#endif // H264VIDEOPIPESERVERMEDIASUBSESSION_H
