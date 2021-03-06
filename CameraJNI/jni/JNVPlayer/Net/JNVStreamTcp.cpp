// JNVStreamTcp.cpp: implementation of the CJNVStreamTcp class.
//
//////////////////////////////////////////////////////////////////////

#include "JNVStreamTcp.h"
#undef	LOG_TAG
#define	LOG_TAG		"JNVStreamTcp.cpp"

long CJNVStreamTcp::s_lFrameNo			= 1;					// 帧号
//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////

CJNVStreamTcp::CJNVStreamTcp()
{
	m_iStreamType		= eJNVStreamUnknown;	// 流类型,见 eJNVStreamType
	m_bAutoLogin		= false;				// 是否自动登录
	m_iDataOffset		= sizeof(stJNVVideoFramInfo)-8;				// 接收偏移(为了组帧方便,少一次的拷贝过程)

	m_lpProtocol		= NULL;					// 协议设备
	memset(m_szDevID,0,JNET_DEV_ID_LEN);		// 设备ID
	m_iChn				= 0;					// 所属的通道号
	
	memset(&m_stRealStart,0,sizeof(stJNVRealStreamStart));		// 实时流参数
	memset(&m_stReplayStart,0,sizeof(stJNVReplayStart));		// 录像回放参数

	m_pfcbStream		= NULL;					// 流回调函数
	m_pStreamParam		= NULL;					// 用户自定义参数

	memset(&m_stDataHeader,sizeof(stJNVStreamHeaderFrameInfo),0);		// 数据头
	m_pfWrite			= NULL;					// 写入文件
}

CJNVStreamTcp::~CJNVStreamTcp()
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
int CJNVStreamTcp::OnExtRecv(int nType,void* lpBuf,int iDataLen)
{
	// 消息派分
	switch(nType)
	{
	case eJVNProRealStreamData:			OnRetStreamData(lpBuf,iDataLen);		break;		// 下发视频流
	case eJVNProRealStreamErrRet:		OnRetStreamErr(lpBuf,iDataLen);			break;		// 请求视频设备返回的错误码 
	case eJVNProReplayAnswerRet:		OnRetReplayAnswer(lpBuf,iDataLen);		break;		// 备录像按时间回放应答
	default:
		return 0;
	}
	return 1;
}
// 登录状态
int CJNVStreamTcp::OnConnect(int iRetType)
{
	int	iRet	= 0;

	iRet = CJNVProtocolTCP::OnConnect(iRetType);
	
	if ( iRetType==JLSOCK_SUCCESS )
	{
		SendEvent(JNET_EET_STREAM_OPENING,m_iStreamType,NULL,0);

		if ( m_iStreamType==eJNVStreamReal )
		{
			SetNetHdr(&(m_stRealStart.stHead), sizeof(m_stRealStart), eJVNProRealStreamOpenReq, stJNVProHead::GetSeq());
			Send(&m_stRealStart,sizeof(m_stRealStart));
		}
		else if ( m_iStreamType==eJNVStreamReplay )
		{			
			SetNetHdr(&(m_stReplayStart.stHead), sizeof(m_stReplayStart), eJVNProReplayOpenReq, stJNVProHead::GetSeq());
			Send(&m_stReplayStart,sizeof(m_stReplayStart));
		}
	}
	else
	{
		// 流打开失败,并回调事件
		SendEvent(JNET_EET_STREAM_ERROR,m_iStreamType,NULL,0);
	}

	return iRet;
}
// 通知函数 end

