# Microsoft Developer Studio Project File - Name="NVTest" - Package Owner=<4>
# Microsoft Developer Studio Generated Build File, Format Version 6.00
# ** DO NOT EDIT **

# TARGTYPE "Win32 (x86) Application" 0x0101

CFG=NVTest - Win32 Debug
!MESSAGE This is not a valid makefile. To build this project using NMAKE,
!MESSAGE use the Export Makefile command and run
!MESSAGE 
!MESSAGE NMAKE /f "NVTest.mak".
!MESSAGE 
!MESSAGE You can specify a configuration when running NMAKE
!MESSAGE by defining the macro CFG on the command line. For example:
!MESSAGE 
!MESSAGE NMAKE /f "NVTest.mak" CFG="NVTest - Win32 Debug"
!MESSAGE 
!MESSAGE Possible choices for configuration are:
!MESSAGE 
!MESSAGE "NVTest - Win32 Release" (based on "Win32 (x86) Application")
!MESSAGE "NVTest - Win32 Debug" (based on "Win32 (x86) Application")
!MESSAGE 

# Begin Project
# PROP AllowPerConfigDependencies 0
# PROP Scc_ProjName ""
# PROP Scc_LocalPath ""
CPP=cl.exe
MTL=midl.exe
RSC=rc.exe

!IF  "$(CFG)" == "NVTest - Win32 Release"

# PROP BASE Use_MFC 6
# PROP BASE Use_Debug_Libraries 0
# PROP BASE Output_Dir "Release"
# PROP BASE Intermediate_Dir "Release"
# PROP BASE Target_Dir ""
# PROP Use_MFC 6
# PROP Use_Debug_Libraries 0
# PROP Output_Dir "Release"
# PROP Intermediate_Dir "Release"
# PROP Ignore_Export_Lib 0
# PROP Target_Dir ""
# ADD BASE CPP /nologo /MD /W3 /GX /O2 /D "WIN32" /D "NDEBUG" /D "_WINDOWS" /D "_AFXDLL" /Yu"stdafx.h" /FD /c
# ADD CPP /nologo /MT /W3 /GX /O2 /I "./" /I "../ffmpeg_win/" /I "./Common" /I "./JJSON" /I "./Net" /I "./Player" /D "WIN32" /D "_WIN32" /D "NDEBUG" /D "_WINDOWS" /D "_MBCS" /Yu"stdafx.h" /FD /c
# ADD BASE MTL /nologo /D "NDEBUG" /mktyplib203 /win32
# ADD MTL /nologo /D "NDEBUG" /mktyplib203 /win32
# ADD BASE RSC /l 0x804 /d "NDEBUG" /d "_AFXDLL"
# ADD RSC /l 0x804 /d "NDEBUG" /d "_AFXDLL"
BSC32=bscmake.exe
# ADD BASE BSC32 /nologo
# ADD BSC32 /nologo
LINK32=link.exe
# ADD BASE LINK32 /nologo /subsystem:windows /machine:I386
# ADD LINK32 JSocketCtrl.lib dsound.lib dxguid.lib ddraw.lib swscale.lib avcodec.lib avutil.lib /nologo /subsystem:windows /machine:I386 /nodefaultlib:"libc.lib" /out:"./bin/NVTest.exe" /libpath:"../ffmpeg_win"

!ELSEIF  "$(CFG)" == "NVTest - Win32 Debug"

