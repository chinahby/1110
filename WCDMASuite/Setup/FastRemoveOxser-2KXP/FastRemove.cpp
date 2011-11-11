#include "stdafx.h"
#include "FastRemove.h"
#include "ffsco.h"
#include "Setup.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

/////////////////////////////////////////////////////////////////////////////
// The one and only application object

CWinApp theApp;

using namespace std;

static void Remove(CString strProduct);
static void ReportError(LPCTSTR fcn, DWORD code);
static BOOL MfgCallback(CInf* inf, const CString& mfgname, const CString& modelname, PVOID context, DWORD& code);
static BOOL DeviceCallback(CInf* inf, const CString& devname, const CString& instsecname, const CStringArray& devid, PVOID context, DWORD& code);
void RemoveDriver(const CString& strHardwareID);

int _tmain(int argc, TCHAR* argv[], TCHAR* envp[])
{
	int nRetCode = 0;

	// initialize MFC and print and error on failure
	if (!AfxWinInit(::GetModuleHandle(NULL), NULL, ::GetCommandLine(), 0))
	{
		// TODO: change error code to suit your needs
		cerr << _T("Fatal Error: MFC initialization failed") << endl;
		nRetCode = 1;
	}
	else
	{
		//if(argc>1)//仅仅删除modem
		//	RemoveDriver("MDMGEN_WEWINS_2300");
		//else
			Remove(argv[1]);
	}

	return nRetCode;
}

bool SystemIsWin2K()
{
	OSVERSIONINFO version;
	version.dwOSVersionInfoSize = sizeof(version);
	GetVersionEx(&version);

	return version.dwMajorVersion == 5 && version.dwMinorVersion==0;
}

using namespace helper_coffs;

#define USB_FILE 0
#define COM_FILE 1

void RemoveDriverFile(int nFlag)
{
	CString strDevName;
	switch(nFlag) 
	{
	case USB_FILE:
		strDevName = "huawei design modem";
		break;
	case COM_FILE:
		strDevName = "";
		break;
	default:
		break;
	}
	///删除OEM文件
	char sys_path[MAX_PATH];
	char win_path[MAX_PATH];
	::GetSystemDirectory(sys_path, MAX_PATH);
	::GetWindowsDirectory(win_path, MAX_PATH);
	
	CString inf_path = win_path;
	inf_path +="\\inf";
	
	ffsco InfSearch;
	InfSearch.dirs(0);
	InfSearch.find(LPCSTR(inf_path), "oem*.inf");
	
	CString str;
	ffsco::typeT coo;
	ffsco::typeT::iterator it;
	
	coo = InfSearch.co_file();
	for (it = coo.begin(); coo.end() != it; it ++) 
	{
		CString file_name = (*it).c_str();
		TRACE1("%s\n", (*it).c_str());
		
		///打开INF文件，读取信息
		CInf inf;
		DWORD code;
		code = inf.Open(file_name, FALSE);
		if (code != 0)
		{
			ReportError(_T("SetupOpenInfFile"), code);
			continue;
		}
		
		// Locate the model statement for the designated device, or for the first device
		// in the file.
		
		InstallStruct is;
		is.done = FALSE;
		if (__argc >= 3)
			is.devid = __targv[2];
		
		code = inf.EnumManufacturers(MfgCallback, (PVOID) &is);
		if (code != 0)
		{
			ReportError(_T("search for specified device"), code);
			continue;
		}
		
		CString mfgname = is.mfgname;
		CString devname = is.devname;
		mfgname.MakeLower();
		devname.MakeLower();
		
		TRACE2("\n DevName:%s MfgName:%s \n", devname, mfgname);
		
		if(devname==strDevName
			&& inf.m_classname=="modem")
		{
			::DeleteFile(LPCSTR(file_name));
			
			TRACE("\n Delete " + file_name + "\n");
			
			///删除对应的pnf文件
			CString strPath = file_name;
			int pos = strPath.ReverseFind('.');
			strPath = strPath.Left(pos);
			strPath += ".pnf";
			
			::DeleteFile(LPCSTR(strPath));
			TRACE("\n Delete " + strPath + "\n");	
		}
		
	}
	
	if(nFlag == USB_FILE)
	{
		///删除驱动文件
		ffsco DriverSearch;
		DriverSearch.dirs(1);
		DriverSearch.find(LPCSTR(sys_path), "husbser.sys");
		
		coo = DriverSearch.co();
		TRACE1("全部 = %d 个\n", coo.size());
		
		coo = DriverSearch.co_dir();
		TRACE1("目录 = %d 个\n", coo.size());
		
		coo = DriverSearch.co_file();
		TRACE1("文件 = %d 个\n", coo.size());
		for (it = coo.begin(); coo.end() != it; it ++) 
		{
			CString file_name = (*it).c_str();
			::DeleteFile(LPCSTR(file_name));
			
			TRACE("\n Delete " + file_name + "\n");
		}
	}
		
}

