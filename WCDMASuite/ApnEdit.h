#if !defined(AFX_APNEDIT_H__D0D2EB86_481B_461B_9F4C_785A31B5E84E__INCLUDED_)
#define AFX_APNEDIT_H__D0D2EB86_481B_461B_9F4C_785A31B5E84E__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000
// ApnEdit.h : header file
//

/////////////////////////////////////////////////////////////////////////////
// CApnEdit window

class CApnEdit : public CEdit
{
// Construction
public:
	CApnEdit();

// Attributes
public:

// Operations
public:

// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CApnEdit)
	//}}AFX_VIRTUAL

// Implementation
public:
	virtual ~CApnEdit();

	// Generated message map functions
protected:
	//{{AFX_MSG(CApnEdit)
	afx_msg void OnChar(UINT nChar, UINT nRepCnt, UINT nFlags);
	//}}AFX_MSG

	DECLARE_MESSAGE_MAP()
};

/////////////////////////////////////////////////////////////////////////////

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_APNEDIT_H__D0D2EB86_481B_461B_9F4C_785A31B5E84E__INCLUDED_)
