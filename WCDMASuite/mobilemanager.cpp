// MobileManager.cpp: implementation of the CMobileManager class.
//
//////////////////////////////////////////////////////////////////////

#include "stdafx.h"
#include "SunnyCat.h"
#include "SunnyCatDlg.h"
#include "MobileManager.h"
#include "GlobalDataCenter.h"
#include <stdlib.h>
#include <fstream>
#include "DevEnum.h"
#include "Util.h"
#include "Porting.h"
#include "ATResponse.h"
#include "CallManager.h"
#include "DlgProgressEx.h"
#include "CDMATime.h"

#include <atlbase.h>
#include "Resource.h"

//#include "IRASClient.h"


#ifdef _DEBUG
#undef THIS_FILE
static char THIS_FILE[]=__FILE__;
#define new DEBUG_NEW
#endif


//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////

CMobileManager::CMobileManager(CSunnyCatDlg *pOwner)
:m_EventDisconnect(false, true),
/*m_EventStatusReport(false, true),*/
 m_SerialPort(this)
{
	TRACE(TEXT("CMobileManager::CMobileManager :> CMobileManager INIT\n"));
	InitATResponse();

	///线程
	m_hThreadConnect = NULL;
	m_pOwner = pOwner;
	m_pCallMgr = new CCallManager();
	m_pCallMgr->SetMobileManager(this);

	m_ConnStat = CONNECT_CONTROL_NOT_CONNECT;
	m_iSMSMaxNum = -1;

	m_bRUIMSupported = true;
	m_bConnectSuccess = false;
	m_registerStatus = 0;
	//noSimHasReport = false;
	m_netGprs = 0;
	m_netEdge = 0;
	
	//m_pbNameMaxlen = 0;//sim卡电话本姓名最大长度
	//m_pbNumMaxlen = 0;//sim卡电话本号码最大长度
	m_nCurBaud = 0;
	m_bSMSLoaded = false;
	m_RasClient.SetListener(this);//注册Ras事件通知
	isAbleCom = false;
	hasInserted = false;
	bRemoved = true;
	m_simLocked = false;
	m_ComName = "";
	m_iNetMode = 8;
	m_bSetNetModeOK = false;

	m_strProduct = GetProduct();
	m_bIsAboveSendSMS = false;	// add by lab686 200912101002
	m_bIsSendSMS = false;			//add by lab 200912081655
	memset( m_iRecNewSMSLocal, -1, sizeof(m_iRecNewSMSLocal));

	m_nSignal = 0;
	m_nHDRSignal = 0;
	m_bSimExist = true;
	m_bpinpuk = true;
	m_uremainingtime = 3;
	g_pfnDwonLoadVerCallBack = NULL;
	m_CurACT = 0;
	m_CurFormat = 0;
	m_IsRegister = false;
}

CMobileManager::~CMobileManager()
{

	delete m_pCallMgr; 
	m_pCallMgr = NULL;
	CleanATResponse();
}

void LogString(const char* str, size_t size);
bool CMobileManager::Init()
{	
	TRACE(TEXT("CMobileManager::Init START"));
	LogString("Search Modem", 0);
	if(!DetectModem())
	{
		return false;
	}

	LogString("Search Port ", 0);
	//检查端口设备是否存在
	if(!IsDeviceExisting())
	{
		return false;
	}

	TRACE(TEXT("CMobileManager::Init ONE"));
	DWORD dwCurrentBaud = CheckModule();
	
	//设置工作波特率为115200
//	if(m_strProduct == "-c")
//	{
//		if((115200 != dwCurrentBaud)&&(460800 != dwCurrentBaud)&&(9600 != dwCurrentBaud))
//		{
//			TRACE(TEXT("设备波特率无效!\n"));
//			return false;			
//		}
//	}
//	else
	{
		if( 115200 != dwCurrentBaud )
		{
			TRACE(TEXT("设备波特率无效!\n"));
//			return false;
		}
		if ( !m_bSimExist )
		{
//			return false;
		}
		
	}
	return true;
}



void CMobileManager::InitATResponse()
{
	m_pNormalResponse  = new CNormalResponse();
	m_pActiveReponse = m_pNormalResponse;

	m_ATResponses.push_back(new CResponseHDRCSQ(this));			//  add by lab686 20091029
	m_ATResponses.push_back(new CResponseCSQ(this));			// OK
	m_ATResponses.push_back(new CResponseCREG(this));
//	m_ATResponses.push_back(new CResponseCGATT(this));
//	m_ATResponses.push_back(new CResponseEDGE(this));
//	m_ATResponses.push_back(new CResponseIMEI(this));
	m_ATResponses.push_back(new CResponseMODE(this));
	m_ATResponses.push_back(new CResponsePREFMODE(this));
	m_ATResponses.push_back(new CResponseSimLock(this));
	m_ATResponses.push_back(new CResponseCONN(this));				// ok
	m_ATResponses.push_back(new CResponseRING(this));				// ok
	m_ATResponses.push_back(new CResponseCLIP(this));				// ok
	m_ATResponses.push_back(new CResponseCEND(this));				// ok
	m_ATResponses.push_back(new CResponseCIND(this));				//电话结束NO USE
	m_ATResponses.push_back(new CResponseCLVL(this));				// OK
	m_ATResponses.push_back(new CResponseORIG(this));				// OK
	m_ATResponses.push_back(new CResponseSMSCMGS(this));
	m_ATResponses.push_back(new CResponseSMSERROR(this));
	//m_ATResponses.push_back(new CResponseZMGF(this));
	m_ATResponses.push_back(new CResponseCMTI(this));
	m_ATResponses.push_back(new CResponseCMGR(this));
	m_ATResponses.push_back(new CResponseCPMS(this));
	m_ATResponses.push_back(new CResponseCPBW(this));
	m_ATResponses.push_back(new CResponseCPBR(this));
//	m_ATResponses.push_back(new CResponseCNUM(this));
//	m_ATResponses.push_back(new CResponseATI3(this));
//	m_ATResponses.push_back(new CResponseZDPB(this));
//	m_ATResponses.push_back(new CResponseZFNM(this));
//	m_ATResponses.push_back(new CResponseZVMI(this));
//	m_ATResponses.push_back(new CResponseCLCK(this));
	m_ATResponses.push_back(new CResponseCMEError(this));
	m_ATResponses.push_back(new CResponseCMSError(this));
	m_ATResponses.push_back(new CResponseError(this));
	m_ATResponses.push_back(new CResponseOK(this));
	m_ATResponses.push_back(new CResponseCMT(this));
	m_ATResponses.push_back(new CResponseCPIN(this));
//	m_ATResponses.push_back(new CResponseIPR(this));
//	m_ATResponses.push_back(new CResponseZCRDN(this));
//	m_ATResponses.push_back(new CResponseTINFO(this));
//	m_ATResponses.push_back(new CResponseCCWA(this));
	m_ATResponses.push_back(new CResponseSMSFULL(this));
	m_ATResponses.push_back(new CResponseSIMER(this));
//	m_ATResponses.push_back(new CResponseBusyNow(this));//jiangguochao added 20070115
	m_ATResponses.push_back(new CResponseNoCarrier(this));//jiangguochao added 20070115
	m_ATResponses.push_back(new CResponseConnected(this));//jiangguochao added 20070115
	m_ATResponses.push_back(new CResponseNoAnswer(this));//jiangguochao added 20070115
//	m_ATResponses.push_back(new CResponseCSCA(this));//jiangguochao added 20070115
	m_ATResponses.push_back(new CResponseCOPS(this));
//	m_ATResponses.push_back(new CResponseCPOL(this));
//	m_ATResponses.push_back(new CResponseSWRV(this));
	m_ATResponses.push_back(new CResponseATOK(this));	
//	m_ATResponses.push_back(new CResponsePPP(this));
}

