#if !defined(AFX_SMSDIALOG_H__26EFEAE9_2DA2_4A33_9278_CD1D30C4DB44__INCLUDED_)
#define AFX_SMSDIALOG_H__26EFEAE9_2DA2_4A33_9278_CD1D30C4DB44__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000
// SMSDialog.h : header file
//
#include "BmpBkDialog.h"
#include "SortListCtrl.h"
#include "data\SMSPC.h"
#include "data\SMSEmbed.h"
#include "ResizeCtrl.h"
#include "SplitterControl.h"

/////////////////////////////////////////////////////////////////////////////
// CSMSDialog dialog

#define SMS_PC			1
#define SMS_PC_RECV		2
#define SMS_PC_SEND     	3
#define SMS_PC_UNSEND   4
#define SMS_PC_DRAFT	5
#define SMS_PC_RECYCLE	6

#define SMS_SIM			7
#define SMS_SIM_RECV	8
#define SMS_SIM_SEND	9
#define SMS_SIM_UNSEND	10



#define SMS_IMAG_PC			4
#define SMS_IMAG_UNREAD		15
#define SMS_IMAG_RECV			12
#define SMS_IMAG_SEND			13
#define SMS_IMAG_UNSEND		11
#define SMS_IMAG_DRAFT			16
#define SMS_IMAG_RECYCLE		14
#define SMS_IMAG_SIM			5
#define SMS_IMAG_NEWSMS		15
class CSMSDialog : public CBmpBkDialog
{
// Construction
public:
	DWORD m_currTreeItem;
	HTREEITEM m_currItem;
	HTREEITEM m_smItem;
	HTREEITEM m_pcItem;
	HTREEITEM m_recItem;
	HTREEITEM m_unsendItem;
	HTREEITEM m_sendItem;
	HTREEITEM m_darftItem;
	HTREEITEM m_recycleItem;
	void InitUI();
	CSMSDialog(CWnd* pParent = NULL);   // standard constructor
	~CSMSDialog();
	int m_nCurSelItem;
	CImageList*  g_pSysImageList;
	CImageList*  g_pSysList;
	void ShowSMSForSIM(int iType);
	void ShowSMSForPC(int iType);
//	void DeleteAll(std::vector<int>& items ,CSMS& smsList);
	void InsertNewSIMSms(TSMSItem item);
	void InsertUnsendSms(TSMSItem item);
	void InsertSendSms(TSMSItem item);
	void ClickTreeShowText(int iBox);
	void InitTree();
	void ShowSMSContent(int selItem);
	void OnSmsNewmsg() ;
	void SetDelBtnState(bool bGray);
	void SetReBtnState(bool bGray);
	void SetTraBtnState(bool bGray);
// Dialog Data
	//{{AFX_DATA(CSMSDialog)
	enum { IDD = IDD_DIALOG_SMS };
	CColorBtn	m_TransmitBtn;
	CColorBtn	m_ReplyBtn;
	CColorBtn	m_DeleteBtn;
	CColorBtn	m_NewBtn;
	CEdit	m_ctrlSmsEdit;
	CSortListCtrl	m_smsList;
	CTreeCtrl	m_smsTree;
	CString	m_editSmsShow;
	//}}AFX_DATA


// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CSMSDialog)
	protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
	virtual LRESULT DefWindowProc(UINT message, WPARAM wParam, LPARAM lParam);
	//}}AFX_VIRTUAL

// Implementation
protected:

	// Generated message map functions
	//{{AFX_MSG(CSMSDialog)
	virtual BOOL OnInitDialog();
	afx_msg void OnClickListSms(NMHDR* pNMHDR, LRESULT* pResult);
	afx_msg void OnDblclkListSms(NMHDR* pNMHDR, LRESULT* pResult);
	afx_msg void OnRclickListSms(NMHDR* pNMHDR, LRESULT* pResult);
	afx_msg void OnButtonNewsms();
	afx_msg void OnItemchangedListSms(NMHDR* pNMHDR, LRESULT* pResult);
	afx_msg void OnDblclkTreeSms(NMHDR* pNMHDR, LRESULT* pResult);
	afx_msg void OnRclickTreeSms(NMHDR* pNMHDR, LRESULT* pResult);
	afx_msg void OnSelchangedTreeSms(NMHDR* pNMHDR, LRESULT* pResult);
	afx_msg void OnButtonRevert();
	afx_msg void OnButtonTransmit();
	afx_msg void OnButtonDel();
	afx_msg void OnButtonClear();
	afx_msg void OnSmsSave();
	afx_msg void OnSmsRevert();
	afx_msg void OnSmsOther();
	afx_msg void OnSmsDel();
	afx_msg void OnSmsDail();
	afx_msg void OnSmsChang();
	afx_msg void OnSmsAll();
	afx_msg void OnSmsRecv();
	afx_msg void OnSmsRecvall();
	afx_msg void OnSmsCopy();
	afx_msg void OnClickTreeSms(NMHDR* pNMHDR, LRESULT* pResult);
	afx_msg LONG OnSMSListNotify(WPARAM wParam, LPARAM lParam);
	afx_msg void OnSize(UINT nType, int cx, int cy);
	afx_msg void OnKeydownListSms(NMHDR* pNMHDR, LRESULT* pResult);
	afx_msg HBRUSH OnCtlColor(CDC* pDC, CWnd* pWnd, UINT nCtlColor);
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()

	LRESULT OnMyMove(WPARAM wParam,LPARAM lParam);	
public:
	CSMSPC m_PCRecv;			//PC收件箱
	CSMSPC m_PCSend;			//PC已发送
	CSMSPC m_PCUnSend;			//PC未发送
	CSMSPC m_PCDraft;			//PC草稿箱
	CSMSPC m_PCRecyle;			//PC回收站

	CSMSEmbed m_EmbedRecv;		//嵌入端收件箱
	CSMSEmbed m_EmbedSend;		//嵌入端已发送
	CSMSEmbed m_EmbedDraft;		//嵌入端草稿箱
	
private:
	void SendSMS(CString strContent,const CString& strNum);

private:
//	CResizeCtrl m_resize;
	CSplitterControl m_wndSplitter1;
	CSplitterControl m_wndSplitter2;

	void DoResize1(int delta);
	void DoResize2(int delta);

	BOOL m_bFirst;
	BOOL m_b2First;

	BOOL m_bListFirst;
	BOOL m_bEditFirst;
	
};

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_SMSDIALOG_H__26EFEAE9_2DA2_4A33_9278_CD1D30C4DB44__INCLUDED_)
