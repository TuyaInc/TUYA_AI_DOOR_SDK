
#ifdef MEDIA_STREAM

#include "media_test.h"
#include "tuya_media_sdk.h"
#include <thread>
#include <unistd.h>
#include <string.h>

#ifdef __ANDROID__
static std::string paths[5] = {
    "/sdcard/resource/video/tuya_1280_720.h264",
    "/sdcard/resource/video/tuya_640_480.h264",
    "/sdcard/resource/audio/tuya_8k_16bit_mono.pcm",
    "/sdcard/resource/audio/tuya_8k_16bit_mono.pcm",
    "/sdcard/output/recvaudio.pcm"
};
#else
static std::string paths[5] = {
    "./resource/video/tuya_1280_720.h264",
    "./resource/video/tuya_640_480.h264",
    "./resource/audio/tuya_8k_16bit_mono.pcm",
    "./resource/audio/tuya_8k_16bit_mono.pcm",
    "./output/recvaudio.pcm"
};
#endif

FILE* fileaudio = fopen(paths[4].c_str(),"w+");// pcmu
void audiocallback(const void* buf, int len, int rate, int databits, int channel)
{
    printf("len = %d, rate = %d, databits = %d, channel = %d\n ",len, rate, databits, channel);
    fwrite((char*)buf,1,len,fileaudio);
}

namespace tuya
{
    #define AUDIO_FRAME_SIZE 640
    #define I_FRAME_SIZE (1280*720*3/2)
    #define I_FRAME_SIZE_SUB (600*480*3/2)
    MediaTest::MediaTest()
    {
        printf("MediaTest::MediaTest()\n");
        filevideobuf = getVideoFromFile(paths[0],sizevideo);
        filevideobufsub = getVideoFromFile(paths[1],sizevideosub);
        fileaudiomic = getAudioFromFile(paths[2],sizeaudiomic);
        fileaudiospk = getAudioFromFile(paths[3],sizeaudiospk);
    }

    MediaTest::~MediaTest()
    {
        if(filevideobuf != nullptr)
        {
            free(filevideobuf);
            filevideobuf = nullptr;
        }
        if(filevideobufsub != nullptr)
        {
            free(filevideobufsub);
            filevideobufsub = nullptr;
        }
        if(fileaudiomic != nullptr)
        {
            free(fileaudiomic);
            fileaudiomic = nullptr;
        }
        if(fileaudiospk != nullptr)
        {
            free(fileaudiospk);
            fileaudiospk = nullptr;
        }
    }

    unsigned char * MediaTest::getVideoFromFile(std::string path,unsigned int& size)
    {
        unsigned char *pVideoBuf = NULL;

        FILE *fp = fopen(path.c_str(), "rb");
        if (fp == nullptr) {
            printf("can't read open video file %s\n", path.c_str());
            size = 0;
            return nullptr;
        }
        fseek(fp, 0L, SEEK_END);
        unsigned int fileSize = ftell(fp);
        size = fileSize;
        fseek(fp, 0, SEEK_SET);
        pVideoBuf = (unsigned char*)malloc(sizeof(unsigned char) * fileSize);
        fread(pVideoBuf, 1, fileSize, fp);
        fclose(fp);
        return pVideoBuf;
    }

    unsigned char * MediaTest::getAudioFromFile(std::string path, unsigned int& size)
    {
        unsigned char *buf = nullptr;
        FILE *fp = fopen(path.c_str(), "rb");
        if (fp == nullptr) {
            printf("can't read open audio file %s\n", path.c_str());
            size = 0;
            return nullptr;
        }
        fseek(fp, 0, SEEK_END);
        unsigned int fileSize = ftell(fp);
        size = fileSize;
        fseek(fp, 0, SEEK_SET);
        buf = (unsigned char*)malloc(sizeof(unsigned char) * fileSize);
        size_t len = fread(buf, 1, fileSize, fp);
        printf("file length = %d\n",len);
        fclose(fp);
        return buf;
    }

