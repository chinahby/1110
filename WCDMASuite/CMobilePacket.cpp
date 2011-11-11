#include "stdafx.h"
#include "CMobilePacket.h"

CMobilePacket::CMobilePacket()
{
	m_nReturn = 0;
	m_nLen = 0;
	memset(m_szMessage, 0, 1024);
}

void CMobilePacket::SetMessage(const char* strCmd, size_t strSize)
{
	m_nLen = min(sizeof(m_szMessage), strSize);
#ifdef _DEBUG
	ZeroMemory(m_szMessage, sizeof(m_szMessage));
#endif
	memcpy(m_szMessage, strCmd, m_nLen);
}

CMobilePacket::CMobilePacket(const char* strCmd, size_t strSize)
{
	m_nReturn = 0;
	SetMessage(strCmd, strSize);
}

CMobilePacket::~CMobilePacket()
{
}

void CMobilePacket::SetReturnCode(int code)
{
	m_nReturn = code;
}
