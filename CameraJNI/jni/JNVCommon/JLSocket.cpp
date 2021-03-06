// JLSocket.cpp: implementation of the CJLSocket class.
//
//////////////////////////////////////////////////////////////////////

#include "JLSocket.h"

#ifdef _DEBUG
#undef THIS_FILE
static char THIS_FILE[]=__FILE__;
//#define new DEBUG_NEW
#endif

//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////
CJLSocket::CJLSocket(SOCKET hSocket)
{
	m_hSocket		= hSocket;
	m_uPort			= 0;						// 本地端口
	memset(m_szAddr,0,sizeof(m_szAddr));		// 地址
	m_bNoBlock		= false;					// 是否为非阻塞式
	m_bConnecting	= false;					// 正在连接中
	m_iSockType		= 0;						// Socket的类型
	
	m_dwConnTick	= 0;						// 登录的时间
	m_iConnTO		= 5000;						// 连接超时(毫秒)
	m_iRecvTO		= 5000;						// 接收超时(毫秒)
	m_iSendTO		= 5000;						// 发送超时(毫秒)

	m_lpSendBuf		= NULL;
	m_iSendBufLen	= 0;
	m_lpRecvBuf		= NULL;
	m_iRecvBufLen	= 0;

	m_pcbCheckState	= NULL;						// 检查回调函数
	m_pcbRecvMsg	= NULL;						// 接收回调函数
	m_lpUserData	= NULL;						// 用户自定义的数据域
	
	m_dwLastRecv	= 0;						// 上次接收时间
	
	m_lHasRecv		= 0;						// 已经接收数
}

CJLSocket::~CJLSocket()
{
	Close();
	GetSendBufSize(-1);							// 清除缓存
	GetRecvBufSize(-1);							// 清除缓存
}

// 启动SOCKET资源
int CJLSocket::InitSocket()
{
#if (defined _WIN32) || (defined _WIN64)
	WSADATA wsdata;
	WORD wVersionRequested = MAKEWORD(2, 2);
	return WSAStartup(wVersionRequested, &wsdata);
#else
	return 0;
#endif
};

// 关闭SOCKET资源
int CJLSocket::CleanSocket()
{
#if (defined _WIN32) || (defined _WIN64)
	return WSACleanup();
#else
	return 0;
#endif
}

// 转换IP字符串及端口号
UINT CJLSocket::GetIPAndPort(const sockaddr_in* addr,char* szIP)
{
	sockaddr*	pAdd	= NULL;
	UINT		uPort	= 0;
	int			iIP[4]	= {0};

	if ( addr==NULL )
	{
		return JLSOCK_ERR_PARAM;
	}
	pAdd = (sockaddr*)addr;
	memcpy(&uPort,pAdd->sa_data,2);
	uPort = ntohs(uPort);
	if ( szIP )
	{
		memcpy(&iIP[0],pAdd->sa_data+2,1);
		memcpy(&iIP[1],pAdd->sa_data+3,1);
		memcpy(&iIP[2],pAdd->sa_data+4,1);
		memcpy(&iIP[3],pAdd->sa_data+5,1);
		sprintf_s(szIP, 15, "%d.%d.%d.%d", iIP[0],iIP[1],iIP[2],iIP[3]);
	}

	return uPort;
}

// 设置sockaddr_in
int CJLSocket::SetIPAndPort(sockaddr_in* addr,char* szIP,UINT uPort)
{
	if ( addr==NULL )
	{
		return JLSOCK_ERR_PARAM;
	}

	addr->sin_family		= AF_INET;					// 协议类型
	addr->sin_port			= htons(uPort);				// 将端口转换成网络能识别的格式
	addr->sin_addr.s_addr	= inet_addr(szIP);			// 连接的远程IP

	return JLSOCK_SUCCESS;
}

// 获取本机IP
hostent* CJLSocket::GetLocalIP()
{
	char		szHostName[128]	= {0};
	hostent*	pHost			= NULL; 
 
	if( gethostname(szHostName, 128) == 0 )
	{
		pHost = gethostbyname(szHostName);
	}
	return pHost;
}

