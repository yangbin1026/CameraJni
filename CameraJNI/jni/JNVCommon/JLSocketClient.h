// JLSocketClient.h: interface for the CJLSocketClient class.
//
//////////////////////////////////////////////////////////////////////

#if !defined(AFX_JLSOCKETCLIENT_H__0BDA3ECC_FA8A_4AC8_9775_18B32320F9CD__INCLUDED_)
#define AFX_JLSOCKETCLIENT_H__0BDA3ECC_FA8A_4AC8_9775_18B32320F9CD__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

#include "JLSocket.h"

namespace JSocketCtrl
{
class CJLSocketClient : public CJLSocket  
{
public:
	CJLSocketClient(SOCKET hSocket=INVALID_SOCKET);
	virtual ~CJLSocketClient();

	// 客户端函数
	virtual	int	Connect(const char* szIP,UINT uPort,int iType=SOCK_STREAM);
	// 梆定端口,如果pAdd为NULL,则梆定本机
	virtual int Bind(UINT uPort,const char* pAdd = NULL,int iType=SOCK_DGRAM);
protected:

// 属性
public:
};
}
using namespace JSocketCtrl;

#endif // !defined(AFX_JLSOCKETCLIENT_H__0BDA3ECC_FA8A_4AC8_9775_18B32320F9CD__INCLUDED_)
