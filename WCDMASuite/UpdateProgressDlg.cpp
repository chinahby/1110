// UpdateProgressDlg.cpp : implementation file
//

#include "stdafx.h"
#include "sunnycat.h"
#include "UpdateProgressDlg.h"
#include "Util.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

/////////////////////////////////////////////////////////////////////////////
// CUpdateProgressDlg dialog


CUpdateProgressDlg::CUpdateProgressDlg(CEvent& EventStopUpdate,CWnd* pParent /*=NULL*/)
	: CBmpBkDialog(CUpdateProgressDlg::IDD, pParent),m_StopEvent(EventStopUpdate)
{
	//{{AFX_DATA_INIT(CUpdateProgressDlg)
		// NOTE: the ClassWizard will add member initialization here
	//}}AFX_DATA_INIT
}


void CUpdateProgressDlg::DoDataExchange(CDataExchange* pDX)
{
	CBmpBkDialog::DoDataExchange(pDX);
	//{{AFX_DATA_MAP(CUpdateProgressDlg)
	DDX_Control(pDX, IDC_PROGRESS_UPDATE, m_ctrlProgress);
	//}}AFX_DATA_MAP
}


BEGIN_MESSAGE_MAP(CUpdateProgressDlg, CBmpBkDialog)
	//{{AFX_MSG_MAP(CUpdateProgressDlg)
	ON_WM_PAINT()
	ON_WM_CLOSE()
	ON_WM_CTLCOLOR()
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CUpdateProgressDlg message handlers

BOOL CUpdateProgressDlg::OnInitDialog() 
{
	CBmpBkDialog::OnInitDialog();
	
	// TODO: Add extra initialization here


//	CWnd* pWnd = GetDlgItem(IDC_PROGRESS_UPDATE);
//	pWnd->CenterWindow(this);

	SetWindowText(::LoadStringEx(IDS_UPDATE_TITLE));
	return TRUE;  // return TRUE unless you set the focus to a control
	              // EXCEPTION: OCX Property Pages should return FALSE
}

void CUpdateProgressDlg::OnPaint() 
{
	CBmpBkDialog::OnPaint();
	CPaintDC dc(this); // device context for painting
	
	// TODO: Add your message handler code here
	CBitmap m_BkBmp ;
	//m_BkBmp.Attach(LoadBmpFormFile("press.bmp"))  ;
	m_BkBmp.LoadBitmap(IDB_BITMAP_PRESS);
	BITMAP bmp = {0};
	int nRet = m_BkBmp.GetBitmap(&bmp);
	ASSERT(nRet != 0);

	if(m_BkBmp.GetSafeHandle())
	{
		CRect rc ;
		GetWindowRect(rc) ;
		CDC memDC;
		memDC.CreateCompatibleDC( &dc );//
		memDC.SelectObject( &m_BkBmp );
		dc.StretchBlt(0,0,rc.Width(),rc.Height(),&memDC,0,0,bmp.bmWidth,bmp.bmHeight,SRCCOPY);
		m_BkBmp.DeleteObject() ;
		memDC.DeleteDC() ;
		ReleaseDC(&memDC) ;
	}
	// Do not call CBmpBkDialog::OnPaint() for painting messages
}

void CUpdateProgressDlg::OnClose() 
{
	// TODO: Add your message handler code here and/or call default
	
	CBmpBkDialog::OnClose();

	if( GMessageBox(::LoadStringEx(IDS_UPDATE_CONFIRMEXIT),NULL,MB_OKCANCEL|MB_ICONQUESTION) == IDOK)
	{
		m_StopEvent.SetEvent();
		
		EndDialog(0);
	}
}

HBRUSH CUpdateProgressDlg::OnCtlColor(CDC* pDC, CWnd* pWnd, UINT nCtlColor) 
{
	HBRUSH hbr = CBmpBkDialog::OnCtlColor(pDC, pWnd, nCtlColor);
	
	// TODO: Change any attributes of the DC here
	if(pWnd->GetDlgCtrlID() == IDC_PROGRESS)
	{
		pDC->SetBkMode(TRANSPARENT);
		HBRUSH hNewbr  =  CreateSolidBrush(RGB(255,255,255));
		DeleteObject(hNewbr);
		return hNewbr;
	}
	// TODO: Return a different brush if the default is not desired
	return hbr;
}

void CUpdateProgressDlg::SetProgress(int nProgress, int max)
{
	m_ctrlProgress.SetRange(0,max);
	m_ctrlProgress.SetPos(nProgress);

	CWnd* pWnd = GetDlgItem(IDC_STATIC_PERCENT);
	CString strUpdate;
	strUpdate = ::LoadStringEx(IDS_UPDATE_PERCENT);
	CString strPercent;
	strPercent.Format(TEXT("%d%%"),nProgress);
	strUpdate += strPercent;
	pWnd->SetWindowText(strUpdate);
}

void CUpdateProgressDlg::OnOK()
{
	return;
}

void CUpdateProgressDlg::OnCancel()
{
	return;
}
