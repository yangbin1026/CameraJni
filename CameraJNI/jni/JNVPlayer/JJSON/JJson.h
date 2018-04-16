#ifndef		__JJSON_H__
#define		__JJSON_H__

#include "j_vs_ver.h"
#if (defined _WIN32) || (defined _WIN64)
#else
typedef struct RECT
{
    long left;
    long top;
    long right;
    long bottom;
}* LPRECT;
typedef DWORD				COLORREF;
#define RGB(r,g,b) ((COLORREF)(((BYTE)(r)|((WORD)((BYTE)(g))<<8))|(((DWORD)(BYTE)(b))<<16)))
#endif
#define JJSON_INVALID		0xFFFF

typedef enum
{
	JJSON_STRING	= 0, 
	JJSON_NUMBER	= 1,
	JJSON_OBJECT	= 2,
	JJSON_ARRAY		= 3,
	JJSON_TRUE		= 4,
	JJSON_FALSE		= 5,
	JJSON_NULL		= 6,
}eJJsonValueType;

struct stJJsonValue;		typedef stJJsonValue*	PSTJJsonValue;
struct stJJsonValue
{
	eJJsonValueType		m_eType;
	const char*			m_szText;
	// FIFO queue data
	PSTJJsonValue		m_pNext;
	PSTJJsonValue		m_pPrevious;
	PSTJJsonValue		m_pParent;
	PSTJJsonValue		m_pChild;
	PSTJJsonValue		m_pChild_end;
};

// This class is exported from the JJson.dll
class CJJson
{
public:
	CJJson();
	virtual ~CJJson();

	bool IsValid();
	bool Parse(LPCTSTR szBuf);
	char* TreeToString(PSTJJsonValue item);
	bool Close();

	PSTJJsonValue GetRootItem();							// 根结点
	PSTJJsonValue GetParentItem(PSTJJsonValue item);		// 父结点
	PSTJJsonValue GetChildItem(PSTJJsonValue item);			// 子结点
	PSTJJsonValue GetPrevSibling(PSTJJsonValue item);		// 上一兄弟结点
	PSTJJsonValue GetNextSibling(PSTJJsonValue item);		// 下一兄弟结点

	PSTJJsonValue FindSubItem(PSTJJsonValue item,LPCTSTR lpszSub);		// 查找子结点

	// 获取值 begin
	bool		GetValue(PSTJJsonValue item,char* strRet);
	bool		GetRectValue(PSTJJsonValue item,LPRECT lpRect);
	COLORREF	GetRGBValue(PSTJJsonValue item);
	int			GetIntValue(PSTJJsonValue item);
	long		GetLongValue(PSTJJsonValue item);
	LPCTSTR		GetPathValue(PSTJJsonValue item,LPCTSTR lpszPrev,char* strRet);
	#if (defined _WIN32) || (defined _WIN64)
	HBITMAP		GetBmpValue(PSTJJsonValue item,LPCTSTR lpszPrev);
	HCURSOR		GetCursorValue(PSTJJsonValue item,LPCTSTR lpszPrev);
	HICON		GetIconValue(PSTJJsonValue item,LPCTSTR lpszPrev);
	#endif
	// 获取值 end

	// 获取值 begin
	bool		IntoItem(PSTJJsonValue item);
	bool		GetValue(LPCTSTR lpszKey,char* strRet);
	bool		GetRectValue(LPCTSTR lpszKey,LPRECT lpRect);
	COLORREF	GetRGBValue(LPCTSTR lpszKey);
	int			GetIntValue(LPCTSTR lpszKey);
	long		GetLongValue(LPCTSTR lpszKey);
	LPCTSTR		GetPathValue(LPCTSTR lpszKey,LPCTSTR lpszPrev,char* strRet);
	#if (defined _WIN32) || (defined _WIN64)
	HBITMAP		GetBmpValue(LPCTSTR lpszKey,LPCTSTR lpszPrev);
	HCURSOR		GetCursorValue(LPCTSTR lpszKey,LPCTSTR lpszPrev);
	HICON		GetIconValue(LPCTSTR lpszKey,LPCTSTR lpszPrev);
	#endif
	bool		OutItem();
	// 获取值 end

	// 添加值 begin
	PSTJJsonValue InsertValue(eJJsonValueType eType,PSTJJsonValue pstParent, PSTJJsonValue pstInsertAfter);
	PSTJJsonValue InsertArray(const char* szTextItem, PSTJJsonValue pstParent, PSTJJsonValue pstInsertAfter);
	PSTJJsonValue InsertString(const char* szTextItem, const char* str, PSTJJsonValue pstParent, PSTJJsonValue pstInsertAfter);
	PSTJJsonValue InsertNumber(const char* szTextItem, const char* szNum, PSTJJsonValue pstParent, PSTJJsonValue pstInsertAfter);
	// 添加值 end

	// 删除值 begin
	void DeleteValue(PSTJJsonValue* ppItem);
	// 删除值 end

protected:
	void SetItemAfterItem(PSTJJsonValue pstItem,PSTJJsonValue pstInsertAfter);	// 设置兄弟结点
protected:
	void*			m_pstState;								// 状态
	int				m_iLastErr;								// 上一错误码
	PSTJJsonValue	m_pstIntoItem;							// 进入的项
};


#endif		// __JJSON_H__
