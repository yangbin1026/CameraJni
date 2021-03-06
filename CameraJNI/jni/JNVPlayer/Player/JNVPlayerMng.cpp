// JNVPlayerMng.cpp: implementation of the CJNVPlayerMng class.
//
//////////////////////////////////////////////////////////////////////

#include "JNVPlayerMng.h"
#include "lib_def.h"

#undef	LOG_TAG
#define	LOG_TAG		"JNVPlayerMng.cpp"

//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////

LPJNVProtocolTCP g_lpCmdTcp = NULL;

CJNVPlayerMng::CJNVPlayerMng()
{
	m_arPlayer			= NULL;				// 播放器数组
	m_iPlayerSize		= 0;				// 播放器个数
	m_pfDecFunNet		= NULL;				// 网络流解码回调函数
	m_pfDecFunLocal		= NULL;				// 本地流解码回调函数
	m_pfEventFun		= NULL;				// 事件回调函数
	m_pfPlayInfoFun		= NULL;				// 播放事件回调函数
	m_iImgRGBFM			= JDRAW_RGB_565;	// 解码后图像RGB格式
	m_nConnectServerType	= 0;				// 连接服务器类型（0 连单服务器版， 1 连级联服务器）
	memset(m_arServerInfo,0,sizeof(stJNVServerInfo)*MAX_SERVER_NUM);		// 服务器信息

	g_lpCmdTcp = &m_tcpCmd;

	m_bThreadExit = false;
	m_mutexItemServer.CreateMutex();
}

CJNVPlayerMng::~CJNVPlayerMng()
{
	m_bThreadExit = true;
	JSleep(10);
	m_mutexItemServer.CloseMutex();
	

	for (int i = 0; i < MAX_SERVER_NUM; i++)
	{
		LPJNVCmdTcp pCmdTcp = (LPJNVCmdTcp)m_arServerInfo[i].lpCasTcp;
		if(pCmdTcp != NULL)
		{
			pCmdTcp->Close();
			delete pCmdTcp;
			m_arServerInfo[i].lpCasTcp = NULL;
		}
	}
}

// 消息分派
fJThRet RunAddServer(void* pParam)
{
	CJNVPlayerMng* pObjPlay = (CJNVPlayerMng*)pParam;
	if (pObjPlay)
	{
		pObjPlay->AddServer();
	}
	
	return 0;
}


// 连接服务器
void CJNVPlayerMng::AddServer()
{
	while(!m_bThreadExit)
	{
		int i = 0;
		m_mutexItemServer.Lock();
		for(i = 0; i < MAX_SERVER_NUM; i++)
		{
			//如果id长度为0，说明数组已到末尾，直接添加
			if (strlen(m_arServerInfo[i].szServerId) != 0 && m_arServerInfo[i].lpCasTcp == NULL )
			{
				if (m_arServerInfo[i].nServerType != eJNVCenterServer)
				{
					JTRACE("AddServerInfo:%s  %d, %s , %d\r\n", m_arServerInfo[i].szServerIP, m_arServerInfo[i].nServerPort, m_arServerInfo[i].szServerId, m_arServerInfo[i].nServerType);
					
					CJNVCmdTcp* pCasTcp = new CJNVCmdTcp();
					pCasTcp->m_nServerType = m_arServerInfo[i].nServerType;
					if (pCasTcp->Login(m_arServerInfo[i].szServerIP, m_arServerInfo[i].nServerPort, m_arServerInfo[i].szServerId, m_arServerInfo[i].szServerId, 15, m_pfEventFun, this) == 0)
					{
						m_arServerInfo[i].lpCasTcp = (void*)pCasTcp;
					}
					else
					{
						delete pCasTcp;
					}

					break;
				}
			}
		}
		m_mutexItemServer.Unlock();
		JSleep(5);
	}
}

