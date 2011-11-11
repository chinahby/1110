// ApnEdit.cpp : implementation file
//

#include "stdafx.h"
#include "sunnycat.h"
#include "ApnEdit.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

/////////////////////////////////////////////////////////////////////////////
// CApnEdit

CApnEdit::CApnEdit()
{
}

CApnEdit::~CApnEdit()
{
}


BEGIN_MESSAGE_MAP(CApnEdit, CEdit)
	//{{AFX_MSG_MAP(CApnEdit)
	ON_WM_CHAR()
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CApnEdit message handlers

void CApnEdit::OnChar(UINT nChar, UINT nRepCnt, UINT nFlags) 
{
	// TODO: Add your message handler code here and/or call default
	if(nChar != '`')
		CEdit::OnChar(nChar, nRepCnt, nFlags);
}
