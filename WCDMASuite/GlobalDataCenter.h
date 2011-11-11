// GlobalDataCenter.h: interface for the CGlobalDataCenter class.
//
//////////////////////////////////////////////////////////////////////

#if !defined(AFX_GLOBALDATACENTER_H__2D0479B9_AF51_40D7_883E_FC97FC8B66C2__INCLUDED_)
#define AFX_GLOBALDATACENTER_H__2D0479B9_AF51_40D7_883E_FC97FC8B66C2__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

#include "Afxtempl.h"
#include "mmsystem.h"
#include "GenMsgBox.h"

class CMobileManager;


CString GetProduct();
CString GetLanguage();

HBITMAP  LoadBmpFormFile(LPCTSTR lpFileName) ;

void SetAutoRunOnStart(bool bRunFlag = true);
BOOL LoadImgList(CImageList*  g_pSysImageList, UINT nIDResource);
BOOL LoadImgList(CImageList*  pSysImageList, CString strResource);

BOOL SetVolume(DWORD dwSrcType, DWORD dwLValue, DWORD dwRValue, BOOL bMono = FALSE);
//BOOL GetWaveVolume(DWORD* pdwLValue,DWORD* pdwRValue);
BOOL SetWaveVolume(DWORD dwLValue,DWORD dwRValue);
//BOOL GetDevCaps(LPMIXERCAPS pmxcaps);
//BOOL GetVolume(DWORD dwSrcType, DWORD* pdwLValue, DWORD* pdwRValue, BOOL bMono);
const CString&GetSysPath() ;	 //取得当前Windows系统路径
const CString&GetCurrentPath() ; //取得当前路径
void     MultiLineString(CString & strSrc) ;
int      GMessageBox(LPCTSTR lpMsgText,LPCTSTR lpCaption=NULL,DWORD dwFlags=MB_OK|MB_ICONASTERISK);
bool EnableKey(TCHAR key, const TCHAR keyTable[]);

bool GetUsbDev(const CString& hwid);
void xJudgeDeviceChangePath(DWORD dwData, CString &strPath) ;
void SetDevToModem(CString strVolume);
void SetWorkMode(CMobileManager& mgr,int nWorkType,const bool bInit);
void   CDRomOpen(BOOL   bOpenDrive,TCHAR   cDrive);
void SetCDToModem(CMobileManager& mgr,const CString strVolume,const bool bInit = false);

enum EUseType
{
	USETYPE_USBMODEM = 0,
	USETYPE_UDISK,
	USETYPE_CDROM
};

//自定义消息
enum ECustomMessage
{
	WM_COMM_RING = WM_USER+12,	
	WM_PROMT_MSG,
	WM_SMS_NEW,	
	WM_SMS_FULL,
	WM_SMS_NOTIFY,
	WM_NET_CONNECTING,		
	WM_SHOW_SIGNAL,		
	WM_SHOW_CDMA_TIME,
	WM_NET_DISCONNECTED,	
	WM_NET_CONNECTED,	
	WM_NOTIFYICON,       
	WM_DIAL_SHOW,		
	WM_VOICEMAIL_NOTIFY,	
	WM_ZFNM,				
	WM_STANDBY,			
	WM_SEARCH_PHONEBOOK,
	WM_PHONEBOOK_CHANGED,	//电话本发生改变
	WM_SERIALPORT_ERROR,	//串口异常
	WM_CALLTIMEBEGIN,	
	WM_CALLWAITING,
	WM_FLASHCALL_CONNECTED,	
	WM_REG_STATUS,
	WM_NET_TYPE,
	WM_CALL_INCOMING,
	WM_SMS_READ,
	WM_LINKMAN_NOTIFY,
	WM_SMSLIST_NOTIFY,
	WM_SMSSEND_SHOW,
	WM_REFRESH_NETRECORD,
	WM_SHOW_CALLIMAG,
	WM_REFRESH_CALLRECORD,
	WM_NETWORK_SHOWRATE,
	WM_NETWORK_TIME,
	WM_NET_DISCONNECTING,
	WM_NET_CONNECT_FAIL,
	WM_REFRESH_INFOBAR,
	WM_REFRESH_FAILINFOBAR,
	WM_REFRSH_TELINFOBAR,
	WM_INCOMING_DISPINEDIT,
	WM_SIM_LOCK,
	WM_UPDATE_FAIL,
	WM_UPDATE_BEG,
	WM_UPDATE_END,
	WM_UPDATE_SETPROGRESS,
	WM_SET_SEARCH,
	WM_SIMREAD_RESULT,
	WM_INIT_START,
	WM_INIT_END,
	WM_INIT_ERROR,
	WM_SMS_REFRESHLIST,
	WM_SHOW_HDRSIGNAL,
	WM_SMSSEND_STATE,
	WM_SIGNAL_SIMLOCK,
	WM_SHOW_NETWORK,
	WM_SET_AUTO
	/*WM_NO_SIM,*/
};

/**
*	拨号过程状态
*/
enum EConnectionState
{
	CONNECT_CONTROL_NOT_CONNECT = 0,
	CONNECT_CONTROL_DAIL,
	CONNECT_CONTROL_CONNECT,
	CONNECT_CONTROL_HANGUP
};

//定时器
enum 
{
	TIMER_NETREG_STATUS	   = 1001,
	TIMER_FLUX_SPEED   = 1002,//上网时间流量定时器
	TIMER_CHECK_SIGNAL = 1004,
	TIMER_SENDPTNUM    = 1005,
	TIMER_TALKING	   = 1006,
	TIMER_OUTGONING	   = 1007,
	TIMER_ENDTALK	   = 1008,
	TIMER_INCOMING	   = 1009,
	TIMER_CALL_STATUS   = 1010,
	TIMER_CALL_RING		= 1011,
	TIMER_NET_PPP		=1012,
	TIMER_RESET_MODEM  = 1013,
	TIMER_NET_POWER    =1014,
	TIMER_SHOW_DLG =1015,
	TIMER_MONITOR_MODEM =1016
};

#define  INDENT			 "    "  //左边显示屏，文字缩进
#define  LEFTPANEL_INDENT			 "       "  //左边显示屏，文字缩进





#endif // !defined(AFX_GLOBALDATACENTER_H__2D0479B9_AF51_40D7_883E_FC97FC8B66C2__INCLUDED_)
