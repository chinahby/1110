// SetManager.cpp: implementation of the CSetManager class.
//
//////////////////////////////////////////////////////////////////////

#include "stdafx.h"
#include "SetManager.h"
#include "devenum.h"

//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////

CSetManager::CSetManager()
{
	m_strIncomeRing = "";
	m_strSMSRing = "";
	if ( !GetPCUICom() )
		m_PCUIComPort = "";
}

CSetManager::~CSetManager()
{

}
//
bool CSetManager::GetPCUICom()
{
	TRACE(TEXT("CSetManager::GetPCUICom :> ENTER"));
	CPortList portList;

	for(size_t i=0; i<portList.Count(); ++i)
	{
		const TPortInfo& info = portList.GetAt(i);
		CString strID = info.strHardwareID;
		strID.MakeLower();

		if(//(info.strFriendlyName.Find(strModem) >=0 ) &&
		       (strID.Find(TEXT("vid_05c6&pid_9003&rev_0000&mi_02")) >=0) )
		{
			TRACE(TEXT("GetPCUICom strCOM = %s\n"), info.strCOM);
			m_PCUIComPort = info.strCOM;
			return true;
		}
	}
	return false;
}
