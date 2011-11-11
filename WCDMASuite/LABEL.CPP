// Label.cpp : implementation file
//

#include "stdafx.h"
#include "Label.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

/////////////////////////////////////////////////////////////////////////////
// CLabel

CLabel::CLabel()
{
	m_crText	 = GetSysColor(COLOR_WINDOWTEXT);
	::GetObject((HFONT)GetStockObject(DEFAULT_GUI_FONT),sizeof(m_lf),&m_lf);
	m_font.CreateFontIndirect(&m_lf);
	m_bTimer =			false;
	m_bState =			false;
	m_bTransparent =	true;
	m_bLink   =			true;
	m_hCursor =			::LoadCursor(NULL,IDC_HAND);//hxb
	m_bNotifyParent =	false;
	m_bRotation  =		false;
	m_bSetCircle =      false;
	m_bSetCheck  =      false;
	m_crCheck    =		RGB(0,255,0);
	m_crBkGround =		RGB(110,155,218) ;
	m_crFlashBkGround=RGB(0,0,0);
	m_crFlashText = RGB(255,0,0);
	m_bLAlign  = false ;
	m_bitmapBk = false ;
	m_pParentBKMemDC = NULL ;
	m_pBkbitmap      = NULL ;
	
	m_nParXPos   = 0 ;
	m_nParYPos   = 0 ;
	m_nParWidth  = 0 ;
	m_nParHeight = 0 ;
	m_bSetHand   = false ;
}

CLabel::~CLabel()
{
	//KillTimer(1) ;
}

BEGIN_MESSAGE_MAP(CLabel, CStatic)
	//{{AFX_MSG_MAP(CLabel)
	ON_WM_TIMER()
	ON_WM_LBUTTONDOWN()
	//ON_WM_SETCURSOR()
	ON_WM_ERASEBKGND()
	ON_WM_DESTROY()
	ON_WM_PAINT()
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CLabel message handlers

void CLabel::ReconstructFont()
{
	m_font.DeleteObject();
	if(m_bSetCircle||m_bSetCheck)
	{
		CFont   cf;
		LOGFONT lf;
		cf.CreatePointFont(30 * 10, m_lf.lfFaceName);
		cf.GetLogFont(&lf);
		m_lf.lfHeight = lf.lfHeight;
		m_lf.lfWidth  = lf.lfWidth;
	}
	m_font.CreateFontIndirect(&m_lf)?true:false;
}


void CLabel::OnTimer(UINT nIDEvent) 
{
	m_bState = !m_bState;

	InvalidateRect(NULL,TRUE);
	UpdateWindow();
	
	CStatic::OnTimer(nIDEvent);
}

BOOL CLabel::OnSetCursor(CWnd* pWnd, UINT nHitTest, UINT message) 
{
	if(m_hCursor != NULL && m_bSetHand)
	{
		::SetCursor(m_hCursor);
		return TRUE;
	}
	return CStatic::OnSetCursor(pWnd, nHitTest, message)?true:false;
}

void CLabel::OnLButtonDown(UINT nFlags, CPoint point) 
{
	if (m_bNotifyParent)
	{
		ShellExecute(NULL,TEXT("open"),m_strText,NULL,NULL,SW_SHOWNORMAL);
	}
	else
	{
		NMHDR nm;
		nm.hwndFrom = GetSafeHwnd();
		nm.idFrom   = GetDlgCtrlID();
		nm.code		= NM_LINKCLICK;
		::SendMessage(GetParent()->m_hWnd,WM_CLICKSTATICCTRL,nm.idFrom,(LPARAM) &nm);
	}
	CStatic::OnLButtonDown(nFlags, point);
}

void CLabel::SetLabelText(LPCSTR lpLabelTxt)
{
	m_strText = lpLabelTxt ;
	if(lpLabelTxt == NULL || strlen(lpLabelTxt))
	{
		SetHandCursor(false) ;
		SetFlashTime(0)      ;
	}

	if(!m_bitmapBk)
	{
		CRect rc ;
		HWND hWnd = GetSafeHwnd() ;
		::GetWindowRect(hWnd,rc);
		GetParent()->ScreenToClient(rc) ;
		::MoveWindow(hWnd,rc.left+1, rc.top+1,0, 0 ,true);
		::MoveWindow(hWnd,rc.left, rc.top, rc.Width(), rc.Height(),true);
		RedrawWindow() ;
	}
	else
	{
		RedrawWindow() ;
	}
}

void CLabel::SetTextColor(COLORREF crText)
{
	m_crText = crText;
	RedrawWindow() ;
}

void CLabel::SetFontBold(bool bBold)
{
	m_lf.lfWeight = bBold ? FW_BOLD : FW_NORMAL;
	ReconstructFont();
	RedrawWindow();
}

void CLabel::SetFontUnderline(bool bSet)
{	
	m_lf.lfUnderline = bSet;
	ReconstructFont();
	RedrawWindow();
}

void CLabel::SetFontItalic(bool bSet)
{
	m_lf.lfItalic = bSet;
	ReconstructFont();
	RedrawWindow();
}

void CLabel::SetFontSize(int nSize)
{
	CFont cf;
	LOGFONT lf;

	cf.CreatePointFont(nSize * 10, m_lf.lfFaceName);
	cf.GetLogFont(&lf);
	m_lf.lfHeight = lf.lfHeight;
	m_lf.lfWidth  = lf.lfWidth;
	ReconstructFont();
	RedrawWindow();
}

void CLabel::SetBkColor(COLORREF crBkgnd)
{
	m_crBkGround = crBkgnd ;
}

