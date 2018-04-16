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
	// ʵʱ��Ƶ��� begin
	// ��ʼʵʱ��Ƶ��,Э���豸ָ��(����),�豸ID,ͨ�����(1��ʼ),�������(����),��������(����)
	long RealStreamStart(LPJNVProtocolTCP lpProtocol,LPCTSTR lpszDevID,int iChn,int iStream,int iType,fcbJStream* pfcbStream,void* pUserParam);
	long RealStreamReqIFrame();
	long RealStreamStop();
	void* GetStreamParam();
	// ʵʱ��Ƶ��� end
	
	// �豸¼��ط���� begin
	long ReplayStart(LPJNVProtocolTCP lpProtocol,LPCTSTR lpszDevID,int iCenter,int iChn,LPCTSTR lpszTimeStart,LPCTSTR lpszTimeEnd,int iOnlyIFrame,int iRecType,int iStartPos,
		fcbJStream* pfcbStream,void* pUserParam);
	long ReplayCtrl(int iCtrlType,int iValue);
	long ReplayStop();
	// �豸¼��ط���� end
protected:
	// ֪ͨ���� begin
	// ������չ�Ľ��մ���
	virtual int OnExtRecv(int nType,void* lpBuf,int iDataLen);
	// ��¼״̬
	virtual int OnConnect(int iRetType);
	// ֪ͨ���� end
protected:
	// ��Ϣ�ɷֽ�� begin
	void OnRetStreamData(void* pData,int iLen);		// �·���Ƶ��
	void OnRetStreamErr(void* pData,int iLen);		// ������Ƶ�豸���صĴ�����
	void OnRetReplayAnswer(void* pData,int iLen);	// ��¼��ʱ��ط�Ӧ��
	// ��Ϣ�ɷֽ�� end
	
public:
protected:
	int					m_iStreamType;				// ������,�� eJNVStreamType
	LPJNVProtocolTCP	m_lpProtocol;				// Э���豸
	char				m_szDevID[JNET_DEV_ID_LEN];	// �豸ID
	int					m_iChn;						// ������ͨ����

	stJNVRealStreamStart	m_stRealStart;			// ʵʱ������
	stJNVReplayStart	m_stReplayStart;			// ¼��طŲ���

	fcbJStream*			m_pfcbStream;				// ���ص�����
	void*				m_pStreamParam;				// �û��Զ������

	stJNVStreamHeaderFrameInfo	m_stDataHeader;			// ����ͷ
	FILE*				m_pfWrite;					// д���ļ�

	static long			s_lFrameNo;					// ֡��
};

#endif // !defined(AFX_JNVSTREAMTCP_H__50A23179_5478_4F0C_97C0_142E54F3891A__INCLUDED_)
