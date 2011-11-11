// SelectNetworkDlg.cpp : implementation file
//

#include "stdafx.h"
#include "sunnycat.h"
#include "SelectNetworkDlg.h"
#include "Util.h"
#include "SunnyCatDlg.h"
#include "GlobalDataCenter.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

/////////////////////////////////////////////////////////////////////////////
// SelectNetworkDlg dialog
SelectNetworkDlg::SelectNetworkDlg(BOOL mode, CWnd* pParent /*=NULL*/)
	: CBmpBkDialog(SelectNetworkDlg::IDD, pParent)
{
	//{{AFX_DATA_INIT(SelectNetworkDlg)
		// NOTE: the ClassWizard will add member initialization here
	m_ThreadCOPS = NULL;
	m_ThreadRegister = NULL;
	m_automode = mode;
	if ( TRUE == m_automode )
	{
		m_rbtnAuto = TRUE;
		m_rbtnManual = FALSE;
	}
	else
	{
		m_rbtnAuto = FALSE;
		m_rbtnManual = TRUE;
	}
	//}}AFX_DATA_INIT
}


void SelectNetworkDlg::DoDataExchange(CDataExchange* pDX)
{
	CBmpBkDialog::DoDataExchange(pDX);
	//{{AFX_DATA_MAP(SelectNetworkDlg)
		// NOTE: the ClassWizard will add DDX and DDV calls here
	DDX_Control(pDX, IDC_BUTTON_SNREFRESH, m_btnRefresh);
	DDX_Control(pDX, IDC_BUTTON_SNREGISTER, m_btnRegister);
	DDX_Control(pDX, IDC_LIST_NETWORK, m_networkList);
	DDX_Check(pDX, IDC_CHECK_AUTO, m_rbtnAuto);
	DDX_Check(pDX, IDC_CHECK_MANUAL, m_rbtnManual);
	//}}AFX_DATA_MAP
}


BEGIN_MESSAGE_MAP(SelectNetworkDlg, CBmpBkDialog)
	//{{AFX_MSG_MAP(SelectNetworkDlg)
	ON_WM_CTLCOLOR()
	ON_BN_CLICKED(IDC_BUTTON_SNREFRESH, OnRefresh)
	ON_BN_CLICKED(IDC_BUTTON_SNREGISTER, OnRegister)
	ON_BN_CLICKED(IDC_CHECK_AUTO, OnAuto)
	ON_BN_CLICKED(IDC_CHECK_MANUAL, OnManual)
	ON_NOTIFY(NM_CLICK, IDC_LIST_NETWORK, OnClickListNetwork)	
	ON_NOTIFY(LVN_ITEMCHANGED, IDC_LIST_NETWORK, OnItemchangedListNetwork)
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// SelectNetworkDlg message handlers

