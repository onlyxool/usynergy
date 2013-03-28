LOCAL_PATH := $(call my-dir)
PKG_CONFIG ?= pkg-config

#INCLUDES = jni

#L_CFLAGS := -mabi=aapcs-linux
#L_CFLAGS += -fpermissive
#L_CFLAGS += -fexceptions
#L_CFLAGS += -fexceptions

#include $(CLEAR_VARS)
#LOCAL_MODULE := mt

#LOCAL_CFLAGS := $(L_CFLAGS)
#LOCAL_SRC_FILES := src/lib/mt/CCondVar.cpp src/lib/mt/CLock.cpp src/lib/mt/CMutex.cpp src/lib/mt/CThread.cpp src/lib/mt/XMT.cpp

#include $(BUILD_STATIC_LIBRARY)

include $(CLEAR_VARS)
LOCAL_MODULE := micro
#LOCAL_C_INCLUDES := $(INCLUDES)
#LOCAL_CFLAGS := $(L_CFLAGS)
LOCAL_SRC_FILES := uSynergy.c
include $(BUILD_STATIC_LIBRARY)


include $(CLEAR_VARS)
LOCAL_MODULE := usynergy
LOCAL_STATIC_LIBRARIES := libmicro
#LOCAL_C_INCLUDES := $(INCLUDES)
#LOCAL_CFLAGS := $(L_CFLAGS)
LOCAL_SRC_FILES := uSynergyUnix.c
include $(BUILD_EXECUTABLE)
