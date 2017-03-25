# GplayCPSDK

CP客户端接入SDK

###common
文件夹下是CPP/JS/LUA都需要的实现，包含:   

- preload_layer：默认场景资源加载相关界面  
- prebuilt：二进制版本的gplay_static库，包含Unity SDK Native、Gplay Native等核心实现  
- cocos_bridge：和cocos引擎相关的桥接部分  

###gplay\_for_cpp

###gplay\_for_js

###gplay\_for_lua

###publish.py
python脚本，负责拷贝common文件夹下的内容到CPP/JS/LUA各接入SDK下

###Demo使用流程

- 基于某一个版本的Cocos引擎新建Cocos CPP/JS/LUA工程
- 依据新工程开发语言拷贝Demo下对应的源文件、资源等到新工程内
- 终端下运行 publish.py 脚本
- 依据新工程开发语言选择对应的`gplay_for_xxx`接入SDK，按docs下SDK接入文档集成GPlay CP SDK
- 使用cocos console命令生成Android apk包
- 使用GPlay Tools划分资源并在设备上运行验证相关功能
