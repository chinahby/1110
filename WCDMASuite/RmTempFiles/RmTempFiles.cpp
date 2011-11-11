// RmTempFiles.cpp : Defines the entry point for the console application.
//

#include "stdafx.h"
#include "RmTempFiles.h"
#include "../porting.h"
#include <atlbase.h>
#include <Ras.h>


#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif


CString GetAppPath()
{
	char path_buffer[MAX_PATH];	
	char drive[_MAX_DRIVE];
	char dir[_MAX_DIR];

	GetModuleFileName(NULL, path_buffer, MAX_PATH);
	_splitpath(path_buffer, drive, dir, 0, 0 );

	CString sPath = drive;
	sPath += dir;

	return sPath;
}
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
		cerr << _T("Fatal Error: MFC initialization failed") << endl;
		nRetCode = 1;
	}
	else
	{
#if 0 
		DeleteFile(GetAppPath()+"log.txt");
		DeleteFile(GetAppPath()+"mobile_info");
		DeleteFile(GetAppPath()+"pcmcia.rec");
		

		TCHAR		szSysPath[MAX_PATH] = {0};
		CString		strEncryptedFilePath;

		if( (GetSystemDirectory(szSysPath, MAX_PATH)) != 0 ) 
		{
			strEncryptedFilePath += szSysPath;
			strEncryptedFilePath += '\\';
		}
		strEncryptedFilePath += GetEncryptFile();
		
		DeleteFile(strEncryptedFilePath);

		//保留记录?

		DeleteFile(GetAppPath()+"Phonebook.txt");
		DeleteFile(GetAppPath()+"SMSSend.txt");
		DeleteFile(GetAppPath()+"SMSRecv.txt");
		DeleteFile(GetAppPath()+"ConnectionRecord.dat");
		DeleteFile(GetAppPath()+"ConnectRecord.txt");
		DeleteFile(GetAppPath()+"callrecord.txt");
#endif
		DeleteFile(GetAppPath()+"log.txt");
		DeleteFile(GetAppPath()+"D-dur.dat");
//		RasDeleteEntry(NULL, GetConnectionName());//删除拨号连接
	}

	return nRetCode;
}


