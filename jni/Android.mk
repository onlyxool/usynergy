LOCAL_PATH := $(call my-dir)
PKG_CONFIG ?= pkg-config

#INCLUDES = jni

#L_CFLAGS := -mabi=aapcs-linux
#L_CFLAGS += -fpermissive
#L_CFLAGS += -fexceptions
#L_CFLAGS += -fexceptions

include $(CLEAR_VARS)
LOCAL_MODULE := suinput
LOCAL_SRC_FILES := suinput.c
include $(BUILD_STATIC_LIBRARY)

include $(CLEAR_VARS)
LOCAL_MODULE := platform
LOCAL_STATIC_LIBRARIES := libsuinput
LOCAL_SRC_FILES := android.c
include $(BUILD_STATIC_LIBRARY)

include $(CLEAR_VARS)
LOCAL_MODULE := micro
LOCAL_LDLIBS := -lpthread
LOCAL_STATIC_LIBRARIES := libplatform
LOCAL_SRC_FILES := uSynergy.c
include $(BUILD_STATIC_LIBRARY)


include $(CLEAR_VARS)
LOCAL_MODULE := usynergy
LOCAL_STATIC_LIBRARIES := libmicro
LOCAL_SRC_FILES := uSynergyUnix.c
include $(BUILD_EXECUTABLE)

include $(CLEAR_VARS)
LOCAL_MODULE := usynercore
LOCAL_STATIC_LIBRARIES := libmicro
LOCAL_SRC_FILES := Interface.c
include $(BUILD_SHARED_LIBRARY)

#include $(CLEAR_VARS)
#LOCAL_MODULE := usynergy
#LOCAL_MODULE_FILENAME := libusynergy
#LOCAL_SRC_FILES := android.c \
#				   Interface.c \
#				   suinput.c \
#				   uSynergy.c \
#				   uSynergyUnix.c \
#LOCAL_C_INCLUDES := \
#					$(LOCAL_PATH)/jni \
#include $(BUILD_SHARED_LIBRARY)
