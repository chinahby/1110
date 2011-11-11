#include "stdafx.h"
#include <fstream>
#include "log.h"

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

static CString GetSysLanguage()
{
	struct TStruct
	{
		LANGID id;
		CString strLang;

		TStruct(LANGID lid, const CString& lang)
			:id(lid), strLang(lang)
		{
		}
	};

	TStruct lang_name[] = 
	{
		TStruct(0x0000, "Neutral"),
		TStruct(0x0400, "Default"),
		TStruct(0x0401, "Arabic"),
		TStruct(0x0801, "Arabic"),
		TStruct(0x0c01, "Arabic"),
		TStruct(0x1001, "Arabic"),
		TStruct(0x1401, "Arabic"),
		TStruct(0x1801, "Arabic"),
		TStruct(0x1c01, "Arabic"),
		TStruct(0x2001, "Arabic"),
		TStruct(0x2401, "Arabic"),
		TStruct(0x2801, "Arabic"),
		TStruct(0x2c01, "Arabic"),
		TStruct(0x3001, "Arabic"),
		TStruct(0x3401, "Arabic"),
		TStruct(0x3801, "Arabic"),
		TStruct(0x3c01, "Arabic"),
		TStruct(0x4001, "Arabic"),
		TStruct(0x0402, "Bulgarian"),
		TStruct(0x0403, "Catalan"),
		TStruct(0x0404, "Chinese-Tradition"), 
		TStruct(0x0804, "Chinese-Simple"),
		TStruct(0x0c04, "Chinese-Tradition"), 
		TStruct(0x1004, "Chinese-Tradition"), 
		TStruct(0x0405, "Czech"),  
		TStruct(0x0406, "Danish"),  
		TStruct(0x0407, "German"), 
		TStruct(0x0807, "German"),  
		TStruct(0x0c07, "German"),  
		TStruct(0x1007, "German"),  
		TStruct(0x1407, "German"),  
		TStruct(0x0408, "Greek"),  
		TStruct(0x0409, "English"), 
		TStruct(0x0809, "English"), 
		TStruct(0x0c09, "English"), 
		TStruct(0x1009, "English"), 
		TStruct(0x1409, "English"), 
		TStruct(0x1809, "English"), 
		TStruct(0x1c09, "English"),  
		TStruct(0x2009, "English"), 
		TStruct(0x2409, "English"), 
		TStruct(0x2809, "English"), 
		TStruct(0x2c09, "English"), 
		TStruct(0x040a, "Spanish"),  
		TStruct(0x080a, "Spanish"), 
		TStruct(0x0c0a, "Spanish"), 
		TStruct(0x100a, "Spanish"),  
		TStruct(0x140a, "Spanish"),  
		TStruct(0x180a, "Spanish"),  
		TStruct(0x1c0a, "Spanish"),  
		TStruct(0x200a, "Spanish"),  
		TStruct(0x240a, "Spanish"), 
		TStruct(0x280a, "Spanish"), 
		TStruct(0x2c0a, "Spanish"), 
		TStruct(0x300a, "Spanish"),  
		TStruct(0x340a, "Spanish"),  
		TStruct(0x380a, "Spanish"),  
		TStruct(0x3c0a, "Spanish"),  
		TStruct(0x400a, "Spanish"),  
		TStruct(0x440a, "Spanish"),  
		TStruct(0x480a, "Spanish"),  
		TStruct(0x4c0a, "Spanish"),  
		TStruct(0x500a, "Spanish"),  
		TStruct(0x040b, "Finnish"), 
		TStruct(0x040c, "French"), 
		TStruct(0x080c, "French"), 
		TStruct(0x0c0c, "French"), 
		TStruct(0x100c, "French"),  
		TStruct(0x140c, "French"),  
		TStruct(0x040d, "Hebrew"), 
		TStruct(0x040e, "Hungarian"), 
		TStruct(0x040f, "Icelandic"), 
		TStruct(0x0410, "Italian"), 
		TStruct(0x0810, "Italian"),  
		TStruct(0x0411, "Japanese"), 
		TStruct(0x0412, "Korean"), 
		TStruct(0x0812, "Korean"),  
		TStruct(0x0413, "Dutch"), 
		TStruct(0x0813, "Dutch"), 
		TStruct(0x0414, "Norwegian"), 
		TStruct(0x0814, "Norwegian"),  
		TStruct(0x0415, "Polish"), 
		TStruct(0x0416, "Portuguese"),  
		TStruct(0x0816, "Portuguese"), 
		TStruct(0x0418, "Romanian"), 
		TStruct(0x0419, "Russian"), 
		TStruct(0x041a, "Croatian"),  
		TStruct(0x081a, "Serbian"), 
		TStruct(0x0c1a, "Serbian"), 
		TStruct(0x041b, "Slovak"), 
		TStruct(0x041c, "Albanian"), 
		TStruct(0x041d, "Swedish"), 
		TStruct(0x081d, "Swedish"), 
		TStruct(0x041e, "Thai"), 
		TStruct(0x041f, "Turkish"), 
		TStruct(0x0421, "Indonesian"), 
		TStruct(0x0422, "Ukrainian"), 
		TStruct(0x0423, "Belarusian"), 
		TStruct(0x0424, "Slovenian"), 
		TStruct(0x0425, "Estonian"), 
		TStruct(0x0426, "Latvian"), 
		TStruct(0x0427, "Lithuanian"), 
		TStruct(0x0429, "Farsi"), 
		TStruct(0x042a, "Vietnamese"), 
		TStruct(0x042d, "Basque"), 
		TStruct(0x0436, "Afrikaans"), 
		TStruct(0x0438, "Faeroese")
	};

	LANGID idUser = GetUserDefaultLangID();

	for(int i=0; i<sizeof(lang_name)/sizeof(lang_name[0]); ++i)
	{
		if(idUser == lang_name[i].id)return lang_name[i].strLang;
	}

	return "";
}

