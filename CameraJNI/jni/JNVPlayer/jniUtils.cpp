#define TAG "jniUtils"

#include <stdlib.h>
#include <android/log.h>
#include "jniUtils.h"
#undef	LOG_TAG
#define	LOG_TAG		"jniUtils.cpp"

extern JavaVM *g_jvm;

// 异常处理
int jniThrowException(JNIEnv* env, const char* className, const char* msg) 
{
    jclass exceptionClass = env->FindClass(className);
    if (exceptionClass == NULL) 
	{
        JTRACE("Unable to find exception class %s", className);
        return -1;
    }

    if (env->ThrowNew(exceptionClass, msg) != JNI_OK) 
	{
       JTRACE("Failed throwing '%s' '%s'",className, msg);
    }
    return 0;
}

// 获取JNI的环境
JNIEnv* getJNIEnv() 
{
    JNIEnv* env = NULL;
    if (g_jvm->GetEnv((void**) &env, JNI_VERSION_1_4) != JNI_OK) 
	{
    	JTRACE("Failed to obtain JNIEnv");
    	return NULL;
    }
    return env;
}

// 注册JNI的方法
int jniRegisterNativeMethods(JNIEnv* env,
                             const char* className,
                             const JNINativeMethod* gMethods,
                             int numMethods)
{
    jclass clazz;

    JTRACE("Registering %s natives-----!!!\n", className);
    clazz = env->FindClass(className);
    if (clazz == NULL)
	{
        JTRACE("Native registration unable to find class '%s'\n", className);
        return -1;
    }
    if (env->RegisterNatives(clazz, gMethods, numMethods) < 0)
	{
        JTRACE("RegisterNatives failed for '%s'\n", className);
        return -1;
    }
    return 0;
}

// 锟斤拷锟斤拷
jint JNI_OnLoad(JavaVM* vm, void* reserved)
{
    JNIEnv* env = NULL;
    jint result = JNI_ERR;
	g_jvm = vm;
	JTRACE("JNI_OnLoad 0x%x\r\n",g_jvm);

    if (vm->GetEnv((void**) &env, JNI_VERSION_1_4) != JNI_OK)
	{
       JTRACE("GetEnv failed!");
        return result;
    }

    JTRACE("loading . . .");

 	if(register_JNVPlayer(env) != JNI_OK) 
 	{
 		JTRACE("can't load register_JNVPlayer");
 		goto end;
 	}
    JTRACE("loaded jni/jni/onLoad.cpp :JNI_OnLoad() go to end");

    result = JNI_VERSION_1_4;

end:
    return result;
}

