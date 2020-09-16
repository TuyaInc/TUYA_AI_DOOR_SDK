####  Q:如果设备已激活，意外删除或破坏了sdk的basepath里的内容，这种情况是不是只能在后台解绑删除后重新注册激活？ 

    A:你也可以删除本地信息重新配网，必须是上次配网的账号才行 

    Q:请问怎样删除本地信息？ 

    A:rm -rf basepath 
    A:mkdir basepath 


    Q:删除basepath后需不需要调用ty_deactivate_sdk？ 

    A:不需要，这时服务端还是处于配网成功状态，所以需要上次账号才能配网,正常 deactivate 是可以用其他账号配网的 

#### Q:我们设备还支持“恢复出厂设置”，我想在恢复出厂设置代码里调用ty_deactivate_sdk解除服务端的配网状态。如果此时网络不通，sdk不能跟服务后台同步信息，会怎样？代码里是不是只要判断ty_deactivate_sdk返回值为成功就可以认为解绑成功？ 
    A:对, int ty_deactivate_sdk(int force) , force 需要是 0 

#### Q:我这里已激活成功的设备，烧入错误的pid/uuid/pkey程序跑起来激活失败后，再烧入正确的pid/uuid/pkey但还是激活失败，提示device id为空。帮忙看看后台log能否看出原因？
    A:清空 basepath, 用上次激活的账号重新激活。 激活后不能更换 uuid、pid 和 pkey 