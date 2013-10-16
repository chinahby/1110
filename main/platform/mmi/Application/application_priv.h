/** ----------------------------------------------------------------------------
 * application_priv.h
 *
 * Copyright(c) 2007 Anylook
 *
 *
 * History:
 *
 * when            who               what and why
 * -----------  -----------    -----------------------------
 * 2008.12.24                created  for  Q1
 *
 * -----------------------------------------------------------------------------
 */

#ifndef APPLICATION_PRIV_H
#define APPLICATION_PRIV_H
#endif

#include "application.brh"
#include "AEEConfig.h"
#include "AEEAnnunciator.h"
#ifdef FEATRUE_SUPPORT_G_SENSOR
#include "G_Sensor.h"
#endif
#include "AEEBacklight.h"

#include "AEEConfig.h"
#include "OEMCFGI.h"
#ifdef FEATURE_OEMOMH
#include "AEERUIM.h"
#endif
#include "AppComFunc.h"
#include "Appscommon.h"
//#include "AEECallList.h"
#include "AEECallHistory.h"
#include "appscommonimages.brh"

/*==============================================================================
                                 
                                相关数据类型定义
                                 
==============================================================================*/

typedef struct _ApplicationMod
{
    DECLARE_VTBL( IModule)

    uint32   referenceCounter;
    
} ApplicationMod;

//Application  下的几个状态
typedef enum ApplicationState
{
    APPLICATIONST_MAIN,
    APPLICATIONST_FLASHLIGHT,
#if defined(FEATURE_VERSION_W317A)
    APPLICATIONST_PCMODEM,
#endif
#if defined(FEATURE_VERSION_K212_ND)
		APPLICATIONST_KEY_TIME,
		APPLICATIONST_SOS_SET,
#endif

    APPLICATIONST_EXIT
} ApplicationState;


typedef enum
{
    APPLICATION_STOP=0x00,
    APPLICATION_RUNNING,
    APPLICATION_SUSPEND
} APPLICATION_STATUS_e_type;

typedef enum DLGRetValue
{
    DLGRET_CREATE,
    DLGRET_FLASHLITHT,
#if defined(FEATURE_VERSION_W317A)
    DLGRET_PCMODEM,
#endif
#if defined(FEATURE_VERSION_K212_ND)
	DLGRET_KEY_TIME,
	DLGRET_SOS_SET,
#endif
    DLGRET_CANCELED
}DLGRetValue;

typedef enum NextFSMAction
{
   NFSMACTION_WAIT,
   NFSMACTION_CONTINUE
   
} NextFSMAction;


typedef struct _Point
{
    int x;
    int y;
} Point;
typedef struct {
    char	*title;
    char	*url;
} StartupInfoURL;

typedef struct {
	char *fMailAddress;
	char *fSubject;
	char *fContent;
	char *fFilePath;
}StartupInfoEmailComposition;

typedef struct{
	char *fStr;
}StartupInfoMMSComposition;

typedef enum {
	JAM_APP_TYPE_UNKOWN = -1,
	JAM_APP_TYPE_CLDC = 0,
	JAM_APP_TYPE_MIDP,
	JAM_APP_TYPE_DOJA,
	JAM_APP_TYPE_DIR,

	JAM_APP_TYPES
} TJamAppType_;
typedef struct{
	TJamAppType_ fType;
	char *fAdfUrl;
	int fAdfUrlLen;
	byte *fAdfContent;
	int fAdfContentSize;
	char *fUserName;
	int fUserNameLen;
	char *fPassword;
	int fPasswordLen;
	byte *fSessionID;
	int fSessionIDLen;
}StartupInfoJava;

typedef struct {
	int appid_fx;
	int subtype_keycode;
	int par_keychar;
	union {  
		StartupInfoURL	url_info;
		StartupInfoEmailComposition	emailcomposition_info;
		StartupInfoMMSComposition	mmscomposition_info;
		StartupInfoJava				java_info;
    	} fInfo;
} StartInfo;

enum {
#if 1 /*APP_CONFIG_USE_EVENTLISTENER*/
	APP_ORIGINATOR_ALLAPPSELECT = -1,
#endif /*APP_CONFIG_USE_EVENTLISTENER*/
	APP_ORIGINATOR_TASK = 0,
	APP_ORIGINATOR_MANAGER,
	APP_ORIGINATOR_LAUNCHER,
	APP_ORIGINATOR_BROWSER,
	APP_ORIGINATOR_MAILER,
	APP_ORIGINATOR_DATAFOLDER,
	APP_ORIGINATOR_ADDRESSBOOK,
	APP_ORIGINATOR_MEDIAPLAYER,
	APP_ORIGINATOR_IrDA,
	APP_ORIGINATOR_BLUETOOTH,
	APP_ORIGINATOR_MMS,
	APP_ORIGINATOR_SISL,
	APP_ORIGINATOR_SISLDAEMON,
	APP_ORIGINATOR_NETMANAGER,
	APP_ORIGINATOR_MIMEVIEWER,
	APP_ORIGINATOR_CALENDAR,
	APP_ORIGINATOR_MMSDAEMON,
	APP_ORIGINATOR_WAPPUSHDAEMON,
	APP_ORIGINATOR_DOWNLOAD,
	APP_ORIGINATOR_JAVA,
	APP_ORIGINATOR_EVENTLISTENER,
	APP_ORIGINATOR_TELCALL,
	APP_ORIGINATOR_JAMSTUB,
	APP_ORIGINATOR_PDFVIEWER,
	APP_ORIGINATOR_DOCVIEWER,
#ifdef APP_CONFIG_USE_DOWNLOAD_DAEMON
	APP_ORIGINATOR_DOWNLOADDAEMON,
	APP_ORIGINATOR_DOWNLOADEXT,
#endif
	APP_ORIGINATORS
};
#define	APPLICATION_MSGBASE_SYSTEM			0x00000

