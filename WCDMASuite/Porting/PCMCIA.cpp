#include "StdAfx.h"
#include "../Porting.h"
#include "../RegistryEx.h"
#include "../devenum.h"



//在注册表中的配置路径



CString GetAppPath()
{
	TCHAR path_buffer[MAX_PATH];	
	TCHAR drive[_MAX_DRIVE];
	TCHAR dir[_MAX_DIR];

	GetModuleFileName(NULL, path_buffer, MAX_PATH);
	_tsplitpath(path_buffer, drive, dir, 0, 0 );

	CString sPath = drive;
	sPath += dir;

	return sPath;
}

CString GetProduct()
{
	CFile file;
	CString strPath = GetAppPath() + "D-dur.dat";
	
	DWORD dwMaximumSizeHigh = 0;
	DWORD dwMaximumSizeLow = 0;
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

	char bufProduct[10];

	strncpy(bufProduct,buf,2);
	bufProduct[2] = '\0';
	CString strProduct = bufProduct;
	
	CloseHandle(hFile);

	return strProduct;				// the same as return "-u";
}


PORTING_API
const CString& GetConfigString()
{
	static CString strConfig[] =
	{
		"Software\\SunnyCat\\SunnyCatConf",
		"Software\\WCDMA Modem\\WCDMA",//,//"Software\\M500EUSB\\M500EUSBCONF",
		"Software\\M500ECOM\\M500ECOMConf"
	};
	CString strProduct = GetProduct();
	size_t i = 0;
//	if(strProduct == "-p")
//		i = 0;
//	else if(strProduct == "-u")
		i = 1;
//	else if(strProduct == "-c")
//		i = 2;
	
	return strConfig[i];
}

//Modem名称
PORTING_API
const CString& GetModemName()
{
	CModemList modemList;

	static CString strModem;
		
	std::vector<size_t> vMatched;
	for(size_t i=0; i<modemList.Count(); ++i)
	{
		const TModemInfo& info = modemList.GetAt(i);
		CString strID = info.strHardwareID;
		strID.MakeLower();
		if( strID.Find(TEXT("vid_05c6&pid_9003&rev_0000&mi_03")) >=0 )
		{
			TRACE(TEXT("GetModemName strFriendlyName = %s\n"), info.strFriendlyName);
			 strModem = info.strFriendlyName;
			 return strModem;
		}
	}	

	static CString strModema;
	strModema.Format(TEXT("SinCom Mobile"));
	return strModema;
}

PORTING_API
const CString& GetModemComName()
{
	CModemList modemList;
		
	std::vector<size_t> vMatched;
	for(size_t i=0; i<modemList.Count(); ++i)
	{
		const TModemInfo& info = modemList.GetAt(i);
		CString strID = info.strHardwareID;
		strID.MakeLower();
		if( strID.Find(TEXT("vid_05c6&pid_9003&rev_0000&mi_03")) >=0 )
		{
			 TRACE(TEXT("GetModemComName strPortAttached = %s\n"), info.strPortAttached);
			 return info.strPortAttached;
		}
	}
	// 报错处理!
	static CString strModem = "COM13";
	return strModem;
}

//modem初始化AT配置路径
PORTING_API
const CString& GetModemConfigString()
{
	static CString strConfig = "SYSTEM\\CurrentControlSet\\Control\\Class\\{4D36E96D-E325-11CE-BFC1-08002BE10318}";
	return strConfig;
}

//连接名
PORTING_API
const CString& GetConnectionName()
{
	static CString strConnection = "WCDMA Wireless Connection"; //连接名称(entry Name)
	return strConnection;
}

//应用ID，用来保证同一个应用不会启动两次
PORTING_API
const CString& GetAppID()
{
	static CString strID[] =
	{
		"{F64E601B-8F2C-4e9c-8B3F-2577498BABE9}",
		"{51EA1C23-BE10-406A-9CF2-B7CDEAB1FB39}",
		"{B167BE46-3966-4567-807C-90E9D10814D9}"
	};
	CString strProduct = GetProduct();
	size_t i = 0;
//	if(strProduct == "-p")
//		i = 0;
//	else if(strProduct == "-u")
		i = 1;
//	else if(strProduct == "-c")
//		i = 2;

	return strID[i];
}

//Splash window的图片文件名
PORTING_API
const CString& GetSplashBMPFileName()
{
	static CString strID = "Splash.bmp";
	return strID;
}

//应用名，显示在标题上
PORTING_API
const CString& GetAppName()
{
	static CString str = "WCDMA Modem";

	return str;
/*	
	static CString str[]=
	{
		"WeWins SC700 CARD",
		"EVDO Modem",
		"WeWins COM Modem"
	};
	CString strProduct = GetProduct();
	size_t i = 0;
//	if(strProduct == "-p")
//		i = 0;
//	else if(strProduct == "-u")
		i = 1;
//	else if(strProduct == "-c")
//		i = 2;

	return str[i];
*/
}

//加密文件名，使用于LockCode
PORTING_API
const CString& GetEncryptFile()
{
	static CString str = "";
	return str;
}

PORTING_API 
const CString& GetHardwareID(int i)
{
	static CString strHWID = "Rev_0000";
	static CString strHWIDNULL="";

	if(i)return strHWIDNULL;

	return strHWID;

	/***********************
	static CString strHWID[] =
	{
		"PCMCIA\\CF-GEN-2027",
	};
	static CString strHWIDUSB[] = 
	{
		"Rev_0000",
	};
	CString strProduct = GetProduct();
	static CString strHWIDNULL="";
	/**********************************
	if(strProduct == "-p")
	{
		int nSize = sizeof(strHWID)/sizeof(strHWID[0]);
		if(i<nSize)
			return strHWID[i];
		else
			return strHWIDNULL;
	}
	else if(strProduct == "-u")
	/*********************************
	{
		int nSize = sizeof(strHWIDUSB)/sizeof(strHWID[0]);
		if(i<nSize)
			return strHWIDUSB[i];
		else
			return strHWIDNULL;
	}
	/***********************
	else if(strProduct == "-c")
	{
		return strHWIDNULL;
	}
	return strHWIDNULL;
	/***********************/
}
