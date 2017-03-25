<h1 >GPlay CPP API文档</h1>

<font color=red>开启c++11支持时各API的回调函数可以是lambda</font>

---
| API 名称 | API 说明 | 
| :------ | :-----: |
| gplay::isInGplayEnv() | 当前游戏是否处于 Gplay 环境 |
| gplay::initSDK() | 初始化GPlay SDK |
| gplay::getChannelID() | 游戏当前所在的渠道ID |
| gplay::getNetworkType() | 获取当前网络类型 |
| gplay::preloadResourceBundle() | 加载单个资源包 |
| gplay::preloadResourceBundles() | 加载多个资源包 |
| gplay::backFromResourceBundle() | 自动切分资源模式下停止分配资源到指定资源包 |
| gplay::setPreloadResponseCallback() | 设置加载资源包的响应回调|
| gplay::isLogined() |检查登录状态 |
| gplay::getUserID() |获取用户 ID | 
| gplay::login() | 登录 |
| gplay::quitGame() | 退出游戏 | 
| gplay::share() | 分享游戏 | 
| gplay::pay() | 支付 |
| gplay::createShortcut() |创建桌面快捷图标 |
| gplay::isFunctionSupported() | 判断是否支持某个方法 |
| gplay::callSyncFunc() | 调用同步扩展接口 |
| gplay::callAsyncFunc() | 调用异步扩展接口 |

### API 说明
---
#### gplay::initSDK(appKey, appSecret, privateKey) 获取参数信息
###### 描述
初始化 GplaySDK, 需要在**<font color="red">使用登录支付等API调用前</font>**调用  

###### 参数
initSDK 所使用到的三个参数, 是开发者在 Gplay 开放平台上配置游戏的时候获得的

| 参数 | 类型 | 说明 |
| :---- | :-: | :-- |
| appKey | const string& |"游戏的 Gplay 唯一标识 |
| appSecret | const string& | 游戏的 Gplay 密钥 |
| privateKey | const string& | 游戏的 Gplay 签名盐值 |

###### 结果
无, 将参数初始化到游戏运行时配置中, 用于敏感 API 的加密.

---

#### gplay::getNetworkType() 获取网络类型
###### 描述
获取当前设备所处的网络类型

###### 参数
无

###### 结果
返回网络状态, 可以使用 SDK 提供的枚举判断, 有如下 3 种:

| 返回枚举值 | 说明 |
| :---- | :-- |
| gplay::NetworkType::NO_NETWORK | 设备当前**无网络连接** |
| gplay::NetworkType::MOBILE | 设备当前处于**移动网络**环境下 |
| gplay::NetworkType::WIFI | 设备当前处于**WIFI网络**环境下 |

---

#### gplay::preloadResourceBundle(group, callback) 加载单个资源包
###### 描述
游戏运行模块前调用, 指定当前模块需要用到的资源包, GPlay SDK 会检测判断是否需要下载缺失的资源:

- 本地已存在指定资源包并且和线上最新版一致, 不需要再次下载：callback 会立即被调用;
- 指定的资源包本地不存在、不完整或者线上有更新, 需要下载：GPlay SDK 开始下载指定的资源包, 下载完成后通过 callback 通知游戏。
- <font color=red>GPlay Tools自动划分资源模式下，callback会立即被调用，在下一个资源包加载前游戏访问的资源会被分配到这个资源包。
- 在 preloadResourceBundle 回调之前, 请勿再次调用此接口。
- preloadGroup和backFromGroup应该保持一对一调用，以支持在GPlay Tools上能够正确地划分资源到各自资源包内</font>

###### 参数
| 参数 | 类型 | 名称 | 参数说明 |
| :---- | :-: | :-: | :-- |
| group | string | 资源包名称 | 指定需要确认加载的资源包名称 |
| callback | function | 回调函数 | 成功加载资源包后调用的回调函数 |

###### 结果
| 加载情况 | 效果 |  
| :--: | :-- |  
| 加载成功 | 执行传入的回调函数 |  
| 空间不足 | 弹框提示“空间不足”错误信息, 提示玩家选择**重试**或者**退出** |  
| 校验失败 | 弹框提示“校验失败”错误信息, 提示玩家选择**重试**或者**退出** |   
| 网络异常 | 弹框提示“网络异常”错误信息, 提示玩家选择**重试**或者**退出** | 

