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
	// ����֡����
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
	// ¼���ʽ
	typedef struct stFrameHead
	{
		DWORD				dwMagic;		// ħ��
		long				dwFrameNo;		// ֡��
		int					nWidth;			// ���
		int					nHeight;		// �߶�
		int					nMediaType;		// ý������,�� eJNetMediaType
		int					nFrameRate;		// ֡��
		__int64				tTime;			// ʱ���
		DWORD				dwDataSize;		// ֡���ݳ�
		DWORD				dwRes;			// ����,Ϊ�˶���
	//	char				pData[0];		// ֡����
	}* PSTFrameHead;
public:
	CFMReaderJNV();
	virtual ~CFMReaderJNV();

	virtual int		OnSetFrameData();				// ������һ֡������
	virtual bool	IsFrameValid();					// ���֡ͷ�Ƿ�Ϸ�
	virtual DWORD	GetReadType();					// ��ȡ��������
	virtual int		GetFrameLen();					// ��ȡ֡��
	virtual int		GetFrameLen(const void* pBuf);	// ��ȡ֡��
	virtual int		GetFrameType();					// ��ȡ֡����
	virtual int		GetFrameType(int iType);		// ��ȡ֡����
	virtual LONGLONG GetFrameTime();				// ��ȡ֡ʱ���(������)
	virtual LONGLONG GetFrameMTime();				// ��ȡ֡ʱ���(��������)
	virtual LONGLONG GetWriteTime();				// ��ȡд��֡��ʱ���(������)
	virtual bool	SetWriteTime(LONGLONG lWrite);	// ����д��֡��ʱ���(������)
	virtual BYTE*	GetFrameData();					// ��ȡ֡����
	virtual BYTE*	GetStreamData();				// ��ȡ֡�е�������
	virtual int		GetStreamLen();					// ��ȡ֡�е������ݳ���
	virtual bool	GetPictureSize(LONG *pWidth,LONG *pHeight);	// ��ȡԭʼͼ���С
	virtual eDecType GetVideoFormat();				// ��ȡ��Ƶ����
	virtual int		GetAudioFormat();				// ��ȡ��Ƶ����
	virtual int		GetAudioSamples();				// ��ȡ��Ƶ������
	virtual DWORD	GetFrameID();					// ��ȡ֡��
	virtual LPCTSTR GetFrameOSD();					// ��ȡOSD
	virtual bool	SetFrameOSD(LPCTSTR lpszOSD);	// ����OSD,� READER_MAX_OSD

protected:
	PSTFrameHead		m_pstHead;					// ֡ͷָ��
	int					m_iFrameHeadLen;			// ֡ͷ��
};

#endif // !defined(AFX_FMREADERJNV_H__0D9BCDBB_E22C_4BFE_9F42_B77876A63CE9__INCLUDED_)
