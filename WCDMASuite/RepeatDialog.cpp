// RepeatDialog.cpp : implementation file
//

#include "stdafx.h"
#include "sunnycat.h"
#include "RepeatDialog.h"
#include "Util.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

/////////////////////////////////////////////////////////////////////////////
// CRepeatDialog dialog


CRepeatDialog::CRepeatDialog(CWnd* pParent /*=NULL*/)
	: CBmpBkDialog(CRepeatDialog::IDD, pParent)
{
	//{{AFX_DATA_INIT(CRepeatDialog)
		// NOTE: the ClassWizard will add member initialization here
	//}}AFX_DATA_INIT
	m_bDrawToolBar = false;
	m_repeat = REPEAT_OK;
}


void CRepeatDialog::DoDataExchange(CDataExchange* pDX)
{
	CBmpBkDialog::DoDataExchange(pDX);
	//{{AFX_DATA_MAP(CRepeatDialog)
	DDX_Control(pDX, IDC_STATIC_REPEAT, m_ctrlRepeat);
	//}}AFX_DATA_MAP
}


BEGIN_MESSAGE_MAP(CRepeatDialog, CBmpBkDialog)
	//{{AFX_MSG_MAP(CRepeatDialog)
	ON_BN_CLICKED(IDC_BUTTON_ALL_OK, OnButtonAllOk)
	ON_BN_CLICKED(IDC_BUTTON_ALL_NO, OnButtonAllNo)
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CRepeatDialog message handlers

void CRepeatDialog::OnOK() 
{
	// TODO: Add extra validation here
	m_repeat = REPEAT_OK;
	CBmpBkDialog::OnOK();
}

void CRepeatDialog::OnCancel() 
{
	// TODO: Add extra cleanup here
	m_repeat = REPEAT_NO;
	CBmpBkDialog::OnCancel();
}

void CRepeatDialog::OnButtonAllOk() 
{
	// TODO: Add your control notification handler code here
	m_repeat = REPEAT_OK_ALL;
	CBmpBkDialog::OnOK();
}

void CRepeatDialog::OnButtonAllNo() 
{
	// TODO: Add your control notification handler code here
	m_repeat = REPEAT_NO_ALL;
	CBmpBkDialog::OnCancel();
}

BOOL CRepeatDialog::OnInitDialog() 
{
	CBmpBkDialog::OnInitDialog();
	SetWindowText(LoadStringEx(IDS_REPEAT_DLG));
	GetDlgItem(IDC_BUTTON_ALL_OK)->SetWindowText(LoadStringEx(IDS_YES_ALL));
	GetDlgItem(IDC_BUTTON_ALL_NO)->SetWindowText(LoadStringEx(IDS_NO_ALL));
	GetDlgItem(IDOK)->SetWindowText(LoadStringEx(IDS_YES));
	GetDlgItem(IDCANCEL)->SetWindowText(LoadStringEx(IDS_NO));
	
	m_ctrlRepeat.SetWindowText(LoadStringEx(IDS_REPEAT_NOTICE));
	return TRUE;  // return TRUE unless you set the focus to a control
	              // EXCEPTION: OCX Property Pages should return FALSE
}
