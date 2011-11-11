// EnFileDialog.cpp : implementation file
//

#include "stdafx.h"
#include "EnFileDialog.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

/////////////////////////////////////////////////////////////////////////////
// CEnFileDialog

	IMPLEMENT_DYNAMIC(CEnFileDialog, CFileDialog)
		
	CEnFileDialog::CEnFileDialog(BOOL bOpenFileDialog, LPCTSTR lpszDefExt, LPCTSTR lpszFileName,
		DWORD dwFlags, LPCTSTR lpszFilter, CWnd* pParentWnd) :
	CFileDialog(bOpenFileDialog, lpszDefExt, lpszFileName, dwFlags, lpszFilter, pParentWnd)
	{
	}
	
	
	BEGIN_MESSAGE_MAP(CEnFileDialog, CFileDialog)
		//{{AFX_MSG_MAP(CEnFileDialog)
		ON_WM_CLOSE()
		//}}AFX_MSG_MAP
	END_MESSAGE_MAP()
		
		
	void CEnFileDialog::OnClose() 
	{
		::SendMessage(GetParent()->m_hWnd,WM_COMMAND,IDCANCEL,0) ;
	}