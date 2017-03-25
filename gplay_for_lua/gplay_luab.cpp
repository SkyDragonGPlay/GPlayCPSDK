#include "gplay_luab.h"
#include "cocos2d.h"

#include <map>
#include <string>

#include "CCLuaEngine.h"
#include "tolua_fix.h"
#include "gplay.h"
#include "cocos_bridge/DivideResourceHelper.h"

#define  LOG_TAG    "luab_gplay_runtime"
#define  LOGD(...)  __android_log_print(ANDROID_LOG_DEBUG, LOG_TAG, __VA_ARGS__)
#define  LOGE(...)  __android_log_print(ANDROID_LOG_ERROR, LOG_TAG, __VA_ARGS__)

#if COCOS2D_VERSION >= 0x00030000
static const char* s_CPSDKVersion = "GPLAY_CP_SDK_INFO:{\"SDK_VERSION\":\"1.5.2\",\"ENGINE\":\"COCOS_LUA\",\"ENGINE_BASE_VERSION\":\"V3\",\"SDK_VERSION_NAME\":\"1.5.2 release 2\"}";
#else
static const char* s_CPSDKVersion = "GPLAY_CP_SDK_INFO:{\"SDK_VERSION\":\"1.5.2\",\"ENGINE\":\"COCOS_LUA\",\"ENGINE_BASE_VERSION\":\"V2\",\"SDK_VERSION_NAME\":\"1.5.2 release 2\"}";
#endif

using namespace std;
using namespace gplay;
USING_NS_CC;

#if COCOS2D_VERSION >= 0x00030000
#define GET_LUA_STACK LuaStack* stack = LuaEngine::getInstance()->getLuaStack();
#else
#define GET_LUA_STACK CCLuaStack* stack = CCLuaEngine::defaultEngine()->getLuaStack();
#endif

namespace gplay {
    extern void RTSetPreloadSuccessCallback(void(*)(int,const char*));
    extern void RTSetPreloadResponseCallback(void(*)(int,int,const std::string&,float,float));
    extern void RTCaptureScreenJNI(const std::string& params);
}

// callback and env
static lua_State* s_global_L = NULL;
static LUA_FUNCTION s_asyncCallbackHandler = -1;
static LUA_FUNCTION s_preloadSuccessLuaHandler = -1;
static LUA_FUNCTION s_preloadResponseLuaHandler = -1;

static int s_currPreloadExt = 0;

static void resetRuntimeStaticVariables()
{
    LOGD("resetRuntimeStaticVariables ...");
    s_global_L = NULL;
    s_asyncCallbackHandler = -1;
    s_preloadSuccessLuaHandler = -1;
    s_preloadResponseLuaHandler = -1;
}

static void RTPreloadSuccessCallback(int result, const char* jsonResult)
{
   // 调用 lua 层的 successCallback
    GET_LUA_STACK

    lua_State* tolua_S = stack->getLuaState();

    tolua_pushstring(tolua_S, jsonResult);
    
    stack->executeFunctionByHandler(s_preloadSuccessLuaHandler, 2);
    stack->clean();

    s_preloadSuccessLuaHandler = -1;
}

static void PreloadResponseCallbackConverter(int resultCode, int errorCode, 
        const std::string& groupName, float percent, float speed)
{
    GET_LUA_STACK
    lua_State* tolua_S = stack->getLuaState();

    tolua_pushnumber(tolua_S, (lua_Number)resultCode);
    tolua_pushnumber(tolua_S, (lua_Number)errorCode);
    tolua_pushstring(tolua_S, groupName.c_str());
    tolua_pushnumber(tolua_S, (lua_Number)percent);
    tolua_pushnumber(tolua_S, (lua_Number)speed);
    
#ifdef ON_VERSION_2
        stack->executeFunctionByHandler(s_preloadResponseLuaHandler, 5);
#else
        stack->executeFunctionByHandler(s_preloadResponseLuaHandler, 6);
#endif
    
    stack->clean();
}

static int tolua_gplay_setPreloadResponseCallback(lua_State* tolua_S)
{
    int argc = lua_gettop(tolua_S) - 1;

    if (argc == 0)
    {
        s_preloadResponseLuaHandler = -1;
        RTSetPreloadResponseCallback(NULL);
        s_currPreloadExt = 0;
    }
    else if(argc == 1)
    {
        LUA_FUNCTION handler = (toluafix_ref_function(tolua_S,2,0));
        s_preloadResponseLuaHandler = handler;

        RTSetPreloadResponseCallback(PreloadResponseCallbackConverter);
        s_currPreloadExt = 1;
    }

    return 0;
}

