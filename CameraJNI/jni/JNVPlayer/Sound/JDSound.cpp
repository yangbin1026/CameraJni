// JDSound.cpp: implementation of the CJDSound class.
//
//////////////////////////////////////////////////////////////////////

#include "stdafx.h"
#include "JDSound.h"

//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////

CJDSound::CJDSound()
{
	m_pDSoundOutID	= NULL;			// 声音输出设备ID
	m_pDSoundOut	= NULL;			// 声音输出设备
	m_lpBufOut		= NULL;			// 输出缓冲
	m_dwOffsetOut	= 0;			// 输出缓冲偏移
	m_pDSNotifyOut	= NULL;			// 提醒消息
	m_hEventOut[0]	= NULL;			// 播放进度事件
	m_hEventOut[1]	= NULL;			// 播放结束事件
	m_lBuffSizeOut	= 0;			// 输出缓冲大小
	m_lFrameRateOut	= 0;			// 输出帧率
	
	m_pDSoundInID	= NULL;			// 声音输入设备ID
	m_pDSoundIn		= NULL;			// 声音输入设备
	m_lpBufIn		= NULL;			// 输入缓冲
	m_dwOffsetIn	= 0;			// 输入缓冲偏移
	m_pDSNotifyIn	= NULL;			// 提醒消息
	m_hEventIn[0]	= NULL;			// 录音进度事件
	m_hEventIn[1]	= NULL;			// 录音结束事件
	m_lBuffSizeIn	= 0;			// 输入缓冲大小
	m_iRecRate		= 0;			// 录音频率
	
	m_pFile			= NULL;			// 录音工具
}

CJDSound::~CJDSound()
{
	DestoryDSound();
}


// 初始化输出buff(需要根据WAV流的采样率等来初始化)
int CJDSound::InitDSBuffOut(HWND hWnd,LPWAVEFORMATEX lpWFX,int iFrameRate)
{
	int					i			= 0;
	HRESULT				hResult		= NULL;
	DSBUFFERDESC		dsbDesc;				// 用于描述声音缓冲区的属性
	DSBPOSITIONNOTIFY*	pNotifyPos	= NULL;		// 提醒位置

	// 参数错误
	if ( lpWFX==NULL ) return -1;

	// 如果采样率不一样才重设
	if (lpWFX->nSamplesPerSec!=m_wfxOut.nSamplesPerSec)
	{
		DestoryDSBuffOut();
	}
	else
	{
		return 0;
	}

	// 初始化输出设备
	if ( m_pDSoundOut==NULL )
	{
		// 创建声音输出设备
		hResult = DirectSoundCreate(NULL,&m_pDSoundOut,NULL);
		if ( hResult != DS_OK )
		{
			// 创建失败,原因可查:
			// DSERR_ALLOCATED ,DSERR_INVALIDPARAM,DSERR_NOAGGREGATION,DSERR_NODRIVER,DSERR_OUTOFMEMORY 
			return -1;
		}

		// 设置声音输出设备的协作度
		hResult = m_pDSoundOut->SetCooperativeLevel(hWnd, DSSCL_NORMAL );// DSSCL_EXCLUSIVE);
		if ( hResult != DS_OK )
		{
			// 创建失败,原因可查:
			// DSERR_ALLOCATED ,DSERR_INVALIDPARAM,DSERR_UNINITIALIZED,DSERR_UNSUPPORTED
			// 销毁输出buff
			DestoryDSBuffOut();
			return -1;
		}
	}

	// 已经初始化buff了
	if ( m_lpBufOut ) return 0;

	// 创建输出缓冲区 begin 
	memcpy(&m_wfxOut,lpWFX,sizeof(WAVEFORMATEX));
	m_dwOffsetOut		= 0;		// 偏移
	// 设置 DSBUFFERDESC 格式结构体
	memset(&dsbDesc, 0, sizeof(DSBUFFERDESC));   
	dsbDesc.dwSize			= sizeof(DSBUFFERDESC);   
	dsbDesc.dwFlags			= DSBCAPS_CTRLPAN | DSBCAPS_CTRLVOLUME | DSBCAPS_CTRLFREQUENCY
							| DSBCAPS_GLOBALFOCUS | DSBCAPS_CTRLPOSITIONNOTIFY; 
	dsbDesc.dwBufferBytes	= lpWFX->nAvgBytesPerSec;				// 1 秒的长度
	dsbDesc.lpwfxFormat		= &m_wfxOut;
	m_lBuffSizeOut			= dsbDesc.dwBufferBytes;				// 输出缓冲大小
	m_lFrameRateOut			= iFrameRate;							// 输出帧率
   
	// 创建缓冲
	hResult = m_pDSoundOut->CreateSoundBuffer(&dsbDesc, &m_lpBufOut, NULL);
	if ( hResult != DS_OK )
	{
		return -1;
	}
	// 创建输出缓冲区 end

	// 创建消息响应 begin
	hResult = m_lpBufOut->QueryInterface(IID_IDirectSoundNotify, (VOID**)&m_pDSNotifyOut);
	if (hResult!=S_OK) 
	{
		return -1; 
	}
	m_hEventOut[0] = CreateEvent(NULL, FALSE, FALSE, NULL);			// 播放进度事件
	m_hEventOut[1] = CreateEvent(NULL, FALSE, FALSE, NULL);			// 播放结束事件
	pNotifyPos = new DSBPOSITIONNOTIFY[iFrameRate+1];
	for (i=0; i<iFrameRate; i++)
	{
		pNotifyPos[i].dwOffset		= (i + 1)*(lpWFX->nAvgBytesPerSec/iFrameRate)-1;
		pNotifyPos[i].hEventNotify	= m_hEventOut[0];
	}
	pNotifyPos[i].dwOffset			= DSBPN_OFFSETSTOP ;
	pNotifyPos[i].hEventNotify		= m_hEventOut[1];
	hResult = m_pDSNotifyOut->SetNotificationPositions(iFrameRate, pNotifyPos);
	if ( hResult != DS_OK )
	{
		return -1;
	}
	if ( pNotifyPos ) {delete[] pNotifyPos;pNotifyPos=NULL;}
	// 创建消息响应 begin
	
	// 设置音量大小
	SetVolumeOut(100);
	
//	m_pFile	= fopen("C:\\1.pcm","wb");			// 录音工具
	return 0;
}

