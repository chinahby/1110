// ExecuteDlg.h : header file
//

#if !defined(AFX_EXECUTEDLG_H__ACF1A0F7_0DF3_4AA1_82A3_92CD6DCC8A61__INCLUDED_)
#define AFX_EXECUTEDLG_H__ACF1A0F7_0DF3_4AA1_82A3_92CD6DCC8A61__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000


#include "Thread.h"
#include "MyProgressBar.h"

extern CString g_strCom;

class CExecuteDlg : public CDialog, public CThread
{
// Construction
public:
	CExecuteDlg(CWnd* pParent = NULL);	// standard constructor

// Dialog Data
	//{{AFX_DATA(CExecuteDlg)
	enum { IDD = IDD_EXECUTE_DIALOG };
		// NOTE: the ClassWizard will add data members here
	//}}AFX_DATA

	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CExecuteDlg)
	protected:
	virtual void DoDataExchange(CDataExchange* pDX);	// DDX/DDV support
	//}}AFX_VIRTUAL

// Implementation
protected:
	HICON m_hIcon;

	// Generated message map functions
	//{{AFX_MSG(CExecuteDlg)
	virtual BOOL OnInitDialog();
	afx_msg void OnPaint();
	afx_msg HCURSOR OnQueryDragIcon();
	afx_msg LONG OnDeviveChange(WPARAM wParam, LPARAM lParam);
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()
private:
	virtual void Go();
	virtual void OnOK();
	virtual void OnCancel();

	bool SetPath(const CString& strInstPath);
	void xJudgeDeviceChangePath(DWORD dwData, CString &strPath) ;
	bool GetUsbDev(const CString& hwid);

private:
	CMyProgressBar m_wndProgressBar;

	bool m_IsInstall;
	CString m_strProduct;
	CString m_strInstallPath;
	CString m_strLanguage;
	CString m_strVolume;

};

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_EXECUTEDLG_H__ACF1A0F7_0DF3_4AA1_82A3_92CD6DCC8A61__INCLUDED_)
