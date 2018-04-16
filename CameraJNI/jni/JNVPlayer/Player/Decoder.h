#ifndef _DECODER_
#define	_DECODER_

#include "j_vs_ver.h"
#include "JLogWriter.h"

// 音频类型定义 begin
#define		JAV_AT_G711_A					0x01				// 64kbps G.711 A, see RFC3551.txt  4.5.14 PCMA
#define		JAV_AT_G711_U					0x02				// 64kbps G.711 U, see RFC3551.txt  4.5.14 PCMU
#define		JAV_AT_G726						0x03				// 16kbps G.726, see RFC3551.txt  4.5.4 G726-16
// IMA ADPCM wave type
#define		JAV_AT_ADPCM_IMA				0x23				// 32kbps ADPCM(IMA)
// 音频类型定义 end

// 定义解码方式 begin
#define		JAV_VT_H264						0x01				// H264
#define		JAV_VT_MJPEG					0x02				// MJPEG
// 定义解码方式 end

//自定义类管理 解码器

// 解码出来的YUV描述 begin
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
		m_iSrcType	= 0;	// 见JAV_VT_H264和JAV_VT_MJPEG
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
// 解码出来的YUV描述 end

// 解码出来的RGB描述 begin
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
// 解码出来的RGB描述 end

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
	// WIN 的 FFMPEG 版本不一样
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
	virtual long Init();																// 初始化
	virtual long UnInit();																// 结束
	virtual long SetMediaType(int iWidth,int iHeight,int iVideoType,int iAudioType);	// 设置媒体类型
	virtual long VideoDec(BYTE* pInBuf,int iInLen,PSTYUVFrameAttr pstYUV);				// 视频解码
	virtual long GetRemainPic();														// 获取剩余的图像
	virtual long AudioEnc(BYTE* pInBuf,int iInLen,BYTE* pOutBuf,int* piOutLen);			// 音频编码
	virtual long AudioDec(BYTE* pInBuf,int iInLen,BYTE* pOutBuf,int* piOutLen);			// 音频解码


private:
	static BOOL			s_bRun;						// 是否运行中
	static int			s_iObjNum;					// 对象个数
	static CMutexLock	s_hMutexCodec;				// 对象锁

	AVCodecContext*		m_pCodecCtx;				// 解码上下文   //解码器数据结构对象
	CodecID				m_eDecodeType;				// 解码器类型
	AVCodec*			m_pAVCodec;					// 解码器
	AVFrame*			m_pAVFrame;					// 需要解码的帧
	AVFrame*			m_pAVFrameTmp;				// 需要解码的帧(临时)
	BOOL				m_bInit;					// 是否初始化
	int					m_iFlag;					// 运行中的状态

	int					m_iImgH;					// 上一图像宽
	int					m_iImgW;					// 上一图像高
	int					m_iVideoType;				// 上一视频类型
	int					m_iAudioType;				// 上一音频类型

	unsigned char*		m_szFrameBuf;				// 帧缓存
	int					m_iBufLen;					// 帧缓存长
}* LPDecoder;
#endif //FFMPEG_DECODER_AUDIO_H