CATResponse* CMobileManager::FindParser(const CString& parserName)const
{
	CATResponse* parser = NULL;
	for(size_t i=0; i<m_ATResponses.size(); ++i)
	{
		if(m_ATResponses[i]->MatchByName(parserName))
		{
			parser = m_ATResponses[i];
			break;
		}
	}
	return parser;
}


void CMobileManager::CleanATResponse()
{
	for(size_t i=0;i<m_ATResponses.size(); ++i)
	{
		delete m_ATResponses[i];
	}
	m_ATResponses.clear();
	delete m_pNormalResponse;
	m_pActiveReponse = NULL;
}

CCallManager& CMobileManager::GetCallManager()
{
	return *m_pCallMgr;
}

const CCallManager& CMobileManager::GetCallManager()const
{
	return *m_pCallMgr;
}



const CString& CMobileManager::GetPortDevName()const
{
	return m_strPortDevName;
}


const CString& 	CMobileManager::GetPinResponse()const
{
	return m_strPINResponse;
}

//检测modem
bool CMobileManager::DetectModem()
{
	CString strModemName;

	strModemName.Format(TEXT(""));
//	strModemName = GetModemName();
	if(IsModemExisting(strModemName))return true;
	/*****************************************************************
	if(m_strProduct == "-p")
	{
		if(!InstallModem())
		{
			m_strErrorReturn = LoadStringEx(IDS_ERROR_MODEM_NOT_FOUND);
			return false;
		}
		return IsModemExisting(strModemName);
	}
	/*****************************************************************/
	return false;

	
}

bool CMobileManager::IsDeviceExisting()
{
	TRACE(TEXT("CMobileManager::IsDeviceExisting :> ENTER\n"));
	CPortList portList;
	for(size_t i=0; i<portList.Count(); ++i)
	{
		const TPortInfo& info = portList.GetAt(i);
		
		const CString& strHardwareID = "vid_05c6&pid_9003&rev_0000&mi_02";//GetHardwareID(nID);
		CString strID = info.strHardwareID;
		TRACE(TEXT("IsDeviceExisting strHardwareID = %s\n"), info.strHardwareID);
		strID.MakeLower();
		if(strHardwareID == "")break;
		if(strID.Find(strHardwareID) >= 0)
		{
			TRACE(TEXT("IsDeviceExisting strCOM = %s\n"), info.strCOM);
			m_Options.m_PCUIComPort = info.strCOM;			// change by lab686 2009/10/28 17:30:00
			m_ComName = info.strCOM;
			m_strPortDevName = info.strFriendlyName;
			m_bDeviceIsForPDA = info.strHardwareID.Find(TEXT("CDMA1XXFFR-PC_PDA-C5BD"))!=-1;
			return true;
		}

	}
	
	m_strErrorReturn = LoadStringEx(IDS_ERROR_HARDWARE_NOT_FOUNT);
	return false;
}

//运行一个程序
HANDLE RunProgram(const CString& strCmdLine, bool bShowWindow);


DWORD OptionsShellType();

bool CMobileManager::InstallModem()
{
	/*********************************************************
	int ShellType = OptionsShellType();

	CString strCmdLine;
	if(ShellType >= Win_2000)
	{
		//strCmdLine = GetAppPath() + "driver\\MdmInst.exe";
		strCmdLine = GetAppPath() + "SINCOMInstall.exe";
	}
	else if(ShellType == Win_98)
	{
		strCmdLine = GetAppPath() + "driver\\ModemInst98.exe";
	}
	else if(ShellType == Win_ME)
	{
		strCmdLine = GetAppPath() + "driver\\ModemInstME.exe";
	}
	else
	{
		return false;
	}
	HANDLE hProcess = RunProgram(strCmdLine, false);
	if(!hProcess)return false;

	WaitForSingleObject(hProcess, INFINITE);
	CloseHandle(hProcess);
	/**********************************************************/

	return true;
}

//检查有卡无卡版本
bool CMobileManager::CheckRUIMSupporting()
{
	CATCmd cmd = SendATPacket(CMobileATProto::CheckRUIMSurport());

	int nReturnCode = WaitForATCompleted(cmd,4000);
	if(nReturnCode == RETURN_TIMEOUT)//超时作为错误
		return false;
	if(nReturnCode == RETURN_ERROR)
	{
		m_bRUIMSupported = true;
		return true;
	}
	else
		return true;
}

void CMobileManager::installModemInitialAT(CString strText)
{
	
	CRegKey reg;
	
    HKEY hKey ;
	CString strSubKey = "SYSTEM\\CurrentControlSet\\Control\\Class\\{4D36E96D-E325-11CE-BFC1-08002BE10318}";
	long lResult = RegOpenKey(HKEY_LOCAL_MACHINE,strSubKey,&hKey);
	CString strKey = GetModemConfigString() + "\\";

	//GetModemConfigString() + "\\0000"; 
	DWORD dwIndex = 0;
	TCHAR strName[50] = {0};
	DWORD dwNameLen = 50;
	FILETIME ft;
	BOOL bLoop = TRUE;
	CString strModemName;
	strModemName = m_Options.m_strDevName;

    while(bLoop)
	{
		dwNameLen = 50;
		ZeroMemory(strName,sizeof(strName));
		lResult = RegEnumKeyEx(hKey,dwIndex++,strName,&dwNameLen,NULL,NULL,NULL,&ft);
		if(lResult == ERROR_NO_MORE_ITEMS)
			break;
		CString strMKey = "";
		strMKey = strKey;
		strMKey += strName;

		if(reg.Open(HKEY_LOCAL_MACHINE,strMKey) != ERROR_SUCCESS)	
		{
			
		}  
		
		TCHAR strKeyValue[50] = {0}; 
		DWORD dwCount = 50;
		long lRet = reg.QueryValue(strKeyValue,TEXT("Model"),&dwCount);

		if(strKeyValue == strModemName )//(CString)"WeWins Modem"
		{
			CString str;
			str.Empty();
			str.Format(TEXT("AT+CGDCONT=1,\"IP\",\"%s\""),strText);
			reg.SetValue(str, TEXT("UserInit"));

			bLoop = FALSE;
		}

		reg.Close();

	}

	RegCloseKey(hKey);
	
}


