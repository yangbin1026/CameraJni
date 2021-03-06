// JNVProtocolTCP.cpp: implementation of the CJNVProtocolTCP class.
//
//////////////////////////////////////////////////////////////////////

#include "JNVProtocolTCP.h"
#undef	LOG_TAG
#define	LOG_TAG		"JNVProtocolTCP.cpp"

//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////

extern LPJNVProtocolTCP g_lpCmdTcp;

CJNVProtocolTCP::CJNVProtocolTCP()
{
	m_pfcbEvent			= NULL;					// 事件通知函数
	m_pUserParam		= NULL;					// 事件通知用户参数

	m_bAutoLogin		= false;				// 是否自动登录
	m_iDataOffset		= 0;					// 接收偏移(为了组帧方便,少一次的拷贝过程)
	memset(m_szDevIP,0,JNET_IP_LEN);			// 设备IP
	m_iDevPort			= 0;					// 设备端口
	m_iStreamPort		= 0;					// 流端口
	memset(m_szUser,0,JNET_NAME_LEN*2);			// 用户名
	memset(m_szPwd,0,JNET_PWD_LEN*2);				// 密码
	
	SetTimeOut(JSS_SO_T_SEND,2*1000);			// 发送超时(毫秒)
	SetTimeOut(JSS_SO_T_RECV,2*1000);			// 接收超时(毫秒)
	
	m_hEventSync		= NULL;					// 同步事件
	
	m_dwLastSendBreath	= 0;					// 上一呼吸时间
	m_dwLastRecv		= 0;					// 上一接收时间
	m_iBreathSpan		= 0;					// 呼吸间隔
	
	m_dwSendSeq			= 0;					// 用于同步接收
	m_lpSendData		= NULL;					// 用于同步接收
	m_iSendDataLen		= 0;					// 用于同步接收
	
	memset(&m_stLoginInfo,0,sizeof(m_stLoginInfo));	// Session

	m_nServerType = eJNVCenterServer;
}

CJNVProtocolTCP::~CJNVProtocolTCP()
{
	m_pfcbEvent		= NULL;						// 事件通知函数
	m_pUserParam	= NULL;						// 用户自定义参数
}

