// JNVTalkTcp.cpp: implementation of the CJNVTalkTcp class.
//
//////////////////////////////////////////////////////////////////////

#include "JNVTalkTcp.h"
#undef	LOG_TAG
#define	LOG_TAG		"JNVTalkTcp.cpp"

//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////

CJNVTalkTcp::CJNVTalkTcp()
{
	m_bAutoLogin		= false;				// 是否自动登录
	m_iDataOffset		= 0;					// 接收偏移(为了组帧方便,少一次的拷贝过程)
	m_bCanSend			= false;				// 是否可以发送了

	m_lpProtocol		= NULL;					// 协议设备
	memset(m_szDevID,0,JNET_DEV_ID_LEN);		// 设备ID
	m_iChn				= 0;					// 所属的通道号
	
	memset(&m_stTalkReq,0,sizeof(stJNVTalkReqSingle));		// 对讲流请求参数
	m_pfWrite			= NULL;					// 写入文件
}

CJNVTalkTcp::~CJNVTalkTcp()
{
	Close();
	if ( m_pfWrite )
	{
		fclose(m_pfWrite);
		m_pfWrite = NULL;
	}
}

// 通知函数 begin
// 用于扩展的接收处理
int CJNVTalkTcp::OnExtRecv(int nType,void* lpBuf,int iDataLen)
{
//	JTRACE("CJNVTalkTcp::OnExtRecv %d,%d\r\n",nType,iDataLen);
	// 消息派分
	switch(nType)
	{
	case eJVNProTalkRet:			OnRetTalkAnswer(lpBuf,iDataLen);		break;		// 对讲请求回应
	case eJVNProTalkTransData:		OnRetTalkData(lpBuf,iDataLen);			break;		// 上传对讲流 
	case eJVNProTalkErr:			OnRetTalkErr(lpBuf,iDataLen);			break;		// 对讲异常
	default:
		return 0;
	}
	return 1;
}
// 登录状态
int CJNVTalkTcp::OnConnect(int iRetType)
{
	int	iRet	= 0;

	iRet = CJNVProtocolTCP::OnConnect(iRetType);
	
	if ( iRetType==JLSOCK_SUCCESS )
	{
		SendEvent(JNET_EET_TALK_OPENING,0,NULL,0);

		SetNetHdr(&(m_stTalkReq.stHead), sizeof(m_stTalkReq), eJVNProTalkReq, stJNVProHead::GetSeq());
		Send(&m_stTalkReq,sizeof(m_stTalkReq));
	}
	else
	{
		// 流打开失败,并回调事件
		SendEvent(JNET_EET_TALK_ERROR,0,NULL,0);
	}

	return iRet;
}
// 通知函数 end

