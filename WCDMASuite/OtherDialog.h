#if !defined(AFX_OTHERDIALOG_H__D9B35020_5806_4F6B_BEFD_A0CCB0310008__INCLUDED_)
#define AFX_OTHERDIALOG_H__D9B35020_5806_4F6B_BEFD_A0CCB0310008__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000
// OtherDialog.h : header file
//
#include "BmpBkDialog.h"
#include "label.h"
enum EBtnStatus
{
	STATUS_CONNECT,
	STATUS_DISCONNECT,
	STATUS_CONNECT_EVDO,
	STATUS_DISCONNECT_EVDO
};
/////////////////////////////////////////////////////////////////////////////
// COtherDialog dialog

class COtherDialog : public CBmpBkDialog
{
// Construction
public:
	COtherDialog(CWnd* pParent = NULL);   // standard constructor

// Dialog Data
	//{{AFX_DATA(COtherDialog)
	enum { IDD = IDD_DIALOG_OTHER };
	CLabel	m_ApnLabel;
	COwnDrawComboBox	m_LinkCombo;
	CColorBtn	m_ConnectBtn;
	CColorBtn	m_ConnectBtnEvdo;
	//}}AFX_DATA


// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(COtherDialog)
	protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
	//}}AFX_VIRTUAL

// Implementation
protected:

	// Generated message map functions
	//{{AFX_MSG(COtherDialog)
	afx_msg HBRUSH OnCtlColor(CDC* pDC, CWnd* pWnd, UINT nCtlColor);
	afx_msg void OnSize(UINT nType, int cx, int cy);
	virtual BOOL OnInitDialog();
	afx_msg void OnButtonConnect();
	afx_msg void OnButtonConnectevdo();
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()
public:
	void SetBtnStatus(EBtnStatus status);
	void InitApn();
private:
	void Connect();
	void DisConnect();
	void SetRegValue();
private:
	EBtnStatus m_BtnStatus;
};

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_OTHERDIALOG_H__D9B35020_5806_4F6B_BEFD_A0CCB0310008__INCLUDED_)