/*
//检查有卡无卡版本
bool CMobileManager::CheckSimExist()
{
	CATCmd cmd = SendATPacket(CMobileATProto::CheckSimExist());

	int nReturnCode = WaitForATCompleted(cmd,4000);
	if(nReturnCode == RETURN_TIMEOUT)//超时作为错误
		return false;
	if(nReturnCode == RETURN_ERROR)
	{
		m_bRUIMSupported = true;
		return true;
	}
	else
		return true;
}
*/
bool CMobileManager::ProcessNewSMSPostMsg(int local)	//add by lab 200912081655
{
	if ( -1 == local )
	{
		for ( unsigned char i = 0; i < 20; ++i )
		{
			if ( -1 != m_iRecNewSMSLocal[i])
			{
				PostMessageToClient(WM_SMS_NOTIFY, m_iRecNewSMSLocal[i], 0);
				m_iRecNewSMSLocal[i]= -1;
			}
		}
	}
	else
	{
		for ( unsigned char i = 0; i < 20; ++i )
		{
			if ( -1 == m_iRecNewSMSLocal[i] )
			{
				m_iRecNewSMSLocal[i] = local;
				break;
			}
		}
	}
	return true;
}

void CMobileManager::OnRasMsg(UINT unMsg,
					RASCONNSTATE rascs,
					DWORD dwError,
					DWORD dwExtendedError)
{
	m_pOwner->PostMessage(WM_NET_CONNECTING, NULL, rascs);
	if(rascs == RASCS_Connected)
	{
		m_bConnectSuccess = true;
		//开始流量统计(初始化参数)、按钮设置
		BeginFluxCount();
		SetConnectionStat(CONNECT_CONTROL_CONNECT);
	}
	m_dwRASError = dwError;
}

void CMobileManager::GetIMEI()
{
	CATCmd cmd = SendATPacket(CMobileATProto::QueryIMEI());

	CString imei_y= m_pNormalResponse->GetResponseString();
	TRACE(TEXT("CString imei = %s\n"),imei_y);
	
	if( RETURN_OK == WaitForATCompleted(cmd,1000) )
	{
		/*****************************************************
		std::string str = parser.GetParsedParam(1);
		m_pMgr->m_imei = ToUnicode(str).c_str();
		/*****************************************************/
	 	CString imei= m_pNormalResponse->GetResponseString();
		TRACE(TEXT("CString imei = %s\n"),imei);
		this->m_imei = imei;
		const char* tmp = ToMultiBytes(LPCTSTR(imei)).c_str();
		int MCC = (tmp[0] - '0') * 100 + (tmp[1] - '0') * 10 + (tmp[2] - '0');
		int MNC = (tmp[3] - '0') * 10 + (tmp[4] - '0');
		TRACE(TEXT("MCC = %d,MNC = %d\n"),MCC,MNC);
	}
}

/*
选择信箱
*/
bool CMobileManager::SMSBoxParam(const CString& strType)
{
	///查询短信息信箱的参数
	CATCmd cmd = SendATPacket(CMobileATProto::SMSBoxSelect(strType));

	return RETURN_OK == WaitForATCompleted(cmd,1000);
}

/*
发送多字节编码短信
*/
bool CMobileManager::SendSMS_MultiByte(const CString& strPhoneNum, 
								int iPriority, 
								const CString& strText)
{
	return SendSMS_ASCII(strPhoneNum, iPriority, strText);	
}

/*
发送单字节编码(7bits)短信
*/
bool CMobileManager::SendSMS_ASCII(const CString& strPhoneNum, 
								int iPriority, 
								const CString& strText)
{
	

	CMobilePacket* cmd1, *cmd2;
	cmd1 = CMobileATProto::SendPDUSMS(m_strCscaNum, strPhoneNum, strText, cmd2);

	CATCmd ATCmd1 = SendATPacket(cmd1);
	WaitForATCompleted(ATCmd1, 2000);	

	CATCmd ATCmd2 = SendATPacket(cmd2);

	return RETURN_OK == WaitForATCompleted(ATCmd2,10000);
}

/*
发送单字节编码(8bits)短信
*/
bool CMobileManager::SendSMS_SingleByte(const CString& strPhoneNum, 
				int iPriority, 
				const CString& strText)
{
	return SendSMS_ASCII(strPhoneNum, iPriority, strText);
}


//检测信号
void CMobileManager::CheckSignal()
{
	m_SerialPort.QuerySignal();
}

CATCmd CMobileManager::SendATPacket(CMobilePacket* Packet)
{
	if(!m_SerialPort.IsThreadActive())
	{
		CATCmd cmd(Packet);
		cmd.SetErrorCode(RETURN_TIMEOUT);
		cmd.SetEvent();
		return cmd;
	}

	CATCmd cmd(Packet);
	m_SerialPort.PushCmd( cmd );

	return cmd;
}

void CMobileManager::ExitConnectThread()
{
	if(WaitForSingleObject(m_hThreadConnect,0)==WAIT_TIMEOUT)
	{
		TRACE(TEXT("Exit connection thread ... \n"));

		m_bExit = true;
		
		m_EventDisconnect.SetEvent();
		WaitForSingleObject(m_hThreadConnect, INFINITE);
		if(m_hThreadConnect)
		{
			BOOL bok = CloseHandle(m_hThreadConnect);
			if(bok)
			{
				m_hThreadConnect = NULL;		
			}
			else
			{
				bok = CloseHandle(m_hThreadConnect);
				if(bok)
				{
					m_hThreadConnect = NULL;
				}
			}
		}
		
		TRACE(TEXT("ExitConnectThread OK\n"));
		//设置当前状态为无连接
		SetConnectionStat(CONNECT_CONTROL_NOT_CONNECT);
	}	
}

bool CMobileManager::ShutDown()
{
	TRACE(TEXT("shutdown\n"));
	ExitConnectThread();
	m_SerialPort.StopMonitoring();//20070517
	m_pOwner->StopPCM();

	return true;
}


void CMobileManager::SendKey(const CString& strKey)
{

/*jiangguochao deleted because below AT command is private 20070115
	for(int i=0;i<strKey.GetLength();++i)
		SendATPacket(CMobileATProto::KeyInput(strKey[i]));
*/

}

