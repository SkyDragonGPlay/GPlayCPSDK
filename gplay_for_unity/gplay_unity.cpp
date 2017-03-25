#include "gplay_unity.h"
#include <android/log.h>
#include <jni.h>

#define LOG_TAG "gplay_unity"
#define LOGD(...)  ;//__android_log_print(ANDROID_LOG_DEBUG, LOG_TAG, __VA_ARGS__)
#define LOGE(...)  ;//__android_log_print(ANDROID_LOG_ERROR, LOG_TAG, __VA_ARGS__)
#define LOG_DIVIDE_ERROR(...)  __android_log_print(ANDROID_LOG_ERROR, "gplay_divide_res", __VA_ARGS__)

using namespace std;

namespace gplay
{
    const char* GPLAY_PRELOAD_RESPONSE_METHOD = "PreloadResponse";

    extern char* MakeStringCopy (char* destStr, const char* string);
    extern void UnitySendMessage(const char* methodName, const char* message);
    extern void preloadGroupsWithCalback(const std::string& jsonGroups, int ext, GPlayPreloadResponseCallback callback);


    static int s_currPreloadExt = 0;
    
     static void actionResultCallback(int resultCode, const char* jsonResult, int callbackID)
     {
        int resultLen = strlen(jsonResult) + 100;
        char *callbackJson = (char *)malloc(resultLen);
        sprintf(callbackJson, "{\"callbackID\":%d, \"resultCode\":%d, \"resultJson\":%s}", callbackID, resultCode, jsonResult);
        UnitySendMessage("ActionCallback", callbackJson);
        free(callbackJson);
     }

    bool isInGplayEnv()
    {
        common::isInGplayEnv();
        return false;
    }

    void initSDK(const char* appKey, const char* appSecret, const char* privateKey)
    {
        if (common::isInGplayEnv())
            common::initSDK(appKey, appSecret, privateKey);
    }

    char* strChannelID = NULL;
    const char* getChannelID()
    {
        return MakeStringCopy(strChannelID, common::getChannelID().c_str());
    }

    common::GplayNetWorkStatus getNetworkType()
    {
        return common::getNetworkType();
    }

    bool isLogined()
    {
        if (!common::isInGplayEnv())
            return false;

        return gplay::common::isLogined();
    }

    /**
     * 获取用户 ID
     */
    char* strUserID = NULL;
    const char* getUserID()
    {
        return MakeStringCopy(strUserID, common::getUserID().c_str());
    }

    /**
     * 登录
     */
    void login(int callbackid)
    {
        common::login(callbackid, actionResultCallback);
    }

    /**
     * 退出游戏
     */
    void quitGame()
    {
        if (common::isInGplayEnv())
            gplay::common::quitGame();
        // else
        //     RTQuitGameCallback();
        // s_currPreloadExt = 0;
    }

    /**
     * 分享游戏
     */
    void share(int callbackid, const char* shareInfoJson)
    {
        gplay::common::share(callbackid, shareInfoJson, actionResultCallback);
    }

    /**
     * 支付
     */
    void pay(int callbackid, const char* payInfoJson)
    {
        gplay::common::pay(callbackid, payInfoJson, actionResultCallback);
    }

    /**
     * 创建桌面快捷图标
     */
    void createShortcut(int callbackid)
    {
        gplay::common::createShortcut(callbackid, actionResultCallback);
    }

    /**
     * 判断是否支持某个方法
     */
    bool isFunctionSupported(const char* funcName)
    {
        if (!common::isInGplayEnv())
            return false;

        return gplay::common::isFunctionSupported(funcName);
    }

    void preloadResponse(int resultCode, int errorCode, const std::string& groupName, float percent, float downloadSpeed)
    {
        char responseInfoJson[200];
        sprintf(responseInfoJson, "{\"resultCode\":%d, \"errorCode\":%d, \"groupName\":%s, \"percent\":%f, \"downloadSpeed\":%f}",
                    resultCode, errorCode, groupName.c_str(), percent, downloadSpeed);
        UnitySendMessage(GPLAY_PRELOAD_RESPONSE_METHOD, responseInfoJson);
    }

    void preloadGroups(const char* jsonGroups, int ext)
    {
        if (!common::isInGplayEnv())
        {
            return;
        }

        preloadGroupsWithCalback(jsonGroups, ext, preloadResponse);
    }


    /**
     * 调用同步扩展接口
     */
    char* callSyncFuncResult = NULL;
    const char* callSyncFunc(const char* funcName, const char* params)
    {
        if (!common::isInGplayEnv())
            return MakeStringCopy(callSyncFuncResult, "");

        std::string result = gplay::common::callSyncFunc(funcName, params);
        return MakeStringCopy(callSyncFuncResult, result.c_str());
    }

    // static GplayResultCallback s_captureScreenCallback = NULL;
    // extern void RTCaptureScreenJNI(const std::string& params);

    /**
     * 调用异步扩展接口
     */
    void callAsyncFunc(const char* funcName, const char* params, int callbackid)
    {
        if (!common::isInGplayEnv())
            return;

        gplay::common::callAsyncFunc(callbackid, funcName, params, actionResultCallback);
    }
}