// 获取空闲的端口
int CJLSocket::GetValidSocketPort(bool bUdp)
{
	int			iLen	= 0;
	SOCKET		sockUdp	= INVALID_SOCKET;
	sockaddr_in addrLocal;
	
	if ( bUdp ) 
	{
		sockUdp = socket(AF_INET,SOCK_DGRAM,0);
	}
	else 
	{
		sockUdp = socket(AF_INET,SOCK_STREAM,0);
	}
	if ( sockUdp==INVALID_SOCKET ) return -1;

	addrLocal.sin_family = AF_INET;						// 协议类型
	addrLocal.sin_port = htons(0);						// 将端口转换成网络能识别的格式
	addrLocal.sin_addr.s_addr = htonl(INADDR_ANY);		// 梆定本机
	memset(&(addrLocal.sin_zero),0,sizeof(addrLocal.sin_zero));		// 清空结构体余下的内容
	if ( bind(sockUdp, (sockaddr *)&addrLocal, sizeof(sockaddr))!=SOCKET_ERROR )
	{
		iLen	= sizeof(sockaddr);
		getsockname(sockUdp,(sockaddr *)&addrLocal, (socklen_t*)&iLen);
		// 梆定成功
		close(sockUdp);
		return addrLocal.sin_port;
	}
	return 0;
}

// 域名解释
DWORD CJLSocket::DomainToIP(const char* szDomain)
{
	DWORD			dwRet		= 0;
	struct hostent*	pHostEnt	= NULL;

	dwRet = inet_addr(szDomain);
	if(dwRet == INADDR_NONE)
	{
		pHostEnt = NULL;
		pHostEnt = gethostbyname(szDomain);
		if(pHostEnt != NULL)
		{
			dwRet = *((unsigned long*)pHostEnt->h_addr_list[0]);
		}
		else
		{
			return 0;
		}
	}

	return dwRet;
}

// 初始化SOCKET句柄
int CJLSocket::InitHandle(int iType)
{
	if ( iType==0 ) return JLSOCK_ERR_PARAM;

	// 如果句柄有效,则返回成功
	if ( m_hSocket!=INVALID_SOCKET )
	{
		return JLSOCK_SUCCESS;
	}

	if ( iType==SOCK_STREAM ) m_hSocket = socket(AF_INET,iType,IPPROTO_TCP);
	else m_hSocket = socket(AF_INET,iType,IPPROTO_UDP);
	if ( m_hSocket == INVALID_SOCKET ) 
	{
		return JLSOCK_ERR_INIT;
	}

	m_iSockType		= iType;						// Socket的类型
	return JLSOCK_SUCCESS;
}

// 根据传入的句柄,构造对象
int CJLSocket::PasteHandle(SOCKET hSocket)
{
	if ( m_hSocket!=INVALID_SOCKET )
	{
		Close();
	}

	m_hSocket = hSocket;
	if ( m_hSocket==INVALID_SOCKET ) 
	{
		return JLSOCK_ERR_INIT;
	}

	return JLSOCK_SUCCESS;
}

// 关闭SOCKET,清理内存
int CJLSocket::Close()
{
	SAFE_CLOSE_SOCK(m_hSocket);
	m_bNoBlock	= false;							// 是否为阻塞式

	return JLSOCK_SUCCESS;
}

