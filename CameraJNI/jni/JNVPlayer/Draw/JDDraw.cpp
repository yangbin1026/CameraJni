// JJDDraw.cpp: implementation of the CJDDraw class.
//
//////////////////////////////////////////////////////////////////////

#include "stdafx.h"
#include "JDDraw.h"

#ifndef RECT_WIDTH
#define	RECT_WIDTH(lpRect) lpRect->right-lpRect->left
#endif
#ifndef RECT_HEIGHT
#define	RECT_HEIGHT(lpRect) lpRect->bottom-lpRect->top
#endif

//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////

// 返回错误码
char* CJDDraw::GetErr(HRESULT hErr,char* szBuf)
{
	switch(hErr)
	{
	case DDERR_ALREADYINITIALIZED		:		strcpy_s(szBuf,1024,"DDERR_ALREADYINITIALIZED		: The object has already been initialized.  ");	break;
	case DDERR_BLTFASTCANTCLIP			:		strcpy_s(szBuf,1024,"DDERR_BLTFASTCANTCLIP			: A DirectDrawClipper object is attached to a source surface that has passed into a call to the IDirectDrawSurface3::BltFast method.  ");	break;
	case DDERR_CANNOTATTACHSURFACE		:		strcpy_s(szBuf,1024,"DDERR_CANNOTATTACHSURFACE		: A surface cannot be attached to another requested surface.  ");	break;
	case DDERR_CANNOTDETACHSURFACE		:		strcpy_s(szBuf,1024,"DDERR_CANNOTDETACHSURFACE		: A surface cannot be detached from another requested surface.  ");	break;
	case DDERR_CANTCREATEDC				:		strcpy_s(szBuf,1024,"DDERR_CANTCREATEDC				: Windows cannot create any more device contexts (DCs).  ");	break;
	case DDERR_CANTDUPLICATE			:		strcpy_s(szBuf,1024,"DDERR_CANTDUPLICATE			: Primary and 3-D surfaces, or surfaces that are implicitly created, cannot be duplicated.  ");	break;
	case DDERR_CANTLOCKSURFACE			:		strcpy_s(szBuf,1024,"DDERR_CANTLOCKSURFACE			: Access to this surface is refused because an attempt was made to lock the primary surface without DCI support.  ");	break;
	case DDERR_CANTPAGELOCK				:		strcpy_s(szBuf,1024,"DDERR_CANTPAGELOCK				: An attempt to page lock a surface failed. Page lock will not work on a display-memory surface or an emulated primary surface.  ");	break;
	case DDERR_CANTPAGEUNLOCK			:		strcpy_s(szBuf,1024,"DDERR_CANTPAGEUNLOCK			: An attempt to page unlock a surface failed. Page unlock will not work on a display-memory surface or an emulated primary surface.  ");	break;
	case DDERR_CLIPPERISUSINGHWND		:		strcpy_s(szBuf,1024,"DDERR_CLIPPERISUSINGHWND		: An attempt was made to set a clip list for a DirectDrawClipper object that is already monitoring a window handle.  ");	break;
	case DDERR_COLORKEYNOTSET			:		strcpy_s(szBuf,1024,"DDERR_COLORKEYNOTSET			: No source color key is specified for this operation.  ");	break;
	case DDERR_CURRENTLYNOTAVAIL		:		strcpy_s(szBuf,1024,"DDERR_CURRENTLYNOTAVAIL		: No support is currently available.  ");	break;
	case DDERR_DCALREADYCREATED			:		strcpy_s(szBuf,1024,"DDERR_DCALREADYCREATED			: A device context (DC) has already been returned for this surface. Only one DC can be retrieved for each surface.  ");	break;
	case DDERR_DEVICEDOESNTOWNSURFACE	:		strcpy_s(szBuf,1024,"DDERR_DEVICEDOESNTOWNSURFACE	: Surfaces created by one direct draw device cannot be used directly by another direct draw device. ");	break;
	case DDERR_DIRECTDRAWALREADYCREATED	:		strcpy_s(szBuf,1024,"DDERR_DIRECTDRAWALREADYCREATED	: A DirectDraw object representing this driver has already been created for this process.  ");	break;
	case DDERR_EXCEPTION				:		strcpy_s(szBuf,1024,"DDERR_EXCEPTION				: An exception was encountered while performing the requested operation.  ");	break;
	case DDERR_EXCLUSIVEMODEALREADYSET	:		strcpy_s(szBuf,1024,"DDERR_EXCLUSIVEMODEALREADYSET	: An attempt was made to set the cooperative level when it was already set to exclusive.  ");	break;
	case DDERR_GENERIC					:		strcpy_s(szBuf,1024,"DDERR_GENERIC					: There is an undefined error condition.  ");	break;
	case DDERR_HEIGHTALIGN				:		strcpy_s(szBuf,1024,"DDERR_HEIGHTALIGN				: The height of the provided rectangle is not a multiple of the required alignment.  ");	break;
	case DDERR_HWNDALREADYSET			:		strcpy_s(szBuf,1024,"DDERR_HWNDALREADYSET			: The DirectDraw cooperative level window handle has already been set. It cannot be reset while the process has surfaces or palettes created.  ");	break;
	case DDERR_HWNDSUBCLASSED			:		strcpy_s(szBuf,1024,"DDERR_HWNDSUBCLASSED			: DirectDraw is prevented from restoring state because the DirectDraw cooperative level window handle has been subclassed.  ");	break;
	case DDERR_IMPLICITLYCREATED		:		strcpy_s(szBuf,1024,"DDERR_IMPLICITLYCREATED		: The surface cannot be restored because it is an implicitly created surface.  ");	break;
	case DDERR_INCOMPATIBLEPRIMARY		:		strcpy_s(szBuf,1024,"DDERR_INCOMPATIBLEPRIMARY		: The primary surface creation request does not match with the existing primary surface.  ");	break;
	case DDERR_INVALIDCAPS				:		strcpy_s(szBuf,1024,"DDERR_INVALIDCAPS				: One or more of the capability bits passed to the callback function are incorrect.  ");	break;
	case DDERR_INVALIDCLIPLIST			:		strcpy_s(szBuf,1024,"DDERR_INVALIDCLIPLIST			: DirectDraw does not support the provided clip list.  ");	break;
	case DDERR_INVALIDDIRECTDRAWGUID	:		strcpy_s(szBuf,1024,"DDERR_INVALIDDIRECTDRAWGUID	: The globally unique identifier (GUID) passed to the DirectDrawCreate function is not a valid DirectDraw driver identifier.  ");	break;
	case DDERR_INVALIDMODE				:		strcpy_s(szBuf,1024,"DDERR_INVALIDMODE				: DirectDraw does not support the requested mode.  ");	break;
	case DDERR_INVALIDOBJECT			:		strcpy_s(szBuf,1024,"DDERR_INVALIDOBJECT			: DirectDraw received a pointer that was an invalid DirectDraw object.  ");	break;
	case DDERR_INVALIDPARAMS			:		strcpy_s(szBuf,1024,"DDERR_INVALIDPARAMS			: One or more of the parameters passed to the method are incorrect.  ");	break;
	case DDERR_INVALIDPIXELFORMAT		:		strcpy_s(szBuf,1024,"DDERR_INVALIDPIXELFORMAT		: The pixel format was invalid as specified.  ");	break;
	case DDERR_INVALIDPOSITION			:		strcpy_s(szBuf,1024,"DDERR_INVALIDPOSITION			: The position of the overlay on the destination is no longer legal.  ");	break;
	case DDERR_INVALIDRECT				:		strcpy_s(szBuf,1024,"DDERR_INVALIDRECT				: The provided rectangle was invalid.  ");	break;
	case DDERR_INVALIDSURFACETYPE		:		strcpy_s(szBuf,1024,"DDERR_INVALIDSURFACETYPE		: The requested operation could not be performed because the surface was of the wrong type.  ");	break;
	case DDERR_LOCKEDSURFACES			:		strcpy_s(szBuf,1024,"DDERR_LOCKEDSURFACES			: One or more surfaces are locked, causing the failure of the requested operation.  ");	break;
	case DDERR_MOREDATA					:		strcpy_s(szBuf,1024,"DDERR_MOREDATA					: There is more data available than the specified buffer size can hold. ");	break;
	case DDERR_NO3D						:		strcpy_s(szBuf,1024,"DDERR_NO3D						: No 3-D hardware or emulation is present.  ");	break;
	case DDERR_NOALPHAHW				:		strcpy_s(szBuf,1024,"DDERR_NOALPHAHW				: No alpha acceleration hardware is present or available, causing the failure of the requested operation.  ");	break;
	case DDERR_NOBLTHW					:		strcpy_s(szBuf,1024,"DDERR_NOBLTHW					: No blitter hardware is present.  ");	break;
	case DDERR_NOCLIPLIST				:		strcpy_s(szBuf,1024,"DDERR_NOCLIPLIST				: No clip list is available.  ");	break;
	case DDERR_NOCLIPPERATTACHED		:		strcpy_s(szBuf,1024,"DDERR_NOCLIPPERATTACHED		: No DirectDrawClipper object is attached to the surface object.  ");	break;
	case DDERR_NOCOLORCONVHW			:		strcpy_s(szBuf,1024,"DDERR_NOCOLORCONVHW			: The operation cannot be carried out because no color-conversion hardware is present or available.  ");	break;
	case DDERR_NOCOLORKEY				:		strcpy_s(szBuf,1024,"DDERR_NOCOLORKEY				: The surface does not currently have a color key.  ");	break;
	case DDERR_NOCOLORKEYHW				:		strcpy_s(szBuf,1024,"DDERR_NOCOLORKEYHW				: The operation cannot be carried out because there is no hardware support for the destination color key.  ");	break;
	case DDERR_NOCOOPERATIVELEVELSET	:		strcpy_s(szBuf,1024,"DDERR_NOCOOPERATIVELEVELSET	: A create function is called without the IDirectDraw2::SetCooperativeLevel method being called.  ");	break;
	case DDERR_NODC						:		strcpy_s(szBuf,1024,"DDERR_NODC						: No DC has ever been created for this surface.  ");	break;
	case DDERR_NODDROPSHW				:		strcpy_s(szBuf,1024,"DDERR_NODDROPSHW				: No DirectDraw raster operation (ROP) hardware is available.  ");	break;
	case DDERR_NODIRECTDRAWHW			:		strcpy_s(szBuf,1024,"DDERR_NODIRECTDRAWHW			: Hardware-only DirectDraw object creation is not possible; the driver does not support any hardware.  ");	break;
	case DDERR_NODIRECTDRAWSUPPORT		:		strcpy_s(szBuf,1024,"DDERR_NODIRECTDRAWSUPPORT		: DirectDraw support is not possible with the current display driver.  ");	break;
	case DDERR_NOEMULATION				:		strcpy_s(szBuf,1024,"DDERR_NOEMULATION				: Software emulation is not available.  ");	break;
	case DDERR_NOEXCLUSIVEMODE			:		strcpy_s(szBuf,1024,"DDERR_NOEXCLUSIVEMODE			: The operation requires the application to have exclusive mode, but the application does not have exclusive mode.  ");	break;
	case DDERR_NOFLIPHW					:		strcpy_s(szBuf,1024,"DDERR_NOFLIPHW					: Flipping visible surfaces is not supported.  ");	break;
	case DDERR_NOFOCUSWINDOW			:		strcpy_s(szBuf,1024,"DDERR_NOFOCUSWINDOW			: An attempt was made to create or set a device window without first setting the focus window. ");	break;
	case DDERR_NOGDI					:		strcpy_s(szBuf,1024,"DDERR_NOGDI					: No GDI is present.  ");	break;
	case DDERR_NOHWND					:		strcpy_s(szBuf,1024,"DDERR_NOHWND					: Clipper notification requires a window handle, or no window handle has been previously set as the cooperative level window handle.  ");	break;
	case DDERR_NOMIPMAPHW				:		strcpy_s(szBuf,1024,"DDERR_NOMIPMAPHW				: The operation cannot be carried out because no mipmap texture mapping hardware is present or available.  ");	break;
	case DDERR_NOMIRRORHW				:		strcpy_s(szBuf,1024,"DDERR_NOMIRRORHW				: The operation cannot be carried out because no mirroring hardware is present or available.  ");	break;
	case DDERR_NONONLOCALVIDMEM			:		strcpy_s(szBuf,1024,"DDERR_NONONLOCALVIDMEM			: An attempt was made to allocate non-local video memory from a device that does not support non-local video memory. ");	break;
	case DDERR_NOOPTIMIZEHW				:		strcpy_s(szBuf,1024,"DDERR_NOOPTIMIZEHW				: The device does not support optimized surfaces. ");	break;
	case DDERR_NOOVERLAYDEST			:		strcpy_s(szBuf,1024,"DDERR_NOOVERLAYDEST			: The IDirectDrawSurface3::GetOverlayPosition method is called on an overlay that the IDirectDrawSurface3::UpdateOverlay method has not been called on to establish a destination.  ");	break;
	case DDERR_NOOVERLAYHW				:		strcpy_s(szBuf,1024,"DDERR_NOOVERLAYHW				: The operation cannot be carried out because no overlay hardware is present or available.  ");	break;
	case DDERR_NOPALETTEATTACHED		:		strcpy_s(szBuf,1024,"DDERR_NOPALETTEATTACHED		: No palette object is attached to this surface.  ");	break;
	case DDERR_NOPALETTEHW				:		strcpy_s(szBuf,1024,"DDERR_NOPALETTEHW				: There is no hardware support for 16- or 256-color palettes.  ");	break;
	case DDERR_NORASTEROPHW				:		strcpy_s(szBuf,1024,"DDERR_NORASTEROPHW				: The operation cannot be carried out because no appropriate raster operation hardware is present or available.  ");	break;
	case DDERR_NOROTATIONHW				:		strcpy_s(szBuf,1024,"DDERR_NOROTATIONHW				: The operation cannot be carried out because no rotation hardware is present or available.  ");	break;
	case DDERR_NOSTRETCHHW				:		strcpy_s(szBuf,1024,"DDERR_NOSTRETCHHW				: The operation cannot be carried out because there is no hardware support for stretching.  ");	break;
	case DDERR_NOT4BITCOLOR				:		strcpy_s(szBuf,1024,"DDERR_NOT4BITCOLOR				: The DirectDrawSurface object is not using a 4-bit color palette and the requested operation requires a 4-bit color palette.  ");	break;
	case DDERR_NOT4BITCOLORINDEX		:		strcpy_s(szBuf,1024,"DDERR_NOT4BITCOLORINDEX		: The DirectDrawSurface object is not using a 4-bit color index palette and the requested operation requires a 4-bit color index palette.  ");	break;
	case DDERR_NOT8BITCOLOR				:		strcpy_s(szBuf,1024,"DDERR_NOT8BITCOLOR				: The DirectDrawSurface object is not using an 8-bit color palette and the requested operation requires an 8-bit color palette.  ");	break;
	case DDERR_NOTAOVERLAYSURFACE		:		strcpy_s(szBuf,1024,"DDERR_NOTAOVERLAYSURFACE		: An overlay component is called for a non-overlay surface.  ");	break;
	case DDERR_NOTEXTUREHW				:		strcpy_s(szBuf,1024,"DDERR_NOTEXTUREHW				: The operation cannot be carried out because no texture-mapping hardware is present or available.  ");	break;
	case DDERR_NOTFLIPPABLE				:		strcpy_s(szBuf,1024,"DDERR_NOTFLIPPABLE				: An attempt has been made to flip a surface that cannot be flipped.  ");	break;
	case DDERR_NOTFOUND					:		strcpy_s(szBuf,1024,"DDERR_NOTFOUND					: The requested item was not found.  ");	break;
	case DDERR_NOTINITIALIZED			:		strcpy_s(szBuf,1024,"DDERR_NOTINITIALIZED			: An attempt was made to call an interface method of a DirectDraw object created by CoCreateInstance before the object was initialized.  ");	break;
	case DDERR_NOTLOADED				:		strcpy_s(szBuf,1024,"DDERR_NOTLOADED				: The surface is an optimized surface, but it has not yet been allocated any memory. ");	break;
	case DDERR_NOTLOCKED				:		strcpy_s(szBuf,1024,"DDERR_NOTLOCKED				: An attempt is made to unlock a surface that was not locked.  ");	break;
	case DDERR_NOTPAGELOCKED			:		strcpy_s(szBuf,1024,"DDERR_NOTPAGELOCKED			: An attempt is made to page unlock a surface with no outstanding page locks.  ");	break;
	case DDERR_NOTPALETTIZED			:		strcpy_s(szBuf,1024,"DDERR_NOTPALETTIZED			: The surface being used is not a palette-based surface.  ");	break;
	case DDERR_NOVSYNCHW				:		strcpy_s(szBuf,1024,"DDERR_NOVSYNCHW				: The operation cannot be carried out because there is no hardware support for vertical blank synchronized operations.  ");	break;
	case DDERR_NOZBUFFERHW				:		strcpy_s(szBuf,1024,"DDERR_NOZBUFFERHW				: The operation to create a z-buffer in display memory or to perform a blit using a z-buffer cannot be carried out because there is no hardware support for z-buffers.  ");	break;
	case DDERR_NOZOVERLAYHW				:		strcpy_s(szBuf,1024,"DDERR_NOZOVERLAYHW				: The overlay surfaces cannot be z-layered based on the z-order because the hardware does not support z-ordering of overlays.  ");	break;
	case DDERR_OUTOFCAPS				:		strcpy_s(szBuf,1024,"DDERR_OUTOFCAPS				: The hardware needed for the requested operation has already been allocated.  ");	break;
	case DDERR_OUTOFMEMORY				:		strcpy_s(szBuf,1024,"DDERR_OUTOFMEMORY				: DirectDraw does not have enough memory to perform the operation.  ");	break;
	case DDERR_OUTOFVIDEOMEMORY			:		strcpy_s(szBuf,1024,"DDERR_OUTOFVIDEOMEMORY			: DirectDraw does not have enough display memory to perform the operation.  ");	break;
	case DDERR_OVERLAYCANTCLIP			:		strcpy_s(szBuf,1024,"DDERR_OVERLAYCANTCLIP			: The hardware does not support clipped overlays.  ");	break;
	case DDERR_OVERLAYCOLORKEYONLYONEACTIVE:	strcpy_s(szBuf,1024,"DDERR_OVERLAYCOLORKEYONLYONEACTIVE: An attempt was made to have more than one color key active on an overlay.  ");	break;
	case DDERR_OVERLAYNOTVISIBLE		:		strcpy_s(szBuf,1024,"DDERR_OVERLAYNOTVISIBLE		: The IDirectDrawSurface3::GetOverlayPosition method is called on a hidden overlay.  ");	break;
	case DDERR_PALETTEBUSY				:		strcpy_s(szBuf,1024,"DDERR_PALETTEBUSY				: Access to this palette is refused because the palette is locked by another thread.  ");	break;
	case DDERR_PRIMARYSURFACEALREADYEXISTS:		strcpy_s(szBuf,1024,"DDERR_PRIMARYSURFACEALREADYEXIST: This process has already created a primary surface.  ");	break;
	case DDERR_REGIONTOOSMALL			:		strcpy_s(szBuf,1024,"DDERR_REGIONTOOSMALL			: The region passed to the IDirectDrawClipper::GetClipList method is too small.  ");	break;
	case DDERR_SURFACEALREADYATTACHED	:		strcpy_s(szBuf,1024,"DDERR_SURFACEALREADYATTACHED	: An attempt was made to attach a surface to another surface to which it is already attached.  ");	break;
	case DDERR_SURFACEALREADYDEPENDENT	:		strcpy_s(szBuf,1024,"DDERR_SURFACEALREADYDEPENDENT	: An attempt was made to make a surface a dependency of another surface to which it is already dependent.  ");	break;
	case DDERR_SURFACEBUSY				:		strcpy_s(szBuf,1024,"DDERR_SURFACEBUSY				: Access to the surface is refused because the surface is locked by another thread.  ");	break;
	case DDERR_SURFACEISOBSCURED		:		strcpy_s(szBuf,1024,"DDERR_SURFACEISOBSCURED		: Access to the surface is refused because the surface is obscured.  ");	break;
	case DDERR_SURFACELOST				:		strcpy_s(szBuf,1024,"DDERR_SURFACELOST				: Access to the surface is refused because the surface memory is gone. The DirectDrawSurface object representing this surface should have the IDirectDrawSurface3::Restore method called on it.  ");	break;
	case DDERR_SURFACENOTATTACHED		:		strcpy_s(szBuf,1024,"DDERR_SURFACENOTATTACHED		: The requested surface is not attached.  ");	break;
	case DDERR_TOOBIGHEIGHT				:		strcpy_s(szBuf,1024,"DDERR_TOOBIGHEIGHT				: The height requested by DirectDraw is too large.  ");	break;
	case DDERR_TOOBIGSIZE				:		strcpy_s(szBuf,1024,"DDERR_TOOBIGSIZE				: The size requested by DirectDraw is too large. However, the individual height and width are OK.  ");	break;
	case DDERR_TOOBIGWIDTH				:		strcpy_s(szBuf,1024,"DDERR_TOOBIGWIDTH				: The width requested by DirectDraw is too large.  ");	break;
	case DDERR_UNSUPPORTED				:		strcpy_s(szBuf,1024,"DDERR_UNSUPPORTED				: The operation is not supported.  ");	break;
	case DDERR_UNSUPPORTEDFORMAT		:		strcpy_s(szBuf,1024,"DDERR_UNSUPPORTEDFORMAT		: The FourCC format requested is not supported by DirectDraw.  ");	break;
	case DDERR_UNSUPPORTEDMASK			:		strcpy_s(szBuf,1024,"DDERR_UNSUPPORTEDMASK			: The bitmask in the pixel format requested is not supported by DirectDraw.  ");	break;
	case DDERR_UNSUPPORTEDMODE			:		strcpy_s(szBuf,1024,"DDERR_UNSUPPORTEDMODE			: The display is currently in an unsupported mode.  ");	break;
	case DDERR_VERTICALBLANKINPROGRESS	:		strcpy_s(szBuf,1024,"DDERR_VERTICALBLANKINPROGRESS	: A vertical blank is in progress.  ");	break;
	case DDERR_VIDEONOTACTIVE			:		strcpy_s(szBuf,1024,"DDERR_VIDEONOTACTIVE			: The video port is not active. ");	break;
	case DDERR_WASSTILLDRAWING			:		strcpy_s(szBuf,1024,"DDERR_WASSTILLDRAWING			: The previous blit operation that is transferring information to or from this surface is incomplete.  ");	break;
	case DDERR_WRONGMODE				:		strcpy_s(szBuf,1024,"DDERR_WRONGMODE				: This surface cannot be restored because it was created in a different mode.  ");	break;
	case DDERR_XALIGN					:		strcpy_s(szBuf,1024,"DDERR_XALIGN					: The provided rectangle was not horizontally aligned on a required boundary.  ");	break;
	default:
		sprintf_s(szBuf,1024,"UNKNOW		: hErr=0x%08x (%ld).",hErr,hErr);
		break;
	}

	return szBuf;
}

