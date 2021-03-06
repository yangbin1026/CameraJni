// NVTestDlg.cpp : implementation file
//

#include "stdafx.h"
#include "NVTest.h"
#include "NVTestDlg.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

static CNVTestDlg*	s_pDlgMain	= NULL;
/////////////////////////////////////////////////////////////////////////////
// CNVTestDlg dialog

CNVTestDlg::CNVTestDlg(CWnd* pParent /*=NULL*/)
	: CDialog(CNVTestDlg::IDD, pParent)
{
	//{{AFX_DATA_INIT(CNVTestDlg)
		// NOTE: the ClassWizard will add member initialization here
	//}}AFX_DATA_INIT
	// Note that LoadIcon does not require a subsequent DestroyIcon in Win32
	m_hIcon		= AfxGetApp()->LoadIcon(IDR_MAINFRAME);
	m_lStream	= 0;
	m_lRecFile	= 0;
	m_lReplay	= 0;
	m_lTalk		= 0;
	
	// 默认的WFX格式 begin
	m_wfxWav.wFormatTag		= WAVE_FORMAT_PCM;
	m_wfxWav.nChannels		= 1;
	m_wfxWav.wBitsPerSample	= 16;
	m_wfxWav.nSamplesPerSec	= 8000;//8820 11025 22050 44100;
	m_wfxWav.nBlockAlign	= m_wfxWav.nChannels * (m_wfxWav.wBitsPerSample / 8);
	m_wfxWav.nAvgBytesPerSec= m_wfxWav.nBlockAlign * m_wfxWav.nSamplesPerSec;
	m_wfxWav.cbSize			= 0;
	// 默认的WFX格式 end
}

void CNVTestDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	//{{AFX_DATA_MAP(CNVTestDlg)
	DDX_Control(pDX, IDC_ST_VIEW, m_stView);
	//}}AFX_DATA_MAP
}

BEGIN_MESSAGE_MAP(CNVTestDlg, CDialog)
	//{{AFX_MSG_MAP(CNVTestDlg)
	ON_BN_CLICKED(IDC_BT_GET_DEV_LIST, OnBtGetDevList)
	ON_BN_CLICKED(IDC_BT_GET_GPS, OnBtGetGps)
	ON_BN_CLICKED(IDC_BT_CLOSE_GPS, OnBtCloseGps)
	ON_BN_CLICKED(IDC_BT_GET_REC_LIST, OnBtGetRecList)
	ON_BN_CLICKED(IDC_BT_GET_REC_DOWN, OnBtGetRecDown)
	ON_BN_CLICKED(IDC_BT_OPEN_STREAM, OnBtOpenStream)
	ON_BN_CLICKED(IDC_BT_CLOSE_STREAM, OnBtCloseStream)
	ON_BN_CLICKED(IDC_BT_LOGIN, OnBtLogin)
	ON_BN_CLICKED(IDC_BT_LOGOUT, OnBtLogout)
	ON_WM_DESTROY()
	ON_BN_CLICKED(IDC_BT_CAPTURE, OnBtCapture)
	ON_BN_CLICKED(IDC_BT_START_REC, OnBtStartRec)
	ON_BN_CLICKED(IDC_BT_STOP_REC, OnBtStopRec)
	ON_BN_CLICKED(IDC_BT_OPEN_REC, OnBtOpenRec)
	ON_BN_CLICKED(IDC_BT_CLOSE_REC, OnBtCloseRec)
	ON_BN_CLICKED(IDC_BT_CAPTURE_REC, OnBtCaptureRec)
	ON_BN_CLICKED(IDC_BT_OPEN_REPLAY, OnBtOpenReplay)
	ON_BN_CLICKED(IDC_BT_PAUSE_REPLAY, OnBtPauseReplay)
	ON_BN_CLICKED(IDC_BT_CLOSE_REPLAY, OnBtCloseReplay)
	ON_BN_CLICKED(IDC_BT_GET_ALARM, OnBtGetAlarm)
	ON_BN_CLICKED(IDC_BT_CLOSE_ALARM, OnBtCloseAlarm)
	ON_BN_CLICKED(IDC_BT_PAUSE_REC, OnBtPauseRec)
	ON_BN_CLICKED(IDC_BT_CONTINUE_REC, OnBtContinueRec)
	ON_BN_CLICKED(IDC_BT_TEST, OnBtTest)
	ON_BN_CLICKED(IDC_BT_START_TALK, OnBtStartTalk)
	ON_BN_CLICKED(IDC_BT_STOP_TALK, OnBtStopTalk)
	ON_WM_TIMER()
	ON_BN_CLICKED(IDC_BT_GET_SERVER_LIST, OnBtGetServerList)
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CNVTestDlg message handlers

