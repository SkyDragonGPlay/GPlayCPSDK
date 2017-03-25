using UnityEngine;
using UnityEditor;
using System.Xml.Serialization;
using System.Collections.Generic;
using System.IO;
using System.Linq;

namespace GPlay
{
    public class ResourceConfigUtility
    {
        private static string PROJECT_PATH = Path.GetDirectoryName(Application.dataPath) + "/";
        private static float sProgress = 0;

        internal static ResConfig Generate(List<string> scenePaths)
        {
            sProgress = 0;
            ResConfig resConfig = new ResConfig();

            #region 提取所有Resources下的文件
            SetProgress("收集Resources信息...", sProgress);
            var resItemQuery = from assetPath in Directory.GetFiles(Application.dataPath, "*.*", SearchOption.AllDirectories)
                               where (!assetPath.EndsWith(".meta")) && IsResourceFile(assetPath)
                               select new AssetItem(EAssetType.Resources,
                                                    AssetDatabase.AssetPathToGUID(assetPath.Replace('\\', '/').Replace(PROJECT_PATH, "")),
                                                    assetPath.Replace('\\', '/').Replace(PROJECT_PATH, ""));
            foreach (AssetItem resItem in resItemQuery)
            {
                if(!IsContainsItemGUID(resConfig.lstAssetItems, resItem.guid))
                    resConfig.lstAssetItems.Add(resItem);
            }

            SetProgress("收集Resources信息...", sProgress + 0.05f);
            #endregion

            #region 提取所有StreamingAssets下的文件
            SetProgress("收集StreamingAssets信息...", sProgress);
            var streamingAssetItemQuery = from assetPath in Directory.GetFiles(Application.streamingAssetsPath, "*.*", SearchOption.AllDirectories)
                               where (!assetPath.EndsWith(".meta"))
                               select new AssetItem(EAssetType.Streaming,
                                                    AssetDatabase.AssetPathToGUID(assetPath.Replace('\\', '/').Replace(PROJECT_PATH, "")),
                                                    assetPath.Replace('\\', '/').Replace(PROJECT_PATH, ""));
            
            foreach (AssetItem streamingAsset in streamingAssetItemQuery)
            {
                if (!IsContainsItemGUID(resConfig.lstAssetItems, streamingAsset.guid))
                    resConfig.lstAssetItems.Add(streamingAsset);
            }
            SetProgress("收集StreamingAssets信息...", sProgress + 0.05f);
            #endregion

            #region 提取场景引用的资源
            SetProgress("收集场景引用信息...", 0.1f);
            for (int sceneID = 0; sceneID < scenePaths.Count; ++sceneID)
            {
                SceneResCfg sceneResCfg = new SceneResCfg()
                {
                    sceneId = sceneID,
                    sceneName = scenePaths[sceneID]
                };
                
                string[] dependentAssets = AssetDatabase.GetDependencies(scenePaths[sceneID]).Where(item => !item.EndsWith(".cs") && !item.EndsWith(".js")).ToArray();
                foreach (string asset in dependentAssets)
                {
                    string guid = AssetDatabase.AssetPathToGUID(asset);
                    AssetItem assetItem = new AssetItem(EAssetType.Reference, guid, asset);

                    if (!sceneResCfg.lstReferencedResGUID.Contains(assetItem.guid))
                        sceneResCfg.lstReferencedResGUID.Add(assetItem.guid);

                    if (!IsContainsItemGUID(resConfig.lstAssetItems, assetItem.guid))
                        resConfig.lstAssetItems.Add(assetItem);
                }
                resConfig.lstSceneResCfg.Add(sceneResCfg);

                SetProgress("收集场景引用信息...", sProgress + 0.2f / scenePaths.Count);
            }
            #endregion

            #region 提取资源引用关系
            ExtractReferenceInfo(resConfig, resConfig.lstAssetItems);
            #endregion

            EditorUtility.ClearProgressBar();
            return resConfig;
        }

