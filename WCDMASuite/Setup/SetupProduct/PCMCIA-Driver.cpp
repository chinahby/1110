#include "stdafx.h"
#include "Execute.h"
#include "log.h"
#include "DriverSetup.h"
#include "Thread.h"
#include "../../DevEnum.h"
#include "PCMCIA-Helper.h"
#include "SignatureMonitor.h"
#include "newdev.h"
#include "DLLModule.h"
#include "..\..\RASClient.h"
#include <fstream>

#include <setupapi.h>

HANDLE RunProgram(const CString& strCmdLine, bool bShowWindow);
CString GetAppPath();
CString GetWinDir();
int ShowMessageBox(UINT strid, UINT nType = MB_OK);
CString LoadStringEx(UINT id);
void LOG(const char* str);
bool DisablePowerManagementOn9X();

/*
static bool Install98CopyINF(const CString& strDestDir)
{
	//Copy drvier files to windows\system directory
	CString strSysDir = GetWinDir()+"system";
	CString strSrvDrv = strDestDir+"\\driver\\oxser.vxd";
	CString strDesDrv = strSysDir+"\\oxser.vxd";
	CopyFile(strSrvDrv, strDesDrv, false);
	strSrvDrv = strDestDir+"\\driver\\oxserui.dll";
	strDesDrv = strSysDir+"\\oxserui.dll";
	CopyFile(strSrvDrv, strDesDrv, false);
	strSrvDrv = strDestDir+"\\driver\\oxui.dll";
	strDesDrv = strSysDir+"\\oxui.dll";
	CopyFile(strSrvDrv, strDesDrv, false);

	//Copy CDMA.inf to windows\inf directory
	strSrvDrv = strDestDir+"\\driver\\CDMA.inf";
	strDesDrv = GetWinDir()+"INF\\CDMA.inf";
	CopyFile(strSrvDrv, strDesDrv, false);

	ShowMessageBox(IDS_INSERTDEVICE_AGAIN, MB_OK|MB_ICONINFORMATION);

	return true;
}

static bool Install98UserEmu(const CString& strDestDir)
{
	//Remove driver first
	CString strCmd = strDestDir+"\\driver\\FastRemove98Me.exe";
	HANDLE hProcess = RunProgram(strCmd, false);
	if(!hProcess)return false;
	WaitForSingleObject(hProcess, INFINITE);
	CloseHandle(hProcess);
	Sleep(300);

	//Instatll driver
	CString strSetupDriver = strDestDir+"\\InstallDriver.exe ";
	CString strINFDir = "\""; strINFDir += strDestDir+"\\driver\" ";
	CString strRemove = "\""; strRemove += strDestDir+"\\driver\\FastRemove98Me.exe\"";

	strCmd = strSetupDriver+strINFDir+strRemove;
	hProcess = RunProgram(strCmd, false);
	if(!hProcess)return false;

	Sleep(300);
	ShowMessageBox(IDS_INSERTDEVICE_AGAIN, MB_OK|MB_ICONINFORMATION);

	WaitForSingleObject(hProcess, INFINITE);
	CloseHandle(hProcess);
	Sleep(300);

	//Install modem

	OSVERSIONINFO version;
	version.dwOSVersionInfoSize = sizeof(version);
	GetVersionEx(&version);

	if(version.dwMajorVersion == 4 && version.dwMinorVersion == 90)
	{
		strCmd = strDestDir+"\\driver\\ModemInstMe.exe";
	}
	else
		strCmd = strDestDir+"\\driver\\ModemInst98.exe";
	hProcess = RunProgram(strCmd, false);
	if(!hProcess)return false;
	WaitForSingleObject(hProcess, INFINITE);
	CloseHandle(hProcess);

	return true;
}

/** 98下的安装
*/
/*
bool InstallPCMCIA_98(const CString& strDestDir)
{
	DisablePowerManagementOn9X();

	//return Install98CopyINF(strDestDir);

	return Install98UserEmu(strDestDir);
}

/** 98下的卸载
*/
/*
bool UninstallPCMCIA_98(const CString& strDestDir)
{
	HANDLE hProcess = RunProgram(strDestDir+"\\driver\\FastRemove98Me.exe", false);
	if(!hProcess)return false;

	WaitForSingleObject(hProcess, INFINITE);

	return true;
}

/**
*	通过 Copy INF 文件和设置注册表来安装
*/
/*
static bool Install2KXPCopyINF(const CString& strDestDir)
{
	CString strINF = strDestDir+"\\driver\\oxser.inf";
	bool bOK = SetupDriver(strINF) == SETUP_OK;
	
	ShowMessageBox(IDS_INSERTDEVICE_AGAIN, MB_OK|MB_ICONINFORMATION);

	return bOK;
}

/**
*	使用 SetupAPI 安装
*/
static bool Install2KXPSetupAPI(const CString& strDestDir)
{
	CString strINF = strDestDir+"\\driver\\oxser.inf";
	SetupDriver(strINF);

	CString strCmd = strDestDir+"\\driver\\InstallDrv.exe";
	HANDLE hProcess = RunProgram(strCmd, false);
	if(!hProcess || hProcess==INVALID_HANDLE_VALUE)return false;

	ShowMessageBox(IDS_INSERTDEVICE_AGAIN, MB_OK|MB_ICONINFORMATION);

	return true;
}

