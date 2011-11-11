// Execute.h : main header file for the EXECUTE application
//

#if !defined(AFX_EXECUTE_H__09E25455_D5AD_4CA8_8B1F_32505A44F61C__INCLUDED_)
#define AFX_EXECUTE_H__09E25455_D5AD_4CA8_8B1F_32505A44F61C__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

#ifndef __AFXWIN_H__
	#error include 'stdafx.h' before including this file for PCH
#endif

#include "resource.h"		// main symbols

/////////////////////////////////////////////////////////////////////////////
// CExecuteApp:
// See Execute.cpp for the implementation of this class
//

class CExecuteApp : public CWinApp
{
public:
	CExecuteApp();

// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CExecuteApp)
	public:
	virtual BOOL InitInstance();
	//}}AFX_VIRTUAL

// Implementation

	//{{AFX_MSG(CExecuteApp)
		// NOTE - the ClassWizard will add and remove member functions here.
		//    DO NOT EDIT what you see in these blocks of generated code !
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()
};


/////////////////////////////////////////////////////////////////////////////

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_EXECUTE_H__09E25455_D5AD_4CA8_8B1F_32505A44F61C__INCLUDED_)
