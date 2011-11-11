
#if !defined(AFX_AUTORUN_H__79A3F1F7_E7C1_4A74_B69C_AB5B5956512F__INCLUDED_)
#define AFX_AUTORUN_H__79A3F1F7_E7C1_4A74_B69C_AB5B5956512F__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

#include "resource.h"
#include <map>

#define IDS_APPNAME                     145

class CStringTable
{
public:
	static CStringTable& Instance(CString strDir);
	~CStringTable();
	bool GetString(UINT id, CString& str)const;
private:
	CStringTable(CString strDir);

	void Load(CString strDir);
private:
	CStringTable& operator=(const CStringTable&);
	CStringTable(const CStringTable&);
private:
	std::map<UINT, CString> m_Map;

	typedef std::map<UINT, CString>::iterator iterator;
	typedef std::map<UINT, CString>::const_iterator const_iterator;
};

bool IsSetup(CString& strDir);
HANDLE RunProgram(const CString& strCmdLine, bool bShowWindow);
bool StartProcess(const CString& strProcessName);
CString GetAppPath();
void ChgAutoRegedit();

#endif // !defined(AFX_AUTORUN_H__79A3F1F7_E7C1_4A74_B69C_AB5B5956512F__INCLUDED_)
