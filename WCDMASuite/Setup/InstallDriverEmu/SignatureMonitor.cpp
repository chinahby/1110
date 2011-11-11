// SignatureMonitor.cpp: implementation of the CSignatureMonitor class.
//
//////////////////////////////////////////////////////////////////////

#include "stdafx.h"
#include "SignatureMonitor.h"
#include "Wndfuncs.h"
#include <iostream.h>
#include <algorithm>

#ifdef _DEBUG
#undef THIS_FILE
static char THIS_FILE[]=__FILE__;
#define new DEBUG_NEW
#endif

CString GetWizardText(const CString& item);

CSignatureMonitor::CSignatureMonitor()
{

}

CSignatureMonitor::~CSignatureMonitor()
{

}

bool SystemIsWin2K();
bool SystemIsWin98();
bool SystemIsWinXP();
void CSignatureMonitor::Go()
{
	cout<<"Waiting signature window..."<<endl;
	HWND hwnd = FindSignatureWnd();
	if(!hwnd)return;
	
	cout<<"found signature window"<<endl;
	CWnd* pWnd = CWnd::FromHandle(hwnd);
	if( SystemIsWin2K())
	{
		vector<HWND> childWnds;
		GetChildWndByClassName(pWnd, "Button",childWnds);
		while(::IsWindow(pWnd->m_hWnd))
		{
			ClickWindow(childWnds[0]);
			::Sleep(100);
		}
	}
	else if(SystemIsWinXP())
	{
		vector<HWND> childWnds;
		GetChildWndByClassName(pWnd, "Button",childWnds);
		while(::IsWindow(pWnd->m_hWnd))
		{
			ClickWindow(childWnds[0]);
			::Sleep(100);
		}
	}
	else if( SystemIsWin98())
	{
		//TODO
	}
}

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
static bool IsSignatureWnd(HWND hwnd)
{
	if(SystemIsWinXP())
	{
		TRACE1("%s\n", GetWindowText(hwnd));

		CString strClassName;
		GetWndClassName(hwnd, strClassName);
		if(strClassName.CompareNoCase("#32770")!=0)return false;

		vector<HWND> childWnds;
		GetChildWndByClassName(CWnd::FromHandle(hwnd), "Button",childWnds);
		if(childWnds.size()!=2)return false;

		GetChildWndByClassName(CWnd::FromHandle(hwnd), "Static",childWnds);
		if(childWnds.size() != 5)return false;

		GetChildWndByClassName(CWnd::FromHandle(hwnd), "SysLink", childWnds);
		return childWnds.size()==1;
	}

	if(SystemIsWin2K())
	{
		CString strClassName;
		GetWndClassName(hwnd, strClassName);
		if(strClassName.CompareNoCase("#32770")!=0)return false;

		vector<HWND> childWnds;
		GetChildWndByClassName(CWnd::FromHandle(hwnd), "Button",childWnds);
		if(childWnds.size()!=3)return false;

		GetChildWndByClassName(CWnd::FromHandle(hwnd), "Static",childWnds);
		if(childWnds.size() == 7)return true;
	}

	if(SystemIsWin98())
	{
		//TODO
	}

	return false;

}

static HWND GetSignatureWnd()
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
			if(IsSignatureWnd(vNewPopWnds[i]))
			{
				hwnd = vNewPopWnds[i];
				break;
			}
		}
	}
	

	return hwnd;
}

HWND CSignatureMonitor::FindSignatureWnd()
{
	return GetSignatureWnd();
}