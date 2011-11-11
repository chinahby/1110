// BitmapEdit.cpp : implementation file
//
#include "stdafx.h"
#include "BitmapEdit.h"
#include "resource.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

/////////////////////////////////////////////////////////////////////////////
// CBitmapEdit

CBitmapEdit::CBitmapEdit()
{
	m_nFontSize = 12 ;
	m_bFlag     = false;
	m_crTxtClr     = RGB(0,0,0) ;
	m_crTxtBkClr   = RGB(255,255,255) ;
	m_crSelTxtBkClr= RGB(100,100,100) ;
	m_crSelTxtClr  = RGB(255,255,255) - m_crTxtClr ;
	SetFontSize();
	m_bMouseDown = false ;
	m_nLastSel   = -1 ;

	m_nSelB = 0 ;
	m_nSelE = 0 ;
}

CBitmapEdit::~CBitmapEdit()
{
}


BEGIN_MESSAGE_MAP(CBitmapEdit, CEdit)
	//{{AFX_MSG_MAP(CBitmapEdit)
	ON_WM_PAINT()
	ON_WM_GETDLGCODE()
	ON_WM_MOUSEMOVE()
	ON_WM_LBUTTONDOWN()
	ON_WM_LBUTTONUP()
	ON_CONTROL_REFLECT(EN_SETFOCUS, OnSetfocus)
	ON_CONTROL_REFLECT(EN_KILLFOCUS, OnKillfocus)
	ON_WM_NCLBUTTONDBLCLK()
	ON_WM_LBUTTONDBLCLK()
	ON_WM_ACTIVATE()
	ON_WM_CHAR()
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CBitmapEdit message handlers


void CBitmapEdit::SetFontSize(int nSize)
{
	CFont   cf;
	LOGFONT lf;
	::GetObject((HFONT)GetStockObject(DEFAULT_GUI_FONT),sizeof(m_lf),&m_lf);
	cf.CreatePointFont(nSize * 10, m_lf.lfFaceName);
	cf.GetLogFont(&lf);
	m_lf.lfHeight = lf.lfHeight;
	m_lf.lfWidth  = lf.lfWidth ;
	m_font.Detach() ;
	m_font.CreateFontIndirect(&m_lf);
	m_bFlag       = false ;
	m_nFontSize   = nSize ;
}

void CBitmapEdit::OnPaint() 
{
	CPaintDC dc(this);
	CRect rc;
	CDC *pDC = &dc ;
	GetClientRect(rc);

	if(!m_bFlag)
	{
		m_bFlag = true ;
		SetFont(&m_font) ;
	}

	CBrush bkBr(m_crTxtBkClr);
	pDC->SelectObject(&bkBr) ;
	rc.bottom+=3 ;
	CRect rcTm(rc);
	int nDes    = 3 ;
	rcTm.top   -= nDes ;
	rcTm.left  -= nDes ;
	rcTm.right += nDes ;
	if(m_nFontSize > 20)
		rcTm.bottom-= 3 ;
	else 
		rcTm.bottom+= 3 ;
	pDC->Rectangle(&rcTm) ;
	pDC->Draw3dRect(rcTm,RGB(62,137,190),RGB(62,137,190)) ;

	pDC->SelectObject(&m_font) ;
	pDC->SetBkMode(TRANSPARENT);
	pDC->SetTextColor(m_crTxtClr) ;

	GetSel(m_nSelB,m_nSelE) ;
	CSize Size = pDC->GetOutputTextExtent(TEXT("2"),1) ;
	int   nCharSize = Size.cx; 

	CString strTxt ;
	GetWindowText(strTxt) ;
	int iLimit = GetLimitText();
	if(strTxt.GetLength() > iLimit)
	{
		strTxt = strTxt.Left(iLimit);
		SetWindowText(strTxt);
		GetSel(m_nSelB,m_nSelE);
	}
	CString strSelTxt ;
	if(!strTxt.IsEmpty())
	{
		CPoint pt(rc.left,rc.top) ;
		strSelTxt = strTxt.Mid(CharFromPos(pt)) ;
		pDC->DrawText(strSelTxt,rc,DT_LEFT|DT_NOPREFIX );
	}
	if(m_nSelE != m_nSelB )
	{
		strSelTxt  = strTxt.Mid(m_nSelB, abs(m_nSelE-m_nSelB)) ;
		CPoint pt1 = PosFromChar(m_nSelB) ;
		rcTm.left  = pt1.x  ;
		int offset = pt1.x % nCharSize ;
		rcTm.left -= offset ;
		rcTm.right = rcTm.left + (m_nSelE - m_nSelB)*nCharSize ;
		if(rcTm.right >= rc.right)
			rcTm.right = rc.right - 1;
		if(rcTm.left < rc.left)
			rcTm.left = rc.left ;
		rcTm.top += 1    ;
		rcTm.bottom -= 1 ;
		CBrush br(m_crSelTxtBkClr) ;
		pDC->FillRect(rcTm,&br);
		rcTm.top    += 1 ;
		rcTm.bottom -= 1 ;
		pDC->SetTextColor(m_crSelTxtClr) ;
		pDC->DrawText(strSelTxt,rcTm,DT_LEFT|DT_NOPREFIX );
		SetCaretPos(CPoint(rcTm.right,rc.top)) ;
		HBRUSH OldBr = (HBRUSH)br.Detach() ;
		br.DeleteObject() ;
		DeleteObject(OldBr) ;
	}
	else
	{
		CPoint GetPt;
		CPoint Carept = PosFromChar(m_nSelE) ;
		int nCarePos  = rcTm.left+Carept.x; ;
		if(nCarePos < 0)
		{
			GetPt = GetCaretPos() ; 
			nCarePos = GetPt.x ;
		}
		nCarePos -= Carept.x % nCharSize ;
		if(nCarePos > rc.right)
		{
			nCarePos = rc.right ;
		}
		if(nCarePos < 0)
			nCarePos = 0;
		if(Carept.x !=-1)
			SetCaretPos(CPoint(nCarePos,rc.top)) ;
	}/**/
	HBRUSH OldBr = (HBRUSH)bkBr.Detach() ;
	ReleaseDC(pDC);
	bkBr.DeleteObject() ;
	DeleteObject(OldBr) ;
}

