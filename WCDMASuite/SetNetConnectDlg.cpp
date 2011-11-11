// SetNetConnectDlg.cpp : implementation file
//

#include "stdafx.h"
#include "SunnyCat.h"
#include "SetNetConnectDlg.h"
#include "Porting.h"
#include <atlbase.h>
#include "ATCmdQ.h"
#include "SunnyCatDlg.h"
#include "Util.h"
#include "ApnDlg.h"


#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

/////////////////////////////////////////////////////////////////////////////
// CSetNetConnectDlg dialog
static HWND setDlgHand = NULL; 

CSetNetConnectDlg::CSetNetConnectDlg(CWnd* pParent /*=NULL*/)
	: CBmpBkDialog(CSetNetConnectDlg::IDD, pParent)
{
	//{{AFX_DATA_INIT(CSetNetConnectDlg)
		// NOTE: the ClassWizard will add member initialization here

	//}}AFX_DATA_INIT
	m_currentItem = -1;
	m_bSearchCops = false;
}

CSetNetConnectDlg::~CSetNetConnectDlg()
{
	setDlgHand = NULL;
	m_bSearchCops = false;
}

void CSetNetConnectDlg::DoDataExchange(CDataExchange* pDX)
{
	CBmpBkDialog::DoDataExchange(pDX);
	//{{AFX_DATA_MAP(CSetNetConnectDlg)
	DDX_Control(pDX, IDC_COMBO_APN, m_ApnCombo);
    DDX_Control(pDX, IDC_LIST_SP_NAME, m_ctrlListSPName);
	DDX_Control(pDX, IDC_EDIT_PHONENUM, m_pEdtPhoneNum);
	DDX_Text(pDX, IDC_EDIT_PASSWORD, m_strPassWord);
	DDX_Text(pDX, IDC_EDIT_APN, m_strApn);
	DDX_Text(pDX, IDC_EDIT_PHONENUM, m_strPhoneNum);
	DDX_Text(pDX, IDC_EDIT_USER, m_strUser);
	//}}AFX_DATA_MAP
}


BEGIN_MESSAGE_MAP(CSetNetConnectDlg, CBmpBkDialog)
	//{{AFX_MSG_MAP(CSetNetConnectDlg)
	ON_BN_CLICKED(IDC_BUTTON_SEARCH_SP, OnButtonSearchSp)
	ON_BN_CLICKED(IDC_BUTTON_SET_SP, OnButtonSetSp)
	ON_NOTIFY(NM_CLICK, IDC_LIST_SP_NAME, OnClickListSpName)
	ON_NOTIFY(NM_DBLCLK, IDC_LIST_SP_NAME, OnDblclkListSpName)
	ON_NOTIFY(LVN_ITEMCHANGED, IDC_LIST_SP_NAME, OnItemchangedListSpName)
	ON_BN_CLICKED(IDC_BUTTON_NEW, OnButtonNew)
	ON_BN_CLICKED(IDC_BUTTON_EDIT, OnButtonEdit)
	ON_BN_CLICKED(IDC_BUTTON_DELETE, OnButtonDelete)
	ON_CBN_SELCHANGE(IDC_COMBO_APN, OnSelchangeComboApn)
	ON_CBN_DROPDOWN(IDC_COMBO_APN, OnDropdownComboApn)
	//}}AFX_MSG_MAP
	ON_MESSAGE(WM_SET_SEARCH,OnSearchMSG)
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CSetNetConnectDlg message handlers

