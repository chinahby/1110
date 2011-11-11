#include "StdAfx.h"
#include "SMSPC.h"
#include "../CriticalSecMon.h"
#include "../Util.h"
#include <algorithm>


#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif



////////////////////////////////////////////////////
//	实现 class CSMSList
////////////////////////////////////////////////////
CSMSPC::CSMSPC()
{
	InitializeCriticalSection(&m_CS);

	SetTable("SC700_SMS");
}

CSMSPC::~CSMSPC()
{
	ClearVector();
	DeleteCriticalSection(&m_CS);
}

void CSMSPC::ClearVector()
{
	CCriticalMonitor mon(m_CS);

	for(size_t i=0; i<Size(); ++i)
	{
		delete m_Data[i];
	}
	m_Data.clear();
}

void CSMSPC::Clear()
{
	CString strSQL;

	if(m_nMailBoxID >= SMS_MAILBOX_RECYLE)
	{
		strSQL.Format(TEXT("MailBoxID = %d"),m_nMailBoxID);
		
		removeDatasByFilter(strSQL);
	}
	else
	{
		MoveAllRecordsToRecyle();
	}
	
	ClearVector();
		
}

TSMSItem& CSMSPC::GetAt(size_t nIndex)
{
	ASSERT(nIndex<Size());
	return *m_Data[nIndex];
}

const TSMSItem& CSMSPC::GetAt(size_t nIndex)const
{
	ASSERT(nIndex<Size());
	return *m_Data[nIndex];
}

void CSMSPC::Delete(size_t nID)
{
	CCriticalMonitor mon(m_CS);
	int nIndex = FindIndexByID(nID);
	ASSERT(nIndex >= 0);
	ASSERT(nIndex < Size());
	delete m_Data[nIndex];
	m_Data.erase(m_Data.begin()+nIndex);

	CString strSQL;
	if(m_nMailBoxID >= SMS_MAILBOX_RECYLE)
	{
		strSQL.Format(TEXT("SMSID = %d"),nID);
		removeDatasByFilter(strSQL);
	}
	else
	{
		MoveOneRecordToRecyle(nID);
	}
}

void CSMSPC::ForeverDelete(size_t nID)
{
	CCriticalMonitor mon(m_CS);
	int nIndex = FindIndexByID(nID);
	ASSERT(nIndex >= 0);
	ASSERT(nIndex < Size());
	delete m_Data[nIndex];
	m_Data.erase(m_Data.begin()+nIndex);

	CString strSQL;

		strSQL.Format(TEXT("SMSID = %d"),nID);
		removeDatasByFilter(strSQL);

}
bool CSMSPC::FindID(int id)const
{
	CCriticalMonitor mon(m_CS);

	for(size_t i=0; i<Size(); ++i)
	{
		if(m_Data[i]->iID == id)
			return true;
	}
	return false;
}


int CSMSPC::FindIndexByID(size_t id)const
{
	CCriticalMonitor mon(m_CS);

	for(size_t i=0; i<Size(); ++i)
	{
		if(m_Data[i]->iID == id)
			return i;
	}
	return -1;
}

TSMSItem& CSMSPC::FindRecordByIndex(size_t id)
{
	int iID = FindIndexByID(id);
	return GetAt(iID);
}

TSMSItem& CSMSPC::AddBack(const TSMSItem& item)
{
	CCriticalMonitor mon(m_CS);

	TSMSItem* newItem = new TSMSItem(item);
	m_Data.push_back(newItem);

	Insert(* newItem);
	TRACE(TEXT("m_PCDraft.Size() = %d\n"), this->Size());
	return *newItem;
}


TSMSItem& CSMSPC::AddFront(const TSMSItem& item)
{
	CCriticalMonitor mon(m_CS);

	TSMSItem* newItem = new TSMSItem(item);
	m_Data.insert(m_Data.begin(), newItem);

	Insert(* newItem);

	return *newItem;
}

size_t CSMSPC::Size()const
{
	return m_Data.size();
}


void CSMSPC::operator+=(const TSMSItem& item)
{
	AddBack(item);
}

void CSMSPC::operator+=(const CSMSPC& list)
{
	CCriticalMonitor mon(m_CS);

	size_t nSize = list.Size();
	for(size_t i=0; i<nSize; ++i)
	{
		AddBack(list.GetAt(i));
	}
}

