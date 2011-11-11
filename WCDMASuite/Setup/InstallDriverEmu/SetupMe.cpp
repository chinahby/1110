#include "StdAfx.h"
#include "Wndfuncs.h"
#include "Wndfuncs.h"
#include "SignatureMonitor.h"
#include <algorithm>

CString GetWizardText(const CString& item);
static CSignatureMonitor s_singmonitThread;
//static HWND FindWizardWnd98();

CString	strMeInfDir;

int  iType = 0;  


bool CheckIsMeFirstWnd(HWND hwnd)
{
	CString strClassName;
	GetWndClassName(hwnd, strClassName);

	if(strClassName.CompareNoCase("#32770")!=0)	return false;

	vector<HWND> childWnds;
	GetChildWndByClassName(CWnd::FromHandle(hwnd), "Static",childWnds);
	
	int num = childWnds.size();
	
	GetChildWndByClassName(CWnd::FromHandle(hwnd), "Button",childWnds);
	
	int num1 = childWnds.size();


	vector<HWND> cmboWnds;

	GetChildWndByClassName(CWnd::FromHandle(hwnd),"Combobox", cmboWnds);
	int num2 = cmboWnds.size();

	if( num == 4 && num1 == 5 && num2)
	{
		iType = 0;
		return true;
		
	}
	else if(num == 6 && num1 == 5)
	{
		iType = 1;
		return true;
	}
	else
		return false;

	return true;
}

HWND CheckMeFirstWnd()
{
	DWORD dwTime = GetTickCount();
	
	HWND hwnd = NULL;
	while(!hwnd)
	{
		hwnd = FindWnd_If(CheckIsMeFirstWnd);
		Sleep(30);
		if(GetTickCount()-dwTime > 60000)break;
	}
	return hwnd;
}


bool IsMeFirstWnd(HWND hwnd)
{
	CString strClassName;
	GetWndClassName(hwnd, strClassName);

	if(strClassName.CompareNoCase("#32770")!=0)	return false;

	vector<HWND> childWnds;
	GetChildWndByClassName(CWnd::FromHandle(hwnd), "Static",childWnds);
	
	int num = childWnds.size();
	if(num != 4)	return false;

	GetChildWndByClassName(CWnd::FromHandle(hwnd), "Button",childWnds);
	
	num = childWnds.size();

    if(num != 5)	return false;

	vector<HWND> cmboWnds;

	GetChildWndByClassName(CWnd::FromHandle(hwnd),"Combobox", cmboWnds);
	if(!cmboWnds.size())
	{
		return false;
	}

	RECT rc;
	LPRECT lpRect=&rc;
    ::GetWindowRect(cmboWnds[0],lpRect);
	ClipCursor(lpRect);
	SetComboText(CWnd::FromHandle(cmboWnds[0]), strMeInfDir);
	ClipCursor(NULL);
	Sleep(100);

	GetChildWndByClassName(CWnd::FromHandle(hwnd), "Button",childWnds);
	
    ::GetWindowRect(childWnds[0],lpRect);
	ClipCursor(lpRect);
	ClickWindow(childWnds[0]);
	ClipCursor(NULL);
	Sleep(100);

	return true;
}

HWND FindMeFirstWnd()
{
	DWORD dwTime = GetTickCount();
	
	HWND hwnd = NULL;
	while(!hwnd)
	{
		hwnd = FindWnd_If(IsMeFirstWnd);
		Sleep(30);
		if(GetTickCount()-dwTime > 60000)break;
	}
	return hwnd;
}

bool IsMeSecondWnd(HWND hwnd)
{
	CString strClassName;
	GetWndClassName(hwnd, strClassName);

	if(strClassName.CompareNoCase("#32770")!=0)	return false;

	vector<HWND> childWnds;
	GetChildWndByClassName(CWnd::FromHandle(hwnd), "Static",childWnds);
	
	int num = childWnds.size();
	if(num != 4)	return false;

	GetChildWndByClassName(CWnd::FromHandle(hwnd), "Button",childWnds);
	
	num = childWnds.size();

    if(num != 5)	return false;

	vector<HWND> cmboWnds;

	GetChildWndByClassName(CWnd::FromHandle(hwnd),"Combobox", cmboWnds);
	if(!cmboWnds.size())
	{
		return false;
	}

	RECT rc;
	LPRECT lpRect=&rc;
    ::GetWindowRect(cmboWnds[0],lpRect);
	ClipCursor(lpRect);
	SetComboText(CWnd::FromHandle(cmboWnds[0]), strMeInfDir);
	ClipCursor(NULL);
	Sleep(100);

	GetChildWndByClassName(CWnd::FromHandle(hwnd), "Button",childWnds);
	
    ::GetWindowRect(childWnds[0],lpRect);
	ClipCursor(lpRect);
	ClickWindow(childWnds[0]);
	ClipCursor(NULL);
	Sleep(100);

	return true;
}


