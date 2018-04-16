// JNVPlayer.h: interface for the CJNVPlayer class.
//
//////////////////////////////////////////////////////////////////////

#if !defined(AFX_JNVPLAYER_H__200E7991_AE85_44FD_B12A_AC5D99DA6359__INCLUDED_)
#define AFX_JNVPLAYER_H__200E7991_AE85_44FD_B12A_AC5D99DA6359__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

#ifdef __ANDROID__
#include "jni.h"
#endif

#include "ProtocolNV.h"
#include "JBuffPool.h"
#include "JLThreadCtrl.h"
#include "JGetRunTimeTC.h"

#include "JNVCmdTcp.h"
#include "JRecFile.h"
#include "Decoder.h"
#include "ImgConver.h"

// 定义单帧运行方式 begin
typedef enum 
{ 
	PORT_PF_NONE	= 0x00000000,	// 非单帧模式
	PORT_PF_NEXT	= 0x00000001,	// 单帧向前
	PORT_PF_BACK	= 0x00000002,	// 单帧向后
}ePortPFrameMode;
// 定义单帧运行方式 end

#define		JDRAW_RGB_NONE		0					// 非RGB(为YUV)
#define		JDRAW_RGB_32		1					// 32位的RGB
#define		JDRAW_RGB_24		2					// 24位的RGB
#define		JDRAW_RGB_565		3					// 16位的RGB(565)

class CJNVPlayer;		typedef class CJNVPlayer*	LPJNVPlayer;
class CJNVPlayer  
{
public:
	CJNVPlayer();
	virtual ~CJNVPlayer();

	void Release();							// 释放
	int GetID();							// 获取ID
	int SetID(int iID);						// 设置ID

	int PutFrame(void *pFrame);				// 加入一帧
	int SetDecRGBFM(int iRGBFM);			// 解码图像的格式:JDRAW_RGB_32,JDRAW_RGB_24,JDRAW_RGB_565
	int SetDecCallBack(fcbDecFrame* pFun, long lParam);
	int SetPlayInfoCallBack(fcbPlay* pFun, long lParam);
	void SendCallBackMsg(AVPPlayCBMsg eMsg);
	
	int OpenFile(LPCTSTR lpszPath);			// 打开录像文件
	int	Play();								// 播放
	int	Stop();								// 停止
	int	Pause();							// 暂停
	int	SetPlaySpeed(int iSpeed);			// 设置播放速度
	int	GetPlaySpeed();						// 获取当前播放速度
	int	NextFrame();						// 下一帧
	int Seek(long lTime);					// 设置播放的时间

	int Capture(LPCTSTR lpszPath);			// 抓图
	long RecStart(LPCTSTR lpszPath);		// 开始录像
	long RecStop();							// 停止录像
protected:
	void	ReadFileAction();								// 处理读文件的动作
	bool	DecAction();									// 处理解码的动作
	float	GetSpeedValue();								// 获取速度值

	void	WaitForNextFrame(LONGLONG lFrameMTime);			// 等待下一帧
	bool	DecBuff(const BYTE* pBuf,int iSize,bool bWait);	// 解码
	
	bool	PrepareFrameBuffSize(int iLen);					// 设置音频缓存大小
	bool	PrepareFrameBuff(PSTYUVFrameAttr pstYUV);		// 设置图像缓存大小
	bool	SetImageSize(int iImgWidth,int iImgHeight);		// 设置图像大小
	static fJThRet RunReadFileThread(void* pParam);
	static fJThRet RunDecThread(void* pParam);
protected:
	CJLThreadCtrl	m_tcReadFile;					// 读文件线程
	CJLThreadCtrl	m_tcDec;						// 解码线程

	#ifdef __ANDROID__
	JNIEnv*			m_pDecEnv;						// JAVA环境指针
	jbyteArray		m_arDecBuf;						// JNI中的BUF
	jbyteArray		m_arDecAudioBuf;				// JNI中的BUF
	LPBYTE			m_lpAudioFrame;					// 当前帧的BUFF
	#endif
	int				m_iID;							// ID
	int				m_iImgRGBFM;					// 解码后图像RGB格式
	long			m_lImgWidth;					// 绘制的帧宽
	long			m_lImgHeight;					// 绘制的帧高
	DWORD			m_dwProtocol;					// 数据协议
	LPDecoder		m_lpDecoder;					// 解码适配器
	CFMReader*		m_pReaderFrame;					// 格式适配器
	CFMReader*		m_pReaderPut;					// 格式适配器
	CJBuffPool		m_bpWaitDec;					// 待解码队列
	bool			m_bAccecpPutIn;					// 是否接受输入
	bool			m_bPause;						// 是否暂停
	CImgConver		m_imgConver;					// 图形转换器

	AVPStreamType	m_eStreamType;					// 流类型
	AVPPlayStatus	m_ePlayStatus;					// 播放状态	
	LONGLONG		m_lFrameTime;					// 帧时间
	long			m_lLastFrameID;					// 上一帧的帧号
	AVPPlaySpeed	m_ePlaySpeed;					// 播放速度
	int				m_iDecBufCount;					// 解码缓存大小
	int				m_iDecBufPoolSize;				// 缓存池大小
	fcbDecFrame*	m_pcbDecFrame;					// 解码回调函数
	stDecFrameCB	m_stDecFrameCB;					// 解码回调时的回调参数
	CJGetRunTimeTC	m_jgrtDec;						// 解码间隔
	LPBYTE			m_lpFrame;						// 当前帧的BUFF
	long			m_lFrameBufLen;					// 当前帧缓存的大小
	bool			m_bNeedIFrame;					// 需要I帧

	ePortPFrameMode	m_ePFMode;						// 单帧运行模式
	int				m_iPFSlider;					// 单帧运行时的滑行数

	long			m_lRead;						// 读入数
	long			m_lDec;							// 解码数
	CMutexLock		m_mutexStop;					// 关闭锁

	CJRecFile		m_recFile;						// 录像文件
	time_t			m_tPlayedTime;					// 播放了的时间
	FILE*			m_pfRec;						// 录像文件
	fcbPlay*		m_pcbPlayInfo;					// 播放信息回调
	long			m_lPlayInfoParam;				// 播放信息回调参数
};

#endif // !defined(AFX_JNVPLAYER_H__200E7991_AE85_44FD_B12A_AC5D99DA6359__INCLUDED_)
