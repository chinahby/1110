#if !defined(AFX_GENMSGBOX_H__83B55156_8D7B_4D22_8BC7_8F1221349B10__INCLUDED_)
#define AFX_GENMSGBOX_H__83B55156_8D7B_4D22_8BC7_8F1221349B10__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000
// GenMsgBox.h : header file
//

#include "BmpBkDialog.h"
#include "resource.h"
/////////////////////////////////////////////////////////////////////////////
// CGenMsgBox dialog

#define IDYESTOALL 10
#define IDNOTOALL  11
#define MB_YESNOALL			0x00000006L
#define MB_YESNOALLCANCEL   0x00000007L
		
class CGenMsgBox : public CBmpBkDialog
{
// Construction
public:
	void  AdjustWindow();
	HICON LoadMsgIcon();
	CGenMsgBox(CWnd* pParent = NULL);   // standard constructor
	DWORD   m_dwProperty ;
	CString m_strTitle   ;
	CStatic	m_staTitle;
	CColorBtn m_arrBtns[5] ;
	DWORD   m_dwDefBtnID ;
	int		m_nBtnCount ;
//	CString strTitle  ;
	
// Dialog Data
	//{{AFX_DATA(CGenMsgBox)
	enum { IDD = IDD_DIALOG_MSGBOX };
	CEdit	m_EditTitle;
	//}}AFX_DATA


// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CGenMsgBox)
	public:
	virtual BOOL PreTranslateMessage(MSG* pMsg);
	protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
	virtual BOOL OnCommand(WPARAM wParam, LPARAM lParam);
	//}}AFX_VIRTUAL

// Implementation
protected:

	// Generated message map functions
	//{{AFX_MSG(CGenMsgBox)
	afx_msg void OnPaint();
	virtual BOOL OnInitDialog();
	afx_msg void OnCloseMsgBox(UINT nBtnID);
	afx_msg void OnShowWindow(BOOL bShow, UINT nStatus);
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()
};

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_GENMSGBOX_H__83B55156_8D7B_4D22_8BC7_8F1221349B10__INCLUDED_)
