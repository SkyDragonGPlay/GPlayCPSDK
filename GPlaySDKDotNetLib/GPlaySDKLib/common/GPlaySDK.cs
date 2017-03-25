using UnityEngine;
using System.Runtime.InteropServices;
using System.Text;

namespace GPlay
{
    public delegate void PreloadResponseCallback(PreloadResponseInfo info);
    public delegate void PreloadSuccessCallback();

    public class GPlaySDK
    {
        private const string STR_GPLAY_PROXY_NAME = "GPlaySDK";

        private static GPlaySDKProxy proxy;
        private static PreloadResponseCallback preloadResponseCallback;
        private static PreloadSuccessCallback preloadSuccessCallback;

        static GPlaySDK()
        {
            GameObject goGPlayProxy = GameObject.Find(STR_GPLAY_PROXY_NAME);
            if(goGPlayProxy == null)
                goGPlayProxy = new GameObject(STR_GPLAY_PROXY_NAME);
            GameObject.DontDestroyOnLoad(goGPlayProxy);

            proxy = goGPlayProxy.GetComponent<GPlaySDKProxy>();
            if (proxy == null)
                proxy = goGPlayProxy.AddComponent<GPlaySDKProxy>();
        }

        /// <summary>
        /// 得到可写路径
        /// </summary>
        /// <returns></returns>
        public static string GetWritablePath()
        {
            string writablePath = string.Empty;

            LogFormat("GPlaySDK.GetWritablePath", "Call Android static method: com.skydragon.gplay.runtime.bridge.CocosRuntimeBridge.getWritablePath");
            AndroidJavaClass javaClass = new AndroidJavaClass("com.skydragon.gplay.runtime.bridge.CocosRuntimeBridge");
            writablePath = javaClass.CallStatic<string>("getWritablePath");
            return writablePath;
        }

        /// <summary>
        /// 当前游戏是否处于 GPlay 环境
        /// </summary>
        public static bool IsInGplayEnv()
        {
            bool result = false;
            if (Application.platform == RuntimePlatform.Android)
                result = isInGplayEnv();

            return result;
        }

        /// <summary>
        /// 初始化GPlay SDK
        /// </summary>
        /// <param name="appKey"></param>
        /// <param name="appSecret"></param>
        /// <param name="privateKey"></param>
        public static void InitSDK(string appKey, string appSecret, string privateKey)
        {
            if (Application.platform == RuntimePlatform.Android)
                initSDK(appKey, appSecret, privateKey);
        }

        /// <summary>
        /// 游戏当前所在的渠道ID
        /// </summary>
        public static string GetChannelID()
        {
            string result = string.Empty;
            if (Application.platform == RuntimePlatform.Android)
                result = getChannelID();
            return result;
        }

        /// <summary>
        /// 获取当前网络类型
        /// </summary>
        public static ENetworkType GetNetworkType()
        {
            ENetworkType result = ENetworkType.NO_NETWORK;
            if (Application.platform == RuntimePlatform.Android)
                result = getNetworkType();
            return result;
        }

        /// <summary>
        /// 自动切分资源模式下停止分配资源到指定资源包
        /// </summary>
        public static bool IsLogined()
        {
            bool result = false;
            if (Application.platform == RuntimePlatform.Android)
                result = isLogined();
            return result;
        }

        /// <summary>
        /// 获取用户 ID
        /// </summary>
        public static string GetUserID()
        {
            string userID = string.Empty;
            if (Application.platform == RuntimePlatform.Android)
                userID = getUserID();
            return userID;
        }

        /// <summary>
        /// 登录
        /// </summary>
        /// <param name="callback"></param>
        public static void Login(ActionCallback callback)
        {
            if (Application.platform == RuntimePlatform.Android)
            {
                if (callback == null)
                {
                    LogErrorFormat("GPlaySDK.Login", "callback is null!!");
                    return;
                }

                int callbackID = ActionCallbackManager.Instance.AddCallback(callback);
                login(callbackID);
            }
        }

