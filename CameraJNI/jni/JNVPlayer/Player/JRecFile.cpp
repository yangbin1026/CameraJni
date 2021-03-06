// JRecFile.cpp: implementation of the CJRecFile class.
//
//////////////////////////////////////////////////////////////////////

#include "JRecFile.h"
#include "FMReaderJNV.h"

//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////

CJRecFile::CJRecFile()
{
	m_pFile					= NULL;			// 文件句柄
	m_lFileSize				= 0;			// 文件的大小
	m_dwFileTime			= 0;			// 文件总时间
	m_dwFileTotalFrames		= 0;			// 文件总帧数
	m_dwCurrentFrameRate	= 0;			// 当前帧率
	m_dwPlayedTime			= 0;			// 已播放时间
	m_dwPlayedFrames		= 0; 			// 已解码的视频帧数
	m_dwLastVideoTime		= 0;			// 上一视频帧的时间
	m_dwFrameSpan			= 40;			// 视频帧间隔
	m_pFMRead				= NULL;			// 格式读入器
	m_bHasFirstIFrame		= FALSE;		// 是否有第一个I帧
	m_lStartOffset			= 0;			// 开始偏移量

	m_mutexFile.CreateMutex();
	
	// 支持移动I帧功能 begin
	m_bInitCurIFrame		= false;		// 当前I帧位置是否有效
	// 支持移动I帧功能 end
}

CJRecFile::~CJRecFile()
{
	m_ltIFrames.clear();
	// 支持移动I帧功能 begin
	m_bInitCurIFrame		= false;		// 当前I帧位置是否有效
	// 支持移动I帧功能 end
	Close();
}

// 打开文件
int CJRecFile::Open(LPCTSTR lpszPath,long lFileOffset)
{
	if ( OpenFile(lpszPath,"rb")==NULL )
	{
		return -1;
	}

	// 开始偏移量
	m_lStartOffset = lFileOffset;
	if ( m_lStartOffset<0 ) m_lStartOffset=0;

	// 也可以在CheckValid里获取文件具体信息
	if ( !CheckValid() )
	{
		Close();
		return -2;
	}

	return 0;
}

// 打开文件
int CJRecFile::OpenForInfo(LPCTSTR lpszPath,long lFileOffset)
{
	int				iRead			= 0;
	int				iMinOffset		= 0;

	if ( OpenFile(lpszPath,"rb")==NULL )
	{
		return -1;
	}

	// 文件头检查 begin
	iMinOffset = ReadFileHead();
	if ( iMinOffset<0 ) return -2;
	if ( m_lStartOffset<iMinOffset ) m_lStartOffset=iMinOffset;
	if ( m_lStartOffset>=0 ) SeekFile(m_lStartOffset,SEEK_SET);
	// 文件头检查 end

	m_tStartTime			= 0;			// 文件起始时间
	m_dwFileTime			= 0;			// 文件总时间
	m_dwFileTotalFrames		= 0;			// 文件总帧数
	m_dwCurrentFrameRate	= 0;			// 当前帧率
	m_dwPlayedTime			= 0;			// 已播放时间
	m_dwPlayedFrames		= 0; 			// 已解码的视频帧数
	m_dwLastVideoTime		= 0;			// 上一视频帧的时间
	m_bHasFirstIFrame		= false;		// 是否有第一个I帧

	if ( m_pFMRead==NULL ) return -3;
	// 无索引信息,只能是从头读起
	// 快速计算数据信息 begin
	iRead = ReadFrame(true);
	while (iRead>0)
	{
		// 下一帧
		if ( READER_FRAME_I==GetFrameType()||READER_FRAME_P==GetFrameType() )
		{
			if ( m_tStartTime==0 ) m_tStartTime = GetFrameTime();
			m_dwFileTotalFrames++;			// 文件总帧数
		}
		iRead = ReadFrame(true);
	}
	m_lFileSize				= TellFile();				// 文件大小
	m_dwFileTime			= m_dwPlayedTime;			// 文件总时间等于已经播放的时间
	// 快速计算数据信息 end

	// 恢复原始状态
	if ( m_lStartOffset>=0 ) SeekFile(m_lStartOffset,SEEK_SET);
	m_dwCurrentFrameRate	= 0;			// 当前帧率
	m_dwPlayedTime			= 0;			// 已播放时间
	m_dwPlayedFrames		= 0; 			// 已解码的视频帧数
	m_dwLastVideoTime		= 0;			// 上一视频帧的时间
	m_bHasFirstIFrame		= false;		// 是否有第一个I帧

//	SAFE_DELETE(m_pFMRead);					// 临时打开的,所以清空格式器
	return 0;
}

