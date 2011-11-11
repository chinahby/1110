// MobileManager.h: interface for the CMobileManager class.
//
//////////////////////////////////////////////////////////////////////

#if !defined(AFX_MOBILEMANAGER_H__9674B603_EC9D_4D2F_840D_33676FBC8B7B__INCLUDED_)
#define AFX_MOBILEMANAGER_H__9674B603_EC9D_4D2F_840D_33676FBC8B7B__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

#include <afxmt.h>
#include <vector>
#include "COMMonitor.h"
#include "SetManager.h"
#include "SetNetConnectDlg.h"
#include "ATResponse.h"
#include "GlobalDataCenter.h"
#include "RasClient.h"
#include "data\SMS.h"

//#include "InfoDialup.h"

/*
enum EEmbedModule
{
	MODULE_5105,
	MODULE_6025
};
*/

class CSunnyCatDlg;
class CATResponse;
class CCallManager;
class CDlgProgressEx;


class CMobileManager: public IRasListener
{
public:
	CMobileManager(CSunnyCatDlg *pOwner);
	~CMobileManager();
	CSunnyCatDlg *m_pOwner;

	bool Init();
	bool setInitialBauteRate();
	bool ShutDown();
	void RestartCOMMonitor();

public:
	const CString& GetErrorMsg()const{return m_strErrorReturn;}
	void CheckSignal();
	CATCmd SendATPacket(CMobilePacket* Packet);
	int WaitForATCompleted(CATCmd& cmd, DWORD dwTimeout = 20000);
	int TestATCompleted(CATCmd& cmd, DWORD dwTimeout = 20000);
	void OnCOMByteRecv(char cRecv);
	CATResponse* FindParser(const CString& parserName)const;
	int  SendFlash(const char* strNum);
public:

	const CString& GetPortDevName()const;
	//EEmbedModule GetEmbedModule();
	bool CheckRUIMSupporting();
	bool CheckSimExist();
	bool IsRUIMSupported()const{return m_bRUIMSupported;}
	int NetResgisterStatus()const{return m_registerStatus;}
	int IsGprsAttached()const{return m_netGprs;}
	int IsEdgeAttached()const{return m_netEdge;}
	//int PbNumMaxLen()const{return m_pbNumMaxlen;}
	//int PbNameMaxLen()const{return m_pbNameMaxlen;}



private:
	EConnectionState m_ConnStat; //连接状态

public:
//	bool DialCallOut(const CString& strPhoneNum, const CString& strName);
//	bool DialHangUp();
//	void DialKeyInput(int iCtrl, const CString& strKey);
	bool CallHold(int code);
	bool CallForwardSet(int reason, int mode, const char* strNum);
	bool CallForwardCancel(int reason, int mode);
	
public:
	bool ConnectRasCreate();
	bool NetworkConnect();
	bool NetworkDisconnect();
	void RasConnect();
	EConnectionState GetConnectionStat()const
	{
		return m_ConnStat;
	}
	CRasClient m_RasClient;
private:
	void OnRasMsg(UINT unMsg,
					RASCONNSTATE rascs,
					DWORD dwError,
					DWORD dwExtendedError);

public:
	void SetConnectionStat(EConnectionState stat)
	{
		m_ConnStat = stat;
	}
private:

	static DWORD WINAPI ThreadConnect(LPVOID pParam);

public:
	bool SendSMS_MultiByte(const CString& strPhoneNum, 
					int iPriority, 
					const CString& strText);
	bool SendSMS_ASCII(const CString& strPhoneNum, 
					int iPriority, 
					const CString& strText);
	bool SendSMS_SingleByte(const CString& strPhoneNum, 
					int iPriority, 
					const CString& strText);


	bool SMSBoxParam(const CString& strType);

	void GetIMEI();
public:
	bool SetVolume(int iVolumn);
	bool GetVolume(int& iVolumn);
	CCallManager& GetCallManager();
	const CCallManager& GetCallManager()const;
	const CString& 	GetPinResponse()const;


public://初始化函数
	bool IsModemExisting(CString strModem);
	bool IsModemUsed();
	bool DetectModem();//检测modem
	void installModemInitialAT(CString strText);//设置modem初始化AT
	DWORD CheckModule();//检测模块
	bool CurrentBaudIs(DWORD dwCurBaud/*,bool isSecondLowBaut = false,bool isLowBaut = false*/);
	bool InstallModem();
	bool IsDeviceExisting(void);
	int  m_iNetMode;
	bool m_bSetNetModeOK;
	bool m_bIsSendSMS;	// 记录是否正在发送短信add by lab 200912081655
	int m_iRecNewSMSLocal[20];	// 记录在发送短信时上报的新短信位置

	bool ProcessNewSMSPostMsg(int local);	//add by lab 200912081655
	bool m_bIsAboveSendSMS;

//081110he=====================================================
	typedef void (CALLBACK *pfnCOPS)(void* pVoid);
	pfnCOPS g_pfnDwonLoadVerCallBack;
	BOOL pfnCopsRegsiterObserver(pfnCOPS dlVersion);
//=============================================================
public:

	CString m_strSysTime;

	///本机号码
	//CString m_strLocalNum;

	//短消息中心
	CString m_strCscaNum;

	///本机版本
	CString m_strLocalVersion;

	//SIM卡找不到是否已经上报
	//bool noSimHasReport;

	///ZFNM
	CString m_strZFNM;

	///网络注册状态
	int  m_registerStatus;

	int m_netGprs;
	int m_netEdge;

	int m_CurACT;
	int m_CurFormat;
	bool m_IsRegister;
	CString m_strCurNet;//
	CStringArray m_AllNet;
	CString m_imei;

