#include "cocos2d.h"
#include <string>
#include <map>
#include <android/log.h>
#include "platform/android/jni/JniHelper.h"
#include "DivideResourceHelper.h"

#include "gplay.h"

#define  LOG_TAG    "cocos_bridge"
#define  LOGD(...)  __android_log_print(ANDROID_LOG_DEBUG, LOG_TAG, __VA_ARGS__)
#define  LOGE(...)  __android_log_print(ANDROID_LOG_ERROR, LOG_TAG, __VA_ARGS__)

#define KEYCODE_BACK 0x04
#define KEYCODE_MENU 0x52

using namespace cocos2d;

extern "C"
{
#if COCOS2D_VERSION > 0x00030200
    extern jintArray Java_org_cocos2dx_lib_Cocos2dxActivity_getGLContextAttrs(JNIEnv*  env, jobject thiz);
#endif
}

namespace gplay {

    extern void RTQuitGameCallback();
    extern void onPreGameStart();

    typedef void(*GPlayPreloadSuccessCallback)(int,const char*);
    static GPlayPreloadSuccessCallback s_preloadSuccessCallback;

    typedef void(*GPlayPreloadResponseCallback)(int,int,const std::string&,float,float);
    static GPlayPreloadResponseCallback s_preloadResponseCallback = NULL;

#if COCOS2D_VERSION >= 0x00030000
// cocos2dx 3.x ================================================================
#define KEYCODE_DPAD_UP 0x13
#define KEYCODE_DPAD_DOWN 0x14
#define KEYCODE_DPAD_LEFT 0x15
#define KEYCODE_DPAD_RIGHT 0x16
#define KEYCODE_ENTER 0x42
#define KEYCODE_PLAY  0x7e
#define KEYCODE_DPAD_CENTER  0x17

    static std::map<int, cocos2d::EventKeyboard::KeyCode> g_keyCodeMap =
    {
        { KEYCODE_BACK , cocos2d::EventKeyboard::KeyCode::KEY_ESCAPE},
        { KEYCODE_MENU , cocos2d::EventKeyboard::KeyCode::KEY_MENU},
#if COCOS2D_VERSION >= 0x00030100
        { KEYCODE_DPAD_UP  , cocos2d::EventKeyboard::KeyCode::KEY_DPAD_UP },
        { KEYCODE_DPAD_DOWN , cocos2d::EventKeyboard::KeyCode::KEY_DPAD_DOWN },
        { KEYCODE_DPAD_LEFT , cocos2d::EventKeyboard::KeyCode::KEY_DPAD_LEFT },
        { KEYCODE_DPAD_RIGHT , cocos2d::EventKeyboard::KeyCode::KEY_DPAD_RIGHT },
        { KEYCODE_ENTER  , cocos2d::EventKeyboard::KeyCode::KEY_ENTER},
        { KEYCODE_PLAY  , cocos2d::EventKeyboard::KeyCode::KEY_PLAY},
        { KEYCODE_DPAD_CENTER  , cocos2d::EventKeyboard::KeyCode::KEY_DPAD_CENTER},
#endif
    };

    bool RTDipspatchEvent(int keyCode, bool isPressed)
    {
        auto iterKeyCode = g_keyCodeMap.find(keyCode);
        if (iterKeyCode == g_keyCodeMap.end()) {
            return false;
        }
        
        auto director = cocos2d::Director::getInstance();
        auto eventDispatcher = director->getEventDispatcher();
        auto cocosKeyCode = g_keyCodeMap.at(keyCode);

        cocos2d::EventKeyboard event(cocosKeyCode, isPressed);
        eventDispatcher->dispatchEvent(&event);

        return true;
    }

    void RTSetDefaultResourceRootPath(const char* resRootPath)
    {
        LOGD("nativeSetDefaultResourceRootPath: %s", resRootPath);

        auto gplayFileUtils = new (std::nothrow) GPlayFileUtils();
        gplayFileUtils->initWithDefaultResourceRootPath(resRootPath);
        FileUtils::setDelegate(gplayFileUtils);

        static bool preGameStartMark = true;
        if (preGameStartMark)
        {
            preGameStartMark = false;
            onPreGameStart();
        }
    }