###### 说明
接入过程中, 只需要把进入模块时的原有逻辑, 或逻辑入口放在 callback 中即可, 如将

```cpp
//...
SceneGame* scene = SceneGame::createScene();
directorInstance->replaceScene(scene);
```
改为

```cpp
static void* preloadResultCallback(int result, const char* jsonResult) {
    //...
    SceneGame scene = SceneGame::createScene();
    directorInstance->replaceScene(scene);
}
gplay::preloadResourceBundles("group1", preloadResultCallback);
```

---  
#### gplay::preloadResourceBundles(groups, callback) 加载多个资源包
###### 描述
使用此方法, 可以在游戏进入模块前, 指定下载当前模块需要的多个资源包。
<font color=red> 使用GPlay Tools自动分场景功能时，不支持一次加载多个资源包（工具无法明确资源应该划分给哪一个资源包）</font>

###### 参数
| 参数 | 类型 | 名称 | 参数说明 |
| :---- | :-: | :-: | :-- |
| groups | vector | 资源包列表 | 指定要加载的多个资源包列表 |
| callback | function | 回调函数 | 加载资源包完成后调用的回调函数 |
###### 示例
```cpp
std::vector<std::string> scenes;
scenes.push_back("scene1");
scenes.push_back("scene2");
//...
gplay::preloadResourceBundle(scenes, [](int result, const char* jsonResult) {
    auto scene = SceneGame::createScene();
    Director::getInstance()->replaceScene(scene);
});
```

---

####
gplay::backFromResourceBundle(group)
###### 描述
GPlay Tools自动划分资源模式下停止分配资源到指定资源包，建议在场景资源相关联的Scene等节点的cleanup调用。

###### 示例
```cpp
void GameScene::cleanup()
{
    Scene::cleanup();
    gplay::backFromResourceBundle("gameScene");
}

```

---

#### gplay::setPreloadResponseCallback(GPreloadResponseCallback callback) 设置加载资源包的响应回调
###### 描述
设置加载资源包的响应回调，CP可用于实现定制资源包加载的进度界面等。如果CP没有定制需求则不需要调用这个接口。  

######参数说明：
resultCode:

- PRELOAD\_RESULT_SUCCESS：下载成功
- PRELOAD\_RESULT_PROGRESS：下载进行中
- PRELOAD\_RESULT_FAILED：下载失败

errorCode:

- PRELOAD\_ERROR_NETWORK：网络错误
- PRELOAD\_ERROR\_VERIFY_FAILED：校验错误
- PRELOAD\_ERROR\_NO_SPACE：存储空间不足
- PRELOAD\_ERROR_UNKNOWN：未知错误
- PRELOAD\_ERROR_NONE：没有错误

groupName:当前正在下载的资源包  
percent：当前下载进度  
speed：当前下载速度  

###### 示例
```cpp
gplay::setPreloadResponseCallback([&](int resultCode, int errorCode, const std::string& groupName, float percent, float speed) {

});
```

---

#### gplay::isLogined() 检查登录状态
###### 描述
检查游戏在渠道用户系统上的登录状态

###### 参数
无

###### 结果
| 返回布尔值 | 说明 |
| :---- | :-- |
| **true** | 已登录 |
| **false** | 未登录或已断线 |

---

#### gplay::getUserID() 获取用户 ID
###### 描述
获取游戏玩家在渠道用户系统上分配的 ID

###### 参数
无

###### 结果
string 类型, 用户 ID

---
#### gplay::login( callback ) 登录
###### 描述
调用 GplaySDK 的登录服务, GPlay通过回调函数通知游戏登录结果。

###### 参数
| 参数 | 类型 | 名称 | 参数说明 |
| :---- | :-: | :-: | :-- |
| callback | function | 登录结果回调函数 | 登录模块处理完毕后调用, 通知游戏登录结果 |

###### 结果
调用传入的 callback 方法, 会传入两个参数

| 参数 | 类型 | 名称 | 参数说明 |
| :---- | :-: | :-: | :-- |
| code | int | 登录结果代码 | 用于标示此次登录的结果 |
| msg | const char* | 登录结果消息 | 相对于登录结果的一条消息 |

###### 使用说明
游戏传入的回调函数, 需要对此回调结果做出相应的处理, 将登录逻辑移至回调当中, 例:

