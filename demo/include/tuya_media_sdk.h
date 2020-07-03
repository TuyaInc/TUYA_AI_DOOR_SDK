#ifndef AI_PAD_SDK_TUYA_MEDIA_SDK_H
#define AI_PAD_SDK_TUYA_MEDIA_SDK_H

#include <common_type.h>

#ifdef  __cplusplus
extern "C" {
#endif

/**
 * 音频回调函数
 * @param buf       音频数据
 * @param len       音频数据大小
 * @param rate      采样率
 * @param databits  采样比特
 * @param channel   通道号
 */
typedef void (*audio_call_back)(const void* buf, int len, int rate, int databits, int channel);

/**
 * 开启推送媒体流，请先配置完媒体信息，再启动推送组件
 * @param type   推送媒体类型
 * @return       true:成功;false:失败
 */
bool ty_start_media();

/**
 * 推送音频媒体流，请先运行ty_start_media
 * @param mic         采集的媒体缓存
 * @param buflen      媒体缓存大小
 * @param spk         采集声音时spk中播放的媒体缓存，用于回声消除（此功能目前还在开发中，可设置成null）
 * @return            true:成功;false:失败
 */
bool ty_push_media_audio(const unsigned char* mic, int buflen, const unsigned char* spk);

/**
 * 推送视频媒体流，请先运行ty_start_media
 * @param type        码流类型：主码流/辅码流
 * @param buf         媒体缓存
 * @param buflen      媒体缓存大小
 * @return            true:成功;false:失败
 */
bool ty_push_media_video(int type, const unsigned char* buf, int buflen);

/*
 * 配置媒体信息，设置视频主辅码流分辨率
 * @param type      主辅码流类型
 * @param width     分辨率高度
 * @param height    分辨率宽度
*/
bool ty_media_set_video_resolution(MediaVideoType type, int width, int height);

/*
 * 配置媒体信息，设置音视频码率
 * @param type          媒体流类型
 * @param framerate     帧率
*/
bool ty_media_set_frame_rate(MediaType type, int framerate);

/*
 * 配置媒体信息，设置音频回调
 * @param func          回调函数
*/
bool ty_media_set_audio_call_back(audio_call_back func);

#ifdef  __cplusplus
}
#endif

#endif // AI_PAD_SDK_TUYA_MEDIA_SDK_H
