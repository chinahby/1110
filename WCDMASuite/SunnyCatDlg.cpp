// SunnyCatDlg.cpp : implementation file
//

#include "stdafx.h"
#include "GlobalDataCenter.h"
#include "SunnyCat.h"
#include "SunnyCatDlg.h"
#include "BmpBkDialog.h"
#include "ColorBtn.h"
#include "About.h"
#include "SetDlg.h"
#include "Util.h"
#include "Porting.h"
#include "DlgProgress.h"
#include "dlgmsg.h"
#include "math.h"
#include "data\phonebookpc.h"
#include "data\phonebookembed.h"
#include "winaddrbook.h"
#include <atlbase.h>
#include "ProgressWnd.h"
#include "NewCardDlg.h"
#include <dbt.h>
#include "contacts.h"
#include "WtsApi32.h"
#include "ImportRepeatPrompt.h"
#include "pbt.h"
#include "OurSetupapi.h"
#include "Update.h"
#include "UpdateProgressDlg.h"
#include "MyIFStream.h"
#include <Tlhelp32.h>
#include "MyDBT.h"
#include "DevEnum.h"
#include "ThreadWaitAuotRun.h"
#include "pindialog.h"
#include "pinoperdialog.h"
#include "selectnetworkdlg.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

static CEvent g_EventStopUpdate(FALSE,TRUE);	//自动更新线程停止事件

#define WM_WTSSESSION_CHANGE            0x02B1

#define WTS_CONSOLE_CONNECT                0x1
#define WTS_CONSOLE_DISCONNECT             0x2
#define WTS_REMOTE_CONNECT                 0x3
#define WTS_REMOTE_DISCONNECT              0x4
#define WTS_SESSION_LOGON                  0x5
#define WTS_SESSION_LOGOFF                 0x6
#define WTS_SESSION_LOCK                   0x7
#define WTS_SESSION_UNLOCK                 0x8
#define WTS_SESSION_REMOTE_CONTROL         0x9

/*char *pszCardFields[] = 
{
	"职务",//0
	"邮件地址",//1
	"商务地址 国家/地区",//2
	"住宅电话",//3
	"商务传真",//4
	"单位",//5
	"商务电话",//6
	"名字",//7
	"移动电话",//8
	"分类"//9
};
*/

CString pszCardFields[] = 
{
	LoadStringEx(IDS_OUTLOOK_HEADSHIP),
	LoadStringEx(IDS_OUTLOOK_MAIL),
	LoadStringEx(IDS_OUTLOOK_COUNTRY),
	LoadStringEx(IDS_OUTLOOK_HOME),
	LoadStringEx(IDS_OUTLOOK_FAX),
	LoadStringEx(IDS_OUTLOOK_COMPANY),
	LoadStringEx(IDS_OUTLOOK_OFFICE),
	LoadStringEx(IDS_OUTLOOK_NAME),
	LoadStringEx(IDS_OUTLOOK_MOBPHONE),
	LoadStringEx(IDS_OUTLOOK_CLASS)
};

HWND CSunnyCatDlg::m_MC5WindowHandle		= NULL;
wchar_t CSunnyCatDlg::m_pwcSDIPath[MAX_PATH]	= {0};
wchar_t CSunnyCatDlg::m_pwcSDIPathCE[MAX_PATH]	= {0};

/////////////////////////////////////////////////////////////////////////////
// CSunnyCatDlg dialog
//CSkinButton * btn = new CSkinButton();
CSunnyCatDlg::CSunnyCatDlg(CWnd* pParent /*=NULL*/)
	: CDialog(CSunnyCatDlg::IDD, pParent),
	m_MobileManager(this),m_InitAT(m_MobileManager)	
{
	//{{AFX_DATA_INIT(CSunnyCatDlg)
	//}}AFX_DATA_INIT
	// Note that LoadIcon does not require a subsequent DestroyIcon in Win32
	m_hIcon = AfxGetApp()->LoadIcon(IDR_MAINFRAME);	
	m_isConnect = false;

	g_pSysImageList = new CImageList();

	wab = new CWinAddrBook();
	outlookContact = new COutlookContact();


	m_bMinmize = FALSE;

	m_bConnected = FALSE;
	m_bUnconnected = TRUE;

	m_removePrompt = new CRemovePrompt(this);
	m_removePrompt->Create(this,IDD_DIALOG_REMOVE_PROMPT);

	m_hMoudle = NULL;

	m_hModuleDDK = NULL;
	m_bIsResetCom = false;

	m_UpdateThread = NULL;
	m_UpdateDlg = NULL;
	m_bHandUpdate = FALSE;
	m_bAutoupdate = FALSE;

	m_bFluxWarnDoModal = FALSE;
	m_bFluxUncntDoModal = FALSE;
	m_bWarnNMdlg = FALSE;

	m_bMC5Enable = TRUE;
	m_bMC5Disable = FALSE;

	m_hMC5Process = NULL;
	m_hMC5ProcessCE = NULL;

	m_bFirst = TRUE;

	m_strProduct.Empty();
	m_strLanguage.Empty();
	m_NewSMSNum.Empty();	//来记录短信的电话号码
	m_NewSMSText.Empty();  //来记录短信的内容

	m_hRegModule = NULL;
	m_RegHandle = NULL;

	m_bReset = true;

	m_strProduct = GetProduct();
	m_strLanguage = GetLanguage();

	m_dwConnectSecond = 0;

	m_pWaveLib = NULL;
	m_bPCMVoice = false;

	m_pRecorder = NULL;
	m_pRecorderSerial = NULL;

	strShowText = "";
    bShow = false;
	m_autoregister = false;
	m_bsmssendsucc = false;
	m_smssendfail = 0;
	m_smssendsucc = 0;

	newFont.CreateFont(
	14,                        // nHeight
	0,                         // nWidth
	0,                         // nEscapement
	0,                         // nOrientation
	FW_NORMAL,                 // nWeight
	FALSE,                     // bItalic
	FALSE,                     // bUnderline
	0,                         // cStrikeOut
	ANSI_CHARSET,              // nCharSet
	OUT_DEFAULT_PRECIS,        // nOutPrecision
	CLIP_DEFAULT_PRECIS,       // nClipPrecision
	DEFAULT_QUALITY,           // nQuality
	DEFAULT_PITCH | FF_SWISS,  // nPitchAndFamily
	TEXT("Arial"));                 // lpszFacename

	m_bCntSuccess = false;
	m_bResetting = false;
	m_bfirstmove = false;		// chagne to false by lab686 20091214
	m_bIsLockUIM = false;
	
	m_linktype = NETTYPE_NOSERVICE;
}

void LogString(const char* str,size_t size);

CSunnyCatDlg::~CSunnyCatDlg()
{
	newFont.DeleteObject();
	delete g_pSysImageList;
	g_pSysImageList = NULL;

	delete wab;
	wab = NULL;

	delete outlookContact;
	outlookContact = NULL;

	m_removePrompt->DestroyWindow();
	delete m_removePrompt;
	m_removePrompt = NULL;

	typedef BOOL (WINAPI * pfWTSUnRegisterSessionNotification)(HWND hWnd);
	pfWTSUnRegisterSessionNotification pfUnN = NULL;
	pfUnN = (pfWTSUnRegisterSessionNotification)::GetProcAddress(m_hMoudle,"WTSUnRegisterSessionNotification");
	if(pfUnN != NULL)
	{
		pfUnN(this->GetSafeHwnd());
	}
	
	
	if(m_hMoudle != NULL)
		::FreeLibrary(m_hMoudle);

	

	if(m_RegHandle != NULL)
	{
		typedef BOOL (WINAPI* pfUnregisterDeviceNotification)(IN HDEVNOTIFY Handle);
		pfUnregisterDeviceNotification pfUnregN = NULL;
		pfUnregN = (pfUnregisterDeviceNotification)::GetProcAddress(m_hRegModule,"UnregisterDeviceNotification");
		if(pfUnregN != NULL)
		{
			pfUnregN(m_RegHandle);
		}
		
	}
	if(m_hRegModule != NULL)
		::FreeLibrary(m_hRegModule);

	if(m_pWaveLib != NULL)
	{
		NMWave::ReleaseWaveLib(m_pWaveLib);
	}
}

void CSunnyCatDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	//{{AFX_DATA_MAP(CSunnyCatDlg)
	DDX_Control(pDX, IDC_MENU_OPERATION, m_OperationBtn);
//	DDX_Control(pDX, IDC_BUTTON_RECORD, m_RecordBtn);
	DDX_Control(pDX, IDC_MENU_SETLINKMAN, m_ctrlContactBtn);
	DDX_Control(pDX, IDC_MENU_HELP, m_ctrlHelpBtn);
	DDX_Control(pDX, IDC_MENU_FILE, m_ctrlFileBtn);
	DDX_Control(pDX, IDC_STATIC_INFOBAR, m_ctrlInfoBar);
	DDX_Control(pDX, IDC_STATIC_GSM, m_netGsm);
	DDX_Control(pDX, IDC_STATIC_GPRS, m_netGprs);
	DDX_Control(pDX, IDC_STATIC_EG, m_netEdge);
	DDX_Control(pDX, IDC_STATIC_NOTREG, m_notRegister);
	DDX_Control(pDX, IDC_STATIC_REG, m_regSuccess);
	DDX_Control(pDX, IDC_STATIC_ROAMING, m_roaming);
	DDX_Control(pDX, IDC_STATIC_SMSIMG, m_imgSMSPromt);
	DDX_Control(pDX, IDC_STATIC_EDGE, m_staOnLine);
	DDX_Control(pDX, IDC_STATIC_S1, m_s1);
	DDX_Control(pDX, IDC_STATIC_S0, m_s0);
	DDX_Control(pDX, IDC_BUTTON_UNCONNECT, m_unconnect);
	DDX_Control(pDX, IDC_BUTTON_CONNECT, m_connect);
//	DDX_Control(pDX, IDC_BUTTON_UNCONNECTEVDO, m_unconnectevdo);	// add by lab686 20091105
//	DDX_Control(pDX, IDC_BUTTON_CONNECTEVDO, m_connectevdo);
	DDX_Control(pDX, IDC_STATIC_S2, m_s2);
	DDX_Control(pDX, IDC_STATIC_S3, m_s3);
	DDX_Control(pDX, IDC_STATIC_S4, m_s4);
	DDX_Control(pDX, IDC_STATIC_S5, m_s5);
	//*******************************************************************
	// Marked By .... 09.11.04
	DDX_Control(pDX, IDC_STATIC_ICON0, m_s0_h);
	DDX_Control(pDX, IDC_STATIC_ICON1, m_s1_h);
	DDX_Control(pDX, IDC_STATIC_ICON2, m_s2_h);
	DDX_Control(pDX, IDC_STATIC_ICON3, m_s3_h);
	DDX_Control(pDX, IDC_STATIC_ICON4, m_s4_h);
	DDX_Control(pDX, IDC_STATIC_ICON5, m_s5_h);

	DDX_Control(pDX, IDC_STATIC_EVDO, m_sEvdo);
	DDX_Control(pDX, IDC_STATIC_1X, m_sOneX);
	//********************************************************************/
	DDX_Control(pDX, IDC_BUTTON_CALL, m_CallBtn);
	DDX_Control(pDX, IDC_BUTTON_SMS, m_SMSBtn);
	DDX_Control(pDX, IDC_BUTTON_NET, m_NetBtn);
	DDX_Control(pDX, IDC_BUTTON_LINK, m_LinkBtn);
	DDX_Control(pDX, IDC_BUTTON_LINKMAN, m_LinkManBtn);
	DDX_Control(pDX, IDC_STATIC_LINE1, m_Line1);
	DDX_Control(pDX, IDC_STATIC_LINE2, m_Line2);
	DDX_Control(pDX, IDC_STATIC_LINE3, m_Line3);
	DDX_Control(pDX, IDC_STATIC_NOTONLINE, m_NotOnLine);
	//}}AFX_DATA_MAP
}
enum{WM_CONN_STAT = WM_USER+203};

BEGIN_MESSAGE_MAP(CSunnyCatDlg, CDialog)
	//{{AFX_MSG_MAP(CSunnyCatDlg)
	ON_MESSAGE(WM_NOTIFYICON,OnNotifyIcon)
	ON_WM_PAINT()
	ON_WM_QUERYDRAGICON()
	ON_WM_LBUTTONDOWN()
	ON_BN_CLICKED(IDC_MENU_FILE, OnMenuFile)
	ON_BN_CLICKED(IDC_MENU_HELP, OnMenuHelp)
	ON_NOTIFY(TCN_SELCHANGE, IDC_TAB, OnSelchangeTab)
	ON_WM_KEYDOWN()
	ON_BN_CLICKED(IDC_BUTTON_CONNECT, OnButtonConnect)
	ON_BN_CLICKED(IDC_BUTTON_UNCONNECT, OnButtonUnconnect)
//	ON_BN_CLICKED(IDC_BUTTON_CONNECTEVDO, OnButtonConnectevdo)			// add by lab686 20091105
//	ON_BN_CLICKED(IDC_BUTTON_UNCONNECTEVDO, OnButtonUnconnectevdo)
//	ON_COMMAND(ID_MENU_UNCONNECT, OnMenuUnconnect)
//	ON_COMMAND(IDR_MENU_CONNECT, OnMenuConnect)
	ON_COMMAND(IDR_MENU_CLOSE, OnMenuClose)
	ON_COMMAND(IDR_MENU_HELPINFO, OnMenuHelpinfo)
	ON_COMMAND(ID_MENU_ABOUT, OnMenuAbout)
	ON_BN_CLICKED(IDC_MENU_SETLINKMAN, OnMenuSetlinkman)
	ON_COMMAND(IDR_MENU_SET, OnMenuSet)
	ON_COMMAND(IDR_MENU_NETWORK, OnMenuNetwork)
	ON_WM_DESTROY()
	ON_WM_TIMER()
	ON_MESSAGE(WM_DIAL_SHOW, OnDialShow)
	ON_BN_CLICKED(IDC_STATIC_SMSIMG, OnStaticSmsimg)
	ON_MESSAGE(WM_CALL_INCOMING,OnQueryCallStatus)
	ON_MESSAGE(WM_SIGNAL_SIMLOCK,OnUnLockUIM)// LAB686
	ON_MESSAGE(WM_SHOW_SIGNAL, OnShowSignal)
	ON_MESSAGE(WM_SHOW_NETWORK, OnShowNetwork)
	ON_MESSAGE(WM_SET_AUTO, OnSetAuto)		
	ON_MESSAGE(WM_SHOW_HDRSIGNAL, OnShowHDRSignal)// LAB686
	ON_MESSAGE(WM_SMSSEND_STATE, OnSetSMSSendState)// LAB686
	ON_MESSAGE(WM_SERIALPORT_ERROR, OnSerialLport)// LAB686
	ON_MESSAGE(WM_REG_STATUS,OnNetRegStatusChange)
	ON_MESSAGE(WM_NET_TYPE,OnNetAttachType)
	ON_MESSAGE(WM_SMS_NEW, OnSMSNew)
	ON_MESSAGE(WM_CONN_STAT,OnMsgStatusChange)
	ON_MESSAGE(WM_NET_CONNECTING, OnNetConnecting)
	ON_MESSAGE(WM_NET_CONNECTED, OnNetConnected)
	ON_MESSAGE(WM_NET_DISCONNECTED, OnNetDisconnected)
	ON_MESSAGE(WM_SMS_NOTIFY, OnSMSNotify)
	ON_WM_SIZE()
	ON_WM_GETMINMAXINFO()
	ON_WM_CLOSE()
	ON_MESSAGE(WM_SMS_READ, OnChangSmsImg)
	ON_MESSAGE(WM_SMSSEND_SHOW, OnShowSMSSendInfo)
	ON_WM_SYSCOMMAND()
	ON_COMMAND(IDR_LINKMAN_INPUT, OnLinkmanInput)
	ON_COMMAND(IDR_LINKMAN_OUTPUT, OnLinkmanOutput)
	ON_COMMAND(IDR_PINOPERATION_ENABLE, OnPINOperationEnable)
	ON_COMMAND(IDR_PINOPERATION_DISABLE, OnPINOperationDisable)
	ON_COMMAND(ID_SYN_TO_OUTLOOK, OnSynToOutlook)
	ON_COMMAND(ID_SYN_FROM_OUTLOOK, OnSynFromOutlook)
	ON_WM_NCCREATE()
	ON_COMMAND(IDR_MENU_ADDLINKMAN, OnMenuAddlinkman)
	ON_MESSAGE(WM_DEVICECHANGE, OnDeviveChange)
	ON_COMMAND(ID_SYN_FROM_OO, OnSynFromOO)
	ON_COMMAND(ID_SYN_TO_OO, OnSynToOO)
	ON_MESSAGE(WM_POWERBROADCAST, OnPower)
	ON_MESSAGE(WM_SIM_LOCK, OnSIMLock)
	ON_BN_CLICKED(IDC_BUTTON_LINK, OnButtonLink)
	ON_BN_CLICKED(IDC_BUTTON_NET, OnButtonNet)
	ON_BN_CLICKED(IDC_BUTTON_CALL, OnButtonCall)
	ON_BN_CLICKED(IDC_BUTTON_SMS, OnButtonSms)
	ON_BN_CLICKED(IDC_BUTTON_LINKMAN, OnButtonLinkman)
	ON_BN_CLICKED(IDC_BUTTON_RECORD, OnButtonRecord)
	ON_BN_CLICKED(IDC_MENU_OPERATION, OnMenuOperation)
	ON_COMMAND(IDM_OPREATION_LINK, OnOperationLink)
	ON_COMMAND(IDM_OPERATION_NETWORK, OnOperationNetWork)
	ON_COMMAND(IDM_OPERATION_CALL, OnOperationCall)
	ON_COMMAND(IDM_OPERATION_SMS, OnOperationSMS)
	ON_COMMAND(IDM_OPERATION_PB, OnOperationPB)
	ON_WM_CTLCOLOR()
	ON_WM_ERASEBKGND()
	//}}AFX_MSG_MAP
	ON_MESSAGE(WM_SHOW_CALLIMAG,OnShowCallImag)
	ON_MESSAGE(WM_REFRESH_NETRECORD,OnRefreshNetRecord)
	ON_MESSAGE(WM_REFRESH_CALLRECORD,OnRefreshCallRecord)
	ON_MESSAGE(WM_NET_DISCONNECTING,OnDisconnecting)
	ON_MESSAGE(WM_NET_CONNECT_FAIL,OnNetConnectFail)
	ON_MESSAGE(WM_REFRESH_INFOBAR,OnRefreshInfoBar)
	ON_MESSAGE(WM_REFRESH_FAILINFOBAR,OnRefreshFailInfoBar)
	ON_MESSAGE(WM_REFRSH_TELINFOBAR,OnRefreshTelInfoBar)
	ON_MESSAGE(WM_INCOMING_DISPINEDIT,OnDispTelInEdit)
	ON_MESSAGE(WM_UPDATE_FAIL,OnUpdateFail)
	ON_MESSAGE(WM_UPDATE_BEG,OnUpdateBeg)
	ON_MESSAGE(WM_UPDATE_END,OnUpdateEnd)
	ON_MESSAGE(WM_UPDATE_SETPROGRESS,OnUpdateSetprogress)
	ON_MESSAGE(WM_SIMREAD_RESULT,OnSimReadResult)
	ON_MESSAGE(WM_INIT_START,OnInitStart)
	ON_MESSAGE(WM_INIT_END,OnInitEnd)
	ON_MESSAGE(WM_INIT_ERROR,OnInitError)
	ON_MESSAGE(WM_SMS_REFRESHLIST,OnSMSRefreshList)
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CSunnyCatDlg message handlers
void CSunnyCatDlg::OnOK()
{
	return;
}

//声音控制
static void StopPlaySound()
{
	PlaySound(NULL, NULL, SND_ASYNC|SND_NODEFAULT );
}

//显示信息发送成功与失败
LONG CSunnyCatDlg::OnShowSMSSendInfo(WPARAM wParam, LPARAM lParam)
{
	CString strText = "";
	CString strTextFail = "";
	CString sendOk(LoadStringEx(IDS_SUNNYCAT_SENDOK));
	CString sendFail(LoadStringEx(IDS_SUNNYCAT_SENDFAIL));	
	strTextFail.Format(TEXT(":%d "), (int)lParam);
	strText.Format( TEXT(":%d "), (int)wParam);
	sendOk = sendOk +  strText;
	sendFail = sendFail + strTextFail;
	strText = "";
	strText = sendOk + sendFail;
	GMessageBox(strText);
	return 0;
}



BOOL CSunnyCatDlg::OnInitDialog()
{
	CDialog::OnInitDialog();
	TRACE(TEXT(" CSunnyCatDlg::OnInitDialog start \n"));
	/*************************************************************/
	CWnd::MoveWindow(0, 0, 450, 270);

	GetDlgItem(IDC_BUTTON_LINK)->MoveWindow(0, 0, 66, 45);
	GetDlgItem(IDC_BUTTON_NET)->MoveWindow(0, 45, 66, 45);
	GetDlgItem(IDC_BUTTON_CALL)->MoveWindow(0, 90, 66, 45);
	GetDlgItem(IDC_BUTTON_SMS)->MoveWindow(0, 135, 66, 45);
	GetDlgItem(IDC_BUTTON_LINKMAN)->MoveWindow(0, 180, 66, 45);
	GetDlgItem(IDC_MENU_FILE)->MoveWindow(0, 225, 66, 45);
	GetDlgItem(IDC_MENU_FILE)->SetFont(&newFont,TRUE);

	CString strPath = GetAppPath();
	m_LinkBtn.SetBitmapPath(strPath+"skin\\other\\link.bmp");
	m_NetBtn.SetBitmapPath(strPath+"skin\\other\\rate.bmp");
	m_CallBtn.SetBitmapPath(strPath+"skin\\other\\call.bmp");
	m_SMSBtn.SetBitmapPath(strPath+"skin\\other\\sms.bmp");
	m_LinkManBtn.SetBitmapPath(strPath+"skin\\other\\phonebook.bmp");
	m_ctrlFileBtn.SetBitmapPath(strPath+"skin\\other\\setting.bmp");

	GetDlgItem(IDC_MENU_OPERATION)->ShowWindow(SW_HIDE);
	GetDlgItem(IDC_MENU_SETLINKMAN)->ShowWindow(SW_HIDE);
	GetDlgItem(IDC_MENU_HELP)->ShowWindow(SW_HIDE);
	/*************************************************************/	
	InitIconState();
	m_strProduct = GetProduct();
	m_strLanguage = GetLanguage();
	
	GetDlgItem(IDC_MENU_FILE)->SetWindowText(LoadStringEx(IDS_FILE));
	GetDlgItem(IDC_MENU_SETLINKMAN)->SetWindowText(LoadStringEx(IDS_CONTACT));
	GetDlgItem(IDC_MENU_HELP)->SetWindowText(LoadStringEx(IDS_HELP));

	GetDlgItem(IDC_STATIC_1X)->ShowWindow(SW_HIDE);

	SetWindowText(TEXT("WCDMA Modem"));

	CRegKey reg;
    
    CString regPath = GetConfigString();

	if(reg.Open(HKEY_LOCAL_MACHINE, regPath) != ERROR_SUCCESS) //打开成功
	{
		reg.Create(HKEY_LOCAL_MACHINE, regPath);		
		if(reg.Open(HKEY_LOCAL_MACHINE, regPath) != ERROR_SUCCESS)
		{
			return TRUE;
		}
	}
	
	DWORD dwValue = 0;

	if(reg.QueryValue(dwValue, TEXT("MinToTask")) == ERROR_SUCCESS)
	{
		if(dwValue >0 )
			m_bMinmize = TRUE;
		else
			m_bMinmize = FALSE;
	}

	dwValue = 0;
	if(reg.QueryValue(dwValue, TEXT("AutoUpdate")) == ERROR_SUCCESS)
	{
		if(dwValue >0 )
			m_bAutoupdate = TRUE;
		else
			m_bAutoupdate = FALSE;
	}
/*	dwValue = 0;
	if(reg.QueryValue(dwValue, TEXT("VoiceChanel")) == ERROR_SUCCESS)
	{
		if(dwValue == 2 )
			m_bPCMVoice = true;
		else
			m_bPCMVoice = false;
	}
*/
	m_bPCMVoice = true;
	/*******记录是否是首次开启软件*****************
	dwValue = 0;
	if(reg.QueryValue(dwValue, TEXT("FirstRun")) == ERROR_SUCCESS)
	{
		if(dwValue == 0 )
			m_bFirstInsert = true;
		else
			m_bFirstInsert = false;
	}
	// add by lab686 200911010
	/*********************************************************/

	reg.Close();

	InitUI();//建立UI的外观

//	bool b_RasDiscon = m_MobileManager.m_RasClient.GetConnectState();//20070514

	// add by lab686 20091102 
//	bool b_modemexit = ((CSunnyCatDlg*)AfxGetMainWnd())->m_MobileManager.IsModemExisting(GetModemName());
	CString strModemName;
	strModemName.Format(TEXT(""));
	bool b_modemexit = m_MobileManager.IsModemExisting(strModemName);
	
	if(!b_modemexit)
	{
//		EndProgress();
		m_MobileManager.ShutDown();
//		SetTimer(TIMER_MONITOR_MODEM,1000,NULL);	
//		PostMessage(WM_CLOSE,0,0);
	}
	else
	{
//	InitProgress();
	m_InitAT.Start();
	m_InitAT.WaitForComplete();
	}

	::Sleep(1000);
	SetIcon(m_hIcon, TRUE);			// Set big icon
	SetIcon(m_hIcon, FALSE);		// Set small icon
	if(!m_MobileManager.isAbleCom)
	{
		m_connect.EnableWindow(false);
		m_connect.iPhoto = IMAG_DISABLECON;
		m_connect.SetFocus();
		m_unconnect.EnableWindow(false);		
		m_unconnect.iPhoto = IMAG_DISABLECUT;
		m_unconnect.SetFocus();
		/************
		m_connectevdo.EnableWindow(false);
		m_connectevdo.iPhoto = IMAG_DISABLECON;
		m_connectevdo.SetFocus();
		m_unconnectevdo.EnableWindow(false);		
		m_unconnectevdo.iPhoto = IMAG_DISABLECUT;
		m_unconnectevdo.SetFocus();
		/************/
		
		m_bConnected = TRUE;
		m_bUnconnected = TRUE;

	}
	else
	{
		m_connect.EnableWindow(true);
		m_connect.iPhoto = IMAG_CONNECT;
		m_connect.SetFocus();
		m_unconnect.EnableWindow(false);		
		m_unconnect.iPhoto = IMAG_DISABLECUT;
		m_unconnect.SetFocus();
		/************
		m_connectevdo.EnableWindow(true);
		m_connectevdo.iPhoto = IMAG_CONNECT;
		m_connectevdo.SetFocus();
		m_unconnectevdo.EnableWindow(false);		
		m_unconnectevdo.iPhoto = IMAG_DISABLECUT;
		m_unconnectevdo.SetFocus();		
		/************/
		m_bConnected = FALSE;
		m_bUnconnected = TRUE;
	}

	m_connect.SetToolTipTxt(LoadStringEx(IDS_SUNNYCAT_CONNECT));
	m_unconnect.SetToolTipTxt(LoadStringEx(IDS_SUNNYCAT_UNCONNECT));
	/************
	m_connectevdo.SetToolTipTxt(LoadStringEx(IDS_SUNNYCAT_CONNECT));
	m_unconnectevdo.SetToolTipTxt(LoadStringEx(IDS_SUNNYCAT_UNCONNECT));
	/************/
	typedef BOOL (WINAPI* pfWTSRegisterSessionNotification)(HWND,DWORD);
	pfWTSRegisterSessionNotification pfProc = NULL;
	
	m_hMoudle = ::LoadLibrary(TEXT("wtsapi32.dll"));
	pfProc = (pfWTSRegisterSessionNotification)::GetProcAddress(m_hMoudle,"WTSRegisterSessionNotification");
	if(pfProc != NULL)
		pfProc(this->GetSafeHwnd(),NOTIFY_FOR_THIS_SESSION);

	const GUID  PortsGUID = {0x2C7089AA,  0x2E0E, 0x11D1, {0xB1, 0x14, 0x00, 0xC0, 0x4F, 0xC2, 0xAA, 0xE4}};
						
	DEV_BROADCAST_DEVICEINTERFACE   DevInt;   
	memset(&DevInt,0,sizeof(DEV_BROADCAST_DEVICEINTERFACE));   
	DevInt.dbcc_size   =   sizeof(DEV_BROADCAST_DEVICEINTERFACE);   
	DevInt.dbcc_devicetype   =   DBT_DEVTYP_DEVICEINTERFACE;   
	memcpy(&(DevInt.dbcc_classguid),&PortsGUID,sizeof(GUID));
	//DevInt.dbcc_classguid   =   DevGuid;   //设备的   GUID,   不同的设备有不同的   GUID，根据实际情况设定 

	typedef  HDEVNOTIFY(WINAPI* pfRegisterDeviceNotification)(IN HANDLE hRecipient,IN LPVOID NotificationFilter,IN DWORD FlagsRegisterDeviceNotificationA);
	pfRegisterDeviceNotification pfRegNf = NULL;
	m_hRegModule = ::LoadLibrary(TEXT("user32.dll"));
	pfRegNf = (pfRegisterDeviceNotification)::GetProcAddress(m_hRegModule,"RegisterDeviceNotificationA");
	if(pfRegNf != NULL)
	{
		m_RegHandle = pfRegNf(this->GetSafeHwnd(),   &DevInt,   DEVICE_NOTIFY_WINDOW_HANDLE);  
	}

	SetFirstNetRegedit();

	m_removePrompt->ShowWindow(SW_SHOW);
	m_removePrompt->UpdateWindow();
	m_removePrompt->ShowWindow(SW_HIDE);
	SetTimer(TIMER_MONITOR_MODEM, 7500, 0); //设置定时监听硬件	// add by lab686 
	
	TRACE(TEXT(" CSunnyCatDlg::OnInitDialog end \n"));
	return TRUE;  // return TRUE  unless you set the focus to a control
}