CString LoadDLLString(const CString& dllName, UINT strID)
{
	CString str;
	HINSTANCE hDll = ::LoadLibrary(dllName);
	if(hDll)
	{
		char buf[1024]={0};
		::LoadString(hDll, strID, buf, sizeof(buf)-1);
		str = buf;
		::FreeLibrary(hDll);
	}

	return str;
}

static CString FindWizardTxtFile(const CString& strWizard,
								 const CString& strSysId)
{
	CString strTxtFile;

	WIN32_FIND_DATA findData;
	CString sFileName = GetAppPath()+"SetupTxt\\*.txt";
	HANDLE hFind = ::FindFirstFile(sFileName, &findData);
	if(hFind != INVALID_HANDLE_VALUE)
	{
		do
		{
			CString strINIFile = GetAppPath()+"SetupTxt\\" + findData.cFileName;
			char buf[512]={0};
			GetPrivateProfileString(
				strSysId,
				"wizardTitle",
				NULL,
				buf,
				sizeof(buf),
				strINIFile);

			if(strWizard == buf)
			{
				strTxtFile = strINIFile;
				break;
			}
		}while(FindNextFile(hFind,&findData));
		::FindClose(hFind);
	}

	return strTxtFile;
}

static CString GetIniFileName(const CString& strSysId)
{
	CString strFile;
	if(SystemIsWin98())
		strFile = GetAppPath()+"SetupTxt\\Setup"+GetSysLanguage()+".txt";
	else if(SystemIsWinME())
	{
		//TODO
	}
	else
	{
		//2000/XP 平台通过查找 NewDev.dll 中 id为 2040的字符串
		//来确定适用哪种语言的setup text file
		CString strWizard = LoadDLLString("NewDev.dll", 2040);
		strFile = FindWizardTxtFile(strWizard, strSysId);	
		if(strFile == "")LOGOUT2("wizard text file not found, WizardTitle=", (LPCTSTR)strWizard);
	}

	return strFile;
}

CString GetWizardTextBySysId(const CString& item, const CString& strSysId)
{
	static CString strINIFile;
	if(strINIFile == "")
	 strINIFile = GetIniFileName(strSysId);

	char buf[512]={0};
	GetPrivateProfileString(
		strSysId,
		item,
		NULL,
		buf,
		sizeof(buf),
		strINIFile);

	return buf;
}

CString GetWizardText(const CString& item)
{
	if(SystemIsWin98())
		return GetWizardTextBySysId(item, "Win98");

	if(SystemIsWin2K())
		return GetWizardTextBySysId(item, "Win2K");

	if(SystemIsWinXP())
		return GetWizardTextBySysId(item, "WinXP");

	if(SystemIsWinME())
		return GetWizardTextBySysId(item, "WinME");

	return "";
}	
