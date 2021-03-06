
#include <stdlib.h>

#include "IOSUtils.h"

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
			s_arCBParam[i].stZero();
			s_iCurCBCount--;
			break;
		}
	}
}
// 添加回调参数
PSTCBParam stCBParam::AddNewCB(int iType,long lCallbackID,void* fcbFun,long lUserParam)
{
	int		i = 0;
	if ( s_iCurCBCount>=JNV_MAX_STREAM ) return NULL;
	for (i=0;i<JNV_MAX_STREAM;i++)
	{
		if (s_arCBParam[i].m_lCallbackID<0 )
		{
			s_arCBParam[i].m_lCallbackID	= lCallbackID;					// 回调ID
			// 回调函数
			s_arCBParam[i].m_fcbFun			= fcbFun;
			s_arCBParam[i].m_iCBType		= iType;						// 回调类型(不同类型的回调,可能ID一样)
			s_arCBParam[i].m_lUserParam		= lUserParam;					// 用户指定的参数
			JTRACE("AddNewCB:%ld,%d\r\n",lCallbackID,iType);
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

// 系统操作 begin
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
						pstItemMsg->m_sjonRet.InsertNumber("alarmChn",NumToStr(pstAlarm->nChannel,szTemp),pstRoot,NULL);	// 报警通道（通道基于零）;锟斤拷锟斤拷悖?
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
						pstItemMsg->m_sjonRet.InsertNumber("gpsDevID",pstGPSAll->szDevID,pstRoot,NULL);										// 设备ID
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
                default:
                    break;
				}
				pstItemMsg->m_pszRet		= pstItemMsg->m_sjonRet.TreeToString(pstRoot);								// 返回的参数jjson格式
				JTRACE("OnConnEvent :%s\r\n",pstItemMsg->m_pszRet);

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
	return 0;

}

// 网络流解码回调

int OnDecFrameNet(PSTDecFrameCB pstDecFrameCB)
{
	PSTCBParam		pstCBParam	= NULL;
	fcbDecFrame*	pFun		= NULL;
	stDecFrameCB	stCB;
	
//	JTRACE("OnDecFrameNet size=%d,type=%d,lpBuf=0x%08x\r\n",
//		pstDecFrameCB->lSize,pstDecFrameCB->lType,pstDecFrameCB->lpBuf);
	memcpy(&stCB,pstDecFrameCB,sizeof(stCB));
	pstCBParam = stCBParam::GetCB(JNV_CB_T_DEC_NET,pstDecFrameCB->lParam);
	if ( pstCBParam )
	{
		stCB.lParam	= pstCBParam->m_lUserParam;
		pFun	= (fcbDecFrame*)(pstCBParam->m_fcbFun);
		if ( pFun )
		{
			pFun(&stCB);
		}
	}
	else
	{
		JTRACE("OnDecFrameNet GetCB err\r\n");
	}
	return 0;
}
int OnDecFrameLocal(PSTDecFrameCB pstDecFrameCB)
{
	PSTCBParam		pstCBParam	= NULL;
	fcbDecFrame*	pFun		= NULL;
	stDecFrameCB	stCB;
	
//	JTRACE("OnDecFrameLocal size=%d,type=%d,lpBuf=0x%08x\r\n",
//		pstDecFrameCB->lSize,pstDecFrameCB->lType,pstDecFrameCB->lpBuf);
	memcpy(&stCB,pstDecFrameCB,sizeof(stCB));
	pstCBParam = stCBParam::GetCB(JNV_CB_T_DEC_LOCAL,pstDecFrameCB->lParam);
	if ( pstCBParam )
	{
		stCB.lParam	= pstCBParam->m_lUserParam;
		pFun	= (fcbDecFrame*)(pstCBParam->m_fcbFun);
		if ( pFun )
		{
			pFun(&stCB);
		}
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
	PSTCBParam		pstCBParam	= NULL;
	fcbPlay*		pFun		= NULL;
	
//	JTRACE("OnPlayInfo eMsg=%d,iPlayerID=%d\r\n",eMsg,iPlayerID);
	pstCBParam = stCBParam::GetCB(JNV_CB_T_PLAY_INFO,iPlayerID);
	if ( pstCBParam )
	{
		pFun	= (fcbPlay*)(pstCBParam->m_fcbFun);
		if ( pFun )
		{
			pFun(eMsg,iPlayerID,pstCBParam->m_lUserParam,lParam1,lParam2,0);
		}
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
	PSTItemMsg		pstMsg				= NULL;
	PSTCBParam		pstCBParam			= NULL;
	fcbJNVEvent*	pCBEvent			= NULL;

	try
	{
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
					pCBEvent	= (fcbJNVEvent*)(pstCBParam->m_fcbFun);
					if ( pCBEvent ) pCBEvent(pstMsg->m_pszRet,(void*)(pstCBParam->m_lUserParam));
					free(pstMsg->m_pszRet);
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
	}
	catch (...)
	{
		JTRACE("try-catch CItemBase::RunMsgDispath\r\n");
	}
	return 0;
}