	CStringArray m_AllNetName;
	CStringArray m_AllNetRegister;

// 	bool m_brequestQuit;

	///串口
	CCOMMonitor  m_SerialPort;
	CString m_ComName;

private:

	int m_iSMSMaxNum;
	int m_iSMSCount;


public:
	void ExitConnectThread();
private:
	friend class CNormalResponse;
	//friend class CResponseCDS;
	//friend class CResponseCCLK;
	friend class CResponseCSQ;
	friend class CResponseHDRCSQ;
	friend class CResponsePREFMODE;
	friend class CResponseMODE;
	friend class CResponseCREG;
	friend class CResponseCGATT;
	friend class CResponseEDGE;
	friend class CResponseIMEI;
	friend class CResponseSimLock;
	friend class CResponseCONN;
	friend class CResponseRING;
	friend class CResponseCLIP;
	friend class CResponseCEND;
	friend class CResponseCIND;
	friend class CResponseCLVL;
	friend class CResponseSMSCMGS;
	friend class CResponseSMSERROR;
	//friend class CResponseZMGF;
	friend class CResponseCMTI;
	friend class CResponseSMSFULL;
	friend class CResponseSIMER;
	friend class CResponseCMGR;
	friend class CResponseCPMS;
	friend class CResponseCPBW;
	friend class CResponseCPBR;
//	friend class CResponseCNUM;
	friend class CResponseATI3;
	friend class CResponseZDPB;
	friend class CResponseZFNM;
	friend class CResponseZVMI;
//	friend class CResponseCLCK;
	friend class CResponseCMEError;
	friend class CResponseCMSError;
	friend class CResponseError;
	friend class CResponseOK;
	friend class CResponseCMT;
	friend class CResponseCPIN;
	friend class CResponseORIG;
	friend class CResponseIPR;
	friend class CResponseZCRDN;
	friend class CResponseTINFO;

	friend class CResponseBusyNow;//jiangguochao added 20070115
	friend class CResponseConnected;//jiangguochao added 20070115
	friend class CResponseNoAnswer;//jiangguochao added 20070115
	friend class CResponseNoCarrier;//jiangguochao added 20070115
	friend class CResponseCPOL;
	friend class CResponseCOPS;
	friend class CResponsePPP;

	std::vector<CATResponse*> m_ATResponses;
	CNormalResponse* m_pNormalResponse;
	CATResponse* m_pActiveReponse;
private:
	void InitATResponse();
	void CleanATResponse();
public:
	
	CEvent	m_EventDisconnect;	//连接线程退出事件
	HANDLE	m_hThreadConnect;	//连接线程handle

	bool	m_bConnectSuccess;
private:
	DWORD   m_dwRASError;		//RAS error code

	int		m_nCurBaud;			//Current baud rate
	int		m_nVolumn;			//AT+CLVL?返回的音量
	CString m_strCurLock;		//当前设备锁AI或AO

	
	CString m_strErrorReturn;	//错误信息
	//CEvent	m_EventStatusReport;//短信状态报告事件
	//int		m_StatusReport;		//短信状态报告码
public:
	const CInfoConnect& RefreshFluxCount();	//刷新流量计数
	const CInfoConnect& GetFluxCount();
	void RecordConnectInfo();
	void GetDialConfigFromReg(CString& strUser,CString& strPassWord,CString& strPhoneNum);
private:
	void BeginFluxCount();	//开始流量计数
	void EndFluxCount();	//结束流量计数

public:
	bool PhoneBookModify(
				const CString& phonebookType,
				int nID, 
				const CString& strPhoneNum, 
				const CString& strName
				);

	bool PhoneBookDelete(const CString& phonebookType,int nID);

//sms
public:
	void SMSEnum();
	const TSMSItem* ReadSMS(int i, DWORD dwTimeout, const CString& storage);
	void AddNewEmbedSMS(const TSMSItem& info);
private:
	void SMSLoadFrom(CDlgProgressEx* pUI);
	

	TSMSItem m_NewSMS;///新收到的短信缓存

public:
	//等待短信状态报告，-1表示超时，其他表示返回码
	int WaitStatusReport(DWORD milliseconds);

	//发送按键音
	void SendKey(const CString& strKey);

	void PostMessageToClient(UINT message, WPARAM wParam = 0, LPARAM lParam = 0 );
	void SendMessageToClient(UINT message, WPARAM wParam = 0, LPARAM lParam = 0 );
	///配置信息
	CSetManager m_Options;
	
	CCallManager* m_pCallMgr;
	bool isAbleCom;
	bool hasInserted;
	bool bRemoved;
	bool m_simLocked;

	CString m_strProductType;
	CString m_strSWRV;
	size_t m_nSignal; 
	size_t m_nHDRSignal;	 // ADD BY LAB686  20091105

	size_t m_callnum;	// add by lab686	20091029
	bool m_bSimExist;
	bool		  m_bSMSLoaded;
	bool 	m_bpinpuk;			// true is pin false if puk
	BYTE 	m_uremainingtime;	// pin or puk remaining time
private:
	bool m_bRUIMSupported;
	
	CString m_strPortDevName;
	//int m_pbNumMaxlen;
	//int m_pbNameMaxlen;
	

private:
	
	int			  m_nBindTag;//绑定标志
public:
	CString m_strPINResponse;
	bool		  m_bExit;
private:
	bool		  m_bDeviceIsForPDA;
	
	CString		  m_strProduct;
public:
	size_t m_nPPPSecond;
};

#endif // !defined(AFX_MOBILEMANAGER_H__9674B603_EC9D_4D2F_840D_33676FBC8B7B__INCLUDED_)
