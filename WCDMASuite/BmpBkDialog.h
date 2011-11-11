#if !defined(AFX_GENENHANCEDDIALOG_H__92B802CC_47D3_46E4_909B_8FBE9D28C574__INCLUDED_)
#define AFX_GENENHANCEDDIALOG_H__92B802CC_47D3_46E4_909B_8FBE9D28C574__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000
// BmpBkDialog.h : header file
//

/////////////////////////////////////////////////////////////////////////////
// CBmpBkDialog dialog
#include "AfxTempl.h"
#include "ColorBtn.h"
#include "OwnDrawComboBox.h"



class CBmpBkDialog : public CDialog
{
// Construction
public:
	bool IsEditCtrl(CWnd *pWnd) ;
//	void DeleteOwnDrawCombox(int nCombID);
	void RedrawEditFrame() ;
	void ShowBmpInrect(CDC *pDC);
	void AutoShowEditSrcoll(CEdit *pEdit) ;
	void RemoveColorBtn(DWORD dwBtnID);
	CColorBtn *GetColorBtn(DWORD dwBtnID) ;
//	COwnDrawComboBox *GetOwndrawCmb(DWORD dwCmbID) ;
	CColorBtn *SetColorBtnBmp(DWORD dwBtnIDs,TCHAR *pBmpName);
	void SetColorButtonsInfo(DWORD *lpBtnIDs,TCHAR **lpBmpNames,int nBtnsCount);
	void SetBackgroundBmp(LPCTSTR lpBmpName);
	CBmpBkDialog(UINT uResource,CWnd* pParent = NULL);   // standard constructor
	virtual ~CBmpBkDialog() ;
	CColorBtn m_btnClose;
	int		  m_nBmbWidth   ;
	int		  m_nBmbHeight  ;
	CString   m_strBkbmpName;
	CString   m_strTitlebmpName;
	CString   m_strCaption ;
	CWnd      *m_pUpdateEdit ;
	SCROLLINFO m_editScr ;
	CArray<CWnd *,CWnd *>m_arrEditIDs ;
	CArray<CListCtrl *,CListCtrl *>m_arrListCtrl ;
	CArray<CTreeCtrl *,CTreeCtrl *>m_arrTreeCtrl ;
	CArray<CColorBtn *,CColorBtn *>m_arrClrBtns  ;
//	CArray<COwnDrawComboBox *,COwnDrawComboBox *>m_arrOwnDrawCmb  ;
	void SetSubDlg(CWnd *pSubWnd=NULL) ;
	CWnd *m_pSubWnd  ;
	bool  m_bCreateExitBtn ;
	bool  m_bDrawToolBar ;
	bool  m_bEnableEnKey ;
	bool  m_bDrawRightFrm ;
	bool  m_bLButtonDown ;

	CRect m_rtButtExit;	//关闭按钮位置
	CRect m_rtButtMax ;	//最大化按钮位置
	CRect m_rtButtMin ;	//最小化按钮位置
	//CRect m_rtButtHelp;	//帮助按钮位置
	//CRect m_rtIcon	  ; //图标位置
	static bool   m_bShiftDown ;
	static bool   m_bCtrlDown  ; 
	static bool   m_bAltDown   ; 
	bool   m_bIsMainFuncDlg  ; 
	bool   m_bDrawEditFrm  ; 

private:
	CBrush m_Bkbr      ;
	CBrush m_Editbr    ;
	UINT   m_nDlgID    ;


// Dialog Data
	//{{AFX_DATA(CBmpBkDialog)
		// NOTE: the ClassWizard will add data members here
	//}}AFX_DATA


// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CBmpBkDialog)
public:
	virtual BOOL Create(CWnd* pParentWnd, UINT dlgID = 0);
	virtual BOOL PreTranslateMessage(MSG* pMsg);
	protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
	virtual BOOL OnCommand(WPARAM wParam, LPARAM lParam);
	//}}AFX_VIRTUAL

// Implementation
public:

	// Generated message map functions
	//{{AFX_MSG(CBmpBkDialog)
	afx_msg HBRUSH OnCtlColor(CDC* pDC, CWnd* pWnd, UINT nCtlColor);
	virtual BOOL OnInitDialog()    ;
	afx_msg void OnButtonCloseDlg();
	afx_msg void OnPaint();
	afx_msg void OnLButtonDown(UINT nFlags, CPoint point);
	afx_msg void OnClose();
	afx_msg void OnSize(UINT nType, int cx, int cy);
	afx_msg void OnDestroy();
	afx_msg BOOL OnHelpInfo(HELPINFO* pHelpInfo);
	afx_msg void OnTimer(UINT nIDEvent);
	afx_msg void OnShowWindow(BOOL bShow, UINT nStatus);
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()
};

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_GENENHANCEDDIALOG_H__92B802CC_47D3_46E4_909B_8FBE9D28C574__INCLUDED_)
