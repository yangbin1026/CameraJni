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

#define _SINGLE_SERVER_					//旧单服务器模式

#define JNET_BREATH_SPAN		2			// 呼吸间隔,2秒
#define	JNET_MAX_CHN			4			// 最大通道数
#define	JNET_MAX_STREAM			128			// 最大流数
#define MAX_SERVER_NUM			128			// 最大服务器数

#define JNET_IP_LEN				128			// 设备IP
#define JNET_NAME_LEN			32			// 用户名
#define JNET_PWD_LEN			32			// 密码

#ifdef _SINGLE_SERVER_
#define JNET_DEV_ID_LEN			8			// 设备ID长
#else
#define JNET_DEV_ID_LEN			24			// 设备ID长(chg by wxf 20130829)
#endif

#define JNET_TIME_STR_LEN		24			// 时间字符串的长,如"2011-01-01 12:00:00"
#define JNET_REC_PATH_LEN		40			// 录像路径长
#define JNET_FILE_PATH_LEN		256			// 录像路径长
#define	JNET_SERVER_ID_LEN		32			// 服务器ID长

// 协议类型
typedef enum
{
	eJVNProNull					= 0,		// 保留
	eJVNProBreath				= 201,		// 心跳

	eJVNProLoginReq				= 101,		// 用户登录
	eJVNProLoginRet				= 102,		// 用户登录返回
	eJVNProLogoutReq			= 103,		// 用户登出

	eJVNProGetDevGPSReq			= 401,		// 订阅坐标流
	eJVNProGetGPSInfoRet		= 402,		// 订阅坐标流返回
	eJVNProGetGPSInfoData		= 403,		// 坐标流下发(BF)
	eJVNProStopDevGPSReq		= 404,		// 取消订阅坐标流

	eJVNProDownRecReq			= 462,		// 下载录像文件
	eJVNProDownRecRet			= 463,		// 下载录像文件应答
	eJVNProDownRecData			= 464,		// 录像文件流回调

	eJVNProReplayOpenReq		= 480,		// 设备录像按时间回放
	eJVNProReplayAnswerRet		= 481,		// 对应应答
	eJVNProReplayCtrlReq		= 482,		// 回放控制

	eJVNProGetDevListReq		= 501,		// 请求设备列表
	eJVNProGetDevListRet		= 502,		// 设备列表返回
	eJVNProGetDevListData		= 503,		// 设备文件传输

	eJVNProAlarmDetail			= 600,		// 报警下发(详细数据)
	eJVNProGetDevAlarmReq		= 601,		// 订阅报警
	eJVNProGetAlarmRet			= 602,		// 订阅报警返回
	eJVNProAlarmSummary			= 603,		// 报警下发
	eJVNProStopDevAlarmReq		= 604,		// 取消订阅报警(CC->S)
//	CS_DEALAL					= 605,		// 处理服务器(CC->S)
	eJVNProRemoveAlarmRet		= 606,		// 消警(S->CC)
	eJVNProGetAllGPSReq			= 607,		// 订阅所有设备gps(para1:0:取消,1:订阅	para2:gps协议类型)
	eJVNProGetAllAlarmReq		= 608,		// 订阅所有设备报警(para1:0:取消,1:订阅)


	eJVNProDatabaseDataTrans	= 693,		//数据库查询记录文件传输
	eJVNProRealStreamOpenReq	= 701,		// 请求视频
//	CS_REQVSR					= 702,		// 请求视频返回
	eJVNProRealStreamData		= 703,		// 下发视频流
	eJVNProRealStreamCloseReq	= 704,		// 取消请求视频
	eJVNProRealStreamErrRet		= 705,		// 请求视频设备返回的错误码
	eJVNProGetIFrameReq 		= 706,		// I帧请求[负载：s_VMS_Client2Server_Protocol_Common]

	eJVNProTalkReq				= 710,		// 对讲请求
	eJVNProTalkRet				= 711,		// 对讲应答
	eJVNProTalkTransData		= 712,		// 对讲交互数据
	eJVNProTalkErr				= 713,		// 对讲异常

	eJVNProPTZCtrlReq			= 741,		// 控制云台
	eJVNProPTZCtrlRet			= 742,		// 控制云台应答

	eJVNProGetRecListReq		= 911,		// 录像查询(大数据量)
	eJVNProGetRecListRet		= 912,		// 查询应答(大数据量）
	eJVNProGetRecListData		= 913,		// 录像查询文件数据传输
	//级联服务器操作----------------------------------------------------------
	eJVNProGetServerListReq		=	1200,	//请求服务器列表
	eJVNProGetServerListRet		=	1201,	//请求服务器列表应答

	eJVNProMax					= 65535,	// 边界值
}eJVNProtocolCmd;

