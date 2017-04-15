#ifndef __MEDIADECODER__H__
#define __MEDIADECODER__H__

extern "C"
{
#include <libavformat/avformat.h>
#include <libavdevice/avdevice.h>
#include <libswscale/swscale.h>
#include <libavcodec/avcodec.h>
#include <libavutil/imgutils.h>
#include <libswresample/swresample.h>
#include <sys/time.h>
}



class MediaDecoder
{
public:
    MediaDecoder();
    void setDataSource(const char *url);
    int prepare();
    int initCodec();
    int getPacket(AVPacket *pkt);
    int getFrame(AVPacket *pkt, AVFrame *frame);

    void setOutVideoWidth(int w);
    void setOutVideoHeight(int h);
    void setOutVideoPixFmt(AVPixelFormat fmt);
    AVFrame* convertVideoFrame(AVFrame *src);
    void initVideoConvert();
    AVRational getVideoTimeBase();
    int getVideoIndex();
    int getDisPlayWidth();
    int getDisPlayHeight();
    int getVideoWidth();
    int getVideoHeight();
    void setDisPlayWidth(int w);
    void setDisPlayHeight(int h);

    int getAudioIndex();
    void initAudioConvert();
    int convertAudioFrame(AVFrame *src, AVFrame *out);
    void setOutAudioFormat(AVSampleFormat fmt);
    void setOutAudioSampleRate(int rate);
    void setOutAudioLayout(uint64_t layout);
    void setOutAudioChannels(int channels);
    uint64_t getAudioLayout();
    int getSampleRate();
    int getChannels();
    AVSampleFormat getAudioFormat();

    static int64_t getMsByPts(AVRational time_base, int64_t pts);
    static int64_t getCurMs();
    static int64_t getCurUs();

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

    char* url;
    AVFormatContext *inputFormatContext;
};

#endif
