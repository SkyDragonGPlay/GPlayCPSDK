#include "gplay_cpp.h"
#include <string>
#include <map>
#include <android/log.h>
#include "gplay.h"

#include "platform/android/jni/JniHelper.h"
#include "DivideResourceHelper.h"

#define LOG_TAG "gplay_cpp"
#define LOGD(...)  __android_log_print(ANDROID_LOG_DEBUG, LOG_TAG, __VA_ARGS__)
#define LOGE(...)  __android_log_print(ANDROID_LOG_ERROR, LOG_TAG, __VA_ARGS__)
#define LOG_DIVIDE_ERROR(...)  __android_log_print(ANDROID_LOG_ERROR, "gplay_divide_res", __VA_ARGS__)

#if COCOS2D_VERSION >= 0x00030000
static const char* s_CPSDKVersion = "GPLAY_CP_SDK_INFO:{\"SDK_VERSION\":\"1.5.2\",\"ENGINE\":\"COCOS_CPP\",\"ENGINE_BASE_VERSION\":\"V3\",\"SDK_VERSION_NAME\":\"1.5.2 release 2\"}";
#else
static const char* s_CPSDKVersion = "GPLAY_CP_SDK_INFO:{\"SDK_VERSION\":\"1.5.2\",\"ENGINE\":\"COCOS_CPP\",\"ENGINE_BASE_VERSION\":\"V2\",\"SDK_VERSION_NAME\":\"1.5.2 release 2\"}";
#endif

using namespace std;
using namespace gplay::common;

namespace gplay {

    extern void RTCaptureScreenJNI(const std::string& params);

    void RTQuitGameCallback()
    {
#if COCOS2D_VERSION >= 0x00030000
        auto director = cocos2d::Director::getInstance();
#else
        cocos2d::CCDirector* director= cocos2d::CCDirector::sharedDirector();
#endif
        director->end();
    }
    
    static string shareParams2String(const GplayShareParams& params)
    {
        string jsonShareParams("{\"url\":\"");
        jsonShareParams += params.pageUrl;
        jsonShareParams += "\",\"title\":\"";
        jsonShareParams += params.title;
        jsonShareParams += "\",\"text\":\"";
        jsonShareParams += params.content;
        jsonShareParams += "\",\"img_url\":\"";
        jsonShareParams += params.imgUrl;
        jsonShareParams += "\",\"imgTitle\":\"";
        jsonShareParams += params.imgTitle;
        jsonShareParams += "\"}";
        return jsonShareParams;
    }

    static string payParams2String(const GplayPayParams& params)
    {
        char convert_buffer[16];

        string jsonPayParams("{\"product_id\":\"");
        jsonPayParams += params.productId;
        jsonPayParams += "\",\"product_name\":\"";
        jsonPayParams += params.productName;

        jsonPayParams += "\",\"product_price\":\"";
        sprintf(convert_buffer, "%d", params.productPrice);
        jsonPayParams += convert_buffer;

        jsonPayParams += "\",\"product_count\":\"";
        sprintf(convert_buffer, "%d", params.productCount);
        jsonPayParams += convert_buffer;

        jsonPayParams += "\",\"product_desc\":\"";
        jsonPayParams += params.productDescription;
        jsonPayParams += "\",\"game_user_id\":\"";
        jsonPayParams += params.gameUserId;
        jsonPayParams += "\",\"game_user_name\":\"";
        jsonPayParams += params.gameUserName;
        jsonPayParams += "\",\"server_id\":\"";
        jsonPayParams += params.serverId;
        jsonPayParams += "\",\"server_name\":\"";
        jsonPayParams += params.serverName;
        jsonPayParams += "\",\"private_data\":\"";
        jsonPayParams += params.extraData;
        jsonPayParams += "\"}";
        return jsonPayParams;
     }

    static int s_callbackid = 0;
    static std::map<int, GplayResultCallback> s_callbackMap;

     static void actionResultCallback(int result, const char* jsonResult, int callbackID)
     {
         std::map<int, GplayResultCallback>::iterator iter = s_callbackMap.find(callbackID);
         if(iter != s_callbackMap.end())
         {
             iter->second(result, jsonResult);
             s_callbackMap.erase(iter);
         }
     }