BOOL CSetNetConnectDlg::OnInitDialog() 
{
	CBmpBkDialog::OnInitDialog();
	setDlgHand = GetSafeHwnd();
	if(!((CSunnyCatDlg *)AfxGetMainWnd())->m_MobileManager.isAbleCom)
	{
		GetDlgItem(IDC_BUTTON_SEARCH_SP)->EnableWindow(FALSE);
	}

	if(((CSunnyCatDlg*)AfxGetMainWnd())->m_isConnect)
	{
		GetDlgItem(IDC_BUTTON_SEARCH_SP)->EnableWindow(FALSE);
	}
	CallStatus m_status;
	m_status = ((CSunnyCatDlg*)AfxGetMainWnd())->m_MobileManager.m_pCallMgr->GetCallStatus();
	if(m_status != CALLSTATE_IDLE)
	{
		GetDlgItem(IDC_BUTTON_SEARCH_SP)->EnableWindow(FALSE);
	}
	GetDlgItem(IDC_STATIC_CONNECT_INFO)->SetWindowText(LoadStringEx(IDS_DIAL_INFO));
	GetDlgItem(IDC_STATIC_USERNAME)->SetWindowText(LoadStringEx(IDS_USER_NAME));
	GetDlgItem(IDC_STATIC_PASSWORD)->SetWindowText(LoadStringEx(IDS_USER_PASSWORD));
	GetDlgItem(IDC_STATIC_PHONENUM)->SetWindowText(LoadStringEx(IDS_USER_PHONE));
	GetDlgItem(IDC_STATIC_APN)->SetWindowText(LoadStringEx(IDS_CONNECT_POINT));
	GetDlgItem(IDC_SEL_NETWORK)->SetWindowText(LoadStringEx(IDS_SELECT_NETWORK));
	GetDlgItem(IDC_CURRENT_NET)->SetWindowText(LoadStringEx(IDS_CURRENT_NETWORK));
	GetDlgItem(IDC_BUTTON_SET_SP)->SetWindowText(LoadStringEx(IDS_BUTTON_SET));
	GetDlgItem(IDC_BUTTON_SEARCH_SP)->SetWindowText(LoadStringEx(IDS_BUTTON_SEARCH));
	GetDlgItem(IDC_BUTTON_NEW)->SetWindowText(LoadStringEx(IDS_APN_NEW));
	GetDlgItem(IDC_BUTTON_EDIT)->SetWindowText(LoadStringEx(IDS_APN_EDIT));
	GetDlgItem(IDC_BUTTON_DELETE)->SetWindowText(LoadStringEx(IDS_APN_DELETE));

	/**************************/// add by lab686 20091106
	GetDlgItem(IDC_SEL_NETWORK)->ShowWindow(false);
	GetDlgItem(IDC_CURRENT_NET)->ShowWindow(false);
	GetDlgItem(IDC_EDIT_SP_NAME)->ShowWindow(false);
	GetDlgItem(IDC_BUTTON_SET_SP)->ShowWindow(false);
	GetDlgItem(IDC_BUTTON_SEARCH_SP)->ShowWindow(false);
	GetDlgItem(IDC_LIST_SP_NAME)->ShowWindow(false);
	/**************************/

	CString strSpName = "";
	CATCmd cmd;
	cmd = ((CSunnyCatDlg*)AfxGetMainWnd())->m_MobileManager.SendATPacket(CMobileATProto::QueryCurNet());//查询当前网络
	((CSunnyCatDlg*)AfxGetMainWnd())->m_MobileManager.WaitForATCompleted(cmd,5000);
	strSpName = ((CSunnyCatDlg*)AfxGetMainWnd())->m_MobileManager.m_strCurNet;
	GetDlgItem(IDC_EDIT_SP_NAME)->SetWindowText(strSpName);
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
	if(reg.QueryValue(szValue, TEXT("UserName"), & nCount) == ERROR_SUCCESS)
	{
		m_strUser = szValue;
	}
	nCount = sizeof(szValue)-1;
	if(reg.QueryValue(szValue,TEXT("PassWord"), & nCount) == ERROR_SUCCESS)
	{
		m_strPassWord = szValue;
	}
	nCount = sizeof(szValue)-1;
	if(reg.QueryValue(szValue, TEXT("Phone"), & nCount) == ERROR_SUCCESS)
	{
		m_strPhoneNum = szValue;
	}
	else
	{
		m_strPhoneNum = TEXT("*99***1#");
	}
	nCount = sizeof(szValue)-1;
	if(reg.QueryValue(szValue, TEXT("Apn"), & nCount) == ERROR_SUCCESS)
	{
		m_strApn = szValue;
	}
	else
	{
		m_strApn = TEXT("WCDMA Wireless");
	}

	CString strApn;
	nCount = sizeof(szValue)-1;
	if(reg.QueryValue(szValue, TEXT("ApnStr"), & nCount) == ERROR_SUCCESS)
	{
		strApn = szValue;
	}
	else
	{
		strApn = TEXT("WCDMA Wireless");
	}

	reg.Close();

	UpdateData(FALSE);
	
	int nFind = -1;
	
	CWnd* pWnd = GetDlgItem(IDC_COMBO_APN);
	CComboBox* pBox = (CComboBox*)pWnd;
	ASSERT(pBox != NULL);
	pBox->ResetContent();

	nFind = strApn.Find(TEXT("`"));
	CString strLeft = strApn;
	int nCurSel = -1;
	int i = 0;

	while( nFind  >=0 )
	{
		CString str = strLeft.Left(nFind);
		pBox->AddString(str);
		if(str == m_strApn)
			nCurSel = i;
		strLeft.Delete(0,nFind+1);
		nFind = strLeft.Find(TEXT("`"));
		i++;
	}


	if(strLeft == m_strApn)
		nCurSel = i;
	pBox->AddString(strLeft);
	pBox->SetCurSel(nCurSel);

	
	(void)m_ctrlListSPName.SetExtendedStyle( LVS_EX_FULLROWSELECT| LVS_EX_INFOTIP);			//隐藏列表线 
	m_ctrlListSPName.InsertColumn(0,  LoadStringEx(IDS_SP_LIST_NUM), LVCFMT_LEFT, 55);	
	m_ctrlListSPName.InsertColumn(1, LoadStringEx(IDS_SP_LIST_NAME), LVCFMT_LEFT, 100);
	m_ctrlListSPName.InsertColumn(2, LoadStringEx(IDS_SP_STATE), LVCFMT_LEFT, 100);
	return TRUE;  // return TRUE unless you set the focus to a control
	              // EXCEPTION: OCX Property Pages should return FALSE
}


