#if !defined(AFX_NEWCLASSDLG_H__5F7835F5_CFBF_4872_8570_90D48636EAAF__INCLUDED_)
#define AFX_NEWCLASSDLG_H__5F7835F5_CFBF_4872_8570_90D48636EAAF__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000
// NewClassDlg.h : header file
//

/////////////////////////////////////////////////////////////////////////////
// CNewClassDlg dialog
#include "BmpBkDialog.h"
#include "Data/phonebookpc.h"
#include "LinkManDialog.h"

class CNewClassDlg : public CBmpBkDialog
{
// Construction
public:
	CNewClassDlg(CWnd* pParent = NULL);   // standard constructor
	CPhonebookPCClass m_pPhonebook;
	void CreateClass();
	bool isNewClass;

// Dialog Data
	//{{AFX_DATA(CNewClassDlg)
	enum { IDD = IDD_NEWCLASS_DLG };
	CEdit	m_ctrlClassName;
	CString	m_strClassName;
	CString	m_strClassVoicePath;
	//}}AFX_DATA


// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CNewClassDlg)
	protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
	//}}AFX_VIRTUAL

// Implementation
protected:

	// Generated message map functions
	//{{AFX_MSG(CNewClassDlg)
	virtual BOOL OnInitDialog();
	afx_msg void OnButtonSelring();
	virtual void OnOK();
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()
};

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_NEWCLASSDLG_H__5F7835F5_CFBF_4872_8570_90D48636EAAF__INCLUDED_)
