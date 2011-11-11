// SpSilderCtrl.cpp : implementation file
//

#include "stdafx.h"
#include "math.h"
#include "SpSilderCtrl.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

/////////////////////////////////////////////////////////////////////////////
// CSpSilderCtrl

CSpSilderCtrl::CSpSilderCtrl()
{
	m_bDraging    = false ;
	m_dwMinPos    = 0  ;
	m_dwMaxPos    = 100;
	m_nClickPt    = 0  ;
	m_nThumbWidth = 25 ;
	m_nThumbHeight= 25 ;
	m_nBackWidth  = 0  ;
	m_nBackHeight = 0  ;
	m_nHMargin    = 5  ;
	m_hParentWnd  = NULL ;
	m_pBkBitMap   = NULL ;
	m_pThBitMap   = NULL ;

}

CSpSilderCtrl::~CSpSilderCtrl()
{

}


BEGIN_MESSAGE_MAP(CSpSilderCtrl, CStatic)
	//{{AFX_MSG_MAP(CSpSilderCtrl)
	ON_WM_PAINT()
	ON_WM_LBUTTONDOWN()
	ON_WM_LBUTTONUP()
	ON_WM_MOUSEMOVE()
	ON_WM_DESTROY()
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CSpSilderCtrl message handlers

void CSpSilderCtrl::DrawSlider()
{
	Invalidate() ;
}

void CSpSilderCtrl::OnPaint() 
{
	CPaintDC dc(this); // device context for painting
	CDC *pDC = &dc;
	
	CDC memDC ;
	memDC.CreateCompatibleDC(pDC) ;
	CBitmap  tmpBt ;
	tmpBt.CreateCompatibleBitmap(pDC,m_rcBack.Width(),m_rcBack.Height()) ;
	memDC.SelectObject(&tmpBt) ;
	if(m_pBkBitMap != NULL)
	{
		CDC memBkDC ;
		memBkDC.CreateCompatibleDC(pDC) ;
		memBkDC.SelectObject(m_pBkBitMap) ;
		memDC.StretchBlt(0,0,m_rcBack.Width(),m_nBackHeight,&memBkDC,
			0,0,m_nBackMapWidth,m_nBackHeight,SRCCOPY) ;
		memBkDC.DeleteDC() ;
		ReleaseDC(&memBkDC) ;
	}
	else
	{
		memDC.FillSolidRect(&m_rcBack,RGB(255,0,255));
	}
	
	if(m_pThBitMap != NULL)
	{
		DrawTransparentBitmap(&memDC,m_pThBitMap,&m_rcThumb,RGB(255,0,255));
	}
	else
	{
		memDC.FillSolidRect(&m_rcThumb,RGB(0,0,255));
	}
	
	pDC->BitBlt(0,0,m_rcBack.Width(),m_rcBack.Height(),&memDC,0,0,SRCCOPY) ;
	memDC.DeleteDC()  ;
	//ReleaseDC(&memDC) ;
	HBITMAP OldBit = (HBITMAP)tmpBt.Detach() ;
	tmpBt.DeleteObject();
	DeleteObject(OldBit);
	tmpBt.DeleteObject();

}

void CSpSilderCtrl::InitSlider()
{
	GetWindowRect(m_rcBack) ;
	ScreenToClient(m_rcBack) ;
	m_rcThumb.left = m_rcBack.left+m_nHMargin;
	m_rcThumb.top  = (m_nBackHeight-m_nThumbHeight)/2;
	m_rcThumb.right= m_rcThumb.left+m_nThumbWidth;
	m_rcThumb.bottom = m_rcThumb.top+m_nThumbHeight;
	m_dwMinPos = 0 ;
	m_dwCurPos = 0 ;
	m_dwMaxPos = m_rcBack.Height() ;
	m_nBackWidth = m_rcBack.Width() - m_nThumbWidth - m_nHMargin*2 ;
	m_hParentWnd = GetParent()->m_hWnd ;
}

void CSpSilderCtrl::OnLButtonDown(UINT nFlags, CPoint point) 
{
	if(m_rcThumb.PtInRect(point))
	{
		m_nClickPt = point.x-m_rcThumb.left ;
		SetCapture() ;
		m_bDraging = true ;
	}
	else
	{
		ResetThumbRect(&point) ;
	}
	CStatic::OnLButtonDown(nFlags, point);
}