void CSMSPC::SetLimit(size_t nLimit)
{
	m_nLimit = nLimit;
}

size_t CSMSPC::GetLimit()const
{
	return m_nLimit;
}



//比较函数，判断是否相等
struct SMSItemEqual
{
	SMSItemEqual(const TSMSItem* p2)
	{
		m_p2 = p2;
	}
	bool operator()(const TSMSItem* p1)const
	{
		return *p1 == *m_p2;
	}
private:
	const TSMSItem* m_p2;
};

bool CSMSPC::Find(const TSMSItem& item)const
{
	return m_Data.end() != std::find_if(m_Data.begin(), m_Data.end(), SMSItemEqual(&item));
}

void CSMSPC::AddObserver(ISMSObserver* pObserver)
{
	CCriticalMonitor mon(m_CS);

	m_Observers.push_back(pObserver);
}

void CSMSPC::RemoveObserver(ISMSObserver* pObserver)
{
	CCriticalMonitor mon(m_CS);

	std::vector<ISMSObserver*>::iterator iFind = 
		std::find( m_Observers.begin(), m_Observers.end(), pObserver);

	if(iFind != m_Observers.end())
		m_Observers.erase(iFind);
}

void CSMSPC::FireEvent_SMSAdd(size_t index, const TSMSItem& sms)
{
	CCriticalMonitor mon(m_CS);

	for(size_t i=0; i<m_Observers.size(); ++i)
	{
		if(m_Observers[i])
			m_Observers[i]->OnSMSAdd(index, sms);
	}
}

void CSMSPC::FireEvent_Sort()
{
	CCriticalMonitor mon(m_CS);

	for(size_t i=0; i<m_Observers.size(); ++i)
	{
		if(m_Observers[i])
			m_Observers[i]->OnSMSSort();
	}
}

BOOL CSMSPC::GetRecordsByMailBoxID(size_t nMailBoxID)
{
	TRACE(TEXT("CSMSPC::GetRecordsByMailBoxID :> ENTER"));
	m_nMailBoxID = nMailBoxID;

	CString strSQL;
	strSQL.Format(TEXT("MailBoxID = %d"),nMailBoxID);
	strSQL += TEXT(" ORDER BY Time DESC");
	
	return getBlobDatasByFilter(strSQL);
}

BOOL CSMSPC::GetRecordsByFilter(CString const filter,int maxRowCount)
{
	TRACE(TEXT("CSMSPC::GetRecordsByFilter :> ENTER"));
	return getBlobDatasByFilter(filter);
}

static CCriticalSection g_CS;

BOOL CSMSPC::getBlobDatasByFilter(CString const filter)
{
	TRACE(TEXT("CSMSPC::getBlobDatasByFilter :> ENTER"));
	g_CS.Lock();

	BOOL bRet = TRUE;
	CString cmd = "SELECT";
	cmd += " * FROM [";
	cmd += "SC700_SMS";
	cmd += "]";
	if (filter != "")
	{
		cmd += " WHERE ";
		cmd += filter;
	}
	
	char* errorMessage = 0;
	std::string str = ToMultiBytes(cmd,cmd.GetLength());
	sqlite3_stmt      *stat = 0;
	int rc = sqlite3_prepare(Data::GetClobalDB(),str.c_str(), -1, &stat, 0);
	if(rc!= SQLITE_OK)
	{
		g_CS.Unlock();
		return FALSE;
	}
	rc = sqlite3_step(stat);
	int i = 0;
	while(rc == SQLITE_ROW)
	{
		TSMSItem* pTSMSItem = new TSMSItem();

		pTSMSItem->iID = sqlite3_column_int(stat,0);
		pTSMSItem->nMailBoxID = (SMSMailBoxID)sqlite3_column_int(stat,1);
		pTSMSItem->nOrigMailBoxID = (SMSMailBoxID)sqlite3_column_int(stat,2);
		pTSMSItem->nPSMSState = (PSMSType)sqlite3_column_int(stat,3);
		pTSMSItem->strPhoneNum = sqlite3_column_text(stat,4);
		
		const void * pBlob = sqlite3_column_blob(stat, 5);
		const unsigned char* time = sqlite3_column_text(stat,6);
		TimeFromString(time,pTSMSItem->tmSMSTime);
		int size = sqlite3_column_bytes(stat, 5);
		TCHAR ch[2100] = {0};
		memcpy(ch,pBlob,size);
		pTSMSItem->strText = ch;
		m_Data.push_back(pTSMSItem);
		
		rc = sqlite3_step(stat);
	}

	rc = sqlite3_finalize(stat);

	g_CS.Unlock();
	
	return bRet;
}


