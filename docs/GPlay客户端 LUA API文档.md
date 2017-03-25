# GplaySDK LUA API文档

### API 列表
---
| API 名称 | API 说明 | 
| :------ | :-----: |
| gplay:isInGplayEnv()| 当前游戏是否处于 Gplay 环境 |
| gplay:initSDK() | 初始化SDK |
| gplay:getChannelId() | 游戏当前所在的渠道ID |
| gplay:getNetworkType() | 获取网络类型 |
| gplay:preloadResourceBundle() | 加载单个资源包 |
| gplay:preloadResourceBundles() | 加载多个资源包 |
| gplay:backFromResourceBundle() | 自动划分资源模式下停止分配资源到指定资源包 |
| gplay:setPreloadResponseCallback() | 设置加载资源包的响应回调|
| gplay:isLogined() |检查登录状态|
| gplay:getUserID() |获取用户 ID | 
| gplay:login() | 登录 |
| gplay:quitGame() | 退出游戏 | 
| gplay:share() | 分享游戏 | 
| gplay:pay() | 支付 |
| gplay:createShortcut() |创建桌面快捷图标 |
| gplay:callSyncFunc() | 调用同步扩展接口 |
| gplay:callAsyncFunc() | 调用异步扩展接口 |

### API 说明
---
#### gplay.isInGplayEnv() 当前游戏是否处于 Gplay 环境
###### 描述
判断当前游戏是否处于 Gplay 环境

###### 参数
无

###### 结果
| 返回 Bool 值 | 说明 |
| :----  | :-- |
| **true** | 游戏当前处于 **Gplay 环境**下 |
| **false** | 游戏当前处于 **非 Gplay 环境**下 |

---

#### gplay:initSDK(appKey, appSecret, privateKey) 获取参数信息
###### 描述
初始化 GplaySDK, 需要在**<font color="red">使用登录支付等API调用前</font>**调用

###### 参数
initSDK 所使用到的三个参数, 是开发者在 Gplay 开放平台上配置游戏的时候获得的

| 参数 | 说明 |
| :----  | :-- |
| appKey | 游戏的 Gplay 唯一标识 |
| appSecret | 游戏的 Gplay 密钥 |
| privateKey | 游戏的 Gplay 签名盐值 |

###### 结果
无, 将参数初始化到游戏运行时配置中, 用于敏感 API 的加密.

---

#### gplay:getNetworkType() 获取网络类型
###### 描述
获取当前设备所处的网络类型

###### 参数
无

###### 结果
返回网络状态, 可以使用 SDK 提供的常量判断, 有如下 3 种:

| 返回常量 | 说明 |
| :----  | :-- |
| gplay.NetworkType.**NO_NETWORK** | 设备当前**无网络连接** |
| gplay.NetworkType.**MOBILE** | 设备当前处于**非 WIFI 环境**下 |
| gplay.NetworkType.**WIFI** | 设备当前处于**WIFI 环境**下 |

---

#### gplay:preloadResourceBundle( group, successCallback) 加载资源包
###### 描述
游戏运行模块前调用, 指定当前模块需要用到的资源包, GPlay SDK 会检测判断是否需要下载缺失的资源:

- 指定的资源包完整, 不需要下载：callback 会立即被调用通知游戏加载完成, 执行回调中的代码
- 指定的资源包本地不存在或者线上有更新, 需要下载：GPlay SDK 开始下载指定的资源包, 下载完成后通过 callback 回到游戏 
- 使用Gplay Tools自动划分资源时，callback会立即被调用通用游戏加载完成，从这一时间点起访问的相关资源会被分配到这个资源包，直到加载新的资源包。
- <font color=red>在 preloadResourceBundle 回调之前, 请勿再次调用此接口
- 对应模块退出时应该调用backFromGroup(sceneName)，以支持在GPlay Tools上使用自动划分资源功能</font>

###### 参数
| 参数 | 名称 | 参数说明 |
| :---- | :-: | :-- |
| group | 资源包名称 | 指定需要确认加载的资源包名称 |
| successCallback | 加载成功回调函数 | 加载资源包完成的后续逻辑, 也就是当前模块的游戏逻辑 |

###### 结果
| 加载情况 | 效果 |  
| :--: | :-- |  
| 加载成功 | 执行传入的回调, 继续游戏 |  
| 空间不足 | 弹框提示“空间不足”错误信息, 提示玩家选择**重试**或者**退出** |  
| 校验失败 | 弹框提示“校验失败”错误信息, 提示玩家选择**重试**或者**退出** |   
| 网络异常 | 弹框提示“网络异常”错误信息, 提示玩家选择**重试**或者**退出** | 

