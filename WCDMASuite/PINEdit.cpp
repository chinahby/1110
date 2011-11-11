// PBEdit.cpp : implementation file
//

#include "stdafx.h"
#include "sunnycat.h"
#include "PINEdit.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

/////////////////////////////////////////////////////////////////////////////
// CPBEdit

CPINEdit::CPINEdit()
{
}

CPINEdit::~CPINEdit()
{
}


BEGIN_MESSAGE_MAP(CPINEdit, CEdit)
	//{{AFX_MSG_MAP(CPINEdit)
	ON_WM_CHAR()
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CPINEdit message handlers

void CPINEdit::OnChar(UINT nChar, UINT nRepCnt, UINT nFlags) 
{
	// TODO: Add your message handler code here and/or call default
	if((nChar >= '0'&& nChar <= '9') || (nChar == '\b'))
	CEdit::OnChar(nChar, nRepCnt, nFlags);
}