    MediaTest::Frame::FrameType MediaTest::getFrameType(const unsigned char* pVideoBuf, unsigned int pos)
    {
        MediaTest::Frame::FrameType type = static_cast<MediaTest::Frame::FrameType>(pVideoBuf[pos] & 0x1f);
//        if(type == MediaTest::Frame::FrameType::emFrameTypeSPS) // sps 序列参数集
//        {
//         printf("frame type : SPS");
//        }
//        else if(type == MediaTest::Frame::FrameType::emFrameTypePPS) // pps图像参数集
//        {
//         printf("frame type : PPS");
//        }
//        else if(type == MediaTest::Frame::FrameType::emFrameTypeIDR) // idr
//        {
//         printf("frame type : IDR");
//        }
//        else if(type == MediaTest::Frame::FrameType::emFrameTypeP)// not IDR
//        {
//         printf("frame type : P");
//        }
//        else
//        {
//         printf("frame type (%x): not found",type);
//        }

        return type;
    }
    
    void MediaTest::getH264HeaderPos(const unsigned char* pVideoBuf, unsigned int buffLen, unsigned int start, struct Frame* frame)
    {
        unsigned int pos = start;
        bool nextframe = false;
        for(;pos <= buffLen - 4;pos++)
        {
            if(pVideoBuf[pos]==0x00
               &&pVideoBuf[pos+1]==0x00)
            {
                if(pVideoBuf[pos+2]==0x01)
                {
                    if(frame->position.head == -1)// 第一帧
                    {
                        frame->position.head = pos;// 从头开始
                        frame->position.headlen = 3;
                    }
                    else
                    {
                        frame->position.data = pos - 1;
                        nextframe = true;
                        break;
                    }
                    pos += 3;
                }
                else if(pVideoBuf[pos+2]==0x00
                        && pVideoBuf[pos+3]==0x01)
                {
                    if(frame->position.head == -1)// 第一帧
                    {
                        frame->position.head = pos;
                        frame->position.headlen = 4;
                    }
                    else
                    {
                        frame->position.data = pos - 1;
                        nextframe = true;
                        break;
                    }
                    pos += 4;
                }
            }
        }
        if(!nextframe)
        {
            frame->position.data = buffLen - 1;
        }
    }

