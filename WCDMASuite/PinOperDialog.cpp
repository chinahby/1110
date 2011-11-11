// PinOperDialog.cpp : implementation file
//

#include "stdafx.h"
#include "sunnycat.h"
#include "PinOperDialog.h"
#include "SunnyCatDlg.h"
#include "GlobalDataCenter.h"
#include "Util.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

/////////////////////////////////////////////////////////////////////////////
// CPinOper dialog


CPinOper::CPinOper(CWnd* pParent /*=NULL*/)
	: CBmpBkDialog(CPinOper::IDD, pParent)
{
	//{{AFX_DATA_INIT(CPinOper)
	m_strPIN = _T("");
//	m_strNumSIM = _T("");
	if (((CSunnyCatDlg *)AfxGetMainWnd())->m_bPINenordis)
	{
		m_iEnableorDisable = 1;	//¿ªËø
	}
	else
	{
		m_iEnableorDisable = 0;	//½âËø
	}
	m_iremaintime = ((CSunnyCatDlg *)AfxGetMainWnd())->m_MobileManager.m_uremainingtime;
	m_bpinpuk	  = ((CSunnyCatDlg *)AfxGetMainWnd())->m_MobileManager.m_bpinpuk;
	//}}AFX_DATA_INIT
}


void CPinOper::DoDataExchange(CDataExchange* pDX)
{
	CBmpBkDialog::DoDataExchange(pDX);
	//{{AFX_DATA_MAP(CPinOper)
	DDX_Control(pDX, IDC_EDIT_PINOPERCODE, m_Editpin);
	DDX_Control(pDX, IDC_TEXT_PINOPERCODE, m_pincode);
	DDX_Control(pDX, IDC_PINOPER_REMAINING, m_remaintime);
	DDX_Text(pDX, IDC_EDIT_PINOPERCODE, m_strPIN);
	DDV_MaxChars(pDX, m_strPIN, 10);	
	//}}AFX_DATA_MAP
}


BEGIN_MESSAGE_MAP(CPinOper, CBmpBkDialog)
	//{{AFX_MSG_MAP(CPinOper)
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CPinOper message handlers

void CPinOper::OnOK() 
{
	// TODO: Add extra validation here
	UpdateData(true);
	if (m_bpinpuk)
	{	
//		TRACE(TEXT("m_strPIN = %s\n"), ToMultiBytes((LPCTSTR)m_strPIN).c_str());
		if( m_strPIN.GetLength() < 4 || m_strPIN.GetLength() > 8)
		{
			GMessageBox(LoadStringEx(IDC_STATIC_INPUTPINLENERROR));
			return;
		}
		if (false == OnPinlock() )
		{
			GMessageBox(LoadStringEx(IDC_STATIC_INPUTPINERROR));
			m_strPIN = "";
			UpdateData(true);
			return ;
		}
	}
	else
	{
//		TRACE(TEXT("m_strPIN = %s\n"), ToMultiBytes((LPCTSTR)m_strPIN).c_str());
		if( m_strPIN.GetLength() != 8)
		{
			GMessageBox(LoadStringEx(IDC_STATIC_INPUTPUKLENERROR));
			return;
		}
		if (false == OnPuklock() )
		{
			GMessageBox(LoadStringEx(IDC_STATIC_INPUTPUKERROR));
			m_strPIN = "";
			UpdateData(true);
			return ;
		}
		else
		{
			GMessageBox(LoadStringEx(IDC_STATIC_UNLOCKPUKSUCC));
		}
	}
	CATCmd cmd;
	cmd = ((CSunnyCatDlg *)AfxGetMainWnd())->m_MobileManager.SendATPacket(CMobileATProto::CheckCPIN());
	((CSunnyCatDlg *)AfxGetMainWnd())->m_MobileManager.WaitForATCompleted(cmd,1000);

	cmd = ((CSunnyCatDlg *)AfxGetMainWnd())->m_MobileManager.SendATPacket(CMobileATProto::TestSimLock());
	((CSunnyCatDlg *)AfxGetMainWnd())->m_MobileManager.WaitForATCompleted(cmd,1000);
	CDialog::OnOK();
}

