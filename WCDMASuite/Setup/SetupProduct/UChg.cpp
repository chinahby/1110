#include "StdAfx.h"
#include "UChg.h"
#include "../../DevEnum.h"
#include "../../comparser.h"
#include <setupapi.h>
#include <mmsystem.h>
#include "Util.h"
#include "resource.h"
#include <iostream.h>
#include <string>


void SetDevToModem(CString strVolume)
{
	
	
	CString strPath;
	
	strPath = strVolume + "\\WeWins.txt";
	
	CFile file(strPath,CFile::modeCreate|CFile::modeReadWrite);
	
	
	BYTE pBuf[20] = { 0x19,0x71,0x10,0x03,0x77,0x2d,0x65,0x77,0x63,0x73,0x6e,0x69,0x5a,0x5a,0x5a,0x5a };
	file.Write(pBuf,16);
	
	file.Close();
	Sleep(300);
	
	CFile file2(strPath,CFile::modeCreate|CFile::modeReadWrite);
	CString strbuf;
	
	BYTE pBuf2[20] ={0x73,0x71,0x65,0x72,0x63,0x74,0x69,0x77,0x6d,0x6f,0x74,0x68,0x6d,0x65,0x64,0x6f,0x05,0x00,0x00,0x00};
	file2.Write(pBuf2,20);
	file2.Close();
	Sleep(300);
	
	CFile file3(strPath,CFile::modeCreate|CFile::modeReadWrite);
	
	BYTE pBuf3[20] = { 0x19,0x71,0x10,0x03,0x77,0x2d,0x65,0x77,0x63,0x73,0x6e,0x69,0x5a,0x5a,0x5a,0x5a };
	file3.Write(pBuf3,16);
	
	file3.Close();
	Sleep(300);
	
	CFile file4(strPath,CFile::modeCreate|CFile::modeReadWrite);
	
	BYTE pBuf4[20] = { 0x72,0x71,0x65,0x72,0x74,0x65,0x73,0x65,0x6d,0x65,0x68,0x74,0x6d,0x65,0x64,0x6f };
	file4.Write(pBuf4,16);
	
	file4.Close();

	strVolume.Empty();
}

void WriteLog(const CString strlog)
{
	/*try
	{
		CFile file(GetAppPath()+"log.txt",CFile::modeCreate|CFile::modeNoTruncate|CFile::modeReadWrite);

		CString strbuf;
		strbuf = strlog;
		strbuf += "\r\n";
		file.SeekToEnd();
	
		file.Write(strbuf.GetBuffer(strbuf.GetLength()),strbuf.GetLength());
		file.Close();
	}
	catch (CFileException& fe)
	{
		cout<<fe.ReportError() <<endl;
	}*/
}


void GetCDROMNumber()
{
	TRACE(TEXT("GetCDROMNumber :> start\n"));
	char diskPath[5] = { 0 };

	DWORD allDisk = GetLogicalDrives(); 
	DWORD dwDiskBk = allDisk;
	if (allDisk!=0)
	{
		TRACE(TEXT("GetCDROMNumber :> enter if\n"));
		for (int i=0;i<26;i++)     //假定最多有26个磁盘
		{
			TRACE(TEXT("GetCDROMNumber :> enter for\n"));
			if ((allDisk & 1)==1)
			{
				sprintf( diskPath, "%c:", 'C'+i );
				TRACE(TEXT("GetCDROMNumber :> diskPath = %s\n"), diskPath);
				CString Volume = diskPath;
				if ( GetDriveType(diskPath) == DRIVE_CDROM )
				{
					TRACE(TEXT("GetCDROMNumber :> DRIVE_CDROM = %s\n"), diskPath);
					if( GetVolumeInformation(diskPath,0,0,0,0,0,0,0) ) //判断驱动是否准备就绪
					{
						TRACE(TEXT("GetCDROMNumber :> GetVolumeInformation DRIVE_CDROM = %s\n"), diskPath);
//						WIN32_FIND_DATA   FindFileData   =   {0}; 
						CString strVolume = diskPath;
//						HANDLE   hfile   =   ::FindFirstFile(strVolume+"\\philips_w\\dt",&FindFileData);   

						::Sleep(1000);
						CDRomOpen(TRUE,strVolume.GetAt(0));
						::Sleep(200);
/*
						if(hfile != INVALID_HANDLE_VALUE)
						{
							TRACE(TEXT("GetCDROMNumber :> INVALID_HANDLE_VALUE = %s\n"), diskPath);
							::FindClose(hfile);

							break;
						}
*/					}
				}
			}
			allDisk = allDisk>>1;
		}
	}
	TRACE(TEXT("GetCDROMNumber :> end\n"));
}


