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
static StartInfo start_info;		
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
	#ifdef FEATURE_VERSION_C337
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
				pMe->m_pImageBgblack  = ISHELL_LoadResImage(pMe->m_pShell, AEE_APPSCOMMONRES_IMAGESFILE, IDB_BACKGROUND);
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
	#ifdef FEATURE_VERSION_C337
	case IDS_PHONEBOOK:
	#endif
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
#if defined (FEATURE_VERSION_H19C) || defined (FEATURE_VERSION_C01) || defined (FEATURE_VERSION_SKY)  
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
    ICON10_ANI,
    ICON11_ANI,
    ICON12_ANI,
#elif defined (FEATURE_DISP_176X220)
#ifndef FEATURE_VERSION_VG68
    ICON10_ANI,
    ICON11_ANI,
    ICON12_ANI,
#endif
#elif defined (FEATURE_DISP_240X320)
    ICON10_ANI,
    ICON11_ANI,
    ICON12_ANI,    
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
    ICON10_ANI_1,
    ICON11_ANI_1,
    ICON12_ANI_1,  
#endif    
#elif defined (FEATURE_DISP_176X220)
#ifndef FEATURE_VERSION_VG68
#ifndef FEATURE_VERSION_SKY
    ICON10_ANI_1,
    ICON11_ANI_1,
    ICON12_ANI_1,
#endif    
#endif
#elif defined (FEATURE_DISP_240X320)
    ICON10_ANI_1,
    ICON11_ANI_1,
    ICON12_ANI_1,     
#elif defined (FEATURE_DISP_320X240)
    ICON10_ANI_1,
    ICON11_ANI_1,
    ICON12_ANI_1,      
