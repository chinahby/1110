#if !defined(AFX_ENHANCEDHEADERCTRL_H__E6AC4A4E_B870_46DF_93C1_EA4B88C4F2E1__INCLUDED_)
#define AFX_ENHANCEDHEADERCTRL_H__E6AC4A4E_B870_46DF_93C1_EA4B88C4F2E1__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000
// EnhancedHeaderCtrl.h : header file
//

/////////////////////////////////////////////////////////////////////////////
// CEnhancedHeaderCtrl window

#define WM_HEADERCLICKMSG (WM_USER+0x3008)

class CEnhancedHeaderCtrl : public CHeaderCtrl
{
// Construction
public:
	CEnhancedHeaderCtrl();
	COLORREF m_clrHeadColor ;
	COLORREF m_clrLineColor ;
	bool	 m_bResizeAble  ;
	bool	 m_bLeftBtnDown ;
// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CEnhancedHeaderCtrl)
	//}}AFX_VIRTUAL

// Implementation 140,185,248
public: 
	int GetActiveCol(CPoint &pt);
	void SetHeadBkColor(COLORREF clrBk=RGB(215,215,215),COLORREF clrLine=RGB(210,210,210));//(255,210,168)
	void SetResizeAble(bool bEnable=true) ;
	virtual ~CEnhancedHeaderCtrl();

	// Generated message map functions
protected:
	//{{AFX_MSG(CEnhancedHeaderCtrl)
	afx_msg void OnPaint();
	afx_msg void OnLButtonDblClk(UINT nFlags, CPoint point);
	afx_msg void OnMouseMove(UINT nFlags, CPoint point);
	afx_msg void OnLButtonDown(UINT nFlags, CPoint point);
	afx_msg void OnLButtonUp(UINT nFlags, CPoint point);
	//}}AFX_MSG

	DECLARE_MESSAGE_MAP()
};

/////////////////////////////////////////////////////////////////////////////

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_ENHANCEDHEADERCTRL_H__E6AC4A4E_B870_46DF_93C1_EA4B88C4F2E1__INCLUDED_)
