#ifndef __GPLAY_CPP_H__
#define __GPLAY_CPP_H__

#include <vector>
#include <string>

#include "cocos2d.h"

#if COCOS2D_VERSION >= 0x00030000
#include <functional>
#endif

namespace gplay {

typedef enum {
    GPLAY_INIT_SUCCESS = 0,                 // succeeding in initing sdk
    GPLAY_INIT_FAIL = 1,                    // failing to init sdk

    USER_LOGIN_RESULT_SUCCESS = 10000,      // login success
    USER_LOGIN_RESULT_FAIL = 10001,         // login failed
    USER_LOGIN_RESULT_CANCEL = 10002,       // login canceled
    USER_LOGOUT_RESULT_SUCCESS = 10003,     // logout success
    USER_LOGOUT_RESULT_FAIL = 10004,        // logout failed
    USER_REGISTER_RESULT_SUCCESS = 10005,   // regiister sucess
    USER_REGISTER_RESULT_FAIL = 10006,      // regiister failed
    USER_REGISTER_RESULT_CANCEL = 10007,    // regiister Cancel
    USER_BIND_RESULT_SUCESS = 10008,        // bind sucess
    USER_BIND_RESULT_CANCEL = 10009,        // bind Cancel
    USER_BIND_RESULT_FAILED = 100010,       // bind failed
    USER_RESULT_NETWROK_ERROR = 10011,      // network error
    USER_RESULT_USEREXTENSION = 19999,      // extension code

    PAY_RESULT_SUCCESS = 20000,             // pay success
    PAY_RESULT_FAIL = 20001,                // pay fail
    PAY_RESULT_CANCEL = 20002,              // pay cancel
    PAY_RESULT_INVALID = 20003,             // incompleting info
    PAY_RESULT_NETWORK_ERROR = 20004,       // network error
    PAY_RESULT_NOW_PAYING = 20005,          // paying now
    PAY_RESULT_PAYEXTENSION = 29999,        // extension code
    
    SHARE_RESULT_SUCCESS = 30000,           // share success
    SHARE_RESULT_FAIL = 30001,              // share failed
    SHARE_RESULT_CANCEL = 30002,            // share canceled
    SHARE_RESULT_NETWORK_ERROR = 30003,     // network error
    SHARE_RESULT_SHAREREXTENSION = 39999,   // extension code
    
    SHORTCUT_RESULT_SUCCESS = 40000,
    SHORTCUT_RESULT_FAILED = 40001,

    CAPTURE_SCREEN_SUCCESS = 41000,
    CAPTURE_SCREEN_FAILED = 41001,

    PRELOAD_RESULT_SUCCESS = 50000,
    PRELOAD_RESULT_PROGRESS,
    PRELOAD_RESULT_FAILED,

    PRELOAD_ERROR_NETWORK = 60000,
    PRELOAD_ERROR_VERIFY_FAILED,
    PRELOAD_ERROR_NO_SPACE,
    PRELOAD_ERROR_UNKNOWN,
    PRELOAD_ERROR_NONE
} ActionResultCode;

namespace NetworkType{
typedef enum {
    NO_NETWORK = -1,
    MOBILE = 0,
    WIFI
} GplayNetWorkType;
}

class GplayShareParams {
public:
    GplayShareParams()
    : pageUrl(NULL)
    , title(NULL)
    , content(NULL)
    , imgUrl(NULL)
    , imgTitle(NULL)
    {
    }

    const char* pageUrl;
    const char* title;
    const char* content;
    const char* imgUrl;
    const char* imgTitle;
};

class GplayPayParams {
public:
    GplayPayParams()
    : productId(NULL)
    , productName(NULL)
    , productPrice(0)
    , productCount(0)
    , productDescription(NULL)
    , gameUserId(NULL)
    , gameUserName(NULL)
    , serverId(NULL)
    , serverName(NULL)
    , extraData(NULL)
    {
    }

    const char* productId; 
    const char* productName;
    int productPrice;
    int productCount;
    const char* productDescription;
    const char* gameUserId;
    const char* gameUserName;
    const char* serverId;
    const char* serverName;
    const char* extraData;
};

#if COCOS2D_VERSION >= 0x00030000

    typedef std::function<void(int,const char*)> GplayResultCallback;

    /**
     * resultCode: 0
     * msg:"success"
     */
    typedef std::function<void(int,const char*)> GPreloadSuccessCallback;

    /**
     * resultCode:
     *            PRELOAD_RESULT_SUCCESS
     *            PRELOAD_RESULT_PROGRESS
     *            PRELOAD_RESULT_FAILED
     * errorCode:
     *            PRELOAD_ERROR_NETWORK
     *            PRELOAD_ERROR_VERIFY_FAILED
     *            PRELOAD_ERROR_NO_SPACE
     *            PRELOAD_ERROR_UNKNOWN
     *            PRELOAD_ERROR_NONE
     * bundleName: string [the name of resource bundle]
     * percent: float [the download process，0-100]
     * download_speed: float [the download speed]
     */
    typedef std::function<void(int,int,const std::string&,float,float)> GPreloadResponseCallback;

#else
    typedef void(*GplayResultCallback)(int,const char*);
    typedef void(*GPreloadSuccessCallback)(int,const char*);
    typedef void(*GPreloadResponseCallback)(int,int,const std::string&,float,float);
#endif

bool isInGplayEnv();

void initSDK(const std::string& appKey, const std::string& appSecret, const std::string& privateKey);

NetworkType::GplayNetWorkType getNetworkType();

void preloadResourceBundle(const std::string& bundleName, GPreloadSuccessCallback callback);

void preloadResourceBundles(const std::vector<std::string>& bundleNames, GPreloadSuccessCallback callback);

void backFromResourceBundle(const std::string& bundleName);

/**
 * Set the response callback of `preload resource bundle`.
 *
 * If you don't want to customize the UI interface of preloadResourceBundle\preloadResourceBundles,
 * you do not need calling this method.
 */
void setPreloadResponseCallback(GPreloadResponseCallback callback);

bool isLogined();

const std::string& getUserID();

void login(GplayResultCallback callback);

void logout(GplayResultCallback callback);

void quitGame();

void share(const GplayShareParams& params, GplayResultCallback callback);

void pay(const GplayPayParams& params, GplayResultCallback callback);

const std::string& getOrderId();

const std::string& getChannelID();

void createShortcut(GplayResultCallback callback);

bool isFunctionSupported(const std::string& funcName);

std::string callSyncFunc(const std::string& funcName, const std::string& params);

void callAsyncFunc(const std::string& funcName, const std::string& params, GplayResultCallback callback);

//弃用
void preloadGroups(const std::vector<std::string>& groups, GPreloadSuccessCallback callback);

//弃用
void preloadGroup(const std::string& group, GPreloadSuccessCallback callback);

//弃用
void backFromGroup(const std::string& group);

}

#endif /** __GPLAY_CPP_H__ */
