// ColorBtn.cpp : implementation file
//

#include "stdafx.h"
#include "GlobalDataCenter.h"
#include "ColorBtn.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

/////////////////////////////////////////////////////////////////////////////
// CColorBtn

static COLORREF crWihte  = RGB(0xFF,0xFF,0xFF) ;
static COLORREF crBlack  = RGB(255,255,255);
static COLORREF crColor  = RGB(0,0,0);
static HCURSOR  hHand    = NULL ;
CColorBtn::CColorBtn()
{
	m_bHold = false ;
	m_pToolTip = NULL ;
	m_bHitOver = false ;
	m_bOvering = false ;
	m_bIsBitMapBtn = false ;
	m_bShowDown    = true  ;
	m_bIsInRect    = false ;
	m_bMouseDown   = false ; 
	m_bDrawSelfText= false ;
	m_bDbClick     = false ;
	m_bSetRgn	   = false ;
	m_bCurrBtn = false;
	m_bMove = false;
	if(hHand == NULL)
	{
		hHand = ::LoadCursor(NULL,IDC_HAND);
	}
	m_clrText = crColor ;
	m_pszNorBmpPath = NULL ;
	m_pszDownBmpPath= NULL ;
	m_pszMoveBmpPath = NULL;
	m_pszGrayBmpPath = NULL;
	SetBkColor(RGB(163,220,235),RGB(50,130,186));
}

CColorBtn::~CColorBtn()
{
	if(m_pToolTip != NULL)
	{
		delete m_pToolTip;
		m_pToolTip = NULL ;
	}

	ReleaseBmpPathInfo();
}


BEGIN_MESSAGE_MAP(CColorBtn, CButton)
	//{{AFX_MSG_MAP(CColorBtn)
	ON_WM_LBUTTONDOWN()
	ON_WM_LBUTTONUP()
	ON_WM_MOUSEMOVE()
	ON_WM_SETCURSOR()
	ON_WM_LBUTTONDBLCLK()
	ON_WM_PAINT()
	ON_WM_ERASEBKGND()
	ON_WM_DELETEITEM()
	ON_WM_DRAWITEM()
	//}}AFX_MSG_MAP
	ON_MESSAGE(WM_MOUSELEAVE, OnMouseLeave)
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CColorBtn message handlers