BOOL SelectNetworkDlg::OnInitDialog() 
{
	CBmpBkDialog::OnInitDialog();

	CString strAppPath = GetAppPath()+"\\skin\\";
	
	// TODO: Add extra initialization here

	/* 设置背景图片 */
	CBitmap bmp; 
	bmp.LoadBitmap(IDB_BITMAP_SELNETWORK); 
	m_brush.CreatePatternBrush(&bmp); 
	bmp.DeleteObject();

	/* 设置控件字体 */
	m_font.CreateFont(13,0,0,0,FW_NORMAL,FALSE,FALSE,0,   
		ANSI_CHARSET,OUT_DEFAULT_PRECIS,   
		CLIP_DEFAULT_PRECIS,DEFAULT_QUALITY,   
		DEFAULT_PITCH|FF_SWISS,_T("宋体"));
	GetDlgItem(IDC_CHECK_AUTO)->SetWindowText(LoadStringEx(IDS_REGISTER_AUTO));
	GetDlgItem(IDC_CHECK_MANUAL)->SetWindowText(LoadStringEx(IDS_REGISTER_MANUAL));
	GetDlgItem(IDC_CHECK_AUTO)->SetFont(&m_font);
	GetDlgItem(IDC_CHECK_MANUAL)->SetFont(&m_font);
	GetDlgItem(IDC_LIST_NETWORK)->SetFont(&m_font);

	if ( TRUE == m_automode )
	{
		/* 设置按钮图片 */

		m_btnRefresh.EnableWindow(FALSE);
		m_btnRefresh.SetBitmapPath(strAppPath+"button_nrefresh.bmp");
		m_btnRefresh.SetToolTipTxt(LoadStringEx(IDS_REFRESH));
		m_btnRefresh.Draw();

		m_btnRegister.EnableWindow(FALSE);
		m_btnRegister.SetBitmapPath(strAppPath+"button_nregister.bmp");
		m_btnRegister.SetToolTipTxt(LoadStringEx(IDS_REGISTER));
		m_btnRegister.Draw();
	}
	else
	{
		m_btnRefresh.EnableWindow(TRUE);
		m_btnRefresh.SetBitmapPath(strAppPath+"button_refresh.bmp");
		m_btnRefresh.SetToolTipTxt(LoadStringEx(IDS_REFRESH));
		m_btnRefresh.Draw();

		m_btnRegister.EnableWindow(FALSE);
		m_btnRegister.SetBitmapPath(strAppPath+"button_nregister.bmp");
		m_btnRegister.SetToolTipTxt(LoadStringEx(IDS_REGISTER));
		m_btnRegister.Draw();
	}

	/* 初始化窗体和控件位置大小 */
	CRect temprect(0, 0, 410, 260);
	CWnd::SetWindowPos(NULL, 0, 0, temprect.Width(), temprect.Height(), SWP_NOZORDER|SWP_NOMOVE);
	
	/* 初始化网络系统列表 */
	(void)m_networkList.SetExtendedStyle( LVS_EX_FULLROWSELECT| LVS_EX_INFOTIP);			//隐藏列表线 
	m_networkList.SetHeadings(LoadStringEx(IDS_NETWORK_LISTHEAD));
	m_networkList.LoadColumnInfo();	
	m_networkList.SetTextColor(RGB(100,100,100));

	UpdateData(TRUE);

	return TRUE;  // return TRUE unless you set the focus to a control
	              // EXCEPTION: OCX Property Pages should return FALSE
}

HBRUSH SelectNetworkDlg::OnCtlColor(CDC* pDC, CWnd* pWnd, UINT nCtlColor) 
{
	HBRUSH hbr = CBmpBkDialog::OnCtlColor(pDC, pWnd, nCtlColor);
	
	// TODO: Change any attributes of the DC here	

	/* 控件透明 */
	if(pWnd->GetDlgCtrlID() == IDC_CHECK_AUTO || pWnd->GetDlgCtrlID() == IDC_CHECK_MANUAL )   
	{    
		pDC->SetBkMode(TRANSPARENT);
		m_brush.CreateStockObject(NULL_BRUSH);   
	}

	if (pWnd == this) 
	{
		return (HBRUSH)m_brush; 
	}

	// TODO: Return a different brush if the default is not desired
	return (HBRUSH)m_brush;
}

void SelectNetworkDlg::OnRefresh()
{
	m_networkList.DeleteAllItems_EX();

	if ( TRUE != m_automode )
	{		
		CATCmd cmd; 
		cmd = ((CSunnyCatDlg*)AfxGetMainWnd())->m_MobileManager.SendATPacket(CMobileATProto::QueryAllNet());//查询所有能侦听到的网络
		((CSunnyCatDlg *)AfxGetMainWnd())->m_MobileManager.WaitForATCompleted(cmd,1000);
	
		m_ThreadCOPS = AfxBeginThread(FindCOPS, NULL);
				
		if(wcops.DoModal() == IDOK)
		{
			return;
		}
		CString strAppPath = GetAppPath()+"\\skin\\";

		ShowNetstate();
		
		m_btnRegister.EnableWindow(TRUE);
		m_btnRegister.SetBitmapPath(strAppPath+"button_register.bmp");
		m_btnRegister.SetToolTipTxt(LoadStringEx(IDS_REGISTER));
		m_btnRegister.Draw();
	}
}

