#ifndef AI_PAD_SDK_MEDIA_TEST_H
#define AI_PAD_SDK_MEDIA_TEST_H
#include<stdio.h>
#include "Singleton.h"
namespace tuya
{
    class MediaTest : public Singleton<MediaTest>
    {
    public:
        MediaTest();
        ~MediaTest();
        void start();
    private:
        unsigned char * getVideoFromFile(unsigned int& size);
        unsigned char * getAudioFromFile(unsigned int& size);
    private:
        unsigned int sizevideo = 0;
        unsigned char * filevideobuf = nullptr;
        unsigned int sizeaudio = 0;
        unsigned char * fileaudiobuf = nullptr;
    };
}

#endif