// GlobalDataCenter.cpp: implementation of the CGlobalDataCenter class.
//
//////////////////////////////////////////////////////////////////////

#include "stdafx.h"
#include "GlobalDataCenter.h"
#include "Util.h"
#include "SunnyCatDlg.h"
#include "Porting.h"


#ifdef _DEBUG
#undef THIS_FILE
static char THIS_FILE[]=__FILE__;
#define new DEBUG_NEW
#endif

CString GetProduct()
{
	CFile file;
	CString strPath = GetAppPath() + "D-dur.dat";

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

	return strProduct;
}

CString GetLanguage()
{
	CString strPath = GetAppPath() + "D-dur.dat";
	
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

	CString str = buf;
	CString strLanguage = str.Right(3);

	CloseHandle(hFile);

	return strLanguage;
}

const CString&GetCurrentPath()
{
	static  std::string strPath ;
	static CString strCurPath;
	if(strPath.size() == 0)
	{
		strPath = __argv[0] ;
		strCurPath = ToUnicode(strPath).c_str();
		
		int nIndex = strCurPath.ReverseFind('\\') ;
		strCurPath = strCurPath.Left(nIndex+1);
	}
	return strCurPath;
}

void MultiLineString(CString & strSrc)
{
	strSrc.Replace(TEXT("\r\n"),TEXT("\n")) ;
	TCHAR szLine = 0x0A ;
	int nFind = strSrc.Find(szLine) ;
	bool bRepl = false ;
	while(nFind != -1)
	{
		strSrc.Insert(nFind,VK_RETURN) ;
		nFind = strSrc.Find(szLine,nFind+2) ;
		bRepl = true ;
	}
	strSrc.TrimRight(VK_RETURN) ;
	strSrc.TrimRight(szLine) ;
	if(!bRepl)
	{
		TCHAR szLine = 0x0d ;
		int nFind = strSrc.Find(szLine) ;
		while(nFind != -1)
		{
			strSrc.Insert(nFind+1,'\n') ;
			nFind = strSrc.Find(szLine,nFind+2) ;
		}
		strSrc.TrimRight(szLine) ;
		strSrc.TrimRight(VK_RETURN) ;
	}
}

//提示对话框
int GMessageBox(LPCTSTR lpMsgText,LPCTSTR lpCaption,DWORD dwFlags)
{
	//HWND hWnd = GetFocus() ;
	CGenMsgBox dlg ;
	dlg.m_dwProperty = dwFlags  ;
	dlg.m_strTitle   = lpMsgText;
	dlg.m_strCaption = lpCaption;
	int nRet = dlg.DoModal() ;
	//SetFocus(hWnd) ;
	return nRet ;
}




void SetAutoRunOnStart(bool bRunFlag)
{
	CString strProduct = GetProduct();
	TCHAR pszValue[50] = {0} ;
	
//	if(strProduct == "-p")	
//		_tcscpy(pszValue ,TEXT("EDGEManager")) ;
//	else if(strProduct == "-u") // USB_MODEM
		_tcscpy(pszValue , TEXT("WCDMA Modem")) ;			//修改了modem的值
//	else if(strProduct == "-c")
//		_tcscpy(pszValue , TEXT("M500ECOM")) ;

	TCHAR *pRegPath = TEXT("SOFTWARE\\Microsoft\\Windows\\CurrentVersion\\Run");
	HKEY hKey ;
	long lRet = RegOpenKey(HKEY_LOCAL_MACHINE,pRegPath,&hKey) ;
	
	if(!bRunFlag)
	{
		RegDeleteValue(hKey,pszValue) ;
		return ;
	}
	DWORD Reserved=0;      // reserved
	DWORD dwType = REG_SZ;        // flag for value type
	CString strRunName(__argv[0]) ;
	strRunName+="  /OnSysStart" ;
	std::string str = ToMultiBytes(LPCTSTR(strRunName));
	CONST BYTE *lpData = (BYTE *)(str.c_str());  // address of value data
	DWORD cbData = strRunName.GetLength() ;
	lRet = RegSetValueEx(hKey,pszValue, Reserved, dwType, lpData,cbData );
	
	RegCloseKey(hKey);	
}

