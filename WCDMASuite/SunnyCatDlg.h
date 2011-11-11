// SunnyCatDlg.h : header file
//

#if !defined(AFX_SUNNYCATDLG_H__7A45DD2C_C0D1_431D_8157_4BC9EDC6B4CC__INCLUDED_)
#define AFX_SUNNYCATDLG_H__7A45DD2C_C0D1_431D_8157_4BC9EDC6B4CC__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

#include "TabSheet.h"
#include "CallDialog.h"
#include "SMSDialog.h"
#include "NetDialog.h"
//#include "ColorBtn.h"
#include "BmpBkDialog.h"
#include "OtherDialog.h"
#include "RecordDialog.h"
#include "LinkManDialog.h"
#include "RoundButton.h"
#include "SkinWin\SkinWin.h"
#include "SkinWin\SkinButton.h"
#include "mobilemanager.h"
#include "CallManager.h"
#include "winaddrbook.h"
#include "EnFileDialog.h"
#include "label.h"
#include "contacts.h"
#include "RemovePrompt.h"
#include "Setupapi.h"
#include "UpdateProgressDlg.h"
#include "public/IWaveLib.h"
#include "public/IRecorder.h"
#include "InitAT.h"
#include "WaitCOPS.h"

/////////////////////////////////////////////////////////////////////////////
// CSunnyCatDlg dialog
enum EConnectionStatus
{
	NET_CONNECTING,
	NET_CONNECTED,
	NET_DISCONNECTING,
	NET_DISCONNECTED,
};

enum NetType
{
	NETTYPE_NOSERVICE,
	NETTYPE_ONEX,
	NETTYPE_EVDO,
};

class CSunnyCatDlg : public CDialog
{
// Construction
public:		
	void InitServerUI();
	void FlashInfoBar(CString strText);
	bool m_isConnect;
//	CSkinButton* btn[4];
	CImageList*  g_pSysImageList;	
	void ShowPage(int item);
	void InitUI();
	CSunnyCatDlg(CWnd* pParent = NULL);	// standard constructor
	~CSunnyCatDlg();
//	CCallManager  *pCallManager;
	CMobileManager m_MobileManager;

	CCallDialog   m_callDlg;
	CNetDialog    m_netDlg;
	CSMSDialog    m_smsDlg;
	CRecordDialog m_recordDlg;
	CLinkManDialog m_linkManDlg;
	COtherDialog  m_LinkDlg;

	//网络信息的变量
	CString m_NetName;
	CString m_NetRegister;

	BOOL m_autoregister;
	bool m_bsmssendsucc;	// 记录发送成功与失败add by lab686
	int m_smssendfail;
	int m_smssendsucc;//记录发送失败的条数   默认为0
	bool m_bPINenordis;
	CWinAddrBook* wab;
	COutlookContact* outlookContact;
	
	CRemovePrompt* m_removePrompt;
	
	void OnConnectionStateChange(EConnectionStatus stat);
	
	void ShowOnLine(bool bShow);
	void FlashInfoForNetWork();
	BOOL SubClassButton( HWND hwnd );
	BOOL SetSkin(CString skinPath);
	void OnCallGoingProcess(const CString& strProcess);
	void OnCallComingProcess(const CString& strProcess);
	void ShowCallPromptWindow(const CString& strPhone,const CString& strName, bool bRepeatSound = true);
	void OnCallComing(CCallManager* pSender);
	void PlayIncomeRing(int nID,bool bRepeat= true);
	void PlaySMSRing();

	BOOL FindNameInPhonebook(CString strPhone,CString& strName,int& nID);
	void ShowConnectMsgInInfoBar(const CString& inStrMsg);
	bool Exit(bool bNeedConfirm); 
	void RemoveModem();

	void Connect();