// 通知函数 begin
// 用于连接到指定的CMS上
int CJNVProtocolTCP::Connect()
{
	return CJLTcpClient::Connect(m_szDevIP,m_iDevPort);
}
// 检查本对象的状态,返回0表示 OnCheckSocket 继续,否则退出检查线程
int CJNVProtocolTCP::OnCheckState(DWORD dwTick)
{
	// 上一接收时间
	if ( IsOverTime(m_dwLastRecv,dwTick,(m_iBreathSpan*3*1000)) )
	{
		// 连接超时了
		JTRACE("CJNVProtocolTCP LastRecv OverTime\r\n");
		// 重连 begin
		ResetSock();								// 重置SOCKET
		if( g_lpCmdTcp == this )
		{
			JTRACE("CJNVProtocolTCP::OnRetLogin: failed22 %d    %d !!!!!!!!!\r\n", g_lpCmdTcp, this);
			SendEvent(JNET_EET_LOGIN_ERROR,0,NULL,0);	// 登录失败
		}
		m_dwLastRecv = dwTick;						// 保证不会频繁操作
		SetNoBlock(true);							// 重连需要非阻塞
		Connect();									// 开始重连
		// 重连 end
	}

	if ( GetHandle()!=INVALID_SOCKET && IsOverTime(m_dwLastSendBreath,dwTick,(m_iBreathSpan*1000)) )
	{
		// 发送心跳
		OnSendBreath(dwTick);
	}
	return JNETErrSuccess;
}
// 客户端消息到达
int CJNVProtocolTCP::OnClientMsg(void* pParam)
{
	LPBYTE			lpBuf		= NULL;
	int				iHasRecv	= 0;
	int				iDataLen	= 0;

	LockThread("CJNVProtocolTCP::OnClientMsg");
	lpBuf	= (LPBYTE)(&m_stLastRecv);
	// 接收协议头
	if ( Recv(lpBuf,stJNVProHeadMin::s_iSize)<=0 )
	{
		JTRACE("CJNVProtocolTCP Recv1 err\r\n");
		ResetSock();							// 重置SOCKET
		UnLockThread("CJNVProtocolTCP::OnClientMsg");
		return JNETErrRecv;
	}
	iHasRecv	+= stJNVProHeadMin::s_iSize;
	if ( m_stLastRecv.nType!=eJVNProBreath )
	{
		// 不是心跳,则需要收全了
		if ( Recv(lpBuf+stJNVProHeadMin::s_iSize,m_stLastRecv.s_iSize-stJNVProHeadMin::s_iSize)<=0 )
		{
		//	JTRACE("CJNVProtocolTCP Recv1 err\r\n");
			ResetSock();							// 重置SOCKET
			UnLockThread("CJNVProtocolTCP::OnClientMsg");
			return JNETErrRecv;
		}
		iHasRecv	+= m_stLastRecv.s_iSize-stJNVProHeadMin::s_iSize;
	}
	// 检查协议头是否正确 begin
//	if ( CheckNetHdr(&m_stLastRecv)!=HI_ERR_SUCCESS )
//	{
//		UnLockThread("CJNVProtocolTCP::OnClientMsg");
//		return JNETErrProtocol;
//	}
//	m_stLastRecv.m_dwSeq	&= 0x7FFFFFFF;		// 包号要去除第一位
	// 检查协议头是否正确 end
	// 接收余下的包数据 begin
	lpBuf		= NULL;
	iDataLen	= m_stLastRecv.nLength-iHasRecv;
	if ( iDataLen>0 )
	{
		lpBuf	= GetRecvBufSize(iDataLen+m_iDataOffset+128);		// 多申请128个字节的东东
		if ( lpBuf==NULL ) return JNETErrNoMem;
		lpBuf += m_iDataOffset;
		if ( Recv(lpBuf,iDataLen)<=0 )
		{
		//	JTRACE("CJNVProtocolTCP Recv2 err\r\n");
			ResetSock();						// 重置SOCKET
			UnLockThread("CJNVProtocolTCP::OnClientMsg");
			return JNETErrRecv;
		}
	}
	// 接收余下的包数据 end

	// 上一接收时间
	m_dwLastRecv		= JGetTickCount();

	// 消息派分
//	JTRACE("CJNVProtocolTCP::OnClientMsg %d\r\n",m_stLastRecv.nType);
	switch(m_stLastRecv.nType)
	{
	case eJVNProBreath:		OnRetBreath(lpBuf,iDataLen);			break;		// 心跳回应
	case eJVNProLoginRet:	OnRetLogin(lpBuf,iDataLen);				break;		// 登录回应
	default:
		if ( OnExtRecv(m_stLastRecv.nType,lpBuf,iDataLen)==0 )
		{
			m_dwSendSeq			= 0;					// 用于同步接收
			m_lpSendData		= NULL;					// 用于同步接收
			m_iSendDataLen		= 0;					// 用于同步接收
			JTRACE("CJNVProtocolTCP::OnClientMsg,unknow type:0x%04x\r\n",m_stLastRecv.nType);
		}
		break;
	}

	UnLockThread("CJNVProtocolTCP::OnClientMsg");
	return JNETErrSuccess;
}
// 用于扩展的接收处理
int CJNVProtocolTCP::OnExtRecv(int nType,void* lpBuf,int iDataLen)
{
	return 0;
}
// 登录状态
int CJNVProtocolTCP::OnConnect(int iRetType)
{
	DbgStrOut("OnConnect00 iRetType=%d\r\n",iRetType);

	m_dwLastSendBreath	= JGetTickCount();			// 上一呼吸时间
	m_dwLastRecv		= m_dwLastSendBreath;		// 上一接收时间
	m_iBreathSpan		= JNET_BREATH_SPAN;			// 呼吸间隔
	if ( iRetType==JLSOCK_ERR_TIMEOUT )
	{
		if( g_lpCmdTcp == this )
		{
			JTRACE("CJNVProtocolTCP::OnRetLogin: failed11  %d  %d !!!!!!!!!\r\n", g_lpCmdTcp, this);
			SendEvent(JNET_EET_LOGIN_ERROR,0,NULL,0);	// 登录失败
		}
	}
	else if ( iRetType==JLSOCK_SUCCESS )
	{
		SetNoBlock(false);
	}

	DbgStrOut("OnConnect iRetType=%d\r\n",iRetType);
	if ( iRetType==JLSOCK_SUCCESS )
	{
		GetRecvBufSize(JCONN_JNV_RCV_BUF);
		GetSendBufSize(JCONN_JNV_SND_BUF);
		DbgStrOut("OnConnect OK,m_bAutoLogin=%d\r\n",m_bAutoLogin);
		if ( m_bAutoLogin ) OnSendLogin();
	}
	else if ( iRetType==JLSOCK_ERR_TIMEOUT )
	{
		if( g_lpCmdTcp == this )
		{
			DbgStrOut("Connect TimeOut for err,ResetSock %s:%d\r\n",m_szDevIP,m_iDevPort);
			// 重置
			ResetSock();
		}
	}
	return CJLTcpClient::OnConnect(iRetType);
}
// 通知函数 end

