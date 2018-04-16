// JNVStreamTcp.h: interface for the CJNVStreamTcp class.
//
//////////////////////////////////////////////////////////////////////

#if !defined(AFX_JNVSTREAMTCP_H__50A23179_5478_4F0C_97C0_142E54F3891A__INCLUDED_)
#define AFX_JNVSTREAMTCP_H__50A23179_5478_4F0C_97C0_142E54F3891A__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

#include "JNVProtocolTCP.h"
#include "FMReader.h"

class CJNVStreamTcp;	typedef CJNVStreamTcp*		LPJNVStreamTcp;

class CJNVStreamTcp : public CJNVProtocolTCP  
{
public:
	CJNVStreamTcp();
	virtual ~CJNVStreamTcp();

public:
	// 实时视频相关 begin
	// 开始实时视频流,协议设备指针(信令),设备ID,通道编号(1开始),码流编号(保留),码流类型(保留)
	long RealStreamStart(LPJNVProtocolTCP lpProtocol,LPCTSTR lpszDevID,int iChn,int iStream,int iType,fcbJStream* pfcbStream,void* pUserParam);
	long RealStreamReqIFrame();
	long RealStreamStop();
	void* GetStreamParam();
	// 实时视频相关 end
	
	// 设备录像回放相关 begin
	long ReplayStart(LPJNVProtocolTCP lpProtocol,LPCTSTR lpszDevID,int iCenter,int iChn,LPCTSTR lpszTimeStart,LPCTSTR lpszTimeEnd,int iOnlyIFrame,int iRecType,int iStartPos,
		fcbJStream* pfcbStream,void* pUserParam);
	long ReplayCtrl(int iCtrlType,int iValue);
	long ReplayStop();
	// 设备录像回放相关 end
protected:
	// 通知函数 begin
	// 用于扩展的接收处理
	virtual int OnExtRecv(int nType,void* lpBuf,int iDataLen);
	// 登录状态
	virtual int OnConnect(int iRetType);
	// 通知函数 end
protected:
	// 消息派分结果 begin
	void OnRetStreamData(void* pData,int iLen);		// 下发视频流
	void OnRetStreamErr(void* pData,int iLen);		// 请求视频设备返回的错误码
	void OnRetReplayAnswer(void* pData,int iLen);	// 备录像按时间回放应答
	// 消息派分结果 end
	
public:
protected:
	int					m_iStreamType;				// 流类型,见 eJNVStreamType
	LPJNVProtocolTCP	m_lpProtocol;				// 协议设备
	char				m_szDevID[JNET_DEV_ID_LEN];	// 设备ID
	int					m_iChn;						// 所属的通道号

	stJNVRealStreamStart	m_stRealStart;			// 实时流参数
	stJNVReplayStart	m_stReplayStart;			// 录像回放参数

	fcbJStream*			m_pfcbStream;				// 流回调函数
	void*				m_pStreamParam;				// 用户自定义参数

	stJNVStreamHeaderFrameInfo	m_stDataHeader;			// 数据头
	FILE*				m_pfWrite;					// 写入文件

	static long			s_lFrameNo;					// 帧号
};

#endif // !defined(AFX_JNVSTREAMTCP_H__50A23179_5478_4F0C_97C0_142E54F3891A__INCLUDED_)