BOOL SetVolume(DWORD dwSrcType, DWORD dwLValue, DWORD dwRValue, BOOL bMono)
{
	MIXERLINE mxl = {0};
//	if (! GetLineInfo(&mxl, MIXERLINE_COMPONENTTYPE_DST_SPEAKERS,dwSrcType))
//		return FALSE;

	MIXERCONTROL mxc = {0};
//	if (! GetLineControl(&mxc, &mxl, MIXERCONTROL_CONTROLTYPE_VOLUME))
//		return FALSE;

	MIXERCONTROLDETAILS			  mxcd = {0};
	MIXERCONTROLDETAILS_UNSIGNED  mxcd_u1;
	MIXERCONTROLDETAILS_UNSIGNED  mxcd_u[2];
	mxcd.cbStruct       = sizeof(mxcd);
	mxcd.dwControlID    = mxc.dwControlID;
	mxcd.cMultipleItems = 0;

	if (bMono)
	{
		mxcd.cChannels   = 1;
		mxcd.cbDetails   = sizeof(mxcd_u1);
		mxcd.paDetails   = &mxcd_u1;
		mxcd_u1.dwValue  = dwLValue;
	}
	else
	{
		mxcd.cChannels     = mxl.cChannels;
		mxcd.cbDetails     = sizeof(*mxcd_u);
		mxcd.paDetails     = mxcd_u;
		mxcd_u[0].dwValue  = dwLValue;
		mxcd_u[1].dwValue  = dwRValue;
	}
//	if (! SetControlDetails(&mxcd, MIXER_OBJECTF_MIXER))
//		return FALSE;
	return TRUE;
}

BOOL SetWaveVolume(DWORD dwLValue,DWORD dwRValue)
{
	return SetVolume(MIXERLINE_COMPONENTTYPE_SRC_WAVEOUT, dwLValue, dwRValue);
}

BOOL LoadImgList(CImageList*  g_pSysImageList, UINT nIDResource)
{	
	CBitmap IconBitmap;
	IconBitmap.LoadBitmap(nIDResource);
	g_pSysImageList->Create(25,25,ILC_MASK|ILC_COLOR24,1,1) ;
//	g_pSysImageList->Create(30,30,ILC_MASK|ILC_COLOR24,1,1) ;
	g_pSysImageList->Add( &IconBitmap,(COLORREF)0xFFFFFF );
	IconBitmap.DeleteObject();	
	return TRUE;
}

BOOL LoadImgList(CImageList*  pSysImageList, CString strResource)
{	
	CBitmap IconBitmap;

	BOOL bLoad = IconBitmap.Attach(LoadBmpFormFile(strResource));
	if(!bLoad)
	{
		return FALSE;
	}

	pSysImageList->Create(16,16,ILC_MASK|ILC_COLOR24,1,1) ;

	pSysImageList->Add( &IconBitmap,(COLORREF)0xFFFFFF );
	IconBitmap.DeleteObject();	
	return TRUE;
}

bool EnableKey(TCHAR key, const TCHAR keyTable[])
{
	for(size_t i=0; keyTable[i]; ++i)
	{
		if(keyTable[i] == key)return true;
	}
	return false;
}

