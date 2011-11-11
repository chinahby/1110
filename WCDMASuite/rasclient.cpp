#include "stdafx.h"
#include "RasClient.h"
#include <afxmt.h>
#include <vector>
#include <algorithm>
#include "Util.h"
#include "resource.h"
#include "RasSpdM.h"
#include "Porting.h"
//#include "GlobalDataCenter.h"

#ifdef _DEBUG
#undef THIS_FILE
static char THIS_FILE[]=__FILE__;
#define new DEBUG_NEW
#endif

extern "C"
{

typedef struct _RAS_STATS
{
    DWORD   dwSize;
    DWORD   dwBytesXmited; //发送的字节数
    DWORD   dwBytesRcved;  //接收的字节数
    DWORD   dwFramesXmited;
    DWORD   dwFramesRcved;
    DWORD   dwCrcErr;
    DWORD   dwTimeoutErr;
    DWORD   dwAlignmentErr;
    DWORD   dwHardwareOverrunErr;
    DWORD   dwFramingErr;
    DWORD   dwBufferOverrunErr;
    DWORD   dwCompressionRatioIn;
    DWORD   dwCompressionRatioOut;
    DWORD   dwBps;  //连接速度
    DWORD   dwConnectDuration;//连接时间

} RAS_STATS, *PRAS_STATS;

typedef 
DWORD (APIENTRY *RasGetConnectionStatistics)(HRASCONN hRasConn, RAS_STATS *lpStatistics);

}

class CRasMgr
{
public:
	static void CALLBACK RasDialFunc1(
			  HRASCONN hrasconn,
			  UINT unMsg,
			  RASCONNSTATE rascs,
			  DWORD dwError,
			  DWORD dwExtendedError
			);

	static CRasMgr& Instance();

	void RegisterRas(CRasClient& ras);
	void UnregisterRas(CRasClient& ras);
	~CRasMgr();

	RasGetConnectionStatistics GetStatisticProc();
private:
	CRasMgr();
private:
	std::vector<CRasClient*> m_Connections;
	CCriticalSection m_CS;

	HINSTANCE m_hLib;
	RasGetConnectionStatistics m_Func;
};

CRasMgr& CRasMgr::Instance()
{
	static CRasMgr s_Inst;

	return s_Inst;
}

CRasMgr::CRasMgr()
{
	m_hLib = LoadLibrary(TEXT("RasAPI32.dll"));
	m_Func = NULL;
	if(m_hLib)
	{
		m_Func = (RasGetConnectionStatistics)GetProcAddress(m_hLib, "RasGetConnectionStatistics");
	}
	if(!m_Func)
	{
		CRasSpdM::InitSpeedMonitor();
	}
}

CRasMgr::~CRasMgr()
{
	if(m_hLib)
	{
		FreeLibrary(m_hLib);
		m_hLib = NULL;
	}
	if(!m_Func)
	{
		CRasSpdM::ClearSpeedMonitor();
	}
	m_Func = NULL;
}

RasGetConnectionStatistics CRasMgr::GetStatisticProc()
{
	return m_Func;
}

void CALLBACK CRasMgr::RasDialFunc1(
						HRASCONN hrasconn,
						UINT unMsg,
						RASCONNSTATE rascs,
						DWORD dwError,
						DWORD dwExtendedError)
{
	CRasMgr& mgr = CRasMgr::Instance();
	mgr.m_CS.Lock();
	CRasClient* pConnect = NULL;
	for(size_t i=0; i<mgr.m_Connections.size(); ++i)
	{
		if(mgr.m_Connections[i]->m_hConnection == hrasconn)
		{
			pConnect = mgr.m_Connections[i];
		}
	}
	mgr.m_CS.Unlock();

	if(pConnect)
	{
		pConnect->FireEvent(unMsg, rascs, dwError, dwExtendedError);
	}
}

void CRasMgr::RegisterRas(CRasClient& ras)
{
	m_CS.Lock();
	m_Connections.push_back(&ras);
	m_CS.Unlock();
}
void CRasMgr::UnregisterRas(CRasClient& ras)
{
	m_CS.Lock();
	
	std::vector<CRasClient*>::iterator iFind = 
		std::find(m_Connections.begin(), m_Connections.end(), &ras);
	if(iFind != m_Connections.end())
	{
		m_Connections.erase(iFind);
	}
	
	m_CS.Unlock();
}

//********************************************************

CRasClient::CRasClient()
:m_hConnection(NULL),
 m_pListener(NULL)
{
}

CRasClient::~CRasClient()
{

}