void CALLBACK COPSAckCallBack(void* pVoid)
{	
	PostMessage((HWND)setDlgHand,WM_SET_SEARCH,0,0);
	return;
}

LONG CSetNetConnectDlg::OnSearchMSG(WPARAM wParam, LPARAM lParam)
{
	m_bSearchCops = false;
	if(setDlgHand == NULL)
		return 0;
	m_netName.Copy(((CSunnyCatDlg*)AfxGetMainWnd())->m_MobileManager.m_AllNet);
	m_netNameSize = m_netName.GetSize();

	for(int i = 0; i < m_netNameSize; i++)
	{	
		CString strNum = "";
		strNum.Format(TEXT("%d"), i+1);
		CString strName = "";
		CString strSatae = "";
		int iPos = m_netName.GetAt(i).Find(TEXT(","));
		if(iPos>=0)
		{
			strSatae = m_netName.GetAt(i).Left(iPos);
			strName = m_netName.GetAt(i).Mid(iPos+1);
			switch(_ttoi(strSatae))
			{
			case 0:
				 strSatae = "unknown";
				break;
			case 1:
				 strSatae = "available";
				break;
			case 2:
				 strSatae = "current";
				break;
			case 3:
				 strSatae = "forbidden";
				break;
			default:
				strSatae = "unknown";
				break;
			}
		}
		int nn=m_ctrlListSPName.InsertItem(m_ctrlListSPName.GetItemCount(), strNum);
		m_ctrlListSPName.SetItemText(nn, 1, strName);
		m_ctrlListSPName.SetItemText(nn, 2, strSatae);
	}
	m_netName.RemoveAll();
	GetDlgItem(IDC_BUTTON_SEARCH_SP)->EnableWindow(TRUE);
	GetDlgItem(IDC_BUTTON_SEARCH_SP)->SetWindowText(LoadStringEx(IDS_BUTTON_SEARCH));
	UpdateWindow();
	m_bSearchCops = false;
	return 0;
}


void CSetNetConnectDlg::OnButtonSearchSp() 
{
	m_bSearchCops = true;
	GetDlgItem(IDC_BUTTON_SEARCH_SP)->SetWindowText(LoadStringEx(IDS_SP_WAIT));
	((CSunnyCatDlg*)AfxGetMainWnd())->m_MobileManager.m_AllNet.RemoveAll();

	((CSunnyCatDlg*)AfxGetMainWnd())->m_MobileManager.pfnCopsRegsiterObserver(COPSAckCallBack);	
	GetDlgItem(IDC_BUTTON_SEARCH_SP)->SetWindowText(LoadStringEx(IDS_SP_WAIT));
	GetDlgItem(IDC_BUTTON_SEARCH_SP)->EnableWindow(FALSE);
	//添加到列表中
	m_ctrlListSPName.DeleteAllItems();
	CATCmd cmd;	
	cmd = ((CSunnyCatDlg*)AfxGetMainWnd())->m_MobileManager.SendATPacket(CMobileATProto::QueryAllNet());//查询所有能侦听到的网络
	bool bError = RETURN_ERROR_REPORT <= ((CSunnyCatDlg*)AfxGetMainWnd())->m_MobileManager.TestATCompleted(cmd,1000);
	if(bError)
	{
		m_bSearchCops= false;
		OnSearchMSG(0,0);
	}
		
	UpdateWindow();
	//	HWND hwHand = GetSafeHwnd();
}

