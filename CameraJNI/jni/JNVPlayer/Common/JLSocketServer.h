// JLSocketServer.h: interface for the CJLSocketServer class.
//
//////////////////////////////////////////////////////////////////////

#if !defined(AFX_JLSOCKETSERVER_H__F5978252_7F56_467D_B215_24262562B7F0__INCLUDED_)
#define AFX_JLSOCKETSERVER_H__F5978252_7F56_467D_B215_24262562B7F0__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

#include "JLSocket.h"

// stl support begin
#if (defined _WIN32) || (defined _WIN64)
	#pragma warning(disable:4100)
	#pragma warning(disable:4189)
	#pragma warning(disable:4541)
	#pragma warning(disable:4786)
#endif
#include "string"
#include "list"
#include "map"
#include <map>
#include <vector>
using namespace std;
// stl support end

namespace JSocketCtrl
{
class CJLSocketServer : public CJLSocket  
{
public:
	CJLSocketServer();
	virtual ~CJLSocketServer();

	// ���عرպ���
	virtual	int Close();

	// ��ʼ����
	virtual	int	StartServer(UINT uPort,const char* pAdd,UINT uBackLog=SOMAXCONN,int iType=SOCK_STREAM,bool bBlock=false);
	// ��ȡ����Ŀͻ��˵�SOCKET���
	virtual	SOCKET	Accept();
	// �ͻ����뿪
	virtual	int	ClientExit(SOCKET hClient);
protected:
	// �𶨶˿�,���pAddΪNULL,��𶨱���
	int 	Bind(UINT uPort,const char *pAdd = NULL);
	// �������𶨵Ķ˿�
	int		Listen(UINT uBackLog=SOMAXCONN);

// ����
public:
protected:
	map<SOCKET,long>	m_mapClient;	// ����ͻ���
	UINT	m_uBackLog;					// ������ͻ��˸���
};
}
using namespace JSocketCtrl;

#endif // !defined(AFX_JLSOCKETSERVER_H__F5978252_7F56_467D_B215_24262562B7F0__INCLUDED_)
