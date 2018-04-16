// JNVPlayerMng.h: interface for the CJNVPlayerMng class.
//
//////////////////////////////////////////////////////////////////////

#if !defined(AFX_JNVPLAYERMNG_H__1ED9B187_1547_4669_B376_CAECA9635D3D__INCLUDED_)
#define AFX_JNVPLAYERMNG_H__1ED9B187_1547_4669_B376_CAECA9635D3D__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

#include "JNVPlayer.h"

class CJNVPlayerMng;		typedef class CJNVPlayerMng*	LPJNVPlayerMng;
class CJNVPlayerMng  
{
public:
	CJNVPlayerMng();
	virtual ~CJNVPlayerMng();

public:
	// 端口管理 begin
	long	Init(int iPlayerSize);					// 初始化端口管理
	long	Clean();								// 销毁
	long	CreatePort();							// 创建播放器
	long	ReleasePort(int iPort);					// 释放播放器
	LPJNVPlayer GetPlayer(int iPort);				// 获取播放器
	// 端口管理 end

	// 连接相关 begin
	// 登录相关 begin
	long Login(LPCTSTR lpszIP,int iDevPort,LPCTSTR lpszUser,LPCTSTR lpszPassword,int iTimeOut);
	long Logout();
	// 登录相关 end

	// 请求设备列表:0,表示成功;其他,表示失败
	long GetDevList(LPCTSTR lpszFilePath);

	// 请求服务器列表:0,表示成功;其他,表示失败
	long GetServerList(LPCTSTR lpszFilePath, int nServerType);

	// 设置连接服务器类型:0,表示成功;
	long SetConnectServerType(int nServerType);

	// 增加服务器信息:0,表示成功;
	long AddServerInfo(LPCTSTR lpszServerId, LPCTSTR lpszServerIp, int nServerPort, int nServerType);

	// 获取级联服务器 
	// lpszDevID   为外部传入参数 格式为设备ID#中心服务器ID#信令服务器ID#媒体服务器ID
	// nServerType 服务器类型（0 中心 1信令 2媒体）
	// szDevID     解析后实际的设备id
	LPJNVCmdTcp GetCasTcp(LPCTSTR lpszDevID, int nServerType, char* szDevID);

	// 连接服务器
	void AddServer();

	// 连接相关 end

	// 报警相关 begin
	long GetAlarmStart(LPCTSTR lpszDevID);
	long GetAlarmStop(LPCTSTR lpszDevID);
	// 报警相关 end

	// GPS信息相关 begin
	long GetGPSStart(LPCTSTR lpszDevID);
	long GetGPSStop(LPCTSTR lpszDevID);
	// GPS信息相关 end

	// 对讲相关 begin
	long TalkStart(LPCTSTR lpszDevID,int iChn);
	long TalkStop(long lTalk);
	long TalkSend(long lTalk,BYTE* lpData,int iLen);
	// 对讲相关 end

	// 录像相关 begin
	long RecQuery(LPCTSTR lpszDevID,int iCenter,int iType,LPCTSTR lpszTimeStart,LPCTSTR lpszTimeEnd,int iChnFlag,LPCTSTR lpszFilePath);
	long RecDownload(LPCTSTR lpszDevID,LPCTSTR lpszDevPath,int iStartPos,LPCTSTR lpszFilePath);
	// 录像相关 end
	
	// 云台控制 begin
	long PtzCtrl(LPCTSTR lpszDevID,int iChn,int iAction,int iValue1,int iValue2);
	// 云台控制 end

	// 实时视频相关 begin
	// 开始实时视频流,设备ID,通道编号(1开始),码流编号(保留),码流类型(保留)
	long RealStreamStart(LPCTSTR lpszDevID,int iChn,int iStream,int iType);
	long RealStreamReqIFrame(long lStream);
	long RealStreamStop(long lStream);
	// 实时视频相关 end

	// 设备录像回放相关 begin
	long ReplayStart(LPCTSTR lpszDevID,int iCenter,int iChn,LPCTSTR lpszTimeStart,LPCTSTR lpszTimeEnd,int iOnlyIFrame,int iRecType,int iStartPos);
	long ReplayCtrl(long lStream,int iCtrlType,int iValue);
	long ReplayStop(long lStream);
	// 设备录像回放相关 end

	// 解码操作 begin
	long Capture(long lStream,LPCTSTR lpszPath);	// 抓图
	long RecStart(long lStream,LPCTSTR lpszPath);	// 开始录像
	long RecStop(long lStream);						// 停止录像
	// 解码操作 end

	// 录像视频相关 begin
	long RecOpenFile(LPCTSTR lpszPath);				// 打开录像文件
	long RecCloseFile(long lRecFile);				// 关闭录像文件
	long RecPlayStart(long lRecFile);				// 开始/继续 播放
	long RecPlayPause(long lRecFile);				// 暂停播放
	long RecPlaySetSpeed(long lRecFile,int iSpeed);	// 设置播放速度
	long RecPlayGetSpeed(long lRecFile);			// 获取播放速度
	long RecPlayNextFrame(long lRecFile);			// 下一帧
	long RecPlaySeek(long lRecFile,long lTime);		// 设置播放速度
	long RecPlayCapture(long lRecFile,LPCTSTR lpszPath);	// 录像抓拍
	// 录像视频相关 end

protected:
	static int __stdcall OnStream(long lHandle, LPBYTE pBuff,DWORD dwRevLen,void* pUserParam);
	static int __stdcall OnConnEvent(long lHandle, eJNetEvent eType,int iDataType,void* pEventData,int iDataLen,void* pUserParam);	
	static int __stdcall OnDecFrame(PSTDecFrameCB pstDecFrameCB);
	static int __stdcall OnPlayControl(AVPPlayCBMsg eMsg,int iPlayerID,long lpUserParam,__int64 lParam1,__int64 lParam2,DWORD dwRes);
public:
	fcbDecFrame*	m_pfDecFunNet;					// 解码回调函数
	fcbDecFrame*	m_pfDecFunLocal;				// 解码回调函数
	fcbJEvent*		m_pfEventFun;					// 事件回调函数
	fcbPlay*		m_pfPlayInfoFun;				// 播放事件回调函数
	int				m_iImgRGBFM;					// 解码后图像RGB格式
protected:
	LPJNVPlayer		m_arPlayer;						// 播放器数组
	int				m_iPlayerSize;					// 播放器个数
	CJNVCmdTcp		m_tcpCmd;						// 信令连接
	CDecoder		m_decAudio;						// 音频编码

	int					m_nConnectServerType;			// 连接服务器类型（0 连单服务器版， 1 连级联服务器）
	//LPJNVCmdTcp			m_arCasTcp[JNET_MAX_STREAM];	// 设备信令通道（级联模式）
	stJNVServerInfo		m_arServerInfo[MAX_SERVER_NUM];	// 服务器


	CMutexLock		m_mutexItemServer;				// 连接服务器锁
	bool			m_bThreadExit;					// 是否退出
	pthread_t		m_hThreadID;					// 处理线程ID
};

#endif // !defined(AFX_JNVPLAYERMNG_H__1ED9B187_1547_4669_B376_CAECA9635D3D__INCLUDED_)
