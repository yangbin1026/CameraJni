// FMReader.h: interface for the CFMReader class.
//
//////////////////////////////////////////////////////////////////////

#if !defined(AFX_FMREADER_H__94509531_0DCA_4D0D_9BD2_E389025347F4__INCLUDED_)
#define AFX_FMREADER_H__94509531_0DCA_4D0D_9BD2_E389025347F4__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

#include "j_vs_ver.h"

// Э������ begin
class CFMReaderJNV;
#define READER_T_JNV		0x204A4E56	// JNV	���Ϸ�ͷ
#define READER_MAGIC_JNV	0x4D4A4E56	// JNVM �Ϸ�ͷ
// Э������ begin

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

// ����֡���� begin
typedef enum 
{
	READER_FRAME_I		= 0x1,			// I֡
	READER_FRAME_P		= 0x2,			// P֡
	READER_FRAME_B		= 0x3,			// B֡
	READER_FRAME_A		= 0x4,			// ��Ƶ֡
	READER_FRAME_STR	= 0x5,			// ��Ļ֡
	READER_FRAME_SHAPE	= 0x6,			// ��״֡
	READER_FRAME_USER	= 0xF,			// �û��Զ���֡
}eFrameType;
// ����֡���� end

// ������뷽ʽ begin
typedef enum 
{ 
	DEC_TYPE_UNKNOW		= 0,			// δ֪��ʽ
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
// ������뷽ʽ end

class CFMReader  
{
public:
	CFMReader();
	virtual ~CFMReader();
	
	int		SetFrameData(const BYTE* pBuf,int iSize);			// ����һ֡������
	void	ClearBuf();											// ��ջ���
	bool	SetBufSize(int iSize);								// ���û����С

	static CFMReader*	GenReder(DWORD dwReadType);				// ����һ��������

	// ��Ҫ�ض���� begin
	virtual int		OnSetFrameData()			= 0;			// ������һ֡������
	virtual bool	IsFrameValid()				= 0;			// ���֡ͷ�Ƿ�Ϸ�
	virtual DWORD	GetReadType()				= 0;			// ��ȡ��������
	virtual int		GetFrameLen()				= 0;			// ��ȡ֡��
	virtual int		GetFrameLen(const void* pBuf)	= 0;		// ��ȡ֡��
	virtual int		GetFrameType()				= 0;			// ��ȡ֡����
	virtual int		GetFrameType(int iType)		= 0;			// ��ȡ֡����
	virtual LONGLONG GetFrameTime()				= 0;			// ��ȡ֡ʱ���(������)
	virtual LONGLONG GetFrameMTime()			= 0;			// ��ȡ֡ʱ���(��������)
	virtual LONGLONG GetWriteTime()				= 0;			// ��ȡд��֡��ʱ���(������)
	virtual bool	SetWriteTime(LONGLONG lWrite)	= 0;		// ����д��֡��ʱ���(������)
	virtual BYTE*	GetFrameData()				= 0;			// ��ȡ֡����
	virtual BYTE*	GetStreamData()				= 0;			// ��ȡ֡�е�������
	virtual int		GetStreamLen()				= 0;			// ��ȡ֡�е������ݳ���
	virtual bool	GetPictureSize(LONG *pWidth,LONG *pHeight)	= 0;	// ��ȡԭʼͼ���С
	virtual eDecType GetVideoFormat()			= 0;			// ��ȡ��Ƶ����
	virtual int		GetAudioFormat()			= 0;			// ��ȡ��Ƶ����
	virtual int		GetAudioSamples()			= 0;			// ��ȡ��Ƶ������
	virtual DWORD	GetFrameID()				= 0;			// ��ȡ֡��
	virtual LPCTSTR GetFrameOSD()				= 0;			// ��ȡOSD
	virtual bool	SetFrameOSD(LPCTSTR lpszOSD)= 0;			// ����OSD,� READER_MAX_OSD
	// ��Ҫ�ض���� end

	// �����ض���� begin
	virtual DWORD	GetFrameHeadLength();						// ��ȡ֡ͷ����
	// �����ض���� end

protected:
	bool			SetHeadLen(int iLen);	// ���ð�ͷ��,ֻ�ڹ���ʱʹ��һ��

public:
	BYTE*		m_pBuf;						// ����
	BYTE*		m_pHeadBuf;					// ��ͷ����
protected:
	bool		m_bNeedFreeBuf;				// �����Ƿ���Ҫ���ͷ�
	int			m_iBufMaxLen;				// ���������С
	int			m_iBufLen;					// �����С
	int			m_iHeadLen;					// ��ͷ��
};
typedef	CFMReader*	LPFMREADER;

#endif // !defined(AFX_FMREADER_H__94509531_0DCA_4D0D_9BD2_E389025347F4__INCLUDED_)