static void actionResultCallback(int result, const char* jsonResult, int callbackID)
{
    CCLOG("on async result: %d, msg: %s. callbackID: %d", result, jsonResult, callbackID);

    GET_LUA_STACK
    lua_State* tolua_S = stack->getLuaState();
    tolua_pushnumber(tolua_S, result);
    tolua_pushstring(tolua_S, jsonResult);
    tolua_pushnumber(tolua_S, callbackID);

#ifdef ON_VERSION_2
        stack->executeFunctionByHandler(s_asyncCallbackHandler, 3);
#else
        stack->executeFunctionByHandler(s_asyncCallbackHandler, 4);
#endif
        stack->clean();
}

/* function to register type */
static void tolua_register_types (lua_State* tolua_S) { 
    tolua_usertype(tolua_S,"luab_gplay"); 
}

#define BINDING_RETURN_ERROR(function) \
    char buffer[50]; \
    snprintf(buffer, 45, "error in function:%s", function); \
    tolua_error(tolua_S, buffer, &tolua_err); \
    return 0;

/* method: init of class UnitSDK */
static int tolua_gplay_init(lua_State* tolua_S) {
#ifndef TOLUA_RELEASE
    tolua_Error tolua_err;
    if (!tolua_isusertable(tolua_S,1,"luab_gplay",0,&tolua_err) ||
        !tolua_iscppstring(tolua_S,2,0,&tolua_err) ||
        !tolua_iscppstring(tolua_S,3,0,&tolua_err) ||
        !tolua_iscppstring(tolua_S,4,0,&tolua_err) ||
        !tolua_isnoobj(tolua_S,5,&tolua_err))
        {BINDING_RETURN_ERROR(__func__);}
    else
#endif
    {
        string appKey = ((string)tolua_tocppstring(tolua_S,2,0));
        string appSecret = ((string)tolua_tocppstring(tolua_S,3,0));
        string privateKey = ((string)tolua_tocppstring(tolua_S,4,0));
        gplay::common::initSDK(appKey, appSecret, privateKey);
    }
    return 0;
}

/* method: getChannelId of class UnitSDK */
static int tolua_gplay_getChannelId(lua_State* tolua_S) {
#ifndef TOLUA_RELEASE
    tolua_Error tolua_err;
    if (!tolua_isusertable(tolua_S,1,"luab_gplay",0,&tolua_err) ||
        !tolua_isnoobj(tolua_S,2,&tolua_err))
        {BINDING_RETURN_ERROR(__func__);}
    else
#endif
    {
        const std::string& tolua_ret = gplay::common::getChannelID();
        tolua_pushcppstring(tolua_S, tolua_ret);
    }
    return 1;
}

/* method: isFunctionSupported of class UnitSDK */
static int tolua_gplay_isFunctionSupported(lua_State* tolua_S) {
#ifndef TOLUA_RELEASE
    tolua_Error tolua_err;
    if (!tolua_isusertable(tolua_S,1,"luab_gplay",0,&tolua_err) ||
        !tolua_iscppstring(tolua_S,2,0,&tolua_err) ||
        !tolua_isnoobj(tolua_S,3,&tolua_err))
        {BINDING_RETURN_ERROR(__func__);}
    else
#endif
    {
        string functionName = ((string)tolua_tocppstring(tolua_S,2,0));
        bool tolua_ret = (bool)gplay::common::isFunctionSupported(functionName);
        tolua_pushboolean(tolua_S,(bool)tolua_ret);
    }
    return 1;
}

/* method: getOrderId of class UnitSDK */
static int tolua_gplay_getOrderId(lua_State* tolua_S) {
#ifndef TOLUA_RELEASE
    tolua_Error tolua_err;
    if (!tolua_isusertable(tolua_S,1,"luab_gplay",0,&tolua_err) ||
        !tolua_isnoobj(tolua_S,2,&tolua_err))
        {BINDING_RETURN_ERROR(__func__);}
    else
#endif
    {
        const std::string& tolua_ret = gplay::common::getOrderId();
        tolua_pushcppstring(tolua_S, tolua_ret);
    }
    return 1;
}

