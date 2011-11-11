#if !defined(AFX_ABOUT_H__E7C079B5_C3CA_4883_8E13_FBE8759B4034__INCLUDED_)
#define AFX_ABOUT_H__E7C079B5_C3CA_4883_8E13_FBE8759B4034__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000
// About.h : header file
//
#include "BmpBkDialog.h"
/////////////////////////////////////////////////////////////////////////////
// CAbout dialog

class CAbout : public CBmpBkDialog
{
// Construction
public:
	CAbout(CWnd* pParent = NULL);   // standard constructor

// Dialog Data
	//{{AFX_DATA(CAbout)
	enum { IDD = IDD_ABOUTBOX };
		// NOTE: the ClassWizard will add data members here
	//}}AFX_DATA


// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CAbout)
	protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
	//}}AFX_VIRTUAL

// Implementation
protected:

	// Generated message map functions
	//{{AFX_MSG(CAbout)
	virtual void OnOK();
	virtual BOOL OnInitDialog();
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()
};

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_ABOUT_H__E7C079B5_C3CA_4883_8E13_FBE8759B4034__INCLUDED_)