bool HardwareExisting(CString& strHWID);


static void RemoveDriverFiles(const CString& strDestDir)
{
	CString strDrvDir = strDestDir+"\\driver\\";
	CString strFiles[] = 
	{
		strDrvDir+"lenovo.inf",
		strDrvDir+"cdma.inf",
		strDrvDir+"cdma-1x.inf",
		strDrvDir+"oxser.inf",

		strDrvDir+"oxser.sys",
		strDrvDir+"oxser.vxd",
		strDrvDir+"oxserui.dll",
		strDrvDir+"oxui.dll",
	};

	for(int i=0; i<sizeof(strFiles)/sizeof(strFiles[0]); ++i)
		DeleteFile(strFiles[i]);
}

bool SystemIsWinXPSP1();
bool SystemIsWinXP();
bool InstallPCMCIASocketDriver(const CString& strINF);
DWORD GetFileSize(const CString& strFile);
CString GetSystemDir()
{
	char buf[1024] = {0};
	GetSystemDirectory(buf, sizeof(buf)-1);

	return buf;
}

//创建115200波特率的Modem inf 文件
void Create115200ModemInfFile(const CString& DestPath)
{
	CString modemInfoFile = DestPath+"\\driver\\1xmdm.inf";
	CString modemInfoNew = DestPath+"\\driver\\1xmdm-temp.inf";
	std::ifstream oldFile(modemInfoFile);
	std::ofstream newFile(modemInfoNew);
	
	while(!oldFile.eof())
	{
		char buf[1024] = {0};
		oldFile.getline(buf, sizeof(buf));
		CString str = buf;
		if(str.Find("%Gen2300%") != -1)
		{
			str.Replace("%Gen2300%", "NBAGame");
			str.Replace("Gen2300", "Gen1152");
			str.Replace("NBAGame", "%Gen2300%");
		}

		newFile<<(LPCSTR)str<<std::endl;
	}

	newFile.close();
	oldFile.close();

	DeleteFile(modemInfoFile);
	MoveFile(modemInfoNew, modemInfoFile);
}

typedef const CString& (*FUNC_GetHardwareID)(int i);

typedef BOOL (WINAPI *FUNC_UpdateDriverForPlugAndPlayDevices)
	(
	HWND hwndParent,
	LPCSTR HardwareId,
	LPCSTR FullInfPath,
	DWORD InstallFlags,
	PBOOL bRebootRequired
	);


