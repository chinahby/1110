// DlgIPDNS.cpp : implementation file
//

#include "stdafx.h"
#include "sunnycat.h"
#include "DlgIPDNS.h"
#include "Util.h"
#include <atlbase.h>
#include "Porting.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

/////////////////////////////////////////////////////////////////////////////
// CDlgIPDNS dialog


CDlgIPDNS::CDlgIPDNS(CWnd* pParent /*=NULL*/)
	: CBmpBkDialog(CDlgIPDNS::IDD, pParent)
{
	//{{AFX_DATA_INIT(CDlgIPDNS)
		// NOTE: the ClassWizard will add member initialization here
	//}}AFX_DATA_INIT
}

CDlgIPDNS::CDlgIPDNS(CWnd* pParent,std::tstring strApn)
	: CBmpBkDialog(CDlgIPDNS::IDD, pParent),m_strApn(strApn)
{
	
}

void CDlgIPDNS::DoDataExchange(CDataExchange* pDX)
{
	CBmpBkDialog::DoDataExchange(pDX);
	//{{AFX_DATA_MAP(CDlgIPDNS)
		// NOTE: the ClassWizard will add DDX and DDV calls here
	//}}AFX_DATA_MAP
}


BEGIN_MESSAGE_MAP(CDlgIPDNS, CBmpBkDialog)
	//{{AFX_MSG_MAP(CDlgIPDNS)
	ON_BN_CLICKED(IDC_RADIO_STATICDNS, OnRadioStaticdns)
	ON_BN_CLICKED(IDC_RADIO_AUTODNS, OnRadioAutodns)
	ON_BN_CLICKED(IDC_RADIO_AUTOIP, OnRadioAutoip)
	ON_BN_CLICKED(IDC_RADIO_STATICIP, OnRadioStaticip)
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CDlgIPDNS message handlers

BOOL CDlgIPDNS::OnInitDialog() 
{
	CBmpBkDialog::OnInitDialog();
	
	// TODO: Add extra initialization here
	SetWindowText(LoadStringEx(IDS_ADVANCED_SET));
	GetDlgItem(IDC_RADIO_AUTOIP)->SetWindowText(LoadStringEx(IDS_AUTO_IP));
	GetDlgItem(IDC_RADIO_STATICIP)->SetWindowText(LoadStringEx(IDS_STATIC_IP));
	GetDlgItem(IDC_RADIO_AUTODNS)->SetWindowText(LoadStringEx(IDS_AUTO_DNS));
	GetDlgItem(IDC_RADIO_STATICDNS)->SetWindowText(LoadStringEx(IDS_STATIC_DNS));
	GetDlgItem(IDC_STATIC_DNS1)->SetWindowText(LoadStringEx(IDS_DNS_1));
	GetDlgItem(IDC_STATIC_DNS2)->SetWindowText(LoadStringEx(IDS_DNS_2));
	GetDlgItem(IDOK)->SetWindowText(LoadStringEx(IDS_APN_OK));
	GetDlgItem(IDCANCEL)->SetWindowText(LoadStringEx(IDS_APN_CANCEL));
	
	InitCtrlValue();

	return TRUE;  // return TRUE unless you set the focus to a control
	              // EXCEPTION: OCX Property Pages should return FALSE
}

void CDlgIPDNS::InitCtrlValue()
{
	CRegKey reg;
    CString lpszKey =  GetConfigString()+"\\"+m_strApn.c_str(); 
    
	if(reg.Open(HKEY_LOCAL_MACHINE,lpszKey) != ERROR_SUCCESS)
	{		
		SetAutoGetIP();
		SetAutoGetDNS();
	}
	else
	{
		TCHAR IP[20] = {0};
		DWORD dwCount = sizeof(IP);
		if(reg.QueryValue(IP,TEXT("IP"),&dwCount) != ERROR_SUCCESS)
		{
			SetAutoGetIP();
		}
		else
		{
			SetStaticIPCtrlState();
			SetStaticGetIP(IP);
		}
		TCHAR DNS1[20] = {0};
		TCHAR DNS2[20] = {0};
		DWORD dwCount1 = sizeof(DNS1);
		DWORD dwCount2 = sizeof(DNS2);
		DWORD Ret1 = reg.QueryValue(DNS1,TEXT("DNS1"),&dwCount1);
		DWORD Ret2 = reg.QueryValue(DNS2,TEXT("DNS2"),&dwCount2);
		if((Ret1 != ERROR_SUCCESS)&&(Ret2 != ERROR_SUCCESS))
		{
			SetAutoGetDNS();
		}
		else
		{
			SetStaticDNSCtrlState();
			SetStaticGetDNS(DNS1,DNS2);
		}
	}
	reg.Close();
}

void CDlgIPDNS::SetStaticGetIP(const CString strIP)
{
	CWnd *pWnd = GetDlgItem(IDC_RADIO_STATICIP);
	CButton *pButton = static_cast<CButton*>(pWnd);
	pButton->SetCheck(1);

	pWnd = GetDlgItem(IDC_IPADDRESS_IP);
	pWnd->SetWindowText(strIP);
}

void CDlgIPDNS::SetStaticGetDNS(const CString strDNS1,const CString strDNS2)
{
	CWnd *pWnd = GetDlgItem(IDC_RADIO_STATICDNS);
	CButton *pButton = static_cast<CButton*>(pWnd);
	pButton->SetCheck(1);

	pWnd = GetDlgItem(IDC_IPADDRESS_DNS1);
	pWnd->SetWindowText(strDNS1);
	pWnd = GetDlgItem(IDC_IPADDRESS_DNS2);
	pWnd->SetWindowText(strDNS2);
}