# PROP BASE Use_MFC 6
# PROP BASE Use_Debug_Libraries 1
# PROP BASE Output_Dir "Debug"
# PROP BASE Intermediate_Dir "Debug"
# PROP BASE Target_Dir ""
# PROP Use_MFC 6
# PROP Use_Debug_Libraries 1
# PROP Output_Dir "Debug"
# PROP Intermediate_Dir "Debug"
# PROP Ignore_Export_Lib 0
# PROP Target_Dir ""
# ADD BASE CPP /nologo /MDd /W3 /Gm /GX /ZI /Od /D "WIN32" /D "_DEBUG" /D "_WINDOWS" /D "_AFXDLL" /Yu"stdafx.h" /FD /GZ /c
# ADD CPP /nologo /MTd /W3 /Gm /GX /ZI /Od /I "./" /I "../ffmpeg_win/" /I "./Common" /I "./JJSON" /I "./Net" /I "./Player" /I "./HI" /D "WIN32" /D "_WIN32" /D "_DEBUG" /D "_WINDOWS" /D "_MBCS" /D "JNVSDK_EXPORTS" /FR /Yu"stdafx.h" /FD /GZ /c
# ADD BASE MTL /nologo /D "_DEBUG" /mktyplib203 /win32
# ADD MTL /nologo /D "_DEBUG" /mktyplib203 /win32
# ADD BASE RSC /l 0x804 /d "_DEBUG" /d "_AFXDLL"
# ADD RSC /l 0x804 /d "_DEBUG" /d "_AFXDLL"
BSC32=bscmake.exe
# ADD BASE BSC32 /nologo
# ADD BSC32 /nologo
LINK32=link.exe
# ADD BASE LINK32 /nologo /subsystem:windows /debug /machine:I386 /pdbtype:sept
# ADD LINK32 JSocketCtrlD.lib dsound.lib dxguid.lib ddraw.lib swscale.lib avcodec.lib avutil.lib /nologo /subsystem:windows /debug /machine:I386 /nodefaultlib:"libcd.lib" /out:".\bin\NVTestD.exe" /pdbtype:sept /libpath:"../ffmpeg_win" /libpath:"./HI"

!ENDIF 

# Begin Target

# Name "NVTest - Win32 Release"
# Name "NVTest - Win32 Debug"
# Begin Group "Source Files"

# PROP Default_Filter "cpp;c;cxx;rc;def;r;odl;idl;hpj;bat"
# Begin Source File

SOURCE=.\IOSUtils.cpp

!IF  "$(CFG)" == "NVTest - Win32 Release"

!ELSEIF  "$(CFG)" == "NVTest - Win32 Debug"

# PROP Exclude_From_Build 1
# SUBTRACT CPP /YX /Yc /Yu

!ENDIF 

# End Source File
# Begin Source File

SOURCE=.\jniUtils.cpp
# PROP Exclude_From_Build 1
# End Source File
# Begin Source File

SOURCE=.\JNVPlayerIOS.cpp

!IF  "$(CFG)" == "NVTest - Win32 Release"

!ELSEIF  "$(CFG)" == "NVTest - Win32 Debug"

# PROP Exclude_From_Build 1
# SUBTRACT CPP /YX /Yc /Yu

!ENDIF 

# End Source File
# Begin Source File

SOURCE=.\JNVPlayerIOS.h
# End Source File
# Begin Source File

SOURCE=.\JNVPlayerJAVA.cpp
# PROP Exclude_From_Build 1
# End Source File
# Begin Source File

SOURCE=.\NVTest.cpp
# End Source File
# Begin Source File

SOURCE=.\NVTest.rc
# End Source File
# Begin Source File

SOURCE=.\NVTestDlg.cpp
# End Source File
# Begin Source File

SOURCE=.\ProtocolNV.cpp
# SUBTRACT CPP /YX /Yc /Yu
# End Source File
# Begin Source File

SOURCE=.\StdAfx.cpp
# ADD CPP /Yc"stdafx.h"
# End Source File
# End Group
# Begin Group "Header Files"

# PROP Default_Filter "h;hpp;hxx;hm;inl"
# Begin Source File

SOURCE=.\IOSUtils.h
# End Source File
# Begin Source File

SOURCE=.\jniUtils.h
# End Source File
# Begin Source File

SOURCE=.\NVTest.h
# End Source File
# Begin Source File

SOURCE=.\NVTestDlg.h
# End Source File
# Begin Source File

SOURCE=.\ProtocolNV.h
# End Source File
# Begin Source File

SOURCE=.\Resource.h
# End Source File
# Begin Source File

