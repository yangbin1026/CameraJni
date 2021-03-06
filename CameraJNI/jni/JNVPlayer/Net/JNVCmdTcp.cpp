// JNVCmdTcp.cpp: implementation of the CJNVCmdTcp class.
//
//////////////////////////////////////////////////////////////////////
#include "JNVCmdTcp.h"
#undef	LOG_TAG
#define	LOG_TAG		"JNVCmdTcp.cpp"

//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////

CJNVStreamTcp		CJNVCmdTcp::m_arStream[JNET_MAX_STREAM];	// 流通道
CJNVTalkTcp			CJNVCmdTcp::m_arTalk[JNET_MAX_STREAM];		// 对讲通道
CJNVCmdTcp::CJNVCmdTcp()
{
	m_bAutoLogin		= true;					// 是否自动登录
	m_iDataOffset		= 0;					// 接收偏移(为了组帧方便,少一次的拷贝过程)

	m_iTaskID			= 0;					// 当前任务ID
	m_pbyBufFile		= NULL;					// 文件缓存(XML)
	m_iBufFileLen		= 0;					// 文件大小
	m_eFileType			= eJNVFileUndef;		// 文件类型
	
	memset(m_szDevListPath,0,MAX_PATH);			// 设备列表路径
	memset(m_szRecListPath,0,MAX_PATH);			// 录像列表路径
	memset(m_szRecDownloadPath,0,MAX_PATH);		// 录像下载路径
	memset(m_szServerListPath,0,MAX_PATH);					// 服务器列表路径
	
	memset(&m_stTalkReq,0,sizeof(stJNVTalkReqSingle));		// 对讲流请求参数
	m_bCanSend			= false;				// 是否可以发送了
}

CJNVCmdTcp::~CJNVCmdTcp()
{
	Logout();
	SAFE_DELETE(m_pbyBufFile);					// 文件缓存(XML)
	m_iBufFileLen	= 0;						// 文件大小
	m_eFileType		= eJNVFileUndef;			// 文件类型
}

// 通知函数 begin
// 用于扩展的接收处理
int CJNVCmdTcp::OnExtRecv(int nType,void* lpBuf,int iDataLen)
{
	// 消息派分
	switch(nType)
	{
	case eJVNProGetDevListRet:			OnRetGetDevListRet(lpBuf,iDataLen);		break;		// 请求设备列表应答
	case eJVNProGetDevListData:			OnRetGetDevListFile(lpBuf,iDataLen);	break;		// 设备列表文件传输
	
	case eJVNProGetServerListRet:		OnRetGetServerListRet(lpBuf,iDataLen);	break;		// 请求设备列表应答
	case eJVNProDatabaseDataTrans:		OnRetGetServerListFile(lpBuf,iDataLen);	break;		// 请求设备列表应答
	case eJVNProGetGPSInfoRet:			OnRetGetGPSInfo(lpBuf,iDataLen);		break;		// 订阅坐标流返回
	case eJVNProGetGPSInfoData:			OnRetDispGPSInfo(lpBuf,iDataLen);		break;		// 坐标流下发(BF)
	case eJVNProGetRecListRet:			OnRetRecQuery(lpBuf,iDataLen);			break;		// 查询应答(大数据量)
	case eJVNProGetRecListData:			OnRetRecListFile(lpBuf,iDataLen);		break;		// 录像查询文件数据传输
	case eJVNProDownRecRet:				OnRetRecDownload(lpBuf,iDataLen);		break;		// 下载录像文件应答
	case eJVNProDownRecData:			OnRetRecDownloadFile(lpBuf,iDataLen);	break;		// 录像文件流回调
		
	case eJVNProAlarmDetail:			OnRetAlarmDetail(lpBuf,iDataLen);		break;		// 报警下发(详细数据)
	case eJVNProGetAlarmRet:			OnRetGetAlarm(lpBuf,iDataLen);			break;		// 订阅报警返回
	case eJVNProAlarmSummary:			OnRetAlarmSummary(lpBuf,iDataLen);		break;		// 报警下发
	case eJVNProRemoveAlarmRet:			OnRetRemoveAlarm(lpBuf,iDataLen);		break;		// 消警

	case eJVNProTalkRet:				OnRetTalkAnswer(lpBuf,iDataLen);		break;		// 对讲请求回应
	case eJVNProTalkTransData:			OnRetTalkData(lpBuf,iDataLen);			break;		// 上传对讲流 
	case eJVNProTalkErr:				OnRetTalkErr(lpBuf,iDataLen);			break;		// 对讲异常
	default:
		return 0;
	}
	return 1;
}
// 通知函数 end