    void MediaTest::start()
    {
        printf("MediaTest::start()MediaTest::start()");
        ty_media_set_video_resolution(emMediaVideoTypeMain,1280,720);
        ty_media_set_video_resolution(emMediaVideoTypeSub1,600,480);
        ty_media_set_frame_rate(emMediaTypeAudio,30);
        ty_media_set_frame_rate(emMediaTypeVideo,30);
        ty_media_set_audio_call_back(audiocallback);
        ty_start_media();

        std::thread myvideo([&]
        {
            int times = 0;
            unsigned char idrframe[I_FRAME_SIZE] = {0};
            unsigned char pframe[I_FRAME_SIZE] = {0};
            while(1)
            {
                printf("video main replay times left = %d,size = %d\n",times++,sizevideo);
                unsigned int start = 0;
                IDRPositon idrposition;
                while(start < sizevideo)
                {
                    struct Frame frame;
                    getH264HeaderPos(filevideobuf, sizevideo, start, &frame);
                    if(frame.position.head == -1)
                    {
                        memset(idrframe,0,I_FRAME_SIZE);
                        memset(pframe,0,I_FRAME_SIZE);
                        break;
                    }
                    frame.size = frame.position.data - frame.position.head + 1;
                    frame.type = getFrameType(filevideobuf,frame.position.head + frame.position.headlen);
                    if(frame.type == MediaTest::Frame::FrameType::emFrameTypeSPS)
                    {
                        idrposition.spspos = frame.position.head;
                        idrposition.spslen = frame.size;
                    }
                    else if(frame.type == MediaTest::Frame::FrameType::emFrameTypePPS)
                    {
                        idrposition.ppspos = frame.position.head;
                        idrposition.ppslen = frame.size;
                    }
                    else if(frame.type == MediaTest::Frame::FrameType::emFrameTypeIDR)
                    {
                        idrposition.idrpos = frame.position.head;
                        idrposition.idrlen = frame.size;
                        if(idrposition.spslen > 0)
                        {
                            memcpy(idrframe,filevideobuf + idrposition.spspos,idrposition.spslen);
                        }
                        if(idrposition.ppslen > 0)
                        {
                            memcpy(idrframe + idrposition.spslen,filevideobuf + idrposition.ppspos,idrposition.ppslen);
                        }
                        memcpy(idrframe + idrposition.spslen + idrposition.ppslen,filevideobuf + idrposition.idrpos,idrposition.idrlen);
                        ty_push_media_video(emMediaVideoTypeMain,idrframe,frame.size);
                        memset(idrframe,0,I_FRAME_SIZE);
                    }
                    else if(frame.type == MediaTest::Frame::FrameType::emFrameTypeP)
                    {
                        memcpy(pframe,filevideobuf + frame.position.head,frame.size);
                        ty_push_media_video(emMediaVideoTypeMain, pframe,frame.size);
                        memset(pframe,0,I_FRAME_SIZE);
                    }
                    start = frame.position.data;
                }
            }
        });
        myvideo.detach();

        std::thread myvideosub([&]
        {
            int times = 0;
            unsigned char idrframe[I_FRAME_SIZE_SUB] = {0};
            unsigned char pframe[I_FRAME_SIZE_SUB] = {0};
            while(1)
            {
                printf("video sub replay times left = %d,size = %d\n",times++,sizevideosub);
                unsigned int start = 0;
                IDRPositon idrposition;
                while(start < sizevideosub)
                {
                    struct Frame frame;
                    getH264HeaderPos(filevideobufsub, sizevideosub, start, &frame);
                    if(frame.position.head == -1)
                    {
                        memset(idrframe,0,I_FRAME_SIZE_SUB);
                        memset(pframe,0,I_FRAME_SIZE_SUB);
                        break;
                    }
                    frame.size = frame.position.data - frame.position.head + 1;
                    frame.type = getFrameType(filevideobufsub,frame.position.head + frame.position.headlen);
                    if(frame.type == MediaTest::Frame::FrameType::emFrameTypeSPS)
                    {
                        idrposition.spspos = frame.position.head;
                        idrposition.spslen = frame.size;
                    }
                    else if(frame.type == MediaTest::Frame::FrameType::emFrameTypePPS)
                    {
                        idrposition.ppspos = frame.position.head;
                        idrposition.ppslen = frame.size;
                    }
                    else if(frame.type == MediaTest::Frame::FrameType::emFrameTypeIDR)
                    {
                        idrposition.idrpos = frame.position.head;
                        idrposition.idrlen = frame.size;
                        if(idrposition.spslen > 0)
                        {
                            memcpy(idrframe,filevideobufsub + idrposition.spspos,idrposition.spslen);
                        }
                        if(idrposition.ppslen > 0)
                        {
                            memcpy(idrframe + idrposition.spslen,filevideobufsub + idrposition.ppspos,idrposition.ppslen);
                        }
                        memcpy(idrframe + idrposition.spslen + idrposition.ppslen,filevideobufsub + idrposition.idrpos,idrposition.idrlen);
                        ty_push_media_video(emMediaVideoTypeSub1,idrframe,frame.size);
                        memset(idrframe,0,I_FRAME_SIZE_SUB);
                    }
                    else if(frame.type == MediaTest::Frame::FrameType::emFrameTypeP)
                    {
                        memcpy(pframe,filevideobufsub + frame.position.head,frame.size);
                        ty_push_media_video(emMediaVideoTypeSub1, pframe,frame.size);
                        memset(pframe,0,I_FRAME_SIZE_SUB);
                    }
                    start = frame.position.data;
                }
            }
        });
        myvideosub.detach();
        
        std::thread myaudio([&]
        {
            int times = 0;
            #define  NN 640
            char spk[NN];
            char mic[NN];
            while(1)
            {
                printf("audio replay times left = %d,mic len = %d,spk len = %d\n",times++,sizeaudiomic,sizeaudiospk);
                unsigned int offset = 0;
                while (offset < sizeaudiomic)
                {
                    unsigned int left = sizeaudiomic - offset;
                    unsigned int len = AUDIO_FRAME_SIZE;
                    if(left < AUDIO_FRAME_SIZE)
                    {
                        memset(spk,0,NN);
                        memset(mic,0,NN);
                        len = left;
                    }
                    memcpy(mic,fileaudiomic+offset,len);
                    memcpy(spk,fileaudiospk+offset,len);
                    ty_push_media_audio(reinterpret_cast<const unsigned char *>(mic), len,nullptr);
                    offset += len;
                }
            }
            printf("ty_stop_mediaty_stop_media\n");
        });
        myaudio.detach();
    
    }
}

#endif