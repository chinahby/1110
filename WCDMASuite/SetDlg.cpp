// SetDlg.cpp : implementation file
//

#include "stdafx.h"
#include "SunnyCat.h"
#include "SetDlg.h"
#include "GlobalDataCenter.h"
#include "SunnyCatDlg.h"
#include "Porting.h"
#include <atlbase.h>
#include "Util.h"
#include "DevEnum.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

/////////////////////////////////////////////////////////////////////////////
// CSetDlg dialog


CSetDlg::CSetDlg(CWnd* pParent /*=NULL*/)
	: CBmpBkDialog(CSetDlg::IDD, pParent)
{
	//{{AFX_DATA_INIT(CSetDlg)
		// NOTE: the ClassWizard will add member initialization here
	//}}AFX_DATA_INIT

	g_pSysImageList = NULL;
}


void CSetDlg::DoDataExchange(CDataExchange* pDX)
{
	CBmpBkDialog::DoDataExchange(pDX);
	//{{AFX_DATA_MAP(CSetDlg)
	DDX_Control(pDX, IDC_TAB_SET, m_setTable);
	//}}AFX_DATA_MAP
}


BEGIN_MESSAGE_MAP(CSetDlg, CBmpBkDialog)
	//{{AFX_MSG_MAP(CSetDlg)
	ON_BN_CLICKED(IDC_BUTTON_RENEW, OnButtonRenew)
	//}}AFX_MSG_MAP
	ON_MESSAGE(WM_REFRESH_COMBOBOX,OnRefreshComboBox)
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CSetDlg message handlers

BOOL CSetDlg::OnInitDialog() 
{
	CBmpBkDialog::OnInitDialog();
	
	SetWindowText(LoadStringEx(IDS_SETTING));
	GetDlgItem(IDC_BUTTON_RENEW)->SetWindowText(LoadStringEx(IDS_DEFAULT_SET));
	GetDlgItem(IDOK)->SetWindowText(LoadStringEx(IDS_OK));
	GetDlgItem(IDCANCEL)->SetWindowText(LoadStringEx(IDS_CANCEL));

	CSize size;
	size.cx = 80;
	size.cy = 30;
	m_setTable.SetRedraw();
	m_setTable.SetItemSize(size);
	
	//CImageList*  g_pSysImageList = NULL;
	g_pSysImageList   = new CImageList();
	CBitmap IconBitmap;
//	IconBitmap.LoadBitmap(IDB_SMALL_TOOLBAR);
	CString strLanguage = GetLanguage();
//	if(strLanguage == "-En")
//		IconBitmap.Attach(LoadBmpFormFile(TEXT("skin//English//SAMLLTOOLBAR.bmp")));
//	else if(strLanguage == "-Ru")
		IconBitmap.Attach(LoadBmpFormFile(TEXT("skin//SAMLLTOOLBAR.bmp")));

	
//	g_pSysImageList->Create(20,20,ILC_MASK|ILC_COLOR24,1,1) ;
	g_pSysImageList->Create(80,30,ILC_MASK|ILC_COLOR24,1,1) ;
	g_pSysImageList->Add( &IconBitmap,(COLORREF)0xFFFFFF );
	IconBitmap.DeleteObject();
//	m_setTable.SetItemState(1, TCIF_IMAGE,TCIS_HIGHLIGHTED );
//	m_setTable.SetImageList(g_pSysImageList);

	m_setTable.SetItemState(1, TCIF_IMAGE,TCIS_HIGHLIGHTED );
	m_setTable.SetImageList(g_pSysImageList);

	
	m_setTable.AddPage(TEXT(""), &m_setConnectDlg, IDD_DIALOG_SETCONNECT);
	m_setTable.AddPage(TEXT(""), &m_setRingDlg, IDD_DIALOG_SETRING);
	m_setTable.AddPage(TEXT(""), &m_setSysDlg, IDD_DIALOG_SYSSET);

//	if(strLanguage == "-Ch")				//中文
//		m_setTable.AddPage("", &m_FluxWarnDlg, IDD_DIALOG_FLUXSTAT);


	// TODO: Add extra initialization here
	m_setTable.Show();
	
	return TRUE;  // return TRUE unless you set the focus to a control
	              // EXCEPTION: OCX Property Pages should return FALSE
}

