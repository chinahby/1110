/*==============================================================================
//           U S E R   I N T E R F A C E   M A I N  M O D U L E
// 文件:
//        CoreAppDialogHandler.c
//        版权所有(c) 2009 Anylook Tech. CO.,LTD.
//        
// 文件说明:
//        本应用是 UI 的起始点，在开机时由 BREW 自动启动。         
//
// 作者:2007-09-18
// 当前版本:Draft
==============================================================================*/

/*==============================================================================
修改历史记录:
时       间       修 改 人     修改内容、位置及原因
-----------      ----------    -----------------------------------------------

==============================================================================*/



/*==============================================================================
                             
                             本文件包含的外部文件
                             
==============================================================================*/
#include "OEMFeatures.h"
#include "CoreApp_priv.h"
#include "AEEMenu.h"
#ifndef WIN32
#include "uim.h"
#include "mobile.h"             // for ver_modelname
#include "mccdma.h"  //for use of cdma_bs_type
#endif
#include "Appscommon.h"
#include "OEMRTC.h"
//#include "AEECallList.h"
#include "clockapps_images.brh"
#include "appscommonimages.brh"
#include "AEEGraphics.h "
#include "AEEConfig.h"
#include "oemcfgi.h"
#include "nv.h"
#include "OEMCFGI.h"

#include "clockapps.brh"
#define  CLOCK_RES_PATH ("fs:/mod/clockapps/" AEE_RES_LANGDIR CLOCKAPPS_RES_FILE)
#include "coreapp.brh"
#define  CORE_RES_PATH ("fs:/mod/coreapp/" AEE_RES_LANGDIR COREAPP_RES_FILE)

#if defined( FEATURE_POWERDOWN_ALARM)
extern void registerPowerdownAlarmclock( void);
#endif
extern void OEMPriv_DrawMessageCB(void *pUnused);
#ifdef FEATRUE_SUPPORT_G_SENSOR
#include "g_sensor.h"
#include "DisplayMenu.h"
#endif

#ifdef USES_MMI
extern boolean   IsRunAsUIMVersion(void);
extern boolean   IsRunAsFactoryTestMode(void);
#endif
/*==============================================================================

                               宏定义和常数
                                 
==============================================================================*/
#define CoreDrawBottomBar(x)                        \
{                                                   \
    BottomBar_Param_type BarParam;                  \
    MEMSET(&BarParam, 0, sizeof(BarParam));         \
    BarParam.eBBarType = x;                         \
    DrawBottomBar(pMe->m_pDisplay, &BarParam);      \
}

// 开关机动画播放时间
#define PWRON_ANI_TIME      ((PWRON_ANI_RATE)*(PWRON_ANI_FRAME_COUNT))
#define PWROFF_ANI_TIME     ((PWROFF_ANI_RATE)*(PWROFF_ANI_FRAME_COUNT))

#if defined(FEATURE_DISP_128X128)
#define IDLE_D_CLOCK_X 		15
#define IDLE_D_CLOCK_Y 		25

#define RPLMN_X				IDLE_D_CLOCK_X
#define RPLMN_Y				(IDLE_D_CLOCK_Y+25)

#define DATA_X				IDLE_D_CLOCK_X
#define DATA_Y				(RPLMN_Y + 30) 

#elif defined(FEATURE_DISP_160X128)

#define IDLE_D_CLOCK_X 		5
#if defined(FEATURE_VERSION_HITZ181)||defined(FEATURE_VERSION_MTM)
#define IDLE_D_CLOCK_Y 		SCREEN_HEIGHT - BOTTOMBAR_HEIGHT - 15
#else
#define IDLE_D_CLOCK_Y 		25
#endif

#define RPLMN_X				5
#define RPLMN_Y				20

#define DATA_X				5
#if defined(FEATURE_VERSION_HITZ181)||defined(FEATURE_VERSION_MTM)
#define DATA_Y				SCREEN_HEIGHT - BOTTOMBAR_HEIGHT - 15
#else
#define DATA_Y				36
#endif

#define WEEK_X              5
#define WEEK_Y              52
#elif defined(FEATURE_DISP_220X176)


#ifdef FEATURE_VERSION_W208S
#define IDLE_D_CLOCK_X 		5
#define IDLE_D_CLOCK_Y 		22

#define RPLMN_X				IDLE_D_CLOCK_X
#define RPLMN_Y				IDLE_D_CLOCK_Y

#define DATA_X				IDLE_D_CLOCK_X
#define DATA_Y				(IDLE_D_CLOCK_Y+25)

#define WEEK_X              DATA_X
#define WEEK_Y              DATA_Y
#else
#define IDLE_D_CLOCK_X 		5
#define IDLE_D_CLOCK_Y 		25

#define RPLMN_X				IDLE_D_CLOCK_X
#define RPLMN_Y				(IDLE_D_CLOCK_Y+25)

#define DATA_X				IDLE_D_CLOCK_X
#define DATA_Y				(RPLMN_Y + 25) 

#define WEEK_X              IDLE_D_CLOCK_X
#define WEEK_Y              (DATA_Y + 25)
#endif

#elif defined(FEATURE_DISP_128X160)

#define IDLE_D_CLOCK_X 		5
#define IDLE_D_CLOCK_Y 		25

#define RPLMN_X				5
#ifdef FEATURE_VERSION_MYANMAR
#define RPLMN_Y				(IDLE_D_CLOCK_Y)
#else
#define RPLMN_Y				(IDLE_D_CLOCK_Y+18)
#endif

#define DATA_X				5
#define DATA_Y				(RPLMN_Y + 16) 

#define WEEK_X              5
#define WEEK_Y              (DATA_Y+16)

#elif defined(FEATURE_DISP_176X220)

#define IDLE_D_CLOCK_X 		15
#define IDLE_D_CLOCK_Y 		150

#define RPLMN_X				5
#define RPLMN_Y				40

#define DATA_X				5
#define DATA_Y				(RPLMN_Y + 25) 

#define WEEK_X              5
#define WEEK_Y              (DATA_Y + 25) 

#elif defined(FEATURE_DISP_240X320)

#define IDLE_D_CLOCK_X 		15
#define IDLE_D_CLOCK_Y 		25

#define RPLMN_X				IDLE_D_CLOCK_X
#define RPLMN_Y				(IDLE_D_CLOCK_Y+25)

#define DATA_X				IDLE_D_CLOCK_X
#define DATA_Y				(RPLMN_Y + 30) 

#define WEEK_X              IDLE_D_CLOCK_X
#define WEEK_Y              (DATA_Y + 30) 

#elif defined(FEATURE_DISP_320X240)

#define IDLE_D_CLOCK_X 		10
#define IDLE_D_CLOCK_Y 		35

#define RPLMN_X				IDLE_D_CLOCK_X
#define RPLMN_Y				(IDLE_D_CLOCK_Y+25)

#define DATA_X				IDLE_D_CLOCK_X
#define DATA_Y				(RPLMN_Y + 30) 

#define WEEK_X              IDLE_D_CLOCK_X
#define WEEK_Y              (DATA_Y + 30)


#else

#define IDLE_D_CLOCK_X 		15
#define IDLE_D_CLOCK_Y 		25

#define RPLMN_X				IDLE_D_CLOCK_X
#define RPLMN_Y				(IDLE_D_CLOCK_Y+25)

#define DATA_X				IDLE_D_CLOCK_X
#define DATA_Y				(RPLMN_Y + 30) 

#endif
// add pandy 2011-11-04
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
// add pandy 2011-11-04
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

/*==============================================================================

                                 类型定义

==============================================================================*/
//typedef struct {
//   char *num_buf;
//   byte  num_len;
//} EmergencyNumber;
static IImage * pWallPaper = NULL;
//int charging_mark = 0;
//int charging_mark2 = 0;

static boolean bHideText = FALSE;
static boolean bImageDecoded = FALSE;
static boolean bsupersingal = FALSE;
static StartInfo start_info;


/*==============================================================================

                                 函数声明
                                 
==============================================================================*/
// 对话框 IDD_ALARM 事件处理函数
#if defined( FEATURE_POWERDOWN_ALARM)
static boolean  IDD_ALARM_Handler(void *pUser,
                                AEEEvent   eCode,
                                uint16     wParam,
                                uint32     dwParam);
#endif//#if FEATURE_POWERDOWN_ALARM
// 对话框 IDD_LPM 事件处理函数
static boolean  IDD_LPM_Handler(void *pUser,
                                AEEEvent   eCode,
                                uint16     wParam,
                                uint32     dwParam);
                                
// 对话框 IDD_MSGBOX 事件处理函数
static boolean  IDD_MSGBOX_Handler(void *pUser,
                                   AEEEvent   eCode,
                                   uint16     wParam,
                                   uint32     dwParam);
                                
// 对话框 IDD_PWDINPUT 事件处理函数
static boolean  IDD_PWDINPUT_Handler(void *pUser,
                                     AEEEvent   eCode,
                                     uint16     wParam,
                                     uint32     dwParam);
                                     
// 对话框 IDD_UIMERR 事件处理函数
static boolean  IDD_UIMERR_Handler(void *pUser,
                                   AEEEvent   eCode,
                                   uint16     wParam,
                                   uint32     dwParam);

// 对话框 IDD_UIMSECCODE 事件处理函数
static boolean  IDD_UIMSECCODE_Handler(void *pUser,
                                       AEEEvent   eCode,
                                       uint16     wParam,
                                       uint32     dwParam);
                                       
// 对话框 IDD_EMERGENCYNUMLIST 事件处理函数
static boolean  IDD_EMERGENCYNUMLIST_Handler(void *pUser,
                                 AEEEvent   eCode,
                                 uint16     wParam,
                                 uint32     dwParam);
                                 
// 对话框 IDD_STARTUPANI 事件处理函数
static boolean  IDD_STARTUPANI_Handler(void *pUser,
                                       AEEEvent   eCode,
                                       uint16     wParam,
                                       uint32     dwParam);
                                       
// 对话框 IDD_LOADING 事件处理函数
static boolean  IDD_LOADING_Handler(void *pUser,
                                 AEEEvent   eCode,
                                 uint16     wParam,
                                 uint32     dwParam);
                                       
// 对话框 IDD_IDLE 事件处理函数
static boolean  IDD_IDLE_Handler(void *pUser,
                                 AEEEvent   eCode,
                                 uint16     wParam,
                                 uint32     dwParam);
                                 
#if defined(FEATURE_WMS_APP)
// 对话框 IDD_WMSTIPS 事件处理函数
static boolean  IDD_WMSTIPS_Handler(void *pUser,
                                 AEEEvent   eCode,
                                 uint16     wParam,
                                 uint32     dwParam);
#endif

// 对话框 IDD_POWERDOWN 事件处理函数
static boolean  IDD_POWERDOWN_Handler(void  *pMe,
                                      AEEEvent    eCode,
                                      uint16      wParam,
                                      uint32      dwParam);

#ifdef FEATURE_UTK2
static boolean  IDD_UTKREFRESH_Handler(void *pUser,
                                   AEEEvent   eCode,
                                   uint16     wParam,
                                   uint32     dwParam);
#endif //FEATURE_UTK2

// 更新待机界面的定时器函数。程序运行稳定后，每分钟执行一次
static void CoreApp_UpdateIdleTimer(void *pUser);                          
static void CoreApp_DrawBannerMessage(void    *pMe);

#ifdef FEATURE_APP_MUSICPLAYER
static void CoreApp_DrawMusicName(CCoreApp    *pMe,uint16 nIdx);
static void CoreApp_MusicNameAutoScroll(CCoreApp *pMe);
static boolean Core_bMusicNameRequiredScroll(CCoreApp *pMe, int nIdx);
static void Core_DrawNameResetScroll(CCoreApp *pMe);
#endif

static void CoreApp_SearchingTimer(void *pUser);

static void CoreApp_UpdateDateTime(CCoreApp    *pMe);

// 开机动画播放函数
static void CoreApp_PlayPwrOnAni(CCoreApp *pMe);

//关机动画播放函数
static void CoreApp_PlayPwrOffAni(CCoreApp *pMe);

//绘制墙纸
static void CoreApp_DrawWallPaper(CCoreApp *pMe);

#ifdef FEATURE_USES_BLACKBERRY
//绘制黑莓IDLE
static void CoreApp_DrawBlackBerry_IDLE(CCoreApp *pMe);
#endif
#ifdef FEATURE_LCD_TOUCH_ENABLE
static void CoreApp_DrawTouch_IDLE(CCoreApp *pMe);
#endif
static boolean CoreApp_LaunchApplet(CCoreApp *pMe,  AEECLSID   classID);

//static void CoreApp_UpdateAnnunciator(CCoreApp *pMe);
static void CoreApp_Process_Rtc_Event(CCoreApp *pMe);

#ifdef FEATURE_KEYGUARD
static void CoreApp_TimeKeyguard(void *pUser);
static void CoreApp_keypadtimer(void *pUser);
#endif
static void CoreApp_Issametimer(void *pUser);

static void CoreApp_UpdateBottomBar(CCoreApp    *pMe); 

#ifdef FEATURE_VERSION_C11
static void CCoreApp_TorchTipTimeOut(CCoreApp *pMe);
#endif

//static void CoreApp_GetRecordCount(CCoreApp *pMe);

//static boolean CoreApp_PowerAlarm_Event(CCoreApp *pMe);
//static void CoreApp_DrawStartAlarm(CCoreApp *pMe);
//boolean CoreApp_Start_Alarm(CCoreApp *pMe);
static void CoreApp_GetSPN(CCoreApp *pMe);

static void CoreApp_ImageNotify(void *po, IImage *pIImage, AEEImageInfo *pii, int nErr);

/*==============================================================================

                                 函数定义
                                 
==============================================================================*/

/*==============================================================================
函数:
    CoreApp_ShowDialog
       
说明:
    函数由状态处理函数用来创建相应对话框资源ID的对话框。

参数:
    pMe [In]: 指向 Core Applet对象结构的指针,该结构包含小程序的特定信息.
    dlgResId [in]:对话框资源ID，函数根据该ID创建对话框。

返回值:
    无

备注:

==============================================================================*/
void CoreApp_ShowDialog(CCoreApp *pMe,uint16  dlgResId)
{
    int nRet;
    // 一次只允许打开一个对话框
    if (ISHELL_GetActiveDialog(pMe->a.m_pIShell) != NULL)
    {
        // Looks like there is one dialog already opened.
        // Flag an error an return without doing anything.
        return;
    }

    nRet = ISHELL_CreateDialog(pMe->a.m_pIShell,
                               AEE_COREAPPRES_LANGFILE,
                               dlgResId,NULL);
}

/*==============================================================================
函数:
    CoreApp_ShowMsgDialog
       
说明:
    函数用来显示指定消息的对话框。

参数:
    pMe [In]: 指向 Core Applet对象结构的指针,该结构包含小程序的特定信息.
    nResId [in]:消息字符串在资源文件中的 ID 号。

返回值:
    none

备注:

==============================================================================*/
void CoreApp_ShowMsgDialog(CCoreApp *pMe,uint16  nResId)
{
    pMe->m_nMsgID = nResId;

    (void)CoreApp_ShowDialog(pMe,IDD_MSGBOX);
}

/*==============================================================================
函数:
    CoreApp_SetDialogHandler
       
说明:
    函数根据 pMe->m_wActiveDlgID 设定对话事件处理函数指针 m_pDialogHandler 相应值。

参数:
    pMe [In]: 指向 Core Applet对象结构的指针,该结构包含小程序的特定信息.

返回值:
    none

备注:
    若找不到对应的对话框处理函数, m_pDialogHandler 被置为 NULL
==============================================================================*/
void CoreApp_SetDialogHandler(CCoreApp *pMe)
{
    switch (pMe->m_wActiveDlgID)
    {
#if defined( FEATURE_POWERDOWN_ALARM)
        case IDD_ALARM:
            pMe->m_pDialogHandler = IDD_ALARM_Handler;
            break;
#endif //#if FEATURE_POWERDOWN_ALARM
        case IDD_LPM:
            pMe->m_pDialogHandler = IDD_LPM_Handler;
            break;
            
        case IDD_PWDINPUT:
            pMe->m_pDialogHandler = IDD_PWDINPUT_Handler;
            break;
            
        case IDD_UIMSECCODE:
            pMe->m_pDialogHandler = IDD_UIMSECCODE_Handler;
            break;
            
        case IDD_UIMERR:
            pMe->m_pDialogHandler = IDD_UIMERR_Handler;
            break;
            
        case IDD_MSGBOX:
            pMe->m_pDialogHandler = IDD_MSGBOX_Handler;
            break;
            
        case IDD_EMERGENCYNUMLIST:
            pMe->m_pDialogHandler = IDD_EMERGENCYNUMLIST_Handler;
            break;
            
        case IDD_STARTUPANI:
            pMe->m_pDialogHandler = IDD_STARTUPANI_Handler;
            break;
            
        case IDD_LOADING:
            pMe->m_pDialogHandler = IDD_LOADING_Handler;
            break;
            
        case IDD_IDLE:
            pMe->m_pDialogHandler = IDD_IDLE_Handler;
            break;

#if defined(FEATURE_WMS_APP)
        case IDD_WMSTIPS:
            pMe->m_pDialogHandler = IDD_WMSTIPS_Handler;
            break;
#endif            
        
        case IDD_POWERDOWN:
            pMe->m_pDialogHandler = IDD_POWERDOWN_Handler;
            break;
            
#ifdef FEATURE_UTK2
        case IDD_UTKREFRESH:
            pMe->m_pDialogHandler = IDD_UTKREFRESH_Handler;
            break;            
#endif //FEATURE_UTK2

        default:
            pMe->m_pDialogHandler = NULL;
            break;
    }
}

/*==============================================================================
函数:
    CoreApp_RouteDialogEvent
       
说明:
    函数将BREW事件路由给当前活动对话框的事件处理函数。

参数:
    pMe [In]: 指向 Core Applet对象结构的指针,该结构包含小程序的特定信息.
    eCode [in]:事件代码。
    wParam [in]:与事件eCode关联的数据。
    dwParam [in]:与事件eCode关联的数据。

返回值:
    TRUE:传入事件被处理。
    FALSE:传入事件没被处理。

备注:

==============================================================================*/
boolean CoreApp_RouteDialogEvent(CCoreApp *pMe,
    AEEEvent    eCode,
    uint16  wParam,
    uint32 dwParam)
{
	MSG_FATAL("***zzg CoreApp_RouteDialogEvent***",0,0,0);
	
    if (NULL == pMe->m_pDialogHandler)
    {
    	MSG_FATAL("***zzg CoreApp_RouteDialogEvent return FALSE***",0,0,0);
        return FALSE;
    }

	MSG_FATAL("***zzg CoreApp_RouteDialogEvent m_pDialogHandler***eCode=%d---wParam=%d---dwParam=%d",eCode,wParam,dwParam);
    
    return pMe->m_pDialogHandler((void *)pMe, eCode, wParam, dwParam);
}

/*==============================================================================
函数:
    DialogTimeoutCallback
    
说明:
    定时回调函数
       
参数:
    pUser [in]: 这里必须是指向 Core Applet 对象结构的指针。

       
返回值:
    none

       
备注:
    函数用来向 Core Applet 发送当前对话框提示超时的事件。
       
==============================================================================*/
static void DialogTimeoutCallback(void *pUser)
{
    CCoreApp *pMe = (CCoreApp*)pUser;

    if (NULL == pMe)
    {
        return;
    }
    
    if (ISHELL_ActiveApplet(pMe->a.m_pIShell) != AEECLSID_CORE_APP)
    {
        return;
    }

    // Post a message to the dialog telling it to close
    (void) ISHELL_PostEvent(pMe->a.m_pIShell,
                            AEECLSID_CORE_APP,
                            EVT_DISPLAYDIALOGTIMEOUT,
                            0,
                            0);
}


/*==============================================================================
函数:
    IDD_MSGBOX_Handler
    
说明:
    IDD_MSGBOX 对话框事件处理函数
       
参数:
    pUser [in]: 这里必须是指向 Core Applet 对象结构的指针。
    eCode [in]: 事件代码。
    wParam: 事件相关数据。
    dwParam: 事件相关数据。
       
返回值:
    TRUE:  传入事件被处理。
    FALSE: 传入事件被忽略。
       
备注:
    函数显示提示消息给用户，并自动关闭返回。
       
==============================================================================*/
static boolean  IDD_MSGBOX_Handler(void       *pUser,
                                   AEEEvent   eCode,
                                   uint16     wParam,
                                   uint32     dwParam)
{
    CCoreApp *pMe = (CCoreApp *)pUser;
    static IStatic * pStatic = NULL;
    if (NULL == pMe)
    {
        return FALSE;
    }

    switch(eCode)
    {
        case EVT_DIALOG_INIT:
        {
            if (NULL == pStatic)
            {
                AEERect rect = {0};
                if (AEE_SUCCESS != ISHELL_CreateInstance(pMe->a.m_pIShell,AEECLSID_STATIC,(void **)&pStatic))
                {
                    return FALSE;
                }
                ISTATIC_SetRect(pStatic, &rect);
            }
            return TRUE;
        }

        case EVT_DIALOG_START:
        {
            uint32 set_time= 1500;
            switch (pMe->m_nMsgID)
            {
                case IDS_LOWBATTMSG_TEXT:
                    set_time = 5000;
                    break;
                case IDS_SERVICEREQ:
                case IDS_EXIT_EMERGENCY_MODE:
                    set_time = 0;
                    break;

#ifdef FEATRUE_AUTO_POWER
                case IDS_AUTO_POWER_OFF:
                    //in this state,we need to disable the key guard.                     
#ifdef FEATURE_KEYGUARD
                    if(OEMKeyguard_IsEnabled())
                    {
                        OEMKeyguard_SetState(FALSE);
                    }
#endif
                    IBACKLIGHT_Enable(pMe->m_pBacklight);
                    set_time = 20000;
                    break;
#endif /* FEATRUE_AUTO_POWER */

#ifdef FEATURE_PLANEMODE
                case IDS_PLANEMODE_QUERY:
                    set_time = 5000;
                    break;
#endif
                case 0xFFFF:
                    set_time = 3000;
                    break;
                    
                case IDS_BATTERY_FULLY_CHARGE:    
                case IDS_CORE_DISPADN:
                case IDS_WAITING:
                    set_time = 0;
                    break;
                default:
                    break;
            }

            if (set_time != 0)
            {
                // 启动发送关闭对话框事件的定时器
                ISHELL_SetTimer(pMe->a.m_pIShell, set_time,DialogTimeoutCallback,pMe);
            }
            ISHELL_PostEvent(pMe->a.m_pIShell, AEECLSID_CORE_APP,EVT_USER_REDRAW,  0, 0);
            return TRUE;
        }

        case EVT_USER_REDRAW:
        {
            if(pMe->m_nMsgID == IDS_CORE_DISPADN)
            {
                ISTATIC_SetProperties(pStatic, ST_CENTERTEXT | ST_MIDDLETEXT |ST_UNDERLINE);
                ISTATIC_SetText(pStatic,pMe->m_pADNName,pMe->m_pADNNumber,AEE_FONT_NORMAL,AEE_FONT_NORMAL);
                ISTATIC_Redraw(pStatic);
                FREEIF(pMe->m_pADNName);
                FREEIF(pMe->m_pADNNumber);
                COREAPP_DRAW_BOTTOMBAR(BTBAR_OK_BACK) 
            }
            else if(pMe->m_nMsgID !=IDS_EXIT_EMERGENCY_MODE)
            {
                PromptMsg_Param_type m_PromptMsg;
                MEMSET(&m_PromptMsg,0,sizeof(PromptMsg_Param_type));
                m_PromptMsg.nMsgResID= pMe->m_nMsgID;
				
                if(pMe->m_nMsgID == IDS_AUTO_POWER_OFF)
                {
                    m_PromptMsg.ePMsgType = MESSAGE_CONFIRM;
                }
                else if(pMe->m_nMsgID == 0xFFFF)
                {
                    m_PromptMsg.nMsgResID = 0;
                    m_PromptMsg.ePMsgType = MESSAGE_WARNNING;
                    m_PromptMsg.pwszMsg = pMe->m_cdg_msgptr;
                }
                else
                {
                    m_PromptMsg.ePMsgType = MESSAGE_WARNNING;
                }

                if(pMe->m_nMsgID != 0xFFFF)
                {
                    STRLCPY(m_PromptMsg.strMsgResFile, AEE_COREAPPRES_LANGFILE,MAX_FILE_NAME);
                    if((pMe->m_nMsgID == IDS_FULLY_CHARGED) || (pMe->m_nMsgID == IDS_CHARGER_ON) || (pMe->m_nMsgID == IDS_WAITING))
                    {
                        m_PromptMsg.eBBarType = BTBAR_NONE;
                    }
                    else
                    {
                       m_PromptMsg.eBBarType = BTBAR_OK_BACK;
                    }
                }
                DrawPromptMessage(pMe->m_pDisplay,pStatic,&m_PromptMsg);
            }
            else
            {
                AECHAR  *wstrText = NULL;
                int32  nSize=0;
                nSize = 128 * sizeof(AECHAR);
                wstrText = (AECHAR *) MALLOC(nSize);
                if (NULL != wstrText)
                {
                    ISHELL_LoadResString(pMe->a.m_pIShell, AEE_COREAPPRES_LANGFILE,  pMe->m_nMsgID,wstrText,nSize);
                    ISTATIC_SetProperties(pStatic, ST_CENTERTEXT | ST_MIDDLETEXT);
                    ISTATIC_SetText(pStatic,NULL,wstrText,AEE_FONT_NORMAL,AEE_FONT_NORMAL);
                    FREEIF(wstrText);
                }                
                ISTATIC_Redraw(pStatic);
                COREAPP_DRAW_BOTTOMBAR(BTBAR_OK_BACK)
            }
            IDISPLAY_UpdateEx(pMe->m_pDisplay,FALSE);
            return TRUE;            
        }

        case EVT_DIALOG_END:
        {
            ISHELL_CancelTimer(pMe->a.m_pIShell, DialogTimeoutCallback,pMe);
            ISTATIC_Release(pStatic);
            pStatic = NULL;
            if(IDS_WAITING == pMe->m_nMsgID)
            {
                FREEIF(pMe->m_cdg_msgptr);
                return TRUE;
            }
#ifdef FEATURE_PLANEMODE
            if(IDS_PLANEMODE_QUERY == pMe->m_nMsgID)
            {
                FREEIF(pMe->m_cdg_msgptr);
                return TRUE;
            }
#endif

            // 此对话框返回值仅为 DLGRET_MSGOK ，为防止挂起 Applet 
            // 关闭对话框回到错误状态，显示给对话框返回值赋值
#ifdef FEATRUE_AUTO_POWER
            pMe->m_b_autopower_off = FALSE;
            /*if(pMe->m_nMsgID == IDS_AUTO_POWER_OFF)
            {
                pMe->m_eDlgRet = DLGRET_CREATE;
            }
            else
            {
                pMe->m_eDlgRet = DLGRET_MSGOK;
            }*/
            pMe->m_eDlgRet = DLGRET_MSGOK;

            if(pMe->m_b_needclose_core)
            {
                ISHELL_CloseApplet(pMe->a.m_pIShell, FALSE);
            }
#else
            pMe->m_eDlgRet = DLGRET_MSGOK;
#endif //#ifdef FEATRUE_AUTO_POWER
            FREEIF(pMe->m_cdg_msgptr);
            return TRUE;
        }
        case EVT_KEY_PRESS:
        case EVT_KEY_RELEASE:
            if(pMe->m_bVerifying && pMe->m_nMsgID == IDS_WAITING)
            {
                return TRUE;
            }
            break;
        case EVT_KEY:
        {
            if(pMe->m_bVerifying && pMe->m_nMsgID == IDS_WAITING)
            {
                return TRUE;
            }
            
            switch (wParam)
            {
                case AVK_SELECT:
                {
                    if(pMe->m_nMsgID ==IDS_EXIT_EMERGENCY_MODE)
                    {
#ifdef FEATURE_ICM
                        //exit the emergency mode
                        ICM_SetSystemPreference(pMe->m_pCM,
                                                        AEECM_MODE_PREF_PERSISTENT, AEECM_PREF_TERM_PERMANENT, 0,
                                                        AEECM_GW_ACQ_ORDER_PREF_NO_CHANGE, AEECM_BAND_PREF_NO_CHANGE,
                                                        AEECM_ROAM_PREF_NO_CHANGE, AEECM_HYBR_PREF_NO_CHANGE,
                                                        AEECM_SRV_DOMAIN_PREF_NO_CHANGE, AEECM_NETWORK_SEL_MODE_PREF_NO_CHANGE,
                                                        NULL, NULL, NULL);
#else
                    	AEETSystemPreference tSysPref={
														AEET_MODE_PREF_PERSISTENT, 
														AEET_PREF_TERM_PERMANENT,
														0,
								                        AEET_GW_ACQ_ORDER_PREF_NO_CHANGE,
								                        AEET_BAND_PREF_NO_CHANGE,
								                        AEET_ROAM_PREF_NO_CHANGE, 
								                        AEET_HYBR_PREF_NO_CHANGE,
								                        AEET_SRV_DOMAIN_PREF_NO_CHANGE, 
								                        AEET_NETWORK_SEL_MODE_PREF_NO_CHANGE,
							                            {0xFF,0xFF,0xFF}};  /*if we don,t use plmn set it to 0xff,0xff,0xff*/
		                //exit the emergency mode
		                IPHONECTL_SetSystemPreference(pMe->m_pIPhoneCtl,NULL,&tSysPref,sizeof(AEETSystemPreference),NULL);
#endif
                        pMe->m_bemergencymode = FALSE;
                    }
#ifdef FEATRUE_AUTO_POWER
                    else if(pMe->m_nMsgID == IDS_AUTO_POWER_OFF)
                    {                    	
                        ISHELL_CancelTimer(pMe->a.m_pIShell,DialogTimeoutCallback,pMe);                        
                        pMe->m_ePowerDownType = POWERDOWN_NORMAL;
                        if (pMe->m_eCurState != COREST_POWEROFF)
                        {
                            //ISHELL_CloseApplet(pMe->a.m_pIShell, TRUE);
                            pMe->m_b_needclose_core = FALSE;
                            pMe->m_wStartupAniTime = 0;
                            DBGPRINTF("IDS_AUTO_POWER_OFF to COREST_POWEROFF");
                            MOVE_TO_STATE(COREST_POWEROFF)
                            CLOSE_DIALOG(DLGRET_CREATE)
                            return TRUE;
                        }
                    }
#endif//#ifdef FEATRUE_AUTO_POWER
#ifdef FEATURE_PLANEMODE
                    else if(pMe->m_nMsgID == IDS_PLANEMODE_QUERY)
                    {
                        ISHELL_CancelTimer(pMe->a.m_pIShell,DialogTimeoutCallback,pMe);
                        pMe->m_SYS_MODE_NO_SRV = TRUE;  // 这里付值可以在idle界面骗过用户，planemode可以马上显示出来
                        CLOSE_DIALOG(DLGRET_YES)
                        return TRUE;
                    }
                    else if(pMe->m_MsgID == IDS_LOWBATTMSG_TEXT )
                    {
                        ISHELL_CancelTimer(pMe->a.m_pIShell,DialogTimeoutCallback,pMe);
                        return TRUE;
                    }
#endif
                }
                case AVK_SOFT2:		//Add By zzg 2010_09_08 for smart and m8
                case AVK_CLR:    
                    (void) ISHELL_CancelTimer(pMe->a.m_pIShell,DialogTimeoutCallback,pMe);                 
#ifdef FEATURE_PLANEMODE
                    if(pMe->m_nMsgID == IDS_PLANEMODE_QUERY)
                    {
                        CLOSE_DIALOG(DLGRET_NO);
                    }
                    else
#endif
                    {
                        CLOSE_DIALOG(DLGRET_MSGOK);
                    }
                    break;

                default:
                    break;
            }
            return TRUE;
        }

        case EVT_DISPLAYDIALOGTIMEOUT:
        {
#ifdef FEATRUE_AUTO_POWER
            if(pMe->m_nMsgID == IDS_AUTO_POWER_OFF)
            {
                pMe->m_ePowerDownType = POWERDOWN_NORMAL;
                if (pMe->m_eCurState != COREST_POWEROFF)
                {
                    //ISHELL_CloseApplet(pMe->a.m_pIShell, TRUE);
                    pMe->m_b_needclose_core = FALSE;
                    pMe->m_wStartupAniTime = 0;
                    DBGPRINTF("IDS_AUTO_POWER_OFF to COREST_POWEROFF");
                    MOVE_TO_STATE(COREST_POWEROFF)
                    CLOSE_DIALOG(DLGRET_CREATE)
                    return TRUE;
                }
            }
#endif//#ifdef FEATRUE_AUTO_POWER
#ifdef FEATURE_PLANEMODE
            if(pMe->m_nMsgID == IDS_PLANEMODE_QUERY)
            {
                CLOSE_DIALOG(DLGRET_NO);
                return TRUE;
            }
#endif
            CLOSE_DIALOG(DLGRET_MSGOK);
            return TRUE;
        }
        
        case EVT_CARD_STATUS:
            if(pMe->m_sPinActionStatus.nCmdStatus == SUCCESS)
            {
#ifndef FEATURE_OEMOMH
                (void) ISHELL_CancelTimer(pMe->a.m_pIShell,DialogTimeoutCallback,pMe);
                CLOSE_DIALOG(DLGRET_MSGOK);
                return TRUE;
#else
                pMe->m_nMsgID = IDS_MSG_OK;
#endif
            }
            else
            {
                // Section 27.16 of 3GPP 51.0.10
                if(pMe->m_sPinActionStatus.sStatus.sw1 == 0x92 && pMe->m_sPinActionStatus.sStatus.sw2 == 0x40)
                {
                    pMe->m_nMsgID = IDS_MSG_MEMISSUE;
                }
                else if(pMe->m_sPinActionStatus.sStatus.sw1 == 0x98 && pMe->m_sPinActionStatus.sStatus.sw2 == 0x04)
                {
                    pMe->m_nMsgID = IDS_MSG_SECISSUE;
                }
                else if(pMe->m_sPinActionStatus.sStatus.sw1 == 0x98 && pMe->m_sPinActionStatus.sStatus.sw2 == 0x40)
                {
                    pMe->m_nMsgID = IDS_UIMBLOCKED;
                }
                else if(pMe->m_sPinActionStatus.sStatus.sw1 == 0x6F)
                {
                    pMe->m_nMsgID = IDS_MSG_TECHISSUE;
                }
                else
                {
                    if(pMe->m_eRUIMSCode == ENTERPIN_ONLY)
                    {
                        pMe->m_nMsgID = IDS_INVALIDPIN;
                    }
                    else
                    {
                        pMe->m_nMsgID = IDS_INVALIDPUK;
                    }
                }
            }
            
            // 启动发送关闭对话框事件的定时器
            ISHELL_SetTimer(pMe->a.m_pIShell, 2000, DialogTimeoutCallback, pMe);
            ISHELL_PostEvent(pMe->a.m_pIShell, AEECLSID_CORE_APP, EVT_USER_REDRAW, 0, 0);
            return TRUE;
            
        default:
            break;
    }
    return FALSE;
}
/*==============================================================================
函数:
    IDD_ALARM_Handler
    
说明:
    IDD_ALARM 对话框事件处理函数
       
参数:
    pUser [in]: 这里必须是指向 Core Applet 对象结构的指针。
    eCode [in]: 事件代码。
    wParam: 事件相关数据。
    dwParam: 事件相关数据。
       
返回值:
    TRUE:  传入事件被处理。
    FALSE: 传入事件被忽略。
==============================================================================*/
#if defined( FEATURE_POWERDOWN_ALARM)
static void CoreApp_AlarmPowerDown( CCoreApp* pMe)
{
    pMe->m_ePowerDownType = POWERDOWN_NORMAL;
    if (pMe->m_eCurState != COREST_POWEROFF)
    {
        pMe->m_wStartupAniTime = 0;
        DBGPRINTF("Alarm Process to COREST_POWEROFF");
        MOVE_TO_STATE(COREST_POWEROFF)
        CLOSE_DIALOG(DLGRET_CREATE)
    }
}
static void defaultProcessAlarm( void* pUser)
{
#ifndef WIN32
    extern void snoozePowerdownAlarmclock( void);
    snoozePowerdownAlarmclock();
#endif
    CoreApp_AlarmPowerDown( (CCoreApp *)pUser);
}

static boolean  IDD_ALARM_Handler(void       *pUser,
                                AEEEvent   eCode,
                                uint16     wParam,
                                uint32     dwParam)
{
    //static IImage * pIImage = NULL;
    static byte keyBeepVolumeSetting = 0;
    static PowerDown_Alarm_Cfg time = {0};

    CCoreApp *pMe = (CCoreApp *)pUser;
    MSG_FATAL("IDD_ALARM_Handler-----eCode=%d---wParam=%d",eCode,wParam,0);
    
#ifdef FEATURE_LCD_TOUCH_ENABLE
    if(eCode ==EVT_PEN_UP)
        {
            int16 wXPos = (int16)AEE_GET_X((const char *)dwParam);
            int16 wYPos = (int16)AEE_GET_Y((const char *)dwParam);
            AEEDeviceInfo devinfo;
            int nBarH ;
            AEERect rc;
             
            nBarH = GetBottomBarHeight(pMe->m_pDisplay);
            MEMSET(&devinfo, 0, sizeof(devinfo));
            ISHELL_GetDeviceInfo(pMe->a.m_pIShell, &devinfo);
            SETAEERECT(&rc, 0, devinfo.cyScreen-nBarH, devinfo.cxScreen, nBarH);  
            if(CORE_PT_IN_RECT(wXPos,wYPos,rc))
            {
                if(wXPos >= rc.x && wXPos < rc.x + (rc.dx/3) )//左
                {
                    eCode=EVT_KEY;
                    wParam=AVK_SELECT;
                } 
                else if(wXPos >= rc.x + (rc.dx/3)*2 && wXPos < rc.x + (rc.dx/3)*3 )//右
                { 
                    eCode=EVT_KEY;
                    wParam=AVK_CLR;
                }
            } 
        }                             
#endif            
    switch (eCode)
    {
        case EVT_DIALOG_INIT:
            OEM_GetConfig( CFGI_POWERDOWN_ALARM, (void *)&time, sizeof(time));

            ISHELL_SetTimer( pMe->a.m_pIShell, 60000, defaultProcessAlarm, pMe);
            return TRUE;

        case EVT_DIALOG_START:
        {
            byte mute = OEMSOUND_MUTE_VOL;
            if(pMe->m_pIAnn != NULL)
            {
                IANNUNCIATOR_SetFieldIsActiveEx(pMe->m_pIAnn,FALSE);
            }
            ISHELL_PostEvent(pMe->a.m_pIShell,AEECLSID_CORE_APP,EVT_USER_REDRAW,0,0);
            ICONFIG_GetItem( pMe->m_pConfig, CFGI_BEEP_VOL, &keyBeepVolumeSetting, sizeof(byte));
            ICONFIG_SetItem( pMe->m_pConfig, CFGI_BEEP_VOL, &mute, sizeof(byte));
			
            return TRUE;
        }

        case EVT_USER_REDRAW:
        {
            uint32  dwAlarmTime;
            AECHAR  wszTime[16];
            IImage* pResImg = NULL;
            uint16  ring_id = 1;
            byte    profilenum;
            ringID  ringid[PROFILENUMBER];
            AEERect rc = {0};
            RGBVAL nOldFontColor;
            
            MEMSET(wszTime,0,sizeof(wszTime));
            dwAlarmTime = time.dwWATime;
            Appscommon_FormatTimeString(dwAlarmTime, wszTime, sizeof(wszTime));
			pResImg = ISHELL_LoadResImage( pMe->a.m_pIShell, "fs:/mod/clockapps/clockapps_images.bar", IDI_ALARMCLOCK);
            Appscommon_ResetBackgroundEx(pMe->m_pDisplay, &pMe->m_rc, TRUE);
            if( pResImg != NULL)
            {
                AEEImageInfo ImageInfo;
                
                IIMAGE_GetInfo(pResImg, &ImageInfo);
                IIMAGE_Draw(pResImg, (pMe->m_rc.dx - ImageInfo.cx)/2, 2*TITLEBAR_HEIGHT + (pMe->m_rc.dy - BOTTOMBAR_HEIGHT - TITLEBAR_HEIGHT- ImageInfo.cy)/2);
                IIMAGE_Release( pResImg);
                pResImg = NULL;
            }

            SETAEERECT(&rc, 0, TITLEBAR_HEIGHT, pMe->m_rc.dx, TITLEBAR_HEIGHT);
            nOldFontColor = IDISPLAY_SetColor( pMe->m_pDisplay, CLR_USER_TEXT, RGB_WHITE);
            (void)IDISPLAY_DrawText(pMe->m_pDisplay, AEE_FONT_BOLD, wszTime, -1, 0, 0, &rc,
                                  IDF_ALIGN_MIDDLE | IDF_ALIGN_CENTER | IDF_TEXT_TRANSPARENT);
            (void)IDISPLAY_SetColor( pMe->m_pDisplay, CLR_USER_TEXT, nOldFontColor);
#if 1
            OEM_GetConfig( CFGI_ALARM_RINGER, &ring_id, sizeof(ring_id));

            ICONFIG_GetItem(pMe->m_pConfig, CFGI_PROFILE_CUR_NUMBER, &profilenum, sizeof(profilenum));
            ICONFIG_GetItem(pMe->m_pConfig, CFGI_PROFILE_ALARM_RINGER, (void*)ringid, sizeof(ringid));
            if(ringid[profilenum].ringType == OEMNV_MID_RINGER)
            {
                IALERT_StartRingerAlert(pMe->m_pAlert, ring_id);
            }
            else
            {
                if ((IALERT_StartMp3Alert(pMe->m_pAlert, ringid[profilenum].szMusicname, ALERT_NORMAL_SND) != SUCCESS))
                {
                    IALERT_StartRingerAlert(pMe->m_pAlert, (uint32)ring_id);
                }
            }
#else
            ICONFIG_GetItem( pMe->m_pConfig,CFGI_ALARM_RINGER,&ring_id,sizeof(ring_id));
            IALERT_StartRingerAlert(pMe->m_pAlert, ring_id);
#endif
            {
                TitleBar_Param_type title = {0};
                AECHAR wszTitle[16] = {0};
                int len = 0;

                ISHELL_LoadResString( pMe->a.m_pIShell,
                                      CLOCK_RES_PATH,
                                      IDS_ALARMCLOCK_TITLE,
                                      wszTitle,
                                      sizeof(wszTitle)
                                  );
                len = WSTRLEN( wszTitle);
                WSPRINTF( wszTitle + len, sizeof( wszTitle) - len*2, L"%d", time.alarm_id + 1);
                MEMSET(&title,0,sizeof(TitleBar_Param_type));
                //title.dwAlignFlags = IDF_TEXT_TRANSPARENT | IDF_ALIGN_CENTER;
                title.dwAlignFlags = IDF_TEXT_TRANSPARENT | IDF_ALIGN_CENTER | IDF_ALIGN_MIDDLE;
                title.pwszTitle = wszTitle;
				#if 0
                DrawTitleBar(pMe->a.m_pIDisplay,&title);
				#else
                if(pMe->m_pIAnn != NULL)
                {
				    IANNUNCIATOR_SetFieldText(pMe->m_pIAnn,wszTitle);
                }
				#endif
                CoreDrawBottomBar( BTBAR_SNOOZE_CONTINUE_STOP);
            }
            IDISPLAY_Update( pMe->a.m_pIDisplay);
            return TRUE;
        }

        case EVT_DIALOG_END:
            IBACKLIGHT_Enable(pMe->m_pBacklight);
#ifdef FEATURE_LED_CONTROL
            IBACKLIGHT_SigLedDisable( pMe->m_pBacklight);
#endif
#if !defined( AEE_SIMULATOR)
            IALERT_StopRingerAlert(pMe->m_pAlert);
            IALERT_StopMp3Alert(pMe->m_pAlert);
#endif
            ICONFIG_SetItem( pMe->m_pConfig, CFGI_BEEP_VOL, &keyBeepVolumeSetting, sizeof(byte));
			ISHELL_CancelTimer( pMe->a.m_pIShell, defaultProcessAlarm, pMe);
            return TRUE;

        case EVT_KEY:
        {
            if(wParam == AVK_INFO)
            {
#if !defined( AEE_SIMULATOR)
                IALERT_StopRingerAlert(pMe->m_pAlert);
#endif

                CLOSE_DIALOG(DLGRET_OK)
            }
            else if( wParam == AVK_SELECT)
            {
                MSG_FATAL("IDD_ALARM_Handler-----AVK_SELECT",0,0,0);
                defaultProcessAlarm( pMe);
            }
            else if( (wParam == AVK_CLR) || (wParam == AVK_SOFT2))	//Add By zzg 2010_09_08 for smart/m8
            {
                CoreApp_AlarmPowerDown( pMe);
            }
        }
        return TRUE;


        case EVT_COMMAND:
            return TRUE;

        default:
            break;
    }
    return FALSE;
} // IDD_ALARM_Handler

#endif

#ifdef FEATURE_USES_LOWMEM
#include "AEERGBVAL.h"
#define BATT_HEIGHT     80
#define BATT_WIDTH      60

static void CoreApp_DrawChargeing(CCoreApp *pMe, int iLevel)
{
    AEERect bgRect;
    AEERect fgRect;
    AEERect hdRect;
    uint16  chHeight;
    byte    R,G;

    SETAEERECT(&bgRect, pMe->m_rc.x+((pMe->m_rc.dx-BATT_WIDTH)>>1), pMe->m_rc.y+((pMe->m_rc.dy-BATT_HEIGHT)>>1), BATT_WIDTH, BATT_HEIGHT);
    fgRect  = bgRect;
    hdRect.dx = BATT_WIDTH/5;
    hdRect.dy = BATT_HEIGHT/15;
    hdRect.x  = bgRect.x+(bgRect.dx-hdRect.dx)/2;
    hdRect.y  = bgRect.y-hdRect.dy;
    chHeight = BATT_HEIGHT/CHARGING_FRAME_COUNT;
    fgRect.dy = chHeight*(iLevel+1);
    fgRect.y  = bgRect.y + chHeight*(CHARGING_FRAME_COUNT-(iLevel+1));

    IDISPLAY_DrawRect(pMe->m_pDisplay, &bgRect, RGB_NONE, RGB_WHITE, IDF_RECT_FILL);
    G = 0xFF*iLevel/(CHARGING_FRAME_COUNT-1);
    R = 0xFF - G;
    IDISPLAY_DrawRect(pMe->m_pDisplay, &fgRect, RGB_NONE,  MAKE_RGB(R, G, 0), IDF_RECT_FILL);
    IDISPLAY_DrawRect(pMe->m_pDisplay, &bgRect, RGB_BLACK, RGB_NONE, IDF_RECT_FRAME);
    IDISPLAY_DrawRect(pMe->m_pDisplay, &hdRect, RGB_NONE, RGB_BLACK, IDF_RECT_FILL);
    IDISPLAY_Update(pMe->m_pDisplay);
}
#endif
/*==============================================================================
函数:
    IDD_LPM_Handler
    
说明:
    IDD_LPM 对话框事件处理函数
       
参数:
    pUser [in]: 这里必须是指向 Core Applet 对象结构的指针。
    eCode [in]: 事件代码。
    wParam: 事件相关数据。
    dwParam: 事件相关数据。
       
返回值:
    TRUE:  传入事件被处理。
    FALSE: 传入事件被忽略。
       
备注:
    函数作卡的 PIN 码、 PUK 码的输入界面处理。
       
==============================================================================*/
static boolean  IDD_LPM_Handler(void       *pUser,
                                AEEEvent   eCode,
                                uint16     wParam,
                                uint32     dwParam)
{
    CCoreApp *pMe = (CCoreApp *)pUser;
	MSG_FATAL("IDD_LPM_Handler:eCode=%x,wParam=%x,dwParam=%x",eCode, wParam, dwParam);
    
    switch (eCode)
    {
        case EVT_DIALOG_INIT:
            pMe->m_battery_count = 0;
#ifndef FEATURE_USES_LOWMEM
            pMe->m_battery_Image = ISHELL_LoadImage(pMe->a.m_pIShell, CHARGING_ANIFILE "charge.png");
            IIMAGE_SetParm(pMe->m_battery_Image, IPARM_NFRAMES, CHARGING_FRAME_COUNT, 0);
#endif
            return TRUE;
            
        case EVT_DIALOG_START:
			{
				// 在此完成闹钟的开机初始化。初始化过程只需执行一次
          		static boolean bNotInitedAlarmLPM = TRUE;
            	//IANNUNCIATOR_EnableAnnunciatorBar(pMe->m_pIAnn,AEECLSID_DISPLAY1,FALSE);
            	ISHELL_PostEvent(pMe->a.m_pIShell,AEECLSID_CORE_APP,EVT_USER_REDRAW,0,0);
            	//Add By zzg 2010_08_11之前是注释着的
				if (bNotInitedAlarmLPM)
          		{
#ifdef FEATURE_UIALARM
              		// 直道取得有效时间才开始启动警报器
            		CoreApp_EnableShellAlarms(pMe, TRUE);  //add by yangdecai 2010-08-12
#endif
             		bNotInitedAlarmLPM = FALSE;
           		}
        	}
            
#ifdef CUST_EDITION    
            {
                extern void CoreApp_InitBattStatus(CCoreApp * pMe);
                ISHELL_SetTimer(pMe->a.m_pIShell, 3*1000,(PFNNOTIFY)CoreApp_InitBattStatus,  pMe);                                                                                                         
            }
#endif
            //Add
            return TRUE;
            
        case EVT_USER_REDRAW:
        case EVT_UPDATEIDLE:
        {
            AEEBatteryChargerStatus status;
            AECHAR  *wszText=NULL;
            int32   nSize;

            nSize = 32 * sizeof(AECHAR);
            wszText =  (AECHAR *) MALLOC(nSize);
            wszText[0] = 0;
            status = IBATTERY_GetChargerStatus(pMe->m_pBatt);
			
            IDISPLAY_ClearScreen(pMe->m_pDisplay);      

            if (AEEBATTERY_CHARGERSTATUS_FULLY_CHARGE == status)
            {                
                ISHELL_CancelTimer(pMe->a.m_pIShell, CoreApp_Draw_Charger_image, pMe);
#ifndef FEATURE_USES_LOWMEM
                if (NULL != pMe->m_battery_Image)
                {
                    IIMAGE_DrawFrame(pMe->m_battery_Image, CHARGING_FRAME_COUNT - 1, 0, 0);
                }                
#else
                CoreApp_DrawChargeing(pMe, CHARGING_FRAME_COUNT - 1);
#endif
            } 
            else if (AEEBATTERY_CHARGERSTATUS_OVERVOLTAGE == status) 
            {
                ISHELL_CancelTimer(pMe->a.m_pIShell, CoreApp_Draw_Charger_image, pMe);
                if (NULL != wszText)
                {
                    (void) ISHELL_LoadResString(pMe->a.m_pIShell,
                                                AEE_COREAPPRES_LANGFILE,
                                                IDS_OVERVOLTAGE,
                                                wszText,
                                                nSize);
                }
            }
            else // AEEBATTERY_CHARGERSTATUS_CHARGING
            {
                CoreApp_Draw_Charger_image(pMe);
            }

            // 绘制提示文本
            if (NULL != wszText && wszText[0] != 0)
            {
                (void)IDISPLAY_DrawText(pMe->m_pDisplay,
                    AEE_FONT_BOLD,
                    wszText, -1,
                    0, 0, &pMe->m_rc, 
                    IDF_TEXT_TRANSPARENT | IDF_ALIGN_CENTER | IDF_ALIGN_TOP);
                
                //FREE ((void*) wszText);
            }

            FREEIF ((void*) wszText);
            IDISPLAY_UpdateEx(pMe->m_pDisplay,FALSE);
            return TRUE;
        }
            
            
        case EVT_DIALOG_END:
        {
            //int i = 0;
            ISHELL_CancelTimer(pMe->a.m_pIShell, CoreApp_Draw_Charger_image, pMe);
#ifndef FEATURE_USES_LOWMEM
            if (NULL != pMe->m_battery_Image)
            {
                IIMAGE_Release(pMe->m_battery_Image);
                pMe->m_battery_Image = NULL;
            }
#endif
            return TRUE;
        }
        
        case EVT_KEY_HELD:
            switch (wParam)
            {
                case AVK_END:
                #ifdef FEATURE_VERSION_M8P
                case AVK_CLR:
                //case AVK_SOFT1:
                #endif
                    CLOSE_DIALOG(DLGRET_OK)                  
                    return TRUE;               
                default:
                    break;
            }
            
            return TRUE;

        case EVT_KEY:
        case EVT_KEY_PRESS:
        case EVT_KEY_RELEASE:
            return TRUE;
            
        default:
            break;
    }
    
    return FALSE;
} // IDD_LPM_Handler

/*==============================================================================
函数:
    IDD_EMERGENCYNUMLIST_Handler
    
说明:
    IDD_EMERGENCYNUMLIST 对话框事件处理函数
       
参数:
    pUser [in]: 这里必须是指向 Core Applet 对象结构的指针。
    eCode [in]：事件代码。
    wParam：事件相关数据。
    dwParam：事件相关数据。
       
返回值:
    TRUE: 传入事件被处理。
    FALSE: 传入事件被忽略。
       
备注:
       
==============================================================================*/
static boolean  IDD_EMERGENCYNUMLIST_Handler(void  *pUser,
                                 AEEEvent   eCode,
                                 uint16     wParam,
                                 uint32     dwParam)
{
    CCoreApp *pMe = (CCoreApp *)pUser;
    IMenuCtl *pMenu = NULL;
    
    if (NULL == pMe)
    {
        return FALSE;
    }
    MSG_FATAL("IDD_EMERGENCYNUMLIST_Handler--eCode=%d----wParam=%d---dwParam=%d",eCode,wParam,dwParam);

    pMenu = (IMenuCtl* )IDIALOG_GetControl(pMe->m_pActiveDlg, IDC_EMERGENCYNUM);
    if (pMenu == NULL)
    {
        return FALSE;
    } 

    switch (eCode)
    {
        case EVT_DIALOG_INIT:
            return TRUE;

        case EVT_DIALOG_START:            
            {
                //AEERect rc;
                AECHAR  wstrNum[MAX_EMERGENCY_NUM_LEN+1];
                EmergencyNum_Table emerg_tab;
                //EmergencyNumber pList[3];
                //EmergencyNumber pTepItem ;
                //EmergencyNumber pItem;
                int   i = 0;
				MSG_FATAL("IDD_EMERGENCYNUMLIST_Handler....................................",0,0,0);
                ICONFIG_GetItem(pMe->m_pConfig, CFGI_EMERGENCYNUM_TABLE, (void*)&emerg_tab, sizeof(EmergencyNum_Table));
                
                IMENUCTL_SetProperties(pMenu, MP_UNDERLINE_TITLE|MP_WRAPSCROLL);
                IMENUCTL_SetOemProperties( pMenu, OEMMP_USE_MENU_STYLE);
#ifdef FEATURE_CARRIER_CHINA_VERTU
                IMENUCTL_SetBackGround(pMenu, AEE_APPSCOMMONRES_IMAGESFILE, IDI_SECURITY_BACKGROUND);
#endif
				MSG_FATAL("IDD_EMERGENCYNUMLIST_Handler............................SIZE:%d",emerg_tab.emert_size,0,0);
                for(i=0; i<emerg_tab.emert_size; i++)
                {
                    wstrNum[0] = 0;
                    STRTOWSTR(emerg_tab.emerg_num[i].num_buf, wstrNum, sizeof(wstrNum));
                    IMENUCTL_AddItem(pMenu, NULL, NULL, 500+i, wstrNum, NULL);
                }
                //SETAEERECT(&rc,  0, 0, pMe->m_rc.dx, pMe->m_rc.dy - (BOTTOMBAR_HEIGHT + IDLE_ICON_HEIGHT));
                //IMENUCTL_SetRect(pMenu, &rc);

                //pList = AEEOEM_GetEmergencyNumList(pMe->m_pOEM_TSGBridge);
                
                //pList[0].num_buf = "10010";
                //pList[0].num_len = 5;

                //pList[1].num_buf = "10011";
                //pList[1].num_len = 5;

                //pList[2].num_buf = "10012";
                //pList[2].num_len = 5;

                //for (j=0; j<3; j++)
                //{
                //    pTepItem = pList[j];
                //    pItem = pTepItem;

                 //   if ((pItem.num_len == 0) || (pItem.num_buf == NULL))
                 //   {
                 //       break;
                 //   }
                           
                 //   wstrNum[0] = 0;
                 //   (void)STRTOWSTR(pItem.num_buf, wstrNum, sizeof(wstrNum));
                 //   (void)IMENUCTL_AddItem(pMenu, NULL, NULL, 500+i, wstrNum, NULL);
                 //   i++;                   
                //}                 
            }
            ISHELL_PostEvent(pMe->a.m_pIShell,AEECLSID_CORE_APP,EVT_USER_REDRAW, 0,0);
            return TRUE;

        case EVT_USER_REDRAW:
            // 绘制底条提示
            // Select       Back
            //CoreDrawBottomBar(BTBAR_SELECT_BACK)
            IMENUCTL_SetBottomBarType(pMenu,BTBAR_SELECT_BACK);
            (void)IMENUCTL_Redraw(pMenu);
            return TRUE;

        case EVT_DIALOG_END:
            if (pMe->m_eDlgRet != DLGRET_CANCELED)
            {
                pMe->m_eDlgRet = DLGRET_CANCELED;
            }
            AEE_CancelTimer(CoreApp_Issametimer,pMe);
    		AEE_SetTimer(2000,CoreApp_Issametimer,pMe);
            return TRUE;
        MSG_FATAL("IDD_EMERGENCYNUMLIST_Handler EVT_KEY...........eCode=%d",eCode,0,0);
        case EVT_KEY_RELEASE:
        case EVT_KEY_PRESS:
        	return TRUE;
        case EVT_KEY:
            switch (wParam)
            {
                case AVK_END:
                case AVK_SOFT2:		//Add By zzg 2010_09_08 for smart and m8
                case AVK_CLR:
                     MSG_FATAL("IDD_EMERGENCYNUMLIST_Handler EVT_KEY...........eCode=%d",eCode,0,0);
                	if(!pMe->m_IsSametime)
                	{
                         MSG_FATAL("IDD_EMERGENCYNUMLIST_Handler EVT_KEY...........eCode=%d",eCode,0,0);
                    	pMe->m_IsSametime = TRUE;
                    	CLOSE_DIALOG(DLGRET_CANCELED)
                    }
                    return TRUE;
  				case AVK_CAMERA:
            	#if defined(FEATURE_VERSION_C306)||defined(FEATURE_VERSION_W0216A)|| defined(FEAUTRE_VERSION_N450)|| defined(FEATURE_VERSION_C01)|| defined(FEATURE_VERSION_W516) || defined(FEATURE_VERSION_W027)
            	{
				nv_item_type	SimChoice;
				OEMNV_Get(NV_SIM_SELECT_I,&SimChoice);
				//if(SimChoice.sim_select == AVK_SEND_TWO)
				{
                    CtlAddItem ai;
                    uint16   wID;
                    
                    wID = IMENUCTL_GetSel(pMenu);
                    if (IMENUCTL_GetItem(pMenu, wID, &ai))
                    {
                       (void)MakeVoiceCall(pMe->a.m_pIShell, FALSE, (AECHAR *)ai.pText);
                    }
                    return TRUE;
				}
				}
				#endif
				break;
                case AVK_SEND:
                    {
                        CtlAddItem ai;
                        uint16   wID;
                        if(!pMe->m_IsSametime)
                	    {
                	    	pMe->m_IsSametime = TRUE;
                	    }
                	    else
                	    {
                	    	return TRUE;
                	    }
                        #if defined(FEATURE_VERSION_C01) 
						{
							nv_item_type	SimChoice;
							OEMNV_Get(NV_SIM_SELECT_I,&SimChoice);
							if(SimChoice.sim_select==AVK_SEND_TWO)
							{
								return TRUE;
							}
						}
						#endif
                        wID = IMENUCTL_GetSel(pMenu);
                        if (IMENUCTL_GetItem(pMenu, wID, &ai))
                        {
                           (void)MakeVoiceCall(pMe->a.m_pIShell, FALSE, (AECHAR *)ai.pText);
                        }
                    }
                    return TRUE;
                    
                default:
                    break;
            }
            return TRUE;

        case EVT_COMMAND:
            {
                CtlAddItem ai;
                wParam =IMENUCTL_GetSel(pMenu);
                if (IMENUCTL_GetItem(pMenu, wParam, &ai))
                {
                     (void)MakeVoiceCall(pMe->a.m_pIShell, FALSE, (AECHAR *)ai.pText);
                }
            }
            return TRUE;

        default:
            break;
    }

    return FALSE;
} // IDD_EMERGENCYNUMLIST_Handler

/*==============================================================================
函数:
    IDD_PWDINPUT_Handler
    
说明:
    IDD_PWDINPUT 对话框事件处理函数
       
参数:
    pUser [in]: 这里必须是指向 Core Applet 对象结构的指针。
    eCode [in]: 事件代码。
    wParam: 事件相关数据。
    dwParam: 事件相关数据。
       
返回值:
    TRUE:  传入事件被处理。
    FALSE: 传入事件被忽略。
       
备注:
    函数作卡的 PIN 码、 PUK 码的输入界面处理。
       
==============================================================================*/
static boolean  IDD_PWDINPUT_Handler(void       *pUser,
                                     AEEEvent   eCode,
                                     uint16     wParam,
                                     uint32     dwParam)
{
    //IMenuCtl *pMenu = NULL;
    CCoreApp *pMe = (CCoreApp *)pUser;
    
    if (NULL == pMe)
    {
        return FALSE;
    }
     MSG_FATAL("IDD_PWDINPUT_Handler--eCode=%d----wParam=%d---dwParam=%d",eCode,wParam,dwParam);
    //pMenu = (IMenuCtl*)IDIALOG_GetControl(pMe->m_pActiveDlg, IDC_PHONEPWD_MENU);
    //if (NULL == pMenu)
    //{
    //    return FALSE;
    //}
#ifdef FEATURE_LCD_TOUCH_ENABLE
    if(eCode ==EVT_PEN_UP)
        {
            int16 wXPos = (int16)AEE_GET_X((const char *)dwParam);
            int16 wYPos = (int16)AEE_GET_Y((const char *)dwParam);
            AEEDeviceInfo devinfo;
            int nBarH ;
            AEERect rc;
            MSG_FATAL("Setting_HandleAuto_Power_DialogEvent wXPos=%d ,wYPos=%d",wXPos,wYPos,0);
             
            nBarH = GetBottomBarHeight(pMe->m_pDisplay);
            MEMSET(&devinfo, 0, sizeof(devinfo));
            ISHELL_GetDeviceInfo(pMe->a.m_pIShell, &devinfo);
            SETAEERECT(&rc, 0, devinfo.cyScreen-nBarH, devinfo.cxScreen, nBarH);  
            if(CORE_PT_IN_RECT(wXPos,wYPos,rc))
            {
                if(wXPos >= rc.x && wXPos < rc.x + (rc.dx/3) )//左
                {
                    eCode=EVT_KEY;
                    wParam=AVK_SELECT;
                } 
                else if(wXPos >= rc.x + (rc.dx/3)*2 && wXPos < rc.x + (rc.dx/3)*3 )//右
                { 
                    eCode=EVT_KEY;
                    wParam=AVK_CLR;
                }
            }
            else
            {
                MEMSET(pMe->m_strPhonePWDtemp, 0, PINCODE_LENGTH + 1);
                ISHELL_StartAppletArgs(pMe->a.m_pIShell, AEECLSID_APP_SECURITYMENU, "InputPW");
            }  
        }
                                        
#endif            
    
    switch (eCode)
    {
        case EVT_DIALOG_INIT:
            MEMSET(pMe->m_strPhonePWD, 0, sizeof(pMe->m_strPhonePWD));
            return TRUE;
            
        case EVT_DIALOG_START:
            (void) ISHELL_PostEvent(pMe->a.m_pIShell,
                                    AEECLSID_CORE_APP,
                                    EVT_USER_REDRAW,
                                    0,
                                    0);
            //if (NULL != pMenu)
            //{
            //     (void)IMENUCTL_SetTitle(pMenu, AEE_COREAPPRES_LANGFILE, IDS_ENTER_PHONELOCK, NULL);
            //}

            return TRUE;
            
        case EVT_USER_REDRAW:
            // 绘制相关信息
            {
                AECHAR  wstrDisplay[PHONEPASSWORDLENTH+1] = {0};
                char    strDisplay[PHONEPASSWORDLENTH+1] = {0};
                int xOffset = 5, nLen = 0;
                AECHAR  text[32] = {0};
                RGBVAL nOldFontColor;
                TitleBar_Param_type  TitleBar_Param = {0};
                MSG_FATAL("IDD_PWDINPUT_Handler--eCode=%d----wParam=%d---dwParam=%d",eCode,wParam,dwParam);
                // 先清屏
#ifdef FEATURE_CARRIER_CHINA_VERTU
                {
                    IImage *pImageBg = ISHELL_LoadResImage(pMe->a.m_pIShell, AEE_APPSCOMMONRES_IMAGESFILE, IDI_SECURITY_BACKGROUND);
                    
                    Appscommon_ResetBackground(pMe->m_pDisplay, pImageBg, APPSCOMMON_BG_COLOR, &pMe->m_rc, 0, 0);
                    if(pImageBg != NULL)
                    {
                        IImage_Release(pImageBg);
                    }
                }
#else
                Appscommon_ResetBackgroundEx(pMe->m_pDisplay, &pMe->m_rc, TRUE);
#endif
                //IDISPLAY_FillRect  (pMe->m_pDisplay,&pMe->m_rc, RGB_BLACK);
                
                // 画标题条
                (void)ISHELL_LoadResString(pMe->a.m_pIShell, 
                                                            AEE_COREAPPRES_LANGFILE,
                                                            IDS_ENTER_PHONELOCK, 
                                                            text,
                                                            sizeof(text));
                TitleBar_Param.pwszTitle = text;
                TitleBar_Param.dwAlignFlags = IDF_ALIGN_MIDDLE | IDF_ALIGN_CENTER | IDF_ALIGN_MIDDLE;
                DrawTitleBar(pMe->m_pDisplay, &TitleBar_Param);
                
                (void)ISHELL_LoadResString(pMe->a.m_pIShell, 
                                                AEE_COREAPPRES_LANGFILE,
                                                IDS_ENTER_PASSWORD, 
                                                text,
                                                sizeof(text));
                nOldFontColor = IDISPLAY_SetColor(pMe->m_pDisplay, CLR_USER_TEXT, RGB_WHITE);
                IDISPLAY_DrawText(pMe->m_pDisplay, 
                                    AEE_FONT_BOLD, 
                                    text,
                                    -1, 
                                    xOffset, 
                                    TITLEBAR_HEIGHT + MENUITEM_HEIGHT*1/2,
                                    NULL, 
                                    IDF_TEXT_TRANSPARENT);
#ifdef FEATURE_LCD_TOUCH_ENABLE                
                if(STRLEN(pMe->m_strPhonePWDtemp)>0)
                {
                    (void) STRCPY( pMe->m_strPhonePWD,pMe->m_strPhonePWDtemp);
                }
#endif                
                // 绘制输入
                nLen = STRLEN(pMe->m_strPhonePWD);
                MEMSET(strDisplay, '*', nLen);
                strDisplay[nLen] = '|';
                strDisplay[nLen + 1] = '\0';
                (void) STRTOWSTR(strDisplay, wstrDisplay, sizeof(wstrDisplay));
                IDISPLAY_DrawText(pMe->m_pDisplay, 
                                AEE_FONT_BOLD, 
                                wstrDisplay,
                                -1, 
                                2*xOffset, 
                                TITLEBAR_HEIGHT + MENUITEM_HEIGHT*3/2,
                                NULL, 
                                IDF_TEXT_TRANSPARENT);
                (void)IDISPLAY_SetColor(pMe->m_pDisplay, CLR_USER_TEXT, nOldFontColor);
                #ifndef FEATURE_ALL_KEY_PAD    //add by yangdecai 
                // 绘制底条提示
                if (nLen > 3)
                {// 确定-----删除
                    CoreDrawBottomBar(BTBAR_OK_DELETE)
                }
                else if(nLen > 0)
                {// 删除
                    CoreDrawBottomBar(BTBAR_DELETE)
                }
                else
                #else
                // 绘制底条提示
                if (nLen > 3)
                {// 确定-----删除
                    //CoreDrawBottomBar(BTBAR_OK_BACK)
                    CoreDrawBottomBar(BTBAR_OK_SOS)
                }
                else if(nLen > 0)
                {// 删除
                    CoreDrawBottomBar(BTBAR_SOS)
                }
                else
                #endif
                {// SOS
                    CoreDrawBottomBar(BTBAR_SOS)
                }
            }
            // 更新显示
            IDISPLAY_UpdateEx(pMe->m_pDisplay, FALSE);  
            return TRUE;
            
        case EVT_DIALOG_END:
            #ifdef FEATURE_LCD_TOUCH_ENABLE
            MEMSET(pMe->m_strPhonePWDtemp, 0, PINCODE_LENGTH + 1);
            #endif
            return TRUE;

        case EVT_KEY:
            {
                char  chEnter = 0;
                int   nLen = 0;
                boolean bRedraw = FALSE;
                
                switch (wParam)
                {
                    case AVK_0:
                    case AVK_1:
                    case AVK_2:
                    case AVK_3:
                    case AVK_4:
                    case AVK_5:
                    case AVK_6:
                    case AVK_7:
                    case AVK_8:
                    case AVK_9:
                        chEnter = '0' + (wParam - AVK_0);
                        break;

                    case AVK_STAR:
                        chEnter = '*';
                        break;
 
                    case AVK_POUND:
                        chEnter = '#';
                        break;
                    case AVK_CAMERA:
                    	#if defined(FEATURE_VERSION_C306) ||defined(FEATURE_VERSION_W0216A)|| defined(FEAUTRE_VERSION_N450)|| defined(FEATURE_VERSION_N021)|| defined(FEATURE_VERSION_C01)
                    	{
						nv_item_type	SimChoice;
						OEMNV_Get(NV_SIM_SELECT_I,&SimChoice);
						if(SimChoice.sim_select ==AVK_SEND_TWO)
						{
							CLOSE_DIALOG(DLGRET_EMGCALL)
                        	return TRUE;
						}
						}
						#endif
						break;
                    case AVK_SEND:
                    	#if defined(FEATURE_VERSION_C01) 
						{
							nv_item_type	SimChoice;
							OEMNV_Get(NV_SIM_SELECT_I,&SimChoice);
							if(SimChoice.sim_select==AVK_SEND_TWO)
							{
								return TRUE;
							}
						}
						#endif
                        CLOSE_DIALOG(DLGRET_EMGCALL)
                        return TRUE;

                    #ifdef FEATURE_ALL_KEY_PAD
                    case AVK_DEL:
                    {
                        chEnter = 0;                        
                        break;
                    }
                    #endif

                    case AVK_SOFT2:		//Add By zzg 2010_09_08 for smart and m8    
                    case AVK_CLR:
                        chEnter = 0;
                        #ifdef FEATURE_LCD_TOUCH_ENABLE
                        MEMSET(pMe->m_strPhonePWDtemp, 0, PINCODE_LENGTH + 1);
                        #endif
                        #ifndef FEATURE_ALL_KEY_PAD    //add by yangdecai 
                        if (STRLEN(pMe->m_strPhonePWD) == 0)
                        {
                            CLOSE_DIALOG(DLGRET_EMGCALL)
                            return TRUE;
                        }
                        #else
                        if(dwParam == 0)
                        {
                        	CLOSE_DIALOG(DLGRET_EMGCALL)
                            return TRUE;
                        }
                        else
                        {
                        	if (STRLEN(pMe->m_strPhonePWD) == 0)
                        	{
                            	CLOSE_DIALOG(DLGRET_EMGCALL)
                            	return TRUE;
                        	}
                        }
                        #endif
                        break;
                        
                    case AVK_SELECT:
                        if (STRLEN(pMe->m_strPhonePWD) < 4)
                        {
                            return TRUE;
                        }
                        else
                        {
                            CLOSE_DIALOG(DLGRET_ENTEROK)
                        }
                        return TRUE;
                        
                    default:
                        return TRUE;
                }
                
                nLen = STRLEN(pMe->m_strPhonePWD);
                if (chEnter == 0)
                {// 删除字符
                    if (nLen > 0)
                    {
                        bRedraw = TRUE;
                        pMe->m_strPhonePWD[nLen-1] = chEnter;
                    }
                }
                else if (nLen < PHONEPASSWORDLENTH)
                {
                    pMe->m_strPhonePWD[nLen] = chEnter;
                    nLen++;
                    pMe->m_strPhonePWD[nLen] = 0;
                    bRedraw = TRUE;
                }
                
                if (bRedraw)
                {
                    (void) ISHELL_PostEvent(pMe->a.m_pIShell,
                                            AEECLSID_CORE_APP,
                                            EVT_USER_REDRAW,
                                            NULL,
                                            NULL);
                }
            }
            return TRUE;
        default:
            break;
    }
    
    return FALSE;
} // IDD_PWDINPUT_Handler

/*==============================================================================
函数:
    IDD_UIMSECCODE_Handler
    
说明:
    IDD_UIMSECCODE 对话框事件处理函数
       
参数:
    pUser [in]: 这里必须是指向 Core Applet 对象结构的指针。
    eCode [in]: 事件代码。
    wParam: 事件相关数据。
    dwParam: 事件相关数据。
       
返回值:
    TRUE:  传入事件被处理。
    FALSE: 传入事件被忽略。
       
备注:
    函数作卡的 PIN 码、 PUK 码的输入界面处理。
       
==============================================================================*/
static boolean  IDD_UIMSECCODE_Handler(void       *pUser,
                                       AEEEvent   eCode,
                                       uint16     wParam,
                                       uint32     dwParam)
{
    static char *szCode = NULL;
    static AECHAR *wszCode = NULL;
    static int nMaxLen = 0;
    static uint16 nResID = 0;
    //IMenuCtl *pMenu = NULL;
    CCoreApp *pMe = (CCoreApp *)pUser;
     MSG_FATAL("IDD_UIMSECCODE_Handler--eCode=%d----wParam=%d---dwParam=%d",eCode,wParam,dwParam);
    if (NULL == pMe)
    {
        return FALSE;
    }
    //pMenu = (IMenuCtl*)IDIALOG_GetControl(pMe->m_pActiveDlg, IDC_UIMSECCODE_MENU);
    //if (NULL == pMenu)
    //{
    //    return FALSE;
    //}

    
#ifdef FEATURE_LCD_TOUCH_ENABLE
    if(eCode == EVT_PEN_UP)
        {
            int16 wXPos = (int16)AEE_GET_X((const char *)dwParam);
            int16 wYPos = (int16)AEE_GET_Y((const char *)dwParam);
            AEEDeviceInfo devinfo;
            int nBarH ;
            AEERect rc;
            nBarH = GetBottomBarHeight(pMe->m_pDisplay);
            MEMSET(&devinfo, 0, sizeof(devinfo));
            ISHELL_GetDeviceInfo(pMe->a.m_pIShell, &devinfo);
            SETAEERECT(&rc, 0, devinfo.cyScreen-nBarH, devinfo.cxScreen, nBarH);  
            if(CORE_PT_IN_RECT(wXPos,wYPos,rc))
            {
                if(wXPos >= rc.x && wXPos < rc.x + (rc.dx/3) )//左
                {
                     eCode = EVT_KEY;
				     wParam = AVK_SELECT;
                } 
                else if(wXPos >= rc.x + (rc.dx/3)*2 && wXPos < rc.x + (rc.dx/3)*3 )//右
                { 
                     eCode = EVT_KEY;
				     wParam = AVK_CLR;
                }
            }
            else
            {
                MEMSET(pMe->m_strPhonePWDtemp, 0, PINCODE_LENGTH + 1);
                ISHELL_StartAppletArgs(pMe->a.m_pIShell, AEECLSID_APP_SECURITYMENU, "InputPW");
            }
        
        }
                                        
#endif    
    switch (eCode)
    {
        case EVT_DIALOG_INIT:
             szCode = NULL;
            nMaxLen = 0;
            return TRUE;
            
        case EVT_DIALOG_START:
        {
            //uint16 nResID = 0;            
            // 初始化初始数据
            switch (pMe->m_eRUIMSCode)
            {
                case ENTERPIN_ONLY:
                    MEMSET(pMe->m_strPIN1, 0, sizeof(pMe->m_strPIN1));
                    szCode = pMe->m_strPIN1;
                    wszCode = pMe->m_wPIN;
                    nMaxLen = PINCODE_LENGTH;
                    nResID = IDS_ENTER_PIN;
                    break;
                    
                case ENTERPUK_STEP1:
                    MEMSET(pMe->m_strPIN1, 0, sizeof(pMe->m_strPIN1));
                    szCode = pMe->m_strPIN1;
                    wszCode = pMe->m_wPIN;
                    nMaxLen = PINCODE_LENGTH;
                    nResID = IDS_NEWPIN;
                    break;
                    
                case ENTERPUK_STEP2:
                    MEMSET(pMe->m_strPIN2, 0, sizeof(pMe->m_strPIN2));
                    szCode = pMe->m_strPIN2;
                    wszCode = pMe->m_wPIN;
                    nMaxLen = PINCODE_LENGTH;
                    nResID = IDS_NEWPIN_AGAIN;
                    break;
                    
                case ENTERPUK_STEP0:
                    MEMSET(pMe->m_strPUK, 0, sizeof(pMe->m_strPUK));
                    szCode = pMe->m_strPUK;
                    wszCode = pMe->m_wPUK;
                    nMaxLen = PUKCODE_LENGTH;
                    nResID = IDS_ENTER_PUK;
                    break;
                
                default:
                    szCode = NULL;
                    break;
            }           
            
            ISHELL_PostEvent(pMe->a.m_pIShell,AEECLSID_CORE_APP,EVT_USER_REDRAW, 0,0);            
            return TRUE;
        }
            
            
        case EVT_USER_REDRAW:
            if (NULL == szCode)
            {
                return TRUE;
            }
            // 绘制相关信息
            {
                AECHAR  wstrDisplay[12] = {0};
                AEECHVStatus  chvst;
                int xOffset = 5, nLen = 0;
                AECHAR  wstrCnt[32] = {0};
                char    strDisplay[12] = {0};
                AECHAR  text[32] = {0};
                RGBVAL nOldFontColor;
                TitleBar_Param_type  TitleBar_Param = {0};
                
                // 先清屏
#ifdef FEATURE_CARRIER_CHINA_VERTU
                {
                    IImage *pImageBg = ISHELL_LoadResImage(pMe->a.m_pIShell, AEE_APPSCOMMONRES_IMAGESFILE, IDI_SECURITY_BACKGROUND);
                    
                    Appscommon_ResetBackground(pMe->m_pDisplay, pImageBg, APPSCOMMON_BG_COLOR, &pMe->m_rc, 0, 0);
                    if(pImageBg != NULL)
                    {
                        IImage_Release(pImageBg);
                    }
                }
#else
                Appscommon_ResetBackgroundEx(pMe->m_pDisplay, &pMe->m_rc, TRUE);
#endif
                //IDISPLAY_FillRect  (pMe->m_pDisplay,&pMe->m_rc,RGB_BLACK);  
                // 画标题条
                if (nResID != 0)
                {
                    ISHELL_LoadResString(pMe->a.m_pIShell, 
                                                        AEE_COREAPPRES_LANGFILE,
                                                        nResID,
                                                        text, 
                                                        sizeof(text));
                    TitleBar_Param.pwszTitle = text;
                    TitleBar_Param.dwAlignFlags = IDF_TEXT_TRANSPARENT | IDF_ALIGN_CENTER | IDF_ALIGN_MIDDLE;
                    DrawTitleBar(pMe->m_pDisplay,&TitleBar_Param);
                }
                
                nOldFontColor = IDISPLAY_SetColor(pMe->m_pDisplay, CLR_USER_TEXT, RGB_WHITE);
                if ((pMe->m_eRUIMSCode == ENTERPIN_ONLY) ||
                    (pMe->m_eRUIMSCode == ENTERPUK_STEP0))
                {// 绘制次数提示字符串
                    // 读取UIM卡的状态
                    (void)IRUIM_GetCHVStatus(pMe->m_pIRUIM, &chvst);
                    
                    (void) ISHELL_LoadResString(pMe->a.m_pIShell,
                                                AEE_COREAPPRES_LANGFILE,
                                                IDS_AVAILCNT,
                                                wstrCnt,
                                                sizeof(wstrCnt));
                                                
                    if (pMe->m_eRUIMSCode == ENTERPIN_ONLY)
                    {
                        WSPRINTF(text, sizeof(text),
                                 wstrCnt,  chvst.chv1CountRemain);
                    }
                    else
                    {
                        WSPRINTF(text, sizeof(text),
                                 wstrCnt,  chvst.chv1UnblockRemain);
                    }
                }
                else
                {
                    (void) ISHELL_LoadResString(pMe->a.m_pIShell,
                                                AEE_COREAPPRES_LANGFILE,
                                                IDS_ENTER_PASSWORD,
                                                text,
                                                sizeof(text));
                }
                    
                IDISPLAY_DrawText(pMe->m_pDisplay, 
                                                    AEE_FONT_BOLD, 
                                                    text,
                                                    -1, 
                                                    xOffset, 
                                                    TITLEBAR_HEIGHT + MENUITEM_HEIGHT*1/2, 
                                                    NULL, 
                                                    IDF_TEXT_TRANSPARENT);
#ifdef FEATURE_LCD_TOUCH_ENABLE                
                MSG_FATAL("pMe->m_strPhonePWDtemp---%x---%x--%x",pMe->m_strPhonePWDtemp[0],pMe->m_strPhonePWDtemp[1],STRLEN(pMe->m_strPhonePWDtemp));
                if(STRLEN(pMe->m_strPhonePWDtemp)>0)
                {
                    AECHAR  wszCodetemp[OEMNV_LOCKPIN_MAXLEN+1] = {0};
                    (void) STRCPY(szCode,pMe->m_strPhonePWDtemp);
                    (void) STRTOWSTR(pMe->m_strPhonePWDtemp,wszCodetemp,sizeof(wszCodetemp));
                    (void) WSTRCPY(wszCode,wszCodetemp);                    
                }
                MSG_FATAL("szCode---%x---%x--%x",szCode[0],szCode[1],STRLEN(szCode));
#endif
                // 绘制输入
                nLen = STRLEN(szCode);
                MEMSET(strDisplay, '*', nLen);
                strDisplay[nLen] = '|';
                strDisplay[nLen + 1] = '\0';
                (void) STRTOWSTR(strDisplay, wstrDisplay, sizeof(wstrDisplay));
                IDISPLAY_DrawText(pMe->m_pDisplay, 
                                                AEE_FONT_BOLD, 
                                                wstrDisplay,
                                                -1, 
                                                2*xOffset,
                                                TITLEBAR_HEIGHT + MENUITEM_HEIGHT*3/2,
                                                NULL, 
                                                IDF_TEXT_TRANSPARENT);
                (void)IDISPLAY_SetColor(pMe->m_pDisplay, CLR_USER_TEXT, nOldFontColor);

                #ifndef FEATURE_ALL_KEY_PAD    //add by yangdecai 
                // 绘制底条提示
                if (nLen > 3)
                {// 确定-----删除
                    CoreDrawBottomBar(BTBAR_OK_DELETE)
                }
                else if(nLen > 0)
                {// 删除
                    CoreDrawBottomBar(BTBAR_DELETE)
                }
                else
                #else
                // 绘制底条提示
                if (nLen > 3)
                {// 确定-----删除
                   CoreDrawBottomBar(BTBAR_OK_BACK)
                }
                else if(nLen > 0)
                {// 删除
                    CoreDrawBottomBar(BTBAR_SOS)
                }
                else
                #endif
                {//SOS
                    CoreDrawBottomBar(BTBAR_SOS)
                }
            }
            
            // 更新显示
            //IDISPLAY_UpdateEx(pMe->m_pDisplay, FALSE);  
            return TRUE;
            
        case EVT_DIALOG_END:
            #ifdef FEATURE_LCD_TOUCH_ENABLE
            MEMSET(pMe->m_strPhonePWDtemp, 0, PINCODE_LENGTH + 1);
            #endif
            return TRUE;

        case EVT_KEY:
            {
                char  chEnter = 0;
                int   nLen = 0;
                boolean bRedraw = FALSE;
                
                switch (wParam)
                {
                    case AVK_0:
                    case AVK_1:
                    case AVK_2:
                    case AVK_3:
                    case AVK_4:
                    case AVK_5:
                    case AVK_6:
                    case AVK_7:
                    case AVK_8:
                    case AVK_9:
                        chEnter = '0' + (wParam - AVK_0);
                        break;

                    case AVK_STAR:
                        chEnter = '*';
                        break;

                    case AVK_POUND:
                        chEnter = '#';
                        break;
                    case AVK_CAMERA:
                    	#if defined(FEATURE_VERSION_C306)||defined(FEATURE_VERSION_W0216A)|| defined(FEAUTRE_VERSION_N450)|| defined(FEATURE_VERSION_N021)|| defined(FEATURE_VERSION_C01)
                    	{
						nv_item_type	SimChoice;
						OEMNV_Get(NV_SIM_SELECT_I,&SimChoice);
						if(SimChoice.sim_select ==AVK_SEND_TWO)
						{
							CLOSE_DIALOG(DLGRET_EMGCALL)
                        	return TRUE;
						}
						}
						#endif
						break;
                    case AVK_SEND:
	                    #if defined(FEATURE_VERSION_C01) 
						{
							nv_item_type	SimChoice;
							OEMNV_Get(NV_SIM_SELECT_I,&SimChoice);
							if(SimChoice.sim_select==AVK_SEND_TWO)
							{
								return TRUE;
							}
						}
						#endif
                        CLOSE_DIALOG(DLGRET_EMGCALL)
                        return TRUE;

                    case AVK_SOFT2:		//Add By zzg 2010_09_08 for smart and m8    
                    case AVK_CLR:
                        chEnter = 0;
                        #ifdef FEATURE_LCD_TOUCH_ENABLE
                        MEMSET(pMe->m_strPhonePWDtemp, 0, PINCODE_LENGTH + 1);
                        #endif
                        #ifndef FEATURE_ALL_KEY_PAD    //add by yangdecai 
                        if (szCode == NULL)
                        {
                            return TRUE;
                        }
                        if (STRLEN(szCode) == 0)
                        {
                            CLOSE_DIALOG(DLGRET_EMGCALL)
                            return TRUE;
                        }
                        #else
                        if(dwParam == 0)
                        {
                        	CLOSE_DIALOG(DLGRET_EMGCALL)
	                        return TRUE;
                        }
                        else
                        {
                        	if (szCode == NULL)
	                        {
	                            return TRUE;
	                        }
	                        if (STRLEN(szCode) == 0)
	                        {
	                            CLOSE_DIALOG(DLGRET_EMGCALL)
	                            return TRUE;
	                        }
                        }
                        #endif
                        break;
                        
                    case AVK_SELECT:
                        if (szCode == NULL)
                        {
                            return TRUE;
                        }
                        if (STRLEN(szCode) < 4)
                        {
                            return TRUE;
                        }
//                        else if ((nMaxLen == PINCODE_LENGTH) && (STRLEN(szCode) < nMaxLen))
//                        {
//                            return TRUE;
//                        }
                        else
                        {
                            CLOSE_DIALOG(DLGRET_OK)
                        }
                        return TRUE;
                        
                    default:
                        return TRUE;
                }
                
                if (szCode == NULL)
                {
                    return TRUE;
                }
                
                nLen = STRLEN(szCode);
                if (chEnter == 0)
                {// 删除字符
                    if (nLen > 0)
                    {
                        bRedraw = TRUE;
                        szCode[nLen-1] = chEnter;
                        wszCode[nLen-1] = chEnter;
                    }
                }
                else if (nLen < nMaxLen)
                {
                    szCode[nLen] = chEnter;
                    wszCode[nLen] = chEnter;
                    nLen++;
                    szCode[nLen] = 0;
                    wszCode[nLen] = 0;
                    bRedraw = TRUE;
                }
                
                if (bRedraw)
                {
                    (void) ISHELL_PostEvent(pMe->a.m_pIShell,
                                            AEECLSID_CORE_APP,
                                            EVT_USER_REDRAW,
                                            NULL,
                                            NULL);
                }
            }
            return TRUE;            
        default:
            break;
    }
    
    return FALSE;
} // IDD_UIMSECCODE_Handler

/*==============================================================================
函数:
    IDD_UIMERR_Handler
       
说明:
    IDD_UIMERR 对话框事件处理函数
       
参数:
    pUser [in]: 这里必须是指向 Core Applet 对象结构的指针。
    eCode [in]: 事件代码。
    wParam: 事件相关数据。
    dwParam: 事件相关数据。
       
返回值:
    TRUE:  传入事件被处理。
    FALSE: 传入事件被忽略。
       
备注:
    函数处理 UIM 错误界面提示及相关事件。
       
==============================================================================*/
static boolean  IDD_UIMERR_Handler(void       *pUser,
                                   AEEEvent   eCode,
                                   uint16     wParam,
                                   uint32     dwParam)
{
    IStatic * pStatic = NULL;
    CCoreApp *pMe = (CCoreApp *)pUser;
    
    if (NULL == pMe)
    {
        return FALSE;
    }
    pStatic = (IStatic*)IDIALOG_GetControl(pMe->m_pActiveDlg, 
                            IDC_UIMERR_STATIC);

    if (NULL == pStatic)
    {
        return FALSE;
    }
    
    switch (eCode)
    {
        case EVT_DIALOG_INIT:
	        MEMSET(pMe->m_strLockuimPWD, 0, sizeof(pMe->m_strLockuimPWD));
            return TRUE;
            
        case EVT_DIALOG_START:
            //设置控件区域
            {
                AEERect rc;
                SETAEERECT(&rc, 0, 0,pMe->m_rc.dx, pMe->m_rc.dy - BOTTOMBAR_HEIGHT);
                
                // 先清屏
                IDISPLAY_EraseRect(pMe->m_pDisplay,&rc);
                rc.x = 1;
                rc.dx -= 2;
                ISTATIC_SetRect(pStatic, &rc);
                
                // 设置静态文本控件属性
                ISTATIC_SetProperties(pStatic, ST_UNDERLINE | 
                        ST_CENTERTEXT  | ST_NOSCROLL|ST_MIDDLETEXT);
            }

            (void) ISHELL_PostEvent(pMe->a.m_pIShell,
                      AEECLSID_CORE_APP,
                      EVT_USER_REDRAW,
                      0,
                      0);
            return TRUE;
            
        case EVT_USER_REDRAW:
            {
                uint16    nResID = 0;
                AECHAR    *wTipsMsg = NULL;

#ifdef FEATURE_VERSION_W208S                     	
                // 设置静态控件文本                
                (void)ISTATIC_SetText(pStatic,
				                        NULL,
				                        NULL,
				                        AEE_FONT_NORMAL,
				                        AEE_FONT_NORMAL);   				
#else
                switch (pMe->m_eUIMErrCode)
                {
                    // 没插入卡
                    case UIMERR_NOUIM:
                        nResID = IDS_NORUIM;
                        break;
                    // UIM 卡无效(原因输PUK码超过规定次数导致卡失效)
                    case UIMERR_BLOCKED:
                        nResID = IDS_UIMBLOCKED;
                        break;
                        
                    // UIM 卡无效(原因是卡不符合运营商要求而要求锁定)
                    case UIMERR_LOCKED:
                        nResID = IDS_LOCKEDRUIM;
                        break;
                        
                    // UIM 卡正常无误, 程序不应执行到此
                    default:
                        break;
                }
                
                if (nResID != 0)
                {
                    int32  nSize = 128 * sizeof(AECHAR);
                    
                    wTipsMsg = (AECHAR *) MALLOC(nSize);
                    
                    if (NULL != wTipsMsg)
                    {
                        // 取提示文本
                        (void) ISHELL_LoadResString(pMe->a.m_pIShell,
                                                    AEE_COREAPPRES_LANGFILE,
                                                    nResID,
                                                    wTipsMsg,
                                                    nSize);
                                                    
                        // 设置静态控件文本
                        (void)ISTATIC_SetText(pStatic,
                                NULL,
                                wTipsMsg,
                                AEE_FONT_NORMAL,
                                AEE_FONT_NORMAL);
                                
                        // 释放不再使用的临时 buffer
                        FREE(wTipsMsg);
                        wTipsMsg = NULL;
                    }
                }
#endif				
                
                ISTATIC_SetActive(pStatic, TRUE);
                (void) ISTATIC_Redraw(pStatic);
                
                // 绘制底部提示菜单
                // -----SOS
                CoreDrawBottomBar(BTBAR_SOS)

                // 立即更新屏幕
                IDISPLAY_UpdateEx(pMe->m_pDisplay, FALSE);
            }          
            
            return TRUE;
            
        case EVT_DIALOG_END:
            return TRUE;
        case EVT_KEY:
		{
		char  chEnter = 0; 
		static int   nLen = 0; 
            switch (wParam)
            {
                    case AVK_0:
                    case AVK_1:
                    case AVK_2:
                    case AVK_3:
                    case AVK_4:
                    case AVK_5:
                    case AVK_6:
                    case AVK_7:
                    case AVK_8:
                    case AVK_9:
                      chEnter = '0' + (wParam - AVK_0);
                        break;

                    case AVK_STAR:
                        chEnter = '*';
                        break;
 
                    case AVK_POUND:
                        chEnter = '#';
                        break;
                    case AVK_CAMERA:
                    	#if defined(FEATURE_VERSION_C306)||defined(FEATURE_VERSION_W0216A) || defined(FEAUTRE_VERSION_N450)|| defined(FEATURE_VERSION_N021)|| defined(FEATURE_VERSION_C01)
                    	{
						nv_item_type	SimChoice;
						OEMNV_Get(NV_SIM_SELECT_I,&SimChoice);
						if(SimChoice.sim_select ==AVK_SEND_TWO)
						{
							CLOSE_DIALOG(DLGRET_EMGCALL)
                        	return TRUE;
						}
						}
						#endif
						break;
                    case AVK_SEND:
	                    #if defined(FEATURE_VERSION_C01) 
						{
							nv_item_type	SimChoice;
							OEMNV_Get(NV_SIM_SELECT_I,&SimChoice);
							if(SimChoice.sim_select==AVK_SEND_TWO)
							{
								return TRUE;
							}
						}
						#endif
                        CLOSE_DIALOG(DLGRET_EMGCALL)
                        return TRUE;

                    case AVK_SOFT2:		//Add By zzg 2010_09_08 for smart and m8
                    case AVK_CLR:                    	
                        chEnter = 0;
                        CLOSE_DIALOG(DLGRET_EMGCALL)
                        return TRUE;
                        
                    case AVK_SELECT:                     
                        CLOSE_DIALOG(DLGRET_ENTER)  
                        return TRUE;
                        
                    default:
                        return TRUE;
                }

                nLen = STRLEN(pMe->m_strLockuimPWD);
                if (chEnter == 0)
                {// 删除字符
                    if (nLen > 0)
                    {
                        pMe->m_strLockuimPWD[nLen-1] = chEnter; 
                    }
                }
                else if (nLen < LOCKUIMPASSWORDLENTH)
                {
                	char wMEID[6]="*#06#";
                    pMe->m_strLockuimPWD[nLen] = chEnter;
                    nLen++;
                    pMe->m_strLockuimPWD[nLen] = 0;
					if (STRCMP(wMEID,pMe->m_strLockuimPWD) == 0)
            		{// 密码符合
            			ISHELL_StartAppletArgs(AEE_GetShell(), AEECLSID_FIELDDEBUGAPP, "*#06#");
					}
					
                }
            break;
        }
        default:
            break;
    }
    
    return FALSE;
} // IDD_UIMERR_Handler
/*==============================================================================
函数:
    IDD_STARTUPANI_Handler
    
说明:
    IDD_STARTUPANI 对话框事件处理函数
       
参数:
    pUser [in]: 这里必须是指向 Core Applet 对象结构的指针。
    eCode [in]: 事件代码。
    wParam: 事件相关数据。
    dwParam: 事件相关数据。
       
返回值:
    TRUE:  传入事件被处理。
    FALSE: 传入事件被忽略。
       
备注：WHEN                WHO      WHAT          WHY
      2008-2-16           fj       V1.6          New develop, Add power down animation  
==============================================================================*/
static boolean  IDD_STARTUPANI_Handler(void       *pUser,
                                       AEEEvent   eCode,
                                       uint16     wParam,
                                       uint32     dwParam)
{
    CCoreApp *pMe = (CCoreApp *)pUser;
    MSG_FATAL("IDD_STARTUPANI_Handler Start",0,0,0);
    switch (eCode) 
    {
        case EVT_DIALOG_INIT:
            IDIALOG_SetProperties((IDialog *)dwParam, DLG_NOT_REDRAW_AFTER_START);
            return TRUE;

        case EVT_DIALOG_START: 		
			//Add By zzg 2012_02_17



			//Add End
			
            if(pMe->m_wStartupAniTime == 0)
            {
#ifndef FEATURE_USES_LOWMEM
                if ( NULL == pMe->m_pStartupAniImg )
                {
                    pMe->m_pStartupAniImg = ISHELL_LoadImage( pMe->a.m_pIShell, PWRON_ANI_FILE);
                    if(pMe->m_pStartupAniImg == NULL)
                    {
                        MSG_FATAL("pMe->m_pStartupAniImg == NULL",0,0,0);
                    }
                }
#endif
                (void) ISHELL_PostEvent(pMe->a.m_pIShell, AEECLSID_CORE_APP, EVT_USER_REDRAW, 0, 0);
            }
            return TRUE;

        case EVT_USER_REDRAW:
            if(pMe->m_wStartupAniTime == 0)
            {
                // 在此播放开机音乐
                uint16    aRing_type[PROFILENUMBER]  = {0} ;                                          // CFGI_PROFILE_STARTUP_MUSIC
                byte      Ring_Cur_Music                      =  OEMNV_PROFILE_NORMALMODE;
                byte      poweronoff_alert[PROFILENUMBER] =  {0};

                ICONFIG_GetItem( pMe->m_pConfig, CFGI_PROFILE_CUR_NUMBER, &Ring_Cur_Music, sizeof(Ring_Cur_Music) ); 
                ICONFIG_GetItem(pMe->m_pConfig,CFGI_PROFILE_POWERONOFF_ALERT,poweronoff_alert,sizeof(poweronoff_alert));  
                if(poweronoff_alert[Ring_Cur_Music]  == OEMNV_POWERONOFF_ENABLE)
                {
                    db_items_value_type db;
                    db_get(DB_POWERUPTYPE,&db);//add by xuhui
                    //如果是自动重启，则不播放开机铃声
                    if(db.db_poweruptype != DB_POWERUP_BYRESET)
                    {
                        ICONFIG_GetItem( pMe->m_pConfig, CFGI_PROFILE_STARTUP_MUSIC, aRing_type, sizeof(aRing_type) );
                        IALERT_StartRingerAlert_Ex( pMe->m_pAlert, (uint32)aRing_type[Ring_Cur_Music] );
                    }
                }

                // 播放开机动画
                //pMe->m_wStartupAniTime = 0;
#ifndef FEATURE_USES_LOWMEM
                if ( NULL != pMe->m_pStartupAniImg )
#endif
                {     
                    CoreApp_PlayPwrOnAni(pMe);
                }
#ifndef FEATURE_USES_LOWMEM
                else
                {
                    CLOSE_DIALOG( DLGRET_OK ); 
                }
#endif
            }
            return TRUE; 

        case EVT_DIALOG_END:
            MSG_FATAL("IDD_STARTUPANI_Handler EVT_DIALOG_END",0,0,0);
            //IALERT_StopRingerAlert(pMe->m_pAlert);
            if (pMe->m_eDlgRet != DLGRET_OK)
            {// 开机动画播放过程中被其他应用启动时中断
#ifndef FEATURE_USES_LOWMEM
                if (NULL != pMe->m_pStartupAniImg)
                {     
                    MSG_FATAL("IDD_STARTUPANI_Handler EVT_DIALOG_END 1",0,0,0);
                    (void)ISHELL_CancelTimer(pMe->a.m_pIShell, (PFNNOTIFY)CoreApp_PlayPwrOnAni, pMe);
                    IIMAGE_Stop(pMe->m_pStartupAniImg);
                    IIMAGE_Release(pMe->m_pStartupAniImg);
                    pMe->m_pStartupAniImg = NULL;
                    
                    pMe->m_eDlgRet = DLGRET_OK;
                }
#else
                (void)ISHELL_CancelTimer(pMe->a.m_pIShell, (PFNNOTIFY)CoreApp_PlayPwrOnAni, pMe);
                pMe->m_eDlgRet = DLGRET_OK;
#endif
                MSG_FATAL("IDD_STARTUPANI_Handler EVT_DIALOG_END 2",0,0,0);
                IALERT_StopRingerAlert(pMe->m_pAlert);
            }
            return TRUE;

        case EVT_DISPLAYDIALOGTIMEOUT:
            return TRUE;

        case EVT_KEY:
            return TRUE;

        default:
            break;
    }

    return FALSE;
}

/*==============================================================================
函数:
    IDD_LOADING_Handler
    
说明:
    IDD_LOADING 对话框事件处理函数
       
参数:
    pUser [in]: 这里必须是指向 Core Applet 对象结构的指针。
    eCode [in]: 事件代码。
    wParam: 事件相关数据。
    dwParam: 事件相关数据。
       
返回值:
    TRUE:  传入事件被处理。
    FALSE: 传入事件被忽略。
       
备注:
    函数显示提示消息给用户，并自动关闭返回。
       
==============================================================================*/
static boolean  IDD_LOADING_Handler(void       *pUser,
                                   AEEEvent   eCode,
                                   uint16     wParam,
                                   uint32     dwParam)
{
    CCoreApp *pMe = (CCoreApp *)pUser;
    IStatic * pStatic = NULL;
    
    if (NULL == pMe)
    {
        return FALSE;
    }
    
    pStatic = (IStatic*)IDIALOG_GetControl(pMe->m_pActiveDlg, IDC_LOADING);

    if (NULL == pStatic)
    {
        return FALSE;
    }
    
    switch(eCode)
    {
        case EVT_DIALOG_INIT:
            ISTATIC_SetProperties(pStatic, (ST_CENTERTEXT | ST_MIDDLETEXT));
            return TRUE;

        case EVT_DIALOG_START:
            (void) ISHELL_PostEvent(pMe->a.m_pIShell, 
                                    AEECLSID_CORE_APP,
                                    EVT_USER_REDRAW, 
                                    0, 0);

            return TRUE;

        case EVT_USER_REDRAW:
            {
                AECHAR  *wstrText=NULL;
                int32  nSize=0;

                nSize = 32 * sizeof(AECHAR);
                wstrText = (AECHAR *) MALLOC(nSize);
                
                if (NULL != wstrText)
                {
                    // 从资源文件取消息内容
                    (void)ISHELL_LoadResString(pMe->a.m_pIShell,
                                    AEE_COREAPPRES_LANGFILE,                                
                                    IDS_LOADING,
                                    wstrText,
                                    nSize);
                                    
                    (void) ISTATIC_SetText(pStatic,
                                    NULL,
                                    wstrText,
                                    AEE_FONT_NORMAL,
                                    AEE_FONT_NORMAL);
                                    
                    FREEIF(wstrText);
                }
            }
            
            (void) ISTATIC_Redraw(pStatic);

            // 设置自动关闭对话框的定时器
            (void)ISHELL_SetTimer(pMe->a.m_pIShell, 
                                  1000,
                                  DialogTimeoutCallback, 
                                  pMe);
#ifdef FEATURE_RUIM_PHONEBOOK
#ifdef FEATURE_VERSION_W208S
#ifdef FEATURE_UIM
			if(TRUE)
#else
			if(FALSE)
#endif				
#else
			if(IsRunAsUIMVersion())
#endif            
            {
                if(IRUIM_IsCardConnected(pMe->m_pIRUIM))
                {
                    if (NULL == pMe->m_pAddrRUIM)
                    {
                        (void) ISHELL_CreateInstance( pMe->a.m_pIShell,
                                                  AEECLSID_ADDRBOOK_RUIM,
                                                  (void **)&pMe->m_pAddrRUIM);
                    }
                }
            }
#endif
            if (NULL == pMe->m_pAddrPhone)
            {            
                (void) ISHELL_CreateInstance( pMe->a.m_pIShell,
                                      AEECLSID_ADDRBOOK,
                                      (void **)&pMe->m_pAddrPhone);
            }	    
            //IDISPLAY_UpdateEx(pMe->m_pDisplay, FALSE);
            
            return TRUE;

        case EVT_DIALOG_END:
            return TRUE;

        case EVT_KEY:
            return TRUE;
        
        case EVT_DISPLAYDIALOGTIMEOUT:            
            CLOSE_DIALOG(DLGRET_INITED);
            return TRUE;

        default:
            break;
    }

    return FALSE;
}

static void CoreApp_ImageNotify(void *po, IImage *pIImage, AEEImageInfo *pii, int nErr)
{   
    CCoreApp *pMe = (CCoreApp *)po;
    
#ifdef FEATRUE_SET_ANN_FULL_SCREEN
    db_items_value_type  need_capture;
#endif
    MSG_FATAL("CoreApp_ImageNotify start",0,0,0);
    //MSG_ERROR("CoreApp_ImageNotify, nErr:%d",nErr, 0, 0);
    if(SUCCESS == nErr) 
    {
#ifdef FEATRUE_SET_ANN_FULL_SCREEN
        if (pMe->m_capture == DB_CAPTURE_NEED)
        {
            extern IDIB ** OEMANNUNCIATOR_GetWallpaperBarDDB(void);
            IDIB **ppDIB=NULL;
            AEERect rc;
            IBitmap *pDevBmp=NULL;
            IBitmap *pBmp =NULL;
            pMe->m_capture = DB_CAPTURE_NONE;
            ppDIB = OEMANNUNCIATOR_GetWallpaperBarDDB();
            if ((NULL != ppDIB) &&
                (SUCCESS == IANNUNCIATOR_GetAnnunciatorBarSize(pMe->m_pIAnn,AEECLSID_DISPLAY1, &rc)))
            {
                if (IDISPLAY_GetDeviceBitmap(pMe->m_pDisplay, &pDevBmp) == SUCCESS) 
                {
                    IBITMAP_CreateCompatibleBitmap(pDevBmp, &pBmp, rc.dx, rc.dy);
                    IBITMAP_Release(pDevBmp);
                    
                    IDISPLAY_SetDestination(pMe->m_pDisplay, pBmp);
                    if ( NULL != pWallPaper )
                    {
                        MSG_FATAL("rc.dx=%d,rc.dy=%d",rc.dx,rc.dy,0);
                        IIMAGE_SetDrawSize(pWallPaper, rc.dx, rc.dy);
                        IIMAGE_Draw(pWallPaper, 0, 0);
                    }
                    else
                    {
                        MSG_FATAL("NULL == pWallPaper",0,0,0);
                        IDISPLAY_EraseRect(pMe->m_pDisplay, &rc);
                    }
                   // IDISPLAY_UpdateEx(pMe->m_pDisplay, TRUE); // debug for update issue
                    
                    IDISPLAY_SetDestination(pMe->m_pDisplay, NULL);
                    (void)IBITMAP_QueryInterface(pBmp, AEECLSID_DIB, (void **) ppDIB);
                    IBITMAP_Release (pBmp);
                }
            }
        }
#endif                
        if ( NULL != pWallPaper )
        {
            AEEImageInfo info;
            IImage_GetInfo(pWallPaper, &info);
            IDisplay_ClearScreen(pMe->m_pDisplay);
            MSG_FATAL("x=%d, y=%d", (pMe->m_rc.dx - info.cx)/2, (pMe->m_rc.dy - info.cy)/2,0);
            IIMAGE_Draw(pWallPaper, (pMe->m_rc.dx - info.cx)/2, (pMe->m_rc.dy - info.cy)/2);
        }
        else
        {
            MSG_FATAL("NULL == pWallPaper",0,0,0);
            IDISPLAY_EraseRect(pMe->m_pDisplay, &pMe->m_rc);
        }

#ifdef FEATRUE_SET_ANN_FULL_SCREEN
        need_capture.b_capture = DB_CAPTURE_INIDLE;
        db_put(DB_CAPTURE_WALLPER,&need_capture);
        if(pMe->m_pIAnn != NULL)
        {
            IANNUNCIATOR_EnableAnnunciatorBar(pMe->m_pIAnn,AEECLSID_DISPLAY1,TRUE);
        }
#endif

        if (bHideText)
        {
            CoreApp_UpdateAnnunciator(pMe);//解决背景播放换歌或按AVK_END键状态栏图表闪烁问题
                                         // 故把该句提到IDISPLAY_UpdateEx之前
#ifdef FEATURE_APP_MUSICPLAYER                                         
            ISHELL_CancelTimer(pMe->a.m_pIShell,(PFNNOTIFY)CoreApp_MusicNameAutoScroll,pMe);
#endif
           // IDISPLAY_UpdateEx(pMe->m_pDisplay, TRUE); // debug for update issue
            bImageDecoded = TRUE;
            return;
        }  
#ifdef FEATURE_APP_MUSICPLAYER
        //CoreApp_DrawMusicName(pMe);
        Core_DrawNameResetScroll(pMe);
#endif
		
        // 绘制当前日期、时间信息
        CoreApp_UpdateDateTime(pMe);
        
        // 绘制服务提供商名和待机问候语
        CoreApp_DrawBannerMessage((void*)pMe);
    
        #ifndef FEATURE_USES_BLACKBERRY
			#ifdef FEATURE_LCD_TOUCH_ENABLE
			CoreApp_DrawTouch_IDLE(pMe);
			#else
	        CoreApp_UpdateBottomBar(pMe);//didn't display the sos and key lock icon at the same time
			#endif
		#else
        MSG_FATAL("CoreApp_DrawBlackBerry_IDLE...........", 0, 0,0);
        if(OEMKeyguard_IsEnabled())
        {
        	CoreApp_UpdateBottomBar(pMe);
        }
        else
        {
        	CoreApp_DrawBlackBerry_IDLE(pMe);
        }
        #endif
        CoreApp_UpdateAnnunciator(pMe);  //解决背景播放换歌或按AVK_END键状态栏图标闪烁问题
                                         // 故把该句提到IDISPLAY_UpdateEx之前
    }   
    bImageDecoded = TRUE;
    IDISPLAY_UpdateEx(pMe->m_pDisplay,TRUE);
}


/*==============================================================================
函数:
    IDD_IDLE_Handler
    
说明:
    IDD_IDLE 对话框事件处理函数
       
参数:
    pUser [in]: 这里必须是指向 Core Applet 对象结构的指针。
    eCode [in]: 事件代码。
    wParam: 事件相关数据。
    dwParam: 事件相关数据。
       
返回值:
    TRUE:  传入事件被处理。
    FALSE: 传入事件被忽略。
       

备注     :  WHEN                    WHO                WHAT                     WHY
           2008-02-20              fj                 V1.6                      New Dev, for Wallpaper       
==============================================================================*/
#ifdef FEATURE_SPORTS_APP
extern boolean is_g_sportsapp_pedometer_bground_flag();
#endif

static boolean  IDD_IDLE_Handler(void       *pUser,
                                 AEEEvent   eCode,
                                 uint16     wParam,
                                 uint32     dwParam)
{
	//int i;
    CCoreApp *pMe = (CCoreApp *)pUser;
	static  boolean bFirstStart = TRUE;	//Add By zzg 2011_10_20
#ifdef FEATURE_KEYGUARD	 
    byte  bData;
#endif
#ifdef FEATURE_VERSION_C11    
    static IStatic * pStatic = NULL;
#endif
    if (NULL == pMe)
    {
        return FALSE;
    }
    if(pMe->m_pIAnn != NULL)
    {
	    IANNUNCIATOR_SetFieldIsActiveEx(pMe->m_pIAnn,TRUE);
    }
    MSG_FATAL("IDD_IDLE_Handler:eCode=%x,wParam=%x,dwParam=%x",eCode, wParam, dwParam);
    switch (eCode)
    {
        case EVT_DIALOG_INIT:
			
            if(pMe->m_pIAnn != NULL)
            {
                #if defined(FEATURE_VERSION_C180) || defined(FEATURE_VERSION_C11) || defined(FEATURE_VERSION_1110W516)|| defined(FEATURE_VERSION_W027)
                IANNUNCIATOR_SetHasTitleText(pMe->m_pIAnn, FALSE);
                #else
                IANNUNCIATOR_SetHasTitleText(pMe->m_pIAnn, TRUE);//返回待机界面时，要把显示titlebar标志还原成TRUE
                #endif
			}

          #ifdef FEATURE_VERSION_C11
           if (NULL == pStatic)
            {
                AEERect rect = {0};
                if (AEE_SUCCESS != ISHELL_CreateInstance(pMe->a.m_pIShell,AEECLSID_STATIC,(void **)&pStatic))
                {
                    return FALSE;
                }
                ISTATIC_SetRect(pStatic, &rect);
            }
           #endif
           
            MEMSET(pMe->m_wstrEnterNum, 0, sizeof(pMe->m_wstrEnterNum));
			MSG_FATAL("***zzg CoreApp EVT_DIALOG_INIT bFirstStart=%x***", bFirstStart, 0, 0);
			
			if (bFirstStart == TRUE)
			{				
#ifdef FEATURE_APP_BLUETOOTH
				boolean bt_status = FALSE;
				ICONFIG_GetItem(pMe->m_pConfig, CFGI_BT_STATUS, &bt_status, sizeof(bt_status));
                MSG_FATAL("bt_status---=%d",bt_status,0,0);
				if (bt_status == TRUE)
				{					
					MSG_FATAL("***zzg CoreApp EVT_DIALOG_INIT ISHELL_StartBackgroundApplet***", 0, 0, 0);
					ISHELL_StartBackgroundApplet(pMe->a.m_pIShell, AEECLSID_BLUETOOTH_APP, NULL);
				}
#endif				
				bFirstStart = FALSE;
			}
			
            //CoreApp_GetRecordCount(pMe);
            CoreApp_DrawWallPaper(pMe);
#ifdef FEATURE_KEYGUARD
            if(pMe->m_b_set_lock)
            {
                OEMKeyguard_SetState(TRUE);
                pMe->m_b_set_lock = FALSE;
            }
#endif
            return TRUE;
            
        case EVT_DIALOG_START:
        {
#ifdef FEATRUE_SUPPORT_G_SENSOR
            dword shake;
#endif

#ifndef CUST_EDITION
            if(!((MMI_GSENSOR_SHAKE_OPEN == mmi_g_sensor_state) 
                ||(MMI_GSENSOR_SHAKE_OPEN_IN_IDLE == mmi_g_sensor_state)))   //ignore if sensor has been open.
            {
                ICONFIG_GetItem(pMe->m_pConfig,CFGI_GSENSOR,&shake,sizeof(shake));
                if(shake & OEMNV_SHAKE_WALLPAPER_MASK)
                {
#ifdef FEATRUE_SUPPORT_G_SENSOR
#ifndef WIN32//wlh 临时修改
                    if(is_g_sportsapp_pedometer_bground_flag() == FALSE)
                    {
                        mmi_g_sensor_process(G_SENSOR_ENABLE | G_SENSOR_SHAKE);
                        mmi_g_sensor_state = MMI_GSENSOR_SHAKE_OPEN;
                    }
#endif//WIN32
#endif
                } 
            }
#endif      
            CoreApp_UpdateIdleTimer(pMe);			
			
#ifdef FEATURE_KEYGUARD
            if(!OEMKeyguard_IsEnabled())
            {
                (void)ISHELL_CancelTimer(pMe->a.m_pIShell,
                                CoreApp_TimeKeyguard,
                                pMe);
            	(void) ICONFIG_GetItem(pMe->m_pConfig,
                                    CFGI_KEY_LOCK_CHECK/*CFGI_PHONE_KEY_LOCK_CHECK*/,
                                    &bData,
                                    sizeof(bData));
            	if(bData)
            	{
                	if ((ISHELL_ActiveApplet(pMe->a.m_pIShell) == AEECLSID_CORE_APP)&&
                                    (pMe->m_wActiveDlgID == IDD_IDLE))
                	{
                		//Add By zzg 2012_10_29
                		#ifdef FEATURE_VERSION_W317A
							switch(bData)
	                		{
	                			case 1:
	                			{
	                    			(void)ISHELL_SetTimer(pMe->a.m_pIShell,
	                                    10*1000,
	                                    CoreApp_TimeKeyguard,
	                                    pMe);
	                        	}
	                        	break;
								
	                			case 2:
	                			{
	                    			(void)ISHELL_SetTimer(pMe->a.m_pIShell,
	                                    30*1000,
	                                    CoreApp_TimeKeyguard,
	                                    pMe);
	                        	}
	                        	break;
								
	                        	case 3:
	                			{
	                    			(void)ISHELL_SetTimer(pMe->a.m_pIShell,
	                                    120*1000,
	                                    CoreApp_TimeKeyguard,
	                                    pMe);
	                        	}
	                        	break;
								
	                        	case 4:
	                			{
	                    			(void)ISHELL_SetTimer(pMe->a.m_pIShell,
	                                    300*1000,
	                                    CoreApp_TimeKeyguard,
	                                    pMe);
	                        	}
								break;
								
	                            default:
	                        	break;
	                    	}
						#else
							switch(bData)
	                		{
	                			case 1:
	                			{
	                    			(void)ISHELL_SetTimer(pMe->a.m_pIShell,
	                                    30*1000,
	                                    CoreApp_TimeKeyguard,
	                                    pMe);
	                        	}
	                        	break;
	                        	case 2:
	                			{
	                    			(void)ISHELL_SetTimer(pMe->a.m_pIShell,
	                                    120*1000,
	                                    CoreApp_TimeKeyguard,
	                                    pMe);
	                        	}
	                        	break;
	                        	case 3:
	                			{
	                    			(void)ISHELL_SetTimer(pMe->a.m_pIShell,
	                                    300*1000,
	                                    CoreApp_TimeKeyguard,
	                                    pMe);
	                        	}
								break;
	                            case 4:
	                            {
	                    			(void)ISHELL_SetTimer(pMe->a.m_pIShell,
	                                    60*1000,
	                                    CoreApp_TimeKeyguard,
	                                    pMe);
	                            }
	                        	break;
	                    	}
						#endif
						//Add End	
                 	}
            	}
            }
#endif		
#ifdef CUST_EDITION    
            {
				extern void CoreApp_InitBattStatus(CCoreApp * pMe);

            	ISHELL_SetTimer(pMe->a.m_pIShell, 3*1000,(PFNNOTIFY)CoreApp_InitBattStatus,  pMe);                                                                                                         
            }
#endif			
        return TRUE;
        }
		case EVT_USER_REDRAW:     
        case EVT_UPDATEIDLE:
        {        
			//Add By zzg 2012_10_29
			#ifdef FEATURE_VERSION_W317A				
			IFileMgr *pFileMgr = NULL;
		    ISHELL_CreateInstance(pMe->a.m_pIShell, AEECLSID_FILEMGR, (void **)&pFileMgr);
			
		    if (pFileMgr)
		    {
				if (IFILEMGR_Test(pFileMgr, AEEFS_CARD0_DIR)==SUCCESS)		
				{
					IANNUNCIATOR_SetField (pMe->m_pIAnn, ANNUN_FIELD_TCARD, ANNUN_STATE_TCARD_ON);
				}
				else
				{
					IANNUNCIATOR_SetField (pMe->m_pIAnn, ANNUN_FIELD_TCARD, ANNUN_STATE_TCARD_OFF);
				}
		    }			
			#endif
			//Add End
				
            CoreApp_DrawWallPaper(pMe); // debug for wallpaper update issue
            if(pWallPaper)
            {
                bImageDecoded = FALSE;
                IImage_Notify(pWallPaper, (PFNIMAGEINFO)CoreApp_ImageNotify, pMe);
            }
            return TRUE;            
        }
#if defined(FEATURE_VERSION_HITZ181)||defined(FEATURE_VERSION_MTM) ||defined(FEATURE_VERSION_S1000T)||defined(FEATURE_LCD_TOUCH_ENABLE)||defined(FEATURE_VERSION_C11)
		case EVT_KEY_HELD:
			 MSG_FATAL("***zzg EVT_KEY_HELD wParam=%x, dwParam=%x", wParam, dwParam, 0);
			
#ifdef FEATURE_LCD_TOUCH_ENABLE
			if(wParam == AVK_CLR)
			{
				MSG_FATAL("evt_key_held============%d",bsupersingal,0,0);
				if(bsupersingal)
				{
					bsupersingal  = FALSE;
					//CoreApp_ShowMsgDialog(pMe,IDS_SUPER_SINGAL_OFF);
					pMe->m_nMsgID = IDS_SUPER_SINGAL_OFF;
          			CLOSE_DIALOG(DLGRET_BATT_INFO)
				}
				else
				{
					bsupersingal  = TRUE;
					//CoreApp_ShowMsgDialog(pMe,IDS_SUPER_SINGAL_ON);
					pMe->m_nMsgID = IDS_SUPER_SINGAL_ON;
          			CLOSE_DIALOG(DLGRET_BATT_INFO)
				}
			}
#else
#ifdef FEATURE_VERSION_C11
            if(wParam == AVK_INFO)
            {
                boolean TorchOn = FALSE;
                pMe->m_keyinfoheld=TRUE;
                OEM_GetConfig(CFGI_FLSHLITHG_STATUS,&TorchOn, sizeof(TorchOn));
                if (TorchOn == FALSE )
                {
                    TorchOn = TRUE;
                    if (pMe->m_pBacklight)
                    {
                        IBACKLIGHT_TurnOnTorch(pMe->m_pBacklight);
                    }
                }
                else
                {
                    TorchOn = FALSE;
                    if (pMe->m_pBacklight)
                    {                           
                        IBACKLIGHT_TurnOffTorch(pMe->m_pBacklight);
                    }
                }
                OEM_SetConfig(CFGI_FLSHLITHG_STATUS,&TorchOn, sizeof(TorchOn));
               {
                 PromptMsg_Param_type m_PromptMsg;
                 MEMSET(&m_PromptMsg,0,sizeof(PromptMsg_Param_type));
                 if(TorchOn)
                 m_PromptMsg.nMsgResID= IDS_MAIN_MENU_TORCHON;
                 else
                 m_PromptMsg.nMsgResID= IDS_MAIN_MENU_TORCHOFF;   
                 m_PromptMsg.ePMsgType = MESSAGE_WARNNING;
                 STRLCPY(m_PromptMsg.strMsgResFile, AEE_COREAPPRES_LANGFILE,MAX_FILE_NAME);
                 m_PromptMsg.eBBarType = BTBAR_NONE;
                 DrawPromptMessage(pMe->m_pDisplay,pStatic,&m_PromptMsg);
                 IDISPLAY_UpdateEx(pMe->m_pDisplay,TRUE);
                 (void)ISHELL_SetTimer(pMe->a.m_pIShell,1000, (PFNNOTIFY)CCoreApp_TorchTipTimeOut,pMe);
                }
                
                return TRUE;
            }
#endif            
    		if(wParam == AVK_SPACE)
            {
            	boolean bData;
				MSG_FATAL("AVK_RWD/......................",0,0,0);
#ifdef FEATURE_KEYGUARD
        		if(!OEMKeyguard_IsEnabled())
        		{
        			(void) ICONFIG_GetItem(pMe->m_pConfig,
                                CFGI_KEY_LOCK_CHECK/*CFGI_PHONE_KEY_LOCK_CHECK*/,
                                &bData,
                                sizeof(bData));
        			if(bData)
        			{
                        CoreApp_TimeKeyguard(pMe);
            		}
        		}
#endif	
				return TRUE;
            }
#ifndef FEATURE_VERSION_C11
#if defined(FEATURE_VERSION_MTM)||defined(FEATURE_VERSION_HITZ181)
	     if(wParam == AVK_SHIFT)
#elif defined(FEATURE_VERSION_S1000T) 
	     if(wParam == AVK_RWD)
#endif
            {
            	byte     curProfile;
			    byte     byte_return[PROFILENUMBER];
			    byte     new_return;
                MSG_FATAL("AVK_RWD/......................",0,0,0);
			    (void) ICONFIG_GetItem(pMe->m_pConfig,
			                                CFGI_PROFILE_CUR_NUMBER,
			                                &curProfile,
			                                sizeof(curProfile));
#if defined( FEATURE_VERSION_C306)||defined(FEATURE_VERSION_W0216A) 
			    if(curProfile == OEMNV_PROFILE_QUIETMODE)
			    {
			        curProfile = OEMNV_PROFILE_NORMALMODE;
			        IANNUNCIATOR_SetField (pMe->m_pIAnn, ANNUN_FIELD_RINGTONE, ANNUN_STATE_RINGTONE_ALERT);
			    }
			    else
			    {
			        curProfile = OEMNV_PROFILE_QUIETMODE;
			        IANNUNCIATOR_SetField (pMe->m_pIAnn, ANNUN_FIELD_RINGTONE, ANNUN_STATE_RINGTONE_SILENT);
			    }
#else
			    if(curProfile == OEMNV_PROFILE_MEETING)
			    {
			        curProfile = OEMNV_PROFILE_NORMALMODE;
			        pMe->m_nMsgID = IDS_USERPROFILE_NOMAL;
			        IANNUNCIATOR_SetField (pMe->m_pIAnn, ANNUN_FIELD_RINGTONE, ANNUN_STATE_RINGTONE_ALERT);
			    }
			    else
			    {
			        curProfile = OEMNV_PROFILE_MEETING;
			        pMe->m_nMsgID = IDS_USERPROFILE_MODE;
			        IANNUNCIATOR_SetField (pMe->m_pIAnn, ANNUN_FIELD_RINGTONE, ANNUN_STATE_RINGTONE_VIBRATOR);
			    }
#endif
			    ICONFIG_SetItem(pMe->m_pConfig,
			                        CFGI_PROFILE_CUR_NUMBER,
			                        &curProfile,
			                        sizeof(curProfile));

			    // 修改当前 响闹 模式
			    (void) ICONFIG_GetItem(pMe->m_pConfig,
			                                CFGI_PROFILE_ALERT_TYPE,
			                                byte_return,
			                                sizeof(byte_return)); 
			    
			    new_return = byte_return[curProfile];
			    
			    (void)ICONFIG_SetItem(pMe->m_pConfig,
			                                CFGI_ALERT_TYPE,
			                                &new_return,
			                                sizeof(new_return));    

			    // 修改短信提示方式
			    (void) ICONFIG_GetItem(pMe->m_pConfig,
			                                CFGI_PROFILE_SMS_RINGER,
			                                byte_return,
			                                sizeof(byte_return));   

			    new_return = byte_return[curProfile];

			    (void)ICONFIG_SetItem(pMe->m_pConfig,
			                                CFGI_SMS_RINGER,
			                                &new_return,
			                                sizeof(new_return));   


			    // 修改按键音
			    (void) ICONFIG_GetItem(pMe->m_pConfig,
			                                CFGI_PROFILE_BEEP_VOL,
			                                byte_return,
			                                sizeof(byte_return)); 

			    new_return = byte_return[curProfile];

			    (void)ICONFIG_SetItem(pMe->m_pConfig,
			                                CFGI_BEEP_VOL,
			                                &new_return,
			                                sizeof(new_return));       

			    // 修改未接来电提示音
			    (void) ICONFIG_GetItem(pMe->m_pConfig,
			                                CFGI_PROFILE_MISSED_CALL_ALERT,
			                                byte_return,
			                                sizeof(byte_return));

			    new_return = byte_return[curProfile];

			    (void)ICONFIG_SetItem(pMe->m_pConfig,
			                                CFGI_MISSED_CALL_ALERT,
			                                &new_return,
			                                sizeof(new_return)); 
			    MSG_FATAL("EVT_MODE_CHANGE........................",0,0,0);
      			CLOSE_DIALOG(DLGRET_BATT_INFO)
            	return TRUE;
            }
#endif
		#endif
        #endif    
        case EVT_DIALOG_END:
            // 取消相关定时器			
            (void) ISHELL_CancelTimer(pMe->a.m_pIShell,
                                      CoreApp_UpdateIdleTimer,
                                      pMe);
            (void) ISHELL_CancelTimer(pMe->a.m_pIShell,
                                      CoreApp_SearchingTimer,
                                      pMe);
              #if defined(FEATURE_VERSION_C180) || defined(FEATURE_VERSION_1110W516)|| defined(FEATURE_VERSION_W027)
             IANNUNCIATOR_SetHasTitleText(pMe->m_pIAnn, TRUE);
             #endif
#ifdef FEATURE_VERSION_C11
             ISTATIC_Release(pStatic);
             pStatic = NULL;
             pMe->m_keyinfoheld=FALSE;
             IANNUNCIATOR_SetHasTitleText(pMe->m_pIAnn, TRUE);
#endif
#ifdef FEATURE_KEYGUARD	    
	     (void)ISHELL_CancelTimer(pMe->a.m_pIShell, 
                                    CoreApp_TimeKeyguard,
                                    pMe);
#endif
#ifdef FEATRUE_SET_ANN_FULL_SCREEN
            {
                db_items_value_type  need_capture;
                need_capture.b_capture = DB_CAPTURE_NONE;
                db_put(DB_CAPTURE_WALLPER,&need_capture);
                if(pMe->m_pIAnn != NULL)
                {
                    IANNUNCIATOR_EnableAnnunciatorBar(pMe->m_pIAnn,AEECLSID_DISPLAY1,FALSE);
                }
            }
#endif
            return TRUE;


		//Add By zzg 2011_12_12
#ifdef FEATURE_VERSION_W208S		
		case EVT_USER:
		{
			MSG_FATAL("***zzg IDD_IDLE_Handle EVT_USER wParam=%x***",wParam,0,0);
			
			if (wParam == 1)
			{
				CoreApp_TimeKeyguard(pMe);
			}

			return TRUE;
		}
#endif		
		//Add End
#ifdef FEATURE_VERSION_C11		
		case EVT_KEY_RELEASE:
        if((AVKType)wParam == AVK_INFO)
        {
            if(pMe->m_keyinfoheld)
            {
               return TRUE;
            }
            else
            {
               return CoreApp_LaunchApplet(pMe, AEECLSID_MAIN_MENU);
            }
        }
            return TRUE;
 #endif
        case EVT_KEY_PRESS: 
            if(pMe->m_bemergencymode)
            {
                return TRUE;
            }
            MSG_FATAL("EVT_KEY_PRESS...........wParam=%d.....",wParam,0,0);
            //if set jpeg as wallpaper, it needs more time to finish decode. in this case
            // press other key in idle quickly while exitting from setting wallpaper, topline
            // will be overlapped with signal bar and other app's title.
            if(bImageDecoded == FALSE && pWallPaper)
            {
            	MSG_FATAL("bImageDecoded=%d",bImageDecoded,0,0);
                return TRUE;
            }
            switch (wParam)
            {
#ifdef	FEATURE_APP_BLUETOOTH  
            	case AVK_BLUETOOTH:
					return CoreApp_LaunchApplet(pMe, AEECLSID_BLUETOOTH_APP);
#endif
#ifndef FEATURE_VERSION_C180
                case AVK_MUSIC:
                	if(pMe->m_iskeypadtime)
					{
						return TRUE;
						//AEE_CancelTimer(CoreApp_keypadtimer,pMe);
						//ISHELL_CancelTimer(pMe->a.m_pIShell,CoreApp_keypadtimer,pMe);
					}
#if defined(FEAUTRE_VERSION_N450)||defined(FEATURE_VERSION_W0216A)||defined(FEATURE_VERSION_1110W516) || defined(FEATURE_VERSION_W027) //add by pyuangui
                    return CoreApp_LaunchApplet(pMe, AEECLSID_APP_FMRADIO);   
#else
                    return CoreApp_LaunchApplet(pMe, AEECLSID_APP_MUSICPLAYER);
#endif    
#endif
                case AVK_FM:
                	if(pMe->m_iskeypadtime)
					{
						return TRUE;
						//AEE_CancelTimer(CoreApp_keypadtimer,pMe);
						//ISHELL_CancelTimer(pMe->a.m_pIShell,CoreApp_keypadtimer,pMe);
					}
                    return CoreApp_LaunchApplet(pMe, AEECLSID_APP_FMRADIO);
                case AVK_CAMERA:
                	
                	#if defined(FEATURE_VERSION_C306) ||defined(FEATURE_VERSION_W0216A)|| defined(FEAUTRE_VERSION_N450)|| defined(FEATURE_VERSION_C01)|| defined(FEATURE_VERSION_W516) || defined(FEATURE_VERSION_W027)
                	{
                	
					nv_item_type	SimChoice;
					if(pMe->m_iskeypadtime)
					{
						return TRUE;
						//AEE_CancelTimer(CoreApp_keypadtimer,pMe);
						//ISHELL_CancelTimer(pMe->a.m_pIShell,CoreApp_keypadtimer,pMe);
					}
					OEMNV_Get(NV_SIM_SELECT_I,&SimChoice);
					MSG_FATAL("NV_SIM_SELECT_I...............0000000=%d",SimChoice.sim_select,0,0);
					if(SimChoice.sim_select ==AVK_SEND_TWO)
					{
						MSG_FATAL("NV_SIM_SELECT_I...............",0,0,0);
						if (pMe->m_bAcquiredTime && !pMe->m_bemergencymode)
                    	{
                        	IRecentCalls  *pRecentCall = NULL;
                        	MSG_FATAL("NV_SIM_SELECT_I...............111111",0,0,0);
                        	if (AEE_SUCCESS == ISHELL_CreateInstance(pMe->a.m_pIShell,
                                                                 AEECLSID_APP_RECENTCALL,
                                                                 (void **)&pRecentCall))
                        	{
                            	IRecentCalls_SendList(pRecentCall);
                            	IRecentCalls_Release(pRecentCall);
                        	}
                        	return TRUE;                        
                    	}
					}
					return FALSE;
					}
					#elif defined(FEATURE_VERSION_MYANMAR) || defined(FEATURE_VERSION_W515V3)
					return CoreApp_LaunchApplet(pMe, AEECLSID_APP_FMRADIO);
                    #else
					return CoreApp_LaunchApplet(pMe, AEECLSID_APP_CAMERA);
					#endif
                    
				case AVK_TV:
					if(pMe->m_iskeypadtime)
					{
						return TRUE;
						//AEE_CancelTimer(CoreApp_keypadtimer,pMe);
						//ISHELL_CancelTimer(pMe->a.m_pIShell,CoreApp_keypadtimer,pMe);
					}
				    return CoreApp_LaunchApplet(pMe, AEECLSID_TVAPP);;
                case AVK_UP:
                #ifdef FEATURE_USES_BLACKBERRY
                	return CoreApp_LaunchApplet(pMe, AEECLSID_APP_SOUNDMENU);
                #else
                	if(pMe->m_iskeypadtime)
					{
						return TRUE;
						//AEE_CancelTimer(CoreApp_keypadtimer,pMe);
						//ISHELL_CancelTimer(pMe->a.m_pIShell,CoreApp_keypadtimer,pMe);
					}
#if defined(FEATURE_VERSION_HITZ181)||defined(FEATURE_VERSION_MTM)||defined(FEATURE_VERSION_C337)   //add by yangdecai
            	    return CoreApp_LaunchApplet(pMe, AEECLSID_APP_MUSICPLAYER);
#elif defined (FEATURE_VERSION_W208S)
					return CoreApp_LaunchApplet(pMe, AEECLSID_MAIN_MENU);
#elif defined(FEATURE_VERSION_C11)||defined(FEATURE_VERSION_W317A)
                    return CoreApp_LaunchApplet(pMe, AEECLSID_APP_CAMERA);
#else
                	return CoreApp_LaunchApplet(pMe, AEECLSID_MEDIAGALLERY);
#endif
				#endif
                case AVK_MESSAGE:
                	if(pMe->m_iskeypadtime)
					{
						return TRUE;
						//AEE_CancelTimer(CoreApp_keypadtimer,pMe);
						//ISHELL_CancelTimer(pMe->a.m_pIShell,CoreApp_keypadtimer,pMe);
					}
#if defined(FEATURE_WMS_APP)
                    return CoreApp_LaunchApplet(pMe, AEECLSID_WMSAPP);
#endif
                case AVK_DOWN:
				{
					#ifdef FEATURE_USES_BLACKBERRY
                	return CoreApp_LaunchApplet(pMe, AEECLSID_APP_SOUNDMENU);
                	#else
					if(pMe->m_iskeypadtime)
					{
						return TRUE;
						//AEE_CancelTimer(CoreApp_keypadtimer,pMe);
						///ISHELL_CancelTimer(pMe->a.m_pIShell,CoreApp_keypadtimer,pMe);
					}
//#if !defined(FEATURE_PROJECT_W022) && !defined(FEATURE_PROJECT_W021) && !defined(FEATURE_PROJECT_W021_128x160) && !defined (FEATURE_PROJECT_W021_176X220) && !defined (FEATURE_PROJECT_W021_240X320)&& !defined (FEATURE_PROJECT_W021_220X176) && !defined (FEATURE_PROJECT_W021_320X240)
#if !defined(FEATURE_IDLE_TORCH_DOWNKEY)
	#if defined	(FEATURE_VERSION_FLEXI203)||defined(FEATURE_VERSION_IVIO203)||defined (FEATURE_VERSION_C500BE)
                    return CoreApp_LaunchApplet(pMe, AEECLSID_ALARMCLOCK); 
	#elif defined (FEATURE_VERSION_SMART)
					return CoreApp_LaunchApplet(pMe, AEECLSID_APP_CONTACT); 
	#elif defined (FEATURE_VERSION_M8)
					return CoreApp_LaunchApplet(pMe, AEECLSID_APP_CONTACT);
	#elif defined (FEATURE_VERSION_M8021)
					return CoreApp_LaunchApplet(pMe, AEECLSID_APP_CONTACT);
	#elif defined (FEATURE_VERSION_M8P)
					return CoreApp_LaunchApplet(pMe, AEECLSID_APP_CONTACT); 
	#elif defined(FEATURE_VERSION_HITZ181)||defined(FEATURE_VERSION_MTM)
					return CoreApp_LaunchApplet(pMe, AEECLSID_APP_FMRADIO); 
	#elif defined(FEATURE_VERSION_W208S)
					return CoreApp_LaunchApplet(pMe, AEECLSID_WMSAPP);
    #elif defined (FEATURE_VERSION_C11)
					return CoreApp_LaunchApplet(pMe, AEECLSID_APP_MUSICPLAYER);
    #elif defined(FEATURE_VERSION_VG68)
                    return CoreApp_LaunchApplet(pMe, AEECLSID_WORLDTIME);
    #elif defined(FEATURE_VERSION_W317A)
                    return CoreApp_LaunchApplet(pMe, AEECLSID_SCHEDULEAPP);
    #elif defined(FEATURE_VERSION_C337)
                    return CoreApp_LaunchApplet(pMe, AEECLSID_WMSAPP);
	#else
					return CoreApp_LaunchApplet(pMe, AEECLSID_ALARMCLOCK); 
	#endif
#else
                    MSG_FATAL("in turnOnTorch",0,0,0);
                    if ( pMe->TorchOn == FALSE )
                    {
                        pMe->TorchOn = TRUE;
                        if (pMe->m_pBacklight)
                        {
                            IBACKLIGHT_TurnOnTorch(pMe->m_pBacklight);
                            //IBACKLIGHT_Disable(pMe->m_pBacklight);
                        }
                    }
                    else
                    {
                        pMe->TorchOn = FALSE;
                        if (pMe->m_pBacklight)
                        {
                            IBACKLIGHT_TurnOffTorch(pMe->m_pBacklight);
                            //IBACKLIGHT_Disable(pMe->m_pBacklight);
                        }
                    }

                    return TRUE;
#endif
#endif
                }
                case AVK_LEFT:
                {
#ifdef FEATURE_USES_BLACKBERRY
					if(pMe->m_CurMainItems>0)
					{
						pMe->m_CurMainItems --;
					}
					else
					{
						pMe->m_CurMainItems = 5;
					}
            		CoreApp_DrawBlackBerry_IDLE(pMe);
#else
                	if(pMe->m_iskeypadtime)
					{
						return TRUE;
						//AEE_CancelTimer(CoreApp_keypadtimer,pMe);
						//ISHELL_CancelTimer(pMe->a.m_pIShell,CoreApp_keypadtimer,pMe);
					}
#if defined	(FEATURE_VERSION_FLEXI203)||defined(FEATURE_VERSION_IVIO203) ||defined(FEATURE_VERSION_C500BE)
                    return CoreApp_LaunchApplet(pMe, AEECLSID_APP_SETTINGMENU);
#elif defined (FEATURE_VERSION_SMART)
					return CoreApp_LaunchApplet(pMe, AEECLSID_APP_MUSICPLAYER); 
#elif defined (FEATURE_VERSION_M8)
					return CoreApp_LaunchApplet(pMe, AEECLSID_APP_MUSICPLAYER); 
#elif defined (FEATURE_VERSION_M8021)
					return CoreApp_LaunchApplet(pMe, AEECLSID_APP_MUSICPLAYER); 
#elif defined (FEATURE_VERSION_M8P)
					return CoreApp_LaunchApplet(pMe, AEECLSID_APP_MUSICPLAYER);
#elif defined (FEATURE_VERSION_C11)||defined (FEATURE_VERSION_W317A)
					return CoreApp_LaunchApplet(pMe, AEECLSID_WMSAPP);
#elif defined(FEATURE_VERSION_HITZ181)||defined(FEATURE_VERSION_MTM)
					return CoreApp_LaunchApplet(pMe, AEECLSID_SCHEDULEAPP);
#elif defined(FEATURE_VERSION_W208S)
					return CoreApp_LaunchApplet(pMe, AEECLSID_APPMANAGER);
#elif defined(FEATURE_VERSION_C337)
                    return CoreApp_LaunchApplet(pMe, AEECLSID_APP_FMRADIO);

					return TRUE;
#else
					
					return CoreApp_LaunchApplet(pMe, AEECLSID_APP_SETTINGMENU);
#endif					                    
#endif
                }
				break;
                case AVK_RIGHT:
					{
                        //extern void wms_msg_cdma_mt_MWI(byte  msg_count);
                        //wms_msg_cdma_mt_MWI(0);
                        //return TRUE;
						#ifdef FEATURE_USES_BLACKBERRY
							if(pMe->m_CurMainItems<(IDLE_BLACKBERRY_ITEMMAX-1))
							{
								pMe->m_CurMainItems ++;
							}
							else
							{
								pMe->m_CurMainItems = 0;
							}
		            		CoreApp_DrawBlackBerry_IDLE(pMe);
						#else
						if(pMe->m_iskeypadtime)
						{
							//AEE_CancelTimer(CoreApp_keypadtimer,pMe);
							//ISHELL_CancelTimer(pMe->a.m_pIShell,CoreApp_keypadtimer,pMe);
							return TRUE;
						}
						#if defined(FEATURE_VERSION_HITZ181)||defined(FEATURE_VERSION_MTM)
							return CoreApp_LaunchApplet(pMe, AEECLSID_WMSAPP);//
					    #elif defined(FEATURE_VERSION_W0216A)||defined(FEATURE_VERSION_C180)
					    	return CoreApp_LaunchApplet(pMe, AEECLSID_APP_MUSICPLAYER);//
					    
				    	#elif defined(FEATURE_VERSION_W208S)
				    	{
                        	char	buf[12];
		                    
                    		OEM_SetBROWSER_ADSAccount();
                    		
                        	start_info.appid_fx = APP_ORIGINATOR_BROWSER;
                            start_info.subtype_keycode = APP_BROWSER_START_MAINMENU;
                            start_info.par_keychar = 0;
                            //start_info.fInfo.url_info.title = "163";
                            //start_info.fInfo.url_info.url = "http://www.163.com";
                            SPRINTF(buf, "%p", &start_info);
                            if(SUCCESS == ISHELL_StartAppletArgs(pMe->a.m_pIShell,AEECLSID_NF3,buf))
                            {
                                return TRUE;
                            }
                            else
                            {
                                return FALSE;
                            }
				    	}
                        #elif defined(FEATURE_VERSION_VG68)||defined(FEATURE_VERSION_C337)
                            return CoreApp_LaunchApplet(pMe, AEECLSID_ALARMCLOCK);
                        #elif defined(FEATURE_VERSION_C11)
                            return CoreApp_LaunchApplet(pMe, AEECLSID_APP_SETTINGMENU);
						#else
							return CoreApp_LaunchApplet(pMe, AEECLSID_APP_FMRADIO);//
						#endif
						#endif
					}
					break;
                case AVK_SELECT:
		 		{
				    int ret = 0;
				    if(!OEMKeyguard_IsEnabled())
                    {
                        #if defined( FEATURE_VERSION_W515V3) 
                           Mainmenu_KeypadLock(TRUE);
                        #endif
                        #if defined(FEATURE_VERSION_MYANMAR) || defined( FEATURE_VERSION_C11) || defined(FEATURE_VERSION_C180) || defined(FEATURE_VERSION_1110W516) || defined(FEATURE_VERSION_W0271)
                           WMSDialog_KeypadLock(TRUE);
                        #endif
    				    #ifdef FEATURE_USES_BLACKBERRY
    				        ret = CoreApp_LaunchApplet(pMe, AEECLSID_MAIN_MENU);
    				    #else
#if defined( FEATURE_VERSION_C306)||defined(FEATURE_VERSION_W0216A)//|| //defined(FEATURE_VERSION_MYANMAR) //||defined( FEATURE_VERSION_W515V3)
    					if(!pMe->m_iskeypadtime)
    					{
    						
    						//ISHELL_CancelTimer(pMe->a.m_pIShell,CoreApp_keypadtimer,pMe);
    						AEE_CancelTimer(CoreApp_keypadtimer,pMe);
    						AEE_SetTimer(2*1000,CoreApp_keypadtimer,pMe);
    						//ISHELL_SetTimer(pMe->a.m_pIShell,2*1000,CoreApp_keypadtimer,pMe);
    						pMe->m_iskeypadtime = TRUE;
    					}
#else
#ifdef FEATURE_VERSION_W208S
                        ret= CoreApp_LaunchApplet(pMe, AEECLSID_MAIN_MENU);
#endif
#if defined	(FEATURE_VERSION_FLEXI203) 
#ifdef FEATURE_FLEXI_STATIC_BREW_APP				
#if defined (FEATURE_NASRANI)
        			    OEM_SetBAM_ADSAccount(STATIC_BREW_APP_FLEXI_NASRANI);
                        ret=  CoreApp_LaunchApplet(pMe, AEECLSID_NASRANI);
#elif defined  (FEATURE_GURU)
                        OEM_SetBAM_ADSAccount(STATIC_BREW_APP_FLEXI_MUSLIM);
                        ret=  CoreApp_LaunchApplet(pMe, AEECLSID_FLEXIGURU);
#elif defined (FEATURE_FMN2010)
    			        OEM_SetBAM_ADSAccount(STATIC_BREW_APP_FLEXI_MUSLIM);
                        ret=  CoreApp_LaunchApplet(pMe, AEECLSID_MUSLIM);
#elif defined (FEATURE_FPT005)
    			        ret= CoreApp_LaunchApplet(pMe, AEECLSID_APP_CONTACT);
#else
    			        ret= CoreApp_LaunchApplet(pMe, AEECLSID_WMSAPP);
#endif /*FEATURE_NASRANI*/
#else
                        ret= CoreApp_LaunchApplet(pMe, AEECLSID_WMSAPP);
#endif  /*FEATURE_FLEXI_STATIC_BREW_APP*/
#elif defined (FEATURE_VERSION_IVIO203)||defined (FEATURE_VERSION_C500BE)
    				    ret= CoreApp_LaunchApplet(pMe, AEECLSID_WMSAPP);
#elif defined (FEATURE_VERSION_FLEXI203P)
    				    ret= CoreApp_LaunchApplet(pMe, AEECLSID_STATIC_APP);
#elif defined (FEATURE_VERSION_SMART)
#ifdef FEATURE_SMARTFREN_STATIC_BREW_APP	
    				    OEM_SetBAM_ADSAccount(STATIC_BREW_APP_SMARTFREN_FACEBOOK);
    				    ret= CoreApp_LaunchApplet(pMe, AEECLSID_SMARTFREN_FACEBOOK);
#else
    				    ret= CoreApp_LaunchApplet(pMe, AEECLSID_WMSAPP);
#endif /*FEATURE_SMARTFREN_STATIC_BREW_APP*/
#elif defined (FEATURE_VERSION_M8021)
#ifdef FEATURE_SMARTFREN_STATIC_BREW_APP	
    				    OEM_SetBAM_ADSAccount(STATIC_BREW_APP_SMARTFREN_FACEBOOK);
    				    ret= CoreApp_LaunchApplet(pMe, AEECLSID_SMARTFREN_FACEBOOK);
#else
    				    ret= CoreApp_LaunchApplet(pMe, AEECLSID_WMSAPP);
#endif	/*FEATURE_SMARTFREN_STATIC_BREW_APP*/
#elif defined (FEATURE_VERSION_M8)
#ifdef FEATURE_SMARTFREN_STATIC_BREW_APP	
    				    OEM_SetBAM_ADSAccount(STATIC_BREW_APP_SMARTFREN_FACEBOOK);
    				    ret= CoreApp_LaunchApplet(pMe, AEECLSID_SMARTFREN_FACEBOOK);
#else
    				    ret= CoreApp_LaunchApplet(pMe, AEECLSID_WMSAPP);
#endif	/*FEATURE_SMARTFREN_STATIC_BREW_APP*/
#elif defined (FEATURE_VERSION_M8P)
#ifdef FEATURE_SMARTFREN_STATIC_BREW_APP	
    				    OEM_SetBAM_ADSAccount(STATIC_BREW_APP_SMARTFREN_FACEBOOK);
    				    ret= CoreApp_LaunchApplet(pMe, AEECLSID_SMARTFREN_FACEBOOK_HITZ233C);
#else
    				    ret= CoreApp_LaunchApplet(pMe, AEECLSID_WMSAPP);
#endif	/*FEATURE_SMARTFREN_STATIC_BREW_APP*/
#elif defined (FEATURE_VERSION_HITZ181) || defined (FEATURE_VERSION_W515V3)
    				    ret= CoreApp_LaunchApplet(pMe, AEECLSID_MAIN_MENU);
#else
                        ret= CoreApp_LaunchApplet(pMe, AEECLSID_WMSAPP);
#endif				    
#endif
#endif
                        return ret;
                    }
				    break;
                }
                
                case AVK_INFO:
					{
#ifdef FEATURE_USES_BLACKBERRY
						switch (pMe->m_CurMainItems)
						{
							case 0:
								{
									return CoreApp_LaunchApplet(pMe, AEECLSID_WMSAPP);
								}
								break;
							case 1:
								{
									return CoreApp_LaunchApplet(pMe, AEECLSID_APP_CONTACT);
								}
								break;
							case 2:
								{
									return CoreApp_LaunchApplet(pMe, AEECLSID_ALARMCLOCK);
								}
								break;
							case 3:
								{
									return CoreApp_LaunchApplet(pMe, AEECLSID_SCHEDULEAPP);
								}
								break;
							case 4:
								{
									return CoreApp_LaunchApplet(pMe, AEECLSID_APP_MUSICPLAYER);
								}
								break;
							case 5:
								{
									return CoreApp_LaunchApplet(pMe, AEECLSID_APP_FMRADIO);
								}
								break;
						}
						
#else
#if defined(FEATURE_VERSION_C01)	
                     if(!pMe->m_iskeypadtime)
        			   {
    					   AEE_CancelTimer(CoreApp_keypadtimer,pMe);
    					   AEE_SetTimer(1000,CoreApp_keypadtimer,pMe);
    					   pMe->m_iskeypadtime = TRUE;
        			   }
                    
						return TRUE;
#elif defined (FEATURE_VERSION_C11) 
                        pMe->m_keyinfoheld=FALSE;
                        return TRUE;
#else
                       if(pMe->m_iskeypadtime)
						{
							//AEE_CancelTimer(CoreApp_keypadtimer,pMe);
							return TRUE;
							//ISHELL_CancelTimer(pMe->a.m_pIShell,CoreApp_keypadtimer,pMe);
						}
#if defined(FEATURE_VERSION_W027)                       
                        Mainmenu_KeypadLock(TRUE);
#endif
						return CoreApp_LaunchApplet(pMe, AEECLSID_MAIN_MENU);
#endif	

#endif
					}
					break;
#ifndef FEATURE_LCD_TOUCH_ENABLE
#if defined(FEATURE_VERSION_SMART) || defined(FEATURE_VERSION_M8) || defined(FEATURE_VERSION_M8P) || defined (FEATURE_VERSION_M8021)
				case AVK_SOFT2:
#else
                case AVK_CLR:
#endif
					if(!OEMKeyguard_IsEnabled())
                    {
                    	MSG_FATAL("DLGRET_EMGCALL...................................4444",0,0,0);
						if (dwParam == 1)
						{
							return TRUE;
						}
                    	if(pMe->m_iskeypadtime)
						{
							//AEE_CancelTimer(CoreApp_keypadtimer,pMe);
							//ISHELL_CancelTimer(pMe->a.m_pIShell,CoreApp_keypadtimer,pMe);
							return TRUE;
						}
#if defined (FEATURE_VERSION_SMART)
#ifdef FEATURE_SMARTFREN_STATIC_BREW_APP    
                    	OEM_SetBAM_ADSAccount(STATIC_BREW_APP_SMARTFREN_SFM);
						return CoreApp_LaunchApplet(pMe, AEECLSID_SMARTFREN_SFM); 
#else                   
						return CoreApp_LaunchApplet(pMe, AEECLSID_APP_CONTACT);		
#endif	/*FEATURE_SMARTFREN_STATIC_BREW_APP*/				
#elif defined (FEATURE_VERSION_M8) || defined(FEATURE_VERSION_M8021)
#ifdef FEATURE_SMARTFREN_STATIC_BREW_APP   
						OEM_SetBAM_ADSAccount(STATIC_BREW_APP_SMARTFREN_SFM);
						return CoreApp_LaunchApplet(pMe, AEECLSID_SMARTFREN_SFM);	
#else                   
						return CoreApp_LaunchApplet(pMe, AEECLSID_APP_CONTACT); 	
#endif	/*FEATURE_SMARTFREN_STATIC_BREW_APP*/
#elif defined (FEATURE_VERSION_M8P)
#ifdef FEATURE_SMARTFREN_STATIC_BREW_APP   
						OEM_SetBAM_ADSAccount(STATIC_BREW_APP_SMARTFREN_SFM);
						return CoreApp_LaunchApplet(pMe, AEECLSID_SMARTFREN_SFM_HITZ233C);	
#else                   
						return CoreApp_LaunchApplet(pMe, AEECLSID_APP_CONTACT); 	
#endif	/*FEATURE_SMARTFREN_STATIC_BREW_APP*/
#elif defined (FEATURE_VERSION_FLEXI203)
#if defined(FEATURE_FLEXI_STATIC_BREW_APP)&&defined(FEATURE_FPT005)
						 OEM_SetBAM_ADSAccount(STATIC_BREW_APP_FLEXI_PORTAL);
						 return CoreApp_LaunchApplet(pMe, AEECLSID_FBROWSER);
#else
						 return CoreApp_LaunchApplet(pMe, AEECLSID_APP_CONTACT);
#endif /*FEATURE_FLEXI_STATIC_BREW_APP&&FEATURE_FPT005*/
#elif defined (FEATURE_VERSION_FLEXI203P)
        	   			return CoreApp_LaunchApplet(pMe, AEECLSID_APP_CONTACT);
#else
#ifdef FEATURE_VERSION_HITZ181
						 if(!IRUIM_IsCardConnected(pMe->m_pIRUIM))
						 {
						 	AECHAR m_Numcall[5] = {L"112"};
						 	(void)MakeVoiceCall(pMe->a.m_pIShell, FALSE, (AECHAR *)m_Numcall);
						 }
						 else
#endif
						 {
						 	return CoreApp_LaunchApplet(pMe, AEECLSID_APP_CONTACT);
						 }
#endif

                    }
			
				    break;
#endif
                default:
                    break;
            }
            return FALSE;
#ifdef FEATURE_LCD_TOUCH_ENABLE
		case EVT_PEN_DOWN:
			{
				break;
			}
		case EVT_PEN_MOVE:
			{
				break;
			}
		case EVT_PEN_UP:
			{
				AEERect rc;
				uint16 wXPos = (int16)AEE_GET_X(dwParam);
				uint16 wYPos = (int16)AEE_GET_Y(dwParam);
				uint16 i = 0;
				boolean m_bInRect = FALSE;
				rc.y = SCREEN_HEIGHT - IDLE_TOUCH_DRAWDX - 40;;
				rc.x = (IDLE_TOUCH_IDLE_BOTTOM_SPC*(i+1))+(i*IDLE_TOUCH_DRAWDX);
				for(i=0;i<IDLE_TOUCH_ITEMMAX;)
				{
					rc.x  = (IDLE_TOUCH_IDLE_BOTTOM_SPC*(i+1))+(i*IDLE_TOUCH_DRAWDX);
					rc.dx = IDLE_TOUCH_DRAWDX;
					rc.dy = IDLE_TOUCH_DRAWDX;
					m_bInRect = CORE_PT_IN_RECT(wXPos,wYPos,rc);
					if(m_bInRect)
					{
						MSG_FATAL("m_bInRect......222........I=%d",i,0,0);
						break;
					}
					i++;
				}
				MSG_FATAL("m_bInRect......111........I=%d",i,0,0);
				switch(i)
				{
					case 0:
						{
						//return CoreApp_LaunchApplet(pMe, AEECLSID_DIALER);
						ICallApp         *pCallApp = NULL;
                        if ( SUCCESS != ISHELL_CreateInstance( pMe->a.m_pIShell,
                                                        AEECLSID_DIALER,
                                                        (void **)&pCallApp))
                        {
                            return FALSE;
                        }
                        MEMSET(pMe->m_wstrEnterNum, 0, sizeof(pMe->m_wstrEnterNum));
                        ICallApp_VoiceCallDial_F(pCallApp,pMe->m_wstrEnterNum);
                        if (pCallApp) 
                        {
                            ICallApp_Release(pCallApp);
                            pCallApp = NULL;
                        }
						return TRUE;
						}
						break;
					case 1:
						return CoreApp_LaunchApplet(pMe, AEECLSID_MAIN_MENU);
						break;
					case 2:
						return CoreApp_LaunchApplet(pMe, AEECLSID_WMSAPP);
						break;
					case 3:
						return CoreApp_LaunchApplet(pMe, AEECLSID_APP_CONTACT);
						break;
					default:
						break;
				}
				break;
			}
#endif

        case EVT_GSENSOR_SHAKE:
        case EVT_KEY:
            {
#ifdef FEATRUE_SUPPORT_G_SENSOR
                boolean b_FMBackground = FALSE;
                dword shake;
#endif
#ifndef CUST_EDITION
                if((wParam != AVK_END)&&(wParam != AVK_GSENSOR_FORWARD)&&(wParam != AVK_GSENSOR_BACKWARD))
                {
                   if(!((MMI_GSENSOR_SHAKE_CLOSE == mmi_g_sensor_state) 
                    ||(MMI_GSENSOR_SHAKE_CLOSE_IN_IDLE == mmi_g_sensor_state)))  //ignore if sensor has been closed.
                    {
                        OEM_GetConfig(CFGI_GSENSOR,&shake,sizeof(shake));
                        OEM_GetConfig(CFGI_FM_BACKGROUND,&b_FMBackground, sizeof(b_FMBackground));
                		//do close shake if FM or MP3 is playing.
                        if(!(((shake & OEMNV_SHAKE_FM_RADIO_MASK) && (TRUE == b_FMBackground))
                            ||((shake & OEMNV_SHAKE_MUSIC_MASK) && (MP3STATUS_NONE != GetMp3PlayerStatus()))))
						{
                    		if(is_g_sportsapp_pedometer_bground_flag() == FALSE)
                            {
#ifdef FEATRUE_SUPPORT_G_SENSOR	
#ifndef WIN32//wlh 临时修改
                                mmi_g_sensor_process(G_SENSOR_SHAKE_DISABLE);
#endif//WIN32
#endif             
                                mmi_g_sensor_state = MMI_GSENSOR_SHAKE_CLOSE;                    
                            }      
                		}
                    }
                }
#endif                
            }		

            MSG_FATAL("***zzg EVT_KEY wParam=%x, dwParam=%x", wParam, dwParam, 0);
			
            if(pMe->m_bemergencymode)
            {
                switch (wParam)
                {
                	case AVK_SOFT2:		//Add By zzg 2010_09_08 for smart and m8
                    case AVK_CLR:
                    case AVK_END:
                        CLOSE_DIALOG(DLGRET_MSG)
                        //CoreApp_ShowMsgDialog(pMe,IDS_EXIT_EMERGENCY_MODE);
                        return TRUE;
                    case AVK_CAMERA:
                    
						#if defined(FEATURE_VERSION_C306)||defined(FEATURE_VERSION_W0216A)|| defined(FEAUTRE_VERSION_N450)|| defined(FEATURE_VERSION_C01)|| defined(FEATURE_VERSION_W516) || defined(FEATURE_VERSION_W027)
						{
						nv_item_type	SimChoice;
						OEMNV_Get(NV_SIM_SELECT_I,&SimChoice);
						if(SimChoice.sim_select ==AVK_SEND_TWO)
						{
							return TRUE;
						}
						}
						#endif
						break;
                    case AVK_SEND:
                    case AVK_LEFT:   
                        return TRUE;
                        
                    default:
                        break;
                }
            }

            //if set jpeg as wallpaper, it needs more time to finish decode. in this case
            // press select key in idle quickly while exitting from setting wallpaper, topline
            // will be overlapped with signal bar and other app's title.
            if(bImageDecoded == FALSE && pWallPaper)
            {
                return TRUE;
            }
            
            switch (wParam)
            {
            	case AVK_SOFT2:		//Add By zzg 2010_09_08 for smart and m8
                case AVK_CLR:

#if  defined(FEATURE_VERSION_W515V3)|| defined(FEATURE_VERSION_C11) || defined(FEATURE_VERSION_C180) || defined(FEATURE_VERSION_1110W516) //|| defined(FEATURE_VERSION_W027)
                if(pMe->m_IsSametime)
                   {
                       return TRUE;
                   }
                   else
                   {
                   if(OEMKeyguard_IsEnabled())
                   {
                       OEMKeyguard_SetState(FALSE);
                       pMe->m_b_set_lock = TRUE;
                       CLOSE_DIALOG(DLGRET_EMGCALL)
                   }
                   }
#endif
                    return TRUE;

                case AVK_SELECT:
                #ifndef FEATURE_VERSION_W027     
                #ifdef FEATURE_KEYGUARD
                 if(pMe->m_IsSametime)
				    {
				    	return TRUE;
				    }
				    else
				    {
                    if(OEMKeyguard_IsEnabled())
                    {
                    	MSG_FATAL("DLGRET_EMGCALL...................................777",0,0,0);
                    	#ifndef FEATURE_VERSION_HITZ181
                        OEMKeyguard_SetState(FALSE);
                        pMe->m_b_set_lock = TRUE;
                        CLOSE_DIALOG(DLGRET_EMGCALL)
                        #endif
                    }
                    }
				#endif
                #endif
                    //return   CoreApp_LaunchApplet(pMe, AEECLSID_DIALER);
                    return TRUE;
                 // 重拨
                case AVK_SEND:
                	  #if defined(FEATURE_VERSION_C01) 
					{
						nv_item_type	SimChoice;
						OEMNV_Get(NV_SIM_SELECT_I,&SimChoice);
						if(SimChoice.sim_select==AVK_SEND_TWO)
						{
							return TRUE;
						}
					}
					#endif
                    if (/*pMe->m_bAcquiredTime &&*/ !pMe->m_bemergencymode)
                    {
                        IRecentCalls  *pRecentCall = NULL;
                        if (AEE_SUCCESS == ISHELL_CreateInstance(pMe->a.m_pIShell,
                                                                 AEECLSID_APP_RECENTCALL,
                                                                 (void **)&pRecentCall))
                        {
                            IRecentCalls_SendList(pRecentCall);
                            IRecentCalls_Release(pRecentCall);
                        }
                        return TRUE;                        
                    }
                    break;
                
				case AVK_SHIFT:
					{
					//#if defined(FEATURE_PROJECT_SMART) || defined(FEATURE_PROJECT_M8)
					#if defined (FEATURE_DOUBLE_SHIFT_NOTEQUAL_W)
						pMe->m_isShift = FALSE;
						return TRUE;
					#else
						if(!pMe->m_isShift)
						{
							pMe->m_isShift = TRUE;
						}
						else
						{
							pMe->m_isShift = FALSE;
						}
						return TRUE;
					#endif
					}
				break;

                case AVK_0:
                case AVK_1:
                case AVK_2:
                case AVK_3:
                case AVK_4:
                case AVK_5:
                case AVK_6:
                case AVK_7:
                case AVK_8:
                case AVK_9:
                case AVK_STAR:
                case AVK_POUND:
				case AVK_P:
				case AVK_O:
                    {
                        ICallApp         *pCallApp = NULL;
                        #if defined( FEATURE_VERSION_C306)||defined(FEATURE_VERSION_W0216A)|| defined(FEATURE_VERSION_MYANMAR) || defined( FEATURE_VERSION_C01) || defined( FEATURE_VERSION_C11)|| defined(FEATURE_VERSION_C180) || defined( FEATURE_VERSION_W515V3) || defined(FEATURE_VERSION_1110W516) || defined(FEATURE_VERSION_W027)
                        if(pMe->m_iskeypadtime)
                        {
                        	if(wParam==AVK_STAR)
                        	{
	                        	boolean bData;
				            	#ifdef FEATURE_KEYGUARD
				            	MSG_FATAL("EVT_USER.....................",0,0,0);
				        		if(!OEMKeyguard_IsEnabled())
				        		{
				        			(void) ICONFIG_GetItem(pMe->m_pConfig,
				                                CFGI_KEY_LOCK_CHECK/*CFGI_PHONE_KEY_LOCK_CHECK*/,
				                                &bData,
				                                sizeof(bData));
				                    pMe->m_iskeypadtime = FALSE;
                                    #if defined(FEATURE_VERSION_W515V3) || defined(FEATURE_VERSION_MYANMAR) || defined( FEATURE_VERSION_C11)|| defined(FEATURE_VERSION_C180) || defined(FEATURE_VERSION_1110W516) || defined(FEATURE_VERSION_W027)
                                      CoreApp_TimeKeyguard(pMe);
                                    #else
				        			if(bData)
				        			{
				                        CoreApp_TimeKeyguard(pMe);
				            		}
                                    #endif
				        		}
								#endif	
								AEE_CancelTimer(CoreApp_keypadtimer,pMe);
	                        	pMe->m_iskeypadtime = FALSE;
	                        	return TRUE;
                        	}
                        	else
                        	{
                        		return TRUE;
                        	}
                        }
                        
                        #endif
                        if ( SUCCESS != ISHELL_CreateInstance( pMe->a.m_pIShell,
                                                        AEECLSID_DIALER,
                                                        (void **)&pCallApp))
                        {
                            return FALSE;
                        }
                        MEMSET(pMe->m_wstrEnterNum, 0, sizeof(pMe->m_wstrEnterNum));
                        if ((AVKType)wParam == AVK_STAR)
                        {
                            pMe->m_wstrEnterNum[0] = (AECHAR)'*';
                        }
                        else if ((AVKType)wParam == AVK_POUND)
                        {
                            pMe->m_wstrEnterNum[0] = (AECHAR)'#';
                        }
						else if((AVKType)wParam == AVK_O)
						{
						
							pMe->m_wstrEnterNum[0] = (AECHAR)'+';
						}
						else if((AVKType)wParam == AVK_P)
						{

							pMe->m_wstrEnterNum[0] = (AECHAR)'p';
						}
                        else
                        {
                            // 48 为'0'的 ASCII 码值
                            if(pMe->m_isShift && (wParam == AVK_1))
                            {
                            	
                            	pMe->m_wstrEnterNum[0] = (AECHAR)'w';
                            }
							else
							{
                            	pMe->m_wstrEnterNum[0] = (AECHAR)
                                	(48 + ((AVKType)wParam - AVK_0));/*lint !e656*/
							}
                        }
                        ICallApp_VoiceCallDial_F(pCallApp,pMe->m_wstrEnterNum);
                        if (pCallApp) 
                        {
                            ICallApp_Release(pCallApp);
                            pCallApp = NULL;
                        }
                        //MakeVoiceCallDial(pMe->a.m_pIShell , pMe->m_wstrEnterNum);
                        return TRUE;
                    }
#ifndef CUST_EDITION
                case AVK_GSENSOR_FORWARD: 
                 {
                   boolean isShake0n = FALSE; 
                   (void) ICONFIG_GetItem(pMe->m_pConfig,
                           CFGI_SHAKE_WALLPAPER_CHECK,
                           &isShake0n, 
                           sizeof(boolean));
                    if(isShake0n)
                    {
                         IDisplayMenu * pDisplayMenu = NULL;
                         if (SUCCESS == ISHELL_CreateInstance(pMe->a.m_pIShell,
                             AEECLSID_APP_DISPLAYMENU,
                             (void**)&pDisplayMenu))
                         {
                             if (pDisplayMenu == NULL)
                             {
                                 return FALSE;
                             }
                             IDisplayMenu_ChangePicture(pDisplayMenu, TRUE);
                         }
                         IDisplayMenu_Release(pDisplayMenu);
                         CLOSE_DIALOG(DLGRET_GS_SHAKE);
                         return TRUE;
                    }
                    else
                    {
                         return TRUE;
                    }
                     break;
                 }
                
                 case AVK_GSENSOR_BACKWARD:
                 {
                    boolean isShake0n = FALSE; 
                     (void) ICONFIG_GetItem(pMe->m_pConfig,
                            CFGI_SHAKE_WALLPAPER_CHECK,
                            &isShake0n, 
                            sizeof(boolean));
                     if(isShake0n)
                     {
                         IDisplayMenu * pDisplayMenu = NULL;
                         if (SUCCESS == ISHELL_CreateInstance(pMe->a.m_pIShell,
                             AEECLSID_APP_DISPLAYMENU,
                             (void**)&pDisplayMenu))
                         {
                             if (pDisplayMenu == NULL)
                             {
                                 return FALSE;
                             }
                             IDisplayMenu_ChangePicture(pDisplayMenu, FALSE);
                         }
                         IDisplayMenu_Release(pDisplayMenu);
                         CLOSE_DIALOG(DLGRET_GS_SHAKE);
                         return TRUE;
                     }
                     else
                     {
                         return TRUE;
                     }
                     break;
                 }
#endif                 
          
                default:
                    break;
            }
            break;
        default:
            break;
    }
    return FALSE;
} // IDD_IDLE_Handler

#if defined(FEATURE_WMS_APP)
/*==============================================================================
函数:
    IDD_WMSTIPS_Handler

说明:
    CoreApp 对话框 IDD_WMSTIPS 事件处理函数。

参数:
    pMe [in]:       指向Core Applet对象结构的指针。该结构包含小程序的特定信息。
    eCode [in]:     事件代码。
    wParam:         事件参数
    dwParam [in]:   与wParam关联的数据。

返回值:
    TRUE:  传入事件得到处理。
    FALSE: 传入事件没被处理。

备注:

==============================================================================*/
static boolean IDD_WMSTIPS_Handler(void        *pUser, 
                                   AEEEvent    eCode, 
                                   uint16      wParam, 
                                   uint32      dwParam)
{
    static IStatic * pStatic = NULL;
    static IBitmap * pDevBmp = NULL;
    CCoreApp *pMe = (CCoreApp *)pUser;
    if (NULL == pMe)
    {
        return FALSE;
    }
    
    if (NULL == pStatic)
    {
        AEERect rc = {0};

        if (ISHELL_CreateInstance(pMe->a.m_pIShell, AEECLSID_STATIC,
                (void**)&pStatic) == SUCCESS)
        {
            ISTATIC_SetRect(pStatic, &rc);
        }
        else
        {
            return FALSE;
        }
    }

    switch (eCode)
    {
        case EVT_DIALOG_INIT:
            if(pMe->m_pIAnn != NULL)
            {
                IANNUNCIATOR_EnableAnnunciatorBar(pMe->m_pIAnn,AEECLSID_DISPLAY1,TRUE);
            }
            return TRUE;

        case EVT_DIALOG_START:
            (void) ISHELL_PostEvent(pMe->a.m_pIShell, 
                                    AEECLSID_CORE_APP,
                                    EVT_USER_REDRAW,
                                    0, 
                                    0);
            return TRUE;

        case EVT_USER_REDRAW:
            {
                AECHAR  wstrText[64]={0};
                AECHAR  wstrFMT[64]={0};
                uint16  nNewsVmail=0, nNewsSMS=0;
                PromptMsg_Param_type  Msg_Param={0};
                db_items_value_type  need_capture;
				//int i = 0;
				//int temp = 0;
                
                need_capture.b_capture = DB_CAPTURE_NONE;
                db_put(DB_CAPTURE_WALLPER,&need_capture);
                // 获取消息数
                wms_cacheinfolist_getcounts(WMS_MB_VOICEMAIL, &nNewsVmail, NULL, NULL);
                wms_cacheinfolist_getcounts(WMS_MB_INBOX, &nNewsSMS, NULL, NULL);
                MSG_FATAL("IDD_WMSTIPS_Handler %d %d",nNewsVmail,nNewsSMS,0);
                if (gbWmsVMailNtf && !(gbWmsLastNtfIsSMS && gbWmsSMSNtf))
                {
                    // 从资源文件取消息内容
                    (void)ISHELL_LoadResString(pMe->a.m_pIShell,
                                    AEE_COREAPPRES_LANGFILE,                                
                                    IDS_VMAILTIPS_FMT,
                                    wstrFMT,
                                    sizeof(wstrFMT));
                                    
                    WSPRINTF(wstrText, sizeof(wstrText), wstrFMT, nNewsVmail);
                }
                else
                {
                    // 从资源文件取消息内容
                    (void)ISHELL_LoadResString(pMe->a.m_pIShell,
                                    AEE_COREAPPRES_LANGFILE,                                
                                    IDS_NEWWMSTIPS_FMT,
                                    wstrFMT,
                                    sizeof(wstrFMT));
                                    
                    WSPRINTF(wstrText, sizeof(wstrText), wstrFMT, nNewsSMS);
                }
  
                Msg_Param.ePMsgType = MESSAGE_INFORMATION;
                Msg_Param.pwszMsg = wstrText;
                Msg_Param.eBBarType = BTBAR_READ_BACK;

                if(pDevBmp != NULL)
                {
                    IDISPLAY_BitBlt(pMe->m_pDisplay, 0, 0, pMe->m_rc.dx, pMe->m_rc.dy, 
                            pDevBmp, 0, 0, AEE_RO_COPY);
                }
                DrawPromptMessage(pMe->m_pDisplay, pStatic, &Msg_Param);
            }
            //IDISPLAY_UpdateEx(pMe->m_pDisplay, FALSE);
            
            return TRUE;

        case EVT_DIALOG_END:
            if(pMe->m_bActive)
            {
                if(pDevBmp != NULL)
                {
                    IBITMAP_Release(pDevBmp);
                    pDevBmp = NULL;
                }
            }
            else if(NULL == pDevBmp)
            {
                IBitmap *pTempBmp = NULL;

                IDISPLAY_GetDeviceBitmap(pMe->m_pDisplay, &pTempBmp);
                IBITMAP_CreateCompatibleBitmap(pTempBmp, &pDevBmp, pMe->m_rc.dx, pMe->m_rc.dy);
                if(NULL != pTempBmp && NULL != pDevBmp)
                {
                    IBITMAP_BltIn(pDevBmp, 0, 0, pMe->m_rc.dx, pMe->m_rc.dy, pTempBmp, 0, 0, AEE_RO_COPY);
                }
            }
            ISTATIC_Release(pStatic);
            pStatic = NULL;
            if(pMe->m_pIAnn != NULL)
            {
                IANNUNCIATOR_EnableAnnunciatorBar(pMe->m_pIAnn,AEECLSID_DISPLAY1,FALSE);
            }
            return TRUE;
  
        case EVT_KEY:
            switch(wParam)
            {
            	case AVK_SOFT2:		//Add By zzg 2010_09_08 for smart and m8
                case AVK_CLR:
                case AVK_END:
#ifdef FEATURE_SMSTONETYPE_MID 
                // 通知收音机关闭声音
                    IALERT_StopSMSAlert(pMe->m_pAlert);
#endif //#if defined FEATURE_SMSTONETYPE_MID		    
                    IALERT_StopMp3Alert(pMe->m_pAlert);
                    pMe->m_bsmstipscheck = FALSE;
                    CLOSE_DIALOG(DLGRET_SMSTIPS_OK)
                    return TRUE;

                case AVK_SELECT:
#ifdef FEATURE_SMSTONETYPE_MID                        
                    IALERT_StopSMSAlert(pMe->m_pAlert);
#endif //#if defined FEATURE_SMSTONETYPE_MID		    
                    IALERT_StopMp3Alert(pMe->m_pAlert);
                    pMe->m_bsmstipscheck = FALSE;
                    CLOSE_DIALOG(DLGRET_VIEWSMS)
                    return TRUE;
  
                default:
                    break;
            }
            break;

        default:
            break;
    }

    return FALSE;
} // IDD_WMSTIPS_Handler
#endif

/*==============================================================================
函数：
       IDD_POWERDOWN_Handler
说明：
       IDD_POWERDOWN对话框事件处理函数
       
参数：
       pMe [in]：指向Core Applet对象结构的指针。该结构包含小程序的特定信息。
       eCode [in]：事件代码。
       wParam：事件相关数据。
       dwParam：事件相关数据。
       
返回值：
       TRUE：传入事件被处理。
       FALSE：传入事件被忽略。
       
备注：WHEN                WHO      WHAT          WHY
     2008-2-16           fj       V1.6          New develop, Add power down animation      
==============================================================================*/
static boolean  IDD_POWERDOWN_Handler(void *pUser,
                                      AEEEvent   eCode,
                                      uint16     wParam,
                                      uint32     dwParam)
{ 
    CCoreApp *pMe = (CCoreApp *)pUser;    
    switch (eCode)
    {
        case EVT_DIALOG_INIT:
            EnableUIKeys(FALSE);
            return TRUE;
            
        case EVT_DIALOG_START:
#ifdef FEATURE_UIALARM
            // 挂起警报器
            CoreApp_EnableShellAlarms(pMe, FALSE);
#endif
            
#ifdef FEATRUE_AUTO_POWER		
            OEMRTC_Process_Auto_Power_On();
#endif

#if defined( FEATURE_POWERDOWN_ALARM)
            {
                extern void registerAgainPowerdownAlarmclock( void);
                registerAgainPowerdownAlarmclock();
            }
#endif
#ifdef FEATURE_APP_MUSICPLAYER
            if(!IsMp3PlayerStatusNone())
            {
                ISHELL_PostEvent(pMe->a.m_pIShell,
                                AEECLSID_APP_MUSICPLAYER,
                                EVT_CLOSEAPP,
                                0,
                                0);
            }
#endif
            // 不再关心 IBatt 通知消息
            //(void) ISHELL_RegisterNotify(pMe->a.m_pIShell,
            //            AEECLSID_CORE_APP,
            //            0);

            // Turn off RSSI indicator
            //IANNUNCIATOR_SetField (pMe->m_pIAnn, ANNUN_FIELD_RSSI, ANNUN_STATE_OFF);
           
            // 关闭电池图标
            //IANNUNCIATOR_SetField (pMe->m_pIAnn, ANNUN_FIELD_BATT, ANNUN_STATE_OFF);
            if(pWallPaper)
            {
                IIMAGE_Release(pWallPaper);
                pWallPaper = NULL;
            }
            (void) ISHELL_PostEvent(pMe->a.m_pIShell,
                              AEECLSID_CORE_APP,
                              EVT_USER_REDRAW,
                              0,
                              0);
            
            return TRUE;
            
        case EVT_USER_REDRAW:
 
            // 提示文字相关代码
            {
                AECHAR    wszMsgText[128];
                wszMsgText[0] = 0;
                switch (pMe->m_ePowerDownType)
                {                         
                    case POWERDOWN_NORMAL:
                        if(pMe->m_wStartupAniTime == 0)
                        {  
                            // 在此播放关机音乐
                            uint16    aRing_type[PROFILENUMBER]  = {0} ;                                          // CFGI_PROFILE_STARTUP_MUSIC
                            byte      Ring_Cur_Music                      =  OEMNV_PROFILE_NORMALMODE;
                            byte      poweronoff_alert[PROFILENUMBER] =  {0};			
                            //在关机前关掉FM声音
                            ISHELL_SendEvent(pMe->a.m_pIShell, AEECLSID_APP_FMRADIO,EVT_USER,EVT_UI_EXIT,0L);
                            ICONFIG_GetItem( pMe->m_pConfig, CFGI_PROFILE_CUR_NUMBER, &Ring_Cur_Music, sizeof(Ring_Cur_Music) ); 
                            ICONFIG_GetItem(pMe->m_pConfig,CFGI_PROFILE_POWERONOFF_ALERT,poweronoff_alert,sizeof(poweronoff_alert));
                            if(poweronoff_alert[Ring_Cur_Music]  == OEMNV_POWERONOFF_ENABLE)
                            {
                                ICONFIG_GetItem( pMe->m_pConfig, CFGI_PROFILE_SHUTDOWN_MUSIC, aRing_type, sizeof(aRing_type) );
                                IALERT_StartRingerAlert_Ex( pMe->m_pAlert, (uint32)aRing_type[Ring_Cur_Music] );
                            }
                            
#ifndef FEATURE_USES_LOWMEM
                            pMe->m_pStartupAniImg = ISHELL_LoadImage( pMe->a.m_pIShell, PWROFF_ANI_FILE);
                            if ( NULL != pMe->m_pStartupAniImg )
#endif
                            {     
                                CoreApp_PlayPwrOffAni(pMe);
                            }
                        }
                        else
                        {
#ifndef FEATURE_USES_LOWMEM
                            if ( NULL != pMe->m_pStartupAniImg )
                            {     
                                IIMAGE_Stop(pMe->m_pStartupAniImg);
                                IIMAGE_Release(pMe->m_pStartupAniImg);
                                pMe->m_pStartupAniImg = NULL;
                            }
#endif
                            (void)ISHELL_PostEvent( pMe->a.m_pIShell,  AEECLSID_CORE_APP, EVT_DISPLAYDIALOGTIMEOUT,  0, 0);
                        }
                    
                        return TRUE;
                        
#ifdef FEATURE_UIM
                    case POWERDOWN_RUIM_DOOR_RESET:
#endif  /* FEATURE_UIM */     
                    case POWERDOWN_RESET:
                        // 取"电话正在" 字符串
                        (void) ISHELL_LoadResString(pMe->a.m_pIShell,
                                AEE_COREAPPRES_LANGFILE,
                                IDS_RESET_2,
                                wszMsgText,
                                sizeof(wszMsgText));
                        break;
                        
                    default:
                        break;
                }
                if(POWERDOWN_NORMAL != pMe->m_ePowerDownType)
                {
                    (void) IDISPLAY_DrawText(pMe->m_pDisplay,
                                    AEE_FONT_NORMAL,
                                    wszMsgText,  -1,  0,  0,  &pMe->m_rc,
                                    IDF_ALIGN_CENTER | IDF_ALIGN_MIDDLE);
                    // 设置自动关闭对话框的定时器
                    (void)ISHELL_SetTimer(pMe->a.m_pIShell, 
                                          3000,
                                          DialogTimeoutCallback,
                                          pMe);
                    
                }
                // 立即更新屏幕                        
                //IDISPLAY_UpdateEx(pMe->m_pDisplay, FALSE);
            }            

            // 设置自动关闭对话框的定时器
//            (void)ISHELL_SetTimer(pMe->a.m_pIShell, 
//                                  RUIM_DOOR_OPEN_TIMER,
//                                  pMe);
            return TRUE;
                    
        case EVT_DIALOG_END:
//            (void) ISHELL_CancelTimer(pMe->a.m_pIShell,
//                                      pMe);
            //IALERT_StopRingerAlert(pMe->m_pAlert);
            return TRUE;

        case EVT_DISPLAYDIALOGTIMEOUT:
        {
            IALERT_StopRingerAlert(pMe->m_pAlert);
            DBGPRINTF("EVT_DISPLAYDIALOGTIMEOUT");
            CoreApp_Poweroff_Phone(pMe);
            CLOSE_DIALOG(DLGRET_OK);
            return TRUE;
        }
          
        default:
            break;
    }
    
    return FALSE;
} // IDD_POWERDOWN_Handler

/*==============================================================================
函数:
    CoreApp_UpdateIdleTimer

说明:
    用于每分钟更新日期时间显示的定时器函数。

参数:
    pUser [in]:这里必须是指向 Core Applet 对象结构的指针。

返回值:
    none

备注:
       
==============================================================================*/
static void CoreApp_UpdateIdleTimer(void *pUser)
{
    CCoreApp    *pMe = (CCoreApp *)pUser;
    uint32         dwSeconds;
	JulianType  julian;
	
    if (pMe == NULL) 
    {
        return;
    }
    
    // 更新日期和时间显示
    ISHELL_PostEvent(pMe->a.m_pIShell, 
                     AEECLSID_CORE_APP,
                     EVT_UPDATEIDLE,
                     0,0L);
    
    // 计算下次定时器定时时间，时间尽可能靠近下一分钟，做到显示时间精确
    dwSeconds = GETTIMESECONDS();
    GetJulianDate(GETTIMESECONDS(), &julian);
    pm_rtc_rw_cmd (PM_RTC_SET_CMD, (pm_rtc_julian_type*)(&julian));
    if (FALSE == pMe->m_bAcquiredTime) 
    {
        // 20x365x24x60x60 = 630720000
        if (dwSeconds > 630720000) 
        {
            pMe->m_bAcquiredTime = TRUE;
        }
        else
        {
            // 重设分钟定时器, 系统没获到有效时间时，间隔 5 秒
            (void) ISHELL_SetTimer(pMe->a.m_pIShell,
                    (int32)(5 * 1000),
                    CoreApp_UpdateIdleTimer,
                    pMe);
                    
            return;
        }
    }
    
    if (dwSeconds > 0) 
    {
        dwSeconds %= 60;
        dwSeconds = (60 - dwSeconds);
    }
     
    if (dwSeconds == 0) 
    {
        dwSeconds = 60;
    }
    
    // 重设分钟定时器
    (void) ISHELL_SetTimer(pMe->a.m_pIShell,
                           (int32)(dwSeconds * 1000),
                           CoreApp_UpdateIdleTimer,
                           pMe);
}

 /*==============================================================================
函数:
    CoreApp_TimeKeyguard

说明:
    用于 定时锁键盘的函数

参数:
    pUser [in]:这里必须是指向 Core Applet 对象结构的指针。

返回值:
    none

备注:
       
==============================================================================*/
#ifdef FEATURE_KEYGUARD
static void CoreApp_TimeKeyguard(void *pUser)
{
    CCoreApp    *pMe = (CCoreApp *)pUser;
    pMe->m_b_set_lock = TRUE;
	
    if(pMe->m_b_set_lock)
    {    	
        OEMKeyguard_SetState(TRUE);		
        CoreApp_UpdateBottomBar(pMe);		
        //IDISPLAY_UpdateEx(pMe->m_pDisplay, FALSE);
        pMe->m_b_set_lock = FALSE;
    }
}
#endif
/*==============================================================================
函数:
    CoreApp_SearchingTimer

说明:
    用于每分钟更新日期时间显示的定时器函数。

参数:
    pUser [in]:这里必须是指向 Core Applet 对象结构的指针。

返回值:
    none

备注:
       
==============================================================================*/
static void CoreApp_SearchingTimer(void *pUser)
{
    CCoreApp  *pMe = (CCoreApp *)pUser;

    if (( pMe == NULL ) || (IDD_IDLE != pMe->m_wActiveDlgID))
    {
        return;
    }

    ISHELL_PostEvent(pMe->a.m_pIShell, 
                     AEECLSID_CORE_APP,
                     EVT_USER_REDRAW,
                     0,0L);

    (void)ISHELL_SetTimer(pMe->a.m_pIShell,
                          IDLESEARCHINTIME,
                          CoreApp_SearchingTimer,
                          pMe);
}

/*==============================================================================
函数:
    CoreApp_DrawBannerMessage

说明:
    函数用于SPN、待机问候语栏的文本绘制处理。

参数:
    pMe [In]: 指向 Core Applet 对象结构的指针。
       
返回值:
    无。

备注:
    刷新显示操作由调用者根据需要执行相关操作。
    
==============================================================================*/
static void CoreApp_DrawBannerMessage(void    *pUser)
{
    AEERect   rc;
    AECHAR    wszBuf[UIM_CDMA_HOME_SERVICE_SIZE+1] = {0};
    int32     nSize = sizeof(wszBuf);
    //AEECMSSInfo  *pssinfo = NULL;
    boolean   bSetsearchingTimer = FALSE;
#ifdef FEATURE_OEMOMH    
    boolean   hasGetSPN = FALSE;
    STATIC uint8 step = RPLMN_X;
    //int str_lenth = 0;
#endif    

	int str_lenth = 0;
	int str2_lenth = 0;

    CCoreApp  *pMe = (CCoreApp *)pUser;
    nv_item_type	SimChoice;
    AECHAR wszBufncard1[UIM_CDMA_HOME_SERVICE_SIZE+1] = {0};
    AECHAR wszBufncard2[UIM_CDMA_HOME_SERVICE_SIZE+1] = {0};
    
    MSG_FATAL("CoreApp_DrawBannerMessage Start",0,0,0);
	OEMNV_Get(NV_SIM_SELECT_I,&SimChoice);

		(void) ISHELL_LoadResString(pMe->a.m_pIShell,
                                        AEE_COREAPPRES_LANGFILE,
                                        IDS_NO_USIM_CARDTWO,
                                        wszBufncard2,
                                        sizeof(wszBufncard2));
	
		(void) ISHELL_LoadResString(pMe->a.m_pIShell,
                                        AEE_COREAPPRES_LANGFILE,
                                        IDS_NO_USIM_CARDONE,
                                        wszBufncard1,
                                        sizeof(wszBufncard1));

    
    // 先取消相关定时器
    (void) ISHELL_CancelTimer(pMe->a.m_pIShell,
                              CoreApp_SearchingTimer,
                              pMe);

    // Determine displaying rectangle     
	SETAEERECT(&rc, RPLMN_X, RPLMN_Y, pMe->m_rc.dx-2*RPLMN_X, pMe->m_nLargeFontHeight);
    
#ifdef FEATURE_PLANEMODE    
    if(pMe->m_SYS_MODE_NO_SRV || pMe->bPlaneModeOn == TRUE)
#else
	if(pMe->m_SYS_MODE_NO_SRV)
#endif
    {// 搜网提示优先
#ifdef FEATURE_PLANEMODE 
        byte planeMode_cfg;
        (void) ICONFIG_GetItem(pMe->m_pConfig,
                               CFGI_PLANEMODE,
                               &planeMode_cfg,
                               sizeof(planeMode_cfg));
        //OEMNV_PLANEMODE_QUERY == bytData 维持当前bPlaneModeOn的值
        if(OEMNV_PLANEMODE_OFF== planeMode_cfg)
        {
            pMe->bPlaneModeOn = FALSE;
        }
        else if(OEMNV_PLANEMODE_ON == planeMode_cfg)
        {
            pMe->bPlaneModeOn = TRUE;
        }
        
        if(TRUE == pMe->bPlaneModeOn)
        {
            (void) ISHELL_LoadResString(pMe->a.m_pIShell,
                                        AEE_COREAPPRES_LANGFILE,
                                        IDS_PLANEMODE,
                                        wszBuf,
                                        nSize);
        }
        else
#endif //FEATURE_PLANEMODE            
        {
            // 搜网提示优先
            bSetsearchingTimer = TRUE;
            (void) ISHELL_LoadResString(pMe->a.m_pIShell,
                                        AEE_COREAPPRES_LANGFILE,
                                        IDS_SEARCHING,
                                        wszBuf,
                                        nSize);
        }
    }
    else if(pMe->m_bemergencymode)
    {
        ISHELL_LoadResString(pMe->a.m_pIShell,
                                    AEE_COREAPPRES_LANGFILE,
                                    IDS_EMERGENCY_MODE,
                                    wszBuf,
                                    nSize);
    }
    else if (IsRunAsFactoryTestMode())
    {
            bSetsearchingTimer = TRUE;
            if (IRUIM_IsCardConnected(pMe->m_pIRUIM))
            {
                (void) ISHELL_LoadResString(pMe->a.m_pIShell,
                                            AEE_COREAPPRES_LANGFILE,
                                            IDS_FACTORYTESTMODE,
                                            wszBuf,
                                            nSize);
            }
            else
            {
#ifdef FEATURE_VERSION_W208S                     	
                //Do nothing  				
#else            
        		(void) ISHELL_LoadResString(pMe->a.m_pIShell,
                                            AEE_COREAPPRES_LANGFILE,
                                            IDS_NORUIM,
                                            wszBuf,
                                            nSize);
#endif
            }
    }
    else
    {// 最后是正常情况下的提示
        // 获取待机问候语 
        CoreApp_GetSPN(pMe);
        
        if(pMe->svc_p_name[0] != 0)
        {
#ifdef FEATURE_OEMOMH 
            MSG_FATAL("CoreApp_DrawBannerMessage hasGetSPN = TRUE",0,0,0);
            DBGPRINTF("svc_p_name s=%S", pMe->svc_p_name);
            hasGetSPN = TRUE;
#endif
            WSTRCPY(wszBuf,pMe->svc_p_name);
        }
        else
        {
            bSetsearchingTimer = TRUE;
#ifdef WIN32
            STRTOWSTR("WIN32BUILD", wszBuf, nSize);
#else
#ifdef FEATURE_VERSION_W208S
			//Do Nothing
#else
            STRTOWSTR(ver_modelname, wszBuf, nSize);
#endif
#endif
        }
    }
#ifdef FEATURE_DISP_128X128    
    {
        str_lenth = IDISPLAY_MeasureText(pMe->m_pDisplay, AEE_FONT_NORMAL, (const AECHAR *)wszBuf);
#ifdef FEATURE_OEMOMH    
        if(hasGetSPN && (str_lenth > 128))
        {
            rc.x = step - 10;
            step -= 10;
        }
        else
#endif        
        rc.x = (128 - str_lenth)/2;
        rc.dx = str_lenth;
        (void)DrawTextWithProfile(pMe->a.m_pIShell,
                                  pMe->m_pDisplay,
                                  RGB_WHITE_NO_TRANS,
                                  AEE_FONT_NORMAL,
                                  wszBuf, -1,
                                  0, 0, &rc, 
                                  IDF_ALIGN_CENTER
                                  | IDF_ALIGN_MIDDLE 
                                  | IDF_TEXT_TRANSPARENT);        
        MSG_FATAL("str_lenth=%d, rc.x=%d", str_lenth, rc.x, 0);
    }
#elif defined (FEATURE_DISP_176X220) 
    {
        str_lenth = IDISPLAY_MeasureText(pMe->m_pDisplay, AEE_FONT_NORMAL, (const AECHAR *)wszBuf);
#ifdef FEATURE_OEMOMH    
        if(hasGetSPN && (str_lenth > 176))
        {
            rc.x = step - 10;
            step -= 10;
        }
        else
#endif          
        rc.x = (176 - str_lenth)/2;
        rc.dx = str_lenth;
        (void)DrawTextWithProfile(pMe->a.m_pIShell,
                                  pMe->m_pDisplay,
                                  RGB_WHITE_NO_TRANS,
                                  AEE_FONT_NORMAL,
                                  wszBuf, -1,
                                  0, 0, &rc, 
                                  IDF_ALIGN_CENTER
                                  | IDF_ALIGN_MIDDLE 
                                  | IDF_TEXT_TRANSPARENT);        
        MSG_FATAL("str_lenth=%d, rc.x=%d", str_lenth, rc.x, 0);
     }
#elif defined (FEATURE_DISP_240X320) 
	#ifdef FEATURE_LCD_TOUCH_ENABLE
	str_lenth = IDISPLAY_MeasureText(pMe->m_pDisplay, AEE_FONT_NORMAL, (const AECHAR *)wszBuf);
#ifdef FEATURE_OEMOMH    
        if(hasGetSPN && (str_lenth > 176))
        {
            rc.x = step - 10;
            step -= 10;
        }
        else
#endif     
		rc.y = 135;
        rc.x = 15;
        rc.dx = str_lenth;
        (void)DrawTextWithProfile(pMe->a.m_pIShell,
                                  pMe->m_pDisplay,
                                  RGB_WHITE_NO_TRANS,
                                  AEE_FONT_NORMAL,
                                  wszBuf, -1,
                                  0, 0, &rc, 
                                  IDF_ALIGN_LEFT
                                  | IDF_ALIGN_MIDDLE
                                  | IDF_TEXT_TRANSPARENT);       
	#endif
#else
    {
        // Display the string
        str_lenth = IDISPLAY_MeasureText(pMe->m_pDisplay, AEE_FONT_NORMAL, (const AECHAR *)wszBuf);
        //str2_lenth = IDISPLAY_MeasureText(pMe->m_pDisplay, AEE_FONT_NORMAL, (const AECHAR *)wszBufncard);
#ifdef FEATURE_OEMOMH    
        if(hasGetSPN && (str_lenth > SCREEN_WIDTH))
        {
            AECHAR    wszBuf1[16] = {0};
            AECHAR    wszBuf2[31] = {0}; 
            uint8 i,j;
            uint8 BufCount = WSTRLEN(wszBuf);
            uint8 Buf1Len = 0;
            uint8 Buf2Len = 0;
            for(i = 0; i < 11; ++i)
            {
                wszBuf1[i] = wszBuf[i];
            }
            wszBuf1[i++] = L'\0';
            i--;
            Buf1Len = IDISPLAY_MeasureText(pMe->m_pDisplay, AEE_FONT_NORMAL, (const AECHAR *)wszBuf1);
            for(j=0; i < BufCount; ++i,++j)
            {
                wszBuf2[j] = wszBuf[i];
            }
            wszBuf2[j] = L'\0';
            Buf2Len = IDISPLAY_MeasureText(pMe->m_pDisplay, AEE_FONT_NORMAL, (const AECHAR *)wszBuf2);

            rc.dx = Buf2Len; 
            (void)DrawTextWithProfile(pMe->a.m_pIShell,
                                      pMe->m_pDisplay,
                                      RGB_WHITE_NO_TRANS,
                                      AEE_FONT_NORMAL,
                                      wszBuf2, -1,
                                      0, 0, &rc, 
                          #ifdef FEATURE_OEMOMH
                                      IDF_ALIGN_LEFT 
                          #elif defined(FEATURE_VERSION_MYANMAR)
                                      IDF_ALIGN_CENTER
                          #else
                                      IDF_ALIGN_RIGHT 
                          #endif
                                      | IDF_ALIGN_MIDDLE 
                                      | IDF_TEXT_TRANSPARENT);    
                          
            rc.x = SCREEN_WIDTH - Buf1Len - 2;
            rc.dx = Buf1Len; 
            rc.y = IDLE_D_CLOCK_Y;
            (void)DrawTextWithProfile(pMe->a.m_pIShell,
                                      pMe->m_pDisplay,
                                      RGB_WHITE_NO_TRANS,
                                      AEE_FONT_NORMAL,
                                      wszBuf1, -1,
                                      0, 0, &rc, 
                          #ifdef FEATURE_OEMOMH
                                      IDF_ALIGN_LEFT 
                          #elif defined(FEATURE_VERSION_MYANMAR)
                                      IDF_ALIGN_CENTER
                          #else
                                      IDF_ALIGN_RIGHT 
                          #endif
                                      | IDF_ALIGN_MIDDLE 
                                      | IDF_TEXT_TRANSPARENT);

        }
        else
#endif      
		#ifdef FEATURE_VERSION_C01
		#ifdef FEATURE_DOUBLE_SIM_CARD
		if(SimChoice.sim_select == AVK_SEND_TWO)
		{
			 AEERect  Temprc = rc;
			 Temprc.y = Temprc.y - 16;
			(void)DrawTextWithProfile(pMe->a.m_pIShell,
                                  pMe->m_pDisplay,
                                  RGB_WHITE_NO_TRANS,
                                  AEE_FONT_NORMAL,
                                  wszBufncard1, -1,
                                  0, 0, &Temprc, 
                                  #ifdef FEATURE_OEMOMH
                                  IDF_ALIGN_LEFT 
                                  #elif defined(FEATURE_VERSION_MYANMAR)
                                  IDF_ALIGN_CENTER
        						  #else
                                  IDF_ALIGN_RIGHT 
                                  #endif
                                  | IDF_ALIGN_MIDDLE 
                                  | IDF_TEXT_TRANSPARENT);
		}
		else
		{
			(void)DrawTextWithProfile(pMe->a.m_pIShell,
                                  pMe->m_pDisplay,
                                  RGB_WHITE_NO_TRANS,
                                  AEE_FONT_NORMAL,
                                  wszBufncard2, -1,
                                  0, 0, &rc, 
                                  #ifdef FEATURE_OEMOMH
                                  IDF_ALIGN_LEFT 
                                  #elif defined(FEATURE_VERSION_MYANMAR)
                                  IDF_ALIGN_CENTER
        						  #else
                                  IDF_ALIGN_RIGHT 
                                  #endif
                                  | IDF_ALIGN_MIDDLE 
                                  | IDF_TEXT_TRANSPARENT);
			rc.y = rc.y - 16;
		}
		#endif
		#endif
        (void)DrawTextWithProfile(pMe->a.m_pIShell,
                                  pMe->m_pDisplay,
                                  RGB_WHITE_NO_TRANS,
                                  AEE_FONT_NORMAL,
                                  wszBuf, -1,
                                  0, 0, &rc, 
                                  #ifdef FEATURE_OEMOMH
                                  IDF_ALIGN_LEFT 
                                  #elif defined(FEATURE_VERSION_MYANMAR)
                                  IDF_ALIGN_CENTER
        						  #else
                                  IDF_ALIGN_RIGHT 
                                  #endif
                                  | IDF_ALIGN_MIDDLE 
                                  | IDF_TEXT_TRANSPARENT);
    }
 #endif   
    if (bSetsearchingTimer)
    {
        (void)ISHELL_SetTimer(pMe->a.m_pIShell,
                              IDLESEARCHINTIME,
                              CoreApp_SearchingTimer,
                              pMe);
    }
   
    MSG_FATAL("CoreApp_DrawBannerMessage End",0,0,0);
}

/*==============================================================================
函数:
    CoreApp_UpdateDateTime

说明:
    用于更新日期时间和星期几的显示。

参数:
    pMe [In]: 指向 Core Applet 对象结构的指针。

返回值:
    无。

备注:
    刷新显示操作由调用者根据需要执行相关操作。
==============================================================================*/
static void CoreApp_UpdateDateTime(CCoreApp    *pMe)
{
    AECHAR      wszDate[64]= {0};
    AECHAR      wFormat[64]= {0};
    AECHAR      wszDatemat[4] = {0};
    JulianType  jDate;
    AEERect     rc;
    AEERect     rc_date;
    AEERect     rc_week;
    //AECHAR      wszBuf[16]= {(AECHAR)'\0'}; //fj remark ,Define but no used
#ifndef FEATURE_CARRIER_SUDAN_SUDATEL
    uint16   nWeekResID;
    //AEEDeviceInfo di;
#endif /*FEATURE_CARRIER_SUDAN_SUDATEL*/
    byte      bTFmt;
//#ifdef FEATURE_VERSION_VG68
    int nFont = 33;//VG68默认时间字体大小
//#endif

    MSG_FATAL("CoreApp_UpdateDateTime.......................",0,0,0);
    
    if ( pMe == NULL ) 
    {
        return;
    }
    
    // 符合下列条件之一，函数立即返回
    if(pMe->m_bSuspended)
    {
        return;
    }

    CoreApp_Process_Rtc_Event(pMe);
    (void) ICONFIG_GetItem( pMe->m_pConfig,
                           CFGI_TIME_FORMAT,
                           &bTFmt,
                           sizeof(bTFmt));

    // 取当前日期
    if(pMe->m_bAcquiredTime)
    {
        GetJulianDate(GETTIMESECONDS(), &jDate);
    }
    else
    {

#if (defined(FEATURE_PM1000_RTC) || defined(FEATURE_PMIC_RTC))
        pm_rtc_julian_type time = {0};
        pm_rtc_rw_cmd(PM_RTC_GET_CMD, &time);
        jDate.wSecond = (uint16)time.second&0xFFFF;
        jDate.wMinute = (uint16)time.minute&0xFFFF;
        jDate.wHour = (uint16)time.hour&0xFFFF;
        jDate.wDay = (uint16)time.day&0xFFF;
        jDate.wMonth = (uint16)time.month&0xFFFF;
        jDate.wYear = (uint16)time.year&0xFFFF;
        jDate.wWeekDay = (uint16)time.day_of_week&0xFFFF;
#else
#ifdef FEATURE_VERSION_W208S
		jDate.wYear = 2012;
        jDate.wMonth = 1;
        jDate.wDay =1;
        jDate.wHour = 0;
        jDate.wMinute =0;
        jDate.wSecond = 0;
        jDate.wWeekDay = 7;
#else
        jDate.wYear = 1980;
        jDate.wMonth = 1;
        jDate.wDay =6;
        jDate.wHour = 0;
        jDate.wMinute =0;
        jDate.wSecond = 0;
        jDate.wWeekDay = 6;
#endif		
#endif
    }
#ifndef FEATURE_CARRIER_SUDAN_SUDATEL
    // 确定星期所用资源字符串
    switch (jDate.wWeekDay)
    {
        case 0: // 星期一
            nWeekResID = AEE_IDS_MONDAY;
            break;
            
        case 1: // 星期二
            nWeekResID = AEE_IDS_TUESDAY;
            break;

        case 2: // 星期三
            nWeekResID = AEE_IDS_WEDNESDAY;
            break;

        case 3: // 星期四
            nWeekResID = AEE_IDS_THURSDAY;
            break;

        case 4: // 星期五
            nWeekResID = AEE_IDS_FRIDAY;
            break;

        case 5: // 星期六
            nWeekResID = AEE_IDS_SATURDAY;
            break;

        default:  // 6:星期日
            nWeekResID = AEE_IDS_SUNDAY;
            break;
    }
#endif /*FEATURE_CARRIER_SUDAN_SUDATEL*/

#ifdef FEATRUE_SET_ANN_FULL_SCREEN
	SETAEERECT(&rc, 
			   IDLE_D_CLOCK_X,
			   IDLE_D_CLOCK_Y,
			   pMe->m_rc.dx-2*IDLE_D_CLOCK_X, 
			   pMe->m_nLargeFontHeight);	//26
	SETAEERECT(&rc_date, 
			   DATA_X,
			   DATA_Y,
			   (pMe->m_rc.dx-2*DATA_X), 
			   pMe->m_nNormalFontHeight);
#if defined(FEATURE_DISP_160X128)
	SETAEERECT(&rc_week, 
		   WEEK_X,
		   WEEK_Y,
		   (pMe->m_rc.dx-2*WEEK_X), 
		   pMe->m_nNormalFontHeight);
#elif defined(FEATURE_DISP_220X176)
	SETAEERECT(&rc_week, 
		   WEEK_X,
		   WEEK_Y,
		   (pMe->m_rc.dx-2*WEEK_X), 
		   pMe->m_nNormalFontHeight);

#elif defined (FEATURE_DISP_128X160)
	SETAEERECT(&rc_week, 
		   WEEK_X,
		   WEEK_Y,
		   (pMe->m_rc.dx-2*WEEK_X), 
		   pMe->m_nNormalFontHeight);
#elif defined (FEATURE_DISP_176X220)
	SETAEERECT(&rc_week, 
		   WEEK_X,
		   WEEK_Y,
		   (pMe->m_rc.dx-2*WEEK_X), 
		   pMe->m_nNormalFontHeight);
#elif defined (FEATURE_DISP_240X320)
	SETAEERECT(&rc_week, 
		   WEEK_X,
		   WEEK_Y,
		   (pMe->m_rc.dx-2*WEEK_X), 
		   pMe->m_nNormalFontHeight);
#elif defined (FEATURE_DISP_320X240)
	SETAEERECT(&rc_week, 
		   WEEK_X,
		   WEEK_Y,
		   (pMe->m_rc.dx-2*WEEK_X), 
		   pMe->m_nNormalFontHeight);
	   
#endif

#else
    SETAEERECT(&rc, 
               1,
               1, 
               pMe->m_rc.dx-2, 
               pMe->m_nNormalFontHeight);
    SETAEERECT(&rc_date, 
               (pMe->m_rc.dx-10)/2 ,
               1, 
               pMe->m_rc.dx-(pMe->m_rc.dx-10)/2, 
               pMe->m_nNormalFontHeight);

	SETAEERECT(&rc_week, 
		   1,
		   1,
		   (pMe->m_rc.dx-2*DATA_X), 
               pMe->m_nNormalFontHeight);
#endif
    // 格式化时间字符串并绘制
    wFormat[0] = 0;
    wszDate[0] = 0;
    
    (void) ISHELL_LoadResString(pMe->a.m_pIShell,
                                AEE_COREAPPRES_LANGFILE,
                                IDS_TIME_TILE,
                                wFormat,
                                sizeof(wFormat));
    if (bTFmt == OEMNV_TIMEFORM_AMPM)
    {        
        uint16    wHour,len;
                                     
        wHour = jDate.wHour > 12 ? (jDate.wHour - 12) : jDate.wHour;
        if(jDate.wHour == 0)
        {
            wHour = 12;
        }
        WSPRINTF(wszDate, sizeof(wszDate), wFormat, wHour, jDate.wMinute);
        len = WSTRLEN(wszDate);
        if(jDate.wHour >= 12)
        {
        	   wszDatemat[0] = (AECHAR)'P';
        	   wszDatemat[1] = (AECHAR)'M';
        	   wszDatemat[2] = 0;
            wszDate[len] = (AECHAR)'P';
            wszDate[len+1] = (AECHAR)'M';
            wszDate[len+2] = 0;
        }
        else
        {
        	   wszDatemat[0] = (AECHAR)'A';
        	   wszDatemat[1] = (AECHAR)'M';
        	   wszDatemat[2] = 0;
            wszDate[len] = (AECHAR)'A';
            wszDate[len+1] = (AECHAR)'M';
            wszDate[len+2] = 0;
        }
    }
    else
    {
        WSPRINTF(wszDate, sizeof(wszDate), wFormat, jDate.wHour, jDate.wMinute);
    }
	//IDISPLAY_UpdateEx(pMe->m_pDisplay,FALSE);//wlh test               
    // Display the string of date or time or weekday
	#if 1
	{
#ifdef FEATURE_DISP_128X128    
    {
       // int str_lenth = IDISPLAY_MeasureText(pMe->m_pDisplay, AEE_FONT_NORMAL, (const AECHAR *)wszDate);
       // rc.x = (128 - str_lenth)/2;
       // rc.dx = str_lenth;
		DrawGreyBitTextWithProfile(pMe->a.m_pIShell,
	                              pMe->m_pDisplay,
	                              RGB_WHITE_NO_TRANS,
	                              18,
	                              wszDate, -1,
	                              0, 0, &rc, 
	                              IDF_ALIGN_MIDDLE
	                              | IDF_ALIGN_CENTER
	                              | IDF_TEXT_TRANSPARENT); 
    }
#elif defined FEATURE_DISP_220X176
	DrawGreyBitTextWithProfile(pMe->a.m_pIShell,
	                              pMe->m_pDisplay,
	                              RGB_WHITE_NO_TRANS,
	                              25, 
	                              wszDate, -1,
	                              0, 0, &rc, 
	                              IDF_ALIGN_MIDDLE
	                              | IDF_ALIGN_LEFT
	                              | IDF_TEXT_TRANSPARENT);
#elif defined FEATURE_DISP_240X320
{
	#ifdef FEATURE_LCD_TOUCH_ENABLE
	{
		 uint16    wHour,len;
         uint16    Ten = 0;
		 uint16    Num = 0;
		 Num = jDate.wMinute%10;
		 Ten = jDate.wMinute/10;
		 MSG_FATAL("Num========%d,Ten========%d",Num,Ten,0);
		 if (bTFmt == OEMNV_TIMEFORM_AMPM)
    	 {
         	wHour = jDate.wHour > 12 ? (jDate.wHour - 12) : jDate.wHour;
			if(jDate.wHour >= 12)
        	{
        	   wszDatemat[0] = (AECHAR)'P';
        	   wszDatemat[1] = (AECHAR)'M';
        	   wszDatemat[2] = 0;
        	}
        	else
        	{
        	   wszDatemat[0] = (AECHAR)'A';
        	   wszDatemat[1] = (AECHAR)'M';
        	   wszDatemat[2] = 0;
        	}
		}
		else
		{
		 	wHour = jDate.wHour;
		}
		 IIMAGE_Draw(pMe->m_pImageTimeIcon[11],
	                    0, 
	                    25);
		switch(wHour)
		{
			case 0:
			case 1:
			case 2:
			case 3:
			case 4:
			case 5:
			case 6:
			case 7:
			case 8:
			case 9:
			case 10:
			case 11:
			case 12:
			case 13:
			case 14:
			case 15:
			case 16:
			case 17:
			case 18:
			case 19:
			case 20:
			case 21:
			case 22:
			case 23:
			{
				int Hour_Ten = wHour/10;
				int Hour_Num = wHour%10;
				IIMAGE_Draw(pMe->m_pImageTimeIcon[Hour_Ten],
								IDLE_TIME_HORE_X,
								IDLE_TIME_Y
								);
				IIMAGE_Draw(pMe->m_pImageTimeIcon[Hour_Num],
								IDLE_TIME_HORE_X+40,
								IDLE_TIME_Y
								);
			}
			break;
			default:
			break;
		}

		IIMAGE_Draw(pMe->m_pImageTimeIcon[Ten],
								IDLE_TIME_MIN_X,
								IDLE_TIME_Y
								);
		IIMAGE_Draw(pMe->m_pImageTimeIcon[Num],
								IDLE_TIME_MIN_X+40,
								IDLE_TIME_Y
								);
		if((jDate.wHour >= 6)&&(jDate.wHour <= 18))
        {
        	IIMAGE_Draw(pMe->m_pImageTimeIcon[10],
								95,
								130
								);
		}
		else
		{
			IIMAGE_Draw(pMe->m_pImageTimeIcon[12],
								95,
								130
								);
		}
	
	}
	#endif
}
#elif defined FEATURE_DISP_320X240
	DrawGreyBitTextWithProfile(pMe->a.m_pIShell,
	                              pMe->m_pDisplay,
	                              RGB_WHITE_NO_TRANS,
	                              30, 
	                              wszDate, -1,
	                              0, 0, &rc, 
	                              IDF_ALIGN_MIDDLE
	                              | IDF_ALIGN_LEFT
	                              | IDF_TEXT_TRANSPARENT);

#elif defined FEATURE_DISP_176X220
#ifdef FEATURE_VERSION_VG68
    {
        byte Timefontmode = 0;
        (void) ICONFIG_GetItem(pMe->m_pConfig,
                               CFGI_IDLE_DATETIME_MODE,
                               &Timefontmode,
                               sizeof(Timefontmode));
        switch(Timefontmode)
        {
            default:
            case 1:
                nFont = 33;
                break;
            case 2:
                nFont = 27;
                break;
            case 3:
                nFont = 21;
                break;
            
        }
        
    }
#endif
    MSG_FATAL("******pji*****rc.y=%d,rc.dy=%d",rc.y,rc.dy,0);
	DrawGreyBitTextWithProfile(pMe->a.m_pIShell,
	                              pMe->m_pDisplay,
	                              RGB_WHITE_NO_TRANS,
	                              nFont, 
	                              wszDate, -1,
	                              0, 0, &rc, 
	                              IDF_ALIGN_MIDDLE
	                              | IDF_ALIGN_CENTER
	                              | IDF_TEXT_TRANSPARENT);


#else	
		//rc.y = 20;
		#ifdef FEATURE_VERSION_MYANMAR
		{
			int         nLineWidth = 4, nNumberWidth = 20, nNumberHeight = 40, nOffset = 5,
	                xStartPos = 0, yStartPos = 0, nTextLen = 0;
	        AEERect rect = {0};
	        uint16    wHour,len; 
	        byte Timefontmode = 0;
	        boolean bMode = TRUE;
	        yStartPos = (SCREEN_HEIGHT*2/5)+10;
			// draw hour
			(void) ICONFIG_GetItem(pMe->m_pConfig,
                                       CFGI_IDLE_DATETIME_MODE,
                                       &Timefontmode,
                                       sizeof(Timefontmode));
            switch(Timefontmode)
            {
            	case 1:
            		nLineWidth = 4;
            		nNumberWidth = 20;
            		nNumberHeight = 40;
            		nOffset = 5;
            		if (bTFmt != OEMNV_TIMEFORM_AMPM)
					{
		       			wHour = jDate.wHour;
						xStartPos = 10;
					}
					else
					{
						xStartPos = 5;
						wHour = jDate.wHour > 12 ? (jDate.wHour - 12) : jDate.wHour;
		       			if(jDate.wHour == 0)
		        			{
		            			wHour = 12;
		       			}
					}
            		break;
            	case 2:
            		xStartPos = 50;
            		yStartPos = (SCREEN_HEIGHT*2/5)+20;
            		nLineWidth = 4;
            		nNumberWidth = 15;
            		nNumberHeight = 30;
            		nOffset = 5;
            		if (bTFmt != OEMNV_TIMEFORM_AMPM)
					{
		       			wHour = jDate.wHour;
						xStartPos = 25;
					}
					else
					{
						xStartPos = 20;
						wHour = jDate.wHour > 12 ? (jDate.wHour - 12) : jDate.wHour;
		       			if(jDate.wHour == 0)
		        			{
		            			wHour = 12;
		       			}
					}
            		break;
            	case 3:
            		bMode = FALSE;
            		break;
            	default:
            		break;
            }
            if(bMode)
            {
				
		    	SETAEERECT(&rect, xStartPos, yStartPos, nNumberWidth, nNumberHeight);
		    	Appscommon_DrawDigitalNumber(pMe->m_pDisplay, (wHour/10), nLineWidth, &rect, RGB_WHITE);
		    	rect.x += nNumberWidth + nOffset;
		    	Appscommon_DrawDigitalNumber(pMe->m_pDisplay, (wHour%10), nLineWidth, &rect, RGB_WHITE);

		   		 // draw colon
		    	SETAEERECT(&rect, xStartPos + 2*(nNumberWidth + nOffset), yStartPos + nNumberHeight/2 - nLineWidth, nLineWidth, nLineWidth);
		    	IDISPLAY_FillRect(pMe->m_pDisplay, &rect, RGB_WHITE);
		    	rect.y = yStartPos + nNumberHeight*3/5 +10 - nLineWidth;
		    	IDISPLAY_FillRect(pMe->m_pDisplay, &rect, RGB_WHITE);
		    
		   		// draw minute
		    	SETAEERECT(&rect, xStartPos + 2*(nNumberWidth + nOffset) + nLineWidth + nOffset, yStartPos, nNumberWidth, nNumberHeight);
		    	Appscommon_DrawDigitalNumber(pMe->m_pDisplay, (jDate.wMinute/10), nLineWidth, &rect, RGB_WHITE);
		    	rect.x += nNumberWidth + nOffset;
		    	Appscommon_DrawDigitalNumber(pMe->m_pDisplay, (jDate.wMinute%10), nLineWidth, &rect, RGB_WHITE);
		    	rect.x += nNumberWidth;
		     	rect.y = rect.y +12;
		    	DrawGreyBitTextWithProfile(pMe->a.m_pIShell,
		                              pMe->m_pDisplay,
		                              RGB_WHITE_NO_TRANS,
		                              18, 
		                              wszDatemat, -1,
		                              0, 0, &rect, 
		                              IDF_ALIGN_MIDDLE
		                              | IDF_ALIGN_LEFT
		                              | IDF_TEXT_TRANSPARENT);
	        }
	        else
	        {
	        	rc.y = rc.y+60;
	        	DrawGreyBitTextWithProfile(pMe->a.m_pIShell,
	                              pMe->m_pDisplay,
	                              RGB_WHITE_NO_TRANS,
	                              18, 
	                              wszDate, -1,
	                              0, 0, &rc, 
	                              IDF_ALIGN_MIDDLE
	                              | IDF_ALIGN_CENTER
	                              | IDF_TEXT_TRANSPARENT);
	           rc.y = rc.y-60;
	        }
	    	IDISPLAY_Update(pMe->m_pDisplay);
    	}
		#else
		#if defined(FEATURE_VERSION_C01)
		rc.y = rc.y-14;
		rc.x = rc.x-2;
		#endif
		DrawGreyBitTextWithProfile(pMe->a.m_pIShell,
	                              pMe->m_pDisplay,
	                              RGB_WHITE_NO_TRANS,
	                              18, 
	                              wszDate, -1,
	                              0, 0, &rc, 
	                              IDF_ALIGN_MIDDLE
	                              | IDF_ALIGN_LEFT
	                              | IDF_TEXT_TRANSPARENT);
	    #if defined(FEATURE_VERSION_C01)
	    rc.y = rc.y+14;
	    rc.x = rc.x+2;
		#endif
	    #endif
        //IDISPLAY_UpdateEx(pMe->m_pDisplay, FALSE);
#endif        
	}

	#else
    (void)DrawTextWithProfile(pMe->a.m_pIShell,
                              pMe->m_pDisplay,
                              RGB_WHITE_NO_TRANS,
                              AEE_FONT_BOLD,
                              wszDate, -1,
                              0, 0, &rc, 
                              IDF_ALIGN_MIDDLE
                              | IDF_ALIGN_CENTER
                              | IDF_TEXT_TRANSPARENT);
	#endif
        
 	//IDISPLAY_UpdateEx(pMe->m_pDisplay,FALSE);//wlh test  
    // 格式化日期字符串并绘制
    wszDate[0] = (AECHAR)'\0';
    wFormat[0] = (AECHAR)'\0';
#ifdef FEATURE_TIME_DATA_SETTING
    {
        //uint16 m_year = 0;
        ICONFIG_GetItem( pMe->m_pConfig, CFGI_DATE_FORMAT,&bTFmt, sizeof(bTFmt));

        //m_year = jDate.wYear -2000;
        switch(bTFmt)
        {
            case OEMNV_DATEFORM_DMY:
#if defined(FEATURE_DISP_176X220)
				WSTRLCPY(wFormat,L"%02d/%02d/%04d",63);
#elif defined(FEATURE_DISP_128X128)
                WSTRLCPY(wFormat,L"%02d/%02d/%04d",63);
#elif defined(FEATURE_DISP_160X128)
				WSTRLCPY(wFormat,L"%02d/%02d/%04d",63);
#elif defined(FEATURE_DISP_220X176)
				WSTRLCPY(wFormat,L"%02d/%02d/%04d",63);
#elif defined(FEATURE_DISP_128X160)
				WSTRLCPY(wFormat,L"%02d/%02d/%04d",63);
#elif defined(FEATURE_DISP_240X320)
				WSTRLCPY(wFormat,L"%02d/%02d/%04d",63);	
#elif defined(FEATURE_DISP_320X240)
				WSTRLCPY(wFormat,L"%02d/%02d/%04d",63);	
#else
                WSTRLCPY(wFormat,L"%02d/%02d",63);
#endif //FEATURE_DISP_176X220
                WSPRINTF(wszDate, sizeof(wszDate), wFormat, jDate.wDay, jDate.wMonth, jDate.wYear);
                break;
                
            case OEMNV_DATEFORM_MDY:
#if defined(FEATURE_DISP_176X220)
				WSTRLCPY(wFormat,L"%02d/%02d/%04d",63);
#elif defined(FEATURE_DISP_128X128)
                WSTRLCPY(wFormat,L"%02d/%02d/%04d",63);
#elif defined(FEATURE_DISP_160X128)
				WSTRLCPY(wFormat,L"%02d/%02d/%04d",63);
#elif defined(FEATURE_DISP_220X176)
				WSTRLCPY(wFormat,L"%02d/%02d/%04d",63);
#elif defined(FEATURE_DISP_128X160)
				WSTRLCPY(wFormat,L"%02d/%02d/%04d",63);
#elif defined(FEATURE_DISP_240X320)
				WSTRLCPY(wFormat,L"%02d/%02d/%04d",63);
#elif defined(FEATURE_DISP_320X240)
				WSTRLCPY(wFormat,L"%02d/%02d/%04d",63);
#else
                WSTRLCPY(wFormat,L"%02d/%02d",63);
#endif //FEATURE_DISP_176X220
                WSPRINTF(wszDate, sizeof(wszDate), wFormat, jDate.wMonth, jDate.wDay, jDate.wYear);
                break;
                
            default:
            case OEMNV_DATEFORM_YMD:
#if defined(FEATURE_DISP_176X220)
				WSTRLCPY(wFormat,L"%04d/%02d/%02d",63);
#elif defined(FEATURE_DISP_128X128)
                WSTRLCPY(wFormat,L"%04d/%02d/%02d",63);
#elif defined(FEATURE_DISP_160X128)
				WSTRLCPY(wFormat,L"%04d/%02d/%02d",63);
#elif defined(FEATURE_DISP_220X176)
				WSTRLCPY(wFormat,L"%04d/%02d/%02d",63);
#elif defined(FEATURE_DISP_128X160)
				WSTRLCPY(wFormat,L"%04d/%02d/%02d",63);
#elif defined(FEATURE_DISP_240X320)
				WSTRLCPY(wFormat,L"%04d/%02d/%02d",63);
#elif defined(FEATURE_DISP_320X240)
				WSTRLCPY(wFormat,L"%04d/%02d/%02d",63);
#else
                WSTRLCPY(wFormat,L"%02d/%02d",63);
#endif //FEATURE_DISP_176X220
                WSPRINTF(wszDate, sizeof(wszDate), wFormat,  jDate.wYear, jDate.wMonth,jDate.wDay);
                break;
        }
#if defined(FEATURE_DISP_176X220)
		DrawTextWithProfile(pMe->a.m_pIShell,
                                  pMe->m_pDisplay,
                                  RGB_WHITE_NO_TRANS,
                                  AEE_FONT_NORMAL,
                                  &wszDate[0], -1,
                                  0, 0, &rc_date, 
                                  IDF_ALIGN_MIDDLE
                                  | IDF_ALIGN_CENTER
                                  | IDF_TEXT_TRANSPARENT);
#elif defined(FEATURE_DISP_128X128)
    {
        int str_lenth = IDISPLAY_MeasureText(pMe->m_pDisplay, AEE_FONT_NORMAL, (const AECHAR *)wszDate);
        rc_date.x = (128 - str_lenth)/2;
        rc_date.dx = str_lenth;
        DrawTextWithProfile(pMe->a.m_pIShell,
                                  pMe->m_pDisplay,
                                  RGB_WHITE_NO_TRANS,
                                  AEE_FONT_NORMAL,
                                  &wszDate[0], -1,
                                  0, 0, &rc_date, 
                                  IDF_ALIGN_MIDDLE
                                  | IDF_ALIGN_CENTER
                                  | IDF_TEXT_TRANSPARENT);  

        MSG_FATAL("str_lenth=%d, rc_date.x=%d", str_lenth, rc_date.x, 0);
    }
    /*    DrawTextWithProfile(pMe->a.m_pIShell,
                                  pMe->m_pDisplay,
                                  RGB_WHITE_NO_TRANS,
                                  AEE_FONT_NORMAL,
                                  &wszDate[0], -1,
                                  0, 0, &rc_date, 
                                  IDF_ALIGN_MIDDLE
                                  | IDF_ALIGN_LEFT
                                  | IDF_TEXT_TRANSPARENT); 		*/
#elif defined(FEATURE_DISP_160X128)
        DrawGreyBitTextWithProfile(pMe->a.m_pIShell,
                                  pMe->m_pDisplay,
                                  RGB_WHITE_NO_TRANS,
                                  #if defined(FEATURE_VERSION_HITZ181)||defined(FEATURE_VERSION_MTM)
                                  18,
                                  #else
                                  12,
                                  #endif
                                  &wszDate[0], -1,
                                  0, 0, &rc_date, 
                                  IDF_ALIGN_MIDDLE
                                  | IDF_ALIGN_RIGHT
                                  | IDF_TEXT_TRANSPARENT); 
#elif defined(FEATURE_DISP_220X176)
        DrawGreyBitTextWithProfile(pMe->a.m_pIShell,
                                  pMe->m_pDisplay,
                                  RGB_WHITE_NO_TRANS,
                                  20,
                                  &wszDate[0], -1,
                                  0, 0, &rc_date, 
                                  IDF_ALIGN_MIDDLE
                                  | IDF_ALIGN_RIGHT          
                                  | IDF_TEXT_TRANSPARENT); 
#elif defined(FEATURE_DISP_128X160)
#ifdef FEATURE_VERSION_MYANMAR
		DrawGreyBitTextWithProfile(pMe->a.m_pIShell,
                                  pMe->m_pDisplay,
                                  RGB_WHITE_NO_TRANS,
                                  12,
                                  &wszDate[0], -1,
                                  0, 0, &rc_date, 
                                  IDF_ALIGN_MIDDLE
                                  | IDF_ALIGN_CENTER
                                  | IDF_TEXT_TRANSPARENT); 

#else
        DrawGreyBitTextWithProfile(pMe->a.m_pIShell,
                                  pMe->m_pDisplay,
                                  RGB_WHITE_NO_TRANS,
                                  12,
                                  &wszDate[0], -1,
                                  0, 0, &rc_date, 
                                  IDF_ALIGN_MIDDLE
                                  | IDF_ALIGN_RIGHT
                                  | IDF_TEXT_TRANSPARENT); 
#endif
#elif defined(FEATURE_DISP_240X320)
#ifdef FEATURE_LCD_TOUCH_ENABLE
	rc_date.y = 130;
	DrawGreyBitTextWithProfile(pMe->a.m_pIShell,
                                  pMe->m_pDisplay,
                                  RGB_WHITE_NO_TRANS,
                                  12,
                                  &wszDate[0], -1,
                                  0, 0, &rc_date, 
                                  IDF_ALIGN_MIDDLE
                                  | IDF_ALIGN_RIGHT
                                  | IDF_TEXT_TRANSPARENT); 
		
#else
        DrawGreyBitTextWithProfile(pMe->a.m_pIShell,
                                  pMe->m_pDisplay,
                                  RGB_WHITE_NO_TRANS,
                                  12,
                                  &wszDate[0], -1,
                                  0, 0, &rc_date, 
                                  IDF_ALIGN_MIDDLE
                                  | IDF_ALIGN_RIGHT
                                  | IDF_TEXT_TRANSPARENT); 
#endif
#elif defined(FEATURE_DISP_320X240)
        DrawGreyBitTextWithProfile(pMe->a.m_pIShell,
                                  pMe->m_pDisplay,
                                  RGB_WHITE_NO_TRANS,
                                  24,
                                  &wszDate[0], -1,
                                  0, 0, &rc_date, 
                                  IDF_ALIGN_MIDDLE
                                  | IDF_ALIGN_RIGHT
                                  | IDF_TEXT_TRANSPARENT); 
#else
        DrawTextWithProfile(pMe->a.m_pIShell,
                                  pMe->m_pDisplay,
                                  RGB_WHITE_NO_TRANS,
                                  AEE_FONT_NORMAL,
                                  &wszDate[0], -1,
                                  0, 0, &rc, 
                                  IDF_ALIGN_MIDDLE
                                  | IDF_ALIGN_RIGHT
                                  | IDF_TEXT_TRANSPARENT); 
#endif //FEATURE_DISP_176X220
    }
#else /*FEATURE_TIME_DATA_SETTING*/
    //Get date format     
    (void) ISHELL_LoadResString(pMe->a.m_pIShell,
                                AEE_COREAPPRES_LANGFILE,
                                IDS_MONTH_DATE,
                                wFormat,
                                sizeof(wFormat));

    WSPRINTF(wszDate, sizeof(wszDate), wFormat,jDate.wYear, jDate.wDay, jDate.wMonth);
#if defined(FEATURE_DISP_176X220)
    (void)DrawTextWithProfile(pMe->a.m_pIShell,
                              pMe->m_pDisplay,
                              RGB_WHITE_NO_TRANS,
                              AEE_FONT_NORMAL,
                              &wszDate[5], -1,
                              0, 0, &rc_date, 
                              IDF_ALIGN_MIDDLE
                              | IDF_ALIGN_LEFT
                              | IDF_TEXT_TRANSPARENT);  
#elif defined(FEATURE_DISP_128X128)
{
    int str_lenth = IDISPLAY_MeasureText(pMe->m_pDisplay, AEE_FONT_NORMAL, (const AECHAR *)wszDate);
    rc_date.x = (128 - str_lenth)/2;
    rc_date.dx = str_lenth;

	(void)DrawTextWithProfile(pMe->a.m_pIShell,
                              pMe->m_pDisplay,
                              RGB_WHITE_NO_TRANS,
                              AEE_FONT_NORMAL,
                              &wszDate[5], -1,
                              0, 0, &rc_date, 
                              IDF_ALIGN_MIDDLE
                              | IDF_ALIGN_LEFT
                              | IDF_TEXT_TRANSPARENT);
    MSG_FATAL("str_lenth=%d, rc_date.x=%d", str_lenth, rc_date.x, 0);
}
#elif defined(FEATURE_DISP_160X128)
	(void)DrawTextWithProfile(pMe->a.m_pIShell,
                              pMe->m_pDisplay,
                              RGB_WHITE_NO_TRANS,
                              AEE_FONT_NORMAL,
                              &wszDate[5], -1,
                              0, 0, &rc_date, 
                              IDF_ALIGN_MIDDLE
                              | IDF_ALIGN_LEFT
                              | IDF_TEXT_TRANSPARENT);
#elif defined(FEATURE_DISP_220X176)
	(void)DrawTextWithProfile(pMe->a.m_pIShell,
                              pMe->m_pDisplay,
                              RGB_WHITE_NO_TRANS,
                              AEE_FONT_NORMAL,
                              &wszDate[5], -1,
                              0, 0, &rc_date, 
                              IDF_ALIGN_MIDDLE
                              | IDF_ALIGN_LEFT
                              | IDF_TEXT_TRANSPARENT);
#elif defined(FEATURE_DISP_128X160)
	(void)DrawTextWithProfile(pMe->a.m_pIShell,
                              pMe->m_pDisplay,
                              RGB_WHITE_NO_TRANS,
                              AEE_FONT_NORMAL,
                              &wszDate[5], -1,
                              0, 0, &rc_date, 
                              IDF_ALIGN_MIDDLE
                              | IDF_ALIGN_LEFT
                              | IDF_TEXT_TRANSPARENT);
#elif defined(FEATURE_DISP_240X320)
	(void)DrawTextWithProfile(pMe->a.m_pIShell,
                              pMe->m_pDisplay,
                              RGB_WHITE_NO_TRANS,
                              AEE_FONT_NORMAL,
                              &wszDate[5], -1,
                              0, 0, &rc_date, 
                              IDF_ALIGN_MIDDLE
                              | IDF_ALIGN_LEFT
                              | IDF_TEXT_TRANSPARENT);
#elif defined(FEATURE_DISP_320X240)
	(void)DrawTextWithProfile(pMe->a.m_pIShell,
                              pMe->m_pDisplay,
                              RGB_WHITE_NO_TRANS,
                              AEE_FONT_NORMAL,
                              &wszDate[5], -1,
                              0, 0, &rc_date, 
                              IDF_ALIGN_MIDDLE
                              | IDF_ALIGN_LEFT
                              | IDF_TEXT_TRANSPARENT);

#else
    (void)DrawTextWithProfile(pMe->a.m_pIShell,
                              pMe->m_pDisplay,
                              RGB_WHITE_NO_TRANS,
                              AEE_FONT_NORMAL,
                              &wszDate[5], -1,
                              0, 0, &rc, 
                              IDF_ALIGN_MIDDLE
                              | IDF_ALIGN_RIGHT
                              | IDF_TEXT_TRANSPARENT);  
#endif //FEATURE_DISP_176X220

#endif /*FEATURE_TIME_DATA_SETTING*/

#ifndef FEATURE_CARRIER_SUDAN_SUDATEL
    // 绘制星期字符串
    wszDate[0] = 0;
    wFormat[0] = 0;
    
    (void) ISHELL_LoadResString(pMe->a.m_pIShell,
                                AEECONTROLS_RES_FILE,
                                nWeekResID,
                                wszDate,
                                sizeof(wszDate));
#ifndef FEATURE_VERSION_N68
    wszDate[3] = (AECHAR)'\0';
#else
    {
    		nv_language_enum_type language;
		(void) ICONFIG_GetItem(pMe->m_pConfig,
                                       CFGI_LANGUAGE_SELECTION,
                                       &language,
                                       sizeof(language));

        if ( language != NV_LANGUAGE_MYANMAR )
        {
        		wszDate[3] = (AECHAR)'\0';
        }
    }
#endif
#if defined(FEATURE_DISP_128X128)
{
    int str_lenth = IDISPLAY_MeasureText(pMe->m_pDisplay, AEE_FONT_NORMAL, (const AECHAR *)wszDate);
    //rc_week.x = (128 - str_lenth)/2;
    //rc_week.dx = str_lenth;
	SETAEERECT(&rc_week, 
		   (128 - str_lenth)/2,
		   RPLMN_Y+15,
		   str_lenth, 
           pMe->m_nNormalFontHeight);

    (void)DrawTextWithProfile(pMe->a.m_pIShell,
                              pMe->m_pDisplay,
                              RGB_WHITE_NO_TRANS,
                              AEE_FONT_NORMAL,
                              wszDate, -1,
                              0, 0, &rc_week, 
                              IDF_ALIGN_MIDDLE
                              | IDF_ALIGN_CENTER
                              | IDF_TEXT_TRANSPARENT);

    MSG_FATAL("str_lenth=%d, rc_week.x=%d", str_lenth, rc_week.x, 0);
}   
#else
	
#ifdef FEATURE_VERSION_N68
     {
     	//int wszDatelen = WSTRLEN(wszDate);
     	//wszDatelen = wszDatelen+1;
     	//wszDate[wszDatelen] = (AECHAR)'\0';
     }
#else
	if ( nWeekResID == AEE_IDS_SUNDAY )
	{
		nv_language_enum_type language;
		(void) ICONFIG_GetItem(pMe->m_pConfig,
                                       CFGI_LANGUAGE_SELECTION,
                                       &language,
                                       sizeof(language));

        if ( language == NV_LANGUAGE_INDONESIAN )
        {
			wszDate[4] = (AECHAR)'\0';
		}
		else
		{
			wszDate[3] = (AECHAR)'\0';
		}
	}
	else    
    {
		wszDate[3] = (AECHAR)'\0';
    }
#endif
#ifdef FEATURE_LCD_TOUCH_ENABLE
	rc_week.y = 150;
	(void)DrawTextWithProfile(pMe->a.m_pIShell,
                              pMe->m_pDisplay,
                              RGB_WHITE_NO_TRANS,
                              AEE_FONT_NORMAL,
                              wszDate, -1,
                              0, 0, &rc_week, 
                              IDF_ALIGN_MIDDLE
                              | IDF_ALIGN_RIGHT								  
                              | IDF_ALIGN_LEFT
                              | IDF_TEXT_TRANSPARENT);

#else
    (void)DrawTextWithProfile(pMe->a.m_pIShell,
                              pMe->m_pDisplay,
                              RGB_WHITE_NO_TRANS,
                              AEE_FONT_NORMAL,
                              wszDate, -1,
                              0, 0, &rc_week, 
                              IDF_ALIGN_MIDDLE
#if defined(FEATURE_DISP_176X220)
							  | IDF_ALIGN_CENTER
#elif defined(FEATURE_DISP_128X128)
                              | IDF_ALIGN_RIGHT
#elif defined(FEATURE_DISP_160X128)
							  | IDF_ALIGN_RIGHT
#elif defined(FEATURE_DISP_220X176)
#ifdef FEATURE_VERSION_W208S
							  | IDF_ALIGN_LEFT
#else
							  | IDF_ALIGN_RIGHT		
#endif							  
#elif defined(FEATURE_DISP_128X160)
#ifdef FEATURE_VERSION_MYANMAR
							  | IDF_ALIGN_CENTER
#else
							  | IDF_ALIGN_RIGHT	
#endif
#elif defined(FEATURE_DISP_240X320)
							  | IDF_ALIGN_RIGHT		
#elif defined(FEATURE_DISP_320X240)
							  | IDF_ALIGN_RIGHT								  
#else
                              | IDF_ALIGN_LEFT
#endif //FEATURE_DISP_176X220
                              | IDF_TEXT_TRANSPARENT);
#endif /*FEATURE_CARRIER_SUDAN_SUDATEL*/
#endif
#endif
            
} // CoreApp_UpdateDateTime

// 绘制待机界面的 "Menu         Contacts"
static void CoreApp_UpdateBottomBar(CCoreApp    *pMe)
{
    BottomBar_e_Type    eBBarType = BTBAR_NONE;  

	MSG_FATAL("***zzg CoreApp_UpdateBottomBar m_bemergencymode=%d, OEMKeyguard_IsEnabled=%d***", pMe->m_bemergencymode, OEMKeyguard_IsEnabled(), 0);

/*	
    if(pMe->m_bemergencymode)
    {
        eBBarType = BTBAR_BACK;
    }
#ifdef FEATURE_KEYGUARD
    else if(OEMKeyguard_IsEnabled())
    {
    	#ifdef FEATURE_LCD_TOUCH_ENABLE
		
		#else
    		#if defined(FEATURE_VERSION_HITZ181)||defined(FEATURE_VERSION_MTM)||defined(FEATURE_VERSION_S1000T)
    			eBBarType = BTBAR_UNLOCK_SOS;
        	#elif defined(FEATURE_VERSION_W515V3)||defined(FEATURE_VERSION_C11)|| defined(FEATURE_VERSION_1110W516)
        		eBBarType = BTBAR_LUNLOCK;
        	#elif defined(FEATURE_VERSION_VERYKOOL)
        		eBBarType = BTBAR_UNLOCK_SOS;
    		#else
        		eBBarType = BTBAR_UNLOCK;
        	#endif
		#endif
    }
#endif
*/

	
#ifdef FEATURE_KEYGUARD
    if(OEMKeyguard_IsEnabled())
    {
    	#ifdef FEATURE_LCD_TOUCH_ENABLE
		
		#else
    		#if defined(FEATURE_VERSION_HITZ181)||defined(FEATURE_VERSION_MTM)||defined(FEATURE_VERSION_S1000T)
    			eBBarType = BTBAR_UNLOCK_SOS;
        	#elif defined(FEATURE_VERSION_W515V3)||defined(FEATURE_VERSION_C11)|| defined(FEATURE_VERSION_C180)|| defined(FEATURE_VERSION_1110W516) 
        		eBBarType = BTBAR_LUNLOCK;
            #elif defined(FEATURE_VERSION_W027)
                eBBarType = BTBAR_UNLOCK_M;
        	#elif defined(FEATURE_VERSION_VERYKOOL)
        		eBBarType = BTBAR_UNLOCK_SOS;
    		#else
        		eBBarType = BTBAR_UNLOCK;
        	#endif
		#endif
    }
	else
#endif
	if(pMe->m_bemergencymode)
    {
        eBBarType = BTBAR_BACK;
    }
    else
    {
#ifdef FEATURE_VERSION_SMART					//Add By zzg 2010_07_20
		eBBarType = BTBAR_FACEBOOK_CHAT;
#elif defined FEATURE_VERSION_M8
	    eBBarType = BTBAR_FACEBOOK_CHAT;
#elif defined FEATURE_VERSION_M8021
		eBBarType = BTBAR_FACEBOOK_CHAT;
#elif defined FEATURE_VERSION_M8P
		eBBarType = BTBAR_FACEBOOK_CHAT;
#elif defined FEATURE_VERSION_FLEXI203P
		eBBarType = BTBAR_MENU_CONTACTS;
#else										//Include IVIO
	#if defined (FEATURE_GURU)
		eBBarType = BTBAR_FGURU_FPORTAL;	
    #elif defined (FEATURE_FMN2010)
        eBBarType = BTBAR_FMUSLIM_FPORTAL;
	#elif defined (FEATURE_NASRANI) 
		eBBarType = BTBAR_FNASRANI_FPORTAL;
	#elif defined (FEATURE_FPT005)
		eBBarType = BTBAR_CONTACTS_FPORTAL; //add by yangdecai
	#elif defined(FEATURE_VERSION_HITZ181)
		#if defined(FEATURE_VERSION_HITZ181)
	    if(!IRUIM_IsCardConnected(pMe->m_pIRUIM))
	    {
			eBBarType = BTBAR_MENU_SOS; //add by yangdecai
		}
		else
		#endif
		{
			eBBarType = BTBAR_MENU_CONTACTS; //add by yangdecai
		}			
	#else
        #if defined(FEATURE_VERSION_W515V3)|| defined(FEATURE_VERSION_S1000T)|| defined(FEATURE_VERSION_W208S)
           eBBarType = BTBAR_MENU_CONTACTS;
        #else
		   eBBarType = BTBAR_MESSAGES_CONTACTS; //add by yangdecai  BTBAR_MESSAGES_CONTACTS
		#endif
	#endif
#endif
	
    }

	MSG_FATAL("***zzg CoreApp_UpdateBottomBar eBBarType=%x***", eBBarType, 0, 0);
	
	#ifdef FEATURE_LCD_TOUCH_ENABLE
	
	#else
    DrawBottomBar_Ex(pMe->a.m_pIShell, pMe->m_pDisplay,eBBarType);
	#endif
}

/*==============================================================================
函数     : CoreApp_PlayPwrOnAni

说明     :  开关机动画有可能不一样，考虑的函数的耦合，
                   开关机动画播放分别用不同的函数

参数     :  pMe [in]: 指向 Core Applet 对象结构的指针。

返回     :  无 

备注     :  WHEN                    WHO                WHAT                     WHY
           2008-02-17              fj                   V1.6                       New Dev, for add play poweroff animation
==============================================================================*/
static void CoreApp_PlayPwrOnAni(CCoreApp *pMe) 
{

    AEEImageInfo  ImgInfo;  //Gets the information about an image

    ASSERT(pMe != NULL);
    MSG_FATAL("CoreApp_PlayPwrOnAni Start",0,0,0);
#ifndef FEATURE_USES_LOWMEM
    if ( (NULL != pMe->m_pStartupAniImg) && (pMe->m_wStartupAniTime < 1)  )
#else
    if (pMe->m_wStartupAniTime < PWRON_ANI_FRAME_COUNT )
#endif
    {
#ifndef FEATURE_USES_LOWMEM
        IIMAGE_GetInfo( pMe->m_pStartupAniImg, &ImgInfo );

        // 设置动画速度(毫秒)
        IIMAGE_SetAnimationRate(pMe->m_pStartupAniImg, PWRON_ANI_RATE);
        IIMAGE_SetParm(pMe->m_pStartupAniImg, IPARM_PLAYTIMES, 1, 0);

        // 设置图像中的帧数
        IIMAGE_SetFrameCount(pMe->m_pStartupAniImg, PWRON_ANI_FRAME_COUNT);

        // 设置要显示的图像的实际大小
        IIMAGE_SetDrawSize( pMe->m_pStartupAniImg, ImgInfo.cx/PWRON_ANI_FRAME_COUNT, ImgInfo.cy );
        
        IIMAGE_Start( pMe->m_pStartupAniImg,
                            (pMe->m_rc.dx - ImgInfo.cx/PWRON_ANI_FRAME_COUNT)/2,
                            (pMe->m_rc.dy - ImgInfo.cy)/2 );
        pMe->m_wStartupAniTime++; // 滚动播放次数
       (void) ISHELL_SetTimer(pMe->a.m_pIShell,
                             PWRON_ANI_TIME,
                             (PFNNOTIFY)CoreApp_PlayPwrOnAni,
                             (void*)pMe);
#else
        {
            #define PWRON_STR L"Welcome"
            extern int GreyBitBrewFont_DrawText(IDisplay *p, int nSize, const AECHAR *psz, int nl, int x, int y, const AEERect *prcb, uint32 flags);
            IDISPLAY_ClearScreen(pMe->m_pDisplay);
            GreyBitBrewFont_DrawText(pMe->m_pDisplay, 
                                     8+2*pMe->m_wStartupAniTime,
                                     PWRON_STR,
                                     -1,
                                     0,0,
                                     NULL,
                                     IDF_ALIGN_CENTER|IDF_ALIGN_MIDDLE);
            IDISPLAY_UpdateEx(pMe->m_pDisplay, FALSE);
        }
        pMe->m_wStartupAniTime++; // 滚动播放次数
       (void) ISHELL_SetTimer(pMe->a.m_pIShell,
                              ANI_RATE,
                             (PFNNOTIFY)CoreApp_PlayPwrOnAni,
                             (void*)pMe);
#endif
    }
    else
    {
        MSG_FATAL("CoreApp_PlayPwrOnAni 4",0,0,0);
        IBACKLIGHT_Enable(pMe->m_pBacklight);
        IALERT_StopRingerAlert(pMe->m_pAlert);
#ifndef FEATURE_USES_LOWMEM
        if ( NULL != pMe->m_pStartupAniImg )
        {     
            MSG_FATAL("CoreApp_PlayPwrOnAni 5",0,0,0);
            IIMAGE_Stop(pMe->m_pStartupAniImg);
            IIMAGE_Release(pMe->m_pStartupAniImg);
            pMe->m_pStartupAniImg = NULL;
        }
#endif
        CLOSE_DIALOG(DLGRET_OK)
    }

}

/*==============================================================================
函数     : CoreApp_PlayPwrOffAni

说明     :  用于播放关机动画，开关机动画有可能不一样，考虑的函数的耦合，
                   开关机动画播放分别用不同的函数

参数     :  pMe [in]: 指向 Core Applet 对象结构的指针。

返回     :  无 

备注     :  WHEN                    WHO                WHAT                     WHY
           2008-02-17              fj                  V1.6                     New Dev, for add play poweroff animation
==============================================================================*/
static void CoreApp_PlayPwrOffAni(CCoreApp *pMe) 
{
    AEEImageInfo  ImgInfo;  //Gets the information about an image

    ASSERT(pMe != NULL);
#ifndef FEATURE_USES_LOWMEM
    if ( (NULL != pMe->m_pStartupAniImg) && (pMe->m_wStartupAniTime < 1)  ) 
#else
    if (pMe->m_wStartupAniTime < PWRON_ANI_FRAME_COUNT) 
#endif
    {
#ifndef FEATURE_USES_LOWMEM
        IIMAGE_GetInfo( pMe->m_pStartupAniImg, &ImgInfo );
        
        // 设置动画速度(毫秒)
        IIMAGE_SetAnimationRate(pMe->m_pStartupAniImg, PWROFF_ANI_RATE);
        IIMAGE_SetParm(pMe->m_pStartupAniImg, IPARM_PLAYTIMES, 1, 0);
        
        // 设置图像中的帧数
        IIMAGE_SetFrameCount(pMe->m_pStartupAniImg, PWROFF_ANI_FRAME_COUNT);

        // 设置要显示的图像的实际大小
        IIMAGE_SetDrawSize( pMe->m_pStartupAniImg, 
                                ImgInfo.cx/PWROFF_ANI_FRAME_COUNT, ImgInfo.cy );
        // 开始播放关机动画
        IIMAGE_Start(pMe->m_pStartupAniImg, 
                    (pMe->m_rc.dx - ImgInfo.cx/PWROFF_ANI_FRAME_COUNT)/2, 
                    (pMe->m_rc.dy - ImgInfo.cy)/2 );
        
        pMe->m_wStartupAniTime++; // 滚动播放次数
        AEE_SetSysTimer( PWROFF_ANI_TIME,  (PFNNOTIFY)CoreApp_PlayPwrOffAni,  (void*)pMe);
#else
        {
            #define PWROFF_STR L"Bye-Bye"
            extern int GreyBitBrewFont_DrawText(IDisplay *p, int nSize, const AECHAR *psz, int nl, int x, int y, const AEERect *prcb, uint32 flags);
            IDISPLAY_ClearScreen(pMe->m_pDisplay);
            GreyBitBrewFont_DrawText(pMe->m_pDisplay, 
                                     8+2*(PWRON_ANI_FRAME_COUNT-pMe->m_wStartupAniTime),
                                     PWROFF_STR,
                                     -1,
                                     0,0,
                                     NULL,
                                     IDF_ALIGN_CENTER|IDF_ALIGN_MIDDLE);
            IDISPLAY_UpdateEx(pMe->m_pDisplay, FALSE);
        }
        pMe->m_wStartupAniTime++; // 滚动播放次数
        AEE_SetSysTimer( ANI_RATE,  (PFNNOTIFY)CoreApp_PlayPwrOffAni,  (void*)pMe);
#endif
    }
    else
    {
#ifndef FEATURE_USES_LOWMEM
        if ( NULL != pMe->m_pStartupAniImg )
        {     
            IIMAGE_Stop(pMe->m_pStartupAniImg);
            IIMAGE_Release(pMe->m_pStartupAniImg);
            pMe->m_pStartupAniImg = NULL;
        }
#endif
        // 发送事件关闭开机动画播放对话
        (void)ISHELL_SendEvent( pMe->a.m_pIShell, AEECLSID_CORE_APP, EVT_DISPLAYDIALOGTIMEOUT, 0, 0);
    }
}

#ifdef FEATURE_USES_BLACKBERRY

/*==============================================================================
函数      :  CoreApp_DrawBlackBerry_IDLE

说明      :  函数用于绘制黑莓IDLE, 绘制黑莓IDLE 后不会进行LCD的刷新,
                  LCD的刷新由调用者管理。

参数      :  pMe [In]: 指向Idle Applet对象结构的指针,该结构包含小程序的特定信息.
       
返回值 : 无。

备注     :  WHEN                    WHO                WHAT                     WHY
           2008-02-20              Yangdecai                 V1.6                    New Dev, for Wallpaper
==============================================================================*/

static void CoreApp_DrawBlackBerry_IDLE(CCoreApp *pMe)
{
	AEEImageInfo   ImgInfo;
	AEERect oldClip;
    AEERect clip;
    AECHAR        szBlackBerryName[AEE_MAX_FILE_NAME] = {0};
    //char               szNewBlackBerryName[AEE_MAX_FILE_NAME/*FILESPECLEN*/];
    int i = 0;
    uint8 Draw_x = 0;
    uint8 Draw_y = SCREEN_HEIGHT - IDLE_BLACKBERRY_DRAWDY;
    uint16 Resid = IDS_STR_BLACKBERRY_ONE +(pMe->m_CurMainItems);
     //int                    nX = 0,  nY = 0;
    if ( (NULL == pMe) || (IDD_IDLE != pMe->m_wActiveDlgID) )
    {
        return;
    }
    if(pWallPaper != NULL)
    {
    	AEEImageInfo m_ImageInfo;
		IImage_GetInfo(pWallPaper,&m_ImageInfo);
		MSG_FATAL("m_ImageInfo.cx=%d, m_ImageInfo.cy=%d", m_ImageInfo.cx, m_ImageInfo.cy, 0);
        SETAEERECT(&clip, 0, Draw_y-20, SCREEN_WIDTH, IDLE_BLACKBERRY_DRAWDY+20); 
        IDISPLAY_GetClipRect( pMe->m_pDisplay, &oldClip);
        IDISPLAY_SetClipRect( pMe->m_pDisplay, &clip);
        MSG_FATAL("clip.x=%d, clip.y=%d,pMe->m_rc.dx=%d", clip.x, clip.y, pMe->m_rc.dx);
        IIMAGE_SetOffset( pWallPaper, ( m_ImageInfo.cx -  SCREEN_WIDTH)/2,(Draw_y-20)-( pMe->m_rc.dy - m_ImageInfo.cy)/2);
        //IIMAGE_SetOffset( pWallPaper, 0,MUSIC_WIDTH);
        MSG_FATAL("clip.dx=%d, clip.dy=%d", clip.dx, clip.dy, 0);
		MSG_FATAL("pMe->m_rc.dy=%d", pMe->m_rc.dy,0,0);
        IIMAGE_SetDrawSize( pWallPaper, clip.dx,clip.dy);
        IIMAGE_Draw( pWallPaper, 0,Draw_y-20);
        IDISPLAY_SetClipRect( pMe->m_pDisplay,&oldClip);
        IIMAGE_SetOffset( pWallPaper, 0,0);
        //IIMAGE_SetDrawSize( pWallPaper, pMe->m_rc.dx,pMe->m_rc.dy);
        IIMAGE_SetDrawSize( pWallPaper, m_ImageInfo.cx,m_ImageInfo.cy);
        
		
    }
    MEMSET( &ImgInfo, 0x00, sizeof(ImgInfo) );
    MSG_FATAL("CoreApp_DrawBlackBerry_IDLE..................2",0,0,0);
	for(i=0;i<IDLE_BLACKBERRY_ITEMMAX;i++)
	{
 		IImage_GetInfo(pMe->m_pImageSelIcon[i],&ImgInfo);
		if(pMe->m_CurMainItems == i)
		{
			IIMAGE_Draw(pMe->m_pImageSelIcon[i],
	                    Draw_x, 
	                    Draw_y);
		}
		else
		{
    		IIMAGE_Draw(pMe->m_pImageIcon[i],
	                    Draw_x, 
	                    Draw_y);
    	}
        Draw_x = Draw_x+ ImgInfo.cx;
    }
    ISHELL_LoadResString(pMe->a.m_pIShell, 
                                            AEE_COREAPPRES_LANGFILE,
                                            Resid,
                                            szBlackBerryName,
                                            AEE_MAX_FILE_NAME);
                                          
     (void)DrawTextWithProfile(pMe->a.m_pIShell,
	                              pMe->m_pDisplay,
	                              RGB_WHITE_NO_TRANS,
	                              AEE_FONT_BOLD,
	                              szBlackBerryName, -1,
	                              0, Draw_y - 20,NULL, 
	                              IDF_ALIGN_CENTER
	                              | IDF_TEXT_TRANSPARENT);
    IDisplay_Update(pMe->m_pDisplay);
}

#endif
#ifdef FEATURE_LCD_TOUCH_ENABLE

static void CoreApp_DrawTouch_IDLE(CCoreApp *pMe)
{
	AEEImageInfo   ImgInfo;
	AEERect oldClip;
    AEERect clip;
    AECHAR        szBlackBerryName[AEE_MAX_FILE_NAME] = {0};
    //char               szNewBlackBerryName[AEE_MAX_FILE_NAME/*FILESPECLEN*/];
    int i = 0;
    uint8 Draw_x = 0;
    uint8 Draw_y = SCREEN_HEIGHT - IDLE_TOUCH_DRAWDX - 40;

     //int                    nX = 0,  nY = 0;
    if ( (NULL == pMe) || (IDD_IDLE != pMe->m_wActiveDlgID) )
    {
        return;
    }
    if(pWallPaper != NULL)
    {
    	AEEImageInfo m_ImageInfo;
		IImage_GetInfo(pWallPaper,&m_ImageInfo);
		MSG_FATAL("m_ImageInfo.cx=%d, m_ImageInfo.cy=%d", m_ImageInfo.cx, m_ImageInfo.cy, 0);
        SETAEERECT(&clip, 0, Draw_y-20, SCREEN_WIDTH, IDLE_TOUCH_DRAWDY+20); 
        IDISPLAY_GetClipRect( pMe->m_pDisplay, &oldClip);
        IDISPLAY_SetClipRect( pMe->m_pDisplay, &clip);
        MSG_FATAL("clip.x=%d, clip.y=%d,pMe->m_rc.dx=%d", clip.x, clip.y, pMe->m_rc.dx);
        IIMAGE_SetOffset( pWallPaper, ( m_ImageInfo.cx -  SCREEN_WIDTH)/2,(Draw_y-20)-( pMe->m_rc.dy - m_ImageInfo.cy)/2);
        //IIMAGE_SetOffset( pWallPaper, 0,MUSIC_WIDTH);
        MSG_FATAL("clip.dx=%d, clip.dy=%d", clip.dx, clip.dy, 0);
		MSG_FATAL("pMe->m_rc.dy=%d", pMe->m_rc.dy,0,0);
        IIMAGE_SetDrawSize( pWallPaper, clip.dx,clip.dy);
        IIMAGE_Draw( pWallPaper, 0,Draw_y-20);
        IDISPLAY_SetClipRect( pMe->m_pDisplay,&oldClip);
        IIMAGE_SetOffset( pWallPaper, 0,0);
        //IIMAGE_SetDrawSize( pWallPaper, pMe->m_rc.dx,pMe->m_rc.dy);
        IIMAGE_SetDrawSize( pWallPaper, m_ImageInfo.cx,m_ImageInfo.cy);
        
		
    }
    MEMSET( &ImgInfo, 0x00, sizeof(ImgInfo) );
	for(i=0;i<IDLE_TOUCH_ITEMMAX;i++)
	{
		
    	AEERect     rc;
		AECHAR      wszBottomstr[20]={0};
		uint16 nResID = IDS_STR_TOUCH_ONE;// 中
		//Draw image
		Draw_x = (IDLE_TOUCH_IDLE_BOTTOM_SPC*(i+1))+(i*IDLE_TOUCH_DRAWDX);
    	IIMAGE_Draw(pMe->m_pImageTouchIcon[i],
	                    Draw_x, 
	                    Draw_y);

		//Draw string
		nResID = nResID+i;
		(void) ISHELL_LoadResString(pMe->a.m_pIShell,
                                    CORE_RES_PATH,
                                    nResID,
                                    wszBottomstr,
                                    sizeof(wszBottomstr));
		rc.x  = (IDLE_TOUCH_IDLE_BOTTOM_SPC*(i+1))+(i*IDLE_TOUCH_DRAWDX)-5;
		rc.y  = SCREEN_HEIGHT - 40;
		rc.dx = IDLE_TOUCH_DRAWDX+10;
		rc.dy = IDLE_TOUCH_DRAWDY;

		DrawGreyBitTextWithProfile(pMe->a.m_pIShell,
	                              pMe->m_pDisplay,
	                              RGB_WHITE_NO_TRANS,
	                              18,
	                              wszBottomstr, -1,
	                              0, 0, &rc, 
	                              IDF_ALIGN_MIDDLE
	                              | IDF_ALIGN_CENTER
	                              | IDF_TEXT_TRANSPARENT); 
		
    }
	if(OEMKeyguard_IsEnabled())
	{
	
		Appscomm_Draw_Keyguard_BackGroudbar(pMe->m_pDisplay,0,SCREEN_HEIGHT-60);
	}
}
#endif

/*==============================================================================
函数      :  CoreApp_DrawWallPaper

说明      :  函数用于绘制墙纸, 绘制墙纸后不会进行LCD的刷新,
                  LCD的刷新由调用者管理。

参数      :  pMe [In]: 指向Idle Applet对象结构的指针,该结构包含小程序的特定信息.
       
返回值 : 无。

备注     :  WHEN                    WHO                WHAT                     WHY
           2008-02-20              fj                  V1.6                    New Dev, for Wallpaper
==============================================================================*/
static void CoreApp_DrawWallPaper(CCoreApp *pMe) 
{
    AEEImageInfo   ImgInfo;
    static char        szWallPaperName[AEE_MAX_FILE_NAME/*FILESPECLEN*/];
    char                 szNewWallPaperName[AEE_MAX_FILE_NAME/*FILESPECLEN*/];
    //int                    nX = 0,  nY = 0;
    if ( (NULL == pMe) || (IDD_IDLE != pMe->m_wActiveDlgID) )
    {
        return;
    }

    MEMSET( &ImgInfo, 0x00, sizeof(ImgInfo) );
#ifdef FEATRUE_SET_ANN_FULL_SCREEN
    pMe->m_capture = DB_CAPTURE_NONE;
#endif
    if ( NULL == pWallPaper )
    {  
        // 初始化墙纸或上次取墙纸不成功
        MEMSET(szWallPaperName, 0x00, sizeof(szWallPaperName));
        
        // 取设定的墙纸文件名
        (void) ICONFIG_GetItem(pMe->m_pConfig,
                                CFGI_WALLPAPER,
                                szWallPaperName,
                                sizeof(szWallPaperName));

        // test the file existed or not, if not existed, load the default wallpaper.
        {
            IFileMgr *pFileMgr = NULL;
            ISHELL_CreateInstance(pMe->a.m_pIShell, AEECLSID_FILEMGR, (void **)&pFileMgr);
            if(pFileMgr)
            {
                if(IFILEMGR_Test(pFileMgr, szWallPaperName) == SUCCESS)
                {
                    pWallPaper = ISHELL_LoadImage(pMe->a.m_pIShell, szWallPaperName);
                }
                else // if specified wallpaper not existed, load the default wallpaper
                {
                    pWallPaper = ISHELL_LoadImage(pMe->a.m_pIShell, OEMNV_WALLPAPER);
                }
                IFILEMGR_Release(pFileMgr);
                pFileMgr = NULL;
            }
        }
        
#ifdef FEATRUE_SET_ANN_FULL_SCREEN
        pMe->m_capture = DB_CAPTURE_NEED;
#endif
    }
    else
    {   
        // 已经成功加载墙纸图片，但须检查墙纸图片设置是否被变更
        MEMSET(szNewWallPaperName, 0x00, sizeof(szNewWallPaperName));
        
        // 取设定的墙纸文件名
        (void) ICONFIG_GetItem(pMe->m_pConfig,
                          CFGI_WALLPAPER,
                          szNewWallPaperName,
                          sizeof(szNewWallPaperName));
                          
        if ( 0 != STRCMP(szNewWallPaperName, szWallPaperName) )
        {   
            // 墙纸设置已变
            MEMSET( szWallPaperName, 0x00, sizeof(szWallPaperName) );
            (void)STRCPY( szWallPaperName, szNewWallPaperName );
            
            // 释放先前图片占用的资源
            IIMAGE_Release(pWallPaper);
            
            // test the file existed or not, if not existed, load the default wallpaper.
            {
                IFileMgr *pFileMgr = NULL;
                ISHELL_CreateInstance(pMe->a.m_pIShell, AEECLSID_FILEMGR, (void **)&pFileMgr);
                if(pFileMgr)
                {
                    if(IFILEMGR_Test(pFileMgr, szWallPaperName) == SUCCESS)
                    {
                        pWallPaper = ISHELL_LoadImage(pMe->a.m_pIShell, szWallPaperName);
                    }
                    else // if specified wallpaper not existed, load the default wallpaper
                    {
                        pWallPaper = ISHELL_LoadImage(pMe->a.m_pIShell, OEMNV_WALLPAPER);
                    }
                    IFILEMGR_Release(pFileMgr);
                    pFileMgr = NULL;
                }
            }
#ifdef FEATRUE_SET_ANN_FULL_SCREEN
            pMe->m_capture = DB_CAPTURE_NEED;
#endif
        }
    }
#if 0
    if ( NULL != pWallPaper )
    {
        IIMAGE_GetInfo(pWallPaper, &ImgInfo);
        
        nX = (ImgInfo.cx >= pMe->m_rc.dx)?  0 : ( (pMe->m_rc.dx - ImgInfo.cx)/2 );
        //#if !defined(LCD_OPIMIZE_ICON)
        //nY = (ImgInfo.cy >= pMe->m_rc.dy)?  12 : ( (pMe->m_rc.dy - ImgInfo.cy - 12)/2 + 12 );
        //#else   
        nY = (ImgInfo.cy >= pMe->m_rc.dy)?  0 : ( (pMe->m_rc.dy - ImgInfo.cy )/2 );
        //#endif 
        
        IIMAGE_Draw(pWallPaper, nX, nY);
    }
    else
    {
        IDISPLAY_EraseRect(pMe->m_pDisplay, &pMe->m_rc);
    }
#endif
}



/*==============================================================================
函数：
      CoreApp_LaunchApplet

说明：
       用于启动指定 Applet。

参数：
       pMe [in]：指向Core Applet对象结构的指针。该结构包含小程序的
                          特定信息。
       classID [in]：指定 Applet 的 类 ID 号。
返回值：
        TRUE：启动成功
        FALSE：当前不能启动或启动失败

备注：
       
==============================================================================*/
static boolean CoreApp_LaunchApplet(CCoreApp *pMe,  AEECLSID classID)
{
/*
    int i = ISHELL_CanStartApplet(pMe->a.m_pIShell, classID);
    if (!ISHELL_CanStartApplet(pMe->a.m_pIShell, classID)) 
    {
        // 当前不能启动 指定 Applet
        ERR("Can't start Applet 0x%X    %d",classID,i,0);
        return FALSE;
    }
*/
    if(pMe->m_pAppStart)
    {
      return FALSE;
    }

    if (SUCCESS != ISHELL_StartApplet(pMe->a.m_pIShell, classID)) 
    {
        ERR("Failed to start Applet 0x%X",classID,0,0);		
        return FALSE;
    } 
    pMe->m_pAppStart=TRUE;

    return TRUE;
}

/*==============================================================================
函数:
    CoreApp_UpdateAnnunciator

说明:
    用于更新Annunciator的显示。

参数:
    pMe [In]: 指向 Core Applet 对象结构的指针。

返回值:
    无。

备注:
    刷新显示操作由调用者根据需要执行相关操作。
==============================================================================*/
void CoreApp_UpdateAnnunciator(CCoreApp *pMe)
{
    /*Set head set icon*/
    uint32 iann    = 0;
    byte alertType;
    boolean b_FMBackground = FALSE;
    boolean b_headset = FALSE;
    boolean missed_call_icon;
    //static boolean b_flag = TRUE;

	//Add By zzg 2010_11_22
#ifdef FEATURE_APP_BLUETOOTH
	boolean bt_status = FALSE;
#endif
	//Add End
	
	//Add for test
	//IANNUNCIATOR_SetField (pMe->m_pIAnn, ANNUN_FIELD_TCARD, ANNUN_STATE_TCARD_ON);
	//IANNUNCIATOR_SetField (pMe->m_pIAnn, ANNUN_FIELD_LOCKSTATUS, ANNUN_STATE_LOCKSTATUS_ON);
	//IANNUNCIATOR_SetField (pMe->m_pIAnn, ANNUN_FIELD_CALL, ANNUN_STATE_CALL_MISSEDCALL_ON);
	//IANNUNCIATOR_SetField (pMe->m_pIAnn, ANNUN_FIELD_SMS, ANNUN_STATE_SMS_SMAIL_ON);
	//IANNUNCIATOR_SetField (pMe->m_pIAnn, ANNUN_FIELD_FMRADIO, ANNUN_STATE_HEADSET_ON);
	//IANNUNCIATOR_SetField (pMe->m_pIAnn, ANNUN_FIELD_BLUETOOTH, ANNUN_STATE_BT_ON);
	//IANNUNCIATOR_SetField (pMe->m_pIAnn, ANNUN_FIELD_ALARM, ANNUN_STATE_ALARM_ON);
	//IANNUNCIATOR_SetField (pMe->m_pIAnn, ANNUN_FIELD_MMS, ANNUN_MMS_FULL_ON);
	//IANNUNCIATOR_SetField (pMe->m_pIAnn, ANNUN_FIELD_RINGTONE, ANNUN_STATE_RINGTONE_ALERT);
	//return;
	//Add End

	//Add By zzg 2012_10_29
	#ifdef FEATURE_VERSION_W317A	
	IFileMgr *pFileMgr = NULL;
    ISHELL_CreateInstance(pMe->a.m_pIShell, AEECLSID_FILEMGR, (void **)&pFileMgr);
	
    if (pFileMgr)
    {
		if (IFILEMGR_Test(pFileMgr, AEEFS_CARD0_DIR)==SUCCESS)		
		{
			IANNUNCIATOR_SetField (pMe->m_pIAnn, ANNUN_FIELD_TCARD, ANNUN_STATE_TCARD_ON);
		}
		else
		{
			IANNUNCIATOR_SetField (pMe->m_pIAnn, ANNUN_FIELD_TCARD, ANNUN_STATE_TCARD_OFF);
		}
    }
	#endif
	//Add End
	

    ICONFIG_GetItem(pMe->m_pConfig, CFGI_HEADSET_PRESENT, &b_headset, sizeof(b_headset));
    ICONFIG_GetItem(pMe->m_pConfig, CFGI_FM_BACKGROUND, &b_FMBackground, sizeof(b_FMBackground));
    ICONFIG_GetItem(pMe->m_pConfig, CFGI_PROFILE_CUR_NUMBER,&alertType, sizeof(alertType));//CFGI_ALERT_TYPE
    ICONFIG_GetItem(pMe->m_pConfig, CFGI_MISSED_CALL_ICON,&missed_call_icon, sizeof(missed_call_icon));
	//Add By zzg 2010_11_22
#ifdef FEATURE_APP_BLUETOOTH
	ICONFIG_GetItem(pMe->m_pConfig, CFGI_BT_STATUS,&bt_status, sizeof(bt_status));
#endif
	//Add End

    if(pMe->m_pIAnn != NULL)
    {
	    if (b_headset)
	    {
	        IANNUNCIATOR_SetField (pMe->m_pIAnn, ANNUN_FIELD_FMRADIO/*ANNUN_FIELD_HEADSET*/, ANNUN_STATE_HEADSET_ON/*ANNUN_STATE_ON*/);
		    if (b_FMBackground)
		    {
		        IANNUNCIATOR_SetField (pMe->m_pIAnn, ANNUN_FIELD_FMRADIO, ANNUN_STATE_FMRADIO_ON/*ANNUN_STATE_ON*/);
		    }
	    }
	    else
	    {
	        IANNUNCIATOR_SetField (pMe->m_pIAnn, ANNUN_FIELD_FMRADIO, ANNUN_STATE_FMRADIO_OFF/*ANNUN_STATE_OFF*/);
	        IANNUNCIATOR_SetField (pMe->m_pIAnn, ANNUN_FIELD_FMRADIO/*ANNUN_FIELD_HEADSET*/, ANNUN_STATE_HEADSET_OFF/*ANNUN_STATE_OFF*/);
	    }
#ifdef FEATURE_QQ_APP 
     IANNUNCIATOR_SetField (pMe->m_pIAnn, ANNUN_FIELD_FMRADIO/*ANNUN_FIELD_QQ*/, ANNUN_STATE_QQ_OFFLINE);
     IANNUNCIATOR_SetField (pMe->m_pIAnn, ANNUN_FIELD_FMRADIO/*ANNUN_FIELD_QQ*/, ANNUN_STATE_QQ_MSG_OFF);
     IANNUNCIATOR_SetField (pMe->m_pIAnn, ANNUN_FIELD_FMRADIO/*ANNUN_FIELD_QQ*/, ANNUN_STATE_QQ_LEAVE_OFF); 
     IANNUNCIATOR_SetField (pMe->m_pIAnn, ANNUN_FIELD_FMRADIO/*ANNUN_FIELD_QQ*/, ANNUN_STATE_QQ_HIDING_OFF);

        switch(pMe->m_qqstate)
               {               
                 case QQSTATE_ONLINE:
                      IANNUNCIATOR_SetField (pMe->m_pIAnn, ANNUN_FIELD_FMRADIO/*ANNUN_FIELD_QQ*/, ANNUN_STATE_QQ_ONLINE);
                      break;  
                 case QQSTATE_AWAYLINE:
                      IANNUNCIATOR_SetField (pMe->m_pIAnn, ANNUN_FIELD_FMRADIO/*ANNUN_FIELD_QQ*/, ANNUN_STATE_QQ_LEAVE_ON);
                      break;
                 case QQSTATE_OFFLINE:
                      break;
                 case QQSTATE_INVISIBLE:                   
                      IANNUNCIATOR_SetField (pMe->m_pIAnn, ANNUN_FIELD_FMRADIO/*ANNUN_FIELD_QQ*/, ANNUN_STATE_QQ_HIDING_ON);
                      break;  
                 case QQSTATE_HAVEMSG:
                      IANNUNCIATOR_SetField (pMe->m_pIAnn, ANNUN_FIELD_FMRADIO/*ANNUN_FIELD_QQ*/, ANNUN_STATE_QQ_MSG_ON);
                      break;
                 case QQSTATE_QUIT:
                     break;          
               }
#endif        
    }
    MSG_FATAL("alertType=%d",alertType,0,0);
    switch(alertType)
    {
        case OEMNV_PROFILE_QUIETMODE:           //OEMNV_ALERTTYPE_OFF :
            iann = ANNUN_STATE_RINGTONE_SILENT;//ANNUN_FIELD_SILENT;
            break;

        case OEMNV_PROFILE_NORMALMODE:         //OEMNV_ALERTTYPE_RINGER :
            iann = ANNUN_STATE_RINGTONE_ALERT;//ANNUN_FIELD_RING;
            break;

        case OEMNV_PROFILE_MEETING:              //OEMNV_ALERTTYPE_VIB :
            iann = ANNUN_STATE_RINGTONE_VIBRATOR;//ANNUN_FIELD_VIBRATE;
            break;

        case OEMNV_PROFILE_NOISEMODE:           //OEMNV_ALERTTYPE_VIBRINGER :
            iann = ANNUN_STATE_RINGTONE_VIBRING;//ANNUN_FIELD_ALERT;
            break;

        case OEMNV_PROFILE_CARMODE:             //OEMNV_ALERTTYPE_VIBANDRINGER :
            iann = ANNUN_STATE_RINGTONE_VIBRING;//ANNUN_FIELD_ALERT;
            break;

        default :
            break;            
    }

	/*
	//Add By zzg 2010_08_31
	{		
	    byte      byte_set[PROFILENUMBER];
	    

	    (void) ICONFIG_GetItem(pMe->m_pConfig,
	                           CFGI_PROFILE_ALERT_TYPE,		//CFGI_PROFILE_SMS_RINGER
	                           byte_set,
	                           sizeof(byte_set));	
        
	    switch(byte_set[alertType])     
	    {
			case OEMNV_SMS_OFF:
			{
				iann = ANNUN_STATE_RINGTONE_SILENT;
				break;
			}
			case OEMNV_SMS_RING:
			{
				iann = ANNUN_STATE_RINGTONE_ALERT;
				break;
			}
			case OEMNV_SMS_VIBONLY:
			{
				iann = ANNUN_STATE_RINGTONE_VIBRATOR;
				break;
			}
			case OEMNV_SMS_RINGVIB:
			{
				iann = ANNUN_STATE_RINGTONE_VIBRING;
				break;
			}
			case OEMNV_ALERTTYPE_VIBANDRINGER:
			{
				iann = ANNUN_STATE_RINGTONE_VIBRING;
				break;
			}
			default:
			{
				break;
			}
	    }
	}                                       
	//Add End
	*/

    
    if(pMe->m_pIAnn != NULL)
    {
        IANNUNCIATOR_SetField (pMe->m_pIAnn, ANNUN_FIELD_RINGTONE, iann);
    }
#ifdef FEATRUE_SET_ANN_FULL_SCREEN
    {
        uint32 state = ANNUN_STATE_OFF;
        uint16 i = 0;
        
        /*
        IANNUNCIATOR_GetField (pMe->m_pIAnn,ANNUN_FIELD_RSSI,&state);
        if(state != ANNUN_STATE_OFF)
        {
            IANNUNCIATOR_SetField (pMe->m_pIAnn, ANNUN_FIELD_RSSI, state);
        }
        */
        // 此处已经设置了全部field的状态，不需要再单独为某个field进行设置
        for (i = ANNUN_FIELD_RSSI; i <=ANNUN_FIELD_BATT; i++) 
        {
            state = ANNUN_STATE_OFF;
            if(pMe->m_pIAnn != NULL)
            {
            IANNUNCIATOR_GetField (pMe->m_pIAnn,i,&state);
            //CORE_ERR("GetField i= %d state= %d",i,state);
            if(state != ANNUN_STATE_OFF)
            {
                IANNUNCIATOR_SetField (pMe->m_pIAnn, i, state);
                }
            }
        }
        /*
        IANNUNCIATOR_GetField (pMe->m_pIAnn,ANNUN_FIELD_BLUETOOTH,&state);
        if(state != ANNUN_STATE_OFF)
        {
            IANNUNCIATOR_SetField (pMe->m_pIAnn, ANNUN_FIELD_BLUETOOTH, state);
        }
        */
    }
#endif
    if((missed_call_icon) && (pMe->m_pIAnn != NULL))
    {
    	#ifdef FEATURE_VERSION_C337
		if (pMe->m_pIRUIM != NULL)
		{
			if(IRUIM_IsCardConnected(pMe->m_pIRUIM))
			{
				IANNUNCIATOR_SetField (pMe->m_pIAnn, ANNUN_FIELD_CALL, ANNUN_STATE_CALL_MISSEDCALL_ON);
			}
		}
		#else
		IANNUNCIATOR_SetField (pMe->m_pIAnn, ANNUN_FIELD_CALL, ANNUN_STATE_CALL_MISSEDCALL_ON);
		#endif
        
    }

	//Add By zzg 2010_11_22
#ifdef FEATURE_APP_BLUETOOTH
	if((bt_status) && (pMe->m_pIAnn != NULL))
    {
        IANNUNCIATOR_SetField (pMe->m_pIAnn, ANNUN_FIELD_BLUETOOTH, ANNUN_STATE_BT_ON);
    }
#endif
	//Add End
}

static void CoreApp_Process_Rtc_Event(CCoreApp *pMe)
{
    static boolean have_run = FALSE;
    if(!have_run)
    {
        uint32 current_time = 0;
        JulianType current_time_ex;
        current_time = GETTIMESECONDS();
        GETJULIANDATE(current_time, &current_time_ex);
        if(current_time_ex.wYear < 2008)
        {
            JulianType *get_rtc_time = NULL;
#ifndef WIN32
            get_rtc_time = (JulianType*)OEMRTC_hs_bsp_test_rtc_get();
#endif//WIN32
            if(get_rtc_time->wYear < 2008)
            {
                return ;
            }
        }
        
#ifdef FEATRUE_AUTO_POWER
#ifndef WIN32
        OEMRTC_Process_Auto_Power_Down(pMe->a.m_pIShell);		
        OEMRTC_Process_Auto_Power_On();
#endif//WIN32
#endif //#ifdef FEATRUE_AUTO_POWER        
        //jignag 20080213 add
        //OEMRTC_Process_PowerDown_Alarm();

#if defined( FEATURE_POWERDOWN_ALARM)
        registerPowerdownAlarmclock();
#endif
        have_run = TRUE;
    }
    return ;
}

//static  boolean CoreApp_PowerAlarm_Event(CCoreApp *pMe)
//{
//
//        uint32 current_time = 0;
//        JulianType current_time_ex;
//        current_time = GETTIMESECONDS();
//        GETJULIANDATE(current_time, &current_time_ex);
//        if(current_time_ex.wYear < 2008)
//        {
//            JulianType *get_rtc_time = NULL;
//            get_rtc_time = (JulianType*)OEMRTC_hs_bsp_test_rtc_get();
//            if(get_rtc_time->wYear < 2008)
//            {
//                return FALSE;
//            }
//        }
//        OEMRTC_hs_bsp_test_rtc_set((void *)&current_time_ex);
//
//        OEMRTC_Process_PowerDown_Alarm();
//}
#if 0
/*static void CoreApp_GetRecordCount(CCoreApp *pMe)
{
    OEMCallHistoryEntry  ent;
    ICallList      *m_pCallList = NULL;
    IANNUNCIATOR_SetField (pMe->m_pIAnn, ANNUN_FIELD_MISSEDCALL,ANNUN_STATE_OFF);
    //CORE_ERR("Count1");
    if (AEE_SUCCESS == ISHELL_CreateInstance(pMe->a.m_pIShell, AEECLSID_CALLLIST, (void **)&m_pCallList))
    {
        //CORE_ERR("Count2");
        if( ICALLLIST_GetEntry(m_pCallList, CALLHISTORY_MISSED_CATEGORY,0,&ent))
        {
            //CORE_ERR("Count3");
            IANNUNCIATOR_SetField (pMe->m_pIAnn, ANNUN_FIELD_MISSEDCALL,ANNUN_STATE_ON);
        }
        ICALLLIST_Release(m_pCallList);
        m_pCallList = NULL;
    }
}*/
#endif
void CoreApp_Poweroff_Phone(void *pp)
{
    CCoreApp *pMe = (CCoreApp *)pp;
    IBacklight   *pBacklight = NULL;
    boolean b_FMBackground = FALSE;
    
    EnableUIKeys(FALSE);
    uisnd_snd_stop();
    uisnd_multi_stop();
    uisnd_tone_stop();
    
    ISHELL_CreateInstance(pMe->a.m_pIShell,
                         AEECLSID_BACKLIGHT,
                         (void**)&pBacklight);
    if(pBacklight != NULL)
    {
        IBACKLIGHT_TurnOff(pBacklight);
        IBACKLIGHT_Release(pBacklight);
    }
    // 不再关心 IBatt 通知消息
    (void) ISHELL_RegisterNotify(pMe->a.m_pIShell,
                                 AEECLSID_CORE_APP,
                                 AEECLSID_BATTERYNOTIFIER,
                                 0);
#ifdef FEATURE_ICM
    (void) ISHELL_RegisterNotify(pMe->a.m_pIShell, 
                                 AEECLSID_CORE_APP, 
                                 AEECLSID_CM_NOTIFIER, 
                                 0);
#else
    (void) ISHELL_RegisterNotify(pMe->a.m_pIShell, 
                                 AEECLSID_CORE_APP, 
                                 AEECLSID_PHONENOTIFIER, 
                                 0);
#endif
    ICONFIG_SetItem(pMe->m_pConfig, CFGI_FM_BACKGROUND, &b_FMBackground, sizeof(b_FMBackground));

    // Turn off RSSI indicator
    if(pMe->m_pIAnn != NULL)
    {
        IANNUNCIATOR_SetField (pMe->m_pIAnn, ANNUN_FIELD_RSSI, ANNUN_STATE_OFF);
        IANNUNCIATOR_SetField (pMe->m_pIAnn, ANNUN_FIELD_BATT, ANNUN_STATE_OFF);
    }
    //IANNUNCIATOR_EnableAnnunciatorBar(pMe->m_pIAnn,AEECLSID_DISPLAY1,FALSE);
    //IDISPLAY_UpdateEx(pMe->m_pDisplay, FALSE);
    
    DBGPRINTF("CoreApp_Poweroff_Phone %d",pMe->m_ePowerDownType);
    if(POWERDOWN_NORMAL != pMe->m_ePowerDownType)
    {
        extern void hw_reset( void );
        hw_reset();
    }
    else
    {
#ifdef FEATURE_ICM
        ICM_SetOperatingMode(pMe->m_pCM, AEECM_OPRT_MODE_PWROFF);
#else
        IPHONECTL_SetOperatingMode(pMe->m_pIPhoneCtl, AEET_OPRT_MODE_PWROFF);
#endif
    }
    CoreTask_SetPwrDnComplete(TRUE);
}


void CoreApp_Draw_Charger_image(void *pp)
{
    CCoreApp *pMe = (CCoreApp *)pp;
    //AEEBatteryChargerStatus status;
    
    //CORE_ERR("%d  %d Charger_image",pMe->m_battery_count,pMe->m_bExtPwrState);
    IDISPLAY_ClearScreen(pMe->m_pDisplay);
    if(pMe->m_battery_count >= CHARGING_FRAME_COUNT)
    {
        pMe->m_battery_count = 0;
    }
#ifndef FEATURE_USES_LOWMEM
    if(NULL != pMe->m_battery_Image)
    {
        IIMAGE_DrawFrame(pMe->m_battery_Image, pMe->m_battery_count, 0, 0);
    }
    IDISPLAY_UpdateEx(pMe->m_pDisplay,FALSE);
#else
    CoreApp_DrawChargeing(pMe, pMe->m_battery_count);
#endif
    pMe->m_battery_count ++; 
    ISHELL_SetTimer(pMe->a.m_pIShell, CHARGING_ANIMATE_RATE,CoreApp_Draw_Charger_image, pMe);
}
#ifdef FEATURE_UTK2
/*==============================================================================
函数：
    CoreApp_CloseRefreshDlgTimer

说明：
    用于发送 EVT_DISPLAYDIALOGTIMEOUT 给 AEECLSID_CORE_APP 通知程序关闭
    当前对话框。

参数：
    pme [in]：指向CoreApp Applet对象结构的指针。该结构包含小程序的特定信息。

返回值：
    无。

备注：
    若 Applet 被挂起，将不做任何操作直接返回。
       
==============================================================================*/
static void CoreApp_CloseRefreshDlgTimer(void *pme)
{
    CCoreApp *pMe = (CCoreApp *)pme;
#ifdef FEATURE_INIT_RUIM_SMSandADD_BYUIMTASK
    db_items_value_type  db_item;
    boolean  bIninted = FALSE;
#endif
    if (NULL == pMe)
    {
        return;
    }
    
    if (pMe->m_bSuspended)
    {
        return;
    }
#ifdef FEATURE_INIT_RUIM_SMSandADD_BYUIMTASK
    db_get(DB_UIMSMSINIT, &db_item);
    bIninted = db_item.db_uimsmsinited;
    db_get(DB_UIMADDINIT, &db_item);
    DBGPRINTF("CoreApp_CloseRefreshDlgTimer %d %d",bIninted,db_item.db_uimaddinited);
    bIninted = (bIninted && db_item.db_uimaddinited);
    if(!bIninted)
    {
        (void)ISHELL_SetTimer(pMe->a.m_pIShell, 
                          500,
                          CoreApp_CloseRefreshDlgTimer, 
                          pMe);
        return;
    }
#endif
	// 发送EVT_DISPLAYDIALOGTIMEOUT事件
    (void) ISHELL_PostEvent(pMe->a.m_pIShell,
                      AEECLSID_CORE_APP,
                      EVT_DISPLAYDIALOGTIMEOUT,
                      0,
                      0);
}

/*==============================================================================
函数：
    IDD_UTKREFRESH_Handler
       
说明：
    IDD_UTKREFRESH 对话框事件处理函数
       
参数：
    pMe [in]：指向CoreApp Applet对象结构的指针。该结构包含小程序的特定信息。
    eCode [in]：事件代码。
    wParam：事件相关数据。
    dwParam：事件相关数据。
       
返回值：
    TRUE：传入事件被处理。
    FALSE：传入事件被忽略。
       
备注：
       
==============================================================================*/
static boolean  IDD_UTKREFRESH_Handler(void *pUser,
                                   AEEEvent   eCode,
                                   uint16     wParam,
                                   uint32     dwParam)
{
    IStatic * pStatic;    
    CCoreApp *pMe = (CCoreApp *)pUser;    
    
    if (NULL == pMe)
    {
        return FALSE;
    }
    
    pStatic = (IStatic*)IDIALOG_GetControl(pMe->m_pActiveDlg, IDC_UTKREFRESH_STATIC);

    if (NULL == pStatic)
    {
        return FALSE;
    }
    
    if (NULL == pStatic)
    {
        return FALSE;
    }
    
    switch (eCode)
    {
        case EVT_DIALOG_INIT:
            // 设置静态文本控件矩形尺寸
            ISTATIC_SetRect(pStatic, &pMe->m_rc);
            
            // 设置静态文本控件属性
            ISTATIC_SetProperties(pStatic, ST_CENTERTEXT | ST_MIDDLETEXT);
            return TRUE;
            
        case EVT_DIALOG_START:
            (void) ISHELL_PostEvent(pMe->a.m_pIShell,
                                    AEECLSID_CORE_APP,
                                    EVT_USER_REDRAW,
                                    0,
                                    0);
            return TRUE;
            
        case EVT_USER_REDRAW:
            {
                AECHAR    wszMsgText[32];
#ifdef FEATURE_INIT_RUIM_SMSandADD_BYUIMTASK
                db_items_value_type db_item;
#endif
                (void) ISHELL_LoadResString(pMe->a.m_pIShell,
                                            AEE_COREAPPRES_LANGFILE,
                                            IDS_UTKREFRESH,
                                            wszMsgText,
                                            sizeof(wszMsgText));
                
                // 设置静态文本控件文本
                (void)ISTATIC_SetText(pStatic, 
                                      NULL, 
                                      wszMsgText, 
                                      AEE_FONT_NORMAL, 
                                      AEE_FONT_NORMAL);

                (void)ISTATIC_Redraw(pStatic);
                
#ifdef FEATURE_CDSMS_RUIM
#if defined(FEATURE_WMS_APP) && !defined(FEATURE_WMSAPP_ONLYSUPPORTVMAIL)
#ifndef WIN32
                // 通知短信刷新卡上短信
                if ((pMe->wRefreshMask == REFRESHMASK_UIMALL) ||
                    (pMe->wRefreshMask & REFRESHMASK_UIMSMS))
                {
                    IWmsApp *pWmsApp=NULL;
#ifdef FEATURE_INIT_RUIM_SMSandADD_BYUIMTASK
                    db_item.db_uimsmsinited = FALSE;
                    db_put(DB_UIMSMSINIT, &db_item);
#endif
                    DBGPRINTF("AEECLSID_WMSAPP");
                    if (ISHELL_CreateInstance(pMe->a.m_pIShell, AEECLSID_WMSAPP,
                            (void**)&pWmsApp) == SUCCESS)
                    {
                        DBGPRINTF("IWmsApp_RefreshRUIMSMS");
                        IWmsApp_RefreshRUIMSMS(pWmsApp);
                        
                        IWmsApp_Release(pWmsApp);
                    }
                }
#endif
#endif
#endif
#ifdef FEATURE_RUIM_PHONEBOOK
                // 先通过释放接口来释放初始化的 Buffer
                if ((pMe->wRefreshMask == REFRESHMASK_UIMALL) ||
                    (pMe->wRefreshMask & REFRESHMASK_UIMADD))
                {
                    extern void OEMRUIMAddr_Refresh(void);
#ifdef FEATURE_INIT_RUIM_SMSandADD_BYUIMTASK
                    db_item.db_uimaddinited = FALSE;
                    db_put(DB_UIMADDINIT, &db_item);
#endif
                    OEMRUIMAddr_Refresh();
                }
#endif

                // 设置自动关闭对话框的定时器
                (void)ISHELL_SetTimer(pMe->a.m_pIShell, 
                                      500,
                                      CoreApp_CloseRefreshDlgTimer, 
                                      pMe);
            }
            
            return TRUE;
            
        case EVT_DIALOG_END:
            return TRUE;
            
        case EVT_DISPLAYDIALOGTIMEOUT:
            CLOSE_DIALOG(DLGRET_OK)
            return TRUE;
        
        case EVT_KEY:
            return TRUE;
        
        default:
            break;
    }
    
    return FALSE;
} // IDD_UTKREFRESH_Handler
#endif /* FEATURE_UTK2 */

#ifdef FEATURE_APP_MUSICPLAYER
/*==============================================================================
函数:
    CoreApp_DrawMusicName

说明:
    绘制背景播放的音乐名字

参数:
    pMe [In]: 指向 Core Applet 对象结构的指针。
       
返回值:
    无。

备注:
    刷新显示操作由调用者根据需要执行相关操作。
    
==============================================================================*/
#if 0
static void CoreApp_DrawMusicName(CCoreApp    *pMe)
{
    AEERect rect;
    AECHAR musicName[DISP_MUSICNAME_MAX + 5] = {0};
    int pos = 0;
    int rnt = 0;

    if(NULL == pMe || MP3STATUS_RUNONBACKGROUND != GetMp3PlayerStatus() || NULL ==pMe->m_pMusicName)
    {
        return ;
    }
    musicName[pos++] = (AECHAR)'[';
    musicName[pos++] = (AECHAR)' ';
    //only display first 20 AECHAR
    if(DISP_MUSICNAME_MAX < WSTRLEN(g_pMp3FileToPlay))
    {
        //'[' take place musicName[0] and "..."take three
        rnt = WSTRLCPY(&musicName[pos], g_pMp3FileToPlay, DISP_MUSICNAME_MAX - 3 + 1);
        pos += (DISP_MUSICNAME_MAX - 3);
        musicName[pos++] = (AECHAR)'.';
        musicName[pos++] = (AECHAR)'.';
        musicName[pos++] = (AECHAR)'.';
    }
    else
    {
        rnt = WSTRLCPY(&musicName[pos], g_pMp3FileToPlay, WSTRLEN(g_pMp3FileToPlay) + 1);
        pos += WSTRLEN(g_pMp3FileToPlay);
    }
    musicName[pos++] = (AECHAR)' ';
    musicName[pos++] = (AECHAR)']';
    musicName[pos] = (AECHAR)'\0';
    
    SETAEERECT(&rect, 0, pMe->m_rc.dy *2/5, pMe->m_rc.dx, pMe->m_nLargeFontHeight); 

    // Display the string
    (void)DrawTextWithProfile(pMe->a.m_pIShell,
                              pMe->m_pDisplay,
                              RGB_WHITE_NO_TRANS,
                              AEE_FONT_NORMAL,
                              musicName, -1,
                              0, 0, &rect, 
                              IDF_ALIGN_CENTER 
                              | IDF_ALIGN_MIDDLE 
                              | IDF_TEXT_TRANSPARENT);
}
#else
static void CoreApp_DrawMusicName(CCoreApp    *pMe,uint16 nIdx)
{
    AEERect rect;
    AEERect oldClip;
    AEERect clip;
    AECHAR bracket[2] = {(AECHAR)'[',(AECHAR)'\0'};
    int m_musicstl = IDISPLAY_MeasureTextEx(pMe->m_pDisplay,
                                      AEE_FONT_NORMAL,
                                      pMe->m_pMusicName+nIdx,
                                      -1,
                                      -1,
                                      NULL);;

    if(NULL == pMe || MP3STATUS_RUNONBACKGROUND != GetMp3PlayerStatus() || NULL == pMe->m_pMusicName)
    {
        return ;
    }
    
    if(pWallPaper != NULL)
    {
    	AEEImageInfo m_ImageInfo;
		IImage_GetInfo(pWallPaper,&m_ImageInfo);
		MSG_FATAL("m_ImageInfo.cx=%d, m_ImageInfo.cy=%d", m_ImageInfo.cx, m_ImageInfo.cy, 0);
        SETAEERECT(&clip, 0, MUSIC_WIDTH, pMe->m_rc.dx, pMe->m_nLargeFontHeight + 4); 
        IDISPLAY_GetClipRect( pMe->m_pDisplay, &oldClip);
        IDISPLAY_SetClipRect( pMe->m_pDisplay, &clip);
        MSG_FATAL("clip.x=%d, clip.y=%d,pMe->m_rc.dx=%d", clip.x, clip.y, pMe->m_rc.dx);
        IIMAGE_SetOffset( pWallPaper, ( m_ImageInfo.cx -  pMe->m_rc.dx)/2,MUSIC_WIDTH-( pMe->m_rc.dy - m_ImageInfo.cy)/2);
        //IIMAGE_SetOffset( pWallPaper, 0,MUSIC_WIDTH);
        MSG_FATAL("clip.dx=%d, clip.dy=%d", clip.dx, clip.dy, 0);
		MSG_FATAL("pMe->m_rc.dy=%d", pMe->m_rc.dy,0,0);
        IIMAGE_SetDrawSize( pWallPaper, clip.dx,clip.dy);
        IIMAGE_Draw( pWallPaper, 0,MUSIC_WIDTH);
        IDISPLAY_SetClipRect( pMe->m_pDisplay,&oldClip);
        IIMAGE_SetOffset( pWallPaper, 0,0);
        //IIMAGE_SetDrawSize( pWallPaper, pMe->m_rc.dx,pMe->m_rc.dy);
        IIMAGE_SetDrawSize( pWallPaper, m_ImageInfo.cx,m_ImageInfo.cy);
        
		
    }
    MSG_FATAL("rect.x=%d,w=%d", pMe->m_rc.dx/6, pMe->m_nLargeFontHeight, 0);
    SETAEERECT(&rect, pMe->m_rc.dx/6, MUSIC_START_WIDTH, pMe->m_nLargeFontHeight, pMe->m_nLargeFontHeight);
    IDISPLAY_SetColor(pMe->m_pDisplay,CLR_USER_TEXT,MAKE_RGB(60, 128, 196));
     // Display the string
     //dele by yangdecai 
    /*(void)DrawTextWithProfile(pMe->a.m_pIShell,
                              pMe->m_pDisplay,
                              RGB_WHITE_NO_TRANS,
                              AEE_FONT_BOLD,
                              bracket, -1,
                              0, 0, &rect, 
                              IDF_ALIGN_LEFT
                              | IDF_ALIGN_MIDDLE 
                              | IDF_TEXT_TRANSPARENT);*/
   //bracket[0]=(AECHAR)']';
   MSG_FATAL("rect.x=%d,w=%d", pMe->m_rc.dx*5/6, pMe->m_nLargeFontHeight, 0);
   SETAEERECT(&rect, pMe->m_rc.dx*5/6, MUSIC_START_WIDTH, pMe->m_nLargeFontHeight, pMe->m_nLargeFontHeight);
    // Display the string
    //dele by yangdecai
   /*(void)DrawTextWithProfile(pMe->a.m_pIShell,
                              pMe->m_pDisplay,
                              RGB_WHITE_NO_TRANS,
                              AEE_FONT_BOLD,
                              bracket, -1,
                              0, 0, &rect, 
                              IDF_ALIGN_LEFT
                              | IDF_ALIGN_MIDDLE 
                              | IDF_TEXT_TRANSPARENT);*/
  MSG_FATAL("m_musicstl====%d",m_musicstl,0,0);
  if(m_musicstl>(pMe->m_rc.dx*2/3-2*DISP_BLANK_WIDTH))
  {
  	m_musicstl=(pMe->m_rc.dx*2/3-2*DISP_BLANK_WIDTH);
  }
  SETAEERECT(&rect, (pMe->m_rc.dx/6 /*((pMe->m_rc.dx*3/4)-m_musicstl)/2*/), MUSIC_START_WIDTH,(pMe->m_rc.dx*2/3), pMe->m_nLargeFontHeight);
  MSG_FATAL("rect.x=%d,w=%d,rect.dy=%d", (pMe->m_rc.dx/6 + pMe->m_nLargeFontHeight), (pMe->m_rc.dx*2/3 - 2*DISP_BLANK_WIDTH), rect.dy);
  #ifdef FEATURE_VERSION_MYANMAR
  {
	  AECHAR M_usicname[128] = {0};
	  int Musicname = WSTRLEN(pMe->m_pMusicName+nIdx);
	  WSTRNCOPY(M_usicname, Musicname-3, pMe->m_pMusicName+nIdx);
	  (void)DrawTextWithProfile(pMe->a.m_pIShell,
	                              pMe->m_pDisplay,
	                              RGB_WHITE_NO_TRANS,
	                              AEE_FONT_BOLD,
	                              M_usicname, -1,
	                              0, 0, &rect, 
	                              IDF_ALIGN_CENTER
	                              | IDF_ALIGN_MIDDLE 
	                              | IDF_TEXT_TRANSPARENT);
  }
  #else
  {
	  (void)DrawTextWithProfile(pMe->a.m_pIShell,
	                              pMe->m_pDisplay,
	                              RGB_WHITE_NO_TRANS,
	                              AEE_FONT_BOLD,
	                              pMe->m_pMusicName+nIdx, -1,
	                              0, 0, &rect, 
	                              IDF_ALIGN_CENTER
	                              | IDF_ALIGN_MIDDLE 
	                              | IDF_TEXT_TRANSPARENT);
  }
  #endif
  IDISPLAY_SetColor(pMe->m_pDisplay,CLR_USER_TEXT,CLR_SYS_TITLE_TEXT);
}
#endif
/*滚动显示当前正在播放的歌曲名称*/
static void CoreApp_MusicNameAutoScroll(CCoreApp *pMe)
{
    int nIdx,nIdxNew,n;
    nIdx  = nIdxNew = pMe->m_nAutoScrollIdx;
    n = WSTRLEN(pMe->m_pMusicName);

    if ((ISHELL_ActiveApplet(pMe->a.m_pIShell) != AEECLSID_CORE_APP) ||
                        (pMe->m_wActiveDlgID != IDD_IDLE))
    {
        ISHELL_CancelTimer(pMe->a.m_pIShell,(PFNNOTIFY)CoreApp_MusicNameAutoScroll,pMe);
        return; 
    }
    if(nIdx < n)
    {
        // Measure the text starting from the auto-scroll offset
        if(Core_bMusicNameRequiredScroll(pMe, nIdx))
        {
            nIdxNew++;
        }
        else
        {
            nIdxNew = 0;
        }
        CoreApp_DrawMusicName(pMe,nIdx);
        IDISPLAY_Update(pMe->m_pDisplay);
        //(void)ISHELL_PostEvent(pMe->a.m_pIShell,
        //                       AEECLSID_CORE_APP,
        //                       EVT_UPDATEIDLE,
        //                       0,
        //                       0);
    }
    else
    {
        nIdxNew = 0;
    }

    pMe->m_nAutoScrollIdx = nIdxNew;

    ISHELL_SetTimer( pMe->a.m_pIShell,
                     MUSICNAME_AUTO_SCROLL_TIME,
                    (PFNNOTIFY)CoreApp_MusicNameAutoScroll, 
                     pMe);
}
/*判断是否需要滚动显示*/
static boolean Core_bMusicNameRequiredScroll(CCoreApp *pMe, int nIdx)
{
    int   cx,cxMax;

    cx = IDISPLAY_MeasureText(pMe->m_pDisplay, AEE_FONT_BOLD, pMe->m_pMusicName + nIdx);
   
    cxMax = pMe->m_rc.dx*3/4 - 2*DISP_BLANK_WIDTH; //显示名字矩形筐宽度   
    
    return(cx > cxMax);
}
/*设置是否需要滚动*/
static void Core_DrawNameResetScroll(CCoreApp *pMe)
{
    if(NULL == pMe || MP3STATUS_RUNONBACKGROUND != GetMp3PlayerStatus() || NULL == pMe->m_pMusicName)
    {
        return ;
    }
    if(pMe==NULL)
    {
        return;
    }
    pMe->m_nAutoScrollIdx = 0;
    if(Core_bMusicNameRequiredScroll(pMe,0))
    {
       CoreApp_MusicNameAutoScroll(pMe); 
    }
    else
    {
        ISHELL_CancelTimer(pMe->a.m_pIShell,(PFNNOTIFY)CoreApp_MusicNameAutoScroll,pMe);
        CoreApp_DrawMusicName(pMe,0);  
    }
}
#endif
#ifdef FEATURE_SPN_FROM_BSMCCMNC
static const ServiceProviderList List_SP[] = 
{
#ifdef FEATURE_CARRIER_ISRAEL_PELEPHONE       
       {000,0,"Pelephone"},  
#endif       
#ifdef FEATURE_CARRIER_VENEZUELA_MOVILNET       
       {000,0,"Movilnet"},  
#endif 

#ifdef FEATURE_CARRIER_MEXICO_IUSACELL       
       {000,0,"Iusacell"},  
#endif 

       //{834,1,"China Unicom"}, 
       {809,0,"TRICOM"},
       {671,15,"Guamcell"},             // 关岛
       {310,37,"Guamcell"},             // 关岛
       {454,5,"Hutchison"},             // 香港
       {460,3,"China Unicom"},          // Chinese Unicom
       {466,5,"APBW"},                  // 台湾
       {520,0,"Hutch"},                 // 泰国??  CAT
       {520,2,"CAT"},                   // 泰国
       {425,3,"Pelephone"},             // 以色列
       {604,5,"WANA"},                  // Morocco WANA
       
       {421,3,"Yemen Mobile"},
       {470,6,"CITYCELL"},
       {470,4,"CITYCELL"},
       {470,3,"CITYCELL"},
       {470,2,"CITYCELL"},
       {428,0,"Skytel"},
       
       {302,64,"Bell Mobility"},        // 加拿大
       
       {244,0,"Movicel"},               // Angola Movicel
       {58,0,"Telefonica"},             // Venezuela Movistar
       {51,6,"Telefonica"},             // Peru Movistar
       
       {450,0,"SKT"},                   // 韩国
       {450,3,"SKT"},                   // 韩国
       {450,5,"SKT"},                   // 韩国
       {450,22,"SKT"},                  // 韩国
       
       {505,11,"Telstra"},              // 澳大利亚
       
       {302,0,"Telus"},                 // 加拿大
       {302,11,"Telus"},                // 加拿大
       {302,86,"Telus"},                // 加拿大
       
       {530,02,"TML NZ"},               // 新西兰
       
       {434,06,"Perfectum"},            // Perfectum // 俄语国家
       
       {510,0, "Flexi"},                // Indonesia flexi
       {510,3, "starone"},              // Indonesia starone
       {510,28,"Mobile-8"},             // Indonesia fren
       {510,99,"esia"},                 // Indonesia esia
       {510,9,"SMART"},                 // Indonesia SMART
       
#ifdef FEATURE_CARRIER_ANGOLA_MOVICEL
       {310,0,"Movicel"},               
#else 
       {310,0,"Verizon"},               // 美国
#endif       
       {724,6,"VIVO"},                  //巴西
       {724,8,"VIVO"},                  //巴西
       {724,9,"VIVO"},                  //巴西
       {724,10,"VIVO"},                 //巴西
       {724,11,"VIVO"},                 //巴西
       {724,12,"VIVO"},                 //巴西
       {724,13,"VIVO"},                 //巴西
       {724,14,"VIVO"},                 //巴西
       {724,17,"VIVO"},                 //巴西
       {724,18,"VIVO"},                 //巴西
       {724,19,"VIVO"},                 //巴西
       {724,20,"VIVO"},                 //巴西
       {724,21,"VIVO"},                 //巴西
       {724,22,"VIVO"},                 //巴西
       
       {440,53,"KDDI"},                 //日本
       {440,54,"KDDI"},                 //日本
       {440,70,"KDDI"},                 //日本
       {440,71,"KDDI"},                 //日本
       {440,72,"KDDI"},                 //日本
       {440,73,"KDDI"},                 //日本
       {440,74,"KDDI"},                 //日本
       {440,75,"KDDI"},                 //日本
       {440,76,"KDDI"},                 //日本
       {440,78,"KDDI"},                 //日本
       
       {310,12,"Sprint"},               //美国
       {310,0,"Sprint"},                //美国
       
       {334,0,"Iusacell"},              //墨西哥
       
       {450,6,"LGT"},                   //韩国
       
       {452,3,"Stel"},                  //越南
       
       {455,2,"MacauUnicom"},           //澳门
       
       {111,1,"Syniverse"},             //信令转接商
       
       {404,0,"Reliance"},              //印度   
       {404,1,"Aircell Digilink"},   
       {404,2,"Bharti Mobile"},   
       {404,3,"Bharti Telenet"},   
       {404,4,"Idea Cellular"},   
       {404,5,"Fascel"},   
       {404,6,"Bharti Mobile"},   
       {404,7,"Idea Cellular"},   
       {404,9,"Reliance Telecom"},   
       {404,10,"Bharti Cellular"},   
       {404,11,"Sterling Cellular"},   
       {404,12,"Escotel Mobile"},   
       {404,13,"Hutchinson Essar South"},   
       {404,14,"Spice"},   
       {404,15,"Aircell Digilink"},
       {404,16,"Hexcom"},
       {404,18,"Reliance Telecom"},
       {404,19,"Escotel Mobile"},
       {255,04,"Inter"}, //intertelecom
       {255,23,"CDMA Ukraine"} //CDMA Ukraine       
       
};
#endif

#ifdef FEATURE_VERSION_C11
static void CCoreApp_TorchTipTimeOut(CCoreApp *pMe)
{
    if (NULL == pMe)
    {
        return;
    }

    (void) ISHELL_PostEvent(pMe->a.m_pIShell,
                            AEECLSID_CORE_APP,
                            EVT_USER_REDRAW,
                            0,
                            0);
}
#endif

static void CoreApp_GetSPN(CCoreApp *pMe)
{

#ifndef WIN32
#if defined FEATURE_CARRIER_THAILAND_CAT
    MEMSET(pMe->svc_p_name,0,sizeof(pMe->svc_p_name));
    return;
#elif ((defined(FEATURE_CARRIER_THAILAND_HUTCH))&&(!(defined(FEATURE_CARRIER_THAILAND_CAT_FACE))))
    MEMSET(pMe->svc_p_name,0,(UIM_CDMA_HOME_SERVICE_SIZE+1)*sizeof(AECHAR));
    ICONFIG_GetItem(pMe->m_pConfig, CFGI_BANNER, pMe->svc_p_name, (NV_MAX_LTRS+1)*sizeof(AECHAR));
    return;
#endif

#ifdef FEATURE_SPN_FROM_BSMCCMNC
    /*1. Represent the 3-digit Mobile Country Code as D1 D2 D3 with the digit equal to zero
        being given the value of ten.
     2. Compute 100 × D1 + 10 × D2 + D3 - 111.
    */
    //Get MCC&MNC codeS
    extern cdma_bs_type *cur_bs_ptr;
    boolean   ServNameinNv = FALSE;
    word curr_mcc;
    byte curr_mnc;

    if(NULL == cur_bs_ptr)
    {
        (void) ISHELL_LoadResString(pMe->a.m_pIShell,
                        AEE_COREAPPRES_LANGFILE,
                        IDS_UNKNOWN_NETWORK,
                        pMe->svc_p_name,
                        sizeof(pMe->svc_p_name));   
        return;
    }
    curr_mcc = cur_bs_ptr->csp.esp.mcc + 111;
    curr_mnc = cur_bs_ptr->csp.esp.imsi_11_12 + 11; 

    //ERR("BS mcc(%d), BS mnc(%d)",cur_bs_ptr->esp.mcc,cur_bs_ptr->esp.imsi_11_12,0);

    if ( curr_mcc % 10 == 0 )
    {
       curr_mcc -= 10;
    }
    if ( (curr_mcc/10) % 10 == 0 )
    {
       curr_mcc -= 100;
    }
    if ( curr_mcc >= 1000 )
    {
       curr_mcc -= 1000; 
    } 

    if ( curr_mnc % 10 == 0 )
    {
       curr_mnc -= 10;
    }
    if ( curr_mnc >= 100 )
    {
       curr_mnc -= 100;
    } 

    //Load name string of service provider
    if ( 460 == curr_mcc && 3 == curr_mnc ) 
    {
       (void) ISHELL_LoadResString(pMe->a.m_pIShell,
                       AEE_COREAPPRES_LANGFILE,
                       IDS_CHINA_TELECOM,
                       pMe->svc_p_name,
                       sizeof(pMe->svc_p_name));   
    }
    else
    {
       // RUIM's mcc,mnc are different from the ones from the BS
       // look for the SP list
       int i;
       int len = sizeof(List_SP)/sizeof(ServiceProviderList);
       EmergencyNum_Table m_entable = {0};
       char m_vmnuber[10] = {0};
       
       if((curr_mnc == 0) && (curr_mcc == 520))  //add by yangdecai 2010-1124
       {
       	   //Hutch
       	   STRCPY(m_vmnuber,"91122");
       	   m_entable.emert_size = OEMNV_EMERT_SEZE;
       	   m_entable.emerg_num[0].num_len = OEMNV_EMERG_NUM_LEN;
       	   STRCPY(m_entable.emerg_num[0].num_buf,OEMNV_EMERG_NUM_ONE);
       	   (void)OEM_SetConfig(CFGI_EMERGENCYNUM_TABLE,
	                          (void*)&m_entable,
	                          sizeof(EmergencyNum_Table));
	       (void)OEM_SetConfig(CFGI_CALLFORWARD_VOICEMAIL_ENABLE,
	                          (void*)m_vmnuber,
	                          sizeof(m_vmnuber));
       }
       else if((curr_mnc == 2) && (curr_mcc == 520))
       {
       	   //CAT
       	   STRCPY(m_vmnuber,"91100");
       	   m_entable.emert_size = OEMNV_EMERT_SEZE;
       	   m_entable.emerg_num[0].num_len = OEMNV_EMERG_NUM_LEN;
       	   STRCPY(m_entable.emerg_num[0].num_buf,OEMNV_EMERG_NUM_TWO);
       	   (void)OEM_SetConfig(CFGI_EMERGENCYNUM_TABLE,
	                          (void*)&m_entable,
	                          sizeof(EmergencyNum_Table));
       	   (void)OEM_SetConfig(CFGI_CALLFORWARD_VOICEMAIL_ENABLE,
	                          (void*)m_vmnuber,
	                          sizeof(m_vmnuber));
       	   
       }
       for (i = 0; i < len; i++)
       {
          if ((List_SP[i].mcc == curr_mcc) && (List_SP[i].mnc == curr_mnc))
          {
             STRTOWSTR ( List_SP[i].ids_name, pMe->svc_p_name, sizeof(pMe->svc_p_name) );
             ServNameinNv = TRUE;
             //ERR("RUIM mcc == BS mcc ",0,0,0);
             break;
          }
       }
       if(!ServNameinNv)
        {
            AECHAR szFormat[16]; 
            STR_TO_WSTR("%d/%d  ", szFormat, sizeof(szFormat));
            WSPRINTF(pMe->svc_p_name, sizeof(pMe->svc_p_name), szFormat, curr_mcc, curr_mnc);
        }
    }
#else //FEATURE_SPN_FROM_BSMCCMNC    

#if(! defined FEATURE_CARRIER_ISRAEL_PELEPHONE) && (! defined FEATURE_LANG_HEBREW)
    if(pMe->svc_p_name[0] == 0)
#endif
    {
#ifdef FEATURE_VERSION_W208S
#ifdef FEATURE_UIM
		if (TRUE)
#else
		if (FALSE)
#endif			
#else
		if(IsRunAsUIMVersion())
#endif     
        
        {
            static int SPNRetry = 5;
            if(EFAILED == IRUIM_Read_Svc_P_Name(pMe->m_pIRUIM,pMe->svc_p_name))
            {
                if(SPNRetry)
                {
                    SPNRetry--;
                    if(SPNRetry)
                    {
                        return;
                    }
                }
                
                MEMSET(pMe->svc_p_name,0,(UIM_CDMA_HOME_SERVICE_SIZE+1)*sizeof(AECHAR));
#if(defined FEATURE_CARRIER_ISRAEL_PELEPHONE) && (defined FEATURE_LANG_HEBREW)
                nv_language_enum_type sel = 0;
                ICONFIG_GetItem(pMe->m_pConfig, CFGI_LANGUAGE_SELECTION, &sel, sizeof(sel));
                if(sel ==  NV_LANGUAGE_HEBREW)
                {
                    ISHELL_LoadResString(pMe->a.m_pIShell,AEE_COREAPPRES_LANGFILE, IDS_ISRAEL_BANNER, pMe->svc_p_name, sizeof(pMe->svc_p_name));
                }
                else
#endif
                {
                    ICONFIG_GetItem(pMe->m_pConfig, CFGI_BANNER, pMe->svc_p_name, (NV_MAX_LTRS+1)*sizeof(AECHAR));
                }
            }        
        }
        else
        {
            MEMSET(pMe->svc_p_name,0,(UIM_CDMA_HOME_SERVICE_SIZE+1)*sizeof(AECHAR));
            {
#if(defined FEATURE_CARRIER_ISRAEL_PELEPHONE) && (defined FEATURE_LANG_HEBREW)
                nv_language_enum_type sel = 0;
                ICONFIG_GetItem(pMe->m_pConfig, CFGI_LANGUAGE_SELECTION, &sel, sizeof(sel));
                if(sel ==  NV_LANGUAGE_HEBREW)
                {
                    ISHELL_LoadResString(pMe->a.m_pIShell,AEE_COREAPPRES_LANGFILE, IDS_ISRAEL_BANNER, pMe->svc_p_name, sizeof(pMe->svc_p_name));
                }
                else
#endif
                {
                    ICONFIG_GetItem(pMe->m_pConfig, CFGI_BANNER, pMe->svc_p_name, (NV_MAX_LTRS+1)*sizeof(AECHAR));
                }
            }
        }
    }
#endif //FEATURE_SPN_FROM_BSMCCMNC   
#endif//WIN32
}
static void CoreApp_Issametimer(void *pUser)
{
	CCoreApp	*pMe = (CCoreApp *)pUser;
	pMe->m_IsSametime = FALSE;
	
}

#ifdef FEATURE_KEYGUARD
static void CoreApp_keypadtimer(void *pUser)
{
	CCoreApp *pMe = (CCoreApp *)pUser;
	int ret = 0;
	pMe->m_iskeypadtime = FALSE;
	#if defined	(FEATURE_VERSION_FLEXI203) 
#ifdef FEATURE_FLEXI_STATIC_BREW_APP				
#if defined (FEATURE_NASRANI)
    OEM_SetBAM_ADSAccount(STATIC_BREW_APP_FLEXI_NASRANI);
    ret=  CoreApp_LaunchApplet(pMe, AEECLSID_NASRANI);
#elif defined  (FEATURE_GURU)
	OEM_SetBAM_ADSAccount(STATIC_BREW_APP_FLEXI_MUSLIM);
	ret=  CoreApp_LaunchApplet(pMe, AEECLSID_FLEXIGURU);
#elif defined (FEATURE_FMN2010)
    OEM_SetBAM_ADSAccount(STATIC_BREW_APP_FLEXI_MUSLIM);
    ret=  CoreApp_LaunchApplet(pMe, AEECLSID_MUSLIM);
#elif defined (FEATURE_FPT005)
	ret= CoreApp_LaunchApplet(pMe, AEECLSID_APP_CONTACT);
#else
	ret= CoreApp_LaunchApplet(pMe, AEECLSID_WMSAPP);
#endif /*FEATURE_NASRANI*/
#else
    ret= CoreApp_LaunchApplet(pMe, AEECLSID_WMSAPP);
#endif  /*FEATURE_FLEXI_STATIC_BREW_APP*/
#elif defined (FEATURE_VERSION_IVIO203)||defined (FEATURE_VERSION_C500BE)
	ret= CoreApp_LaunchApplet(pMe, AEECLSID_WMSAPP);
#elif defined (FEATURE_VERSION_FLEXI203P)
	ret= CoreApp_LaunchApplet(pMe, AEECLSID_MAIN_MENU);
#elif defined (FEATURE_VERSION_SMART)
#ifdef FEATURE_SMARTFREN_STATIC_BREW_APP	
	OEM_SetBAM_ADSAccount(STATIC_BREW_APP_SMARTFREN_FACEBOOK);
	ret= CoreApp_LaunchApplet(pMe, AEECLSID_SMARTFREN_FACEBOOK);
#else
	ret= CoreApp_LaunchApplet(pMe, AEECLSID_WMSAPP);
#endif /*FEATURE_SMARTFREN_STATIC_BREW_APP*/
#elif defined (FEATURE_VERSION_M8021)
#ifdef FEATURE_SMARTFREN_STATIC_BREW_APP
	OEM_SetBAM_ADSAccount(STATIC_BREW_APP_SMARTFREN_FACEBOOK);
	ret= CoreApp_LaunchApplet(pMe, AEECLSID_SMARTFREN_FACEBOOK);
#else
	ret= CoreApp_LaunchApplet(pMe, AEECLSID_WMSAPP);
#endif	/*FEATURE_SMARTFREN_STATIC_BREW_APP*/
#elif defined (FEATURE_VERSION_M8)
#ifdef FEATURE_SMARTFREN_STATIC_BREW_APP	
	OEM_SetBAM_ADSAccount(STATIC_BREW_APP_SMARTFREN_FACEBOOK);
	ret= CoreApp_LaunchApplet(pMe, AEECLSID_SMARTFREN_FACEBOOK);
#else
	ret= CoreApp_LaunchApplet(pMe, AEECLSID_WMSAPP);
#endif	/*FEATURE_SMARTFREN_STATIC_BREW_APP*/
#elif defined (FEATURE_VERSION_M8P)
#ifdef FEATURE_SMARTFREN_STATIC_BREW_APP	
	OEM_SetBAM_ADSAccount(STATIC_BREW_APP_SMARTFREN_FACEBOOK);
	ret= CoreApp_LaunchApplet(pMe, AEECLSID_SMARTFREN_FACEBOOK_HITZ233C);
#else
	ret= CoreApp_LaunchApplet(pMe, AEECLSID_WMSAPP);
#endif	/*FEATURE_SMARTFREN_STATIC_BREW_APP*/
#elif defined(FEATURE_VERSION_HITZ181)||defined(FEATURE_VERSION_MTM)
	ret= CoreApp_LaunchApplet(pMe, AEECLSID_MAIN_MENU);
#elif defined (FEATURE_VERSION_C01)
	ret= CoreApp_LaunchApplet(pMe, AEECLSID_MAIN_MENU);
#elif defined (FEATURE_VERSION_W515V3) || defined(FEATURE_VERSION_S1000T)
	ret= CoreApp_LaunchApplet(pMe, AEECLSID_MAIN_MENU);
#else
	ret= CoreApp_LaunchApplet(pMe, AEECLSID_WMSAPP);
#endif
}
#endif
