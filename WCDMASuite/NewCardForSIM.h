#if !defined(AFX_NEWCARDFORSIM_H__4098A9F3_2702_44FD_9FE6_7B435EF03936__INCLUDED_)
#define AFX_NEWCARDFORSIM_H__4098A9F3_2702_44FD_9FE6_7B435EF03936__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000
// NewCardForSIM.h : header file
//
#include "data/phonebookembed.h"
/////////////////////////////////////////////////////////////////////////////
// CNewCardForSIM dialog
#include "BmpBkDialog.h"
#include "PBEdit.h"
class CNewCardForSIM : public CBmpBkDialog
{
// Construction
public:
	CNewCardForSIM(CWnd* pParent = NULL);   // standard constructor
//	CPhonebookEmbed m_phonebook;
	bool isNewCard;
// Dialog Data
	//{{AFX_DATA(CNewCardForSIM)
	enum { IDD = IDD_DIALOG_NEWCARD_SIM };
	CPBEdit	m_EditSimPB;
	CString	m_strNameSIM;
	CString	m_strNumSIM;
	//}}AFX_DATA

//	unsigned char m_ucNameLen;		// add by lab686 20091118
	unsigned char m_ucLenName;		// add by lab686 20091118
//	CString m_strNameLast;			// add by lab686 20091118

// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CNewCardForSIM)
	protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
	//}}AFX_VIRTUAL

// Implementation
protected:

	// Generated message map functions
	//{{AFX_MSG(CNewCardForSIM)
	virtual void OnOK();
	virtual BOOL OnInitDialog();
	afx_msg void OnChangeEditNameContent();
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()
};

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_NEWCARDFORSIM_H__4098A9F3_2702_44FD_9FE6_7B435EF03936__INCLUDED_)