// 常用的fcntl begin
// 设置为非阻塞方式
int CJLSocket::SetNoBlock(bool bNoBlock)
{
	int iRet = 0;
	unsigned long	argp = 1;					// ioctlsocket 的参数

	if ( InitHandle(m_iSockType)!=JLSOCK_SUCCESS ) return JLSOCK_ERR_INIT;
	if ( m_bNoBlock == bNoBlock ) return JLSOCK_SUCCESS;

#if (defined _WIN32) || (defined _WIN64)
	if ( bNoBlock ) argp = 1;
	else argp = 0;
	iRet = ioctlsocket(m_hSocket, FIONBIO, &argp);	
#else
	argp = fcntl(m_hSocket, F_GETFL, 0);                       // 获取文件的flags值
	if ( bNoBlock ) argp |= O_NONBLOCK;
	else argp &= ~O_NONBLOCK;
	iRet = fcntl(m_hSocket, F_SETFL, argp);
#endif

	if ( iRet==0 )
	{
		m_bNoBlock = bNoBlock;
		
	//	if ( bNoBlock ) JTRACE("socket:%d SetNoBlock OK\r\n",m_hSocket);
	//	else JTRACE("socket:%d SetBlock OK\r\n",m_hSocket);
		return JLSOCK_SUCCESS;
	}
	else
	{
	//	if ( bNoBlock ) JTRACE("socket:%d SetNoBlock Err\r\n",m_hSocket);
	//	else JTRACE("socket:%d SetBlock Err\r\n",m_hSocket);
		return JLSOCK_ERR_NOBLOCK;
	}
}
bool CJLSocket::IsNoBlock()
{
	return m_bNoBlock;
}
int CJLSocket::SetSoBuf(int iType,int iSize)
{
	int iSockBuf	= iSize;

	if ( InitHandle(m_iSockType)!=JLSOCK_SUCCESS ) return JLSOCK_ERR_INIT;
	if ( iType==JSS_SO_T_SEND )
	{
		// 发送
		if (setsockopt(m_hSocket,SOL_SOCKET,SO_SNDBUF,(const char*)&iSockBuf,sizeof(iSockBuf))==SOCKET_ERROR)
		{
			return JLSOCK_ERR_OPERATE;
		}
	}
	else if ( iType==JSS_SO_T_RECV )
	{
		// 接收
		if (setsockopt(m_hSocket,SOL_SOCKET,SO_RCVBUF,(const char*)&iSockBuf,sizeof(iSockBuf))==SOCKET_ERROR)
		{
			return JLSOCK_ERR_OPERATE;
		}
	}
	else return JLSOCK_ERR_PARAM;
	return JLSOCK_SUCCESS;
}
int CJLSocket::SetTimeOut(int iType,int iMilliseconds)
{
	int iTimeOut	= iMilliseconds;

	if ( iType==JSS_SO_T_CONN )
	{
		// 连接
		m_iConnTO	= iMilliseconds;
	}
	else if ( iType==JSS_SO_T_SEND )
	{
		// 发送
		m_iSendTO	= iMilliseconds;
		if (setsockopt(m_hSocket,SOL_SOCKET,SO_SNDTIMEO,(const char*)&iTimeOut,sizeof(iTimeOut))==SOCKET_ERROR)
		{
			return JLSOCK_ERR_OPERATE;
		}
	}
	else if ( iType==JSS_SO_T_RECV )
	{
		// 接收
		m_iRecvTO	= iMilliseconds;
		if (setsockopt(m_hSocket,SOL_SOCKET,SO_RCVTIMEO,(const char*)&iTimeOut,sizeof(iTimeOut))==SOCKET_ERROR)
		{
			return JLSOCK_ERR_OPERATE;
		}
	}
	else return JLSOCK_ERR_PARAM;
	return JLSOCK_SUCCESS;
}
// 常用的fcntl end

// 获取相应的句柄
SOCKET CJLSocket::GetHandle()
{
	return m_hSocket;
}

// 获取远端地址(对服务器方式不可用)
int	CJLSocket::GetPeerAddr(sockaddr_in* addr,char* szStr)
{	
    socklen_t		len		= sizeof(sockaddr_in);
	sockaddr_in*	pAddr	= NULL;
	sockaddr_in		stAddr;
	UINT			uPort	= 0;

	if ( addr ) pAddr = addr;
	else pAddr = &stAddr;

    memset( pAddr, 0, sizeof(sockaddr_in) );
	if ( getpeername(m_hSocket,(sockaddr*)pAddr,&len)<0 )
	{
		return JLSOCK_ERR_PEER;
	}
	uPort = GetIPAndPort(pAddr,szStr);

	return uPort;
}

