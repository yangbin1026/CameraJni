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

	BYTE*	m_pBuf;				// 缓存
	int		m_iBufSize;			// 缓存大小
	int		m_iMaxSize;			// 缓存大小
	long	m_lRSID;			// 实时流ID
	bool	m_bIsNew;			// 是否为新的
	DWORD	m_dwData;			// 用户数据
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
	long					m_lUserID;				// 用户ID
	PSTBPElement*			m_arBuf;				// 元素数组
	long					m_lHasRecv;				// 已写入数
	long					m_lHasRead;				// 已读出数
	int						m_iRecvPos;				// 接收位置
	int						m_iReadPos;				// 读取位置
	int						m_iLastRecvPos;			// 上一接收位置(调试用)
	int						m_iLastReadPos;			// 上一读取位置(调试用)
	int						m_iCanRead;				// 可读数设置
	int						m_iCurStep;				// 当前步骤
	int						m_iBuffSize;			// 池大小
	int						m_iEleSize;				// 元素大小
	CMutexLock				m_mutexBuff;			// 互斥变量

	bool					m_bCanRead;				// 是否可读
//	bool					m_bCanWrite;			// 是否可写
	bool					m_bEndAdd;				// 是否结束写入
};

#endif // !defined(AFX_JBUFFPOOL_H__9199E105_4894_42AE_AE02_FBD3D95E6BDA__INCLUDED_)
