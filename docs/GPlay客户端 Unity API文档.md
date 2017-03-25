<h1>GPlay Unity API文档</h1>

---
| API 名称 | API 说明 | 
| :------ | :-----: |
| GPlay.GPlaySDK.IsInGplayEnv() | 当前游戏是否处于 GPlay 环境 |
| GPlay.GPlaySDK.InitSDK() | 初始化GPlay SDK |
| GPlay.GPlaySDK.GetChannelID() | 游戏当前所在的渠道ID |
| GPlay.GPlaySDK.GetNetworkType() | 获取当前网络类型 |
| GPlay.GPlaySDK.PreloadGroup() | 加载单个资源包 |
| GPlay.GPlaySDK.PreloadGroups() | 加载多个资源包 |
| GPlay.GPlaySDK.IsLogined() |检查登录状态 |
| GPlay.GPlaySDK.GetUserID() |获取用户 ID | 
| GPlay.GPlaySDK.Login() | 登录 |
| GPlay.GPlaySDK.QuitGame() | 退出游戏 | 
| GPlay.GPlaySDK.Share() | 分享游戏 | 
| GPlay.GPlaySDK.Pay() | 支付 |
| GPlay.GPlaySDK.CreateShortcut() |创建桌面快捷图标 |
| GPlay.GPlaySDK.IsFunctionSupported() | 判断是否支持某个方法 |
| GPlay.GPlaySDK.CallSyncFunc() | 调用同步扩展接口 |
| GPlay.GPlaySDK.CallAsyncFunc() | 调用异步扩展接口 |


### API 说明
---
#### GPlay.GPlaySDK.InitSDK(appKey, appSecret, privateKey) 获取参数信息
##### 描述
初始化 GPlaySDK, 需要在**<font color="red">使用登录支付等API调用前</font>**调用  

##### 参数
InitSDK 所使用到的三个参数, 是开发者在 GPlay 开放平台上配置游戏的时候获得的

| 参数 | 类型 | 说明 |
| :---- | :-: | :-- |
| appKey | string |"游戏的 GPlay 唯一标识 |
| appSecret | string | 游戏的 GPlay 密钥 |
| privateKey | string | 游戏的 GPlay 签名盐值 |

##### 结果
无, 将参数初始化到游戏运行时配置中, 用于敏感 API 的加密.

---

#### GPlay.GPlaySDK.GetNetworkType() 获取网络类型
##### 描述
获取当前设备所处的网络类型

##### 参数
无

##### 结果
返回网络状态, 可以使用 SDK 提供的枚举判断, 有如下 3 种:

| 返回枚举值 | 说明 |
| :---- | :-- |
| GPlay.ENetworkType.NO_NETWORK | 设备当前**无网络连接** |
| GPlay.ENetworkType.MOBILE | 设备当前处于**移动网络**环境下 |
| GPlay.ENetworkType.WIFI | 设备当前处于**WIFI网络**环境下 |


---
#### GPlay.GPlaySDK.IsLogined() 检查登录状态
##### 描述
检查游戏在渠道用户系统上的登录状态

##### 参数
无

##### 结果
| 返回布尔值 | 说明 |
| :---- | :-- |
| **true** | 已登录 |
| **false** | 未登录或已断线 |

---

#### GPlay.GPlaySDK.GetUserID() 获取用户 ID
##### 描述
获取游戏玩家在渠道用户系统上分配的 ID

##### 参数
无

##### 结果
string 类型, 用户 ID

---
#### GPlay.GPlaySDK.Login( callback ) 登录
##### 描述
调用 GPlaySDK 的登录服务, GPlay通过回调函数通知游戏登录结果。

##### 参数
| 参数 | 类型 | 名称 | 参数说明 |
| :---- | :-: | :-: | :-- |
| callback | GPlay.ActionCallback | 登录结果回调函数 | 登录模块处理完毕后调用, 通知游戏登录结果 |

##### 结果
调用传入的 callback 方法, 会传入两个参数

| 参数 | 类型 | 名称 | 参数说明 |
| :---- | :-: | :-: | :-- |
| resultCode | GPlay.EActionResultCode | 登录结果代码 | 用于标示此次登录的结果 |
| msg | string | 登录结果消息 | 相对于登录结果的一条消息 |

