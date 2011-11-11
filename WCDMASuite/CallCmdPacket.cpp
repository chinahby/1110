#include "stdafx.h"
#include "CallCmdPacket.h"
#include "CallManager.h"

CCallCmdPacket::CCallCmdPacket(const std::string& number, CCallManager* mgr)
:m_pCallMgr(mgr)
{
	std::string cmd = "ATD";
	cmd += number;
	cmd += ";";
	cmd += "\r";
	SetMessage(cmd.c_str(), cmd.size());
}

void CCallCmdPacket::SetReturnCode(int code)
{
	TRACE(TEXT("CCallCmdPacket::SetReturnCode:> "));
	CMobilePacket::SetReturnCode(code);
	if(m_pCallMgr && code==RETURN_OK)
	{
		m_pCallMgr->OnCallContinue();
	}
	else
	{
		TRACE(TEXT("CCallCmdPacket::SetReturnCode:> OnCallEnd"));
		m_pCallMgr->OnCallEnd(0);
	}
}