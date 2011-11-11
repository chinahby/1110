#ifndef SORTHEADERCTRL_H
#define SORTHEADERCTRL_H

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000
#include <vector>

class CSortHeaderCtrl : public CHeaderCtrl
{
// Construction
public:
	CSortHeaderCtrl();

// Attributes
public:

// Operations
public:

// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CSortHeaderCtrl)
	public:
	virtual void Serialize(CArchive& ar);
	virtual void DrawItem(LPDRAWITEMSTRUCT lpDrawItemStruct );
	
	//}}AFX_VIRTUAL

// Implementation
public:
	virtual ~CSortHeaderCtrl();

	void SetSortArrow( const int iColumn, const BOOL bAscending );
	void SetHeadBkColor(COLORREF clrBk = RGB(156,184,223),COLORREF clrLine = (210,210,210));
	void SetHeadText(CString str);
	void ClearColTextBuf();
	// Generated message map functions
protected:
	
	COLORREF m_clrHeadColor ;
	COLORREF m_clrLineColor ;
	int m_iSortColumn;
	BOOL m_bSortAscending;
	std::vector<CString> headText;
	bool bDown;
//	int R,G,B;//ÆðÊ¼É«ËØRGB 

	//{{AFX_MSG(CSortHeaderCtrl)
	afx_msg void OnPaint();
	//}}AFX_MSG

	DECLARE_MESSAGE_MAP()
};

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

#endif // SORTHEADERCTRL_H
