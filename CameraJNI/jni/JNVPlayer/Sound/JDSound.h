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

	// ��ʼ�����buff(��Ҫ����WAV���Ĳ����ʵ�����ʼ��)
	int InitDSBuffOut(HWND hWnd,LPWAVEFORMATEX lpWFX,int iFrameRate=25);
	// �������buff
	int DestoryDSBuffOut();

	// ��ʼ������buff
	int InitDSBuffIn(LPWAVEFORMATEX lpWFX,int iRecRate);
	// ��������buff
	int DestoryDSBuffIn();

	// ��ʼ��DSound
	int InitDSound(HWND hWnd);
	// ����DSound
	int DestoryDSound();

	// �������λ��
	int ResetOutPos();
	// ����һ��WAV�����������
	int SetBufDataOut(LPBYTE lpbSoundData, DWORD dwSoundBytes);
	// ��ȡ�������Ĵ�С
	long GetOutBuffSize();
	// ��ȡ�����ʽ
	LPWAVEFORMATEX GetOutFormat();
	// ��ȡ���������С
	long GetVolumeOut();
	// �������������С
	int SetVolumeOut(long lVolume);

	// �����뻺���ȡһ��WAV��
	int BeginGetBufDataIn(LPVOID* pDataPtr1, LPDWORD pDataBytes1, LPVOID* pDataPtr2, LPDWORD pDataBytes2);
	int EndGetBufDataIn(VOID* pDataPtr1, DWORD dwDataBytes1, VOID* pDataPtr2, DWORD dwDataBytes2);
	// ��ȡ���뻺��Ĵ�С
	long GetInBuffSize();
	// ��ȡ�����ʽ
	LPWAVEFORMATEX GetInFormat();

	// ��ʼ����
	int Play(DWORD dwFlags=DSBPLAY_LOOPING);
	// ��������
	int StopPlay();
	// �ȴ������¼�
	// WAIT_OBJECT_0 : ��ʾ¼�������и���
	// WAIT_OBJECT_0 + 1 : ��ʾ¼������
	// WAIT_TIMEOUT : �ȴ���ʱ
	DWORD WaitForPlayNotify();

	// ��ʼ¼��
	int Record(DWORD dwFlags=DSCBSTART_LOOPING);
	// ����¼��
	int StopRecord();
	// �ȴ�¼���¼�
	// WAIT_OBJECT_0 : ��ʾ¼�������и���
	// WAIT_OBJECT_0 + 1 : ��ʾ¼������
	// WAIT_TIMEOUT : �ȴ���ʱ
	DWORD WaitForRecordNotify();

protected:
	LPCGUID					m_pDSoundOutID;		// ��������豸ID
	LPDIRECTSOUND			m_pDSoundOut;		// ��������豸
	LPDIRECTSOUNDBUFFER		m_lpBufOut;			// �������
	WAVEFORMATEX			m_wfxOut;			// ���waveform-audio ���ݵĸ�ʽ
	DWORD					m_dwOffsetOut;		// �������ƫ��
	LPDIRECTSOUNDNOTIFY		m_pDSNotifyOut;		// ������Ϣ
	HANDLE					m_hEventOut[2];		// �¼�
	LONG					m_lBuffSizeOut;		// ��������С
	LONG					m_lFrameRateOut;	// ���֡��

	LPCGUID					m_pDSoundInID;		// ���������豸ID
	LPDIRECTSOUNDCAPTURE	m_pDSoundIn;		// ���������豸
	LPDIRECTSOUNDCAPTUREBUFFER	m_lpBufIn;		// ���뻺��
	WAVEFORMATEX			m_wfxIn;			// ����waveform-audio ���ݵĸ�ʽ
	DWORD					m_dwOffsetIn;		// ���뻺��ƫ��
	LPDIRECTSOUNDNOTIFY		m_pDSNotifyIn;		// ������Ϣ
	HANDLE					m_hEventIn[2];		// �¼�
	LONG					m_lBuffSizeIn;		// ���뻺���С
	int						m_iRecRate;			// ¼��Ƶ��

	FILE*					m_pFile;			// ¼������
};

#endif // !defined(AFX_JDSOUND_H__DE6FB2EB_7E41_4351_995C_2ADB1FE50178__INCLUDED_)