void SelectNetworkDlg::OnRegister()
{
	int i = 0;

	if ( TRUE != m_automode )
	{
		CATCmd cmd;
		if ( FindString(choose_name, "CU-GSM") )
		{
			cmd = ((CSunnyCatDlg*)AfxGetMainWnd())->m_MobileManager.SendATPacket(CMobileATProto::RegisterNet("46001"));//注册选择到的网络
		}
		else if ( FindString(choose_name, "3 HK") )
		{
			cmd = ((CSunnyCatDlg*)AfxGetMainWnd())->m_MobileManager.SendATPacket(CMobileATProto::RegisterNet("45403"));//注册选择到的网络
		}
		else if ( FindString(choose_name, "CMCC") )
		{		
			cmd = ((CSunnyCatDlg*)AfxGetMainWnd())->m_MobileManager.SendATPacket(CMobileATProto::RegisterNet("46000"));//注册选择到的网络
		}
		((CSunnyCatDlg *)AfxGetMainWnd())->m_MobileManager.WaitForATCompleted(cmd,1000);

		m_ThreadRegister = AfxBeginThread(RegisterNetwork, NULL);

		CRegisterCOPS wrecops;
		if(wrecops.DoModal() == IDOK)
		{
			return;
		}
	}

	CATCmd cmd;
	cmd = ((CSunnyCatDlg*)AfxGetMainWnd())->m_MobileManager.SendATPacket(CMobileATProto::QueryCurNet());
	((CSunnyCatDlg *)AfxGetMainWnd())->m_MobileManager.WaitForATCompleted(cmd,1500);

	if ( true == ((CSunnyCatDlg*)AfxGetMainWnd())->m_MobileManager.m_IsRegister )
	{
		GMessageBox(LoadStringEx(IDS_REGISTER_OK));
	}
	else
	{
		GMessageBox(LoadStringEx(IDS_REGISTER_FAILTURE));
		if ( 2 == ((CSunnyCatDlg*)AfxGetMainWnd())->m_MobileManager.m_CurFormat )
		{
			cmd = ((CSunnyCatDlg*)AfxGetMainWnd())->m_MobileManager.SendATPacket(CMobileATProto::RegisterNet(((CSunnyCatDlg*)AfxGetMainWnd())->m_MobileManager.m_strCurNet));//注册选择到的网络
		}
		else
		{	
			if ( FindString(((CSunnyCatDlg*)AfxGetMainWnd())->m_MobileManager.m_strCurNet, "CHN-CUGSM") )
			{
				cmd = ((CSunnyCatDlg*)AfxGetMainWnd())->m_MobileManager.SendATPacket(CMobileATProto::RegisterNet("46001"));//注册选择到的网络
			}
			else if ( FindString(((CSunnyCatDlg*)AfxGetMainWnd())->m_MobileManager.m_strCurNet, "3 HK") )
			{
				cmd = ((CSunnyCatDlg*)AfxGetMainWnd())->m_MobileManager.SendATPacket(CMobileATProto::RegisterNet("45403"));//注册选择到的网络
			}
			else if ( FindString(((CSunnyCatDlg*)AfxGetMainWnd())->m_MobileManager.m_strCurNet, "CHINA  MOBILE") )
			{		
				cmd = ((CSunnyCatDlg*)AfxGetMainWnd())->m_MobileManager.SendATPacket(CMobileATProto::RegisterNet("46000"));//注册选择到的网络
			}
		}
		((CSunnyCatDlg *)AfxGetMainWnd())->m_MobileManager.WaitForATCompleted(cmd,1500);
	}

}

/* 自动注册 */
void SelectNetworkDlg::OnAuto()
{
	// TODO: Add your control notification handler code here
	UpdateData(TRUE);

	CString strAppPath = GetAppPath()+"\\skin\\";
	
	m_networkList.DeleteAllItems_EX();

	m_automode = TRUE;
	/* 设置按钮图片 */
	m_btnRefresh.EnableWindow(FALSE);
	m_btnRefresh.SetBitmapPath(strAppPath+"button_nrefresh.bmp");
	m_btnRefresh.SetToolTipTxt(LoadStringEx(IDS_REFRESH));
	m_btnRefresh.Draw();

	m_btnRegister.EnableWindow(FALSE);
	m_btnRegister.SetBitmapPath(strAppPath+"button_nregister.bmp");
	m_btnRegister.SetToolTipTxt(LoadStringEx(IDS_REGISTER));
	m_btnRegister.Draw();

	UpdateData(FALSE);
}

