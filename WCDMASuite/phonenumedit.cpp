//////////////////////////////////////////////
//Class:CPhoneNumEdit
//Des: A Editor(just Number, #, * is allowed input.)
//Author: zou.zhangfeng
//////////////////////////////////////
#include "stdafx.h"
#include "PhoneNumEdit.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

/////////////////////////////////////////////////////////////////////////////
// CPhoneNumEdit
/**
只能输入数字，#,*
*/

CPhoneNumEdit::CPhoneNumEdit()
{
}

CPhoneNumEdit::~CPhoneNumEdit()
{
}


BEGIN_MESSAGE_MAP(CPhoneNumEdit, CEdit)
	//{{AFX_MSG_MAP(CPhoneNumEdit)
	ON_WM_CHAR()
	ON_MESSAGE(WM_PASTE, OnPaste)
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

static bool IsValidPhoneNumber(const TCHAR* str, size_t strLen=0)
{
	const TCHAR* c=str;
	size_t i=0;
	strLen = strLen? strLen : 99999;
	for(;i<strLen && *c; ++i, ++c)
	{
		bool bValid =	(*c>='0' && *c<='9') || 
						(*c=='*') || (*c=='#') || 
						(*c==0X08)/*Backspace*/;
		if(!bValid)return false;
	}

	return true;
}

void CPhoneNumEdit::OnChar(UINT nChar, UINT nRepCnt, UINT nFlags) 
{
	bool bCtrlDown = ::GetKeyState(VK_CONTROL)<0 ;
	if( !bCtrlDown )
	{
		TCHAR ch = (TCHAR)nChar;
		CString str;
		GetWindowText(str);
		if(IsValidPhoneNumber(&ch, 1))
		{
			CEdit::OnChar(nChar, nRepCnt, nFlags);
		}
		return;
	}

	CEdit::OnChar(nChar, nRepCnt, nFlags);
}

class CTextClipboard//文本剪贴板
{
public:
	CTextClipboard(CWnd& wnd)//在构造函数中打开剪贴板
	{
		wnd.OpenClipboard();
	}
	~CTextClipboard()//在析构函数中关闭剪贴板
	{
		CloseClipboard();
	}

	bool GetText(CString& str)//获得剪贴板中的文本数据
	{
		HANDLE hData = ::GetClipboardData(CF_TEXT);
		if(!hData)hData = ::GetClipboardData(CF_OEMTEXT);

		bool bOK = false;
		if(hData)
		{
			str = (const char*)GlobalLock(hData);
			GlobalUnlock(hData);

			bOK = true;
		}

		return bOK;
	}

	bool SetText(const CString& str)//设置剪贴板中的文本数据
	{
		HGLOBAL handle = ::GlobalAlloc(GMEM_MOVEABLE | GMEM_DDESHARE, str.GetLength()+1);
		TCHAR* pData = (TCHAR*)::GlobalLock(handle);
		_tcscpy(pData, str);
		GlobalUnlock(handle);

		return NULL != SetClipboardData(CF_TEXT, handle);
	}
};

void CPhoneNumEdit::OnPaste(WPARAM, LPARAM)
{
	CTextClipboard clipboard(*this);

	CString str;
	if(clipboard.GetText(str) && IsValidPhoneNumber(str))
	{
		int nStartChar = 0;
		int nEndChar = 0;
		GetSel(nStartChar, nEndChar );
		if(nStartChar != nEndChar)
		{
			ReplaceSel(str, TRUE);
		}
		else
		{
			for(const TCHAR* c=str; *c; ++c)
			{
				SendMessage(WM_CHAR, *c, 0);
			}
		}
	}
}
