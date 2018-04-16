#ifndef	__PROTOCOL_NV_H__
#define	__PROTOCOL_NV_H__

#include "j_vs_ver.h"

// stl support begin
#if (defined _WIN32) || (defined _WIN64)
	#pragma warning(disable:4100)
	#pragma warning(disable:4189)
	#pragma warning(disable:4541)
	#pragma warning(disable:4786)
#endif
#include <list>
using namespace std;
// stl support end

#ifndef _WIN32
# define DEF_PRAGMA_PACKED					__attribute__((packed))
#else
# define DEF_PRAGMA_PACKED	
#pragma warning(disable:4200)				
#endif

#define _SINGLE_SERVER_					//�ɵ�������ģʽ

#define JNET_BREATH_SPAN		2			// �������,2��
#define	JNET_MAX_CHN			4			// ���ͨ����
#define	JNET_MAX_STREAM			128			// �������
#define MAX_SERVER_NUM			128			// ����������

#define JNET_IP_LEN				128			// �豸IP
#define JNET_NAME_LEN			32			// �û���
#define JNET_PWD_LEN			32			// ����

#ifdef _SINGLE_SERVER_
#define JNET_DEV_ID_LEN			8			// �豸ID��
#else
#define JNET_DEV_ID_LEN			24			// �豸ID��(chg by wxf 20130829)
#endif

#define JNET_TIME_STR_LEN		24			// ʱ���ַ����ĳ�,��"2011-01-01 12:00:00"
#define JNET_REC_PATH_LEN		40			// ¼��·����
#define JNET_FILE_PATH_LEN		256			// ¼��·����
#define	JNET_SERVER_ID_LEN		32			// ������ID��

// Э������
typedef enum
{
	eJVNProNull					= 0,		// ����
	eJVNProBreath				= 201,		// ����

	eJVNProLoginReq				= 101,		// �û���¼
	eJVNProLoginRet				= 102,		// �û���¼����
	eJVNProLogoutReq			= 103,		// �û��ǳ�

	eJVNProGetDevGPSReq			= 401,		// ����������
	eJVNProGetGPSInfoRet		= 402,		// ��������������
	eJVNProGetGPSInfoData		= 403,		// �������·�(BF)
	eJVNProStopDevGPSReq		= 404,		// ȡ������������

	eJVNProDownRecReq			= 462,		// ����¼���ļ�
	eJVNProDownRecRet			= 463,		// ����¼���ļ�Ӧ��
	eJVNProDownRecData			= 464,		// ¼���ļ����ص�

	eJVNProReplayOpenReq		= 480,		// �豸¼��ʱ��ط�
	eJVNProReplayAnswerRet		= 481,		// ��ӦӦ��
	eJVNProReplayCtrlReq		= 482,		// �طſ���

	eJVNProGetDevListReq		= 501,		// �����豸�б�
	eJVNProGetDevListRet		= 502,		// �豸�б���
	eJVNProGetDevListData		= 503,		// �豸�ļ�����

	eJVNProAlarmDetail			= 600,		// �����·�(��ϸ����)
	eJVNProGetDevAlarmReq		= 601,		// ���ı���
	eJVNProGetAlarmRet			= 602,		// ���ı�������
	eJVNProAlarmSummary			= 603,		// �����·�
	eJVNProStopDevAlarmReq		= 604,		// ȡ�����ı���(CC->S)
//	CS_DEALAL					= 605,		// ���������(CC->S)
	eJVNProRemoveAlarmRet		= 606,		// ����(S->CC)
	eJVNProGetAllGPSReq			= 607,		// ���������豸gps(para1:0:ȡ��,1:����	para2:gpsЭ������)
	eJVNProGetAllAlarmReq		= 608,		// ���������豸����(para1:0:ȡ��,1:����)


	eJVNProDatabaseDataTrans	= 693,		//���ݿ��ѯ��¼�ļ�����
	eJVNProRealStreamOpenReq	= 701,		// ������Ƶ
//	CS_REQVSR					= 702,		// ������Ƶ����
	eJVNProRealStreamData		= 703,		// �·���Ƶ��
	eJVNProRealStreamCloseReq	= 704,		// ȡ��������Ƶ
	eJVNProRealStreamErrRet		= 705,		// ������Ƶ�豸���صĴ�����
	eJVNProGetIFrameReq 		= 706,		// I֡����[���أ�s_VMS_Client2Server_Protocol_Common]

	eJVNProTalkReq				= 710,		// �Խ�����
	eJVNProTalkRet				= 711,		// �Խ�Ӧ��
	eJVNProTalkTransData		= 712,		// �Խ���������
	eJVNProTalkErr				= 713,		// �Խ��쳣

	eJVNProPTZCtrlReq			= 741,		// ������̨
	eJVNProPTZCtrlRet			= 742,		// ������̨Ӧ��

	eJVNProGetRecListReq		= 911,		// ¼���ѯ(��������)
	eJVNProGetRecListRet		= 912,		// ��ѯӦ��(����������
	eJVNProGetRecListData		= 913,		// ¼���ѯ�ļ����ݴ���
	//��������������----------------------------------------------------------
	eJVNProGetServerListReq		=	1200,	//����������б�
	eJVNProGetServerListRet		=	1201,	//����������б�Ӧ��

	eJVNProMax					= 65535,	// �߽�ֵ
}eJVNProtocolCmd;