/*
设置音量
*/
bool CMobileManager::SetVolume(int iVolumn)
{
///	if(iVolumn > 0)
///	{
		///设置音量
		SendATPacket( CMobileATProto::SetVolumn(iVolumn));
///	}
///	else
///	{
		//关闭音量
///		SendATPacket(CMobileATProto::SetMICMute(true));
///	}

	return true;
}

bool CMobileManager::GetVolume(int& iVolumn)
{
	CATCmd cmd = SendATPacket(CMobileATProto::GetVolumn());
	if(RETURN_OK == WaitForATCompleted(cmd,2000))
	{
		if(m_nVolumn<3)
		{
			/*jiangguochao 20070115 changed 1->96*/
			m_nVolumn = 5;//音量过小时自动调节
			WaitForATCompleted(SendATPacket( CMobileATProto::SetVolumn(m_nVolumn)));
		}
		iVolumn = m_nVolumn;
		return true;
	}
	else
		return false;
}



void CMobileManager::OnCOMByteRecv(char cRecv)
{
	if(!m_pActiveReponse)return ;
	bool bComplete = m_pActiveReponse->OnRecvChar(cRecv);
	if(bComplete)
	{
		if(m_pActiveReponse == m_pNormalResponse)
		{
			CString str = m_pNormalResponse->GetResponseString();
		
			if(!str.GetLength())return ;

			for(size_t i=0;i<m_ATResponses.size(); ++i)
			{
				if(m_ATResponses[i]->MatchByName(str))
				{
					m_pActiveReponse = m_ATResponses[i];
					break;
				}
			}
		}
		else
			m_pActiveReponse = m_pNormalResponse;

		if(m_pActiveReponse == m_pNormalResponse)
			CATResponse::ResetRxBuf();
	}
}




int CMobileManager::WaitForATCompleted(CATCmd& cmd, DWORD dwTimeout)
{
	ASSERT(cmd.GetPacket());
	if(!m_SerialPort.IsThreadActive())//如果没有串口操纵线程，则返回超时
	{
		cmd.SetErrorCode(RETURN_TIMEOUT);
		cmd.SetEvent();
		return cmd.GetPacket()->ReturnCode();
	}

	DWORD dwBegin = GetTickCount();
	while(TRUE)
	{
		if(cmd.WaitEvent(20) == WAIT_OBJECT_0)
		{
			break;
		}
		if(GetTickCount()-dwBegin > dwTimeout)
		{
			cmd.SetErrorCode(RETURN_TIMEOUT);
			cmd.SetEvent();
			break;
		}
	}

	return cmd.GetPacket()->ReturnCode();
}

int CMobileManager::TestATCompleted(CATCmd& cmd, DWORD dwTimeout /* = 20000 */)
{
	ASSERT(cmd.GetPacket());
	if(!m_SerialPort.IsThreadActive())//如果没有串口操纵线程，则返回超时
	{
		cmd.SetErrorCode(RETURN_TIMEOUT);
		cmd.SetEvent();
		return cmd.GetPacket()->ReturnCode();
	}
	DWORD dwBegin = GetTickCount();
	while(TRUE)
	{
		if(cmd.WaitEvent(20) == WAIT_OBJECT_0)
		{
			break;
		}
		if(GetTickCount()-dwBegin > dwTimeout)
		{
			break;
		}
	}

	return cmd.GetPacket()->ReturnCode();
}

bool CMobileManager::IsModemExisting(CString strModem)
{
	/******************************************************
	if(m_strProduct == "-p")
	{
		ASSERT(m_Options.m_ComPort != "");
		CModemList modemList;
		
		std::vector<size_t> vMatched;
		for(size_t i=0; i<modemList.Count(); ++i)
		{
			const TModemInfo& info = modemList.GetAt(i);
			
			if(info.strPortAttached == m_Options.m_ComPort)
			{
				vMatched.push_back(i);
			}
		}
		if(vMatched.size()>1)
		{
			for(size_t i=0; i<modemList.Count(); ++i)
			{
				const TModemInfo& info = modemList.GetAt(i);
				
				if(info.strFriendlyName == GetModemName())
				{
					m_Options.m_strDevName = info.strFriendlyName;
					return true;
				}
			}
		}
		else if(vMatched.size()==1)
		{
			m_Options.m_strDevName = modemList.GetAt(vMatched[0]).strFriendlyName;
			return true;
		}
	}
	/*****************************************************///note by lab686 2009/10/29
//	else if((m_strProduct == "-u")||(m_strProduct == "-c"))
//	{
		CModemList modemList;
		
		std::vector<size_t> vMatched;
		for(size_t i=0; i<modemList.Count(); ++i)
		{
			const TModemInfo& info = modemList.GetAt(i);
			CString strID = info.strHardwareID;
			strID.MakeLower();
			if(//(info.strFriendlyName.Find(strModem) >=0 ) &&
				(strID.Find(TEXT("vid_05c6&pid_9003&rev_0000&mi_03")) >=0) )
			{
				m_Options.m_strDevName = info.strFriendlyName;
				m_Options.m_ComPort = info.strPortAttached;
				
				TRACE(TEXT("IsModemExisting strFriendlyName = %s\n"), info.strFriendlyName);
				TRACE(TEXT("IsModemExisting strPortAttached = %s\n"), info.strPortAttached);
				return true;
			}
		}
//	}
	m_strErrorReturn = LoadStringEx(IDS_ERROR_HARDWARE_NOT_FOUNT);
	return false;
}

