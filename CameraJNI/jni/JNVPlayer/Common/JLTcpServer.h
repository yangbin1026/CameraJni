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

	// 启动服务
	virtual int	Start(UINT uPort,const char* pAdd);

	// 停止服务
	virtual int Stop();

	// 获取服务状态
	virtual int	GetState();

	// 客户端接入,返回构造的客户端对象
	virtual void* OnClientEnter(SOCKET hClient);
	// 客户端离开
	virtual int OnClientExit(void* pParam);
	// 客户端消息到达
	virtual int OnClientMsg(void* pParam);

protected:
	virtual CJLTcpClient*	GetNewCli(SOCKET hClient);	// 创建一个客户端连接

	// ACCEPT 线程
	static fJThRet RunAcceptThread(void* pParam);
	// Client Recv Notify
	static int __stdcall OnRecvClient(void* pParam, void* pUserData);
	static int __stdcall OnCheckClient(void* pParam, DWORD dwTick, void* pUserData);

public:
protected:
	CJLThreadCtrl	m_tcAccept;				// ACCEPT 线程
	mapCliSocket	m_mapCliSocket;			// 客户端列表
	int				m_iBreath;				// 最大呼吸间隔心跳(秒)

};
}
using namespace JSocketCtrl;

#endif // !defined(AFX_JLTCPSERVER_H__3DEC8F0A_8702_4EED_9F5C_AB16DB81E623__INCLUDED_)