// ��������� begin
typedef enum
{
	JNETErrSuccess				= 0,		// �ɹ�
	JNETErrUnInit				= -1,		// δ��ʼ��
	JNETErrHandle				= -2,		// ���������
	JNETErrParam				= -3,		// ��������
	JNETErrBuffSize				= -4,		// ������
	JNETErrNoMem				= -5,		// �ڴ治��
	JNETErrRecv					= -6,		// ���մ���
	JNETErrSend					= -7,		// ���ʹ���
	JNETErrOperate				= -8,		// ��������
	JNETErrCreateFile			= -9,		// �����ļ�����
	JNETErrNoFreePort			= -100,		// û�п���ͨ��
	JNETErrProtocol				= -101,		// Э�����
	JNETErrXMLFormat			= -102,		// �����XML����
	JNETErrNotSupport			= -103,		// ��֧�ֵĲ���
	JNETErrGetParam				= -104,		// ��ȡ��������
	JNETErrSetParam				= -105,		// ���ò�������
	JNETErrOpenFile				= -106,		// ���ļ�����
	JNETErrUpgOpen				= -107,		// ��������
}JNETErr;
// ��������� end

// ���������� begin
typedef enum
{
	AVPStreamUndef		= 0,			// δ����
	AVPStreamFile		= 1,			// �ļ���
	AVPStreamReal		= 2,			// ʵʱ��
	AVPStreamPlayback	= 3,			// �ط���
}AVPStreamType;
// ���������� end

// ���岥��״̬ begin
typedef enum
{
	AVPPlayStatusStop	= 0,			// ֹͣ
	AVPPlayStatusRun	= 1,			// ����
	AVPPlayStatusIdle	= 2,			// ����
}AVPPlayStatus;
// ���岥��״̬ end

// ���岥���ٶ� begin
typedef enum
{
	AVPPlaySpeed_1_16X	= 0x1,			// ��16����
	AVPPlaySpeed_1_8X	= 0x2,			// ��8����
	AVPPlaySpeed_1_4X	= 0x3,			// ��4����
	AVPPlaySpeed_1_2X	= 0x4,			// ��2����
	AVPPlaySpeed_1X		= 0x5,			// �����ٶ�
	AVPPlaySpeed_2X		= 0x6,			// ��2����
	AVPPlaySpeed_4X		= 0x7,			// ��4����
	AVPPlaySpeed_8X		= 0x8,			// ��8����
	AVPPlaySpeed_16X	= 0x9,			// ��16����
	AVPPlaySpeed_32X	= 0x10,			// ��32����
	AVPPlaySpeed_0_1X	= 0x20,			// ��2������
	AVPPlaySpeed_0_2X	= 0x21,			// ��2������
	AVPPlaySpeed_0_4X	= 0x22,			// ��4������
	AVPPlaySpeed_0_8X	= 0x23,			// ��8������
	AVPPlaySpeed_0_16X	= 0x24,			// ��16������
	AVPPlaySpeed_0_32X	= 0x25,			// ��32������
}AVPPlaySpeed;
// ���岥���ٶ� end

// ����ص����� begin
typedef enum
{
	AVPDecCBTypeNone	= 0,			// ��
	AVPDecCBTypeAudio16	= 1,			// ��Ƶ.������16khz,������,ÿ��������16λ��ʾ
	AVPDecCBTypeRGB32	= 2,			// ��Ƶ.ÿ����4�ֽ�,���з�ʽ��λͼ����,"BGR0",��һ������λ��ͼ�����½� 
	AVPDecCBTypeYV12	= 3,			// ��Ƶ,yv12��ʽ.����˳��"Y0Y1...","V0V1...","U0U1..."
	AVPDecCBTypeUYVY	= 4,			// ��Ƶ,uyvy��ʽ.����˳��"U0Y0V0Y1U2Y2V2Y3......",��һ������λ��ͼ�����Ͻ�
	AVPDecCBTypeYUV420	= 5,			// ��Ƶ,YUV420��ʽ.����˳��"Y0Y1...","U0U1...","V0V1..."
	AVPDecCBTypeYUYV	= 6,			// ��Ƶ,yuy2��yuyv��ʽ.����˳��"Y0 U0 Y1 V0 Y2 U2 Y3 V2 ... ... ",��һ������λ��ͼ�����Ͻ�.
	AVPDecCBTypeRGB24	= 7,			// ��Ƶ.ÿ����3�ֽ�,���з�ʽ��λͼ����,"BGR",��һ������λ��ͼ�����½� 
	AVPDecCBTypeRGB565	= 8,			// ��Ƶ.ÿ����3�ֽ�,���з�ʽ��λͼ����,"BGR",��һ������λ��ͼ�����½� 
}AVPDecCBType;
// ����ص����� end

// ����ص��������ò��� begin
typedef struct stDecFrameCB
{
	int			iPort;					// ����ͨ����
	LPBYTE		lpBuf;					// ����������
	long		lSize;					// ��������ݳ�
	long		lWidth;					// ��
	long		lHeight;				// ��
	long		lStamp;					// ֡ʱ��
	long		lType;					// AVPDecCBType
	long		lParam;					// �û��Զ������
	DWORD		dwRes0;					// ��������0
	DWORD		dwRes1;					// ��������1
	DWORD		dwRes2;					// ��������2
	DWORD		dwRes3;					// ��������3
}* PSTDecFrameCB;
// ����ص��������ò��� end

