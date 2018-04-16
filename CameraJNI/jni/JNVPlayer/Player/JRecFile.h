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
		time_t	m_tTime;				// 帧时间(秒)
		DWORD	m_dwMTime;				// 帧时间(毫秒)
		long	m_lPos;					// 文件位置
	}* PSTJRecIFrame;
public:
	CJRecFile();
	virtual ~CJRecFile();

	int		Open(LPCTSTR lpszPath,long lFileOffset);	// 打开文件
	int		OpenForInfo(LPCTSTR lpszPath,long lFileOffset);	// 打开文件
	int		ReOpen();									// 重新打开文件
	int		Close();									// 关闭文件
	bool	IsOpen();									// 是否已经打开了
	long	Seek(long offset, int origin );				// 移动指针
	long	Seek(time_t tTime);							// 移动到指定的帧

	long	GetFileHeadLen();							// 获取文件头的长
	long	GetFileSize();								// 获取文件的大小
	DWORD	GetFileProtocal();							// 获取文件的协议类型
	DWORD	GetFileEncoder();							// 获取文件的编码器
	time_t	GetStartTime();								// 获取文件起始时间
	DWORD	GetFileTime();								// 获取文件总时间
	DWORD	GetFileTotalFrames();						// 获取文件总帧数
	DWORD	GetCurrentFrameRate();						// 获取当前帧率
	DWORD	GetPlayedTime();							// 获取已播放时间
	DWORD	GetPlayedFrames(); 							// 获取已解码的视频帧数
	bool	GetPictureSize(LONG *pWidth,LONG *pHeight);	// 获取原始图像大小
	eDecType GetVideoFormat();							// 获取视频类型
	int		GetAudioFormat();							// 获取音频类型
	int		GetAudioSamples();							// 获取采样率

	// 支持移动I帧功能 begin
	int		ReadIFrame(bool bNext);						// 读取前/后一I帧
	// 支持移动I帧功能 end
	int		ReadFrame(bool bSkip);						// 读取一帧
	int		GetFrameLen();								// 获取帧长
	int		GetFrameType();								// 获取帧类型
	LONGLONG	GetNextFrameMTime();					// 获取帧时间戳(毫秒类型)
	time_t	GetFrameTime();								// 获取帧时间戳(秒类型)
	DWORD	GetFrameMTime();							// 获取帧时间戳(毫秒类型)
	time_t	GetWriteTime();								// 获取写入帧的时间戳(秒类型)
	DWORD	GetFrameSpan();								// 获取帧时间戳的差
	BYTE*	GetFrameData();								// 获取帧数据
	BYTE*	GetStreamData();							// 获取帧中的流数据
	int		GetStreamLen();								// 获取帧中的流数据长度
	LPCTSTR GetFrameOSD();								// 获取OSD

	int		MoveToPrevVideoFrame(int iCount);			// 向后移动指定的视频帧数
protected:
	// 文件操作 begin
	FILE*	OpenFile(const char* filename, const char* mode);	// 打开文件
	int		CloseFile();										// 关闭文件
	bool	IsOpenFile();										// 文件是否打开了
	long	TellFile();											// 返回文件当前位置
	int		SeekFile(long offset, int origin);					// 移动文件位置
	size_t	ReadFile(void *buffer, size_t size, size_t count);	// 读文件
	int		ReadFileHead();							 			// 获取文件头,私有信息
	int		ReadOneFrame(bool bSkip);							// 读入一帧
	// 文件操作 end

	bool	CheckValid();								// 是否有效

public:
protected:
	FILE*			m_pFile;						// 文件句柄
	long			m_lFileSize;					// 文件的大小
	time_t			m_tStartTime;					// 文件起始时间
	DWORD			m_dwFileTime;					// 文件总时间
	DWORD			m_dwFileTotalFrames;			// 文件总帧数
	DWORD			m_dwCurrentFrameRate;			// 当前帧率
	DWORD			m_dwPlayedTime;					// 已播放时间
	DWORD			m_dwPlayedFrames; 				// 已解码的视频帧数
	DWORD			m_dwLastVideoTime;				// 上一视频帧的时间
	DWORD			m_dwFrameSpan;					// 视频帧的间隔
	long			m_lStartOffset;					// 开始偏移量
	CFMReader*		m_pFMRead;						// 格式读入器
	
	bool			m_bHasFirstIFrame;				// 是否有第一个I帧

	// 支持移动I帧功能 begin
	bool			m_bInitCurIFrame;				// 当前I帧位置是否有效
	list<stJRecIFrame>::iterator m_itorCurIFrame;	// 当前I帧位置
	// 支持移动I帧功能 end

	list<stJRecIFrame>	m_ltIFrames;				// I帧信息列表
	CMutexLock		m_mutexFile;					// 文件锁
};

#endif // !defined(AFX_JRECFILE_H__0DDB6562_9180_4FDE_BAB8_068294821A6D__INCLUDED_)