/*void CSetNetConnectDlg::OnButtonSearchSp() 
{
	// TODO: Add your control notification handler code here
	GetDlgItem(IDC_BUTTON_SEARCH_SP)->SetWindowText(LoadStringEx(IDS_SP_WAIT));
	((CSunnyCatDlg*)AfxGetMainWnd())->m_MobileManager.m_AllNet.RemoveAll();
	CATCmd cmd;
	CWnd * pWnd = (CSunnyCatDlg*)AfxGetMainWnd();
	cmd = ((CSunnyCatDlg*)AfxGetMainWnd())->m_MobileManager.SendATPacket(CMobileATProto::QueryAllNet());//查询所有能侦听到的网络
	((CSunnyCatDlg*)AfxGetMainWnd())->m_MobileManager.WaitForATCompleted(cmd,180000);	
	m_netName.Copy(((CSunnyCatDlg*)AfxGetMainWnd())->m_MobileManager.m_AllNet);
	m_netNameSize = m_netName.GetSize();
	GetDlgItem(IDC_BUTTON_SEARCH_SP)->SetWindowText(LoadStringEx(IDS_SP_SEARCH));
	//添加到列表中
	m_ctrlListSPName.DeleteAllItems();
	for(int i = 0; i < m_netNameSize; i++)
	{	
		CString strNum = "";
		strNum.Format(TEXT("%d"), i+1);
		CString strName = "";
		CString strSatae = "";
		int iPos = m_netName.GetAt(i).Find(TEXT(","));
		if(iPos>=0)
		{
			strSatae = m_netName.GetAt(i).Left(iPos);
			strName = m_netName.GetAt(i).Mid(iPos+1);
			switch(_ttoi(strSatae))
			{
			case 0:
				 strSatae = TEXT("unknown");
				break;
			case 1:
				 strSatae = TEXT("available");
				break;
			case 2:
				 strSatae = TEXT("current");
				break;
			case 3:
				 strSatae = TEXT("forbidden");
				break;
			default:
				strSatae = TEXT("unknown");
				break;
			}
		}
		int nn=m_ctrlListSPName.InsertItem(m_ctrlListSPName.GetItemCount(), strNum);
		m_ctrlListSPName.SetItemText(nn, 1, strName);
		m_ctrlListSPName.SetItemText(nn, 2, strSatae);
	}
	m_netName.RemoveAll();

}
*/
void CSetNetConnectDlg::OnButtonSetSp() 
{
	// TODO: Add your control notification handler code here
	CString strSetNet = "";
	if(m_currentItem >= 0)
	{
		CString strSatae = "";
		strSatae = m_ctrlListSPName.GetItemText(m_currentItem, 2);
		if(strSatae == "current")
		{
			GMessageBox(LoadStringEx(IDS_SP_REG_CURRENT));
			return;
		}
		if(strSatae != "available")
		{
			GMessageBox(LoadStringEx(IDS_SP_REG_UNABLE));
			return;
		}
	//	strSetNet = m_ctrlListSPName.GetItemText(m_currentItem, 1);
		GetDlgItem(IDC_EDIT_SP_NAME)->GetWindowText(strSetNet);
		CATCmd cmd;
		cmd = ((CSunnyCatDlg*)AfxGetMainWnd())->m_MobileManager.SendATPacket(CMobileATProto::RegisterNet(strSetNet));//查询所有能侦听到的网络
		if(RETURN_OK == ((CSunnyCatDlg*)AfxGetMainWnd())->m_MobileManager.WaitForATCompleted(cmd,20000))	
		{
			GMessageBox(LoadStringEx(IDS_SP_REG_OK));
		}
		else
		{
			GMessageBox(LoadStringEx(IDS_SP_REG_FAIL));
		}
	}
	return;
}

void CSetNetConnectDlg::OnClickListSpName(NMHDR* pNMHDR, LRESULT* pResult) 
{
	// TODO: Add your control notification handler code here
	CString strName = "";
	strName = m_ctrlListSPName.GetItemText(m_currentItem, 1);
	GetDlgItem(IDC_EDIT_SP_NAME)->SetWindowText(strName);
	*pResult = 0;
}

