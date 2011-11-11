#ifndef CRITICAL_SECTION_MONITOR_
#define CRITICAL_SECTION_MONITOR_


#include <afxmt.h>

/**
*	简单的临界区监视类.
*	利用对象的生存期来实现自动的进出临界区
*/
class CCriticalMonitor
{
public:
	CCriticalMonitor(CRITICAL_SECTION& obj)
		:m_obj(obj)
	{
		EnterCriticalSection(&m_obj);
	}

	CCriticalMonitor(CCriticalSection& obj)
		:m_obj(obj.m_sect)
	{
		EnterCriticalSection(&m_obj);
	}

	~CCriticalMonitor()
	{
		LeaveCriticalSection(&m_obj);
	}
private://禁止拷贝构造和operator=
	CCriticalMonitor(const CCriticalMonitor&);
	CCriticalMonitor& operator=(const CCriticalMonitor&);
private:
	CRITICAL_SECTION& m_obj;
};

#endif