CJDDraw::CJDDraw()
{
	m_iID			= 0;			// 序号
	m_pcbDisplay	= NULL;			// 回调函数
	m_pstDisplayCB	= NULL;			// 回调函数的用户参数
	m_pDD			= NULL;			// DirectDraw对象
	m_pDDSFront		= NULL;			// 前页面
	m_pDDSBack		= NULL;			// 后页面
	m_pDDSOSD		= NULL;			// OSD页面
	m_pDDClip		= NULL;			// 裁剪板
	m_bFullScreen	= false;		// 是否全屏
	m_hDrawWnd		= NULL;			// 需要绘制的窗口
//	m_stDispDev		= NULL;			// 显示设备
	memset(&m_ddcapsDrv,0,sizeof(m_ddcapsDrv));	m_ddcapsDrv.dwSize	= sizeof(m_ddcapsDrv);
	memset(&m_ddcapsHel,0,sizeof(m_ddcapsHel));	m_ddcapsHel.dwSize	= sizeof(m_ddcapsHel);
	memset(&m_rtWndOffset,0,sizeof(RECT));	// 显示区域偏移量
	memset(&m_rtSrcOffset,0,sizeof(RECT));	// 显示源区域偏移量

	m_iRGBMode		= JDRAW_RGB_32;	// RGB模式
	m_iRGBDeep		= 4;			// RGB的色深
	m_bUseYUV		= FALSE;		// 是否使用YUV
	m_tDevTime		= 0;			// 设备时间
	
	m_iWidth		= 0;			// 视频宽
	m_iHeight		= 0;			// 视频高
	m_lBackSize		= 0;			// 背屏大小
	m_eScaleType	= AVPScaleFull;	// 放伸方式
	m_bNoTearing	= false;		// 抗裂痕
	
	m_lHasRead		= 0;			// 读出数
	m_lHasWrite		= 0;			// 写入数

	// Get windows version begin
	OSVERSIONINFO Ver;
	Ver.dwOSVersionInfoSize = sizeof(OSVERSIONINFO);
	if(GetVersionEx(&Ver))
	{
		m_dwSystemVer = Ver.dwMajorVersion;
	}
	else
	{
		m_dwSystemVer = 0;
	}
	// Get windows version end
}