BOOL CPinOper::OnInitDialog() 
{
	CBmpBkDialog::OnInitDialog();

	SetWindowText(LoadStringEx(IDC_PINDIALOG_WINTEXT));
	if (m_bpinpuk == true)
	{
		m_pincode.SetWindowText(LoadStringEx(IDC_STATIC_PINCODE));
	}
	else
	{
		m_pincode.SetWindowText(LoadStringEx(IDC_STATIC_PUKCODE));
	}

	CString strRecv;
	strRecv.Format(TEXT("%d %s"), m_iremaintime, LoadStringEx(IDC_STATIC_PINREMAINING));
	m_remaintime.SetWindowText(strRecv);//LoadStringEx(IDC_STATIC_PINREMAINING));
	GetDlgItem(IDOK)->SetWindowText(LoadStringEx(IDS_OK));
	GetDlgItem(IDCANCEL)->SetWindowText(LoadStringEx(IDS_CANCEL));

	UpdateData(true);

	// TODO: Add extra initialization here
	
	return TRUE;  // return TRUE unless you set the focus to a control
	              // EXCEPTION: OCX Property Pages should return FALSE
}

bool CPinOper::OnPinlock()
{
	CATCmd cmd;
	cmd = ((CSunnyCatDlg *)AfxGetMainWnd())->m_MobileManager.SendATPacket(CMobileATProto::SETSimLock(m_iEnableorDisable, m_strPIN));
	if (RETURN_OK == ((CSunnyCatDlg *)AfxGetMainWnd())->m_MobileManager.WaitForATCompleted(cmd,1000) )
	{
		return true;
	}
	else
	{
		GetRemaintime();
		return false;
	}
}
bool CPinOper::OnPuklock()
{
	CATCmd cmd;
	CString NewPin;
	NewPin.Format(TEXT("1234"));
	cmd = ((CSunnyCatDlg *)AfxGetMainWnd())->m_MobileManager.SendATPacket(CMobileATProto::InputPUK1(m_strPIN, NewPin));
	if (RETURN_OK == ((CSunnyCatDlg *)AfxGetMainWnd())->m_MobileManager.WaitForATCompleted(cmd,1000) )
	{
		return true;
	}
	else
	{
		GetRemaintime();
		return false;
	}
}

void CPinOper::GetRemaintime()
{
	CATCmd cmd;
	cmd = ((CSunnyCatDlg *)AfxGetMainWnd())->m_MobileManager.SendATPacket(CMobileATProto::CheckCPIN());
	((CSunnyCatDlg *)AfxGetMainWnd())->m_MobileManager.WaitForATCompleted(cmd,2000);

	cmd = ((CSunnyCatDlg *)AfxGetMainWnd())->m_MobileManager.SendATPacket(CMobileATProto::TestSimLock());
	((CSunnyCatDlg *)AfxGetMainWnd())->m_MobileManager.WaitForATCompleted(cmd,2000);

	m_iremaintime = ((CSunnyCatDlg *)AfxGetMainWnd())->m_MobileManager.m_uremainingtime;
	m_bpinpuk	  = ((CSunnyCatDlg *)AfxGetMainWnd())->m_MobileManager.m_bpinpuk;

	if (m_bpinpuk == true)
	{
		m_pincode.SetWindowText(LoadStringEx(IDC_STATIC_PINCODE));
	}
	else
	{
		m_pincode.SetWindowText(LoadStringEx(IDC_STATIC_PUKCODE));
	}

	CString strRecv;
//	strRecv = Util::intToString(m_iremaintime) + LoadStringEx(IDC_STATIC_PINREMAINING);
	strRecv.Format(TEXT("%d %s"), m_iremaintime, LoadStringEx(IDC_STATIC_PINREMAINING));
	m_remaintime.SetWindowText(strRecv);
	
	UpdateData(true);
}