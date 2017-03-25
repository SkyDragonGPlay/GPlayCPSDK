#include "gplay_jsb.h"
#if (COCOS2D_VERSION >= 0x00030000 && COCOS2D_VERSION < 0x00030500)
#include <vector>
#include <string>
#include <android/log.h>

#include "js_manual_conversions.h"
#include "js_bindings_config.h"
#include "js_bindings_core.h"
#include "ScriptingCore.h"
#include "cocos2d_specifics.hpp"
#include "gplay.h"
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

static jsval s_asyncJSCallbackFunc = JSVAL_VOID;
static jsval s_asyncJSCallbackTarget = JSVAL_VOID;

static jsval s_preloadSuccessJSFunc = JSVAL_VOID;
static jsval s_preloadSuccessJSTarget = JSVAL_VOID;

static jsval s_preloadResponseJSFunc = JSVAL_VOID;
static jsval s_preloadResponseJSTarget = JSVAL_VOID;

static int s_currPreloadExt = 0;

static void RTPreloadSuccessCallback(int result, const char* jsonResult)
{
    JSB_AUTOCOMPARTMENT_WITH_GLOBAL_OBJCET;

    JSContext* cx = ScriptingCore::getInstance()->getGlobalContext();
    jsval retVal;
    JS::RootedValue jsonResultVal(cx);

    jsval jsonResultStringVal = c_string_to_jsval(cx, jsonResult);

    JSObject *thisObj = JSVAL_IS_VOID(s_preloadSuccessJSTarget) ? NULL : JSVAL_TO_OBJECT(s_preloadSuccessJSTarget);

    // 调用 js 层的 successCallback, 执行当前场景游戏逻辑
    JS_CallFunctionValue(cx,
                         thisObj,
                         JS::RootedValue(cx,s_preloadSuccessJSFunc),
                         1,
                         &jsonResultStringVal,
                         &retVal);

    // TODO: remove these two off root
    s_preloadSuccessJSFunc = JSVAL_VOID;
    s_preloadSuccessJSTarget = JSVAL_VOID;
}

static void actionResultCallback(int result, const char* jsonResult, int callbackId)
{
    JSContext* cx = ScriptingCore::getInstance()->getGlobalContext();
    JSObject *thisObj = JSVAL_IS_VOID(s_asyncJSCallbackTarget) ? NULL : JSVAL_TO_OBJECT(s_asyncJSCallbackTarget);

    jsval retval;

    if ( s_asyncJSCallbackFunc == JSVAL_VOID ) {
        CCLOG("===> error: AsyncFunc callbackFunc not been set");
    } else {

        jsval valArr[3];
        valArr[0] = INT_TO_JSVAL(result);
        valArr[1] = std_string_to_jsval(cx, jsonResult);
        valArr[2] = INT_TO_JSVAL(callbackId);

        JS_AddValueRoot(cx, valArr);

        JSB_AUTOCOMPARTMENT_WITH_GLOBAL_OBJCET
        JS_CallFunctionValue(cx, thisObj, s_asyncJSCallbackFunc, 3, valArr, &retval);

        JS_RemoveValueRoot(cx, valArr);
    }
}

// 静态函数 JS 层调用 设置 JS 回调方法到监听器单例中
static bool jsb_gplay_setAsyncActionResultListener(JSContext *cx, uint32_t argc, jsval *vp)
{
    CCLOG("jsb setAsyncActionResultListener, argc:%d.", argc);

    if (argc == 2)
    {
        jsval *argv = JS_ARGV(cx, vp);
        s_asyncJSCallbackFunc = argv[0];
        s_asyncJSCallbackTarget = argv[1];
        return true;
    }

    JS_ReportError(cx, "jsb_gplay_setAsyncActionResultListener : wrong number of arguments: %d, was expecting %d", argc, 2);
    return false;
}

static void PreloadResponseCallbackConverter(int resultCode, int errorCode, 
        const std::string& groupName, float percent, float speed)
{
    JSContext* cx = ScriptingCore::getInstance()->getGlobalContext();
    JSObject *thisObj = JSVAL_IS_VOID(s_preloadResponseJSTarget) ? NULL : JSVAL_TO_OBJECT(s_asyncJSCallbackTarget);

    jsval retval;

    jsval valArr[5];
    valArr[0] = INT_TO_JSVAL(resultCode);
    valArr[1] = INT_TO_JSVAL(errorCode);
    valArr[2] = c_string_to_jsval(cx, groupName.c_str());
    valArr[3] = DOUBLE_TO_JSVAL(percent);
    valArr[4] = DOUBLE_TO_JSVAL(speed);

    JS_AddValueRoot(cx, valArr);

    JSB_AUTOCOMPARTMENT_WITH_GLOBAL_OBJCET
    JS_CallFunctionValue(cx, thisObj, s_preloadResponseJSFunc, 5, valArr, &retval);

    JS_RemoveValueRoot(cx, valArr);
}

