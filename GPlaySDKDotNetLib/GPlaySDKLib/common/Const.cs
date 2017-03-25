using UnityEngine;
using System.Collections;


namespace GPlay
{
    public enum ENetworkType
    {
        NO_NETWORK = -1,
        MOBILE = 0,
        WIFI
    }
    
    public enum EActionResultCode
    {
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
    }
}