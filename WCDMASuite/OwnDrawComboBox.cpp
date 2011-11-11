// OwnDrawComboBox.cpp : implementation file
//

#include "stdafx.h"
#include "OwnDrawComboBox.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

/////////////////////////////////////////////////////////////////////////////
// COwnDrawComboBox

static DWORD g_dwDefNormalBtID =-1;
static DWORD g_dwDefHoverBtID  =-1;
static DWORD g_dwDefDownBtID   =-1;

COwnDrawComboBox::COwnDrawComboBox()
{
	m_bMouseIn	= false;
	m_bMouseDown= false;
	m_dwNormalBtID=-1;
	m_dwHoverBtID =-1;
	m_dwDownBtID  =-1;

}

COwnDrawComboBox::~COwnDrawComboBox()
{
	
}


BEGIN_MESSAGE_MAP(COwnDrawComboBox, CComboBox)
	//{{AFX_MSG_MAP(COwnDrawComboBox)
	ON_WM_PAINT()
	ON_CONTROL_REFLECT(CBN_CLOSEUP, OnCloseup)
	ON_CONTROL_REFLECT(CBN_DROPDOWN, OnDropdown)
	ON_WM_TIMER()
	ON_WM_MOUSEMOVE()
	ON_WM_LBUTTONDOWN()
	ON_CONTROL_REFLECT(CBN_KILLFOCUS, OnKillfocus)
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// COwnDrawComboBox message handlers
void COwnDrawComboBox::SetCmbThumbDefBtIDs(DWORD dwNormal, DWORD dwHover, DWORD dwDown)
{
	g_dwDefNormalBtID = dwNormal;
	g_dwDefHoverBtID  = dwHover;
	g_dwDefDownBtID   = dwDown;
}

void COwnDrawComboBox::SetCmbThumbBtIDs(DWORD dwNormal, DWORD dwHover, DWORD dwDown)
{
	m_dwNormalBtID = dwNormal;
	m_dwHoverBtID  = dwHover;
	m_dwDownBtID   = dwDown;
}

void COwnDrawComboBox::OnPaint() 
{
	CPaintDC dc(this) ;
	CDC *pDC = &dc;   //GetDC() ;
	if(pDC->GetSafeHdc() == NULL)
		return ;
	CRect rc ;
	GetClientRect(rc) ;
	if(!m_bMouseIn)
	{
		pDC->FillSolidRect(rc,RGB(250,250,250));
	}
	CRect rcFc=rc;
	rcFc.InflateRect(1,0,1,0) ;
	pDC->Draw3dRect(rcFc,RGB(210,210,210),RGB(210,210,210)) ;
	rcFc=rc;
	CString strtxt ;
	GetWindowText(strtxt)   ;
	pDC->SetBkMode(TRANSPARENT);
	pDC->SelectObject(GetParent()->GetFont());
	rc.left+=4 ;
	if(GetFocus() == this )
	{
		rcFc.right-=16 ;
		rcFc.InflateRect(-2,-3,-2,-3) ;
		if(!m_bIsDorp)
		{
			pDC->SetTextColor(RGB(255,255,255))    ;
			pDC->FillSolidRect(rcFc,RGB(10,36,106));
			pDC->DrawFocusRect(rcFc) ;
			rcFc.left+=1 ;
		}
		rcFc.left+=2 ;
		pDC->DrawText(strtxt,&rcFc,DT_VCENTER|DT_SINGLELINE);
	}
	else
	{
		pDC->DrawText(strtxt,&rc,DT_VCENTER|DT_SINGLELINE);
	}

	int a = 3 ;
	rc.left = rc.right - 20 ;
	rc.InflateRect(-a,-a,-a,-a)  ;
	DWORD dwBitmapID = m_dwNormalBtID ;
	if(dwBitmapID == -1)
	{
		dwBitmapID = g_dwDefNormalBtID ;
	}
	COLORREF clrBtn = RGB(255,0,0) ;
	if(m_bMouseIn)
	{
		clrBtn = RGB(0,255,0) ;
		dwBitmapID = m_dwHoverBtID ;
		if(dwBitmapID == -1)
		{
			dwBitmapID = g_dwDefHoverBtID ;
		}
	}
	if(m_bMouseDown)
	{
		clrBtn = RGB(0,0,255) ;
		dwBitmapID = m_dwDownBtID ;
		if(dwBitmapID == -1)
		{
			dwBitmapID = g_dwDefDownBtID ;
		}
	}
	if(dwBitmapID != -1)
	{
		CBitmap bt;
		CDC  memdc ;
		bt.LoadBitmap(dwBitmapID) ;
		memdc.CreateCompatibleDC(pDC) ;
		memdc.SelectObject(&bt) ;
		pDC->BitBlt(rc.left+1,2,15,16,&memdc,0,0,SRCCOPY) ;
		
		HBITMAP OldBit = (HBITMAP) bt.Detach() ;
		DeleteObject(OldBit) ;
		bt.DeleteObject() ;

		GetClientRect(rc) ;
		CPen gridPen(PS_SOLID, 1, RGB(49, 106, 197));
		CPen* oldPen= pDC->SelectObject(&gridPen);
		pDC->MoveTo(0,0);
		pDC->LineTo(rc.right,0);
		pDC->MoveTo(0,0);
		pDC->LineTo(0,rc.bottom);
		pDC->MoveTo(0,rc.bottom);
		pDC->LineTo(rc.right,rc.bottom);
		pDC->MoveTo(rc.right,0);
		pDC->LineTo(rc.right,rc.bottom);
		pDC->SelectObject(oldPen);
		gridPen.DeleteObject();
	}
	else
	{
		pDC->FillSolidRect(rc,clrBtn) ;
	}
	ReleaseDC(pDC);
}

void COwnDrawComboBox::OnMouseMove(UINT nFlags, CPoint point) 
{
	SetTimer(1,10,NULL);
	CComboBox::OnMouseMove(nFlags, point);
}

void COwnDrawComboBox::OnLButtonDown(UINT nFlags, CPoint point) 
{
	m_bMouseDown = true ;
	m_bMouseIn   = false;
	RedrawWindow() ;
	CComboBox::OnLButtonDown(nFlags, point);
}

void COwnDrawComboBox::OnCloseup() 
{
	m_bMouseDown = false ;
	m_bIsDorp    = false ;
	m_bCloseUp   = true  ;
	RedrawWindow() ;
}



void COwnDrawComboBox::OnTimer(UINT nIDEvent) 
{
	POINT pt ;
	GetCursorPos(&pt);
	CRect rcItem ;
	GetWindowRect(&rcItem);

	static bool bPainted = false;
	if(m_bMouseDown==true)
	{
		KillTimer (1);
		if(bPainted == true)
		{
			RedrawWindow() ;
			bPainted = false;
		}
		return;
	}

	if(!rcItem.PtInRect(pt))
	{
		m_bMouseIn = false ;
		KillTimer (1);
		if(bPainted == true)
		{
			RedrawWindow() ;
			bPainted = false;
		}
		return;
	}
	else
	{
		m_bMouseIn   = true ;
		if (bPainted == true)
		{
			return;
		}
		else
		{
			bPainted = true;
			RedrawWindow() ;
		}
	}
	CComboBox::OnTimer(nIDEvent);
}

void COwnDrawComboBox::OnDropdown() 
{
	m_bIsDorp = TRUE;	
}

void COwnDrawComboBox::OnKillfocus() 
{
	m_bMouseDown = false ;
	m_bIsDorp    = false ;
	RedrawWindow() ;	
}