// 获取文件缓存
LPBYTE CJNVCmdTcp::GetFileBufSize(int iLen,eJNVFileType eType)
{
	m_eFileType	= eType;

	if ( m_iBufFileMax>=iLen )
	{
		m_iBufFileLen	= iLen;
		return m_pbyBufFile;
	}
	
	SAFE_DELETE(m_pbyBufFile);
	if ( iLen==0 )
	{
		m_eFileType	= eType;
		m_iBufFileMax	= iLen;
		m_iBufFileLen	= iLen;
	}
	else
	{
		m_pbyBufFile = new BYTE[iLen];
		if ( m_pbyBufFile )
		{
			m_iBufFileMax	= iLen;
			m_iBufFileLen	= iLen;
		}
		else
		{
			m_iBufFileMax	= 0;
			m_iBufFileLen	= 0;
		}
	}
	return m_pbyBufFile;
}
// 写文件
void CJNVCmdTcp::WriteTempFile(LPCTSTR lpszFile,void* pBuf,int iSize)
{
	// 写到临时目录中 begin
	FILE*	pfWrite	= NULL;
	pfWrite = fopen(lpszFile,"wb");
	if ( pfWrite )
	{
		fwrite(pBuf,iSize,1,pfWrite);
		fclose(pfWrite);
	}
	// 写到临时目录中 end
}
// 请求设备列表:0,表示成功;其他,表示失败
long CJNVCmdTcp::GetDevList(LPCTSTR lpszFilePath)
{
	stJNVProHead		stPro;
	SetNetHdr(&stPro, stPro.s_iSize, eJVNProGetDevListReq, stPro.GetSeq());
	strcpy(m_szDevListPath,lpszFilePath);		// 设备列表路径

	Send(&stPro,stPro.nLength);
	return JNETErrSuccess;
}


// 请求服务器列表:0,表示成功;其他,表示失败
long CJNVCmdTcp::GetServerList(LPCTSTR lpszFilePath,int nServerType)
{
	stJNVProHead		stPro;
	SetNetHdr(&stPro, stPro.s_iSize, eJVNProGetServerListReq, stPro.GetSeq());
	stPro.nParameter1 = nServerType;
	strcpy(m_szServerListPath,lpszFilePath);		// 设备列表路径

	Send(&stPro,stPro.nLength);
	return JNETErrSuccess;
}
// 报警相关 begin
long CJNVCmdTcp::GetAlarmStart(LPCTSTR lpszDevID)
{
	if (lpszDevID==NULL || strlen(lpszDevID)==0)
	{
		// 订阅全部
		stJNVProHead		stPro;
		SetNetHdr(&stPro, stPro.s_iSize, eJVNProGetAllAlarmReq, stPro.GetSeq());
		stPro.nParameter1	= 1;
		Send(&stPro,stPro.nLength);
	}
	else
	{
		// 订阅单个
		stJNVProAlarmStart	stAlarmStart;
		SetNetHdr(&(stAlarmStart.stHead), sizeof(stAlarmStart), eJVNProGetDevAlarmReq, stJNVProHead::GetSeq());
		StrNCpy(stAlarmStart.szDeviceID,lpszDevID,JNET_DEV_ID_LEN);	// 设备ID
		Send(&stAlarmStart,sizeof(stAlarmStart));
	}
	return JNETErrSuccess;
}
long CJNVCmdTcp::GetAlarmStop(LPCTSTR lpszDevID)
{
	if (lpszDevID==NULL || strlen(lpszDevID)==0)
	{
		// 订阅全部
		stJNVProHead		stPro;
		SetNetHdr(&stPro, stPro.s_iSize, eJVNProGetAllAlarmReq, stPro.GetSeq());
		stPro.nParameter1	= 0;
		Send(&stPro,stPro.nLength);
	}
	else
	{		
		stJNVProAlarmStop	stAlarmStop;
		SetNetHdr(&(stAlarmStop.stHead), sizeof(stAlarmStop), eJVNProStopDevAlarmReq, stJNVProHead::GetSeq());
		StrNCpy(stAlarmStop.szDeviceID,lpszDevID,JNET_DEV_ID_LEN);	// 设备ID
		Send(&stAlarmStop,sizeof(stAlarmStop));
	}
	return JNETErrSuccess;
}
// 报警相关 end

