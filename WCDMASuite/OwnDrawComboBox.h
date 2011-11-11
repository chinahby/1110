#if !defined(AFX_OWNDRAWCOMBOBOX_H__509B5FE9_D9E8_4A51_904C_C481F5879450__INCLUDED_)
#define AFX_OWNDRAWCOMBOBOX_H__509B5FE9_D9E8_4A51_904C_C481F5879450__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000
// OwnDrawComboBox.h : header file
//

/////////////////////////////////////////////////////////////////////////////
// COwnDrawComboBox window

class COwnDrawComboBox : public CComboBox
{
// Construction
public:
	COwnDrawComboBox();
	bool	m_bMouseIn  ;
	bool	m_bMouseDown;
	bool	m_bIsDorp   ;
	bool	m_bCloseUp   ;
	DWORD   m_dwNormalBtID;
	DWORD   m_dwHoverBtID ;
	DWORD   m_dwDownBtID  ;
	static void SetCmbThumbDefBtIDs(DWORD dwNormal, DWORD dwHover, DWORD dwDown) ;
	void SetCmbThumbBtIDs(DWORD dwNormal, DWORD dwHover, DWORD dwDown) ;
// Attributes
public:

// Operations
public:

// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(COwnDrawComboBox)
	//}}AFX_VIRTUAL

// Implementation
public:
	virtual ~COwnDrawComboBox();

	// Generated message map functions
protected:
	//{{AFX_MSG(COwnDrawComboBox)
	afx_msg void OnPaint();
	afx_msg void OnCloseup();
	afx_msg void OnDropdown();
	afx_msg void OnTimer(UINT nIDEvent);
	afx_msg void OnMouseMove(UINT nFlags, CPoint point);
	afx_msg void OnLButtonDown(UINT nFlags, CPoint point);
	afx_msg void OnKillfocus();
	//}}AFX_MSG

	DECLARE_MESSAGE_MAP()
};

/////////////////////////////////////////////////////////////////////////////

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_OWNDRAWCOMBOBOX_H__509B5FE9_D9E8_4A51_904C_C481F5879450__INCLUDED_)
