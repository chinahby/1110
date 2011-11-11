#ifndef CONNECTRECORD_7965C2BBCF8F4f47B74F12B535D3673A
#define CONNECTRECORD_7965C2BBCF8F4f47B74F12B535D3673A

#include <vector>
#include "Data.h"
#include <winbase.h>

struct TConnectInfo
{
	int		nID;		//上网记录唯一ID
	CTime tmBeginTime;  //开始时间
	unsigned int iTotalSecond; //总共连接的秒数
	__int64 iSend; //发送量
	__int64 iReceive; //接收量

	__int64 Flux()const
	{
		return iSend + iReceive;
	}
	unsigned int BelongYearMonth()const;

	TConnectInfo();
	CString TimeString()const;
};

struct IConnectionRecordObserver
{
	virtual void OnAdd(size_t index, const TConnectInfo& info) = 0;
};

class CConnectRecord : public Data
{
public:
	CConnectRecord();
	~CConnectRecord();

	size_t Size()const;				//得到上网记录数
	const TConnectInfo& GetAt(size_t index)const;	//得到一条上网记录
	void AddFront(const TConnectInfo& info);		//增加一条上网记录在前面
	void AddTail(const TConnectInfo& info);			//增加一条上网记录在后面
	void Clear();									//清空所有上网记录
	void Delete(size_t nID);						//删除一条上网记录
	
	void GetStatTime(CTime tmStartTime, CTime tmEndTime,
		CString &strTime, CString &strFlux)const;
	void CConnectRecord::GetStatTime(CTime tmStartTime, 
								 CTime tmEndTime, 
								 __int64& i64FluxUpLoad,
								 __int64& i64FluxDownLoad) const;

	size_t GetMonthStatCount()const;
	bool GetMonthStat(size_t iIndex, 
			CString &strDate, 
			CString &strLastTime, 
			CString &strFlux)const;
	void ClearVector();
	void AddObserver(IConnectionRecordObserver* pObserver);
	void RemoveObserver(IConnectionRecordObserver* pObserver);

	BOOL GetAllRecords();			//得到所有上网记录

	int FindIndexByID(size_t id)const;
	
private:
	virtual void  modifyFieldByDB(int argc, char** argv, char** columnName);
	
	BOOL Insert(const TConnectInfo& item);	//插入一条上网记录信息
	void FireEvent_Add(size_t index, const TConnectInfo& info);
private:
	CConnectRecord(const CConnectRecord&);
	CConnectRecord& operator=(const CConnectRecord&);
private:
	std::vector<TConnectInfo*> m_Connections;
	std::vector<IConnectionRecordObserver*> m_Observers;
	mutable CRITICAL_SECTION m_CS;

	sqlite3* m_pDB;
};

/*
CREATE TABLE SC700_ConnectRecord(ConnectRecordID integer PRIMARY KEY,
							 BegTime char(30) default(NULL),
							 ConnectPeriod char(30) default(NULL),
							 SendNum char(30) default(NULL),
							 RecvNum char(30) default(NULL))
*/

#endif
