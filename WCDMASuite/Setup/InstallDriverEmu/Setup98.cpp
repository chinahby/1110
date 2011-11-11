#include "StdAfx.h"
#include "Wndfuncs.h"
#include "Wndfuncs.h"
#include "SignatureMonitor.h"
#include <algorithm>

CString GetWizardText(const CString& item);
static CSignatureMonitor s_singmonitThread;
//static HWND FindWizardWnd98();

CString	strInfDir;

bool IsFirstWnd(HWND hwnd)
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
	while(true)
	{
		if(GetTickCount()-dwWait>5000)
		{
			return false;
		} 

		GetChildWndByClassName(CWnd::FromHandle(hwnd), "Static",childWnds);
		
		num = childWnds.size();

		GetChildWndByClassName(CWnd::FromHandle(hwnd), "Button",childWnds);
		
		num1 = childWnds.size();

		if(num == 5 && num1 == 3)	
		{
			RECT rc;
			LPRECT lpRect=&rc;

			::GetWindowRect(childWnds[1],lpRect);
			ClipCursor(lpRect);
			ClickWindow(childWnds[1]);
			ClipCursor(NULL);
			Sleep(100);

		}
		else
		{
			return true;
		}
		Sleep(50);
	}

	return true;
}

HWND FindFirstWnd()
{
	DWORD dwTime = GetTickCount();
	
	HWND hwnd = NULL;
	while(!hwnd)
	{
		hwnd = FindWnd_If(IsFirstWnd);
		Sleep(50);
		if(GetTickCount()-dwTime > 120000)break;
	}
	return hwnd;
}

bool IsSecondWnd(HWND hwnd)
{
	CString strClassName;
	GetWndClassName(hwnd, strClassName);

	if(strClassName.CompareNoCase("#32770")!=0)	return false;

	vector<HWND> childWnds;
	GetChildWndByClassName(CWnd::FromHandle(hwnd), "Static",childWnds);
	
	int num = childWnds.size();
	if(num != 3)	return false;

	GetChildWndByClassName(CWnd::FromHandle(hwnd), "Button",childWnds);
	
	int num1 = childWnds.size();
	if(num1 != 5)	return false;

	DWORD dwWait = GetTickCount();
	while(true)
	{
		if(GetTickCount()-dwWait>5000)
		{
			return false;
		} 

		GetChildWndByClassName(CWnd::FromHandle(hwnd), "Static",childWnds);
		
		num = childWnds.size();

		GetChildWndByClassName(CWnd::FromHandle(hwnd), "Button",childWnds);
		
		num1 = childWnds.size();

		if(num == 3 && num1 == 5)	
		{
			RECT rc;
			LPRECT lpRect=&rc;

			::GetWindowRect(childWnds[0],lpRect);
			ClipCursor(lpRect);
			ClickWindow(childWnds[0]);
			ClipCursor(NULL);
			Sleep(100);

			::GetWindowRect(childWnds[3],lpRect);
			ClipCursor(lpRect);
			ClickWindow(childWnds[3]);
			ClipCursor(NULL);
			Sleep(100);

		}
		else
		{
			return true;
		}
		Sleep(50);
	}

	return true;
}

HWND FindSecondWnd()
{
	DWORD dwTime = GetTickCount();
	
	HWND hwnd = NULL;
	while(!hwnd)
	{
		hwnd = FindWnd_If(IsSecondWnd);
		Sleep(50);
		if(GetTickCount()-dwTime > 60000)break;
	}
	return hwnd;
}

bool IsThirdWnd(HWND hwnd)
{
	CString strClassName;
	GetWndClassName(hwnd, strClassName);

	if(strClassName.CompareNoCase("#32770")!=0)	return false;

	vector<HWND> childWnds;
	GetChildWndByClassName(CWnd::FromHandle(hwnd), "Static",childWnds);
	
	int num = childWnds.size();
	if(num != 3)	return false;

	GetChildWndByClassName(CWnd::FromHandle(hwnd), "Button",childWnds);
	
	int num1 = childWnds.size();
	if(num1 != 8)	return false;

	vector<HWND> comboWnds;

	GetChildWndByClassName(CWnd::FromHandle(hwnd), "Combobox", comboWnds);
	int num2 = comboWnds.size();
	if(num2 != 1)	return false;

	DWORD dwWait = GetTickCount();
	while(true)
	{
		if(GetTickCount()-dwWait>5000)
		{
			return false;
		} 

		GetChildWndByClassName(CWnd::FromHandle(hwnd), "Static",childWnds);
		
		num = childWnds.size();

		GetChildWndByClassName(CWnd::FromHandle(hwnd), "Button",childWnds);
		
		num1 = childWnds.size();

		GetChildWndByClassName(CWnd::FromHandle(hwnd), "Combobox", comboWnds);
		num2 = comboWnds.size();

		if(num == 3 && num1 == 8 && num2 == 1)	
		{
			RECT rc;
			LPRECT lpRect=&rc;

			::GetWindowRect(childWnds[0],lpRect);
			ClipCursor(lpRect);
			CheckWindow(childWnds[0], false);
			ClipCursor(NULL);
			Sleep(50);
		
			::GetWindowRect(childWnds[1],lpRect);
			ClipCursor(lpRect);
			CheckWindow(childWnds[1], false);
			ClipCursor(NULL);
			Sleep(50);

			::GetWindowRect(childWnds[2],lpRect);
			ClipCursor(lpRect);
			CheckWindow(childWnds[2], false);
			ClipCursor(NULL);
			Sleep(50);

			::GetWindowRect(childWnds[3],lpRect);
			ClipCursor(lpRect);
			CheckWindow(childWnds[3], true);
			ClipCursor(NULL);
			Sleep(50);

			::GetWindowRect(comboWnds[0],lpRect);
			ClipCursor(lpRect);
			SetComboText(CWnd::FromHandle(comboWnds[0]), strInfDir);
			ClipCursor(NULL);
			Sleep(50);

			::GetWindowRect(childWnds[6],lpRect);
			ClipCursor(lpRect);
			ClickWindow(childWnds[6]);
			ClipCursor(NULL);
			Sleep(50);

		}
		else
		{
			return true;
		}
		Sleep(50);
	}

	return true;
}

