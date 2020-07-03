# 文件夹说明	

demo : 门禁 SDK demo	

web_dist: demo 网页


# Updates

### Version: 1.0.1
	1. 添加云端解绑能力。`ty_set_device_callback` 回调会在接收到云端解绑的时候触发。 如果设备不在线云端解绑了，设备在调用 `ty_activate_sdk`时会失败, 这里无法区分是网络错误造成的，还是云端解绑造成的激活失败。因为云端已经没有了设备的绑定信息，无法进行通信。云端解绑尽量在服务端判断到设备在线再执行
	2. 强制解绑。`ty_deactivate_sdk` 添加 `force` 参数

### Version: 1.0.2
	1. 只在注册时候关注上报设备信息的成功与失败。 后续非注册行为，不关注设备信息上报状况。这个更改后就可以一直通过 `ty_set_device_callback` 回调会在接收到云端解绑的消息。防止出现 1.0.1 版本中无法区分云端解绑还是网络错误的情况。

### Version: 1.0.3
	1. 明确 `ty_get_server_time` 接口 时间戳单位为 second。 修复demo 时间设置错误。

### Version: 1.0.4
	1. 上报设备固件版本， 升级时需要的逻辑判断。接口没有变化

### Version: 1.0.5
	1. `SYNC_FACE_DATA_CHANGE_CALLBCK` 添加 `name` 参数

### Version: 1.0.6
	1. `SYNC_STATUS_CHANGE_CALLBACK` 回调函数 `status` 参数添加 `SYNC_STATUS_ALL_REMOVED`状态
	2. 添加 USER_STATUS_XXXX MACRO
	3. 添加 SYNC_STATUS_XXXX MACRO
	4. demo 补充 USER_STATUS_XXXX & SYNC_STATUS_XXXX 使用 case

### Version: 1.0.7
	1. `USER_STATUS_REMOVE` 修改为 0
	2. 添加 `SYNC_FACE_DATA_CHANGE_CALLBCK` 输出 `USER_STATUS_MODIFY` 功能（1.0.6只添加了定义，未实现相关功能)

### Version: 1.0.8
	1. `tuya_upgrade_available_callback` 修改返回值 `bool` to `int`
	2. 修改升级回调添加函数 `addUpgradeCallback` 为 `ty_add_upgrade_callback`
	3. demo 添加 升级相关内容
	4. dp enum 类型上报 bug 修复

### Version: 1.0.9
	1. `ty_report_access` 添加 `temp`, `tempOk` 参数用于体温上传。 
	2. demo 添加体温上传内容

### Version: 1.0.10
	1. 更换 ipc-sdk 修复 SDK_TOO_OLD 报错
	2. 添加remote 配网模式


### Version: 1.0.11
	1. 每次配网前先清除一边遗留配网文件
	2. 修复定时数据同步未生效问题
	3. 完成 云端配网（`ACTIVATE_MODE_REMOTE`） demo

### Version: 1.0.12
	1. 修复 接收不到 mqtt 消息通知问题
	2. 修复无法接收到云端配网失败的问题

### Version: 1.0.13
	1. 修复 历史配网信息由于没有设置 basePath 而造成的清除失败

### Version: 1.0.14
	1. 云端配网切换成生产环境

### Version: 1.0.15
	1. 添加 `ty_set_sdk_timezone_offset_mills` 接口， sdk 默认时区 +8 区

### Version: 1.0.16
	1. 重试注册上报接口 10次， 每次等待300毫秒。 最大等待时常3秒。 减少由于数据不同步造成的配网签名失败。

### Version: 1.0.17
	1. 去除云端配网 CN 区域限制
	2. 移除 `ty_set_sdk_timezone_offset_mills`， 添加 `ty_set_get_timezone_offset_millis_callback`。 目的是为了方便接入方计算时令
	3. 添加 `ty_set_tuya_log_callback` 接口， 提供 sdk 日志输出能力
	4. 修复 aws s3 对象存储的上传&下载 bug

### Version: 1.0.18
	1. 添加云端配网 REGION 配置字段 `ActivateEnv::remoteRegion` ，区域： `TY_REGION_CN`, `TY_REGION_EU`, `TY_REGION_US`

### Version: 1.0.19
	1. 修复云端配网失败的时候， RESET事件未触发的 bug


### Version: 1.0.20
	1. 限制 数据同步通知 的触发频率， 防止同步流程频繁重启

### Version: 1.0.21
	1. 非注册激活时，异步执行设备上报

### Version: 1.0.22
	1. 修改 TUYA_DP_CALLBACK 返回 为 int
	
### Version: 1.0.23
	1. 修复由于class实例被释放后继续使用造成的崩溃。发生在exit() 函数调用时

### Version: 1.0.24
	1. 修复 demo crash：由于 exit 前没有停止 thread pool 引起的crash
	2. 修复 demo crash：mg_send_websocket_frame 不支持多线程，在调用前加锁

### Version: 1.0.25
	1. 增加涂鸦智能app音视频推送配置，设置分辨率/码率
	2. 增加设备音频推送涂鸦智能app功能
	3. 增加主辅码流视频推送到涂鸦智能app功能
	4. 实现设备和涂鸦智能app语音双向对讲功能（需要在app端开启双向对讲功能）
	





	