// 端口管理 begin
// 初始化端口管理
long CJNVPlayerMng::Init(int iPlayerSize)
{
	CJLSocket::InitSocket();

	SAFE_DELETE_A(m_arPlayer);
	m_iPlayerSize = 0;

	pthread_create( &m_hThreadID, NULL, RunAddServer, this);

	m_arPlayer = new CJNVPlayer[iPlayerSize];
	if ( m_arPlayer )
	{
		m_iPlayerSize = iPlayerSize;
		m_decAudio.Init();
		m_decAudio.SetMediaType(0,0,-1,JAV_AT_G726);
		return JNETErrSuccess;
	}
	else
	{
		return JNETErrNoMem;
	}



}



// 销毁
long CJNVPlayerMng::Clean()
{
	SAFE_DELETE_A(m_arPlayer);
	m_iPlayerSize = 0;

	CJLSocket::CleanSocket();
	m_decAudio.UnInit();
	return JNETErrSuccess;
}
// 创建播放器
long CJNVPlayerMng::CreatePort()
{
	int	i	= 0;

	// 创建播放器 begin
	for (i=0;i<m_iPlayerSize;i++)
	{
		if (m_arPlayer[i].GetID()<0 )
		{
			m_arPlayer[i].SetID(i);
			m_arPlayer[i].SetDecRGBFM(m_iImgRGBFM);
			return i;
		}
	}
	return JNETErrNoFreePort;
	// 创建播放器 end
}
// 释放播放器
long CJNVPlayerMng::ReleasePort(int iPort)
{
	LPJNVPlayer	pPlayer		= GetPlayer(iPort);
	if ( pPlayer==NULL ) return JNETErrHandle;
	m_arPlayer[iPort].Release();
	
	return JNETErrSuccess;
}
// 获取播放器
LPJNVPlayer CJNVPlayerMng::GetPlayer(int iPort)
{
	if ( iPort<0 || iPort>= m_iPlayerSize ) return NULL;
	return m_arPlayer+iPort;
}
// 端口管理 end


// 连接相关 begin
// 登录相关 begin
long CJNVPlayerMng::Login(LPCTSTR lpszIP,int iDevPort,LPCTSTR lpszUser,LPCTSTR lpszPassword,int iTimeOut)
{
	if ( m_pfEventFun==NULL ) m_pfEventFun = OnConnEvent;
	return m_tcpCmd.Login(lpszIP,iDevPort,lpszUser,lpszPassword,iTimeOut,m_pfEventFun,this);		// 信令连接
}
long CJNVPlayerMng::Logout()
{
	return m_tcpCmd.Logout();
}
// 登录相关 end
// 请求设备列表:0,表示成功;其他,表示失败
long CJNVPlayerMng::GetDevList(LPCTSTR lpszFilePath)
{
	return m_tcpCmd.GetDevList(lpszFilePath);
}

// 请求服务器列表:0,表示成功;其他,表示失败
long CJNVPlayerMng::GetServerList(LPCTSTR lpszFilePath, int nServerType)
{
	return m_tcpCmd.GetServerList(lpszFilePath, nServerType);
}

// 设置连接服务器类型:0,表示成功;其他,表示失败
long CJNVPlayerMng::SetConnectServerType(int nServerType)
{
	m_nConnectServerType = nServerType;
	return JNETErrSuccess;
}