// 获取本地地址
int	CJLSocket::GetLocalAddr(sockaddr_in* addr,char* szStr)
{
    socklen_t		len		= sizeof(sockaddr_in);
	sockaddr_in*	pAddr	= NULL;
	sockaddr_in		stAddr;
	UINT			uPort	= 0;

	if ( addr ) pAddr = addr;
	else pAddr = &stAddr;

    memset( pAddr, 0, sizeof(sockaddr_in) );
	if ( getsockname(m_hSocket,(sockaddr*)pAddr,&len)<0 )
	{
		return JLSOCK_ERR_PEER;
	}
	uPort = GetIPAndPort(pAddr,szStr);

	return uPort;
}

// 发送 byBuf 中的消息,发送长度为 iBufLen ,返回实际发送的长度
int CJLSocket::Send(const void* byBuf,int iBufLen)
{
	int	iRet	= 0;
	int	iHasSend= 0;

	if ( m_hSocket==INVALID_SOCKET )
	{
		return JLSOCK_ERR_SEND;
	}

	if ( iBufLen>=m_iSendBufLen )
	{
		DbgStrOut("WARN:XXXXXXXXXXXXXXXXX CJLSocket::Send %d:%s\r\n",iBufLen,byBuf);
	}
	while(iHasSend<iBufLen)
	{
		iRet = send(m_hSocket,(const char*)(byBuf)+iHasSend,iBufLen-iHasSend,MSG_NOSIGNAL);
		if ( iRet<=0 ) break;
		iHasSend+=iRet;
	}

	return iHasSend;
}

// 接收消息到 byBuf 中,接收长度为 iBufLen ,返回实际接收的长度
int CJLSocket::Recv(void* byBuf,int iRecvLen)
{
	int	iRet	= 0;
	int	iHasRecv= 0;

	if (m_lHasRecv>1024*1024*100)
	{
		// DEMO版本只能接收100M的数据
		return JLSOCK_ERR_RECV;
	}

	if ( m_hSocket==INVALID_SOCKET )
	{
		return JLSOCK_ERR_RECV;
	}

	while(iHasRecv<iRecvLen)
	{
		iRet = recv(m_hSocket,(char*)(byBuf)+iHasRecv,iRecvLen-iHasRecv,0);
		if ( iRet<=0 )
		{
			break;
		}
		iHasRecv+=iRet;
		m_lHasRecv+=iRet;						// 已经接收数
	}

	return iHasRecv;
}
// 接收消息到 byBuf 中,接收长度为 iBufLen ,返回实际接收的长度
int CJLSocket::RecvNoBlock(void* byBuf,int iRecvLen)
{
	if ( m_hSocket==INVALID_SOCKET )
	{
		return JLSOCK_ERR_RECV;
	}
	int iRet	= recv(m_hSocket,(char*)byBuf,iRecvLen,0);
	return iRet;
}
// 发送 byBuf 中的消息,发送长度为 iBufLen ,返回实际发送的长度(用于UDP)
int CJLSocket::SendTo(const void* byBuf,int iBufLen,const char* szIP,UINT uPort)
{
	int			iRet	= 0;
	sockaddr_in	serv_addr;

	if ( m_hSocket==INVALID_SOCKET )
	{
		return JLSOCK_ERR_SEND;
	}

	serv_addr.sin_family		= AF_INET;					// 协议类型
	serv_addr.sin_port			= htons(uPort);				// 将端口转换成网络能识别的格式
	serv_addr.sin_addr.s_addr	= inet_addr(szIP);			// 连接的远程IP

	iRet = sendto(m_hSocket,(const char*)byBuf,iBufLen,MSG_NOSIGNAL,(sockaddr *)&serv_addr, sizeof(sockaddr));

	return iRet;
}

