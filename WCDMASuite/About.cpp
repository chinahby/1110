// About.cpp : implementation file
//

#include "stdafx.h"
#include "SunnyCat.h"
#include "About.h"
#include "Util.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif


/////////////////////////////////////////////////////////////////////////////
// CAbout dialog


CAbout::CAbout(CWnd* pParent /*=NULL*/)
	: CBmpBkDialog(CAbout::IDD, pParent)
{
	//{{AFX_DATA_INIT(CAbout)
		// NOTE: the ClassWizard will add member initialization here
	//}}AFX_DATA_INIT
}


void CAbout::DoDataExchange(CDataExchange* pDX)
{
	CBmpBkDialog::DoDataExchange(pDX);
	//{{AFX_DATA_MAP(CAbout)
		// NOTE: the ClassWizard will add DDX and DDV calls here
	//}}AFX_DATA_MAP
}


BEGIN_MESSAGE_MAP(CAbout, CBmpBkDialog)
	//{{AFX_MSG_MAP(CAbout)
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CAbout message handlers

void CAbout::OnOK() 
{
	// TODO: Add extra validation here
	
	CBmpBkDialog::OnOK();
}

BOOL CAbout::OnInitDialog() 
{
	CBmpBkDialog::OnInitDialog();
	SetWindowText(LoadStringEx(IDS_ABOUT_TEXT));
	// TODO: Add extra initialization here
	CWnd* pWnd = GetDlgItem(IDC_APPNAME);
	pWnd->SetWindowText(LoadStringEx(IDS_APPNAME));
	GetDlgItem(IDC_COPYRIGHT)->SetWindowText(LoadStringEx(IDS_COPYREGHT));
	GetDlgItem(IDOK)->SetWindowText(LoadStringEx(IDS_OK));
	return TRUE;  // return TRUE unless you set the focus to a control
	              // EXCEPTION: OCX Property Pages should return FALSE
}