// 用于回调的参数集 begin
stCBParam	stCBParam::s_arCBParam[JNV_MAX_STREAM];
int			stCBParam::s_iCurCBCount	= 0;			// 回调参数个数
stCBParam::stCBParam()
{
	stZero();
}
void stCBParam::stZero()
{
	m_iCBType		= JNV_CB_T_EVENT;	// 回调类型(不同类型的回调,可能ID一样)
	m_lCallbackID	= -1;				// 回调ID
	m_pEnv			= NULL;				// JAVA环境指针
	m_javaObject	= NULL;				// JAVA对象
	m_fcbFun		= NULL;				// 回调函数
	m_lUserParam	= 0;				// 用户指定的参数
	m_lRes0			= 0;				// 保留位
	m_lRes1			= 0;				// 保留位
	m_lRes2			= 0;				// 保留位
	m_lRes3			= 0;				// 保留位
}
stCBParam::~stCBParam()
{
	Clear(m_lCallbackID);
}
// 清除某个回调参数,小于0,表示全部
void stCBParam::Clear(long lCallbackID)
{
	int		i = 0;
	for (i=0;i<JNV_MAX_STREAM;i++)
	{
		if (s_arCBParam[i].m_lCallbackID==lCallbackID || lCallbackID<0 )
		{
			if ( s_arCBParam[i].m_pEnv )
			{
				s_arCBParam[i].m_pEnv->DeleteGlobalRef(s_arCBParam[i].m_javaObject);
			}
			s_arCBParam[i].stZero();
			s_iCurCBCount--;
			break;
		}
	}
}
// 添加回调参数
PSTCBParam stCBParam::AddNewCB(int iType,long lCallbackID,JNIEnv* env,jobject thiz,LPCTSTR lpszFun,long lUserParam)
{
	int		i = 0;
	if ( s_iCurCBCount>=JNV_MAX_STREAM ) return NULL;
	for (i=0;i<JNV_MAX_STREAM;i++)
	{
		if (s_arCBParam[i].m_lCallbackID<0 )
		{
			jclass clazz;
			clazz =(jclass)(env->GetObjectClass(thiz));

			s_arCBParam[i].m_lCallbackID	= lCallbackID;					// 回调ID
			s_arCBParam[i].m_pEnv			= env;							// JAVA环境指针
			// JAVA对象
			s_arCBParam[i].m_javaObject		= env->NewGlobalRef(thiz);
			// 回调函数
			if ( iType==JNV_CB_T_EVENT )
			{
				// OnConnEvent
				s_arCBParam[i].m_fcbFun		= env->GetMethodID(clazz,lpszFun, "(JLjava/lang/String;)I");
			}
			else if ( iType==JNV_CB_T_DEC_NET )
			{
				// OnDecFrame
				s_arCBParam[i].m_fcbFun		= env->GetMethodID(clazz,lpszFun, "(I[BIIIJII)I");
			}
			else if ( iType==JNV_CB_T_DEC_LOCAL )
			{
				// OnDecFrame
				s_arCBParam[i].m_fcbFun		= env->GetMethodID(clazz,lpszFun, "(I[BIIIJII)I");
			}
			else if ( iType==JNV_CB_T_PLAY_INFO )
			{
				// OnPlayInfo
				s_arCBParam[i].m_fcbFun		= env->GetMethodID(clazz,lpszFun, "(IJJI)I");
			}
			s_arCBParam[i].m_iCBType		= iType;						// 回调类型(不同类型的回调,可能ID一样)
			s_arCBParam[i].m_lUserParam		= lUserParam;					// 用户指定的参数
			env->DeleteLocalRef(clazz);
			JTRACE("AddNewCB:%ld,%s\r\n",lCallbackID,lpszFun);
			return s_arCBParam+i;
		}
	}
	return NULL;
}
// 根据流句柄来获取回调参数
PSTCBParam stCBParam::GetCB(int iType,long lCallbackID)
{
	int		i = 0;
	for (i=0;i<JNV_MAX_STREAM;i++)
	{
		if (lCallbackID<0 || (s_arCBParam[i].m_lCallbackID==lCallbackID && s_arCBParam[i].m_iCBType==iType) )
		{
			return s_arCBParam+i;
		}
	}
	JTRACE("GetCB:%d,%ld err\r\n",iType,lCallbackID);
	return NULL;
}
// 用于回调的参数集 end

// 事件信息 begin
CMutexLock	stItemMsg::s_mutexItemMsg;					// 项目消息锁
bool		stItemMsg::s_bMsgEnable		= false;		// 启用消息处理
pthread_t	stItemMsg::s_hThreadID		= 0;			// 处理线程ID
stItemMsg	stItemMsg::s_arItemMsg[JNV_MAX_MSG];		// 项目消息数组
int			stItemMsg::s_iItemMsgCount	= 0;			// 当前项目消息项数
int			stItemMsg::s_iCurReadMsg	= 0;			// 当前读取项目消息项
int			stItemMsg::s_iCurWriteMsg	= 0;			// 当前写入项目消息项
stItemMsg::stItemMsg()
{
	stZero();
}
stItemMsg::~stItemMsg()
{
	Clear();
}
void stItemMsg::stZero()
{
	m_pstCBParam	= NULL;			// 回调参数
//	m_sjonRet;						// 返回的参数jjson格式
	m_pszRet		= NULL;			// 返回的参数jjson格式
	m_bBusy			= false;		// 是否正在处理中
}
void stItemMsg::Clear()
{
	m_bBusy			= false;		// 是否正在处理中
}
// 事件信息 end

// 字符串转换函数 begin
CJNVString::CJNVString(JNIEnv *env, jstring* lpValue)
{
	m_env 		= env;
	m_lpValue	= lpValue;
	m_lpszValue = env->GetStringUTFChars(*m_lpValue,0);
}
CJNVString::~CJNVString()
{
	if ( m_lpValue )
	{
		m_env->ReleaseStringUTFChars(*m_lpValue, m_lpszValue);
	}
}
CJNVString::operator LPCTSTR() const
{
	return m_lpszValue;
}
LPCTSTR CJNVString::GetString()
{
	return m_lpszValue;
}
// 字符串转换函数 end

