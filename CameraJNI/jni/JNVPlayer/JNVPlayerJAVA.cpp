
#include <jni.h>
#include "jniUtils.h"
#undef	LOG_TAG
#define	LOG_TAG		"JNVPlayer.cpp"


static const char* const kClassPathName = "com/jniUtil/JNVPlayerUtil";  //根据java绑定的类指定

JavaVM*			g_jvm = NULL;						// 全局保存虚拟机
CJNVPlayerMng	g_jnvMng;							// 网络连接


// 系统操作 begin
// 更新CPU信息,返回CPU个数,<=0表示错误
long JNV_UpdateCpuInfo(JNIEnv *env, jobject thiz)
{
	if ( CCPUInfo::s_cpuInfo.UpdateState() ) return CCPUInfo::s_cpuInfo.GetCpuCount();
	else return JNETErrOperate;
}
// 获取CPU使用率
long JNV_GetCPUUsage(JNIEnv *env, jobject thiz,int iIndex)
{
	return (long)(CCPUInfo::s_cpuInfo.GetUsage(iIndex)*100);
}
// 返回错误值,见 JNETErr 说明
long JNV_Init(JNIEnv *env, jobject thiz,int iSize)
{
	JTRACE("JNV_Init\r\n");
	
	// 初始化消息处理 begin
	stItemMsg::s_mutexItemMsg.CreateMutex();		// 项目消息锁
	stItemMsg::s_bMsgEnable		= true;				// 启用消息处理
	pthread_create( &(stItemMsg::s_hThreadID), NULL,RunMsgDispath, NULL);
	// 初始化消息处理 end

	if ( iSize<32 )		iSize = 32;
	if ( iSize>1024 )	iSize = 1024;
	g_jnvMng.Init(iSize);
	g_jnvMng.m_iImgRGBFM	= JDRAW_RGB_565;
	g_jnvMng.m_pfDecFunNet	= OnDecFrameNet;
	g_jnvMng.m_pfDecFunLocal= OnDecFrameLocal;
	g_jnvMng.m_pfEventFun	= OnConnEvent;
	g_jnvMng.m_pfPlayInfoFun= OnPlayInfo;

	return JNETErrSuccess;
}
// 返回错误值,见 JNETErr 说明
long JNV_UnInit(JNIEnv *env, jobject thiz)
{
	JTRACE("JNV_UnInit\r\n");

	// 反初始化消息处理 begin
	stItemMsg::s_bMsgEnable		= false;			// 启用消息处理
	void*	tRet	= NULL;
	pthread_join(stItemMsg::s_hThreadID,&tRet);
	stItemMsg::s_mutexItemMsg.CloseMutex();			// 项目消息锁
	// 反初始化消息处理 end

	g_jnvMng.m_pfDecFunNet	= NULL;
	g_jnvMng.m_pfDecFunLocal= NULL;
	g_jnvMng.m_pfEventFun	= NULL;
	g_jnvMng.m_pfPlayInfoFun= NULL;
	g_jnvMng.Clean();
	stCBParam::Clear(-1);

	return JNETErrSuccess;
}
// 系统操作 end

// 登录相关 begin
// 返回登录句柄(>=0)或错误值(见 JNETErr 说明)
// fcbJavaEvent:事件回调函数
long JNV_N_Login(JNIEnv *env, jobject thiz,jstring lpszIP,int iDevPort,jstring lpszUser,jstring lpszPassword,int iTimeOut,
		jobject Target ,jstring fcbJavaEvent,long lUserParam)
{
	PSTCBParam	pstCBParam	= NULL;
	CJNVString	strIP(env,&lpszIP);
	CJNVString	strUser(env,&lpszUser);
	CJNVString	strPsw(env,&lpszPassword);
	CJNVString	strFunName(env,&fcbJavaEvent);
	JTRACE("JNV_N_Login %s,%d,%s,%s\r\n",(LPCTSTR)strIP,iDevPort,(LPCTSTR)strUser,(LPCTSTR)strPsw);
	// 增加映射
	pstCBParam = stCBParam::AddNewCB(JNV_CB_T_EVENT,JNV_EVENT_ID,env,Target,strFunName,lUserParam);
	if ( pstCBParam==NULL ) return JNETErrNoMem;
	return g_jnvMng.Login(strIP,iDevPort,strUser,strPsw,iTimeOut);
}