void CColorBtn::DrawItem(LPDRAWITEMSTRUCT lpBtnStr) 
{
	CDC *pDC = CDC::FromHandle(lpBtnStr->hDC);
	CRect rc(lpBtnStr->rcItem);
	int nSave = pDC->SaveDC() ;
	CString strTxt ;
	GetWindowText(strTxt) ;
	static DWORD dwTextAlg = DT_CENTER|DT_VCENTER|DT_SINGLELINE;
	bool bOldHold = m_bHold ;
	if(m_bIsBitMapBtn)
	{
		CDC	Memdc;
		Memdc.CreateCompatibleDC(pDC) ;
	    CBitmap BkBit ;
		if(!m_bOvering)
		{
		if(m_bHold)
		{
			if(m_pszDownBmpPath != NULL)
			{
				BkBit.Attach(LoadBmpFormFile(m_pszDownBmpPath));
			}
			else
			{
				BkBit.LoadBitmap(m_dwDownBitID); 
			}
			if(m_bShowDown)
				m_bHold = false ;
		}
		else
		{
			if(m_pszNorBmpPath != NULL)
			{
				BkBit.Attach(LoadBmpFormFile(m_pszNorBmpPath));
			}
			else
			{
				BkBit.LoadBitmap(m_dwNorBitID);
			}
		}
		}
		else
		{
			m_bOvering = false;
			BkBit.Attach(LoadBmpFormFile(m_pszMoveBmpPath));
		}
		
		if(BkBit.GetSafeHandle())
		{
			Memdc.SelectObject(&BkBit);
			if(m_bDrawSelfText)
			{
				CRect   rcTmp(rc) ;
				Memdc.SetBkMode(TRANSPARENT) ;
				Memdc.SelectObject(GetParent()->GetFont()) ;
				if(lpBtnStr->itemState == ODS_DISABLED)
				{
					rcTmp.left+=1;
					rcTmp.top +=1;
					Memdc.SetTextColor(RGB(255,255,255)) ;
					Memdc.DrawText(strTxt,&rcTmp,dwTextAlg );
					Memdc.SetTextColor(RGB(128,128,128)) ;
					Memdc.DrawText(strTxt,&rc,dwTextAlg);
				}
				else
				{
					if(m_bMouseDown)
					{
						rcTmp.InflateRect(0,0,2,2) ;
					
					}
					Memdc.DrawText(strTxt,&rcTmp,dwTextAlg) ;
				}
			}
			BITMAP bm ;
			if(BkBit.GetBitmap(&bm))
			{
				if(bm.bmWidth>0 && bm.bmHeight>0) 
					SetWindowPos(NULL, 0, 0, bm.bmWidth,bm.bmHeight , SWP_NOZORDER|SWP_NOMOVE) ;
			}
			pDC->BitBlt(0,0,rc.Width(),rc.Height(),&Memdc,0,0,SRCCOPY) ;
			HBITMAP OldBit = (HBITMAP)BkBit.Detach() ;
			BkBit.DeleteObject();
			Memdc.DeleteDC();
			ReleaseDC(&Memdc);
			DeleteObject(OldBit) ;
		}
	}
	else
	{
		static	CBrush brFoucs(RGB(255,0,255)) ;
		pDC->SelectObject(GetParent()->GetFont()) ;
		COLORREF clrBk = m_clrBkNor ;
		if(m_bHold)
		{
			clrBk =  m_clrBkOver ;
		}
		pDC->SelectObject(&CBrush(clrBk)) ;
		if(m_bHold)
		{
			pDC->SelectObject(&brFoucs);
		}
		pDC->SetBkMode(TRANSPARENT)    ;
		if(m_clrBkNor == crColor)
		{
			pDC->SetTextColor(crWihte) ;
		}
		else
		{
			pDC->SetTextColor(crColor) ;
		}
		static COLORREF clrFrm = RGB(78,117,148) ;
		static CBrush   brFrm(clrFrm) ;
		static CPoint   pt(6,6)  ;
		CRgn  rgnInter;
		CRgn  rgnOuter;
		rgnOuter.CreateRoundRectRgn(0,0,rc.Width(),rc.Height(),pt.x,pt.y) ;
		rgnInter.CreateRoundRectRgn(1,1,rc.Width()-1,rc.Height()-1,pt.x,pt.y) ;
		pDC->SelectClipRgn(&rgnInter,COMPLEXREGION );
		pDC->FrameRgn(&rgnOuter,&CBrush(clrFrm),rc.Width()+1,rc.Height()+1) ;
		pDC->SelectClipRgn(NULL) ;
		pDC->SelectClipRgn( &rgnInter );

		COLORREF top	= m_clrBkNor ;
		COLORREF bottom	= m_clrBkOver;

		COLOR16 r = (COLOR16) ((top & 0x000000FF)<<8);
		COLOR16 g = (COLOR16) ( top & 0x0000FF00);
		COLOR16 b = (COLOR16) ((top & 0x00FF0000)>>8);

		TRIVERTEX     vert[2] ;
		GRADIENT_RECT gRect   ;
		vert[0].x     = 1;
		vert[0].y     = 1;
		vert[0].Red   = r;
		vert[0].Green = g;
		vert[0].Blue  = b;
		vert[0].Alpha = 0x0000;

		r = (COLOR16) ((bottom & 0x000000FF)<<8);
		g = (COLOR16) (bottom  & 0x0000FF00)	;
		b = (COLOR16) ((bottom & 0x00FF0000)>>8);

		vert[1].x     = rc.Width() -1;
		vert[1].y     = rc.Height()-1;
		vert[1].Red   = r;
		vert[1].Green = g;
		vert[1].Blue  = b;
		vert[1].Alpha = 0x0000;

		gRect.UpperLeft  = 0;
		gRect.LowerRight = 1;
		GradientFill(pDC->m_hDC, vert, 2, &gRect,1,GRADIENT_FILL_RECT_V);

		pDC->SelectClipRgn(NULL) ;
		pDC->SetBkMode(TRANSPARENT) ;
		if(lpBtnStr->itemState == ODS_DISABLED)
		{
			CRect rcTmp(rc) ;
			rcTmp.left+=2;
			rcTmp.top +=2;
			pDC->SetTextColor(RGB(255,255,255)) ;
			pDC->DrawText(strTxt,&rcTmp,dwTextAlg );
			pDC->SetTextColor(RGB(128,128,128)) ;
			pDC->DrawText(strTxt,&rc,dwTextAlg );
		}
		else
		{
			CFont btnFt ;
			CFont *pFont = GetParent()->GetFont() ;
			if(m_bOvering)
			{
				LOGFONT lf ;
				pFont->GetLogFont(&lf) ;
				lf.lfUnderline = false  ;
				btnFt.CreateFontIndirect(&lf)   ;
				pDC->SetTextColor(RGB(250,250,250)) ;
				pDC->SelectObject(&btnFt) ;
			}
			if(m_bMouseDown)
			{
				rc.left+=2 ;
				rc.top +=2 ;
			}  
			pDC->DrawText(strTxt,&rc,dwTextAlg) ;
			brFoucs.DeleteObject() ;
			btnFt.DeleteObject()   ;
		}	
		SetRoundRect() ;
	}

	if(m_bIsInRect && !m_bHold )
	{
//		pDC->Draw3dRect(&rc,m_bMouseDown?crBlack:crWihte,m_bMouseDown?crWihte:crBlack) ;
	}
	if(ODS_FOCUS == lpBtnStr->itemState && !m_bHold && !m_bMouseDown)
	{
	/*	rc.InflateRect(-2,-2,-2,-3);
		if(!m_bIsBitMapBtn)
		{
			rc.InflateRect(-2,0,-3,0);
		}
		pDC->DrawFocusRect(&rc) ;
		*/
	}
	
	m_bHold = bOldHold    ;
	pDC->RestoreDC(nSave) ;
//	pDC->DeleteDC();
	ReleaseDC(pDC);				//070807he
}

