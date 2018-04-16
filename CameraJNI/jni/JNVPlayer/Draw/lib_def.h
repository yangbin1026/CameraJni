
#ifndef		__LIB_COMMON__
#define		__LIB_COMMON__

#ifndef		SAFE_FREE
#define		SAFE_FREE(pMem)				if(pMem){free(pMem);pMem=NULL;}
#endif
#ifndef		SAFE_DELETE
#define		SAFE_DELETE(pMem)			if(pMem){delete pMem;pMem=NULL;}
#endif
#ifndef		SAFE_DELETE_A
#define		SAFE_DELETE_A(pMem)			if(pMem){delete [] pMem;pMem=NULL;}
#endif
#ifndef		SAFE_DELETE_T
#define		SAFE_DELETE_T(pMem,Type)	if(pMem){delete (Type *)pMem;pMem=NULL;}
#endif
#ifndef		SAFE_CLOSE_A
#define		SAFE_CLOSE_A(pMem)			if(pMem){pMem->Close();delete(pMem);pMem=NULL;}
#endif
#ifndef		SAFE_CLOSE_B
#define		SAFE_CLOSE_B(pMem)			if(pMem){pMem->Close();pMem=NULL;}
#endif
#ifndef		SAFE_RELEASE_A
#define		SAFE_RELEASE_A(pMem)		if(pMem){pMem->Release();delete(pMem);pMem=NULL;}
#endif
#ifndef		SAFE_RELEASE_B
#define		SAFE_RELEASE_B(pMem)		if(pMem){pMem->Release();pMem=NULL;}
#endif

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#if (defined _WIN32) || (defined _WIN64)
	// CJLThreadCtrl 类相关的宏定义 begin
	#include <windows.h>
	#define	pthread_t	DWORD
	#define usleep(t)	Sleep(t)
	#define CUS_T_RET	DWORD WINAPI
	#define	pthread_create(t_id,attr,fun,arg) \
			CreateThread(attr,0,fun,arg,0,t_id)
	// CJLThreadCtrl 类相关的宏定义 end
#else
	// CJLThreadCtrl 类相关的宏定义 begin
	#include <pthread.h>
	#include <unistd.h>
	#define CUS_T_RET	void*
	// CJLThreadCtrl 类相关的宏定义 end
#endif


// stl support begin
#if (defined _WIN32) || (defined _WIN64)
	#pragma warning(disable:4100)
	#pragma warning(disable:4189)
	#pragma warning(disable:4541)
	#pragma warning(disable:4786)
#endif
#include "string"
#include "list"
#include "map"
#include <map>
#include <vector>
using namespace std;
// stl support end


#define	AVPDisDevStrLen			124				// 显示设备名称长度
// 显示设备定义 begin
typedef struct stDisplayDev
{
	GUID			stGUID;								// 显示设备之GUID
	char			szDescription[AVPDisDevStrLen];		// 显示设备之描述
	char			szDriveName[AVPDisDevStrLen];		// 显示设备之驱动名称
	RECT			rcMonitor;							// 显示器的区域
	RECT			rcWork;								// 显示器的工作区域
	DWORD			dwFlags;							// 显示器的标志位,MONITORINFOF_PRIMARY 表示为主显示屏
	DWORD			dwData;								// 私有数据域
	stDisplayDev()
	{
		memset(&stGUID,0,sizeof(GUID));					// 显示设备之GUID
		memset(szDescription,0,AVPDisDevStrLen);		// 显示设备之描述
		memset(szDriveName,0,AVPDisDevStrLen);			// 显示设备之驱动名称
		memset(&rcMonitor,0,sizeof(RECT));				// 对应显示器信息
		memset(&rcWork,0,sizeof(RECT));					// 对应显示器信息
		dwFlags		= NULL;								// 对应显示器信息
		dwData		= NULL;								// 私有数据域
	}
}* PSTDisplayDev;
// 显示设备定义 end

// 显示回调函数所用参数 begin
typedef struct stDisplayCB
{
	long		lpWnd;					// CWnd*
	void*		hDC;					// HDC
	RECT		rtTag;					// 目标区域
	RECT		rtSrc;					// 源区域
	int			iScaleType;				// 放缩类型
	time_t		tDevTime;				// 设备时间
}* PSTDisplayCB;
// 显示回调函数所用参数 end

// 显示回调函数,0表示继续处理,1表示不能继续处理
typedef int(CALLBACK fcbDisplay)(PSTDisplayCB pstDisplayCB);

// 定义放缩类型 begin
typedef enum
{
	AVPScaleNone		= 0x0,			// 不放缩
	AVPScaleFull		= 0x1,			// 全屏放缩
	AVPScaleKeep		= 0x2,			// 保持原比例
	AVPScale4_3			= 0x3,			// 4:3放缩
	AVPScale16_9		= 0x4,			// 16:9放缩
}AVPScaleType;
// 定义放缩类型 end

#endif	// __LIB_COMMON__