/* method: login of class UnitSDK */
static int tolua_gplay_login(lua_State* tolua_S) {
#ifndef TOLUA_RELEASE
    tolua_Error tolua_err;
    if (!tolua_isusertable(tolua_S,1,"luab_gplay",0,&tolua_err) ||
        !tolua_isnumber(tolua_S,2,0,&tolua_err) ||
        !tolua_isnoobj(tolua_S,3,&tolua_err))
        {BINDING_RETURN_ERROR(__func__);}
    else
#endif
    {
        int callbackID = (unsigned)tolua_tonumber(tolua_S, 2, 0);
        gplay::common::login(callbackID, actionResultCallback);
    }
    return 0;
}

/* method: isLogined of class UnitSDK */
static int tolua_gplay_isLogined(lua_State* tolua_S) {
#ifndef TOLUA_RELEASE
    tolua_Error tolua_err;
    if (!tolua_isusertable(tolua_S,1,"luab_gplay",0,&tolua_err) ||
        !tolua_isnoobj(tolua_S,2,&tolua_err))
        {BINDING_RETURN_ERROR(__func__);}
    else
#endif
    {
        bool tolua_ret = gplay::common::isLogined();
        tolua_pushboolean(tolua_S,(bool)tolua_ret);
    }
    return 1;
}

/* method: getUserID of class UnitSDK */
static int tolua_gplay_getUserID(lua_State* tolua_S) {
#ifndef TOLUA_RELEASE
    tolua_Error tolua_err;
    if (!tolua_isusertable(tolua_S,1,"luab_gplay",0,&tolua_err) ||
        !tolua_isnoobj(tolua_S,2,&tolua_err))
        {BINDING_RETURN_ERROR(__func__);}
    else
#endif
    {
        const std::string& tolua_ret = gplay::common::getUserID();
        tolua_pushcppstring(tolua_S, tolua_ret);
    }
    return 1;
}

/* method: setAsyncActionResultListener of luab_gplay */
static int tolua_gplay_setAsyncActionResultListener(lua_State* tolua_S) {
#ifndef TOLUA_RELEASE 
    tolua_Error tolua_err;
    if (!tolua_isusertable(tolua_S,1,"luab_gplay",0,&tolua_err) ||
        !toluafix_isfunction(tolua_S,2,"LUA_FUNCTION",0,&tolua_err))
        {BINDING_RETURN_ERROR(__func__);}
    else
#endif 
    {
        LUA_FUNCTION handler = (toluafix_ref_function(tolua_S,2,0));
        s_asyncCallbackHandler = handler;
        return 0;
    }
}

/* method: share of class UnitSDK */
static int tolua_gplay_share(lua_State* tolua_S) {
    CCLOG("in function share\n");
#ifndef TOLUA_RELEASE 
    tolua_Error tolua_err;
    if (!tolua_isusertable(tolua_S,1,"luab_gplay",0,&tolua_err) ||
        !tolua_isnumber(tolua_S,2,0,&tolua_err) ||
        !tolua_iscppstring(tolua_S,3,0,&tolua_err) ||
        !tolua_isnoobj(tolua_S,4,&tolua_err))
        {BINDING_RETURN_ERROR(__func__);}
    else
#endif
    {
        int callbackID = (unsigned)tolua_tonumber(tolua_S, 2, 0);
        string params = (string)tolua_tocppstring(tolua_S, 3, 0);
        gplay::common::share(callbackID, params, actionResultCallback);
    }
    return 0;
}

/* method: payForProduct of class UnitSDK */
static int tolua_gplay_payForProduct(lua_State* tolua_S) {
#ifndef TOLUA_RELEASE
    tolua_Error tolua_err;
    if (!tolua_isusertable(tolua_S,1,"luab_gplay",0,&tolua_err) ||
        !tolua_isnumber(tolua_S,2,0,&tolua_err) ||
        !tolua_iscppstring(tolua_S,3,0,&tolua_err) ||
        !tolua_isnoobj(tolua_S,4,&tolua_err))
        {BINDING_RETURN_ERROR(__func__);}
    else
#endif
    {
        int callbackID = (unsigned)tolua_tonumber(tolua_S, 2, 0);
        string params = (string)tolua_tocppstring(tolua_S,3,0);
        gplay::common::pay(callbackID, params, actionResultCallback);
    }
    return 0;
}

