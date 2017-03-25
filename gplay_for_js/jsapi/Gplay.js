/* Gplay.js 是 GplaySDK 的 JS 接入层代码, 提供了一套通用的预加载, 登陆, 支付接口供 CP 调用*/

cc.log("loading Gplay.js");
// ======= gplay const ========

var GplayNetworkPreloadStatus = {
    DOWNLOAD:1,
    UNZIP: 2
};

// ======================= GplayPayParams =============
var GplayShareParams = cc.Class.extend({
    params : null,

    ctor: function(page_url, title, text, img_url, img_title){
        cc.log("GplayShareParams ctor");
        this.params = {};
        this.params.url = page_url;             // 分享后点击的页面 URL
        this.params.title = title;              // 分享界面标题
        this.params.text = text;                // 分享内容
        this.params.img_url = img_url;          // 图片 URL
        this.params.img_title = img_title;      // 图片标题
    },

    getParamsData: function () {
        if (this.params.url !== null && 
            this.params.title !== null &&
            this.params.text !== null &&
            this.params.img_url !== null &&
            this.params.img_title !== null) 
        {
            return this.params;
        } else {
            cc.log("===> GplayShareParams parameter error, please check again");
            return null;
        }
    }, 

    setPageUrl: function( page_url ) {
        this.params.url = page_url;
    },

    setTitle: function( title ) {
        this.params.title = title;
    },

    setText: function(text) {
        this.params.text = text;
    },

    setImgUrl: function(img_url) {
        this.params.img_url = img_url;
    },

    setImgTitle: function(img_title) {
        this.params.img_title = img_title;
    }
});

var GplayPayParams = cc.Class.extend({
    params : null,

    ctor: function( product_id, product_name, product_price, product_count, product_desc, game_user_id, game_user_name, server_id, server_name, extra){
        cc.log("GplayPayParams ctor");
        this.params = {};
        this.setProductID(product_id);              // 商品ID
        this.setProductName(product_name);          // 商品名称
        this.setProductPrice(product_price);        // 商品单价
        this.setProductCount(product_count);        // 商品数量
        this.setProductDescription(product_desc);   // 商品描述
        this.setGameUserID(game_user_id);           // 玩家游戏 ID
        this.setGameUserName(game_user_name);       // 玩家游戏昵称
        this.setServerID(server_id);                // 玩家所在服务器 ID
        this.setServerName(server_name);            // 玩家所在服务器名称
        this.setExtraData(extra);                   // 扩展数据 可用于透传信息诸如 游戏玩家昵称, 服务器名称
    },

    getParamsData: function () {
        if (typeof(this.params.product_id) === 'string' &&
            typeof(this.params.product_name) === 'string' &&
            typeof(this.params.product_price) === 'number' &&
            typeof(this.params.product_count) === 'number' &&
            typeof(this.params.product_desc) === 'string' &&
            typeof(this.params.game_user_id) === 'string' &&
            typeof(this.params.game_user_name) === 'string' &&
            typeof(this.params.server_id) === 'string' &&
            typeof(this.params.server_name) === 'string')
        {
            return this.params;
        } else {
            cc.log("GplayPayParams parameter error, please check again:" + JSON.stringify(this.params));
            return null;
        }
    },

    setProductID: function ( product_id ) {
        this.params.product_id = product_id;
    },

    setProductName: function ( product_name ) {
        this.params.product_name = product_name;
    },

    setProductPrice: function ( product_price ) {
        this.params.product_price = product_price;
    },

    setProductCount: function ( product_count ) {
        this.params.product_count = product_count;
    },

    setProductDescription: function ( product_desc ) {
        this.params.product_desc = product_desc;
    },

    setGameUserID: function ( game_user_id ) {
        this.params.game_user_id = game_user_id;
    },

    setGameUserName: function ( game_user_name ) {
        this.params.game_user_name = game_user_name;
    },

    setServerID: function (server_id) {
        this.params.server_id = server_id;
    },

    setServerName: function (server_name) {
        this.params.server_name = server_name;
    },

    setExtraData: function (extra_data) {
        this.params.ext = extra_data;
    }
});

