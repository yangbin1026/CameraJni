// ImgConver.h: interface for the CImgConver class.
//
//////////////////////////////////////////////////////////////////////

#if !defined(AFX_IMGCONVER_H__8045ABBC_7E36_4F6C_9743_B8700829677B__INCLUDED_)
#define AFX_IMGCONVER_H__8045ABBC_7E36_4F6C_9743_B8700829677B__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

#define AV_HAVE_BIGENDIAN 0
#include "Decoder.h"

#if (defined _WIN32) || (defined _WIN64)
#else
typedef struct tagBITMAPFILEHEADER
{
	WORD	bfType;
	DWORD	bfSize;
	WORD	bfReserved1;
	WORD	bfReserved2;
	DWORD	bfOffBits;
} BITMAPFILEHEADER, LPBITMAPFILEHEADER, *PBITMAPFILEHEADER;
typedef struct tagBITMAPINFOHEADER
{
    DWORD	biSize;
    LONG	biWidth;
    LONG	biHeight;
    WORD	biPlanes;
    WORD	biBitCount;
    DWORD	biCompression;
    DWORD	biSizeImage;
    LONG	biXPelsPerMeter;
    LONG	biYPelsPerMeter;
    DWORD	biClrUsed;
    DWORD	biClrImportant;
} BITMAPINFOHEADER, *LPBITMAPINFOHEADER, *PBITMAPINFOHEADER;
typedef struct tagRGBQUAD {
        BYTE    rgbBlue;
        BYTE    rgbGreen;
        BYTE    rgbRed;
        BYTE    rgbReserved;
} RGBQUAD,* LPRGBQUAD;
typedef struct tagBITMAPINFO {
    BITMAPINFOHEADER    bmiHeader;
    RGBQUAD             bmiColors[1];
} BITMAPINFO, *LPBITMAPINFO, *PBITMAPINFO;

#define BI_RGB        0L
#define BI_RLE8       1L
#define BI_RLE4       2L
#define BI_BITFIELDS  3L
#endif

class CImgConver  
{
public:
	CImgConver();
	virtual ~CImgConver();

	int YUV_2_RGB(PSTYUVFrameAttr pstYUV, unsigned char* pbyTag, int iSrcFM,int iTagFM);
	int RGB_2_YUV(PSTRGBFrameAttr pstRGB, unsigned char* pbyTag, int iSrcFM,int iTagFM);

	int YUV420_2_RGB32(PSTYUVFrameAttr pstYUV, unsigned char* pbyTag);
	int YUV420_2_RGB565(PSTYUVFrameAttr pstYUV, unsigned char* pbyTag);
	int RGB32_2_YUV420(PSTRGBFrameAttr pstRGB, unsigned char* pbyTag);
protected:
};
#endif // !defined(AFX_IMGCONVER_H__8045ABBC_7E36_4F6C_9743_B8700829677B__INCLUDED_)
