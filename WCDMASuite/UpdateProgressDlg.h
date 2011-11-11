#if !defined(AFX_UPDATEPROGRESSDLG_H__34DBE0C3_FB09_47F2_B7AC_27D51BDD8EBD__INCLUDED_)
#define AFX_UPDATEPROGRESSDLG_H__34DBE0C3_FB09_47F2_B7AC_27D51BDD8EBD__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000
// UpdateProgressDlg.h : header file
//

#include "BmpBkDialog.h"
#include <Afxmt.h>
/////////////////////////////////////////////////////////////////////////////
// CUpdateProgressDlg dialog

class CUpdateProgressDlg : public CBmpBkDialog
{
// Construction
public:
	CUpdateProgressDlg(CEvent&EventStopUpdate, CWnd* pParent = NULL);   // standard constructor

// Dialog Data
	//{{AFX_DATA(CUpdateProgressDlg)
	enum { IDD = IDD_DIALOG_UPDATEPROGRESS };
	CProgressCtrl	m_ctrlProgress;
	//}}AFX_DATA


// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CUpdateProgressDlg)
	protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
	//}}AFX_VIRTUAL

// Implementation
protected:

	// Generated message map functions
	//{{AFX_MSG(CUpdateProgressDlg)
	virtual BOOL OnInitDialog();
	afx_msg void OnPaint();
	afx_msg void OnClose();
	afx_msg HBRUSH OnCtlColor(CDC* pDC, CWnd* pWnd, UINT nCtlColor);
	virtual void OnOK( );
	virtual void OnCancel( );
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()
public:
	//…Ë÷√Ω¯∂»
	void SetProgress(int nProgress,int nMax);	
private:
	CEvent& m_StopEvent;
};

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_UPDATEPROGRESSDLG_H__34DBE0C3_FB09_47F2_B7AC_27D51BDD8EBD__INCLUDED_)