bool CMobileManager::IsModemUsed()
{
	if (m_Options.GetPCUICom())
	{
		bool bOK;
		if ( !m_SerialPort.m_commonitorstate )			// modification by lab686 2009/10/28 17:43:00
			bOK = m_SerialPort.StartMonitoring(m_Options.m_PCUIComPort, 115200);
		return bOK;
	}
	else
	{
		return false;
	}
}
//检测模块
DWORD CMobileManager::CheckModule()
{
	LogString("检测嵌入软件响应", 0);

	DWORD dwTime = GetTickCount();
	m_strErrorReturn = "";
	bool bHasSim = false;
	bool bOK = false;
	CATCmd cmd;
	bool isLowBautOK = CurrentBaudIs(115200);
	if(!isLowBautOK)
	{
		cmd = SendATPacket(CMobileATProto::CheckCPIN());
		bHasSim = RETURN_OK == WaitForATCompleted(cmd,1000);

		if(bHasSim)
		{
			m_bSimExist = true;
		}
		return 115200;
	}
	m_strErrorReturn = LoadStringEx(IDS_ERROR_NOT_OPEN_COM);
	return 0;
	/***************************************************************
	while((GetTickCount()-dwTime) <= 45000)
	{
		//115200波特率测试
		bool isLowBautOK = CurrentBaudIs(115200);
		if(isLowBautOK)
		{
			cmd = SendATPacket(CMobileATProto::CheckCPIN());
			bHasSim = RETURN_OK == WaitForATCompleted(cmd,1000);
						
			if(bHasSim)
			{
				m_bSimExist = true;				
			}		
			return 115200;
		}
		else
		{
			cmd = SendATPacket(CMobileATProto::TestComATOK());
			bOK = RETURN_OK == WaitForATCompleted(cmd,1000);
			DWORD simTime = GetTickCount();
			if(bOK)
			{

				while(true)
				{
					if((GetTickCount()-simTime) > 2000)
					{
						cmd = SendATPacket(CMobileATProto::CheckCPIN());
						bHasSim = RETURN_OK == WaitForATCompleted(cmd,2000);

						if(!bHasSim)
						{
							m_strErrorReturn = LoadStringEx(IDS_ERROR_UIM_NOT_FOUND);
							m_bSimExist = false;
							return 0;
						}
						else
						{
							m_bSimExist = true;
							return 115200;
						}
					}

					
				}
			}		
		}

		//460800波特率测试
		bool isHiBautOK = CurrentBaudIs(460800);
		if(isHiBautOK)
		{
			cmd = SendATPacket(CMobileATProto::TestComATOK());
			bOK = RETURN_OK == WaitForATCompleted(cmd,1000);
			DWORD simHiBautTime = GetTickCount();
			if(bOK)
			{
				while(true)
				{
					if((GetTickCount()-simHiBautTime) > 2000)
					{
						cmd = SendATPacket(CMobileATProto::CheckCPIN());
						bHasSim = RETURN_OK == WaitForATCompleted(cmd,2000);
						if(!bHasSim)
						{
							m_bSimExist = false;
							m_strErrorReturn = LoadStringEx(IDS_ERROR_UIM_NOT_FOUND);
							return 0;
						}
						else
						{
							m_bSimExist = true;
							return 460800;
						}
					}
				}
			}
			return 460800;
		}
		else
		{
			cmd = SendATPacket(CMobileATProto::TestComATOK());
			bOK = RETURN_OK == WaitForATCompleted(cmd,1000);
			DWORD simHiBautTime = GetTickCount();
			if(bOK)
			{
				while(true)
				{
					if((GetTickCount()-simHiBautTime) > 2000)
					{
						cmd = SendATPacket(CMobileATProto::CheckCPIN());
						bHasSim = RETURN_OK == WaitForATCompleted(cmd,2000);
						if(!bHasSim)
						{
							m_bSimExist = false;
							m_strErrorReturn = LoadStringEx(IDS_ERROR_UIM_NOT_FOUND);
							return 0;
						}
						else
						{
							m_bSimExist = true;
							return 460800;
						}
					}

					
				}
				
			}							
			
		}
	}

	
	m_strErrorReturn = LoadStringEx(IDS_ERROR_NOT_OPEN_COM);
	return 0;
	/***************************************************************/
}

bool CMobileManager::CurrentBaudIs(DWORD dwCurBaud/*,bool isSecondLowBaut,bool isLowBaut*/)
{
	TRACE(TEXT("CMobileManager::CurrentBaudIs :> ENTER\n"));
	bool bOK = false;
	bool isSimExist = false;
	m_SerialPort.StopMonitoring();
	m_Options.GetPCUICom();
	if ( !m_SerialPort.m_commonitorstate )			// modification by lab686 2009/10/28 17:43:00
		bOK = m_SerialPort.StartMonitoring(m_Options.m_PCUIComPort, dwCurBaud);	

	if(bOK)
	{
		CATCmd cmd;
		for(int i=0;i<2;i++)
		{
			cmd = SendATPacket(CMobileATProto::TestComATOK());
			bOK = RETURN_OK == WaitForATCompleted(cmd,1000);
			if(bOK)
			{

				break;
			}
		}
		
		if(bOK)
		{
			for(int k = 0; k< 2; k++)
			{
				cmd = SendATPacket(CMobileATProto::CheckCPIN());
				isSimExist = RETURN_OK == WaitForATCompleted(cmd,1000);
				
				if(isSimExist)
				{
					m_bSimExist = true;
					return true;
				}
			}
			
			if(!isSimExist)
			{
				m_bSimExist = false;
				return false;
			}
		}
	}
	TRACE(TEXT("CMobileManager::CurrentBaudIs :> m_bSimExist = %d\n"), m_bSimExist);
	TRACE(TEXT("CMobileManager::CurrentBaudIs :> END\n"));
	return false;		// modification by lab686
}


bool CMobileManager::setInitialBauteRate()
{
	const int maxSetTimes = 3;
	const int maxTestATTimes = 3;
	bool bBauteSet = false;
		
	for(int i = 0;i < maxSetTimes; i++)
	{
		bool bTest = false;
//		WaitForATCompleted(SendATPacket(CMobileATProto::SetBaudrate(115200)),1000);	
		m_SerialPort.StopMonitoring();
		
		Sleep(500);
	
		if ( !m_SerialPort.m_commonitorstate )
			m_SerialPort.StartMonitoring(m_Options.m_PCUIComPort, 115200);
		
		for(int i = 0;i < maxTestATTimes;i++)
		{
			bTest = RETURN_OK == WaitForATCompleted(SendATPacket(CMobileATProto::TestComATOK()),1000);
			if(bTest)
			{
				bBauteSet = true;

				break;
			}
		}
		
		if(bBauteSet)
		{
			return true;
		}
		else
		{
			m_strErrorReturn = LoadStringEx(IDS_ERROR_NOT_OPEN_COM);
			return false;
		}
	}
	return false;
}

/*
上网
*/
bool CMobileManager::NetworkConnect()
{	
	if(WaitForSingleObject(m_hThreadConnect,0)==WAIT_TIMEOUT)return false;

	WaitForATCompleted(SendATPacket(CMobileATProto::TestSimLock()),2000);
	WaitForATCompleted(SendATPacket(CMobileATProto::CheckSYSINFO()),2000);
	WaitForATCompleted(SendATPacket(CMobileATProto::CheckSignal()),2000);
//	WaitForATCompleted(SendATPacket(CMobileATProto::CheckHDRSignal()),2000);

	if(GetConnectionStat() == CONNECT_CONTROL_NOT_CONNECT)
	{
		SetConnectionStat(CONNECT_CONTROL_DAIL);//20070514
		///启动线程
		m_EventDisconnect.ResetEvent();

		if(!m_hThreadConnect || WaitForSingleObject(m_hThreadConnect,0)==WAIT_OBJECT_0)
		{
			if(m_hThreadConnect)CloseHandle(m_hThreadConnect);
			m_hThreadConnect = CreateThreadEx(ThreadConnect, this);
		}
		if (!m_hThreadConnect)
			return false;
	}
	/*SetConnectionStat(CONNECT_CONTROL_DAIL);//20070507 jgc*/
	return true;
}
/*
断网
*/
bool CMobileManager::NetworkDisconnect()
{	
	m_EventDisconnect.SetEvent();
	return true;
}

