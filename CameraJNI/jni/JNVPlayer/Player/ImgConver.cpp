// ImgConver.cpp: implementation of the CImgConver class.
//
//////////////////////////////////////////////////////////////////////

#include "ImgConver.h"

//////////////////////////////////////////////////////////////////////
// Construction/Destruction  ͼƬ��ʽת��
//////////////////////////////////////////////////////////////////////

CImgConver::CImgConver()
{

}

CImgConver::~CImgConver()
{

}

int CImgConver::YUV_2_RGB(PSTYUVFrameAttr pstYUV, unsigned char* pbyTag, int iSrcFM,int iTagFM)
{
	int			len		= 0;
	SwsContext*	pSwsCtx;
	AVPicture*	pAVSrc = NULL;
	AVPicture*	pAVDsc = NULL;

	pSwsCtx = sws_getContext(pstYUV->m_uWidth, pstYUV->m_uHeight, (PixelFormat)iSrcFM,
			 pstYUV->m_uWidth, pstYUV->m_uHeight, (PixelFormat)iTagFM, SWS_BILINEAR, NULL, NULL, NULL);
	
	if(pSwsCtx)
	{
		pAVSrc = (AVPicture*)avcodec_alloc_frame();	
		pAVDsc = (AVPicture*)avcodec_alloc_frame();	

		avpicture_fill(pAVSrc, pstYUV->m_pY,(PixelFormat)iSrcFM, pstYUV->m_uWidth, pstYUV->m_uHeight);
		avpicture_fill(pAVDsc, pbyTag, (PixelFormat)iTagFM, pstYUV->m_uWidth, pstYUV->m_uHeight);
		sws_scale(pSwsCtx, pAVSrc->data, pAVSrc->linesize, 0, pstYUV->m_uHeight, pAVDsc->data, pAVDsc->linesize);
		len = pstYUV->m_uWidth*pstYUV->m_uHeight*4;
		av_free(pAVSrc);
		av_free(pAVDsc);
	}
	else
	{
		return -1;
	}

	sws_freeContext(pSwsCtx);
	return len;
}

int CImgConver::RGB_2_YUV(PSTRGBFrameAttr pstRGB, unsigned char* pbyTag, int iSrcFM,int iTagFM)
{
	int			len		= 0;
	SwsContext*	pSwsCtx;
	AVPicture*	pAVSrc = NULL;
	AVPicture*	pAVDsc = NULL;

	pSwsCtx = sws_getContext(pstRGB->m_uWidth, pstRGB->m_uHeight, (PixelFormat)iSrcFM,
			 pstRGB->m_uWidth, pstRGB->m_uHeight, (PixelFormat)iTagFM, SWS_BILINEAR, NULL, NULL, NULL);
	
	if(pSwsCtx)
	{
		pAVSrc = (AVPicture*)avcodec_alloc_frame();	
		pAVDsc = (AVPicture*)avcodec_alloc_frame();	

		avpicture_fill(pAVSrc, pstRGB->m_pRGB,(PixelFormat)iSrcFM, pstRGB->m_uWidth, pstRGB->m_uHeight);
		avpicture_fill(pAVDsc, pbyTag, (PixelFormat)iTagFM, pstRGB->m_uWidth, pstRGB->m_uHeight);
		sws_scale(pSwsCtx, pAVSrc->data, pAVSrc->linesize, 0, pstRGB->m_uHeight, pAVDsc->data, pAVDsc->linesize);
		len = (pstRGB->m_uWidth*pstRGB->m_uHeight*3)/2;
		av_free(pAVSrc);
		av_free(pAVDsc);
	}
	else
	{
		return -1;
	}

	sws_freeContext(pSwsCtx);
	return len;
}