    void initSDK(const std::string& appKey, const std::string& appSecret, const std::string& privateKey)
    {
        if (common::isInGplayEnv())
            gplay::common::initSDK(appKey, appSecret, privateKey);
    }

    NetworkType::GplayNetWorkType getNetworkType()
    {
        int status = gplay::common::getNetworkType();
        return (NetworkType::GplayNetWorkType)status;
    }

    extern void RTSetPreloadSuccessCallback(void(*)(int,const char*));
    extern void RTSetPreloadResponseCallback(void(*)(int,int,const std::string&,float,float));

#if COCOS2D_VERSION >= 0x00030000
    static GplayResultCallback s_preloadSuccessCallback = NULL;
    static GPreloadResponseCallback s_preloadResponseCallback = NULL;

    static void PreloadSuccessCallbackConverter(int retCode, const char* jsonResult)
    {
        s_preloadSuccessCallback(retCode, jsonResult);
    }

    static void PreloadResponseCallbackConverter(int resultCode, int errorCode, 
        const std::string& bundleName, float percent, float speed)
    {
        s_preloadResponseCallback(resultCode, errorCode, bundleName, percent, speed);
    }
#endif

    static int s_currPreloadExt = 0;

    void setPreloadResponseCallback(GPreloadResponseCallback callback)
    {
        if (!common::isInGplayEnv())
            return;

#if COCOS2D_VERSION >= 0x00030000
            s_preloadResponseCallback = callback;
            if(callback)
                RTSetPreloadResponseCallback(PreloadResponseCallbackConverter);
            else
                RTSetPreloadResponseCallback(NULL);
#else
            RTSetPreloadResponseCallback(callback);
#endif
            if (callback)
                s_currPreloadExt = 1;
            else
                s_currPreloadExt = 0;
    }

    void preloadResourceBundles(const std::vector<std::string>& bundleNames, GPreloadSuccessCallback callback)
    {
        if (!common::isInGplayEnv())
        {
            callback(0,"");
            return;
        }

        if (!bundleNames.empty())
        {
            if(GPlayFileUtils::isDivideResourceMode())
            {
                if(bundleNames.size() > 1)
                {
                    LOG_DIVIDE_ERROR("error-preload-multiple");
                    CCAssert(false, "gplay::preloadResourceBundles not support preload multiple bundle in automatic mode");
                }
                common::noticePreloadBegin(bundleNames.at(0));
                callback(0,"");
                return;
            }

#if COCOS2D_VERSION >= 0x00030000
            s_preloadSuccessCallback = callback;
            RTSetPreloadSuccessCallback(PreloadSuccessCallbackConverter);
#else
            RTSetPreloadSuccessCallback(callback);
#endif
            std::string jsonBundleNames("{\"scenes\":[\"");
            jsonBundleNames += bundleNames[0];
            jsonBundleNames += "\"";

            int groupCount = bundleNames.size();
            for(int index = 1; index < groupCount; ++index){
                jsonBundleNames += ",\"";
                jsonBundleNames += bundleNames[index];
                jsonBundleNames += "\"";
            }
            jsonBundleNames += "]}";
            gplay::common::preloadGroups(jsonBundleNames, s_currPreloadExt);
        }
        else
            LOGE("preloadResourceBundles error:bundleNames is empty");
    }

    void preloadResourceBundle(const std::string& bundleName, GPreloadSuccessCallback callback)
    {
        if (!common::isInGplayEnv())
        {
            callback(0,"");
            return;
        }

        if (!bundleName.empty())
        {
            if(GPlayFileUtils::isDivideResourceMode())
            {
                common::noticePreloadBegin(bundleName);
                callback(0,"");
                return;
            }

#if COCOS2D_VERSION >= 0x00030000
            s_preloadSuccessCallback = callback;
            RTSetPreloadSuccessCallback(PreloadSuccessCallbackConverter);
#else
            RTSetPreloadSuccessCallback(callback);
#endif
            char tmp[200];
            sprintf(tmp,"{\"scenes\":[\"%s\"]}", bundleName.c_str());
            gplay::common::preloadGroups(tmp, s_currPreloadExt);
        }
        else
            LOGE("preloadResourceBundle error:bundleName is empty");
    }

