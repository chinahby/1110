#include "stdafx.h"
#include "CallRecord.h"
#include <algorithm>
#include "../CriticalSecMon.h"
#include "../Util.h"

#ifdef _DEBUG
#undef THIS_FILE
static char THIS_FILE[]=__FILE__;
#define new DEBUG_NEW
#endif

TCallInfo::TCallInfo()
{
	type = CALL_INCOMING;
	begin = CTime::GetCurrentTime();
	period = 0;
}

CString TCallInfo::TimeString()const
{
	return begin.Format("%Y-%m-%d %H:%M:%S");
}

bool TCallInfo::IsMissingCall()const
{
	return type==CALL_INCOMING && period==0;
}

bool TCallInfo::IsFailingCall()const
{
	return type==CALL_OUTGOING && period==0;
}

CCallRecord::CCallRecord()
{
	SetTable("SC700_CallRecord");
	InitializeCriticalSection(&m_CS);
}

CCallRecord::~CCallRecord()
{
	ClearVector();
	DeleteCriticalSection(&m_CS);
}

size_t CCallRecord::Size()const
{
	return m_Data.size();
}

TCallInfo& CCallRecord::GetAt(size_t index)
{
	ASSERT(index<Size());
	return *m_Data[index];
}

const TCallInfo& CCallRecord::GetAt(size_t index)const
{
	ASSERT(index<Size());
	return *m_Data[index];
}

TCallInfo& CCallRecord::Add(const TCallInfo& item)
{
	CCriticalMonitor mon(m_CS);//多线程保护
	
	TCallInfo* pNew = new TCallInfo(item);
	m_Data.insert(m_Data.begin(), pNew);
	
	Insert(* pNew);
	
	FireEvent_Add(0, *pNew);
	
	return *pNew;
}

void  CCallRecord::FireEvent_Add(int nIndex, const TCallInfo& newItem)
{
	for(size_t i=0; i<m_Observers.size(); ++i)
	{
		if(m_Observers[i])
			m_Observers[i]->OnAdd(this, nIndex, newItem);
	}
}

void CCallRecord::Delete(size_t nID)
{
	CCriticalMonitor mon(m_CS);//多线程保护
	
	//int nID = m_Data[index]->nID;
	int nIndex = FindIndexByID(nID);
	ASSERT(nIndex >= 0);
	ASSERT(nIndex < Size());
	
	delete m_Data[nIndex];
	m_Data.erase(m_Data.begin()+nIndex);
	
	
	CString strSQL;
	strSQL.Format(TEXT("CallRecordID = %d"),nID);
	removeDatasByFilter(strSQL);
	
}

int CCallRecord::FindIndexByID(size_t id)const
{
	CCriticalMonitor mon(m_CS);
	
	for(size_t i=0; i<Size(); ++i)
	{
		if(m_Data[i]->nID == id)
			return i;
	}
	return -1;
}

void CCallRecord::Clear()
{
	CCriticalMonitor mon(m_CS);
	ClearVector();
	
	CString strSQL;
	
	strSQL.Format(TEXT("RecordType = %d"),m_nRecordType);
	removeDatasByFilter(strSQL);
}

void CCallRecord::ClearVector()
{
	CCriticalMonitor mon(m_CS);//多线程保护
	
	for(size_t i=0; i<Size(); ++i)
	{
		delete m_Data[i];
	}
	m_Data.clear();
}

DWORD CCallRecord::CalculateTime(ECallType type)const
{
	CCriticalMonitor mon(m_CS);//多线程保护
	
	DWORD dwTime = 0;
	for(size_t i=0; i<Size(); ++i)
	{
		const TCallInfo& item = GetAt(i);
		if(item.type == type)
		{
			dwTime += item.period;
		}
	}
	
	return dwTime;
}

DWORD CCallRecord::GetCallInTime()const
{
	return CalculateTime(CALL_INCOMING);
}

DWORD CCallRecord::GetCallOutTime()const
{
	return CalculateTime(CALL_OUTGOING);
}

void CCallRecord::AddObserver(ICallRecordObserver* pObserver)
{
	CCriticalMonitor mon(m_CS);//多线程保护
	
	m_Observers.push_back( pObserver );
}

void CCallRecord::RemoveObserver(ICallRecordObserver* pObserver)
{
	CCriticalMonitor mon(m_CS);//多线程保护
	
	std::vector<ICallRecordObserver*>::iterator iFind = 
		std::find(m_Observers.begin(), m_Observers.end(), pObserver);
	
	if(iFind != m_Observers.end())
	{
		m_Observers.erase(iFind);
	}
}

BOOL CCallRecord::GetRecordsByRecordType(size_t nRecordType)
{
	CCriticalMonitor mon(m_CS);
	
	m_nRecordType = nRecordType;
	
	CString strSQL;
	strSQL.Format(TEXT("RecordType = %d"),nRecordType);
	strSQL += TEXT(" ORDER BY BegTime DESC");
	return getDatasByFilter(strSQL,this);
}

void CCallRecord:: modifyFieldByDB(int argc, char** argv, char** columnName)
{
	TCallInfo* pTCallInfo = new TCallInfo();
	
	pTCallInfo->nID = atoi(argv[Util::getIndexByName(argc, columnName, "CallRecordID")]);
	pTCallInfo->eRecordType = (CallRecordType)atoi(argv[Util::getIndexByName(argc, columnName, "RecordType")]);
	pTCallInfo->type = (ECallType)atoi(argv[Util::getIndexByName(argc, columnName, "Flag")]);
	pTCallInfo->strTelephone = argv[Util::getIndexByName(argc, columnName, "Telephone")];
	pTCallInfo->strName = argv[Util::getIndexByName(argc, columnName, "Name")];
	TimeFromString(argv[Util::getIndexByName(argc, columnName, "BegTime")],pTCallInfo->begin);
	pTCallInfo->period = atol(argv[Util::getIndexByName(argc, columnName, "CallPeriod")]);
	
	m_Data.push_back(pTCallInfo);	
}

BOOL CCallRecord::Insert(TCallInfo& item)
{
	CString strSQL = "INSERT INTO ";
	CString str;

	str = ChangeStrToStr(item.strName);
	
	strSQL += GettableName();
	strSQL += "(CallRecordID,RecordType,Flag,Telephone,Name,BegTime,CallPeriod)Values(";
	strSQL += "NULL",
		strSQL += ",",
		strSQL += Util::intToString(item.eRecordType);
	strSQL += ",";
	strSQL += Util::intToString(item.type);
	strSQL += ",'";
	strSQL += item.strTelephone;
	strSQL += "','";
	strSQL += str;
	strSQL += "','";
	strSQL += item.TimeString();
	strSQL += "','";
	strSQL += Util::intToString(item.period);
	strSQL += "')";
	
	return execCommand(strSQL);
}