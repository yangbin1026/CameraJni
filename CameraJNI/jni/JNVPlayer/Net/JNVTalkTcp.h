// JNVTalkTcp.h: interface for the CJNVTalkTcp class.
//
//////////////////////////////////////////////////////////////////////

#if !defined(AFX_JNVTALKTCP_H__9FB98A92_0867_46A4_895D_3F843286B750__INCLUDED_)
#define AFX_JNVTALKTCP_H__9FB98A92_0867_46A4_895D_3F843286B750__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

#include "JNVProtocolTCP.h"

class CJNVTalkTcp;	typedef CJNVTalkTcp*		LPJNVTalkTcp;

class CJNVTalkTcp : public CJNVProtocolTCP  
{
public:
	CJNVTalkTcp();
	virtual ~CJNVTalkTcp();

public:
	// 对讲相关 begin
	// 开始对讲请求,协议设备指针(信令),设备ID,通道编号(1开始)
	long TalkStart(LPJNVProtocolTCP lpProtocol,LPCTSTR lpszDevID,int iChn);
	long TalkSendData(BYTE* lpData,int iLen);
	long TalkStop();
	// 对讲相关 end

protected:
	// 通知函数 begin
	// 用于扩展的接收处理
	virtual int OnExtRecv(int nType,void* lpBuf,int iDataLen);
	// 登录状态
	virtual int OnConnect(int iRetType);
	// 通知函数 end
protected:
	// 消息派分结果 begin
	void OnRetTalkData(void* pData,int iLen);		// 上传对讲流
	void OnRetTalkErr(void* pData,int iLen);		// 对讲异常
	void OnRetTalkAnswer(void* pData,int iLen);		// 对讲请求回应
	// 消息派分结果 end
	
public:
protected:
	LPJNVProtocolTCP	m_lpProtocol;				// 协议设备
	char				m_szDevID[JNET_DEV_ID_LEN];	// 设备ID
	int					m_iChn;						// 所属的通道号

	stJNVTalkReqSingle	m_stTalkReq;				// 对讲流请求参数
	bool				m_bCanSend;					// 是否可以发送了

	FILE*				m_pfWrite;					// 写入文件
};

#endif // !defined(AFX_JNVTALKTCP_H__9FB98A92_0867_46A4_895D_3F843286B750__INCLUDED_)