// GPS信息相关 begin
long CJNVCmdTcp::GetGPSStart(LPCTSTR lpszDevID)
{
	if (lpszDevID==NULL || strlen(lpszDevID)==0)
	{
		// 订阅全部
		stJNVProHead		stPro;
		SetNetHdr(&stPro, stPro.s_iSize, eJVNProGetAllGPSReq, stPro.GetSeq());
		stPro.nParameter1	= 1;
		Send(&stPro,stPro.nLength);
	}
	else
	{
		stJNVProGPSStart	stGpsStart;
		SetNetHdr(&(stGpsStart.stHead), sizeof(stGpsStart), eJVNProGetDevGPSReq, stJNVProHead::GetSeq());
		StrNCpy(stGpsStart.szDeviceID,lpszDevID,JNET_DEV_ID_LEN);	// 设备ID
		Send(&stGpsStart,sizeof(stGpsStart));
	}
	return JNETErrSuccess;
}
long CJNVCmdTcp::GetGPSStop(LPCTSTR lpszDevID)
{
	if (lpszDevID==NULL || strlen(lpszDevID)==0)
	{
		// 订阅全部
		stJNVProHead		stPro;
		SetNetHdr(&stPro, stPro.s_iSize, eJVNProGetAllGPSReq, stPro.GetSeq());
		stPro.nParameter1	= 0;
		Send(&stPro,stPro.nLength);
	}
	else
	{
		stJNVProGPSStop		stGpsStop;
		SetNetHdr(&(stGpsStop.stHead), sizeof(stGpsStop), eJVNProStopDevGPSReq, stJNVProHead::GetSeq());
		StrNCpy(stGpsStop.szDeviceID,lpszDevID,JNET_DEV_ID_LEN);	// 设备ID
		Send(&stGpsStop,sizeof(stGpsStop));
	}
	return JNETErrSuccess;
}
// GPS信息相关 end

