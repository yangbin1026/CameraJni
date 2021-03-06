// JJson.cpp : Defines the entry point for the DLL application.
//

#include "stdlib.h"
#include "JJson.h"
#include "json.h"

typedef json_parsing_info stParsing;
typedef stParsing* PSTParsing;

#define		JJSON_STATE()		((PSTParsing)m_pstState)

CJJson::CJJson()
{
	m_pstState				= new stParsing;		// 状态
	JJSON_STATE()->state	= JJSON_INVALID;
	m_iLastErr				= JSON_OK;
	m_pstIntoItem			= NULL;					// 进入的项
}

CJJson::~CJJson()
{
	PSTJJsonValue	pstRoot	= GetRootItem();
	
	if ( pstRoot )
	{
		DeleteValue(&pstRoot);
	}
	if ( m_pstState )
	{
		delete (stParsing*)m_pstState;
		m_pstState = NULL;
	}
}

bool CJJson::IsValid()
{
	if ( JJSON_STATE() && JJSON_STATE()->state==JJSON_INVALID ) return false;
	else return true;
}

bool CJJson::Parse(LPCTSTR szBuf)
{
	Close();
	json_jpi_init(JJSON_STATE());

	switch (m_iLastErr = json_parse_fragment(JJSON_STATE(),(char*)szBuf))
	{
	case JSON_OK:
	case JSON_WAITING_FOR_EOF:
	case JSON_INCOMPLETE_DOCUMENT:
		break;
	default:
		if ( JJSON_STATE()->cursor )
		{
			json_free_value(&(JJSON_STATE()->cursor));
		}
		return false;
	}
	return true;
}

char* CJJson::TreeToString(PSTJJsonValue item)
{
	char*	szOut	= NULL;
	m_iLastErr = json_tree_to_string( (json_t*)item, &szOut );
	if ( JSON_OK!=m_iLastErr ) return NULL;
	else return szOut;
}

bool CJJson::Close()
{
	if ( IsValid() )
	{
		if ( JJSON_STATE()->cursor )
		{
			json_free_value(&(JJSON_STATE()->cursor));
		}
		JJSON_STATE()->state	= JJSON_INVALID;
	}
	m_pstIntoItem	= NULL;					// 进入的项

	return true;
}

// 根结点
PSTJJsonValue CJJson::GetRootItem()
{
	if ( IsValid()==false ) return NULL;
	return (PSTJJsonValue)(JJSON_STATE()->cursor);
}

// 父结点
PSTJJsonValue CJJson::GetParentItem(PSTJJsonValue item)
{
	if ( item==NULL ) return NULL;

	return (PSTJJsonValue)(item->m_pParent);
}

// 子结点
PSTJJsonValue CJJson::GetChildItem(PSTJJsonValue item)
{
	if ( item==NULL ) return NULL;

	return (PSTJJsonValue)(item->m_pChild);
}

// 上一兄弟结点
PSTJJsonValue CJJson::GetPrevSibling(PSTJJsonValue item)
{
	if ( item==NULL ) return NULL;

	return (PSTJJsonValue)(item->m_pPrevious);
}

// 下一兄弟结点
PSTJJsonValue CJJson::GetNextSibling(PSTJJsonValue item)
{
	if ( item==NULL ) return NULL;

	return (PSTJJsonValue)(item->m_pNext);
}

// 查找子结点
PSTJJsonValue CJJson::FindSubItem(PSTJJsonValue item,LPCTSTR lpszSub)
{
	if ( item==NULL || lpszSub==NULL ) return NULL;
	PSTJJsonValue	itemSub	= NULL;

	itemSub = item->m_pChild;
	while(itemSub)
	{
		if ( itemSub->m_szText && strcmp(itemSub->m_szText,lpszSub)==0 )
		{
			return itemSub->m_pChild;
		}
		itemSub = itemSub->m_pNext;
	}

	return NULL;
}


