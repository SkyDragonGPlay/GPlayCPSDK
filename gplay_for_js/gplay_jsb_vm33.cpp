#include "gplay_jsb.h"
#if COCOS2D_VERSION >= 0x00030500
#include <vector>
#include <string>
#include <android/log.h>

#include "gplay.h"

#include "js_manual_conversions.h"
#include "js_bindings_config.h"
#include "js_bindings_core.h"
#include "ScriptingCore.h"
#include "cocos2d_specifics.hpp"
#include "DivideResourceHelper.h"

#define  LOG_TAG    "gplay_jsb"
#define  LOGD(...)  __android_log_print(ANDROID_LOG_DEBUG, LOG_TAG, __VA_ARGS__)
#define  LOGE(...)  __android_log_print(ANDROID_LOG_ERROR, LOG_TAG, __VA_ARGS__)

static const char* s_CPSDKVersion = "GPLAY_CP_SDK_INFO:{\"SDK_VERSION\":\"1.5.2\",\"ENGINE\":\"COCOS_JS\",\"ENGINE_BASE_VERSION\":\"V3\",\"SDK_VERSION_NAME\":\"1.5.2 release 2\"}";

USING_NS_CC;

using namespace gplay;
using namespace std;

namespace gplay {
    extern void RTSetPreloadSuccessCallback(void(*)(int,const char*));
    extern void RTSetPreloadResponseCallback(void(*)(int,int,const std::string&,float,float));
    extern void RTCaptureScreenJNI(const std::string& params);
}

// static variabel
static int s_currPreloadExt = 0;

static jsval s_preloadCallbackJSFunc = JSVAL_VOID;
static JSObject* s_preloadCallbackJSTarget = nullptr;

static jsval s_preloadResponseJSFunc = JSVAL_VOID;
static JSObject* s_preloadResponseJSTarget = nullptr;

static jsval s_asyncJSCallbackFunc = JSVAL_VOID;
static JSObject* s_asyncJSCallbackTarget = nullptr;

static void RTPreloadSuccessCallback(int result, const char* jsonResult)
{
    JSB_AUTOCOMPARTMENT_WITH_GLOBAL_OBJCET;

    JSContext* cx = ScriptingCore::getInstance()->getGlobalContext();
    JS::RootedValue retVal(cx);

    jsval jsonResultVal = c_string_to_jsval(cx, jsonResult);

    // 调用 js 层的 successCallback, 执行当前场景游戏逻辑
    JS_CallFunctionValue(cx,
                         JS::RootedObject(cx, s_preloadCallbackJSTarget),
                         JS::RootedValue(cx,s_preloadCallbackJSFunc),
                         JS::HandleValueArray::fromMarkedLocation(1, &jsonResultVal),
                         &retVal);

    // TODO: remove these two off root
    s_preloadCallbackJSFunc = JSVAL_VOID;
    s_preloadCallbackJSTarget = nullptr;
}

static void actionResultCallback(int result, const char* jsonResult, int callbackID)
{
    CCLOG("===> actionResultCallback thread id");
    JSB_AUTOCOMPARTMENT_WITH_GLOBAL_OBJCET

    JSContext* cx = ScriptingCore::getInstance()->getGlobalContext();
    JS::RootedValue retVal(cx);

    if ( s_asyncJSCallbackFunc == JSVAL_VOID ) {
        CCLOG("===> error: AsyncFunc callbackFunc not been set");
    } else {
        CCLOG("===> AsyncFunc callbackFunc has been set");

        JS::AutoValueVector valArr(cx);
        valArr.append( INT_TO_JSVAL(result) );
        valArr.append( c_string_to_jsval(cx, jsonResult) );
        valArr.append( INT_TO_JSVAL(callbackID) );

        JS::HandleValueArray args = JS::HandleValueArray::fromMarkedLocation(3, valArr.begin());

        // 调用 js 层, 执行异步回调
        JS_CallFunctionValue(cx, 
                             JS::RootedObject(cx, s_asyncJSCallbackTarget),
                             JS::RootedValue(cx, s_asyncJSCallbackFunc),
                             args,
                             &retVal);
    }
}

