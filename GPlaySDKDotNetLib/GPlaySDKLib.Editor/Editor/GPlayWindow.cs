using System.IO;
using System.Text;
using UnityEditor;
using UnityEngine;

namespace GPlay
{
    internal class GPlayWindow : EditorWindow
    {
        private string m_StrGUID;
        private string m_assetPath;
        private Object m_assetObj;
        private Object m_pickObj;
        
        //[MenuItem("Window/GPlay Window")]
        //static void Open()
        //{
        //    GetWindow<GPlayWindow>();
        //}

        void Awake()
        {
            titleContent.text = "GPlay";
        }

        void OnGUI()
        {
            #region
            //if (Event.current.type == EventType.Repaint)
            //{
            //    ListInternalIconWindow.style.lineStyle.Draw(new Rect(0, 0, 200, 200), "xxx", true, false, true, false);
            //    GUILayout.Label("xxx");
            //}

            m_StrGUID = EditorGUILayout.TextField("GUID", m_StrGUID);

            if (GUILayout.Button("输出路径"))
            {
                m_assetPath = AssetDatabase.GUIDToAssetPath(m_StrGUID);
                Debug.Log("---------------------- " + m_assetPath + " ----------------------");

                m_assetObj = AssetDatabase.LoadAssetAtPath<Object>(m_assetPath);
                if (m_assetObj != null)
                {
                    Debug.Log("assetObj.ToString   " + m_assetObj.ToString());
                    Debug.Log("assetObj.GetType    " + m_assetObj.GetType());
                    Debug.Log("assetObj.name   " + m_assetObj.name);
                    EditorGUIUtility.PingObject(m_assetObj);
                }
            }

            m_pickObj = EditorGUILayout.ObjectField(m_pickObj, typeof(Object), true);
            if (GUILayout.Button("输出Obj信息并跳转") && m_pickObj != null)
            {
                m_assetObj = m_pickObj;
                m_assetObj.GetInstanceID();
                m_assetPath = AssetDatabase.GetAssetPath(m_assetObj);

                EditorGUIUtility.PingObject(m_assetObj);
            }


            if (GUILayout.Button("跳转", GUILayout.Width(100)) && !string.IsNullOrEmpty(m_StrGUID))
            {
                PingObjectWithGUID(m_StrGUID);
            }

            if (m_assetObj != null)
            {
                string isMainAsset = GUILayout.TextField("Is Main Asset: " + AssetDatabase.IsMainAsset(m_assetObj));
                string isNativeAsset = GUILayout.TextField("Is Native Asset: " + AssetDatabase.IsNativeAsset(m_assetObj));
                string isSubAsset = GUILayout.TextField("Is Sub Asset: " + AssetDatabase.IsSubAsset(m_assetObj));
                string assetPath = GUILayout.TextField("Asset Path: " + m_assetPath);
                string textMetaFilePath = GUILayout.TextField("Text Meta File Path: " + AssetDatabase.GetTextMetaFilePathFromAssetPath(m_assetPath));
                string assetObjType = GUILayout.TextField("Asset Obj Type: " + m_assetObj.GetType());

                StringBuilder sb = new StringBuilder();
                Object[] assetObjs = AssetDatabase.LoadAllAssetRepresentationsAtPath(m_assetPath);
                if (assetObjs != null && assetObjs.Length > 0)
                {
                    for (int i = 0; i < assetObjs.Length; ++i)
                    {
                        if (assetObjs[i].GetType() != typeof(DefaultAsset))
                            sb.Append(assetObjs[i].name).Append("   ").Append(assetObjs[i].GetType().ToString()).Append("\n");
                    }
                }
                string assets = GUILayout.TextArea(sb.ToString());
            }
            #endregion

            CalculateMD5();
        }

        string m_filePath, m_md5;
        void CalculateMD5()
        {
            EditorGUILayout.BeginHorizontal();
            m_filePath = EditorGUILayout.TextField("File Path: ", m_filePath);
            if(GUILayout.Button("生成MD5"))
            {
                if (!File.Exists(m_filePath))
                {
                    m_md5 = "file not exists";
                }
                else
                {
                    using (FileStream inputStream = File.Open(m_filePath, FileMode.Open))
                    {
                        var md5 = System.Security.Cryptography.MD5.Create();
                        var fileMD5Byte = md5.ComputeHash(inputStream);
                        m_md5 = System.BitConverter.ToString(fileMD5Byte).Replace("-", "");
                    }
                }
            }
            EditorGUILayout.EndHorizontal();
            EditorGUILayout.TextField("MD5: ", m_md5);
        }


        public static void PingObjectWithGUID(string guid)
        {
            int instanceID = ReflectionUtility.GetInstanceIDFromGUID(guid);
            EditorGUIUtility.PingObject(instanceID);

            //string assetPath = AssetDatabase.GUIDToAssetPath(strGUID);
            //if(!string.IsNullOrEmpty(assetPath))
            //{
            //    Object assetObj = AssetDatabase.LoadAssetAtPath<Object>(assetPath);
            //    EditorGUIUtility.PingObject(assetObj);
            //}
        }
    }
}
