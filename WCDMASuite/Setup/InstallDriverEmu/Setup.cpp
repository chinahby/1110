#include "stdafx.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif


bool SetupXP(const CString& strINFDir);
bool Setup98(const CString& strINFDir);
bool Setup2K(const CString& strINFDir);
bool SetupMe(const CString& strINFDir);



bool SystemIsWin98();
bool SystemIsWinME();
bool SystemIsWin2K();
bool SystemIsWinXP();

static HANDLE RunProgram(const CString& strCmdLine, bool bShowWindow)
{
	STARTUPINFO startupInfo;
	PROCESS_INFORMATION processInfo;
	memset (&startupInfo, 0, sizeof(STARTUPINFO));
	memset (&processInfo, 0, sizeof(PROCESS_INFORMATION));
	startupInfo.cb = sizeof(STARTUPINFO);
	startupInfo.dwFlags = STARTF_USESHOWWINDOW;
	startupInfo.wShowWindow = bShowWindow? SW_MINIMIZE: SW_HIDE;

	char szCmdLine[256] ={0};
	memcpy(szCmdLine,strCmdLine, strCmdLine.GetLength());
	if(!CreateProcess (NULL, szCmdLine, NULL, NULL, FALSE, 
	   NULL, //use DETACHED_PROCESS if try to create a console process
	   NULL, 
	   NULL, &startupInfo, &processInfo))
	{
		return NULL;
	}
	CloseHandle(processInfo.hThread);

	return 	processInfo.hProcess;
}

bool Setup(const CString& strINFDir, const CString& strRemoveDrvProgram)
{
	//Remove the device first
	//HANDLE hProcessRemove = RunProgram(strRemoveDrvProgram, SW_HIDE);
	//if(hProcessRemove)
	//	WaitForSingleObject(hProcessRemove, INFINITE);

	//CloseHandle(hProcessRemove);

	bool bOK = false;
	if(SystemIsWin98())
	{
		bOK = Setup98(strINFDir);
	}
	else if(SystemIsWin2K())
	{
		bOK = Setup2K(strINFDir);
	}
	else if(SystemIsWinXP())
	{
		bOK = SetupXP(strINFDir);
	}
	else if(SystemIsWinME())
	{
		bOK = SetupMe(strINFDir);
	}

	return bOK;
}