        /// <summary>
        /// 退出游戏
        /// </summary>
        public static void QuitGame()
        {
            if (Application.platform == RuntimePlatform.Android)
                quitGame();
        }

        /// <summary>
        /// 分享游戏
        /// </summary>
        /// <param name="shareInfo"></param>
        /// <param name="callback"></param>
        public static void Share(GPlayShareParams shareInfo, ActionCallback callback)
        {
            if (Application.platform == RuntimePlatform.Android)
            {
                if (shareInfo == null || callback == null)
                {
                    LogErrorFormat("GPlaySDK.Share", "shareInfo or callback is null!!");
                    return;
                }
                string shareInfoJson = ShareParams2String(shareInfo);
                //string shareInfoJson = JsonConvert.SerializeObject(shareInfo);
                int callbackid = ActionCallbackManager.Instance.AddCallback(callback);
                share(callbackid, shareInfoJson);
            }
        }

        /// <summary>
        /// 支付
        /// </summary>
        /// <param name="payInfo"></param>
        /// <param name="callback"></param>
        public static void Pay(GPlayPayParams payInfo, ActionCallback callback)
        {
            if (Application.platform == RuntimePlatform.Android)
            {
                if (payInfo == null || callback == null)
                {
                    LogErrorFormat("GPlaySDK.Pay", "payInfo or callback is null!!");
                    return;
                }

                string payInfoJson = PayParams2String(payInfo);
                int callbackid = ActionCallbackManager.Instance.AddCallback(callback);
                pay(callbackid, payInfoJson);
            }
        }

        /// <summary>
        /// 创建桌面快捷图标
        /// </summary>
        /// <param name="callback"></param>
        public static void CreateShortcut(ActionCallback callback)
        {
            if (Application.platform == RuntimePlatform.Android)
            {
                if (callback == null)
                {
                    LogErrorFormat("GPlaySDK.CreateShortcut", "callback is null!!");
                    return;
                }

                int callbackid = ActionCallbackManager.Instance.AddCallback(callback);
                createShortcut(callbackid);
            }
        }

        /// <summary>
        /// 判断是否支持某个方法
        /// </summary>
        /// <param name="funcName"></param>
        public static bool IsFunctionSupported(string funcName)
        {
            bool result = false;
            if (Application.platform == RuntimePlatform.Android)
                result = isFunctionSupported(funcName);
            return result;
        }

        /// <summary>
        /// 同步调用扩展接口
        /// </summary>
        /// <param name="funcName"></param>
        /// <param name="parameters"></param>
        /// <returns></returns>
        public static string CallSyncFunc(string funcName, string parameters)
        {
            string result = string.Empty;
            if (Application.platform == RuntimePlatform.Android)
                result = callSyncFunc(funcName, parameters);
            return result;
        }

        /// <summary>
        /// 异步调用扩展接口
        /// </summary>
        /// <param name="funcName"></param>
        /// <param name="parameters"></param>
        /// <param name="callback"></param>
        public static void CallAsyncFunc(string funcName, string parameters, ActionCallback callback)
        {
            if (Application.platform == RuntimePlatform.Android)
            {
                LogFormat("GPlaySDK.CallAsyncFunc", "funcName:{0}  parameters:{1}", funcName, parameters);
                int callbackid = ActionCallbackManager.Instance.AddCallback(callback);
                callAsyncFunc(funcName, parameters, callbackid);
            }
        }
        
        /// <summary>
        /// 加载单个资源包
        /// </summary>
        /// <param name="bundleName"></param>
        /// <param name="preloadResponseCallback"></param>
        public static void PreloadGroup(string group, PreloadResponseCallback preloadResponseCallback)
        {
            if (Application.platform == RuntimePlatform.Android)
            {
                if (!string.IsNullOrEmpty(group))
                    PreloadGroups(new string[] { group }, preloadResponseCallback);
                else
                    LogErrorFormat("PreloadResourceBundle", "bundle name can not be empty or null");
            }
        }

