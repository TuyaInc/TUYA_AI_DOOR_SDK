//
// Created by hick on 2019/12/16.
//

#ifndef AI_PAD_SDK_COMMON_TYPE_H
#define AI_PAD_SDK_COMMON_TYPE_H

#define FACE_DATA_ERR_SUCCESS  0       //成功
#define FACE_DATA_ERR_DOWNLOAD  1      //图片下载失败
#define FACE_DATA_ERR_REGISTER  2      //图片算法注册失败
#define FACE_DATA_ERR_INFO  3          //用户信息错误

#define  FACE_TYPE_MEMBER 1
#define  FACE_TYPE_VISITOR 2
#define  FACE_TYPE_UNKNOWN 3

#define USER_STATUS_REMOVE 0
#define USER_STATUS_ADD 1
#define USER_STATUS_MODIFY 2

#define SYNC_STATUS_START 1
#define SYNC_STATUS_PROGRESS 2
#define SYNC_STATUS_FINISH 3
#define SYNC_STATUS_ERROR 4
#define SYNC_STATUS_ALL_REMOVED 5           //全量信息移除，触发时机：全量移除方式同步，设备解绑

#define ACTIVATE_MODE_WIRED     1
#define ACTIVATE_MODE_WIFI      2
#define ACTIVATE_MODE_REMOTE      3


#define TY_LOG_LEVEL_VERBOSE        0
#define TY_LOG_LEVEL_DEBUG          1
#define TY_LOG_LEVEL_INFO           2
#define TY_LOG_LEVEL_WARNING        3
#define TY_LOG_LEVEL_ERROR          4
#define TY_LOG_LEVEL_FATAL          5


#define TY_REGION_CN        0
#define TY_REGION_EU        1
#define TY_REGION_US        2

struct DPEvent {
    int dpId;
    short dpType;
    union _dpValue {
        signed int iValue;
        unsigned int enumValue;
        char *strValue;
        int bValue;
        unsigned int bmapValue;
    } dpValue;
};


#ifndef SW_VER_LEN
#define FW_URL_LEN      255       // max length of firmware download url
#endif
#ifndef FW_MD5_LEN
#define FW_MD5_LEN      32        // max length of firmware md5
#endif
#ifndef SW_VER_LEN
#define SW_VER_LEN      20      // max string length of VERSION
#endif

typedef struct {
    unsigned char tp;      // firmware type
    char fw_url[FW_URL_LEN + 1];  // firmware download url
    char fw_md5[FW_MD5_LEN + 1];  // firmware md5
    char sw_ver[SW_VER_LEN + 1];  // firmware version
    unsigned int file_size;             // firmware size in BYTE
} A_FW_UG_S;


#define STAT_CLOUD_CONN         7   // cloud connect

#define PROP_VALUE 1
#define PROP_STR 2
#define PROP_ENUM 3
#define PROP_BOOL 0

#define GENDER_UNKNOWN -1
#define GENDER_FEMALE 0
#define GENDER_MALE 1


typedef char CHAR_T;
typedef unsigned int UINT_T;
typedef unsigned char BYTE_T;

typedef enum {
    ACTIVATED,
    UNBIND,
    RESET
} DeviceEvent;


struct DeviceDetail {
    char *devId;
    char *name;
    char *companyName;
    char *companyLogo;
    char *groupName;
    long groupId;
};

// 媒体类型，音频/视频
enum MediaType {
    emMediaTypeNone = 0,
    emMediaTypeVideo = 1,
    emMediaTypeAudio = 2,
    emMediaTypeAudioAndVideo = emMediaTypeVideo | emMediaTypeAudio
};

// 视频通道号
enum MediaVideoType {
    emMediaVideoTypeMain = 0,
    emMediaVideoTypeSub1,
    emMediaVideoTypeSub2,
    emMediaVideoTypeSub3,
    emMediaVideoTypeCount
};


#endif //AI_PAD_SDK_COMMON_TYPE_H
