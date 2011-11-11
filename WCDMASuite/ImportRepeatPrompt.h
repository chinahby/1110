#if !defined(AFX_IMPORTREPEATPROMPT_H__7EEF17E5_31A6_46D7_8626_CC9EB169D5B9__INCLUDED_)
#define AFX_IMPORTREPEATPROMPT_H__7EEF17E5_31A6_46D7_8626_CC9EB169D5B9__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000
// ImportRepeatPrompt.h : header file
//

/////////////////////////////////////////////////////////////////////////////
// CImportRepeatPrompt dialog

class CImportRepeatPrompt : public CBmpBkDialog
{
// Construction
public:
	CImportRepeatPrompt(CWnd* pParent = NULL);   // standard constructor
	CImportRepeatPrompt(CWnd* pParent,CString strRepeatInfo);

// Dialog Data
	//{{AFX_DATA(CImportRepeatPrompt)
	enum { IDD = IDD_DIALOG_IMPORT_REPEAT };
	BOOL	m_app2All;
	//}}AFX_DATA
	CString strPromptInfo;


// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CImportRepeatPrompt)
	protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
	//}}AFX_VIRTUAL

// Implementation
protected:

	// Generated message map functions
	//{{AFX_MSG(CImportRepeatPrompt)
	virtual BOOL OnInitDialog();
	virtual void OnCancel();
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()
};

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_IMPORTREPEATPROMPT_H__7EEF17E5_31A6_46D7_8626_CC9EB169D5B9__INCLUDED_)
