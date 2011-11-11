#include "StdAfx.h"
#include "Wndfuncs.h"
#include <afxcview.h>
#include "SignatureMonitor.h"
#include "log.h"
#include <algorithm>


CString GetWizardText(const CString& item);
void DumpChildWndNames(CWnd* pWnd);

static CString GetWizardTitle();
class CThreadSelDir: public CThread
{
	CString m_strINFDir;
public:
	CThreadSelDir(const CString& strINFDir)
		:m_strINFDir(strINFDir)
	{
	}
protected:
	virtual void Go()
	{
		Sleep(200);
		HWND hwndWizard = FindWindowTM(GetWizardTitle(), 120000);
		CWnd* pWizard = CWnd::FromHandle(hwndWizard);
		if(!hwndWizard)return ;		
		while(true)
		{
			vector<HWND> vWnds;
			GetChildWndByClassName(pWizard, "Button", vWnds);
			int nBtnCount = vWnds.size();
			GetChildWndByClassName(pWizard, "Combobox", vWnds);
			int nCombCount = vWnds.size();
			if(nCombCount==1 && nBtnCount==3)break;

			Sleep(30);
			hwndWizard = FindWindowTM(GetWizardTitle(), 2000);
			pWizard = CWnd::FromHandle(hwndWizard);
		}

		vector<HWND> comboWnds;
		GetChildWndByClassName(pWizard, "Combobox", comboWnds);
		if(!comboWnds.size())return;
		SetComboText(CWnd::FromHandle(comboWnds[0]), m_strINFDir);
		Sleep(50);
		while(::IsWindow(pWizard->m_hWnd))
		{
			GetChildWndByClassName(pWizard, "Button", comboWnds);
			ClickWindow(comboWnds[0]);

			Sleep(200);
		}
	}
};

static CSignatureMonitor s_singmonitThread;

void TRACEWndText(CWnd* pWnd)
{
	vector<HWND> vWnd;
	GetChildeWnds(pWnd, vWnd);
	for(int i=0; i<vWnd.size(); ++i)
	{
		CString str;
		CWnd* pW = CWnd::FromHandle(vWnd[i]);
		pW->GetWindowText(str);
		TRACE(str);
		TRACE("\n");
	}
}

static HWND GetWizardWnd();
static bool IsSecondPage(CPropertySheet* pWizard)
{
	vector<HWND> vWnds;
	GetChildWndByClassName(pWizard, "Button", vWnds);
	
	return vWnds.size()==5;
}

static bool IsThirdPage(CPropertySheet* pWizard)
{
	vector<HWND> vWnds;
	GetChildWndByClassName(pWizard, "Button", vWnds);
	
	return vWnds.size()==7;
}

bool Setup2K(const CString& strINFDir)
{
	//Find the wizard window
	HWND hwndWizard = GetWizardWnd();
	if(!hwndWizard)
		return false;

	// Go to next page
	CPropertySheet* pWizard = (CPropertySheet*)CWnd::FromHandle(hwndWizard);
	pWizard->ShowWindow(SW_SHOWMINIMIZED);
	pWizard->PressButton(PSBTN_NEXT);
	Sleep(50);

	//Check the second page 
	DWORD dwTime = GetTickCount();
	while(::IsWindow(pWizard->m_hWnd)&& !IsSecondPage(pWizard) && GetTickCount()-dwTime<5000)
	{
		Sleep(50);
	}
	if(!IsSecondPage(pWizard))return false;
	
	//Go to Next page
	vector<HWND> vWnds;
	GetChildWndByClassName(pWizard, "Button", vWnds);
	ClickWindow(vWnds[0]);
	Sleep(50);
	pWizard->PressButton(PSBTN_NEXT);
	Sleep(50);


	//Check the third page
	while(::IsWindow(pWizard->m_hWnd) && !IsThirdPage(pWizard))
	{
		Sleep(50);
	}
	if(!::IsWindow(pWizard->m_hWnd))return false;
	GetChildWndByClassName(pWizard, "Button", vWnds);
	CheckWindow(vWnds[0], false);
	CheckWindow(vWnds[1], false);
	CheckWindow(vWnds[2], true);
	CheckWindow(vWnds[3], false);

	CThreadSelDir selDirThread(strINFDir);
	selDirThread.Start();
	Sleep(50);

	pWizard->PressButton(PSBTN_NEXT);
	selDirThread.WaitForComplete();

	//monitor digital signature
	s_singmonitThread.Start();
	while(!s_singmonitThread.IsRunning())
	{
		Sleep(10);
	}

	while(IsWindow(pWizard->m_hWnd))
	{
		vector<HWND> btns;
		GetChildWndByClassName(pWizard, "Button", btns);
		if(btns.size()!=3)
		{
			Sleep(30);
			continue;
		}
		if(	IsWindowEnabled(btns[1]) &&
			IsWindowEnabled(btns[0]) &&
			IsWindowEnabled(btns[3]))
		{
			break;
		}
		Sleep(50);
	}
	if(!::IsWindow(pWizard->m_hWnd))return false;
	Sleep(50);
	pWizard->PressButton(PSBTN_NEXT);
	Sleep(1500);

	while(IsWindow(pWizard->m_hWnd))
	{
		vector<HWND> btns;
		GetChildWndByClassName(pWizard, "Button", btns);
		if(btns.size()!=3)
		{
			Sleep(30);
			continue;
		}
		if(	!IsWindowEnabled(btns[1]) &&
			!IsWindowEnabled(btns[0]) &&
			!IsWindowEnabled(btns[3]))
		{
			::Sleep(30);	
			continue;
		}
		Sleep(200);
		while(IsWindow(pWizard->m_hWnd))
		{
			pWizard->PressButton(PSBTN_FINISH);
			Sleep(30);
		}
	}
	
	return true;
}


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

static HWND GetWizardWnd()
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