// CPU使用情况 begin
CCPUState::CCPUState()
{
	m_iCpuID	= 0;				// CPU索引号
	m_lUser		= 0;				// 从系统启动开始累计到当前时刻,用户态的CPU时间(单位:jiffies)
	m_lNice		= 0;				// 从系统启动开始累计到当前时刻,nice值为负的进程所占用的CPU时间(单位:jiffies)
	m_lSystem	= 0;				// 从系统启动开始累计到当前时刻,核心时间(单位:jiffies)
	m_lIdle		= 0;				// 从系统启动开始累计到当前时刻,除硬盘IO等待时间以外其它等待时间(单位:jiffies)
	m_lIOWait	= 0;				// 从系统启动开始累计到当前时刻,硬盘IO等待时间(单位:jiffies)
	m_lIrq		= 0;				// 从系统启动开始累计到当前时刻,硬中断时间(单位:jiffies)
	m_lSoftIrq	= 0;				// 从系统启动开始累计到当前时刻,软中断时间(单位:jiffies)
}
const CCPUState& CCPUState::operator =( const CCPUState& stSrc )
{
	CCPUState*	pThis = this;
	m_iCpuID	= stSrc.m_iCpuID;			// CPU索引号
	m_lUser		= stSrc.m_lUser;			// 从系统启动开始累计到当前时刻,用户态的CPU时间(单位:jiffies)
	m_lNice		= stSrc.m_lNice;			// 从系统启动开始累计到当前时刻,nice值为负的进程所占用的CPU时间(单位:jiffies)
	m_lSystem	= stSrc.m_lSystem;			// 从系统启动开始累计到当前时刻,核心时间(单位:jiffies)
	m_lIdle		= stSrc.m_lIdle;			// 从系统启动开始累计到当前时刻,除硬盘IO等待时间以外其它等待时间(单位:jiffies)
	m_lIOWait	= stSrc.m_lIOWait;			// 从系统启动开始累计到当前时刻,硬盘IO等待时间(单位:jiffies)
	m_lIrq		= stSrc.m_lIrq;				// 从系统启动开始累计到当前时刻,硬中断时间(单位:jiffies)
	m_lSoftIrq	= stSrc.m_lSoftIrq;			// 从系统启动开始累计到当前时刻,软中断时间(单位:jiffies)
	return *pThis;
}
bool CCPUState::UpdateState(LPCTSTR lpszRead)
{
	char	szName[8]		= {0};
	LPCTSTR	lpszText		= NULL;

//	if ( m_iCpuID==0 )	sprintf(szName,"cpu");
//	else				sprintf(szName,"cpu%d",m_iCpuID);
	lpszText = strstr(lpszRead,szName);
	if ( lpszText==NULL ) return false;
	sscanf(lpszText,"%s%d%d%d%d%d%d%d",szName,&m_lUser,&m_lNice,&m_lSystem,&m_lIdle,&m_lIOWait,&m_lIrq,&m_lSystem);
	return true;
}
float CCPUState::GetCpuTime()
{
	return (float)(m_lUser+m_lSystem+m_lNice+m_lIdle+m_lIOWait+m_lIrq+m_lSoftIrq);
}
float CCPUState::GetUsage(CCPUState* pstStateOld)
{
	if ( pstStateOld==NULL ) return 0.0f;

	float fCpu		= GetCpuTime();
	float fCpuOld	= pstStateOld->GetCpuTime();
	float fRet		= 1.0f -((float)(m_lIdle-pstStateOld->m_lIdle))/(fCpu-fCpuOld);
	return fRet;
}
CCPUInfo::CCPUInfo()
{
	int		i				= 0;

	// 初始化 begin
	m_iCPUCount	= 0;
	for (i=0;i<MAX_CPU_CORE;i++)
	{
		memset(&(m_stState[i]),0,sizeof(CCPUState));
		m_stState[i].m_iCpuID	= i;
		memset(&(m_stStateOld[i]),0,sizeof(CCPUState));
		m_stStateOld[i].m_iCpuID= i;
		m_fUsage[i]				= 0;
	}
	// 初始化 end

	// 更新当前状态
	UpdateState();
}
int CCPUInfo::GetCpuCount()
{
	return m_iCPUCount;
}
bool CCPUInfo::UpdateState()
{
	char	szName[8]		= {0};
	char	szRead[2048]	= {0};
	LPCTSTR	lpszText		= NULL;
	FILE*	pfRead			= NULL;
	int		i				= 0;

	pfRead = fopen("/proc/stat","rb");
	if ( pfRead==NULL )
	{
		JTRACE("CCPUInfo::UpdateState() open /proc/stat err\r\n",m_iCPUCount);
		return false;
	}
	fread(szRead,2048-1,1,pfRead);
	lpszText	= szRead;

	m_iCPUCount = 0;
	for (i=0;i<MAX_CPU_CORE;i++)
	{
		if ( i==0 )	sprintf(szName,"cpu");
		else		sprintf(szName,"cpu%d",i-1);
		lpszText = strstr(lpszText,szName);
		if ( lpszText==NULL || lpszText==szName )
		{
			break;
		}
		else
		{
			m_stState[i].m_iCpuID = i;
			m_stState[i].UpdateState(lpszText);
			m_fUsage[i]		= m_stState[i].GetUsage(m_stStateOld+i);
			m_stStateOld[i] = m_stState[i];
			m_iCPUCount++;
		}
	}
	fclose(pfRead);
	pfRead = NULL;
	JTRACE("CCPUInfo::UpdateState() CpuCount=%d\r\n",m_iCPUCount);
	return true;
}
float CCPUInfo::GetUsage(int iID)
{
	int		i		= 0;
	float	fSum	= 0.0f;

	if ( m_iCPUCount==0 ) return 0.0f;

	if (iID<0 || iID>=m_iCPUCount)
	{
		for (i=0;i<m_iCPUCount;i++)
		{
			fSum += m_fUsage[i];
		}
		return fSum/m_iCPUCount;
	}
	else
	{
		return m_fUsage[iID];
	}

	return 0.0f;
}
CCPUInfo CCPUInfo::s_cpuInfo;
// CPU使用情况 end