void CSpSilderCtrl::OnLButtonUp(UINT nFlags, CPoint point) 
{
	ReleaseCapture() ;
	DWORD dwLastPos = m_dwCurPos ;
	GetSLPos();
	DrawSlider() ;
	m_nClickPt = 0 ;
	m_bDraging = false ;
	if(dwLastPos != m_dwCurPos)
		SendPosChangeMessage() ;
	CStatic::OnLButtonUp(nFlags, point);
}

void CSpSilderCtrl::OnMouseMove(UINT nFlags, CPoint point) 
{
	if(m_bDraging)
	{
		//TRACE("POS:%d\n",GetSLPos());
		ResetThumbRect(&point) ;
		if(m_hParentWnd == NULL)
		{
			m_hParentWnd = GetParent()->m_hWnd ;
		}
		SendPosChangeMessage() ;
	}
	CStatic::OnMouseMove(nFlags, point);
}

void CSpSilderCtrl::ResetThumbRect(const CPoint *pNewPt)
{
	//TRACE("X:%d\n",pNewPt->x) ;
	int nMaxPt = m_rcBack.right-m_nHMargin - (m_nThumbWidth - m_nClickPt) ;
	int nNewPtX = pNewPt->x;
	if(nNewPtX < 0)
		nNewPtX = 0 ;
	if(nNewPtX > nMaxPt)
		nNewPtX = nMaxPt ;

	int nWidth = m_rcThumb.Width() ;
	m_rcThumb.left = nNewPtX - m_nClickPt;
	if(m_rcThumb.left < m_nHMargin)
		m_rcThumb.left = m_nHMargin;
	m_rcThumb.right= m_rcThumb.left+nWidth ;
	DrawSlider() ;
	//OnPaint();
}

void CSpSilderCtrl::SetSLRange(DWORD dwBegin, DWORD dwEnd)
{
	ASSERT(dwBegin<dwEnd) ;
	m_dwMinPos = dwBegin  ;
	m_dwMaxPos = dwEnd    ;
	if(m_dwMinPos>m_dwMaxPos)
	{
		DWORD dwSwp = m_dwMinPos ;
		m_dwMinPos  = m_dwMaxPos ;
		m_dwMaxPos  = m_dwMinPos ;
	}
	if(m_dwMinPos==m_dwMaxPos)
	{
		m_dwMaxPos +=10 ;
	}
}

void CSpSilderCtrl::SetSLPos(DWORD dwPos)
{
	DWORD dwTmpPos = dwPos;
	if(dwTmpPos<m_dwMinPos)
		dwTmpPos = m_dwMinPos;
	if(dwTmpPos>m_dwMaxPos)
		dwTmpPos = m_dwMaxPos;
	m_dwCurPos = dwTmpPos;
	int x = (m_nBackWidth*(dwTmpPos-m_dwMinPos))/(m_dwMaxPos-m_dwMinPos) ;
	CPoint pt(x,0) ;
	ResetThumbRect(&pt) ;
}

DWORD CSpSilderCtrl::GetSLPos()
{
	int nCurPos = m_rcThumb.left-m_nHMargin ;
	if( nCurPos>0 && (m_nBackWidth < (int)(m_dwMaxPos-m_dwMinPos)))
		nCurPos +=1 ;
	double dPoint =(m_dwMaxPos-m_dwMinPos)*(nCurPos*10.0/m_nBackWidth)/10.0+m_dwMinPos ;
	//TRACE("%d,%d,%d\n",m_rcThumb.left,m_nBackWidth,0xFFFF) ;
	m_dwCurPos = (DWORD)dPoint;
	if(m_dwCurPos>m_dwMaxPos)
		m_dwCurPos = m_dwMaxPos;
	return m_dwCurPos ;
}

void CSpSilderCtrl::SendPosChangeMessage()
{
	DWORD dwInfo = GetSLPos();
	dwInfo<<=16 ;
	::SendMessage(m_hParentWnd,WM_HSCROLL,dwInfo,(LPARAM)m_hWnd) ;
	::SendMessage(m_hParentWnd,WM_VSCROLL,dwInfo,(LPARAM)m_hWnd) ;
}