// 获取值 begin
bool CJJson::GetValue(PSTJJsonValue item,char* strRet)
{
	if ( item==NULL ) return false;

	if ( item->m_szText )
	{
		strcpy(strRet,item->m_szText);
		return true;
	}
	else return false;
}

// 将值转化为区域类型
bool CJJson::GetRectValue(PSTJJsonValue item,LPRECT lpRect)
{
	char		strText[128]= {0};
	int			iX			= 0;
	int			iY			= 0;
	int			iWidth		= 0;
	int			iHeight		= 0;
	if ( item==NULL || lpRect==NULL ) return false;

	if ( GetValue(item,strText)==false ) return false;
	sscanf(strText,"%d,%d,%d,%d",&iX,&iY,&iWidth,&iHeight);

	lpRect->left	= iX;
	lpRect->top		= iY;
	lpRect->right	= iX+iWidth;
	lpRect->bottom	= iY+iHeight;
	return true;
}

// 将值转化为RGB类型
COLORREF CJJson::GetRGBValue(PSTJJsonValue item)
{
	COLORREF	clrRet		= RGB(0,0,0);
	int			iRed		= 0;
	int			iGreen		= 0;
	int			iBlue		= 0;
	char		strText[128]= {0};
	if ( item==NULL ) return clrRet;

	if ( GetValue(item,strText)==false ) return RGB(0,0,0);
	sscanf(strText,"RGB(%d,%d,%d)",&iRed,&iGreen,&iBlue);
	clrRet = RGB(iRed,iGreen,iBlue);
	return clrRet;
}

// 将值转化为int类型
int CJJson::GetIntValue(PSTJJsonValue item)
{
	int			iRet		= 0;
	char		strText[128]= {0};
	if ( item==NULL ) return iRet;

	if ( GetValue(item,strText)==false ) strcpy(strText,"");
	iRet = atoi(strText);
	return iRet;
}

// 将值转化为long类型
long CJJson::GetLongValue(PSTJJsonValue item)
{
	long		lRet		= 0;
	char		strText[128]= {0};
	if ( item==NULL ) return lRet;

	if ( GetValue(item,strText)==false ) strcpy(strText,"");
	lRet = atol(strText);
	return lRet;
}

// 将值转化为路径
LPCTSTR CJJson::GetPathValue(PSTJJsonValue item,LPCTSTR lpszPrev,char* strRet)
{
	char		strText[MAX_PATH]	= {0};
	if ( item==NULL ) return NULL;

	if ( GetValue(item,strText)==false ) return NULL;
	if ( lpszPrev ) sprintf(strRet,"%s%s",lpszPrev,strText);
	else strcpy(strRet,strText);
	return strRet;
}

// 将值转化为bmp类型
#if (defined _WIN32) || (defined _WIN64)
HBITMAP CJJson::GetBmpValue(PSTJJsonValue item,LPCTSTR lpszPrev)
{
	HBITMAP		hbmpRet				= NULL;
	char		strText[MAX_PATH]	= {0};
	char		strText2[MAX_PATH]	= {0};
	if ( item==NULL ) return hbmpRet;

	if ( GetValue(item,strText2)==false ) return NULL;
	if ( lpszPrev ) sprintf(strText,"%s%s",lpszPrev,strText2);
	else strcpy(strText,strText2);
	hbmpRet	= (HBITMAP)LoadImage(NULL,strText,IMAGE_BITMAP,0,0,LR_LOADFROMFILE|LR_CREATEDIBSECTION|LR_SHARED);
	return hbmpRet;
}

// 将值转化为Cursor类型
HCURSOR CJJson::GetCursorValue(PSTJJsonValue item,LPCTSTR lpszPrev)
{
	HCURSOR		hCurRet				= NULL;
	char		strText[MAX_PATH]	= {0};
	char		strText2[MAX_PATH]	= {0};
	if ( item==NULL ) return hCurRet;

	if ( GetValue(item,strText2)==false ) return NULL;
	if ( lpszPrev ) sprintf(strText,"%s%s",lpszPrev,strText2);
	else strcpy(strText,strText2);
	hCurRet	= LoadCursorFromFile(strText);
	return hCurRet;
}