// 增加服务器信息 0,表示成功;其他,表示失败
long CJNVPlayerMng::AddServerInfo(LPCTSTR lpszServerId, LPCTSTR lpszServerIp, int nServerPort, int nServerType)
{
	try
	{
		m_mutexItemServer.Lock();

		int i = 0;
		for(i = 0; i < MAX_SERVER_NUM; i++)
		{
			//如果id长度为0，说明数组已到末尾，直接添加
			if (strlen(m_arServerInfo[i].szServerId) == 0)
			{
				strcpy(m_arServerInfo[i].szServerId, lpszServerId);
				strcpy(m_arServerInfo[i].szServerIP, lpszServerIp);
				m_arServerInfo[i].nServerType = nServerType;
				m_arServerInfo[i].nServerPort = nServerPort;
				
				/*
				if (nServerType != eJNVCenterServer)
				{
					JTRACE("AddServerInfo:%s  %d, %s , %d\r\n", m_arServerInfo[i].szServerIP, m_arServerInfo[i].nServerPort, lpszServerId, nServerType);

					CJNVCmdTcp* pCasTcp = new CJNVCmdTcp();
					if (pCasTcp->Login(m_arServerInfo[i].szServerIP, m_arServerInfo[i].nServerPort, lpszServerId, lpszServerId, 15, m_pfEventFun, this) == 0)
					{
						m_arServerInfo[i].lpCasTcp = (void*)pCasTcp;
					}
					else
					{
						delete pCasTcp;
					}
				}
				*/
				m_mutexItemServer.Unlock();
				return JNETErrSuccess;
			}
			else
			{
				//如果服务器类型、id 在数组中已存在同样记录，直接更新
				if (m_arServerInfo[i].nServerType == nServerType && strcmp(m_arServerInfo[i].szServerId, lpszServerId) == 0)
				{
					strcpy(m_arServerInfo[i].szServerIP, lpszServerIp);
					m_arServerInfo[i].nServerPort = nServerPort;
					m_mutexItemServer.Unlock();
					return JNETErrSuccess;
				}
			}
		}

	}
	catch(...)
	{
		m_mutexItemServer.Unlock();
		return JNETErrNoMem;
	}

	//没有添加成功，返回失败
	m_mutexItemServer.Unlock();
	return JNETErrBuffSize;
}


// 获取级联服务器 
// lpszDevID   为外部传入参数 格式为设备ID#中心服务器ID#信令服务器ID#媒体服务器ID
// nServerType 服务器类型（0 中心 1信令 2媒体）
// szDevID     解析后实际的设备id
LPJNVCmdTcp CJNVPlayerMng::GetCasTcp(LPCTSTR lpszDevID, int nServerType, char* szDevID)
{
	if (lpszDevID == NULL || szDevID == NULL)
		return NULL;

	//解析设备id
	char szTemp[1024] = {0};
	strcpy(szTemp, lpszDevID);
	char* p = strchr(szTemp, '#');
	if(p == NULL)
		return NULL;

	*p = '\0';
	strcpy(szDevID, szTemp);
	p++;

	char szServerId[JNET_SERVER_ID_LEN] = {0};

	try
	{
		//解析
		if (nServerType == eJNVCmdServer)
		{
			char* pszServer = strchr(p, '#');
			*pszServer = '\0';
			pszServer++;
			p = pszServer;
			pszServer = strchr(p, '#');
			*pszServer = '\0';
			strcpy(szServerId, p);
		}
		else if (nServerType == eJNVMediaServer)
		{
			char* pszServer = strrchr(p, '#');
			pszServer++;
			strcpy(szServerId, pszServer);
		}

		int i = 0;
		for(i = 0; i < MAX_SERVER_NUM; i++)
		{
			//找服务器
			if (m_arServerInfo[i].nServerType == nServerType && strcmp(m_arServerInfo[i].szServerId, szServerId) == 0)
			{
				//未连接 则先连接，再登录 返回对象指针
				if (m_arServerInfo[i].lpCasTcp == NULL)
				{
					return NULL;
					/*
					CJNVCmdTcp* pCasTcp = new CJNVCmdTcp();
					if (pCasTcp->Login(m_arServerInfo[i].szServerIP, m_arServerInfo[i].nServerPort, szServerId, szServerId, 15, m_pfEventFun, this) == 0)
					{
						m_arServerInfo[i].lpCasTcp = (void*)pCasTcp;
						JSleep(10);
						return pCasTcp;
					}
					else
					{
						delete pCasTcp;
						return NULL;
					}
					*/
				}
				else
				{
					//已连接，直接返回对象指针
					return (LPJNVCmdTcp)m_arServerInfo[i].lpCasTcp;
				}
			}
		}
	}
	catch (...)
	{
		return NULL;
	}

	return NULL;
}
// 连接相关 end