// 定义错误码 begin
typedef enum
{
	JNETErrSuccess				= 0,		// 成功
	JNETErrUnInit				= -1,		// 未初始化
	JNETErrHandle				= -2,		// 句柄不存在
	JNETErrParam				= -3,		// 参数错误
	JNETErrBuffSize				= -4,		// 缓存满
	JNETErrNoMem				= -5,		// 内存不足
	JNETErrRecv					= -6,		// 接收错误
	JNETErrSend					= -7,		// 发送错误
	JNETErrOperate				= -8,		// 操作错误
	JNETErrCreateFile			= -9,		// 创建文件错误
	JNETErrNoFreePort			= -100,		// 没有空闲通道
	JNETErrProtocol				= -101,		// 协议错误
	JNETErrXMLFormat			= -102,		// 错误的XML数据
	JNETErrNotSupport			= -103,		// 不支持的操作
	JNETErrGetParam				= -104,		// 获取参数错误
	JNETErrSetParam				= -105,		// 设置参数错误
	JNETErrOpenFile				= -106,		// 打开文件出错
	JNETErrUpgOpen				= -107,		// 升级出错
}JNETErr;
// 定义错误码 end

// 定义流类型 begin
typedef enum
{
	AVPStreamUndef		= 0,			// 未定义
	AVPStreamFile		= 1,			// 文件流
	AVPStreamReal		= 2,			// 实时流
	AVPStreamPlayback	= 3,			// 回放流
}AVPStreamType;
// 定义流类型 end

// 定义播放状态 begin
typedef enum
{
	AVPPlayStatusStop	= 0,			// 停止
	AVPPlayStatusRun	= 1,			// 运行
	AVPPlayStatusIdle	= 2,			// 闲置
}AVPPlayStatus;
// 定义播放状态 end

// 定义播放速度 begin
typedef enum
{
	AVPPlaySpeed_1_16X	= 0x1,			// 慢16倍速
	AVPPlaySpeed_1_8X	= 0x2,			// 慢8倍速
	AVPPlaySpeed_1_4X	= 0x3,			// 慢4倍速
	AVPPlaySpeed_1_2X	= 0x4,			// 慢2倍速
	AVPPlaySpeed_1X		= 0x5,			// 正常速度
	AVPPlaySpeed_2X		= 0x6,			// 快2倍速
	AVPPlaySpeed_4X		= 0x7,			// 快4倍速
	AVPPlaySpeed_8X		= 0x8,			// 快8倍速
	AVPPlaySpeed_16X	= 0x9,			// 快16倍速
	AVPPlaySpeed_32X	= 0x10,			// 快32倍速
	AVPPlaySpeed_0_1X	= 0x20,			// 快2倍速退
	AVPPlaySpeed_0_2X	= 0x21,			// 快2倍速退
	AVPPlaySpeed_0_4X	= 0x22,			// 快4倍速退
	AVPPlaySpeed_0_8X	= 0x23,			// 快8倍速退
	AVPPlaySpeed_0_16X	= 0x24,			// 快16倍速退
	AVPPlaySpeed_0_32X	= 0x25,			// 快32倍速退
}AVPPlaySpeed;
// 定义播放速度 end

// 定义回调类型 begin
typedef enum
{
	AVPDecCBTypeNone	= 0,			// 无
	AVPDecCBTypeAudio16	= 1,			// 音频.采样率16khz,单声道,每个采样点16位表示
	AVPDecCBTypeRGB32	= 2,			// 视频.每像素4字节,排列方式与位图相似,"BGR0",第一个像素位于图像左下角 
	AVPDecCBTypeYV12	= 3,			// 视频,yv12格式.排列顺序"Y0Y1...","V0V1...","U0U1..."
	AVPDecCBTypeUYVY	= 4,			// 视频,uyvy格式.排列顺序"U0Y0V0Y1U2Y2V2Y3......",第一个像素位于图像左上角
	AVPDecCBTypeYUV420	= 5,			// 视频,YUV420格式.排列顺序"Y0Y1...","U0U1...","V0V1..."
	AVPDecCBTypeYUYV	= 6,			// 视频,yuy2或yuyv格式.排列顺序"Y0 U0 Y1 V0 Y2 U2 Y3 V2 ... ... ",第一个像素位于图像左上角.
	AVPDecCBTypeRGB24	= 7,			// 视频.每像素3字节,排列方式与位图相似,"BGR",第一个像素位于图像左下角 
	AVPDecCBTypeRGB565	= 8,			// 视频.每像素3字节,排列方式与位图相似,"BGR",第一个像素位于图像左下角 
}AVPDecCBType;
// 定义回调类型 end

