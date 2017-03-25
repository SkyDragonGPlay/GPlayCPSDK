<h1 id="top">GPlaySDK 游戏开发接入文档</h1>

<h3>1.简述</h3>
---
GPlay 手机游戏解决方案, 可以让游戏

- 无需安装, 点开即玩  
- 按需下载需要的资源,快速进入游戏  
- 一次接入, 无需再次开发, 可直接上架所有 GPlay 合作渠道  
- 像网页游戏一样灵活地部署和升级  

本文档将介绍游戏如何接入 **GPlaySDK**, 主要内容包括

 - [接入环境要求](#environmental_requirement)
 - [接入步骤](#integrate)
  - [创建游戏](#3_game_create)
  - [集成GPlay SDK](#3_integrate_sdk)  
  - [处理非Cocos引擎功能的JNI调用](#3_deal_jni)
  - [通过 GPlay 接入工具运行游戏, 验证是否有适配问题](#3_run_game)
  - [资源分包及调优游戏体验](#3_divide_resource)
  - [调用GPlay SDK接口](#3_invoke_gplay_api)
  - [发布、测试、上线](#3_release_game)
  - [游戏版本更新](#3_upgrade_game)
 - [GPlay API 文档](#gplay_sdk_api)
 - [GPlay SDK FAQ](#gplay_faq)

<h3 id="environmental_requirement">2.接入环境要求</h3>  
---

- 基于Cocos2d-x 3.3以上（包含3.3）版本的游戏建议使用NDK r10e ([Window](http://dl.google.com/android/ndk/android-ndk-r10e-windows-x86_64.exe)、[Mac](http://dl.google.com/android/repository/android-ndk-r10e-darwin-x86_64.zip))编译本地代码，具体缘由请查看[FAQ](#gplay_faq)。
  
- 基于Cocos2d-x 3.2及以下版本的游戏建议使用NDK r9d ([Windown](https://dl.google.com/android/ndk/android-ndk-r9d-windows-x86_64.zip)、[Mac](http://dl.google.com/android/ndk/android-ndk-r9d-darwin-x86_64.tar.bz2))编译本地代码，具体缘由请查看[FAQ](#gplay_faq)。

更多Android开发相关工具（SDK、NDK、JDK等）可考虑从国内镜像网站[Android Dev tools](http://www.androiddevtools.cn/)下载。  

<h3 id="integrate">3.接入步骤</h3>
---

<h4 id="3_game_create">3.1 创建游戏</h4>
游戏开发者需要登录 [GPlay 开放平台](http://open.skydragon-inc.com) 创建一个游戏, 填写游戏信息. 游戏创建成功以后, 会获得游戏的 <font color=red>`client_id`, `client_secret`, `private_key`</font> 三个参数, 需要在初始化 GPlaySDK 的时候传入, 用于标识游戏和 SDK 信息的传输加密;  

在接入阶段可以使用测试参数来完成接入，三个参数对应的测试值都是<font color=red>`gplaydemo`</font>。测试完成之后务必将这三个参数改成从GPlay 开发平台申请到的值。  

<font color=red>注: 游戏开发者需要保持这三个秘钥的保密性.</font>  

<h4 id="3_integrate_sdk">3.2 集成GPlay SDK</h4>

[Cocos2d-x 3.x CPP 集成GPlay SDK](#cocos_v3_cpp)    
[Cocos2d-x 3.x JS 集成GPlay SDK](#cocos_v3_js)  
[Cocos2d-x 3.x LUA 集成GPlay SDK](#cocos_v3_lua)    

[Cocos2d-x 2.x CPP 集成GPlay SDK](#cocos_v2_cpp)  
[Cocos2d-x 2.x JS 集成GPlay SDK](#cocos_v2_js)  
[Cocos2d-x 2.x LUA 集成GPlay SDK](#cocos_v2_lua)  

<h6 id="cocos_v3_cpp">3.2.1 Cocos2d-x 3.x CPP 集成GPlay SDK</h6>  

- 获取 [GPlaySDKForCPP.zip](), 解压得到 `gplay_for_cpp`文件夹

- 添加对 GPlaySDK 库的依赖: 
  - 复制 `gplay_for_cpp` 文件夹到 游戏Android工程 `pro.android/jni` 目录下 
      
  - 编辑 游戏Android工程 `pro.android/jni` 目录下的 **Android.mk** 文件, 添加对 GPlaySDK 的引用  
  >
```
# 在文件中原 LOCAL_WHOLE_STATIC_LIBRARIES 之后添加
LOCAL_WHOLE_STATIC_LIBRARIES += gplay_static
# ...
# 在文件中原 import-module 之后添加
$(call import-module, ../proj.android/jni/gplay_for_cpp)
```

 - **引擎版本低于 3.4 的游戏**需要在 `cocos/platform/CCFileUtils.h` 中往FileUtils类添加以下函数:
 >
 ```cpp
 	static void setDelegate(FileUtils *delegate) {
 	        if (s_sharedFileUtils != delegate)
 	            delete s_sharedFileUtils;
 	        s_sharedFileUtils = delegate;
	}
 ```
 
 - **引擎版本低于 3.3 的游戏**修改`cocos2dx/platform/android/CCFileUtilsAndroid.h`文件，构造函数的访问权限修改为protected
  >
  ```cpp
  class CC_DLL FileUtilsAndroid : public FileUtils
  {
  protected:
            friend class FileUtils;
            FileUtilsAndroid();
  }
  ```
  
[返回顶部](#top)
 
<h6 id="cocos_v3_js">3.2.2 Cocos2d-x 3.x JS 集成GPlay SDK</h6>

- 获取 [GPlaySDKForJS.zip](), 解压得到 `gplay_for_js`文件夹  

- 添加对 GPlaySDK 库的依赖: 
  - 复制 `gplay_for_js` 文件夹到 游戏Android工程 `pro.android/jni` 目录下。  
  > <font color=red> 3.7以下版本（3.0/3.1/3.2/3.3/3.5/3.6）的JSB工程，复制后请修改`gplay_for_js/Android.mk`文件中`cocos2d_js_static`为`cocos_jsb_static`</font>  
   
  - 编辑 游戏Android工程 `pro.android/jni` 目录下的 **Android.mk** 文件, 增加对 GPlaySDK 的依赖
  > 
```
# ...
# 在文件中原 LOCAL_WHOLE_STATIC_LIBRARIES 之后添加
LOCAL_WHOLE_STATIC_LIBRARIES += gplay_static
# ...
# 在文件中原 import-module 之后添加
$(call import-module, ../runtime-src/proj.android/jni/gplay_for_js)
```  

 - **引擎版本低于 3.4 的游戏**需要在 `cocos/platform/CCFileUtils.h` 中往FileUtils类添加以下函数:
 >
```cpp
 	static void setDelegate(FileUtils *delegate) {
 	        if (s_sharedFileUtils != delegate)
 	            delete s_sharedFileUtils;
 	        s_sharedFileUtils = delegate;
	}
 ``` 
 
 - **引擎版本低于 3.3 的游戏**修改`cocos/platform/android/CCFileUtilsAndroid.h`文件，构造函数的访问权限修改为protected
  >
  ```cpp
  class CC_DLL FileUtilsAndroid : public FileUtils
  {
  protected:
            friend class FileUtils;
            FileUtilsAndroid();
  }
  ```

- 添加 GPlaySDK JS 层接口
  - 复制`gplay_for_js/jsapi`中的JS脚本文件到游戏脚本目录`src`中
  - 添加 **Gplay.js** 到游戏工程 **project.json** 文件的 jsList 列表中 

[返回顶部](#top)

<h6 id="cocos_v3_lua">3.2.3 Cocos2d-x 3.x LUA 集成GPlay SDK</h6>

- 获取 [GPlaySDKForLua.zip](), 解压得到 `gplay_for_lua`

- 添加对 GPlaySDK 库的依赖: 
  - 复制 `gplay_for_lua` 文件夹到 游戏Android工程 `pro.android/jni` 目录下
  
  - <font color=red>基于Quick LUA创建</font>的工程 编辑 游戏Android工程 `pro.android/jni` 目录下的 **android.mk** 文件, 增加对 GPlaySDK 的依赖
  > 
```
# 在文件中原 LOCAL_WHOLE_STATIC_LIBRARIES 之后添加
LOCAL_WHOLE_STATIC_LIBRARIES += gplay_static
# ...
# 在文件中原 import-module 之后添加
$(call import-module, jni/gplay_for_lua)
```  

 - <font color=red>基于Cocos LUA创建</font>的工程 编辑 游戏Android工程 `pro.android/jni` 目录下的 **android.mk** 文件, 增加对 GPlaySDK 的依赖
  > 
```
# 在文件中原 LOCAL_WHOLE_STATIC_LIBRARIES 之后添加
LOCAL_WHOLE_STATIC_LIBRARIES += gplay_static
# ...
# 在文件中原 import-module 之后添加
$(call import-module, ../runtime-src/proj.android/jni/gplay_for_lua)
```  

 - <font color=red>引擎版本低于 3.4 的游戏需要在 `cocos/platform/CCFileUtils.h` 中往FileUtils类添加以下函数:
 >
```cpp
 	static void setDelegate(FileUtils *delegate) {
 	        if (s_sharedFileUtils != delegate)
 	            delete s_sharedFileUtils;
 	        s_sharedFileUtils = delegate;
	}
 ```
 
 - **引擎版本低于 3.3 的游戏**修改`cocos2dx/platform/android/CCFileUtilsAndroid.h`文件，构造函数的访问权限修改为protected
  >
  ```cpp
  class CC_DLL FileUtilsAndroid : public FileUtils
  {
  protected:
            friend class FileUtils;
            FileUtilsAndroid();
  }
  ```
 
 - 引擎版本低于 3.3(3.0/3.1/3.2) 的游戏复制后请修改`gplay_for_lua/Android.mk`文件中`cocos2d_lua_static`为`cocos_lua_static`。</font>

- 添加 GPlaySDK LUA 层接口
  - 复制 `gplay_for_lua/luaapi` 中的LUA脚本文件到游戏 **src/** 中
  - 在游戏代码中添加引用 `require("Gplay")`

[返回顶部](#top)

<h6 id="cocos_v2_cpp">3.2.4 Cocos2d-x 2.x CPP 集成GPlay SDK</h6>

- 获取 [GPlaySDKForCPP.zip](), 解压得到 `gplay_for_cpp`文件夹

- 添加对 GPlaySDK 库的依赖: 
  - 复制 `gplay_for_cpp` 文件夹到 游戏Android工程 `pro.android/jni` 目录下 
      
  - 修改 游戏Android工程 `pro.android/jni` 目录下的 **Android.mk** 文件, 添加对 GPlaySDK 的引用  
  >
```
# 在文件中原 LOCAL_WHOLE_STATIC_LIBRARIES 之后添加
LOCAL_WHOLE_STATIC_LIBRARIES += gplay_static
# ...
# 在文件中原 import-module 之后添加
$(call import-module, jni/gplay_for_cpp)
```

 - 修改游戏Android工程目录下的编译脚本build_native.sh，往NDK\_MODULE\_PATH添加APP\_ANDROID\_ROOT路径。(<font color=red>window下可以使用tools目录下的python脚本编译Native代码，免去安装cygwin等用于执行shell的环境</font>)
 >
 ```shell
 "$NDK_ROOT"/ndk-build -C "$APP_ANDROID_ROOT" $* \
        "NDK_MODULE_PATH=${APP_ANDROID_ROOT}.....
 ```

 - 在 `cocos2dx/platform/CCFileUtils.h`的CCFileUtils类中添加以下函数:
 >
 ```cpp
 	static void setDelegate(CCFileUtils *delegate) {
 	        if (s_sharedFileUtils != delegate)
 	            delete s_sharedFileUtils;
 	        s_sharedFileUtils = delegate;
	}
 ```  
 
  - 修改`cocos2dx/platform/android/CCFileUtilsAndroid.h`文件，构造函数的访问权限修改为protected
  >
  ```cpp
  class CC_DLL CCFileUtilsAndroid : public CCFileUtils
  {
  protected:
            friend class CCFileUtils;
            CCFileUtilsAndroid();
  }
  ```

[返回顶部](#top)

<h6 id="cocos_v2_js">3.2.5 Cocos2d-x 2.x JS 集成GPlay SDK</h6>

- 获取 [GPlaySDKForJS.zip](), 解压得到 `gplay_for_js`文件夹  

- 添加对 GPlaySDK 库的依赖: 
  - 复制 `gplay_for_js` 文件夹到 游戏Android工程 `pro.android/jni` 目录下。  
  > <font color=red> 复制后请修改`gplay_for_js/Android.mk`文件中`cocos2d_js_static`为`scriptingcore-spidermonkey`</font>  
   
  - 编辑 游戏Android工程 `pro.android/jni` 目录下的 **Android.mk** 文件, 增加对 GPlaySDK 的依赖
  > 
```
# ...
# 在文件中原 LOCAL_WHOLE_STATIC_LIBRARIES 之后添加
LOCAL_WHOLE_STATIC_LIBRARIES += gplay_static
# ...
# 在文件中原 import-module 之后添加
$(call import-module, jni/gplay_for_js)
```  

 - 修改游戏Android工程目录下的编译脚本build_native.sh，往NDK\_MODULE\_PATH添加APP\_ANDROID\_ROOT路径。(<font color=red>window下可以考虑使用tools目录下的python脚本编译Native代码，免去安装cygwin等用于执行shell的环境</font>)
 >
 ```
 "$NDK_ROOT"/ndk-build -C "$APP_ANDROID_ROOT" $* \
        "NDK_MODULE_PATH=${APP_ANDROID_ROOT}.....
 ```

 - 在 `cocos2dx/platform/CCFileUtils.h`的CCFileUtils类中添加以下函数:
 >
```cpp
 	static void setDelegate(CCFileUtils *delegate) {
 	        if (s_sharedFileUtils != delegate)
 	            delete s_sharedFileUtils;
 	        s_sharedFileUtils = delegate;
	}
 ``` 
 - 修改`cocos2dx/platform/android/CCFileUtilsAndroid.h`文件，构造函数的访问权限修改为protected
  >
  ```cpp
  class CC_DLL CCFileUtilsAndroid : public CCFileUtils
  {
  protected:
            friend class CCFileUtils;
            CCFileUtilsAndroid();
  }
  ```

- 添加 GPlaySDK JS 层接口
  - 复制`gplay_for_js/jsapi`中的JS脚本文件到游戏`src`中
  - 在游戏脚本中```require Gplay.js```脚本,一般在JS入口文件"cocos2d-jsb.js"中有一个游戏脚本列表，可将Gplay.js加入其中： 
  
  ```javascript  
  require("jsb.js");  
  var appFiles = [  
    'src/resource.js',  
    'src/myApp.js',
    'src/Gplay.js'  
  ];  
  cc.dumpConfig();  
  for( var i=0; i < appFiles.length; i++) {  
      require( appFiles[i] );  
  }  
  ```
    
[返回顶部](#top)

<h6 id="cocos_v2_lua">3.2.6 Cocos2d-x 2.x LUA 集成GPlay SDK</h6>

- 获取 [GPlaySDKForLua.zip](), 解压得到 `gplay_for_lua`

- 添加对 GPlaySDK 库的依赖: 
  - 复制 `gplay_for_lua` 文件夹到 游戏Android工程 `pro.android/jni` 目录下
  - <font color=red> 复制后非Quick Lua请修改`gplay_for_lua/Android.mk`文件中`cocos2d_lua_static`为`cocos_lua_static`, Quick Lua请修改为quickcocos2dx</font>
  
  - 编辑 游戏Android工程 `pro.android/jni` 目录下的 **android.mk** 文件, 增加对 GPlaySDK 的依赖
  
  >  
```
#在文件中原 LOCAL_WHOLE_STATIC_LIBRARIES之后添加
LOCAL_WHOLE_STATIC_LIBRARIES += gplay_static
#在文件中原import-module之后添加
$(call import-module, jni/gplay_for_lua)
```  

 - 修改游戏Android工程目录下的编译脚本build_native.sh，往NDK\_MODULE\_PATH添加APP\_ANDROID\_ROOT路径。(<font color=red>window下可以使用tools目录下的python脚本编译Native代码，免去安装cygwin等用于执行shell的环境</font>)
 >
 ```
 "$NDK_ROOT"/ndk-build -C "$APP_ANDROID_ROOT" $* \
        "NDK_MODULE_PATH=${APP_ANDROID_ROOT}......
 ```
 
 - 需要在 `cocos2dx/platform/CCFileUtils.h`的CCFileUtils类中添加以下函数:
 >
```cpp
 	static void setDelegate(CCFileUtils *delegate) {
 	        if (s_sharedFileUtils != delegate)
 	            delete s_sharedFileUtils;
 	        s_sharedFileUtils = delegate;
	}
 ```
 - 修改`cocos2dx/platform/android/CCFileUtilsAndroid.h`文件，构造函数的访问权限修改为protected
  >
  ```cpp
  class CC_DLL CCFileUtilsAndroid : public CCFileUtils
  {
  protected:
            friend class CCFileUtils;
            CCFileUtilsAndroid();
  }
  ```
  
- <font color=red> 因脚本资源保存路径中文件夹可能包含"."点号，和引擎 2.x LUA模块定义有冲突，所以游戏入口脚本(通常在AppDelegate.cpp)的加载不要使用完整路径。例如：
  >
  ```cpp
  string fullPath = CCFileUtils::sharedFileUtils()-fullPathForFilename("hello.lua");
  luaEngine->executeScriptFile(fullPath.c_str());
  ```

  上面的加载方式需要修改为：
  >
  ```cpp
  luaEngine->executeScriptFile("hello.lua");
  ```
 </font>
 
- 添加 GPlaySDK LUA 层接口
  - 复制 `gplay_for_lua/luaapi` 中的LUA脚本文件到游戏 **src/** 中
  - 在游戏代码中添加引用 `require("Gplay")`
  - <font color=red>使用引擎版本低于2.2.2版本的游戏还需要引用`require("json")`</font>

[返回顶部](#top)

<h4 id="3_deal_jni">3.3 处理非Cocos引擎功能的JNI调用</h4>

GPlay在Java层只提供了Cocos引擎原有功能和GPlay服务的支持，游戏代码如果包含有其他非引擎原有功能的JNI调用及相关依赖等, 可能导致游戏在运行时出现 **JNI相关调用异常**等。

在GPlay模式下需要屏蔽这类调用, 或者将这类功能单独编译成一个dex格式的JAR。
这些调用通常是对第三方SDK的调用, 如统计SDK, 支付SDK, 推送SDK以及开发者在Java层扩展的其他功能等。 

处理非Cocos引擎功能JNI的调用有两种可选方法  

-  使用扩展jar，保留大部分或者所有非引擎功能的JNI调用  
-  完全去除非引擎功能的JNI调用，可使用宏控制是否编译或者运行时通过GPlay的接口(<font color=red>isInGplayEnv</font>)判断是否在GPlay环境

扩展JAR包必须符合以下条件：  
  
- jar必须转成dex格式;  
- 扩展jar不能访问Android SDK任何组件及不能对Android UI做任何控制;  
- 如果Jar中有对JNI的调用，则必须确保游戏so中有此JNI方法;
- 如果游戏native代码中有对Java方法的调用，jar必须正确包含此Java函数的实现;  
- jar包不得与宿主App产生类冲突
 
[返回顶部](#top)

<h4 id="3_run_game">3.4 通过 GPlay 接入工具运行游戏, 验证是否有适配问题</h4>
重新打包游戏APK，这一阶段建议编译 **Debug** 包, 接下来的调试阶段可能需要经常查看游戏和引擎的log输出。  

打开 GPlay 接入工具 **GPlayTool**, 导入生成的游戏 APK 来添加游戏. 连接 Android设备（PC上也可使用海马等模拟器）, 点击右上角**运行游戏**. Android 设备上将会以 GPlay 形式启动游戏, 验证游戏运行是否正常

 - 注: 
   - 工具使用请参考 **"[GPlay 接入工具使用说明文档]()"**
   - 未分资源的情况下运行游戏, 会先下载游戏的所有资源, 主要用于测试适配性, 分过资源以后, 只需要下载 Boot_Scene 里的资源就可以启动游戏
   - 点击**显示日志** 可以查看设备运行的 log
   - 若有修改native代码（C、C++）需要重新编译生成 Debug APK  
   
**完成条件: 通过 GPlay 接入工具运行游戏, 游戏正常运行, 没有 JNI 调用异常等错误**

[返回顶部](#top)

<h4 id="3_divide_resource">3.5 资源分包及调优游戏体验</h4>
- 使用 GPlay 接入工具为游戏资源分包:
 
对于游戏的不同模块如 **战斗**, **商店** 等建立资源分包, 在工具中配置此模块中需要用到的资源。以满足该模块正常运行的资源集合为准, 不同的资源分包可以重复包含其他资源包的资源。  
 
- 修改游戏代码
 
模块运行前先使用 `gplay::preloadResourceBundle(bundleName, successCallback)`加载该模块需要的资源包,在 successCallback 回调中运行模块. (接口使用请参考API文档)
 
- 测试资源包划分是否有遗漏
 
重新打包游戏APK，**关闭** 工具 **静默下载** 开关, 点击 **运行** 启动游戏。工具会按照当前的资源划分情况重新运行游戏, 但不会在后台自动下载资源, 每次加载资源包都会触发前台加载界面. 期间游戏可能由于缺少了必需的资源而导致游戏崩溃, 游戏开发者可以通过查看游戏日志可以获得提示, 找到当前需要加载却没有配置的资源, 加入到对应的资源包. 调整资源分包后继续点击 **运行** 再次启动游戏测试, 直到没有崩溃.

- 测试划分资源后游戏体验是否流畅

**打开** 工具 **静默下载** 开关, 点击 **运行** 启动游戏, 工具会按照当前的资源划分运行游戏, 并且静默下载后续资源, 观察游戏进入前台加载的情况, 如果次数过多, 则需要调整优化资源分包情况. 原则上将最先用到的几个资源包分得细一点, 可以达到比较流程的游戏体验, 尽可能减少出现前台加载资源包.

**完成条件: 通过 GPlay 接入工具运行游戏, 没有崩溃, 没有无法接受的前台加载资源包状况**

- 注:  
  - **这个过程需要多次反复测试以达到最佳效果**
  - **<font color=red>在 preloadResourceBundle 回调之前, 请勿再次调用此接口</font>**
  
[返回顶部](#top)

<h4 id="">3.6 GPlay 模式下屏蔽游戏自身的热更新</h4>
GPlay 模式有一套完善的游戏版本更新机制, 将游戏原本需要的大/小版本更新统一成了 **以资源包为单位** 的 **增量更新**, 使用GPlay Tools发布时会生成升级包随同新版本一起发布到线上. 升级更加方便和顺滑。使用这种方式升级, **游戏开发者需要<font color=red>屏蔽游戏原有的热更新</font>**

[返回顶部](#top)

<h4 id="3_invoke_gplay_api">3.7 调用 GPlaySDK 接口</h4>
- 客户端接入
  - 调用渠道的**登陆**, **支付**, **分享**, **发送到桌面快捷方式**等功能
  - 修改原来结束游戏的接口, 改用 **gplay::quitGame()**  

- 服务端接入
  - 使用 GPlaySDK 的支付接口, 游戏开发者需要在 GPlay 开放平台配置当前游戏的 **支付回调接口**, 客户端支付成功后该回调接口会被调用, 返回支付信息 *( 参考 **GPlaySDK 服务端接入文档** )*

[返回顶部](#top)

<h4 id="3_release_game">3.8 发布, 测试, 上线</h4>
- 发布

修改 jni/Application.mk: `APP_ABI := armeabi armeabi-v7a x86` 生成包含各个架构的 **release** 版本 APK, 在工具中更换 APK 包, 点击 **发布**, 工具将根据最新的资源分包, 生成一套 GPlayPackage, 用于发布. 游戏开发者需要将此文件夹部署到 CDN 上, 并到 GPlay 开放平台上配置 **下载地址** 为游戏包根目录的 URL. 

- 测试

安装工具包中附带的 GplayDemo.apk 到 Android 设备中, 运行并找到自己的游戏, 点开即玩.

- 上线

GPlay 审核游戏过后, 会将游戏上线

[返回顶部](#top)

<h4 id="3_upgrade_game">3.9 游戏版本更新</h4>
游戏在准备正式发布大小版本的时候, 需要同时部署更新对应版本的 GPlayPackage

- 更新新版本资源

使用 GPlay 接入工具导入新版本APK, 工具会自动清理上版本资源配置文件中新版本中不再使用的资源, 其余资源配置保持不变. 游戏开发者根据新版本的资源变动, 回到 **步骤 5**, 将未分配的资源分配到各个资源包中。

- 生成

配置游戏的当前线上地址到工具游戏配置的 **发布地址** 栏目中, 点击 **发布** , 此时生成的 GPlayPackage 将包含对当前线上版本资源的 **增量升级包**, 玩家在进入游戏后会按需自动下载资源**增量升级包** 来升级游戏资源。                                                                    

- 部署

直接使用最新的 GPlayPackage 的内容替换游戏发布 服务器 上的 GPlayPackage 

- 注:
  - GPlay 模式下, 不管是资源、脚本还是本地代码的变化, 都将统一使用 **增量更新**
  - 增量更新的用户可以快速开始新版本的游戏, 按需升级不同的资源包.
  - **<font color=red> GPlay 模式的更新机制统一了游戏的大/小版本更新, 开发者需要屏蔽游戏自身的热更新逻辑</font>**

[返回顶部](#top)

<h3 id="gplay_sdk_api">GPlay API文档<h3>
---

[GPlay CPP API 文档](GPlay客户端 CPP API文档.html)  
[GPlay JS API 文档](GPlay客户端 JS API文档.html)  
[GPlay LUA API 文档](GPlay客户端 LUA API文档.html)  

<h3 id="gplay_faq">GPlay SDK FAQ<h3>
---  

- ####Q: 为什么要使用特定版本的 NDK
>
GPlay 使用的引擎标准 so 是基于 特定版本的NDK编译的. 开发者使用相同版本的 NDK 编译, 可以使游戏在 GPlay 模式下更快地启动（差分包大小更小）

- ####Q: 我该如何划分资源最好?
>
一般基于以下几个原则
 >
  - 初始几个资源包要尽量小, 可以使游戏运行更加流畅
  - 控制游戏首包(Boot_Scene)在 5M 以下, 可以更快速地进入游戏
  - 后续资源包按实际会使用到资源来划分
 
- ####Q: 我该如何减小游戏首包(Boot_Scene)的体积?
>
首场景通常是登录界面, 可以从以下几点下手做到小包优化:
  >
   - 在画质可以接受范围内压缩登录界面图片资源的大小
   - 中文字体库通常较大, 更换更精简的字体库文件可以大大减小包的体积
   - 登陆界面可能用到了 1 分多钟的背景音乐, 考虑改为十几秒音乐循环播放
   
- ####Q: 我的资源包之间有很多重复资源, 会被重复下载吗?
>
工具会对资源包中重复的资源进行优化划分, 确保资源不会在实际运行时被多次下载

- ####Q: 游戏每次 preloadResourceBundles 都会去下载指定的资源吗?
>
SDK 会检查需要加载的资源包本地是否完整, 如果本地已存在对应完整的资源包则直接调用回调函数

- ####Q: 为什么需要屏蔽游戏对非引擎 Java 层代码的调用（JNI调用）?
>
GPlay 模式分离出纯粹的游戏脚本和使用的引擎, 不包含游戏开发者自行添加的 Android 层代码, 如对第三方 SDK, 或其他 Java 开源库. 如果在 GPlay 模式下运行的时候仍然对这些 Android 代码发起调用, 会因为找不到这些方法而崩溃.

- ####Q: 游戏在 GPlay 模式下正常运行, 打包出原生游戏却崩溃要怎么办?
>
GPlay 在运行环境中添加了自己的 Android 层, 响应脚本 gplay::login() 等接口的调用. 游戏在原生 APK 中运行的时候是找不到这些 Android 方法的, 仍然调用这些接口就会出现问题. 建议通过预定义宏控制编译Runtime（GPlay）模式和非Runtime模式（原生）游戏。

- ####Q: x86 架构编译失败, gcc no such file or directory
>
检查 gcc 是否被 360 安全卫士等安全保护软件当作破坏性程序拦截了

[返回顶部](#top)
