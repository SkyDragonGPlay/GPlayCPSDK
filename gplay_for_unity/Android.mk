
LOCAL_PATH := $(call my-dir)

#===============================================================================
include $(CLEAR_VARS)

LOCAL_MODULE := gplay_prebuilt
LOCAL_MODULE_FILENAME := gplay_prebuilt
LOCAL_SRC_FILES := ../common/prebuilt/$(APP_STL)/$(TARGET_ARCH_ABI)/libgplay_static.a

include $(PREBUILT_STATIC_LIBRARY)

#===============================================================================
include $(CLEAR_VARS)

LOCAL_MODULE := gplay_hook
LOCAL_MODULE_FILENAME := gplay_hook
#LOCAL_SRC_FILES := ./gplay_hook/libhook_static.a
LOCAL_SRC_FILES := ./gplay_hook/local/$(TARGET_ARCH_ABI)/libhook_static.a

include $(PREBUILT_STATIC_LIBRARY)

#===============================================================================
include $(CLEAR_VARS)

LOCAL_MODULE := gplay

LOCAL_SRC_FILES := gplay_unity.cpp \
                   unity_bridge/unity_bridge.cpp

LOCAL_WHOLE_STATIC_LIBRARIES := gplay_prebuilt gplay_hook
LOCAL_LDLIBS:=-llog

LOCAL_C_INCLUDES := $(LOCAL_PATH) \
					../common


include $(BUILD_SHARED_LIBRARY)





