// JLUdpServer.h: interface for the CJLUdpServer class.
//
//////////////////////////////////////////////////////////////////////

#if !defined(AFX_JLUDPSERVER_H__D5029BB3_CEF2_4F6E_89C4_C67E9A6FF41E__INCLUDED_)
#define AFX_JLUDPSERVER_H__D5029BB3_CEF2_4F6E_89C4_C67E9A6FF41E__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

#include "JLSocketServer.h"
#include "JLThreadCtrl.h"


namespace JSocketCtrl
{
class CJLUdpServer : public  CJLSocketServer  
{
public:
	CJLUdpServer();
	virtual ~CJLUdpServer();

	// ��������
	virtual int	Start(UINT uPort,const char* pAdd);

	// ֹͣ����
	virtual int Stop();

	// ��ȡ����״̬
	virtual int	GetState();

	// �ͻ�����Ϣ����
	virtual int OnClientMsg(void* pParam);

protected:
	// ��ʼ����
	virtual	int	StartServer(UINT uPort,const char* pAdd,UINT uBackLog=SOMAXCONN);

	// RECV �߳�
	static fJThRet RunRecvThread(void* pParam);

public:
protected:
	CJLThreadCtrl						m_tcRecv;			// RECV �߳�

};
}
using namespace JSocketCtrl;

#endif // !defined(AFX_JLUDPSERVER_H__D5029BB3_CEF2_4F6E_89C4_C67E9A6FF41E__INCLUDED_)
