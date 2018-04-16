// JBunchEX.h: interface for the CJBunchEX class.
//
//////////////////////////////////////////////////////////////////////

#if !defined(AFX_JBUNCHEX_H_JSOCK__EF0FD059_5166_4495_9A7D_61DB2C1F99A2__INCLUDED_)
#define AFX_JBUNCHEX_H_JSOCK__EF0FD059_5166_4495_9A7D_61DB2C1F99A2__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

#include "j_vs_ver.h"

namespace JSocketCtrl
{
class CJBunchEX  
{
public:
	CJBunchEX();
	virtual ~CJBunchEX();

// ����
public:
protected:

// ����
public:
	// ���ҷ���
	long static FindSymbol( LPCTSTR lpszMode, long lLenMode,int iSymbol,long lStartPos, BOOL bSensCase = FALSE, BOOL bWholeWord = TRUE );
	// ���ҵ���
	long static FindWord( LPCTSTR lpszMode, long lLenMode, LPCTSTR lpszWord, long lLenSub, long lStartPos, BOOL bSensCase = FALSE, BOOL bWholeWord = TRUE );
	// ���ҵ���,����ʱ������ֵ��������
	long static FindWordOffset( LPCTSTR lpszMode, long lLenMode, LPCTSTR lpszWord, long lLenSub, long lStartPos, BOOL bSensCase = FALSE, BOOL bWholeWord = TRUE );

	// ģʽ�Ӵ���ʼ����.��ڲ���:ģʽ��ָ��pMode,�Ӵ�ָ��pSub,ģʽ����iLenMode,�Ӵ���iLenSub,
	// ��ʼ����λ��iStartPos, ��Сд�Ƿ�����bSensCase, ���ƥ�䴮����lMaxMatch, ���ƥ�䴮��ʼλ��plMaxStartPos
	// ����С��0ʱ,��ʾ����,����>=0ʱ,��ʾ������
	long static SubIndex(void *pMode, void *pSub, long lLenMode, long lLenSub, 
		long lStartPos, BOOL bSensCase = TRUE, long *plMaxMatch = NULL, long *plMaxStartPos = NULL );
};
}
using namespace JSocketCtrl;

#endif // !defined(AFX_JBUNCHEX_H_JSOCK__EF0FD059_5166_4495_9A7D_61DB2C1F99A2__INCLUDED_)