// ����˿ڻص���Ϣ begin
typedef enum 
{
	AVPPlayCBUnknow		= 0,			// δ֪��Ϣ
	AVPPlayCBError,						// ֪ͨ ���� ��Ϣ,����ֵ�޾�������
	AVPPlayCBStart,						// ֪ͨ ��ʼ���� ��Ϣ,����ֵ�޾�������,lParam=�ļ��Ŀ�ʼʱ��
	AVPPlayCBEnd,						// �Ѿ����õ���Ϣ
	AVPPlayCBFrame,						// ֪ͨ ֡ʱ�䷢���仯(��) ��Ϣ,����ֵ�޾�������,lParam=��ǰʱ��
	AVPPlayCBStop,						// ֪ͨ �������� ��Ϣ,����ֵ�޾�������,lParam=��
	AVPPlayCBPause,						// ֪ͨ �������� ��Ϣ,����ֵ�޾�������,lParam=�ļ��Ѳ���ʱ��
	AVPPlayCBContinue,					// ֪ͨ �������� ��Ϣ,����ֵ�޾�������,lParam=�ļ��Ѳ���ʱ��
	AVPPlayCBSpeed,						// ֪ͨ �����ٶ� ��Ϣ,����ֵ�޾�������,lParam=��ǰ�����ٶ�
}AVPPlayCBMsg;
// ����˿ڻص���Ϣ end

// �����¼����� begin
typedef enum
{
	JNET_EET_UNKNOWN			= -1,		// δ����
											
	JNET_EET_LOGINING			= 0,		// ���ڵ�¼
	JNET_EET_LOGIN_OK			= 1,		// ��¼�ɹ�
	JNET_EET_LOGIN_ERROR		= 2,		// ��¼ʧ��
	JNET_EET_LOGOUT				= 3,		// �ǳ�
	JNET_EET_STREAM_OPENING		= 4,		// �����ڴ�,����� eJNVStreamType
	JNET_EET_STREAM_OK			= 5,		// ���򿪳ɹ�,����� eJNVStreamType
	JNET_EET_STREAM_ERROR		= 6,		// ����ʧ��,����� eJNVStreamType
	JNET_EET_STREAM_CLOSE		= 7,		// ���ر�
	JNET_EET_PB_OPENING			= 8,		// �ط������ڴ�
	JNET_EET_PB_OK				= 9,		// �ط����򿪳ɹ�
	JNET_EET_PB_ERROR			= 10,		// �ط�����ʧ��
	JNET_EET_PB_CLOSE			= 11,		// �ط����ر�
	JNET_EET_TALK_OPENING		= 12,		// �Խ����ڴ�
	JNET_EET_TALK_OK			= 13,		// �Խ��򿪳ɹ�
	JNET_EET_TALK_ERROR			= 14,		// �Խ���ʧ��
	JNET_EET_TALK_CLOSE			= 15,		// �Խ��ر�
	JNET_EET_UPG_START			= 16,		// ������ʼ
	JNET_EET_UPG_OK				= 17,		// �����ɹ�
	JNET_EET_UPG_ERR			= 18,		// ����ʧ��
	JNET_EET_CFG_SET			= 100,		// ��ȡ����
	JNET_EET_CFG_GET			= 101,		// ��������
	JNET_EET_PTZ_CTRL			= 102,		// ��̨����
	JNET_EET_DEV_CTRL			= 103,		// �豸����
	JNET_EET_STREAM_LOST		= 104,		// ����ʧ
	JNET_EET_EVENT_NOTIFY		= 201,		// �����¼�
	JNET_EET_EVENT_DEV_LIST		= 301,		// ��ȡ�豸�б�,iDataType=eJNVFileType
	JNET_EET_EVENT_REC_LIST		= 302,		// ��ȡ¼���б�,iDataType=eJNVFileType
	JNET_EET_EVENT_REC_DOWN		= 303,		// ¼������,iDataType=eJNVFileType
	JNET_EET_EVENT_TRANS_PERCENT= 304,		// �����ٷݱ�,iDataType=eJNVFileType
	JNET_EET_EVENT_SERVER_LIST	= 305,		// ��ȡ�������б�,iDataType=eJNVFileType
	JNET_EET_EVENT_GPS_INFO		= 400,		// �·�GPS��Ϣ,pEventData=PSTJNVGPSAll
}eJNetEvent;
// �����¼����� end

// ������
typedef enum
{
	eJNVStreamUnknown			= 0,		// δ֪��
	eJNVStreamReal				= 1,		// ʵʱ��
	eJNVStreamReplay			= 2,		// ¼����
}eJNVStreamType;

// �¼�֪ͨ����ļ�����
typedef enum 
{
	eJNVFileErr			= -1,				// �ļ�ʧ��
	eJNVFileUndef		= 0,				// δ֪����
	eJNVFileDevList		= 1,				// �豸�б��ļ�
	eJNVFileRecList		= 2,				// ¼���ѯ�ļ�
	eJNVFileRecDown		= 3,				// ¼������
	eJNVFileServerList	= 4,				// �������б��ļ�
}eJNVFileType;

