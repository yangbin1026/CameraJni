LOCAL_PATH := $(call my-dir)
include $(CLEAR_VARS)

IN_NDK := true

LOCAL_CFLAGS := -D__STDC_CONSTANT_MACROS
LOCAL_CPPFLAGS += -fexceptions -lgcc_s -lc

WITH_CONVERTOR := true
WITH_PLAYER := true
ANDROID := true

ifeq ($(WITH_PLAYER),true)
LOCAL_CFLAGS += -DBUILD_WITH_PLAYER
endif

ifeq ($(WITH_CONVERTOR),true)
LOCAL_CFLAGS += -DBUILD_WITH_CONVERTOR
endif

ifeq ($(ANDROID),true)
LOCAL_CFLAGS += -DBUILD_ANDROID
endif

LOCAL_C_INCLUDES += \
    $(LOCAL_PATH) \
    $(LOCAL_PATH)/../ffmpeg_android \
    $(LOCAL_PATH)/Common \
    $(LOCAL_PATH)/Net \
    $(LOCAL_PATH)/JJSON \
    $(LOCAL_PATH)/Player/Audio \
    $(LOCAL_PATH)/Player
    
LOCAL_SRC_FILES += \
    jniUtils.cpp \
	ProtocolNV.cpp \
    JNVPlayerJAVA.cpp \
	Common/JGetRunTimeTC.cpp \
	JJSON/JJson.cpp \
	JJSON/json.cpp \
	JJSON/json_helper.cpp \
    Net/JNVProtocolTCP.cpp \
	Net/JNVCmdTcp.cpp \
	Net/JNVStreamTcp.cpp \
	Net/JNVTalkTcp.cpp \
	Player/Audio/g72x.cpp \
	Player/Audio/g711.cpp \
	Player/Audio/g726.cpp \
	Player/Audio/g726_16.cpp \
	Player/Audio/g726_24.cpp \
	Player/Audio/g726_32.cpp \
	Player/Audio/g726_40.cpp \
	Player/Decoder.cpp \
	Player/ImgConver.cpp \
	Player/FMReader.cpp \
	Player/FMReaderJNV.cpp \
	Player/JBuffPool.cpp \
	Player/JRecFile.cpp \
	Player/JNVPlayer.cpp \
	Player/JNVPlayerMng.cpp

ifeq ($(IN_NDK),true)	
LOCAL_LDLIBS :=-llog
else
LOCAL_PRELINK_MODULE := false
LOCAL_SHARED_LIBRARIES := liblog
endif

LOCAL_STATIC_LIBRARIES := libffmpeg libJNVCommon_jni libpostproc

LOCAL_MODULE := libJNVPlayer_jni

include $(BUILD_SHARED_LIBRARY)
