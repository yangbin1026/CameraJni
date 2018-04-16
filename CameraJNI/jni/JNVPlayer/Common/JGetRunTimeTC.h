// JGetRunTimeTC.h: interface for the CJGetRunTimeTC class.
//
//////////////////////////////////////////////////////////////////////

#if !defined(AFX_JGETRUNTIMETC_H__7295DA9F_18CD_4D00_A9C2_2033DCA215E4__INCLUDED_)
#define AFX_JGETRUNTIMETC_H__7295DA9F_18CD_4D00_A9C2_2033DCA215E4__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

#include "j_vs_ver.h"
class CJGetRunTimeTC  
{
public:
	CJGetRunTimeTC();
	virtual ~CJGetRunTimeTC();

	DWORD		Start();						// 开始计算
	DWORD		End();							// 结束计算
	float		GetTimeSpan();					// 获取时间差
	float		GetCurSpan();					// 获取当前时间差
	DWORD		SetData(DWORD dwData);			// 设置用户值
	DWORD		GetData();						// 获取用户值
protected:
	DWORD			m_dwStart;					// 开始时间
	DWORD			m_dwEnd;					// 结束时间
	DWORD			m_dwData;					// 用户值

};

#endif // !defined(AFX_JGETRUNTIMETC_H__7295DA9F_18CD_4D00_A9C2_2033DCA215E4__INCLUDED_)