// 对讲相关 begin
long CJNVCmdTcp::TalkStart(LPCTSTR lpszDevID,int iChn)
{
	if ( iChn<1 || iChn>JNET_MAX_CHN ) return JNETErrParam;
	
	/*
	// 使用当前连接 begin
	// 构造请求结构体 begin
	SetNetHdr(&(m_stTalkReq.stHead), sizeof(m_stTalkReq), eJVNProTalkReq, stJNVProHead::GetSeq());
	m_stTalkReq.nWorkMode			= 0;				// 工作方式(0:对讲,1:广播,2:PTT,3:监听)
	m_stTalkReq.usControl			= 1;				// 控制方式(0:取消,1:请求)
	m_stTalkReq.usChannel			= iChn;				// 通道
	m_stTalkReq.nDeviceNumber		= 1;				// 设备数量
	m_stTalkReq.nSessionID			= m_stLoginInfo.nSessionID;	// SessionID
	StrNCpy(m_stTalkReq.szDeviceID,lpszDevID,JNET_DEV_ID_LEN);	// 设备ID
	// 构造请求结构体 end
	Send(&m_stTalkReq,sizeof(m_stTalkReq));
	return 1;
	// 使用当前连接 end
	*/

	// 新建连接 begin
	// 查找空闲的通道
	int	i = 0;
	for ( i=0;i<JNET_MAX_STREAM;i++ )
	{
		if ( m_arTalk[i].GetHandle()==INVALID_SOCKET )
		{
			m_arTalk[i].TalkStart(this,lpszDevID,iChn);
			return i+1;
		}
	}
	// 新建连接 end
	return JNETErrNoFreePort;
}
long CJNVCmdTcp::TalkStop(long lTalk)
{
	if ( lTalk<1 || lTalk>JNET_MAX_STREAM ) return JNETErrParam;
	
	/*
	// 使用当前连接 begin
	// 构造请求结构体 begin
	SetNetHdr(&(m_stTalkReq.stHead), sizeof(m_stTalkReq), eJVNProTalkReq, stJNVProHead::GetSeq());
	m_stTalkReq.nWorkMode			= 0;				// 工作方式(0:对讲,1:广播,2:PTT,3:监听)
	m_stTalkReq.usControl			= 0;				// 控制方式(0:取消,1:请求)
	m_stTalkReq.usChannel			= 1;				// 通道
	m_stTalkReq.nDeviceNumber		= 1;				// 设备数量
	m_stTalkReq.nSessionID			= m_stLoginInfo.nSessionID;	// SessionID
//	StrNCpy(m_stTalkReq.szDeviceID,m_szDevID,JNET_DEV_ID_LEN);	// 设备ID
	// 构造请求结构体 end
	Send(&m_stTalkReq,sizeof(m_stTalkReq));
	Close();
	SendEvent(JNET_EET_TALK_CLOSE,0,NULL,0);
	m_bCanSend			= false;				// 是否可以发送了
	return JNETErrSuccess;
	// 使用当前连接 end
	*/

	// 新建连接 begin
	// 每一通道一个对讲流
	return m_arTalk[lTalk-1].TalkStop();
	// 新建连接 end
}
long CJNVCmdTcp::TalkSend(long lTalk,BYTE* lpData,int iLen)
{
	if ( lTalk<1 || lTalk>JNET_MAX_STREAM ) return JNETErrParam;
	
	/*
	// 使用当前连接 begin
	char			szTemp[2048]	= {0};
	PSTJNVProHead	pstNetHdr		= (PSTJNVProHead)szTemp;
	if ( m_bCanSend==false )
	{
		// 是否可以发送了
		return JNETErrSuccess;
	}
//	SetNetHdr(pstNetHdr, iLen, eJVNProTalkTransData, stJNVProHead::GetSeq());
	SetNetHdr(pstNetHdr, pstNetHdr->s_iSize+iLen, eJVNProTalkTransData, 0);
	memcpy(pstNetHdr+1,lpData,iLen);
	Send(szTemp,pstNetHdr->s_iSize+iLen);
	JTRACE("CJNVCmdTcp::TalkSendData:%d\r\n",pstNetHdr->s_iSize+iLen);
	return JNETErrSuccess;
	// 使用当前连接 end
	*/

	// 新建连接 begin
	// 每一通道一个对讲流
	return m_arTalk[lTalk-1].TalkSendData(lpData,iLen);
	// 新建连接 end
}
// 对讲相关 end

// 录像相关 begin
long CJNVCmdTcp::RecQuery(LPCTSTR lpszDevID,int iCenter,int iType,LPCTSTR lpszTimeStart,LPCTSTR lpszTimeEnd,int iChnFlag,LPCTSTR lpszFilePath)
{
	stJNVRecQuery		stRecQuery;
	SetNetHdr(&(stRecQuery.stHead), sizeof(stRecQuery), eJVNProGetRecListReq, stJNVProHead::GetSeq());
	StrNCpy(stRecQuery.szDeviceID,lpszDevID,JNET_DEV_ID_LEN);	// 设备ID
	stRecQuery.iCenter						= iCenter;			// 中心或者设备录像(1,表示中心录像;0,表示设备录像)
	stRecQuery.nStorage						= 0;				// 存储介质(保留)
	stRecQuery.nType						= iType;			// 录像类型
	StrNCpy(stRecQuery.szStartTime,lpszTimeStart,JNET_TIME_STR_LEN);		// 开始时间，时间格式如：年-月-日 时:分:秒
	StrNCpy(stRecQuery.szStopTime,lpszTimeEnd,JNET_TIME_STR_LEN);			// 结束时间-月-日 时:分:秒
	stRecQuery.nChannelList					= iChnFlag;		// 通道编号列表(按位表示通道:比如:0x0001 表示第一个通道)

	strcpy(m_szRecListPath,lpszFilePath);		// 录像列表路径
	Send(&stRecQuery,sizeof(stRecQuery));
	return JNETErrSuccess;
	
}
long CJNVCmdTcp::RecDownload(LPCTSTR lpszDevID,LPCTSTR lpszDevPath,int iStartPos,LPCTSTR lpszFilePath)
{
	stJNVRecDown		stRecDown;
	SetNetHdr(&(stRecDown.stHead), sizeof(stRecDown), eJVNProDownRecReq, stJNVProHead::GetSeq());
	StrNCpy(stRecDown.szDeviceID,lpszDevID,JNET_DEV_ID_LEN);	// 设备ID
	stRecDown.nSessionID					= m_stLoginInfo.nSessionID;	// SessionID
	StrNCpy(stRecDown.szDevPath,lpszDevPath,JNET_FILE_PATH_LEN);// 文件路径
	stRecDown.iStartPos						= iStartPos;		// 开始位置

	strcpy(m_szRecDownloadPath,lpszFilePath);	// 录像下载路径
	Send(&stRecDown,sizeof(stRecDown));
	return JNETErrSuccess;
}
// 录像相关 end