// 报警相关 begin
long CJNVPlayerMng::GetAlarmStart(LPCTSTR lpszDevID)
{
	if (m_nConnectServerType)
	{
		char szDevID[JNET_DEV_ID_LEN] = {0};
		LPJNVCmdTcp lpCasTcp = GetCasTcp(lpszDevID, eJNVCmdServer, szDevID);
		if (lpCasTcp)
		{
			return lpCasTcp->GetAlarmStart(szDevID);
		}

		return JNETErrHandle;
	}

	return m_tcpCmd.GetAlarmStart(lpszDevID);
}
long CJNVPlayerMng::GetAlarmStop(LPCTSTR lpszDevID)
{
	if (m_nConnectServerType)
	{
		char szDevID[JNET_DEV_ID_LEN] = {0};
		LPJNVCmdTcp lpCasTcp = GetCasTcp(lpszDevID, eJNVCmdServer, szDevID);
		if (lpCasTcp)
		{
			return lpCasTcp->GetAlarmStop(szDevID);
		}

		return JNETErrHandle;
	}

	return m_tcpCmd.GetAlarmStop(lpszDevID);
}
// 报警相关 end

// GPS信息相关 begin
long CJNVPlayerMng::GetGPSStart(LPCTSTR lpszDevID)
{
	if (m_nConnectServerType)
	{
		char szDevID[JNET_DEV_ID_LEN] = {0};
		LPJNVCmdTcp lpCasTcp = GetCasTcp(lpszDevID, eJNVCmdServer, szDevID);
		if (lpCasTcp)
		{
			return lpCasTcp->GetGPSStart(szDevID);
		}

		return JNETErrHandle;
	}

	return m_tcpCmd.GetGPSStart(lpszDevID);
}
long CJNVPlayerMng::GetGPSStop(LPCTSTR lpszDevID)
{
	if (m_nConnectServerType)
	{
		char szDevID[JNET_DEV_ID_LEN] = {0};
		LPJNVCmdTcp lpCasTcp = GetCasTcp(lpszDevID, eJNVCmdServer, szDevID);
		if (lpCasTcp)
		{
			return lpCasTcp->GetGPSStop(szDevID);
		}

		return JNETErrHandle;
	}

	return m_tcpCmd.GetGPSStop(lpszDevID);
}
// GPS信息相关 end

// 对讲相关 begin
long CJNVPlayerMng::TalkStart(LPCTSTR lpszDevID,int iChn)
{
	if (m_nConnectServerType)
	{
		char szDevID[JNET_DEV_ID_LEN] = {0};
		LPJNVCmdTcp lpCasTcp = GetCasTcp(lpszDevID, eJNVMediaServer, szDevID);
		if (lpCasTcp)
		{
			return lpCasTcp->TalkStart(szDevID, iChn);
		}

		return JNETErrHandle;
	}

	return m_tcpCmd.TalkStart(lpszDevID,iChn);
}
long CJNVPlayerMng::TalkStop(long lTalk)
{
	return m_tcpCmd.TalkStop(lTalk);
}
long CJNVPlayerMng::TalkSend(long lTalk,BYTE* lpData,int iLen)
{
	BYTE	byOut[2048]		= {0};
	LPBYTE	lpOut			= byOut;
	int		iInCount		= 320;
	int		iOutLen			= 0;
	int		iTotalOut		= 0;
	int		iLeftCount		= iLen;

	while(iLeftCount>=iInCount)
	{
		m_decAudio.AudioEnc(lpData,iInCount,lpOut,&iOutLen);
		lpData+=iInCount;
		iLeftCount-=iInCount;
		lpOut+=iOutLen;
		iTotalOut+=iOutLen;
	}

	return m_tcpCmd.TalkSend(lTalk,byOut,iTotalOut);
}
// 对讲相关 end

