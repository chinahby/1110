/** ----------------------------------------------------------------------------
 * MainMenu.c
 *
 * Copyright(c) 2007 Anylook
 *
 *
 * History:
 *
 * when         who            what and why
 * -----------  -----------    -----------------------------
 * 2008.12.29            create  3 X 4 large icon style menu  &  list menu
 *
 * -----------------------------------------------------------------------------
 */


#include "customer.h"
#include "OEMFeatures.h"
#include "OEMClassIds.h"
#ifndef AEECLSID_MAIN_MENU
    #error AEECLSID_MAIN_MENU must be defined
#endif

#include "AEEShell.h"
#include "AEE_OEM.h"
#include "AEEModGen.h"
#include "AEEModTable.h"
#include "AEEMenu.h"
#include "AEEStdLib.h"
#include "AEEAnnunciator.h"
#include "mainmenu.h"
#include "mainmenu_priv.h" 
#include "msg.h"
#include "ContApp.h"
#include "OEMNV.h"
#include "OEMSVC.h"
#include "OEMCFGI.h"

//Add By zzg 2013_09_09
#ifdef FEATURE_APP_MUSICPLAYER
#include "MusicPlayer.h"
#endif 
//Add End

#include "Appscommon.h"
#include "appscommon.brh"
#include "appscommonimages.brh"
#ifdef FEATURE_RANDOM_MENU_REND
#include "DisplayRendUtils.h"
#endif
#if !defined( AEE_SIMULATOR)
#include "AEERUIM.h" 
#include "OEMCFGI.h"
#endif //#if !defined( AEE_SIMULATOR)
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
static int m_row = 0;
static StartInfo start_info;	
boolean m_main_avk_star = FALSE;
#ifdef FEATURE_LCD_TOUCH_ENABLE
#define PARAM_NOT_REF(x)
/*==============================================================================

                              本地函数声明

==============================================================================*/
static MainMenuMod gMainMenuMod;
static const VTBL( IModule) gModFuncs =
{
    MainMenuMod_AddRef,
    MainMenuMod_Release,
    MainMenuMod_CreateInstance,
    MainMenuMod_FreeResources
};

static uint32  MainMenu_AddRef( IMainMenu *pi);

static uint32  MainMenu_Release ( IMainMenu *pi);

static boolean MainMenu_HandleEvent( IMainMenu *pi,
    AEEEvent eCode,
    uint16  wParam,
    uint32 dwParam
);

static MainMenu gMainMenu={0};

static const VTBL( IMainMenu) gMainMenuMethods =
{
    MainMenu_AddRef,
    MainMenu_Release,
    MainMenu_HandleEvent
};

int MainMenuMod_Load( IShell *pIShell, void *ph, IModule **ppMod);

int MainMenuMod_New( int16 nSize,
    IShell              *pIShell,
    void                *ph,
    IModule             **ppMod,
    PFNMODCREATEINST    pfnMC,
    PFNFREEMODDATA      pfnMF
);

static int MainMenu_New( IShell *ps, IModule *pIModule, IMainMenu **ppObj);

static int CMainMenu_InitAppData(MainMenu *pMe);

static void CMainMenu_FreeAppData(MainMenu *pMe);

static void MainMenu_RunFSM(MainMenu *pMe);

static void CalculateScreenParameters(MainMenu *pMe);

static void MainMenu_DrawBackGround(MainMenu *pMe, AEERect *pRect);
//static void MainMenu_DrawSelectIcon(MainMenu *pMe);
static void MovePen(MainMenu * pMe,int dx);
// 初始整个背景及全部初始图标
static void DrawMatrix(MainMenu *pMe);
static void DrawFocusIcon(MainMenu *pMe);
static void MoveCursorTo(MainMenu *pMe, int row, int column);
static void DrawMatrixBottom(MainMenu *pMe);
//static void DrawMatrixBottomStr(MainMenu *pMe);
static void DrawMatrixStr(MainMenu *pMe);
static void AutoMovePage(MainMenu *pMe);
//移动后选中移动
//移动后整个界面显示
static void DrawMatrixMove(MainMenu *pMe,int dx);
static void DrawFocusIconMove(MainMenu *pMe,int dx);
static void MoveCursoToMove(MainMenu *pMe, int row, int column,int dx);
static void DrawMatrixStr_Move(MainMenu *pMe,int dx);

static void SetCursor(MainMenu *pMe, int row, int column);

static int StartApplet(MainMenu *pMe, int i);

boolean MainMenu_RouteDialogEvt(MainMenu *pMe,
    AEEEvent    eCode,
    uint16  wParam,
    uint32 dwParam
);

NextFSMAction MainMenu_ProcessState(MainMenu *pMe);

void MainMenu_ShowDialog(MainMenu  *pMe,  uint16 dlgResId);

static boolean  gbMainmenuLock = FALSE;

static void Main_keypadtimer(void *pUser);


// MAINST_MAIN 状态处理函数
static NextFSMAction MAINST_MAIN_Handler(MainMenu *pMe);
//MAINST_EXIT  状态处理函数
static NextFSMAction MAINST_EXIT_Handler(MainMenu *pMe);
static boolean  MainMenu_IconMenuHandler(MainMenu *pMe, AEEEvent eCode, uint16 wParam, uint32 dwParam);

/*==============================================================================
                              
                              本地（静态）数据
                              
==============================================================================*/
static char* ICON_ANI[] =
{

    ICONBACK1_ANI,
   // 10,
};


static char* ICON_ANI_1[] =
{
    ICONFOUCS_ANI,
};


/*=============================================================================
FUNCTION:  MainMenuMod_Load

DESCRIPTION: 

PARAMETERS:

RETURN VALUE:

COMMENTS:

SIDE EFFECTS:

SEE ALSO:

=============================================================================*/
int MainMenuMod_Load( IShell *pIShell, void *ph, IModule **ppMod)
{	
    return MainMenuMod_New( sizeof( MainMenuMod),
                    pIShell,
                    ph,
                    ppMod,
                    NULL,
                    NULL);
}

/*=============================================================================
FUNCTION:  MainMenuMod_New

DESCRIPTION: 

PARAMETERS:

RETURN VALUE:

COMMENTS:

SIDE EFFECTS:

SEE ALSO:

=============================================================================*/
int  MainMenuMod_New( int16 nSize,
    IShell   *pIShell,
    void     *ph,
    IModule  **ppMod,
    PFNMODCREATEINST pfnMC,
    PFNFREEMODDATA pfnMF
)
{
#if !defined( AEE_SIMULATOR)
    PARAM_NOT_REF( nSize)
    PARAM_NOT_REF( pfnMC)
    PARAM_NOT_REF( pfnMF)
    PARAM_NOT_REF( ph)
#endif

    if( NULL == ppMod)
    {
        return EFAILED;
    }

    MEMSET( &gMainMenuMod, 0, sizeof( MainMenuMod));

    INIT_VTBL( &gMainMenuMod, IModule, gModFuncs);
    gMainMenuMod.referenceCounter = 1;
    *ppMod = ( IModule *)&gMainMenuMod;
    return AEE_SUCCESS;
}

/*=============================================================================
FUNCTION:  MainMenuMod_AddRef

DESCRIPTION: 

PARAMETERS:

RETURN VALUE:

COMMENTS:

SIDE EFFECTS:

SEE ALSO:

=============================================================================*/
static uint32 MainMenuMod_AddRef( IModule *po)
{
    return( ++( ( MainMenuMod *)po)->referenceCounter);
}

/*=============================================================================
FUNCTION:  MainMenuMod_Release

DESCRIPTION: 

PARAMETERS:

RETURN VALUE:

COMMENTS:

SIDE EFFECTS:

SEE ALSO:

=============================================================================*/
static uint32 MainMenuMod_Release( IModule *po)
{
    if ( ( ( MainMenuMod *) po)->referenceCounter == 0)
    {
        return 0;
    }

    return( --( ( MainMenuMod *)po)->referenceCounter);
}

/*=============================================================================
FUNCTION:  MainMenuMod_CreateInstance

DESCRIPTION: 

PARAMETERS:

RETURN VALUE:

COMMENTS:

SIDE EFFECTS:

SEE ALSO:

=============================================================================*/
static int  MainMenuMod_CreateInstance( IModule *po,
    IShell *pIShell,
    AEECLSID ClsId,
    void **ppObj
)
{
    *ppObj = NULL;

    if( ClsId != AEECLSID_MAIN_MENU || MainMenu_New( pIShell, po, ( IMainMenu**)ppObj) != SUCCESS)
    {
        return EFAILED;
    }

    return SUCCESS;
}

/*=============================================================================
FUNCTION:  MainMenuMod_FreeResources

DESCRIPTION: 

PARAMETERS:

RETURN VALUE:

COMMENTS:

SIDE EFFECTS:

SEE ALSO:

=============================================================================*/
static void MainMenuMod_FreeResources( IModule *po, IHeap *ph, IFileMgr *pfm)
{
}

/*=============================================================================
FUNCTION:  MainMenu_New

DESCRIPTION: 

PARAMETERS:

RETURN VALUE:

COMMENTS:

SIDE EFFECTS:

SEE ALSO:

=============================================================================*/
static int MainMenu_New( IShell *ps, IModule *pIModule, IMainMenu **ppObj)
{
    int retVal = SUCCESS;
    if( 0 == gMainMenu.referenceCounter)
    {

        if( pIModule == NULL || ps == NULL)
        {
            return EFAILED;
        }

        MEMSET(&gMainMenu, 0, sizeof(gMainMenu));
        INIT_VTBL(&gMainMenu, IMainMenu, gMainMenuMethods);

       
        gMainMenu.m_pShell     = ps;
        gMainMenu.m_pModule    = pIModule;
        retVal = CMainMenu_InitAppData(&gMainMenu);
        if(retVal != SUCCESS)
        {
            CMainMenu_FreeAppData((MainMenu*)&gMainMenu);
            return retVal;
        }
        (void) ISHELL_AddRef( ps);
        (void) IMODULE_AddRef( pIModule);
        gMainMenu.referenceCounter  = 0;

        
    }

    ++ gMainMenu.referenceCounter;
    *ppObj = ( IMainMenu*)&gMainMenu;
    
    return retVal;
}

/*==============================================================================
函数:
    CMainMenuApp_InitAppData

说明:
    初始化 MainMenu Applet 结构数据缺省值。

参数:
    pMe [in]: 指向 MainMenu Applet 对象结构的指针。该结构包含小程序的特定信息。

返回值:
    EFAILED: 初始化失败。
    SUCCESS: 初始化成功。

备注:

==============================================================================*/
static int CMainMenu_InitAppData(MainMenu *pMe)
{
    if (NULL == pMe)
    {
        return EFAILED;
    }
	
	pMe->m_nRow        = 1;
    pMe->m_nColumn     = 1;	
	pMe->m_nCurPage    = 0;
	pMe->m_PenPos      = -1;
	pMe->m_bmove       = FALSE;
	pMe->m_PrsentPage  = 0;
    //pMe->m_bDraOver    = FALSE;
	pMe->m_step        = 0;
	pMe->m_bRight      = FALSE;
	pMe->m_bReraw      = FALSE;

    // 接口创建及相关初始化
    if (ISHELL_CreateInstance(pMe->m_pShell, AEECLSID_CONFIG,
            (void **)&pMe->m_pConfig) != SUCCESS)
    {    	
        return EFAILED;
    }

    if( ISHELL_CreateInstance( pMe->m_pShell, AEECLSID_BACKLIGHT, (void **)&pMe->m_pBacklight)!=AEE_SUCCESS)
    {
        return FALSE;
    }
     
    if (ISHELL_CreateInstance(pMe->m_pShell, AEECLSID_DISPLAY, 
            (void **) &pMe->m_pDisplay) != SUCCESS)
    {    	
        return EFAILED;
    }
    
    // 初始化菜单Title
    pMe->m_IconTitle[0]      = IDS_MAIN_MENU_MEDIAGALLERY;
	#if defined (FEATURE_VERSION_C337) || defined(FEATURE_VERSION_IC241A_MMX)
	pMe->m_IconTitle[1]      = IDS_PHONEBOOK;
	#else
    pMe->m_IconTitle[1]      = IDS_MAIN_MENU_CONTACTS;
	#endif
    pMe->m_IconTitle[2]      = IDS_MAIN_MENU_USERPROFILE;
    pMe->m_IconTitle[3]      = IDS_MAIN_MENU_RECENTCALLS;
    pMe->m_IconTitle[4]      = IDS_MAIN_MENU_MESSAGES;
    pMe->m_IconTitle[5]      = IDS_MAIN_MENU_CAMERA;
    pMe->m_IconTitle[6]      = IDS_MAIN_MENU_MUSICPLAYER;
    pMe->m_IconTitle[7]      = IDS_MAIN_MENU_SETTINGS;
    pMe->m_IconTitle[8]      = IDS_MAIN_MENU_FM;
    pMe->m_IconTitle[9]      = IDS_MAIN_MENU_BT;
    pMe->m_IconTitle[10]     = IDS_MAIN_MENU_SCHEDULER;
    pMe->m_IconTitle[11]     = IDS_MAIN_MENU_CALCULATOR;
	pMe->m_IconTitle[12]     = IDS_MAIN_MENU_UNIT;
    pMe->m_IconTitle[13]     = IDS_MAIN_MENU_ALARM;
    pMe->m_IconTitle[14]     = IDS_MAIN_MENU_VIDEOPLAYER;
    pMe->m_IconTitle[15]     = IDS_MAIN_MENU_STOPWATCH;
    pMe->m_IconTitle[16]     = IDS_MAIN_MENU_RECORDER;
    pMe->m_IconTitle[17]     = IDS_MAIN_MENU_WORLDCLO;
    pMe->m_IconTitle[18]     = IDS_MAIN_MENU_PINTU;
    pMe->m_IconTitle[19]     = IDS_MAIN_MENU_TETIRS;
    pMe->m_IconTitle[20]     = IDS_MAIN_MENU_BRICKATTACK;
    pMe->m_IconTitle[21]     = IDS_MAIN_MENU_BLACKJACK;
    pMe->m_IconTitle[22]     = IDS_MAIN_MENU_CALIBRA;
    pMe->m_IconTitle[23]     = IDS_MAIN_MENU_TIMER;
	//pMe->m_IconTitle[24]     = IDS_MAIN_MENU_CONTACTS;
    return SUCCESS;
}

/*==============================================================================
函数:
    CMainMenu_FreeAppData

说明:
    释放 MainMenu Applet 相关资源。

参数:
    pMe [in]: 指向MainMenu Applet 对象结构的指针。该结构包含小程序的特定信息。

返回值:
    none

备注:

==============================================================================*/
static void CMainMenu_FreeAppData(MainMenu *pMe)
{
    if (NULL == pMe)
    {
        return;
    }

    pMe->m_eAppStatus = MAINMENU_STOP;
    if (NULL != pMe->m_pConfig)
    {
        ICONFIG_Release(pMe->m_pConfig);
        pMe->m_pConfig = NULL;
    }    
    
    if (pMe->m_pDisplay != NULL)
    {
        (void) IDISPLAY_Release(pMe->m_pDisplay);
        pMe->m_pDisplay = NULL;
    }
	//if (pMe->m_pIAnn)
    //{
    //    IANNUNCIATOR_Release(pMe->m_pIAnn);
	//	pMe->m_pIAnn = NULL;
    //}

    if(pMe->m_pBacklight)
    {
        IBACKLIGHT_Release(pMe->m_pBacklight);
        pMe->m_pBacklight=NULL;
    }
    //释放图片资源
    {
        int i;
        int j;
		for(i=0;i<MAX_MATRIX_PAGE;i++)
		{
	        if (pMe->m_pImageBg[i] !=NULL)
	        {
	            (void) IIMAGE_Release(pMe->m_pImageBg[i]);
	            pMe->m_pImageBg[i] = NULL;
	        }
		}
		if(pMe->m_pImageBgblack !=NULL)
		{
			(void)IIMAGE_Release(pMe->m_pImageBgblack);
            pMe->m_pImageBgblack = NULL;
		}
		#if 0
        for(i=0;i<MAX_MATRIX_ITEMS;i++)
        {
            if(pMe->m_pImageIcon[i] != NULL)
            {
                (void)IIMAGE_Release(pMe->m_pImageIcon[i]);
                pMe->m_pImageIcon[i] = NULL;
            }
        } 
		
		for(j = 0;j<MAX_BOTTOM_ITEMS;j++)
		{
			 if(pMe->m_pImageButtom[j] != NULL)
            {
                (void)IIMAGE_Release(pMe->m_pImageButtom[j]);
                pMe->m_pImageButtom[j] = NULL;
            }
		}
		#endif

        if(pMe->m_pAnimate != NULL)
        {
            (void)IIMAGE_Release(pMe->m_pAnimate);
            pMe->m_pAnimate = NULL;
        }
		if(pMe->m_pImageSelect != NULL)
		{
			(void)IIMAGE_Release(pMe->m_pImageSelect);
			pMe->m_pImageSelect = NULL;
		}
		if(pMe->m_pImageSelect_foucs!= NULL)
		{
			(void)IIMAGE_Release(pMe->m_pImageSelect_foucs);
			pMe->m_pImageSelect_foucs= NULL;
		}
    }  
}

/*=============================================================================
FUNCTION:  MainMenu_AddRef

DESCRIPTION: 

PARAMETERS:

RETURN VALUE:

COMMENTS:

SIDE EFFECTS:

SEE ALSO:

=============================================================================*/
static uint32  MainMenu_AddRef( IMainMenu *pi)
{
    return ( ++gMainMenu.referenceCounter);
}

/*=============================================================================
FUNCTION:  MainMenu_Release

DESCRIPTION: 

PARAMETERS:

RETURN VALUE:

COMMENTS:

SIDE EFFECTS:

SEE ALSO:

=============================================================================*/
static uint32  MainMenu_Release( IMainMenu *pi)
{
    register MainMenu *pMe = (MainMenu*)pi;
    if (pMe->referenceCounter == 0)
    {
        return 0;
    }
    
    if( --pMe->referenceCounter)
    {
        return pMe->referenceCounter;
    }


    CMainMenu_FreeAppData(pMe);

    (void) ISHELL_Release(pMe->m_pShell);
    (void) IMODULE_Release(pMe->m_pModule);
    return 0;
}

/*==============================================================================
函数:
    MainMenu_RunFSM

说明:
    运行有限状态机引擎。

参数:
    pMe [in]: 指向MainMenu Applet对象结构的指针。该结构包含小程序的特定信息。
       
返回值:
    无。

备注:

==============================================================================*/
static void MainMenu_RunFSM(MainMenu *pMe)
{
    NextFSMAction nextFSMAction = NFSMACTION_WAIT;
    for ( ; ; )
    {
        nextFSMAction = MainMenu_ProcessState(pMe);

        // 这里不需要复杂的状态机，因此为简化状态机设计这里仅用一个布尔变量来
        // 指示dlgResult是否应该被改写。在某些特殊场合（见状态机设计），这种作
        // 法，避免了不必要的复杂性，简化了设计。
        if (pMe->m_bDoNotOverwriteDlgResult)
        {
            // We allow only one iteration without touching this variable. So,
            // let's reset it back to indicate that we are going to overwrite the
            // result for all subsequent iterations.
            pMe->m_bDoNotOverwriteDlgResult = FALSE;
        }
        else
        {
            // Overwrite the dlgResult to its default value. The next dialog that
            // we are going to display will set and return the dlgResult based on
            // the users action.
            pMe->m_eDlgReturn = DLGRET_CREATE;
        }

        // If the state machine processing engine indicates that it created a
        // dialog and requesting us to exit this loop, then we quit.
        if (nextFSMAction == NFSMACTION_WAIT)
        {
            break;
        }
    }
}

/*=============================================================================
FUNCTION:  MainMenu_HandleEvent

DESCRIPTION: 

PARAMETERS:

RETURN VALUE:

COMMENTS:

SIDE EFFECTS:

SEE ALSO:

=============================================================================*/
static boolean MainMenu_HandleEvent( IMainMenu *pi,
    AEEEvent eCode,
    uint16  wParam,
    uint32 dwParam
)
{
    MainMenu *pMe = (MainMenu*)pi;
    AEEAppStart* as = NULL;
    switch ( eCode)
    {
        case EVT_APP_START:
			
#ifdef FEATURE_RANDOM_MENU_REND
            DisplayRend_Enable(TRUE);
#endif
            // 此事件dwParam为指针，不应为0
            if (dwParam == 0) 
            {
                return FALSE;
            }            
            as = (AEEAppStart*)dwParam;
   
            pMe->m_rc = as->rc;

            pMe->m_currState = MAINST_MAIN;
            pMe->m_eDlgReturn = DLGRET_CREATE;
            pMe->m_eAppStatus = MAINMENU_RUNNING;
            //IANNUNCIATOR_SetFieldIsActiveEx(pMe->m_pIAnn,TRUE); 
            MainMenu_RunFSM(pMe);
            return TRUE;

        case EVT_APP_STOP:
            {
                int theFocus = 4;
                SetCursor( pMe, theFocus / 3, theFocus % 3);
                (void)ISHELL_CancelTimer( pMe->m_pShell, NULL, pMe);
                pMe->m_eAppStatus = MAINMENU_STOP;
#ifdef FEATURE_RANDOM_MENU_REND
                DisplayRend_Enable(FALSE);
#endif
                return TRUE;
            }

        case EVT_APP_SUSPEND:
            (void)ISHELL_CancelTimer(pMe->m_pShell, NULL, pMe);
            pMe->m_eAppStatus = MAINMENU_SUSPEND;
#ifdef FEATURE_RANDOM_MENU_REND
            DisplayRend_Push();
            DisplayRend_Start(FALSE);
#endif
            return TRUE;
        

        case EVT_APP_RESUME:
            {
                AEEAppStart* as = ( AEEAppStart*)dwParam;
                pMe->m_rc    = as->rc;
                pMe->m_eAppStatus = MAINMENU_RUNNING;
#ifdef FEATURE_RANDOM_MENU_REND
                DisplayRend_Push();
                DisplayRend_Start(TRUE);
#endif
                // 开始跑WMS状态机
                MainMenu_RunFSM(pMe); 
                return TRUE;
            }

        case EVT_DIALOG_INIT:
            pMe->m_pActiveIDlg = (IDialog*)dwParam;
            pMe->m_pActivedlgID = wParam;
            return MainMenu_RouteDialogEvt(pMe,eCode,wParam,dwParam);

        case EVT_DIALOG_START:
            return MainMenu_RouteDialogEvt(pMe,eCode,wParam,dwParam);

        case EVT_USER_REDRAW:
            return MainMenu_RouteDialogEvt(pMe,eCode,wParam,dwParam);

        case EVT_DIALOG_END:
            (void) MainMenu_RouteDialogEvt(pMe,eCode,wParam,dwParam);
            pMe->m_pActiveIDlg = NULL;
            pMe->m_pActivedlgID = 0;
            if(pMe->m_eAppStatus == MAINMENU_RUNNING)
            {
                MainMenu_RunFSM(pMe);
            }
            return TRUE;
        case EVT_KEY:
#if MIN_BREW_VERSION(3,0)
            // do not want to handle au
            if (((dwParam & KB_AUTOREPEAT) != 0) &&
                (wParam != AVK_UP) &&
                (wParam != AVK_DOWN) &&
                (wParam != AVK_LEFT) &&
                (wParam != AVK_RIGHT))
            {
                return TRUE;
            }
#endif
            return MainMenu_RouteDialogEvt(pMe,eCode,wParam,dwParam);  
#if defined (FEATURE_VERSION_N021)||defined(FEATURE_VERSION_W0216A)
		case EVT_KEY_HELD:
			{
				if(wParam == AVK_INFO)
				{
					(void) ISHELL_PostEvent(pMe->m_pShell,
                                        AEECLSID_CORE_APP,
                                        EVT_USER,
                                        1,
                                        0);
                    ISHELL_CloseApplet(pMe->m_pShell,TRUE);
				}
			}
			break;
#endif
        case EVT_KEY_PRESS:
        case EVT_KEY_RELEASE:
        case EVT_COMMAND:
            return MainMenu_RouteDialogEvt(pMe,eCode,wParam,dwParam); 

        default:
            return MainMenu_RouteDialogEvt(pMe,eCode,wParam,dwParam);
    }
}

void Mainmenu_KeypadLock(boolean block)
{
   gbMainmenuLock = block;
}

/*==============================================================================
函数:
    MainMenu_ProcessState

说明:
    根据 pMe->m_currState, 调用相应的状态处理函数。

参数:
    pMe [in]: 指向MainMenu Applet对象结构的指针。该结构包含小程序的特定信息。

返回值:
    NFSMACTION_CONTINUE: 指示不停状态机。
    NFSMACTION_WAIT: 指示停止状态机。

备注:

==============================================================================*/
NextFSMAction MainMenu_ProcessState(MainMenu *pMe)
{
    NextFSMAction retVal = NFSMACTION_WAIT;
    if (NULL == pMe)
    {
        return retVal;
    }
    switch(pMe->m_currState)
    {
        case MAINST_MAIN:
            return MAINST_MAIN_Handler(pMe);
        case MAINST_EXIT:
            return MAINST_EXIT_Handler(pMe);  
        default:
            break;

    }
    return retVal;
}

/*==============================================================================
函数:
    MAINST_MAIN_Handler

说明:
    MAINST_MAIN 状态处理函数。

参数:
    pMe [in]: 指向MAINMENU Applet对象结构的指针。该结构包含小程序的特定信息。

返回值:
    NFSMACTION_CONTINUE: 指示不停状态机。
    NFSMACTION_WAIT: 指示停止状态机。

备注:

==============================================================================*/
static NextFSMAction MAINST_MAIN_Handler(MainMenu *pMe)
{
    if (NULL == pMe)
    {
        return NFSMACTION_WAIT;
    }
    switch (pMe->m_eDlgReturn)
    {
        // 进入主界面
        case DLGRET_CREATE:
            {
                MainMenu_ShowDialog(pMe, IDD_MAIN_MENU);
            }
            return NFSMACTION_WAIT;
            
        case DLGRET_CANCELED:
            MOVE_TO_STATE(MAINST_EXIT)
            return NFSMACTION_CONTINUE;

        default:
            MOVE_TO_STATE(MAINST_EXIT)
            return NFSMACTION_CONTINUE;
    }
}

/*==============================================================================
函数:
    MAINST_EXIT_Handler

说明:
    MAINST_EXIT 状态处理函数。

参数:
    pMe [in]: 指向MAINMENU  Applet对象结构的指针。该结构包含小程序的特定信息。

返回值:
    NFSMACTION_CONTINUE: 指示不停状态机。
    NFSMACTION_WAIT: 指示停止状态机。

备注:

==============================================================================*/
static NextFSMAction MAINST_EXIT_Handler(MainMenu *pMe)
{
    (void) ISHELL_CloseApplet(pMe->m_pShell, FALSE);
    return NFSMACTION_WAIT;
} 

/*==============================================================================
函数:
       MainMenu_ShowDialog

说明:
    函数由状态处理函数用来唤起相应对话框资源ID的对话框。

参数:
    pMe [in]: 指向 MAINMENU Applet 对象结构的指针。该结构包含小程序的特定信息。
    dlgResId [in]: 对话框资源ID，函数根据该ID创建对话框。

返回值:
    none

备注:

==============================================================================*/
void MainMenu_ShowDialog(MainMenu  *pMe,  uint16 dlgResId)
{
    int nRet;

    // 每次最多打开一个对话框
    if (ISHELL_GetActiveDialog(pMe->m_pShell) != NULL)
    {
        // 如果当前已经有对话框被打开，直接返回
        return;
    }
   
    if (NULL != pMe->m_pDisplay)
    {
        AEEDeviceInfo di={0,};
        //if (dlgResId == IDD_MAIN_MENU)
        //{
        //    (void)IDISPLAY_SetPrefs(pMe->m_pDisplay, "a:0", STRLEN("a:0"));
        //}
        //else
        {
            (void)IDISPLAY_SetPrefs(pMe->m_pDisplay, "a:0", STRLEN("a:0"));
        }
        ISHELL_GetDeviceInfo(pMe->m_pShell, &di);
        pMe->m_rc.dx = di.cxScreen;
        pMe->m_rc.dy = di.cyScreen;
        IDISPLAY_SetClipRect(pMe->m_pDisplay, &pMe->m_rc);
        CalculateScreenParameters(pMe);
    }
    nRet = ISHELL_CreateDialog(pMe->m_pShell,MAINMENU_RES_FILE_LANG,dlgResId,NULL);
    if (nRet != SUCCESS)
    {
    }
}


/*==============================================================================
函数:
       MainMenu_RouteDialogEvt
说明:
       函数将BREW事件路由给当前活动对话框的事件处理函数。

参数:
       pMe [In]: 指向SettingMenu Applet对象结构的指针,该结构包含小程序的特定信息.
       eCode [in]：事件代码。
       wParam [in]：与事件eCode关联的数据。
       dwParam [in]：与事件eCode关联的数据。

返回值:
       TRUE：传入事件被处理。
       FALSE：传入事件没被处理。

备注:

==============================================================================*/
boolean MainMenu_RouteDialogEvt(MainMenu *pMe,
    AEEEvent    eCode,
    uint16  wParam,
    uint32 dwParam
)

{
    if (NULL == pMe)
    {
        return FALSE;
    }

    if (NULL == pMe->m_pActiveIDlg)
    {
        return FALSE;
    }    
    switch( pMe->m_pActivedlgID)
    {
        case IDD_MAIN_MENU:
            return MainMenu_IconMenuHandler(pMe, eCode, wParam, dwParam);
            
        default:
            return FALSE;
    }
}

/*=============================================================================
FUNCTION:  MainMenu_IconMenuHandler

DESCRIPTION: 12宫格菜单

PARAMETERS:

=============================================================================*/
static boolean MainMenu_IconMenuHandler(MainMenu *pMe, AEEEvent eCode, uint16 wParam, uint32 dwParam)
{
    PARAM_NOT_REF( dwParam)
    switch ( eCode)
    {
        case EVT_DIALOG_INIT:	
            IDIALOG_SetProperties((IDialog *)dwParam, DLG_NOT_REDRAW_AFTER_START);
            return TRUE;

        case EVT_DIALOG_START:
			MSG_FATAL("MainMenu_IconMenuHandler  EVT_APP_START.......",0,0,0);
            if(pMe->m_pImageBgblack  == NULL)
            {
                    pMe->m_pImageBgblack = ISHELL_LoadImage(pMe->m_pShell,"fs:/image/mainmenu/backgroud.png");//modi by yangdecai  
				    if(pMe->m_pImageBgblack == NULL)
				    {
				    	MSG_FATAL("pMe->m_pImageBg[0] is null",0,0,0);
				    }
            }
			if(pMe->m_pImageBg[1] == NULL)
            {
				//pMe->m_pImageBg[1] = ISHELL_LoadImage(pMe->m_pShell,
				//                                  ICONBACK1_ANI);//modi by yangdecai  
			}
			if(pMe->m_pImageBgblack ==NULL)
			{
				#ifndef FEATURE_VERSION_NO_BG
				pMe->m_pImageBgblack  = ISHELL_LoadResImage(pMe->m_pShell, AEE_APPSCOMMONRES_IMAGESFILE, IDB_BACKGROUND);
				#endif
			}
            
            // 初始整个背景及全部初始图标	
            MSG_FATAL("MainMenu_IconMenuHandler  EVT_USER_REDRAW STAR.......",0,0,0);
            DrawMatrix(pMe);
			MSG_FATAL("MainMenu_IconMenuHandler  EVT_USER_REDRAW mid.......",0,0,0);
            // 绘制聚焦过程动画
            MoveCursorTo(pMe, pMe->m_nRow, pMe->m_nColumn);
			DrawMatrixStr(pMe);
            return TRUE;
            
        case EVT_DIALOG_END:
            {
                int i;
                int j;
				for(i=0;i<MAX_MATRIX_PAGE;i++)
				{
	                if (pMe->m_pImageBg[i] !=NULL)
	                {
	                    (void) IIMAGE_Release(pMe->m_pImageBg[i]);
	                    pMe->m_pImageBg[i] = NULL;
	                }
				}
				if(pMe->m_pImageBgblack !=NULL)
				{
					(void)IIMAGE_Release(pMe->m_pImageBgblack);
		            pMe->m_pImageBgblack = NULL;
				}
				#if 0
                for(i=0;i<MAX_MATRIX_ITEMS;i++)
                {
                    if(pMe->m_pImageIcon[i] != NULL)
                    {
                        (void)IIMAGE_Release(pMe->m_pImageIcon[i]);
                        pMe->m_pImageIcon[i] = NULL;
                    }
                } 
				
                for(j = 0;j<MAX_BOTTOM_ITEMS;j++)
				{
					 if(pMe->m_pImageButtom[j] != NULL)
		            {
                        (void)IIMAGE_Release(pMe->m_pImageButtom[j]);
		                pMe->m_pImageButtom[j] = NULL;
		            }
				}
				#endif
                if(pMe->m_pAnimate != NULL)
                {
                    (void)IIMAGE_Release(pMe->m_pAnimate);
                    pMe->m_pAnimate = NULL;
                }
                if(pMe->m_pImageSelect != NULL)
				{
					(void)IIMAGE_Release(pMe->m_pImageSelect);
					pMe->m_pImageSelect = NULL;
				}
                if(pMe->m_pImageSelect_foucs!= NULL)
				{
					(void)IIMAGE_Release(pMe->m_pImageSelect_foucs);
					pMe->m_pImageSelect_foucs = NULL;
				}
                ISHELL_CancelTimer(pMe->m_pShell, NULL, (void**)pMe);
            }
            return TRUE;

        case EVT_KEY_HELD:
            
        #if defined(FEATURE_TORCH_KEY_INFO)
           if((AVKType)wParam == AVK_INFO)
            {
                boolean TorchOn = FALSE;
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
                ISHELL_CloseApplet(pMe->m_pShell, TRUE); 
                return TRUE;
            }
        #endif
            return TRUE;
        case EVT_KEY:
            switch( wParam)
            {
                case AVK_CLR:
                     CLOSE_DIALOG(DLGRET_CANCELED)
                    return TRUE;
                    
                case AVK_UP:
                    {
                        int nRow = pMe->m_nRow;
                        int nCol = pMe->m_nColumn;
                        MSG_FATAL("AVK_UPpMe->m_nRow==%d,pMe->m_nColumn==%d",pMe->m_nRow,pMe->m_nColumn,0);
                        if (nRow>0)
                        {
                            nRow--;
                        }
                        else
                        {
                            if (nCol>0)
                            {
                                nCol--;
                            }
                            else
                            {
                                nCol = MAX_MATRIX_COLS-1;
                            }
                            nRow = MAX_MATRIX_ROWS-1;
                        }
                        MoveCursorTo(pMe, nRow, nCol);
                    }
                    return TRUE;
                    
                case AVK_DOWN:
                    {
                        int nRow = pMe->m_nRow;
                        int nCol = pMe->m_nColumn;
                        MSG_FATAL("AVK_DOWNpMe->m_nRow==%d,pMe->m_nColumn==%d",pMe->m_nRow,pMe->m_nColumn,0);
                        if (nRow<MAX_MATRIX_ROWS-1)
                        {
                            nRow++;
                        }
                        else
                        {
                            if (nCol<MAX_MATRIX_COLS-1)
                            {
                                nCol++;
                            }
                            else
                            {
                                nCol = 0;
                            }
                            nRow = 0;
                        }
                        MoveCursorTo(pMe, nRow, nCol);
                    }
                    return TRUE;
                    
                case AVK_LEFT:
                    {
                        int nRow = pMe->m_nRow;
                        int nCol = pMe->m_nColumn;
                        MSG_FATAL("AVK_leftpMe->m_nRow==%d,pMe->m_nColumn==%d",pMe->m_nRow,pMe->m_nColumn,0);
                        if (nCol>0)
                        {
                            nCol--;
                        }
                        else
                        {
                            if (nRow>0)
                            {
                                nRow--;
                            }
                            else
                            {
                            	#ifdef FEATURE_VERSION_X3
								if(pMe->m_PrsentPage>0)
								{
									pMe->m_PrsentPage--;
									pMe->m_bRight = FALSE;
									pMe->m_bReraw = TRUE;
									AutoMovePage(pMe);
									pMe->m_bReraw = FALSE;
								}
								else
								{
									nRow = MAX_MATRIX_ROWS-1;
								}
								#else
                                nRow = MAX_MATRIX_ROWS-1;
								#endif
                            }
                            nCol = MAX_MATRIX_COLS-1;
                        }
                        MoveCursorTo(pMe, nRow, nCol);
                    }
                    return TRUE;
                    
                case AVK_RIGHT:
                    {
                        int nRow = pMe->m_nRow;
                        int nCol = pMe->m_nColumn;
                        MSG_FATAL("AVK_RIGHTpMe->m_nRow==%d,pMe->m_nColumn==%d",pMe->m_nRow,pMe->m_nColumn,0);
                        if (nCol<MAX_MATRIX_COLS-1)
                        {
                            nCol++;
                        }
                        else
                        {
                            if (nRow<MAX_MATRIX_ROWS-1)
                            {
                                nRow++;
                            }
                            else
                            {
                            	#ifdef FEATURE_VERSION_X3
								if(pMe->m_PrsentPage==0)
								{
									pMe->m_PrsentPage++;
									pMe->m_bRight = TRUE;
									pMe->m_bReraw = TRUE;
									AutoMovePage(pMe);
									pMe->m_bReraw = FALSE;
								}
								else
								{
									nRow = 0;
								}
								#else
                                nRow = 0;
								#endif
                            }
                            nCol = 0;
                        }
                        MoveCursorTo(pMe, nRow, nCol);
                    }
                    return TRUE;
                    
                case AVK_SELECT:
                case AVK_INFO:
                    {
                        int Focus = pMe->m_nRow * MAX_MATRIX_COLS + pMe->m_nColumn;
                        StartApplet(pMe, pMe->m_IconTitle[(pMe->m_PrsentPage*MAX_MATRIX_ITEMS)+Focus]);
                    }
                    return TRUE;
                    
                case AVK_1:
                case AVK_2:
                case AVK_3:
                case AVK_4:
                case AVK_5:
                case AVK_6:
                case AVK_7:
                case AVK_8:
                case AVK_9:
                    StartApplet(pMe, pMe->m_IconTitle[wParam - AVK_1]);
                    return TRUE;
                    
                case AVK_STAR:
					MSG_FATAL("gbMainmenuLock======%d",gbMainmenuLock,0,0);
                    if(gbMainmenuLock)
                    {
                        OEMKeyguard_SetState(TRUE);
                        ISHELL_CloseApplet(pMe->m_pShell, TRUE); 
                    }
                    else
                    {
                     StartApplet(pMe, pMe->m_IconTitle[9]);
                    }
                    return TRUE;

                case AVK_0:
                    StartApplet(pMe, pMe->m_IconTitle[10]);
                    return TRUE;
                    
                case AVK_POUND:
                    StartApplet(pMe, pMe->m_IconTitle[11]);
                    return TRUE;

                default:
                    break;
            }

            return TRUE;
        case EVT_PEN_DOWN:
				{
					
					AEERect rc;
					uint16 wXPos = (int16)AEE_GET_X(dwParam);
					uint16 wYPos = (int16)AEE_GET_Y(dwParam);
					uint16 i = 0;
					boolean m_bInRect = FALSE;
					uint16 nRow = 0;
					uint16 nCol = 0;
					pMe->m_Pdown_Pt.x = wXPos;
					pMe->m_Pdown_Pt.y = wYPos;
					pMe->m_Primove_Pt.x = wXPos;
					pMe->m_Primove_Pt.y = wYPos;
					pMe->m_bmove = FALSE;
					MSG_FATAL("EVT_PEN_DOWN................",0,0,0);
					for(i=0;i<MAX_MATRIX_ITEMS;i++)
					{
						rc.x = pMe->m_Icondefault_Pt[i].x;
						rc.y = pMe->m_Icondefault_Pt[i].y;
						rc.dx = MAIN_ICON_W;
						rc.dy = MAIN_ICON_H;
						m_bInRect = MAINMENU_PT_IN_RECT(wXPos,wYPos,rc);
						if(m_bInRect)
						{
							pMe->m_PenPos = i;
							break;
						}
					}
					if(pMe->m_PenPos>-1)
					{
						nRow = (pMe->m_PenPos)/MAX_MATRIX_COLS;
						nCol = (pMe->m_PenPos)%MAX_MATRIX_COLS;
						MoveCursorTo(pMe, nRow, nCol);
					}
					
					return TRUE;
				}
				break;
		case EVT_PEN_MOVE:
				{
					AEEDeviceInfo devinfo;
					int nBarH ;
					AEERect rc;
					uint16 wXPos = (int16)AEE_GET_X(dwParam);
					uint16 wYPos = (int16)AEE_GET_Y(dwParam);
					int wDxPos   = wXPos-pMe->m_Primove_Pt.x;
					int wDyPos   = wYPos-pMe->m_Primove_Pt.y;
					uint16 uDxPos = 0;
					
					MSG_FATAL("EVT_PEN_MOVE................",0,0,0);
					if(wDxPos>0)
					{
						uDxPos = wDxPos;
					}
					else
					{
						uDxPos = -wDxPos;
					}
					if(uDxPos>DX_MAIN_MOVE)
					{
						MSG_FATAL("EVT_PEN_MOVE...........start.....ok",0,0,0);
						pMe->m_Primove_Pt.x = wXPos;
						pMe->m_Primove_Pt.y = wYPos;
						pMe->m_bmove = TRUE;
					}
					return TRUE;
				}
				break;
		case EVT_PEN_UP:
			{
				AEERect rc;
				uint16 wXPos = (int16)AEE_GET_X(dwParam);
				uint16 wYPos = (int16)AEE_GET_Y(dwParam);
				uint16 i = 0;
				boolean m_bInRect = FALSE;
				boolean m_bbottom = FALSE;
				uint16 nRow = 0;
				uint16 nCol = 0;
				uint16 j    = 0;
				MSG_FATAL("EVT_PEN_UP m_bmove=%d",pMe->m_bmove,0,0);
				for(i=0;i<MAX_MATRIX_ITEMS;i++)
				{
					rc.x = pMe->m_Icondefault_Pt[i].x;
					rc.y = pMe->m_Icondefault_Pt[i].y;
					rc.dx = MAIN_ICON_W;
					rc.dy = MAIN_ICON_H;
					m_bInRect = MAINMENU_PT_IN_RECT(wXPos,wYPos,rc);
					if(m_bInRect)
					{
						if(pMe->m_PenPos != i)
						{
							pMe->m_PenPos = -1;
							break;
						}
						pMe->m_PenPos = i;
						break;
					}
				}
				if((pMe->m_PenPos != -1)&&(!pMe->m_bmove))
				{
					StartApplet( pMe,pMe->m_IconTitle[(pMe->m_PrsentPage*MAX_MATRIX_ITEMS)+pMe->m_PenPos]);
					pMe->m_PenPos = -1;
				}
				else
				{
					int m_Downx = pMe->m_Pdown_Pt.x - wXPos;
					MSG_FATAL("EVT_PEN_UP....................",0,0,0);
					if(m_Downx>0)
					{
						if(m_Downx>DX_MAIN_MOVE)
						{
							if(pMe->m_PrsentPage < (MAX_MATRIX_PAGE-1))
							{
								pMe->m_PrsentPage++;
								pMe->m_bReraw = TRUE;
							}
							else
							{
								pMe->m_PrsentPage = (MAX_MATRIX_PAGE-1);
								pMe->m_bReraw = FALSE;
							}
							pMe->m_bRight = TRUE;
						}
					}
					else
					{
						m_Downx = -m_Downx;
						if(m_Downx>DX_MAIN_MOVE)
						{
							if(pMe->m_PrsentPage>0)
							{
								pMe->m_PrsentPage--;
								pMe->m_bReraw = TRUE;
							}
							else
							{
								pMe->m_PrsentPage = 0;
								pMe->m_bReraw = FALSE;
							}
							pMe->m_bRight = FALSE;
						}
						
					}
					if(pMe->m_bReraw)
					{
						AutoMovePage(pMe);
						pMe->m_bReraw = FALSE;
						//MoveCursorTo(pMe, nRow, nCol);
					}
				}
				
				MSG_FATAL("EVT_PEN_UP m_bmove  end=%d",pMe->m_bmove,0,0);
				return TRUE;
			}
			break;
        default:
            break;
    }

    return FALSE;
}
static void AutoMovePage(MainMenu *pMe)
{
	MSG_FATAL("AutoMovePage........................",0,0,0);

		if(pMe->m_step<5)
		{
			MSG_FATAL("ISHELL_SetTimer................",0,0,0);
		    (void)ISHELL_SetTimer(pMe->m_pShell,
                                150,
                                (PFNNOTIFY)AutoMovePage,
                                pMe);
			if(pMe->m_bRight)
			{
				
				MovePen(pMe,(5-pMe->m_step)*35);
			}
			else
			{
				MovePen(pMe,-((5-pMe->m_step)*35));	
			}
            IDISPLAY_UpdateEx(pMe->m_pDisplay, TRUE);
            pMe->m_step++;
		}
        else if(pMe->m_step == 5)
        {
            (void)ISHELL_SetTimer(pMe->m_pShell,
                                150,
                                (PFNNOTIFY)AutoMovePage,
                                pMe);
            // 初始整个背景及全部初始图标	
    		DrawMatrix(pMe);
            IDISPLAY_UpdateEx(pMe->m_pDisplay, TRUE);
            pMe->m_step ++;
        }
		else
		{
			//pMe->m_step = 5;
			//pMe->m_bDraOver = TRUE;
			pMe->m_step = 0;
    		MSG_FATAL("pMe->m_PrsentPage=========%d",pMe->m_PrsentPage,0,0);
    		// 初始整个背景及全部初始图标	
    		DrawMatrix(pMe);
    		// 绘制聚焦过程动画
    		MoveCursorTo(pMe, pMe->m_nRow, pMe->m_nColumn);
    		MSG_FATAL("pMe->m_nRow===%d,pMe->m_nColumn=%d",pMe->m_nRow,pMe->m_nColumn,0);
    		DrawMatrixStr(pMe);
    		pMe->m_bmove = FALSE;
            IDISPLAY_UpdateEx(pMe->m_pDisplay, TRUE);
			
		}

	
    /*
	pMe->m_step++;
    
	if(pMe->m_bRight)
	{
		
		MovePen(pMe,(5-pMe->m_step)*35);
	}
	else
	{
		MovePen(pMe,-((5-pMe->m_step)*35));	
	}
    IDISPLAY_UpdateEx(pMe->m_pDisplay, TRUE);
    if(pMe->m_step >= 5)
    {
        pMe->m_step = 0;
		MSG_FATAL("pMe->m_PrsentPage=========%d",pMe->m_PrsentPage,0,0);
		// 初始整个背景及全部初始图标	
		DrawMatrix(pMe);
		// 绘制聚焦过程动画
		MoveCursorTo(pMe, pMe->m_nRow, pMe->m_nColumn);
		MSG_FATAL("pMe->m_nRow===%d,pMe->m_nColumn=%d",pMe->m_nRow,pMe->m_nColumn,0);
		DrawMatrixStr(pMe);
		pMe->m_bmove = FALSE;
        IDISPLAY_UpdateEx(pMe->m_pDisplay, TRUE);
    }
    else
    {
        MSG_FATAL("ISHELL_SetTimer................",0,0,0);
		(void)ISHELL_SetTimer(pMe->m_pShell,
                                150,
                                (PFNNOTIFY)AutoMovePage,
                                pMe);
    }
    */
    
}

static void Main_keypadtimer(void *pUser)
{
    gbMainmenuLock =FALSE;
}

/*=============================================================================
FUNCTION:  CalculateScreenParameters

DESCRIPTION: 计算焦点图片以及默认图片的坐标

=============================================================================*/
static void CalculateScreenParameters(MainMenu *pMe)
{
    AEEImageInfo    imageInfoIcon;
    uint16          iconSpaceHorizontal = 0;
    uint16          iconSpaceVertical   = 0;
    uint8           i                   = 0;
	uint8           j                   = 0;
	AEEImageInfo    BottomimageInfoIcon;
	
	BottomimageInfoIcon.cx = BOTTOM_ICON_WIDTH;
	BottomimageInfoIcon.cy = BOTTOM_ICON_HEIGHT;
    //计算默认图片的坐标
    imageInfoIcon.cx = ICON_WIDTH;
    imageInfoIcon.cy = ICON_HEIGHT;

    /* icon size in all might be larger than screen*/
    if(pMe->m_rc.dx > imageInfoIcon.cx * MAX_MATRIX_COLS)
    {
        iconSpaceHorizontal = (pMe->m_rc.dx - imageInfoIcon.cx * MAX_MATRIX_COLS) / (MAX_MATRIX_COLS+1);
    }
    else
    {
        iconSpaceHorizontal = 0;
    }

    if(pMe->m_rc.dy > TITLEBAR_HEIGHT + imageInfoIcon.cy * MAX_MATRIX_ROWS)
    {
        iconSpaceVertical   = (pMe->m_rc.dy - TITLEBAR_HEIGHT*2 - imageInfoIcon.cy * MAX_MATRIX_ROWS) / (MAX_MATRIX_ROWS+1);
    }
    else
    {
        iconSpaceVertical = 0;
    }
    for( i = 0; i < MAX_MATRIX_ITEMS; i ++)
    {
        pMe->m_Icondefault_Pt[i].x = iconSpaceHorizontal +
            ( imageInfoIcon.cx + iconSpaceHorizontal) * ( i % MAX_MATRIX_COLS);
        
        pMe->m_Icondefault_Pt[i].y = TITLEBAR_HEIGHT + iconSpaceVertical +
            ( imageInfoIcon.cy + iconSpaceVertical) * ( i / MAX_MATRIX_COLS);

        //计算焦点图片的坐标
        //pMe->m_IconFocus_Pt[i].x = pMe->m_Icondefault_Pt[i].x - (ICON_ANIMATED_WIDTH - imageInfoIcon.cx)/2;

        //pMe->m_IconFocus_Pt[i].y = pMe->m_Icondefault_Pt[i].y - (ICON_ANIMATED_HEIGHT- imageInfoIcon.cy)/2;
        //end added
        MSG_FATAL("pMe->m_Icondefault_Pt[i].x=%d,pMe->m_Icondefault_Pt[i].y=%d",pMe->m_Icondefault_Pt[i].x,pMe->m_Icondefault_Pt[i].y,0);
    }
}

static void MainMenu_DrawBackGround(MainMenu *pMe, AEERect *pRect)
{
#ifdef FEATURE_RANDOM_MENU_COLOR
    IDISPLAY_FillRect(pMe->m_pDisplay, pRect, pMe->m_nBgColor);
    if(pMe->m_nRandomMenu != 0)
    {
        Appscommon_ResetBackground(pMe->m_pDisplay, 
                                                    pMe->m_pImageBg, 
                                                    pMe->m_nBgColor, 
                                                    pRect, 
                                                    0, 
                                                    0);
    }
    else
#endif
    {
    	
        Appscommon_ResetBackground(pMe->m_pDisplay, 
                                                    pMe->m_pImageBgblack, 
                                                    APPSCOMMON_BG_COLOR, 
                                                    pRect, 
                                                    0, 
                                                    0);
		
    } 
}
/*
static void MainMenu_DrawSelectIcon(MainMenu *pMe)
{
	int i =0;
	if(pMe->m_pImageSelect == NULL)
	{
        pMe->m_pImageSelect = ISHELL_LoadImage(pMe->m_pShell, ICON_SELECT);
	}
	if(pMe->m_pImageSelect_foucs == NULL)
	{
        pMe->m_pImageSelect_foucs = ISHELL_LoadImage(pMe->m_pShell, ICON_SELECT_FOCUS);
	}
    for(i=0;i<MAX_MATRIX_PAGE;i++)
	{
        if(i == pMe->m_PrsentPage)
		{
           IIMAGE_Draw(pMe->m_pImageSelect_foucs,
                        pMe->m_IconSelect_Pt[i].x,
                        pMe->m_IconSelect_Pt[i].y);
		}
		else
		{
           IIMAGE_Draw(pMe->m_pImageSelect,
                        pMe->m_IconSelect_Pt[i].x,
                        pMe->m_IconSelect_Pt[i].y);
		}
	}
}*/

/*=============================================================================
FUNCTION:  DrawMatrix

DESCRIPTION: // 初始整个背景及全部初始图标

=============================================================================*/
static void DrawMatrix(MainMenu *pMe)
{
    int i = 0;
	int j = 0;
	int k = 0;
	int Max =MAX_MATRIX_ITEMS;
	
	//BottomBar_Param_type BarParam={0};//wlh add
    
    if (NULL == pMe)
    {
        return;
    }
    //draw bg image
    //MainMenu_DrawBackGround(pMe, &pMe->m_rc);
	//Draw select icon
	//MainMenu_DrawSelectIcon(pMe);
    //Draw icon
    MSG_FATAL("pMe->m_PrsentPage========%d",pMe->m_PrsentPage,0,0);
	k = (pMe->m_PrsentPage*MAX_MATRIX_ITEMS);
	MSG_FATAL("k========%d",k,0,0);
	#if 0
    for (i = 0; i < Max; i ++)
    {
        pMe->m_pImageIcon[i] = ISHELL_LoadImage(pMe->m_pShell,
				                                  ICON_ANI[k+i]);
        MSG_FATAL("pMe->m_Icondefault_Pt[%d].x======%d",i,pMe->m_Icondefault_Pt[i].x,0);
		MSG_FATAL("pMe->m_Icondefault_Pt[%d].y======%d",i,pMe->m_Icondefault_Pt[i].y,0);
        if (pMe->m_pImageIcon[i] != NULL)
        {
        	MSG_FATAL("drwa.............",0,0,0);
            IIMAGE_Draw(pMe->m_pImageIcon[i],
                        pMe->m_Icondefault_Pt[i].x,
                        pMe->m_Icondefault_Pt[i].y);
        }
    }  
	#else
	#if 1
	if(pMe->m_PrsentPage)
	{
			IIMAGE_Draw(pMe->m_pImageBgblack,
                        -218,
                        0);
	}
	else
	{
			IIMAGE_Draw(pMe->m_pImageBgblack,
                        0,
                        0);
	}
	#else
	IIMAGE_Draw(pMe->m_pImageBg[pMe->m_PrsentPage],
                        0,
                        0);
	#endif
	#endif

}
/*
static void DrawMatrixBottomStr(MainMenu *pMe)
{
	int i = 0;
	int baseBottom = BASE_BOTTON_TITLE;
	for(i=0;i<MAX_BOTTOM_ITEMS;i++)
	{
		uint16      nResID = 0;// 中
    	AEERect     rc;
		AECHAR      wszBottomstr[20]={0};
		baseBottom = baseBottom + i;
		(void) ISHELL_LoadResString(pMe->m_pShell,
                                    MAINMENU_RES_FILE_LANG,
                                    pMe->m_IconTitle[(pMe->m_PrsentPage*MAX_MATRIX_ITEMS)+baseBottom],
                                    wszBottomstr,
                                    sizeof(wszBottomstr));
		rc.x  = pMe->m_IconButtom_pt[i].x+15;
		rc.y  = SCREEN_HEIGHT - 20;
		rc.dx = BOTTOM_ICON_WIDTH+30;
		rc.dy = 20;
		DrawGreyBitTextWithProfile(pMe->m_pShell,
	                              pMe->m_pDisplay,
	                              RGB_WHITE_NO_TRANS,
	                              18,
	                              wszBottomstr, -1,
	                              0, 0, &rc, 
	                              IDF_ALIGN_MIDDLE
	                              | IDF_ALIGN_CENTER
	                              | IDF_TEXT_TRANSPARENT); 
		
	}
}
*/
static void DrawMatrixStr(MainMenu *pMe)
{
	int i = 0;
	int Max = MAX_MATRIX_ITEMS;
	
	for(i=0;i<Max;i++)
	{
		uint16      nResID = 0;// 中
    	AEERect     rc;
		AECHAR      wszBottomstr[20]={0};
		(void) ISHELL_LoadResString(pMe->m_pShell,
                                    MAINMENU_RES_FILE_LANG,
                                    pMe->m_IconTitle[(pMe->m_PrsentPage*MAX_MATRIX_ITEMS)+i],
                                    wszBottomstr,
                                    sizeof(wszBottomstr));
		rc.x  = pMe->m_Icondefault_Pt[i].x-15;
		rc.y  = pMe->m_Icondefault_Pt[i].y+ICON_HEIGHT+3;
		rc.dx = ICON_WIDTH+30;
		rc.dy = 20;
		DrawGreyBitTextWithProfile(pMe->m_pShell,
	                              pMe->m_pDisplay,
	                              RGB_WHITE_NO_TRANS,
	                              16,
	                              wszBottomstr, -1,
	                              0, 0, &rc, 
	                              IDF_ALIGN_MIDDLE
	                              | IDF_ALIGN_CENTER
	                              | IDF_TEXT_TRANSPARENT); 
			
	}
}
static void DrawMatrixStr_Move(MainMenu *pMe,int dx)
{
	int i = 0;
	for(i=0;i<MAX_MATRIX_ITEMS;i++)
	{
		uint16      nResID = 0;// 中
    	AEERect     rc;
		AECHAR      wszBottomstr[20]={0};
		(void) ISHELL_LoadResString(pMe->m_pShell,
                                    MAINMENU_RES_FILE_LANG,
                                    pMe->m_IconTitle[(pMe->m_PrsentPage*MAX_MATRIX_ITEMS)+i],
                                    wszBottomstr,
                                    sizeof(wszBottomstr));
		rc.x  = pMe->m_Icondefault_Pt[i].x+dx-15;
		rc.y  = pMe->m_Icondefault_Pt[i].y+ICON_HEIGHT;
		rc.dx = ICON_WIDTH+30;
		rc.dy = 20;
		DrawGreyBitTextWithProfile(pMe->m_pShell,
	                              pMe->m_pDisplay,
	                              RGB_WHITE_NO_TRANS,
	                              16,
	                              wszBottomstr, -1,
	                              0, 0, &rc, 
	                              IDF_ALIGN_MIDDLE
	                              | IDF_ALIGN_CENTER
	                              | IDF_TEXT_TRANSPARENT); 
		
	}
}

static void DrawMatrixBottom(MainMenu *pMe)
{
	
}


static void DrawMatrixMove(MainMenu *pMe,int dx)
{
	int i = 0;
	//BottomBar_Param_type BarParam={0};//wlh add
    
    if (NULL == pMe)
    {
        return;
    }
    //draw bg image
    //MainMenu_DrawBackGround(pMe, &pMe->m_rc); 
	//Draw select icon
	//MainMenu_DrawSelectIcon(pMe);
	
    //Draw icon
    #if 0
    for (i = 0; i < MAX_MATRIX_ITEMS; i ++)
    {
        pMe->m_pImageIcon[i] = ISHELL_LoadImage(pMe->m_pShell,
                                                    ICON_ANI[(pMe->m_PrsentPage*MAX_MATRIX_ITEMS)+i]);

        if (pMe->m_pImageIcon[i] != NULL)
        {
            IIMAGE_Draw(pMe->m_pImageIcon[i],
                        pMe->m_Icondefault_Pt[i].x+dx,
                        pMe->m_Icondefault_Pt[i].y);
        }
    }  
	#else
	#if 1
	if(pMe->m_PrsentPage)
		{
			IIMAGE_Draw(pMe->m_pImageBgblack,
                        dx-218,
                        0);
		}
	else
		{
			IIMAGE_Draw(pMe->m_pImageBgblack,
                        dx,
                        0);
		}
	#else
	IIMAGE_Draw(pMe->m_pImageBg[pMe->m_PrsentPage],
                        dx,
                        0);
	#endif
	#endif
	//DrawMatrixStr_Move(pMe,dx);
}

/*=============================================================================
FUNCTION:  DrawFocusIcon

DESCRIPTION: 

PARAMETERS:

RETURN VALUE:

COMMENTS:


=============================================================================*/
static void DrawFocusIcon(MainMenu *pMe)
{
    int theFocus = pMe->m_nRow * MAX_MATRIX_COLS + pMe->m_nColumn;
    /*
	if(pMe->m_PrsentPage == (MAX_MATRIX_PAGE-1))
    {
    	theFocus = 0;
    }
	*/
    if (NULL == pMe)
    {
        return;
    }      
    if(pMe->m_pAnimate == NULL)
    {
        pMe->m_pAnimate = ISHELL_LoadImage(pMe->m_pShell, ICON_ANI_1[0]);
    }
	if((theFocus<0)||(theFocus>=MAX_MATRIX_ITEMS))
	{
		theFocus = 0;
		pMe->m_nRow = 0;
		pMe->m_nColumn = 0;
	}
	if( (pMe->m_pAnimate != NULL))
    {
    	MSG_FATAL("theFocus====%d,,x=%d,y=%d",theFocus,pMe->m_Icondefault_Pt[theFocus].x,pMe->m_Icondefault_Pt[theFocus].y);
		IIMAGE_Draw(pMe->m_pAnimate,
                    pMe->m_Icondefault_Pt[theFocus].x, 
                    pMe->m_Icondefault_Pt[theFocus].y);
	    //IIMAGE_Release(pMe->m_pAnimate);
        //pMe->m_pAnimate = NULL;   
		MSG_FATAL("pMe->m_pDisplay    TURE",0,0,0);
	    //IDISPLAY_UpdateEx(pMe->m_pDisplay, TRUE);
	}
}

static void DrawFocusIconMove(MainMenu *pMe,int dx)
{
	int theFocus = pMe->m_nRow * MAX_MATRIX_COLS + pMe->m_nColumn;
    
    if (NULL == pMe)
    {
        return;
    }      
    if(pMe->m_pAnimate == NULL)
    {
        pMe->m_pAnimate = ISHELL_LoadImage(pMe->m_pShell, ICON_ANI_1[0]);
    }
	if((theFocus<0)||(theFocus>=MAX_MATRIX_ITEMS))
	{
		theFocus = 0;
	}
	if( (pMe->m_pAnimate != NULL))
    {
		IIMAGE_Draw(pMe->m_pAnimate,
                    pMe->m_Icondefault_Pt[theFocus].x+dx, 
                    pMe->m_Icondefault_Pt[theFocus].y);
	    IIMAGE_Release(pMe->m_pAnimate);
        pMe->m_pAnimate = NULL;       
	    IDISPLAY_UpdateEx(pMe->m_pDisplay, TRUE);
	}
}


/*=============================================================================
FUNCTION:  MoveCursorTo

DESCRIPTION: 

PARAMETERS:

RETURN VALUE:

COMMENTS:

SIDE EFFECTS:

SEE ALSO:

=============================================================================*/
static void MoveCursorTo(MainMenu *pMe, int row, int column)
{
    int theFocus = pMe->m_nRow * MAX_MATRIX_COLS + pMe->m_nColumn;
    AEERect rect;
	/*
    if(pMe->m_PrsentPage == (MAX_MATRIX_PAGE-1))
    {
    	theFocus = 0;
    }
	*/
    // 绘制聚焦后矩阵初始界面

	if(pMe->m_PrsentPage)
		{
			AEEImageInfo m_ImageInfo;
		    IImage_GetInfo(pMe->m_pImageBgblack,&m_ImageInfo);
            IIMAGE_SetOffset(pMe->m_pImageBgblack,pMe->m_Icondefault_Pt[theFocus].x+218,pMe->m_Icondefault_Pt[theFocus].y);
			IIMAGE_SetDrawSize( pMe->m_pImageBgblack, 55,44);
			
			IIMAGE_Draw(pMe->m_pImageBgblack,
                        pMe->m_Icondefault_Pt[theFocus].x,
                        pMe->m_Icondefault_Pt[theFocus].y);
			IIMAGE_SetOffset( pMe->m_pImageBgblack, 0,0);
        	//IIMAGE_SetDrawSize( pWallPaper, pMe->m_rc.dx,pMe->m_rc.dy);
        	IIMAGE_SetDrawSize( pMe->m_pImageBgblack, m_ImageInfo.cx,m_ImageInfo.cy);
		}
	else
		{
			AEEImageInfo m_ImageInfo;
		    IImage_GetInfo(pMe->m_pImageBgblack,&m_ImageInfo);
            IIMAGE_SetOffset(pMe->m_pImageBgblack,pMe->m_Icondefault_Pt[theFocus].x,pMe->m_Icondefault_Pt[theFocus].y);
			IIMAGE_SetDrawSize( pMe->m_pImageBgblack, 55,44);
			IIMAGE_Draw(pMe->m_pImageBgblack,
                        pMe->m_Icondefault_Pt[theFocus].x,
                        pMe->m_Icondefault_Pt[theFocus].y);
			IIMAGE_SetOffset( pMe->m_pImageBgblack, 0,0);
        	//IIMAGE_SetDrawSize( pWallPaper, pMe->m_rc.dx,pMe->m_rc.dy);
        	IIMAGE_SetDrawSize( pMe->m_pImageBgblack, m_ImageInfo.cx,m_ImageInfo.cy);
		}
	
    // 开始聚焦动画过程
    SetCursor(pMe, row, column);
    DrawFocusIcon(pMe);

	IDISPLAY_UpdateEx(pMe->m_pDisplay, TRUE);
}
static void MoveCursoToMove(MainMenu *pMe, int row, int column,int dx)
{
	int theFocus = pMe->m_nRow * MAX_MATRIX_COLS + pMe->m_nColumn;
    AEERect rect;
    
    // 绘制聚焦后矩阵初始界面
    #if 0
    SETAEERECT(&rect, pMe->m_IconFocus_Pt[theFocus].x+dx, 
                      pMe->m_IconFocus_Pt[theFocus].y, 
                      ICON_ANIMATED_WIDTH, 
                      ICON_ANIMATED_HEIGHT);
    
    MainMenu_DrawBackGround(pMe, &rect);
    
    if (pMe->m_pImageIcon[theFocus])
    {
        IIMAGE_Draw(pMe->m_pImageIcon[theFocus],
                    pMe->m_Icondefault_Pt[theFocus].x+dx, 
                    pMe->m_Icondefault_Pt[theFocus].y);
    }
	#endif
    // 开始聚焦动画过程
    SetCursor(pMe, row, column);
    DrawFocusIconMove(pMe,dx);
}

static void MovePen(MainMenu * pMe,int dx)
{
	DrawMatrixMove(pMe,dx);
	//MoveCursoToMove(pMe,pMe->m_nRow,pMe->m_nColumn,dx);
}

/*=============================================================================
FUNCTION:  SetCursor

DESCRIPTION: 

PARAMETERS:

RETURN VALUE:

COMMENTS:

SIDE EFFECTS:

SEE ALSO:

=============================================================================*/
static void SetCursor(MainMenu *pMe, int row, int column)
{
    pMe->m_nRow        = row;
    pMe->m_nColumn     = column;
    
    if(pMe->m_pAnimate != NULL)
    {
        IImage_Release(pMe->m_pAnimate);
        pMe->m_pAnimate  = NULL;
    }
}

/*=============================================================================
FUNCTION:  StartApplet

DESCRIPTION:  启动applet

PARAMETERS:  如果APPLET 有变动，只需改动次函数

=============================================================================*/
static int StartApplet(MainMenu *pMe, int i)
{
    int Result = EUNSUPPORTED;
    
    switch(i){
    case IDS_MAIN_MENU_GALLERY:
    case IDS_MAIN_MENU_MEDIAGALLERY:
        Result = ISHELL_StartApplet(pMe->m_pShell, AEECLSID_MEDIAGALLERY);
        break;
        
    case IDS_MAIN_MENU_CONTACT:
	case IDS_PHONEBOOK:
    case IDS_MAIN_MENU_CONTACTS:
        {
            IContactApp *ca = NULL;
            Result = ISHELL_CreateInstance(pMe->m_pShell,AEECLSID_APP_CONTACT, (void**)&ca);
            if(SUCCESS == Result)
            {
                ICONTAPP_MainMenu(ca);
                IContactApp_Release(ca);
            }
        }
        break;
        
    case IDS_MAIN_MENU_SMARTFRENACCESS:
    case IDS_MAIN_MENU_UTK:
        Result = ISHELL_StartApplet(pMe->m_pShell, AEECLSID_APP_UTK);
        break;
        
    case IDS_MAIN_MENU_TOOLS:
    case IDS_MAIN_MENU_APPLICATION:
        Result = ISHELL_StartApplet(pMe->m_pShell, AEECLSID_APPLICATION);
        break;
        
    case IDS_MAIN_MENU_RECENTCALLS:
        Result = ISHELL_StartApplet(pMe->m_pShell, AEECLSID_APP_RECENTCALL);
        break;
        
    case IDS_MAIN_MENU_SMS:
    case IDS_MAIN_MENU_MESSAGES:
        Result = ISHELL_StartApplet(pMe->m_pShell, AEECLSID_WMSAPP);
        break;
        
    case IDS_MAIN_MENU_MULTIMEDIA:
        Result = ISHELL_StartApplet(pMe->m_pShell, AEECLSID_MULTIMEDIA_LIST);
        break;
        
    case IDS_MAIN_MENU_STATIC_APPLICATION:
#if defined(FEATURE_APP_MANAGER)
#ifdef FEATURE_OEMOMH
        if(ISHELL_SendEvent(pMe->m_pShell,AEECLSID_DIALER,EVT_OMH_PROMPT,0,0))
        {
            break;
        }
#endif

        Result = ISHELL_StartApplet(pMe->m_pShell, AEECLSID_APPMANAGER);
#else
        Result = ISHELL_StartApplet(pMe->m_pShell, AEECLSID_STATIC_APP);
#endif
    break;
        
    case IDS_MAIN_MENU_CALENDAR:
    case IDS_MAIN_MENU_SCHEDULER:
        Result = ISHELL_StartApplet(pMe->m_pShell, AEECLSID_SCHEDULEAPP);
        break;
        
    case IDS_MAIN_MENU_SETTINGS:
        Result = ISHELL_StartApplet(pMe->m_pShell, AEECLSID_APP_SETTINGMENU);
        break;
        
    case IDS_MAIN_MENU_USERPROFILE:
        Result = ISHELL_StartApplet(pMe->m_pShell, AEECLSID_APP_SOUNDMENU);
        break;
        
    case IDS_MAIN_MENU_GAMES:
        Result = ISHELL_StartApplet(pMe->m_pShell, AEECLSID_GAME);
        break;
        
    case IDS_MAIN_MENU_CALCULATOR:
		 Result = ISHELL_StartApplet(pMe->m_pShell, AEECLSID_CALCAPP);
        break;
        
    case IDS_MAIN_MENU_FMRADIO:
        Result = ISHELL_StartApplet(pMe->m_pShell, AEECLSID_APP_FMRADIO);
        break;
        
    case IDS_MAIN_MENU_MUSICPLAYER:
        Result = ISHELL_StartApplet(pMe->m_pShell, AEECLSID_APP_MUSICPLAYER);
        break;
        
    case IDS_MAIN_MENU_SMARTWORLD:
    case IDS_MAIN_MENU_BLIVE:
#ifdef FEATURE_SMARTFREN_STATIC_BREW_APP
		Result = ISHELL_StartApplet(pMe->m_pShell, AEECLSID_STATIC_APP);
#else
#ifdef FEATURE_OEMOMH
        if(ISHELL_SendEvent(pMe->m_pShell,AEECLSID_DIALER,EVT_OMH_PROMPT,0,0))
        {
            break;
        }
#endif
        Result = ISHELL_StartApplet(pMe->m_pShell, AEECLSID_APPMANAGER);
#endif  
        break; 
    case IDS_MAIN_MENU_CAMERA:
        Result = ISHELL_StartApplet(pMe->m_pShell, AEECLSID_APP_CAMERA);
        break;
	 case IDS_MAIN_MENU_FM:
        Result = ISHELL_StartApplet(pMe->m_pShell, AEECLSID_APP_FMRADIO);
        break;
	 case IDS_MAIN_MENU_BT:
        Result = ISHELL_StartApplet(pMe->m_pShell, AEECLSID_BLUETOOTH_APP);
        break;
	 case IDS_MAIN_MENU_UNIT:
        Result = ISHELL_StartApplet(pMe->m_pShell, AEECLSID_CONVERTER);
        break;

	 case IDS_MAIN_MENU_ALARM:
        Result = ISHELL_StartApplet(pMe->m_pShell, AEECLSID_ALARMCLOCK);
        break;
	 case IDS_MAIN_MENU_VIDEOPLAYER:
        Result = ISHELL_StartApplet(pMe->m_pShell, AEECLSID_VIDEOPLAYER);
        break;
     case IDS_MAIN_MENU_STOPWATCH:
        Result = ISHELL_StartApplet(pMe->m_pShell, AEECLSID_STOPWATCH);
        break;
	 case IDS_MAIN_MENU_RECORDER:
        Result = ISHELL_StartApplet(pMe->m_pShell, AEECLSID_RECORDER);
        break;
	 case IDS_MAIN_MENU_WORLDCLO:
        Result = ISHELL_StartApplet(pMe->m_pShell, AEECLSID_WORLDTIME);
        break;
	 case IDS_MAIN_MENU_TIMER:
        Result = ISHELL_StartApplet(pMe->m_pShell, AEECLSID_APPTIMER);
        break;

	 case IDS_MAIN_MENU_PINTU:
        Result = ISHELL_StartApplet(pMe->m_pShell, AEECLSID_PINTU);
		MSG_FATAL("Result=========%d",Result,0,0);
        break;
	 case IDS_MAIN_MENU_TETIRS:
        Result = ISHELL_StartApplet(pMe->m_pShell, AEECLSID_TETRIS);
		break;
	 case IDS_MAIN_MENU_BRICKATTACK:
        Result = ISHELL_StartApplet(pMe->m_pShell, AEECLSID_BRICK);
        break;

	 case IDS_MAIN_MENU_BLACKJACK:
        Result = ISHELL_StartApplet(pMe->m_pShell, AEECLSID_BLACKJACK);
        break;
	 case IDS_MAIN_MENU_CALIBRA:
        Result = ISHELL_StartApplet(pMe->m_pShell, AEECLSID_ADJUSTPENAPP);
		break;
    default:
        break;

    }
    return Result;
}

#else
#ifdef	FEATURE_VERSION_K212
static boolean Main_loadover = FALSE;
#ifdef FEATURE_SOUND_BO
static char* MAIN_SOUND_NAME[] =
{
	MUSIC_PATH1,
	MUSIC_PATH2,
	MUSIC_PATH3,
	MUSIC_PATH4,
	MUSIC_PATH5,
	MUSIC_PATH6,
	MUSIC_PATH7,
	MUSIC_PATH8,
	MUSIC_PATH9,
};
#endif
#define PARAM_NOT_REF(x)
/*==============================================================================

                              本地函数声明

==============================================================================*/
static MainMenuMod gMainMenuMod;
static const VTBL( IModule) gModFuncs =
{
    MainMenuMod_AddRef,
    MainMenuMod_Release,
    MainMenuMod_CreateInstance,
    MainMenuMod_FreeResources
};
#ifdef FEATURE_SOUND_BO
static void MainMenu_PlayShutterSound(MainMenu *pMe,int key);
static void MainMenu_MediaNotify(void *pUser, AEEMediaCmdNotify *pCmdNotify);
#endif
static uint32  MainMenu_AddRef( IMainMenu *pi);

static uint32  MainMenu_Release ( IMainMenu *pi);

static boolean MainMenu_HandleEvent( IMainMenu *pi,
    AEEEvent eCode,
    uint16  wParam,
    uint32 dwParam
);

static MainMenu gMainMenu={0};

static const VTBL( IMainMenu) gMainMenuMethods =
{
    MainMenu_AddRef,
    MainMenu_Release,
    MainMenu_HandleEvent
};

int MainMenuMod_Load( IShell *pIShell, void *ph, IModule **ppMod);

int MainMenuMod_New( int16 nSize,
    IShell              *pIShell,
    void                *ph,
    IModule             **ppMod,
    PFNMODCREATEINST    pfnMC,
    PFNFREEMODDATA      pfnMF
);

static int MainMenu_New( IShell *ps, IModule *pIModule, IMainMenu **ppObj);

static int CMainMenu_InitAppData(MainMenu *pMe);

static void CMainMenu_FreeAppData(MainMenu *pMe);

static void MainMenu_RunFSM(MainMenu *pMe);

static void CalculateScreenParameters(MainMenu *pMe);

static void MainMenu_DrawBackGround(MainMenu *pMe, AEERect *pRect);

// 初始整个背景及全部初始图标

int SetBrowserArr_Main(IShell *pShell ,char *purl);

static void DrawMatrix(MainMenu *pMe);

//static void DrawFocusIcon(MainMenu *pMe);

static void MoveCursorTo(MainMenu *pMe, int row, int column);

static void SetCursor(MainMenu *pMe, int row, int column);

static int StartApplet(MainMenu *pMe, int i);

boolean MainMenu_RouteDialogEvt(MainMenu *pMe,
    AEEEvent    eCode,
    uint16  wParam,
    uint32 dwParam
);

NextFSMAction MainMenu_ProcessState(MainMenu *pMe);

void MainMenu_ShowDialog(MainMenu  *pMe,  uint16 dlgResId);

static boolean  gbMainmenuLock = FALSE;

static void Main_keypadtimer(void *pUser);
static void Main_loadtimer(void *pUser);


// MAINST_MAIN 状态处理函数
static NextFSMAction MAINST_MAIN_Handler(MainMenu *pMe);
static NextFSMAction MAINST_VIEW_TIME_Handler(MainMenu *pMe);

//MAINST_EXIT  状态处理函数
static NextFSMAction MAINST_EXIT_Handler(MainMenu *pMe);
static boolean  MainMenu_IconMenuHandler(MainMenu *pMe, AEEEvent eCode, uint16 wParam, uint32 dwParam);
static boolean  MainMenu_ViewTimeHandler(MainMenu *pMe, AEEEvent eCode, uint16 wParam, uint32 dwParam);

/*==============================================================================
                              
                              本地（静态）数据
                              
==============================================================================*/
static char* ICON_ANI[] =
{
    ICON4_ANI,//ICON1_ANI,
    ICON2_ANI,
    ICON7_ANI,//ICON3_ANI,
    ICON6_ANI,//ICON4_ANI,
    ICON5_ANI,
    ICON1_ANI,//ICON6_ANI,
    ICON3_ANI,//ICON7_ANI,
    ICON8_ANI,
    ICON9_ANI, 
};


static char* ICON_ANI_EN[] =
{
    ICON4_ANI_EN,//ICON1_ANI,
    ICON2_ANI_EN,
    ICON7_ANI_EN,//ICON3_ANI,
    ICON6_ANI_EN,//ICON4_ANI,
    ICON5_ANI_EN,
    ICON1_ANI_EN,//ICON6_ANI,
    ICON3_ANI_EN,//ICON7_ANI,
    ICON8_ANI_EN,
    ICON9_ANI_EN, 
};


static char* ICON_ANI_1[] =
{
    ICON4_ANI_1,//ICON1_ANI_1,
    ICON2_ANI_1,
    ICON7_ANI_1,//ICON3_ANI_1,
    ICON6_ANI_1,//ICON4_ANI_1,
    ICON5_ANI_1,
    ICON1_ANI_1,
    ICON3_ANI_1,
    ICON8_ANI_1,
    ICON9_ANI_1,
    ICON10_ANI_1,
    ICON11_ANI_1,
    ICON12_ANI_1,
};

/*
static char* ICON_ANI_2[] =
{
	ICON1_ANI_2,
	ICON2_ANI_2,
	ICON3_ANI_2,
	ICON4_ANI_2,
	ICON5_ANI_2,
	ICON6_ANI_2,
	ICON7_ANI_2,
	ICON8_ANI_2,
	ICON9_ANI_2,
};
*/


static char* MUSIC_PATH[] =
{
	MUSIC_PATH1,
	MUSIC_PATH2,
	MUSIC_PATH3,
	MUSIC_PATH4,
	MUSIC_PATH5,
	MUSIC_PATH6,
	MUSIC_PATH7,
	MUSIC_PATH8,
	MUSIC_PATH9,
};

/*=============================================================================
FUNCTION:  MainMenuMod_Load

DESCRIPTION: 

PARAMETERS:

RETURN VALUE:

COMMENTS:

SIDE EFFECTS:

SEE ALSO:

=============================================================================*/
int MainMenuMod_Load( IShell *pIShell, void *ph, IModule **ppMod)
{	
    return MainMenuMod_New( sizeof( MainMenuMod),
                    pIShell,
                    ph,
                    ppMod,
                    NULL,
                    NULL);
}

/*=============================================================================
FUNCTION:  MainMenuMod_New

DESCRIPTION: 

PARAMETERS:

RETURN VALUE:

COMMENTS:

SIDE EFFECTS:

SEE ALSO:

=============================================================================*/
int  MainMenuMod_New( int16 nSize,
    IShell   *pIShell,
    void     *ph,
    IModule  **ppMod,
    PFNMODCREATEINST pfnMC,
    PFNFREEMODDATA pfnMF
)
{
#if !defined( AEE_SIMULATOR)
    PARAM_NOT_REF( nSize)
    PARAM_NOT_REF( pfnMC)
    PARAM_NOT_REF( pfnMF)
    PARAM_NOT_REF( ph)
#endif

    if( NULL == ppMod)
    {
        return EFAILED;
    }

    MEMSET( &gMainMenuMod, 0, sizeof( MainMenuMod));

    INIT_VTBL( &gMainMenuMod, IModule, gModFuncs);
    gMainMenuMod.referenceCounter = 1;
    *ppMod = ( IModule *)&gMainMenuMod;
    return AEE_SUCCESS;
}

/*=============================================================================
FUNCTION:  MainMenuMod_AddRef

DESCRIPTION: 

PARAMETERS:

RETURN VALUE:

COMMENTS:

SIDE EFFECTS:

SEE ALSO:

=============================================================================*/
static uint32 MainMenuMod_AddRef( IModule *po)
{
    return( ++( ( MainMenuMod *)po)->referenceCounter);
}

/*=============================================================================
FUNCTION:  MainMenuMod_Release

DESCRIPTION: 

PARAMETERS:

RETURN VALUE:

COMMENTS:

SIDE EFFECTS:

SEE ALSO:

=============================================================================*/
static uint32 MainMenuMod_Release( IModule *po)
{
    if ( ( ( MainMenuMod *) po)->referenceCounter == 0)
    {
        return 0;
    }

    return( --( ( MainMenuMod *)po)->referenceCounter);
}

/*=============================================================================
FUNCTION:  MainMenuMod_CreateInstance

DESCRIPTION: 

PARAMETERS:

RETURN VALUE:

COMMENTS:

SIDE EFFECTS:

SEE ALSO:

=============================================================================*/
static int  MainMenuMod_CreateInstance( IModule *po,
    IShell *pIShell,
    AEECLSID ClsId,
    void **ppObj
)
{
    *ppObj = NULL;

    if( ClsId != AEECLSID_MAIN_MENU || MainMenu_New( pIShell, po, ( IMainMenu**)ppObj) != SUCCESS)
    {
        return EFAILED;
    }

    return SUCCESS;
}

/*=============================================================================
FUNCTION:  MainMenuMod_FreeResources

DESCRIPTION: 

PARAMETERS:

RETURN VALUE:

COMMENTS:

SIDE EFFECTS:

SEE ALSO:

=============================================================================*/
static void MainMenuMod_FreeResources( IModule *po, IHeap *ph, IFileMgr *pfm)
{
}

/*=============================================================================
FUNCTION:  MainMenu_New

DESCRIPTION: 

PARAMETERS:

RETURN VALUE:

COMMENTS:

SIDE EFFECTS:

SEE ALSO:

=============================================================================*/
static int MainMenu_New( IShell *ps, IModule *pIModule, IMainMenu **ppObj)
{
    int retVal = SUCCESS;
    if( 0 == gMainMenu.referenceCounter)
    {

        if( pIModule == NULL || ps == NULL)
        {
            return EFAILED;
        }

        MEMSET(&gMainMenu, 0, sizeof(gMainMenu));
        INIT_VTBL(&gMainMenu, IMainMenu, gMainMenuMethods);

       
        gMainMenu.m_pShell     = ps;
        gMainMenu.m_pModule    = pIModule;
        retVal = CMainMenu_InitAppData(&gMainMenu);
        if(retVal != SUCCESS)
        {
            CMainMenu_FreeAppData((MainMenu*)&gMainMenu);
            return retVal;
        }
        (void) ISHELL_AddRef( ps);
        (void) IMODULE_AddRef( pIModule);
        gMainMenu.referenceCounter  = 0;

        
    }

    ++ gMainMenu.referenceCounter;
    *ppObj = ( IMainMenu*)&gMainMenu;
    
    return retVal;
}

/*==============================================================================
函数:
    CMainMenuApp_InitAppData

说明:
    初始化 MainMenu Applet 结构数据缺省值。

参数:
    pMe [in]: 指向 MainMenu Applet 对象结构的指针。该结构包含小程序的特定信息。

返回值:
    EFAILED: 初始化失败。
    SUCCESS: 初始化成功。

备注:

==============================================================================*/
static int CMainMenu_InitAppData(MainMenu *pMe)
{
	int i = 0;
	int j = 0;
	int8 count_main = 1;
	pMe->language = 0;

    pMe->m_pMedia = NULL;
    
    if (NULL == pMe)
    {
        return EFAILED;
    }
	pMe->m_bChangeL =FALSE;
	OEM_GetConfig(CFGI_COUNT_OF_MAIN, &count_main, sizeof(int8));
	MSG_FATAL("CMainMenu_InitAppData.............=%d",count_main,0,0);
	pMe->m_nRow        = count_main;
    pMe->m_nColumn     = 1;		
	Main_loadover = FALSE;
    
    // 接口创建及相关初始化
    if (ISHELL_CreateInstance(pMe->m_pShell, AEECLSID_CONFIG,
            (void **)&pMe->m_pConfig) != SUCCESS)
    {    	
        return EFAILED;
    }

    if( ISHELL_CreateInstance( pMe->m_pShell, AEECLSID_BACKLIGHT, (void **)&pMe->m_pBacklight)!=AEE_SUCCESS)
    {
        return FALSE;
    }
     
    if (ISHELL_CreateInstance(pMe->m_pShell, AEECLSID_DISPLAY, 
            (void **) &pMe->m_pDisplay) != SUCCESS)
    {    	
        return EFAILED;
    }
	if (AEE_SUCCESS != ISHELL_CreateInstance(pMe->m_pShell,
                                            AEECLSID_ANNUNCIATOR,
                                            (void **)&pMe->m_pIAnn))
    {    	
        return EFAILED;
    }
    // 初始化菜单Title

	pMe->m_IconTitle[0]     = IDS_MAIN_MENU_MESSAGES;
    pMe->m_IconTitle[1]     = IDS_MAIN_MENU_CONTACTS;
    pMe->m_IconTitle[2]     = IDS_MAIN_MENU_RECENTCALLS;
    pMe->m_IconTitle[3]     = IDS_MAIN_MENU_USER_PROFILE;
    pMe->m_IconTitle[4]     = IDS_MAIN_MENU_MULTIMEDIA;
    pMe->m_IconTitle[5]     = IDS_MAIN_MENU_CAMERA;
    pMe->m_IconTitle[6]     = IDS_MAIN_MENU_TOOLS;
    pMe->m_IconTitle[7]     = IDS_MAIN_MENU_SETTINGS;
    pMe->m_IconTitle[8]     = IDS_MAIN_MENU_UTK;
    pMe->m_IconTitle[9]     = IDS_MAIN_MENU_GAMES;
    pMe->m_IconTitle[10]    = IDS_MAIN_MENU_SCHEDULER;
    pMe->m_IconTitle[11]    = IDS_MAIN_MENU_CALCULATOR;
	pMe->m_pImageBg = ISHELL_LoadImage(pMe->m_pShell,ICON_ANI_BG);
	if((pMe->m_nRow>=0) && (pMe->m_nRow<4))
	{
		pMe->m_pImageIcon[0] = ISHELL_LoadImage(pMe->m_pShell,
	                                                    ICON_ANI_1[0]);
		pMe->m_pImageIcon[1] = ISHELL_LoadImage(pMe->m_pShell,
	                                                    ICON_ANI_1[1]);
		pMe->m_pImageIcon[2] = ISHELL_LoadImage(pMe->m_pShell,
	                                                    ICON_ANI_1[2]);
		pMe->m_pImageIcon[3] = ISHELL_LoadImage(pMe->m_pShell,
	                                                    ICON_ANI_1[3]);
	}
	else if((pMe->m_nRow>=4) && (pMe->m_nRow<8))
	{
            pMe->m_pImageIcon[4] = ISHELL_LoadImage(pMe->m_pShell,
                                                    ICON_ANI_1[4]);
            pMe->m_pImageIcon[5] = ISHELL_LoadImage(pMe->m_pShell,
                                                    ICON_ANI_1[5]);
            pMe->m_pImageIcon[6] = ISHELL_LoadImage(pMe->m_pShell,
                                                    ICON_ANI_1[6]);
            pMe->m_pImageIcon[7] = ISHELL_LoadImage(pMe->m_pShell,
                                                    ICON_ANI_1[7]);
	}
	else
	{
            pMe->m_pImageIcon[5] = ISHELL_LoadImage(pMe->m_pShell,
                                                    ICON_ANI_1[5]);
            pMe->m_pImageIcon[6] = ISHELL_LoadImage(pMe->m_pShell,
                                                    ICON_ANI_1[6]);
            pMe->m_pImageIcon[7] = ISHELL_LoadImage(pMe->m_pShell,
                                                    ICON_ANI_1[7]);
            pMe->m_pImageIcon[8] = ISHELL_LoadImage(pMe->m_pShell,
                                                    ICON_ANI_1[8]);
	}
	
	pMe->m_pImageIcon[9] = ISHELL_LoadImage(pMe->m_pShell,
                                                    ICON_ANI_1[9]);
	pMe->m_pImageIcon[10] = ISHELL_LoadImage(pMe->m_pShell,
                                                    ICON_ANI_1[10]);
	(void) ICONFIG_GetItem(pMe->m_pConfig,
									   CFGI_LANGUAGE_SELECTION,
									   &pMe->language,
									   sizeof(pMe->language));
	if(pMe->language == NV_LANGUAGE_ENGLISH)
	{
		pMe->m_pImageSelectk212[pMe->m_nRow] = ISHELL_LoadImage(pMe->m_pShell,
                                                    ICON_ANI_EN[pMe->m_nRow]);
		pMe->m_bChangeL =TRUE;
	}
	else
	{
		pMe->m_pImageSelectk212[pMe->m_nRow] = ISHELL_LoadImage(pMe->m_pShell,
                                                    ICON_ANI[pMe->m_nRow]);
		pMe->m_bChangeL =FALSE;
	}
	//pMe->m_pImageSelectk212 = ISHELL_LoadImage(pMe->m_pShell, ICON_ANI[pMe->m_nRow]);

	pMe->m_pImageSelectkbar = ISHELL_LoadImage(pMe->m_pShell, ICON_ANI_BAR);

	pMe->m_pAnimate = ISHELL_LoadImage(pMe->m_pShell, ICON12_ANI_1);
	MSG_FATAL("CMainMenu_InitAppData.............over.....",0,0,0);
    return SUCCESS;
}

/*==============================================================================
函数:
    CMainMenu_FreeAppData

说明:
    释放 MainMenu Applet 相关资源。

参数:
    pMe [in]: 指向MainMenu Applet 对象结构的指针。该结构包含小程序的特定信息。

返回值:
    none

备注:

==============================================================================*/
static void CMainMenu_FreeAppData(MainMenu *pMe)
{
    if (NULL == pMe)
    {
        return;
    }

    pMe->m_eAppStatus = MAINMENU_STOP;
    if (NULL != pMe->m_pConfig)
    {
        ICONFIG_Release(pMe->m_pConfig);
        pMe->m_pConfig = NULL;
    }    
    
    if (pMe->m_pDisplay != NULL)
    {
        (void) IDISPLAY_Release(pMe->m_pDisplay);
        pMe->m_pDisplay = NULL;
    }
	if (pMe->m_pIAnn)
    {
        IANNUNCIATOR_Release(pMe->m_pIAnn);
		pMe->m_pIAnn = NULL;
    }

    if(pMe->m_pBacklight)
    {
        IBACKLIGHT_Release(pMe->m_pBacklight);
        pMe->m_pBacklight=NULL;
    }
	if(pMe->m_pMedia)
    {
        IMEDIA_Release(pMe->m_pMedia);
        pMe->m_pMedia = NULL;
    }
    //释放图片资源
    Main_loadover = FALSE;
    {
         int i;
         int j;
         if (pMe->m_pImageBg !=NULL)
         {
             (void) IIMAGE_Release(pMe->m_pImageBg);
             pMe->m_pImageBg = NULL;
         }
 
         for(i=0;i<MAX_MATRIX_ITEMS;i++)
         {
             if(pMe->m_pImageIcon[i] != NULL)
             {
                 (void)IIMAGE_Release(pMe->m_pImageIcon[i]);
                 pMe->m_pImageIcon[i] = NULL;
             }
         } 
 
 		for(j=0;j<9;j++)
         {
             if(pMe->m_pImageSelectk212[j] != NULL)
             {
                 (void)IIMAGE_Release(pMe->m_pImageSelectk212[j]);
                 pMe->m_pImageSelectk212[j] = NULL;
             }
         } 
 
 		if(pMe->m_pImageSelectkbar!=NULL)
 		{
 			(void)IIMAGE_Release(pMe->m_pImageSelectkbar);
             pMe->m_pImageSelectkbar = NULL;
 		}
         
         if(pMe->m_pAnimate != NULL)
         {
             (void)IIMAGE_Release(pMe->m_pAnimate);
             pMe->m_pAnimate = NULL;
         }
    }  
}

/*=============================================================================
FUNCTION:  MainMenu_AddRef

DESCRIPTION: 

PARAMETERS:

RETURN VALUE:

COMMENTS:

SIDE EFFECTS:

SEE ALSO:

=============================================================================*/
static uint32  MainMenu_AddRef( IMainMenu *pi)
{
    return ( ++gMainMenu.referenceCounter);
}

/*=============================================================================
FUNCTION:  MainMenu_Release

DESCRIPTION: 

PARAMETERS:

RETURN VALUE:

COMMENTS:

SIDE EFFECTS:

SEE ALSO:

=============================================================================*/
static uint32  MainMenu_Release( IMainMenu *pi)
{
    register MainMenu *pMe = (MainMenu*)pi;
    if (pMe->referenceCounter == 0)
    {
        return 0;
    }
    
    if( --pMe->referenceCounter)
    {
        return pMe->referenceCounter;
    }


    CMainMenu_FreeAppData(pMe);

    (void) ISHELL_Release(pMe->m_pShell);
    (void) IMODULE_Release(pMe->m_pModule);
    return 0;
}

/*==============================================================================
函数:
    MainMenu_RunFSM

说明:
    运行有限状态机引擎。

参数:
    pMe [in]: 指向MainMenu Applet对象结构的指针。该结构包含小程序的特定信息。
       
返回值:
    无。

备注:

==============================================================================*/
static void MainMenu_RunFSM(MainMenu *pMe)
{
    NextFSMAction nextFSMAction = NFSMACTION_WAIT;
    for ( ; ; )
    {
        nextFSMAction = MainMenu_ProcessState(pMe);

        // 这里不需要复杂的状态机，因此为简化状态机设计这里仅用一个布尔变量来
        // 指示dlgResult是否应该被改写。在某些特殊场合（见状态机设计），这种作
        // 法，避免了不必要的复杂性，简化了设计。
        if (pMe->m_bDoNotOverwriteDlgResult)
        {
            // We allow only one iteration without touching this variable. So,
            // let's reset it back to indicate that we are going to overwrite the
            // result for all subsequent iterations.
            pMe->m_bDoNotOverwriteDlgResult = FALSE;
        }
        else
        {
            // Overwrite the dlgResult to its default value. The next dialog that
            // we are going to display will set and return the dlgResult based on
            // the users action.
            pMe->m_eDlgReturn = DLGRET_CREATE;
        }

        // If the state machine processing engine indicates that it created a
        // dialog and requesting us to exit this loop, then we quit.
        if (nextFSMAction == NFSMACTION_WAIT)
        {
            break;
        }
    }
}

/*=============================================================================
FUNCTION:  MainMenu_HandleEvent

DESCRIPTION: 

PARAMETERS:

RETURN VALUE:

COMMENTS:

SIDE EFFECTS:

SEE ALSO:

=============================================================================*/
static boolean MainMenu_HandleEvent( IMainMenu *pi,
    AEEEvent eCode,
    uint16  wParam,
    uint32 dwParam
)
{
    MainMenu *pMe = (MainMenu*)pi;
    AEEAppStart* as = NULL;
    switch ( eCode)
    {
        case EVT_APP_START:
#ifdef FEATURE_RANDOM_MENU_REND
            DisplayRend_Enable(TRUE);
#endif
            // 此事件dwParam为指针，不应为0
            if (dwParam == 0) 
            {
                return FALSE;
            }            
            as = (AEEAppStart*)dwParam;
   
            pMe->m_rc = as->rc;

            pMe->m_currState = MAINST_MAIN;
            pMe->m_eDlgReturn = DLGRET_CREATE;
            pMe->m_eAppStatus = MAINMENU_RUNNING;
            IANNUNCIATOR_SetFieldIsActiveEx(pMe->m_pIAnn,TRUE); 
            MainMenu_RunFSM(pMe);
            return TRUE;

        case EVT_APP_STOP:
            {
                int theFocus = 4;
                SetCursor( pMe, theFocus / 3, theFocus % 3);
                (void)ISHELL_CancelTimer( pMe->m_pShell, NULL, pMe);
                pMe->m_eAppStatus = MAINMENU_STOP;
#ifdef FEATURE_RANDOM_MENU_REND
                DisplayRend_Enable(FALSE);
#endif
                return TRUE;
            }

        case EVT_APP_SUSPEND:
            (void)ISHELL_CancelTimer(pMe->m_pShell, NULL, pMe);
            pMe->m_eAppStatus = MAINMENU_SUSPEND;
#ifdef FEATURE_RANDOM_MENU_REND
            DisplayRend_Push();
            DisplayRend_Start(FALSE);
#endif
            return TRUE;
        

        case EVT_APP_RESUME:
            {
                AEEAppStart* as = ( AEEAppStart*)dwParam;
                pMe->m_rc    = as->rc;
                pMe->m_eAppStatus = MAINMENU_RUNNING;
#ifdef FEATURE_RANDOM_MENU_REND
                DisplayRend_Push();
                DisplayRend_Start(TRUE);
#endif
                // 开始跑WMS状态机
                MainMenu_RunFSM(pMe); 
                return TRUE;
            }

        case EVT_DIALOG_INIT:
            pMe->m_pActiveIDlg = (IDialog*)dwParam;
            pMe->m_pActivedlgID = wParam;
            return MainMenu_RouteDialogEvt(pMe,eCode,wParam,dwParam);

        case EVT_DIALOG_START:
            return MainMenu_RouteDialogEvt(pMe,eCode,wParam,dwParam);

        case EVT_USER_REDRAW:
            return MainMenu_RouteDialogEvt(pMe,eCode,wParam,dwParam);

        case EVT_DIALOG_END:

            (void) MainMenu_RouteDialogEvt(pMe,eCode,wParam,dwParam);
            pMe->m_pActiveIDlg = NULL;
            pMe->m_pActivedlgID = 0;
            if(pMe->m_eAppStatus == MAINMENU_RUNNING)
            {
                MainMenu_RunFSM(pMe);
            }
            return TRUE;
        case EVT_KEY:
#if MIN_BREW_VERSION(3,0)
            // do not want to handle au
            if (((dwParam & KB_AUTOREPEAT) != 0) &&
                (wParam != AVK_UP) &&
                (wParam != AVK_DOWN) &&
                (wParam != AVK_LEFT) &&
                (wParam != AVK_RIGHT))
            {
                return TRUE;
            }
#endif
            return MainMenu_RouteDialogEvt(pMe,eCode,wParam,dwParam);  
#if defined (FEATURE_VERSION_N021)||defined(FEATURE_VERSION_W0216A)
		case EVT_KEY_HELD:
			{
				if(wParam == AVK_INFO)
				{
					(void) ISHELL_PostEvent(pMe->m_pShell,
                                        AEECLSID_CORE_APP,
                                        EVT_USER,
                                        1,
                                        0);
                    ISHELL_CloseApplet(pMe->m_pShell,TRUE);
				}
			}
			break;
#endif
        case EVT_KEY_PRESS:
        case EVT_KEY_RELEASE:
        case EVT_COMMAND:
            return MainMenu_RouteDialogEvt(pMe,eCode,wParam,dwParam); 

        default:
            return MainMenu_RouteDialogEvt(pMe,eCode,wParam,dwParam);
    }
}

void Mainmenu_KeypadLock(boolean block)
{
   gbMainmenuLock = block;
}

/*==============================================================================
函数:
    MainMenu_ProcessState

说明:
    根据 pMe->m_currState, 调用相应的状态处理函数。

参数:
    pMe [in]: 指向MainMenu Applet对象结构的指针。该结构包含小程序的特定信息。

返回值:
    NFSMACTION_CONTINUE: 指示不停状态机。
    NFSMACTION_WAIT: 指示停止状态机。

备注:

==============================================================================*/
NextFSMAction MainMenu_ProcessState(MainMenu *pMe)
{
    NextFSMAction retVal = NFSMACTION_WAIT;
    if (NULL == pMe)
    {
        return retVal;
    }
    switch(pMe->m_currState)
    {
        case MAINST_MAIN:
            return MAINST_MAIN_Handler(pMe);
		case MAINST_VIEW_TIME:
			return MAINST_VIEW_TIME_Handler(pMe);
        case MAINST_EXIT:
            return MAINST_EXIT_Handler(pMe);  
        default:
            break;

    }
    return retVal;
}

/*==============================================================================
函数:
    MAINST_MAIN_Handler

说明:
    MAINST_MAIN 状态处理函数。

参数:
    pMe [in]: 指向MAINMENU Applet对象结构的指针。该结构包含小程序的特定信息。

返回值:
    NFSMACTION_CONTINUE: 指示不停状态机。
    NFSMACTION_WAIT: 指示停止状态机。

备注:

==============================================================================*/
static NextFSMAction MAINST_MAIN_Handler(MainMenu *pMe)
{
    if (NULL == pMe)
    {
        return NFSMACTION_WAIT;
    }
    switch (pMe->m_eDlgReturn)
    {
        // 进入主界面
        case DLGRET_CREATE:
            {
                MainMenu_ShowDialog(pMe, IDD_MAIN_MENU);
            }
            return NFSMACTION_WAIT;
            
        case DLGRET_CANCELED:
            MOVE_TO_STATE(MAINST_EXIT)
            return NFSMACTION_CONTINUE;
		case DLGRET_VIEW_TIEM:
			MOVE_TO_STATE(MAINST_VIEW_TIME)
            return NFSMACTION_CONTINUE;

        default:
            MOVE_TO_STATE(MAINST_EXIT)
            return NFSMACTION_CONTINUE;
    }
}


static NextFSMAction MAINST_VIEW_TIME_Handler(MainMenu *pMe)
{
    if (NULL == pMe)
    {
        return NFSMACTION_WAIT;
    }
    switch (pMe->m_eDlgReturn)
    {
        // 进入主界面
        case DLGRET_CREATE:
            {
                MainMenu_ShowDialog(pMe, IDD_VIEW_TIME);
            }
            return NFSMACTION_WAIT;
            
        case DLGRET_CANCELED:
            MOVE_TO_STATE(MAINST_EXIT)
            return NFSMACTION_CONTINUE;

        default:
            MOVE_TO_STATE(MAINST_EXIT)
            return NFSMACTION_CONTINUE;
    }
}


/*==============================================================================
函数:
    MAINST_EXIT_Handler

说明:
    MAINST_EXIT 状态处理函数。

参数:
    pMe [in]: 指向MAINMENU  Applet对象结构的指针。该结构包含小程序的特定信息。

返回值:
    NFSMACTION_CONTINUE: 指示不停状态机。
    NFSMACTION_WAIT: 指示停止状态机。

备注:

==============================================================================*/
static NextFSMAction MAINST_EXIT_Handler(MainMenu *pMe)
{
    (void) ISHELL_CloseApplet(pMe->m_pShell, FALSE);
    return NFSMACTION_WAIT;
} 

/*==============================================================================
函数:
       MainMenu_ShowDialog

说明:
    函数由状态处理函数用来唤起相应对话框资源ID的对话框。

参数:
    pMe [in]: 指向 MAINMENU Applet 对象结构的指针。该结构包含小程序的特定信息。
    dlgResId [in]: 对话框资源ID，函数根据该ID创建对话框。

返回值:
    none

备注:

==============================================================================*/
void MainMenu_ShowDialog(MainMenu  *pMe,  uint16 dlgResId)
{
    int nRet;

    // 每次最多打开一个对话框
    if (ISHELL_GetActiveDialog(pMe->m_pShell) != NULL)
    {
        // 如果当前已经有对话框被打开，直接返回
        return;
    }
   
    if (NULL != pMe->m_pDisplay)
    {
        AEEDeviceInfo di={0,};
#if defined (FEATURE_VERSION_H19C)
        if(pMe->m_pIAnn != NULL)
        {
            if (dlgResId == IDD_MAIN_MENU)
            {
                IANNUNCIATOR_SetHasTitleText(pMe->m_pIAnn, FALSE);
            }
            else
            {
                IANNUNCIATOR_SetHasTitleText(pMe->m_pIAnn, TRUE);
            }
            IANNUNCIATOR_Redraw(pMe->m_pIAnn);
        }
        (void)IDISPLAY_SetPrefs(pMe->m_pDisplay, "a:1", STRLEN("a:1"));
#else
        if (dlgResId == IDD_MAIN_MENU)
        {
            (void)IDISPLAY_SetPrefs(pMe->m_pDisplay, "a:0", STRLEN("a:0"));
        }
        else
        {
            (void)IDISPLAY_SetPrefs(pMe->m_pDisplay, "a:1", STRLEN("a:1"));
        }
#endif
        ISHELL_GetDeviceInfo(pMe->m_pShell, &di);
        pMe->m_rc.dx = di.cxScreen;
        pMe->m_rc.dy = di.cyScreen;       
        IDISPLAY_SetClipRect(pMe->m_pDisplay, &pMe->m_rc);
        CalculateScreenParameters(pMe);
    }
    
    nRet = ISHELL_CreateDialog(pMe->m_pShell,MAINMENU_RES_FILE_LANG,dlgResId,NULL);
    if (nRet != SUCCESS)
    {
    }
}


/*==============================================================================
函数:
       MainMenu_RouteDialogEvt
说明:
       函数将BREW事件路由给当前活动对话框的事件处理函数。

参数:
       pMe [In]: 指向SettingMenu Applet对象结构的指针,该结构包含小程序的特定信息.
       eCode [in]：事件代码。
       wParam [in]：与事件eCode关联的数据。
       dwParam [in]：与事件eCode关联的数据。

返回值:
       TRUE：传入事件被处理。
       FALSE：传入事件没被处理。

备注:

==============================================================================*/
boolean MainMenu_RouteDialogEvt(MainMenu *pMe,
    AEEEvent    eCode,
    uint16  wParam,
    uint32 dwParam
)

{
    if (NULL == pMe)
    {
        return FALSE;
    }

    if (NULL == pMe->m_pActiveIDlg)
    {
        return FALSE;
    }    
    switch( pMe->m_pActivedlgID)
    {
        case IDD_MAIN_MENU:
            return MainMenu_IconMenuHandler(pMe, eCode, wParam, dwParam);
        case IDD_VIEW_TIME:
			return MainMenu_ViewTimeHandler(pMe, eCode, wParam, dwParam);
        default:
            return FALSE;
    }
}
static boolean  MainMenu_ViewTimeHandler(MainMenu *pMe, AEEEvent eCode, uint16 wParam, uint32 dwParam)
{
	 PARAM_NOT_REF( dwParam)
    switch ( eCode)
    {
        case EVT_DIALOG_INIT:			

            return TRUE;

        case EVT_DIALOG_START:			

            (void) ISHELL_PostEvent( pMe->m_pShell,
                                     AEECLSID_MAIN_MENU,
                                     EVT_USER_REDRAW,
                                     0,
                                     0);
			MSG_FATAL("MainMenu_IconMenuHandler.........EVT_DIALOG_START....",0,0,0);
            return TRUE;

        case EVT_USER_REDRAW:
            // 初始整个背景及全部初始图标
            {
            	
				IDisplay      *pd = NULL;
    
    			(void) ISHELL_CreateInstance(pMe->m_pShell,AEECLSID_DISPLAY,(void**) &pd);
    			if(pd)
    			{
        			Appscommon_Draw_Keyguard_Time(pd);
        			IDISPLAY_Release(pd);
    			}
				OEMKeyguard_SetState(TRUE);
				ISHELL_CloseApplet(pMe->m_pShell, TRUE); 		
            	return TRUE;
            }
        case EVT_KEY:
            switch( wParam)
            {
                case AVK_CLR:
                     CLOSE_DIALOG(DLGRET_CANCELED)
                    return TRUE;
            }
        case EVT_DIALOG_END:

            return TRUE;
    }
}

/*=============================================================================
FUNCTION:  MainMenu_IconMenuHandler

DESCRIPTION: 12宫格菜单

PARAMETERS:

=============================================================================*/
static boolean MainMenu_IconMenuHandler(MainMenu *pMe, AEEEvent eCode, uint16 wParam, uint32 dwParam)
{
    PARAM_NOT_REF( dwParam)
    switch ( eCode)
    {
        case EVT_DIALOG_INIT:			
            IDIALOG_SetProperties((IDialog *)dwParam, DLG_NOT_REDRAW_AFTER_START);
			MSG_FATAL("MainMenu_IconMenuHandler.........init....",0,0,0);
            return TRUE;

        case EVT_DIALOG_START:			
			AEE_CancelTimer(Main_keypadtimer,pMe);
			AEE_SetTimer(5*1000,Main_keypadtimer,pMe);
            (void) ISHELL_PostEvent( pMe->m_pShell,
                                     AEECLSID_MAIN_MENU,
                                     EVT_USER_REDRAW,
                                     0,
                                     0);
			MSG_FATAL("MainMenu_IconMenuHandler.........EVT_DIALOG_START....",0,0,0);
            return TRUE;

        case EVT_USER_REDRAW:
            // 初始整个背景及全部初始图标
            {
            	
            	MSG_FATAL("MainMenu_IconMenuHandler.........EVT_USER_REDRAW....",0,0,0);
            	DrawMatrix(pMe);
				MSG_FATAL("MainMenu_IconMenuHandler.........EVT_USER_REDRAW..over..",0,0,0);
				//AEE_CancelTimer(Main_loadtimer,pMe);
				//AEE_SetTimer(100,Main_loadtimer,pMe);
            	return TRUE;
            }
            
        case EVT_DIALOG_END:
            {
                ISHELL_CancelTimer(pMe->m_pShell, NULL, (void**)pMe);
            }
            return TRUE;

        case EVT_KEY_HELD:
            
        #if defined(FEATURE_TORCH_KEY_INFO)
           if((AVKType)wParam == AVK_INFO)
            {
                boolean TorchOn = FALSE;
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
                ISHELL_CloseApplet(pMe->m_pShell, TRUE); 
                return TRUE;
            }
        #endif
	 #if defined(FEATURE_TORCH_KEY_SELECT) //xxzhen
           if((AVKType)wParam == AVK_SELECT)
            {
                boolean TorchOn = FALSE;
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
                ISHELL_CloseApplet(pMe->m_pShell, TRUE); 
                return TRUE;
            }
        #endif

            return TRUE;
        case EVT_KEY:
            switch( wParam)
            {
                case AVK_CLR:
                     CLOSE_DIALOG(DLGRET_CANCELED)
                    return TRUE;
                    
                case AVK_UP:
                    if(pMe->m_nRow>=4)
                    {
                    	pMe->m_nRow = pMe->m_nRow-4;
						MoveCursorTo(pMe,pMe->m_nRow,pMe->m_nColumn);
                    }
                    return TRUE;
                    
                case AVK_DOWN:
                  	if(pMe->m_nRow<4)
                    {
                    	pMe->m_nRow = pMe->m_nRow+4;
						MoveCursorTo(pMe,pMe->m_nRow,pMe->m_nColumn);
                    }
                    return TRUE;
                    
                case AVK_LEFT:
                   if(pMe->m_nRow>0)
                   {
                   	   pMe->m_nRow = pMe->m_nRow-1;
                   }
				   else if(pMe->m_nRow == 0)
				   {
				   		pMe->m_nRow = 8;
				   }
				   MoveCursorTo(pMe,pMe->m_nRow,pMe->m_nColumn);
                   return TRUE;
                    
                case AVK_RIGHT:

                   if(pMe->m_nRow<8)
                   {
                   	   pMe->m_nRow = pMe->m_nRow+1;
					  
                   }
				   else if(pMe->m_nRow == 8)
				   {
				   		pMe->m_nRow = 0;
				   }
				   MoveCursorTo(pMe,pMe->m_nRow,pMe->m_nColumn);
                   return TRUE;
                    
                case AVK_SELECT:
                case AVK_INFO:
                    {
					   int8 count_main = pMe->m_nRow;
					   MSG_FATAL("count_main=======%d",count_main,0,0);
					   OEM_SetConfig(CFGI_COUNT_OF_MAIN, &count_main, sizeof(int8));
                       StartApplet(pMe, pMe->m_IconTitle[pMe->m_nRow]);
                    }
                    return TRUE;
                case AVK_STAR:
                    if(gbMainmenuLock)
                    {
                    	m_main_avk_star = TRUE;
						OEMKeyguard_SetState(TRUE);
                        ISHELL_CloseApplet(pMe->m_pShell, TRUE); 
						
						//CLOSE_DIALOG(DLGRET_VIEW_TIEM)
                    	return TRUE;
                    }
                    return TRUE;
                default:
                    break;
            }

            return TRUE;
            
        default:
            break;
    }

    return FALSE;
}

static void Main_keypadtimer(void *pUser)
{
    gbMainmenuLock =FALSE;
}
static void Main_loadtimer(void *pUser)
{
	MainMenu	*pMe = (MainMenu *)pUser;
	int i = 0;
	int j = 0;
	for (i = 0; i < MAX_MATRIX_ITEMS; i ++)
    {
        if (pMe->m_pImageIcon[i] == NULL)
        {
            pMe->m_pImageIcon[i] = ISHELL_LoadImage(pMe->m_pShell,
                                                    ICON_ANI_1[i]);
        }
    }  
	for(j=0;j<9;j++)
	{
		if (pMe->m_pImageSelectk212[j] == NULL)
        {
            pMe->m_pImageSelectk212[j] = ISHELL_LoadImage(pMe->m_pShell,
                                                    ICON_ANI[j]);
        }
	}
	Main_loadover = TRUE;
	
}

/*=============================================================================
FUNCTION:  CalculateScreenParameters

DESCRIPTION: 计算焦点图片以及默认图片的坐标

=============================================================================*/
static void CalculateScreenParameters(MainMenu *pMe)
{
    AEEImageInfo    imageInfoIcon;
    uint16          iconSpaceHorizontal = 0;
    uint16          iconSpaceVertical   = 0;
    uint8           i                   = 0;
	
    /* icon size in all might be larger than screen*/  
    pMe->m_Icondefault_Pt[0].x = 35;
    pMe->m_Icondefault_Pt[0].y = 10;
	
    pMe->m_Icondefault_Pt[1].x = 4;
    pMe->m_Icondefault_Pt[1].y = 231;
	
    pMe->m_Icondefault_Pt[2].x = 229;
    pMe->m_Icondefault_Pt[2].y = 231;

    pMe->m_Icondefault_Pt[3].x = 16;
    pMe->m_Icondefault_Pt[3].y = 213;
	
    pMe->m_Icondefault_Pt[4].x = 68;
    pMe->m_Icondefault_Pt[4].y = 213;
	
    pMe->m_Icondefault_Pt[5].x = 120;
    pMe->m_Icondefault_Pt[5].y = 213;

    pMe->m_Icondefault_Pt[6].x = 172;
    pMe->m_Icondefault_Pt[6].y = 213;
	
    
  
}

static void MainMenu_DrawBackGround(MainMenu *pMe, AEERect *pRect)
{

    Appscommon_ResetBackground(pMe->m_pDisplay, 
                                              pMe->m_pImageBg, 
                                              APPSCOMMON_BG_COLOR, 
                                              pRect, 
                                              0, 
                                              0);

}

/*=============================================================================
FUNCTION:  DrawMatrix

DESCRIPTION: // 初始整个背景及全部初始图标

=============================================================================*/
static void DrawMatrix(MainMenu *pMe)
{
    
	BottomBar_e_Type    eBBarType = BTBAR_NONE;
	BottomBar_Param_type BarParam; 
    boolean m_sound_bo_main = FALSE;
    if (NULL == pMe)
    {
        return;
    }
    //draw bg image
    MainMenu_DrawBackGround(pMe, &pMe->m_rc);
	(void) ICONFIG_GetItem(pMe->m_pConfig,
									   CFGI_LANGUAGE_SELECTION,
									   &pMe->language,
									   sizeof(pMe->language));
	if(pMe->m_pAnimate != NULL)
	{
		int row = pMe->m_nRow;
		if(row == 8)
	    {
	    	row = 3;
	    }
		else
		{
			row = row %4;
		}
		IIMAGE_Draw(pMe->m_pAnimate,
                    pMe->m_Icondefault_Pt[3+row].x,
                    pMe->m_Icondefault_Pt[3+row].y);
	}

	if((pMe->m_nRow>=0) && (pMe->m_nRow<4))
	{
		if(pMe->m_pImageIcon[0]!=NULL)
		{
			IIMAGE_Draw(pMe->m_pImageIcon[0],
                        pMe->m_Icondefault_Pt[3].x,
                        pMe->m_Icondefault_Pt[3].y);
		}
		if(pMe->m_pImageIcon[1]!=NULL)
		{
			IIMAGE_Draw(pMe->m_pImageIcon[1],
                        pMe->m_Icondefault_Pt[4].x,
                        pMe->m_Icondefault_Pt[4].y);
		}
		if(pMe->m_pImageIcon[2]!=NULL)
		{
			IIMAGE_Draw(pMe->m_pImageIcon[2],
                        pMe->m_Icondefault_Pt[5].x,
                        pMe->m_Icondefault_Pt[5].y);
		}
		if(pMe->m_pImageIcon[3]!=NULL)
		{
			IIMAGE_Draw(pMe->m_pImageIcon[3],
                        pMe->m_Icondefault_Pt[6].x,
                        pMe->m_Icondefault_Pt[6].y);
		}
	}
	else if((pMe->m_nRow>=4) && (pMe->m_nRow<8))
	{
		if(pMe->m_pImageIcon[4]!=NULL)
		{
			IIMAGE_Draw(pMe->m_pImageIcon[4],
                        pMe->m_Icondefault_Pt[3].x,
                        pMe->m_Icondefault_Pt[3].y);
		}
		if(pMe->m_pImageIcon[5]!=NULL)
		{
			IIMAGE_Draw(pMe->m_pImageIcon[5],
                        pMe->m_Icondefault_Pt[4].x,
                        pMe->m_Icondefault_Pt[4].y);
		}
		if(pMe->m_pImageIcon[6]!=NULL)
		{
			IIMAGE_Draw(pMe->m_pImageIcon[6],
                        pMe->m_Icondefault_Pt[5].x,
                        pMe->m_Icondefault_Pt[5].y);
		}
		if(pMe->m_pImageIcon[7]!=NULL)
		{
			IIMAGE_Draw(pMe->m_pImageIcon[7],
                        pMe->m_Icondefault_Pt[6].x,
                        pMe->m_Icondefault_Pt[6].y);
		}
	}
	else
	{
		if(pMe->m_pImageIcon[5]!=NULL)
		{
			IIMAGE_Draw(pMe->m_pImageIcon[5],
                        pMe->m_Icondefault_Pt[3].x,
                        pMe->m_Icondefault_Pt[3].y);
		}
		if(pMe->m_pImageIcon[6]!=NULL)
		{
			IIMAGE_Draw(pMe->m_pImageIcon[6],
                        pMe->m_Icondefault_Pt[4].x,
                        pMe->m_Icondefault_Pt[4].y);
		}

		if(pMe->m_pImageIcon[7]!=NULL)
		{
			IIMAGE_Draw(pMe->m_pImageIcon[7],
                        pMe->m_Icondefault_Pt[5].x,
                        pMe->m_Icondefault_Pt[5].y);
		}

		if(pMe->m_pImageIcon[8]!=NULL)
		{
			IIMAGE_Draw(pMe->m_pImageIcon[8],
                        pMe->m_Icondefault_Pt[6].x,
                        pMe->m_Icondefault_Pt[6].y);
		}
	}
	if(pMe->m_pImageIcon[9]!=NULL)
	{
		IIMAGE_Draw(pMe->m_pImageIcon[9],
                        pMe->m_Icondefault_Pt[1].x,
                        pMe->m_Icondefault_Pt[1].y);
	}
	if(pMe->m_pImageIcon[10]!=NULL)
	{
		IImage_Draw(pMe->m_pImageIcon[10],
                        pMe->m_Icondefault_Pt[2].x,
                        pMe->m_Icondefault_Pt[2].y);
	}
	
	if(pMe->m_pImageSelectk212[pMe->m_nRow] !=NULL)
	{
		if(pMe->language == NV_LANGUAGE_ENGLISH)
		{
			if(pMe->m_bChangeL != TRUE)
			{
				(void)IIMAGE_Release(pMe->m_pImageSelectk212[pMe->m_nRow]);
            	 pMe->m_pImageSelectk212[pMe->m_nRow] = NULL;
			}
		}
		else
		{
			if(pMe->m_bChangeL != FALSE)
			{
				(void)IIMAGE_Release(pMe->m_pImageSelectk212[pMe->m_nRow]);
            	pMe->m_pImageSelectk212[pMe->m_nRow] = NULL;
			}
		}
	}

	if(pMe->m_pImageSelectk212[pMe->m_nRow] ==NULL)
	{
		if(pMe->language == NV_LANGUAGE_ENGLISH)
		{
			pMe->m_pImageSelectk212[pMe->m_nRow] = ISHELL_LoadImage(pMe->m_pShell,
	                                                    ICON_ANI_EN[pMe->m_nRow]);
		}
		else
		{
			pMe->m_pImageSelectk212[pMe->m_nRow] = ISHELL_LoadImage(pMe->m_pShell,
                                                    ICON_ANI[pMe->m_nRow]);
		}
	}
	
	if(pMe->m_pImageSelectk212[pMe->m_nRow] != NULL)
	{
		IIMAGE_Draw(pMe->m_pImageSelectk212[pMe->m_nRow],
                        pMe->m_Icondefault_Pt[0].x,
                        pMe->m_Icondefault_Pt[0].y);
	}
	if(pMe->m_pImageSelectkbar!=NULL)
	{
		//IIMAGE_Draw(pMe->m_pImageSelectkbar,0,285);
	}
    #ifdef FEATURE_SOUND_BO
   	(void) ICONFIG_GetItem(pMe->m_pConfig,
                                 CFGI_SOUND_BO_MAIN,
                                 &m_sound_bo_main,
                                sizeof(boolean));
	if(m_sound_bo_main)
	{
        /*
		nv_item_type	SimChoice;
		(void)OEMNV_Get(NV_SIM_SELECT_I,&SimChoice);
		if(SimChoice.sim_select != 1)
        */   
        if (GetMp3PlayerStatus() == MP3STATUS_NONE)
		{
			MainMenu_PlayShutterSound(pMe,pMe->m_nRow);
		}
	}
	#endif
	//eBBarType = BTBAR_OK_BACK;
	//DrawBottomBar_Ex(pMe->m_pShell, pMe->m_pDisplay,eBBarType);
    MEMSET(&BarParam, 0, sizeof(BarParam));         
    BarParam.eBBarType = BTBAR_OK_BACK;                         
    DrawBottomBar(pMe->m_pDisplay, &BarParam);   
	IDISPLAY_UpdateEx(pMe->m_pDisplay, TRUE);
    //BarParam.eBBarType = BTBAR_SELECT_BACK;
    //DrawBottomBar(pMe->m_pDisplay, &BarParam);//wlh 20090412 add
}

/*=============================================================================
FUNCTION:  DrawFocusIcon

DESCRIPTION: 

PARAMETERS:

RETURN VALUE:

COMMENTS:


=============================================================================*/
//static void DrawFocusIcon(MainMenu *pMe)
//{
        
//	    IDISPLAY_UpdateEx(pMe->m_pDisplay, TRUE);
//}

/*=============================================================================
FUNCTION:  MoveCursorTo

DESCRIPTION: 

PARAMETERS:

RETURN VALUE:

COMMENTS:

SIDE EFFECTS:

SEE ALSO:

=============================================================================*/
static void MoveCursorTo(MainMenu *pMe, int row, int column)
{
	BottomBar_e_Type    eBBarType = BTBAR_NONE;
	BottomBar_Param_type BarParam; 
	boolean m_sound_bo_main = FALSE;
	//draw bg image
	 if (NULL == pMe)
    {
        return;
    }
	(void) ICONFIG_GetItem(pMe->m_pConfig,
									   CFGI_LANGUAGE_SELECTION,
									   &pMe->language,
									   sizeof(pMe->language));
	MSG_FATAL("row=======%d",row,0,0);
#ifdef FEATURE_SOUND_BO

	(void) ICONFIG_GetItem(pMe->m_pConfig,
                                 CFGI_SOUND_BO_MAIN,
                                 &m_sound_bo_main,
                                 sizeof(boolean));
	if(m_sound_bo_main)
	{
	    /*
		nv_item_type	SimChoice;
		(void)OEMNV_Get(NV_SIM_SELECT_I,&SimChoice);
		if(SimChoice.sim_select != 1)
        */    
        if (GetMp3PlayerStatus() == MP3STATUS_NONE)
		{
			MainMenu_PlayShutterSound(pMe,row);
		}
	}
#endif
    MainMenu_DrawBackGround(pMe, &pMe->m_rc);
    if(row == 8)
    {
    	row = 3;
    }
	else
	{
		row = row %4;
	}
	if(pMe->m_pAnimate != NULL)
	{
			IIMAGE_Draw(pMe->m_pAnimate,
                        pMe->m_Icondefault_Pt[3+row].x,
                        pMe->m_Icondefault_Pt[3+row].y);
	}

	if((pMe->m_nRow>=0) && (pMe->m_nRow<4))
	{
		if (pMe->m_pImageIcon[0] == NULL)
        {
            pMe->m_pImageIcon[0] = ISHELL_LoadImage(pMe->m_pShell,
                                                    ICON_ANI_1[0]);
        }
		if (pMe->m_pImageIcon[1] == NULL)
        {
            pMe->m_pImageIcon[1] = ISHELL_LoadImage(pMe->m_pShell,
                                                    ICON_ANI_1[1]);
        }
		if (pMe->m_pImageIcon[2] == NULL)
        {
            pMe->m_pImageIcon[2] = ISHELL_LoadImage(pMe->m_pShell,
                                                    ICON_ANI_1[2]);
        }
		if (pMe->m_pImageIcon[3] == NULL)
        {
            pMe->m_pImageIcon[3] = ISHELL_LoadImage(pMe->m_pShell,
                                                    ICON_ANI_1[3]);
        }
		IIMAGE_Draw(pMe->m_pImageIcon[0],
                        pMe->m_Icondefault_Pt[3].x,
                        pMe->m_Icondefault_Pt[3].y);
		IIMAGE_Draw(pMe->m_pImageIcon[1],
                        pMe->m_Icondefault_Pt[4].x,
                        pMe->m_Icondefault_Pt[4].y);
		IIMAGE_Draw(pMe->m_pImageIcon[2],
                        pMe->m_Icondefault_Pt[5].x,
                        pMe->m_Icondefault_Pt[5].y);
		IIMAGE_Draw(pMe->m_pImageIcon[3],
                        pMe->m_Icondefault_Pt[6].x,
                        pMe->m_Icondefault_Pt[6].y);
	}
	else if((pMe->m_nRow>=4) && (pMe->m_nRow<8))
	{
		if (pMe->m_pImageIcon[4] == NULL)
        {
            pMe->m_pImageIcon[4] = ISHELL_LoadImage(pMe->m_pShell,
                                                    ICON_ANI_1[4]);
        }
		if (pMe->m_pImageIcon[5] == NULL)
        {
            pMe->m_pImageIcon[5] = ISHELL_LoadImage(pMe->m_pShell,
                                                    ICON_ANI_1[5]);
        }
		if (pMe->m_pImageIcon[6] == NULL)
        {
            pMe->m_pImageIcon[6] = ISHELL_LoadImage(pMe->m_pShell,
                                                    ICON_ANI_1[6]);
        }
		if (pMe->m_pImageIcon[7] == NULL)
        {
            pMe->m_pImageIcon[7] = ISHELL_LoadImage(pMe->m_pShell,
                                                    ICON_ANI_1[7]);
        }
		IIMAGE_Draw(pMe->m_pImageIcon[4],
                        pMe->m_Icondefault_Pt[3].x,
                        pMe->m_Icondefault_Pt[3].y);
		IIMAGE_Draw(pMe->m_pImageIcon[5],
                        pMe->m_Icondefault_Pt[4].x,
                        pMe->m_Icondefault_Pt[4].y);
		IIMAGE_Draw(pMe->m_pImageIcon[6],
                        pMe->m_Icondefault_Pt[5].x,
                        pMe->m_Icondefault_Pt[5].y);
		IIMAGE_Draw(pMe->m_pImageIcon[7],
                        pMe->m_Icondefault_Pt[6].x,
                        pMe->m_Icondefault_Pt[6].y);
	}
	else
	{
		
		if (pMe->m_pImageIcon[5] == NULL)
        {
            pMe->m_pImageIcon[5] = ISHELL_LoadImage(pMe->m_pShell,
                                                    ICON_ANI_1[5]);
        }
		if (pMe->m_pImageIcon[6] == NULL)
        {
            pMe->m_pImageIcon[6] = ISHELL_LoadImage(pMe->m_pShell,
                                                    ICON_ANI_1[6]);
        }
		if (pMe->m_pImageIcon[7] == NULL)
        {
            pMe->m_pImageIcon[7] = ISHELL_LoadImage(pMe->m_pShell,
                                                    ICON_ANI_1[7]);
        }
		if (pMe->m_pImageIcon[8] == NULL)
        {
            pMe->m_pImageIcon[8] = ISHELL_LoadImage(pMe->m_pShell,
                                                    ICON_ANI_1[8]);
        }
		IIMAGE_Draw(pMe->m_pImageIcon[5],
                        pMe->m_Icondefault_Pt[3].x,
                        pMe->m_Icondefault_Pt[3].y);
		IIMAGE_Draw(pMe->m_pImageIcon[6],
                        pMe->m_Icondefault_Pt[4].x,
                        pMe->m_Icondefault_Pt[4].y);
		IIMAGE_Draw(pMe->m_pImageIcon[7],
                        pMe->m_Icondefault_Pt[5].x,
                        pMe->m_Icondefault_Pt[5].y);
		IIMAGE_Draw(pMe->m_pImageIcon[8],
                        pMe->m_Icondefault_Pt[6].x,
                        pMe->m_Icondefault_Pt[6].y);
	}
	

	IIMAGE_Draw(pMe->m_pImageIcon[9],
                        pMe->m_Icondefault_Pt[1].x,
                        pMe->m_Icondefault_Pt[1].y);
	IIMAGE_Draw(pMe->m_pImageIcon[10],
                        pMe->m_Icondefault_Pt[2].x,
                        pMe->m_Icondefault_Pt[2].y);
	
	if(pMe->m_pImageSelectk212[pMe->m_nRow] !=NULL)
	{
		if(pMe->language == NV_LANGUAGE_ENGLISH)
		{
			if(pMe->m_bChangeL != TRUE)
			{
				(void)IIMAGE_Release(pMe->m_pImageSelectk212[pMe->m_nRow]);
            	 pMe->m_pImageSelectk212[pMe->m_nRow] = NULL;
			}
		}
		else
		{
			if(pMe->m_bChangeL != FALSE)
			{
				(void)IIMAGE_Release(pMe->m_pImageSelectk212[pMe->m_nRow]);
            	pMe->m_pImageSelectk212[pMe->m_nRow] = NULL;
			}
		}
	}
	
	if(pMe->m_pImageSelectk212[pMe->m_nRow] ==NULL)
	{
		if(pMe->language == NV_LANGUAGE_ENGLISH)
		{
			pMe->m_pImageSelectk212[pMe->m_nRow] = ISHELL_LoadImage(pMe->m_pShell,
	                                                    ICON_ANI_EN[pMe->m_nRow]);
		}
		else
		{
			pMe->m_pImageSelectk212[pMe->m_nRow] = ISHELL_LoadImage(pMe->m_pShell,
                                                    ICON_ANI[pMe->m_nRow]);
		}
	}
	if(pMe->m_pImageSelectk212[pMe->m_nRow] != NULL)
	{
		
		IIMAGE_Draw(pMe->m_pImageSelectk212[pMe->m_nRow],
                        pMe->m_Icondefault_Pt[0].x,
                        pMe->m_Icondefault_Pt[0].y);
	}

	if(pMe->m_pImageSelectkbar!=NULL)
	{
		//IIMAGE_Draw(pMe->m_pImageSelectkbar,0,285);
	}
	
	//eBBarType = BTBAR_OK_BACK;
	//DrawBottomBar_Ex(pMe->m_pShell, pMe->m_pDisplay,eBBarType);
	MEMSET(&BarParam, 0, sizeof(BarParam));         
    BarParam.eBBarType = BTBAR_OK_BACK;                         
    DrawBottomBar(pMe->m_pDisplay, &BarParam);   
	IDISPLAY_UpdateEx(pMe->m_pDisplay, TRUE);
}
/*=============================================================================
FUNCTION:  SetCursor

DESCRIPTION: 

PARAMETERS:

RETURN VALUE:

COMMENTS:

SIDE EFFECTS:

SEE ALSO:

=============================================================================*/
static void SetCursor(MainMenu *pMe, int row, int column)
{
    pMe->m_nRow        = row;
    pMe->m_nColumn     = column;
    
    if(pMe->m_pAnimate != NULL)
    {
        IImage_Release(pMe->m_pAnimate);
        pMe->m_pAnimate  = NULL;
    }
}

/*=============================================================================
FUNCTION:  StartApplet

DESCRIPTION:  启动applet

PARAMETERS:  如果APPLET 有变动，只需改动次函数

=============================================================================*/
static int StartApplet(MainMenu *pMe, int i)
{
    int Result = EUNSUPPORTED;
	//uint32  dwTotal = 0;
	//uint32 free = 0;
	//GETFSFREE(&dwTotal);
	//free = GETRAMFREE(NULL,NULL);
	//MSG_FATAL("dwTotal======%d,free===%d",dwTotal,free,0);

    
#ifdef FEATURE_VERSION_H19C  
    if(pMe->m_pIAnn != NULL)
    {
        IANNUNCIATOR_SetHasTitleText(pMe->m_pIAnn, TRUE);
    }
#endif
		#ifdef FEATURE_VERSION_K202_LM129C //xxzhen
		  if(pMe->m_pIAnn != NULL)
          {
              IANNUNCIATOR_SetHasTitleText(pMe->m_pIAnn, TRUE);
          }
		#endif

    switch(i)
	{
#ifdef FEATURE_VERSION_W208S
	case IDS_MAIN_MENU_MAP:
	{
		char	buf[12];
		
		OEM_SetBROWSER_ADSAccount();	//Add By zzg 2011_07_08
		
    	MSG_FATAL("AEECLSID_BRW_APP...........START",0,0,0);
    	start_info.appid_fx = APP_ORIGINATOR_BROWSER;
        start_info.subtype_keycode = APP_BROWSER_START_URL;
        start_info.par_keychar = 0;

		start_info.fInfo.url_info.title = "map";
		start_info.fInfo.url_info.url = "http://wap.movidamovil.com/venezuela/?debug=true";		
		
        //start_info.fInfo.url_info.title = "163";
        //start_info.fInfo.url_info.url = "http://wap.163.com";

		
        SPRINTF(buf, "%p", &start_info);
		Result = ISHELL_StartAppletArgs(pMe->m_pShell,AEECLSID_NF3,buf);
        //Result = ISHELL_StartApplet(pMe->m_pShell, AEECLSID_BRW_APP);
        MSG_FATAL("AEECLSID_BRW_APP...........Result=%d",Result,0,0);		
		
		
		break;
	}
	case IDS_MAIN_MENU_NEO_APP:
	{
        Result = ISHELL_StartApplet(pMe->m_pShell, AEECLSID_APPMANAGER);
		break;
	}
	case IDS_MAIN_MENU_NEO_NAV:
	{
		char	buf[12];
		
		OEM_SetBROWSER_ADSAccount();	//Add By zzg 2011_07_08
		
    	MSG_FATAL("AEECLSID_BRW_APP...........START",0,0,0);
    	start_info.appid_fx = APP_ORIGINATOR_BROWSER;
        start_info.subtype_keycode = APP_BROWSER_START_MAINMENU;
        start_info.par_keychar = 0;
        //start_info.fInfo.url_info.title = "163";
        //start_info.fInfo.url_info.url = "http://www.163.com";
        SPRINTF(buf, "%p", &start_info);
		Result = ISHELL_StartAppletArgs(pMe->m_pShell,AEECLSID_NF3,buf);
        //Result = ISHELL_StartApplet(pMe->m_pShell, AEECLSID_BRW_APP);
        MSG_FATAL("AEECLSID_BRW_APP...........Result=%d",Result,0,0);		
		
		break;
	}
#endif

    case IDS_MAIN_MENU_FILEMGR:
    case IDS_MAIN_MENU_GALLERY:
	case IDS_MAIN_MENU_MEDIAGALLERY_C337:	
    case IDS_MAIN_MENU_MEDIAGALLERY:
        Result = ISHELL_StartApplet(pMe->m_pShell, AEECLSID_MEDIAGALLERY);
        break;
        
    case IDS_MAIN_MENU_CONTACT:
	case IDS_PHONEBOOK:
    case IDS_MAIN_MENU_CONTACTS:
        {
            IContactApp *ca = NULL;
            Result = ISHELL_CreateInstance(pMe->m_pShell,AEECLSID_APP_CONTACT, (void**)&ca);
            if(SUCCESS == Result)
            {
                ICONTAPP_MainMenu(ca);
                IContactApp_Release(ca);
            }
        }
        break;
    case IDS_MAIN_MENU_INTEXZONE: 
#ifdef FEATURE_VERSION_C316
		Result = ISHELL_StartApplet(pMe->m_pShell, AEECLSID_INDEX_ZONE);
		break;
#endif
    case IDS_MAIN_MENU_SMARTFRENACCESS:
    case IDS_MAIN_MENU_UTK:
        Result = ISHELL_StartApplet(pMe->m_pShell, AEECLSID_APP_UTK);
        break;
        
    case IDS_MAIN_MENU_TOOLS:
	case IDS_MAIN_MENU_ORGANIZER:	
    case IDS_MAIN_MENU_APPLICATION:
        Result = ISHELL_StartApplet(pMe->m_pShell, AEECLSID_APPLICATION);
        break;
		
    case IDS_MAIN_MENU_CALL_LOGS:
	case IDS_MAIN_MENU_RECENTCALLS_C337:	
    case IDS_MAIN_MENU_RECENTCALLS:
        Result = ISHELL_StartApplet(pMe->m_pShell, AEECLSID_APP_RECENTCALL);
        break;
        
    case IDS_MAIN_MENU_SMS:
    case IDS_MAIN_MENU_MESSAGES:
        Result = ISHELL_StartApplet(pMe->m_pShell, AEECLSID_WMSAPP);
        break;
        
    case IDS_MAIN_MENU_MULTIMEDIA:
        Result = ISHELL_StartApplet(pMe->m_pShell, AEECLSID_MULTIMEDIA_LIST);
        break;
        
    case IDS_MAIN_MENU_STATIC_APPLICATION:
#if defined(FEATURE_APP_MANAGER)
#ifdef FEATURE_OEMOMH
        if(ISHELL_SendEvent(pMe->m_pShell,AEECLSID_DIALER,EVT_OMH_PROMPT,0,0))
        {
            break;
        }
#endif

        Result = ISHELL_StartApplet(pMe->m_pShell, AEECLSID_APPMANAGER);
#else
        Result = ISHELL_StartApplet(pMe->m_pShell, AEECLSID_STATIC_APP);
#endif
    break;
        
    case IDS_MAIN_MENU_CALENDAR:
    case IDS_MAIN_MENU_SCHEDULER:
        Result = ISHELL_StartApplet(pMe->m_pShell, AEECLSID_SCHEDULEAPP);
        break;
        
    case IDS_MAIN_MENU_SETTINGS:
        Result = ISHELL_StartApplet(pMe->m_pShell, AEECLSID_APP_SETTINGMENU);
        break;

	//Add By zzg 2012_11_08 for C337 
	case IDS_MAIN_MENU_SERVICES:		
	    #if defined (FEATURE_VERSION_C337) || defined(FEATURE_VERSION_IC241A_MMX)
        #if defined (FEATURE_VERSION_IN50A)
        Result = SetBrowserArr_Main(pMe->m_pShell,(char*)"http://www.google.com.hk/"); 
        #else
        Result = SetBrowserArr_Main(pMe->m_pShell,(char*)"http://mimicromax.com"); //ISHELL_StartAppletArgs(pMe->m_pShell, AEECLSID_UCWEB, (char*)"call_ucweb:setmainpageurl:http://mimicromax.com");
        #endif
        #else
        Result = SetBrowserArr_Main(pMe->m_pShell,NULL);//ISHELL_StartApplet(pMe->m_pShell, AEECLSID_UCWEB);
        #endif
		break;
	case IDS_MAIN_MENU_MSTORE:
#if defined (FEATURE_VERSION_C337) || defined(FEATURE_VERSION_IC241A_MMX)
#if defined (FEATURE_VERSION_IN50A)
        Result = SetBrowserArr_Main(pMe->m_pShell,(char*)"http://www.google.com.hk/");
#else
        Result = SetBrowserArr_Main(pMe->m_pShell,(char*)"http://mimicromax.com"); //ISHELL_StartAppletArgs(pMe->m_pShell, AEECLSID_UCWEB, (char*)"call_ucweb:setmainpageurl:http://mimicromax.com");      
#endif
#else
        Result = Result = SetBrowserArr_Main(pMe->m_pShell,NULL);//ISHELL_StartAppletArgs(pMe->m_pShell, AEECLSID_UCWEB, (char*)"call_ucweb:setmainpageurl:http://mimicromax.com");
#endif
		break;
	case IDS_MAIN_MENU_MZONE:
		Result = ISHELL_StartApplet(pMe->m_pShell, AEECLSID_MiZone);
		break;
	//Add End
	
	case IDS_MAIN_MENU_USER_PROFILE:	
    case IDS_MAIN_MENU_USERPROFILE:
        Result = ISHELL_StartApplet(pMe->m_pShell, AEECLSID_APP_SOUNDMENU);
        break;
        
    case IDS_MAIN_MENU_GAMES:
        Result = ISHELL_StartApplet(pMe->m_pShell, AEECLSID_GAME);
        break;
		
	#ifdef FEATURE_VERSION_C316
    case IDS_MAIN_MENU_SERVICES_C316:  
		Result = ISHELL_StartApplet(pMe->m_pShell, AEECLSID_SERVICE);
        break;
	#endif	
	
    case IDS_MAIN_WAPBROWSER:
		#ifdef FEATURE_VERSION_K202_LM129C
		  IANNUNCIATOR_SetHasTitleText(pMe->m_pIAnn,FALSE); //xxzhen
		   IANNUNCIATOR_Redraw(pMe->m_pIAnn);
		#endif

        #if defined (FEATURE_VERSION_C337) || defined(FEATURE_VERSION_IC241A_MMX)
        #if defined (FEATURE_VERSION_IN50A)
        Result = SetBrowserArr_Main(pMe->m_pShell,(char*)"http://www.google.com.hk/"); 
        #else
        Result = SetBrowserArr_Main(pMe->m_pShell,(char*)"http://mimicromax.com"); //ISHELL_StartAppletArgs(pMe->m_pShell, AEECLSID_UCWEB, (char*)"call_ucweb:setmainpageurl:http://mimicromax.com");
        #endif        
        #else
        Result = SetBrowserArr_Main(pMe->m_pShell,(char*)""); //ISHELL_StartApplet(pMe->m_pShell, AEECLSID_UCWEB);
        #endif
        break;
    
    case IDS_MAIN_MENU_CALCULATOR: 
		 Result = ISHELL_StartApplet(pMe->m_pShell, AEECLSID_CALCAPP);
        break;
        
    case IDS_MAIN_MENU_FMRADIO:
        Result = ISHELL_StartApplet(pMe->m_pShell, AEECLSID_APP_FMRADIO);
        break;
        
    case IDS_MAIN_MENU_MUSICPLAYER:
        Result = ISHELL_StartApplet(pMe->m_pShell, AEECLSID_APP_MUSICPLAYER);
        break;
        
    case IDS_MAIN_MENU_SMARTWORLD:
    case IDS_MAIN_MENU_BLIVE:
#ifdef FEATURE_SMARTFREN_STATIC_BREW_APP
		Result = ISHELL_StartApplet(pMe->m_pShell, AEECLSID_STATIC_APP);
#else
#ifdef FEATURE_OEMOMH
        if(ISHELL_SendEvent(pMe->m_pShell,AEECLSID_DIALER,EVT_OMH_PROMPT,0,0))
        {
            break;
        }
#endif
        Result = ISHELL_StartApplet(pMe->m_pShell, AEECLSID_APPMANAGER);
#endif  
        break;
    
    case IDS_MAIN_MENU_FRENDUO:
#if defined(FEATURE_VERSION_M8) || defined(FEATURE_VERSION_M8P) || defined (FEATURE_VERSION_M8021)
		Result = ISHELL_StartApplet(pMe->m_pShell, AEECLSID_FRENDUO);
#endif
        break;
    
    case IDS_MAIN_MENU_TIMER:
        Result = ISHELL_StartApplet(pMe->m_pShell, AEECLSID_APPTIMER);
        break;
        
    case IDS_MAIN_MENU_CAMERA:
	{		
		Result = ISHELL_StartApplet(pMe->m_pShell, AEECLSID_APP_CAMERA);
		break;
	}  
    
    default:
        break;

    }
    return Result;
}

extern char charsvc_p_name[UIM_CDMA_HOME_SERVICE_SIZE+1];
int SetBrowserArr_Main(IShell *pShell ,char *purl)
{
	int Result = EUNSUPPORTED;
	char urlCan[1024] = {0};
    
	DBGPRINTF("svc_p_name %s %d",charsvc_p_name,charsvc_p_name[0],0);
	OEM_SetUCBROWSER_ADSAccount();
    
	if(purl && STRLEN(purl)>1)
	{
        SPRINTF(urlCan,"call_ucweb:setexternurl:%s\2\3",purl);
		STRCAT(urlCan,"useragent:BREW-Applet/0x20068888 (BREW/3.1.5.20; DeviceId: 8976509865757e; Lang: hi; Profile/MIDP-2.0_Configuration/CLDC-1.1) ucweb-squid\2\3");
	}
	else
	{
		STRCPY(urlCan,"useragent:BREW-Applet/0x20068888 (BREW/3.1.5.20; DeviceId: 8976509865757e; Lang: hi; Profile/MIDP-2.0_Configuration/CLDC-1.1) ucweb-squid\2\3");
	}
    
	if(STRISTR (charsvc_p_name,"mts"))
	{
		MSG_FATAL("mst................",0,0,0);
        STRCAT(urlCan,"access_point:proxy_is:10.50.5.140:8080");
	}
	else if(STRISTR (charsvc_p_name,"tata"))
	{
        MSG_FATAL("tata................",0,0,0);
        STRCAT(urlCan,"access_point:proxy_is:172.23.252.15:9401");
	}
	else if(STRISTR (charsvc_p_name,"reliance"))
	{
        MSG_FATAL("reliance................",0,0,0);
        STRCAT(urlCan,"access_point:proxy_is:http://wapgw.ricinfo.com:8080");
	}
	else if(STRISTR (charsvc_p_name,"vmi"))
	{
        MSG_FATAL("vmi................",0,0,0);
        STRCAT(urlCan,"access_point:proxy_is:172.23.142.15:9401");
	}
	
	DBGPRINTF("urlCan==%s", urlCan);
    if(urlCan[0])
    {
	    Result = ISHELL_StartAppletArgs(pShell, AEECLSID_UCWEB, (char*)urlCan);
    }
    else
    {
        Result = ISHELL_StartApplet(pShell, AEECLSID_UCWEB);
    }

	return Result;	
}
#ifdef FEATURE_SOUND_BO

static void MainMenu_PlayShutterSound(MainMenu *pMe,int key)
{
    AEEMediaCmdNotify cmd;
	int temp = 0;
	char music_name[256] = {0};
    if(pMe->m_pMedia)
	{
		IMEDIA_Stop(pMe->m_pMedia);
		IMEDIA_Release(pMe->m_pMedia);
		pMe->m_pMedia = NULL;
	}
    // 如果pMe->m_pMedia接口为空，创建接口
    if(!pMe->m_pMedia)
    {
        AEEMediaData      md;
       
        if(!pMe)
           return;
        md.clsData = MMD_FILE_NAME;  		   
		md.pData = (void *)MAIN_SOUND_NAME[key];
        md.dwSize = 0;
       
        (void)AEEMediaUtil_CreateMedia(pMe->m_pShell, &md, &pMe->m_pMedia);
    }
   
    if(pMe->m_pMedia)
    {        
        IMEDIA_SetVolume(pMe->m_pMedia, AEE_MAX_VOLUME*3/5); //max volum is 100
     
        if(IMEDIA_RegisterNotify(pMe->m_pMedia, MainMenu_MediaNotify, pMe) != SUCCESS)
        {
            cmd.nCmd    = MM_CMD_PLAY;
            cmd.nStatus = MM_STATUS_DONE;
            MainMenu_MediaNotify((void *)pMe, &cmd);
            return;
        }
        MSG_FATAL("MainMenu_PlayShutterSound,IMEDIA_Play....",0,0,0);
        if(IMEDIA_Play(pMe->m_pMedia) != SUCCESS)
        {
            cmd.nCmd    = MM_CMD_PLAY;
            cmd.nStatus = MM_STATUS_DONE;
            MainMenu_MediaNotify((void *)pMe, &cmd);
            return;
        }
    }
    else
    {
        cmd.nCmd    = MM_CMD_PLAY;
        cmd.nStatus = MM_STATUS_DONE;
        MainMenu_MediaNotify((void *)pMe, &cmd);
    }
}


static void MainMenu_MediaNotify(void *pUser, AEEMediaCmdNotify *pCmdNotify)
{
    MainMenu *pMe = (MainMenu *)pUser;

    if(!pMe || !pCmdNotify)
        return;

    if(pCmdNotify->nCmd == MM_CMD_PLAY)  // IMEDIA_Play events
    {
        switch (pCmdNotify->nStatus)
        {
            case MM_STATUS_ABORT:            
                break;

            case MM_STATUS_DONE:    // playback done
				if(pMe->m_pMedia)
				{
					IMEDIA_Release(pMe->m_pMedia);
					pMe->m_pMedia = NULL;
				}
                break;
            default:
                break;
        }
    }
}
#endif
#elif defined (FEATURE_VERSION_K212_ND)
static boolean Main_loadover = FALSE;
#ifdef FEATURE_SOUND_BO

#if 0   //FEATURE_CTA
static char* MAIN_SOUND_NAME[] =
{
	MUSIC_PATH1,
	MUSIC_PATH2,
	MUSIC_PATH3,
	MUSIC_PATH4,
	MUSIC_PATH5,
	/*MUSIC_PATH6,*/
	MUSIC_PATH7,
	MUSIC_PATH8,
};
#else
static char* MAIN_SOUND_NAME[] =
{
	MUSIC_PATH1,
	MUSIC_PATH2,
	MUSIC_PATH3,
	MUSIC_PATH4,
	MUSIC_PATH5,
	MUSIC_PATH6,
	MUSIC_PATH7,
	MUSIC_PATH8,
};
#endif
#endif
#define PARAM_NOT_REF(x)
/*==============================================================================

                              本地函数声明

==============================================================================*/
static MainMenuMod gMainMenuMod;
static const VTBL( IModule) gModFuncs =
{
    MainMenuMod_AddRef,
    MainMenuMod_Release,
    MainMenuMod_CreateInstance,
    MainMenuMod_FreeResources
};
#ifdef FEATURE_SOUND_BO
static void MainMenu_PlayShutterSound(MainMenu *pMe,int key);
static void MainMenu_MediaNotify(void *pUser, AEEMediaCmdNotify *pCmdNotify);
#endif
static uint32  MainMenu_AddRef( IMainMenu *pi);

static uint32  MainMenu_Release ( IMainMenu *pi);

static boolean MainMenu_HandleEvent( IMainMenu *pi,
    AEEEvent eCode,
    uint16  wParam,
    uint32 dwParam
);

static MainMenu gMainMenu={0};

static const VTBL( IMainMenu) gMainMenuMethods =
{
    MainMenu_AddRef,
    MainMenu_Release,
    MainMenu_HandleEvent
};

int MainMenuMod_Load( IShell *pIShell, void *ph, IModule **ppMod);

int MainMenuMod_New( int16 nSize,
    IShell              *pIShell,
    void                *ph,
    IModule             **ppMod,
    PFNMODCREATEINST    pfnMC,
    PFNFREEMODDATA      pfnMF
);

static int MainMenu_New( IShell *ps, IModule *pIModule, IMainMenu **ppObj);

static int CMainMenu_InitAppData(MainMenu *pMe);

static void CMainMenu_FreeAppData(MainMenu *pMe);

static void MainMenu_RunFSM(MainMenu *pMe);

static void MainMenu_DrawBackGround(MainMenu *pMe, AEERect *pRect);

// 初始整个背景及全部初始图标

int SetBrowserArr_Main(IShell *pShell ,char *purl);

static void DrawMatrix(MainMenu *pMe);

static void MoveCursorTo(MainMenu *pMe, int index);

static void SetCursor(MainMenu *pMe, int index);

static int StartApplet(MainMenu *pMe, int i);

boolean MainMenu_RouteDialogEvt(MainMenu *pMe,
    AEEEvent    eCode,
    uint16  wParam,
    uint32 dwParam
);

NextFSMAction MainMenu_ProcessState(MainMenu *pMe);

void MainMenu_ShowDialog(MainMenu  *pMe,  uint16 dlgResId);

static boolean  gbMainmenuLock = FALSE;

static void Main_keypadtimer(void *pUser);
static void Main_loadtimer(void *pUser);


// MAINST_MAIN 状态处理函数
static NextFSMAction MAINST_MAIN_Handler(MainMenu *pMe);
static NextFSMAction MAINST_FLASHLIGHT_Handler(MainMenu *pMe);        //Add By zzg 2013_08_20
//MAINST_EXIT  状态处理函数
static NextFSMAction MAINST_EXIT_Handler(MainMenu *pMe);
static boolean  MainMenu_IconMenuHandler(MainMenu *pMe, AEEEvent eCode, uint16 wParam, uint32 dwParam);
static boolean  MainMenu_FlashlightMenuHandler(MainMenu *pMe, AEEEvent eCode, uint16 wParam, uint32 dwParam); //Add By zzg 2013_08_20

/*==============================================================================
                              
                              本地（静态）数据
                              
==============================================================================*/
#if 0   //FEATURE_CTA
	static char* ICON_ANI[] =
	{
		ICON1_ANI,
		ICON2_ANI,
		ICON3_ANI,
		ICON4_ANI,
		ICON5_ANI,
		/*ICON6_ANI,*/
		ICON7_ANI,
		ICON8_ANI,
	};
	
	static char* MUSIC_PATH[] =
	{
		MUSIC_PATH1,
		MUSIC_PATH2,
		MUSIC_PATH3,
		MUSIC_PATH4,
		MUSIC_PATH5,
		/*MUSIC_PATH6,*/
		MUSIC_PATH7,
		MUSIC_PATH8,
	};
#else
	static char* ICON_ANI[] =
	{
		ICON1_ANI,
		ICON2_ANI,
		ICON3_ANI,
		ICON4_ANI,
		ICON5_ANI,
		ICON6_ANI,
		ICON7_ANI,
		ICON8_ANI,
	};
	
	static char* MUSIC_PATH[] =
	{
		MUSIC_PATH1,
		MUSIC_PATH2,
		MUSIC_PATH3,
		MUSIC_PATH4,
		MUSIC_PATH5,
		MUSIC_PATH6,
		MUSIC_PATH7,
		MUSIC_PATH8,
	};
#endif
	
	/*=============================================================================
	FUNCTION:  MainMenuMod_Load
	
	DESCRIPTION: 
	
	PARAMETERS:
	
	RETURN VALUE:
	
	COMMENTS:
	
	SIDE EFFECTS:
	
	SEE ALSO:
	
	=============================================================================*/
	int MainMenuMod_Load( IShell *pIShell, void *ph, IModule **ppMod)
	{	
		return MainMenuMod_New( sizeof( MainMenuMod),
						pIShell,
						ph,
						ppMod,
						NULL,
						NULL);
	}
	
	/*=============================================================================
	FUNCTION:  MainMenuMod_New
	
	DESCRIPTION: 
	
	PARAMETERS:
	
	RETURN VALUE:
	
	COMMENTS:
	
	SIDE EFFECTS:
	
	SEE ALSO:
	
	=============================================================================*/
	int  MainMenuMod_New( int16 nSize,
		IShell	 *pIShell,
		void	 *ph,
		IModule  **ppMod,
		PFNMODCREATEINST pfnMC,
		PFNFREEMODDATA pfnMF
	)
	{
#if !defined( AEE_SIMULATOR)
		PARAM_NOT_REF( nSize)
		PARAM_NOT_REF( pfnMC)
		PARAM_NOT_REF( pfnMF)
		PARAM_NOT_REF( ph)
#endif
	
		if( NULL == ppMod)
		{
			return EFAILED;
		}
	
		MEMSET( &gMainMenuMod, 0, sizeof( MainMenuMod));
	
		INIT_VTBL( &gMainMenuMod, IModule, gModFuncs);
		gMainMenuMod.referenceCounter = 1;
		*ppMod = ( IModule *)&gMainMenuMod;
		return AEE_SUCCESS;
	}
	
	/*=============================================================================
	FUNCTION:  MainMenuMod_AddRef
	
	DESCRIPTION: 
	
	PARAMETERS:
	
	RETURN VALUE:
	
	COMMENTS:
	
	SIDE EFFECTS:
	
	SEE ALSO:
	
	=============================================================================*/
	static uint32 MainMenuMod_AddRef( IModule *po)
	{
		return( ++( ( MainMenuMod *)po)->referenceCounter);
	}
	
	/*=============================================================================
	FUNCTION:  MainMenuMod_Release
	
	DESCRIPTION: 
	
	PARAMETERS:
	
	RETURN VALUE:
	
	COMMENTS:
	
	SIDE EFFECTS:
	
	SEE ALSO:
	
	=============================================================================*/
	static uint32 MainMenuMod_Release( IModule *po)
	{
		if ( ( ( MainMenuMod *) po)->referenceCounter == 0)
		{
			return 0;
		}
	
		return( --( ( MainMenuMod *)po)->referenceCounter);
	}
	
	/*=============================================================================
	FUNCTION:  MainMenuMod_CreateInstance
	
	DESCRIPTION: 
	
	PARAMETERS:
	
	RETURN VALUE:
	
	COMMENTS:
	
	SIDE EFFECTS:
	
	SEE ALSO:
	
	=============================================================================*/
	static int	MainMenuMod_CreateInstance( IModule *po,
		IShell *pIShell,
		AEECLSID ClsId,
		void **ppObj
	)
	{
		*ppObj = NULL;
	
		if( ClsId != AEECLSID_MAIN_MENU || MainMenu_New( pIShell, po, ( IMainMenu**)ppObj) != SUCCESS)
		{
			return EFAILED;
		}
	
		return SUCCESS;
	}
	
	/*=============================================================================
	FUNCTION:  MainMenuMod_FreeResources
	
	DESCRIPTION: 
	
	PARAMETERS:
	
	RETURN VALUE:
	
	COMMENTS:
	
	SIDE EFFECTS:
	
	SEE ALSO:
	
	=============================================================================*/
	static void MainMenuMod_FreeResources( IModule *po, IHeap *ph, IFileMgr *pfm)
	{
	}
	
	/*=============================================================================
	FUNCTION:  MainMenu_New
	
	DESCRIPTION: 
	
	PARAMETERS:
	
	RETURN VALUE:
	
	COMMENTS:
	
	SIDE EFFECTS:
	
	SEE ALSO:
	
	=============================================================================*/
	static int MainMenu_New( IShell *ps, IModule *pIModule, IMainMenu **ppObj)
	{
		int retVal = SUCCESS;
		if( 0 == gMainMenu.referenceCounter)
		{
	
			if( pIModule == NULL || ps == NULL)
			{
				return EFAILED;
			}
	
			MEMSET(&gMainMenu, 0, sizeof(gMainMenu));
			INIT_VTBL(&gMainMenu, IMainMenu, gMainMenuMethods);
	
		   
			gMainMenu.m_pShell	   = ps;
			gMainMenu.m_pModule    = pIModule;
			retVal = CMainMenu_InitAppData(&gMainMenu);
			if(retVal != SUCCESS)
			{
				CMainMenu_FreeAppData((MainMenu*)&gMainMenu);
				return retVal;
			}
			(void) ISHELL_AddRef( ps);
			(void) IMODULE_AddRef( pIModule);
			gMainMenu.referenceCounter	= 0;
	
			
		}
	
		++ gMainMenu.referenceCounter;
		*ppObj = ( IMainMenu*)&gMainMenu;
		
		return retVal;
	}
	
	/*==============================================================================
	函数:
		CMainMenuApp_InitAppData
	
	说明:
		初始化 MainMenu Applet 结构数据缺省值。
	
	参数:
		pMe [in]: 指向 MainMenu Applet 对象结构的指针。该结构包含小程序的特定信息。
	
	返回值:
		EFAILED: 初始化失败。
		SUCCESS: 初始化成功。
	
	备注:
	
	==============================================================================*/
	static int CMainMenu_InitAppData(MainMenu *pMe)
	{		
		int i = 0;
		pMe->language = 0;
	
		pMe->m_pMedia = NULL;
		
		if (NULL == pMe)
		{
			return EFAILED;
		}
		pMe->m_bChangeL =FALSE;
			 
		pMe->m_index = 0;
		
		Main_loadover = FALSE;
	
		// 初始化菜单Title
		pMe->m_IconTitle[0] 	= IDS_MAIN_MENU_MESSAGES;//IDS_MAIN_MENU_RECENTCALLS;	
		pMe->m_IconTitle[1] 	= IDS_MAIN_MENU_CONTACTS;//IDS_MAIN_MENU_USERPROFILE;		   
		pMe->m_IconTitle[2] 	= IDS_MAIN_MENU_RECENTCALLS;//IDS_MAIN_MENU_MULTIMEDIA;
		pMe->m_IconTitle[3] 	= IDS_MAIN_MENU_SETTINGS;//IDS_MAIN_MENU_CONTACTS;	   
		pMe->m_IconTitle[4] 	= IDS_MAIN_MENU_MULTIMEDIA;//IDS_MAIN_MENU_SETTINGS;	
	
#if 0   //FEATURE_CTA
		pMe->m_IconTitle[5] 	= IDS_MAIN_MENU_TOOLS;		 
		pMe->m_IconTitle[6] 	= IDS_MAIN_MENU_MESSAGES;
#else    
		pMe->m_IconTitle[5] 	= IDS_MAIN_MENU_TOOLS;//IDS_MAIN_MENU_UTK;	   
		pMe->m_IconTitle[6] 	= IDS_MAIN_MENU_USERPROFILE;//IDS_MAIN_MENU_TOOLS;
		pMe->m_IconTitle[7] 	= IDS_MAIN_MENU_UTK;//IDS_MAIN_MENU_MESSAGES;	 
#endif    
		
		
		//pMe->m_pImageBg = ISHELL_LoadImage(pMe->m_pShell, ICON_ANI_BG);
		
		// 接口创建及相关初始化
		if (ISHELL_CreateInstance(pMe->m_pShell, AEECLSID_CONFIG,
				(void **)&pMe->m_pConfig) != SUCCESS)
		{		
			return EFAILED;
		}
	
		if( ISHELL_CreateInstance( pMe->m_pShell, AEECLSID_BACKLIGHT, (void **)&pMe->m_pBacklight)!=AEE_SUCCESS)
		{
			return FALSE;
		}
		 
		if (ISHELL_CreateInstance(pMe->m_pShell, AEECLSID_DISPLAY, 
				(void **) &pMe->m_pDisplay) != SUCCESS)
		{		
			return EFAILED;
		}
		if (AEE_SUCCESS != ISHELL_CreateInstance(pMe->m_pShell,
												AEECLSID_ANNUNCIATOR,
												(void **)&pMe->m_pIAnn))
		{		
			return EFAILED;
		}
	  
		(void) ICONFIG_GetItem(pMe->m_pConfig,
										   CFGI_LANGUAGE_SELECTION,
										   &pMe->language,
										   sizeof(pMe->language));
	
		/*
		if(pMe->language == NV_LANGUAGE_ENGLISH)
		{
			for (i=0; i<MAX_MATRIX_ITEMS; i++)
			{
				pMe->m_pImageSelectEC99[i] = ISHELL_LoadImage(pMe->m_pShell, ICON_ANI[i]);				  
			}
			pMe->m_bChangeL =TRUE;
		}
		else
		{
			for (i=0; i<MAX_MATRIX_ITEMS; i++)
			{
				pMe->m_pImageSelectEC99[i] = ISHELL_LoadImage(pMe->m_pShell, ICON_ANI[i]);	  
			}
			pMe->m_bChangeL =FALSE;
		}	
		*/
		
		MSG_FATAL("CMainMenu_InitAppData.............over.....",0,0,0);
		return SUCCESS;
	}
	
	/*==============================================================================
	函数:
		CMainMenu_FreeAppData
	
	说明:
		释放 MainMenu Applet 相关资源。
	
	参数:
		pMe [in]: 指向MainMenu Applet 对象结构的指针。该结构包含小程序的特定信息。
	
	返回值:
		none
	
	备注:
	
	==============================================================================*/
	static void CMainMenu_FreeAppData(MainMenu *pMe)
	{
		if (NULL == pMe)
		{
			return;
		}
	
		pMe->m_eAppStatus = MAINMENU_STOP;
		if (NULL != pMe->m_pConfig)
		{
			ICONFIG_Release(pMe->m_pConfig);
			pMe->m_pConfig = NULL;
		}	 
		
		if (pMe->m_pDisplay != NULL)
		{
			(void) IDISPLAY_Release(pMe->m_pDisplay);
			pMe->m_pDisplay = NULL;
		}
		if (pMe->m_pIAnn)
		{
			IANNUNCIATOR_Release(pMe->m_pIAnn);
			pMe->m_pIAnn = NULL;
		}
	
		if(pMe->m_pBacklight)
		{
			IBACKLIGHT_Release(pMe->m_pBacklight);
			pMe->m_pBacklight=NULL;
		}
		if(pMe->m_pMedia)
		{
			IMEDIA_Release(pMe->m_pMedia);
			pMe->m_pMedia = NULL;
		}
		//释放图片资源
		Main_loadover = FALSE;
		{
			 int i;
			 int j;
			 if (pMe->m_pImageBg !=NULL)
			 {
				 (void) IIMAGE_Release(pMe->m_pImageBg);
				 pMe->m_pImageBg = NULL;
			 }
	 
			 for(i=0;i<MAX_MATRIX_ITEMS;i++)
			 {
				 if(pMe->m_pImageIcon[i] != NULL)
				 {
					 (void)IIMAGE_Release(pMe->m_pImageIcon[i]);
					 pMe->m_pImageIcon[i] = NULL;
				 }
			 } 
	
			/*
			for(j=0;j<MAX_MATRIX_ITEMS;j++)
			 {
				 if(pMe->m_pImageSelectEC99[j] != NULL)
				 {	 
					 (void)IIMAGE_Release(pMe->m_pImageSelectEC99[j]);
					 pMe->m_pImageSelectEC99[j] = NULL;
				 }
			 } 
			*/
			if(pMe->m_pImageSelectK212ND != NULL)
				 {	 
					 (void)IIMAGE_Release(pMe->m_pImageSelectK212ND);
					 pMe->m_pImageSelectK212ND= NULL;
				 }
		}  
	}
	
	/*=============================================================================
	FUNCTION:  MainMenu_AddRef
	
	DESCRIPTION: 
	
	PARAMETERS:
	
	RETURN VALUE:
	
	COMMENTS:
	
	SIDE EFFECTS:
	
	SEE ALSO:
	
	=============================================================================*/
	static uint32  MainMenu_AddRef( IMainMenu *pi)
	{
		return ( ++gMainMenu.referenceCounter);
	}
	
	/*=============================================================================
	FUNCTION:  MainMenu_Release
	
	DESCRIPTION: 
	
	PARAMETERS:
	
	RETURN VALUE:
	
	COMMENTS:
	
	SIDE EFFECTS:
	
	SEE ALSO:
	
	=============================================================================*/
	static uint32  MainMenu_Release( IMainMenu *pi)
	{
		register MainMenu *pMe = (MainMenu*)pi;
		if (pMe->referenceCounter == 0)
		{
			return 0;
		}
		
		if( --pMe->referenceCounter)
		{
			return pMe->referenceCounter;
		}
	
	
		CMainMenu_FreeAppData(pMe);
	
		(void) ISHELL_Release(pMe->m_pShell);
		(void) IMODULE_Release(pMe->m_pModule);
		return 0;
	}
	
	/*==============================================================================
	函数:
		MainMenu_RunFSM
	
	说明:
		运行有限状态机引擎。
	
	参数:
		pMe [in]: 指向MainMenu Applet对象结构的指针。该结构包含小程序的特定信息。
		   
	返回值:
		无。
	
	备注:
	
	==============================================================================*/
	static void MainMenu_RunFSM(MainMenu *pMe)
	{
		NextFSMAction nextFSMAction = NFSMACTION_WAIT;
		for ( ; ; )
		{
			nextFSMAction = MainMenu_ProcessState(pMe);
	
			// 这里不需要复杂的状态机，因此为简化状态机设计这里仅用一个布尔变量来
			// 指示dlgResult是否应该被改写。在某些特殊场合（见状态机设计），这种作
			// 法，避免了不必要的复杂性，简化了设计。
			if (pMe->m_bDoNotOverwriteDlgResult)
			{
				// We allow only one iteration without touching this variable. So,
				// let's reset it back to indicate that we are going to overwrite the
				// result for all subsequent iterations.
				pMe->m_bDoNotOverwriteDlgResult = FALSE;
			}
			else
			{
				// Overwrite the dlgResult to its default value. The next dialog that
				// we are going to display will set and return the dlgResult based on
				// the users action.
				pMe->m_eDlgReturn = DLGRET_CREATE;
			}
	
			// If the state machine processing engine indicates that it created a
			// dialog and requesting us to exit this loop, then we quit.
			if (nextFSMAction == NFSMACTION_WAIT)
			{
				break;
			}
		}
	}
	
	/*=============================================================================
	FUNCTION:  MainMenu_HandleEvent
	
	DESCRIPTION: 
	
	PARAMETERS:
	
	RETURN VALUE:
	
	COMMENTS:
	
	SIDE EFFECTS:
	
	SEE ALSO:
	
	=============================================================================*/
	static boolean MainMenu_HandleEvent( IMainMenu *pi,
		AEEEvent eCode,
		uint16	wParam,
		uint32 dwParam
	)
	{
		MainMenu *pMe = (MainMenu*)pi;
		AEEAppStart* as = NULL;
		switch ( eCode)
		{
			case EVT_APP_START:
#ifdef FEATURE_RANDOM_MENU_REND
				DisplayRend_Enable(TRUE);
#endif
				// 此事件dwParam为指针，不应为0
				if (dwParam == 0) 
				{
					return FALSE;
				}			 
				as = (AEEAppStart*)dwParam;
	
				pMe->m_index = 0;
	   
				pMe->m_rc = as->rc;
	
				pMe->m_currState = MAINST_MAIN;
				pMe->m_eDlgReturn = DLGRET_CREATE;
				pMe->m_eAppStatus = MAINMENU_RUNNING;
				IANNUNCIATOR_SetFieldIsActiveEx(pMe->m_pIAnn,TRUE); 
				MainMenu_RunFSM(pMe);
				return TRUE;
	
			case EVT_APP_STOP:
				{
					int theFocus = 0;
					SetCursor( pMe, theFocus);
					(void)ISHELL_CancelTimer( pMe->m_pShell, NULL, pMe);
					pMe->m_eAppStatus = MAINMENU_STOP;
#ifdef FEATURE_RANDOM_MENU_REND
					DisplayRend_Enable(FALSE);
#endif
					return TRUE;
				}
	
			case EVT_APP_SUSPEND:
				(void)ISHELL_CancelTimer(pMe->m_pShell, NULL, pMe);
				pMe->m_eAppStatus = MAINMENU_SUSPEND;
#ifdef FEATURE_RANDOM_MENU_REND
				DisplayRend_Push();
				DisplayRend_Start(FALSE);
#endif
				return TRUE;
			
	
			case EVT_APP_RESUME:
				{
					AEEAppStart* as = ( AEEAppStart*)dwParam;
					pMe->m_rc	 = as->rc;
					pMe->m_eAppStatus = MAINMENU_RUNNING;
#ifdef FEATURE_RANDOM_MENU_REND
					DisplayRend_Push();
					DisplayRend_Start(TRUE);
#endif
					// 开始跑WMS状态机
					MainMenu_RunFSM(pMe); 
					return TRUE;
				}
	
			case EVT_DIALOG_INIT:
				pMe->m_pActiveIDlg = (IDialog*)dwParam;
				pMe->m_pActivedlgID = wParam;
				return MainMenu_RouteDialogEvt(pMe,eCode,wParam,dwParam);
	
			case EVT_DIALOG_START:
				return MainMenu_RouteDialogEvt(pMe,eCode,wParam,dwParam);
	
			case EVT_USER_REDRAW:
				return MainMenu_RouteDialogEvt(pMe,eCode,wParam,dwParam);
	
			case EVT_DIALOG_END:
#ifdef FEATURE_VERSION_K212_ND
				if (pMe->m_pImageSelectK212ND!= NULL)
				{	
					 (void)IIMAGE_Release(pMe->m_pImageSelectK212ND);
					 pMe->m_pImageSelectK212ND= NULL;
				}
#endif	
				(void) MainMenu_RouteDialogEvt(pMe,eCode,wParam,dwParam);
				pMe->m_pActiveIDlg = NULL;
				pMe->m_pActivedlgID = 0;
				if(pMe->m_eAppStatus == MAINMENU_RUNNING)
				{
					MainMenu_RunFSM(pMe);
				}
				return TRUE;
			case EVT_KEY:
#if MIN_BREW_VERSION(3,0)
				// do not want to handle au
				if (((dwParam & KB_AUTOREPEAT) != 0) &&
					(wParam != AVK_UP) &&
					(wParam != AVK_DOWN) &&
					(wParam != AVK_LEFT) &&
					(wParam != AVK_RIGHT))
				{
					return TRUE;
				}
#endif
				return MainMenu_RouteDialogEvt(pMe,eCode,wParam,dwParam);  
#if defined (FEATURE_VERSION_N021)||defined(FEATURE_VERSION_W0216A)
			case EVT_KEY_HELD:
				{
					if(wParam == AVK_INFO)
					{
						(void) ISHELL_PostEvent(pMe->m_pShell,
											AEECLSID_CORE_APP,
											EVT_USER,
											1,
											0);
						ISHELL_CloseApplet(pMe->m_pShell,TRUE);
					}
				}
				break;
#endif
			case EVT_KEY_PRESS:
			case EVT_KEY_RELEASE:
			case EVT_COMMAND:
				return MainMenu_RouteDialogEvt(pMe,eCode,wParam,dwParam); 
	
			default:
				return MainMenu_RouteDialogEvt(pMe,eCode,wParam,dwParam);
		}
	}
	
	void Mainmenu_KeypadLock(boolean block)
	{
	   gbMainmenuLock = block;
	}
	
	/*==============================================================================
	函数:
		MainMenu_ProcessState
	
	说明:
		根据 pMe->m_currState, 调用相应的状态处理函数。
	
	参数:
		pMe [in]: 指向MainMenu Applet对象结构的指针。该结构包含小程序的特定信息。
	
	返回值:
		NFSMACTION_CONTINUE: 指示不停状态机。
		NFSMACTION_WAIT: 指示停止状态机。
	
	备注:
	
	==============================================================================*/
	NextFSMAction MainMenu_ProcessState(MainMenu *pMe)
	{
		NextFSMAction retVal = NFSMACTION_WAIT;
		if (NULL == pMe)
		{
			return retVal;
		}
		switch(pMe->m_currState)
		{
			case MAINST_MAIN:
				return MAINST_MAIN_Handler(pMe);
			case MAINST_FLASHLIGHT:
				return MAINST_FLASHLIGHT_Handler(pMe);	  
			case MAINST_EXIT:
				return MAINST_EXIT_Handler(pMe);  
			default:
				break;
	
		}
		return retVal;
	}
	
	/*==============================================================================
	函数:
		MAINST_MAIN_Handler
	
	说明:
		MAINST_MAIN 状态处理函数。
	
	参数:
		pMe [in]: 指向MAINMENU Applet对象结构的指针。该结构包含小程序的特定信息。
	
	返回值:
		NFSMACTION_CONTINUE: 指示不停状态机。
		NFSMACTION_WAIT: 指示停止状态机。
	
	备注:
	
	==============================================================================*/
	static NextFSMAction MAINST_MAIN_Handler(MainMenu *pMe)
	{
		if (NULL == pMe)
		{
			return NFSMACTION_WAIT;
		}
		switch (pMe->m_eDlgReturn)
		{
			// 进入主界面
			case DLGRET_CREATE:
				{
					MainMenu_ShowDialog(pMe, IDD_MAIN_MENU);
				}
				return NFSMACTION_WAIT;
				
			case DLGRET_FLASHLITHT:
				{
					MOVE_TO_STATE(MAINST_FLASHLIGHT)
					return NFSMACTION_CONTINUE;
				}	 
			case DLGRET_CANCELED:
				MOVE_TO_STATE(MAINST_EXIT)
				return NFSMACTION_CONTINUE;
	
			default:
				MOVE_TO_STATE(MAINST_EXIT)
				return NFSMACTION_CONTINUE;
		}
	}
	
	
	static NextFSMAction MAINST_FLASHLIGHT_Handler(MainMenu *pMe)
	{
		if (NULL == pMe)
		{
			return NFSMACTION_WAIT;
		}
		switch (pMe->m_eDlgReturn)
		{
			// 进入主界面
			case DLGRET_CREATE:
				{
					MainMenu_ShowDialog(pMe, IDD_FLASHLIGHT_SETTING);
				}
				return NFSMACTION_WAIT;
			 case DLGRET_CANCELED:		   
				MOVE_TO_STATE(MAINST_MAIN)
				return NFSMACTION_CONTINUE;
			default:
				MOVE_TO_STATE(MAINST_EXIT)
				return NFSMACTION_CONTINUE;
		}
	}
	
	
	/*==============================================================================
	函数:
		MAINST_EXIT_Handler
	
	说明:
		MAINST_EXIT 状态处理函数。
	
	参数:
		pMe [in]: 指向MAINMENU	Applet对象结构的指针。该结构包含小程序的特定信息。
	
	返回值:
		NFSMACTION_CONTINUE: 指示不停状态机。
		NFSMACTION_WAIT: 指示停止状态机。
	
	备注:
	
	==============================================================================*/
	static NextFSMAction MAINST_EXIT_Handler(MainMenu *pMe)
	{
		(void) ISHELL_CloseApplet(pMe->m_pShell, FALSE);
		return NFSMACTION_WAIT;
	} 
	
	/*==============================================================================
	函数:
		   MainMenu_ShowDialog
	
	说明:
		函数由状态处理函数用来唤起相应对话框资源ID的对话框。
	
	参数:
		pMe [in]: 指向 MAINMENU Applet 对象结构的指针。该结构包含小程序的特定信息。
		dlgResId [in]: 对话框资源ID，函数根据该ID创建对话框。
	
	返回值:
		none
	
	备注:
	
	==============================================================================*/
	void MainMenu_ShowDialog(MainMenu  *pMe,  uint16 dlgResId)
	{
		int nRet;
	
		// 每次最多打开一个对话框
		if (ISHELL_GetActiveDialog(pMe->m_pShell) != NULL)
		{
			// 如果当前已经有对话框被打开，直接返回
			return;
		}
	   
		if (NULL != pMe->m_pDisplay)
		{
			AEEDeviceInfo di={0,};
#if defined (FEATURE_VERSION_H19C)
			if(pMe->m_pIAnn != NULL)
			{
				if (dlgResId == IDD_MAIN_MENU)
				{
					IANNUNCIATOR_SetHasTitleText(pMe->m_pIAnn, FALSE);
				}
				else
				{
					IANNUNCIATOR_SetHasTitleText(pMe->m_pIAnn, TRUE);
				}
				IANNUNCIATOR_Redraw(pMe->m_pIAnn);
			}
			(void)IDISPLAY_SetPrefs(pMe->m_pDisplay, "a:1", STRLEN("a:1"));
#else
			if (dlgResId == IDD_MAIN_MENU)
			{
				(void)IDISPLAY_SetPrefs(pMe->m_pDisplay, "a:0", STRLEN("a:0"));
			}
			else
			{
				(void)IDISPLAY_SetPrefs(pMe->m_pDisplay, "a:1", STRLEN("a:1"));
			}
#endif
			ISHELL_GetDeviceInfo(pMe->m_pShell, &di);
			pMe->m_rc.dx = di.cxScreen;
			pMe->m_rc.dy = di.cyScreen; 	  
			IDISPLAY_SetClipRect(pMe->m_pDisplay, &pMe->m_rc);		  
		}
		
		nRet = ISHELL_CreateDialog(pMe->m_pShell,MAINMENU_RES_FILE_LANG,dlgResId,NULL);
		if (nRet != SUCCESS)
		{
		}
	}
	
	
	/*==============================================================================
	函数:
		   MainMenu_RouteDialogEvt
	说明:
		   函数将BREW事件路由给当前活动对话框的事件处理函数。
	
	参数:
		   pMe [In]: 指向SettingMenu Applet对象结构的指针,该结构包含小程序的特定信息.
		   eCode [in]：事件代码。
		   wParam [in]：与事件eCode关联的数据。
		   dwParam [in]：与事件eCode关联的数据。
	
	返回值:
		   TRUE：传入事件被处理。
		   FALSE：传入事件没被处理。
	
	备注:
	
	==============================================================================*/
	boolean MainMenu_RouteDialogEvt(MainMenu *pMe,
		AEEEvent	eCode,
		uint16	wParam,
		uint32 dwParam
	)
	
	{
		if (NULL == pMe)
		{
			return FALSE;
		}
	
		if (NULL == pMe->m_pActiveIDlg)
		{
			return FALSE;
		}	 
		switch( pMe->m_pActivedlgID)
		{
			case IDD_MAIN_MENU:
				return MainMenu_IconMenuHandler(pMe, eCode, wParam, dwParam);
	
			case IDD_FLASHLIGHT_SETTING:
				return MainMenu_FlashlightMenuHandler(pMe, eCode, wParam,dwParam);
			default:
				return FALSE;
		}
	}
	
	/*=============================================================================
	FUNCTION:  MainMenu_IconMenuHandler
	
	DESCRIPTION: 12宫格菜单
	
	PARAMETERS:
	
	=============================================================================*/
	static boolean MainMenu_IconMenuHandler(MainMenu *pMe, AEEEvent eCode, uint16 wParam, uint32 dwParam)
	{
		PARAM_NOT_REF( dwParam)
		switch ( eCode)
		{
			case EVT_DIALOG_INIT:			
				IDIALOG_SetProperties((IDialog *)dwParam, DLG_NOT_REDRAW_AFTER_START);
				MSG_FATAL("MainMenu_IconMenuHandler.........init....",0,0,0);
				return TRUE;
	
			case EVT_DIALOG_START:			
				AEE_CancelTimer(Main_keypadtimer,pMe);
				AEE_SetTimer(5*1000,Main_keypadtimer,pMe);
				(void) ISHELL_PostEvent( pMe->m_pShell,
										 AEECLSID_MAIN_MENU,
										 EVT_USER_REDRAW,
										 0,
										 0);
				MSG_FATAL("MainMenu_IconMenuHandler.........EVT_DIALOG_START....",0,0,0);
				return TRUE;
	
			case EVT_USER_REDRAW:
				// 初始整个背景及全部初始图标
				{
					
					MSG_FATAL("MainMenu_IconMenuHandler.........EVT_USER_REDRAW....",0,0,0);
					DrawMatrix(pMe);
					MSG_FATAL("MainMenu_IconMenuHandler.........EVT_USER_REDRAW..over..",0,0,0);
					//AEE_CancelTimer(Main_loadtimer,pMe);
					//AEE_SetTimer(100,Main_loadtimer,pMe);
					return TRUE;
				}
				
			case EVT_DIALOG_END:
				{
					ISHELL_CancelTimer(pMe->m_pShell, NULL, (void**)pMe);
				}
				return TRUE;
	
			case EVT_KEY_HELD:			  
        #if defined(FEATURE_TORCH_KEY_INFO)
			   if((AVKType)wParam == AVK_INFO)
				{
					boolean TorchOn = FALSE;
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
					ISHELL_CloseApplet(pMe->m_pShell, TRUE); 
					return TRUE;
				}
        #endif
	 #if defined(FEATURE_TORCH_KEY_SELECT) //xxzhen
			   if((AVKType)wParam == AVK_SELECT)
				{
					boolean TorchOn = FALSE;
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
					ISHELL_CloseApplet(pMe->m_pShell, TRUE); 
					return TRUE;
				}
        #endif
	
				return TRUE;
			case EVT_KEY:
				switch( wParam)
				{
					case AVK_CLR:
						 CLOSE_DIALOG(DLGRET_CANCELED)
						return TRUE;
						 
					case AVK_UP:	   
					case AVK_LEFT:
					   if(pMe->m_index>0)
					   {
						   pMe->m_index = pMe->m_index-1;
					   }
					   else if(pMe->m_index == 0)
					   {
							pMe->m_index = (MAX_MATRIX_ITEMS-1);
					   }
					   MoveCursorTo(pMe, pMe->m_index);
					   return TRUE;
	
					case AVK_DOWN:	   
					case AVK_RIGHT:
	
					   if(pMe->m_index<(MAX_MATRIX_ITEMS-1))
					   {
						   pMe->m_index = pMe->m_index+1;
						  
					   }
					   else if(pMe->m_index == (MAX_MATRIX_ITEMS-1))
					   {
							pMe->m_index = 0;
					   }
					   MoveCursorTo(pMe, pMe->m_index);
					   return TRUE;
						
					case AVK_SELECT:
					case AVK_INFO:
						{
						   StartApplet(pMe, pMe->m_IconTitle[pMe->m_index]);
						}
						return TRUE;
					case AVK_STAR:
						if(gbMainmenuLock)
						{
							OEMKeyguard_SetState(TRUE);
							ISHELL_CloseApplet(pMe->m_pShell, TRUE); 
						}
						return TRUE;
					default:
						break;
				}
	
				return TRUE;
				
			default:
				break;
		}
	
		return FALSE;
	}
	
	//Add By zzg 2013_08_20
	static boolean MainMenu_FlashlightMenuHandler(MainMenu *pMe, AEEEvent eCode, uint16 wParam, uint32 dwParam)
	{
		PARAM_NOT_REF(dwParam)
		IMenuCtl *pMenu = (IMenuCtl*)IDIALOG_GetControl(pMe->m_pActiveIDlg,IDC_FLASHLIGHT_SET);
		AECHAR WTitle[40] = {0};
		if (pMenu == NULL)
		{
			return FALSE;
		}
		if(pMe->m_pIAnn != NULL)
		{
			IANNUNCIATOR_SetFieldIsActiveEx(pMe->m_pIAnn,FALSE);
		}
		 switch (eCode)
		{
			case EVT_DIALOG_INIT:
				(void)ISHELL_LoadResString(pMe->m_pShell,
											MAINMENU_RES_FILE_LANG, 							   
											IDS_MAIN_MENU_TORCH,
											WTitle,
											sizeof(WTitle));
	
				if(pMe->m_pIAnn != NULL)
				{
					IANNUNCIATOR_SetFieldTextEx(pMe->m_pIAnn,WTitle,FALSE);
				}
				IMENUCTL_AddItem(pMenu, MAINMENU_RES_FILE_LANG, IDS_MAIN_MENU_TORCH_ON, IDS_MAIN_MENU_TORCH_ON, NULL, 0);
				IMENUCTL_AddItem(pMenu, MAINMENU_RES_FILE_LANG, IDS_MAIN_MENU_TORCH_OFF, IDS_MAIN_MENU_TORCH_OFF, NULL, 0);
				return TRUE;
				
			case EVT_DIALOG_START:
				{
					uint16 wItemID;
					boolean Is_on = FALSE;
					IMENUCTL_SetProperties(pMenu, MP_UNDERLINE_TITLE|MP_WRAPSCROLL|MP_TEXT_ALIGN_LEFT_ICON_ALIGN_RIGHT);
					IMENUCTL_SetOemProperties(pMenu, OEMMP_USE_MENU_STYLE);
	
					IMENUCTL_SetBottomBarType(pMenu,BTBAR_SELECT_BACK);
					OEM_GetConfig(CFGI_FLSHLITHG_STATUS,&Is_on, sizeof(Is_on));
					if(Is_on)
					{
						wItemID = IDS_MAIN_MENU_TORCH_ON;
					}
					else
					{
						wItemID = IDS_MAIN_MENU_TORCH_OFF;
					}
	
					InitMenuIcons(pMenu);
					SetMenuIcon(pMenu, wItemID, TRUE);
					IMENUCTL_SetSel(pMenu, wItemID);
				}
				return TRUE;
	
			case EVT_DIALOG_END:
				return TRUE;
	
			case EVT_KEY:
				switch(wParam)
				{
					case AVK_CLR:
						CLOSE_DIALOG(DLGRET_CANCELED)
						return TRUE;
	
					  default:
						break;
				}
				return TRUE;
#ifdef FEATURE_LCD_TOUCH_ENABLE//andrew add for LCD touch
			case EVT_PEN_UP:
				{
					AEEDeviceInfo devinfo;
					int nBarH ;
					AEERect rc;
					int16 wXPos = (int16)AEE_GET_X(dwParam);
					int16 wYPos = (int16)AEE_GET_Y(dwParam);
	
					nBarH = GetBottomBarHeight(pMe->m_pDisplay);
			
					MEMSET(&devinfo, 0, sizeof(devinfo));
					ISHELL_GetDeviceInfo(pMe->m_pShell, &devinfo);
					SETAEERECT(&rc, 0, devinfo.cyScreen-nBarH, devinfo.cxScreen, nBarH);
	
					if(TOUCH_PT_IN_RECT(wXPos,wYPos,rc))
					{
						if(wXPos >= rc.x && wXPos < rc.x + (rc.dx/3) )//左
						{
							boolean rt =  ISHELL_PostEvent(pMe->m_pShell,AEECLSID_APPLICATION,EVT_KEY,AVK_SELECT,0);
							return rt;
						}
						else if(wXPos >= rc.x + (rc.dx/3)	&& wXPos < rc.x + (rc.dx/3)*2 )//左
						{
							 boolean rt = ISHELL_PostEvent(pMe->m_pShell,AEECLSID_APPLICATION,EVT_KEY,AVK_INFO,0);
							 return rt;
						}
						else if(wXPos >= rc.x + (rc.dx/3)*2 && wXPos < rc.x + (rc.dx/3)*3 )//左
						{						
							 boolean rt = ISHELL_PostEvent(pMe->m_pShell,AEECLSID_APPLICATION,EVT_KEY,AVK_CLR,0);
							 return rt;
						}
					}
	
				}
				break;
#endif 
			case EVT_COMMAND:
				{
					boolean bytNewData = 0;
	
					switch (wParam)
					{
						case  IDS_MAIN_MENU_TORCH_ON:
						   bytNewData = TRUE;
						   IBACKLIGHT_TurnOnTorch(pMe->m_pBacklight);
						   break;
	
						case IDS_MAIN_MENU_TORCH_OFF:
						   bytNewData = FALSE;
						   IBACKLIGHT_TurnOffTorch(pMe->m_pBacklight);
						   break;
	
						default:
						   break;
	
					}
					OEM_SetConfig(CFGI_FLSHLITHG_STATUS,&bytNewData, sizeof(bytNewData));
					InitMenuIcons(pMenu);
					SetMenuIcon(pMenu, wParam, TRUE);
					(void) ISHELL_PostEvent( pMe->m_pShell,
											 AEECLSID_MAIN_MENU,
											 EVT_USER_REDRAW,
											 0,
											 0);
				}
			default:
				break;
		}
	}
	//Add End
	
	//Add End
	
	
	static void Main_keypadtimer(void *pUser)
	{
		gbMainmenuLock =FALSE;
	}
	static void Main_loadtimer(void *pUser)
	{
		MainMenu	*pMe = (MainMenu *)pUser;
		int i = 0;
		int j = 0;
		for (i = 0; i < MAX_MATRIX_ITEMS; i ++)
		{
			if (pMe->m_pImageIcon[i] == NULL)
			{
				pMe->m_pImageIcon[i] = ISHELL_LoadImage(pMe->m_pShell,
														ICON_ANI[i]);
			}
		}  
		/*
		for(j=0;j<MAX_MATRIX_ITEMS;j++)
		{
			if (pMe->m_pImageSelectEC99[j] == NULL)
			{			
				pMe->m_pImageSelectEC99[j] = ISHELL_LoadImage(pMe->m_pShell, ICON_ANI[j]);
			}
		}
		*/
		Main_loadover = TRUE;
		
	}
	
	static void MainMenu_DrawBackGround(MainMenu *pMe, AEERect *pRect)
	{
	
		Appscommon_ResetBackground(pMe->m_pDisplay, 
								  pMe->m_pImageBg, 
								  APPSCOMMON_BG_COLOR, 
								  pRect, 
								  0, 
								  0);
	
	}
	
	/*=============================================================================
	FUNCTION:  DrawMatrix
	
	DESCRIPTION: // 初始整个背景及全部初始图标
	
	=============================================================================*/
	static void DrawMatrix(MainMenu *pMe)
	{
		
		BottomBar_e_Type	eBBarType = BTBAR_NONE;
		boolean m_sound_bo_main = FALSE;
		if (NULL == pMe)
		{
			return;
		}
		//draw bg image
		//MainMenu_DrawBackGround(pMe, &pMe->m_rc);
		
		(void) ICONFIG_GetItem(pMe->m_pConfig,
										   CFGI_LANGUAGE_SELECTION,
										   &pMe->language,
										   sizeof(pMe->language));
		   
		if (pMe->m_pImageSelectK212ND!= NULL)
		{	
			 (void)IIMAGE_Release(pMe->m_pImageSelectK212ND);
			 pMe->m_pImageSelectK212ND= NULL;
		}
		
		pMe->m_pImageSelectK212ND= ISHELL_LoadImage(pMe->m_pShell, ICON_ANI[pMe->m_index]);		
		if(pMe->m_pImageSelectK212ND!= NULL)
		{
			IIMAGE_Draw(pMe->m_pImageSelectK212ND, 0, 0);
		}
		
#ifdef FEATURE_SOUND_BO		
		(void) ICONFIG_GetItem(pMe->m_pConfig,
									 CFGI_SOUND_BO_MAIN,
									 &m_sound_bo_main,
									 sizeof(boolean));
	
		if(m_sound_bo_main)
		{
			/*
			nv_item_type	SimChoice;
			(void)OEMNV_Get(NV_SIM_SELECT_I,&SimChoice);
			if(SimChoice.sim_select != 1)
			*/	  
			if (GetMp3PlayerStatus() == MP3STATUS_NONE)
			{
				MainMenu_PlayShutterSound(pMe,pMe->m_index);
			}
		}
#endif
		IDISPLAY_UpdateEx(pMe->m_pDisplay, TRUE);	
	}
	
	
	/*=============================================================================
	FUNCTION:  MoveCursorTo
	
	DESCRIPTION: 
	
	PARAMETERS:
	
	RETURN VALUE:
	
	COMMENTS:
	
	SIDE EFFECTS:
	
	SEE ALSO:
	
	=============================================================================*/
	static void MoveCursorTo(MainMenu *pMe, int index)
	{
		
		boolean m_sound_bo_main = FALSE;
		//draw bg image
		if (NULL == pMe)
		{
			return;
		}
		(void) ICONFIG_GetItem(pMe->m_pConfig,
										   CFGI_LANGUAGE_SELECTION,
										   &pMe->language,
										   sizeof(pMe->language));
	
#ifdef FEATURE_SOUND_BO    
		(void) ICONFIG_GetItem(pMe->m_pConfig,
									 CFGI_SOUND_BO_MAIN,
									 &m_sound_bo_main,
									 sizeof(boolean));
		if(m_sound_bo_main)
		{
			/*
			nv_item_type	SimChoice;
			(void)OEMNV_Get(NV_SIM_SELECT_I,&SimChoice);		
			if(SimChoice.sim_select != 1)
			*/	  
			if (GetMp3PlayerStatus() == MP3STATUS_NONE)
			{
				MainMenu_PlayShutterSound(pMe,pMe->m_index);
			}
		}
#endif
		MainMenu_DrawBackGround(pMe, &pMe->m_rc);
	
	
		if (pMe->m_pImageSelectK212ND!= NULL)
		{	
			 (void)IIMAGE_Release(pMe->m_pImageSelectK212ND);
			 pMe->m_pImageSelectK212ND= NULL;
		}
	
		pMe->m_pImageSelectK212ND= ISHELL_LoadImage(pMe->m_pShell, ICON_ANI[pMe->m_index]);		
		if(pMe->m_pImageSelectK212ND!= NULL)
		{
			IIMAGE_Draw(pMe->m_pImageSelectK212ND, 0, 0);
		}
		
		IDISPLAY_UpdateEx(pMe->m_pDisplay, TRUE);
	}
	/*=============================================================================
	FUNCTION:  SetCursor
	
	DESCRIPTION: 
	
	PARAMETERS:
	
	RETURN VALUE:
	
	COMMENTS:
	
	SIDE EFFECTS:
	
	SEE ALSO:
	
	=============================================================================*/
	static void SetCursor(MainMenu *pMe, int index)
	{
		pMe->m_index		= index;   
		
		if(pMe->m_pAnimate != NULL)
		{
			IImage_Release(pMe->m_pAnimate);
			pMe->m_pAnimate  = NULL;
		}
	}
	
	/*=============================================================================
	FUNCTION:  StartApplet
	
	DESCRIPTION:  启动applet
	
	PARAMETERS:  如果APPLET 有变动，只需改动次函数
	
	=============================================================================*/
	static int StartApplet(MainMenu *pMe, int i)
	{
		int Result = EUNSUPPORTED;
		//uint32  dwTotal = 0;
		//uint32 free = 0;
		//GETFSFREE(&dwTotal);
		//free = GETRAMFREE(NULL,NULL);
		//MSG_FATAL("dwTotal======%d,free===%d",dwTotal,free,0);
	
		
#ifdef FEATURE_VERSION_H19C  
		if(pMe->m_pIAnn != NULL)
		{
			IANNUNCIATOR_SetHasTitleText(pMe->m_pIAnn, TRUE);
		}
#endif
		#ifdef FEATURE_VERSION_K202_LM129C //xxzhen
			  if(pMe->m_pIAnn != NULL)
			  {
				  IANNUNCIATOR_SetHasTitleText(pMe->m_pIAnn, TRUE);
			  }
		#endif
	
		switch(i)
		{
#ifdef FEATURE_VERSION_W208S
		case IDS_MAIN_MENU_MAP:
		{
			char	buf[12];
			
			OEM_SetBROWSER_ADSAccount();	//Add By zzg 2011_07_08
			
			MSG_FATAL("AEECLSID_BRW_APP...........START",0,0,0);
			start_info.appid_fx = APP_ORIGINATOR_BROWSER;
			start_info.subtype_keycode = APP_BROWSER_START_URL;
			start_info.par_keychar = 0;
	
			start_info.fInfo.url_info.title = "map";
			start_info.fInfo.url_info.url = "http://wap.movidamovil.com/venezuela/?debug=true"; 	
			
			//start_info.fInfo.url_info.title = "163";
			//start_info.fInfo.url_info.url = "http://wap.163.com";
	
			
			SPRINTF(buf, "%p", &start_info);
			Result = ISHELL_StartAppletArgs(pMe->m_pShell,AEECLSID_NF3,buf);
			//Result = ISHELL_StartApplet(pMe->m_pShell, AEECLSID_BRW_APP);
			MSG_FATAL("AEECLSID_BRW_APP...........Result=%d",Result,0,0);		
			
			
			break;
		}
		case IDS_MAIN_MENU_NEO_APP:
		{
			Result = ISHELL_StartApplet(pMe->m_pShell, AEECLSID_APPMANAGER);
			break;
		}
		case IDS_MAIN_MENU_NEO_NAV:
		{
			char	buf[12];
			
			OEM_SetBROWSER_ADSAccount();	//Add By zzg 2011_07_08
			
			MSG_FATAL("AEECLSID_BRW_APP...........START",0,0,0);
			start_info.appid_fx = APP_ORIGINATOR_BROWSER;
			start_info.subtype_keycode = APP_BROWSER_START_MAINMENU;
			start_info.par_keychar = 0;
			//start_info.fInfo.url_info.title = "163";
			//start_info.fInfo.url_info.url = "http://www.163.com";
			SPRINTF(buf, "%p", &start_info);
			Result = ISHELL_StartAppletArgs(pMe->m_pShell,AEECLSID_NF3,buf);
			//Result = ISHELL_StartApplet(pMe->m_pShell, AEECLSID_BRW_APP);
			MSG_FATAL("AEECLSID_BRW_APP...........Result=%d",Result,0,0);		
			
			break;
		}
#endif
	
		case IDS_MAIN_MENU_FILEMGR:
		case IDS_MAIN_MENU_GALLERY:
		case IDS_MAIN_MENU_MEDIAGALLERY_C337:	
		case IDS_MAIN_MENU_MEDIAGALLERY:
			Result = ISHELL_StartApplet(pMe->m_pShell, AEECLSID_MEDIAGALLERY);
			break;
			
		case IDS_MAIN_MENU_CONTACT:
		case IDS_PHONEBOOK:
		case IDS_MAIN_MENU_CONTACTS:
			{
				IContactApp *ca = NULL;
				Result = ISHELL_CreateInstance(pMe->m_pShell,AEECLSID_APP_CONTACT, (void**)&ca);
				if(SUCCESS == Result)
				{
					ICONTAPP_MainMenu(ca);
					IContactApp_Release(ca);
				}
			}
			break;
		case IDS_MAIN_MENU_INTEXZONE: 
#ifdef FEATURE_VERSION_C316
			Result = ISHELL_StartApplet(pMe->m_pShell, AEECLSID_INDEX_ZONE);
			break;
#endif
		case IDS_MAIN_MENU_SMARTFRENACCESS:
		case IDS_MAIN_MENU_UTK:
			Result = ISHELL_StartApplet(pMe->m_pShell, AEECLSID_APP_UTK);
			break;
			
		case IDS_MAIN_MENU_TOOLS:
		case IDS_MAIN_MENU_ORGANIZER:	
		case IDS_MAIN_MENU_APPLICATION:
			Result = ISHELL_StartApplet(pMe->m_pShell, AEECLSID_APPLICATION);
			break;
			
		case IDS_MAIN_MENU_CALL_LOGS:
		case IDS_MAIN_MENU_RECENTCALLS_C337:	
		case IDS_MAIN_MENU_RECENTCALLS:
			Result = ISHELL_StartApplet(pMe->m_pShell, AEECLSID_APP_RECENTCALL);
			break;
			
		case IDS_MAIN_MENU_SMS:
		case IDS_MAIN_MENU_MESSAGES:
			Result = ISHELL_StartApplet(pMe->m_pShell, AEECLSID_WMSAPP);
			break;
			
		case IDS_MAIN_MENU_MULTIMEDIA:
			Result = ISHELL_StartApplet(pMe->m_pShell, AEECLSID_MULTIMEDIA_LIST);
			break;
			
		case IDS_MAIN_MENU_STATIC_APPLICATION:
#if defined(FEATURE_APP_MANAGER)
#ifdef FEATURE_OEMOMH
			if(ISHELL_SendEvent(pMe->m_pShell,AEECLSID_DIALER,EVT_OMH_PROMPT,0,0))
			{
				break;
			}
#endif
	
			Result = ISHELL_StartApplet(pMe->m_pShell, AEECLSID_APPMANAGER);
#else
			Result = ISHELL_StartApplet(pMe->m_pShell, AEECLSID_STATIC_APP);
#endif
		break;
			
		case IDS_MAIN_MENU_CALENDAR:
		case IDS_MAIN_MENU_SCHEDULER:
			Result = ISHELL_StartApplet(pMe->m_pShell, AEECLSID_SCHEDULEAPP);
			break;
			
		case IDS_MAIN_MENU_SETTINGS:
			Result = ISHELL_StartApplet(pMe->m_pShell, AEECLSID_APP_SETTINGMENU);
			break;
	
		//Add By zzg 2012_11_08 for C337 
		case IDS_MAIN_MENU_SERVICES:		
	    #if defined (FEATURE_VERSION_C337) || defined(FEATURE_VERSION_IC241A_MMX)
        #if defined (FEATURE_VERSION_IN50A)
        Result = SetBrowserArr_Main(pMe->m_pShell,(char*)"http://www.google.com.hk/"); 
        #else
        Result = SetBrowserArr_Main(pMe->m_pShell,(char*)"http://mimicromax.com"); //ISHELL_StartAppletArgs(pMe->m_pShell, AEECLSID_UCWEB, (char*)"call_ucweb:setmainpageurl:http://mimicromax.com");
        #endif			
        #else
			Result = SetBrowserArr_Main(pMe->m_pShell,NULL);//ISHELL_StartApplet(pMe->m_pShell, AEECLSID_UCWEB);
        #endif
			break;
		case IDS_MAIN_MENU_MSTORE:
#if defined (FEATURE_VERSION_C337) || defined(FEATURE_VERSION_IC241A_MMX)
#if defined (FEATURE_VERSION_IN50A)
            Result = SetBrowserArr_Main(pMe->m_pShell,(char*)"http://www.google.com.hk/"); 
#else
            Result = SetBrowserArr_Main(pMe->m_pShell,(char*)"http://mimicromax.com"); //ISHELL_StartAppletArgs(pMe->m_pShell, AEECLSID_UCWEB, (char*)"call_ucweb:setmainpageurl:http://mimicromax.com");
#endif			
#else
			Result = SetBrowserArr_Main(pMe->m_pShell,NULL);//ISHELL_StartAppletArgs(pMe->m_pShell, AEECLSID_UCWEB, (char*)"call_ucweb:setmainpageurl:http://mimicromax.com");
#endif
			break;
		case IDS_MAIN_MENU_MZONE:
			Result = ISHELL_StartApplet(pMe->m_pShell, AEECLSID_MiZone);
			break;
		//Add End
		
		case IDS_MAIN_MENU_USER_PROFILE:	
		case IDS_MAIN_MENU_USERPROFILE:
			Result = ISHELL_StartApplet(pMe->m_pShell, AEECLSID_APP_SOUNDMENU);
			break;
			
		case IDS_MAIN_MENU_GAMES:
			Result = ISHELL_StartApplet(pMe->m_pShell, AEECLSID_GAME);
			break;
			
	#ifdef FEATURE_VERSION_C316
		case IDS_MAIN_MENU_SERVICES_C316:  
			Result = ISHELL_StartApplet(pMe->m_pShell, AEECLSID_SERVICE);
			break;
	#endif	
		
		case IDS_MAIN_WAPBROWSER:
		#ifdef FEATURE_VERSION_K202_LM129C
			  IANNUNCIATOR_SetHasTitleText(pMe->m_pIAnn,FALSE); //xxzhen
			   IANNUNCIATOR_Redraw(pMe->m_pIAnn);
		#endif
	
        #if defined (FEATURE_VERSION_C337) || defined(FEATURE_VERSION_IC241A_MMX)
        #if defined (FEATURE_VERSION_IN50A)
        Result = SetBrowserArr_Main(pMe->m_pShell,(char*)"http://www.google.com.hk/"); 
        #else
        Result = SetBrowserArr_Main(pMe->m_pShell,(char*)"http://mimicromax.com"); //ISHELL_StartAppletArgs(pMe->m_pShell, AEECLSID_UCWEB, (char*)"call_ucweb:setmainpageurl:http://mimicromax.com");
        #endif			
        #else
			Result = SetBrowserArr_Main(pMe->m_pShell,(char*)""); //ISHELL_StartApplet(pMe->m_pShell, AEECLSID_UCWEB);
        #endif
			break;
		
		case IDS_MAIN_MENU_CALCULATOR: 
			 Result = ISHELL_StartApplet(pMe->m_pShell, AEECLSID_CALCAPP);
			break;
			
		case IDS_MAIN_MENU_FMRADIO:
			Result = ISHELL_StartApplet(pMe->m_pShell, AEECLSID_APP_FMRADIO);
			break;
			
		case IDS_MAIN_MENU_MUSICPLAYER:
			Result = ISHELL_StartApplet(pMe->m_pShell, AEECLSID_APP_MUSICPLAYER);
			break;
			
		case IDS_MAIN_MENU_SMARTWORLD:
		case IDS_MAIN_MENU_BLIVE:
#ifdef FEATURE_SMARTFREN_STATIC_BREW_APP
			Result = ISHELL_StartApplet(pMe->m_pShell, AEECLSID_STATIC_APP);
#else
#ifdef FEATURE_OEMOMH
			if(ISHELL_SendEvent(pMe->m_pShell,AEECLSID_DIALER,EVT_OMH_PROMPT,0,0))
			{
				break;
			}
#endif
			Result = ISHELL_StartApplet(pMe->m_pShell, AEECLSID_APPMANAGER);
#endif  
			break;
		
		case IDS_MAIN_MENU_FRENDUO:
#if defined(FEATURE_VERSION_M8) || defined(FEATURE_VERSION_M8P) || defined (FEATURE_VERSION_M8021)
			Result = ISHELL_StartApplet(pMe->m_pShell, AEECLSID_FRENDUO);
#endif
			break;
		
		case IDS_MAIN_MENU_TIMER:
			Result = ISHELL_StartApplet(pMe->m_pShell, AEECLSID_APPTIMER);
			break;
			
		case IDS_MAIN_MENU_CAMERA:
		{		
			Result = ISHELL_StartApplet(pMe->m_pShell, AEECLSID_APP_CAMERA);
			break;
		}  
		
		case IDS_MAIN_MENU_TORCH:
		{		
			//Result = ISHELL_StartApplet(pMe->m_pShell, AEECLSID_TORCH);
			CLOSE_DIALOG(DLGRET_FLASHLITHT)
			break;
		}	 
		
		default:
			break;
	
		}
		return Result;
	}
	
	extern char charsvc_p_name[UIM_CDMA_HOME_SERVICE_SIZE+1];
	int SetBrowserArr_Main(IShell *pShell ,char *purl)
	{
		int Result = EUNSUPPORTED;
		char urlCan[1024] = {0};
		
		DBGPRINTF("svc_p_name %s %d",charsvc_p_name,charsvc_p_name[0],0);
		OEM_SetUCBROWSER_ADSAccount();
		
		if(purl && STRLEN(purl)>1)
		{
			SPRINTF(urlCan,"call_ucweb:setexternurl:%s\2\3",purl);
			STRCAT(urlCan,"useragent:BREW-Applet/0x20068888 (BREW/3.1.5.20; DeviceId: 8976509865757e; Lang: hi; Profile/MIDP-2.0_Configuration/CLDC-1.1) ucweb-squid\2\3");
		}
		else
		{
			STRCPY(urlCan,"useragent:BREW-Applet/0x20068888 (BREW/3.1.5.20; DeviceId: 8976509865757e; Lang: hi; Profile/MIDP-2.0_Configuration/CLDC-1.1) ucweb-squid\2\3");
		}
		
		if(STRISTR (charsvc_p_name,"mts"))
		{
			MSG_FATAL("mst................",0,0,0);
			STRCAT(urlCan,"access_point:proxy_is:10.50.5.140:8080");
		}
		else if(STRISTR (charsvc_p_name,"tata"))
		{
			MSG_FATAL("tata................",0,0,0);
			STRCAT(urlCan,"access_point:proxy_is:172.23.252.15:9401");
		}
		else if(STRISTR (charsvc_p_name,"reliance"))
		{
			MSG_FATAL("reliance................",0,0,0);
			STRCAT(urlCan,"access_point:proxy_is:http://wapgw.ricinfo.com:8080");
		}
		else if(STRISTR (charsvc_p_name,"vmi"))
		{
			MSG_FATAL("vmi................",0,0,0);
			STRCAT(urlCan,"access_point:proxy_is:172.23.142.15:9401");
		}
		
		DBGPRINTF("urlCan==%s", urlCan);
		if(urlCan[0])
		{
			Result = ISHELL_StartAppletArgs(pShell, AEECLSID_UCWEB, (char*)urlCan);
		}
		else
		{
			Result = ISHELL_StartApplet(pShell, AEECLSID_UCWEB);
		}
	
		return Result;	
	}
	#ifdef FEATURE_SOUND_BO
	static void MainMenu_PlayShutterSound(MainMenu *pMe,int key)
	{
	
		AEEMediaCmdNotify cmd;
		int temp = 0;
		char music_name[256] = {0};
		if(pMe->m_pMedia)
		{
			IMEDIA_Stop(pMe->m_pMedia);
			IMEDIA_Release(pMe->m_pMedia);
			pMe->m_pMedia = NULL;
		}
		// 如果pMe->m_pMedia接口为空，创建接口
		if(!pMe->m_pMedia)
		{
			AEEMediaData	  md;
		   
			if(!pMe)
			   return;
			md.clsData = MMD_FILE_NAME; 		   
			md.pData = (void *)MAIN_SOUND_NAME[key];
			md.dwSize = 0;
		   
			(void)AEEMediaUtil_CreateMedia(pMe->m_pShell, &md, &pMe->m_pMedia);
		}
	   
		if(pMe->m_pMedia)
		{		 
			IMEDIA_SetVolume(pMe->m_pMedia, AEE_MAX_VOLUME*3/5); //max volum is 100
		 
			if(IMEDIA_RegisterNotify(pMe->m_pMedia, MainMenu_MediaNotify, pMe) != SUCCESS)
			{
				cmd.nCmd	= MM_CMD_PLAY;
				cmd.nStatus = MM_STATUS_DONE;
				MainMenu_MediaNotify((void *)pMe, &cmd);
				return;
			}
			MSG_FATAL("MainMenu_PlayShutterSound,IMEDIA_Play....",0,0,0);
			if(IMEDIA_Play(pMe->m_pMedia) != SUCCESS)
			{
				cmd.nCmd	= MM_CMD_PLAY;
				cmd.nStatus = MM_STATUS_DONE;
				MainMenu_MediaNotify((void *)pMe, &cmd);
				return;
			}
		}
		else
		{
			cmd.nCmd	= MM_CMD_PLAY;
			cmd.nStatus = MM_STATUS_DONE;
			MainMenu_MediaNotify((void *)pMe, &cmd);
		}  
	}
	
	static void MainMenu_MediaNotify(void *pUser, AEEMediaCmdNotify *pCmdNotify)
	{
		MainMenu *pMe = (MainMenu *)pUser;
	
		if(!pMe || !pCmdNotify)
			return;
	
		if(pCmdNotify->nCmd == MM_CMD_PLAY)  // IMEDIA_Play events
		{
			switch (pCmdNotify->nStatus)
			{
				case MM_STATUS_ABORT:			 
					break;
	
				case MM_STATUS_DONE:	// playback done
					if(pMe->m_pMedia)
					{
						IMEDIA_Release(pMe->m_pMedia);
						pMe->m_pMedia = NULL;
					}
					break;
				default:
					break;
			}
		}
	}
	#endif
#elif defined (FEATURE_VERSION_EC99)
static boolean Main_loadover = FALSE;
#ifdef FEATURE_SOUND_BO

#if 0   //FEATURE_CTA
static char* MAIN_SOUND_NAME[] =
{
	MUSIC_PATH1,
	MUSIC_PATH2,
	MUSIC_PATH3,
	MUSIC_PATH4,
	MUSIC_PATH5,
	/*MUSIC_PATH6,*/
	MUSIC_PATH7,
	MUSIC_PATH8,
};
#else
static char* MAIN_SOUND_NAME[] =
{
	MUSIC_PATH1,
	MUSIC_PATH2,
	MUSIC_PATH3,
	MUSIC_PATH4,
	MUSIC_PATH5,
	MUSIC_PATH6,
	MUSIC_PATH7,
	MUSIC_PATH8,
};
#endif
#endif
#define PARAM_NOT_REF(x)
/*==============================================================================

                              本地函数声明

==============================================================================*/
static MainMenuMod gMainMenuMod;
static const VTBL( IModule) gModFuncs =
{
    MainMenuMod_AddRef,
    MainMenuMod_Release,
    MainMenuMod_CreateInstance,
    MainMenuMod_FreeResources
};
#ifdef FEATURE_SOUND_BO
static void MainMenu_PlayShutterSound(MainMenu *pMe,int key);
static void MainMenu_MediaNotify(void *pUser, AEEMediaCmdNotify *pCmdNotify);
#endif
static uint32  MainMenu_AddRef( IMainMenu *pi);

static uint32  MainMenu_Release ( IMainMenu *pi);

static boolean MainMenu_HandleEvent( IMainMenu *pi,
    AEEEvent eCode,
    uint16  wParam,
    uint32 dwParam
);

static MainMenu gMainMenu={0};

static const VTBL( IMainMenu) gMainMenuMethods =
{
    MainMenu_AddRef,
    MainMenu_Release,
    MainMenu_HandleEvent
};

int MainMenuMod_Load( IShell *pIShell, void *ph, IModule **ppMod);

int MainMenuMod_New( int16 nSize,
    IShell              *pIShell,
    void                *ph,
    IModule             **ppMod,
    PFNMODCREATEINST    pfnMC,
    PFNFREEMODDATA      pfnMF
);

static int MainMenu_New( IShell *ps, IModule *pIModule, IMainMenu **ppObj);

static int CMainMenu_InitAppData(MainMenu *pMe);

static void CMainMenu_FreeAppData(MainMenu *pMe);

static void MainMenu_RunFSM(MainMenu *pMe);

static void MainMenu_DrawBackGround(MainMenu *pMe, AEERect *pRect);

// 初始整个背景及全部初始图标

int SetBrowserArr_Main(IShell *pShell ,char *purl);

static void DrawMatrix(MainMenu *pMe);

static void MoveCursorTo(MainMenu *pMe, int index);

static void SetCursor(MainMenu *pMe, int index);

static int StartApplet(MainMenu *pMe, int i);

boolean MainMenu_RouteDialogEvt(MainMenu *pMe,
    AEEEvent    eCode,
    uint16  wParam,
    uint32 dwParam
);

NextFSMAction MainMenu_ProcessState(MainMenu *pMe);

void MainMenu_ShowDialog(MainMenu  *pMe,  uint16 dlgResId);

static boolean  gbMainmenuLock = FALSE;

static void Main_keypadtimer(void *pUser);
static void Main_loadtimer(void *pUser);


// MAINST_MAIN 状态处理函数
static NextFSMAction MAINST_MAIN_Handler(MainMenu *pMe);
static NextFSMAction MAINST_FLASHLIGHT_Handler(MainMenu *pMe);        //Add By zzg 2013_08_20
//MAINST_EXIT  状态处理函数
static NextFSMAction MAINST_EXIT_Handler(MainMenu *pMe);
static boolean  MainMenu_IconMenuHandler(MainMenu *pMe, AEEEvent eCode, uint16 wParam, uint32 dwParam);
static boolean  MainMenu_FlashlightMenuHandler(MainMenu *pMe, AEEEvent eCode, uint16 wParam, uint32 dwParam); //Add By zzg 2013_08_20

/*==============================================================================
                              
                              本地（静态）数据
                              
==============================================================================*/

#if 0   //FEATURE_CTA
static char* ICON_ANI[] =
{
    ICON1_ANI,
    ICON2_ANI,
    ICON3_ANI,
    ICON4_ANI,
    ICON5_ANI,
    /*ICON6_ANI,*/
    ICON7_ANI,
    ICON8_ANI,
};

static char* MUSIC_PATH[] =
{
	MUSIC_PATH1,
	MUSIC_PATH2,
	MUSIC_PATH3,
	MUSIC_PATH4,
	MUSIC_PATH5,
	/*MUSIC_PATH6,*/
	MUSIC_PATH7,
	MUSIC_PATH8,
};
#else
static char* ICON_ANI[] =
{
    ICON1_ANI,
    ICON2_ANI,
    ICON3_ANI,
    ICON4_ANI,
    ICON5_ANI,
    ICON6_ANI,
    ICON7_ANI,
    ICON8_ANI,
};

static char* MUSIC_PATH[] =
{
	MUSIC_PATH1,
	MUSIC_PATH2,
	MUSIC_PATH3,
	MUSIC_PATH4,
	MUSIC_PATH5,
	MUSIC_PATH6,
	MUSIC_PATH7,
	MUSIC_PATH8,
};
#endif

/*=============================================================================
FUNCTION:  MainMenuMod_Load

DESCRIPTION: 

PARAMETERS:

RETURN VALUE:

COMMENTS:

SIDE EFFECTS:

SEE ALSO:

=============================================================================*/
int MainMenuMod_Load( IShell *pIShell, void *ph, IModule **ppMod)
{	
    return MainMenuMod_New( sizeof( MainMenuMod),
                    pIShell,
                    ph,
                    ppMod,
                    NULL,
                    NULL);
}

/*=============================================================================
FUNCTION:  MainMenuMod_New

DESCRIPTION: 

PARAMETERS:

RETURN VALUE:

COMMENTS:

SIDE EFFECTS:

SEE ALSO:

=============================================================================*/
int  MainMenuMod_New( int16 nSize,
    IShell   *pIShell,
    void     *ph,
    IModule  **ppMod,
    PFNMODCREATEINST pfnMC,
    PFNFREEMODDATA pfnMF
)
{
#if !defined( AEE_SIMULATOR)
    PARAM_NOT_REF( nSize)
    PARAM_NOT_REF( pfnMC)
    PARAM_NOT_REF( pfnMF)
    PARAM_NOT_REF( ph)
#endif

    if( NULL == ppMod)
    {
        return EFAILED;
    }

    MEMSET( &gMainMenuMod, 0, sizeof( MainMenuMod));

    INIT_VTBL( &gMainMenuMod, IModule, gModFuncs);
    gMainMenuMod.referenceCounter = 1;
    *ppMod = ( IModule *)&gMainMenuMod;
    return AEE_SUCCESS;
}

/*=============================================================================
FUNCTION:  MainMenuMod_AddRef

DESCRIPTION: 

PARAMETERS:

RETURN VALUE:

COMMENTS:

SIDE EFFECTS:

SEE ALSO:

=============================================================================*/
static uint32 MainMenuMod_AddRef( IModule *po)
{
    return( ++( ( MainMenuMod *)po)->referenceCounter);
}

/*=============================================================================
FUNCTION:  MainMenuMod_Release

DESCRIPTION: 

PARAMETERS:

RETURN VALUE:

COMMENTS:

SIDE EFFECTS:

SEE ALSO:

=============================================================================*/
static uint32 MainMenuMod_Release( IModule *po)
{
    if ( ( ( MainMenuMod *) po)->referenceCounter == 0)
    {
        return 0;
    }

    return( --( ( MainMenuMod *)po)->referenceCounter);
}

/*=============================================================================
FUNCTION:  MainMenuMod_CreateInstance

DESCRIPTION: 

PARAMETERS:

RETURN VALUE:

COMMENTS:

SIDE EFFECTS:

SEE ALSO:

=============================================================================*/
static int  MainMenuMod_CreateInstance( IModule *po,
    IShell *pIShell,
    AEECLSID ClsId,
    void **ppObj
)
{
    *ppObj = NULL;

    if( ClsId != AEECLSID_MAIN_MENU || MainMenu_New( pIShell, po, ( IMainMenu**)ppObj) != SUCCESS)
    {
        return EFAILED;
    }

    return SUCCESS;
}

/*=============================================================================
FUNCTION:  MainMenuMod_FreeResources

DESCRIPTION: 

PARAMETERS:

RETURN VALUE:

COMMENTS:

SIDE EFFECTS:

SEE ALSO:

=============================================================================*/
static void MainMenuMod_FreeResources( IModule *po, IHeap *ph, IFileMgr *pfm)
{
}

/*=============================================================================
FUNCTION:  MainMenu_New

DESCRIPTION: 

PARAMETERS:

RETURN VALUE:

COMMENTS:

SIDE EFFECTS:

SEE ALSO:

=============================================================================*/
static int MainMenu_New( IShell *ps, IModule *pIModule, IMainMenu **ppObj)
{
    int retVal = SUCCESS;
    if( 0 == gMainMenu.referenceCounter)
    {

        if( pIModule == NULL || ps == NULL)
        {
            return EFAILED;
        }

        MEMSET(&gMainMenu, 0, sizeof(gMainMenu));
        INIT_VTBL(&gMainMenu, IMainMenu, gMainMenuMethods);

       
        gMainMenu.m_pShell     = ps;
        gMainMenu.m_pModule    = pIModule;
        retVal = CMainMenu_InitAppData(&gMainMenu);
        if(retVal != SUCCESS)
        {
            CMainMenu_FreeAppData((MainMenu*)&gMainMenu);
            return retVal;
        }
        (void) ISHELL_AddRef( ps);
        (void) IMODULE_AddRef( pIModule);
        gMainMenu.referenceCounter  = 0;

        
    }

    ++ gMainMenu.referenceCounter;
    *ppObj = ( IMainMenu*)&gMainMenu;
    
    return retVal;
}

/*==============================================================================
函数:
    CMainMenuApp_InitAppData

说明:
    初始化 MainMenu Applet 结构数据缺省值。

参数:
    pMe [in]: 指向 MainMenu Applet 对象结构的指针。该结构包含小程序的特定信息。

返回值:
    EFAILED: 初始化失败。
    SUCCESS: 初始化成功。

备注:

==============================================================================*/
static int CMainMenu_InitAppData(MainMenu *pMe)
{		
    int i = 0;
	pMe->language = 0;

    pMe->m_pMedia = NULL;
    
    if (NULL == pMe)
    {
        return EFAILED;
    }
	pMe->m_bChangeL =FALSE;
		 
    pMe->m_index = 0;
    
	Main_loadover = FALSE;

    // 初始化菜单Title
	pMe->m_IconTitle[0]     = IDS_MAIN_MENU_RECENTCALLS;    
    pMe->m_IconTitle[1]     = IDS_MAIN_MENU_USERPROFILE;           
    pMe->m_IconTitle[2]     = IDS_MAIN_MENU_MULTIMEDIA;
    pMe->m_IconTitle[3]     = IDS_MAIN_MENU_CONTACTS;      
    pMe->m_IconTitle[4]     = IDS_MAIN_MENU_SETTINGS;   

#if 0   //FEATURE_CTA
    pMe->m_IconTitle[5]     = IDS_MAIN_MENU_TOOLS;       
    pMe->m_IconTitle[6]     = IDS_MAIN_MENU_MESSAGES;
#else    
    pMe->m_IconTitle[5]     = IDS_MAIN_MENU_UTK;       
    pMe->m_IconTitle[6]     = IDS_MAIN_MENU_TOOLS;
    pMe->m_IconTitle[7]     = IDS_MAIN_MENU_MESSAGES;    
#endif    
    
    
    //pMe->m_pImageBg = ISHELL_LoadImage(pMe->m_pShell, ICON_ANI_BG);
    
    // 接口创建及相关初始化
    if (ISHELL_CreateInstance(pMe->m_pShell, AEECLSID_CONFIG,
            (void **)&pMe->m_pConfig) != SUCCESS)
    {    	
        return EFAILED;
    }

    if( ISHELL_CreateInstance( pMe->m_pShell, AEECLSID_BACKLIGHT, (void **)&pMe->m_pBacklight)!=AEE_SUCCESS)
    {
        return FALSE;
    }
     
    if (ISHELL_CreateInstance(pMe->m_pShell, AEECLSID_DISPLAY, 
            (void **) &pMe->m_pDisplay) != SUCCESS)
    {    	
        return EFAILED;
    }
	if (AEE_SUCCESS != ISHELL_CreateInstance(pMe->m_pShell,
                                            AEECLSID_ANNUNCIATOR,
                                            (void **)&pMe->m_pIAnn))
    {    	
        return EFAILED;
    }
  
	(void) ICONFIG_GetItem(pMe->m_pConfig,
									   CFGI_LANGUAGE_SELECTION,
									   &pMe->language,
									   sizeof(pMe->language));

    /*
	if(pMe->language == NV_LANGUAGE_ENGLISH)
	{
	    for (i=0; i<MAX_MATRIX_ITEMS; i++)
		{
		    pMe->m_pImageSelectEC99[i] = ISHELL_LoadImage(pMe->m_pShell, ICON_ANI[i]);                
        }
		pMe->m_bChangeL =TRUE;
	}
	else
	{
	    for (i=0; i<MAX_MATRIX_ITEMS; i++)
		{
		    pMe->m_pImageSelectEC99[i] = ISHELL_LoadImage(pMe->m_pShell, ICON_ANI[i]);    
        }
		pMe->m_bChangeL =FALSE;
	}	
    */
	
	MSG_FATAL("CMainMenu_InitAppData.............over.....",0,0,0);
    return SUCCESS;
}

/*==============================================================================
函数:
    CMainMenu_FreeAppData

说明:
    释放 MainMenu Applet 相关资源。

参数:
    pMe [in]: 指向MainMenu Applet 对象结构的指针。该结构包含小程序的特定信息。

返回值:
    none

备注:

==============================================================================*/
static void CMainMenu_FreeAppData(MainMenu *pMe)
{
    if (NULL == pMe)
    {
        return;
    }

    pMe->m_eAppStatus = MAINMENU_STOP;
    if (NULL != pMe->m_pConfig)
    {
        ICONFIG_Release(pMe->m_pConfig);
        pMe->m_pConfig = NULL;
    }    
    
    if (pMe->m_pDisplay != NULL)
    {
        (void) IDISPLAY_Release(pMe->m_pDisplay);
        pMe->m_pDisplay = NULL;
    }
	if (pMe->m_pIAnn)
    {
        IANNUNCIATOR_Release(pMe->m_pIAnn);
		pMe->m_pIAnn = NULL;
    }

    if(pMe->m_pBacklight)
    {
        IBACKLIGHT_Release(pMe->m_pBacklight);
        pMe->m_pBacklight=NULL;
    }
	if(pMe->m_pMedia)
    {
        IMEDIA_Release(pMe->m_pMedia);
        pMe->m_pMedia = NULL;
    }
    //释放图片资源
    Main_loadover = FALSE;
    {
         int i;
         int j;
         if (pMe->m_pImageBg !=NULL)
         {
             (void) IIMAGE_Release(pMe->m_pImageBg);
             pMe->m_pImageBg = NULL;
         }
 
         for(i=0;i<MAX_MATRIX_ITEMS;i++)
         {
             if(pMe->m_pImageIcon[i] != NULL)
             {
                 (void)IIMAGE_Release(pMe->m_pImageIcon[i]);
                 pMe->m_pImageIcon[i] = NULL;
             }
         } 

        /*
 		for(j=0;j<MAX_MATRIX_ITEMS;j++)
         {
             if(pMe->m_pImageSelectEC99[j] != NULL)
             {   
                 (void)IIMAGE_Release(pMe->m_pImageSelectEC99[j]);
                 pMe->m_pImageSelectEC99[j] = NULL;
             }
         } 
        */

		
         if(pMe->m_pImageSelectEC99 != NULL)
             {   
                 (void)IIMAGE_Release(pMe->m_pImageSelectEC99);
                 pMe->m_pImageSelectEC99 = NULL;
             }
    }  
}

/*=============================================================================
FUNCTION:  MainMenu_AddRef

DESCRIPTION: 

PARAMETERS:

RETURN VALUE:

COMMENTS:

SIDE EFFECTS:

SEE ALSO:

=============================================================================*/
static uint32  MainMenu_AddRef( IMainMenu *pi)
{
    return ( ++gMainMenu.referenceCounter);
}

/*=============================================================================
FUNCTION:  MainMenu_Release

DESCRIPTION: 

PARAMETERS:

RETURN VALUE:

COMMENTS:

SIDE EFFECTS:

SEE ALSO:

=============================================================================*/
static uint32  MainMenu_Release( IMainMenu *pi)
{
    register MainMenu *pMe = (MainMenu*)pi;
    if (pMe->referenceCounter == 0)
    {
        return 0;
    }
    
    if( --pMe->referenceCounter)
    {
        return pMe->referenceCounter;
    }


    CMainMenu_FreeAppData(pMe);

    (void) ISHELL_Release(pMe->m_pShell);
    (void) IMODULE_Release(pMe->m_pModule);
    return 0;
}

/*==============================================================================
函数:
    MainMenu_RunFSM

说明:
    运行有限状态机引擎。

参数:
    pMe [in]: 指向MainMenu Applet对象结构的指针。该结构包含小程序的特定信息。
       
返回值:
    无。

备注:

==============================================================================*/
static void MainMenu_RunFSM(MainMenu *pMe)
{
    NextFSMAction nextFSMAction = NFSMACTION_WAIT;
    for ( ; ; )
    {
        nextFSMAction = MainMenu_ProcessState(pMe);

        // 这里不需要复杂的状态机，因此为简化状态机设计这里仅用一个布尔变量来
        // 指示dlgResult是否应该被改写。在某些特殊场合（见状态机设计），这种作
        // 法，避免了不必要的复杂性，简化了设计。
        if (pMe->m_bDoNotOverwriteDlgResult)
        {
            // We allow only one iteration without touching this variable. So,
            // let's reset it back to indicate that we are going to overwrite the
            // result for all subsequent iterations.
            pMe->m_bDoNotOverwriteDlgResult = FALSE;
        }
        else
        {
            // Overwrite the dlgResult to its default value. The next dialog that
            // we are going to display will set and return the dlgResult based on
            // the users action.
            pMe->m_eDlgReturn = DLGRET_CREATE;
        }

        // If the state machine processing engine indicates that it created a
        // dialog and requesting us to exit this loop, then we quit.
        if (nextFSMAction == NFSMACTION_WAIT)
        {
            break;
        }
    }
}

/*=============================================================================
FUNCTION:  MainMenu_HandleEvent

DESCRIPTION: 

PARAMETERS:

RETURN VALUE:

COMMENTS:

SIDE EFFECTS:

SEE ALSO:

=============================================================================*/
static boolean MainMenu_HandleEvent( IMainMenu *pi,
    AEEEvent eCode,
    uint16  wParam,
    uint32 dwParam
)
{
    MainMenu *pMe = (MainMenu*)pi;
    AEEAppStart* as = NULL;
    switch ( eCode)
    {
        case EVT_APP_START:
#ifdef FEATURE_RANDOM_MENU_REND
            DisplayRend_Enable(TRUE);
#endif
            // 此事件dwParam为指针，不应为0
            if (dwParam == 0) 
            {
                return FALSE;
            }            
            as = (AEEAppStart*)dwParam;

            pMe->m_index = 0;
   
            pMe->m_rc = as->rc;

            pMe->m_currState = MAINST_MAIN;
            pMe->m_eDlgReturn = DLGRET_CREATE;
            pMe->m_eAppStatus = MAINMENU_RUNNING;
            IANNUNCIATOR_SetFieldIsActiveEx(pMe->m_pIAnn,TRUE); 
            MainMenu_RunFSM(pMe);
            return TRUE;

        case EVT_APP_STOP:
            {
                int theFocus = 0;
                SetCursor( pMe, theFocus);
                (void)ISHELL_CancelTimer( pMe->m_pShell, NULL, pMe);
                pMe->m_eAppStatus = MAINMENU_STOP;
#ifdef FEATURE_RANDOM_MENU_REND
                DisplayRend_Enable(FALSE);
#endif
                return TRUE;
            }

        case EVT_APP_SUSPEND:
            (void)ISHELL_CancelTimer(pMe->m_pShell, NULL, pMe);
            pMe->m_eAppStatus = MAINMENU_SUSPEND;
#ifdef FEATURE_RANDOM_MENU_REND
            DisplayRend_Push();
            DisplayRend_Start(FALSE);
#endif
            return TRUE;
        

        case EVT_APP_RESUME:
            {
                AEEAppStart* as = ( AEEAppStart*)dwParam;
                pMe->m_rc    = as->rc;
                pMe->m_eAppStatus = MAINMENU_RUNNING;
#ifdef FEATURE_RANDOM_MENU_REND
                DisplayRend_Push();
                DisplayRend_Start(TRUE);
#endif
                // 开始跑WMS状态机
                MainMenu_RunFSM(pMe); 
                return TRUE;
            }

        case EVT_DIALOG_INIT:
            pMe->m_pActiveIDlg = (IDialog*)dwParam;
            pMe->m_pActivedlgID = wParam;
            return MainMenu_RouteDialogEvt(pMe,eCode,wParam,dwParam);

        case EVT_DIALOG_START:
            return MainMenu_RouteDialogEvt(pMe,eCode,wParam,dwParam);

        case EVT_USER_REDRAW:
            return MainMenu_RouteDialogEvt(pMe,eCode,wParam,dwParam);

        case EVT_DIALOG_END:
#ifdef FEATURE_VERSION_EC99
            if (pMe->m_pImageSelectEC99 != NULL)
            {   
                 (void)IIMAGE_Release(pMe->m_pImageSelectEC99);
                 pMe->m_pImageSelectEC99 = NULL;
            }
#endif            

            (void) MainMenu_RouteDialogEvt(pMe,eCode,wParam,dwParam);
            pMe->m_pActiveIDlg = NULL;
            pMe->m_pActivedlgID = 0;
            if(pMe->m_eAppStatus == MAINMENU_RUNNING)
            {
                MainMenu_RunFSM(pMe);
            }
            return TRUE;
        case EVT_KEY:
#if MIN_BREW_VERSION(3,0)
            // do not want to handle au
            if (((dwParam & KB_AUTOREPEAT) != 0) &&
                (wParam != AVK_UP) &&
                (wParam != AVK_DOWN) &&
                (wParam != AVK_LEFT) &&
                (wParam != AVK_RIGHT))
            {
                return TRUE;
            }
#endif
            return MainMenu_RouteDialogEvt(pMe,eCode,wParam,dwParam);  
#if defined (FEATURE_VERSION_N021)||defined(FEATURE_VERSION_W0216A)
		case EVT_KEY_HELD:
			{
				if(wParam == AVK_INFO)
				{
					(void) ISHELL_PostEvent(pMe->m_pShell,
                                        AEECLSID_CORE_APP,
                                        EVT_USER,
                                        1,
                                        0);
                    ISHELL_CloseApplet(pMe->m_pShell,TRUE);
				}
			}
			break;
#endif
        case EVT_KEY_PRESS:
        case EVT_KEY_RELEASE:
        case EVT_COMMAND:
            return MainMenu_RouteDialogEvt(pMe,eCode,wParam,dwParam); 

        default:
            return MainMenu_RouteDialogEvt(pMe,eCode,wParam,dwParam);
    }
}

void Mainmenu_KeypadLock(boolean block)
{
   gbMainmenuLock = block;
}

/*==============================================================================
函数:
    MainMenu_ProcessState

说明:
    根据 pMe->m_currState, 调用相应的状态处理函数。

参数:
    pMe [in]: 指向MainMenu Applet对象结构的指针。该结构包含小程序的特定信息。

返回值:
    NFSMACTION_CONTINUE: 指示不停状态机。
    NFSMACTION_WAIT: 指示停止状态机。

备注:

==============================================================================*/
NextFSMAction MainMenu_ProcessState(MainMenu *pMe)
{
    NextFSMAction retVal = NFSMACTION_WAIT;
    if (NULL == pMe)
    {
        return retVal;
    }
    switch(pMe->m_currState)
    {
        case MAINST_MAIN:
            return MAINST_MAIN_Handler(pMe);
        case MAINST_FLASHLIGHT:
        	return MAINST_FLASHLIGHT_Handler(pMe);    
        case MAINST_EXIT:
            return MAINST_EXIT_Handler(pMe);  
        default:
            break;

    }
    return retVal;
}

/*==============================================================================
函数:
    MAINST_MAIN_Handler

说明:
    MAINST_MAIN 状态处理函数。

参数:
    pMe [in]: 指向MAINMENU Applet对象结构的指针。该结构包含小程序的特定信息。

返回值:
    NFSMACTION_CONTINUE: 指示不停状态机。
    NFSMACTION_WAIT: 指示停止状态机。

备注:

==============================================================================*/
static NextFSMAction MAINST_MAIN_Handler(MainMenu *pMe)
{
    if (NULL == pMe)
    {
        return NFSMACTION_WAIT;
    }
    switch (pMe->m_eDlgReturn)
    {
        // 进入主界面
        case DLGRET_CREATE:
            {
                MainMenu_ShowDialog(pMe, IDD_MAIN_MENU);
            }
            return NFSMACTION_WAIT;
            
        case DLGRET_FLASHLITHT:
        	{
        		MOVE_TO_STATE(MAINST_FLASHLIGHT)
            	return NFSMACTION_CONTINUE;
        	}    
        case DLGRET_CANCELED:
            MOVE_TO_STATE(MAINST_EXIT)
            return NFSMACTION_CONTINUE;

        default:
            MOVE_TO_STATE(MAINST_EXIT)
            return NFSMACTION_CONTINUE;
    }
}


static NextFSMAction MAINST_FLASHLIGHT_Handler(MainMenu *pMe)
{
	if (NULL == pMe)
    {
        return NFSMACTION_WAIT;
    }
    switch (pMe->m_eDlgReturn)
    {
        // 进入主界面
        case DLGRET_CREATE:
            {
                MainMenu_ShowDialog(pMe, IDD_FLASHLIGHT_SETTING);
            }
            return NFSMACTION_WAIT;
         case DLGRET_CANCELED:         
            MOVE_TO_STATE(MAINST_MAIN)
            return NFSMACTION_CONTINUE;
        default:
            MOVE_TO_STATE(MAINST_EXIT)
            return NFSMACTION_CONTINUE;
    }
}


/*==============================================================================
函数:
    MAINST_EXIT_Handler

说明:
    MAINST_EXIT 状态处理函数。

参数:
    pMe [in]: 指向MAINMENU  Applet对象结构的指针。该结构包含小程序的特定信息。

返回值:
    NFSMACTION_CONTINUE: 指示不停状态机。
    NFSMACTION_WAIT: 指示停止状态机。

备注:

==============================================================================*/
static NextFSMAction MAINST_EXIT_Handler(MainMenu *pMe)
{
    (void) ISHELL_CloseApplet(pMe->m_pShell, FALSE);
    return NFSMACTION_WAIT;
} 

/*==============================================================================
函数:
       MainMenu_ShowDialog

说明:
    函数由状态处理函数用来唤起相应对话框资源ID的对话框。

参数:
    pMe [in]: 指向 MAINMENU Applet 对象结构的指针。该结构包含小程序的特定信息。
    dlgResId [in]: 对话框资源ID，函数根据该ID创建对话框。

返回值:
    none

备注:

==============================================================================*/
void MainMenu_ShowDialog(MainMenu  *pMe,  uint16 dlgResId)
{
    int nRet;

    // 每次最多打开一个对话框
    if (ISHELL_GetActiveDialog(pMe->m_pShell) != NULL)
    {
        // 如果当前已经有对话框被打开，直接返回
        return;
    }
   
    if (NULL != pMe->m_pDisplay)
    {
        AEEDeviceInfo di={0,};
#if defined (FEATURE_VERSION_H19C)
        if(pMe->m_pIAnn != NULL)
        {
            if (dlgResId == IDD_MAIN_MENU)
            {
                IANNUNCIATOR_SetHasTitleText(pMe->m_pIAnn, FALSE);
            }
            else
            {
                IANNUNCIATOR_SetHasTitleText(pMe->m_pIAnn, TRUE);
            }
            IANNUNCIATOR_Redraw(pMe->m_pIAnn);
        }
        (void)IDISPLAY_SetPrefs(pMe->m_pDisplay, "a:1", STRLEN("a:1"));
#else
        if (dlgResId == IDD_MAIN_MENU)
        {
            (void)IDISPLAY_SetPrefs(pMe->m_pDisplay, "a:0", STRLEN("a:0"));
        }
        else
        {
            (void)IDISPLAY_SetPrefs(pMe->m_pDisplay, "a:1", STRLEN("a:1"));
        }
#endif
        ISHELL_GetDeviceInfo(pMe->m_pShell, &di);
        pMe->m_rc.dx = di.cxScreen;
        pMe->m_rc.dy = di.cyScreen;       
        IDISPLAY_SetClipRect(pMe->m_pDisplay, &pMe->m_rc);        
    }
    
    nRet = ISHELL_CreateDialog(pMe->m_pShell,MAINMENU_RES_FILE_LANG,dlgResId,NULL);
    if (nRet != SUCCESS)
    {
    }
}


/*==============================================================================
函数:
       MainMenu_RouteDialogEvt
说明:
       函数将BREW事件路由给当前活动对话框的事件处理函数。

参数:
       pMe [In]: 指向SettingMenu Applet对象结构的指针,该结构包含小程序的特定信息.
       eCode [in]：事件代码。
       wParam [in]：与事件eCode关联的数据。
       dwParam [in]：与事件eCode关联的数据。

返回值:
       TRUE：传入事件被处理。
       FALSE：传入事件没被处理。

备注:

==============================================================================*/
boolean MainMenu_RouteDialogEvt(MainMenu *pMe,
    AEEEvent    eCode,
    uint16  wParam,
    uint32 dwParam
)

{
    if (NULL == pMe)
    {
        return FALSE;
    }

    if (NULL == pMe->m_pActiveIDlg)
    {
        return FALSE;
    }    
    switch( pMe->m_pActivedlgID)
    {
        case IDD_MAIN_MENU:
            return MainMenu_IconMenuHandler(pMe, eCode, wParam, dwParam);

        case IDD_FLASHLIGHT_SETTING:
            return MainMenu_FlashlightMenuHandler(pMe, eCode, wParam,dwParam);
        default:
            return FALSE;
    }
}

/*=============================================================================
FUNCTION:  MainMenu_IconMenuHandler

DESCRIPTION: 12宫格菜单

PARAMETERS:

=============================================================================*/
static boolean MainMenu_IconMenuHandler(MainMenu *pMe, AEEEvent eCode, uint16 wParam, uint32 dwParam)
{
    PARAM_NOT_REF( dwParam)
    switch ( eCode)
    {
        case EVT_DIALOG_INIT:			
            IDIALOG_SetProperties((IDialog *)dwParam, DLG_NOT_REDRAW_AFTER_START);
			MSG_FATAL("MainMenu_IconMenuHandler.........init....",0,0,0);
            return TRUE;

        case EVT_DIALOG_START:			
			AEE_CancelTimer(Main_keypadtimer,pMe);
			AEE_SetTimer(5*1000,Main_keypadtimer,pMe);
            (void) ISHELL_PostEvent( pMe->m_pShell,
                                     AEECLSID_MAIN_MENU,
                                     EVT_USER_REDRAW,
                                     0,
                                     0);
			MSG_FATAL("MainMenu_IconMenuHandler.........EVT_DIALOG_START....",0,0,0);
            return TRUE;

        case EVT_USER_REDRAW:
            // 初始整个背景及全部初始图标
            {
            	
            	MSG_FATAL("MainMenu_IconMenuHandler.........EVT_USER_REDRAW....",0,0,0);
            	DrawMatrix(pMe);
				MSG_FATAL("MainMenu_IconMenuHandler.........EVT_USER_REDRAW..over..",0,0,0);
				//AEE_CancelTimer(Main_loadtimer,pMe);
				//AEE_SetTimer(100,Main_loadtimer,pMe);
            	return TRUE;
            }
            
        case EVT_DIALOG_END:
            {
                ISHELL_CancelTimer(pMe->m_pShell, NULL, (void**)pMe);
            }
            return TRUE;

        case EVT_KEY_HELD:            
        #if defined(FEATURE_TORCH_KEY_INFO)
           if((AVKType)wParam == AVK_INFO)
            {
                boolean TorchOn = FALSE;
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
                ISHELL_CloseApplet(pMe->m_pShell, TRUE); 
                return TRUE;
            }
        #endif
	 #if defined(FEATURE_TORCH_KEY_SELECT) //xxzhen
           if((AVKType)wParam == AVK_SELECT)
            {
                boolean TorchOn = FALSE;
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
                ISHELL_CloseApplet(pMe->m_pShell, TRUE); 
                return TRUE;
            }
        #endif

            return TRUE;
        case EVT_KEY:
            switch( wParam)
            {
                case AVK_CLR:
                     CLOSE_DIALOG(DLGRET_CANCELED)
                    return TRUE;
                     
                case AVK_UP:       
                case AVK_LEFT:
                   if(pMe->m_index>0)
                   {
                   	   pMe->m_index = pMe->m_index-1;
                   }
				   else if(pMe->m_index == 0)
				   {
				   		pMe->m_index = (MAX_MATRIX_ITEMS-1);
				   }
				   MoveCursorTo(pMe, pMe->m_index);
                   return TRUE;

                case AVK_DOWN:     
                case AVK_RIGHT:

                   if(pMe->m_index<(MAX_MATRIX_ITEMS-1))
                   {
                   	   pMe->m_index = pMe->m_index+1;
					  
                   }
				   else if(pMe->m_index == (MAX_MATRIX_ITEMS-1))
				   {
				   		pMe->m_index = 0;
				   }
				   MoveCursorTo(pMe, pMe->m_index);
                   return TRUE;
                    
                case AVK_SELECT:
                case AVK_INFO:
                    {
                       StartApplet(pMe, pMe->m_IconTitle[pMe->m_index]);
                    }
                    return TRUE;
                case AVK_STAR:
                    if(gbMainmenuLock)
                    {
                        OEMKeyguard_SetState(TRUE);
                        ISHELL_CloseApplet(pMe->m_pShell, TRUE); 
                    }
                    return TRUE;
                default:
                    break;
            }

            return TRUE;
            
        default:
            break;
    }

    return FALSE;
}

//Add By zzg 2013_08_20
static boolean MainMenu_FlashlightMenuHandler(MainMenu *pMe, AEEEvent eCode, uint16 wParam, uint32 dwParam)
{
    PARAM_NOT_REF(dwParam)
    IMenuCtl *pMenu = (IMenuCtl*)IDIALOG_GetControl(pMe->m_pActiveIDlg,IDC_FLASHLIGHT_SET);
    AECHAR WTitle[40] = {0};
    if (pMenu == NULL)
    {
        return FALSE;
    }
    if(pMe->m_pIAnn != NULL)
    {
	    IANNUNCIATOR_SetFieldIsActiveEx(pMe->m_pIAnn,FALSE);
    }
     switch (eCode)
    {
        case EVT_DIALOG_INIT:
			(void)ISHELL_LoadResString(pMe->m_pShell,
                                        MAINMENU_RES_FILE_LANG,                                
                                        IDS_MAIN_MENU_TORCH,
                                        WTitle,
                                        sizeof(WTitle));

            if(pMe->m_pIAnn != NULL)
            {
			    IANNUNCIATOR_SetFieldTextEx(pMe->m_pIAnn,WTitle,FALSE);
            }
            IMENUCTL_AddItem(pMenu, MAINMENU_RES_FILE_LANG, IDS_MAIN_MENU_TORCH_ON, IDS_MAIN_MENU_TORCH_ON, NULL, 0);
            IMENUCTL_AddItem(pMenu, MAINMENU_RES_FILE_LANG, IDS_MAIN_MENU_TORCH_OFF, IDS_MAIN_MENU_TORCH_OFF, NULL, 0);
            return TRUE;
            
        case EVT_DIALOG_START:
            {
            	uint16 wItemID;
                boolean Is_on = FALSE;
                IMENUCTL_SetProperties(pMenu, MP_UNDERLINE_TITLE|MP_WRAPSCROLL|MP_TEXT_ALIGN_LEFT_ICON_ALIGN_RIGHT);
                IMENUCTL_SetOemProperties(pMenu, OEMMP_USE_MENU_STYLE);

                IMENUCTL_SetBottomBarType(pMenu,BTBAR_SELECT_BACK);
                OEM_GetConfig(CFGI_FLSHLITHG_STATUS,&Is_on, sizeof(Is_on));
                if(Is_on)
                {
                	wItemID = IDS_MAIN_MENU_TORCH_ON;
                }
                else
                {
                	wItemID = IDS_MAIN_MENU_TORCH_OFF;
                }

                InitMenuIcons(pMenu);
                SetMenuIcon(pMenu, wItemID, TRUE);
                IMENUCTL_SetSel(pMenu, wItemID);
            }
            return TRUE;

        case EVT_DIALOG_END:
            return TRUE;

        case EVT_KEY:
            switch(wParam)
            {
                case AVK_CLR:
                    CLOSE_DIALOG(DLGRET_CANCELED)
                    return TRUE;

                  default:
                    break;
            }
            return TRUE;
#ifdef FEATURE_LCD_TOUCH_ENABLE//andrew add for LCD touch
		case EVT_PEN_UP:
			{
				AEEDeviceInfo devinfo;
				int nBarH ;
				AEERect rc;
				int16 wXPos = (int16)AEE_GET_X(dwParam);
				int16 wYPos = (int16)AEE_GET_Y(dwParam);

				nBarH = GetBottomBarHeight(pMe->m_pDisplay);
        
				MEMSET(&devinfo, 0, sizeof(devinfo));
				ISHELL_GetDeviceInfo(pMe->m_pShell, &devinfo);
				SETAEERECT(&rc, 0, devinfo.cyScreen-nBarH, devinfo.cxScreen, nBarH);

				if(TOUCH_PT_IN_RECT(wXPos,wYPos,rc))
				{
					if(wXPos >= rc.x && wXPos < rc.x + (rc.dx/3) )//左
					{
						boolean rt =  ISHELL_PostEvent(pMe->m_pShell,AEECLSID_APPLICATION,EVT_KEY,AVK_SELECT,0);
						return rt;
					}
					else if(wXPos >= rc.x + (rc.dx/3)   && wXPos < rc.x + (rc.dx/3)*2 )//左
					{
						 boolean rt = ISHELL_PostEvent(pMe->m_pShell,AEECLSID_APPLICATION,EVT_KEY,AVK_INFO,0);
						 return rt;
					}
					else if(wXPos >= rc.x + (rc.dx/3)*2 && wXPos < rc.x + (rc.dx/3)*3 )//左
					{						
						 boolean rt = ISHELL_PostEvent(pMe->m_pShell,AEECLSID_APPLICATION,EVT_KEY,AVK_CLR,0);
						 return rt;
					}
				}

			}
			break;
#endif 
        case EVT_COMMAND:
            {
                boolean bytNewData = 0;

                switch (wParam)
                {
                    case  IDS_MAIN_MENU_TORCH_ON:
                       bytNewData = TRUE;
                       IBACKLIGHT_TurnOnTorch(pMe->m_pBacklight);
                       break;

                    case IDS_MAIN_MENU_TORCH_OFF:
                       bytNewData = FALSE;
                       IBACKLIGHT_TurnOffTorch(pMe->m_pBacklight);
                       break;

                    default:
                       break;

                }
                OEM_SetConfig(CFGI_FLSHLITHG_STATUS,&bytNewData, sizeof(bytNewData));
                InitMenuIcons(pMenu);
                SetMenuIcon(pMenu, wParam, TRUE);
                (void) ISHELL_PostEvent( pMe->m_pShell,
                                         AEECLSID_MAIN_MENU,
                                         EVT_USER_REDRAW,
                                         0,
                                         0);
            }
        default:
        	break;
	}
}
//Add End


static void Main_keypadtimer(void *pUser)
{
    gbMainmenuLock =FALSE;
}
static void Main_loadtimer(void *pUser)
{
	MainMenu	*pMe = (MainMenu *)pUser;
	int i = 0;
	int j = 0;
	for (i = 0; i < MAX_MATRIX_ITEMS; i ++)
    {
        if (pMe->m_pImageIcon[i] == NULL)
        {
            pMe->m_pImageIcon[i] = ISHELL_LoadImage(pMe->m_pShell,
                                                    ICON_ANI[i]);
        }
    }  
    /*
	for(j=0;j<MAX_MATRIX_ITEMS;j++)
	{
		if (pMe->m_pImageSelectEC99[j] == NULL)
        {           
            pMe->m_pImageSelectEC99[j] = ISHELL_LoadImage(pMe->m_pShell, ICON_ANI[j]);
        }
	}
    */
	Main_loadover = TRUE;
	
}

static void MainMenu_DrawBackGround(MainMenu *pMe, AEERect *pRect)
{

    Appscommon_ResetBackground(pMe->m_pDisplay, 
                              pMe->m_pImageBg, 
                              APPSCOMMON_BG_COLOR, 
                              pRect, 
                              0, 
                              0);

}

/*=============================================================================
FUNCTION:  DrawMatrix

DESCRIPTION: // 初始整个背景及全部初始图标

=============================================================================*/
static void DrawMatrix(MainMenu *pMe)
{
    
	BottomBar_e_Type    eBBarType = BTBAR_NONE;
    boolean m_sound_bo_main = FALSE;
    if (NULL == pMe)
    {
        return;
    }
    //draw bg image
    //MainMenu_DrawBackGround(pMe, &pMe->m_rc);
    
	(void) ICONFIG_GetItem(pMe->m_pConfig,
									   CFGI_LANGUAGE_SELECTION,
									   &pMe->language,
									   sizeof(pMe->language));

   /*
    if(pMe->m_pImageSelectEC99[pMe->m_index]!=NULL)
	{
		IIMAGE_Draw(pMe->m_pImageSelectEC99[pMe->m_index], 0, 0);
	}
    */

   
    if (pMe->m_pImageSelectEC99 != NULL)
    {   
         (void)IIMAGE_Release(pMe->m_pImageSelectEC99);
         pMe->m_pImageSelectEC99 = NULL;
    }
    
    pMe->m_pImageSelectEC99 = ISHELL_LoadImage(pMe->m_pShell, ICON_ANI[pMe->m_index]);      
    if(pMe->m_pImageSelectEC99 != NULL)
    {
        IIMAGE_Draw(pMe->m_pImageSelectEC99, 0, 0);
    }
	
#ifdef FEATURE_SOUND_BO		
   	(void) ICONFIG_GetItem(pMe->m_pConfig,
                                 CFGI_SOUND_BO_MAIN,
                                 &m_sound_bo_main,
                                 sizeof(boolean));

	if(m_sound_bo_main)
	{
        /*
		nv_item_type	SimChoice;
		(void)OEMNV_Get(NV_SIM_SELECT_I,&SimChoice);
		if(SimChoice.sim_select != 1)
        */    
        if (GetMp3PlayerStatus() == MP3STATUS_NONE)
		{
			MainMenu_PlayShutterSound(pMe,pMe->m_index);
		}
	}
#endif
	IDISPLAY_UpdateEx(pMe->m_pDisplay, TRUE);   
}


/*=============================================================================
FUNCTION:  MoveCursorTo

DESCRIPTION: 

PARAMETERS:

RETURN VALUE:

COMMENTS:

SIDE EFFECTS:

SEE ALSO:

=============================================================================*/
static void MoveCursorTo(MainMenu *pMe, int index)
{
	
	boolean m_sound_bo_main = FALSE;
	//draw bg image
	if (NULL == pMe)
    {
        return;
    }
	(void) ICONFIG_GetItem(pMe->m_pConfig,
									   CFGI_LANGUAGE_SELECTION,
									   &pMe->language,
									   sizeof(pMe->language));

#ifdef FEATURE_SOUND_BO    
	(void) ICONFIG_GetItem(pMe->m_pConfig,
                                 CFGI_SOUND_BO_MAIN,
                                 &m_sound_bo_main,
                                 sizeof(boolean));
	if(m_sound_bo_main)
	{
	    /*
		nv_item_type	SimChoice;
		(void)OEMNV_Get(NV_SIM_SELECT_I,&SimChoice);        
		if(SimChoice.sim_select != 1)
        */    
        if (GetMp3PlayerStatus() == MP3STATUS_NONE)
		{
			MainMenu_PlayShutterSound(pMe,pMe->m_index);
		}
	}
#endif
    MainMenu_DrawBackGround(pMe, &pMe->m_rc);


    if (pMe->m_pImageSelectEC99 != NULL)
    {   
         (void)IIMAGE_Release(pMe->m_pImageSelectEC99);
         pMe->m_pImageSelectEC99 = NULL;
    }

    pMe->m_pImageSelectEC99 = ISHELL_LoadImage(pMe->m_pShell, ICON_ANI[pMe->m_index]);      
    if(pMe->m_pImageSelectEC99 != NULL)
    {
        IIMAGE_Draw(pMe->m_pImageSelectEC99, 0, 0);
    }
    
	IDISPLAY_UpdateEx(pMe->m_pDisplay, TRUE);
}
/*=============================================================================
FUNCTION:  SetCursor

DESCRIPTION: 

PARAMETERS:

RETURN VALUE:

COMMENTS:

SIDE EFFECTS:

SEE ALSO:

=============================================================================*/
static void SetCursor(MainMenu *pMe, int index)
{
    pMe->m_index        = index;   
    
    if(pMe->m_pAnimate != NULL)
    {
        IImage_Release(pMe->m_pAnimate);
        pMe->m_pAnimate  = NULL;
    }
}

/*=============================================================================
FUNCTION:  StartApplet

DESCRIPTION:  启动applet

PARAMETERS:  如果APPLET 有变动，只需改动次函数

=============================================================================*/
static int StartApplet(MainMenu *pMe, int i)
{
    int Result = EUNSUPPORTED;
	//uint32  dwTotal = 0;
	//uint32 free = 0;
	//GETFSFREE(&dwTotal);
	//free = GETRAMFREE(NULL,NULL);
	//MSG_FATAL("dwTotal======%d,free===%d",dwTotal,free,0);

    
#ifdef FEATURE_VERSION_H19C  
    if(pMe->m_pIAnn != NULL)
    {
        IANNUNCIATOR_SetHasTitleText(pMe->m_pIAnn, TRUE);
    }
#endif
		#ifdef FEATURE_VERSION_K202_LM129C //xxzhen
		  if(pMe->m_pIAnn != NULL)
          {
              IANNUNCIATOR_SetHasTitleText(pMe->m_pIAnn, TRUE);
          }
		#endif

    switch(i)
	{
#ifdef FEATURE_VERSION_W208S
	case IDS_MAIN_MENU_MAP:
	{
		char	buf[12];
		
		OEM_SetBROWSER_ADSAccount();	//Add By zzg 2011_07_08
		
    	MSG_FATAL("AEECLSID_BRW_APP...........START",0,0,0);
    	start_info.appid_fx = APP_ORIGINATOR_BROWSER;
        start_info.subtype_keycode = APP_BROWSER_START_URL;
        start_info.par_keychar = 0;

		start_info.fInfo.url_info.title = "map";
		start_info.fInfo.url_info.url = "http://wap.movidamovil.com/venezuela/?debug=true";		
		
        //start_info.fInfo.url_info.title = "163";
        //start_info.fInfo.url_info.url = "http://wap.163.com";

		
        SPRINTF(buf, "%p", &start_info);
		Result = ISHELL_StartAppletArgs(pMe->m_pShell,AEECLSID_NF3,buf);
        //Result = ISHELL_StartApplet(pMe->m_pShell, AEECLSID_BRW_APP);
        MSG_FATAL("AEECLSID_BRW_APP...........Result=%d",Result,0,0);		
		
		
		break;
	}
	case IDS_MAIN_MENU_NEO_APP:
	{
        Result = ISHELL_StartApplet(pMe->m_pShell, AEECLSID_APPMANAGER);
		break;
	}
	case IDS_MAIN_MENU_NEO_NAV:
	{
		char	buf[12];
		
		OEM_SetBROWSER_ADSAccount();	//Add By zzg 2011_07_08
		
    	MSG_FATAL("AEECLSID_BRW_APP...........START",0,0,0);
    	start_info.appid_fx = APP_ORIGINATOR_BROWSER;
        start_info.subtype_keycode = APP_BROWSER_START_MAINMENU;
        start_info.par_keychar = 0;
        //start_info.fInfo.url_info.title = "163";
        //start_info.fInfo.url_info.url = "http://www.163.com";
        SPRINTF(buf, "%p", &start_info);
		Result = ISHELL_StartAppletArgs(pMe->m_pShell,AEECLSID_NF3,buf);
        //Result = ISHELL_StartApplet(pMe->m_pShell, AEECLSID_BRW_APP);
        MSG_FATAL("AEECLSID_BRW_APP...........Result=%d",Result,0,0);		
		
		break;
	}
#endif

    case IDS_MAIN_MENU_FILEMGR:
    case IDS_MAIN_MENU_GALLERY:
	case IDS_MAIN_MENU_MEDIAGALLERY_C337:	
    case IDS_MAIN_MENU_MEDIAGALLERY:
        Result = ISHELL_StartApplet(pMe->m_pShell, AEECLSID_MEDIAGALLERY);
        break;
        
    case IDS_MAIN_MENU_CONTACT:
	case IDS_PHONEBOOK:
    case IDS_MAIN_MENU_CONTACTS:
        {
            IContactApp *ca = NULL;
            Result = ISHELL_CreateInstance(pMe->m_pShell,AEECLSID_APP_CONTACT, (void**)&ca);
            if(SUCCESS == Result)
            {
                ICONTAPP_MainMenu(ca);
                IContactApp_Release(ca);
            }
        }
        break;
    case IDS_MAIN_MENU_INTEXZONE: 
#ifdef FEATURE_VERSION_C316
		Result = ISHELL_StartApplet(pMe->m_pShell, AEECLSID_INDEX_ZONE);
		break;
#endif
    case IDS_MAIN_MENU_SMARTFRENACCESS:
    case IDS_MAIN_MENU_UTK:
        Result = ISHELL_StartApplet(pMe->m_pShell, AEECLSID_APP_UTK);
        break;
        
    case IDS_MAIN_MENU_TOOLS:
	case IDS_MAIN_MENU_ORGANIZER:	
    case IDS_MAIN_MENU_APPLICATION:
        Result = ISHELL_StartApplet(pMe->m_pShell, AEECLSID_APPLICATION);
        break;
		
    case IDS_MAIN_MENU_CALL_LOGS:
	case IDS_MAIN_MENU_RECENTCALLS_C337:	
    case IDS_MAIN_MENU_RECENTCALLS:
        Result = ISHELL_StartApplet(pMe->m_pShell, AEECLSID_APP_RECENTCALL);
        break;
        
    case IDS_MAIN_MENU_SMS:
    case IDS_MAIN_MENU_MESSAGES:
        Result = ISHELL_StartApplet(pMe->m_pShell, AEECLSID_WMSAPP);
        break;
        
    case IDS_MAIN_MENU_MULTIMEDIA:
        Result = ISHELL_StartApplet(pMe->m_pShell, AEECLSID_MULTIMEDIA_LIST);
        break;
        
    case IDS_MAIN_MENU_STATIC_APPLICATION:
#if defined(FEATURE_APP_MANAGER)
#ifdef FEATURE_OEMOMH
        if(ISHELL_SendEvent(pMe->m_pShell,AEECLSID_DIALER,EVT_OMH_PROMPT,0,0))
        {
            break;
        }
#endif

        Result = ISHELL_StartApplet(pMe->m_pShell, AEECLSID_APPMANAGER);
#else
        Result = ISHELL_StartApplet(pMe->m_pShell, AEECLSID_STATIC_APP);
#endif
    break;
        
    case IDS_MAIN_MENU_CALENDAR:
    case IDS_MAIN_MENU_SCHEDULER:
        Result = ISHELL_StartApplet(pMe->m_pShell, AEECLSID_SCHEDULEAPP);
        break;
        
    case IDS_MAIN_MENU_SETTINGS:
        Result = ISHELL_StartApplet(pMe->m_pShell, AEECLSID_APP_SETTINGMENU);
        break;

	//Add By zzg 2012_11_08 for C337 
	case IDS_MAIN_MENU_SERVICES:		
	    #if defined (FEATURE_VERSION_C337) || defined(FEATURE_VERSION_IC241A_MMX)
        #if defined (FEATURE_VERSION_IN50A)
        Result = SetBrowserArr_Main(pMe->m_pShell,(char*)"http://www.google.com.hk/"); 
        #else
        Result = SetBrowserArr_Main(pMe->m_pShell,(char*)"http://mimicromax.com"); //ISHELL_StartAppletArgs(pMe->m_pShell, AEECLSID_UCWEB, (char*)"call_ucweb:setmainpageurl:http://mimicromax.com");
        #endif        
        #else
        Result = SetBrowserArr_Main(pMe->m_pShell,NULL);//ISHELL_StartApplet(pMe->m_pShell, AEECLSID_UCWEB);
        #endif
		break;
	case IDS_MAIN_MENU_MSTORE:
#if defined (FEATURE_VERSION_C337) || defined(FEATURE_VERSION_IC241A_MMX)
#if defined (FEATURE_VERSION_IN50A)
        Result = SetBrowserArr_Main(pMe->m_pShell,(char*)"http://www.google.com.hk/"); 
#else
        Result = SetBrowserArr_Main(pMe->m_pShell,(char*)"http://mimicromax.com"); //ISHELL_StartAppletArgs(pMe->m_pShell, AEECLSID_UCWEB, (char*)"call_ucweb:setmainpageurl:http://mimicromax.com");
#endif        
#else
        Result = SetBrowserArr_Main(pMe->m_pShell,NULL);//ISHELL_StartAppletArgs(pMe->m_pShell, AEECLSID_UCWEB, (char*)"call_ucweb:setmainpageurl:http://mimicromax.com");
#endif
		break;
	case IDS_MAIN_MENU_MZONE:
		Result = ISHELL_StartApplet(pMe->m_pShell, AEECLSID_MiZone);
		break;
	//Add End
	
	case IDS_MAIN_MENU_USER_PROFILE:	
    case IDS_MAIN_MENU_USERPROFILE:
        Result = ISHELL_StartApplet(pMe->m_pShell, AEECLSID_APP_SOUNDMENU);
        break;
        
    case IDS_MAIN_MENU_GAMES:
        Result = ISHELL_StartApplet(pMe->m_pShell, AEECLSID_GAME);
        break;
		
	#ifdef FEATURE_VERSION_C316
    case IDS_MAIN_MENU_SERVICES_C316:  
		Result = ISHELL_StartApplet(pMe->m_pShell, AEECLSID_SERVICE);
        break;
	#endif	
	
    case IDS_MAIN_WAPBROWSER:
		#ifdef FEATURE_VERSION_K202_LM129C
		  IANNUNCIATOR_SetHasTitleText(pMe->m_pIAnn,FALSE); //xxzhen
		   IANNUNCIATOR_Redraw(pMe->m_pIAnn);
		#endif

        #if defined (FEATURE_VERSION_C337) || defined(FEATURE_VERSION_IC241A_MMX)
        #if defined (FEATURE_VERSION_IN50A)
        Result = SetBrowserArr_Main(pMe->m_pShell,(char*)"http://www.google.com.hk/"); 
        #else
        Result = SetBrowserArr_Main(pMe->m_pShell,(char*)"http://mimicromax.com"); //ISHELL_StartAppletArgs(pMe->m_pShell, AEECLSID_UCWEB, (char*)"call_ucweb:setmainpageurl:http://mimicromax.com");
        #endif        
        #else
        Result = SetBrowserArr_Main(pMe->m_pShell,(char*)""); //ISHELL_StartApplet(pMe->m_pShell, AEECLSID_UCWEB);
        #endif
        break;
    
    case IDS_MAIN_MENU_CALCULATOR: 
		 Result = ISHELL_StartApplet(pMe->m_pShell, AEECLSID_CALCAPP);
        break;
        
    case IDS_MAIN_MENU_FMRADIO:
        Result = ISHELL_StartApplet(pMe->m_pShell, AEECLSID_APP_FMRADIO);
        break;
        
    case IDS_MAIN_MENU_MUSICPLAYER:
        Result = ISHELL_StartApplet(pMe->m_pShell, AEECLSID_APP_MUSICPLAYER);
        break;
        
    case IDS_MAIN_MENU_SMARTWORLD:
    case IDS_MAIN_MENU_BLIVE:
#ifdef FEATURE_SMARTFREN_STATIC_BREW_APP
		Result = ISHELL_StartApplet(pMe->m_pShell, AEECLSID_STATIC_APP);
#else
#ifdef FEATURE_OEMOMH
        if(ISHELL_SendEvent(pMe->m_pShell,AEECLSID_DIALER,EVT_OMH_PROMPT,0,0))
        {
            break;
        }
#endif
        Result = ISHELL_StartApplet(pMe->m_pShell, AEECLSID_APPMANAGER);
#endif  
        break;
    
    case IDS_MAIN_MENU_FRENDUO:
#if defined(FEATURE_VERSION_M8) || defined(FEATURE_VERSION_M8P) || defined (FEATURE_VERSION_M8021)
		Result = ISHELL_StartApplet(pMe->m_pShell, AEECLSID_FRENDUO);
#endif
        break;
    
    case IDS_MAIN_MENU_TIMER:
        Result = ISHELL_StartApplet(pMe->m_pShell, AEECLSID_APPTIMER);
        break;
        
    case IDS_MAIN_MENU_CAMERA:
	{		
		Result = ISHELL_StartApplet(pMe->m_pShell, AEECLSID_APP_CAMERA);
		break;
	}  
    
    case IDS_MAIN_MENU_TORCH:
    {		
		//Result = ISHELL_StartApplet(pMe->m_pShell, AEECLSID_TORCH);
		CLOSE_DIALOG(DLGRET_FLASHLITHT)
		break;
	}    
    
    default:
        break;

    }
    return Result;
}

extern char charsvc_p_name[UIM_CDMA_HOME_SERVICE_SIZE+1];
int SetBrowserArr_Main(IShell *pShell ,char *purl)
{
	int Result = EUNSUPPORTED;
	char urlCan[1024] = {0};
    
	DBGPRINTF("svc_p_name %s %d",charsvc_p_name,charsvc_p_name[0],0);
	OEM_SetUCBROWSER_ADSAccount();
    
	if(purl && STRLEN(purl)>1)
	{
        SPRINTF(urlCan,"call_ucweb:setexternurl:%s\2\3",purl);
		STRCAT(urlCan,"useragent:BREW-Applet/0x20068888 (BREW/3.1.5.20; DeviceId: 8976509865757e; Lang: hi; Profile/MIDP-2.0_Configuration/CLDC-1.1) ucweb-squid\2\3");
	}
	else
	{
		STRCPY(urlCan,"useragent:BREW-Applet/0x20068888 (BREW/3.1.5.20; DeviceId: 8976509865757e; Lang: hi; Profile/MIDP-2.0_Configuration/CLDC-1.1) ucweb-squid\2\3");
	}
    
	if(STRISTR (charsvc_p_name,"mts"))
	{
		MSG_FATAL("mst................",0,0,0);
        STRCAT(urlCan,"access_point:proxy_is:10.50.5.140:8080");
	}
	else if(STRISTR (charsvc_p_name,"tata"))
	{
        MSG_FATAL("tata................",0,0,0);
        STRCAT(urlCan,"access_point:proxy_is:172.23.252.15:9401");
	}
	else if(STRISTR (charsvc_p_name,"reliance"))
	{
        MSG_FATAL("reliance................",0,0,0);
        STRCAT(urlCan,"access_point:proxy_is:http://wapgw.ricinfo.com:8080");
	}
	else if(STRISTR (charsvc_p_name,"vmi"))
	{
        MSG_FATAL("vmi................",0,0,0);
        STRCAT(urlCan,"access_point:proxy_is:172.23.142.15:9401");
	}
	
	DBGPRINTF("urlCan==%s", urlCan);
    if(urlCan[0])
    {
	    Result = ISHELL_StartAppletArgs(pShell, AEECLSID_UCWEB, (char*)urlCan);
    }
    else
    {
        Result = ISHELL_StartApplet(pShell, AEECLSID_UCWEB);
    }

	return Result;	
}
static void MainMenu_PlayShutterSound(MainMenu *pMe,int key)
{

    AEEMediaCmdNotify cmd;
	int temp = 0;
	char music_name[256] = {0};

    byte        byte_return;    
    ICONFIG_GetItem(pMe->m_pConfig,CFGI_RINGER_VOL,&byte_return,sizeof(byte_return));

    if (OEMSOUND_MUTE_VOL == byte_return)
    {
        return;
    }
    
    if(pMe->m_pMedia)
	{
		IMEDIA_Stop(pMe->m_pMedia);
		IMEDIA_Release(pMe->m_pMedia);
		pMe->m_pMedia = NULL;
	}
    // 如果pMe->m_pMedia接口为空，创建接口
    if(!pMe->m_pMedia)
    {
        AEEMediaData      md;
       
        if(!pMe)
           return;
        md.clsData = MMD_FILE_NAME;  		   
		md.pData = (void *)MAIN_SOUND_NAME[key];
        md.dwSize = 0;
       
        (void)AEEMediaUtil_CreateMedia(pMe->m_pShell, &md, &pMe->m_pMedia);
    }
   
    if(pMe->m_pMedia)
    {        
        IMEDIA_SetVolume(pMe->m_pMedia, AEE_MAX_VOLUME*3/5); //max volum is 100
     
        if(IMEDIA_RegisterNotify(pMe->m_pMedia, MainMenu_MediaNotify, pMe) != SUCCESS)
        {
            cmd.nCmd    = MM_CMD_PLAY;
            cmd.nStatus = MM_STATUS_DONE;
            MainMenu_MediaNotify((void *)pMe, &cmd);
            return;
        }
        MSG_FATAL("MainMenu_PlayShutterSound,IMEDIA_Play....",0,0,0);
        if(IMEDIA_Play(pMe->m_pMedia) != SUCCESS)
        {
            cmd.nCmd    = MM_CMD_PLAY;
            cmd.nStatus = MM_STATUS_DONE;
            MainMenu_MediaNotify((void *)pMe, &cmd);
            return;
        }
    }
    else
    {
        cmd.nCmd    = MM_CMD_PLAY;
        cmd.nStatus = MM_STATUS_DONE;
        MainMenu_MediaNotify((void *)pMe, &cmd);
    }  
}

static void MainMenu_MediaNotify(void *pUser, AEEMediaCmdNotify *pCmdNotify)
{
    MainMenu *pMe = (MainMenu *)pUser;

    if(!pMe || !pCmdNotify)
        return;

    if(pCmdNotify->nCmd == MM_CMD_PLAY)  // IMEDIA_Play events
    {
        switch (pCmdNotify->nStatus)
        {
            case MM_STATUS_ABORT:            
                break;

            case MM_STATUS_DONE:    // playback done
				if(pMe->m_pMedia)
				{
					IMEDIA_Release(pMe->m_pMedia);
					pMe->m_pMedia = NULL;
				}
                break;
            default:
                break;
        }
    }
}
#elif defined (FEATURE_VERSION_K212_20D)
static boolean Main_loadover = FALSE;
#ifdef FEATURE_SOUND_BO
static char* MAIN_SOUND_NAME[] =
{
	MUSIC_PATH1,
	MUSIC_PATH2,
	MUSIC_PATH3,
	MUSIC_PATH4,
	MUSIC_PATH5,
	MUSIC_PATH6,
	MUSIC_PATH7,
};
#endif
#define PARAM_NOT_REF(x)
/*==============================================================================

                              本地函数声明

==============================================================================*/
static MainMenuMod gMainMenuMod;
static const VTBL( IModule) gModFuncs =
{
    MainMenuMod_AddRef,
    MainMenuMod_Release,
    MainMenuMod_CreateInstance,
    MainMenuMod_FreeResources
};
#ifdef FEATURE_SOUND_BO
static void MainMenu_PlayShutterSound(MainMenu *pMe,int key);
static void MainMenu_MediaNotify(void *pUser, AEEMediaCmdNotify *pCmdNotify);
#endif
static uint32  MainMenu_AddRef( IMainMenu *pi);

static uint32  MainMenu_Release ( IMainMenu *pi);

static boolean MainMenu_HandleEvent( IMainMenu *pi,
    AEEEvent eCode,
    uint16  wParam,
    uint32 dwParam
);

static MainMenu gMainMenu={0};

static const VTBL( IMainMenu) gMainMenuMethods =
{
    MainMenu_AddRef,
    MainMenu_Release,
    MainMenu_HandleEvent
};

int MainMenuMod_Load( IShell *pIShell, void *ph, IModule **ppMod);

int MainMenuMod_New( int16 nSize,
    IShell              *pIShell,
    void                *ph,
    IModule             **ppMod,
    PFNMODCREATEINST    pfnMC,
    PFNFREEMODDATA      pfnMF
);

static int MainMenu_New( IShell *ps, IModule *pIModule, IMainMenu **ppObj);

static int CMainMenu_InitAppData(MainMenu *pMe);

static void CMainMenu_FreeAppData(MainMenu *pMe);

static void MainMenu_RunFSM(MainMenu *pMe);

static void MainMenu_DrawBackGround(MainMenu *pMe, AEERect *pRect);

// 初始整个背景及全部初始图标

int SetBrowserArr_Main(IShell *pShell ,char *purl);

static void DrawMatrix(MainMenu *pMe);

static void MoveCursorTo(MainMenu *pMe, int index);

static void SetCursor(MainMenu *pMe, int index);

static int StartApplet(MainMenu *pMe, int i);

boolean MainMenu_RouteDialogEvt(MainMenu *pMe,
    AEEEvent    eCode,
    uint16  wParam,
    uint32 dwParam
);

NextFSMAction MainMenu_ProcessState(MainMenu *pMe);

void MainMenu_ShowDialog(MainMenu  *pMe,  uint16 dlgResId);

static boolean  gbMainmenuLock = FALSE;

static void Main_keypadtimer(void *pUser);
static void Main_loadtimer(void *pUser);


// MAINST_MAIN 状态处理函数
static NextFSMAction MAINST_MAIN_Handler(MainMenu *pMe);
static NextFSMAction MAINST_FLASHLIGHT_Handler(MainMenu *pMe);        //Add By zzg 2013_08_20
//MAINST_EXIT  状态处理函数
static NextFSMAction MAINST_EXIT_Handler(MainMenu *pMe);
static boolean  MainMenu_IconMenuHandler(MainMenu *pMe, AEEEvent eCode, uint16 wParam, uint32 dwParam);
static boolean  MainMenu_FlashlightMenuHandler(MainMenu *pMe, AEEEvent eCode, uint16 wParam, uint32 dwParam); //Add By zzg 2013_08_20

/*==============================================================================
                              
                              本地（静态）数据
                              
==============================================================================*/

static char* ICON_ANI[] =
{
    ICON1_ANI,
    ICON2_ANI,
    ICON3_ANI,
    ICON4_ANI,
    ICON5_ANI,
    ICON6_ANI,
    ICON7_ANI,
};

static char* MUSIC_PATH[] =
{
	MUSIC_PATH1,
	MUSIC_PATH2,
	MUSIC_PATH3,
	MUSIC_PATH4,
	MUSIC_PATH5,
	MUSIC_PATH6,
	MUSIC_PATH7,
};

/*=============================================================================
FUNCTION:  MainMenuMod_Load

DESCRIPTION: 

PARAMETERS:

RETURN VALUE:

COMMENTS:

SIDE EFFECTS:

SEE ALSO:

=============================================================================*/
int MainMenuMod_Load( IShell *pIShell, void *ph, IModule **ppMod)
{	
    return MainMenuMod_New( sizeof( MainMenuMod),
                    pIShell,
                    ph,
                    ppMod,
                    NULL,
                    NULL);
}

/*=============================================================================
FUNCTION:  MainMenuMod_New

DESCRIPTION: 

PARAMETERS:

RETURN VALUE:

COMMENTS:

SIDE EFFECTS:

SEE ALSO:

=============================================================================*/
int  MainMenuMod_New( int16 nSize,
    IShell   *pIShell,
    void     *ph,
    IModule  **ppMod,
    PFNMODCREATEINST pfnMC,
    PFNFREEMODDATA pfnMF
)
{
#if !defined( AEE_SIMULATOR)
    PARAM_NOT_REF( nSize)
    PARAM_NOT_REF( pfnMC)
    PARAM_NOT_REF( pfnMF)
    PARAM_NOT_REF( ph)
#endif

    if( NULL == ppMod)
    {
        return EFAILED;
    }

    MEMSET( &gMainMenuMod, 0, sizeof( MainMenuMod));

    INIT_VTBL( &gMainMenuMod, IModule, gModFuncs);
    gMainMenuMod.referenceCounter = 1;
    *ppMod = ( IModule *)&gMainMenuMod;
    return AEE_SUCCESS;
}

/*=============================================================================
FUNCTION:  MainMenuMod_AddRef

DESCRIPTION: 

PARAMETERS:

RETURN VALUE:

COMMENTS:

SIDE EFFECTS:

SEE ALSO:

=============================================================================*/
static uint32 MainMenuMod_AddRef( IModule *po)
{
    return( ++( ( MainMenuMod *)po)->referenceCounter);
}

/*=============================================================================
FUNCTION:  MainMenuMod_Release

DESCRIPTION: 

PARAMETERS:

RETURN VALUE:

COMMENTS:

SIDE EFFECTS:

SEE ALSO:

=============================================================================*/
static uint32 MainMenuMod_Release( IModule *po)
{
    if ( ( ( MainMenuMod *) po)->referenceCounter == 0)
    {
        return 0;
    }

    return( --( ( MainMenuMod *)po)->referenceCounter);
}

/*=============================================================================
FUNCTION:  MainMenuMod_CreateInstance

DESCRIPTION: 

PARAMETERS:

RETURN VALUE:

COMMENTS:

SIDE EFFECTS:

SEE ALSO:

=============================================================================*/
static int  MainMenuMod_CreateInstance( IModule *po,
    IShell *pIShell,
    AEECLSID ClsId,
    void **ppObj
)
{
    *ppObj = NULL;

    if( ClsId != AEECLSID_MAIN_MENU || MainMenu_New( pIShell, po, ( IMainMenu**)ppObj) != SUCCESS)
    {
        return EFAILED;
    }

    return SUCCESS;
}

/*=============================================================================
FUNCTION:  MainMenuMod_FreeResources

DESCRIPTION: 

PARAMETERS:

RETURN VALUE:

COMMENTS:

SIDE EFFECTS:

SEE ALSO:

=============================================================================*/
static void MainMenuMod_FreeResources( IModule *po, IHeap *ph, IFileMgr *pfm)
{
}

/*=============================================================================
FUNCTION:  MainMenu_New

DESCRIPTION: 

PARAMETERS:

RETURN VALUE:

COMMENTS:

SIDE EFFECTS:

SEE ALSO:

=============================================================================*/
static int MainMenu_New( IShell *ps, IModule *pIModule, IMainMenu **ppObj)
{
    int retVal = SUCCESS;
    if( 0 == gMainMenu.referenceCounter)
    {

        if( pIModule == NULL || ps == NULL)
        {
            return EFAILED;
        }

        MEMSET(&gMainMenu, 0, sizeof(gMainMenu));
        INIT_VTBL(&gMainMenu, IMainMenu, gMainMenuMethods);

       
        gMainMenu.m_pShell     = ps;
        gMainMenu.m_pModule    = pIModule;
        retVal = CMainMenu_InitAppData(&gMainMenu);
        if(retVal != SUCCESS)
        {
            CMainMenu_FreeAppData((MainMenu*)&gMainMenu);
            return retVal;
        }
        (void) ISHELL_AddRef( ps);
        (void) IMODULE_AddRef( pIModule);
        gMainMenu.referenceCounter  = 0;

        
    }

    ++ gMainMenu.referenceCounter;
    *ppObj = ( IMainMenu*)&gMainMenu;
    
    return retVal;
}

/*==============================================================================
函数:
    CMainMenuApp_InitAppData

说明:
    初始化 MainMenu Applet 结构数据缺省值。

参数:
    pMe [in]: 指向 MainMenu Applet 对象结构的指针。该结构包含小程序的特定信息。

返回值:
    EFAILED: 初始化失败。
    SUCCESS: 初始化成功。

备注:

==============================================================================*/
static int CMainMenu_InitAppData(MainMenu *pMe)
{	
	int8 count_main = 1;
    int i = 0;
	pMe->language = 0;

    pMe->m_pMedia = NULL;
    
    if (NULL == pMe)
    {
        return EFAILED;
    }
	pMe->m_bChangeL =FALSE;
	OEM_GetConfig(CFGI_COUNT_OF_MAIN, &count_main, sizeof(int8));
	MSG_FATAL("CMainMenu_InitAppData.............=%d",count_main,0,0);
	 
    pMe->m_index = count_main;
    
	Main_loadover = FALSE;

    // 初始化菜单Title
	pMe->m_IconTitle[0]     = IDS_MAIN_MENU_RECENTCALLS;          
    pMe->m_IconTitle[1]     = IDS_MAIN_MENU_MULTIMEDIA;
    pMe->m_IconTitle[2]     = IDS_MAIN_MENU_CONTACTS;      
    pMe->m_IconTitle[3]     = IDS_MAIN_MENU_SETTINGS;   
    pMe->m_IconTitle[4]     = IDS_MAIN_MENU_UTK;       
    pMe->m_IconTitle[5]     = IDS_MAIN_MENU_TOOLS;
    pMe->m_IconTitle[6]     = IDS_MAIN_MENU_MESSAGES;   
    
    
    //pMe->m_pImageBg = ISHELL_LoadImage(pMe->m_pShell, ICON_ANI_BG);
    
    // 接口创建及相关初始化
    if (ISHELL_CreateInstance(pMe->m_pShell, AEECLSID_CONFIG,
            (void **)&pMe->m_pConfig) != SUCCESS)
    {    	
        return EFAILED;
    }

    if( ISHELL_CreateInstance( pMe->m_pShell, AEECLSID_BACKLIGHT, (void **)&pMe->m_pBacklight)!=AEE_SUCCESS)
    {
        return FALSE;
    }
     
    if (ISHELL_CreateInstance(pMe->m_pShell, AEECLSID_DISPLAY, 
            (void **) &pMe->m_pDisplay) != SUCCESS)
    {    	
        return EFAILED;
    }
	if (AEE_SUCCESS != ISHELL_CreateInstance(pMe->m_pShell,
                                            AEECLSID_ANNUNCIATOR,
                                            (void **)&pMe->m_pIAnn))
    {    	
        return EFAILED;
    }
  
	(void) ICONFIG_GetItem(pMe->m_pConfig,
									   CFGI_LANGUAGE_SELECTION,
									   &pMe->language,
									   sizeof(pMe->language));
	
	MSG_FATAL("CMainMenu_InitAppData.............over.....",0,0,0);
    return SUCCESS;
}

/*==============================================================================
函数:
    CMainMenu_FreeAppData

说明:
    释放 MainMenu Applet 相关资源。

参数:
    pMe [in]: 指向MainMenu Applet 对象结构的指针。该结构包含小程序的特定信息。

返回值:
    none

备注:

==============================================================================*/
static void CMainMenu_FreeAppData(MainMenu *pMe)
{
    if (NULL == pMe)
    {
        return;
    }

    pMe->m_eAppStatus = MAINMENU_STOP;
    if (NULL != pMe->m_pConfig)
    {
        ICONFIG_Release(pMe->m_pConfig);
        pMe->m_pConfig = NULL;
    }    
    
    if (pMe->m_pDisplay != NULL)
    {
        (void) IDISPLAY_Release(pMe->m_pDisplay);
        pMe->m_pDisplay = NULL;
    }
	if (pMe->m_pIAnn)
    {
        IANNUNCIATOR_Release(pMe->m_pIAnn);
		pMe->m_pIAnn = NULL;
    }

    if(pMe->m_pBacklight)
    {
        IBACKLIGHT_Release(pMe->m_pBacklight);
        pMe->m_pBacklight=NULL;
    }
	if(pMe->m_pMedia)
    {
        IMEDIA_Release(pMe->m_pMedia);
        pMe->m_pMedia = NULL;
    }
    //释放图片资源
    Main_loadover = FALSE;
    {
         int i;
         int j;
         if (pMe->m_pImageBg !=NULL)
         {
             (void) IIMAGE_Release(pMe->m_pImageBg);
             pMe->m_pImageBg = NULL;
         }
 
         for(i=0;i<MAX_MATRIX_ITEMS;i++)
         {
             if(pMe->m_pImageIcon[i] != NULL)
             {
                 (void)IIMAGE_Release(pMe->m_pImageIcon[i]);
                 pMe->m_pImageIcon[i] = NULL;
             }
         } 
         if(pMe->m_pImageSelectEC99 != NULL)
             {   
                 (void)IIMAGE_Release(pMe->m_pImageSelectEC99);
                 pMe->m_pImageSelectEC99 = NULL;
             }
    }  
}

/*=============================================================================
FUNCTION:  MainMenu_AddRef

DESCRIPTION: 

PARAMETERS:

RETURN VALUE:

COMMENTS:

SIDE EFFECTS:

SEE ALSO:

=============================================================================*/
static uint32  MainMenu_AddRef( IMainMenu *pi)
{
    return ( ++gMainMenu.referenceCounter);
}

/*=============================================================================
FUNCTION:  MainMenu_Release

DESCRIPTION: 

PARAMETERS:

RETURN VALUE:

COMMENTS:

SIDE EFFECTS:

SEE ALSO:

=============================================================================*/
static uint32  MainMenu_Release( IMainMenu *pi)
{
    register MainMenu *pMe = (MainMenu*)pi;
    if (pMe->referenceCounter == 0)
    {
        return 0;
    }
    
    if( --pMe->referenceCounter)
    {
        return pMe->referenceCounter;
    }


    CMainMenu_FreeAppData(pMe);

    (void) ISHELL_Release(pMe->m_pShell);
    (void) IMODULE_Release(pMe->m_pModule);
    return 0;
}

/*==============================================================================
函数:
    MainMenu_RunFSM

说明:
    运行有限状态机引擎。

参数:
    pMe [in]: 指向MainMenu Applet对象结构的指针。该结构包含小程序的特定信息。
       
返回值:
    无。

备注:

==============================================================================*/
static void MainMenu_RunFSM(MainMenu *pMe)
{
    NextFSMAction nextFSMAction = NFSMACTION_WAIT;
    for ( ; ; )
    {
        nextFSMAction = MainMenu_ProcessState(pMe);

        // 这里不需要复杂的状态机，因此为简化状态机设计这里仅用一个布尔变量来
        // 指示dlgResult是否应该被改写。在某些特殊场合（见状态机设计），这种作
        // 法，避免了不必要的复杂性，简化了设计。
        if (pMe->m_bDoNotOverwriteDlgResult)
        {
            // We allow only one iteration without touching this variable. So,
            // let's reset it back to indicate that we are going to overwrite the
            // result for all subsequent iterations.
            pMe->m_bDoNotOverwriteDlgResult = FALSE;
        }
        else
        {
            // Overwrite the dlgResult to its default value. The next dialog that
            // we are going to display will set and return the dlgResult based on
            // the users action.
            pMe->m_eDlgReturn = DLGRET_CREATE;
        }

        // If the state machine processing engine indicates that it created a
        // dialog and requesting us to exit this loop, then we quit.
        if (nextFSMAction == NFSMACTION_WAIT)
        {
            break;
        }
    }
}

/*=============================================================================
FUNCTION:  MainMenu_HandleEvent

DESCRIPTION: 

PARAMETERS:

RETURN VALUE:

COMMENTS:

SIDE EFFECTS:

SEE ALSO:

=============================================================================*/
static boolean MainMenu_HandleEvent( IMainMenu *pi,
    AEEEvent eCode,
    uint16  wParam,
    uint32 dwParam
)
{
    MainMenu *pMe = (MainMenu*)pi;
    AEEAppStart* as = NULL;
    switch ( eCode)
    {
        case EVT_APP_START:
#ifdef FEATURE_RANDOM_MENU_REND
            DisplayRend_Enable(TRUE);
#endif
            // 此事件dwParam为指针，不应为0
            if (dwParam == 0) 
            {
                return FALSE;
            }            
            as = (AEEAppStart*)dwParam;
   
            pMe->m_rc = as->rc;

            pMe->m_currState = MAINST_MAIN;
            pMe->m_eDlgReturn = DLGRET_CREATE;
            pMe->m_eAppStatus = MAINMENU_RUNNING;
            IANNUNCIATOR_SetFieldIsActiveEx(pMe->m_pIAnn,TRUE); 
            MainMenu_RunFSM(pMe);
            return TRUE;

        case EVT_APP_STOP:
            {
                int theFocus = 0;
                SetCursor( pMe, theFocus);
                (void)ISHELL_CancelTimer( pMe->m_pShell, NULL, pMe);
                pMe->m_eAppStatus = MAINMENU_STOP;
#ifdef FEATURE_RANDOM_MENU_REND
                DisplayRend_Enable(FALSE);
#endif
                return TRUE;
            }

        case EVT_APP_SUSPEND:
            (void)ISHELL_CancelTimer(pMe->m_pShell, NULL, pMe);
            pMe->m_eAppStatus = MAINMENU_SUSPEND;
#ifdef FEATURE_RANDOM_MENU_REND
            DisplayRend_Push();
            DisplayRend_Start(FALSE);
#endif
            return TRUE;
        

        case EVT_APP_RESUME:
            {
                AEEAppStart* as = ( AEEAppStart*)dwParam;
                pMe->m_rc    = as->rc;
                pMe->m_eAppStatus = MAINMENU_RUNNING;
#ifdef FEATURE_RANDOM_MENU_REND
                DisplayRend_Push();
                DisplayRend_Start(TRUE);
#endif
                // 开始跑WMS状态机
                MainMenu_RunFSM(pMe); 
                return TRUE;
            }

        case EVT_DIALOG_INIT:
            pMe->m_pActiveIDlg = (IDialog*)dwParam;
            pMe->m_pActivedlgID = wParam;
            return MainMenu_RouteDialogEvt(pMe,eCode,wParam,dwParam);

        case EVT_DIALOG_START:
            return MainMenu_RouteDialogEvt(pMe,eCode,wParam,dwParam);

        case EVT_USER_REDRAW:
            return MainMenu_RouteDialogEvt(pMe,eCode,wParam,dwParam);

        case EVT_DIALOG_END:

            (void) MainMenu_RouteDialogEvt(pMe,eCode,wParam,dwParam);
            pMe->m_pActiveIDlg = NULL;
            pMe->m_pActivedlgID = 0;
            if(pMe->m_eAppStatus == MAINMENU_RUNNING)
            {
                MainMenu_RunFSM(pMe);
            }
            return TRUE;
        case EVT_KEY:
#if MIN_BREW_VERSION(3,0)
            // do not want to handle au
            if (((dwParam & KB_AUTOREPEAT) != 0) &&
                (wParam != AVK_UP) &&
                (wParam != AVK_DOWN) &&
                (wParam != AVK_LEFT) &&
                (wParam != AVK_RIGHT))
            {
                return TRUE;
            }
#endif
            return MainMenu_RouteDialogEvt(pMe,eCode,wParam,dwParam);  
#if defined (FEATURE_VERSION_N021)||defined(FEATURE_VERSION_W0216A)
		case EVT_KEY_HELD:
			{
				if(wParam == AVK_INFO)
				{
					(void) ISHELL_PostEvent(pMe->m_pShell,
                                        AEECLSID_CORE_APP,
                                        EVT_USER,
                                        1,
                                        0);
                    ISHELL_CloseApplet(pMe->m_pShell,TRUE);
				}
			}
			break;
#endif
        case EVT_KEY_PRESS:
        case EVT_KEY_RELEASE:
        case EVT_COMMAND:
            return MainMenu_RouteDialogEvt(pMe,eCode,wParam,dwParam); 

        default:
            return MainMenu_RouteDialogEvt(pMe,eCode,wParam,dwParam);
    }
}

void Mainmenu_KeypadLock(boolean block)
{
   gbMainmenuLock = block;
}

/*==============================================================================
函数:
    MainMenu_ProcessState

说明:
    根据 pMe->m_currState, 调用相应的状态处理函数。

参数:
    pMe [in]: 指向MainMenu Applet对象结构的指针。该结构包含小程序的特定信息。

返回值:
    NFSMACTION_CONTINUE: 指示不停状态机。
    NFSMACTION_WAIT: 指示停止状态机。

备注:

==============================================================================*/
NextFSMAction MainMenu_ProcessState(MainMenu *pMe)
{
    NextFSMAction retVal = NFSMACTION_WAIT;
    if (NULL == pMe)
    {
        return retVal;
    }
    switch(pMe->m_currState)
    {
        case MAINST_MAIN:
            return MAINST_MAIN_Handler(pMe);
        case MAINST_FLASHLIGHT:
        	return MAINST_FLASHLIGHT_Handler(pMe);    
        case MAINST_EXIT:
            return MAINST_EXIT_Handler(pMe);  
        default:
            break;

    }
    return retVal;
}

/*==============================================================================
函数:
    MAINST_MAIN_Handler

说明:
    MAINST_MAIN 状态处理函数。

参数:
    pMe [in]: 指向MAINMENU Applet对象结构的指针。该结构包含小程序的特定信息。

返回值:
    NFSMACTION_CONTINUE: 指示不停状态机。
    NFSMACTION_WAIT: 指示停止状态机。

备注:

==============================================================================*/
static NextFSMAction MAINST_MAIN_Handler(MainMenu *pMe)
{
    if (NULL == pMe)
    {
        return NFSMACTION_WAIT;
    }
    switch (pMe->m_eDlgReturn)
    {
        // 进入主界面
        case DLGRET_CREATE:
            {
                MainMenu_ShowDialog(pMe, IDD_MAIN_MENU);
            }
            return NFSMACTION_WAIT;
            
        case DLGRET_FLASHLITHT:
        	{
        		MOVE_TO_STATE(MAINST_FLASHLIGHT)
            	return NFSMACTION_CONTINUE;
        	}    
        case DLGRET_CANCELED:
            MOVE_TO_STATE(MAINST_EXIT)
            return NFSMACTION_CONTINUE;

        default:
            MOVE_TO_STATE(MAINST_EXIT)
            return NFSMACTION_CONTINUE;
    }
}


static NextFSMAction MAINST_FLASHLIGHT_Handler(MainMenu *pMe)
{
	if (NULL == pMe)
    {
        return NFSMACTION_WAIT;
    }
    switch (pMe->m_eDlgReturn)
    {
        // 进入主界面
        case DLGRET_CREATE:
            {
                MainMenu_ShowDialog(pMe, IDD_FLASHLIGHT_SETTING);
            }
            return NFSMACTION_WAIT;
         case DLGRET_CANCELED:         
            MOVE_TO_STATE(MAINST_MAIN)
            return NFSMACTION_CONTINUE;
        default:
            MOVE_TO_STATE(MAINST_EXIT)
            return NFSMACTION_CONTINUE;
    }
}


/*==============================================================================
函数:
    MAINST_EXIT_Handler

说明:
    MAINST_EXIT 状态处理函数。

参数:
    pMe [in]: 指向MAINMENU  Applet对象结构的指针。该结构包含小程序的特定信息。

返回值:
    NFSMACTION_CONTINUE: 指示不停状态机。
    NFSMACTION_WAIT: 指示停止状态机。

备注:

==============================================================================*/
static NextFSMAction MAINST_EXIT_Handler(MainMenu *pMe)
{
    (void) ISHELL_CloseApplet(pMe->m_pShell, FALSE);
    return NFSMACTION_WAIT;
} 

/*==============================================================================
函数:
       MainMenu_ShowDialog

说明:
    函数由状态处理函数用来唤起相应对话框资源ID的对话框。

参数:
    pMe [in]: 指向 MAINMENU Applet 对象结构的指针。该结构包含小程序的特定信息。
    dlgResId [in]: 对话框资源ID，函数根据该ID创建对话框。

返回值:
    none

备注:

==============================================================================*/
void MainMenu_ShowDialog(MainMenu  *pMe,  uint16 dlgResId)
{
    int nRet;

    // 每次最多打开一个对话框
    if (ISHELL_GetActiveDialog(pMe->m_pShell) != NULL)
    {
        // 如果当前已经有对话框被打开，直接返回
        return;
    }
   
    if (NULL != pMe->m_pDisplay)
    {
        AEEDeviceInfo di={0,};
#if defined (FEATURE_VERSION_H19C)
        if(pMe->m_pIAnn != NULL)
        {
            if (dlgResId == IDD_MAIN_MENU)
            {
                IANNUNCIATOR_SetHasTitleText(pMe->m_pIAnn, FALSE);
            }
            else
            {
                IANNUNCIATOR_SetHasTitleText(pMe->m_pIAnn, TRUE);
            }
            IANNUNCIATOR_Redraw(pMe->m_pIAnn);
        }
        (void)IDISPLAY_SetPrefs(pMe->m_pDisplay, "a:1", STRLEN("a:1"));
#else
        if (dlgResId == IDD_MAIN_MENU)
        {
            (void)IDISPLAY_SetPrefs(pMe->m_pDisplay, "a:0", STRLEN("a:0"));
        }
        else
        {
            (void)IDISPLAY_SetPrefs(pMe->m_pDisplay, "a:1", STRLEN("a:1"));
        }
#endif
        ISHELL_GetDeviceInfo(pMe->m_pShell, &di);
        pMe->m_rc.dx = di.cxScreen;
        pMe->m_rc.dy = di.cyScreen;       
        IDISPLAY_SetClipRect(pMe->m_pDisplay, &pMe->m_rc);        
    }
    
    nRet = ISHELL_CreateDialog(pMe->m_pShell,MAINMENU_RES_FILE_LANG,dlgResId,NULL);
    if (nRet != SUCCESS)
    {
    }
}


/*==============================================================================
函数:
       MainMenu_RouteDialogEvt
说明:
       函数将BREW事件路由给当前活动对话框的事件处理函数。

参数:
       pMe [In]: 指向SettingMenu Applet对象结构的指针,该结构包含小程序的特定信息.
       eCode [in]：事件代码。
       wParam [in]：与事件eCode关联的数据。
       dwParam [in]：与事件eCode关联的数据。

返回值:
       TRUE：传入事件被处理。
       FALSE：传入事件没被处理。

备注:

==============================================================================*/
boolean MainMenu_RouteDialogEvt(MainMenu *pMe,
    AEEEvent    eCode,
    uint16  wParam,
    uint32 dwParam
)

{
    if (NULL == pMe)
    {
        return FALSE;
    }

    if (NULL == pMe->m_pActiveIDlg)
    {
        return FALSE;
    }    
    switch( pMe->m_pActivedlgID)
    {
        case IDD_MAIN_MENU:
            return MainMenu_IconMenuHandler(pMe, eCode, wParam, dwParam);

        case IDD_FLASHLIGHT_SETTING:
            return MainMenu_FlashlightMenuHandler(pMe, eCode, wParam,dwParam);
        default:
            return FALSE;
    }
}

/*=============================================================================
FUNCTION:  MainMenu_IconMenuHandler

DESCRIPTION: 12宫格菜单

PARAMETERS:

=============================================================================*/
static boolean MainMenu_IconMenuHandler(MainMenu *pMe, AEEEvent eCode, uint16 wParam, uint32 dwParam)
{
    PARAM_NOT_REF( dwParam)
    switch ( eCode)
    {
        case EVT_DIALOG_INIT:			
            IDIALOG_SetProperties((IDialog *)dwParam, DLG_NOT_REDRAW_AFTER_START);
			MSG_FATAL("MainMenu_IconMenuHandler.........init....",0,0,0);
            return TRUE;

        case EVT_DIALOG_START:			
			AEE_CancelTimer(Main_keypadtimer,pMe);
			AEE_SetTimer(5*1000,Main_keypadtimer,pMe);
            (void) ISHELL_PostEvent( pMe->m_pShell,
                                     AEECLSID_MAIN_MENU,
                                     EVT_USER_REDRAW,
                                     0,
                                     0);
			MSG_FATAL("MainMenu_IconMenuHandler.........EVT_DIALOG_START....",0,0,0);
            return TRUE;

        case EVT_USER_REDRAW:
            // 初始整个背景及全部初始图标
            {
            	
            	MSG_FATAL("MainMenu_IconMenuHandler.........EVT_USER_REDRAW....",0,0,0);
            	DrawMatrix(pMe);
				MSG_FATAL("MainMenu_IconMenuHandler.........EVT_USER_REDRAW..over..",0,0,0);
				//AEE_CancelTimer(Main_loadtimer,pMe);
				//AEE_SetTimer(100,Main_loadtimer,pMe);
            	return TRUE;
            }
            
        case EVT_DIALOG_END:
            {
                ISHELL_CancelTimer(pMe->m_pShell, NULL, (void**)pMe);
            }
            return TRUE;

        case EVT_KEY_HELD:            
        #if defined(FEATURE_TORCH_KEY_INFO)
           if((AVKType)wParam == AVK_INFO)
            {
                boolean TorchOn = FALSE;
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
                ISHELL_CloseApplet(pMe->m_pShell, TRUE); 
                return TRUE;
            }
        #endif
	 #if defined(FEATURE_TORCH_KEY_SELECT) //xxzhen
           if((AVKType)wParam == AVK_SELECT)
            {
                boolean TorchOn = FALSE;
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
                ISHELL_CloseApplet(pMe->m_pShell, TRUE); 
                return TRUE;
            }
        #endif

            return TRUE;
        case EVT_KEY:
            switch( wParam)
            {
                case AVK_CLR:
                     CLOSE_DIALOG(DLGRET_CANCELED)
                    return TRUE;
                     
                case AVK_UP:       
                case AVK_LEFT:
                   if(pMe->m_index>0)
                   {
                   	   pMe->m_index = pMe->m_index-1;
                   }
				   else if(pMe->m_index == 0)
				   {
				   		pMe->m_index = (MAX_MATRIX_ITEMS-1);
				   }
				   MoveCursorTo(pMe, pMe->m_index);
                   return TRUE;

                case AVK_DOWN:     
                case AVK_RIGHT:

                   if(pMe->m_index<(MAX_MATRIX_ITEMS-1))
                   {
                   	   pMe->m_index = pMe->m_index+1;
					  
                   }
				   else if(pMe->m_index == (MAX_MATRIX_ITEMS-1))
				   {
				   		pMe->m_index = 0;
				   }
				   MoveCursorTo(pMe, pMe->m_index);
                   return TRUE;
                    
                case AVK_SELECT:
                case AVK_INFO:
                    {
					   int8 count_main = pMe->m_index;
					   MSG_FATAL("count_main=======%d",count_main,0,0);
					   OEM_SetConfig(CFGI_COUNT_OF_MAIN, &count_main, sizeof(int8));
                       StartApplet(pMe, pMe->m_IconTitle[pMe->m_index]);
                    }
                    return TRUE;
                case AVK_STAR:
                    if(gbMainmenuLock)
                    {
                        OEMKeyguard_SetState(TRUE);
                        ISHELL_CloseApplet(pMe->m_pShell, TRUE); 
                    }
                    return TRUE;
                default:
                    break;
            }

            return TRUE;
            
        default:
            break;
    }

    return FALSE;
}

//Add By zzg 2013_08_20
static boolean MainMenu_FlashlightMenuHandler(MainMenu *pMe, AEEEvent eCode, uint16 wParam, uint32 dwParam)
{
    PARAM_NOT_REF(dwParam)
    IMenuCtl *pMenu = (IMenuCtl*)IDIALOG_GetControl(pMe->m_pActiveIDlg,IDC_FLASHLIGHT_SET);
    AECHAR WTitle[40] = {0};
    if (pMenu == NULL)
    {
        return FALSE;
    }
    if(pMe->m_pIAnn != NULL)
    {
	    IANNUNCIATOR_SetFieldIsActiveEx(pMe->m_pIAnn,FALSE);
    }
     switch (eCode)
    {
        case EVT_DIALOG_INIT:
			(void)ISHELL_LoadResString(pMe->m_pShell,
                                        MAINMENU_RES_FILE_LANG,                                
                                        IDS_MAIN_MENU_TORCH,
                                        WTitle,
                                        sizeof(WTitle));

            if(pMe->m_pIAnn != NULL)
            {
			    IANNUNCIATOR_SetFieldTextEx(pMe->m_pIAnn,WTitle,FALSE);
            }
            IMENUCTL_AddItem(pMenu, MAINMENU_RES_FILE_LANG, IDS_MAIN_MENU_TORCH_ON, IDS_MAIN_MENU_TORCH_ON, NULL, 0);
            IMENUCTL_AddItem(pMenu, MAINMENU_RES_FILE_LANG, IDS_MAIN_MENU_TORCH_OFF, IDS_MAIN_MENU_TORCH_OFF, NULL, 0);
            return TRUE;
            
        case EVT_DIALOG_START:
            {
            	uint16 wItemID;
                boolean Is_on = FALSE;
                IMENUCTL_SetProperties(pMenu, MP_UNDERLINE_TITLE|MP_WRAPSCROLL|MP_TEXT_ALIGN_LEFT_ICON_ALIGN_RIGHT);
                IMENUCTL_SetOemProperties(pMenu, OEMMP_USE_MENU_STYLE);

                IMENUCTL_SetBottomBarType(pMenu,BTBAR_SELECT_BACK);
                OEM_GetConfig(CFGI_FLSHLITHG_STATUS,&Is_on, sizeof(Is_on));
                if(Is_on)
                {
                	wItemID = IDS_MAIN_MENU_TORCH_ON;
                }
                else
                {
                	wItemID = IDS_MAIN_MENU_TORCH_OFF;
                }

                InitMenuIcons(pMenu);
                SetMenuIcon(pMenu, wItemID, TRUE);
                IMENUCTL_SetSel(pMenu, wItemID);
            }
            return TRUE;

        case EVT_DIALOG_END:
            return TRUE;

        case EVT_KEY:
            switch(wParam)
            {
                case AVK_CLR:
                    CLOSE_DIALOG(DLGRET_CANCELED)
                    return TRUE;

                  default:
                    break;
            }
            return TRUE;
#ifdef FEATURE_LCD_TOUCH_ENABLE//andrew add for LCD touch
		case EVT_PEN_UP:
			{
				AEEDeviceInfo devinfo;
				int nBarH ;
				AEERect rc;
				int16 wXPos = (int16)AEE_GET_X(dwParam);
				int16 wYPos = (int16)AEE_GET_Y(dwParam);

				nBarH = GetBottomBarHeight(pMe->m_pDisplay);
        
				MEMSET(&devinfo, 0, sizeof(devinfo));
				ISHELL_GetDeviceInfo(pMe->m_pShell, &devinfo);
				SETAEERECT(&rc, 0, devinfo.cyScreen-nBarH, devinfo.cxScreen, nBarH);

				if(TOUCH_PT_IN_RECT(wXPos,wYPos,rc))
				{
					if(wXPos >= rc.x && wXPos < rc.x + (rc.dx/3) )//左
					{
						boolean rt =  ISHELL_PostEvent(pMe->m_pShell,AEECLSID_APPLICATION,EVT_KEY,AVK_SELECT,0);
						return rt;
					}
					else if(wXPos >= rc.x + (rc.dx/3)   && wXPos < rc.x + (rc.dx/3)*2 )//左
					{
						 boolean rt = ISHELL_PostEvent(pMe->m_pShell,AEECLSID_APPLICATION,EVT_KEY,AVK_INFO,0);
						 return rt;
					}
					else if(wXPos >= rc.x + (rc.dx/3)*2 && wXPos < rc.x + (rc.dx/3)*3 )//左
					{						
						 boolean rt = ISHELL_PostEvent(pMe->m_pShell,AEECLSID_APPLICATION,EVT_KEY,AVK_CLR,0);
						 return rt;
					}
				}

			}
			break;
#endif 
        case EVT_COMMAND:
            {
                boolean bytNewData = 0;

                switch (wParam)
                {
                    case  IDS_MAIN_MENU_TORCH_ON:
                       bytNewData = TRUE;
                       IBACKLIGHT_TurnOnTorch(pMe->m_pBacklight);
                       break;

                    case IDS_MAIN_MENU_TORCH_OFF:
                       bytNewData = FALSE;
                       IBACKLIGHT_TurnOffTorch(pMe->m_pBacklight);
                       break;

                    default:
                       break;

                }
                OEM_SetConfig(CFGI_FLSHLITHG_STATUS,&bytNewData, sizeof(bytNewData));
                InitMenuIcons(pMenu);
                SetMenuIcon(pMenu, wParam, TRUE);
                (void) ISHELL_PostEvent( pMe->m_pShell,
                                         AEECLSID_MAIN_MENU,
                                         EVT_USER_REDRAW,
                                         0,
                                         0);
            }
        default:
        	break;
	}
}
//Add End


static void Main_keypadtimer(void *pUser)
{
    gbMainmenuLock =FALSE;
}
static void Main_loadtimer(void *pUser)
{
	MainMenu	*pMe = (MainMenu *)pUser;
	int i = 0;
	int j = 0;
	for (i = 0; i < MAX_MATRIX_ITEMS; i ++)
    {
        if (pMe->m_pImageIcon[i] == NULL)
        {
            pMe->m_pImageIcon[i] = ISHELL_LoadImage(pMe->m_pShell,
                                                    ICON_ANI[i]);
        }
    }  
    /*
	for(j=0;j<MAX_MATRIX_ITEMS;j++)
	{
		if (pMe->m_pImageSelectEC99[j] == NULL)
        {           
            pMe->m_pImageSelectEC99[j] = ISHELL_LoadImage(pMe->m_pShell, ICON_ANI[j]);
        }
	}
    */
	Main_loadover = TRUE;
	
}

static void MainMenu_DrawBackGround(MainMenu *pMe, AEERect *pRect)
{

    Appscommon_ResetBackground(pMe->m_pDisplay, 
                              pMe->m_pImageBg, 
                              APPSCOMMON_BG_COLOR, 
                              pRect, 
                              0, 
                              0);

}

/*=============================================================================
FUNCTION:  DrawMatrix

DESCRIPTION: // 初始整个背景及全部初始图标

=============================================================================*/
static void DrawMatrix(MainMenu *pMe)
{
    
	BottomBar_e_Type    eBBarType = BTBAR_NONE;
    boolean m_sound_bo_main = FALSE;
    if (NULL == pMe)
    {
        return;
    }
    //draw bg image
    MainMenu_DrawBackGround(pMe, &pMe->m_rc);
	(void) ICONFIG_GetItem(pMe->m_pConfig,
									   CFGI_LANGUAGE_SELECTION,
									   &pMe->language,
									   sizeof(pMe->language));

   /*
    if(pMe->m_pImageSelectEC99[pMe->m_index]!=NULL)
	{
		IIMAGE_Draw(pMe->m_pImageSelectEC99[pMe->m_index], 0, 0);
	}
    */
    if (pMe->m_pImageSelectEC99 != NULL)
    {   
         (void)IIMAGE_Release(pMe->m_pImageSelectEC99);
         pMe->m_pImageSelectEC99 = NULL;
    }
    
    pMe->m_pImageSelectEC99 = ISHELL_LoadImage(pMe->m_pShell, ICON_ANI[pMe->m_index]);      
    if(pMe->m_pImageSelectEC99 != NULL)
    {
        IIMAGE_Draw(pMe->m_pImageSelectEC99, 0, 0);
    }
	
#ifdef FEATURE_SOUND_BO		
   	(void) ICONFIG_GetItem(pMe->m_pConfig,
                                 CFGI_SOUND_BO_MAIN,
                                 &m_sound_bo_main,
                                 sizeof(boolean));

	if(m_sound_bo_main)
	{

		 /*
		nv_item_type	SimChoice;
		(void)OEMNV_Get(NV_SIM_SELECT_I,&SimChoice);
		if(SimChoice.sim_select != 1)
        */   
        if (GetMp3PlayerStatus() == MP3STATUS_NONE)
		{
			MainMenu_PlayShutterSound(pMe,pMe->m_index);
		}
	}
#endif
	IDISPLAY_UpdateEx(pMe->m_pDisplay, TRUE);   
}


/*=============================================================================
FUNCTION:  MoveCursorTo

DESCRIPTION: 

PARAMETERS:

RETURN VALUE:

COMMENTS:

SIDE EFFECTS:

SEE ALSO:

=============================================================================*/
static void MoveCursorTo(MainMenu *pMe, int index)
{
	
	boolean m_sound_bo_main = FALSE;
	//draw bg image
	if (NULL == pMe)
    {
        return;
    }
	(void) ICONFIG_GetItem(pMe->m_pConfig,
									   CFGI_LANGUAGE_SELECTION,
									   &pMe->language,
									   sizeof(pMe->language));

#ifdef FEATURE_SOUND_BO    
	(void) ICONFIG_GetItem(pMe->m_pConfig,
                                 CFGI_SOUND_BO_MAIN,
                                 &m_sound_bo_main,
                                 sizeof(boolean));
	if(m_sound_bo_main)
	{
		 /*
		nv_item_type	SimChoice;
		(void)OEMNV_Get(NV_SIM_SELECT_I,&SimChoice);
		if(SimChoice.sim_select != 1)
        */   
        if (GetMp3PlayerStatus() == MP3STATUS_NONE)
		{
			MainMenu_PlayShutterSound(pMe,pMe->m_index);
		}
	}
#endif
    MainMenu_DrawBackGround(pMe, &pMe->m_rc);

    if (pMe->m_pImageSelectEC99 != NULL)
    {   
         (void)IIMAGE_Release(pMe->m_pImageSelectEC99);
         pMe->m_pImageSelectEC99 = NULL;
    }

    pMe->m_pImageSelectEC99 = ISHELL_LoadImage(pMe->m_pShell, ICON_ANI[pMe->m_index]);      
    if(pMe->m_pImageSelectEC99 != NULL)
    {
        IIMAGE_Draw(pMe->m_pImageSelectEC99, 0, 0);
    }
    
	IDISPLAY_UpdateEx(pMe->m_pDisplay, TRUE);
}
/*=============================================================================
FUNCTION:  SetCursor

DESCRIPTION: 

PARAMETERS:

RETURN VALUE:

COMMENTS:

SIDE EFFECTS:

SEE ALSO:

=============================================================================*/
static void SetCursor(MainMenu *pMe, int index)
{
    pMe->m_index        = index;   
    
    if(pMe->m_pAnimate != NULL)
    {
        IImage_Release(pMe->m_pAnimate);
        pMe->m_pAnimate  = NULL;
    }
}

/*=============================================================================
FUNCTION:  StartApplet

DESCRIPTION:  启动applet

PARAMETERS:  如果APPLET 有变动，只需改动次函数

=============================================================================*/
static int StartApplet(MainMenu *pMe, int i)
{
    int Result = EUNSUPPORTED;
	//uint32  dwTotal = 0;
	//uint32 free = 0;
	//GETFSFREE(&dwTotal);
	//free = GETRAMFREE(NULL,NULL);
	//MSG_FATAL("dwTotal======%d,free===%d",dwTotal,free,0);

    
#ifdef FEATURE_VERSION_H19C  
    if(pMe->m_pIAnn != NULL)
    {
        IANNUNCIATOR_SetHasTitleText(pMe->m_pIAnn, TRUE);
    }
#endif
		#ifdef FEATURE_VERSION_K202_LM129C //xxzhen
		  if(pMe->m_pIAnn != NULL)
          {
              IANNUNCIATOR_SetHasTitleText(pMe->m_pIAnn, TRUE);
          }
		#endif

    switch(i)
	{
#ifdef FEATURE_VERSION_W208S
	case IDS_MAIN_MENU_MAP:
	{
		char	buf[12];
		
		OEM_SetBROWSER_ADSAccount();	//Add By zzg 2011_07_08
		
    	MSG_FATAL("AEECLSID_BRW_APP...........START",0,0,0);
    	start_info.appid_fx = APP_ORIGINATOR_BROWSER;
        start_info.subtype_keycode = APP_BROWSER_START_URL;
        start_info.par_keychar = 0;

		start_info.fInfo.url_info.title = "map";
		start_info.fInfo.url_info.url = "http://wap.movidamovil.com/venezuela/?debug=true";		
		
        //start_info.fInfo.url_info.title = "163";
        //start_info.fInfo.url_info.url = "http://wap.163.com";

		
        SPRINTF(buf, "%p", &start_info);
		Result = ISHELL_StartAppletArgs(pMe->m_pShell,AEECLSID_NF3,buf);
        //Result = ISHELL_StartApplet(pMe->m_pShell, AEECLSID_BRW_APP);
        MSG_FATAL("AEECLSID_BRW_APP...........Result=%d",Result,0,0);		
		
		
		break;
	}
	case IDS_MAIN_MENU_NEO_APP:
	{
        Result = ISHELL_StartApplet(pMe->m_pShell, AEECLSID_APPMANAGER);
		break;
	}
	case IDS_MAIN_MENU_NEO_NAV:
	{
		char	buf[12];
		
		OEM_SetBROWSER_ADSAccount();	//Add By zzg 2011_07_08
		
    	MSG_FATAL("AEECLSID_BRW_APP...........START",0,0,0);
    	start_info.appid_fx = APP_ORIGINATOR_BROWSER;
        start_info.subtype_keycode = APP_BROWSER_START_MAINMENU;
        start_info.par_keychar = 0;
        //start_info.fInfo.url_info.title = "163";
        //start_info.fInfo.url_info.url = "http://www.163.com";
        SPRINTF(buf, "%p", &start_info);
		Result = ISHELL_StartAppletArgs(pMe->m_pShell,AEECLSID_NF3,buf);
        //Result = ISHELL_StartApplet(pMe->m_pShell, AEECLSID_BRW_APP);
        MSG_FATAL("AEECLSID_BRW_APP...........Result=%d",Result,0,0);		
		
		break;
	}
#endif

    case IDS_MAIN_MENU_FILEMGR:
    case IDS_MAIN_MENU_GALLERY:
	case IDS_MAIN_MENU_MEDIAGALLERY_C337:	
    case IDS_MAIN_MENU_MEDIAGALLERY:
        Result = ISHELL_StartApplet(pMe->m_pShell, AEECLSID_MEDIAGALLERY);
        break;
        
    case IDS_MAIN_MENU_CONTACT:
	case IDS_PHONEBOOK:
    case IDS_MAIN_MENU_CONTACTS:
        {
            IContactApp *ca = NULL;
            Result = ISHELL_CreateInstance(pMe->m_pShell,AEECLSID_APP_CONTACT, (void**)&ca);
            if(SUCCESS == Result)
            {
                ICONTAPP_MainMenu(ca);
                IContactApp_Release(ca);
            }
        }
        break;
    case IDS_MAIN_MENU_INTEXZONE: 
#ifdef FEATURE_VERSION_C316
		Result = ISHELL_StartApplet(pMe->m_pShell, AEECLSID_INDEX_ZONE);
		break;
#endif
    case IDS_MAIN_MENU_SMARTFRENACCESS:
    case IDS_MAIN_MENU_UTK:
        Result = ISHELL_StartApplet(pMe->m_pShell, AEECLSID_APP_UTK);
        break;
        
    case IDS_MAIN_MENU_TOOLS:
	case IDS_MAIN_MENU_ORGANIZER:	
    case IDS_MAIN_MENU_APPLICATION:
        Result = ISHELL_StartApplet(pMe->m_pShell, AEECLSID_APPLICATION);
        break;
		
    case IDS_MAIN_MENU_CALL_LOGS:
	case IDS_MAIN_MENU_RECENTCALLS_C337:	
    case IDS_MAIN_MENU_RECENTCALLS:
        Result = ISHELL_StartApplet(pMe->m_pShell, AEECLSID_APP_RECENTCALL);
        break;
        
    case IDS_MAIN_MENU_SMS:
    case IDS_MAIN_MENU_MESSAGES:
        Result = ISHELL_StartApplet(pMe->m_pShell, AEECLSID_WMSAPP);
        break;
        
    case IDS_MAIN_MENU_MULTIMEDIA:
        Result = ISHELL_StartApplet(pMe->m_pShell, AEECLSID_MULTIMEDIA_LIST);
        break;
        
    case IDS_MAIN_MENU_STATIC_APPLICATION:
#if defined(FEATURE_APP_MANAGER)
#ifdef FEATURE_OEMOMH
        if(ISHELL_SendEvent(pMe->m_pShell,AEECLSID_DIALER,EVT_OMH_PROMPT,0,0))
        {
            break;
        }
#endif

        Result = ISHELL_StartApplet(pMe->m_pShell, AEECLSID_APPMANAGER);
#else
        Result = ISHELL_StartApplet(pMe->m_pShell, AEECLSID_STATIC_APP);
#endif
    break;
        
    case IDS_MAIN_MENU_CALENDAR:
    case IDS_MAIN_MENU_SCHEDULER:
        Result = ISHELL_StartApplet(pMe->m_pShell, AEECLSID_SCHEDULEAPP);
        break;
        
    case IDS_MAIN_MENU_SETTINGS:
        Result = ISHELL_StartApplet(pMe->m_pShell, AEECLSID_APP_SETTINGMENU);
        break;

	//Add By zzg 2012_11_08 for C337 
	case IDS_MAIN_MENU_SERVICES:		
	    #if defined (FEATURE_VERSION_C337) || defined(FEATURE_VERSION_IC241A_MMX)
        #if defined (FEATURE_VERSION_IN50A)
        Result = SetBrowserArr_Main(pMe->m_pShell,(char*)"http://www.google.com.hk/"); 
        #else
        Result = SetBrowserArr_Main(pMe->m_pShell,(char*)"http://mimicromax.com"); //ISHELL_StartAppletArgs(pMe->m_pShell, AEECLSID_UCWEB, (char*)"call_ucweb:setmainpageurl:http://mimicromax.com");
        #endif        
        #else
        Result = SetBrowserArr_Main(pMe->m_pShell,NULL);//ISHELL_StartApplet(pMe->m_pShell, AEECLSID_UCWEB);
        #endif
		break;
	case IDS_MAIN_MENU_MSTORE:
#if defined (FEATURE_VERSION_C337) || defined(FEATURE_VERSION_IC241A_MMX)
#if defined (FEATURE_VERSION_IN50A)
        Result = SetBrowserArr_Main(pMe->m_pShell,(char*)"http://www.google.com.hk/"); 
#else
        Result = SetBrowserArr_Main(pMe->m_pShell,(char*)"http://mimicromax.com"); //ISHELL_StartAppletArgs(pMe->m_pShell, AEECLSID_UCWEB, (char*)"call_ucweb:setmainpageurl:http://mimicromax.com");
#endif        
#else
        Result = SetBrowserArr_Main(pMe->m_pShell,NULL);//ISHELL_StartAppletArgs(pMe->m_pShell, AEECLSID_UCWEB, (char*)"call_ucweb:setmainpageurl:http://mimicromax.com");
#endif
		break;
	case IDS_MAIN_MENU_MZONE:
		Result = ISHELL_StartApplet(pMe->m_pShell, AEECLSID_MiZone);
		break;
	//Add End
	
	case IDS_MAIN_MENU_USER_PROFILE:	
    case IDS_MAIN_MENU_USERPROFILE:
        Result = ISHELL_StartApplet(pMe->m_pShell, AEECLSID_APP_SOUNDMENU);
        break;
        
    case IDS_MAIN_MENU_GAMES:
        Result = ISHELL_StartApplet(pMe->m_pShell, AEECLSID_GAME);
        break;
		
	#ifdef FEATURE_VERSION_C316
    case IDS_MAIN_MENU_SERVICES_C316:  
		Result = ISHELL_StartApplet(pMe->m_pShell, AEECLSID_SERVICE);
        break;
	#endif	
	
    case IDS_MAIN_WAPBROWSER:
		#ifdef FEATURE_VERSION_K202_LM129C
		  IANNUNCIATOR_SetHasTitleText(pMe->m_pIAnn,FALSE); //xxzhen
		   IANNUNCIATOR_Redraw(pMe->m_pIAnn);
		#endif

        #if defined (FEATURE_VERSION_C337) || defined(FEATURE_VERSION_IC241A_MMX)
        #if defined (FEATURE_VERSION_IN50A)
        Result = SetBrowserArr_Main(pMe->m_pShell,(char*)"http://www.google.com.hk/"); 
        #else
        Result = SetBrowserArr_Main(pMe->m_pShell,(char*)"http://mimicromax.com"); //ISHELL_StartAppletArgs(pMe->m_pShell, AEECLSID_UCWEB, (char*)"call_ucweb:setmainpageurl:http://mimicromax.com");
        #endif        
        #else
        Result = SetBrowserArr_Main(pMe->m_pShell,(char*)""); //ISHELL_StartApplet(pMe->m_pShell, AEECLSID_UCWEB);
        #endif
        break;
    
    case IDS_MAIN_MENU_CALCULATOR: 
		 Result = ISHELL_StartApplet(pMe->m_pShell, AEECLSID_CALCAPP);
        break;
        
    case IDS_MAIN_MENU_FMRADIO:
        Result = ISHELL_StartApplet(pMe->m_pShell, AEECLSID_APP_FMRADIO);
        break;
        
    case IDS_MAIN_MENU_MUSICPLAYER:
        Result = ISHELL_StartApplet(pMe->m_pShell, AEECLSID_APP_MUSICPLAYER);
        break;
        
    case IDS_MAIN_MENU_SMARTWORLD:
    case IDS_MAIN_MENU_BLIVE:
#ifdef FEATURE_SMARTFREN_STATIC_BREW_APP
		Result = ISHELL_StartApplet(pMe->m_pShell, AEECLSID_STATIC_APP);
#else
#ifdef FEATURE_OEMOMH
        if(ISHELL_SendEvent(pMe->m_pShell,AEECLSID_DIALER,EVT_OMH_PROMPT,0,0))
        {
            break;
        }
#endif
        Result = ISHELL_StartApplet(pMe->m_pShell, AEECLSID_APPMANAGER);
#endif  
        break;
    
    case IDS_MAIN_MENU_FRENDUO:
#if defined(FEATURE_VERSION_M8) || defined(FEATURE_VERSION_M8P) || defined (FEATURE_VERSION_M8021)
		Result = ISHELL_StartApplet(pMe->m_pShell, AEECLSID_FRENDUO);
#endif
        break;
    
    case IDS_MAIN_MENU_TIMER:
        Result = ISHELL_StartApplet(pMe->m_pShell, AEECLSID_APPTIMER);
        break;
        
    case IDS_MAIN_MENU_CAMERA:
	{		
		Result = ISHELL_StartApplet(pMe->m_pShell, AEECLSID_APP_CAMERA);
		break;
	}  
    
    case IDS_MAIN_MENU_TORCH:
    {		
		//Result = ISHELL_StartApplet(pMe->m_pShell, AEECLSID_TORCH);
		CLOSE_DIALOG(DLGRET_FLASHLITHT)
		break;
	}    
    
    default:
        break;

    }
    return Result;
}

extern char charsvc_p_name[UIM_CDMA_HOME_SERVICE_SIZE+1];
int SetBrowserArr_Main(IShell *pShell ,char *purl)
{
	int Result = EUNSUPPORTED;
	char urlCan[1024] = {0};
    
	DBGPRINTF("svc_p_name %s %d",charsvc_p_name,charsvc_p_name[0],0);
	OEM_SetUCBROWSER_ADSAccount();
    
	if(purl && STRLEN(purl)>1)
	{
        SPRINTF(urlCan,"call_ucweb:setexternurl:%s\2\3",purl);
		STRCAT(urlCan,"useragent:BREW-Applet/0x20068888 (BREW/3.1.5.20; DeviceId: 8976509865757e; Lang: hi; Profile/MIDP-2.0_Configuration/CLDC-1.1) ucweb-squid\2\3");
	}
	else
	{
		STRCPY(urlCan,"useragent:BREW-Applet/0x20068888 (BREW/3.1.5.20; DeviceId: 8976509865757e; Lang: hi; Profile/MIDP-2.0_Configuration/CLDC-1.1) ucweb-squid\2\3");
	}
    
	if(STRISTR (charsvc_p_name,"mts"))
	{
		MSG_FATAL("mst................",0,0,0);
        STRCAT(urlCan,"access_point:proxy_is:10.50.5.140:8080");
	}
	else if(STRISTR (charsvc_p_name,"tata"))
	{
        MSG_FATAL("tata................",0,0,0);
        STRCAT(urlCan,"access_point:proxy_is:172.23.252.15:9401");
	}
	else if(STRISTR (charsvc_p_name,"reliance"))
	{
        MSG_FATAL("reliance................",0,0,0);
        STRCAT(urlCan,"access_point:proxy_is:http://wapgw.ricinfo.com:8080");
	}
	else if(STRISTR (charsvc_p_name,"vmi"))
	{
        MSG_FATAL("vmi................",0,0,0);
        STRCAT(urlCan,"access_point:proxy_is:172.23.142.15:9401");
	}
	
	DBGPRINTF("urlCan==%s", urlCan);
    if(urlCan[0])
    {
	    Result = ISHELL_StartAppletArgs(pShell, AEECLSID_UCWEB, (char*)urlCan);
    }
    else
    {
        Result = ISHELL_StartApplet(pShell, AEECLSID_UCWEB);
    }

	return Result;	
}
static void MainMenu_PlayShutterSound(MainMenu *pMe,int key)
{

    AEEMediaCmdNotify cmd;
	int temp = 0;
	char music_name[256] = {0};
    if(pMe->m_pMedia)
	{
		IMEDIA_Stop(pMe->m_pMedia);
		IMEDIA_Release(pMe->m_pMedia);
		pMe->m_pMedia = NULL;
	}
    // 如果pMe->m_pMedia接口为空，创建接口
    if(!pMe->m_pMedia)
    {
        AEEMediaData      md;
       
        if(!pMe)
           return;
        md.clsData = MMD_FILE_NAME;  		   
		md.pData = (void *)MAIN_SOUND_NAME[key];
        md.dwSize = 0;
       
        (void)AEEMediaUtil_CreateMedia(pMe->m_pShell, &md, &pMe->m_pMedia);
    }
   
    if(pMe->m_pMedia)
    {        
        IMEDIA_SetVolume(pMe->m_pMedia, AEE_MAX_VOLUME*3/5); //max volum is 100
     
        if(IMEDIA_RegisterNotify(pMe->m_pMedia, MainMenu_MediaNotify, pMe) != SUCCESS)
        {
            cmd.nCmd    = MM_CMD_PLAY;
            cmd.nStatus = MM_STATUS_DONE;
            MainMenu_MediaNotify((void *)pMe, &cmd);
            return;
        }
        MSG_FATAL("MainMenu_PlayShutterSound,IMEDIA_Play....",0,0,0);
        if(IMEDIA_Play(pMe->m_pMedia) != SUCCESS)
        {
            cmd.nCmd    = MM_CMD_PLAY;
            cmd.nStatus = MM_STATUS_DONE;
            MainMenu_MediaNotify((void *)pMe, &cmd);
            return;
        }
    }
    else
    {
        cmd.nCmd    = MM_CMD_PLAY;
        cmd.nStatus = MM_STATUS_DONE;
        MainMenu_MediaNotify((void *)pMe, &cmd);
    }  
}

static void MainMenu_MediaNotify(void *pUser, AEEMediaCmdNotify *pCmdNotify)
{
    MainMenu *pMe = (MainMenu *)pUser;

    if(!pMe || !pCmdNotify)
        return;

    if(pCmdNotify->nCmd == MM_CMD_PLAY)  // IMEDIA_Play events
    {
        switch (pCmdNotify->nStatus)
        {
            case MM_STATUS_ABORT:            
                break;

            case MM_STATUS_DONE:    // playback done
				if(pMe->m_pMedia)
				{
					IMEDIA_Release(pMe->m_pMedia);
					pMe->m_pMedia = NULL;
				}
                break;
            default:
                break;
        }
    }
}
#elif defined (FEATURE_VERSION_K232_X3)
static boolean Main_loadover = FALSE;
#if 0 //def FEATURE_SOUND_BO
static char* MAIN_SOUND_NAME[] =
{
	MUSIC_PATH1,
	MUSIC_PATH2,
	MUSIC_PATH3,
	MUSIC_PATH4,
	MUSIC_PATH5,
	MUSIC_PATH6,
	MUSIC_PATH7,
};
#endif
#define PARAM_NOT_REF(x)
/*==============================================================================

                              本地函数声明

==============================================================================*/
static MainMenuMod gMainMenuMod;
static const VTBL( IModule) gModFuncs =
{
    MainMenuMod_AddRef,
    MainMenuMod_Release,
    MainMenuMod_CreateInstance,
    MainMenuMod_FreeResources
};
#if 0 //def FEATURE_SOUND_BO
static void MainMenu_PlayShutterSound(MainMenu *pMe,int key);
static void MainMenu_MediaNotify(void *pUser, AEEMediaCmdNotify *pCmdNotify);
#endif
static uint32  MainMenu_AddRef( IMainMenu *pi);

static uint32  MainMenu_Release ( IMainMenu *pi);

static boolean MainMenu_HandleEvent( IMainMenu *pi,
    AEEEvent eCode,
    uint16  wParam,
    uint32 dwParam
);

static MainMenu gMainMenu={0};

static const VTBL( IMainMenu) gMainMenuMethods =
{
    MainMenu_AddRef,
    MainMenu_Release,
    MainMenu_HandleEvent
};

int MainMenuMod_Load( IShell *pIShell, void *ph, IModule **ppMod);

int MainMenuMod_New( int16 nSize,
    IShell              *pIShell,
    void                *ph,
    IModule             **ppMod,
    PFNMODCREATEINST    pfnMC,
    PFNFREEMODDATA      pfnMF
);

static int MainMenu_New( IShell *ps, IModule *pIModule, IMainMenu **ppObj);

static int CMainMenu_InitAppData(MainMenu *pMe);

static void CMainMenu_FreeAppData(MainMenu *pMe);

static void MainMenu_RunFSM(MainMenu *pMe);

static void MainMenu_DrawBackGround(MainMenu *pMe, AEERect *pRect);

// 初始整个背景及全部初始图标

int SetBrowserArr_Main(IShell *pShell ,char *purl);

static void DrawMatrix(MainMenu *pMe);

static void MoveCursorTo(MainMenu *pMe, int index);

static void SetCursor(MainMenu *pMe, int index);

static int StartApplet(MainMenu *pMe, int i);

boolean MainMenu_RouteDialogEvt(MainMenu *pMe,
    AEEEvent    eCode,
    uint16  wParam,
    uint32 dwParam
);

NextFSMAction MainMenu_ProcessState(MainMenu *pMe);

void MainMenu_ShowDialog(MainMenu  *pMe,  uint16 dlgResId);

static boolean  gbMainmenuLock = FALSE;

static void Main_keypadtimer(void *pUser);
static void Main_loadtimer(void *pUser);


// MAINST_MAIN 状态处理函数
static NextFSMAction MAINST_MAIN_Handler(MainMenu *pMe);
static NextFSMAction MAINST_FLASHLIGHT_Handler(MainMenu *pMe);        //Add By zzg 2013_08_20
//MAINST_EXIT  状态处理函数
static NextFSMAction MAINST_EXIT_Handler(MainMenu *pMe);
static boolean  MainMenu_IconMenuHandler(MainMenu *pMe, AEEEvent eCode, uint16 wParam, uint32 dwParam);
static boolean  MainMenu_FlashlightMenuHandler(MainMenu *pMe, AEEEvent eCode, uint16 wParam, uint32 dwParam); //Add By zzg 2013_08_20

/*==============================================================================
                              
                              本地（静态）数据
                              
==============================================================================*/

static char* ICON_ANI[] =
{
    ICON1_ANI,
    ICON2_ANI,
    ICON3_ANI,
    ICON4_ANI,
    ICON5_ANI,
    ICON6_ANI,
    ICON7_ANI,
    ICON8_ANI,
    ICON9_ANI,
};

static char* ICON_ANI_cn[] =
{
    ICON1_ANI_cn,
    ICON2_ANI_cn,
    ICON3_ANI_cn,
    ICON4_ANI_cn,
    ICON5_ANI_cn,
    ICON6_ANI_cn,
    ICON7_ANI_cn,
    ICON8_ANI_cn,
    ICON9_ANI_cn,
};

static char* ICON_ANI_en[] =
{
    ICON1_ANI_en,
    ICON2_ANI_en,
    ICON3_ANI_en,
    ICON4_ANI_en,
    ICON5_ANI_en,
    ICON6_ANI_en,
    ICON7_ANI_en,
    ICON8_ANI_en,
    ICON9_ANI_en,
};


#if 0
static char* MUSIC_PATH[] =
{
	MUSIC_PATH1,
	MUSIC_PATH2,
	MUSIC_PATH3,
	MUSIC_PATH4,
	MUSIC_PATH5,
	MUSIC_PATH6,
	MUSIC_PATH7,
};
#endif
/*=============================================================================
FUNCTION:  MainMenuMod_Load

DESCRIPTION: 

PARAMETERS:

RETURN VALUE:

COMMENTS:

SIDE EFFECTS:

SEE ALSO:

=============================================================================*/
int MainMenuMod_Load( IShell *pIShell, void *ph, IModule **ppMod)
{	
    return MainMenuMod_New( sizeof( MainMenuMod),
                    pIShell,
                    ph,
                    ppMod,
                    NULL,
                    NULL);
}

/*=============================================================================
FUNCTION:  MainMenuMod_New

DESCRIPTION: 

PARAMETERS:

RETURN VALUE:

COMMENTS:

SIDE EFFECTS:

SEE ALSO:

=============================================================================*/
int  MainMenuMod_New( int16 nSize,
    IShell   *pIShell,
    void     *ph,
    IModule  **ppMod,
    PFNMODCREATEINST pfnMC,
    PFNFREEMODDATA pfnMF
)
{
#if !defined( AEE_SIMULATOR)
    PARAM_NOT_REF( nSize)
    PARAM_NOT_REF( pfnMC)
    PARAM_NOT_REF( pfnMF)
    PARAM_NOT_REF( ph)
#endif

    if( NULL == ppMod)
    {
        return EFAILED;
    }

    MEMSET( &gMainMenuMod, 0, sizeof( MainMenuMod));

    INIT_VTBL( &gMainMenuMod, IModule, gModFuncs);
    gMainMenuMod.referenceCounter = 1;
    *ppMod = ( IModule *)&gMainMenuMod;
    return AEE_SUCCESS;
}

/*=============================================================================
FUNCTION:  MainMenuMod_AddRef

DESCRIPTION: 

PARAMETERS:

RETURN VALUE:

COMMENTS:

SIDE EFFECTS:

SEE ALSO:

=============================================================================*/
static uint32 MainMenuMod_AddRef( IModule *po)
{
    return( ++( ( MainMenuMod *)po)->referenceCounter);
}

/*=============================================================================
FUNCTION:  MainMenuMod_Release

DESCRIPTION: 

PARAMETERS:

RETURN VALUE:

COMMENTS:

SIDE EFFECTS:

SEE ALSO:

=============================================================================*/
static uint32 MainMenuMod_Release( IModule *po)
{
    if ( ( ( MainMenuMod *) po)->referenceCounter == 0)
    {
        return 0;
    }

    return( --( ( MainMenuMod *)po)->referenceCounter);
}

/*=============================================================================
FUNCTION:  MainMenuMod_CreateInstance

DESCRIPTION: 

PARAMETERS:

RETURN VALUE:

COMMENTS:

SIDE EFFECTS:

SEE ALSO:

=============================================================================*/
static int  MainMenuMod_CreateInstance( IModule *po,
    IShell *pIShell,
    AEECLSID ClsId,
    void **ppObj
)
{
    *ppObj = NULL;

    if( ClsId != AEECLSID_MAIN_MENU || MainMenu_New( pIShell, po, ( IMainMenu**)ppObj) != SUCCESS)
    {
        return EFAILED;
    }

    return SUCCESS;
}

/*=============================================================================
FUNCTION:  MainMenuMod_FreeResources

DESCRIPTION: 

PARAMETERS:

RETURN VALUE:

COMMENTS:

SIDE EFFECTS:

SEE ALSO:

=============================================================================*/
static void MainMenuMod_FreeResources( IModule *po, IHeap *ph, IFileMgr *pfm)
{
}

/*=============================================================================
FUNCTION:  MainMenu_New

DESCRIPTION: 

PARAMETERS:

RETURN VALUE:

COMMENTS:

SIDE EFFECTS:

SEE ALSO:

=============================================================================*/
static int MainMenu_New( IShell *ps, IModule *pIModule, IMainMenu **ppObj)
{
    int retVal = SUCCESS;
    if( 0 == gMainMenu.referenceCounter)
    {

        if( pIModule == NULL || ps == NULL)
        {
            return EFAILED;
        }

        MEMSET(&gMainMenu, 0, sizeof(gMainMenu));
        INIT_VTBL(&gMainMenu, IMainMenu, gMainMenuMethods);

       
        gMainMenu.m_pShell     = ps;
        gMainMenu.m_pModule    = pIModule;
        retVal = CMainMenu_InitAppData(&gMainMenu);
        if(retVal != SUCCESS)
        {
            CMainMenu_FreeAppData((MainMenu*)&gMainMenu);
            return retVal;
        }
        (void) ISHELL_AddRef( ps);
        (void) IMODULE_AddRef( pIModule);
        gMainMenu.referenceCounter  = 0;

        
    }

    ++ gMainMenu.referenceCounter;
    *ppObj = ( IMainMenu*)&gMainMenu;
    
    return retVal;
}

/*==============================================================================
函数:
    CMainMenuApp_InitAppData

说明:
    初始化 MainMenu Applet 结构数据缺省值。

参数:
    pMe [in]: 指向 MainMenu Applet 对象结构的指针。该结构包含小程序的特定信息。

返回值:
    EFAILED: 初始化失败。
    SUCCESS: 初始化成功。

备注:

==============================================================================*/
static int CMainMenu_InitAppData(MainMenu *pMe)
{	
	int8 count_main = 1;
    int i = 0;
	pMe->language = 0;

    pMe->m_pMedia = NULL;
    
    if (NULL == pMe)
    {
        return EFAILED;
    }
	pMe->m_bChangeL =FALSE;
	OEM_GetConfig(CFGI_COUNT_OF_MAIN, &count_main, sizeof(int8));
	MSG_FATAL("CMainMenu_InitAppData.............=%d",count_main,0,0);
	 
    pMe->m_index = 6;// count_main;
    
	Main_loadover = FALSE;

    // 初始化菜单Title
	pMe->m_IconTitle[0]     = IDS_MAIN_MENU_RECENTCALLS;          
    pMe->m_IconTitle[1]     = IDS_MAIN_MENU_MULTIMEDIA;
    pMe->m_IconTitle[2]     = IDS_MAIN_MENU_CONTACTS;      
    pMe->m_IconTitle[3]     = IDS_MAIN_MENU_SETTINGS;   
    pMe->m_IconTitle[4]     = IDS_MAIN_MENU_UTK;       
    pMe->m_IconTitle[5]     = IDS_MAIN_MENU_TOOLS;
    pMe->m_IconTitle[6]     = IDS_MAIN_MENU_MESSAGES;   
	pMe->m_IconTitle[7]     = IDS_MAIN_MENU_CAMERA; 
	pMe->m_IconTitle[8]     = IDS_MAIN_MENU_USERPROFILE; 
    
    
    //pMe->m_pImageBg = ISHELL_LoadImage(pMe->m_pShell, ICON_ANI_BG);
    
    // 接口创建及相关初始化
    if (ISHELL_CreateInstance(pMe->m_pShell, AEECLSID_CONFIG,
            (void **)&pMe->m_pConfig) != SUCCESS)
    {    	
        return EFAILED;
    }

    if( ISHELL_CreateInstance( pMe->m_pShell, AEECLSID_BACKLIGHT, (void **)&pMe->m_pBacklight)!=AEE_SUCCESS)
    {
        return FALSE;
    }
     
    if (ISHELL_CreateInstance(pMe->m_pShell, AEECLSID_DISPLAY, 
            (void **) &pMe->m_pDisplay) != SUCCESS)
    {    	
        return EFAILED;
    }
	if (AEE_SUCCESS != ISHELL_CreateInstance(pMe->m_pShell,
                                            AEECLSID_ANNUNCIATOR,
                                            (void **)&pMe->m_pIAnn))
    {    	
        return EFAILED;
    }
  
	(void) ICONFIG_GetItem(pMe->m_pConfig,
									   CFGI_LANGUAGE_SELECTION,
									   &pMe->language,
									   sizeof(pMe->language));
	
	MSG_FATAL("CMainMenu_InitAppData.............over.....",0,0,0);
    return SUCCESS;
}

/*==============================================================================
函数:
    CMainMenu_FreeAppData

说明:
    释放 MainMenu Applet 相关资源。

参数:
    pMe [in]: 指向MainMenu Applet 对象结构的指针。该结构包含小程序的特定信息。

返回值:
    none

备注:

==============================================================================*/
static void CMainMenu_FreeAppData(MainMenu *pMe)
{
    if (NULL == pMe)
    {
        return;
    }

    pMe->m_eAppStatus = MAINMENU_STOP;
    if (NULL != pMe->m_pConfig)
    {
        ICONFIG_Release(pMe->m_pConfig);
        pMe->m_pConfig = NULL;
    }    
    
    if (pMe->m_pDisplay != NULL)
    {
        (void) IDISPLAY_Release(pMe->m_pDisplay);
        pMe->m_pDisplay = NULL;
    }
	if (pMe->m_pIAnn)
    {
        IANNUNCIATOR_Release(pMe->m_pIAnn);
		pMe->m_pIAnn = NULL;
    }

    if(pMe->m_pBacklight)
    {
        IBACKLIGHT_Release(pMe->m_pBacklight);
        pMe->m_pBacklight=NULL;
    }
	if(pMe->m_pMedia)
    {
        IMEDIA_Release(pMe->m_pMedia);
        pMe->m_pMedia = NULL;
    }
    //释放图片资源
    Main_loadover = FALSE;
    {
         int i;
         int j;
         if (pMe->m_pImageBg !=NULL)
         {
             (void) IIMAGE_Release(pMe->m_pImageBg);
             pMe->m_pImageBg = NULL;
         }
         #if 0
         for(i=0;i<MAX_MATRIX_ITEMS;i++)
         {
             if(pMe->m_pImageIcon[i] != NULL)
             {
                 (void)IIMAGE_Release(pMe->m_pImageIcon[i]);
                 pMe->m_pImageIcon[i] = NULL;
             }
         } 
		 #endif
         if(pMe->m_pImageSelectEC99 != NULL)
             {   
                 (void)IIMAGE_Release(pMe->m_pImageSelectEC99);
                 pMe->m_pImageSelectEC99 = NULL;
             }
    }  
}

/*=============================================================================
FUNCTION:  MainMenu_AddRef

DESCRIPTION: 

PARAMETERS:

RETURN VALUE:

COMMENTS:

SIDE EFFECTS:

SEE ALSO:

=============================================================================*/
static uint32  MainMenu_AddRef( IMainMenu *pi)
{
    return ( ++gMainMenu.referenceCounter);
}

/*=============================================================================
FUNCTION:  MainMenu_Release

DESCRIPTION: 

PARAMETERS:

RETURN VALUE:

COMMENTS:

SIDE EFFECTS:

SEE ALSO:

=============================================================================*/
static uint32  MainMenu_Release( IMainMenu *pi)
{
    register MainMenu *pMe = (MainMenu*)pi;
    if (pMe->referenceCounter == 0)
    {
        return 0;
    }
    
    if( --pMe->referenceCounter)
    {
        return pMe->referenceCounter;
    }


    CMainMenu_FreeAppData(pMe);

    (void) ISHELL_Release(pMe->m_pShell);
    (void) IMODULE_Release(pMe->m_pModule);
    return 0;
}

/*==============================================================================
函数:
    MainMenu_RunFSM

说明:
    运行有限状态机引擎。

参数:
    pMe [in]: 指向MainMenu Applet对象结构的指针。该结构包含小程序的特定信息。
       
返回值:
    无。

备注:

==============================================================================*/
static void MainMenu_RunFSM(MainMenu *pMe)
{
    NextFSMAction nextFSMAction = NFSMACTION_WAIT;
    for ( ; ; )
    {
        nextFSMAction = MainMenu_ProcessState(pMe);

        // 这里不需要复杂的状态机，因此为简化状态机设计这里仅用一个布尔变量来
        // 指示dlgResult是否应该被改写。在某些特殊场合（见状态机设计），这种作
        // 法，避免了不必要的复杂性，简化了设计。
        if (pMe->m_bDoNotOverwriteDlgResult)
        {
            // We allow only one iteration without touching this variable. So,
            // let's reset it back to indicate that we are going to overwrite the
            // result for all subsequent iterations.
            pMe->m_bDoNotOverwriteDlgResult = FALSE;
        }
        else
        {
            // Overwrite the dlgResult to its default value. The next dialog that
            // we are going to display will set and return the dlgResult based on
            // the users action.
            pMe->m_eDlgReturn = DLGRET_CREATE;
        }

        // If the state machine processing engine indicates that it created a
        // dialog and requesting us to exit this loop, then we quit.
        if (nextFSMAction == NFSMACTION_WAIT)
        {
            break;
        }
    }
}

/*=============================================================================
FUNCTION:  MainMenu_HandleEvent

DESCRIPTION: 

PARAMETERS:

RETURN VALUE:

COMMENTS:

SIDE EFFECTS:

SEE ALSO:

=============================================================================*/
static boolean MainMenu_HandleEvent( IMainMenu *pi,
    AEEEvent eCode,
    uint16  wParam,
    uint32 dwParam
)
{
    MainMenu *pMe = (MainMenu*)pi;
    AEEAppStart* as = NULL;
    switch ( eCode)
    {
        case EVT_APP_START:
#ifdef FEATURE_RANDOM_MENU_REND
            DisplayRend_Enable(TRUE);
#endif
            // 此事件dwParam为指针，不应为0
            if (dwParam == 0) 
            {
                return FALSE;
            }            
            as = (AEEAppStart*)dwParam;
   
            pMe->m_rc = as->rc;

            pMe->m_currState = MAINST_MAIN;
            pMe->m_eDlgReturn = DLGRET_CREATE;
            pMe->m_eAppStatus = MAINMENU_RUNNING;
            IANNUNCIATOR_SetFieldIsActiveEx(pMe->m_pIAnn,TRUE); 
            MainMenu_RunFSM(pMe);
            return TRUE;

        case EVT_APP_STOP:
            {
                int theFocus = 0;
                SetCursor( pMe, theFocus);
                (void)ISHELL_CancelTimer( pMe->m_pShell, NULL, pMe);
                pMe->m_eAppStatus = MAINMENU_STOP;
#ifdef FEATURE_RANDOM_MENU_REND
                DisplayRend_Enable(FALSE);
#endif
                return TRUE;
            }

        case EVT_APP_SUSPEND:
            (void)ISHELL_CancelTimer(pMe->m_pShell, NULL, pMe);
            pMe->m_eAppStatus = MAINMENU_SUSPEND;
#ifdef FEATURE_RANDOM_MENU_REND
            DisplayRend_Push();
            DisplayRend_Start(FALSE);
#endif
            return TRUE;
        

        case EVT_APP_RESUME:
            {
                AEEAppStart* as = ( AEEAppStart*)dwParam;
                pMe->m_rc    = as->rc;
                pMe->m_eAppStatus = MAINMENU_RUNNING;
#ifdef FEATURE_RANDOM_MENU_REND
                DisplayRend_Push();
                DisplayRend_Start(TRUE);
#endif
                // 开始跑WMS状态机
                MainMenu_RunFSM(pMe); 
                return TRUE;
            }

        case EVT_DIALOG_INIT:
            pMe->m_pActiveIDlg = (IDialog*)dwParam;
            pMe->m_pActivedlgID = wParam;
            return MainMenu_RouteDialogEvt(pMe,eCode,wParam,dwParam);

        case EVT_DIALOG_START:
            return MainMenu_RouteDialogEvt(pMe,eCode,wParam,dwParam);

        case EVT_USER_REDRAW:
            return MainMenu_RouteDialogEvt(pMe,eCode,wParam,dwParam);

        case EVT_DIALOG_END:

            (void) MainMenu_RouteDialogEvt(pMe,eCode,wParam,dwParam);
            pMe->m_pActiveIDlg = NULL;
            pMe->m_pActivedlgID = 0;
            if(pMe->m_eAppStatus == MAINMENU_RUNNING)
            {
                MainMenu_RunFSM(pMe);
            }
            return TRUE;
        case EVT_KEY:
#if MIN_BREW_VERSION(3,0)
            // do not want to handle au
            if (((dwParam & KB_AUTOREPEAT) != 0) &&
                (wParam != AVK_UP) &&
                (wParam != AVK_DOWN) &&
                (wParam != AVK_LEFT) &&
                (wParam != AVK_RIGHT))
            {
                return TRUE;
            }
#endif
            return MainMenu_RouteDialogEvt(pMe,eCode,wParam,dwParam);  
#if defined (FEATURE_VERSION_N021)||defined(FEATURE_VERSION_W0216A)
		case EVT_KEY_HELD:
			{
				if(wParam == AVK_INFO)
				{
					(void) ISHELL_PostEvent(pMe->m_pShell,
                                        AEECLSID_CORE_APP,
                                        EVT_USER,
                                        1,
                                        0);
                    ISHELL_CloseApplet(pMe->m_pShell,TRUE);
				}
			}
			break;
#endif
        case EVT_KEY_PRESS:
        case EVT_KEY_RELEASE:
        case EVT_COMMAND:
            return MainMenu_RouteDialogEvt(pMe,eCode,wParam,dwParam); 

        default:
            return MainMenu_RouteDialogEvt(pMe,eCode,wParam,dwParam);
    }
}

void Mainmenu_KeypadLock(boolean block)
{
   gbMainmenuLock = block;
}

/*==============================================================================
函数:
    MainMenu_ProcessState

说明:
    根据 pMe->m_currState, 调用相应的状态处理函数。

参数:
    pMe [in]: 指向MainMenu Applet对象结构的指针。该结构包含小程序的特定信息。

返回值:
    NFSMACTION_CONTINUE: 指示不停状态机。
    NFSMACTION_WAIT: 指示停止状态机。

备注:

==============================================================================*/
NextFSMAction MainMenu_ProcessState(MainMenu *pMe)
{
    NextFSMAction retVal = NFSMACTION_WAIT;
    if (NULL == pMe)
    {
        return retVal;
    }
    switch(pMe->m_currState)
    {
        case MAINST_MAIN:
            return MAINST_MAIN_Handler(pMe);
        case MAINST_FLASHLIGHT:
        	return MAINST_FLASHLIGHT_Handler(pMe);    
        case MAINST_EXIT:
            return MAINST_EXIT_Handler(pMe);  
        default:
            break;

    }
    return retVal;
}

/*==============================================================================
函数:
    MAINST_MAIN_Handler

说明:
    MAINST_MAIN 状态处理函数。

参数:
    pMe [in]: 指向MAINMENU Applet对象结构的指针。该结构包含小程序的特定信息。

返回值:
    NFSMACTION_CONTINUE: 指示不停状态机。
    NFSMACTION_WAIT: 指示停止状态机。

备注:

==============================================================================*/
static NextFSMAction MAINST_MAIN_Handler(MainMenu *pMe)
{
    if (NULL == pMe)
    {
        return NFSMACTION_WAIT;
    }
    switch (pMe->m_eDlgReturn)
    {
        // 进入主界面
        case DLGRET_CREATE:
            {
                MainMenu_ShowDialog(pMe, IDD_MAIN_MENU);
            }
            return NFSMACTION_WAIT;
            
        case DLGRET_FLASHLITHT:
        	{
        		MOVE_TO_STATE(MAINST_FLASHLIGHT)
            	return NFSMACTION_CONTINUE;
        	}    
        case DLGRET_CANCELED:
            MOVE_TO_STATE(MAINST_EXIT)
            return NFSMACTION_CONTINUE;

        default:
            MOVE_TO_STATE(MAINST_EXIT)
            return NFSMACTION_CONTINUE;
    }
}


static NextFSMAction MAINST_FLASHLIGHT_Handler(MainMenu *pMe)
{
	if (NULL == pMe)
    {
        return NFSMACTION_WAIT;
    }
    switch (pMe->m_eDlgReturn)
    {
        // 进入主界面
        case DLGRET_CREATE:
            {
                MainMenu_ShowDialog(pMe, IDD_FLASHLIGHT_SETTING);
            }
            return NFSMACTION_WAIT;
         case DLGRET_CANCELED:         
            MOVE_TO_STATE(MAINST_MAIN)
            return NFSMACTION_CONTINUE;
        default:
            MOVE_TO_STATE(MAINST_EXIT)
            return NFSMACTION_CONTINUE;
    }
}


/*==============================================================================
函数:
    MAINST_EXIT_Handler

说明:
    MAINST_EXIT 状态处理函数。

参数:
    pMe [in]: 指向MAINMENU  Applet对象结构的指针。该结构包含小程序的特定信息。

返回值:
    NFSMACTION_CONTINUE: 指示不停状态机。
    NFSMACTION_WAIT: 指示停止状态机。

备注:

==============================================================================*/
static NextFSMAction MAINST_EXIT_Handler(MainMenu *pMe)
{
    (void) ISHELL_CloseApplet(pMe->m_pShell, FALSE);
    return NFSMACTION_WAIT;
} 

/*==============================================================================
函数:
       MainMenu_ShowDialog

说明:
    函数由状态处理函数用来唤起相应对话框资源ID的对话框。

参数:
    pMe [in]: 指向 MAINMENU Applet 对象结构的指针。该结构包含小程序的特定信息。
    dlgResId [in]: 对话框资源ID，函数根据该ID创建对话框。

返回值:
    none

备注:

==============================================================================*/
void MainMenu_ShowDialog(MainMenu  *pMe,  uint16 dlgResId)
{
    int nRet;

    // 每次最多打开一个对话框
    if (ISHELL_GetActiveDialog(pMe->m_pShell) != NULL)
    {
        // 如果当前已经有对话框被打开，直接返回
        return;
    }
   
    if (NULL != pMe->m_pDisplay)
    {
        AEEDeviceInfo di={0,};
#if defined (FEATURE_VERSION_H19C)
        if(pMe->m_pIAnn != NULL)
        {
            if (dlgResId == IDD_MAIN_MENU)
            {
                IANNUNCIATOR_SetHasTitleText(pMe->m_pIAnn, FALSE);
            }
            else
            {
                IANNUNCIATOR_SetHasTitleText(pMe->m_pIAnn, TRUE);
            }
            IANNUNCIATOR_Redraw(pMe->m_pIAnn);
        }
        (void)IDISPLAY_SetPrefs(pMe->m_pDisplay, "a:1", STRLEN("a:1"));
#else
        if (dlgResId == IDD_MAIN_MENU)
        {
            (void)IDISPLAY_SetPrefs(pMe->m_pDisplay, "a:0", STRLEN("a:0"));
        }
        else
        {
            (void)IDISPLAY_SetPrefs(pMe->m_pDisplay, "a:1", STRLEN("a:1"));
        }
#endif
        ISHELL_GetDeviceInfo(pMe->m_pShell, &di);
        pMe->m_rc.dx = di.cxScreen;
        pMe->m_rc.dy = di.cyScreen;       
        IDISPLAY_SetClipRect(pMe->m_pDisplay, &pMe->m_rc);        
    }
    
    nRet = ISHELL_CreateDialog(pMe->m_pShell,MAINMENU_RES_FILE_LANG,dlgResId,NULL);
    if (nRet != SUCCESS)
    {
    }
}


/*==============================================================================
函数:
       MainMenu_RouteDialogEvt
说明:
       函数将BREW事件路由给当前活动对话框的事件处理函数。

参数:
       pMe [In]: 指向SettingMenu Applet对象结构的指针,该结构包含小程序的特定信息.
       eCode [in]：事件代码。
       wParam [in]：与事件eCode关联的数据。
       dwParam [in]：与事件eCode关联的数据。

返回值:
       TRUE：传入事件被处理。
       FALSE：传入事件没被处理。

备注:

==============================================================================*/
boolean MainMenu_RouteDialogEvt(MainMenu *pMe,
    AEEEvent    eCode,
    uint16  wParam,
    uint32 dwParam
)

{
    if (NULL == pMe)
    {
        return FALSE;
    }

    if (NULL == pMe->m_pActiveIDlg)
    {
        return FALSE;
    }    
    switch( pMe->m_pActivedlgID)
    {
        case IDD_MAIN_MENU:
            return MainMenu_IconMenuHandler(pMe, eCode, wParam, dwParam);

        case IDD_FLASHLIGHT_SETTING:
            return MainMenu_FlashlightMenuHandler(pMe, eCode, wParam,dwParam);
        default:
            return FALSE;
    }
}

/*=============================================================================
FUNCTION:  MainMenu_IconMenuHandler

DESCRIPTION: 12宫格菜单

PARAMETERS:

=============================================================================*/
static boolean MainMenu_IconMenuHandler(MainMenu *pMe, AEEEvent eCode, uint16 wParam, uint32 dwParam)
{
    PARAM_NOT_REF( dwParam)
    switch ( eCode)
    {
        case EVT_DIALOG_INIT:			
            IDIALOG_SetProperties((IDialog *)dwParam, DLG_NOT_REDRAW_AFTER_START);
			MSG_FATAL("MainMenu_IconMenuHandler.........init....",0,0,0);
            return TRUE;

        case EVT_DIALOG_START:			
			AEE_CancelTimer(Main_keypadtimer,pMe);
			AEE_SetTimer(5*1000,Main_keypadtimer,pMe);
            (void) ISHELL_PostEvent( pMe->m_pShell,
                                     AEECLSID_MAIN_MENU,
                                     EVT_USER_REDRAW,
                                     0,
                                     0);
			MSG_FATAL("MainMenu_IconMenuHandler.........EVT_DIALOG_START....",0,0,0);
            return TRUE;

        case EVT_USER_REDRAW:
            // 初始整个背景及全部初始图标
            {
            	
            	MSG_FATAL("MainMenu_IconMenuHandler.........EVT_USER_REDRAW....",0,0,0);
            	DrawMatrix(pMe);
				MSG_FATAL("MainMenu_IconMenuHandler.........EVT_USER_REDRAW..over..",0,0,0);
				//AEE_CancelTimer(Main_loadtimer,pMe);
				//AEE_SetTimer(100,Main_loadtimer,pMe);
            	return TRUE;
            }
            
        case EVT_DIALOG_END:
            {
                ISHELL_CancelTimer(pMe->m_pShell, NULL, (void**)pMe);
            }
            return TRUE;

        case EVT_KEY_HELD:            
        #if defined(FEATURE_TORCH_KEY_INFO)
           if((AVKType)wParam == AVK_INFO)
            {
                boolean TorchOn = FALSE;
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
                ISHELL_CloseApplet(pMe->m_pShell, TRUE); 
                return TRUE;
            }
        #endif
	 #if defined(FEATURE_TORCH_KEY_SELECT) //xxzhen
           if((AVKType)wParam == AVK_SELECT)
            {
                boolean TorchOn = FALSE;
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
                ISHELL_CloseApplet(pMe->m_pShell, TRUE); 
                return TRUE;
            }
        #endif

            return TRUE;
        case EVT_KEY:
            switch( wParam)
            {
                case AVK_CLR:
                     CLOSE_DIALOG(DLGRET_CANCELED)
                    return TRUE;
                     
                case AVK_UP:       
                case AVK_LEFT:
                   if(pMe->m_index>0)
                   {
                   	   pMe->m_index = pMe->m_index-1;
                   }
				   else if(pMe->m_index == 0)
				   {
				   		pMe->m_index = (MAX_MATRIX_ITEMS-1);
				   }
				   MoveCursorTo(pMe, pMe->m_index);
                   return TRUE;

                case AVK_DOWN:     
                case AVK_RIGHT:

                   if(pMe->m_index<(MAX_MATRIX_ITEMS-1))
                   {
                   	   pMe->m_index = pMe->m_index+1;
					  
                   }
				   else if(pMe->m_index == (MAX_MATRIX_ITEMS-1))
				   {
				   		pMe->m_index = 0;
				   }
				   MoveCursorTo(pMe, pMe->m_index);
                   return TRUE;
                    
                case AVK_SELECT:
                case AVK_INFO:
                    {
					   int8 count_main = pMe->m_index;
					   MSG_FATAL("count_main=======%d",count_main,0,0);
					   OEM_SetConfig(CFGI_COUNT_OF_MAIN, &count_main, sizeof(int8));
                       StartApplet(pMe, pMe->m_IconTitle[pMe->m_index]);
                    }
                    return TRUE;
                case AVK_STAR:
                    if(gbMainmenuLock)
                    {
                        OEMKeyguard_SetState(TRUE);
                        ISHELL_CloseApplet(pMe->m_pShell, TRUE); 
                    }
                    return TRUE;
                default:
                    break;
            }

            return TRUE;
            
        default:
            break;
    }

    return FALSE;
}

//Add By zzg 2013_08_20
static boolean MainMenu_FlashlightMenuHandler(MainMenu *pMe, AEEEvent eCode, uint16 wParam, uint32 dwParam)
{
    PARAM_NOT_REF(dwParam)
    IMenuCtl *pMenu = (IMenuCtl*)IDIALOG_GetControl(pMe->m_pActiveIDlg,IDC_FLASHLIGHT_SET);
    AECHAR WTitle[40] = {0};
    if (pMenu == NULL)
    {
        return FALSE;
    }
    if(pMe->m_pIAnn != NULL)
    {
	    IANNUNCIATOR_SetFieldIsActiveEx(pMe->m_pIAnn,FALSE);
    }
     switch (eCode)
    {
        case EVT_DIALOG_INIT:
			(void)ISHELL_LoadResString(pMe->m_pShell,
                                        MAINMENU_RES_FILE_LANG,                                
                                        IDS_MAIN_MENU_TORCH,
                                        WTitle,
                                        sizeof(WTitle));

            if(pMe->m_pIAnn != NULL)
            {
			    IANNUNCIATOR_SetFieldTextEx(pMe->m_pIAnn,WTitle,FALSE);
            }
            IMENUCTL_AddItem(pMenu, MAINMENU_RES_FILE_LANG, IDS_MAIN_MENU_TORCH_ON, IDS_MAIN_MENU_TORCH_ON, NULL, 0);
            IMENUCTL_AddItem(pMenu, MAINMENU_RES_FILE_LANG, IDS_MAIN_MENU_TORCH_OFF, IDS_MAIN_MENU_TORCH_OFF, NULL, 0);
            return TRUE;
            
        case EVT_DIALOG_START:
            {
            	uint16 wItemID;
                boolean Is_on = FALSE;
                IMENUCTL_SetProperties(pMenu, MP_UNDERLINE_TITLE|MP_WRAPSCROLL|MP_TEXT_ALIGN_LEFT_ICON_ALIGN_RIGHT);
                IMENUCTL_SetOemProperties(pMenu, OEMMP_USE_MENU_STYLE);

                IMENUCTL_SetBottomBarType(pMenu,BTBAR_SELECT_BACK);
                OEM_GetConfig(CFGI_FLSHLITHG_STATUS,&Is_on, sizeof(Is_on));
                if(Is_on)
                {
                	wItemID = IDS_MAIN_MENU_TORCH_ON;
                }
                else
                {
                	wItemID = IDS_MAIN_MENU_TORCH_OFF;
                }

                InitMenuIcons(pMenu);
                SetMenuIcon(pMenu, wItemID, TRUE);
                IMENUCTL_SetSel(pMenu, wItemID);
            }
            return TRUE;

        case EVT_DIALOG_END:
            return TRUE;

        case EVT_KEY:
            switch(wParam)
            {
                case AVK_CLR:
                    CLOSE_DIALOG(DLGRET_CANCELED)
                    return TRUE;

                  default:
                    break;
            }
            return TRUE;
#ifdef FEATURE_LCD_TOUCH_ENABLE//andrew add for LCD touch
		case EVT_PEN_UP:
			{
				AEEDeviceInfo devinfo;
				int nBarH ;
				AEERect rc;
				int16 wXPos = (int16)AEE_GET_X(dwParam);
				int16 wYPos = (int16)AEE_GET_Y(dwParam);

				nBarH = GetBottomBarHeight(pMe->m_pDisplay);
        
				MEMSET(&devinfo, 0, sizeof(devinfo));
				ISHELL_GetDeviceInfo(pMe->m_pShell, &devinfo);
				SETAEERECT(&rc, 0, devinfo.cyScreen-nBarH, devinfo.cxScreen, nBarH);

				if(TOUCH_PT_IN_RECT(wXPos,wYPos,rc))
				{
					if(wXPos >= rc.x && wXPos < rc.x + (rc.dx/3) )//左
					{
						boolean rt =  ISHELL_PostEvent(pMe->m_pShell,AEECLSID_APPLICATION,EVT_KEY,AVK_SELECT,0);
						return rt;
					}
					else if(wXPos >= rc.x + (rc.dx/3)   && wXPos < rc.x + (rc.dx/3)*2 )//左
					{
						 boolean rt = ISHELL_PostEvent(pMe->m_pShell,AEECLSID_APPLICATION,EVT_KEY,AVK_INFO,0);
						 return rt;
					}
					else if(wXPos >= rc.x + (rc.dx/3)*2 && wXPos < rc.x + (rc.dx/3)*3 )//左
					{						
						 boolean rt = ISHELL_PostEvent(pMe->m_pShell,AEECLSID_APPLICATION,EVT_KEY,AVK_CLR,0);
						 return rt;
					}
				}

			}
			break;
#endif 
        case EVT_COMMAND:
            {
                boolean bytNewData = 0;

                switch (wParam)
                {
                    case  IDS_MAIN_MENU_TORCH_ON:
                       bytNewData = TRUE;
                       IBACKLIGHT_TurnOnTorch(pMe->m_pBacklight);
                       break;

                    case IDS_MAIN_MENU_TORCH_OFF:
                       bytNewData = FALSE;
                       IBACKLIGHT_TurnOffTorch(pMe->m_pBacklight);
                       break;

                    default:
                       break;

                }
                OEM_SetConfig(CFGI_FLSHLITHG_STATUS,&bytNewData, sizeof(bytNewData));
                InitMenuIcons(pMenu);
                SetMenuIcon(pMenu, wParam, TRUE);
                (void) ISHELL_PostEvent( pMe->m_pShell,
                                         AEECLSID_MAIN_MENU,
                                         EVT_USER_REDRAW,
                                         0,
                                         0);
            }
        default:
        	break;
	}
}
//Add End


static void Main_keypadtimer(void *pUser)
{
    gbMainmenuLock =FALSE;
}
static void Main_loadtimer(void *pUser)
{
	MainMenu	*pMe = (MainMenu *)pUser;
	int i = 0;
	int j = 0;
	#if 0
	for (i = 0; i < MAX_MATRIX_ITEMS; i ++)
    {
        if (pMe->m_pImageIcon[i] == NULL)
        {
            pMe->m_pImageIcon[i] = ISHELL_LoadImage(pMe->m_pShell,
                                                    ICON_ANI[i]);
        }
    }  
	#endif
    /*
	for(j=0;j<MAX_MATRIX_ITEMS;j++)
	{
		if (pMe->m_pImageSelectEC99[j] == NULL)
        {           
            pMe->m_pImageSelectEC99[j] = ISHELL_LoadImage(pMe->m_pShell, ICON_ANI[j]);
        }
	}
    */
	Main_loadover = TRUE;
	
}

static void MainMenu_DrawBackGround(MainMenu *pMe, AEERect *pRect)
{

    #ifdef FEATURE_VERSION_K232_X3
    Appscommon_ResetBackground(pMe->m_pDisplay, 
                                                pMe->m_pImageBg, 
                                                RGB_WHITE, 
                                                pRect, 
                                                0, 
                                                0);
    #else
    Appscommon_ResetBackground(pMe->m_pDisplay, 
                                                pMe->m_pImageBg, 
                                                APPSCOMMON_BG_COLOR, 
                                                pRect, 
                                                0, 
                                                0);
    #endif
	

}

/*=============================================================================
FUNCTION:  DrawMatrix

DESCRIPTION: // 初始整个背景及全部初始图标

=============================================================================*/
static void DrawMatrix(MainMenu *pMe)
{
    BottomBar_Param_type BarParam={0};
    boolean m_sound_bo_main = FALSE;
	nv_language_enum_type language;
    if (NULL == pMe)
    {
        return;
    }
	(void) ICONFIG_GetItem(pMe->m_pConfig,
                           CFGI_LANGUAGE_SELECTION,
                           &language,
                           sizeof(language));
    //draw bg image
    MainMenu_DrawBackGround(pMe, &pMe->m_rc);
	(void) ICONFIG_GetItem(pMe->m_pConfig,
									   CFGI_LANGUAGE_SELECTION,
									   &pMe->language,
									   sizeof(pMe->language));

   /*
    if(pMe->m_pImageSelectEC99[pMe->m_index]!=NULL)
	{
		IIMAGE_Draw(pMe->m_pImageSelectEC99[pMe->m_index], 0, 0);
	}
    */
    if (pMe->m_pImageSelectEC99 != NULL)
    {   
         (void)IIMAGE_Release(pMe->m_pImageSelectEC99);
         pMe->m_pImageSelectEC99 = NULL;
    }
    
    pMe->m_pImageSelectEC99 = ISHELL_LoadImage(pMe->m_pShell, ICON_ANI[pMe->m_index]);      
    if(pMe->m_pImageSelectEC99 != NULL)
    {
        IIMAGE_Draw(pMe->m_pImageSelectEC99, 0, pMe->m_rc.dy /4);
    }

	 if (pMe->m_pImageSelectEC99 != NULL)
    {   
         (void)IIMAGE_Release(pMe->m_pImageSelectEC99);
         pMe->m_pImageSelectEC99 = NULL;
    }
    if(language == NV_LANGUAGE_ENGLISH)
       pMe->m_pImageSelectEC99 = ISHELL_LoadImage(pMe->m_pShell, ICON_ANI_en[pMe->m_index]);
	else
       pMe->m_pImageSelectEC99 = ISHELL_LoadImage(pMe->m_pShell, ICON_ANI_cn[pMe->m_index]);      
    if(pMe->m_pImageSelectEC99 != NULL)
    {
        IIMAGE_Draw(pMe->m_pImageSelectEC99, 0, pMe->m_rc.dy /4 - 80);
    }
#if 0 //def FEATURE_SOUND_BO		
   	(void) ICONFIG_GetItem(pMe->m_pConfig,
                                 CFGI_SOUND_BO_MAIN,
                                 &m_sound_bo_main,
                                 sizeof(boolean));

	if(m_sound_bo_main)
	{

		 /*
		nv_item_type	SimChoice;
		(void)OEMNV_Get(NV_SIM_SELECT_I,&SimChoice);
		if(SimChoice.sim_select != 1)
        */   
        if (GetMp3PlayerStatus() == MP3STATUS_NONE)
		{
			MainMenu_PlayShutterSound(pMe,pMe->m_index);
		}
	}
#endif
	BarParam.eBBarType = BTBAR_SELECT_BACK;
	DrawBottomBar(pMe->m_pDisplay, &BarParam);

	IDISPLAY_UpdateEx(pMe->m_pDisplay, TRUE);   
}


/*=============================================================================
FUNCTION:  MoveCursorTo

DESCRIPTION: 

PARAMETERS:

RETURN VALUE:

COMMENTS:

SIDE EFFECTS:

SEE ALSO:

=============================================================================*/
static void MoveCursorTo(MainMenu *pMe, int index)
{
	BottomBar_Param_type BarParam={0};
	boolean m_sound_bo_main = FALSE;
	nv_language_enum_type language;
	//draw bg image
	if (NULL == pMe)
    {
        return;
    }
	(void) ICONFIG_GetItem(pMe->m_pConfig,
                                   CFGI_LANGUAGE_SELECTION,
                                   &language,
                                   sizeof(language));

#if 0 //def FEATURE_SOUND_BO    
	(void) ICONFIG_GetItem(pMe->m_pConfig,
                                 CFGI_SOUND_BO_MAIN,
                                 &m_sound_bo_main,
                                 sizeof(boolean));
	if(m_sound_bo_main)
	{
		 /*
		nv_item_type	SimChoice;
		(void)OEMNV_Get(NV_SIM_SELECT_I,&SimChoice);
		if(SimChoice.sim_select != 1)
        */   
        if (GetMp3PlayerStatus() == MP3STATUS_NONE)
		{
			MainMenu_PlayShutterSound(pMe,pMe->m_index);
		}
	}
#endif
    MainMenu_DrawBackGround(pMe, &pMe->m_rc);

    if (pMe->m_pImageSelectEC99 != NULL)
    {   
         (void)IIMAGE_Release(pMe->m_pImageSelectEC99);
         pMe->m_pImageSelectEC99 = NULL;
    }

    pMe->m_pImageSelectEC99 = ISHELL_LoadImage(pMe->m_pShell, ICON_ANI[pMe->m_index]);      
    if(pMe->m_pImageSelectEC99 != NULL)
    {
        IIMAGE_Draw(pMe->m_pImageSelectEC99, 0, pMe->m_rc.dy /4);
    }

	if (pMe->m_pImageSelectEC99 != NULL)
    {   
         (void)IIMAGE_Release(pMe->m_pImageSelectEC99);
         pMe->m_pImageSelectEC99 = NULL;
    }
    
    if(language == NV_LANGUAGE_ENGLISH)
       pMe->m_pImageSelectEC99 = ISHELL_LoadImage(pMe->m_pShell, ICON_ANI_en[pMe->m_index]);
	else
       pMe->m_pImageSelectEC99 = ISHELL_LoadImage(pMe->m_pShell, ICON_ANI_cn[pMe->m_index]);      
    if(pMe->m_pImageSelectEC99 != NULL)
    {
        IIMAGE_Draw(pMe->m_pImageSelectEC99, 0, pMe->m_rc.dy /4 - 80);
    }
	BarParam.eBBarType = BTBAR_SELECT_BACK;
	DrawBottomBar(pMe->m_pDisplay, &BarParam);
	IDISPLAY_UpdateEx(pMe->m_pDisplay, TRUE);
}
/*=============================================================================
FUNCTION:  SetCursor

DESCRIPTION: 

PARAMETERS:

RETURN VALUE:

COMMENTS:

SIDE EFFECTS:

SEE ALSO:

=============================================================================*/
static void SetCursor(MainMenu *pMe, int index)
{
    pMe->m_index        = index;   
    
    if(pMe->m_pAnimate != NULL)
    {
        IImage_Release(pMe->m_pAnimate);
        pMe->m_pAnimate  = NULL;
    }
}

/*=============================================================================
FUNCTION:  StartApplet

DESCRIPTION:  启动applet

PARAMETERS:  如果APPLET 有变动，只需改动次函数

=============================================================================*/
static int StartApplet(MainMenu *pMe, int i)
{
    int Result = EUNSUPPORTED;
	//uint32  dwTotal = 0;
	//uint32 free = 0;
	//GETFSFREE(&dwTotal);
	//free = GETRAMFREE(NULL,NULL);
	//MSG_FATAL("dwTotal======%d,free===%d",dwTotal,free,0);

    
#ifdef FEATURE_VERSION_H19C  
    if(pMe->m_pIAnn != NULL)
    {
        IANNUNCIATOR_SetHasTitleText(pMe->m_pIAnn, TRUE);
    }
#endif
		#ifdef FEATURE_VERSION_K202_LM129C //xxzhen
		  if(pMe->m_pIAnn != NULL)
          {
              IANNUNCIATOR_SetHasTitleText(pMe->m_pIAnn, TRUE);
          }
		#endif

    switch(i)
	{
#ifdef FEATURE_VERSION_W208S
	case IDS_MAIN_MENU_MAP:
	{
		char	buf[12];
		
		OEM_SetBROWSER_ADSAccount();	//Add By zzg 2011_07_08
		
    	MSG_FATAL("AEECLSID_BRW_APP...........START",0,0,0);
    	start_info.appid_fx = APP_ORIGINATOR_BROWSER;
        start_info.subtype_keycode = APP_BROWSER_START_URL;
        start_info.par_keychar = 0;

		start_info.fInfo.url_info.title = "map";
		start_info.fInfo.url_info.url = "http://wap.movidamovil.com/venezuela/?debug=true";		
		
        //start_info.fInfo.url_info.title = "163";
        //start_info.fInfo.url_info.url = "http://wap.163.com";

		
        SPRINTF(buf, "%p", &start_info);
		Result = ISHELL_StartAppletArgs(pMe->m_pShell,AEECLSID_NF3,buf);
        //Result = ISHELL_StartApplet(pMe->m_pShell, AEECLSID_BRW_APP);
        MSG_FATAL("AEECLSID_BRW_APP...........Result=%d",Result,0,0);		
		
		
		break;
	}
	case IDS_MAIN_MENU_NEO_APP:
	{
        Result = ISHELL_StartApplet(pMe->m_pShell, AEECLSID_APPMANAGER);
		break;
	}
	case IDS_MAIN_MENU_NEO_NAV:
	{
		char	buf[12];
		
		OEM_SetBROWSER_ADSAccount();	//Add By zzg 2011_07_08
		
    	MSG_FATAL("AEECLSID_BRW_APP...........START",0,0,0);
    	start_info.appid_fx = APP_ORIGINATOR_BROWSER;
        start_info.subtype_keycode = APP_BROWSER_START_MAINMENU;
        start_info.par_keychar = 0;
        //start_info.fInfo.url_info.title = "163";
        //start_info.fInfo.url_info.url = "http://www.163.com";
        SPRINTF(buf, "%p", &start_info);
		Result = ISHELL_StartAppletArgs(pMe->m_pShell,AEECLSID_NF3,buf);
        //Result = ISHELL_StartApplet(pMe->m_pShell, AEECLSID_BRW_APP);
        MSG_FATAL("AEECLSID_BRW_APP...........Result=%d",Result,0,0);		
		
		break;
	}
#endif

    case IDS_MAIN_MENU_FILEMGR:
    case IDS_MAIN_MENU_GALLERY:
	case IDS_MAIN_MENU_MEDIAGALLERY_C337:	
    case IDS_MAIN_MENU_MEDIAGALLERY:
        Result = ISHELL_StartApplet(pMe->m_pShell, AEECLSID_MEDIAGALLERY);
        break;
        
    case IDS_MAIN_MENU_CONTACT:
	case IDS_PHONEBOOK:
    case IDS_MAIN_MENU_CONTACTS:
        {
            IContactApp *ca = NULL;
            Result = ISHELL_CreateInstance(pMe->m_pShell,AEECLSID_APP_CONTACT, (void**)&ca);
            if(SUCCESS == Result)
            {
                ICONTAPP_MainMenu(ca);
                IContactApp_Release(ca);
            }
        }
        break;
    case IDS_MAIN_MENU_INTEXZONE: 
#ifdef FEATURE_VERSION_C316
		Result = ISHELL_StartApplet(pMe->m_pShell, AEECLSID_INDEX_ZONE);
		break;
#endif
    case IDS_MAIN_MENU_SMARTFRENACCESS:
    case IDS_MAIN_MENU_UTK:
        Result = ISHELL_StartApplet(pMe->m_pShell, AEECLSID_APP_UTK);
        break;
        
    case IDS_MAIN_MENU_TOOLS:
	case IDS_MAIN_MENU_ORGANIZER:	
    case IDS_MAIN_MENU_APPLICATION:
        Result = ISHELL_StartApplet(pMe->m_pShell, AEECLSID_APPLICATION);
        break;
		
    case IDS_MAIN_MENU_CALL_LOGS:
	case IDS_MAIN_MENU_RECENTCALLS_C337:	
    case IDS_MAIN_MENU_RECENTCALLS:
        Result = ISHELL_StartApplet(pMe->m_pShell, AEECLSID_APP_RECENTCALL);
        break;
        
    case IDS_MAIN_MENU_SMS:
    case IDS_MAIN_MENU_MESSAGES:
        Result = ISHELL_StartApplet(pMe->m_pShell, AEECLSID_WMSAPP);
        break;
        
    case IDS_MAIN_MENU_MULTIMEDIA:
        Result = ISHELL_StartApplet(pMe->m_pShell, AEECLSID_MULTIMEDIA_LIST);
        break;
        
    case IDS_MAIN_MENU_STATIC_APPLICATION:
#if defined(FEATURE_APP_MANAGER)
#ifdef FEATURE_OEMOMH
        if(ISHELL_SendEvent(pMe->m_pShell,AEECLSID_DIALER,EVT_OMH_PROMPT,0,0))
        {
            break;
        }
#endif

        Result = ISHELL_StartApplet(pMe->m_pShell, AEECLSID_APPMANAGER);
#else
        Result = ISHELL_StartApplet(pMe->m_pShell, AEECLSID_STATIC_APP);
#endif
    break;
        
    case IDS_MAIN_MENU_CALENDAR:
    case IDS_MAIN_MENU_SCHEDULER:
        Result = ISHELL_StartApplet(pMe->m_pShell, AEECLSID_SCHEDULEAPP);
        break;
        
    case IDS_MAIN_MENU_SETTINGS:
        Result = ISHELL_StartApplet(pMe->m_pShell, AEECLSID_APP_SETTINGMENU);
        break;

	//Add By zzg 2012_11_08 for C337 
	case IDS_MAIN_MENU_SERVICES:		
	    #if defined (FEATURE_VERSION_C337) || defined(FEATURE_VERSION_IC241A_MMX)
        #if defined (FEATURE_VERSION_IN50A)
        Result = SetBrowserArr_Main(pMe->m_pShell,(char*)"http://www.google.com.hk/"); 
        #else
        Result = SetBrowserArr_Main(pMe->m_pShell,(char*)"http://mimicromax.com"); //ISHELL_StartAppletArgs(pMe->m_pShell, AEECLSID_UCWEB, (char*)"call_ucweb:setmainpageurl:http://mimicromax.com");
        #endif        
        #else
        Result = SetBrowserArr_Main(pMe->m_pShell,NULL);//ISHELL_StartApplet(pMe->m_pShell, AEECLSID_UCWEB);
        #endif
		break;
	case IDS_MAIN_MENU_MSTORE:
#if defined (FEATURE_VERSION_C337) || defined(FEATURE_VERSION_IC241A_MMX)
#if defined (FEATURE_VERSION_IN50A)
        Result = SetBrowserArr_Main(pMe->m_pShell,(char*)"http://www.google.com.hk/"); 
#else
        Result = SetBrowserArr_Main(pMe->m_pShell,(char*)"http://mimicromax.com"); //ISHELL_StartAppletArgs(pMe->m_pShell, AEECLSID_UCWEB, (char*)"call_ucweb:setmainpageurl:http://mimicromax.com");
#endif
#else
        Result = SetBrowserArr_Main(pMe->m_pShell,NULL);//ISHELL_StartAppletArgs(pMe->m_pShell, AEECLSID_UCWEB, (char*)"call_ucweb:setmainpageurl:http://mimicromax.com");
#endif
		break;
	case IDS_MAIN_MENU_MZONE:
		Result = ISHELL_StartApplet(pMe->m_pShell, AEECLSID_MiZone);
		break;
	//Add End
	
	case IDS_MAIN_MENU_USER_PROFILE:	
    case IDS_MAIN_MENU_USERPROFILE:
        Result = ISHELL_StartApplet(pMe->m_pShell, AEECLSID_APP_SOUNDMENU);
        break;
        
    case IDS_MAIN_MENU_GAMES:
        Result = ISHELL_StartApplet(pMe->m_pShell, AEECLSID_GAME);
        break;
		
	#ifdef FEATURE_VERSION_C316
    case IDS_MAIN_MENU_SERVICES_C316:  
		Result = ISHELL_StartApplet(pMe->m_pShell, AEECLSID_SERVICE);
        break;
	#endif	
	
    case IDS_MAIN_WAPBROWSER:
		#ifdef FEATURE_VERSION_K202_LM129C
		  IANNUNCIATOR_SetHasTitleText(pMe->m_pIAnn,FALSE); //xxzhen
		   IANNUNCIATOR_Redraw(pMe->m_pIAnn);
		#endif

        #if defined (FEATURE_VERSION_C337) || defined(FEATURE_VERSION_IC241A_MMX)
        #if defined (FEATURE_VERSION_IN50A)
        Result = SetBrowserArr_Main(pMe->m_pShell,(char*)"http://www.google.com.hk/"); 
        #else
        Result = SetBrowserArr_Main(pMe->m_pShell,(char*)"http://mimicromax.com"); //ISHELL_StartAppletArgs(pMe->m_pShell, AEECLSID_UCWEB, (char*)"call_ucweb:setmainpageurl:http://mimicromax.com");
        #endif        
        #else
        Result = SetBrowserArr_Main(pMe->m_pShell,(char*)""); //ISHELL_StartApplet(pMe->m_pShell, AEECLSID_UCWEB);
        #endif
        break;
    
    case IDS_MAIN_MENU_CALCULATOR: 
		 Result = ISHELL_StartApplet(pMe->m_pShell, AEECLSID_CALCAPP);
        break;
        
    case IDS_MAIN_MENU_FMRADIO:
        Result = ISHELL_StartApplet(pMe->m_pShell, AEECLSID_APP_FMRADIO);
        break;
        
    case IDS_MAIN_MENU_MUSICPLAYER:
        Result = ISHELL_StartApplet(pMe->m_pShell, AEECLSID_APP_MUSICPLAYER);
        break;
        
    case IDS_MAIN_MENU_SMARTWORLD:
    case IDS_MAIN_MENU_BLIVE:
#ifdef FEATURE_SMARTFREN_STATIC_BREW_APP
		Result = ISHELL_StartApplet(pMe->m_pShell, AEECLSID_STATIC_APP);
#else
#ifdef FEATURE_OEMOMH
        if(ISHELL_SendEvent(pMe->m_pShell,AEECLSID_DIALER,EVT_OMH_PROMPT,0,0))
        {
            break;
        }
#endif
        Result = ISHELL_StartApplet(pMe->m_pShell, AEECLSID_APPMANAGER);
#endif  
        break;
    
    case IDS_MAIN_MENU_FRENDUO:
#if defined(FEATURE_VERSION_M8) || defined(FEATURE_VERSION_M8P) || defined (FEATURE_VERSION_M8021)
		Result = ISHELL_StartApplet(pMe->m_pShell, AEECLSID_FRENDUO);
#endif
        break;
    
    case IDS_MAIN_MENU_TIMER:
        Result = ISHELL_StartApplet(pMe->m_pShell, AEECLSID_APPTIMER);
        break;
        
    case IDS_MAIN_MENU_CAMERA:
	{		
		Result = ISHELL_StartApplet(pMe->m_pShell, AEECLSID_APP_CAMERA);
		break;
	}  
    
    case IDS_MAIN_MENU_TORCH:
    {		
		//Result = ISHELL_StartApplet(pMe->m_pShell, AEECLSID_TORCH);
		CLOSE_DIALOG(DLGRET_FLASHLITHT)
		break;
	}    
    
    default:
        break;

    }
    return Result;
}

extern char charsvc_p_name[UIM_CDMA_HOME_SERVICE_SIZE+1];
int SetBrowserArr_Main(IShell *pShell ,char *purl)
{
	int Result = EUNSUPPORTED;
	char urlCan[1024] = {0};
    
	DBGPRINTF("svc_p_name %s %d",charsvc_p_name,charsvc_p_name[0],0);
	OEM_SetUCBROWSER_ADSAccount();
    
	if(purl && STRLEN(purl)>1)
	{
        SPRINTF(urlCan,"call_ucweb:setexternurl:%s\2\3",purl);
		STRCAT(urlCan,"useragent:BREW-Applet/0x20068888 (BREW/3.1.5.20; DeviceId: 8976509865757e; Lang: hi; Profile/MIDP-2.0_Configuration/CLDC-1.1) ucweb-squid\2\3");
	}
	else
	{
		STRCPY(urlCan,"useragent:BREW-Applet/0x20068888 (BREW/3.1.5.20; DeviceId: 8976509865757e; Lang: hi; Profile/MIDP-2.0_Configuration/CLDC-1.1) ucweb-squid\2\3");
	}
    
	if(STRISTR (charsvc_p_name,"mts"))
	{
		MSG_FATAL("mst................",0,0,0);
        STRCAT(urlCan,"access_point:proxy_is:10.50.5.140:8080");
	}
	else if(STRISTR (charsvc_p_name,"tata"))
	{
        MSG_FATAL("tata................",0,0,0);
        STRCAT(urlCan,"access_point:proxy_is:172.23.252.15:9401");
	}
	else if(STRISTR (charsvc_p_name,"reliance"))
	{
        MSG_FATAL("reliance................",0,0,0);
        STRCAT(urlCan,"access_point:proxy_is:http://wapgw.ricinfo.com:8080");
	}
	else if(STRISTR (charsvc_p_name,"vmi"))
	{
        MSG_FATAL("vmi................",0,0,0);
        STRCAT(urlCan,"access_point:proxy_is:172.23.142.15:9401");
	}
	
	DBGPRINTF("urlCan==%s", urlCan);
    if(urlCan[0])
    {
	    Result = ISHELL_StartAppletArgs(pShell, AEECLSID_UCWEB, (char*)urlCan);
    }
    else
    {
        Result = ISHELL_StartApplet(pShell, AEECLSID_UCWEB);
    }

	return Result;	
}
#if 0 //def FEATURE_SOUND_BO 
static void MainMenu_PlayShutterSound(MainMenu *pMe,int key)
{

    AEEMediaCmdNotify cmd;
	int temp = 0;
	char music_name[256] = {0};
    if(pMe->m_pMedia)
	{
		IMEDIA_Stop(pMe->m_pMedia);
		IMEDIA_Release(pMe->m_pMedia);
		pMe->m_pMedia = NULL;
	}
    // 如果pMe->m_pMedia接口为空，创建接口
    if(!pMe->m_pMedia)
    {
        AEEMediaData      md;
       
        if(!pMe)
           return;
        md.clsData = MMD_FILE_NAME;  		   
		md.pData = (void *)MAIN_SOUND_NAME[key];
        md.dwSize = 0;
       
        (void)AEEMediaUtil_CreateMedia(pMe->m_pShell, &md, &pMe->m_pMedia);
    }
   
    if(pMe->m_pMedia)
    {        
        IMEDIA_SetVolume(pMe->m_pMedia, AEE_MAX_VOLUME*3/5); //max volum is 100
     
        if(IMEDIA_RegisterNotify(pMe->m_pMedia, MainMenu_MediaNotify, pMe) != SUCCESS)
        {
            cmd.nCmd    = MM_CMD_PLAY;
            cmd.nStatus = MM_STATUS_DONE;
            MainMenu_MediaNotify((void *)pMe, &cmd);
            return;
        }
        MSG_FATAL("MainMenu_PlayShutterSound,IMEDIA_Play....",0,0,0);
        if(IMEDIA_Play(pMe->m_pMedia) != SUCCESS)
        {
            cmd.nCmd    = MM_CMD_PLAY;
            cmd.nStatus = MM_STATUS_DONE;
            MainMenu_MediaNotify((void *)pMe, &cmd);
            return;
        }
    }
    else
    {
        cmd.nCmd    = MM_CMD_PLAY;
        cmd.nStatus = MM_STATUS_DONE;
        MainMenu_MediaNotify((void *)pMe, &cmd);
    }  
}
#endif
static void MainMenu_MediaNotify(void *pUser, AEEMediaCmdNotify *pCmdNotify)
{
    MainMenu *pMe = (MainMenu *)pUser;

    if(!pMe || !pCmdNotify)
        return;

    if(pCmdNotify->nCmd == MM_CMD_PLAY)  // IMEDIA_Play events
    {
        switch (pCmdNotify->nStatus)
        {
            case MM_STATUS_ABORT:            
                break;

            case MM_STATUS_DONE:    // playback done
				if(pMe->m_pMedia)
				{
					IMEDIA_Release(pMe->m_pMedia);
					pMe->m_pMedia = NULL;
				}
                break;
            default:
                break;
        }
    }
}

#else
#define PARAM_NOT_REF(x)
/*==============================================================================

                              本地函数声明

==============================================================================*/
static MainMenuMod gMainMenuMod;
static const VTBL( IModule) gModFuncs =
{
    MainMenuMod_AddRef,
    MainMenuMod_Release,
    MainMenuMod_CreateInstance,
    MainMenuMod_FreeResources
};

static uint32  MainMenu_AddRef( IMainMenu *pi);

static uint32  MainMenu_Release ( IMainMenu *pi);

static boolean MainMenu_HandleEvent( IMainMenu *pi,
    AEEEvent eCode,
    uint16  wParam,
    uint32 dwParam
);

static MainMenu gMainMenu={0};

static const VTBL( IMainMenu) gMainMenuMethods =
{
    MainMenu_AddRef,
    MainMenu_Release,
    MainMenu_HandleEvent
};

int MainMenuMod_Load( IShell *pIShell, void *ph, IModule **ppMod);

int MainMenuMod_New( int16 nSize,
    IShell              *pIShell,
    void                *ph,
    IModule             **ppMod,
    PFNMODCREATEINST    pfnMC,
    PFNFREEMODDATA      pfnMF
);

static int MainMenu_New( IShell *ps, IModule *pIModule, IMainMenu **ppObj);

static int CMainMenu_InitAppData(MainMenu *pMe);

static void CMainMenu_FreeAppData(MainMenu *pMe);

static void MainMenu_RunFSM(MainMenu *pMe);

static void CalculateScreenParameters(MainMenu *pMe);

static void MainMenu_DrawBackGround(MainMenu *pMe, AEERect *pRect);

// 初始整个背景及全部初始图标

int SetBrowserArr_Main(IShell *pShell, char *purl);

static void DrawMatrix(MainMenu *pMe);

static void DrawFocusIcon(MainMenu *pMe);

static void MoveCursorTo(MainMenu *pMe, int row, int column);

static void SetCursor(MainMenu *pMe, int row, int column);

static int StartApplet(MainMenu *pMe, int i);

boolean MainMenu_RouteDialogEvt(MainMenu *pMe,
    AEEEvent    eCode,
    uint16  wParam,
    uint32 dwParam
);

NextFSMAction MainMenu_ProcessState(MainMenu *pMe);

void MainMenu_ShowDialog(MainMenu  *pMe,  uint16 dlgResId);

static boolean  gbMainmenuLock = FALSE;

static void Main_keypadtimer(void *pUser);


// MAINST_MAIN 状态处理函数
static NextFSMAction MAINST_MAIN_Handler(MainMenu *pMe);
//MAINST_EXIT  状态处理函数
static NextFSMAction MAINST_EXIT_Handler(MainMenu *pMe);
static boolean  MainMenu_IconMenuHandler(MainMenu *pMe, AEEEvent eCode, uint16 wParam, uint32 dwParam);

/*==============================================================================
                              
                              本地（静态）数据
                              
==============================================================================*/
static char* ICON_ANI[] =
{
#if defined (FEATURE_VERSION_H19C) || defined (FEATURE_VERSION_C01) || defined (FEATURE_VERSION_SKY) || defined(FEATURE_VERSION_W021_C11) 
    ICON_ANI_BG,
#else
    ICON1_ANI,
    ICON2_ANI,
    ICON3_ANI,
    ICON4_ANI,
    ICON5_ANI,
    ICON6_ANI,
    ICON7_ANI,
    ICON8_ANI,
    ICON9_ANI,
#if defined (FEATURE_DISP_160X128)
    ICON10_ANI,
    ICON11_ANI,
    ICON12_ANI,
#elif defined (FEATURE_DISP_220X176)
    ICON10_ANI,
    ICON11_ANI,
    ICON12_ANI,    
#elif defined (FEATURE_DISP_128X160)
#if !defined(FEATURE_VERSION_C316) && !defined(FEATURE_VERSION_W450_JFY)
    ICON10_ANI,
    ICON11_ANI,
    ICON12_ANI,
#endif    
#elif defined (FEATURE_DISP_176X220)
#if !defined( FEATURE_VERSION_VG68) && !defined(FEATURE_VERSION_K202_LM129C)&&!defined(FEATURE_VERSION_W516_C260)
    ICON10_ANI,
    ICON11_ANI,
    ICON12_ANI,
#endif
#elif defined (FEATURE_DISP_240X320)
#if !defined(FEATURE_VERSION_K212_HUALU)
    ICON10_ANI,
    ICON11_ANI,
    ICON12_ANI,
#endif
#elif defined (FEATURE_DISP_320X240)
    ICON10_ANI,
    ICON11_ANI,
    ICON12_ANI,        
#endif
#endif
};

static char* ICON_ANI_1[] =
{
    ICON1_ANI_1,
    ICON2_ANI_1,
    ICON3_ANI_1,
    ICON4_ANI_1,
    ICON5_ANI_1,
    ICON6_ANI_1,
    ICON7_ANI_1,
    ICON8_ANI_1,
    ICON9_ANI_1,
#if defined (FEATURE_DISP_160X128)
    ICON10_ANI_1,
    ICON11_ANI_1,
    ICON12_ANI_1,
#elif defined (FEATURE_DISP_220X176)
    ICON10_ANI_1,
    ICON11_ANI_1,
    ICON12_ANI_1,    
#elif defined (FEATURE_DISP_128X160)
#ifndef FEATURE_VERSION_C01
#ifndef FEATURE_VERSION_C316
#ifndef FEATURE_VERSION_W450_JFY
#ifndef FEATURE_VERSION_W021_C11
    ICON10_ANI_1,
    ICON11_ANI_1,
    ICON12_ANI_1,
#endif
#endif    
#endif 
#endif
#elif defined (FEATURE_DISP_176X220)
#if !defined( FEATURE_VERSION_VG68)&&!defined(FEATURE_VERSION_SKY)&&!defined(FEATURE_VERSION_K202_LM129C)&&!defined(FEATURE_VERSION_W516_C260)
    ICON10_ANI_1,
    ICON11_ANI_1,
    ICON12_ANI_1,
#endif    
#elif defined (FEATURE_DISP_240X320)
#if !defined(FEATURE_VERSION_K212_HUALU)
    ICON10_ANI_1,
    ICON11_ANI_1,
    ICON12_ANI_1,     
#endif
#elif defined (FEATURE_DISP_320X240)
    ICON10_ANI_1,
    ICON11_ANI_1,
    ICON12_ANI_1,      
#endif
};

#if defined (FEATURE_VERSION_K212_HUALU)
static char* ICON_EN_ANI[] =
{
    ICON1_EN_ANI,
    ICON2_EN_ANI,
    ICON3_EN_ANI,
    ICON4_EN_ANI,
    ICON5_EN_ANI,
    ICON6_EN_ANI,
    ICON7_EN_ANI,
    ICON8_EN_ANI,
    ICON9_EN_ANI,
};

static char* ICON_EN_ANI_1[] =
{
    ICON1_EN_ANI_1,
    ICON2_EN_ANI_1,
    ICON3_EN_ANI_1,
    ICON4_EN_ANI_1,
    ICON5_EN_ANI_1,
    ICON6_EN_ANI_1,
    ICON7_EN_ANI_1,
    ICON8_EN_ANI_1,
    ICON9_EN_ANI_1,
};
#endif
/*=============================================================================
FUNCTION:  MainMenuMod_Load

DESCRIPTION: 

PARAMETERS:

RETURN VALUE:

COMMENTS:

SIDE EFFECTS:

SEE ALSO:

=============================================================================*/
int MainMenuMod_Load( IShell *pIShell, void *ph, IModule **ppMod)
{	
    return MainMenuMod_New( sizeof( MainMenuMod),
                    pIShell,
                    ph,
                    ppMod,
                    NULL,
                    NULL);
}

/*=============================================================================
FUNCTION:  MainMenuMod_New

DESCRIPTION: 

PARAMETERS:

RETURN VALUE:

COMMENTS:

SIDE EFFECTS:

SEE ALSO:

=============================================================================*/
int  MainMenuMod_New( int16 nSize,
    IShell   *pIShell,
    void     *ph,
    IModule  **ppMod,
    PFNMODCREATEINST pfnMC,
    PFNFREEMODDATA pfnMF
)
{
#if !defined( AEE_SIMULATOR)
    PARAM_NOT_REF( nSize)
    PARAM_NOT_REF( pfnMC)
    PARAM_NOT_REF( pfnMF)
    PARAM_NOT_REF( ph)
#endif

    if( NULL == ppMod)
    {
        return EFAILED;
    }

    MEMSET( &gMainMenuMod, 0, sizeof( MainMenuMod));

    INIT_VTBL( &gMainMenuMod, IModule, gModFuncs);
    gMainMenuMod.referenceCounter = 1;
    *ppMod = ( IModule *)&gMainMenuMod;
    return AEE_SUCCESS;
}

/*=============================================================================
FUNCTION:  MainMenuMod_AddRef

DESCRIPTION: 

PARAMETERS:

RETURN VALUE:

COMMENTS:

SIDE EFFECTS:

SEE ALSO:

=============================================================================*/
static uint32 MainMenuMod_AddRef( IModule *po)
{
    return( ++( ( MainMenuMod *)po)->referenceCounter);
}

/*=============================================================================
FUNCTION:  MainMenuMod_Release

DESCRIPTION: 

PARAMETERS:

RETURN VALUE:

COMMENTS:

SIDE EFFECTS:

SEE ALSO:

=============================================================================*/
static uint32 MainMenuMod_Release( IModule *po)
{
    if ( ( ( MainMenuMod *) po)->referenceCounter == 0)
    {
        return 0;
    }

    return( --( ( MainMenuMod *)po)->referenceCounter);
}

/*=============================================================================
FUNCTION:  MainMenuMod_CreateInstance

DESCRIPTION: 

PARAMETERS:

RETURN VALUE:

COMMENTS:

SIDE EFFECTS:

SEE ALSO:

=============================================================================*/
static int  MainMenuMod_CreateInstance( IModule *po,
    IShell *pIShell,
    AEECLSID ClsId,
    void **ppObj
)
{
    *ppObj = NULL;

    if( ClsId != AEECLSID_MAIN_MENU || MainMenu_New( pIShell, po, ( IMainMenu**)ppObj) != SUCCESS)
    {
        return EFAILED;
    }

    return SUCCESS;
}

/*=============================================================================
FUNCTION:  MainMenuMod_FreeResources

DESCRIPTION: 

PARAMETERS:

RETURN VALUE:

COMMENTS:

SIDE EFFECTS:

SEE ALSO:

=============================================================================*/
static void MainMenuMod_FreeResources( IModule *po, IHeap *ph, IFileMgr *pfm)
{
}

/*=============================================================================
FUNCTION:  MainMenu_New

DESCRIPTION: 

PARAMETERS:

RETURN VALUE:

COMMENTS:

SIDE EFFECTS:

SEE ALSO:

=============================================================================*/
static int MainMenu_New( IShell *ps, IModule *pIModule, IMainMenu **ppObj)
{
    int retVal = SUCCESS;
    if( 0 == gMainMenu.referenceCounter)
    {

        if( pIModule == NULL || ps == NULL)
        {
            return EFAILED;
        }

        MEMSET(&gMainMenu, 0, sizeof(gMainMenu));
        INIT_VTBL(&gMainMenu, IMainMenu, gMainMenuMethods);

       
        gMainMenu.m_pShell     = ps;
        gMainMenu.m_pModule    = pIModule;
        retVal = CMainMenu_InitAppData(&gMainMenu);
        if(retVal != SUCCESS)
        {
            CMainMenu_FreeAppData((MainMenu*)&gMainMenu);
            return retVal;
        }
        (void) ISHELL_AddRef( ps);
        (void) IMODULE_AddRef( pIModule);
        gMainMenu.referenceCounter  = 0;

        
    }

    ++ gMainMenu.referenceCounter;
    *ppObj = ( IMainMenu*)&gMainMenu;
    
    return retVal;
}

/*==============================================================================
函数:
    CMainMenuApp_InitAppData

说明:
    初始化 MainMenu Applet 结构数据缺省值。

参数:
    pMe [in]: 指向 MainMenu Applet 对象结构的指针。该结构包含小程序的特定信息。

返回值:
    EFAILED: 初始化失败。
    SUCCESS: 初始化成功。

备注:

==============================================================================*/
static int CMainMenu_InitAppData(MainMenu *pMe)
{
    if (NULL == pMe)
    {
        return EFAILED;
    }
#if defined (FEATURE_DISP_160X128)

//#if defined(FEATURE_PROJECT_SMART) || defined(FEATURE_PROJECT_M8)
#if defined(FEATURE_MENU_FOCUS_INDEX)	
	pMe->m_nRow        = 1;
    pMe->m_nColumn     = 2;
#else
    pMe->m_nRow        = 1;
    pMe->m_nColumn     = 1;
#endif

#elif defined (FEATURE_DISP_220X176)
	pMe->m_nRow        = 1;
    pMe->m_nColumn     = 1;
#elif defined (FEATURE_DISP_128X128)
	pMe->m_nRow        = 1;
    pMe->m_nColumn     = 1;
#elif defined (FEATURE_DISP_128X160)
	pMe->m_nRow        = 1;
    pMe->m_nColumn     = 1;	
#elif defined (FEATURE_DISP_176X220)
	pMe->m_nRow        = 1;
    pMe->m_nColumn     = 1;	
#elif defined (FEATURE_DISP_240X320)
	pMe->m_nRow        = 1;
    pMe->m_nColumn     = 1;		
#elif defined (FEATURE_DISP_320X240)
	pMe->m_nRow        = 1;
    pMe->m_nColumn     = 1;			
#endif
    
    // 接口创建及相关初始化
    if (ISHELL_CreateInstance(pMe->m_pShell, AEECLSID_CONFIG,
            (void **)&pMe->m_pConfig) != SUCCESS)
    {    	
        return EFAILED;
    }

    if( ISHELL_CreateInstance( pMe->m_pShell, AEECLSID_BACKLIGHT, (void **)&pMe->m_pBacklight)!=AEE_SUCCESS)
    {
        return FALSE;
    }
     
    if (ISHELL_CreateInstance(pMe->m_pShell, AEECLSID_DISPLAY, 
            (void **) &pMe->m_pDisplay) != SUCCESS)
    {    	
        return EFAILED;
    }
	if (AEE_SUCCESS != ISHELL_CreateInstance(pMe->m_pShell,
                                            AEECLSID_ANNUNCIATOR,
                                            (void **)&pMe->m_pIAnn))
    {    	
        return EFAILED;
    }
    // 初始化菜单Title
#if defined (FEATURE_DISP_160X128)
#if defined	(FEATURE_VERSION_FLEXI203) || defined	(FEATURE_VERSION_IVIO203)
    pMe->m_IconTitle[0]     = IDS_MAIN_MENU_MEDIAGALLERY;
    pMe->m_IconTitle[1]     = IDS_MAIN_MENU_CONTACTS;
    pMe->m_IconTitle[2]     = IDS_MAIN_MENU_UTK;
    pMe->m_IconTitle[3]     = IDS_MAIN_MENU_TOOLS;
    pMe->m_IconTitle[4]     = IDS_MAIN_MENU_RECENTCALLS;
    pMe->m_IconTitle[5]     = IDS_MAIN_MENU_MESSAGES;
    pMe->m_IconTitle[6]     = IDS_MAIN_MENU_MULTIMEDIA;
    pMe->m_IconTitle[7]     = IDS_MAIN_MENU_STATIC_APPLICATION;
    pMe->m_IconTitle[8]     = IDS_MAIN_MENU_SCHEDULER;
    pMe->m_IconTitle[9]     = IDS_MAIN_MENU_SETTINGS;
    pMe->m_IconTitle[10]    = IDS_MAIN_MENU_USERPROFILE;
    pMe->m_IconTitle[11]    = IDS_MAIN_MENU_GAMES;
#elif defined (FEATURE_VERSION_C500BE)
	pMe->m_IconTitle[0] 	= IDS_MAIN_MENU_MEDIAGALLERY;
	pMe->m_IconTitle[1] 	= IDS_MAIN_MENU_CONTACTS;
	pMe->m_IconTitle[2] 	= IDS_MAIN_MENU_UTK;
	pMe->m_IconTitle[3] 	= IDS_MAIN_MENU_CALCULATOR;
	pMe->m_IconTitle[4] 	= IDS_MAIN_MENU_RECENTCALLS;
	pMe->m_IconTitle[5] 	= IDS_MAIN_MENU_MESSAGES;
	pMe->m_IconTitle[6] 	= IDS_MAIN_MENU_MULTIMEDIA;
	pMe->m_IconTitle[7] 	= IDS_MAIN_MENU_TOOLS;
	pMe->m_IconTitle[8] 	= IDS_MAIN_MENU_SCHEDULER;
	pMe->m_IconTitle[9] 	= IDS_MAIN_MENU_SETTINGS;
	pMe->m_IconTitle[10]	= IDS_MAIN_MENU_USERPROFILE;
	pMe->m_IconTitle[11]	= IDS_MAIN_MENU_GAMES;
#elif defined (FEATURE_VERSION_SMART)
    pMe->m_IconTitle[0]     = IDS_MAIN_MENU_SMARTWORLD;
    pMe->m_IconTitle[1]     = IDS_MAIN_MENU_SMARTFRENACCESS;
    pMe->m_IconTitle[2]     = IDS_MAIN_MENU_MUSICPLAYER;
    pMe->m_IconTitle[3]     = IDS_MAIN_MENU_FMRADIO;
    pMe->m_IconTitle[4]     = IDS_MAIN_MENU_CALENDAR;
    pMe->m_IconTitle[5]     = IDS_MAIN_MENU_CONTACT;
    pMe->m_IconTitle[6]     = IDS_MAIN_MENU_SMS;
    pMe->m_IconTitle[7]     = IDS_MAIN_MENU_GAMES;
    pMe->m_IconTitle[8]     = IDS_MAIN_MENU_RECENTCALLS;
    pMe->m_IconTitle[9]     = IDS_MAIN_MENU_GALLERY;
    pMe->m_IconTitle[10]    = IDS_MAIN_MENU_APPLICATION;
    pMe->m_IconTitle[11]    = IDS_MAIN_MENU_SETTINGS;
#elif defined (FEATURE_VERSION_M8) 
	pMe->m_IconTitle[0] 	= IDS_MAIN_MENU_BLIVE;
   	pMe->m_IconTitle[1]	   = IDS_MAIN_MENU_SMARTFRENACCESS;
   	pMe->m_IconTitle[2]	   = IDS_MAIN_MENU_MUSICPLAYER;
  	pMe->m_IconTitle[3]	   = IDS_MAIN_MENU_FMRADIO;
   	pMe->m_IconTitle[4]	   = IDS_MAIN_MENU_FRENDUO;
   	pMe->m_IconTitle[5]	   = IDS_MAIN_MENU_CONTACT;
   	pMe->m_IconTitle[6]	   = IDS_MAIN_MENU_SMS;
   	pMe->m_IconTitle[7]	   = IDS_MAIN_MENU_GAMES;
   	pMe->m_IconTitle[8]	   = IDS_MAIN_MENU_RECENTCALLS;
   	pMe->m_IconTitle[9]	   = IDS_MAIN_MENU_GALLERY;
  	pMe->m_IconTitle[10]    = IDS_MAIN_MENU_APPLICATION;
   	pMe->m_IconTitle[11]    = IDS_MAIN_MENU_SETTINGS;
#elif defined (FEATURE_VERSION_M8P)
	pMe->m_IconTitle[0] 	= IDS_MAIN_MENU_BLIVE;
	pMe->m_IconTitle[1] 	= IDS_MAIN_MENU_SMARTFRENACCESS;
	pMe->m_IconTitle[2] 	= IDS_MAIN_MENU_MUSICPLAYER;
	pMe->m_IconTitle[3] 	= IDS_MAIN_MENU_FMRADIO;
	pMe->m_IconTitle[4] 	= IDS_MAIN_MENU_FRENDUO;
	pMe->m_IconTitle[5] 	= IDS_MAIN_MENU_CONTACT;
	pMe->m_IconTitle[6] 	= IDS_MAIN_MENU_SMS;
	pMe->m_IconTitle[7] 	= IDS_MAIN_MENU_GAMES;
	pMe->m_IconTitle[8] 	= IDS_MAIN_MENU_RECENTCALLS;
	pMe->m_IconTitle[9] 	= IDS_MAIN_MENU_GALLERY;
	pMe->m_IconTitle[10]	= IDS_MAIN_MENU_APPLICATION;
	pMe->m_IconTitle[11]	= IDS_MAIN_MENU_SETTINGS;
#else
    pMe->m_IconTitle[0]     = IDS_MAIN_MENU_MEDIAGALLERY;
    pMe->m_IconTitle[1]     = IDS_MAIN_MENU_CONTACTS;
    pMe->m_IconTitle[2]     = IDS_MAIN_MENU_UTK;
    pMe->m_IconTitle[3]     = IDS_MAIN_MENU_CALCULATOR;
    pMe->m_IconTitle[4]     = IDS_MAIN_MENU_RECENTCALLS;
    pMe->m_IconTitle[5]     = IDS_MAIN_MENU_MESSAGES;
    pMe->m_IconTitle[6]     = IDS_MAIN_MENU_MULTIMEDIA;
    pMe->m_IconTitle[7]     = IDS_MAIN_MENU_APPLICATION;
    pMe->m_IconTitle[8]     = IDS_MAIN_MENU_SCHEDULER;
    pMe->m_IconTitle[9]     = IDS_MAIN_MENU_SETTINGS;
    pMe->m_IconTitle[10]    = IDS_MAIN_MENU_GAMES;
    pMe->m_IconTitle[11]    = IDS_MAIN_MENU_MUSICPLAYER;
#endif
#elif defined (FEATURE_DISP_220X176)
#if defined (FEATURE_VERSION_M8P)
	pMe->m_IconTitle[0] 	= IDS_MAIN_MENU_BLIVE;
	pMe->m_IconTitle[1] 	= IDS_MAIN_MENU_SMARTFRENACCESS;
	pMe->m_IconTitle[2] 	= IDS_MAIN_MENU_MUSICPLAYER;
	pMe->m_IconTitle[3] 	= IDS_MAIN_MENU_FMRADIO;
	pMe->m_IconTitle[4] 	= IDS_MAIN_MENU_FRENDUO;
	pMe->m_IconTitle[5] 	= IDS_MAIN_MENU_CONTACT;
	pMe->m_IconTitle[6] 	= IDS_MAIN_MENU_SMS;
	pMe->m_IconTitle[7] 	= IDS_MAIN_MENU_GAMES;
	pMe->m_IconTitle[8] 	= IDS_MAIN_MENU_RECENTCALLS;
	pMe->m_IconTitle[9] 	= IDS_MAIN_MENU_GALLERY;
	pMe->m_IconTitle[10]	= IDS_MAIN_MENU_APPLICATION;
	pMe->m_IconTitle[11]	= IDS_MAIN_MENU_SETTINGS;
#elif defined(FEATURE_VERSION_FLEXI203P)
    pMe->m_IconTitle[0]     = IDS_MAIN_MENU_MEDIAGALLERY;
    pMe->m_IconTitle[1]     = IDS_MAIN_MENU_CONTACTS;
    pMe->m_IconTitle[2]     = IDS_MAIN_MENU_UTK;
    pMe->m_IconTitle[3]     = IDS_MAIN_MENU_TOOLS;
    pMe->m_IconTitle[4]     = IDS_MAIN_MENU_RECENTCALLS;
    pMe->m_IconTitle[5]     = IDS_MAIN_MENU_MESSAGES;
    pMe->m_IconTitle[6]     = IDS_MAIN_MENU_MULTIMEDIA;
    pMe->m_IconTitle[7]     = IDS_MAIN_MENU_STATIC_APPLICATION;
    pMe->m_IconTitle[8]     = IDS_MAIN_MENU_SCHEDULER;
    pMe->m_IconTitle[9]     = IDS_MAIN_MENU_SETTINGS;
    pMe->m_IconTitle[10]    = IDS_MAIN_MENU_USERPROFILE;
    pMe->m_IconTitle[11]    = IDS_MAIN_MENU_GAMES;
#elif defined(FEATURE_VERSION_W208S)
    pMe->m_IconTitle[0]     = IDS_MAIN_MENU_CALL_LOGS;	//IDS_MAIN_MENU_MEDIAGALLERY;
    pMe->m_IconTitle[1]     = IDS_MAIN_MENU_CONTACTS;
    pMe->m_IconTitle[2]     = IDS_MAIN_MENU_MULTIMEDIA;		//IDS_MAIN_MENU_UTK;
    pMe->m_IconTitle[3]     = IDS_MAIN_MENU_MAP;
    pMe->m_IconTitle[4]     = IDS_MAIN_MENU_NEO_APP;
    pMe->m_IconTitle[5]     = IDS_MAIN_MENU_NEO_NAV;
    pMe->m_IconTitle[6]     = IDS_MAIN_MENU_MESSAGES;		//IDS_MAIN_MENU_MULTIMEDIA;
    pMe->m_IconTitle[7]     = IDS_MAIN_MENU_CAMERA;			//IDS_MAIN_MENU_STATIC_APPLICATION;
    pMe->m_IconTitle[8]     = IDS_MAIN_MENU_FMRADIO;		 //IDS_MAIN_MENU_SCHEDULER;
    pMe->m_IconTitle[9]     = IDS_MAIN_MENU_SETTINGS;
    pMe->m_IconTitle[10]    = IDS_MAIN_MENU_FILEMGR;		//IDS_MAIN_MENU_USERPROFILE;
    pMe->m_IconTitle[11]    = IDS_MAIN_MENU_TOOLS;			//IDS_MAIN_MENU_GAMES;	
#elif defined(FEATURE_VERSION_K212_20D)
    pMe->m_IconTitle[0]     = IDS_MAIN_MENU_MEDIAGALLERY;
    pMe->m_IconTitle[1]     = IDS_MAIN_MENU_CONTACTS;
    pMe->m_IconTitle[2]     = IDS_MAIN_MENU_UTK;
    pMe->m_IconTitle[3]     = IDS_MAIN_MENU_CALCULATOR;;
    pMe->m_IconTitle[4]     = IDS_MAIN_MENU_RECENTCALLS;
    pMe->m_IconTitle[5]     = IDS_MAIN_MENU_MESSAGES;
    pMe->m_IconTitle[6]     = IDS_MAIN_MENU_MULTIMEDIA;
    pMe->m_IconTitle[7]     = IDS_MAIN_MENU_APPLICATION;
    pMe->m_IconTitle[8]     = IDS_MAIN_MENU_SCHEDULER;;
    pMe->m_IconTitle[9]     = IDS_MAIN_MENU_SETTINGS;;
    pMe->m_IconTitle[10]    = IDS_MAIN_MENU_GAMES;
    pMe->m_IconTitle[11]    = IDS_MAIN_MENU_MUSICPLAYER;	    
#else
    pMe->m_IconTitle[0]     = IDS_MAIN_MENU_MEDIAGALLERY;
    pMe->m_IconTitle[1]     = IDS_MAIN_MENU_CONTACTS;
    pMe->m_IconTitle[2]     = IDS_MAIN_MENU_UTK;
    pMe->m_IconTitle[3]     = IDS_MAIN_MENU_MULTIMEDIA;
    pMe->m_IconTitle[4]     = IDS_MAIN_MENU_MESSAGES;
    pMe->m_IconTitle[5]     = IDS_MAIN_MENU_RECENTCALLS;
    pMe->m_IconTitle[6]     = IDS_MAIN_MENU_MUSICPLAYER;
    pMe->m_IconTitle[7]     = IDS_MAIN_MENU_SETTINGS;
    pMe->m_IconTitle[8]     = IDS_MAIN_MENU_APPLICATION;
    pMe->m_IconTitle[9]     = IDS_MAIN_MENU_GAMES;
    pMe->m_IconTitle[10]    = IDS_MAIN_MENU_SCHEDULER;
    pMe->m_IconTitle[11]    = IDS_MAIN_MENU_CALCULATOR;
#endif
#elif defined (FEATURE_DISP_128X128)
#ifndef FEATURE_VERSION_H19C
    pMe->m_IconTitle[0]     = IDS_MAIN_MENU_MEDIAGALLERY;
    pMe->m_IconTitle[1]     = IDS_MAIN_MENU_CONTACTS;
    pMe->m_IconTitle[2]     = IDS_MAIN_MENU_UTK;
    pMe->m_IconTitle[3]     = IDS_MAIN_MENU_MULTIMEDIA;
    pMe->m_IconTitle[4]     = IDS_MAIN_MENU_MESSAGES;
    pMe->m_IconTitle[5]     = IDS_MAIN_MENU_RECENTCALLS;
    pMe->m_IconTitle[6]     = IDS_MAIN_MENU_MUSICPLAYER;
    pMe->m_IconTitle[7]     = IDS_MAIN_MENU_SETTINGS;
    pMe->m_IconTitle[8]     = IDS_MAIN_MENU_APPLICATION;
#else
    pMe->m_IconTitle[0]     = IDS_MAIN_MENU_RECENTCALLS;
    pMe->m_IconTitle[1]     = IDS_MAIN_MENU_MULTIMEDIA;
    pMe->m_IconTitle[2]     = IDS_MAIN_MENU_CONTACTS;
    pMe->m_IconTitle[3]     = IDS_MAIN_MENU_USERPROFILE;
    pMe->m_IconTitle[4]     = IDS_MAIN_MENU_MESSAGES;
    pMe->m_IconTitle[5]     = IDS_MAIN_MENU_TOOLS;
    pMe->m_IconTitle[6]     = IDS_MAIN_MENU_SETTINGS;
    pMe->m_IconTitle[7]     = IDS_MAIN_MENU_GAMES;
    pMe->m_IconTitle[8]     = IDS_MAIN_MENU_UTK;
#endif
#elif defined (FEATURE_DISP_128X160)
    #if defined (FEATURE_VERSION_M8021)
    pMe->m_IconTitle[0]     = IDS_MAIN_MENU_BLIVE;
    pMe->m_IconTitle[1]     = IDS_MAIN_MENU_SMARTFRENACCESS;
    pMe->m_IconTitle[2]     = IDS_MAIN_MENU_MUSICPLAYER;
    pMe->m_IconTitle[3]     = IDS_MAIN_MENU_FMRADIO;
    pMe->m_IconTitle[4]     = IDS_MAIN_MENU_FRENDUO;
    pMe->m_IconTitle[5]     = IDS_MAIN_MENU_CONTACT;
    pMe->m_IconTitle[6]     = IDS_MAIN_MENU_SMS;
    pMe->m_IconTitle[7]     = IDS_MAIN_MENU_GAMES;
    pMe->m_IconTitle[8]     = IDS_MAIN_MENU_RECENTCALLS;
    pMe->m_IconTitle[9]     = IDS_MAIN_MENU_GALLERY;
    pMe->m_IconTitle[10]    = IDS_MAIN_MENU_APPLICATION;
    pMe->m_IconTitle[11]    = IDS_MAIN_MENU_SETTINGS;
    #elif defined(FEATURE_VERSION_W450_JFY)
    pMe->m_IconTitle[0]     = IDS_MAIN_MENU_RECENTCALLS;
    pMe->m_IconTitle[1]     = IDS_MAIN_MENU_MULTIMEDIA;
    pMe->m_IconTitle[2]     = IDS_MAIN_MENU_CONTACTS;
    pMe->m_IconTitle[3]     = IDS_MAIN_MENU_USERPROFILE;
    pMe->m_IconTitle[4]     = IDS_MAIN_MENU_MESSAGES;
    pMe->m_IconTitle[5]     = IDS_MAIN_MENU_SETTINGS;
    pMe->m_IconTitle[6]     = IDS_MAIN_MENU_GAMES;
    pMe->m_IconTitle[7]     = IDS_MAIN_MENU_TOOLS;
    pMe->m_IconTitle[8]     = IDS_MAIN_MENU_UTK;
    #elif defined (FEATURE_VERSION_MYANMAR)
    pMe->m_IconTitle[0]     = IDS_MAIN_MENU_MEDIAGALLERY;
    pMe->m_IconTitle[1]     = IDS_MAIN_MENU_CONTACTS;
    pMe->m_IconTitle[2]     = IDS_MAIN_MENU_USERPROFILE;
    pMe->m_IconTitle[3]     = IDS_MAIN_MENU_MULTIMEDIA;
    pMe->m_IconTitle[4]     = IDS_MAIN_MENU_MESSAGES;
    pMe->m_IconTitle[5]     = IDS_MAIN_MENU_RECENTCALLS;
    pMe->m_IconTitle[6]     = IDS_MAIN_MENU_MUSICPLAYER;
    pMe->m_IconTitle[7]     = IDS_MAIN_MENU_SETTINGS;
    pMe->m_IconTitle[8]     = IDS_MAIN_MENU_APPLICATION;
    pMe->m_IconTitle[9]     = IDS_MAIN_MENU_GAMES;
    pMe->m_IconTitle[10]    = IDS_MAIN_MENU_SCHEDULER;
    pMe->m_IconTitle[11]    = IDS_MAIN_MENU_CALCULATOR;
    #elif defined (FEATURE_VERSION_C306)
    pMe->m_IconTitle[0]     = IDS_MAIN_MENU_MEDIAGALLERY;
    pMe->m_IconTitle[1]     = IDS_MAIN_MENU_CONTACTS;
    pMe->m_IconTitle[2]     = IDS_MAIN_MENU_UTK;
    pMe->m_IconTitle[3]     = IDS_MAIN_MENU_MULTIMEDIA;
    pMe->m_IconTitle[4]     = IDS_MAIN_MENU_MESSAGES;
    pMe->m_IconTitle[5]     = IDS_MAIN_MENU_RECENTCALLS;
    pMe->m_IconTitle[6]     = IDS_MAIN_MENU_MUSICPLAYER;
    pMe->m_IconTitle[7]     = IDS_MAIN_MENU_SETTINGS;
    pMe->m_IconTitle[8]     = IDS_MAIN_MENU_APPLICATION;
    pMe->m_IconTitle[9]     = IDS_MAIN_MENU_TIMER;
    pMe->m_IconTitle[10]    = IDS_MAIN_MENU_SCHEDULER;
    pMe->m_IconTitle[11]    = IDS_MAIN_MENU_CALCULATOR;
    #elif defined (FEATURE_VERSION_C01)
    pMe->m_IconTitle[0]     = IDS_MAIN_MENU_RECENTCALLS;
    pMe->m_IconTitle[1]     = IDS_MAIN_MENU_MULTIMEDIA;
    pMe->m_IconTitle[2]     = IDS_MAIN_MENU_CONTACTS;
    pMe->m_IconTitle[3]     = IDS_MAIN_MENU_USERPROFILE;
    pMe->m_IconTitle[4]     = IDS_MAIN_MENU_MESSAGES;
    pMe->m_IconTitle[5]     = IDS_MAIN_MENU_TOOLS;
    pMe->m_IconTitle[6]     = IDS_MAIN_MENU_SETTINGS;
    pMe->m_IconTitle[7]     = IDS_MAIN_MENU_GAMES;
    pMe->m_IconTitle[8]     = IDS_MAIN_MENU_UTK;
	#elif defined (FEATURE_VERSION_W021_C11)
	pMe->m_IconTitle[0]     = IDS_MAIN_MENU_RECENTCALLS;
    pMe->m_IconTitle[1]     = IDS_MAIN_MENU_MULTIMEDIA;
    pMe->m_IconTitle[2]     = IDS_MAIN_MENU_CONTACTS;
    pMe->m_IconTitle[3]     = IDS_MAIN_MENU_USERPROFILE;
    pMe->m_IconTitle[4]     = IDS_MAIN_MENU_MESSAGES;
    pMe->m_IconTitle[5]     = IDS_MAIN_MENU_SCHEDULER;
    pMe->m_IconTitle[6]     = IDS_MAIN_MENU_SETTINGS;
    pMe->m_IconTitle[7]     = IDS_MAIN_MENU_GAMES;
    pMe->m_IconTitle[8]     = IDS_MAIN_MENU_GALLERY;
	#elif defined (FEATURE_VERSION_C316)
    pMe->m_IconTitle[0]     = IDS_MAIN_MENU_CONTACTS;
    pMe->m_IconTitle[1]     = IDS_MAIN_MENU_MESSAGES;
    pMe->m_IconTitle[2]     = IDS_MAIN_MENU_RECENTCALLS;
    pMe->m_IconTitle[3]     = IDS_MAIN_MENU_SETTINGS;
    pMe->m_IconTitle[4]     = IDS_MAIN_MENU_MULTIMEDIA;
    pMe->m_IconTitle[5]     = IDS_MAIN_MENU_MEDIAGALLERY;
    pMe->m_IconTitle[6]     = IDS_MAIN_MENU_ORGANIZER;
    pMe->m_IconTitle[7]     = IDS_MAIN_MENU_INTEXZONE;
    pMe->m_IconTitle[8]     = IDS_MAIN_MENU_SERVICES_C316;
    #elif defined(FEATURE_VERSION_C337)||defined(FEATURE_VERSION_K232_Y100A)
    pMe->m_IconTitle[0]     = IDS_MAIN_MENU_MULTIMEDIA;	//IDS_MAIN_MENU_MUSICPLAYER;	
	pMe->m_IconTitle[1]     = IDS_PHONEBOOK;	
    pMe->m_IconTitle[2]     = IDS_MAIN_MENU_RECENTCALLS_C337;	//IDS_MAIN_MENU_RECENTCALLS;
    #ifdef FEATURE_VERSION_K232_Y100A
    pMe->m_IconTitle[3]     = IDS_MAIN_MENU_CAMERA;
    #else
    pMe->m_IconTitle[3]     = IDS_MAIN_MENU_SERVICES;	//IDS_MAIN_WAPBROWSER;
    #endif
    pMe->m_IconTitle[4]     = IDS_MAIN_MENU_MESSAGES;
    pMe->m_IconTitle[5]     = IDS_MAIN_MENU_MEDIAGALLERY_C337;	//IDS_MAIN_MENU_MEDIAGALLERY;
	#if defined(FEATURE_VERSION_K232_Y100A)
    pMe->m_IconTitle[6]     = IDS_MAIN_MENU_UTK;
    #else
    pMe->m_IconTitle[6]     = IDS_MAIN_MENU_MSTORE;	//IDS_MAIN_MENU_MULTIMEDIA;		//m! STORE
    #endif
    pMe->m_IconTitle[7]     = IDS_MAIN_MENU_ORGANIZER;	//IDS_MAIN_MENU_APPLICATION;
#if defined(FEATURE_VERSION_C260_IC19)||defined(FEATURE_VERSION_K232_Y100A)
    pMe->m_IconTitle[8]     = IDS_MAIN_MENU_MUSICPLAYER;
#else
    pMe->m_IconTitle[8]     = IDS_MAIN_MENU_MZONE;	//IDS_MAIN_MENU_APPLICATION;		//m! ZONE
#endif  
	
    pMe->m_IconTitle[9]     = IDS_MAIN_MENU_USER_PROFILE;	//IDS_MAIN_MENU_USERPROFILE;	//IDS_MAIN_MENU_APPLICATION;
    pMe->m_IconTitle[10]    = IDS_MAIN_MENU_GAMES;
    pMe->m_IconTitle[11]    = IDS_MAIN_MENU_SETTINGS;	
	#elif defined(FEATURE_VERSION_W317A)
    pMe->m_IconTitle[0]     = IDS_MAIN_MENU_CONTACTS;
    pMe->m_IconTitle[1]     = IDS_MAIN_MENU_RECENTCALLS_C337;	//IDS_MAIN_MENU_RECENTCALLS;
    pMe->m_IconTitle[2]     = IDS_MAIN_MENU_SETTINGS;
    pMe->m_IconTitle[3]     = IDS_MAIN_MENU_MEDIAGALLERY_C337;	//IDS_MAIN_MENU_MEDIAGALLERY;
    pMe->m_IconTitle[4]     = IDS_MAIN_MENU_MESSAGES;
    pMe->m_IconTitle[5]     = IDS_MAIN_MENU_MULTIMEDIA;
    pMe->m_IconTitle[6]     = IDS_MAIN_MENU_UTK;
    pMe->m_IconTitle[7]     = IDS_MAIN_MENU_MUSICPLAYER;
    pMe->m_IconTitle[8]     = IDS_MAIN_MENU_CALENDAR;	//IDS_MAIN_MENU_SCHEDULER;
    pMe->m_IconTitle[9]     = IDS_MAIN_MENU_APPLICATION;
    pMe->m_IconTitle[10]    = IDS_MAIN_MENU_GAMES;
    pMe->m_IconTitle[11]    = IDS_MAIN_MENU_CALCULATOR;
    #else
    pMe->m_IconTitle[0]     = IDS_MAIN_MENU_MEDIAGALLERY;
    pMe->m_IconTitle[1]     = IDS_MAIN_MENU_CONTACTS;
    pMe->m_IconTitle[2]     = IDS_MAIN_MENU_UTK;
    pMe->m_IconTitle[3]     = IDS_MAIN_MENU_MULTIMEDIA;
    pMe->m_IconTitle[4]     = IDS_MAIN_MENU_MESSAGES;
    pMe->m_IconTitle[5]     = IDS_MAIN_MENU_RECENTCALLS;
    pMe->m_IconTitle[6]     = IDS_MAIN_MENU_MUSICPLAYER;
    pMe->m_IconTitle[7]     = IDS_MAIN_MENU_SETTINGS;
    pMe->m_IconTitle[8]     = IDS_MAIN_MENU_APPLICATION;
    pMe->m_IconTitle[9]     = IDS_MAIN_MENU_GAMES;
    pMe->m_IconTitle[10]    = IDS_MAIN_MENU_SCHEDULER;
    pMe->m_IconTitle[11]    = IDS_MAIN_MENU_CALCULATOR;
    #endif
#elif defined (FEATURE_DISP_176X220)
#if defined(FEATURE_VERSION_VG68)
    pMe->m_IconTitle[0]     = IDS_MAIN_MENU_RECENTCALLS;
    pMe->m_IconTitle[1]     = IDS_MAIN_MENU_MULTIMEDIA;
    pMe->m_IconTitle[2]     = IDS_MAIN_MENU_CONTACTS;
    pMe->m_IconTitle[3]     = IDS_MAIN_MENU_SCHEDULER;
    pMe->m_IconTitle[4]     = IDS_MAIN_MENU_MESSAGES;
    pMe->m_IconTitle[5]     = IDS_MAIN_MENU_TOOLS;
    pMe->m_IconTitle[6]     = IDS_MAIN_MENU_SETTINGS;
    pMe->m_IconTitle[7]     = IDS_MAIN_MENU_GAMES;
    pMe->m_IconTitle[8]     = IDS_MAIN_MENU_UTK;
#elif defined(FEATURE_VERSION_K202_LM129C) //xxzhen
    pMe->m_IconTitle[0]     = IDS_MAIN_MENU_RECENTCALLS;
    pMe->m_IconTitle[1]     = IDS_MAIN_MENU_CONTACTS;
    pMe->m_IconTitle[2]     = IDS_MAIN_MENU_FILEMGR;
    pMe->m_IconTitle[3]     = IDS_MAIN_MENU_TOOLS;
    pMe->m_IconTitle[4]     = IDS_MAIN_MENU_MESSAGES;
    pMe->m_IconTitle[5]     = IDS_MAIN_MENU_MUSICPLAYER;
    pMe->m_IconTitle[6]     = IDS_MAIN_MENU_UTK; //xxzhen browser for utk
    pMe->m_IconTitle[7]     = IDS_MAIN_MENU_MULTIMEDIA;
    pMe->m_IconTitle[8]     = IDS_MAIN_MENU_SETTINGS;
#elif defined (FEATURE_VERSION_C310) // add by pyuangui 20121224
    pMe->m_IconTitle[0]     = IDS_MAIN_MENU_CONTACTS;
    pMe->m_IconTitle[1]     = IDS_MAIN_MENU_CALL_LOGS;	
    pMe->m_IconTitle[2]     = IDS_MAIN_MENU_SETTINGS;
    pMe->m_IconTitle[3]     = IDS_MAIN_MENU_FILEMGR;	
    pMe->m_IconTitle[4]     = IDS_MAIN_MENU_MESSAGES;
    pMe->m_IconTitle[5]     = IDS_MAIN_MENU_MULTIMEDIA;
    pMe->m_IconTitle[6]     = IDS_MAIN_MENU_CAMERA;
    pMe->m_IconTitle[7]     = IDS_MAIN_MENU_MUSICPLAYER;
    pMe->m_IconTitle[8]     = IDS_MAIN_MENU_CALENDAR;	
    pMe->m_IconTitle[9]     = IDS_MAIN_MENU_APPLICATION;
    pMe->m_IconTitle[10]    = IDS_MAIN_MENU_GAMES;
    pMe->m_IconTitle[11]    = IDS_MAIN_WAPBROWSER;
#elif defined (FEATURE_VERSION_W516_C260)
	pMe->m_IconTitle[0]     = IDS_MAIN_MENU_MEDIAGALLERY;
    pMe->m_IconTitle[1]     = IDS_MAIN_MENU_CONTACTS;
    pMe->m_IconTitle[2]     = IDS_MAIN_MENU_RECENTCALLS;         //IDS_MAIN_MENU_UTK;
    pMe->m_IconTitle[3]     = IDS_MAIN_MENU_MULTIMEDIA;		//IDS_MAIN_MENU_MULTIMEDIA;
    pMe->m_IconTitle[4]     = IDS_MAIN_MENU_MESSAGES;
    pMe->m_IconTitle[5]     = IDS_MAIN_MENU_SETTINGS;			//IDS_MAIN_MENU_RECENTCALLS;
    pMe->m_IconTitle[6]     = IDS_MAIN_MENU_APPLICATION;		//IDS_MAIN_MENU_MUSICPLAYER;
    pMe->m_IconTitle[7]     = IDS_MAIN_MENU_GAMES;				//IDS_MAIN_MENU_SETTINGS;
    pMe->m_IconTitle[8]     = IDS_MAIN_MENU_UTK;				//IDS_MAIN_MENU_APPLICATION;
#elif defined(FEATURE_VERSION_W317A)
    pMe->m_IconTitle[0]     = IDS_MAIN_MENU_CONTACTS;
    pMe->m_IconTitle[1]     = IDS_MAIN_MENU_RECENTCALLS_C337;	//IDS_MAIN_MENU_RECENTCALLS;
    pMe->m_IconTitle[2]     = IDS_MAIN_MENU_SETTINGS;
    pMe->m_IconTitle[3]     = IDS_MAIN_MENU_MEDIAGALLERY_C337;	//IDS_MAIN_MENU_MEDIAGALLERY;
    pMe->m_IconTitle[4]     = IDS_MAIN_MENU_MESSAGES;
    pMe->m_IconTitle[5]     = IDS_MAIN_MENU_MULTIMEDIA;
    pMe->m_IconTitle[6]     = IDS_MAIN_MENU_CAMERA;
    pMe->m_IconTitle[7]     = IDS_MAIN_MENU_MUSICPLAYER;
    pMe->m_IconTitle[8]     = IDS_MAIN_MENU_CALENDAR;	//IDS_MAIN_MENU_SCHEDULER;
    pMe->m_IconTitle[9]     = IDS_MAIN_MENU_APPLICATION;
    pMe->m_IconTitle[10]    = IDS_MAIN_MENU_GAMES;
    pMe->m_IconTitle[11]    = IDS_MAIN_WAPBROWSER;
#elif defined(FEATURE_VERSION_C337) 
    pMe->m_IconTitle[0]     = IDS_MAIN_MENU_MULTIMEDIA;	//IDS_MAIN_MENU_MUSICPLAYER;	
	pMe->m_IconTitle[1]     = IDS_PHONEBOOK;	
    pMe->m_IconTitle[2]     = IDS_MAIN_MENU_RECENTCALLS_C337;	//IDS_MAIN_MENU_RECENTCALLS;
    pMe->m_IconTitle[3]     = IDS_MAIN_MENU_SERVICES;	//IDS_MAIN_WAPBROWSER;
    pMe->m_IconTitle[4]     = IDS_MAIN_MENU_MESSAGES;
    pMe->m_IconTitle[5]     = IDS_MAIN_MENU_MEDIAGALLERY_C337;	//IDS_MAIN_MENU_MEDIAGALLERY;
	
    pMe->m_IconTitle[6]     = IDS_MAIN_MENU_MSTORE;	//IDS_MAIN_MENU_MULTIMEDIA;		//m! STORE
    pMe->m_IconTitle[7]     = IDS_MAIN_MENU_ORGANIZER;	//IDS_MAIN_MENU_APPLICATION;
    pMe->m_IconTitle[8]     = IDS_MAIN_MENU_MZONE;	//IDS_MAIN_MENU_APPLICATION;		//m! ZONE	
    pMe->m_IconTitle[9]     = IDS_MAIN_MENU_USER_PROFILE;	//IDS_MAIN_MENU_USERPROFILE;	//IDS_MAIN_MENU_APPLICATION;
    pMe->m_IconTitle[10]    = IDS_MAIN_MENU_GAMES;
    pMe->m_IconTitle[11]    = IDS_MAIN_MENU_SETTINGS;	
#elif defined (FEATURE_VERSION_SKY)
    pMe->m_IconTitle[0]     = IDS_MAIN_MENU_MESSAGES;
    pMe->m_IconTitle[1]     = IDS_MAIN_MENU_CONTACTS;
    pMe->m_IconTitle[2]     = IDS_MAIN_MENU_RECENTCALLS;
    pMe->m_IconTitle[3]     = IDS_MAIN_MENU_MULTIMEDIA;
    pMe->m_IconTitle[4]     = IDS_MAIN_MENU_UTK;
    pMe->m_IconTitle[5]     = IDS_MAIN_MENU_TOOLS;
    pMe->m_IconTitle[6]     = IDS_MAIN_MENU_GAMES;
    pMe->m_IconTitle[7]     = IDS_MAIN_MENU_USERPROFILE;
    pMe->m_IconTitle[8]     = IDS_MAIN_MENU_SETTINGS;
#else
    pMe->m_IconTitle[0]     = IDS_MAIN_MENU_MEDIAGALLERY;
    pMe->m_IconTitle[1]     = IDS_MAIN_MENU_CONTACTS;
    pMe->m_IconTitle[2]     = IDS_MAIN_MENU_UTK;
    pMe->m_IconTitle[3]     = IDS_MAIN_MENU_MULTIMEDIA;
    pMe->m_IconTitle[4]     = IDS_MAIN_MENU_MESSAGES;
    pMe->m_IconTitle[5]     = IDS_MAIN_MENU_RECENTCALLS;
    pMe->m_IconTitle[6]     = IDS_MAIN_MENU_MUSICPLAYER;
    pMe->m_IconTitle[7]     = IDS_MAIN_MENU_APPLICATION;
    pMe->m_IconTitle[8]     = IDS_MAIN_MENU_SCHEDULER;
    pMe->m_IconTitle[9]     = IDS_MAIN_MENU_SETTINGS;
    pMe->m_IconTitle[10]    = IDS_MAIN_MENU_GAMES;
	#ifndef FEATURE_VERSION_1110W516
	pMe->m_IconTitle[11]    = IDS_MAIN_MENU_CALCULATOR;
	#else
    pMe->m_IconTitle[11]    = IDS_MAIN_MENU_CAMERA;
	#endif
#endif
#elif defined (FEATURE_DISP_240X320)
#ifdef FEATURE_VERSION_K212
	pMe->m_IconTitle[0]     = IDS_MAIN_MENU_MEDIAGALLERY;
    pMe->m_IconTitle[1]     = IDS_MAIN_MENU_CONTACTS;
    pMe->m_IconTitle[2]     = IDS_MAIN_MENU_UTK;
    pMe->m_IconTitle[3]     = IDS_MAIN_MENU_RECENTCALLS;
    pMe->m_IconTitle[4]     = IDS_MAIN_MENU_MESSAGES;
    pMe->m_IconTitle[5]     = IDS_MAIN_MENU_MULTIMEDIA;
    pMe->m_IconTitle[6]     = IDS_MAIN_MENU_MUSICPLAYER;
    pMe->m_IconTitle[7]     = IDS_MAIN_MENU_SETTINGS;
    pMe->m_IconTitle[8]     = IDS_MAIN_MENU_APPLICATION;
    pMe->m_IconTitle[9]     = IDS_MAIN_MENU_GAMES;
    pMe->m_IconTitle[10]    = IDS_MAIN_MENU_SCHEDULER;
    pMe->m_IconTitle[11]    = IDS_MAIN_MENU_CALCULATOR;
#elif defined(FEATURE_VERSION_IC241A_MMX)
#ifdef FEATURE_VERSION_IN50_MMX
    pMe->m_IconTitle[0]     = IDS_MAIN_MENU_MULTIMEDIA;	//IDS_MAIN_MENU_MUSICPLAYER;	
	pMe->m_IconTitle[1]     = IDS_PHONEBOOK;	
    pMe->m_IconTitle[2]     = IDS_MAIN_MENU_RECENTCALLS_C337;	//IDS_MAIN_MENU_RECENTCALLS;
    pMe->m_IconTitle[3]     = IDS_MAIN_MENU_SERVICES;	//IDS_MAIN_WAPBROWSER;
    pMe->m_IconTitle[4]     = IDS_MAIN_MENU_MESSAGES;
    pMe->m_IconTitle[5]     = IDS_MAIN_MENU_MUSICPLAYER;	//IDS_MAIN_MENU_MEDIAGALLERY;
	
    pMe->m_IconTitle[6]     = IDS_MAIN_MENU_FMRADIO;	//IDS_MAIN_MENU_MULTIMEDIA;		//m! STORE
    pMe->m_IconTitle[7]     = IDS_MAIN_MENU_ORGANIZER;	//IDS_MAIN_MENU_APPLICATION;
    pMe->m_IconTitle[8]     = IDS_MAIN_MENU_MEDIAGALLERY_C337;	//IDS_MAIN_MENU_APPLICATION;		//m! ZONE
	
    pMe->m_IconTitle[9]     = IDS_MAIN_MENU_CAMERA;	//IDS_MAIN_MENU_USERPROFILE;	//IDS_MAIN_MENU_APPLICATION;
    pMe->m_IconTitle[10]    = IDS_MAIN_MENU_GAMES;
    pMe->m_IconTitle[11]    = IDS_MAIN_MENU_SETTINGS;
#elif defined(FEATURE_VERSION_IN50A)
    pMe->m_IconTitle[0]     = IDS_MAIN_MENU_MULTIMEDIA;	//IDS_MAIN_MENU_MUSICPLAYER;	
	pMe->m_IconTitle[1]     = IDS_PHONEBOOK;	
    pMe->m_IconTitle[2]     = IDS_MAIN_MENU_RECENTCALLS_C337;	//IDS_MAIN_MENU_RECENTCALLS;
    pMe->m_IconTitle[3]     = IDS_MAIN_MENU_SERVICES;	//IDS_MAIN_WAPBROWSER;
    pMe->m_IconTitle[4]     = IDS_MAIN_MENU_MESSAGES;
    pMe->m_IconTitle[5]     = IDS_MAIN_MENU_CAMERA_LIST;    //IDS_MAIN_MENU_CAMERA;
	
    pMe->m_IconTitle[6]     = IDS_MAIN_MENU_FMRADIO;	//IDS_MAIN_MENU_MULTIMEDIA;		//m! STORE
    pMe->m_IconTitle[7]     = IDS_MAIN_MENU_ORGANIZER;	//IDS_MAIN_MENU_APPLICATION;
    pMe->m_IconTitle[8]     = IDS_MAIN_MENU_MEDIAGALLERY_C337;	//IDS_MAIN_MENU_APPLICATION;		//m! ZONE
	
    pMe->m_IconTitle[9]     = IDS_MAIN_MENU_USER_PROFILE;	//IDS_MAIN_MENU_USERPROFILE;	//IDS_MAIN_MENU_APPLICATION;
    pMe->m_IconTitle[10]    = IDS_MAIN_MENU_GAMES;
    pMe->m_IconTitle[11]    = IDS_MAIN_MENU_SETTINGS;	    
#else
    pMe->m_IconTitle[0]     = IDS_MAIN_MENU_MULTIMEDIA;	//IDS_MAIN_MENU_MUSICPLAYER;	
	pMe->m_IconTitle[1]     = IDS_PHONEBOOK;	
    pMe->m_IconTitle[2]     = IDS_MAIN_MENU_RECENTCALLS_C337;	//IDS_MAIN_MENU_RECENTCALLS;
    pMe->m_IconTitle[3]     = IDS_MAIN_MENU_SERVICES;	//IDS_MAIN_WAPBROWSER;
    pMe->m_IconTitle[4]     = IDS_MAIN_MENU_MESSAGES;
    pMe->m_IconTitle[5]     = IDS_MAIN_MENU_MSTORE;	//IDS_MAIN_MENU_MEDIAGALLERY;
	
    pMe->m_IconTitle[6]     = IDS_MAIN_MENU_FMRADIO;	//IDS_MAIN_MENU_MULTIMEDIA;		//m! STORE
    pMe->m_IconTitle[7]     = IDS_MAIN_MENU_ORGANIZER;	//IDS_MAIN_MENU_APPLICATION;
    pMe->m_IconTitle[8]     = IDS_MAIN_MENU_MEDIAGALLERY_C337;	//IDS_MAIN_MENU_APPLICATION;		//m! ZONE
	
    pMe->m_IconTitle[9]     = IDS_MAIN_MENU_USER_PROFILE;	//IDS_MAIN_MENU_USERPROFILE;	//IDS_MAIN_MENU_APPLICATION;
    pMe->m_IconTitle[10]    = IDS_MAIN_MENU_GAMES;
    pMe->m_IconTitle[11]    = IDS_MAIN_MENU_SETTINGS;	  
#endif    
#elif defined(FEATURE_VERSION_K212_HUALU)
	pMe->m_IconTitle[0]     = IDS_MAIN_MENU_CONTACTS;//IDS_MAIN_MENU_MEDIAGALLERY;
    pMe->m_IconTitle[1]     = IDS_MAIN_MENU_RECENTCALLS;//IDS_MAIN_MENU_CONTACTS;
    pMe->m_IconTitle[2]     = IDS_MAIN_MENU_MESSAGES;//IDS_MAIN_MENU_UTK;
    pMe->m_IconTitle[3]     = IDS_MAIN_MENU_MULTIMEDIA;//IDS_MAIN_MENU_RECENTCALLS;
    pMe->m_IconTitle[4]     = IDS_MAIN_MENU_UTK;//IDS_MAIN_MENU_MESSAGES;
    pMe->m_IconTitle[5]     = IDS_MAIN_MENU_APPLICATION;//IDS_MAIN_MENU_MULTIMEDIA;
    pMe->m_IconTitle[6]     = IDS_MAIN_MENU_SETTINGS;//IDS_MAIN_MENU_MUSICPLAYER;
    pMe->m_IconTitle[7]     = IDS_MAIN_MENU_USERPROFILE;
    pMe->m_IconTitle[8]     = IDS_MAIN_MENU_MEDIAGALLERY;
#else
    pMe->m_IconTitle[0]     = IDS_MAIN_MENU_MEDIAGALLERY;
    pMe->m_IconTitle[1]     = IDS_MAIN_MENU_CONTACTS;
    pMe->m_IconTitle[2]     = IDS_MAIN_MENU_UTK;
    pMe->m_IconTitle[3]     = IDS_MAIN_MENU_MULTIMEDIA;
    pMe->m_IconTitle[4]     = IDS_MAIN_MENU_MESSAGES;
    pMe->m_IconTitle[5]     = IDS_MAIN_MENU_RECENTCALLS;
    pMe->m_IconTitle[6]     = IDS_MAIN_MENU_MUSICPLAYER;
    pMe->m_IconTitle[7]     = IDS_MAIN_MENU_SETTINGS;
    pMe->m_IconTitle[8]     = IDS_MAIN_MENU_APPLICATION;
    pMe->m_IconTitle[9]     = IDS_MAIN_MENU_GAMES;
    pMe->m_IconTitle[10]    = IDS_MAIN_MENU_SCHEDULER;
    pMe->m_IconTitle[11]    = IDS_MAIN_MENU_CALCULATOR;
#endif
#elif defined (FEATURE_DISP_320X240)
    pMe->m_IconTitle[0]     = IDS_MAIN_MENU_MEDIAGALLERY;
    pMe->m_IconTitle[1]     = IDS_MAIN_MENU_CONTACTS;
    pMe->m_IconTitle[2]     = IDS_MAIN_MENU_UTK;
    pMe->m_IconTitle[3]     = IDS_MAIN_MENU_MULTIMEDIA;
    pMe->m_IconTitle[4]     = IDS_MAIN_MENU_MESSAGES;
    pMe->m_IconTitle[5]     = IDS_MAIN_MENU_RECENTCALLS;
    pMe->m_IconTitle[6]     = IDS_MAIN_MENU_MUSICPLAYER;
    pMe->m_IconTitle[7]     = IDS_MAIN_MENU_SETTINGS;
    pMe->m_IconTitle[8]     = IDS_MAIN_MENU_APPLICATION;
    pMe->m_IconTitle[9]     = IDS_MAIN_MENU_GAMES;
    pMe->m_IconTitle[10]    = IDS_MAIN_MENU_SCHEDULER;
    pMe->m_IconTitle[11]    = IDS_MAIN_MENU_CALCULATOR;
#endif
    return SUCCESS;
}

/*==============================================================================
函数:
    CMainMenu_FreeAppData

说明:
    释放 MainMenu Applet 相关资源。

参数:
    pMe [in]: 指向MainMenu Applet 对象结构的指针。该结构包含小程序的特定信息。

返回值:
    none

备注:

==============================================================================*/
static void CMainMenu_FreeAppData(MainMenu *pMe)
{
    if (NULL == pMe)
    {
        return;
    }

    pMe->m_eAppStatus = MAINMENU_STOP;
    if (NULL != pMe->m_pConfig)
    {
        ICONFIG_Release(pMe->m_pConfig);
        pMe->m_pConfig = NULL;
    }    
    
    if (pMe->m_pDisplay != NULL)
    {
        (void) IDISPLAY_Release(pMe->m_pDisplay);
        pMe->m_pDisplay = NULL;
    }
	if (pMe->m_pIAnn)
    {
        IANNUNCIATOR_Release(pMe->m_pIAnn);
		pMe->m_pIAnn = NULL;
    }

    if(pMe->m_pBacklight)
    {
        IBACKLIGHT_Release(pMe->m_pBacklight);
        pMe->m_pBacklight=NULL;
    }
    //释放图片资源
    {
        int i;
        
        if (pMe->m_pImageBg !=NULL)
        {
            (void) IIMAGE_Release(pMe->m_pImageBg);
            pMe->m_pImageBg = NULL;
        }
        
        #ifdef FEATURE_VERSION_K212_HUALU
        if(pMe->m_pImageIcon != NULL)
        {
            (void)IIMAGE_Release(pMe->m_pImageIcon);
             pMe->m_pImageIcon = NULL;
        }
        #else
        for(i=0;i<MAX_MATRIX_ITEMS;i++)
        {
            if(pMe->m_pImageIcon[i] != NULL)
            {
                (void)IIMAGE_Release(pMe->m_pImageIcon[i]);
                pMe->m_pImageIcon[i] = NULL;
            }
        } 
		    #endif
		

        if(pMe->m_pAnimate != NULL)
        {
            (void)IIMAGE_Release(pMe->m_pAnimate);
            pMe->m_pAnimate = NULL;
        }
    }  
}

/*=============================================================================
FUNCTION:  MainMenu_AddRef

DESCRIPTION: 

PARAMETERS:

RETURN VALUE:

COMMENTS:

SIDE EFFECTS:

SEE ALSO:

=============================================================================*/
static uint32  MainMenu_AddRef( IMainMenu *pi)
{
    return ( ++gMainMenu.referenceCounter);
}

/*=============================================================================
FUNCTION:  MainMenu_Release

DESCRIPTION: 

PARAMETERS:

RETURN VALUE:

COMMENTS:

SIDE EFFECTS:

SEE ALSO:

=============================================================================*/
static uint32  MainMenu_Release( IMainMenu *pi)
{
    register MainMenu *pMe = (MainMenu*)pi;
    if (pMe->referenceCounter == 0)
    {
        return 0;
    }
    
    if( --pMe->referenceCounter)
    {
        return pMe->referenceCounter;
    }


    CMainMenu_FreeAppData(pMe);

    (void) ISHELL_Release(pMe->m_pShell);
    (void) IMODULE_Release(pMe->m_pModule);
    return 0;
}

/*==============================================================================
函数:
    MainMenu_RunFSM

说明:
    运行有限状态机引擎。

参数:
    pMe [in]: 指向MainMenu Applet对象结构的指针。该结构包含小程序的特定信息。
       
返回值:
    无。

备注:

==============================================================================*/
static void MainMenu_RunFSM(MainMenu *pMe)
{
    NextFSMAction nextFSMAction = NFSMACTION_WAIT;
    for ( ; ; )
    {
        nextFSMAction = MainMenu_ProcessState(pMe);

        // 这里不需要复杂的状态机，因此为简化状态机设计这里仅用一个布尔变量来
        // 指示dlgResult是否应该被改写。在某些特殊场合（见状态机设计），这种作
        // 法，避免了不必要的复杂性，简化了设计。
        if (pMe->m_bDoNotOverwriteDlgResult)
        {
            // We allow only one iteration without touching this variable. So,
            // let's reset it back to indicate that we are going to overwrite the
            // result for all subsequent iterations.
            pMe->m_bDoNotOverwriteDlgResult = FALSE;
        }
        else
        {
            // Overwrite the dlgResult to its default value. The next dialog that
            // we are going to display will set and return the dlgResult based on
            // the users action.
            pMe->m_eDlgReturn = DLGRET_CREATE;
        }

        // If the state machine processing engine indicates that it created a
        // dialog and requesting us to exit this loop, then we quit.
        if (nextFSMAction == NFSMACTION_WAIT)
        {
            break;
        }
    }
}

/*=============================================================================
FUNCTION:  MainMenu_HandleEvent

DESCRIPTION: 

PARAMETERS:

RETURN VALUE:

COMMENTS:

SIDE EFFECTS:

SEE ALSO:

=============================================================================*/
static boolean MainMenu_HandleEvent( IMainMenu *pi,
    AEEEvent eCode,
    uint16  wParam,
    uint32 dwParam
)
{
    MainMenu *pMe = (MainMenu*)pi;
    AEEAppStart* as = NULL;
    switch ( eCode)
    {
        case EVT_APP_START:
#ifdef FEATURE_RANDOM_MENU_REND
            DisplayRend_Enable(TRUE);
#endif
            // 此事件dwParam为指针，不应为0
            if (dwParam == 0) 
            {
                return FALSE;
            }            
            as = (AEEAppStart*)dwParam;
   
            pMe->m_rc = as->rc;

            pMe->m_currState = MAINST_MAIN;
            pMe->m_eDlgReturn = DLGRET_CREATE;
            pMe->m_eAppStatus = MAINMENU_RUNNING;
            IANNUNCIATOR_SetFieldIsActiveEx(pMe->m_pIAnn,TRUE); 
            MainMenu_RunFSM(pMe);
            return TRUE;

        case EVT_APP_STOP:
            {
                int theFocus = 4;
                SetCursor( pMe, theFocus / 3, theFocus % 3);
                (void)ISHELL_CancelTimer( pMe->m_pShell, NULL, pMe);
                pMe->m_eAppStatus = MAINMENU_STOP;
#ifdef FEATURE_RANDOM_MENU_REND
                DisplayRend_Enable(FALSE);
#endif
                return TRUE;
            }

        case EVT_APP_SUSPEND:
            (void)ISHELL_CancelTimer(pMe->m_pShell, NULL, pMe);
            pMe->m_eAppStatus = MAINMENU_SUSPEND;
#ifdef FEATURE_RANDOM_MENU_REND
            DisplayRend_Push();
            DisplayRend_Start(FALSE);
#endif
            return TRUE;
        

        case EVT_APP_RESUME:
            {
                AEEAppStart* as = ( AEEAppStart*)dwParam;
                pMe->m_rc    = as->rc;
                pMe->m_eAppStatus = MAINMENU_RUNNING;
#ifdef FEATURE_RANDOM_MENU_REND
                DisplayRend_Push();
                DisplayRend_Start(TRUE);
#endif
                // 开始跑WMS状态机
                MainMenu_RunFSM(pMe); 
                return TRUE;
            }

        case EVT_DIALOG_INIT:
            pMe->m_pActiveIDlg = (IDialog*)dwParam;
            pMe->m_pActivedlgID = wParam;
            return MainMenu_RouteDialogEvt(pMe,eCode,wParam,dwParam);

        case EVT_DIALOG_START:
            return MainMenu_RouteDialogEvt(pMe,eCode,wParam,dwParam);

        case EVT_USER_REDRAW:
            return MainMenu_RouteDialogEvt(pMe,eCode,wParam,dwParam);

        case EVT_DIALOG_END:

            (void) MainMenu_RouteDialogEvt(pMe,eCode,wParam,dwParam);
            pMe->m_pActiveIDlg = NULL;
            pMe->m_pActivedlgID = 0;
            if(pMe->m_eAppStatus == MAINMENU_RUNNING)
            {
                MainMenu_RunFSM(pMe);
            }
            return TRUE;
        case EVT_KEY:
#if MIN_BREW_VERSION(3,0)
            // do not want to handle au
            if (((dwParam & KB_AUTOREPEAT) != 0) &&
                (wParam != AVK_UP) &&
                (wParam != AVK_DOWN) &&
                (wParam != AVK_LEFT) &&
                (wParam != AVK_RIGHT))
            {
                return TRUE;
            }
#endif
            return MainMenu_RouteDialogEvt(pMe,eCode,wParam,dwParam);  
#if defined (FEATURE_VERSION_N021)||defined(FEATURE_VERSION_W0216A)
		case EVT_KEY_HELD:
			{
				if(wParam == AVK_INFO)
				{
					(void) ISHELL_PostEvent(pMe->m_pShell,
                                        AEECLSID_CORE_APP,
                                        EVT_USER,
                                        1,
                                        0);
                    ISHELL_CloseApplet(pMe->m_pShell,TRUE);
				}
			}
			break;
#endif
        case EVT_KEY_PRESS:
        case EVT_KEY_RELEASE:
        case EVT_COMMAND:
            return MainMenu_RouteDialogEvt(pMe,eCode,wParam,dwParam); 

        default:
            return MainMenu_RouteDialogEvt(pMe,eCode,wParam,dwParam);
    }
}

void Mainmenu_KeypadLock(boolean block)
{
   gbMainmenuLock = block;
}

/*==============================================================================
函数:
    MainMenu_ProcessState

说明:
    根据 pMe->m_currState, 调用相应的状态处理函数。

参数:
    pMe [in]: 指向MainMenu Applet对象结构的指针。该结构包含小程序的特定信息。

返回值:
    NFSMACTION_CONTINUE: 指示不停状态机。
    NFSMACTION_WAIT: 指示停止状态机。

备注:

==============================================================================*/
NextFSMAction MainMenu_ProcessState(MainMenu *pMe)
{
    NextFSMAction retVal = NFSMACTION_WAIT;
    if (NULL == pMe)
    {
        return retVal;
    }
    switch(pMe->m_currState)
    {
        case MAINST_MAIN:
            return MAINST_MAIN_Handler(pMe);
        case MAINST_EXIT:
            return MAINST_EXIT_Handler(pMe);  
        default:
            break;

    }
    return retVal;
}

/*==============================================================================
函数:
    MAINST_MAIN_Handler

说明:
    MAINST_MAIN 状态处理函数。

参数:
    pMe [in]: 指向MAINMENU Applet对象结构的指针。该结构包含小程序的特定信息。

返回值:
    NFSMACTION_CONTINUE: 指示不停状态机。
    NFSMACTION_WAIT: 指示停止状态机。

备注:

==============================================================================*/
static NextFSMAction MAINST_MAIN_Handler(MainMenu *pMe)
{
    if (NULL == pMe)
    {
        return NFSMACTION_WAIT;
    }
    switch (pMe->m_eDlgReturn)
    {
        // 进入主界面
        case DLGRET_CREATE:
            {
                MainMenu_ShowDialog(pMe, IDD_MAIN_MENU);
            }
            return NFSMACTION_WAIT;
            
        case DLGRET_CANCELED:
            MOVE_TO_STATE(MAINST_EXIT)
            return NFSMACTION_CONTINUE;

        default:
            MOVE_TO_STATE(MAINST_EXIT)
            return NFSMACTION_CONTINUE;
    }
}

/*==============================================================================
函数:
    MAINST_EXIT_Handler

说明:
    MAINST_EXIT 状态处理函数。

参数:
    pMe [in]: 指向MAINMENU  Applet对象结构的指针。该结构包含小程序的特定信息。

返回值:
    NFSMACTION_CONTINUE: 指示不停状态机。
    NFSMACTION_WAIT: 指示停止状态机。

备注:

==============================================================================*/
static NextFSMAction MAINST_EXIT_Handler(MainMenu *pMe)
{
    (void) ISHELL_CloseApplet(pMe->m_pShell, FALSE);
    return NFSMACTION_WAIT;
} 

/*==============================================================================
函数:
       MainMenu_ShowDialog

说明:
    函数由状态处理函数用来唤起相应对话框资源ID的对话框。

参数:
    pMe [in]: 指向 MAINMENU Applet 对象结构的指针。该结构包含小程序的特定信息。
    dlgResId [in]: 对话框资源ID，函数根据该ID创建对话框。

返回值:
    none

备注:

==============================================================================*/
void MainMenu_ShowDialog(MainMenu  *pMe,  uint16 dlgResId)
{
    int nRet;

    // 每次最多打开一个对话框
    if (ISHELL_GetActiveDialog(pMe->m_pShell) != NULL)
    {
        // 如果当前已经有对话框被打开，直接返回
        return;
    }
   
    if (NULL != pMe->m_pDisplay)
    {
        AEEDeviceInfo di={0,};
#if defined (FEATURE_VERSION_H19C)
        if(pMe->m_pIAnn != NULL)
        {
            if (dlgResId == IDD_MAIN_MENU)
            {
                IANNUNCIATOR_SetHasTitleText(pMe->m_pIAnn, FALSE);
            }
            else
            {
                IANNUNCIATOR_SetHasTitleText(pMe->m_pIAnn, TRUE);
            }
            IANNUNCIATOR_Redraw(pMe->m_pIAnn);
        }
        (void)IDISPLAY_SetPrefs(pMe->m_pDisplay, "a:1", STRLEN("a:1"));
#else
        if (dlgResId == IDD_MAIN_MENU)
        {
            (void)IDISPLAY_SetPrefs(pMe->m_pDisplay, "a:0", STRLEN("a:0"));
        }
        else
        {
            (void)IDISPLAY_SetPrefs(pMe->m_pDisplay, "a:1", STRLEN("a:1"));
        }
#endif
        ISHELL_GetDeviceInfo(pMe->m_pShell, &di);
#if defined (FEATURE_VERSION_C01)|| defined(FEATURE_VERSION_VG68)|| defined (FEATURE_VERSION_SKY)||defined(FEATURE_VERSION_K202_LM129C) || defined(FEATURE_VERSION_W021_C11)
        pMe->m_rc.x = 0;
        pMe->m_rc.y = 0;
        
        pMe->m_rc.dx = di.cxScreen;
        pMe->m_rc.dy = di.cyScreen;
#else
        pMe->m_rc.dx = di.cxScreen;
        pMe->m_rc.dy = di.cyScreen;
#endif        
        IDISPLAY_SetClipRect(pMe->m_pDisplay, &pMe->m_rc);
        CalculateScreenParameters(pMe);
    }
    
    nRet = ISHELL_CreateDialog(pMe->m_pShell,MAINMENU_RES_FILE_LANG,dlgResId,NULL);
    if (nRet != SUCCESS)
    {
    }
}


/*==============================================================================
函数:
       MainMenu_RouteDialogEvt
说明:
       函数将BREW事件路由给当前活动对话框的事件处理函数。

参数:
       pMe [In]: 指向SettingMenu Applet对象结构的指针,该结构包含小程序的特定信息.
       eCode [in]：事件代码。
       wParam [in]：与事件eCode关联的数据。
       dwParam [in]：与事件eCode关联的数据。

返回值:
       TRUE：传入事件被处理。
       FALSE：传入事件没被处理。

备注:

==============================================================================*/
boolean MainMenu_RouteDialogEvt(MainMenu *pMe,
    AEEEvent    eCode,
    uint16  wParam,
    uint32 dwParam
)

{
    if (NULL == pMe)
    {
        return FALSE;
    }

    if (NULL == pMe->m_pActiveIDlg)
    {
        return FALSE;
    }    
    switch( pMe->m_pActivedlgID)
    {
        case IDD_MAIN_MENU:
            return MainMenu_IconMenuHandler(pMe, eCode, wParam, dwParam);
            
        default:
            return FALSE;
    }
}

/*=============================================================================
FUNCTION:  MainMenu_IconMenuHandler

DESCRIPTION: 12宫格菜单

PARAMETERS:

=============================================================================*/
static boolean MainMenu_IconMenuHandler(MainMenu *pMe, AEEEvent eCode, uint16 wParam, uint32 dwParam)
{
    PARAM_NOT_REF( dwParam)
        
    switch ( eCode)
    {
        case EVT_DIALOG_INIT:			
            IDIALOG_SetProperties((IDialog *)dwParam, DLG_NOT_REDRAW_AFTER_START);
            return TRUE;

        case EVT_DIALOG_START:			
#ifdef FEATURE_RANDOM_MENU_COLOR
            (void)OEM_GetConfig(CFGI_MENU_BGCOLOR, &pMe->m_nBgColor, sizeof(pMe->m_nBgColor));
#endif
            if(pMe->m_pImageBg == NULL)
            {
#ifdef FEATURE_RANDOM_MENU_COLOR
                (void)OEM_GetConfig(CFGI_RANDOM_MENU, (void*)&pMe->m_nRandomMenu, sizeof(pMe->m_nRandomMenu));
				#if 0
                if(pMe->m_nRandomMenu != 0)
                {
                    pMe->m_pImageBg = ISHELL_LoadResImage(pMe->m_pShell, AEE_APPSCOMMONRES_IMAGESFILE, IDI_MENU_BACKGROUND_TRANS);
                }
                else
				#endif 
#endif
                {
#if defined (FEATURE_VERSION_H19C) || defined (FEATURE_VERSION_C01)|| defined (FEATURE_VERSION_SKY)|| defined(FEATURE_VERSION_W021_C11)
                    pMe->m_pImageBg = ISHELL_LoadImage(pMe->m_pShell,ICON_ANI[0]);
//Add by pyuangui 20121221
#elif defined(FEATURE_VERSION_W317A)
                    pMe->m_pImageBg = ISHELL_LoadResImage(pMe->m_pShell, AEE_APPSCOMMONRES_IMAGESFILE, IDB_MAINMENU_BG);
//Add by pyuangui 20121221
#else
#ifndef FEATURE_LOW_MEM_BIGFONT
                    pMe->m_pImageBg = ISHELL_LoadResImage(pMe->m_pShell, AEE_APPSCOMMONRES_IMAGESFILE, IDB_BACKGROUND);//modi by yangdecai
#endif            
#endif
                }
            }
			AEE_CancelTimer(Main_keypadtimer,pMe);
			AEE_SetTimer(5*1000,Main_keypadtimer,pMe);
            (void) ISHELL_PostEvent( pMe->m_pShell,
                                     AEECLSID_MAIN_MENU,
                                     EVT_USER_REDRAW,
                                     0,
                                     0);
            return TRUE;


        case EVT_USER_REDRAW:
            // 初始整个背景及全部初始图标
           // ERR("EVT_USER_REDRAW:::::::::::1111111111",0,0,0);
            {
                uint32  dwTotal = 0;
    			uint32 free = 0;
            	DrawMatrix(pMe);
            	// 绘制聚焦过程动画
            	MoveCursorTo(pMe, pMe->m_nRow, pMe->m_nColumn);
    			GETFSFREE(&dwTotal);
    			free = GETRAMFREE(NULL,NULL);
    			MSG_FATAL("MainMenu_IconMenuHandler dwTotal======%d,free====%d",dwTotal,free,0);
            	return TRUE;
            }
            
        case EVT_DIALOG_END:
            {
                int i;
                
                if (pMe->m_pImageBg !=NULL)
                {
                    (void) IIMAGE_Release(pMe->m_pImageBg);
                    pMe->m_pImageBg = NULL;
                }

                #ifdef FEATURE_VERSION_K212_HUALU
                if(pMe->m_pImageIcon != NULL)
                {
                    (void)IIMAGE_Release(pMe->m_pImageIcon);
                     pMe->m_pImageIcon = NULL;
                }
                #else
                for(i=0;i<MAX_MATRIX_ITEMS;i++)
                {
                    if(pMe->m_pImageIcon[i] != NULL)
                    {
                        (void)IIMAGE_Release(pMe->m_pImageIcon[i]);
                        pMe->m_pImageIcon[i] = NULL;
                    }
                } 
                #endif
                
                if(pMe->m_pAnimate != NULL)
                {
                    (void)IIMAGE_Release(pMe->m_pAnimate);
                    pMe->m_pAnimate = NULL;
                }
                ISHELL_CancelTimer(pMe->m_pShell, NULL, (void**)pMe);
            }
            return TRUE;

        case EVT_KEY_HELD:
        {                
        #if defined(FEATURE_TORCH_KEY_INFO)
            int nFocus = pMe->m_nRow * MAX_MATRIX_COLS + pMe->m_nColumn;      
        
            if ((nFocus == 3) || (nFocus == 6))
            {
                return TRUE;
            }
            
           if((AVKType)wParam == AVK_INFO)
            {
                boolean TorchOn = FALSE;
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
                ISHELL_CloseApplet(pMe->m_pShell, TRUE); 
                return TRUE;
            }
        #endif
	 #if defined(FEATURE_TORCH_KEY_SELECT) //xxzhen
           if((AVKType)wParam == AVK_SELECT)
            {
                boolean TorchOn = FALSE;
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
                ISHELL_CloseApplet(pMe->m_pShell, TRUE); 
                return TRUE;
            }
        #endif

            return TRUE;
        }
        case EVT_KEY:
            //ISHELL_CancelTimer(pMe->m_pShell, NULL, (void**)pMe);
            switch( wParam)
            {
                case AVK_CLR:
                   // ERR("AVK_CLR:::::::::::::::::::!111111111111",0,0,0);
                     CLOSE_DIALOG(DLGRET_CANCELED)
                    return TRUE;
                    
                case AVK_UP:
                    {
                        int nRow = pMe->m_nRow;
                        int nCol = pMe->m_nColumn;
                        
                        if (nRow>0)
                        {
                            nRow--;
                        }
                        else
                        {
                            if (nCol>0)
                            {
                                nCol--;
                            }
                            else
                            {
                                nCol = MAX_MATRIX_COLS-1;
                            }
                            nRow = MAX_MATRIX_ROWS-1;
                        }
                        MoveCursorTo(pMe, nRow, nCol);
                    }
                    return TRUE;
                    
                case AVK_DOWN:
                    {
                        int nRow = pMe->m_nRow;
                        int nCol = pMe->m_nColumn;
                        
                        if (nRow<MAX_MATRIX_ROWS-1)
                        {
                            nRow++;
                        }
                        else
                        {
                            if (nCol<MAX_MATRIX_COLS-1)
                            {
                                nCol++;
                            }
                            else
                            {
                                nCol = 0;
                            }
                            nRow = 0;
                        }
                        MoveCursorTo(pMe, nRow, nCol);
                    }
                    return TRUE;
                    
                case AVK_LEFT:
                    {
                        int nRow = pMe->m_nRow;
                        int nCol = pMe->m_nColumn;
                        
                        if (nCol>0)
                        {
                            nCol--;
                        }
                        else
                        {
                            if (nRow>0)
                            {
                                nRow--;
                            }
                            else
                            {
                                nRow = MAX_MATRIX_ROWS-1;
                            }
                            nCol = MAX_MATRIX_COLS-1;
                        }
                        MoveCursorTo(pMe, nRow, nCol);
                    }
                    return TRUE;
                    
                case AVK_RIGHT:
                    {
                        int nRow = pMe->m_nRow;
                        int nCol = pMe->m_nColumn;
                        
                        if (nCol<MAX_MATRIX_COLS-1)
                        {
                            nCol++;
                        }
                        else
                        {
                            if (nRow<MAX_MATRIX_ROWS-1)
                            {
                                nRow++;
                            }
                            else
                            {
                                nRow = 0;
                            }
                            nCol = 0;
                        }
                        MoveCursorTo(pMe, nRow, nCol);
                    }
                    return TRUE;
                    
                case AVK_SELECT:
                case AVK_INFO:
                    {
                        int Focus = pMe->m_nRow * MAX_MATRIX_COLS + pMe->m_nColumn;
                        StartApplet(pMe, pMe->m_IconTitle[Focus]);
                    }
                    return TRUE;
                    
                case AVK_1:
                case AVK_2:
                case AVK_3:
                case AVK_4:
                case AVK_5:
                case AVK_6:
                case AVK_7:
                case AVK_8:
                case AVK_9:
                    StartApplet(pMe, pMe->m_IconTitle[wParam - AVK_1]);
                    return TRUE;
                    
                case AVK_STAR:
                    if(gbMainmenuLock)
                    {
                        OEMKeyguard_SetState(TRUE);
                        ISHELL_CloseApplet(pMe->m_pShell, TRUE); 
                    }
                    else
                    {
                     StartApplet(pMe, pMe->m_IconTitle[9]);
                    }
                    return TRUE;

                case AVK_0:
                    StartApplet(pMe, pMe->m_IconTitle[10]);
                    return TRUE;
                    
                case AVK_POUND:
                    StartApplet(pMe, pMe->m_IconTitle[11]);
                    return TRUE;

                default:
                    break;
            }

            return TRUE;
            
        default:
            break;
    }

    return FALSE;
}

static void Main_keypadtimer(void *pUser)
{
    gbMainmenuLock =FALSE;
}



/*=============================================================================
FUNCTION:  CalculateScreenParameters

DESCRIPTION: 计算焦点图片以及默认图片的坐标

=============================================================================*/
static void CalculateScreenParameters(MainMenu *pMe)
{
    AEEImageInfo    imageInfoIcon;
    uint16          iconSpaceHorizontal = 0;
    uint16          iconSpaceVertical   = 0;
    uint8           i                   = 0;

    //计算默认图片的坐标
    imageInfoIcon.cx = ICON_WIDTH;
    imageInfoIcon.cy = ICON_HEIGHT;

    /* icon size in all might be larger than screen*/
#ifdef FEATURE_VERSION_C01   
    pMe->m_Icondefault_Pt[0].x = 0;
    pMe->m_Icondefault_Pt[0].y = 23;
    pMe->m_Icondefault_Pt[1].x = 42;
    pMe->m_Icondefault_Pt[1].y = 23;
    pMe->m_Icondefault_Pt[2].x = 86;
    pMe->m_Icondefault_Pt[2].y = 23;

    pMe->m_Icondefault_Pt[3].x = 0;
    pMe->m_Icondefault_Pt[3].y = 64;
    pMe->m_Icondefault_Pt[4].x = 42;
    pMe->m_Icondefault_Pt[4].y = 64;
    pMe->m_Icondefault_Pt[5].x = 86;
    pMe->m_Icondefault_Pt[5].y = 64;

    pMe->m_Icondefault_Pt[6].x = 0;
    pMe->m_Icondefault_Pt[6].y = 104;
    pMe->m_Icondefault_Pt[7].x = 42;
    pMe->m_Icondefault_Pt[7].y = 104;
    pMe->m_Icondefault_Pt[8].x = 86;
    pMe->m_Icondefault_Pt[8].y = 104;  

    //计算焦点图片的坐标
    pMe->m_IconFocus_Pt[0].x = 0;
    pMe->m_IconFocus_Pt[0].y = 18;
    pMe->m_IconFocus_Pt[1].x = 43;
    pMe->m_IconFocus_Pt[1].y = 18;
    pMe->m_IconFocus_Pt[2].x = 86;
    pMe->m_IconFocus_Pt[2].y = 18;    

    pMe->m_IconFocus_Pt[3].x = 0;
    pMe->m_IconFocus_Pt[3].y = 59;
    pMe->m_IconFocus_Pt[4].x = 43;
    pMe->m_IconFocus_Pt[4].y = 59;
    pMe->m_IconFocus_Pt[5].x = 86;
    pMe->m_IconFocus_Pt[5].y = 59;    

    pMe->m_IconFocus_Pt[6].x = 0;
    pMe->m_IconFocus_Pt[6].y = 100;
    pMe->m_IconFocus_Pt[7].x = 43;
    pMe->m_IconFocus_Pt[7].y = 100;
    pMe->m_IconFocus_Pt[8].x = 86;
    pMe->m_IconFocus_Pt[8].y = 100;      
#else

#ifdef FEATURE_VERSION_H19C   
    iconSpaceHorizontal = 12;
    iconSpaceVertical = 2;
#else
    if(pMe->m_rc.dx > imageInfoIcon.cx * MAX_MATRIX_COLS)
    {
        iconSpaceHorizontal = (pMe->m_rc.dx - imageInfoIcon.cx * MAX_MATRIX_COLS) / (MAX_MATRIX_COLS+1);
    }
    else
    {
        iconSpaceHorizontal = 0;
    }

    if(pMe->m_rc.dy > TITLEBAR_HEIGHT + imageInfoIcon.cy * MAX_MATRIX_ROWS)
    {
        iconSpaceVertical   = (pMe->m_rc.dy - TITLEBAR_HEIGHT*2 - imageInfoIcon.cy * MAX_MATRIX_ROWS) / (MAX_MATRIX_ROWS+1);
    }
    else
    {
        iconSpaceVertical = 0;
    }
#endif
	MSG_FATAL("iconSpaceHorizontal====%d,iconSpaceVertical===%d",iconSpaceHorizontal,iconSpaceVertical,0);

    for( i = 0; i < MAX_MATRIX_ITEMS; i ++)
    {
        pMe->m_Icondefault_Pt[i].x = iconSpaceHorizontal +
            ( imageInfoIcon.cx + iconSpaceHorizontal) * ( i % MAX_MATRIX_COLS);
        
        pMe->m_Icondefault_Pt[i].y = TITLEBAR_HEIGHT + iconSpaceVertical +
            ( imageInfoIcon.cy + iconSpaceVertical) * ( i / MAX_MATRIX_COLS);

#ifdef FEATURE_VERSION_IN50_MMX
        pMe->m_Icondefault_Pt[i].x += 10;
        //pMe->m_Icondefault_Pt[i].y -= 5;        
#endif

        //计算焦点图片的坐标
        pMe->m_IconFocus_Pt[i].x = pMe->m_Icondefault_Pt[i].x - (ICON_ANIMATED_WIDTH - imageInfoIcon.cx)/2;

        pMe->m_IconFocus_Pt[i].y = pMe->m_Icondefault_Pt[i].y - (ICON_ANIMATED_HEIGHT- imageInfoIcon.cy)/2;
        //end added

        MSG_FATAL("***zzg m_Icondefault_Pt[%d].x=%d, m_Icondefault_Pt.y=%d", i, pMe->m_Icondefault_Pt[i].x, pMe->m_Icondefault_Pt[i].y);
        MSG_FATAL("***zzg m_IconFocus_Pt[%d].x=%d, m_IconFocus_Pt.y=%d", i, pMe->m_IconFocus_Pt[i].x, pMe->m_IconFocus_Pt[i].y);
    }
#endif    
}

static void MainMenu_DrawBackGround(MainMenu *pMe, AEERect *pRect)
{
#ifdef FEATURE_RANDOM_MENU_COLOR
    IDISPLAY_FillRect(pMe->m_pDisplay, pRect, pMe->m_nBgColor);
    if(pMe->m_nRandomMenu != 0)
    {
        Appscommon_ResetBackground(pMe->m_pDisplay, 
                                                    pMe->m_pImageBg, 
                                                    pMe->m_nBgColor, 
                                                    pRect, 
                                                    0, 
                                                    0);
    }
    else
#endif
    {
    	MSG_FATAL("pRect.dx  ===%d,,,dy===%d,=====x==%d",pRect->dx,pRect->dy,pRect->x);
        #ifdef FEATURE_VERSION_K212_HUALU
        #ifdef FEATURE_VERSION_K212_VHOPE
        Appscommon_ResetBackground(pMe->m_pDisplay, 
                                                    pMe->m_pImageBg, 
                                                    RGB_BLACK, 
                                                    pRect, 
                                                    0, 
                                                    0);
        #else
        Appscommon_ResetBackground(pMe->m_pDisplay, 
                                                    pMe->m_pImageBg, 
                                                    RGB_WHITE, 
                                                    pRect, 
                                                    0, 
                                                    0);
        #endif
        #else
        Appscommon_ResetBackground(pMe->m_pDisplay, 
                                                    pMe->m_pImageBg, 
                                                    APPSCOMMON_BG_COLOR, 
                                                    pRect, 
                                                    0, 
                                                    0);
        #endif
    } 
}

/*=============================================================================
FUNCTION:  DrawMatrix

DESCRIPTION: // 初始整个背景及全部初始图标

=============================================================================*/
static void DrawMatrix(MainMenu *pMe)
{
    int i = 0;
	  BottomBar_Param_type BarParam={0};//wlh add
    nv_language_enum_type language;
    if (NULL == pMe)
    {
        return;
    }
    //draw bg image
    MainMenu_DrawBackGround(pMe, &pMe->m_rc);
    (void) ICONFIG_GetItem(pMe->m_pConfig,
                                   CFGI_LANGUAGE_SELECTION,
                                   &language,
                                   sizeof(language));
#ifndef FEATURE_VERSION_H19C  
#ifndef FEATURE_VERSION_C01 
#ifndef FEATURE_VERSION_SKY
#ifndef FEATURE_VERSION_W021_C11
    //Draw icon
    for (i = 0; i < MAX_MATRIX_ITEMS; i ++)
    {
        #ifdef FEATURE_VERSION_K212_HUALU
        if(language == NV_LANGUAGE_ENGLISH)
        {

                pMe->m_pImageIcon = ISHELL_LoadImage(pMe->m_pShell,
                                                        ICON_EN_ANI[i]);
                IIMAGE_Draw(pMe->m_pImageIcon,
                            pMe->m_Icondefault_Pt[i].x,
                            pMe->m_Icondefault_Pt[i].y);
        }
        else
        {

                pMe->m_pImageIcon = ISHELL_LoadImage(pMe->m_pShell,
                                                        ICON_ANI[i]);
                IIMAGE_Draw(pMe->m_pImageIcon,
                            pMe->m_Icondefault_Pt[i].x,
                            pMe->m_Icondefault_Pt[i].y);

        }
        if(pMe->m_pImageIcon != NULL)
        {
            (void)IIMAGE_Release(pMe->m_pImageIcon);
             pMe->m_pImageIcon = NULL;
        }
        #else
        if (pMe->m_pImageIcon[i] == NULL)
        {
            pMe->m_pImageIcon[i] = ISHELL_LoadImage(pMe->m_pShell,
                                                    ICON_ANI[i]);
        }

        if (pMe->m_pImageIcon[i] != NULL)
        {
            IIMAGE_Draw(pMe->m_pImageIcon[i],
                        pMe->m_Icondefault_Pt[i].x,
                        pMe->m_Icondefault_Pt[i].y);
        }
        #endif

    }  
#endif    
#endif
#endif
#endif
    BarParam.eBBarType = BTBAR_SELECT_BACK;
#ifdef FEATURE_VERSION_W021_C11
    DrawBottomBar_Ex(pMe->m_pShell,pMe->m_pDisplay, BTBAR_SELECT_BACK);//wlh 20090412 add
#else
	DrawBottomBar(pMe->m_pDisplay, &BarParam);//wlh 20090412 add
#endif
}

/*=============================================================================
FUNCTION:  DrawFocusIcon

DESCRIPTION: 

PARAMETERS:

RETURN VALUE:

COMMENTS:


=============================================================================*/
static void DrawFocusIcon(MainMenu *pMe)
{
    int theFocus = pMe->m_nRow * MAX_MATRIX_COLS + pMe->m_nColumn;
    TitleBar_Param_type  titleBarParms;
    
    if (NULL == pMe)
    {
        return;
    }
            
    MEMSET( &titleBarParms, 0, sizeof( TitleBar_Param_type));
    
    titleBarParms.dwAlignFlags  = IDF_TEXT_TRANSPARENT | IDF_ALIGN_CENTER | IDF_ALIGN_MIDDLE;
    STRCPY( titleBarParms.strTitleResFile, MAINMENU_RES_FILE_LANG);
	titleBarParms.nTitleResID   = pMe->m_IconTitle[theFocus];
    DrawTitleBar(pMe->m_pDisplay, &titleBarParms);
    
 #if defined (FEATURE_VERSION_H19C) || defined (FEATURE_VERSION_C01) || defined(FEATURE_VERSION_SKY)|| defined(FEATURE_VERSION_IN50_MMX)
    if(pMe->m_pAnimate != NULL)
    {
        IIMAGE_Stop(pMe->m_pAnimate);
        IIMAGE_Release(pMe->m_pAnimate);
        pMe->m_pAnimate = NULL;
    }
#endif    
    if(pMe->m_pAnimate == NULL)
    {
    		nv_language_enum_type language;
    	  (void) ICONFIG_GetItem(pMe->m_pConfig,
                                   CFGI_LANGUAGE_SELECTION,
                                   &language,
                                   sizeof(language));
        #ifdef FEATURE_VERSION_K212_HUALU
        if(language == NV_LANGUAGE_ENGLISH)
        {
            pMe->m_pAnimate = ISHELL_LoadImage(pMe->m_pShell, ICON_EN_ANI_1[theFocus]);
        }
        else
        #endif
        {
        	  pMe->m_pAnimate = ISHELL_LoadImage(pMe->m_pShell, ICON_ANI_1[theFocus]);
        }
    }

	if( pMe->m_pAnimate != NULL)
    {
    		
#if defined (FEATURE_VERSION_H19C) || defined (FEATURE_VERSION_C01)|| defined(FEATURE_VERSION_IN50_MMX)
        IIMAGE_Start(pMe->m_pAnimate,
                     pMe->m_IconFocus_Pt[theFocus].x, 
                     pMe->m_IconFocus_Pt[theFocus].y);
#elif defined(FEATURE_VERSION_SKY)
{
        AEERect rc;
        rc.x  = 0;
        rc.y  = 22;
        rc.dx = 176;
        rc.dy = 176;
        Appscommon_ResetBackground_Start(pMe->m_pDisplay,  pMe->m_pAnimate, 
                                    APPSCOMMON_BG_COLOR, &rc, 0, 0);
}

#else        
		IIMAGE_Draw(pMe->m_pAnimate,
                    pMe->m_IconFocus_Pt[theFocus].x, 
                    pMe->m_IconFocus_Pt[theFocus].y);
	    IIMAGE_Release(pMe->m_pAnimate);
        pMe->m_pAnimate = NULL;
#endif     
        
	    IDISPLAY_UpdateEx(pMe->m_pDisplay, TRUE);
	}
}

/*=============================================================================
FUNCTION:  MoveCursorTo

DESCRIPTION: 

PARAMETERS:

RETURN VALUE:

COMMENTS:

SIDE EFFECTS:

SEE ALSO:

=============================================================================*/
static void MoveCursorTo(MainMenu *pMe, int row, int column)
{
#ifndef FEATURE_VERSION_H19C
#ifndef FEATURE_VERSION_C01
#ifndef FEATURE_VERSION_SKY
    int theFocus = pMe->m_nRow * MAX_MATRIX_COLS + pMe->m_nColumn;
    AEERect rect;
    nv_language_enum_type language;
	//uint32  dwTotal = 0;
	//uint32 free = 0;
	//GETFSFREE(&dwTotal);
	//free = GETRAMFREE(NULL,NULL);
	//MSG_FATAL("MoveCursorTodwTotal======%d,free===%d",dwTotal,free,0);
    (void) ICONFIG_GetItem(pMe->m_pConfig,
                                   CFGI_LANGUAGE_SELECTION,
                                   &language,
                                   sizeof(language));
    // 绘制聚焦后矩阵初始界面
    SETAEERECT(&rect, pMe->m_IconFocus_Pt[theFocus].x, 
                      pMe->m_IconFocus_Pt[theFocus].y, 
                      ICON_ANIMATED_WIDTH, 
                      ICON_ANIMATED_HEIGHT);
	  MSG_FATAL("***pyg***pMe->m_IconFocus_Pt[theFocus].x=%d---pMe->m_IconFocus_Pt[theFocus].y=%d",pMe->m_IconFocus_Pt[theFocus].x,pMe->m_IconFocus_Pt[theFocus].y,0);
    MSG_FATAL("***pyg***ICON_ANIMATED_WIDTH=%d---ICON_ANIMATED_HEIGHT=%d",ICON_ANIMATED_WIDTH,ICON_ANIMATED_HEIGHT,0);
    MainMenu_DrawBackGround(pMe, &rect);
    
    #ifdef FEATURE_VERSION_K212_HUALU
    if(language == NV_LANGUAGE_ENGLISH)
    {
        pMe->m_pImageIcon = ISHELL_LoadImage(pMe->m_pShell,
                                                        ICON_EN_ANI[theFocus]);
        IIMAGE_Draw(pMe->m_pImageIcon,
                    pMe->m_Icondefault_Pt[theFocus].x, 
                    pMe->m_Icondefault_Pt[theFocus].y);
    }
    else
    {
        pMe->m_pImageIcon = ISHELL_LoadImage(pMe->m_pShell,
                                                        ICON_ANI[theFocus]);
        IIMAGE_Draw(pMe->m_pImageIcon,
                    pMe->m_Icondefault_Pt[theFocus].x, 
                    pMe->m_Icondefault_Pt[theFocus].y);
    }
    if(pMe->m_pImageIcon != NULL)
    {
        (void)IIMAGE_Release(pMe->m_pImageIcon);
         pMe->m_pImageIcon = NULL;
    }
    #else
    if (pMe->m_pImageIcon[theFocus])
    {
        IIMAGE_Draw(pMe->m_pImageIcon[theFocus],
                    pMe->m_Icondefault_Pt[theFocus].x, 
                    pMe->m_Icondefault_Pt[theFocus].y);
    }
    #endif
#endif
#endif
#endif
    // 开始聚焦动画过程
    SetCursor(pMe, row, column);
    DrawFocusIcon(pMe);
}
/*=============================================================================
FUNCTION:  SetCursor

DESCRIPTION: 

PARAMETERS:

RETURN VALUE:

COMMENTS:

SIDE EFFECTS:

SEE ALSO:

=============================================================================*/
static void SetCursor(MainMenu *pMe, int row, int column)
{
    pMe->m_nRow        = row;
    pMe->m_nColumn     = column;
    
    if(pMe->m_pAnimate != NULL)
    {
        IImage_Release(pMe->m_pAnimate);
        pMe->m_pAnimate  = NULL;
    }
}

/*=============================================================================
FUNCTION:  StartApplet

DESCRIPTION:  启动applet

PARAMETERS:  如果APPLET 有变动，只需改动次函数

=============================================================================*/
static int StartApplet(MainMenu *pMe, int i)
{
    int Result = EUNSUPPORTED;
	//uint32  dwTotal = 0;
	//uint32 free = 0;
	//GETFSFREE(&dwTotal);
	//free = GETRAMFREE(NULL,NULL);
	//MSG_FATAL("dwTotal======%d,free===%d",dwTotal,free,0);

    
#ifdef FEATURE_VERSION_H19C  
    if(pMe->m_pIAnn != NULL)
    {
        IANNUNCIATOR_SetHasTitleText(pMe->m_pIAnn, TRUE);
    }
#endif
		#ifdef FEATURE_VERSION_K202_LM129C //xxzhen
		  if(pMe->m_pIAnn != NULL)
          {
              IANNUNCIATOR_SetHasTitleText(pMe->m_pIAnn, TRUE);
          }
		#endif

    switch(i)
	{
#ifdef FEATURE_VERSION_W208S
	case IDS_MAIN_MENU_MAP:
	{
		char	buf[12];
		
		OEM_SetBROWSER_ADSAccount();	//Add By zzg 2011_07_08
		
    	MSG_FATAL("AEECLSID_BRW_APP...........START",0,0,0);
    	start_info.appid_fx = APP_ORIGINATOR_BROWSER;
        start_info.subtype_keycode = APP_BROWSER_START_URL;
        start_info.par_keychar = 0;

		start_info.fInfo.url_info.title = "map";
		start_info.fInfo.url_info.url = "http://wap.movidamovil.com/venezuela/?debug=true";		
		
        //start_info.fInfo.url_info.title = "163";
        //start_info.fInfo.url_info.url = "http://wap.163.com";

		
        SPRINTF(buf, "%p", &start_info);
		Result = ISHELL_StartAppletArgs(pMe->m_pShell,AEECLSID_NF3,buf);
        //Result = ISHELL_StartApplet(pMe->m_pShell, AEECLSID_BRW_APP);
        MSG_FATAL("AEECLSID_BRW_APP...........Result=%d",Result,0,0);		
		
		
		break;
	}
	case IDS_MAIN_MENU_NEO_APP:
	{
        Result = ISHELL_StartApplet(pMe->m_pShell, AEECLSID_APPMANAGER);
		break;
	}
	case IDS_MAIN_MENU_NEO_NAV:
	{
		char	buf[12];
		
		OEM_SetBROWSER_ADSAccount();	//Add By zzg 2011_07_08
		
    	MSG_FATAL("AEECLSID_BRW_APP...........START",0,0,0);
    	start_info.appid_fx = APP_ORIGINATOR_BROWSER;
        start_info.subtype_keycode = APP_BROWSER_START_MAINMENU;
        start_info.par_keychar = 0;
        //start_info.fInfo.url_info.title = "163";
        //start_info.fInfo.url_info.url = "http://www.163.com";
        SPRINTF(buf, "%p", &start_info);
		Result = ISHELL_StartAppletArgs(pMe->m_pShell,AEECLSID_NF3,buf);
        //Result = ISHELL_StartApplet(pMe->m_pShell, AEECLSID_BRW_APP);
        MSG_FATAL("AEECLSID_BRW_APP...........Result=%d",Result,0,0);		
		
		break;
	}
#endif

    case IDS_MAIN_MENU_FILEMGR:
    case IDS_MAIN_MENU_GALLERY:
	case IDS_MAIN_MENU_MEDIAGALLERY_C337:	
    case IDS_MAIN_MENU_MEDIAGALLERY:
		#ifdef FEATURE_VERSION_W021_C11
		Result = ISHELL_StartApplet(pMe->m_pShell, AEECLSID_APPLICATION);
		#else
        Result = ISHELL_StartApplet(pMe->m_pShell, AEECLSID_MEDIAGALLERY);
		#endif
        break;
        
    case IDS_MAIN_MENU_CONTACT:
	case IDS_PHONEBOOK:
    case IDS_MAIN_MENU_CONTACTS:
        {
            IContactApp *ca = NULL;
            Result = ISHELL_CreateInstance(pMe->m_pShell,AEECLSID_APP_CONTACT, (void**)&ca);
            if(SUCCESS == Result)
            {
                ICONTAPP_MainMenu(ca);
                IContactApp_Release(ca);
            }
        }
        break;
    case IDS_MAIN_MENU_INTEXZONE: 
#ifdef FEATURE_VERSION_C316
		Result = ISHELL_StartApplet(pMe->m_pShell, AEECLSID_INDEX_ZONE);
		break;
#endif
    case IDS_MAIN_MENU_SMARTFRENACCESS:
    case IDS_MAIN_MENU_UTK:
        Result = ISHELL_StartApplet(pMe->m_pShell, AEECLSID_APP_UTK);
        break;
        
    case IDS_MAIN_MENU_TOOLS:
	case IDS_MAIN_MENU_ORGANIZER:	
    case IDS_MAIN_MENU_APPLICATION:
        Result = ISHELL_StartApplet(pMe->m_pShell, AEECLSID_APPLICATION);
        break;
		
    case IDS_MAIN_MENU_CALL_LOGS:
	case IDS_MAIN_MENU_RECENTCALLS_C337:	
    case IDS_MAIN_MENU_RECENTCALLS:
        Result = ISHELL_StartApplet(pMe->m_pShell, AEECLSID_APP_RECENTCALL);
        break;
        
    case IDS_MAIN_MENU_SMS:
    case IDS_MAIN_MENU_MESSAGES:
        Result = ISHELL_StartApplet(pMe->m_pShell, AEECLSID_WMSAPP);
        break;
        
    case IDS_MAIN_MENU_MULTIMEDIA:
        Result = ISHELL_StartApplet(pMe->m_pShell, AEECLSID_MULTIMEDIA_LIST);
        break;
        
    case IDS_MAIN_MENU_STATIC_APPLICATION:
#if defined(FEATURE_APP_MANAGER)
#ifdef FEATURE_OEMOMH
        if(ISHELL_SendEvent(pMe->m_pShell,AEECLSID_DIALER,EVT_OMH_PROMPT,0,0))
        {
            break;
        }
#endif

        Result = ISHELL_StartApplet(pMe->m_pShell, AEECLSID_APPMANAGER);
#else
        Result = ISHELL_StartApplet(pMe->m_pShell, AEECLSID_STATIC_APP);
#endif
    break;
        
    case IDS_MAIN_MENU_CALENDAR:
    case IDS_MAIN_MENU_SCHEDULER:
        Result = ISHELL_StartApplet(pMe->m_pShell, AEECLSID_SCHEDULEAPP);
        break;
        
    case IDS_MAIN_MENU_SETTINGS:
        Result = ISHELL_StartApplet(pMe->m_pShell, AEECLSID_APP_SETTINGMENU);
        break;

	//Add By zzg 2012_11_08 for C337 
	case IDS_MAIN_MENU_SERVICES:
	    #if defined (FEATURE_VERSION_C337) || defined(FEATURE_VERSION_IC241A_MMX)
		 // liyz add for test @131206
       {
        
		AECHAR WBrowserTitle[40] = {0};
		(void)ISHELL_LoadResString(pMe->m_pShell,
									MAINMENU_RES_FILE_LANG, 							   
									IDS_MAIN_MENU_SERVICES,
									WBrowserTitle,
									sizeof(WBrowserTitle));
		if(pMe->m_pIAnn != NULL)
		{
		
		    DBGPRINTF("IDS_MAIN_MENU_MSTORE IANNUNCIATOR_Redraw");
			IANNUNCIATOR_SetFieldTextEx(pMe->m_pIAnn,WBrowserTitle,FALSE);
			IANNUNCIATOR_Redraw(pMe->m_pIAnn);
		}
        #ifdef FEATURE_VERSION_IN50A
        Result = SetBrowserArr_Main(pMe->m_pShell,(char*)"http://www.google.com.hk/"); //ISHELL_StartAppletArgs(pMe->m_pShell, AEECLSID_UCWEB, (char*)"call_ucweb:setmainpageurl:http://mimicromax.com");
        #else
        Result = SetBrowserArr_Main(pMe->m_pShell,(char*)"http://mimicromax.com"); //ISHELL_StartAppletArgs(pMe->m_pShell, AEECLSID_UCWEB, (char*)"call_ucweb:setmainpageurl:http://mimicromax.com");
        #endif
       }
		#else
        Result = SetBrowserArr_Main(pMe->m_pShell,NULL);//ISHELL_StartApplet(pMe->m_pShell, AEECLSID_UCWEB);
        #endif
		break;
	case IDS_MAIN_MENU_MSTORE:
#if defined (FEATURE_VERSION_C337) || defined(FEATURE_VERSION_IC241A_MMX)
#if defined (FEATURE_VERSION_IN50A)
        Result = SetBrowserArr_Main(pMe->m_pShell,(char*)"http://www.google.com.hk/"); 
#else
        Result = SetBrowserArr_Main(pMe->m_pShell,(char*)"http://mimicromax.com"); //ISHELL_StartAppletArgs(pMe->m_pShell, AEECLSID_UCWEB, (char*)"call_ucweb:setmainpageurl:http://mimicromax.com");
#endif        
#else
        Result = SetBrowserArr_Main(pMe->m_pShell,NULL);//ISHELL_StartAppletArgs(pMe->m_pShell, AEECLSID_UCWEB, (char*)"call_ucweb:setmainpageurl:http://mimicromax.com");
#endif
		break;
	case IDS_MAIN_MENU_MZONE:
		Result = ISHELL_StartApplet(pMe->m_pShell, AEECLSID_MiZone);
		break;
	//Add End
	
	case IDS_MAIN_MENU_USER_PROFILE:	
    case IDS_MAIN_MENU_USERPROFILE:
        Result = ISHELL_StartApplet(pMe->m_pShell, AEECLSID_APP_SOUNDMENU);
        break;
        
    case IDS_MAIN_MENU_GAMES:
        Result = ISHELL_StartApplet(pMe->m_pShell, AEECLSID_GAME);
        break;
		
	#ifdef FEATURE_VERSION_C316
    case IDS_MAIN_MENU_SERVICES_C316:  
		Result = ISHELL_StartApplet(pMe->m_pShell, AEECLSID_SERVICE);
        break;
	#endif	
	
    case IDS_MAIN_WAPBROWSER:
		#ifdef FEATURE_VERSION_K202_LM129C
		IANNUNCIATOR_SetHasTitleText(pMe->m_pIAnn,FALSE); //xxzhen
		IANNUNCIATOR_Redraw(pMe->m_pIAnn);
		#endif
        #if defined (FEATURE_VERSION_C337) || defined(FEATURE_VERSION_IC241A_MMX)
        #if defined (FEATURE_VERSION_IN50A)
        Result = SetBrowserArr_Main(pMe->m_pShell,(char*)"http://www.google.com.hk/"); 
        #else
        Result = SetBrowserArr_Main(pMe->m_pShell,(char*)"http://mimicromax.com"); //ISHELL_StartAppletArgs(pMe->m_pShell, AEECLSID_UCWEB, (char*)"call_ucweb:setmainpageurl:http://mimicromax.com");
        #endif        
        #else
        Result = SetBrowserArr_Main(pMe->m_pShell,(char*)""); //ISHELL_StartApplet(pMe->m_pShell, AEECLSID_UCWEB);
        #endif
        break;
    
    case IDS_MAIN_MENU_CALCULATOR: 
		 Result = ISHELL_StartApplet(pMe->m_pShell, AEECLSID_CALCAPP);
        break;
        
    case IDS_MAIN_MENU_FMRADIO:
        Result = ISHELL_StartApplet(pMe->m_pShell, AEECLSID_APP_FMRADIO);
        break;
        
    case IDS_MAIN_MENU_MUSICPLAYER:
        Result = ISHELL_StartApplet(pMe->m_pShell, AEECLSID_APP_MUSICPLAYER);
        break;
        
    case IDS_MAIN_MENU_SMARTWORLD:
    case IDS_MAIN_MENU_BLIVE:
#ifdef FEATURE_SMARTFREN_STATIC_BREW_APP
		Result = ISHELL_StartApplet(pMe->m_pShell, AEECLSID_STATIC_APP);
#else
#ifdef FEATURE_OEMOMH
        if(ISHELL_SendEvent(pMe->m_pShell,AEECLSID_DIALER,EVT_OMH_PROMPT,0,0))
        {
            break;
        }
#endif
        Result = ISHELL_StartApplet(pMe->m_pShell, AEECLSID_APPMANAGER);
#endif  
        break;
    
    case IDS_MAIN_MENU_FRENDUO:
#if defined(FEATURE_VERSION_M8) || defined(FEATURE_VERSION_M8P) || defined (FEATURE_VERSION_M8021)
		Result = ISHELL_StartApplet(pMe->m_pShell, AEECLSID_FRENDUO);
#endif
        break;
    
    case IDS_MAIN_MENU_TIMER:
        Result = ISHELL_StartApplet(pMe->m_pShell, AEECLSID_APPTIMER);
        break;
        
    case IDS_MAIN_MENU_CAMERA:
	{		
		Result = ISHELL_StartApplet(pMe->m_pShell, AEECLSID_APP_CAMERA);
		break;
	}  

#if defined(FEATURE_VERSION_IN50A)
    case IDS_MAIN_MENU_CAMERA_LIST:
    {		
        Result = ISHELL_StartApplet(pMe->m_pShell, AEECLSID_CAMERA_LIST);
		break;
	}  
#endif    
    
    default:
        break;

    }
    return Result;
}

extern char charsvc_p_name[UIM_CDMA_HOME_SERVICE_SIZE+1];
int SetBrowserArr_Main(IShell *pShell ,char *purl)
{
	int Result = EUNSUPPORTED;
	char urlCan[1024] = {0};

    OEM_SetUCBROWSER_ADSAccount();
    
	DBGPRINTF("svc_p_name %s %d",charsvc_p_name,charsvc_p_name[0],0);
	
	if(purl && STRLEN(purl)>1)
	{
        SPRINTF(urlCan,"call_ucweb:setexternurl:%s\2\3",purl);
		STRCAT(urlCan,"useragent:BREW-Applet/0x20068888 (BREW/3.1.5.20; DeviceId: 8976509865757e; Lang: hi; Profile/MIDP-2.0_Configuration/CLDC-1.1) ucweb-squid\2\3");
	}
	else
	{
		STRCPY(urlCan,"useragent:BREW-Applet/0x20068888 (BREW/3.1.5.20; DeviceId: 8976509865757e; Lang: hi; Profile/MIDP-2.0_Configuration/CLDC-1.1) ucweb-squid\2\3");
	}
    
	if(STRISTR (charsvc_p_name,"mts"))
	{
		MSG_FATAL("mst................",0,0,0);
        STRCAT(urlCan,"access_point:proxy_is:10.50.5.140:8080");
	}
	else if(STRISTR (charsvc_p_name,"tata"))
	{
        MSG_FATAL("tata................",0,0,0);
        STRCAT(urlCan,"access_point:proxy_is:172.23.252.15:9401");
	}
	else if(STRISTR (charsvc_p_name,"reliance"))
	{
        MSG_FATAL("reliance................",0,0,0);
        STRCAT(urlCan,"access_point:proxy_is:http://wapgw.ricinfo.com:8080");
	}
	else if(STRISTR (charsvc_p_name,"vmi"))
	{
        MSG_FATAL("vmi................",0,0,0);
        STRCAT(urlCan,"access_point:proxy_is:172.23.142.15:9401");
	}
	
	DBGPRINTF("urlCan==%s", urlCan);
    if(urlCan[0])
    {
	    Result = ISHELL_StartAppletArgs(pShell, AEECLSID_UCWEB, (char*)urlCan);
    }
    else
    {
        Result = ISHELL_StartApplet(pShell, AEECLSID_UCWEB);
    }

	return Result;	
}
#endif
#endif