void Remove(CString strProduct)
{


	if(strProduct == "-u") //USB_MODEM
	{
		RemoveDriver("USB\\Vid_0471&Pid_1234&Rev_0100");
		/*RemoveDriver("USB\\Vid_5421&Pid_8888&Rev_0100");
		RemoveDriver("USB\\Vid_0471&Pid_1201");
		RemoveDriver("USB\\Vid_0471&Pid_1201&Rev_0100&Mi_02");
		RemoveDriver("USB\\Vid_0471&Pid_1201&Rev_0100&Mi_01");*/
		RemoveDriverFile(USB_FILE);
	}
	else if(strProduct == "-c") //COM_MODEM
	{
		RemoveDriver("WEWINS_MDMGEN");
	}
	else if(strProduct == "-p") //PCMCIA
	{
		RemoveDriver("PCMCIA\\CF-GEN-2027");
		RemoveDriver("MDMGEN_WEWINS_2300");
		
		///删除OEM文件
		char sys_path[MAX_PATH];
		char win_path[MAX_PATH];
		::GetSystemDirectory(sys_path, MAX_PATH);
		::GetWindowsDirectory(win_path, MAX_PATH);
		
		CString inf_path = win_path;
		inf_path +="\\inf";
		
		ffsco InfSearch;
		InfSearch.dirs(0);
		InfSearch.find(LPCSTR(inf_path), "oem*.inf");
		
		CString str;
		ffsco::typeT coo;
		ffsco::typeT::iterator it;
		
		coo = InfSearch.co_file();
		for (it = coo.begin(); coo.end() != it; it ++) 
		{
			CString file_name = (*it).c_str();
			TRACE1("%s\n", (*it).c_str());
			
			///打开INF文件，读取信息
			CInf inf;
			DWORD code;
			code = inf.Open(file_name, FALSE);
			if (code != 0)
			{
				ReportError(_T("SetupOpenInfFile"), code);
				continue;
			}
			
			// Locate the model statement for the designated device, or for the first device
			// in the file.
			
			InstallStruct is;
			is.done = FALSE;
			if (__argc >= 3)
				is.devid = __targv[2];
			
			code = inf.EnumManufacturers(MfgCallback, (PVOID) &is);
			if (code != 0)
			{
				ReportError(_T("search for specified device"), code);
				continue;
			}
			
			CString mfgname = is.mfgname;
			CString devname = is.devname;
			mfgname.MakeLower();
			devname.MakeLower();
			
			TRACE2("\n DevName:%s MfgName:%s \n", devname, mfgname);
			
			if(devname=="ox16c95x communications port"
				&& inf.m_classname=="ports")
			{
				::DeleteFile(LPCSTR(file_name));
				
				TRACE("\n Delete " + file_name + "\n");
				
				///删除对应的pnf文件
				CString strPath = file_name;
				int pos = strPath.ReverseFind('.');
				strPath = strPath.Left(pos);
				strPath += ".pnf";
				
				TRACE("\n Delete " + strPath + "\n");	
			}
			
			if(mfgname=="WeWins"
				&& inf.m_classname=="modem")
			{
				::DeleteFile(LPCSTR(file_name));
				
				TRACE("\n Delete " + file_name + "\n");
				
				///删除对应的pnf文件
				CString strPath = file_name;
				int pos = strPath.ReverseFind('.');
				strPath = strPath.Left(pos);
				strPath += ".pnf";
				
				TRACE("\n Delete " + strPath + "\n");	
			}
			
			if(mfgname=="(Enhanced ports)"
				&& inf.m_classname=="ports")
			{
				::DeleteFile(LPCSTR(file_name));
				
				TRACE("\n Delete " + file_name + "\n");
				
				///删除对应的pnf文件
				CString strPath = file_name;
				int pos = strPath.ReverseFind('.');
				strPath = strPath.Left(pos);
				strPath += ".pnf";
				
				TRACE("\n Delete " + strPath + "\n");	
			}
			
			if(devname=="WeWins Modem"
				&& inf.m_classname=="modem")
			{
				::DeleteFile(LPCSTR(file_name));
				
				TRACE("\n Delete " + file_name + "\n");
				
				///删除对应的pnf文件
				CString strPath = file_name;
				int pos = strPath.ReverseFind('.');
				strPath = strPath.Left(pos);
				strPath += ".pnf";
				
				TRACE("\n Delete " + strPath + "\n");	
			}
			
		}
		
		///删除驱动文件
		ffsco DriverSearch;
		DriverSearch.dirs(1);
		DriverSearch.find(LPCSTR(sys_path), "Oxser.sys; Oxserui.dll");
		
		coo = DriverSearch.co();
		TRACE1("全部 = %d 个\n", coo.size());
		
		coo = DriverSearch.co_dir();
		TRACE1("目录 = %d 个\n", coo.size());
		
		coo = DriverSearch.co_file();
		TRACE1("文件 = %d 个\n", coo.size());
		for (it = coo.begin(); coo.end() != it; it ++) 
		{
			CString file_name = (*it).c_str();
			::DeleteFile(LPCSTR(file_name));
			
			TRACE("\n Delete " + file_name + "\n");
		}

	}

}

