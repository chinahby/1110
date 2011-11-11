#include "stdafx.h"
#include "Execute.h"
#include "Thread.h"
#include "PCMCIA-Helper.h"
#include "SignatureMonitor.h"
#include "newdev.h"
#include "../../DevEnum.h"
#include "DLLModule.h"
#include "UChg.h"
#include <atlbase.h>
#include "ChgModemThread.h"
#include <mmsystem.h> 
#include "OSVersion.h"
#include <windows.h>
#include <atlbase.h>
#include "devioctl.h"
#include "ntddscsi.h"
#include "spti.h"

bool g_bFindHW = false;
extern CString g_strVolume;

HANDLE RunProgram(const CString& strCmdLine, bool bShowWindow);
CString GetAppPath();
int ShowMessageBox(UINT strid, UINT nType = MB_OK);
void installModemInitialAT(CString strText);



bool HardwareExisting(CString& strHWID);



typedef const CString& (*FUNC_GetHardwareID)(int i);

class CThreadUSBInstall: public CThread
{
public :
	bool m_DetectOK;
private:
	CString m_strHWID;
	CString m_strDstDir;
public:
	CThreadUSBInstall(const CString& strDst)
		:m_DetectOK(false),
		m_strDstDir(strDst)
	{
	}
protected:
	virtual void Go()
	{
		m_DetectOK = true;		
		
		CDllModule dll(m_strDstDir+"porting.dll");
		
		FUNC_GetHardwareID GetHardwareID = (FUNC_GetHardwareID)dll.GetFuncAdd("?GetHardwareID@@YAABVCString@@H@Z");
		if(!GetHardwareID)
		{
			return ;
		}
		DWORD dwTime = GetTickCount();
		bool bFound = false;
		CString strHWID;
		while(GetTickCount()-dwTime < 120000 && !bFound)
		{
			for(int i=0;true;++i)
			{

#ifdef CDROM 
		//		SetWorkMode(USETYPE_USBMODEM,g_strVolume);
#endif
				strHWID  = GetHardwareID(i);
				
				TRACE("%s\n",strHWID);
				if(strHWID.IsEmpty())break;
				if(HardwareExisting(strHWID))
				{
					g_bFindHW = true;
					bFound = true;
					break;
				}
			}
			if(bFound)break;
			Sleep(200);
		}
		if(!bFound)
		{
		
			ShowMessageBox(IDS_NOTINSERT_FAIL, MB_OK|MB_ICONINFORMATION);
		
			return;
		}

		m_strHWID = strHWID;
		m_DetectOK = SetupDriver();

		if(m_DetectOK)
		{
			ShowMessageBox(IDS_DRVIER_INSTALL_OK, MB_OK|MB_ICONINFORMATION);
		}
		else
		{
			ShowMessageBox(IDS_RESTART_PC, MB_OK|MB_ICONINFORMATION);
		}
	}

private:
	bool SetupDriver()
	{
		TRACE("SetupDriver Setup start\n");
		static CSignatureMonitor sigMon;
		static CWizardMonitor	wizMon;
		
		wizMon.Start();
		sigMon.Start();

		wizMon.WaitForComplete(15000);

		typedef BOOL (WINAPI *FUNC_UpdateDriverForPlugAndPlayDevices)
			(
			HWND hwndParent,
			LPCSTR HardwareId,
			LPCSTR FullInfPath,
			DWORD InstallFlags,
			PBOOL bRebootRequired
			);

		CDllModule dll("newdev.dll");

		FUNC_UpdateDriverForPlugAndPlayDevices UpdateDriver = (FUNC_UpdateDriverForPlugAndPlayDevices)dll.GetFuncAdd("UpdateDriverForPlugAndPlayDevicesA");
		if(!UpdateDriver)
		{
			return false;
		}
		bool bOK = false;
		

		OSVERSIONINFOEX osvi;
		ZeroMemory(&osvi, sizeof(OSVERSIONINFOEX));
		osvi.dwOSVersionInfoSize = sizeof(OSVERSIONINFOEX);

		GetVersionEx((OSVERSIONINFO *) &osvi);
		CString strMdmUsb;
		//vista
		if ( osvi.dwMajorVersion > 5)
		{
			if(m_strHWID.Find("0471") > 0)
				strMdmUsb = m_strDstDir + "driver\\Windows_Vista-32\\MdmUsb.inf";
			if(m_strHWID.Find("5421") > 0)
				strMdmUsb = m_strDstDir + "driver\\Windows_Vista-32\\USB_WeWins.inf";
		}
		else
		{
			if(m_strHWID.Find("0471") > 0)
				strMdmUsb = m_strDstDir + "driver\\Windows_XP-32\\MdmUsb.inf";
			if(m_strHWID.Find("5421") > 0)
				strMdmUsb = m_strDstDir + "driver\\Windows_XP-32\\USB_WeWins.inf";
		}
		bOK = UpdateDriver(NULL,
						m_strHWID,
						strMdmUsb,
						INSTALLFLAG_FORCE, 0);



		return bOK;
	}

	
};


