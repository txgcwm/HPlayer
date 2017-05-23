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
    virtual ~SDL();
    
    bool createWindow();
    void setVideoWidthHeight(int width, int height);
    void setBuffer(void* pixels, int pitch);
    void showFrame(int mSec);
    void setAudioFreq(int freq);
    void setAudioSamples(int samples);
    void setAudioUserData(void* userdata);
    void setAudioCallBack(SDL_AudioCallback callBack);
    bool playAudio();
    bool pauseAudio();

private:
    SDL_Window*     window;
    SDL_Renderer*   renderer;
    SDL_Texture*    texture;
    int             vWidth;
    int             vHeight;
    void*           vPixels;
    int             vPitch;
    Uint32          vFormat;
    SDL_AudioSpec   wanted_spec;
    bool            isAudioOpen;
};

#endif