void RemoveDriver(const CString& strHardwareID)
{
	char szDevDesc[256] = {0};

	//class ID of ComPorts
	
	HDEVINFO hDevInfo;
	
	hDevInfo = SetupDiGetClassDevs(NULL, NULL, NULL, DIGCF_ALLCLASSES);
	
	if ( INVALID_HANDLE_VALUE == hDevInfo)return;

	SP_DEVINFO_DATA devInfoElem;
	devInfoElem.cbSize = sizeof(SP_DEVINFO_DATA);
		
	int index = 0;
	CString strHWID = strHardwareID;
	strHWID.MakeLower();
	while ( SetupDiEnumDeviceInfo(hDevInfo, index++, &devInfoElem))
	{
		if(SetupDiGetDeviceRegistryProperty(
			hDevInfo,
			&devInfoElem,
			SPDRP_HARDWAREID,
			NULL,
			(PBYTE)szDevDesc,
			sizeof(szDevDesc),
			NULL))
		{
			TRACE1("%s\n",szDevDesc);
			CString strID = szDevDesc;
			strID.MakeLower();
			if(strID.Find(strHWID) != -1)//Find
			{
				TRACE("Matched ID\n");
				SetupDiCallClassInstaller(DIF_REMOVE,
                    hDevInfo,
                    &devInfoElem);
				break;
			}
		}
	}
	SetupDiDestroyDeviceInfoList(hDevInfo);
}


