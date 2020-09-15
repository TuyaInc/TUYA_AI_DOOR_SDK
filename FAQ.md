####  Q:如果设备已激活，意外删除或破坏了sdk的basepath里的内容，这种情况是不是只能在后台解绑删除后重新注册激活？ 

    A:你也可以删除本地信息重新配网，必须是上次配网的账号才行 

    Q:请问怎样删除本地信息？ 

    A:rm -rf basepath 
    A:mkdir basepath 


    Q:删除basepath后需不需要调用ty_deactivate_sdk？ 

    A:不需要，这时服务端还是处于配网成功状态，所以需要上次账号才能配网,正常 deactivate 是可以用其他账号配网的 