#include "JJson.h"

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

int __stdcall OnDecFrameLocal(PSTDecFrameCB pstDecFrameCB)
{
	return s_pDlgMain->OnDecFrame(pstDecFrameCB);
}

// 事件回调节函数
int __stdcall OnConnEvent(long lHandle, eJNetEvent eType,int iDataType,void* pEventData,int iDataLen,void* pUserParam)
{
	int					i			= 0;
	CJJson				m_sjonRet;
	float*				pFloat		= NULL;
	PSTJJsonValue		pstRoot		= NULL;
	char				szTemp[128]	= {0};
	char*				m_pszRet	= NULL;

	// 填充JJSON begin
	pstRoot	= m_sjonRet.InsertValue(JJSON_OBJECT,m_sjonRet.GetRootItem(),NULL);
	m_sjonRet.InsertNumber("eventType",NumToStr(eType,szTemp),pstRoot,NULL);					// 事件类型
	m_sjonRet.InsertNumber("dataType",NumToStr(iDataType,szTemp),pstRoot,NULL);					// 数据类型
	switch(eType)
	{
	case JNET_EET_EVENT_NOTIFY:
		{
			PSTAlarmData		pstAlarm	= (PSTAlarmData)pEventData;
			// 报警
			m_sjonRet.InsertString("devID",pstAlarm->szDeviceID,pstRoot,NULL);						// 设备ID
			m_sjonRet.InsertString("alarmTime",pstAlarm->szTime,pstRoot,NULL);						// 报警时间
			m_sjonRet.InsertNumber("alarmChn",NumToStr(pstAlarm->nChannel,szTemp),pstRoot,NULL);	// 报警通道（通道基于零）
			m_sjonRet.InsertNumber("alarmType",NumToStr(pstAlarm->nAlarmType,szTemp),pstRoot,NULL);	// 报警类型
		}
		break;
	case JNET_EET_EVENT_DEV_LIST:
		// 设备列表
		if ( eJNVFileDevList==iDataType )
		{
			m_sjonRet.InsertString("filePath",(LPCTSTR)pEventData,pstRoot,NULL);					// 文件路径
		}
		break;
	case JNET_EET_EVENT_SERVER_LIST:
		// 设备列表
		if ( eJNVFileServerList==iDataType )
		{
			m_sjonRet.InsertString("filePath",(LPCTSTR)pEventData,pstRoot,NULL);					// 文件路径
		}
		break;
	case JNET_EET_EVENT_REC_LIST:
		// 录像列表
		if ( eJNVFileRecList==iDataType )
		{
			m_sjonRet.InsertString("filePath",(LPCTSTR)pEventData,pstRoot,NULL);					// 文件路径
		}
		break;
	case JNET_EET_EVENT_REC_DOWN:
		// 录像下载
		if ( eJNVFileRecDown==iDataType )
		{
			m_sjonRet.InsertString("filePath",(LPCTSTR)pEventData,pstRoot,NULL);					// 文件路径
		}
		break;
	case JNET_EET_EVENT_TRANS_PERCENT:
		// 交互百份比
		pFloat	= (float*)pEventData;
		m_sjonRet.InsertNumber("transPercent",FloatToStr(*pFloat,szTemp),pstRoot,NULL);				// 百份比,2位小数值
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
			// 基础信息
			m_sjonRet.InsertString("gpsDevID",pstGPSAll->szDevID,pstRoot,NULL);										// 设备ID
			m_sjonRet.InsertNumber("gpsBaseValid",NumToStr(pstGpsBase->bValid,szTemp),pstRoot,NULL);				// 信息有效性
			m_sjonRet.InsertNumber("gpsBaseLongitude",FloatToStr(pstGpsBase->dbLongitude,szTemp),pstRoot,NULL);		// 经度(>0:东经,<0:西经)
			m_sjonRet.InsertNumber("gpsBaseLatitude",FloatToStr(pstGpsBase->dbLatitude,szTemp),pstRoot,NULL);		// 纬度(>0:南纬,<0:北纬)
			m_sjonRet.InsertNumber("gpsBaseDirect",FloatToStr(pstGpsBase->fDirect,szTemp),pstRoot,NULL);			// 方向(0~360°)
			m_sjonRet.InsertNumber("gpsBaseSpeed",FloatToStr(pstGpsBase->fSpeed,szTemp),pstRoot,NULL);				// 速度(千米/小时,负数无效)
			/*
			// GPS状态
			m_sjonRet.InsertNumber("gpsStatusValid",NumToStr(pstGpsStatus->bValid,szTemp),pstRoot,NULL);			// 信息有效性
			m_sjonRet.InsertNumber("gpsStatusKey",NumToStr(pstGpsStatus->bKeyValid,szTemp),pstRoot,NULL);			// 车钥匙有效性
			m_sjonRet.InsertNumber("gpsStatusBrake",NumToStr(pstGpsStatus->bBrakeValid,szTemp),pstRoot,NULL);		// 刹车信号有效性
			m_sjonRet.InsertNumber("gpsStatusLeft",NumToStr(pstGpsStatus->bLeftValid,szTemp),pstRoot,NULL);			// 左转有效性
			m_sjonRet.InsertNumber("gpsStatusRight",NumToStr(pstGpsStatus->bRightValid,szTemp),pstRoot,NULL);		// 右转有效性
			m_sjonRet.InsertNumber("gpsStatusModule",NumToStr(pstGpsStatus->bGPSModuleValid,szTemp),pstRoot,NULL);	// GPS模块有效性
			// GSensor
			m_sjonRet.InsertNumber("gpsSensorValid",NumToStr(pstGpsSensor->bValid,szTemp),pstRoot,NULL);			// 信息有效性
			m_sjonRet.InsertNumber("gpsSensorAcceX",FloatToStr(pstGpsSensor->fAccelate_X,szTemp),pstRoot,NULL);		// x方向加速度(单位:g)
			m_sjonRet.InsertNumber("gpsSensorAcceY",FloatToStr(pstGpsSensor->fAccelate_Y,szTemp),pstRoot,NULL);		// y方向加速度(单位:g)
			m_sjonRet.InsertNumber("gpsSensorAcceZ",FloatToStr(pstGpsSensor->fAccelate_Z,szTemp),pstRoot,NULL);		// z方向加速度(单位:g)
			// GyroSensor
			m_sjonRet.InsertNumber("gpsGyroValid",NumToStr(pstGpsGyro->bValid,szTemp),pstRoot,NULL);				// 信息有效性
			m_sjonRet.InsertNumber("gpsGyroCorner",FloatToStr(pstGpsGyro->fCorner,szTemp),pstRoot,NULL);			// 转角加速度(单位:g)
			// GPS扩展信息
			m_sjonRet.InsertNumber("gpsExtValid",NumToStr(pstGpsExt->bValid,szTemp),pstRoot,NULL);							// 信息有效性
			m_sjonRet.InsertNumber("gpsExtHigh",NumToStr(pstGpsExt->usHigh,szTemp),pstRoot,NULL);							// 海拔高度
			m_sjonRet.InsertNumber("gpsExtSatelliteNumber",NumToStr(pstGpsExt->ucSatelliteNumber,szTemp),pstRoot,NULL);		// 卫星数量
			m_sjonRet.InsertNumber("gpsExtTemperature",FloatToStr(pstGpsExt->fTemperature,szTemp),pstRoot,NULL);			// 温度(摄氏度,>=-10000无效)
			// 时间信息
			m_sjonRet.InsertString("gpsInfoTime",pstGPSAll->stTime,pstRoot,NULL);								// 时间信息
			*/
		}
		break;
	}
	m_pszRet		= m_sjonRet.TreeToString(pstRoot);								// 返回的参数jjson格式
	JTRACE("OnConnEvent :%s\r\n",m_pszRet);
	free(m_pszRet);
	// 填充JJSON end

	return 0;
}