void CSunnyCatDlg::InitATSetting(bool hasInitialUI)
{
	m_InitAT.Start();
}

void CSunnyCatDlg::StopPCM()
{
	TRACE(TEXT("CSunnyCatDlg::StopPCM :> ENTER"));
	if(m_pWaveLib != NULL)
	{
		NMWave::ReleaseWaveLib(m_pWaveLib);
		m_pWaveLib = NULL;
	}	
	TRACE(TEXT("CSunnyCatDlg::StopPCM :> END"));
}

void CSunnyCatDlg::StartVoice()
{
	/*
	if(m_pWaveLib != NULL)
	{
		m_pWaveLib->GetAudioInput().Start();
		m_pWaveLib->GetAudioOutPut().Start();
	}
	*/
	TRACE(TEXT("CSunnyCatDlg::StartVoice :> ENTER"));
	bool bOK = m_pWaveLib != NULL;
	TRACE(TEXT("StartVoice:1>%d"),bOK);
	if(bOK) bOK = m_pWaveLib->GetAudioInput().Start();
	TRACE(TEXT("StartVoice:2>%d"),bOK);
	if(bOK) bOK = m_pWaveLib->GetSerialInput().Start();
	TRACE(TEXT("StartVoice:3>%d"),bOK);
	if(bOK) bOK = m_pWaveLib->GetAudioOutPut().Start();
	TRACE(TEXT("StartVoice:4>%d"),bOK);
	if(bOK) bOK = m_pWaveLib->GetSerialOutput().Start();
	TRACE(TEXT("StartVoice:5>%d"),bOK);
	TRACE(TEXT("CSunnyCatDlg::StartVoice :> END"));

}

void CSunnyCatDlg::StopVoice()
{
	/*
	if(m_pWaveLib != NULL)
	{
		m_pWaveLib->GetAudioInput().Stop();
		m_pWaveLib->GetAudioOutPut().Stop();
	}
	*/
	TRACE(TEXT("CSunnyCatDlg::StopVoice :> ENTER"));
	bool bOK = m_pWaveLib != NULL;
	TRACE(TEXT("StopVoice:1>%d"),bOK);
	if(bOK) bOK = m_pWaveLib->GetAudioInput().Stop();
	TRACE(TEXT("StopVoice:2>%d"),bOK);
	if(bOK) bOK = m_pWaveLib->GetSerialInput().Stop();
	TRACE(TEXT("StopVoice:3>%d"),bOK);
	if(bOK) bOK = m_pWaveLib->GetAudioOutPut().Stop();
	TRACE(TEXT("StopVoice:4>%d"),bOK);
	if(bOK) bOK = m_pWaveLib->GetSerialOutput().Stop();
	TRACE(TEXT("StopVoice:5>%d"),bOK);
	TRACE(TEXT("CSunnyCatDlg::StopVoice :> END"));

}

void CSunnyCatDlg::StopRecord()
{
	TRACE(TEXT("CSunnyCatDlg::StopRecord :> ENTER"));
	if(m_pRecorder != NULL)
		m_pRecorder->Close();
	if(m_pRecorderSerial != NULL)
		m_pRecorderSerial->Close();
	TRACE(TEXT("CSunnyCatDlg::StopRecord :> END"));
}

LONG CSunnyCatDlg::OnChangSmsImg(WPARAM wParam, LPARAM lParam)
{
	if(wParam == 1)
	{	m_imgSMSPromt.ShowWindow(SW_SHOW);
		return 1;
	}
	else
	{
		m_imgSMSPromt.ShowWindow(SW_HIDE);
		return 0;
	}
}

//显示信号强度
LONG CSunnyCatDlg::OnShowSignal(WPARAM wParam, LPARAM lParam)
{
	
	CStatic* signalCtrl[6] = 
	{
		&m_s0,
		&m_s1,
		&m_s2,
		&m_s3,
		&m_s4,
		&m_s5
	};

	TRACE(_T("CSunnyCatDlg::OnShowSignal"));

	if(wParam == 99)wParam=0;
    int strength = (int)ceil(wParam / 6.0);
	//if(strength>4)strength=4;
	if(strength>=5)strength=5;
	if(strength<=0)strength=0;
	for(int i=0; i<sizeof(signalCtrl)/sizeof(signalCtrl[0]); ++i)
	{
		TRACE(_T("strength = %d"), strength);
		signalCtrl[i]->ShowWindow(i==strength);
		
	}
    
	return 0;
}

//显示当前网络信息
LONG CSunnyCatDlg::OnShowNetwork(WPARAM wParam, LPARAM lParam)
{
	CRect rc;
	m_sEvdo.GetWindowRect(rc);
	::MapWindowPoints( HWND_DESKTOP, GetSafeHwnd(), (LPPOINT)(RECT*)&rc, 2 );
	InvalidateRect(rc);
	CString m_strCurNet1 = _T("");

	if ( 2 == m_MobileManager.m_CurFormat )
	{
		if ( "46001" == m_MobileManager.m_strCurNet )
		{
			m_strCurNet1 = "CHN-CUGSM";
		}
		else if ( "46000" == m_MobileManager.m_strCurNet )
		{
			m_strCurNet1 = "CHINA  MOBILE";
		}
		else if ( "45403" == m_MobileManager.m_strCurNet )
		{
			m_strCurNet1 = "3 HK";
		}
	
		if ( 2 == m_MobileManager.m_CurACT )
		{
			m_sEvdo.SetWindowText(m_strCurNet1 + "(3G)");
		}
		else if ( 0 == m_MobileManager.m_CurACT )
		{
			m_sEvdo.SetWindowText(m_strCurNet1 + "(2G)");
		}
	}
	else
	{
		if ( 2 == m_MobileManager.m_CurACT )
		{
			m_sEvdo.SetWindowText(m_MobileManager.m_strCurNet + "(3G)");
		}
		else if ( 0 == m_MobileManager.m_CurACT )
		{
			m_sEvdo.SetWindowText(m_MobileManager.m_strCurNet + "(2G)");
		}
	}
	return 0;
}

//判断是否是自动搜网模式
LONG CSunnyCatDlg::OnSetAuto(WPARAM wParam, LPARAM lParam)
{
	if ( wParam == 0 )
	{
		m_autoregister = TRUE;
	}
	else if ( wParam == 1 )
	{
		m_autoregister = FALSE;
	}

	return 0;
}

//显示HDR 信号强度
LONG CSunnyCatDlg::OnShowHDRSignal(WPARAM wParam, LPARAM lParam)
{
//*********************************
	CStatic* signalCtrl[6] = 
	{
		&m_s0_h,
		&m_s1_h,
		&m_s2_h,
		&m_s3_h,
		&m_s4_h,
		&m_s5_h
	};

	TRACE(_T("CSunnyCatDlg::OnShowHDRSignal"));
	int strength;
	if(wParam == 0)strength=0;
	else
	{
    		strength = (int)ceil(wParam / 20.0);
		strength += 1;
	}
	//if(strength>4)strength=4;
	if(strength>=5)strength=5;
	if(strength<=0)strength=0;
	for(int i=0; i<sizeof(signalCtrl)/sizeof(signalCtrl[0]); ++i)
	{
		TRACE(_T("strength = %d"), strength);
		signalCtrl[i]->ShowWindow(i==strength);
	}
   /*************************************/
	return 0;
}

LONG CSunnyCatDlg::OnSetSMSSendState(WPARAM wParam, LPARAM lParam)
{
	TRACE(_T("CSunnyCatDlg::OnSetSMSSendState"));
	if ( 1 == wParam )
	{
		m_bsmssendsucc = true;
		++m_smssendsucc;
		TRACE(TEXT("m_smssendsucc = %d"),m_bsmssendsucc);
	}
	else if (0 == wParam)
	{
		m_bsmssendsucc = false;
		++m_smssendfail;
		TRACE(TEXT("m_smssendfail = %d"),m_bsmssendsucc);
	}
	return 0;
}

//匹配类型
LONG CSunnyCatDlg::OnNetAttachType(WPARAM wParam, LPARAM lParam)
{
	CStatic* regNetType[3] = 
	{
		&m_netGsm,
		&m_netGprs,
		&m_netEdge
	};
	int strength = (int)wParam;
	if(1==strength)
	{
		regNetType[0]->ShowWindow(false);		
		if(m_MobileManager.m_netEdge)
		{
		regNetType[1]->ShowWindow(false);
		regNetType[2]->ShowWindow(true);
		}
		else
		{
			regNetType[1]->ShowWindow(true);
			regNetType[2]->ShowWindow(false);
		}
	}
	else if(2==strength)
	{
		regNetType[0]->ShowWindow(false);
		regNetType[1]->ShowWindow(false);
		regNetType[2]->ShowWindow(true);
	}
	else
	{
		
		if(1 == m_MobileManager.m_netEdge)
		{
			regNetType[0]->ShowWindow(false);
			regNetType[1]->ShowWindow(false);
			regNetType[2]->ShowWindow(true);
		}
		else if(1 == m_MobileManager.m_netGprs)
		{
			regNetType[0]->ShowWindow(false);
			regNetType[1]->ShowWindow(true);
			regNetType[2]->ShowWindow(false);
		}
		else
		{
			if((1==m_MobileManager.NetResgisterStatus())||(5==m_MobileManager.NetResgisterStatus()))
			{
				regNetType[0]->ShowWindow(true);
				regNetType[1]->ShowWindow(false);
				regNetType[2]->ShowWindow(false);
			}
			else
			{
				regNetType[0]->ShowWindow(false);
				regNetType[1]->ShowWindow(false);
				regNetType[2]->ShowWindow(false);
			}
		}
	}
	return 0;
	
}


//注册状态
LONG CSunnyCatDlg::OnNetRegStatusChange(WPARAM wParam, LPARAM lParam)
{
	CStatic* regStatus[3] = 
	{
		&m_notRegister,
		&m_regSuccess,
		&m_roaming
	};
	int strength = (int)wParam;
	if(1==strength)
	{
		regStatus[0]->ShowWindow(false);
		regStatus[1]->ShowWindow(false);
		regStatus[2]->ShowWindow(false);
	}
	else if(5==strength)
	{
		regStatus[0]->ShowWindow(false);
		regStatus[1]->ShowWindow(false);
		regStatus[2]->ShowWindow(true);
	}
	else
	{
		regStatus[0]->ShowWindow(false);
		regStatus[1]->ShowWindow(false);
		regStatus[2]->ShowWindow(false);
	}
	return 0;
	
}

//收到新短信
LONG CSunnyCatDlg::OnSMSNew(WPARAM wParam, LPARAM count)
{
	TRACE(TEXT("CSunnyCatDlg::OnSMSNew :> SHOW NEW SMS WINDOW\n"));
	//CRect rc;
	//m_lableBtnsystemTimeShow.GetWindowRect(rc);
	//ScreenToClient(rc);
	//CRect rcSMS(rc.left-16,rc.top, rc.left-4, rc.bottom);
	//m_imgSMSPromt.MoveWindow(rcSMS);

	m_imgSMSPromt.ShowWindow(SW_SHOW);
	CString strInfo;

	///播放声音
	PlaySMSRing();
    
	///弹出提示窗口
	CDlgMsg& msgWindow = *CDlgMsg::CreateMsgDlg(GetFont());
	msgWindow.CreateMsgWindow();

	strInfo = LoadStringEx(IDS_NEWSMS);
	strInfo += "\r\n";
	
	if(!m_NewSMSNum.IsEmpty())
		strInfo += m_NewSMSNum;
	strInfo += "\r\n";
	
	if(!m_NewSMSText.IsEmpty())
	{
		if (m_NewSMSText.GetLength() < 10)
			strInfo += m_NewSMSText;
		else
		{
			strInfo += m_NewSMSText.Left(10);
			strInfo += "...";
		}
	}

	msgWindow.SetPromptMessage(strInfo);

	strInfo = LoadStringEx(IDS_USERINFO);
	msgWindow.SetPromptCaption(strInfo);

	CRect rect;
	msgWindow.GetWindowRect(&rect);
	msgWindow.SetWindowPos(&wndTopMost, 
		rect.left, rect.top, 
		rect.Width(), rect.Height(), 
		SWP_NOMOVE | SWP_NOSIZE | SWP_SHOWWINDOW);

	msgWindow.ShowWindow(SW_SHOW);

	msgWindow.UpdateWindow();

//	if(m_LockStatus != LOCKTYPE_PHONE)
//		ShowPage(PAGE_READSMS);	
	m_NewSMSNum.Empty();
	m_NewSMSNum.Empty();
	return 0;
}



void CSunnyCatDlg::InitUI()
{
//	if(m_strLanguage == "-En")
//	{
		CRect rRect;
	
		m_OperationBtn.GetWindowRect(rRect);
		m_OperationBtn.MoveWindow(rRect.left-5,2,rRect.Width()+2,rRect.Height());
		m_ctrlContactBtn.GetWindowRect(rRect);
		m_ctrlContactBtn.MoveWindow(rRect.left-3,2,rRect.Width(),rRect.Height());
		GetDlgItem(IDC_MENU_HELP)->GetWindowRect(rRect);
		GetDlgItem(IDC_MENU_HELP)->MoveWindow(rRect.left-3,2,rRect.Width(),rRect.Height());

//	}
	CString appPath = GetAppPath();
	CString filePath("skin\\neostyle\\theme.ini");
 	CString path =appPath+filePath;

	InitDlg();
//	设置皮肤
	//m_ctrlInfoBar.SetLeftAlign();20070507
//	m_ctrlInfoBar.SetTextColor(RGB(100,120,100));

//	SetSkin(path);
	//SetSkin("skin\\neostyle\\theme.ini");
	//SetSkin("skin\\xpstyle\\theme.ini");
/*	CRect rc;
	GetWindowRect(&rc);
	rc -= rc.TopLeft(); //normalize
	m_rgn.CreateRoundRectRgn(rc.left,rc.top,rc.right,rc.bottom, 30,30);
	SetWindowRgn(m_rgn,TRUE);
*/
	CMenu*   pMenu   =   AfxGetMainWnd()->GetSystemMenu(FALSE);   
	pMenu->DeleteMenu(SC_MAXIMIZE,   MF_BYCOMMAND|MF_GRAYED);   
	pMenu->DeleteMenu(SC_MOVE,MF_BYCOMMAND|MF_GRAYED);   
	pMenu->DeleteMenu(SC_SIZE,MF_BYCOMMAND|MF_GRAYED);   


#if 0
	CSize size;
	size.cx = 66;
	size.cy = 30;
	m_table.SetRedraw();
	m_table.SetItemSize(size);
	

//	LoadImgList(g_pSysImageList, IDB_LINKMANTREE);
	CBitmap IconBitmap;
//	IconBitmap.LoadBitmap(IDB_TOOLBAR);

	if(m_strLanguage == "-En")
		IconBitmap.Attach(LoadBmpFormFile("skin//English//TOOLBAR.bmp"));
	else if(m_strLanguage == "-Ch")
		IconBitmap.Attach(LoadBmpFormFile("skin//Chinese//TOOLBAR.bmp"));

	

//	g_pSysImageList->Create(20,20,ILC_MASK|ILC_COLOR24,1,1) ;
	g_pSysImageList->Create(66,30,ILC_MASK|ILC_COLOR24,1,1) ;
	g_pSysImageList->Add( &IconBitmap,(COLORREF)0xFFFFFF );
	IconBitmap.DeleteObject();
	m_table.SetItemState(1, TCIF_IMAGE,TCIS_HIGHLIGHTED );
	m_table.SetImageList(g_pSysImageList);
	m_callDlg.SetParenWnd(this);
	
	m_table.AddPage("", &m_netDlg, IDD_DIALOG_NETWORK);
	m_table.AddPage("", &m_callDlg, IDD_DIALOG_CALL);
	m_table.AddPage("", &m_smsDlg, IDD_DIALOG_SMS);
	m_table.AddPage("", &m_linkManDlg, IDD_DIALOG_LINKMAN);
	m_table.AddPage("", &m_recordDlg, IDD_DIALOG_RECORD);
#endif	
	
//	m_table.AddPage("   其它   ", &m_LinkDlg, IDD_DIALOG_OTHER);
	SetSkin(path);
//	m_table.Show();	
	CreateOperationMenu();
	m_ctrlInfoBar.SetLeftAlign(TRUE);
	//=============================
//	FlashInfoBar("");20070507
	TaskBarAddIcon();

	m_OperatonMenu.CheckMenuItem(IDM_OPREATION_LINK,MF_CHECKED);
	m_OperatonMenu.CheckMenuItem(IDM_OPERATION_NETWORK,MF_UNCHECKED);
	m_OperatonMenu.CheckMenuItem(IDM_OPERATION_CALL,MF_UNCHECKED);
	m_OperatonMenu.CheckMenuItem(IDM_OPERATION_SMS,MF_UNCHECKED);
	m_OperatonMenu.CheckMenuItem(IDM_OPERATION_PB,MF_UNCHECKED);
}


void CSunnyCatDlg::OnPaint() 
{
	CPaintDC dc(this); 
	CRect rect;
	GetClientRect(&rect);
	if (IsIconic())
	{
		// device context for painting

		SendMessage(WM_ICONERASEBKGND, (WPARAM) dc.GetSafeHdc(), 0);

		// Center icon in client rectangle
		int cxIcon = GetSystemMetrics(SM_CXICON);
		int cyIcon = GetSystemMetrics(SM_CYICON);
	
		int x = (rect.Width() - cxIcon + 1) / 2;
		int y = (rect.Height() - cyIcon + 1) / 2;

		// Draw the icon
		dc.DrawIcon(x, y, m_hIcon);
	}
	else
	{
		
		//dc.FillSolidRect(rect, RGB(228,238,242));


		if(m_bMinmize)
		{
			ShowWindow(SW_MINIMIZE);
			m_bMinmize = FALSE;
		}
	//	m_connect.Invalidate();
	//	m_unconnect.Invalidate();
		PaintMenuBk(dc);
		PaintTop(dc);
		PaintBottom(dc);
		
		CDialog::OnPaint();
	}

	CRect rc;
 	m_ctrlInfoBar.GetWindowRect(rc);
 	::MapWindowPoints( HWND_DESKTOP, GetSafeHwnd(), (LPPOINT)(RECT*)&rc, 2 ); 
	dc.SetBkMode(TRANSPARENT);
	dc.SetTextColor(RGB(255,255,255));

	CRect rc_1;
 	m_sEvdo.GetWindowRect(rc_1);
 	::MapWindowPoints( HWND_DESKTOP, GetSafeHwnd(), (LPPOINT)(RECT*)&rc_1, 2 ); 
	dc.SetBkMode(TRANSPARENT);
	dc.SetTextColor(RGB(255,255,255));

	CFont* oldFont = dc.SelectObject(&newFont);	
//	dc.DrawText(strShowText,&rc,DT_LEFT|DT_VCENTER| DT_END_ELLIPSIS);
	dc.TextOut(rc.left,rc.top+7,strShowText);
	dc.SelectObject(oldFont);	
	bShow = false;

}

void CSunnyCatDlg::PaintMenuBk(CPaintDC& dc)
{
	CBitmap BkBmp;
	CString strAppPath = GetAppPath();

	strAppPath += "skin\\PhoneSkins\\";

	BkBmp.Attach(LoadBmpFormFile(strAppPath + "menubk.bmp"));
	CRect rcBtn;
	m_ctrlFileBtn.GetWindowRect(rcBtn);
	::MapWindowPoints( HWND_DESKTOP, GetSafeHwnd(), (LPPOINT)(RECT*)&rcBtn, 2 );
	if(BkBmp.GetSafeHandle())
	{
		CRect rc;

		rc.top = rcBtn.top -2;
		rc.bottom = rc.bottom+2;
		rc.left = 0;
		rc.right = 200;
		CDC memDC;
		memDC.CreateCompatibleDC( &dc );//
		memDC.SelectObject( &BkBmp );
		CRect rcClient;
		GetClientRect(rcClient);
		BOOL bOk = dc.StretchBlt(rc.left,rc.top,rcClient.Width(),57,&memDC,0,0,200,57,SRCCOPY);
		
		BkBmp.DeleteObject() ;
		memDC.DeleteDC() ;
		ReleaseDC(&memDC) ;
	}
}

void CSunnyCatDlg::PaintTop(CPaintDC& dc)
{
	CBitmap BkBmp;
	CString strAppPath = GetAppPath();

	strAppPath += "skin\\PhoneSkins\\";

	BkBmp.Attach(LoadBmpFormFile(strAppPath + TEXT("toolbar.bmp")));
	CRect rcBtn;
	m_ctrlFileBtn.GetWindowRect(rcBtn);
	::MapWindowPoints( HWND_DESKTOP, GetSafeHwnd(), (LPPOINT)(RECT*)&rcBtn, 2 );
	if(BkBmp.GetSafeHandle())
	{
		CRect rc;

		rc.top = rcBtn.bottom +1;
		rc.bottom = rc.top+57;
		rc.left = 0;
		rc.right = 200;
		CDC memDC;
		memDC.CreateCompatibleDC( &dc );//
		memDC.SelectObject( &BkBmp );
		CRect rcClient;
		GetClientRect(rcClient);
		BOOL bOk = dc.StretchBlt(rc.left,rc.top,rcClient.Width(),57,&memDC,0,0,200,57,SRCCOPY);
		
		BkBmp.DeleteObject() ;
		memDC.DeleteDC() ;
		ReleaseDC(&memDC) ;
	}
}

void CSunnyCatDlg::PaintBottom(CPaintDC& dc)
{
	CBitmap BkBmp;
	CString strAppPath = GetAppPath();

	strAppPath += "skin\\other\\";

	BkBmp.Attach(LoadBmpFormFile(strAppPath + "back_bottom.bmp"));
	CRect rcBtn;
	m_ctrlFileBtn.GetWindowRect(rcBtn);
	::MapWindowPoints( HWND_DESKTOP, GetSafeHwnd(), (LPPOINT)(RECT*)&rcBtn, 2 );
	if(BkBmp.GetSafeHandle())
	{
		CRect rc;

		rc.top = rcBtn.bottom +1;
		rc.bottom = rc.top+57;
		rc.left = 0;
		rc.right = 200;
		CDC memDC;
		memDC.CreateCompatibleDC( &dc );//
		memDC.SelectObject( &BkBmp );
		CRect rcClient;
		GetClientRect(rcClient);
		
		dc.StretchBlt(rc.left,rcClient.bottom-41,rcClient.Width(),41,&memDC,0,0,170,41,SRCCOPY);
		BkBmp.DeleteObject() ;
		memDC.DeleteDC() ;
		ReleaseDC(&memDC) ;
	}
}
// The system calls this to obtain the cursor to display while the user drags
//  the minimized window.
HCURSOR CSunnyCatDlg::OnQueryDragIcon()
{
	return (HCURSOR) m_hIcon;
}


void CSunnyCatDlg::OnLButtonDown(UINT nFlags, CPoint point) 
{
	// TODO: Add your message handler code here and/or call default
	PostMessage(WM_NCLBUTTONDOWN, HTCAPTION, MAKELPARAM(point.x, point.y));	
	CDialog::OnLButtonDown(nFlags, point);
}

void CSunnyCatDlg::OnCallComing(CCallManager* pSender)
{
	//不能在callback函数里获取CDMA时间，所以这里需要异步处理
	//PostMessage(WM_CALLTIMEBEGIN, 0, 0 );
	
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
	
	DWORD dwValue = 0;
	BOOL bAutoShowCall = FALSE;

	if(reg.QueryValue(dwValue, TEXT("AutoShowWindow")) == ERROR_SUCCESS)
	{
		if(dwValue >0 )
			bAutoShowCall = TRUE;
		else
			bAutoShowCall = FALSE;
	}

	if(bAutoShowCall)
	{
		ShowWindow(SW_SHOWNORMAL);
		ShowPage(2);
	}
	
	//CWnd* pWnd = m_callDlg.GetDlgItem(IDC_EDIT_DAIL_NUM);
	//pWnd->SetWindowText(pSender->GetCallInfo().strTelephone);
	reg.Close();
	PostMessage(WM_INCOMING_DISPINEDIT,0,0);
	PostMessage(WM_DIAL_SHOW, 
		(WPARAM)(&pSender->GetCallInfo().strTelephone),	
		0);
}

/*
来电显示
*/
LONG CSunnyCatDlg::OnDialShow(WPARAM wParam, LPARAM lParam)
{

	const CString* strPhone;
	CString strName;
	int nID;
	
	strName.Empty();
	strPhone = (const CString*)wParam;
	FindNameInPhonebook(*strPhone,strName,nID);
	PlayIncomeRing(nID,FALSE);

//	AfxMessageBox(*strPhone);

	if(strName.IsEmpty())
		FlashInfoBar(*strPhone);
	else
		FlashInfoBar(strName);

	
	ShowCallPromptWindow(*strPhone,strName);

	return 0;
}

void CSunnyCatDlg::PlayIncomeRing(int nID,bool bRepeat)
{
	CPhonebookPCClass PBClass;
	size_t nSize;
	int i = 0;
	TPBClass item;
	CString strVoicePath;
	strVoicePath.Empty();

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
	
	TCHAR szValue[1024] = {0};
	DWORD nCount = sizeof(szValue)-1;;
	if(reg.QueryValue(szValue, TEXT("CallRing"), & nCount) == ERROR_SUCCESS)
	{
		strVoicePath = szValue;
	}
	if(strVoicePath.IsEmpty())
	{
		strVoicePath = GetAppPath() + "music\\ring.wav";
	}
	
	PBClass.GetAllRecords();
	nSize = PBClass.Size();

	for(i=0 ; i<nSize; i++)
	{
		item = PBClass.GetAt(i);
		if(nID == item.nID)
		{
			strVoicePath = item.strVoicePath;
			break;
		}
	}
	if(strVoicePath.IsEmpty())
	{
		strVoicePath = GetAppPath() + "music\\ring.wav";
	}
	reg.Close();
	PlaySound(strVoicePath, NULL, SND_ASYNC|SND_LOOP);
}

BOOL CSunnyCatDlg::FindNameInPhonebook(CString strPhone,CString& strName,int& nID)
{
	CPhonebookPC phPC;
	size_t nSize = 0;
	int i = 0;
	BOOL bOK = FALSE;

	if(strPhone.GetLength() == 0)
		return bOK;

	nID = -1;

	phPC.GetAllRecords();
	nSize = phPC.Size();

	for(i=0; i<nSize; i++)
	{
		const TPhoneBookItemEx item = phPC.GetAt(i);
		if(strPhone == item.strMobilePhone)
		{
			strName = item.strName;
			nID = item.nParentID;
			bOK = TRUE;
			break;
		}
		if(strPhone == item.strHomePhone)
		{
			strName = item.strName;
			nID = item.nParentID;
			bOK = TRUE;
			break;
		}
		
		if(strPhone == item.strOfficePhone)
		{
			strName = item.strName;
			nID = item.nParentID;
			bOK = TRUE;
			break;
		}
		
	}

	if(i < nSize)
		return bOK;

	nSize = m_linkManDlg.m_pbookEmbed.Size();
	for(i=0; i<nSize; i++)
	{
		TPhoneBookItemEmbed* item = (TPhoneBookItemEmbed*)&m_linkManDlg.m_pbookEmbed.GetAt(i);
		if(strPhone == item->strMobilePhone)
		{
			strName = item->strName;
			bOK = TRUE;
			break;
		}
	}
	return bOK;
}