/* 手动注册 */
void SelectNetworkDlg::OnManual()
{
	// TODO: Add your control notification handler code here
	UpdateData(TRUE);

	CString strAppPath = GetAppPath()+"\\skin\\";

	m_automode = FALSE;
	
	/* 设置按钮图片 */
	m_btnRefresh.EnableWindow(TRUE);
	m_btnRefresh.SetBitmapPath(strAppPath+"button_refresh.bmp");
	m_btnRefresh.Draw();

	m_btnRegister.EnableWindow(FALSE);
	m_btnRegister.SetBitmapPath(strAppPath+"button_nregister.bmp");
	m_btnRegister.Draw();
	
	UpdateData(FALSE);	
}

void SelectNetworkDlg::OnClickListNetwork(NMHDR* pNMHDR, LRESULT* pResult) 
{
	// TODO: Add your control notification handler code here
	
	choose_name = m_networkList.GetItemText(m_nCurSelItemNetwork, 0);;
}

void SelectNetworkDlg::OnItemchangedListNetwork(NMHDR* pNMHDR, LRESULT* pResult) 
{
	NM_LISTVIEW* pNMListView = (NM_LISTVIEW*)pNMHDR;
	// TODO: Add your control notification handler code here
	
	m_nCurSelItemNetwork = pNMListView->iItem;
}

// 显示当前搜索到的网络
void SelectNetworkDlg::ShowNetstate()
{
	int i = 0;
	int j = 0;
	int count,colon_count;
	int str_len = 0;
		
	count = ((CSunnyCatDlg*)AfxGetMainWnd())->m_MobileManager.m_AllNet.GetSize(); 
	for (i = 0; i<count; i++)
	{
		CString NetState = _T("");
		CString NetName = _T("");
		CString NetRegister = _T("");
		CString Net3G = _T("");			
	
		colon_count = 0;

		NetState = ((CSunnyCatDlg*)AfxGetMainWnd())->m_MobileManager.m_AllNet.GetAt(i);

		str_len = NetState.GetLength();

		NetRegister = NetState.GetAt(0);
		Net3G = NetState.GetAt(str_len - 1);

		for (j = 1; j<str_len; j++)
		{
			if ( '"' == NetState.GetAt(j) )
			{
				colon_count ++;
			}
			else
			{
				if ( 3 == colon_count )
				{		
					NetName += NetState.GetAt(j);
				}
				if ( 4 == colon_count )
				{
					break;
				}
			}
		}
		
		if ( "0" == Net3G )
		{			
			if ( "1" == NetRegister )
			{
				m_networkList.AddItem(i, NetName+"(2G)" , TEXT("Available"));
			}
			else if ( "2" == NetRegister )
			{
				m_networkList.AddItem(i, NetName+"(2G)" , TEXT("Register"));
			}
			else if ( "3" == NetRegister )
			{
				m_networkList.AddItem(i, NetName+"(2G)" , TEXT("Forbidden"));
			}
		}
		else if ( "2" == Net3G )
		{
			if ( "1" == NetRegister )
			{
				m_networkList.AddItem(i, NetName+"(3G)" , TEXT("Available"));
			}
			else if ( "2" == NetRegister )
			{
				m_networkList.AddItem(i, NetName+"(3G)" , TEXT("Register"));
			}
			else if ( "3" == NetRegister )
			{
				m_networkList.AddItem(i, NetName+"(3G)" , TEXT("Forbidden"));
			}
		}
		
	}

	((CSunnyCatDlg*)AfxGetMainWnd())->m_MobileManager.m_AllNet.RemoveAll();
}

UINT FindCOPS(LPVOID lpParameter)
{
	Sleep(45000);

	GMessageBox(LoadStringEx(IDS_FINISH_COPS));
	
	return 0;	
}

UINT RegisterNetwork(LPVOID lpParameter)
{
	Sleep(5000);

	GMessageBox(LoadStringEx(IDS_FINISH_REGISTER));
	
	return 0;	
}

