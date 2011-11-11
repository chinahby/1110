// OtherDialog.cpp : implementation file
//

#include "stdafx.h"
#include "SunnyCat.h"
#include "OtherDialog.h"
#include "SunnyCatDlg.h"
#include "Util.h"
#include <atlbase.h>
#include "Porting.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

/////////////////////////////////////////////////////////////////////////////
// COtherDialog dialog


COtherDialog::COtherDialog(CWnd* pParent /*=NULL*/)
	: CBmpBkDialog(COtherDialog::IDD, pParent),
	  m_BtnStatus(STATUS_CONNECT)
{
	//{{AFX_DATA_INIT(COtherDialog)
		// NOTE: the ClassWizard will add member initialization here

	//}}AFX_DATA_INIT
}


void COtherDialog::DoDataExchange(CDataExchange* pDX)
{
	CBmpBkDialog::DoDataExchange(pDX);
	//{{AFX_DATA_MAP(COtherDialog)
	DDX_Control(pDX, IDC_STATIC_APN, m_ApnLabel);
	DDX_Control(pDX, IDC_COMBO_APN, m_LinkCombo);
	DDX_Control(pDX, IDC_BUTTON_CONNECT, m_ConnectBtn);
	DDX_Control(pDX, IDC_BUTTON_CONNECTEVDO, m_ConnectBtnEvdo);
	//}}AFX_DATA_MAP
}


BEGIN_MESSAGE_MAP(COtherDialog, CBmpBkDialog)
	//{{AFX_MSG_MAP(COtherDialog)
	ON_WM_CTLCOLOR()
	ON_WM_SIZE()
	ON_BN_CLICKED(IDC_BUTTON_CONNECT, OnButtonConnect)
	ON_BN_CLICKED(IDC_BUTTON_CONNECTEVDO, OnButtonConnectevdo)
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// COtherDialog message handlers

HBRUSH COtherDialog::OnCtlColor(CDC* pDC, CWnd* pWnd, UINT nCtlColor) 
{
	HBRUSH hbr = CBmpBkDialog::OnCtlColor(pDC, pWnd, nCtlColor);

	// TODO:  在此更改 DC 的任何属性

	// TODO:  如果默认的不是所需画笔，则返回另一个画笔
	if(nCtlColor == CTLCOLOR_DLG)
	{
		static HBRUSH brush=CreateSolidBrush(RGB(221,241,251));
		return brush;

	}

	return hbr;
}