    void backFromResourceBundle(const std::string& bundleName)
    {
        if(GPlayFileUtils::isDivideResourceMode())
        {
            common::noticePreloadEnd(bundleName);
        }
    }

    bool isLogined()
    {
        if (!common::isInGplayEnv())
            return false;

        return gplay::common::isLogined();
    }

    const std::string& getUserID()
    {
        return gplay::common::getUserID();
    }

    const std::string& getOrderId() {
        return gplay::common::getOrderId();
    }

    const std::string& getChannelID() {
        return gplay::common::getChannelID();
    }

    void login(GplayResultCallback callback)
    {
        s_callbackMap.insert(std::make_pair(s_callbackid,callback));
        gplay::common::login(s_callbackid++, actionResultCallback);
    }

    void logout(GplayResultCallback callback)
    {
        s_callbackMap.insert(std::make_pair(s_callbackid,callback));
        gplay::common::logout(s_callbackid++, actionResultCallback);
    }

    void quitGame() {
        if (common::isInGplayEnv())
            gplay::common::quitGame();
        else
            RTQuitGameCallback();
        s_currPreloadExt = 0;
    }

    void share(const GplayShareParams& params, GplayResultCallback callback)
    {
        string shareInfo = shareParams2String(params);
        s_callbackMap.insert(std::make_pair(s_callbackid,callback));
        gplay::common::share(s_callbackid++, shareInfo.c_str(), actionResultCallback);
    }

    void pay(const GplayPayParams& params, GplayResultCallback callback)
    {
        string payInfo = payParams2String(params);
        s_callbackMap.insert(std::make_pair(s_callbackid,callback));
        gplay::common::pay(s_callbackid++, payInfo.c_str(), actionResultCallback);
    }

    void createShortcut(GplayResultCallback callback)
    {
        s_callbackMap.insert(std::make_pair(s_callbackid,callback));
        gplay::common::createShortcut(s_callbackid++, actionResultCallback);
    }

    bool isFunctionSupported(const std::string& funcName)
    {
        if (!common::isInGplayEnv())
            return false;

        return gplay::common::isFunctionSupported(funcName);
    }

    std::string callSyncFunc(const std::string& funcName, const std::string& params)
    {
        if (!common::isInGplayEnv())
            return "";

        return gplay::common::callSyncFunc(funcName, params);
    }

    static GplayResultCallback s_captureScreenCallback = NULL;

    void callAsyncFunc(const std::string& funcName, const std::string& params, GplayResultCallback callback)
    {
        if (!common::isInGplayEnv())
            return;
        
        if (funcName.compare("CAPTURE_SCREEN") == 0)
        {
            s_captureScreenCallback = callback;
            RTCaptureScreenJNI(params);
        }
        else
        {
            s_callbackMap.insert(std::make_pair(s_callbackid, callback));
            gplay::common::callAsyncFunc(s_callbackid++, funcName, params, actionResultCallback);
        }
    }

    void onPreGameStart()
    {
        LOGD("%s", s_CPSDKVersion);
    }

    bool isInGplayEnv() {
        return common::isInGplayEnv();
    }

    void preloadGroup(const std::string& group, GPreloadSuccessCallback callback)
    {
        preloadResourceBundle(group, callback);
    }
    
    void preloadGroups(const std::vector<std::string>& groups, GPreloadSuccessCallback callback)
    {
        preloadResourceBundles(groups, callback);
    }

    void backFromGroup(const std::string& group)
    {
        backFromResourceBundle(group);
    }
}

extern "C" {
    void Java_org_cocos2dx_lib_Cocos2dxHelper_nativeOnCaptureScreenResult(JNIEnv* env, jobject thiz, jint resultCode)
    {
        if (resultCode == 1)
            gplay::s_captureScreenCallback(gplay::CAPTURE_SCREEN_SUCCESS, "");
        else
            gplay::s_captureScreenCallback(gplay::CAPTURE_SCREEN_FAILED, "");
    }
}