###### 说明
接入过程中, 只需要把进入场景时的原有逻辑, 或逻辑入口放在 successCallback 中即可, 如将

```lua
function GameLayer:ctor()
	self:initView()
end
```
改为

```lua
function GameLayer:ctor()
    local this = self
    gplay:preloadResourceBundle("hall_scene", function()
        this:initView()
    end);
end
```

---
#### gplay:preloadResourceBundles(groups, successCallback) 加载多个资源包
###### 描述
使用此方法, 可以在游戏运行某个模块前, 指定下载游戏模块需要的多个资源包。
<font color=red> 使用GPlay Tools自动划分资源功能时，不支持一次加载多个资源包（工具无法明确资源应该划分给哪一个资源包）</font>

###### 参数
| 参数 | 名称 | 参数说明 |
| :---- | :-: | :-: | :-- |
| groups | 资源包列表 | 指定需要加载的资源包列表 |
| successCallback | 回调函数 | 加载资源包完成后调用的回调函数 |

###### 示例
```lua
function GameLayer:ctor()
    local this = self
    gplay:preloadResourceBundles({"hero", "monster", "skill_res"}, function()
        this:initView()
    end);
end
```
---

####
gplay:backFromResourceBundle(group)
###### 描述
GPlay Tools自动划分资源模式下停止分配资源到指定资源包，建议在场景资源相关联的Scene等节点的cleanup调用。

###### 示例
```lua
    function GameScene:cleanup()
        gplay:backFromResourceBundle("gameScene")
    end
```

---

#### gplay.setPreloadResponseCallback(callback) 设置加载资源包的响应回调
###### 描述
设置加载资源包的响应回调，用于CP定制资源包加载的进度界面等。如果CP没有定制需求则不需要调用这个接口。  

######参数说明：
resultCode:

- gplay.ActionResultCode.PRELOAD\_RESULT_SUCCESS：下载成功
- gplay.ActionResultCode.PRELOAD\_RESULT_PROGRESS：下载进行中
- gplay.ActionResultCode.PRELOAD\_RESULT_FAILED：下载失败

errorCode:

- gplay.ActionResultCode.PRELOAD\_ERROR_NETWORK：网络错误
- gplay.ActionResultCode.PRELOAD\_ERROR\_VERIFY_FAILED：校验错误
- gplay.ActionResultCode.PRELOAD\_ERROR\_NO_SPACE：存储空间不足
- gplay.ActionResultCode.PRELOAD\_ERROR_UNKNOWN：未知错误
- gplay.ActionResultCode.PRELOAD\_ERROR_NONE：没有错误

groupName:当前正在下载的资源包  
percent：当前下载进度  
speed：当前下载速度  

###### 示例
```lua
gplay:setPreloadResponseCallback(function(resultCode, errorCode, groupName, percent, speed)  

end);
```

---

#### gplay:isLogined() 检查登录状态
###### 描述
检查游戏在渠道用户系统上的登录状态

###### 参数
无

###### 结果
返回 Boolean 结果

| 返回值 | 说明 |
| :----  | :-- |
| **true** | 正常登录 |
| **false** | 未登录或已断线 |

---

#### gplay:getUserID() 获取用户 ID
###### 描述
获取游戏玩家在渠道用户系统上分配的 ID

###### 参数
无

###### 结果
返回 string 结果, 表示用户 ID

---
#### gplay:login( callback ) 登录
###### 描述
调用 GplaySDK 的登录方法, 呼唤渠道的登录功能, 登录成功以后调用回调方法, 通知游戏登录结果.

###### 参数
| 参数 | 名称 | 参数说明 |
| :---- | :-: | :-- |
| callback | 登录结果回调函数 | 登录模块处理完毕后调用, 通知游戏登录结果 |

###### 结果
调用传入的 callback 方法, 会传入两个参数

| 参数 | 名称 | 参数说明 |
| :---- | :-: | :-- |
| code | 登录结果代码 | 用于标示此次登录的结果 |
| msg | 登录结果消息 | 相对于登录结果的一条消息 |

###### 使用说明
游戏传入的回调函数, 需要对此回调结果做出相应的处理, 将登录逻辑移至回调当中, 例:

