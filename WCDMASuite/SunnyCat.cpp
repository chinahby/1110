// SunnyCat.cpp : Defines the class behaviors for the application.
//

#include "stdafx.h"
#include "SunnyCat.h"
#include "SunnyCatDlg.h"
#include "Porting.h"
#include <vector>

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif


//#pragma  data_seg("Shared")
//int volatile g_lAppInstance =0;
//#pragma  data_seg()

//#pragma comment(linker,"/section:Shared,RWS")
/////////////////////////////////////////////////////////////////////////////
// CSunnyCatApp

BEGIN_MESSAGE_MAP(CSunnyCatApp, CWinApp)
	//{{AFX_MSG_MAP(CSunnyCatApp)
		// NOTE - the ClassWizard will add and remove mapping macros here.
		//    DO NOT EDIT what you see in these blocks of generated code!
	//}}AFX_MSG
	ON_COMMAND(ID_HELP, CWinApp::OnHelp)
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CSunnyCatApp construction

#ifdef USE_RESOURCEDLL
static CString GetLastErrorMessage()
{
	DWORD dwError = ::GetLastError();

	LPTSTR lpBuffer;
	BOOL bRet = FormatMessage(FORMAT_MESSAGE_ALLOCATE_BUFFER | FORMAT_MESSAGE_FROM_SYSTEM,
			                      NULL,  dwError, MAKELANGID(LANG_NEUTRAL, SUBLANG_SYS_DEFAULT),
			                      (LPTSTR) &lpBuffer, 0, NULL);

	CString sErr;
	if (bRet)
	{
		sErr = lpBuffer;

		LocalFree(lpBuffer);
	}

	return sErr;
}
#endif

CSunnyCatApp::CSunnyCatApp()
{
#ifdef USE_RESOURCEDLL
	m_hResource = NULL;
#endif
	m_AppLock = NULL;
	// TODO: add construction code here,
	// Place all significant initialization in InitInstance
}

/////////////////////////////////////////////////////////////////////////////
// The one and only CSunnyCatApp object

CSunnyCatApp theApp;

/////////////////////////////////////////////////////////////////////////////
// CSunnyCatApp initialization

BOOL CSunnyCatApp::InitInstance()
{
	//检测是否已经有实例在运行
	m_AppLock = CreateSemaphore(
		NULL, 
		1, //仅仅允许一个实例运行
		1,
		GetAppID()//用一个GUID来标志本程序
		);
	if(!m_AppLock)return FALSE;
	if(WAIT_TIMEOUT == WaitForSingleObject(m_AppLock,0))
	{
		CloseHandle(m_AppLock);
		m_AppLock = NULL;
		return false;
	}

	Data::OpenGlobalDB();

#ifdef USE_RESOURCEDLL
	//装载资源dll
	CString sLib = "ResLib.dll";
	m_hResource = ::LoadLibrary(sLib);
	if(m_hResource)
	{
		AfxSetResourceHandle(m_hResource);
	}
#endif

	AfxEnableControlContainer();


#ifdef _AFXDLL
	Enable3dControls();			// Call this when using MFC in a shared DLL
#else
	Enable3dControlsStatic();	// Call this when linking to MFC statically
#endif
	
//	//=============================================
//	if(++g_lAppInstance>1)
//	{
//		AfxMessageBox("该程序已经在运行!");
//		return FALSE;
//	}
	//==============================================
	// Standard initialization
	// If you are not using these features and wish to reduce the size
	//  of your final executable, you should remove from the following
	//  the specific initialization routines you do not need.



	//SetRegistryKey(_T("Local AppWizard-Generated Applications"));

	//LoadStdProfileSettings();  // Load standard INI file options (including MRU)

	CSunnyCatDlg dlg;
	m_pMainWnd = &dlg;
	dlg.DoModal();


	// Since the dialog has been closed, return FALSE so that we exit the
	//  application, rather than start the application's message pump.
	return TRUE;
}


int CSunnyCatApp::ExitInstance()
{
#ifdef USE_RESOURCEDLL
	if(m_hResource)
	{
		::FreeLibrary(m_hResource);
		m_hResource = NULL;
	}
#endif

	Data::CloseGlobalDB();
	
	if(m_AppLock)
	{
		ReleaseSemaphore(m_AppLock,1,NULL);
		CloseHandle(m_AppLock);
		m_AppLock=NULL;
	}
	return 0;
}
