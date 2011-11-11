#if !defined(AFX_DIALNUMEDIT_H__1463EE96_46A7_4AD4_BCE8_3DA7BAECEE93__INCLUDED_)
#define AFX_DIALNUMEDIT_H__1463EE96_46A7_4AD4_BCE8_3DA7BAECEE93__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000
// DialNumEdit.h : header file
//

/////////////////////////////////////////////////////////////////////////////
// CDialNumEdit window

class CDialNumEdit : public CEdit
{
// Construction
public:
	CDialNumEdit();

// Attributes
public:

// Operations
public:

// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CDialNumEdit)
	//}}AFX_VIRTUAL

// Implementation
public:
	virtual ~CDialNumEdit();

	// Generated message map functions
protected:
	//{{AFX_MSG(CDialNumEdit)
	afx_msg void OnChar(UINT nChar, UINT nRepCnt, UINT nFlags);
	//}}AFX_MSG

	DECLARE_MESSAGE_MAP()
};

/////////////////////////////////////////////////////////////////////////////

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_DIALNUMEDIT_H__1463EE96_46A7_4AD4_BCE8_3DA7BAECEE93__INCLUDED_)
