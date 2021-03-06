// JNVPlayer.cpp: implementation of the CJNVPlayer class.
//
//////////////////////////////////////////////////////////////////////

#include "JNVPlayer.h"
#undef	LOG_TAG
#define	LOG_TAG		"JNVPlayer.cpp"

#ifdef __ANDROID__
extern JavaVM *g_jvm;
#endif

#define	JAV_MAX_DELAY_MS		200					// 最大延时
#define	JAV_CAN_READ_REAL		1					// 实时播放下可读大小
#define	JAV_POOL_SIZE_REAL		60					// 实时播放下缓存池大小
#define	JAV_CAN_READ_SMOOTH		20					// 流畅播放下可读大小
#define	JAV_POOL_SIZE_SMOOTH	60					// 流畅播放下缓存池大小
#define JAV_POOL_ELE_SIZE		1024*200			// 初始大小200K

//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////

CJNVPlayer::CJNVPlayer()
{
	strcpy_s(m_tcReadFile.m_szName,J_DGB_NAME_LEN,"m_tcReadFile");	// 名称
	m_tcReadFile.SetOwner(this);					// 播放线程
	m_tcReadFile.SetParam(this);					// 播放线程
	strcpy_s(m_tcDec.m_szName,J_DGB_NAME_LEN,"m_tcDec");			// 名称
	m_tcDec.SetOwner(this);							// 解码线程
	m_tcDec.SetParam(this);							// 解码线程

	#ifdef __ANDROID__
	m_pDecEnv				= NULL;					// JAVA环境指针
	m_arDecBuf				= NULL;					// JNI中的BUF
	m_arDecAudioBuf			= NULL;					// JNI中的BUF
	m_lpAudioFrame			= NULL;					// 当前帧的BUFF
	#endif

	m_iID					= -1;					// ID
	m_iImgRGBFM				= JDRAW_RGB_565;		// 解码后图像RGB格式
	m_lImgWidth				= 0;					// 绘制的帧宽
	m_lImgHeight			= 0;					// 绘制的帧高
	m_dwProtocol			= READER_T_JNV;			// 数据协议
	m_lpDecoder				= NULL;					// 解码适配器
	m_pReaderFrame			= CFMReader::GenReder(m_dwProtocol);	// 格式适配器
	m_pReaderPut			= CFMReader::GenReder(m_dwProtocol);	// 格式适配器
	m_bpWaitDec.SetUserID(m_iID);					// 池的ID
	m_bAccecpPutIn			= false;				// 是否接受输入
	
	m_eStreamType			= AVPStreamReal;		// 流类型
	m_ePlayStatus			= AVPPlayStatusStop;	// 播放状态
	m_lFrameTime			= 0;					// 帧时间
	m_lLastFrameID			= 0;					// 上一帧的帧号
	m_ePlaySpeed			= AVPPlaySpeed_1X;		// 播放速度
	m_iDecBufCount			= 0;					// 解码缓存大小
	m_iDecBufPoolSize		= 0;					// 缓存池大小

	m_pcbDecFrame			= NULL;					// 解码回调函数
	memset(&m_stDecFrameCB,0,sizeof(stDecFrameCB));	// 解码回调时的回调参数
	m_stDecFrameCB.iPort	= m_iID;
	m_lpFrame				= NULL;					// 当前帧的BUFF
	m_lFrameBufLen			= 0;					// 当前帧缓存的大小
	m_bNeedIFrame			= TRUE;					// 需要I帧

	m_ePFMode				= PORT_PF_NONE;			// 单帧类型
	m_iPFSlider				= 0;					// 单帧运行时的滑行数

	m_lRead					= 0;					// 读入数
	m_lDec					= 0;					// 解码数
	
	m_tPlayedTime			= 0;					// 播放了的时间
	m_pfRec					= NULL;					// 录像文件
	m_pcbPlayInfo			= 0;					// 播放信息回调
	m_lPlayInfoParam		= 0;					// 播放信息回调参数
	m_bPause				= false;

	m_mutexStop.CreateMutex();
}

CJNVPlayer::~CJNVPlayer()
{
	Release();
	SAFE_DELETE(m_pReaderFrame);
	SAFE_DELETE(m_pReaderPut);
}

// 释放
void CJNVPlayer::Release()
{
	Stop();
	m_iID = -1;
	m_bpWaitDec.Clear();
}
// 获取ID
int CJNVPlayer::GetID()
{
	return m_iID;
}
// 设置ID
int CJNVPlayer::SetID(int iID)
{
	m_iID = iID;
	return m_iID;
}

// 加入一帧
int CJNVPlayer::PutFrame(void *pFrame)
{
	PSTBPElement	pstEle		= NULL;
	int				iLen		= 0;
//	LONGLONG		lFrameTime	= 0;
	bool			bLock		= false;

	try
	{
		if ( m_bAccecpPutIn==false || m_pReaderPut==NULL )
		{
			return JNETErrUnInit;
		}

		iLen = m_pReaderPut->GetFrameLen(pFrame);
		if ( iLen<=0 )
		{
			return JNETErrParam;
		}
		m_mutexStop.Lock();
		if ( m_pfRec )
		{
			fwrite(pFrame,iLen,1,m_pfRec);
		}
		m_mutexStop.Unlock();
		if ( (m_ePlaySpeed!=AVPPlaySpeed_1X && m_eStreamType==AVPStreamReal) )
 		{
			// 实时优先
			if ( m_mutexStop.Lock()==true ) bLock = true;
			else bLock = false;
 			DecBuff((const BYTE*)pFrame,iLen,false);
 			if ( bLock ) m_mutexStop.Unlock();
 			return iLen;
 		}
 		else
		{
			if ( m_pReaderPut->SetFrameData((const BYTE*)pFrame,iLen)>0 )
			{
				pstEle = m_bpWaitDec.BeginAddBuff(pFrame,iLen,0,0);
				if ( pstEle )
				{
					m_lRead++;
					m_bpWaitDec.EndAddBuff();
					return iLen;
				}
			}
		}

		return JNETErrBuffSize;
	}
	catch(...)
	{
		DbgStrOut("try-catch CJNVPlayer::PutFrame\r\n");
	}
	return JNETErrOperate;
}

// 解码图像的格式:JDRAW_RGB_32,JDRAW_RGB_24,JDRAW_RGB_565
int CJNVPlayer::SetDecRGBFM(int iRGBFM)
{
	if ( iRGBFM==JDRAW_RGB_NONE )
	{
	}
	else if ( iRGBFM==JDRAW_RGB_32 )
	{
	}
	else if ( iRGBFM==JDRAW_RGB_24 )
	{
	}
	else if ( iRGBFM==JDRAW_RGB_565 )
	{
	}
	else
	{
		return JNETErrOperate;
	}

	m_iImgRGBFM	= iRGBFM;					// 解码后图像RGB格式
	return JNETErrSuccess;
}

