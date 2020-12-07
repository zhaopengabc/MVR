#ifndef FRAMEDPIPESOURCE_H
#define FRAMEDPIPESOURCE_H
#include "ByteStreamFileSource.hh"

class FramedPipeSource: public ByteStreamFileSource
{
protected:
    FramedPipeSource(UsageEnvironment& env,FILE* fid,unsigned preferredFrameSize,
                     unsigned playTimePerFrame);
public:

    static FramedPipeSource* createNew(UsageEnvironment& env,FILE* fid,unsigned preferredFrameSize = 0,
                                       unsigned playTimePerFrame = 0) ;
    //virtual void doGetNextFrame();
    virtual ~FramedPipeSource();
};

#endif // FRAMEDPIPESOURCE_H