class CThreadDetectHW: public CThread
{
public :
	bool m_DetectOK;
private:
	CString m_strHWID;
	CString m_strDstDir;
public:
	CThreadDetectHW(const CString& strDst)
		:m_DetectOK(false),
		m_strDstDir(strDst)
	{
	}
protected:
	virtual void Go()
	{
		m_DetectOK = true;	
		CDllModule dll(m_strDstDir+"porting.dll");
		FUNC_GetHardwareID GetHardwareID = (FUNC_GetHardwareID)dll.GetFuncAdd("?GetHardwareID@@YAABVCString@@H@Z");
		
		if(!GetHardwareID)
		{
			return ;
		}
		
		bool bFound = false;
		bool hasfailed = false;
		CString strHWID;
		DWORD dwTime = GetTickCount();	
		while(!bFound)
		{
			
			for(int i=0;true;++i)
			{
				strHWID  = GetHardwareID(i);
				
				if(strHWID.IsEmpty())break;
				if(HardwareExisting(strHWID))
				{
					bFound = true;
					break;
				}

				if(GetTickCount() - dwTime >120000)
				{
					hasfailed = true;
					break;//ShowMessageBox(IDS_INSERTDEVICE_AGAIN, MB_OK|MB_ICONINFORMATION);
				}

			}

			if(hasfailed)
			{
				break;
			}
			Sleep(200);
		}

		if(hasfailed)
		{
			ShowMessageBox(IDS_NOTINSERT_FAIL, MB_OK|MB_ICONINFORMATION);
			//m_DetectOK = false;
			return;			
		}

		m_strHWID = strHWID;
		m_DetectOK = SetupDriver();
	}

private:
	bool SetupDriver()
	{
		CSignatureMonitor sigMon;
		CWizardMonitor	wizMon;
		
		wizMon.Start();
		sigMon.Start();

		wizMon.WaitForComplete(15000);


		CString m_strDll("");
		char szBuf[255]={0};
		::GetSystemDirectory(szBuf,255);
		m_strDll = (CString)szBuf + "\\newdev.dll";

		CDllModule dll(m_strDll);

		FUNC_UpdateDriverForPlugAndPlayDevices UpdateDriver = (FUNC_UpdateDriverForPlugAndPlayDevices)dll.GetFuncAdd("UpdateDriverForPlugAndPlayDevicesA");
		if(!UpdateDriver)
		{
			return false;
		}
		
		CString strInf;
//////////////////////////////////////////////////////////////////////////
		OSVERSIONINFOEX osvi;
		ZeroMemory(&osvi, sizeof(OSVERSIONINFOEX));
		osvi.dwOSVersionInfoSize = sizeof(OSVERSIONINFOEX);

		GetVersionEx((OSVERSIONINFO *) &osvi);

		if ( osvi.dwMajorVersion <= 4 )// Windows NT 4.0 or below os
		{
			//no to do
			//printf("Microsoft Windows NT ");
			//strInf = m_strDstDir+"driver\\oxser.inf";
		}            

        if (( osvi.dwMajorVersion == 5) &&(osvi.dwMinorVersion == 0 ))
		{
			//printf ("Microsoft Windows 2000 ");
			strInf = m_strDstDir+"driver\\Windows_2000\\oxser.inf";
		}            

        if ( (osvi.dwMajorVersion == 5 )&& (osvi.dwMinorVersion == 1) )
		{
            //printf ("Microsoft Windows XP ");
			HMODULE hModuleXP;
			SYSTEM_INFO sysInfo;
			hModuleXP = ::LoadLibrary("kernel32.dll");
			typedef BOOL (WINAPI* pfnProc)(LPSYSTEM_INFO);
		//	FARPROC lpfnDLLProc;
			pfnProc lpfnDLLProc = NULL;
			lpfnDLLProc = (pfnProc)GetProcAddress(hModuleXP,"GetNativeSystemInfo");
			if(lpfnDLLProc != NULL)//is 64bit os
			{
				//lpfnDLLProc->GetNativeSystemInfo(&sysInfo);
				lpfnDLLProc(&sysInfo);
				if(sysInfo.dwProcessorType == PROCESSOR_ARCHITECTURE_IA64)
				{
					strInf = m_strDstDir+"driver\\Windows_XP-64\\oxser.inf";
				}
				else
				{
					strInf = m_strDstDir+"driver\\Windows_XP-32\\oxser.inf";
				}
			}
			else
			{
				strInf = m_strDstDir+"driver\\Windows_XP-32\\oxser.inf";
			}
			
		}
		if ( (osvi.dwMajorVersion == 5 )&& (osvi.dwMinorVersion == 2) )
		{
			//printf ("Microsoft Windows 2003 ");
			strInf = m_strDstDir+"driver\\Windows_XP-32\\oxser.inf";
		}   

		if ( osvi.dwMajorVersion > 5)
		{
            //printf ("Microsoft Windows XP ");
			HMODULE hModuleVista;
			SYSTEM_INFO sysInfo;
			hModuleVista = ::LoadLibrary("kernel32.dll");
			typedef BOOL (WINAPI* pfnProc)(LPSYSTEM_INFO);
		//	FARPROC lpfnDLLProc;
			pfnProc lpfnDLLProc = NULL;
			lpfnDLLProc = (pfnProc)GetProcAddress(hModuleVista,"GetNativeSystemInfo");
			if(lpfnDLLProc != NULL)//is 64bit os
			{
				lpfnDLLProc(&sysInfo);
				if(sysInfo.dwProcessorType == PROCESSOR_ARCHITECTURE_IA64)
				{
					strInf = m_strDstDir+"driver\\Windows_Vista-64\\oxser.inf";
				}
				else
				{
					strInf = m_strDstDir+"driver\\Windows_Vista-32\\oxser.inf";
				}
			}
			else
			{
				strInf = m_strDstDir+"driver\\Windows_Vista-32\\oxser.inf";
			}
			
		}
		
//////////////////////////////////////////////////////////////////////////

		//CString strInf = m_strDstDir+"driver\\oxser.inf";
		bool bOK = false;
		if(!UpdateDriver(NULL,m_strHWID,strInf,INSTALLFLAG_FORCE, 0))
		{
			bOK = false;
		}
		else
		{
			bOK = true;
		}

		DeleteTempInf(m_strDstDir);

		//Install modem
		if(bOK)//Success
		{
			CString strCmd = m_strDstDir+"MdmInst.exe";
			HANDLE hProcess = RunProgram(strCmd, false);
			if(!hProcess)return false;

			WaitForSingleObject(hProcess, INFINITE);
			CloseHandle(hProcess);
		}

		Sleep(3000);
		RemoveDriverFiles(m_strDstDir);
/*
		if(bOK && SystemIsWinXP() )//XP SP1需要更新PCMCIA driver
		{
			CString strNewDrv = m_strDstDir+"\\driver\\pcmcia.sys";
			CString strOldDrv = GetSystemDir()+"\\drivers\\pcmcia.sys";
			if(GetFileSize(strNewDrv) > GetFileSize(strOldDrv))
			{
				CString strPCMCIAINF = m_strDstDir+"\\driver\\pcmcia.inf";
				bOK = InstallPCMCIASocketDriver(strPCMCIAINF);
			}
		}
*/

		if(bOK)
		{
			CheckPortReady();
		}

		if(bOK)
		{
			bOK = CreateRASEntry();
		}

		return bOK;
	}

