// MutexLockLinux.h: interface for the CMutexLock class.
//
//////////////////////////////////////////////////////////////////////

#if !defined(AFX_MUTEXLOCKLINUX_H__FDC365C0_4E88_4C5D_8846_6D22FF9655E3__INCLUDED_)
#define AFX_MUTEXLOCKLINUX_H__FDC365C0_4E88_4C5D_8846_6D22FF9655E3__INCLUDED_

#include "pthread.h"
class CMutexLock  
{
public:
	CMutexLock();
	~CMutexLock();

	bool CreateMutex();
	bool Lock();
	bool Unlock();
	bool IsCreate();
	bool CloseMutex();

protected:
	pthread_mutex_t m_hObject;				//linux
	bool	m_bCreate;
};
class CMutexLockAuto
{
public:
	CMutexLockAuto(pthread_mutex_t& obj);
	~CMutexLockAuto();
protected:
	pthread_mutex_t* m_hObject;				//linux
};

#endif // !defined(AFX_MUTEXLOCKLINUX_H__FDC365C0_4E88_4C5D_8846_6D22FF9655E3__INCLUDED_)