static bool jsb_gplay_setPreloadResponseCallback(JSContext *cx, uint32_t argc, jsval *vp)
{
    if (argc == 2)
    {
        jsval *argv = JS_ARGV(cx, vp);
        s_preloadResponseJSFunc = argv[0];
        s_preloadResponseJSTarget = argv[1];

        gplay::RTSetPreloadResponseCallback(PreloadResponseCallbackConverter);
        s_currPreloadExt = 1;

        return true;
    }
    else if (argc == 0)
    {
        s_preloadResponseJSFunc = JSVAL_VOID;
        s_preloadResponseJSTarget = JSVAL_VOID;

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
    jsval *argv = JS_ARGV(cx, vp);

    if (argc == 3){
        string appKey;
        string appSecret;
        string privateKey;

        if (jsval_to_std_string(cx, argv[0], &appKey) && 
            jsval_to_std_string(cx, argv[1], &appSecret) &&
            jsval_to_std_string(cx, argv[2], &privateKey) ) 
        {
            CCLOG("initSDK");
            gplay::common::initSDK(appKey, appSecret, privateKey);
        } else {
            CCLOG("fail to parse arguments");
        }

        JS_SET_RVAL(cx, vp, JSVAL_VOID);
        return true;
    }

    JS_ReportError(cx, "Wrong number of arguments");
    return false;
}

static bool jsb_gplay_isInGplayEnv(JSContext *cx, uint32_t argc, jsval *vp)
{
    jsval *argv = JS_ARGV(cx, vp);

    jsval jsret = BOOLEAN_TO_JSVAL(common::isInGplayEnv());
    JS_SET_RVAL(cx, vp, jsret);

    return true;
}

static bool jsb_gplay_isDivideResourceMode(JSContext *cx, uint32_t argc, jsval *vp)
{
    jsval *argv = JS_ARGV(cx, vp);

    jsval jsret = BOOLEAN_TO_JSVAL(GPlayFileUtils::isDivideResourceMode());
    JS_SET_RVAL(cx, vp, jsret);

    return true;
}

// jsb_gplay.preloadGroups(jsonGroups, successcb, target)
static bool jsb_gplay_preloadGroups(JSContext *cx, uint32_t argc, jsval *vp)
{
    JSB_PRECONDITION2( argc == 3, cx, false, "jsb_gplay_preloadGroups Invalid number of arguments" );
    jsval *argv = JS_ARGV(cx, vp);

    if (argv[0].isString())
    {
        std::string jsonGroups;
        if (jsval_to_std_string(cx, argv[0], &jsonGroups))
        {

            s_preloadSuccessJSFunc = argv[1];
            s_preloadSuccessJSTarget = argv[2];

            gplay::RTSetPreloadSuccessCallback(RTPreloadSuccessCallback);

            gplay::common::preloadGroups(jsonGroups); // through gplay

            JS_SET_RVAL(cx, vp, JSVAL_VOID);
            return true;
        }

        JS_ReportError(cx, "Error processing arguments");
        JS_SET_RVAL(cx, vp, JSVAL_VOID);
        return false;
    }

    LOGE("first argument of preload should be string");
    JS_SET_RVAL(cx, vp, JSVAL_VOID);
    return false;
}

#define LOG_DIVIDE_ERROR(...)  __android_log_print(ANDROID_LOG_ERROR, "gplay_divide_res", __VA_ARGS__)

static bool jsb_gplay_noticePreloadMultipleGroup(JSContext *cx, uint32_t argc, jsval *vp)
{
    LOG_DIVIDE_ERROR("error-preload-multiple");
    JS_ReportError(cx, "gplay.preloadGroups not support preload multiple group in automatic mode");
    JS_SET_RVAL(cx, vp, JSVAL_VOID);
    return false;
}

static bool jsb_gplay_noticePreloadBegin(JSContext *cx, uint32_t argc, jsval *vp)
{
    jsval *argv = JS_ARGV(cx, vp);

    if (argc > 0 && argv[0].isString())
    {
        std::string sceneName;
        if (jsval_to_std_string(cx, argv[0], &sceneName))
        {
            common::noticePreloadBegin(sceneName);
            JS_SET_RVAL(cx, vp, JSVAL_VOID);
            return true;
        }

        JS_ReportError(cx, "Error processing arguments");
        JS_SET_RVAL(cx, vp, JSVAL_VOID);
        return false;
    }

    LOGE("First argument of jsb_gplay.noticePreloadBegin should be string");
    JS_SET_RVAL(cx, vp, JSVAL_VOID);
    return false;
}

static bool jsb_gplay_noticePreloadEnd(JSContext *cx, uint32_t argc, jsval *vp)
{
    jsval *argv = JS_ARGV(cx, vp);

    if (argc > 0 && argv[0].isString())
    {
        std::string sceneName;
        if (jsval_to_std_string(cx, argv[0], &sceneName))
        {
            common::noticePreloadEnd(sceneName);
            JS_SET_RVAL(cx, vp, JSVAL_VOID);
            return true;
        }

        JS_ReportError(cx, "Error processing arguments");
        JS_SET_RVAL(cx, vp, JSVAL_VOID);
        return false;
    }

    LOGE("First argument of jsb_gplay.endPreloadGroup should be string");
    JS_SET_RVAL(cx, vp, JSVAL_VOID);
    return false;
}

// jsb_gplay.getNetworkType()
static bool jsb_gplay_getNetworkType(JSContext *cx, uint32_t argc, jsval *vp)
{
    LOGD("calling jsb_gplay_getNetworkType");
    
    JS::CallArgs argv = JS::CallArgsFromVp(argc, vp);
    int status = gplay::common::getNetworkType();
    JS_SET_RVAL(cx, vp, INT_TO_JSVAL(status));
    return true;
}

// jsb_gplay.quitGame()
static bool jsb_gplay_quitGame(JSContext *cx, uint32_t argc, jsval *vp)
{
    LOGD("calling jsb_gplay_quitGame");
    
    JS::CallArgs argv = JS::CallArgsFromVp(argc, vp);
    gplay::common::quitGame();
    JS_SET_RVAL(cx, vp, JSVAL_VOID);
    return true;
}

static bool jsb_gplay_isLogon(JSContext* cx, uint32_t argc, jsval* vp)
{
    CCLOG("jsb_gplay_isLogon()");
    jsval *argv = JS_ARGV(cx, vp);
    
    bool isLogged = gplay::common::isLogined();
    JS_SET_RVAL(cx, vp, BOOLEAN_TO_JSVAL(isLogged));
    return true;
}

static bool jsb_gplay_getUserID(JSContext* cx, uint32_t argc, jsval* vp)
{
    CCLOG("jsb_gplay_getUserID()");

    jsval *argv = JS_ARGV(cx, vp);
    const std::string& userId = gplay::common::getUserID();
    JS_SET_RVAL(cx, vp, std_string_to_jsval(cx, userId));
    return true;
}

static bool jsb_gplay_getOrderId(JSContext* cx, uint32_t argc, jsval* vp)
{
    CCLOG("jsb_gplay_getOrderId()");
    jsval *argv = JS_ARGV(cx, vp);
    
    const std::string& orderId = gplay::common::getOrderId();
    JS_SET_RVAL(cx, vp, std_string_to_jsval(cx, orderId));
    return true;
}

static bool jsb_gplay_getChannelId(JSContext* cx, uint32_t argc, jsval* vp)
{
    CCLOG("jsb_gplay_getChannelId()");
    jsval *argv = JS_ARGV(cx, vp);
    
    const std::string& channelId = gplay::common::getChannelID();
    JS_SET_RVAL(cx, vp, std_string_to_jsval(cx, channelId));
    return true;
}

static bool jsb_gplay_isSupportingFunc(JSContext* cx, uint32_t argc, jsval* vp)
{
    CCLOG("jsb_gplay_isSupportingFunc()");
    jsval *argv = JS_ARGV(cx, vp);
    
    if (argc == 1)
    {
        string funtionName;
        if ( jsval_to_std_string(cx, argv[0], &funtionName) )
        {
            bool isSupportingFunc = gplay::common::isFunctionSupported(funtionName);
            JS_SET_RVAL(cx, vp, BOOLEAN_TO_JSVAL(isSupportingFunc));
            return true;
        }

        JS_ReportError(cx, "Error processing arguments");
        return false;
    }

    JS_ReportError(cx, "Wrong number of arguments");
    return false;
}

static bool jsb_gplay_login(JSContext* cx, uint32_t argc, jsval* vp)
{
    CCLOG("jsb_gplay_login()");
    jsval *argv = JS_ARGV(cx, vp);
    
    if (argc == 1)
    {
        int callbackId;
        if (jsval_to_int(cx, argv[0], &callbackId))
        {
            gplay::common::login(callbackId, actionResultCallback);
            return true;
        }

        JS_ReportError(cx, "fail to process argument");
        return false;
    }

    JS_ReportError(cx, "Wrong number of arguments");
    return false;
}

static bool jsb_gplay_share(JSContext* cx, uint32_t argc, jsval* vp)
{
    CCLOG("jsb_gplay_share()");
    
    jsval *argv = JS_ARGV(cx, vp);
    
    if (argc == 2)
    {
        JS::RootedValue jsCallbackId(cx, argv[0]);
        JS::RootedValue jsParam(cx, argv[1]);
        int callbackId;
        string param;

        if (jsval_to_int(cx, jsCallbackId, &callbackId) &&
            jsval_to_std_string(cx, jsParam, &param) )
        {
            gplay::common::share(callbackId, param, actionResultCallback);
            return true;
        }

        CCLOG("jsb_gplay_share: Error processing arguments");
        return false;
    }

    JS_ReportError(cx, "Wrong number of arguments");
    return false;
}

static bool jsb_gplay_pay(JSContext* cx, uint32_t argc, jsval* vp)
{
    CCLOG("jsb_gplay_pay()");
    jsval *argv = JS_ARGV(cx, vp);
    
    if (argc == 2)
    {
        JS::RootedValue jsCallbackId(cx, argv[0]);
        JS::RootedValue jsParam(cx, argv[1]);
        string param;
        int callbackId;

        if (jsval_to_int(cx, jsCallbackId, &callbackId) &&
            jsval_to_std_string(cx, jsParam, &param) )
        {
            gplay::common::pay(callbackId, param, actionResultCallback);
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
    jsval *argv = JS_ARGV(cx, vp);
    
    if (argc == 1)
    {
        int callbackId;
        JS::RootedValue jsCallbackId(cx, argv[0]);
        
        if (jsval_to_int(cx, jsCallbackId, &callbackId))
        {
            gplay::common::createShortcut(callbackId, actionResultCallback);
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
    
    jsval *argv = JS_ARGV(cx, vp);

    if(2 == argc)
    {
        string funtionName;
        string jsonParams;
        if( jsval_to_std_string(cx, argv[0], &funtionName) && 
            jsval_to_std_string(cx, argv[1], &jsonParams))
        {
            string result = gplay::common::callSyncFunc(funtionName, jsonParams);
            JS_SET_RVAL(cx, vp, std_string_to_jsval(cx, result));
            return true;
        }

        CCLOG("failed to process funtion name");
        JS_SET_RVAL(cx, vp, JSVAL_VOID);
        return false;
    }

    JS_ReportError(cx, "Invalid number of arguments");
    return false;
}

static int s_captureScreenCallbackID;

static bool jsb_gplay_callAsyncFunc(JSContext* cx, uint32_t argc, jsval* vp)
{
    CCLOG("jsb_gplay_callAsyncFunc()");
    jsval *argv = JS_ARGV(cx, vp);
    
    if(3 == argc)
    {
        string funtionName;
        string jsonParams;
        int callbackID;

        if(jsval_to_int(cx, argv[0], &callbackID) &&
           jsval_to_std_string(cx, argv[1], &funtionName) &&
           jsval_to_std_string(cx, argv[2], &jsonParams))
        {
            CCLOG("funtionName: %s, callbackId: %d\n", funtionName.c_str(), callbackID);
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

void register_gplay_runtime(JSContext* cx, JSObject* global)
{
    LOGE("sc->register_gplay_runtime");

    JS::RootedValue jsbGplayVal(cx);
    JS::RootedObject jsbGplayObj(cx);
    JS_GetProperty(cx, global, "jsb_gplay", &jsbGplayVal);
    if (jsbGplayVal == JSVAL_VOID) {
        jsbGplayObj = JS_NewObject(cx, NULL, NULL, NULL);
        jsbGplayVal = OBJECT_TO_JSVAL(jsbGplayObj);
        JS_SetProperty(cx, global, "jsb_gplay", jsbGplayVal);
    } else {
        JS_ValueToObject(cx, jsbGplayVal, &jsbGplayObj);
    }

    JS_DefineFunction(cx, jsbGplayObj, "isLogined", jsb_gplay_isLogon, 0, JSPROP_PERMANENT | JSPROP_ENUMERATE),
    JS_DefineFunction(cx, jsbGplayObj, "getUserID", jsb_gplay_getUserID, 0, JSPROP_PERMANENT | JSPROP_ENUMERATE),
    JS_DefineFunction(cx, jsbGplayObj, "getOrderId", jsb_gplay_getOrderId, 0, JSPROP_PERMANENT | JSPROP_ENUMERATE),
    JS_DefineFunction(cx, jsbGplayObj, "getChannelId", jsb_gplay_getChannelId, 0, JSPROP_PERMANENT | JSPROP_ENUMERATE),
    JS_DefineFunction(cx, jsbGplayObj, "isSupportingFunc", jsb_gplay_isSupportingFunc, 1, JSPROP_PERMANENT | JSPROP_ENUMERATE),
    JS_DefineFunction(cx, jsbGplayObj, "login", jsb_gplay_login, 1, JSPROP_PERMANENT | JSPROP_ENUMERATE),
    JS_DefineFunction(cx, jsbGplayObj, "share", jsb_gplay_share, 2, JSPROP_PERMANENT | JSPROP_ENUMERATE),
    JS_DefineFunction(cx, jsbGplayObj, "pay", jsb_gplay_pay, 2, JSPROP_PERMANENT | JSPROP_ENUMERATE),
    JS_DefineFunction(cx, jsbGplayObj, "createShortCut", jsb_gplay_createShortCut, 1, JSPROP_PERMANENT | JSPROP_ENUMERATE),
    JS_DefineFunction(cx, jsbGplayObj, "callSyncFunc", jsb_gplay_callSyncFunc, 2, JSPROP_PERMANENT | JSPROP_ENUMERATE),
    JS_DefineFunction(cx, jsbGplayObj, "callAsyncFunc", jsb_gplay_callAsyncFunc, 3, JSPROP_PERMANENT | JSPROP_ENUMERATE),
    JS_DefineFunction(cx, jsbGplayObj, "preloadGroups", jsb_gplay_preloadGroups, 3, JSPROP_READONLY | JSPROP_PERMANENT | JSPROP_ENUMERATE );
    JS_DefineFunction(cx, jsbGplayObj, "noticePreloadMultipleGroup", jsb_gplay_noticePreloadMultipleGroup, 0, JSPROP_READONLY | JSPROP_PERMANENT | JSPROP_ENUMERATE );
    JS_DefineFunction(cx, jsbGplayObj, "noticePreloadBegin", jsb_gplay_noticePreloadBegin, 1, JSPROP_READONLY | JSPROP_PERMANENT | JSPROP_ENUMERATE );
    JS_DefineFunction(cx, jsbGplayObj, "noticePreloadEnd", jsb_gplay_noticePreloadEnd, 1, JSPROP_READONLY | JSPROP_PERMANENT | JSPROP_ENUMERATE );
    JS_DefineFunction(cx, jsbGplayObj, "getNetworkType", jsb_gplay_getNetworkType, 0, JSPROP_READONLY | JSPROP_PERMANENT | JSPROP_ENUMERATE );
    JS_DefineFunction(cx, jsbGplayObj, "quitGame", jsb_gplay_quitGame, 0, JSPROP_READONLY | JSPROP_PERMANENT | JSPROP_ENUMERATE );
    JS_DefineFunction(cx, jsbGplayObj, "isInGplayEnv", jsb_gplay_isInGplayEnv, 0, JSPROP_READONLY | JSPROP_PERMANENT | JSPROP_ENUMERATE );
    JS_DefineFunction(cx, jsbGplayObj, "isDivideResourceMode", jsb_gplay_isDivideResourceMode, 0, JSPROP_READONLY | JSPROP_PERMANENT | JSPROP_ENUMERATE );
    JS_DefineFunction(cx, jsbGplayObj, "init", jsb_gplay_init, 3, JSPROP_PERMANENT | JSPROP_ENUMERATE),
    JS_DefineFunction(cx, jsbGplayObj, "setAsyncActionResultListener", jsb_gplay_setAsyncActionResultListener, 1, JSPROP_ENUMERATE | JSPROP_PERMANENT);
    JS_DefineFunction(cx, jsbGplayObj, "setPreloadResponseCallback", jsb_gplay_setPreloadResponseCallback, 2, JSPROP_ENUMERATE | JSPROP_PERMANENT);
}

namespace gplay
{
    void RTQuitGameCallback()
    {
        LOGD("RTQuitGameCallback()");

        cocos2d::Director::getInstance()->end();

        //reset static variables
        s_preloadSuccessJSTarget = JSVAL_VOID;
        s_preloadSuccessJSFunc = JSVAL_VOID;
        s_asyncJSCallbackFunc = JSVAL_VOID;
        s_asyncJSCallbackTarget = JSVAL_VOID;
        s_preloadResponseJSTarget = JSVAL_VOID;
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
