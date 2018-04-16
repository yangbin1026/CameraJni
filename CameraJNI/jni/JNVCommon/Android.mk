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

LOCAL_LDLIBS := -lGLESv1_CM -llog # �����������һ��Ļ�������ʾ��__android_log_print δ����
LOCAL_MODULE := libJNVCommon_jni

include $(BUILD_SHARED_LIBRARY)
