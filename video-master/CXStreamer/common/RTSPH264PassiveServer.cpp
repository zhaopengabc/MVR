#include "RTSPH264PassiveServer.h"
#include "FramedPipeSource.h"
#include <GroupsockHelper.hh>
#include <unistd.h>
#include <fcntl.h>
#include "cf_logger/cf_logger.h"
namespace CXS {
    RTSPH264PassiveServer::RTSPH264PassiveServer()
    {
        pipe(mPipeId);
        //int flags = fcntl(mPipeId[0],F_GETFL);
        //fcntl(mPipeId[0],F_SETFL,flags|O_NONBLOCK);
        //flags = fcntl(mPipeId[1],F_GETFL);
        //fcntl(mPipeId[1],F_SETFL,flags|O_NONBLOCK);
        mFid = fdopen(mPipeId[0],"rb");

        mEnv = nullptr;
    }

    RTSPH264PassiveServer::~RTSPH264PassiveServer()
    {
        ::close(mPipeId[0]);
        ::close(mPipeId[1]);
        mPipeId[0] = 0;
        mPipeId[1] = 0;
    }
    int RTSPH264PassiveServer::pushData(uint8_t* buff,int len){

        cf_log(NULL, CF_LOG_DEBUG,"write h264");
        int off = 0;
        while(len > 0){
            int writed = write(mPipeId[1],buff+off,len);
            if(writed < 0)
                break;
            len -= writed;
            off += writed;
        }
        cf_log(NULL, CF_LOG_DEBUG,"write h264 ok");
        return 0;
    }
    static int play(RTSPH264PassiveServer* server,void (afterPlayingFunc)(void* clientData));
    static void afterPlaying(void* clientData);
    int RTSPH264PassiveServer::establishAndRunServer(uint16_t servPort)
    {
        // Begin by setting up our usage environment:
          TaskScheduler* scheduler = BasicTaskScheduler::createNew();
          mEnv = BasicUsageEnvironment::createNew(*scheduler);

          // Create 'groupsocks' for RTP and RTCP:
          struct in_addr destinationAddress;
          destinationAddress.s_addr = chooseRandomIPv4SSMAddress(*mEnv);
          // Note: This is a multicast address.  If you wish instead to stream
          // using unicast, then you should use the "testOnDemandRTSPServer"
          // test program - not this test program - as a model.

          const unsigned short rtpPortNum = servPort+10;
          const unsigned short rtcpPortNum = rtpPortNum+1;
          const unsigned char ttl = 255;

          const Port rtpPort(rtpPortNum);
          const Port rtcpPort(rtcpPortNum);

          Groupsock rtpGroupsock(*mEnv, destinationAddress, rtpPort, ttl);
          rtpGroupsock.multicastSendOnly(); // we're a SSM source
          Groupsock rtcpGroupsock(*mEnv, destinationAddress, rtcpPort, ttl);
          rtcpGroupsock.multicastSendOnly(); // we're a SSM source

          // Create a 'H264 Video RTP' sink from the RTP 'groupsock':
          OutPacketBuffer::maxSize = 100000;
          mVideoSink = H264VideoRTPSink::createNew(*mEnv, &rtpGroupsock, 96);

          // Create (and start) a 'RTCP instance' for this RTP sink:
          const unsigned estimatedSessionBandwidth = 500; // in kbps; for RTCP b/w share
          const unsigned maxCNAMElen = 100;
          unsigned char CNAME[maxCNAMElen+1];
          gethostname((char*)CNAME, maxCNAMElen);
          CNAME[maxCNAMElen] = '\0'; // just in case
          RTCPInstance* rtcp
          = RTCPInstance::createNew(*mEnv, &rtcpGroupsock,
                        estimatedSessionBandwidth, CNAME,
                        mVideoSink, NULL /* we're a server */,
                        True /* we're a SSM source */);
          // Note: This starts RTCP running automatically

          ::RTSPServer* rtspServer = ::RTSPServer::createNew(*mEnv, servPort);
          if (rtspServer == NULL) {
            *mEnv << "Failed to create RTSP server: " << mEnv->getResultMsg() << "\n";
            return -1;
          }
          ServerMediaSession* sms
            = ServerMediaSession::createNew(*mEnv, "echovideo", "pipe-file",
                   "Session streamed by \"testH264VideoStreamer\"",
                               True /*SSM*/);
          sms->addSubsession(PassiveServerMediaSubsession::createNew(*mVideoSink, rtcp));
          rtspServer->addServerMediaSession(sms);

          char* url = rtspServer->rtspURL(sms);
          *mEnv << "Play this stream using the URL \"" << url << "\"\n";
          delete[] url;

          // Start the streaming:
          *mEnv << "Beginning streaming...\n";
          play(this,afterPlaying);

          mEnv->taskScheduler().doEventLoop(); // does not return

          return 0; // only to prevent compiler warning
    }

    static void afterPlaying(void* clientData) {
        RTSPH264PassiveServer* rtspServer = static_cast<RTSPH264PassiveServer*>(clientData);
      *rtspServer->getEnv() << "...done reading from file\n";
      rtspServer->getVideoSink()->stopPlaying();
      Medium::close(rtspServer->getVideoSource());
      // Note that this also closes the input file that this source read from.

      // Start playing once again:
    }
    int play(RTSPH264PassiveServer* server,void (afterPlayingFunc)(void* clientData)) {
      // Open the input file as a 'byte-stream file source':
      FramedPipeSource* fileSource
        = FramedPipeSource::createNew(*server->getEnv(), server->getFid());
      if (fileSource == NULL) {
        *server->getEnv() << "Unable to open file \""
             << "\" as a FramedPipeSource file source\n";
        return -1;
      }

      FramedSource* videoES = fileSource;

      // Create a framer for the Video Elementary Stream:
      H264VideoStreamFramer* videoSource = H264VideoStreamFramer::createNew(*server->getEnv(), videoES);
      server->setVideoSource(videoSource);

      // Finally, start playing:
      *server->getEnv() << "Beginning to read from file...\n";
      server->getVideoSink()->startPlaying(*videoSource, afterPlayingFunc, server);
      return 0;
    }
}