// 数字转字符串
LPCTSTR	NumToStr(long lNum,char* strText)
{
	sprintf(strText,"\"%ld\"",lNum);
	return strText;
}
// 数字转字符串
LPCTSTR	FloatToStr(float fNum,char* strText)
{
	sprintf(strText,"\"%.6f\"",fNum);
	return strText;
}
// 事件回调节函数
int OnConnEvent(long lHandle, eJNetEvent eType,int iDataType,void* pEventData,int iDataLen,void* pUserParam)
{
	int					i			= 0;
	float*				pFloat		= NULL;
	PSTItemMsg			pstItemMsg	= NULL;
	PSTJJsonValue		pstRoot		= NULL;
	char				szTemp[128]	= {0};
		
	stItemMsg::s_mutexItemMsg.Lock();
	try
	{
	//	JTRACE("OnConnEvent :%ld,%d,%d,%d\r\n",lHandle,eType,iDataType,iDataLen);
		for (i=stItemMsg::s_iCurWriteMsg+1;i<=JNV_MAX_MSG;i++)
		{
			if ( i==JNV_MAX_MSG )
			{
				// 循环
				i = 0;
			}
			if ( i==stItemMsg::s_iCurWriteMsg )
			{
				pstItemMsg = NULL;
				break;
			}
			pstItemMsg = stItemMsg::s_arItemMsg+i;
			if ( pstItemMsg->m_bBusy==false )
			{
				pstItemMsg->m_bBusy			= true;
				pstItemMsg->m_pstCBParam	= stCBParam::GetCB(JNV_CB_T_EVENT,JNV_EVENT_ID);
				// 填充JJSON begin
				pstRoot	= pstItemMsg->m_sjonRet.InsertValue(JJSON_OBJECT,pstItemMsg->m_sjonRet.GetRootItem(),NULL);
				pstItemMsg->m_sjonRet.InsertNumber("eventType",NumToStr(eType,szTemp),pstRoot,NULL);					// 事件类型
				pstItemMsg->m_sjonRet.InsertNumber("dataType",NumToStr(iDataType,szTemp),pstRoot,NULL);					// 数据类型
				switch(eType)
				{
				case JNET_EET_EVENT_NOTIFY:
					{
						PSTAlarmData		pstAlarm	= (PSTAlarmData)pEventData;
						// 报警
						pstItemMsg->m_sjonRet.InsertString("devID",pstAlarm->szDeviceID,pstRoot,NULL);						// 设备ID
						pstItemMsg->m_sjonRet.InsertString("alarmTime",pstAlarm->szTime,pstRoot,NULL);						// 报警时间
						pstItemMsg->m_sjonRet.InsertNumber("alarmChn",NumToStr(pstAlarm->nChannel,szTemp),pstRoot,NULL);	// 报警通道（通道基于零）;锟斤拷锟斤拷悖�
						pstItemMsg->m_sjonRet.InsertNumber("alarmType",NumToStr(pstAlarm->nAlarmType,szTemp),pstRoot,NULL);	// 报警类型
					}
					break;
				case JNET_EET_EVENT_DEV_LIST:
					// 设备列表
					if ( eJNVFileDevList==iDataType )
					{
						pstItemMsg->m_sjonRet.InsertString("filePath",(LPCTSTR)pEventData,pstRoot,NULL);					// 文件路径
					}
					break;
				case JNET_EET_EVENT_REC_LIST:
					// 录像列表
					if ( eJNVFileRecList==iDataType )
					{
						pstItemMsg->m_sjonRet.InsertString("filePath",(LPCTSTR)pEventData,pstRoot,NULL);					// 文件路径
					}
					break;
				case JNET_EET_EVENT_REC_DOWN:
					// 录像下载
					if ( eJNVFileRecDown==iDataType )
					{
						pstItemMsg->m_sjonRet.InsertString("filePath",(LPCTSTR)pEventData,pstRoot,NULL);					// 文件路径
					}
					break;
				case JNET_EET_EVENT_TRANS_PERCENT:
					// 交互百份比
					pFloat	= (float*)pEventData;
					pstItemMsg->m_sjonRet.InsertNumber("transPercent",FloatToStr(*pFloat,szTemp),pstRoot,NULL);				// 百份比,6位小数值
					break;
				case JNET_EET_EVENT_GPS_INFO:
					{
						// GPS 信息
						PSTJNVGPSBaseInfo	pstGpsBase	= NULL;
						PSTJNVGPSStatus		pstGpsStatus= NULL;
						PSTJNVGPSGSensor	pstGpsSensor= NULL;
						PSTJNVGPSGyroSensor	pstGpsGyro	= NULL;
						PSTJNVGPSExternInfo	pstGpsExt	= NULL;
						PSTJNVGPSAll		pstGPSAll	= NULL;
						pstGPSAll	= (PSTJNVGPSAll)pEventData;
						pstGpsBase	= &(pstGPSAll->stBaseInfo);
						pstGpsStatus= &(pstGPSAll->stStatus);
						pstGpsSensor= &(pstGPSAll->stGSensor);
						pstGpsGyro	= &(pstGPSAll->stGyroSensor);
						pstGpsExt	= &(pstGPSAll->stExternInfo);						
						pstItemMsg->m_sjonRet.InsertString("gpsDevID",pstGPSAll->szDevID,pstRoot,NULL);										// 设备ID
						// 基础信息
						pstItemMsg->m_sjonRet.InsertNumber("gpsBaseValid",NumToStr(pstGpsBase->bValid,szTemp),pstRoot,NULL);				// 信息有效性
						pstItemMsg->m_sjonRet.InsertNumber("gpsBaseLongitude",FloatToStr(pstGpsBase->dbLongitude,szTemp),pstRoot,NULL);		// 经度(>0:东经,<0:西经)
						pstItemMsg->m_sjonRet.InsertNumber("gpsBaseLatitude",FloatToStr(pstGpsBase->dbLatitude,szTemp),pstRoot,NULL);		// 纬度(>0:南纬,<0:北纬)
						pstItemMsg->m_sjonRet.InsertNumber("gpsBaseDirect",FloatToStr(pstGpsBase->fDirect,szTemp),pstRoot,NULL);			// 方向(0~360°)
						pstItemMsg->m_sjonRet.InsertNumber("gpsBaseSpeed",FloatToStr(pstGpsBase->fSpeed,szTemp),pstRoot,NULL);				// 速度(千米/小时,负数无效)
						// GPS状态
						pstItemMsg->m_sjonRet.InsertNumber("gpsStatusValid",NumToStr(pstGpsStatus->bValid,szTemp),pstRoot,NULL);			// 信息有效性
						pstItemMsg->m_sjonRet.InsertNumber("gpsStatusKey",NumToStr(pstGpsStatus->bKeyValid,szTemp),pstRoot,NULL);			// 车钥匙有效性
						pstItemMsg->m_sjonRet.InsertNumber("gpsStatusBrake",NumToStr(pstGpsStatus->bBrakeValid,szTemp),pstRoot,NULL);		// 刹车信号有效性
						pstItemMsg->m_sjonRet.InsertNumber("gpsStatusLeft",NumToStr(pstGpsStatus->bLeftValid,szTemp),pstRoot,NULL);			// 左转有效性
						pstItemMsg->m_sjonRet.InsertNumber("gpsStatusRight",NumToStr(pstGpsStatus->bRightValid,szTemp),pstRoot,NULL);		// 右转有效性
						pstItemMsg->m_sjonRet.InsertNumber("gpsStatusModule",NumToStr(pstGpsStatus->bGPSModuleValid,szTemp),pstRoot,NULL);	// GPS模块有效性
						// GSensor
						pstItemMsg->m_sjonRet.InsertNumber("gpsSensorValid",NumToStr(pstGpsSensor->bValid,szTemp),pstRoot,NULL);			// 信息有效性
						pstItemMsg->m_sjonRet.InsertNumber("gpsSensorAcceX",FloatToStr(pstGpsSensor->fAccelate_X,szTemp),pstRoot,NULL);		// x方向加速度(单位:g)
						pstItemMsg->m_sjonRet.InsertNumber("gpsSensorAcceY",FloatToStr(pstGpsSensor->fAccelate_Y,szTemp),pstRoot,NULL);		// y方向加速度(单位:g)
						pstItemMsg->m_sjonRet.InsertNumber("gpsSensorAcceZ",FloatToStr(pstGpsSensor->fAccelate_Z,szTemp),pstRoot,NULL);		// z方向加速度(单位:g)
						// GyroSensor
						pstItemMsg->m_sjonRet.InsertNumber("gpsGyroValid",NumToStr(pstGpsGyro->bValid,szTemp),pstRoot,NULL);				// 信息有效性
						pstItemMsg->m_sjonRet.InsertNumber("gpsGyroCorner",FloatToStr(pstGpsGyro->fCorner,szTemp),pstRoot,NULL);			// 转角加速度(单位:g)
						// GPS扩展信息
						pstItemMsg->m_sjonRet.InsertNumber("gpsExtValid",NumToStr(pstGpsExt->bValid,szTemp),pstRoot,NULL);							// 信息有效性
						pstItemMsg->m_sjonRet.InsertNumber("gpsExtHigh",NumToStr(pstGpsExt->usHigh,szTemp),pstRoot,NULL);							// 海拔高度
						pstItemMsg->m_sjonRet.InsertNumber("gpsExtSatelliteNumber",NumToStr(pstGpsExt->ucSatelliteNumber,szTemp),pstRoot,NULL);		// 卫星数量
						pstItemMsg->m_sjonRet.InsertNumber("gpsExtTemperature",FloatToStr(pstGpsExt->fTemperature,szTemp),pstRoot,NULL);			// 温度(摄氏度,>=-10000无效)
						// 时间信息
						pstItemMsg->m_sjonRet.InsertString("gpsInfoTime",pstGPSAll->stTime,pstRoot,NULL);								// 时间信息
					}
					break;
				}
				pstItemMsg->m_pszRet		= pstItemMsg->m_sjonRet.TreeToString(pstRoot);								// 返回的参数jjson格式
			//	JTRACE("OnConnEvent :%s\r\n",pstItemMsg->m_pszRet);

				// 填充JJSON end
				stItemMsg::s_iCurWriteMsg	= i;					// 当前写入项目消息项
				if ( stItemMsg::s_iItemMsgCount==0 )
				{
					stItemMsg::s_iCurReadMsg = stItemMsg::s_iCurWriteMsg;
				}
				stItemMsg::s_iItemMsgCount++;						// 当前项目消息项数
				break;
			}
		}
	}
	catch(...)
	{
		JTRACE("try-catch OnConnEvent\r\n");
	}
	stItemMsg::s_mutexItemMsg.Unlock();
	/*
	JNET_EET_UNKNOWN			= -1,		// 未定义
											
	JNET_EET_LOGINING			= 0,		// 正在登录
	JNET_EET_LOGIN_OK			= 1,		// 登录成功
	JNET_EET_LOGIN_ERROR		= 2,		// 登录失败
	JNET_EET_LOGOUT				= 3,		// 登出
	JNET_EET_STREAM_OPENING		= 4,		// 流正在打开,子项见 eJNVStreamType
	JNET_EET_STREAM_OK			= 5,		// 流打开成功,子项见 eJNVStreamType
	JNET_EET_STREAM_ERROR		= 6,		// 流打开失败,子项见 eJNVStreamType
	JNET_EET_STREAM_CLOSE		= 7,		// 流关闭
	JNET_EET_PB_OPENING			= 8,		// 回放流正在打开
	JNET_EET_PB_OK				= 9,		// 回放流打开成功
	JNET_EET_PB_ERROR			= 10,		// 回放流打开失败
	JNET_EET_PB_CLOSE			= 11,		// 回放流关闭
	JNET_EET_TALK_OPENING		= 12,		// 对讲正在打开
	JNET_EET_TALK_OK			= 13,		// 对讲打开成功
	JNET_EET_TALK_ERROR			= 14,		// 对讲打开失败
	JNET_EET_TALK_CLOSE			= 15,		// 对讲关闭
	JNET_EET_UPG_START			= 16,		// 升级开始
	JNET_EET_UPG_OK				= 17,		// 升级成功
	JNET_EET_UPG_ERR			= 18,		// 升级失败
	JNET_EET_CFG_SET			= 100,		// 获取配置
	JNET_EET_CFG_GET			= 101,		// 设置配置
	JNET_EET_PTZ_CTRL			= 102,		// 云台控制
	JNET_EET_DEV_CTRL			= 103,		// 设备控制
	JNET_EET_STREAM_LOST		= 104,		// 流丢失
	JNET_EET_EVENT_NOTIFY		= 201,		// 报警事件
	JNET_EET_EVENT_DEV_LIST		= 301,		// 获取设备列表,iDataType=eJNVFileType
	JNET_EET_EVENT_REC_LIST		= 302,		// 获取录像列表,iDataType=eJNVFileType
	JNET_EET_EVENT_REC_DOWN		= 303,		// 录像下载,iDataType=eJNVFileType
	JNET_EET_EVENT_TRANS_PERCENT= 304,		// 交互百份比,iDataType=eJNVFileType
	JNET_EET_EVENT_GPS_INFO		= 400,		// 下发GPS信息,pEventData=PSTJNVGPSAll
	*/
	return 0;
}


