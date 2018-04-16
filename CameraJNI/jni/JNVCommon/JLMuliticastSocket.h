// JMulticastSocket.h: interface for the CJLMuliticastSocket class.
//
//////////////////////////////////////////////////////////////////////

#if !defined(AFX_JLMULITICASTSOCKET_H__05A5A2A2_0C4D_46E6_A2F2_F256C18EBD0E__INCLUDED_)
#define AFX_JLMULITICASTSOCKET_H__05A5A2A2_0C4D_46E6_A2F2_F256C18EBD0E__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

#include "j_vs_ver.h"
#include "JLSocket.h"
#include "JLThreadCtrl.h"

namespace JSocketCtrl
{
class CJLMuliticastSocket : public CJLSocket  
{
public:
	CJLMuliticastSocket();
	virtual ~CJLMuliticastSocket();

	virtual int Close();

	bool	IsConnected();
	int		AddGroup(LPCTSTR lpszIP,int iPort,int iLocalPort=0);
	int		SendMuMsg(void* lpData,int iLen);

	// ֪ͨ���� begin
	// �ͻ�����Ϣ����
	virtual int OnClientMsg(void* pParam);
	// ֪ͨ���� end

protected:
	void LockThread(LPCTSTR lpszNotice);
	void UnLockThread(LPCTSTR lpszNotice);
protected:
	// RECV �߳�
	static fJThRet RunRecvThread(void* pParam);
protected:	
	bool			m_bConnected;
	sockaddr_in		m_addrGroup;
	sockaddr_in		m_addrLocal;
	CJLThreadCtrl	m_tcRecv;		// RECV �̱߳�
	int				m_iSleepMS;		// ����ʱ��
	CMutexLock		m_mutexThread;
};
}
using namespace JSocketCtrl;

#endif // !defined(AFX_JLMULITICASTSOCKET_H__05A5A2A2_0C4D_46E6_A2F2_F256C18EBD0E__INCLUDED_)