// 静态函数 JS 层调用 设置 JS 回调方法到监听器单例中
static bool jsb_gplay_setAsyncActionResultListener(JSContext *cx, uint32_t argc, jsval *vp)
{
    CCLOG("jsb setAsyncActionResultListener, argc:%d.", argc);
    
    if (argc == 2)
    {
        JS::CallArgs args = JS::CallArgsFromVp(argc, vp);

        s_asyncJSCallbackFunc = args.get(0).get();
        s_asyncJSCallbackTarget = args.get(1).toObjectOrNull();

        return true;
    }
    
    JS_ReportError(cx, "jsb_gplay_setAsyncActionResultListener : wrong number of arguments: %d, was expecting %d", argc, 2);
    return false;
}

static void PreloadResponseCallbackConverter(int resultCode, int errorCode, 
        const std::string& groupName, float percent, float speed)
{
    JSB_AUTOCOMPARTMENT_WITH_GLOBAL_OBJCET

    JSContext* cx = ScriptingCore::getInstance()->getGlobalContext();
    JS::RootedValue retVal(cx);

    JS::AutoValueVector valArr(cx);
    valArr.append( INT_TO_JSVAL(resultCode) );
    valArr.append( INT_TO_JSVAL(errorCode) );
    valArr.append( c_string_to_jsval(cx, groupName.c_str()) );
    valArr.append( DOUBLE_TO_JSVAL(percent) );
    valArr.append( DOUBLE_TO_JSVAL(speed) );

    JS::HandleValueArray args = JS::HandleValueArray::fromMarkedLocation(5, valArr.begin());

    JS_CallFunctionValue(cx,
                        JS::RootedObject(cx, s_preloadResponseJSTarget),
                        JS::RootedValue(cx, s_preloadResponseJSFunc),
                        args,
                        &retVal);
}

static bool jsb_gplay_setPreloadResponseCallback(JSContext *cx, uint32_t argc, jsval *vp)
{
    if (argc == 2)
    {
        JS::CallArgs args = JS::CallArgsFromVp(argc, vp);

        s_preloadResponseJSFunc = args.get(0).get();
        s_preloadResponseJSTarget = args.get(1).toObjectOrNull();

        gplay::RTSetPreloadResponseCallback(PreloadResponseCallbackConverter);
        s_currPreloadExt = 1;

        return true;
    }
    else if (argc == 0)
    {
        s_preloadResponseJSFunc = JSVAL_VOID;
        s_preloadResponseJSTarget = nullptr;

        gplay::RTSetPreloadResponseCallback(NULL);
        s_currPreloadExt = 0;

        return true;
    }
    
    JS_ReportError(cx, "jsb_gplay_setPreloadResponseCallback : wrong number of arguments: %d, was expecting %d", argc, 2);
    return false;
}

static bool jsb_gplay_init(JSContext* cx, uint32_t argc, jsval* vp)
{
    CCLOG("jsb_gplay_init()");
    JS::CallArgs args = JS::CallArgsFromVp(argc, vp);
    
    if (argc == 3){
        string appKey;
        string appSecret;
        string privateKey;

        if (jsval_to_std_string(cx, args.get(0), &appKey) && 
            jsval_to_std_string(cx, args.get(1), &appSecret) &&
            jsval_to_std_string(cx, args.get(2), &privateKey) ) 
        {
            CCLOG("initSDK");
            gplay::common::initSDK(appKey, appSecret, privateKey);
        } else {
            CCLOG("fail to parse arguments");
        }

        args.rval().setUndefined();
        return true;
    }

    JS_ReportError(cx, "Invalid number of arguments");
    return false;
}

static bool jsb_gplay_isInGplayEnv(JSContext *cx, uint32_t argc, jsval *vp)
{
    JS::CallArgs args = JS::CallArgsFromVp(argc, vp);

    jsval jsret = BOOLEAN_TO_JSVAL(common::isInGplayEnv());
    args.rval().set(jsret);

    return true;
}

static bool jsb_gplay_isDivideResourceMode(JSContext *cx, uint32_t argc, jsval *vp)
{
    JS::CallArgs args = JS::CallArgsFromVp(argc, vp);

    jsval jsret = BOOLEAN_TO_JSVAL(GPlayFileUtils::isDivideResourceMode());
    args.rval().set(jsret);

    return true;
}