// ����֡����
typedef enum
{
	JNET_EMT_I_FRAME			= 0x00002001,//��֡
	JNET_EMT_P_FRAME			= 0x00002002,//��֡
	JNET_EMT_A_FRAME			= 0x00002003,
	JNET_EMT_PICTURE			= 0x00002004,
	JNET_EMT_MIC_DATA			= 0x00002005,
	JNET_EMT_FILE_DATA			= 0x00002006,
	JNET_EMT_TRANSPARENT		= 0x00002007,
	JNET_EMT_GPS_DATA			= 0x00002008,
	JNET_EMT_HEAD_FRAME			= 0x00002009,
}eJNetMediaType; 

// ����������
typedef enum
{
	eNVStreamPayloadInvalidFrame	= 0,			// ��Ч֡
	eNVStreamPayloadHeaderFrame		= 0x68643030,	// ͷ֡('00hd')
	eNVStreamPayloadVideoIFrame		= 0x62643030,	// ��Ƶ��֡('00db') I֡
	eNVStreamPayloadVideoVFrame		= 0x63643030,	// ��Ƶ��֡('00dc')	P֡
	eNVStreamPayloadAudioFrame		= 0x62773030,	// ��Ƶ֡('00wb')
	eNVStreamPayloadGPSFrame		= 0x73706730,	// GPS֡('0gps')
	eNVStreamPayloadAlarmFrame		= 0x6d613030,	// ����֡('00am')
}eNVStreamPayload;

// �طſ�������
typedef enum
{
	eNVReplayNULL					= 0x00,			// ��Ч
	eNVReplayFastForward			= 0x01,			// ���(1~4��������Ч)
	eNVReplayFastBack				= 0x02,			// ����(1~4��������Ч)
	eNVReplayPosition				= 0x03,			// ��λ
	eNVReplayNormal					= 0x04,			// ����
	eNVReplayPause					= 0x05,			// ��ͣ
}eNVReplayType;

// ��̨��������
typedef enum
{
	eNVPtzActionNull				= 0x00,			// ��Ч
	eNVPtzActionUp					= 0x01,			// ��
	eNVPtzActionDown				= 0x02,			// ��
	eNVPtzActionLeft				= 0x03,			// ��
	eNVPtzActionRight				= 0x04,			// ��
	eNVPtzActionLeftUp				= 0x05,			// ����
	eNVPtzActionRightUp				= 0x06,			// ����
	eNVPtzActionLeftDown			= 0x07,			// ����
	eNVPtzActionRigthDown			= 0x08,			// ����
	eNVPtzActionZoomIn				= 0x09,			// �佹��(��ͷ��Զ)
	eNVPtzActionZoomOut				= 0x0a,			// �佹С(��ͷ����)
	eNVPtzActionFocusNear			= 0x0b,			// �۽�С
	eNVPtzActionFocusFar			= 0x0c,			// �۽���
	eNVPtzActionIrisOpen			= 0x0D,			// ��Ȧ����
	eNVPtzActionIrisClose			= 0x0E,			// ��Ȧ��С	
	eNVPtzActionRunCruise			= 0x0F,			// ��ʼ�켣
	eNVPtzActionRunSeq				= 0x10,			// ��ʼѲ�� 
	eNVPtzActionStopSeq				= 0x11,			// ֹͣѲ��
	eNVPtzActionSetPreset			= 0x12,			// ����Ԥ�õ� 
	eNVPtzActionClearPreset			= 0x13,			// ���Ԥ�õ� 
	eNVPtzActionGotoPreset			= 0x14,			// ת��Ԥ�õ�
	eNVPtzActionLightOpen			= 0x15,			// ��ͨ�ƹ��Դ
	eNVPtzActionWiperOpen			= 0x16,			// ��ͨ��ˢ����
	eNVPtzActionFanOpen				= 0x17,			// ��ͨ���ȿ���
	eNVPtzActionHeaterOpen			= 0x18,			// ��ͨ����������
	eNVPtzActionAuxOpen				= 0x19,			// ��ͨ�����豸����
	eNVPtzActionStop				= 0xFF,			// ֹͣ
}eNVPtzActionType;

