// JNVCmdTcp.h: interface for the CJNVCmdTcp class.
//
//////////////////////////////////////////////////////////////////////

#if !defined(AFX_JNVCMDTCP_H__01E4C994_978C_4F4A_BB2F_C1038E5CB61E__INCLUDED_)
#define AFX_JNVCMDTCP_H__01E4C994_978C_4F4A_BB2F_C1038E5CB61E__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

#include "JNVProtocolTCP.h"
#include "JNVStreamTcp.h"
#include "JNVTalkTcp.h"

class CJNVCmdTcp;	typedef CJNVCmdTcp*		LPJNVCmdTcp;

class CJNVCmdTcp : public CJNVProtocolTCP 
{
	friend class CJNVStreamTcp;
public:
	CJNVCmdTcp();
	virtual ~CJNVCmdTcp();

public:
	// 请求设备列表:0,表示成功;其他,表示失败
	long GetDevList(LPCTSTR lpszFilePath);

	// 请求服务器列表:0,表示成功;其他,表示失败
	long GetServerList(LPCTSTR lpszFilePath,int nServerType);
	// 报警相关 begin
	long GetAlarmStart(LPCTSTR lpszDevID);
	long GetAlarmStop(LPCTSTR lpszDevID);
	// 报警相关 end

	// GPS信息相关 begin
	long GetGPSStart(LPCTSTR lpszDevID);
	long GetGPSStop(LPCTSTR lpszDevID);
	// GPS信息相关 end

	// 对讲相关 begin
	long TalkStart(LPCTSTR lpszDevID,int iChn);
	long TalkStop(long lTalk);
	long TalkSend(long lTalk,BYTE* lpData,int iLen);
	// 对讲相关 end

	// 录像相关 begin
	long RecQuery(LPCTSTR lpszDevID,int iCenter,int iType,LPCTSTR lpszTimeStart,LPCTSTR lpszTimeEnd,int iChnFlag,LPCTSTR lpszFilePath);
	long RecDownload(LPCTSTR lpszDevID,LPCTSTR lpszDevPath,int iStartPos,LPCTSTR lpszFilePath);
	// 录像相关 end

	// 实时视频相关 begin
	// 开始实时视频流,设备ID,通道编号(1开始),码流编号(保留),码流类型(保留)
	long RealStreamStart(LPCTSTR lpszDevID,int iChn,int iStream,int iType,fcbJStream* pfcbStream,void* pUserParam);
	long RealStreamReqIFrame(long lStream);
	long RealStreamStop(long lStream);
	void* GetStreamParam(long lStream);
	// 实时视频相关 end

	// 设备录像回放相关 begin
	long ReplayStart(LPCTSTR lpszDevID,int iCenter,int iChn,LPCTSTR lpszTimeStart,LPCTSTR lpszTimeEnd,int iOnlyIFrame,int iRecType,int iStartPos,
		fcbJStream* pfcbStream,void* pUserParam);
	long ReplayCtrl(long lStream,int iCtrlType,int iValue);
	long ReplayStop(long lStream);
	// 设备录像回放相关 end

	// 云台控制 begin
	long PtzCtrl(LPCTSTR lpszDevID,int iChn,int iAction,int iValue1,int iValue2);
	// 云台控制 end
protected:
	// 通知函数 begin
	// 用于扩展的接收处理
	virtual int OnExtRecv(int nType,void* lpBuf,int iDataLen);
	// 通知函数 end
	
	// 获取文件缓存
	LPBYTE GetFileBufSize(int iLen,eJNVFileType eType);
	// 写文件
	void WriteTempFile(LPCTSTR lpszFile,void* pBuf,int iSize);
protected:
	// 消息派分结果 begin
	void OnRetGetDevListRet(void* pData,int iLen);		// 请求设备列表应答
	void OnRetGetDevListFile(void* pData,int iLen);		// 设备列表文件传输
	void OnRetGetGPSInfo(void* pData,int iLen);			// 订阅坐标流返回
	void OnRetDispGPSInfo(void* pData,int iLen);		// 坐标流下发(BF)
	void OnRetRecQuery(void* pData,int iLen);			// 查询应答(大数据量)
	void OnRetRecListFile(void* pData,int iLen);		// 录像查询文件数据传输
	void OnRetRecDownload(void* pData,int iLen);		// 下载录像文件应答
	void OnRetRecDownloadFile(void* pData,int iLen);	// 录像文件流回调
	void OnRetAlarmDetail(void* pData,int iLen);		// 报警下发(详细数据)
	void OnRetGetAlarm(void* pData,int iLen);			// 订阅报警返回
	void OnRetAlarmSummary(void* pData,int iLen);		// 报警下发
	void OnRetRemoveAlarm(void* pData,int iLen);		// 消警
	void OnRetTalkData(void* pData,int iLen);			// 上传对讲流
	void OnRetTalkErr(void* pData,int iLen);			// 对讲异常
	void OnRetTalkAnswer(void* pData,int iLen);			// 对讲请求回应
	void OnRetGetServerListRet(void* pData,int iLen);	// 请求服务器列表应答
	void OnRetGetServerListFile(void* pData,int iLen);	// 服务器列表文件传输
	// 消息派分结果 end
public:
protected:
	int					m_iTaskID;						// 当前任务ID
	LPBYTE				m_pbyBufFile;					// 文件缓存(XML)
	int					m_iBufFileMax;					// 文件缓存大小
	int					m_iBufFileLen;					// 文件大小
	eJNVFileType		m_eFileType;					// 文件类型

	char				m_szDevListPath[MAX_PATH];		// 设备列表路径
	char				m_szRecListPath[MAX_PATH];		// 录像列表路径
	char				m_szRecDownloadPath[MAX_PATH];	// 录像下载路径
	char				m_szServerListPath[MAX_PATH];	// 服务器列表路径

	stJNVTalkReqSingle	m_stTalkReq;
	bool				m_bCanSend;						// 是否可以发送了

public:
	static  CJNVStreamTcp		m_arStream[JNET_MAX_STREAM];	// 流通道
	static	CJNVTalkTcp			m_arTalk[JNET_MAX_STREAM];		// 对讲通道
};

#endif // !defined(AFX_JNVCMDTCP_H__01E4C994_978C_4F4A_BB2F_C1038E5CB61E__INCLUDED_)
