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

	// 重载关闭函数
	virtual	int Close();

	// 开始服务
	virtual	int	StartServer(UINT uPort,const char* pAdd,UINT uBackLog=SOMAXCONN,int iType=SOCK_STREAM,bool bBlock=false);
	// 获取接入的客户端的SOCKET句柄
	virtual	SOCKET	Accept();
	// 客户端离开
	virtual	int	ClientExit(SOCKET hClient);
protected:
	// 梆定端口,如果pAdd为NULL,则梆定本机
	int 	Bind(UINT uPort,const char *pAdd = NULL);
	// 监听所梆定的端口
	int		Listen(UINT uBackLog=SOMAXCONN);

// 属性
public:
protected:
	map<SOCKET,long>	m_mapClient;	// 接入客户表
	UINT	m_uBackLog;					// 最大接入客户端个数
};
}
using namespace JSocketCtrl;

#endif // !defined(AFX_JLSOCKETSERVER_H__F5978252_7F56_467D_B215_24262562B7F0__INCLUDED_)