void CSunnyCatDlg::ShowCallPromptWindow(const CString& strPhone, const CString& strName,bool bRepeatSound)
{	
	//StopPlaySound();
	 
	//FindNameInPhonebook(strPhone,strName,strClassName);
	
	//PlayIncomeRing(strClassName,bRepeatSound);

	///弹出提示窗口
	CDlgMsg& msgWindow = *CDlgMsg::CreateMsgDlg(GetFont());

	msgWindow.CreateMsgWindow();

	CString strInfo;
	strInfo = LoadStringEx(IDS_CALL_INCOME);
	strInfo += "\r\n";
	
	if(!strName.IsEmpty())
		strInfo += strName;
	else
		strInfo += strPhone;

	msgWindow.SetPromptMessage(strInfo);

	strInfo = LoadStringEx(IDS_USERINFO);
	msgWindow.SetPromptCaption(strInfo);

	CRect rect;
	msgWindow.GetWindowRect(&rect);
	msgWindow.SetWindowPos(&wndTopMost, 
		rect.left, rect.top, 
		rect.Width(), rect.Height(), 
		SWP_NOMOVE | SWP_NOSIZE | SWP_SHOWWINDOW);
	msgWindow.ShowWindow(SW_SHOW);

	msgWindow.UpdateWindow();
}
///====================================================================================
///								界面皮肤
///====================================================================================


#define     GetWindowStyle(hwnd)    ((DWORD)GetWindowLong(hwnd, GWL_STYLE))

BOOL CALLBACK EnumChildProc(  HWND hwnd,      // handle to child window
							LPARAM lParam   // application-defined value
							)
{
	TCHAR classname[200];
	CSunnyCatDlg *dlg = (CSunnyCatDlg *)lParam;
	//CSunnyCatDlg *dlg = (CSunnyCatDlg *)AfxGetMainWnd();
	DWORD style;
		
	GetClassName( hwnd, classname, 200 );
	style = GetWindowStyle( hwnd );
	if ( _tcscmp( classname, TEXT("Button") ) == 0 )
	{
		style = (UINT)GetWindowLong(hwnd, GWL_STYLE) & 0xff;
		if ( style == BS_PUSHBUTTON || style == BS_DEFPUSHBUTTON )
			dlg->SubClassButton( hwnd );
			
		
		
	}


	return TRUE;
}

BOOL CSunnyCatDlg::SubClassButton( HWND hwnd )
{
	//CSkinButton *btn = new CSkinButton();
	static int i = 0;
/*	btn[i] = new CSkinButton();
	
	CWnd* pWnd = CWnd::FromHandlePermanent(hwnd);
	if ( pWnd == NULL)
	{
		btn[i]->SubclassWindow( hwnd );
		btn[i]->SetResource( &m_btnres );
	    i++;
		return TRUE;

	}*/
	i++;
	return FALSE;
}

BOOL CSunnyCatDlg::SetSkin(CString skinPath)
{
	
	m_skinWin.LoadSkin( ToMultiBytes(LPCTSTR(skinPath)).c_str() );
	m_btnres.LoadSkin( ToMultiBytes(LPCTSTR(skinPath)).c_str() );
	if ( m_bFirst )
	{
		m_skinWin.InstallSkin( this );
	//	EnumChildWindows( m_hWnd, EnumChildProc, (LPARAM)this );

		m_bFirst = FALSE;
	}
	SetWindowPos( 0, 0, 0, 0, 0, SWP_NOSIZE | SWP_NOMOVE |SWP_FRAMECHANGED );
	return TRUE;
}
///====================================================================================
///====================================================================================



void CSunnyCatDlg::OnMenuFile() 
{
	// TODO: Add your control notification handler code here
		CPoint pt ;	
		CMenu CardMenu, *pPopMenu = NULL ;
		GetCursorPos(&pt) ;

		if(m_strLanguage == "-Ru")
		{
			BOOL bLoad = CardMenu.LoadMenu(IDR_MENU_CH);
		}
		else if (m_strLanguage == "-En")
		{
			BOOL bLoad = CardMenu.LoadMenu(IDR_MENU);
		}
		
		pPopMenu = CardMenu.GetSubMenu(0) ;
		if(pPopMenu != NULL)
		{
//			pPopMenu->ModifyMenu(IDR_MENU_CONNECT, MF_BYCOMMAND|MF_STRING, IDR_MENU_CONNECT , LoadStringEx(IDS_MENU_CONNECT));
//			pPopMenu->ModifyMenu(ID_MENU_UNCONNECT, MF_BYCOMMAND|MF_STRING, ID_MENU_UNCONNECT , LoadStringEx(IDS_MENU_UNCONNECT));
//			pPopMenu->ModifyMenu(IDR_NETWORKTYPE_CDMA, MF_BYCOMMAND|MF_STRING, IDR_NETWORKTYPE_CDMA , LoadStringEx(IDS_MENU_CDMA));
//			pPopMenu->ModifyMenu(IDR_NETWORKTYPE_HDR, MF_BYCOMMAND|MF_STRING, IDR_NETWORKTYPE_HDR , LoadStringEx(IDS_MENU_HDR));
//			pPopMenu->ModifyMenu(IDR_NETWORKTYPE_HYBRID, MF_BYCOMMAND|MF_STRING, IDR_NETWORKTYPE_HYBRID , LoadStringEx(IDS_MENU_HYBRID));
			pPopMenu->ModifyMenu(IDR_MENU_SET, MF_BYCOMMAND|MF_STRING, IDR_MENU_SET , LoadStringEx(IDS_MENU_SET));
			pPopMenu->ModifyMenu(IDR_MENU_NETWORK, MF_BYCOMMAND|MF_STRING, IDR_MENU_NETWORK , LoadStringEx(IDS_MENU_SELNETWORK));
			pPopMenu->ModifyMenu(IDR_LINKMAN_INPUT, MF_BYCOMMAND|MF_STRING, IDR_LINKMAN_INPUT , LoadStringEx(IDS_LINKMAN_INPUT));
			pPopMenu->ModifyMenu(IDR_LINKMAN_OUTPUT, MF_BYCOMMAND|MF_STRING, IDR_LINKMAN_OUTPUT , LoadStringEx(IDS_LINKMAN_OUTPUT));
			pPopMenu->ModifyMenu(IDR_PINOPERATION_ENABLE, MF_BYCOMMAND|MF_STRING, IDR_PINOPERATION_ENABLE , LoadStringEx(IDS_PINENABLE));
			pPopMenu->ModifyMenu(IDR_PINOPERATION_DISABLE, MF_BYCOMMAND|MF_STRING, IDR_PINOPERATION_DISABLE , LoadStringEx(IDS_PINDISABLE));
			pPopMenu->ModifyMenu(IDR_MENU_HELPINFO, MF_BYCOMMAND|MF_STRING, IDR_MENU_HELPINFO , LoadStringEx(IDS_MENU_HELPINFO));
			pPopMenu->ModifyMenu(ID_MENU_ABOUT, MF_BYCOMMAND|MF_STRING, ID_MENU_ABOUT , LoadStringEx(IDS_MENU_ABOUT));
			pPopMenu->ModifyMenu(IDR_MENU_CLOSE, MF_BYCOMMAND|MF_STRING, IDR_MENU_CLOSE , LoadStringEx(IDS_MENU_CLOSE));

//			pPopMenu->CheckMenuItem(IDR_PINOPERATION_ENABLE,MF_CHECKED);
//			pPopMenu->CheckMenuItem(IDR_PINOPERATION_DISABLE,MF_UNCHECKED);
			if (!m_MobileManager.m_simLocked)
			{
				pPopMenu->EnableMenuItem(IDR_PINOPERATION_ENABLE,MF_BYCOMMAND|MF_ENABLED) ;
				pPopMenu->EnableMenuItem(IDR_PINOPERATION_DISABLE,MF_BYCOMMAND|MF_GRAYED) ;
			}
			else
			{
				pPopMenu->EnableMenuItem(IDR_PINOPERATION_ENABLE,MF_BYCOMMAND|MF_GRAYED) ;
				pPopMenu->EnableMenuItem(IDR_PINOPERATION_DISABLE,MF_BYCOMMAND|MF_ENABLED) ;
			}
			if ( !m_MobileManager.isAbleCom )
			{
				pPopMenu->EnableMenuItem(IDR_PINOPERATION_ENABLE,MF_BYCOMMAND|MF_GRAYED) ;
				pPopMenu->EnableMenuItem(IDR_PINOPERATION_DISABLE,MF_BYCOMMAND|MF_GRAYED) ;
			}
/*			if(!m_bConnected)
			{
//				pPopMenu->EnableMenuItem(1,MF_BYPOSITION|MF_GRAYED) ;
			}
			else if(!m_bUnconnected)
			{
				pPopMenu->EnableMenuItem(0,MF_BYPOSITION|MF_GRAYED) ;
			}
			else
			{
				pPopMenu->EnableMenuItem(0,MF_BYPOSITION|MF_GRAYED) ;
//				pPopMenu->EnableMenuItem(1,MF_BYPOSITION|MF_GRAYED) ;
			}
*/
			CRect rc;
			m_ctrlFileBtn.GetWindowRect(rc);
			pPopMenu->TrackPopupMenu(TPM_LEFTBUTTON,rc.left,rc.bottom,this);
		//	pPopMenu->TrackPopupMenu(TPM_LEFTBUTTON,pt.x-30,pt.y+15,this) ;
		}
}

void CSunnyCatDlg::PlaySMSRing()
{
	CRegKey reg;
    
    CString regPath = GetConfigString();
	CString strVoicePath;

	if(reg.Open(HKEY_LOCAL_MACHINE, regPath) != ERROR_SUCCESS) //打开成功
	{
		reg.Create(HKEY_LOCAL_MACHINE, regPath);		
		if(reg.Open(HKEY_LOCAL_MACHINE, regPath) != ERROR_SUCCESS)
		{
			return ;
		}
	}
	
	TCHAR szValue[1024] = {0};
	DWORD nCount = sizeof(szValue)-1;;
	if(reg.QueryValue(szValue, TEXT("SMSRing"), & nCount) == ERROR_SUCCESS)
	{
		strVoicePath = szValue;
	}
	
	if(strVoicePath.IsEmpty())
	{
		strVoicePath = GetAppPath() + "music\\sms.wav";
	}

	PlaySound(strVoicePath, NULL, SND_ASYNC|SND_NODEFAULT );
	reg.Close();
}

void CSunnyCatDlg::OnMenuHelp() 
{
	// TODO: Add your control notification handler code here
		CPoint pt ;	
		CMenu CardMenu, *pPopMenu = NULL ;
		GetCursorPos(&pt) ;
		CardMenu.LoadMenu(IDR_MENU);
		pPopMenu = CardMenu.GetSubMenu(2) ;		
		if(pPopMenu != NULL)
		{			
	//		pPopMenu->ModifyMenu(IDM_AUTO_UPDATE, MF_BYCOMMAND|MF_STRING, IDM_AUTO_UPDATE , LoadStringEx(IDS_AUTO_UPDATE));
			if(m_strLanguage == "-Ru")
			pPopMenu->ModifyMenu(IDR_MENU_HELPINFO, MF_BYCOMMAND|MF_STRING, IDR_MENU_HELPINFO , LoadStringEx(IDS_MENU_HELPINFO));
			else if(m_strLanguage == "-En")
				pPopMenu->DeleteMenu(0,MF_BYPOSITION);
			pPopMenu->ModifyMenu(ID_MENU_ABOUT, MF_BYCOMMAND|MF_STRING, ID_MENU_ABOUT , LoadStringEx(IDS_MENU_ABOUT));					
			
			CRect rc;
			m_ctrlHelpBtn.GetWindowRect(rc);
			pPopMenu->TrackPopupMenu(TPM_LEFTBUTTON,rc.left,rc.bottom,this);
		//	pPopMenu->TrackPopupMenu(TPM_LEFTBUTTON,pt.x-30,pt.y+15,this) ;
		}
}

void CSunnyCatDlg::OnSelchangeTab(NMHDR* pNMHDR, LRESULT* pResult) 
{
	// TODO: Add your control notification handler code here
/*	if(m_table.GetCurSel() == 1)
	{
		m_MobileManager.SMSEnum();
	}
	*/
//	m_table.SetBkColor(RGB(0,0,0));
	*pResult = 0;
}

void CSunnyCatDlg::OnKeyDown(UINT nChar, UINT nRepCnt, UINT nFlags) 
{
	// TODO: Add your message handler code here and/or call default
	CDialog::OnKeyDown(nChar, nRepCnt, nFlags);
}

void CSunnyCatDlg::ShowOnLine(bool bShow)
{
	m_s1.ShowWindow(false);
	m_s2.ShowWindow(false);
	m_s3.ShowWindow(false);
	m_s4.ShowWindow(false);
	m_s5.ShowWindow(false);
	m_s0.ShowWindow(false);
	m_s0_h.ShowWindow(false);
	m_s2_h.ShowWindow(false);
	m_s3_h.ShowWindow(false);
	m_s4_h.ShowWindow(false);
	m_s5_h.ShowWindow(false);
	m_s1_h.ShowWindow(false);
	
	m_staOnLine.ShowWindow(bShow);
	m_NotOnLine.ShowWindow(false);
}

/*
void CSunnyCatDlg::OnMenuSwitch() 
{
	// TODO: Add your command handler code here

}
*/

//正在断开网络
LRESULT CSunnyCatDlg::OnDisconnecting(WPARAM wParam,LPARAM lParam)
{
	TRACE(TEXT("CSunnyCatDlg::OnDisconnecting :> ENTER\n"));
	KillTimer(TIMER_FLUX_SPEED);
	KillTimer(TIMER_NET_POWER);
	TRACE(TEXT("CSunnyCatDlg::OnDisconnecting :>  WINEXEC POWERCFG.EXE\n"));
	WinExec("Powercfg.exe /hibernate on",SW_HIDE); 
	CString str;
	str=LoadStringEx(IDS_CONNECT_DISCONNECTING);

	TRACE(TEXT("CSunnyCatDlg::OnDisconnecting :> END\n"));
	DispTextInfobar(str);

	return 1;
}

LRESULT CSunnyCatDlg::OnNetConnectFail(WPARAM wParam,LPARAM lParam)
{
	DispTextInfobar(LoadStringEx(IDS_CONNECT_FAIL));
	::Sleep(1000);

	return 1;
}

//正在连接网络
LONG CSunnyCatDlg::OnNetConnecting(WPARAM wParam, LPARAM lParam)
{
	RASCONNSTATE rasstate = (RASCONNSTATE)lParam;

	CString strInfo = CRasClient::GetStatusString(rasstate);

	DispTextInfobar(strInfo);

	if(rasstate == RASCS_Connected)
	{
		ShowOnLine(true);
		OnNetConnected(0,0);
	}
    return 0;
}

//网络已经连接
LONG CSunnyCatDlg::OnNetConnected(WPARAM wParam, LPARAM lParam)
{	
	SetTimer(TIMER_FLUX_SPEED, 1000, 0);
	SetTimer(TIMER_NET_POWER,60000,0);
	OnConnectionStateChange(NET_CONNECTED);

	m_connect.EnableWindow(FALSE);
	m_unconnect.EnableWindow(TRUE);
	m_connect.iPhoto = IMAG_DISABLECON;
	m_connect.SetFocus();
	m_unconnect.iPhoto = IMAG_DISCONNECT;
	m_unconnect.SetFocus();
	
	m_bConnected = TRUE;
	m_bUnconnected = FALSE;
	m_netDlg.PostMessage(WM_NETWORK_TIME, 1, 1);
//	m_MobileManager.SetConnectionStat(CONNECT_CONTROL_CONNECT);
	m_LinkDlg.SetBtnStatus(STATUS_DISCONNECT);

	TRACE(TEXT("CSunnyCatDlg::OnNetConnected:> Net is Connecting!!"));

	m_bCntSuccess = true;

	return 0;
}

void CSunnyCatDlg::OnButtonConnect()
{
	///判断是否已插卡
	if(!m_MobileManager.isAbleCom)
	{
		GMessageBox(LoadStringEx(IDS_NO_SC700_CARD));
		return;
	}
	
	/************************************************************
	///判断是否正在打电话
	if(m_MobileManager.m_pCallMgr->GetCallStatus() != CALLSTATE_IDLE)
	{
		GMessageBox(LoadStringEx(IDS_PROMPT_CALL));
		return;
	}
	/************************************************************/
    
	if(CONNECT_CONTROL_NOT_CONNECT != m_MobileManager.GetConnectionStat())
	{
		return;
	}

	DispTextInfobar(LoadStringEx(IDS_PREPARE_NETCONNECT));

	m_connect.EnableWindow(FALSE);
	m_connect.iPhoto = IMAG_DISABLECON;			
	m_connect.SetFocus();
	m_unconnect.EnableWindow(FALSE);
	m_unconnect.iPhoto = IMAG_DISABLECUT;
	m_unconnect.SetFocus();		
	m_bConnected = TRUE;
	m_bUnconnected = TRUE;

	
	if(m_MobileManager.GetConnectionStat() == CONNECT_CONTROL_NOT_CONNECT)
	{
		m_bFluxUncntDoModal = FALSE;
		m_bFluxWarnDoModal = FALSE;
		m_bWarnNMdlg = FALSE;

		
		if(m_MobileManager.NetworkConnect())
		{
			OnMsgStatusChange(NET_CONNECTING, 0);//直接将状态改过来，而不是PostMessage
			m_isConnect = true;	
			m_bCntSuccess = false;
		}
	}
}

void CSunnyCatDlg::ConnectNet() 
{
	// TODO: Add your control notification handler code here
	if (NETTYPE_EVDO == m_linktype)
	{
		GMessageBox(LoadStringEx(IDS_MSG_NETLINK), TEXT(""), MB_OKCANCEL);
		return ;
	}
	m_linktype = NETTYPE_ONEX;
//	ConnectNet();
}

//网络已经断开
LONG CSunnyCatDlg::OnNetDisconnected(WPARAM wParam, LPARAM lParam)
{
	TRACE(TEXT("CSunnyCatDlg::OnNetDisconnected :> ENTER\n"));
	m_dwConnectSecond = 0;
	m_bConnected = FALSE;
	m_bUnconnected = TRUE;
	
	///显示网络断开信息
	CRect rc;
	
	DispTextInfobar(LoadStringEx(IDS_CONNECT_OFFLINE));
	m_MobileManager.m_bSMSLoaded = false;
	::Sleep(1000);

	m_staOnLine.ShowWindow(false);
	m_NotOnLine.ShowWindow(true);
/*
	m_s0.ShowWindow(true);
	m_s1.ShowWindow(false);
	m_s2.ShowWindow(false);
	m_s3.ShowWindow(false);
	m_s4.ShowWindow(false);
	m_s5.ShowWindow(false);
	m_s0_h.ShowWindow(true);
	m_s1_h.ShowWindow(false);
	m_s2_h.ShowWindow(false);
	m_s3_h.ShowWindow(false);
	m_s4_h.ShowWindow(false);
	m_s5_h.ShowWindow(false);
*/
	m_netDlg.PostMessage(WM_NETWORK_SHOWRATE, 0, 0);

	OnConnectionStateChange(NET_DISCONNECTED);
	m_MobileManager.SetConnectionStat(CONNECT_CONTROL_NOT_CONNECT);
	m_isConnect = false;
	m_connect.EnableWindow(TRUE);
	m_connect.iPhoto = IMAG_CONNECT;
	m_connect.SetFocus();

	m_unconnect.EnableWindow(FALSE);
	m_unconnect.iPhoto = IMAG_DISABLECUT;
	m_unconnect.SetFocus();

	m_netDlg.PostMessage(WM_NETWORK_TIME, 0, 0);

	m_LinkDlg.SetBtnStatus(STATUS_CONNECT);

	CString strModemName;
	strModemName.Format(TEXT(""));
//	strModemName = GetModemName();

	bool bDeviceExit;
///	if(m_strProduct == "-p")
//		bDeviceExit = m_MobileManager.IsDeviceExisting();
//	else if((m_strProduct == "-u")||(m_strProduct == "-c"))
		bDeviceExit = m_MobileManager.IsModemExisting(strModemName);
	if(bDeviceExit)
	{		
		CATCmd cmd;

		cmd = m_MobileManager.SendATPacket(CMobileATProto::SetMode(8));//查询信号强度
		m_MobileManager.WaitForATCompleted(cmd,1000);
	
		cmd = m_MobileManager.SendATPacket(CMobileATProto::CheckSignal());//查询信号强度
		m_MobileManager.WaitForATCompleted(cmd,1000);

//		cmd = m_MobileManager.SendATPacket(CMobileATProto::CheckHDRSignal());//查询信号强度
//		m_MobileManager.WaitForATCompleted(cmd,1000);
		
//		cmd = m_MobileManager.SendATPacket(CMobileATProto::QueryCreg());//查询注册状态
//		m_MobileManager.WaitForATCompleted(cmd,1000);
/*		
		if((m_MobileManager.m_registerStatus == 1)||(m_MobileManager.m_registerStatus == 5));
		else if(m_bCntSuccess);
		else
		{
			CRegKey reg;
			
			CString regPath = GetConfigString();
			
			if(reg.Open(HKEY_LOCAL_MACHINE, regPath) != ERROR_SUCCESS) //打开成功
			{
				reg.Create(HKEY_LOCAL_MACHINE, regPath);		
				if(reg.Open(HKEY_LOCAL_MACHINE, regPath) != ERROR_SUCCESS)
				{
				}
			}
			TCHAR szValue[1024] = {0};
			DWORD nCount = sizeof(szValue)-1;
			CString strApn;	
			nCount = sizeof(szValue)-1;
			if(reg.QueryValue(szValue, TEXT("Apn"), & nCount) == ERROR_SUCCESS)
			{
				strApn = szValue;
			}
			else
			{
				strApn = TEXT("WCDMA Wireless");
			}
			reg.Close();

//			cmd = m_MobileManager.SendATPacket(CMobileATProto::SetApn(strApn));//设置APN
//			bool bOK = RETURN_OK == m_MobileManager.WaitForATCompleted(cmd,1000);
//			if(!bOK)
//			{
//				cmd = m_MobileManager.SendATPacket(CMobileATProto::RSPDevice());//设备复位
//				m_bResetting = true;
//				DispTextInfobar(LoadStringEx(IDS_SEARCH_NET));
//			}
		}
*/
//		cmd = m_MobileManager.SendATPacket(CMobileATProto::QueryGprsAttach());//查询GPRS附着
//		m_MobileManager.WaitForATCompleted(cmd,1000);	
		
//		cmd = m_MobileManager.SendATPacket(CMobileATProto::QueryEDGEAttach());//查询EDGE附着
//		m_MobileManager.WaitForATCompleted(cmd,1000);

		cmd = m_MobileManager.SendATPacket(CMobileATProto::SetCMGF());//设置短信格式
		m_MobileManager.WaitForATCompleted(cmd,1000);

		//允许来电显示
		cmd = m_MobileManager.SendATPacket(CMobileATProto::SetDialShow(true));
		m_MobileManager.WaitForATCompleted(cmd,1000);
		
//		cmd = m_MobileManager.SendATPacket(CMobileATProto::SetCharset());//设置字符集
//		m_MobileManager.WaitForATCompleted(cmd,1000);
		
//		SetTimer(TIMER_CHECK_SIGNAL, 8000, NULL);	
	}

//	strModemName = GetModemName();

	
//	if(m_strProduct == "-p")
//		bDeviceExit = m_MobileManager.IsDeviceExisting();
//	else if((m_strProduct == "-u")||(m_strProduct == "-c"))
		bDeviceExit = m_MobileManager.IsModemExisting(strModemName);
	GetClientRect(rc);
	rc.top = rc.bottom-50;
	
	if(bDeviceExit)
	{
		if(m_bResetting)
		{
			DispTextInfobar(LoadStringEx(IDS_SEARCH_NET));
		}
		if (m_MobileManager.isAbleCom)				// MODIFY BY LAB686 DEL ELSE IF
		{
			DispTextInfobar(LoadStringEx(IDS_STRING_FREE));
		}
		else
		{
			DispTextInfobar(LoadStringEx(IDS_NO_SC700_CARD));
		}
	}
	else
	{
		DispTextInfobar(LoadStringEx(IDS_NO_SC700_CARD));

	}
	m_linktype = NETTYPE_NOSERVICE;
	InvalidateRect(&rc);

	TRACE(TEXT("CSunnyCatDlg::OnNetDisconnected :> END\n"));

	return 0;
}



LONG CSunnyCatDlg::OnMsgStatusChange(WPARAM wp, LPARAM)
{
	EConnectionStatus status = (EConnectionStatus)wp;
	return 0;
}

void CSunnyCatDlg::OnConnectionStateChange(EConnectionStatus stat)
{
	PostMessage(WM_CONN_STAT, stat, 0);
}

void CSunnyCatDlg::Connect()
{
	OnButtonConnect();
}

void CSunnyCatDlg::Disconnect()
{
	OnButtonUnconnect();
}

void CSunnyCatDlg::OnButtonUnconnect() 
{
	// TODO: Add your control notification handler code here
		// TODO: Add your control notification handler code here
	
	if(CONNECT_CONTROL_NOT_CONNECT == m_MobileManager.GetConnectionStat())
	{
		return;
	}

	if(GMessageBox(LoadStringEx(IDS_DISCONNECT_PROMPT), TEXT(""), MB_ICONQUESTION | MB_YESNO) == IDYES)
	{
		OnConnectionStateChange(NET_DISCONNECTING);
        m_MobileManager.NetworkDisconnect();
		
	}
	else
	{
		return;
	}
	m_netDlg.SendMessage(WM_NETWORK_SHOWRATE, 0, 0);

	m_unconnect.EnableWindow(FALSE);
	m_unconnect.iPhoto = IMAG_DISABLECUT;
	m_unconnect.SetFocus();
	m_connect.EnableWindow(FALSE);
	m_connect.iPhoto = IMAG_DISABLECON;
	m_connect.SetFocus();	

	m_bConnected = TRUE;
	m_bUnconnected = TRUE;
}
/***********************************
void CSunnyCatDlg::OnButtonUnconnectevdo()
{
}
void CSunnyCatDlg::OnButtonConnectevdo()
{
}
/************************************/
void CSunnyCatDlg::OnMenuUnconnect() 
{
	// TODO: Add your command handler code here
	OnButtonUnconnect();
}

void CSunnyCatDlg::OnMenuConnect() 
{
	// TODO: Add your command handler code here
	OnButtonConnect();
}

void CSunnyCatDlg::OnMenuClose() 
{
	// TODO: Add your command handler code here
	//DestroyWindow();
	//OnClose();
	PostMessage(WM_CLOSE);
}


void CSunnyCatDlg::OnMenuHelpinfo() 
{
	// TODO: Add your command handler code here
//	if(m_strLanguage == "-Ch")		//中文帮助
//	{
		CString exeFullPath = GetAppPath();
		exeFullPath=exeFullPath+"resource\\help_en.pdf";
		
		HINSTANCE hr = ShellExecute(0, TEXT("open"), exeFullPath, NULL,NULL,SW_SHOWNORMAL);
		int dwhr = reinterpret_cast<int>(hr);
		if(dwhr <= 32)
		{
			GMessageBox(LoadStringEx(IDS_NOTINSTALL_PDF));
		}
//	}
}

void CSunnyCatDlg::OnMenuAbout() 
{
	// TODO: Add your command handler code here
	CAbout about;
	about.DoModal();
}

//添加图标到任务栏托盘
BOOL CSunnyCatDlg::TaskBarAddIcon()
{
    BOOL bRet; 
    NOTIFYICONDATA tnid;    
    CString strTip = GetAppName();
 
    tnid.cbSize = sizeof(NOTIFYICONDATA); 
    tnid.hWnd = m_hWnd; 
    tnid.uID = WM_NOTIFYICON; 
    tnid.uFlags = NIF_MESSAGE | NIF_ICON | NIF_TIP; 
    tnid.uCallbackMessage = WM_NOTIFYICON; 
    tnid.hIcon = m_hIcon;
    _tcscpy(tnid.szTip, (LPCTSTR)strTip);
 
    bRet = Shell_NotifyIcon(NIM_ADD, &tnid);  
    
    return bRet;
}


//删除任务栏托盘图标
BOOL CSunnyCatDlg::TaskBarDeleteIcon()
{
    BOOL bRet; 
    NOTIFYICONDATA tnid; 
 
    tnid.cbSize = sizeof(NOTIFYICONDATA); 
    tnid.hWnd = m_hWnd; 
    tnid.uID = WM_NOTIFYICON; 
         
    bRet = Shell_NotifyIcon(NIM_DELETE, &tnid);  
    
    return bRet;
}