bool CRasClient::Dial(const CString& strConnection,
					  const CString& strPhoneNum,
					  const CString& strUser,
					  const CString& strPasswrod)
{
	if(m_hConnection)return false;

	RASDIALPARAMS dialParam;
	memset(&dialParam, 0, sizeof(dialParam));
	dialParam.dwSize = sizeof(dialParam);
	_tcscpy(dialParam.szEntryName, strConnection);
	_tcscpy(dialParam.szPhoneNumber, strPhoneNum);
	_tcscpy(dialParam.szUserName, strUser);
	_tcscpy(dialParam.szPassword, strPasswrod);

	DWORD dwSuccess = RasDial(NULL,
						NULL,
						&dialParam,
						1,
						(LPVOID)CRasMgr::RasDialFunc1,
						&m_hConnection);
	if(0 == dwSuccess)
	{
		CRasMgr::Instance().RegisterRas(*this);
	}

	return 0 == dwSuccess;
}

void CRasClient::Hangup()
{
	if(m_hConnection)
	{
		RasHangUp(m_hConnection);

		RASCONNSTATUS status;
		status.dwSize = sizeof(status);

		while(true)
		{
			if(RasGetConnectStatus(m_hConnection, &status) == ERROR_INVALID_HANDLE)
			{
				TRACE(TEXT("DISCONNECTED NOW!\n"));
				break;
			}
			Sleep(10);
		}
		m_hConnection = NULL;
		CRasMgr::Instance().UnregisterRas(*this);
	}
}

RASCONNSTATE CRasClient::GetStatus(DWORD* dwError)const
{
	ASSERT(m_hConnection);
	RASCONNSTATUS status;
	status.dwSize = sizeof(status);

	DWORD dwRet = RasGetConnectStatus(m_hConnection, &status);

	if(dwRet == ERROR_INVALID_HANDLE)
	{
		return RASCS_Disconnected;
	}

	if(dwError)*dwError=status.dwError;

	return status.rasconnstate;
}

void CRasClient::SetListener(IRasListener* pListener)
{
	ASSERT(!m_pListener || m_pListener==pListener);

	m_pListener = pListener;
}


void CRasClient::FireEvent( UINT unMsg,
					RASCONNSTATE rascs,
					DWORD dwError,
					DWORD dwExtendedError)
{
	if(m_pListener)
		m_pListener->OnRasMsg(unMsg, rascs, dwError, dwExtendedError);
}


CString CRasClient::GetStatusString(RASCONNSTATE status)
{
	switch(status)
	{
	case RASCS_Connected:
		return LoadStringEx(IDS_CONNECT_LINKED);
	case RASCS_Disconnected:
		return LoadStringEx(IDS_CONNECT_OFFLINE);
	case RASCS_OpenPort:
		return LoadStringEx(IDS_CONNECT_OPEN_PORT);
	case RASCS_PortOpened:
		return LoadStringEx(IDS_CONNECT_PORT_OPENED);
	case RASCS_ConnectDevice:
		return LoadStringEx(IDS_CONNECT_OPEN_DEVICE);
	case RASCS_DeviceConnected:
	case RASCS_AllDevicesConnected:
		return LoadStringEx(IDS_CONNECT_DEVICE_OPENED);
	case RASCS_Authenticate:
		return LoadStringEx(IDS_CONNECT_AUTHEN);  
	case RASCS_AuthNotify:
		return LoadStringEx(IDS_AUTH_NOTIFY);
	case RASCS_AuthRetry:
	case RASCS_AuthCallback:
		return "";
	case RASCS_AuthChangePassword:
		return "";
	case RASCS_AuthProject:
		return LoadStringEx(IDS_REGISTERING_NETWORK);
	case RASCS_AuthLinkSpeed:
		return "";
	case RASCS_AuthAck:
		return LoadStringEx(IDS_AUTH_ACK); 
	case RASCS_ReAuthenticate:
		return "";
	case RASCS_Authenticated:
		return "";
	case RASCS_PrepareForCallback:
		return "";
	case RASCS_WaitForModemReset:
		return "";
	case RASCS_WaitForCallback:
		return "";
	case RASCS_Projected:
		return "";
	case RASCS_StartAuthentication:
		return "";  
	case RASCS_CallbackComplete:
		return "";
	case RASCS_LogonNetwork:
		return LoadStringEx(IDS_LOGON_NETWORK);
	case RASCS_SubEntryConnected:
		return "";
	case RASCS_SubEntryDisconnected:
		return "";
	case RASCS_Interactive:
		return "";
	case RASCS_RetryAuthentication:
		return "";
	case RASCS_CallbackSetByCaller:
		return "";
	case RASCS_PasswordExpired:
		return "";
	}
	return "";
}