// 返回错误值,见 JNETErr 说明
long JNV_N_Logout(JNIEnv *env, jobject thiz)
{
	JTRACE("JNV_N_Logout\r\n");
	return g_jnvMng.Logout();
}
// 登录相关 end

// 请求设备列表:0,表示成功;其他,表示失败
// 返回错误值,见 JNETErr 说明
long JNV_N_GetDevList(JNIEnv *env, jobject thiz,jstring lpszFilePath)
{
	JTRACE("JNV_N_GetDevList\r\n");
	CJNVString	strFilePath(env,&lpszFilePath);
	return g_jnvMng.GetDevList(strFilePath);
}

// 请求服务器列表:0,表示成功;其他,表示失败
// 返回错误值,见 JNETErr 说明
long JNV_N_GetServerList(JNIEnv *env, jobject thiz,jstring lpszFilePath, int nServerType)
{
	JTRACE("JNV_N_GetServerList\r\n");
	CJNVString	strFilePath(env,&lpszFilePath);
	return g_jnvMng.GetServerList(strFilePath, nServerType);
}

// 设置连接服务器类型:0,表示成功;其他,表示失败
// 参数：nConnectServerType  连接服务器类型  0 连单服务器版   1 连级联版本
// 返回错误值,见 JNETErr 说明
long JNV_N_SetConnectServerType(JNIEnv *env, jobject thiz,int nConnectServerType)
{
	JTRACE("JNV_N_SetServerVersion\r\n");
	return g_jnvMng.SetConnectServerType(nConnectServerType);
}

// 增加服务器信息:0,表示成功;其他,表示失败
// 参数 lpszServerId 服务器id  
//		lpszServerIp 服务器ip
//		nServerPort  服务器port
//		nServerType  服务器类型（0 中心 1信令 2媒体）
// 返回错误值,见 JNETErr 说明
long JNV_N_AddServerInfo(JNIEnv *env, jobject thiz,jstring lpszServerId, jstring lpszServerIp, int nServerPort, int nServerType)
{
	JTRACE("JNV_N_AddServerInfo\r\n");
	CJNVString	strServerId(env,&lpszServerId);
	CJNVString	strServerIp(env,&lpszServerIp);
	return g_jnvMng.AddServerInfo(strServerId, strServerIp, nServerPort, nServerType);
}
// 报警相关 begin
// 返回错误值,见 JNETErr 说明
long JNV_N_GetAlarmStart(JNIEnv *env, jobject thiz,jstring lpszDevID)
{
	CJNVString	strDevID(env,&lpszDevID);
	JTRACE("JNV_N_GetAlarmStart %s\r\n",(LPCTSTR)strDevID);
	return g_jnvMng.GetAlarmStart(strDevID);
}
// 返回错误值,见 JNETErr 说明
long JNV_N_GetAlarmStop(JNIEnv *env, jobject thiz,jstring lpszDevID)
{
	CJNVString	strDevID(env,&lpszDevID);
	JTRACE("JNV_N_GetAlarmStop %s\r\n",(LPCTSTR)strDevID);
	return g_jnvMng.GetAlarmStop(strDevID);
}
// 报警相关 end

// 对讲相关 begin
long JNV_N_TalkStart(JNIEnv *env, jobject thiz,jstring lpszDevID,int iChn)
{
	CJNVString	strDevID(env,&lpszDevID);
	JTRACE("JNV_N_TalkStart %s\r\n",(LPCTSTR)strDevID);
	return g_jnvMng.TalkStart(strDevID,iChn);
}
long JNV_N_TalkStop(JNIEnv *env, jobject thiz,long lTalk)
{
	JTRACE("JNV_N_TalkStop %ld\r\n",lTalk);
	return g_jnvMng.TalkStop(lTalk);
}
long JNV_N_TalkSend(JNIEnv *env, jobject thiz,long lTalk,jbyteArray pjBuff,int iLen)
{
	int			iRet;
	uint8_t*	pBuff;
	jbyte*		jBuff	= env->GetByteArrayElements(pjBuff,0);

//	JTRACE("JNV_N_TalkSend %ld,%d\r\n",lTalk,iLen);
	pBuff	= (uint8_t *)jBuff;
	iRet	= g_jnvMng.TalkSend(lTalk,pBuff,iLen);
	env->ReleaseByteArrayElements(pjBuff,jBuff,0);
	return iRet;
}
// 对讲相关 end

