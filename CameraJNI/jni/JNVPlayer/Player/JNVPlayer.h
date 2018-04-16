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

// ���嵥֡���з�ʽ begin
typedef enum 
{ 
	PORT_PF_NONE	= 0x00000000,	// �ǵ�֡ģʽ
	PORT_PF_NEXT	= 0x00000001,	// ��֡��ǰ
	PORT_PF_BACK	= 0x00000002,	// ��֡���
}ePortPFrameMode;
// ���嵥֡���з�ʽ end

#define		JDRAW_RGB_NONE		0					// ��RGB(ΪYUV)
#define		JDRAW_RGB_32		1					// 32λ��RGB
#define		JDRAW_RGB_24		2					// 24λ��RGB
#define		JDRAW_RGB_565		3					// 16λ��RGB(565)

class CJNVPlayer;		typedef class CJNVPlayer*	LPJNVPlayer;
class CJNVPlayer  
{
public:
	CJNVPlayer();
	virtual ~CJNVPlayer();

	void Release();							// �ͷ�
	int GetID();							// ��ȡID
	int SetID(int iID);						// ����ID

	int PutFrame(void *pFrame);				// ����һ֡
	int SetDecRGBFM(int iRGBFM);			// ����ͼ��ĸ�ʽ:JDRAW_RGB_32,JDRAW_RGB_24,JDRAW_RGB_565
	int SetDecCallBack(fcbDecFrame* pFun, long lParam);
	int SetPlayInfoCallBack(fcbPlay* pFun, long lParam);
	void SendCallBackMsg(AVPPlayCBMsg eMsg);
	
	int OpenFile(LPCTSTR lpszPath);			// ��¼���ļ�
	int	Play();								// ����
	int	Stop();								// ֹͣ
	int	Pause();							// ��ͣ
	int	SetPlaySpeed(int iSpeed);			// ���ò����ٶ�
	int	GetPlaySpeed();						// ��ȡ��ǰ�����ٶ�
	int	NextFrame();						// ��һ֡
	int Seek(long lTime);					// ���ò��ŵ�ʱ��

	int Capture(LPCTSTR lpszPath);			// ץͼ
	long RecStart(LPCTSTR lpszPath);		// ��ʼ¼��
	long RecStop();							// ֹͣ¼��
protected:
	void	ReadFileAction();								// ������ļ��Ķ���
	bool	DecAction();									// �������Ķ���
	float	GetSpeedValue();								// ��ȡ�ٶ�ֵ

	void	WaitForNextFrame(LONGLONG lFrameMTime);			// �ȴ���һ֡
	bool	DecBuff(const BYTE* pBuf,int iSize,bool bWait);	// ����
	
	bool	PrepareFrameBuffSize(int iLen);					// ������Ƶ�����С
	bool	PrepareFrameBuff(PSTYUVFrameAttr pstYUV);		// ����ͼ�񻺴��С
	bool	SetImageSize(int iImgWidth,int iImgHeight);		// ����ͼ���С
	static fJThRet RunReadFileThread(void* pParam);
	static fJThRet RunDecThread(void* pParam);
protected:
	CJLThreadCtrl	m_tcReadFile;					// ���ļ��߳�
	CJLThreadCtrl	m_tcDec;						// �����߳�

	#ifdef __ANDROID__
	JNIEnv*			m_pDecEnv;						// JAVA����ָ��
	jbyteArray		m_arDecBuf;						// JNI�е�BUF
	jbyteArray		m_arDecAudioBuf;				// JNI�е�BUF
	LPBYTE			m_lpAudioFrame;					// ��ǰ֡��BUFF
	#endif
	int				m_iID;							// ID
	int				m_iImgRGBFM;					// �����ͼ��RGB��ʽ
	long			m_lImgWidth;					// ���Ƶ�֡��
	long			m_lImgHeight;					// ���Ƶ�֡��
	DWORD			m_dwProtocol;					// ����Э��
	LPDecoder		m_lpDecoder;					// ����������
	CFMReader*		m_pReaderFrame;					// ��ʽ������
	CFMReader*		m_pReaderPut;					// ��ʽ������
	CJBuffPool		m_bpWaitDec;					// ���������
	bool			m_bAccecpPutIn;					// �Ƿ��������
	bool			m_bPause;						// �Ƿ���ͣ
	CImgConver		m_imgConver;					// ͼ��ת����

	AVPStreamType	m_eStreamType;					// ������
	AVPPlayStatus	m_ePlayStatus;					// ����״̬	
	LONGLONG		m_lFrameTime;					// ֡ʱ��
	long			m_lLastFrameID;					// ��һ֡��֡��
	AVPPlaySpeed	m_ePlaySpeed;					// �����ٶ�
	int				m_iDecBufCount;					// ���뻺���С
	int				m_iDecBufPoolSize;				// ����ش�С
	fcbDecFrame*	m_pcbDecFrame;					// ����ص�����
	stDecFrameCB	m_stDecFrameCB;					// ����ص�ʱ�Ļص�����
	CJGetRunTimeTC	m_jgrtDec;						// ������
	LPBYTE			m_lpFrame;						// ��ǰ֡��BUFF
	long			m_lFrameBufLen;					// ��ǰ֡����Ĵ�С
	bool			m_bNeedIFrame;					// ��ҪI֡

	ePortPFrameMode	m_ePFMode;						// ��֡����ģʽ
	int				m_iPFSlider;					// ��֡����ʱ�Ļ�����

	long			m_lRead;						// ������
	long			m_lDec;							// ������
	CMutexLock		m_mutexStop;					// �ر���

	CJRecFile		m_recFile;						// ¼���ļ�
	time_t			m_tPlayedTime;					// �����˵�ʱ��
	FILE*			m_pfRec;						// ¼���ļ�
	fcbPlay*		m_pcbPlayInfo;					// ������Ϣ�ص�
	long			m_lPlayInfoParam;				// ������Ϣ�ص�����
};

#endif // !defined(AFX_JNVPLAYER_H__200E7991_AE85_44FD_B12A_AC5D99DA6359__INCLUDED_)
