-- Gplay.lua 是 GplaySDK 的 LUA 接入层代码, 提供了一套通用的预加载, 登陆, 支付接口供 CP 调用

print("loading Gplay.lua");
-- ======= gplay const ========
if cc and cc.exports then
    cc.exports.GplayNetworkPreloadStatus = {
        DOWNLOAD = 1,
        UNZIP = 2
    }
else
    GplayNetworkPreloadStatus = {
        DOWNLOAD = 1,
        UNZIP = 2
    }
end

if cc and cc.exports then
    cc.exports.printTable = function(t1)
        for k, v in pairs(t1) do
            print(k .. ":")
            print(v)
            print(" ")
        end
    end
else
    printTable = function(t1)
        for k, v in pairs(t1) do
            print(k .. ":")
            print(v)
            print(" ")
        end
    end
end

-- ========================= gplay class ===============
if not class then
    function class(classname, super)
       local superType = type(super)
       local cls

       if superType ~= "function" and superType ~= "table" then
           superType = nil
           super = nil
       end

       if superType == "function" or (super and super.__ctype == 1) then
           -- inherited from native C++ Object
           cls = {}

           if superType == "table" then
               -- copy fields from super
               for k,v in pairs(super) do cls[k] = v end
               cls.__create = super.__create
               cls.super    = super
           else
               cls.__create = super
               cls.ctor = function() end
           end

           cls.__cname = classname
           cls.__ctype = 1

           function cls.new(...)
               local instance = cls.__create(...)
               -- copy fields from class to native object
               for k,v in pairs(cls) do instance[k] = v end
               instance.class = cls
               instance:ctor(...)
               return instance
           end

       else
           -- inherited from Lua Object
           if super then
               cls = {}
               setmetatable(cls, {__index = super})
               cls.super = super
           else
               cls = {ctor = function() end}
           end

           cls.__cname = classname
           cls.__ctype = 2 -- lua
           cls.__index = cls

           function cls.new(...)
               local instance = setmetatable({}, cls)
               instance.class = cls
               instance:ctor(...)
               return instance
           end
       end

       return cls
    end
end
-- ======================= GplayPayParams =============
if cc and cc.exports then
    cc.exports.GplayPayParams = class("GplayPayParams")
else
    GplayPayParams = class("GplayPayParams")
end

function GplayPayParams:ctor( product_id, product_name, product_price, product_count, product_desc, game_user_id, game_user_name, server_id, server_name, extra)
    print("===> GplayPayParams:ctor()")
    self.params = {};

    self:setProductID(product_id);              -- 商品ID
    self:setProductName(product_name);          -- 商品名称
    self:setProductPrice(product_price);        -- 商品单价
    self:setProductCount(product_count);        -- 商品数量
    self:setProductDescription(product_desc);   -- 商品描述
    self:setGameUserID(game_user_id);           -- 玩家游戏 ID
    self:setGameUserName(game_user_name);       -- 玩家游戏昵称
    self:setServerID(server_id);                -- 玩家所在服务器 ID
    self:setServerName(server_name);            -- 玩家所在服务器名称
    self:setExtraData(extra);                   -- 扩展数据 可用于透传信息诸如 游戏玩家昵称, 服务器名称
end

function GplayPayParams:getParamData()

    if (self.params["product_id"] ~= nil and
        self.params["product_name"] ~= nil and
        self.params["product_price"] ~= nil and
        self.params["product_count"] ~= nil and
        self.params["product_desc"] ~= nil and
        self.params["game_user_id"] ~= nil and
        self.params["game_user_name"] ~= nil and
        self.params["server_id"] ~= nil and
        self.params["server_name"] ~= nil) and
        type(self.params["product_id"]) == "string" and
        type(self.params["product_name"]) == "string" and
        type(self.params["product_price"]) == "number" and
        type(self.params["product_count"]) == "number" and
        type(self.params["product_desc"]) == "string" and
        type(self.params["game_user_id"]) == "string" and
        type(self.params["game_user_name"]) == "string" and
        type(self.params["server_id"]) == "string" and
        type(self.params["server_name"]) == "string" then
        return self.params;
    else
        print("===> GplayPayParams parameter error, please check again");
        return nil;
    end
end

function GplayPayParams:setProductID(product_id)
    self.params["product_id"] = product_id;
end

function GplayPayParams:setProductName(product_name)
    self.params["product_name"] = product_name;
end

function GplayPayParams:setProductPrice(product_price)
    self.params["product_price"] = product_price;
end

function GplayPayParams:setProductCount(product_count)
    self.params["product_count"] = product_count;
end

function GplayPayParams:setProductDescription(product_desc)
    self.params["product_desc"] = product_desc;