// jsb_gplay.preloadGroups(jsonGroups, successcb, target)
static bool jsb_gplay_preloadGroups(JSContext *cx, uint32_t argc, jsval *vp)
{
    JSB_PRECONDITION2( argc == 3, cx, false, "jsb_gplay_preloadGroups Invalid number of arguments" );
    JS::CallArgs args = JS::CallArgsFromVp(argc, vp);

    if (args.get(0).isString())
    {
        std::string jsonGroups = "";
        if (jsval_to_std_string(cx, args.get(0), &jsonGroups)) {

            s_preloadCallbackJSFunc = args.get(1).get();
            s_preloadCallbackJSTarget = args.get(2).toObjectOrNull();

            gplay::RTSetPreloadSuccessCallback(RTPreloadSuccessCallback);

            gplay::common::preloadGroups(jsonGroups, s_currPreloadExt);

            args.rval().setUndefined();
            return true;
        }

        JS_ReportError(cx, "Error processing arguments");
        args.rval().setUndefined();
        return false;
    }

    LOGE("first argument of preload should be string");
    args.rval().setUndefined();
    return false;
}

#define LOG_DIVIDE_ERROR(...)  __android_log_print(ANDROID_LOG_ERROR, "gplay_divide_res", __VA_ARGS__)

static bool jsb_gplay_noticePreloadMultipleGroup(JSContext *cx, uint32_t argc, jsval *vp)
{
    JS::CallArgs args = JS::CallArgsFromVp(argc, vp);
    LOG_DIVIDE_ERROR("error-preload-multiple");
    
    JS_ReportError(cx, "gplay.preloadGroups not support preload multiple group in automatic mode");
    args.rval().setUndefined();
    return false;
}

static bool jsb_gplay_noticePreloadBegin(JSContext *cx, uint32_t argc, jsval *vp)
{
    JS::CallArgs args = JS::CallArgsFromVp(argc, vp);

    if (argc > 0 && args.get(0).isString())
    {
        std::string sceneName = "";
        if (jsval_to_std_string(cx, args.get(0), &sceneName)) {

            common::noticePreloadBegin(sceneName);

            args.rval().setUndefined();
            return true;
        }

        JS_ReportError(cx, "Error processing arguments");
        args.rval().setUndefined();
        return false;
    }

    LOGE("First argument of jsb_gplay.noticePreloadBegin should be string");
    args.rval().setUndefined();
    return false;
}

static bool jsb_gplay_noticePreloadEnd(JSContext *cx, uint32_t argc, jsval *vp)
{
    JS::CallArgs args = JS::CallArgsFromVp(argc, vp);

    if (argc > 0 && args.get(0).isString())
    {
        std::string sceneName = "";
        if (jsval_to_std_string(cx, args.get(0), &sceneName)) {

            common::noticePreloadEnd(sceneName);

            args.rval().setUndefined();
            return true;
        }

        JS_ReportError(cx, "Error processing arguments");
        args.rval().setUndefined();
        return false;
    }

    LOGE("First argument of gplay.endPreloadGroup should be string");
    args.rval().setUndefined();
    return false;
}

// jsb_gplay.getNetworkType()
static bool jsb_gplay_getNetworkType(JSContext *cx, uint32_t argc, jsval *vp)
{
    LOGD("calling jsb_gplay_getNetworkType");
    
    JS::CallArgs argv = JS::CallArgsFromVp(argc, vp);
    int status = gplay::common::getNetworkType();
    argv.rval().setInt32(status);
    return true;
}

// jsb_gplay.quitGame()
static bool jsb_gplay_quitGame(JSContext *cx, uint32_t argc, jsval *vp)
{
    LOGD("calling jsb_gplay_quitGame");
    
    JS::CallArgs argv = JS::CallArgsFromVp(argc, vp);
    gplay::common::quitGame();
    argv.rval().setUndefined();
    return true;
}

static bool jsb_gplay_isLogon(JSContext* cx, uint32_t argc, jsval* vp)
{
    CCLOG("jsb_gplay_isLogon()");
    JS::CallArgs args = JS::CallArgsFromVp(argc, vp);
    
    bool isLogged = gplay::common::isLogined();
    args.rval().set( JS::BooleanValue(isLogged) );
    return true;
}