// 重新打开文件
int CJRecFile::ReOpen()
{
	if ( Seek(0,SEEK_SET)==0 && CheckValid() ) return 0;

	Close();
	return -1;
}

// 关闭文件
int CJRecFile::Close()
{
	CloseFile();
	
	m_lFileSize				= 0;				// 文件的大小
	m_dwFileTime			= 0;				// 文件总时间
	m_dwFileTotalFrames		= 0;				// 文件总帧数
	m_dwCurrentFrameRate	= 0;				// 当前帧率
	m_dwPlayedTime			= 0;				// 已播放时间
	m_dwPlayedFrames		= 0; 				// 已解码的视频帧数
	m_lStartOffset			= 0;				// 开始偏移量
	m_ltIFrames.clear();						// 清空I帧信息列表
	// 支持移动I帧功能 begin
	m_bInitCurIFrame		= false;			// 当前I帧位置是否有效
	// 支持移动I帧功能 end

	SAFE_DELETE(m_pFMRead);						// 清空格式器
	return 0;
}

// 是否已经打开了
bool CJRecFile::IsOpen()
{
	if ( m_pFMRead==NULL ) return false;
	return IsOpenFile();
}

// 移动指针
long CJRecFile::Seek(long offset, int origin )
{
	long lRet	= 0;
	lRet = SeekFile(offset,origin);
	if ( lRet!=0 ) return lRet;

	return 0;
}

// 移动到指定的帧
long CJRecFile::Seek(time_t tTime)
{
	list<stJRecIFrame>::iterator itor;
	PSTJRecIFrame	pstIFrame		= NULL;
	PSTJRecIFrame	pstIFrameOld	= NULL;
	long			lSetPos			= 0;
	int				iRet			= 0;

	// 支持移动I帧功能 begin
	m_bInitCurIFrame	= false;	// 当前I帧位置是否有效
	// 支持移动I帧功能 end

	if ( IsOpen()==false ) return -1;

	for (itor = m_ltIFrames.begin(); itor!=m_ltIFrames.end(); itor++)
	{
		pstIFrame = &(*itor);
		if ( pstIFrame )
		{
			if ( pstIFrameOld==NULL ) pstIFrameOld=pstIFrame;
			if ( tTime>=(pstIFrameOld->m_tTime) && tTime<=(pstIFrame->m_tTime) )
			{
				lSetPos = pstIFrameOld->m_lPos;
				break;
			}
			pstIFrameOld=pstIFrame;
		}
	}

	if ( lSetPos>0 ) 
	{
		m_dwLastVideoTime = 0;
		iRet = SeekFile(lSetPos,SEEK_SET);
		ReaderTrace("CJRecFile::Seek:set=%ld,get=%ld.\r\n", lSetPos,TellFile());
		return iRet;
	}
	else
	{
		ReaderTrace("CJRecFile::Seek:Find I Frame error.set=%ld\r\n",tTime);
		return -1;
	}
}

// 获取文件头的长
long CJRecFile::GetFileHeadLen()
{
	return m_lStartOffset;
}

// 获取文件的大小
long CJRecFile::GetFileSize()
{
	return m_lFileSize;						// 文件的大小
}

// 获取文件的协议类型
DWORD CJRecFile::GetFileProtocal()
{
//	return m_stFileIndex.m_dwProtocal;
	if ( m_pFMRead==NULL ) return 0;
	return m_pFMRead->GetReadType();
}

// 获取文件的编码器
DWORD CJRecFile::GetFileEncoder()
{
	return READER_T_JNV;
}

// 获取文件起始时间
time_t CJRecFile::GetStartTime()
{
	return m_tStartTime;					// 文件起始时间
}

// 获取文件总时间
DWORD CJRecFile::GetFileTime()
{
	return m_dwFileTime;					// 文件总时间
}

