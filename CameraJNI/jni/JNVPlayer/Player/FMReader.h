// FMReader.h: interface for the CFMReader class.
//
//////////////////////////////////////////////////////////////////////

#if !defined(AFX_FMREADER_H__94509531_0DCA_4D0D_9BD2_E389025347F4__INCLUDED_)
#define AFX_FMREADER_H__94509531_0DCA_4D0D_9BD2_E389025347F4__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

#include "j_vs_ver.h"

// 协议类型 begin
class CFMReaderJNV;
#define READER_T_JNV		0x204A4E56	// JNV	不合法头
#define READER_MAGIC_JNV	0x4D4A4E56	// JNVM 合法头
// 协议类型 begin

#define READER_MAX_OSD		128

#ifdef		DbgStrOut
#define		ReaderTrace		DbgStrOut
#else
#define		ReaderTrace		printf
#endif

#ifndef		SAFE_DELETE
#define		SAFE_DELETE(pMem)			if(pMem){delete pMem;pMem=NULL;}
#endif

// CpuType
typedef enum
{	
	Encoder_Unknow	= -1,
	Encoder_DM365	= 0,
	Encoder_DM368	= 1,
	Encoder_DM8127	= 2,
	Encoder_DM8168	= 3,
	Encoder_HI		= 0x100-1,
	Encoder_3507	= 0x100,
	Encoder_3510,
	Encoder_3511,
	Encoder_3512,
	Encoder_3515,
	Encoder_3516,
	Encoder_3517,
	Encoder_3518,
	Encoder_3520,
	Encoder_3521,
	Encoder_3531,
	Encoder_3532,
	Encoder_3531_3532,
}eEncoderType;

// 定义帧类型 begin
typedef enum 
{
	READER_FRAME_I		= 0x1,			// I帧
	READER_FRAME_P		= 0x2,			// P帧
	READER_FRAME_B		= 0x3,			// B帧
	READER_FRAME_A		= 0x4,			// 音频帧
	READER_FRAME_STR	= 0x5,			// 字幕帧
	READER_FRAME_SHAPE	= 0x6,			// 形状帧
	READER_FRAME_USER	= 0xF,			// 用户自定义帧
}eFrameType;
// 定义帧类型 end

// 定义解码方式 begin
typedef enum 
{ 
	DEC_TYPE_UNKNOW		= 0,			// 未知格式
	DEC_TYPE_H264		= 1,			// H264
	DEC_TYPE_MJPEG		= 2,			// MJPEG
}eDecType;
typedef enum 
{ 
	AUDIO_TYPE_PCM		= 0,			// PCM
	AUDIO_TYPE_G711A	= 1,			// G711A
	AUDIO_TYPE_G711U	= 2,			// G711U
	AUDIO_TYPE_G726		= 3,			// G726
}eAudioType;
// 定义解码方式 end

class CFMReader  
{
public:
	CFMReader();
	virtual ~CFMReader();
	
	int		SetFrameData(const BYTE* pBuf,int iSize);			// 设置一帧的数据
	void	ClearBuf();											// 清空缓存
	bool	SetBufSize(int iSize);								// 设置缓存大小

	static CFMReader*	GenReder(DWORD dwReadType);				// 生成一个读入器

	// 需要重定义的 begin
	virtual int		OnSetFrameData()			= 0;			// 设置了一帧的数据
	virtual bool	IsFrameValid()				= 0;			// 检查帧头是否合法
	virtual DWORD	GetReadType()				= 0;			// 获取解码类型
	virtual int		GetFrameLen()				= 0;			// 获取帧长
	virtual int		GetFrameLen(const void* pBuf)	= 0;		// 获取帧长
	virtual int		GetFrameType()				= 0;			// 获取帧类型
	virtual int		GetFrameType(int iType)		= 0;			// 获取帧类型
	virtual LONGLONG GetFrameTime()				= 0;			// 获取帧时间戳(秒类型)
	virtual LONGLONG GetFrameMTime()			= 0;			// 获取帧时间戳(毫秒类型)
	virtual LONGLONG GetWriteTime()				= 0;			// 获取写入帧的时间戳(秒类型)
	virtual bool	SetWriteTime(LONGLONG lWrite)	= 0;		// 设置写入帧的时间戳(秒类型)
	virtual BYTE*	GetFrameData()				= 0;			// 获取帧数据
	virtual BYTE*	GetStreamData()				= 0;			// 获取帧中的流数据
	virtual int		GetStreamLen()				= 0;			// 获取帧中的流数据长度
	virtual bool	GetPictureSize(LONG *pWidth,LONG *pHeight)	= 0;	// 获取原始图像大小
	virtual eDecType GetVideoFormat()			= 0;			// 获取视频类型
	virtual int		GetAudioFormat()			= 0;			// 获取音频类型
	virtual int		GetAudioSamples()			= 0;			// 获取音频采样率
	virtual DWORD	GetFrameID()				= 0;			// 获取帧号
	virtual LPCTSTR GetFrameOSD()				= 0;			// 获取OSD
	virtual bool	SetFrameOSD(LPCTSTR lpszOSD)= 0;			// 设置OSD,最长 READER_MAX_OSD
	// 需要重定义的 end

	// 可以重定义的 begin
	virtual DWORD	GetFrameHeadLength();						// 获取帧头长度
	// 可以重定义的 end

protected:
	bool			SetHeadLen(int iLen);	// 设置包头长,只在构造时使用一次

public:
	BYTE*		m_pBuf;						// 缓存
	BYTE*		m_pHeadBuf;					// 包头缓存
protected:
	bool		m_bNeedFreeBuf;				// 缓存是否需要被释放
	int			m_iBufMaxLen;				// 缓存的最大大小
	int			m_iBufLen;					// 缓存大小
	int			m_iHeadLen;					// 包头长
};
typedef	CFMReader*	LPFMREADER;

#endif // !defined(AFX_FMREADER_H__94509531_0DCA_4D0D_9BD2_E389025347F4__INCLUDED_)