void CSetDlg::OnOK() 
{
	// TODO: Add extra validation here
	if(m_setConnectDlg.m_bSearchCops)
	{
		GMessageBox(LoadStringEx(IDS_SP_NOTICE));
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
	
	CWnd* pWnd = NULL;
	CString strText;

	strText.Empty();

	pWnd = m_setRingDlg.GetDlgItem(IDC_EDIT_CALLRING);
	pWnd->GetWindowText(strText);	
	reg.SetValue(strText, TEXT("CallRing"));

	pWnd = m_setRingDlg.GetDlgItem(IDC_EDIT_SMSRING);
	pWnd->GetWindowText(strText);	
	reg.SetValue(strText, TEXT("SMSRing"));

	reg.SetValue(m_setSysDlg.m_bMinToTask,TEXT("MinToTask"));
	reg.SetValue(m_setSysDlg.m_bShowWindow,TEXT("AutoShowWindow"));
	reg.SetValue(m_setSysDlg.m_bAutoUpdate,TEXT("AutoUpdate"));
	pWnd = m_setSysDlg.GetDlgItem(IDC_RADIO_VOICEOUT);
	DWORD dwValue = 0;
	reg.QueryValue(dwValue,TEXT("VoiceChanel"));
	int iCh = 0;
	CButton *pButton = static_cast<CButton*>(pWnd);

	/**************************************************/
	int nPCMVoice = 0;
	if(pButton->GetCheck() ==1)
	{
		reg.SetValue(1,TEXT("VoiceChanel"));
		nPCMVoice = 0;
	}
	else
	{
		reg.SetValue(2,TEXT("VoiceChanel"));
		nPCMVoice = 2;
	}
	/**************************************************/
	
	
	CSunnyCatDlg *pDlg = static_cast<CSunnyCatDlg*>(AfxGetMainWnd());
	
	
	CATCmd cmd = pDlg->m_MobileManager.SendATPacket(CMobileATProto::SetPCMSWitch(nPCMVoice));
	pDlg->m_MobileManager.WaitForATCompleted(cmd,1000);

	SetAutoRunOnStart(m_setSysDlg.m_bAutoRun?true:false) ;

//	SetWorkMode(m_setSysDlg.m_nUseType);

	
	pWnd = m_setConnectDlg.GetDlgItem(IDC_EDIT_USER);
	pWnd->GetWindowText(strText);
	reg.SetValue(strText,TEXT("UserName"));

	pWnd = m_setConnectDlg.GetDlgItem(IDC_EDIT_PASSWORD);
	pWnd->GetWindowText(strText);
	reg.SetValue(strText,TEXT("PassWord"));

	pWnd = m_setConnectDlg.GetDlgItem(IDC_EDIT_PHONENUM);
	pWnd->GetWindowText(strText);
	reg.SetValue(strText,TEXT("Phone"));

	pWnd = m_setConnectDlg.GetDlgItem(IDC_COMBO_APN);
	pWnd->GetWindowText(strText);
	reg.SetValue(strText,TEXT("Apn"));

	CString strLanguage = GetLanguage();
/*	if(strLanguage == "-Ch")
	{
		m_FluxWarnDlg.UpdateData(TRUE);
		long nUncntNM = atol(m_FluxWarnDlg.m_NmUncnt);
		long nWarnNM = atol(m_FluxWarnDlg.m_NmWarn);
		if(nUncntNM < nWarnNM)
		{
			GMessageBox(::LoadStringEx(IDS_FLUX_ERRORSET),::LoadStringEx(IDS_FLUX_ERRORSETTITLE),MB_OK|MB_ICONEXCLAMATION);
			reg.Close();
			return;
		}
		reg.SetValue(m_FluxWarnDlg.m_bYN,"UseFluxStat");
		reg.SetValue(m_FluxWarnDlg.m_bUncnt,"bUncnt");
		reg.SetValue(m_FluxWarnDlg.m_Date,"Date");
		CString strNM;
		if(atoi(m_FluxWarnDlg.m_NmUncnt) <= 0)
		{
			strNM = "1";
		}
		else
		{
			strNM = m_FluxWarnDlg.m_NmUncnt;
		}
		reg.SetValue(strNM,"UncntNM");

		if(atoi(m_FluxWarnDlg.m_NmWarn) <= 0)
		{
			strNM = "1";
		}
		else
		{
			strNM = m_FluxWarnDlg.m_NmWarn;
		}
		reg.SetValue(strNM,"WarnNM");

		if(atoi(m_FluxWarnDlg.m_Nm) <= 0)
		{
			strNM = "1";
		}
		else
		{
			strNM = m_FluxWarnDlg.m_Nm;
		}
		reg.SetValue(strNM,"NM");
	}
	*/
	pDlg = NULL;
	pDlg = (CSunnyCatDlg*)AfxGetMainWnd();
//	pDlg->m_MobileManager.installModemInitialAT(strText);
	pDlg->m_LinkDlg.InitApn();
	reg.Close();

	delete g_pSysImageList;

	CDialog::OnOK();
}

void CSetDlg::OnButtonRenew() 
{
	// TODO: Add your control notification handler code here
	//int nCurrsel = m_setTable.GetCurSel();

	//switch(nCurrsel)
	//{
//	case 0:	//网络设置
//		break;
//	case 1:	//铃音设置
//		break;
//	case 2:	//系统设置
//		break;
//	}
	CSunnyCatDlg* pDlg = (CSunnyCatDlg*)AfxGetMainWnd();
	if((pDlg->m_MobileManager.isAbleCom)&&
		(pDlg->m_MobileManager.m_bSimExist))
	{
		if(m_setConnectDlg.m_bSearchCops)
		{
			GMessageBox(LoadStringEx(IDS_SP_NOTICE));
			return;
		}
	}
	m_setConnectDlg.m_strUser.Empty();
	m_setConnectDlg.m_strPassWord.Empty();
	m_setConnectDlg.m_strPhoneNum = "*99#";
	m_setConnectDlg.m_strApn = "WCDMA Wireless";
	
	m_setConnectDlg.UpdateData(FALSE);
		int nApnCount = m_setConnectDlg.m_ApnCombo.GetCount();
		CString strComboApn;
		int nCurSel = -1;
		for(int i=0; i < nApnCount; i++ )
		{
			m_setConnectDlg.m_ApnCombo.GetLBText(i,strComboApn);
			if(strComboApn == "WCDMA Wireless")
				nCurSel = i;
		}
		
		m_setConnectDlg.m_ApnCombo.SetCurSel(nCurSel);
	
	CString strCallRing;
	CString strSMSRing;
	CWnd* pWnd = NULL;
	
	strCallRing = GetAppPath() + "music\\ring.wav";
	strSMSRing = GetAppPath() + "music\\sms.wav";
	
	pWnd = m_setRingDlg.GetDlgItem(IDC_EDIT_CALLRING);
	pWnd->SetWindowText(strCallRing);
	
	pWnd = m_setRingDlg.GetDlgItem(IDC_EDIT_SMSRING);
	pWnd->SetWindowText(strSMSRing);
	
	m_setRingDlg.UpdateData(FALSE);
	
	m_setSysDlg.m_bMinToTask = false;
	m_setSysDlg.m_bShowWindow = false;
	m_setSysDlg.m_bAutoRun = false;
	m_setSysDlg.m_bAutoUpdate = FALSE;
	m_setSysDlg.UpdateData(FALSE);

	pWnd = m_setSysDlg.GetDlgItem(IDC_RADIO_VOICEOUT);
	CButton *pButton = static_cast<CButton*>(pWnd);
	pButton->SetCheck(1);
	

	pWnd = m_setSysDlg.GetDlgItem(IDC_RADIO_VOICEPC);
	pButton = static_cast<CButton*>(pWnd);
	pButton->SetCheck(0);


/*	CString strLanguage = GetLanguage();
	if(strLanguage == "-Ch")
	{
		m_FluxWarnDlg.m_bYN = TRUE;
		m_FluxWarnDlg.m_bUncnt = FALSE;
		m_FluxWarnDlg.m_Date = "1";
		m_FluxWarnDlg.m_NmUncnt = "1000";
		m_FluxWarnDlg.m_NmWarn = "800";
		m_FluxWarnDlg.m_Nm = "10";
		m_FluxWarnDlg.UpdateData(FALSE);
	}
*/

	
}

bool CSetDlg::IsModemExit(CString strModem)
{
	CModemList modemList;

	std::vector<size_t> vMatched;
	for(size_t i=0; i<modemList.Count(); ++i)
	{
		const TModemInfo& info = modemList.GetAt(i);
		if(info.strFriendlyName == strModem)
		{
			
			return true;
		}
	}

	return false;
}

LRESULT CSetDlg::OnRefreshComboBox(WPARAM wParam,LPARAM lParam)
{
	m_setConnectDlg.RefreshComBox();
	return 1;
}

void CSetDlg::OnClose() 
{
	// TODO: Add your message handler code here and/or call default
	CSunnyCatDlg* pDlg = (CSunnyCatDlg*)AfxGetMainWnd();
	if((pDlg->m_MobileManager.isAbleCom)&&
		(pDlg->m_MobileManager.m_bSimExist))
	{
		if(m_setConnectDlg.m_bSearchCops)
		{
			GMessageBox(LoadStringEx(IDS_SP_NOTICE));
			return;
		}
	}
	CBmpBkDialog::OnClose();
}

void CSetDlg::OnCancel() 
{
	// TODO: Add extra cleanup here
	CSunnyCatDlg* pDlg = (CSunnyCatDlg*)AfxGetMainWnd();
	if((pDlg->m_MobileManager.isAbleCom)&&
		(pDlg->m_MobileManager.m_bSimExist))
	{
		if(m_setConnectDlg.m_bSearchCops)
		{
			GMessageBox(LoadStringEx(IDS_SP_NOTICE));
			return;
		}
	}
	
	CBmpBkDialog::OnCancel();
}
