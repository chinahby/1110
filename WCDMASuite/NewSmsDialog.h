#if !defined(AFX_NEWSMSDIALOG_H__C2DA915D_DC68_474D_ACBA_A24E04BE82A7__INCLUDED_)
#define AFX_NEWSMSDIALOG_H__C2DA915D_DC68_474D_ACBA_A24E04BE82A7__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000
// NewSmsDialog.h : header file
//
#include "BmpBkDialog.h"
#include "mobilemanager.h"
#include "SelectLinkmanDlg.h"
#include "DialNumEdit.h"
#include "GlobalDataCenter.h"
/////////////////////////////////////////////////////////////////////////////
// CNewSmsDialog dialog

class CNewSmsDialog : public CBmpBkDialog
{
// Construction
public:
	CNewSmsDialog(CWnd* pParent = NULL);   // standard constructor
	CNewSmsDialog(CWnd* pParent ,CString strPhone,int nNum);
	CSelectLinkmanDlg selLinkManDlg;
	CString strNumbers;
	void UpdateInfo(CString numbers);
	
// Dialog Data
	//{{AFX_DATA(CNewSmsDialog)
	enum { IDD = IDD_DIALOG_NEW_SMS };
	CStatic	m_ctrlSendNum;
	CStatic	m_ctrlContent;
	CDialNumEdit	m_ctrlSmsNum;
	CString	m_strSendSmsNum;
	CString	m_strSendSmsContent;
	CString	m_strSmsInfo;
	CString	m_strContent;
	CString	m_strSendNum;
	//}}AFX_DATA


// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CNewSmsDialog)
	protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
	//}}AFX_VIRTUAL

// Implementation
protected:

	// Generated message map functions
	//{{AFX_MSG(CNewSmsDialog)
	afx_msg void OnButtonSmsSave();
	afx_msg void OnButtonSend();
	afx_msg void OnButtonSelLinkman();
	afx_msg void OnSetfocusEditSmsNum();
	afx_msg void OnChangeEditSmsNum();
	virtual void OnCancel();
	virtual BOOL OnInitDialog();
	afx_msg void OnChangeEditSmsContent();
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()
private:
	CWnd* m_pParent;
	CString m_strPhone;
	int m_nNum;
	bool m_bEditSMS;
	CString m_strOriSendNum ;
	CString m_strOriContent ;
};

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_NEWSMSDIALOG_H__C2DA915D_DC68_474D_ACBA_A24E04BE82A7__INCLUDED_)
