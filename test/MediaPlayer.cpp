#include "MediaDecoder.h"
#include "MediaBuffer.h"
#include "FFSDL.h"
#define REFRESH_EVENT  (SDL_USEREVENT + 1)

Uint8 sdlBuffer[4096];
int sdlBufferLen = 0;
int startPos = 0;


static void sdl_fill_audio(void *udata, Uint8 *stream, int len) {
    if(len > sdlBufferLen) {
        len = sdlBufferLen;
    }
    if(len > 0) {
        SDL_memset(stream, 0, len);
        SDL_MixAudio(stream, sdlBuffer + startPos, len, SDL_MIX_MAXVOLUME/2);
        sdlBufferLen -= len;
    } else {
        av_log(NULL, AV_LOG_ERROR, "there is no audio!\n");
    }
}

class MediaPlayer {
private:
    MediaBuffer *mediaPktBuffer;
    MediaDecoder *decoder;
    const int audioChannels = 2;

public:
    MediaPlayer();
    int setDataSource(const char* url);
    int prepare();
    void setDisplayParam(int w, int h);
    static void sdlFillAudio(void *userdata, Uint8 *stream, int len);
};

MediaPlayer::MediaPlayer()
{
    mediaPktBuffer = new MediaBuffer();
    decoder = new MediaDecoder();
}

int MediaPlayer::setDataSource(const char *url)
{
    decoder->setDataSource(url);
    return 1;
}

int MediaPlayer::prepare()
{
    decoder->prepare();
    return 1;
}

void MediaPlayer::setDisplayParam(int w, int h)
{
    decoder->setDisPlayWidth(w);
    decoder->setDisPlayHeight(h);
}

