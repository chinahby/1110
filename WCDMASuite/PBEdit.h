#if !defined(AFX_PBEDIT_H__74EA86EA_D110_42AE_910B_A6A3728B6FE6__INCLUDED_)
#define AFX_PBEDIT_H__74EA86EA_D110_42AE_910B_A6A3728B6FE6__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000
// PBEdit.h : header file
//

/////////////////////////////////////////////////////////////////////////////
// CPBEdit window

class CPBEdit : public CEdit
{
// Construction
public:
	CPBEdit();

// Attributes
public:

// Operations
public:

// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CPBEdit)
	//}}AFX_VIRTUAL

// Implementation
public:
	virtual ~CPBEdit();

	// Generated message map functions
protected:
	//{{AFX_MSG(CPBEdit)
	afx_msg void OnChar(UINT nChar, UINT nRepCnt, UINT nFlags);
	//}}AFX_MSG

	DECLARE_MESSAGE_MAP()
};

/////////////////////////////////////////////////////////////////////////////

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_PBEDIT_H__74EA86EA_D110_42AE_910B_A6A3728B6FE6__INCLUDED_)
