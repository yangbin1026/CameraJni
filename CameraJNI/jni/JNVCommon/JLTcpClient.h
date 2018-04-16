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

	// ���ú���
	virtual int ResetSock();
	// �رպ���
	virtual int Close();
	// �ͻ��˺���
	virtual	int	Connect(const char* szIP,UINT uPort,int iType=SOCK_STREAM);
	// ����˺���
	virtual int StartRecv();
	// ����˺���
	virtual int StopRecv(bool bWait);
protected:
	void LockThread(LPCTSTR lpszNotice);
	void UnLockThread(LPCTSTR lpszNotice);
protected:
	// RECV �߳�
	static fJThRet RunRecvThread(void* pParam);
	CJLThreadCtrl	m_tcRecv;		// RECV �̱߳�
	int				m_iSleepMS;		// ����ʱ��
	CMutexLock		m_mutexThread;
};
}
using namespace JSocketCtrl;

#endif // !defined(AFX_JLSOCKETCLI_H__EC4BC74D_4B80_46C3_ADF5_09244007097D__INCLUDED_)
