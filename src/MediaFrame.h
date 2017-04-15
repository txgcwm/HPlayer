#ifndef __MEDIAFRAME__H__
#define __MEDIAFRAME__H__

#include <cstdint>

extern "C" {
	#include "libavutil/frame.h"
}


class MediaFrame {
public:
    MediaFrame(AVFrame *frame);
    void setPts(int64_t pts);
    void setDts(int64_t dts);
    AVFrame* getFrame();

private:
    AVFrame *frame;
    int64_t pts;
    int64_t dts;
};


#endif
