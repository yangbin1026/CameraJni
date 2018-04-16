// FMReaderJNV.h: interface for the CFMReaderJNV class.
//
//////////////////////////////////////////////////////////////////////

#if !defined(AFX_FMREADERJNV_H__0D9BCDBB_E22C_4BFE_9F42_B77876A63CE9__INCLUDED_)
#define AFX_FMREADERJNV_H__0D9BCDBB_E22C_4BFE_9F42_B77876A63CE9__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

#include "FMReader.h"

class CFMReaderJNV : public CFMReader  
{
public:
	// 数据帧类型
	typedef enum
	{
		JNET_EMT_I_FRAME			= 0x00002001,
		JNET_EMT_P_FRAME			= 0x00002002,
		JNET_EMT_A_FRAME			= 0x00002003,
		JNET_EMT_PICTURE			= 0x00002004,
		JNET_EMT_MIC_DATA			= 0x00002005,
		JNET_EMT_FILE_DATA			= 0x00002006,
		JNET_EMT_TRANSPARENT		= 0x00002007,
		JNET_EMT_GPS_DATA			= 0x00002008,
		JNET_EMT_HEAD_FRAME			= 0x00002009,
	}eJNetMediaType; 
	// 录像格式
	typedef struct stFrameHead
	{
		DWORD				dwMagic;		// 魔字
		long				dwFrameNo;		// 帧号
		int					nWidth;			// 宽度
		int					nHeight;		// 高度
		int					nMediaType;		// 媒体类型,见 eJNetMediaType
		int					nFrameRate;		// 帧率
		__int64				tTime;			// 时间戳
		DWORD				dwDataSize;		// 帧数据长
		DWORD				dwRes;			// 保留,为了对齐
	//	char				pData[0];		// 帧数据
	}* PSTFrameHead;
public:
	CFMReaderJNV();
	virtual ~CFMReaderJNV();

	virtual int		OnSetFrameData();				// 设置了一帧的数据
	virtual bool	IsFrameValid();					// 检查帧头是否合法
	virtual DWORD	GetReadType();					// 获取解码类型
	virtual int		GetFrameLen();					// 获取帧长
	virtual int		GetFrameLen(const void* pBuf);	// 获取帧长
	virtual int		GetFrameType();					// 获取帧类型
	virtual int		GetFrameType(int iType);		// 获取帧类型
	virtual LONGLONG GetFrameTime();				// 获取帧时间戳(秒类型)
	virtual LONGLONG GetFrameMTime();				// 获取帧时间戳(毫秒类型)
	virtual LONGLONG GetWriteTime();				// 获取写入帧的时间戳(秒类型)
	virtual bool	SetWriteTime(LONGLONG lWrite);	// 设置写入帧的时间戳(秒类型)
	virtual BYTE*	GetFrameData();					// 获取帧数据
	virtual BYTE*	GetStreamData();				// 获取帧中的流数据
	virtual int		GetStreamLen();					// 获取帧中的流数据长度
	virtual bool	GetPictureSize(LONG *pWidth,LONG *pHeight);	// 获取原始图像大小
	virtual eDecType GetVideoFormat();				// 获取视频类型
	virtual int		GetAudioFormat();				// 获取音频类型
	virtual int		GetAudioSamples();				// 获取音频采样率
	virtual DWORD	GetFrameID();					// 获取帧号
	virtual LPCTSTR GetFrameOSD();					// 获取OSD
	virtual bool	SetFrameOSD(LPCTSTR lpszOSD);	// 设置OSD,最长 READER_MAX_OSD

protected:
	PSTFrameHead		m_pstHead;					// 帧头指针
	int					m_iFrameHeadLen;			// 帧头长
};

#endif // !defined(AFX_FMREADERJNV_H__0D9BCDBB_E22C_4BFE_9F42_B77876A63CE9__INCLUDED_)