// 获取文件总帧数
DWORD CJRecFile::GetFileTotalFrames()
{
	return m_dwFileTotalFrames;				// 文件总帧数
}

// 获取当前帧率
DWORD CJRecFile::GetCurrentFrameRate()
{
	return m_dwCurrentFrameRate;			// 当前帧率
}

// 获取已播放时间
DWORD CJRecFile::GetPlayedTime()
{
	return m_dwPlayedTime;					// 已播放时间
}

// 获取已解码的视频帧数
DWORD CJRecFile::GetPlayedFrames()
{
	return m_dwPlayedFrames; 				// 已解码的视频帧数
}

// 获取原始图像大小
bool CJRecFile::GetPictureSize(LONG *pWidth,LONG *pHeight)
{
	if ( m_pFMRead==NULL ) return false;
	return m_pFMRead->GetPictureSize(pWidth,pHeight);
}

// 获取视频类型
eDecType CJRecFile::GetVideoFormat()
{
	if ( m_pFMRead==NULL ) return DEC_TYPE_UNKNOW;
	return m_pFMRead->GetVideoFormat();
}

// 获取音频类型
int CJRecFile::GetAudioFormat()
{
	if ( m_pFMRead==NULL ) return 0;
	return m_pFMRead->GetAudioFormat();
}

// 获取采样率
int CJRecFile::GetAudioSamples()
{
	if ( m_pFMRead==NULL ) return 0;
	return m_pFMRead->GetAudioSamples();
}

// 文件操作 begin
// 打开文件
FILE* CJRecFile::OpenFile(const char* filename, const char* mode)
{
	if ( m_pFile ) return m_pFile;

	m_mutexFile.Lock();
	if ( m_pFile )
	{
		fclose(m_pFile);
		m_pFile = NULL;
	}
	fopen_s(&m_pFile,filename,mode);
	m_mutexFile.Unlock();

	return m_pFile;
}

// 关闭文件
int CJRecFile::CloseFile()
{
	int iRet	= 0;

	m_mutexFile.Lock();
	if ( m_pFile==NULL )
	{
		m_mutexFile.Unlock();
		return 0;
	}
	iRet		= fclose(m_pFile);
	m_pFile		= NULL;
	m_mutexFile.Unlock();

	return iRet;
}

// 文件是否打开了
bool CJRecFile::IsOpenFile()
{
	if ( m_pFile==NULL ) return false;
	else return true;
}

// 返回文件当前位置
long CJRecFile::TellFile()
{
	long	lRet	= 0;

	m_mutexFile.Lock();
	if ( m_pFile==NULL )
	{
		m_mutexFile.Unlock();
		return -1;
	}
	lRet = ftell(m_pFile);
	m_mutexFile.Unlock();

	return lRet;
}

// 移动文件位置
int CJRecFile::SeekFile(long offset, int origin)
{
	long	lRet	= 0;

	m_mutexFile.Lock();
	if ( m_pFile==NULL )
	{
		m_mutexFile.Unlock();
		return -1;
	}
	lRet = fseek(m_pFile,offset,origin);
	m_mutexFile.Unlock();

	return lRet;
}

// 读文件
size_t CJRecFile::ReadFile(void* buffer, size_t size, size_t count)
{
	size_t	iRet	= 0;
	
	m_mutexFile.Lock();
	if ( m_pFile==NULL )
	{
		m_mutexFile.Unlock();
		return -1;
	}
	iRet = fread(buffer,size,count,m_pFile);
	m_mutexFile.Unlock();

	return iRet;
}

// 获取文件头,私有信息
int CJRecFile::ReadFileHead()
{
//	int				iRead			= 0;
	int				iMinOffset		= 0;

	if ( m_pFile==NULL ) return -1;
	SAFE_DELETE(m_pFMRead);

	iMinOffset = 0;
	m_pFMRead = CFMReader::GenReder(READER_T_JNV);

	return iMinOffset;
}

