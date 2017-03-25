using UnityEngine;
using System.Collections;
using System.Xml.Serialization;
using System.IO;

namespace GPlay
{
    public class SerializeUtility
    {
        public static void XmlSerializeToFile<T>(T obj, string fileName)
        {
            XmlSerializer serializer = new XmlSerializer(typeof(T));
            using (TextWriter writer = new StreamWriter(fileName))
            {
                serializer.Serialize(writer, obj);
                writer.Close();
            }
        }
    }
}