	//==========================080825he
	void InitDlg();
	void SetHold(CColorBtn &button);
	void PaintBottom(CPaintDC& dc);
	void PaintTop(CPaintDC& dc);
	void PaintMenuBk(CPaintDC& dc);
	void InitIconState();
// Dialog Data
	//{{AFX_DATA(CSunnyCatDlg)
	enum { IDD = IDD_SUNNYCAT_DIALOG };
	CColorBtn	m_OperationBtn;
//	CColorBtn	m_RecordBtn;
	CColorBtn	m_ctrlContactBtn;
	CColorBtn	m_ctrlHelpBtn;
	CColorBtn	m_ctrlFileBtn;
	CLabel	m_ctrlInfoBar;
	CStatic	m_netGsm;
	CStatic	m_netGprs;
	CStatic	m_netEdge;
	CStatic	m_notRegister;
	CStatic	m_regSuccess;
	CStatic	m_roaming;
	CStatic	m_imgSMSPromt;
	CStatic	m_staOnLine;
	CStatic	m_s1;
	CStatic	m_s0;
	CLabel	m_sEvdo;
	CLabel 	m_sOneX;
	CRoundButton	m_unconnect;
	CRoundButton	m_connect;
//	CRoundButton	m_unconnectevdo;		// add by lab686
//	CRoundButton	m_connectevdo;		// add by lab686
	NetType	m_linktype;		// add by lab686 used marker the net type
	CStatic	m_s2;
	CStatic	m_s3;
	CStatic	m_s4;
	CStatic	m_s5;

	CStatic    m_s0_h;
	CStatic    m_s1_h;
	CStatic    m_s2_h;
	CStatic    m_s3_h;
	CStatic    m_s4_h;
	CStatic    m_s5_h;
	
	CColorBtn	m_CallBtn;
	CColorBtn	m_SMSBtn;
	CColorBtn	m_NetBtn;
	CColorBtn   m_LinkBtn;
	CColorBtn   m_LinkManBtn;
	CStatic		m_Line1;
	CStatic		m_Line2;
	CStatic		m_Line3;
	CStatic		m_NotOnLine;
	//}}AFX_DATA

	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CSunnyCatDlg)
	protected:
	virtual void DoDataExchange(CDataExchange* pDX);	// DDX/DDV support
	virtual LRESULT DefWindowProc(UINT message, WPARAM wParam, LPARAM lParam);
	virtual BOOL PreCreateWindow(CREATESTRUCT& cs);
	//}}AFX_VIRTUAL

// Implementation
protected:
	BOOL TaskBarAddIcon();
	BOOL TaskBarDeleteIcon();
	HICON m_hIcon;
	CRgn		m_rgn;
	CSkinButtonResource		m_btnres;		//skin button resource
	CSkinWin				m_skinWin;		//skin win
	BOOL					m_bFirst;		//first time call
	// Generated message map functions
	//{{AFX_MSG(CSunnyCatDlg)
	 afx_msg LONG OnNotifyIcon(WPARAM wParam, LPARAM lParam);
	virtual BOOL OnInitDialog();
	afx_msg void OnPaint();
	afx_msg HCURSOR OnQueryDragIcon();
	afx_msg void OnLButtonDown(UINT nFlags, CPoint point);
	afx_msg void OnMenuFile();
	afx_msg void OnMenuHelp();
	afx_msg void OnSelchangeTab(NMHDR* pNMHDR, LRESULT* pResult);
	afx_msg void OnKeyDown(UINT nChar, UINT nRepCnt, UINT nFlags);
	afx_msg void OnButtonConnect();
	afx_msg void OnButtonUnconnect();
	afx_msg void OnButtonConnectevdo();
	afx_msg void OnButtonUnconnectevdo();
	afx_msg void OnMenuUnconnect();
	afx_msg void OnMenuConnect();
	afx_msg void OnMenuClose();
	afx_msg void OnMenuHelpinfo();
	afx_msg void OnMenuAbout();
	afx_msg void OnMenuSetlinkman();
	afx_msg void OnMenuSet();
	afx_msg void OnMenuNetwork();
	afx_msg void OnDestroy();
	afx_msg void OnTimer(UINT nIDEvent);
	afx_msg LONG OnDialShow(WPARAM wParam, LPARAM lParam);
	afx_msg void OnStaticSmsimg();
	afx_msg LONG OnQueryCallStatus(WPARAM wParam, LPARAM lParam);
	afx_msg LONG OnUnLockUIM(WPARAM wParam, LPARAM lParam);	// add by lab686 20091110
	afx_msg LONG OnShowSignal(WPARAM wParam, LPARAM lParam);
	afx_msg LONG OnShowNetwork(WPARAM wParam, LPARAM lParam);
	afx_msg LONG OnSetAuto(WPARAM wParam, LPARAM lParam);	
	afx_msg LONG OnShowHDRSignal(WPARAM wParam, LPARAM lParam);		// ADD BY LAB686 20091105
	afx_msg LONG OnSetSMSSendState(WPARAM wParam, LPARAM lParam);		// ADD BY LAB686 20091108
	afx_msg LONG OnSerialLport(WPARAM wParam, LPARAM lParam);		// ADD BY LAB686 2009112
	afx_msg LONG OnNetRegStatusChange(WPARAM wParam, LPARAM lParam);
	afx_msg LONG OnNetAttachType(WPARAM wParam, LPARAM lParam);
	afx_msg LONG OnSMSNew(WPARAM wParam, LPARAM count);
	afx_msg LONG OnMsgStatusChange(WPARAM wp, LPARAM);
	afx_msg LONG OnNetConnecting(WPARAM wParam, LPARAM lParam);
	afx_msg LONG OnNetConnected(WPARAM wParam, LPARAM lParam);
	afx_msg LONG OnNetDisconnected(WPARAM wParam, LPARAM lParam);
	afx_msg void OnOK();
	afx_msg LONG OnSMSNotify(WPARAM wParam, LPARAM lParam);
	afx_msg void OnSize(UINT nType, int cx, int cy);
	afx_msg void OnGetMinMaxInfo( MINMAXINFO FAR* lpMMI );
	afx_msg void OnClose();