HWND FindMeSecondWnd()
{
	DWORD dwTime = GetTickCount();
	
	HWND hwnd = NULL;
	while(!hwnd)
	{
		hwnd = FindWnd_If(IsMeSecondWnd);
		Sleep(30);
		if(GetTickCount()-dwTime > 60000)break;
	}
	return hwnd;
}


bool IsMeThirdWnd(HWND hwnd)
{
	CString strClassName;
	GetWndClassName(hwnd, strClassName);

	if(strClassName.CompareNoCase("#32770")!=0)	return false;

	vector<HWND> childWnds;
	GetChildWndByClassName(CWnd::FromHandle(hwnd), "Static",childWnds);
	
	int num = childWnds.size();
	if(num != 6)	return false;

	GetChildWndByClassName(CWnd::FromHandle(hwnd), "Button",childWnds);
	
	num = childWnds.size();

    if(num != 5)	return false;

	RECT rc;
	LPRECT lpRect=&rc;

    ::GetWindowRect(childWnds[1],lpRect);
	ClipCursor(lpRect);
	ClickWindow(childWnds[1]);
	ClipCursor(NULL);
	Sleep(100);

    ::GetWindowRect(childWnds[3],lpRect);
	ClipCursor(lpRect);
	ClickWindow(childWnds[3]);
	ClipCursor(NULL);
	Sleep(100);

	return true;
}


HWND FindMeThirdWnd()
{
	DWORD dwTime = GetTickCount();
	
	HWND hwnd = NULL;
	while(!hwnd)
	{
		hwnd = FindWnd_If(IsMeThirdWnd);
		Sleep(30);
		if(GetTickCount()-dwTime > 60000)break;
	}
	return hwnd;
}

bool IsMeFourWnd(HWND hwnd)
{
	CString strClassName;
	GetWndClassName(hwnd, strClassName);

	if(strClassName.CompareNoCase("#32770")!=0)	return false;

	vector<HWND> childWnds;
	GetChildWndByClassName(CWnd::FromHandle(hwnd), "Static",childWnds);
	
	int num = childWnds.size();
	if(num != 3)	return false;

	GetChildWndByClassName(CWnd::FromHandle(hwnd), "Button",childWnds);
	
	num = childWnds.size();

    if(num != 8)	return false;

	vector<HWND> cmboWnds;

	GetChildWndByClassName(CWnd::FromHandle(hwnd),"Combobox", cmboWnds);
	if(!cmboWnds.size())
	{
		return false;
	}


	RECT rc;
	LPRECT lpRect=&rc;

    ::GetWindowRect(childWnds[0],lpRect);
	ClipCursor(lpRect);
	ClickWindow(childWnds[0]);
	ClipCursor(NULL);
	Sleep(100);

    ::GetWindowRect(childWnds[1],lpRect);
	ClipCursor(lpRect);
	CheckWindow(childWnds[1], false);
	ClipCursor(NULL);
	Sleep(100);

    ::GetWindowRect(childWnds[2],lpRect);
	ClipCursor(lpRect);
	CheckWindow(childWnds[2], true);
	ClipCursor(NULL);
	Sleep(100);

    ::GetWindowRect(cmboWnds[0],lpRect);
	ClipCursor(lpRect);
	SetComboText(CWnd::FromHandle(cmboWnds[0]), strMeInfDir);
	ClipCursor(NULL);
	Sleep(100);

    ::GetWindowRect(childWnds[6],lpRect);
	ClipCursor(lpRect);
	ClickWindow(childWnds[6]);
	ClipCursor(NULL);
	Sleep(100);

	return true;
}


HWND FindMeFourWnd()
{
	DWORD dwTime = GetTickCount();
	
	HWND hwnd = NULL;
	while(!hwnd)
	{
		hwnd = FindWnd_If(IsMeFourWnd);
		Sleep(30);
		if(GetTickCount()-dwTime > 60000)break;
	}
	return hwnd;
}