// 实时视频相关 begin
// 开始对讲请求,协议设备指针(信令),设备ID,通道编号(1开始)
long CJNVTalkTcp::TalkStart(LPJNVProtocolTCP lpProtocol,LPCTSTR lpszDevID,int iChn)
{
	m_lpProtocol	= lpProtocol;					// 协议设备
	StrNCpy(m_szDevID,lpszDevID,JNET_DEV_ID_LEN);	// 设备ID
	m_iChn			= iChn;							// 所属的通道号

	if ( m_lpProtocol==NULL ) return JNETErrParam;
	// 复制登录信息
	CopyConn(m_lpProtocol);

	// 构造请求结构体 begin
	SetNetHdr(&(m_stTalkReq.stHead), sizeof(m_stTalkReq), eJVNProTalkReq, stJNVProHead::GetSeq());
	m_stTalkReq.nWorkMode			= 0;				// 工作方式(0:对讲,1:广播,2:PTT,3:监听)
	m_stTalkReq.usControl			= 1;				// 控制方式(0:取消,1:请求)
	m_stTalkReq.usChannel			= iChn;				// 通道
	m_stTalkReq.nDeviceNumber		= 1;				// 设备数量
	m_stTalkReq.nSessionID			= m_stLoginInfo.nSessionID;	// SessionID
	StrNCpy(m_stTalkReq.szDeviceID,lpszDevID,JNET_DEV_ID_LEN);	// 设备ID
	// 构造请求结构体 end

//	m_pfWrite	= fopen("d:\\TalkData.g726","wb");

	// 先只登录
	return Connect();
}
long CJNVTalkTcp::TalkSendData(BYTE* lpData,int iLen)
{
	char			szTemp[1024]	= {0};
	PSTJNVProHead	pstNetHdr		= (PSTJNVProHead)szTemp;

	if ( m_bCanSend==false )
	{
		// 是否可以发送了
		return JNETErrSuccess;
	}
	if ( m_pfWrite )
	{
		int		iLeft	= iLen;
		LPBYTE	lpWrite	= lpData;
		while(iLeft>=104)
		{
			fwrite(lpWrite+4,100,1,m_pfWrite);
			lpWrite+=104;
			iLeft-=104;
		}
	}

//	SetNetHdr(pstNetHdr, pstNetHdr->s_iSize+iLen, eJVNProTalkTransData, stJNVProHead::GetSeq());
	SetNetHdr(pstNetHdr, pstNetHdr->s_iSize+iLen, eJVNProTalkTransData, 0);
	memcpy(pstNetHdr+1,lpData,iLen);
	Send(szTemp,pstNetHdr->s_iSize+iLen);
	JTRACE("CJNVTalkTcp::TalkSendData:%d\r\n",pstNetHdr->s_iSize+iLen);

	return JNETErrSuccess;
}
long CJNVTalkTcp::TalkStop()
{
	// 构造请求结构体 begin
	SetNetHdr(&(m_stTalkReq.stHead), sizeof(m_stTalkReq), eJVNProTalkReq, stJNVProHead::GetSeq());
	m_stTalkReq.nWorkMode			= 0;				// 工作方式(0:对讲,1:广播,2:PTT,3:监听)
	m_stTalkReq.usControl			= 0;				// 控制方式(0:取消,1:请求)
	m_stTalkReq.usChannel			= m_iChn;			// 通道
	m_stTalkReq.nDeviceNumber		= 1;				// 设备数量
	m_stTalkReq.nSessionID			= m_stLoginInfo.nSessionID;	// SessionID
	StrNCpy(m_stTalkReq.szDeviceID,m_szDevID,JNET_DEV_ID_LEN);	// 设备ID
	// 构造请求结构体 end

	Send(&m_stTalkReq,sizeof(m_stTalkReq));
	Close();
	SendEvent(JNET_EET_TALK_CLOSE,0,NULL,0);
	if ( m_pfWrite )
	{
		fclose(m_pfWrite);
		m_pfWrite = NULL;
	}
	m_bCanSend			= false;				// 是否可以发送了
	return JNETErrSuccess;
}
// 实时视频相关 end

// 消息派分结果 begin
// 上传对讲流
void CJNVTalkTcp::OnRetTalkData(void* pData,int iLen)
{
//	JTRACE("CJNVTalkTcp OnRetTalkData:len=%d\r\n",iLen);
}
// 对讲异常
void CJNVTalkTcp::OnRetTalkErr(void* pData,int iLen)
{
	m_bCanSend			= false;				// 是否可以发送了
	JTRACE("CJNVTalkTcp::OnRetTalkErr %d,%d,%d\r\n",iLen,m_stLastRecv.nParameter1,m_stLastRecv.nParameter2);
}
// 对讲请求回应
void CJNVTalkTcp::OnRetTalkAnswer(void* pData,int iLen)
{
	m_bCanSend			= true;				// 是否可以发送了
	JTRACE("CJNVTalkTcp::OnRetTalkAnswer %d,%d,%d\r\n",iLen,m_stLastRecv.nParameter1,m_stLastRecv.nParameter2);
}
// 消息派分结果 end