static bool jsb_gplay_getUserID(JSContext* cx, uint32_t argc, jsval* vp)
{
    CCLOG("jsb_gplay_getUserID()");
    JS::CallArgs args = JS::CallArgsFromVp(argc, vp);
    
    const std::string& userId = gplay::common::getUserID();
    args.rval().set( std_string_to_jsval(cx, userId) );
    return true;
}

static bool jsb_gplay_getOrderId(JSContext* cx, uint32_t argc, jsval* vp)
{
    CCLOG("jsb_gplay_getOrderId()");
    JS::CallArgs args = JS::CallArgsFromVp(argc, vp);
    
    const std::string& orderId = gplay::common::getOrderId();
    args.rval().set( std_string_to_jsval(cx, orderId) );
    return true;
}

static bool jsb_gplay_getChannelId(JSContext* cx, uint32_t argc, jsval* vp)
{
    CCLOG("jsb_gplay_getChannelId()");
    JS::CallArgs args = JS::CallArgsFromVp(argc, vp);
    
    const std::string& channelId = gplay::common::getChannelID();
    args.rval().set( std_string_to_jsval(cx, channelId) );
    return true;
}

static bool jsb_gplay_isSupportingFunc(JSContext* cx, uint32_t argc, jsval* vp)
{
    CCLOG("jsb_gplay_isSupportingFunc()");
    
    if (argc == 1)
    {
        JS::CallArgs args = JS::CallArgsFromVp(argc, vp);
        string funtionName;
        if ( jsval_to_std_string(cx, args.get(0), &funtionName) )
        {
            bool isSupportingFunc = gplay::common::isFunctionSupported(funtionName);
            args.rval().set( JS::BooleanValue(isSupportingFunc) );
            return true;
        }
        
        JS_ReportError(cx, "Error processing arguments");
        return false;
    }

    JS_ReportError(cx, "Invalid number of arguments");
    return false;
}

static bool jsb_gplay_login(JSContext* cx, uint32_t argc, jsval* vp)
{
    CCLOG("jsb_gplay_login()");
    
    if (argc == 1)
    {
        JS::CallArgs args = JS::CallArgsFromVp(argc, vp);
        int callbackID;
        if (jsval_to_int(cx, args.get(0), &callbackID))
        {
            gplay::common::login(callbackID, actionResultCallback);
            return true;
        }
        
        JS_ReportError(cx, "fail to process argument");
        return false;
    }

    JS_ReportError(cx, "Invalid number of arguments");
    return false;
}

static bool jsb_gplay_share(JSContext* cx, uint32_t argc, jsval* vp)
{
    CCLOG("jsb_gplay_share()");
    
    if (argc == 2)
    {
        JS::CallArgs args = JS::CallArgsFromVp(argc, vp);
        JS::RootedValue jsCallbackId(cx, args.get(0));
        JS::RootedValue jsParam(cx, args.get(1));
        
        int callbackID;
        string param;
        if (jsval_to_int(cx, jsCallbackId, &callbackID) &&
            jsval_to_std_string(cx, jsParam, &param) )
        {
            gplay::common::share(callbackID, param, actionResultCallback);
            return true;
        }
        
        CCLOG("jsb_gplay_share: Error processing arguments");
        return false;
    }

    JS_ReportError(cx, "Invalid number of arguments");
    return false;
}

static bool jsb_gplay_pay(JSContext* cx, uint32_t argc, jsval* vp)
{
    CCLOG("jsb_gplay_pay()");
    
    if (argc == 2)
    {
        JS::CallArgs args = JS::CallArgsFromVp(argc, vp);
        JS::RootedValue jsCallbackId(cx, args.get(0));
        JS::RootedValue jsParam(cx, args.get(1));

        string payInfo;
        int callbackID;

        if (jsval_to_int(cx, jsCallbackId, &callbackID) &&
            jsval_to_std_string(cx, jsParam, &payInfo) )
        {
            gplay::common::pay(callbackID, payInfo, actionResultCallback);
            return true;
        }

        CCLOG("jsb_gplay_pay: Error processing arguments");
        return false;
    }

    JS_ReportError(cx, "Wrong number of arguments");
    return false;
}