// 录像相关 begin
long CJNVPlayerMng::RecQuery(LPCTSTR lpszDevID,int iCenter,int iType,LPCTSTR lpszTimeStart,LPCTSTR lpszTimeEnd,int iChnFlag,LPCTSTR lpszFilePath)
{
	if (m_nConnectServerType)
	{
		char szDevID[JNET_DEV_ID_LEN] = {0};
		LPJNVCmdTcp lpCasTcp = GetCasTcp(lpszDevID, eJNVMediaServer, szDevID);
		if (lpCasTcp)
		{
			return lpCasTcp->RecQuery(szDevID,iCenter,iType,lpszTimeStart,lpszTimeEnd,iChnFlag,lpszFilePath);
		}

		return JNETErrHandle;
	}

	return m_tcpCmd.RecQuery(lpszDevID,iCenter,iType,lpszTimeStart,lpszTimeEnd,iChnFlag,lpszFilePath);
}
long CJNVPlayerMng::RecDownload(LPCTSTR lpszDevID,LPCTSTR lpszDevPath,int iStartPos,LPCTSTR lpszFilePath)
{
	if (m_nConnectServerType)
	{
		char szDevID[JNET_DEV_ID_LEN] = {0};
		LPJNVCmdTcp lpCasTcp = GetCasTcp(lpszDevID, eJNVMediaServer, szDevID);
		if (lpCasTcp)
		{
			return lpCasTcp->RecDownload(szDevID,lpszDevPath,iStartPos,lpszFilePath);
		}

		return JNETErrHandle;
	}

	return m_tcpCmd.RecDownload(lpszDevID,lpszDevPath,iStartPos,lpszFilePath);
}
// 录像相关 end

// 云台控制 begin
long CJNVPlayerMng::PtzCtrl(LPCTSTR lpszDevID,int iChn,int iAction,int iValue1,int iValue2)
{
	if (m_nConnectServerType)
	{
		char szDevID[JNET_DEV_ID_LEN] = {0};
		LPJNVCmdTcp lpCasTcp = GetCasTcp(lpszDevID, eJNVMediaServer, szDevID);
		if (lpCasTcp)
		{
			return lpCasTcp->PtzCtrl(szDevID,iChn,iAction,iValue1,iValue2);
		}

		return JNETErrHandle;
	}

	return m_tcpCmd.PtzCtrl(lpszDevID,iChn,iAction,iValue1,iValue2);
}
// 云台控制 end

// 实时视频相关 begin
// 开始实时视频流,设备ID,通道编号(1开始),码流编号(保留),码流类型(保留)
long CJNVPlayerMng::RealStreamStart(LPCTSTR lpszDevID,int iChn,int iStream,int iType)
{
	long	lPort	= CreatePort();
	long	lStream	= 0;
	if ( lPort<0 ) return lPort;

	LPJNVPlayer	lpPlayer	= m_arPlayer+lPort;
	lpPlayer->Play();

	if (m_nConnectServerType)
	{
		char szDevID[JNET_DEV_ID_LEN] = {0};
		LPJNVCmdTcp lpCasTcp = GetCasTcp(lpszDevID, eJNVMediaServer, szDevID);
		if (lpCasTcp)
		{
			lStream = lpCasTcp->RealStreamStart(szDevID,iChn,iStream,iType,OnStream,lpPlayer);
		}
	}
	else
	{
		lStream = m_tcpCmd.RealStreamStart(lpszDevID,iChn,iStream,iType,OnStream,lpPlayer);
	}
	if ( lStream>0 )
	{
		if ( m_pfDecFunNet==NULL ) m_pfDecFunNet = OnDecFrame;
		// 解码回调函数
		lpPlayer->SetDecCallBack(m_pfDecFunNet,lStream);
		return lStream;
	}
	else
	{
		lpPlayer->Stop();
		ReleasePort(lPort);
		return JNETErrOperate;
	}
}
long CJNVPlayerMng::RealStreamReqIFrame(long lStream)
{
	return m_tcpCmd.RealStreamReqIFrame(lStream);
}
long CJNVPlayerMng::RealStreamStop(long lStream)
{
	long		lRet		= 0;
	LPJNVPlayer	lpPlayer	= (LPJNVPlayer)(m_tcpCmd.GetStreamParam(lStream));

	lRet =  m_tcpCmd.RealStreamStop(lStream);
	if ( lpPlayer )
	{
		lpPlayer->SetDecCallBack(NULL,0);
		lpPlayer->Stop();
		ReleasePort(lpPlayer->GetID());
	}
	return lRet;
}
// 实时视频相关 end