int CNVTestDlg::OnDecFrame(PSTDecFrameCB pstDecFrameCB)
{
	if ( pstDecFrameCB==NULL ) return 0;

	if ( pstDecFrameCB->lType==AVPDecCBTypeRGB32 )
	{
		int			iWidth	= m_drawWnd.GetWidth();
		int			iHeight	= m_drawWnd.GetHeight();
		if ( pstDecFrameCB->lWidth==0 || pstDecFrameCB->lHeight==0 ) return 0;
		if ( pstDecFrameCB->lWidth!=iWidth || pstDecFrameCB->lHeight==iHeight )
		{
			m_drawWnd.InitBackSurface(pstDecFrameCB->lHeight,pstDecFrameCB->lWidth,false);
		}
		m_drawWnd.DrawFrame(pstDecFrameCB->lpBuf,pstDecFrameCB->lSize,pstDecFrameCB->lStamp);
	}
	else if ( pstDecFrameCB->lType==AVPDecCBTypeYUV420 )
	{
		int			iWidth	= m_drawWnd.GetWidth();
		int			iHeight	= m_drawWnd.GetHeight();
		if ( pstDecFrameCB->lWidth==0 || pstDecFrameCB->lHeight==0 ) return 0;
		if ( pstDecFrameCB->lWidth!=iWidth || pstDecFrameCB->lHeight==iHeight )
		{
			m_drawWnd.InitBackSurface(pstDecFrameCB->lHeight,pstDecFrameCB->lWidth,true);
		}
		m_drawWnd.DrawFrame(pstDecFrameCB->lpBuf,pstDecFrameCB->lSize,pstDecFrameCB->lStamp);
	}
	else if ( pstDecFrameCB->lType==AVPDecCBTypeAudio16 )
	{
		m_dsOut.SetBufDataOut(pstDecFrameCB->lpBuf,pstDecFrameCB->lSize);
	}
	
	return 0;
}

