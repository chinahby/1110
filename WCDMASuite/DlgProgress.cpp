// DlgProgress.cpp : implementation file
//

#include "stdafx.h"
#include "sunnycat.h"
#include "DlgProgress.h"
#include "GlobalDataCenter.h"


#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

/////////////////////////////////////////////////////////////////////////////
// CDlgProgress dialog


CDlgProgress::CDlgProgress(CWnd* pParent /*=NULL*/)
	: CDialog(CDlgProgress::IDD, pParent)
{
	//{{AFX_DATA_INIT(CDlgProgress)
	m_strInfo = _T("");
	//}}AFX_DATA_INIT
	m_whiteBk.CreateSolidBrush(RGB(255,255,255));
}


void CDlgProgress::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	//{{AFX_DATA_MAP(CDlgProgress)
	DDX_Control(pDX, IDC_PROGRESS, m_ctrlProgress2);
	DDX_Control(pDX, IDC_PROGRESS_BAR, m_ctrlProgress);
	DDX_Text(pDX, IDC_PROGRESS_INFO, m_strInfo);
	//}}AFX_DATA_MAP
}


BEGIN_MESSAGE_MAP(CDlgProgress, CDialog)
	//{{AFX_MSG_MAP(CDlgProgress)
	ON_WM_PAINT()
	ON_WM_CTLCOLOR()
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CDlgProgress message handlers
void CDlgProgress::SetProgress(int nProgress, const CString& strInfo /* = "" */)
{
	m_ctrlProgress.SetPos(nProgress);

	if(strInfo.GetLength())
	{
		m_strInfo = strInfo;
		
	}

	UpdateData(FALSE);
}

CDlgProgress * g_pDlgProgress = NULL;

void InitProgress(int nMax)
{
	CWnd * pWnd = AfxGetMainWnd();

	if(pWnd)
	{
		pWnd->UpdateWindow();
	}

	if(NULL == g_pDlgProgress)
	{
		g_pDlgProgress = new CDlgProgress;
		g_pDlgProgress->Create(IDD_PROGRESS, NULL);

		if(pWnd)
		{
			g_pDlgProgress->CenterWindow(pWnd);
		}
	}

    if(nMax != 0)
    {
        g_pDlgProgress->SetMaxProgress(nMax);
    }

	g_pDlgProgress->ShowWindow(SW_NORMAL);
	g_pDlgProgress->UpdateWindow();
}

void ShowProgress(int nProgress, CString strInfo /* = "" */)
{
	if(NULL == g_pDlgProgress)
	{
		InitProgress();
	}

	g_pDlgProgress->SetProgress(nProgress, strInfo);
}

void EndProgress()
{
	if(g_pDlgProgress)
	{
		delete g_pDlgProgress;
		g_pDlgProgress = NULL;
	}
}

void CDlgProgress::SetMaxProgress(int nMax)
{
    m_ctrlProgress.SetRange(0,nMax);
}

void CDlgProgress::OnPaint() 
{
	CPaintDC dc(this); // device context for painting
	
	// TODO: Add your message handler code here
	CBitmap m_BkBmp ;


	m_BkBmp.Attach(LoadBmpFormFile(TEXT("skin\\other\\login.bmp")));

	if(m_BkBmp.GetSafeHandle())
	{
		CRect rc ;
		GetWindowRect(rc) ;
		CDC memDC;
		memDC.CreateCompatibleDC( &dc );//
		memDC.SelectObject( &m_BkBmp );
		dc.StretchBlt(0,0,rc.Width(),rc.Height(),&memDC,0,0,rc.Width(),rc.Height(),SRCCOPY);
		m_BkBmp.DeleteObject() ;
		memDC.DeleteDC() ;
	
	}
}

HBRUSH CDlgProgress::OnCtlColor(CDC* pDC, CWnd* pWnd, UINT nCtlColor) 
{
	HBRUSH hbr = CDialog::OnCtlColor(pDC, pWnd, nCtlColor);
	
	// TODO: Change any attributes of the DC here
	pDC->SetBkMode(TRANSPARENT);
	pDC->SetBkColor(RGB(255,255,255)) ;
	return m_whiteBk ;
	// TODO: Return a different brush if the default is not desired
	return hbr;
}

BOOL CDlgProgress::OnCommand(WPARAM wParam, LPARAM lParam) 
{
	if(wParam == IDCANCEL)
		return true ;
	return CDialog::OnCommand(wParam, lParam);
}

BOOL CDlgProgress::PreTranslateMessage(MSG* pMsg) 
{
	if(pMsg->message == WM_KEYDOWN)
	{
		if(pMsg->wParam == 0x0D || pMsg->wParam == VK_SPACE || pMsg->wParam == VK_ESCAPE)
			return true ;
	}	
	return CDialog::PreTranslateMessage(pMsg);
}

BOOL CDlgProgress::OnInitDialog() 
{
	CDialog::OnInitDialog();
	CenterWindow();
	// TODO: Add extra initialization here
	m_ctrlProgress.ShowWindow(false);
	m_ctrlProgress2.ShowWindow(false);
	CRect rc;
	GetWindowRect(rc) ; 
	CRgn rgnWnd ;
	rgnWnd.CreateRoundRectRgn(0,0,rc.Width()+1,rc.Height()+1,19,19) ;
	SetWindowRgn((HRGN)rgnWnd,TRUE) ;
	return TRUE;  // return TRUE unless you set the focus to a control
	              // EXCEPTION: OCX Property Pages should return FALSE
}