end

function GplayPayParams:setGameUserID(game_user_id)
    self.params["game_user_id"] = game_user_id;
end

function GplayPayParams:setGameUserName(game_user_name)
    self.params["game_user_name"] = game_user_name;
end

function GplayPayParams:setServerID(server_id)
    self.params["server_id"] = server_id;
end

function GplayPayParams:setServerName(server_name)
    self.params["server_name"] = server_name;
end

function GplayPayParams:setExtraData(extra_data)
    self.params["ext"] = extra_data;
end


-- ======================= GplayShareParams =============

if cc and cc.exports then
    cc.exports.GplayShareParams = class("GplayShareParams")
else
    GplayShareParams = class("GplayShareParams")
end

function GplayShareParams:ctor(page_url, title, text, img_url, img_title)
    print("===> GplayShareParams:ctor()")
    self.params = {} 

    self:setPageUrl(page_url);      -- 分享后点击的页面 URL 
    self:setTitle(title);           -- 分享界面标题 
    self:setText(text);             -- 分享内容 
    self:setImgUrl(img_url);        -- 图片 URL 
    self:setImgTitle(img_title);    -- 图片标题
end

function GplayShareParams:getParamData()
    if (self.params["url"] ~= nil and
        self.params["title"] ~= nil and
        self.params["text"] ~= nil and
        self.params["img_url"] ~= nil and
        self.params["img_title"] ~= nil) then
        return self.params;
    else
        return nil;
    end
end

function GplayShareParams:setPageUrl( page_url )
    self.params["url"] = page_url;
end

function GplayShareParams:setTitle( title )
    self.params["title"] = title;
end

function GplayShareParams:setText( text )
    self.params["text"] = text;
end

function GplayShareParams:setImgUrl( img_url )
    self.params["img_url"] = img_url;
end

function GplayShareParams:setImgTitle( img_title )
    self.params["img_title"] = img_title;
end


-- ======================= GplayImplement ======================
local GplayImplement = {}

GplayImplement.ActionResultCode = {
    GPLAY_INIT_SUCCESS = 0,                 -- succeeding in initing sdk
    GPLAY_INIT_FAIL = 1,                    -- failing to init sdk

    USER_LOGIN_RESULT_SUCCESS = 10000,      -- login success
    USER_LOGIN_RESULT_FAIL = 10001,         -- login failed
    USER_LOGIN_RESULT_CANCEL = 10002,       -- login canceled
    USER_LOGOUT_RESULT_SUCCESS = 10003,     -- logout success
    USER_LOGOUT_RESULT_FAIL = 10004,        -- logout failed
    USER_REGISTER_RESULT_SUCCESS = 10005,   -- regiister sucess
    USER_REGISTER_RESULT_FAIL = 10006,      -- regiister failed
    USER_REGISTER_RESULT_CANCEL = 10007,    -- regiister Cancel
    USER_BIND_RESULT_SUCESS = 10008,        -- bind sucess
    USER_BIND_RESULT_CANCEL = 10009,        -- bind Cancel
    USER_BIND_RESULT_FAILED = 100010,       -- bind failed
    USER_RESULT_NETWROK_ERROR = 10011,      -- network error
    USER_RESULT_USEREXTENSION = 19999,      -- extension code

    PAY_RESULT_SUCCESS = 20000,             -- pay success
    PAY_RESULT_FAIL = 20001,                -- pay fail
    PAY_RESULT_CANCEL = 20002,              -- pay cancel
    PAY_RESULT_INVALID = 20003,             -- incompleting info
    PAY_RESULT_NETWORK_ERROR = 20004,       -- network error
    PAY_RESULT_NOW_PAYING = 20005,          -- paying now
    PAY_RESULT_PAYEXTENSION = 29999,        -- extension code
    
    SHARE_RESULT_SUCCESS = 30000,           -- share success
    SHARE_RESULT_FAIL = 30001,              -- share failed
    SHARE_RESULT_CANCEL = 30002,            -- share canceled
    SHARE_RESULT_NETWORK_ERROR = 30003,     -- network error
    SHARE_RESULT_SHAREREXTENSION = 39999,   -- extension code
    
    SHORTCUT_RESULT_SUCCESS = 40000,
    SHORTCUT_RESULT_FAILED = 40001,

    CAPTURE_SCREEN_SUCCESS = 41000,
    CAPTURE_SCREEN_FAILED = 41001,

    PRELOAD_RESULT_SUCCESS = 50000,
    PRELOAD_RESULT_PROGRESS = 50001,
    PRELOAD_RESULT_FAILED = 50002,

    PRELOAD_ERROR_NETWORK = 60000,
    PRELOAD_ERROR_VERIFY_FAILED = 60001,
    PRELOAD_ERROR_NO_SPACE = 60002,
    PRELOAD_ERROR_UNKNOWN = 60003,
    PRELOAD_ERROR_NONE = 60004
}