// 销毁输出buff
int CJDSound::DestoryDSBuffOut()
{
	CloseHandle(m_hEventOut[0]);m_hEventOut[0]=NULL;
	CloseHandle(m_hEventOut[1]);m_hEventOut[1]=NULL;
	SAFE_RELEASE_B(m_pDSNotifyOut);	// 销毁输出提示事件
	SAFE_RELEASE_B(m_lpBufOut);		// 输出缓冲
	m_dwOffsetOut	= 0;			// 偏移
	m_lBuffSizeOut	= 0;			// 输出缓冲大小
	m_lFrameRateOut	= 0;			// 输出帧率
	SAFE_RELEASE_B(m_pDSoundOut);	// 销毁声音输出设备
	memset(&m_wfxOut,0,sizeof(WAVEFORMATEX));	// 输出waveform-audio 数据的格式

	if ( m_pFile )
	{
		fclose(m_pFile);
		m_pFile = NULL;
	}
	return 0;
}

// 初始化输入buff
int CJDSound::InitDSBuffIn(LPWAVEFORMATEX lpWFX,int iRecRate)
{
	int					i			= 0;
	HRESULT				hResult		= NULL;
	DSCBUFFERDESC		dscbDesc;				// 用于描述声音缓冲区的属性
	DSBPOSITIONNOTIFY*	pNotifyPos	= NULL;		// 提醒位置

	// 初始化输入设备
	if ( m_pDSoundIn==NULL )
	{
		// 创建声音输入设备
		hResult = DirectSoundCaptureCreate(NULL,&m_pDSoundIn,NULL);
		if ( hResult != DS_OK )
		{
			// 创建失败,原因可查:
			// DSERR_ALLOCATED ,DSERR_INVALIDPARAM,DSERR_NOAGGREGATION,DSERR_NODRIVER,DSERR_OUTOFMEMORY 
			return -1;
		}
	}

	// 参数错误
	if ( lpWFX==NULL || iRecRate<=0 ) return -1;

	// 已经初始化buff了
	if ( m_lpBufIn ) return 0;

	// 创建输出缓冲区 begin 
	// 设置 DSBUFFERDESC 格式结构体
	memset(&dscbDesc, 0, sizeof(DSCBUFFERDESC));
	dscbDesc.dwSize			= sizeof(DSCBUFFERDESC); 
	dscbDesc.dwFlags		= 0; 
	dscbDesc.dwBufferBytes	= lpWFX->nAvgBytesPerSec;				// 1 秒的长度
	dscbDesc.dwReserved		= 0; 
	dscbDesc.lpwfxFormat	= lpWFX; 
	m_lBuffSizeIn			= dscbDesc.dwBufferBytes;
	m_iRecRate				= iRecRate;								// 录音频率
   
	// 创建缓冲
	hResult = m_pDSoundIn->CreateCaptureBuffer(&dscbDesc, &m_lpBufIn, NULL);
	if ( hResult != DS_OK )
	{
		return -1;
	}
	memcpy(&m_wfxIn,lpWFX,sizeof(WAVEFORMATEX));
	m_dwOffsetIn		= 0;			// 偏移
	// 创建输出缓冲区 end

	// 创建消息响应 begin
	hResult = m_lpBufIn->QueryInterface(IID_IDirectSoundNotify, (VOID**)&m_pDSNotifyIn);
	if (hResult!=S_OK) 
	{
		return -1; 
	}
	m_hEventIn[0] = CreateEvent(NULL, FALSE, FALSE, NULL);			// 录音进度事件
	m_hEventIn[1] = CreateEvent(NULL, FALSE, FALSE, NULL);			// 录音结束事件
	pNotifyPos = new DSBPOSITIONNOTIFY[iRecRate+1];					// 按帧率来定断点位置
	for (i=0; i<iRecRate; i++)
	{
		pNotifyPos[i].dwOffset		= (i + 1)*(lpWFX->nAvgBytesPerSec/iRecRate)-1;
		pNotifyPos[i].hEventNotify	= m_hEventIn[0];
	}
	pNotifyPos[i].dwOffset			= DSBPN_OFFSETSTOP ;
	pNotifyPos[i].hEventNotify		= m_hEventIn[1];
	hResult = m_pDSNotifyIn->SetNotificationPositions(iRecRate, pNotifyPos);
	if ( hResult != DS_OK )
	{
		return -1;
	}
	if ( pNotifyPos ) {delete[] pNotifyPos;pNotifyPos=NULL;}
	// 创建消息响应 begin

	return 0;
}

