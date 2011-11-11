#include "stdafx.h"
#include "InfoConnect.h"

CInfoConnect::CInfoConnect()
{
	m_iConnectTotalSecond = 0;

	m_dwRecvTotal = 0;
	m_dwSendTotal = 0;
	m_dwRecvPS = 0;
	m_dwSendPS = 0;
}