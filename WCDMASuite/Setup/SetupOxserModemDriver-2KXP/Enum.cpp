// Enum.cpp : Defines the entry point for the console application.
//

#include "stdafx.h"
#include "Enum.h"
#include <setupapi.h>
#include "newdev.h"
#include "SignatureMonitor.h"
#include "../../DevEnum.h"


#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

/////////////////////////////////////////////////////////////////////////////
// The one and only application object

CWinApp theApp;

using namespace std;

CString GetModemID(const CString& strINF);
bool GetClassGUID(const CString& strINF, GUID& classID);
CString GetModemName(const CString& strINF);
CString GetPort(const CString& portName);
bool InstallModem(const CString& strINF, const CString& strCOMAttached);
bool RemoveModem(const CString& strINF);
CString GetAppPath();
bool IsModemExist();
static int g_ID; //0ÎªPCMCIA£¬1ÎªCOM

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
		if(argc > 1)
		{
			
			g_ID = 1;
			CString strINF = GetAppPath()+"Driver\\M500Emdm.inf";
			if(!IsModemExist())
				InstallModem(strINF,argv[1] );
		
			
		}
		else
		{
			g_ID = 0;
			CString strINF = GetAppPath()+"Driver\\wewinsmdm.inf";
			CString hardwareId = "PCMCIA\\CF-GEN-2027";

			InstallModem(strINF, GetPort(hardwareId));
		}
		
	}

	return nRetCode;
}

bool IsModemExist()
{
	CModemList modemList;

	std::vector<size_t> vMatched;
	for(size_t i=0; i<modemList.Count(); ++i)
	{
		const TModemInfo& info = modemList.GetAt(i);
		if(info.strFriendlyName == "WeWins M500E Modem" )//GetModemName()
		{
			
			
			return true;
		}
	}
	return false;
}

CString GetPort(const CString& hwid)
{

	//class ID of ComPorts
	
	const GUID  PortsGUID = {0x4d36e978L, 0xe325, 0x11ce, {0xbf, 0xc1, 0x08, 0x00, 0x2b, 0xe1, 0x03, 0x18}};
	HDEVINFO hDevInfo;
	hDevInfo = SetupDiGetClassDevs((LPGUID)&PortsGUID, NULL, NULL, DIGCF_PRESENT);
	
	if ( INVALID_HANDLE_VALUE == hDevInfo)return "";

	SP_DEVINFO_DATA devInfoElem;
	devInfoElem.cbSize = sizeof(SP_DEVINFO_DATA);
		
	CString strPort;
	int index = 0;
	while ( SetupDiEnumDeviceInfo(hDevInfo, index++, &devInfoElem))
	{
		char szDevDesc[256] = {0};
		bool bMatch = false;
		if(SetupDiGetDeviceRegistryProperty(
			hDevInfo,
			&devInfoElem,
			SPDRP_HARDWAREID,
			NULL,
			(PBYTE)szDevDesc,
			sizeof(szDevDesc),
			NULL))
		{
			strPort = szDevDesc;
			strPort.MakeUpper();
			if(hwid.Find(strPort) !=-1 || strPort.Find(hwid)!=-1)
				bMatch = true;
		}

		if(bMatch)
		{
			HKEY hKeyDev = SetupDiOpenDevRegKey(hDevInfo, &devInfoElem, DICS_FLAG_GLOBAL, 0, DIREG_DEV, KEY_READ);
			if (INVALID_HANDLE_VALUE != hKeyDev) 
			{
				WORD length = sizeof(szDevDesc);
				if(ERROR_SUCCESS == RegQueryValueEx(hKeyDev, 
										_T((const char *)"PortName"), 
										NULL, 
										NULL, 
										(unsigned char *)szDevDesc, 
										(LPDWORD)&length)) 
				{
					strPort = szDevDesc;
				}
				RegCloseKey(hKeyDev);
			} 
			break;
		}

	}
	SetupDiDestroyDeviceInfoList(hDevInfo);

	return strPort;
}

