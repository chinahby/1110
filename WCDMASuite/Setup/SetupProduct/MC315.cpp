#include "stdafx.h"
#include "Execute.h"
#include "atlbase.h"

HANDLE RunProgram(const CString& strCmdLine, bool bShowWindow);
CString GetAppPath();
//bool InstallPCMCIA_98(const CString& strDestDir);
//bool UninstallPCMCIA_98(const CString& strDestDir);
bool InstallPCMCIA_2KXP(const CString& strDestDir);
bool UninstallPCMCIA_2KXP(const CString& strDestDir);
bool InstallUSB_2KXP(const CString& strDestDir);
bool UninstallUSB_2KXP(const CString& strDestDir);
bool InstallCOM_2KXP(const CString& strDestDir);
bool UninstallCOM_2KXP(const CString& strDestDir);

bool CopyResourceDir(const CString& strLanguage, const CString& strDestDir);
bool RemoveResourceDir(const CString& strDestDir);

int ShowMessageBox(UINT strid, UINT nType = MB_OK);



//MC315 Win2000/XP 的安装
bool InstallMC315_2KXP(const CString& strProduct,const CString& strLanguage, const CString& strDestDir)
{

	InstallUSB_2KXP(strDestDir);


	return true;
}


//MC315 Win2000/XP 的卸载
bool UninstallMC315_2KXP(const CString& strProduct,const CString& strDestDir)
{
	return UninstallUSB_2KXP(strDestDir);
}