// 销毁输出buff
int CJDSound::DestoryDSBuffIn()
{
	SAFE_RELEASE_B(m_pDSNotifyIn);	// 销毁输入提示事件
	CloseHandle(m_hEventIn[0]);m_hEventIn[0]=NULL;
	CloseHandle(m_hEventIn[1]);m_hEventIn[1]=NULL;
	SAFE_RELEASE_B(m_lpBufIn);		// 输入缓冲
	m_dwOffsetIn	= 0;			// 偏移
	m_lBuffSizeIn	= 0;			// 输入缓冲大小
	m_iRecRate	= 0;			// 输入帧率
	SAFE_RELEASE_B(m_pDSoundIn);	// 销毁声音输入设备
	memset(&m_wfxIn,0,sizeof(WAVEFORMATEX));	// 输入waveform-audio 数据的格式
	return 0;
}

// 销毁DSound
int CJDSound::DestoryDSound()
{
	// 销毁输出buff
	DestoryDSBuffOut();
	
	// 销毁输入buff
	DestoryDSBuffIn();

	return 0;
}

// 重置输出位置
int CJDSound::ResetOutPos()
{
	m_dwOffsetOut = 0;
	m_lpBufOut->SetCurrentPosition(m_dwOffsetOut);	// 指定播放是循环的
	return 0;
}

