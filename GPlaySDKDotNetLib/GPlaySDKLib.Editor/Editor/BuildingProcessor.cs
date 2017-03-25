using UnityEngine;
using UnityEditor;
using System.Collections.Generic;
using System.IO;
using UnityEditor.SceneManagement;
using System.Xml.Serialization;

namespace GPlay
{
    public class BuildingProcessor
    {
        [MenuItem("GPlay/Generate GPlayConfig")]
        static void BuildPlayer()
        {
            if (EditorUserBuildSettings.activeBuildTarget != BuildTarget.Android)
            {
                Debug.LogError("请选择 Android 平台后，再次打包!!!");
                return;
            }

            // 将 meta 设置为 text 模式
            if (SerializationMode.ForceText != EditorSettings.serializationMode)
            {
                Debug.LogError("请将 Edit->Project Setting->Asset Serialization->Mode 设置项设置为 ForceText");
                return;
                //EditorSettings.serializationMode = SerializationMode.ForceText;
            }

            string gplayConfigPath = Application.streamingAssetsPath + "/GPlayConfig.xml";
            if (File.Exists(gplayConfigPath))
                File.Delete(gplayConfigPath);

            #region 得到需要打包的场景
            List<string> buildScenes = new List<string>();
            foreach (var buildScene in EditorBuildSettings.scenes)
            {
                if (buildScene.enabled)
                    buildScenes.Add(buildScene.path);
            }

            if (buildScenes.Count == 0)
            {
                UnityEngine.SceneManagement.Scene activeScene = EditorSceneManager.GetActiveScene();
                if (string.IsNullOrEmpty(activeScene.path))
                {
                    //EditorApplication.ExecuteMenuItem("File/BuildSettings");
                    EditorWindow.GetWindow(ReflectionUtility.GetType("UnityEditor.BuildPlayerWindow"), true, "Build Settings");
                    Debug.Log("请在BuildSetting中添加要打包的场景");
                    return;
                }
                else
                    buildScenes.Add(activeScene.path);
            }
            #endregion

            #region 生成配置文件
            if (!Directory.Exists(Application.streamingAssetsPath))
                Directory.CreateDirectory(Application.streamingAssetsPath);

            var gplayConfig = new GPlayConfig() { unityVersion = Application.unityVersion };

            // 生成资源配置信息
            gplayConfig.resConfig = ResourceConfigUtility.Generate(buildScenes);

            SerializeUtility.XmlSerializeToFile<GPlayConfig>(gplayConfig, Application.streamingAssetsPath + "/GPlayConfig.xml");

            AssetDatabase.Refresh();
            //processWindow.Close();
            #endregion

            //// 设置为非开发模式
            //EditorUserBuildSettings.development = false;
            //ReflectionUtility.BuildPlayerWithDefaultSettings(true, BuildOptions.ShowBuiltPlayer);

            //// 删除配置文件 
            //AssetDatabase.DeleteAsset("Assets/StreamingAssets/GPlayConfig.xml");

            #region comment
            //string path = EditorUtility.SaveFolderPanel("Choose Location of Built Game", "", "");
            //path = Path.Combine(path, GetBuildFileName());

            //BuildOptions options = BuildOptions.None;
            //if (EditorUserBuildSettings.allowDebugging && EditorUserBuildSettings.development)
            //    options |= BuildOptions.AllowDebugging;
            //if (EditorUserBuildSettings.development)
            //    options |= BuildOptions.Development;
            //if (EditorUserBuildSettings.connectProfiler)
            //    options |= BuildOptions.ConnectWithProfiler;

            //BuildPipeline.BuildPlayer(buildScenes.ToArray(), path, EditorUserBuildSettings.activeBuildTarget, options);
            #endregion
        }

        //[PostProcessBuildAttribute(1)]
        //static void OnProcessBuild(BuildTarget target, string pathToBuildProject)
        //{
        //    Debug.Log(System.DateTime.Now.ToString("hh:MM:ss") + "    " + pathToBuildProject);
        //}

        private static string GetBuildFileName()
        {
            string extention = string.Empty;
            switch (EditorUserBuildSettings.activeBuildTarget)
            {
                case BuildTarget.Android: extention = ".apk"; break;
                case BuildTarget.StandaloneWindows:
                case BuildTarget.StandaloneWindows64:
                    extention = ".exe";
                    break;
            }

            return PlayerSettings.productName + extention;
        }

        #region Xml 实体类
        public class GPlayConfig
        {
            [XmlAttribute("unity_version")]
            public string unityVersion;
            [XmlAttribute("sdk_version")]
            public string sdkVersion = "1.0.0";
            [XmlAttribute("engine")]
            public string engine = "unity";

            [XmlElement("SettingConfig")]
            public SettingConfig settingConfig = new SettingConfig();

            [XmlElement("ResConfig")]
            public ResConfig resConfig;
        }
        #endregion
    }
}
