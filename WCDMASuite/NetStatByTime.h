#if !defined(AFX_NETSTATBYTIME_H__F0EADE90_D3E8_41AD_9EE5_959F835B4931__INCLUDED_)
#define AFX_NETSTATBYTIME_H__F0EADE90_D3E8_41AD_9EE5_959F835B4931__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000
// NetStatByTime.h : header file
//
#include "BmpBkDialog.h"
/////////////////////////////////////////////////////////////////////////////
// CNetStatByTime dialog

class CNetStatByTime : public CBmpBkDialog
{
// Construction
public:
	CNetStatByTime(CWnd* pParent = NULL);   // standard constructor

// Dialog Data
	//{{AFX_DATA(CNetStatByTime)
	enum { IDD = IDD_DIALOG_NETSTAT_TIME };
	CTime	m_StartTime;
	CTime	m_EndTime;
	//}}AFX_DATA


// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CNetStatByTime)
	protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
	//}}AFX_VIRTUAL

// Implementation
protected:

	// Generated message map functions
	//{{AFX_MSG(CNetStatByTime)
	virtual BOOL OnInitDialog();
	virtual void OnOK();
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()
};

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_NETSTATBYTIME_H__F0EADE90_D3E8_41AD_9EE5_959F835B4931__INCLUDED_)