BOOL CNVTestDlg::OnInitDialog()
{
	int		iRGBMode	= JDRAW_RGB_NONE;
	CDialog::OnInitDialog();
	
	m_jnvMng.Init(1024);
	m_jnvMng.m_iImgRGBFM	= iRGBMode;
	m_jnvMng.m_pfEventFun	= OnConnEvent;
	m_jnvMng.m_pfDecFunLocal= OnDecFrameLocal;
	m_jnvMng.m_pfDecFunNet	= OnDecFrameLocal;

	s_pDlgMain	= this;
	m_drawWnd.InitDD(NULL,m_stView.GetSafeHwnd(),false,iRGBMode);
	
	m_wfxWav.nSamplesPerSec = 8000;
	m_dsIn.InitDSBuffIn(&m_wfxWav,40);
	m_dsOut.InitDSBuffOut(m_hWnd,&m_wfxWav,50);

	m_dsIn.Record();
	m_dsOut.Play();
	Sleep(500);
	SetTimer(TIMER_REC,1000/25,NULL);
	return TRUE;  // return TRUE  unless you set the focus to a control
}

void CNVTestDlg::OnDestroy() 
{
	m_dsOut.DestoryDSound();
	m_dsIn.DestoryDSound();
	m_drawWnd.DestoryDD();
	CDialog::OnDestroy();
	m_jnvMng.Clean();
}