//响应任务栏托盘图标消息
LONG CSunnyCatDlg::OnNotifyIcon(WPARAM wParam, LPARAM lParam)
{
    UINT uID; 
    UINT uMouseMsg; 
 
    uID = (UINT) wParam; 
    uMouseMsg = (UINT) lParam;
    if ((uMouseMsg == WM_LBUTTONDOWN)||
        (uMouseMsg == WM_LBUTTONDBLCLK))
    {
        //show the main window.
        SendMessage(WM_SYSCOMMAND, SC_RESTORE, 0);
    }

    if(uMouseMsg == WM_RBUTTONDOWN)
    {
        //display the system menu.
        CMenu SysMenu;
        CString strMenu;
        POINT mousePos;

	
        SysMenu.CreatePopupMenu(); 
        //Restory.
        strMenu = LoadStringEx(IDS_RESTORY);
        strMenu += "(&R)";
        SysMenu.AppendMenu(MF_STRING,SC_RESTORE,strMenu);
        //Minsize.
        strMenu = LoadStringEx(IDS_MINIMIZE);
        strMenu += "(&N)";
        SysMenu.AppendMenu(MF_STRING,SC_MINIMIZE,strMenu);
        //Separator.
        SysMenu.AppendMenu(MF_SEPARATOR);
        //Close.
        strMenu = LoadStringEx(IDS_CLOSE);
        strMenu += "(&C)";
        SysMenu.AppendMenu(MF_STRING,SC_CLOSE,strMenu);
        //Separator.
        SysMenu.AppendMenu(MF_SEPARATOR);
        //About.
        strMenu = LoadStringEx(IDS_ABOUTBOX);
        SysMenu.AppendMenu(MF_STRING, ID_MENU_ABOUT, strMenu);
        /* Draw and track the "floating" popup */
        GetCursorPos(&mousePos);
        SysMenu.TrackPopupMenu(TPM_RIGHTBUTTON,mousePos.x,mousePos.y,this,NULL);


        /* Destroy the menu since were are done with it. */
        DestroyMenu(SysMenu.m_hMenu);        
    }
    return 0;
}

void CSunnyCatDlg::OnMenuSetlinkman() 
{
	// TODO: Add your control notification handler code here
		CPoint pt ;	
		CMenu CardMenu, *pPopMenu = NULL ;
		GetCursorPos(&pt) ;

		if(m_strLanguage == "-En")			//中文
			CardMenu.LoadMenu(IDR_MENU);
		else if(m_strLanguage == "-Ru")					//英文
		CardMenu.LoadMenu(IDR_MENU_CN);

		pPopMenu = CardMenu.GetSubMenu(1) ;	
		
		OSVERSIONINFOEX osver = {0};
		osver.dwOSVersionInfoSize = sizeof(OSVERSIONINFOEX);
		if(::GetVersionEx((LPOSVERSIONINFO)&osver) != 0)
		{
			if((osver.dwPlatformId == VER_PLATFORM_WIN32_NT)&&(osver.dwMajorVersion > 5))
			{
				if(pPopMenu != NULL)
				{
					
					CMenu* pSubMenu = pPopMenu->GetSubMenu(2);
					if(pSubMenu != NULL)
					{
						pSubMenu->EnableMenuItem(0,MF_BYPOSITION|MF_GRAYED);
						pSubMenu->EnableMenuItem(1,MF_BYPOSITION|MF_GRAYED);
					}
				}
				
			}
				
		}
		
		if(pPopMenu != NULL)
		{				
			CRect rc;
			m_ctrlContactBtn.GetWindowRect(rc);
			pPopMenu->TrackPopupMenu(TPM_LEFTBUTTON,rc.left,rc.bottom,this);
		}
}

void CSunnyCatDlg::OnMenuSet() 
{
	// TODO: Add your command handler code here
	CSetDlg setDlg;
	if(setDlg.DoModal() == IDOK)
	{
		return;
	}
}

void CSunnyCatDlg::OnMenuNetwork() 
{
	// TODO: Add your command handler code here
	SelectNetworkDlg SNDlg(m_autoregister, this);
	if(SNDlg.DoModal() == IDOK)
	{
		return;
	}
}

void CSunnyCatDlg::OnDestroy() 
{
	
	m_MobileManager.ShutDown();

	CDialog::OnDestroy();
	
	DestroyIcon(m_hIcon); 
    TaskBarDeleteIcon();
}

//UIM card unlock
LONG CSunnyCatDlg::OnUnLockUIM(WPARAM wParam, LPARAM lParam)
{
	TRACE(TEXT("CSunnyCatDlg::OnUnLockUIM :> START\n"));

	m_MobileManager.m_strPINResponse.MakeLower();
	if((m_MobileManager.m_strPINResponse == "ready"))
	{
		TRACE(TEXT("CSunnyCatDlg::OnUnLockUIM:> NO SHOW"));
		return 0;
	}
	else if ((m_MobileManager.m_strPINResponse == "sim pin") 
		|| (m_MobileManager.m_strPINResponse == "sim puk"))
	{
		TRACE(TEXT("CSunnyCatDlg::OnUnLockUIM:> SHOW"));
		CPincode m_pindialog(this);
		if(m_pindialog.DoModal() == IDOK)
		{
			return 0;
		}
		else
		{
			m_bIsLockUIM = true;
			m_bfirstmove = false;
			m_MobileManager.isAbleCom = false;
			m_MobileManager.m_bSimExist = false;
			m_MobileManager.ShutDown();
			m_connect.EnableWindow(false);
			m_connect.iPhoto = IMAG_DISABLECON;
			m_connect.SetFocus();
			m_unconnect.EnableWindow(false);
			m_unconnect.iPhoto = IMAG_DISABLECUT;
			m_unconnect.SetFocus();
			m_bConnected = TRUE;
			m_bUnconnected = TRUE;

			GMessageBox(LoadStringEx(IDS_SIM_LOCK));
			DispTextInfobar(LoadStringEx(IDS_SIM_LOCK));
		}

	}
	else
	{
		m_MobileManager.PostMessageToClient(WM_INIT_ERROR,2,0);
		m_MobileManager.ShutDown();
		m_bIsLockUIM = false;
		m_bfirstmove = false;
		m_MobileManager.isAbleCom = false;
		m_MobileManager.m_bSimExist = false;
	}
	KillTimer(TIMER_CHECK_SIGNAL);

	TRACE(TEXT("m_bfirstmove = %d\n"), m_bfirstmove);
	TRACE(TEXT("!m_bIsLockUIM = %d\n"),  !m_bIsLockUIM);
	TRACE(TEXT("m_MobileManager.m_bSimExist = %d\n"), m_MobileManager.m_bSimExist);

	TRACE(TEXT("CSunnyCatDlg::OnUnLockUIM :> END \n"));

	return 1;
}

//查询状态
LONG CSunnyCatDlg::OnQueryCallStatus(WPARAM wParam, LPARAM lParam)
{
	if(CALLSTATE_INCOMING == m_MobileManager.m_pCallMgr->GetCallStatus())
	{
//		SetTimer(TIMER_CALL_STATUS, 1000, 0);
		//CATCmd cmd;
	//	cmd = m_MobileManager.SendATPacket(CMobileATProto::QueryCallEnd());//查询呼叫是否挂断
	//	m_MobileManager.WaitForATCompleted(cmd,1000);
		
		if(!m_bSetQueryRingTimer)
		{
//			if(m_strProduct == "-u")
			{
				m_bSetQueryRingTimer = true;
				SetTimer(TIMER_CALL_RING,1000,0);	
			}
		}				
	}

	return 0;
}
void CSunnyCatDlg::OnTimer(UINT nIDEvent) 
{
	// TODO: Add your message handler code here and/or call default
	switch(nIDEvent)
	{
	case TIMER_TALKING://显示通话时间
		{
			CString strInfo;
			const TCallInfo& callInfo = m_MobileManager.GetCallManager().GetCallInfo();
			if(callInfo.strName.IsEmpty())
				strInfo = callInfo.strTelephone;
			else
				strInfo = callInfo.strName;
			DWORD dwTime = m_MobileManager.GetCallManager().GetCurCallTime();
			CString strTime;
			strTime.Format(TEXT("%02d:%02d:%02d"),
			dwTime/3600, 
			(dwTime%3600)/60,
			(dwTime%60));

			strInfo += "   ";
			strInfo += strTime;
			FlashInfoBar(strInfo);
		}
		break;
	case TIMER_FLUX_SPEED:
		if(CONNECT_CONTROL_CONNECT == m_MobileManager.GetConnectionStat()) 
		{
			///正在上网,进行流量数据统计
			CInfoConnect InfoCnt = m_MobileManager.RefreshFluxCount();
			///显示上网信息
			ShowConnectMsgInInfoBar("");
		}
		
		break;
	case TIMER_NET_POWER:
		{
			WinExec("Powercfg.exe /hibernate off",SW_HIDE); 
		}
		break;
	case TIMER_CHECK_SIGNAL:
		{
			///发送信号检测命令
			m_MobileManager.CheckSignal();		
		}
		break;
	case TIMER_SENDPTNUM:
		//SendTPNumber();jiangguochao 20070119 deleted for deleting P/T delaying
		break;
	case TIMER_OUTGONING:
		if( m_MobileManager.GetCallManager().m_setCallStatus == CALLSTATE_OUTGONING)
		{
			static int s_Count = 0;
			s_Count++;
			
			CString strProcess;
			for(int i=0;i<s_Count%4;++i)strProcess += " .";
			strProcess += "\r\n";

			OnCallGoingProcess(strProcess);
		}
		else
		{
			KillTimer(TIMER_OUTGONING);
		}
		break;
	case TIMER_ENDTALK://通话结束10s回到空闲状态
		break;
	case TIMER_INCOMING:
		if(m_MobileManager.GetCallManager().m_setCallStatus == CALLSTATE_INCOMING)
		{
			static int s_Count = 0;
			s_Count++;			
			CString strProcess;
			for(int i=0;i<s_Count%4;++i)
				strProcess += " .";
			strProcess += "\r\n";

			OnCallComingProcess(strProcess);
		}
		else
		{
			KillTimer(TIMER_INCOMING);
		}
		break;

	case TIMER_CALL_STATUS:
		{
			CATCmd cmd;
			cmd = m_MobileManager.SendATPacket(CMobileATProto::QueryCallEnd());//查询呼叫是否挂断
			m_MobileManager.WaitForATCompleted(cmd,1000);
		}		
		break;
	case TIMER_CALL_RING:
		{
			CATCmd cmd;
			cmd = m_MobileManager.SendATPacket(CMobileATProto::QueryState());//查询呼叫是否挂断
			m_MobileManager.WaitForATCompleted(cmd,1000);	
		}		
		break;

	case TIMER_RESET_MODEM:
		{
			CString strModemName;
			strModemName.Format(TEXT(""));
			if(!((CSunnyCatDlg*)AfxGetMainWnd())->m_MobileManager.IsModemExisting(strModemName))
			{
//				GMessageBox(LoadStringEx(IDS_CDROM_REINSTALL));
				KillTimer(TIMER_RESET_MODEM);
			}
		}
		break;
		/******************************************************************/
	case TIMER_MONITOR_MODEM:				// ADD BY LAB686 20091102
		{
			CString strModemName;
			strModemName.Format(TEXT(""));
			if(m_MobileManager.IsModemExisting(strModemName))
			{
				KillTimer(TIMER_MONITOR_MODEM);
				if (m_bfirstmove && 
					!m_bIsLockUIM && 
					m_MobileManager.m_bSimExist && 
					m_MobileManager.IsModemUsed()
				   )
				{
					::Sleep(4000);
					ClearSimPB();
//					InitProgress();
					bool bok = m_InitAT.Start();
					m_InitAT.WaitForComplete();
					if (true == bok)
					{
						m_bfirstmove = false;
					}
					else
					{
						TRACE(TEXT("m_bfirstmove = true"));
						m_bfirstmove = true;
					}
					if(!m_MobileManager.isAbleCom)
					{
						m_connect.EnableWindow(false);
						m_connect.iPhoto = IMAG_DISABLECON;
						m_connect.SetFocus();
						m_unconnect.EnableWindow(false);		
						m_unconnect.iPhoto = IMAG_DISABLECUT;
						m_unconnect.SetFocus();
						/************
						m_connectevdo.EnableWindow(false);
						m_connectevdo.iPhoto = IMAG_DISABLECON;
						m_connectevdo.SetFocus();
						m_unconnectevdo.EnableWindow(false);		
						m_unconnectevdo.iPhoto = IMAG_DISABLECUT;
						m_unconnectevdo.SetFocus();
						/************/
						
						m_bConnected = TRUE;
						m_bUnconnected = TRUE;
					}
					else
					{
						m_connect.EnableWindow(true);
						m_connect.iPhoto = IMAG_CONNECT;
						m_connect.SetFocus();
						m_unconnect.EnableWindow(false);		
						m_unconnect.iPhoto = IMAG_DISABLECUT;
						m_unconnect.SetFocus();
						/************
						m_connectevdo.EnableWindow(true);
						m_connectevdo.iPhoto = IMAG_CONNECT;
						m_connectevdo.SetFocus();
						m_unconnectevdo.EnableWindow(false);		
						m_unconnectevdo.iPhoto = IMAG_DISABLECUT;
						m_unconnectevdo.SetFocus();		
						/************/
						m_bConnected = FALSE;
						m_bUnconnected = TRUE;
					}

				}
				else
				{
					if (!m_MobileManager.isAbleCom)
					{
						OnShowSignal(0, 0);
//						OnShowHDRSignal(0, 0);
						UpdateWindow();
					}
				}
				SetTimer(TIMER_MONITOR_MODEM, 7500, 0); //设置定时监听硬件	// add by lab686 
			}
			else
			{
				TRACE(TEXT("TIMER_MONITOR_MODEM :>m_bfirstmove = %d\n"), m_bfirstmove);
				if ( !m_bfirstmove )
				{
//					m_MobileManager.m_bSimExist = true;
					m_MobileManager.ShutDown();
					m_MobileManager.isAbleCom = false;

					if(!m_MobileManager.isAbleCom)
					{
						m_connect.EnableWindow(false);
						m_connect.iPhoto = IMAG_DISABLECON;
						m_connect.SetFocus();
						m_unconnect.EnableWindow(false);		
						m_unconnect.iPhoto = IMAG_DISABLECUT;
						m_unconnect.SetFocus();
						m_bConnected = TRUE;
						m_bUnconnected = TRUE;
					}
					else
					{
						m_connect.EnableWindow(true);
						m_connect.iPhoto = IMAG_CONNECT;
						m_connect.SetFocus();
						m_unconnect.EnableWindow(false);		
						m_unconnect.iPhoto = IMAG_DISABLECUT;
						m_unconnect.SetFocus();
						m_bConnected = FALSE;
						m_bUnconnected = TRUE;
					}
					CString strInfo = _T("");
					strInfo = LoadStringEx(IDS_NO_SC700_CARD);
					DispTextInfobar(strInfo);
					OnShowSignal(0, 0);
//					OnShowHDRSignal(0, 0);	
					m_bfirstmove = true;
				}
				m_bIsLockUIM = false;
				m_MobileManager.m_bSimExist = true;
				SetWorkMode(m_MobileManager,USETYPE_USBMODEM,true);
				UpdateWindow();
			}
			TRACE(TEXT("TIMER_MONITOR_MODEM :>m_bfirstmove = %d\n"), m_bfirstmove);
			TRACE(TEXT("TIMER_MONITOR_MODEM :>!m_bIsLockUIM = %d\n"), !m_bIsLockUIM);
			TRACE(TEXT("TIMER_MONITOR_MODEM :>m_MobileManager.m_bSimExist = %d\n"), m_MobileManager.m_bSimExist);
			TRACE(TEXT("TIMER_MONITOR_MODEM :>m_MobileManager.isAbleCom = %d\n"), m_MobileManager.isAbleCom);
		}
		break;
		/******************************************************************/
	case TIMER_SHOW_DLG:
		{
			KillTimer(TIMER_SHOW_DLG);
			m_removePrompt->ShowWindow(SW_HIDE);
		}
		break
			;
		
	default:
		break;
	}	

	CDialog::OnTimer(nIDEvent);
}


void CSunnyCatDlg::OnCallGoingProcess(const CString& strProcess)
{
	CString str = LoadStringEx(IDS_CALL_CALLING);

	str += strProcess;
}

void CSunnyCatDlg::OnCallComingProcess(const CString& strProcess)
{
	CString str = LoadStringEx(IDS_CALL_INCOME);
	str += strProcess;
}


//显示状态栏的提示信息
void CSunnyCatDlg::FlashInfoBar(CString strText)
{
	if(m_bResetting)
	{
		DispTextInfobar(LoadStringEx(IDS_SEARCH_NET));
		
		return;
	}
	bool b_ableCom = m_MobileManager.isAbleCom;
	CString strInfo = _T("");
	if(!b_ableCom)
	{		
		strInfo = LoadStringEx(IDS_NO_SC700_CARD);
		DispTextInfobar(strInfo);

		return;
	}
	

	CallStatus m_callStatus;// = pCallManager->GetCallStatus();
	m_callStatus = m_MobileManager.m_pCallMgr->GetCallStatus();
	
	CRect rc;
	switch(m_callStatus)
	{
			 //空闲状态	
	case CALLSTATE_IDLE:
		if(!m_isConnect)
		{
			strInfo = LoadStringEx(IDS_STRING_FREE);
			
			DispTextInfobar(strInfo);

		}

		break;
		    //来电振玲状态
	case CALLSTATE_INCOMING: 
		strInfo = LoadStringEx(IDS_CALL_INCOME);
		strInfo += "  ";
		strInfo += strText;
	
		DispTextInfobar(strInfo);

	

		break;
		    //呼入接听状态
	case CALLSTATE_IN_TALKING: 
		strInfo = LoadStringEx(IDS_CALL_TALKING);
		strInfo += "  ";
		strInfo += strText;
	
		DispTextInfobar(strInfo);


		m_callDlg.ShowCallInImag(4);
		break;
		    //呼出接听状态
	case CALLSTATE_OUT_TALKING:			
		strInfo = LoadStringEx(IDS_CALL_TALKING);
		strInfo += "  ";
		strInfo += strText;
	
		DispTextInfobar(strInfo);

		m_callDlg.ShowCallInImag(4);
		break;
		    //正在呼出状态
	case CALLSTATE_OUTGONING:			
		strInfo = LoadStringEx(IDS_CALL_CALLING);
		strInfo += "  ";
		strInfo += strText;
	
		DispTextInfobar(strInfo);

	//	m_ctrlInfoBar.SetBkColor(RGB(200,0,0));
		m_callDlg.ShowCallInImag(4);

		break;
		
	default:
		m_ctrlInfoBar.GetWindowRect(rc);
		DispTextInfobar("");
	

		break;
	}

}

//刷新上网时信息栏的显示
void CSunnyCatDlg::FlashInfoForNetWork()
{
	//连接中
	
	//上网持续

	//断开网络
}

void CSunnyCatDlg::OnStaticSmsimg() 
{
	// TODO: Add your control notification handler code here
	ShowPage(3);
}

//初使化服务信息栏
void CSunnyCatDlg::InitServerUI()
{
	
}

LONG CSunnyCatDlg::OnSMSNotify(WPARAM wParam, LPARAM lParam)
{
	TRACE(TEXT("OnSMSNotify :> READ NEW SMS\n"));
	const TSMSItem* pSMS = m_MobileManager.ReadSMS(wParam, 10000, "SM");/*jiangguochao changed MT->SM 20070115*/
	if(pSMS)
	{
	//	m_MobileManager.AddNewEmbedSMS(*pSMS);
		m_smsDlg.InsertNewSIMSms(*pSMS);
		m_NewSMSNum = pSMS->strPhoneNum;
		m_NewSMSText = pSMS->strText;
		PostMessage(WM_SMS_NEW, 0, 1);
	}

	return 0;
}


void CSunnyCatDlg::FormatConnInfo(const CInfoConnect& Info, CString& strInfo)
{
	CString info;
	info = LoadStringEx(IDS_CONNECT_CONNECTED);			


	float f_dwRecvTotal, f_dwSendTotal, f_dwRecvPS, f_dwSendPS;

	f_dwRecvTotal = (float)Info.m_dwRecvTotal;
	f_dwRecvTotal /= 1024;

	f_dwSendTotal = (float)Info.m_dwSendTotal;
	f_dwSendTotal /= 1024;

	f_dwRecvPS = (float)Info.m_dwRecvPS;
	f_dwRecvPS /= 1024;

	f_dwSendPS = (float)Info.m_dwSendPS;
	f_dwSendPS /= 1024;

	CString strSend;
	strSend.Format(TEXT("%s:%.3f KBPS "),
				LoadStringEx(IDS_SEND),
				f_dwSendPS);

	CString strRecv;
	strRecv.Format(TEXT("%s:%.3f KBPS "),
				LoadStringEx(IDS_RECEIVE),
				f_dwRecvPS);

	strInfo += strSend + strRecv;
	//((CSunnyCatDlg *)AfxGetMainWnd())->m_netDlg.PostMessage(WM_NETWORK_SHOWRATE, ceil(f_dwRecvPS*8), ceil(f_dwSendPS*8));

	m_netDlg.DispConnectInfo(m_dwConnectSecond++,f_dwRecvTotal,f_dwSendTotal,f_dwRecvPS,f_dwSendPS);
}

void CSunnyCatDlg::ShowConnectMsgInInfoBar(const CString& inStrMsg)
{
	CString strInfo = "";
	CString info;
				
	switch(m_MobileManager.GetConnectionStat())
	{
	case CONNECT_CONTROL_NOT_CONNECT:
		if(inStrMsg == "")
		{
			strInfo = "\r\n\r\n\r\n";
			strInfo += LEFTPANEL_INDENT;
			info = LoadStringEx(IDS_CONNECT_OFFLINE);
			strInfo += info;			
		}
		else
		{
			strInfo = inStrMsg;
		}

		break;
	case CONNECT_CONTROL_DAIL:
		if(inStrMsg == "")
		{
			info = LoadStringEx(IDS_CONNECT_CONNECTING);
			strInfo = info;			
		}

		else
		{
			strInfo = inStrMsg;
		}	

		break;
	case CONNECT_CONTROL_CONNECT:
		if(inStrMsg.IsEmpty())
		{
			const CInfoConnect& Info = m_MobileManager.GetFluxCount();
			FormatConnInfo(Info, strInfo);
		}
		else
		{
			strInfo = inStrMsg;
		}

		break;
	case CONNECT_CONTROL_HANGUP:
		if(inStrMsg == "")
		{
			strInfo =LoadStringEx(IDS_CONNECT_DISCONNECTING);
		}  
		else
		{
			strInfo = inStrMsg;
		}

		break;
	default:
		return;
		break;	
	}
	
	DispTextInfobar(strInfo);

}

void CSunnyCatDlg::OnSize(UINT nType, int cx, int cy) 
{
	CDialog::OnSize(nType, cx, cy);
	
	// TODO: 在此处添加消息处理程序代码

	CWnd* pWnd[]=
	{
		&m_LinkDlg,
		&m_netDlg,
		&m_callDlg,
		&m_smsDlg,
		&m_linkManDlg,
		&m_recordDlg
	};
	for(size_t i=0;i<sizeof(pWnd)/sizeof(pWnd[0]);i++)
	{
		if(pWnd[i]->GetSafeHwnd() != NULL)
		{
			CRect rc;
			pWnd[i]->GetWindowRect(rc);
			::MapWindowPoints( HWND_DESKTOP, GetSafeHwnd(), (LPPOINT)(RECT*)&rc, 2 );
			rc.right = cx;
			rc.bottom = cy-41;
			pWnd[i]->MoveWindow(&rc,TRUE);
		}
	}
	CWnd* pWndbottom[] =
	{
		&m_s0,
		&m_s1,
		&m_s2,
		&m_s3,
		&m_s4,
		&m_s5,
		&m_s0_h,
		&m_s1_h,
		&m_s2_h,
		&m_s3_h,
		&m_s4_h,
		&m_s5_h,

		&m_sEvdo,
		&m_sOneX,
		&m_Line1,
		&m_Line2,
		&m_staOnLine,
		&m_netEdge,
		&m_netGprs,
		&m_netGsm,
		&m_notRegister,
		&m_roaming,
		&m_NotOnLine
	};
	for(i=0;i<sizeof(pWndbottom)/sizeof(pWndbottom[0]);i++)
	{
		if(pWndbottom[i]->GetSafeHwnd() != NULL)
		{
			CRect rc;
			pWndbottom[i]->GetWindowRect(rc);
			::MapWindowPoints( HWND_DESKTOP, GetSafeHwnd(), (LPPOINT)(RECT*)&rc, 2 );
			int nHeight = rc.Height();
			rc.top = cy-30;
			rc.bottom = rc.top+nHeight;
			pWndbottom[i]->MoveWindow(&rc,TRUE);
		}
	}

	if(m_Line3.GetSafeHwnd() != NULL)
	{
		CRect rc;
		m_Line3.GetWindowRect(rc);
		::MapWindowPoints( HWND_DESKTOP, GetSafeHwnd(), (LPPOINT)(RECT*)&rc, 2 );
		int nWidth = rc.Width();
		int nHeight = rc.Height();
		rc.top = cy-33;
		rc.bottom = rc.top+nHeight;
		rc.left = cx-50;
		rc.right = rc.left+nWidth;
		m_Line3.MoveWindow(&rc,TRUE);
	}
	if(m_imgSMSPromt.GetSafeHwnd() != NULL)
	{
		CRect rc;
		m_imgSMSPromt.GetWindowRect(rc);
		::MapWindowPoints( HWND_DESKTOP, GetSafeHwnd(), (LPPOINT)(RECT*)&rc, 2 );
		int nWidth = rc.Width();
		int nHeight = rc.Height();
		rc.top = cy-33;
		rc.bottom = rc.top+nHeight;
		rc.left = cx-40;
		rc.right = rc.left+nWidth;
		m_imgSMSPromt.MoveWindow(&rc,TRUE);
	}

	if(m_ctrlInfoBar.GetSafeHwnd() != NULL)
	{
		CRect rc;
		m_ctrlInfoBar.GetWindowRect(rc);
		::MapWindowPoints( HWND_DESKTOP, GetSafeHwnd(), (LPPOINT)(RECT*)&rc, 2 );
		int nWidth = rc.Width();
		int nHeight = rc.Height();
	
		rc.top = cy-35;
		rc.bottom = rc.top+nHeight;
		
		m_ctrlInfoBar.MoveWindow(&rc,TRUE);
	}
	
}

void CSunnyCatDlg::OnGetMinMaxInfo( MINMAXINFO FAR* lpMMI )
{
	CClientDC dc(this);	

	int nHorz = dc.GetDeviceCaps(HORZRES);	
	int nVert = dc.GetDeviceCaps(VERTRES);

	lpMMI->ptMinTrackSize.x = 600;
	lpMMI->ptMinTrackSize.y = 400;


}

void CSunnyCatDlg::OnClose() 
{
	if(Exit(true))
	{
		CDialog::OnOK();
	}
}

bool CSunnyCatDlg::Exit(bool bNeedConfirm)
{
	TRACE(TEXT("CSunnyCatDlg::Exit :> ENTER\n"));
	Sleep(500);
	CString strInfo;
	if(m_MobileManager.GetConnectionStat() != CONNECT_CONTROL_NOT_CONNECT)
	{
		if(bNeedConfirm)
		{
			if(IDCANCEL == GMessageBox(LoadStringEx(IDS_QUIT_ONLINE_PROMPT), TEXT(""), MB_OKCANCEL|MB_ICONEXCLAMATION))
			{
				return false;
			}
		}
		m_MobileManager.ShutDown();
		//OnConnectionStateChange(NET_DISCONNECTED);
		m_MobileManager.SetConnectionStat(CONNECT_CONTROL_NOT_CONNECT);
	
	}


	if(m_MobileManager.m_pCallMgr->GetCallStatus() != CALLSTATE_IDLE)
	{
		if(bNeedConfirm)
		{
			if(IDCANCEL == GMessageBox(LoadStringEx(IDS_QUIT_CALLING_PROMPT), TEXT(""), MB_OKCANCEL|MB_ICONEXCLAMATION))
				return false;
		}

		m_MobileManager.m_pCallMgr->EndCall();
	}
	TRACE(TEXT("CSunnyCatDlg::Exit :> END\n"));
	return true;
}


void CSunnyCatDlg::OnSysCommand(UINT nID, LPARAM lParam) 
{
	// TODO: Add your message handler code here and/or call default
	if ((nID & 0xFFF0) == IDD_ABOUTBOX)
	{	
		OnMenuAbout();
	}
	else
	{
        CDialog::OnSysCommand(nID, lParam); 
        if((nID & 0xFFF0) == SC_MINIMIZE)
        {
            ShowWindow(SW_HIDE);
        }

        if((nID & 0xFFF0) == SC_RESTORE)
        {            
            ShowWindow(SW_SHOWNORMAL);
            SetForegroundWindow();
			m_netDlg.Invalidate();
        }
	}
}

//static LPCSTR g_lpWjFormat = "****====WJFORMATCLASSDATA====****" ; 

void CSunnyCatDlg::RemoveDoubleChar(CString & strSrc)
{
	int nFind = strSrc.Find(TEXT("\"\""));
	while(nFind != -1)
	{
		strSrc.Delete(nFind);
		nFind = strSrc.Find(TEXT("\"\""),nFind+1);
	}
}

