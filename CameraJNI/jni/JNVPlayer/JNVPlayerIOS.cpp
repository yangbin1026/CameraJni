
#include "IOSUtils.h"
#include "JNVPlayerIOS.h"

CJNVPlayerMng	g_jnvMng;							// 网络连接

// 系统操作 begin
// 返回错误值,见 JNETErr 说明
long JNV_Init(int iSize)
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
	g_jnvMng.m_pfDecFunNet	= OnDecFrameNet;
	g_jnvMng.m_pfDecFunLocal= OnDecFrameLocal;
	g_jnvMng.m_pfEventFun	= OnConnEvent;
	g_jnvMng.m_pfPlayInfoFun= OnPlayInfo;
	g_jnvMng.m_iImgRGBFM	= JDRAW_RGB_24;

	return JNETErrSuccess;
}
// 返回错误值,见 JNETErr 说明
long JNV_UnInit()
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
long JNV_N_Login(LPCTSTR lpszIP,int iDevPort,LPCTSTR lpszUser,LPCTSTR lpszPassword,int iTimeOut,
	fcbJNVEvent* pfEventFun,long lUserParam)
{
	PSTCBParam	pstCBParam	= NULL;
	// 增加映射
	pstCBParam = stCBParam::AddNewCB(JNV_CB_T_EVENT,JNV_EVENT_ID,(void*)pfEventFun,lUserParam);
	if ( pstCBParam==NULL ) return JNETErrNoMem;
	return g_jnvMng.Login(lpszIP,iDevPort,lpszUser,lpszPassword,iTimeOut);
}

// 返回错误值,见 JNETErr 说明
long JNV_N_Logout()
{
	JTRACE("JNV_N_Logout\r\n");
	return g_jnvMng.Logout();
}
// 登录相关 end

// 请求设备列表:0,表示成功;其他,表示失败
// 返回错误值,见 JNETErr 说明
long JNV_N_GetDevList(LPCTSTR lpszFilePath)
{
	JTRACE("JNV_N_GetDevList\r\n");
	return g_jnvMng.GetDevList(lpszFilePath);
}

// 请求设备列表:0,表示成功;其他,表示失败
// 返回错误值,见 JNETErr 说明
long JNV_N_GetServerList(LPCTSTR lpszFilePath, int nServerType)
{
	JTRACE("JNV_N_GetServerList\r\n");
	return g_jnvMng.GetServerList(lpszFilePath, nServerType);
}

// 设置连接服务器类型:0,表示成功;其他,表示失败
// 参数：nConnectServerType  连接服务器类型  0 连单服务器版   1 连级联版本
// 返回错误值,见 JNETErr 说明
long JNV_N_SetConnectServerType(int nConnectServerType)
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
long JNV_N_AddServerInfo(LPCTSTR lpszServerId, LPCTSTR lpszServerIp, int nServerPort, int nServerType)
{
	JTRACE("JNV_N_AddServerInfo\r\n");
	return g_jnvMng.AddServerInfo(lpszServerId, lpszServerIp, nServerPort, nServerType);
}
// 报警相关 begin
// 返回错误值,见 JNETErr 说明
long JNV_N_GetAlarmStart(LPCTSTR lpszDevID)
{
	JTRACE("JNV_N_GetAlarmStart %s\r\n",lpszDevID);
	return g_jnvMng.GetAlarmStart(lpszDevID);
}
// 返回错误值,见 JNETErr 说明
long JNV_N_GetAlarmStop(LPCTSTR lpszDevID)
{
	JTRACE("JNV_N_GetAlarmStop %s\r\n",lpszDevID);
	return g_jnvMng.GetAlarmStop(lpszDevID);
}
// 报警相关 end

// 对讲相关 begin
long JNV_N_TalkStart(LPCTSTR lpszDevID,int iChn)
{
	JTRACE("JNV_N_TalkStart %s\r\n",lpszDevID);
	return g_jnvMng.TalkStart(lpszDevID,iChn);
}
long JNV_N_TalkStop(long lTalk)
{
	JTRACE("JNV_N_TalkStop %ld\r\n",lTalk);
	return g_jnvMng.TalkStop(lTalk);
}
long JNV_N_TalkSend(long lTalk,LPBYTE lpBuff,int iLen)
{
	int			iRet;

	JTRACE("JNV_N_TalkSend %ld,%d\r\n",lTalk,iLen);
	iRet	= g_jnvMng.TalkSend(lTalk,lpBuff,iLen);
	return iRet;
}
// 对讲相关 end