##### 使用说明
游戏传入的回调函数, 需要对此回调结果做出相应的处理, 将登录逻辑移至回调当中, 例:

```C#
GPlay.GPlaySDK.Login(GPlay.GPlaySDK.Login((result, msg) =>
{
    switch (result)
    {
        case GPlay.EActionResultCode.USER_LOGIN_RESULT_SUCCESS:
        {//玩家登录成功
            break;
        }
        case GPlay.EActionResultCode.USER_LOGIN_RESULT_FAIL:
        {//玩家登录失败
            break;
        }
        case GPlay.EActionResultCode.USER_LOGIN_RESULT_CANCEL:
        {//玩家取消登录
            break;
        }
        case GPlay.EActionResultCode.USER_RESULT_NETWROK_ERROR:
        {//发生网络错误
            break;
        }
        case GPlay.EActionResultCode.USER_RESULT_USEREXTENSION:
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
---
#### GPlay.GPlaySDK.QuitGame() 退出游戏
##### 描述
游戏开发者需要将结束游戏的 `Application.Quit()` 等结束游戏的 API 替换为 `GPlay.GPlaySDK.QuitGame()`.

##### 参数
无

##### 结果
关闭游戏, 游戏中的生命周期函数在关闭游戏之前都会被调用到.

---
#### GPlay.GPlaySDK.Share( shareParams, callback ) 分享游戏
##### 描述
调用 GPlaySDK 的分享游戏方法, 呼唤渠道的分享游戏功能, GPlay通过回调函数通知游戏分享结果.

##### 参数
| 参数 | 类型 | 名称 | 参数说明 |
| :---- | :-: | :-: | :-- |
| shareParams | GPlay.GPlayShareParams | 分享参数 | 对象实例保存着参数键值对 |
| callback | GPlay.ActionCallback| 分享结果回调 | 分享功能处理完毕后通知游戏分享结果 |

##### 结果
调用传入的 callback 方法, 会传入两个参数

| 参数 | 类型 | 名称 | 参数说明 |
| :---- | :-: | :-: | :-- |
| resultCode | GPlay.EActionResultCode | 分享结果代码 | 用于标示此次分享的结果 |
| msg | string | 分享结果消息 | 相对于分享结果的一条消息 |

分享接口的结果有如下几种:

| 枚举值 | 说明 |
| :---- | :--: |
| GPlay.EActionResultCode.SHARE\_RESULT_SUCCESS | 分享成功 |
| GPlay.EActionResultCode.SHARE\_RESULT_FAIL | 分享失败 |
| GPlay.EActionResultCode.SHARE\_RESULT_CANCEL | 取消分享 |
| GPlay.EActionResultCode.SHARE\_RESULT\_NETWORK_ERROR | 网络错误 |
| GPlay.EActionResultCode.SHARE\_RESULT_SHAREREXTENSION | 扩展结果 |

##### 使用说明
GPlayShareParams 是分享接口需要传入的分享参数类型, 需要提前设置好必填参数, 否则会报错.

```C#
GPlay.GPlayShareParams params;

params.pageUrl = "http://192.168.1.3";                                -- 分享页面
params.title = "UnitySDKDemo";                                          -- 分享标题
params.content = "UnitySDKDemo 是 GPlaySDK 的线下测试版本";           -- 分享内容
params.imgUrl = "http://192.168.1.3:8888/moon/icon/large_icon.png";   -- 分享图片
params.imgTitle = "large_icon";                                       -- 分享图片标题

GPlay.GPlaySDK.Share(params, shareResultCallback)

