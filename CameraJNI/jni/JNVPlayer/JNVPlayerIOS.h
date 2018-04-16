#ifdef JNVSDK_EXPORTS
#define JNVSDK_API extern "C"
#else
#define JNVSDK_API
#endif

// 系统操作 begin
// 返回错误值,见 JNETErr 说明
JNVSDK_API long JNV_Init(int iSize);
// 返回错误值,见 JNETErr 说明
JNVSDK_API long JNV_UnInit();
// 系统操作 end

// 登录相关 begin
// 返回登录句柄(>=0)或错误值(见 JNETErr 说明)
// fcbJavaEvent:事件回调函数
JNVSDK_API long JNV_N_Login(LPCTSTR lpszIP,int iDevPort,LPCTSTR lpszUser,LPCTSTR lpszPassword,int iTimeOut,
	fcbJNVEvent* pfEventFun,long lUserParam);

// 返回错误值,见 JNETErr 说明
JNVSDK_API long JNV_N_Logout();
// 登录相关 end

// 请求设备列表:0,表示成功;其他,表示失败
// 返回错误值,见 JNETErr 说明
JNVSDK_API long JNV_N_GetDevList(LPCTSTR lpszFilePath);

// 请求服务器列表:0,表示成功;其他,表示失败
// 返回错误值,见 JNETErr 说明
JNVSDK_API long JNV_N_GetServerList(LPCTSTR lpszFilePath, int nServerType);

// 设置连接服务器类型:0,表示成功;其他,表示失败
// 参数：nConnectServerType  连接服务器类型  0 连单服务器版   1 连级联版本
// 返回错误值,见 JNETErr 说明
JNVSDK_API long JNV_N_SetConnectServerType(int nConnectServerType);

// 增加服务器信息:0,表示成功;其他,表示失败
// 参数 lpszServerId 服务器id  
//		lpszServerIp 服务器ip
//		nServerPort  服务器port
//		nServerType  服务器类型（0 中心 1信令 2媒体）
// 返回错误值,见 JNETErr 说明
JNVSDK_API long JNV_N_AddServerInfo(LPCTSTR lpszServerId, LPCTSTR lpszServerIp, int nServerPort, int nServerType);
// 报警相关 begin
// 返回错误值,见 JNETErr 说明
JNVSDK_API long JNV_N_GetAlarmStart(LPCTSTR lpszDevID);
// 返回错误值,见 JNETErr 说明
JNVSDK_API long JNV_N_GetAlarmStop(LPCTSTR lpszDevID);
// 报警相关 end

// 对讲相关 begin
JNVSDK_API long JNV_N_TalkStart(LPCTSTR lpszDevID,int iChn);
JNVSDK_API long JNV_N_TalkStop(long lTalk);
JNVSDK_API long JNV_N_TalkSend(long lTalk,LPBYTE lpBuff,int iLen);
// 对讲相关 end

// GPS信息相关 begin
// 返回错误值,见 JNETErr 说明
JNVSDK_API long JNV_N_GetGPSStart(LPCTSTR lpszDevID);
// 返回错误值,见 JNETErr 说明
JNVSDK_API long JNV_N_GetGPSStop(LPCTSTR lpszDevID);
// GPS信息相关 end

// 录像下载相关 begin
// 返回错误值,见 JNETErr 说明
JNVSDK_API long JNV_N_RecQuery(LPCTSTR lpszDevID,int iCenter,int iType,LPCTSTR lpszTimeStart,LPCTSTR lpszTimeEnd,int iChnFlag,LPCTSTR lpszFilePath);
// 返回错误值,见 JNETErr 说明
JNVSDK_API long JNV_N_RecDownload(LPCTSTR lpszDevID,LPCTSTR lpszDevPath,int iStartPos,LPCTSTR lpszFilePath);
// 录像下载相关 end

// 云台控制 begin
// 返回错误值,见 JNETErr 说明
// iAction,见 eNVPtzActionType 说明
// iValue1:保留1(x速度/y速度/预置位)
// iValue2:保留2(y速度)
JNVSDK_API long JNV_N_PtzCtrl(LPCTSTR lpszDevID,int iChn,int iAction,int iValue1,int iValue2);
// 云台控制 end

// 实时视频相关 begin
// 开始实时视频流,设备ID,通道编号(1开始),码流编号(保留),码流类型(保留)
// 返回流句柄(>=0)或错误值(见 JNETErr 说明)
// fcbJavaDec:解码事件回调函数
JNVSDK_API long JNV_OpenStream(LPCTSTR lpszDevID,int iChn,int iStream,int iType,
		fcbDecFrame* pfDecFunLocal,long lUserParam);
// 返回错误值,见 JNETErr 说明
JNVSDK_API long JNV_StopStream(long lStream);
// 实时视频相关 end

// 远程录像回放相关 begin
// 返回流句柄(>=0)或错误值(见 JNETErr 说明)
// fcbJavaDec:解码事件回调函数
JNVSDK_API long JNV_ReplayStart(LPCTSTR lpszDevID,int iCenter,int iChn,LPCTSTR lpszTimeStart,LPCTSTR lpszTimeEnd,int iOnlyIFrame,int iRecType,int iStartPos,
		fcbDecFrame* pfDecFunLocal,long lUserParam);
// 返回错误值,见 JNETErr 说明
// iCtrlType,见 eNVReplayType 说明
JNVSDK_API long JNV_ReplayCtrl(long lStream,int iCtrlType,int iValue);
// 返回错误值,见 JNETErr 说明
JNVSDK_API long JNV_ReplayStop(long lStream);
// 远程录像回放相关 end

// 解码操作 begin
// 抓图
// 返回错误值,见 JNETErr 说明
JNVSDK_API long JNV_Capture(long lStream,LPCTSTR lpszPath);
// 开始录像
// 返回错误值,见 JNETErr 说明
JNVSDK_API long JNV_RecStart(long lStream,LPCTSTR lpszPath);
// 停止录像
// 返回错误值,见 JNETErr 说明
JNVSDK_API long JNV_RecStop(long lStream);
// 解码操作 end

// 本地录像回放相关 begin
// 返回流句柄(>=0)或错误值(见 JNETErr 说明)
// fcbJavaDec:解码事件回调函数
// fcbJavaPlayInfo:回放信息回调函数
JNVSDK_API long JNV_RecOpenFile(LPCTSTR lpszPath,
		fcbDecFrame* pfDecFunLocal,fcbPlay* pfPlayInfo,long lUserParam);
// 返回错误值,见 JNETErr 说明
JNVSDK_API long JNV_RecCloseFile(long lRecFile);
// 返回错误值,见 JNETErr 说明
JNVSDK_API long JNV_RecPlayStart(long lRecFile);
// 返回错误值,见 JNETErr 说明
JNVSDK_API long JNV_RecPlayPause(long lRecFile);
// 返回错误值,见 JNETErr 说明
JNVSDK_API long JNV_RecPlaySetSpeed(long lRecFile,int iSpeed);
// 返回速度值 见 AVPPlaySpeed 说明
JNVSDK_API long JNV_RecPlayGetSpeed(long lRecFile);
// 返回错误值,见 JNETErr 说明
JNVSDK_API long JNV_RecPlayNextFrame(long lRecFile);
// 返回错误值,见 JNETErr 说明
JNVSDK_API long JNV_RecPlaySeek(long lRecFile,long lTime);
// 本地录像回放相关 end