// 将值转化为ICON类型
HICON CJJson::GetIconValue(PSTJJsonValue item,LPCTSTR lpszPrev)
{
	HICON		hiconRet			= NULL;
	char		strText[MAX_PATH]	= {0};
	char		strText2[MAX_PATH]	= {0};
	if ( item==NULL ) return hiconRet;

	if ( GetValue(item,strText2)==false ) return NULL;
	if ( lpszPrev ) sprintf(strText,"%s%s",lpszPrev,strText2);
	else strcpy(strText,strText2);
	hiconRet	= (HICON)LoadImage(NULL,strText,IMAGE_ICON,0,0,LR_LOADFROMFILE|LR_CREATEDIBSECTION|LR_SHARED);
	return hiconRet;
}
#endif
// 获取值 end



// 获取值 begin
bool CJJson::IntoItem(PSTJJsonValue item)
{
	m_pstIntoItem = item;
	return true;
}
// 获取值
bool CJJson::GetValue(LPCTSTR lpszKey,char* strRet)
{
	if ( m_pstIntoItem==NULL ) return false;

	PSTJJsonValue	itemFind	= NULL;
	itemFind = FindSubItem(m_pstIntoItem,lpszKey);
	if ( itemFind==NULL ) return false;
	
	strcpy(strRet,itemFind->m_szText);
	return true;
}
// 将值转化为区域类型
bool CJJson::GetRectValue(LPCTSTR lpszKey,LPRECT lpRect)
{
	char		strText[128]= {0};
	int			iX			= 0;
	int			iY			= 0;
	int			iWidth		= 0;
	int			iHeight		= 0;

	if ( m_pstIntoItem==NULL || lpRect==NULL ) return false;
	
	if ( GetValue(FindSubItem(m_pstIntoItem,lpszKey),strText)==false ) return false;
	sscanf(strText,"%d,%d,%d,%d",&iX,&iY,&iWidth,&iHeight);

	lpRect->left	= iX;
	lpRect->top		= iY;
	lpRect->right	= iX+iWidth;
	lpRect->bottom	= iY+iHeight;
	return true;
}
// 将值转化为RGB类型
COLORREF CJJson::GetRGBValue(LPCTSTR lpszKey)
{
	COLORREF	clrRet		= RGB(0,0,0);
	int			iRed		= 0;
	int			iGreen		= 0;
	int			iBlue		= 0;
	char		strText[128]= {0};
	if ( m_pstIntoItem==NULL ) return clrRet;

	if ( GetValue(FindSubItem(m_pstIntoItem,lpszKey),strText)==false ) return RGB(0,0,0);
	sscanf(strText,"RGB(%d,%d,%d)",&iRed,&iGreen,&iBlue);
	clrRet = RGB(iRed,iGreen,iBlue);
	return clrRet;
}
// 将值转化为int类型
int CJJson::GetIntValue(LPCTSTR lpszKey)
{
	int			iRet		= 0;
	char		strText[128]= {0};
	if ( m_pstIntoItem==NULL ) return iRet;

	if ( GetValue(FindSubItem(m_pstIntoItem,lpszKey),strText)==false ) strcpy(strText,"");
	iRet = atoi(strText);
	return iRet;
}

// 将值转化为long类型
long CJJson::GetLongValue(LPCTSTR lpszKey)
{
	long		lRet		= 0;
	char		strText[128]= {0};
	if ( m_pstIntoItem==NULL ) return lRet;

	if ( GetValue(FindSubItem(m_pstIntoItem,lpszKey),strText)==false ) strcpy(strText,"");
	lRet = atol(strText);
	return lRet;
}