CJDDraw::~CJDDraw()
{
	DestoryDD();
}

// 初始化
int CJDDraw::InitDD(PSTDisplayDev pstDispDev,HWND hDrawWnd,bool bFullScreen,int iRGBMode)
{
	JLOG_TRY
	HRESULT					hRet		= NULL;			// 用于返回
	DDSURFACEDESC			ddsd;						// 页面属性
	DWORD					dwFlags		= NULL;
	char					szBuf[1024]	= {0};

	// 参数检查及设置 begin
	// 已经初始化过了
	if ( m_pDD ) return 1;

	// RGB模式 begin
	m_iRGBMode = iRGBMode;
	if ( m_iRGBMode==JDRAW_RGB_32 )			m_iRGBDeep = 4;
	else if ( m_iRGBMode==JDRAW_RGB_24 )	m_iRGBDeep = 3;
	else if ( m_iRGBMode==JDRAW_RGB_565 )	m_iRGBDeep = 2;
	else m_bUseYUV = TRUE;//m_iRGBDeep = 4;
	// RGB模式 end

	// 传入的参数不正确
	if ( hDrawWnd==NULL || ::IsWindow(hDrawWnd)==FALSE ) return -1;
	m_hDrawWnd		= hDrawWnd;
	// 参数检查及设置 end

	// 获取显示设备 begin
	m_bFullScreen	= bFullScreen;		// 是否全屏
	if ( pstDispDev ) memcpy(&m_stDispDev,pstDispDev,sizeof(stDisplayDev));
	// 获取显示设备 end

	// 根据窗口大小来确定要显示的源及目标区域
	SetWndPosOffset(NULL);

	// 创建 DirectDraw 对象 begin
	DirectDrawCreate( &(m_stDispDev.stGUID), &m_pDD, NULL);
	// 设置协作层
	dwFlags = DDSCL_NORMAL;
	hRet = m_pDD->SetCooperativeLevel(m_hDrawWnd,dwFlags);
	if ( m_pDD->GetCaps(&m_ddcapsDrv,&m_ddcapsHel)!=DD_OK )
	{
		m_ddcapsDrv.dwCaps	= NULL;
	}
	// 创建 DirectDraw 对象 end

	// 创建前页(主表面) begin
	ZeroMemory(&ddsd, sizeof(ddsd));
	ddsd.dwSize			= sizeof(ddsd);					// 尺寸
	ddsd.ddsCaps.dwCaps	= DDSCAPS_PRIMARYSURFACE;		// 指定我们用的是前页
	ddsd.dwFlags		= DDSD_CAPS;
	hRet				= m_pDD->CreateSurface(&ddsd, &m_pDDSFront, NULL);
	// 当发生错误时, 要记得 Release对象.
	if (hRet!=DD_OK) 
	{
		DbgStrOut("CJDDraw::InitDD :CreateSurface:%s\r\n",GetErr(hRet,szBuf));
		m_pDD->Release();
		m_pDD = NULL;
		return -1;
	}
	// 创建前页(主表面) end

	// 创建裁剪板(为了防止 DirectDraw 画到窗口外面去) begin
	// 使用了 裁剪板 会使BltFast失效
	hRet = m_pDD->CreateClipper(0, &m_pDDClip, NULL);
	hRet = m_pDDClip->SetHWnd(0, m_hDrawWnd);
	// m_pClipper
	hRet = m_pDDSFront->SetClipper(m_pDDClip);
	// 创建裁剪板(为了防止 DirectDraw 画到窗口外面去) end

	m_mutexFrameBuf.CreateMutex();				// 帧缓存锁

	return 0;
	JLOG_CATCH("try-catch CJDDraw::InitDD \r\n")
	return 0;
}