void CColorBtn::SetBitmapID(int nNorID,int nDownID)
{
	m_bIsBitMapBtn = true ;
	m_dwDownBitID = nDownID ;
	m_dwNorBitID  = nNorID;
	if(nNorID == -1)
	{
		m_dwNorBitID  = nDownID;
	}
	if(nNorID == -1 && nDownID == -1)
		m_bIsBitMapBtn = false ;
}

void CColorBtn::SetBitmapPath(LPCTSTR lpNorPath,LPCTSTR lpDownPath,LPCTSTR lpMovePath)
{
	m_bIsBitMapBtn = false ;
	ReleaseBmpPathInfo();
	if(lpNorPath != NULL)
	{
		if(m_pszNorBmpPath == NULL)
			m_pszNorBmpPath = (TCHAR *)calloc(MAX_PATH+20,sizeof(TCHAR));
		_tcscpy(m_pszNorBmpPath,lpNorPath) ;
		m_bIsBitMapBtn = true ;
	}
	if(lpDownPath != NULL)
	{
		if(m_pszDownBmpPath == NULL)
			m_pszDownBmpPath= (TCHAR *)calloc(MAX_PATH+20,sizeof(TCHAR));
		_tcscpy(m_pszDownBmpPath,lpDownPath) ;
		m_bIsBitMapBtn = true ;
	}
	if(lpMovePath != NULL)
	{
		if(m_pszMoveBmpPath == NULL)
			m_pszMoveBmpPath= (TCHAR *)calloc(MAX_PATH+20,sizeof(TCHAR));
		_tcscpy(m_pszMoveBmpPath,lpMovePath) ;
		m_bIsBitMapBtn = true ;
	}
	
}
 
