// InstallDriver.cpp : Defines the entry point for the console application.
//

#include "stdafx.h"
#include "InstallDriver.h"
#include "log.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif


bool Setup(const CString& strINFDir, const CString& strRemoveDrvProgram);
/////////////////////////////////////////////////////////////////////////////
// The one and only application object

CWinApp theApp;

using namespace std;

int _tmain(int argc, TCHAR* argv[], TCHAR* envp[])
{
	int nRetCode = 0;

	// initialize MFC and print and error on failure
	if (!AfxWinInit(::GetModuleHandle(NULL), NULL, ::GetCommandLine(), 0))
	{
		// TODO: change error code to suit your needs
		cerr << _T("Fatal Error: MFC initialization failed") << endl;
		nRetCode = 1;
	}
	else
	{
		CString Usage =	"Usage: \r\n"
						"InstallDriver.exe infDir removeDrvApp";
		if(argc !=3)
		{
			LOGOUT((LPCSTR)Usage);
			cout<<(LPCSTR)Usage<<endl;
			return 1;
		}
		//CString strINFDir = "G:\\GUAN-Work\\pcmciaapp\\Driver\\pcmcia_driver\\win2000_xp";
		CString strINFDir = argv[1];
		CString strRemove = argv[2];

		nRetCode = Setup(strINFDir, strRemove)? 0:2;
	}

	return nRetCode;
}