``` 

```C#
static void shareResultCallback(GPlay.EActionResultCode resultCode, string msg)
{
    switch (resultCode)
    {
        case GPlay.EActionResultCode.SHARE_RESULT_SUCCESS:
        {//分享成功
            break;
        }
        default:
            break;
    }
}
```

---
#### GPlay.GPlaySDK.Pay( payParams, callback ) 支付
##### 描述
调用 GPlaySDK 的支付方法, 呼唤第三方支付系统的支付功能, 成功以后调用回调方法, 通知游戏支付结果.

##### 参数
| 参数 | 类型 | 名称 | 参数说明 |
| :---- | :-: | :-: | :-- |
| payParams | GPlay.GPlayPayParams | 支付参数 | 对象实例保存着参数键值对 |
| callback |  GPlay.ActionCallback | 支付结果回调 | 支付功能处理完毕后调用通知游戏支付结果 |  

##### 结果
调用传入的 callback 方法, 会传入两个参数

| 参数 | 类型 | 名称 | 参数说明 |
| :---- | :-: | :-: | :-- |
| resultCode | GPlay.EActionResultCode | 支付结果代码 | 用于标示此次支付的结果 |
| msg | string | 支付结果消息 | 相对于支付结果的一条消息 |


支付接口的结果有如下几种:

| 枚举值 | 说明 |
| :---- | :--: |
| GPlay.EActionResultCode.PAY\_RESULT_SUCCESS | 支付成功 |
| GPlay.EActionResultCode.PAY\_RESULT_FAIL | 支付失败 |
| GPlay.EActionResultCode.PAY\_RESULT_CANCEL | 支付取消 |
| GPlay.EActionResultCode.PAY\_RESULT\_NETWORK_ERROR | 网络错误 |
| GPlay.EActionResultCode.PAY\_RESULT_INVALID | 支付参数不完整 |
| GPlay.EActionResultCode.PAY\_RESULT\_NOW_PAYING | 正在支付 |
| GPlay.EActionResultCode.PAY\_RESULT_PAYEXTENSION | 扩展结果 |

##### 使用说明
GPlayPayParams 是支付接口需要传入的支付参数类型, 需要提前设置好必填参数, 否则会报错.

```C#
static void payResultCallback(GPlay.EActionResultCode resultCode, string msg)
{
    switch (resultCode)
    {
        case GPlay.EActionResultCode.PAY_RESULT_SUCCESS:
        {//支付成功
            break;
        }
        case GPlay.EActionResultCode.PAY_RESULT_FAIL:
        {//支付失败
            break;
        }
        default:
            break;
    }
}
//...
GPlay.GPlayPayParams params;
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
GPlay.GPlaySDK.Pay(params, payResultCallback);
```  

---
#### GPlay.GPlaySDK.CreateShortcut( callback ) 创建桌面快捷图标
##### 描述
调用 GPlaySDK 的创建桌面快捷图标功能, 成功以后调用回调方法, 通知游戏是否创建成功.

##### 参数
| 参数 | 类型| 名称 | 参数说明 |
| :-- | :-: | :-: | :-- |
| callback | GPlay.ActionCallback | 创建图标结果回调 | 创建图标功能处理完毕后调用通知游戏创建结果 |

##### 结果
调用传入的 callback 方法, 会传入两个参数

| 参数 | 类型 | 名称 | 参数说明 |
| :---- | :-: | :-: | :-- |
| resultCode | GPlay.EActionResultCode | 创建结果代码 | 用于标示此次创建的结果 |
| msg | string | 创建结果消息 | 相对于创建结果的一条消息 |

使用方法请参考 Login 接口的例子

创建桌面快捷图标接口的结果有如下几种:

| 枚举值 | 说明 |
| :---- | :--: |
| GPlay.EActionResultCode.SHORTCUT\_RESULT_SUCCESS | 创建成功 |
| GPlay.EActionResultCode.SHORTCUT\_RESULT_FAILED | 创建失败 |

---

#### GPlay.GPlaySDK.IsFunctionSupported( funcName ) 判断是否支持某个接口
##### 描述
除了 Login 等通用接口外，通过此函数可以来判断是否还支持其他接口. 

##### 参数
| 参数 | 类型 | 名称 | 参数说明 |
| :-- | :-: | :-: | :-- |
| funcName | string | 方法名称 | 希望调用到的方法名 |

##### 结果
返回一个 bool 类型结果, true 表示支持, false 表示不支持

---

#### GPlay.GPlaySDK.CallSyncFunc( funcName, params ) 渠道同步扩展接口
##### 描述
除 GPlay 标准 API 之外, 渠道有其他扩展功能的支持时, 可通用此扩展接口来调用渠道的扩展功能（同步调用）.

##### 参数
| 参数 | 类型 | 名称 | 参数说明 |
| :-- | :-: | :-: | :-- |
| funcName | string | 方法名称 | 希望调用到的方法名 |
| params  | string | 扩展接口参数 | jsonString, 保存着参数信息 |

##### 结果
返回一个 JSON 格式的 string 字符串, 游戏开发者根据文档从中解析出方法执行的结果

---
#### GPlay.GPlaySDK.CallAsyncFunc( funcName, params, callback ) 渠道异步扩展接口
##### 描述
除 GPlay 标准 API 之外, 渠道有其他扩展功能的支持时, 可通用此扩展接口来调用渠道的扩展功能（异步调用，通过回调函数传递执行结果）.

##### 参数
| 参数 | 类型 | 名称 | 参数说明 |
| :-- | :-: | :-: | :-- |
| funcName | string | 方法名称 | 希望调用到的方法名 |
| params  | string | 扩展接口参数 | 保存着参数信息 |
| callback | GPlay.ActionCallback | 扩展接口回调 | 异步接口调用, 通知游戏执行结果 |

##### 结果
调用传入的 callbackFunc 方法, 会传入两个参数

| 参数 | 类型 | 名称 | 参数说明 |
| :---- | :-: | :-: | :-- |
| resultCode | GPlay.EActionResultCode |  执行代码 | 用于标示此次执行的结果 |
| msg | string | 消息 | 相对于执行状态的一条消息 |

游戏开发者根据约定的 code 和 msg 获得方法执行的状态, 执行相应的回调代码.

---
#### GPlay.GPlaySDK.PreloadGroup(group, preloadSuccessCallback) 加载单个资源包
##### 描述
游戏运行模块前调用, 指定当前模块需要用到的资源包, GPlay SDK 会检测判断是否需要下载缺失的资源:

- 本地已存在指定资源包并且和线上最新版一致, 不需要再次下载：preloadSuccessCallback 会立即被调用;
- 指定的资源包本地不存在、不完整或者线上有更新, 需要下载：GPlay SDK 开始下载指定的资源包, 下载完成后通过 preloadSuccessCallback 通知游戏。

##### 参数
| 参数 | 类型 | 名称 | 参数说明 |
| :---- | :-: | :-: | :-- |
| group | string | 资源包名称 | 指定需要确认加载的资源包名称 |
| preloadSuccessCallback | GPlay.PreloadSuccessCallback | 回调函数 | 成功加载资源包后调用的回调函数 |

##### 结果
| 加载情况 | 效果 |  
| :--: | :-- |  
| 加载成功 | 执行传入的回调函数 |  
| 空间不足 | 弹框提示“空间不足”错误信息, 提示玩家选择**重试**或者**退出** |  
| 校验失败 | 弹框提示“校验失败”错误信息, 提示玩家选择**重试**或者**退出** |   
| 网络异常 | 弹框提示“网络异常”错误信息, 提示玩家选择**重试**或者**退出** | 

##### 说明
接入过程中, 只需要把进入模块时的原有逻辑, 或逻辑入口放在 preloadSuccessCallback 中即可, 如将

```C#
//...
UnityEngine.SceneManagement.SceneManager.LoadScene("sceneName");
```
改为

```C#
private void OnPreloadGroupSuccess()
{
    UnityEngine.SceneManagement.SceneManager.LoadScene("sceneName");
}