// 网络流解码回调
int OnDecFrameNet(PSTDecFrameCB pstDecFrameCB)
{
	PSTCBParam	pstCBParam	= NULL;
	JNIEnv*		pEnv		= (JNIEnv*)(pstDecFrameCB->dwRes1);		// JAVA环境指针
	
//	JTRACE("OnDecFrameNet size=%d,type=%d,lpBuf=0x%08x\r\n",
//		pstDecFrameCB->lSize,pstDecFrameCB->lType,pstDecFrameCB->lpBuf);
	pstCBParam = stCBParam::GetCB(JNV_CB_T_DEC_NET,pstDecFrameCB->lParam);
	if ( pstCBParam )
	{
		pEnv->CallIntMethod(pstCBParam->m_javaObject,pstCBParam->m_fcbFun,
			pstDecFrameCB->lParam,pstDecFrameCB->lpBuf,pstDecFrameCB->lSize,
			pstDecFrameCB->lWidth,pstDecFrameCB->lHeight,(__int64)pstDecFrameCB->lStamp,pstDecFrameCB->lType,
			pstCBParam->m_lUserParam);
	}
	else
	{
		JTRACE("OnDecFrameNet GetCB err\r\n");
	}
	return 0;
}
int OnDecFrameLocal(PSTDecFrameCB pstDecFrameCB)
{
	PSTCBParam	pstCBParam	= NULL;
	JNIEnv*		pEnv		= (JNIEnv*)(pstDecFrameCB->dwRes1);		// JAVA环境指针
	
//	JTRACE("OnDecFrameLocal size=%d,type=%d,lpBuf=0x%08x\r\n",
//		pstDecFrameCB->lSize,pstDecFrameCB->lType,pstDecFrameCB->lpBuf);
	pstCBParam = stCBParam::GetCB(JNV_CB_T_DEC_LOCAL,pstDecFrameCB->lParam);
	if ( pstCBParam )
	{
		pEnv->CallIntMethod(pstCBParam->m_javaObject,pstCBParam->m_fcbFun,
			pstDecFrameCB->lParam,pstDecFrameCB->lpBuf,pstDecFrameCB->lSize,
			pstDecFrameCB->lWidth,pstDecFrameCB->lHeight,(__int64)pstDecFrameCB->lStamp,pstDecFrameCB->lType,
			pstCBParam->m_lUserParam);
	}
	else
	{
		JTRACE("OnDecFrameLocal GetCB err\r\n");
	}
	return 0;
}