/* method: createShortcut of class UnitSDK */
static int tolua_gplay_createShortcut(lua_State* tolua_S) {
#ifndef TOLUA_RELEASE
    tolua_Error tolua_err;
    if (!tolua_isusertable(tolua_S,1,"luab_gplay",0,&tolua_err) ||
        !tolua_isnumber(tolua_S,2,0,&tolua_err) ||
        !tolua_isnoobj(tolua_S,3,&tolua_err))
        {BINDING_RETURN_ERROR(__func__);}
    else
#endif
    {
        int callbackID = (unsigned)tolua_tonumber(tolua_S, 2, 0);
        gplay::common::createShortcut(callbackID, actionResultCallback);
    }
    return 0;
}

static int s_captureScreenCallbackID;

// 通用扩展接口
/* method: callAsyncFunc of class UnitSDK */
static int tolua_gplay_callAsyncFunc(lua_State* tolua_S) {
#ifndef TOLUA_RELEASE 
    tolua_Error tolua_err;
    if (!tolua_isusertable(tolua_S,1,"luab_gplay",0,&tolua_err) ||
        !tolua_isnumber(tolua_S,2,0,&tolua_err) ||
        !tolua_iscppstring(tolua_S,3,0,&tolua_err) ||
        !tolua_iscppstring(tolua_S,4,0,&tolua_err))
        {BINDING_RETURN_ERROR(__func__);}
    else
#endif 
    {
        int callbackID = (unsigned)tolua_tonumber(tolua_S, 2, 0);
        string functionName = tolua_tostring(tolua_S, 3, 0);
        string strParams = tolua_tostring(tolua_S, 4, 0);

        CCLOG("callAsyncFunc with params: functionName: %s, callbackID: %d\n", functionName.c_str(), callbackID);
        
        if (functionName.compare("CAPTURE_SCREEN") == 0)
        {
            s_captureScreenCallbackID = callbackID;
            RTCaptureScreenJNI(strParams);
        }
        else
            gplay::common::callAsyncFunc(callbackID, functionName, strParams, actionResultCallback);
    }
    return 0;
}

/* method: callSyncStringFunc of class UnitSDK */
static int tolua_gplay_callSyncFunc(lua_State* tolua_S) {
#ifndef TOLUA_RELEASE 
    tolua_Error tolua_err;
    if (!tolua_isusertable(tolua_S,1,"luab_gplay",0,&tolua_err) ||
        !tolua_iscppstring(tolua_S,2,0,&tolua_err) ||
        !tolua_iscppstring(tolua_S,3,0,&tolua_err))
        {BINDING_RETURN_ERROR(__func__);}
    else
#endif 
    { 
        string functionName = tolua_tostring(tolua_S, 2, 0);
        string strParams = tolua_tostring(tolua_S, 3, 0);

        string strRet = gplay::common::callSyncFunc(functionName, strParams);
        tolua_pushcppstring(tolua_S, strRet);
        return 1;
    }
}

// help class to register
class luab_gplay{
public:
    luab_gplay(){};
};

// ============== Java 层发生事件回调通过绑定层反馈给 LUA 层 ===================
static void nativeCocosRuntimeOnStartGame() {
    LOGD("nativeCocosRuntimeOnStartGame()");
}

namespace gplay {
#if COCOS2D_VERSION >= 0x00030000

//3.0-3.2版本，LuaEngine初始化是会加载部分引擎脚本，脚本中需要调用到Application对象
#if COCOS2D_VERSION <= 0x00030200
class GPlayAppDelegate : private cocos2d::Application
{
public:
    GPlayAppDelegate(){}
    virtual ~GPlayAppDelegate(){}
    virtual bool applicationDidFinishLaunching(){}
    virtual void applicationDidEnterBackground(){}
    virtual void applicationWillEnterForeground(){}
};
GPlayAppDelegate s_GPlayAppDelegate;
#endif

    void RTQuitGameCallback() {
        LOGD("nativeCocosRuntimeOnQuitGame()");
        cocos2d::Director::getInstance()->end();
        
        resetRuntimeStaticVariables();
    }

    void onPreGameStart()
    {
        LOGD("%s", s_CPSDKVersion);
        auto engine = LuaEngine::getInstance();
        ScriptEngineManager::getInstance()->setScriptEngine(engine);
        lua_State* L = engine->getLuaStack()->getLuaState();
        tolua_gplay_open(L);
    }
#else
    void RTQuitGameCallback() {
        LOGD("nativeCocosRuntimeOnQuitGame()");
        cocos2d::CCDirector::sharedDirector()->end();
        
        resetRuntimeStaticVariables();
    }

