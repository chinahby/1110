/*==============================================================================
//           WapBrowser  A P P L E T  M O D U L E
// 文件:
//        WapBrowserApp.c
//        版权所有(c) 2011 WaterWorld Tech. CO.,LTD.
//        
// 文件说明：
//        浏览器模块主文件
//                 
//
==============================================================================*/

/*==============================================================================
修改历史记录：
时       间       修 改 人     修改内容、位置及原因
-----------      ----------    -----------------------------------------------

==============================================================================*/


/*==============================================================================

  本文件包含的外部文件
  
==============================================================================*/
#include "WapBrowser_priv.h"      // 主Applet头文件

/*==============================================================================

  宏定义和常数
  
==============================================================================*/

#define APP_ISHELL (((AEEApplet*)Brew_GetAppInstance())->m_pIShell)
//#define AEECLSID_NF3							(0x0100711E)
#define EVT_USER_BASE				(EVT_USER + 0x4100)
/* NetFront3 User Events */
#define EVT_NF3_BASE				(EVT_USER_BASE + 0x1100)
#define EVT_APP_RECEIVE_START		(EVT_NF3_BASE + 0x01)
/* NetFront3 Sub Events */
#define EVT_SUBEVT_BASE				(0x00)
#define EVT_RECEIVE_WAPPUSH			(EVT_SUBEVT_BASE + 0x11)


typedef struct _param tparam;

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