// 登录
long CJNVProtocolTCP::Login(LPCTSTR lpszIP,int iDevPort,LPCTSTR lpszUser,LPCTSTR lpszPassword,int iTimeOut,fcbJEvent* pfcbEvent,void* pUserParam)
{
	int		iRet	= 0;

	DbgStrOut("CJNVProtocolTCP::Login %s,%d,%s,%s begin\r\n",
		lpszIP,iDevPort,lpszUser,lpszPassword);

	m_pfcbEvent		= pfcbEvent;			// 事件通知函数
	m_pUserParam	= pUserParam;			// 用户自定义参数
	strcpy(m_szDevIP,lpszIP);				// 设备IP
	m_iDevPort		= iDevPort;				// 设备端口
	m_iStreamPort	= m_iDevPort;			// 流端口
	strcpy(m_szUser,lpszUser);				// 用户名
	strcpy(m_szPwd,lpszPassword);			// 密码
	SetNoBlock(true);
	SetTimeOut(JSS_SO_T_CONN,iTimeOut*1000);// 连接超时(毫秒)
	
	SendEvent(JNET_EET_LOGINING,0,NULL,0);	// 登录中
	iRet = Connect();
	if ( iRet!=JLSOCK_SUCCESS )
	{
		DbgStrOut("CJNVProtocolTCP::Login end err with %d\r\n",iRet);
	}
	else
	{
		DbgStrOut("CJNVProtocolTCP::Login end success\r\n");
		m_hEventSync		= JCreateEvent(NULL, FALSE, FALSE, NULL);		// 同步事件
		if ( m_hEventSync ) ResetEvent(m_hEventSync);
	}
	return iRet;
}

// 登出
long CJNVProtocolTCP::Logout()
{
	if ( m_hEventSync )
	{
		CloseEventHandle(m_hEventSync);
		m_hEventSync = NULL;
	}
	DbgStrOut("CJNVProtocolTCP::Logout %s,%d,%s,%s\r\n",
		m_szDevIP,m_iDevPort,m_szUser,m_szPwd);
	OnSendLogout();
	Close();
	return JNETErrSuccess;
}

// 发送回调消息
long CJNVProtocolTCP::SendEvent(eJNetEvent eType,int iDataType,void* pEventData,int iDataLen)
{
	try
	{
		if ( m_pfcbEvent )
		{
			m_pfcbEvent((long)this,eType,iDataType,pEventData,iDataLen,m_pUserParam);
			return JNETErrSuccess;
		}
	}
	catch(...)
	{
		DbgStrOut("CJNVProtocolTCP::SendEvent %d,%d,%d\r\n",eType,iDataType,iDataLen);
	}
	return JNETErrSuccess;
}

// 复制连接信息
long CJNVProtocolTCP::CopyConn(LPJNVProtocolTCP lpProtocol)
{
	if ( lpProtocol==NULL ) return JNETErrParam;

	m_pfcbEvent		= lpProtocol->m_pfcbEvent;						// 事件通知函数
	m_pUserParam	= lpProtocol->m_pUserParam;						// 用户自定义参数
	StrNCpy(m_szDevIP,lpProtocol->m_szDevIP,JNET_IP_LEN);			// 设备IP
	m_iDevPort		= lpProtocol->m_iDevPort;						// 设备端口
	m_iStreamPort	= lpProtocol->m_iStreamPort;					// 流端口
	StrNCpy(m_szUser,lpProtocol->m_szUser,JNET_NAME_LEN*2-1);			// 用户名
	StrNCpy(m_szPwd,lpProtocol->m_szPwd,JNET_PWD_LEN*2-1);				// 密码
	memcpy(&m_stLoginInfo,&(lpProtocol->m_stLoginInfo),sizeof(m_stLoginInfo));
	return JNETErrSuccess;
}

