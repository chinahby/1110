// ThreadWaitAuotRun.cpp: implementation of the CThreadWaitAuotRun class.
//
//////////////////////////////////////////////////////////////////////

#include "stdafx.h"
#include "sunnycat.h"
#include "ThreadWaitAuotRun.h"
#include <atlbase.h>
#include "SunnyCatDlg.h"

#ifdef _DEBUG
#undef THIS_FILE
static char THIS_FILE[]=__FILE__;
#define new DEBUG_NEW
#endif

void LogString(CString strLog);
//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////

CThreadWaitAuotRun::CThreadWaitAuotRun(CMobileManager& mgr)
:m_Mgr(mgr)
{

}

CThreadWaitAuotRun::~CThreadWaitAuotRun()
{

}

void CThreadWaitAuotRun::Go()
{
	CRegKey reg;
	
	if(reg.Open(HKEY_LOCAL_MACHINE,TEXT("Software\\WCDMA Modem\\WCDMA"))!=ERROR_SUCCESS)
	{
		reg.Create(HKEY_LOCAL_MACHINE,TEXT("Software\\WCDMA Modem\\WCDMA"));		
		if(reg.Open(HKEY_LOCAL_MACHINE,TEXT("Software\\WCDMA Modem\\WCDMA")) != ERROR_SUCCESS)
		{
			return ;
		}
	}
	
	DWORD dwAutoRun = 0;
	DWORD dwTickcount = GetTickCount();
	while((GetTickCount()-dwTickcount) <10000)
	{		
		reg.QueryValue(dwAutoRun,TEXT("bAutoRun"));
		CString str;
		str.Format(TEXT("%ld"),dwAutoRun);
		LogString(LPCTSTR(str));
		if(dwAutoRun == 1)
		{		
			m_Mgr.m_pOwner->ChgAutoRegdit();
			::Sleep(1000);
			break;
		}
		::Sleep(1000);
	}
	SetWorkMode(m_Mgr,USETYPE_USBMODEM,false);
	
	return ;
}