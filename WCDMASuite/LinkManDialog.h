#if !defined(AFX_LINKMANDIALOG_H__0A23381F_9AFC_42AD_956B_8AA79CF56C5A__INCLUDED_)
#define AFX_LINKMANDIALOG_H__0A23381F_9AFC_42AD_956B_8AA79CF56C5A__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000
// LinkManDialog.h : header file
//
#include "BmpBkDialog.h"
#include "SortListCtrl.h"
#include "data/phonebookpc.h"
#include "data/phonebookembed.h"
#include "NewClassDlg.h"
#include "BitmapEdit.h"
#include "ResizeCtrl.h"
#include "SplitterControl.h"
#include "RepeatDialog.h"

#define LINKMAN_PC      1
#define LINKMAN_SIM		0
#define LINKMAN_COUNT	1000
#define LINKMAN_NODIVCLASS 6

#define LINKMAN_TREE_PC    4
#define LINKMAN_TREE_SIM   5
#define LINKMAN_TREE_COUNT 21

#define  CUR_CTRL_CLSTREE  1
#define  CUR_CTRL_CARDLIST 2 

/////////////////////////////////////////////////////////////////////////////
// CLinkManDialog dialog

class CLinkManDialog : public CBmpBkDialog
{
// Construction
public:
	void InitUI();
	void InitTree();
	void ShowPBForPC(DWORD currItemData);
	void ShowPBForSIM();
	void SearchInPB(CString queryStr);
	void OnClickTreeItem();
	void OnUpdateCard();
	void SearchInPBSIM(CPhonebookEmbed& pb);
	void SetButtonState(BOOL bState);
	void ShowClassText(HTREEITEM treeItem, DWORD treeData);
	void SetTreeText(DWORD nID);
	void SelectCountItem();
	void SetInputFlag(BOOL isInput);
	void SetEditBtnState(bool bGray);
	void SetSendBtnState(bool bGray);
	void SetDelBtnState(bool bGray);
	void SetNewBtnState(bool bGray,bool bSim = false);
	void SetQueryBtnState(bool bGray);
	int  currSelListItem;
	HTREEITEM m_count;
	HTREEITEM m_linkManItem;
	HTREEITEM m_undivItem;
	HTREEITEM m_linkmanSmItem;
	bool m_bLDragging;
	bool m_bMoveable;
//	void OnSearch();
//	void SearchInPB(CPhonebook& pb);
	bool isNewClass;
	bool isNewCard;
	BYTE m_nCtrlDest;
	CEdit m_ctrlQuery;
	HTREEITEM m_htiDrag, m_htiDrop, m_htiOldDrop;
	DWORD desID;
	int   m_iRepeat;
	BOOL bInputOP;			//070713he

	CLinkManDialog(CWnd* pParent = NULL);   // standard constructor
	~CLinkManDialog();
	CImageList*  g_pSysImageList;
	CImageList		  *m_pDragImage;
	CImageList*  g_pTreeImage;
	CPhonebookPCClass *m_PBClass;
	DWORD	m_currTreeItemData;
	DWORD   m_otherClassID;
	HTREEITEM m_currTreeItem;
	CPhonebookPC m_phBookPC;
	CPhonebookEmbed m_pbookEmbed;
	CBitmap m_DragBTM;

// Dialog Data
	//{{AFX_DATA(CLinkManDialog)
	enum { IDD = IDD_DIALOG_LINKMAN };
	CColorBtn	m_ctrlEdit;
	CColorBtn	m_ctrlSendCard;
	CColorBtn	m_ctrlSearchCard;
	CColorBtn	m_ctrlDelCard;
	CColorBtn	m_ctrlNewCard;
	CTreeCtrl	m_ctrlLinkManTree;
	CListCtrl	m_ctrlLinkManShow;
	CSortListCtrl	m_ctrlLinkmanList;
	CString	m_strQuery;
	//}}AFX_DATA


// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CLinkManDialog)
	protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
	virtual LRESULT DefWindowProc(UINT message, WPARAM wParam, LPARAM lParam);
	//}}AFX_VIRTUAL

// Implementation
protected:

	// Generated message map functions
	//{{AFX_MSG(CLinkManDialog)
	virtual BOOL OnInitDialog();
	afx_msg void OnRclickListLinkman(NMHDR* pNMHDR, LRESULT* pResult);
	afx_msg void OnButtonNewLinkman();
	afx_msg void OnButtonNewclassLinkman();
	afx_msg void OnSelchangedTreeLinkman(NMHDR* pNMHDR, LRESULT* pResult);
	afx_msg void OnRclickTreeLinkman(NMHDR* pNMHDR, LRESULT* pResult);
	afx_msg void OnLinkmanSsms();
	afx_msg void OnLinkmanNewclass();
	afx_msg void OnLinkmanNewcard();
	afx_msg void OnLinkmanEditclass();
	afx_msg void OnLinkmanDelclass();
	afx_msg void OnLinkmanClearcard();
	afx_msg void OnLinkmanSms();
	afx_msg void OnLinkmanSelall();
	afx_msg void OnLinkmanSelother();
	afx_msg void OnLinkmanDial();
	afx_msg void OnLinkmanDel();
	afx_msg void OnLinkmanEditcard();
	afx_msg void OnClickListLinkman(NMHDR* pNMHDR, LRESULT* pResult);
	afx_msg void OnDblclkListLinkman(NMHDR* pNMHDR, LRESULT* pResult);
	afx_msg void OnItemchangedListLinkman(NMHDR* pNMHDR, LRESULT* pResult);
	afx_msg void OnButtonDelLinkman();
	afx_msg void OnButtonClearLinkman();
	afx_msg void OnButtonQueryLinkman();
	afx_msg void OnMouseMove(UINT nFlags, CPoint point);
	afx_msg void OnLButtonUp(UINT nFlags, CPoint point);
	afx_msg LONG OnNotifyList(WPARAM wParam, LPARAM lParam);
	afx_msg void OnClickTreeLinkman(NMHDR* pNMHDR, LRESULT* pResult);
	afx_msg void OnBeginrdragListLinkman(NMHDR* pNMHDR, LRESULT* pResult);
	afx_msg void OnBegindragListLinkman(NMHDR* pNMHDR, LRESULT* pResult);
	afx_msg void OnSize(UINT nType, int cx, int cy);
	afx_msg HBRUSH OnCtlColor(CDC* pDC, CWnd* pWnd, UINT nCtlColor);
	afx_msg void OnButtonSend();
	afx_msg void OnButtonEdit();
	afx_msg void OnKeydownListLinkman(NMHDR* pNMHDR, LRESULT* pResult);
	afx_msg void OnChangeEditQueryLinkman();
	//}}AFX_MSG
	afx_msg   void   OnDialNum(UINT   nID);   
	DECLARE_MESSAGE_MAP()
	LRESULT OnMyMove(WPARAM wParam,LPARAM lParam);
private:
//	CResizeCtrl m_resize;
	CSplitterControl m_wndSplitter1;

	void DoResize1(int delta);

	BOOL m_bFirst;
	BOOL m_bListFirst;

	TPhoneBookItemEx m_item ;

};

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_LINKMANDIALOG_H__0A23381F_9AFC_42AD_956B_8AA79CF56C5A__INCLUDED_)