// 初始化后页面缓冲区
int CJDDraw::InitBackSurface(int iSrcHeight,int iSrcWidth,bool bUseYUV)
{
	JLOG_TRY
	HRESULT			hRet		= NULL;			// 用于返回
	DDSURFACEDESC	ddsd;						// 页面属性
	int				i			= 0;
	bool			bLock		= false;
	char			szBuf[1024]	= {0};
	
	if ( m_pDD==NULL ) return -1;

	// 16字节对齐
//	iSrcWidth = ((iSrcWidth+15)/16)*16;
//	iSrcHeight = ((iSrcHeight+15)/16)*16;

	// 格式不变,不需要修改
	if ( iSrcHeight==m_iHeight && iSrcWidth==m_iWidth && bUseYUV==m_bUseYUV )
	{
		return JDRAW_ERR_SUCC;
	}
	
	if ( bUseYUV )
	{
		if ( m_ddcapsDrv.dwCaps&DDCAPS_BLTFOURCC && m_ddcapsDrv.dwCaps&DDCAPS_BLTSTRETCH ) {}
	//	if ( m_ddcapsDrv.dwCaps&DDCAPS_BLTFOURCC ) {}
		else
		{
			return JDRAW_ERR_MODE;
		}
	}
	DbgStrOut("int CJDDraw::InitBackSurface %d,%d,%d,0x%x...OK\r\n",iSrcWidth,iSrcHeight,bUseYUV,m_ddcapsDrv.dwCaps);
	// 如果已经初始化了,就先释放
	DestoryBackSurface();
	
	// MEDIASUBTYPE_YV12

	// 创建后页 begin
	if ( m_mutexFrameBuf.Lock() ) bLock=true;
	else bLock = false;
	ZeroMemory(&ddsd, sizeof(ddsd));
	ddsd.dwSize			= sizeof(ddsd);
	ddsd.ddsCaps.dwCaps	= DDSCAPS_OFFSCREENPLAIN;
	ddsd.dwFlags		= DDSD_CAPS | DDSD_HEIGHT | DDSD_WIDTH | DDSD_PIXELFORMAT;
//	ddsd.dwFlags		= DDSD_CAPS | DDSD_HEIGHT | DDSD_WIDTH;
	ddsd.dwWidth		= iSrcWidth;
	ddsd.dwHeight		= iSrcHeight;
	if ( bUseYUV )
	{
		ddsd.ddpfPixelFormat.dwSize			= sizeof(DDPIXELFORMAT);
		ddsd.ddpfPixelFormat.dwFlags		= DDPF_FOURCC | DDPF_YUV ;
		ddsd.ddpfPixelFormat.dwFourCC		= MAKEFOURCC('Y','V', '1', '2');
		ddsd.ddpfPixelFormat.dwYUVBitCount	= 8;
	}
	else
	{
		ddsd.ddpfPixelFormat.dwSize			= sizeof(DDPIXELFORMAT);
		ddsd.ddpfPixelFormat.dwFlags		= DDPF_RGB;
		if ( m_iRGBMode==JDRAW_RGB_32 )
		{
			// RGB32
			ddsd.ddpfPixelFormat.dwRGBBitCount	= (DWORD)32;
			ddsd.ddpfPixelFormat.dwRBitMask		= 0x00FF0000;
			ddsd.ddpfPixelFormat.dwGBitMask		= 0x0000FF00;
			ddsd.ddpfPixelFormat.dwBBitMask		= 0x000000FF;
		}
		else if ( m_iRGBMode==JDRAW_RGB_24)
		{
			// RGB24
			ddsd.ddpfPixelFormat.dwRGBBitCount	= (DWORD)24;
			ddsd.ddpfPixelFormat.dwRBitMask		= 0xFF0000;
			ddsd.ddpfPixelFormat.dwGBitMask		= 0x00FF00;
			ddsd.ddpfPixelFormat.dwBBitMask		= 0x0000FF;
		}
		else if ( m_iRGBMode==JDRAW_RGB_565 )
		{
			// RGB565
			ddsd.ddpfPixelFormat.dwRGBBitCount	= 16;
			ddsd.ddpfPixelFormat.dwRBitMask		= 0xF800;
			ddsd.ddpfPixelFormat.dwGBitMask		= 0x07e0;
			ddsd.ddpfPixelFormat.dwBBitMask		= 0x001F;
		}
		else return JDRAW_ERR_MODE;
	}
	hRet = m_pDD->CreateSurface(&ddsd, &m_pDDSBack, NULL);
	if ( hRet != DD_OK)
	{
		ddsd.ddsCaps.dwCaps	= DDSCAPS_OFFSCREENPLAIN|DDSCAPS_SYSTEMMEMORY;
		ddsd.dwFlags		= DDSD_CAPS | DDSD_HEIGHT | DDSD_WIDTH;
		hRet = m_pDD->CreateSurface(&ddsd, &m_pDDSBack, NULL);
		if ( hRet != DD_OK)
		{
			if ( bLock ) m_mutexFrameBuf.Unlock();
			DbgStrOut("CJDDraw::InitBackSurface CreateSurface error:%s.",GetErr(hRet,szBuf));
			// MessageBox("Error Create Off Surface.", s);
			return JDRAW_ERR_CREATE;
		}
	}
	m_bUseYUV = bUseYUV;
	if ( m_bUseYUV )
	{
		// 创建OSD页面 begin
		ZeroMemory(&ddsd, sizeof(ddsd));
		ddsd.dwSize			= sizeof(ddsd);
	//	ddsd.ddsCaps.dwCaps2=DDSCAPS2_HINTDYNAMIC;
		ddsd.ddsCaps.dwCaps	= DDSCAPS_OFFSCREENPLAIN|DDSCAPS_3DDEVICE;
		ddsd.dwFlags		= DDSD_CAPS | DDSD_HEIGHT | DDSD_WIDTH | DDSD_PIXELFORMAT;
	//	ddsd.dwFlags		= DDSD_CAPS | DDSD_HEIGHT | DDSD_WIDTH;
	//	ddsd.dwWidth		= 200;
	//	ddsd.dwHeight		= 200;
		ddsd.dwWidth		= iSrcWidth;
		ddsd.dwHeight		= iSrcHeight;
 		ddsd.ddpfPixelFormat.dwSize			= sizeof(DDPIXELFORMAT);
		ddsd.ddpfPixelFormat.dwFlags		= DDPF_RGB;
		if ( m_iRGBMode==JDRAW_RGB_32 )
		{
			// RGB32
			ddsd.ddpfPixelFormat.dwRGBBitCount	= (DWORD)32;
			ddsd.ddpfPixelFormat.dwRBitMask		= 0x00FF0000;
			ddsd.ddpfPixelFormat.dwGBitMask		= 0x0000FF00;
			ddsd.ddpfPixelFormat.dwBBitMask		= 0x000000FF;
		}
		else if ( m_iRGBMode==JDRAW_RGB_24)
		{
			// RGB24
			ddsd.ddpfPixelFormat.dwRGBBitCount	= (DWORD)24;
			ddsd.ddpfPixelFormat.dwRBitMask		= 0xFF0000;
			ddsd.ddpfPixelFormat.dwGBitMask		= 0x00FF00;
			ddsd.ddpfPixelFormat.dwBBitMask		= 0x0000FF;
		}
		else if ( m_iRGBMode==JDRAW_RGB_565 )
		{
			// RGB565
			ddsd.ddpfPixelFormat.dwRGBBitCount	= 16;
			ddsd.ddpfPixelFormat.dwRBitMask		= 0xF800;
			ddsd.ddpfPixelFormat.dwGBitMask		= 0x07e0;
			ddsd.ddpfPixelFormat.dwBBitMask		= 0x001F;
		}
		hRet = m_pDD->CreateSurface(&ddsd, &m_pDDSOSD, NULL);
		if ( hRet != DD_OK)
		{
			DbgStrOut("CJDDraw::InitBackSurface CreateOSDSurface error:%s.",GetErr(hRet,szBuf));
		}
		// 创建OSD页面 begin
	}
	if ( bLock ) m_mutexFrameBuf.Unlock();
	// 创建后页 end
	
	// 记录下当前值
	m_iWidth		= iSrcWidth;			// 视频宽
	m_iHeight		= iSrcHeight;			// 视频高
	// 背屏大小
	if ( bUseYUV )
	{
		m_lBackSize		= (m_iWidth*m_iHeight*3)/2;
	}
	else
	{
		m_lBackSize		= m_iWidth*m_iHeight*m_iRGBDeep;
	}
	
	// 初始化帧缓存
	m_tDevTime		= 0;			// 设备时间
	m_lHasRead		= 0;			// 读出数
	m_lHasWrite		= 0;			// 写入数
//	m_jgrt.Start();

	return JDRAW_ERR_SUCC;
	JLOG_CATCH("try-catch CJDDraw::InitBackSurface \r\n")
	return JDRAW_ERR_CREATE;
}

