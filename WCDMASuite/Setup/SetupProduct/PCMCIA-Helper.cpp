#include "StdAfx.h"
#include "PCMCIA-Helper.h"
#include <fstream>
#include <vector>
#include "WndFuncs.h"

using namespace  std;

bool SystemIsWin98()
{
	OSVERSIONINFO version;
	version.dwOSVersionInfoSize = sizeof(version);
	GetVersionEx(&version);

	return (version.dwMajorVersion == 4 && (version.dwMinorVersion > 0 && version.dwMinorVersion < 90)) ;
}

bool SystemIsWinME()
{
	OSVERSIONINFO version;
	version.dwOSVersionInfoSize = sizeof(version);
	GetVersionEx(&version);

	return (version.dwMajorVersion == 4 && version.dwMinorVersion == 90) ;
}


bool SystemIsWin2K()
{
	OSVERSIONINFO version;
	version.dwOSVersionInfoSize = sizeof(version);
	GetVersionEx(&version);

	return version.dwMajorVersion == 5 && version.dwMinorVersion==0;
}

bool SystemIsWinXP()
{
	OSVERSIONINFO version;
	version.dwOSVersionInfoSize = sizeof(version);
	GetVersionEx(&version);

	return version.dwMajorVersion == 5 && version.dwMinorVersion>0;
}

bool SystemIsWinVista()
{
	OSVERSIONINFO version;
	version.dwOSVersionInfoSize = sizeof(version);
	GetVersionEx(&version);

	return version.dwMajorVersion > 5 ;
}

HWND GetWizardWndXP();
HWND GetWizardWnd2K();
HWND GetWizardWndVista();


void CWizardMonitor::Go()
{
	typedef HWND (*FUNC_GetWizardWnd)();

	FUNC_GetWizardWnd GetWizardWnd = NULL;
	if(SystemIsWin2K())GetWizardWnd = GetWizardWnd2K;
	if(SystemIsWinXP())GetWizardWnd = GetWizardWndXP;
	if(SystemIsWinVista())GetWizardWnd = GetWizardWndVista;
	if(GetWizardWnd)
	{
		HWND hWizard = GetWizardWnd();
		if(!hWizard)return;
		
		if(!SystemIsWinVista())
		{
			CPropertySheet* pWizard = (CPropertySheet*)CWnd::FromHandle(hWizard);
			pWizard->PressButton(PSBTN_CANCEL);
		}
		
		else
		{
			vector<HWND> childWnds;
			CWnd* pWnd = CWnd::FromHandle(hWizard);
			GetChildWndByClassName(pWnd, "Button",childWnds);
			while(::IsWindow(pWnd->m_hWnd))
			{
				
				ClickWindow(childWnds[3]);
				
				::Sleep(100);
			}
		}

		
	}
}


CString CreateTempInf(const CString& strDstPath, const CString& strHWID)
{
	CString strOld = strDstPath+"\\driver\\oxser.inf";
	CString strNew = strDstPath+"\\driver\\oxser1.inf";

	ifstream in(strOld);
	ofstream out(strNew);
	if(!in.is_open() || !out.is_open())return "";
	
	char buf[1024] = {0};
	while(!in.eof())
	{
		in.getline(buf,sizeof(buf)-1);
		CString str = buf;
		if(str.Find("PCMCIA\\CF_CARD-GENERIC-A569")!=-1)
		{
			str.Replace("PCMCIA\\CF_CARD-GENERIC-A569", strHWID);
		}
		out<<(LPCSTR)str<<endl;
	}

	return strNew;
}
void DeleteTempInf(const CString& strDstPath)
{
	DeleteFile(strDstPath+"\\driver\\oxser1.inf");
}