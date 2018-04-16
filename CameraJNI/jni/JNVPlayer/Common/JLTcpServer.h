// JLTcpServer.h: interface for the CJLTcpServer class.
//
//////////////////////////////////////////////////////////////////////

#if !defined(AFX_JLTCPSERVER_H__3DEC8F0A_8702_4EED_9F5C_AB16DB81E623__INCLUDED_)
#define AFX_JLTCPSERVER_H__3DEC8F0A_8702_4EED_9F5C_AB16DB81E623__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

#include "JLSocketServer.h"
#include "JLTcpClient.h"

namespace JSocketCtrl
{
class CJLTcpServer : public CJLSocketServer  
{
public:
	typedef map<SOCKET,CJLTcpClient*>			mapCliSocket;
	typedef mapCliSocket::iterator				itorCliSocket;
	typedef mapCliSocket::value_type			valueCliSocket;
public:
	CJLTcpServer();
	virtual ~CJLTcpServer();

	// ��������
	virtual int	Start(UINT uPort,const char* pAdd);

	// ֹͣ����
	virtual int Stop();

	// ��ȡ����״̬
	virtual int	GetState();

	// �ͻ��˽���,���ع���Ŀͻ��˶���
	virtual void* OnClientEnter(SOCKET hClient);
	// �ͻ����뿪
	virtual int OnClientExit(void* pParam);
	// �ͻ�����Ϣ����
	virtual int OnClientMsg(void* pParam);

protected:
	virtual CJLTcpClient*	GetNewCli(SOCKET hClient);	// ����һ���ͻ�������

	// ACCEPT �߳�
	static fJThRet RunAcceptThread(void* pParam);
	// Client Recv Notify
	static int __stdcall OnRecvClient(void* pParam, void* pUserData);
	static int __stdcall OnCheckClient(void* pParam, DWORD dwTick, void* pUserData);

public:
protected:
	CJLThreadCtrl	m_tcAccept;				// ACCEPT �߳�
	mapCliSocket	m_mapCliSocket;			// �ͻ����б�
	int				m_iBreath;				// �������������(��)

};
}
using namespace JSocketCtrl;

#endif // !defined(AFX_JLTCPSERVER_H__3DEC8F0A_8702_4EED_9F5C_AB16DB81E623__INCLUDED_)