// 接收消息到 byBuf 中,接收长度为 iBufLen ,返回实际接收的长度(用于UDP)
int CJLSocket::RecvFrom(void* byBuf,int iRecvLen,sockaddr_in* addr)
{
	int			iRet	= 0;
	socklen_t	iLen	= 0;

	if ( m_hSocket==INVALID_SOCKET )
	{
		return JLSOCK_ERR_RECV;
	}

	iLen = sizeof(sockaddr_in);
	iRet = recvfrom(m_hSocket,(char*)byBuf,iRecvLen,0,(sockaddr *)addr,&iLen);

	return iRet;
}

// 检查SOCKET的状态,返回值见JSS_CAN_XXXX
int CJLSocket::OnCheckSocket()
{
	fd_set	fdRead;							// 是否可读
	fd_set	fdWrite;						// 是否可写
	fd_set	fdExp;							// 是否异常
	int		iSelResult	= SOCKET_ERROR;		// select 返回结果
	DWORD	dwTick		= 0;				// 当前Tick

	dwTick		= JGetTickCount();			// 当前Tick
	
	if ( m_pcbCheckState )
	{
		if ( m_pcbCheckState((void*)this, dwTick, m_lpUserData)!=0 )
		{
			return 0;
		}
	}
	// 检查本对象的状态,返回0表示 OnCheckSocket 继续,否则退出检查线程
	if ( OnCheckState(dwTick)!=0 )
	{
		return 0;
	}

	// 如果句柄不可用
	if ( m_hSocket == INVALID_SOCKET )
	{
		return 0;
	}

	// 连接超时判断
	if ( IsOverTime(m_dwConnTick,dwTick,m_iConnTO) )
	{
		// 连接超时了,由各派生类自行决定
		OnConnect(JLSOCK_ERR_TIMEOUT);
		return 0;
	}

	// 设置超时
	struct timeval timeout;
	timeout.tv_sec	= 0;
	timeout.tv_usec = 1*1000; 
	FD_ZERO(&fdRead); 
	FD_SET(m_hSocket, &fdRead);
	FD_ZERO(&fdWrite); 
	FD_SET(m_hSocket, &fdWrite);
	FD_ZERO(&fdExp); 
	FD_SET(m_hSocket, &fdExp);

	if ( m_hSocket==INVALID_SOCKET ) return 0;
	iSelResult = select(m_hSocket + 1, &fdRead, &fdWrite, &fdExp, &timeout);
	switch (iSelResult)
	{
	case SOCKET_ERROR :
		break;
	case 0: 
		break; 
	default: 
	//	iSelResult = 0;
		if ( FD_ISSET(m_hSocket,&fdRead) )
		{
			if ( m_bConnecting )
			{
				// 正在连接中
				m_dwConnTick	= 0;
				m_bConnecting	= false;
				m_dwLastRecv	= 0;				// 上次接收时间
				OnConnect(JLSOCK_SUCCESS);
			}
			else
			{
				iSelResult|=JSS_CAN_READ;
				m_dwLastRecv	= dwTick;			// 上次接收时间
				OnClientMsg(this);
			}
		}
		if ( m_hSocket==INVALID_SOCKET ) break;
		if ( FD_ISSET(m_hSocket,&fdWrite) )
		{
			if ( m_bConnecting )
			{
				// 正在连接中
				m_dwConnTick	= 0;
				m_bConnecting	= false;
				m_dwLastRecv	= 0;				// 上次接收时间
				OnConnect(JLSOCK_SUCCESS);
			}
		}
		if ( m_hSocket==INVALID_SOCKET ) break;
		if ( FD_ISSET(m_hSocket,&fdExp) )
		{
			// 连接异常了,由各派生类自行决定
            JTRACE("FD_ISSET(%d,&fdExp)\r\n",m_hSocket);
			OnConnect(JLSOCK_ERR_TIMEOUT);
		}
		break;
	}
	return iSelResult;
}

