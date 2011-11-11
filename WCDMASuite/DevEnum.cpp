#include "stdAfx.h"
#include "DevEnum.h"
#include <setupapi.h>
#include <iostream>


using namespace std;

bool TModemInfo::operator==(const TModemInfo& another)const
{
	return	strHardwareID.CompareNoCase(another.strHardwareID) == 0 &&
			strPortAttached.CompareNoCase(another.strPortAttached) == 0 &&
			strFriendlyName.CompareNoCase(another.strFriendlyName) == 0;
}

CModemList::CModemList()
{
	Load();
}

void CModemList::Load()
{
	m_Modems.clear();

	TCHAR szDevDesc[256] = {0};
	const GUID  ModemClassGUID = {0x4D36E96DL, 0xE325, 0x11CE, {0xBF, 0xC1, 0x08, 0x00, 0x2B, 0xE1, 0x03, 0x18}};
	
	HDEVINFO hDevInfo = SetupDiGetClassDevs((LPGUID)&ModemClassGUID, NULL, NULL, DIGCF_PRESENT);
	
	if ( INVALID_HANDLE_VALUE == hDevInfo)return;

	SP_DEVINFO_DATA devInfoElem;
	devInfoElem.cbSize = sizeof(SP_DEVINFO_DATA);
		
	TModemInfo modemInfo;
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
			modemInfo.strHardwareID = szDevDesc;
		}

		//Friendly name
		if(SetupDiGetDeviceRegistryProperty(
			hDevInfo,
			&devInfoElem,
			SPDRP_FRIENDLYNAME,
			NULL,
			(PBYTE)szDevDesc,
			sizeof(szDevDesc),
			NULL))
		{
			modemInfo.strFriendlyName = szDevDesc;
		}

		//Port attached to
		HKEY hKeyDev = SetupDiOpenDevRegKey(
									hDevInfo, 
									&devInfoElem, 
									DICS_FLAG_GLOBAL, 
									0, 
									DIREG_DRV, 
									KEY_READ);
		if (INVALID_HANDLE_VALUE != hKeyDev) 
		{
			for(DWORD i=0; true; ++i)
			{
				TCHAR nam[512]={0};
				TCHAR val[512]={0};
				DWORD dwNam=sizeof(nam);
				DWORD dwVal=sizeof(val);
				if(ERROR_SUCCESS == RegEnumValue(
										hKeyDev,
										i,
										nam,
										&dwNam,
										NULL,NULL,
										(LPBYTE)val,
										&dwVal))
				{
					CString strName = nam;
					if(strName.CompareNoCase(TEXT("AttachedTo"))==0)
					{
						modemInfo.strPortAttached = val;
						break;
					}
				}
				else break;
			}
			RegCloseKey(hKeyDev);
		} 

		m_Modems.push_back(modemInfo);
	}
	SetupDiDestroyDeviceInfoList(hDevInfo);
}

void CModemList::Remove(const TModemInfo& info)
{
	m_Modems.clear();

	TCHAR szDevDesc[256] = {0};
	const GUID  ModemClassGUID = {0x4D36E96DL, 0xE325, 0x11CE, {0xBF, 0xC1, 0x08, 0x00, 0x2B, 0xE1, 0x03, 0x18}};
	
	HDEVINFO hDevInfo = SetupDiGetClassDevs((LPGUID)&ModemClassGUID, NULL, NULL, DIGCF_PRESENT);
	
	if ( INVALID_HANDLE_VALUE == hDevInfo)return;

	SP_DEVINFO_DATA devInfoElem;
	devInfoElem.cbSize = sizeof(SP_DEVINFO_DATA);
		
	TModemInfo modemInfo;
	for(int index = 0; SetupDiEnumDeviceInfo(hDevInfo, index, &devInfoElem); index++)
	{
		modemInfo.strHardwareID="";

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
			modemInfo.strHardwareID = szDevDesc;
		}

		//Friendly name
		if(SetupDiGetDeviceRegistryProperty(
			hDevInfo,
			&devInfoElem,
			SPDRP_FRIENDLYNAME,
			NULL,
			(PBYTE)szDevDesc,
			sizeof(szDevDesc),
			NULL))
		{
			modemInfo.strFriendlyName = szDevDesc;
		}

		//Port attached to
		HKEY hKeyDev = SetupDiOpenDevRegKey(
									hDevInfo, 
									&devInfoElem, 
									DICS_FLAG_GLOBAL, 
									0, 
									DIREG_DRV, 
									KEY_READ);
		if (INVALID_HANDLE_VALUE != hKeyDev) 
		{
			for(DWORD i=0; true; ++i)
			{
				TCHAR nam[512]={0};
				TCHAR val[512]={0};
				DWORD dwNam=sizeof(nam);
				DWORD dwVal=sizeof(val);
				if(ERROR_SUCCESS == RegEnumValue(
										hKeyDev,
										i,
										nam,
										&dwNam,
										NULL,NULL,
										(LPBYTE)val,
										&dwVal))
				{
					CString strName = nam;
					if(strName.CompareNoCase(TEXT("AttachedTo"))==0)
					{
						modemInfo.strPortAttached = val;
						break;
					}
				}
				else break;
			}
			RegCloseKey(hKeyDev);
		} 

		if(modemInfo == info)
		{
			if(SetupDiCallClassInstaller(DIF_REMOVE,
                    hDevInfo,
                    &devInfoElem))
			{
				continue;
			}
		}

		m_Modems.push_back(modemInfo);
	}
	SetupDiDestroyDeviceInfoList(hDevInfo);
}

