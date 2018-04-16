// JNVProtocolTCP.h: interface for the CJNVProtocolTCP class.
//
//////////////////////////////////////////////////////////////////////

#if !defined(AFX_JNVPROTOCOLTCP_H__94661FDA_0EE7_4575_8998_30D74A15F37D__INCLUDED_)
#define AFX_JNVPROTOCOLTCP_H__94661FDA_0EE7_4575_8998_30D74A15F37D__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

#include "JLTcpClient.h"
#include "ProtocolNV.h"

#undef DbgStrOut
#define DbgStrOut	JTRACE

#define JCONN_JNV_RCV_BUF		(1024*128)		// 128K
#define JCONN_JNV_SND_BUF		(1024*128)		// 128K

class CJNVProtocolTCP;	typedef CJNVProtocolTCP*		LPJNVProtocolTCP;

class CJNVProtocolTCP : public CJLTcpClient  
{
public:
	CJNVProtocolTCP();
	virtual ~CJNVProtocolTCP();

public:
	// ��¼��� begin
	long Login(LPCTSTR lpszIP,int iDevPort,LPCTSTR lpszUser,LPCTSTR lpszPassword,int iTimeOut,fcbJEvent* pfcbEvent,void* pUserParam);
	long Logout();
	// ��¼��� end

protected:
	// ֪ͨ���� begin
	// �������ӵ�ָ����CMS��
	virtual int Connect();
	// ��鱾�����״̬,����0��ʾ OnCheckSocket ����,�����˳�����߳�
	virtual int OnCheckState(DWORD dwTick);
	// �ͻ�����Ϣ����
	virtual int OnClientMsg(void* pParam);
	// ��¼״̬
	virtual int OnConnect(int iRetType);
	// ������չ�Ľ��մ���
	virtual int OnExtRecv(int nType,void* lpBuf,int iDataLen);
	// ֪ͨ���� end

protected:
	// ���ͻص���Ϣ
	long SendEvent(eJNetEvent eType,int iDataType,void* pEventData,int iDataLen);
	// ����������Ϣ
	long CopyConn(LPJNVProtocolTCP lpProtocol);

	// ���ڲ������麯�� begin
	virtual long OnSendLogin();
	virtual long OnSendLogout();
	virtual long OnSendBreath(DWORD dwTick);
	// ���ڲ������麯�� end

	// ��Ϣ�ɷֽ�� begin
	virtual void OnRetLogin(void* pData,int iLen);	// ��¼Ӧ��
	virtual void OnRetBreath(void* pData,int iLen);	// ����Ӧ��
	// ��Ϣ�ɷֽ�� end

public:
protected:
	fcbJEvent*		m_pfcbEvent;					// �¼�֪ͨ����
	void*			m_pUserParam;					// �û��Զ������

	DWORD			m_dwLastSendBreath;				// ��һ����ʱ��
	DWORD			m_dwLastRecv;					// ��һ����ʱ��
	int				m_iBreathSpan;					// �������

	bool			m_bAutoLogin;					// �Ƿ��Զ���¼
	int				m_iDataOffset;					// ����ƫ��(Ϊ����֡����,��һ�εĿ�������)
	char			m_szDevIP[JNET_IP_LEN];			// �豸IP
	int				m_iDevPort;						// �豸�˿�
	int				m_iStreamPort;					// ���˿�
	char			m_szUser[JNET_NAME_LEN*2];		// �û���
	char			m_szPwd[JNET_PWD_LEN*2];			// ����
	
	PJHANDLE		m_hEventSync;					// ͬ���¼�
	
	stJNVProHead	m_stLastRecv;					// �ϴν���
	DWORD			m_dwSendSeq;					// ����ͬ������
	void*			m_lpSendData;					// ����ͬ������
	int				m_iSendDataLen;					// ����ͬ������
	
	stJNVC2SLoginRet	m_stLoginInfo;				// ��¼��Ϣ

public:
	int				m_nServerType;					// ����������
};
#endif // !defined(AFX_JNVPROTOCOLTCP_H__94661FDA_0EE7_4575_8998_30D74A15F37D__INCLUDED_)
