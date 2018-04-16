// JNVProtocolTCP.h: interface for the CJNVProtocolTCP class.
//
//////////////////////////////////////////////////////////////////////

#if !defined(AFX_JNVPROTOCOLTCP_H__94661FDA_0EE7_4575_8998_30D74A15F37D__INCLUDED_)
#define AFX_JNVPROTOCOLTCP_H__94661FDA_0EE7_4575_8998_30D74A15F37D__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

#include "JLTcpClient.h"
#include "ProtocolNV.h"

#undef DbgStrOut
#define DbgStrOut	JTRACE

#define JCONN_JNV_RCV_BUF		(1024*128)		// 128K
#define JCONN_JNV_SND_BUF		(1024*128)		// 128K

class CJNVProtocolTCP;	typedef CJNVProtocolTCP*		LPJNVProtocolTCP;

class CJNVProtocolTCP : public CJLTcpClient  
{
public:
	CJNVProtocolTCP();
	virtual ~CJNVProtocolTCP();

public:
	// 登录相关 begin
	long Login(LPCTSTR lpszIP,int iDevPort,LPCTSTR lpszUser,LPCTSTR lpszPassword,int iTimeOut,fcbJEvent* pfcbEvent,void* pUserParam);
	long Logout();
	// 登录相关 end

protected:
	// 通知函数 begin
	// 用于连接到指定的CMS上
	virtual int Connect();
	// 检查本对象的状态,返回0表示 OnCheckSocket 继续,否则退出检查线程
	virtual int OnCheckState(DWORD dwTick);
	// 客户端消息到达
	virtual int OnClientMsg(void* pParam);
	// 登录状态
	virtual int OnConnect(int iRetType);
	// 用于扩展的接收处理
	virtual int OnExtRecv(int nType,void* lpBuf,int iDataLen);
	// 通知函数 end

protected:
	// 发送回调消息
	long SendEvent(eJNetEvent eType,int iDataType,void* pEventData,int iDataLen);
	// 复制连接信息
	long CopyConn(LPJNVProtocolTCP lpProtocol);

	// 用于操作的虚函数 begin
	virtual long OnSendLogin();
	virtual long OnSendLogout();
	virtual long OnSendBreath(DWORD dwTick);
	// 用于操作的虚函数 end

	// 消息派分结果 begin
	virtual void OnRetLogin(void* pData,int iLen);	// 登录应答
	virtual void OnRetBreath(void* pData,int iLen);	// 呼吸应答
	// 消息派分结果 end

public:
protected:
	fcbJEvent*		m_pfcbEvent;					// 事件通知函数
	void*			m_pUserParam;					// 用户自定义参数

	DWORD			m_dwLastSendBreath;				// 上一呼吸时间
	DWORD			m_dwLastRecv;					// 上一接收时间
	int				m_iBreathSpan;					// 呼吸间隔

	bool			m_bAutoLogin;					// 是否自动登录
	int				m_iDataOffset;					// 接收偏移(为了组帧方便,少一次的拷贝过程)
	char			m_szDevIP[JNET_IP_LEN];			// 设备IP
	int				m_iDevPort;						// 设备端口
	int				m_iStreamPort;					// 流端口
	char			m_szUser[JNET_NAME_LEN*2];		// 用户名
	char			m_szPwd[JNET_PWD_LEN*2];			// 密码
	
	PJHANDLE		m_hEventSync;					// 同步事件
	
	stJNVProHead	m_stLastRecv;					// 上次接收
	DWORD			m_dwSendSeq;					// 用于同步接收
	void*			m_lpSendData;					// 用于同步接收
	int				m_iSendDataLen;					// 用于同步接收
	
	stJNVC2SLoginRet	m_stLoginInfo;				// 登录信息

public:
	int				m_nServerType;					// 服务器类型
};
#endif // !defined(AFX_JNVPROTOCOLTCP_H__94661FDA_0EE7_4575_8998_30D74A15F37D__INCLUDED_)