void CColorBtn::SetBkColor(COLORREF clrBkNor,COLORREF clrBkOver)
{
	m_clrBkNor  = clrBkNor  ;
	m_clrBkOver = clrBkOver ;
}

LRESULT CColorBtn::OnMouseLeave(WPARAM wparam, LPARAM lparam)
{
	if(m_bHitOver && m_bOvering )
	{
		DWORD dwTmp  = m_dwNorBitID;
		m_dwNorBitID = m_dwDownBitID;	
		m_dwDownBitID = dwTmp;
		
		
		COLORREF clrTmp = m_clrBkNor;
		m_clrBkNor  = m_clrBkOver;	
		m_clrBkOver = clrTmp;
		

		m_bOvering = false ;
	}
	m_bIsInRect = false ;
	CRect rc;
	GetClientRect(&rc);
	CRgn rgn;
    rgn.CreateRoundRectRgn(rc.left, rc.top, rc.right, rc.bottom,10,10);
	RedrawWindow(NULL, &rgn);
	Draw() ;
	return 0;
}

void CColorBtn::OnLButtonDown(UINT nFlags, CPoint point) 
{
	m_bMouseDown = true;
	if(m_bShowDown)
	{
		SetHold() ;
	}
	
	SetCapture() ;
	CButton::OnLButtonDown(nFlags, point);
}

void CColorBtn::OnLButtonUp(UINT nFlags, CPoint point) 
{
	if((!m_bDbClick && !m_bDraging) || m_bHitOver || m_bShowDown)
	{
		SetHold(false) ;
	}
	m_bMouseDown = false ;
	m_bIsInRect  = false ;
	m_bDraging   = false ;
	m_bDbClick   = false ;
	CButton::OnLButtonUp(nFlags, point);
	ReleaseCapture() ;
}

void CColorBtn::Draw()
{
	RedrawWindow();
}

void CColorBtn::PreSubclassWindow() 
{
	DWORD dwStyle = GetStyle() ;
	dwStyle|= BS_OWNERDRAW ;
	ModifyStyle(0,dwStyle) ;
	CButton::PreSubclassWindow();
}

void CColorBtn::SetShowDown(bool bShow)
{
	m_bShowDown = bShow ;
}

BOOL CColorBtn::PreTranslateMessage(MSG* pMsg) 
{
	if (m_pToolTip != NULL)
	{
		if (pMsg->message == WM_MOUSEMOVE)
		{
			m_pToolTip->RelayEvent(pMsg);
		}
	}	
	return CButton::PreTranslateMessage(pMsg);
}

void CColorBtn::SetToolTipTxt(LPCTSTR lpTipTxt)
{
	if(m_pToolTip == NULL)
	{
		m_pToolTip = new CToolTipCtrl() ;
		m_pToolTip->Create(this,TTS_ALWAYSTIP) ;
		m_pToolTip->AddTool(this,lpTipTxt) ;
		m_pToolTip->SetDelayTime(TTDT_INITIAL,5) ;
	}
	else
	{
		if(lpTipTxt == NULL || _tcslen(lpTipTxt) == 0)
		{
			m_pToolTip->Activate(false) ;
			return ;
		}
		else
		{
			m_pToolTip->UpdateTipText(lpTipTxt,this);
		}
	}
	m_pToolTip->Activate(TRUE) ;
}