void CLabel::SetFontName(LPCTSTR lpFontName)
{
	if(lpFontName != NULL)
	{
		_tcscpy(m_lf.lfFaceName,lpFontName);
		ReconstructFont();
		RedrawWindow();
	}
}

void CLabel::SetLink(bool bLink,bool bNotifyParent)
{
	m_bLink = bLink;
	m_bNotifyParent = bNotifyParent;

	if (bLink)
		ModifyStyle(0,SS_NOTIFY);
	else
		ModifyStyle(SS_NOTIFY,0);
}

void CLabel::SetLinkCursor(HCURSOR hCursor)
{
	m_hCursor = hCursor;
}

void CLabel::SetRotationAngle(UINT nAngle,bool bRotation)
{
	m_lf.lfEscapement = m_lf.lfOrientation = (nAngle * 10);
	m_bRotation = bRotation;
	ReconstructFont();	
	RedrawWindow();
}

void CLabel::SetLeftAlign(bool bLAlign)
{
	m_bLAlign = bLAlign;
}

void CLabel::SetBkBitmap(DWORD dwbtID)
{
	m_bitmapID = dwbtID;
	m_bitmapBk = true ;
}

void CLabel::SetBkBitmap(CBitmap *pBkbitmap)
{
	m_bitmapBk = true ;
	m_pBkbitmap = pBkbitmap ;
}

void CLabel::SetParentBkMemDC(CDC *pPBkMemDC,int nX,int nY,int nWidth,int nHeight)
{
	m_bitmapBk = true ;
	m_pParentBKMemDC = pPBkMemDC;
	m_nParXPos = nX ;
	m_nParYPos = nY;
	m_nParWidth = nWidth ;
	m_nParHeight = nHeight ;
}


void CLabel::SetHandCursor(bool bHand)
{
	m_bSetHand = bHand;
}

BOOL CLabel::OnEraseBkgnd(CDC* pDC) 
{
	return TRUE ;
}

void CLabel::SetTransparent(bool bTrans)
{
	m_bTransparent = bTrans ;
}

void CLabel::SetFlashTime(DWORD dwTime)
{
	KillTimer(1) ;
	if(dwTime > 0)
	{
		SetTimer(1,dwTime,NULL) ;
	}
	else
	{
		m_bState = false ;
	}
}

void CLabel::OnDestroy() 
{
	CStatic::OnDestroy();

	if(m_font.GetSafeHandle())
		m_font.DeleteObject();
	KillTimer(1) ;	
}

void CLabel::SetFlaseColor(COLORREF crText,COLORREF crBkgnd)
{
	m_crFlashText	  = crText  ;
	m_crFlashBkGround = crBkgnd ;
}

void CLabel::DrawLabel(CDC *pDC) 
{
	CRect rc;
	DWORD dwFlags = 0;
	GetClientRect(rc);
	CString strText = m_strText;
	//if(m_strText.IsEmpty())
	{
		GetWindowText(strText);
		m_strText = strText;
	}
	DWORD nAlignFlag = DT_CENTER|DT_VCENTER| DT_END_ELLIPSIS;
	if(m_bLAlign)
		nAlignFlag = DT_LEFT | DT_SINGLELINE | DT_NOCLIP | DT_VCENTER | DT_END_ELLIPSIS;

	if(m_bitmapBk)
	{
		CDC memDC;
		memDC.CreateCompatibleDC(pDC) ;

		CBitmap memBitmap;
		memBitmap.CreateCompatibleBitmap(pDC,rc.Width(),rc.Height()) ;
		memDC.SelectObject(&memBitmap) ;
		memDC.SetTextColor(m_bState?m_crFlashText:m_crText);
		memDC.SelectObject(&m_font);
		if(m_pParentBKMemDC != NULL)
		{
			memDC.StretchBlt(0,0,rc.Width(),rc.Height(),m_pParentBKMemDC,
				m_nParXPos,m_nParYPos,m_nParWidth ,m_nParHeight,SRCCOPY);
			m_pParentBKMemDC->DeleteDC();
			ReleaseDC(m_pParentBKMemDC) ;
		}
		else
		{
			if(m_pBkbitmap != NULL)
			{
				memDC.SelectObject(m_pBkbitmap) ;
			}
			else
			{
				CBitmap bkBitmap;
				if(bkBitmap.LoadBitmap(m_bitmapID))
				{
					memDC.SelectObject(&bkBitmap) ;
					bkBitmap.DeleteObject() ;
				}
			}
		}

		memDC.SetBkMode(TRANSPARENT) ;
		rc.left+=5 ;
		if(m_bState)
		{
			memDC.SetTextColor(m_crFlashText) ;
		}
		memDC.DrawText(strText,-1,&rc,nAlignFlag);
		pDC->BitBlt(0,0,rc.Width()+5,rc.Height(),&memDC,0,0,SRCCOPY);
		memBitmap.DeleteObject() ;
		memDC.DeleteDC()  ;
		ReleaseDC(&memDC) ;
	}
	else
	{
		pDC->SelectObject(&m_font);
		if(!m_bTransparent)
			pDC->FillSolidRect(&rc,m_bState?m_crFlashBkGround:m_crBkGround) ;
		pDC->SetBkMode(TRANSPARENT) ;
		pDC->SetTextColor(m_bState?m_crFlashText:m_crText) ;
		pDC->DrawText(strText,-1,&rc,nAlignFlag);
	}
}

void CLabel::OnPaint() 
{
	CPaintDC dc(this);
	DrawLabel(&dc) ;
	//ReleaseDC(&dc) ;
}