// 读入一帧,存于m_pBuf中,不与SetFrameData共用
int CJRecFile::ReadOneFrame(bool bSkip)
{
	int		iRead		= 0;			// 每次读入数
	int		iHasRead	= 0;			// 读入总数,用于返回
	int		iFrameLen	= 0;			// 帧长
	int		iHeadLen	= 0;			// 帧头长
	BYTE*	lpFrameBuf	= NULL;			// 帧数据缓存

	if ( m_pFMRead==NULL || m_pFile==NULL ) return iHasRead;
	
	iHeadLen = m_pFMRead->GetFrameHeadLength();
	// 读入包头 begin
	iRead = ReadFile(m_pFMRead->m_pHeadBuf,1,iHeadLen);
	if ( m_pFMRead->IsFrameValid()==false )
	{
		ReaderTrace("CJRecFile::ReadOneFrame Frame isn't Valid\r\n");
		return 0;
	}
	if ( iRead<iHeadLen )
	{
		ReaderTrace("CJRecFile::ReadOneFrame read < Head len\r\n");
		return 0;
	}
	iHasRead+=iRead;
	iFrameLen = m_pFMRead->GetFrameLen();
	if ( iFrameLen<=0 )
	{
		ReaderTrace("CJRecFile::ReadOneFrame frame len < 0\r\n");
		return 0;
	}
	// 读入包头 end

	// 读入包体 begin
	if ( bSkip )
	{
		SeekFile(iFrameLen-iHeadLen,SEEK_CUR);
		iHasRead+=(iFrameLen-iHeadLen);
	}
	else
	{
		if ( m_pFMRead->SetBufSize(iFrameLen)==false ) return iHasRead;
		lpFrameBuf	= m_pFMRead->m_pBuf;			// 帧数据缓存
		memcpy(lpFrameBuf,m_pFMRead->m_pHeadBuf,iHeadLen);
		lpFrameBuf+=iHeadLen;

		// 帧模式
		iRead = ReadFile(lpFrameBuf,1,iFrameLen-iHeadLen);
		if ( iRead<=0 )
		{
			ReaderTrace("CJRecFile::ReadOneFrame read frame < 0\r\n");
			return 0;
		}
		iHasRead+=iRead;
	}
	// 读入包体 end

	return iHasRead;
}
// 文件操作 end

// 是否有效
bool CJRecFile::CheckValid()
{
	int				iRead			= 0;
	stJRecIFrame	stIFrame;
	int				iMinOffset		= 0;
	int				iPos			= 0;

	// 文件头检查 begin
	iMinOffset = ReadFileHead();
	if ( iMinOffset<0 ) return false;
	// 文件头检查 end

	m_tStartTime			= 0;			// 文件起始时间
	m_dwFileTime			= 0;			// 文件总时间
	m_dwFileTotalFrames		= 0;			// 文件总帧数
	m_dwCurrentFrameRate	= 0;			// 当前帧率
	m_dwPlayedTime			= 0;			// 已播放时间
	m_dwPlayedFrames		= 0; 			// 已解码的视频帧数
	m_dwLastVideoTime		= 0;			// 上一视频帧的时间
	m_bHasFirstIFrame		= false;		// 是否有第一个I帧

	if ( m_lStartOffset<iMinOffset ) m_lStartOffset=iMinOffset;
	if ( m_lStartOffset>=0 ) SeekFile(m_lStartOffset,SEEK_SET);

	// 快速计算数据信息 begin
	iPos = TellFile();
	iRead = ReadFrame(true);
	// 支持移动I帧功能 begin
	m_bInitCurIFrame	= false;	// 当前I帧位置是否有效
	// 支持移动I帧功能 end
	while (iRead>0)
	{
		// 下一帧
		if ( READER_FRAME_I==GetFrameType()||READER_FRAME_P==GetFrameType() )
		{
			if ( m_tStartTime==0 ) m_tStartTime = GetFrameTime();
			m_dwFileTotalFrames++;			// 文件总帧数
			if ( READER_FRAME_I==GetFrameType() )
			{
				stIFrame.m_dwMTime	= GetFrameMTime();
				stIFrame.m_tTime	= GetFrameTime();
				stIFrame.m_lPos		= iPos;
				m_ltIFrames.push_back(stIFrame);
			}
		}
		iPos = TellFile();
		iRead = ReadFrame(true);
	}
	m_lFileSize				= TellFile();				// 文件大小
	m_dwFileTime			= m_dwPlayedTime;			// 文件总时间等于已经播放的时间
	// 快速计算数据信息 end

	// 恢复原始状态
	SeekFile(m_lStartOffset,SEEK_SET);
	m_dwCurrentFrameRate	= 0;			// 当前帧率
	m_dwPlayedTime			= 0;			// 已播放时间
	m_dwPlayedFrames		= 0; 			// 已解码的视频帧数
	m_dwLastVideoTime		= 0;			// 上一视频帧的时间
	m_bHasFirstIFrame		= false;		// 是否有第一个I帧
	
	return true;
}