// GPS信息相关 begin
// 返回错误值,见 JNETErr 说明
long JNV_N_GetGPSStart(JNIEnv *env, jobject thiz,jstring lpszDevID)
{
	CJNVString	strDevID(env,&lpszDevID);
	JTRACE("JNV_N_GetGPSStart %s\r\n",(LPCTSTR)strDevID);
	return g_jnvMng.GetGPSStart(strDevID);
}
// 返回错误值,见 JNETErr 说明
long JNV_N_GetGPSStop(JNIEnv *env, jobject thiz,jstring lpszDevID)
{
	CJNVString	strDevID(env,&lpszDevID);
	JTRACE("JNV_N_GetGPSStop %s\r\n",(LPCTSTR)strDevID);
	return g_jnvMng.GetGPSStop(strDevID);
}
// GPS信息相关 end

// 录像下载相关 begin
// 返回错误值,见 JNETErr 说明
long JNV_N_RecQuery(JNIEnv *env, jobject thiz,jstring lpszDevID,int iCenter,int iType,jstring lpszTimeStart,jstring lpszTimeEnd,int iChnFlag,jstring lpszFilePath)
{
	CJNVString	strDevID(env,&lpszDevID);
	CJNVString	strTimeStart(env,&lpszTimeStart);
	CJNVString	strTimeEnd(env,&lpszTimeEnd);
	CJNVString	strFilePath(env,&lpszFilePath);
	return g_jnvMng.RecQuery(strDevID,iCenter,iType,strTimeStart,strTimeEnd,iChnFlag,strFilePath);
}
// 返回错误值,见 JNETErr 说明
long JNV_N_RecDownload(JNIEnv *env, jobject thiz,jstring lpszDevID,jstring lpszDevPath,int iStartPos,jstring lpszFilePath)
{
	CJNVString	strDevID(env,&lpszDevID);
	CJNVString	strDevPath(env,&lpszDevPath);
	CJNVString	strFilePath(env,&lpszFilePath);
	return g_jnvMng.RecDownload(strDevID,strDevPath,iStartPos,strFilePath);
}
// 录像下载相关 end

// 云台控制 begin
// 返回错误值,见 JNETErr 说明
// iAction,见 eNVPtzActionType 说明
// iValue1:保留1(x速度/y速度/预置位)
// iValue2:保留2(y速度)
long JNV_N_PtzCtrl(JNIEnv *env, jobject thiz,jstring lpszDevID,int iChn,int iAction,int iValue1,int iValue2)
{
	CJNVString	strDevID(env,&lpszDevID);
	JTRACE("JNV_N_PtzCtrl DevID=%s,Chn=%d,iAction=%d,iValue1=%d,iValue2=%d\r\n",(LPCTSTR)strDevID,iChn,iAction,iValue1,iValue2);
	return g_jnvMng.PtzCtrl(strDevID,iChn,iAction,iValue1,iValue2);
}
// 云台控制 end

// 实时视频相关 begin
// 开始实时视频流,设备ID,通道编号(1开始),码流编号(保留),码流类型(保留)
// 返回流句柄(>=0)或错误值(见 JNETErr 说明)
// fcbJavaDec:解码事件回调函数
long JNV_OpenStream(JNIEnv *env, jobject thiz,jstring lpszDevID,int iChn,int iStream,int iType,
		jobject Target ,jstring fcbJavaDec,long lUserParam)
{
	CJNVString	strDevID(env,&lpszDevID);
	CJNVString	strFunName(env,&fcbJavaDec);
	long		lStreamID	= 0;
	JTRACE("JNVPlayerJava lpszDev=%s,iChn=%d,iStream=%d,iType=%d\r\n",(LPCTSTR)strDevID,iChn,iStream,iType);

	lStreamID = g_jnvMng.RealStreamStart(strDevID,iChn,iStream,iType);
	if ( lStreamID>=0 )
	{
		// 成功,增加映射
		stCBParam::AddNewCB(JNV_CB_T_DEC_NET,lStreamID,env,Target,strFunName,lUserParam);
	}
	else
	{
		// 失败,返回
	}
	return lStreamID;
}
// 返回错误值,见 JNETErr 说明
long JNV_StopStream(JNIEnv *env, jobject thiz,long lStream)
{
	long lRet	= 0;
	JTRACE("JNVPlayerJAVA JNV_StopStream() lStream=%ld\r\n",lStream);
	lRet = g_jnvMng.RealStreamStop(lStream);
	stCBParam::Clear(lStream);
	return lRet;
}
// 实时视频相关 end