/*
拨号连接线程
*/
DWORD WINAPI CMobileManager::ThreadConnect(LPVOID pParam)
{
	CMobileManager* pMobileMgr = reinterpret_cast<CMobileManager*>(pParam);
	pMobileMgr->RasConnect();
	return 0;
}

void GetAdvancedDialConfig(CString& strIP,CString& strDNS1,CString& strDNS2)
{
	TCHAR szValue[1024] = {0};
	DWORD nCount = sizeof(szValue)-1;
	CRegKey reg;
	CString strApn;
    
    CString regPath = GetConfigString();

	if(reg.Open(HKEY_LOCAL_MACHINE, regPath) != ERROR_SUCCESS) //打开成功
	{
		reg.Create(HKEY_LOCAL_MACHINE, regPath);		
		if(reg.Open(HKEY_LOCAL_MACHINE, regPath) != ERROR_SUCCESS)
		{
			return ;
		}
	}
	if(reg.QueryValue(szValue, TEXT("Apn"), & nCount) == ERROR_SUCCESS)
	{
		strApn = szValue;
	}
	reg.Close();

	regPath = GetConfigString()+"\\"+strApn;

	if(reg.Open(HKEY_LOCAL_MACHINE, regPath) != ERROR_SUCCESS) //打开成功
	{		
		reg.Close();
		return ;
	}
	nCount = sizeof(szValue)-1;
	if(reg.QueryValue(szValue, TEXT("IP"), & nCount) == ERROR_SUCCESS)
	{
		strIP = szValue;
	}
	nCount = sizeof(szValue)-1;
	if(reg.QueryValue(szValue, TEXT("DNS1"), & nCount) == ERROR_SUCCESS)
	{
		strDNS1 = szValue;
	}
	nCount = sizeof(szValue)-1;
	if(reg.QueryValue(szValue, TEXT("DNS2"), & nCount) == ERROR_SUCCESS)
	{
		strDNS2 = szValue;
	}

	reg.Close();
}
/*
建立拨号上网连接(entry)
*/
bool CMobileManager::ConnectRasCreate()
{
	TRACE(TEXT("CMobileManager::ConnectRasCreate :> ENTER\n"));
	if(!DetectModem())
	{
		GMessageBox(LoadStringEx(IDS_ERROR_MODEM_NOT_FOUND), TEXT(""), MB_OK|MB_ICONERROR);
		return false;
	}
		
/*	CRasEntry connection(m_Options.m_strDevName, m_Options.m_strConnection);
	if(!connection.Create(	m_Options.m_strPhoneNum, 
							m_Options.m_strStaticIP, 
							m_Options.m_strDNSMaster, 
							m_Options.m_strDNSSlave,
							m_Options.m_strUserName,
							m_Options.m_strPassword))
							*/
	CString strUser;
	CString strPassWord;
	CString strPhoneNum;

	strUser.Empty();
	strPassWord.Empty();
	strPhoneNum.Empty();

	GetDialConfigFromReg(strUser,strPassWord,strPhoneNum);
//	CString strModemName;

//	strModemName = GetModemName();

	CString strIP;
	CString strDNS1;
	CString strDNS2;

	GetAdvancedDialConfig(strIP,strDNS1,strDNS2);

	CRasEntry connection(m_Options.m_strDevName, TEXT("WCDMA"));//"WeWins Modem"
	if(!connection.Create(strPhoneNum,strIP,strDNS1,strDNS2,strUser,strPassWord))												
	{
		TRACE(TEXT("创建连接失败!\n"));
		return false;
	}
	return true;
}

