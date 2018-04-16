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

	PSTJJsonValue GetRootItem();							// �����
	PSTJJsonValue GetParentItem(PSTJJsonValue item);		// �����
	PSTJJsonValue GetChildItem(PSTJJsonValue item);			// �ӽ��
	PSTJJsonValue GetPrevSibling(PSTJJsonValue item);		// ��һ�ֵܽ��
	PSTJJsonValue GetNextSibling(PSTJJsonValue item);		// ��һ�ֵܽ��

	PSTJJsonValue FindSubItem(PSTJJsonValue item,LPCTSTR lpszSub);		// �����ӽ��

	// ��ȡֵ begin
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
	// ��ȡֵ end

	// ��ȡֵ begin
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
	// ��ȡֵ end

	// ���ֵ begin
	PSTJJsonValue InsertValue(eJJsonValueType eType,PSTJJsonValue pstParent, PSTJJsonValue pstInsertAfter);
	PSTJJsonValue InsertArray(const char* szTextItem, PSTJJsonValue pstParent, PSTJJsonValue pstInsertAfter);
	PSTJJsonValue InsertString(const char* szTextItem, const char* str, PSTJJsonValue pstParent, PSTJJsonValue pstInsertAfter);
	PSTJJsonValue InsertNumber(const char* szTextItem, const char* szNum, PSTJJsonValue pstParent, PSTJJsonValue pstInsertAfter);
	// ���ֵ end

	// ɾ��ֵ begin
	void DeleteValue(PSTJJsonValue* ppItem);
	// ɾ��ֵ end

protected:
	void SetItemAfterItem(PSTJJsonValue pstItem,PSTJJsonValue pstInsertAfter);	// �����ֵܽ��
protected:
	void*			m_pstState;								// ״̬
	int				m_iLastErr;								// ��һ������
	PSTJJsonValue	m_pstIntoItem;							// �������
};


#endif		// __JJSON_H__
