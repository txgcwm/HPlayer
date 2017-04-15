#ifndef __MEDIAFRAME__H__
#define __MEDIAFRAME__H__

#include <cstdint>

extern "C" {
#include <libavutil/frame.h>
}


class MediaFrame {
public:
    MediaFrame(AVFrame *frame);
    ~MediaFrame();

    AVFrame* getFrame();
    void setPts(int64_t pts);
    void setDts(int64_t dts);

private:
    AVFrame *m_frame;
    int64_t m_pts;
    int64_t m_dts;
};


#endif