// 将值转化为路径
LPCTSTR CJJson::GetPathValue(LPCTSTR lpszKey,LPCTSTR lpszPrev,char* strRet)
{
	char		strText[128]= {0};
	if ( m_pstIntoItem==NULL ) return NULL;

	if ( GetValue(FindSubItem(m_pstIntoItem,lpszKey),strText)==false ) return NULL;
	if ( lpszPrev ) sprintf(strRet,"%s%s",lpszPrev,strText);
	else strcpy(strRet,strText);
	return strRet;
}

// 将值转化为bmp类型
#if (defined _WIN32) || (defined _WIN64)
HBITMAP CJJson::GetBmpValue(LPCTSTR lpszKey,LPCTSTR lpszPrev)
{
	HBITMAP		hbmpRet			= NULL;
	char		strText[128]	= {0};
	char		strText2[128]	= {0};
	if ( m_pstIntoItem==NULL ) return hbmpRet;

	if ( GetValue(FindSubItem(m_pstIntoItem,lpszKey),strText2)==false ) return NULL;
	if ( lpszPrev ) sprintf(strText,"%s%s",lpszPrev,strText2);
	else strcpy(strText,strText2);
	hbmpRet	= (HBITMAP)LoadImage(NULL,strText,IMAGE_BITMAP,0,0,LR_LOADFROMFILE|LR_CREATEDIBSECTION|LR_SHARED);
	return hbmpRet;
}

// 将值转化为Cursor类型
HCURSOR CJJson::GetCursorValue(LPCTSTR lpszKey,LPCTSTR lpszPrev)
{
	HCURSOR		hCurRet			= NULL;
	char		strText[128]	= {0};
	char		strText2[128]	= {0};
	if ( m_pstIntoItem==NULL ) return hCurRet;

	if ( GetValue(FindSubItem(m_pstIntoItem,lpszKey),strText2)==false ) return NULL;
	if ( lpszPrev ) sprintf(strText,"%s%s",lpszPrev,strText2);
	else strcpy(strText,strText2);
	hCurRet	= LoadCursorFromFile(strText);
	return hCurRet;
}

// 将值转化为ICON类型
HICON CJJson::GetIconValue(LPCTSTR lpszKey,LPCTSTR lpszPrev)
{
	HICON		hiconRet		= NULL;
	char		strText[128]	= {0};
	char		strText2[128]	= {0};
	if ( m_pstIntoItem==NULL ) return hiconRet;

	if ( GetValue(FindSubItem(m_pstIntoItem,lpszKey),strText2)==false ) return NULL;
	if ( lpszPrev ) sprintf(strText,"%s%s",lpszPrev,strText2);
	else strcpy(strText,strText2);
	hiconRet	= (HICON)LoadImage(NULL,strText,IMAGE_ICON,0,0,LR_LOADFROMFILE|LR_CREATEDIBSECTION|LR_SHARED);
	return hiconRet;
}
#endif
bool CJJson::OutItem()
{
	m_pstIntoItem = NULL;
	return true;
}
// 获取值 end


// 添加值 begin
PSTJJsonValue CJJson::InsertValue(eJJsonValueType eType,PSTJJsonValue pstParent, PSTJJsonValue pstInsertAfter)
{
	PSTJJsonValue	pstNew	= NULL;
	
	pstNew = (PSTJJsonValue)json_new_value((json_value_type)eType);
	if ( pstNew )
	{
		if ( pstParent ) json_insert_child((json_t*)pstParent,(json_t*)pstNew);
		SetItemAfterItem(pstNew,pstInsertAfter);
	}
	if ( JJSON_STATE()->state==JJSON_INVALID )
	{
		json_jpi_init(JJSON_STATE());
		JJSON_STATE()->cursor = (json_t*)pstNew;
	}
	return pstNew;
}

