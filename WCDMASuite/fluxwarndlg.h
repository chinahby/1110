#if !defined(AFX_FLUXWARN_H__0B6704A2_9F82_4CF8_8780_59C6C85E5B20__INCLUDED_)
#define AFX_FLUXWARN_H__0B6704A2_9F82_4CF8_8780_59C6C85E5B20__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000
// FluxWarn.h : header file
//
#include "BmpBkDialog.h"
/////////////////////////////////////////////////////////////////////////////
// CFluxWarn dialog

class CFluxWarnDlg : public CBmpBkDialog
{
// Construction
public:
	CFluxWarnDlg(CWnd* pParent = NULL);   // standard constructor

// Dialog Data
	//{{AFX_DATA(CFluxWarn)
	enum { IDD = IDD_DIALOG_FLUXSTAT };
	CSpinButtonCtrl	m_WarnSpin;
	CSpinButtonCtrl	m_UncntSpin;
	CEdit	m_NmWarnEdit;
	CEdit	m_NmcntEdit;
	CButton	m_UncntCheck;
	CSpinButtonCtrl	m_NmSpin;
	CSpinButtonCtrl	m_DateSpin;
	CButton	m_YNCheck;
	CEdit	m_NmEdit;
	CEdit	m_DateEdit;
	BOOL	m_bYN;
	CString	m_Date;
	CString	m_Nm;
	BOOL	m_bUncnt;
	CString	m_NmUncnt;
	CString	m_NmWarn;
	//}}AFX_DATA


// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CFluxWarn)
	protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
	//}}AFX_VIRTUAL

// Implementation
protected:

	// Generated message map functions
	//{{AFX_MSG(CFluxWarn)
	virtual BOOL OnInitDialog();
	afx_msg void OnCheckYn();
	afx_msg void OnDeltaposSpinDate(NMHDR* pNMHDR, LRESULT* pResult);
	afx_msg void OnDeltaposSpinNm(NMHDR* pNMHDR, LRESULT* pResult);
	afx_msg void OnDeltaposSpinWarn(NMHDR* pNMHDR, LRESULT* pResult);
	afx_msg void OnDeltaposSpinUncnt(NMHDR* pNMHDR, LRESULT* pResult);
	afx_msg void OnKillfocusEditDate();
	afx_msg void OnKillfocusEditUncnt();
	afx_msg void OnKillfocusEditWarn();
	afx_msg void OnKillfocusEditNm();
	afx_msg void OnCheckUncnt();
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()

private:
	BOOL GetValueFromReg();
	void EnableCtrls();
	void DisableCtrls();
};

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_FLUXWARN_H__0B6704A2_9F82_4CF8_8780_59C6C85E5B20__INCLUDED_)
