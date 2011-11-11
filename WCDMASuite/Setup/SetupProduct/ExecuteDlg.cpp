// ExecuteDlg.cpp : implementation file
//

#include "stdafx.h"
#include "Execute.h"
#include "ExecuteDlg.h"
#include "StringTable.h"
#include "DLLModule.h"
#include "SelComDlg.h"
#include <atlbase.h>
#include <DBT.h>
#include <setupapi.h>

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

CString g_strCom;
CString g_strLanguage;
CString g_strVolume;
/////////////////////////////////////////////////////////////////////////////
// CExecuteDlg dialog

CExecuteDlg::CExecuteDlg(CWnd* pParent /*=NULL*/)
	: CDialog(CExecuteDlg::IDD, pParent)
{
	//{{AFX_DATA_INIT(CExecuteDlg)
		// NOTE: the ClassWizard will add member initialization here
	//}}AFX_DATA_INIT
	// Note that LoadIcon does not require a subsequent DestroyIcon in Win32
	m_hIcon = AfxGetApp()->LoadIcon(IDR_MAINFRAME);

	m_IsInstall = false;
}

void CExecuteDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	//{{AFX_DATA_MAP(CExecuteDlg)
		// NOTE: the ClassWizard will add DDX and DDV calls here
	//}}AFX_DATA_MAP
}

BEGIN_MESSAGE_MAP(CExecuteDlg, CDialog)
	//{{AFX_MSG_MAP(CExecuteDlg)
	ON_WM_PAINT()
	ON_WM_QUERYDRAGICON()
	ON_MESSAGE(WM_DEVICECHANGE, OnDeviveChange)
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CExecuteDlg message handlers
bool ParseCmdLn(bool& bInstall, 
				CString& strProduct, 
				CString& strLanguage,
				CString& strDestPath);
void ShowUsage();

BOOL CExecuteDlg::OnInitDialog()
{
	CDialog::OnInitDialog();

	// Set the icon for this dialog.  The framework does this automatically
	//  when the application's main window is not a dialog
	SetIcon(m_hIcon, TRUE);			// Set big icon
	SetIcon(m_hIcon, FALSE);		// Set small icon

	if(!m_wndProgressBar.Create(
			NULL,
			NULL,
			WS_CHILD | WS_VISIBLE , 
			CRect(0,0,100,100), 
			this,
			1234,
			NULL))
	{
		return false;
	}

	CRect rc;
	GetClientRect(rc);
	rc.left += 60;
	rc.right -= 7;
	int W= rc.Width();
	if(W%4)W=W-(W%4);
	CRect rcWnd(rc.left, rc.CenterPoint().y-6, rc.left+W, rc.CenterPoint().y+8);
	m_wndProgressBar.MoveWindow(rcWnd);


	if(!ParseCmdLn(m_IsInstall, m_strProduct, m_strLanguage, m_strInstallPath))
	{	
		ShowUsage();
		PostMessage(WM_CLOSE, 0, 0);
		
		return TRUE;
	}
	TRACE(TEXT("CExecuteDlg::OnInitDialog()\nm_IsInstall:>%d\nm_strProduct:>%s\nm_strLanguage:>%s"),m_IsInstall);
/*	if(m_strProduct == "-c")
	{
		if(m_IsInstall)
		{
			CSelComDlg dlg;
			if(dlg.DoModal() == IDOK)
			{
				g_strCom = dlg.m_strCom;
			}
		}
	}
*/

	Start();
	
	return TRUE;  // return TRUE  unless you set the focus to a control
}

// If you add a minimize button to your dialog, you will need the code below
//  to draw the icon.  For MFC applications using the document/view model,
//  this is automatically done for you by the framework.

