// JLSocketClient.cpp: implementation of the CJLSocketClient class.
//
//////////////////////////////////////////////////////////////////////

#include "JLSocketClient.h"

//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////
CJLSocketClient::CJLSocketClient(SOCKET hSocket)
	: CJLSocket(hSocket)
{
}

CJLSocketClient::~CJLSocketClient()
{

}

int	CJLSocketClient::Connect(const char* szIP,UINT uPort,int iType)
{
	int			iRet	= 0;
	sockaddr_in	serv_addr;

	if ( m_hSocket==INVALID_SOCKET )
	{
		if ( JLSOCK_SUCCESS != InitHandle(iType) )
		{
			return JLSOCK_ERR_CONNECT;
		}
	}

	serv_addr.sin_family		= AF_INET;					// 协议类型
	serv_addr.sin_port			= htons(uPort);				// 将端口转换成网络能识别的格式
	serv_addr.sin_addr.s_addr	= DomainToIP(szIP);			// 连接的远程IP

	m_bConnecting	= true;									// 正在连接中
	m_dwConnTick	= JGetTickCount();						// 登录时间
	iRet = connect(m_hSocket,(sockaddr*)&serv_addr, sizeof(sockaddr));
	if ( iRet==0 )
	{
		if ( IsNoBlock()==false )
		{
			OnConnect(JLSOCK_SUCCESS);
		}
		else
		{
			m_bConnecting	= false;						// 正在连接中
		}
		return JLSOCK_SUCCESS;
	}
	else
	{
		if ( IsNoBlock() )
		{
			return JLSOCK_SUCCESS;
		}
		else
		{
			return JLSOCK_ERR_CONNECT;
		}
	}

	return iRet;
}

// 梆定本机端口
int CJLSocketClient::Bind(UINT uPort,const char* pAdd,int iType)
{
	sockaddr_in LocalAddr;
	LocalAddr.sin_family = AF_INET;						// 协议类型
	LocalAddr.sin_port = htons(uPort);					// 将端口转换成网络能识别的格式
	if (pAdd == NULL||strlen(pAdd)==0)
	{
		LocalAddr.sin_addr.s_addr = htonl(INADDR_ANY);	// 梆定本机
	}
	else
	{
		LocalAddr.sin_addr.s_addr = inet_addr(pAdd);
	}
	memset(&(LocalAddr.sin_zero),0,sizeof(LocalAddr.sin_zero));		// 清空结构体余下的内容
	
	if ( m_hSocket==INVALID_SOCKET )
	{
		if ( JLSOCK_SUCCESS != InitHandle(iType) )
		{
			return JLSOCK_ERR_CONNECT;
		}
	}

	int iBindResult = bind(m_hSocket, (sockaddr*)&LocalAddr, sizeof(struct sockaddr));
	if ( iBindResult == SOCKET_ERROR )
	{
//		TRACE0("Error when bind (Bind).");
		Close();
		return JLSOCK_ERR_BIND;
	}

	// 记录下梆定的端口
	m_uPort = uPort;
	return JLSOCK_SUCCESS;
}