void CNVTestDlg::OnBtLogin() 
{
//	m_jnvMng.Login("113.240.225.137",6008,"g2","123",15);
	m_jnvMng.Login("59.44.127.34",9008,"rq","rrq22",15);
	//m_jnvMng.Login("218.17.160.218",9015,"xulan1","123",15);

	//m_jnvMng.Login("192.168.3.45",7800,"super","super",15);
	//m_jnvMng.Login("192.168.4.160",7808,"super","super",15);
	//m_jnvMng.Login("220.231.128.170",6700,"super","111",15);
	//m_jnvMng.Login("218.67.64.106",6008,"super","111",15);
	//m_jnvMng.Login("218.86.119.220",6008,"super","111",15);
	//m_jnvMng.SetConnectServerType(1);
	//m_jnvMng.AddServerInfo("11112", "192.168.4.160", 8808, 1 );
	//m_jnvMng.AddServerInfo("11113", "192.168.4.160", 8810, 2 );
	//m_jnvMng.AddServerInfo("46002", "218.67.64.106", 8810, 2 );

}

void CNVTestDlg::OnBtLogout() 
{
	m_jnvMng.Logout();
}

void CNVTestDlg::OnBtGetDevList() 
{
	m_jnvMng.GetDevList(".\\DevList.xml");
}

void CNVTestDlg::OnBtGetGps() 
{
	m_jnvMng.GetGPSStart("10010#11111#11112#11113");

	//m_jnvMng.GetGPSStart(NULL);
}

void CNVTestDlg::OnBtCloseGps() 
{
	m_jnvMng.GetGPSStop("10010#11111#11112#11113");
	//m_jnvMng.GetGPSStop(NULL);
}

void CNVTestDlg::OnBtGetAlarm() 
{
	m_jnvMng.GetAlarmStart("10010#11111#11112#11113");
	//m_jnvMng.GetAlarmStart("");
}

void CNVTestDlg::OnBtCloseAlarm() 
{
	m_jnvMng.GetAlarmStop("10010#11111#11112#11113");
	//m_jnvMng.GetAlarmStop("");
}

void CNVTestDlg::OnBtGetRecList() 
{
	m_jnvMng.RecQuery("00802",0,1,"2013-04-27 00:00:00","2013-04-27 23:59:59",0x1111,".\\RecList.xml");
}

void CNVTestDlg::OnBtGetRecDown() 
{
	m_jnvMng.RecDownload("60502","/sd1/2013-03-12/F003001P1N1P000.asf",0,".\\Rec.jnv");
}

void CNVTestDlg::OnBtOpenStream() 
{
	//m_lStream = m_jnvMng.RealStreamStart("00300",1,0,0);
	m_lStream = m_jnvMng.RealStreamStart("57021",1,0,0);
	//m_lStream = m_jnvMng.RealStreamStart("70123",1,0,0);
	//m_lStream = m_jnvMng.RealStreamStart("11111",1,0,0);

	//m_jnvMng.AddServerInfo("11113", "192.168.4.160", 8810, 2 );

	//m_lStream = m_jnvMng.RealStreamStart("4000210010#40002#43002#46002",1,0,0);

	//m_lStream = m_jnvMng.RealStreamStart("1111110010#40002#43002#46002",1,0,0);

	//m_lStream = m_jnvMng.RealStreamStart("1111110010#11111#11112#11113",1,0,0);
	
}

void CNVTestDlg::OnBtCapture() 
{
	m_jnvMng.Capture(m_lStream,"D:\\AAAA.BMP");
}

void CNVTestDlg::OnBtStartRec() 
{
	m_jnvMng.RecStart(m_lStream,"D:\\AAA.JNV");
}

void CNVTestDlg::OnBtStopRec() 
{
	m_jnvMng.RecStop(m_lStream);
}

void CNVTestDlg::OnBtCloseStream() 
{
	m_jnvMng.RealStreamStop(m_lStream);
}

void CNVTestDlg::OnBtOpenReplay() 
{
	m_lReplay = m_jnvMng.ReplayStart("60502",0,1,"2013-03-12 06:50:55","2013-03-12 06:55:55",0,1,0);
}

