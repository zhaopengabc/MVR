#include "FramedPipeSource.h"
#include "CXLog/CXLog.h"
FramedPipeSource::FramedPipeSource(UsageEnvironment& env,FILE* fid,unsigned preferredFrameSize,
                                   unsigned playTimePerFrame):ByteStreamFileSource(env,fid,preferredFrameSize,playTimePerFrame)
{

}
FramedPipeSource* FramedPipeSource::createNew(UsageEnvironment& env,FILE* fid,unsigned preferredFrameSize,
                                              unsigned playTimePerFrame) {
    return new FramedPipeSource(env,fid,preferredFrameSize,playTimePerFrame);
}

FramedPipeSource::~FramedPipeSource(){
    fFid = nullptr;
}