void CSMSPC:: modifyFieldByDB(int argc, char** argv, char** columnName)
{
	TSMSItem* pTSMSItem = new TSMSItem();

	pTSMSItem->iID = atoi(argv[Util::getIndexByName(argc, columnName, "SMSID")]);
	pTSMSItem->nMailBoxID = (SMSMailBoxID)atoi(argv[Util::getIndexByName(argc, columnName, "MailBoxID")]);
	pTSMSItem->nOrigMailBoxID = (SMSMailBoxID)atoi(argv[Util::getIndexByName(argc, columnName, "OrigMailBoxID")]);
	TimeFromString(argv[Util::getIndexByName(argc, columnName, "Time")],pTSMSItem->tmSMSTime);
	//pTSMSItem->strText = argv[Util::getIndexByName(argc, columnName, "Text")];
	pTSMSItem->strPhoneNum = argv[Util::getIndexByName(argc, columnName, "Telephone")];
	pTSMSItem->nPSMSState = (PSMSType)atoi(argv[Util::getIndexByName(argc, columnName, "Flag")]);
	
	m_Data.push_back(pTSMSItem);	
}

void CSMSPC::MoveAllRecordsToRecyle()
{
	BOOL bReturn = FALSE;

	for(size_t i=0; i<Size(); i++)
	{		
		MoveOneRecordToRecyle(m_Data[i]->iID);
	}
}

BOOL CSMSPC::UpdateStatus(int index)
{
	CString strSQL = "UPDATE ";
	strSQL += GettableName();
	strSQL += " SET Flag = ";
	strSQL += Util::intToString(SMS_PC_RECV);
	strSQL += " WHERE SMSID = ";
	strSQL += Util::intToString(index);

	int id = FindIndexByID(index);
	m_Data[id]->nPSMSState = (PSMSType)0;
	
	return execCommand(strSQL);
}

BOOL CSMSPC::MoveOneRecordToRecyle(size_t nID)
{
	CString strSQL = "UPDATE ";

	strSQL += GettableName();
	strSQL += " SET MailBoxID = ";
	strSQL += Util::intToString(SMS_MAILBOX_RECYLE);
	strSQL += " WHERE SMSID = ";
	strSQL += Util::intToString(nID);

	return execCommand(strSQL);
}

BOOL CSMSPC::Insert(TSMSItem& item)
{
	CString str;
	//str = ChangeStrToStr(item.strText);
	str = item.strText;
	CString strSQL = "INSERT INTO ";

	strSQL += GettableName();
	strSQL += "(SMSID,MailBoxID,OrigMailBoxID,Flag,Telephone,Text,Time)Values(";
	strSQL += "NULL",
	strSQL += ",",
	strSQL += Util::intToString(item.nMailBoxID);
	strSQL += ",";
	strSQL += Util::intToString(item.nOrigMailBoxID);
	strSQL += ",";
	strSQL += Util::intToString(item.nPSMSState);
	strSQL += ",'";
	strSQL += item.strPhoneNum;
	strSQL += "',";
	strSQL += "?";
	strSQL += ",'";
	strSQL += item.TimeString();
	strSQL += "')";

	return exeBlobCommand(strSQL,str);
}

BOOL CSMSPC::RestoreOneRecord(size_t nID)
{

	int nIndex = FindIndexByID(nID);
	int nMailBoxID = m_Data[nIndex]->nOrigMailBoxID;
	CString strSQL = "UPDATE ";
	
	strSQL += GettableName();
	strSQL += " SET MailBoxID = ";
	strSQL += Util::intToString(nMailBoxID);
	strSQL += " WHERE SMSID = ";
	strSQL += Util::intToString(nID);
	return execCommand(strSQL);
}

void CSMSPC::RestoreAllRecords()
{
	for(int i=0; i < Size(); i++)
	{
		RestoreOneRecord(m_Data[i]->iID);
	}
}