void CExecuteDlg::OnPaint() 
{
	if (IsIconic())
	{
		CPaintDC dc(this); // device context for painting

		SendMessage(WM_ICONERASEBKGND, (WPARAM) dc.GetSafeHdc(), 0);

		// Center icon in client rectangle
		int cxIcon = GetSystemMetrics(SM_CXICON);
		int cyIcon = GetSystemMetrics(SM_CYICON);
		CRect rect;
		GetClientRect(&rect);
		int x = (rect.Width() - cxIcon + 1) / 2;
		int y = (rect.Height() - cyIcon + 1) / 2;

		// Draw the icon
		dc.DrawIcon(x, y, m_hIcon);
	}
	else
	{
		CDialog::OnPaint();
	}
}

// The system calls this to obtain the cursor to display while the user drags
//  the minimized window.
HCURSOR CExecuteDlg::OnQueryDragIcon()
{
	return (HCURSOR) m_hIcon;
}

CString GetAppPath()
{
	TRACE(TEXT("CExecuteDlg::GetAppPath()"));
	char path_buffer[MAX_PATH];	
	char drive[_MAX_DRIVE];
	char dir[_MAX_DIR];

	GetModuleFileName(NULL, path_buffer, MAX_PATH);
	_splitpath(path_buffer, drive, dir, 0, 0 );

	CString sPath = drive;
	sPath += dir;

	return sPath;
}

CString GetWinDir()
{
	TRACE(TEXT("CExecuteDlg::GetWinDir()"));
	char buf[512] = {0};
	
	GetWindowsDirectory(buf, sizeof(buf)-2);
	TRACE(TEXT("CExecuteDlg::GetWinDir()\n :> %s"), buf);
	CString str = buf;
	if(str[str.GetLength()-1]!='\\')str+='\\';

	return str;
}

void CExecuteDlg::OnOK()
{
	if(WaitForComplete(20) == WAIT_OBJECT_0)
		CDialog::OnOK();
}

void CExecuteDlg::OnCancel()
{
	if(WaitForComplete(20) == WAIT_OBJECT_0)
		CDialog::OnCancel();
}

HANDLE RunProgram(const CString& strCmdLine, bool bShowWindow)
{
	STARTUPINFO startupInfo;
	PROCESS_INFORMATION processInfo;
	memset (&startupInfo, 0, sizeof(STARTUPINFO));
	memset (&processInfo, 0, sizeof(PROCESS_INFORMATION));
	startupInfo.cb = sizeof(STARTUPINFO);
	startupInfo.dwFlags = STARTF_USESHOWWINDOW;
	startupInfo.wShowWindow = bShowWindow? SW_MINIMIZE: SW_HIDE;

	char szCmdLine[256] ={0};
	memcpy(szCmdLine,strCmdLine, strCmdLine.GetLength());
	
	if(!CreateProcess (NULL, szCmdLine, NULL, NULL, FALSE, 
	   NULL, //use DETACHED_PROCESS if try to create a console process
	   NULL, 
	   NULL, &startupInfo, &processInfo))
	{
		return NULL;
	}
	CloseHandle(processInfo.hThread);

	return 	processInfo.hProcess;
}


bool IsNT()
{
	OSVERSIONINFOEX osvi;
	ZeroMemory(&osvi, sizeof(OSVERSIONINFOEX));
	osvi.dwOSVersionInfoSize = sizeof(OSVERSIONINFOEX);

	GetVersionEx((OSVERSIONINFO *) &osvi);

	if(osvi.dwPlatformId == VER_PLATFORM_WIN32_NT)
		return true;
	else
		return false;

}

void ShowUsage()
{
	TRACE(TEXT("CExecuteDlg::ShowUsage()\n"));
	CString strUsage = "Usage\r\n"
					   "SetupProduct.exe -i <To install>\r\n"
					   "SetupProduct.exe -u <To uinstall>";
	AfxMessageBox(strUsage);
}