// �������Ͷ��� begin
typedef enum
{
	eJNVAlarm0x01		= 0x01,		// ����
	eJNVAlarm0x02		= 0x02,		// ��������
	eJNVAlarm0x04		= 0x04,		// ���봥��
	eJNVAlarm0x08		= 0x08,		// �ƶ����
	eJNVAlarm0x10		= 0x10,		// ��Ƶ��ʧ

	eJNVAlarm0x11		= 0x11,		// ���ٱ���
	eJNVAlarm0x12		= 0x12,		// ��������

	eJNVAlarm0x13		= 0x13,		// Υ��ͣ������
	eJNVAlarm0x14		= 0x14,		// Υ��ͣ������

	eJNVAlarm0x15		= 0x15,		// gps�洢���̿ռ䲻��
	eJNVAlarm0x16		= 0x16,		// gps�洢ֹͣ�洢

	eJNVAlarm0x17		= 0x17,		// ���ٳ���
	eJNVAlarm0x18		= 0x18,		// ���ٳ�������
	eJNVAlarm0x19		= 0x19,		// ���ٵ��ٱ���
	eJNVAlarm0x1A		= 0x1A,		// ���ٵ�������
 
	eJNVAlarm0x1B		= 0x1B,		// ��������
	eJNVAlarm0x1C		= 0x1C,		// ������������
	eJNVAlarm0x1D		= 0x1D,		// ��������
	eJNVAlarm0x1E		= 0x1E,		// ������������
 

	eJNVAlarm0x20		= 0x20,		// GPSģ���쳣��
	eJNVAlarm0x40		= 0x40,		// GSensor������
	eJNVAlarm0x0x80		= 0x80,		// �洢����1�쳣��

	eJNVAlarm0x100		= 0x100,	// �洢����2�쳣��
	eJNVAlarm0x200		= 0x200,	// ϵͳ�쳣��

	eJNVAlarm0x1000		= 0x1000,	// �豸����
	eJNVAlarm0x2000		= 0x2000,	// �豸����
	eJNVAlarm0x4000		= 0x4000,	// �豸����WIFI����
	eJNVAlarm0x8000		= 0x8000,	// �뿪����WIFI����

	eJNVAlarm0x10000	= 0x10000,	// ���
	eJNVAlarm0x20000	= 0x20000,	// Ϩ��

	eJNVAlarm0x20001	= 0x20001,	// ǰ�Ŵ�
	eJNVAlarm0x20002	= 0x20002,	// ǰ�Źر�
	eJNVAlarm0x20004	= 0x20004,	// ���Ŵ�
	eJNVAlarm0x20008	= 0x20008,	// ���Źر�
	eJNVAlarm0x20010	= 0x20010,	// ���Ŵ�
	eJNVAlarm0x20020	= 0x20020,	// ���Źر�
	eJNVAlarm0x20040	= 0x20040,	// ��վδͣ��
	eJNVAlarm0x20080	= 0x20080,	// �����г�
	eJNVAlarm0x20100	= 0x20100,	// ��վδ������
	eJNVAlarm0x20200	= 0x20200,	// ��;������
	eJNVAlarm0x20400	= 0x20400,	// �����¶ȸ�
	eJNVAlarm0x20800	= 0x20800,	// �����¶ȵ�
	eJNVAlarm0x21000	= 0x21000,	// ��վ�澯
	eJNVAlarm0x22000	= 0x22000,	// ��������
	eJNVAlarm0x24000	= 0x24000,	// ����ɲ��
	eJNVAlarm0x28000	= 0x28000,	// Ʊ���ű���
	eJNVAlarm0x30001	= 0x30001,	// ��������
	eJNVAlarm0x30002	= 0x30002,	// ƫ����·
	eJNVAlarm0x30004	= 0x30004,	// ��������
	eJNVAlarm0x30008	= 0x30008,	// �������ر�
	eJNVAlarm0x30010	= 0x30010,	// �������쳣
	eJNVAlarm0x30020	= 0x30020,	// ����1����
	eJNVAlarm0x30040	= 0x30040,	// ����2����
	eJNVAlarm0x30080	= 0x30080,	// ����3����
	eJNVAlarm0x30100	= 0x30100,	// ����4����
	eJNVAlarm0x30200	= 0x30200,	// ����5����
	eJNVAlarm0x30400	= 0x30400,	// ����6����
	eJNVAlarm0x30800	= 0x30800,	// ����7����
	eJNVAlarm0x31000	= 0x31000,	// ����8����
	eJNVAlarm0x32000	= 0x32000,	// �Ƿ�������
	eJNVAlarm0x34000	= 0x34000,	// �����ؿͳ���

	eJNVAlarm0x40000	= 0x40000,	// Խ��
	eJNVAlarm0x80000	= 0x80000,	// Խ������
	eJNVAlarm0x100000	= 0x100000,	// ��ʱͣ��
	eJNVAlarm0x200000	= 0x200000,	// ��ʱͣ������
	eJNVAlarm0x400000	= 0x400000,	// ƣ�ͼ�ʻ
	eJNVAlarm0x800000	= 0x800000,	// ƣ�ͼ�ʻ����
}eJNVAlarm;
// �������Ͷ��� end

//�ͻ�������
typedef enum e_NV_ClientType
{
		E_NV_ClientType_MvsClient				=	0x00,		//MVS_Client�ͻ���
		E_NV_ClientType_AndriodClient			=	0x01,		//android�ֻ��ƶ��ͻ���
		E_NV_ClientType_IPhoneClient			=	0x02,		//iphone�ֻ��ƶ��ͻ���
		E_NV_ClientType_WinPhoneClient			=	0x03,		//winphone�ֻ��ƶ��ͻ���
		E_NV_ClientType_DbTool					=	0x04,		//DB_TOOL����
		E_NV_ClientType_DeviceConfigTool		=	0x05,		//�豸�������ù���
		E_NV_ClientType_AutoDownloadTool		=	0x06,		//�Զ����ع���
		E_NV_ClientType_ReportStationTool		=	0x07,		//��վ����
		
		E_NV_ClientType_Device					=	0xfd,		//�豸ͨ�ſͻ���
		E_NV_ClientType_DebugTool				=	0xfe,		//���Թ���
		E_NV_ClientType_Other					=	0xff,		//��������
}e_NV_ClientType;

