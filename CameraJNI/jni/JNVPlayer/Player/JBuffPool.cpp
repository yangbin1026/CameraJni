// JBuffPool.cpp: implementation of the CJBuffPool class.
//
//////////////////////////////////////////////////////////////////////

#include "JBuffPool.h"

//缓存池

stBPElement::stBPElement()
{
	m_pBuf		= NULL;			// 缓存
	m_iBufSize	= 0;			// 缓存大小
	m_lRSID		= 0;			// 实时流ID
	m_bIsNew	= true;			// 是否为新的
	m_dwData	= 0;			// 用户数据
}

stBPElement::stBPElement(const void* pBuf,int iSize,long lRealID,DWORD dwData)
{
	if (iSize<=0)
	{
		m_iBufSize	= 0;
		m_iMaxSize	= 0;
		m_pBuf		= NULL;
	}
	else
	{
		m_pBuf	= new BYTE[iSize];
		if ( m_pBuf )
		{
			m_iBufSize	= iSize;
			m_iMaxSize	= iSize;
			if ( pBuf ) jmemcpy(m_pBuf,pBuf,iSize);
		}
	}

	m_lRSID		= lRealID;		// 实时流ID
	m_bIsNew	= true;			// 是否为新的
	m_dwData	= dwData;		// 用户数据
}

const stBPElement& stBPElement::operator =( const stBPElement& stSrc )
{
	stBPElement*	pThis = this;

	if ( m_iBufSize<stSrc.m_iBufSize )
	{
		Set(stSrc.m_iBufSize);
	}
	
	m_lRSID		= stSrc.m_lRSID;				// 实时流ID
	m_bIsNew	= stSrc.m_bIsNew;				// 是否为新的
	m_dwData	= stSrc.m_dwData;				// 用户数据
	jmemcpy(m_pBuf,stSrc.m_pBuf,stSrc.m_iBufSize);		// 缓存

	return *pThis;
}

stBPElement::~stBPElement()
{
	Free();
}

void stBPElement::Free()
{
	SAFE_DELETE(m_pBuf);		// 缓存
	m_iBufSize	= 0;			// 缓存大小
	m_iMaxSize	= 0;			// 缓存大小
	m_lRSID		= 0;			// 实时流ID
	m_dwData	= 0;			// 用户数据
}

void stBPElement::Set(int iSize)
{
	Free();
	if ( iSize>0 )
	{
		m_iBufSize	= iSize;			// 缓存大小
		m_pBuf		= new BYTE[iSize];
		m_iMaxSize	= iSize;			// 缓存大小
	}
}

//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////

CJBuffPool::CJBuffPool()
{
	m_lUserID		= 0;				// 用户ID
	m_arBuf			= NULL;				// 元素数组

	m_lHasRecv		= 0;				// 已写入数
	m_lHasRead		= 0;				// 已读出数
	m_iRecvPos		= -1;				// 接收位置
	m_iReadPos		= -1;				// 读取位置
	
	m_mutexBuff.CreateMutex();
//	m_mutexVar.CreateMutex(NULL,FALSE,NULL,"CJBuffPool:m_mutexVar");
//	m_eventRead.CreateEvent(NULL,FALSE,FALSE,NULL,"CJBuffPool:m_eventRead");

	m_bCanRead		= false;			// 是否可读
//	m_bCanWrite		= false;			// 是否可写
	m_bEndAdd		= false;			// 是否结束写入
	m_iBuffSize		= 0;				// 池大小
	
	m_iCanRead		= 0;				// 可读数设置
	m_iCurStep		= 0;				// 当前步骤
//	SetSize(60,2);
}

CJBuffPool::~CJBuffPool()
{
	Clear();

	return;
}

void CJBuffPool::SetUserID(long lUserID)
{
	m_lUserID	= lUserID;
}

void CJBuffPool::StopAdd()
{
	JLOG_TRY
	m_mutexBuff.Lock();
	m_bEndAdd		= true;			// 是否结束写入
	m_bCanRead		= false;
	m_iRecvPos		= -1;			// 接收位置
	m_iReadPos		= -1;			// 读取位置
//	m_eventRead.SetEvent();
	m_mutexBuff.Unlock();
	JLOG_CATCH("try-catch CJBuffPool::StopAdd\r\n")
}

