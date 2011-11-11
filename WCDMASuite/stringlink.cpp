#include "stdafx.h"
#include "StringLink.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////

CStringLink::CStringLink()
:m_nSize(0)
{
}

CStringLink::~CStringLink()
{
}

/*
删除所有记录(字符串)
*/
void CStringLink::DeleteAll()
{
	//这里并不删除真正的内部存储
	//而是仅仅将数据的个数改为0，
	//内部对象作为对象池给下一次使用,
	//可以以高效率
	m_nSize = 0;
}

/*
在队列尾部添加记录(字符串)，并返回新添加的记录的序号
*/
size_t CStringLink:: AddRecord(const std::string& inStrLine)
{
	if(m_nSize==m_Data.size())
	{
		m_Data.push_back(inStrLine);
		++m_nSize;
	}
	else
	{
		m_Data[m_nSize] = inStrLine;
		++m_nSize;
	}
	return m_nSize-1;
}

const std::string& CStringLink::GetRecord(size_t iIndex)const
{
	ASSERT(iIndex<m_nSize);

	return m_Data[iIndex];
}

size_t CStringLink::Size()const
{
	return m_nSize;
}