HWND FindThirdWnd()
{
	DWORD dwTime = GetTickCount();
	
	HWND hwnd = NULL;
	while(!hwnd)
	{
		hwnd = FindWnd_If(IsThirdWnd);
		Sleep(50);
		if(GetTickCount()-dwTime > 60000)break;
	}
	return hwnd;
}

bool IsFourWnd(HWND hwnd)
{
	CString strClassName;
	GetWndClassName(hwnd, strClassName);

	if(strClassName.CompareNoCase("#32770")!=0)	return false;

	vector<HWND> childWnds;
	GetChildWndByClassName(CWnd::FromHandle(hwnd), "Static",childWnds);
	
	int num = childWnds.size();
	if(num != 8)	return false;

	GetChildWndByClassName(CWnd::FromHandle(hwnd), "Button",childWnds);
	
	int num1 = childWnds.size();
	if(num1 != 3)	return false;


	DWORD dwWait = GetTickCount();
	while(true)
	{
		if(GetTickCount()-dwWait>5000)
		{
			return false;
		} 

		GetChildWndByClassName(CWnd::FromHandle(hwnd), "Static",childWnds);
		
		num = childWnds.size();

		GetChildWndByClassName(CWnd::FromHandle(hwnd), "Button",childWnds);
		
		num1 = childWnds.size();

		if(num == 8 && num1 == 3)	
		{
			if(::IsWindowEnabled(childWnds[1]))
			{
				RECT rc;
				LPRECT lpRect=&rc;

				::GetWindowRect(childWnds[1],lpRect);
				ClipCursor(lpRect);
				ClickWindow(childWnds[1]);
				ClipCursor(NULL);
				Sleep(100);
				break;
			}
			else
			{
				return true;
			}

		}
		else
		{
			return true;
		}
		Sleep(50);
	}

	return true;
}

HWND FindFourWnd()
{
	DWORD dwTime = GetTickCount();
	
	HWND hwnd = NULL;
	while(!hwnd)
	{
		hwnd = FindWnd_If(IsFourWnd);
		Sleep(50);
		if(GetTickCount()-dwTime > 60000)break;
	}
	return hwnd;
}

bool IsFiveWnd(HWND hwnd)
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
	while(true)
	{
		if(GetTickCount()-dwWait>60000)
		{
			ClickWindow(childWnds[1]);
			break;
		} 

		GetChildWndByClassName(CWnd::FromHandle(hwnd), "Button", childWnds);
		
		if(::IsWindowEnabled(childWnds[1]))
		{
			RECT rc;
			LPRECT lpRect=&rc;

			::GetWindowRect(childWnds[1],lpRect);
			ClipCursor(lpRect);
			ClickWindow(childWnds[1]);
			ClipCursor(NULL);
			Sleep(100);
			break;
		}

		::Sleep(50);
	}

	RECT rc;
	LPRECT lpRect=&rc;

	::GetWindowRect(childWnds[1],lpRect);
	ClipCursor(lpRect);
	ClickWindow(childWnds[1]);
	ClipCursor(NULL);
	Sleep(100);

	return true;
}

HWND FindFiveWnd()
{
	DWORD dwTime = GetTickCount();
	
	HWND hwnd = NULL;
	while(!hwnd)
	{
		hwnd = FindWnd_If(IsFiveWnd);
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
bool Setup98(const CString& strINFDir)
{
	strInfDir = strINFDir;

	HWND hwnd = FindFirstWnd();
	if(!hwnd)	return false;

	hwnd = FindSecondWnd();
	if(!hwnd)	return false;

	hwnd = FindThirdWnd();
	if(!hwnd)	return false;

	hwnd = FindFourWnd();
	if(!hwnd)	return false;

	hwnd = FindFiveWnd();
	if(!hwnd)	return false;
	
	return true;
}

