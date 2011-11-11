// AutoRun.cpp : Defines the entry point for the console application.
//

#include "stdafx.h"
#include "AutoRun.h"
#include <fstream>
#include <atlbase.h>

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

/////////////////////////////////////////////////////////////////////////////
// The one and only application object

CWinApp theApp;

using namespace std;

int _tmain(int argc, TCHAR* argv[], TCHAR* envp[])
{
	int nRetCode = 0;
	bool bSetup = false;

	// initialize MFC and print and error on failure
	if (!AfxWinInit(::GetModuleHandle(NULL), NULL, ::GetCommandLine(), 0))
	{
		// TODO: change error code to suit your needs
		cerr << _T("Fatal Error: MFC initialization failed") << endl;
		nRetCode = 1;
	}
	else
	{
		// TODO: code your application's behavior here.
		//检测是否已经有实例在运行

		HANDLE AppLock = CreateSemaphore(
			NULL, 
			1, //仅仅允许一个实例运行
			1,
			"{171214C8-079E-4622-9315-44C0364C6ABA}"//用一个GUID来标志本程序
			);
		if(!AppLock)return nRetCode;
		if(WAIT_TIMEOUT == WaitForSingleObject(AppLock,0))
		{
			CloseHandle(AppLock);
			AppLock = NULL;
			return nRetCode;
		}

		CString strDir;
		CString strProcessName;
		if(IsSetup(strDir))
		{
			bSetup = true;
			strProcessName = strDir + "\\WCDMA.exe";
		}
		else
		{
			strProcessName = GetAppPath() + "Setup.exe";
		}

		StartProcess(strProcessName);

		if(AppLock)
		{
			ReleaseSemaphore(AppLock,1,NULL);
			CloseHandle(AppLock);
			AppLock=NULL;
		}
	}

	if(bSetup)
		ChgAutoRegedit();

	return nRetCode;
}

void ChgAutoRegedit()
{
	CRegKey reg;
	
	if(reg.Open(HKEY_LOCAL_MACHINE,"Software\\WCDMA Modem\\WCDMA")!=ERROR_SUCCESS)
	{
		reg.Create(HKEY_LOCAL_MACHINE,"Software\\WCDMA Modem\\WCDMA");		
		if(reg.Open(HKEY_LOCAL_MACHINE,"Software\\WCDMA Modem\\WCDMA") != ERROR_SUCCESS)
		{
			return ;
		}
	}
	
	reg.SetValue(1,TEXT("bAutoRun"));

	reg.Close();
}
CString GetLanguage(CString strDir)
{
	CString strPath = strDir + "\\D-dur.dat";
	
	HANDLE hFile = ::CreateFile(strPath,GENERIC_READ,NULL,NULL,OPEN_EXISTING,FILE_ATTRIBUTE_NORMAL,NULL);
	if(hFile == INVALID_HANDLE_VALUE)
	{
		if( GetLastError() == ERROR_SHARING_VIOLATION)
		{
			Sleep(200);
			hFile = ::CreateFile(strPath,GENERIC_READ,NULL,NULL,OPEN_EXISTING,FILE_ATTRIBUTE_NORMAL,NULL);
			if(hFile == INVALID_HANDLE_VALUE)
			{
				if( GetLastError() == ERROR_SHARING_VIOLATION)
				{
					Sleep(200);
					hFile = ::CreateFile(strPath,GENERIC_READ,NULL,NULL,OPEN_EXISTING,FILE_ATTRIBUTE_NORMAL,NULL);
				}
			}	
		}		
	}

	char buf[100]= {0};
	DWORD dwRead = 0;
	ReadFile(hFile,buf,100,&dwRead,NULL);

	CString str = buf;
	CString strLanguage = str.Right(3);

	CloseHandle(hFile);

	return strLanguage;
}

CStringTable& CStringTable::Instance(CString strDir)
{
	static CStringTable s_Instance(strDir);

	return s_Instance;
}
CStringTable::~CStringTable()
{
}
bool CStringTable::GetString(UINT id, CString& str)const
{
	const_iterator it = m_Map.find(id);
	if(it != m_Map.end())
	{
		str = it->second;
		return true;
	}
	
	return false;
}
CStringTable::CStringTable(CString strDir)
{
	
	Load(strDir);
}



void CStringTable::Load(CString strDir)
{
	CString strLanguage = GetLanguage(strDir);
	CString sFile = _T("");
	
	if(strLanguage == "-Ru")
	{
		sFile = strDir+"\\resource\\resource_ru_usb.txt";
	}
	else if(strLanguage == "-En")
	{
		sFile = strDir+"\\resource\\resource_en_usb.txt";	
	}

	
	std::ifstream file(sFile);
	if(!file.is_open())return;

	char buf[1024];
	while(!file.eof())
	{
		memset(buf,0, sizeof(buf));
		file.getline(buf,sizeof(buf)-1);
		CString str = buf;
		if(!str.GetLength())continue;
		int nFind = str.Find(";;;");//使用;;;作为ID和字符串的分割标志
		if(nFind == -1)continue;
		CString strNum = str.Left(nFind);
		if(!strNum.GetLength())continue;
		UINT nID = atoi(strNum);
		CString strV = str.Right(str.GetLength()-nFind-3);
		nFind = strV.Find('\r');
		if(nFind!=-1)strV.Delete(nFind);
		if(!strV.GetLength())continue;
		strV.Replace("\\n","\n");
		m_Map[nID] = strV;
	}
}

CString LoadStringEx(CString strDir,UINT id)
{
	CString str;
	if(!CStringTable::Instance(strDir).GetString(id, str))
	{
		str.Format("%d: Message not defined", id);
	}
	return str;
}

bool IsSetup(CString& strDir)
{
	CRegKey key;

	if(key.Open(HKEY_LOCAL_MACHINE,"Software\\WCDMA Modem\\WCDMA")!=ERROR_SUCCESS)
	{
		return false;
	}
	
	char szValue[1024] = {0};
	DWORD nCount = sizeof(szValue)-1;

	if(key.QueryValue(szValue, "Uninstall", & nCount) != ERROR_SUCCESS)
	{
		return false;
	}
	CString str = szValue;

	int nFind = -1;
	nFind = str.ReverseFind('\\');
	if( nFind < 0)
	{
		return false;
	}
	strDir = str.Left(nFind);
	CString strVersion = LoadStringEx(strDir,IDS_APPNAME);

	if(strVersion.Find("WCDMA") < 0)
	{
		return false;
	}
	return true;
}

HANDLE RunProgram(const CString& strCmdLine, bool bShowWindow)
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

bool StartProcess(const CString& strProcessName)
{
	CString strCmd = strProcessName;
	HANDLE hProcess = RunProgram(strCmd, false);
	if(!hProcess)
		return false;
	//WaitForSingleObject(hProcess, INFINITE);
	CloseHandle(hProcess);

	return true;
}

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