static bool jsb_gplay_createShortCut(JSContext* cx, uint32_t argc, jsval* vp)
{
    CCLOG("jsb_gplay_createShortCut()");
    
    if (argc == 1)
    {
        JS::CallArgs args = JS::CallArgsFromVp(argc, vp);
        int callbackID;
        JS::RootedValue jsCallbackId(cx, args.get(0));

        if (jsval_to_int(cx, jsCallbackId, &callbackID))
        {
            gplay::common::createShortcut(callbackID, actionResultCallback);
            return true;
        }
        
        CCLOG("jsb_gplay_createShortCut: Error processing arguments");
        return false;
    }
    
    JS_ReportError(cx, "Wrong number of arguments");
    return false;
}

static bool jsb_gplay_callSyncFunc(JSContext* cx, uint32_t argc, jsval* vp)
{
    CCLOG("jsb_gplay_callSyncFunc()");
    
    if(2 == argc)
    {
        JS::CallArgs args = JS::CallArgsFromVp(argc, vp);
        string funtionName;
        string jsonParams;

        if( jsval_to_std_string(cx, args.get(0), &funtionName) && 
            jsval_to_std_string(cx, args.get(1), &jsonParams))
        {
            string result = gplay::common::callSyncFunc(funtionName, jsonParams);
            args.rval().set( std_string_to_jsval(cx, result) );
            return true;
        }

        CCLOG("failed to process funtion name");
        args.rval().setUndefined();
        return false;
    }

    JS_ReportError(cx, "Invalid number of arguments");
    return false;
}

static int s_captureScreenCallbackID;

static bool jsb_gplay_callAsyncFunc(JSContext* cx, uint32_t argc, jsval* vp)
{
    CCLOG("jsb_gplay_callAsyncFunc()");
    
    if(3 == argc)
    {
        JS::CallArgs args = JS::CallArgsFromVp(argc, vp);
        string funtionName;
        string jsonParams;
        int callbackID;

        if(jsval_to_int(cx, args.get(0), &callbackID) &&
           jsval_to_std_string(cx, args.get(1), &funtionName) &&
           jsval_to_std_string(cx, args.get(2), &jsonParams))
        {
            CCLOG("funtionName: %s, callbackID: %d\n", funtionName.c_str(), callbackID);

            if (funtionName.compare("CAPTURE_SCREEN") == 0)
            {
                s_captureScreenCallbackID = callbackID;
                RTCaptureScreenJNI(jsonParams);
            }
            else
                gplay::common::callAsyncFunc(callbackID, funtionName, jsonParams, actionResultCallback);

            return true;
        }
            
        CCLOG("failt to process arguments");
        return false;
    }

    JS_ReportError(cx, "Invalid number of arguments");
    return false;
}