void CColorBtn::OnMouseMove(UINT nFlags, CPoint point) 
{

	if(m_bHitOver && !m_bOvering )
	{
		DWORD dwTmp   = m_dwNorBitID;
		m_dwNorBitID  = m_dwDownBitID;	
		m_dwDownBitID = dwTmp;

		
		COLORREF clrTmp   = m_clrBkNor;
		m_clrBkNor  = m_clrBkOver;	
		m_clrBkOver = clrTmp;

	/*	if(m_pszNorBmpPath != NULL)
		{
			char  szTmpPath[MAX_PATH*2]={0}  ;
			strcpy(szTmpPath,m_pszDownBmpPath) ;
			strcpy(m_pszDownBmpPath,m_pszNorBmpPath) ;
			strcpy(m_pszNorBmpPath,szTmpPath) ;
		}
	*/	m_bOvering = true ;		
		Draw() ;
	}

	if(!m_bIsInRect && !m_bHitOver && m_pToolTip == NULL)
	{
		m_bIsInRect = true ;
		CRect rc;
		GetClientRect(&rc);
		CRgn rgn;
	//	rgn.CreateRoundRectRgn(rc.left, rc.top, rc.right, rc.bottom,10,10);
	//	RedrawWindow(NULL, &rgn);
	//	Draw() ;
	}

	TRACKMOUSEEVENT tme;
	tme.cbSize  = sizeof(TRACKMOUSEEVENT);
	tme.dwFlags = /*TME_HOVER |*/ TME_LEAVE;
	tme.dwHoverTime = HOVER_DEFAULT;
	tme.hwndTrack   = m_hWnd;
	::_TrackMouseEvent( &tme );	
	m_bDraging = m_bMouseDown ;
	CButton::OnMouseMove(nFlags, point);
}

void CColorBtn::SetHitOver(bool bHitOver)
{
	m_bHitOver = bHitOver ;
}

BOOL CColorBtn::OnSetCursor(CWnd* pWnd, UINT nHitTest, UINT message) 
{
	if(!m_bHold)
	{
		::SetCursor(hHand) ;
		return TRUE ;
	}
	return CButton::OnSetCursor(pWnd, nHitTest, message);
}

void CColorBtn::ReleaseBmpPathInfo()
{
	if(m_pszNorBmpPath != NULL)
	{
		free(m_pszNorBmpPath);
		m_pszNorBmpPath = NULL;
	}
	if( m_pszDownBmpPath!= NULL)
	{
		free(m_pszDownBmpPath);
		m_pszDownBmpPath = NULL;
	}
	if( m_pszMoveBmpPath!= NULL)
	{
		free(m_pszMoveBmpPath);
		m_pszMoveBmpPath = NULL;
	}
}

void CColorBtn::SetDrawTextFlag(bool bDrawFlag/*=true*/)
{
	m_bDrawSelfText = bDrawFlag ;
}

void CColorBtn::SetRoundRect()
{
	if(m_bSetRgn)
		return ;
	CRect rc ;
	GetClientRect(rc) ;
	HRGN rgn ;
	SetWindowRgn(NULL,false) ;
	rgn = CreateRoundRectRgn(0,0,rc.Width(),rc.Height(),15,15) ;
	SetWindowRgn((HRGN)rgn,FALSE) ;
	m_bSetRgn = true ;
	Draw() ;
}

void CColorBtn::OnLButtonDblClk(UINT nFlags, CPoint point) 
{
	m_bDbClick = true ;
	CButton::OnLButtonDblClk(nFlags, point);
}

void CColorBtn::OnPaint() 
{
	Default() ;
	//CPaintDC dc(this); // device context for painting
	
	// TODO: Add your message handler code here
	
	// Do not call CButton::OnPaint() for painting messages
}

BOOL CColorBtn::OnEraseBkgnd(CDC* pDC) 
{

	return false ;
	return CButton::OnEraseBkgnd(pDC);
}

void CColorBtn::OnDeleteItem(int nIDCtl, LPDELETEITEMSTRUCT lpDeleteItemStruct) 
{
	// TODO: Add your message handler code here and/or call default
	
	CButton::OnDeleteItem(nIDCtl, lpDeleteItemStruct);
}

void CColorBtn::OnDrawItem(int nIDCtl, LPDRAWITEMSTRUCT lpDrawItemStruct) 
{
	// TODO: Add your message handler code here and/or call default
	
	CButton::OnDrawItem(nIDCtl, lpDrawItemStruct);
}

void CColorBtn::IsCurrentBtn(bool bCurrBtn)
{
	m_bCurrBtn = bCurrBtn;
}