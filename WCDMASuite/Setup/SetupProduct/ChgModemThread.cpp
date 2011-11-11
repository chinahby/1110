// ChgModemThread.cpp: implementation of the CChgModemThread class.
//
//////////////////////////////////////////////////////////////////////

#include "stdafx.h"
#include "execute.h"
#include "ChgModemThread.h"
#include "UChg.h"

#ifdef _DEBUG
#undef THIS_FILE
static char THIS_FILE[]=__FILE__;
#define new DEBUG_NEW
#endif


extern CString g_strVolume;
extern bool g_bFindHW;
//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////

CChgModemThread::CChgModemThread()
{

}

CChgModemThread::~CChgModemThread()
{

}

void CChgModemThread::Go()
{
	DWORD dwTime = GetTickCount();
	while(GetTickCount()-dwTime < 120000)
	{
		SetWorkMode(USETYPE_USBMODEM,g_strVolume);
		::Sleep(1000);
		if(g_bFindHW)
			break;
	}
}