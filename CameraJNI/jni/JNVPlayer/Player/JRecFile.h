// JRecFile.h: interface for the CJRecFile class.
//
//////////////////////////////////////////////////////////////////////

#if !defined(AFX_JRECFILE_H__0DDB6562_9180_4FDE_BAB8_068294821A6D__INCLUDED_)
#define AFX_JRECFILE_H__0DDB6562_9180_4FDE_BAB8_068294821A6D__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

#include "ProtocolNV.h"
#include "FMReader.h"

class CJRecFile  
{
	typedef struct stJRecIFrame
	{
		time_t	m_tTime;				// ֡ʱ��(��)
		DWORD	m_dwMTime;				// ֡ʱ��(����)
		long	m_lPos;					// �ļ�λ��
	}* PSTJRecIFrame;
public:
	CJRecFile();
	virtual ~CJRecFile();

	int		Open(LPCTSTR lpszPath,long lFileOffset);	// ���ļ�
	int		OpenForInfo(LPCTSTR lpszPath,long lFileOffset);	// ���ļ�
	int		ReOpen();									// ���´��ļ�
	int		Close();									// �ر��ļ�
	bool	IsOpen();									// �Ƿ��Ѿ�����
	long	Seek(long offset, int origin );				// �ƶ�ָ��
	long	Seek(time_t tTime);							// �ƶ���ָ����֡

	long	GetFileHeadLen();							// ��ȡ�ļ�ͷ�ĳ�
	long	GetFileSize();								// ��ȡ�ļ��Ĵ�С
	DWORD	GetFileProtocal();							// ��ȡ�ļ���Э������
	DWORD	GetFileEncoder();							// ��ȡ�ļ��ı�����
	time_t	GetStartTime();								// ��ȡ�ļ���ʼʱ��
	DWORD	GetFileTime();								// ��ȡ�ļ���ʱ��
	DWORD	GetFileTotalFrames();						// ��ȡ�ļ���֡��
	DWORD	GetCurrentFrameRate();						// ��ȡ��ǰ֡��
	DWORD	GetPlayedTime();							// ��ȡ�Ѳ���ʱ��
	DWORD	GetPlayedFrames(); 							// ��ȡ�ѽ������Ƶ֡��
	bool	GetPictureSize(LONG *pWidth,LONG *pHeight);	// ��ȡԭʼͼ���С
	eDecType GetVideoFormat();							// ��ȡ��Ƶ����
	int		GetAudioFormat();							// ��ȡ��Ƶ����
	int		GetAudioSamples();							// ��ȡ������

	// ֧���ƶ�I֡���� begin
	int		ReadIFrame(bool bNext);						// ��ȡǰ/��һI֡
	// ֧���ƶ�I֡���� end
	int		ReadFrame(bool bSkip);						// ��ȡһ֡
	int		GetFrameLen();								// ��ȡ֡��
	int		GetFrameType();								// ��ȡ֡����
	LONGLONG	GetNextFrameMTime();					// ��ȡ֡ʱ���(��������)
	time_t	GetFrameTime();								// ��ȡ֡ʱ���(������)
	DWORD	GetFrameMTime();							// ��ȡ֡ʱ���(��������)
	time_t	GetWriteTime();								// ��ȡд��֡��ʱ���(������)
	DWORD	GetFrameSpan();								// ��ȡ֡ʱ����Ĳ�
	BYTE*	GetFrameData();								// ��ȡ֡����
	BYTE*	GetStreamData();							// ��ȡ֡�е�������
	int		GetStreamLen();								// ��ȡ֡�е������ݳ���
	LPCTSTR GetFrameOSD();								// ��ȡOSD

	int		MoveToPrevVideoFrame(int iCount);			// ����ƶ�ָ������Ƶ֡��
protected:
	// �ļ����� begin
	FILE*	OpenFile(const char* filename, const char* mode);	// ���ļ�
	int		CloseFile();										// �ر��ļ�
	bool	IsOpenFile();										// �ļ��Ƿ����
	long	TellFile();											// �����ļ���ǰλ��
	int		SeekFile(long offset, int origin);					// �ƶ��ļ�λ��
	size_t	ReadFile(void *buffer, size_t size, size_t count);	// ���ļ�
	int		ReadFileHead();							 			// ��ȡ�ļ�ͷ,˽����Ϣ
	int		ReadOneFrame(bool bSkip);							// ����һ֡
	// �ļ����� end

	bool	CheckValid();								// �Ƿ���Ч

public:
protected:
	FILE*			m_pFile;						// �ļ����
	long			m_lFileSize;					// �ļ��Ĵ�С
	time_t			m_tStartTime;					// �ļ���ʼʱ��
	DWORD			m_dwFileTime;					// �ļ���ʱ��
	DWORD			m_dwFileTotalFrames;			// �ļ���֡��
	DWORD			m_dwCurrentFrameRate;			// ��ǰ֡��
	DWORD			m_dwPlayedTime;					// �Ѳ���ʱ��
	DWORD			m_dwPlayedFrames; 				// �ѽ������Ƶ֡��
	DWORD			m_dwLastVideoTime;				// ��һ��Ƶ֡��ʱ��
	DWORD			m_dwFrameSpan;					// ��Ƶ֡�ļ��
	long			m_lStartOffset;					// ��ʼƫ����
	CFMReader*		m_pFMRead;						// ��ʽ������
	
	bool			m_bHasFirstIFrame;				// �Ƿ��е�һ��I֡

	// ֧���ƶ�I֡���� begin
	bool			m_bInitCurIFrame;				// ��ǰI֡λ���Ƿ���Ч
	list<stJRecIFrame>::iterator m_itorCurIFrame;	// ��ǰI֡λ��
	// ֧���ƶ�I֡���� end

	list<stJRecIFrame>	m_ltIFrames;				// I֡��Ϣ�б�
	CMutexLock		m_mutexFile;					// �ļ���
};

#endif // !defined(AFX_JRECFILE_H__0DDB6562_9180_4FDE_BAB8_068294821A6D__INCLUDED_)