void COtherDialog::OnSize(UINT nType, int cx, int cy) 
{
	CBmpBkDialog::OnSize(nType, cx, cy);

	// TODO: 在此处添加消息处理程序代码
	CRect rcApn;
	CRect rcLinkCombo;
	CRect rcConnectBtn;
	CRect reConnectBtnEVDO;
	if(m_ApnLabel.GetSafeHwnd() != NULL)
	{
		m_ApnLabel.GetWindowRect(&rcApn);
		m_ConnectBtn.GetWindowRect(&rcConnectBtn);
		m_ConnectBtnEvdo.GetWindowRect(&reConnectBtnEVDO);
		::MapWindowPoints( HWND_DESKTOP, GetSafeHwnd(), (LPPOINT)(RECT*)&rcApn, 2 );
		::MapWindowPoints( HWND_DESKTOP, GetSafeHwnd(), (LPPOINT)(RECT*)&rcConnectBtn, 2 );
		::MapWindowPoints( HWND_DESKTOP, GetSafeHwnd(), (LPPOINT)(RECT*)&reConnectBtnEVDO, 2 );
		int nWidth = rcApn.Width();
		int nHeight = rcApn.Height();
		rcApn.left = (cx-(rcConnectBtn.right-rcApn.left))/2;
		rcApn.right = rcApn.left+nWidth;
		rcApn.top = cy/2+5;
		rcApn.bottom = rcApn.top+nHeight;
		m_ApnLabel.MoveWindow(&rcApn);
	}
	if(m_LinkCombo.GetSafeHwnd() != NULL)
	{
		m_LinkCombo.GetWindowRect(&rcLinkCombo);
		::MapWindowPoints( HWND_DESKTOP, GetSafeHwnd(), (LPPOINT)(RECT*)&rcLinkCombo, 2 );
		int nWidth = rcLinkCombo.Width();
		int nHeight = rcLinkCombo.Height();
		rcLinkCombo.left = rcApn.right + 5;
		rcLinkCombo.right = rcLinkCombo.left+nWidth;
		rcLinkCombo.top = cy/2;
		rcLinkCombo.bottom = rcLinkCombo.top+nHeight;
		m_LinkCombo.MoveWindow(&rcLinkCombo);
	}
	if(m_ConnectBtn.GetSafeHwnd() != NULL)
	{	
		m_ConnectBtn.GetWindowRect(&rcConnectBtn);
		::MapWindowPoints( HWND_DESKTOP, GetSafeHwnd(), (LPPOINT)(RECT*)&rcConnectBtn, 2 );
		int nWidth = rcConnectBtn.Width();
		int nHeight = rcConnectBtn.Height();
		rcConnectBtn.left = rcLinkCombo.right + 15;
		rcConnectBtn.right = rcConnectBtn.left+nWidth;
		rcConnectBtn.top = (cy/8) * 3;
		rcConnectBtn.bottom = rcConnectBtn.top+nHeight;
		m_ConnectBtn.MoveWindow(&rcConnectBtn);
	}
	if(m_ConnectBtnEvdo.GetSafeHwnd() != NULL)
	{	
		m_ConnectBtnEvdo.GetWindowRect(&reConnectBtnEVDO);
		::MapWindowPoints( HWND_DESKTOP, GetSafeHwnd(), (LPPOINT)(RECT*)&reConnectBtnEVDO, 2 );
		int nWidth = reConnectBtnEVDO.Width();
		int nHeight = reConnectBtnEVDO.Height();
		reConnectBtnEVDO.left = rcLinkCombo.right + 15;
		reConnectBtnEVDO.right = reConnectBtnEVDO.left+nWidth;
		reConnectBtnEVDO.top = cy/2;		// modify 4 by 2
		reConnectBtnEVDO.bottom = reConnectBtnEVDO.top+nHeight;
		m_ConnectBtnEvdo.MoveWindow(&reConnectBtnEVDO);
	}
}

BOOL COtherDialog::OnInitDialog() 
{
	
	// TODO: Add extra initialization here
	CBmpBkDialog::OnInitDialog();

	/********************************************************************/// modify ui
	CRect temprect(66, 0, 431, 251);
	CWnd::SetWindowPos(NULL, 0, 0, temprect.Width(), temprect.Height(), SWP_NOZORDER|SWP_NOMOVE);
	
//	GetDlgItem(IDC_BUTTON_CONNECT)->MoveWindow(150, 70, 110, 35);
//	GetDlgItem(IDC_BUTTON_CONNECTEVDO)->MoveWindow(150, 110, 110, 25);
//	GetDlgItem(IDC_BUTTON_UNCONNECTEVDO)->MoveWindow(150, 150, 110, 35);

//	GetDlgItem(IDC_BUTTON_CONNECT)->ShowWindow(SW_HIDE);
//	GetDlgItem(IDC_COMBO_APN)->ShowWindow(SW_HIDE);
//	GetDlgItem(IDC_STATIC_APN)->ShowWindow(SW_HIDE);

	/*********************************************************************/

	// TODO:  在此添加额外的初始化

	m_LinkCombo.SetCmbThumbBtIDs(IDB_BITMAP_COMBONO,IDB_BITMAP_COMBOOVER,IDB_BITMAP_COMBODOWN);
	m_ApnLabel.SetWindowText(LoadStringEx(IDS_CONNECT_POINT));
	
//	m_ConnectBtn.SetWindowText(LoadStringEx(IDS_CONNECT));
//	m_ConnectBtn.SetHitOver();
	m_ConnectBtn.ShowWindow(SW_HIDE);
	m_ConnectBtnEvdo.SetWindowText(LoadStringEx(IDS_CONNECT_EVDO));
	m_ConnectBtnEvdo.SetHitOver();
	m_ApnLabel.SetLeftAlign(FALSE);

	InitApn();

	// 异常: OCX 属性页应返回 FALSE
	return TRUE;  // return TRUE unless you set the focus to a control
	              // EXCEPTION: OCX Property Pages should return FALSE
}