void CDlgIPDNS::SetAutoDNSCtrlState()
{
	CWnd *pWnd = GetDlgItem(IDC_IPADDRESS_DNS1);
	pWnd->EnableWindow(FALSE);
	pWnd = GetDlgItem(IDC_IPADDRESS_DNS2);
	pWnd->EnableWindow(FALSE);
}

void CDlgIPDNS::SetAutoIPCtrlState()
{
	CWnd *pWnd = GetDlgItem(IDC_IPADDRESS_IP);
	pWnd->EnableWindow(FALSE);
}

void CDlgIPDNS::SetStaticDNSCtrlState()
{
	CWnd *pWnd = GetDlgItem(IDC_IPADDRESS_DNS1);
	pWnd->EnableWindow(TRUE);
	pWnd = GetDlgItem(IDC_IPADDRESS_DNS2);
	pWnd->EnableWindow(TRUE);
}

void CDlgIPDNS::SetStaticIPCtrlState()
{
	CWnd *pWnd = GetDlgItem(IDC_IPADDRESS_IP);
	pWnd->EnableWindow(TRUE);
}

void CDlgIPDNS::SetAutoGetIP()
{
	CWnd *pWnd = GetDlgItem(IDC_RADIO_AUTOIP);
	CButton *pButton = static_cast<CButton*>(pWnd);
	pButton->SetCheck(1);
	SetAutoIPCtrlState();
}

void CDlgIPDNS::SetAutoGetDNS()
{
	CWnd *pWnd = GetDlgItem(IDC_RADIO_AUTODNS);
	CButton *pButton = static_cast<CButton*>(pWnd);
	pButton->SetCheck(1);
	SetAutoDNSCtrlState();
}

void UpdateRegeditAPN(const std::tstring strApn,const std::tstring strKeyName,const std::tstring strIP)
{
	if(strApn.empty()) return;
	CRegKey reg;
    CString lpszKey =  GetConfigString()+"\\"+strApn.c_str(); 
    
	if(reg.Open(HKEY_LOCAL_MACHINE,lpszKey) != ERROR_SUCCESS)
	{
		reg.Create(HKEY_LOCAL_MACHINE,lpszKey);		
		if(reg.Open(HKEY_LOCAL_MACHINE,lpszKey) != ERROR_SUCCESS)
		{
			return ;
		}
	}
	
	reg.SetValue(strIP.c_str(),strKeyName.c_str());
	reg.Close();
}

void DeleteRegestryAPN(const std::tstring strApn,const std::tstring strKeyName)
{
	if(strApn.empty()) return;
	CRegKey reg;
    CString lpszKey =  GetConfigString()+"\\"+strApn.c_str(); 
    
	if(reg.Open(HKEY_LOCAL_MACHINE,lpszKey) != ERROR_SUCCESS)
	{		
		return ;	
	}
	
	reg.DeleteValue(strKeyName.c_str());
	reg.Close();
}

void CDlgIPDNS::OnOK() 
{
	// TODO: Add extra validation here
	if(m_strApn.empty())
		CDialog::OnOK();

	CWnd *pWnd = GetDlgItem(IDC_RADIO_AUTOIP);
	CButton *pButton = static_cast<CButton*>(pWnd);
	if(pButton->GetCheck() > 0)
	{
		DeleteRegestryAPN(m_strApn,TEXT("IP"));
	}
	else
	{
		CString strIP;
		pWnd = GetDlgItem(IDC_IPADDRESS_IP);	
		pWnd->GetWindowText(strIP);
		UpdateRegeditAPN(m_strApn,TEXT("IP"),LPCTSTR(strIP));
	}

	pWnd = GetDlgItem(IDC_RADIO_AUTODNS);
	pButton = static_cast<CButton*>(pWnd);
	if(pButton->GetCheck() > 0)
	{
		DeleteRegestryAPN(m_strApn,TEXT("DNS1"));
		DeleteRegestryAPN(m_strApn,TEXT("DNS2"));
	}
	else
	{
		CString strDNS1;
		CString strDNS2;
		pWnd = GetDlgItem(IDC_IPADDRESS_DNS1);	
		pWnd->GetWindowText(strDNS1);
		UpdateRegeditAPN(m_strApn,TEXT("DNS1"),LPCTSTR(strDNS1));
		pWnd = GetDlgItem(IDC_IPADDRESS_DNS2);	
		pWnd->GetWindowText(strDNS2);
		UpdateRegeditAPN(m_strApn,TEXT("DNS2"),LPCTSTR(strDNS2));
	}

	CDialog::OnOK();
}

void CDlgIPDNS::OnRadioStaticdns() 
{
	// TODO: Add your control notification handler code here
	SetStaticDNSCtrlState();
}

void CDlgIPDNS::OnRadioAutodns() 
{
	// TODO: Add your control notification handler code here
	SetAutoDNSCtrlState();
}

void CDlgIPDNS::OnRadioAutoip() 
{
	// TODO: Add your control notification handler code here
	SetAutoIPCtrlState();
}

void CDlgIPDNS::OnRadioStaticip() 
{
	// TODO: Add your control notification handler code here
	SetStaticIPCtrlState();
}
