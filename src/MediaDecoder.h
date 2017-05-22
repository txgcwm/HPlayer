#ifndef __MEDIADECODER__H__
#define __MEDIADECODER__H__

extern "C" {
#include <libavformat/avformat.h>
#include <libavdevice/avdevice.h>
#include <libswscale/swscale.h>
#include <libavcodec/avcodec.h>
#include <libavutil/imgutils.h>
#include <libswresample/swresample.h>
}

#include <string>



class CMediaDecoder
{
public:
    CMediaDecoder();

    int prepare(const char* url);
    int getPacket(AVPacket *pkt);
    int getFrame(AVPacket *pkt, AVFrame *frame);

    AVFrame* convertVideoFrame(AVFrame *src);
    
    AVRational getVideoTimeBase();
    int getVideoIndex();
    int getVideoWidth();
    int getVideoHeight();
    int getAudioIndex(); 
    int convertAudioFrame(AVFrame *src, AVFrame *out);
    int getSampleRate();

    static int64_t getMsByPts(AVRational time_base, int64_t pts);

private:
    int initCodec();
    void initAudioConvert();
    void initVideoConvert();

private:
    bool hasVideo;
    bool hasAudio;
    int videoIndex;
    int audioIndex;
    int videoWidth;
    int videoHeight;
    int displayWidth;
    int displayHeight;
    AVRational videoTimeBase;
    AVPixelFormat videoPixFmt;
    AVPixelFormat outPixFmt;
    SwsContext *swsVideoCtx;
    SwrContext *swrAudioCtx;
    int audioSampleRate;
    int audioChannels;
    AVSampleFormat audioSampleFormat;
    uint64_t audioLayout;
    int outSampleRate;
    int outChannels;
    AVSampleFormat outSampleFormat;
    int outNbSamples;
    uint64_t outLayout;

    std::string m_url;
    AVFormatContext *inputFormatContext;
};

#endif
