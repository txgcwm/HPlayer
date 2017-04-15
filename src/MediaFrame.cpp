#include "MediaFrame.h"



MediaFrame::MediaFrame(AVFrame *frame)
: m_frame(frame)
, m_pts(0)
, m_dts(0)
{
}

MediaFrame::~MediaFrame()
{
}

AVFrame* MediaFrame::getFrame()
{
    return m_frame;
}

void MediaFrame::setPts(int64_t pts)
{
    m_pts = pts;

    return;
}

void MediaFrame::setDts(int64_t dts)
{
    m_dts = dts;

    return;
}