// 设置解码回调函数
int CJNVPlayer::SetDecCallBack(fcbDecFrame* pFun, long lParam)
{
	bool	bLock	= false;

	if ( m_mutexStop.Lock() ) bLock = true;
	else bLock = false;
	m_pcbDecFrame	= pFun;					// 解码回调函数
	m_stDecFrameCB.lParam	= lParam;		// 解码回调时的回调参数
	if (bLock ) m_mutexStop.Unlock();
	return JNETErrSuccess;
}

// 设置播放信息回调函数
int CJNVPlayer::SetPlayInfoCallBack(fcbPlay* pFun, long lParam)
{
	bool	bLock	= false;

	if ( m_mutexStop.Lock() ) bLock = true;
	else bLock = false;
	m_pcbPlayInfo			= pFun;			// 播放信息回调
	m_lPlayInfoParam		= lParam;		// 播放信息回调参数
	if (bLock ) m_mutexStop.Unlock();
	return JNETErrSuccess;
}

void CJNVPlayer::SendCallBackMsg(AVPPlayCBMsg eMsg)
{
	if ( m_pcbPlayInfo==NULL ) return;
	
	int				iPlayerID	= GetID();
	long			lpUserParam	= m_lPlayInfoParam;
	__int64			lParam1		= 0;
	__int64			lParam2		= 0;
	DWORD			dwRes		= 0;
	
	
	#ifdef __ANDROID__
	dwRes = (DWORD)m_pDecEnv;
	#endif

	switch(eMsg)
	{
	case AVPPlayCBUnknow	:
		// 未知消息
		break;
	case AVPPlayCBError		:
		// 通知 错误 消息,返回值无具体意义
		break;
	case AVPPlayCBStart		:
		// 通知 开始播放 消息,返回值无具体意义,lParam=文件的开始时间
		lParam1	= m_recFile.GetStartTime();
		lParam2	= lParam1+m_recFile.GetFileTime();
		break;
	case AVPPlayCBEnd		:
		// 已经弃用的消息
		break;
	case AVPPlayCBFrame		:
		// 通知 帧时间发生变化(秒) 消息,返回值无具体意义,lParam=当前时间
		lParam1	= m_tPlayedTime;
		break;
	case AVPPlayCBStop		:
		// 通知 结束播放 消息,返回值无具体意义,lParam=空
		break;
	case AVPPlayCBPause		:
		// 通知 继续播放 消息,返回值无具体意义,lParam=文件已播放时间
		lParam1	= m_recFile.GetPlayedTime();
		break;
	case AVPPlayCBContinue	:
		// 通知 继续播放 消息,返回值无具体意义,lParam=文件已播放时间
		lParam1	= m_recFile.GetPlayedTime();
		break;
	case AVPPlayCBSpeed		:
		// 通知 播放速度 消息,返回值无具体意义,lParam=当前播放速度
	//	lParam1	= GetPlaySpeed();
		break;
	default:
		eMsg	= AVPPlayCBUnknow;
		break;
	}

	m_pcbPlayInfo(eMsg,iPlayerID,lpUserParam,lParam1,lParam2,dwRes);
}

// 打开录像文件
int CJNVPlayer::OpenFile(LPCTSTR lpszPath)
{
	JLOG_TRY

	if ( m_recFile.Open(lpszPath,0)==0 )
	{
		m_tPlayedTime	= m_recFile.GetStartTime();	// 播放了的时间
		m_eStreamType	= AVPStreamFile;			// 流类型
		return JNETErrSuccess;
	}
	else
	{
		return JNETErrOpenFile;
	}
	JLOG_CATCH("try-catch CJAVPlayer::OpenFile\r\n");
	return JNETErrOpenFile;
}

// 播放
int	CJNVPlayer::Play()
{
	if ( m_ePlayStatus==AVPPlayStatusIdle )
	{
	//	m_tcReadFile.ContinueThread();
		m_ePFMode		= PORT_PF_NONE;			// 单帧方式
		m_ePlayStatus	= AVPPlayStatusRun;		// 正在播放
		m_bAccecpPutIn	= true;					// 是否接受输入
		m_bPause		= false;
		if ( m_eStreamType==AVPStreamFile )
		{
			m_tcReadFile.ContinueThread();
		}
		else
		{
			m_tcDec.ContinueThread();
		}
	}
	else if ( m_ePlayStatus==AVPPlayStatusStop )
	{
		m_bPause		= false;
		m_bNeedIFrame		= true;				// 需要I帧
		m_lRead				= 0;				// 读入数
		m_lDec				= 0;				// 解码数
		m_ePFMode			= PORT_PF_NONE;		// 单帧方式
		m_lFrameTime		= 0;				// 重置帧时间
		m_tPlayedTime		= 0;				// 播放了的时间
		if ( m_lpDecoder==NULL ) m_lpDecoder = new CDecoder;			// 解码适配器
		if ( m_eStreamType==AVPStreamFile )
		{
			m_iDecBufCount		= JAV_CAN_READ_SMOOTH;					// 解码缓存大小
			m_iDecBufPoolSize	= JAV_POOL_SIZE_SMOOTH;					// 缓存池大小
			m_bpWaitDec.SetSize(m_iDecBufPoolSize,m_iDecBufCount,JAV_POOL_ELE_SIZE);		// 需要重置池,使其可写入
			// 如果是文件回放,则需要启动读文件线程
			m_tcReadFile.StartThread(RunReadFileThread);
		}
		else
		{
			// 优先方式:实时
			m_iDecBufCount		= JAV_CAN_READ_REAL;				// 解码缓存大小
			m_iDecBufPoolSize	= JAV_POOL_SIZE_REAL;				// 缓存池大小
			m_bpWaitDec.SetSize(m_iDecBufPoolSize,m_iDecBufCount,JAV_POOL_ELE_SIZE);	// 需要重置池,使其可写入
			m_tcDec.StartThread(RunDecThread);
		}
		m_ePlayStatus	= AVPPlayStatusRun;		// 正在播放
		m_bAccecpPutIn	= true;					// 是否接受输入
	}
	else
	{
		// AVPPlayStatusRun
	}

	return JNETErrSuccess;
}

