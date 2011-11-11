// EnhancedHeaderCtrl.cpp : implementation file
//

#include "stdafx.h"
#include "EnhancedHeaderCtrl.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

/////////////////////////////////////////////////////////////////////////////
// CEnhancedHeaderCtrl

CEnhancedHeaderCtrl::CEnhancedHeaderCtrl()
{
	SetResizeAble(false)  ;
	m_bLeftBtnDown = false;
	SetHeadBkColor() ;
}

CEnhancedHeaderCtrl::~CEnhancedHeaderCtrl()
{
}


BEGIN_MESSAGE_MAP(CEnhancedHeaderCtrl, CHeaderCtrl)
	//{{AFX_MSG_MAP(CEnhancedHeaderCtrl)
	ON_WM_PAINT()
	ON_WM_LBUTTONDBLCLK()
	ON_WM_MOUSEMOVE()
	ON_WM_LBUTTONDOWN()
	ON_WM_LBUTTONUP()
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CEnhancedHeaderCtrl message handlers

void CEnhancedHeaderCtrl::OnPaint() 
{
	CPaintDC dc(this); // device context for painting
	CRect ItemRc ;
	GetWindowRect(ItemRc) ;
	ScreenToClient(ItemRc) ;

	int nCount = GetItemCount() ;

	HDITEM HdItem ;
	TCHAR  szColTxt[256];
	HdItem.pszText    = szColTxt;
	HdItem.cchTextMax = 255;
	HdItem.mask = HDI_TEXT ;
	CPen   penL(PS_SOLID ,1,m_clrLineColor) ;
	CPen  *pOldPen = dc.SelectObject(&penL) ;
	ItemRc.bottom+=20 ;
	dc.FillSolidRect(&ItemRc,m_clrHeadColor) ;
	CFont *pOldFont = dc.SelectObject(GetParent()->GetFont()) ;
	dc.SetBkMode(TRANSPARENT) ;
	for(int i=0; i<nCount; i++)
	{
		GetItem(i,&HdItem) ;
		GetItemRect(i,&ItemRc) ;
		dc.DrawText(szColTxt,&ItemRc,DT_VCENTER|DT_CENTER|DT_SINGLELINE) ;
		dc.MoveTo(ItemRc.right,ItemRc.top) ;
		dc.LineTo(ItemRc.right,ItemRc.bottom) ;
	}
	dc.SelectObject(pOldPen) ;
	dc.SelectObject(pOldFont) ;
	penL.DeleteObject() ;
}


//不允许改变Header大小
void CEnhancedHeaderCtrl::OnMouseMove(UINT nFlags, CPoint point) 
{
	if(!m_bResizeAble && m_bLeftBtnDown)
		return ; 
	CHeaderCtrl::OnMouseMove(nFlags, point);
}


void CEnhancedHeaderCtrl::OnLButtonDblClk(UINT nFlags, CPoint point) 
{
	::PostMessage(GetParent()->GetSafeHwnd(),WM_HEADERCLICKMSG,NM_DBLCLK,GetActiveCol(point));
	if(!m_bResizeAble )
		return ;
	CHeaderCtrl::OnLButtonDblClk(nFlags, point);
}

void CEnhancedHeaderCtrl::SetHeadBkColor(COLORREF clrBk,COLORREF clrLine)
{
	m_clrHeadColor = clrBk   ;
	m_clrLineColor = clrLine ;
}

void CEnhancedHeaderCtrl::SetResizeAble(bool bEnable)
{
	m_bResizeAble = bEnable;
}

void CEnhancedHeaderCtrl::OnLButtonDown(UINT nFlags, CPoint point) 
{
	::PostMessage(GetParent()->GetSafeHwnd(),WM_HEADERCLICKMSG,NM_CLICK,GetActiveCol(point));
	m_bLeftBtnDown = true ;
	if(m_bResizeAble)
	{
		SetCapture() ;
		CHeaderCtrl::OnLButtonDown(nFlags, point);
	}
}

void CEnhancedHeaderCtrl::OnLButtonUp(UINT nFlags, CPoint point) 
{
	m_bLeftBtnDown = false ;
	RedrawWindow() ;
	if(m_bResizeAble)
	{
		ReleaseCapture() ;
		CHeaderCtrl::OnLButtonUp(nFlags, point);
	}
}


int CEnhancedHeaderCtrl::GetActiveCol(CPoint &pt)
{
	CRect ItemRc ;
	int nCount = GetItemCount() ;
	for(int i=0; i<nCount; i++)
	{
		GetItemRect(i,&ItemRc) ;
		if(ItemRc.PtInRect(pt))
		{
			return i ;
		}
	}
	return -1 ;
}