typedef int (__stdcall fcbJNVEvent)(LPCTSTR lpszEventStr,void* pUserParam);															// �¼��ص�����
typedef int (__stdcall fcbJStream)(long lHandle, LPBYTE pBuff,DWORD dwRevLen,void* pUserParam);										// ���յ���Ϣ�Ļص�����
typedef int (__stdcall fcbJEvent)(long lHandle, eJNetEvent eType,int iDataType,void* pEventData,int iDataLen,void* pUserParam);		// �¼��ص�����
typedef int (__stdcall fcbDecFrame)(PSTDecFrameCB pstDecFrameCB);																	// ����ص�����,0��ʾ��������,1��ʾ���ܼ�������
typedef int (__stdcall fcbPlay)(AVPPlayCBMsg eMsg,int iPlayerID,long lpUserParam,__int64 lParam1,__int64 lParam2,DWORD dwRes);		// ���Żص�����

// ���������İ�ͷ begin
typedef struct stJNVProHeadMin
{
	int		nType;			// ���� (�μ�Э������_eVMSProtocolType, ����Ӧ��ͬ�ĸ��ӽṹ)
	int		nLength;		// ���ݳ��� ��������+���ӽṹ)
	static int		s_iSize;
}* PSTJNVProHeadMin;
// ���������İ�ͷ end

// ��������İ�ͷ begin
typedef struct stJNVProHead
{
	int		nType;			// ���� (�μ�Э������_eVMSProtocolType, ����Ӧ��ͬ�ĸ��ӽṹ)
	int		nLength;		// ���ݳ��� ��������+���ӽṹ)
	int		nQueue;			// ����
	int		nParameter1;	// ����1
	int		nParameter2;	// ����2
	stJNVProHead()
	{
		stZero();
	}
	void stZero()
	{
		nType		= 0;	// ���� (�μ�Э������_eVMSProtocolType, ����Ӧ��ͬ�ĸ��ӽṹ)
		nLength		= 0;	// ���ݳ��� ��������+���ӽṹ)
		nQueue		= 0;	// ����
		nParameter1	= 0;	// ����1
		nParameter2	= 0;	// ����2
	}
	static DWORD GetSeq()
	{
		DWORD	dwSeq	= s_dwSeq;
		s_dwSeq++;
		return dwSeq;
	}
	static int		s_iSize;
	static DWORD	s_dwSeq;
}* PSTJNVProHead;
// ��������İ�ͷ end

// ���ڻ�ȡ�ļ���Ϣ�����ݽṹ begin
typedef struct stJNVProFileData
{
	int		nTransID;			// ����ID
	char	pFileData[0];		// ���������
}* PSTJNVProFileData;
// ���ڻ�ȡ�ļ���Ϣ�����ݽṹ end

// �û���½Ӧ��
typedef struct stJNVC2SLoginRet
{
	int			nUserID;
	int			nUserType;
	int			nSessionID;
}* PSTJNVC2SLoginRet;

//�û���½����
typedef struct stJNVC2SLoginReq
{
	char		szUserName[JNET_NAME_LEN];
	char		szUserPassword[JNET_PWD_LEN];
}* PSTJNVC2SLoginReq;

//�û���½����
typedef struct stJNVC2SLoginReqEx
{
	char		szUserName[JNET_NAME_LEN*2];
	char		szUserPassword[JNET_PWD_LEN*2];
	int			nClientType;			//�ͻ�������
	char		szClientVersion[12];	//�ͻ��˰汾�ţ���ʽΪ1.1.0.1
}* PSTJNVC2SLoginReqEx;


// �����ı��� begin
typedef struct stJNVProAlarmStart
{
	stJNVProHead	stHead;		// ��Ϣͷ
	char			szDeviceID[JNET_DEV_ID_LEN];	// �豸ID
}* PSTJNVProAlarmStart;
// �����ı��� end

// ����ȡ�����ı��� begin
typedef struct stJNVProAlarmStop
{
	stJNVProHead	stHead;		// ��Ϣͷ
	char			szDeviceID[JNET_DEV_ID_LEN];	// �豸ID
}* PSTJNVProAlarmStop;
// ����ȡ�����ı��� end

// �����ṹ�� begin
typedef struct stAlarmData
{
	char szDeviceID[JNET_DEV_ID_LEN];				// �豸ID
	char szTime[JNET_TIME_STR_LEN];					// ����ʱ��
	int  nChannel;									// ����ͨ����ͨ�������㣩,�� param1
	int  nAlarmType;								// ��������,�� param2
}* PSTAlarmData;
// �����ṹ�� end

// ������GPS begin
typedef struct stJNVProGPSStart
{
	stJNVProHead	stHead;		// ��Ϣͷ
	char			szDeviceID[JNET_DEV_ID_LEN];	// �豸ID
}* PSTJNVProGPSStart;
// ������GPS end

// ����ȡ������GPS begin
typedef struct stJNVProGPSStop
{
	stJNVProHead	stHead;		// ��Ϣͷ
	char			szDeviceID[JNET_DEV_ID_LEN];	// �豸ID
}* PSTJNVProGPSStop;
// ����ȡ������GPS end

