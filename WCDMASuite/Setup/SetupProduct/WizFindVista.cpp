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
	//if(GetWindowText(hwnd) == "")return false;
	//TRACE1("%s\n", GetWindowText(hwnd));

	vector<HWND> vWnds;
	
	CWnd* pWnd = CWnd::FromHandle(hwnd);

	GetChildWndByClassName(pWnd, "ScrollBar", vWnds);
	int nScrollCount = vWnds.size();
	GetChildWndByClassName(pWnd, "SysLink", vWnds);
	int nSysLinkCount = vWnds.size();
	GetChildWndByClassName(pWnd, "Button", vWnds);
	int nBtnCount = vWnds.size();
	
	if(nScrollCount==0 && nSysLinkCount==0 &&nBtnCount==4)
	{
		return true;
	}

	return false;
}



/*
static bool IsFinishHWWizardWnd(HWND hwnd)
{
	CString strClassName;
	GetWndClassName(hwnd, strClassName);
	if(strClassName.CompareNoCase("#32770")!=0)return false;


	
	vector<HWND> vWnds;
	GetChildWndByClassName(CWnd::FromHandle(hwnd), "#32770", vWnds);
	if(vWnds.size()<1)return false;
	if(!IsWindow(vWnds[0]))return false;
	CWnd* pDlg = CWnd::FromHandle(vWnds[0]);
	GetChildWndByClassName(pDlg, "Static", vWnds);
	int nStaticCount = vWnds.size();
	GetChildWndByClassName(pDlg, "Button", vWnds);
	int nBtnCount = vWnds.size();

	if(nBtnCount==0 && nStaticCount==3)return true;
	

	return false;
}
*/
HWND GetWizardWndVista()
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



/*
HWND GetFinishHWWizardWndVista()
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
			if(IsFinishHWWizardWnd(vNewPopWnds[i]))
			{
				hwnd = vNewPopWnds[i];
				break;
			}
		}
	}
	

	return hwnd;
}*/