SOURCE=.\StdAfx.h
# End Source File
# End Group
# Begin Group "Resource Files"

# PROP Default_Filter "ico;cur;bmp;dlg;rc2;rct;bin;rgs;gif;jpg;jpeg;jpe"
# Begin Source File

SOURCE=.\res\NVTest.ico
# End Source File
# Begin Source File

SOURCE=.\res\NVTest.rc2
# End Source File
# End Group
# Begin Group "Common"

# PROP Default_Filter ""
# Begin Source File

SOURCE=.\Common\j_vs_ver.h
# End Source File
# Begin Source File

SOURCE=.\Common\JBunchEX.h
# End Source File
# Begin Source File

SOURCE=.\Common\JGetRunTimeTC.cpp
# SUBTRACT CPP /YX /Yc /Yu
# End Source File
# Begin Source File

SOURCE=.\Common\JGetRunTimeTC.h
# End Source File
# Begin Source File

SOURCE=.\Common\JLMuliticastSocket.h
# End Source File
# Begin Source File

SOURCE=.\Common\JLogWriter.h
# End Source File
# Begin Source File

SOURCE=.\Common\JLSocket.h
# End Source File
# Begin Source File

SOURCE=.\Common\JLSocketClient.h
# End Source File
# Begin Source File

SOURCE=.\Common\JLSocketDef.h
# End Source File
# Begin Source File

SOURCE=.\Common\JLSocketServer.h
# End Source File
# Begin Source File

SOURCE=.\Common\JLTcpClient.h
# End Source File
# Begin Source File

SOURCE=.\Common\JLTcpServer.h
# End Source File
# Begin Source File

SOURCE=.\Common\JLThreadCtrl.h
# End Source File
# Begin Source File

SOURCE=.\Common\JLUdpServer.h
# End Source File
# Begin Source File

SOURCE=.\Common\MutexLockLinux.h

!IF  "$(CFG)" == "NVTest - Win32 Release"

!ELSEIF  "$(CFG)" == "NVTest - Win32 Debug"

# PROP Exclude_From_Build 1

!ENDIF 

# End Source File
# Begin Source File

SOURCE=.\Common\MutexLockWnd.h
# End Source File
# End Group
# Begin Group "JJSON"

# PROP Default_Filter ""
# Begin Source File

SOURCE=.\JJSON\JJson.cpp
# SUBTRACT CPP /YX /Yc /Yu
# End Source File
# Begin Source File

SOURCE=.\JJSON\JJson.h
# End Source File
# Begin Source File

SOURCE=.\JJSON\json.cpp
# SUBTRACT CPP /YX /Yc /Yu
# End Source File
# Begin Source File

SOURCE=.\JJSON\json.h
# End Source File
# Begin Source File

SOURCE=.\JJSON\json_helper.cpp
# SUBTRACT CPP /YX /Yc /Yu
# End Source File
# Begin Source File

SOURCE=.\JJSON\json_helper.h
# End Source File
# End Group
# Begin Group "Net"

# PROP Default_Filter ""
# Begin Source File

SOURCE=.\Net\JNVCmdTcp.cpp
# SUBTRACT CPP /YX /Yc /Yu
# End Source File
# Begin Source File

SOURCE=.\Net\JNVCmdTcp.h
# End Source File
# Begin Source File

SOURCE=.\Net\JNVProtocolTCP.cpp
# SUBTRACT CPP /YX /Yc /Yu
# End Source File
# Begin Source File

SOURCE=.\Net\JNVProtocolTCP.h
# End Source File
# Begin Source File

SOURCE=.\Net\JNVStreamTcp.cpp
# SUBTRACT CPP /YX /Yc /Yu
# End Source File
# Begin Source File

SOURCE=.\Net\JNVStreamTcp.h
# End Source File
# Begin Source File

SOURCE=.\Net\JNVTalkTcp.cpp
# SUBTRACT CPP /YX /Yc /Yu
# End Source File
# Begin Source File

