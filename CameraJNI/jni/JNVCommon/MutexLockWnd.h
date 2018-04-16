// MutexLockWnd.h: interface for the CMutexLock class.
//
//////////////////////////////////////////////////////////////////////

#if !defined(AFX_MUTEXLOCKWND_H_JSOCK__FDC365C0_4E88_4C5D_8846_6D22FF9655E3__INCLUDED_)
#define AFX_MUTEXLOCKWND_H_JSOCK__FDC365C0_4E88_4C5D_8846_6D22FF9655E3__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

#include "j_vs_ver.h"

namespace JSocketCtrl
{
class CMutexLock 
{
public:
	CMutexLock();
	virtual ~CMutexLock();

	bool CreateMutex();
	bool Lock();
	bool Unlock();
	bool IsCreate();
	bool CloseMutex();

public:
protected:
	long	m_hObject;						// Ëø¶ÔÏó
	bool	m_bCreate;

};
}
using namespace JSocketCtrl;

#endif // !defined(AFX_MUTEXLOCKWND_H_JSOCK__FDC365C0_4E88_4C5D_8846_6D22FF9655E3__INCLUDED_)