// 停止
int	CJNVPlayer::Stop()
{
	JLOG_TRY
	bool	bLock	= false;

//	DbgStrOut("%d CJAVPlayer::Stop()...start.\r\n",m_iID);
	m_bAccecpPutIn	= false;				// 是否接受输入
	if ( m_mutexStop.Lock()==true ) bLock = true;
	else bLock = false;
	if ( m_ePlayStatus==AVPPlayStatusStop )
	{
		if ( bLock ) m_mutexStop.Unlock();
	//	DbgStrOut("%d CJAVPlayer::Stop()...stop.\r\n",m_iID);
		return JNETErrSuccess;
	}
	m_ePlayStatus	= AVPPlayStatusStop;	// 停止播放
	if ( bLock ) m_mutexStop.Unlock();

	DbgStrOut("%d CJNVPlayer::Stop().\r\n",m_iID);
//	m_bpWaitDec.StopAdd();

	m_tcDec.StopThread();
	m_tcReadFile.StopThread();

	m_bpWaitDec.Clear();					// 需要先清空池,使线程可退出
	
	m_eStreamType	= AVPStreamReal;		// 流类型
	PrepareFrameBuff(NULL);
	m_lImgWidth		= 0;					// 绘制的帧宽
	m_lImgHeight	= 0;					// 绘制的帧高
	m_bPause		= false;

	SAFE_DELETE(m_lpDecoder);

	return JNETErrSuccess;
	JLOG_CATCH("try-catch CJNVPlayer::Stop\r\n");
	return JNETErrOperate;
}

// 暂停
int	CJNVPlayer::Pause()
{
	if ( m_ePlayStatus==AVPPlayStatusRun )
	{
		if ( m_eStreamType == AVPStreamFile )
		{
			JTRACE("CJNVPlayer::Pause m_bAccecpPutIn=%d,m_bPause=%d  begin \r\n",m_bAccecpPutIn,m_bPause);
			m_bPause = true;
			m_ePlayStatus = AVPPlayStatusIdle;	// 暂停播放
			SendCallBackMsg(AVPPlayCBPause);
			m_tcReadFile.PauseThread();
			JTRACE("CJNVPlayer::Pause m_bAccecpPutIn=%d,m_bPause=%d  end \r\n",m_bAccecpPutIn,m_bPause);
		}
		else
		{
			m_bAccecpPutIn	= false;			// 是否接受输入
			m_ePlayStatus = AVPPlayStatusIdle;	// 暂停播放
			SendCallBackMsg(AVPPlayCBPause);
			m_tcDec.PauseThread();
		}
	}
	return JNETErrSuccess;
}
// 设置播放速度
int	CJNVPlayer::SetPlaySpeed(int iSpeed)
{
	JTRACE("CJNVPlayer::SetPlaySpeed %d,%d,%d\r\n",m_iID,m_ePlaySpeed,iSpeed);
	switch(iSpeed)
	{
		case AVPPlaySpeed_1_16X:	m_ePlaySpeed = AVPPlaySpeed_1_16X;	break;
		case AVPPlaySpeed_1_8X:		m_ePlaySpeed = AVPPlaySpeed_1_8X;	break;
		case AVPPlaySpeed_1_4X:		m_ePlaySpeed = AVPPlaySpeed_1_4X;	break;
		case AVPPlaySpeed_1_2X:		m_ePlaySpeed = AVPPlaySpeed_1_2X;	break;
		case AVPPlaySpeed_1X:		m_ePlaySpeed = AVPPlaySpeed_1X;		break;
		case AVPPlaySpeed_2X:		m_ePlaySpeed = AVPPlaySpeed_2X;		break;
		case AVPPlaySpeed_4X:		m_ePlaySpeed = AVPPlaySpeed_4X;		break;
		case AVPPlaySpeed_8X:		m_ePlaySpeed = AVPPlaySpeed_8X;		break;
		case AVPPlaySpeed_16X:		m_ePlaySpeed = AVPPlaySpeed_16X;	break;
		case AVPPlaySpeed_32X:		m_ePlaySpeed = AVPPlaySpeed_32X;	break;

		case AVPPlaySpeed_0_1X:		m_ePlaySpeed = AVPPlaySpeed_0_1X;	break;
		case AVPPlaySpeed_0_2X:		m_ePlaySpeed = AVPPlaySpeed_0_2X;	break;
		case AVPPlaySpeed_0_4X:		m_ePlaySpeed = AVPPlaySpeed_0_4X;	break;
		case AVPPlaySpeed_0_8X:		m_ePlaySpeed = AVPPlaySpeed_0_8X;	break;
		case AVPPlaySpeed_0_16X:	m_ePlaySpeed = AVPPlaySpeed_0_16X;	break;
		case AVPPlaySpeed_0_32X:	m_ePlaySpeed = AVPPlaySpeed_0_32X;	break;

		default:					m_ePlaySpeed = AVPPlaySpeed_1X;		break;
	}
 	if (m_ePlaySpeed!=AVPPlaySpeed_1X && m_eStreamType==AVPStreamReal)
	{
		m_bpWaitDec.Reset();
	}
	SendCallBackMsg(AVPPlayCBSpeed);
	return m_ePlaySpeed;
}
// 获取当前播放速度
int	CJNVPlayer::GetPlaySpeed()
{
	return m_ePlaySpeed;
}
// 下一帧
int	CJNVPlayer::NextFrame()
{
	if ( m_ePlayStatus==AVPPlayStatusRun )
	{
		// 启用暂停,使维护线程起作用.
		Pause();
	}
	m_ePFMode		= PORT_PF_NEXT;
	m_bAccecpPutIn	= true;
	if ( m_eStreamType==AVPStreamFile )
	{
		m_tcReadFile.ContinueThread();
	}
	else
	{
		m_tcDec.ContinueThread();
	}
	SendCallBackMsg(AVPPlayCBFrame);

	return JNETErrSuccess;
}
// 设置播放的时间
int CJNVPlayer::Seek(long lTime)
{
	int		iRet	= 0;
	bool	bLock	= false;
	if ( m_recFile.IsOpen()==false ) return JNETErrUnInit;

	if ( m_mutexStop.Lock() ) bLock = true;
	else bLock = false;

	iRet = m_recFile.Seek(lTime+m_recFile.GetStartTime());
	m_lFrameTime = m_recFile.GetNextFrameMTime();
	if ( m_ePlayStatus==AVPPlayStatusIdle )
	{
		// 暂停的情况下,SEEK完后,播放当前帧
		m_ePFMode		= PORT_PF_NEXT;
		m_iPFSlider		= 2;	// 单帧运行时的滑行数
		m_bAccecpPutIn	= true;
		m_tcReadFile.ContinueThread();
		SendCallBackMsg(AVPPlayCBFrame);
	}
	if ( iRet>=0 )
	{
	//	m_sound.ResetOutPos();
	}

	if ( bLock ) m_mutexStop.Unlock();

	return iRet;
}