// GPS信息相关 begin
// 返回错误值,见 JNETErr 说明
long JNV_N_GetGPSStart(LPCTSTR lpszDevID)
{
	JTRACE("JNV_N_GetGPSStart %s\r\n",lpszDevID);
	return g_jnvMng.GetGPSStart(lpszDevID);
}
// 返回错误值,见 JNETErr 说明
long JNV_N_GetGPSStop(LPCTSTR lpszDevID)
{
	JTRACE("JNV_N_GetGPSStop %s\r\n",lpszDevID);
	return g_jnvMng.GetGPSStop(lpszDevID);
}
// GPS信息相关 end

// 录像下载相关 begin
// 返回错误值,见 JNETErr 说明
long JNV_N_RecQuery(LPCTSTR lpszDevID,int iCenter,int iType,LPCTSTR lpszTimeStart,LPCTSTR lpszTimeEnd,int iChnFlag,LPCTSTR lpszFilePath)
{
	return g_jnvMng.RecQuery(lpszDevID,iCenter,iType,lpszTimeStart,lpszTimeEnd,iChnFlag,lpszFilePath);
}
// 返回错误值,见 JNETErr 说明
long JNV_N_RecDownload(LPCTSTR lpszDevID,LPCTSTR lpszDevPath,int iStartPos,LPCTSTR lpszFilePath)
{
	return g_jnvMng.RecDownload(lpszDevID,lpszDevPath,iStartPos,lpszFilePath);
}
// 录像下载相关 end

// 云台控制 begin
// 返回错误值,见 JNETErr 说明
// iAction,见 eNVPtzActionType 说明
// iValue1:保留1(x速度/y速度/预置位)
// iValue2:保留2(y速度)
long JNV_N_PtzCtrl(LPCTSTR lpszDevID,int iChn,int iAction,int iValue1,int iValue2)
{
	JTRACE("JNV_N_PtzCtrl DevID=%s,Chn=%d,iAction=%d,iValue1=%d,iValue2=%d\r\n",lpszDevID,iChn,iAction,iValue1,iValue2);
	return g_jnvMng.PtzCtrl(lpszDevID,iChn,iAction,iValue1,iValue2);
}
// 云台控制 end

// 实时视频相关 begin
// 开始实时视频流,设备ID,通道编号(1开始),码流编号(保留),码流类型(保留)
// 返回流句柄(>=0)或错误值(见 JNETErr 说明)
// fcbJavaDec:解码事件回调函数
long JNV_OpenStream(LPCTSTR lpszDevID,int iChn,int iStream,int iType,
					fcbDecFrame* pfDecFunLocal,long lUserParam)
{
	long		lStreamID	= 0;
	JTRACE("JNV_OpenStream lpszDev=%s,iChn=%d,iStream=%d,iType=%d\r\n",lpszDevID,iChn,iStream,iType);

	lStreamID = g_jnvMng.RealStreamStart(lpszDevID,iChn,iStream,iType);
	if ( lStreamID>=0 )
	{
		// 成功,增加映射
		stCBParam::AddNewCB(JNV_CB_T_DEC_NET,lStreamID,(void*)pfDecFunLocal,lUserParam);
	}
	else
	{
		// 失败,返回
	}
	return lStreamID;
}
// 返回错误值,见 JNETErr 说明
long JNV_StopStream(long lStream)
{
	long lRet	= 0;
	JTRACE("JNV_StopStream lStream=%ld\r\n",lStream);
	lRet = g_jnvMng.RealStreamStop(lStream);
	stCBParam::Clear(lStream);
	return lRet;
}
// 实时视频相关 end

// 远程录像回放相关 begin
// 返回流句柄(>=0)或错误值(见 JNETErr 说明)
// fcbJavaDec:解码事件回调函数
long JNV_ReplayStart(LPCTSTR lpszDevID,int iCenter,int iChn,LPCTSTR lpszTimeStart,LPCTSTR lpszTimeEnd,int iOnlyIFrame,int iRecType,int iStartPos,
					 fcbDecFrame* pfDecFunLocal,long lUserParam)
{
	long		lStreamID	= 0;
	JTRACE("JNV_ReplayStart lpszDev=%s,iChn=%d\r\n",lpszDevID,iChn);

	lStreamID = g_jnvMng.ReplayStart(lpszDevID,iCenter,iChn,lpszTimeStart,lpszTimeEnd,iOnlyIFrame,iRecType,iStartPos);
	if ( lStreamID>=0 )
	{
		// 成功,增加映射
		stCBParam::AddNewCB(JNV_CB_T_DEC_NET,lStreamID,(void*)pfDecFunLocal,lUserParam);
	}
	else
	{
		// 失败,返回
	}
	return lStreamID;
}
// 返回错误值,见 JNETErr 说明
// iCtrlType,见 eNVReplayType 说明
long JNV_ReplayCtrl(long lStream,int iCtrlType,int iValue)
{
	JTRACE("JNV_ReplayCtrl lStream=%ld,iCtrlType=%d,iValue=%d\r\n",lStream,iCtrlType,iValue);
	return g_jnvMng.ReplayCtrl(lStream,iCtrlType,iValue);
}
// 返回错误值,见 JNETErr 说明
long JNV_ReplayStop(long lStream)
{	
	long lRet	= 0;
	JTRACE("JNV_StopStream lStream=%ld\r\n",lStream);
	lRet = g_jnvMng.ReplayStop(lStream);
	stCBParam::Clear(lStream);
	return lRet;
}
// 远程录像回放相关 end