        /// <summary>
        /// 加载单个资源包
        /// </summary>
        /// <param name="bundleName"></param>
        /// <param name="preloadSuccessCallback"></param>
        public static void PreloadGroup(string group, PreloadSuccessCallback preloadSuccessCallback)
        {
            if (Application.platform == RuntimePlatform.Android)
            {
                if (!string.IsNullOrEmpty(group))
                    PreloadGroups(new string[] { group }, preloadSuccessCallback);
                else
                    LogErrorFormat("PreloadResourceBundle", "bundle name can not be empty or null");
            }
        }

        /// <summary>
        /// 加载多个资源包
        /// </summary>
        /// <param name="bundleNames"></param>
        /// <param name="preloadResponseCallback"></param>
        public static void PreloadGroups(string[] groups, PreloadResponseCallback preloadResponseCallback)
        {
            if (Application.platform == RuntimePlatform.Android)
            {
                if (preloadResponseCallback != null)
                    PreloadGroups(groups, preloadResponseCallback, null);
                else
                    LogErrorFormat("PreloadResourceBundles", "preloadResponseCallback can not be null!!!");
            }
        }

        /// <summary>
        /// 加载多个资源包
        /// </summary>
        /// <param name="bundleNames"></param>
        /// <param name="preloadSuccessCallback"></param>
        public static void PreloadGroups(string[] groups, PreloadSuccessCallback preloadSuccessCallback)
        {
            if (Application.platform == RuntimePlatform.Android)
            {
                if (preloadSuccessCallback != null)
                    PreloadGroups(groups, null, preloadSuccessCallback);
                else
                    LogErrorFormat("PreloadResourceBundles", "preloadSuccessCallback can not be null!!!");
            }
        }

        /// <summary>
        /// 加载多个资源包
        /// </summary>
        /// <param name="bundleNames"></param>
        /// <param name="preloadResponseCallback"></param>
        /// <param name="preloadSuccessCallback"></param>
        private static void PreloadGroups(string[] groups, PreloadResponseCallback preloadResponseCallback, PreloadSuccessCallback preloadSuccessCallback)
        {
            if (Application.platform == RuntimePlatform.Android)
            {
                GPlaySDK.preloadResponseCallback = preloadResponseCallback;
                GPlaySDK.preloadSuccessCallback = preloadSuccessCallback;

                StringBuilder groupsJson = new StringBuilder("{\"scenes\":[\"");
                groupsJson.Append(groups[0]).Append("\"");
                for (int i = 1; i < groups.Length; ++i)
                {
                    if (string.IsNullOrEmpty(groups[i]))
                        continue;
                    groupsJson.Append(",\"").Append(groups[i]).Append("\"");
                }
                groupsJson.Append("]}");

                // 若 ext == 1 则会回调进度给 preloadResponseCallback
                // 若 ext == 0 则只会在下载成功后回调给 preloadSuccessCallback
                int ext = preloadResponseCallback == null ? 0 : 1;
                preloadGroups(groupsJson.ToString(), ext);
            }
        }

        internal static void PreloadResponse(PreloadResponseInfo responseInfo)
        {
            if (responseInfo == null)
                return;

            if(preloadResponseCallback != null)
            {
                preloadResponseCallback(responseInfo);
            }

            if(responseInfo.resultCode == EActionResultCode.PRELOAD_RESULT_SUCCESS && preloadSuccessCallback != null)
            {
                preloadSuccessCallback();
            }
        }

        internal static void LogFormat(string methodName, string format, params object[] args)
        {
            Debug.LogFormat("GPlaySDK  " + methodName + " : " + format, args);
        }

        internal static void LogErrorFormat(string methodName, string format, params object[] args)
        {
            Debug.LogErrorFormat("GPlaySDK  " + methodName + " : " + format, args);
        }
        
