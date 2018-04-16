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
	// �����豸�б�:0,��ʾ�ɹ�;����,��ʾʧ��
	long GetDevList(LPCTSTR lpszFilePath);

	// ����������б�:0,��ʾ�ɹ�;����,��ʾʧ��
	long GetServerList(LPCTSTR lpszFilePath,int nServerType);
	// ������� begin
	long GetAlarmStart(LPCTSTR lpszDevID);
	long GetAlarmStop(LPCTSTR lpszDevID);
	// ������� end

	// GPS��Ϣ��� begin
	long GetGPSStart(LPCTSTR lpszDevID);
	long GetGPSStop(LPCTSTR lpszDevID);
	// GPS��Ϣ��� end

	// �Խ���� begin
	long TalkStart(LPCTSTR lpszDevID,int iChn);
	long TalkStop(long lTalk);
	long TalkSend(long lTalk,BYTE* lpData,int iLen);
	// �Խ���� end

	// ¼����� begin
	long RecQuery(LPCTSTR lpszDevID,int iCenter,int iType,LPCTSTR lpszTimeStart,LPCTSTR lpszTimeEnd,int iChnFlag,LPCTSTR lpszFilePath);
	long RecDownload(LPCTSTR lpszDevID,LPCTSTR lpszDevPath,int iStartPos,LPCTSTR lpszFilePath);
	// ¼����� end

	// ʵʱ��Ƶ��� begin
	// ��ʼʵʱ��Ƶ��,�豸ID,ͨ�����(1��ʼ),�������(����),��������(����)
	long RealStreamStart(LPCTSTR lpszDevID,int iChn,int iStream,int iType,fcbJStream* pfcbStream,void* pUserParam);
	long RealStreamReqIFrame(long lStream);
	long RealStreamStop(long lStream);
	void* GetStreamParam(long lStream);
	// ʵʱ��Ƶ��� end

	// �豸¼��ط���� begin
	long ReplayStart(LPCTSTR lpszDevID,int iCenter,int iChn,LPCTSTR lpszTimeStart,LPCTSTR lpszTimeEnd,int iOnlyIFrame,int iRecType,int iStartPos,
		fcbJStream* pfcbStream,void* pUserParam);
	long ReplayCtrl(long lStream,int iCtrlType,int iValue);
	long ReplayStop(long lStream);
	// �豸¼��ط���� end

	// ��̨���� begin
	long PtzCtrl(LPCTSTR lpszDevID,int iChn,int iAction,int iValue1,int iValue2);
	// ��̨���� end
protected:
	// ֪ͨ���� begin
	// ������չ�Ľ��մ���
	virtual int OnExtRecv(int nType,void* lpBuf,int iDataLen);
	// ֪ͨ���� end
	
	// ��ȡ�ļ�����
	LPBYTE GetFileBufSize(int iLen,eJNVFileType eType);
	// д�ļ�
	void WriteTempFile(LPCTSTR lpszFile,void* pBuf,int iSize);
protected:
	// ��Ϣ�ɷֽ�� begin
	void OnRetGetDevListRet(void* pData,int iLen);		// �����豸�б�Ӧ��
	void OnRetGetDevListFile(void* pData,int iLen);		// �豸�б��ļ�����
	void OnRetGetGPSInfo(void* pData,int iLen);			// ��������������
	void OnRetDispGPSInfo(void* pData,int iLen);		// �������·�(BF)
	void OnRetRecQuery(void* pData,int iLen);			// ��ѯӦ��(��������)
	void OnRetRecListFile(void* pData,int iLen);		// ¼���ѯ�ļ����ݴ���
	void OnRetRecDownload(void* pData,int iLen);		// ����¼���ļ�Ӧ��
	void OnRetRecDownloadFile(void* pData,int iLen);	// ¼���ļ����ص�
	void OnRetAlarmDetail(void* pData,int iLen);		// �����·�(��ϸ����)
	void OnRetGetAlarm(void* pData,int iLen);			// ���ı�������
	void OnRetAlarmSummary(void* pData,int iLen);		// �����·�
	void OnRetRemoveAlarm(void* pData,int iLen);		// ����
	void OnRetTalkData(void* pData,int iLen);			// �ϴ��Խ���
	void OnRetTalkErr(void* pData,int iLen);			// �Խ��쳣
	void OnRetTalkAnswer(void* pData,int iLen);			// �Խ������Ӧ
	void OnRetGetServerListRet(void* pData,int iLen);	// ����������б�Ӧ��
	void OnRetGetServerListFile(void* pData,int iLen);	// �������б��ļ�����
	// ��Ϣ�ɷֽ�� end
public:
protected:
	int					m_iTaskID;						// ��ǰ����ID
	LPBYTE				m_pbyBufFile;					// �ļ�����(XML)
	int					m_iBufFileMax;					// �ļ������С
	int					m_iBufFileLen;					// �ļ���С
	eJNVFileType		m_eFileType;					// �ļ�����

	char				m_szDevListPath[MAX_PATH];		// �豸�б�·��
	char				m_szRecListPath[MAX_PATH];		// ¼���б�·��
	char				m_szRecDownloadPath[MAX_PATH];	// ¼������·��
	char				m_szServerListPath[MAX_PATH];	// �������б�·��

	stJNVTalkReqSingle	m_stTalkReq;
	bool				m_bCanSend;						// �Ƿ���Է�����

public:
	static  CJNVStreamTcp		m_arStream[JNET_MAX_STREAM];	// ��ͨ��
	static	CJNVTalkTcp			m_arTalk[JNET_MAX_STREAM];		// �Խ�ͨ��
};

#endif // !defined(AFX_JNVCMDTCP_H__01E4C994_978C_4F4A_BB2F_C1038E5CB61E__INCLUDED_)
