// MutexLockLinux.cpp: implementation of the CMutexLock class.
//
//////////////////////////////////////////////////////////////////////

#include "MutexLockLinux.h"
//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////

CMutexLock::CMutexLock()
{
	m_bCreate	= false;
}

CMutexLock::~CMutexLock()
{
	CloseMutex();
}

bool CMutexLock::CreateMutex()
{
	// 初始化锁的属性
	pthread_mutexattr_t attr;
	pthread_mutexattr_init(&attr);
	pthread_mutexattr_settype(&attr, PTHREAD_MUTEX_RECURSIVE);	// 设置锁的属性为可递归
	
	// 设置锁的属性
	if (pthread_mutex_init(&m_hObject, &attr)==0)
	{
		// 销毁
		pthread_mutexattr_destroy(&attr);
		m_bCreate	= true;
		return true;
	}
	else
	{
		// 销毁
		pthread_mutexattr_destroy(&attr);
		m_bCreate	= false;
		return false;
	}
}

bool CMutexLock::Lock()
{
	if ( m_bCreate==false ) return false;
	pthread_mutex_lock(&m_hObject);
	return true;
}

bool CMutexLock::Unlock()
{
	if ( m_bCreate==false ) return false;
	pthread_mutex_unlock(&m_hObject); 
	return true;
}

bool CMutexLock::IsCreate()
{
	return m_bCreate;
}

bool CMutexLock:: CloseMutex()
{
	if ( m_bCreate==false ) return true;
	pthread_mutex_destroy(&m_hObject); 
	m_bCreate	= false;
	return true;
}

CMutexLockAuto::CMutexLockAuto(pthread_mutex_t& obj)
{
	m_hObject = &obj;
	pthread_mutex_lock(m_hObject);
}
CMutexLockAuto::~CMutexLockAuto()
{
	pthread_mutex_unlock(m_hObject);
}
