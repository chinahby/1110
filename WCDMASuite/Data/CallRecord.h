#ifndef AFX_CALLRECORD_H__078CC5AD_2C0A_48B1_9CBE_D725AF866E63__INCLUDED_
#define AFX_CALLRECORD_H__078CC5AD_2C0A_48B1_9CBE_D725AF866E63__INCLUDED_

#include <vector>
#include "Data.h"

//通话类型
enum ECallType
{
	CALL_INCOMING,	//打进
	CALL_OUTGOING,	//打出
};

enum CallRecordType
{
	CALLRECORD_RECV = 0,	//已接电话
	CALLRECORD_DAIL,		//已拨电话
	CALLRECORD_MISS			//未接电话
};


struct TCallInfo
{
	int		  nID;		//通话记录唯一ID
	CallRecordType eRecordType;	//通话记录类型
	ECallType type;		//类型
	CString   strTelephone;	//号码
	CString   strName;		//姓名
	CTime	  begin;	//开始时间
	DWORD	  period;	//呼叫时长
	
	TCallInfo();
	CString TimeString()const;
	bool IsMissingCall()const;
	bool IsFailingCall()const;
};

class CCallRecord;
struct ICallRecordObserver
{
	virtual void OnAdd(CCallRecord* pSender, int nIndex, const TCallInfo& newItem) = 0;
};

class CCallRecord : public Data 
{
public:
	CCallRecord();
	~CCallRecord();

	size_t Size()const;							//得到通话记录条数
	TCallInfo& GetAt(size_t index);				//得到一条通话记录
	const TCallInfo& GetAt(size_t index)const;	//得到一条通话记录
	TCallInfo& Add(const TCallInfo& item);		//新增一条通话记录
	void Delete(size_t index);					//删除一条通话记录
	void Clear();								//清除所有通话记录

	DWORD GetCallInTime()const;					//计算打入电话时间
	DWORD GetCallOutTime()const;				//计算打出电话时间

	void AddObserver(ICallRecordObserver* pObserver);
	void RemoveObserver(ICallRecordObserver* pObserver);

	BOOL GetRecordsByRecordType(size_t nRecordType);		//根据类型得到通话记录
	void ClearVector();							//清空内存Vector
private:
	CCallRecord(const CCallRecord&);
	CCallRecord& operator=(const CCallRecord&);
private:
	DWORD CalculateTime(ECallType type)const;
	void  FireEvent_Add(int nIndex, const TCallInfo& newItem);

	virtual void  modifyFieldByDB(int argc, char** argv, char** columnName);
	BOOL Insert(TCallInfo& item);				//插入一条信息到数据库
	int FindIndexByID(size_t nID)const;
	
private:
	std::vector<TCallInfo*> m_Data;
	std::vector<ICallRecordObserver*>	m_Observers;

	mutable CRITICAL_SECTION		m_CS;//临界区保护

	size_t m_nRecordType;				//当前通话记录类型

	sqlite3* m_pDB;
};

/*
CREATE TABLE SC700_CallRecord(CallRecordID integer PRIMARY KEY,
							 RecordType int default(0),
							 Flag int (0),
							 Telephone char(20) default(NULL),
							 Name char(50) default(NULL),
							 BegTime char(30) default(NULL),
							 CallPeriod char(30) default(NULL))
*/

#endif // !defined(AFX_CALLRECORD_H__078CC5AD_2C0A_48B1_9CBE_D725AF866E63__INCLUDED_)