// 实时视频相关 begin
// 开始实时视频流,设备ID,通道编号(1开始),码流编号(保留),SessionID,码流类型(保留)
long CJNVCmdTcp::RealStreamStart(LPCTSTR lpszDevID,int iChn,int iStream,int iType,fcbJStream* pfcbStream,void* pUserParam)
{
	if ( iChn<1 || iChn>JNET_MAX_CHN ) return JNETErrParam;
	
	// 查找空闲的通道
	int	i = 0;
	for ( i=0;i<JNET_MAX_STREAM;i++ )
	{
		if ( m_arStream[i].GetHandle()==INVALID_SOCKET )
		{
			m_arStream[i].RealStreamStart(this,lpszDevID,iChn,iStream,iType,pfcbStream,pUserParam);
			return i+1;
		}
	}
	return JNETErrNoFreePort;
}
long CJNVCmdTcp::RealStreamReqIFrame(long lStream)
{
	if ( lStream<1 || lStream>JNET_MAX_STREAM ) return JNETErrParam;
	
	// 每一通道一个实时流
	return m_arStream[lStream-1].RealStreamReqIFrame();
}
long CJNVCmdTcp::RealStreamStop(long lStream)
{
	if ( lStream<1 || lStream>JNET_MAX_STREAM ) return JNETErrParam;
	
	// 每一通道一个实时流
	return m_arStream[lStream-1].RealStreamStop();
}
void* CJNVCmdTcp::GetStreamParam(long lStream)
{
	if ( lStream<1 || lStream>JNET_MAX_STREAM ) return NULL;
	
	// 每一通道一个实时流
	return m_arStream[lStream-1].GetStreamParam();
}
// 实时视频相关 end
// 设备录像回放相关 begin
long CJNVCmdTcp::ReplayStart(LPCTSTR lpszDevID,int iCenter,int iChn,LPCTSTR lpszTimeStart,LPCTSTR lpszTimeEnd,int iOnlyIFrame,int iRecType,int iStartPos,
	fcbJStream* pfcbStream,void* pUserParam)
{
	if ( iChn<1 || iChn>JNET_MAX_CHN ) return JNETErrParam;
	
	// 查找空闲的通道
	int	i = 0;
	for ( i=0;i<JNET_MAX_STREAM;i++ )
	{
		if ( m_arStream[i].GetHandle()==INVALID_SOCKET )
		{
			m_arStream[i].ReplayStart(this,lpszDevID,iCenter,iChn,lpszTimeStart,lpszTimeEnd,iOnlyIFrame,iRecType,iStartPos,
				pfcbStream,pUserParam);
			return i+1;
		}
	}
	return JNETErrNoFreePort;
}
long CJNVCmdTcp::ReplayCtrl(long lStream,int iCtrlType,int iValue)
{
	if ( lStream<1 || lStream>JNET_MAX_STREAM ) return JNETErrParam;
	
	// 每一通道一个实时流
	return m_arStream[lStream-1].ReplayCtrl(iCtrlType,iValue);
}
long CJNVCmdTcp::ReplayStop(long lStream)
{
	if ( lStream<1 || lStream>JNET_MAX_STREAM ) return JNETErrParam;
	
	// 每一通道一个实时流
	return m_arStream[lStream-1].ReplayStop();
}
// 设备录像回放相关 end
// 云台控制 begin
long CJNVCmdTcp::PtzCtrl(LPCTSTR lpszDevID,int iChn,int iAction,int iValue1,int iValue2)
{
	stJNVPtzCtrl		stPtzCtrl;
	SetNetHdr(&(stPtzCtrl.stHead), sizeof(stPtzCtrl), eJVNProPTZCtrlReq, stJNVProHead::GetSeq());
	StrNCpy(stPtzCtrl.szDeviceID,lpszDevID,JNET_DEV_ID_LEN);	// 设备ID
	stPtzCtrl.nChannelIndex			= iChn;						// 通道编号
	stPtzCtrl.nActionType			= iAction;					// 控制类型(参见 eNVPtzActionType)
	stPtzCtrl.nReserved1			= iValue1;					// 保留1(x速度/y速度/预置位)
	stPtzCtrl.nReserved2			= iValue2;					// 保留2(y速度)
	stPtzCtrl.nSessionID			= m_stLoginInfo.nSessionID;	// SessionID

	Send(&stPtzCtrl,sizeof(stPtzCtrl));
	return JNETErrSuccess;
}
// 控制云台 end
// 用于操作的虚函数 end