    void  RTAddSearchPath(const char* resSearchPath)
    {
        LOGD("nativeAddSearchPath: %s", resSearchPath);
        FileUtils::getInstance()->addSearchPath(resSearchPath);

        //避免在Cocos LUA 工程上通用资源路径因为引擎bug而被移除
        if (strstr(resSearchPath, "/resources/"))
        {
            FileUtils::getInstance()->addSearchPath(resSearchPath);
        }
    }
#else 
// cocos2dx 2.x ================================================================
    bool RTDipspatchEvent(int keyCode, bool isPressed)
    {
        CCKeypadDispatcher* keypadDispatcher = CCDirector::sharedDirector()->getKeypadDispatcher();
        switch (keyCode)
        {
            case KEYCODE_BACK:
            {
                keypadDispatcher->dispatchKeypadMSG(kTypeBackClicked);
                return true;
            }
            case KEYCODE_MENU:
                keypadDispatcher->dispatchKeypadMSG(kTypeMenuClicked);
                return true;
            default:
                return false;
        }
    }

    void RTSetDefaultResourceRootPath(const char* resRootPath)
    {
        LOGD("nativeSetDefaultResourceRootPath: %s", resRootPath);
        GPlayFileUtils* gplayFileUtils = new (std::nothrow) GPlayFileUtils();
        gplayFileUtils->initWithDefaultResourceRootPath(resRootPath);
        CCFileUtils::setDelegate(gplayFileUtils);

        static bool preGameStartMark = true;
        if (preGameStartMark)
        {
            preGameStartMark = false;
            onPreGameStart();
        }
    }

    void  RTAddSearchPath(const char* resSearchPath)
    {
        LOGD("nativeAddSearchPath: %s", resSearchPath);
        CCFileUtils::sharedFileUtils()->addSearchPath(resSearchPath);
    }
#endif //cocos2dx 2.x

//------------------------------------------------------------------------------
    void RTPreloadResponse(int resultCode, int errorCode, const std::string& groupName, float percent, float downloadSpeed)
    {
        if (s_preloadResponseCallback)
        {
            s_preloadResponseCallback(resultCode, errorCode, groupName, percent, downloadSpeed);
        }

        if(common::PRELOAD_RESULT_SUCCESS == resultCode)
        {
            s_preloadSuccessCallback(0, "success");
        }
    }

    void RTRuntimeInit()
    {
        LOGD("RTRuntimeInit.");
#if COCOS2D_VERSION > 0x00030200
        static bool _initGlContextAttrs = true;
        if(_initGlContextAttrs)
            Java_org_cocos2dx_lib_Cocos2dxActivity_getGLContextAttrs(JniHelper::getEnv(), NULL);
        _initGlContextAttrs = false;
#endif
        bool isJniFuncCallSuccess;
        common::callExtensionSyncAPI("UseNewPreloadResponseMode", isJniFuncCallSuccess, "", 0, 0);
    }

    void RTSetPreloadSuccessCallback(GPlayPreloadSuccessCallback callback)
    {
        s_preloadSuccessCallback = callback;
    }

    void RTSetPreloadResponseCallback(GPlayPreloadResponseCallback callback)
    {
        s_preloadResponseCallback = callback;
    }

    void RTQuitGame()
    {
        RTQuitGameCallback();
    }

    void RTCaptureScreenJNI(const std::string& params)
    {
        cocos2d::JniMethodInfo t;
        if (cocos2d::JniHelper::getStaticMethodInfo(t, "org/cocos2dx/lib/Cocos2dxHelper", 
            "captureScreen", "(Ljava/lang/String;)V"))
        {
            jstring stringArg1 = t.env->NewStringUTF(params.c_str());
            t.env->CallStaticVoidMethod(t.classID, t.methodID, stringArg1);
            t.env->DeleteLocalRef(stringArg1);
            t.env->DeleteLocalRef(t.classID);
        }
    }
}