GPlaySDK.PreloadGroup("group1", OnPreloadGroupSuccess);
```

---
#### GPlay.GPlaySDK.PreloadGroup(group, preloadResponseCallback) 加载单个资源包
##### 描述
CP可用于实现定制资源包加载的进度界面等。如果CP没有定制需求则不需要调用这个接口。

游戏运行模块前调用, 指定当前模块需要用到的资源包, GPlay SDK 会检测判断是否需要下载缺失的资源:

- 本地已存在指定资源包并且和线上最新版一致, 不需要再次下载：preloadResponseCallback 会立即被调用;
- 指定的资源包本地不存在、不完整或者线上有更新, 需要下载：GPlay SDK 开始下载指定的资源包, 下载过程会通过 preloadResponseCallback 通知游戏。

##### 参数
| 参数 | 类型 | 名称 | 参数说明 |
| :---- | :-: | :-: | :-- |
| group | string | 资源包名称 | 指定需要确认加载的资源包名称 |
| preloadResponseCallback | GPlay.PreloadResponseCallback | 回调函数 | 加载资源包加载详情的回调函数 |

##### 回调函数参数说明：
调用传入的 preloadResponseCallback 方法, 会传入一个参数

| 参数 | 类型 | 名称 | 参数说明 |
| :---- | :-: | :-: | :-- |
| responseInfo | GPlay.PreloadResponseInfo | 资源加载信息 | 用于传递资源加载过程的相关信息 |

##### 结果
| 加载情况 | 效果 |
| :--: | :-- |
| 加载成功 | 执行传入的回调函数 |
| 空间不足 | 弹框提示“空间不足”错误信息, 提示玩家选择**重试**或者**退出** |
| 校验失败 | 弹框提示“校验失败”错误信息, 提示玩家选择**重试**或者**退出** |
| 网络异常 | 弹框提示“网络异常”错误信息, 提示玩家选择**重试**或者**退出** |

##### 说明
接入过程中, 只需要把进入模块时的原有逻辑, 或逻辑入口放在 preloadResponseCallback 中即可, 如将

```C#
//...
UnityEngine.SceneManagement.SceneManager.LoadScene("sceneName");
```
改为

```C#
private void OnPreloadGroupResponse(GPlay.PreloadResponseInfo info)
{
    switch(info.resultCode)
    {
        case EActionResultCode.PRELOAD_RESULT_SUCCESS:
            UnityEngine.SceneManagement.SceneManager.LoadScene("sceneName");
            break;

        case EActionResultCode.PRELOAD_RESULT_PROGRESS:
            string infoStr = string.Format("Preload Response :\ndownloadSpeed: {0}    errorCode: {1}   groupName: {2}    percent: {3}   resultCode: {4}", info.downloadSpeed, info.errorCode, info.groupName, info.percent, info.resultCode);

            Debug.Log(infoStr);
                break;
        }
}

