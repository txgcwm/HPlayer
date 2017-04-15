#ifndef __LOOPBUFFER__H__
#define __LOOPBUFFER__H__

#include <string.h>

class LoopBuffer {
public:
    LoopBuffer(int size);
    bool pushData(unsigned char* data, int len);
    bool getData(unsigned char* data, int len);
    int getSize();
    int getMaxSize();
    int getFree();

private:
    unsigned char* buffer;
    int maxSize;
    int size;
    int startPos;
    int endPos;
};

#endif
