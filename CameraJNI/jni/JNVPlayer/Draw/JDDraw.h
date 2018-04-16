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

#define		JDRAW_RGB_32		1					// 32位的RGB
#define		JDRAW_RGB_24		2					// 24位的RGB
#define		JDRAW_RGB_565		3					// 16位的RGB(565)

#define		JDRAW_CLR_BK		0x000000

#define		JDRAW_ERR_SUCC		0					// 成功
#define		JDRAW_ERR_MODE		-1					// 模式错误
#define		JDRAW_ERR_CREATE	-2					// 创建错误
#define		JDRAW_ERR_DRAW		-3					// 绘制错误
class CJDDraw  
{
protected:
public:
	CJDDraw();
	virtual ~CJDDraw();

	int InitDD(PSTDisplayDev pstDispDev,HWND hDrawWnd,bool bFullScreen,int iRGBMode);	// 初始化DD
	int InitBackSurface(int iSrcHeight,int iSrcWidth,bool bUseYUV);						// 初始化后页面缓冲区
	LPDIRECTDRAW GetDirectDraw();				// 获取使用的DirectDraw对象
	int SetDisplayCallBack(fcbDisplay* pFun,PSTDisplayCB pstDisplayCB);	// 设置显示的回调函数
	int SetWndPosOffset(LPCRECT lpOffsetRect);	// 设置显示位置偏移量
	int	SetSrcPosOffset(LPCRECT lpOffsetRect);	// 设置源位置及拉伸倍数
	int DestoryBackSurface();					// 销毁后页面缓冲区
	int DestoryDD();							// 销毁DD
	int FillSurfaceColor(LPDIRECTDRAWSURFACE lpdds,LPRECT lpRectWnd,LPRECT lpRectDraw,int iClr);	// 填充表面
	int DrawFrame(LPBYTE lpFrame,long lFrameLen,time_t tDevTime);		// 画一帧
	int Redraw();								// 重绘(用于画面维护)
	int SetScaleType(int iScaleType);			// 设置放伸方式
	int GetScaleType();							// 获取放伸方式
	bool SetNoTearing(bool bEnable);			// 抗裂痕
	bool IsNoTearing();							// 获取当前抗裂痕设置
	int GetWidth();								// 获取宽度
	int GetHeight();							// 获取高度
protected:
	char* GetErr(HRESULT hErr,char* szBuf);		// 返回错误码
	HRESULT ResetDD();							// 重置DD
	int CheckReadBuf();							// 检查读缓存
	int DrawBack(LPBYTE lpFrame,long lFrameLen);// 绘制到背屏
	LPDIRECTDRAWSURFACE DrawCallback(LPRECT lpRectDraw,LPRECT lpRectSrc);	// 绘制回调
	void DrawCallback();						// 绘制回调(完成后)
	bool GetDrawRect(LPRECT lpRectDraw,LPRECT lpRectSrc,LPRECT lpRectWnd);
	bool DrawToFront(LPRECT lpRectWnd,LPRECT lpRectDraw,LPRECT lpRectSrc,LPDIRECTDRAWSURFACE lpBackDDS);

public:
	int						m_iID;				// 序号
protected:
	DWORD					m_dwSystemVer;		// 系统版本号
	LPDIRECTDRAW			m_pDD;				// DirectDraw对象
	LPDIRECTDRAWSURFACE		m_pDDSFront;		// 前页面
	LPDIRECTDRAWSURFACE		m_pDDSBack;			// 后页面
	LPDIRECTDRAWSURFACE		m_pDDSOSD;			// OSD页面
	LPDIRECTDRAWCLIPPER		m_pDDClip;			// 裁剪板
	DDCAPS					m_ddcapsDrv;		// DDraw能力
	DDCAPS					m_ddcapsHel;		// DDraw能力
	stDisplayDev			m_stDispDev;		// 显示设备
	bool					m_bFullScreen;		// 是否全屏
	HWND					m_hDrawWnd;			// 需要绘制的窗口
	RECT					m_rtWndOffset;		// 显示区域偏移量
	RECT					m_rtSrcOffset;		// 显示源区域偏移量
	time_t					m_tDevTime;			// 设备时间
	CMutexLock				m_mutexFrameBuf;	// 帧缓存锁

	int						m_iRGBMode;			// RGB模式
	int						m_iRGBDeep;			// RGB的色深
	bool					m_bUseYUV;			// 是否使用YUV
	fcbDisplay*				m_pcbDisplay;		// 回调函数
	PSTDisplayCB			m_pstDisplayCB;		// 回调函数的用户参数

	int						m_iWidth;			// 视频宽
	int						m_iHeight;			// 视频高
	long					m_lBackSize;		// 背屏大小
	AVPScaleType			m_eScaleType;		// 放伸方式
	bool					m_bNoTearing;		// 抗裂痕

//	CJGetRunTime			m_jgrt;
	long					m_lHasRead;			// 读出数
	long					m_lHasWrite;		// 写入数
};

#endif // !defined(AFX_JDDRAW_H__7BFE8A69_5A0D_40EB_9C27_D9C0977FEE77__INCLUDED_)
