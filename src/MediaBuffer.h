#ifndef __MEDIABUFFER__H__
#define __MEDIABUFFER__H__

extern "C"
{
#include <libavformat/avformat.h>
}

#include "SDL2/SDL.h"

#include "Queue.h"


class MediaBuffer {
public:
    MediaBuffer();
    bool enQueueVideoPacket(AVPacket* packet);
    bool enQueueAudioPacket(AVPacket* packet);
    bool deQueueVideoPacket(AVPacket* &packet);
    bool deQueueAudioPacket(AVPacket* &packet);
    int getVideoPacketCount();
    int getAudioPacketCount();
    void lockAudio();
    void unlockAudio();
    void lockVideo();
    void unlockVideo();

private:
    Queue<AVPacket*> videoBuffer;
    Queue<AVPacket*> audioBuffer;
    SDL_mutex *audioMutex;
    SDL_mutex *videoMutex;
};

#endif
