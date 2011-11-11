// SetBaudCmdpacket.h: interface for the CSetBaudCmdpacket class.
//
//////////////////////////////////////////////////////////////////////

#if !defined(AFX_SETBAUDCMDPACKET_H__70EF4924_FDA8_481A_9E64_E1A580E16F94__INCLUDED_)
#define AFX_SETBAUDCMDPACKET_H__70EF4924_FDA8_481A_9E64_E1A580E16F94__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000
#include "CMobilePacket.h"

class CSetBaudCmdpacket : public CMobilePacket 
{
public:
	CSetBaudCmdpacket(int iRate);
	virtual ~CSetBaudCmdpacket();

};

#endif // !defined(AFX_SETBAUDCMDPACKET_H__70EF4924_FDA8_481A_9E64_E1A580E16F94__INCLUDED_)