void CMobileManager::GetDialConfigFromReg(CString& strUser,CString& strPassWord,CString& strPhoneNum)
{
	TCHAR szValue[1024] = {0};
	DWORD nCount = sizeof(szValue)-1;
	CRegKey reg;
    
    CString regPath = GetConfigString();

	if(reg.Open(HKEY_LOCAL_MACHINE, regPath) != ERROR_SUCCESS) //打开成功
	{
		reg.Create(HKEY_LOCAL_MACHINE, regPath);		
		if(reg.Open(HKEY_LOCAL_MACHINE, regPath) != ERROR_SUCCESS)
		{
			return ;
		}
	}
	if(reg.QueryValue(szValue, TEXT("UserName"), & nCount) == ERROR_SUCCESS)
	{
		strUser = szValue;
	}
	

	nCount = sizeof(szValue)-1;
	if(reg.QueryValue(szValue, TEXT("PassWord"), & nCount) == ERROR_SUCCESS)
	{
		strPassWord = szValue;
	}

	
	nCount = sizeof(szValue)-1;
	if(reg.QueryValue(szValue, TEXT("Phone"), & nCount) == ERROR_SUCCESS)
	{
		strPhoneNum = szValue;
		if(strPhoneNum.IsEmpty())
			strPhoneNum = TEXT("*99#");
	}
	else
	{
		strPhoneNum = TEXT("*99#");
	}
	reg.Close();
}
void CMobileManager::RasConnect()
{
	if(!ConnectRasCreate())
	{
		m_pOwner->PostMessage(WM_NET_DISCONNECTED, 0,0);

		return ;//创建Ras连接失败
	}

//	if(m_strProduct == "-p")	
//		bool bOK = RETURN_OK == WaitForATCompleted(SendATPacket(CMobileATProto::SetBaudrate(460800)),1000);/*230400->115200*/

//	if(m_strProduct == "-u")	

	WaitForATCompleted(SendATPacket(CMobileATProto::CheckSYSINFO()),2000);
	WaitForATCompleted(SendATPacket(CMobileATProto::CheckSignal()),2000);
//	WaitForATCompleted(SendATPacket(CMobileATProto::CheckHDRSignal()),2000);
	
//	m_pOwner->KillTimer(TIMER_CHECK_SIGNAL);
//	m_SerialPort.StopMonitoring();				// close by lab686
	SetConnectionStat(CONNECT_CONTROL_DAIL);


	Sleep(500);
	
	
	m_bExit = false;
	m_bConnectSuccess = false;
	m_dwRASError = 0;

	CString strUser;
	CString strPassWord;
	CString strPhoneNum;
//	CString strconnectname;

	strUser.Empty();
	strPassWord.Empty();
	strPhoneNum.Empty();
//	strconnectname.Format(TEXT("CDMA"));

	GetDialConfigFromReg(strUser,strPassWord,strPhoneNum);
	
	m_RasClient.Dial(
		"WCDMA",//strconnectname,
		strPhoneNum,
		strUser,
		strPassWord);

	bool bExceptionDisConnect = false;

	while(true)
	{
		TRACE(TEXT("CMobileManager::RasConnect :> WAIT DISCONNECT........\n"));
		if(WaitForSingleObject(m_EventDisconnect, 1000) == WAIT_OBJECT_0)//等待用户断开
		{
			TRACE(TEXT("CMobileManager::RasConnect :> DISCONNECTING....\n"));
			break;
		}

		/*********************************************************************************/
		DWORD dwError = 0;
		RASCONNSTATE stat = m_RasClient.GetStatus(&dwError);

		if( stat!= RASCS_Connected && m_bConnectSuccess)//异常断开，重新尝试连接
		{
			bExceptionDisConnect = true;
			TRACE(TEXT("In rasconnect 异常断开，ErrorCode = %d\n"), dwError);
			break;
		}
		
		if((dwError || m_dwRASError) && (dwError!=600) )
		{
			TRACE(TEXT("CMobileManager::RasConnect :> \n"));
			if(m_strProduct == "-p")
				m_pOwner->ResetCom(TEXT("4D36E96D-E325-11CE-BFC1-08002BE10318"),TEXT("CDMA 2000 EV-DO Rev.A"));

			TRACE(TEXT(" In rasconncet RAS error occured!\r\n"));
			TRACE(TEXT(" Ras:m_dwRasError = %ld\n"),m_dwRASError);

			CString strError = m_RasClient.GetErrorString(dwError);
			if(!strError.GetLength())
			{
				strError = m_RasClient.GetErrorString(m_dwRASError);
			}

			if(strError.GetLength())
			{
				m_pOwner->PostMessage(WM_NET_CONNECT_FAIL,0,0);
			}
			break;
		}
		/*********************************************************************************/
	}

	TRACE(TEXT("In rasconnect Ras hangup\n"));
	if(!m_bExit)
	{
		m_pOwner->PostMessage(WM_NET_DISCONNECTING,bExceptionDisConnect,0);
	}
	

	SetConnectionStat(CONNECT_CONTROL_HANGUP);

	m_RasClient.Hangup();
				
	///添加到上网纪录
	TRACE(TEXT("In rasconnect 上网记录\n"));

	EndFluxCount();

	RecordConnectInfo();

	if(!m_bExit)
	{
		TRACE(TEXT(" In rasconnect 重新切换回AT模式\n"));
		//重新切换回AT模式
//		m_SerialPort.StopMonitoring();
//		bool bOK;
//		if(m_strProduct == "-p")
//			bOK = m_SerialPort.StartMonitoring(m_Options.m_ComPort, 460800);
//		if(m_strProduct == "-u")	
//			if ( !m_SerialPort.m_commonitorstate )
//				bOK = m_SerialPort.StartMonitoring(m_Options.m_PCUIComPort, 115200);
//		else if(m_strProduct == "-c")
//			bOK = m_SerialPort.StartMonitoring(m_Options.m_ComPort, 115200);
/*
		if(bOK)
		{
			WaitForATCompleted(SendATPacket(CMobileATProto::SetBaudrate(115200)),1000);
			m_SerialPort.StopMonitoring();

			Sleep(500);
			if ( !m_SerialPort.m_commonitorstate )
				bOK = m_SerialPort.StartMonitoring(m_Options.m_PCUIComPort, 115200);
			if(bOK)
			{
				CATCmd cmd;
				cmd = SendATPacket(CMobileATProto::SetEcho(false));//关闭Echo
				bOK = RETURN_OK == WaitForATCompleted(cmd,1000);
			}
			TRACE(TEXT(" In rasconnect 重新切换回AT模式成功\n"));
		}
*/
		///通知界面
		TRACE(TEXT(" In rasconnect 通知界面\n"));
//		m_pOwner->m_linktype = NETTYPE_NOSERVICE;
		m_pOwner->PostMessage(WM_NET_DISCONNECTED, bExceptionDisConnect, 0);
	}
	m_bConnectSuccess = false;
}

void CMobileManager::BeginFluxCount()	//开始流量计数
{
	m_RasClient.BeginFluxCount(CCDMATime::Instance().GetCDMATime());
}

const CInfoConnect& CMobileManager::RefreshFluxCount()	//刷新流量计数
{
	return m_RasClient.RefreshFluxCount();
}

void CMobileManager::EndFluxCount()		//结束流量计数
{
	m_RasClient.EndFluxCount();
}

const CInfoConnect& CMobileManager::GetFluxCount()
{
	return m_RasClient.GetFluxCount();
}

void CMobileManager::RecordConnectInfo()
{
	const CInfoConnect& info = m_RasClient.GetFluxCount();
	if(m_bConnectSuccess && info.m_iConnectTotalSecond)
	{
		TConnectInfo infoRec;
		infoRec.iReceive = info.m_dwRecvTotal;
		infoRec.iSend    = info.m_dwSendTotal;
		infoRec.tmBeginTime = info.m_tmConnectBeginTime;
		infoRec.iTotalSecond = info.m_iConnectTotalSecond;
		
		//CConnectRecord ConnectRecord;
		//ConnectRecord.AddFront(infoRec);

		m_pOwner->m_netDlg.m_connectRecord.AddFront(infoRec);

//		m_pOwner->PostMessage(WM_REFRESH_NETRECORD,0,0);
		//m_pOwner->m_recordDlg.m_ctrlRecordList.DeleteAllCol();
		//m_pOwner->m_recordDlg.LoadListHead(NETRECORD);
		//m_pOwner->m_recordDlg.ShowListNetRecord();

		m_bConnectSuccess = false;
	}
}

/*
int CMobileManager::WaitStatusReport(DWORD milliseconds)
{
	if(WaitForSingleObject(m_EventStatusReport, milliseconds) == WAIT_OBJECT_0)
		return m_StatusReport;
	else	
		return 32768;//超时表示成功
}
*/




/*
void CMobileManager::AddPBPCMCIA(const CString& name,
				 const CString& mobile_num,
				 const CString& home_num,
				 const CString& office_num,
				 const CString& email,
				 const CString& remark)
{
	AddEmbedPB(m_pPBMgr->NormalPhonebook(),
				 name,
				 mobile_num,
				 home_num,
				 office_num,
				 email,
				 remark);
}
*/
void CMobileManager::PostMessageToClient(UINT message, WPARAM wParam, LPARAM lParam )
{
	if(m_pOwner->GetSafeHwnd())
		m_pOwner->PostMessage(message,wParam,lParam);
}

void CMobileManager::SendMessageToClient(UINT message, WPARAM wParam, LPARAM lParam )
{
	if(m_pOwner->GetSafeHwnd())
		m_pOwner->SendMessage(message,wParam,lParam);
}

void CMobileManager::RestartCOMMonitor()
{
	m_SerialPort.StopMonitoring();	
	m_SerialPort.StartMonitoring(m_Options.m_PCUIComPort, 115200);
}

