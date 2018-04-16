#ifndef _JNI_UTILS_H_
#define _JNI_UTILS_H_

#include <stdlib.h>
#include <jni.h>

#include "j_vs_ver.h"
#include "JLogWriter.h"
#include "JJson.h"
#include "JNVPlayerMng.h"

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
// 注册播放器
int register_JNVPlayer(JNIEnv *env);
// 异常处理
int jniThrowException(JNIEnv* env, const char* className, const char* msg);
// 获取JNI的环境
JNIEnv* getJNIEnv();
// 注册JNI的方法
int jniRegisterNativeMethods(JNIEnv* env,
                             const char* className,
                             const JNINativeMethod* gMethods,
                             int numMethods);

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
	JNIEnv*		m_pEnv;				// JAVA环境指针
	jobject     m_javaObject;		// JAVA对象
	jmethodID	m_fcbFun;			// 回调函数
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
	static PSTCBParam AddNewCB(int iType,long lCallbackID,JNIEnv* env,jobject thiz,LPCTSTR lpszFun,long lUserParam);
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

// 字符串转换函数 begin
class CJNVString
{
public:
	CJNVString(JNIEnv *env, jstring* lpValue);
	~CJNVString();
	operator LPCTSTR() const;
	LPCTSTR	GetString();
protected:
	JNIEnv*		m_env;
	jstring* 	m_lpValue;
	LPCTSTR 	m_lpszValue;
};
// 字符串转换函数 end

// CPU使用情况 begin
typedef class CCPUState
{
public:
	int		m_iCpuID;				// CPU索引号
	long	m_lUser;				// 从系统启动开始累计到当前时刻,用户态的CPU时间(单位:jiffies)
	long	m_lNice;				// 从系统启动开始累计到当前时刻,nice值为负的进程所占用的CPU时间(单位:jiffies)
	long	m_lSystem;				// 从系统启动开始累计到当前时刻,核心时间(单位:jiffies)
	long	m_lIdle;				// 从系统启动开始累计到当前时刻,除硬盘IO等待时间以外其它等待时间(单位:jiffies)
	long	m_lIOWait;				// 从系统启动开始累计到当前时刻,硬盘IO等待时间(单位:jiffies)
	long	m_lIrq;					// 从系统启动开始累计到当前时刻,硬中断时间(单位:jiffies)
	long	m_lSoftIrq;				// 从系统启动开始累计到当前时刻,软中断时间(单位:jiffies)
public:
	CCPUState();
	const CCPUState& operator =( const CCPUState& stSrc );
	bool UpdateState(LPCTSTR lpszRead);
	float GetCpuTime();
	float GetUsage(CCPUState* pstStateOld);
}* LPCPUState;
#define	MAX_CPU_CORE	16
typedef class CCPUInfo
{
public:
	CCPUInfo();
protected:
	int			m_iCPUCount;
	CCPUState	m_stState[MAX_CPU_CORE];
	CCPUState	m_stStateOld[MAX_CPU_CORE];
	float		m_fUsage[MAX_CPU_CORE];
public:
	int		GetCpuCount();
	bool	UpdateState();
	float	GetUsage(int iID);
	static CCPUInfo	s_cpuInfo;
}* LPCPUInfo;
// CPU使用情况 end
#endif /* _JNI_UTILS_H_ */