    void onPreGameStart() {
        LOGD("%s", s_CPSDKVersion);
        CCLuaEngine* engine = CCLuaEngine::defaultEngine();
        lua_State* L = engine->getLuaStack()->getLuaState();
        tolua_gplay_open(L);
    }
#endif
}

// ============== Binding lua method to invoke C++ method ============================

/* function to release collected object via destructor */
#ifdef __cplusplus

static int tolua_collect_luab_gplay (lua_State* tolua_S) {
    luab_gplay* self = (luab_gplay*)tolua_tousertype(tolua_S,1,0);
    Mtolua_delete(self);
    return 0;
}

#endif // ifdef __cplusplus

// LUA_API(internal) luab_gplay.isInGplayEnv()
static int tolua_gplay_isInGplayEnv(lua_State* tolua_S) {
#ifndef TOLUA_RELEASE 
    LOGD("tolua_gplay_isInGplayEnv");
    tolua_Error tolua_err;
    if (!tolua_isusertable(tolua_S,1,"luab_gplay",0,&tolua_err) ||
        !tolua_isnoobj(tolua_S,2,&tolua_err))
        {BINDING_RETURN_ERROR(__func__);}
    else
#endif
    {
        tolua_pushboolean(tolua_S, common::isInGplayEnv());
    }
    return 1;
}

// LUA_API(internal) luab_gplay.isDivideResourceMode()
static int tolua_gplay_isDivideResourceMode(lua_State* tolua_S) {
#ifndef TOLUA_RELEASE 
    LOGD("tolua_gplay_isDivideResourceMode");
    tolua_Error tolua_err;
    if (!tolua_isusertable(tolua_S,1,"luab_gplay",0,&tolua_err) ||
        !tolua_isnoobj(tolua_S,2,&tolua_err))
        {BINDING_RETURN_ERROR(__func__);}
    else
#endif
    {
        tolua_pushboolean(tolua_S, GPlayFileUtils::isDivideResourceMode());
    }
    return 1;
}

// LUA_API: luab_gplay:tolua_gplay_preloadGroups(scene, successCallback)
static int tolua_gplay_preloadGroups(lua_State* tolua_S) {
#ifndef TOLUA_RELEASE 
    LOGD("tolua_gplay_preloadGroups");
    tolua_Error tolua_err;
    if (!tolua_isusertable(tolua_S,1,"luab_gplay",0,&tolua_err) ||
        !tolua_iscppstring(tolua_S,2,0,&tolua_err) ||
        !toluafix_isfunction(tolua_S,3,"LUA_FUNCTION",0,&tolua_err) ||
        !tolua_isnoobj(tolua_S,4,&tolua_err))
        {BINDING_RETURN_ERROR(__func__);}
    else
#endif 
    {
        s_preloadSuccessLuaHandler = toluafix_ref_function(tolua_S,3,0);
        RTSetPreloadSuccessCallback(RTPreloadSuccessCallback);
        string jsonGroups = (string)tolua_tocppstring(tolua_S,2,0); 
        gplay::common::preloadGroups(jsonGroups, s_currPreloadExt);
    }
    return 0;
}

#define LOG_DIVIDE_ERROR(...)  __android_log_print(ANDROID_LOG_ERROR, "gplay_divide_res", __VA_ARGS__)

// LUA_API: luab_gplay:noticePreloadMultipleGroup()
static int tolua_gplay_noticePreloadMultipleGroup(lua_State* tolua_S)
{
    LOG_DIVIDE_ERROR("error-preload-multiple");

    tolua_Error tolua_err;
    tolua_error(tolua_S, "gplay:preloadGroups not support preload multiple group in automatic mode", &tolua_err);
    
    return 0;
}

// LUA_API: luab_gplay:noticePreloadBegin()
static int tolua_gplay_noticePreloadBegin(lua_State* tolua_S)
{
    string sceneName = (string)tolua_tocppstring(tolua_S, 2, 0);
    common::noticePreloadBegin(sceneName);
    return 0;
}

// LUA_API: luab_gplay:noticePreloadEnd()
static int tolua_gplay_noticePreloadEnd(lua_State* tolua_S)
{
    string sceneName = (string)tolua_tocppstring(tolua_S, 2, 0);
    common::noticePreloadEnd(sceneName);
    return 0;
}

