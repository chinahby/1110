#include "stdafx.h"
#include "DlgProgressEx.h"
#include "Util.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

void CPrgressThread::Go()
{
	DoWork();
	m_pUI->CloseDlg();
}

CDlgProgressEx::CDlgProgressEx(CPrgressThread& thread, 
						   CWnd* pParent /*=NULL*/,
						   bool canCancel /*=false*/)
	: CDialog(CDlgProgressEx::IDD, pParent), 
	m_Thread(thread),
	m_bCanCancel(canCancel)
{
	//{{AFX_DATA_INIT(CDlgProgress)
	//}}AFX_DATA_INIT
	m_whiteBk.CreateSolidBrush(RGB(255,255,255));
	m_Thread.SetUI(this);
}


void CDlgProgressEx::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	//{{AFX_DATA_MAP(CDlgProgress)
	DDX_Control(pDX, IDC_PROGRESS_BAR, m_ctrlProgress);

	//}}AFX_DATA_MAP
}


BEGIN_MESSAGE_MAP(CDlgProgressEx, CDialog)
	//{{AFX_MSG_MAP(CDlgProgressEx)
		// NOTE: the ClassWizard will add message map macros here
		ON_WM_PAINT()
		ON_WM_CTLCOLOR()
	//}}AFX_MSG_MAP
	
END_MESSAGE_MAP()


void CDlgProgressEx::OnPaint() 
{
	CPaintDC dc(this); // device context for painting
	
	// TODO: Add your message handler code here
	CBitmap m_BkBmp ;
	//m_BkBmp.Attach(LoadBmpFormFile("press.bmp"))  ;
	m_BkBmp.LoadBitmap(IDB_BITMAP_PRESS);
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
		ReleaseDC(&memDC) ;
	}
	
	// Do not call CDialog::OnPaint() for painting messages
}

HBRUSH CDlgProgressEx::OnCtlColor(CDC* pDC, CWnd* pWnd, UINT nCtlColor) 
{
	HBRUSH hbr = CDialog::OnCtlColor(pDC, pWnd, nCtlColor);
	
	// TODO: Change any attributes of the DC here
	pDC->SetBkMode(TRANSPARENT);
	pDC->SetBkColor(RGB(255,255,255)) ;
	return m_whiteBk ;
	// TODO: Return a different brush if the default is not desired
	return hbr;
}
/////////////////////////////////////////////////////////////////////////////
// CDlgProgress message handlers
void CDlgProgressEx::SetProgress(int nProgress, int max, const CString& strInfo /* = "" */)
{
	m_ctrlProgress.SetRange(0,max);
	m_ctrlProgress.SetPos(nProgress);

	if(strInfo.GetLength())
	{
		SetDlgItemText(IDC_PROGRESS_INFO, strInfo);
		SetDlgItemText(IDC_PROGRESS, strInfo);
	}
}

BOOL CDlgProgressEx::OnInitDialog()
{
	CDialog::OnInitDialog();
	
	CRect rcWnd;
	GetWindowRect(rcWnd);
	CRect rc;
	rc.left = rcWnd.left+20;
	rc.right = rcWnd.right-80;
	rc.bottom = rcWnd.top+rcWnd.Height()/4+105;
	rc.top = rcWnd.top+120;
	MoveWindow(&rc);
	CenterWindow();
	
	if(m_bCanCancel)//如果能够cancel，则加入一个Cancel button
	{
		enum{WIDTH =35, HEIGHT=20};
		CRect rc;
		GetDlgItem(IDC_PROGRESS_BAR)->GetClientRect(rc);
		rc.right = rc.right-30;
		rc.bottom = rc.bottom+25;
		rc.top = rc.top+23;
		rc.left = rc.left+10;
		GetDlgItem(IDC_PROGRESS_BAR)->MoveWindow(&rc);

	
		//加入Cancel button
	
		CRect rcBtn(rc.right+10,rc.top-2,rc.right+50,rc.bottom+2);


		m_tbnCancel.Create(LoadStringEx(IDS_CANCEL),
						   BS_DEFPUSHBUTTON | BS_OWNERDRAW | WS_VISIBLE,
						   rcBtn,
						   this,
						   IDCANCEL);

		m_tbnCancel.SetFont(GetFont());
		
	}

	CenterWindow();	
	//启动线程
	m_Thread.Start();
	return TRUE;
}


void CDlgProgressEx::OnCancel()
{
	if(m_bCanCancel)
	{
		m_Thread.Cancel();
	}

	if(m_Thread.WaitForComplete(100) != WAIT_TIMEOUT)//等待线程结束
		CDialog::OnCancel();
}

void CDlgProgressEx::OnOK()
{
	if(m_bCanCancel)
	{
		m_Thread.Cancel();
	}

	if(m_Thread.WaitForComplete(100) != WAIT_TIMEOUT)//等待线程结束
		CDialog::OnCancel();
}

void CDlgProgressEx::CloseDlg()
{
	PostMessage(WM_CLOSE,0,0);
}

