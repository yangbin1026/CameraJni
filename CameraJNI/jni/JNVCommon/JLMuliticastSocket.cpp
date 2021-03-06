// JLMuliticastSocket.cpp: implementation of the CJLMuliticastSocket class.
//
//////////////////////////////////////////////////////////////////////

#include "JLMuliticastSocket.h"
#undef	LOG_TAG
#define	LOG_TAG		"JLMuliticastSocket.cpp"

//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////

CJLMuliticastSocket::CJLMuliticastSocket()
{
	m_bConnected	= false;
	m_iSockType		= SOCK_DGRAM;
	m_iSleepMS		= 1;		// 休眠时长
}

CJLMuliticastSocket::~CJLMuliticastSocket()
{

}

int CJLMuliticastSocket::Close()
{
	int		iRet	= 0;

	LockThread("CJLMuliticastSocket::Close");
	iRet = CJLSocket::Close();
	UnLockThread("CJLMuliticastSocket::Close");
	m_tcRecv.StopThread(true);

	return iRet;
}

bool CJLMuliticastSocket::IsConnected()
{
	return m_bConnected;
}

int CJLMuliticastSocket::AddGroup(LPCTSTR lpszMuIP,int iMuPort,int iLocalPort)
{
	int optval = 1;

	// 防止没加上广播网关
	#if (defined _WIN32) || (defined _WIN64)
	#else
	system("route add -net 224.0.0.0 netmask 240.0.0.0 dev eth0 & ");
	#endif
	if ( InitHandle(m_iSockType)!=JLSOCK_SUCCESS ) return JLSOCK_ERR_INIT;

	if ( setsockopt(m_hSocket, SOL_SOCKET,	SO_REUSEADDR, (char *) & optval, sizeof(optval))==SOCKET_ERROR )
	{
		JTRACE("ERRROR : setsockopt(SO_REUSEADDR) \r\n");
		return JLSOCK_ERR_OPERATE;
	}
	
	m_addrLocal.sin_addr.s_addr = htonl(INADDR_ANY);
	m_addrLocal.sin_family = AF_INET;
	m_addrLocal.sin_port = htons(iLocalPort);
	if(bind(m_hSocket, (sockaddr *)&m_addrLocal, sizeof(m_addrLocal)) == SOCKET_ERROR)
	{
		JTRACE("ERRROR : bind \r\n");
		return JLSOCK_ERR_BIND;
	}
	
	m_addrGroup.sin_addr.s_addr	= inet_addr(lpszMuIP);			// 连接的远程IP
	m_addrGroup.sin_family		= AF_INET;
	m_addrGroup.sin_port		= htons(iMuPort);
	ip_mreq mcast;
	mcast.imr_multiaddr.s_addr = m_addrGroup.sin_addr.s_addr;
	mcast.imr_interface.s_addr = m_addrLocal.sin_addr.s_addr;
	if (setsockopt(m_hSocket, IPPROTO_IP, IP_ADD_MEMBERSHIP, (char *)&mcast, sizeof(mcast)) == SOCKET_ERROR)
	{
		JTRACE("ERRROR : setsockopt(IP_ADD_MEMBERSHIP) \r\n");
		return JLSOCK_ERR_OPERATE;
	}
	
	// optval = 16;
	optval = 1;
	if (setsockopt(m_hSocket, IPPROTO_IP, IP_MULTICAST_TTL, (char *)&optval, sizeof(optval)) == SOCKET_ERROR)
	{
		JTRACE("ERRROR : setsockopt(IP_MULTICAST_TTL) \r\n");
		return JLSOCK_ERR_OPERATE;
	}
	
	// 启动 RECV 线程
	if ( m_tcRecv.GetThreadState()!=THREAD_STATE_STOP )
	{
		return JLSOCK_SUCCESS;
	}
	m_tcRecv.SetOwner(this);							// 设置所有者,供线程函数使用
	if ( m_tcRecv.StartThread(RunRecvThread)==0 )
	{
		Close();
		return JLSOCK_ERR_RECV_TH;
	}

	return JLSOCK_SUCCESS;
}