// 远程录像回放相关 begin
// 返回流句柄(>=0)或错误值(见 JNETErr 说明)
// fcbJavaDec:解码事件回调函数
long JNV_ReplayStart(JNIEnv *env, jobject thiz,jstring lpszDevID,int iCenter,int iChn,jstring lpszTimeStart,jstring lpszTimeEnd,int iOnlyIFrame,int iRecType,int iStartPos,
		jobject Target ,jstring fcbJavaDec,long lUserParam)
{
	CJNVString	strDevID(env,&lpszDevID);
	CJNVString	strTimeStart(env,&lpszTimeStart);
	CJNVString	strTimeEnd(env,&lpszTimeEnd);
	CJNVString	strFunName(env,&fcbJavaDec);
	long		lStreamID	= 0;
	JTRACE("JNV_ReplayStart lpszDev=%s,iChn=%d\r\n",(LPCTSTR)strDevID,iChn);

	lStreamID = g_jnvMng.ReplayStart(strDevID,iCenter,iChn,strTimeStart,strTimeEnd,iOnlyIFrame,iRecType,iStartPos);
	if ( lStreamID>=0 )
	{
		// 成功,增加映射
		stCBParam::AddNewCB(JNV_CB_T_DEC_NET,lStreamID,env,Target,strFunName,lUserParam);
	}
	else
	{
		// 失败,返回
	}
	return lStreamID;
}
// 返回错误值,见 JNETErr 说明
// iCtrlType,见 eNVReplayType 说明
long JNV_ReplayCtrl(JNIEnv *env, jobject thiz,long lStream,int iCtrlType,int iValue)
{
	JTRACE("JNV_ReplayCtrl lStream=%ld,iCtrlType=%d,iValue=%d\r\n",lStream,iCtrlType,iValue);
	return g_jnvMng.ReplayCtrl(lStream,iCtrlType,iValue);
}
// 返回错误值,见 JNETErr 说明
long JNV_ReplayStop(JNIEnv *env, jobject thiz,long lStream)
{	
	long lRet	= 0;
	JTRACE("JNV_ReplayStop lStream=%ld\r\n",lStream);
	lRet = g_jnvMng.ReplayStop(lStream);
	stCBParam::Clear(lStream);
	return lRet;
}
// 远程录像回放相关 end

// 解码操作 begin
// 抓图
// 返回错误值,见 JNETErr 说明
long JNV_Capture(JNIEnv *env, jobject thiz,long lStream,jstring lpszPath)
{
	CJNVString	strPath(env,&lpszPath);
	return g_jnvMng.Capture(lStream,strPath);
}
// 开始录像
// 返回错误值,见 JNETErr 说明
long JNV_RecStart(JNIEnv *env, jobject thiz,long lStream,jstring lpszPath)
{
	CJNVString	strPath(env,&lpszPath);
	return g_jnvMng.RecStart(lStream,strPath);
}
// 停止录像
// 返回错误值,见 JNETErr 说明
long JNV_RecStop(JNIEnv *env, jobject thiz,long lStream)
{
	return g_jnvMng.RecStop(lStream);
}
// 解码操作 end

