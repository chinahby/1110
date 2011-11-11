// ApnDlg.cpp : implementation file
//

#include "stdafx.h"
#include "sunnycat.h"
#include "ApnDlg.h"
#include <atlbase.h>
#include "Porting.h"
#include "SetNetConnectDlg.h"
#include "SetDlg.h"
#include "Util.h"
#include "DlgIPDNS.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

/////////////////////////////////////////////////////////////////////////////
// CApnDlg dialog


CApnDlg::CApnDlg(CWnd* pParent /*=NULL*/)
	: CBmpBkDialog(CApnDlg::IDD, pParent)
{
	//{{AFX_DATA_INIT(CApnDlg)
	m_strApn = _T("");
	m_strPhone = _T("");
	m_strPsw = _T("");
	m_strUser = _T("");
	//}}AFX_DATA_INIT
	m_ApnState = APN_NEW;
}

CApnDlg::CApnDlg(CWnd* pParent,CString strApn)
	: CBmpBkDialog(CApnDlg::IDD, pParent)
{
	m_strEdit = strApn;
	m_ApnState = APN_EDIT;
}

void CApnDlg::DoDataExchange(CDataExchange* pDX)
{
	CBmpBkDialog::DoDataExchange(pDX);
	//{{AFX_DATA_MAP(CApnDlg)
	DDX_Control(pDX, IDC_EDIT_APN, m_apnEdit);
	DDX_Control(pDX, IDC_EDIT_PHONE, m_phoneEdit);
	DDX_Control(pDX, IDC_EDIT_PSW, m_pswEdit);
	DDX_Control(pDX, IDC_EDIT_USER, m_userEdit);
	DDX_Text(pDX, IDC_EDIT_APN, m_strApn);
	DDX_Text(pDX, IDC_EDIT_PHONE, m_strPhone);
	DDX_Text(pDX, IDC_EDIT_PSW, m_strPsw);
	DDX_Text(pDX, IDC_EDIT_USER, m_strUser);
	//}}AFX_DATA_MAP
}


BEGIN_MESSAGE_MAP(CApnDlg, CBmpBkDialog)
	//{{AFX_MSG_MAP(CApnDlg)
	ON_BN_CLICKED(IDC_BUTTON_ADVANCED, OnButtonAdvanced)
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CApnDlg message handlers

void CApnDlg::OnOK() 
{
	// TODO: Add extra validation here
	UpdateData(TRUE);

	if(m_strApn.IsEmpty())
	{
		GMessageBox(::LoadStringEx(IDS_APN_NOTNULL));
		return;
	}
	CRegKey reg;
    LPCTSTR lpszKey =  GetConfigString(); 
    
	if(reg.Open(HKEY_LOCAL_MACHINE,lpszKey) != ERROR_SUCCESS)
	{
		reg.Create(HKEY_LOCAL_MACHINE,lpszKey);		
		if(reg.Open(HKEY_LOCAL_MACHINE,lpszKey) != ERROR_SUCCESS)
		{
			return ;
		}
	}	

	CString strText;
	TCHAR szValue[1024] = {0};
	DWORD nCount = sizeof(szValue)-1;
	if(reg.QueryValue(szValue, TEXT("ApnStr"), & nCount) == ERROR_SUCCESS)
	{
		strText = szValue;
	}

	int nFind = strText.Find(TEXT("`"));
	CString strLeft = strText;
	int nCurSel = -1;
	int i = 0;

	while( nFind  >=0 )
	{
		CString str = strLeft.Left(nFind);
		
		if(str.CompareNoCase(m_strApn) == 0)
			nCurSel = i;
		strLeft.Delete(0,nFind+1);
		nFind = strLeft.Find(TEXT("`"));
		i++;
	}


	if(strLeft.CompareNoCase(m_strApn) == 0)
		nCurSel = i;
	
	if(m_ApnState == APN_NEW)
	{
		if(nCurSel >= 0)
		{
			GMessageBox(::LoadStringEx(IDS_APN_EXIST));
			reg.Close();

			return;
		}
		if(!strText.IsEmpty())
			strText += "`";
		strText += m_strApn;
		reg.SetValue(strText, TEXT("ApnStr"));

		strText = m_strUser + "```"+m_strPsw + "```"+m_strPhone;
		reg.SetValue(strText,m_strApn);
	}
	else if(m_ApnState == APN_EDIT)
	{
		if(m_strApn != m_strEdit)
		{
			if(nCurSel >= 0)
			{
				GMessageBox(::LoadStringEx(IDS_APN_EXIST));
				reg.Close();

				return;
			}
		}
		
		CString strFind = m_strEdit+"`";
		int nFind = strText.Find(strFind);
		if(nFind >= 0)
		{
			reg.DeleteValue(m_strEdit);
			strText.Delete(nFind,strFind.GetLength());
			strText.Insert(nFind,m_strApn+"`");
			//strText.Replace(m_strEdit,m_strApn);
			reg.SetValue(strText, TEXT("ApnStr"));
			
			strText = m_strUser + "```"+m_strPsw + "```"+m_strPhone;
			reg.SetValue(strText,m_strApn);
		}
		else
		{
			nFind = strText.Find(m_strEdit);
			if(nFind + m_strEdit.GetLength() == strText.GetLength())
			{
				reg.DeleteValue(m_strEdit);
				strText.Delete(nFind,m_strEdit.GetLength());
				strText.Insert(nFind,m_strApn);
				//strText.Replace(m_strEdit,m_strApn);
				reg.SetValue(strText, TEXT("ApnStr"));
				strText = m_strUser + "```"+m_strPsw + "```"+m_strPhone;
				reg.SetValue(strText,m_strApn);
			}
		}
		
	}

	reg.Close();
	GetParent()->SendMessage(WM_REFRESH_COMBOBOX,0,0);
	CDialog::OnOK();
}