// 解码回调函数所用参数 begin
typedef struct stDecFrameCB
{
	int			iPort;					// 解码通道号
	LPBYTE		lpBuf;					// 解码后的数据
	long		lSize;					// 解码后数据长
	long		lWidth;					// 宽
	long		lHeight;				// 高
	long		lStamp;					// 帧时间
	long		lType;					// AVPDecCBType
	long		lParam;					// 用户自定义参数
	DWORD		dwRes0;					// 保留数据0
	DWORD		dwRes1;					// 保留数据1
	DWORD		dwRes2;					// 保留数据2
	DWORD		dwRes3;					// 保留数据3
}* PSTDecFrameCB;
// 解码回调函数所用参数 end

// 定义端口回调消息 begin
typedef enum 
{
	AVPPlayCBUnknow		= 0,			// 未知消息
	AVPPlayCBError,						// 通知 错误 消息,返回值无具体意义
	AVPPlayCBStart,						// 通知 开始播放 消息,返回值无具体意义,lParam=文件的开始时间
	AVPPlayCBEnd,						// 已经弃用的消息
	AVPPlayCBFrame,						// 通知 帧时间发生变化(秒) 消息,返回值无具体意义,lParam=当前时间
	AVPPlayCBStop,						// 通知 结束播放 消息,返回值无具体意义,lParam=空
	AVPPlayCBPause,						// 通知 继续播放 消息,返回值无具体意义,lParam=文件已播放时间
	AVPPlayCBContinue,					// 通知 继续播放 消息,返回值无具体意义,lParam=文件已播放时间
	AVPPlayCBSpeed,						// 通知 播放速度 消息,返回值无具体意义,lParam=当前播放速度
}AVPPlayCBMsg;
// 定义端口回调消息 end

// 定义事件类型 begin
typedef enum
{
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
	JNET_EET_EVENT_SERVER_LIST	= 305,		// 获取服务器列表,iDataType=eJNVFileType
	JNET_EET_EVENT_GPS_INFO		= 400,		// 下发GPS信息,pEventData=PSTJNVGPSAll
}eJNetEvent;
// 定义事件类型 end

// 流类型
typedef enum
{
	eJNVStreamUnknown			= 0,		// 未知流
	eJNVStreamReal				= 1,		// 实时流
	eJNVStreamReplay			= 2,		// 录像流
}eJNVStreamType;

// 事件通知里的文件类型
typedef enum 
{
	eJNVFileErr			= -1,				// 文件失败
	eJNVFileUndef		= 0,				// 未知类型
	eJNVFileDevList		= 1,				// 设备列表文件
	eJNVFileRecList		= 2,				// 录像查询文件
	eJNVFileRecDown		= 3,				// 录像下载
	eJNVFileServerList	= 4,				// 服务器列表文件
}eJNVFileType;

// 数据帧类型
typedef enum
{
	JNET_EMT_I_FRAME			= 0x00002001,//主帧
	JNET_EMT_P_FRAME			= 0x00002002,//虚帧
	JNET_EMT_A_FRAME			= 0x00002003,
	JNET_EMT_PICTURE			= 0x00002004,
	JNET_EMT_MIC_DATA			= 0x00002005,
	JNET_EMT_FILE_DATA			= 0x00002006,
	JNET_EMT_TRANSPARENT		= 0x00002007,
	JNET_EMT_GPS_DATA			= 0x00002008,
	JNET_EMT_HEAD_FRAME			= 0x00002009,
}eJNetMediaType; 

// 流负载类型
typedef enum
{
	eNVStreamPayloadInvalidFrame	= 0,			// 无效帧
	eNVStreamPayloadHeaderFrame		= 0x68643030,	// 头帧('00hd')
	eNVStreamPayloadVideoIFrame		= 0x62643030,	// 视频主帧('00db') I帧
	eNVStreamPayloadVideoVFrame		= 0x63643030,	// 视频虚帧('00dc')	P帧
	eNVStreamPayloadAudioFrame		= 0x62773030,	// 音频帧('00wb')
	eNVStreamPayloadGPSFrame		= 0x73706730,	// GPS帧('0gps')
	eNVStreamPayloadAlarmFrame		= 0x6d613030,	// 报警帧('00am')
}eNVStreamPayload;

// 回放控制命令
typedef enum
{
	eNVReplayNULL					= 0x00,			// 无效
	eNVReplayFastForward			= 0x01,			// 快进(1~4，其他无效)
	eNVReplayFastBack				= 0x02,			// 快退(1~4，其他无效)
	eNVReplayPosition				= 0x03,			// 定位
	eNVReplayNormal					= 0x04,			// 正常
	eNVReplayPause					= 0x05,			// 暂停
}eNVReplayType;

