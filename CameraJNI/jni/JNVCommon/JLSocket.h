// JLSocket.h: interface for the CJLSocket class.
//
//////////////////////////////////////////////////////////////////////

#if !defined(AFX_JLSOCKET_H__E3030ACE_A643_4A68_A320_FBB1FA03143A__INCLUDED_)
#define AFX_JLSOCKET_H__E3030ACE_A643_4A68_A320_FBB1FA03143A__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

#include "j_vs_ver.h"
#include "JLogWriter.h"
#include "JLSocketDef.h"

#define		JLSOCK_SUCCESS			0						// 操作成功
#define		JLSOCK_ERR_PARAM		0x000A0001				// 参数范围错误
#define		JLSOCK_ERR_PASTE		0x000A0002				// 复制句柄失败
#define		JLSOCK_ERR_INIT			0x000A0003				// 初始化失败
#define		JLSOCK_ERR_CLOSE		0x000A0004				// 关闭失败
#define		JLSOCK_ERR_CONNECT		0x000A0005				// 连接失败
#define		JLSOCK_ERR_BIND			0x000A0006				// 梆定端口失败
#define		JLSOCK_ERR_LISTEN		0x000A0007				// 启用监听失败
#define		JLSOCK_ERR_ACCEPT		0x000A0008				// ACCEPT失败
#define		JLSOCK_ERR_CLI_EXIT		0x000A0009				// 处理客户端离开时失败
#define		JLSOCK_ERR_SEND			0x000A0010				// 发送消息失败
#define		JLSOCK_ERR_RECV			0x000A0011				// 接收消息失败
#define		JLSOCK_ERR_PEER			0x000A0012				// 获取远端地址错误
#define		JLSOCK_ERR_NOBLOCK		0x000A0013				// 设置为非阻塞方式时错误
#define		JLSOCK_ERR_MEM			0x000A0014				// 内存不足错误
#define		JLSOCK_ERR_TIMEOUT		0x000A0015				// 超时错误
#define		JLSOCK_ERR_OPERATE		0x000A0016				// 操作错误
#define		JLSOCK_ERR_ACCEPT_TH	0x000A1001				// 开启 ACCEPT 线程失败
#define		JLSOCK_ERR_RECV_TH		0x000A1002				// 开启 RECV 线程失败

#define		JSS_CAN_READ			0x0001					// SOCK可读
#define		JSS_CAN_WRITE			0x0002					// SOCK可写

#define		JSS_SO_T_CONN			0						// 连接操作
#define		JSS_SO_T_SEND			1						// 发送操作
#define		JSS_SO_T_RECV			2						// 接收操作


namespace JSocketCtrl
{
class CJLSocket  
{
public:
	CJLSocket(SOCKET hSocket=INVALID_SOCKET);
	virtual ~CJLSocket();

	virtual int Close();

	// 常用的fcntl begin
	// 设置为非阻塞方式
	int SetNoBlock(bool bNoBlock);
	bool IsNoBlock();
	int SetSoBuf(int iType,int iSize);				// iType:见 JSS_SO_T_XXX
	int SetTimeOut(int iType,int iMilliseconds);	// iType:见 JSS_SO_T_XXX
	// 常用的fcntl end

	// 帮助函数 begin
	// 获取相应的句柄
	SOCKET GetHandle();
	// 获取远端地址(对服务器方式不可用)
	int	GetPeerAddr(sockaddr_in* addr,char* szStr=NULL);
	// 获取本地地址
	int	GetLocalAddr(sockaddr_in* addr,char* szStr=NULL);
	// 帮助函数 end

