#include "StdAfx.h"
#include "../Porting.h"
#include "../RegistryEx.h"
#inlcude "../devenum.h"

#ifdef USB_MODEM

//在注册表中的配置路径

PORTING_API
const CString& GetConfigString()
{
	static CString strConfig = "Software\\WCDMA Modem\\WCDMA";//"Software\\M500EUSB\\M500EUSBConf";

	return strConfig;
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

	static CString strModem;
	strModem.Format(TEXT("SinCom Mobile"));
	return strModem;
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
	static CString strID = "{51EA1C23-BE10-406A-9CF2-B7CDEAB1FB39}";

	return strID;
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
}

//modem初始化AT配置路径
PORTING_API
const CString& GetModemConfigString()
{
	static CString strConfig = "SYSTEM\\CurrentControlSet\\Control\\Class\\{4D36E96D-E325-11CE-BFC1-08002BE10318}";
	return strConfig;
}

#endif//USB_MODEM