BOOL CApnDlg::OnInitDialog() 
{
	CBmpBkDialog::OnInitDialog();
	m_strApn = m_strEdit;

	if(m_ApnState == APN_EDIT)
	{
		GetEditValueByApn(m_strApn);
		if(m_strEdit == TEXT("WCDMA"))
		{
			DWORD dwStyle = m_apnEdit.GetStyle();
			::SetWindowLong(m_apnEdit.GetSafeHwnd(),GWL_STYLE,dwStyle|WS_DISABLED);		
		}
	}
	

	UpdateData(FALSE);
	// TODO: Add extra initialization here

	SetWindowText(LoadStringEx(IDS_DIAL_INFO));
	GetDlgItem(IDOK)->SetWindowText(LoadStringEx(IDS_APN_OK));
	GetDlgItem(IDCANCEL)->SetWindowText(LoadStringEx(IDS_APN_CANCEL));
	GetDlgItem(IDC_STATIC_USER)->SetWindowText(LoadStringEx(IDS_USER_NAME));
	GetDlgItem(IDC_STATIC_PSW)->SetWindowText(LoadStringEx(IDS_USER_PASSWORD));
	GetDlgItem(IDC_STATIC_PHONE)->SetWindowText(LoadStringEx(IDS_USER_PHONE));
	GetDlgItem(IDC_STATIC_APN)->SetWindowText(LoadStringEx(IDS_CONNECT_POINT));
	GetDlgItem(IDC_BUTTON_ADVANCED)->SetWindowText(LoadStringEx(IDS_APN_ADVANCED));

	return TRUE;  // return TRUE unless you set the focus to a control
	              // EXCEPTION: OCX Property Pages should return FALSE
}

void CApnDlg::GetEditValueByApn(CString strApn)
{
	CRegKey reg;
    LPCTSTR lpszKey =  GetConfigString(); 
    
	if(reg.Open(HKEY_LOCAL_MACHINE,lpszKey) != ERROR_SUCCESS)
	{
		reg.Create(HKEY_LOCAL_MACHINE,lpszKey);		
		if(reg.Open(HKEY_LOCAL_MACHINE,lpszKey) != ERROR_SUCCESS)
		{
			return ;
		}
	}
	
	CString strText;
	TCHAR szValue[1024] = {0};
	DWORD nCount = sizeof(szValue)-1;
	if(reg.QueryValue(szValue, strApn, & nCount) == ERROR_SUCCESS)
	{
		strText = szValue;
	}

	int nFind = strText.Find(TEXT("```"));
	CString strLeft = strText;
	int nCurSel = 0;
	int i = 0;
	CString str;


	if(nFind >= 0)
	{
		str = strLeft.Left(nFind);
		m_strUser = str;
		strLeft.Delete(0,nFind+3);
	}

	nFind = strLeft.Find(TEXT("```"));
	if(nFind >= 0)
	{
		str = strLeft.Left(nFind);
		m_strPsw = str;
		strLeft.Delete(0,nFind+3);
	}

	
	m_strPhone = strLeft;
	
	
}

void CApnDlg::OnButtonAdvanced()
{
	CWnd *pWnd = GetDlgItem(IDC_EDIT_APN);
	CString strApn;
	pWnd->GetWindowText(strApn);

	CDlgIPDNS dlg(NULL,LPCTSTR(strApn));
	dlg.DoModal();
}