// 设备录像回放相关 begin
long CJNVPlayerMng::ReplayStart(LPCTSTR lpszDevID,int iCenter,int iChn,LPCTSTR lpszTimeStart,LPCTSTR lpszTimeEnd,int iOnlyIFrame,int iRecType,int iStartPos)
{
	long	lPort	= CreatePort();
	long	lStream	= 0;
	if ( lPort<0 ) return lPort;

	LPJNVPlayer	lpPlayer	= m_arPlayer+lPort;
	lpPlayer->Play();

	if (m_nConnectServerType)
	{
		char szDevID[JNET_DEV_ID_LEN] = {0};
		LPJNVCmdTcp lpCasTcp = GetCasTcp(lpszDevID, eJNVMediaServer, szDevID);
		if (lpCasTcp)
		{
			lStream = lpCasTcp->ReplayStart(szDevID,iCenter,iChn,lpszTimeStart,lpszTimeEnd,iOnlyIFrame,iRecType,iStartPos,OnStream,lpPlayer);
		}
	}
	else
	{
		lStream = m_tcpCmd.ReplayStart(lpszDevID,iCenter,iChn,lpszTimeStart,lpszTimeEnd,iOnlyIFrame,iRecType,iStartPos,OnStream,lpPlayer);
	}
	if ( lStream>0 )
	{
		if ( m_pfDecFunNet==NULL ) m_pfDecFunNet = OnDecFrame;
		// 解码回调函数
		lpPlayer->SetDecCallBack(m_pfDecFunNet,lStream);
		return lStream;
	}
	else
	{
		lpPlayer->Stop();
		ReleasePort(lPort);
		return JNETErrOperate;
	}
}
long CJNVPlayerMng::ReplayCtrl(long lStream,int iCtrlType,int iValue)
{
	return m_tcpCmd.ReplayCtrl(lStream,iCtrlType,iValue);
}
long CJNVPlayerMng::ReplayStop(long lStream)
{
	return m_tcpCmd.ReplayStop(lStream);
}
// 设备录像回放相关 end

// 解码操作 begin
// 抓图
long CJNVPlayerMng::Capture(long lStream,LPCTSTR lpszPath)
{
	LPJNVPlayer	lpPlayer	= (LPJNVPlayer)(m_tcpCmd.GetStreamParam(lStream));
	if ( lpPlayer )
	{
		return lpPlayer->Capture(lpszPath);
	}
	return JNETErrParam;
}
// 开始录像
long CJNVPlayerMng::RecStart(long lStream,LPCTSTR lpszPath)
{
	LPJNVPlayer	lpPlayer	= (LPJNVPlayer)(m_tcpCmd.GetStreamParam(lStream));
	if ( lpPlayer )
	{
		return lpPlayer->RecStart(lpszPath);
	}
	return JNETErrParam;
}
// 停止录像
long CJNVPlayerMng::RecStop(long lStream)
{
	LPJNVPlayer	lpPlayer	= (LPJNVPlayer)(m_tcpCmd.GetStreamParam(lStream));
	if ( lpPlayer )
	{
		return lpPlayer->RecStop();
	}
	return JNETErrParam;
}
// 解码操作 end

