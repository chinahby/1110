// NetStatByTime.cpp : implementation file
//

#include "stdafx.h"
#include "sunnycat.h"
#include "NetStatByTime.h"
#include "CDMATime.h"
#include "Util.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

/////////////////////////////////////////////////////////////////////////////
// CNetStatByTime dialog


CNetStatByTime::CNetStatByTime(CWnd* pParent /*=NULL*/)
	: CBmpBkDialog(CNetStatByTime::IDD, pParent)
{
	//{{AFX_DATA_INIT(CNetStatByTime)
	m_StartTime = 0;
	m_EndTime = 0;
	//}}AFX_DATA_INIT
}


void CNetStatByTime::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	//{{AFX_DATA_MAP(CNetStatByTime)
	DDX_DateTimeCtrl(pDX, IDC_DATETIMEPICKER_START, m_StartTime);
	DDX_DateTimeCtrl(pDX, IDC_DATETIMEPICKER_END, m_EndTime);
	//}}AFX_DATA_MAP
}


BEGIN_MESSAGE_MAP(CNetStatByTime, CBmpBkDialog)
	//{{AFX_MSG_MAP(CNetStatByTime)
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CNetStatByTime message handlers

BOOL CNetStatByTime::OnInitDialog() 
{
	CBmpBkDialog::OnInitDialog();
	
	// TODO: Add extra initialization here
	
	SetWindowText(LoadStringEx(IDS_NET_COUNT));
	GetDlgItem(IDC_STATIC_START)->SetWindowText(LoadStringEx(IDS_START_TIME));
	GetDlgItem(IDC_STATIC_END)->SetWindowText(LoadStringEx(IDS_END_TIME));
	GetDlgItem(IDOK)->SetWindowText(LoadStringEx(IDS_OK));
	GetDlgItem(IDCANCEL)->SetWindowText(LoadStringEx(IDS_CANCEL));
	m_EndTime = CCDMATime::Instance().GetCDMATime();
	m_StartTime = m_EndTime;

	UpdateData(FALSE);

	return TRUE;  // return TRUE unless you set the focus to a control
	              // EXCEPTION: OCX Property Pages should return FALSE
}

void CNetStatByTime::OnOK() 
{
	// TODO: Add extra validation here
	UpdateData(TRUE);
	if(m_StartTime > m_EndTime)
	{
		GMessageBox(LoadStringEx(IDS_STARTGREAT_ENDTIME));
		return;
	}
	CBmpBkDialog::OnOK();
}