```cpp
gplay::login([](int result, const char* jsonResult) {
    switch (result)
    {
        case USER_LOGIN_RESULT_SUCCESS:
        {//玩家登录成功
            break;
        }
        case USER_LOGIN_RESULT_FAIL:
        {//玩家登录失败
            break;
        }
        case USER_LOGIN_RESULT_CANCEL:
        {//玩家取消登录
            break;
        }
        case USER_RESULT_NETWROK_ERROR:
        {//发生网络错误
            break;
        }
        case USER_RESULT_USEREXTENSION:
        {//其他扩展的结果类型
            break;
        }
        default:
        {//未知错误
            break;
        }
    }
});

```

#### gplay:quitGame() 退出游戏
###### 描述
游戏开发者需要将结束游戏的 `Director::getInstance()->end();/` 等结束游戏的 API 替换为 `gplay::quitGame()`.

###### 参数
无

###### 结果
关闭游戏, 游戏中的生命周期函数在关闭游戏之前都会被调用到.

---

#### gplay::share( shareParams, callback ) 分享游戏
###### 描述
调用 GplaySDK 的分享游戏方法, 呼唤渠道的分享游戏功能, GPlay通过回调函数通知游戏分享结果.

###### 参数
| 参数 | 类型 | 名称 | 参数说明 |
| :---- | :-: | :-: | :-- |
| shareParams | GplayShareParams | 分享参数 | 对象实例保存着参数键值对 |
| callback | function| 分享结果回调 | 分享功能处理完毕后通知游戏分享结果 |

###### 结果
调用传入的 callback 方法, 会传入两个参数

| 参数 | 类型 | 名称 | 参数说明 |
| :---- | :-: | :-: | :-- |
| code | int | 分享结果代码 | 用于标示此次分享的结果 |
| msg | const char* | 分享结果消息 | 相对于分享结果的一条消息 |

分享接口的结果有如下几种:

| 枚举值 | 说明 |
| :---- | :--: |
| gplay::SHARE\_RESULT_SUCCESS | 分享成功 |
| gplay::SHARE\_RESULT_FAIL | 分享失败 |
| gplay::SHARE\_RESULT_CANCEL | 取消分享 |
| gplay::SHARE\_RESULT\_NETWORK_ERROR | 网络错误 |
| gplay::SHARE\_RESULT_SHAREREXTENSION | 扩展结果 |

##### 使用说明
GplayShareParams 是分享接口需要传入的分享参数类型, 需要提前设置好必填参数, 否则会报错.

```cpp
GplayShareParams params;

params.pageUrl = "http://192.168.1.3";                                -- 分享页面
params.title = "CppSDKDemo";                                          -- 分享标题
params.content = "CocosCppSDKDemo 是 GplaySDK 的线下测试版本";           -- 分享内容
params.imgUrl = "http://192.168.1.3:8888/moon/icon/large_icon.png";   -- 分享图片
params.imgTitle = "large_icon";                                       -- 分享图片标题

gplay::share(params, shareResultCallback)

``` 

```cpp
static void shareResultCallback(int result, const char* jsonResult) {
    switch (result)
    {
        case SHARE_RESULT_SUCCESS:
        {//分享成功
            break;
        }
        default:
            break;
    }
}
```
---

#### gplay::pay( payParams, callback ) 支付
###### 描述
调用 GplaySDK 的支付方法, 呼唤第三方支付系统的支付功能, 成功以后调用回调方法, 通知游戏支付结果.

###### 参数
| 参数 | 类型 | 名称 | 参数说明 |
| :---- | :-: | :-: | :-- |
| payParams | GplayPayParams | 支付参数 | 对象实例保存着参数键值对 |
| callback |  function| 支付结果回调 | 支付功能处理完毕后调用通知游戏支付结果 |  

###### 结果
调用传入的 callback 方法, 会传入两个参数

| 参数 | 类型 | 名称 | 参数说明 |
| :---- | :-: | :-: | :-- |
| code | int | 支付结果代码 | 用于标示此次支付的结果 |
| msg | const char* | 支付结果消息 | 相对于支付结果的一条消息 |


支付接口的结果有如下几种:

| 枚举值 | 说明 |
| :---- | :--: |
| gplay::PAY\_RESULT_SUCCESS | 支付成功 |
| gplay::PAY\_RESULT_FAIL | 支付失败 |
| gplay::PAY\_RESULT_CANCEL | 支付取消 |
| gplay::PAY\_RESULT\_NETWORK_ERROR | 网络错误 |
| gplay::PAY\_RESULT_INVALID | 支付参数不完整 |
| gplay::PAY\_RESULT\_NOW_PAYING | 正在支付 |
| gplay::PAY\_RESULT_PAYEXTENSION | 扩展结果 |

