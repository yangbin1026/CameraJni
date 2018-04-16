#ifndef _JNI_UTILS_H_
#define _JNI_UTILS_H_

#include <stdlib.h>
#include <jni.h>

#include "j_vs_ver.h"
#include "JLogWriter.h"
#include "JJson.h"
#include "JNVPlayerMng.h"

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
// ע�Ქ����
int register_JNVPlayer(JNIEnv *env);
// �쳣����
int jniThrowException(JNIEnv* env, const char* className, const char* msg);
// ��ȡJNI�Ļ���
JNIEnv* getJNIEnv();
// ע��JNI�ķ���
int jniRegisterNativeMethods(JNIEnv* env,
                             const char* className,
                             const JNINativeMethod* gMethods,
                             int numMethods);

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
	JNIEnv*		m_pEnv;				// JAVA����ָ��
	jobject     m_javaObject;		// JAVA����
	jmethodID	m_fcbFun;			// �ص�����
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
	static PSTCBParam AddNewCB(int iType,long lCallbackID,JNIEnv* env,jobject thiz,LPCTSTR lpszFun,long lUserParam);
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

// �ַ���ת������ begin
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
// �ַ���ת������ end

// CPUʹ����� begin
typedef class CCPUState
{
public:
	int		m_iCpuID;				// CPU������
	long	m_lUser;				// ��ϵͳ������ʼ�ۼƵ���ǰʱ��,�û�̬��CPUʱ��(��λ:jiffies)
	long	m_lNice;				// ��ϵͳ������ʼ�ۼƵ���ǰʱ��,niceֵΪ���Ľ�����ռ�õ�CPUʱ��(��λ:jiffies)
	long	m_lSystem;				// ��ϵͳ������ʼ�ۼƵ���ǰʱ��,����ʱ��(��λ:jiffies)
	long	m_lIdle;				// ��ϵͳ������ʼ�ۼƵ���ǰʱ��,��Ӳ��IO�ȴ�ʱ�����������ȴ�ʱ��(��λ:jiffies)
	long	m_lIOWait;				// ��ϵͳ������ʼ�ۼƵ���ǰʱ��,Ӳ��IO�ȴ�ʱ��(��λ:jiffies)
	long	m_lIrq;					// ��ϵͳ������ʼ�ۼƵ���ǰʱ��,Ӳ�ж�ʱ��(��λ:jiffies)
	long	m_lSoftIrq;				// ��ϵͳ������ʼ�ۼƵ���ǰʱ��,���ж�ʱ��(��λ:jiffies)
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
// CPUʹ����� end
#endif /* _JNI_UTILS_H_ */


