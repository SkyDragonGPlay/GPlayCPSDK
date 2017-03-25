#include <string>
#include <jni.h>
#include <android/log.h>
#include "gplay.h"
#include "JniHelper.h"

extern "C"{
#include "gplayhook.h"
}


#define LOG_TAG "unity_bridge"
#define LOGD(...)  __android_log_print(ANDROID_LOG_DEBUG, LOG_TAG, __VA_ARGS__)
#define LOGE(...)  __android_log_print(ANDROID_LOG_ERROR, LOG_TAG, __VA_ARGS__)
#define LOG_DIVIDE_ERROR(...)  __android_log_print(ANDROID_LOG_ERROR, "gplay_divide_res", __VA_ARGS__)


// JNIEXPORT jint JNI_OnLoad(JavaVM* jvm, void* reserved)
// {
//     JNIEnv* env = NULL;
//     jint result = -1; 

//     if (jvm->GetEnv((void**) &env, JNI_VERSION_1_6) != JNI_OK) {
//         return result;
//     }

//     gplay::framework::JniHelper::setJavaVM(jvm);

//     // 返回jni的版本
//     return JNI_VERSION_1_6;
// }



namespace gplay
{
    typedef void(*GPlayPreloadResponseCallback)(int,int,const std::string&,float,float);
    static GPlayPreloadResponseCallback s_preloadResponseCallback = NULL;

    char* MakeStringCopy (char* destStr, const char* string)
    {
        if(destStr != NULL)
            free(destStr);
        
        if (string == NULL)
        {
            destStr = (char*)malloc(1);
            destStr[0] = '\0';
        }
        else
        {
            destStr = (char*)malloc(strlen(string) + 1);
            strcpy(destStr, string);
        }
        return destStr;
    }

    const char* GPLAY_GAMEOBJECT_NAME = "GPlaySDK";
    void UnitySendMessage(const char* methodName, const char* message)
    {
        GplayUnitySendMessage(GPLAY_GAMEOBJECT_NAME, methodName, message);
    }

    //////////////////////////////////////////////////////////////////////
    void RTRuntimeInit()
    {
        LOGD("RTRuntimeInit");
        bool isJniFuncCallSuccess;
        common::callExtensionSyncAPI("UseNewPreloadResponseMode", isJniFuncCallSuccess, "", 0, 0);
        hookDlopen();
    }
    
    void RTQuitGame()
    {
        LOGD("RTQuitGame");
    }

    void RTPreloadResponse(int resultCode, int errorCode, const std::string& groupName, float percent, float downloadSpeed)
    {
        LOGD("RTPreloadResponse(int resultCode, int errorCode, const std::string& groupName, float percent, float downloadSpeed)");
        if(s_preloadResponseCallback != NULL)
        {
            s_preloadResponseCallback(resultCode, errorCode, groupName, percent, downloadSpeed);
        }
    }

    void RTPreloadResponse(const char* responseJson, bool isDone, long ext)
    {
        LOGD("RTPreloadResponse(const char* responseJson, bool isDone, long ext)");
    }
    
    bool RTDipspatchEvent(int keyCode, bool isPressed)
    {
        LOGD("RTDipspatchEvent");
        return true;
    }

    void RTSetDefaultResourceRootPath(const char* resRootPath)
    {
        LOGD("RTSetDefaultResourceRootPath");
        SetDefaultResourceRootPath(resRootPath);
    }

    void RTAddSearchPath(const char* resSearchPath)
    {
        LOGD("RTAddSearchPath :  %s", resSearchPath);
        AddResourceSearchPath(resSearchPath);
    }
    //////////////////////////////////////////////////////////////////////

    void preloadGroupsWithCalback(const std::string& jsonGroups, int ext, GPlayPreloadResponseCallback callback)
    {
        LOGD("preloadGroupsWithCalback");
        s_preloadResponseCallback = callback;
        gplay::common::preloadGroups(jsonGroups, ext);
    }

    extern "C"
    {
        JNIEXPORT void JNICALL Java_com_skydragon_gplay_runtime_bridge_CocosRuntimeBridge_nativeCaptureScreen(JNIEnv *env, jclass obj, jstring jstrPictureSaveFile, jint jstrQuality)
        {
            const char *strPictureSaveFile = env->GetStringUTFChars(jstrPictureSaveFile, 0);
            int quality = jstrQuality;

            char captureScreenInfo[256]; 
            sprintf(captureScreenInfo, "{\"pictureSaveFile\":%s, \"quality\":%d}", strPictureSaveFile, quality);

            LOGD("captureScreenInfo:  %s", captureScreenInfo);
            UnitySendMessage("CaptureScreen", captureScreenInfo);

            env->ReleaseStringUTFChars(jstrPictureSaveFile, strPictureSaveFile);
        }
    }
}