void CSpSilderCtrl::SetSLBitmap(int nBkMapID, int nThumbMapID)
{
	if(m_pBkBitMap != NULL)
	{
		m_pBkBitMap->DeleteObject() ;
		delete m_pBkBitMap ;
		m_pBkBitMap = NULL ;
	}
	if(m_pThBitMap != NULL)
	{
		m_pThBitMap->DeleteObject() ;
		delete m_pThBitMap ;
		m_pThBitMap = NULL ;
	}
	m_pBkBitMap = new CBitmap() ;
	m_pBkBitMap->LoadBitmap(nBkMapID) ;
	m_pThBitMap = new CBitmap() ;
	m_pThBitMap->LoadBitmap(nThumbMapID) ;

	BITMAP bt ;
	m_pThBitMap->GetBitmap(&bt) ;
	m_nThumbWidth = bt.bmWidth ;
	m_nThumbHeight = bt.bmHeight ;
	
	m_pBkBitMap->GetBitmap(&bt) ;
	m_nBackHeight  = bt.bmHeight ;
	m_nBackMapWidth= bt.bmWidth ;
	InitSlider() ;
}

void CSpSilderCtrl::DrawTransparentBitmap(CDC *pDC,CBitmap *pTest,CRect *pBmpRc,COLORREF crColour) 
{
	CDC dcImage,dcMask; 
	
	BITMAP bm;
	pTest->GetBitmap(&bm); 
	int nWidth=bm.bmWidth,nHeight=bm.bmHeight; 
	int x=pBmpRc->left;
	int y=pBmpRc->top;
	
	//为图像及mask各创建一个DC 
	dcImage.CreateCompatibleDC(pDC); 
	dcMask.CreateCompatibleDC(pDC); 

	dcImage.SelectObject(pTest); 
	COLORREF crOldBack=pDC->SetBkColor(RGB(255,255,255)); 
	COLORREF crOldText=pDC->SetTextColor(RGB(0,0,0)); 
	
	CBitmap bitmapMask;			//为“掩码”位图创建一个单色bitmap 
	bitmapMask.CreateBitmap(nWidth, nHeight, 1, 1, NULL); //把mask位图装入mask DC 
	dcMask.SelectObject(&bitmapMask); //用透明色创建“掩码”位图 
	dcImage.SetBkColor(crColour);//crColor是位图中的透明色 
	dcMask.BitBlt(0, 0, nWidth, nHeight, &dcImage, 0, 0, SRCCOPY);//分3步进行实际的绘制 
	pDC->BitBlt(x, y, nWidth, nHeight, &dcImage, 0, 0, SRCINVERT); 
	pDC->BitBlt(x, y, nWidth, nHeight, &dcMask, 0, 0, SRCAND); 
	pDC->BitBlt(x, y, nWidth, nHeight, &dcImage, 0, 0, SRCINVERT); 

	pDC->SetBkColor(crOldBack); 
	pDC->SetTextColor(crOldText);
	dcImage.DeleteDC();
	dcMask.DeleteDC();
	ReleaseDC(&dcImage) ;
	ReleaseDC(&dcMask) ;
	bitmapMask.DeleteObject() ;
}

void CSpSilderCtrl::DrawTransparentBitmap(CDC *pDC, UINT IDImage,CRect *pBmpRc, COLORREF rgbMask)
{
	CBitmap TmpImage;
	TmpImage.LoadBitmap(IDImage) ;
	DrawTransparentBitmap(pDC, &TmpImage,pBmpRc, rgbMask);
	TmpImage.DeleteObject() ;
}

void CSpSilderCtrl::SetMargin(int nMargin)
{
	m_nHMargin = nMargin;
}

bool CSpSilderCtrl::GetDraging()
{
	return m_bDraging ;
}

void CSpSilderCtrl::OnDestroy() 
{
	CStatic::OnDestroy();
	
	if(m_pBkBitMap != NULL)
	{
		m_pBkBitMap->DeleteObject();
		delete m_pBkBitMap ;
		m_pBkBitMap  = NULL;
	}
	if(m_pThBitMap != NULL)
	{
		m_pThBitMap->DeleteObject();
		delete m_pThBitMap ;
		m_pThBitMap  = NULL;
	}	
}