bool CSunnyCatDlg::DisorceTxtFile(LPCTSTR lpFileName,CArray<CString,CString>&arrLines)
{ 
	CFile txtFile ;
	
	try
	{
		
		if(txtFile.Open(lpFileName,CFile::modeRead))
		{
			DWORD dwLen = txtFile.GetLength() ;
			char *pszTxt = (char *)calloc(dwLen+1,1) ;
			txtFile.ReadHuge(pszTxt,dwLen) ;
			CString strTxt(pszTxt);
			CString strTmp ;
			int nFind = strTxt.Find(TEXT("\r\n"));
			while(nFind>0)
			{
				strTmp = strTxt.Left(nFind);
				strTxt = strTxt.Mid(nFind+2) ;
				arrLines.Add(strTmp) ;
				nFind = strTxt.Find(TEXT("\r\n"));
			}
			free(pszTxt) ;
			txtFile.Close() ;
			return true ;
		}
	}
	catch( CFileException e )
	{   
		//异常处理
	}
	return false ;
}

//分解字符串到数组
int  CSunnyCatDlg::DivStringToArray(char *pszBuf,CArray<CString,CString>&arrDest)
{
	if(pszBuf == NULL)
		return -1 ;
	char szDivChar = ',' ;
	char szUint = 0;
	int  nLen = strlen(pszBuf) ;
	if(pszBuf[nLen-1] != szDivChar)//By weilf 2006-3-3  11:30:12
	{
		pszBuf[nLen] = szDivChar;
		pszBuf[nLen+1] = '\0';
		nLen++ ;
	}
	CString strTmp ;
	for(int i=0 ; i<nLen; )
	{
		switch(pszBuf[i])
		{
		case '\"':
			i++ ;
			while(1)
			{
				szUint = pszBuf[i++] ;
				if( (szUint == '\"' && pszBuf[i] == szDivChar) || i == nLen-1 )
				{
					i++ ;
					RemoveDoubleChar(strTmp) ;
					strTmp.TrimLeft();
					strTmp.TrimRight();
					arrDest.Add(strTmp) ;
					strTmp.Empty() ;
					break ;
				}
				strTmp += szUint ;
			}
			break;
		default:
			while(1)
			{
				szUint = pszBuf[i++] ;
				if(szUint == szDivChar || i == nLen )
				{
					RemoveDoubleChar(strTmp) ;
					strTmp.TrimLeft();
					strTmp.TrimRight();
					arrDest.Add(strTmp) ;
					strTmp.Empty() ;
					break ;
				}
				strTmp += szUint ;
			}
			break;
		}
	}
	return arrDest.GetSize() ;
}
	
	//分解字符串到数组
	int  CSunnyCatDlg::DivStringToArray(CString &strSrc,CArray<CString,CString>&arrDest,bool bUseTrim)
	{
		arrDest.RemoveAll() ;
		CString strTmp ;
		char szDivChar = ',' ;
		int nIndex = strSrc.Find(szDivChar);
		int nLen = 0;
		if(nIndex == -1)
		{
			szDivChar = '\t' ;
		}
		while(1)
		{
			nLen   = strSrc.GetLength() ;
			nIndex = strSrc.Find(szDivChar);
			if(nIndex == -1)
				break ;
			strTmp = strSrc.Left(nIndex) ;
			if(!bUseTrim)
			{
				strTmp.TrimLeft()  ;
				strTmp.TrimRight() ;
			}
			arrDest.Add(strTmp) ;
			strSrc = strSrc.Right(nLen-(nIndex+1)) ;
			//TRACE("%s\n",strTmp) ;
		}
		if(!bUseTrim)
		{
			strSrc.TrimLeft()  ;
			strSrc.TrimRight() ;
		}
		arrDest.Add(strSrc) ;
		int count = arrDest.GetSize() ;
		return count;
	}

void CSunnyCatDlg::OnPINOperationEnable()
{
	CATCmd cmd;
	cmd = m_MobileManager.SendATPacket(CMobileATProto::CheckCPIN());
	m_MobileManager.WaitForATCompleted(cmd,1000);

	cmd = m_MobileManager.SendATPacket(CMobileATProto::TestSimLock());
	m_MobileManager.WaitForATCompleted(cmd,1000);

	m_bPINenordis = true;
	CPinOper m_pinoperdialog(this);
	if(m_pinoperdialog.DoModal() == IDOK)
	{
		return ;
	}

}

void CSunnyCatDlg::OnPINOperationDisable()
{
	CATCmd cmd;
	cmd = m_MobileManager.SendATPacket(CMobileATProto::CheckCPIN());
	m_MobileManager.WaitForATCompleted(cmd,1000);

	cmd = m_MobileManager.SendATPacket(CMobileATProto::TestSimLock());
	m_MobileManager.WaitForATCompleted(cmd,1000);

	m_bPINenordis = false;
	CPinOper m_pinoperdialog(this);
	if(m_pinoperdialog.DoModal() == IDOK)
	{
		return ;
	}
}

void CSunnyCatDlg::OnLinkmanInput() 
{
	//取得文件路径
	CArray<CString,CString>arrTxts;
	int nLineCount = 0;
	int nFieldCount = 0;
	CArray<CString,CString>arrCardField ;//分解cvs头部字段
	int nFailed  = 0;//导入失败的行数
	int nSucceess = 0;//导入成功的行数
	int nCanceled = 0;//因为重复取消的行数
	int countItem = 0;//要导入的总行数
	int nOverlapped = 0;//覆盖的行数
	BOOL b_selApp2All = FALSE;//重复时是否应用到所有
	BOOL b_selOK = FALSE;//是否选择的是覆盖

	CString strFileName ;//|TXT Files (*.TXT)|*.TXT|All Files (*.*)|*.*
	static TCHAR *pszFilter= TEXT("CSV Files (*.CSV)|*.CSV||");
	CEnFileDialog SelFile(TRUE,TEXT("csv"),NULL,OFN_HIDEREADONLY | OFN_OVERWRITEPROMPT,pszFilter );	
	if(SelFile.DoModal() == IDOK)
	{
		strFileName = SelFile.GetPathName();
	}

	strFileName.TrimLeft();
	strFileName.TrimRight();
	if (strFileName.IsEmpty())
	{
		return ;
	}	

	if( !DisorceTxtFile(strFileName,arrTxts))//映射cvs记录到arrTxts
	{
		//CString strMsg ;
		//strMsg.Format("导入联系人数据失败，无法打开文件[%s]，\n可能是别的应用程序正在使用此文件！",strFileName) ;
		GMessageBox(LoadStringEx(IDS_IMPORT_OPENFILEFAIL),LoadStringEx(IDS_PROMPT),MB_ICONEXCLAMATION);
		return ;
	}
	
	nLineCount  = arrTxts.GetSize();
	if(nLineCount <= 0)
	{
		GMessageBox(LoadStringEx(IDS_IMPORT_FILE_EMPTY),LoadStringEx(IDS_PROMPT),MB_ICONEXCLAMATION);
		return;
	}
	//int nFoundIndex = nLineCount ;	
	
	DivStringToArray(arrTxts[0],arrCardField); //分解cvs字段头部到arrCardField

	nFieldCount = arrCardField.GetSize() ;

	for(int i=nFieldCount-1; i>=0; i--)
	{
		 if(arrCardField[i].IsEmpty() || arrCardField[i] =="\"" )
		 {
			arrCardField.RemoveAt(i);
		 }
			
	}

	nFieldCount = arrCardField.GetSize() ;
	
	//bool bSameCard=false; //判断导入名片本身有没相同

    //进度显示
	CProgressWnd wndProgress(this, LoadStringEx(IDS_OUTPUT_INFO));
	
	wndProgress.GoModal();
	wndProgress.SetRange(0,nLineCount-1);
	wndProgress.SetText(LoadStringEx(IDS_INPUT_INFO));

	countItem = nLineCount -1;//要导入的总行数

	for(int m=1;m < nLineCount;m++)//20070505
	{
		
		CPhonebookPC pbPCContacts; 
		CArray<CString,CString>tempCardField;

		wndProgress.StepIt();
		wndProgress.PeekAndPump();
		
		if (wndProgress.Cancelled()) 
		{
			nFailed = countItem -m+1-nCanceled -nOverlapped;
			GMessageBox(LoadStringEx(IDS_IMPORT_CANCEL),LoadStringEx(IDS_PROMPT),MB_ICONEXCLAMATION);
			break;
		}

		
		DivStringToArray(arrTxts[m],tempCardField,true); //分解cvs字段头部到arrCardField
		TPhoneBookItemEx item;
		//item.nID = 			atoi(tempCardField[0]);		//联系人唯一ID
		//item.nParentID= 	atoi(tempCardField[1]);		//父ID
		item.nParentID = 6;
		item.strTypeName = 	"";		//类型名
		item.strBusiness = 	"";     //职务
		item.strEmail = "";		    //邮件
		item.strCountry = "";		//国家/地区
		item.strHomePhone = "";		//家庭电话
		item.strFax = "";		    //传真
		item.strOffice = "";		//单位	
		item.strOfficePhone = "";	//办公电话
		item.strName = "";		    //姓名
		item.strMobilePhone = "";	//移动电

		//查找职务项位置
		for(int n =0; n < nFieldCount; n++)
		{	
			CString fieldName;
			fieldName.Format(TEXT("%s"),arrCardField[n]);

			if(fieldName == LoadStringEx(IDS_OUTLOOK_HEADSHIP))
			{
				item.strBusiness = 	tempCardField[n];//职务
				break;
			}

		}

		//查找邮件地址项位置
		for(int n1 =0; n1 < nFieldCount; n1++)
		{	
			CString fieldName;
			fieldName.Format(TEXT("%s"),arrCardField[n1]);

			if(fieldName == LoadStringEx(IDS_OUTLOOK_MAIL))
			{
				item.strEmail = tempCardField[n1];//邮件地址
				break;
			}

		}
		//查找国家/地区项位置
		for(int n2 =0; n2 < nFieldCount; n2++)
		{	
			CString fieldName;
			fieldName.Format(TEXT("%s"),arrCardField[n2]);

			if(fieldName == LoadStringEx(IDS_OUTLOOK_COUNTRY))
			{
				item.strCountry = tempCardField[n2];//国家/地区
				break;
			}

		}
		//查找住宅电话项位置
		for(int n3 =0; n3 < nFieldCount; n3++)
		{	
			CString fieldName;
			fieldName.Format(TEXT("%s"),arrCardField[n3]);

			if(fieldName == LoadStringEx(IDS_OUTLOOK_HOME))
			{
				item.strHomePhone= 	tempCardField[n3];//住宅电话
				break;
			}

		}
		//查找商务传真项位置
		for(int n4 =0; n4 < nFieldCount; n4++)
		{	
			CString fieldName;
			fieldName.Format(TEXT("%s"),arrCardField[n4]);

			if(fieldName == LoadStringEx(IDS_OUTLOOK_FAX))
			{
				item.strFax = tempCardField[n4];//商务传真
				break;
			}

		}
		//查找单位项位置
		for(int n5 =0; n5 < nFieldCount; n5++)
		{	
			CString fieldName;
			fieldName.Format(TEXT("%s"),arrCardField[n5]);

			if(fieldName == LoadStringEx(IDS_OUTLOOK_COMPANY))
			{
				item.strOffice= tempCardField[n5];//单位	
				break;
			}

		}
		//查找商务电话项位置
		for(int n6 =0; n6 < nFieldCount; n6++)
		{	
			CString fieldName;
			fieldName.Format(TEXT("%s"),arrCardField[n6]);

			if(fieldName == LoadStringEx(IDS_OUTLOOK_OFFICE))
			{
				item.strOfficePhone = tempCardField[n6];	//办公电话
				break;
			}

		}
		//查找名字项位置
		for(int n7 =0; n7 < nFieldCount; n7++)
		{	
			CString fieldName;
			fieldName.Format(TEXT("%s"),arrCardField[n7]);

			if(fieldName == LoadStringEx(IDS_OUTLOOK_NAME))
			{
				item.strName = tempCardField[n7]; //名字
				break;
			}

		}
		//查找移动电话项位置
		for(int n8 =0; n8 < nFieldCount; n8++)
		{	
			CString fieldName;
			fieldName.Format(TEXT("%s"),arrCardField[n8]);

			if(fieldName == LoadStringEx(IDS_OUTLOOK_MOBPHONE))
			{
				item.strMobilePhone = tempCardField[n8];	//移动电话
				break;
			}

		}
		
		//查找分类项位置
		for(int n9 =0; n9 < nFieldCount; n9++)
		{	
			CString fieldName;
			fieldName.Format(TEXT("%s"),arrCardField[n9]);
			
			if(fieldName == LoadStringEx(IDS_OUTLOOK_CLASS))
			{
				CString strClassName = tempCardField[n9];			
				if((strClassName == LoadStringEx(IDS_CLASS_NOCLASS))||(strClassName.IsEmpty()))
				{
					item.nParentID = 6;
					item.strTypeName = strClassName;
				}
				else
				{
					CPhonebookPCClass pbClassObj;
					pbClassObj.GetAllRecords();
					
					int classID;
					classID = pbClassObj.FindIDByName(strClassName);
					if(-1 != classID)
					{
						item.nParentID = classID;
						item.strTypeName = strClassName;
					}
					else
					{
						TPBClass pbClass;
						pbClass.strClassName = strClassName;
						pbClass.nParentID = 0;
						pbClass.strVoicePath = "";
						pbClassObj.Add(pbClass);
						
						pbClassObj.ClearVector();
						pbClassObj.GetAllRecords();
						classID = pbClassObj.FindIDByName(strClassName);
						item.nParentID = classID;
						item.strTypeName = strClassName;
					}
				}
				break;
			}
			
		}

		if(pbPCContacts.IsExitRecord(item, TRUE))
		{
			if(!b_selApp2All)
			{
				CString strRepeatInfo;
				CString strA(LoadStringEx(IDS_OUTLOOK_REPEAT_A));				//提示重复的记录是否要添加
				CString strB(LoadStringEx(IDS_OUTLOOK_REPEAT_B));				//
				strRepeatInfo.Format(TEXT("%s "),item.strName);
				strA = strA + strRepeatInfo;
				strA = strA + strB;
				CImportRepeatPrompt repeatDlg(&wndProgress,strA);
				if(IDOK == repeatDlg.DoModal())
				{
					b_selOK = TRUE;
					if(repeatDlg.m_app2All)
					{
						b_selApp2All = TRUE;
					}

					//nOverlapped +=1;
					bool bAddok1 = pbPCContacts.Add(item);
					if(bAddok1)
					{
						nOverlapped +=1;
					}
					else
					{
						nFailed +=1;
					}
				}
				else
				{
					if(repeatDlg.m_app2All)
					{
						b_selApp2All = TRUE;
					}
					b_selOK = FALSE;
					nCanceled += 1;
				}				
			}
			else
			{
				if(b_selOK)
				{
					//nOverlapped +=1;
					bool bAddok = pbPCContacts.Add(item);
					if(bAddok)
					{
						nOverlapped +=1;
					}
					else
					{
						nFailed +=1;
					}

				}
				else
				{
					nCanceled += 1;
				}
			}
		}
		else
		{
			bool bok = pbPCContacts.Add(item);
			if(bok)
			{
				nSucceess +=1;
			}
			else
			{
				nFailed +=1;
			}
		}
		

	}

	wndProgress.m_bCancelled = true;
	wndProgress.Hide();
	if(nSucceess + nOverlapped + nCanceled>0 )//
	{
		
		if((nSucceess+nFailed+nCanceled+nOverlapped) < countItem)
		{
			nFailed = countItem - nSucceess-nCanceled -nOverlapped;
		}
		//完成后隐藏进度显示
		CString strMsg ;
		strMsg = LoadStringEx(IDS_OUTLOOK_INFO_A);
		CString strCount;
		strCount.Format(TEXT("%d"), countItem);
		strMsg += strCount + LoadStringEx(IDS_OUTLOOK_INFO_B);
		CString strSucc;
		strSucc.Format(TEXT("%d"), nSucceess);
		strMsg += strSucc + LoadStringEx(IDS_OUTLOOK_INFO_C);
		CString strOverlapped;
		strOverlapped.Format(TEXT("%d"), nOverlapped);
		strMsg += strOverlapped + LoadStringEx(IDS_OUTLOOK_INFO_D);
		CString strCanncel;
		strCanncel.Format(TEXT("%d"), nCanceled);
		strMsg += strCanncel + LoadStringEx(IDS_OUTLOOK_INFO_E);
		CString strFail;
		strFail.Format(TEXT("%d"), nFailed);
		strMsg += strFail + LoadStringEx(IDS_OUTLOOK_INFO_F);
//		strMsg.Format("尝试导入联系人条数共计： %d 条\r\n导入不重复记录的条数：%d条\r\n出现重复用户选择覆盖的条数：%d条\r\n出现重复被用户取消的条数：%d 条\r\n导入失败的条数：%d条",
//			countItem,nSucceess,nOverlapped,nCanceled,nFailed);		
		GMessageBox(strMsg,LoadStringEx(IDS_PROMPT),MB_ICONINFORMATION) ;
	}
	else if ( m = nLineCount )
	{
		GMessageBox(LoadStringEx(IDS_IMPORT_EMPTY),LoadStringEx(IDS_PROMPT),MB_ICONEXCLAMATION);
	}
	else 
	{
		GMessageBox(LoadStringEx(IDS_IMPORT_FAIL));
	}
	
	arrCardField.RemoveAll();
	arrTxts.RemoveAll();
	
//	m_linkManDlg.SelectCountItem();
	m_linkManDlg.SetInputFlag(TRUE);
	m_linkManDlg.InitTree();
	m_linkManDlg.PostMessage(WM_LINKMAN_NOTIFY,0,0);		//070625he
	m_linkManDlg.SelectCountItem();
	m_linkManDlg.SetInputFlag(FALSE);
}


void CSunnyCatDlg::OnLinkmanOutput() 
{

	// TODO: Add your command handler code here
	CString strClassName="";
    
	int nFieldCount = 0;    //列名数 
	int nSuccCount  = 0;    //成功导出的名片数
	int nCardCount  = 0;

	CString strTipMsg = "" ;
    CString strPath   = "" ;
	//TRY
	try
	{

		CString strPath ;//|TXT Files (*.TXT)|*.TXT|All Files (*.*)|*.*
		static TCHAR * pszFilter= TEXT("CSV Files (*.CSV)|*.CSV||");
		CEnFileDialog SelFile(FALSE ,TEXT("csv"),strClassName,OFN_HIDEREADONLY | OFN_OVERWRITEPROMPT,pszFilter ) ;
		if(SelFile.DoModal() == IDOK)
		{
			strPath = SelFile.GetPathName();
		}
		if(strPath.IsEmpty())
		{
			return ;
		}
		
		CStdioFile Export(strPath,CFile::modeCreate|CFile::modeWrite) ;
		CString strWriteText = "";
		//---------------------------导出名片--------------------------//
		nFieldCount = 10 ;
		//用pszCardFields初始化列名		
		
		//进度显示
		CProgressWnd wndProgress(this, LoadStringEx(IDS_EXPORT_CARD));
		
		wndProgress.GoModal();
		wndProgress.SetRange(0,nFieldCount-1);
		wndProgress.SetText(LoadStringEx(IDS_CARD_EXPORTING));				 

		for( int j=0;j<nFieldCount; j++)
		{
			
			wndProgress.StepIt();
			wndProgress.PeekAndPump();
			
			if (wndProgress.Cancelled()) 
			{
				GMessageBox(LoadStringEx(IDS_EXPORT_CANCEL),LoadStringEx(IDS_PROMPT),MB_ICONEXCLAMATION);
				break;
			}
			
			strWriteText+=pszCardFields[j];
			if(j != nFieldCount-1)
				strWriteText+=",";
		}
		char filename[512]={0};
		strWriteText += "\n";
		UnicodeToASCII(strWriteText,strWriteText.GetLength(),filename,512);
	//	Export.WriteString(strWriteText+"\n");
		int tt = strlen(filename);
		Export.Write(filename,strlen(filename));
		//写名片记录
		CString strExportTxt;
		CPhonebookPC pbPCContacts;
		pbPCContacts.GetAllRecords();
		CPhonebookPCClass pbclass;
		pbclass.GetAllRecords();
		
		for (int i=0;i<pbPCContacts.Size(); ++i)
		{
			strExportTxt.Empty();
			TPhoneBookItemEx item = pbPCContacts.GetAt(i);
			CString strOfficeDuty = item.strBusiness+ ",";//职务
			CString strMailAddr = item.strEmail+ ",";//邮件
			CString strOfficeCountry = item.strCountry+ ",";  //国家/地区
			CString strHomePhone = item.strHomePhone+ ",";	//家庭电话
			CString strOfficeFax = item.strFax+ ",";			//传真
			CString strCompany = item.strOffice+ ",";		//单位	
			CString strOfficePhone = item.strOfficePhone+ ",";	//办公电话
			CString strName = item.strName+ ",";//姓名
			CString strMobilePhone = item.strMobilePhone+ ",";//移动电话
			CString strClassName = pbclass.FindNameByID(item.nParentID);//分类
			
			strExportTxt =strOfficeDuty+strMailAddr+strOfficeCountry+strHomePhone
				+strOfficeFax+strCompany+strOfficePhone+strName+strMobilePhone+strClassName;
			strWriteText = strExportTxt;
			strWriteText += "\n";
			UnicodeToASCII(strWriteText,strWriteText.GetLength(),filename,512);
			Export.Write(filename,strlen(filename));
		//	Export.WriteString(strWriteText+"\n") ;
			nSuccCount++;
		}

		Export.Close() ;
		wndProgress.m_bCancelled = TRUE;
		wndProgress.Hide();
		CString strA(LoadStringEx(IDS_OUTPUT_A));
		CString strB(LoadStringEx(IDS_OUTPUT_B));
		CString strC;
	//	strTipMsg.Format("成功导出%d条名片记录！",nSuccCount);
		strC.Format(TEXT("%d"), nSuccCount);
		strTipMsg = strA + strC;
		strTipMsg += strB;
		//完成后隐藏进度显示
	}
	catch(CFileException e)//CATCH(CFileException, e )
	{
 		strTipMsg.Format(LoadStringEx(IDS_OUTPUT_FAIL));
	}
	//END_CATCH

//	MessageBox(strTipMsg,"提示",MB_ICONINFORMATION) ;	
	GMessageBox(strTipMsg,LoadStringEx(IDS_LINKMAN_NOTICE),MB_ICONINFORMATION);
	
}

void CSunnyCatDlg::OnSynToOutlook() 
{

	// TODO: Add your command handler code here
	if(wab->InitAddrBook())
	{
		GMessageBox(LoadStringEx(IDS_OPENOE_FAIL));
		return;
	}
	
	CPhonebookPC pbPCContacts;	
	pbPCContacts.GetAllRecords();

	wab->InsertWABs(pbPCContacts);	
}

void CSunnyCatDlg::OnSynFromOutlook() 
{
	
	// TODO: Add your command handler code here
	if(wab->InitAddrBook())
	{
		GMessageBox(LoadStringEx(IDS_OPENOE_FAIL));
		return;
	}
	wab->LoadWABs();
	
	//进度显示
	CProgressWnd wndProgress(AfxGetMainWnd(), LoadStringEx(IDS_SYN_PROGRESS));
	int count = wab->AddrMemBook.size();

	wndProgress.GoModal();
	wndProgress.SetRange(0,count);
	wndProgress.SetText(LoadStringEx(IDS_SYNFROMOE_NOW));
	
	int nFailed = 0;
	int nsuccessed = 0;
	int nCanceled = 0;//因为重复取消的行数
	int nOverlapped = 0;//覆盖的行数
	BOOL b_selApp2All = FALSE;//重复时是否应用到所有
	BOOL b_selOK = FALSE;//是否选择的是覆盖
	
	//save addrbook content
	for(int i = 0;i<count;i++)
	{
		wndProgress.StepIt();
		wndProgress.PeekAndPump();
		
		if (wndProgress.Cancelled()) 
		{
			nFailed = count -i - nCanceled - nOverlapped;
			GMessageBox(LoadStringEx(IDS_EXPORT_CANCEL),LoadStringEx(IDS_PROMPT),MB_ICONEXCLAMATION);
			break;
		}
		WABS tempwab = wab->AddrMemBook[i];
		TPhoneBookItemEx item;
		item.nParentID = 6;
		item.strBusiness = tempwab.officeDuty;
		item.strCountry = tempwab.officeCountry;
		item.strEmail = tempwab.EmailAddr;
		item.strFax = tempwab.officeFax;
		item.strHomePhone = tempwab.homePhone;
		item.strMobilePhone = tempwab.mobilePhone;
		item.strName =tempwab.DisplayName;
		item.strOffice = tempwab.company;
		item.strOfficePhone = tempwab.officePhone;

		CPhonebookPC pbPCContacts;		
		if(pbPCContacts.IsExitRecord(item, TRUE))
		{
			if(!b_selApp2All)
			{
				CString strRepeatInfo;
				CString strA(LoadStringEx(IDS_OUTLOOK_REPEAT_A));
				CString strB;
				CString strC(LoadStringEx(IDS_OUTLOOK_REPEAT_B));
				strB.Format(TEXT("%s"),item.strName);
				strRepeatInfo = strA + strB;
				strRepeatInfo += strC;
				CImportRepeatPrompt repeatDlg(&wndProgress,strRepeatInfo);				

				if(IDOK == repeatDlg.DoModal())
				{
					b_selOK = TRUE;
					if(repeatDlg.m_app2All)
					{
						b_selApp2All = TRUE;
					}
					
					//nOverlapped +=1;
					bool bAddok1 = pbPCContacts.Add(item);
					if(bAddok1)
					{
						nOverlapped +=1;
					}
					else
					{
						nFailed +=1;
					}
				}
				else
				{
					if(repeatDlg.m_app2All)
					{
						b_selApp2All = TRUE;
					}
					b_selOK = FALSE;
					nCanceled += 1;
				}				
			}
			else
			{
				if(b_selOK)
				{
					//nOverlapped +=1;
					bool bAddok = pbPCContacts.Add(item);
					if(bAddok)
					{
						nOverlapped +=1;
					}
					else
					{
						nFailed +=1;
					}
				}
				else
				{
					nCanceled += 1;
				}
			}
		}
		else
		{
			bool bok = pbPCContacts.Add(item);
			if(bok)
			{
				nsuccessed +=1;
			}
			else
			{
				nFailed +=1;
			}
		}
	}

	wndProgress.m_bCancelled = true;
	wndProgress.Hide();
	if(nsuccessed + nOverlapped + nCanceled>0 )//
	{
		
		if((nsuccessed + nFailed + nCanceled + nOverlapped) < count)
		{
			nFailed = count - nsuccessed - nCanceled - nOverlapped;
		}
		//完成后隐藏进度显示
		CString strMsg ;
		strMsg = LoadStringEx(IDS_SAMESTEP_A);
		CString strCount;
		strCount.Format(TEXT("%d"), count);
		strMsg += strCount + LoadStringEx(IDS_SAMESTEP_B);
		CString strSucc;
		strSucc.Format(TEXT("%d"), nsuccessed);
		strMsg += strSucc + LoadStringEx(IDS_SAMESTEP_C);
		CString strOverlapped;
		strOverlapped.Format(TEXT("%d"), nOverlapped);
		strMsg += strOverlapped + LoadStringEx(IDS_SAMESTEP_D);
		CString strCanncel;
		strCanncel.Format(TEXT("%d"), nCanceled);
		strMsg += strCanncel + LoadStringEx(IDS_SAMESTEP_E);
		CString strFail;
		strFail.Format(TEXT("%d"), nFailed);
		strMsg += strFail + LoadStringEx(IDS_OUTLOOK_INFO_F);
		
	//	strMsg.Format("尝试同步联系人条数共计： %d 条\r\n同步不重复记录的条数：%d条\r\n出现重复用户选择覆盖的条数：%d条\r\n出现重复被用户取消的条数：%d 条\r\n同步失败的条数：%d条",
	//		count,nsuccessed,nOverlapped,nCanceled,nFailed);
		
		GMessageBox(strMsg) ;
	}
	else if ( nsuccessed = count )
	{
		GMessageBox(LoadStringEx(IDS_CONTACTS_ISNULL));
	}
	else 
	{
		GMessageBox(LoadStringEx(IDS_SYN_FAIL));
	}
	wab->AddrMemBook.clear();

	m_linkManDlg.SelectCountItem();
	m_linkManDlg.PostMessage(WM_LINKMAN_NOTIFY,0,0);
}

BOOL CSunnyCatDlg::OnNcCreate(LPCREATESTRUCT lpCreateStruct) 
{
	if (!CDialog::OnNcCreate(lpCreateStruct))
		return FALSE;
	
	// TODO: Add your specialized creation code here

	return TRUE;
}

void CSunnyCatDlg::OnMenuAddlinkman() 
{
	// TODO: Add your command handler code here
	CNewCardDlg newCardDlg;
	newCardDlg.DoModal();
}

void LogString(CString strLog);

bool CSunnyCatDlg::WaitForAutoRun(const CString& strVolume)
{
	static CThreadWaitAuotRun WaitAutoRun(m_MobileManager);
	WaitAutoRun.Start();

	return true;
}

