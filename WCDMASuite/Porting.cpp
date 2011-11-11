#include "StdAfx.h"
#include "Porting.h"
#include "registryex.h"

//检测Modem
PORTING_API
#ifdef USB_MODEM
DWORD OptionsDetectPortOn2K()
{
	CRegistryEx		RegistryEx;
    DWORD			status = 0;
    CStringArray	strArray;
	CString			pcimciaName;
	CString         comNum;

	//检测是否有PCMCIA卡
	RegistryEx.ListValues (TEXT("HARDWARE\\DEVICEMAP\\SERIALCOMM"), strArray);
    for(int i=0; i<strArray.GetSize (); i++)
	{
		pcimciaName = strArray.GetAt(i);
	    if (pcimciaName.Find ("Serial",0) > 0)
		{
			RegistryEx.Open( HKEY_LOCAL_MACHINE, TEXT("HARDWARE\\DEVICEMAP\\SERIALCOMM"));
			int retCode = RegistryEx.Read (pcimciaName,comNum);
			RegistryEx.Close();
					
			if (retCode == FALSE)
				return 1;

			if(strcmp(comNum, "COM1") == 0 || strcmp(comNum, "COM2") == 0)
				continue;
			else
			{ 			
				break;
			}
		}
	}	
    if ( i >= strArray.GetSize ())
	{
		return 0;
	}


	//给系统参数赋值
	int nComIndex = 0;
	int len = comNum.GetLength () - 3;
	CString comIndex = comNum.Right (len);
	nComIndex = atoi(comIndex);

	CRegistryEx  reg2;
	int ret = reg2.Open (HKEY_LOCAL_MACHINE, _T("SOFTWARE\\Microsoft\\Windows NT\\CurrentVersion\\Ports"));
	ASSERT(ret != FALSE);

	reg2.Close ();

	return nComIndex;
}
#else
DWORD OptionsDetectPortOn2K()
{
	CRegistryEx		RegistryEx;
    DWORD			status = 0;
    CStringArray	strArray;
	CString			pcimciaName;
	CString         comNum;
	
	//检测是否有PCMCIA卡
	RegistryEx.ListValues (TEXT("HARDWARE\\DEVICEMAP\\SERIALCOMM"), strArray);
    for(int i=0; i<strArray.GetSize (); i++)
	{
		pcimciaName = strArray.GetAt(i);
	    if (pcimciaName.Find ("Oxser",0) > 0)
			break;
	}	
    if ( i >= strArray.GetSize ())
		return 0;
	else
		return 1;
}
#endif


PORTING_API
#ifdef USB_MODEM
void ChangePortSetting(DWORD nBaud)
{
}
#else
void ChangePortSetting(DWORD nBaud)
{
	OSVERSIONINFOEX osvi;
	ZeroMemory(&osvi, sizeof(OSVERSIONINFOEX));
	osvi.dwOSVersionInfoSize = sizeof(OSVERSIONINFOEX);

	GetVersionEx((OSVERSIONINFO *) &osvi);

	if(osvi.dwPlatformId == VER_PLATFORM_WIN32_NT)return;

	///WIN98下修改注册表
	HKEY hPortKey;
	HKEY hSubKey;
	CString strPosition = "SYSTEM\\CurrentControlSet\\Services\\Class\\Ports";
	
	LONG lReturn = ::RegOpenKeyEx( HKEY_LOCAL_MACHINE, 
		strPosition,
		0, KEY_READ | KEY_WRITE | KEY_QUERY_VALUE | KEY_SET_VALUE, &hPortKey);
	if(lReturn == ERROR_SUCCESS)
	{
		DWORD	dwSubKeys = 0;
		DWORD	dwValues = 0;
		TCHAR	szKeyStr[200];
		TCHAR	szValueStr[200];
		DWORD	dwSubKeyNameLen;
		DWORD	dwValueNameLen;
		DWORD	dwType;
		FILETIME	LastFileTime;
		
		lReturn = ::RegQueryInfoKey( hPortKey, NULL, NULL, NULL
			, &dwSubKeys, NULL, NULL, NULL, NULL, NULL, NULL, NULL );
		
		bool found = false;
		///查找对应的端口键
		for(DWORD i = 0; i < dwSubKeys; i++)
		{
			dwSubKeyNameLen = 200;
			lReturn = ::RegEnumKeyEx( 
				hPortKey
				,	i
				,	szKeyStr
				,	&dwSubKeyNameLen
				,	NULL
				,	NULL
				,	NULL
				,	&LastFileTime
				);
			
			lReturn = ::RegOpenKeyEx (HKEY_LOCAL_MACHINE, 
				strPosition + "\\" + szKeyStr,
				0L, KEY_READ | KEY_WRITE | KEY_QUERY_VALUE | KEY_SET_VALUE, &hSubKey);
			
			lReturn = ::RegQueryInfoKey( hSubKey, NULL, NULL, NULL
				, NULL, NULL, NULL, &dwValues, NULL, NULL, NULL, NULL );
			
			///查找对应的硬件标志
			for ( DWORD j = 0; j < dwValues; j++) 
			{
				dwValueNameLen = 200;
				lReturn = ::RegEnumValue( 
					hSubKey					// handle to key to query
					,	j					// index of value to query
					,	szValueStr				// address of buffer for value string
					,	&dwValueNameLen		// address for size of value buffer
					,	NULL				// reserved
					,	&dwType				// address of buffer for type code
					,	NULL				// address of buffer for value data
					,	NULL				// address for size of data buffer
					);
				
				///对比硬件标识
				CString strValue = szValueStr;
				strValue.TrimLeft();
				strValue.TrimRight();
				
				if(strValue == "MatchingDeviceId")
				{
					//char dwBuffer[255];        // bytes to allocate for buffers
					//DWORD dwBufferSize = 255;    // size of dwBuffer
					
					CString strHardware;
					DWORD dwType = 0;
					DWORD dwCount = 0;
					
					lReturn = ::RegQueryValueEx(hSubKey, 
						strValue,
						NULL, &dwType, NULL, &dwCount );
					
					lReturn = ::RegQueryValueEx(hSubKey, 
						strValue, 
						NULL, &dwType, (LPBYTE)strHardware.GetBuffer( dwCount / sizeof( TCHAR ) ), &dwCount );
					
					strHardware.TrimLeft();
					strHardware.TrimRight();
					
					BYTE spec115200[] = {
								   0x00,0x08,0x20,0x04,
								   0x04,0x70,0x40,0x78,
								   0x12,0x14,0x07,0x00,
								   0x08,0x00,0x00,0x1C};

					BYTE spec230400[] = {
								   0x00,0x08,0x20,0x04,
								   0x04,0x70,0x40,0x78,
								   0x12,0x14,0x07,0x00,
								   0x04,0x00,0x00,0x1C};

					const BYTE* spec = nBaud==115200? spec115200 : spec230400;
					
					for(int i=0;true; ++i)
					{
						if(strHardware == GetHardwareID(i))
						{
							lReturn = ::RegSetValueEx(hSubKey, "950Specific", 0, REG_BINARY, (const BYTE *)spec, 16);
							found = true;
							break;
						}
						if(GetHardwareID(i).IsEmpty())break;
					}
					if(found)break;
					strHardware.ReleaseBuffer();
				}
			}
			
			RegCloseKey (hSubKey);
			
			///找到对应的键,跳出
			//if(found)
			//	;//break;
		}
		
	}
			
	RegCloseKey(hPortKey);
}
#endif