void register_gplay_runtime(JSContext* cx, JS::HandleObject global)
{
    JS::RootedObject jsbGplayObj(cx);
    get_or_create_js_obj(cx, JS::RootedObject(cx, global), "jsb_gplay", &jsbGplayObj);

    JS_DefineFunction(cx, jsbGplayObj, "isLogined", jsb_gplay_isLogon, 0, JSPROP_PERMANENT | JSPROP_ENUMERATE);
    JS_DefineFunction(cx, jsbGplayObj, "getUserID", jsb_gplay_getUserID, 0, JSPROP_PERMANENT | JSPROP_ENUMERATE);
    JS_DefineFunction(cx, jsbGplayObj, "getOrderId", jsb_gplay_getOrderId, 0, JSPROP_PERMANENT | JSPROP_ENUMERATE);
    JS_DefineFunction(cx, jsbGplayObj, "getChannelId", jsb_gplay_getChannelId, 0, JSPROP_PERMANENT | JSPROP_ENUMERATE);
    JS_DefineFunction(cx, jsbGplayObj, "isSupportingFunc", jsb_gplay_isSupportingFunc, 1, JSPROP_PERMANENT | JSPROP_ENUMERATE);
    JS_DefineFunction(cx, jsbGplayObj, "login", jsb_gplay_login, 1, JSPROP_PERMANENT | JSPROP_ENUMERATE);
    JS_DefineFunction(cx, jsbGplayObj, "share", jsb_gplay_share, 2, JSPROP_PERMANENT | JSPROP_ENUMERATE);
    JS_DefineFunction(cx, jsbGplayObj, "pay", jsb_gplay_pay, 2, JSPROP_PERMANENT | JSPROP_ENUMERATE);
    JS_DefineFunction(cx, jsbGplayObj, "createShortCut", jsb_gplay_createShortCut, 1, JSPROP_PERMANENT | JSPROP_ENUMERATE);
    JS_DefineFunction(cx, jsbGplayObj, "callSyncFunc", jsb_gplay_callSyncFunc, 2, JSPROP_PERMANENT | JSPROP_ENUMERATE);
    JS_DefineFunction(cx, jsbGplayObj, "callAsyncFunc", jsb_gplay_callAsyncFunc, 3, JSPROP_PERMANENT | JSPROP_ENUMERATE);
    JS_DefineFunction(cx, jsbGplayObj, "preloadGroups", jsb_gplay_preloadGroups, 3, JSPROP_READONLY | JSPROP_PERMANENT | JSPROP_ENUMERATE );
    JS_DefineFunction(cx, jsbGplayObj, "noticePreloadMultipleGroup", jsb_gplay_noticePreloadMultipleGroup, 0, JSPROP_READONLY | JSPROP_PERMANENT | JSPROP_ENUMERATE );
    JS_DefineFunction(cx, jsbGplayObj, "noticePreloadBegin", jsb_gplay_noticePreloadBegin, 1, JSPROP_READONLY | JSPROP_PERMANENT | JSPROP_ENUMERATE );
    JS_DefineFunction(cx, jsbGplayObj, "noticePreloadEnd", jsb_gplay_noticePreloadEnd, 1, JSPROP_READONLY | JSPROP_PERMANENT | JSPROP_ENUMERATE );
    JS_DefineFunction(cx, jsbGplayObj, "getNetworkType", jsb_gplay_getNetworkType, 0, JSPROP_READONLY | JSPROP_PERMANENT | JSPROP_ENUMERATE );
    JS_DefineFunction(cx, jsbGplayObj, "quitGame", jsb_gplay_quitGame, 0, JSPROP_READONLY | JSPROP_PERMANENT | JSPROP_ENUMERATE );
    JS_DefineFunction(cx, jsbGplayObj, "isInGplayEnv", jsb_gplay_isInGplayEnv, 0, JSPROP_READONLY | JSPROP_PERMANENT | JSPROP_ENUMERATE );
    JS_DefineFunction(cx, jsbGplayObj, "isDivideResourceMode", jsb_gplay_isDivideResourceMode, 0, JSPROP_READONLY | JSPROP_PERMANENT | JSPROP_ENUMERATE );
    JS_DefineFunction(cx, jsbGplayObj, "init", jsb_gplay_init, 3, JSPROP_PERMANENT | JSPROP_ENUMERATE);
    JS_DefineFunction(cx, jsbGplayObj, "setAsyncActionResultListener", jsb_gplay_setAsyncActionResultListener, 2, JSPROP_ENUMERATE | JSPROP_PERMANENT);
    JS_DefineFunction(cx, jsbGplayObj, "setPreloadResponseCallback", jsb_gplay_setPreloadResponseCallback, 2, JSPROP_ENUMERATE | JSPROP_PERMANENT);
}

namespace gplay
{
    void RTQuitGameCallback()
    {
        LOGD("RTQuitGameCallback()");
        cocos2d::Director::getInstance()->end();

        //reset static variables
        s_preloadCallbackJSTarget = nullptr;
        s_preloadCallbackJSFunc = JSVAL_VOID;
        s_asyncJSCallbackTarget = nullptr;
        s_asyncJSCallbackFunc = JSVAL_VOID;
        s_preloadResponseJSTarget = nullptr;
        s_preloadResponseJSFunc = JSVAL_VOID;
        s_currPreloadExt = 0;
    }

    void onPreGameStart()
    {
        LOGD("%s", s_CPSDKVersion);
        ScriptingCore* sc = ScriptingCore::getInstance();
        sc->addRegisterCallback(register_gplay_runtime);
    }
}

extern "C" {
    void Java_org_cocos2dx_lib_Cocos2dxHelper_nativeOnCaptureScreenResult(JNIEnv* env, jobject thiz, jint resultCode)
    {
        if (resultCode == 1)
            actionResultCallback(gplay::common::CAPTURE_SCREEN_SUCCESS, "", s_captureScreenCallbackID);
        else
            actionResultCallback(gplay::common::CAPTURE_SCREEN_FAILED, "", s_captureScreenCallbackID);
    }
}

#endif