// 获取使用的DirectDraw对象
LPDIRECTDRAW CJDDraw::GetDirectDraw()
{
	return m_pDD;
}

// 设置显示的回调函数
int CJDDraw::SetDisplayCallBack(fcbDisplay* pFun,PSTDisplayCB pstDisplayCB)
{
	bool	bLock	= false;

	if ( m_mutexFrameBuf.Lock() ) bLock=true;
	else bLock = false;
	m_pcbDisplay	= pFun;
	m_pstDisplayCB	= pstDisplayCB;
	if ( bLock ) m_mutexFrameBuf.Unlock();

	return 0;
}

// 设置显示位置偏移量
int CJDDraw::SetWndPosOffset(LPCRECT lpOffsetRect)
{
	if (m_hDrawWnd==NULL)
	{
		return -1;
	}

	if ( lpOffsetRect )
	{
		m_rtWndOffset.left		= lpOffsetRect->left;
		m_rtWndOffset.right		= lpOffsetRect->right;
		m_rtWndOffset.top		= lpOffsetRect->top;
		m_rtWndOffset.bottom	= lpOffsetRect->bottom;
	}
	else
	{
		memset(&m_rtWndOffset,0,sizeof(RECT));
	}

	return 0;
}

// 设置源位置及拉伸倍数
int	CJDDraw::SetSrcPosOffset(LPCRECT lpOffsetRect)
{
	if ( lpOffsetRect )
	{
		m_rtSrcOffset.left		= lpOffsetRect->left;
		m_rtSrcOffset.right		= lpOffsetRect->right;
		m_rtSrcOffset.top		= lpOffsetRect->top;
		m_rtSrcOffset.bottom	= lpOffsetRect->bottom;
	}
	else
	{
		memset(&m_rtSrcOffset,0,sizeof(RECT));
	}

	return 0;
}

// 销毁后页面缓冲区
int CJDDraw::DestoryBackSurface()
{
	JLOG_TRY
	int				i		= 0;
//	PSTYUVFrameAttr pstYUV	= NULL;
	bool			bLock	= false;
	
	if ( m_mutexFrameBuf.Lock() ) bLock=true;
	else bLock = false;
	SAFE_RELEASE_B(m_pDDSBack);			// 后页面
	SAFE_RELEASE_B(m_pDDSOSD);			// 后页面
	m_lHasRead		= 0;				// 读出数
	m_lHasWrite		= 0;				// 写入数
	if ( bLock ) m_mutexFrameBuf.Unlock();

	m_iWidth		= 0;				// 视频宽
	m_iHeight		= 0;				// 视频高
	m_lBackSize		= 0;				// 背屏大小
	return 0;
	JLOG_CATCH("try-catch CJDDraw::DestoryBackSurface \r\n")
	return 0;
}

// 销毁DD
int CJDDraw::DestoryDD()
{
	bool			bLock	= false;
	JLOG_TRY
	// 已经销毁过了
	if ( m_pDD==NULL ) return 0;
	
	if ( m_mutexFrameBuf.Lock() ) bLock=true;
	else bLock = false;
	DestoryBackSurface();

	SAFE_RELEASE_B(m_pDDClip);			// 裁剪板
	SAFE_RELEASE_B(m_pDDSFront);		// 前页面
	SAFE_RELEASE_B(m_pDD);				// DirectDraw对象
	
	memset(&(m_stDispDev.stGUID),0,sizeof(GUID));				// 显示设备之GUID
	memset(m_stDispDev.szDescription,0,AVPDisDevStrLen);		// 显示设备之描述
	memset(m_stDispDev.szDriveName,0,AVPDisDevStrLen);			// 显示设备之驱动名称
	memset(&(m_stDispDev.rcMonitor),0,sizeof(RECT));			// 对应显示器信息
	memset(&(m_stDispDev.rcWork),0,sizeof(RECT));				// 对应显示器信息
	m_stDispDev.dwFlags	 = 0;									// 对应显示器信息

	if ( bLock ) m_mutexFrameBuf.Unlock();
	m_mutexFrameBuf.CloseMutex();		// 帧缓存锁

	return 0;
	JLOG_CATCH("try-catch CJDDraw::DestoryDD \r\n")
	return 0;
}

