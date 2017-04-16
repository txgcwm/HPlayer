#include <sys/time.h>

#include "MediaDecoder.h"
#include "MediaBuffer.h"
#include "FFSDL.h"

#define REFRESH_EVENT  (SDL_USEREVENT + 1)

Uint8 sdlBuffer[8192];
int sdlBufferLen = 0;
int startPos = 0;

int64_t getCurMs()
{
    struct timeval start;
    gettimeofday(&start, NULL);

    return start.tv_usec/1000 + start.tv_sec*1000;
}

int64_t getCurUs()
{
    struct timeval start;
    gettimeofday(&start, NULL);

    return start.tv_usec + start.tv_sec*1000*1000;
}

static void sdl_fill_audio(void *udata, Uint8 *stream, int len)
{
    av_log(NULL, AV_LOG_INFO, "audio len: %d!\n", len);

    if(len > sdlBufferLen) {
        len = sdlBufferLen;
    }

    if(len > 0) {
        SDL_memset(stream, 0, len);
        SDL_MixAudio(stream, sdlBuffer + startPos, len, SDL_MIX_MAXVOLUME/2);
        sdlBufferLen -= len;
    }

    return;
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

    av_init_packet(pkt);

    SDL sdl(SDL_INIT_VIDEO | SDL_INIT_AUDIO | SDL_INIT_TIMER);
    SDL_Event event;
    //SDL_LogSetAllPriority(SDL_LOG_PRIORITY_VERBOSE);

    int64_t lastVideoFrameTime = -1;
    int64_t lastVideoFramePts = -1;

    sdl.setVideoWidthHeight(decoder.getVideoWidth(), decoder.getVideoHeight());
    sdl.setVideoPixFormat(SDL_PIXELFORMAT_IYUV);
    sdl.createWindow();

    sdl.setAudioFreq(decoder.getSampleRate());
    sdl.setAudioChannels(audioChannels);
    sdl.setAudioFormat(AUDIO_S16SYS);
    sdl.setAudioSilence(0);
    sdl.setAudioSamples(8192);
    sdl.setAudioCallBack(sdl_fill_audio);

    MediaBuffer *mediaPktBuffer = new MediaBuffer();

    if(!sdl.playAudio()) {
        exit(0);
    }

    event.type = REFRESH_EVENT;
    SDL_PushEvent(&event);

    while(decoder.getPacket(pkt) >= 0) {
        SDL_WaitEvent(&event);
        if(event.type == SDL_QUIT){
            break;
        }

        if(pkt->stream_index == decoder.getVideoIndex()) {
            mediaPktBuffer->enQueueVideoPacket(pkt);

            if(decoder.getFrame(pkt, frame) > 0) {
                AVFrame* outFrame = decoder.convertVideoFrame(frame);
                av_log(NULL, AV_LOG_DEBUG, "pkt pts %ld\n", pkt->pts);
                sdl.setBuffer(outFrame->data[0], outFrame->linesize[0]);
                //sdl.setBuffer(frame->data[0], frame->linesize[0]);
                if(lastVideoFrameTime == -1) {
                    sdl.showFrame(0);
                    lastVideoFrameTime = getCurMs();
                    lastVideoFramePts = decoder.getMsByPts(decoder.getVideoTimeBase(), pkt->pts);
                } else {
                    int64_t curMsTime = getCurMs();
                    int64_t curPtsTime = decoder.getMsByPts(decoder.getVideoTimeBase(), pkt->pts);
                    int sleepTime = (curMsTime - lastVideoFrameTime) - (curPtsTime - lastVideoFramePts);
                    if(sleepTime >= 0) {
                        av_log(NULL, AV_LOG_ERROR, "sleepTime %d ms\n", sleepTime);
                        sdl.showFrame(sleepTime);
                    } else {
                        sdl.showFrame(-sleepTime);
                        av_log(NULL, AV_LOG_ERROR, "sleepTime %d ms\n", -sleepTime);
                    }
                    lastVideoFrameTime = getCurMs();
                    lastVideoFramePts = decoder.getMsByPts(decoder.getVideoTimeBase(), pkt->pts);
                }

                printf("%d %d %d\n", outFrame->linesize[0], outFrame->linesize[1], outFrame->linesize[2]);
                av_frame_free(&outFrame);
            }
        } else if(pkt->stream_index == decoder.getAudioIndex()) {
            mediaPktBuffer->enQueueAudioPacket(pkt);

            while(pkt->size > 0) {
                int readN = 0;
                int count = 0;

                if((readN = decoder.getFrame(pkt, frame)) > 0) {
                    AVFrame *outFrame = av_frame_alloc();
                    int len = decoder.convertAudioFrame(frame, outFrame);

                    // sdl.setBuffer(outFrame->data[0], outFrame->linesize[0]);

                    int audioLen = outFrame->linesize[0];
                    Uint8 *audioBuffer = outFrame->data[0];

                    while(audioLen > 0) {
                        if(sdlBufferLen <= 0) {
                            if(audioLen >= 8192) {
                                memcpy(sdlBuffer, audioBuffer, 8192);
                                audioLen -= 8192;
                                sdlBufferLen = 8192;
                            } else {
                                memcpy(sdlBuffer, audioBuffer, audioLen);
                                sdlBufferLen = audioLen;
                                audioLen = 0;
                            }
                        }
                    }

                    av_log(NULL, AV_LOG_DEBUG, "outFrame linesize(%d, %d) readN: %d pkt->size: %d\n",
                            outFrame->linesize[0], outFrame->linesize[1], readN, pkt->size);

                    pkt->size -= readN;
                    pkt->data += readN;
                    av_frame_free(&outFrame);
                }
            }
        }

        event.type = REFRESH_EVENT;
        SDL_PushEvent(&event);

        // av_free_packet(pkt);
        av_packet_unref(pkt);
    }
    
    printf("video count %d audio count %d\n",
            mediaPktBuffer->getVideoPacketCount(), mediaPktBuffer->getAudioPacketCount());

    av_frame_free(&frame);
    // av_free_packet(pkt);
    av_packet_unref(pkt);

    event.type = REFRESH_EVENT;
    SDL_PushEvent(&event);
    SDL_Quit();

    return 0;
}


