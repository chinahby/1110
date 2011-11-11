#if !defined(AFX_CALLDIALOG_H__581E4297_6371_413A_87AC_B8295B63A41A__INCLUDED_)
#define AFX_CALLDIALOG_H__581E4297_6371_413A_87AC_B8295B63A41A__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000
// CallDialog.h : header file
//
#include "BmpBkDialog.h"
#include "ColorBtn.h"
#include "BitmapEdit.h"
#include "SpSilderCtrl.h"
#include "RecordDialog.h"
#include "CallManager.h"
#include "ResizeCtrl.h"
#include "label.h"

#define LONG_CLEAR_TIME				1
#define HALF_XPOS					24
/////////////////////////////////////////////////////////////////////////////
// CCallDialog dialog

class CCallDialog : public CBmpBkDialog
{
// Construction
public:
	CString m_callNumber;
	void Call(CString number);

	void SetParenWnd(CWnd * pParent);
	void ShowCallInImag(int id);
	UINT SecSendProc(LPVOID pParam);
	void PlayImagThread();
	
	CCallDialog(CWnd* pParent = NULL);   // standard constructor
	CBitmapEdit m_editDial;
	CCallManager  m_callmanger;
	
	CWnd* m_pParent;

	
// Dialog Data
	//{{AFX_DATA(CCallDialog)
	enum { IDD = IDD_DIALOG_CALL };
	CStatic	m_ctrlCall6;
	CStatic	m_ctrlCall5;
	CStatic	m_ctrlCall4;
	CStatic	m_ctrlCall3;
	CStatic	m_ctrlCall2;
	CStatic	m_ctrlCall1;
	CStatic	m_ctrlCall0;
	CSpSilderCtrl	m_ctrlSound;
	CColorBtn	m_ctrlDail;
	CColorBtn	m_ctrlHangup;
	CColorBtn	m_ctrlClear;
	CColorBtn	m_ctrl9;
	CColorBtn	m_ctrl8;
	CColorBtn	m_ctrl7;
	CColorBtn	m_ctrl6;
	CColorBtn	m_ctrl5;
	CColorBtn	m_ctrl4;
	CColorBtn	m_ctrl3;
	CColorBtn	m_ctrl2;
	CColorBtn	m_ctrl11;
	CColorBtn	m_ctrl10;
	CColorBtn	m_ctrl1;
	CColorBtn	m_ctrl0;
	CTreeCtrl	m_ctrlTree;
	CString	    m_strDialNum;
	CColorBtn m_12Btn;
	CColorBtn m_13Btn;
	CColorBtn m_14Btn;
	CColorBtn m_AddNumBtn;
	//}}AFX_DATA
	time_t	m_tBeginTime;	
	time_t	m_tEndTime;
	
public:
	void PressKey(TCHAR key);
// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CCallDialog)
	public:
	virtual BOOL DestroyWindow();
	protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
	virtual BOOL PreTranslateMessage(MSG* pMsg);
	//}}AFX_VIRTUAL

// Implementation
protected:

	// Generated message map functions
	//{{AFX_MSG(CCallDialog)
	virtual BOOL OnInitDialog();
	afx_msg void OnButtonDialNum();
	afx_msg void OnButtonDialClear();
	afx_msg void OnButton1();
	afx_msg void OnButton2();
	afx_msg void OnButton3();
	afx_msg void OnButton4();
	afx_msg void OnButton5();
	afx_msg void OnButton6();
	afx_msg void OnButton7();
	afx_msg void OnButton8();
	afx_msg void OnButton9();
	afx_msg void OnButton10();
	afx_msg void OnButton0();
	afx_msg void OnButton11();
	afx_msg void OnHScroll(UINT nSBCode, UINT nPos, CScrollBar* pScrollBar);
	afx_msg void OnKeyDown(UINT nChar, UINT nRepCnt, UINT nFlags);
	afx_msg void OnButtonHangup();
	afx_msg void OnOK();
	afx_msg void OnSize(UINT nType, int cx, int cy);
	afx_msg void OnChar(UINT nChar, UINT nRepCnt, UINT nFlags);
	afx_msg HBRUSH OnCtlColor(CDC* pDC, CWnd* pWnd, UINT nCtlColor);
	afx_msg void OnButtonAddnum();
	afx_msg void OnButton13();
	afx_msg void OnButton14();
	afx_msg void OnTimer(UINT nIDEvent);
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()
private:
//	CResizeCtrl m_resize;
	BOOL m_bFirst;
};

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.
	extern CCallDialog	 *pCallDialog ;
#endif // !defined(AFX_CALLDIALOG_H__581E4297_6371_413A_87AC_B8295B63A41A__INCLUDED_)
