#ifndef _IOS_UTILS_H_
#define _IOS_UTILS_H_

#include <stdlib.h>

#include "j_vs_ver.h"
#include "JLogWriter.h"
#include "JJson.h"
#include "JNVPlayerMng.h"

#if (defined _WIN32) || (defined _WIN64)
	// CJLThreadCtrl ����صĺ궨�� begin
	#include <windows.h>
	#define	pthread_t	DWORD
	#define usleep(t)	Sleep(t)
	#define CUS_T_RET	DWORD WINAPI
	#define	pthread_create(t_id,attr,fun,arg) \
			CreateThread(attr,0,fun,arg,0,t_id)
	#define pthread_join(hHandle, retval) \
			CloseHandle((HANDLE)hHandle)
	// CJLThreadCtrl ����صĺ궨�� end
#else
	// CJLThreadCtrl ����صĺ궨�� begin
	#include <pthread.h>
	#include <unistd.h>
	#define CUS_T_RET	void*
	// CJLThreadCtrl ����صĺ궨�� end
#endif

#define JNV_CB_T_EVENT		0			// ��¼�¼��ص�
#define JNV_CB_T_DEC_NET	1			// ����������ص�
#define JNV_CB_T_DEC_LOCAL	2			// ����������ص�
#define JNV_CB_T_PLAY_INFO	3			// ������Ϣ�ص�

#define	JNV_MAX_STREAM		128
#define	JNV_MAX_MSG			128
#define JNV_EVENT_ID		99999

#ifdef __cplusplus
extern "C"
{
#endif
// ����ת�ַ���
LPCTSTR	NumToStr(long lNum,char* strText);
// ����ת�ַ���
LPCTSTR	FloatToStr(float fNum,char* strText);
// �¼��ص��ں���
int OnConnEvent(long lHandle, eJNetEvent eType,int iDataType,void* pEventData,int iDataLen,void* pUserParam);
// ����ص�����
int OnDecFrameNet(PSTDecFrameCB pstDecFrameCB);
int OnDecFrameLocal(PSTDecFrameCB pstDecFrameCB);
int OnPlayInfo(AVPPlayCBMsg eMsg,int iPlayerID,long lpUserParam,__int64 lParam1,__int64 lParam2,DWORD dwRes);
// ��Ϣ�ɳ�������
fJThRet RunMsgDispath(void* pParam);

#ifdef __cplusplus
}
#endif

// ���ڻص��Ĳ����� begin
struct stCBParam;	typedef stCBParam*	PSTCBParam;
struct stCBParam
{
public:
	int			m_iCBType;			// �ص�����(��ͬ���͵Ļص�,����IDһ��)
	long		m_lCallbackID;		// �ص�ID
	void*		m_fcbFun;			// �ص�����
	long		m_lUserParam;		// �û�ָ���Ĳ���
	long		m_lRes0;			// ����λ
	long		m_lRes1;			// ����λ
	long		m_lRes2;			// ����λ
	long		m_lRes3;			// ����λ
public:
	stCBParam();
	~stCBParam();
	void stZero();
	static stCBParam	s_arCBParam[JNV_MAX_STREAM];	// �ص���������
	static int			s_iCurCBCount;					// �ص���������

	// ���ĳ���ص�����,С��0,��ʾȫ��
	static void Clear(long lCallbackID);
	// ��ӻص�����
	static PSTCBParam AddNewCB(int iType,long lCallbackID,void* fcbFun,long lUserParam);
	// �������������ȡ�ص�����
	static PSTCBParam GetCB(int iType,long lCallbackID);
};
// ���ڻص��Ĳ����� end

// �¼���Ϣ begin
struct stItemMsg;	typedef stItemMsg*	PSTItemMsg;
struct stItemMsg
{
public:
	PSTCBParam			m_pstCBParam;		// �ص�����
	CJJson				m_sjonRet;			// ���صĲ���jjson��ʽ
	char*				m_pszRet;			// ���صĲ���jjson��ʽ
	bool				m_bBusy;			// �Ƿ����ڴ�����

	static CMutexLock	s_mutexItemMsg;		// ��Ŀ��Ϣ��
	static bool			s_bMsgEnable;		// ������Ϣ����
	static pthread_t	s_hThreadID;		// �����߳�ID
	static stItemMsg	s_arItemMsg[JNV_MAX_MSG];		// ��Ŀ��Ϣ����
	static int			s_iItemMsgCount;	// ��ǰ��Ŀ��Ϣ����
	static int			s_iCurReadMsg;		// ��ǰ��ȡ��Ŀ��Ϣ��
	static int			s_iCurWriteMsg;		// ��ǰд����Ŀ��Ϣ��
public:
	stItemMsg();
	~stItemMsg();
	void stZero();
	void Clear();
};
// �¼���Ϣ end

#endif /* _IOS_UTILS_H_ */


