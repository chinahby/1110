// SetBaudCmdpacket.cpp: implementation of the CSetBaudCmdpacket class.
//
//////////////////////////////////////////////////////////////////////

#include "stdafx.h"
#include "sunnycat.h"
#include "SetBaudCmdpacket.h"
#include "Util.h"

#ifdef _DEBUG
#undef THIS_FILE
static char THIS_FILE[]=__FILE__;
#define new DEBUG_NEW
#endif

//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////

CSetBaudCmdpacket::CSetBaudCmdpacket(int iRate)
{
	CString strCmd;
	strCmd.Format(TEXT("AT*IPR=%d\r"), iRate);
	SetMessage(ToMultiBytes(LPCTSTR(strCmd)).c_str(), ToMultiBytes(LPCTSTR(strCmd)).size());
}

CSetBaudCmdpacket::~CSetBaudCmdpacket()
{

}
