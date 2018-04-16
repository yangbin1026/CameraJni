#ifndef _DECODER_
#define	_DECODER_

#include "j_vs_ver.h"
#include "JLogWriter.h"

// ��Ƶ���Ͷ��� begin
#define		JAV_AT_G711_A					0x01				// 64kbps G.711 A, see RFC3551.txt  4.5.14 PCMA
#define		JAV_AT_G711_U					0x02				// 64kbps G.711 U, see RFC3551.txt  4.5.14 PCMU
#define		JAV_AT_G726						0x03				// 16kbps G.726, see RFC3551.txt  4.5.4 G726-16
// IMA ADPCM wave type
#define		JAV_AT_ADPCM_IMA				0x23				// 32kbps ADPCM(IMA)
// ��Ƶ���Ͷ��� end

// ������뷽ʽ begin
#define		JAV_VT_H264						0x01				// H264
#define		JAV_VT_MJPEG					0x02				// MJPEG
// ������뷽ʽ end

//�Զ�������� ������

// ���������YUV���� begin
typedef struct stYUVFrameAttr
{
	int		m_iSrcType;
	unsigned char*	m_pY;
	unsigned char*	m_pU;
	unsigned char*	m_pV;
	UINT	m_uWidth;
	UINT	m_uHeight;
	UINT	m_uYStride;
	UINT	m_uUStride;
	UINT	m_uVStride;
	stYUVFrameAttr()
	{
		m_iSrcType	= 0;	// ��JAV_VT_H264��JAV_VT_MJPEG
		m_pY		= NULL;
		m_pU		= NULL;
		m_pV		= NULL;
		m_uWidth	= 0;
		m_uHeight	= 0;
		m_uYStride	= 0;
		m_uUStride	= 0;
		m_uVStride	= 0;
	}
}* PSTYUVFrameAttr;
// ���������YUV���� end

// ���������RGB���� begin
typedef struct stRGBFrameAttr
{
	unsigned char*	m_pRGB;
	UINT	m_uWidth;
	UINT	m_uHeight;
	stRGBFrameAttr()
	{
		m_pRGB		= NULL;
		m_uWidth	= 0;
		m_uHeight	= 0;
	}
}* PSTRGBFrameAttr;
// ���������RGB���� end

typedef enum
{
	CODEC_NONE = 0,
	CODEC_DECODE_OPEN = 1,
	CODEC_ENCODE_OPEN = 2,
}CodecFlag;

extern "C" 
{
#if (defined _WIN32) || (defined _WIN64)
#else
	// WIN �� FFMPEG �汾��һ��
#include "libavformat/avformat.h"
#endif
#include "libavcodec/avcodec.h"
#include "libswscale/swscale.h"
}
#include "Audio/g72x.h"
#include "Audio/g726.h"

typedef class CDecoder
{
public:
    CDecoder();
    ~CDecoder();

public:
	virtual long Init();																// ��ʼ��
	virtual long UnInit();																// ����
	virtual long SetMediaType(int iWidth,int iHeight,int iVideoType,int iAudioType);	// ����ý������
	virtual long VideoDec(BYTE* pInBuf,int iInLen,PSTYUVFrameAttr pstYUV);				// ��Ƶ����
	virtual long GetRemainPic();														// ��ȡʣ���ͼ��
	virtual long AudioEnc(BYTE* pInBuf,int iInLen,BYTE* pOutBuf,int* piOutLen);			// ��Ƶ����
	virtual long AudioDec(BYTE* pInBuf,int iInLen,BYTE* pOutBuf,int* piOutLen);			// ��Ƶ����


private:
	static BOOL			s_bRun;						// �Ƿ�������
	static int			s_iObjNum;					// �������
	static CMutexLock	s_hMutexCodec;				// ������

	AVCodecContext*		m_pCodecCtx;				// ����������   //���������ݽṹ����
	CodecID				m_eDecodeType;				// ����������
	AVCodec*			m_pAVCodec;					// ������
	AVFrame*			m_pAVFrame;					// ��Ҫ�����֡
	AVFrame*			m_pAVFrameTmp;				// ��Ҫ�����֡(��ʱ)
	BOOL				m_bInit;					// �Ƿ��ʼ��
	int					m_iFlag;					// �����е�״̬

	int					m_iImgH;					// ��һͼ���
	int					m_iImgW;					// ��һͼ���
	int					m_iVideoType;				// ��һ��Ƶ����
	int					m_iAudioType;				// ��һ��Ƶ����

	unsigned char*		m_szFrameBuf;				// ֡����
	int					m_iBufLen;					// ֡���泤
}* LPDecoder;
#endif //FFMPEG_DECODER_AUDIO_H
