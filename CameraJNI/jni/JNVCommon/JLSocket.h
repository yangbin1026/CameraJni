// JLSocket.h: interface for the CJLSocket class.
//
//////////////////////////////////////////////////////////////////////

#if !defined(AFX_JLSOCKET_H__E3030ACE_A643_4A68_A320_FBB1FA03143A__INCLUDED_)
#define AFX_JLSOCKET_H__E3030ACE_A643_4A68_A320_FBB1FA03143A__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

#include "j_vs_ver.h"
#include "JLogWriter.h"
#include "JLSocketDef.h"

#define		JLSOCK_SUCCESS			0						// �����ɹ�
#define		JLSOCK_ERR_PARAM		0x000A0001				// ������Χ����
#define		JLSOCK_ERR_PASTE		0x000A0002				// ���ƾ��ʧ��
#define		JLSOCK_ERR_INIT			0x000A0003				// ��ʼ��ʧ��
#define		JLSOCK_ERR_CLOSE		0x000A0004				// �ر�ʧ��
#define		JLSOCK_ERR_CONNECT		0x000A0005				// ����ʧ��
#define		JLSOCK_ERR_BIND			0x000A0006				// �𶨶˿�ʧ��
#define		JLSOCK_ERR_LISTEN		0x000A0007				// ���ü���ʧ��
#define		JLSOCK_ERR_ACCEPT		0x000A0008				// ACCEPTʧ��
#define		JLSOCK_ERR_CLI_EXIT		0x000A0009				// ����ͻ����뿪ʱʧ��
#define		JLSOCK_ERR_SEND			0x000A0010				// ������Ϣʧ��
#define		JLSOCK_ERR_RECV			0x000A0011				// ������Ϣʧ��
#define		JLSOCK_ERR_PEER			0x000A0012				// ��ȡԶ�˵�ַ����
#define		JLSOCK_ERR_NOBLOCK		0x000A0013				// ����Ϊ��������ʽʱ����
#define		JLSOCK_ERR_MEM			0x000A0014				// �ڴ治�����
#define		JLSOCK_ERR_TIMEOUT		0x000A0015				// ��ʱ����
#define		JLSOCK_ERR_OPERATE		0x000A0016				// ��������
#define		JLSOCK_ERR_ACCEPT_TH	0x000A1001				// ���� ACCEPT �߳�ʧ��
#define		JLSOCK_ERR_RECV_TH		0x000A1002				// ���� RECV �߳�ʧ��

#define		JSS_CAN_READ			0x0001					// SOCK�ɶ�
#define		JSS_CAN_WRITE			0x0002					// SOCK��д

#define		JSS_SO_T_CONN			0						// ���Ӳ���
#define		JSS_SO_T_SEND			1						// ���Ͳ���
#define		JSS_SO_T_RECV			2						// ���ղ���


namespace JSocketCtrl
{
class CJLSocket  
{
public:
	CJLSocket(SOCKET hSocket=INVALID_SOCKET);
	virtual ~CJLSocket();

	virtual int Close();

	// ���õ�fcntl begin
	// ����Ϊ��������ʽ
	int SetNoBlock(bool bNoBlock);
	bool IsNoBlock();
	int SetSoBuf(int iType,int iSize);				// iType:�� JSS_SO_T_XXX
	int SetTimeOut(int iType,int iMilliseconds);	// iType:�� JSS_SO_T_XXX
	// ���õ�fcntl end

	// �������� begin
	// ��ȡ��Ӧ�ľ��
	SOCKET GetHandle();
	// ��ȡԶ�˵�ַ(�Է�������ʽ������)
	int	GetPeerAddr(sockaddr_in* addr,char* szStr=NULL);
	// ��ȡ���ص�ַ
	int	GetLocalAddr(sockaddr_in* addr,char* szStr=NULL);
	// �������� end