void CJBuffPool::Clear()
{
	JLOG_TRY
	int i = 0;
	PSTBPElement	pstBPEle	= NULL;
	
	m_mutexBuff.Lock();
	m_iRecvPos	= -1;				// 接收位置
	m_iReadPos	= -1;				// 读取位置
	m_lHasRecv	= 0;				// 已写入数
	m_lHasRead	= 0;				// 已读出数
	m_bCanRead	= true;				// 是否可读
//	m_bCanWrite	= false;			// 是否可写
	m_bEndAdd	= true;				// 是否结束写入
//	m_eventRead.SetEvent();
//	m_eventWrite.SetEvent();
	m_iCanRead	= 0;				// 可读数设置
	m_iCurStep	= 0;				// 当前步骤
	for (i=0;i<m_iBuffSize;i++)
	{
		pstBPEle = m_arBuf[i];
		SAFE_DELETE(pstBPEle);
	}
	SAFE_DELETE_A(m_arBuf);
	m_iBuffSize = 0;
	m_mutexBuff.Unlock();
	JLOG_CATCH("try-catch CJBuffPool::Clear\r\n")
}

void CJBuffPool::Reset()
{
	JLOG_TRY
//	m_mutexBuff.Lock();
	m_iRecvPos	= 0;
	m_iReadPos	= -1;
	m_lHasRecv	= 0;				// 已写入数
	m_lHasRead	= 0;				// 已读出数
//	m_bCanWrite	= true;				// 是否可写
	m_bEndAdd	= false;			// 是否结束写入
//	m_eventRead.ResetEvent();
	m_bCanRead	= false;			// 是否可读
//	m_eventWrite.ResetEvent();
//	m_iCanRead	= 0;				// 可读数设置
	m_iCurStep	= 0;				// 当前步骤
//	m_mutexBuff.Unlock();
	JLOG_CATCH("try-catch CJBuffPool::Reset\r\n")
}

int	CJBuffPool::SetSize(int iBufSize,int iCanRead,int iEleSize)
{
	int i = 0;
	JLOG_TRY
	m_mutexBuff.Lock();
	Clear();
	m_arBuf		= new PSTBPElement[iBufSize];
	m_iBuffSize	= iBufSize;	// 池大小
	m_iEleSize	= iEleSize;			// 元素大小
	m_iCanRead	= iCanRead;
	m_iCurStep	= 0;				// 当前步骤
	Reset();
	// 初始化 begin
	for (i=0;i<m_iBuffSize;i++)
	{
		m_arBuf[i]	= new stBPElement(NULL,iEleSize,0,0);
	}
	// 初始化 end
	m_mutexBuff.Unlock();

	return m_iBuffSize;
	JLOG_CATCH("try-catch CJBuffPool::SetSize\r\n")
	return 0;
}

int CJBuffPool::SetCanRead(int iCanRead)
{
	if ( m_iCanRead==iCanRead ) return 0;
	if (m_mutexBuff.Lock()==false)
	{
		return -1;
	}

	if ( m_iCanRead<iCanRead )
	{
		m_iCanRead	= iCanRead;
		// 设置的可读数大,需等待
		m_iCurStep	= 0;
		m_bCanRead	= false;
	}
	else
	{
		// 设置的可读数小,立即开始即可
	//	Reset();
		m_iCanRead	= iCanRead;
		m_bCanRead	= true;
	}
	m_mutexBuff.Unlock();

	return 0;
}