// 消息派分结果 begin
// 请求设备列表应答
void CJNVCmdTcp::OnRetGetDevListRet(void* pData,int iLen)
{
	if ( m_stLastRecv.nParameter1!=0 || m_stLastRecv.nParameter2!=m_iTaskID )
	{
		// 表示失败
		m_iTaskID	= 0;					// 当前任务ID
		SendEvent(JNET_EET_EVENT_DEV_LIST,eJNVFileErr,NULL,0);
	}
	else
	{
		WriteTempFile(m_szDevListPath,m_pbyBufFile,m_iBufFileLen);	// 写到临时目录中
		// 成功
		SendEvent(JNET_EET_EVENT_DEV_LIST,m_eFileType,(void*)m_szDevListPath,strlen(m_szDevListPath));
	}
}
// 设备列表文件传输
void CJNVCmdTcp::OnRetGetDevListFile(void* pData,int iLen)
{
	LPBYTE				lpFileBuf	= NULL;
	PSTJNVProFileData	pstFileData	= (PSTJNVProFileData)pData;

	m_iTaskID	= pstFileData->nTransID;	// 当前任务ID
	lpFileBuf	= GetFileBufSize(m_stLastRecv.nParameter1,eJNVFileDevList);
	memcpy(lpFileBuf+m_stLastRecv.nParameter2,pstFileData->pFileData,iLen-4);
}

