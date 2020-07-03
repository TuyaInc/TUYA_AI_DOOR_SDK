# 涂鸦门禁 SDK

支持设备: his3516DV300

### 编译
1. 进入 demo 文件夹，使用交叉编译工具，编译 ipc_demo
2. 将编译结果 ipc_demo 推送到设备。`scp ipc_demo dv300:/mnt/mtd/demo/ipc_demo`
3. 将 web_dist 推送到设备，重命名成dist，与 ipc_demo 同目录。`scp -r web_dist dv300:/mnt/mtd/demo/dist`
4. 启动 ipc_demo
5. 通过网页访问 http:\\\\{device_ip}:8000 使用 demo 功能


### 文件说明
1. demo/lib/liball_static.a: SDK静态库
2. demo/include: SDK 头文件
3. demo/third_party: demo 中使用的三方库
4. demo/resource: 推送音视频可以使用的文件流
5. web_dist: demo 中使用的 web 页面文件

### 多媒体（音视频）推送功能
1. 设备必须注册上云端才能实现多媒体推送，默认同时开启音频和视频推送，目前提供主/辅码流两路视频流，一路音频流
2. 只要开启了媒体推送，设备会持续推送音视频到云端，不管涂鸦智能app是否连接上设备，暂未实现关闭推送功能
3. 多媒体推送开发
    1. 配置多媒体推送参数：ty_media_set_video_resolution/ty_media_set_frame_rate
        1. 如果不设置参数，默认视频帧率25，主码流12980*720，辅码流128*128；音频帧率25
        2. 不可设置参数，推送音频格式pcm/8k/8bit/mono，视频比特率主码流1M，辅码流512K
    2. 开启推送模块ty_start_media
    3. 开始推送多媒体ty_push_media_audio/ty_push_media_video
    4. 涂鸦智能app喊话声音会在ty_media_set_audio_call_back设置的回调函数中，上层应用可以做相应操作
4. 视频推送接口ty_push_media_video只支持idr和P帧推送，h264编码格式的视频帧需要组装好后下发到sdk，详见media_test.cpp

### 可能出现的问题

```
./ipc_demo: /lib/a53_softfp_neon-vfpv4/libstdc++.so.6: version `GLIBCXX_3.4.23' not found (required by ./ipc_demo)
./ipc_demo: /lib/a53_softfp_neon-vfpv4/libstdc++.so.6: version `CXXABI_1.3.11' not found (required by ./ipc_demo)
```
从编译工具中找到最新的`libstdc++.so.6` 通过 `LD_LIBRARY_PATH=. ./ipc_demo"` 方式指定加载库位置