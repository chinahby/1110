// SetManager.h: interface for the CSetManager class.
//
//////////////////////////////////////////////////////////////////////

#if !defined(AFX_SETMANAGER_H__C7E6CD37_36A8_432F_8077_F55BEBE539E9__INCLUDED_)
#define AFX_SETMANAGER_H__C7E6CD37_36A8_432F_8077_F55BEBE539E9__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

class CSetManager  
{
public:
	CString m_strDevName;
	CString m_ComPort;			// modem port  add by lab686 time 2009/10/28 17:28:00
	CString m_PCUIComPort;
	CString m_strIncomeRing;
	CString m_strSMSRing;
	CSetManager();
	virtual ~CSetManager();
	bool GetPCUICom();

};

#endif // !defined(AFX_SETMANAGER_H__C7E6CD37_36A8_432F_8077_F55BEBE539E9__INCLUDED_)
