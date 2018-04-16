// JDSound.h: interface for the CJDSound class.
//
//////////////////////////////////////////////////////////////////////

#if !defined(AFX_JDSOUND_H__DE6FB2EB_7E41_4351_995C_2ADB1FE50178__INCLUDED_)
#define AFX_JDSOUND_H__DE6FB2EB_7E41_4351_995C_2ADB1FE50178__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

#ifndef DWORD_PTR
#define	DWORD_PTR	DWORD*
#endif

#include <mmsystem.h> 
#include <mmreg.h> 
#include <dsound.h> 

typedef const GUID* LPCGUID;

class CJDSound  
{
public:
	CJDSound();
	virtual ~CJDSound();

	// 初始化输出buff(需要根据WAV流的采样率等来初始化)
	int InitDSBuffOut(HWND hWnd,LPWAVEFORMATEX lpWFX,int iFrameRate=25);
	// 销毁输出buff
	int DestoryDSBuffOut();

	// 初始化输入buff
	int InitDSBuffIn(LPWAVEFORMATEX lpWFX,int iRecRate);
	// 销毁输入buff
	int DestoryDSBuffIn();

	// 初始化DSound
	int InitDSound(HWND hWnd);
	// 销毁DSound
	int DestoryDSound();

	// 重置输出位置
	int ResetOutPos();
	// 设置一段WAV流到输出缓冲
	int SetBufDataOut(LPBYTE lpbSoundData, DWORD dwSoundBytes);
	// 获取输出缓存的大小
	long GetOutBuffSize();
	// 获取输出格式
	LPWAVEFORMATEX GetOutFormat();
	// 获取输出音量大小
	long GetVolumeOut();
	// 设置输出音量大小
	int SetVolumeOut(long lVolume);

	// 从输入缓冲获取一段WAV流
	int BeginGetBufDataIn(LPVOID* pDataPtr1, LPDWORD pDataBytes1, LPVOID* pDataPtr2, LPDWORD pDataBytes2);
	int EndGetBufDataIn(VOID* pDataPtr1, DWORD dwDataBytes1, VOID* pDataPtr2, DWORD dwDataBytes2);
	// 获取输入缓存的大小
	long GetInBuffSize();
	// 获取输入格式
	LPWAVEFORMATEX GetInFormat();

	// 开始播放
	int Play(DWORD dwFlags=DSBPLAY_LOOPING);
	// 结束播放
	int StopPlay();
	// 等待播放事件
	// WAIT_OBJECT_0 : 表示录音进度有更新
	// WAIT_OBJECT_0 + 1 : 表示录音结束
	// WAIT_TIMEOUT : 等待超时
	DWORD WaitForPlayNotify();

	// 开始录音
	int Record(DWORD dwFlags=DSCBSTART_LOOPING);
	// 结束录音
	int StopRecord();
	// 等待录音事件
	// WAIT_OBJECT_0 : 表示录音进度有更新
	// WAIT_OBJECT_0 + 1 : 表示录音结束
	// WAIT_TIMEOUT : 等待超时
	DWORD WaitForRecordNotify();

protected:
	LPCGUID					m_pDSoundOutID;		// 声音输出设备ID
	LPDIRECTSOUND			m_pDSoundOut;		// 声音输出设备
	LPDIRECTSOUNDBUFFER		m_lpBufOut;			// 输出缓冲
	WAVEFORMATEX			m_wfxOut;			// 输出waveform-audio 数据的格式
	DWORD					m_dwOffsetOut;		// 输出缓冲偏移
	LPDIRECTSOUNDNOTIFY		m_pDSNotifyOut;		// 提醒消息
	HANDLE					m_hEventOut[2];		// 事件
	LONG					m_lBuffSizeOut;		// 输出缓冲大小
	LONG					m_lFrameRateOut;	// 输出帧率

	LPCGUID					m_pDSoundInID;		// 声音输入设备ID
	LPDIRECTSOUNDCAPTURE	m_pDSoundIn;		// 声音输入设备
	LPDIRECTSOUNDCAPTUREBUFFER	m_lpBufIn;		// 输入缓冲
	WAVEFORMATEX			m_wfxIn;			// 输入waveform-audio 数据的格式
	DWORD					m_dwOffsetIn;		// 输入缓冲偏移
	LPDIRECTSOUNDNOTIFY		m_pDSNotifyIn;		// 提醒消息
	HANDLE					m_hEventIn[2];		// 事件
	LONG					m_lBuffSizeIn;		// 输入缓冲大小
	int						m_iRecRate;			// 录音频率

	FILE*					m_pFile;			// 录音工具
};

#endif // !defined(AFX_JDSOUND_H__DE6FB2EB_7E41_4351_995C_2ADB1FE50178__INCLUDED_)