// 重置DD
HRESULT CJDDraw::ResetDD()
{
	JLOG_TRY
	int				iWidth		= m_iWidth;		// 视频宽
	int				iHeight		= m_iHeight;	// 视频高
	HWND			hDrawWnd	= m_hDrawWnd;
	RECT			rtWndOffset;				// 显示区域偏移量
	RECT			rtSrcOffset;				// 显示源区域偏移量
	bool			bFullScreen	= m_bFullScreen;

	memcpy(&rtWndOffset,&m_rtWndOffset,sizeof(RECT));		// 显示区域偏移量
	memcpy(&rtSrcOffset,&m_rtSrcOffset,sizeof(RECT));		// 显示源区域偏移量

	DestoryDD();											// 销毁DD
	InitDD(&m_stDispDev,hDrawWnd,bFullScreen,m_iRGBMode);	// 初始化DD
	InitBackSurface(iHeight,iWidth,m_bUseYUV);				// 初始化后页面缓冲区
	
	SetWndPosOffset(&rtWndOffset);				// 设置显示位置偏移量
	SetSrcPosOffset(&rtSrcOffset);				// 设置源位置及拉伸倍数

	return DD_OK;
	JLOG_CATCH("try-catch CJDDraw::ResetDD \r\n")
	return false;
}

int CJDDraw::DrawBack(LPBYTE lpFrame,long lFrameLen)
{
	HRESULT				hRet		= NULL;
	LPDIRECTDRAWSURFACE pDDS		= NULL;
	DDSURFACEDESC		ddsd;
	LPBYTE				lpSurf		= NULL;
	bool				bLock		= false;
	char				szBuf[1024]	= {0};

	try
	{
		if ( m_pDDSBack==NULL ) return -1;

		// 绘制到后表面
		pDDS = m_pDDSBack;
		// 初始化参数 begin
		memset(&ddsd, 0, sizeof(ddsd));
		ddsd.dwSize = sizeof(ddsd);

		if ( m_mutexFrameBuf.Lock() ) bLock=true;
		else bLock = false;
		hRet = pDDS->Lock(NULL,&ddsd,DDLOCK_WAIT | DDLOCK_WRITEONLY,NULL);
		if ( hRet==DDERR_SURFACELOST )
		{
			// 表面丢失,重来.
			hRet = pDDS->Restore();
			if ( DDERR_WRONGMODE == hRet )
			{
				hRet = ResetDD();
				pDDS = m_pDDSBack;
				hRet = pDDS->Lock(NULL,&ddsd,DDLOCK_WAIT | DDLOCK_WRITEONLY,NULL);
			}
		}
		if(hRet != DD_OK)
		{
			if ( bLock ) m_mutexFrameBuf.Unlock();
			DbgStrOut("CJDDraw::DrawBack Lock error %s\r\n",GetErr(hRet,szBuf));
			return -1;
		}
		lpSurf	= (LPBYTE)ddsd.lpSurface;
		// 后表面为空,填充离屏表面
		if(lpSurf)
		{
			if (m_bUseYUV)
			{
				int		i	= 0;
				LPBYTE	lpY	= lpFrame;
				LPBYTE	lpU	= lpY+m_iWidth*m_iHeight;
				LPBYTE	lpV	= lpU+(m_iWidth*m_iHeight)/4;
				for(i = 0; i < (int)ddsd.dwHeight; i++)
				{
					jmemcpy(lpSurf, lpY, ddsd.dwWidth);
					lpY += ddsd.dwWidth;
					lpSurf += ddsd.lPitch;
				}
				for(i = 0; i < (int)ddsd.dwHeight/2; i++)
				{
					jmemcpy(lpSurf, lpV, ddsd.dwWidth / 2);
					lpV += ddsd.dwWidth / 2;
					lpSurf += ddsd.lPitch / 2;
				}
				for(i = 0; i < (int)ddsd.dwHeight/2; i++)
				{
					jmemcpy(lpSurf, lpU, ddsd.dwWidth / 2);
					lpU += ddsd.dwWidth / 2; 
					lpSurf += ddsd.lPitch / 2;
				}
			}
			else
			{
				long	lCount	= 0;
				int		iPitch	= 0;
				int		iCopy	= 0;

				lCount	= min(lFrameLen,m_lBackSize);
				if ( ddsd.lPitch==ddsd.dwWidth*m_iRGBDeep )
				{
					jmemcpy(lpSurf,lpFrame,lCount);
				}
				else
				{
					iPitch = ddsd.lPitch/m_iRGBDeep;
					while(lCount>0)
					{
						iCopy = (int)(ddsd.dwWidth);
						if ( iCopy>lCount ) iCopy = lCount;
						jmemcpy(lpSurf,lpFrame,iCopy);
						lpSurf	+= iPitch;
						lpFrame	+= iCopy;
						lCount	-= iCopy;
					}
				}
			}
		}
		pDDS->Unlock(NULL);
		if ( bLock ) m_mutexFrameBuf.Unlock();
		
		return 0;
	}
	catch(...)
	{
		if ( bLock )
		{
			pDDS->Unlock(NULL);
			m_mutexFrameBuf.Unlock();
		}
		DbgStrOut("try-catch CJDDraw::DrawBack \r\n");
	}
	return 0;
}

// 绘制回调,0表示不绘制,1表示绘制后表面,-1表示绘制OSD表面
LPDIRECTDRAWSURFACE CJDDraw::DrawCallback(LPRECT lpRectDraw,LPRECT lpRectSrc)
{
	HDC		hDC			= NULL;
	HRESULT	hRet		= NULL;
	long	lLine		= __LINE__;
	char	szBuf[1024] = {0};
	LPDIRECTDRAWSURFACE	pDDRet	= NULL;
//	CJGetRunTime		jgrtSpan;

	if ( m_pstDisplayCB==NULL ) return m_pDDSBack;
	if ( m_pDDSBack==NULL && m_pDDSOSD==NULL )
	{
		return NULL;
	}

	try
	{
		pDDRet	= m_pDDSBack;
		if ( m_pstDisplayCB->lpWnd )
		{
			if ( m_bUseYUV )
			{
			//	jgrtSpan.Start();
			//	m_pDDSOSD->Blt(lpRectSrc, pDDS, lpRectSrc, DDBLT_WAIT, NULL);
			//	hRet = m_pDDSOSD->Blt(lpRectSrc, pDDRet, lpRectSrc, DDBLT_WAIT, NULL);
				hRet = m_pDDSOSD->BltFast(lpRectSrc->left, lpRectSrc->top, pDDRet, lpRectSrc, DDBLTFAST_NOCOLORKEY|DDBLTFAST_WAIT);
				if ( hRet==DDERR_SURFACELOST )
				{
					// 表面丢失,重来.
					hRet = m_pDDSOSD->Restore();
					if ( DDERR_WRONGMODE == hRet )
					{
						hRet = ResetDD();
						hRet = m_pDDSOSD->Blt(lpRectSrc, pDDRet, lpRectSrc, DDBLT_WAIT, NULL);
					}
					if ( hRet!=DD_OK )
					{
						DbgStrOut("CJDDraw::DrawCallback0 :CreateSurface:%s",GetErr(hRet,szBuf));
						return NULL;
					}
				}
			//	jgrtSpan.End();
			//	JTRACE("CJDDraw::DrawCallback m_pDDSOSD Span %.2f\r\n",jgrtSpan.GetTimeSpan());
				pDDRet = m_pDDSOSD;
			}
			
		//	jgrtSpan.Start();
			pDDRet->GetDC(&hDC);
		//	jgrtSpan.End();
		//	JTRACE("CJDDraw::DrawCallback GetDC Span 0x%08x,%d,%.2f\r\n",hDC,pDDRet->IsLost(),jgrtSpan.GetTimeSpan());
			if ( hDC )
			{
				lLine		= __LINE__;
				m_pstDisplayCB->hDC			= hDC;
				lLine		= __LINE__;
				memcpy(&(m_pstDisplayCB->rtSrc),lpRectSrc,sizeof(RECT));
				memcpy(&(m_pstDisplayCB->rtTag),lpRectDraw,sizeof(RECT));
				m_pstDisplayCB->iScaleType	= m_eScaleType;
				m_pstDisplayCB->tDevTime	= m_tDevTime;
				if ( m_pcbDisplay(m_pstDisplayCB)!=0 )
				{
					pDDRet->ReleaseDC(hDC);
					return NULL;
				}
				pDDRet->ReleaseDC(hDC);
			}
			lLine		= __LINE__;
		}
		return pDDRet;
	}
	catch(...)
	{
		DbgStrOut("try-catch CJDDraw::DrawCallback0 at:%ld.\r\n",lLine);
	}

	return NULL;
}

