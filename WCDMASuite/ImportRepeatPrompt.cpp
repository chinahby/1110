// ImportRepeatPrompt.cpp : implementation file
//

#include "stdafx.h"
#include "sunnycat.h"
#include "ImportRepeatPrompt.h"
#include "Util.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

/////////////////////////////////////////////////////////////////////////////
// CImportRepeatPrompt dialog


CImportRepeatPrompt::CImportRepeatPrompt(CWnd* pParent /*=NULL*/)
	: CBmpBkDialog(CImportRepeatPrompt::IDD, pParent)
{
	//{{AFX_DATA_INIT(CImportRepeatPrompt)
	m_app2All = FALSE;
	//}}AFX_DATA_INIT
	m_bDrawToolBar = false;
	strPromptInfo = "";
}

CImportRepeatPrompt::CImportRepeatPrompt(CWnd* pParent,CString strRepeatInfo)
	: CBmpBkDialog(CImportRepeatPrompt::IDD, pParent)
{
	m_app2All = FALSE;
	m_bDrawToolBar = false;
	strPromptInfo = strRepeatInfo;
}

void CImportRepeatPrompt::DoDataExchange(CDataExchange* pDX)
{
	CBmpBkDialog::DoDataExchange(pDX);
	//{{AFX_DATA_MAP(CImportRepeatPrompt)
	DDX_Check(pDX, IDC_CHECK_APP_ALL, m_app2All);
	//}}AFX_DATA_MAP
}


BEGIN_MESSAGE_MAP(CImportRepeatPrompt, CBmpBkDialog)
	//{{AFX_MSG_MAP(CImportRepeatPrompt)
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CImportRepeatPrompt message handlers

BOOL CImportRepeatPrompt::OnInitDialog() 
{
	CBmpBkDialog::OnInitDialog();
	
	// TODO: Add extra initialization here
	SetWindowText(LoadStringEx(IDS_REPEAT_TEXT));
	GetDlgItem(IDC_STATIC_REPEAT_PROMPT)->SetWindowText(strPromptInfo);
	GetDlgItem(IDC_CHECK_APP_ALL)->SetWindowText(LoadStringEx(IDS_CHECK_REPEAT));
	GetDlgItem(IDOK)->SetWindowText(LoadStringEx(IDS_YES));
	GetDlgItem(IDCANCEL)->SetWindowText(LoadStringEx(IDS_NO));
	return TRUE;  // return TRUE unless you set the focus to a control
	              // EXCEPTION: OCX Property Pages should return FALSE
}

void CImportRepeatPrompt::OnCancel() 
{
	// TODO: Add extra cleanup here
	UpdateData(TRUE);
	CBmpBkDialog::OnCancel();
}