void CSetNetConnectDlg::OnDblclkListSpName(NMHDR* pNMHDR, LRESULT* pResult) 
{
	// TODO: Add your control notification handler code here
	CString strName = "";
	strName = m_ctrlListSPName.GetItemText(m_currentItem, 1);
	GetDlgItem(IDC_EDIT_SP_NAME)->SetWindowText(strName);
	*pResult = 0;
}

void CSetNetConnectDlg::OnItemchangedListSpName(NMHDR* pNMHDR, LRESULT* pResult) 
{
	NM_LISTVIEW* pNMListView = (NM_LISTVIEW*)pNMHDR;
	// TODO: Add your control notification handler code here
	m_currentItem = pNMListView->iItem;
	CString strName = "";
	strName = m_ctrlListSPName.GetItemText(m_currentItem, 1);
	GetDlgItem(IDC_EDIT_SP_NAME)->SetWindowText(strName);
	*pResult = 0;
}

void CSetNetConnectDlg::OnButtonNew() 
{
	// TODO: Add your control notification handler code here
	CApnDlg dlg;
	dlg.DoModal();
}

void CSetNetConnectDlg::OnButtonEdit() 
{
	// TODO: Add your control notification handler code here
	CWnd* pWnd = GetDlgItem(IDC_COMBO_APN);
	CComboBox* pBox = (CComboBox*)pWnd;
	ASSERT(pBox != NULL);
	CString strCurApn;
	pBox->GetLBText(pBox->GetCurSel(),strCurApn);
	CApnDlg dlg(this,strCurApn);
	dlg.DoModal();
}

void CSetNetConnectDlg::OnButtonDelete() 
{
	// TODO: Add your control notification handler code here
	if(GMessageBox(::LoadStringEx(IDS_DEL_INFO),NULL,MB_OKCANCEL|MB_ICONQUESTION) == IDCANCEL)
		return;
	
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

	CString strApn;
	TCHAR szValue[1024] = {0};
	DWORD nCount = sizeof(szValue)-1;
	if(reg.QueryValue(szValue, TEXT("ApnStr"), & nCount) == ERROR_SUCCESS)
	{
		strApn = szValue;
	}
	else
	{
		strApn = "WCDMA Wireless";
	}
	
	CWnd* pWnd = GetDlgItem(IDC_COMBO_APN);
	CComboBox* pBox = (CComboBox*)pWnd;
	ASSERT(pBox != NULL);
	CString strCurApn;
	pBox->GetLBText(pBox->GetCurSel(),strCurApn);

	if(strCurApn == "WCDMA Wireless")
	{
		GMessageBox(::LoadStringEx(IDS_APN_CMNETNOTDEL));
		reg.Close();
		return;
	}
	CString strFind = strCurApn+"`";
	int nFind = strApn.Find(strFind);
	if(nFind >= 0)
	{
		strApn.Delete(nFind,strFind.GetLength());
	}
	else
	{
		nFind = strApn.Find(strCurApn);
		if(nFind + strCurApn.GetLength() == strApn.GetLength())
		{
			strApn.Delete(nFind,strCurApn.GetLength());
		}
	}

	if(strApn.GetAt(0) == TEXT('`'))
		strApn.Delete(0);
	if(strApn.GetAt(strApn.GetLength()-1) == TEXT('`'))
		strApn.Delete(strApn.GetLength()-1);
	strApn.Replace(TEXT("``"),TEXT("`"));
	
	reg.SetValue(strApn,TEXT("ApnStr"));
	reg.DeleteValue(strCurApn);
	reg.DeleteSubKey(strCurApn);
	reg.Close();

	RefreshComBox(TRUE);
	OnSelchangeComboApn();
}

void CSetNetConnectDlg::OnSelchangeComboApn() 
{
	// TODO: Add your control notification handler code here
	CString strApn;
	m_ApnCombo.GetWindowText(strApn);
	GetEditValueByApn(strApn);
}

