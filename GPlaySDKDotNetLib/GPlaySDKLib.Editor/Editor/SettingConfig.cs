using System.Xml.Serialization;
using UnityEditor;

namespace GPlay
{
    public class SettingConfig
    {
        [XmlElement]
        public int scriptingBackend;

        public SettingConfig()
        {
            PlayerSettings.GetPropertyOptionalInt("ScriptingBackend", ref scriptingBackend, BuildTargetGroup.Unknown);
        }
    }
}
