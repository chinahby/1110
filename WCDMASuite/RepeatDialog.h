#if !defined(AFX_REPEATDIALOG_H__D5927BBF_6C92_4728_9D93_EC9A7AA80CAB__INCLUDED_)
#define AFX_REPEATDIALOG_H__D5927BBF_6C92_4728_9D93_EC9A7AA80CAB__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000
// RepeatDialog.h : header file
//

/////////////////////////////////////////////////////////////////////////////
// CRepeatDialog dialog
#include "BmpBkDialog.h"
#define  REPEAT_OK      1
#define  REPEAT_OK_ALL  2
#define  REPEAT_NO		3
#define  REPEAT_NO_ALL  4

class CRepeatDialog : public CBmpBkDialog
{
// Construction
public:
	int m_repeat;
	CRepeatDialog(CWnd* pParent = NULL);   // standard constructor

// Dialog Data
	//{{AFX_DATA(CRepeatDialog)
	enum { IDD = IDD_DIALOG_REPEAT };
	CStatic	m_ctrlRepeat;
	//}}AFX_DATA


// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CRepeatDialog)
	protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
	//}}AFX_VIRTUAL

// Implementation
protected:

	// Generated message map functions
	//{{AFX_MSG(CRepeatDialog)
	virtual void OnOK();
	virtual void OnCancel();
	afx_msg void OnButtonAllOk();
	afx_msg void OnButtonAllNo();
	virtual BOOL OnInitDialog();
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()
};

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_REPEATDIALOG_H__D5927BBF_6C92_4728_9D93_EC9A7AA80CAB__INCLUDED_)
