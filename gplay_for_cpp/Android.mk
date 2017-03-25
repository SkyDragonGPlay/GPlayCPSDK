LOCAL_PATH := $(call my-dir)

include $(CLEAR_VARS)

LOCAL_MODULE := gplay_prebuilt
LOCAL_MODULE_FILENAME := gplay_prebuilt
LOCAL_SRC_FILES := prebuilt/$(APP_STL)/$(TARGET_ARCH_ABI)/libgplay_static.a

include $(PREBUILT_STATIC_LIBRARY)
#===============================================================================
include $(CLEAR_VARS)

LOCAL_MODULE := gplay_static
LOCAL_MODULE_FILENAME := gplay_static

LOCAL_SRC_FILES := cocos_bridge/cocos_bridge.cpp \
                   cocos_bridge/DivideResourceHelper.cpp \
                   gplay_cpp.cpp

LOCAL_STATIC_LIBRARIES := cocos2dx_static
LOCAL_WHOLE_STATIC_LIBRARIES := gplay_prebuilt

LOCAL_C_INCLUDES := $(LOCAL_PATH)/ \
                    $(LOCAL_PATH)/preload_layer \
                    $(LOCAL_PATH)/cocos_bridge \
                    $(LOCAL_PATH)/../../../Classes

LOCAL_EXPORT_C_INCLUDES := $(LOCAL_PATH)/

include $(BUILD_STATIC_LIBRARY)
