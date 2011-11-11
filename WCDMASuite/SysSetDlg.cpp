// SysSetDlg.cpp : implementation file
//

#include "stdafx.h"
#include "SunnyCat.h"
#include "SysSetDlg.h"
#include "GlobalDataCenter.h"
#include "Porting.h"
#include "Util.h"
#include "DevEnum.h"
#include <atlbase.h>

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

/////////////////////////////////////////////////////////////////////////////
// CSysSetDlg dialog


CSysSetDlg::CSysSetDlg(CWnd* pParent /*=NULL*/)
	: CBmpBkDialog(CSysSetDlg::IDD, pParent)
{
	//{{AFX_DATA_INIT(CSysSetDlg)
	m_bAutoRun = FALSE;
	m_bMinToTask = FALSE;
	m_bShowWindow = FALSE;
	m_bAutoUpdate = FALSE;
	//}}AFX_DATA_INIT
}


void CSysSetDlg::DoDataExchange(CDataExchange* pDX)
{
	CBmpBkDialog::DoDataExchange(pDX);
	//{{AFX_DATA_MAP(CSysSetDlg)
	DDX_Control(pDX, IDC_COMBO_COM, m_selCom);
	DDX_Check(pDX, IDC_CHECK_AUTO, m_bAutoRun);
	DDX_Check(pDX, IDC_CHECK_MIN, m_bMinToTask);
	DDX_Check(pDX, IDC_CHECK_SHOWWND, m_bShowWindow);
	DDX_Check(pDX, IDC_CHECK_AUTOUPDATE, m_bAutoUpdate);
	//}}AFX_DATA_MAP
}


BEGIN_MESSAGE_MAP(CSysSetDlg, CBmpBkDialog)
	//{{AFX_MSG_MAP(CSysSetDlg)
	ON_BN_CLICKED(IDC_CHECK_AUTO, OnCheckAuto)
	ON_BN_CLICKED(IDC_CHECK_MIN, OnCheckMin)
	ON_BN_CLICKED(IDC_CHECK_SHOWWND, OnCheckShowwnd)
	ON_BN_CLICKED(IDC_CHECK_AUTOUPDATE, OnCheckAutoupdate)
	ON_CBN_DROPDOWN(IDC_COMBO_COM, OnDropdownComboCom)
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CSysSetDlg message handlers

