#include "stdAfx.h"
#include "WndFuncs.h"
#include <algorithm>

using namespace std;



HWND FindWindowTM(const CString& strTitle, DWORD dwWaitTime)
{
	DWORD dwTime = GetTickCount();
	CWnd* pWndFind = NULL;
	while(true)
	{
		if(GetTickCount()-dwTime>dwWaitTime)break;
		pWndFind = CWnd::FindWindow(NULL, strTitle);
		if(pWndFind)
		{
			return pWndFind->m_hWnd;
		}
		::Sleep(50);
	}

	return NULL;
}


BOOL CALLBACK EnumChildProc(
  HWND hwnd,      // handle to child window
  LPARAM lParam   // application-defined value
)
{
	vector<HWND>* pWnds = reinterpret_cast<vector<HWND>*>(lParam);
	if(IsWindow(hwnd))
	{
		CWnd* pWnd = CWnd::FromHandle(hwnd);
		if(pWnd->IsWindowVisible())//Only visible window enumerated
			pWnds->push_back(hwnd);
	}

	return TRUE;
}

void GetChildeWnds(CWnd* pWnd, vector<HWND>& childWnds)
{
	childWnds.clear();
	if(!IsWindow(pWnd->m_hWnd))return;

	EnumChildWindows(pWnd->GetSafeHwnd(), EnumChildProc, (LPARAM)&childWnds);
}

HWND GetChildWndByName(CWnd* pWnd, const CString& strChildWndName)
{
	vector<HWND> vTemp;
	GetChildeWnds(pWnd, vTemp);
	for(int i=0;i<vTemp.size(); ++i)
	{
		if(!IsWindow(vTemp[i]))continue;
		CWnd* pWnd = CWnd::FromHandle(vTemp[i]);
		CString strName;
		pWnd->GetWindowText(strName);
		if(strName.CompareNoCase(strChildWndName) == 0)
		{
			return vTemp[i];
		}
	}

	return NULL;
}

void GetWndClassName(HWND hwnd, CString& className)
{
	char strName[256] = {0};
	::GetClassName(hwnd, strName, sizeof(strName));

	className = strName;
}

bool MatchClass(HWND hwnd, const CString& className)
{
	CString strClass;
	::GetWndClassName(hwnd, strClass);
	return strClass.CompareNoCase(className) == 0;
}

void GetChildWndByClassName(CWnd* pWnd, const CString& className, vector<HWND>& childWnds)
{
	childWnds.clear();
	if(!IsWindow(pWnd->m_hWnd))return;

	vector<HWND> vTemp;
	GetChildeWnds(pWnd, vTemp);
	for(int i=0;i<vTemp.size(); ++i)
	{
		if(!IsWindow(vTemp[i]))continue;
		if(MatchClass(vTemp[i], className))
		{
			childWnds.push_back(vTemp[i]);
		}
	}
}

bool SetComboText(CWnd* pWnd, const CString& strText)
{
	if(!MatchClass(pWnd->GetSafeHwnd(), "ComboBox"))
		return false;

	CComboBox* pCombo = (CComboBox*)pWnd;

	int nFind = pCombo->FindString(0,strText);
	if(nFind == CB_ERR )
	{
		pCombo->InsertString(0, strText);
		nFind = 0;
	}
	pCombo->SetCurSel(nFind);

	return true;
}

void ClickWindow(HWND hwnd)
{
	POINTS point={3,3};
	LPARAM* p =(LPARAM*)&point;
	::PostMessage(hwnd, WM_LBUTTONDOWN, MK_LBUTTON, *p);
	::Sleep(10);
	::PostMessage(hwnd, WM_LBUTTONUP, 0, *p);
}

bool ClickChildWnd(CWnd* pWnd, const CString& strChildWndName)
{
	HWND hwnd = GetChildWndByName(pWnd, strChildWndName);
	if(!hwnd)return false;

	ClickWindow(hwnd);

	return true;
}

bool CheckWindow(HWND hwnd, bool bChecked)
{
	if(!MatchClass(hwnd, "Button"))
		return false;

	CButton* pBtn = (CButton*)CWnd::FromHandle(hwnd);
	pBtn->SetCheck(bChecked);

	return true;
}

bool CheckChildWnd(CWnd* pWnd, const CString& strBtnWndName, bool bCheck)
{
	HWND hwnd = GetChildWndByName(pWnd, strBtnWndName);
	if(!hwnd)return false;

	return CheckWindow(hwnd, bCheck);
}


HWND FindStringInChildWnds(CWnd* pWnd, const CString& str)
{
	vector<HWND> vTemp;
	GetChildeWnds(pWnd, vTemp);
	for(int i=0;i<vTemp.size(); ++i)
	{
		if(ContainText(vTemp[i], str))return vTemp[i];
	}

	return NULL;
}

bool ContainText(HWND hwnd, const CString& str)
{
	CWnd* pWnd = CWnd::FromHandle(hwnd);
	CString strName;
	pWnd->GetWindowText(strName);

	strName.MakeLower();
	CString str1 = str;
	str1.MakeLower(); 
	return strName.Find(str1) >=0;
}

class CWndFinder
{
public:
	CWndFinder(TCondition condition)
		:m_Condition(condition),
		 m_hwnd(NULL)
	{
	}

	bool Put(HWND hwnd)
	{
		if(m_Condition(hwnd))
		{
			m_hwnd = hwnd;
			return false; //Stop enumeration
		}
		
		return true;	  //Continue enumeration
	}

	HWND GetHwnd()const
	{
		return m_hwnd;
	}
private:
	TCondition m_Condition;
	HWND m_hwnd;
};

static BOOL CALLBACK EnumWindowsProc
(
  HWND hwnd,      // handle to parent window
  LPARAM lParam   // application-defined value
)
{
	CWndFinder* pFinder = (CWndFinder*)lParam;

	return pFinder->Put(hwnd);
}

HWND FindWnd_If(TCondition condition)
{
	CWndFinder finder(condition);
	EnumWindows(EnumWindowsProc, (LPARAM)&finder);

	return finder.GetHwnd();
}

CString GetWindowText(HWND hwnd)
{
	CString str;
	CWnd::FromHandle(hwnd)->GetWindowText(str);
	return str;
}