int main(int argc, char **argv)
{
    int audioChannels = 2;
    MediaDecoder decoder;
    decoder.setDataSource(argv[1]);
    decoder.prepare();
    decoder.setOutVideoPixFmt(AV_PIX_FMT_YUV420P);
    decoder.setDisPlayWidth(decoder.getVideoWidth());
    decoder.setDisPlayHeight(decoder.getVideoHeight());
    decoder.setOutAudioChannels(audioChannels/**decoder.getChannels()*/);
    decoder.setOutAudioLayout(av_get_default_channel_layout(audioChannels)/**decoder.getAudioLayout()*/);
    decoder.setOutAudioFormat(AV_SAMPLE_FMT_S16/**decoder.getAudioFormat()*/);
    decoder.setOutAudioSampleRate(decoder.getSampleRate());
    decoder.initVideoConvert();
    decoder.initAudioConvert();

    AVPacket *pkt = (AVPacket *)av_malloc(sizeof(AVPacket));
    AVFrame *frame = av_frame_alloc();

    FILE *pcmFile = fopen("test.pcm", "wb+");

    SDL sdl(0);
    SDL_Event event;
    //SDL_LogSetAllPriority(SDL_LOG_PRIORITY_VERBOSE);

    int64_t lastVideoFrameTime = -1;
    int64_t lastVideoFramePts = -1;

    sdl.setVideoWidth(decoder.getVideoWidth());
    sdl.setVideoHeight(decoder.getVideoHeight());
    sdl.setVideoPixFormat(SDL_PIXELFORMAT_IYUV);
    sdl.createWindow();
    sdl.initRect();
    sdl.createTextrue();
    sdl.showWindow();

    sdl.setAudioFreq(decoder.getSampleRate());
    sdl.setAudioChannels(audioChannels);
    sdl.setAudioFormat(AUDIO_S16SYS);
    sdl.setAudioSilence(0);
    sdl.setAudioSamples(4096);
    sdl.setAudioCallBack(sdl_fill_audio);

    MediaBuffer *mediaPktBuffer = new MediaBuffer();

    if(!sdl.playAudio()) {
        exit(0);
    }

    event.type = REFRESH_EVENT;
    SDL_PushEvent(&event);
    while(decoder.getPacket(pkt) >= 0) {
        SDL_WaitEvent(&event);
        if(event.type==SDL_QUIT){
            break;
        }
        if(pkt->stream_index == decoder.getVideoIndex()) {
            mediaPktBuffer->enQueueVideoPacket(pkt);
            /**
            if(decoder.getFrame(pkt, frame) > 0) {
                AVFrame* outFrame = decoder.convertVideoFrame(frame);
                av_log(NULL, AV_LOG_DEBUG, "pkt pts %lld\n", pkt->pts);
                sdl.setBuffer(outFrame->data[0], outFrame->linesize[0]);
                //sdl.setBuffer(frame->data[0], frame->linesize[0]);
                if(lastVideoFrameTime == -1) {
                    sdl.showFrame(0);
                    lastVideoFrameTime = decoder.getCurMs();
                    lastVideoFramePts = decoder.getMsByPts(decoder.getVideoTimeBase(), pkt->pts);
                } else {
                    int64_t curMsTime = decoder.getCurMs();
                    int64_t curPtsTime = decoder.getMsByPts(decoder.getVideoTimeBase(), pkt->pts);
                    int sleepTime = (curMsTime - lastVideoFrameTime) - (curPtsTime - lastVideoFramePts);
                    if(sleepTime >= 0) {
                        av_log(NULL, AV_LOG_ERROR, "sleepTime %d\n", sleepTime);
                        sdl.showFrame(sleepTime);
                    } else {
                        sdl.showFrame(-sleepTime);
                        av_log(NULL, AV_LOG_ERROR, "sleepTime %d\n", -sleepTime);
                    }
                    lastVideoFrameTime = decoder.getCurMs();
                    lastVideoFramePts = decoder.getMsByPts(decoder.getVideoTimeBase(), pkt->pts);
                }
                printf("%d %d %d\n", outFrame->linesize[0], outFrame->linesize[1], outFrame->linesize[2]);
                av_frame_free(&outFrame);
                av_frame_free(&frame);
            }*/
        } else if(pkt->stream_index == decoder.getAudioIndex()) {
            /**
            while(pkt->size > 0) {
                int readN = 0;
                int count = 0;
                if((readN = decoder.getFrame(pkt, frame)) > 0) {
                    AVFrame *outFrame = av_frame_alloc();
                    int len = decoder.convertAudioFrame(frame, outFrame);
                    if(pcmFile) {
                        int n = fwrite(outFrame->data[0], 1, outFrame->linesize[0], pcmFile);
                        int audioLen = outFrame->linesize[0];
                        Uint8 *audioBuffer = outFrame->data[0];
                        while(audioLen > 0) {
                            if(sdlBufferLen <= 0) {
                                if(audioLen >= 4096) {
                                    memcpy(sdlBuffer, audioBuffer, 4096);
                                    audioLen -= 4096;
                                    sdlBufferLen = 4096;
                                } else {
                                    memcpy(sdlBuffer, audioBuffer, audioLen);
                                    sdlBufferLen = audioLen;
                                    audioLen = 0;
                                }
                            }
                        }
                        av_log(NULL, AV_LOG_DEBUG, "outFrame linesize %d %d writen %d readN %d pkt->size %d \n", outFrame->linesize[0], outFrame->linesize[1], n, readN, pkt->size);
                    }
                    pkt->size -= readN;
                    pkt->data += readN;
                    av_frame_free(&outFrame);
                }
            }*/
            mediaPktBuffer->enQueueAudioPacket(pkt);
        }

        event.type = REFRESH_EVENT;
        SDL_PushEvent(&event);
        av_free_packet(pkt);
    }
    
    printf("video count %d audio count %d\n", mediaPktBuffer->getVideoPacketCount(), mediaPktBuffer->getAudioPacketCount());
    av_free_packet(pkt);
    event.type = REFRESH_EVENT;
    SDL_PushEvent(&event);
    SDL_Quit();
    fclose(pcmFile);

    return 0;
}