void SetWorkMode(int nWorkType,const CString& strVolume)
{
	switch(nWorkType)
	{
	case USETYPE_USBMODEM:
		if(strVolume.IsEmpty())
		{
			char diskPath[5] = { 0 };

			DWORD allDisk = GetLogicalDrives(); 
			DWORD dwDiskBk = allDisk;
			if (allDisk!=0)
			{
				for (int i=0;i<26;i++)     //假定最多有26个磁盘
				{
					if ((allDisk & 1)==1)
					{
						sprintf( diskPath, "%c:", 'C'+i );
						CString Volume = diskPath;
#ifdef CDROM 
						if ( GetDriveType(diskPath)==DRIVE_CDROM )
						{
						
							if( GetVolumeInformation(diskPath,0,0,0,0,0,0,0) ) //判断驱动是否准备就绪
							{
								WIN32_FIND_DATA   FindFileData   =   {0}; 
								CString strVolume = diskPath;
								HANDLE   hfile   =   ::FindFirstFile(strVolume+"\\philips_w\\dt",&FindFileData);   
								
								if(hfile!=INVALID_HANDLE_VALUE)   
								{
									::FindClose(hfile);

									SetCDToModem(strVolume);

									break;
								}
							}
						}
#endif
					}
					
					allDisk = allDisk>>1;
				}
				if(i>=26)
				{
					for (int i=0;i<24;i++)     //假定最多有26个磁盘
					{					
						sprintf( diskPath, "%c:", 'C'+i );
						CString Volume = diskPath;

						if( GetVolumeInformation(diskPath,0,0,0,0,0,0,0) ) //判断驱动是否准备就绪
						{
							WIN32_FIND_DATA   FindFileData   =   {0}; 
							CString strVolume = diskPath;
							HANDLE   hfile   =   ::FindFirstFile(strVolume+"\\philips_w\\dt",&FindFileData);   
							
							if(hfile != INVALID_HANDLE_VALUE)   
							{
								::FindClose(hfile);   
								
								SetCDToModem(strVolume);
							}
						}		
					}
				}
			}

		}
		else
		{
#ifdef UDISK		
			SetDevToModem(strVolume) ;
#elif CDROM 
			SetCDToModem(strVolume);
#endif
		}

		break;
	
	default:
		break;
	}

}

bool IsModemExisting(CString strModem,CString& strCOM)
{	
	CModemList modemList;
	strCOM.Empty();	
	std::vector<size_t> vMatched;
	for(size_t i=0; i<modemList.Count(); ++i)
	{
		const TModemInfo& info = modemList.GetAt(i);
		if(info.strFriendlyName == strModem )//GetModemName()
		{
			strCOM = info.strPortAttached;
			return true;
		}
	}
	return false;
}

