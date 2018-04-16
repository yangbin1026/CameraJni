#ifndef _IOS_UTILS_H_
#define _IOS_UTILS_H_

#include <stdlib.h>

#include "j_vs_ver.h"
#include "JLogWriter.h"
#include "JJson.h"
#include "JNVPlayerMng.h"

#if (defined _WIN32) || (defined _WIN64)
	// CJLThreadCtrl 类相关的宏定义 begin
	#include <windows.h>
	#define	pthread_t	DWORD
	#define usleep(t)	Sleep(t)
	#define CUS_T_RET	DWORD WINAPI
	#define	pthread_create(t_id,attr,fun,arg) \
			CreateThread(attr,0,fun,arg,0,t_id)
	#define pthread_join(hHandle, retval) \
			CloseHandle((HANDLE)hHandle)
	// CJLThreadCtrl 类相关的宏定义 end
#else
	// CJLThreadCtrl 类相关的宏定义 begin
	#include <pthread.h>
	#include <unistd.h>
	#define CUS_T_RET	void*
	// CJLThreadCtrl 类相关的宏定义 end
#endif

#define JNV_CB_T_EVENT		0			// 登录事件回调
#define JNV_CB_T_DEC_NET	1			// 网络流解码回调
#define JNV_CB_T_DEC_LOCAL	2			// 本地流解码回调
#define JNV_CB_T_PLAY_INFO	3			// 播放信息回调

#define	JNV_MAX_STREAM		128
#define	JNV_MAX_MSG			128
#define JNV_EVENT_ID		99999

#ifdef __cplusplus
extern "C"
{
#endif
// 数字转字符串
LPCTSTR	NumToStr(long lNum,char* strText);
// 数字转字符串
LPCTSTR	FloatToStr(float fNum,char* strText);
// 事件回调节函数
int OnConnEvent(long lHandle, eJNetEvent eType,int iDataType,void* pEventData,int iDataLen,void* pUserParam);
// 解码回调函数
int OnDecFrameNet(PSTDecFrameCB pstDecFrameCB);
int OnDecFrameLocal(PSTDecFrameCB pstDecFrameCB);
int OnPlayInfo(AVPPlayCBMsg eMsg,int iPlayerID,long lpUserParam,__int64 lParam1,__int64 lParam2,DWORD dwRes);
// 消息派出发函数
fJThRet RunMsgDispath(void* pParam);

#ifdef __cplusplus
}
#endif

// 用于回调的参数集 begin
struct stCBParam;	typedef stCBParam*	PSTCBParam;
struct stCBParam
{
public:
	int			m_iCBType;			// 回调类型(不同类型的回调,可能ID一样)
	long		m_lCallbackID;		// 回调ID
	void*		m_fcbFun;			// 回调函数
	long		m_lUserParam;		// 用户指定的参数
	long		m_lRes0;			// 保留位
	long		m_lRes1;			// 保留位
	long		m_lRes2;			// 保留位
	long		m_lRes3;			// 保留位
public:
	stCBParam();
	~stCBParam();
	void stZero();
	static stCBParam	s_arCBParam[JNV_MAX_STREAM];	// 回调参数数组
	static int			s_iCurCBCount;					// 回调参数个数

	// 清除某个回调参数,小于0,表示全部
	static void Clear(long lCallbackID);
	// 添加回调参数
	static PSTCBParam AddNewCB(int iType,long lCallbackID,void* fcbFun,long lUserParam);
	// 根据流句柄来获取回调参数
	static PSTCBParam GetCB(int iType,long lCallbackID);
};
// 用于回调的参数集 end

// 事件信息 begin
struct stItemMsg;	typedef stItemMsg*	PSTItemMsg;
struct stItemMsg
{
public:
	PSTCBParam			m_pstCBParam;		// 回调参数
	CJJson				m_sjonRet;			// 返回的参数jjson格式
	char*				m_pszRet;			// 返回的参数jjson格式
	bool				m_bBusy;			// 是否正在处理中

	static CMutexLock	s_mutexItemMsg;		// 项目消息锁
	static bool			s_bMsgEnable;		// 启用消息处理
	static pthread_t	s_hThreadID;		// 处理线程ID
	static stItemMsg	s_arItemMsg[JNV_MAX_MSG];		// 项目消息数组
	static int			s_iItemMsgCount;	// 当前项目消息项数
	static int			s_iCurReadMsg;		// 当前读取项目消息项
	static int			s_iCurWriteMsg;		// 当前写入项目消息项
public:
	stItemMsg();
	~stItemMsg();
	void stZero();
	void Clear();
};
// 事件信息 end

#endif /* _IOS_UTILS_H_ */