CString GetModemID(const CString& strINF)
{
	HINF hInf = SetupOpenInfFile(
		strINF, 
		0,
		INF_STYLE_WIN4,
		0);
	if(INVALID_HANDLE_VALUE == hInf)return "";

	INFCONTEXT infContext;
	CString strSection;
if(g_ID > 0)
	strSection = "Generic.NTx86";
else
	strSection = "Generic";

	BOOL bOK = SetupFindFirstLine(
		hInf,
		strSection,
		0,
		&infContext);

	char buf[256]={0};
if(g_ID > 0)
	strSection = "Gen556";
else
	strSection = "Generic";

	if(bOK)
	{
		bOK = SetupGetLineText(
			&infContext,
			hInf,
			"Generic",
			0,
			buf,
			sizeof(buf),
			0);
	}

	CString str;
	if(bOK)
	{
		for(int i=0;i<sizeof(buf); ++i)
		{
			if(buf[i]==',')break;
		}
		
		for(++i;i<sizeof(buf); ++i)
		{
			if(buf[i]!=' ')break;
		}
		str = buf+i;
	}

	SetupCloseInfFile(hInf);

	return str;
}

bool GetClassGUID(const CString& strINF, GUID& classGUID)
{
	return TRUE== SetupDiGetINFClass(strINF,&classGUID, 0, 0,0);
}

CString GetModemName(const CString& strINF)
{
	HINF hInf = SetupOpenInfFile(
		strINF, 
		0,
		INF_STYLE_WIN4,
		0);
	if(INVALID_HANDLE_VALUE == hInf)return "";

	INFCONTEXT infContext;
	BOOL bOK = SetupFindFirstLine(
		hInf,
		"Strings",
		"Gen2300",
		&infContext);

	char buf[256]={0};
	if(bOK)
	{
		bOK = SetupGetLineText(
			&infContext,
			hInf,
			"Strings",
			"Gen2300",
			buf,
			sizeof(buf),
			0);
	}

	CString str;
	if(bOK)
	{
		str = buf;
	}

	SetupCloseInfFile(hInf);

	return str;

}

BOOL RegisterModem(
			  HDEVINFO hdi,
			  PSP_DEVINFO_DATA pdevData,
			  LPCTSTR pszPort)
{
	BOOL bRet;
	SP_DRVINFO_DATA drvData;
	DWORD nErr = NO_ERROR;
	DWORD dwRet;
	TCHAR const c_szAttachedTo[] = _T("AttachedTo");
	HKEY hKeyDev;
	bRet = FALSE;
	
	if( !SetupDiRegisterDeviceInfo(hdi, pdevData, 0, NULL, NULL, NULL) )
	{
		return bRet;
	}
	
	hKeyDev = SetupDiOpenDevRegKey(hdi, pdevData, DICS_FLAG_GLOBAL, 0, DIREG_DRV, KEY_ALL_ACCESS); //// This call fails....
	
	if( (INVALID_HANDLE_VALUE == hKeyDev) && ( ERROR_KEY_DOES_NOT_EXIST == GetLastError()) )
	{
		hKeyDev = SetupDiCreateDevRegKey(hdi, pdevData, DICS_FLAG_GLOBAL, 0, DIREG_DRV, NULL, NULL);
		
		if( INVALID_HANDLE_VALUE == hKeyDev )
		{
			return FALSE;
		}
	}
	else {
		return FALSE;
	}
	
	if (ERROR_SUCCESS != (dwRet = RegSetValueEx (hKeyDev, c_szAttachedTo, 0, REG_SZ,
		(PBYTE)pszPort, (lstrlen(pszPort)+1)*sizeof(TCHAR))))
	{
		SetLastError (dwRet);
		bRet = FALSE;
	}
	RegCloseKey (hKeyDev);

	if( !SetupDiRegisterDeviceInfo(hdi, pdevData, 0, NULL, NULL, NULL) )
	{
		return FALSE;
	}
	
	if ( !SetupDiGetSelectedDriver(hdi, pdevData, &drvData)) {
	
	}
	
	return TRUE;
}

