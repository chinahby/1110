// DialNumEdit.cpp : implementation file
//

#include "stdafx.h"

#include "DialNumEdit.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

/////////////////////////////////////////////////////////////////////////////
// CDialNumEdit

CDialNumEdit::CDialNumEdit()
{
}

CDialNumEdit::~CDialNumEdit()
{
}


BEGIN_MESSAGE_MAP(CDialNumEdit, CEdit)
	//{{AFX_MSG_MAP(CDialNumEdit)
	ON_WM_CHAR()
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CDialNumEdit message handlers

void CDialNumEdit::OnChar(UINT nChar, UINT nRepCnt, UINT nFlags) 
{
	// TODO: Add your message handler code here and/or call default
	if((nChar >= '0'&& nChar <= '9')||(nChar == '+')||
		(nChar == ';')||(nChar == '\b'))
	CEdit::OnChar(nChar, nRepCnt, nFlags);
}