enum{
	APP_BROWSER_START_MAINMENU = APPLICATION_MSGBASE_SYSTEM,
	APP_BROWSER_START_HOME,		/* arg: not use */
	APP_BROWSER_START_BY_MAINMENU, /* arg : slim_int IDM_BROWSER_XXX */
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

void init_menu(CWapBrowserApp * pMe);
void draw_menu(CWapBrowserApp * pMe);
void move_cursor(CWapBrowserApp *pMe, int inc);
void start_nf3(CWapBrowserApp *pMe);

//boolean NF3Test_HandleEvent( AEEApplet * pme, AEEEvent eCode,uint16 wParam, uint32 dwParam);

/*==============================================================================

  相关数据类型定义
  
==============================================================================*/


/*==============================================================================

  本地函数声明
  
==============================================================================*/
//模块相关函数声明


int CWapBrowserAppMod_Load(IShell *pIShell, void *ph, IModule **ppMod);

int CWapBrowserAppMod_New(int16                nSize, 
                      IShell               *pIShell,
                      void                 *ph,   
                      IModule              **ppMod,
                      PFNMODCREATEINST     pfnMC, 
                      PFNFREEMODDATA       pfnMF);

static uint32 CWapBrowserAppMod_AddRef(IModule *po);

static uint32 CWapBrowserAppMod_Release(IModule *po);

static int CWapBrowserAppMod_CreateInstance(IModule  *po, 
                                        IShell   *pIShell,
                                        AEECLSID ClsId, 
                                        void     **ppObj);

static void CWapBrowserAppMod_FreeResources(IModule *po, IHeap *ph, IFileMgr *pfm);

// Applet相关函数声明
// Applet创建和释放函数
static int WapBrowserApp_New(IShell *ps, IModule *pIModule, IWapBrowserApp **ppObj);

static uint32  WapBrowserApp_AddRef(IWapBrowserApp *pi);

static uint32  WapBrowserApp_Release(IWapBrowserApp *pi);

// Applet事件处理函数
static boolean WapBrowserApp_HandleEvent(IWapBrowserApp   *pi, 
                                     AEEEvent     eCode,
                                     uint16       wParam, 
                                     uint32       dwParam);

// 内部Applet方法声明
static int WapBrowserApp_InitAppData(CWapBrowserApp *pMe);

static void WapBrowserApp_FreeAppData(CWapBrowserApp *pMe);

// Applet主状态机处理方法
static void WapBrowserApp_RunFSM(CWapBrowserApp *pMe);

// 各种外来通知事件的处理函数
//static boolean TVApp_HandleNotifyEvent(CTVApp *pMe, AEENotify *pN);

static void WapBrowserApp_APPIsReadyTimer(void *pme);



/*==============================================================================
全局数据
==============================================================================*/
// 重要提示::
//        g_dwAEEStdLibEntry全局变量仅用于SDK !
//        BREW SDK用户不能改写和直接使用本变量。它用作AEEStdLib的一个入口点，不
//        要移动下列三行代码。
#ifdef AEE_SIMULATOR
uint32 g_dwAEEStdLibEntry;
//   #error The TV applet was never intended to be used with the AEE Simulator.
#endif

/*==============================================================================

  本地（静态）数据
  
==============================================================================*/

enum {
	mStartMode_BrowserMenu,
	mStartMode_BrowserHome,
	mStartMode_BrowserURL,
	mStartMode_BrowserPushRcv,
	mStartMode_BrowserOTABookmarkRcv,
	mStartMode_BrowserOTARcv,
	mStartMode_Background,
	mStartMode_All
};

static char *gMenuTexts[] = {
	" Start: Brw  Menu",
	" Start: Brw  Home",
	" Start: Brw  URL",
	" Start: Brw  Push Received",
	" Start: Brw  OTA bookmark Received",
	" Start: Brw  OTA Received",
	" Background",
	NULL
};

static int gMenuItemNr, gLcdItemNr, gCharHeight, gMargin;
static int gTopItem, gCurItem;
AEERect lcd_r;

void init_menu(CWapBrowserApp *pMe)
{
	AEEDeviceInfo di;
	char **p;
	int a, d;

	p = gMenuTexts;
	gMenuItemNr = 0;
	while (*p++) gMenuItemNr++;
	gTopItem = gCurItem = 0;
	ISHELL_GetDeviceInfo(pMe->m_pShell, &di);
	lcd_r.x = 0;
	lcd_r.y = 0;
	lcd_r.dx = di.cxScreen;
	lcd_r.dy = di.cyScreen;
	IDISPLAY_GetFontMetrics(pMe->m_pDisplay, AEE_FONT_NORMAL, &a, &d);
	gCharHeight = a + d;
	gLcdItemNr = lcd_r.dy / gCharHeight;
	gMargin = lcd_r.dy - gLcdItemNr * gCharHeight;
}

void draw_menu(CWapBrowserApp *pMe)
{
	int i, n;
	AEERect r;
	AECHAR wstr[64];
	RGBVAL bg, fg;
	int count=0;

	for (i=gTopItem; i<gMenuItemNr; i++,count++) {
		if (i == gCurItem) {
			bg = MAKE_RGB(0, 0, 255);
			fg = MAKE_RGB(255, 255, 255);
		}
		else {
			bg = MAKE_RGB(255, 255, 255);
			fg = MAKE_RGB(0, 0, 0);
		}
		r = lcd_r;
		r.y += count * gCharHeight;
		r.dy = gCharHeight;
		n = STRLEN(gMenuTexts[i]);
		UTF8TOWSTR((void*)gMenuTexts[i], n, wstr, 64 * sizeof(AECHAR));
		IDISPLAY_DrawRect(pMe->m_pDisplay, &r, 0, bg, IDF_RECT_FILL);
		IDISPLAY_SetColor(pMe->m_pDisplay, CLR_USER_TEXT, fg);
		IDISPLAY_DrawText(pMe->m_pDisplay, AEE_FONT_NORMAL, wstr, n, 0, r.y, NULL, IDF_TEXT_TRANSPARENT);
	}
	r.y = lcd_r.y + lcd_r.dy - gMargin;
	r.dy = gMargin;
	IDISPLAY_DrawRect(pMe->m_pDisplay, &r, 0, MAKE_RGB(255, 255, 255), IDF_RECT_FILL);

	IDISPLAY_Update (pMe->m_pDisplay);
}

void move_cursor(CWapBrowserApp *pMe, int inc)
{
	if (inc == 1) {
		if (gCurItem < gMenuItemNr - 1) {
			gCurItem++;
			if (gCurItem - gTopItem + 1 > gLcdItemNr)
				gTopItem++;
			draw_menu(pMe);
		}
		return;
	}
	if (inc == -1) {
		if (gCurItem > 0) {
			gCurItem--;
			if (gCurItem < gTopItem)
				gTopItem--;
			draw_menu(pMe);
		}
		return;
	}
}

typedef struct {
    char	*title;
    char	*url;
} StartupInfoURL;

#define AUTO_RECV_FLAG_IMAIL    0x0001
#define AUTO_RECV_FLAG_MSGR     0x0004
#define AUTO_RECV_FLAG_MSGF     0x0008

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

static StartInfo start_info;
static char *address = "sun@access.co.jp";
enum {
	JAM_START_MODE_LIST = 0,
	JAM_START_MODE_INSTALL,
	JAM_START_MODE_EXEC,
	JAM_START_MODE_EXEC_EMBED,
	JAM_START_MODE_TCK,
	JAM_START_MODE_OTA_TCK,
	JAM_START_MODE_BOOT,
	JAM_START_MODE_BATCH_INSTALL,
	/* MIDP only */
	JAM_START_MODE_PUSH,
	/* DoJa only */
	JAM_START_MODE_CHECK_EXEC,
	JAM_START_MODE_STANDBY_START,
	JAM_START_MODE_STANDBY_LIST,
	JAM_START_MODE_STANDBY_RELEASE,
	JAM_START_MODE_AUTO_START,
#ifdef JAM_CONFIG_DOJA_ENABLE_START_AUTO_START_FAILURE_LOG
	JAM_START_MODE_AUTO_START_FAILURE_LOG,
#endif /* JAM_CONFIG_DOJA_ENABLE_START_AUTO_START_FAILURE_LOG */
#ifdef SLIM_CONFIG_USE_DESKTOPICON
	JAM_START_MODE_EXEC_ID,
#endif /* SLIM_CONFIG_USE_DESKTOPICON */
  /* #if defined (JLT_CONFIG_DOJA) && (30 <= JLT_CONFIG_DOJA) */
	JAM_START_MODE_MSGAPP_START,
	JAM_START_MODE_MSGAPP_STATUS,
  /* #endif */
#ifdef JLT_CONFIG_DOJA_USE_LOCATION
	JAM_START_MODE_LOCATION_EXEC,
#endif
	/* DoJa only end */