// 绘制回调(完成后)
void CJDDraw::DrawCallback()
{
	if ( m_pstDisplayCB==NULL )
	{
		return;
	}

	try
	{
		if ( m_pstDisplayCB->lpWnd )
		{
			m_pstDisplayCB->hDC			= NULL;
			m_pcbDisplay(m_pstDisplayCB);
		}
	}
	catch(...)
	{
		DbgStrOut("try-catch CJDDraw::DrawCallback1.\r\n");
	}
}

// 填充表面
int CJDDraw::FillSurfaceColor(LPDIRECTDRAWSURFACE lpdds,LPRECT lpRectWnd,LPRECT lpRectDraw,int iClr) 
{ 
	JLOG_TRY
	DDBLTFX ddbltfx;	// this contains the DDBLTFX structure
	RECT	rtFill;

	// 设置填充颜色 begin
	ZeroMemory(&ddbltfx, sizeof(ddbltfx)); 
	ddbltfx.dwSize=sizeof(ddbltfx);
	ddbltfx.dwFillColor = iClr;
	// 设置填充颜色 end

	// 填充表面
	// 左右
	rtFill.top		= lpRectWnd->top;
	rtFill.bottom	= lpRectWnd->bottom;
	// 左
	rtFill.left		= lpRectWnd->left;
	rtFill.right	= lpRectDraw->left;
	if ( rtFill.right-rtFill.left>0 ) lpdds->Blt(&rtFill, NULL, NULL, DDBLT_COLORFILL, &ddbltfx);
	// 右
	rtFill.left		= lpRectDraw->right;
	rtFill.right	= lpRectWnd->right;
	if ( rtFill.right-rtFill.left>0 ) lpdds->Blt(&rtFill, NULL, NULL, DDBLT_COLORFILL, &ddbltfx);
	// 上下
	rtFill.left		= lpRectDraw->left;
	rtFill.right	= lpRectDraw->right;
	// 上
	rtFill.top		= lpRectWnd->top;
	rtFill.bottom	= lpRectDraw->top;
	if ( rtFill.bottom-rtFill.top>0 ) lpdds->Blt(&rtFill, NULL, NULL, DDBLT_COLORFILL, &ddbltfx);
	// 下
	rtFill.top		= lpRectDraw->bottom;
	rtFill.bottom	= lpRectWnd->bottom;
	if ( rtFill.bottom-rtFill.top>0 ) lpdds->Blt(&rtFill, NULL, NULL, DDBLT_COLORFILL, &ddbltfx);

	// return success 
	return(1); 
	JLOG_CATCH("try-catch CJDDraw::FillSurfaceColor \r\n")
	return 0;
}

bool CJDDraw::DrawToFront(LPRECT lpRectWnd, LPRECT lpRectDraw, LPRECT lpRectSrc, LPDIRECTDRAWSURFACE lpBackDDS)
{
	HRESULT	hRet		= NULL;			// 用于返回
	char	szBuf[1024]	= {0};

	if ( m_pDDSFront==NULL || lpBackDDS==NULL ) return false;

	if ( m_bNoTearing )
	{
		m_pDD->WaitForVerticalBlank(DDWAITVB_BLOCKBEGIN ,NULL);
	}

	FillSurfaceColor(m_pDDSFront,lpRectWnd,lpRectDraw,JDRAW_CLR_BK);
	hRet = m_pDDSFront->Blt(lpRectDraw, lpBackDDS, lpRectSrc, DDBLT_WAIT, NULL);
//	hRet = m_pDDSFront->BltFast(lpRectDraw->left, lpRectDraw->top, lpBackDDS, lpRectSrc, DDBLTFAST_NOCOLORKEY|DDBLTFAST_WAIT);
 	
	if ( hRet != DD_OK )
 	{
		if ( DDERR_INVALIDRECT==hRet || DDERR_EXCEPTION==hRet )
		{
			DbgStrOut("CJDDraw::DrawToFront: Err:%s\r\n",GetErr(hRet,szBuf));
		}
		else
		{
			DbgStrOut("CJDDraw::DrawToFront: Blt:%s\r\n",GetErr(hRet,szBuf));
			// 表面丢失,重来.
			hRet = m_pDDSFront->Restore();
			if ( DD_OK != hRet )
			{
				DbgStrOut("CJDDraw::DrawToFront: Restore:%s\r\n",GetErr(hRet,szBuf));
				hRet = ResetDD();
				return false;
			}
			hRet = m_pDDSFront->Blt(lpRectDraw, lpBackDDS, lpRectSrc, DDBLT_WAIT, NULL);
		//	hRet = m_pDDSFront->BltFast(lpRectDraw->left, lpRectDraw->top, lpBackDDS, lpRectSrc, DDBLTFAST_NOCOLORKEY|DDBLTFAST_WAIT);
		}
 	}

	return true;
}

int CJDDraw::DrawFrame(LPBYTE lpFrame,long lFrameLen,time_t tDevTime)
{
	RECT	rtSrc;
	RECT	rtDraw;
	RECT	rtWnd;
	bool	bLock		= false;

	try
	{
		if ( m_mutexFrameBuf.Lock() ) bLock=true;
		else bLock = false;

		if (m_pDD==NULL||lpFrame==NULL)
		{
			if ( bLock ) m_mutexFrameBuf.Unlock();
			return JDRAW_ERR_DRAW;
		}

		// 写背屏
		if ( DrawBack(lpFrame,lFrameLen)!=0 )
		{
			if ( bLock ) m_mutexFrameBuf.Unlock();
			return JDRAW_ERR_DRAW;
		}

		m_tDevTime = tDevTime;
		if ( GetDrawRect(&rtDraw,&rtSrc,&rtWnd) && rtDraw.right>rtDraw.left && rtDraw.bottom>rtDraw.top )
		{
			// 刷屏 begin
		//	if ( m_iID==1 )
		//	{
		//		JTRACE("CJDDraw::GetDrawRect %d,%d,%d,%d\r\n",rtWnd.left,rtWnd.top,rtWnd.right,rtWnd.bottom);
		//	}
			DrawToFront(&rtWnd,&rtDraw,&rtSrc,DrawCallback(&rtDraw,&rtSrc));
		//	DrawToFront(&rtWnd,&rtDraw,&rtSrc,m_pDDSBack);
		//	DrawCallback();
		}
		if ( bLock ) m_mutexFrameBuf.Unlock();
		// 刷屏 end

		return JDRAW_ERR_SUCC;
	}
	catch (...)
	{
		DbgStrOut("try-catch CJDDraw::DrawFrame.\r\n");
	}
	return -1;
}

// 重绘(用于画面维护)
int CJDDraw::Redraw()
{
	JLOG_TRY
	RECT	rtSrc;
	RECT	rtDraw;
	RECT	rtWnd;
	HRESULT	hRet		= NULL;			// 用于返回
	bool	bLock		= false;

	if ( m_pDDSFront && m_pDDSBack )
	{
		if ( GetDrawRect(&rtDraw,&rtSrc,&rtWnd) && rtDraw.right>rtDraw.left && rtDraw.bottom>rtDraw.top )
		{
			// 刷屏 begin
			if ( m_mutexFrameBuf.Lock() ) bLock=true;
			else bLock = false;
			DrawToFront(&rtWnd,&rtDraw,&rtSrc,DrawCallback(&rtDraw,&rtSrc));
			if ( bLock ) m_mutexFrameBuf.Unlock();
			// 刷屏 end
		}
	}
	return 0;
	JLOG_CATCH("try-catch CJDDraw::Redraw \r\n")
	return -1;
}