	// 常规操作 begin
	// 发送 byBuf 中的消息,发送长度为 iBufLen ,返回实际发送的长度(用于TCP)
	virtual int Send(const void* byBuf,int iBufLen);
	// 接收消息到 byBuf 中,接收长度为 iBufLen ,返回实际接收的长度(用于TCP)
	virtual int Recv(void* byBuf,int iRecvLen);
	virtual int RecvNoBlock(void* byBuf,int iRecvLen);
	// 发送 byBuf 中的消息,发送长度为 iBufLen ,返回实际发送的长度(用于UDP)
	virtual int SendTo(const void* byBuf,int iBufLen,const char* szIP,UINT uPort);
	// 接收消息到 byBuf 中,接收长度为 iBufLen ,返回实际接收的长度(用于UDP)
	virtual int RecvFrom(void* byBuf,int iRecvLen,sockaddr_in* addr);
	// 常规操作 end
	
	// 通知函数 begin
	// 检查SOCKET的状态,返回值见JSS_CAN_XXXX
	virtual int OnCheckSocket();
	// 检查本对象的状态,返回0表示 OnCheckSocket 继续,否则退出检查线程
	virtual int OnCheckState(DWORD dwTick);
	// 登录成功,返回的值作为OnCheckSocket的返回值
	virtual int OnConnect(int iRetType);
	// 客户端接入
	virtual void* OnClientEnter(SOCKET hClient);
	// 客户端离开
	virtual int OnClientExit(void* pParam);
	// 客户端消息到达
	virtual int OnClientMsg(void* pParam);
	// 通知函数 end

	// 静态函数 begin
	// 启动SOCKET资源
	static int InitSocket();
	// 关闭SOCKET资源
	static int CleanSocket();
	// 转换IP字符串及端口号
	static UINT GetIPAndPort(const sockaddr_in* addr,char* szIP);
	// 设置sockaddr_in
	static int SetIPAndPort(sockaddr_in* addr,char* szIP,UINT uPort);
	// 获取本机IP
	static hostent* GetLocalIP();
	// 获取空闲的端口
	static int GetValidSocketPort(bool bUdp);
	// 域名解释
	static DWORD DomainToIP(const char* szDomain);
	// 静态函数 begin
	
	void	SetCheckCallback(fcbRecvMsg* pcbRecvMsg,fcbCheckState* pcbCheckState,void* lpUserData);
	
	LPBYTE	GetSendBufSize(int iLen);
	LPBYTE	GetRecvBufSize(int iLen);

	// 检查时间
	inline bool IsOverTime(DWORD dwLast,DWORD dwCur,DWORD dwMax)
	{
		if ( (long)dwMax>0 && (long)dwLast>0 && (long)(dwCur-dwLast)>(long)(dwMax) ) return true;
		else return false;
	}
protected:
	// 初始化SOCKET句柄
	virtual int InitHandle(int iType=SOCK_STREAM);
	// 根据传入的句柄,构造对象
	virtual int PasteHandle(SOCKET hSocket);
// 属性
public:
	DWORD			m_dwLastRecv;			// 上次接收时间
protected:
	SOCKET			m_hSocket;				// SOCKET句柄
	UINT			m_uPort;				// 本地端口
	char			m_szAddr[128];			// 地址
	bool			m_bNoBlock;				// 是否为阻塞式
	bool			m_bConnecting;			// 正在连接中
	int				m_iSockType;			// Socket的类型

	LONG			m_dwConnTick;			// 登录的时间
	int				m_iConnTO;				// 连接超时(毫秒)
	int				m_iRecvTO;				// 接收超时(毫秒)
	int				m_iSendTO;				// 发送超时(毫秒)

	LPBYTE			m_lpSendBuf;			// 发送缓存
	int				m_iSendBufLen;			// 发送大小
	LPBYTE			m_lpRecvBuf;			// 接收缓存
	int				m_iRecvBufLen;			// 缓存大小

	fcbCheckState*	m_pcbCheckState;		// 检查回调函数
	fcbRecvMsg*		m_pcbRecvMsg;			// 接收回调函数
	void*			m_lpUserData;			// 用户自定义的数据域

	long			m_lHasRecv;				// 已经接收数
};
}
using namespace JSocketCtrl;

#endif // !defined(AFX_JLSOCKET_H__E3030ACE_A643_4A68_A320_FBB1FA03143A__INCLUDED_)
