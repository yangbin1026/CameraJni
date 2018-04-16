// JBuffPool.h: interface for the CJBuffPool class.
//
//////////////////////////////////////////////////////////////////////

#if !defined(AFX_JBUFFPOOL_H__9199E105_4894_42AE_AE02_FBD3D95E6BDA__INCLUDED_)
#define AFX_JBUFFPOOL_H__9199E105_4894_42AE_AE02_FBD3D95E6BDA__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

#include "j_vs_ver.h"
#include "JLSocketDef.h"
#include "JLogWriter.h"

typedef class stBPElement
{
public:
	stBPElement();
	stBPElement(const void* pBuf,int iSize,long lRealID,DWORD dwData);
	const stBPElement& operator =( const stBPElement& stSrc );
	~stBPElement();
	void Free();
	void Set(int iSize);

	BYTE*	m_pBuf;				// ����
	int		m_iBufSize;			// �����С
	int		m_iMaxSize;			// �����С
	long	m_lRSID;			// ʵʱ��ID
	bool	m_bIsNew;			// �Ƿ�Ϊ�µ�
	DWORD	m_dwData;			// �û�����
}* PSTBPElement;

class CJBuffPool  
{
public:
	CJBuffPool();
	virtual ~CJBuffPool();

	void		SetUserID(long lUserID);
	void		StopAdd();
	void		Clear();
	void		Reset();
	int			SetSize(int iBufSize,int iCanRead,int iEleSize);
	int			SetCanRead(int iCanRead);
	PSTBPElement BeginAddBuff(const void* pBuf,int iSize,long lRealID,DWORD dwData);
	void		EndAddBuff();
	PSTBPElement BeginGetBuff();
	void		EndGetBuff();
	int			GetSpan();
	int			GetSize();

protected:
	long					m_lUserID;				// �û�ID
	PSTBPElement*			m_arBuf;				// Ԫ������
	long					m_lHasRecv;				// ��д����
	long					m_lHasRead;				// �Ѷ�����
	int						m_iRecvPos;				// ����λ��
	int						m_iReadPos;				// ��ȡλ��
	int						m_iLastRecvPos;			// ��һ����λ��(������)
	int						m_iLastReadPos;			// ��һ��ȡλ��(������)
	int						m_iCanRead;				// �ɶ�������
	int						m_iCurStep;				// ��ǰ����
	int						m_iBuffSize;			// �ش�С
	int						m_iEleSize;				// Ԫ�ش�С
	CMutexLock				m_mutexBuff;			// �������

	bool					m_bCanRead;				// �Ƿ�ɶ�
//	bool					m_bCanWrite;			// �Ƿ��д
	bool					m_bEndAdd;				// �Ƿ����д��
};

#endif // !defined(AFX_JBUFFPOOL_H__9199E105_4894_42AE_AE02_FBD3D95E6BDA__INCLUDED_)