bool UninstallUSB_2KXP(const CString& strDestDir);
CString LoadStringEx(UINT id);
/** 2000/XP下的安装
*/

void SendSCSI(char* Drive, int num)
{
    ULONG length = 0;
    ULONG errorCode = 0;
    ULONG returned = 0;
    ULONG sectorSize = 512;
    BOOL status = 0;
    HANDLE fileHandle = NULL;
    PUCHAR dataBuffer = NULL;
    SCSI_PASS_THROUGH_WITH_BUFFERS sptwb;
    DWORD accessMode = 0, shareMode = 0;
    char fileman[MAX_PATH + 1] = {0};
	char temp[3] = {0};
	
	strncpy(temp,Drive,2);;
    strcpy(fileman,"\\\\.\\");
	strcat(fileman,temp);

    shareMode = FILE_SHARE_READ | FILE_SHARE_WRITE;  // default
    accessMode = GENERIC_WRITE | GENERIC_READ;       // default
    
    fileHandle = CreateFile(fileman,
        accessMode,
        shareMode,
        NULL,
        OPEN_EXISTING,
        0,
        NULL);

    if (fileHandle == INVALID_HANDLE_VALUE) {
        return;
    }

    ZeroMemory(&sptwb,sizeof(SCSI_PASS_THROUGH_WITH_BUFFERS));
    sptwb.spt.Length = sizeof(SCSI_PASS_THROUGH);
    sptwb.spt.PathId = 0;
    sptwb.spt.TargetId = 1;
    sptwb.spt.Lun = 0;
    sptwb.spt.CdbLength = CDB10GENERIC_LENGTH;
    sptwb.spt.SenseInfoLength = 24;
    sptwb.spt.DataIn = SCSI_IOCTL_DATA_IN;
    sptwb.spt.DataTransferLength = 0;
    sptwb.spt.TimeOutValue = 2;
    sptwb.spt.DataBufferOffset =
        offsetof(SCSI_PASS_THROUGH_WITH_BUFFERS,ucDataBuf);
    sptwb.spt.SenseInfoOffset =
        offsetof(SCSI_PASS_THROUGH_WITH_BUFFERS,ucSenseBuf);
    sptwb.spt.Cdb[0] = SCSIOP_TRANSPARENT;
    sptwb.spt.Cdb[1] = 0x0;
    sptwb.spt.Cdb[2] = 0x03;
    sptwb.spt.Cdb[3] = 0x24;
    sptwb.spt.Cdb[4] = 0x01;
    sptwb.spt.Cdb[5] = (unsigned char)num;
    sptwb.spt.Cdb[6] = 0x0;
    sptwb.spt.Cdb[7] = 0x0;
    sptwb.spt.Cdb[8] = 0x0;
    sptwb.spt.Cdb[9] = 0x0;
    length = offsetof(SCSI_PASS_THROUGH_WITH_BUFFERS,ucDataBuf);

    status = DeviceIoControl(fileHandle,
        IOCTL_SCSI_PASS_THROUGH,
        &sptwb,
        sizeof(SCSI_PASS_THROUGH),
        &sptwb,
        length,
        &returned,
        FALSE); 
    CloseHandle(fileHandle);
    
    return;
}


void SendAllSCSI(int num)
{ 
  DWORD  dwBuffer=256;   
  char   szBuffer[256];   

  memset(szBuffer, 0, dwBuffer);   
  int nSize = GetLogicalDriveStrings(dwBuffer, szBuffer);   
  if (nSize == 0)   
  {   
	 return;   
  }   
       
  char szTemp[4];   
  memset(szTemp, 0, 4);   
  int j = 0;   
  int nLength = 0;

  for (int i = 0; i < nSize;   i++)   
  {   
	  if(szBuffer[i] == '\0')   
	  {   
		  szTemp[j]='\0';     
		    
		  UINT nDriver = GetDriveType(szTemp);     
          SendSCSI(szTemp, num);

		  switch(nDriver)   
		  {   
			  case   DRIVE_REMOVABLE:
		      break;

			  case   DRIVE_CDROM:
			  break;   

			  case   DRIVE_FIXED:
			  break;   
			    
			  case   DRIVE_REMOTE:   
			  break;   

			  case   DRIVE_RAMDISK:   
			  break;   
		  }   

          nLength++;   
		  j=0;   
		  continue;   
	  } 

	  szTemp[j]=szBuffer[i];   
	  j++;   
  }   
     
  return;    
}


