#include "StdAfx.h"
#include "Wndfuncs.h"
#include <algorithm>

static vector<HWND> vActiveWnds;
static vector<HWND> vNewPopWnds;
static CString      strWizardTitle;
static CString GetWizardTitle()
{
	return strWizardTitle;
}
static bool IsNewWindow(HWND hwnd)
{
	if(std::find(vActiveWnds.begin(), vActiveWnds.end(), hwnd) == vActiveWnds.end())
	{
		//vNewPopWnds.push_back(hwnd);
		vActiveWnds.push_back(hwnd);
	}
	vNewPopWnds.push_back(hwnd);
	return false;
}
static bool IsWizardWnd(HWND hwnd)
{
	CString strClassName;
	GetWndClassName(hwnd, strClassName);
	if(strClassName.CompareNoCase("#32770")!=0)return false;

	CWnd* pWnd = CWnd::FromHandle(hwnd);
	TRACE1("%s\n", GetWindowText(hwnd));

	vector<HWND> vWnds;
	GetChildWndByClassName(pWnd, "#32770", vWnds);
	if(vWnds.size()!=1)return false;
	GetChildeWnds(pWnd, vWnds);
	if(vWnds.size()!=8)return false;

	GetChildWndByClassName(pWnd, "Static", vWnds);
	if(vWnds.size()!=4)return false;
	GetChildWndByClassName(pWnd, "Button", vWnds);
	if(vWnds.size()!=3)return false;

	CWnd::FromHandle(hwnd)->GetWindowText(strWizardTitle);
	return true;
}

HWND GetWizardWnd2K()
{
	HWND hwnd = NULL;

	DWORD dwTime = GetTickCount();
	vActiveWnds.clear();
	FindWnd_If(IsNewWindow);

	while(!hwnd && GetTickCount()-dwTime<120000)
	{
		Sleep(150);
		vNewPopWnds.clear();
		FindWnd_If(IsNewWindow);
		for(size_t i=0; i<vNewPopWnds.size(); ++i)
		{
			if(IsWizardWnd(vNewPopWnds[i]))
			{
				hwnd = vNewPopWnds[i];
				break;
			}
		}
	}

	return hwnd;
}