void COtherDialog::SetRegValue()
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
	CWnd *pWnd = GetDlgItem(IDC_COMBO_APN);
	CString strApn;
	pWnd->GetWindowText(strApn);
	reg.SetValue(strApn,TEXT("Apn"));
	CSunnyCatDlg *pDlg = NULL;
	pDlg = (CSunnyCatDlg*)AfxGetMainWnd();
//	pDlg->m_MobileManager.installModemInitialAT(strApn);

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
		reg.SetValue(str,TEXT("UserName"));
		strLeft.Delete(0,nFind+3);
	}
	
	nFind = strLeft.Find(TEXT("```"));
	if(nFind >= 0)
	{
		str = strLeft.Left(nFind);
		reg.SetValue(str,TEXT("PassWord"));
		strLeft.Delete(0,nFind+3);
	}
	reg.SetValue(strLeft,TEXT("Phone"));

	reg.Close();
}

void COtherDialog::OnButtonConnect() 
{
	// TODO: Add your control notification handler code here
	CSunnyCatDlg *pDlg = (CSunnyCatDlg*)AfxGetMainWnd();
	///判断是否已插卡
	if(!pDlg->m_MobileManager.isAbleCom)
	{
		GMessageBox(LoadStringEx(IDS_NO_SC700_CARD));
		return;
	}
	
	/************************************************************
	///判断是否正在打电话
	if(m_MobileManager.m_pCallMgr->GetCallStatus() != CALLSTATE_IDLE)
	{
		GMessageBox(LoadStringEx(IDS_PROMPT_CALL));
		return;
	}
	/************************************************************/
	
	TRACE(TEXT("COtherDialog::OnButtonConnect:\n"));

	if ( NETTYPE_EVDO== pDlg->m_linktype)
	{
		TRACE(TEXT("COtherDialog::OnButtonConnect:> NETTYPE_EVDO== pDlg->m_linktype"));
		GMessageBox(LoadStringEx(IDS_CONNECTING_EVDO));
		return ;
	}
	if(m_BtnStatus == STATUS_CONNECT)
	{
		TRACE(TEXT("COtherDialog::OnButtonConnect:> m_BtnStatus == STATUS_CONNECT"));

		pDlg->m_linktype = NETTYPE_ONEX;
		pDlg->m_MobileManager;

/*		
		// 设置网络模式为1X模式
		CATCmd cmd;
		cmd =  pDlg->m_MobileManager.SendATPacket(CMobileATProto::QueryMode());
		pDlg->m_MobileManager.WaitForATCompleted(cmd,2000);
		cmd =  pDlg->m_MobileManager.SendATPacket(CMobileATProto::SetMode(2));
		if ( RETURN_OK == pDlg->m_MobileManager.WaitForATCompleted(cmd,6000))
		{

		}
		
		cmd = pDlg->m_MobileManager.SendATPacket(CMobileATProto::QueryIMEI());				// ok lab686
		pDlg->m_MobileManager.WaitForATCompleted(cmd,2000);
		cmd = pDlg->m_MobileManager.SendATPacket(CMobileATProto::CheckSignal());//查询信号强度 // ok
		pDlg->m_MobileManager.WaitForATCompleted(cmd,2000);						
		cmd = pDlg->m_MobileManager.SendATPacket(CMobileATProto::CheckHDRSignal());//查询信号强度 // ok
		pDlg->m_MobileManager.WaitForATCompleted(cmd,2000);	
*/

		SetRegValue();
		Connect();
	}
	else
	{
		TRACE(TEXT("COtherDialog::OnButtonConnect:> DisConnect"));
		DisConnect();
	}
}

