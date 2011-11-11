#if !defined(AFX_SELECTCLASSDLG_H__8F061C24_0147_4149_A4F6_EC2BAAFF9B4C__INCLUDED_)
#define AFX_SELECTCLASSDLG_H__8F061C24_0147_4149_A4F6_EC2BAAFF9B4C__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000
// SelectClassDlg.h : header file
//
#include "BmpBkDialog.h"
#include "Data/phonebookpc.h"
#include "LinkManDialog.h"
/////////////////////////////////////////////////////////////////////////////
// CSelectClassDlg dialog

class CSelectClassDlg : public CBmpBkDialog
{
// Construction
public:
	CSelectClassDlg(CWnd* pParent = NULL);   // standard constructor
	void InitTree();
	CPhonebookPCClass *m_PBClass;
	CImageList *g_pTreeImage;
	HTREEITEM m_count;
	DWORD     m_currTreeItemData;
	HTREEITEM m_currTreeItem;
	~CSelectClassDlg();
// Dialog Data
	//{{AFX_DATA(CSelectClassDlg)
	enum { IDD = IDD_DIALOG_SELECTCLASS };
	CTreeCtrl	m_ctrlListSel;
	//}}AFX_DATA


// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CSelectClassDlg)
	protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
	//}}AFX_VIRTUAL

// Implementation
protected:

	// Generated message map functions
	//{{AFX_MSG(CSelectClassDlg)
	virtual BOOL OnInitDialog();
	virtual void OnOK();
	afx_msg void OnSelchangedTreeSel(NMHDR* pNMHDR, LRESULT* pResult);
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()
};

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_SELECTCLASSDLG_H__8F061C24_0147_4149_A4F6_EC2BAAFF9B4C__INCLUDED_)
