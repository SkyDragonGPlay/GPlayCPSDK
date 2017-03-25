using UnityEngine;
using System.Text.RegularExpressions;
using System.Collections;
using System.IO;

namespace GPlay
{
    public class GPlaySDKProxy : MonoBehaviour
    {
        #region c++ 中调用 UnitySendMessage 的消息接收函数

        private void PreloadResponse(string responseInfoStr)
        {
            GPlaySDK.LogFormat("GPlaySDKProxy.PreloadResponse", "responseInfoStr: {0}", responseInfoStr);
            PreloadResponseInfo responseInfo = ParsePreloadResponseInfo(responseInfoStr);
            if (responseInfo != null)
            {
                GPlaySDK.PreloadResponse(responseInfo);
            }
        }

        private void ActionCallback(string callbackInfoStr)
        {
            GPlaySDK.LogFormat("GPlaySDKProxy.ActionCallback", "callbackInfoStr: {0}", callbackInfoStr);
            var callbackInfo = ParseCallbackResult(callbackInfoStr);
            if (callbackInfo != null)
            {
                ActionCallbackManager.Instance.Invoke(callbackInfo.callbackID, callbackInfo.resultCode, callbackInfo.resultJson);
            }
        }

        private PreloadResponseInfo ParsePreloadResponseInfo(string responseInfoStr)
        {
            GPlaySDK.LogFormat("GPlaySDKProxy.ParsePreloadResponseInfo", "responseInfoStr: {0}", responseInfoStr);
            Regex regex = new Regex(@"\{""resultCode"":(?<resultCode>[+\-\d]+), ""errorCode"":(?<errorCode>[+\-\d]+), ""groupName"":(?<groupName>[\s\S]*), ""percent"":(?<percent>[+\-\.\d]+), ""downloadSpeed"":(?<downloadSpeed>[+\-\.\d]+)\}");

            Match match = regex.Match(responseInfoStr);
            var responseInfo = new PreloadResponseInfo();
            responseInfo.resultCode = (EActionResultCode)int.Parse(match.Groups["resultCode"].Value);
            responseInfo.errorCode = (EActionResultCode)int.Parse(match.Groups["errorCode"].Value);
            responseInfo.groupName = match.Groups["groupName"].Value;
            responseInfo.percent = float.Parse(match.Groups["percent"].Value);
            responseInfo.downloadSpeed = float.Parse(match.Groups["downloadSpeed"].Value);

            return responseInfo;
        }

        private ActionCallbackInfo ParseCallbackResult(string callbackInfoStr)
        {
            GPlaySDK.LogFormat("GPlaySDKProxy.ParseCallbackResult", "callbackInfoStr: {0}", callbackInfoStr);
            ActionCallbackInfo actionCallbackInfo = new ActionCallbackInfo();

            Regex regex = new Regex(@"\{""callbackID"":(?<callbackID>[+\-\d]+), ""resultCode"":(?<resultCode>[+\-\d]+), ""resultJson"":(?<resultJson>[\s\S]*)\}");
            Match match = regex.Match(callbackInfoStr);
            actionCallbackInfo.callbackID = int.Parse(match.Groups["callbackID"].Value);
            actionCallbackInfo.resultCode = (EActionResultCode)int.Parse(match.Groups["resultCode"].Value);
            actionCallbackInfo.resultJson = match.Groups["resultJson"].Value;

            return actionCallbackInfo;
        }


        #endregion

        #region 截屏
        /// <summary>
        /// C++ 中通过 UnitySendMessage 调用截屏
        /// </summary>
        /// <param name="captureScreenInfoStr"></param>
        private void CaptureScreen(string captureScreenInfoStr)
        {
            Debug.Log("CaptureScreenInfo:  " + captureScreenInfoStr);
            Regex regex = new Regex(@"\{""pictureSaveFile"":(?<pictureSaveFile>[\s\S]*), ""quality"":(?<quality>[+\-\d]+)\}");

            Match match = regex.Match(captureScreenInfoStr);
            string pictureSaveFile = match.Groups["pictureSaveFile"].Value;
            int quality = int.Parse(match.Groups["quality"].Value);

            StartCoroutine(CaptureScreenshot(pictureSaveFile, quality));
        }
        
        /// <summary>
        /// 将截屏后的图片处理成指定质量的图片到指定路径下
        /// </summary>
        /// <param name="sourcePath"></param>
        /// <param name="destPath"></param>
        /// <param name="quality"></param>
        /// <returns></returns>
        private IEnumerator CaptureScreenshot(string pictureSaveFile, int quality)
        {
            string screenshotFilePath = Application.persistentDataPath + "/Screenshot.png";
            Debug.LogFormat("source: {0}   dest: {1}  quality: {2}", screenshotFilePath, pictureSaveFile, quality);

            Application.CaptureScreenshot("Screenshot.png");
            while (!File.Exists(screenshotFilePath))
                yield return null;

            string url = "file:///" + screenshotFilePath;

            using (WWW www = new WWW(url))
            {
                while (!www.isDone)
                    yield return null;

                if (!string.IsNullOrEmpty(www.error))
                    Debug.LogError("WWW error: " + www.error);

                string dir = Path.GetDirectoryName(pictureSaveFile);
                if(!Directory.Exists(dir))
                    Directory.CreateDirectory(dir);

                yield return null;
                Debug.Log("Encode to jpg");
                Texture2D texture = www.texture;
                byte[] screenshotBuffer = texture.EncodeToJPG(quality);
                using (FileStream fileStream = File.Open(pictureSaveFile, FileMode.OpenOrCreate))
                {
                    fileStream.Write(screenshotBuffer, 0, screenshotBuffer.Length);
                }
            }

            yield return null;
            Debug.Log("Delete source file: " + screenshotFilePath);
            File.Delete(screenshotFilePath);

            yield return null;
            Debug.Log("Call Android static method: com.skydragon.gplay.runtime.bridge.CocosRuntimeBridge.captureScreenCallback    param: " + pictureSaveFile);
            AndroidJavaClass javaClass = new AndroidJavaClass("com.skydragon.gplay.runtime.bridge.CocosRuntimeBridge");
            javaClass.CallStatic("captureScreenCallback", pictureSaveFile);
        }
        #endregion
    }
    //void preloadResponse(int resultCode, int errorCode, const std::string& groupName, float percent, float downloadSpeed)
}