void COtherDialog::OnButtonConnectevdo() 
{
	// TODO: Add your control notification handler code here
	CSunnyCatDlg *pDlg = (CSunnyCatDlg*)AfxGetMainWnd();
	///判断是否已插卡
	if(!pDlg->m_MobileManager.isAbleCom)
	{
		GMessageBox(LoadStringEx(IDS_NO_SC700_CARD));
		return;
	}
	
	CWnd *pWnd = GetDlgItem(IDC_COMBO_APN);
	CString strApn;
	pWnd->GetWindowText(strApn);

	TRACE(TEXT("\strApn:%s\n"),strApn);

	CATCmd cmd;
	cmd = pDlg->m_MobileManager.SendATPacket(CMobileATProto::SetApn(strApn));//设置APN
	pDlg->m_MobileManager.WaitForATCompleted(cmd,1000);

	/************************************************************
	///判断是否正在打电话
	if(m_MobileManager.m_pCallMgr->GetCallStatus() != CALLSTATE_IDLE)
	{
		GMessageBox(LoadStringEx(IDS_PROMPT_CALL));
		return;
	}
	/************************************************************/

	if ( NETTYPE_ONEX == pDlg->m_linktype)
	{
		GMessageBox(LoadStringEx(IDS_CONNECTING_1X));
		return ;
	}
	if(m_BtnStatus == STATUS_CONNECT)
	{
		pDlg->m_linktype = NETTYPE_EVDO;
		pDlg->m_MobileManager;
/*
		// 设置网络模式为混合模式
		CATCmd cmd;
		cmd =  pDlg->m_MobileManager.SendATPacket(CMobileATProto::QueryMode());
		pDlg->m_MobileManager.WaitForATCompleted(cmd,2000);
		cmd =  pDlg->m_MobileManager.SendATPacket(CMobileATProto::SetMode(8));
		if ( RETURN_OK == pDlg->m_MobileManager.WaitForATCompleted(cmd,6000))
		{
		}	

		cmd = pDlg->m_MobileManager.SendATPacket(CMobileATProto::QueryIMEI());				// ok lab686
		pDlg->m_MobileManager.WaitForATCompleted(cmd,2000);
		cmd = pDlg->m_MobileManager.SendATPacket(CMobileATProto::CheckSignal());//查询信号强度 // ok
		pDlg->m_MobileManager.WaitForATCompleted(cmd,2000);						
		cmd = pDlg->m_MobileManager.SendATPacket(CMobileATProto::CheckHDRSignal());//查询信号强度 // ok
		pDlg->m_MobileManager.WaitForATCompleted(cmd,2000);	
*/
		SetRegValue();	
		Connect();
	}
	else
	{
		
		DisConnect();
	}
}

void COtherDialog::Connect()
{
	CSunnyCatDlg *pDlg = (CSunnyCatDlg*)AfxGetMainWnd();
	if(pDlg->m_bResetting)
	{
		GMessageBox(LoadStringEx(IDS_SEARCH_NET));
		return;
	}
	if(pDlg->m_InitAT.IsInitRunning())
	{
		GMessageBox(LoadStringEx(IDS_INIT_RUNNING));
		return;
	}
	pDlg->Connect();

}

void COtherDialog::DisConnect()
{
	CSunnyCatDlg *pDlg = (CSunnyCatDlg*)AfxGetMainWnd();
	pDlg->Disconnect();
}

void COtherDialog::SetBtnStatus(EBtnStatus status)
{ 
	CSunnyCatDlg *pDlg = (CSunnyCatDlg*)AfxGetMainWnd();
	if(status == STATUS_CONNECT)
	{
		m_ConnectBtn.SetWindowText(LoadStringEx(IDS_CONNECT));
		m_ConnectBtnEvdo.SetWindowText(LoadStringEx(IDS_CONNECT_EVDO));
	}
	else if (status == STATUS_DISCONNECT)
	{
		if ( NETTYPE_EVDO == pDlg->m_linktype )
		{
			m_ConnectBtn.SetWindowText(LoadStringEx(IDS_CONNECT));
			m_ConnectBtnEvdo.SetWindowText(LoadStringEx(IDS_DISCONNECT_EVDO));
		}
		else
		{
			m_ConnectBtn.SetWindowText(LoadStringEx(IDS_DISCONNECT));
			m_ConnectBtnEvdo.SetWindowText(LoadStringEx(IDS_CONNECT_EVDO));
		}
	}

	m_BtnStatus = status;
}

void COtherDialog::InitApn()
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
	TCHAR szValue[1024] = {0};
	DWORD nCount = sizeof(szValue)-1;
	CString m_strApn;	
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
		strApn = "WCDMA Wireless";
	}

	CWnd* pWnd = GetDlgItem(IDC_COMBO_APN);
	CComboBox* pBox = (CComboBox*)pWnd;
	ASSERT(pBox != NULL);
	pBox->ResetContent();

	int nFind = strApn.Find(TEXT("`"));
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
}


