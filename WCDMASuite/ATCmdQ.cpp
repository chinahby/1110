#include "stdafx.h"
#include "ATCmdQ.h"
#include "CriticalSecMon.h"

#ifdef _DEBUG
#undef THIS_FILE
static char THIS_FILE[]=__FILE__;
#define new DEBUG_NEW
#endif


CATCmdQ::CATCmdQ()
:m_EventQ(false, true)
{
}

CATCmdQ::~CATCmdQ()
{
	DeleteAll();
}

void CATCmdQ::DeleteAll()
{
	CCriticalMonitor lock(m_cs);//ÁÙ½çÇøËø¶¨

	m_Data.clear();

	m_EventQ.ResetEvent();
}


void CATCmdQ:: AddBack(const CATCmd & Record)
{
	CCriticalMonitor lock(m_cs);

	m_Data.push_back(Record);
	
	TRACE(TEXT("cmdque:size = %d"),m_Data.size());
	m_EventQ.SetEvent();
}

void CATCmdQ::AddFront(const CATCmd& Record)
{
	CCriticalMonitor lock(m_cs);

	m_Data.insert(m_Data.begin(), Record);

	m_EventQ.SetEvent();
}

bool CATCmdQ::PopTopRecord(CATCmd& Record)
{
	ASSERT(m_Data.size());
	CCriticalMonitor lock(m_cs);
	if(!m_Data.size())return false;

	Record = *m_Data.begin();
	
	m_Data.erase(m_Data.begin());

	if(!m_Data.size())
	{
		m_EventQ.ResetEvent();
	}

	return true;
}

