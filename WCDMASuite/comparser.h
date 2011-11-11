// ComParser.h: interface for the CComParser class.
//
//////////////////////////////////////////////////////////////////////

#if !defined(AFX_COMPARSER_H__1B8D70F7_6CEA_4490_B565_0BF69AEA9622__INCLUDED_)
#define AFX_COMPARSER_H__1B8D70F7_6CEA_4490_B565_0BF69AEA9622__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

#include "StringLink.h"

class CComParser  
{
public:
	CComParser();
	virtual ~CComParser();

	void ParseString(const std::string& strParent);
	const std::string& GetParsedParam(int iIndex)const;
	size_t GetParsedParamCount()const;
private:
	void ResizeBuffer(size_t nSize);
private:
	CStringLink m_parsedParamLink;
	char*		m_ParserBuffer;
	size_t		m_BufferSize;
};

#endif // !defined(AFX_COMPARSER_H__1B8D70F7_6CEA_4490_B565_0BF69AEA9622__INCLUDED_)
