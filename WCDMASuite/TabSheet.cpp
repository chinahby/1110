// TabSheet.cpp : implementation file
//

#include "stdafx.h"
#include "SunnyCat.h"
#include "TabSheet.h"
#include "ColorBtn.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

/////////////////////////////////////////////////////////////////////////////
// CTabSheet

CTabSheet::CTabSheet()
{
	m_nNumOfPages = 0;
	m_nCurrentPage = 0;
	ppDC = NULL;
	m_whiteBk.CreateSolidBrush(RGB(255,255,255));
}

CTabSheet::~CTabSheet()
{
}


BEGIN_MESSAGE_MAP(CTabSheet, CTabCtrl)
	//{{AFX_MSG_MAP(CTabSheet)
	ON_WM_LBUTTONDOWN()
	ON_WM_ERASEBKGND()
	ON_WM_CTLCOLOR_REFLECT()
	ON_WM_NCPAINT()
	ON_WM_SIZECLIPBOARD()
	ON_WM_PAINTCLIPBOARD()
	ON_WM_CREATE()

	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CTabSheet message handlers

BOOL CTabSheet::AddPage(LPCTSTR title, CDialog *pDialog,UINT ID)
{
	if( MAXPAGE == m_nNumOfPages )
		return FALSE;

	m_nNumOfPages++;

	m_pPages[m_nNumOfPages-1] = pDialog;
	m_IDD[m_nNumOfPages-1] = ID;
	m_Title[m_nNumOfPages-1] = title;

	return TRUE;
}

void CTabSheet::SetRect()
{
	CRect tabRect, itemRect;
	int nX, nY, nXc, nYc;

	GetClientRect(&tabRect);
	GetItemRect(0, &itemRect);
//	CColorBtn colorBtn;
//	colorBtn.Create("", BS_OWNERDRAW, itemRect, this, );
	nX=itemRect.left;
	nY=itemRect.bottom+1;
	nXc=tabRect.right-itemRect.left-2;
	nYc=tabRect.bottom-nY-2;

	m_pPages[0]->SetWindowPos(&wndTop, nX, nY, nXc, nYc, SWP_SHOWWINDOW);
	for( int nCount=1; nCount < m_nNumOfPages; nCount++ )
		m_pPages[nCount]->SetWindowPos(&wndTop, nX, nY, nXc, nYc, SWP_HIDEWINDOW);

}

void CTabSheet::Show()
{
	for( int i=0; i < m_nNumOfPages; i++ )
	{
		m_pPages[i]->Create( m_IDD[i], this );
		InsertItem( i, m_Title[i], i );
	}

	m_pPages[0]->ShowWindow(SW_SHOW);
	for( i=1; i < m_nNumOfPages; i++)
		m_pPages[i]->ShowWindow(SW_HIDE);

	SetRect();

}

void CTabSheet::OnLButtonDown(UINT nFlags, CPoint point) 
{
	CTabCtrl::OnLButtonDown(nFlags, point);

	if(m_nCurrentPage != GetCurFocus())
	{
		m_pPages[m_nCurrentPage]->ShowWindow(SW_HIDE);
		m_nCurrentPage=GetCurFocus();
		m_pPages[m_nCurrentPage]->ShowWindow(SW_SHOW);
//		m_pPages[m_nCurrentPage]->SetFocus();
	}
}

int CTabSheet::SetCurSel(int nItem)
{
	if( nItem < 0 || nItem >= m_nNumOfPages)
		return -1;

	int ret = m_nCurrentPage;

	if(m_nCurrentPage != nItem )
	{
		m_pPages[m_nCurrentPage]->ShowWindow(SW_HIDE);
		m_nCurrentPage = nItem;
		m_pPages[m_nCurrentPage]->ShowWindow(SW_SHOW);
//		m_pPages[m_nCurrentPage]->SetFocus();
		CTabCtrl::SetCurSel(nItem);
	}

	return ret;
}

int CTabSheet::GetCurSel()
{
	return CTabCtrl::GetCurSel();
}

BOOL CTabSheet::OnEraseBkgnd(CDC *pDC)
{
	CRect rect;
	GetClientRect(rect);
	CBrush brush(GetSysColor(COLOR_BACKGROUND));
	pDC->FillSolidRect(rect,RGB(228,238,242));
	ppDC = pDC;
	brush.DeleteObject();
	return TRUE;
}

BOOL CTabSheet::SetBkColor(COLORREF colorRef)
{
	CRect rect;
	GetWindowRect(rect);
	ppDC->FillSolidRect(rect, colorRef);
	return TRUE;
}

HBRUSH CTabSheet::CtlColor(CDC* pDC, UINT nCtlColor) 
{
	// TODO: Change any attributes of the DC here
	pDC->SetBkMode(TRANSPARENT);
	pDC->SetBkColor(RGB(255,255,255)) ;
	return m_whiteBk;
	// TODO: Return a non-NULL brush if the parent's handler should not be called
	return NULL;
}

void CTabSheet::OnNcPaint() 
{
	// TODO: Add your message handler code here

	// Do not call CTabCtrl::OnNcPaint() for painting messages
}

void CTabSheet::OnSizeClipboard(CWnd* pClipAppWnd, HGLOBAL hRect) 
{
	CTabCtrl::OnSizeClipboard(pClipAppWnd, hRect);
	
	// TODO: Add your message handler code here
	
}

void CTabSheet::OnPaintClipboard(CWnd* pClipAppWnd, HGLOBAL hPaintStruct) 
{
	// TODO: Add your message handler code here and/or call default
	
		CTabCtrl::OnPaintClipboard(pClipAppWnd, hPaintStruct);
}

int CTabSheet::OnCreate(LPCREATESTRUCT lpCreateStruct) 
{
	if (CTabCtrl::OnCreate(lpCreateStruct) == -1)
		return -1;
	
	// TODO: Add your specialized creation code here
	
	return 0;
}