GplayImplement.NetworkType = {
    NO_NETWORKS = -1,
    MOBILE = 0,
    WIFI = 1
}

local callbackDelegateMap = nil
local s_callbackID = 1

-- 构造函数
function GplayImplement:ctor()
    print("===> GplayImplement:ctor()")

    callbackDelegateMap = {}

    luab_gplay:setAsyncActionResultListener(self.onAsyncFuncResult)
end

-- ============ runtime ===========

-- 初始化 GplaySDK
function GplayImplement:initSDK(appKey, appSecret, privateKey)
    print("===> GplayImplement:initSDK()")
    luab_gplay:init(appKey, appSecret, privateKey)
end

function GplayImplement:getChannelID()
    return luab_gplay:getChannelId()
end

-- 检查是否处于登录状态
function GplayImplement:isLogined()
    local bIsLogined = luab_gplay:isLogined()
    print("===> GplayImplement:isLogined() ? " .. tostring(bIsLogined))
    return bIsLogined
end

-- 登录
function GplayImplement:login(callback)
    print("===> GplayImplement:login()")

    local callbackId = self:addCallbackToMapAndGetCallbackId(callback);
    luab_gplay:login(callbackId)
end

-- 支付
function GplayImplement:pay(gplayPayParams, callback)
    print("===> GplayImplement:pay()")
    local params = gplayPayParams:getParamData()
    params = json.encode(params)
    print("send info is " .. params)

    local callbackId = self:addCallbackToMapAndGetCallbackId(callback);
    luab_gplay:payForProduct(callbackId, params)
end

-- 发送至桌面
function GplayImplement:createShortcut(callback)
    print("===> GplayImplement:createShortcut()")

    local callbackId = self:addCallbackToMapAndGetCallbackId(callback);
    luab_gplay:createShortcut(callbackId)
end

-- 获取用户id
function GplayImplement:getUserID()
    print("===> GplayImplement:getUserID()")
    return luab_gplay:getUserID()
end

-- 分享
function GplayImplement:share(gplayShareParams, callback)
    print("===> GplayImplement:share()")
    local params = gplayShareParams:getParamData()
    params = json.encode(params)
    print("send info is " .. params)

    local callbackId = self:addCallbackToMapAndGetCallbackId(callback);
    luab_gplay:share(callbackId, params)
end

-- 异步回调
function GplayImplement:onAsyncFuncResult(ret, msg, callbackId)
    print("===> GplayImplement:onAsyncFuncResult()")

    local callbackDelegate = callbackDelegateMap[callbackId];

    if callbackDelegate then
        callbackDelegate(ret, msg)
        callbackDelegate = nil
    end
    callbackDelegateMap[callbackId] = nil
end

-- 保存回调, 返回 id
function GplayImplement:addCallbackToMapAndGetCallbackId(callback)
    -- new a random alphaNum string to be callbackId
    local callbackId = s_callbackID
    s_callbackID = s_callbackID + 1
    callbackDelegateMap[callbackId] = callback

    return callbackId
end

-- 是否支持该函数
function GplayImplement:isSupportingFunc(funcName)
    return luab_gplay:isSupportingFunc(funcName)
end

-- 调用渠道异步扩展接口
function GplayImplement:callAsyncFunc(funcName, params, callbackFunc)
    print("===> GplayImplement:callAsyncFunc() " .. funcName .. " with param: " .. json.encode(params))

    local callbackId = self:addCallbackToMapAndGetCallbackId(callbackFunc);

    if params == nil then
        luab_gplay:callAsyncFunc(callbackId, funcName, "")
    else 
        luab_gplay:callAsyncFunc(callbackId, funcName, params)
    end
end

-- 调用渠道同步扩展接口
function GplayImplement:callSyncFunc(funcName, params)
    return luab_gplay:callSyncStringFunc(funcName, params)
end

function GplayImplement:setPreloadResponseCallback(callback)
    if(callback == nil) then
        gplay._preloadResponseCallback = nil
        luab_gplay:setPreloadResponseCallback()
    else
        gplay._preloadResponseCallback = callback
        luab_gplay:setPreloadResponseCallback(gplay._onPreloadResponse)
    end
end

function GplayImplement:_onPreloadResponse(resultCode, errorCode, groupName, percent, speed)
    gplay._preloadResponseCallback(resultCode, errorCode, groupName, percent, speed);
end

