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

	// 通知函数 begin
	// 客户端消息到达
	virtual int OnClientMsg(void* pParam);
	// 通知函数 end

protected:
	void LockThread(LPCTSTR lpszNotice);
	void UnLockThread(LPCTSTR lpszNotice);
protected:
	// RECV 线程
	static fJThRet RunRecvThread(void* pParam);
protected:	
	bool			m_bConnected;
	sockaddr_in		m_addrGroup;
	sockaddr_in		m_addrLocal;
	CJLThreadCtrl	m_tcRecv;		// RECV 线程表
	int				m_iSleepMS;		// 休眠时长
	CMutexLock		m_mutexThread;
};
}
using namespace JSocketCtrl;

#endif // !defined(AFX_JLMULITICASTSOCKET_H__05A5A2A2_0C4D_46E6_A2F2_F256C18EBD0E__INCLUDED_)