// 用于操作的虚函数 begin
long CJNVProtocolTCP::OnSendLogin()
{
#ifdef _SINGLE_SERVER_
	stJNVProHead		stPro;
	stJNVC2SLoginReq	stLogin;
	int					iDataLen	= sizeof(stLogin);
	char*				lpBuf		= NULL;

	lpBuf		= (char*)GetSendBufSize(JCONN_JNV_SND_BUF);
	if (lpBuf==NULL) return JNETErrNoMem;
	// 登录信息 begin
	memset(&stLogin,0,sizeof(stLogin));
	StrNCpy(stLogin.szUserName, m_szUser, JNET_NAME_LEN);
	StrNCpy(stLogin.szUserPassword, m_szPwd, JNET_PWD_LEN);
	// 登录信息 end
	// 协议头 begin
	SetNetHdr(&stPro, stPro.s_iSize+iDataLen, eJVNProLoginReq, stPro.GetSeq());
	// 协议头 end

	memcpy(lpBuf,&stPro,stPro.s_iSize);
	memcpy(lpBuf+stPro.s_iSize,&stLogin,iDataLen);
	Send(lpBuf,stPro.nLength);
	return JNETErrSuccess;

#else	
	stJNVProHead		stPro;
	stJNVC2SLoginReqEx	stLogin;

	int					iDataLen	= sizeof(stLogin);
	char*				lpBuf		= NULL;

	lpBuf		= (char*)GetSendBufSize(JCONN_JNV_SND_BUF);
	if (lpBuf==NULL) return JNETErrNoMem;
	// 登录信息 begin
	memset(&stLogin,0,sizeof(stLogin));
	StrNCpy(stLogin.szUserName, m_szUser, JNET_NAME_LEN*2-1);
	StrNCpy(stLogin.szUserPassword, m_szPwd, JNET_PWD_LEN*2-1);

	stLogin.nClientType = E_NV_ClientType_AndriodClient;

	if (m_nServerType != eJNVCenterServer)
	{
		stLogin.nClientType = E_NV_ClientType_Device;
	}

	StrNCpy(stLogin.szClientVersion,"1.0.0.0",8);
	
	// 登录信息 end

	// 协议头 begin
	SetNetHdr(&stPro, stPro.s_iSize+iDataLen, eJVNProLoginReq, stPro.GetSeq());
	// 协议头 end

	memcpy(lpBuf,&stPro,stPro.s_iSize);
	memcpy(lpBuf+stPro.s_iSize,&stLogin,iDataLen);
	Send(lpBuf,stPro.nLength);
	return JNETErrSuccess;
#endif
	
}

long CJNVProtocolTCP::OnSendLogout()
{
	stJNVProHead		stPro;

	SendEvent(JNET_EET_LOGOUT,0,NULL,0);	// 登出
	SetNetHdr(&stPro, stPro.s_iSize, eJVNProLogoutReq, stPro.GetSeq());
	Send(&stPro,stPro.nLength);

	return JNETErrSuccess;
}

long CJNVProtocolTCP::OnSendBreath(DWORD dwTick)
{
	stJNVProHead		stPro;

	SetNetHdr(&stPro, stJNVProHeadMin::s_iSize, eJVNProBreath, 0);
	Send(&stPro,stJNVProHeadMin::s_iSize);

	m_dwLastSendBreath = dwTick;
	return JNETErrSuccess;
}
// 用于操作的虚函数 end

// 消息派分结果 begin
// 登录应答
void CJNVProtocolTCP::OnRetLogin(void* pData,int iLen)
{
	stJNVC2SLoginRet*	pLoginInfo	= (stJNVC2SLoginRet*)pData;
	
	if ( m_stLastRecv.nParameter1!=0 )
	{
		// 设置心跳间隔,并回调事件
		if( g_lpCmdTcp == this )
		{
			JTRACE("CJNVProtocolTCP::OnRetLogin: failed  %d !!!!!!!!!\r\n", m_stLastRecv.nParameter1);
			SendEvent(JNET_EET_LOGIN_ERROR,m_stLastRecv.nParameter1,(void*)pData,iLen);
		}
		m_dwLastSendBreath	= JGetTickCount();
		m_iBreathSpan		= JNET_BREATH_SPAN;				// 2*3秒后重连
	}
	else
	{
		// 登录信息
		memcpy(&m_stLoginInfo,pLoginInfo,sizeof(m_stLoginInfo));
		// 设置心跳间隔,并回调事件
		if( g_lpCmdTcp == this )
		{
			JTRACE("CJNVProtocolTCP::OnRetLogin: succ %d  %d !!!!!!!!!\r\n", g_lpCmdTcp, this);
			SendEvent(JNET_EET_LOGIN_OK,0,(void*)pData,iLen);	// 登录成功
		}
		m_dwLastSendBreath	= JGetTickCount();
		m_iBreathSpan		= JNET_BREATH_SPAN;
	}
	JTRACE("CJNVProtocolTCP::OnRetLogin: len %d\r\n",iLen);
}
// 呼吸应答
void CJNVProtocolTCP::OnRetBreath(void* pData,int iLen)
{
	//OnSendBreath(JGetTickCount());
//	JTRACE("CJNVProtocolTCP::OnRetBreath: %d\r\n",iLen);
}
// 消息派分结果 end