SOURCE=.\Net\JNVTalkTcp.h
# End Source File
# End Group
# Begin Group "Player"

# PROP Default_Filter ""
# Begin Group "Audio"

# PROP Default_Filter ""
# Begin Source File

SOURCE=.\Player\Audio\g711.cpp
# SUBTRACT CPP /YX /Yc /Yu
# End Source File
# Begin Source File

SOURCE=.\Player\Audio\g726.cpp
# SUBTRACT CPP /YX /Yc /Yu
# End Source File
# Begin Source File

SOURCE=.\Player\Audio\g726.h
# End Source File
# Begin Source File

SOURCE=.\Player\Audio\g726_16.cpp
# SUBTRACT CPP /YX /Yc /Yu
# End Source File
# Begin Source File

SOURCE=.\Player\Audio\g726_24.cpp
# SUBTRACT CPP /YX /Yc /Yu
# End Source File
# Begin Source File

SOURCE=.\Player\Audio\g726_32.cpp
# SUBTRACT CPP /YX /Yc /Yu
# End Source File
# Begin Source File

SOURCE=.\Player\Audio\g726_40.cpp
# SUBTRACT CPP /YX /Yc /Yu
# End Source File
# Begin Source File

SOURCE=.\Player\Audio\g72x.cpp
# SUBTRACT CPP /YX /Yc /Yu
# End Source File
# Begin Source File

SOURCE=.\Player\Audio\g72x.h
# End Source File
# Begin Source File

SOURCE=.\Player\Audio\private.h
# End Source File
# End Group
# Begin Source File

SOURCE=.\Player\Decoder.cpp
# SUBTRACT CPP /YX /Yc /Yu
# End Source File
# Begin Source File

SOURCE=.\Player\Decoder.h
# End Source File
# Begin Source File

SOURCE=.\Player\FMReader.cpp
# SUBTRACT CPP /YX /Yc /Yu
# End Source File
# Begin Source File

SOURCE=.\Player\FMReader.h
# End Source File
# Begin Source File

SOURCE=.\Player\FMReaderJNV.cpp
# SUBTRACT CPP /YX /Yc /Yu
# End Source File
# Begin Source File

SOURCE=.\Player\FMReaderJNV.h
# End Source File
# Begin Source File

SOURCE=.\Player\ImgConver.cpp
# SUBTRACT CPP /YX /Yc /Yu
# End Source File
# Begin Source File

SOURCE=.\Player\ImgConver.h
# End Source File
# Begin Source File

SOURCE=.\Player\JBuffPool.cpp
# SUBTRACT CPP /YX /Yc /Yu
# End Source File
# Begin Source File

SOURCE=.\Player\JBuffPool.h
# End Source File
# Begin Source File

SOURCE=.\Player\JNVPlayer.cpp
# SUBTRACT CPP /YX /Yc /Yu
# End Source File
# Begin Source File

SOURCE=.\Player\JNVPlayer.h
# End Source File
# Begin Source File

SOURCE=.\Player\JNVPlayerMng.cpp
# SUBTRACT CPP /YX /Yc /Yu
# End Source File
# Begin Source File

SOURCE=.\Player\JNVPlayerMng.h
# End Source File
# Begin Source File

SOURCE=.\Player\JRecFile.cpp
# SUBTRACT CPP /YX /Yc /Yu
# End Source File
# Begin Source File

SOURCE=.\Player\JRecFile.h
# End Source File
# End Group
# Begin Group "Draw"

# PROP Default_Filter ""
# Begin Source File

SOURCE=.\Draw\JDDraw.cpp
# End Source File
# Begin Source File

SOURCE=.\Draw\JDDraw.h
# End Source File
# End Group
# Begin Group "Sound"

# PROP Default_Filter ""
# Begin Source File

SOURCE=.\Sound\JDSound.cpp
# End Source File
# Begin Source File

SOURCE=.\Sound\JDSound.h
# End Source File
# End Group
# Begin Source File

SOURCE=.\ReadMe.txt
# End Source File
# End Target
# End Project