// 录像回放信息
int OnPlayInfo(AVPPlayCBMsg eMsg,int iPlayerID,long lpUserParam,__int64 lParam1,__int64 lParam2,DWORD dwRes)
{
	PSTCBParam	pstCBParam	= NULL;
	JNIEnv*		pEnv		= (JNIEnv*)(dwRes);		// JAVA环境指针
	
//	JTRACE("OnPlayInfo eMsg=%d,iPlayerID=%d\r\n",eMsg,iPlayerID);
	pstCBParam = stCBParam::GetCB(JNV_CB_T_PLAY_INFO,iPlayerID);
	if ( pstCBParam )
	{
		pEnv->CallIntMethod(pstCBParam->m_javaObject,pstCBParam->m_fcbFun,
			eMsg,lParam1,lParam2,pstCBParam->m_lUserParam);
	}
	else
	{
		JTRACE("OnPlayInfo GetCB err\r\n");
	}
	return 0;
}

// 消息分派
fJThRet RunMsgDispath(void* pParam)
{
	int				iRet				= 0;			// 返回值
	JNIEnv*			pEventEnv			= NULL;			// JAVA环境指针
	PSTItemMsg		pstMsg				= NULL;
	PSTCBParam		pstCBParam			= NULL;

	try
	{
		if ( g_jvm->AttachCurrentThread(&pEventEnv, NULL)!=JNI_OK )
		{
			JTRACE("RunMsgDispath AttachCurrentThread ERR\r\n");
		}
		// 进行任务处理 begin
		while(stItemMsg::s_bMsgEnable)
		{
			JSleep(10);

			stItemMsg::s_mutexItemMsg.Lock();
			// 获取需要处理的消息 begin
			pstMsg	= NULL;
			if ( stItemMsg::s_iItemMsgCount>0 )
			{
				if ( stItemMsg::s_iCurReadMsg==JNV_MAX_MSG )
				{
					// 循环
					stItemMsg::s_iCurReadMsg = 0;
				}
				pstMsg	= stItemMsg::s_arItemMsg+stItemMsg::s_iCurReadMsg;
			}
			// 获取需要处理的消息 end
			stItemMsg::s_mutexItemMsg.Unlock();

			if ( pstMsg && pstMsg->m_bBusy )
			{
				// 处理回调消息 begin
				pstCBParam	= pstMsg->m_pstCBParam;
				if (pstMsg->m_pszRet)
				{
					jstring jstrEvent = pEventEnv->NewStringUTF(pstMsg->m_pszRet);
					pEventEnv->CallIntMethod(pstCBParam->m_javaObject,pstCBParam->m_fcbFun,
						(__int64)(pstCBParam->m_lUserParam),jstrEvent);
					pEventEnv->ReleaseStringUTFChars(jstrEvent, pEventEnv->GetStringUTFChars(jstrEvent,JNI_FALSE));
					free(pstMsg->m_pszRet);
					pEventEnv->DeleteLocalRef(jstrEvent);
					pstMsg->m_pszRet = NULL;
				}
				// 处理回调消息 end

				stItemMsg::s_mutexItemMsg.Lock();
				// 释放消息 begin
				pstMsg->m_bBusy = false;
				stItemMsg::s_iCurReadMsg++;
				stItemMsg::s_iItemMsgCount--;
				// 释放消息 end
				stItemMsg::s_mutexItemMsg.Unlock();
			}
		}
		// 进行任务处理 end
		g_jvm->DetachCurrentThread();
	}
	catch (...)
	{
		JTRACE("try-catch CItemBase::RunMsgDispath\r\n");
	}
	return 0;
}
