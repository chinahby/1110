#ifndef SMS_HEADER_5F6FCE2A453545f1BBCCE862BF35C08D
#define SMS_HEADER_5F6FCE2A453545f1BBCCE862BF35C08D

#include <vector>
#include "Data.h"
#include "SMS.h"


class CSMSPC : public Data,public CSMS
{
public:
	CSMSPC();
	~CSMSPC();

	virtual void Clear();					//清空当前信箱,回收站为彻底删除,其他为放入回收站
	virtual TSMSItem& GetAt(size_t nIndex);	//得到一条信息
	virtual const TSMSItem& GetAt(size_t nIndex)const;	//得到一条信息
	virtual void Delete(size_t nID);	//删除一条信息
	virtual bool FindID(int id)const;	
	
	virtual TSMSItem& AddBack(const TSMSItem& item);	//增加一条信息在最后同时更新数据库
	virtual TSMSItem& AddFront(const TSMSItem& item);	//增加一条信息在最前同时更新数据库
	void ForeverDelete(size_t nID);					

	virtual size_t Size()const;							//返回信息条数

	void operator+=(const TSMSItem& item);
	void operator+=(const CSMSPC& list);

	virtual void SetLimit(size_t nLimit);
	virtual size_t GetLimit()const;

	virtual bool Find(const TSMSItem& item)const;

	virtual void AddObserver(ISMSObserver* pObserver);
	virtual void RemoveObserver(ISMSObserver* pObserver);
	BOOL UpdateStatus(int index);
	BOOL GetRecordsByMailBoxID(size_t nMailBoxID);	//根据信箱ID得到当前信箱全部信息
	BOOL GetRecordsByFilter(CString filter, int maxRowCount = -1);	//根据SQL语句得到当前信箱全部信息
	void MoveAllRecordsToRecyle();				//移动当前信箱所有信息到回收站
	BOOL MoveOneRecordToRecyle(size_t nID);		//移动当前信息到回收站
	
	BOOL RestoreOneRecord(size_t nID);			//从回收站中还原一条信息
	void RestoreAllRecords();					//从回收站中还原所有信息
	void ClearVector();
	TSMSItem& CSMSPC::FindRecordByIndex(size_t id);	
	int FindIndexByID(size_t nID)const;

private:
	void FireEvent_SMSAdd(size_t index, const TSMSItem& sms);
	void FireEvent_Sort();

	virtual void  modifyFieldByDB(int argc, char** argv, char** columnName);	

	BOOL Insert(TSMSItem& item);				//插入一条信息到数据库
								//清空内存Vector
	BOOL getBlobDatasByFilter(CString const filter);
	
private://禁止访问
	CSMSPC& operator=(const CSMSPC&);
	CSMSPC(const CSMSPC&);
private:
	std::vector<TSMSItem*> m_Data;
	size_t m_nLimit;
	size_t m_nMailBoxID;	//当前信箱ID

	std::vector<ISMSObserver*> m_Observers;
	mutable CRITICAL_SECTION		m_CS;

	sqlite3* m_pDB;
};

/*
CREATE TABLE SC700_SMS(SMSID integer PRIMARY KEY,
					   MailBoxID int default(0),
					   OrigMailBoxID int default(0),
					   Flag int default(0),
					   Telephone char(20) default(NULL),
					   Text blob,
					   Time char(30) default(NULL));
*/
#endif
