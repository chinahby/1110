// ComParser.cpp: implementation of the CComParser class.
//
//////////////////////////////////////////////////////////////////////

#include "stdafx.h"
#include "ComParser.h"

#ifdef _DEBUG
#undef THIS_FILE
static char THIS_FILE[]=__FILE__;
#define new DEBUG_NEW
#endif


//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////

CComParser::CComParser()
{
	m_ParserBuffer = new char[512];
	m_BufferSize = 512;
}

CComParser::~CComParser()
{
	delete[] m_ParserBuffer;
	m_BufferSize = NULL;
	m_BufferSize = 0;
}

void CComParser::ResizeBuffer(size_t nSize)
{
	if(nSize>m_BufferSize)
	{
		delete[] m_ParserBuffer; 
		m_ParserBuffer = new char[nSize];
		m_BufferSize = nSize;
	}
}


//去掉左边的指定字符,返回第一个不是该字符的位置
static const char* EraseLeft(const char* str, const char* end, char trim)
{
	for(;str<end && *str == trim; ++str);
	return str;
}

//找到第一个指定字符的位置
static const char* FindFirst(const char* str, const char* end, char c)
{
	for(;str<end && *str != c; ++str);
	return str;
}
//抹去最后的指定字符
static void EraseRight(char* str, size_t size, char c)
{
	for(char* ss = str+size-1; ss>=str && *ss==c; --ss);
	if(ss<str)
		*str = 0;
	else
		*(ss+1) = 0;
}
/*
将字符串的各参数解析出来，放到m_parsedParamLink中
这个数据必须是标准的 :,,"",(), 这样的格式
*/
void CComParser::ParseString(const std::string& strParent)
{
	const char* cBegin = strParent.c_str();
	const char* cEnd = cBegin+strParent.size();
	const char* cCur = cBegin;
	m_parsedParamLink.DeleteAll();
	ResizeBuffer(strParent.size());

	char* buf = m_ParserBuffer;

	cBegin = EraseLeft(cBegin, cEnd, ' ');
	if(cBegin==cEnd)return;
	cCur = FindFirst(cBegin, cEnd, ':');
	if(cCur == cEnd)return;

	size_t strSize = cCur-cBegin;
	memcpy(buf, cBegin, strSize);
	EraseRight(buf,strSize,' ');
	if(buf[0])m_parsedParamLink.AddRecord(buf);

	cBegin = cCur+1;
	cCur = cBegin;
	while(cCur<cEnd)
	{
		cBegin = EraseLeft(cBegin, cEnd, ' ');
		if(cBegin==cEnd)break;

		if(*cBegin == '\"')//匹配引号
		{
			cCur = FindFirst(cBegin+1, cEnd, '\"');
			cCur = EraseLeft(cCur, cEnd,' ');
			if(cCur>=cEnd)break;

			strSize = cCur-cBegin-1;
			memcpy(buf, cBegin+1, strSize);
			EraseRight(buf,strSize,' ');
			if(buf[0])
				m_parsedParamLink.AddRecord(buf);
			else
				m_parsedParamLink.AddRecord("");
		}
		else if(*cBegin == '(')//匹配括号
		{
			cCur = FindFirst(cBegin+1, cEnd, ')');
			cCur = EraseLeft(cCur, cEnd,' ');
			if(cCur>=cEnd)break;

			strSize = cCur-cBegin-1;
			memcpy(buf, cBegin+1, strSize);
			EraseRight(buf,strSize,' ');
			if(buf[0])
				m_parsedParamLink.AddRecord(buf);
			else
				m_parsedParamLink.AddRecord("");
		}
		else
		{
			cCur = FindFirst(cBegin+1, cEnd, ',');
			strSize = cCur-cBegin;
			memcpy(buf, cBegin, strSize);
			EraseRight(buf,strSize,' ');
			if(buf[0])m_parsedParamLink.AddRecord(buf);
		}

		cBegin = FindFirst(cCur, cEnd, ',');
		if(cBegin >= cEnd)break;
		++cBegin;//跳过','
		cCur = cBegin;
	}
}

/*
得到ParseString解析后的数据项
*/
const std::string& CComParser::GetParsedParam(int iIndex)const
{
	static const std::string NULLString;

	ASSERT(iIndex<m_parsedParamLink.Size());
	if(iIndex>=m_parsedParamLink.Size())
		return NULLString;

	return m_parsedParamLink.GetRecord(iIndex);
}

size_t CComParser::GetParsedParamCount()const
{
	return m_parsedParamLink.Size();
}

