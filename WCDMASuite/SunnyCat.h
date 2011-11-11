// SunnyCat.h : main header file for the SUNNYCAT application
//

#if !defined(AFX_SUNNYCAT_H__20D5E726_086E_491D_929B_F7F569C4B52F__INCLUDED_)
#define AFX_SUNNYCAT_H__20D5E726_086E_491D_929B_F7F569C4B52F__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

#ifndef __AFXWIN_H__
	#error include 'stdafx.h' before including this file for PCH
#endif

#include "resource.h"		// main symbols
#include "GlobalDataCenter.h"

/////////////////////////////////////////////////////////////////////////////
// CSunnyCatApp:
// See SunnyCat.cpp for the implementation of this class
//
#define USE_RESOURCEDLL
//#define CN_SUNNYCAT
class CSunnyCatApp : public CWinApp
{
public:
	CSunnyCatApp();

// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CSunnyCatApp)
	public:
	virtual BOOL InitInstance();
	virtual int ExitInstance();
	//}}AFX_VIRTUAL

// Implementation

	//{{AFX_MSG(CSunnyCatApp)
		// NOTE - the ClassWizard will add and remove member functions here.
		//    DO NOT EDIT what you see in these blocks of generated code !
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()
private:
	
#ifdef USE_RESOURCEDLL
	HINSTANCE m_hResource;//资源dll instance handle
#endif
	HANDLE m_AppLock;//唯一应用实例锁
};


/////////////////////////////////////////////////////////////////////////////

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_SUNNYCAT_H__20D5E726_086E_491D_929B_F7F569C4B52F__INCLUDED_)