// 录像视频相关 begin
// 打开录像文件
long CJNVPlayerMng::RecOpenFile(LPCTSTR lpszPath)
{
	long	lPort	= CreatePort();
	if ( lPort<0 ) return lPort;

	LPJNVPlayer	lpPlayer	= m_arPlayer+lPort;
	if ( lpPlayer->OpenFile(lpszPath)==JNETErrSuccess )
	{
		lpPlayer->Play();
		if ( m_pfDecFunLocal==NULL ) m_pfDecFunLocal = OnDecFrame;
		if( m_pfPlayInfoFun==NULL ) m_pfPlayInfoFun = OnPlayControl;
		// 解码回调函数
		lpPlayer->SetDecCallBack(m_pfDecFunLocal,lPort);
		lpPlayer->SetPlayInfoCallBack(m_pfPlayInfoFun,lPort);
		return lpPlayer->GetID();
	}
	else
	{
		return JNETErrOpenFile;
	}
}
// 关闭录像文件
long CJNVPlayerMng::RecCloseFile(long lRecFile)
{
	LPJNVPlayer	lpPlayer	= GetPlayer(lRecFile);
	if ( lpPlayer )
	{
		lpPlayer->Stop();
		lpPlayer->SetDecCallBack(NULL,0);
		lpPlayer->SetPlayInfoCallBack(NULL,0);
		ReleasePort(lRecFile);
	}
	return JNETErrSuccess;
}
// 开始/继续 播放
long CJNVPlayerMng::RecPlayStart(long lRecFile)
{
	LPJNVPlayer	lpPlayer	= GetPlayer(lRecFile);
	if ( lpPlayer )
	{
		lpPlayer->Play();
	}
	return JNETErrSuccess;

}
// 暂停播放
long CJNVPlayerMng::RecPlayPause(long lRecFile)
{
	LPJNVPlayer	lpPlayer	= GetPlayer(lRecFile);
	if ( lpPlayer )
	{
		lpPlayer->Pause();
	}
	return JNETErrSuccess;
}
// 设置播放速度
long CJNVPlayerMng::RecPlaySetSpeed(long lRecFile,int iSpeed)
{
	LPJNVPlayer	lpPlayer	= GetPlayer(lRecFile);
	if ( lpPlayer )
	{
		lpPlayer->SetPlaySpeed(iSpeed);
	}
	return JNETErrSuccess;
}
// 获取播放速度
long CJNVPlayerMng::RecPlayGetSpeed(long lRecFile)
{
	LPJNVPlayer	lpPlayer	= GetPlayer(lRecFile);
	if ( lpPlayer )
	{
		return lpPlayer->GetPlaySpeed();
	}
	return AVPPlaySpeed_1X;
}
// 下一帧
long CJNVPlayerMng::RecPlayNextFrame(long lRecFile)
{
	LPJNVPlayer	lpPlayer	= GetPlayer(lRecFile);
	if ( lpPlayer )
	{
		return lpPlayer->NextFrame();
	}
	return JNETErrParam;
}
// 设置播放速度
long CJNVPlayerMng::RecPlaySeek(long lRecFile,long lTime)
{
	LPJNVPlayer	lpPlayer	= GetPlayer(lRecFile);
	if ( lpPlayer )
	{
		return lpPlayer->Seek(lTime);
	}
	return JNETErrParam;
}
// 抓图
long CJNVPlayerMng::RecPlayCapture(long lRecFile,LPCTSTR lpszPath)
{
	LPJNVPlayer	lpPlayer	= GetPlayer(lRecFile);
	if ( lpPlayer )
	{
		return lpPlayer->Capture(lpszPath);
	}
	return JNETErrParam;
}
// 录像视频相关 end

// 流回调
int CJNVPlayerMng::OnStream(long lHandle, LPBYTE pBuff,DWORD dwRevLen,void* pUserParam)
{
	LPJNVPlayer	lpPlayer	= (LPJNVPlayer)pUserParam;
	
	lpPlayer->PutFrame(pBuff);
//	JTRACE("CJNVPlayerMng::OnStream(Handle=%ld,DataLen=%d)\r\n",lHandle,dwRevLen);
	return 0;
}