#define FIX_CRASH_WHEN_ADD_BKMARK_FROM_SMS

enum{
	APP_BROWSER_START_MAINMENU = APPLICATION_MSGBASE_SYSTEM,
	APP_BROWSER_START_HOME,		/* arg: not use */
	APP_BROWSER_START_BY_MAINMENU, /* arg : slim_int IDM_BROWSER_XXX */
#ifdef FIX_CRASH_WHEN_ADD_BKMARK_FROM_SMS
	APP_BROWSER_START_NORMAL,
#endif	
	APP_BROWSER_START_URL,			/* arg: TString url */
	APP_BROWSER_START_PAGESAVEINFO,	/* arg : TPageSaveInfo (slim_handle) */
	APP_BROWSER_START_BY_PUSH,			/* arg: TString url */
	APP_BROWSER_RESULT_SAVEFILE,	/* arg: TString file name */
#ifdef WIRELESS_BROWSER_CONFIG_USE_FILEUPLOAD
	APP_BROWSER_RESULT_FILESELECT,	/* arg: TString file name */
#endif
	APP_BROWSER_RESULT_ADDRESS_BOOK,	/* arg : TString add addressbook */
#ifdef WIRELESS_BROWSER_CONFIG_USE_VBOOKMARK
	APP_BROWSER_VBOOKMARK_IMPORT,				/* arg: TString file name */
#endif
	APP_BROWSER_REQUEST_BOOKMARKLIST_UPDATE,	/* arg: not use */
#ifdef APP_CONFIG_USE_DRM
	APP_BROWSER_START_RIGHTS,
	APP_BROWSER_START_RIGHTS_ONMEMORY,
#endif
#ifdef APP_CONFIG_USE_CASTING
	APP_BROWSER_START_URL_WITH_POST,
#endif
	APP_BROWSER_RES_INSTALL,
	APP_BROWSER_STARTS
};
enum {
	WIRELESS_MESSAGING_START_MAINMENU = APPLICATION_MSGBASE_SYSTEM,
	WIRELESS_MESSAGING_END_ADDRESS_BOOK,
	WIRELESS_MESSAGING_END_SELECT_FILE,
	WIRELESS_MESSAGING_EXTERNAL,
	WIRELESS_MESSAGING_MESSAGES
};
enum {
	WIRELESS_MAILER_START_MAINMENU = WIRELESS_MESSAGING_MESSAGES,
	WIRELESS_MAILER_START_RECEIVE,
	WIRELESS_MAILER_START_INBOX,
	WIRELESS_MAILER_START_OUTBOX,
	WIRELESS_MAILER_START_SENTMAIL,
	WIRELESS_MAILER_START_COMPOSER,	/* arg: TString mailt to url */
	WIRELESS_MAILER_START_ACCOUNT,
	WIRELESS_MAILER_START_OPTION_MENU,
	WIRELESS_MAILER_START_AUTO_RECEIVE,
	WIRELESS_MAILER_START_COMPOSER_ATTACH,
	WIRELESS_MAILER_START_OPEN_SETTING_SITE,
	WIRELESS_MAILER_START_NEWMAIL_CONFIRM,
	WIRELESS_MAILER_START_MAILBOX_CONFIRM,
	WIRELESS_MAILER_START_EMAIL_CANCEL_OVER,
	WIRELESS_MAILER_END_DAEMON,
	WIRELESS_MAILER_END_DATAFOLDER,
	WIRELESS_MAILER_END_CANCEL_BGRECV,
	WIRELESS_MAILER_EXTERNAL,
	WIRELESS_MAILER_MESSAGES
};
enum {
	APP_MMS_START_BASE = WIRELESS_MESSAGING_MESSAGES,
	APP_MMS_START_SUBTYPE_NORMAL,
	APP_MMS_END_SELECT_FILE2, /* From mmsSMIL (SMIL Editor) */
	APP_MMS_START_ATTACH,
	APP_MMS_EXTERNAL,
	APP_MMS_START_COMPOSER,
	APP_MMS_START_TEXT_COMPOSER,
	APP_MMS_START_VCARD,
	APP_MMS_START_VCALENDER,
	APP_MMS_START_TELNUM,
	APP_MMS_START_SMS_CLASS0,
	APP_MMS_START_INBOX,
	APP_MMS_CMD,
	APP_MMS_SEND_DONE,
	APP_MMS_SMSSEND_DONE,
	APP_MMS_RCV_DONE,
	APP_MMS_SEND_CANCEL_DONE,
	APP_MMS_SMSSEND_CANCEL_DONE,
	APP_MMS_RCV_NOTIFYCATION,
	APP_MMS_RCV_CANCEL_DONE,
	APP_MMS_MSG_MOVE_DONE,
	APP_MMS_SETTING_DONE,
	APP_MMS_SETTING_RESET_DONE,
	APP_MMS_MSG_DELETE_DONE,
	APP_MMS_MSG_RESET_DONE,
	APP_MMS_START_COMPOSER_BROWSER,
	APP_MMS_START_COMPOSER_BROWSER_ANCHOR,
	APP_MMS_RCV_UPDATE,
	APP_MMS_ZAP_MMSCOMMAND_DONE,
	APP_MMS_START_SLIDE,
#ifdef SLIM_CONFIG_USE_MMBOX
	APP_MMS_SVMAIL_LISTRCV_DONE,
	APP_MMS_SVMAIL_LISTRCV_CANCEL_DONE,
	APP_MMS_SVMAIL_RECEIVE_DONE,
	APP_MMS_SVMAIL_RECEIVE_CANCEL_DONE,
	APP_MMS_SVMAIL_DELETE_DONE,
	APP_MMS_SVMAIL_DELETE_CANCEL_DONE,
	APP_MMS_SVMAIL_FORWARD_DONE,
	APP_MMS_VIEWER_SVMAIL_DELETE_DONE,
	APP_MMS_VIEWER_SVMAIL_DELETE_CANCEL_DONE,
	APP_MMS_SUMMARY_SVMAIL_DELETE_DONE,
	APP_MMS_SUMMARY_SVMAIL_DELETE_CANCEL_DONE,
#endif /* SLIM_CONFIG_USE_MMBOX */
	APP_MMS_MESSAGES
};

