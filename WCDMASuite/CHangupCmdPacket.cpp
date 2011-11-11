#include "stdafx.h"
#include ".\CHangupCmdPacket.h"
#include <string>

CHangupCmdPacket::CHangupCmdPacket()
{
	std::string cmd = "AT+CHUP\r";
	SetMessage(cmd.c_str(), cmd.size());
}