void CNVTestDlg::OnBtPauseReplay() 
{
	m_jnvMng.ReplayCtrl(m_lReplay,eNVReplayPause,1);
}

void CNVTestDlg::OnBtCloseReplay() 
{
	m_jnvMng.ReplayStop(m_lReplay);
}

static int s_iIndex = 0;
void CNVTestDlg::OnBtOpenRec() 
{
	m_lRecFile = m_jnvMng.RecOpenFile("D:\\AAA.JNV");;
	return;

	if ( s_iIndex%2 )	m_lRecFile = m_jnvMng.RecOpenFile("D:\\AAA.JNV");
	else				m_lRecFile = m_jnvMng.RecOpenFile("D:\\720P_25.JNV");
	s_iIndex++;
//	m_lRecFile = m_jnvMng.RecOpenFile("D:\\20130504095430.jav");
}

void CNVTestDlg::OnBtCaptureRec() 
{
	m_jnvMng.RecPlayCapture(m_lRecFile,"D:\\AAAA.BMP");
}

void CNVTestDlg::OnBtPauseRec() 
{
	m_jnvMng.RecPlayPause(m_lRecFile);
}

void CNVTestDlg::OnBtContinueRec() 
{
	m_jnvMng.RecPlayStart(m_lRecFile);
}

void CNVTestDlg::OnBtCloseRec() 
{
	m_jnvMng.RecCloseFile(m_lRecFile);
	
}

void CNVTestDlg::OnBtStartTalk() 
{
	m_lTalk = m_jnvMng.TalkStart("91001",1);
}

void CNVTestDlg::OnBtStopTalk() 
{
	m_jnvMng.TalkStop(m_lTalk);
}

void CNVTestDlg::OnBtTest() 
{
}

void CNVTestDlg::OnTimer(UINT nIDEvent) 
{
	UINT				uWrite1				= 0;
	UINT				uWrite2				= 0;
	LPVOID				lpDataPtr1			= NULL;
	DWORD				dwDataBytes1		= 0;
	LPVOID				lpDataPtr2			= NULL;
	DWORD				dwDataBytes2		= 0;
	DWORD				dwRet				= 0;
	BYTE				lpOut[4196]			= {0};
	int					iOutLen				= 4196;

	if ( nIDEvent==TIMER_REC )
	{
		// 回调到函数中 begin
		uWrite1 = uWrite2 = 0;
		m_dsIn.BeginGetBufDataIn(&lpDataPtr1,&dwDataBytes1,&lpDataPtr2,&dwDataBytes2);
		if ( dwDataBytes1>0 ) jmemcpy(lpOut,lpDataPtr1,dwDataBytes1);
		if ( dwDataBytes2>0 ) jmemcpy(lpOut+dwDataBytes1,lpDataPtr2,dwDataBytes2);
		m_dsIn.EndGetBufDataIn(lpDataPtr1,dwDataBytes1,lpDataPtr2,dwDataBytes2);

	//	JTRACE("CNVTestDlg::OnTimer dwDataBytes1=%u,dwDataBytes2=%u\r\n",dwDataBytes1,dwDataBytes2);
		if ( dwDataBytes1>0 ) m_dsOut.SetBufDataOut(lpOut,dwDataBytes1);
		if ( dwDataBytes2>0 ) m_dsOut.SetBufDataOut(lpOut+dwDataBytes1,dwDataBytes2);
		
		if ( m_lTalk )
		{
			if ( dwDataBytes1>0 ) m_jnvMng.TalkSend(m_lTalk,lpOut,dwDataBytes1);
			if ( dwDataBytes2>0 ) m_jnvMng.TalkSend(m_lTalk,lpOut+dwDataBytes1,dwDataBytes2);
		}
		// 回调到函数中 end
	}
	
	CDialog::OnTimer(nIDEvent);
}


void CNVTestDlg::OnBtGetServerList() 
{
	// TODO: Add your control notification handler code here
	m_jnvMng.GetServerList(".\\ServerList2.xml", 1);
}
