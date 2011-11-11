#include "StdAfx.h"
#include "ConnectRecord.h"
#include <fstream>
#include "../Util.h"
#include "../CriticalSecMon.h"
#include <algorithm>

using namespace std;
void TimeFromString(const CString& strCDMA, CTime& tm);
//**************************************************
//	class TConnectInfo
//**************************************************
TConnectInfo::TConnectInfo()
{
	tmBeginTime = CTime::GetCurrentTime();
	iTotalSecond = 0;
	iSend = 0;
	iReceive = 0;
}

unsigned int TConnectInfo::BelongYearMonth()const
{
	CTimeSpan tsAdd(iTotalSecond);
	CTime tm = tmBeginTime+tsAdd;
	int iYear = tm.GetYear();
	int iMonth = tm.GetMonth();
	return (iYear * 12 + iMonth);
}

CString TConnectInfo::TimeString()const
{
	return tmBeginTime.Format("%Y-%m-%d %H:%M:%S");
}

//**************************************************
//	class CConnectRecord
//**************************************************

CConnectRecord::CConnectRecord()
{
	SetTable("SC700_ConnectRecord");
	InitializeCriticalSection(&m_CS);
}

CConnectRecord::~CConnectRecord()
{
	ClearVector();
	DeleteCriticalSection(&m_CS);

//	sqlite3_close(m_pDB);
}

size_t CConnectRecord::Size()const
{
	return m_Connections.size();
}

const TConnectInfo& CConnectRecord::GetAt(size_t index)const
{
	ASSERT(index<Size());
	return *m_Connections[index];
}

void CConnectRecord::AddFront(const TConnectInfo& info)
{
	CCriticalMonitor mon(m_CS);

	m_Connections.insert(m_Connections.begin(), new TConnectInfo(info));

	Insert(info);

	FireEvent_Add(0, *m_Connections[0]);
}

void CConnectRecord::AddTail(const TConnectInfo& info)
{
	CCriticalMonitor mon(m_CS);

	m_Connections.push_back(new TConnectInfo(info));

	size_t index = m_Connections.size()-1;
	FireEvent_Add(index, *m_Connections[index]);
}

void CConnectRecord::ClearVector()
{
	CCriticalMonitor mon(m_CS);

	for(size_t i=0; i<m_Connections.size(); ++i)
		delete m_Connections[i];

	m_Connections.clear();
}

void CConnectRecord::Clear()
{
	CCriticalMonitor mon(m_CS);
	ClearVector();
	
	CString strSQL;

	strSQL.Empty();
	removeDatasByFilter(strSQL);
}

int CConnectRecord::FindIndexByID(size_t id)const
{
	//CCriticalMonitor mon(m_CS);

	for(size_t i=0; i<Size(); ++i)
	{
		if(m_Connections[i]->nID == id)
			return i;
	}
	return -1;
}

void CConnectRecord::Delete(size_t nID)
{
	CCriticalMonitor mon(m_CS);
	int index = FindIndexByID(nID);
	if(index<Size())
	{
		delete m_Connections[index];
		m_Connections.erase(m_Connections.begin()+index);
	}

	CString strSQL;
	strSQL.Format(TEXT("ConnectRecordID = %d"),nID);
	removeDatasByFilter(strSQL);
}

void CConnectRecord::GetStatTime(CTime tmStartTime, CTime tmEndTime,CString &strTime, CString &strFlux)const
{
	CTime tmStartTemp( tmStartTime.GetYear(), tmStartTime.GetMonth(), tmStartTime.GetDay(), 0, 0, 0);

	CTime tmEndTemp(tmEndTime.GetYear(), tmEndTime.GetMonth(), tmEndTime.GetDay(), 23, 59, 59);

	__int64 iStatTotalSecond = 0;
	__int64 iStatTotalFlux = 0;

	for(size_t i=0; i<Size(); ++i)
	{
		const TConnectInfo& item = GetAt(i);
		if(item.tmBeginTime >= tmStartTemp && item.tmBeginTime <= tmEndTemp)
		{	//匹配,进行统计
			iStatTotalSecond += item.iTotalSecond;
			iStatTotalFlux += item.Flux();
		}
	}

	//形成返回信息
	strTime = SecondToTime(iStatTotalSecond);
	strFlux.Format(TEXT("%I64u"), iStatTotalFlux);
}

void CConnectRecord::GetStatTime(CTime tmStartTime, 
								 CTime tmEndTime, 
								 __int64& i64FluxUpLoad,
								 __int64& i64FluxDownLoad) const
{
	CTime tmStartTemp( tmStartTime.GetYear(), tmStartTime.GetMonth(), tmStartTime.GetDay(), 0, 0, 0);

	CTime tmEndTemp(tmEndTime.GetYear(), tmEndTime.GetMonth(), tmEndTime.GetDay(), 23, 59, 59);

	__int64 iStatUpLoadFlux = 0;
	__int64 iStatDownLoadFlux = 0;

	for(size_t i=0; i<Size(); ++i)
	{
		const TConnectInfo& item = GetAt(i);
		if(item.tmBeginTime >= tmStartTemp && item.tmBeginTime <= tmEndTemp)
		{	//匹配,进行统计
			iStatUpLoadFlux += item.iSend;
			iStatDownLoadFlux += item.iReceive;
		}
	}

	//形成返回信息
	i64FluxUpLoad = iStatUpLoadFlux;
	i64FluxDownLoad = iStatDownLoadFlux;
}

