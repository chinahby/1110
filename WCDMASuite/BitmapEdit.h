#if !defined(AFX_BITMAPEDIT_H__48F5615E_777A_43AF_9903_4B9DCCCF3F37__INCLUDED_)
#define AFX_BITMAPEDIT_H__48F5615E_777A_43AF_9903_4B9DCCCF3F37__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000
// BitmapEdit.h : header file
//

class CBitmapEdit : public CEdit
{
// Construction
public:
	void SetTxtColor(COLORREF crTxtBkClr,COLORREF crSelTxtClr=RGB(0,0,0));
	void SetSelTxtColor(COLORREF crSelTxtBkClr,COLORREF crSelTxtClr=RGB(255,255,255));
	void SetFontSize(int nSize=22);
	void ReDraw();
	COLORREF m_crTxtClr ;
	COLORREF m_crTxtBkClr ;
	COLORREF m_crSelTxtBkClr ;
	COLORREF m_crSelTxtClr ;
	UINT     m_nLastChar;
	int		 m_nSelB ;
	int		 m_nSelE ;
	CBitmapEdit();
	virtual ~CBitmapEdit();

protected:
	LOGFONT	m_lf;
	CFont	m_font;
	bool	m_bFlag  ; 
	int		m_nFontSize;
	bool    m_bMouseDown ;
	int		m_nLastSel;
// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CBitmapEdit)
	protected:
	virtual LRESULT WindowProc(UINT message, WPARAM wParam, LPARAM lParam);
	//}}AFX_VIRTUAL

	// Generated message map functions
protected:
	//{{AFX_MSG(CBitmapEdit)
	afx_msg void OnPaint();
	afx_msg UINT OnGetDlgCode();
	afx_msg void OnMouseMove(UINT nFlags, CPoint point);
	afx_msg void OnLButtonDown(UINT nFlags, CPoint point);
	afx_msg void OnLButtonUp(UINT nFlags, CPoint point);
	afx_msg void OnSetfocus();
	afx_msg void OnKillfocus();
	afx_msg void OnNcLButtonDblClk(UINT nHitTest, CPoint point);
	afx_msg void OnLButtonDblClk(UINT nFlags, CPoint point);
	afx_msg void OnActivate(UINT nState, CWnd* pWndOther, BOOL bMinimized);
	afx_msg void OnChar(UINT nChar, UINT nRepCnt, UINT nFlags);
	//}}AFX_MSG

	DECLARE_MESSAGE_MAP()
};

/////////////////////////////////////////////////////////////////////////////

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_BITMAPEDIT_H__48F5615E_777A_43AF_9903_4B9DCCCF3F37__INCLUDED_)
