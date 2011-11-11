#if !defined(AFX_SETDLG_H__6B19E7E6_0E1B_4A62_8472_8E9FCE1EB65A__INCLUDED_)
#define AFX_SETDLG_H__6B19E7E6_0E1B_4A62_8472_8E9FCE1EB65A__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000
// SetDlg.h : header file
//
#include "TabSheet.h"
#include "SetRingDlg.h"
#include "SetNetConnectDlg.h"
#include "SysSetDlg.h"
#include "BmpBkDialog.h"
#include "FluxWarnDlg.h"

#define WM_REFRESH_COMBOBOX WM_USER+500

/////////////////////////////////////////////////////////////////////////////
// CSetDlg dialog

class CSetDlg : public CBmpBkDialog
{
// Construction
public:
	CSetDlg(CWnd* pParent = NULL);   // standard constructor
	CSetRingDlg			m_setRingDlg;
	CSysSetDlg			m_setSysDlg;
	CSetNetConnectDlg   m_setConnectDlg; 
	CFluxWarnDlg		m_FluxWarnDlg;
// Dialog Data
	//{{AFX_DATA(CSetDlg)
	enum { IDD = IDD_DIALOG_SET };
	CTabSheet	m_setTable;
	//}}AFX_DATA


// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CSetDlg)
	protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
	//}}AFX_VIRTUAL

// Implementation
protected:

	// Generated message map functions
	//{{AFX_MSG(CSetDlg)
	virtual BOOL OnInitDialog();
	virtual void OnOK();
	afx_msg void OnButtonRenew();
	afx_msg void OnClose();
	virtual void OnCancel();
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()
	LRESULT OnRefreshComboBox(WPARAM wParam,LPARAM lParam);
public:
	bool IsModemExit(CString strModem);
	CImageList*  g_pSysImageList ;
};

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_SETDLG_H__6B19E7E6_0E1B_4A62_8472_8E9FCE1EB65A__INCLUDED_)