afx_msg LONG OnChangSmsImg(WPARAM wParam, LPARAM lParam);
	afx_msg LONG OnShowSMSSendInfo(WPARAM wParam, LPARAM lParam);
	afx_msg void OnSysCommand(UINT nID, LPARAM lParam);
	afx_msg void OnLinkmanInput();
	afx_msg void OnLinkmanOutput();
	afx_msg void OnPINOperationEnable();
	afx_msg void OnPINOperationDisable();
	afx_msg void OnSynToOutlook();
	afx_msg void OnSynFromOutlook();
	afx_msg BOOL OnNcCreate(LPCREATESTRUCT lpCreateStruct);
	afx_msg void OnMenuAddlinkman();
	afx_msg LONG OnDeviveChange(WPARAM wParam, LPARAM lParam);
	afx_msg void OnDeviceRemoved();
	afx_msg void OnSynFromOO();
	afx_msg void OnSynToOO();
	afx_msg HBRUSH OnCtlColor( CDC* pDC, CWnd* pWnd, UINT nCtlColor );
	afx_msg LONG OnPower(WPARAM wParam, LPARAM lParam);
	afx_msg LONG OnSIMLock(WPARAM wParam, LPARAM count);
	afx_msg void OnAutoUpdate();
	afx_msg void OnMenuAccelerate();
	virtual void OnCancel( );
	afx_msg void OnButtonLink();
	afx_msg void OnButtonNet();
	afx_msg void OnButtonCall();
	afx_msg void OnButtonSms();
	afx_msg void OnButtonLinkman();
	afx_msg void OnButtonRecord();
	afx_msg void OnMenuOperation();
	afx_msg void OnOperationLink();
	afx_msg void OnOperationNetWork();
	afx_msg void OnOperationCall();
	afx_msg void OnOperationSMS();
	afx_msg void OnOperationPB();
	afx_msg BOOL OnEraseBkgnd(CDC* pDC) ;
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()
		
	LRESULT OnShowCallImag(WPARAM wParam,LPARAM lParam);
	LRESULT OnRefreshNetRecord(WPARAM wParam,LPARAM lParam);
	LRESULT OnRefreshCallRecord(WPARAM wParam,LPARAM lParam);
	LRESULT OnNetConnectFail(WPARAM wParam,LPARAM lParam);
	LRESULT OnDisconnecting(WPARAM wParam,LPARAM lParam);
	LRESULT OnRefreshInfoBar(WPARAM wParam,LPARAM lParam);
	LRESULT OnRefreshFailInfoBar(WPARAM wParam,LPARAM lParam);
	LRESULT OnRefreshTelInfoBar(WPARAM wParam,LPARAM lParam);
	LRESULT OnDispTelInEdit(WPARAM wParam,LPARAM lParam);
	LRESULT OnUpdateFail(WPARAM wParam,LPARAM lParam);
	LRESULT OnUpdateBeg(WPARAM wParam,LPARAM lParam);
	LRESULT OnUpdateEnd(WPARAM wParam,LPARAM lParam);
	LRESULT OnUpdateSetprogress(WPARAM wParam,LPARAM lParam);
	LRESULT OnNetPPP(WPARAM wParam,LPARAM lParam);
	LRESULT OnSimReadResult(WPARAM wParam,LPARAM lParam);
	LRESULT OnInitStart(WPARAM wParam,LPARAM lParam);
	LRESULT OnInitEnd(WPARAM wParam,LPARAM lParam);
	LRESULT OnInitError(WPARAM wParam,LPARAM lParam);
	LRESULT OnSMSRefreshList(WPARAM wParam,LPARAM lParam);
				