// 云台控制类型
typedef enum
{
	eNVPtzActionNull				= 0x00,			// 无效
	eNVPtzActionUp					= 0x01,			// 上
	eNVPtzActionDown				= 0x02,			// 下
	eNVPtzActionLeft				= 0x03,			// 左
	eNVPtzActionRight				= 0x04,			// 右
	eNVPtzActionLeftUp				= 0x05,			// 左上
	eNVPtzActionRightUp				= 0x06,			// 左下
	eNVPtzActionLeftDown			= 0x07,			// 左下
	eNVPtzActionRigthDown			= 0x08,			// 右下
	eNVPtzActionZoomIn				= 0x09,			// 变焦大(镜头拉远)
	eNVPtzActionZoomOut				= 0x0a,			// 变焦小(镜头拉近)
	eNVPtzActionFocusNear			= 0x0b,			// 聚焦小
	eNVPtzActionFocusFar			= 0x0c,			// 聚焦大
	eNVPtzActionIrisOpen			= 0x0D,			// 光圈扩大
	eNVPtzActionIrisClose			= 0x0E,			// 光圈缩小	
	eNVPtzActionRunCruise			= 0x0F,			// 开始轨迹
	eNVPtzActionRunSeq				= 0x10,			// 开始巡航 
	eNVPtzActionStopSeq				= 0x11,			// 停止巡航
	eNVPtzActionSetPreset			= 0x12,			// 设置预置点 
	eNVPtzActionClearPreset			= 0x13,			// 清除预置点 
	eNVPtzActionGotoPreset			= 0x14,			// 转到预置点
	eNVPtzActionLightOpen			= 0x15,			// 接通灯光电源
	eNVPtzActionWiperOpen			= 0x16,			// 接通雨刷开关
	eNVPtzActionFanOpen				= 0x17,			// 接通风扇开关
	eNVPtzActionHeaterOpen			= 0x18,			// 接通加热器开关
	eNVPtzActionAuxOpen				= 0x19,			// 接通辅助设备开关
	eNVPtzActionStop				= 0xFF,			// 停止
}eNVPtzActionType;

// 报警类型定义 begin
typedef enum
{
	eJNVAlarm0x01		= 0x01,		// 超速
	eJNVAlarm0x02		= 0x02,		// 超速消警
	eJNVAlarm0x04		= 0x04,		// 输入触发
	eJNVAlarm0x08		= 0x08,		// 移动侦测
	eJNVAlarm0x10		= 0x10,		// 视频丢失

	eJNVAlarm0x11		= 0x11,		// 低速报警
	eJNVAlarm0x12		= 0x12,		// 低速消警

	eJNVAlarm0x13		= 0x13,		// 违规停车报警
	eJNVAlarm0x14		= 0x14,		// 违规停车消警

	eJNVAlarm0x15		= 0x15,		// gps存储磁盘空间不足
	eJNVAlarm0x16		= 0x16,		// gps存储停止存储

	eJNVAlarm0x17		= 0x17,		// 高速超速
	eJNVAlarm0x18		= 0x18,		// 高速超速消警
	eJNVAlarm0x19		= 0x19,		// 高速低速报警
	eJNVAlarm0x1A		= 0x1A,		// 高速低速消警
 
	eJNVAlarm0x1B		= 0x1B,		// 国道超速
	eJNVAlarm0x1C		= 0x1C,		// 国道超速消警
	eJNVAlarm0x1D		= 0x1D,		// 国道低速
	eJNVAlarm0x1E		= 0x1E,		// 国道低速消警
 

	eJNVAlarm0x20		= 0x20,		// GPS模块异常。
	eJNVAlarm0x40		= 0x40,		// GSensor报警。
	eJNVAlarm0x0x80		= 0x80,		// 存储介质1异常。

	eJNVAlarm0x100		= 0x100,	// 存储介质2异常。
	eJNVAlarm0x200		= 0x200,	// 系统异常。

	eJNVAlarm0x1000		= 0x1000,	// 设备上线
	eJNVAlarm0x2000		= 0x2000,	// 设备离线
	eJNVAlarm0x4000		= 0x4000,	// 设备进入WIFI区域
	eJNVAlarm0x8000		= 0x8000,	// 离开进入WIFI区域

	eJNVAlarm0x10000	= 0x10000,	// 点火
	eJNVAlarm0x20000	= 0x20000,	// 熄火

	eJNVAlarm0x20001	= 0x20001,	// 前门打开
	eJNVAlarm0x20002	= 0x20002,	// 前门关闭
	eJNVAlarm0x20004	= 0x20004,	// 中门打开
	eJNVAlarm0x20008	= 0x20008,	// 中门关闭
	eJNVAlarm0x20010	= 0x20010,	// 后门打开
	eJNVAlarm0x20020	= 0x20020,	// 后门关闭
	eJNVAlarm0x20040	= 0x20040,	// 到站未停车
	eJNVAlarm0x20080	= 0x20080,	// 开门行车
	eJNVAlarm0x20100	= 0x20100,	// 到站未开车门
	eJNVAlarm0x20200	= 0x20200,	// 中途开车门
	eJNVAlarm0x20400	= 0x20400,	// 车厢温度高
	eJNVAlarm0x20800	= 0x20800,	// 车厢温度低
	eJNVAlarm0x21000	= 0x21000,	// 滞站告警
	eJNVAlarm0x22000	= 0x22000,	// 紧急加速
	eJNVAlarm0x24000	= 0x24000,	// 紧急刹车
	eJNVAlarm0x28000	= 0x28000,	// 票箱门报警
	eJNVAlarm0x30001	= 0x30001,	// 紧急报警
	eJNVAlarm0x30002	= 0x30002,	// 偏离线路
	eJNVAlarm0x30004	= 0x30004,	// 电子锁打开
	eJNVAlarm0x30008	= 0x30008,	// 电子锁关闭
	eJNVAlarm0x30010	= 0x30010,	// 电子锁异常
	eJNVAlarm0x30020	= 0x30020,	// 输入1触发
	eJNVAlarm0x30040	= 0x30040,	// 输入2触发
	eJNVAlarm0x30080	= 0x30080,	// 输入3触发
	eJNVAlarm0x30100	= 0x30100,	// 输入4触发
	eJNVAlarm0x30200	= 0x30200,	// 输入5触发
	eJNVAlarm0x30400	= 0x30400,	// 输入6触发
	eJNVAlarm0x30800	= 0x30800,	// 输入7触发
	eJNVAlarm0x31000	= 0x31000,	// 输入8触发
	eJNVAlarm0x32000	= 0x32000,	// 非法开关门
	eJNVAlarm0x34000	= 0x34000,	// 车辆载客超载

	eJNVAlarm0x40000	= 0x40000,	// 越界
	eJNVAlarm0x80000	= 0x80000,	// 越界消除
	eJNVAlarm0x100000	= 0x100000,	// 超时停车
	eJNVAlarm0x200000	= 0x200000,	// 超时停车消除
	eJNVAlarm0x400000	= 0x400000,	// 疲劳驾驶
	eJNVAlarm0x800000	= 0x800000,	// 疲劳驾驶消除
}eJNVAlarm;
// 报警类型定义 end

