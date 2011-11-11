#include "StdAfx.h"
#include <Setupapi.h>
#include <atlbase.h>
#include <vector>
#include <algorithm>

void EnumExistingDev(std::vector<CString>& devs)
{
	devs.clear();
	char szDevDesc[256] = {0};
	
	HDEVINFO hDevInfo = SetupDiGetClassDevs(NULL, NULL, NULL, DIGCF_ALLCLASSES);
	
	if ( INVALID_HANDLE_VALUE == hDevInfo)return;

	SP_DEVINFO_DATA devInfoElem;
	devInfoElem.cbSize = sizeof(SP_DEVINFO_DATA);
		
	for(int index = 0; SetupDiEnumDeviceInfo(hDevInfo, index, &devInfoElem); index++)
	{
		CString strID;
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
			devs.push_back(szDevDesc);
		}

	}

	SetupDiDestroyDeviceInfoList(hDevInfo);
}

void Test()
{
	//std::vector<CString> devs;
	//EnumExistingDev(devs);
}

static std::vector<CString> devs;
bool HardwareExisting(CString& strHWID)
{
	if(!devs.size())
	{
		EnumExistingDev(devs);
		return false;
	}

	char szDevDesc[256] = {0};

	HDEVINFO hDevInfo = SetupDiGetClassDevs(NULL, NULL, NULL, DIGCF_ALLCLASSES);
	
	if ( INVALID_HANDLE_VALUE == hDevInfo)return false;

	SP_DEVINFO_DATA devInfoElem;
	devInfoElem.cbSize = sizeof(SP_DEVINFO_DATA);

	CString strIDFound = strHWID;
	strIDFound.MakeLower();

	bool bFound = false;
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
			TRACE1("%s\n", szDevDesc);
			CString strID = szDevDesc;
			strID.MakeLower();
			if(	strID.Find(strIDFound)!=-1 && 
				std::find(devs.begin(), devs.end(), szDevDesc)==devs.end())
			{
				strHWID = szDevDesc;
				bFound = true;
				break;
			}
		}
	}
	SetupDiDestroyDeviceInfoList(hDevInfo);

	return bFound;
}