var GplayImplement= cc.Class.extend({
    ActionResultCode:{
        GPLAY_INIT_SUCCESS:0,                       // succeeding in initing sdk.
        GPLAY_INIT_FAIL:1,                          // failing to init sdk.

        USER_LOGIN_RESULT_SUCCESS : 10000,          // login success
        USER_LOGIN_RESULT_FAIL : 10001,             // login failed
        USER_LOGIN_RESULT_CANCEL : 10002,           // login canceled
        USER_LOGOUT_RESULT_SUCCESS : 10003,         // logout success
        USER_LOGOUT_RESULT_FAIL : 10004,            // logout failed
        USER_REGISTER_RESULT_SUCCESS : 10005,       // register success
        USER_REGISTER_RESULT_FAIL : 10006,          // register failed
        USER_REGISTER_RESULT_CANCEL : 10007,        // register canceled
        USER_BIND_RESULT_SUCCESS : 10008,           // bind success
        USER_BIND_RESULT_CANCEL : 10009,            // bind canceled
        USER_BIND_RESULT_FAIL : 10010,              // bind failed
        USER_RESULT_NETWROK_ERROR : 10011,          // network error
        USER_RESULT_USEREXTENSION : 19999,          // extension code

        PAY_RESULT_SUCCESS : 20000,                 // paying success
        PAY_RESULT_FAIL : 20001,                    // pay failed
        PAY_RESULT_CANCEL : 20002,                  // pay canceled
        PAY_RESULT_INVALID : 20003,                 // incompleting info
        PAY_RESULT_NETWORK_ERROR : 20004,           // network error
        PAY_RESULT_NOW_PAYING : 20005,              // paying now
        PAY_RESULT_PAYEXTENSION : 29999,            // extension code

        SHARE_RESULT_SUCCESS : 30000,               // sharing success
        SHARE_RESULT_FAIL : 30001,                  // sharing failed
        SHARE_RESULT_CANCEL : 30002,                // sharing canceled
        SHARE_RESULT_NETWORK_ERROR : 30003,         // network error
        SHARE_RESULT_INVALID : 30004,               // incompleting info
        SHARE_RESULT_SHAREREXTENSION : 39999,       // extension code

        SHORTCUT_RESULT_SUCCESS : 40000,
        SHORTCUT_RESULT_FAILED : 40001,

        CAPTURE_SCREEN_SUCCESS : 41000,
        CAPTURE_SCREEN_FAILED : 41001,

        PRELOAD_RESULT_SUCCESS : 50000,
        PRELOAD_RESULT_PROGRESS : 50001,
        PRELOAD_RESULT_FAILED : 50002,

        PRELOAD_ERROR_NETWORK : 60000,
        PRELOAD_ERROR_VERIFY_FAILED : 60001,
        PRELOAD_ERROR_NO_SPACE : 60002,
        PRELOAD_ERROR_UNKNOWN : 60003,
        PRELOAD_ERROR_NONE : 60004
    },
    NetworkType:{
        NO_NETWORK:-1,
        MOBILE: 0,
        WIFI: 1
    },

    _callbackID:0,
    callbackDelegateMap:null,

    ctor: function () {
        cc.log("GplayImplement ctor");

        if (jsb_gplay) {
            jsb_gplay.setAsyncActionResultListener(this.onAsyncFuncResult, this);
        }

        this.callbackDelegateMap = {};
    },

    // ================== runtime =====================

    // 初始化 GplaySDK
    initSDK: function (appKey, appSecret, privateKey) {
        cc.log("GplayImplement initSDK");
        jsb_gplay.init( appKey, appSecret, privateKey);
    },

    getChannelID: function () {
        return jsb_gplay.getChannelId();
    },

    // 检查登录状态
    isLogined: function () {
        var bIsLogined = jsb_gplay.isLogined();
        cc.log("===> is Logined ? " + bIsLogined);
        return bIsLogined;
    },

    // 登录
    login: function (callback) {
        cc.log("log in...");

        var callbackId = this.addCallbackToMapAndGetCallbackId(callback);
        jsb_gplay.login(callbackId);
    },

    // 获取用户id
    getUserID: function () {
        cc.log("get user id...");
        var userId = jsb_gplay.getUserID();
        return userId;
    },

    // 唤起分享界面
    share: function (gplayShareParams, callback) {
        cc.log("on share...");

        var param = gplayShareParams.getParamsData();
        if (param === null) {
            cc.log("Error: info incompleting");
            callback(ActionResultCode.SHARE_RESULT_INVALID, "info incompleting");
        } else {
            var jsonParam = JSON.stringify(param);
            cc.log("share info is " + jsonParam);

            var callbackId = this.addCallbackToMapAndGetCallbackId(callback);
            jsb_gplay.share(callbackId, jsonParam);
        }
    },

    // 支付
    pay: function (gplayPayParams, callback) {
        cc.log("paying...");

        var param = gplayPayParams.getParamsData();
        if (param === null) {
            cc.log("Error: info incompleting");
            callback(ActionResultCode.PAY_RESULT_INVALID, "info incompleting");
        } else {
            var jsonParam = JSON.stringify(param);
            cc.log("pay info is " + jsonParam);

            var callbackId = this.addCallbackToMapAndGetCallbackId(callback);
            jsb_gplay.pay(callbackId, jsonParam);
        }
    },

    // 创建桌面快捷图标
    createShortcut: function (callback) {
        cc.log("createShortcut...");
        var callbackId = this.addCallbackToMapAndGetCallbackId(callback);
        jsb_gplay.createShortCut(callbackId);
    },

    // 保存回调, 返回 id
    addCallbackToMapAndGetCallbackId: function (callbackFunc, callbackThis) {
        var callbackId = this._callbackID;

        if (callbackFunc) {
            this.callbackDelegateMap[callbackId]={
                callbackFunc: callbackFunc,
                callbackThis: callbackThis ? callbackThis : null
            };
        }
        this._callbackID += 1;

        return callbackId;
    },

    // 测试是否支持函数
    isSupportingFunc: function (funcName) {
        return jsb_gplay.isSupportingFunc(funcName)
    },

    // 调用渠道同步扩展接口
    callSyncFunc: function (funcName, strParams) {
        return jsb_gplay.callSyncFunc(funcName, strParams);
    },

    // 调用渠道异步扩展接口
    callAsyncFunc: function (funcName, strParams, callbackFunc, callbackThis) {
        cc.log("callAsyncFunc " + funcName + " WithParam: " + strParams);

        var callbackId = this.addCallbackToMapAndGetCallbackId(callbackFunc, callbackThis);

        strParams = strParams || ""
        jsb_gplay.callAsyncFunc(callbackId, funcName, strParams);
    },

    // 回调函数
    onAsyncFuncResult: function (ret, msg, callbackId) {
        cc.log("onAsyncFuncResult: ret=" + ret + ", msg=" + msg + ", callbackId=" + callbackId);
        var callbackDelegate = this.callbackDelegateMap[callbackId];
        if (callbackDelegate.callbackThis) {
            callbackDelegate.callbackFunc.call(callbackDelegate.callbackThis, ret, msg);
        } else {
            callbackDelegate.callbackFunc(ret, msg);
        }
        delete this.callbackDelegateMap[callbackId];
    },

    // ================== gplay =====================

    // API: 获取配置信息
    getConfig: function () {
        return jsb_gplay.config;
    },

    // 获取当前网络状态
    getNetworkType: function () {
        return jsb_gplay.getNetworkType();
    },

    // 结束游戏
    quitGame: function () {
        cc.log("Gplay jsb_gplay quitGame");
        jsb_gplay.quitGame();
    },

    backFromResourceBundle: function(bundleName) {
        if(jsb_gplay._isDivideResourceMode) {
            jsb_gplay.noticePreloadEnd(bundleName);
        }
    },

    // 加载一个或多个资源包
    preloadResourceBundles: function (bundleNames, successCallback, target) {
        if(jsb_gplay._isDivideResourceMode) {
            if (bundleNames.constructor === String) {
                jsb_gplay.noticePreloadBegin(bundleNames);
            }
            else if (bundleNames.constructor === Array) {
                if(bundleNames.length > 1)
                    jsb_gplay.noticePreloadMultipleGroup();
                else
                    jsb_gplay.noticePreloadBegin(bundleNames[0]);
            }
            else {
                cc.assert(false, "wrong parameter of preloadResourceBundles #1, only String or Array supported");
            }

            if (target)
                successCallback.call(target);
            else
                successCallback();

            return;
        }

        var objBundleNames = {};
        if (bundleNames.constructor === String) {
            objBundleNames = {
                "scenes" : [bundleNames]
            };
        } else if (bundleNames.constructor === Array) {
            objBundleNames = {
                "scenes" : bundleNames
            };
        } 
        else {
            cc.assert(false, "wrong parameter of preloadResourceBundles #1, only String or Array supported");
        }

        if (target)
            this._preloadCallback = successCallback.bind(target);
        else
            this._preloadCallback = successCallback;

        jsb_gplay.preloadGroups(JSON.stringify(objBundleNames), this._onPreloadSuccess, this);
    },

    _onPreloadSuccess: function() {
        if (this._preloadCallback) {
            this._preloadCallback();
            this._preloadCallback = null;
        }
    },

    setPreloadResponseCallback: function(callback, target) {
        if(callback) {
            if (target) {
                this._preloadResponseCallback = callback.bind(target);
            }
            else {
                this._preloadResponseCallback = callback;
            }

            jsb_gplay.setPreloadResponseCallback(this._onPreloadResponse, this);
        }
        else {
            this._preloadResponseCallback = null;
            jsb_gplay.setPreloadResponseCallback();
        }
    },

    _onPreloadResponse: function(resultCode, errorCode, bundleName, percent, speed) {
        if (this._preloadResponseCallback) {
            this._preloadResponseCallback(resultCode, errorCode, bundleName, percent, speed);
        }
    }
});

