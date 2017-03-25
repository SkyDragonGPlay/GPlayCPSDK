using System.Collections.Generic;

namespace GPlay
{
    public delegate void ActionCallback(EActionResultCode resultCode, string msg);
    internal class ActionCallbackManager
    {
        public int callbackID = 0;
        private Dictionary<int, ActionCallback> actionCallbacks = new Dictionary<int, ActionCallback>();

        private static ActionCallbackManager instance;
        public static ActionCallbackManager Instance
        {
            get
            {
                if (instance == null)
                    instance = new ActionCallbackManager();
                return instance;
            }
        }
        private ActionCallbackManager() { }

        public int AddCallback(ActionCallback callback)
        {
            GPlaySDK.LogErrorFormat("ActionCallbackManager.AddCallback", "callbackID: {0}", callbackID);
            actionCallbacks[callbackID] = callback;
            return callbackID++;
        }

        public void Invoke(int callbackID, EActionResultCode resultCode, string msg)
        {
            GPlaySDK.LogFormat("ActionCallbackManager.Invoke", "callbackID: {0}  result:{1}  msg:{2}", callbackID, resultCode, msg);
            if (actionCallbacks.ContainsKey(callbackID))
            {
                if (actionCallbacks[callbackID] != null)
                    actionCallbacks[callbackID].Invoke(resultCode, msg);
                else
                    GPlaySDK.LogErrorFormat("ActionCallbackManager.Invoke", "callbackID callback({0}) is null", callbackID);

                actionCallbacks.Remove(callbackID);
            }
            else
            {
                GPlaySDK.LogErrorFormat("ActionCallbackManager.Invoke", "callbackID({0}) not exists", callbackID);
            }
        }
    }
}