/*BOOL GetDevCaps(LPMIXERCAPS pmxcaps)
{
    if (MMSYSERR_NOERROR != mixerGetDevCaps(m_uMxId, pmxcaps, sizeof(MIXERCAPS)))
        return FALSE;
    return TRUE;
}

BOOL GetVolume(DWORD dwSrcType, DWORD* pdwLValue, DWORD* pdwRValue, BOOL bMono)
{
    MIXERLINE   mxl;
    if (! GetLineInfo(&mxl, MIXERLINE_COMPONENTTYPE_DST_SPEAKERS, dwSrcType))
        return FALSE;
    MIXERCONTROL mxc;
    if (! GetLineControl(&mxc, &mxl, MIXERCONTROL_CONTROLTYPE_VOLUME))
        return FALSE;

    MIXERCONTROLDETAILS				mxcd;
    MIXERCONTROLDETAILS_UNSIGNED    mxcd_u1;
    MIXERCONTROLDETAILS_UNSIGNED    mxcd_u[2];

    mxcd.cbStruct       = sizeof(mxcd);
    mxcd.dwControlID    = mxc.dwControlID;
    mxcd.cMultipleItems = 0;

    if (bMono)
    {
        mxcd.cChannels  = 1;
        mxcd.cbDetails  = sizeof(mxcd_u1);
        mxcd.paDetails  = &mxcd_u1;
 
		if (! GetControlDetails(&mxcd, MIXER_GETCONTROLDETAILSF_VALUE))
            return FALSE;

		*pdwLValue = mxcd_u1.dwValue;
	}
	else
	{
		mxcd.cChannels   = mxl.cChannels;
		mxcd.cbDetails   = sizeof(*mxcd_u);
		mxcd.paDetails   = mxcd_u;
		if (! GetControlDetails(&mxcd, MIXER_GETCONTROLDETAILSF_VALUE))
			return FALSE;
		*pdwLValue = mxcd_u[0].dwValue;
		*pdwRValue = mxcd_u[1].dwValue;
	}
	return TRUE;
}

 

BOOL GetWaveVolume(DWORD* pdwLValue,DWORD* pdwRValue)
{
	return GetVolume(MIXERLINE_COMPONENTTYPE_SRC_WAVEOUT, pdwLValue, pdwRValue);
}

const CString&GetSysPath()
{
	static  CString strSysPath ;
	if(strSysPath.IsEmpty())
	{
		char szSPath[256] ;
		::GetSystemDirectory(szSPath,256) ;
		strSysPath = szSPath;
	}
	return strSysPath;
}
*/
HBITMAP  LoadBmpFormFile(LPCTSTR lpFileName) 
{
	if(lpFileName == NULL)
		return NULL;
	CString strFullPath(lpFileName) ;
	if(lpFileName[1] != ':')
	{
		strFullPath = GetAppPath()+lpFileName;
	//	strFullPath = "..\\res\\PhoneSkins\\" ;
	//	strFullPath += lpFileName;
	}
	int nCount = 0 ;
	HBITMAP hBitmap = NULL ;
	while(hBitmap == NULL)
	{
		nCount++;
		hBitmap = (HBITMAP)LoadImage(AfxGetInstanceHandle(),strFullPath,IMAGE_BITMAP,0,0,LR_LOADFROMFILE) ;
		if(nCount>4)
			break ;
		if(hBitmap == NULL)
			Sleep(1) ;
	}
	return hBitmap ;
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
		TCHAR szDevDesc[256] = {0};
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

void xJudgeDeviceChangePath(DWORD dwData, CString &strPath) 
{ 
	
	char i; 	
	for (i = 0; i < 26; ++i) 		
	{ 		
		if (dwData & 0x1) 			
		{ 			
			CString str = TCHAR(i+'A'); 			
			strPath = str+_T(":"); 
			
			break; 			
		} 
		
		dwData = dwData >> 1; 		
		if(dwData == 0x0000) 
			break; 
		
	} 
	
} 

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


	Sleep(5000);
	DWORD dwCount = GetTickCount();
	CString strModemName;
	strModemName.Format(TEXT(""));
	while(!((CSunnyCatDlg*)AfxGetMainWnd())->m_MobileManager.IsModemExisting(strModemName))
	{
		Sleep(1000);
		if((GetTickCount() - dwCount) > 5000)
			break;
	}
}

void LogString(const char* str, size_t size);
void LogString(const wchar_t* str,size_t size);
void LogString(CString strLog);

void SetWorkMode(CMobileManager& mgr,int nWorkType,const bool bInit)
{
	TRACE(TEXT("SetWorkMode :> start\n"));
	CString strVolume;
	
	switch(nWorkType)
	{
	case USETYPE_USBMODEM:
// 		if(!GetUsbDev("HUAWEI MOBILE  CONNECT"))
// 		{
// 			break;
// 		}
		TRACE(TEXT("SetWorkMode :> USETYPE_USBMODEM \n"));
//		if(mgr.m_pOwner->m_strVolume.IsEmpty())
		{
			TRACE(TEXT("SetWorkMode :> m_strVolume.IsEmpty \n"));
			
			TCHAR diskPath[5] = { 0 };

			DWORD allDisk = GetLogicalDrives(); 
			if (allDisk!=0)
			{
				for (int i=0;i<26;i++)     //假定最多有26个磁盘
				{
					TRACE(TEXT("SetWorkMode :> enter for\n"));
					if ((allDisk & 1)==1)
					{
						_stprintf( diskPath, TEXT("%c:"), 'C'+i );
						strVolume = diskPath;	
						TRACE(TEXT("SetWorkMode :> diskPath = %s\n"), diskPath);
						LogString(strVolume);
						if ( GetDriveType(diskPath)==DRIVE_CDROM )
						{
							TRACE(TEXT("SetWorkMode :> DRIVE_CDROM = %s\n"), diskPath);
							if( GetVolumeInformation(diskPath,0,0,0,0,0,0,0) ) //判断驱动是否准备就绪
							{
								TRACE(TEXT("SetWorkMode :> GetVolumeInformation DRIVE_CDROM = %s\n"), diskPath);
								
								WIN32_FIND_DATA   FindFileData   =   {0}; 
								CString strVolume = diskPath;
								HANDLE   hfile   =   ::FindFirstFile(strVolume+"\\philips_w\\dt",&FindFileData);   

								::Sleep(1000);
								CDRomOpen(TRUE,strVolume.GetAt(0));
								::Sleep(200);
//								break;
/*						
								if(hfile!=INVALID_HANDLE_VALUE)   
								{
									::FindClose(hfile);   

									SetCDToModem(mgr,strVolume,bInit);

									break;
								}
*/								TRACE(TEXT("SetWorkMode :> GetVolumeInformation DRIVE_CDROM over\n"));
							}
						}									
						
					}
					
					allDisk = allDisk>>1;
				}
				if(i>=26)
				{
					for (int i=0;i<24;i++)     //假定最多有26个磁盘
					{
						_stprintf( diskPath, TEXT("%c:"), 'C'+i );
						CString Volume = diskPath;					
						WIN32_FIND_DATA   FindFileData   =   {0}; 
						CString strVolume = diskPath;
						LogString(strVolume);
						HANDLE   hfile   =   ::FindFirstFile(strVolume+"\\philips_w\\dt",&FindFileData);   
						
						if(hfile!=INVALID_HANDLE_VALUE)   
						{
							::FindClose(hfile);   
							
							SetCDToModem(mgr,strVolume);
						}
		
					}
				}
			}

		}
//		else
//		{
//			::Sleep(1000);
//			CDRomOpen(TRUE,mgr.m_pOwner->m_strVolume.GetAt(0));
//			::Sleep(200);
//			SetCDToModem(mgr,mgr.m_pOwner->m_strVolume,bInit);
//		}

		break;

	default:
		break;
	}
	TRACE(TEXT("SetWorkMode :> end\n"));
}