void CSetNetConnectDlg::OnDropdownComboApn() 
{
	// TODO: Add your control notification handler code here
	/*CRegKey reg;

    CString regPath = GetConfigString();

	if(reg.Open(HKEY_LOCAL_MACHINE, regPath) != ERROR_SUCCESS) //打开成功
	{
		reg.Create(HKEY_LOCAL_MACHINE, regPath);		
		if(reg.Open(HKEY_LOCAL_MACHINE, regPath) != ERROR_SUCCESS)
		{
			return ;
		}
	}
	CString strApn;
	char szValue[1024] = {0};
	DWORD nCount = sizeof(szValue)-1;
	if(reg.QueryValue(szValue, "ApnStr", & nCount) == ERROR_SUCCESS)
	{
		strApn = szValue;
	}
	else
	{
		strApn = "WCDMA Wireless";
	}

	reg.Close();
	
	int nFind = -1;
	
	CWnd* pWnd = GetDlgItem(IDC_COMBO_APN);
	CComboBox* pBox = (CComboBox*)pWnd;
	ASSERT(pBox != NULL);
	pBox->ResetContent();

	nFind = strApn.Find(";");
	CString strLeft = strApn;
	while( nFind  >=0 )
	{
		CString str = strLeft.Left(nFind);
		pBox->AddString(str);
		strLeft.Delete(0,nFind+1);
		nFind = strLeft.Find(";");
	}

	pBox->AddString(strLeft);*/
}

void CSetNetConnectDlg::RefreshComBox(BOOL bDelete)
{
	CRegKey reg;

    CString regPath = GetConfigString();

	if(reg.Open(HKEY_LOCAL_MACHINE, regPath) != ERROR_SUCCESS) //打开成功
	{
		reg.Create(HKEY_LOCAL_MACHINE, regPath);		
		if(reg.Open(HKEY_LOCAL_MACHINE, regPath) != ERROR_SUCCESS)
		{
			return ;
		}
	}
	CString strApn;
	TCHAR szValue[1024] = {0};
	DWORD nCount = sizeof(szValue)-1;
	if(reg.QueryValue(szValue, TEXT("ApnStr"), & nCount) == ERROR_SUCCESS)
	{
		strApn = szValue;
	}
	else
	{
		strApn = "WCDMA Wireless";
	}

	reg.Close();
	
	int nFind = -1;
	
	CWnd* pWnd = GetDlgItem(IDC_COMBO_APN);
	CComboBox* pBox = (CComboBox*)pWnd;
	ASSERT(pBox != NULL);
	int nCurSel = pBox->GetCurSel();
	pBox->ResetContent();

	nFind = strApn.Find(TEXT("`"));
	CString strLeft = strApn;
	while( nFind  >=0 )
	{
		CString str = strLeft.Left(nFind);
		pBox->AddString(str);
		strLeft.Delete(0,nFind+1);
		nFind = strLeft.Find(TEXT("`"));
	}

	pBox->AddString(strLeft);
	if(!bDelete)
		pBox->SetCurSel(nCurSel);
	else
		pBox->SetCurSel(0);
	OnSelchangeComboApn();
}

void CSetNetConnectDlg::GetEditValueByApn(CString strApn)
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

	CWnd* pWnd = NULL;
	if(strText.IsEmpty())
	{
		pWnd = GetDlgItem(IDC_EDIT_USER);
		pWnd->SetWindowText(TEXT(""));
		pWnd = GetDlgItem(IDC_EDIT_PASSWORD);
		pWnd->SetWindowText(TEXT(""));
		pWnd = GetDlgItem(IDC_EDIT_PHONENUM);
		pWnd->SetWindowText(TEXT(""));

		reg.SetValue(TEXT(""),TEXT("UserName"));
		reg.SetValue(TEXT(""),TEXT("PassWord"));
		reg.SetValue(TEXT(""),TEXT("Phone"));
		reg.SetValue(TEXT(""),TEXT("Apn"));
		reg.Close();
		return;
	}
	
	int nFind = strText.Find(TEXT("```"));
	CString strLeft = strText;
	int nCurSel = 0;
	int i = 0;
	CString str;
	

	if(nFind >= 0)
	{
		str = strLeft.Left(nFind);
		pWnd = GetDlgItem(IDC_EDIT_USER);
		pWnd->SetWindowText(str);
		strLeft.Delete(0,nFind+3);
	}

	nFind = strLeft.Find(TEXT("```"));
	if(nFind >= 0)
	{
		str = strLeft.Left(nFind);
		pWnd = GetDlgItem(IDC_EDIT_PASSWORD);
		pWnd->SetWindowText(str);
		strLeft.Delete(0,nFind+3);
	}

	
	pWnd = GetDlgItem(IDC_EDIT_PHONENUM);
	pWnd->SetWindowText(strLeft);
	
}
