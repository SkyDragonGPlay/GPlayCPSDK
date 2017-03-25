#ifndef GPLAY_UNITY_H_INCLUDED
#define GPLAY_UNITY_H_INCLUDED

// #include <vector>
// #include <string>
#include "gplay.h"

extern "C"
{
    namespace gplay
    {
        // typedef void(*GplayResultCallback)(int,const char*);
        // typedef void(*GPreloadSuccessCallback)(int,const char*);
        typedef void(*GPlayPreloadResponseCallback)(int,int,const std::string&,float,float);

        /**
         * 当前游戏是否处于 Gplay 环境
         */
        bool isInGplayEnv();

        /**
         * 初始化GPlay SDK
         */
        void initSDK(const char* appKey, const char* appSecret, const char* privateKey);

        /**
         * 游戏当前所在的渠道ID
         */
        const char* getChannelID();

        /**
         * 获取当前网络类型
         */
        common::GplayNetWorkStatus getNetworkType();

        /**
         * 自动切分资源模式下停止分配资源到指定资源包
         */
        bool isLogined();

        /**
         * 获取用户 ID
         */
        const char* getUserID();

        /**
         * 登录
         */
        void login(int callbackid);

        /**
         * 退出游戏
         */
        void quitGame();

        /**
         * 分享游戏
         */
        void share(int callbackid, const char* shareInfoJson);

        /**
         * 支付
         */
        void pay(int callbackid, const char* payInfoJson);

        /**
         * 创建桌面快捷图标
         */
        void createShortcut(int callbackid);

        /**
         * 判断是否支持某个方法
         */
        bool isFunctionSupported(const char* funcName);

        /**
         * 加载多个资源包
         */
        void preloadGroups(const char* jsonGroups, int ext);

        /**
         * 调用同步扩展接口
         */
        const char* callSyncFunc(const char* funcName, const char* params);

        /**
         * 调用异步扩展接口
         */
        void callAsyncFunc(const char* funcName, const char* params, int callbackid);
    }
}

#endif // GPLAY_UNITY_H_INCLUDED
