#if !defined(AFX_SYSSETDLG_H__0E6034F5_7AD0_4000_B3B8_C8108840376D__INCLUDED_)
#define AFX_SYSSETDLG_H__0E6034F5_7AD0_4000_B3B8_C8108840376D__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000
// SysSetDlg.h : header file
//


/////////////////////////////////////////////////////////////////////////////
// CSysSetDlg dialog
#include "BmpBkDialog.h"
class CSysSetDlg : public CBmpBkDialog
{
// Construction
public:
	CSysSetDlg(CWnd* pParent = NULL);   // standard constructor

// Dialog Data
	//{{AFX_DATA(CSysSetDlg)
	enum { IDD = IDD_DIALOG_SYSSET };
	CComboBox	m_selCom;
	BOOL	m_bAutoRun;
	BOOL	m_bMinToTask;
	BOOL	m_bShowWindow;
	BOOL	m_bAutoUpdate;
	//}}AFX_DATA


// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CSysSetDlg)
	protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
	//}}AFX_VIRTUAL

// Implementation
protected:

	// Generated message map functions
	//{{AFX_MSG(CSysSetDlg)
	virtual BOOL OnInitDialog();
	afx_msg void OnCheckAuto();
	afx_msg void OnCheckMin();
	afx_msg void OnCheckShowwnd();
	afx_msg void OnCheckAutoupdate();
	afx_msg void OnDropdownComboCom();
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()
};

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_SYSSETDLG_H__0E6034F5_7AD0_4000_B3B8_C8108840376D__INCLUDED_)