	//检查端口安装成功否
	void CheckPortReady()
	{
		CPortList ports;
		m_strHWID.MakeLower();
		for(size_t i=0; i<ports.Count(); ++i)
		{
			CString strID = ports.GetAt(i).strHardwareID;
			strID.MakeLower();
			if(strID.Find(m_strHWID)!=-1)//Hardware ID matched
			{
				CString strCOM = "\\\\.\\";
				strCOM += ports.GetAt(i).strCOM;

				HANDLE hCOM = CreateFile(strCOM, GENERIC_READ | GENERIC_WRITE,0,NULL, OPEN_EXISTING,0, NULL);
				if(hCOM != INVALID_HANDLE_VALUE)//the port is working now
				{
					CloseHandle(hCOM);
					ShowMessageBox(IDS_DRVIER_INSTALL_OK, MB_OK|MB_ICONINFORMATION);
				}
				else //Need restart computer
				{
					ShowMessageBox(IDS_RESTART_PC, MB_OK|MB_ICONINFORMATION);
				}
				break;
			}
		}
	}

	bool CreateRASEntry()
	{
		CRasEntry connection("WeWins Modem", "EDGE");
		return connection.Create(	
			"*99***1#", //号码
			"",			//指定IP
			"",			//DNS major
			"",			//DNS Minor
			"",			//User name
			"");		//Password	
	}
};

