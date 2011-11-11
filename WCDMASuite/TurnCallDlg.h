#if !defined(AFX_TURNCALLDLG_H__FF74C5C8_B782_4F22_874B_8704E01EC2AF__INCLUDED_)
#define AFX_TURNCALLDLG_H__FF74C5C8_B782_4F22_874B_8704E01EC2AF__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000
// TurnCallDlg.h : header file
//
#include "BmpBkDialog.h"
/////////////////////////////////////////////////////////////////////////////
// CTurnCallDlg dialog

class CTurnCallDlg : public CBmpBkDialog
{
// Construction
public:
	CTurnCallDlg(CWnd* pParent = NULL);   // standard constructor

// Dialog Data
	//{{AFX_DATA(CTurnCallDlg)
	enum { IDD = IDD_DIALOG_TURNCALL };
	CColorBtn	m_cancel;
	CColorBtn	m_ok;
	//}}AFX_DATA


// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CTurnCallDlg)
	protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
	//}}AFX_VIRTUAL

// Implementation
protected:

	// Generated message map functions
	//{{AFX_MSG(CTurnCallDlg)
		// NOTE: the ClassWizard will add member functions here
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()
};

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_TURNCALLDLG_H__FF74C5C8_B782_4F22_874B_8704E01EC2AF__INCLUDED_)
