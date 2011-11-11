#if !defined(AFX_NEWCARDDLG_H__042143B2_614E_49CD_A17B_5C93487D7CB9__INCLUDED_)
#define AFX_NEWCARDDLG_H__042143B2_614E_49CD_A17B_5C93487D7CB9__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000
// NewCardDlg.h : header file
//

/////////////////////////////////////////////////////////////////////////////
// CNewCardDlg dialog
#include "BmpBkDialog.h"
#include "data/phonebookpc.h"
#include "PBEdit.h"
class CNewCardDlg : public CBmpBkDialog
{
// Construction
public:
	CNewCardDlg(CWnd* pParent = NULL);   // standard constructor
	CNewCardDlg(CWnd* pParent,CString strPhone);
	CPhonebookPC phonebookPC;
	bool		 isNewCard;
	CString      m_className;
	int			 m_classID;
	int			 m_iRepeat;
// Dialog Data
	//{{AFX_DATA(CNewCardDlg)
	enum { IDD = IDD_NEWCARD_DLG };
	CPBEdit	m_EditOfficePB;
	CPBEdit	m_EditMobilePB;
	CPBEdit	m_EditHomePB;
	CPBEdit	m_EditFaxPB;
	CEdit	m_ctrlClassName;
	CString	m_strCompany;
	CString	m_strCountry;
	CString	m_strEmail;
	CString	m_strFaxPhone;
	CString	m_strHeadShip;
	CString	m_strHomePhone;
	CString	m_strMobilePhone;
	CString	m_strName;
	CString	m_strOfficePhone;
	CString	m_strClassName;
	//}}AFX_DATA


// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CNewCardDlg)
	protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
	//}}AFX_VIRTUAL

// Implementation
protected:

	// Generated message map functions
	//{{AFX_MSG(CNewCardDlg)
	afx_msg void OnButtonSelcardcls();
	virtual void OnOK();
	virtual BOOL OnInitDialog();
	afx_msg void OnButtonSelclass();
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()
private:
	CString m_strPhone;
};

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_NEWCARDDLG_H__042143B2_614E_49CD_A17B_5C93487D7CB9__INCLUDED_)
