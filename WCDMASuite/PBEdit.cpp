// PBEdit.cpp : implementation file
//

#include "stdafx.h"
#include "sunnycat.h"
#include "PBEdit.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

/////////////////////////////////////////////////////////////////////////////
// CPBEdit

CPBEdit::CPBEdit()
{
}

CPBEdit::~CPBEdit()
{
}


BEGIN_MESSAGE_MAP(CPBEdit, CEdit)
	//{{AFX_MSG_MAP(CPBEdit)
	ON_WM_CHAR()
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CPBEdit message handlers

void CPBEdit::OnChar(UINT nChar, UINT nRepCnt, UINT nFlags) 
{
	// TODO: Add your message handler code here and/or call default
	if((nChar >= '0'&& nChar <= '9')||(nChar == '+')||(nChar == '\b')||(nChar == '#')||(nChar == '*'))
	CEdit::OnChar(nChar, nRepCnt, nFlags);
}
