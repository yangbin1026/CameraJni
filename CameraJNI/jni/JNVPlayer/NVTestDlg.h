// NVTestDlg.h : header file
//

#if !defined(AFX_NVTESTDLG_H__D656DFD5_EB00_4746_90D9_44B8964BBECB__INCLUDED_)
#define AFX_NVTESTDLG_H__D656DFD5_EB00_4746_90D9_44B8964BBECB__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

#include "JNVPlayerMng.h"
#include "Draw/JDDraw.h"
#include "Sound/JDSound.h"

#define	TIMER_REC	0
/////////////////////////////////////////////////////////////////////////////
// CNVTestDlg dialog

class CNVTestDlg : public CDialog
{
// Construction
public:
	CNVTestDlg(CWnd* pParent = NULL);	// standard constructor

// Dialog Data
	//{{AFX_DATA(CNVTestDlg)
	enum { IDD = IDD_NVTEST_DIALOG };
	CStatic	m_stView;
	//}}AFX_DATA

	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CNVTestDlg)
	protected:
	virtual void DoDataExchange(CDataExchange* pDX);	// DDX/DDV support
	//}}AFX_VIRTUAL

// Implementation
public:
	int OnDecFrame(PSTDecFrameCB pstDecFrameCB);
protected:
	HICON			m_hIcon;
	CJNVPlayerMng	m_jnvMng;
	long			m_lStream;
	long			m_lRecFile;
	long			m_lReplay;
	long			m_lTalk;

	WAVEFORMATEX	m_wfxWav;
	CJDDraw			m_drawWnd;
	CJDSound		m_dsIn;
	CJDSound		m_dsOut;
	// Generated message map functions
	//{{AFX_MSG(CNVTestDlg)
	virtual BOOL OnInitDialog();
	afx_msg void OnBtGetDevList();
	afx_msg void OnBtGetGps();
	afx_msg void OnBtCloseGps();
	afx_msg void OnBtGetRecList();
	afx_msg void OnBtGetRecDown();
	afx_msg void OnBtOpenStream();
	afx_msg void OnBtCloseStream();
	afx_msg void OnBtLogin();
	afx_msg void OnBtLogout();
	afx_msg void OnDestroy();
	afx_msg void OnBtCapture();
	afx_msg void OnBtStartRec();
	afx_msg void OnBtStopRec();
	afx_msg void OnBtOpenRec();
	afx_msg void OnBtCloseRec();
	afx_msg void OnBtCaptureRec();
	afx_msg void OnBtOpenReplay();
	afx_msg void OnBtPauseReplay();
	afx_msg void OnBtCloseReplay();
	afx_msg void OnBtGetAlarm();
	afx_msg void OnBtCloseAlarm();
	afx_msg void OnBtPauseRec();
	afx_msg void OnBtContinueRec();
	afx_msg void OnBtTest();
	afx_msg void OnBtStartTalk();
	afx_msg void OnBtStopTalk();
	afx_msg void OnTimer(UINT nIDEvent);
	afx_msg void OnBtGetServerList();
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()
};

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_NVTESTDLG_H__D656DFD5_EB00_4746_90D9_44B8964BBECB__INCLUDED_)