void   CDRomOpen(BOOL   bOpenDrive,TCHAR   cDrive) 
{
	TRACE(TEXT("CDRomOpen :> START\n"));
	MCI_OPEN_PARMS   open; 
	MCI_STATUS_PARMS   status; 
	DWORD   flags; 
	TCHAR   szDriveName[4] = {0}; 
	_tcscpy(szDriveName, TEXT("?: ")); 
	::ZeroMemory(&open,sizeof(MCI_OPEN_PARMS)); 
	open.lpstrDeviceType=(LPCTSTR)MCI_DEVTYPE_CD_AUDIO; 
	szDriveName[0]=cDrive; 
	open.lpstrElementName=szDriveName; 
	flags=MCI_OPEN_TYPE|MCI_OPEN_TYPE_ID|MCI_OPEN_ELEMENT|MCI_OPEN_SHAREABLE; 
	if(!mciSendCommand(0,MCI_OPEN,flags,(unsigned   long)&open))
	{ 
		TRACE(TEXT("CDRomOpen :> ENTER IF !mciSendCommand\n"));
		status.dwItem=MCI_STATUS_READY; 
		if(bOpenDrive)
		{
			TRACE(TEXT("CDRomOpen :> ENTER IF bOpenDrive = %d\n"), bOpenDrive);
			MCIERROR  error = mciSendCommand(open.wDeviceID,MCI_SET,MCI_SET_DOOR_OPEN|MCI_WAIT,0); 
			CString str;
			str.Format(TEXT("CDROM Open:%ld"),error);
			LogString(str,str.GetLength());
		}
		else 
			mciSendCommand(open.wDeviceID,MCI_SET,MCI_SET_DOOR_CLOSED,0); 
		MCIERROR  error = mciSendCommand(open.wDeviceID,MCI_CLOSE,MCI_WAIT,0); 
		CString str;
		str.Format(TEXT("CDROM Wait:%ld"),error);
		LogString(str,str.GetLength());
	} 
	else
	{
		CString str;
		str.Format(TEXT("OPen Fail"),10);
		LogString(str,str.GetLength());
	}
	TRACE(TEXT("CDRomOpen :> END\n"));
} 



void SetCDToModem(CMobileManager& mgr,const CString strVolume,const bool bInit)
{
	TRACE(TEXT("SetCDToModem:volume=%s\n"),strVolume);

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
	
	int n = 5;
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
	
	if(!bInit)
		mgr.m_pOwner->SetTimer(TIMER_RESET_MODEM,10000,NULL);
	else
	{
 		::Sleep(5000);
		int n =10;
		while(n-->=0)
		{
			CString strModemName;
			strModemName.Format(TEXT(""));
			if(mgr.m_pOwner->m_MobileManager.IsModemExisting(strModemName))
			{
				break;
 			}
			Sleep(1000);
		}
		//if(!((CSunnyCatDlg*)AfxGetMainWnd())->m_MobileManager.IsModemExisting(GetModemName()))
		//	GMessageBox(LoadStringEx(IDS_CDROM_REINSTALL));
	}
}