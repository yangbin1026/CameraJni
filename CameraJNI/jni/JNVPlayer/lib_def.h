
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

#endif	// __LIB_COMMON__
