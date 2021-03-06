// JLTcpClient.cpp: implementation of the CJLTcpClient class.
//
//////////////////////////////////////////////////////////////////////

#include "JLTcpClient.h"

//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////

CJLTcpClient::CJLTcpClient(SOCKET hSocket)
	: CJLSocketClient(hSocket)
{
	m_iSockType		= SOCK_STREAM;
	m_iSleepMS		= 1;		// 休眠时长
}

CJLTcpClient::~CJLTcpClient()
{
	Close();
}

// 重置函数
int CJLTcpClient::ResetSock()
{
	// 仅关闭SOCKET
	LockThread("CJLTcpClient::ResetSock");
	CJLSocketClient::Close();
	UnLockThread("CJLTcpClient::ResetSock");
	return JLSOCK_SUCCESS;
}

// 关闭函数
int CJLTcpClient::Close()
{
	LockThread("CJLTcpClient::Close");
	CJLSocketClient::Close();
	UnLockThread("CJLTcpClient::Close");
	m_tcRecv.StopThread(true);

	return JLSOCK_SUCCESS;
}

// 客户端函数
int	CJLTcpClient::Connect(const char* szIP,UINT uPort,int iType)
{
	int	iRet	= 0;
//	if ( m_hSocket && m_hSocket!=INVALID_SOCKET )
//	{
//		return JLSOCK_SUCCESS;
//	}

	iRet = CJLSocketClient::Connect(szIP,uPort,iType);

	// 启动 RECV 线程
	return StartRecv();
}

// 服务端函数
int CJLTcpClient::StartRecv()
{
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

// 服务端函数
int CJLTcpClient::StopRecv(bool bWait)
{
	ResetSock();
	return m_tcRecv.StopThread(bWait);
}

void CJLTcpClient::LockThread(LPCTSTR lpszNotice)
{
//	if ( lpszNotice ) JTRACE("CJLTcpClient::LockThread :%s\r\n",lpszNotice);
//	else JTRACE("CJLTcpClient::LockThread \r\n");
	m_mutexThread.Lock();
}

void CJLTcpClient::UnLockThread(LPCTSTR lpszNotice)
{
//	if ( lpszNotice ) JTRACE("CJLTcpClient::UnLockThread :%s\r\n",lpszNotice);
//	else JTRACE("CJLTcpClient::UnLockThread \r\n");
	m_mutexThread.Unlock();
}

// RECV 线程
fJThRet CJLTcpClient::RunRecvThread(void* pParam)
{	
	int					iIsRun				= 0;				// 是否需要运行
	CJLThreadCtrl*		pThreadCtrl			= NULL;				// 对应用线程控制器
	CJLTcpClient*		pClient				= NULL;				// 客户端指针

	// 初始化参数 begin
	pThreadCtrl	= (CJLThreadCtrl*)pParam;
	if ( pThreadCtrl==NULL )
	{
		return 0;
	}
	pClient		= (CJLTcpClient*)pThreadCtrl->GetOwner();
	if ( pClient==NULL )
	{
		pThreadCtrl->SetThreadState(THREAD_STATE_STOP);		// 运行状态
		return 0;
	}
	// 初始化参数 end

	// 进行 Recv 操作 begin
	iIsRun	= 1;											// 需要运行
	pClient->m_mutexThread.CreateMutex();
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
			pClient->LockThread("CJLTcpClient::RunRecvThread");
			if ( pClient->OnCheckSocket()<0 )
			{
				pThreadCtrl->SetThreadState(THREAD_STATE_STOP);
				pClient->Close();
				// 客户端离开了
				iIsRun = 0;								// 不再运行
				pClient->UnLockThread("CJLTcpClient::RunRecvThread");
				break;
			}
			pClient->UnLockThread("CJLTcpClient::RunRecvThread");
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

	pClient->m_mutexThread.CloseMutex();
	return 0;
}

