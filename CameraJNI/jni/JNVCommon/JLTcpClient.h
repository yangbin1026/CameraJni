// JLTcpClient.h: interface for the CJLTcpClient class.
//
//////////////////////////////////////////////////////////////////////

#if !defined(AFX_JLTCPCLIENT_H__EC4BC74D_4B80_46C3_ADF5_09244007097D__INCLUDED_)
#define AFX_JLTCPCLIENT_H__EC4BC74D_4B80_46C3_ADF5_09244007097D__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

#include "JLSocketClient.h"
#include "JLThreadCtrl.h"

namespace JSocketCtrl
{
class CJLTcpClient : public CJLSocketClient  
{
public:
	CJLTcpClient(SOCKET hSocket=INVALID_SOCKET);
	virtual ~CJLTcpClient();

	// 重置函数
	virtual int ResetSock();
	// 关闭函数
	virtual int Close();
	// 客户端函数
	virtual	int	Connect(const char* szIP,UINT uPort,int iType=SOCK_STREAM);
	// 服务端函数
	virtual int StartRecv();
	// 服务端函数
	virtual int StopRecv(bool bWait);
protected:
	void LockThread(LPCTSTR lpszNotice);
	void UnLockThread(LPCTSTR lpszNotice);
protected:
	// RECV 线程
	static fJThRet RunRecvThread(void* pParam);
	CJLThreadCtrl	m_tcRecv;		// RECV 线程表
	int				m_iSleepMS;		// 休眠时长
	CMutexLock		m_mutexThread;
};
}
using namespace JSocketCtrl;

#endif // !defined(AFX_JLSOCKETCLI_H__EC4BC74D_4B80_46C3_ADF5_09244007097D__INCLUDED_)
