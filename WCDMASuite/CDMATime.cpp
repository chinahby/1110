#include "stdafx.h"
#include "SunnyCat.h"
#include "CDMATime.h"

CCDMATime::CCDMATime()
:m_bConfigured(false)
{

}

CCDMATime::~CCDMATime()
{

}

CCDMATime& CCDMATime::Instance()
{
	static CCDMATime s_Inst;

	return s_Inst;
}

CTime	CCDMATime::GetCDMATime()
{
	return CTime::GetCurrentTime();
/*	if(!m_bConfigured)
	{
		return CTime::GetCurrentTime();
	}
	else
	{
		DWORD dwTickCount = GetTickCount()-m_dwTickcount;
		CTime tm = m_tmStartup;
		tm += CTimeSpan(0,0,0, dwTickCount/1000);
		return tm;
	}
	*/
}

void	CCDMATime::SetCDMATime(CTime tm)
{
	if(!m_bConfigured)
	{
		m_bConfigured = true;
		m_dwTickcount = GetTickCount();
		m_tmStartup = tm;
	}
}

bool	CCDMATime::HasInited()
{
	return m_bConfigured;
}

