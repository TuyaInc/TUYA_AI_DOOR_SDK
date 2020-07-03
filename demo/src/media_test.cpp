
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
            int times = 20;
            unsigned char* m_idrbuf;
            while(times-- > 0)
            {
                printf("video main replay times left = %d,size = %d\n",times,sizevideo);
                unsigned int start = 0;
                IDRPositon idrposition;
                while(start < sizevideo)
                {
                    struct Frame frame;
                    getH264HeaderPos(filevideobuf, sizevideo, start, &frame);
                    if(frame.position.head == -1)
                    {
                        if(m_idrbuf != nullptr)
                        {
                            free(m_idrbuf);
                        }
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
                        if(m_idrbuf != nullptr)
                        {
                            free(m_idrbuf);
                        }
                        m_idrbuf = (BYTE_T*)malloc(idrposition.spslen + idrposition.ppslen + idrposition.idrlen);

                        if(idrposition.spslen > 0)
                        {
                            memcpy(m_idrbuf,filevideobuf + idrposition.spspos,idrposition.spslen);
                        }
                        if(idrposition.ppslen > 0)
                        {
                            memcpy(m_idrbuf + idrposition.spslen,filevideobuf + idrposition.ppspos,idrposition.ppslen);
                        }
                        memcpy(m_idrbuf + idrposition.spslen + idrposition.ppslen,filevideobuf + idrposition.idrpos,idrposition.idrlen);
                        ty_push_media_video(emMediaVideoTypeMain,m_idrbuf,frame.size);
                        if(m_idrbuf != nullptr)
                        {
                            free(m_idrbuf);
                            m_idrbuf = nullptr;
                        }
                    }
                    else if(frame.type == MediaTest::Frame::FrameType::emFrameTypeP)
                    {
                        m_idrbuf = (BYTE_T*)malloc(frame.size);
                        memcpy(m_idrbuf,filevideobuf + frame.position.head,frame.size);
                        ty_push_media_video(emMediaVideoTypeMain, m_idrbuf,frame.size);
                        if(m_idrbuf != nullptr)
                        {
                            free(m_idrbuf);
                            m_idrbuf = nullptr;
                        }
                    }
                    start = frame.position.data;
                }
            }
        });
        myvideo.detach();

        std::thread myvideosub([&]
        {
            int times = 20;
            unsigned char* m_idrbuf;
            while(times-- > 0)
            {
                printf("video sub replay times left = %d,size = %d\n",times,sizevideosub);
                unsigned int start = 0;
                IDRPositon idrposition;
                while(start < sizevideosub)
                {
                    struct Frame frame;
                    getH264HeaderPos(filevideobufsub, sizevideosub, start, &frame);
                    if(frame.position.head == -1)
                    {
                        if(m_idrbuf != nullptr)
                        {
                            free(m_idrbuf);
                        }
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
                        if(m_idrbuf != nullptr)
                        {
                            free(m_idrbuf);
                        }
                        m_idrbuf = (BYTE_T*)malloc(idrposition.spslen + idrposition.ppslen + idrposition.idrlen);

                        if(idrposition.spslen > 0)
                        {
                            memcpy(m_idrbuf,filevideobufsub + idrposition.spspos,idrposition.spslen);
                        }
                        if(idrposition.ppslen > 0)
                        {
                            memcpy(m_idrbuf + idrposition.spslen,filevideobufsub + idrposition.ppspos,idrposition.ppslen);
                        }
                        memcpy(m_idrbuf + idrposition.spslen + idrposition.ppslen,filevideobufsub + idrposition.idrpos,idrposition.idrlen);
                        ty_push_media_video(emMediaVideoTypeSub1,m_idrbuf,frame.size);
                        if(m_idrbuf != nullptr)
                        {
                            free(m_idrbuf);
                            m_idrbuf = nullptr;
                        }
                    }
                    else if(frame.type == MediaTest::Frame::FrameType::emFrameTypeP)
                    {
                        m_idrbuf = (BYTE_T*)malloc(frame.size);
                        memcpy(m_idrbuf,filevideobufsub + frame.position.head,frame.size);
                        ty_push_media_video(emMediaVideoTypeSub1, m_idrbuf,frame.size);
                        if(m_idrbuf != nullptr)
                        {
                            free(m_idrbuf);
                            m_idrbuf = nullptr;
                        }
                    }
                    start = frame.position.data;
                }
            }
        });
        myvideosub.detach();
        
        std::thread myaudio([&]
        {
            int times = 30;
            #define  NN 640
            char spk[NN];
            char mic[NN];
            while(times-- > 0)
            {
                printf("audio replay times left = %d,mic len = %d,spk len = %d\n",times,sizeaudiomic,sizeaudiospk);
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