##### 使用说明
GplayPayParams 是支付接口需要传入的支付参数类型, 需要提前设置好必填参数, 否则会报错.

```cpp
static void payResultCallback(int result, const char* jsonResult) {
    switch (result)
    {
        case PAY_RESULT_SUCCESS:
        {//支付成功
            break;
        }
        case PAY_RESULT_FAIL:
        {//支付失败
            break;
        }
        default:
            break;
    }
}
//...
GplayPayParams params;
params.productId = "gplayid001";
params.productName = "monthly card";
params.productPrice = 100;
params.productCount = 100;
params.productDescription = "monthly";
params.gameUserId = "userid001";
params.gameUserName = "zhangjunfei";
params.serverId = "001";
params.serverName = "server01";
params.extraData = "{}";
pay(params, payResultCallback);

```  

---

#### gplay:createShortcut( callback ) 创建桌面快捷图标
###### 描述
调用 GplaySDK 的创建桌面快捷图标功能, 成功以后调用回调方法, 通知游戏是否创建成功.

###### 参数
| 参数 | 类型| 名称 | 参数说明 |
| :-- | :-: | :-: | :-- |
| callback | function | 创建图标结果回调 | 创建图标功能处理完毕后调用通知游戏创建结果 |

###### 结果
调用传入的 callback 方法, 会传入两个参数

| 参数 | 类型 | 名称 | 参数说明 |
| :---- | :-: | :-: | :-- |
| code | int | 创建结果代码 | 用于标示此次创建的结果 |
| msg | const char* | 创建结果消息 | 相对于创建结果的一条消息 |

使用方法请参考 login 接口的例子

创建桌面快捷图标接口的结果有如下几种:

| 枚举值 | 说明 |
| :---- | :--: |
| gplay::SHORTCUT\_RESULT_SUCCESS | 创建成功 |
| gplay::SHORTCUT\_RESULT_FAILED | 创建失败 |

---

#### isFunctionSupported( funcName ) 判断是否支持某个接口
###### 描述
除了 login 等通用接口外，通过此函数可以来判断是否还支持其他接口. 

###### 参数
| 参数 | 类型 | 名称 | 参数说明 |
| :-- | :-: | :-: | :-- |
| funcName | const string& | 方法名称 | 希望调用到的方法名 |

###### 结果
返回一个 bool 类型结果, true 表示支持, false 表示不支持

---

#### gplay:callSyncFunc( funcName, params ) 渠道同步扩展接口
###### 描述
除 Gplay 标准 API 之外, 渠道有其他扩展功能的支持时, 可通用此扩展接口来调用渠道的扩展功能（同步调用）.

###### 参数
| 参数 | 类型 | 名称 | 参数说明 |
| :-- | :-: | :-: | :-- |
| funcName | const string& | 方法名称 | 希望调用到的方法名 |
| params  | const string& | 扩展接口参数 | jsonString, 保存着参数信息 |

###### 结果
返回一个 JSON 格式的 string 字符串, 游戏开发者根据文档从中解析出方法执行的结果

---
#### gplay:callAsyncFunc( funcName, params, callbackFunc ) 渠道异步扩展接口
###### 描述
除 GPlay 标准 API 之外, 渠道有其他扩展功能的支持时, 可通用此扩展接口来调用渠道的扩展功能（异步调用，通过回调函数传递执行结果）.

###### 参数
| 参数 | 类型 | 名称 | 参数说明 |
| :-- | :-: | :-: | :-- |
| funcName | const string& | 方法名称 | 希望调用到的方法名 |
| params  | const string& | 扩展接口参数 | 保存着参数信息 |
| callbackFunc | function | 扩展接口回调 | 异步接口调用, 通知游戏执行结果 |

###### 结果
调用传入的 callbackFunc 方法, 会传入两个参数

| 参数 | 类型 | 名称 | 参数说明 |
| :---- | :-: | :-: | :-- |
| code | int |  执行代码 | 用于标示此次执行的结果 |
| msg | const char* | 消息 | 相对于执行状态的一条消息 |

游戏开发者根据约定的 code 和 msg 获得方法执行的状态, 执行相应的回调代码.
