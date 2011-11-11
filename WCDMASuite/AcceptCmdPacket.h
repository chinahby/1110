#pragma once

#include "CMobilePacket.h"

class CCallManager;
class CAcceptCmdPacket: public CMobilePacket
{
	CCallManager* m_pCallMgr;
public:
	CAcceptCmdPacket(CCallManager* mgr);

	virtual void SetReturnCode(int code);
};