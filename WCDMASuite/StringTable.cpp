#include "stdafx.h"
#include "StringTable.h"
#include <fstream>

CStringTable& CStringTable::Instance()
{
	static CStringTable s_Instance;

	return s_Instance;
}

CStringTable::~CStringTable()
{

}

bool CStringTable::GetString(UINT id, CString& str)const
{
	const_iterator it = m_Map.find(id);
	if(it != m_Map.end())
	{
		str = it->second;
		return true;
	}
	
	return false;
}

CStringTable::CStringTable()
{
}

void CStringTable::Load(const CString& strFile)
{
	CString sFile = strFile;
	std::ifstream file(sFile);
	if(!file.is_open())return;

	char buf[1024];
	while(!file.eof())
	{
		memset(buf,0, sizeof(buf));
		file.getline(buf,sizeof(buf));
		CString str = buf;
		if(!str.GetLength())continue;
		int nFind = str.Find(";;;");//使用;;;作为ID和字符串的分割标志
		if(nFind == -1)continue;
		CString strNum = str.Left(nFind);
		if(!strNum.GetLength())continue;
		UINT nID = atoi(strNum);
		CString strV = str.Right(str.GetLength()-nFind-3);
		nFind = strV.Find('\r');
		if(nFind!=-1)strV.Delete(nFind);
		if(!strV.GetLength())continue;
		strV.Replace("\\n","\n");
		m_Map[nID] = strV;
	}
}

CString LoadStringEx(UINT id)
{
	CString str;
	if(!CStringTable::Instance().GetString(id, str))
		str.LoadString(id);

	return str;
}