```lua
-- 登录回调
function LoginLayer:loginCallback(code, msg)
    if code == gplay.ActionResultCode.USER_LOGIN_RESULT_SUCCESS then
        -- 登录成功
    elseif code == gplay.ActionResultCode.USER_LOGIN_RESULT_FAIL then
        -- 登录失败
    else
        ...
    end
end
...
local this = self
gplay:login(function(code, msg)
    this:loginCallback(code, msg)
end)

```
登录接口的结果有如下几种:

| 常量值 | 说明 |
| :---- | :--: |
| gplay.ActionResultCode.USER\_RESULT\_NETWROK_ERROR | 网络错误 |
| gplay.ActionResultCode.USER\_LOGIN\_RESULT_SUCCESS | 登录成功 |
| gplay.ActionResultCode.USER\_LOGIN\_RESULT_FAIL | 登录失败 |
| gplay.ActionResultCode.USER\_RESULT_USEREXTENSION | 扩展结果 |

---

#### gplay:quitGame() 退出游戏
###### 描述
游戏开发者需要将结束游戏的 `cc.Director:getInstance():endToLua()` 等结束游戏的 API 替换为 `gplay:quitGame()`.

###### 参数
无

###### 结果
关闭游戏, 游戏中的 `onExit()` 等生命周期不需要改动, 在关闭游戏之前都会被调用到.

---

#### gplay:share( gplayShareParams, callback ) 分享游戏
###### 描述
调用 GplaySDK 的分享游戏方法, 呼唤渠道的分享游戏功能, 成功以后调用回调方法, 通知游戏分享结果.

###### 参数
| 参数 | 名称 | 参数说明 |
| :---- | :-: | :-- |
| paragplayShareParamsms | 分享参数对象 | GplayShareParams 对象, 保存着参数键值对 |
| callback | 分享结果回调函数 | 分享功能处理完毕后调用, 通知游戏分享结果 |

###### 结果
调用传入的 callback 方法, 会传入两个参数

| 参数 | 名称 | 参数说明 |
| :---- | :-: | :-- |
| code | 分享结果代码 | 用于标示此次分享的结果 |
| msg | 分享结果消息 | 相对于分享结果的一条消息 |


分享接口的结果有如下几种:

| 常量值 | 说明 |
| :---- | :--: |
| gplay.ActionResultCode.SHARE\_RESULT_SUCCESS | 分享成功 |
| gplay.ActionResultCode.SHARE\_RESULT_FAIL | 分享失败 |
| gplay.ActionResultCode.SHARE\_RESULT_CANCEL | 取消分享 |
| gplay.ActionResultCode.SHARE\_RESULT\_NETWORK_ERROR | 网络错误 |
| gplay.ActionResultCode.SHARE\_RESULT_SHAREREXTENSION | 扩展结果 |

##### 使用说明
GplayPayParams 是支付接口需要传入的支付参数类型, 需要提前设置好必填参数, 否则会报错.

```lua
local params = GplayShareParams.new()

params:setPageUrl("http://192.168.1.3");                                -- 分享页面
params:setTitle("LUASDKDemo");                                          -- 分享标题
params:setText("LUASDKDemo 是 GplaySDK 的线下测试版本");                   -- 分享内容
params:setImgUrl("http://192.168.1.3:8888/moon/icon/large_icon.png");   -- 分享图片
params:setImgTitle("large_icon");                                       -- 分享图片标题

gplay:share(params, function(ret, msg)
    ...
end)
``` 
---

#### gplay:pay( gplayPayParams, callback ) 支付
###### 描述
调用 GplaySDK 的支付方法, 呼唤第三方支付系统的支付功能, 成功以后调用回调方法, 通知游戏支付结果.

###### 参数
| 参数 | 名称 | 参数说明 |
| :---- | :-: | :-- |
| gplayPayParams | 支付参数对象 | GplayPayParams 对象, 保存着参数键值对 |
| callback | 支付结果回调函数 | 支付功能处理完毕后调用, 通知游戏支付结果 |  

###### 结果
调用传入的 callback 方法, 会传入两个参数

| 参数 | 名称 | 参数说明 |
| :---- | :-: | :-- |
| code | 支付结果代码 | 用于标示此次支付的结果 |
| msg | 支付结果消息 | 相对于支付结果的一条消息 |


支付接口的结果有如下几种:

