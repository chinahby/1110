#if !defined(AFX_ENFILEDIALOG_H__710E6A98_8540_4BFE_8920_4C82661EF370__INCLUDED_)
#define AFX_ENFILEDIALOG_H__710E6A98_8540_4BFE_8920_4C82661EF370__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000
// EnFileDialog.h : header file
//

/////////////////////////////////////////////////////////////////////////////
// CEnFileDialog dialog

	class CEnFileDialog : public CFileDialog
	{
		DECLARE_DYNAMIC(CEnFileDialog)
			
	public:
		CEnFileDialog(BOOL bOpenFileDialog, // TRUE for FileOpen, FALSE for FileSaveAs
			LPCTSTR lpszDefExt = NULL,
			LPCTSTR lpszFileName = NULL,
			DWORD dwFlags = OFN_HIDEREADONLY | OFN_OVERWRITEPROMPT,
			LPCTSTR lpszFilter = NULL,
			CWnd* pParentWnd = NULL);
		
	protected:
		//{{AFX_MSG(CEnFileDialog)
		afx_msg void OnClose();
		//}}AFX_MSG
		DECLARE_MESSAGE_MAP()
	};
	
	//{{AFX_INSERT_LOCATION}}
	// Microsoft Visual C++ will insert additional declarations immediately before the previous line.	

#endif // !defined(AFX_ENFILEDIALOG_H__710E6A98_8540_4BFE_8920_4C82661EF370__INCLUDED_)