void CBitmapEdit::ReDraw()
{
	RedrawWindow(NULL, NULL,RDW_INVALIDATE | RDW_ERASE ); 
}

void CBitmapEdit::SetTxtColor(COLORREF crTxtBkClr,COLORREF crTxtClr)
{
	m_crTxtClr   = crTxtClr;
	m_crTxtBkClr = crTxtBkClr;
}

void CBitmapEdit::SetSelTxtColor(COLORREF crSelTxtBkClr,COLORREF crSelTxtClr)
{
	m_crSelTxtClr   = crSelTxtClr;
	m_crSelTxtBkClr = crSelTxtBkClr;
}


UINT CBitmapEdit::OnGetDlgCode() 
{
	ReDraw() ;
	return CEdit::OnGetDlgCode();
}

void CBitmapEdit::OnMouseMove(UINT nFlags, CPoint point) 
{
	if(m_bMouseDown)
	{
		int nSelB,nSelE     ;
		GetSel(nSelB,nSelE) ;
		if(nSelB != m_nSelB || nSelE != m_nSelE)
		{
			ReDraw() ;
		}
	}

	CEdit::OnMouseMove(nFlags, point);
}

void CBitmapEdit::OnLButtonDown(UINT nFlags, CPoint point) 
{
	int nSelB,nSelE     ;
	GetSel(nSelB,nSelE) ;
	SetSel(nSelB,nSelE);
	m_bMouseDown = TRUE;
	SetCapture();
	CEdit::OnLButtonDown(nFlags, point);
}

void CBitmapEdit::OnLButtonUp(UINT nFlags, CPoint point) 
{
	if(m_bMouseDown)
	{
		ReDraw();
		ReleaseCapture();
		m_bMouseDown = false;
	}
	CEdit::OnLButtonUp(nFlags, point);
}

LRESULT CBitmapEdit::WindowProc(UINT message, WPARAM wParam, LPARAM lParam) 
{
	if(EM_SETSEL == message || EM_REPLACESEL == message  )
	{
		ReDraw() ;
	}
	return CEdit::WindowProc(message, wParam, lParam);
}

void CBitmapEdit::OnSetfocus() 
{
	ReDraw();	
}

void CBitmapEdit::OnKillfocus() 
{
	ReDraw();
}

void CBitmapEdit::OnNcLButtonDblClk(UINT nHitTest, CPoint point) 
{
	SetSel(0,-1) ;
	ReDraw();
}

void CBitmapEdit::OnLButtonDblClk(UINT nFlags, CPoint point) 
{
	SetSel(0,-1) ;
	ReDraw();
}

void CBitmapEdit::OnActivate(UINT nState, CWnd* pWndOther, BOOL bMinimized) 
{
	CEdit::OnActivate(nState, pWndOther, bMinimized);
	
	ReDraw();
	
}

void CBitmapEdit::OnChar(UINT nChar, UINT nRepCnt, UINT nFlags) 
{
	// TODO: Add your message handler code here and/or call default
	if((nChar >= '0'&& nChar <= '9')||(nChar == '+')||(nChar == '\b')||(nChar == 'p')||(nChar == 'P'))		
	CEdit::OnChar(nChar, nRepCnt, nFlags);
}

