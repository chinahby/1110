#if !defined(AFX_PHONENUMEDIT_H__2E85AF71_1229_4D99_A329_98DEC5D43013__INCLUDED_)
#define AFX_PHONENUMEDIT_H__2E85AF71_1229_4D99_A329_98DEC5D43013__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000
// PhoneNumEdit.h : header file
//

/////////////////////////////////////////////////////////////////////////////
// CPhoneNumEdit window

class CPhoneNumEdit : public CEdit
{
// Construction
public:
	CPhoneNumEdit();

// Attributes
public:

// Operations
public:

// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CPhoneNumEdit)
	//}}AFX_VIRTUAL

// Implementation
public:
	virtual ~CPhoneNumEdit();

	// Generated message map functions
protected:
	//{{AFX_MSG(CPhoneNumEdit)
	afx_msg void OnChar(UINT nChar, UINT nRepCnt, UINT nFlags);
	afx_msg void OnPaste(WPARAM, LPARAM);
	//}}AFX_MSG

	DECLARE_MESSAGE_MAP()
};

/////////////////////////////////////////////////////////////////////////////

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_PHONENUMEDIT_H__2E85AF71_1229_4D99_A329_98DEC5D43013__INCLUDED_)