// 抓图
int CJNVPlayer::Capture(LPCTSTR lpszPath)
{
	int		i					= 0;
	char	szPath[MAX_PATH]	= {0};
	FILE*	pfCapture			= NULL;
	LPBYTE	lpRGBFrame			= NULL;
//	long	lFrameBuffSize		= 0;
	BITMAPFILEHEADER 	bmpFileHeader;			// 位图头
	BITMAPINFO			bmpinfo;				// 位图文件头
	int		iRGBDeep			= 0;
	int		iBitCount			= 0;

	if ( m_lpFrame==NULL ) return JNETErrUnInit;

	if ( m_iImgRGBFM==JDRAW_RGB_32 )		iRGBDeep = 4;
	else if ( m_iImgRGBFM==JDRAW_RGB_24 )	iRGBDeep = 3;
	else if ( m_iImgRGBFM==JDRAW_RGB_565 )	iRGBDeep = 2;
	else iRGBDeep = 4;
	iBitCount			= 8*iRGBDeep;

	// 直接用
	lpRGBFrame	= m_lpFrame;

	// 创建位图头部 begin
	memset(&bmpinfo, 0, sizeof(BITMAPINFO));
	bmpinfo.bmiHeader.biSize		= sizeof(BITMAPINFOHEADER);
	bmpinfo.bmiHeader.biWidth		= m_lImgWidth;
	bmpinfo.bmiHeader.biHeight		= m_lImgHeight;
	bmpinfo.bmiHeader.biPlanes		= 1;
	bmpinfo.bmiHeader.biBitCount	= iBitCount;
	bmpinfo.bmiHeader.biCompression	= BI_BITFIELDS;
	bmpinfo.bmiHeader.biSizeImage	= ((m_lImgWidth*iRGBDeep*8+(iBitCount-1))&~(iBitCount-1))/8 * m_lImgHeight;
	bmpinfo.bmiColors[0].rgbBlue	= 0xF8;
	bmpinfo.bmiColors[0].rgbGreen	= 0x7e;
	bmpinfo.bmiColors[0].rgbRed		= 0x1F;
	bmpinfo.bmiColors[0].rgbReserved= 0;
	// 创建位图头部 end
	
	// 创建位图头部 begin
	bmpFileHeader.bfType		= 0x4d42;		// 'BM'
	bmpFileHeader.bfSize		= bmpinfo.bmiHeader.biSizeImage;
	bmpFileHeader.bfReserved1	= 0;
	bmpFileHeader.bfReserved2	= 0;
	bmpFileHeader.bfOffBits		= (DWORD)sizeof(bmpFileHeader) + (DWORD)sizeof(bmpinfo);
	// 创建位图头部 begin

	// 创建抓拍图片 begin
	sprintf_s(szPath,MAX_PATH,"%s",lpszPath);
	pfCapture = fopen(szPath,"wb");
	if ( pfCapture==NULL ) return JNETErrCreateFile;
	// 创建抓拍图片 end

	// 写入内容 begin
	fwrite(&bmpFileHeader, sizeof(BITMAPFILEHEADER), 1, pfCapture);		// 写文件头
	fwrite(&bmpinfo, sizeof(BITMAPINFO),  1, pfCapture);				// 写位图头
	for(i=0; i < bmpinfo.bmiHeader.biHeight; i++)
	{
		// 写位图信息
		fwrite(lpRGBFrame+(bmpinfo.bmiHeader.biHeight-i-1)*iRGBDeep*bmpinfo.bmiHeader.biWidth,
			iRGBDeep*bmpinfo.bmiHeader.biWidth, 1, pfCapture);
	}
	// 写入内容 end
	
	// 关闭文件 begin
	fclose(pfCapture);
	pfCapture = NULL;
	// 关闭文件 end

	return JNETErrSuccess;
}

// 开始录像
long CJNVPlayer::RecStart(LPCTSTR lpszPath)
{
	if ( m_pfRec ) return JNETErrSuccess;						// 录像文件
	m_mutexStop.Lock();
	m_pfRec = fopen(lpszPath,"wb");
	m_mutexStop.Unlock();
	if ( m_pfRec ) return JNETErrSuccess;
	else return JNETErrCreateFile;
}

// 停止录像
long CJNVPlayer::RecStop()
{
	if ( m_pfRec==NULL ) return JNETErrSuccess;					// 录像文件
	m_mutexStop.Lock();
	fclose(m_pfRec);
	m_pfRec = NULL;
	m_mutexStop.Unlock();
	return JNETErrSuccess;
}

// 播放文件的动作
void CJNVPlayer::ReadFileAction()
{
	BYTE*	pFrame		= NULL;
	int		iLen		= 0;
	bool	bLock		= false;
	

	JLOG_TRY
	DWORD	dwTick		= JGetTickCount();
	while(m_bAccecpPutIn)
	{
		if (m_eStreamType == AVPStreamFile && m_bPause)
		{
			JSleep(200);
			continue;
		}

		if ( m_mutexStop.Lock() ) bLock = true;
		else bLock = false;
		if ( m_ePlaySpeed>=AVPPlaySpeed_0_1X )
		{
			iLen = m_recFile.ReadIFrame(false);
		}
		else
		{
			iLen = m_recFile.ReadFrame(false);
		}
		if (iLen<0)
		{
			break;
		}
		else
		{
			pFrame			= m_recFile.GetFrameData();
			iLen			= m_recFile.GetFrameLen();
			DecBuff(pFrame,iLen,true);
			m_lRead++;
			if ( bLock ) m_mutexStop.Unlock();
		}
	}
	m_recFile.Close();
	DbgStrOut("%d ReadFileAction()::end [%ld]\r\n",m_iID,m_lRead);
//	Sleep(1100);
	m_tcReadFile.StopThread(false);
	Stop();
	if ( bLock ) m_mutexStop.Unlock();
	JTRACE("CJNVPlayer::ReadFileActio Span %d\r\n",int(JGetTickCount()-dwTick));
	JLOG_CATCH("try-catch CJAVPlayer::ReadFileAction \r\n");
}

