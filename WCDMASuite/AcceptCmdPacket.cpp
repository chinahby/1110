#include "stdafx.h"
#include "AcceptCmdPacket.h"
#include "CallManager.h"

CAcceptCmdPacket::CAcceptCmdPacket(CCallManager* mgr)
:m_pCallMgr(mgr)
{
	std::string cmd = "ATA\r";
	SetMessage(cmd.c_str(), cmd.size());
}

void CAcceptCmdPacket::SetReturnCode(int code)
{
	TRACE(TEXT("CAcceptCmdPacket::SetReturnCode:>"));
	CMobilePacket::SetReturnCode(code);
	if(m_pCallMgr && code==RETURN_OK)
	{
		m_pCallMgr->OnCallContinue();
	}
	else
	{
		TRACE(TEXT("CAcceptCmdPacket::SetReturnCode:> OnCallEnd"));
		m_pCallMgr->OnCallEnd(0);
	}
}