GPlaySDK.PreloadGroup("group1-0", OnPreloadGroupResponse);
```

---
#### GPlay.GPlaySDK.PreloadGroup(groups, preloadSuccessCallback) 加载多个资源包
##### 描述
游戏运行模块前调用, 指定当前模块需要用到的资源包, GPlay SDK 会检测判断是否需要下载缺失的资源:

- 本地已存在指定资源包并且和线上最新版一致, 不需要再次下载：preloadSuccessCallback 会立即被调用;
- 指定的资源包本地不存在、不完整或者线上有更新, 需要下载：GPlay SDK 开始下载指定的资源包, 下载完成后通过 preloadSuccessCallback 通知游戏。

##### 参数
| 参数 | 类型 | 名称 | 参数说明 |
| :---- | :-: | :-: | :-- |
| groups | string | 资源包名称 | 指定需要确认加载的资源包名称 |
| preloadSuccessCallback | GPlay.PreloadSuccessCallback | 回调函数 | 成功加载资源包后调用的回调函数 |

##### 结果
| 加载情况 | 效果 |  
| :--: | :-- |  
| 加载成功 | 执行传入的回调函数 |  
| 空间不足 | 弹框提示“空间不足”错误信息, 提示玩家选择**重试**或者**退出** |  
| 校验失败 | 弹框提示“校验失败”错误信息, 提示玩家选择**重试**或者**退出** |   
| 网络异常 | 弹框提示“网络异常”错误信息, 提示玩家选择**重试**或者**退出** | 

##### 说明
接入过程中, 只需要把进入模块时的原有逻辑, 或逻辑入口放在 PreloadSuccessCallback 中即可, 如将

```C#
//...
UnityEngine.SceneManagement.SceneManager.LoadScene("sceneName");
```
改为

```C#
private void OnPreloadGroupSuccess()
{
    UnityEngine.SceneManagement.SceneManager.LoadScene("sceneName");
}

