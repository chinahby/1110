#ifndef _SMSEMBED_H_
#define _SMSEMBED_H_

#include <vector>
#include "Data.h"
#include "SMS.h"

class CSMSEmbed : public CSMS
{
public:
	CSMSEmbed();
	~CSMSEmbed();
	
	virtual void Clear();
	virtual TSMSItem& GetAt(size_t index);
	virtual const TSMSItem& GetAt(size_t index)const;
	virtual void Delete(size_t index);
	void DeleteByID(int id);
	virtual bool FindID(int id)const;
	virtual TSMSItem& AddBack(const TSMSItem& item);
	virtual TSMSItem& AddBack();
	virtual TSMSItem& AddFront(const TSMSItem& item);
	virtual TSMSItem& AddFront();

	virtual size_t Size()const;

	void operator+=(const TSMSItem& item);
	void operator+=(const CSMSEmbed& list);

	virtual void SetLimit(size_t nLimit);
	virtual size_t GetLimit()const;

	void SortByTime(bool bUporder = false);
	bool Find(const TSMSItem& item)const;

	virtual void AddObserver(ISMSObserver* pObserver);
	virtual void RemoveObserver(ISMSObserver* pObserver);
private:
	void FireEvent_SMSAdd(size_t index, const TSMSItem& sms);
	void FireEvent_Sort();
private://Ω˚÷π∑√Œ 
	CSMSEmbed& operator=(const CSMSEmbed&);
	CSMSEmbed(const CSMSEmbed&);
private:
	std::vector<TSMSItem*> m_Data;
	size_t m_nLimit;

	std::vector<ISMSObserver*> m_Observers;
	mutable CRITICAL_SECTION		m_CS;
};

#endif