LONG CSunnyCatDlg::OnSerialLport(WPARAM wParam, LPARAM lParam)
{
	TRACE(TEXT("CSunnyCatDlg::OnSerialLport :> START\n"));
//	GMessageBox(LoadStringEx(IDS_SYN_FAIL));

	m_MobileManager.bRemoved = true;
	//m_removePrompt->ShowWindow(SW_SHOW);
	
	//上网时拔卡会导致modem损坏，所以干脆删除modem，下一次启动时再安装
	bool bOnline = (m_MobileManager.GetConnectionStat() != CONNECT_CONTROL_NOT_CONNECT);
	if(bOnline)
	{
		KillTimer(TIMER_FLUX_SPEED);
		KillTimer(TIMER_NET_POWER);
		WinExec("Powercfg.exe /hibernate on",SW_HIDE); 

		m_netDlg.PostMessage(WM_NETWORK_TIME, 0, 0);
		
		m_bConnected = FALSE;
		m_bUnconnected = TRUE;
	
		m_MobileManager.m_bConnectSuccess = false;	
		DispTextInfobar(LoadStringEx(IDS_NO_SC700_CARD));

		m_isConnect = false;
		
		m_connect.EnableWindow(true);
		m_connect.iPhoto = IMAG_CONNECT;
		m_connect.SetFocus();
		m_unconnect.EnableWindow(false);
		m_unconnect.iPhoto = IMAG_DISABLECUT;
		m_unconnect.SetFocus();
		m_bConnected = FALSE;
		m_bUnconnected = TRUE;
	}

	m_LinkDlg.SetBtnStatus(STATUS_CONNECT);
	m_linktype = NETTYPE_NOSERVICE;

	Exit(false);

	//=============================

//	FlashInfoBar("");

	m_MobileManager.m_bSimExist = true;
	m_MobileManager.isAbleCom = false;
	m_bfirstmove = false;
	m_bIsLockUIM = false;
	
	KillTimer(TIMER_CHECK_SIGNAL);

	m_NotOnLine.ShowWindow(true);

	OnShowSignal(0, 0);
//	OnShowHDRSignal(0, 0);
	CString strInfo = _T("");
	CRect rc;
	m_sEvdo.GetWindowRect(rc);
	::MapWindowPoints( HWND_DESKTOP, GetSafeHwnd(), (LPPOINT)(RECT*)&rc, 2 );
	InvalidateRect(rc);
	m_sEvdo.SetWindowText(strInfo);
	strInfo = LoadStringEx(IDS_NO_SC700_CARD);
//	LogString(IDS_NO_SC700_CARD);
	DispTextInfobar(strInfo);
	
	TRACE(TEXT("m_bfirstmove = %d\n"), m_bfirstmove);
	TRACE(TEXT("!m_bIsLockUIM = %d\n"),  !m_bIsLockUIM);
	TRACE(TEXT("m_MobileManager.m_bSimExist = %d\n"), m_MobileManager.m_bSimExist);

	TRACE(TEXT("CSunnyCatDlg::OnSerialLport :> END \n"));

	return 0;
}

//static bool b_firstRemovedMessage = true;
LONG CSunnyCatDlg::OnDeviveChange(WPARAM wParam, LPARAM lParam)
{
	TRACE(TEXT("CSunnyCatDlg::OnDeviveChange :> ENTER\n"));
	DEV_BROADCAST_HDR*   dhr   =   (DEV_BROADCAST_HDR*)lParam;  
	CString strModemName;
	strModemName.Format(TEXT(""));
	TRACE(TEXT("wParam = %ld\n"),long(wParam));

	PDEV_BROADCAST_HDR pDevBroadcastHdr; 
	PDEV_BROADCAST_VOLUME pDevBroadcastVolume; 

	CString strVolume;

//	if(m_strProduct == "-u")
	{
		if(wParam == DBT_DEVICEARRIVAL)
		{
			TRACE(TEXT("CSunnyCatDlg::OnDeviveChange :> wParam == DBT_DEVICEARRIVAL\n"));
			pDevBroadcastHdr = (PDEV_BROADCAST_HDR) lParam; 
			if (pDevBroadcastHdr->dbch_devicetype == DBT_DEVTYP_VOLUME) 
			{ 
				pDevBroadcastVolume = (PDEV_BROADCAST_VOLUME)pDevBroadcastHdr; 
				strVolume.Empty();
				xJudgeDeviceChangePath(pDevBroadcastVolume->dbcv_unitmask,strVolume); 
				
				if(GetUsbDev("SinCom Mobile Connect"))
				{
					WaitForAutoRun(strVolume);
					m_strVolume = strVolume;
				}
				m_bResetting = false;
			} 

			if(dhr->dbch_devicetype   ==   DBT_DEVTYP_DEVICEINTERFACE)
			{
				PDEV_BROADCAST_DEVICEINTERFACE   lpdev   =   (PDEV_BROADCAST_DEVICEINTERFACE)dhr;  
				CString strDevName = lpdev->dbcc_name;
				CString str[] =
				{
					"Vid_19F5&Pid_9901",
					"VID_19F5&PID_9901",
					"Vid_0471&Pid_1201",
					"VID_0471&PID_1201"
				};
				bool bFind = false;
				for(int i=0;i<4;i++)
				{
					if(strDevName.Find(str[i]) != -1)
					{
						bFind = true;
						break;
					}
				}
				if(!bFind) return 0;
				if(!m_MobileManager.IsModemExisting(strModemName)) return 0;					
				m_removePrompt->ShowWindow(SW_HIDE);
				if(m_MobileManager.isAbleCom)return true;	
				
				
				TRACE(TEXT("COME DEVICE INSERT message\r\n"));
				TRACE(TEXT("COME DEVICE INSERT begin\r\n"));
				
				//m_removePrompt->ShowWindow(SW_HIDE);
				
			
					
				InitATSetting(true);
				
				
				m_callDlg.GetDlgItem(IDC_BUTTON_DIAL_NUM)->EnableWindow(TRUE);
				m_MobileManager.hasInserted = true;
				m_MobileManager.bRemoved = false;
				
				FlashInfoBar("");
				if(m_MobileManager.isAbleCom)
				{
					m_connect.EnableWindow(true);
					m_connect.iPhoto = IMAG_CONNECT;
					m_connect.SetFocus();
				}
				else
				{
					m_connect.EnableWindow(false);
					m_connect.iPhoto = IMAG_DISABLECON;
					m_connect.SetFocus();
				}
				
				m_unconnect.EnableWindow(false);
				m_unconnect.iPhoto = IMAG_DISABLECUT;
				m_unconnect.SetFocus();
				m_bConnected = FALSE;
				m_bUnconnected = TRUE;
				TRACE(TEXT("COME DEVICE INSERT end\r\n"));
				
			}
		}
			
		else if(wParam == DBT_DEVICEREMOVECOMPLETE)
		{
			TRACE(TEXT("CSunnyCatDlg::OnDeviveChange :> wParam == DBT_DEVICEREMOVECOMPLETE\n"));
			pDevBroadcastHdr = (PDEV_BROADCAST_HDR) lParam; 
			if (pDevBroadcastHdr->dbch_devicetype == DBT_DEVTYP_VOLUME) 
			{ 
				pDevBroadcastVolume = (PDEV_BROADCAST_VOLUME)pDevBroadcastHdr; 
				m_strVolume.Empty();
				xJudgeDeviceChangePath(pDevBroadcastVolume->dbcv_unitmask,m_strVolume); 
				
				if(GetUsbDev("HUAWEI MOBILE  CONNECT"))
				{
					m_strVolume.Empty();
				}
				
			} 

			if(dhr->dbch_devicetype   ==   DBT_DEVTYP_DEVICEINTERFACE)
			{
				PDEV_BROADCAST_DEVICEINTERFACE   lpdev   =   (PDEV_BROADCAST_DEVICEINTERFACE)dhr;  
				CString strDevName = lpdev->dbcc_name;
				CString str[] =
				{
					"Vid_19F5&Pid_9901",
					"VID_19F5&PID_9901",
					"Vid_0471&Pid_1201",
					"VID_0471&PID_1201"
				};
				bool bFind = false;
				for(int i=0;i<4;i++)
				{
					if(strDevName.Find(str[i]) != -1)
					{
						bFind = true;
						break;
					}
				}
				if(!bFind) return 0;
				
				if(m_bResetting)
				{
					m_MobileManager.ShutDown();
					m_MobileManager.isAbleCom = false;
					m_MobileManager.m_bSimExist = false;
					return true;
				}

				ClearSimPB();
				KillTimer(TIMER_RESET_MODEM);
				m_MobileManager.hasInserted = false;
				
				if(!m_MobileManager.isAbleCom)
				{
#ifndef	IN_CARD			
					m_removePrompt->ShowWindow(SW_SHOW);
					m_removePrompt->UpdateWindow();
					SetTimer(TIMER_SHOW_DLG,2000,NULL);									
#endif
				}			
				
				if(!m_MobileManager.isAbleCom)return true;
				TRACE(TEXT("COME DEVICE REMOVE start\r\n"));
				//if(!m_MobileManager.isAbleCom)return true;
				m_MobileManager.isAbleCom = false;
				OnDeviceRemoved();
				m_MobileManager.m_bSimExist = false;
#ifndef	IN_CARD
				m_removePrompt->ShowWindow(SW_SHOW);
				m_removePrompt->UpdateWindow();
				SetTimer(TIMER_SHOW_DLG,2000,NULL);
			
#endif
				
				m_connect.EnableWindow(false);
				m_connect.iPhoto = IMAG_DISABLECON;
				m_connect.SetFocus();
				m_connect.Invalidate();
				m_unconnect.EnableWindow(false);
				m_unconnect.iPhoto = IMAG_DISABLECUT;
				m_unconnect.SetFocus();
				m_unconnect.Invalidate();
				m_bConnected = true;
				m_bUnconnected = true;
				
				InitIconState();
				
			}
			
		}
		
		
	}
	TRACE(TEXT("CSunnyCatDlg::OnDeviveChange :> m_MobileManager.m_bSimExist = %d\n"), m_MobileManager.m_bSimExist);
	TRACE(TEXT("CSunnyCatDlg::OnDeviveChange :> END\n"));
	return true;
}


static CString GetRemoveModemAppName()
{
	DWORD winVerson = OptionsShellType();
	if(winVerson >= Win_2000)
	{
		return GetAppPath() + "FastRemove2KXP.exe";
	}
	else
	{
		return GetAppPath() + "FastRemove98Me.exe";
	}
}

//static void RemoveModem()
void CSunnyCatDlg::RemoveModem()
{
	/************************************************************************
	CString strCmdLine;
	strCmdLine.Format(TEXT("\"%s\" -m"), (LPCTSTR)GetRemoveModemAppName());

	HANDLE hProcess = RunProgram(strCmdLine, false);
	if(hProcess)
	{
		//WaitForSingleObject(hProcess,INFINITE);
		if(WAIT_OBJECT_0 == MsgWaitForMultipleObjects(1, &hProcess, FALSE, INFINITE, QS_ALLINPUT))
		{
			CloseHandle(hProcess);
		}		
	}
	/************************************************************************/
}



void CSunnyCatDlg::OnDeviceRemoved()
{
	//if(m_MobileManager.bRemoved)return;//避免多次提示
	m_MobileManager.bRemoved = true;
	//m_removePrompt->ShowWindow(SW_SHOW);
	
	//上网时拔卡会导致modem损坏，所以干脆删除modem，下一次启动时再安装
	bool bOnline = (m_MobileManager.GetConnectionStat() != CONNECT_CONTROL_NOT_CONNECT);
	if(bOnline)
	{
	
		KillTimer(TIMER_FLUX_SPEED);
		KillTimer(TIMER_NET_POWER);
		WinExec("Powercfg.exe /hibernate on",SW_HIDE); 

		m_netDlg.PostMessage(WM_NETWORK_TIME, 0, 0);
		
		m_bConnected = FALSE;
		m_bUnconnected = TRUE;
	
		m_MobileManager.m_bConnectSuccess = false;	
		DispTextInfobar(LoadStringEx(IDS_NO_SC700_CARD));

		m_isConnect = false;
	}
	
	
	if(!bOnline)
	{
		m_MobileManager.ShutDown();
	}

	m_LinkDlg.SetBtnStatus(STATUS_CONNECT);
	m_linktype = NETTYPE_NOSERVICE;

	Exit(false);

		//=============================

	
	FlashInfoBar("");

}

void CSunnyCatDlg::OnSynFromOO() 
{
	if(!outlookContact->Open())
	{
		GMessageBox(LoadStringEx(IDS_OPENOO_FAIL));
		return;
	}

	bool bok = outlookContact->ReadAllItemsFromOutlook();
	if(!bok)
	{
		outlookContact->Close();
	}
	m_linkManDlg.SelectCountItem();
	m_linkManDlg.PostMessage(WM_LINKMAN_NOTIFY,0,0);
	
}

void CSunnyCatDlg::OnSynToOO() 
{
	if(!outlookContact->Open())
	{
		GMessageBox(LoadStringEx(IDS_OPENOO_FAIL));
		return;
	}
	
	CPhonebookPC pbPCContacts;	
	pbPCContacts.GetAllRecords();
	outlookContact->Insert2OutllookAllItems(pbPCContacts);
}

HBRUSH CSunnyCatDlg::OnCtlColor(CDC* pDC, CWnd* pWnd, UINT nCtlColor) 
{
	HBRUSH hbr = CDialog::OnCtlColor(pDC, pWnd, nCtlColor);
	CBrush m_brush;

	// TODO: Change any attributes of the DC here

	/* 控件透明 */
	if(nCtlColor == CTLCOLOR_DLG)
	{
		pDC->SetBkMode(TRANSPARENT);
		static HBRUSH brush=CreateSolidBrush(RGB(35,119,179));
		return brush;
	}

	return (HBRUSH)m_brush;
}


void CSunnyCatDlg::OnTalking(CCallManager* pSender)
{
	StartVoice();			// 打电话时打开端口
	SetTimer(TIMER_TALKING, 1000, NULL);
}

LRESULT CSunnyCatDlg::DefWindowProc(UINT message, WPARAM wParam, LPARAM lParam) 
{
	// TODO: Add your specialized code here and/or call the base class
	CString str;
	switch(message)   
	{
	case   WM_WTSSESSION_CHANGE: 
		switch(wParam)   
		{      
		case   WTS_SESSION_LOGON:  
			TRACE(TEXT("WTS_SESSION_LOGON\n"));
			Invalidate();
			ShowWindow(SW_NORMAL);
			InitATSetting(true);
			break;   
		case   WTS_SESSION_LOGOFF: 
			TRACE(TEXT("WTS_SESSION_LOGOFF\n"));

			m_MobileManager.ShutDown();
			m_netDlg.PostMessage(WM_NETWORK_TIME,0,0);
			ShowWindow(SW_HIDE); 

			break;   

		default:   
			break;   
		}   
		break;   
	case   WM_QUERYENDSESSION:
		m_MobileManager.ShutDown();
		m_netDlg.PostMessage(WM_NETWORK_TIME,0,0);
		break;
	default:   
		break;   
	}   

	return CDialog::DefWindowProc(message, wParam, lParam);
}

LRESULT CSunnyCatDlg::OnShowCallImag(WPARAM wParam,LPARAM lParam)
{
	int nImagIndex = (int)wParam;

	m_callDlg.ShowCallInImag(nImagIndex);

	return 1;
}

LRESULT CSunnyCatDlg::OnRefreshNetRecord(WPARAM wParam,LPARAM lParam)
{
	m_recordDlg.m_ctrlRecordList.DeleteAllCol();
	m_recordDlg.LoadListHead(NETRECORD);
	m_recordDlg.ShowListNetRecord();

	return 1;
}

LRESULT CSunnyCatDlg::OnRefreshCallRecord(WPARAM wParam,LPARAM lParam)
{
	CallRecordType type = (CallRecordType)wParam;

	m_recordDlg.m_ctrlRecordList.DeleteAllCol();
	m_recordDlg.LoadListHead(CALLRECORD);
	m_recordDlg.ShowList(type);

	return 1;
}


LRESULT CSunnyCatDlg::OnRefreshInfoBar(WPARAM wParam,LPARAM lParam)
{
	CString strInfo;

	strInfo.Empty();
	if(m_MobileManager.isAbleCom&&m_MobileManager.m_bSimExist)
	{
		strInfo = LoadStringEx(IDS_STRING_FREE);
	}
	else
		strInfo = LoadStringEx(IDS_NO_SC700_CARD);
	DispTextInfobar(strInfo);
	
	return 1;
}

LRESULT CSunnyCatDlg::OnRefreshFailInfoBar(WPARAM wParam,LPARAM lParam)
{
	CString strMsg;
	strMsg.Empty();
	strMsg = LoadStringEx(IDS_CALL_FAILED);
	DispTextInfobar(strMsg);
	
	return 1;
}


LRESULT CSunnyCatDlg::OnRefreshTelInfoBar(WPARAM wParam,LPARAM lParam)
{
	CString strInfo;
	CString strText;
	TCallInfo CallInfo;

	strText.Empty();
	CallInfo = m_MobileManager.m_pCallMgr->GetCallInfo();

	strText = CallInfo.strTelephone;

	strInfo.Empty();
	strInfo = LoadStringEx(IDS_CALL_TALKING);
	strInfo += "  ";
	strInfo += strText;
	DispTextInfobar(strInfo);

	m_callDlg.ShowCallInImag(4);

	return 1;
}

LRESULT CSunnyCatDlg::OnDispTelInEdit(WPARAM wParam,LPARAM lParam)
{
	CWnd* pWnd = m_callDlg.GetDlgItem(IDC_EDIT_DAIL_NUM);
	CString strTel;
	strTel.Empty();
	strTel = m_MobileManager.m_pCallMgr->GetCallInfo().strTelephone;
	pWnd->SetWindowText(strTel);

	return 1;
}


///休眠管理
LONG CSunnyCatDlg::OnPower(WPARAM wParam, LPARAM lParam) 
{
	switch(wParam)
	{
	case PBT_APMQUERYSUSPEND:
		TRACE(TEXT("PBT_APMQUERYSUSPEND\n"));
		Invalidate();
		/**************************
		if(m_strProduct == "-p")
		{
		if(m_MobileManager.GetConnectionStat() == CONNECT_CONTROL_NOT_CONNECT)
		{
			m_MobileManager.ShutDown();
		}	
		}
		/**************************/
//		if(m_strProduct == "-u")
		{
			if(m_MobileManager.m_pCallMgr->GetCallStatus() != CALLSTATE_IDLE)
			{
				m_MobileManager.m_pCallMgr->EndCall(true);
			}
			m_MobileManager.m_pCallMgr->SetCallStatus(CALLSTATE_IDLE);
			m_MobileManager.SetConnectionStat(CONNECT_CONTROL_NOT_CONNECT);
			KillTimer(TIMER_TALKING);

			m_MobileManager.ShutDown();
		}
	

		break;
	case PBT_APMSUSPEND: //suspending
		TRACE(TEXT("PBT_APMSUSPEND\n"));
		Invalidate();
		/**********************
		if(m_strProduct == "-p")
		{
		if(m_MobileManager.GetConnectionStat() != CONNECT_CONTROL_NOT_CONNECT)
		{
			m_MobileManager.ShutDown();
			if(m_MobileManager.m_pCallMgr->GetCallStatus() != CALLSTATE_IDLE)
			{
				m_MobileManager.m_pCallMgr->EndCall(true);
				m_callDlg.ShowCallInImag(0);
			}
			m_MobileManager.m_pCallMgr->SetCallStatus(CALLSTATE_IDLE);
			m_MobileManager.SetConnectionStat(CONNECT_CONTROL_NOT_CONNECT);
			FlashInfoBar("");
		}
		}
		/**********************/
//		if(m_strProduct == "-u")
		{
			

			if(m_MobileManager.m_pCallMgr->GetCallStatus() != CALLSTATE_IDLE)
			{
				m_MobileManager.m_pCallMgr->EndCall(true);
			}
			m_MobileManager.ShutDown();
			m_MobileManager.m_pCallMgr->SetCallStatus(CALLSTATE_IDLE);
			m_MobileManager.SetConnectionStat(CONNECT_CONTROL_NOT_CONNECT);
			FlashInfoBar("");
		}
		
			
		break;
	case PBT_APMRESUMESUSPEND: //Resuming
		TRACE(TEXT("PBT_APMRESUMESUSPEND\n"));
		//Invalidate();
		ShowWindow(SW_NORMAL);
		
		if(m_strProduct == "-p")
		{
		InitATSetting(true);
		OnNetDisconnected(0,0);
		m_netDlg.PostMessage(WM_NETWORK_TIME,0,0);
		
		if(m_MobileManager.m_pCallMgr->GetCallStatus() != CALLSTATE_IDLE)
		{
			m_MobileManager.m_pCallMgr->EndCall(true);
			m_callDlg.ShowCallInImag(0);
		}
		m_MobileManager.m_pCallMgr->SetCallStatus(CALLSTATE_IDLE);
		m_MobileManager.SetConnectionStat(CONNECT_CONTROL_NOT_CONNECT);
		FlashInfoBar("");
		PostMessage(WM_SHOW_CALLIMAG,0,0);
		KillTimer(TIMER_TALKING);

		}
		CString strModemName;
		strModemName.Format(TEXT(""));
//		strModemName = GetModemName();
		
		if(m_MobileManager.IsModemExisting(strModemName)&&m_MobileManager.m_bSimExist)					
		{
			InitATSetting(true);
			if(m_MobileManager.m_pCallMgr->GetCallStatus() != CALLSTATE_IDLE)
			{
				m_MobileManager.m_pCallMgr->EndCall(true);
			}
			m_MobileManager.m_pCallMgr->SetCallStatus(CALLSTATE_IDLE);
			m_MobileManager.SetConnectionStat(CONNECT_CONTROL_NOT_CONNECT);
			KillTimer(TIMER_TALKING);
		}
		OnNetDisconnected(0,0);
		FlashInfoBar("");
		Invalidate();	

		break;
	}

	return TRUE;
}

//SIM卡绑定失败
LONG CSunnyCatDlg::OnSIMLock(WPARAM wParam, LPARAM count)
{
	m_MobileManager.m_simLocked = true;
	/*
	if(m_MobileManager.GetConnectionStat() == CONNECT_CONTROL_NOT_CONNECT)
	{
		m_MobileManager.ShutDown();
	}
	
	Exit(false);
	GMessageBox(LoadStringEx(IDS_SIM_LOCK_ERROR),LoadStringEx(IDS_PROMPT),MB_ICONEXCLAMATION);
	PostMessage(WM_CLOSE);
	AfxPostQuitMessage(0);
	*/
	return 0;
}


BOOL CSunnyCatDlg::ResetCom(TCHAR * GUIDString,CString strDeviceName) 
{
	HDEVINFO hInfo = NULL;
	GUID uID ;	//= {4D36E978-E325-11CE-BFC1-08002BE10318};

//	TCHAR * GUIDString = NULL;
//	GUIDString = _T("4D36E978-E325-11CE-BFC1-08002BE10318");
	BOOL bRet = FALSE;

	UuidFromString((TCHAR *)GUIDString, &uID);


 	typedef  HDEVINFO( WINAPI * pfnSetupDiGetClassDevsA)(IN CONST GUID *,IN PCSTR ,IN HWND ,IN DWORD);
	m_hModuleDDK = ::LoadLibrary(TEXT("setupapi.dll"));
	pfnSetupDiGetClassDevsA pf = (pfnSetupDiGetClassDevsA)::GetProcAddress(m_hModuleDDK,"SetupDiGetClassDevsA");
	
	if(pf == NULL)
		return FALSE;

	hInfo = pf(&uID,NULL,NULL,DIGCF_PRESENT );
	
	bRet = StateChange(DICS_DISABLE,hInfo,strDeviceName);
	if(!bRet)
	{
		return FALSE;
	}
	
	bRet = StateChange(DICS_ENABLE,hInfo,strDeviceName);
	if(!bRet)
	{
		return FALSE;
	}
	
		
	if(m_hModuleDDK != NULL)
	{
		::FreeLibrary(m_hModuleDDK);
		m_hModuleDDK = NULL;
	}

	return TRUE;
}


BOOL CSunnyCatDlg::StateChange(DWORD NewState, HDEVINFO m_hDevInfo,CString strDeviceName) 
{ 
	SP_PROPCHANGE_PARAMS PropChangeParams = {sizeof(SP_CLASSINSTALL_HEADER)}; 
	SP_DEVINFO_DATA DeviceInfoData = {sizeof(SP_DEVINFO_DATA)};  
	TCHAR   szBuf[MAX_PATH];  
	//CString strName = "Compact Flash OX16CF950";
	// 
	// Get a handle to the Selected Item. 
	//
	typedef  BOOL( WINAPI * pfnSetupDiEnumDeviceInfo)(IN HDEVINFO,IN DWORD,OUT PSP_DEVINFO_DATA);
	typedef BOOL (WINAPI * pfnSetupDiGetDeviceRegistryPropertyA)(IN  HDEVINFO, IN  PSP_DEVINFO_DATA,IN  DWORD,OUT PDWORD,OUT PBYTE,IN  DWORD ,OUT PDWORD);

	pfnSetupDiEnumDeviceInfo pf = (pfnSetupDiEnumDeviceInfo)::GetProcAddress(m_hModuleDDK,"SetupDiEnumDeviceInfo");
	pfnSetupDiGetDeviceRegistryPropertyA pfp = (pfnSetupDiGetDeviceRegistryPropertyA)::GetProcAddress(m_hModuleDDK,"SetupDiGetDeviceRegistryPropertyA");
	
	if(pf == NULL)
		return FALSE;
	if(pfp == NULL)
		return FALSE;


	for(int i = 0;i >= 0; i++ )
	{
		BOOL bRet = pf(m_hDevInfo,i,&DeviceInfoData);
		if(!bRet)
		{
			return FALSE;
		}
		if(pfp( m_hDevInfo,&DeviceInfoData,SPDRP_DEVICEDESC,NULL,(PBYTE)szBuf,MAX_PATH, NULL))   
		{   
			if(strDeviceName == szBuf)
				break;
		}   
	}
	

	// 
	// Set the PropChangeParams structure. 
	// 	
	
	PropChangeParams.ClassInstallHeader.InstallFunction = DIF_PROPERTYCHANGE; 
	PropChangeParams.Scope = DICS_FLAG_GLOBAL;//CONFIGSPECIFIC; 
	PropChangeParams.HwProfile = 0; 
	PropChangeParams.StateChange = NewState; 
	
	typedef BOOL(WINAPI *pfSetupDiSetClassInstallParamsA)(IN HDEVINFO,IN PSP_DEVINFO_DATA ,IN PSP_CLASSINSTALL_HEADER,IN DWORD);
	pfSetupDiSetClassInstallParamsA pfa = (pfSetupDiSetClassInstallParamsA)::GetProcAddress(m_hModuleDDK,"SetupDiSetClassInstallParamsA");
	if(pfa == NULL)
		return FALSE;
	
	if (!pfa(m_hDevInfo, &DeviceInfoData, (SP_CLASSINSTALL_HEADER *)&PropChangeParams, sizeof(PropChangeParams))) 
	{ 
		return FALSE; 
	} 
	
	
	typedef BOOL(WINAPI *pfnSetupDiChangeState)(IN     HDEVINFO, IN OUT PSP_DEVINFO_DATA);
	pfnSetupDiChangeState pfs = (pfnSetupDiChangeState)::GetProcAddress(m_hModuleDDK,"SetupDiChangeState");
	if(pfs == NULL)
		return FALSE;
	
	if(!pfs(m_hDevInfo,&DeviceInfoData))
		return FALSE; 
	
	
	typedef BOOL(WINAPI *pfnSetupDiCallClassInstaller)(IN DI_FUNCTION,IN HDEVINFO,IN PSP_DEVINFO_DATA);
	pfnSetupDiCallClassInstaller pfi = (pfnSetupDiCallClassInstaller)::GetProcAddress(m_hModuleDDK,"SetupDiCallClassInstaller");
	if(pfi == NULL)
		return FALSE;
	
	if (!pfi(DIF_PROPERTYCHANGE,m_hDevInfo, &DeviceInfoData)) 
	{ 	
		return FALSE; 
	} 		

	return TRUE; 
}

/*

void CSunnyCatDlg::ReplaceIATEntryInOneMod(PCSTR pszCalleeModName,PROC pfnCurrent,PROC pfnNew,HMODULE hmodCaller)
{
	ULONG ulSize;
	PIMAGE_IMPORT_DESCRIPTOR pImportDesc = 
	(PIMAGE_IMPORT_DESCRIPTOR)ImageDirectoryEntryToData(hmodCaller,TRUE,IMAGE_DIRECTORY_ENTRY_IMPORT,&ulSize);
	
	if(pImportDesc == NULL)
		return;
	
	for(; pImportDesc->Name; pImportDesc++)
	{
		PSTR pszModName = (PSTR)((PBYTE)hmodCaller + pImportDesc->Name);
		if(lstrcmpiA(pszModName,pszCalleeModName) == 0)
			break;
			
	}
	if(pImportDesc->Name == 0)
		return;
		
	PIMAGE_THUNK_DATA pThunk = (PIMAGE_THUNK_DATA)((PBYTE)hmodCaller + pImportDesc->FirstThunk);
	
	for(;pThunk->ul.Function;pThunk++)
	{
		PROC* ppfn = (PROC*)&pThunk->ul.Function;
		
		BOOL fFound = (*ppfn == pfnCurrent);
		
		if(fFound)
		{
			WriteProcessMemory(GetCurrentProcess(),ppfn,&pfnNew,sizeof(pfnNew),NULL);
			return;
		}
	}
}

*/