PSTJJsonValue CJJson::InsertArray(const char* szTextItem, PSTJJsonValue pstParent, PSTJJsonValue pstInsertAfter)
{
	PSTJJsonValue	pstNew		= NULL;
	PSTJJsonValue	pstString	= NULL;
	
	if ( szTextItem && strlen(szTextItem)>0 )
	{
		pstString = (PSTJJsonValue)json_new_string(szTextItem);
		if ( pstParent ) json_insert_child((json_t*)pstParent,(json_t*)pstString);
		SetItemAfterItem(pstString,pstInsertAfter);
		pstParent = pstString;
	}

	pstNew = (PSTJJsonValue)json_new_array();
	if ( pstNew ) json_insert_child((json_t*)pstParent,(json_t*)pstNew);

	return pstNew;
}

PSTJJsonValue CJJson::InsertString(const char* szTextItem, const char* str, PSTJJsonValue pstParent, PSTJJsonValue pstInsertAfter)
{
	PSTJJsonValue	pstNew	= NULL;
	PSTJJsonValue	pstObj	= NULL;
	
	if (pstParent->m_eType==JJSON_OBJECT) pstObj = pstParent;
	else
	{
		pstObj = (PSTJJsonValue)json_new_object();
		if ( pstObj==NULL ) return NULL;
		if ( pstParent ) json_insert_child((json_t*)pstParent,(json_t*)pstObj);
		SetItemAfterItem(pstObj,pstInsertAfter);
	}

	pstNew = (PSTJJsonValue)json_new_string(str);
	if ( pstNew ) json_insert_pair_into_object((json_t*)pstObj,szTextItem,(json_t*)pstNew);

	if ( JJSON_STATE()->state==JJSON_INVALID )
	{
		json_jpi_init(JJSON_STATE());
		JJSON_STATE()->cursor = (json_t*)pstObj;
	}
	return pstObj;
}

PSTJJsonValue CJJson::InsertNumber(const char* szTextItem, const char* szNum, PSTJJsonValue pstParent, PSTJJsonValue pstInsertAfter)
{
	PSTJJsonValue	pstNew	= NULL;
	PSTJJsonValue	pstObj	= NULL;
	
	if (pstParent->m_eType==JJSON_OBJECT) pstObj = pstParent;
	else
	{
		pstObj = (PSTJJsonValue)json_new_object();
		if ( pstObj==NULL ) return NULL;
		if ( pstParent ) json_insert_child((json_t*)pstParent,(json_t*)pstObj);
		SetItemAfterItem(pstObj,pstInsertAfter);
	}

	if (szNum==NULL || strlen(szNum)==0) pstNew = (PSTJJsonValue)json_new_string("0");
	pstNew = (PSTJJsonValue)json_new_number(szNum);
	if ( pstNew ) json_insert_pair_into_object((json_t*)pstObj,szTextItem,(json_t*)pstNew);

	if ( JJSON_STATE()->state==JJSON_INVALID )
	{
		json_jpi_init(JJSON_STATE());
		JJSON_STATE()->cursor = (json_t*)pstObj;
	}
	return pstObj;
}
// 添加值 end

// 删除值 begin
void CJJson::DeleteValue(PSTJJsonValue* ppItem)
{
	if ( *ppItem )
	{
		json_free_value((json_t**)ppItem);
	}
}
// 删除值 end

// 设置兄弟结点
void CJJson::SetItemAfterItem(PSTJJsonValue pstItem,PSTJJsonValue pstInsertAfter)
{
	if ( pstItem==NULL ) return;

	if ( pstInsertAfter )
	{
		pstItem->m_pPrevious	= pstInsertAfter;
		pstItem->m_pNext		= pstInsertAfter->m_pNext;
		if ( pstInsertAfter->m_pNext )
		{
			pstInsertAfter->m_pNext->m_pPrevious = pstItem;
			pstInsertAfter->m_pNext = pstItem;
		}
	}
}

