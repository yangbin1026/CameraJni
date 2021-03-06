// JGetRunTimeTC.cpp: implementation of the CJGetRunTimeTC class.
//
//////////////////////////////////////////////////////////////////////

#include "JGetRunTimeTC.h"

#ifdef _DEBUG
#undef THIS_FILE
static char THIS_FILE[]=__FILE__;
#define new DEBUG_NEW
#endif

//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////

CJGetRunTimeTC::CJGetRunTimeTC()
{
	m_dwData	= 0;					// 用户值
}

CJGetRunTimeTC::~CJGetRunTimeTC()
{

}

// 开始计算
DWORD CJGetRunTimeTC::Start()
{
	m_dwStart = JGetTickCount();
	return m_dwStart;
}

// 结束计算
DWORD CJGetRunTimeTC::End()
{
	m_dwEnd = JGetTickCount();
	return m_dwEnd;
}

// 获取时间差
float CJGetRunTimeTC::GetTimeSpan()
{
	return float(long(m_dwEnd)-long(m_dwStart));
}

// 获取当前时间差
float CJGetRunTimeTC::GetCurSpan()
{
	DWORD	dwEnd;

	dwEnd = JGetTickCount();	
	return float(dwEnd-m_dwStart);
}

// 设置用户值
DWORD CJGetRunTimeTC::SetData(DWORD dwData)
{
	DWORD dwRet	= m_dwData;

	m_dwData = dwData;
	return dwRet;
}

// 获取用户值
DWORD CJGetRunTimeTC::GetData()
{
	return m_dwData;
}

	
