// SetRingDlg.cpp : implementation file
//

#include "stdafx.h"
#include "SunnyCat.h"
#include "SetRingDlg.h"
#include "Util.h"
#include <atlbase.h>
#include "Porting.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

/////////////////////////////////////////////////////////////////////////////
// CSetRingDlg dialog


CSetRingDlg::CSetRingDlg(CWnd* pParent /*=NULL*/)
	: CBmpBkDialog(CSetRingDlg::IDD, pParent)
{
	//{{AFX_DATA_INIT(CSetRingDlg)
		// NOTE: the ClassWizard will add member initialization here
	//}}AFX_DATA_INIT
}


void CSetRingDlg::DoDataExchange(CDataExchange* pDX)
{
	CBmpBkDialog::DoDataExchange(pDX);
	//{{AFX_DATA_MAP(CSetRingDlg)
		// NOTE: the ClassWizard will add DDX and DDV calls here
	//}}AFX_DATA_MAP
}


BEGIN_MESSAGE_MAP(CSetRingDlg, CBmpBkDialog)
	//{{AFX_MSG_MAP(CSetRingDlg)
	ON_BN_CLICKED(IDC_BUTTON_SETCALLRING, OnButtonSetcallring)
	ON_BN_CLICKED(IDC_BUTTON_SETSMSRING, OnButtonSetsmsring)
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CSetRingDlg message handlers

void CSetRingDlg::OnButtonSetcallring() 
{
	// TODO: Add your control notification handler code here
	CString strFilter = LoadStringEx(IDS_WAVEFILE_TYPE);
	CString strFilename;
	strFilter += "|*.wav||";
	CFileDialog dlg(
		true,		//OPEN 
		TEXT("*.wav"),	//Default filter extern name
		strFilename, //file name
		OFN_EXPLORER | OFN_FILEMUSTEXIST,//flag
		strFilter,
		this);

	if(dlg.DoModal() == IDOK)
	{
		CWnd* pWnd = NULL;
		pWnd = GetDlgItem(IDC_EDIT_CALLRING);

		pWnd->SetWindowText(dlg.GetPathName());
		
		m_strCallRing = dlg.GetPathName();
	}

}

void CSetRingDlg::OnButtonSetsmsring() 
{
	// TODO: Add your control notification handler code here
	CString strFilter = LoadStringEx(IDS_WAVEFILE_TYPE);
	CString strFilename;
	strFilter += "|*.wav||";
	CFileDialog dlg(
		true,		//OPEN 
		TEXT("*.wav"),	//Default filter extern name
		strFilename, //file name
		OFN_EXPLORER | OFN_FILEMUSTEXIST,//flag
		strFilter,
		this);

	if(dlg.DoModal() == IDOK)
	{
		CWnd* pWnd = NULL;
		pWnd = GetDlgItem(IDC_EDIT_SMSRING);

		pWnd->SetWindowText(dlg.GetPathName());

		m_strSMSRing = dlg.GetPathName();
	}
}

BOOL CSetRingDlg::OnInitDialog() 
{
	CBmpBkDialog::OnInitDialog();
	
	GetDlgItem(IDC_COMING_TONE)->SetWindowText(LoadStringEx(IDS_COMING_TONE));
	GetDlgItem(IDC_SMS_TONE)->SetWindowText(LoadStringEx(IDS_SMS_TONE));
	GetDlgItem(IDC_BUTTON_SETCALLRING)->SetWindowText(LoadStringEx(IDS_BUTTON_SET));
	GetDlgItem(IDC_BUTTON_SETSMSRING)->SetWindowText(LoadStringEx(IDS_BUTTON_SET));
	
	// TODO: Add extra initialization here
	CRegKey reg;
    
    CString regPath = GetConfigString();

	if(reg.Open(HKEY_LOCAL_MACHINE, regPath) != ERROR_SUCCESS) //打开成功
	{
		reg.Create(HKEY_LOCAL_MACHINE, regPath);		
		if(reg.Open(HKEY_LOCAL_MACHINE, regPath) != ERROR_SUCCESS)
		{
			return TRUE;
		}
	}
	
	TCHAR szValue[1024] = {0};
	DWORD nCount = sizeof(szValue)-1;
	if(reg.QueryValue(szValue, TEXT("CallRing"), & nCount) == ERROR_SUCCESS)
	{
		m_strCallRing = szValue;
	}
	else
	{
		m_strCallRing = GetAppPath() + "music\\ring.wav";
		
	}

	nCount = sizeof(szValue)-1;
	
	if(reg.QueryValue(szValue, TEXT("SMSRing"), & nCount) == ERROR_SUCCESS)
	{
		m_strSMSRing = szValue;
	}
	else
	{
		m_strSMSRing = GetAppPath() + "music\\sms.wav";
	}

	CWnd* pWnd = NULL;
	pWnd = GetDlgItem(IDC_EDIT_CALLRING);
	pWnd->SetWindowText(m_strCallRing);

	pWnd = GetDlgItem(IDC_EDIT_SMSRING);
	pWnd->SetWindowText(m_strSMSRing);
	reg.Close();
	return TRUE;  // return TRUE unless you set the focus to a control
	              // EXCEPTION: OCX Property Pages should return FALSE
}