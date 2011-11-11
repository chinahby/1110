#if !defined(AFX_SELCOMDLG_H__38E9918C_56DD_4222_BD4C_F3FE69B4FCDA__INCLUDED_)
#define AFX_SELCOMDLG_H__38E9918C_56DD_4222_BD4C_F3FE69B4FCDA__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000
// SelComDlg.h : header file
//

/////////////////////////////////////////////////////////////////////////////
// CSelComDlg dialog

class CSelComDlg : public CDialog
{
// Construction
public:
	CSelComDlg(CWnd* pParent = NULL);   // standard constructor

// Dialog Data
	//{{AFX_DATA(CSelComDlg)
	enum { IDD = IDD_DIALOG_COM };
	CString	m_strCom;
	//}}AFX_DATA


// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CSelComDlg)
	protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
	//}}AFX_VIRTUAL

// Implementation
protected:

	// Generated message map functions
	//{{AFX_MSG(CSelComDlg)
	virtual BOOL OnInitDialog();
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()
};

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_SELCOMDLG_H__38E9918C_56DD_4222_BD4C_F3FE69B4FCDA__INCLUDED_)