// 支持移动I帧功能 begin
// 读取前/后一I帧
int CJRecFile::ReadIFrame(bool bNext)
{
	list<stJRecIFrame>::iterator itor;
	PSTJRecIFrame	pstIFrame		= NULL;
	PSTJRecIFrame	pstIFrameOld	= NULL;
	long			lSetPos			= 0;
	int				iRet			= 0;
	time_t			tTime			= GetFrameTime();

	if ( IsOpen()==false ) return -1;

	if ( m_bInitCurIFrame==false )
	{
		// 未准备好当前I帧位置,搜索之
		for (itor = m_ltIFrames.begin(); itor!=m_ltIFrames.end(); itor++)
		{
			pstIFrame = &(*itor);
			if ( pstIFrame )
			{
				if ( pstIFrameOld==NULL ) pstIFrameOld=pstIFrame;
				if ( tTime>=(pstIFrameOld->m_tTime) && tTime<=(pstIFrame->m_tTime) )
				{
					lSetPos = pstIFrameOld->m_lPos;
					break;
				}
				pstIFrameOld=pstIFrame;
			}
		}
		if ( lSetPos>0 ) 
		{
			// 找到当前I帧
			m_bInitCurIFrame	= true;
			m_itorCurIFrame		= itor;
		}
		else
		{
			m_bInitCurIFrame	= false;
			ReaderTrace("CJRecFile::ReadIFrame:Find I Frame error.set=%ld\r\n",tTime);
			return -1;
		}
	}
	
	if ( bNext )
	{
		// 后移
		m_itorCurIFrame++;
		if ( m_itorCurIFrame==m_ltIFrames.end() )
		{
			m_bInitCurIFrame	= false;
			ReaderTrace("CJRecFile::ReadIFrame:Find Next I Frame error.set=%ld\r\n",tTime);
			return -1;
		}
	}
	else
	{
		// 前移
		if ( m_itorCurIFrame==m_ltIFrames.begin() )
		{
			m_bInitCurIFrame	= false;
			ReaderTrace("CJRecFile::ReadIFrame:Find Next I Frame error.set=%ld\r\n",tTime);
			return -1;
		}
		m_itorCurIFrame--;
	}

	// 移动文件
	pstIFrame = &(*m_itorCurIFrame);
	m_dwLastVideoTime = 0;
	iRet = SeekFile(pstIFrame->m_lPos,SEEK_SET);
	if (iRet>=0)
	{
		ReaderTrace("CJRecFile::ReadIFrame:set=%ld,get=%ld.\r\n", lSetPos,TellFile());
		return ReadFrame(false);
	}
	ReaderTrace("CJRecFile::ReadIFrame:seek pos =%ld ERROR.\r\n", lSetPos);
	return iRet;
}
// 支持移动I帧功能 end

