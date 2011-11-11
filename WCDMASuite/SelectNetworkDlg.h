#if !defined(AFX_SELECTNETWORKDLG_H__458530AC_86AC_4E70_AE93_532D2C5BD943__INCLUDED_)
#define AFX_SELECTNETWORKDLG_H__458530AC_86AC_4E70_AE93_532D2C5BD943__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000
// SelectNetworkDlg.h : header file
//

#include "SortListCtrl.h"
#include "BmpBkDialog.h"
#include "WaitCOPS.h"
#include "RegisterCOPS.h"

/////////////////////////////////////////////////////////////////////////////
// SelectNetworkDlg dialog

class SelectNetworkDlg : public CBmpBkDialog
{
// Construction
public:
	SelectNetworkDlg(BOOL mode, CWnd* pParent = NULL);   // standard constructor

// Dialog Data
	//{{AFX_DATA(SelectNetworkDlg)
	enum { IDD = IDD_DIALOG_SELNETWORK };
	CBrush m_brush;
	CFont m_font;
	CColorBtn m_btnRefresh;
	CColorBtn m_btnRegister;
	CSortListCtrl m_networkList;
	BOOL m_rbtnAuto;
	BOOL m_rbtnManual;

	CWaitCOPS wcops;

	BOOL m_automode;

	CString choose_name;
	int m_nCurSelItemNetwork;

	CWinThread *m_ThreadCOPS;
	CWinThread *m_ThreadRegister;

		// NOTE: the ClassWizard will add data members here
	//}}AFX_DATA

// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(SelectNetworkDlg)
	protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
	void ShowNetstate();
	//}}AFX_VIRTUAL

// Implementation
protected:

	// Generated message map functions
	//{{AFX_MSG(SelectNetworkDlg)
	virtual BOOL OnInitDialog();
	afx_msg HBRUSH OnCtlColor(CDC* pDC, CWnd* pWnd, UINT nCtlColor);
	afx_msg void OnRefresh();
	afx_msg void OnRegister();
	afx_msg void OnAuto();
	afx_msg void OnManual();			
//	afx_msg LONG OnShowNetstate(WPARAM wParam, LPARAM lParam);
	afx_msg void OnClickListNetwork(NMHDR* pNMHDR, LRESULT* pResult);	
	afx_msg void OnItemchangedListNetwork(NMHDR* pNMHDR, LRESULT* pResult);
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()
};

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

UINT FindCOPS(LPVOID lpParameter);
UINT RegisterNetwork(LPVOID lpParameter);

#endif // !defined(AFX_SELECTNETWORKDLG_H__458530AC_86AC_4E70_AE93_532D2C5BD943__INCLUDED_)
