#if !defined(AFX_RECORDDIALOG_H__FEB96DA1_A8DC_464F_975C_A521B552E943__INCLUDED_)
#define AFX_RECORDDIALOG_H__FEB96DA1_A8DC_464F_975C_A521B552E943__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000
// RecordDialog.h : header file
//
#include "BmpBkDialog.h"
#include "SortListCtrl.h"
#include "Data/ConnectRecord.h"
#include "data/CallRecord.h"
#include "ResizeCtrl.h"
#include "SplitterControl.h"

#define CALLRECORD				100
#define NETRECORD				101
#define NETRECORD_MONTH			102
#define NETRECORD_STATBYTIME	103

#define CALL_RECORD            0
#define CALL_RECORD_RECV	   1
#define CALL_RECORD_OUTCALL	   2
#define CALL_RECORD_NOTGET	   3

#define RECORD_IMAG_CALL			5
#define RECORD_IMAG_CALL_RECV		3
#define RECORD_IMAG_CALL_CALL		1
#define RECORD_IMAG_CALL_MISS		2

#define NETRECORD_IMAG			20    
#define NETRECORD_IMAG_ALL		18
#define NETRECORD_IMAG_MONTH	19
#define NETRECORD_IMAG_TIME		0

#define NET_RECORD				4
#define NET_RECORD_ALL			5
#define NET_RECORD_MONTH		6
#define NET_RECORD_TIME			7

//定义呼叫记录列
#define CALLRECORD_PHONE		2

/////////////////////////////////////////////////////////////////////////////
// CRecordDialog dialog

class CRecordDialog : public CBmpBkDialog
{
// Construction
public:
	void ShowList(CallRecordType Type);
	void ShowListNetRecord();
	void InitUI();
	void LoadListHead(int Type);
	void ShowTreeText(DWORD currItemData);
	CRecordDialog(CWnd* pParent = NULL);   // standard constructor
	~CRecordDialog();
	HTREEITEM currItem;
	DWORD  currItemData;
	CImageList*  g_pSysImageList;
	CConnectRecord m_connectRecord;
	CCallRecord    m_callRecord;
	HTREEITEM m_talkItem;
	HTREEITEM m_dialItem;
	HTREEITEM m_missItem;
	HTREEITEM m_allItem;
// Dialog Data
	//{{AFX_DATA(CRecordDialog)
	enum { IDD = IDD_DIALOG_RECORD };
	CTreeCtrl	m_ctrlRecordTree;
	CSortListCtrl	m_ctrlRecordList;
	//}}AFX_DATA


// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CRecordDialog)
	protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
	virtual LRESULT DefWindowProc(UINT message, WPARAM wParam, LPARAM lParam);
	//}}AFX_VIRTUAL

// Implementation
protected:

	// Generated message map functions
	//{{AFX_MSG(CRecordDialog)
	virtual BOOL OnInitDialog();
	afx_msg void OnRclickListRecord(NMHDR* pNMHDR, LRESULT* pResult);
	afx_msg void OnRecordSave();
	afx_msg void OnRecordOther();
	afx_msg void OnRecordDial();
	afx_msg void OnRecordDel();
	afx_msg void OnRecordClear();
	afx_msg void OnRecordAll();
	afx_msg void OnClickListRecord(NMHDR* pNMHDR, LRESULT* pResult);
	afx_msg void OnDblclkListRecord(NMHDR* pNMHDR, LRESULT* pResult);
	afx_msg void OnButtonRecordDel();
	afx_msg void OnButtonRecordClear();
	afx_msg void OnRclickTreeRecord(NMHDR* pNMHDR, LRESULT* pResult);
	afx_msg void OnSelchangedTreeRecord(NMHDR* pNMHDR, LRESULT* pResult);
	afx_msg void OnItemchangedListRecord(NMHDR* pNMHDR, LRESULT* pResult);
	afx_msg void OnRecordSms();
	afx_msg void OnDelete();
	afx_msg void OnClear();
	afx_msg void OnSize(UINT nType, int cx, int cy);
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()
	LRESULT OnMyMove(WPARAM wParam,LPARAM lParam);
public:
	void ShowMonthListNetRecord();
	void ShowStatTimeListNetRecord();
	void SetButtonState(BOOL bState);
	
private:
//	CResizeCtrl m_resize;
	CSplitterControl m_wndSplitter1;

	void DoResize1(int delta);

	BOOL m_bFirst;
	BOOL m_bListFirst;
};

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_RECORDDIALOG_H__FEB96DA1_A8DC_464F_975C_A521B552E943__INCLUDED_)
