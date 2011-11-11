// StringTable.h: interface for the CStringTable class.
//
//////////////////////////////////////////////////////////////////////

#ifndef STIRNGSADE_TABLE
#define STIRNGSADE_TABLE

#include <map>

class CStringTable
{
public:
	static CStringTable& Instance();
	~CStringTable();
	bool GetString(UINT id, CString& str)const;
	void Load(const CString& strFile);
private:
	CStringTable();

	void Load();
private:
	CStringTable& operator=(const CStringTable&);
	CStringTable(const CStringTable&);
private:
//	std::map<UINT, CString> m_Map;

	typedef std::map<UINT, CString>::iterator iterator;
	typedef std::map<UINT, CString>::const_iterator const_iterator;
};

CString LoadStringEx(UINT id);


#endif

