#if !defined(AFX_SETNETCONNECTDLG_H__67C6F7F5_36F2_4260_BE19_BEE0041DC613__INCLUDED_)
#define AFX_SETNETCONNECTDLG_H__67C6F7F5_36F2_4260_BE19_BEE0041DC613__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000
// SetNetConnectDlg.h : header file
//

/////////////////////////////////////////////////////////////////////////////
// CSetNetConnectDlg dialog

#include "BmpBkDialog.h"
#include "infodialup.h"
#include "Resource.h"
#include "phonenumedit.h"
#include "SortListCtrl.h"

class CInfoOptions;
class CSetNetConnectDlg : public CBmpBkDialog
{
// Construction
public:
	CSetNetConnectDlg(CWnd* pParent = NULL);   // standard constructor
	~CSetNetConnectDlg();
	CStringArray m_netName;
	int m_netNameSize;
	bool m_bSearchCops;   //ÊÇ·ñÕýÔÚËÑË÷ÍøÂç
// Dialog Data
	//{{AFX_DATA(CSetNetConnectDlg)
	enum { IDD = IDD_DIALOG_SETCONNECT };
	CComboBox	m_ApnCombo;
	CListCtrl	m_ctrlListSPName;
	CPhoneNumEdit	m_pEdtPhoneNum;
	CString	m_strPassWord;
	CString   m_strApn;
	CString	m_strPhoneNum;
	CString	m_strUser;
		// NOTE: the ClassWizard will add data members here
	//}}AFX_DATA


// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CSetNetConnectDlg)
	protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
	//}}AFX_VIRTUAL

// Implementation
private:
	int m_currentItem;
private:

//	CInfoOptions& m_Option;

	// Generated message map functions
	//{{AFX_MSG(CSetNetConnectDlg)
	virtual BOOL OnInitDialog();
	afx_msg void OnButtonSearchSp();
	afx_msg void OnButtonSetSp();
	afx_msg void OnClickListSpName(NMHDR* pNMHDR, LRESULT* pResult);
	afx_msg void OnDblclkListSpName(NMHDR* pNMHDR, LRESULT* pResult);
	afx_msg void OnItemchangedListSpName(NMHDR* pNMHDR, LRESULT* pResult);
	afx_msg void OnButtonNew();
	afx_msg void OnButtonEdit();
	afx_msg void OnButtonDelete();
	afx_msg void OnSelchangeComboApn();
	afx_msg void OnDropdownComboApn();
	//}}AFX_MSG
	afx_msg LONG OnSearchMSG(WPARAM wParam, LPARAM lParam);
	DECLARE_MESSAGE_MAP()
public:
	void RefreshComBox(BOOL bDelete = FALSE);
	void GetEditValueByApn(CString strApn);
	

	
};

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_SETNETCONNECTDLG_H__67C6F7F5_36F2_4260_BE19_BEE0041DC613__INCLUDED_)
