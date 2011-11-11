// SignatureMonitor.h: interface for the CSignatureMonitor class.
//
//////////////////////////////////////////////////////////////////////

#if !defined(AFX_SIGNATUREMONITOR_H__D03BFC0C_12C7_482F_8FB8_D19C3BD72741__INCLUDED_)
#define AFX_SIGNATUREMONITOR_H__D03BFC0C_12C7_482F_8FB8_D19C3BD72741__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

#include "Thread.h"

class CSignatureMonitor : public CThread  
{
public:
	CSignatureMonitor();
	virtual ~CSignatureMonitor();
private:
	virtual void Go();
private:
	static HWND FindSignatureWnd();
};

#endif // !defined(AFX_SIGNATUREMONITOR_H__D03BFC0C_12C7_482F_8FB8_D19C3BD72741__INCLUDED_)
