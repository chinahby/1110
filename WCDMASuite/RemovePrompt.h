#if !defined(AFX_REMOVEPROMPT_H__24D7933E_8D8A_45D1_99BF_1D7C84CAD5E1__INCLUDED_)
#define AFX_REMOVEPROMPT_H__24D7933E_8D8A_45D1_99BF_1D7C84CAD5E1__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000
// RemovePrompt.h : header file
//
#include "BmpBkDialog.h"

/////////////////////////////////////////////////////////////////////////////
// CRemovePrompt dialog

class CRemovePrompt : public CBmpBkDialog
{
// Construction
public:
	CRemovePrompt(CWnd* pParent = NULL);   // standard constructor

// Dialog Data
	//{{AFX_DATA(CRemovePrompt)
	enum { IDD = IDD_DIALOG_REMOVE_PROMPT };
		// NOTE: the ClassWizard will add data members here
	
	//}}AFX_DATA


// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CRemovePrompt)
	protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
	//}}AFX_VIRTUAL

// Implementation
protected:

	// Generated message map functions
	//{{AFX_MSG(CRemovePrompt)
	virtual BOOL OnInitDialog();
	afx_msg void OnPaint();
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()
};

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_REMOVEPROMPT_H__24D7933E_8D8A_45D1_99BF_1D7C84CAD5E1__INCLUDED_)
