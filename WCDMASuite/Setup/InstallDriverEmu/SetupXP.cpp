#include "StdAfx.h"
#include "Wndfuncs.h"
#include "SignatureMonitor.h"
#include <algorithm>


CString GetWizardText(const CString& item);
void DumpChildWndNames(CWnd* pWnd)
{
	vector<HWND> childWnds;
	GetChildeWnds(pWnd, childWnds);

	for(size_t i=0; i<childWnds.size(); ++i)
	{
		CString str;
		CWnd::FromHandle(childWnds[i])->GetWindowText(str);

		TRACE(str);
		TRACE("\r\n");
	}
}

static CSignatureMonitor s_singmonitThread;

CString LoadDLLString(const CString& dllName, UINT strID);
static HWND GetWizardWnd();

bool SetupXP(const CString& strINFDir)
{
	HWND hwndWizard = GetWizardWnd();
	if(!hwndWizard)
		return false;

	CPropertySheet* pWizard = (CPropertySheet*)CWnd::FromHandle(hwndWizard);
	pWizard->ShowWindow(SW_SHOWMINIMIZED);

	vector<HWND> vWnds;
	GetChildWndByClassName(pWizard, "#32770", vWnds);
	if(!vWnds.size())return false;
	CWnd* pDlg = CWnd::FromHandle(vWnds[0]);
	GetChildWndByClassName(pDlg, "Button", vWnds);
	if(vWnds.size()==3)
	{
		CButton* pBtn = (CButton*)CWnd::FromHandle(vWnds[0]);
		while(!pBtn->GetCheck())
		{
			ClickWindow(vWnds[0]);
			Sleep(50);
		}
		pWizard->PressButton(PSBTN_NEXT);
		Sleep(50);
	}

	GetChildWndByClassName(pWizard, "#32770", vWnds);
	pDlg = CWnd::FromHandle(vWnds[0]);
	GetChildWndByClassName(pDlg, "Button", vWnds);
	if(vWnds.size()!=2)return false;
	while(::IsWindow(pWizard->m_hWnd))
	{
		CButton* pBtn = (CButton*)CWnd::FromHandle(vWnds[1]);
		while(!pBtn->GetCheck())
		{
			ClickWindow(vWnds[1]);
			Sleep(50);
		}
		break;
	}
	if(!::IsWindow(pWizard->m_hWnd))return false;
	pWizard->PressButton(PSBTN_NEXT);
	Sleep(50);

	while(::IsWindow(pWizard->m_hWnd))
	{
		GetChildWndByClassName(pWizard, "#32770", vWnds);
		pDlg = CWnd::FromHandle(vWnds[0]);
		GetChildWndByClassName(pDlg, "Combobox", vWnds);
		if(vWnds.size())break;
		Sleep(30);
	}
	if(!::IsWindow(pWizard->m_hWnd))return false;
	GetChildWndByClassName(pDlg, "Button", vWnds);
	if(vWnds.size() != 5)return false;
	ClickWindow(vWnds[0]);
	Sleep(50);
	CheckWindow(vWnds[2], false);
	CheckWindow(vWnds[3], true);
	vector<HWND> comboWnds;
	GetChildWndByClassName(pDlg, "Combobox", comboWnds);
	if(!comboWnds.size())return false;
	SetComboText(CWnd::FromHandle(comboWnds[0]), strINFDir);

	s_singmonitThread.Start();
	while(!s_singmonitThread.IsRunning())
	{
		Sleep(10);
	}

	Sleep(50);
	pWizard->PressButton(PSBTN_NEXT);
	Sleep(50);

	while(::IsWindow(pWizard->m_hWnd))
	{
		GetChildWndByClassName(pWizard, "Button", vWnds);
		if(vWnds.size() == 3)
		{
			if(  IsWindowEnabled(vWnds[1]) &&
				!IsWindowEnabled(vWnds[0]) &&
				!IsWindowEnabled(vWnds[2]))
			{
				pWizard->PressButton(PSBTN_FINISH);
				break;
			}
		}
		Sleep(50);
	}
	
	//DumpChildWndNames(pWizard);

	return true;
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