bool CJDDraw::GetDrawRect(LPRECT lpRectDraw,LPRECT lpRectSrc,LPRECT lpRectWnd)
{
	JLOG_TRY
	float	fScale		= 0;
	int		iTemp		= 0;
	int		iWidth		= 0;
	int		iHeight		= 0;
	int		iSrcWidth	= 0;
	int		iSrcHeight	= 0;

	// 窗口位置
	if ( m_bFullScreen )
	{
	//	iWidth	= -(m_stDispDev.rcWork.left);
	//	iHeight	= -(m_stDispDev.rcWork.top);

 		lpRectWnd->left		= 0+m_rtWndOffset.left;
 		lpRectWnd->right	= m_stDispDev.rcMonitor.right-m_stDispDev.rcMonitor.left-m_rtWndOffset.right;
 		lpRectWnd->top		= 0+m_rtWndOffset.top;
 		lpRectWnd->bottom	= m_stDispDev.rcMonitor.bottom-m_stDispDev.rcMonitor.top-m_rtWndOffset.bottom;
	}
	else
	{
	//	if ( m_hDrawWnd ) GetWindowRect(m_hDrawWnd,lpRectWnd);
		lpRectWnd->left		= 0;
		lpRectWnd->right	= 0;
		lpRectWnd->top		= 0;
		lpRectWnd->bottom	= 0;
		if ( m_hDrawWnd==NULL || GetWindowRect(m_hDrawWnd,lpRectWnd)==FALSE )
		{
			return false;
		}
	 	lpRectWnd->left		= lpRectWnd->left+m_rtWndOffset.left;
	 	lpRectWnd->right	= lpRectWnd->right-m_rtWndOffset.right;
	 	lpRectWnd->top		= lpRectWnd->top+m_rtWndOffset.top;
	 	lpRectWnd->bottom	= lpRectWnd->bottom-m_rtWndOffset.bottom;
	}
	
// 	if ( m_hDrawWnd ) GetWindowRect(m_hDrawWnd,lpRectWnd);
// 	lpRectWnd->left		= lpRectWnd->left+iWidth+m_rtWndOffset.left;
// 	lpRectWnd->right	= lpRectWnd->right+iWidth-m_rtWndOffset.right;
// 	lpRectWnd->top		= lpRectWnd->top+iHeight+m_rtWndOffset.top;
// 	lpRectWnd->bottom	= lpRectWnd->bottom+iHeight-m_rtWndOffset.bottom;


	// 源位置
	lpRectSrc->right	= m_iWidth;
	lpRectSrc->bottom	= m_iHeight;
	lpRectSrc->left		= 0+m_rtSrcOffset.left;
	lpRectSrc->right	= lpRectSrc->right-m_rtSrcOffset.right;
	lpRectSrc->top		= 0+m_rtSrcOffset.top;
	lpRectSrc->bottom	= lpRectSrc->bottom-m_rtSrcOffset.bottom;
	iSrcWidth			= lpRectSrc->right-lpRectSrc->left;
	iSrcHeight			= lpRectSrc->bottom-lpRectSrc->top;

	if ( m_eScaleType==AVPScaleNone )
	{
		// 不放缩
		iWidth	= lpRectWnd->right-lpRectWnd->left;
		iHeight	= lpRectWnd->bottom-lpRectWnd->top;

		// 宽
		if ( iWidth>iSrcWidth )
		{
			lpRectDraw->left	= lpRectWnd->left+(iWidth-iSrcWidth)/2;
			lpRectDraw->right	= lpRectDraw->left+iSrcWidth;
		}
		else
		{
			lpRectDraw->left	= lpRectWnd->left;
			lpRectDraw->right	= lpRectWnd->right;
			lpRectSrc->right	= lpRectSrc->left+iWidth;
		}
	
		// 高
		if ( iHeight>iSrcHeight )
		{
			lpRectDraw->top		= lpRectWnd->top+(iHeight-iSrcHeight)/2;
			lpRectDraw->bottom	= lpRectDraw->top+iSrcHeight;
		}
		else
		{
			lpRectDraw->top		= lpRectWnd->top;
			lpRectDraw->bottom	= lpRectWnd->bottom;
			lpRectSrc->bottom	= lpRectSrc->top+iHeight;
		}
	}
	else if ( m_eScaleType==AVPScaleFull )
	{
		// 全屏放缩
		lpRectDraw->left	= lpRectWnd->left;
		lpRectDraw->right	= lpRectWnd->right;
		lpRectDraw->top		= lpRectWnd->top;
		lpRectDraw->bottom	= lpRectWnd->bottom;
	}
	else if ( m_eScaleType==AVPScaleKeep )
	{
		// 保持原比例
		fScale	= (float)(iSrcWidth)/(float)(iSrcHeight);
	}
	else if ( m_eScaleType==AVPScale4_3 )
	{
		// 4:3放缩
		fScale	= (float)(4.0f)/(float)(3.0f);
	}
	else if ( m_eScaleType==AVPScale16_9 )
	{
		// 16:9放缩
		fScale	= (float)(16.0f)/(float)(9.0f);
	}

	if ( fScale>0.0f )
	{
		iWidth	= lpRectWnd->right-lpRectWnd->left;
		iHeight	= lpRectWnd->bottom-lpRectWnd->top;

		if ( float(iWidth)/float(iHeight)>fScale )
		{
			// 显示屏过宽,以长为基准
			iTemp				= (int)(float(iHeight)*fScale);
			lpRectDraw->left	= lpRectWnd->left+(iWidth-iTemp)/2;
			lpRectDraw->right	= lpRectDraw->left+iTemp;
			lpRectDraw->top		= lpRectWnd->top;
			lpRectDraw->bottom	= lpRectWnd->bottom;
		}
		else
		{
			// 显示屏过长,以宽为基准
			iTemp				= (int)(float(iWidth)/fScale);
			lpRectDraw->top		= lpRectWnd->top+(iHeight-iTemp)/2;
			lpRectDraw->bottom	= lpRectDraw->top+iTemp;
			lpRectDraw->left	= lpRectWnd->left;
			lpRectDraw->right	= lpRectWnd->right;
		}
	}

	if (RECT_WIDTH(lpRectSrc) > m_iWidth)
	{
		lpRectSrc->right = lpRectSrc->left+iSrcWidth;
	}
	if (RECT_HEIGHT(lpRectSrc) > m_iHeight)
	{
		lpRectSrc->bottom = lpRectSrc->top+iSrcHeight;
	}
	return true;
	JLOG_CATCH("try-catch CJDDraw::GetDrawRect \r\n")
	return false;
}

// 设置放伸方式
int CJDDraw::SetScaleType(int iScaleType)
{
	JLOG_TRY
	int		iRet	= m_eScaleType;
	bool	bLock	= false;

	if ( m_mutexFrameBuf.Lock() ) bLock = true;
	else bLock = false;
	/*
	if ( m_ddcapsDrv.dwCaps&DDCAPS_BLTSTRETCH )
	{
	}
	else
	{
		// 不支持缩放
		DbgStrOut("CJDDraw::SetScaleType unspport blt stretch\r\n");
		iScaleType = AVPScaleNone;
	}
	*/
	switch(iScaleType)
	{
	case AVPScaleNone:	m_eScaleType = AVPScaleNone;	break;	// 不放缩
	case AVPScaleFull:	m_eScaleType = AVPScaleFull;	break;	// 全屏放缩
	case AVPScaleKeep:	m_eScaleType = AVPScaleKeep;	break;	// 自适应
	case AVPScale4_3:	m_eScaleType = AVPScale4_3;		break;	// 4:3放缩
	case AVPScale16_9:	m_eScaleType = AVPScale16_9;	break;	// 16:9放缩
	default:			m_eScaleType = AVPScaleFull;	break;	// 全屏放缩
	}
	Redraw();
	if ( bLock ) m_mutexFrameBuf.Unlock();

	return iRet;
	JLOG_CATCH("try-catch CJDDraw::SetScaleType \r\n")
	return -1;
}

// 获取放伸方式
int CJDDraw::GetScaleType()
{
	return m_eScaleType;
}

// 抗裂痕
bool CJDDraw::SetNoTearing(bool bEnable)
{
	// 只在WINDOWS版本低于5时可设置
	if (m_dwSystemVer<=5) m_bNoTearing=bEnable;
	else m_bNoTearing = false;
	
	return m_bNoTearing;
}

// 获取当前抗裂痕设置
bool CJDDraw::IsNoTearing()
{
	return m_bNoTearing;
}

// 获取宽度
int CJDDraw::GetWidth()
{
	return m_iWidth;
}

// 获取高度
int CJDDraw::GetHeight()
{
	return m_iHeight;
}