// LUA_API: luab_gplay:getNetworkType()
static int tolua_gplay_getNetworkType(lua_State* tolua_S) {
#ifndef TOLUA_RELEASE 
    tolua_Error tolua_err;
    if (!tolua_isusertable(tolua_S,1,"luab_gplay",0,&tolua_err) ||
        !tolua_isnoobj(tolua_S,2,&tolua_err))
        {BINDING_RETURN_ERROR(__func__);}
    else
#endif
    {
        int status = gplay::common::getNetworkType();
        tolua_pushnumber(tolua_S,(lua_Number)status);
    }
    return 1;
}

// LUA_API: luab_gplay:quitGame()
static int tolua_gplay_quitGame(lua_State* tolua_S) {
#ifndef TOLUA_RELEASE 
    tolua_Error tolua_err;
    if (!tolua_isusertable(tolua_S,1,"luab_gplay",0,&tolua_err) ||
        !tolua_isnoobj(tolua_S,2,&tolua_err))
        {BINDING_RETURN_ERROR(__func__);}
    else
#endif 
    {
        s_global_L = tolua_S;
        gplay::common::quitGame();
    }
    return 0;
}

/* Open function */
TOLUA_API int tolua_gplay_open(lua_State* tolua_S) {
    tolua_open(tolua_S);
    tolua_register_types(tolua_S);
    tolua_module(tolua_S,NULL,0);
    tolua_beginmodule(tolua_S,NULL);
    
    #ifdef __cplusplus
        tolua_cclass(tolua_S,"luab_gplay","luab_gplay","",tolua_collect_luab_gplay);
    #else
        tolua_cclass(tolua_S,"luab_gplay","luab_gplay","",NULL);
    #endif
        tolua_beginmodule(tolua_S,"luab_gplay");
            tolua_function(tolua_S,"init",tolua_gplay_init);
            tolua_function(tolua_S,"getChannelId",tolua_gplay_getChannelId);
            tolua_function(tolua_S,"getOrderId",tolua_gplay_getOrderId);
            tolua_function(tolua_S,"getUserID",tolua_gplay_getUserID);
            tolua_function(tolua_S,"isSupportingFunc",tolua_gplay_isFunctionSupported);
            tolua_function(tolua_S,"login",tolua_gplay_login);
            tolua_function(tolua_S,"isLogined",tolua_gplay_isLogined);
            tolua_function(tolua_S,"preloadGroups",tolua_gplay_preloadGroups);
            tolua_function(tolua_S,"noticePreloadMultipleGroup",tolua_gplay_noticePreloadMultipleGroup);
            tolua_function(tolua_S,"noticePreloadBegin",tolua_gplay_noticePreloadBegin);
            tolua_function(tolua_S,"noticePreloadEnd",tolua_gplay_noticePreloadEnd);
            tolua_function(tolua_S,"getNetworkType",tolua_gplay_getNetworkType);
            tolua_function(tolua_S,"quitGame",tolua_gplay_quitGame);
            tolua_function(tolua_S,"isInGplayEnv", tolua_gplay_isInGplayEnv);
            tolua_function(tolua_S,"isDivideResourceMode", tolua_gplay_isDivideResourceMode);
            tolua_function(tolua_S, "setAsyncActionResultListener", tolua_gplay_setAsyncActionResultListener);
            tolua_function(tolua_S, "share", tolua_gplay_share);
            tolua_function(tolua_S, "payForProduct", tolua_gplay_payForProduct);
            tolua_function(tolua_S, "createShortcut", tolua_gplay_createShortcut);
            tolua_function(tolua_S, "callAsyncFunc", tolua_gplay_callAsyncFunc);
            tolua_function(tolua_S, "callSyncStringFunc", tolua_gplay_callSyncFunc);
            tolua_function(tolua_S, "setPreloadResponseCallback", tolua_gplay_setPreloadResponseCallback);
            
        tolua_endmodule(tolua_S);

    tolua_endmodule(tolua_S);

    return 1;
}

#include "platform/android/jni/JniHelper.h"

extern "C" {
    void Java_org_cocos2dx_lib_Cocos2dxHelper_nativeOnCaptureScreenResult(JNIEnv* env, jobject thiz, jint resultCode)
    {
        if (resultCode == 1)
            actionResultCallback(gplay::common::CAPTURE_SCREEN_SUCCESS, "", s_captureScreenCallbackID);
        else
            actionResultCallback(gplay::common::CAPTURE_SCREEN_FAILED, "", s_captureScreenCallbackID);
    }
}

//#endif
