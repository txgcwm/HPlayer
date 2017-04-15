#include "MediaBuffer.h"



MediaBuffer::MediaBuffer()
: audioMutex(NULL)
, videoMutex(NULL)
{
    audioMutex = SDL_CreateMutex();
    videoMutex = SDL_CreateMutex();
}

void MediaBuffer::lockAudio()
{
    SDL_LockMutex(audioMutex);
}

void MediaBuffer::unlockAudio()
{
    SDL_UnlockMutex(audioMutex);
}

void MediaBuffer::lockVideo()
{
    SDL_LockMutex(videoMutex);
}

void MediaBuffer::unlockVideo()
{
    SDL_UnlockMutex(videoMutex);
}

bool MediaBuffer::enQueueVideoPacket(AVPacket* packet)
{
    lockVideo();
    QueueNode<AVPacket*> *node = new QueueNode<AVPacket*>(packet);
    videoBuffer.push(node);
    unlockVideo();

    return true;
}

bool MediaBuffer::enQueueAudioPacket(AVPacket *packet)
{
    lockAudio();
    QueueNode<AVPacket*> *node = new QueueNode<AVPacket*>(packet);
    audioBuffer.push(node);
    unlockAudio();

    return true;
}

bool MediaBuffer::deQueueVideoPacket(AVPacket* &packet)
{
    lockVideo();
    QueueNode<AVPacket*> *node = NULL;
    videoBuffer.dequeue(node);
    unlockVideo();

    return node->getVal();
}

bool MediaBuffer::deQueueAudioPacket(AVPacket* &packet)
{
    lockAudio();
    QueueNode<AVPacket*> *node = NULL;
    audioBuffer.dequeue(node);
    unlockAudio();

    return node->getVal();
}

int MediaBuffer::getAudioPacketCount()
{
    lockAudio();
    int size = audioBuffer.getSize();
    unlockAudio();

    return size;
}

int MediaBuffer::getVideoPacketCount()
{
    lockVideo();
    int size = videoBuffer.getSize();
    unlockVideo();

    return size;
}


