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

// 属性
public:
protected:

// 方法
public:
	// 查找符号
	long static FindSymbol( LPCTSTR lpszMode, long lLenMode,int iSymbol,long lStartPos, BOOL bSensCase = FALSE, BOOL bWholeWord = TRUE );
	// 查找单词
	long static FindWord( LPCTSTR lpszMode, long lLenMode, LPCTSTR lpszWord, long lLenSub, long lStartPos, BOOL bSensCase = FALSE, BOOL bWholeWord = TRUE );
	// 查找单词,返回时的索引值跳过单词
	long static FindWordOffset( LPCTSTR lpszMode, long lLenMode, LPCTSTR lpszWord, long lLenSub, long lStartPos, BOOL bSensCase = FALSE, BOOL bWholeWord = TRUE );

	// 模式子串起始索引.入口参数:模式串指针pMode,子串指针pSub,模式串长iLenMode,子串长iLenSub,
	// 起始搜索位置iStartPos, 大小写是否敏感bSensCase, 最大匹配串长度lMaxMatch, 最大匹配串起始位置plMaxStartPos
	// 返回小于0时,表示错误,返回>=0时,表示索引号
	long static SubIndex(void *pMode, void *pSub, long lLenMode, long lLenSub, 
		long lStartPos, BOOL bSensCase = TRUE, long *plMaxMatch = NULL, long *plMaxStartPos = NULL );
};
}
using namespace JSocketCtrl;

#endif // !defined(AFX_JBUNCHEX_H_JSOCK__EF0FD059_5166_4495_9A7D_61DB2C1F99A2__INCLUDED_)