void ReportError(LPCTSTR fcn, DWORD code)
{							// ReportError
	static struct {DWORD code; LPTSTR text;} setuperrors[] = {
		{ERROR_EXPECTED_SECTION_NAME, _T("ERROR_EXPECTED_SECTION_NAME")},
		{ERROR_BAD_SECTION_NAME_LINE, _T("ERROR_BAD_SECTION_NAME_LINE")},
		{ERROR_SECTION_NAME_TOO_LONG, _T("ERROR_SECTION_NAME_TOO_LONG")},
		{ERROR_GENERAL_SYNTAX, _T("ERROR_GENERAL_SYNTAX")},
		{ERROR_WRONG_INF_STYLE, _T("ERROR_WRONG_INF_STYLE")},
		{ERROR_SECTION_NOT_FOUND, _T("ERROR_SECTION_NOT_FOUND")},
		{ERROR_LINE_NOT_FOUND, _T("ERROR_LINE_NOT_FOUND")},
		{ERROR_NO_ASSOCIATED_CLASS, _T("ERROR_NO_ASSOCIATED_CLASS")},
		{ERROR_CLASS_MISMATCH, _T("ERROR_CLASS_MISMATCH")},
		{ERROR_DUPLICATE_FOUND, _T("ERROR_DUPLICATE_FOUND")},
		{ERROR_NO_DRIVER_SELECTED, _T("ERROR_NO_DRIVER_SELECTED")},
		{ERROR_KEY_DOES_NOT_EXIST, _T("ERROR_KEY_DOES_NOT_EXIST")},
		{ERROR_INVALID_DEVINST_NAME, _T("ERROR_INVALID_DEVINST_NAME")},
		{ERROR_INVALID_CLASS, _T("ERROR_INVALID_CLASS")},
		{ERROR_DEVINST_ALREADY_EXISTS, _T("ERROR_DEVINST_ALREADY_EXISTS")},
		{ERROR_DEVINFO_NOT_REGISTERED, _T("ERROR_DEVINFO_NOT_REGISTERED")},
		{ERROR_INVALID_REG_PROPERTY, _T("ERROR_INVALID_REG_PROPERTY")},
		{ERROR_NO_INF, _T("ERROR_NO_INF")},
		{ERROR_NO_SUCH_DEVINST, _T("ERROR_NO_SUCH_DEVINST")},
		{ERROR_CANT_LOAD_CLASS_ICON, _T("ERROR_CANT_LOAD_CLASS_ICON")},
		{ERROR_INVALID_CLASS_INSTALLER, _T("ERROR_INVALID_CLASS_INSTALLER")},
		{ERROR_DI_DO_DEFAULT, _T("ERROR_DI_DO_DEFAULT")},
		{ERROR_DI_NOFILECOPY, _T("ERROR_DI_NOFILECOPY")},
		{ERROR_INVALID_HWPROFILE, _T("ERROR_INVALID_HWPROFILE")},
		{ERROR_NO_DEVICE_SELECTED, _T("ERROR_NO_DEVICE_SELECTED")},
		{ERROR_DEVINFO_LIST_LOCKED, _T("ERROR_DEVINFO_LIST_LOCKED")},
		{ERROR_DEVINFO_DATA_LOCKED, _T("ERROR_DEVINFO_DATA_LOCKED")},
		{ERROR_DI_BAD_PATH, _T("ERROR_DI_BAD_PATH")},
		{ERROR_NO_CLASSINSTALL_PARAMS, _T("ERROR_NO_CLASSINSTALL_PARAMS")},
		{ERROR_FILEQUEUE_LOCKED, _T("ERROR_FILEQUEUE_LOCKED")},
		{ERROR_BAD_SERVICE_INSTALLSECT, _T("ERROR_BAD_SERVICE_INSTALLSECT")},
		{ERROR_NO_CLASS_DRIVER_LIST, _T("ERROR_NO_CLASS_DRIVER_LIST")},
		{ERROR_NO_ASSOCIATED_SERVICE, _T("ERROR_NO_ASSOCIATED_SERVICE")},
		{ERROR_NO_DEFAULT_DEVICE_INTERFACE, _T("ERROR_NO_DEFAULT_DEVICE_INTERFACE")},
		{ERROR_DEVICE_INTERFACE_ACTIVE, _T("ERROR_DEVICE_INTERFACE_ACTIVE")},
		{ERROR_DEVICE_INTERFACE_REMOVED, _T("ERROR_DEVICE_INTERFACE_REMOVED")},
		{ERROR_BAD_INTERFACE_INSTALLSECT, _T("ERROR_BAD_INTERFACE_INSTALLSECT")},
		{ERROR_NO_SUCH_INTERFACE_CLASS, _T("ERROR_NO_SUCH_INTERFACE_CLASS")},
		{ERROR_INVALID_REFERENCE_STRING, _T("ERROR_INVALID_REFERENCE_STRING")},
		{ERROR_INVALID_MACHINENAME, _T("ERROR_INVALID_MACHINENAME")},
		{ERROR_REMOTE_COMM_FAILURE, _T("ERROR_REMOTE_COMM_FAILURE")},
		{ERROR_MACHINE_UNAVAILABLE, _T("ERROR_MACHINE_UNAVAILABLE")},
		{ERROR_NO_CONFIGMGR_SERVICES, _T("ERROR_NO_CONFIGMGR_SERVICES")},
		{ERROR_INVALID_PROPPAGE_PROVIDER, _T("ERROR_INVALID_PROPPAGE_PROVIDER")},
		{ERROR_NO_SUCH_DEVICE_INTERFACE, _T("ERROR_NO_SUCH_DEVICE_INTERFACE")},
		{ERROR_DI_POSTPROCESSING_REQUIRED, _T("ERROR_DI_POSTPROCESSING_REQUIRED")},
		{ERROR_INVALID_COINSTALLER, _T("ERROR_INVALID_COINSTALLER")},
		{ERROR_NO_COMPAT_DRIVERS, _T("ERROR_NO_COMPAT_DRIVERS")},
		{ERROR_NO_DEVICE_ICON, _T("ERROR_NO_DEVICE_ICON")},
		{ERROR_INVALID_INF_LOGCONFIG, _T("ERROR_INVALID_INF_LOGCONFIG")},
		{ERROR_DI_DONT_INSTALL, _T("ERROR_DI_DONT_INSTALL")},
		{ERROR_INVALID_FILTER_DRIVER, _T("ERROR_INVALID_FILTER_DRIVER")},
		{ERROR_NO_DEFAULT_INTERFACE_DEVICE, _T("ERROR_NO_DEFAULT_INTERFACE_DEVICE")},
		{ERROR_INTERFACE_DEVICE_ACTIVE, _T("ERROR_INTERFACE_DEVICE_ACTIVE")},
		{ERROR_INTERFACE_DEVICE_REMOVED, _T("ERROR_INTERFACE_DEVICE_REMOVED")},
		{ERROR_NO_SUCH_INTERFACE_DEVICE, _T("ERROR_NO_SUCH_INTERFACE_DEVICE")},
		{ERROR_NOT_INSTALLED, _T("ERROR_NOT_INSTALLED")},
		};

	LPTSTR msg = NULL;
	TCHAR msgbuf[512];

	if (FormatMessage(FORMAT_MESSAGE_ALLOCATE_BUFFER | FORMAT_MESSAGE_FROM_SYSTEM,
		NULL, code, MAKELANGID(LANG_NEUTRAL, SUBLANG_DEFAULT),
		(LPTSTR) &msg, 0, NULL))

		_stprintf(msgbuf, _T("Error %d in %s: %s\n"), code, fcn, msg);
	else
		{						// not a standard message
		for (int i = 0; i < arraysize(setuperrors); ++i)
			if (setuperrors[i].code == code)
				break;
		if (i < arraysize(setuperrors))
			_stprintf(msgbuf, _T("Error %8.8lX in %s: %s\n"), code, fcn, setuperrors[i].text);
		else
			_stprintf(msgbuf, _T("Error %8.8lX in %s\n"), code, fcn);
		}						// not a standard message

	if (msg)
		LocalFree((LPVOID) msg);

	TRACE(msgbuf);//AfxMessageBox(msgbuf);
}							// ReportError

BOOL MfgCallback(CInf* inf, const CString& mfgname, const CString& modelname, PVOID context, DWORD& code)
{
	InstallStruct* isp = (InstallStruct*) context;
	isp->mfgname = mfgname;
	code = inf->EnumDevices(modelname, DeviceCallback, context);
	return code == 0 && !isp->done;
}


BOOL DeviceCallback(CInf* inf, const CString& devname, const CString& instsecname, const CStringArray& devid, PVOID context, DWORD& code)
{
	InstallStruct* isp = (InstallStruct*) context;
	int n = devid.GetSize();
	for (int i = 0; i < n; ++i)
		if (isp->devid == devid[i] || isp->devid.GetLength() == 0)
			break;

	if (i >= n)
		return TRUE;			// keep enumerating -- this isn't it

	isp->devid = devid[i];
	isp->devname = devname;

	code = 0;					// successful conclusion
	isp->done = TRUE;
	return FALSE;				// stop enumeration
}
