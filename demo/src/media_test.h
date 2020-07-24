#ifndef AI_PAD_SDK_MEDIA_TEST_H
#define AI_PAD_SDK_MEDIA_TEST_H
#include<stdio.h>
#include "Singleton.h"
#include <common_type.h>
namespace tuya
{
    class MediaTest : public Singleton<MediaTest>
    {
    private:
        struct Frame
        {
        public:
            enum FrameType
            {
                emFrameTypeP    = 1,
                emFrameTypeIDR  = 5,
                emFrameTypeSPS  = 7,
                emFrameTypePPS  = 8,
            };
            struct Position
            {
            public:
                Position()
                {
                    head = -1;
                    data = 0;
                    headlen = 0;
                }
                unsigned int head;      // 数据开始
                unsigned int data;      // 数据结束
                unsigned int headlen;   // 帧头长度000001还是00000001
            };
        public:
            FrameType type;
            unsigned int size;
            Position position;
        };
        struct IDRPositon
        {
        public:
            IDRPositon()
            {
                spspos = 0;
                spslen = 0;
                ppspos = 0;
                ppslen = 0;
                idrpos = 0;
                idrlen = 0;
            }
            unsigned int spspos;
            unsigned int spslen;
            unsigned int ppspos;
            unsigned int ppslen;
            unsigned int idrpos;
            unsigned int idrlen;
        };
    public:
        MediaTest();
        ~MediaTest();
        bool start();
        bool stop();
        int getMediaState();
    private:
        unsigned char * getVideoFromFile(std::string path, unsigned int& size);
        unsigned char * getAudioFromFile(std::string path, unsigned int& size);
        Frame::FrameType getFrameType(const unsigned char* pVideoBuf, unsigned int pos);
        void getH264HeaderPos(const unsigned char* pVideoBuf, unsigned int buffLen, unsigned int start, struct Frame* frame);
        void startpush();
    private:
        unsigned int sizevideo = 0;
        unsigned char * filevideobuf = nullptr;
        unsigned int sizevideosub = 0;
        unsigned char * filevideobufsub = nullptr;
        unsigned int sizeaudiomic = 0;
        unsigned char * fileaudiomic = nullptr;
        unsigned int sizeaudiospk = 0;
        unsigned char * fileaudiospk = nullptr;
    };
}

#endif