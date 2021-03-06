// FMReader.cpp: implementation of the CFMReader class.
//
//////////////////////////////////////////////////////////////////////

#include "FMReader.h"
#include "FMReaderJNV.h"

//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////

CFMReader::CFMReader()
{
	m_pBuf			= NULL;				// 缓存
	m_iBufMaxLen	= 0;				// 缓存的最大大小
	m_iBufLen		= 0;				// 缓存大小
	m_bNeedFreeBuf	= false;			// 缓存是否需要被释放
	m_iHeadLen		= 0;				// 包头长
	m_pHeadBuf		= NULL;				// 包头缓存
}

CFMReader::~CFMReader()
{
	if ( m_bNeedFreeBuf )
	{
		SAFE_DELETE(m_pBuf);
		m_bNeedFreeBuf = false;
	}
	m_iBufLen	= 0;
	m_iBufMaxLen= 0;

	// 释放包头缓存 begin
	SAFE_DELETE(m_pHeadBuf);			// 包头缓存
	m_iHeadLen		= 0;				// 包头长
	// 释放包头缓存 end
}

// 设置包头长,只在构造时使用一次
bool CFMReader::SetHeadLen(int iLen)
{
	// 释放包头缓存 begin
	SAFE_DELETE(m_pHeadBuf);			// 包头缓存
	m_iHeadLen		= 0;				// 包头长
	// 释放包头缓存 end
	
	m_pHeadBuf = new BYTE[iLen];
	if ( m_pHeadBuf )
	{
		m_iHeadLen = iLen;
		return true;
	}
	else return false;
}

// 清空缓存
void CFMReader::ClearBuf()
{
	if ( m_bNeedFreeBuf )
	{
		// 删除旧的
		SAFE_DELETE(m_pBuf);
		m_iBufLen		= 0;
		m_iBufMaxLen	= 0;
		m_bNeedFreeBuf	= false;		// 缓存是否需要被释放
	}
}

// 设置缓存大小
bool CFMReader::SetBufSize(int iSize)
{
	if ( m_bNeedFreeBuf )
	{
		if ( iSize<=m_iBufMaxLen )
		{
			// 不需扩容
			m_iBufLen		= iSize;
			return true;
		}

		// 删除旧的
		SAFE_DELETE(m_pBuf);
		m_iBufLen		= 0;
		m_iBufMaxLen	= 0;
		m_bNeedFreeBuf	= false;		// 缓存是否需要被释放
	}

	// 新申请
	m_pBuf = new BYTE[iSize];
	if ( m_pBuf )
	{
		m_iBufLen		= iSize;
		m_iBufMaxLen	= iSize;
		m_bNeedFreeBuf	= true;			// 缓存是否需要被释放
		return true;
	}
	else
	{
		m_iBufLen		= 0;
		m_iBufMaxLen	= 0;
		m_bNeedFreeBuf	= false;		// 缓存是否需要被释放
		return false;
	}
}

// 生成一个读入器
CFMReader* CFMReader::GenReder(DWORD dwReadType)
{
	if ( dwReadType==READER_T_JNV )	return new CFMReaderJNV;
	
	return NULL;
}

// 设置一帧的数据,不与ReadOneFrame共用
int CFMReader::SetFrameData(const BYTE* pBuf,int iSize)
{
	m_pBuf			= (BYTE*)pBuf;		// 缓存
	m_iBufLen		= iSize;			// 缓存大小
	m_iBufMaxLen	= iSize;			// 缓存大小
	m_bNeedFreeBuf	= false;			// 缓存是否需要被释放

	if ( pBuf==NULL ) return -1;
	// 判断帧数据是否正确
	if ( OnSetFrameData()!=0 )
	{
		m_pBuf			= 0;			// 缓存
		m_iBufLen		= 0;			// 缓存大小
		return -1;
	}
	return m_iBufLen;
}

// 获取帧头长度
DWORD CFMReader::GetFrameHeadLength()
{
	return m_iHeadLen;
}

