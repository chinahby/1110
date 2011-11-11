#ifndef AFX_TXBUFFERLINK_H__C19E50DE_6999_4A8A_8E29_4A678BB63D0C__INCLUDED_
#define AFX_TXBUFFERLINK_H__C19E50DE_6999_4A8A_8E29_4A678BB63D0C__INCLUDED_

#include "GlobalDataCenter.h"
#include "MobileATProto.h"
#include <list>
#include <Afxmt.h>

/**
*	发送的AT命令缓冲类.
*	该类用来缓存需要发送的AT命令.该类是线程安全的.
*/
class CATCmdQ  
{
public:
	//构造函数
	CATCmdQ();

	//析构函数
	~CATCmdQ();

	//取出头部
	bool PopTopRecord(CATCmd & Record);

	//加到尾部
	void AddBack(const CATCmd & Record);

	//加到头部
	void AddFront(const CATCmd& Record);

	//删除所有
	void DeleteAll();

	//查询总数
	size_t Size()const
	{
		return m_Data.size();
	}

	HANDLE	EventNotEmpty(){return m_EventQ;}
private://禁止访问copy constructor 和 operator=
	CATCmdQ(const CATCmdQ&);
	CATCmdQ& operator=(const CATCmdQ&);
protected:
	CEvent				m_EventQ;	//队列非空
	CCriticalSection	m_cs; //缓存列表临界区
	std::list<CATCmd>	m_Data;//缓存的数据

	typedef std::list<CATCmd>::iterator ITERATOR;
	typedef std::list<CATCmd>::const_iterator CONST_ITERATOR;
};

#endif
