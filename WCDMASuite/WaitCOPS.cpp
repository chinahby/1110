// WaitCOPS.cpp : implementation file
//

#include "stdafx.h"
#include "sunnycat.h"
#include "WaitCOPS.h"
#include "SelectNetworkDlg.h"
#include "Util.h"
#include "SunnyCatDlg.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

/////////////////////////////////////////////////////////////////////////////
// CWaitCOPS dialog


CWaitCOPS::CWaitCOPS(CWnd* pParent /*=NULL*/)
	: CBmpBkDialog(CWaitCOPS::IDD, pParent)
{
	//{{AFX_DATA_INIT(CWaitCOPS)
		// NOTE: the ClassWizard will add member initialization here
	//}}AFX_DATA_INIT
}


void CWaitCOPS::DoDataExchange(CDataExchange* pDX)
{
	CBmpBkDialog::DoDataExchange(pDX);
	//{{AFX_DATA_MAP(CWaitCOPS)
		// NOTE: the ClassWizard will add DDX and DDV calls here
	//}}AFX_DATA_MAP
}


BEGIN_MESSAGE_MAP(CWaitCOPS, CBmpBkDialog)
	//{{AFX_MSG_MAP(CWaitCOPS)
	ON_WM_CTLCOLOR()
	ON_WM_TIMER()
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CWaitCOPS message handlers

BOOL CWaitCOPS::OnInitDialog() 
{
	CBmpBkDialog::OnInitDialog();
	
	// TODO: Add extra initialization here

	/* ÉèÖÃ±³¾°Í¼Æ¬ */
	CBitmap bmp; 
	bmp.LoadBitmap(IDB_BITMAP_SELNETWORK); 
	m_brush.CreatePatternBrush(&bmp); 
	bmp.DeleteObject();

	/* ÉèÖÃ¿Ø¼þ×ÖÌå */
	m_font.CreateFont(13,0,0,0,FW_NORMAL,FALSE,FALSE,0,   
		ANSI_CHARSET,OUT_DEFAULT_PRECIS,   
		CLIP_DEFAULT_PRECIS,DEFAULT_QUALITY,   
		DEFAULT_PITCH|FF_SWISS,_T("ËÎÌå"));
	GetDlgItem(IDC_STATIC_REGISTER)->SetWindowText(LoadStringEx(IDS_WAIT_COPS));
	GetDlgItem(IDC_STATIC_REGISTER)->SetFont(&m_font);

	return TRUE;  // return TRUE unless you set the focus to a control
	              // EXCEPTION: OCX Property Pages should return FALSE
}

HBRUSH CWaitCOPS::OnCtlColor(CDC* pDC, CWnd* pWnd, UINT nCtlColor) 
{
	HBRUSH hbr = CBmpBkDialog::OnCtlColor(pDC, pWnd, nCtlColor);
	
	// TODO: Change any attributes of the DC here
	
	pDC->SetBkMode(TRANSPARENT);

	if (pWnd == this) 
	{	
		return (HBRUSH)m_brush; 
	}

	// TODO: Return a different brush if the default is not desired
	return (HBRUSH)m_brush;

}

void CWaitCOPS::OnTimer(UINT nIDEvent) 
{
	// TODO: Add your message handler code here and/or call default

//	KillTimer(1);

//	CBmpBkDialog::OnDestroy();
	
	CBmpBkDialog::OnTimer(nIDEvent);
}