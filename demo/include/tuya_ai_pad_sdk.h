//
// Created by hick on 2019/12/16.
//

#ifndef AI_PAD_SDK_TUYA_AI_PAD_SDK_H
#define AI_PAD_SDK_TUYA_AI_PAD_SDK_H

#include <stdint.h>
#include <common_type.h>

#ifdef  __cplusplus
extern "C" {
#endif

#define TY_SDK_VERSION "1.0.29"

#define  USER_TYPE_MEMBER FACE_TYPE_MEMBER
#define  USER_TYPE_VISITOR FACE_TYPE_VISITOR
#define  USER_TYPE_UNKNOWN FACE_TYPE_UNKNOWN

struct ActivateEnv {
    char *token;            //激活token
    char *basePath;         //工作文件夹基础路径
    char *pid;              //pid 产品ID
    char *uuid;             //uuid 设备唯一标识符
    char *pkey;             //key 与 uuid 匹配的，用于验证uuid是否合法
    char *dbKey;            //数据库加密 key
    uint32_t dbKdfIter;          //数据库 salt 生成迭代次数， 次数越多越安全，但是启动耗时也越久

    char *model;            //设备型号
    char *sn;               // SN
    char *mac;              // 设备 mac 地址
    char *ip;               // 设备局域网 ip 地址
    char *kernelVersion;    // 设备 kernel verison
    char *firmwareVersion;  // 当前固件版本, 版本号会参与固件升级的逻辑判断
    uint64_t activateTimeoutMills; //激活超时时间, ACTIVATE_MODE_REMOTE 模式下忽略此参数
    int activateMode;               //激活方式 ACTIVATE_MODE_WIFI, ACTIVATE_MODE_WIRED, ACTIVATE_MODE_REMOTE
    int remoteRegion;              //云端配网mqtt连接区域， TY_REGION_CN, TY_REGION_EU, TY_REGION_US,
};

struct Member {
    char *uid;
    char *name;
    char *workNo;
    char *ruleIds;
    char *avatarPath;
    int gender; // GENDER_UNKNOWN,GENDER_FEMALE,GENDER_MALE
};


struct Visitor {
    char *uid;
    char *name;
    char *inviteReason;
    char *allowedDate;
    char *avatarPath;
};


struct Rule {
    uint64_t id;
    char *name;
    char *period;
    char *allowedDate;
    char *deniedDate;
};


/**
 * 升级固件信息
 */
#define TUYA_FW_URL_LEN      255       // max length of firmware download url
#define TUYA_FW_MD5_LEN      32        // max length of firmware md5
#define TUYA_SW_VER_LEN      10        // max string length of VERSION
typedef struct TUYA_FW_INFO {
    unsigned char tp;                  // firmware type
    char fw_url[TUYA_FW_URL_LEN + 1];  // firmware download url
    char fw_md5[TUYA_FW_MD5_LEN + 1];  // firmware md5
    char sw_ver[TUYA_SW_VER_LEN + 1];  // firmware version
    unsigned int file_size;            // firmware size in BYTE
} TUYA_FW_INFO;

/**
 * 日志打印回调
 * @param level
 *          TY_LOG_LEVEL_VERBOSE
 *          TY_LOG_LEVEL_DEBUG
 *          TY_LOG_LEVEL_INFO
 *          TY_LOG_LEVEL_WARNING
 *          TY_LOG_LEVEL_ERROR
 *          TY_LOG_LEVEL_FATAL
 *
 * @param msg 日志信息， max_size = 16 * 1024
 */
typedef void (*ty_log_cb)(int level, const char *msg);

/**
 * 获取 时区偏移毫秒数 callback
 * @return timezone offset millisecond
 */
typedef long (*ty_get_timezone_offset_millis)();

/**
 * 升级可用回调
 * @param TUYA_FW_INFO 固件信息
 * @return 0:进行升级； 非0：不进行升级
 */
typedef int (*tuya_upgrade_available_callback)(const TUYA_FW_INFO *);

/**
 * 升级下载完成回调
 * @param TUYA_FW_INFO      固件信息
 * @param downloadResult    0:下载成功;其他值下载失败
 */
typedef void (*tuya_upgrade_download_finished_callback)(const TUYA_FW_INFO *, int downloadResult);

/**
 * 升级进度回调
 * @param TUYA_FW_INFO      固件信息
 * @param totalSize         固件总大小
 * @param dataSize          本次下载数据的大小
 */
typedef void (*tuya_upgrade_progress_callback)(const TUYA_FW_INFO *, uint64_t totalSize, uint64_t dataSize);

/**
 * 同步状态回调
 * @param status
 *          SYNC_STATUS_START
 *          SYNC_STATUS_PROGRESS
 *          SYNC_STATUS_FINISH
 *          SYNC_STATUS_ERROR
 *          SYNC_STATUS_ALL_REMOVED
 *
 * @params msg 附加信息
 */
typedef void(*SYNC_STATUS_CHANGE_CALLBACK)(int status, char *msg);

/**
 * 人脸数据变化回调
 *
 * @param uid user id
 * @param name user name
 * @param avatarPath 用户头像本地文件地址，如果不存在表示图片未下载成功
 * @param type  用户类型，如下
 *              FACE_TYPE_MEMBER
 *              FACE_TYPE_VISITOR
 *              FACE_TYPE_UNKNOWN
 *
 * @param status 用户数据变更状态
 *              USER_STATUS_ADD
 *              USER_STATUS_MODIFY
 *              USER_STATUS_REMOVE
 *
 * @param data type为 FACE_TYPE_VISITOR 时 指针类型是 Visitor *;  type 为 FACE_TYPE_MEMBER 时 指针类型时 Member *
 *
 * @return 人脸注册处理结果
 *             FACE_DATA_ERR_SUCCESS
 *             FACE_DATA_ERR_DOWNLOAD
 *             FACE_DATA_ERR_REGISTER
 *             FACE_DATA_ERR_INFO
 */
typedef int(*SYNC_FACE_DATA_CHANGE_CALLBACK)(char *uid, char *name, char *avatarPath, int type, int status, void *data);

/**
 * 获取DP回调
 * @param event
 * int dpId;
 * short dpType; 类型包含枚举、bool、str、value
 * union _dpValue
 *
 * @return 返回 0 dp功能点设置成功， 返回 非0 设置失败
 */
typedef int (*TUYA_DP_CALLBACK)(DPEvent *event);

/**
 * 设备信息回调
 * @param event
 * ACTIVATED 云端配网激活成功
 * UNBIND 解绑
 * RESET  重制
 *
 */
typedef void (*TUYA_DEVICE_CALLBACK)(DeviceEvent event);

/**
 * 设备DP数据上报
 */
typedef void (*TUYA_DP_STATUS_REPORT_CALLBACK)();

/**
 * 设备信息变更回调，不要再回调外使用参数内容
 */
typedef void (*TUYA_DEVICE_DETAIL_CALLBACK)(DeviceDetail *);

//设置dp回调
void ty_set_dp_callback(TUYA_DP_CALLBACK callback);

//设置设备事件回调
void ty_set_device_callback(TUYA_DEVICE_CALLBACK callback);

//设置dp上报回调
void ty_set_dp_report_callback(TUYA_DP_STATUS_REPORT_CALLBACK callback);

/**
 * 发送dp信息
 * @param event
 * @return 0 success
 */
int ty_DP_report(DPEvent *event);

/**
 *  初始化 SDK
 * @param bashPath  数据初始化基础路径
 * @param kvPassword  kv 数据存储 密码
 * @return
 */
int ty_tuya_ai_pad_initSDK(char *bashPath, char *kvPassword);

/**
 * 释放 sdk 资源，关闭文件
 */
void ty_free_sdk();


/**
 *  设备是否注册接口
 * @return 0 已注册. 非0 未注册
 */
int ty_is_device_activated();

/**
 * 设备激活
 *      ACTIVATE_MODE_REMOTE 模式下，该函数只是启动配网流程，并不表示配网完成。需要等待 TUYA_DEVICE_CALLBACK 回调返回 ACTIVATED 参数
 *
 * @param env 配置
 * @return 0 成功， 非0 失败. 第一次的激活必须成功后才能在服务端激活成功。 随后的激活失败并不影响服务端注册，可以认为可运行（有些设备可能激活之后处于断网状态）
 */
int ty_activate_sdk(ActivateEnv *env);

/**
 * 获取激活时用的token
 * @return token，不用时需要负责释放
 */
char *ty_activated_token();

/**
 * 设备解绑, 解绑之后必须杀死进程重新启动
 * @param force 1:强制解除； 0: 非强制解除;  default 0
 * @return 0 成功， 非0 失败
 */
int ty_deactivate_sdk(int force = 0);

/**
 * 人脸数据同步状态回调
 * @param cb
 */
void ty_set_face_sync_status_change_cb(SYNC_STATUS_CHANGE_CALLBACK cb);

/**
 * 人脸数据状态回调
 * @param cb
 */
void ty_set_face_data_change_cb(SYNC_FACE_DATA_CHANGE_CALLBACK cb);

/**
 * 规则数据同步状态回调
 * @param cb
 */
void ty_set_rule_sync_status_change_cb(SYNC_STATUS_CHANGE_CALLBACK cb);

/**
 * 设置设备信息的回调，当设备信息更新时，会产生回调
 * @param cb
 */
void ty_set_device_detail_cb(TUYA_DEVICE_DETAIL_CALLBACK cb);

/**
 * 获取设备详情
 * @return 调用方负责释放结构体内部 char* 指针
 */
DeviceDetail ty_get_device_detail();

/**
 * 触发人脸同步
 * @param full 1 全量， 0 增量
 * @param remove full 为 1 时才生效， 1 移除全部数据， 0 无任何处理
 */
void ty_trigger_face_sync(int full, int remove);


/**
 * 触发规则同步
 * @param full 1 全量， 0 增量
 * @param remove full 为 1 时才生效， 1 移除全部数据， 0 无任何处理
 */
void ty_trigger_rule_sync(int full, int remove);

/**
 * 获取全部 访客 数据
 * @param p 指向 数组 的指针， 不需要时，需要通过 free_visitor_info 释放
 * @param out_size 全部访客个数
 * @param offset 查询偏移， offset 小于0 表示查询全部内容
 * @param limit 最大输出结果数量
 * @return
 */
void ty_get_all_visitor_info(Visitor **p, uint32_t *out_size, int offset = -1, int limit = 50);

/**
 * 通过 uid 获取访客信息
 * @param uid uid
 * @return null 表示未查询到, 不需要时，需要通过 free_visitor_info 释放
 */
Visitor *ty_get_visitor_by_uid(const char *uid);

/**
 * 访客数据内存释放接口
 * @param p 指向 数组 的指针
 * @param size 数组大小
 */
void ty_free_visitor_info(Visitor *p, uint32_t size);


/**
 * 获取全部 成员 数据
 * @param p 指向 数组 的指针， 不需要时，需要通过 free_member_info 释放
 * @param out_size 全部成员个数
 * @param offset 查询偏移， offset 小于0 表示查询全部内容
 * @param limit 最大输出结果数量
 * @return
 */
void ty_get_all_member_info(Member **p, uint32_t *out_size, int offset = -1, int limit = 50);

/**
 * 通过 uid 获取成员信息
 * @param uid uid
 * @return null 表示未查询到, 不需要时，需要通过 free_member_info 释放
 */
Member *ty_get_member_by_uid(const char *uid);

/**
 * 成员数据释放接口
 * @param p 指向 数组 的指针
 * @param size 数组大小
 */
void ty_free_member_info(Member *p, uint32_t size);


/**
 * 获取全部规则数据
 * @param p 指向 数组 的指针， 不需要时，需要通过 free_rule_info 释放
 * @param out_size 规则数据数量
 */
void ty_get_all_rule_info(Rule **p, uint32_t *out_size);

/**
 * 通过id 获取 rule info
 * @param id  rule id
 * @return null 表示未查询到
 */
Rule *ty_get_rule_by_id(uint64_t id);

/**
 * 释放规则数据指针
 * @param p 指向 数组 的指针
 * @param size 数组大小
 */
void ty_free_rule_info(Rule *p, uint32_t size);

/**
 * 上传访问记录， 此方法比较耗时，需要上传图片到服务器后台
 *
 * 该方法的返回结果是从服务端查询出来的， 一般用于在线判断权限模式
 *
 * @param uid visitor 或 member 的uid， 当为陌生人时，该参数被忽略
 * @param picPath 访问记录照片文件路径
 * @param user_type 用户类型 1 正式成员， 2 访客， 3 陌生人
 * @param timestamp 访问发生时的时间戳, millisecond
 * @param temp 体温测量值。 小于 0 表示未进行体温测量； 大于等于 0 表示当前进行体温测量的体温数值
 * @param tempOk 当前体温值是否在合理体温范围内。 1 表示合理； 0 表示不合理
 * @return 该用户是否有访问权限,服务端判断，0 有访问权限， 非0 没有访问权限
 */
int ty_report_access(const char *uid, const char *picPath, int user_type, uint64_t timestamp, float temp, int tempOk);

/**
 * 通过rules 字段，本地判断该成员是否有访问权限
 * @param rules member.rules 字段
 * @param timestamp 访问发生时的时间戳
 * @return 该用户是否有访问权限，0 有访问权限， 非0 没有访问权限
 */
int ty_member_access_permission_by_rules(char *rules, uint64_t timestamp);

/**
 * 通过 allowed dates 字段，本地判断该成员是否有访问权限
 * @param dates visitor.allowedDate 字段内容
 * @param timestamp 访问发生时的时间戳
 * @return 该用户是否有访问权限，0 有访问权限， 非0 没有访问权限
 */
int ty_visitor_access_permission_by_allowed_date(char *dates, uint64_t timestamp);

/**
 * 设置升级回调
 * @param upgradePath                       升级下载路径
 * @param upgradeAvailableCallback          升级可用callback
 * @param upgradeDownloadFinishedCallback   升级下载完成callback
 * @param upgradeProgressCallback           升级进度callback
 */
void ty_add_upgrade_callback(const char *upgradePath,
                             tuya_upgrade_available_callback upgradeAvailableCallback,
                             tuya_upgrade_download_finished_callback upgradeDownloadFinishedCallback,
                             tuya_upgrade_progress_callback upgradeProgressCallback);


/**
 * 获取服务器时间, 时区通过dp点下发获取
 * @param timestamp timestamp , unit second
 */
void ty_get_server_time(uint64_t *timestamp);

/**
 * 设置 sdk 时区偏差. 使用callback 方式方便计算时令问题
 * @param cb 时区偏差毫秒回调。 如果为 null 默认+8区 始终使用 28800000 偏差
 */
void ty_set_get_timezone_offset_millis_callback(ty_get_timezone_offset_millis cb);


/**
 * 设置涂鸦日志打印输出接口
 *
 * @param cb
 */
void ty_set_tuya_log_callback(ty_log_cb cb);

#ifdef  __cplusplus
};
#endif
#endif //AI_PAD_SDK_TUYA_AI_PAD_SDK_H
