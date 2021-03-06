// JBunchEX.cpp: implementation of the CJBunchEX class.
//
//////////////////////////////////////////////////////////////////////

#include "JBunchEX.h"

#define JBUNCHEX_ERR_INDEX	-1
//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////

CJBunchEX::CJBunchEX()
{

}

CJBunchEX::~CJBunchEX()
{

}

// 查找符号
long CJBunchEX::FindSymbol( LPCTSTR lpszMode, long lLenMode,int iSymbol,long lStartPos, BOOL bSensCase, BOOL bWholeWord )
{
	long lModeIndex	= lStartPos;				// 模式串索引
	LPBYTE pbyMode	= (LPBYTE)lpszMode;

	while ( lModeIndex < lLenMode )
	{
		if ( pbyMode[lModeIndex]==iSymbol || 
			(!bSensCase && (pbyMode[lModeIndex]+32==iSymbol || pbyMode[lModeIndex]==iSymbol+32) )
			)
		{
			++lModeIndex;

			if ( bWholeWord )
			{
				if ( isalpha(pbyMode[lModeIndex]) )		// 下一个字符为字母，匹配失败。
				{
					return lModeIndex-1;
				}
			}
			else
			{
				return lModeIndex-1;
			}
		}
		lModeIndex++;
	}

	return -1;
}

// 查找单词
long CJBunchEX::FindWord( LPCTSTR lpszMode, long lLenMode, LPCTSTR lpszWord, long lLenSub, long lStartPos, BOOL bSensCase, BOOL bWholeWord )
{
	long lModeIndex	= lStartPos;				// 模式串索引
	long lSubIndex	= 0;						// 子串索引
	long lMaxMatch	= 0;						// 找到的最大匹配数
	long lMaxStartPos	= 0;					// 找到的最大匹配串起始位
	LPBYTE pbyMode	= (LPBYTE)lpszMode;
	LPBYTE pbySub	= (LPBYTE)lpszWord;

	while ( lModeIndex < lLenMode && lSubIndex < lLenSub )
	{
		if ( pbyMode[lModeIndex] == pbySub[lSubIndex] )	// 当前的相等,继续比较后续的字符
		{
			++lModeIndex;
			++lSubIndex;

			// 如果需要匹配整词，则需要查看模式串中的下一个是否为字母
			if ( bWholeWord && lSubIndex == lLenSub )
			{
				if ( isalpha(pbyMode[lModeIndex]) )		// 下一个字符为字母，匹配失败。
				{
					if ( lMaxMatch < lSubIndex ) 
					{
						lMaxStartPos = lModeIndex - lSubIndex;
						lMaxMatch = lSubIndex;
					}
					lModeIndex = lModeIndex - lSubIndex + 1;
					lSubIndex = 0;
				}
			}
		}
		else if ( !bSensCase && 
			(pbyMode[lModeIndex]+32==pbySub[lSubIndex] || pbyMode[lModeIndex]==pbySub[lSubIndex]+32) )
		{
			++lModeIndex;
			++lSubIndex;
		}
		else											// 索引后退重新开始匹配
		{
			if ( lMaxMatch < lSubIndex ) 
			{
				lMaxStartPos = lModeIndex - lSubIndex;
				lMaxMatch = lSubIndex;
			}
			lModeIndex = lModeIndex - lSubIndex + 1;
			lSubIndex = 0;
		}
	}
	if ( lSubIndex == lLenSub ) return lModeIndex - lLenSub;
	else return JBUNCHEX_ERR_INDEX;						// 找不到子串,返回-1;
}

// 查找单词,返回时的索引值跳过单词
long CJBunchEX::FindWordOffset( LPCTSTR lpszMode, long lLenMode, LPCTSTR lpszWord, long lLenSub, long lStartPos, BOOL bSensCase, BOOL bWholeWord )
{
	long lRevalue = FindWord(lpszMode, lLenMode, lpszWord, lLenSub, lStartPos, bSensCase, bWholeWord);

	if ( lRevalue==JBUNCHEX_ERR_INDEX ) return JBUNCHEX_ERR_INDEX;
	else return lRevalue+lLenSub;
}

// 模式子串起始索引.入口参数:模式串指针pMode,子串指针pSub,模式串长iLenMode,子串长iLenSub,
// 起始搜索位置iStartPos, 最大匹配串长度lMaxMatch, 最大匹配串起始位置plMaxStartPos
// 返回小于0时,表示错误,返回>=0时,表示索引号
long CJBunchEX::SubIndex(void *pMode, void *pSub, long lLenMode, long lLenSub,
						 long lStartPos, BOOL bSensCase, long *plMaxMatch, long *plMaxStartPos)
{
	long lModeIndex	= lStartPos;				// 模式串索引
	long lSubIndex	= 0;						// 子串索引
	long lMaxMatch	= 0;						// 找到的最大匹配数
	long lMaxStartPos	= 0;					// 找到的最大匹配串起始位
	LPBYTE pbyMode	= (LPBYTE)pMode;
	LPBYTE pbySub	= (LPBYTE)pSub;
	while ( lModeIndex < lLenMode && lSubIndex < lLenSub )
	{
		if ( pbyMode[lModeIndex] == pbySub[lSubIndex] )	// 当前的相等,继续比较后续的字符
		{
			++lModeIndex;
			++lSubIndex;
		}
		else if ( !bSensCase && 
			(pbyMode[lModeIndex]+32==pbySub[lSubIndex] || pbyMode[lModeIndex]==pbySub[lSubIndex]+32) )
		{
			++lModeIndex;
			++lSubIndex;
		}
		else											// 索引后退重新开始匹配
		{
			if ( lMaxMatch < lSubIndex ) 
			{
				lMaxStartPos = lModeIndex - lSubIndex;
				lMaxMatch = lSubIndex;
			}
			lModeIndex = lModeIndex - lSubIndex + 1;
			lSubIndex = 0;
		}
	}
	if ( plMaxMatch ) *plMaxMatch = lMaxMatch;			// 最大匹配数
	if ( plMaxStartPos ) *plMaxStartPos = lMaxStartPos;	// 最大匹配串起始位置
	if ( lSubIndex == lLenSub ) return lModeIndex - lLenSub;
	else return JBUNCHEX_ERR_INDEX;						// 找不到子串,返回-1;
}