        /// <summary>
        /// 提取资源文件的引用关系
        /// </summary>
        /// <param name="resConfig"></param>
        /// <param name="lstAssetItem"></param>
        private static void ExtractReferenceInfo(ResConfig resConfig, List<AssetItem> lstAssetItem)
        {
            SetProgress("收集文件引用信息...", 0.3f);
            for (int i = 0; i < lstAssetItem.Count; ++i)
            {
                AssetItem assetItem = lstAssetItem[i];

                var referenceInfo = new AssetReferenceInfo() { guid = assetItem.guid };
                string[] dependentAsset = AssetDatabase.GetDependencies(assetItem.path).Where(item => !item.EndsWith(".cs") && !item.EndsWith(".js")).ToArray(); ;
                foreach (string asset in dependentAsset)
                {
                    string guid = AssetDatabase.AssetPathToGUID(asset);
                    if (!referenceInfo.lstReferences.Contains(guid))
                        referenceInfo.lstReferences.Add(guid);

                    if (!IsContainsItemGUID(resConfig.lstAssetItems, guid))
                    {
                        AssetItem referenceItem = new AssetItem(EAssetType.Reference, guid, asset);
                        resConfig.lstAssetItems.Add(referenceItem);
                    }
                }
                if (referenceInfo.lstReferences.Count > 0)
                    resConfig.lstAssetReferenceInfos.Add(referenceInfo);

                SetProgress("收集文件引用信息...", sProgress + 0.7f / lstAssetItem.Count);
            }
        }

        /// <summary>
        /// 是否是Resources文件夹下的文件
        /// </summary>
        /// <param name="filePath"></param>
        /// <returns></returns>
        private static bool IsResourceFile(string filePath)
        {
            string tmp = filePath.Remove(0, Application.dataPath.Length).Replace('\\', '/');
            if (tmp.Contains("/Resources/"))
                return true;
            return false;
        }

        /// <summary>
        /// AssetItem 列表中是否含有 某个 guid 的AssetItem
        /// </summary>
        /// <param name="lstAssetItem"></param>
        /// <param name="guid"></param>
        /// <returns></returns>
        private static bool IsContainsItemGUID(List<AssetItem> lstAssetItem, string guid)
        {
            for (int i = 0; i < lstAssetItem.Count; ++i)
                if (lstAssetItem[i].guid == guid)
                    return true;
            return false;
        }

        private static void SetProgress(string info, float progress)
        {
            sProgress = progress;
            EditorUtility.DisplayProgressBar("收集资源信息", info, sProgress);
        }
    }

    public class ResConfig
    {
        [XmlArray("AssetItemList")]
        public List<AssetItem> lstAssetItems = new List<AssetItem>();
        
        [XmlArray("SceneResourceConfigList")]
        public List<SceneResCfg> lstSceneResCfg = new List<SceneResCfg>();

        [XmlArray("AssetReferenceInfoList")]
        public List<AssetReferenceInfo> lstAssetReferenceInfos = new List<AssetReferenceInfo>();
    }

    public class SceneResCfg
    {
        [XmlAttribute("scene_id")]
        public int sceneId;

        [XmlAttribute("scene_name")]
        public string sceneName;

        [XmlArray("ReferencedResGUIDList")]
        public List<string> lstReferencedResGUID = new List<string>();
    }

    public enum EAssetType
    {
        Resources,
        Streaming,
        Reference,
    }

    public class AssetItem
    {
        [XmlAttribute] public EAssetType type;
        [XmlAttribute] public string guid;
        [XmlAttribute] public string path;
        [XmlArray("SubAssetList")] public List<SubAsset> lstSubAssets;

        public AssetItem() { }
        public AssetItem(EAssetType type, string guid, string path)
        {
            this.type = type;
            this.guid = guid;
            this.path = path;
        }

        public override string ToString()
        {
            return string.Format("{{ type: {0}, guid: {1}, name: {2}}}", type, guid, path);
        }
    }

    public class SubAsset
    {
        [XmlAttribute] public string name;
        [XmlAttribute] public string type;
    }

    public class AssetReferenceInfo
    {
        [XmlElement] public string guid;
        [XmlArray("References")] public List<string> lstReferences = new List<string>();
    }
}
