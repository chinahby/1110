#include "StdAfx.h"
#include "SMSEmbed.h"
#include "../CriticalSecMon.h"
#include <algorithm>

CSMSEmbed::CSMSEmbed()
{
	InitializeCriticalSection(&m_CS);
}

CSMSEmbed::~CSMSEmbed()
{
	Clear();
	DeleteCriticalSection(&m_CS);
}

void CSMSEmbed::Clear()
{
	CCriticalMonitor mon(m_CS);

	for(size_t i=0; i<Size(); ++i)
	{
		delete m_Data[i];
	}
	m_Data.clear();
}

TSMSItem& CSMSEmbed::GetAt(size_t index)
{
	ASSERT(index<Size());
	return *m_Data[index];
}

const TSMSItem& CSMSEmbed::GetAt(size_t index)const
{
	ASSERT(index<Size());
	return *m_Data[index];
}

void CSMSEmbed::Delete(size_t index)
{
	CCriticalMonitor mon(m_CS);
	int smsSize = Size();
	ASSERT(index < smsSize);
	delete m_Data[index];
	m_Data.erase(m_Data.begin()+index);
}

void CSMSEmbed::DeleteByID(int id)
{
	CCriticalMonitor mon(m_CS);

	for(size_t i=0; i<Size(); ++i)
	{
		if(m_Data[i]->iID == id)
		{
			Delete(i);
			break;
		}
	}
}

bool CSMSEmbed::FindID(int id)const
{
	CCriticalMonitor mon(m_CS);

	for(size_t i=0; i<Size(); ++i)
	{
		if(m_Data[i]->iID == id)
			return true;
	}
	return false;
}

TSMSItem& CSMSEmbed::AddBack(const TSMSItem& item)
{
	CCriticalMonitor mon(m_CS);

	TSMSItem* newItem = new TSMSItem(item);
	m_Data.push_back(newItem);
	return *newItem;
}

TSMSItem& CSMSEmbed::AddBack()
{
	CCriticalMonitor mon(m_CS);

	TSMSItem* newItem = new TSMSItem;
	m_Data.push_back(newItem);

	FireEvent_SMSAdd(m_Data.size()-1, *newItem);

	return *newItem;
}

TSMSItem& CSMSEmbed::AddFront(const TSMSItem& item)
{
	CCriticalMonitor mon(m_CS);

	TSMSItem* newItem = new TSMSItem(item);
	m_Data.insert(m_Data.begin(), newItem);

	FireEvent_SMSAdd(0, *newItem);
	return *newItem;
}

TSMSItem& CSMSEmbed::AddFront()
{
	CCriticalMonitor mon(m_CS);

	TSMSItem* newItem = new TSMSItem;
	m_Data.insert(m_Data.begin(), newItem);
	return *newItem;
}

size_t CSMSEmbed::Size()const
{
	return m_Data.size();
}

void CSMSEmbed::operator+=(const TSMSItem& item)
{
	AddBack(item);
}

void CSMSEmbed::operator+=(const CSMSEmbed& list)
{
	CCriticalMonitor mon(m_CS);

	size_t nSize = list.Size();
	for(size_t i=0; i<nSize; ++i)
	{
		AddBack(list.GetAt(i));
	}
}

void CSMSEmbed::SetLimit(size_t nLimit)
{
	m_nLimit = nLimit;
}

size_t CSMSEmbed::GetLimit()const
{
	return m_nLimit;
}

//比较函数，时间早的放在前面
struct CompareByTimeUpOrder
{
	bool operator()(const TSMSItem* p1, const TSMSItem* p2)const
	{
		return (p1->tmSMSTime < p2->tmSMSTime) == TRUE;
	}
};
//比较函数，时间晚的放在前面
struct CompareByTimeDownOrder
{
	bool operator()(const TSMSItem* p1, const TSMSItem* p2)const
	{
		return (p1->tmSMSTime > p2->tmSMSTime) == TRUE;
	}
};
void CSMSEmbed::SortByTime(bool bUporder)
{
	
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
bool CSMSEmbed::Find(const TSMSItem& item)const
{
	return m_Data.end() != std::find_if(m_Data.begin(), m_Data.end(), SMSItemEqual(&item));
}

void CSMSEmbed::AddObserver(ISMSObserver* pObserver)
{
	CCriticalMonitor mon(m_CS);

	m_Observers.push_back(pObserver);
}
void CSMSEmbed::RemoveObserver(ISMSObserver* pObserver)
{
	CCriticalMonitor mon(m_CS);

	std::vector<ISMSObserver*>::iterator iFind = 
		std::find( m_Observers.begin(), m_Observers.end(), pObserver);

	if(iFind != m_Observers.end())
		m_Observers.erase(iFind);
}

void CSMSEmbed::FireEvent_SMSAdd(size_t index, const TSMSItem& sms)
{
	CCriticalMonitor mon(m_CS);

	for(size_t i=0; i<m_Observers.size(); ++i)
	{
		if(m_Observers[i])
			m_Observers[i]->OnSMSAdd(index, sms);
	}
}

void CSMSEmbed::FireEvent_Sort()
{
	CCriticalMonitor mon(m_CS);

	for(size_t i=0; i<m_Observers.size(); ++i)
	{
		if(m_Observers[i])
			m_Observers[i]->OnSMSSort();
	}
}