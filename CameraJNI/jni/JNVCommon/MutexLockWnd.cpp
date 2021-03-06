// MutexLockWnd.cpp: implementation of the CMutexLock class.
//
//////////////////////////////////////////////////////////////////////

#include "MutexLockWnd.h"
//#include "windows.h"

//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////

CMutexLock::CMutexLock()
{
	m_hObject	= NULL;
	m_bCreate	= false;
}

CMutexLock::~CMutexLock()
{
	CloseMutex();
}

bool CMutexLock::CreateMutex()
{
	m_hObject	= (long)::CreateMutex(NULL,FALSE,NULL);
	if (m_hObject==NULL) return false;
	m_bCreate	= true;
	return true;
}

bool CMutexLock::Lock()
{
	if ( m_bCreate==false ) return false;
	::WaitForSingleObject((HANDLE)m_hObject, INFINITE);
	return true;
}

bool CMutexLock::Unlock()
{
	if ( m_bCreate==false ) return false;
	::ReleaseMutex((HANDLE)m_hObject);
	return true;
}

bool CMutexLock::IsCreate()
{
	return m_bCreate;
}

bool CMutexLock::CloseMutex()
{
	if ( m_bCreate==false ) return true;
	::CloseHandle((HANDLE)m_hObject);
	m_hObject		= NULL;
	m_bCreate		= false;
	return true;
}
