#ifndef __FFSDL__H__
#define __FFSDL__H__

#include <iostream>
#include <exception>
#include <string>

#include "SDL2/SDL.h"



class InitError: public std::exception
{
public:
    InitError();
    InitError(const std::string&);
    virtual ~InitError() throw();
    virtual const char* what() const throw();

private:
    std::string msg;
};

class SDL {
public:
    SDL(Uint32 flags = 0) throw(InitError);
    bool createWindow();
    bool showWindow();
    static void sdlDelay(unsigned int delay);
    void createTextrue();
    void setVideoWidth(int height);
    void setVideoHeight(int width);
    void setVideoPixFormat(Uint32 format);
    void initRect();
    void setBuffer(void* pixels, int pitch);
    void showFrame(int mSec);
    void setAudioFreq(int freq);
    void setAudioFormat(SDL_AudioFormat);
    void setAudioChannels(int channels);
    void setAudioSilence(int silence);
    void setAudioSamples(int samples);
    void setAudioUserData(void* userdata);
    void setAudioCallBack(SDL_AudioCallback callBack);
    bool playAudio();
    bool pauseAudio();
    virtual ~SDL();

private:
    SDL_Window *window;
    SDL_Renderer *renderer;
    SDL_Texture *texture;
    SDL_Rect rect;
    int vWidth;
    int vHeight;
    void* vPixels;
    int vPitch;
    Uint32 vFormat;
    SDL_AudioSpec wanted_spec;
    bool isAudioOpen;
};

#endif

