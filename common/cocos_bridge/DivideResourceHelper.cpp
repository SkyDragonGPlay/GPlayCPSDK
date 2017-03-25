#include "DivideResourceHelper.h"

#if (CC_TARGET_PLATFORM == CC_PLATFORM_ANDROID)

#include <android/log.h>
#include "gplay.h"

#define LOG_DIVIDE_RES(...)  __android_log_print(ANDROID_LOG_INFO, "gplay_divide_res", __VA_ARGS__)
#define LOG_DIVIDE_ERROR(...)  __android_log_print(ANDROID_LOG_ERROR, "gplay_divide_res", __VA_ARGS__)
#define LOG_MISS_RES(...)  __android_log_print(ANDROID_LOG_INFO, "gplay_check_res", __VA_ARGS__)

#define IS_ABSOLUTE_PATH(path) (path[0] == '/')

namespace gplay
{
    GameRunMode GPlayFileUtils::s_gameRunMode = RUN_MODE_NORMAL;

    static GameRunMode getGplayGameRunModeJNI()
    {
        GameRunMode ret = RUN_MODE_NORMAL;

        bool isJniFuncCallSuccess;
        std::string mode = gplay::common::callExtensionSyncAPI("getGPlayGameRunMode", isJniFuncCallSuccess, "", 0, 0.0);
        if (isJniFuncCallSuccess && mode.compare("gplay_divide_res") == 0)
        {
            ret = RUN_MODE_DIVIDE_RES;
        }

        return ret;
    }

    GPlayFileUtils* GPlayFileUtils::getCurrInstance()
    {
        return (GPlayFileUtils*)s_sharedFileUtils;
    }

    GPlayFileUtils::GPlayFileUtils()
    {
        s_gameRunMode = getGplayGameRunModeJNI();
    }

    bool GPlayFileUtils::initWithDefaultResourceRootPath(const std::string &path)
    {
        _gplayDefaultResourceRootPath = path;
#if COCOS2D_VERSION >= 0x00030000
        _defaultResRootPath = path;
        return FileUtils::init();
#else
        m_strDefaultResRootPath = path;
        return CCFileUtils::init();
#endif
    }
//-----------------------cocos2d-x 2.x begin--------------------------------------
#if COCOS2D_VERSION < 0x00030000
unsigned char* GPlayFileUtils::getFileData(const char* pszFileName, const char* pszMode, unsigned long * pSize)
    {
        if (pszFileName == NULL || 0 == strlen(pszFileName))
        {
            if(pSize) pSize = 0;
            return NULL;
        }

        if (IS_ABSOLUTE_PATH(pszFileName))
        {
            return CCFileUtilsAndroid::getFileData(pszFileName, pszMode, pSize);
        }
        else
        {
            std::string fullPath = fullPathForFilename(pszFileName);
            if (IS_ABSOLUTE_PATH(fullPath.c_str()))
            {
                return CCFileUtilsAndroid::getFileData(fullPath.c_str(), pszMode, pSize);
            }
            else
            {
                if(pSize) pSize = 0;
                return NULL;
            }
        }
    }

    bool GPlayFileUtils::isFileExist(const std::string& strFilePath)
    {
        if (strFilePath.empty())
            return false;
        
        const char* targetPath;
        std::string fullPath;
        if (IS_ABSOLUTE_PATH(strFilePath.c_str()))
        {
            targetPath = strFilePath.c_str();
        }
        else
        {
            fullPath = _gplayDefaultResourceRootPath + strFilePath;
            targetPath = fullPath.c_str();
        }

        FILE *fp = fopen(targetPath, "r");
        if(fp)
        {
            fclose(fp);
            return true;
        }

        return false;
    }
#endif
//-----------------------cocos2d-x 2.x end--------------------------------------

    void GPlayFileUtils::log_for_divide_res(const std::string& fullPath, const char* accessesFile) const
    {
        if(fullPath.empty() || !IS_ABSOLUTE_PATH(fullPath.c_str()))
        {
            LOG_MISS_RES("miss file:%s", accessesFile);
        }
        else if (s_gameRunMode == RUN_MODE_DIVIDE_RES)
        {
            if(fullPath.find(_gplayDefaultResourceRootPath) == 0)
                LOG_DIVIDE_RES("scene:%s; accesses resource:%s", common::getCurrPreloadSceneName().c_str(), 
                    fullPath.substr(_gplayDefaultResourceRootPath.length()).c_str());
            else if(!IS_ABSOLUTE_PATH(fullPath.c_str()))
                LOG_DIVIDE_RES("scene:%s; accesses resource:%s", common::getCurrPreloadSceneName().c_str(), 
                    fullPath.c_str());
        }
    }

#if COCOS2D_VERSION >= 0x00030600
    std::string GPlayFileUtils::fullPathForFilename(const std::string &filename) const
    {
        if (filename.empty())
            return filename;

        std::string fullPath = cocos2d::FileUtilsAndroid::fullPathForFilename(filename);
        log_for_divide_res(fullPath, filename.c_str());
        
        return fullPath;
    }
#elif COCOS2D_VERSION >= 0x00030000
    std::string GPlayFileUtils::fullPathForFilename(const std::string &filename)
    {
        if (filename.empty())
            return filename;

        std::string fullPath = cocos2d::FileUtilsAndroid::fullPathForFilename(filename);
        log_for_divide_res(fullPath, filename.c_str());
        
        return fullPath;
    }
#else // cocos2d-x 2.x
    std::string GPlayFileUtils::fullPathForFilename(const char* filename)
    {
        if (filename == NULL || 0 == strlen(filename))
            return "";
        
        std::string fullPath = cocos2d::CCFileUtilsAndroid::fullPathForFilename(filename);
        log_for_divide_res(fullPath, filename);
        
        return fullPath;
    }
#endif

} //namespace gplay

#endif //CC_PLATFORM_ANDROID