//客户端类型
typedef enum e_NV_ClientType
{
		E_NV_ClientType_MvsClient				=	0x00,		//MVS_Client客户端
		E_NV_ClientType_AndriodClient			=	0x01,		//android手机移动客户端
		E_NV_ClientType_IPhoneClient			=	0x02,		//iphone手机移动客户端
		E_NV_ClientType_WinPhoneClient			=	0x03,		//winphone手机移动客户端
		E_NV_ClientType_DbTool					=	0x04,		//DB_TOOL工具
		E_NV_ClientType_DeviceConfigTool		=	0x05,		//设备参数配置工具
		E_NV_ClientType_AutoDownloadTool		=	0x06,		//自动下载工具
		E_NV_ClientType_ReportStationTool		=	0x07,		//报站工具
		
		E_NV_ClientType_Device					=	0xfd,		//设备通信客户端
		E_NV_ClientType_DebugTool				=	0xfe,		//调试工具
		E_NV_ClientType_Other					=	0xff,		//其他工具
}e_NV_ClientType;

typedef int (__stdcall fcbJNVEvent)(LPCTSTR lpszEventStr,void* pUserParam);															// 事件回调函数
typedef int (__stdcall fcbJStream)(long lHandle, LPBYTE pBuff,DWORD dwRevLen,void* pUserParam);										// 接收到信息的回调函数
typedef int (__stdcall fcbJEvent)(long lHandle, eJNetEvent eType,int iDataType,void* pEventData,int iDataLen,void* pUserParam);		// 事件回调函数
typedef int (__stdcall fcbDecFrame)(PSTDecFrameCB pstDecFrameCB);																	// 解码回调函数,0表示继续处理,1表示不能继续处理
typedef int (__stdcall fcbPlay)(AVPPlayCBMsg eMsg,int iPlayerID,long lpUserParam,__int64 lParam1,__int64 lParam2,DWORD dwRes);		// 播放回调函数

// 用于心跳的包头 begin
typedef struct stJNVProHeadMin
{
	int		nType;			// 类型 (参见协议类型_eVMSProtocolType, 并对应不同的附加结构)
	int		nLength;		// 数据长度 （本身长度+附加结构)
	static int		s_iSize;
}* PSTJNVProHeadMin;
// 用于心跳的包头 end

