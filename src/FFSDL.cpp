#include "FFSDL.h"



InitError::InitError()
: exception()
, msg(SDL_GetError())
{
}
    
InitError::InitError(const std::string& m)
: exception()
, msg(m)
{
}

InitError::~InitError() throw()
{
}

const char* InitError::what() const throw()
{
    return msg.c_str();
}

SDL::SDL(Uint32 flags) throw(InitError)
: isAudioOpen(false)
{
    if (SDL_Init(flags) != 0) {
        throw InitError();
    }

    SDL_memset(&wanted_spec, 0, sizeof(wanted_spec));
}

SDL::~SDL()
{
    SDL_Quit();
}

bool SDL::createWindow()
{
    window = SDL_CreateWindow("HPlayer",
                                SDL_WINDOWPOS_UNDEFINED, SDL_WINDOWPOS_UNDEFINED,
                                vWidth, vHeight,
                                SDL_WINDOW_OPENGL);
    renderer = SDL_CreateRenderer(window, -1, 0);

    initRect();

    texture = SDL_CreateTexture(renderer, vFormat, SDL_TEXTUREACCESS_STREAMING, vWidth, vHeight);

    SDL_ShowWindow(window);

    return true;
}

void SDL::showFrame(int mSec)
{
    SDL_UpdateTexture(texture, NULL, vPixels, vPitch);
    SDL_RenderClear(renderer);
    SDL_RenderCopy(renderer, texture, NULL, NULL);
    SDL_RenderPresent(renderer);
    SDL_Delay(mSec); // ms

    return;
}

void SDL::setVideoWidthHeight(int width, int height)
{
    vWidth = width;
    vHeight = height;

    SDL_Log("width(%d), height(%d)\n", width, height);

    return;
}

void SDL::setVideoPixFormat(Uint32 format)
{
    vFormat = format;
}

bool SDL::playAudio()
{
    if(!isAudioOpen) {
        if(SDL_OpenAudio(&wanted_spec, NULL) < 0) {
            SDL_Log("Failed to open audio: %s\n", SDL_GetError());
            return false;
        }

        isAudioOpen = true;
    }

    SDL_PauseAudio(0);

    return true;
}

bool SDL::pauseAudio()
{
    if(!isAudioOpen) {
        if(SDL_OpenAudio(&wanted_spec, NULL)) {
            SDL_Log("Failed to open audio: %s", SDL_GetError());
            return false;
        }

        isAudioOpen = true;
    }

    SDL_PauseAudio(1);

    return true;
}

void SDL::setAudioFreq(int freq)
{
    printf("*************************** freq: %d\n", freq);
    wanted_spec.freq = freq;
}

void SDL::setAudioFormat(SDL_AudioFormat fmt)
{
    wanted_spec.format = fmt;
}

void SDL::setAudioChannels(int channels)
{
    wanted_spec.channels = channels;
}

void SDL::setAudioSilence(int silence)
{
    wanted_spec.silence = silence;
}

void SDL::setAudioSamples(int samples)
{
    wanted_spec.samples = samples;
}

void SDL::setAudioCallBack(SDL_AudioCallback callback)
{
    wanted_spec.callback = callback;
}

void SDL::setAudioUserData(void *userdata)
{
    wanted_spec.userdata = userdata;
}

void SDL::initRect()
{
    rect.x = 0;
    rect.y = 0;
    rect.w = vWidth;
    rect.h = vHeight;
}

void SDL::setBuffer(void* pixels, int pitch)
{
    vPixels = pixels;
    vPitch = pitch;
}


