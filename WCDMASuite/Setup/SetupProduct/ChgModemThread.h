// ChgModemThread.h: interface for the CChgModemThread class.
//
//////////////////////////////////////////////////////////////////////

#if !defined(AFX_CHGMODEMTHREAD_H__9646B83C_F272_448F_8BB8_DF677742D52E__INCLUDED_)
#define AFX_CHGMODEMTHREAD_H__9646B83C_F272_448F_8BB8_DF677742D52E__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

#include "Thread.h"

class CChgModemThread : public CThread  
{
public:
	CChgModemThread();
	virtual ~CChgModemThread();
protected:
	virtual void Go();
};

#endif // !defined(AFX_CHGMODEMTHREAD_H__9646B83C_F272_448F_8BB8_DF677742D52E__INCLUDED_)