| 常量值 | 说明 |
| :---- | :--: |
| gplay.ActionResultCode.PAY\_RESULT_SUCCESS | 支付成功 |
| gplay.ActionResultCode.PAY\_RESULT_FAIL | 支付失败 |
| gplay.ActionResultCode.PAY\_RESULT_CANCEL | 支付取消 |
| gplay.ActionResultCode.PAY\_RESULT\_NETWORK_ERROR | 网络错误 |
| gplay.ActionResultCode.PAY\_RESULT_INVALID | 支付参数不完整 |
| gplay.ActionResultCode.PAY\_RESULT\_NOW_PAYING | 正在支付 |
| gplay.ActionResultCode.PAY\_RESULT_PAYEXTENSION | 扩展结果 |

##### 使用说明
GplayPayParams 是支付接口需要传入的支付参数类型, 需要提前设置好必填参数, 否则会报错.

```lua
local params = GplayPayParams.new();

params:setProductID("id_gold");      -- 商品 ID
params:setProductName("gold");       -- 商品名称
params:setProductPrice(1);           -- 商品单价 number 类型
params:setProductCount(1);           -- 商品数量 number 类型
params:setProductDescription("game currency, use to purchase in app"); -- 商品描述
params:setGameUserID("lixiaoyaoid"); -- 玩家 ID
params:setGameUserName("lixiaoyao"); -- 玩家昵称
params:setServerID("skybigid");      -- 服务器 ID
params:setServerName("skybig");      -- 服务器名称
params:setExtraData('{"key1":"value1", "key2":"value2"}'); -- 扩展参数, 默认为空

gplay:pay(params, function (ret, msg)
    ...
end)
``` 
注: price 和 count 参数为 **number**, 其余为 string, 请勿传错类型.

---

#### gplay:createShortcut( callback ) 创建桌面快捷图标
###### 描述
调用 GplaySDK 的创建桌面快捷图标功能, 成功以后调用回调方法, 通知游戏是否创建成功.

###### 参数
| 参数 | 名称 | 参数说明 |
| :-- | :-: | :-- |
| callback | 创建图标结果回调函数 | 创建图标功能处理完毕后调用, 通知游戏创建结果 |

###### 结果
调用传入的 callback 方法, 会传入两个参数

| 参数 | 名称 | 参数说明 |
| :-- | :-: | :-- |
| code | 创建结果代码 | 用于标示此次创建的结果 |
| msg | 创建结果消息 | 相对于创建结果的一条消息 |

使用方法请参考 login 接口的例子

创建桌面快捷图标接口的结果有如下几种:

| 常量值 | 说明 |
| :---- | :--: |
| gplay.ActionResultCode.SHORTCUT\_RESULT_SUCCESS | 创建成功 |
| gplay.ActionResultCode.SHORTCUT\_RESULT_FAILED | 创建失败 |

---

#### gplay:callSyncFunc( funcName, params ) 渠道同步扩展接口
###### 描述
除 Gplay 标准 API 之外, 渠道单独提供特殊接口需要实现时, 使用此通用的扩展接口来调用同步方法. 

###### 参数
| 参数 | 名称 | 参数说明 |
| :-- | :-: | :-- |
| funcName | 方法名称 | 希望调用到的方法名 |
| params | 扩展接口参数 | js 对象, 保存着参数键值对 |

###### 结果
返回一个 String 类型结果, 游戏开发者从 string 中解析出方法执行的结果

---
#### gplay:callAsyncFunc( funcName, params, callbackFunc ) 渠道异步扩展接口
###### 描述
除 Gplay 标准 API 之外, 渠道单独提供特殊接口需要实现时, 使用此通用的扩展接口来调用异步方法.

###### 参数
| 参数 | 名称 | 参数说明 |
| :-- | :-: | :-- |
| funcName | 方法名称 | 希望调用到的方法名 |
| params | 扩展接口参数 | js 对象, 保存着参数键值对 |
| callbackFunc | 扩展接口回调函数 | 异步接口调用, 通知游戏执行结果 |

###### 结果
调用传入的 callbackFunc 方法, 会传入两个参数

| 参数 | 名称 | 参数说明 |
| :-- | :-: | :-- |
| code | 执行代码 | 用于标示此次执行的结果 |
| msg | 消息 | 相对于执行状态的一条消息 |

游戏开发者根据约定的 code 和 msg 获得方法执行的状态, 执行相应的回调代码.

使用方法请参考 login 接口的例子