int  CMobileManager::SendFlash(const char* strNum)
{
	CATCmd cmd = SendATPacket(CMobileATProto::SendFlash(strNum));
	return WaitForATCompleted(cmd, 300);
}

bool CMobileManager::CallHold(int code)
{
	CATCmd cmd = SendATPacket(CMobileATProto::CallHold(code));
	return RETURN_OK == WaitForATCompleted(cmd, 1500);
}

bool CMobileManager::CallForwardSet(int reason, int mode, const char* strNum)
{
	CATCmd cmd = SendATPacket(CMobileATProto::CallForwardSet(reason,mode,strNum));
	return RETURN_OK == WaitForATCompleted(cmd, 10000);
}

bool CMobileManager::CallForwardCancel(int reason, int mode)
{
	CATCmd cmd = SendATPacket(CMobileATProto::CallForwardCancel(reason,mode));
	return RETURN_OK == WaitForATCompleted(cmd, 10000);
}

bool CMobileManager::PhoneBookModify(const CString& phonebookType, int nID, const CString& strPhoneNum, const CString& strName)
{
	//选择电话本
	CATCmd cmd;
	cmd = SendATPacket(CMobileATProto::PhoneBookSelect(phonebookType));
	WaitForATCompleted(cmd,1000);
	
       //电话本新增或修改
      cmd = SendATPacket(CMobileATProto::PhoneBookModify(nID,strPhoneNum,strName));
	
    return WaitForATCompleted(cmd,6000) == RETURN_OK;
}

bool  CMobileManager::PhoneBookDelete(const CString& phonebookType,int nID)
{
	//选择电话本
	CATCmd cmd;
//	cmd = SendATPacket(CMobileATProto::PhoneBookSelect(phonebookType));
//	WaitForATCompleted(cmd,2000);
	
    //电话本删除
	cmd = SendATPacket(CMobileATProto::PhoneBookDelete(nID));
    return WaitForATCompleted(cmd,6000) == RETURN_OK;
}

//sms
void SMSLoadProgress(CDlgProgressEx* pUI, int nPos, int nMax, const TSMSItem& sms)
{
	if(pUI)
	{
		CString str;
		str.Format(TEXT("[%s]%s"), sms.strPhoneNum, GetLeftString(sms.strText, 4));
		pUI->SetProgress(nPos, nMax, str);
	}
}

class CThreadLoadSMS: public CPrgressThread
{
	CMobileManager& m_Mgr;
	void (CMobileManager::*m_Func)(CDlgProgressEx* pUI);
public:
	CThreadLoadSMS(CMobileManager& mgr, void (CMobileManager::*Func)(CDlgProgressEx*))
		:m_Mgr(mgr), m_Func(Func)
	{
	}
protected:
	virtual void DoWork()
	{
		(m_Mgr.*m_Func)(m_pUI);
	}
};

void CMobileManager::SMSEnum()
{
	if(m_bSMSLoaded)return ;

	void (CMobileManager::*func_enumSMS)(CDlgProgressEx*) = NULL;


	func_enumSMS = SMSLoadFrom;//jiangguochao 20070115 added

	if(func_enumSMS)
	{
		CThreadLoadSMS thread(*this, func_enumSMS);
		CDlgProgressEx dlg(thread, NULL, false);
		dlg.DoModal();
	}

	m_bSMSLoaded = func_enumSMS != NULL;
}

void CMobileManager::SMSLoadFrom(CDlgProgressEx* pUI)
{
	///读取收件箱
	m_iSMSCount = 0;
	m_iSMSMaxNum = -1;

	if(!SMSBoxParam("SM"))
	{
		return ;//jiangguochao changed from MT ->SM 20070115 
	}
	
	int nNew = m_iSMSCount - m_pOwner->m_smsDlg.m_EmbedRecv.Size();//m_SMSMgr.GetSMSEmbedRecv().Size()
	for(int i = 0; i<=40; i ++)//m_iSMSMaxNum && nNew>0
	{
		if(!(m_pOwner->m_smsDlg.m_EmbedRecv.FindID(i)
			||m_pOwner->m_smsDlg.m_EmbedSend.FindID(i)
			||m_pOwner->m_smsDlg.m_EmbedDraft.FindID(i)))
		{
			const TSMSItem* pSMSNew = ReadSMS(i,6000, "SM");//jiangguochao changed from MT ->SM 20070115
			if(pSMSNew)
			{
				AddNewEmbedSMS(*pSMSNew);
				SMSLoadProgress(pUI, i+1, m_iSMSCount, *pSMSNew);
				-- nNew;
			}
		}
	}

	m_pOwner->m_smsDlg.m_EmbedRecv.SortByTime();
	m_pOwner->m_smsDlg.m_EmbedSend.SortByTime();
	m_pOwner->m_smsDlg.m_EmbedDraft.SortByTime();
}

const TSMSItem* CMobileManager::ReadSMS(int i, DWORD dwTimeout, const CString& storage)
{
	TRACE(TEXT("CMobileManager::ReadSMS :> READ SMS\n"));

	CATCmd cmd = SendATPacket(CMobileATProto::SMSBoxSelect(storage));
	bool bOK = RETURN_OK == WaitForATCompleted(cmd, dwTimeout);
	if (!bOK)
		return NULL;

	///读取
	m_NewSMS.iID = i;
	m_NewSMS.strPhoneNum.Empty();
	Sleep(200);
	TRACE(TEXT("m_NewSMS.iID = %d\n"),m_NewSMS.iID);
	bOK = RETURN_OK == WaitForATCompleted( SendATPacket(CMobileATProto::SMSRead(i)), dwTimeout);
	const TSMSItem* pSMS = bOK? &m_NewSMS : NULL;

	if (m_NewSMS.strPhoneNum.IsEmpty())
	{
		return NULL;
	}
	else
	{
		return pSMS;
	}
}

void CMobileManager::AddNewEmbedSMS(const TSMSItem& info)
{
	if((m_pOwner->m_smsDlg.m_EmbedRecv.FindID(info.iID)
		||m_pOwner->m_smsDlg.m_EmbedSend.FindID(info.iID)
		||m_pOwner->m_smsDlg.m_EmbedDraft.FindID(info.iID)))
		
		return;//重复ID不记录

	switch(info.iSMSState)
	{
	case SMS_READ:
	case SMS_UNREAD:
		m_pOwner->m_smsDlg.m_EmbedRecv.AddFront(info);
		break;
	case SMS_SEND:
		m_pOwner->m_smsDlg.m_EmbedSend.AddFront(info);
		break;
	case SMS_UNSEND:
		m_pOwner->m_smsDlg.m_EmbedDraft.AddFront(info);
		break;
	}
}

BOOL CMobileManager::pfnCopsRegsiterObserver(pfnCOPS dlVersion)
{
	g_pfnDwonLoadVerCallBack = dlVersion;
	return TRUE;
}
