// NVTest.h : main header file for the NVTEST application
//

#if !defined(AFX_NVTEST_H__9F052A62_9624_4153_8760_B258FA1EC7DE__INCLUDED_)
#define AFX_NVTEST_H__9F052A62_9624_4153_8760_B258FA1EC7DE__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

#ifndef __AFXWIN_H__
	#error include 'stdafx.h' before including this file for PCH
#endif

#include "resource.h"		// main symbols

/////////////////////////////////////////////////////////////////////////////
// CNVTestApp:
// See NVTest.cpp for the implementation of this class
//

class CNVTestApp : public CWinApp
{
public:
	CNVTestApp();

// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CNVTestApp)
	public:
	virtual BOOL InitInstance();
	virtual int ExitInstance();
	//}}AFX_VIRTUAL

// Implementation

	//{{AFX_MSG(CNVTestApp)
		// NOTE - the ClassWizard will add and remove member functions here.
		//    DO NOT EDIT what you see in these blocks of generated code !
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()
};


/////////////////////////////////////////////////////////////////////////////

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_NVTEST_H__9F052A62_9624_4153_8760_B258FA1EC7DE__INCLUDED_)