int CJLMuliticastSocket::SendMuMsg(void* lpData,int iLen)
{	
	int			iRet	= 0;

	if ( m_hSocket==INVALID_SOCKET )
	{
		return JLSOCK_ERR_SEND;
	}

	iRet = sendto(m_hSocket,(const char*)lpData,iLen,MSG_NOSIGNAL,(sockaddr *)&m_addrGroup, sizeof(sockaddr));

	return iRet;
}

void CJLMuliticastSocket::LockThread(LPCTSTR lpszNotice)
{
//	if ( lpszNotice ) JTRACE("CJLMuliticastSocket::LockThread :%s\r\n",lpszNotice);
//	else JTRACE("CJLMuliticastSocket::LockThread \r\n");
	m_mutexThread.Lock();
}

void CJLMuliticastSocket::UnLockThread(LPCTSTR lpszNotice)
{
//	if ( lpszNotice ) JTRACE("CJLMuliticastSocket::UnLockThread :%s\r\n",lpszNotice);
//	else JTRACE("CJLMuliticastSocket::UnLockThread \r\n");
	m_mutexThread.Unlock();
}

// RECV 线程
fJThRet CJLMuliticastSocket::RunRecvThread(void* pParam)
{	
	int					iIsRun				= 0;				// 是否需要运行
	CJLThreadCtrl*		pThreadCtrl			= NULL;				// 对应用线程控制器
	CJLMuliticastSocket*		pClient				= NULL;				// 客户端指针

	// 初始化参数 begin
	pThreadCtrl	= (CJLThreadCtrl*)pParam;
	if ( pThreadCtrl==NULL )
	{
		return 0;
	}
	pClient		= (CJLMuliticastSocket*)pThreadCtrl->GetOwner();
	if ( pClient==NULL )
	{
		pThreadCtrl->SetThreadState(THREAD_STATE_STOP);		// 运行状态
		return 0;
	}
	// 初始化参数 end

	// 进行 Recv 操作 begin
	iIsRun	= 1;											// 需要运行
	pClient->m_mutexThread.CreateMutex();
	pClient->m_bConnected = true;
	while(iIsRun)
	{
		// 检查当前状态 begin
		if ( pThreadCtrl->GetNextAction()==THREAD_STATE_STOP )
		{
			pThreadCtrl->SetThreadState(THREAD_STATE_STOP);
			pClient->Close();
			// 结束 或 等待结束,马上跳出循环
			iIsRun = 0;										// 不再运行
			break;
		}
		// 检查当前状态 end
		
		if ( iIsRun )
		{
			// 执行 ReadSelect begin
			pClient->LockThread("CJLMuliticastSocket::RunRecvThread");
			if ( pClient->OnCheckSocket()<0 )
			{
				pThreadCtrl->SetThreadState(THREAD_STATE_STOP);
				pClient->Close();
				// 客户端离开了
				iIsRun = 0;								// 不再运行
				pClient->UnLockThread("CJLMuliticastSocket::RunRecvThread");
				break;
			}
			pClient->UnLockThread("CJLMuliticastSocket::RunRecvThread");
			// 执行 ReadSelect end
			
			// 如果句柄不可用,多休息一会 begin
			if ( pClient->m_hSocket == INVALID_SOCKET )
			{
				JSleep(pClient->m_iSleepMS*10);
			}
			else
			{
				JSleep(pClient->m_iSleepMS);
			}
			// 如果句柄不可用,多休息一会 end
		}
	}
	// 进行 Recv 操作 end
	pThreadCtrl->SetThreadState(THREAD_STATE_STOP);

	pClient->m_bConnected = false;
	pClient->m_mutexThread.CloseMutex();
	return 0;
}

int CJLMuliticastSocket::OnClientMsg(void* pParam)
{
	int		iRecv	= 0;
	LPBYTE	lpBuf	= GetRecvBufSize(1500);

	iRecv = RecvNoBlock(lpBuf,1500);
	if ( iRecv<=0 )
	{
		Close();
	}
	else
	{
		JTRACE("CJLMuliticastSocket::OnClientMsg %s -- %d\r\n",lpBuf,iRecv);
	}
	return JLSOCK_SUCCESS;
}