// 解码操作 begin
// 抓图
// 返回错误值,见 JNETErr 说明
long JNV_Capture(long lStream,LPCTSTR lpszPath)
{
	return g_jnvMng.Capture(lStream,lpszPath);
}
// 开始录像
// 返回错误值,见 JNETErr 说明
long JNV_RecStart(long lStream,LPCTSTR lpszPath)
{
	return g_jnvMng.RecStart(lStream,lpszPath);
}
// 停止录像
// 返回错误值,见 JNETErr 说明
long JNV_RecStop(long lStream)
{
	return g_jnvMng.RecStop(lStream);
}
// 解码操作 end

// 本地录像回放相关 begin
// 返回流句柄(>=0)或错误值(见 JNETErr 说明)
// fcbJavaDec:解码事件回调函数
// fcbJavaPlayInfo:回放信息回调函数
long JNV_RecOpenFile(LPCTSTR lpszPath,
					 fcbDecFrame* pfDecFunLocal,fcbPlay* pfPlayInfo,long lUserParam)
{
	long		lRecFile	= 0;
	JTRACE("JNV_RecOpenFile lpszPath=%s\r\n",lpszPath);

	lRecFile = g_jnvMng.RecOpenFile(lpszPath);
	if ( lRecFile>=0 )
	{
		// 成功,增加映射
		stCBParam::AddNewCB(JNV_CB_T_DEC_LOCAL,lRecFile,(void*)pfDecFunLocal,lUserParam);
		stCBParam::AddNewCB(JNV_CB_T_PLAY_INFO,lRecFile,(void*)pfPlayInfo,lUserParam);
	}
	else
	{
		// 失败,返回
	}
	return lRecFile;
}
// 返回错误值,见 JNETErr 说明
long JNV_RecCloseFile(long lRecFile)
{
	long lRet	= 0;
	JTRACE("JNV_RecCloseFile lRecFile=%ld\r\n",lRecFile);
	lRet = g_jnvMng.RecCloseFile(lRecFile);
	stCBParam::Clear(lRecFile);
	return lRet;
}
// 返回错误值,见 JNETErr 说明
long JNV_RecPlayStart(long lRecFile)
{
	JTRACE("JNV_RecPlayStart lRecFile=%ld\r\n",lRecFile);
	return g_jnvMng.RecPlayStart(lRecFile);
}
// 返回错误值,见 JNETErr 说明
long JNV_RecPlayPause(long lRecFile)
{
	JTRACE("JNV_RecPlayPause lRecFile=%ld\r\n",lRecFile);
	return g_jnvMng.RecPlayPause(lRecFile);
}
// 返回错误值,见 JNETErr 说明
long JNV_RecPlaySetSpeed(long lRecFile,int iSpeed)
{
	JTRACE("JNV_RecPlaySetSpeed lRecFile=%ld\r\n",lRecFile);
	return g_jnvMng.RecPlaySetSpeed(lRecFile,iSpeed);
}
// 返回速度值 见 AVPPlaySpeed 说明
long JNV_RecPlayGetSpeed(long lRecFile)
{
	JTRACE("JNV_RecPlayGetSpeed lRecFile=%ld\r\n",lRecFile);
	return g_jnvMng.RecPlayGetSpeed(lRecFile);
}
// 返回错误值,见 JNETErr 说明
long JNV_RecPlayNextFrame(long lRecFile)
{
	JTRACE("JNV_RecPlayNextFrame lRecFile=%ld\r\n",lRecFile);
	return g_jnvMng.RecPlayNextFrame(lRecFile);
}
// 返回错误值,见 JNETErr 说明
long JNV_RecPlaySeek(long lRecFile,long lTime)
{
	JTRACE("JNV_RecPlaySeek lRecFile=%ld,lTime=%ld\r\n",lRecFile,lTime);
	return g_jnvMng.RecPlaySeek(lRecFile,lTime);
}
// 本地录像回放相关 end