// 检查本对象的状态,返回0表示 OnCheckSocket 继续,否则退出检查线程
int CJLSocket::OnCheckState(DWORD dwTick)
{
	return 0;
}

// 登录成功
int CJLSocket::OnConnect(int iRetType)
{
	m_dwConnTick	= 0;
	m_bConnecting	= false;						// 非连接中
	if ( iRetType!=JLSOCK_SUCCESS )
	{
		return -1;
	}
	else
	{
		SetTimeOut(JSS_SO_T_SEND,m_iSendTO);		// 发送超时(毫秒)
		SetTimeOut(JSS_SO_T_RECV,m_iRecvTO);		// 接收超时(毫秒)
		return JLSOCK_SUCCESS;
	}
}

// 客户端接入
void* CJLSocket::OnClientEnter(SOCKET hClient)
{
	return NULL;
}

// 客户端离开
int CJLSocket::OnClientExit(void* pParam)
{
	return JLSOCK_SUCCESS;
}

// 客户端消息到达
int CJLSocket::OnClientMsg(void* pParam)
{
	LPBYTE	lpBuf	= NULL;
	if ( m_pcbRecvMsg ) 
	{
		return m_pcbRecvMsg(pParam,m_lpUserData);
	}
	else
	{
		lpBuf	= GetRecvBufSize(1500);
		if ( RecvNoBlock(lpBuf,1500)<=0 )
		{
			Close();
		}
	}
	return JLSOCK_SUCCESS;
}

void CJLSocket::SetCheckCallback(fcbRecvMsg* pcbRecvMsg,fcbCheckState* pcbCheckState,void* lpUserData)
{
	m_pcbRecvMsg	= pcbRecvMsg;
	m_pcbCheckState	= pcbCheckState;
	m_lpUserData	= lpUserData;
}

LPBYTE CJLSocket::GetSendBufSize(int iLen)
{
	LPBYTE	lpNew	= NULL;
	try
	{
		if ( iLen<0 )
		{
			SAFE_DELETE(m_lpSendBuf);
			m_iSendBufLen = 0;
			return NULL;
		}
		if ( iLen==0 ) return m_lpSendBuf;
		if ( iLen<=m_iSendBufLen ) return m_lpSendBuf;
		// 需要重新生成
		lpNew = new BYTE[iLen];
		if ( lpNew )
		{
			if ( m_lpSendBuf ) memcpy(lpNew,m_lpSendBuf,m_iSendBufLen);
			m_iSendBufLen = iLen;
			SAFE_DELETE(m_lpSendBuf);
			m_lpSendBuf = lpNew;
			return m_lpSendBuf;
		}
		else
		{
			return NULL;
		}
	}
	catch(...)
	{
		DbgStrOut("try-catch CJLSocket::GetSendBufSize:%d\r\n",iLen);
		return NULL;
	}
}

LPBYTE CJLSocket::GetRecvBufSize(int iLen)
{
	LPBYTE	lpNew	= NULL;
	try
	{
		if ( iLen<0 )
		{
			SAFE_DELETE(m_lpRecvBuf);
			m_iRecvBufLen = 0;
			return NULL;
		}
		if ( iLen==0 ) return m_lpRecvBuf;
		if ( iLen<=m_iRecvBufLen ) return m_lpRecvBuf;
		// 需要重新生成
		lpNew = new BYTE[iLen];
		if ( lpNew )
		{
			if ( m_lpRecvBuf ) memcpy(lpNew,m_lpRecvBuf,m_iRecvBufLen);
			m_iRecvBufLen = iLen;
			SAFE_DELETE(m_lpRecvBuf);
			m_lpRecvBuf = lpNew;
			return m_lpRecvBuf;
		}
		else
		{
			return NULL;
		}
	}
	catch(...)
	{
		DbgStrOut("try-catch CJLSocket::GetRecvBufSize:%d\r\n",iLen);
		return NULL;
	}
}
