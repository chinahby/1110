#if !defined(AFX_APNDLG_H__FFA907EF_5FF5_4B18_8865_9544D7A1F97A__INCLUDED_)
#define AFX_APNDLG_H__FFA907EF_5FF5_4B18_8865_9544D7A1F97A__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000
// ApnDlg.h : header file
//
#include "BmpBkDialog.h"
#include "ColorBtn.h"
#include "ApnEdit.h"
/////////////////////////////////////////////////////////////////////////////
// CApnDlg dialog

enum EApnState
	{
		APN_NEW = 0,
		APN_EDIT,
		APN_DELETE
	};

class CApnDlg : public CBmpBkDialog
{
// Construction
public:
	CApnDlg(CWnd* pParent = NULL);   // standard constructor
	CApnDlg(CWnd* pParent ,CString strApn);   // standard constructor

// Dialog Data
	//{{AFX_DATA(CApnDlg)
	enum { IDD = IDD_DIALOG_APN };
	CApnEdit	m_apnEdit;
	CApnEdit	m_phoneEdit;
	CApnEdit	m_pswEdit;
	CApnEdit	m_userEdit;
	CString	m_strApn;
	CString	m_strPhone;
	CString	m_strPsw;
	CString	m_strUser;
	//}}AFX_DATA


// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CApnDlg)
	protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
	//}}AFX_VIRTUAL

// Implementation
protected:

	// Generated message map functions
	//{{AFX_MSG(CApnDlg)
	virtual void OnOK();
	virtual BOOL OnInitDialog();
	afx_msg void OnButtonAdvanced();
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()
private:
	CString m_strEdit;
	EApnState m_ApnState;
private:
	void GetEditValueByApn(CString strApn);
};

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_APNDLG_H__FFA907EF_5FF5_4B18_8865_9544D7A1F97A__INCLUDED_)
