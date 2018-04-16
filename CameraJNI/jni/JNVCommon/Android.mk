LOCAL_PATH := $(call my-dir)
include $(CLEAR_VARS)

LOCAL_CFLAGS := -D__STDC_CONSTANT_MACROS
LOCAL_CPPFLAGS += -fexceptions -lgcc_s -lc

LOCAL_C_INCLUDES += \
    $(LOCAL_PATH) 

LOCAL_SRC_FILES += \
	MutexLockLinux.cpp \
	j_vs_ver.cpp \
	JLogWriter.cpp \
	JLSocket.cpp \
	JLSocketClient.cpp \
	JLTcpClient.cpp \
	JLThreadCtrl.cpp 

LOCAL_LDLIBS := -lGLESv1_CM -llog # 如果不包含这一句的话，会提示：__android_log_print 未定义
LOCAL_MODULE := libJNVCommon_jni

include $(BUILD_SHARED_LIBRARY)
