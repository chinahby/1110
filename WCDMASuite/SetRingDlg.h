#if !defined(AFX_SETRINGDLG_H__22A672A6_8E6D_43BC_B236_BA8EF2182D90__INCLUDED_)
#define AFX_SETRINGDLG_H__22A672A6_8E6D_43BC_B236_BA8EF2182D90__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000
// SetRingDlg.h : header file
//
#include "BmpBkDialog.h"
/////////////////////////////////////////////////////////////////////////////
// CSetRingDlg dialog

class CSetRingDlg : public CBmpBkDialog
{
// Construction
public:
	CSetRingDlg(CWnd* pParent = NULL);   // standard constructor

// Dialog Data
	//{{AFX_DATA(CSetRingDlg)
	enum { IDD = IDD_DIALOG_SETRING };
		// NOTE: the ClassWizard will add data members here
	//}}AFX_DATA


// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CSetRingDlg)
	protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
	//}}AFX_VIRTUAL

// Implementation
protected:

	// Generated message map functions
	//{{AFX_MSG(CSetRingDlg)
	afx_msg void OnButtonSetcallring();
	afx_msg void OnButtonSetsmsring();
	virtual BOOL OnInitDialog();
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()

public:
	CString m_strCallRing;
	CString m_strSMSRing;
};

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_SETRINGDLG_H__22A672A6_8E6D_43BC_B236_BA8EF2182D90__INCLUDED_)
