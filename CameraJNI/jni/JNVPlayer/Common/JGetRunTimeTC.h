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

	DWORD		Start();						// ��ʼ����
	DWORD		End();							// ��������
	float		GetTimeSpan();					// ��ȡʱ���
	float		GetCurSpan();					// ��ȡ��ǰʱ���
	DWORD		SetData(DWORD dwData);			// �����û�ֵ
	DWORD		GetData();						// ��ȡ�û�ֵ
protected:
	DWORD			m_dwStart;					// ��ʼʱ��
	DWORD			m_dwEnd;					// ����ʱ��
	DWORD			m_dwData;					// �û�ֵ

};

#endif // !defined(AFX_JGETRUNTIMETC_H__7295DA9F_18CD_4D00_A9C2_2033DCA215E4__INCLUDED_)