// 用于信令的包头 begin
typedef struct stJNVProHead
{
	int		nType;			// 类型 (参见协议类型_eVMSProtocolType, 并对应不同的附加结构)
	int		nLength;		// 数据长度 （本身长度+附加结构)
	int		nQueue;			// 序列
	int		nParameter1;	// 参数1
	int		nParameter2;	// 参数2
	stJNVProHead()
	{
		stZero();
	}
	void stZero()
	{
		nType		= 0;	// 类型 (参见协议类型_eVMSProtocolType, 并对应不同的附加结构)
		nLength		= 0;	// 数据长度 （本身长度+附加结构)
		nQueue		= 0;	// 序列
		nParameter1	= 0;	// 参数1
		nParameter2	= 0;	// 参数2
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
// 用于信令的包头 end

// 用于获取文件信息的数据结构 begin
typedef struct stJNVProFileData
{
	int		nTransID;			// 传输ID
	char	pFileData[0];		// 传输的数据
}* PSTJNVProFileData;
// 用于获取文件信息的数据结构 end

// 用户登陆应答
typedef struct stJNVC2SLoginRet
{
	int			nUserID;
	int			nUserType;
	int			nSessionID;
}* PSTJNVC2SLoginRet;

//用户登陆请求
typedef struct stJNVC2SLoginReq
{
	char		szUserName[JNET_NAME_LEN];
	char		szUserPassword[JNET_PWD_LEN];
}* PSTJNVC2SLoginReq;

//用户登陆请求
typedef struct stJNVC2SLoginReqEx
{
	char		szUserName[JNET_NAME_LEN*2];
	char		szUserPassword[JNET_PWD_LEN*2];
	int			nClientType;			//客户端类型
	char		szClientVersion[12];	//客户端版本号，格式为1.1.0.1
}* PSTJNVC2SLoginReqEx;


// 请求订阅报警 begin
typedef struct stJNVProAlarmStart
{
	stJNVProHead	stHead;		// 消息头
	char			szDeviceID[JNET_DEV_ID_LEN];	// 设备ID
}* PSTJNVProAlarmStart;
// 请求订阅报警 end

// 请求取消订阅报警 begin
typedef struct stJNVProAlarmStop
{
	stJNVProHead	stHead;		// 消息头
	char			szDeviceID[JNET_DEV_ID_LEN];	// 设备ID
}* PSTJNVProAlarmStop;
// 请求取消订阅报警 end

// 报警结构体 begin
typedef struct stAlarmData
{
	char szDeviceID[JNET_DEV_ID_LEN];				// 设备ID
	char szTime[JNET_TIME_STR_LEN];					// 报警时间
	int  nChannel;									// 报警通道（通道基于零）,见 param1
	int  nAlarmType;								// 报警类型,见 param2
}* PSTAlarmData;
// 报警结构体 end

// 请求订阅GPS begin
typedef struct stJNVProGPSStart
{
	stJNVProHead	stHead;		// 消息头
	char			szDeviceID[JNET_DEV_ID_LEN];	// 设备ID
}* PSTJNVProGPSStart;
// 请求订阅GPS end

// 请求取消订阅GPS begin
typedef struct stJNVProGPSStop
{
	stJNVProHead	stHead;		// 消息头
	char			szDeviceID[JNET_DEV_ID_LEN];	// 设备ID
}* PSTJNVProGPSStop;
// 请求取消订阅GPS end

// GPS数据 -- 新协议 begin
// GSensor参数
typedef struct stJNVGPSGSensor
{
	bool			bValid;				// 信息有效性
	float			fAccelate_X;		// x方向加速度(单位:g)
	float			fAccelate_Y;		// y方向加速度(单位:g)
	float			fAccelate_Z;		// z方向加速度(单位:g)
}* PSTJNVGPSGSensor;
// GyroSensor
typedef struct stJNVGPSGyroSensor
{
	bool			bValid;				// 信息有效性
	float			fCorner;			// 转角加速度(单位:g)
}* PSTJNVGPSGyroSensor;
// GPS状态
typedef struct stJNVGPSStatus
{
	bool			bValid;				// 信息有效性
	bool			bKeyValid;			// 车钥匙有效性
	bool			bBrakeValid;		// 刹车信号有效性
	bool			bLeftValid;			// 左转有效性
	bool			bRightValid;		// 右转有效性
	bool			bGPSModuleValid;	// GPS模块有效性
}* PSTJNVGPSStatus;
// GPS基础信息
typedef struct stJNVGPSBaseInfo
{
	bool			bValid;				// 信息有效性
	double			dbLongitude;		// 经度(>0:东经,<0:西经)
	double			dbLatitude;			// 纬度(>0:南纬,<0:北纬)
	float			fDirect;			// 方向(0~360°)
	float			fSpeed;				// 速度(千米/小时,负数无效)
}* PSTJNVGPSBaseInfo;
// GPS扩展信息
typedef struct stJNVGPSExternInfo
{
	bool			bValid;				// 信息有效性
	short			usHigh;				// 海拔高度
	char			ucSatelliteNumber;	// 卫星数量
	float			fTemperature;		// 温度(摄氏度,>=-10000无效)
}* PSTJNVGPSExternInfo;
// GPS相关所有信息
typedef struct stJNVGPSAll
{
	char				szDevID[JNET_DEV_ID_LEN];	// 设备ID
	stJNVGPSBaseInfo	stBaseInfo;		// 基础信息
	stJNVGPSStatus		stStatus;		// 状态数据
	stJNVGPSGSensor		stGSensor;		// G-Sensor数据
	stJNVGPSGyroSensor	stGyroSensor;	// Gyro-Sensor数据
	stJNVGPSExternInfo	stExternInfo;	// 扩展信息
	char				stTime[24];		// 时间信息
}* PSTJNVGPSAll;
// GPS数据 -- 新协议 end

// 对讲相关 begin
typedef struct stJNVTalkReqSingle
{
	stJNVProHead	stHead;									// 消息头
	int				nWorkMode;								// 工作方式(0:对讲,1:广播,2:PTT,3:监听)
	short			usControl;								// 控制方式(0:取消,1:请求)
	short			usChannel;								// 通道
	int				nDeviceNumber;							// 设备数量
	int				nSessionID;								// 会话ID
	char			szDeviceID[JNET_DEV_ID_LEN];			// 设备ID
}* PSTJNVTalkReqSingle;
// 对讲相关 end

// 录像相关 begin
// 录像查询
typedef struct stJNVRecQuery
{
	stJNVProHead		stHead;								// 消息头
	char				szDeviceID[JNET_DEV_ID_LEN];		// 设备ID
	int					iCenter;							// 中心或者设备录像(1,表示中心录像;0,表示设备录像)
	int					nStorage;							// 存储介质(保留)
	int					nType;								// 录像类型
	char				szStartTime[JNET_TIME_STR_LEN];		// 开始时间，时间格式如：年-月-日 时:分:秒
	char				szStopTime[JNET_TIME_STR_LEN];		// 结束时间
	int					nChannelList;						// 通道编号列表(按位表示通道，比如：00001111，表示1-4通道)
}* PSTJNVRecQuery;
//录像记录
typedef struct stJNVRecSumInfo
{
	char				szFileName[JNET_REC_PATH_LEN];		// 文件名
	char				szStartTime[JNET_TIME_STR_LEN];		// 开始时间(格式:2010-11-12 21:12:12)
	char				szStopTime[JNET_TIME_STR_LEN];		// 结束时间
	unsigned int		uiFileSize;							// 文件大小
	unsigned int		uiSeconds;							// 总录像秒数
	int					nChannelIndex;						// 通道编号
	int					nRecordType;						// 录像类型(参见e_NV_RecordType)
}* PSTJNVRecSumInfo;
// 录像下载
typedef struct stJNVRecDown
{
	stJNVProHead		stHead;								// 消息头
	char				szDeviceID[JNET_DEV_ID_LEN];		// 设备ID
	int					nSessionID;							// SessionID
	char				szDevPath[JNET_FILE_PATH_LEN];		// 设备文件路径
	int					iStartPos;							// 开始位置
}* PSTJNVRecDown;
// 录像相关 end

// 实时视频相关 begin
// 打开流
typedef struct stJNVRealStreamStart
{
	stJNVProHead		stHead;								// 消息头
	char				szDeviceID[JNET_DEV_ID_LEN];		// 设备ID
	int					nChannel;							// 通道编号(1开始)
	int					nStreamIndex;						// 码流编号(保留)
	int					nSessionID;							// Session
	int					nStreamType;						// 码流类型(保留)
}* PSTJNVRealStreamStart;
// 请求I帧
typedef struct stJNVRealStreamGetIFrame
{
	stJNVProHead		stHead;								// 消息头
	char				szDeviceID[JNET_DEV_ID_LEN];		// 设备ID
	int					nChannel;							// 通道编号(1开始)
}* PSTJNVRealStreamGetIFrame;

// 头帧数据结构
typedef struct stJNVStreamHeaderFrameInfo
{
	int					nWidth;			// 宽度
	int					nHeight;		// 高度
	int					nAudioType;		// 音频编码类型
	int					nFrameRate;		// 帧率
//	char				pData[0];		// 帧数据
}* PSTJNVStreamHeaderFrameInfo;


// 视频帧数据
typedef struct stJNVStreamFrameVideoInfo
{
	__int64				tTime;			// 时间戳
//	char				pData[0];		// 视频数据(104字节整数倍)
}* PSTJNVStreamFrameVideoInfo;
// 音频帧数据
typedef struct stJNVStreamFrameAudioInfo
{
	__int64				tTime;			// 时间戳
//	char				pData[0];		// 音频数据(104字节整数倍)
}* PSTJNVStreamFrameAudioInfo;
// 录像格式
typedef struct stJNVVideoFramInfo
{
	DWORD				dwMagic;		// 魔字	// 0x204A4E56	// JNV
	long				dwFrameNo;		// 帧号
	int					nWidth;			// 宽度
	int					nHeight;		// 高度
	int					nMediaType;		// 媒体类型,见 eJNetMediaType
	int					nFrameRate;		// 帧率
	__int64				tTime;			// 时间戳
	DWORD				dwDataSize;		// 帧数据长
//	char				pData[0];		// 帧数据
}* PSTJNVVideoFramInfo;
// 实时视频相关 end

// 回放视频相关 begin
// 打开录像回放
typedef struct stJNVReplayStart
{
	stJNVProHead		stHead;								// 消息头
	char				szDeviceID[JNET_DEV_ID_LEN];		// 设备ID
	bool				bCenter;							// 中心或者设备录像
	int					nChannel;							// 通道编号
	int					nSessionID;							// Session id
	char				szStartTime[JNET_TIME_STR_LEN];		// 开始时间
	char				szStopTime[JNET_TIME_STR_LEN];		// 结束时间
	int					nOnlyIFrame;						// 是否只要I帧
	int					nRecordType;						// 录像类型
	int					nStartPosition;						// 开始位置
}* PSTJNVReplayStart;
// 录像回放控制
typedef struct stJNVReplayCtrl
{
	stJNVProHead		stHead;								// 消息头
	char				szDeviceID[JNET_DEV_ID_LEN];		// 设备ID
	int					nChannel;							// 通道编号
	int					nControlType;						// 控制类型(参见 eNVReplayType)	
	int					nValue;								// 设置值
}* PSTJNVReplayCtrl;
// 回放视频相关 end

// 控制云台 begin
typedef struct stJNVPtzCtrl
{
	stJNVProHead		stHead;								// 消息头
	char				szDeviceID[JNET_DEV_ID_LEN];		// 设备ID
	int					nChannelIndex;						// 通道编号
	int					nActionType;						// 控制类型(参见 eNVPtzActionType)
	int					nReserved1;							// 保留1(1:有x方向速度时候为x方向速度
															// 2:无x方向速度,但是有y方向速度,未y方向速度
															// 3:有预置位时候为预置位
															// 4:其他保留
	int					nReserved2;							// 保留2(1:保留1有x方向速度速度时候,此处有y方向速度的时候,为y方向速度
															// 2:其他保留)
	int					nSessionID;												
}* PSTJNVPtzCtrl;
// 控制云台 end




// 服务器类型
typedef enum
{
	eJNVCenterServer	= 0,
	eJNVCmdServer		= 1,
	eJNVMediaServer		= 2,
}eJNVServerType;

// 服务器数组
typedef struct stJNVServerInfo
{
	char				szServerId[JNET_SERVER_ID_LEN];		// 服务器ID
	char				szServerIP[20];						// 服务器IP
	int					nServerPort;						// 服务器port
	int					nServerType;						// 服务器类型
	void*				lpCasTcp;							// 服务器连接对象指针
}* PSTJNVServerInfo;
// 服务器数组 end








// 字符拷贝
inline char* StrNCpy(char* pTag,const char* pSrc,int iLen)
{
	strncpy_s(pTag,pSrc,iLen);
	pTag[iLen-1] = '\0';
	return pTag;
}
// 设置发送头
inline int SetNetHdr(PSTJNVProHead pstNetHdr, DWORD dwPackLen, DWORD dwPackCmd, DWORD dwSeq)
{
	pstNetHdr->nType		= dwPackCmd;	// 类型 (参见协议类型_eVMSProtocolType, 并对应不同的附加结构)
	pstNetHdr->nLength		= dwPackLen;	// 数据长度 （本身长度+附加结构)
	pstNetHdr->nQueue		= dwSeq;		// 序列
	pstNetHdr->nParameter1	= 0;			// 参数1
	pstNetHdr->nParameter2	= 0;			// 参数2
	return JNETErrSuccess;
}
inline int GetMin(int a,int b)
{
	if ( a>b ) return b;
	else return a;
}

#endif	// __PROTOCOL_NV_H__



