#include "StdAfx.h"
#include "Wndfuncs.h"
#include <algorithm>

static vector<HWND> vActiveWnds;
static vector<HWND> vNewPopWnds;
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
	if(GetWindowText(hwnd) == "")return false;

	TRACE1("%s\n", GetWindowText(hwnd));
	vector<HWND> vWnds;
	GetChildWndByClassName(CWnd::FromHandle(hwnd), "#32770", vWnds);
	if(vWnds.size()<1)return false;
	if(!IsWindow(vWnds[0]))return false;
	CWnd* pDlg = CWnd::FromHandle(vWnds[0]);
	GetChildWndByClassName(pDlg, "Static", vWnds);
	int nStaticCount = vWnds.size();
	GetChildWndByClassName(pDlg, "Button", vWnds);
	int nBtnCount = vWnds.size();

	if(nBtnCount==2 && nStaticCount==7)return true;
	if(nBtnCount==3 && nStaticCount==4)
	{
		GetChildWndByClassName(pDlg, "SysLink", vWnds);
		if(vWnds.size() == 1)return true;
	}

	return false;
}

HWND GetWizardWndXP()
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