// 处理解码的动作
bool CJNVPlayer::DecAction()
{
	JLOG_TRY
	PSTBPElement	pstEle	= NULL;
	PSTBPElement	pstEleV	= NULL;
	bool			bRet	= false;
	bool			bLock	= false;

	pstEle = m_bpWaitDec.BeginGetBuff();
	if ( pstEle )
	{
		pstEleV		= pstEle;
		m_bpWaitDec.EndGetBuff();

		if ( m_mutexStop.Lock()==true ) bLock = true;
		else bLock = false;
		bRet = DecBuff(pstEleV->m_pBuf,pstEleV->m_iBufSize,true);
		if ( bLock ) m_mutexStop.Unlock();
		if ( pstEleV->m_iMaxSize>JAV_POOL_ELE_SIZE )
		{
			JTRACE("CJNVPlayer::DecAction() free :%ld\r\n",pstEleV->m_iMaxSize);
			pstEleV->Free();
		}
		return bRet;
	}
	else
	{
		JSleep(5);
	}
	return true;
	JLOG_CATCH("try-catch CJNVPlayer::DecAction \r\n");
	return false;
}

// 获取速度值
float CJNVPlayer::GetSpeedValue()
{
	switch(m_ePlaySpeed)
	{
		case AVPPlaySpeed_1_16X:		return 1.0f*16.0f;
		case AVPPlaySpeed_1_8X:			return 1.0f*8.0f;
		case AVPPlaySpeed_1_4X:			return 1.0f*4.0f;
		case AVPPlaySpeed_1_2X:			return 1.0f*2.0f;
		case AVPPlaySpeed_1X:			return 1.0f;
		case AVPPlaySpeed_2X:			return 1.0f/2.0f;
		case AVPPlaySpeed_4X:			return 1.0f/4.0f;
		case AVPPlaySpeed_8X:			return 1.0f/8.0f;
		case AVPPlaySpeed_16X:			return 1.0f/16.0f;
		case AVPPlaySpeed_32X:			return 1.0f/32.0f;
			
		case AVPPlaySpeed_0_1X:			return 1.0f;
		case AVPPlaySpeed_0_2X:			return 1.0f/2.0f;
		case AVPPlaySpeed_0_4X:			return 1.0f/4.0f;
		case AVPPlaySpeed_0_8X:			return 1.0f/8.0f;
		case AVPPlaySpeed_0_16X:		return 1.0f/16.0f;
		case AVPPlaySpeed_0_32X:		return 1.0f/32.0f;
		default:						return 1.0f;
	}
}

// 等待下一帧
void CJNVPlayer::WaitForNextFrame(LONGLONG lFrameMTime)
{
	int		iFrameSpan	= 0;
	int		iWaitMSec	= 0;
	float	fDecSpan	= 0.0f;
	int		iBuffSpan	= 0;
	float	fSpeed		= GetSpeedValue();

//	return ;	// to do
	// 第一帧或当前为单帧,直接就出了
	if (m_lFrameTime==0 || m_ePFMode!=PORT_PF_NONE)
	{
		return;
	}
	else
	{
		fDecSpan = m_jgrtDec.GetTimeSpan();

		// 第N帧,需要先等待
		iFrameSpan		= (int)(lFrameMTime-m_lFrameTime);
		iFrameSpan		= (int)(iFrameSpan*fSpeed);
		iWaitMSec		= iFrameSpan - int(fDecSpan) - 1;

		iBuffSpan = m_bpWaitDec.GetSpan();
		if ( m_eStreamType==AVPStreamFile )
		{
			if ( iWaitMSec<0 )		iWaitMSec = -iWaitMSec;
			if ( iWaitMSec<5 )		iWaitMSec = 5;
			if ( iWaitMSec>2000 )	iWaitMSec = 5;
		//	if (m_bpWaitDec.GetSpan()<(m_iDecBufCount))		iWaitMSec+=2;
		//	if (m_bpWaitDec.GetSpan()>(m_iDecBufCount*2))	iWaitMSec-=2;
		}
		else // if ( m_ePriorityType==AVPPlaySmooth )
		{
			if ( iWaitMSec<5 )					iWaitMSec = 5;
			if ( iWaitMSec>JAV_MAX_DELAY_MS )	iWaitMSec = 5;
			if ( iBuffSpan<(m_iDecBufCount) )	iWaitMSec +=2;
			if ( iBuffSpan>(m_iDecBufCount*2) )	iWaitMSec -=2;
			if ( iBuffSpan>(m_iDecBufCount*3) )	iWaitMSec -=2;
			if ( iBuffSpan>(m_iDecBufCount*4) )	iWaitMSec -=2;
		}

		if ( iWaitMSec>5 )
		{
			JSleep(iWaitMSec);
		}
		else
		{
		//	JSleep(1);
		}
	}
}

