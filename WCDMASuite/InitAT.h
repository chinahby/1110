// InitAT.h: interface for the CInitAT class.
//
//////////////////////////////////////////////////////////////////////

#if !defined(AFX_INITAT_H__7A46CB52_0C05_4BE6_8BA2_1AAFE6827D1A__INCLUDED_)
#define AFX_INITAT_H__7A46CB52_0C05_4BE6_8BA2_1AAFE6827D1A__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

#include "Thread.h"
#include "mobilemanager.h"

class CInitAT : public CThread  
{
public:
	CInitAT(CMobileManager& mgr);
	virtual ~CInitAT();
public:
	bool IsInitRunning(){return m_bRunning;}
protected:
	virtual void Go();
private:
	CMobileManager& m_Mgr;
	bool m_bIsResetCom;
	bool m_bRunning;
};

#endif // !defined(AFX_INITAT_H__7A46CB52_0C05_4BE6_8BA2_1AAFE6827D1A__INCLUDED_)
