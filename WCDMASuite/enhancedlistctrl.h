#if !defined(AFX_ENHANCEDLISTCTRL_H__408080BD_15F5_42F0_8DB5_BC9B3B7FF4D3__INCLUDED_)
#define AFX_ENHANCEDLISTCTRL_H__408080BD_15F5_42F0_8DB5_BC9B3B7FF4D3__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000
// EnhancedListCtrl.h : header file
//

/////////////////////////////////////////////////////////////////////////////
// CEnhancedListCtrl window

#define WM_HOTTRACKLISTITEM  (WM_USER+0x2003)
#define WM_LISTKEYDOWNMSG    (WM_USER+0x3003)
#define LISTCOMMAND_DELETE   0x7FFF

#include "EnhancedHeaderCtrl.h"
class CEnhancedListCtrl : public CListCtrl
{
// Construction
public:
	CEnhancedListCtrl();
	CEnhancedHeaderCtrl m_enHeader ;

private:
	int  m_nLastItem  ;
	int  m_nToSelCount ;
	bool m_bHotAllItem ;
	bool m_bReDoKeyDown;
	static bool m_bShiftDown  ;
	static bool m_bCtrlDown   ; 
	static bool m_bAltDown    ; 
	bool m_bEnableDbItemColor ; 
	bool m_bSubItemCursor ;
	bool m_bEntryLaastCol ;
	int  m_nHighlight ;
	int  m_nHotItem   ;
	int  m_nMarkItem  ;
	int  m_nLSelItem  ;

	COLORREF m_clrDbItemBK  ;
	COLORREF m_clrNorBK  ;
	COLORREF m_clrNorTxt ;
	COLORREF m_clrMarkBK ;
	COLORREF m_clrMarkTxt;

	COLORREF m_clrHotBK  ;
	COLORREF m_clrHotTxt ;

	COLORREF m_clrFocusBK  ;
	COLORREF m_clrFocusTxt ;
	
	COLORREF m_clrLFocusBK  ;
	COLORREF m_clrLFocusTxt ;
	
	CImageList *m_pItemHeight ;
// Attributes
public:

// Operations
public:

// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CEnhancedListCtrl)
	public:
	virtual BOOL PreTranslateMessage(MSG* pMsg);
	protected:
	virtual void PreSubclassWindow();
	//}}AFX_VIRTUAL

// Implementation
public:
	void SetLastColEntry(bool bEntry=true);
	void SetItemIconIndex(int nItem,int nIndex=-1);
	void SetHeaderCtrlColor(COLORREF clrHeader=RGB(255,255,255),COLORREF clrLine=RGB(210,210,210)) ;
	void SetItemHeight( int nItemHeight) ;
	void EnableDbItemColor(bool bEnable=true,COLORREF clr=RGB(228,241,226));
	void GetItemColor(COLORREF *pClrBk,COLORREF *pClrTxt,int nItem,bool bHighLight);
	void SetSubitemCursor(bool bSetFlag=true);
	void SelectItem(int nItem,bool bSetVisible=false) ;
	void SetLastItemsVisible(int nItemCount) ;
	void SetMarkItem(int nItem = -1);
	void UnSelectItem(int nItem) ;
	void SetHotAllItem(bool bHotItem);
	int SelectAllItem()  ;
	int UnSelectAllItem();
	void SetReDoKeyDown(bool bDoFlag=true);
	virtual ~CEnhancedListCtrl();

	// Generated message map functions
protected:
	//{{AFX_MSG(CEnhancedListCtrl)
	afx_msg void OnMouseMove(UINT nFlags, CPoint point);
	afx_msg void OnLButtonDown(UINT nFlags, CPoint point);
	afx_msg void OnPaint();
	afx_msg BOOL OnEraseBkgnd(CDC* pDC);
	afx_msg void OnKeydown(NMHDR* pNMHDR, LRESULT* pResult);
	afx_msg void OnTimer(UINT nIDEvent);
	afx_msg void OnDividerdblclick(NMHDR* pNMHDR, LRESULT* pResult);
	afx_msg void OnColumnclick(NMHDR* pNMHDR, LRESULT* pResult);
	//}}AFX_MSG
	virtual void DrawItem(LPDRAWITEMSTRUCT lpDrawItemStruct);
	int OnMouseLeave(WPARAM wparam, LPARAM lparam);
	afx_msg void OnHeaderClickMsg(WPARAM wParam,LPARAM lParam);
	DECLARE_MESSAGE_MAP()
};


class CMemDC : public CDC
{
private:	
	CBitmap	m_bitmap;		// Offscreen bitmap
	CBitmap	*m_oldBitmap;	// bitmap originally found in CMemDC
	CDC		*m_pDC;			// Saves CDC passed in constructor
	CRect	m_rect;			// Rectangle of drawing area.
	BOOL	m_bMemDC;		// TRUE if CDC really is a Memory DC.

public:
	CMemDC(CDC* pDC,const CRect *pRect=NULL, BOOL bBg=FALSE) : CDC()
	{
		ASSERT(pDC != NULL); 

		// Some initialization
		m_pDC       = pDC;
		m_oldBitmap = NULL;
		m_bMemDC    = !pDC->IsPrinting();

		if(pRect == NULL)
		{
			pDC->GetClipBox(&m_rect);
		}
		else
		{
			m_rect = *pRect;
		}

		if(m_bMemDC)
		{
			CreateCompatibleDC(pDC);
			pDC->LPtoDP(&m_rect);

			m_bitmap.CreateCompatibleBitmap(pDC, m_rect.Width(), m_rect.Height());
			m_oldBitmap = SelectObject(&m_bitmap);

			//SetMapMode(pDC->GetMapMode());
			//SetWindowExt(pDC->GetWindowExt());
			//SetViewportExt(pDC->GetViewportExt());

			pDC->DPtoLP(&m_rect);
			SetWindowOrg(m_rect.left, m_rect.top);
		}
		else
		{
			// Make a copy of the relevent parts of the current DC for printing
			m_bPrinting = pDC->m_bPrinting;
			m_hDC       = pDC->m_hDC;
			m_hAttribDC = pDC->m_hAttribDC;
		}

		// Fill background 
		if(bBg)
			BitBlt( m_rect.left, m_rect.top,
					m_rect.Width(),	m_rect.Height(),
					m_pDC, m_rect.left, m_rect.top, SRCCOPY);
		else
			FillSolidRect(m_rect, pDC->GetBkColor());
	}
	
	~CMemDC()	
	{		
		if(m_bMemDC)
		{
			// Copy the offscreen bitmap onto the screen.
			m_pDC->BitBlt(m_rect.left, m_rect.top, m_rect.Width(), m_rect.Height(),
				this, m_rect.left, m_rect.top, SRCCOPY);			
			
			//Swap back the original bitmap.
			SelectObject(m_oldBitmap);		
		}
		else
		{
			// All we need to do is replace the DC with an illegal value,
			// this keeps us from accidently deleting the handles associated with
			// the CDC that was passed to the constructor.			
			m_hDC = m_hAttribDC = NULL;
		}	
	}
	
	// Allow usage as a pointer	
	CMemDC *operator->() 
	{
		return this;
	}	

	// Allow usage as a pointer	
	operator CMemDC *() 
	{
		return this;
	}
};




/////////////////////////////////////////////////////////////////////////////

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_ENHANCEDLISTCTRL_H__408080BD_15F5_42F0_8DB5_BC9B3B7FF4D3__INCLUDED_)