////////////////////////////////////////////////////////////
// Pix format convert
int CImgConver::YUV420_2_RGB32(PSTYUVFrameAttr pstYUV, unsigned char* pbyTag)
{
	int			len		= 0;
	SwsContext*	pSwsCtx;
	AVPicture*	pAVSrc = NULL;
	AVPicture*	pAVDsc = NULL;

	pSwsCtx = sws_getContext(pstYUV->m_uWidth, pstYUV->m_uHeight, PIX_FMT_YUV420P,
		//	 pstYUV->m_uWidth, pstYUV->m_uHeight, PIX_FMT_BGRA, SWS_BILINEAR, NULL, NULL, NULL);
			 pstYUV->m_uWidth, pstYUV->m_uHeight, PIX_FMT_RGBA, SWS_BILINEAR, NULL, NULL, NULL);
	
	if(pSwsCtx)
	{
		pAVSrc = (AVPicture*)avcodec_alloc_frame();	
		pAVDsc = (AVPicture*)avcodec_alloc_frame();	

		avpicture_fill(pAVSrc, pstYUV->m_pY,PIX_FMT_YUV420P, pstYUV->m_uWidth, pstYUV->m_uHeight);
		avpicture_fill(pAVDsc, pbyTag, PIX_FMT_BGRA, pstYUV->m_uWidth, pstYUV->m_uHeight);
		sws_scale(pSwsCtx, pAVSrc->data, pAVSrc->linesize, 0, pstYUV->m_uHeight, pAVDsc->data, pAVDsc->linesize);
		len = pstYUV->m_uWidth*pstYUV->m_uHeight*4;
		av_free(pAVSrc);
		av_free(pAVDsc);
	}
	else
	{
		return -1;
	}

	sws_freeContext(pSwsCtx);
	return len;
}
int CImgConver::YUV420_2_RGB565(PSTYUVFrameAttr pstYUV, unsigned char* pbyTag)
{
	int			len		= 0;
	SwsContext*	pSwsCtx;
	AVPicture*	pAVSrc = NULL;
	AVPicture*	pAVDsc = NULL;

	pSwsCtx = sws_getContext(pstYUV->m_uWidth, pstYUV->m_uHeight, PIX_FMT_YUV420P,
			 pstYUV->m_uWidth, pstYUV->m_uHeight, PIX_FMT_RGB565, SWS_BILINEAR, NULL, NULL, NULL);
	
	if(pSwsCtx)
	{
		pAVSrc = (AVPicture*)avcodec_alloc_frame();	
		pAVDsc = (AVPicture*)avcodec_alloc_frame();	

		avpicture_fill(pAVSrc, pstYUV->m_pY,PIX_FMT_YUV420P, pstYUV->m_uWidth, pstYUV->m_uHeight);
		avpicture_fill(pAVDsc, pbyTag, PIX_FMT_RGB565, pstYUV->m_uWidth, pstYUV->m_uHeight);
		sws_scale(pSwsCtx, pAVSrc->data, pAVSrc->linesize, 0, pstYUV->m_uHeight, pAVDsc->data, pAVDsc->linesize);
		len = pstYUV->m_uWidth*pstYUV->m_uHeight*4;
		av_free(pAVSrc);
		av_free(pAVDsc);
	}
	else
	{
		return -1;
	}

	sws_freeContext(pSwsCtx);
	return len;
}
int CImgConver::RGB32_2_YUV420(PSTRGBFrameAttr pstRGB, unsigned char* pbyTag)
{
	int			len		= 0;
	SwsContext*	pSwsCtx;
	AVPicture*	pAVSrc = NULL;
	AVPicture*	pAVDsc = NULL;

	pSwsCtx = sws_getContext(pstRGB->m_uWidth, pstRGB->m_uHeight, PIX_FMT_BGRA,
			 pstRGB->m_uWidth, pstRGB->m_uHeight, PIX_FMT_YUV420P, SWS_BILINEAR, NULL, NULL, NULL);
	
	if(pSwsCtx)
	{
		pAVSrc = (AVPicture*)avcodec_alloc_frame();	
		pAVDsc = (AVPicture*)avcodec_alloc_frame();	

		avpicture_fill(pAVSrc, pstRGB->m_pRGB,PIX_FMT_BGRA, pstRGB->m_uWidth, pstRGB->m_uHeight);
		avpicture_fill(pAVDsc, pbyTag, PIX_FMT_YUV420P, pstRGB->m_uWidth, pstRGB->m_uHeight);
		sws_scale(pSwsCtx, pAVSrc->data, pAVSrc->linesize, 0, pstRGB->m_uHeight, pAVDsc->data, pAVDsc->linesize);
		len = (pstRGB->m_uWidth*pstRGB->m_uHeight*3)/2;
		av_free(pAVSrc);
		av_free(pAVDsc);
	}
	else
	{
		return -1;
	}

	sws_freeContext(pSwsCtx);
	return len;
}