PSTBPElement CJBuffPool::BeginAddBuff(const void* pBuf,int iSize,long lRealID,DWORD dwData)
{
	JLOG_TRY
	PSTBPElement	pstBPEle	= NULL;
//	CJGetRunTime	jgrt;

//	jgrt.Start();
	if (m_mutexBuff.Lock()==false)
	{
	//	m_mutexBuff.Unlock();
		DbgStrOut("BeginAddBuff m_mutexBuff.Lock WAIT_TIMEOUT,%ld\r\n",m_lUserID);
		return NULL;
	}
	if ( m_bEndAdd )
	{
		m_mutexBuff.Unlock();
		DbgStrOut("BeginAddBuff m_bEndAdd is TRUE,%ld\r\n",m_lUserID);
		return NULL;
	}
	if ( m_iRecvPos==-1 )
	{
		m_mutexBuff.Unlock();
		DbgStrOut("BeginAddBuff m_iRecvPos=-1,%ld\r\n",m_lUserID);
		return NULL;
	}
//	if ( m_iReadPos<0 )	m_iReadPos = 0;
//	if ( m_iRecvPos==m_iBuffSize ) m_iRecvPos=0;
	// 写数据 begin
	pstBPEle = m_arBuf[m_iRecvPos];
	if ( pstBPEle==NULL || pstBPEle->m_iMaxSize<iSize )
	{
		if ( pstBPEle )
		{
			pstBPEle->Set(iSize);
		}
		else
		{
			pstBPEle = new stBPElement(pBuf,iSize,lRealID,dwData);
			if ( pstBPEle==NULL )
			{
				m_arBuf[m_iRecvPos] = NULL;
				m_mutexBuff.Unlock();
				DbgStrOut("BeginAddBuff new stBPElement error,%ld\r\n",m_lUserID);
				return NULL;
			}
			m_arBuf[m_iRecvPos] = pstBPEle;
		}
	}
	pstBPEle->m_lRSID	= lRealID;
	pstBPEle->m_dwData	= dwData;
	pstBPEle->m_iBufSize= iSize;
	jmemcpy(pstBPEle->m_pBuf,pBuf,iSize);
	pstBPEle->m_bIsNew	= true;
	// 写数据 end
//	jgrt.End();
//	fTemp = jgrt.GetTimeSpan();
//	JTRACE("[%ld] CJBuffPool::BeginAddBuff Span [%.2f] ms\r\n",m_lUserID,fTemp);
//	if ( fTemp>30.0f) DbgStrOut("[%ld] CJBuffPool::BeginAddBuff Span [%.2f] ms\r\n",m_lUserID,fTemp);
	return pstBPEle;
	JLOG_CATCH("try-catch CJBuffPool::BeginAddBuff\r\n")
	return NULL;
}

void CJBuffPool::EndAddBuff()
{
	JLOG_TRY
//	CJGetRunTimeTC	jgrt;

	if ( m_bEndAdd )
	{
		DbgStrOut("EndAddBuff m_bEndAdd is TRUE,%ld\r\n",m_lUserID);
		return;
	}

//	jgrt.Start();
	// 后移计数器 begin
	m_iRecvPos++;
	m_lHasRecv++;				// 已写入数
	if ( m_iReadPos<0 )	m_iReadPos = 0;
	if ( m_iRecvPos==m_iBuffSize )
	{
		m_iRecvPos=0;
	}
	if ( GetSpan()>=m_iBuffSize )
	{
		// 爆了,怎么办?目前是Reset
		DbgStrOut("CJBuffPool::%ld Buff Bom,span=%d,m_lHasRecv=%ld,m_lHasRead=%ld.\r\n",
			m_lUserID,GetSpan(),m_lHasRecv,m_lHasRead);
		Reset();
	}
//	else
//	{
//		if ( m_lHasRecv%60==0 )
//		{
//			DbgStrOut("CJBuffPool::%ld,span=%d,m_lHasRecv=%ld,m_lHasRead=%ld.\r\n",m_lUserID,GetSpan(),m_lHasRecv,m_lHasRead);
//		}
//	}
	m_mutexBuff.Unlock();
	// 后移计数器 end
//	jgrt.End();
//	fTemp = jgrt.GetTimeSpan();
//	if ( fTemp>30.0f) DbgStrOut("[%ld] CJBuffPool::EndAddBuff Span [%.2f] ms\r\n",m_lUserID,fTemp);
	JLOG_CATCH("try-catch CJBuffPool::EndAddBuff\r\n")
	return;
}

