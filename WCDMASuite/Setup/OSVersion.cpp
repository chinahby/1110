#include "stdafx.h"
#include ".\OSVersion.h"
#include ".\DLLModule.h"

COSVersion::EOSType COSVersion::GetOSVersion()
{
	OSVERSIONINFO osv;
	osv.dwOSVersionInfoSize = sizeof(osv);
	GetVersionEx(&osv);
	if(	osv.dwMajorVersion == 4 &&
		osv.dwMinorVersion == 10 &&
		osv.dwPlatformId == VER_PLATFORM_WIN32_WINDOWS)
	{
		return WIN_98;
	}

	if(	osv.dwMajorVersion == 4 &&
		osv.dwMinorVersion == 90 &&
		osv.dwPlatformId == VER_PLATFORM_WIN32_WINDOWS)
	{
		return WIN_ME;
	}

	if(	osv.dwMajorVersion == 5 &&
		osv.dwMinorVersion == 0 &&
		osv.dwPlatformId == VER_PLATFORM_WIN32_NT)
	{
		return WIN_2K;
	}

	if(	osv.dwMajorVersion == 5 &&
		osv.dwMinorVersion == 1 &&
		osv.dwPlatformId == VER_PLATFORM_WIN32_NT)
	{
		return WIN_XP;
	}

	if(	osv.dwMajorVersion == 5 &&
		osv.dwMinorVersion == 2 &&
		osv.dwPlatformId == VER_PLATFORM_WIN32_NT)
	{
		return WIN_2003;
	}

	if(	osv.dwMajorVersion == 6 )
	{
		return WIN_VISTA;
	}

	return WIN_UNKNOWN;
}

bool COSVersion::IsWin9X()
{
	EOSType type = GetOSVersion();
	return type == WIN_98 || type == WIN_ME;
}

bool COSVersion::IsAMD64()
{
	typedef void  (WINAPI *FUNC_GetSysInf)(LPSYSTEM_INFO);
	CDllModule dll(TEXT("Kernel32.dll"));
	bool bOK = false;
	FUNC_GetSysInf getSysInf = (FUNC_GetSysInf)dll.GetFuncAdd("GetNativeSystemInfo");
	if(getSysInf)
	{
		SYSTEM_INFO sysInfo;
		getSysInf(&sysInfo);
		bOK = sysInfo.wProcessorArchitecture == PROCESSOR_ARCHITECTURE_AMD64;
	}
	return bOK;
}

bool COSVersion::Is64BitsPlatform()
{
	static bool s_IsAMD64 = IsAMD64();

	return s_IsAMD64;
}