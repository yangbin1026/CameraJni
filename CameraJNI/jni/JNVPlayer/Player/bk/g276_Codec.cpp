#include "g726.h"
#include <jni.h>

#include <android/log.h>
#include <stdlib.h>
#include <stdio.h>


#define  LOG_TAG    "hello-jni.cpp"
#define  LOGE(...)  __android_log_print(ANDROID_LOG_ERROR,LOG_TAG,__VA_ARGS__)

// int main(char argv,char* args){
// 	
// 	int sample;
// 	unsigned char result;
// 
// 	g726_state state;
// 	g726_init_state(&state);
// 
// 	sample = 800;
// 
// 	result = (unsigned char)g726_32_encoder(sample,AUDIO_ENCODING_LINEAR,&state);
// 	printf("result:%d \n",result);
// 	
// 	return 0;
// }

/************************************************************************/
/* 1: success 
 * -1: fail
/************************************************************************/

extern "C" {
	

	JNIEXPORT jint JNICALL	Java_com_gsee360_audio_g726Codec_encode(JNIEnv *env, jclass obj,jshortArray SpeechData, jbyteArray BitstreamData){

		g726_state_t* g_state726_32 = NULL;

		g_state726_32 = (g726_state_t *)malloc(sizeof(g726_state_t));
		g_state726_32 = g726_init(g_state726_32, 32000);

// 		unsigned char ucInBuff [320] = {0};//一帧语音数据
// 		env->GetByteArrayRegion(SpeechData,0,320,(jbyte*)ucInBuff);

		short ucInBuffer [160] = {0};
		env->GetShortArrayRegion(SpeechData,0,160,(jshort*)ucInBuffer);

		//拷贝语音帧数据到NDK层

		unsigned char ucOutBuff[80] = {0};
// 
 		int iRet = g726_encode(g_state726_32, ucOutBuff, ucInBuffer, 160);


// 		for (int i = 0; i < 80; i++)
// 		{
// 			ucOutBuff[i] = g726_32_encoder(g_state726_32,ucInBuffer[i*2])<<4||g726_32_encoder(g_state726_32,ucInBuffer[i*2+1]);
// 		}


		//将NDK层数据转移到JAVA层
		env->SetByteArrayRegion(BitstreamData,0,80,(jbyte*)ucOutBuff);

		free(g_state726_32);
		g_state726_32 = NULL;

		return iRet;
	}

	//解码
	JNIEXPORT jint JNICALL	Java_com_gsee360_audio_g726Codec_decode(JNIEnv *env, jclass obj,jbyteArray SpeechData, jshortArray BitstreamData){

		g726_state_t* g_state726_32 = NULL;

		g_state726_32 = (g726_state_t *)malloc(sizeof(g726_state_t));
		g_state726_32 = g726_init(g_state726_32, 8000*4);

		unsigned char ucInBuff [80] = {0};
		env->GetByteArrayRegion(SpeechData,0,80,(jbyte*)ucInBuff);

//		unsigned char ucOutBuff[320] = {0};

		short ucOutBuff [160] = {0};

		int iRet = g726_decode(g_state726_32,(short*)ucOutBuff,ucInBuff,80);

//		env->SetByteArrayRegion(BitstreamData,0,160,(jbyte*)ucOutBuff);
		
		env->SetShortArrayRegion(BitstreamData,0,160,(jshort*)ucOutBuff);
			

		free(g_state726_32);
		g_state726_32 = NULL;

		return iRet;
	}

}

