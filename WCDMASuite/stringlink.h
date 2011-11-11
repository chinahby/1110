#if !defined(AFX_RXLINELINK1_H__81746CC3_0B63_41F8_8071_BA643C2BF0FA__INCLUDED_)
#define AFX_RXLINELINK1_H__81746CC3_0B63_41F8_8071_BA643C2BF0FA__INCLUDED_

#include <vector>

class CStringLink  
{
public:	
	void DeleteAll();
	size_t AddRecord(const std::string& inStrLine);
	const std::string& GetRecord(size_t iIndex)const;
	size_t Size()const;

	CStringLink();
	~CStringLink();
private:
	size_t m_nSize;
	std::vector<std::string> m_Data;
	typedef std::vector<std::string>::iterator ITERATOR;
	typedef std::vector<std::string>::const_iterator CONST_ITERATOR;
};

#endif 