// 信令事件回调
int CJNVPlayerMng::OnConnEvent(long lHandle, eJNetEvent eType,int iDataType,void* pEventData,int iDataLen,void* pUserParam)
{
//	LPJNVPlayerMng	lpMng	= (LPJNVPlayerMng)pUserParam;

	JTRACE("CJNVPlayerMng::OnConnEvent(Handle=%d,Event=%d,DataType=%d,DataLen=%d)\r\n",
		lHandle, eType,iDataType,iDataLen);
	/*
	JNET_EET_UNKNOWN			= -1,		// 未定义
											
	JNET_EET_LOGINING			= 0,		// 正在登录
	JNET_EET_LOGIN_OK			= 1,		// 登录成功
	JNET_EET_LOGIN_ERROR		= 2,		// 登录失败
	JNET_EET_LOGOUT				= 3,		// 登出
	JNET_EET_STREAM_OPENING		= 4,		// 流正在打开
	JNET_EET_STREAM_OK			= 5,		// 流打开成功
	JNET_EET_STREAM_ERROR		= 6,		// 流打开失败
	JNET_EET_STREAM_CLOSE		= 7,		// 流关闭
	JNET_EET_PB_OPENING			= 8,		// 回放流正在打开
	JNET_EET_PB_OK				= 9,		// 回放流打开成功
	JNET_EET_PB_ERROR			= 10,		// 回放流打开失败
	JNET_EET_PB_CLOSE			= 11,		// 回放流关闭
	JNET_EET_TALK_OPENING		= 12,		// 对讲正在打开
	JNET_EET_TALK_OK			= 13,		// 对讲打开成功
	JNET_EET_TALK_ERROR			= 14,		// 对讲打开失败
	JNET_EET_TALK_CLOSE			= 15,		// 对讲关闭
	JNET_EET_UPG_START			= 16,		// 升级开始
	JNET_EET_UPG_OK				= 17,		// 升级成功
	JNET_EET_UPG_ERR			= 18,		// 升级失败
	JNET_EET_CFG_SET			= 100,		// 获取配置
	JNET_EET_CFG_GET			= 101,		// 设置配置
	JNET_EET_PTZ_CTRL			= 102,		// 云台控制
	JNET_EET_DEV_CTRL			= 103,		// 设备控制
	JNET_EET_STREAM_LOST		= 104,		// 流丢失
	JNET_EET_EVENT_NOTIFY		= 201,		// 报警事件
	JNET_EET_EVENT_DEV_LIST		= 301,		// 获取设备列表,iDataType=eJNVFileType
	JNET_EET_EVENT_REC_LIST		= 302,		// 获取录像列表,iDataType=eJNVFileType
	JNET_EET_EVENT_REC_DOWN		= 303,		// 录像下载,iDataType=eJNVFileType
	JNET_EET_EVENT_GPS_INFO		= 40,		// 下发GPS信息,pEventData=PSTJNVGPSAll
	*/
	return 0;
}

int CJNVPlayerMng::OnDecFrame(PSTDecFrameCB pstDecFrameCB)
{
	JTRACE("CJNVPlayerMng::OnDecFrame size=%d,%d*%d,type=%d,lStream=%d\r\n",
		pstDecFrameCB->lSize,pstDecFrameCB->lWidth,pstDecFrameCB->lHeight,pstDecFrameCB->lType,pstDecFrameCB->lParam);

	return 0;
}

int CJNVPlayerMng::OnPlayControl(AVPPlayCBMsg eMsg,int iPlayerID,long lpUserParam,__int64 lParam1,__int64 lParam2,DWORD dwRes)
{	
	JTRACE("CJNVPlayerMng::OnPlayControl eMsg=%d,iPlayerID=%d,lpUserParam=%d,lParam1=%d,lParam2=%d,dwRes=%d\r\n",
		   eMsg,iPlayerID,lpUserParam,lParam1,lParam2,dwRes);

	return 0;
}