bool ParseCmdLn(bool& bInstall, 
				CString& strProduct, 
				CString& strLanguage,
				CString& strDestPath)
{
	TRACE(TEXT("CExecuteDlg::ParseCmdLn()\n"));
	CWinApp* pApp = AfxGetApp();
	if(!pApp->m_lpCmdLine)return false;

	CString strCmdLn = pApp->m_lpCmdLine;
	if(strCmdLn.IsEmpty())return false;
	strCmdLn.TrimLeft();

	//install or unistall
	CString strInstall = strCmdLn.Left(2);
	if(strInstall.CompareNoCase("-i")==0)
		bInstall = true;
	else if(strInstall.CompareNoCase("-u")==0)
		bInstall = false;
	else
		return false;

	strCmdLn.Delete(0,2);
	strCmdLn.TrimLeft();

	strProduct = strCmdLn.Left(2);
	strCmdLn.Delete(0,2);
	strCmdLn.TrimLeft();

	strLanguage = strCmdLn.Left(3);
	g_strLanguage = strLanguage;

	TRACE(TEXT("CExecuteDlg::ParseCmdLn()\n :> return true"));
	return true;

}



bool InstallMC315_2KXP(const CString& strProduct,const CString& strLanguage, const CString& strDestDir);
bool UninstallMC315_2KXP(const CString& strProduct,const CString& strDestDir);


bool InstallMG870_2KXP(const CString& strLanguage, const CString& strDestDir);
bool UninstallMG870_2KXP(const CString& strDestDir);


typedef bool (*INSTALL_FUNC)(const CString& strProduct,const CString& strLanguage, const CString& strDestDir);
typedef bool (*UNINSTALL_FUNC)(const CString& strProduct,const CString& strDestDir);

static void CheckAppRunning()
{	
	HANDLE AppLock = CreateSemaphore(
		NULL, 
		1, //仅仅允许一个实例运行
		1,
		"{51EA1C23-BE10-406A-9CF2-B7CDEAB1FB39}"//用一个GUID来标志本程序
		);
	if(!AppLock)return;
	if(GetLastError() == ERROR_ALREADY_EXISTS)//App is running
	{
		if(WAIT_TIMEOUT == WaitForSingleObject(AppLock,0))
		{
			AfxMessageBox(LoadStringEx(IDS_APPRUNING), MB_OK|MB_ICONINFORMATION);
		}
		while(WAIT_TIMEOUT == WaitForSingleObject(AppLock,100));
	}
	CloseHandle(AppLock);
}

void CExecuteDlg::Go()
{
	TRACE(TEXT("CExecuteDlg::Go()\n"));
	m_wndProgressBar.StartPlay();


	if(m_strLanguage == "-Ru")
	{
//		if(m_strProduct == "-p")
//		{
//			CStringTable::Instance().Load(GetAppPath()+"resource\\resource_cn.txt");
//		}
//		else if(m_strProduct == "-u")
//		{
//			CStringTable::Instance().Load(GetAppPath()+"resource\\resource_cn_usb.txt");
//		}
//		else if(m_strProduct == "-c")
//		{
			CStringTable::Instance().Load(GetAppPath()+"resource\\resource_ru_usb.txt");
//		}
	}
	else if(m_strLanguage == "-En")
	{
//		if(m_strProduct == "-p")
//		{
//			CStringTable::Instance().Load(GetAppPath()+"resource\\resource_en.txt");
//		}
//		else if(m_strProduct == "-u")
//		{
			CStringTable::Instance().Load(GetAppPath()+"resource\\resource_en_usb.txt");
//		}
//		else if(m_strProduct == "-c")
//		{
//			CStringTable::Instance().Load(GetAppPath()+"resource\\resource_en_com.txt");
//		}
	}


	if(!m_IsInstall)//使用应用程序的字符串表，便于翻译
	{
		CheckAppRunning();//检查应用程序是否在运行

		HANDLE hProcess = RunProgram(GetAppPath()+"RmTemp.exe", false);
		::WaitForSingleObject(hProcess,INFINITE);
		::CloseHandle(hProcess);
	}
	TRACE(TEXT("CExecuteDlg::Go()\n :> 1"));
	INSTALL_FUNC install2KXP = NULL;

	UNINSTALL_FUNC uninstall2KXP = NULL;

	CString strInstallPath = GetAppPath();	
	CString strInstPath = strInstallPath + "driver";

	SetPath(strInstPath);

	
	install2KXP	= InstallMC315_2KXP;
	uninstall2KXP = UninstallMC315_2KXP;
	

	TRACE(TEXT("CExecuteDlg::Go()\n :> %d\n"), m_IsInstall);
	if(m_IsInstall )//&& !IsWin9X
	{
		install2KXP(m_strProduct,m_strLanguage, strInstallPath);
	}
	else 
	{
		uninstall2KXP(m_strProduct,GetAppPath());
	}
	TRACE(TEXT("CExecuteDlg::Go()\n :> 2"));
	m_wndProgressBar.StopPlay();

	PostMessage(WM_CLOSE,0,0);
}