	// ������� begin
	// ���� byBuf �е���Ϣ,���ͳ���Ϊ iBufLen ,����ʵ�ʷ��͵ĳ���(����TCP)
	virtual int Send(const void* byBuf,int iBufLen);
	// ������Ϣ�� byBuf ��,���ճ���Ϊ iBufLen ,����ʵ�ʽ��յĳ���(����TCP)
	virtual int Recv(void* byBuf,int iRecvLen);
	virtual int RecvNoBlock(void* byBuf,int iRecvLen);
	// ���� byBuf �е���Ϣ,���ͳ���Ϊ iBufLen ,����ʵ�ʷ��͵ĳ���(����UDP)
	virtual int SendTo(const void* byBuf,int iBufLen,const char* szIP,UINT uPort);
	// ������Ϣ�� byBuf ��,���ճ���Ϊ iBufLen ,����ʵ�ʽ��յĳ���(����UDP)
	virtual int RecvFrom(void* byBuf,int iRecvLen,sockaddr_in* addr);
	// ������� end
	
	// ֪ͨ���� begin
	// ���SOCKET��״̬,����ֵ��JSS_CAN_XXXX
	virtual int OnCheckSocket();
	// ��鱾�����״̬,����0��ʾ OnCheckSocket ����,�����˳�����߳�
	virtual int OnCheckState(DWORD dwTick);
	// ��¼�ɹ�,���ص�ֵ��ΪOnCheckSocket�ķ���ֵ
	virtual int OnConnect(int iRetType);
	// �ͻ��˽���
	virtual void* OnClientEnter(SOCKET hClient);
	// �ͻ����뿪
	virtual int OnClientExit(void* pParam);
	// �ͻ�����Ϣ����
	virtual int OnClientMsg(void* pParam);
	// ֪ͨ���� end

	// ��̬���� begin
	// ����SOCKET��Դ
	static int InitSocket();
	// �ر�SOCKET��Դ
	static int CleanSocket();
	// ת��IP�ַ������˿ں�
	static UINT GetIPAndPort(const sockaddr_in* addr,char* szIP);
	// ����sockaddr_in
	static int SetIPAndPort(sockaddr_in* addr,char* szIP,UINT uPort);
	// ��ȡ����IP
	static hostent* GetLocalIP();
	// ��ȡ���еĶ˿�
	static int GetValidSocketPort(bool bUdp);
	// ��������
	static DWORD DomainToIP(const char* szDomain);
	// ��̬���� begin
	
	void	SetCheckCallback(fcbRecvMsg* pcbRecvMsg,fcbCheckState* pcbCheckState,void* lpUserData);
	
	LPBYTE	GetSendBufSize(int iLen);
	LPBYTE	GetRecvBufSize(int iLen);

	// ���ʱ��
	inline bool IsOverTime(DWORD dwLast,DWORD dwCur,DWORD dwMax)
	{
		if ( (long)dwMax>0 && (long)dwLast>0 && (long)(dwCur-dwLast)>(long)(dwMax) ) return true;
		else return false;
	}
protected:
	// ��ʼ��SOCKET���
	virtual int InitHandle(int iType=SOCK_STREAM);
	// ���ݴ���ľ��,�������
	virtual int PasteHandle(SOCKET hSocket);
// ����
public:
	DWORD			m_dwLastRecv;			// �ϴν���ʱ��
protected:
	SOCKET			m_hSocket;				// SOCKET���
	UINT			m_uPort;				// ���ض˿�
	char			m_szAddr[128];			// ��ַ
	bool			m_bNoBlock;				// �Ƿ�Ϊ����ʽ
	bool			m_bConnecting;			// ����������
	int				m_iSockType;			// Socket������

	LONG			m_dwConnTick;			// ��¼��ʱ��
	int				m_iConnTO;				// ���ӳ�ʱ(����)
	int				m_iRecvTO;				// ���ճ�ʱ(����)
	int				m_iSendTO;				// ���ͳ�ʱ(����)

	LPBYTE			m_lpSendBuf;			// ���ͻ���
	int				m_iSendBufLen;			// ���ʹ�С
	LPBYTE			m_lpRecvBuf;			// ���ջ���
	int				m_iRecvBufLen;			// �����С

	fcbCheckState*	m_pcbCheckState;		// ���ص�����
	fcbRecvMsg*		m_pcbRecvMsg;			// ���ջص�����
	void*			m_lpUserData;			// �û��Զ����������

	long			m_lHasRecv;				// �Ѿ�������
};
}
using namespace JSocketCtrl;

#endif // !defined(AFX_JLSOCKET_H__E3030ACE_A643_4A68_A320_FBB1FA03143A__INCLUDED_)
