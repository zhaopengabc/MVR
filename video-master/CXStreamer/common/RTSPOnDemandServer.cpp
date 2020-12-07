#include "RTSPOnDemandServer.h"
namespace CXS{
    RTSPOnDemandServer::RTSPOnDemandServer(Format format)
    {
        mSubSersion = nullptr;
        mFormat = format;
    }
    int RTSPOnDemandServer::pushData(uint8_t* buff,int len)
    {
        if(mSubSersion)
        {
            mSubSersion->pushPkg(buff,len);
        }
        return 0;
    }
    static void announceStream(::RTSPServer* rtspServer, ServerMediaSession* sms,
                   char const* streamName);
    int RTSPOnDemandServer::establishAndRunServer(uint16_t servPort){
        OutPacketBuffer::increaseMaxSizeTo(2000000); // bytes
        // Begin by setting up our usage environment:
        TaskScheduler* scheduler = BasicTaskScheduler::createNew();

        UsageEnvironment*  env = BasicUsageEnvironment::createNew(*scheduler);

        UserAuthenticationDatabase* authDB = NULL;

          // Create the RTSP server:
          ::RTSPServer* rtspServer = ::RTSPServer::createNew(*env, servPort, authDB);

          if (rtspServer == NULL) {
            *env << "Failed to create RTSP server: " << env->getResultMsg() << "\n";
            return -1;
          }

          char const* descriptionString
            = "Session streamed by \"testOnDemandRTSPServer\"";

          // Set up each of the possible streams that can be served by the
          // RTSP server.  Each such stream is implemented using a
          // "ServerMediaSession" object, plus one or more
          // "ServerMediaSubsession" objects for each audio/video substream.

          // A H.264 video elementary stream:
          {
            char const* streamName = "echovideo";
            //char const* inputFileName = "test.264";
            ServerMediaSession* sms
              = ServerMediaSession::createNew(*env, streamName, streamName,
                              descriptionString);
            Boolean reuseFirstSource = True;
    //                sms->addSubsession(H264VideoFileServerMediaSubsession
    //                           ::createNew(*env, inputFileName, reuseFirstSource));
            mSubSersion = VideoPipeServerMediaSubsession::createNew(*env,  reuseFirstSource,(VideoPipeServerMediaSubsession::Format)mFormat);
            sms->addSubsession(mSubSersion);
            rtspServer->addServerMediaSession(sms);

            announceStream(rtspServer, sms, streamName);
          }

          // Also, attempt to create a HTTP server for RTSP-over-HTTP tunneling.
          // Try first with the default HTTP port (80), and then with the alternative HTTP
          // port numbers (8000 and 8080).

          //RTSP-over-HTTP 会导致使用tcp传输流
    //              if (rtspServer->setUpTunnelingOverHTTP(80) || rtspServer->setUpTunnelingOverHTTP(8000) || rtspServer->setUpTunnelingOverHTTP(8080)) {
    //                *env << "\n(We use port " << rtspServer->httpServerPortNum() << " for optional RTSP-over-HTTP tunneling.)\n";
    //              } else {
    //                *env << "\n(RTSP-over-HTTP tunneling is not available.)\n";
    //              }

          env->taskScheduler().doEventLoop(); // does not return

          //return 0; // only to prevent compiler warning
        return 0;
    }

    static void announceStream(::RTSPServer* rtspServer, ServerMediaSession* sms,
                   char const* streamName) {
      char* url = rtspServer->rtspURL(sms);
      UsageEnvironment& env = rtspServer->envir();
      env << "\n\"" << streamName << "\" stream, hi3531d \n";
      env << "Play this stream using the URL \"" << url << "\"\n";
      delete[] url;
    }
}