// 实时视频相关 begin
// 开始实时视频流,协议设备指针(信令),设备ID,通道编号(1开始),码流编号(保留),码流类型(保留)
long CJNVStreamTcp::RealStreamStart(LPJNVProtocolTCP lpProtocol,LPCTSTR lpszDevID,int iChn,int iStream,int iType,fcbJStream* pfcbStream,void* pUserParam)
{
	m_iStreamType	= eJNVStreamReal;				// 流类型,见 eJNVStreamType
	m_lpProtocol	= lpProtocol;					// 协议设备
	StrNCpy(m_szDevID,lpszDevID,JNET_DEV_ID_LEN);	// 设备ID
	m_iChn			= iChn;							// 所属的通道号
	m_pfcbStream	= pfcbStream;					// 流回调函数
	m_pStreamParam	= pUserParam;					// 用户自定义参数

	if ( m_lpProtocol==NULL ) return JNETErrParam;
	// 复制登录信息
	CopyConn(m_lpProtocol);

	// 构造请求结构体 begin
	StrNCpy(m_stRealStart.szDeviceID,m_szDevID,JNET_DEV_ID_LEN);	// 设备ID
	m_stRealStart.nChannel			= iChn;							// 通道编号(1开始)
	m_stRealStart.nStreamIndex		= iStream;						// 码流编号(保留)
	m_stRealStart.nSessionID		= m_stLoginInfo.nSessionID;		// SessionID
	m_stRealStart.nStreamType		= iType;						// 码流类型(保留)
	// 构造请求结构体 end

	// 先只登录
	return Connect();
}
long CJNVStreamTcp::RealStreamReqIFrame()
{
	stJNVRealStreamGetIFrame	stGetIFrame;

	SetNetHdr(&(stGetIFrame.stHead), sizeof(stGetIFrame.stHead), eJVNProGetIFrameReq, stJNVProHead::GetSeq());
	StrNCpy(stGetIFrame.szDeviceID,m_szDevID,JNET_DEV_ID_LEN);		// 设备ID
	stGetIFrame.nChannel					= m_iChn;				// 通道编号(1开始)
	Send(&stGetIFrame,sizeof(stGetIFrame));
	return JNETErrSuccess;
}
long CJNVStreamTcp::RealStreamStop()
{
	stJNVProHead	stHead;
	SetNetHdr(&stHead, sizeof(stHead), eJVNProRealStreamCloseReq, stJNVProHead::GetSeq());

	Send(&stHead,sizeof(stHead));
	Close();
	SendEvent(JNET_EET_STREAM_CLOSE,m_iStreamType,NULL,0);
	if ( m_pfWrite )
	{
		fclose(m_pfWrite);
		m_pfWrite = NULL;
	}
	return JNETErrSuccess;
}
void* CJNVStreamTcp::GetStreamParam()
{
	return m_pStreamParam;
}
// 实时视频相关 end

// 设备录像回放相关 begin
long CJNVStreamTcp::ReplayStart(LPJNVProtocolTCP lpProtocol,LPCTSTR lpszDevID,int iCenter,int iChn,LPCTSTR lpszTimeStart,LPCTSTR lpszTimeEnd,int iOnlyIFrame,int iRecType,int iStartPos,
	fcbJStream* pfcbStream,void* pUserParam)
{
	m_iStreamType	= eJNVStreamReplay;				// 流类型,见 eJNVStreamType
	m_lpProtocol	= lpProtocol;					// 协议设备
	StrNCpy(m_szDevID,lpszDevID,JNET_DEV_ID_LEN);	// 设备ID
	m_iChn			= iChn;							// 所属的通道号

	if ( m_lpProtocol==NULL ) return JNETErrParam;
	// 复制登录信息
	CopyConn(m_lpProtocol);

	// 构造请求结构体 begin
	StrNCpy(m_stReplayStart.szDeviceID,lpszDevID,JNET_DEV_ID_LEN);			// 设备ID
	// 中心或者设备录像
	if ( iCenter ) m_stReplayStart.bCenter	= true;
	else m_stReplayStart.bCenter			= false;
	m_stReplayStart.nChannel		= iChn;									// 通道编号
	m_stReplayStart.nSessionID		= m_stLoginInfo.nSessionID;				// Session id
	StrNCpy(m_stReplayStart.szStartTime,lpszTimeStart,JNET_TIME_STR_LEN);	// 开始时间
	StrNCpy(m_stReplayStart.szStopTime,lpszTimeEnd,JNET_TIME_STR_LEN);		// 结束时间
	m_stReplayStart.nOnlyIFrame		= iOnlyIFrame;							// 是否只要I帧
	m_stReplayStart.nRecordType		= iRecType;								// 录像类型
	m_stReplayStart.nStartPosition	= iStartPos;							// 开始位置
	// 构造请求结构体 end

	// 先只登录
	return Connect();

}
long CJNVStreamTcp::ReplayCtrl(int iCtrlType,int iValue)
{
	stJNVReplayCtrl	stReplayCtrl;

	SetNetHdr(&(stReplayCtrl.stHead), sizeof(stReplayCtrl.stHead), eJVNProReplayCtrlReq, stJNVProHead::GetSeq());
	StrNCpy(stReplayCtrl.szDeviceID,m_szDevID,JNET_DEV_ID_LEN);	// 设备ID
	stReplayCtrl.nChannel			= m_iChn;					// 通道编号(1开始)
	stReplayCtrl.nControlType		= iCtrlType;				// 控制类型(参见 eNVReplayType )
	stReplayCtrl.nValue				= iValue;					// 设置值
	Send(&stReplayCtrl,sizeof(stReplayCtrl));
	return JNETErrSuccess;
}
long CJNVStreamTcp::ReplayStop()
{
	Close();
	SendEvent(JNET_EET_STREAM_CLOSE,m_iStreamType,NULL,0);
	if ( m_pfWrite )
	{
		fclose(m_pfWrite);
		m_pfWrite = NULL;
	}
	return JNETErrSuccess;
}
// 设备录像回放相关 end

