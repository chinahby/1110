// RemovePrompt.cpp : implementation file
//

#include "stdafx.h"
#include "sunnycat.h"
#include "RemovePrompt.h"
#include "Util.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

/////////////////////////////////////////////////////////////////////////////
// CRemovePrompt dialog


CRemovePrompt::CRemovePrompt(CWnd* pParent /*=NULL*/)
	: CBmpBkDialog(CRemovePrompt::IDD,pParent)
{
	
	//{{AFX_DATA_INIT(CRemovePrompt)
		// NOTE: the ClassWizard will add member initialization here
	//}}AFX_DATA_INIT
	m_bDrawToolBar = false;
}


void CRemovePrompt::DoDataExchange(CDataExchange* pDX)
{
	CBmpBkDialog::DoDataExchange(pDX);
	//{{AFX_DATA_MAP(CRemovePrompt)
		// NOTE: the ClassWizard will add DDX and DDV calls here
	//}}AFX_DATA_MAP
}


BEGIN_MESSAGE_MAP(CRemovePrompt, CBmpBkDialog)
	//{{AFX_MSG_MAP(CRemovePrompt)
	ON_WM_PAINT()
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CRemovePrompt message handlers

BOOL CRemovePrompt::OnInitDialog() 
{
	CBmpBkDialog::OnInitDialog();
	SetWindowText(LoadStringEx(IDS_REMOVE_PROMPT));
	GetDlgItem(IDOK)->SetWindowText(LoadStringEx(IDS_OK));
	GetDlgItem(IDC_STATIC_REMOVE_CARD)->SetWindowText(LoadStringEx(IDS_ONLINE_REMOVED));
	// TODO: Add extra initialization here
	return TRUE;  // return TRUE unless you set the focus to a control
	              // EXCEPTION: OCX Property Pages should return FALSE
}

void CRemovePrompt::OnPaint() 
{
	CBmpBkDialog::OnPaint();
	CDC *pDC = GetDC();
	// TODO: Add your message handler code here
	HICON hIcon = ::LoadIcon(NULL,IDI_EXCLAMATION);

	CPoint pPoint(14+2, 14+26);
	pDC->DrawIcon(pPoint, hIcon);
	pDC->DeleteDC();
	ReleaseDC(pDC);



	// Do not call CBmpBkDialog::OnPaint() for painting messages
}
