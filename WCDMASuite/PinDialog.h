#if !defined(AFX_PINDIALOG_H__4098A9F3_PIN_44FD_9FE6_7B435EF03936__INCLUDED_)
#define AFX_PINDIALOG_H__4098A9F3_PIN_44FD_9FE6_7B435EF03936__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000
// PinDialog.h : header file
//
/////////////////////////////////////////////////////////////////////////////
// CPincode dialog
#include "BmpBkDialog.h"
#include "PBEdit.h"
#include "label.h"
#include "pinedit.h"
class CPincode : public CBmpBkDialog
{
// Construction
public:
	CPincode(CWnd* pParent = NULL);   // standard constructor
	
// Dialog Data
	//{{AFX_DATA(CPincode)
	enum { IDD = IDD_DIALOG_PIN };
	CStatic		m_pincode;
	CStatic 		m_remaintime;
	CPINEdit	m_Editpin;
	CString		m_strPIN;
	int			m_iremaintime;
	bool		m_bpinpuk;
	//}}AFX_DATA

	bool		OnPinlock();
	bool		OnPuklock();
	void		GetRemaintime();

// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CPincode)
	protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
	//}}AFX_VIRTUAL

// Implementation
protected:

	// Generated message map functions
	//{{AFX_MSG(CPincode)
	virtual void OnOK();
	virtual BOOL OnInitDialog();
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()
};

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_PINDIALOG_H__4098A9F3_PIN_44FD_9FE6_7B435EF03936__INCLUDED_)