// 消息派分结果 begin
// 下发视频流
void CJNVStreamTcp::OnRetStreamData(void* pData,int iLen)
{
	PSTJNVStreamHeaderFrameInfo	pstStreamHeader	= NULL;
	PSTJNVStreamFrameVideoInfo	pStreamVideo	= NULL;
	PSTJNVStreamFrameAudioInfo	pStreamAudio	= NULL;
	__int64					tTime			= 0;			// 时间戳
	PSTJNVVideoFramInfo			pstFrame		= NULL;			// 帧结构

//	JTRACE("eNVStreamPayload FrameType:%i \r\n", m_stLastRecv.nParameter1 );
	// 流负载类型
//		eNVStreamPayloadInvalidFrame	= 0,			// 无效帧
//		eNVStreamPayloadHeaderFrame		= 0x68643030,	// 头帧('00hd')
//		eNVStreamPayloadVideoIFrame		= 0x62643030,	// 视频主帧('00db') I帧
//		eNVStreamPayloadVideoVFrame		= 0x63643030,	// 视频虚帧('00dc')	P帧
//		eNVStreamPayloadAudioFrame		= 0x62773030,	// 音频帧('00wb')
//		eNVStreamPayloadGPSFrame		= 0x73706730,	// GPS帧('0gps')
//		eNVStreamPayloadAlarmFrame		= 0x6d613030,	// 报警帧('00am')
	switch(m_stLastRecv.nParameter1)
	{
	case eNVStreamPayloadHeaderFrame	:		// 头帧('00hd')
		pstStreamHeader	= (PSTJNVStreamHeaderFrameInfo)(pData);
		memcpy(&m_stDataHeader,pstStreamHeader,sizeof(m_stDataHeader));
		JTRACE("eNVStreamPayload Receiver: Video  HFrame:%dx%d,rate=%d,type=0x%08x\r\n",
			pstStreamHeader->nWidth,pstStreamHeader->nHeight,pstStreamHeader->nFrameRate,pstStreamHeader->nAudioType);
		break;
	case eNVStreamPayloadVideoIFrame	:		// 视频主帧('00db')
		pStreamVideo	= (PSTJNVStreamFrameVideoInfo)(pData);
		JTRACE("eNVStreamPayload Receiver: Video  IFrame:len=%d,time=%lld \r\n",iLen,pStreamVideo->tTime);
		tTime	= pStreamVideo->tTime;
		pstFrame= (PSTJNVVideoFramInfo)((LPBYTE)pData-m_iDataOffset);
		pstFrame->dwMagic		= READER_MAGIC_JNV;				// 魔字
		pstFrame->dwFrameNo		= s_lFrameNo++;					// 帧号
		pstFrame->nWidth		= m_stDataHeader.nWidth;		// 宽度
		pstFrame->nHeight		= m_stDataHeader.nHeight;		// 高度
		pstFrame->nMediaType	= JNET_EMT_I_FRAME;				// 媒体类型,见 eJNetMediaType
		pstFrame->nFrameRate	= m_stDataHeader.nFrameRate;	// 帧率
		pstFrame->tTime			= tTime;						// 时间戳
		pstFrame->dwDataSize	= iLen-8;						// 帧数据长
		if ( m_pfcbStream )
		{
			m_pfcbStream((long)this,(LPBYTE)pstFrame,sizeof(stJNVVideoFramInfo)+pstFrame->dwDataSize,m_pStreamParam);
		}
		if ( m_pfWrite )
		{
			fwrite(pstFrame,sizeof(stJNVVideoFramInfo)+pstFrame->dwDataSize,1,m_pfWrite);
		}
		break;
	case eNVStreamPayloadVideoVFrame	:		// 视频虚帧('00dc')
		pStreamVideo	= (PSTJNVStreamFrameVideoInfo)(pData);
		JTRACE("eNVStreamPayload Receiver: Video  VFrame:len=%d,time=%lld\r\n",iLen,pStreamVideo->tTime);
		tTime	= pStreamVideo->tTime;
		pstFrame= (PSTJNVVideoFramInfo)((LPBYTE)pData-m_iDataOffset);
		pstFrame->dwMagic		= READER_MAGIC_JNV;				// 魔字
		pstFrame->dwFrameNo		= s_lFrameNo++;					// 帧号
		pstFrame->nWidth		= m_stDataHeader.nWidth;		// 宽度
		pstFrame->nHeight		= m_stDataHeader.nHeight;		// 高度
		pstFrame->nMediaType	= JNET_EMT_P_FRAME;				// 媒体类型,见 eJNetMediaType
		pstFrame->nFrameRate	= m_stDataHeader.nFrameRate;	// 帧率
		pstFrame->tTime			= tTime;						// 时间戳
		pstFrame->dwDataSize	= iLen-8;						// 帧数据长
		if ( m_pfcbStream )
		{
			m_pfcbStream((long)this,(LPBYTE)pstFrame,sizeof(stJNVVideoFramInfo)+pstFrame->dwDataSize,m_pStreamParam);
		}
		if ( m_pfWrite )
		{
			fwrite(pstFrame,sizeof(stJNVVideoFramInfo)+pstFrame->dwDataSize,1,m_pfWrite);
		}
		break;
	case eNVStreamPayloadAudioFrame		:		// 音频帧('00wb')
		pStreamAudio	= (PSTJNVStreamFrameAudioInfo)(pData);
		JTRACE("eNVStreamPayload AudioFrame:len=%d,%I64d\r\n",iLen,pStreamAudio->tTime);
		tTime	= pStreamAudio->tTime;
		pstFrame= (PSTJNVVideoFramInfo)((LPBYTE)pData-m_iDataOffset);
		pstFrame->dwMagic		= READER_MAGIC_JNV;				// 魔字
		pstFrame->dwFrameNo		= 0;							// 帧号
		pstFrame->nWidth		= m_stDataHeader.nWidth;		// 宽度
		pstFrame->nHeight		= m_stDataHeader.nHeight;		// 高度
		pstFrame->nMediaType	= JNET_EMT_A_FRAME;				// 媒体类型,见 eJNetMediaType
		pstFrame->nFrameRate	= m_stDataHeader.nFrameRate;	// 帧率
		pstFrame->tTime			= tTime;						// 时间戳
		pstFrame->dwDataSize	= iLen-8;						// 帧数据长
		if ( m_pfcbStream )
		{
			m_pfcbStream((long)this,(LPBYTE)pstFrame,sizeof(stJNVVideoFramInfo)+pstFrame->dwDataSize,m_pStreamParam);
		}
		if ( m_pfWrite )
		{
			fwrite(pstFrame,sizeof(stJNVVideoFramInfo)+pstFrame->dwDataSize,1,m_pfWrite);
		}
		break;
	case eNVStreamPayloadInvalidFrame	:		// 无效帧
		JTRACE("eNVStreamPayload InvalidFrame:len=%d\r\n",iLen);
		break;
	case eNVStreamPayloadGPSFrame		:		// GPS帧('0gps')
		JTRACE("eNVStreamPayload GPSFrame:len=%d\r\n",iLen);
		break;
	case eNVStreamPayloadAlarmFrame		:		// 报警帧('00am')
		JTRACE("eNVStreamPayload AlarmFrame:len=%d\r\n",iLen);
		break;
	default:
		JTRACE("eNVStreamPayload Unknown:len=%d\r\n",iLen);
		break;
	}
}
// 请求视频设备返回的错误码
void CJNVStreamTcp::OnRetStreamErr(void* pData,int iLen)
{
	if ( m_stLastRecv.nParameter1==0 )
	{
		SendEvent(JNET_EET_STREAM_OK,m_iStreamType,pData,iLen);
		// to do 写入到文件中
		if ( m_pfWrite==NULL )
		{
		//	m_pfWrite	= fopen("D:\\AAA.JAV","wb");
		}
	}
	else
	{
		SendEvent(JNET_EET_STREAM_ERROR,m_iStreamType,pData,iLen);
	}
	JTRACE("CJNVStreamTcp::OnRetStreamErr %d,%d,%d\r\n",iLen,m_stLastRecv.nParameter1,m_stLastRecv.nParameter2);
}
// 备录像按时间回放应答
void CJNVStreamTcp::OnRetReplayAnswer(void* pData,int iLen)
{
	JTRACE("CJNVStreamTcp::OnRetReplayAnswer %d,%d,%d\r\n",iLen,m_stLastRecv.nParameter1,m_stLastRecv.nParameter2);
}
// 消息派分结果 end