// 本地录像回放相关 begin
// 返回流句柄(>=0)或错误值(见 JNETErr 说明)
// fcbJavaDec:解码事件回调函数
// fcbJavaPlayInfo:回放信息回调函数
long JNV_RecOpenFile(JNIEnv *env, jobject thiz,jstring lpszPath,
		jobject Target ,jstring fcbJavaDec,jstring fcbJavaPlayInfo,long lUserParam)
{
	CJNVString	strPath(env,&lpszPath);
	CJNVString	strFunName1(env,&fcbJavaDec);
	CJNVString	strFunName2(env,&fcbJavaPlayInfo);
	long		lRecFile	= 0;
	JTRACE("JNV_RecOpenFile lpszPath=%s\r\n",(LPCTSTR)strPath);

	lRecFile = g_jnvMng.RecOpenFile(strPath);
	if ( lRecFile>=0 )
	{
		// 成功,增加映射
		stCBParam::AddNewCB(JNV_CB_T_DEC_LOCAL,lRecFile,env,Target,strFunName1,lUserParam);
		stCBParam::AddNewCB(JNV_CB_T_PLAY_INFO,lRecFile,env,Target,strFunName2,lUserParam);
	}
	else
	{
		// 失败,返回
	}
	return lRecFile;
}
// 返回错误值,见 JNETErr 说明
long JNV_RecCloseFile(JNIEnv *env, jobject thiz,long lRecFile)
{
	long lRet	= 0;
	JTRACE("JNV_RecCloseFile lRecFile=%ld\r\n",lRecFile);
	lRet = g_jnvMng.RecCloseFile(lRecFile);
	stCBParam::Clear(lRecFile);
	return lRet;
}
// 返回错误值,见 JNETErr 说明
long JNV_RecPlayStart(JNIEnv *env, jobject thiz,long lRecFile)
{
	JTRACE("JNV_RecPlayStart lRecFile=%ld\r\n",lRecFile);
	return g_jnvMng.RecPlayStart(lRecFile);
}
// 返回错误值,见 JNETErr 说明
long JNV_RecPlayPause(JNIEnv *env, jobject thiz,long lRecFile)
{
	JTRACE("JNV_RecPlayPause lRecFile=%ld\r\n",lRecFile);
	return g_jnvMng.RecPlayPause(lRecFile);
}
// 返回错误值,见 JNETErr 说明
long JNV_RecPlaySetSpeed(JNIEnv *env, jobject thiz,long lRecFile,int iSpeed)
{
	JTRACE("JNV_RecPlaySetSpeed lRecFile=%ld\r\n",lRecFile);
	return g_jnvMng.RecPlaySetSpeed(lRecFile,iSpeed);
}
// 返回速度值 见 AVPPlaySpeed 说明
long JNV_RecPlayGetSpeed(JNIEnv *env, jobject thiz,long lRecFile)
{
	JTRACE("JNV_RecPlayGetSpeed lRecFile=%ld\r\n",lRecFile);
	return g_jnvMng.RecPlayGetSpeed(lRecFile);
}
// 返回错误值,见 JNETErr 说明
long JNV_RecPlayNextFrame(JNIEnv *env, jobject thiz,long lRecFile)
{
	JTRACE("JNV_RecPlayNextFrame lRecFile=%ld\r\n",lRecFile);
	return g_jnvMng.RecPlayNextFrame(lRecFile);
}
// 返回错误值,见 JNETErr 说明
long JNV_RecPlaySeek(JNIEnv *env, jobject thiz,long lRecFile,long lTime)
{
	JTRACE("JNV_RecPlaySeek lRecFile=%ld,lTime=%ld\r\n",lRecFile,lTime);
	return g_jnvMng.RecPlaySeek(lRecFile,lTime);
}
// 本地录像回放相关 end
// 测试 begin
jbyteArray JNV_TEST(JNIEnv *env, jobject thiz,long lStream)
{
	jboolean	isCopy	= JNI_FALSE;
	jbyteArray	array	= env->NewByteArray(1843200);			// 定义数据变量

	long		lTick	= JGetTickCount();
	jbyte*		pBuf	= env->GetByteArrayElements(array,&isCopy); 
	strcpy((char*)pBuf,"abcd"); 
    env->ReleaseByteArrayElements(array,pBuf,JNI_COMMIT);  
	JTRACE("JNV_TEST %ld\r\n",JGetTickCount()-lTick);
	return array;
}
// 测试 end