#include "Audio/g726.h"
// 解码		
bool CJNVPlayer::DecBuff(const BYTE* pBuf,int iSize,bool bWait)
{
	long			lTemp		= 0;
	long			lTemp1		= 0;
	long			lFrameID	= 0;
	LONGLONG		lFrameMTime	= 0;
	float			fSpeed		= 0.0;

	int				iFrameType	= 0;
	time_t			tDevTime	= 0;
	long			lTotalLen	= 0;		// 视频帧总长
	long			lHasDecLen	= 0;		// 已经解码的长
	
	long			lRet		= 0;      //解码结果

	stYUVFrameAttr	stYUVBuff;				// 上一YUV

	long			lLine		= __LINE__;

	try
	{
		fSpeed	= GetSpeedValue();

		if ( m_ePlayStatus==AVPPlayStatusStop || m_lpDecoder==NULL )
		{
			JSleep(1);
			return false;
		}

		if ( m_pReaderFrame==NULL ) m_pReaderFrame = CFMReader::GenReder(m_dwProtocol);
		if ( m_pReaderFrame->GetReadType()!=m_dwProtocol )
		{
			// 格式变换了,重置之
			SAFE_DELETE(m_pReaderFrame);
			m_pReaderFrame = CFMReader::GenReder(m_dwProtocol);
			if ( m_pReaderFrame==NULL )
			{
				JSleep(1);
				return false;
			}
		}

		lLine		= __LINE__;
		if ( m_pReaderFrame->SetFrameData(pBuf,iSize)>0 )
		{
			iFrameType = m_pReaderFrame->GetFrameType();
			if ( iFrameType==READER_FRAME_A )
			{
				m_lpDecoder->SetMediaType(0,0,-1,m_pReaderFrame->GetAudioFormat());

				LPBYTE	lpSrc		= NULL;
				int		iOutLen		= 0;
				int		iLeftCount	= m_pReaderFrame->GetStreamLen();

				lpSrc	= m_pReaderFrame->GetStreamData();
				while(iLeftCount>=104)
				{
					#ifdef __ANDROID__
					m_lpDecoder->AudioDec(lpSrc+4,100,m_lpAudioFrame,&iOutLen);
					#else
					m_lpDecoder->AudioDec(lpSrc+4,100,m_lpFrame,&iOutLen);
					#endif

					lLine		= __LINE__;
					m_stDecFrameCB.iPort	= m_iID;					// 解码通道号
					m_stDecFrameCB.lpBuf	= m_lpFrame;				// 解码后的数据
					m_stDecFrameCB.lSize	= iOutLen;					// 解码后数据长
					m_stDecFrameCB.lWidth	= 0;						// 宽
					m_stDecFrameCB.lHeight	= 0;						// 高
					m_stDecFrameCB.lStamp	= (long)0;					// 帧时间
					m_stDecFrameCB.lType	= AVPDecCBTypeAudio16;		// AVPDecCBType
					m_stDecFrameCB.dwRes0	= (DWORD)m_pReaderFrame;
					#ifdef __ANDROID__
					m_stDecFrameCB.dwRes1	= (DWORD)m_pDecEnv;
					m_stDecFrameCB.lpBuf	= (LPBYTE)m_arDecAudioBuf;	// 解码后的数据
					m_pDecEnv->ReleaseByteArrayElements(m_arDecAudioBuf,(jbyte*)m_lpAudioFrame,JNI_COMMIT);  
					#endif
					if ( m_pcbDecFrame==NULL || m_pcbDecFrame(&m_stDecFrameCB)==0 )
					{
						// 不用自己来显示,让应用层在回调里做这个事
					}

					iLeftCount-=104;
					lpSrc+=104;
				}
			}
			else if ( iFrameType==READER_FRAME_I || iFrameType==READER_FRAME_P)
			{
				lLine		= __LINE__;
				lFrameID	= m_pReaderFrame->GetFrameID();
				if ( lFrameID-m_lLastFrameID>1 )
				{
					DbgStrOut("%d DecBuff()::frame lost lastFrameID:[%ld]  thisID:[%ld]\r\n",m_iID,m_lLastFrameID,lFrameID);
					m_bNeedIFrame = true;
				}
				m_lLastFrameID	= lFrameID;
				lLine		= __LINE__;

				lFrameMTime	= m_pReaderFrame->GetFrameMTime();
				if ( bWait ) WaitForNextFrame(lFrameMTime);
				m_lFrameTime = lFrameMTime;			// 帧时间

				// 视频帧
				if ( m_pReaderFrame->GetPictureSize(&lTemp,&lTemp1)==false )
				{
					return true;
				}
				if ( lTemp1==0 || lTemp1==0 )//2个一样的？
				{
					return true;
				}
				if ( m_lImgWidth!=lTemp || m_lImgHeight!=lTemp1 )
				{
					// 重置解码器
					m_lpDecoder->UnInit();
					m_lpDecoder->Init();
					m_bNeedIFrame	= true;
					m_lImgWidth	= lTemp;
					m_lImgHeight= lTemp1;
				}
				lLine		= __LINE__;
				if ( SetImageSize(m_lImgWidth,m_lImgHeight)==false ) return false;	// 重置解码器
				m_lpDecoder->SetMediaType(m_lImgWidth,m_lImgHeight,m_pReaderFrame->GetVideoFormat(),-1);
				lTotalLen = m_pReaderFrame->GetStreamLen();					// 视频帧总长
				lHasDecLen= 0;
				lLine		= __LINE__;
				m_jgrtDec.Start();
				do
				{
					if ( m_bNeedIFrame )
					{
						// 需要先从I帧开始
						if ( iFrameType==READER_FRAME_I )
						{
							m_bNeedIFrame = false;
						}
						else
						{
							DbgStrOut("CJAVPlayer::DecBuff:Need I Frame.\r\n");
							break;
						}
					}
					lLine		= __LINE__;
					lRet = m_lpDecoder->VideoDec(m_pReaderFrame->GetStreamData()+lHasDecLen,
						m_pReaderFrame->GetStreamLen()-lHasDecLen,&stYUVBuff);
					lLine		= __LINE__;
					if ( lRet==0 )
					{
						lHasDecLen = lTotalLen;
						DbgStrOut("%d DecBuff()::id=%ld,statue=%ld,type=%d\r\n",m_iID,lFrameID,lRet,iFrameType);
					}
					else if ( lRet>0 )
					{
						// 解码成功
						lHasDecLen+=lRet;

						tDevTime	= (time_t)(m_pReaderFrame->GetFrameTime());
						lLine		= __LINE__;
						// 送显示 begin
						PrepareFrameBuff(&stYUVBuff);
						lLine		= __LINE__;
						m_stDecFrameCB.dwRes0	= (DWORD)m_pReaderFrame;
						#ifdef __ANDROID__
						m_stDecFrameCB.dwRes1	= (DWORD)m_pDecEnv;
						m_pDecEnv->ReleaseByteArrayElements(m_arDecBuf,(jbyte*)m_lpFrame,JNI_COMMIT);  
						#endif
						if ( m_pcbDecFrame==NULL || m_pcbDecFrame(&m_stDecFrameCB)==0 )
						{
							// 不用自己来显示,让应用层在回调里做这个事
						}
						lLine		= __LINE__;
						// 送显示 end
						lRet = m_lpDecoder->GetRemainPic();
						m_lDec++;
						if ( lHasDecLen<lTotalLen )
						{
							DbgStrOut("%d DecBuff()::left buff lHasDecLen=[%ld],lTotalLen=[%ld].\r\n",m_iID,lHasDecLen,lTotalLen);
						}
					}
					else
					{
						// 不足一帧,等下一包过来再推
						DbgStrOut("%d DecBuff()::err [%ld]\r\n",m_iID,lRet);
						m_bNeedIFrame = true;
						JSleep(1);
						m_lpDecoder->UnInit();
						m_lpDecoder->Init();
					}
					break;
				} while(lHasDecLen<lTotalLen);
				// 单帧类型
				if ( m_ePFMode != PORT_PF_NONE )
				{
					// 单帧运行时的滑行数
					if ( m_iPFSlider>0 ) m_iPFSlider--;
					else
					{
						m_ePlayStatus = AVPPlayStatusIdle;	// 暂停播放
						m_bAccecpPutIn= false;
						SendCallBackMsg(AVPPlayCBPause);
						m_tcReadFile.PauseThread();
						m_jgrtDec.Start();
					}
				}
				m_jgrtDec.End();
			}
		}
	}
	catch(...)
	{
		m_bNeedIFrame = true;
		m_lpDecoder->UnInit();
		m_lpDecoder->Init();
		DbgStrOut("try-catch CJAVPlayer::DecBuff at %ld\r\n",lLine);
	}
	return true;
}

