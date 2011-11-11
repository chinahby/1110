#if !defined(AFX_REGISTERCOPS_H__291405F0_D222_4570_AF8E_28266BC88406__INCLUDED_)
#define AFX_REGISTERCOPS_H__291405F0_D222_4570_AF8E_28266BC88406__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000
// RegisterCOPS.h : header file
//

/////////////////////////////////////////////////////////////////////////////
// CRegisterCOPS dialog

class CRegisterCOPS : public CBmpBkDialog
{
// Construction
public:
	CRegisterCOPS(CWnd* pParent = NULL);   // standard constructor

// Dialog Data
	//{{AFX_DATA(CRegisterCOPS)
	enum { IDD = IDD_DIALOG_COPSRE };
	CBrush m_brush;
	CFont m_font;
		// NOTE: the ClassWizard will add data members here
	//}}AFX_DATA


// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CRegisterCOPS)
	protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
	//}}AFX_VIRTUAL

// Implementation
protected:

	// Generated message map functions
	//{{AFX_MSG(CRegisterCOPS)
	afx_msg HBRUSH OnCtlColor(CDC* pDC, CWnd* pWnd, UINT nCtlColor);
	virtual BOOL OnInitDialog();
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()
};

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_REGISTERCOPS_H__291405F0_D222_4570_AF8E_28266BC88406__INCLUDED_)