	JAM_START_MODE
};
enum {
	APP_EVENT_APPCONTROL = 0,
	APP_EVENT_START,
	APP_EVENT_STOP,
	APP_EVENT_SUSPEND,
	APP_EVENT_RESUME,
	APP_EVENT_STOP_CONFIRM,
	APP_EVENT_INTERRUPT,
	APP_EVENT_FINALIZE,
	APP_EVENT_LOW_BATTERY,
	APP_EVENT_SIM,
	APP_EVENT_IME,
	APP_EVENT_JAVA_NOTIFY,
	APP_EVENT_ADDRESSBOOK,
	APP_EVENT_SMS,
	APP_EVENT_BROWSEFILE,
	APP_EVENT_REGIST_BOOKMARK,
	APP_EVENT_MAIL_INCOMING,
	APP_EVENT_MAIL_CANCEL,
	APP_EVENT_IRMCOBEX_CANCEL,
	APP_EVENT_FLUSH_PAINT,
	APP_EVENT_TELCALL,
	APP_EVENT_SMS_RECEIVED,
	APP_EVENT_SMS_SENT,
	APP_EVENT_EMERGENCY_CALL,
	APP_EVENT_MAIL_REMAIN_ON_SERVER,
	APP_EVENT_EXTENDED_BASE = 0x10000,
	APP_EVENT_EXTENDED_JAVA_BASE = 0x20000,
	APP_EVENTS
};

#define APP_EVENT_IMAIL		0x01
/*#define APP_EVENT_IMMS		0x02*/
#define APP_EVENT_MESSAGE_R	0x04
#define APP_EVENT_MESSAGE_F	0x08

typedef struct {
    int32    fEventID;
    int32    fEventFlag;   /* bit-wise OR of EXTENDED_EVENT_FLAG_XXX */
    char    *fTransactionID;    /* only used for APP_EVENT_MAIL_INCOMING */
    char    *fContentLocation;  /* only used for APP_EVENT_MAIL_INCOMING */
} ExtendedEvent;

ExtendedEvent eevt;

/*#define NF3_TEST_SI_RECEIVE*/
#define NF3_TEST_SI_RECEIVE_FAIL_2

static void test_Push_Msg_Rcv(CWapBrowserApp *pMe)
{
#if 0
    nf3_wap_push_info smspus_data;

    uint8 sms[] = {
		0x73,0x06,0x1A,0xAE,0xB4,0x87,0xAF,0x82,0x45,0x6E,0x63,0x6F,0x64,0x69,0x6E,0x67,0x2D,0x56,0x65,
		0x72,0x73,0x69,0x6F,0x6E,0x00,0x31,0x2E,0x32,0x00,0x02, 0x05, 0x6a, 0x00, 0x45, 0xc6, 0x08, 0x0c, 
		0x03, 0x77, 0x61, 0x70, 0x2e, 0x79, 0x61, 0x68, 0x6f,0x6f, 0x00, 0x85, 0x0a, 0xc3, 0x07, 0x20, 0x03, 
		0x10, 0x06, 0x10, 0x17, 0x03, 0x10, 0xc3, 0x07, 0x20,0x04, 0x10, 0x06, 0x10, 0x17, 0x03, 0x01, 0x03, 
		0x20, 0x54, 0x68, 0x69, 0x73, 0x20, 0x69, 0x73, 0x20,0x64, 0x69, 0x73, 0x70, 0x6c, 0x61, 0x79, 0x65,
		0x64, 0x20, 0x74, 0x65, 0x78, 0x74, 0x2e, 0x20, 0x00,0x01, 0x01
        };

    int smssize = sizeof(sms);

    MEMSET( &smspus_data, 0x00, sizeof(nf3_wap_push_info) );
    smspus_data.sm_len = smssize;
    smspus_data.sm_data = (uint8*) MALLOC(smssize);
    MEMSET( smspus_data.sm_data, 0x00, smssize);
    MEMCPY( smspus_data.sm_data , sms, smssize);

    ISHELL_SendEvent(pMe->m_pIShell, AEECLSID_NF3, (AEEEvent)EVT_APP_RECEIVE_START, 
                        (uint16)EVT_RECEIVE_WAPPUSH, (uint32)&smspus_data);

    FREEIF(smspus_data.sm_data);
#else
	;
#endif
}

static void test_OTA_Bookmark_Rcv(CWapBrowserApp *pMe)
{
    char title[] = {'t','e','s','t','\0','\0','\0','\0'};
    char url[]   = {'h','t','t','p',':','/','/','t','e','s','t','.','c','o','m','\0','\0','\0'};
    TBookmarkObj ota_data;
             
    ota_data.fTitle = title;
    ota_data.fURL = url;
    ota_data.fProtected = FALSE;
    ota_data.fRet = FALSE;
             
    ISHELL_SendEvent(pMe->m_pShell, AEECLSID_NF3, EVT_USER, EVT_NF3_ADD_BOOKMARK_OTA, (uint32)&ota_data);    
}

static void test_OTA_Rcv(CWapBrowserApp *pMe)
{
    otap_browser_t ota_data;
    otap_pxlogical_t proxy;
    otap_pxphysical_t pxphysical;
    otap_port_t port;
    otap_resource_t bookmark;
    
    char name[] = {'c','o','n','n','\0','\0'};
    char NAPId[] = {'z','t','e','\0','\0','\0'};
    char pxaddress[] = {'p','r','o','x','y','\0'};
    char bm_title[] = {'t','e','s','t','\0','\0','\0','\0'};
    char bm_url[]   = {'h','t','t','p',':','/','/','t','e','s','t','1','.','c','o','m','\0','\0','\0'};
  
    MEMSET( &ota_data, 0x00, sizeof(otap_browser_t) );
    MEMSET( &proxy, 0x00, sizeof(otap_pxlogical_t) );
    MEMSET( &pxphysical, 0x00, sizeof(otap_pxphysical_t) );
    MEMSET( &port, 0x00, sizeof(otap_port_t) );
    MEMSET( &bookmark, 0x00, sizeof(otap_resource_t) );

    /* bookmark */
    ota_data.pBookmark = &bookmark;
    bookmark.pszName = bm_title;
    bookmark.pszURI = bm_url;
    bookmark.bStartPage = FALSE;
    bookmark.pNext = NULL;

    /* connection */
    ota_data.pszName = name;
    ota_data.pProxy = &proxy;
    proxy.pPxphysical = &pxphysical;
    pxphysical.pszPxAddr = pxaddress;
    pxphysical.pPort = &port;
    port.uPortNBR = 80;
    pxphysical.uProfileNum = 1;    
    
    ISHELL_SendEvent(pMe->m_pShell, AEECLSID_NF3, EVT_USER, EVT_PUSH_DATA_IND, (uint32)&ota_data); 
}

void start_nf3(CWapBrowserApp *pMe)
{
	char	buf[12];

	MEMSET(&start_info, 0, sizeof(StartInfo));
	switch (gCurItem) {
	case mStartMode_BrowserMenu:
		start_info.appid_fx = APP_ORIGINATOR_BROWSER;
		start_info.subtype_keycode = APP_BROWSER_START_BY_MAINMENU;
		start_info.par_keychar = 0;	
		break;
	case mStartMode_BrowserHome:
		start_info.appid_fx = APP_ORIGINATOR_BROWSER;
		start_info.subtype_keycode = APP_BROWSER_START_HOME;
		start_info.par_keychar = 0;		
		break;
	case mStartMode_BrowserURL:
	        start_info.appid_fx = APP_ORIGINATOR_BROWSER;
	        start_info.subtype_keycode = APP_BROWSER_START_URL;
	        start_info.par_keychar = 0;
	        start_info.fInfo.url_info.title = "zte";
	        start_info.fInfo.url_info.url = "http://www.zte.com.cn";
		break;
	case mStartMode_BrowserPushRcv:
	{
            test_Push_Msg_Rcv(pMe);
            break;
	}
       case mStartMode_BrowserOTABookmarkRcv:
       {
            test_OTA_Bookmark_Rcv(pMe);  
            break;
       }
       case mStartMode_BrowserOTARcv:
       {
            test_OTA_Rcv(pMe);
            break;
       }

	default:
		start_info.appid_fx = APP_ORIGINATOR_BROWSER;
		start_info.subtype_keycode = APP_BROWSER_START_BY_MAINMENU;
		start_info.par_keychar = 0;
		break;
	}
	SPRINTF(buf, "%p", &start_info);
	ISHELL_StartAppletArgs(pMe->m_pShell, AEECLSID_NF3, buf);
}


static CWapBrowserAppMod gWapBrowserAppMod;

static const VTBL(IModule) gModFuncs =
{
    CWapBrowserAppMod_AddRef,    
    CWapBrowserAppMod_Release,
    CWapBrowserAppMod_CreateInstance,
    CWapBrowserAppMod_FreeResources
};

// 只允许一个 WapBrowserApp 实例。每次创建 WapBrowser Applet 时，返回同一结构指针给 BREW 层。
static CWapBrowserApp gWapBrowserApp;//={0};

static const VTBL(IWapBrowserApp) gWapBrowserAppMethods =
{
    WapBrowserApp_AddRef,
    WapBrowserApp_Release,
    WapBrowserApp_HandleEvent
};

/*==============================================================================
函数:
       CWapBrowserAppMod_Load

说明:
       模块装载函数。

参数:
       pIShell [in]:IShell接口指针。
       ph :没有使用
       ppMod [out]:
 
返回值:
       int
   
备注:
  
==============================================================================*/
int CWapBrowserAppMod_Load(IShell   *pIShell,
                           void     *ph,
                           IModule  **ppMod)
{
    // 对于动态Applet，验证AEE STD库版本，以确保库与建立本模块时使用的库版本匹配
    // 。在AEE_SIMULATOR下不必进行本检查。
    MSG_FATAL("CWapBrowserAppMod_Load...........Start!!!!",0,0,0);
    if(ph != NULL && ppMod !=NULL && pIShell !=NULL)
    {
    	MSG_FATAL("CWapBrowserAppMod_Load...........Start!!!!1111",0,0,0);
    }
#if !defined(BREW_STATIC_APP) && !defined(AEE_SIMULATOR)
    if(GET_HELPER_VER() != AEESTDLIB_VER)
    {
    	MSG_FATAL("CWapBrowserAppMod_Load...........Start!!!!2222",0,0,0);
        return EVERSIONNOTSUPPORT;
    }
#endif
	MSG_FATAL("CWapBrowserAppMod_Load================================",0,0,0);
    return CWapBrowserAppMod_New(sizeof(CWapBrowserAppMod), pIShell, ph, ppMod, NULL, NULL);
}

/*==============================================================================
函数:
       CWapBrowserAppMod_New

说明:
       用来初始化WapBrowserApp模块虚拟表（也增加引用计数）的助手函数。
  
参数:
       nSize: 没有使用
       pIShell [in]: IShell 接口指针。
       ph : 没有使用
       ppMod [out]:
       pfnMC: 没有使用
       pfnMF: 没有使用
 
返回值:
       int
   
备注:
  
==============================================================================*/
int CWapBrowserAppMod_New(int16             nSize,
                      IShell            *pIShell,
                      void              *ph,
                      IModule           **ppMod,            
                      PFNMODCREATEINST  pfnMC,
                      PFNFREEMODDATA    pfnMF)
{
    PARAM_NOT_REF(nSize)
    PARAM_NOT_REF(pfnMC)
    PARAM_NOT_REF(pfnMF)
#ifndef AEE_SIMULATOR
    PARAM_NOT_REF(ph)
#endif
    if(!ppMod)
    {
        return EFAILED;
    }
  
    *ppMod = NULL;
  
// 重要提示:
//        g_dwAEEStdLibEntry全局变量仅用于SDK !
//        BREW SDK用户不能改写和直接使用本变量。它用作AEEStdLib的一个入口点
//        。
#ifdef AEE_SIMULATOR
    g_dwAEEStdLibEntry = (uint32)ph;
    if(!pIShell || !ph)
    {
        return EFAILED;
    }
#else
    if(!pIShell)
    {
        return EFAILED;
    }
#endif
  
    MEMSET(&gWapBrowserAppMod,0,sizeof(CWapBrowserAppMod));
  
    INIT_VTBL(&gWapBrowserAppMod,IModule,gModFuncs);
  
    gWapBrowserAppMod.m_nRefs = 1;
  
    *ppMod = (IModule *)&gWapBrowserAppMod;
  
    return AEE_SUCCESS;
}

/*==============================================================================
函数:
       CWapBrowserAppMod_AddRef

说明:
       WapBrowserApp模块引用计数加1，并返回当前的引用数。
  
参数:
       po [in/out]:IModule接口对象指针。
 
返回值:
       当前对WapBrowserApp模块的引用数
   
备注:
  
==============================================================================*/
static uint32 CWapBrowserAppMod_AddRef(IModule *po)
{
    return(++((CWapBrowserAppMod *)po)->m_nRefs);
}

/*==============================================================================
函数:
       CWapBrowserAppMod_Release

说明:
       WapBrowserApp模块引用计数减1，并返回当前的引用数。

参数:
       po [in/out]:IModule接口对象指针。
 
返回值:
       当前对TVApp模块的引用数
   
备注:
  
==============================================================================*/
static uint32 CWapBrowserAppMod_Release(IModule *po)
{
    if (((CWapBrowserAppMod *) po)->m_nRefs == 0)
    {
        return 0;
    }
 
    return(--((CWapBrowserAppMod *)po)->m_nRefs);
}

/*==============================================================================
函数:
       CWapBrowserAppMod_CreateInstance

说明:
       通过创建(初始化)WapBrowserApp对象来创建模块实例。
  
参数:
       po [in]:IModule接口对象指针。
       pIShell [in]:IShell接口对象指针。
       ClsId [in]:要创建的实例的类ID。
       ppObj [out]::用于保存对象实例的指针的指针变量。
 
返回值:
       none
   
备注:
       不需用户去调用。
  
==============================================================================*/
static int CWapBrowserAppMod_CreateInstance(IModule   *po,
                                        IShell    *pIShell,
                                        AEECLSID  ClsId,
                                        void      **ppObj)
{
    *ppObj = NULL;
 
    if(ClsId != AEECLSID_TVAPP)
    {
        return EFAILED;
    }
 
    if(WapBrowserApp_New(pIShell, po, (IWapBrowserApp**)ppObj) != SUCCESS)
    {
        return EFAILED;
    }
 
    return SUCCESS;
}

/*==============================================================================
函数:
       CWapBrowserAppMod_FreeResources

说明:
       释放WapBrowserApp模块资源的函数。

参数:
       po :没有使用。
       IHeap :没有使用。
       pfm ::没有使用。
 
返回值:
       none
   
备注:
       不需用户去调用。
  
==============================================================================*/
static void CWapBrowserAppMod_FreeResources(IModule *po, IHeap  *ph, IFileMgr *pfm)
{
    MSG_FATAL("po address = %x", po, 0, 0);
    MSG_FATAL("ph address = %x", ph, 0, 0);
    MSG_FATAL("pfm address = %x", pfm, 0, 0);
    PARAM_NOT_REF(po)
    PARAM_NOT_REF(ph)
    PARAM_NOT_REF(pfm)
}

/*==============================================================================
函数:
       WapBrowserApp_New

说明:
       初始化 WapBrowserApp Applet 虚拟表和内部数据变量，同时 WapBrowserApp Applet 引用计数加1。
  
参数:
       ps [in]: IShell接口对象指针。
       pIModule [in/out]: pIModule接口对象指针。
       ppObj [out]: 指向 ITVApp 对象指针的指针。
 
返回值:
       EFAILED: 失败。
       SUCCESS: 成功。
   
备注:
       不需用户去调用。
       关于Applet结构对象可使用动态分配内存的方式创建，本处采用的是使用静态
       变量的方式。建议对结构比较大的Applet使用静态变量的方式。对于动态分配
       内存的方式需要注意对所分配空间的释放。
  
==============================================================================*/
static int WapBrowserApp_New(IShell *ps, IModule *pIModule, IWapBrowserApp **ppObj)
{
    int retVal = SUCCESS;
   // ITvUtil* pITvUtil = NULL;
	//CTVApp* pThis = NULL;
	
    if(0 == gWapBrowserApp.m_nRefs)
    {
        MEMSET(&gWapBrowserApp, 0, sizeof(gWapBrowserApp));
        INIT_VTBL(&gWapBrowserApp, IWapBrowserApp, gWapBrowserAppMethods);
  
        gWapBrowserApp.m_nRefs = 0;
        gWapBrowserApp.m_pShell = ps;
        gWapBrowserApp.m_pModule = pIModule;
        //gWapBrowserApp.pIMMITv=NULL;
        (void)ISHELL_AddRef(ps);
        (void)IMODULE_AddRef(pIModule);
  
        retVal = WapBrowserApp_InitAppData(&gWapBrowserApp);
		
    }
 
    ++gWapBrowserApp.m_nRefs;
 
    *ppObj = (IWapBrowserApp*)&gWapBrowserApp;

    return retVal;
}

/*==============================================================================
函数:
       WapBrowserApp_AddRef

说明:
       WapBrowserApp Applet 引用计数加1，并返回当前的引用数。
  
参数:
       pi [in/out]: IWapBrowserApp 接口对象指针。
 
返回值:
       当前对 WapBrowserApp Applet 的引用数
   
备注:
  
==============================================================================*/
static uint32 WapBrowserApp_AddRef(IWapBrowserApp *pi)
{
    register CWapBrowserApp *pMe = (CWapBrowserApp*)pi;
 
    ASSERT(pMe != NULL);
    ASSERT(pMe->m_nRefs > 0);
 
    return (++pMe->m_nRefs);
}

/*==============================================================================
函数:
       WapBrowserApp_Release

说明:
       释放对 WapBrowserApp Applet 的引用，引用计数减1，并返回当前的引用数。
       如果当前的引用数为零，还将释放 Applet 及相关资源。
  
参数:
       pi [in/out]: IWapBrowserApp 接口对象指针。
 
返回值:
       当前对 TV Applet 的引用数
   
备注:
  
==============================================================================*/
static uint32 WapBrowserApp_Release(IWapBrowserApp *p)
{
    register CWapBrowserApp *pMe = (CWapBrowserApp*)p;
 
    if(pMe->m_nRefs == 0)
    {
        return 0;
    }
 
    if(--pMe->m_nRefs)
    {
        return pMe->m_nRefs;
    }
 
    WapBrowserApp_FreeAppData(pMe);
    MSG_FATAL("m_pShell address = %x", pMe->m_pShell, 0, 0);
    MSG_FATAL("m_pModule address = %x", pMe->m_pModule, 0, 0);
    (void)ISHELL_Release(pMe->m_pShell);
    (void)IMODULE_Release(pMe->m_pModule);
 
    // 注意:pMe是静态分配空间，因此不需要释放。FREE()
    return 0;
}

/*==============================================================================
函数:
       WapBrowserApp_InitAppData

说明:
       初始化 WapBrowserApp Applet 结构数据缺省值。
  
参数:
       pMe [in]: 指向 WapBrowserApp Applet 对象结构的指针。该结构包含小程序的特定信息。
 
返回值:
       EFAILED: 初始化失败。
       SUCCESS: 初始化成功。
   
备注:
  
==============================================================================*/
static int WapBrowserApp_InitAppData(CWapBrowserApp *pMe)
{
    AEEDeviceInfo  di;
    if(NULL == pMe)
    {
        return EFAILED;
    }
    ISHELL_GetDeviceInfo(pMe->m_pShell, &di);
    pMe->m_rc.x       = 0;
    pMe->m_rc.y       = 0;
    pMe->m_rc.dx      = (int16) di.cxScreen;
    pMe->m_rc.dy      = (int16) di.cyScreen;

	if(AEE_SUCCESS != ISHELL_CreateInstance(pMe->m_pShell,
                                            AEECLSID_DISPLAY,
                                            (void **)&pMe->m_pDisplay))
    {
        WapBrowserApp_FreeAppData(pMe);
		MSG_FATAL("AEECLSID_DISPLAY-----------------%d",EFAILED,0,0);
        return EFAILED;
    } 
    return SUCCESS;

	
} 



/*==============================================================================
函数:
       WapBrowserApp_FreeAppData

说明:
       释放 WapBrowserApp Applet 相关资源。
  
参数:
       pMe [in]: 指向WapBrowserApp Applet 对象结构的指针。该结构包含小程序的特定信息。
 
返回值:
       none
   
备注:
  
==============================================================================*/
static void WapBrowserApp_FreeAppData(CWapBrowserApp *pMe)
{
    if(NULL == pMe)
    {
        return;
    }
    
    
}

/*==============================================================================
函数:
       WapBrowserApp_RunFSM
    
说明:
       WapBrowserApp Applet有限状态机引擎。
   
参数:
       pMe [in]：指向WapBrowserApp Applet对象结构的指针。该结构包含小程序的特定信息。
     
返回值:
       无。
    
备注:
      
==============================================================================*/
static void WapBrowserApp_RunFSM(CWapBrowserApp *pMe)
{
    //NextFSMAction nextFSMAction = NFSMACTION_WAIT;
     
    for( ; ; )
    {
        //nextFSMAction = WapBrowserApp_ProcessState(pMe);
      
        if(pMe->m_bNotOverwriteDlgRet)
        {
            pMe->m_bNotOverwriteDlgRet = FALSE;
        }
        else
        {
            //pMe->m_eDlgRet = DLGRET_CREATE;
        }
      
       // if(nextFSMAction == NFSMACTION_WAIT)
        //{
        //    break;
        //}
    }
}
    
/*==============================================================================
函数:
       WapBrowserApp_HandleEvent
    
说明:
       WapBrowserApp Applet事件处理主函数。所有属于本Applet的事件，都将经由该函数处理。
   
参数:
       pi [in]:指向IWapBrowserApp接口t对象的指针。
       eCode:BREW事件代码。
       wParam:事件参数
       dwParam [in]:与wParam关联的数据。。
     
返回值:
       TRUE:传入事件被处理。
       FALSE:传入事件没被处理。
    
备注:
       该函数供BREW调用，用户不得调用。
      
==============================================================================*/
static boolean WapBrowserApp_HandleEvent(IWapBrowserApp  *pi,
                                     AEEEvent     eCode,
                                     uint16       wParam,
                                     uint32       dwParam)
{
    CWapBrowserApp *pMe = (CWapBrowserApp*)pi;   
    AEEAppStart *as;    
    int  nAscent,nDescent;

    MSG_FATAL("WapBrowserApp_HandleEvent-----------------------0x%x",eCode,0,0);
    switch(eCode)
    {
        case EVT_APP_START:
			MSG_FATAL("WapBrowserApp_HandleEvent--------EVT_APP_START---------------",0,0,0);
            init_menu(pMe);
			draw_menu(pMe);
			MSG_FATAL("WapBrowserApp_HandleEvent--------EVT_APP_START----------end  ",0,0,0);
			IDISPLAY_Update (pMe->m_pDisplay);
            return TRUE;
      
        case EVT_APP_STOP:
            return TRUE;
      
        case EVT_APP_SUSPEND:
            
            return TRUE;
            
        case EVT_ALARM:
            return TRUE;
            
        case EVT_APP_RESUME:
            return TRUE;
      
        case EVT_DIALOG_INIT:
        case EVT_DIALOG_START:
        case EVT_USER_REDRAW:
            return TRUE;
            
        case EVT_CAMERA_NOTIFY:
            return TRUE;
            
        case EVT_APPISREADY:
            return TRUE;
      
        case EVT_KEY_PRESS:
        {
        	switch (wParam) {
				case AVK_UP:
					move_cursor(pMe, -1);
					break;
				case AVK_DOWN:
					move_cursor(pMe, 1);
					break;
				case AVK_SELECT:
					start_nf3(pMe);
					break;
				case AVK_CLR:
					ISHELL_CloseApplet(pMe->m_pShell, FALSE);
					break;
				case AVK_2:
					ISHELL_SendEvent(pMe->m_pShell, AEECLSID_NF3, EVT_USER, 32001, 0);
					break;
				default:
					break;
			}
		}
		return TRUE;
        case EVT_KEY_RELEASE:
        case EVT_KEY:
        case EVT_COMMAND:         
        case EVT_DIALOG_END:
            return TRUE;
   
        default:
            //将接收到的事件路由至当前活动的对话框事件处理函数。
            //return WapBrowserApp_RouteDialogEvent(pMe, eCode, wParam, dwParam);
            break;
    }

    return FALSE;
} //TVApp_HandleEvent()

/*==============================================================================
函数:
       WapBrowserApp_APPIsReadyTimer
  
说明:
       定时器回调函数。主要用于控制快速按键问题的处理。
 
参数:
       pme [in]：void *类型指针，暗指CWapBrowserApp结构指针。
   
返回值:
       无。
  
备注:
    
==============================================================================*/
static void WapBrowserApp_APPIsReadyTimer(void *pme)
{
    CWapBrowserApp *pMe = (CWapBrowserApp *)pme;
    
    if(NULL == pMe)
    {
        return;
    }

}