/*
void SetDevUDisk(CString& strCOM)
{
	IATManager *pMgr = CreateATManager(LPCTSTR(strCOM),115200);
	if(pMgr != NULL)
	{
		EATReturnCode RetCode;
		pMgr->GetCommonMgrIC().TestComATOK(RetCode);
		if(RetCode != RETURN_OK) return;
		std::string strswrv =  pMgr->GetCommonMgrIC().QuerySWRV(RetCode);
		if((strswrv.find("M580e_E.CB1.09D.04.01.20") != std::string::npos)
			||(strswrv.find("M580e_E.CB1.09C.04.01.20") != std::string::npos))
			return;
		pMgr->GetCommonMgrIC().SetDevToUDisk(RetCode);
	}
	ReleaseATManager(pMgr);
}
*/
void   CDRomOpen(BOOL   bOpenDrive,TCHAR   cDrive) 
{
	TRACE(TEXT("CDRomOpen :> start\n"));
	MCI_OPEN_PARMS   open; 
	MCI_STATUS_PARMS   status; 
	DWORD   flags; 
	TCHAR   szDriveName[4] = {0}; 
	strcpy(szDriveName, "?: "); 
	::ZeroMemory(&open,sizeof(MCI_OPEN_PARMS)); 
	open.lpstrDeviceType=(LPCSTR)MCI_DEVTYPE_CD_AUDIO; 
	szDriveName[0]=cDrive; 
	open.lpstrElementName=szDriveName; 
	flags=MCI_OPEN_TYPE|MCI_OPEN_TYPE_ID|MCI_OPEN_ELEMENT|MCI_OPEN_SHAREABLE; 
	if(!mciSendCommand(0,MCI_OPEN,flags,(unsigned   long)&open))
	{
		TRACE(TEXT("CDRomOpen :> enter if\n"));
		status.dwItem=MCI_STATUS_READY; 
		if(bOpenDrive) 
			mciSendCommand(open.wDeviceID,MCI_SET,MCI_SET_DOOR_OPEN,0); 
		else 
			mciSendCommand(open.wDeviceID,MCI_SET,MCI_SET_DOOR_CLOSED,0); 
		mciSendCommand(open.wDeviceID,MCI_CLOSE,MCI_WAIT,0); 
	}
	TRACE(TEXT("CDRomOpen :> end\n"));
} 

void SetCDToModem(const CString strVolume)
{
	CString strPath;
	strPath = strVolume+"\\PHILIPS_W\\dt";
	WIN32_FIND_DATA   FindFileData   =   {0};
	HANDLE   hfile   =   ::FindFirstFile(strPath,&FindFileData);   
	
	if(hfile==INVALID_HANDLE_VALUE)   
	{
		return;	   
	}
	::FindClose(hfile);
	
	::Sleep(1000);
	CDRomOpen(TRUE,strVolume.GetAt(0));
	::Sleep(200);
	
	size_t n = 5;
	while(n-->=0)
	{
		CString strPath = strVolume+"\\PHILIPS_W\\dt";
		WIN32_FIND_DATA   FindFileData   =   {0};
		HANDLE   hfile   =   ::FindFirstFile(strPath,&FindFileData);   
									
		if(hfile==INVALID_HANDLE_VALUE)
			break;
		
		CDRomOpen(TRUE,strVolume.GetAt(0));
		::Sleep(2000);		
	}
}

bool GetUsbDev(const CString& hwid)
{

	//class ID of ComPorts
	bool bRet = false;
#ifdef CDROM
	const GUID  PortsGUID = {0x4d36e965L, 0xe325, 0x11ce, {0xbf, 0xc1, 0x08, 0x00, 0x2b, 0xe1, 0x03, 0x18}};
#else
	const GUID  PortsGUID = {0x4d36e967L, 0xe325, 0x11ce, {0xbf, 0xc1, 0x08, 0x00, 0x2b, 0xe1, 0x03, 0x18}};
#endif
	HDEVINFO hDevInfo;
	hDevInfo = SetupDiGetClassDevs((LPGUID)&PortsGUID, NULL, NULL, DIGCF_PRESENT);
	
	if ( INVALID_HANDLE_VALUE == hDevInfo)
		return false;

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
			SPDRP_FRIENDLYNAME,
			NULL,
			(PBYTE)szDevDesc,
			sizeof(szDevDesc),
			NULL))
		{
			strPort = szDevDesc;
			strPort.MakeUpper();
			if(strPort.Find(hwid) !=-1)
				bMatch = true;
		}

		if(bMatch)
		{
			bRet = true;
			break;
		}

	}
	SetupDiDestroyDeviceInfoList(hDevInfo);

	return bRet;
}


