#if !defined(AFX_COLORBTN_H__0199C4BC_F6C2_4D2D_98B2_3AEF3393FA71__INCLUDED_)
#define AFX_COLORBTN_H__0199C4BC_F6C2_4D2D_98B2_3AEF3393FA71__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000
// ColorBtn.h : header file
//

/////////////////////////////////////////////////////////////////////////////
// CColorBtn window

class CColorBtn : public CButton
{
// Construction
public:
	void Draw();
	void SetRoundRect() ;
	void SetBitmapID(int nNorID,int nDownID=-1) ;
	void SetBitmapPath(LPCTSTR lpNorPath,LPCTSTR lpDownPath=NULL,LPCTSTR lpMovePath=NULL) ;
	void SetHold(bool bHold = true) {m_bHold = bHold; Draw() ;} ;
	void SetBkColor(COLORREF clrBkNor,COLORREF clrBkOver);
	void SetShowDown(bool bShow=true);
	void SetHitOver(bool bHitOver=true);
	void SetDrawTextFlag(bool bDrawFlag=true);
	void SetToolTipTxt(LPCTSTR lpTipTxt);
	void IsCurrentBtn(bool bCurrBtn);
	CColorBtn();
	virtual ~CColorBtn();
public:
	void ReleaseBmpPathInfo();	
private:
	bool            m_bCurrBtn;
	CToolTipCtrl	*m_pToolTip ;
	COLORREF m_clrBkNor    ;
	COLORREF m_clrText     ;
	COLORREF m_clrBkOver   ;
	DWORD	 m_dwNorBitID  ;
	DWORD	 m_dwDownBitID ;
	TCHAR     *m_pszNorBmpPath ;
	TCHAR     *m_pszDownBmpPath;
	TCHAR     *m_pszMoveBmpPath;
	TCHAR     *tempPath;
	bool	 m_bHold ;
	bool	 m_bDbClick ;
	bool	 m_bHitOver  ;
	bool	 m_bOvering  ;
	bool	 m_bDraging  ;
	bool	 m_bShowDown ;
	bool	 m_bIsBitMapBtn ;
	bool	 m_bIsInRect  ;
	bool	 m_bMouseDown ;
	bool	 m_bDrawSelfText ;
	bool     m_bSetRgn  ;
	bool     m_bMove;
	char     *m_pszGrayBmpPath;

// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CColorBtn)
	public:
	virtual void DrawItem(LPDRAWITEMSTRUCT lpDrawItemStruct);
	virtual BOOL PreTranslateMessage(MSG* pMsg);
	protected:
	virtual void PreSubclassWindow();
	//}}AFX_VIRTUAL

// Implementation


	// Generated message map functions
protected:
	//{{AFX_MSG(CColorBtn)
	afx_msg void OnLButtonDown(UINT nFlags, CPoint point);
	afx_msg void OnLButtonUp(UINT nFlags, CPoint point);
	afx_msg void OnMouseMove(UINT nFlags, CPoint point);
	afx_msg BOOL OnSetCursor(CWnd* pWnd, UINT nHitTest, UINT message);
	afx_msg void OnLButtonDblClk(UINT nFlags, CPoint point);
	afx_msg void OnPaint();
	afx_msg BOOL OnEraseBkgnd(CDC* pDC);
	afx_msg void OnDeleteItem(int nIDCtl, LPDELETEITEMSTRUCT lpDeleteItemStruct);
	afx_msg void OnDrawItem(int nIDCtl, LPDRAWITEMSTRUCT lpDrawItemStruct);
	//}}AFX_MSG
	LRESULT OnMouseLeave(WPARAM wparam, LPARAM lparam);
	DECLARE_MESSAGE_MAP()
};

class CBtnDC : public CDC 
{
private:
    CBitmap*        m_bitmap;
    CBitmap*        m_oldBitmap;
    CDC*            m_pDC;
    CRect           m_rcBounds;
public:
    CBtnDC(CDC* pDC, const CRect& rcBounds) : CDC()
    {
		CreateCompatibleDC(pDC);
		m_bitmap = new CBitmap;
		m_bitmap->CreateCompatibleBitmap(pDC, rcBounds.Width(), rcBounds.Height());
		m_oldBitmap = SelectObject(m_bitmap);
		m_pDC = pDC;
		m_rcBounds = rcBounds;
    }

    ~CBtnDC() 
    {
        m_pDC->BitBlt(m_rcBounds.left, m_rcBounds.top, m_rcBounds.Width(), m_rcBounds.Height(), 
                                this, m_rcBounds.left, m_rcBounds.top, SRCCOPY);
        SelectObject(m_oldBitmap);

        if (m_bitmap != NULL)
			delete m_bitmap;
    }

    CBtnDC* operator->() 
	{
        return this;
    }
};

/////////////////////////////////////////////////////////////////////////////

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_COLORBTN_H__0199C4BC_F6C2_4D2D_98B2_3AEF3393FA71__INCLUDED_)
