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
	// �Խ���� begin
	// ��ʼ�Խ�����,Э���豸ָ��(����),�豸ID,ͨ�����(1��ʼ)
	long TalkStart(LPJNVProtocolTCP lpProtocol,LPCTSTR lpszDevID,int iChn);
	long TalkSendData(BYTE* lpData,int iLen);
	long TalkStop();
	// �Խ���� end

protected:
	// ֪ͨ���� begin
	// ������չ�Ľ��մ���
	virtual int OnExtRecv(int nType,void* lpBuf,int iDataLen);
	// ��¼״̬
	virtual int OnConnect(int iRetType);
	// ֪ͨ���� end
protected:
	// ��Ϣ�ɷֽ�� begin
	void OnRetTalkData(void* pData,int iLen);		// �ϴ��Խ���
	void OnRetTalkErr(void* pData,int iLen);		// �Խ��쳣
	void OnRetTalkAnswer(void* pData,int iLen);		// �Խ������Ӧ
	// ��Ϣ�ɷֽ�� end
	
public:
protected:
	LPJNVProtocolTCP	m_lpProtocol;				// Э���豸
	char				m_szDevID[JNET_DEV_ID_LEN];	// �豸ID
	int					m_iChn;						// ������ͨ����

	stJNVTalkReqSingle	m_stTalkReq;				// �Խ����������
	bool				m_bCanSend;					// �Ƿ���Է�����

	FILE*				m_pfWrite;					// д���ļ�
};

#endif // !defined(AFX_JNVTALKTCP_H__9FB98A92_0867_46A4_895D_3F843286B750__INCLUDED_)
