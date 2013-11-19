LOCAL_PATH := $(call my-dir)

include $(CLEAR_VARS)

LOCAL_MODULE := lua
LOCAL_SRC_FILES := $(subst $(LOCAL_PATH)/,, $(wildcard $(LOCAL_PATH)/src/*.c))

include $(BUILD_STATIC_LIBRARY)