CString CRasClient::GetErrorString(DWORD dwError)
{
	TCHAR buf[1024] = {0};
	RasGetErrorString(dwError, buf, sizeof(buf));
	return buf;
}

bool CRasClient::GetStatics(TConnectionStastics& info)const
{
	RasGetConnectionStatistics statisticAPI = CRasMgr::Instance().GetStatisticProc();
	if(statisticAPI)
	{
		RAS_STATS statistic = 
		{
			sizeof(statistic),
			0
		};

		bool bOK = ERROR_SUCCESS == statisticAPI(m_hConnection, &statistic);
		if(bOK)
		{
			info.dwBytesRcved	= statistic.dwBytesRcved;
			info.dwBytesXmited	= statistic.dwBytesXmited;
		}
		
		return bOK;
	}
	else
	{
		DWORD TPS = 0, RPS = 0, TT = 0, RT = 0;
		CRasSpdM::GetRASSpeed(&TPS, &RPS, &TT, &RT); 

		info.dwBytesRcved = RT;
		info.dwBytesXmited= TT;
		return true;
	}
}

void CRasClient::BeginFluxCount(const CTime& tmBegin)
{
	m_ConnectInfo.m_dwRecvTotal = 0;
	m_ConnectInfo.m_dwSendTotal = 0;
	m_ConnectInfo.m_dwRecvPS = 0;
	m_ConnectInfo.m_dwSendPS = 0;

	m_ConnectInfo.m_tmConnectBeginTime = tmBegin;
	m_ConnectInfo.m_iConnectTotalSecond = 0;

	m_dwBeginTick = GetTickCount();
	if(!CRasMgr::Instance().GetStatisticProc())
	{
		CRasSpdM::ResetStatistic();
	}
}

const CInfoConnect& CRasClient::RefreshFluxCount()
{
	TConnectionStastics info = {0};
	if(GetStatics(info))
	{

		DWORD dwTimeCount = (GetTickCount()-m_dwBeginTick)/1000;
		if(	dwTimeCount>m_ConnectInfo.m_iConnectTotalSecond)
		{
			DWORD interval = dwTimeCount-m_ConnectInfo.m_iConnectTotalSecond;
			m_ConnectInfo.m_dwRecvPS = (info.dwBytesRcved-m_ConnectInfo.m_dwRecvTotal)/interval;
			m_ConnectInfo.m_dwSendPS = (info.dwBytesXmited-m_ConnectInfo.m_dwSendTotal)/interval;

			if(info.dwBytesRcved>m_ConnectInfo.m_dwRecvTotal)
			{
				m_ConnectInfo.m_dwRecvTotal = info.dwBytesRcved;
			}
			
			if(info.dwBytesXmited>m_ConnectInfo.m_dwSendTotal)
			{
				m_ConnectInfo.m_dwSendTotal = info.dwBytesXmited;
			}
		}
		m_ConnectInfo.m_iConnectTotalSecond = dwTimeCount;
	}

	return m_ConnectInfo;
}

void CRasClient::EndFluxCount()
{
	RefreshFluxCount();
}

const CInfoConnect& CRasClient::GetFluxCount()
{
	return m_ConnectInfo;
}


//********************************************************

CRasEntry::CRasEntry(const CString& modemName, const CString& connectionName)
:m_DeveiceName(modemName),
 m_ConnectionName(connectionName)
{
}

bool CRasEntry::IsExisting()const
{
	return ::RasValidateEntryName(TEXT(""), m_ConnectionName) == ERROR_ALREADY_EXISTS;
}

static bool FromStringToIpAddr(CString str, RASIPADDR &rasIP)
{
	CString		s;
	int			nPos;
	int			n;
	int			nWhich = 1;

	while (1)
	{
		nPos = str.Find(TEXT("."));

		if (nPos==-1)
			s = str;
		else
			s = str.Left(nPos);

		n = _ttoi(s);
		str = str.Right(str.GetLength()-(nPos+1));

		switch (nWhich)
		{
		case 1:
			rasIP.a = (BYTE)n;
			break;
		case 2:
			rasIP.b = (BYTE)n;
			break;
		case 3:
			rasIP.c = (BYTE)n;
			break;
		case 4:
			rasIP.d = (BYTE)n;
			break;
		default:
			ASSERT (FALSE);
			break;
		}

		nWhich++;

		if (nPos==-1)
			break;
	} 

	return FALSE;
}

inline void InitRASIP(RASIPADDR &rasIP)
{
	rasIP.a	=0;
	rasIP.b =0;
	rasIP.c =0;
	rasIP.d =0;
}