static JNINativeMethod gMethods[] = 
{
	// 系统操作
 	{"JNV_UpdateCpuInfo",		"()I",								(void *)JNV_UpdateCpuInfo},
 	{"JNV_GetCPUUsage",    		"(I)I",								(void *)JNV_GetCPUUsage},
 	{"JNV_Init",        		"(I)I",								(void *)JNV_Init},
 	{"JNV_UnInit",				"()I",								(void *)JNV_UnInit},
	// 登录相关
 	{"JNV_N_Login",				"(Ljava/lang/String;ILjava/lang/String;Ljava/lang/String;ILjava/lang/Object;Ljava/lang/String;I)I",	(void *)JNV_N_Login},
 	{"JNV_N_Logout",			"()I",								(void *)JNV_N_Logout},
	// 请求设备列表
 	{"JNV_N_GetDevList",		"(Ljava/lang/String;)I",			(void *)JNV_N_GetDevList},
	// 请求服务器列表
	{"JNV_N_GetServerList",				"(Ljava/lang/String;I)I",			(void *)JNV_N_GetServerList},
	//设置连接服务器类型
	{"JNV_N_SetConnectServerType",		"(I)I",								(void *)JNV_N_SetConnectServerType},
	//增加服务器信息
	{"JNV_N_AddServerInfo",			    "(Ljava/lang/String;Ljava/lang/String;II)I",			(void *)JNV_N_AddServerInfo},
	// 报警相关
 	{"JNV_N_GetAlarmStart",		"(Ljava/lang/String;)I",			(void *)JNV_N_GetAlarmStart},
 	{"JNV_N_GetAlarmStop",		"(Ljava/lang/String;)I",			(void *)JNV_N_GetAlarmStop},
	// GPS信息相关
 	{"JNV_N_GetGPSStart",		"(Ljava/lang/String;)I",			(void *)JNV_N_GetGPSStart},
 	{"JNV_N_GetGPSStop",		"(Ljava/lang/String;)I",			(void *)JNV_N_GetGPSStop},
	// 对讲相关
 	{"JNV_N_TalkStart",			"(Ljava/lang/String;I)I",			(void *)JNV_N_TalkStart},
 	{"JNV_N_TalkStop",			"(I)I",								(void *)JNV_N_TalkStop},
 	{"JNV_N_TalkSend",			"(I[BI)I",							(void *)JNV_N_TalkSend},
	// 录像下载相关
 	{"JNV_N_RecQuery",			"(Ljava/lang/String;IILjava/lang/String;Ljava/lang/String;ILjava/lang/String;)I",	(void *)JNV_N_RecQuery},
 	{"JNV_N_RecDownload",		"(Ljava/lang/String;Ljava/lang/String;ILjava/lang/String;)I",						(void *)JNV_N_RecDownload},
	// 云台
	{"JNV_N_PtzCtrl",			"(Ljava/lang/String;IIII)I",		(void *)JNV_N_PtzCtrl},
	// 实时视频相关
 	{"JNV_OpenStream",			"(Ljava/lang/String;IIILjava/lang/Object;Ljava/lang/String;I)I",(void *)JNV_OpenStream},
 	{"JNV_StopStream",			"(I)I",								(void *)JNV_StopStream},
	// 远程录像回放相关
 	{"JNV_ReplayStart",			"(Ljava/lang/String;IILjava/lang/String;Ljava/lang/String;IIILjava/lang/Object;Ljava/lang/String;I)I",	(void *)JNV_ReplayStart},
 	{"JNV_ReplayCtrl",			"(III)I",							(void *)JNV_ReplayCtrl},
 	{"JNV_ReplayStop",			"(I)I",								(void *)JNV_ReplayStop},
	// 解码操作
 	{"JNV_Capture",				"(ILjava/lang/String;)I",			(void *)JNV_Capture},
 	{"JNV_RecStart",			"(ILjava/lang/String;)I",			(void *)JNV_RecStart},
 	{"JNV_RecStop",				"(I)I",								(void *)JNV_RecStop},
	// 本地录像回放相关
 	{"JNV_RecOpenFile",			"(Ljava/lang/String;Ljava/lang/Object;Ljava/lang/String;Ljava/lang/String;I)I",	(void *)JNV_RecOpenFile},
 	{"JNV_RecCloseFile",		"(I)I",								(void *)JNV_RecCloseFile},
 	{"JNV_RecPlayStart",		"(I)I",								(void *)JNV_RecPlayStart},
 	{"JNV_RecPlayPause",		"(I)I",								(void *)JNV_RecPlayPause},
 	{"JNV_RecPlaySetSpeed",		"(II)I",							(void *)JNV_RecPlaySetSpeed},
 	{"JNV_RecPlayGetSpeed",		"(I)I",								(void *)JNV_RecPlayGetSpeed},
 	{"JNV_RecPlayNextFrame",	"(I)I",								(void *)JNV_RecPlayNextFrame},
 	{"JNV_RecPlaySeek",			"(II)I",							(void *)JNV_RecPlaySeek},
	// 测试
 	{"JNV_TEST",				"(I)[B",							(void *)JNV_TEST},
};

int register_JNVPlayer(JNIEnv *env) 
{
	JTRACE("load interface in JNVPlayer.cpp");
	return jniRegisterNativeMethods(env, kClassPathName, gMethods, sizeof(gMethods) / sizeof(gMethods[0]));
}