bool IsMeFiveWnd(HWND hwnd)
{
	CString strClassName;
	GetWndClassName(hwnd, strClassName);

	if(strClassName.CompareNoCase("#32770")!=0)	return false;

	vector<HWND> childWnds;
	GetChildWndByClassName(CWnd::FromHandle(hwnd), "Static",childWnds);
	
	int num = childWnds.size();
	if(num != 7)	return false;

	GetChildWndByClassName(CWnd::FromHandle(hwnd), "Button",childWnds);
	
	num = childWnds.size();

    if(num != 3)	return false;

	RECT rc;
	LPRECT lpRect=&rc;

    ::GetWindowRect(childWnds[1],lpRect);
	ClipCursor(lpRect);
	ClickWindow(childWnds[1]);
	ClipCursor(NULL);
	Sleep(100);

	return true;
}


HWND FindMeFiveWnd()
{
	DWORD dwTime = GetTickCount();
	
	HWND hwnd = NULL;
	while(!hwnd)
	{
		hwnd = FindWnd_If(IsMeFiveWnd);
		Sleep(30);
		if(GetTickCount()-dwTime > 60000)break;
	}
	return hwnd;
}

bool IsMeSixWnd(HWND hwnd)
{
	CString strClassName;
	GetWndClassName(hwnd, strClassName);

	if(strClassName.CompareNoCase("#32770")!=0)	return false;

	vector<HWND> childWnds;
	GetChildWndByClassName(CWnd::FromHandle(hwnd), "Static",childWnds);
	
	int num = childWnds.size();
	if(num != 5)	return false;

	GetChildWndByClassName(CWnd::FromHandle(hwnd), "Button",childWnds);
	
	int num1 = childWnds.size();
	if(num1 != 3)	return false;


	DWORD dwWait = GetTickCount();

	RECT rc;
	LPRECT lpRect=&rc;


	while(true)
	{
		if(GetTickCount()-dwWait>60000)
		{
			::GetWindowRect(childWnds[1],lpRect);
			ClipCursor(lpRect);
			ClickWindow(childWnds[1]);
			ClipCursor(NULL);
			Sleep(100);
			break;
		} 

		GetChildWndByClassName(CWnd::FromHandle(hwnd), "Button", childWnds);
		
		if(::IsWindowEnabled(childWnds[1]))
		{
			::GetWindowRect(childWnds[1],lpRect);
			ClipCursor(lpRect);
			ClickWindow(childWnds[1]);
			ClipCursor(NULL);
			Sleep(100);
			break;
		}

		::Sleep(50);
	}


    ::GetWindowRect(childWnds[1],lpRect);
	ClipCursor(lpRect);
	ClickWindow(childWnds[1]);
	ClipCursor(NULL);
	Sleep(100);

	return true;
}

HWND FindMeSixWnd()
{
	DWORD dwTime = GetTickCount();
	
	HWND hwnd = NULL;
	while(!hwnd)
	{
		hwnd = FindWnd_If(IsMeSixWnd);
		Sleep(50);
		if(GetTickCount()-dwTime > 60000)break;
	}
	return hwnd;
}



/** Setup the drive by search a specified directory.
*	Install a device driver by enmulate user interact
*	with Winodws hardware wizard.
*
*	@param	strINFDir[in]
*	the directory that containes the driver inf file.
*/
bool SetupMe(const CString& strINFDir)
{
	strMeInfDir = strINFDir;

	//
	HWND hwnd = CheckMeFirstWnd();
	if(!hwnd)	return false;

	if(iType == 0)
	{
		//安装PCMCIA卡 复制oxser.vxd
		HWND hwnd = FindMeFirstWnd();
		if(!hwnd)	return false;

		//安装PCMCIA卡 复制oxserui.dll
		hwnd = FindMeSecondWnd();
		if(!hwnd)	return false;
	}
	else
	{
		hwnd = FindMeThirdWnd();
		if(!hwnd)	return false;

		hwnd = FindMeFourWnd();
		if(!hwnd)	return false;

		hwnd = FindMeFiveWnd();
		if(!hwnd)	return false;

		hwnd = FindMeSixWnd();
		if(!hwnd)	return false;
	}
	return true;
}
