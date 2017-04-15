#include "LoopBuffer.h"



LoopBuffer::LoopBuffer(int size)
{
    maxSize = size;
    if(maxSize > 0) {
        buffer = new unsigned char[maxSize];
    }

    startPos = 0;
    endPos = 0;
    size = 0;
}

bool LoopBuffer::pushData(unsigned char *data, int len)
{
    if(len > maxSize - size) {
        return false;
    }

    if(size == 0) {
        memcpy(buffer, data, len);
        endPos = len - 1;
        startPos = 0;
    } else if(endPos >= startPos) {
        int tailLen = maxSize - 1 - endPos;
        if(len <= tailLen) {
            memcpy(buffer + endPos + 1, data, len);
            endPos += len;
        } else {
            memcpy(buffer + endPos + 1, data, tailLen);
            memcpy(buffer, data + tailLen, len - tailLen);
            endPos = len - tailLen - 1;
        }
    } else  {
        memcpy(buffer + endPos + 1, data, len);
        endPos += len;
    }

    size += len;

    return true;
}

bool LoopBuffer::getData(unsigned char* data, int len)
{
    if(len > size || len <= 0) {
        return false;
    }

    if(startPos <= endPos) {
        memcpy(data, buffer + startPos, endPos - startPos -1);
        startPos += len;
    } else {
        int tailLen = maxSize - 1 - startPos;
        memcpy(data, buffer + startPos, tailLen);
        memcpy(data + tailLen, buffer, len - tailLen);
        startPos = len - tailLen - 1;
    }

    size -= len;

    return true;
}

int LoopBuffer::getSize()
{
    return size;
}

int LoopBuffer::getMaxSize()
{
    return maxSize;
}

int LoopBuffer::getFree()
{
    return maxSize - size;
}