bool InstallUSB_2KXP(const CString& strDestDir)
{
	//UninstallUSB_2KXP(strDestDir);//首先删除已有的Driver
	TRACE(TEXT("InstallUSB_2KXP :> START"));
	TRACE(TEXT("InstallUSB_2KXP"));
	TRACE(TEXT("InstallUSB_2KXP\n :> CThreadUSBInstall :>"));
	
	GetCDROMNumber();

//	mciSendString("set cdaudio door closed",NULL,0,0);
/*	
	int tmp = mciSendString("set cdaudio door open",0,0,NULL);     //弹开光驱
	TRACE(TEXT("InstallUSB_2KXP\n :> tmp = %d\n"),tmp);
	tmp = mciSendString("set cdaudio door open",0,0,NULL);     //弹开光驱
	TRACE(TEXT("InstallUSB_2KXP\n :> tmp = %d\n"),tmp);
	tmp = mciSendString("set cdaudio door open",0,0,NULL);     //弹开光驱
	TRACE(TEXT("InstallUSB_2KXP\n :> tmp = %d\n"),tmp);
*/
//这段没有起到作用，要重写
/************************************************************************/

//

//	Sleep(4000);
	TRACE(TEXT("InstallUSB_2KXP\n :> Sleep Over\n"));
	CString strCmd1;

	#pragma message("********不同版本要进行修改********")
//	TRACE(TEXT("InstallUSB_2KXP\n :> IsX86Platform\n"));
	strCmd1 = strDestDir+"driver\\WCDMAInstall.exe";
/*	
	// 判断平台
	if(COSVersion::Is64BitsPlatform())
	{
		#pragma message("*********3GWIRELESS/19f5,9909************")
		#pragma message("********不同版本要进行修改********")
		TRACE(TEXT("InstallUSB_2KXP\n :> Is64BitsPlatform\n"));
		strCmd1 = strDestDir+"driver\\3GWIRELESSAMD64.exe";
	}
	else
	{	
		#pragma message("********不同版本要进行修改********")
		TRACE(TEXT("InstallUSB_2KXP\n :> IsX86Platform\n"));
		strCmd1 = strDestDir+"driver\\3GWIRELESSInstall.exe";
	}
*/
	TRACE(TEXT("InstallUSB_2KXP\n :> \n"));
	TRACE(TEXT("InstallUSB_2KXP"));
	TRACE(TEXT("InstallUSB_2KXP\n :> set CDAudio door open"));

	HANDLE hProcess1 = RunProgram(strCmd1,true);
	if(!hProcess1)return false;
	WaitForSingleObject(hProcess1, INFINITE);
	CloseHandle(hProcess1);
/************************************************************************/

/*
	CThreadUSBInstall thread(strDestDir);
	thread.Start();
#ifdef UDISK
#elif CDROM 
#else
	MessageBox(AfxGetApp()->m_pMainWnd->GetSafeHwnd(),
		LoadStringEx(IDS_INSERTDEVICE_AGAIN), 
		"",
		MB_OK|MB_ICONINFORMATION);
#endif	
	::Sleep(1000);
	static CChgModemThread cchg;
	cchg.Start();

	thread.WaitForComplete(180000);
*/
	TRACE(TEXT("InstallUSB_2KXP :> END"));
	return true;			//thread.m_DetectOK;
}

/** 2000/XP下的卸载
*/
bool UninstallUSB_2KXP(const CString& strDestDir)
{
	#pragma message("********不同版本要进行修改********")
	TRACE(TEXT("UninstallUSB_2KXP :> START"));
	HANDLE hProcess = RunProgram(strDestDir+"driver\\WCDMAUnInstall.exe", false);
	if(!hProcess)return false;

	WaitForSingleObject(hProcess, INFINITE);
	TRACE(TEXT("UninstallUSB_2KXP :> END"));
	return true;
}

void installModemInitialAT(CString strText)
{
	
	CRegKey reg;
	
    HKEY hKey ;
	CString strSubKey = "SYSTEM\\CurrentControlSet\\Control\\Class\\{4D36E96D-E325-11CE-BFC1-08002BE10318}";
	long lResult = RegOpenKey(HKEY_LOCAL_MACHINE,strSubKey,&hKey);
	CString strKey = strSubKey + "\\";

	//GetModemConfigString() + "\\0000"; 
	DWORD dwIndex = 0;
	TCHAR strName[50] = {0};
	DWORD dwNameLen = 50;
	FILETIME ft;
	BOOL bLoop = TRUE;
	CString strModemName;
	strModemName = "WCDMA Modem";

    while(bLoop)
	{
		dwNameLen = 50;
		ZeroMemory(strName,sizeof(strName));
		lResult = RegEnumKeyEx(hKey,dwIndex++,strName,&dwNameLen,NULL,NULL,NULL,&ft);
		if(lResult == ERROR_NO_MORE_ITEMS)
			break;
		CString strMKey = "";
		strMKey = strKey;
		strMKey += strName;

		if(reg.Open(HKEY_LOCAL_MACHINE,strMKey) != ERROR_SUCCESS)	
		{
			
		}  
		
		TCHAR strKeyValue[50] = {0}; 
		DWORD dwCount = 50;
		long lRet = reg.QueryValue(strKeyValue,"Model",&dwCount);

		if(strKeyValue == strModemName )//(CString)"WeWins Modem"
		{
			CString str;
			str.Empty();
			str.Format("AT+CGDCONT=1,\"IP\",\"%s\"",strText);
			reg.SetValue(str, "UserInit");

			bLoop = FALSE;
		}

		reg.Close();

	}

	RegCloseKey(hKey);
	
}