BOOL CSysSetDlg::OnInitDialog() 
{
	
	CBmpBkDialog::OnInitDialog();

	
	
	// TODO: Add extra initialization here
	GetDlgItem(IDC_CHECK_AUTO)->SetWindowText(LoadStringEx(IDS_SYSSET_AUTO));
	GetDlgItem(IDC_CHECK_MIN)->SetWindowText(LoadStringEx(IDS_SYSSET_MIN));
	GetDlgItem(IDC_CHECK_SHOWWND)->SetWindowText(LoadStringEx(IDS_SYSSET_SHOW));
	GetDlgItem(IDC_CHECK_AUTOUPDATE)->SetWindowText(LoadStringEx(IDS_SYSSET_UPDATE));
	GetDlgItem(IDC_STATIC_SEL_COM)->SetWindowText(LoadStringEx(IDS_SEL_COM));
//#ifndef COM_MODEM
	GetDlgItem(IDC_STATIC_SEL_COM)->ShowWindow(SW_HIDE);
	GetDlgItem(IDC_COMBO_COM)->ShowWindow(SW_HIDE);
//#endif
	GetDlgItem(IDC_STATIC_VOICE)->SetWindowText(LoadStringEx(IDS_VOICE));
	GetDlgItem(IDC_RADIO_VOICEOUT)->SetWindowText(LoadStringEx(IDS_VOICEOUT));
	GetDlgItem(IDC_RADIO_VOICEPC)->SetWindowText(LoadStringEx(IDS_VOICEPC));
	GetDlgItem(IDC_STATIC_EAR)->SetWindowText(LoadStringEx(IDS_VOICE_SWITCH));
	
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
	
	DWORD dwValue = 0;

	if(reg.QueryValue(dwValue, TEXT("MinToTask")) == ERROR_SUCCESS)
	{
		if(dwValue >0 )
			m_bMinToTask = TRUE;
		else
			m_bMinToTask = FALSE;
	}

	if(reg.QueryValue(dwValue, TEXT("AutoUpdate")) == ERROR_SUCCESS)
	{
		if(dwValue >0 )
			m_bAutoUpdate = TRUE;
		else
			m_bAutoUpdate = FALSE;
	}

	if(reg.QueryValue(dwValue, TEXT("AutoShowWindow")) == ERROR_SUCCESS)
	{
		if(dwValue >0 )
			m_bShowWindow = TRUE;
		else
			m_bShowWindow = FALSE;
	}

	dwValue = 0;
	CWnd *pWnd = GetDlgItem(IDC_RADIO_VOICEOUT);
	CButton *pButton = static_cast<CButton*>(pWnd);
	reg.QueryValue(dwValue,TEXT("VoiceChanel"));
	if(dwValue != 2)
	{
		pButton->SetCheck(1);
	}
	else
	{
		pButton->SetCheck(0);
	}
	pWnd = GetDlgItem(IDC_RADIO_VOICEPC);
	pButton = static_cast<CButton*>(pWnd);
	if(dwValue != 2)
	{
		pButton->SetCheck(0);
	}
	else
	{
		pButton->SetCheck(1);
	}
	

	TCHAR strValue[1024] = {0};
	DWORD iCount = sizeof(strValue)-1;
	if(reg.QueryValue(strValue, TEXT("Com"), &iCount) == ERROR_SUCCESS)
	{
		 GetDlgItem(IDC_COMBO_COM)->SetWindowText(strValue);
	}
	else
	{
		GetDlgItem(IDC_COMBO_COM)->SetWindowText(TEXT("COM3"));		
	}
	regPath = TEXT("SOFTWARE\\Microsoft\\Windows\\CurrentVersion\\Run");
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
	CString strRunKey;
	CString strProduct = GetProduct();
//	if(strProduct == "-p")
//		strRunKey = "SunnyCat";
//	else if(strProduct == "-u") // USB_MODEM
		strRunKey = "WCDMA Modem";
//	else if(strProduct == "-c")
//	strRunKey = "M500ECOM";

	if(reg.QueryValue(szValue, strRunKey, & nCount) == ERROR_SUCCESS)
	{
		m_bAutoRun = TRUE;
	}

	

	reg.Close();
	UpdateData(FALSE);
	
	return TRUE;  // return TRUE unless you set the focus to a control
	              // EXCEPTION: OCX Property Pages should return FALSE
}

void CSysSetDlg::OnCheckAuto() 
{
	// TODO: Add your control notification handler code here
	UpdateData(TRUE);
	SetAutoRunOnStart(m_bAutoRun?true:false) ;
}

void CSysSetDlg::OnCheckMin() 
{
	// TODO: Add your control notification handler code here
	UpdateData(TRUE);
}

void CSysSetDlg::OnCheckShowwnd() 
{
	// TODO: Add your control notification handler code here
	UpdateData(TRUE);
}

void CSysSetDlg::OnCheckAutoupdate() 
{
	// TODO: Add your control notification handler code here
	UpdateData(TRUE);
}

void CSysSetDlg::OnDropdownComboCom() 
{
	// TODO: Add your control notification handler code here	
	m_selCom.ResetContent();
	//枚举ＣＯＭ口
	//===============
	TRACE(TEXT("CSysSetDlg::OnDropdownComboCom :> 枚举ＣＯＭ口"));
	CPortList portList;
	size_t sSize = portList.Count();
	for(size_t i=0; i<sSize; ++i)
	{
		const TPortInfo& info = portList.GetAt(i);
		if(info.strCOM.Find(TEXT("COM")) != -1)
		{	
			m_selCom.AddString(info.strCOM);
		}
	
		//m_Options.m_ComPort = info.strCOM;
		//	m_ComName = info.strCOM;
		
	}
	//===============
	UpdateData(FALSE);
}
