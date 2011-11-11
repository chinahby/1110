#pragma once 

class CInfoConnect
{
public:
	CInfoConnect();

	CTime m_tmConnectBeginTime;
	unsigned int m_iConnectTotalSecond;
	///总接收
	DWORD m_dwRecvTotal;
	///总发送
	DWORD m_dwSendTotal;
	///每秒接收
	DWORD m_dwRecvPS;
	///每秒发送
	DWORD m_dwSendPS;
};