// 设置音频缓存大小
bool CJNVPlayer::PrepareFrameBuffSize(int iLen)
{
	long	lFrameBuffSize	= 0;

	if ( iLen==0 )
	{
		#ifdef __ANDROID__
		if ( m_pDecEnv )
		{
			if ( m_lFrameBufLen>0 )
			{
			//	m_pDecEnv->DeleteLocalRef(m_arDecBuf);
			//	m_pDecEnv->ReleaseByteArrayElements(m_arDecBuf,(jbyte*)m_lpFrame,JNI_ABORT);
				m_lpFrame	= NULL;
			}
		}
		#else
		SAFE_DELETE(m_lpFrame);
		#endif
		m_lFrameBufLen	= 0;						// 当前帧缓存的大小
		return true;
	}
	// 申请缓存 begin
	lFrameBuffSize	= iLen;
	if ( m_lFrameBufLen<lFrameBuffSize )
	{
		DbgStrOut("%d PrepareFrameBuff()::request BuffSize: [%ld]\r\n",m_iID,lFrameBuffSize);
		#ifdef __ANDROID__
		jboolean	isCopy	= JNI_FALSE;
		if ( m_pDecEnv )
		{
			if ( m_lFrameBufLen>0 )
			{
			//	m_pDecEnv->DeleteLocalRef(m_arDecBuf);
			//	m_pDecEnv->ReleaseByteArrayElements(m_arDecBuf,(jbyte*)m_lpFrame,JNI_ABORT);
				m_lpFrame	= NULL;
			}
			m_arDecBuf	= m_pDecEnv->NewByteArray(lFrameBuffSize);			// JNI中的BUF
			m_lpFrame	= (LPBYTE)(m_pDecEnv->GetByteArrayElements(m_arDecBuf,&isCopy));
		}
		#else
		SAFE_DELETE(m_lpFrame);
		m_lpFrame	= new BYTE[lFrameBuffSize];
		#endif
		if ( m_lpFrame )
		{
			m_lFrameBufLen	= lFrameBuffSize;		// 当前帧缓存的大小
		}
		else
		{
			m_lFrameBufLen	= 0;					// 当前帧缓存的大小
		}
	}
	if ( m_lpFrame==NULL )
	{
		return false;
	}
	// 申请缓存 end
	return true;
}

// 设置图像缓存大小
bool CJNVPlayer::PrepareFrameBuff(PSTYUVFrameAttr pstYUV)
{
	long	lFrameBuffSize	= 0;
	long	lImgSize		= 0;
	int		iRGBDeep		= 0;
	AVPDecCBType eImgType	= AVPDecCBTypeRGB32;		

	try
	{
		if ( pstYUV==NULL )
		{
			PrepareFrameBuffSize(0);
			return true;
		}
		
		if ( m_iImgRGBFM==JDRAW_RGB_32 )		iRGBDeep = 4;
		else if ( m_iImgRGBFM==JDRAW_RGB_24 )	iRGBDeep = 3;
		else if ( m_iImgRGBFM==JDRAW_RGB_565 )	iRGBDeep = 2;
		else iRGBDeep = 4;

		// 申请缓存 begin
		lImgSize		= m_lImgHeight*m_lImgWidth;
		lFrameBuffSize	= lImgSize*iRGBDeep;
		if ( PrepareFrameBuffSize(lFrameBuffSize)==false )
		{
			return false;
		}
		// 申请缓存 end
		
		// 拷贝结果 begin
		m_stDecFrameCB.iPort	= m_iID;				// 解码通道号
		// YUV 转 RGB begin
		#ifdef __ANDROID__
		m_stDecFrameCB.lpBuf	= (LPBYTE)m_arDecBuf;	// 解码后的数据
		if ( m_iImgRGBFM==JDRAW_RGB_32 )		{m_imgConver.YUV_2_RGB(pstYUV,m_lpFrame,PIX_FMT_YUV420P,PIX_FMT_RGBA);eImgType=AVPDecCBTypeRGB32;}
		else if ( m_iImgRGBFM==JDRAW_RGB_24 )	{m_imgConver.YUV_2_RGB(pstYUV,m_lpFrame,PIX_FMT_YUV420P,PIX_FMT_BGR24);eImgType=AVPDecCBTypeRGB24;}
		else if ( m_iImgRGBFM==JDRAW_RGB_565 )	{m_imgConver.YUV_2_RGB(pstYUV,m_lpFrame,PIX_FMT_YUV420P,PIX_FMT_RGB565);eImgType=AVPDecCBTypeRGB565;}
		else {m_imgConver.YUV_2_RGB(pstYUV,m_lpFrame,PIX_FMT_YUV420P,PIX_FMT_YUV420P);eImgType=AVPDecCBTypeYUV420;}
		#else
		m_stDecFrameCB.lpBuf	= m_lpFrame;			// 解码后的数据
		if ( m_iImgRGBFM==JDRAW_RGB_32 )		{m_imgConver.YUV_2_RGB(pstYUV,m_lpFrame,PIX_FMT_YUV420P,PIX_FMT_BGRA);eImgType=AVPDecCBTypeRGB32;}
		else if ( m_iImgRGBFM==JDRAW_RGB_24 )	{m_imgConver.YUV_2_RGB(pstYUV,m_lpFrame,PIX_FMT_YUV420P,PIX_FMT_RGB24);eImgType=AVPDecCBTypeRGB24;}
		else if ( m_iImgRGBFM==JDRAW_RGB_565 )	{m_imgConver.YUV_2_RGB(pstYUV,m_lpFrame,PIX_FMT_YUV420P,PIX_FMT_RGB565);eImgType=AVPDecCBTypeRGB565;}
		else {m_imgConver.YUV_2_RGB(pstYUV,m_lpFrame,PIX_FMT_YUV420P,PIX_FMT_YUV420P);eImgType=AVPDecCBTypeYUV420;}
		#endif
		// YUV 转 RGB end
		m_stDecFrameCB.lSize	= lFrameBuffSize;		// 解码后数据长
		m_stDecFrameCB.lWidth	= m_lImgWidth;			// 宽
		m_stDecFrameCB.lHeight	= m_lImgHeight;			// 高
		m_stDecFrameCB.lStamp	= (long)m_lFrameTime;	// 帧时间
		m_stDecFrameCB.lType	= eImgType;				// AVPDecCBType
		// 拷贝结果 end

		return true;
	}
	catch(...)
	{
		JTRACE("try-catch CJNVPlayer::PrepareFrameBuff\r\n");
		m_lpFrame		= NULL;
		m_lFrameBufLen	= 0;
		return false;
	}
}