bool CRasEntry::Create(const CString& strPhoneNum,
					   const CString& ipaddr, 
					   const CString& dns1, 
					   const CString& dns2,
					   const CString& strUserName, 
					   const CString& strPassword)
{
	RASENTRY rasEntry;
	::ZeroMemory(&rasEntry, sizeof(rasEntry));
	rasEntry.dwSize = sizeof(rasEntry);
	_tcscpy(rasEntry.szLocalPhoneNumber, strPhoneNum);
	rasEntry.dwfNetProtocols = RASNP_Ip;
	rasEntry.dwFramingProtocol = RASFP_Ppp; 
	// DON'T USE "RASDT_Modem", use "modem", otherwise dwRV = 87
	
	_tcscpy(rasEntry.szDeviceType, TEXT("modem")); 
	_tcscpy(rasEntry.szDeviceName, m_DeveiceName);

	if(!dns1.IsEmpty() || !dns2.IsEmpty())
		rasEntry.dwfOptions = RASEO_ModemLights | RASEO_RemoteDefaultGateway | RASEO_SpecificNameServers;
	else
		rasEntry.dwfOptions = RASEO_ModemLights | RASEO_RemoteDefaultGateway;

	if(!ipaddr.IsEmpty())
		rasEntry.dwfOptions = rasEntry.dwfOptions | RASEO_SpecificIpAddr;
	
	// IP addresses
	FromStringToIpAddr(ipaddr, rasEntry.ipaddr);
	FromStringToIpAddr(dns1, rasEntry.ipaddrDns);
	FromStringToIpAddr(dns2, rasEntry.ipaddrDnsAlt);

	//Delete old Connection
	RasDeleteEntry(NULL, m_ConnectionName);

	DWORD dwRV = RasValidateEntryName(NULL, m_ConnectionName);

	//Create a new connection
	dwRV = RasSetEntryProperties(
					NULL, 
					m_ConnectionName, 
					&rasEntry,
					sizeof(rasEntry), 
					NULL, 
					0);

	if(dwRV == 0)
	{
		RASDIALPARAMS dailParam;
		memset(&dailParam, 0, sizeof(dailParam));
		dailParam.dwSize = sizeof(dailParam);
		_tcsncpy(dailParam.szEntryName, m_ConnectionName, sizeof(dailParam.szEntryName));
		
		_tcsncpy(dailParam.szPhoneNumber, strPhoneNum, sizeof(dailParam.szPhoneNumber));
		
		_tcsncpy(dailParam.szCallbackNumber, TEXT(""), sizeof(dailParam.szCallbackNumber));
		
		_tcsncpy(dailParam.szUserName, strUserName, sizeof(dailParam.szUserName));
		
		//_tcsncpy(dailParam.szPassword, strPassword, sizeof(dailParam.szPassword));
		
		dwRV = RasSetEntryDialParams(NULL, &dailParam, false);
		
	}


	return dwRV == 0;
}

bool CRasClient::GetConnectState() 
{ 

	DWORD cbBuf = 0; 
	DWORD cConn = 0; 
	DWORD dwRet = 0; 
	HRASCONN hrasconn; 
	RASCONNSTATUS rasStatus; 
	UINT ndx; 
	UINT nhangup;
	bool b_hasHangup = false;
	RASCONN lpRasConn[100] ;

	cbBuf = sizeof(RASCONN)* 100 ; 
				
	lpRasConn[0].dwSize = sizeof( RASCONN ); 
	dwRet = RasEnumConnections( lpRasConn,&cbBuf,&cConn ); 
	bool b_hasCanceled = false;
	CString strModemName;
	strModemName = GetModemName();

	for (ndx = 0; ndx < cConn; ndx++) 
	{ 
		if((CString)lpRasConn[ndx].szDeviceName == strModemName )//"WeWins Modem"
		{
			hrasconn = lpRasConn[ndx].hrasconn; 
			rasStatus.dwSize = sizeof(RASCONNSTATUS); 
			dwRet = RasGetConnectStatus( hrasconn,&rasStatus ); 
			if(RASCS_Connected == rasStatus.rasconnstate)
			{
				if(IDCANCEL == GMessageBox(LoadStringEx(IDS_RASCON_PROMPT), NULL,MB_OKCANCEL|MB_ICONASTERISK))
				{
					b_hasCanceled = true;
					break;
				}
				else
				{
					for(nhangup  = 0; nhangup < 3; nhangup++)
					{
						DWORD hangupOK = RasHangUp(hrasconn);
						if(0 == hangupOK)
						{
							b_hasHangup = true;
							break;
						}
					}
					if(!b_hasHangup)
					{
						return false;
					}
					else
					{
						return true;
					}
					
				}				
			}
		}
	}

	if(b_hasCanceled)
	{
		return false;
	}	

	return true;
}