static StartInfo start_info;

typedef struct _Application
{
    DECLARE_VTBL(IApplication)
    uint32       referenceCounter;
    
    IModule     *m_pModule;
    IDisplay    *m_pDisplay;
    IShell      *m_pShell;

    IDialog     *m_pActiveIDlg;
    uint32       m_pActivedlgID;
    uint16       m_MainSel;   //一级菜单光标
    DLGRetValue  m_eDlgReturn;

    APPLICATION_STATUS_e_type  m_eAppStatus;   // Applet 当前运行状态
    
    boolean     m_bDoNotOverwriteDlgResult; //是否不重写对话框结果    
    ApplicationState             m_prevState;        // Applet前一状态
    ApplicationState             m_currState;        // Applet当前状态
    AEERect     m_rc;
    IAnnunciator                    *m_pIAnn;
	#if defined(FEATURE_VERSION_W317A)
    IStatic    						*m_pIStatic;      // Static for displaying information
    #endif
    IBacklight     *m_pBacklight;       //背光接口指针
    boolean        m_StartCore;
#ifdef FEATURE_OEMOMH
    IRUIM        *m_pIRUIM;
#endif
} Application;

/*==============================================================================
                                 
                                 宏定义和常数
                                 
==============================================================================*/
#if defined (FEATURE_DISP_128X128)
#define  MAX_MATRIX_ITEMS 10
#elif defined(FEATURE_DISP_160X128)
#define  MAX_MATRIX_ITEMS 7
#elif defined(FEATURE_DISP_220X176)
#define  MAX_MATRIX_ITEMS 7
#elif defined(FEATURE_DISP_128X160)
#define  MAX_MATRIX_ITEMS 7
#elif defined(FEATURE_DISP_176X220)
#define  MAX_MATRIX_ITEMS 7
#elif defined(FEATURE_DISP_240X320)
#define  MAX_MATRIX_ITEMS 7
#elif defined(FEATURE_DISP_320X240)
#define  MAX_MATRIX_ITEMS 7
#endif

#define APPLICATION_RES_FILE_LANG        AEE_RES_LANGDIR APPLICATION_RES_FILE

#define STARTARGPREFIX_VIEWPLAYER            'P'
#define STARTARGPREFIX_VIEWDATA              'D'




// 此宏用当前状态更新先前状态，再用nextState状态更新当前状态
#define MOVE_TO_STATE(nextState)            \
{                                           \
    ApplicationState tpState;                    \
    tpState = nextState;                    \
    pMe->m_prevState = pMe->m_currState;    \
    pMe->m_currState = tpState;             \
}

//关闭对话框的宏
#define CLOSE_DIALOG(dlgResult)             \
{                                           \
    pMe->m_eDlgReturn = dlgResult;          \
    (void) ISHELL_EndDialog(pMe->m_pShell); \
}

/*==============================================================================

                                 函数声明

==============================================================================*/

static int ApplicationMod_CreateInstance( IModule *po,
    IShell      *pIShell,
    AEECLSID    ClsId,
    void        **ppObj
);

static uint32 ApplicationMod_AddRef( IModule *po);

static uint32 ApplicationMod_Release( IModule *po);

static void ApplicationMod_FreeResources( IModule *po, IHeap *ph, IFileMgr *pfm);

