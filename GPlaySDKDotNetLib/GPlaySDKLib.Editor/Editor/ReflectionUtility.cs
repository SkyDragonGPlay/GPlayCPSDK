using UnityEngine;
using System.Collections;
using System.Reflection;
using UnityEditor;
using System;

public class ReflectionUtility
{
    public static Type GetType(string typeName)
    {
        Type type = null;
        Assembly[] assemblies = AppDomain.CurrentDomain.GetAssemblies();
        foreach (var assembly in assemblies)
        {
            type = assembly.GetType(typeName);
            if (null != type)
                break;
        }
        return type;
    }

    public static T GetPropertyValue<T>(string typeName, string propertyName)
    {
        Type type = null;
        Assembly[] assemblies = AppDomain.CurrentDomain.GetAssemblies();
        foreach(var assembly in assemblies)
        {
            type = assembly.GetType(typeName);
            if (null != type)
                break;
        }
        if(null != type)
        {
            return GetPropertyValue<T>(type, null, propertyName);
        }

        return default(T);
    }

    /// <summary>
    /// 反射得到属性值
    /// </summary>
    /// <typeparam name="T">属性类型</typeparam>
    /// <param name="type">属性所在的类型</param>
    /// <param name="obj">类型实例，若是静态属性，则obj传null即可</param>
    /// <param name="propertyName">属性名</param>
    /// <returns>属性值</returns>
    public static T GetPropertyValue<T>(Type type, object obj, string propertyName)
    {
        T result = default(T);
        PropertyInfo propertyInfo = type.GetProperty(propertyName);
        if (null != propertyInfo)
        {
            result = (T)propertyInfo.GetValue(obj, null);
        }
        return result;
    }

    // AssetDatabase.GetInstanceIDFromGUID
    public static int GetInstanceIDFromGUID(string guid)
    {
        int result = 0;
        MethodInfo miGetInstanceIDFromGUID = typeof(AssetDatabase).GetMethod("GetInstanceIDFromGUID", BindingFlags.Static | BindingFlags.NonPublic);
        if (null != miGetInstanceIDFromGUID)
            result = (int)miGetInstanceIDFromGUID.Invoke(null, new object[] { guid });
        return result;
    }

    // EditorGUI.ToolbarSearchField
    public static string ToolbarSearchField(int id, Rect position, string text, bool showWithPopupArrow)
    {
        MethodInfo miToolbarSearchField = typeof(EditorGUI).GetMethod("ToolbarSearchField",
            BindingFlags.Static | BindingFlags.NonPublic, Type.DefaultBinder,
            new Type[] { typeof(int), typeof(Rect), typeof(string), typeof(bool) }, null);
        if (null != miToolbarSearchField)
            text = miToolbarSearchField.Invoke(null, new object[] { id, position, text, showWithPopupArrow }) as string;
        return text;
    }

    // EditorGUIUtility.HandleHorizontalSplitter
    public static Rect HandleHorizontalSplitter(Rect dragRect, float width, float minLeftSide, float minRightSide)
    {
        Rect result = default(Rect);
        MethodInfo miHandleHorizontalSplitter = typeof(EditorGUIUtility).GetMethod("HandleHorizontalSplitter", BindingFlags.Static | BindingFlags.NonPublic);
        if (null != miHandleHorizontalSplitter)
            result = (Rect)miHandleHorizontalSplitter.Invoke(null, new object[] { dragRect, width, minLeftSide, minRightSide });
        return result;
    }

    // UnityEditor.BuildPlayerWindow.BuildPlayerWithDefaultSettings
    public static bool BuildPlayerWithDefaultSettings(bool askForBuildLocation, BuildOptions forceOptions)
    {
        Type BuildPlayerWindow = GetType("UnityEditor.BuildPlayerWindow");
        MethodInfo miBuildPlayerWithDefaultSettings = BuildPlayerWindow.GetMethod("BuildPlayerWithDefaultSettings",
            BindingFlags.Static | BindingFlags.NonPublic, Type.DefaultBinder, new Type[] { typeof(bool), typeof(BuildOptions) }, null);
        return (bool)miBuildPlayerWithDefaultSettings.Invoke(null, new object[] { askForBuildLocation, forceOptions });
    }
}
