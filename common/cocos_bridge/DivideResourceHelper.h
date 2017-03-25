#ifndef __GPLAY_DIVIDE_RES_H__
#define __GPLAY_DIVIDE_RES_H__

#include <string>

#include "cocos2d.h"
#if COCOS2D_VERSION >= 0x00030300
#include "platform/android/CCFileUtils-android.h"
#define COCOS_FILE_UTILS_ANDROID cocos2d::FileUtilsAndroid
#elif COCOS2D_VERSION >= 0x00030000
#include "platform/android/CCFileUtilsAndroid.h"
#define COCOS_FILE_UTILS_ANDROID cocos2d::FileUtilsAndroid
#else
#include "platform/android/CCFileUtilsAndroid.h"
#define COCOS_FILE_UTILS_ANDROID cocos2d::CCFileUtilsAndroid
#endif

#if CC_TARGET_PLATFORM == CC_PLATFORM_ANDROID

namespace gplay
{
    typedef enum
    {
        RUN_MODE_NORMAL = 1,
        RUN_MODE_DIVIDE_RES = 2
    } GameRunMode;

    class GPlayFileUtils : public COCOS_FILE_UTILS_ANDROID
    {
    public:
        static GPlayFileUtils* getCurrInstance();

        static GameRunMode getGameRunMode() {
            return s_gameRunMode;
        }

        static bool isDivideResourceMode() {
            return s_gameRunMode == RUN_MODE_DIVIDE_RES;
        }

        GPlayFileUtils();

        virtual bool initWithDefaultResourceRootPath(const std::string &path);
#if COCOS2D_VERSION >= 0x00030600
        virtual std::string fullPathForFilename(const std::string &filename) const override;
#elif COCOS2D_VERSION >= 0x00030000
        virtual std::string fullPathForFilename(const std::string &filename) override;
#else
        virtual std::string fullPathForFilename(const char* pszFileName);

        virtual bool isFileExist(const std::string& strFilePath);

        virtual unsigned char* getFileData(const char* pszFileName, const char* pszMode, unsigned long * pSize);
#endif
    protected:
        void log_for_divide_res(const std::string& fullPath, const char* accessesFile) const;

        static GameRunMode s_gameRunMode;
        std::string _gplayDefaultResourceRootPath;
    };
    
}

#endif //CC_PLATFORM_ANDROID

#endif // __GPLAY_DIVIDE_RES_H__
