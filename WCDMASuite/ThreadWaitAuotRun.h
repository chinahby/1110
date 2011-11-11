// ThreadWaitAuotRun.h: interface for the CThreadWaitAuotRun class.
//
//////////////////////////////////////////////////////////////////////

#if !defined(AFX_THREADWAITAUOTRUN_H__53FE36F7_F315_4B1E_9561_F464DC3EDC5B__INCLUDED_)
#define AFX_THREADWAITAUOTRUN_H__53FE36F7_F315_4B1E_9561_F464DC3EDC5B__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

#include "Thread.h"
#include "mobilemanager.h"

class CThreadWaitAuotRun : public CThread  
{
public:
	CThreadWaitAuotRun(CMobileManager& mgr);
	virtual ~CThreadWaitAuotRun();
protected:
	virtual void Go();
private:
	CMobileManager& m_Mgr;
};

#endif // !defined(AFX_THREADWAITAUOTRUN_H__53FE36F7_F315_4B1E_9561_F464DC3EDC5B__INCLUDED_)