/**
*	模拟用户输入安装
*/
static bool Install2KXPUserEmu(const CString& strDestDir)
{
	CThreadDetectHW thread(strDestDir);
	thread.Start();

	MessageBox(AfxGetApp()->m_pMainWnd->GetSafeHwnd(),
		LoadStringEx(IDS_INSERTDEVICE_AGAIN), 
		"",MB_OK|MB_ICONINFORMATION);

	
	thread.WaitForComplete();

	return thread.m_DetectOK;
}


/** 2000/XP 下的安装
*/

bool InstallPCMCIA_2KXP(const CString& strDestDir)
{
	//Remove driver first
	CString strCmd = strDestDir+"FastRemove2KXP.exe -p";
	HANDLE hProcess = RunProgram(strCmd, false);
	if(!hProcess)return false;
	WaitForSingleObject(hProcess, INFINITE);
	CloseHandle(hProcess);

	return Install2KXPUserEmu(strDestDir);
}

/** 2000/XP下的卸载
*/
bool UninstallPCMCIA_2KXP(const CString& strDestDir)
{
	HANDLE hProcess = RunProgram(strDestDir+"FastRemove2KXP.exe -p", false);
	if(!hProcess)return false;

	WaitForSingleObject(hProcess, INFINITE);

	return true;
}

bool DisablePowerManagementOn9X()
{
	///WIN98下修改注册表中的电源管理
	HKEY hPortKey;
	CString strPosition = "SYSTEM\\CurrentControlSet\\Services\\VxD\\VCOMM";
	
	LONG lReturn = ::RegOpenKeyEx( HKEY_LOCAL_MACHINE, 
		strPosition,
		0, KEY_READ | KEY_WRITE | KEY_QUERY_VALUE | KEY_SET_VALUE, &hPortKey);
	if(lReturn == ERROR_SUCCESS)
	{
		DWORD dwData = 0x0;
		lReturn = ::RegSetValueEx(hPortKey, (LPCTSTR)"EnablePowerManagement", 0, REG_DWORD, (const byte *)&dwData, sizeof(DWORD));
		
		TRACE("\n修改98下串口\\Services\\VxD\\VCOMM设置EnablePowerManagement\n");		
	}

	return true;
}

CString GetPCMCIASocketHardwareID()
{
	char szDevDesc[256] = {0};

	const GUID  ClassGUID = {0x4d36e977L, 0xe325, 0x11ce, {0xbf, 0xc1, 0x08, 0x00, 0x2b, 0xe1, 0x03, 0x18}};
	
	HDEVINFO hDevInfo = SetupDiGetClassDevs((LPGUID)&ClassGUID, NULL, NULL, DIGCF_PRESENT);

	
	if ( INVALID_HANDLE_VALUE == hDevInfo)return "";

	SP_DEVINFO_DATA devInfoElem;
	devInfoElem.cbSize = sizeof(SP_DEVINFO_DATA);

	CString strHWID;
	for(int index = 0; SetupDiEnumDeviceInfo(hDevInfo, index, &devInfoElem); index++)
	{
		//Hardware ID
		if(SetupDiGetDeviceRegistryProperty(
			hDevInfo,
			&devInfoElem,
			SPDRP_HARDWAREID,
			NULL,
			(PBYTE)szDevDesc,
			sizeof(szDevDesc),
			NULL))
		{
			strHWID = szDevDesc;
			break;
		}
	}
	SetupDiDestroyDeviceInfoList(hDevInfo);

	return strHWID;
}

bool InstallPCMCIASocketDriver(const CString& strINF)
{
	CString strHWID = GetPCMCIASocketHardwareID();
	if(strHWID.IsEmpty())
	{
		return false;
	}
	CSignatureMonitor sigMon;
	sigMon.Start();

	CDllModule dll("newdev.dll");

	FUNC_UpdateDriverForPlugAndPlayDevices UpdateDriver = (FUNC_UpdateDriverForPlugAndPlayDevices)dll.GetFuncAdd("UpdateDriverForPlugAndPlayDevicesA");
	if(!UpdateDriver)
	{
		return false;
	}
	bool bOK = FALSE != UpdateDriver(NULL,
						strHWID,
						strINF,
						INSTALLFLAG_FORCE, 0);

	return bOK;
}

DWORD GetFileSize(const CString& strFile)
{
	CFile file;
	if(file.Open(strFile, CFile::modeRead))
	{
		return file.GetLength();
	}

	return 0;
}