function GplayImplement:onPreloadSuccess(jsonResult)
    gplay._preloadCallback(jsonResult)
    gplay._preloadCallback = nil
end

function GplayImplement:backFromResourceBundle( bundleName)
    print("===> GplayImplement:backFromResourceBundle " .. bundleName)
    if luab_gplay:isDivideResourceMode() then
        if type(bundleName) == "string" then
            luab_gplay:noticePreloadEnd(bundleName)
        else
            cc.assert(false, "wrong parameter of backFromResourceBundle #1, only String supported");
        end
    end
end

-- 加载一个或多个资源包
function GplayImplement:preloadResourceBundles(bundleNames, successCallbackFunc)
    print("===> GplayImplement:preloadResourceBundles()")

    if luab_gplay:isDivideResourceMode() then
        if type(bundleNames) == "string" then
            luab_gplay:noticePreloadBegin(bundleNames)
        elseif type(bundleNames) == "table" then
            if table.getn(bundleNames) > 1 then
                luab_gplay:noticePreloadMultipleGroup()
            else
                luab_gplay:noticePreloadBegin(bundleNames[1])
            end
        else
            cc.assert(false, "wrong parameter of preloadResourceBundles #1, only String or Array supported");
        end
        successCallbackFunc("")
        return nil
    end

    local objBundleNames = {}

    if type(bundleNames) == "string" then
        objBundleNames["scenes"] = {bundleNames}
    elseif type(bundleNames) == "table" then
        objBundleNames["scenes"] = bundleNames
    else
        cc.assert(false, "wrong parameter of preloadResourceBundles #1, only String or Array supported");
    end

    local jsonBundleNames = json.encode(objBundleNames)

    gplay._preloadCallback = successCallbackFunc

    luab_gplay:preloadGroups(jsonBundleNames, gplay.onPreloadSuccess)
end

-- 获取当前网络状态
function GplayImplement:getNetworkType()
    print("===> GplayImplement:getNetworkType()")
    return luab_gplay:getNetworkType()
end

-- 调用 GplaySDK quitGame 通知结束游戏
function GplayImplement:quitGame()
    print("===> GplayImplement:quitGame()")
    luab_gplay:quitGame();
end

-- 真正结束游戏
if cc and cc.exports then
    cc.exports.DirectorEndToLua = function ()
        print("===> DirectorEndToLua()")
        cc.Director:getInstance():endToLua()
    end
else
    DirectorEndToLua = function ()
        print("===> DirectorEndToLua()")
        cc.Director:getInstance():endToLua()
    end
end

-- ======================== instance ======================

if type(luab_gplay) == "table" then
    local isInGPlayEnv = luab_gplay:isInGplayEnv()
    
    if isInGPlayEnv then
        GplayImplement:ctor()

        if cc and cc.exports then
            cc.exports.gplay = GplayImplement
        else
            gplay = GplayImplement
        end

        gplay.isInGplayEnv = function()
            return true
        end

        gplay.preloadGroup = gplay.preloadResourceBundles
        gplay.preloadGroups = gplay.preloadResourceBundles
        gplay.preloadResourceBundle = gplay.preloadResourceBundles
        gplay.backFromGroup = gplay.backFromResourceBundle
    else
        print("fatal Error: GplaySDK not registed in LUAJIT environment!"); 
        if cc and cc.exports then
            cc.exports.gplay = {}
        else
            gplay = {}
        end

        gplay.isInGplayEnv = function()
            return false
        end
        
        gplay.preloadResourceBundles = function(self, bundleNames, successCallback)
           if (successCallback) then
               successCallback(); 
           end
        end

        gplay.preloadGroup = gplay.preloadResourceBundles
        gplay.preloadGroups = gplay.preloadResourceBundles
        gplay.preloadResourceBundle = gplay.preloadResourceBundles
    
        gplay.backFromResourceBundle = function() end
        gplay.backFromGroup = gplay.backFromResourceBundle
    end
else
    print("fatal Error: GplaySDK not registed in LUAJIT environment!!"); 

    if cc and cc.exports then
        cc.exports.gplay = {}
    else
        gplay = {}
    end

    gplay.isInGplayEnv = function()
        return false
    end

    gplay.preloadResourceBundles = function(self, bundleNames, successCallback)
        if (successCallback) then
            successCallback(); 
        end
    end

    gplay.preloadGroup = gplay.preloadResourceBundles
    gplay.preloadGroups = gplay.preloadResourceBundles
    gplay.preloadResourceBundle = gplay.preloadResourceBundles
 
    gplay.backFromResourceBundle = function() end
    gplay.backFromGroup = gplay.backFromResourceBundle
end