// 获取帧头
int CJRecFile::ReadFrame(bool bSkip)
{
	int		iRet			= 0;
	int		iRead			= 0;
	DWORD	dwFrameTime		= 0;
	int		iFrameType		= 0;

	if ( m_pFMRead==NULL ) return -1;
	if ( !IsOpen() ) return -1;

	// 如果还没找到第一个I帧,应该丢弃先前的P帧
	do 
	{
		iRet = 0;
		// 读入一帧
	//	ReaderTrace("ReadFrame :%ld.\r\n", ftell(m_pFile));
		iRead = ReadOneFrame(bSkip);
		if ( iRead<=0 ) return -1;
		iRet+=iRead;
		iFrameType = GetFrameType();
		
		// 设置第一个I帧的位置
		if ( !m_bHasFirstIFrame )
		{
			if ( READER_FRAME_I==iFrameType )
			{
				m_bHasFirstIFrame = true;
			}
			else
			{
				// 下一帧
				continue;
			}
		}

		if ( READER_FRAME_I==iFrameType||READER_FRAME_P==iFrameType )
		{
			m_dwPlayedFrames++; 			// 已解码的视频帧数
			dwFrameTime = (DWORD)(m_pFMRead->GetFrameMTime());
			if ( m_dwLastVideoTime==0 )
			{
				m_dwLastVideoTime	= dwFrameTime;
				m_dwPlayedTime		= 0;	// 已播放时间
				m_dwCurrentFrameRate= 0;	// 当前帧率
				m_dwFrameSpan		= 0;	// 视频帧间隔
			}
			else
			{
				// 已播放时间
 				if ( long(dwFrameTime-m_dwLastVideoTime)<1000 )
 				{
					m_dwFrameSpan	= dwFrameTime-m_dwLastVideoTime;
 				}
				m_dwPlayedTime		+= m_dwFrameSpan;
				// 当前帧率
				if ( dwFrameTime>m_dwLastVideoTime )
				{
					m_dwCurrentFrameRate= 1000/(dwFrameTime-m_dwLastVideoTime);
				}
			}
			m_dwLastVideoTime = dwFrameTime;								// 上一视频帧的时间
		}
		else
		{
			// 音频帧则跳出
			break;
		}
	} while(!m_bHasFirstIFrame);
	return iRet;
}

// 获取帧长
int CJRecFile::GetFrameLen()
{
	if ( m_pFMRead==NULL ) return -1;

	return m_pFMRead->GetFrameLen();
}

// 获取帧类型
int CJRecFile::GetFrameType()
{
	if ( m_pFMRead==NULL ) return -1;

	return m_pFMRead->GetFrameType();
}

// 获取帧时间戳(毫秒类型)
LONGLONG CJRecFile::GetNextFrameMTime()
{
	int		iRead		= 0;
	int		iHeadLen	= 0;
	int		iFrameLen	= 0;
	
	if ( m_pFMRead==NULL || m_pFile==NULL ) return 0;
	
	iHeadLen = m_pFMRead->GetFrameHeadLength();
	// 读入包头 begin
	iRead = ReadFile(m_pFMRead->m_pHeadBuf,1,iHeadLen);
	if ( iRead<=0 ) return 0;
	SeekFile(-iRead,SEEK_CUR);
	if ( m_pFMRead->IsFrameValid()==false || iRead<iHeadLen )
	{
		return 0;
	}
	iFrameLen = m_pFMRead->GetFrameLen();
	if ( iFrameLen<=0 )
	{
		return 0;
	}
	// 读入包头 end

	return (m_pFMRead->GetFrameMTime());
}

// 获取帧时间戳(秒类型)
time_t CJRecFile::GetFrameTime()
{
	if ( m_pFMRead==NULL ) return -1;

	return (time_t)(m_pFMRead->GetFrameTime());
}

// 获取帧时间戳(毫秒类型)
DWORD CJRecFile::GetFrameMTime()
{
	if ( m_pFMRead==NULL ) return -1;

	return (DWORD)(m_pFMRead->GetFrameMTime());
}

// 获取写入帧的时间戳(秒类型)
time_t CJRecFile::GetWriteTime()
{
	if ( m_pFMRead==NULL ) return -1;

	return (DWORD)(m_pFMRead->GetWriteTime());
}

// 获取帧时间戳的差
DWORD CJRecFile::GetFrameSpan()
{
	if ( m_pFMRead==NULL ) return -1;

	return m_dwFrameSpan;
}

// 获取帧数据
BYTE* CJRecFile::GetFrameData()
{
	if ( m_pFMRead==NULL ) return NULL;

	return m_pFMRead->GetFrameData();
}

// 获取帧中的流数据
BYTE* CJRecFile::GetStreamData()
{
	if ( m_pFMRead==NULL ) return NULL;

	return m_pFMRead->GetStreamData();
}

// 获取帧中的流数据长度
int CJRecFile::GetStreamLen()
{
	if ( m_pFMRead==NULL ) return -1;

	return m_pFMRead->GetStreamLen();
}

// 获取OSD
LPCTSTR CJRecFile::GetFrameOSD()
{
	if ( m_pFMRead==NULL ) return NULL;

	return m_pFMRead->GetFrameOSD();
}