static CSignatureMonitor s_singmonitThread;

bool InstallModem(const CString& strINF, const CString& strCOMAttached)
{
	s_singmonitThread.Start();
	while(!s_singmonitThread.IsRunning())
	{
		Sleep(10);
	}

	GUID ClassGUID;
	TCHAR ClassName[256]={0};

	BOOL bOK = SetupDiGetINFClass(strINF,&ClassGUID,ClassName,sizeof(ClassName),0);

	HDEVINFO DeviceInfoSet = INVALID_HANDLE_VALUE;

	if(bOK)
	{
		DeviceInfoSet = SetupDiCreateDeviceInfoList(&ClassGUID,0);
		bOK = DeviceInfoSet != INVALID_HANDLE_VALUE; 
	}

	SP_DEVINFO_DATA DeviceInfoData;
	DeviceInfoData.cbSize = sizeof(SP_DEVINFO_DATA);
	if(bOK)
	{
		bOK = SetupDiCreateDeviceInfo(DeviceInfoSet,
					ClassName,
					&ClassGUID,
					NULL,
					0,
					DICD_GENERATE_ID,
					&DeviceInfoData);
	}	
	
	CString strHardwareId = GetModemID(strINF);
	if(bOK)
	{
		int nSize = strHardwareId.GetLength()+2;
		BYTE* buf = new BYTE[nSize];
		memset(buf,0,nSize);
		memcpy(buf,(LPCSTR)strHardwareId,strHardwareId.GetLength());

		bOK = SetupDiSetDeviceRegistryProperty(DeviceInfoSet,
				&DeviceInfoData,
				SPDRP_HARDWAREID,
				buf,
				nSize);
		delete[] buf;
		TRACE1("%d\n",GetLastError());
	}

	if(bOK)
	{
		bOK = RegisterModem(DeviceInfoSet, &DeviceInfoData, strCOMAttached);
	}

	if(bOK)
	{
		bOK = UpdateDriverForPlugAndPlayDevices(0,
			strHardwareId,
			strINF,
			INSTALLFLAG_FORCE,
			0);
	}

	SetupDiDestroyDeviceInfoList(DeviceInfoSet);

	return bOK ==  TRUE;
}

bool RemoveModem(const CString& strINF)
{
	GUID ClassGUID;
	TCHAR ClassName[256]={0};

	BOOL bOK = SetupDiGetINFClass(strINF,&ClassGUID,ClassName,sizeof(ClassName),0);

	HDEVINFO DeviceInfoSet = INVALID_HANDLE_VALUE;

	if(bOK)
	{
		DeviceInfoSet = SetupDiCreateDeviceInfoList(&ClassGUID,0);
		bOK = DeviceInfoSet != INVALID_HANDLE_VALUE; 
	}

	SP_DEVINFO_DATA DeviceInfoData;
	DeviceInfoData.cbSize = sizeof(SP_DEVINFO_DATA);
	if(bOK)
	{
		bOK = SetupDiCreateDeviceInfo(DeviceInfoSet,
					ClassName,
					&ClassGUID,
					NULL,
					0,
					DICD_GENERATE_ID,
					&DeviceInfoData);
	}	
	
	CString strHardwareId = GetModemID(strINF);
	if(bOK)
	{
		int nSize = strHardwareId.GetLength()+2;
		BYTE* buf = new BYTE[nSize];
		memset(buf,0,nSize);
		memcpy(buf,(LPCSTR)strHardwareId,strHardwareId.GetLength());

		bOK = SetupDiSetDeviceRegistryProperty(DeviceInfoSet,
				&DeviceInfoData,
				SPDRP_HARDWAREID,
				buf,
				nSize);
		delete[] buf;
		TRACE1("%d\n",GetLastError());
	}

	if(bOK)
	{
		SetupDiCallClassInstaller(
			DIF_REMOVE,
			DeviceInfoSet,
			&DeviceInfoData);
	}

	bOK = SetupDiDestroyDeviceInfoList(DeviceInfoSet);

	return bOK ==  TRUE;
}

