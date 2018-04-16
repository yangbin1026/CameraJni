// JDDraw.h: interface for the CJDDraw class.
//
//////////////////////////////////////////////////////////////////////

#if !defined(AFX_JDDRAW_H__7BFE8A69_5A0D_40EB_9C27_D9C0977FEE77__INCLUDED_)
#define AFX_JDDRAW_H__7BFE8A69_5A0D_40EB_9C27_D9C0977FEE77__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

#include "lib_def.h"
#include "JLogWriter.h"
#include "ddraw.h"
#include "MutexLockWnd.h"

#define	MAX_FRAME_BUF	3
#ifndef MAKEFOURCC
#define MAKEFOURCC(ch0, ch1, ch2, ch3)   ((DWORD)(BYTE)(ch0) | ((DWORD)(BYTE)(ch1) << 8) |   ((DWORD)(BYTE)(ch2) << 16) | ((DWORD)(BYTE)(ch3) << 24 ))
#endif

#define		JDRAW_RGB_32		1					// 32λ��RGB
#define		JDRAW_RGB_24		2					// 24λ��RGB
#define		JDRAW_RGB_565		3					// 16λ��RGB(565)

#define		JDRAW_CLR_BK		0x000000

#define		JDRAW_ERR_SUCC		0					// �ɹ�
#define		JDRAW_ERR_MODE		-1					// ģʽ����
#define		JDRAW_ERR_CREATE	-2					// ��������
#define		JDRAW_ERR_DRAW		-3					// ���ƴ���
class CJDDraw  
{
protected:
public:
	CJDDraw();
	virtual ~CJDDraw();

	int InitDD(PSTDisplayDev pstDispDev,HWND hDrawWnd,bool bFullScreen,int iRGBMode);	// ��ʼ��DD
	int InitBackSurface(int iSrcHeight,int iSrcWidth,bool bUseYUV);						// ��ʼ����ҳ�滺����
	LPDIRECTDRAW GetDirectDraw();				// ��ȡʹ�õ�DirectDraw����
	int SetDisplayCallBack(fcbDisplay* pFun,PSTDisplayCB pstDisplayCB);	// ������ʾ�Ļص�����
	int SetWndPosOffset(LPCRECT lpOffsetRect);	// ������ʾλ��ƫ����
	int	SetSrcPosOffset(LPCRECT lpOffsetRect);	// ����Դλ�ü����챶��
	int DestoryBackSurface();					// ���ٺ�ҳ�滺����
	int DestoryDD();							// ����DD
	int FillSurfaceColor(LPDIRECTDRAWSURFACE lpdds,LPRECT lpRectWnd,LPRECT lpRectDraw,int iClr);	// ������
	int DrawFrame(LPBYTE lpFrame,long lFrameLen,time_t tDevTime);		// ��һ֡
	int Redraw();								// �ػ�(���ڻ���ά��)
	int SetScaleType(int iScaleType);			// ���÷��췽ʽ
	int GetScaleType();							// ��ȡ���췽ʽ
	bool SetNoTearing(bool bEnable);			// ���Ѻ�
	bool IsNoTearing();							// ��ȡ��ǰ���Ѻ�����
	int GetWidth();								// ��ȡ���
	int GetHeight();							// ��ȡ�߶�
protected:
	char* GetErr(HRESULT hErr,char* szBuf);		// ���ش�����
	HRESULT ResetDD();							// ����DD
	int CheckReadBuf();							// ��������
	int DrawBack(LPBYTE lpFrame,long lFrameLen);// ���Ƶ�����
	LPDIRECTDRAWSURFACE DrawCallback(LPRECT lpRectDraw,LPRECT lpRectSrc);	// ���ƻص�
	void DrawCallback();						// ���ƻص�(��ɺ�)
	bool GetDrawRect(LPRECT lpRectDraw,LPRECT lpRectSrc,LPRECT lpRectWnd);
	bool DrawToFront(LPRECT lpRectWnd,LPRECT lpRectDraw,LPRECT lpRectSrc,LPDIRECTDRAWSURFACE lpBackDDS);

public:
	int						m_iID;				// ���
protected:
	DWORD					m_dwSystemVer;		// ϵͳ�汾��
	LPDIRECTDRAW			m_pDD;				// DirectDraw����
	LPDIRECTDRAWSURFACE		m_pDDSFront;		// ǰҳ��
	LPDIRECTDRAWSURFACE		m_pDDSBack;			// ��ҳ��
	LPDIRECTDRAWSURFACE		m_pDDSOSD;			// OSDҳ��
	LPDIRECTDRAWCLIPPER		m_pDDClip;			// �ü���
	DDCAPS					m_ddcapsDrv;		// DDraw����
	DDCAPS					m_ddcapsHel;		// DDraw����
	stDisplayDev			m_stDispDev;		// ��ʾ�豸
	bool					m_bFullScreen;		// �Ƿ�ȫ��
	HWND					m_hDrawWnd;			// ��Ҫ���ƵĴ���
	RECT					m_rtWndOffset;		// ��ʾ����ƫ����
	RECT					m_rtSrcOffset;		// ��ʾԴ����ƫ����
	time_t					m_tDevTime;			// �豸ʱ��
	CMutexLock				m_mutexFrameBuf;	// ֡������

	int						m_iRGBMode;			// RGBģʽ
	int						m_iRGBDeep;			// RGB��ɫ��
	bool					m_bUseYUV;			// �Ƿ�ʹ��YUV
	fcbDisplay*				m_pcbDisplay;		// �ص�����
	PSTDisplayCB			m_pstDisplayCB;		// �ص��������û�����

	int						m_iWidth;			// ��Ƶ��
	int						m_iHeight;			// ��Ƶ��
	long					m_lBackSize;		// ������С
	AVPScaleType			m_eScaleType;		// ���췽ʽ
	bool					m_bNoTearing;		// ���Ѻ�

//	CJGetRunTime			m_jgrt;
	long					m_lHasRead;			// ������
	long					m_lHasWrite;		// д����
};

#endif // !defined(AFX_JDDRAW_H__7BFE8A69_5A0D_40EB_9C27_D9C0977FEE77__INCLUDED_)
