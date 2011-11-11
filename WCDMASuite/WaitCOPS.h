#if !defined(AFX_WAITCOPS_H__7C21450B_8825_43C6_80B9_99CA067920A0__INCLUDED_)
#define AFX_WAITCOPS_H__7C21450B_8825_43C6_80B9_99CA067920A0__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000
// WaitCOPS.h : header file
//

/////////////////////////////////////////////////////////////////////////////
// CWaitCOPS dialog

class CWaitCOPS : public CBmpBkDialog
{
// Construction
public:
	CWaitCOPS(CWnd* pParent = NULL);   // standard constructor

// Dialog Data
	//{{AFX_DATA(CWaitCOPS)
	enum { IDD = IDD_DIALOG_COPS };
	CBrush m_brush;
	CFont m_font;
		// NOTE: the ClassWizard will add data members here
	//}}AFX_DATA


// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CWaitCOPS)
	protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support

	//}}AFX_VIRTUAL

// Implementation
protected:

	// Generated message map functions
	//{{AFX_MSG(CWaitCOPS)
	virtual BOOL OnInitDialog();
	afx_msg HBRUSH OnCtlColor(CDC* pDC, CWnd* pWnd, UINT nCtlColor);
	afx_msg void OnTimer(UINT nIDEvent);
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()
};

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_WAITCOPS_H__7C21450B_8825_43C6_80B9_99CA067920A0__INCLUDED_)