// 设置图像大小
bool CJNVPlayer::SetImageSize(int iImgWidth,int iImgHeight)
{
	bool bLock	= false;

	if ( m_mutexStop.Lock() ) bLock = true;
	else bLock = false;

	if ( m_lImgWidth!=iImgWidth || m_lImgHeight!=iImgHeight )
	{
		//	DbgStrOut("DecBuff()::Reset coder:%s.\r\n",m_szCoder);
		// 删除旧的
		SAFE_DELETE(m_lpDecoder);
		// 申请新的
		m_lpDecoder		= new CDecoder;			// 指定路径
		// 初始化解码器
		if ( m_lpDecoder )
		{
			m_lImgWidth		= iImgWidth;
			m_lImgHeight	= iImgHeight;
			//	m_bNeedIFrame	= true;
		}
		else
		{
			if ( bLock ) m_mutexStop.Unlock();
			return false;
		}
	}

	if ( bLock ) m_mutexStop.Unlock();
	return true;
}

fJThRet CJNVPlayer::RunReadFileThread(void* pParam)
{
	JLOG_TRY
	int					iIsRun				= 0;				// 是否需要运行
	CJLThreadCtrl*		pThreadCtrl			= NULL;				// 对应用线程控制器
	LPJNVPlayer			lpPlayer			= NULL;				// 播放器

	// 初始化参数 begin
	pThreadCtrl	= (CJLThreadCtrl*)pParam;
	if ( pThreadCtrl==NULL )
	{
		return 0;
	}
	lpPlayer	= (LPJNVPlayer)pThreadCtrl->GetOwner();
	if ( lpPlayer==NULL )
	{
		// 通知发生了错误
		lpPlayer->SendCallBackMsg(AVPPlayCBError);
		pThreadCtrl->SetThreadState(THREAD_STATE_STOP);		// 运行状态
		return 0;
	}
	// 初始化参数 end

	#ifdef __ANDROID__
	jboolean	isCopy	= JNI_FALSE;
	if ( g_jvm->AttachCurrentThread(&(lpPlayer->m_pDecEnv), NULL)!=JNI_OK )
	{
		JTRACE("CJNVPlayer::RunDecThread AttachCurrentThread ERR\r\n");
	}
	lpPlayer->m_arDecAudioBuf	= lpPlayer->m_pDecEnv->NewByteArray(320);					// JNI中的BUF
	lpPlayer->m_lpAudioFrame	= (LPBYTE)(lpPlayer->m_pDecEnv->GetByteArrayElements(lpPlayer->m_arDecAudioBuf,&isCopy));
	#endif

	// 通知开始播放
	lpPlayer->SendCallBackMsg(AVPPlayCBStart);
	// 进行读文件操作 begin
	iIsRun	= 1;											// 需要运行
	while(iIsRun)
	{
		// 检查当前状态 begin
		if ( pThreadCtrl->GetNextAction()==THREAD_STATE_STOP )
		{
			// 结束 或 等待结束,马上跳出循环
			iIsRun = 0;										// 不再运行
			// 通知结束播放
			lpPlayer->SendCallBackMsg(AVPPlayCBStop);
			pThreadCtrl->NotifyStop();
			break;
		}
		// 检查当前状态 end
		
		lpPlayer->ReadFileAction();							// 处理单个播放动作
	//	iIsRun = 0;											// 不再运行
	//	lpPlayer->SendCallBackMsg(AVPPlayCBStop);
	}
	iIsRun = 0;
	// 进行读文件操作 end
	
	#ifdef __ANDROID__
	g_jvm->DetachCurrentThread();
	#endif

	return 0;
	JLOG_CATCH("try-catch CJNVPlayer::RunReadFileThread \r\n");
	return 0;
}

fJThRet CJNVPlayer::RunDecThread(void* pParam)
{
	JTRACE("CJNVPlayer::RunDecThread begin \r\n");

	JLOG_TRY
	int					iIsRun				= 0;				// 是否需要运行
	CJLThreadCtrl*		pThreadCtrl			= NULL;				// 对应用线程控制器
	LPJNVPlayer			lpPlayer			= NULL;				// 播放器

	// 初始化参数 begin
	pThreadCtrl	= (CJLThreadCtrl*)pParam;
	if ( pThreadCtrl==NULL )
	{
		return 0;
	}
	lpPlayer	= (LPJNVPlayer)pThreadCtrl->GetOwner();
	if ( lpPlayer==NULL )
	{
		pThreadCtrl->SetThreadState(THREAD_STATE_STOP);		// 运行状态
		return 0;
	}
	// 初始化参数 end

	#ifdef __ANDROID__
	jboolean	isCopy	= JNI_FALSE;
	if ( g_jvm->AttachCurrentThread(&(lpPlayer->m_pDecEnv), NULL)!=JNI_OK )
	{
		JTRACE("CJNVPlayer::RunDecThread AttachCurrentThread ERR\r\n");
	}
	lpPlayer->m_arDecAudioBuf	= lpPlayer->m_pDecEnv->NewByteArray(320);					// JNI中的BUF
	lpPlayer->m_lpAudioFrame	= (LPBYTE)(lpPlayer->m_pDecEnv->GetByteArrayElements(lpPlayer->m_arDecAudioBuf,&isCopy));
	#endif
	// 进行解码操作 begin
	iIsRun	= 1;											// 需要运行
	while(iIsRun)
	{
		// 检查当前状态 begin
		if ( pThreadCtrl->GetNextAction()==THREAD_STATE_STOP )
		{
			// 结束 或 等待结束,马上跳出循环
			iIsRun = 0;										// 不再运行
			break;
		}
		// 检查当前状态 end

		// 处理单个解码动作
		if ( lpPlayer->DecAction()==false )
		{
			iIsRun = 0;										// 不再运行
			break;
		}
	}
	// 通知结束播放
//	lpPlayer->SendCallBackMsg(AVPCBFTypePlay,AVPPlayCBStop);
	pThreadCtrl->NotifyStop();
	iIsRun = 0;
	// 进行解码操作 end

	#ifdef __ANDROID__
	g_jvm->DetachCurrentThread();
	#endif

	return 0;
	JLOG_CATCH("try-catch CJNVPlayer::RunDecThread \r\n");
	return 0;
}
