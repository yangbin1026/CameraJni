// JLThreadCtrl.h: interface for the CJLThreadCtrl class.
//
//////////////////////////////////////////////////////////////////////

#if !defined(AFX_JLTHREADCTRL_H_JSOCK__E038D85C_E4D1_4414_B4B1_D78EA4273387__INCLUDED_)
#define AFX_JLTHREADCTRL_H_JSOCK__E038D85C_E4D1_4414_B4B1_D78EA4273387__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

//#include "jxj_lib_def.h"
#include "j_vs_ver.h"
#include "stdio.h"
#if (defined _WIN32) || (defined _WIN64)
	// CJLThreadCtrl ����صĺ궨�� begin
	#include "MutexLockWnd.h"
	#define	pthread_t	DWORD
	#define fJThRet		DWORD WINAPI
	typedef HANDLE PJHANDLE;
	// CJLThreadCtrl ����صĺ궨�� end
#else
	// CJLThreadCtrl ����صĺ궨�� begin
	#include <pthread.h>
	#include <unistd.h>
	#include "MutexLockLinux.h"
	#define fJThRet		void*
	// CJLThreadCtrl ����صĺ궨�� end
#endif
//void DbgStrOut(const TCHAR *fmt,... );

typedef fJThRet fcbJThread (void *);

#define		THREAD_STATE_STOP		0x00000000	// ����
#define		THREAD_STATE_W_STOP		0x00001000	// �ȴ�����
#define		THREAD_STATE_RUN		0x00000001	// ����
#define		THREAD_STATE_W_RUN		0x00001001	// �ȴ�����
#define		THREAD_STATE_PAUSE		0x00000002	// ��ͣ

#define		THREAD_MAX_WAIT_RET		10			// ���ȴ�10��
#define		THREAD_PER_WAIT_TIME	40			// ÿ���ȴ��ĺ�����

#ifndef		J_DGB_NAME_LEN
#define		J_DGB_NAME_LEN			64
#endif

namespace JSocketCtrl
{
class CJLThreadCtrl  
{
public:
	CJLThreadCtrl(void* pOwner = NULL);
	virtual ~CJLThreadCtrl();

	// ��ʼ�̺߳���
	HANDLE			StartThread(fcbJThread* pThreadFun);
	// �����̺߳���
	int				StopThread(bool bWaitRet=true);
	// ��ͣ�̺߳���
	int				PauseThread();
	// �����̺߳���
	int				ContinueThread();
	// ��ȡ�߳�״̬
	DWORD			GetThreadState();
	// ֪ͨ����
	void			NotifyStop();

	// ������һ����Ĳ���
	// THREAD_STATE_STOP:��ʾ�߳�Ӧ��Ҫ������;
	// THREAD_STATE_RUN:��ʾ��������;
	// THREAD_STATE_PAUSE:��ʾ��Ҫ��ͣ
	DWORD			GetNextAction();

	// ��ȡ��Ӧ�ľ��
	pthread_t		GetHandle();

	// ���ñ������Ĳ���,���̺߳����е���
	int				SetParam(void* pParam);
	// ��ȡ�������Ĳ���,���̺߳����е���
	void*			GetParam();

	// ���ñ�������������(�ڻص�������ʹ��)
	int				SetOwner(void* pParam);
	// ��ȡ��������������(�ڻص�������ʹ��)
	void*			GetOwner();

	// �����߳�״̬,ע��:ֻ��������Ӧ��״ֵ̬,�������߼�����,һ��ֻ���̺߳�����,���ý���״̬
	void			SetThreadState(DWORD dwThreadState);

public:
	char			m_szName[J_DGB_NAME_LEN];	// ����...������
protected:
	pthread_t		m_hThreadID;				// �߳�ID
	HANDLE			m_hThread;					// �߳̾��
	DWORD			m_dwThreadState;			// �߳�״̬
	fcbJThread*		m_pThreadFun;				// �̺߳���
	void*			m_pParam;					// ����(�ڻص�������ʹ��)
	void*			m_pOwner;					// ������(�ڻص�������ʹ��)
//	HANDLE			m_hEventStop;				// ֹͣ�¼�
	PJHANDLE		m_hEventPause;				// ��ͣ�¼�
};
}
using namespace JSocketCtrl;

#endif // !defined(AFX_JLTHREADCTRL_H_JSOCK__E038D85C_E4D1_4414_B4B1_D78EA4273387__INCLUDED_)