#endif
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
        
        for(i=0;i<MAX_MATRIX_ITEMS;i++)
        {
            if(pMe->m_pImageIcon[i] != NULL)
            {
                (void)IIMAGE_Release(pMe->m_pImageIcon[i]);
                pMe->m_pImageIcon[i] = NULL;
            }
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
#if defined (FEATURE_VERSION_C01)|| defined(FEATURE_VERSION_VG68)|| defined (FEATURE_VERSION_SKY)
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
#if defined (FEATURE_VERSION_H19C) || defined (FEATURE_VERSION_C01)|| defined (FEATURE_VERSION_SKY)
                    pMe->m_pImageBg = ISHELL_LoadImage(pMe->m_pShell,ICON_ANI[0]);
#elif defined(FEATURE_VERSION_W317A)
                    pMe->m_pImageBg = ISHELL_LoadResImage(pMe->m_pShell, AEE_APPSCOMMONRES_IMAGESFILE, IDB_MAINMENU_BG);
#else
                    pMe->m_pImageBg = ISHELL_LoadResImage(pMe->m_pShell, AEE_APPSCOMMONRES_IMAGESFILE, IDB_BACKGROUND);//modi by yangdecai
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
            DrawMatrix(pMe);
            // 绘制聚焦过程动画
            MoveCursorTo(pMe, pMe->m_nRow, pMe->m_nColumn);
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

                for(i=0;i<MAX_MATRIX_ITEMS;i++)
                {
                    if(pMe->m_pImageIcon[i] != NULL)
                    {
                        (void)IIMAGE_Release(pMe->m_pImageIcon[i]);
                        pMe->m_pImageIcon[i] = NULL;
                    }
                } 
                
                if(pMe->m_pAnimate != NULL)
                {
                    (void)IIMAGE_Release(pMe->m_pAnimate);
                    pMe->m_pAnimate = NULL;
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
    for( i = 0; i < MAX_MATRIX_ITEMS; i ++)
    {
        pMe->m_Icondefault_Pt[i].x = iconSpaceHorizontal +
            ( imageInfoIcon.cx + iconSpaceHorizontal) * ( i % MAX_MATRIX_COLS);
        
        pMe->m_Icondefault_Pt[i].y = TITLEBAR_HEIGHT + iconSpaceVertical +
            ( imageInfoIcon.cy + iconSpaceVertical) * ( i / MAX_MATRIX_COLS);

        //计算焦点图片的坐标
        pMe->m_IconFocus_Pt[i].x = pMe->m_Icondefault_Pt[i].x - (ICON_ANIMATED_WIDTH - imageInfoIcon.cx)/2;

        pMe->m_IconFocus_Pt[i].y = pMe->m_Icondefault_Pt[i].y - (ICON_ANIMATED_HEIGHT- imageInfoIcon.cy)/2;
        //end added
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
        Appscommon_ResetBackground(pMe->m_pDisplay, 
                                                    pMe->m_pImageBg, 
                                                    APPSCOMMON_BG_COLOR, 
                                                    pRect, 
                                                    0, 
                                                    0);
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
    
    if (NULL == pMe)
    {
        return;
    }
    //draw bg image
    MainMenu_DrawBackGround(pMe, &pMe->m_rc);
#ifndef FEATURE_VERSION_H19C  
#ifndef FEATURE_VERSION_C01 
#ifndef FEATURE_VERSION_SKY
    //Draw icon
    for (i = 0; i < MAX_MATRIX_ITEMS; i ++)
    {
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
    }  
#endif    
#endif
#endif
    BarParam.eBBarType = BTBAR_SELECT_BACK;
    DrawBottomBar(pMe->m_pDisplay, &BarParam);//wlh 20090412 add
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
    
 #if defined (FEATURE_VERSION_H19C) || defined (FEATURE_VERSION_C01) || defined(FEATURE_VERSION_SKY)
    if(pMe->m_pAnimate != NULL)
    {
        IIMAGE_Stop(pMe->m_pAnimate);
        IIMAGE_Release(pMe->m_pAnimate);
        pMe->m_pAnimate = NULL;
    }
#endif    
    if(pMe->m_pAnimate == NULL)
    {
        pMe->m_pAnimate = ISHELL_LoadImage(pMe->m_pShell, ICON_ANI_1[theFocus]);
    }

	if( pMe->m_pAnimate != NULL)
    {
#if defined (FEATURE_VERSION_H19C) || defined (FEATURE_VERSION_C01)
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
    
    // 绘制聚焦后矩阵初始界面
    SETAEERECT(&rect, pMe->m_IconFocus_Pt[theFocus].x, 
                      pMe->m_IconFocus_Pt[theFocus].y, 
                      ICON_ANIMATED_WIDTH, 
                      ICON_ANIMATED_HEIGHT);
    
    MainMenu_DrawBackGround(pMe, &rect);
    
    if (pMe->m_pImageIcon[theFocus])
    {
        IIMAGE_Draw(pMe->m_pImageIcon[theFocus],
                    pMe->m_Icondefault_Pt[theFocus].x, 
                    pMe->m_Icondefault_Pt[theFocus].y);
    }
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
    
#ifdef FEATURE_VERSION_H19C  
    if(pMe->m_pIAnn != NULL)
    {
        IANNUNCIATOR_SetHasTitleText(pMe->m_pIAnn, TRUE);
    }
#endif
    switch(i)
	{
#ifdef FEATURE_VERSION_W208S
	case IDS_MAIN_MENU_CALL_LOGS:
	{
		Result = ISHELL_StartApplet(pMe->m_pShell, AEECLSID_APP_RECENTCALL);
		break;
	}
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
	
#ifdef FEATURE_BREW_CAMERA	
	case IDS_MAIN_MENU_CAMERA:
	{		
		Result = ISHELL_StartApplet(pMe->m_pShell, AEECLSID_APP_CAMERA);
		break;
	}
#endif
	
	case IDS_MAIN_MENU_FILEMGR:
	{
		Result = ISHELL_StartApplet(pMe->m_pShell, AEECLSID_MEDIAGALLERY);
		break;
	}
#endif
    case IDS_MAIN_MENU_GALLERY:
	case IDS_MAIN_MENU_MEDIAGALLERY_C337:	
    case IDS_MAIN_MENU_MEDIAGALLERY:
        Result = ISHELL_StartApplet(pMe->m_pShell, AEECLSID_MEDIAGALLERY);
        break;
        
    case IDS_MAIN_MENU_CONTACT:
	#ifdef FEATURE_VERSION_C337
	case IDS_PHONEBOOK:
	#endif
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
	case IDS_MAIN_MENU_ORGANIZER:	
    case IDS_MAIN_MENU_APPLICATION:
        Result = ISHELL_StartApplet(pMe->m_pShell, AEECLSID_APPLICATION);
        break;

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
		Result = ISHELL_StartApplet(pMe->m_pShell, AEECLSID_UCWEB);
		break;
	case IDS_MAIN_MENU_MSTORE:
        Result = ISHELL_StartAppletArgs(pMe->m_pShell, AEECLSID_UCWEB, (char*)"call_ucweb:setmainpageurl:http://mimicromax.com");
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
        
    case IDS_MAIN_WAPBROWSER:
#if(defined(FEATURE_OEMOMH) 
        extern void OEM_SetBROWSER_ADSAccount(void);
        OEM_SetBROWSER_ADSAccount();
#endif		
        #ifdef FEATURE_VERSION_C337
        Result = ISHELL_StartAppletArgs(pMe->m_pShell, AEECLSID_UCWEB, (char*)"call_ucweb:setmainpageurl:http://mimicromax.com");
        #else
        Result = ISHELL_StartApplet(pMe->m_pShell, AEECLSID_UCWEB);
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
#endif