/*
*	原作者：依星(airen3339@163.com)
*	xuzhm更改
*/
UINT UpdateProgram( LPVOID pParam )
{
	HWND hMainWnd = reinterpret_cast< HWND >(pParam);
	CString strUrl;
	CString strFileName;
	CUpdateDownload UpdateDownload(hMainWnd);
	BOOL bGetVersion = FALSE;

	strUrl.Empty();
	strFileName.Empty();

	
	strUrl = ::LoadStringEx(IDS_UPDATE_URL);
	strFileName = GetAppPath()+TEXT("\\SC700.txt");

	::DeleteFile(strFileName);	//下载前先删除
	bGetVersion = UpdateDownload.InternetGetFile(strUrl,strFileName,g_EventStopUpdate);
	if(!bGetVersion)
	{
		//::PostMessage(hMainWnd,WM_UPDATE_FAIL,DOWNLOAD_FAIL,0);
		return 0;
	}

	int nLoop = 2;
	CString strCurrent = ::LoadStringEx(IDS_APPNAME);
//	if(strCurrent.Find("P_CA") >= 0)
//		nLoop = 2;
//	if(strCurrent.Find("P_EA") >= 0)
//		nLoop = 2;
	char filename[255]={0};
	UnicodeToASCII(strFileName,strFileName.GetLength(),filename,255);
	
	MyStream file(filename);
	char buf[1024]={0};
	CString str;
	for(size_t i=0; i<nLoop;i++)
	{
		str.Empty();
		do
		{
			memset(buf, 0, sizeof(buf));
			file.readline(buf,sizeof(buf)-1);
			str = buf;
		}while(str.IsEmpty()&& !file.eof());
	}
	

	//CString strCurrent = ::LoadStringEx(IDS_APPNAME);
	int nPos = strCurrent.ReverseFind('_');
	CString strCurVer = strCurrent.Mid(nPos+1);
	int nPos1 = str.Find(TEXT("##"));
	int nPos2 = str.Find(TEXT("##"),nPos1+2);
	int nPos3 = str.Find(TEXT("##"),nPos2+2);
	int nPos4 = str.Find(TEXT("##"),nPos3+2);

	if((nPos1 < 0) || (nPos2 < 0) || (nPos3 < 0) || (nPos4 < 0) )
		return 0;
	CString strDownLoadVersion = str.Mid(nPos2+2,nPos3-nPos2-2);
	CString strDownLoadUrl = str.Mid(nPos3+2,nPos4-nPos3-2);
	nPos = strDownLoadVersion.ReverseFind('_');
	if(nPos <0)
		return 0;
	strDownLoadVersion = strDownLoadVersion.Mid(nPos+1);
	nPos = strDownLoadUrl.ReverseFind('/');
	if(nPos <0)
		return 0;
	CString strDlFileName = strDownLoadUrl.Mid(nPos+1);
	strFileName = GetAppPath()+strDlFileName;
	if(strCurVer >= strDownLoadVersion)
	{
		::PostMessage(hMainWnd,WM_UPDATE_FAIL,VERSION_ERROR,0);
		return 1;
	}
	
	if(GMessageBox(LoadStringEx(IDS_UPDATE_CONFIRM),LoadStringEx(IDS_UPDATE_CAPTION),MB_OKCANCEL|MB_ICONQUESTION) == IDCANCEL)
		return 0;

	::PostMessage(hMainWnd,WM_UPDATE_BEG,0,0);

	bGetVersion = UpdateDownload.InternetGetFile(strDownLoadUrl,strFileName,g_EventStopUpdate);

	if(!bGetVersion)
	{
		::PostMessage(hMainWnd,WM_UPDATE_FAIL,DOWNLOAD_FAIL,0);
		return 0;
	}
	nPos = strDlFileName.ReverseFind('.');
	if(nPos < 0)
		return 0;
	strDlFileName = strDlFileName.Mid(0,nPos);
	strDlFileName += TEXT(".exe");
	strDlFileName = GetAppPath()+strDlFileName;

	::DeleteFile(strDlFileName);	//下载前先删除
	::PostMessage(hMainWnd,WM_UPDATE_END,0,0);
	if(GMessageBox(LoadStringEx(IDS_UPDATE_SETUP),LoadStringEx(IDS_UPDATE_CAPTION),MB_OKCANCEL|MB_ICONQUESTION) == IDCANCEL)
		return 0;
	::MoveFile(strFileName,strDlFileName);

	STARTUPINFO startupInfo;
	PROCESS_INFORMATION processInfo;
	memset (&startupInfo, 0, sizeof(STARTUPINFO));
	memset (&processInfo, 0, sizeof(PROCESS_INFORMATION));
	startupInfo.cb = sizeof(STARTUPINFO);
	
	

	if(!::CreateProcess(strDlFileName,NULL,NULL,NULL,FALSE,NULL,NULL,NULL,&startupInfo, &processInfo))
	{
		::PostMessage(hMainWnd,WM_UPDATE_FAIL,DOWLOADFILE_ERROR,0);
		return 0;
	}
	
	CloseHandle(processInfo.hThread);
	CloseHandle(processInfo.hProcess);

	::PostMessage(hMainWnd,WM_CLOSE,0,0);

	//::WaitForSingleObject(processInfo.hProcess,INFINITE);

	return 1;
}

void CSunnyCatDlg::OnAutoUpdate() 
{
	// TODO: Add your command handler code here
	m_bHandUpdate = TRUE;
	BegAutoUpdate();
	
}

void CSunnyCatDlg::BegAutoUpdate()
{	
	if(!UpdateThreadIsRun())
	{
		g_EventStopUpdate.ResetEvent();

		HWND hMainWnd = GetSafeHwnd();
		m_UpdateThread = AfxBeginThread(UpdateProgram,hMainWnd);

	//	::CloseHandle(m_UpdateThread->m_hThread);
	}
}

BOOL CSunnyCatDlg::UpdateThreadIsRun()
{
	if(m_UpdateThread == NULL)
		return FALSE;

	if(::WaitForSingleObject(m_UpdateThread->m_hThread,0) == WAIT_TIMEOUT)
		return TRUE;

	if(::WaitForSingleObject(m_UpdateThread->m_hThread,0) == WAIT_OBJECT_0)
	{
		CloseHandle(m_UpdateThread->m_hThread);
	}
	
	return FALSE;
}

LRESULT CSunnyCatDlg::OnUpdateFail(WPARAM wParam,LPARAM lParam)
{
	size_t unError = (size_t)wParam;
	CString strError;
	
	strError.Empty();

	switch(unError)
	{
	case DOWNLOAD_FAIL:
		strError = ::LoadStringEx(DOWNLOAD_FAIL);
		break;
	case VERSION_ERROR:
		strError = ::LoadStringEx(VERSION_ERROR);
		break;
	case DOWLOADFILE_ERROR:
		strError = ::LoadStringEx(DOWLOADFILE_ERROR);
		break;
	default:
		strError = TEXT("Error");
		break;
	}
	if(!m_bHandUpdate && unError==VERSION_ERROR)
	{
		//GMessageBox(strError);
	}
	else
	{
	GMessageBox(strError);
	}
	
	if(!UpdateThreadIsRun())
	{
		if(m_UpdateDlg != NULL)
		{
			m_UpdateDlg->DestroyWindow();
			delete m_UpdateDlg;
			m_UpdateDlg = NULL;
		}
	}
	

	return 1;
}

LRESULT CSunnyCatDlg::OnUpdateBeg(WPARAM wParam,LPARAM lParam)
{
	
	m_UpdateDlg = new CUpdateProgressDlg(g_EventStopUpdate,this); 
	ASSERT(m_UpdateDlg != NULL);

	CString strTitle;
	CString strCaption;

	strTitle = ::LoadStringEx(IDS_UPDATE_TITLE);
	strCaption = ::LoadStringEx(IDS_UPDATE_CAPTION);

	m_UpdateDlg->Create(this,IDD_PROGRESS);
	m_UpdateDlg->CenterWindow(this);
	m_UpdateDlg->ShowWindow(SW_NORMAL);

	return 1;
}

LRESULT CSunnyCatDlg::OnUpdateEnd(WPARAM wParam,LPARAM lParam)
{
	if(m_UpdateDlg != NULL)
	{
		m_UpdateDlg->DestroyWindow();
		delete m_UpdateDlg;
		m_UpdateDlg = NULL;
	}	
	
	return 1;
}

LRESULT CSunnyCatDlg::OnUpdateSetprogress(WPARAM wParam,LPARAM lParam)
{
	if(m_UpdateDlg != NULL)
	{
		size_t nProgress = (size_t)wParam;
		size_t nMax = (size_t)lParam;

		m_UpdateDlg->SetProgress(nProgress,nMax);
	}

	return 1;
}

void CSunnyCatDlg::EndDownloadThread()
{
	if(m_UpdateThread != NULL)
	{
		g_EventStopUpdate.SetEvent();
		DWORD dw = ::WaitForSingleObject(m_UpdateThread->m_hThread,2000);
		if(::WaitForSingleObject(m_UpdateThread->m_hThread,2000) == WAIT_TIMEOUT)
		{
			if(m_UpdateDlg != NULL)
			{
				m_UpdateDlg->DestroyWindow();
				delete m_UpdateDlg;
				m_UpdateDlg = NULL;
			}	

			TerminateThread(m_UpdateThread->m_hThread,0);
			CloseHandle(m_UpdateThread->m_hThread);
			m_UpdateThread = NULL;
		}
		else
		{
			if(m_UpdateDlg != NULL)
			{
				m_UpdateDlg->DestroyWindow();
				delete m_UpdateDlg;
				m_UpdateDlg = NULL;
			}	
		}
	}		
}

void CSunnyCatDlg::CalcNetFlux(CInfoConnect InfoCnt)
{
	CString strDate;
	CString strUncntNM;
	CString strWarnNM;
	CString strNm;
	BOOL bYN = TRUE;
	BOOL bUnCnt = FALSE;

	GetParaFromReg(bYN,bUnCnt,strDate,strUncntNM,strWarnNM,strNm);

	if(!bYN)
		return;
	
	int nRegDate = 1;
	nRegDate = _ttoi(strDate);
	CTime curtime = CTime::GetCurrentTime();
	int nYear = curtime.GetYear();
	int nMonth = curtime.GetMonth();
	int nDay = curtime.GetDay();

	CConnectRecord ConnectRecord;
	CString strTime = "";
	CString strFlux = "";

	ConnectRecord.GetAllRecords();

	if(nDay >= nRegDate)
	{
		CTime t(nYear,nMonth,nRegDate,0,0,0);
		ConnectRecord.GetStatTime(t, curtime, strTime, strFlux);
	}
	else
	{
		if(nMonth == 1)
		{
			CTime t(nYear-1,12,nRegDate,0,0,0);
			ConnectRecord.GetStatTime(t, curtime, strTime, strFlux);
		}
		else
		{
			CTime t(nYear,nMonth-1,nRegDate,0,0,0);
			ConnectRecord.GetStatTime(t, curtime, strTime, strFlux);
		}
	}
	DWORDLONG dwlTotalFlux = _ttoi64(strFlux) + InfoCnt.m_dwRecvTotal + InfoCnt.m_dwSendTotal;
	DWORDLONG dwlUncnt = _ttoi64(strUncntNM);
	DWORDLONG dwlWarn = _ttoi64(strWarnNM);
	
	if(dwlTotalFlux >= dwlWarn*1024*1024)
	{		
		if(bUnCnt)
		{
			if(dwlTotalFlux >= dwlUncnt*1024*1024)
			{
				if(!m_bFluxUncntDoModal)
				{
					m_bFluxUncntDoModal = TRUE;
					UnconnectNet();
					ShowFluxPrompWindow(::LoadStringEx(IDS_FLUX_UNCNTPROMPT));
					GMessageBox(::LoadStringEx(IDS_FLUX_UNCNT),::LoadStringEx(IDS_FLUX_UNCNTTITLE),MB_OK|MB_ICONEXCLAMATION);
					return;
					
				}
				
			}
		}
		
		if( !m_bFluxWarnDoModal )
		{
			m_bFluxWarnDoModal = TRUE;
			CString strText = "";
			CString strTextPrompt = "";
			if(bUnCnt)
			{
				strText.Format(::LoadStringEx(IDS_FLUX_SET),dwlTotalFlux/(1024*1024));
				strTextPrompt.Format(::LoadStringEx(IDS_FLUX_SETPROMPT),dwlTotalFlux/(1024*1024));
			}
			else
			{
				strText.Format(::LoadStringEx(IDS_FLUX_NOTSET),dwlTotalFlux/(1024*1024));
				strTextPrompt.Format(::LoadStringEx(IDS_FLUX_NOTSETPROMPT),dwlTotalFlux/(1024*1024));
			}


			ShowFluxPrompWindow(strTextPrompt);
			GMessageBox(strText,::LoadStringEx(IDS_FLUX_WARNTITLE),MB_OK|MB_ICONEXCLAMATION);		
		}
		else
		{
			DWORDLONG dwlNm = _ttoi64(strNm);
			static size_t snFluxCount = (dwlTotalFlux-dwlWarn*1024*1024)/(dwlNm*1024*1024);
			LONG64 dwlFlux = dwlTotalFlux-dwlWarn*1024*1024-dwlNm*snFluxCount*1024*1024;
			
			if(dwlFlux > dwlNm*1024*1024*2)
			{
				snFluxCount = (dwlTotalFlux-dwlWarn*1024*1024)/(dwlNm*1024*1024);
				dwlFlux = dwlTotalFlux-dwlWarn*1024*1024-dwlNm*snFluxCount*1024*1024;
			}
			if(dwlFlux > 0)
			{
				if(dwlFlux >= dwlNm*1024*1024)
				{
					snFluxCount++;
					CString strText = "";
					CString strTextPrompt = "";
					if(bUnCnt)
					{
						strText.Format(::LoadStringEx(IDS_FLUX_SET),dwlTotalFlux/(1024*1024));
						strTextPrompt.Format(::LoadStringEx(IDS_FLUX_SETPROMPT),dwlTotalFlux/(1024*1024));
					}
					else
					{
						strText.Format(::LoadStringEx(IDS_FLUX_NOTSET),dwlTotalFlux/(1024*1024));
						strTextPrompt.Format(::LoadStringEx(IDS_FLUX_NOTSETPROMPT),dwlTotalFlux/(1024*1024));
					}


					if(!m_bWarnNMdlg)
					{
						m_bWarnNMdlg = TRUE;
						ShowFluxPrompWindow(strTextPrompt);
						GMessageBox(strText,::LoadStringEx(IDS_FLUX_WARNTITLE),MB_OK|MB_ICONEXCLAMATION);
						
							m_bWarnNMdlg = FALSE;
						
						}
												
					}
				}
			else
			{
				snFluxCount = (dwlTotalFlux-dwlWarn*1024*1024)/(dwlNm*1024*1024);
			}
			
		}
	}
}

void CSunnyCatDlg::GetParaFromReg(BOOL& bYN,BOOL& bUnCnt,CString& strDate,CString& strUncntNM,CString& strWarnNM,CString& strNm )
{
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
	TCHAR szValue[1024] = {0};
	DWORD nPreCount = sizeof(szValue)-1;
	DWORD nCount = sizeof(szValue)-1;

	if(reg.QueryValue(szValue, TEXT("Date"), & nCount) == ERROR_SUCCESS)
	{
		strDate = szValue;
	}
	else
	{
		strDate = TEXT("1");
	}
	
	nCount = nPreCount;
	if(reg.QueryValue(szValue, TEXT("UncntNM"), & nCount) == ERROR_SUCCESS)
	{
		strUncntNM = szValue;
	}
	else
	{
		strUncntNM = TEXT("1000");
	}
	nCount = nPreCount;
	if(reg.QueryValue(szValue, TEXT("WarnNM"), & nCount) == ERROR_SUCCESS)
	{
		strWarnNM = szValue;
	}
	else
	{
		strWarnNM = TEXT("800");
	}

	nCount = nPreCount;
	if(reg.QueryValue(szValue, TEXT("NM"), & nCount) == ERROR_SUCCESS)
	{
		strNm = szValue;
	}
	else
	{
		strNm = TEXT("10");
	}

	DWORD dwValue = 0;
	if(reg.QueryValue(dwValue, TEXT("UseFluxStat")) == ERROR_SUCCESS)
	{
		if(dwValue >0 )
			bYN = TRUE;
		else
			bYN = FALSE;
	}
	else
	{
		bYN = TRUE;
	}

	if(reg.QueryValue(dwValue, TEXT("bUncnt")) == ERROR_SUCCESS)
	{
		if(dwValue >0 )
			bUnCnt = TRUE;
		else
			bUnCnt = FALSE;
	}
	else
	{
		bUnCnt = FALSE;
	}
	reg.Close();
}

void CSunnyCatDlg::UnconnectNet()
{
	if(CONNECT_CONTROL_NOT_CONNECT == m_MobileManager.GetConnectionStat())
	{
		return;
	}	

	OnConnectionStateChange(NET_DISCONNECTING);
    m_MobileManager.NetworkDisconnect();

	m_netDlg.PostMessage(WM_NETWORK_SHOWRATE, 0, 0);

	m_unconnect.EnableWindow(FALSE);
	m_unconnect.iPhoto = IMAG_DISABLECUT;
	m_unconnect.SetFocus();
	m_connect.EnableWindow(FALSE);
	m_connect.iPhoto = IMAG_DISABLECON;
	m_connect.SetFocus();	

	/********************
	m_unconnectevdo.EnableWindow(FALSE);
	m_unconnectevdo.iPhoto = IMAG_DISABLECUT;
	m_unconnectevdo.SetFocus();
	m_connectevdo.EnableWindow(FALSE);
	m_connectevdo.iPhoto = IMAG_DISABLECON;
	m_connectevdo.SetFocus();	
	/*********************/

	m_bConnected = TRUE;
	m_bUnconnected = TRUE;

	m_bFluxUncntDoModal = FALSE;
	m_bFluxWarnDoModal = FALSE;
	m_bWarnNMdlg = FALSE;

}

void CSunnyCatDlg::ShowFluxPrompWindow(CString strInfo)
{
//	strInfo.Remove('\n');
//	int nLength = strInfo.GetLength();
//	int i = 1;
//	while(nLength > 28)
//	{
//		strInfo.Insert(28*i," \n");
//		
//		nLength -= 28;
//		i++;
//	}
	//弹出提示窗口
	CDlgMsg& msgWindow = *CDlgMsg::CreateMsgDlg(GetFont());

	msgWindow.CreateMsgWindow();

	msgWindow.SetPromptMessage(strInfo);

	strInfo = LoadStringEx(IDS_USERINFO);
	msgWindow.SetPromptCaption(strInfo);

	CRect rect;
	msgWindow.GetWindowRect(&rect);
	msgWindow.SetWindowPos(&wndTopMost, 
		rect.left, rect.top, 
		rect.Width(), rect.Height(), 
		SWP_NOMOVE | SWP_NOSIZE | SWP_SHOWWINDOW);
	msgWindow.ShowWindow(SW_SHOW);

	msgWindow.UpdateWindow();

}
void CSunnyCatDlg::OnMenuAccelerate() 
{
	// TODO: Add your control notification handler code here
	CPoint pt ;	
	CMenu CardMenu, *pPopMenu = NULL ;
	GetCursorPos(&pt) ;
	CardMenu.LoadMenu(IDR_MENU_CN);
	pPopMenu = CardMenu.GetSubMenu(3) ;		
	if(pPopMenu != NULL)
	{
		if(!m_bMC5Enable)
		{
			pPopMenu->EnableMenuItem(1,MF_BYPOSITION|MF_GRAYED) ;
		}
		if(!m_bMC5Disable)
		{
			pPopMenu->EnableMenuItem(3,MF_BYPOSITION|MF_GRAYED) ;
			pPopMenu->EnableMenuItem(0,MF_BYPOSITION|MF_GRAYED) ;
			pPopMenu->EnableMenuItem(2,MF_BYPOSITION|MF_GRAYED) ;
			pPopMenu->EnableMenuItem(4,MF_BYPOSITION|MF_GRAYED) ;
		
		}

		pPopMenu->TrackPopupMenu(TPM_LEFTBUTTON,pt.x-30,pt.y+15,this) ;
	}
}

BOOL CALLBACK CSunnyCatDlg::EnumWindowsProc(HWND hwnd,LPARAM lParam)
{
	DWORD			dwProcessID			= 0;
	HANDLE			hSnapshot;
	MODULEENTRY32W	module				= {0};
	BOOL			bFound				= FALSE;
	wchar_t			pwcPath[MAX_PATH]	= {0};

	/* Find client from modle path
	*/
	GetWindowThreadProcessId(hwnd, &dwProcessID);
	hSnapshot = CreateToolhelp32Snapshot(TH32CS_SNAPMODULE, dwProcessID);

	module.dwSize = sizeof(module);

	if (Module32FirstW(hSnapshot, &module))
	{
		do
		{
			if(0 == StrCmpIW(m_pwcSDIPath, module.szExePath))
			{
				bFound = TRUE;
				m_MC5WindowHandle = hwnd;
			}
		}
		while (!bFound && Module32NextW(hSnapshot, &module));
    }

	CloseHandle (hSnapshot);
	return (!bFound);
}

void CSunnyCatDlg::OnCancel( )
{
	return;
}

void CSunnyCatDlg::SetFirstNetRegedit()
{
	CRegKey reg;
    LPCTSTR lpszKey =  GetConfigString(); 
    
	if(reg.Open(HKEY_LOCAL_MACHINE,lpszKey) != ERROR_SUCCESS)
	{
		reg.Create(HKEY_LOCAL_MACHINE,lpszKey);		
		if(reg.Open(HKEY_LOCAL_MACHINE,lpszKey) != ERROR_SUCCESS)
		{
			return ;
		}
	}

	DWORD dwSetNet = 0;
	if(reg.QueryValue(dwSetNet,TEXT("bSetNet")) != ERROR_SUCCESS)
	{
		dwSetNet = 0;
	}
	if(dwSetNet == 0)
	{
		reg.SetValue(TEXT(""),TEXT("UserName"));
		reg.SetValue(TEXT(""),TEXT("PassWord"));
		reg.SetValue(TEXT("*99#"),TEXT("Phone"));
		reg.SetValue(TEXT("WCDMA Wireless"),TEXT("Apn"));
		reg.SetValue(TEXT("WCDMA Wireless"),TEXT("ApnStr"));
		reg.SetValue(TEXT("``````*99#"),TEXT("WCDMA Wireless"));

		reg.SetValue((DWORD)0,TEXT("bRecordAudio"));
		reg.SetValue(1,TEXT("bSetNet"));
	}
	reg.Close();
}

//080825he**********************************//