size_t CConnectRecord::GetMonthStatCount()const
{
	CCriticalMonitor mon(m_CS);

	if(!Size())return 0;

	unsigned int iYearMonth = GetAt(0).BelongYearMonth();

	size_t nCount = 1;
	for(size_t i=1; i<Size(); ++i)
	{
		if(GetAt(i).BelongYearMonth() != iYearMonth)
		{
			++nCount;
			iYearMonth = GetAt(i).BelongYearMonth();
		}
	}

	return nCount;
}

bool CConnectRecord::GetMonthStat(size_t iIndex, 
			CString &strDate, 
			CString &strLastTime, 
			CString &strFlux)const
{
	CCriticalMonitor mon(m_CS);

	if(iIndex >= GetMonthStatCount())return false;

	//找到第一条记录
	unsigned int iYearMonth = 0;
	size_t nCount = 0;
	for(size_t i=0; i<Size(); ++i)
	{
		if(GetAt(i).BelongYearMonth() != iYearMonth)
		{
			++nCount;
			iYearMonth = GetAt(i).BelongYearMonth();
			if(nCount == iIndex+1)break;
		}
	}
	size_t nBegin = i;

	//找到下一个月的第一条记录
	for(++i; i<Size(); ++i)
	{
		if(GetAt(i).BelongYearMonth() != iYearMonth)break;
	}
	size_t nEnd = i;

	__int64 iStatTotalSecond = 0;
	__int64 iStatTotalFlux = 0;
	//统计
	for(i=nBegin; i<nEnd; ++i)
	{
		iStatTotalSecond += GetAt(i).iTotalSecond;
		iStatTotalFlux   += GetAt(i).Flux();
	}

	//格式化输出
	int year = iYearMonth / 12;
	int month = iYearMonth % 12;
	if(month==0)
	{
		year--;
		month = 12;
	}

	CString strTemp;
	strTemp.Format(TEXT("%04d-%02d-01"), year, month);
	
	CTime tm;
	TimeFromString(strTemp, tm);
	strDate = TimeToString(tm, TIMEF_YM);

	strLastTime = SecondToTime(iStatTotalSecond);
	strFlux.Format(TEXT("%I64u"), iStatTotalFlux);

	return true;
}

void CConnectRecord::AddObserver(IConnectionRecordObserver* pObserver)
{
	CCriticalMonitor mon(m_CS);

	m_Observers.push_back(pObserver);
}

void CConnectRecord::RemoveObserver(IConnectionRecordObserver* pObserver)
{
	CCriticalMonitor mon(m_CS);

	std::vector<IConnectionRecordObserver*>::iterator iFind = 
		std::find(m_Observers.begin(), m_Observers.end(), pObserver);

	if(iFind != m_Observers.end() )
	{
		m_Observers.erase(iFind);
	}
}

void CConnectRecord::FireEvent_Add(size_t index, const TConnectInfo& info)
{
	CCriticalMonitor mon(m_CS);

	for(size_t i=0; i<m_Observers.size(); ++i)
	{
		IConnectionRecordObserver* pObserver = m_Observers[i];
		if(pObserver)
			pObserver->OnAdd(index, info);
	}
}

BOOL CConnectRecord::GetAllRecords()
{
	CCriticalMonitor mon(m_CS);

	CString strSQL;

	strSQL.Empty();
	strSQL.Format(TEXT(" Order by BegTime Desc"));
	
	return getDatasOrderByTime(strSQL,this,-1);
}

void CConnectRecord:: modifyFieldByDB(int argc, char** argv, char** columnName)
{
	TConnectInfo* pTConnectInfo = new TConnectInfo();

	pTConnectInfo->nID = atoi(argv[Util::getIndexByName(argc, columnName, "ConnectRecordID")]);
	TimeFromString(argv[Util::getIndexByName(argc, columnName, "BegTime")],pTConnectInfo->tmBeginTime);
	pTConnectInfo->iTotalSecond = atoi(argv[Util::getIndexByName(argc, columnName, "ConnectPeriod")]);
	pTConnectInfo->iSend = _atoi64(argv[Util::getIndexByName(argc, columnName, "SendNum")]);
	pTConnectInfo->iReceive = _atoi64(argv[Util::getIndexByName(argc, columnName, "RecvNum")]);

	m_Connections.push_back(pTConnectInfo);	
}

BOOL CConnectRecord::Insert(const TConnectInfo& item)
{
	
	CString strSQL = "INSERT INTO ";
	char result[32];

	

	strSQL += GettableName();
	strSQL += "(ConnectRecordID,BegTime,ConnectPeriod,SendNum,RecvNum)Values(";
	strSQL += "NULL",
	strSQL += ",'",
	strSQL += item.TimeString();
	strSQL += "','";
	strSQL += Util::intToString(item.iTotalSecond);
	strSQL += "','";
	_i64toa(item.iSend,result,10);
	strSQL += result;
	strSQL += "','";
	_i64toa(item.iReceive,result,10);
	strSQL += result;
	strSQL += "')";

	return execCommand(strSQL);
}