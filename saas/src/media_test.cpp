
#ifdef MEDIA_STREAM

#include "media_test.h"
#include "tuya_media_sdk.h"
#include <thread>
#include <unistd.h>

FILE* fileaudio = fopen("./output/recvaudio.pcm","w+");
void audiocallback(const BYTE_T* buf, int len, int rate, int databits, int channel)
{
    printf("len = %d, rate = %d, databits = %d, channel = %d\n ",len, rate, databits, channel);
    fwrite(buf,1,len,fileaudio);
}

namespace tuya
{
    MediaTest::MediaTest()
    {
        filevideobuf = getVideoFromFile(sizevideo);
        fileaudiobuf = getAudioFromFile(sizeaudio);
    }

    MediaTest::~MediaTest()
    {
        if(filevideobuf != nullptr)
        {
            free(filevideobuf);
            filevideobuf = nullptr;
        }
        if(fileaudiobuf != nullptr)
        {
            free(fileaudiobuf);
            fileaudiobuf = nullptr;
        }
    }

    unsigned char * MediaTest::getVideoFromFile(unsigned int& size)
    {
        std::string m_path = "./source/test.h264";

        unsigned char *pVideoBuf = NULL;

        FILE *fp = fopen(m_path.c_str(), "rb");
        if (fp == nullptr) {
            printf("can't read open video file %s\n", m_path.c_str());
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

    unsigned char * MediaTest::getAudioFromFile(unsigned int& size)
    {
        std::string m_path = "./source/test.pcm";

        unsigned char *buf = nullptr;
        FILE *fp = fopen(m_path.c_str(), "rb");
        if (fp == nullptr) {
            printf("can't read open audio file %s\n", m_path.c_str());
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

    void MediaTest::start()
    {
        printf("MediaTest::start()MediaTest::start()");
        ty_media_set_frame_rate(emMediaTypeAudio,30);
        ty_media_set_audio_call_back(audiocallback);
        ty_start_media();

        std::thread myvideo([&]
        {
            int times = 10;
            while(times-- > 0)
            {
                printf("video replay times left = %d,size = %d\n",times,sizevideo);
                ty_push_media(emMediaTypeVideo,filevideobuf,sizevideo);
            }
        });
        myvideo.detach();
        
        std::thread myaudio([&]
        {
            int times = 10;
            while(times-- > 0)
            {
                printf("audio replay times left = %d,size = %d\n",times,sizeaudio);
                ty_push_media(emMediaTypeAudio,fileaudiobuf,sizeaudio);
            }
        });
        myaudio.detach();
    }
}


#endif