// 请求服务器列表应答
void CJNVCmdTcp::OnRetGetServerListRet(void* pData,int iLen)
{
	if ( m_stLastRecv.nParameter1!=0 || m_stLastRecv.nParameter2!=m_iTaskID )
	{
		// 表示失败
		m_iTaskID	= 0;					// 当前任务ID
		SendEvent(JNET_EET_EVENT_SERVER_LIST,eJNVFileErr,NULL,0);
	}
	else
	{
		WriteTempFile(m_szServerListPath,m_pbyBufFile,m_iBufFileLen);	// 写到临时目录中
		// 成功
		SendEvent(JNET_EET_EVENT_SERVER_LIST,m_eFileType,(void*)m_szServerListPath,strlen(m_szServerListPath));
	}
}
// 服务器列表文件传输
void CJNVCmdTcp::OnRetGetServerListFile(void* pData,int iLen)
{
	LPBYTE				lpFileBuf	= NULL;
	PSTJNVProFileData	pstFileData	= (PSTJNVProFileData)pData;

	m_iTaskID	= pstFileData->nTransID;	// 当前任务ID
	lpFileBuf	= GetFileBufSize(m_stLastRecv.nParameter1,eJNVFileServerList);
	memcpy(lpFileBuf+m_stLastRecv.nParameter2,pstFileData->pFileData,iLen-4);
}
// 订阅坐标流返回
void CJNVCmdTcp::OnRetGetGPSInfo(void* pData,int iLen)
{
//	JTRACE("CJNVCmdTcp::OnRetGetGPSInfo %d\r\n",iLen);
}
// 坐标流下发(BF)
void CJNVCmdTcp::OnRetDispGPSInfo(void* pData,int iLen)
{
//	PSTJNVGPSAll	pstGpsAll	= (PSTJNVGPSAll)pData;
	SendEvent(JNET_EET_EVENT_GPS_INFO,0,(void*)pData,iLen);
}
// 查询应答(大数据量)
void CJNVCmdTcp::OnRetRecQuery(void* pData,int iLen)
{
	if ( m_stLastRecv.nParameter1!=0 || m_stLastRecv.nParameter2!=m_iTaskID )
	{
		// 表示失败
		m_iTaskID	= 0;					// 当前任务ID
		SendEvent(JNET_EET_EVENT_REC_LIST,eJNVFileErr,NULL,0);
	}
	else
	{
		// 写文件 begin
		FILE*				pfWrite			= NULL;
		int					i				= 0;
		int					iCount			= (m_iBufFileLen/sizeof(stJNVRecSumInfo));
		PSTJNVRecSumInfo	pstRecSumInfo	= NULL;
		char				szBuf[256]		= {0};
		pfWrite = fopen(m_szRecListPath,"wb");
		if ( pfWrite )
		{
			sprintf(szBuf,"<?xml version=\"1.0\" encoding=\"gb2312\" ?><msg>");
			fwrite(szBuf,strlen(szBuf),1,pfWrite);

			JTRACE("CJNVCmdTcp::OnRetRecQuery %d,%d\r\n",m_iBufFileLen,iCount);
			pstRecSumInfo = (PSTJNVRecSumInfo)(m_pbyBufFile);
			for (i=0;i<iCount;i++)
			{
				sprintf(szBuf,"<recInfo fileName=\"%s\" bTime=\"%s\" eTime=\"%s\" fileSize=\"%u\" timeLen=\"%u\" chnIndex=\"%d\" recType=\"%d\" />",
					pstRecSumInfo->szFileName,pstRecSumInfo->szStartTime,pstRecSumInfo->szStopTime,
					pstRecSumInfo->uiFileSize,pstRecSumInfo->uiSeconds,pstRecSumInfo->nChannelIndex,pstRecSumInfo->nRecordType);
				fwrite(szBuf,strlen(szBuf),1,pfWrite);
				JTRACE("%03d Rec name=%s,%s,%s,size=%d,len=%d,chn=%d,type=%d\r\n",i,
					pstRecSumInfo->szFileName,pstRecSumInfo->szStartTime,pstRecSumInfo->szStopTime,
					pstRecSumInfo->uiFileSize,pstRecSumInfo->uiSeconds,pstRecSumInfo->nChannelIndex,pstRecSumInfo->nRecordType);
				pstRecSumInfo++;
			}
			sprintf(szBuf,"</msg>");
			fwrite(szBuf,strlen(szBuf),1,pfWrite);

			fclose(pfWrite);
		}
		// 写文件 end
		// 成功
		SendEvent(JNET_EET_EVENT_REC_LIST,m_eFileType,(void*)m_szRecListPath,strlen(m_szRecListPath));
	}
}
// 录像查询文件数据传输
void CJNVCmdTcp::OnRetRecListFile(void* pData,int iLen)
{
	LPBYTE				lpFileBuf		= NULL;
	PSTJNVProFileData	pstFileData		= (PSTJNVProFileData)pData;
	float				fPercent		= 0;

	m_iTaskID	= pstFileData->nTransID;	// 当前任务ID
	lpFileBuf	= GetFileBufSize(m_stLastRecv.nParameter1,eJNVFileRecList);
	memcpy(lpFileBuf+m_stLastRecv.nParameter2,pstFileData->pFileData,iLen-4);
	
	fPercent	= ((float)(m_stLastRecv.nParameter2*100))/((float)(m_stLastRecv.nParameter1));
	// 上报进度
	SendEvent(JNET_EET_EVENT_TRANS_PERCENT,m_eFileType,&fPercent,sizeof(fPercent));
}
// 下载录像文件应答
void CJNVCmdTcp::OnRetRecDownload(void* pData,int iLen)
{
	if ( m_stLastRecv.nParameter1!=0 || m_stLastRecv.nParameter2!=m_iTaskID )
	{
		// 表示失败
		m_iTaskID	= 0;					// 当前任务ID
		SendEvent(JNET_EET_EVENT_REC_DOWN,eJNVFileErr,NULL,0);
	}
	else
	{
		WriteTempFile(m_szRecDownloadPath,m_pbyBufFile,m_iBufFileLen);	// 写到临时目录中
		// 成功
		SendEvent(JNET_EET_EVENT_REC_DOWN,m_eFileType,(void*)m_szRecDownloadPath,strlen(m_szRecDownloadPath));
	}
}
// 录像文件流回调
void CJNVCmdTcp::OnRetRecDownloadFile(void* pData,int iLen)
{
	LPBYTE				lpFileBuf		= NULL;
	PSTJNVProFileData	pstFileData		= (PSTJNVProFileData)pData;
	int					iWriteLen		= 0;
	float				fPercent		= 0;

	iWriteLen	= GetMin(m_stLastRecv.nParameter1-m_stLastRecv.nParameter2,(iLen-4));
	if ( iWriteLen<=0 ) 
	{
		JTRACE("CJNVCmdTcp::OnRetRecDownloadFile xxx p1=%d,p2=%d,len=%d\r\n",
			m_stLastRecv.nParameter1,m_stLastRecv.nParameter2,iLen-4);
		return;
	}

	m_iTaskID	= pstFileData->nTransID;	// 当前任务ID
	lpFileBuf	= GetFileBufSize(m_stLastRecv.nParameter1,eJNVFileRecDown);
	memcpy(lpFileBuf+m_stLastRecv.nParameter2,pstFileData->pFileData,iWriteLen);
	fPercent	= ((float)(m_stLastRecv.nParameter2*100))/((float)(m_stLastRecv.nParameter1));

	// 上报进度
	SendEvent(JNET_EET_EVENT_TRANS_PERCENT,m_eFileType,&fPercent,sizeof(fPercent));
//	JTRACE("CJNVCmdTcp::OnRetRecDownloadFile p1=%d,p2=%d,len=%d...%.2f%%\r\n",
//		m_stLastRecv.nParameter1,m_stLastRecv.nParameter2,iLen-4,fPercent);
}
// 报警下发(详细数据)
void CJNVCmdTcp::OnRetAlarmDetail(void* pData,int iLen)
{
	JTRACE("CJNVCmdTcp::OnRetAlarmDetail:%d\r\n",iLen);
}
// 订阅报警返回
void CJNVCmdTcp::OnRetGetAlarm(void* pData,int iLen)
{
	JTRACE("CJNVCmdTcp::OnRetGetAlarm:%d\r\n",iLen);
}
// 报警下发
void CJNVCmdTcp::OnRetAlarmSummary(void* pData,int iLen)
{
	PSTAlarmData	pstAlarm	= (PSTAlarmData)pData;
	
	// 超几个字节无所谓,因为在申请时会多申请128个字节的
	pstAlarm->nChannel		= m_stLastRecv.nParameter1;
	pstAlarm->nAlarmType	= m_stLastRecv.nParameter2;
//	JTRACE("CJNVCmdTcp::OnRetAlarmSummary:%d\r\n",iLen);
	SendEvent(JNET_EET_EVENT_NOTIFY,pstAlarm->nAlarmType,(void*)pstAlarm,sizeof(stAlarmData));
}
// 消警
void CJNVCmdTcp::OnRetRemoveAlarm(void* pData,int iLen)
{
	JTRACE("CJNVCmdTcp::OnRetRemoveAlarm:%d\r\n",iLen);
}
// 上传对讲流
void CJNVCmdTcp::OnRetTalkData(void* pData,int iLen)
{
//	JTRACE("CJNVCmdTcp OnRetTalkData:len=%d\r\n",iLen);
}
// 对讲异常
void CJNVCmdTcp::OnRetTalkErr(void* pData,int iLen)
{
	m_bCanSend			= false;				// 是否可以发送了
	JTRACE("CJNVCmdTcp::OnRetTalkErr %d,%d,%d\r\n",iLen,m_stLastRecv.nParameter1,m_stLastRecv.nParameter2);
}
// 对讲请求回应
void CJNVCmdTcp::OnRetTalkAnswer(void* pData,int iLen)
{
	m_bCanSend			= true;				// 是否可以发送了
	JTRACE("CJNVCmdTcp::OnRetTalkAnswer %d,%d,%d\r\n",iLen,m_stLastRecv.nParameter1,m_stLastRecv.nParameter2);
}
// 消息派分结果 end
