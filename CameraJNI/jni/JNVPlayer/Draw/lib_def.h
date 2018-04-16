
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
	// CJLThreadCtrl ����صĺ궨�� begin
	#include <windows.h>
	#define	pthread_t	DWORD
	#define usleep(t)	Sleep(t)
	#define CUS_T_RET	DWORD WINAPI
	#define	pthread_create(t_id,attr,fun,arg) \
			CreateThread(attr,0,fun,arg,0,t_id)
	// CJLThreadCtrl ����صĺ궨�� end
#else
	// CJLThreadCtrl ����صĺ궨�� begin
	#include <pthread.h>
	#include <unistd.h>
	#define CUS_T_RET	void*
	// CJLThreadCtrl ����صĺ궨�� end
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


#define	AVPDisDevStrLen			124				// ��ʾ�豸���Ƴ���
// ��ʾ�豸���� begin
typedef struct stDisplayDev
{
	GUID			stGUID;								// ��ʾ�豸֮GUID
	char			szDescription[AVPDisDevStrLen];		// ��ʾ�豸֮����
	char			szDriveName[AVPDisDevStrLen];		// ��ʾ�豸֮��������
	RECT			rcMonitor;							// ��ʾ��������
	RECT			rcWork;								// ��ʾ���Ĺ�������
	DWORD			dwFlags;							// ��ʾ���ı�־λ,MONITORINFOF_PRIMARY ��ʾΪ����ʾ��
	DWORD			dwData;								// ˽��������
	stDisplayDev()
	{
		memset(&stGUID,0,sizeof(GUID));					// ��ʾ�豸֮GUID
		memset(szDescription,0,AVPDisDevStrLen);		// ��ʾ�豸֮����
		memset(szDriveName,0,AVPDisDevStrLen);			// ��ʾ�豸֮��������
		memset(&rcMonitor,0,sizeof(RECT));				// ��Ӧ��ʾ����Ϣ
		memset(&rcWork,0,sizeof(RECT));					// ��Ӧ��ʾ����Ϣ
		dwFlags		= NULL;								// ��Ӧ��ʾ����Ϣ
		dwData		= NULL;								// ˽��������
	}
}* PSTDisplayDev;
// ��ʾ�豸���� end

// ��ʾ�ص��������ò��� begin
typedef struct stDisplayCB
{
	long		lpWnd;					// CWnd*
	void*		hDC;					// HDC
	RECT		rtTag;					// Ŀ������
	RECT		rtSrc;					// Դ����
	int			iScaleType;				// ��������
	time_t		tDevTime;				// �豸ʱ��
}* PSTDisplayCB;
// ��ʾ�ص��������ò��� end

// ��ʾ�ص�����,0��ʾ��������,1��ʾ���ܼ�������
typedef int(CALLBACK fcbDisplay)(PSTDisplayCB pstDisplayCB);

// ����������� begin
typedef enum
{
	AVPScaleNone		= 0x0,			// ������
	AVPScaleFull		= 0x1,			// ȫ������
	AVPScaleKeep		= 0x2,			// ����ԭ����
	AVPScale4_3			= 0x3,			// 4:3����
	AVPScale16_9		= 0x4,			// 16:9����
}AVPScaleType;
// ����������� end

#endif	// __LIB_COMMON__
