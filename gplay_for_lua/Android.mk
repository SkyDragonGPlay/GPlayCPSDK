LOCAL_PATH := $(call my-dir)

include $(CLEAR_VARS)
LOCAL_MODULE := gplay_prebuilt
LOCAL_MODULE_FILENAME := gplay_prebuilt
LOCAL_SRC_FILES := prebuilt/$(APP_STL)/$(TARGET_ARCH_ABI)/libgplay_static.a

include $(PREBUILT_STATIC_LIBRARY)

include $(CLEAR_VARS)
LOCAL_MODULE := gplay_static
LOCAL_MODULE_FILENAME := gplay_static
LOCAL_SRC_FILES := cocos_bridge/cocos_bridge.cpp \
                   cocos_bridge/DivideResourceHelper.cpp \
                   gplay_luab.cpp

#cocos2dx 3.0/3.1/3.2
#LOCAL_STATIC_LIBRARIES := cocos_lua_static

#cocos2dx 3.3+
LOCAL_STATIC_LIBRARIES := cocos2d_lua_static

#cocos2dx 2.x
#LOCAL_STATIC_LIBRARIES += cocos_lua_static

#quick lua 2.x
#LOCAL_STATIC_LIBRARIES += quickcocos2dx

LOCAL_WHOLE_STATIC_LIBRARIES := gplay_prebuilt

LOCAL_C_INCLUDES := $(LOCAL_PATH)/ \
                    $(LOCAL_PATH)/preload_layer \
                    $(LOCAL_PATH)/cocos_bridge \
                    $(LOCAL_PATH)/../../../Classes
                    
LOCAL_EXPORT_C_INCLUDES := $(LOCAL_PATH)/

include $(BUILD_STATIC_LIBRARY)
