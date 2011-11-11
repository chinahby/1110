#pragma once

#include "CMobilePacket.h"
#include <string>

class CCallManager;
class CCallCmdPacket: public CMobilePacket
{
	CCallManager* m_pCallMgr;
public:
	CCallCmdPacket(const std::string& number, CCallManager* mgr);

	virtual void SetReturnCode(int code);
};