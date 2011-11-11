#ifndef AFX_THREADLOADPB_H__9B456CBE_E4F3_4A3F_8F63_EF0C75EDB2DA__INCLUDED_
#define AFX_THREADLOADPB_H__9B456CBE_E4F3_4A3F_8F63_EF0C75EDB2DA__INCLUDED_

#include "dlgprogressEx.h"

class CMobileManager;
struct TPhoneBookItem;
class CPhonebookEmbed;
class CThreadLoadPB : public CPrgressThread  
{
	CMobileManager&		m_Mgr;
	CPhonebookEmbed&	m_PB;
public:
	CThreadLoadPB(CMobileManager& m_Mgr,CPhonebookEmbed& PB);
	~CThreadLoadPB();
public:
	void OnPBRead(const TPhoneBookItem& item, int id);
	void SetIDRange(int maxID);
protected:
	virtual void DoWork();
};

#endif // !defined(AFX_THREADLOADPB_H__9B456CBE_E4F3_4A3F_8F63_EF0C75EDB2DA__INCLUDED_)