// ======================== instance ======================

if (typeof(jsb_gplay) === "object") {
    jsb_gplay._isInGplayEnv = jsb_gplay.isInGplayEnv();
    if (jsb_gplay._isInGplayEnv) {
        cc.log("return a GplayImplement named gplay"); 
        var gplay = new GplayImplement();
        jsb_gplay._isDivideResourceMode = jsb_gplay.isDivideResourceMode();
        gplay.isInGplayEnv = function() { return true; }

        gplay.preloadGroup = gplay.preloadResourceBundles;
        gplay.preloadGroups = gplay.preloadResourceBundles;
        gplay.preloadResourceBundle = gplay.preloadResourceBundles;
        gplay.backFromGroup = gplay.backFromResourceBundle;
        gplay.getChannelId = gplay.getChannelID;
    }
    else {
        cc.log("warning: current environment is not gplay");
        var gplay = {};
        gplay.isInGplayEnv = function() { return fasle; }
        gplay.preloadResourceBundles = function(sceneName, successCallback, target) {
            if (successCallback) {
                if (target) {
                    successCallback.call(target);
                } else {
                    successCallback(); 
                } 
            }
        };
        gplay.preloadGroup = gplay.preloadResourceBundles;
        gplay.preloadGroups = gplay.preloadResourceBundles;
        gplay.preloadResourceBundle = gplay.preloadResourceBundles;

        gplay.backFromResourceBundle = function() {};
        gplay.backFromGroup = gplay.backFromResourceBundle;
    }
} else { 
    cc.log("jsb_gplay not registed in spiderMonkey environment"); 
    var gplay = {}; 
    gplay.isInGplayEnv = function() {return fasle; }
    gplay.preloadResourceBundles = function(sceneName, successCallback, target) {
        if (successCallback) { 
            if (target) { 
                successCallback.call(target); 
            } else { 
                successCallback(); 
            } 
        } 
    };
    gplay.preloadGroup = gplay.preloadResourceBundles;
    gplay.preloadGroups = gplay.preloadResourceBundles;
    gplay.preloadResourceBundle = gplay.preloadResourceBundles;

    gplay.backFromResourceBundle = function() {};
    gplay.backFromGroup = gplay.backFromResourceBundle;
}
