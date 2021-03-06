// FMReaderJNV.cpp: implementation of the CFMReaderJNV class.
//
//////////////////////////////////////////////////////////////////////

#include "FMReaderJNV.h"
#include "time.h"

//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////
//读入器
CFMReaderJNV::CFMReaderJNV()
{
	m_iFrameHeadLen	= sizeof(stFrameHead);			// 帧头长

	SetHeadLen(m_iFrameHeadLen);
	m_pstHead		= (stFrameHead*)m_pHeadBuf;		// 帧头指针
}

CFMReaderJNV::~CFMReaderJNV()
{
	m_pstHead		= NULL;		// 帧头指针
}

// 设置了一帧的数据
int CFMReaderJNV::OnSetFrameData()
{
	m_pstHead		= (stFrameHead*)m_pBuf;		// 帧头指针

	if (m_pstHead->dwMagic!=READER_MAGIC_JNV || m_pstHead->dwDataSize<=0)
	{
		m_pstHead		= NULL;
		return -1;
	}

	return 0;
}

// 检查帧头是否合法
bool CFMReaderJNV::IsFrameValid()
{
	if (m_pstHead==NULL || m_pstHead->dwMagic!=READER_MAGIC_JNV || m_pstHead->dwDataSize<=0)
	{
		if (m_pstHead==NULL)
		{
			ReaderTrace("CFMReaderJNV::IsFrameValid m_pstHead is null\r\n");
		}
		else if (m_pstHead->dwMagic!=READER_MAGIC_JNV)
		{
			ReaderTrace("CFMReaderJNV::IsFrameValid m_pstHead->dwMagic!=0x%x\r\n",READER_MAGIC_JNV);
		}
		else if (m_pstHead->dwDataSize<=0)
		{
			ReaderTrace("CFMReaderJNV::IsFrameValid m_pstHead->dwDataSize\r\n");
		}
		return false;
	}

	return true;
}

// 获取解码类型
DWORD CFMReaderJNV::GetReadType()
{
	return READER_T_JNV;
}

// 获取帧长
int CFMReaderJNV::GetFrameLen()
{
	if ( m_pstHead==NULL ) return -1;

	return m_pstHead->dwDataSize+m_iHeadLen;
}

// 获取帧长
int CFMReaderJNV::GetFrameLen(const void* pBuf)
{
	PSTFrameHead pstHead	= NULL;
	
	pstHead		= (PSTFrameHead)pBuf;		// 帧头指针

	if (pstHead->dwMagic!=READER_MAGIC_JNV || pstHead->dwDataSize<=0)
	{
		return -1;
	}
	else
	{
		return pstHead->dwDataSize+sizeof(stFrameHead);
	}
}

// 获取帧类型
int CFMReaderJNV::GetFrameType()
{
	if ( m_pstHead==NULL ) return -1;

	switch(m_pstHead->nMediaType)
	{
	case JNET_EMT_A_FRAME:
		return READER_FRAME_A;			// 音频帧
	case JNET_EMT_I_FRAME:
		return READER_FRAME_I;			// I帧
	case JNET_EMT_P_FRAME:
		return READER_FRAME_P;			// P帧
	default:
		return READER_FRAME_USER;		// 用户自定义帧
	}

	if ( m_pstHead==NULL ) return -1;

	return m_pstHead->nMediaType;
}

// 获取帧类型
int CFMReaderJNV::GetFrameType(int iType)
{
	switch(iType)
	{
	case JNET_EMT_A_FRAME:
		return READER_FRAME_A;			// 音频帧
	case JNET_EMT_I_FRAME:
		return READER_FRAME_I;			// I帧
	case JNET_EMT_P_FRAME:
		return READER_FRAME_P;			// P帧
	default:
		return READER_FRAME_USER;		// 用户自定义帧
	}
}

// 获取帧时间戳
LONGLONG CFMReaderJNV::GetFrameTime()
{
	LONGLONG	lRet	= 0;
	if ( m_pstHead==NULL ) return -1;

	lRet = (m_pstHead->tTime)/1000;		// 转换为秒
	if ( (long)lRet<0 ) lRet = 0;
	return lRet;
}

// 获取帧时间戳(毫秒类型)
LONGLONG CFMReaderJNV::GetFrameMTime()
{
	return m_pstHead->tTime;			// 转为毫秒
}

// 获取写入帧的时间戳(秒类型)
LONGLONG CFMReaderJNV::GetWriteTime()
{
	return GetFrameTime();
}

// 设置写入帧的时间戳(秒类型)
bool CFMReaderJNV::SetWriteTime(LONGLONG lWrite)
{
	return false;
}

// 获取帧数据
BYTE* CFMReaderJNV::GetFrameData()
{
	if ( m_pstHead==NULL || m_pBuf==NULL ) return NULL;
	
	return m_pBuf;
}

// 获取帧中的流数据
BYTE* CFMReaderJNV::GetStreamData()
{
	if ( m_pstHead==NULL || m_pBuf==NULL ) return NULL;
	
	return m_pBuf+m_iHeadLen;
}

// 获取帧中的流数据长度
int CFMReaderJNV::GetStreamLen()
{
	if ( m_pstHead==NULL ) return -1;
	
	return m_pstHead->dwDataSize;
}

// 获取原始图像大小
bool CFMReaderJNV::GetPictureSize(LONG *pWidth,LONG *pHeight)
{
	if ( m_pstHead==NULL ) return false;

	if ( pWidth )	*pWidth		= m_pstHead->nWidth;		// 原始图像宽
	if ( pHeight )	*pHeight	= m_pstHead->nHeight;	// 原始图像高

	return true;
}

// 获取视频类型
eDecType CFMReaderJNV::GetVideoFormat()
{
	if ( m_pstHead==NULL ) return DEC_TYPE_UNKNOW;
	if ( GetFrameType()==READER_FRAME_A ) return DEC_TYPE_UNKNOW;

	return DEC_TYPE_H264;
}

// 获取音频类型
int CFMReaderJNV::GetAudioFormat()
{
	if ( m_pstHead==NULL ) return false;
	if ( GetFrameType()!=READER_FRAME_A ) return 0;

	return AUDIO_TYPE_G726;
}

// 获取音频采样率
int CFMReaderJNV::GetAudioSamples()
{
	return 8000;
}

// 获取帧号
DWORD CFMReaderJNV::GetFrameID()
{
	if ( m_pstHead==NULL ) return -1;

	return m_pstHead->dwFrameNo;
}

// 获取OSD
LPCTSTR CFMReaderJNV::GetFrameOSD()
{
	return NULL;
}

// 设置OSD,最长 READER_MAX_OSD
bool CFMReaderJNV::SetFrameOSD(LPCTSTR lpszOSD)
{
	return false;
}