        private static string ShareParams2String(GPlayShareParams gplayShareParams)
        {
            StringBuilder shareJson = new StringBuilder("{\"url\":\"");
            shareJson.Append(gplayShareParams.pageUrl);
            shareJson.Append("\",\"title\":\"");
            shareJson.Append(gplayShareParams.title);
            shareJson.Append("\",\"text\":\"");
            shareJson.Append(gplayShareParams.content);
            shareJson.Append("\",\"img_url\":\"");
            shareJson.Append(gplayShareParams.imgUrl);
            shareJson.Append("\",\"imgTitle\":\"");
            shareJson.Append(gplayShareParams.imgTitle);
            shareJson.Append("\"}");
            return shareJson.ToString();
        }
        
        private static string PayParams2String(GPlayPayParams payParams)
        {
            StringBuilder payJson = new StringBuilder("{\"product_id\":\"");
            payJson.Append(payParams.productId);
            payJson.Append("\",\"product_name\":\"");
            payJson.Append(payParams.productName);

            payJson.Append("\",\"product_price\":\"");
            payJson.Append(payParams.productPrice);

            payJson.Append("\",\"product_count\":\"");
            payJson.Append(payParams.productCount);

            payJson.Append("\",\"product_desc\":\"");
            payJson.Append(payParams.productDescription);
            payJson.Append("\",\"game_user_id\":\"");
            payJson.Append(payParams.gameUserId);
            payJson.Append("\",\"game_user_name\":\"");
            payJson.Append(payParams.gameUserName);
            payJson.Append("\",\"server_id\":\"");
            payJson.Append(payParams.serverId);
            payJson.Append("\",\"server_name\":\"");
            payJson.Append(payParams.serverName);
            payJson.Append("\",\"private_data\":\"");
            payJson.Append(payParams.extraData);
            payJson.Append("\"}");
            return payJson.ToString();
        }

        #region GPlayCPSDK native
        [DllImport("gplay")]
        private extern static bool isInGplayEnv();

        [DllImport("gplay")]
        private extern static void initSDK(string appKey, string appSecret, string privateKey);

        [DllImport("gplay")]
        private extern static string getChannelID();

        [DllImport("gplay")]
        private extern static ENetworkType getNetworkType();

        [DllImport("gplay")]
        private extern static void preloadGroups(string jsonGroups, int ext);

        ///// <summary>
        ///// 自动切分资源模式下停止分配资源到指定资源包
        ///// </summary>
        ///// <param name="bundleName"></param>
        //[DllImport("GPlayCPSDK", EntryPoint = "backFromResourceBundle")]
        //public extern static void BackFromResourceBundle(string bundleName);

        [DllImport("gplay")]
        private extern static bool isLogined();

        [DllImport("gplay")]
        private extern static string getUserID();

        [DllImport("gplay")]
        private extern static void login(int callbackID);

        [DllImport("gplay")]
        private extern static void quitGame();

        [DllImport("gplay")]
        private extern static void share(int callbackid, string shareInfoJson);

        [DllImport("gplay")]
        private extern static void pay(int callbackid, string payInfoJson);

        [DllImport("gplay")]
        private extern static void createShortcut(int callbackid);

        [DllImport("gplay")]
        private extern static bool isFunctionSupported(string funcName);

        [DllImport("gplay")]
        private extern static string callSyncFunc(string funcName, string parameters);

        [DllImport("gplay")]
        private extern static void callAsyncFunc(string funcName, string parameters, int callbackid);
        #endregion
    }

    public class GPlayShareParams
    {
        public string pageUrl;
        public string title;
        public string content;
        public string imgUrl;
        public string imgTitle;
    }

    public class GPlayPayParams
    {
        public string productId;
        public string productName;
        public int productPrice;
        public int productCount;
        public string productDescription;
        public string gameUserId;
        public string gameUserName;
        public string serverId;
        public string serverName;
        public string extraData;
    };

    public class PreloadResponseInfo
    {
        public EActionResultCode resultCode;
        public EActionResultCode errorCode;
        public string groupName;
        public float percent;
        public float downloadSpeed;
    }

    public class ActionCallbackInfo
    {
        public int callbackID;
        public EActionResultCode resultCode;
        public string resultJson;
    }
}