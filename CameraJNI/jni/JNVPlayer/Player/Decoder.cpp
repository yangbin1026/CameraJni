#include "Decoder.h"
#undef	LOG_TAG
#define	LOG_TAG		"Decoder.cpp"

////////////////////////////////////////////
BOOL		CDecoder::s_bRun			= FALSE;		// 是否运行中
int			CDecoder::s_iObjNum			= 0;			// 对象个数
CMutexLock	CDecoder::s_hMutexCodec;
////////////////////////////////////////////

//自定义类管理 解码器
CDecoder::CDecoder()
{
	m_pCodecCtx		= NULL;				// 解码上下文
	m_eDecodeType	= CODEC_ID_NONE;	// 解码器类型
	m_pAVCodec		= NULL;				// 解码器
	m_pAVFrame		= NULL;				// 需要解码的帧
	m_pAVFrameTmp	= NULL;				// 需要解码的帧(临时)
	m_bInit			= FALSE;			// 是否初始化
	m_iFlag			= 0;				// 运行中的状态

	m_iImgH			= 0;				// 上一图像宽
	m_iImgW			= 0;				// 上一图像高
	m_iVideoType	= 0;				// 上一视频类型
	m_iAudioType	= 0;				// 上一音频类型
	
	m_szFrameBuf	= NULL;				// 帧缓存
	m_iBufLen		= 0;				// 帧缓存长

	if(s_iObjNum==0)
	{
		s_hMutexCodec.CreateMutex();
	}
	s_iObjNum++;

	Init();
	JTRACE("CDecoder::Create codec obj\r\n");
	return; 
}
//////////////////////////////////
CDecoder::~CDecoder()
{
	UnInit();
	s_iObjNum--;
	if(s_iObjNum==0)
	{
		s_hMutexCodec.CloseMutex();
	}
	JTRACE("CDecoder::Destory codec obj\r\n");
	return;
}
////////////////////////////////////
long CDecoder::Init()
{
	s_hMutexCodec.Lock();

	if(m_bInit)
	{
		s_hMutexCodec.Unlock();
		return 0;
	}
	if(s_bRun==FALSE)
	{
		s_bRun = TRUE;
		JTRACE("CDecoder::Init():Init decodec dll \r\n");
		#if (defined _WIN32) || (defined _WIN64)
		avcodec_init();
		#else
		av_register_all();
		#endif
		avcodec_register_all();
	}

	// 分配资源
	m_pCodecCtx		= avcodec_alloc_context();		// 解码上下文
	m_pAVFrame		= avcodec_alloc_frame();		// 需要解码的帧
	m_pAVFrameTmp	= avcodec_alloc_frame();		// 需要解码的帧(临时)
	if(m_pCodecCtx == NULL || m_pAVFrame == NULL || m_pAVFrameTmp == NULL)
	{
		// 分配资源失败
		if(m_pCodecCtx)		av_free(m_pCodecCtx);	// 解码上下文
		if(m_pAVFrame)		av_free(m_pAVFrame);	// 需要解码的帧
		if(m_pAVFrameTmp)	av_free(m_pAVFrameTmp);	// 需要解码的帧(临时)
		s_hMutexCodec.Unlock();
		return -1;
	}
	
	// 初始化成功
	m_bInit = TRUE;
	s_hMutexCodec.Unlock();
	JTRACE("CDecoder::Init():Init codec sucess\r\n");
	
	return 0;
}
//////////////////////////////////////////
long CDecoder::UnInit()
{
	int result;
	s_hMutexCodec.Lock();
	if(m_bInit==FALSE)
	{
		s_hMutexCodec.Unlock();
		return 0;
	}
	// 关闭解码器
	#if (defined _WIN32) || (defined _WIN64)
	if(m_iFlag&CODEC_DECODE_OPEN)
	{
		avcodec_decode_video(m_pCodecCtx, m_pAVFrame, &result, NULL, 0);
	}
	#else
	AVPacket packet = {0};
	packet.data = NULL;
	packet.size = 0;
	if(m_iFlag&CODEC_DECODE_OPEN)
	{
		avcodec_decode_video2(m_pCodecCtx, m_pAVFrame, &result, &packet);
	}
	#endif
	// 清空上下文
	if(m_pCodecCtx)
	{
		avcodec_close(m_pCodecCtx);
		av_free(m_pCodecCtx);
		m_pCodecCtx = NULL;
	}
	// 回收解码帧
	if(m_pAVFrame)
	{
		av_free(m_pAVFrame);
		m_pAVFrame = NULL;
	}
	if(m_pAVFrameTmp)
	{
		av_free(m_pAVFrameTmp);
		m_pAVFrameTmp = NULL;
	}
	// 清空缓存
	if(m_szFrameBuf)
	{
		delete m_szFrameBuf;
		m_szFrameBuf	= NULL;
		m_iBufLen		= 0;
	}
	// 反初始化成功
	m_iFlag	= 0;
	m_bInit	=FALSE;
	s_hMutexCodec.Unlock();
	JTRACE("CDecoder::UnInit():Uninit codec\r\n");
	return 0;
}
/**
 * iVideoType: 上一视频类型，
 *
 *		JAV_VT_H264						0x01				// H264
 *		JAV_VT_MJPEG					0x02				// MJPEG
 *
 * iAudioType: 上一音频类型
 */
