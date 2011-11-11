#pragma once

enum EUseType
{
	USETYPE_USBMODEM = 0,
	USETYPE_UDISK,
	USETYPE_CDROM
};


void SetWorkMode(int nWorkType,const CString& strVolume);
void SetDevToModem(CString strVolume);
void SetCDToModem(CString strVolume);
void   CDRomOpen(BOOL   bOpenDrive,TCHAR   cDrive);
bool IsModemExisting(CString strModem,CString& strCOM);
void GetCDROMNumber();
void SetDevUDisk(CString& strCOM);

bool GetUsbDev(const CString& hwid);
