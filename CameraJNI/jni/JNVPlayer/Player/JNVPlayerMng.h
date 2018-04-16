// JNVPlayerMng.h: interface for the CJNVPlayerMng class.
//
//////////////////////////////////////////////////////////////////////

#if !defined(AFX_JNVPLAYERMNG_H__1ED9B187_1547_4669_B376_CAECA9635D3D__INCLUDED_)
#define AFX_JNVPLAYERMNG_H__1ED9B187_1547_4669_B376_CAECA9635D3D__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

#include "JNVPlayer.h"

class CJNVPlayerMng;		typedef class CJNVPlayerMng*	LPJNVPlayerMng;
class CJNVPlayerMng  
{
public:
	CJNVPlayerMng();
	virtual ~CJNVPlayerMng();

public:
	// �˿ڹ��� begin
	long	Init(int iPlayerSize);					// ��ʼ���˿ڹ���
	long	Clean();								// ����
	long	CreatePort();							// ����������
	long	ReleasePort(int iPort);					// �ͷŲ�����
	LPJNVPlayer GetPlayer(int iPort);				// ��ȡ������
	// �˿ڹ��� end

	// ������� begin
	// ��¼��� begin
	long Login(LPCTSTR lpszIP,int iDevPort,LPCTSTR lpszUser,LPCTSTR lpszPassword,int iTimeOut);
	long Logout();
	// ��¼��� end

	// �����豸�б�:0,��ʾ�ɹ�;����,��ʾʧ��
	long GetDevList(LPCTSTR lpszFilePath);

	// ����������б�:0,��ʾ�ɹ�;����,��ʾʧ��
	long GetServerList(LPCTSTR lpszFilePath, int nServerType);

	// �������ӷ���������:0,��ʾ�ɹ�;
	long SetConnectServerType(int nServerType);

	// ���ӷ�������Ϣ:0,��ʾ�ɹ�;
	long AddServerInfo(LPCTSTR lpszServerId, LPCTSTR lpszServerIp, int nServerPort, int nServerType);

	// ��ȡ���������� 
	// lpszDevID   Ϊ�ⲿ������� ��ʽΪ�豸ID#���ķ�����ID#���������ID#ý�������ID
	// nServerType ���������ͣ�0 ���� 1���� 2ý�壩
	// szDevID     ������ʵ�ʵ��豸id
	LPJNVCmdTcp GetCasTcp(LPCTSTR lpszDevID, int nServerType, char* szDevID);

	// ���ӷ�����
	void AddServer();

	// ������� end

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
	
	// ��̨���� begin
	long PtzCtrl(LPCTSTR lpszDevID,int iChn,int iAction,int iValue1,int iValue2);
	// ��̨���� end

	// ʵʱ��Ƶ��� begin
	// ��ʼʵʱ��Ƶ��,�豸ID,ͨ�����(1��ʼ),�������(����),��������(����)
	long RealStreamStart(LPCTSTR lpszDevID,int iChn,int iStream,int iType);
	long RealStreamReqIFrame(long lStream);
	long RealStreamStop(long lStream);
	// ʵʱ��Ƶ��� end

	// �豸¼��ط���� begin
	long ReplayStart(LPCTSTR lpszDevID,int iCenter,int iChn,LPCTSTR lpszTimeStart,LPCTSTR lpszTimeEnd,int iOnlyIFrame,int iRecType,int iStartPos);
	long ReplayCtrl(long lStream,int iCtrlType,int iValue);
	long ReplayStop(long lStream);
	// �豸¼��ط���� end

	// ������� begin
	long Capture(long lStream,LPCTSTR lpszPath);	// ץͼ
	long RecStart(long lStream,LPCTSTR lpszPath);	// ��ʼ¼��
	long RecStop(long lStream);						// ֹͣ¼��
	// ������� end

	// ¼����Ƶ��� begin
	long RecOpenFile(LPCTSTR lpszPath);				// ��¼���ļ�
	long RecCloseFile(long lRecFile);				// �ر�¼���ļ�
	long RecPlayStart(long lRecFile);				// ��ʼ/���� ����
	long RecPlayPause(long lRecFile);				// ��ͣ����
	long RecPlaySetSpeed(long lRecFile,int iSpeed);	// ���ò����ٶ�
	long RecPlayGetSpeed(long lRecFile);			// ��ȡ�����ٶ�
	long RecPlayNextFrame(long lRecFile);			// ��һ֡
	long RecPlaySeek(long lRecFile,long lTime);		// ���ò����ٶ�
	long RecPlayCapture(long lRecFile,LPCTSTR lpszPath);	// ¼��ץ��
	// ¼����Ƶ��� end

protected:
	static int __stdcall OnStream(long lHandle, LPBYTE pBuff,DWORD dwRevLen,void* pUserParam);
	static int __stdcall OnConnEvent(long lHandle, eJNetEvent eType,int iDataType,void* pEventData,int iDataLen,void* pUserParam);	
	static int __stdcall OnDecFrame(PSTDecFrameCB pstDecFrameCB);
	static int __stdcall OnPlayControl(AVPPlayCBMsg eMsg,int iPlayerID,long lpUserParam,__int64 lParam1,__int64 lParam2,DWORD dwRes);
public:
	fcbDecFrame*	m_pfDecFunNet;					// ����ص�����
	fcbDecFrame*	m_pfDecFunLocal;				// ����ص�����
	fcbJEvent*		m_pfEventFun;					// �¼��ص�����
	fcbPlay*		m_pfPlayInfoFun;				// �����¼��ص�����
	int				m_iImgRGBFM;					// �����ͼ��RGB��ʽ
protected:
	LPJNVPlayer		m_arPlayer;						// ����������
	int				m_iPlayerSize;					// ����������
	CJNVCmdTcp		m_tcpCmd;						// ��������
	CDecoder		m_decAudio;						// ��Ƶ����

	int					m_nConnectServerType;			// ���ӷ��������ͣ�0 �����������棬 1 ��������������
	//LPJNVCmdTcp			m_arCasTcp[JNET_MAX_STREAM];	// �豸����ͨ��������ģʽ��
	stJNVServerInfo		m_arServerInfo[MAX_SERVER_NUM];	// ������


	CMutexLock		m_mutexItemServer;				// ���ӷ�������
	bool			m_bThreadExit;					// �Ƿ��˳�
	pthread_t		m_hThreadID;					// �����߳�ID
};

#endif // !defined(AFX_JNVPLAYERMNG_H__1ED9B187_1547_4669_B376_CAECA9635D3D__INCLUDED_)