// GPS���� -- ��Э�� begin
// GSensor����
typedef struct stJNVGPSGSensor
{
	bool			bValid;				// ��Ϣ��Ч��
	float			fAccelate_X;		// x������ٶ�(��λ:g)
	float			fAccelate_Y;		// y������ٶ�(��λ:g)
	float			fAccelate_Z;		// z������ٶ�(��λ:g)
}* PSTJNVGPSGSensor;
// GyroSensor
typedef struct stJNVGPSGyroSensor
{
	bool			bValid;				// ��Ϣ��Ч��
	float			fCorner;			// ת�Ǽ��ٶ�(��λ:g)
}* PSTJNVGPSGyroSensor;
// GPS״̬
typedef struct stJNVGPSStatus
{
	bool			bValid;				// ��Ϣ��Ч��
	bool			bKeyValid;			// ��Կ����Ч��
	bool			bBrakeValid;		// ɲ���ź���Ч��
	bool			bLeftValid;			// ��ת��Ч��
	bool			bRightValid;		// ��ת��Ч��
	bool			bGPSModuleValid;	// GPSģ����Ч��
}* PSTJNVGPSStatus;
// GPS������Ϣ
typedef struct stJNVGPSBaseInfo
{
	bool			bValid;				// ��Ϣ��Ч��
	double			dbLongitude;		// ����(>0:����,<0:����)
	double			dbLatitude;			// γ��(>0:��γ,<0:��γ)
	float			fDirect;			// ����(0~360��)
	float			fSpeed;				// �ٶ�(ǧ��/Сʱ,������Ч)
}* PSTJNVGPSBaseInfo;
// GPS��չ��Ϣ
typedef struct stJNVGPSExternInfo
{
	bool			bValid;				// ��Ϣ��Ч��
	short			usHigh;				// ���θ߶�
	char			ucSatelliteNumber;	// ��������
	float			fTemperature;		// �¶�(���϶�,>=-10000��Ч)
}* PSTJNVGPSExternInfo;
// GPS���������Ϣ
typedef struct stJNVGPSAll
{
	char				szDevID[JNET_DEV_ID_LEN];	// �豸ID
	stJNVGPSBaseInfo	stBaseInfo;		// ������Ϣ
	stJNVGPSStatus		stStatus;		// ״̬����
	stJNVGPSGSensor		stGSensor;		// G-Sensor����
	stJNVGPSGyroSensor	stGyroSensor;	// Gyro-Sensor����
	stJNVGPSExternInfo	stExternInfo;	// ��չ��Ϣ
	char				stTime[24];		// ʱ����Ϣ
}* PSTJNVGPSAll;
// GPS���� -- ��Э�� end

// �Խ���� begin
typedef struct stJNVTalkReqSingle
{
	stJNVProHead	stHead;									// ��Ϣͷ
	int				nWorkMode;								// ������ʽ(0:�Խ�,1:�㲥,2:PTT,3:����)
	short			usControl;								// ���Ʒ�ʽ(0:ȡ��,1:����)
	short			usChannel;								// ͨ��
	int				nDeviceNumber;							// �豸����
	int				nSessionID;								// �ỰID
	char			szDeviceID[JNET_DEV_ID_LEN];			// �豸ID
}* PSTJNVTalkReqSingle;
// �Խ���� end

// ¼����� begin
// ¼���ѯ
typedef struct stJNVRecQuery
{
	stJNVProHead		stHead;								// ��Ϣͷ
	char				szDeviceID[JNET_DEV_ID_LEN];		// �豸ID
	int					iCenter;							// ���Ļ����豸¼��(1,��ʾ����¼��;0,��ʾ�豸¼��)
	int					nStorage;							// �洢����(����)
	int					nType;								// ¼������
	char				szStartTime[JNET_TIME_STR_LEN];		// ��ʼʱ�䣬ʱ���ʽ�磺��-��-�� ʱ:��:��
	char				szStopTime[JNET_TIME_STR_LEN];		// ����ʱ��
	int					nChannelList;						// ͨ������б�(��λ��ʾͨ�������磺00001111����ʾ1-4ͨ��)
}* PSTJNVRecQuery;
//¼���¼
typedef struct stJNVRecSumInfo
{
	char				szFileName[JNET_REC_PATH_LEN];		// �ļ���
	char				szStartTime[JNET_TIME_STR_LEN];		// ��ʼʱ��(��ʽ:2010-11-12 21:12:12)
	char				szStopTime[JNET_TIME_STR_LEN];		// ����ʱ��
	unsigned int		uiFileSize;							// �ļ���С
	unsigned int		uiSeconds;							// ��¼������
	int					nChannelIndex;						// ͨ�����
	int					nRecordType;						// ¼������(�μ�e_NV_RecordType)
}* PSTJNVRecSumInfo;
// ¼������
typedef struct stJNVRecDown
{
	stJNVProHead		stHead;								// ��Ϣͷ
	char				szDeviceID[JNET_DEV_ID_LEN];		// �豸ID
	int					nSessionID;							// SessionID
	char				szDevPath[JNET_FILE_PATH_LEN];		// �豸�ļ�·��
	int					iStartPos;							// ��ʼλ��
}* PSTJNVRecDown;
// ¼����� end

// ʵʱ��Ƶ��� begin
// ����
typedef struct stJNVRealStreamStart
{
	stJNVProHead		stHead;								// ��Ϣͷ
	char				szDeviceID[JNET_DEV_ID_LEN];		// �豸ID
	int					nChannel;							// ͨ�����(1��ʼ)
	int					nStreamIndex;						// �������(����)
	int					nSessionID;							// Session
	int					nStreamType;						// ��������(����)
}* PSTJNVRealStreamStart;
// ����I֡
typedef struct stJNVRealStreamGetIFrame
{
	stJNVProHead		stHead;								// ��Ϣͷ
	char				szDeviceID[JNET_DEV_ID_LEN];		// �豸ID
	int					nChannel;							// ͨ�����(1��ʼ)
}* PSTJNVRealStreamGetIFrame;

