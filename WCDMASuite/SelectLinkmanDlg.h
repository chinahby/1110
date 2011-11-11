#if !defined(AFX_SELECTLINKMANDLG_H__0BA55AC5_E42F_4D7C_8CFC_F31EE997606F__INCLUDED_)
#define AFX_SELECTLINKMANDLG_H__0BA55AC5_E42F_4D7C_8CFC_F31EE997606F__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000
// SelectLinkmanDlg.h : header file
//
#include "BmpBkDialog.h"
#include "SortListCtrl.h"
#include "data/phonebookpc.h"
//#include "Data/PhoneBook.h"
#include "Data/phonebookembed.h"
#include "LinkManDialog.h"

/////////////////////////////////////////////////////////////////////////////
// CSelectLinkmanDlg dialog


class CSelectLinkmanDlg : public CBmpBkDialog
{
// Construction
public:
	CSelectLinkmanDlg(CWnd* pParent = NULL);   // standard constructor
	~CSelectLinkmanDlg();
	void InitUI();
	void InitTree();
	void ShowPBForSIM();
	void ShowPBForPC(DWORD currItemData);
	void OnButtonSelall(CSortListCtrl m_listExp);
	void OnButtonSelrev(CSortListCtrl m_listExp);
	void Search(CString queryStr);
	void CSelectLinkmanDlg::SearchInPBSIM(CPhonebookEmbed& pb);
	HTREEITEM m_count;
	CImageList *g_pSysImageList;

	CPhonebookPCClass *m_PBClass;
	HTREEITEM m_currTreeItem;
	DWORD     m_currTreeItemData;
	CPhonebookPC m_phBookPC;
	CPhonebookEmbed m_pbookEmbed;
	std::vector<CString> m_numbers;
	//CArray<CString, CString> numbers;
	
// Dialog Data
	//{{AFX_DATA(CSelectLinkmanDlg)
	enum { IDD = IDD_DIALOG_SMSSELECTCARD };
	CStatic	m_ctrlSel;
	CSortListCtrl	m_ctrlListSel;
	CTreeCtrl	m_ctrlLinkManTree;
	CSortListCtrl	m_ctrlLinkmanList;
	CString	m_strQuery;
	//}}AFX_DATA


// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CSelectLinkmanDlg)
	protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
	//}}AFX_VIRTUAL

// Implementation
protected:

	// Generated message map functions
	//{{AFX_MSG(CSelectLinkmanDlg)
	virtual BOOL OnInitDialog();
	afx_msg void OnSelchangedTreeSelLinkman(NMHDR* pNMHDR, LRESULT* pResult);
	afx_msg void OnButtonAddtosel();
	afx_msg void OnButtonAddalltosel();
	afx_msg void OnButtonRemove();
	afx_msg void OnButtonRemoveall();
	afx_msg void OnRclickListForsel(NMHDR* pNMHDR, LRESULT* pResult);
	afx_msg void OnSelAllcard();
	afx_msg void OnReturnCard();
	virtual void OnOK();
	afx_msg void OnButtonSmssearch();
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()
};

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_SELECTLINKMANDLG_H__0BA55AC5_E42F_4D7C_8CFC_F31EE997606F__INCLUDED_)