void CSunnyCatDlg::InitDlg()
{
		CString strPath = GetAppPath();

	if(m_strLanguage == "-Ru")
	{

		m_LinkBtn.SetBitmapPath(strPath+"skin\\Russian\\BMP\\connect_no.bmp",strPath+"skin\\English\\BMP\\connect_down.bmp",strPath+"skin\\English\\BMP\\connect_on.bmp");
		m_NetBtn.SetBitmapPath(strPath+"skin\\Russian\\BMP\\net_no.bmp",strPath+"skin\\English\\BMP\\net_down.bmp",strPath+"skin\\English\\BMP\\net_on.bmp");
		m_CallBtn.SetBitmapPath(strPath+"skin\\Russian\\BMP\\call_no.bmp",strPath+"skin\\English\\BMP\\call_down.bmp",strPath+"skin\\English\\BMP\\call_on.bmp");
		m_SMSBtn.SetBitmapPath(strPath+"skin\\Russian\\BMP\\sms_no.bmp",strPath+"skin\\English\\BMP\\sms_down.bmp",strPath+"skin\\English\\BMP\\sms_on.bmp");
		m_LinkManBtn.SetBitmapPath(strPath+"skin\\Russian\\BMP\\notebook_no.bmp",strPath+"skin\\English\\BMP\\notebook_down.bmp",strPath+"skin\\English\\BMP\\notebook_on.bmp");
//		m_RecordBtn.SetBitmapPath(strPath+"skin\\English\\BMP\\record_no.bmp",strPath+"skin\\English\\BMP\\record_down.bmp",strPath+"skin\\English\\BMP\\record_on.bmp");
/*
		m_LinkBtn.SetBitmapPath(strPath+"skin\\other\\link.bmp");
		m_NetBtn.SetBitmapPath(strPath+"skin\\other\\rate.bmp");
		m_CallBtn.SetBitmapPath(strPath+"skin\\other\\call.bmp");
		m_SMSBtn.SetBitmapPath(strPath+"skin\\other\\sms.bmp");
		m_LinkManBtn.SetBitmapPath(strPath+"skin\\other\\phonebook.bmp");
		m_ctrlFileBtn.SetBitmapPath(strPath+"skin\\other\\setting.bmp");
*/
		m_ctrlFileBtn.SetBitmapPath(strPath+"Skin\\Russian\\BMP\\set_no.bmp",strPath+"Skin\\English\\BMP\\set_down.bmp",strPath+"Skin\\English\\BMP\\set_on.bmp");
		m_OperationBtn.SetBitmapPath(strPath+"Skin\\Russian\\BMP\\oper_no.bmp",strPath+"Skin\\English\\BMP\\oper_down.bmp",strPath+"Skin\\English\\BMP\\oper_over.bmp");
		m_ctrlContactBtn.SetBitmapPath(strPath+"Skin\\Russian\\BMP\\tool_no.bmp",strPath+"Skin\\English\\BMP\\tool_down.bmp",strPath+"Skin\\English\\BMP\\tool_over.bmp");
		m_ctrlHelpBtn.SetBitmapPath(strPath+"Skin\\Russian\\BMP\\help_no.bmp",strPath+"Skin\\English\\BMP\\help_down.bmp",strPath+"Skin\\English\\BMP\\help_over.bmp"); 	
	
//		m_notRegister.SetBitmap(LoadBmpFormFile(TEXT("skin\\other\\notRegister_ch.bmp")));
	}
	else if(m_strLanguage == "-En")
	{
		m_LinkBtn.SetBitmapPath(strPath+"skin\\English\\BMP\\connect_no.bmp",strPath+"skin\\English\\BMP\\connect_down.bmp",strPath+"skin\\English\\BMP\\connect_on.bmp");
		m_NetBtn.SetBitmapPath(strPath+"skin\\English\\BMP\\net_no.bmp",strPath+"skin\\English\\BMP\\net_down.bmp",strPath+"skin\\English\\BMP\\net_on.bmp");
		m_CallBtn.SetBitmapPath(strPath+"skin\\English\\BMP\\call_no.bmp",strPath+"skin\\English\\BMP\\call_down.bmp",strPath+"skin\\English\\BMP\\call_on.bmp");
		m_SMSBtn.SetBitmapPath(strPath+"skin\\English\\BMP\\sms_no.bmp",strPath+"skin\\English\\BMP\\sms_down.bmp",strPath+"skin\\English\\BMP\\sms_on.bmp");
		m_LinkManBtn.SetBitmapPath(strPath+"skin\\English\\BMP\\notebook_no.bmp",strPath+"skin\\English\\BMP\\notebook_down.bmp",strPath+"skin\\English\\BMP\\notebook_on.bmp");
	//	m_RecordBtn.SetBitmapPath(strPath+"skin\\English\\BMP\\record_no.bmp",strPath+"skin\\English\\BMP\\record_down.bmp",strPath+"skin\\English\\BMP\\record_on.bmp");
/*
		m_LinkBtn.SetBitmapPath(strPath+"skin\\other\\link.bmp");
		m_NetBtn.SetBitmapPath(strPath+"skin\\other\\rate.bmp");
		m_CallBtn.SetBitmapPath(strPath+"skin\\other\\call.bmp");
		m_SMSBtn.SetBitmapPath(strPath+"skin\\other\\sms.bmp");
		m_LinkManBtn.SetBitmapPath(strPath+"skin\\other\\phonebook.bmp");
		m_ctrlFileBtn.SetBitmapPath(strPath+"skin\\other\\setting.bmp");
*/
		m_ctrlFileBtn.SetBitmapPath(strPath+"Skin\\English\\BMP\\set_no.bmp",strPath+"Skin\\English\\BMP\\set_down.bmp",strPath+"Skin\\English\\BMP\\set_on.bmp");
		m_OperationBtn.SetBitmapPath(strPath+"Skin\\English\\BMP\\oper_no.bmp",strPath+"Skin\\English\\BMP\\oper_down.bmp",strPath+"Skin\\English\\BMP\\oper_over.bmp");
		m_ctrlContactBtn.SetBitmapPath(strPath+"Skin\\English\\BMP\\tool_no.bmp",strPath+"Skin\\English\\BMP\\tool_down.bmp",strPath+"Skin\\English\\BMP\\tool_over.bmp");
		m_ctrlHelpBtn.SetBitmapPath(strPath+"Skin\\English\\BMP\\help_no.bmp",strPath+"Skin\\English\\BMP\\help_down.bmp",strPath+"Skin\\English\\BMP\\help_over.bmp");		
		
//		m_notRegister.SetBitmap(LoadBmpFormFile(TEXT("skin\\other\\notRegister_en.bmp")));
	}

	m_netGsm.SetBitmap(LoadBmpFormFile(TEXT("skin\\other\\GSM.bmp")));
	m_netGprs.SetBitmap(LoadBmpFormFile(TEXT("skin\\other\\GPRS.bmp")));
	m_netEdge.SetBitmap(LoadBmpFormFile(TEXT("skin\\other\\EDGE.bmp")));
	m_roaming.SetBitmap(LoadBmpFormFile(TEXT("skin\\other\\roaming.bmp")));
	m_staOnLine.SetBitmap(LoadBmpFormFile(TEXT("skin\\other\\Online.bmp")));
	m_imgSMSPromt.SetBitmap(LoadBmpFormFile(TEXT("skin\\other\\smsPrompt.bmp")));

	m_s0.SetBitmap(LoadBmpFormFile(TEXT("skin\\other\\Signal-0.bmp")));
	m_s1.SetBitmap(LoadBmpFormFile(TEXT("skin\\other\\Signal-1.bmp")));
	m_s2.SetBitmap(LoadBmpFormFile(TEXT("skin\\other\\Signal-2.bmp")));
	m_s3.SetBitmap(LoadBmpFormFile(TEXT("skin\\other\\Signal-3.bmp")));
	m_s4.SetBitmap(LoadBmpFormFile(TEXT("skin\\other\\Signal-4.bmp")));
	m_s5.SetBitmap(LoadBmpFormFile(TEXT("skin\\other\\Signal-5.bmp")));

	m_s0_h.SetBitmap(LoadBmpFormFile(TEXT("skin\\other\\Signal-0.bmp")));
	m_s1_h.SetBitmap(LoadBmpFormFile(TEXT("skin\\other\\Signal-1.bmp")));
	m_s2_h.SetBitmap(LoadBmpFormFile(TEXT("skin\\other\\Signal-2.bmp")));
	m_s3_h.SetBitmap(LoadBmpFormFile(TEXT("skin\\other\\Signal-3.bmp")));
	m_s4_h.SetBitmap(LoadBmpFormFile(TEXT("skin\\other\\Signal-4.bmp")));
	m_s5_h.SetBitmap(LoadBmpFormFile(TEXT("skin\\other\\Signal-5.bmp")));

	m_Line1.SetBitmap(LoadBmpFormFile(TEXT("skin\\other\\separator.bmp")));
	m_Line2.SetBitmap(LoadBmpFormFile(TEXT("skin\\other\\separator.bmp")));
	m_Line3.SetBitmap(LoadBmpFormFile(TEXT("skin\\other\\separator.bmp")));
	m_NotOnLine.SetBitmap(LoadBmpFormFile(TEXT("skin\\other\\NotOnline.bmp")));

	m_Line1.ShowWindow(SW_HIDE);
	m_ctrlInfoBar.SetTextColor(RGB(255,255,255));
	m_sEvdo.SetTextColor(RGB(255,255,255));
	
	m_ctrlFileBtn.SetHitOver();
	m_OperationBtn.SetHitOver();
	m_ctrlContactBtn.SetHitOver();
	m_ctrlHelpBtn.SetHitOver();

	m_CallBtn.SetHitOver();
	m_SMSBtn.SetHitOver();
	m_NetBtn.SetHitOver();
	m_LinkBtn.SetHitOver();
	m_LinkManBtn.SetHitOver();
//	m_RecordBtn.SetHitOver();


	CRect rc;
	int y;
	m_NetBtn.GetWindowRect(rc);
	ScreenToClient(rc);
	y=rc.bottom-1;

	m_LinkDlg.Create(this,IDD_DIALOG_OTHER);
	m_LinkDlg.GetClientRect(rc);
//	m_LinkDlg.MoveWindow(0,y,rc.Width(),rc.Height(),TRUE);
/******************************************************/// modify ui
	m_LinkDlg.MoveWindow(110, 0, 431, 251,TRUE);
/******************************************************/
	m_LinkDlg.ShowWindow(SW_SHOW);

	m_netDlg.Create(this,IDD_DIALOG_NETWORK);
	m_netDlg.GetClientRect(rc);	
//	m_netDlg.MoveWindow(0,y,rc.Width(),rc.Height(),TRUE);
/******************************************************/// modify ui
	m_netDlg.MoveWindow(110, 0, 431, 251,TRUE);
/******************************************************/
	m_netDlg.ShowWindow(SW_HIDE);

	m_callDlg.Create(this,IDD_DIALOG_CALL);
//	m_callDlg.MoveWindow(0,y,rc.Width(),rc.Height(),TRUE);
/******************************************************/// modify ui
	m_callDlg.MoveWindow(110, 0, 431, 251,TRUE);
/******************************************************/
	m_callDlg.ShowWindow(SW_HIDE);

	m_smsDlg.Create(this,IDD_DIALOG_SMS);
//	m_smsDlg.MoveWindow(0,y,rc.Width(),rc.Height(),TRUE);
/******************************************************/// modify ui
	m_smsDlg.MoveWindow(110, 0, 431, 251,TRUE);
/******************************************************/
	m_smsDlg.ShowWindow(SW_HIDE);

	m_linkManDlg.Create(this,IDD_DIALOG_LINKMAN);
//	m_linkManDlg.MoveWindow(0,y,rc.Width(),rc.Height(),TRUE);
/******************************************************/// modify ui
	m_linkManDlg.MoveWindow(110, 0, 431, 251,TRUE);
/******************************************************/
	m_linkManDlg.ShowWindow(SW_HIDE);

/*	m_recordDlg.Create(this,IDD_DIALOG_RECORD);
	m_recordDlg.MoveWindow(0,y,rc.Width(),rc.Height(),TRUE);
	m_recordDlg.ShowWindow(SW_HIDE);*/
	
}


void CSunnyCatDlg::SetHold(CColorBtn &button)
{
	m_CallBtn.SetHold(false);
	m_SMSBtn.SetHold(false);
	m_NetBtn.SetHold(false);
	m_LinkBtn.SetHold(false);
	m_LinkManBtn.SetHold(false);
//	m_RecordBtn.SetHold(false);

	m_CallBtn.SetHitOver(true);
	m_SMSBtn.SetHitOver(true);
	m_NetBtn.SetHitOver(true);
	m_LinkBtn.SetHitOver(true);
	m_LinkManBtn.SetHitOver(true);
//	m_RecordBtn.SetHitOver(true);
	button.SetHitOver(false);
	button.SetHold(true);
}

void CSunnyCatDlg::OnButtonLink() 
{
	// TODO: Add your control notification handler code here
	SetHold(m_LinkBtn);
	m_LinkDlg.ShowWindow(SW_SHOW);
	m_smsDlg.ShowWindow(SW_HIDE);
	m_netDlg.ShowWindow(SW_HIDE);
	m_callDlg.ShowWindow(SW_HIDE);
	m_linkManDlg.ShowWindow(SW_HIDE);
//	m_recordDlg.ShowWindow(SW_HIDE);

	m_OperatonMenu.CheckMenuItem(IDM_OPREATION_LINK,MF_CHECKED);
	m_OperatonMenu.CheckMenuItem(IDM_OPERATION_NETWORK,MF_UNCHECKED);
	m_OperatonMenu.CheckMenuItem(IDM_OPERATION_CALL,MF_UNCHECKED);
	m_OperatonMenu.CheckMenuItem(IDM_OPERATION_SMS,MF_UNCHECKED);
	m_OperatonMenu.CheckMenuItem(IDM_OPERATION_PB,MF_UNCHECKED);
}

void CSunnyCatDlg::OnButtonNet() 
{
	// TODO: Add your control notification handler code here
	SetHold(m_NetBtn);
	m_LinkDlg.ShowWindow(SW_HIDE);
	m_netDlg.ShowWindow(SW_SHOW);
	m_callDlg.ShowWindow(SW_HIDE);
	m_smsDlg.ShowWindow(SW_HIDE);
	m_linkManDlg.ShowWindow(SW_HIDE);
//	m_recordDlg.ShowWindow(SW_HIDE);

	m_OperatonMenu.CheckMenuItem(IDM_OPREATION_LINK,MF_UNCHECKED);
	m_OperatonMenu.CheckMenuItem(IDM_OPERATION_NETWORK,MF_CHECKED);
	m_OperatonMenu.CheckMenuItem(IDM_OPERATION_CALL,MF_UNCHECKED);
	m_OperatonMenu.CheckMenuItem(IDM_OPERATION_SMS,MF_UNCHECKED);
	m_OperatonMenu.CheckMenuItem(IDM_OPERATION_PB,MF_UNCHECKED);
}

void CSunnyCatDlg::OnButtonCall() 
{
	// TODO: Add your control notification handler code here
	SetHold(m_CallBtn);
	m_LinkDlg.ShowWindow(SW_HIDE);
	m_callDlg.ShowWindow(SW_SHOW);
	m_netDlg.ShowWindow(SW_HIDE);
	m_smsDlg.ShowWindow(SW_HIDE);
	m_linkManDlg.ShowWindow(SW_HIDE);
//	m_recordDlg.ShowWindow(SW_HIDE);

	m_OperatonMenu.CheckMenuItem(IDM_OPREATION_LINK,MF_UNCHECKED);
	m_OperatonMenu.CheckMenuItem(IDM_OPERATION_NETWORK,MF_UNCHECKED);
	m_OperatonMenu.CheckMenuItem(IDM_OPERATION_CALL,MF_CHECKED);
	m_OperatonMenu.CheckMenuItem(IDM_OPERATION_SMS,MF_UNCHECKED);
	m_OperatonMenu.CheckMenuItem(IDM_OPERATION_PB,MF_UNCHECKED);
}

void CSunnyCatDlg::OnButtonSms() 
{
	// TODO: Add your control notification handler code here
	SetHold(m_SMSBtn);
	m_LinkDlg.ShowWindow(SW_HIDE);
	m_smsDlg.ShowWindow(SW_SHOW);
	m_netDlg.ShowWindow(SW_HIDE);
	m_callDlg.ShowWindow(SW_HIDE);
	m_linkManDlg.ShowWindow(SW_HIDE);
	//m_recordDlg.ShowWindow(SW_HIDE);

	m_OperatonMenu.CheckMenuItem(IDM_OPREATION_LINK,MF_UNCHECKED);
	m_OperatonMenu.CheckMenuItem(IDM_OPERATION_NETWORK,MF_UNCHECKED);
	m_OperatonMenu.CheckMenuItem(IDM_OPERATION_CALL,MF_UNCHECKED);
	m_OperatonMenu.CheckMenuItem(IDM_OPERATION_SMS,MF_CHECKED);
	m_OperatonMenu.CheckMenuItem(IDM_OPERATION_PB,MF_UNCHECKED);
}

void CSunnyCatDlg::OnButtonLinkman() 
{
	// TODO: Add your control notification handler code here
	SetHold(m_LinkManBtn);
	m_LinkDlg.ShowWindow(SW_HIDE);
	m_smsDlg.ShowWindow(SW_HIDE);
	m_netDlg.ShowWindow(SW_HIDE);
	m_callDlg.ShowWindow(SW_HIDE);
	m_linkManDlg.ShowWindow(SW_SHOW);
	//m_recordDlg.ShowWindow(SW_HIDE);

	m_OperatonMenu.CheckMenuItem(IDM_OPREATION_LINK,MF_UNCHECKED);
	m_OperatonMenu.CheckMenuItem(IDM_OPERATION_NETWORK,MF_UNCHECKED);
	m_OperatonMenu.CheckMenuItem(IDM_OPERATION_CALL,MF_UNCHECKED);
	m_OperatonMenu.CheckMenuItem(IDM_OPERATION_SMS,MF_UNCHECKED);
	m_OperatonMenu.CheckMenuItem(IDM_OPERATION_PB,MF_CHECKED);
}

void CSunnyCatDlg::OnButtonRecord() 
{
	// TODO: Add your control notification handler code here
//	SetHold(m_RecordBtn);
	m_LinkDlg.ShowWindow(SW_HIDE);
	m_smsDlg.ShowWindow(SW_HIDE);
	m_netDlg.ShowWindow(SW_HIDE);
	m_callDlg.ShowWindow(SW_HIDE);
	m_linkManDlg.ShowWindow(SW_HIDE);
//	m_recordDlg.ShowWindow(SW_SHOW);
}

void CSunnyCatDlg::InitIconState()
{
	m_s0.ShowWindow(SW_SHOW);
	m_s0_h.ShowWindow(SW_HIDE);
	m_NotOnLine.ShowWindow(SW_SHOW);
//	m_notRegister.ShowWindow(SW_SHOW);
	
	m_s1.ShowWindow(SW_HIDE);
	m_s2.ShowWindow(SW_HIDE);
	m_s3.ShowWindow(SW_HIDE);
	m_s4.ShowWindow(SW_HIDE);
	m_s5.ShowWindow(SW_HIDE);

	m_s1_h.ShowWindow(SW_HIDE);
	m_s2_h.ShowWindow(SW_HIDE);
	m_s3_h.ShowWindow(SW_HIDE);
	m_s4_h.ShowWindow(SW_HIDE);
	m_s5_h.ShowWindow(SW_HIDE);

	m_netGsm.ShowWindow(SW_HIDE);
	m_netGprs.ShowWindow(SW_HIDE);
	m_netEdge.ShowWindow(SW_HIDE);
}

void CSunnyCatDlg::ShowPage(int iPage)
{
	switch(iPage)
	{
		case 0:
			OnButtonLink();
			break;
		case 1:
			OnButtonNet();
			break;
		case 2:
			OnButtonCall();
			break;
		case 3:
			OnButtonSms();
			break;
		case 4:
			OnButtonLinkman();
			break;
		case 5:			
			OnButtonRecord();
			break;	
		default:
			break;

	}
	return;
	

}

void CSunnyCatDlg::DispTextInfobar(CString strText)
{
	
//	m_ctrlInfoBar.SetLabelText(strText);
	
	CRect rc;
	m_ctrlInfoBar.GetWindowRect(rc);
	::MapWindowPoints( HWND_DESKTOP, GetSafeHwnd(), (LPPOINT)(RECT*)&rc, 2 );
	InvalidateRect(rc);
	m_ctrlInfoBar.SetWindowText(strText);
	
	strShowText = strText;
	bShow = true;

}

void CSunnyCatDlg::CreateOperationMenu()
{
	m_OperatonMenu.CreatePopupMenu();
	
	m_OperatonMenu.AppendMenu(MF_STRING,IDM_OPREATION_LINK,LoadStringEx(IDS_LINK));
	m_OperatonMenu.AppendMenu(MF_STRING,IDM_OPERATION_NETWORK,LoadStringEx(IDS_STAT));	 
	m_OperatonMenu.AppendMenu(MF_STRING,IDM_OPERATION_CALL,LoadStringEx(IDS_CALL));
	m_OperatonMenu.AppendMenu(MF_STRING,IDM_OPERATION_SMS,LoadStringEx(IDS_SMS));	 
	m_OperatonMenu.AppendMenu(MF_STRING,IDM_OPERATION_PB,LoadStringEx(IDS_PB));
}

void CSunnyCatDlg::OnMenuOperation() 
{
	// TODO: Add your control notification handler code here
	CRect rc;
	m_OperationBtn.GetWindowRect(rc);
	m_OperatonMenu.TrackPopupMenu(TPM_LEFTBUTTON,rc.left,rc.bottom,this);
}

void CSunnyCatDlg::OnOperationLink()
{
	OnButtonLink();
	m_OperatonMenu.CheckMenuItem(IDM_OPREATION_LINK,MF_CHECKED);
	m_OperatonMenu.CheckMenuItem(IDM_OPERATION_NETWORK,MF_UNCHECKED);
	m_OperatonMenu.CheckMenuItem(IDM_OPERATION_CALL,MF_UNCHECKED);
	m_OperatonMenu.CheckMenuItem(IDM_OPERATION_SMS,MF_UNCHECKED);
	m_OperatonMenu.CheckMenuItem(IDM_OPERATION_PB,MF_UNCHECKED);
}
void CSunnyCatDlg::OnOperationNetWork()
{
	OnButtonNet();
	m_OperatonMenu.CheckMenuItem(IDM_OPREATION_LINK,MF_UNCHECKED);
	m_OperatonMenu.CheckMenuItem(IDM_OPERATION_NETWORK,MF_CHECKED);
	m_OperatonMenu.CheckMenuItem(IDM_OPERATION_CALL,MF_UNCHECKED);
	m_OperatonMenu.CheckMenuItem(IDM_OPERATION_SMS,MF_UNCHECKED);
	m_OperatonMenu.CheckMenuItem(IDM_OPERATION_PB,MF_UNCHECKED);
}
void CSunnyCatDlg::OnOperationCall()
{
	OnButtonCall();
	m_OperatonMenu.CheckMenuItem(IDM_OPREATION_LINK,MF_UNCHECKED);
	m_OperatonMenu.CheckMenuItem(IDM_OPERATION_NETWORK,MF_UNCHECKED);
	m_OperatonMenu.CheckMenuItem(IDM_OPERATION_CALL,MF_CHECKED);
	m_OperatonMenu.CheckMenuItem(IDM_OPERATION_SMS,MF_UNCHECKED);
	m_OperatonMenu.CheckMenuItem(IDM_OPERATION_PB,MF_UNCHECKED);
}
void CSunnyCatDlg::OnOperationSMS()
{
	OnButtonSms();
	m_OperatonMenu.CheckMenuItem(IDM_OPREATION_LINK,MF_UNCHECKED);
	m_OperatonMenu.CheckMenuItem(IDM_OPERATION_NETWORK,MF_UNCHECKED);
	m_OperatonMenu.CheckMenuItem(IDM_OPERATION_CALL,MF_UNCHECKED);
	m_OperatonMenu.CheckMenuItem(IDM_OPERATION_SMS,MF_CHECKED);
	m_OperatonMenu.CheckMenuItem(IDM_OPERATION_PB,MF_UNCHECKED);
}
void CSunnyCatDlg::OnOperationPB()
{	
	OnButtonLinkman();
	m_OperatonMenu.CheckMenuItem(IDM_OPREATION_LINK,MF_UNCHECKED);
	m_OperatonMenu.CheckMenuItem(IDM_OPERATION_NETWORK,MF_UNCHECKED);
	m_OperatonMenu.CheckMenuItem(IDM_OPERATION_CALL,MF_UNCHECKED);
	m_OperatonMenu.CheckMenuItem(IDM_OPERATION_SMS,MF_UNCHECKED);
	m_OperatonMenu.CheckMenuItem(IDM_OPERATION_PB,MF_CHECKED);
}

BOOL CSunnyCatDlg::OnEraseBkgnd(CDC* pDC) 
{
	// TODO: Add your message handler code here and/or call default
	return TRUE;
	return CDialog::OnEraseBkgnd(pDC);
}

void CSunnyCatDlg::ChgAutoRegdit()
{
	CRegKey reg;
	
	if(reg.Open(HKEY_LOCAL_MACHINE,TEXT("Software\\WCDMA Modem\\WCDMA"))!=ERROR_SUCCESS)
	{
		reg.Create(HKEY_LOCAL_MACHINE,TEXT("Software\\WCDMA Modem\\WCDMA"));		
		if(reg.Open(HKEY_LOCAL_MACHINE,TEXT("Software\\WCDMA Modem\\WCDMA")) != ERROR_SUCCESS)
		{
			return ;
		}
	}
	
	reg.SetValue(DWORD(0),TEXT("bAutoRun"));
	
	reg.Close();
}

LRESULT CSunnyCatDlg::OnSimReadResult(WPARAM wParam,LPARAM lParam)
{
	bool bSimRead = static_cast<bool>(wParam);
	TRACE(TEXT("CSunnyCatDlg::OnSimReadResult"));
	m_linkManDlg.m_pbookEmbed.SetReadResult(bSimRead);
	if(!bSimRead)
	{
		m_linkManDlg.SetNewBtnState(true);
		GMessageBox(::LoadStringEx(IDS_SIMREAD_RESULT))	;
	}
	else
	{
		m_linkManDlg.SetNewBtnState(false);
	}

	return 0;
}
 
void CSunnyCatDlg::ClearSimPB()
{
	TRACE(TEXT("CSunnyCatDlg::ClearSimPB"));
	m_linkManDlg.m_pbookEmbed.SetReadResult(false);
	if(m_linkManDlg.m_currTreeItemData == LINKMAN_SIM)
	{
		m_linkManDlg.m_ctrlLinkmanList.SetRedraw(false);
		m_linkManDlg.m_ctrlLinkmanList.DeleteAllItems_EX();
		m_linkManDlg.m_ctrlLinkmanList.SetRedraw(true);
		CString strText = m_linkManDlg.m_ctrlLinkManTree.GetItemText(m_linkManDlg.m_currTreeItem);
		int nFind = strText.Find(TEXT('('));
		CString strDisp = strText.Left(nFind+1)+TEXT("0")+")";
		m_linkManDlg.m_ctrlLinkManTree.SetItemText(m_linkManDlg.m_currTreeItem,strDisp);

		m_linkManDlg.m_pbookEmbed.Clean();

		m_linkManDlg.SetNewBtnState(true);
		m_linkManDlg.SetEditBtnState(true);
		m_linkManDlg.SetDelBtnState(true);
		m_linkManDlg.SetSendBtnState(true);
	}
}

LRESULT CSunnyCatDlg::OnInitStart(WPARAM wparam,LPARAM lParam)
{
	ChgAutoRegdit();
	m_bIsResetCom = false;
	if(!m_bResetting)
	{
//		InitProgress();
	}
	m_LinkDlg.SetBtnStatus(STATUS_CONNECT);
	m_linktype = NETTYPE_NOSERVICE;

	return 0;
}

LRESULT CSunnyCatDlg::OnInitEnd(WPARAM wParam,LPARAM lParam)
{
	SetTimer(TIMER_CHECK_SIGNAL, 8000, 0);//设置信号检测定时器

	TRACE(TEXT("CSunnyCatDlg::OnInitEnd"));
	
	CPortList portList;
	CString ComName;
	for(size_t i=0; i<portList.Count(); ++i)
	{
		const TPortInfo& info = portList.GetAt(i);
//		for(int nID=0; true; ++nID)
//		{
			const CString strHardwareID = "rev_0000&mi_01";//GetHardwareID(nID);//"Rev_0000&MI_03";
//			if(strHardwareID == "")break;
			CString strID = info.strHardwareID;
			strID.MakeLower();
			
			if(strID.Find(strHardwareID)!=-1)
			{
				ComName = info.strCOM;
				break;
			}
//		}
	}
	if(m_bPCMVoice)
	{
		m_pWaveLib = NMWave::CreateWaveLib(ToMultiBytes(LPCTSTR(ComName)).c_str(), 115200);
		if(m_pWaveLib != NULL)
		{
			/*
			m_pWaveLib->GetAudioInput().Open();
			m_pWaveLib->GetSerialInput().Open();
			m_pWaveLib->GetAudioOutPut().Open();
			m_pWaveLib->GetSerialOutput().Open();
			*/
			bool bOK = m_pWaveLib != NULL;
			TRACE(TEXT("1>bOK = %d"),bOK);
			if(bOK) bOK = m_pWaveLib->GetAudioInput().Open();
			TRACE(TEXT("2>bOK = %d"),bOK);
			if(bOK) bOK = m_pWaveLib->GetSerialInput().Open();
			TRACE(TEXT("3>bOK = %d"),bOK);
			if(bOK) bOK = m_pWaveLib->GetAudioOutPut().Open();
			TRACE(TEXT("4>bOK = %d"),bOK);
			if(bOK) bOK = m_pWaveLib->GetSerialOutput().Open();
			TRACE(TEXT("5>bOK = %d"),bOK);

			
			//启动设备
//			m_pWaveLib->GetAudioInput().Start();
//			m_pWaveLib->GetSerialInput().Start();
//			m_pWaveLib->GetAudioOutPut().Start();
//			m_pWaveLib->GetSerialOutput().Start();
			
//			GMessageBox(::LoadStringEx(IDS_NO_DATABASE));  //
			CRegKey reg;
			
			CString regPath = GetConfigString();
			
			if(reg.Open(HKEY_LOCAL_MACHINE, regPath) != ERROR_SUCCESS) //打开成功
			{
				reg.Create(HKEY_LOCAL_MACHINE, regPath);		
				if(reg.Open(HKEY_LOCAL_MACHINE, regPath) != ERROR_SUCCESS)
				{
					return 0;
				}
			}
			DWORD dwValue = 0;
			
			if(reg.QueryValue(dwValue, TEXT("bRecordAudio")) == ERROR_SUCCESS)
			{
				if(dwValue >0 )
				{
					std::string str = ToMultiBytes(LPCTSTR(GetAppPath()+TEXT("Audio.wav"))).c_str();
					
					m_pRecorder = m_pWaveLib->CreateRecorder(str);
					NMWave::IWaveIn* pWaveIn = &m_pWaveLib->GetAudioInput();
					pWaveIn->AddListener(*m_pRecorder);//加入到音源中
					m_pRecorder->Open();//打开录音机
					m_pRecorder->Start();//启动录音
					
					str = ToMultiBytes(LPCTSTR(GetAppPath()+TEXT("Serial.wav"))).c_str();
					m_pRecorderSerial = m_pWaveLib->CreateRecorder(str);
					NMWave::IWaveIn* pWaveInSerial = &m_pWaveLib->GetSerialInput();
					pWaveInSerial->AddListener(*m_pRecorderSerial);//加入到音源中
					m_pRecorderSerial->Open();//打开录音机
					m_pRecorderSerial->Start();//启动录音
				}
				
			}
			reg.Close();
		}
	}

	TRACE(TEXT("CSunnyCatDlg::OnInitEnd:>m_bResetting = %d\n"), m_bResetting);
	if(!m_bResetting)
	{
//		EndProgress();		
	}
	m_bResetting = false;
	m_bfirstmove = false;
	
	TRACE(TEXT("m_bfirstmove = %d\n"), m_bfirstmove);
	TRACE(TEXT("!m_bIsLockUIM = %d\n"),  !m_bIsLockUIM);
	TRACE(TEXT("m_MobileManager.m_bSimExist = %d\n"), m_MobileManager.m_bSimExist);

	FlashInfoBar("");
	TRACE(TEXT("CSunnyCatDlg::OnInitEnd:>end"));
	return 0;
}

LRESULT CSunnyCatDlg::OnInitError(WPARAM wParam,LPARAM lParam)
{
	TRACE(TEXT("CSunnyCatDlg::OnInitError :> START \n"));
	size_t nCase = static_cast<size_t>(wParam);
	switch(nCase)
	{
	case 1:
		{
			CString strError = m_MobileManager.GetErrorMsg();
//			EndProgress();
//			GMessageBox(strError);			// modify by lab686 20091217
			DispTextInfobar(strError);		// add by lab686 20091217
		}
		break;
	case 2:
		{
//			EndProgress();
			GMessageBox(LoadStringEx(IDS_SIM_LOCK));
			DispTextInfobar(LoadStringEx(IDS_SIM_LOCK));
		}
		break;
	default:
		break;
	}
	m_bIsLockUIM   = false;
	m_bfirstmove = true;
	
	KillTimer(TIMER_CHECK_SIGNAL);

	TRACE(TEXT("m_bfirstmove = %d\n"), m_bfirstmove);
	TRACE(TEXT("!m_bIsLockUIM = %d\n"),  !m_bIsLockUIM);
	TRACE(TEXT("m_MobileManager.m_bSimExist = %d\n"), m_MobileManager.m_bSimExist);

	TRACE(TEXT("CSunnyCatDlg::OnInitError :> END \n"));
	return 0;
}

BOOL CSunnyCatDlg::PreCreateWindow(CREATESTRUCT& cs) 
{
	// TODO: Add your specialized code here and/or call the base class
	return CDialog::PreCreateWindow(cs);
	/************************************************************************
	if( !CDialog::PreCreateWindow(cs) )
   		return FALSE;

	cs.style&=~WS_THICKFRAME; //窗口固定大小
//	cs.style&=~FWS_ADDTOTITLE; //去掉无标题
	cs.style&=~(WS_MAXIMIZEBOX); //去最大＼最小按扭

	return TRUE;
	/************************************************************************/
}

LRESULT CSunnyCatDlg::OnSMSRefreshList(WPARAM wParam,LPARAM lParam)
{
	int nID = static_cast<int>(wParam);
	m_smsDlg.ClickTreeShowText(nID);
	
	return 0;
}