GPlaySDK.PreloadGroup(new string[]{"group1-0", "group1-1"}, OnPreloadGroupSuccess);
```

---
#### GPlay.GPlaySDK.PreloadGroup(groups, preloadResponseCallback) 加载多个资源包
##### 描述
CP可用于实现定制资源包加载的进度界面等。如果CP没有定制需求则不需要调用这个接口。

游戏运行模块前调用, 指定当前模块需要用到的资源包, GPlay SDK 会检测判断是否需要下载缺失的资源:

- 本地已存在指定资源包并且和线上最新版一致, 不需要再次下载：preloadResponseCallback 会立即被调用;
- 指定的资源包本地不存在、不完整或者线上有更新, 需要下载：GPlay SDK 开始下载指定的资源包, 下载过程会通过 preloadResponseCallback 通知游戏。

##### 参数
| 参数 | 类型 | 名称 | 参数说明 |
| :---- | :-: | :-: | :-- |
| groups | string | 资源包名称 | 指定需要确认加载的资源包名称 |
| preloadResponseCallback | GPlay.PreloadResponseCallback | 回调函数 | 加载资源包加载详情的回调函数 |

##### 回调函数参数说明：
调用传入的 preloadResponseCallback 方法, 会传入一个参数

| 参数 | 类型 | 名称 | 参数说明 |
| :---- | :-: | :-: | :-- |
| responseInfo | GPlay.PreloadResponseInfo | 资源加载信息 | 用于传递资源加载过程的相关信息 |

##### 结果
| 加载情况 | 效果 |
| :--: | :-- |
| 加载成功 | 执行传入的回调函数 |
| 空间不足 | 弹框提示“空间不足”错误信息, 提示玩家选择**重试**或者**退出** |
| 校验失败 | 弹框提示“校验失败”错误信息, 提示玩家选择**重试**或者**退出** |
| 网络异常 | 弹框提示“网络异常”错误信息, 提示玩家选择**重试**或者**退出** |

##### 说明
接入过程中, 只需要把进入模块时的原有逻辑, 或逻辑入口放在 preloadResponseCallback 中即可, 如将

```C#
//...
UnityEngine.SceneManagement.SceneManager.LoadScene("sceneName");
```
改为

```C#
private void OnPreloadGroupResponse(GPlay.PreloadResponseInfo info)
{
    switch(info.resultCode)
    {
        case EActionResultCode.PRELOAD_RESULT_SUCCESS:
            UnityEngine.SceneManagement.SceneManager.LoadScene("sceneName");
            break;

        case EActionResultCode.PRELOAD_RESULT_PROGRESS:
            string infoStr = string.Format("Preload Response :\ndownloadSpeed: {0}    errorCode: {1}   groupName: {2}    percent: {3}   resultCode: {4}", info.downloadSpeed, info.errorCode, info.groupName, info.percent, info.resultCode);

            Debug.Log(infoStr);
                break;
        }
}

GPlaySDK.PreloadGroup(new string[]{"group1-0", "group1-1"}, OnPreloadGroupResponse);
```

---
#### **GPlay.PreloadResponseCallback说明**
public delegate void PreloadResponseCallback(PreloadResponseInfo info);

---
#### **GPlay.PreloadResponseInfo说明**
| 成员变量 | 类型 | 参数说明 |
| :---- | :-: | :-: | :-- |
| resultCode | GPlay.EActionResultCode | 结果码 |
| errorCode | GPlay.EActionResultCode | 错误码 |
| groupName | string | 当前正在下载的资源包  |
| percent | float | 当前下载进度 |
| downloadSpeed | float | 当前下载速度 |

resultCode:

- GPlay.EActionResultCode.PRELOAD\_RESULT_SUCCESS：下载成功
- GPlay.EActionResultCode.PRELOAD\_RESULT_PROGRESS：下载进行中
- GPlay.EActionResultCode.PRELOAD\_RESULT_FAILED：下载失败

errorCode:

- GPlay.EActionResultCode.PRELOAD\_ERROR_NETWORK：网络错误
- GPlay.EActionResultCode.PRELOAD\_ERROR\_VERIFY_FAILED：校验错误
- GPlay.EActionResultCode.PRELOAD\_ERROR\_NO_SPACE：存储空间不足
- GPlay.EActionResultCode.PRELOAD\_ERROR_UNKNOWN：未知错误
- GPlay.EActionResultCode.PRELOAD\_ERROR_NONE：没有错误
