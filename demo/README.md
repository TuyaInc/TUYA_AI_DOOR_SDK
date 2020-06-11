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
4. web_dist: demo 中使用的 web 页面文件

### 可能出现的问题

```
./ipc_demo: /lib/a53_softfp_neon-vfpv4/libstdc++.so.6: version `GLIBCXX_3.4.23' not found (required by ./ipc_demo)
./ipc_demo: /lib/a53_softfp_neon-vfpv4/libstdc++.so.6: version `CXXABI_1.3.11' not found (required by ./ipc_demo)
```
从编译工具中找到最新的`libstdc++.so.6` 通过 `LD_LIBRARY_PATH=. ./ipc_demo"` 方式指定加载库位置