long CDecoder::SetMediaType(int iWidth,int iHeight,int iVideoType,int iAudioType)
{
		JTRACE("CDecoder::SetMediaType(): m_iVideoType = %d\r\n",m_iVideoType);
		JTRACE("CDecoder::SetMediaType(): iWidth = %d iHeight=%d \r\n",iWidth,iHeight);
		JTRACE("CDecoder::SetMediaType(): iAudioType = %d\r\n",iAudioType);
	if(iWidth!=-1)		m_iImgW			= iWidth;		// 上一图像宽
	if(iHeight!=-1)		m_iImgH			= iHeight;		// 上一图像高
	if(iVideoType!=-1)	m_iVideoType	= iVideoType;	// 上一视频类型
	if(iAudioType!=-1)	m_iAudioType	= iAudioType;	// 上一音频类型
	if(m_iVideoType == JAV_VT_H264)
	{
		m_eDecodeType = CODEC_ID_H264;
	}
	else if(m_iVideoType == JAV_VT_MJPEG)
	{
		m_eDecodeType = CODEC_ID_MJPEG;
	}
	else 
	{
		m_eDecodeType = CODEC_ID_MPEG4;
	}
	if(m_iAudioType!=iAudioType)
	{
		m_iAudioType = iAudioType;
	}
	if(m_szFrameBuf)
	{
		delete m_szFrameBuf;
		m_szFrameBuf	= NULL;
		m_iBufLen		= 0;
	}

	return 0;
}
long CDecoder::VideoDec(BYTE* pInBuf,int iInLen, PSTYUVFrameAttr pstYUV)
{
	int len, result;

//	s_hMutexCodec.Lock();
	// 打开解码器 begin
	if( (m_iFlag & CODEC_DECODE_OPEN) == 0)
	{
		m_pAVCodec = avcodec_find_decoder(m_eDecodeType);
		if(m_pAVCodec == NULL)
		{
			JTRACE("CDecoder::VideoDec(): AVCodec is NUll");
//			s_hMutexCodec.Unlock();
			return -1;
		}
		
		if(m_pAVCodec->capabilities & CODEC_CAP_TRUNCATED)
		{
			m_pCodecCtx->flags |= CODEC_FLAG_TRUNCATED;
		}

		if(avcodec_open(m_pCodecCtx, m_pAVCodec) < 0)
		{
			m_pAVCodec = NULL;
			JTRACE("CDecoder::avcodec_open(): AVCodec <0");
//			s_hMutexCodec.Unlock();
			return -1;
		}
		
		m_iFlag |= CODEC_DECODE_OPEN;
	}
	// 打开解码器 begin

	#if (defined _WIN32) || (defined _WIN64)
	len = avcodec_decode_video(m_pCodecCtx, m_pAVFrame, &result, pInBuf, iInLen);
	#else
	AVPacket packet = {0};
	packet.data = pInBuf;
	packet.size = iInLen;
	len = avcodec_decode_video2(m_pCodecCtx, m_pAVFrame, &result, &packet);
	#endif
	if(result)
	{
		if(pstYUV == NULL)
		{
			JTRACE("CDecoder::VideoDec(): pstYUV is NUll");
//			s_hMutexCodec.Unlock();
			return -1;
		}
		// 申请缓存
		if(m_szFrameBuf==NULL)
		{	
			m_iBufLen = avpicture_get_size((PixelFormat)(m_pCodecCtx->pix_fmt), m_pCodecCtx->width,
				m_pCodecCtx->height);
			JTRACE("CDecoder::BUffLength: %ld",m_iBufLen);
			m_szFrameBuf =  new unsigned char[m_iBufLen];
			if(m_szFrameBuf == NULL)
			{
				m_iBufLen = 0;
				JTRACE("CDecoder::VideoDec(): m_szFrameBuf is NUll");
//				s_hMutexCodec.Unlock();
				return -1;
			}		
			
			avpicture_fill((AVPicture *)m_pAVFrameTmp, (unsigned char*)m_szFrameBuf, (PixelFormat)(m_pCodecCtx->pix_fmt),
				m_pCodecCtx->width, m_pCodecCtx->height);
		}

		avpicture_layout((AVPicture *)m_pAVFrame, m_pCodecCtx->pix_fmt, m_pCodecCtx->width, m_pCodecCtx->height, m_szFrameBuf, m_iBufLen);
		pstYUV->m_iSrcType = m_iVideoType;
		pstYUV->m_pY = m_pAVFrameTmp->data[0];
		pstYUV->m_pU = m_pAVFrameTmp->data[1];
		pstYUV->m_pV = m_pAVFrameTmp->data[2];
		pstYUV->m_uYStride = m_pAVFrameTmp->linesize[0];
		pstYUV->m_uUStride = m_pAVFrameTmp->linesize[1];
		pstYUV->m_uVStride = m_pAVFrameTmp->linesize[2];
		pstYUV->m_uWidth = m_pCodecCtx->width;
		pstYUV->m_uHeight = m_pCodecCtx->height;
	}
	else
	{
		JTRACE("CDecoder::VideoDec(): avcodec_decode_video2 FAIL!");
//		s_hMutexCodec.Unlock();
		return -1;
	}
//	s_hMutexCodec.Unlock();

	return iInLen;
}
long CDecoder::GetRemainPic()
{
	
	return -1;
}
//音频编码
long CDecoder::AudioEnc(BYTE* pInBuf,int iInLen,BYTE* pOutBuf,int* piOutLen)
{
	int		iRet	= 0;

	if ( m_iAudioType==JAV_AT_G726 )
	{
		g726_Encode((unsigned char*)pInBuf,(char*)pOutBuf+4,iRet,40);
		pOutBuf[0]	= 0x00;
		pOutBuf[1]	= 0x01;
		pOutBuf[2]	= 0x32;
		pOutBuf[3]	= 0x00;
		iRet	= 104;
		if ( piOutLen ) *piOutLen = iRet;
	}
	return iRet;
}
//音频解码
long CDecoder::AudioDec(BYTE* pInBuf,int iInLen,BYTE* pOutBuf,int* piOutLen)
{
	int		iRet	= 0;

	if ( m_iAudioType==JAV_AT_G726 )
	{
		g726_Decode((char*)pInBuf,(unsigned char*)pOutBuf,40);
		iRet	= 320;
		if ( piOutLen ) *piOutLen = iRet;
	}
	return iRet;
}