// 设置一段WAV流到输出缓冲
int CJDSound::SetBufDataOut(LPBYTE lpbSoundData, DWORD dwSoundBytes)
{
	HRESULT		hResult		= NULL;		// 返回值
	LPVOID		lpvPtr1		= NULL;		// 第一块缓冲的起始地址
	DWORD		dwBytes1	= 0;		// 第一块缓冲的大小
	LPVOID		lpvPtr2		= NULL;		// 第二块缓冲的起始地址
	DWORD		dwBytes2	= 0;		// 第二块缓冲的大小
	long		lBufSize	= 0;

	// 未初始化
	if ( m_pDSoundOut==NULL || m_lpBufOut==NULL ) return -1;

	if ( m_pFile )
	{
		fwrite(lpbSoundData,dwSoundBytes,1,m_pFile);
	}

	// 获取用于写入的缓存的地址,因为缓冲是循环使用的,所以如果地址因不够而重回到起始点时,
	// 会返回2个地址指针
	hResult = m_lpBufOut->Lock(m_dwOffsetOut, dwSoundBytes, &lpvPtr1, &dwBytes1, &lpvPtr2, &dwBytes2, 0);

	// 缓存已经丢失,需要重新加载并重新尝试Lock
	if (DSERR_BUFFERLOST == hResult)   
	{
		// 缓存已经丢失,需要重新加载
		m_lpBufOut->Restore();
		hResult = m_lpBufOut->Lock(m_dwOffsetOut, dwSoundBytes, &lpvPtr1, &dwBytes1, &lpvPtr2, &dwBytes2, 0);   
	}
	if ( hResult != DS_OK )
	{
		return -1;
	}

	// 写入到缓冲
	CopyMemory(lpvPtr1, lpbSoundData, dwBytes1);
	m_dwOffsetOut += dwBytes1;			// 偏移
	if (NULL != lpvPtr2)
	{
		CopyMemory(lpvPtr2, lpbSoundData+dwBytes1, dwBytes2);
		m_dwOffsetOut = dwBytes2;		// 偏移
	}
	if ( (long)m_dwOffsetOut>=GetOutBuffSize() )
	{
		m_dwOffsetOut = dwBytes2;
	}

	// 释放缓冲
	hResult = m_lpBufOut->Unlock(lpvPtr1, dwBytes1, lpvPtr2, dwBytes2);
	if ( hResult != DS_OK )
	{
		return -1;
	}

	return 0;
}
// 获取输出缓存的大小
long CJDSound::GetOutBuffSize()
{
	// 未初始化
	if ( m_pDSoundOut==NULL || m_lpBufOut==NULL ) return 0;

	return m_lBuffSizeOut;
}
// 获取输出格式
LPWAVEFORMATEX CJDSound::GetOutFormat()
{
	return &m_wfxOut;
}
// 获取输出音量大小
long CJDSound::GetVolumeOut()
{
	HRESULT	hResult	= NULL;
	long	lVolume = 0;
	// 未初始化
	if ( m_pDSoundOut==NULL || m_lpBufOut==NULL ) return -1;

	hResult = m_lpBufOut->GetVolume(&lVolume);
	if ( hResult != DS_OK )
	{
		return -1;
	}
	lVolume = (lVolume-(DSBVOLUME_MIN))/100;
	return lVolume;
}
// 设置输出音量大小
int CJDSound::SetVolumeOut(long lVolume)
{
	HRESULT	hResult	= NULL;
	// 未初始化
	if ( m_pDSoundOut==NULL || m_lpBufOut==NULL ) return -1;

	lVolume = 100*lVolume+(DSBVOLUME_MIN);
	hResult = m_lpBufOut->SetVolume(lVolume);
	if ( hResult != DS_OK )
	{
		return -1;
	}
	return lVolume;
}

/*
// 从输入缓冲获取一段WAV流
int CJDSound::BeginGetBufDataIn(LPVOID* pDataPtr1, LPDWORD pDataBytes1, LPVOID* pDataPtr2, LPDWORD pDataBytes2)
{
	HRESULT		hResult			= NULL;		// 返回值
	DWORD		dwReadPos		= 0;		// 该值之前的均可读取
	DWORD		dwCapturePos	= 0;		// 该值为正准备要采集的位置
	DWORD		dwLockSize		= 0;		// 要读的数据段大小
	DWORD		dwBufLen		= 0;		// 缓存大小

	// 未初始化
	if ( m_pDSoundIn==NULL || m_lpBufIn==NULL ) return -1;

	// 获取当前位置信息
	hResult = m_lpBufIn->GetCurrentPosition(&dwCapturePos, &dwReadPos);
	if ( hResult != DS_OK )
	{
		return -1;
	}
	
	// 计算要读的数据的大小
	dwBufLen = GetInBuffSize();
	if ( dwReadPos>=m_dwOffsetIn )
	{
		dwLockSize = dwReadPos - m_dwOffsetIn;
	}
	else // if ( dwReadPos<m_dwOffsetIn )
	{
		dwLockSize = dwBufLen - m_dwOffsetIn + dwReadPos;
	}

	// 锁住要读取的位置
	hResult = m_lpBufIn->Lock(m_dwOffsetIn, dwLockSize, pDataPtr1, pDataBytes1, pDataPtr2, pDataBytes2, 0L);
	if ( hResult != DS_OK )
	{
		return -1;
	}
	m_dwOffsetIn += dwLockSize;				// 偏移
	if ( (long)m_dwOffsetIn>=(long)dwBufLen )
	{
		m_dwOffsetIn = *pDataBytes2;		// 偏移
	}
	
	return 0;
}
*/