public:
	void ConnectNet();
	void Disconnect();
	void OnTalking(CCallManager* pSender);
	BOOL ResetCom(TCHAR * GUIDString,CString strDeviceName) ;
	void GetParaFromReg(BOOL& bYN,BOOL& bUnCnt,CString& strDate,CString& strUncntNM,CString& strWarnNM,CString& strNm );
	void StopRecord();
	void StartVoice();
	void StopVoice();
	void StopPCM();
	void ChgAutoRegdit();

private:
	bool DisorceTxtFile(LPCTSTR lpFileName,CArray<CString,CString>&arrLines);
	int  DivStringToArray(char *pszBuf,CArray<CString,CString>&arrDest);
	int  DivStringToArray(CString &strSrc,CArray<CString,CString>&arrDest,bool bUseTrim = false);
	void RemoveDoubleChar(CString & strSrc);
	void InitATSetting(bool hasInitialUI = false);
	
	BOOL StateChange(DWORD NewState, HDEVINFO m_hDevInfo,CString strDeviceName) ;
	void BegAutoUpdate();
	BOOL UpdateThreadIsRun();
	void EndDownloadThread();

	void CalcNetFlux(CInfoConnect InfoCnt);
	void UnconnectNet();
	void ShowFluxPrompWindow(CString strInfo);

	void MC5SendMessage(int nOffset);
	static BOOL CALLBACK EnumWindowsProc(HWND hwnd, LPARAM lParam );

	void SetFirstNetRegedit();
	void FormatConnInfo(const CInfoConnect& Info, CString& strInfo);
	void DispTextInfobar(CString strText);
	void CreateOperationMenu();
	bool WaitForAutoRun(const CString& strVolume);
	
	void ClearSimPB();
private:
	BOOL m_bMinmize;

	BOOL m_bConnected;
	BOOL m_bUnconnected;
//	BOOL m_bNeedRemveMdm;

	HMODULE m_hMoudle;
	HMODULE m_hModuleDDK;

	CWinThread* m_UpdateThread;
	CUpdateProgressDlg* m_UpdateDlg;
	BOOL m_bHandUpdate;
	BOOL m_bAutoupdate;

	BOOL m_bFluxWarnDoModal;
	BOOL m_bFluxUncntDoModal;
	CGenMsgBox m_Fluxdlg ;
	BOOL m_bWarnNMdlg;

	static HWND m_MC5WindowHandle;
	static wchar_t m_pwcSDIPath[MAX_PATH] ;
	static wchar_t m_pwcSDIPathCE[MAX_PATH] ;
	BOOL m_bMC5Enable;
	BOOL m_bMC5Disable;
	HANDLE m_hMC5Process;
	HANDLE m_hMC5ProcessCE;

//	BOOL m_bConProcessQuit;

	CString m_strProduct;

	HMODULE m_hRegModule;
	HANDLE m_RegHandle;
	
	CInfoConnect m_InfoCnt; 
//	bool m_bAutoReConnect;
	bool m_bReset;
	DWORD m_dwConnectSecond;
	CString strShowText;
	bool bShow;
	CFont newFont;
//public:	
//	bool m_bPPPOK;
public:
	CString m_strVolume;
	bool m_bIsResetCom;
	CInitAT m_InitAT;
	bool m_bResetting;
	bool m_bfirstmove;		// add by lab686 20091102     用于记录除去硬件后第一次监听显示提示框
	bool m_bIsLockUIM;

public:
	bool m_bSetQueryRingTimer;
	CString m_strLanguage;
private:
	CString  m_NewSMSNum;
	CString m_NewSMSText;
	CMenu m_OperatonMenu;
	NMWave::IWaveLib* m_pWaveLib;
	bool m_bPCMVoice;
	NMWave::IRecorder *m_pRecorder;
	NMWave::IRecorder *m_pRecorderSerial;
	bool m_bCntSuccess;
	
};

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_SUNNYCATDLG_H__7A45DD2C_C0D1_431D_8157_4BC9EDC6B4CC__INCLUDED_)





















