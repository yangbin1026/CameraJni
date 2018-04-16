#ifndef	__JLOG_WRITER__ 
#define	__JLOG_WRITER__

#define JLOG_MAX_EXT_LEN	10
#define JLOG_DEF_FILE_LEN	1024*1024*5
#ifdef __ANDROID__
#include <android/log.h>
#define	LOG_TAG		"JLogWriter.h"
#define LOGD(...)	__android_log_print(ANDROID_LOG_DEBUG,LOG_TAG,__VA_ARGS__)
#define LOGI(...)	__android_log_print(ANDROID_LOG_INFO,LOG_TAG,__VA_ARGS__)
#define LOGW(...)	__android_log_print(ANDROID_LOG_WARN,LOG_TAG,__VA_ARGS__)
#define LOGE(...)	__android_log_print(ANDROID_LOG_ERROR,LOG_TAG,__VA_ARGS__)
#define LOGF(...)	__android_log_print(ANDROID_LOG_FATAL,LOG_TAG,__VA_ARGS__)
#define	JTRACE(...)	__android_log_print(ANDROID_LOG_DEBUG,LOG_TAG,__VA_ARGS__)
#define	DbgStrOut			JTRACE
#else
#define	DbgStrOut			CJLogWriter::s_jlog.WriteLog
#define	JTRACE				CJLogWriter::JTrace
#endif
#define JLOG_TRY			try{
#define JLOG_CATCH(str)		}catch (...){DbgStrOut(str);}

#include "stdio.h"
#include "stdlib.h"
#include "j_vs_ver.h"

namespace JSocketCtrl
{
class CJLogWriter
{
public:
	CJLogWriter();
	~CJLogWriter();

	bool Init(LPCTSTR lpszDir,LPCTSTR lpszName,LPCTSTR lpszExt,long lMaxSize);
	bool Close();
	time_t	GetCurTime();
	const char* GetLogFilePath();
	void WriteLog(LPCTSTR fmt,... );
	
	static void JTrace(LPCTSTR fmt,... );
	static CJLogWriter	s_jlog;
	static int s_iCatchNO;
protected:
	bool OpenFile(LPCTSTR lpszPath);	// ���ļ�

	FILE*		m_pfLog;				// ��־�ļ�ָ��
	CMutexLock	m_mutexLog;				// д����
	DWORD		m_dwStartTm;			// ��ʼʱ��(���ڻ�ȡ��ǰʱ��)
	DWORD		m_dwStartTickCount;		// ��ʼʱ��TickCount(���ڻ�ȡ��ǰʱ��)
	long		m_lMaxSize;				// ����С
	char		m_szDir[MAX_PATH];		// ����Ŀ¼
	char		m_szName[MAX_PATH];		// ��־����
	char		m_szExt[JLOG_MAX_EXT_LEN];			// ��׺����
	char		m_szPath[MAX_PATH];		// ��־·��
};
}
using namespace JSocketCtrl;


#endif	// __JLOG_WRITER__