size_t CModemList::Count()const
{
	return m_Modems.size();
}
const TModemInfo& CModemList::GetAt(size_t index)const
{
	return m_Modems[index];
}

//********************************************************
//	Implementation of class CPortList
//********************************************************

bool TPortInfo::operator==(const TPortInfo& another)const
{
	return	strHardwareID.CompareNoCase(another.strHardwareID) == 0 &&
			strCOM.CompareNoCase(another.strCOM) == 0 &&
			strFriendlyName.CompareNoCase(another.strFriendlyName) == 0;
}


CPortList::CPortList()
{
	Load();
}
size_t CPortList::Count()const
{
	return m_Ports.size();
}
const TPortInfo& CPortList::GetAt(size_t index)const
{
	return m_Ports[index];
}
void CPortList::Load()
{
	TRACE(TEXT("CPortList::Load :> START"));
	TCHAR szDevDesc[256] = {0};
	const GUID  PortsGUID = {0x4d36e978L, 0xe325, 0x11ce, {0xbf, 0xc1, 0x08, 0x00, 0x2b, 0xe1, 0x03, 0x18}};
	
	HDEVINFO hDevInfo = SetupDiGetClassDevs((LPGUID)&PortsGUID, NULL, NULL, DIGCF_PRESENT);
	
	if ( INVALID_HANDLE_VALUE == hDevInfo)return;

	SP_DEVINFO_DATA devInfoElem;
	devInfoElem.cbSize = sizeof(SP_DEVINFO_DATA);
		
	TPortInfo portInfo;
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
			portInfo.strHardwareID = szDevDesc;
		}

		//Friendly name
		if(SetupDiGetDeviceRegistryProperty(
			hDevInfo,
			&devInfoElem,
			SPDRP_FRIENDLYNAME,
			NULL,
			(PBYTE)szDevDesc,
			sizeof(szDevDesc),
			NULL))
		{
			portInfo.strFriendlyName = szDevDesc;
		}

		//Port attached to
		HKEY hKeyDev = SetupDiOpenDevRegKey(
									hDevInfo, 
									&devInfoElem, 
									DICS_FLAG_GLOBAL, 
									0, 
									DIREG_DEV, 
									KEY_READ);
		if (INVALID_HANDLE_VALUE != hKeyDev) 
		{
			for(DWORD i=0; true; ++i)
			{
				TCHAR nam[512]={0};
				TCHAR val[512]={0};
				DWORD dwNam=sizeof(nam);
				DWORD dwVal=sizeof(val);
				if(ERROR_SUCCESS == RegEnumValue(
										hKeyDev,
										i,
										nam,
										&dwNam,
										NULL,NULL,
										(LPBYTE)val,
										&dwVal))
				{
					CString strName = nam;
					if(strName.CompareNoCase(TEXT("PortName"))==0)
					{
						portInfo.strCOM = val;
						break;
					}
				}
				else break;
			}

			RegCloseKey(hKeyDev);
		} 
		TRACE(TEXT("CPortList::Load :> PUSH_BACK ONCE\n"));
		m_Ports.push_back(portInfo);
	}
	SetupDiDestroyDeviceInfoList(hDevInfo);
	TRACE(TEXT("CPortList::Load :> END"));
}
