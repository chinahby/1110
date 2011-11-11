#if !defined(AFX_DLGIPDNS_H__F3B29EAF_60D4_47CF_8E0C_F97BFB0CE084__INCLUDED_)
#define AFX_DLGIPDNS_H__F3B29EAF_60D4_47CF_8E0C_F97BFB0CE084__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000
// DlgIPDNS.h : header file
//
#include <string>
#include "BmpBkDialog.h"
/////////////////////////////////////////////////////////////////////////////
// CDlgIPDNS dialog

class CDlgIPDNS : public CBmpBkDialog
{
// Construction
public:
	CDlgIPDNS(CWnd* pParent = NULL);   // standard constructor
	CDlgIPDNS(CWnd* pParent,std::tstring strApn);
// Dialog Data
	//{{AFX_DATA(CDlgIPDNS)
	enum { IDD = IDD_DIALOG_IPDNS };
		// NOTE: the ClassWizard will add data members here
	//}}AFX_DATA


// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CDlgIPDNS)
	protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
	//}}AFX_VIRTUAL

// Implementation
protected:

	// Generated message map functions
	//{{AFX_MSG(CDlgIPDNS)
	virtual BOOL OnInitDialog();
	virtual void OnOK();
	afx_msg void OnRadioStaticdns();
	afx_msg void OnRadioAutodns();
	afx_msg void OnRadioAutoip();
	afx_msg void OnRadioStaticip();
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()
private:
	void InitCtrlValue();
	void SetAutoGetIP();
	void SetAutoGetDNS();
	void SetAutoIPCtrlState();
	void SetAutoDNSCtrlState();
	void SetStaticIPCtrlState();
	void SetStaticDNSCtrlState();
	void SetStaticGetIP(const CString strIP);
	void SetStaticGetDNS(const CString strDNS1,const CString strDNS2);
private:
	std::tstring m_strApn;
};

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_DLGIPDNS_H__F3B29EAF_60D4_47CF_8E0C_F97BFB0CE084__INCLUDED_)
