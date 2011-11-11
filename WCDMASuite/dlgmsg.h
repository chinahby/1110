#ifndef AFX_MSGWND_H__1DC7047D_8675_4C80_B335_54F78F3BBD76__INCLUDED_
#define AFX_MSGWND_H__1DC7047D_8675_4C80_B335_54F78F3BBD76__INCLUDED_

/////////////////////////////////////////////////////////////////////////////
// CDlgMsg window

class CDlgMsg : public CWnd
{
// Construction
private://禁止直接构造，必须调用CreateMsgDlg()来创建
	CDlgMsg();

private://不支持
	CDlgMsg(const CDlgMsg&);
	CDlgMsg& operator=(const CDlgMsg&);

protected:
    CBitmap m_Bitmap;
	BOOL m_bFlag;
	
	CString m_strMessage;
	CString m_strCaption;
	
// Operations
public:

// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CDlgMsg)
	//}}AFX_VIRTUAL

// Implementation
public:	

	virtual ~CDlgMsg();
    void CreateMsgWindow();
	void SetPromptMessage(LPCTSTR lpszMsg);
	void SetPromptCaption(LPCTSTR lpszCaption);
	static CDlgMsg* CreateMsgDlg(CFont* pFont);
	// Generated message map functions
protected:
	//{{AFX_MSG(CDlgMsg)
	afx_msg void OnPaint();
	afx_msg void OnTimer(UINT nIDEvent);
	afx_msg void OnKillFocus(CWnd* pNewWnd);
	afx_msg void OnClose();
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()
};

/////////////////////////////////////////////////////////////////////////////

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

#endif