// 从输入缓冲获取一段WAV流
int CJDSound::BeginGetBufDataIn(LPVOID* pDataPtr1, LPDWORD pDataBytes1, LPVOID* pDataPtr2, LPDWORD pDataBytes2)
{
	HRESULT		hResult			= NULL;		// 返回值
	DWORD		dwReadPos		= 0;		// 该值之前的均可读取
	DWORD		dwCapturePos	= 0;		// 该值为正准备要采集的位置
	DWORD		dwLockSize		= 0;		// 要读的数据段大小
	DWORD		dwBufLen		= 0;		// 缓存大小

	// 未初始化
	if ( m_pDSoundIn==NULL || m_lpBufIn==NULL ) return -1;
	
	// 计算要读的数据的大小
	dwLockSize = GetInBuffSize()/m_iRecRate;

	// 锁住要读取的位置
	hResult = m_lpBufIn->Lock(m_dwOffsetIn, dwLockSize, pDataPtr1, pDataBytes1, pDataPtr2, pDataBytes2, 0L);
	if ( hResult != DS_OK )
	{
		return -1;
	}
	m_dwOffsetIn += dwLockSize;				// 偏移
	if ( (long)m_dwOffsetIn>=(long)GetInBuffSize() )
	{
		m_dwOffsetIn = *pDataBytes2;		// 偏移
	}
	
	return 0;
}
// 从输入缓冲获取一段WAV流
int CJDSound::EndGetBufDataIn(VOID* pDataPtr1, DWORD dwDataBytes1, VOID* pDataPtr2, DWORD dwDataBytes2)
{
	HRESULT		hResult			= NULL;		// 返回值
	
	// 未初始化
	if ( m_pDSoundIn==NULL || m_lpBufIn==NULL ) return -1;

	// unlock
	hResult = m_lpBufIn->Unlock(pDataPtr1, dwDataBytes1, pDataPtr2, dwDataBytes2);
	if ( hResult != DS_OK )
	{
		return -1;
	}

	return 0;
}
// 获取输入缓存的大小
long CJDSound::GetInBuffSize()
{
	// 未初始化
	if ( m_pDSoundIn==NULL || m_lpBufIn==NULL ) return 0;

	return m_lBuffSizeIn;
}
// 获取输入格式
LPWAVEFORMATEX CJDSound::GetInFormat()
{
	return &m_wfxIn;
}

// 开始播放
int CJDSound::Play(DWORD dwFlags)
{
	HRESULT		hResult		= NULL;				// 返回值

	// 未初始化
	if ( m_pDSoundOut==NULL || m_lpBufOut==NULL ) return -1;
	
	hResult = m_lpBufOut->Play(0,0,dwFlags);	// 指定播放是循环的
	if ( hResult != DS_OK )
	{
		return -1;
	}   

	return 0;
}
// 结束播放
int CJDSound::StopPlay()
{
	HRESULT		hResult		= NULL;				// 返回值
	
	// 未初始化
	if ( m_pDSoundOut==NULL || m_lpBufOut==NULL ) return -1;
	
	hResult = m_lpBufOut->Stop();
	if ( hResult != DS_OK )
	{
		return -1;
	}

	return 0;
}

// 等待播放事件
DWORD CJDSound::WaitForPlayNotify()
{
	// WAIT_OBJECT_0 : 表示播放进度有更新
	// WAIT_OBJECT_0 + 1 : 表示播放结束
	// WAIT_TIMEOUT : 等待超时
	
	DWORD	dwResult;
	dwResult = MsgWaitForMultipleObjects(2, m_hEventOut, FALSE, INFINITE, NULL);
	
	return dwResult;
}

// 开始录音
int CJDSound::Record(DWORD dwFlags)
{
	HRESULT		hResult		= NULL;				// 返回值

	// 未初始化
	if ( m_pDSoundIn==NULL || m_lpBufIn==NULL ) return -1;
	
	hResult = m_lpBufIn->Start(dwFlags);
	if ( hResult != DS_OK )
	{
		return -1;
	}

	return 0;
}

// 结束录音
int CJDSound::StopRecord()
{
	HRESULT		hResult		= NULL;				// 返回值

	// 未初始化
	if ( m_pDSoundIn==NULL || m_lpBufIn==NULL ) return 0;

	hResult = m_lpBufIn->Stop();
	if ( hResult != DS_OK )
	{
		return -1;
	}

	return 0;
}

// 等待录音事件
DWORD CJDSound::WaitForRecordNotify()
{
	// WAIT_OBJECT_0 : 表示录音进度有更新
	// WAIT_OBJECT_0 + 1 : 表示录音结束
	// WAIT_TIMEOUT : 等待超时
	
	DWORD	dwResult;
	dwResult = MsgWaitForMultipleObjects(2, m_hEventIn, FALSE, INFINITE, NULL);
	
	return dwResult;
}