bool CExecuteDlg::SetPath(const CString& strInstPath)
{
	///设置Installation Sources和SourcePath
	HKEY hPortKey;
	CString strPosition = "SOFTWARE\\Microsoft\\Windows\\CurrentVersion\\Setup";
	
	LONG lReturn = ::RegOpenKeyEx( HKEY_LOCAL_MACHINE, 
		strPosition,
		0, KEY_READ | KEY_WRITE | KEY_QUERY_VALUE | KEY_SET_VALUE, &hPortKey);
	if(lReturn == ERROR_SUCCESS)
	{
		char  temp[256];

		memset(temp, 0, sizeof(temp));
		strcpy(temp, strInstPath.Left(strInstPath.GetLength()));

		lReturn = ::RegSetValueEx(hPortKey, (LPCTSTR)"Installation Sources", 0, REG_SZ, (unsigned char *)temp, strlen(temp)+1);
		lReturn = ::RegSetValueEx(hPortKey, (LPCTSTR)"SourcePath", 0, REG_SZ, (unsigned char *)temp, strlen(temp)+1);
		
	}

	return true;
}

LONG CExecuteDlg::OnDeviveChange(WPARAM wParam, LPARAM lParam)
{
	DEV_BROADCAST_HDR*   dhr   =   (DEV_BROADCAST_HDR*)lParam;  
	CString strModemName;
	strModemName = "WCDMA Modem";
	TRACE("wParam = %ld\n",long(wParam));

	PDEV_BROADCAST_HDR pDevBroadcastHdr; 
	PDEV_BROADCAST_VOLUME pDevBroadcastVolume; 

	
	if(wParam == DBT_DEVICEARRIVAL)
	{
		pDevBroadcastHdr = (PDEV_BROADCAST_HDR) lParam; 
		if (pDevBroadcastHdr->dbch_devicetype == DBT_DEVTYP_VOLUME) 
		{ 
			pDevBroadcastVolume = (PDEV_BROADCAST_VOLUME)pDevBroadcastHdr; 
			m_strVolume.Empty();
			xJudgeDeviceChangePath(pDevBroadcastVolume->dbcv_unitmask,m_strVolume); 
			#pragma message("********不同版本要进行修改*******")
			if(GetUsbDev("HUAWEI MOBILE  CONNECT"))
			{
				g_strVolume = m_strVolume;
			}
			
		} 

		
	}
	return true;
}

void CExecuteDlg::xJudgeDeviceChangePath(DWORD dwData, CString &strPath) 
{ 
	
	char i; 	
	for (i = 0; i < 26; ++i) 		
	{ 		
		if (dwData & 0x1) 			
		{ 			
			CString str = char(i+'A'); 			
			strPath = str+_T(":"); 
			
			break; 			
		} 
		
		dwData = dwData >> 1; 		
		if(dwData == 0x0000) 
			break; 
		
	} 
	
} 


bool CExecuteDlg::GetUsbDev(const CString& hwid)
{

	//class ID of ComPorts
	bool bRet = false;
#ifdef CDROM 
	const GUID  PortsGUID = {0x4d36e96dL, 0xe325, 0x11ce, {0xbf, 0xc1, 0x08, 0x00, 0x2b, 0xe1, 0x03, 0x18}};
#else  
	const GUID  PortsGUID = {0x4d36e96dL, 0xe325, 0x11ce, {0xbf, 0xc1, 0x08, 0x00, 0x2b, 0xe1, 0x03, 0x18}};
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