// ͷ֡���ݽṹ
typedef struct stJNVStreamHeaderFrameInfo
{
	int					nWidth;			// ���
	int					nHeight;		// �߶�
	int					nAudioType;		// ��Ƶ��������
	int					nFrameRate;		// ֡��
//	char				pData[0];		// ֡����
}* PSTJNVStreamHeaderFrameInfo;


// ��Ƶ֡����
typedef struct stJNVStreamFrameVideoInfo
{
	__int64				tTime;			// ʱ���
//	char				pData[0];		// ��Ƶ����(104�ֽ�������)
}* PSTJNVStreamFrameVideoInfo;
// ��Ƶ֡����
typedef struct stJNVStreamFrameAudioInfo
{
	__int64				tTime;			// ʱ���
//	char				pData[0];		// ��Ƶ����(104�ֽ�������)
}* PSTJNVStreamFrameAudioInfo;
// ¼���ʽ
typedef struct stJNVVideoFramInfo
{
	DWORD				dwMagic;		// ħ��	// 0x204A4E56	// JNV
	long				dwFrameNo;		// ֡��
	int					nWidth;			// ���
	int					nHeight;		// �߶�
	int					nMediaType;		// ý������,�� eJNetMediaType
	int					nFrameRate;		// ֡��
	__int64				tTime;			// ʱ���
	DWORD				dwDataSize;		// ֡���ݳ�
//	char				pData[0];		// ֡����
}* PSTJNVVideoFramInfo;
// ʵʱ��Ƶ��� end

// �ط���Ƶ��� begin
// ��¼��ط�
typedef struct stJNVReplayStart
{
	stJNVProHead		stHead;								// ��Ϣͷ
	char				szDeviceID[JNET_DEV_ID_LEN];		// �豸ID
	bool				bCenter;							// ���Ļ����豸¼��
	int					nChannel;							// ͨ�����
	int					nSessionID;							// Session id
	char				szStartTime[JNET_TIME_STR_LEN];		// ��ʼʱ��
	char				szStopTime[JNET_TIME_STR_LEN];		// ����ʱ��
	int					nOnlyIFrame;						// �Ƿ�ֻҪI֡
	int					nRecordType;						// ¼������
	int					nStartPosition;						// ��ʼλ��
}* PSTJNVReplayStart;
// ¼��طſ���
typedef struct stJNVReplayCtrl
{
	stJNVProHead		stHead;								// ��Ϣͷ
	char				szDeviceID[JNET_DEV_ID_LEN];		// �豸ID
	int					nChannel;							// ͨ�����
	int					nControlType;						// ��������(�μ� eNVReplayType)	
	int					nValue;								// ����ֵ
}* PSTJNVReplayCtrl;
// �ط���Ƶ��� end

// ������̨ begin
typedef struct stJNVPtzCtrl
{
	stJNVProHead		stHead;								// ��Ϣͷ
	char				szDeviceID[JNET_DEV_ID_LEN];		// �豸ID
	int					nChannelIndex;						// ͨ�����
	int					nActionType;						// ��������(�μ� eNVPtzActionType)
	int					nReserved1;							// ����1(1:��x�����ٶ�ʱ��Ϊx�����ٶ�
															// 2:��x�����ٶ�,������y�����ٶ�,δy�����ٶ�
															// 3:��Ԥ��λʱ��ΪԤ��λ
															// 4:��������
	int					nReserved2;							// ����2(1:����1��x�����ٶ��ٶ�ʱ��,�˴���y�����ٶȵ�ʱ��,Ϊy�����ٶ�
															// 2:��������)
	int					nSessionID;												
}* PSTJNVPtzCtrl;
// ������̨ end




// ����������
typedef enum
{
	eJNVCenterServer	= 0,
	eJNVCmdServer		= 1,
	eJNVMediaServer		= 2,
}eJNVServerType;

// ����������
typedef struct stJNVServerInfo
{
	char				szServerId[JNET_SERVER_ID_LEN];		// ������ID
	char				szServerIP[20];						// ������IP
	int					nServerPort;						// ������port
	int					nServerType;						// ����������
	void*				lpCasTcp;							// ���������Ӷ���ָ��
}* PSTJNVServerInfo;
// ���������� end








// �ַ�����
inline char* StrNCpy(char* pTag,const char* pSrc,int iLen)
{
	strncpy_s(pTag,pSrc,iLen);
	pTag[iLen-1] = '\0';
	return pTag;
}
// ���÷���ͷ
inline int SetNetHdr(PSTJNVProHead pstNetHdr, DWORD dwPackLen, DWORD dwPackCmd, DWORD dwSeq)
{
	pstNetHdr->nType		= dwPackCmd;	// ���� (�μ�Э������_eVMSProtocolType, ����Ӧ��ͬ�ĸ��ӽṹ)
	pstNetHdr->nLength		= dwPackLen;	// ���ݳ��� ��������+���ӽṹ)
	pstNetHdr->nQueue		= dwSeq;		// ����
	pstNetHdr->nParameter1	= 0;			// ����1
	pstNetHdr->nParameter2	= 0;			// ����2
	return JNETErrSuccess;
}
inline int GetMin(int a,int b)
{
	if ( a>b ) return b;
	else return a;
}

#endif	// __PROTOCOL_NV_H__