PSTBPElement CJBuffPool::BeginGetBuff()
{
	JLOG_TRY
	PSTBPElement	pstBPEle	= NULL;
//	CJGetRunTimeTC	jgrt;

//	jgrt.Start();
	if (m_mutexBuff.Lock()==false)
	{
	//	m_mutexBuff.Unlock();
		DbgStrOut("BeginGetBuff m_mutexBuff.Lock WAIT_TIMEOUT,%ld\r\n",m_lUserID);
		return NULL;
	}
	if ( m_bEndAdd )
	{
		m_mutexBuff.Unlock();
		DbgStrOut("BeginGetBuff m_bEndAdd is TRUE,%ld\r\n",m_lUserID);
		return NULL;
	}

	// 判断是否可读了 begin
	switch(m_iCurStep)
	{
	case 0:
		if ( m_lHasRecv-m_lHasRead<m_iCanRead )
		{
			m_iCurStep	= 1;
			m_mutexBuff.Unlock();
			return NULL;
		}
		break;
	case 1:
	//	if ( m_lHasRecv-m_lHasRead>m_iCanRead )
		if ( m_lHasRecv<=m_lHasRead )
		{
			m_iCurStep	= 0;
			m_mutexBuff.Unlock();
			return NULL;
		}
		break;
	}
	// 判断是否可读了 end

	if ( m_iReadPos<0 )
	{
		m_mutexBuff.Unlock();
		DbgStrOut("BeginGetBuff m_iReadPos=-1,%ld\r\n",m_lUserID);
		return NULL;
	}

	pstBPEle = m_arBuf[m_iReadPos];
	if ( pstBPEle==NULL )
	{
		m_mutexBuff.Unlock();
		DbgStrOut("BeginGetBuff pstBPEle=NULL,%ld\r\n",m_lUserID);
		return NULL;
	}
	pstBPEle->m_bIsNew	= false;
//	jgrt.End();
//	fTemp = jgrt.GetTimeSpan();
//	if ( fTemp>30.0f) DbgStrOut("[%x] CJBuffPool::BeginGetBuff Span [%.2f] ms\r\n",this,fTemp);

	return pstBPEle;
	JLOG_CATCH("try-catch CJBuffPool::BeginGetBuff\r\n")
	return NULL;
}

void CJBuffPool::EndGetBuff()
{
	JLOG_TRY
//	CJGetRunTimeTC	jgrt;
//	PSTBPElement	pstBPEle= NULL;

	if ( m_bEndAdd )
	{
		DbgStrOut("EndGetBuff m_bEndAdd is TRUE,%ld\r\n",m_lUserID);
		return;
	}

	/*
	pstBPEle = m_arBuf[m_iReadPos];
	if ( pstBPEle && pstBPEle->m_iMaxSize>m_iEleSize )
	{
		JTRACE("CJBuffPool::EndGetBuff() free :%ld\r\n",pstBPEle->m_iMaxSize);
		pstBPEle->Free();
	}
	*/

//	jgrt.Start();
	// 后移计数器 begin
	m_lHasRead++;
	m_iReadPos++;
	if ( m_iReadPos==m_iBuffSize ) m_iReadPos=0;

	m_mutexBuff.Unlock();
	// 后移计数器 end
//	jgrt.End();
//	fTemp = jgrt.GetTimeSpan();
//	if ( fTemp>30.0f) DbgStrOut("[%ld] CJBuffPool::EndGetBuff Span [%.2f] ms\r\n",m_lUserID,fTemp);
	JLOG_CATCH("try-catch CJBuffPool::EndGetBuff\r\n")
	return;
}

int CJBuffPool::GetSpan()
{
	JLOG_TRY
	return m_lHasRecv-m_lHasRead;
	JLOG_CATCH("try-catch CJBuffPool::GetSpan\r\n")
	return 0;
}

int CJBuffPool::GetSize()
{
	JLOG_TRY
	return m_iBuffSize;
	JLOG_CATCH("try-catch CJBuffPool::GetSize\r\n")
	return 0;
}
