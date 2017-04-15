#include "MediaFrame.h"



MediaFrame::MediaFrame(AVFrame *frame)
{
    this->frame = frame;
    // nextFrame = NULL;
}

AVFrame* MediaFrame::getFrame()
{
    return frame;
}

void MediaFrame::setPts(int64_t pts)
{
    this->pts = pts;
}

void MediaFrame::setDts(int64_t dts)
{
    this->dts = dts;
}
