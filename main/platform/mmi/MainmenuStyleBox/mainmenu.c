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

#include "ContApp.h"

#include "Appscommon.h"
#include "appscommon.brh"
#if defined(FEATURE_DISP_160X128)
#include "Appscommon_160x128.brh"
#elif defined(FEATURE_DISP_128X128)
#include "Appscommon_color.brh"
#else
#include "Appscommon_color.brh"
#endif
#include "err.h"
#if !defined( AEE_SIMULATOR)
#include "AEERUIM.h" 
#include "OEMCFGI.h"
#endif //#if !defined( AEE_SIMULATOR)

#define PARAM_NOT_REF(x)
#ifdef FEATURE_LCD_TOUCH_ENABLE//wlh add for LCD touch
#define MAINMENU_SELECT 10000
#define MAINMENU_PT_IN_RECT(a,b,rct)      (boolean)( ((a) >= (rct).x && (a) <= ((rct).x + (rct).dx)) && ((b) >= (rct).y && (b) <= ((rct).y + (rct).dy)) )
#endif//FEATURE_LCD_TOUCH_ENABLE
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

static int     MainMenu_MainMenuService( IMainMenu *pi, MainMenuServiceType eStype);

static MainMenu gMainMenu={0};

static const VTBL( IMainMenu) gMainMenuMethods =
{
    MainMenu_AddRef,
    MainMenu_Release,
    MainMenu_HandleEvent,
    MainMenu_MainMenuService
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

static void calculateScreenParameters(MainMenu *pMe);

static void MainMenu_DrawBackGround(MainMenu *pMe, AEERect *pRect);

// 初始整个背景及全部初始图标
static void DrawMatrix(MainMenu *pMe);

#ifdef FEATURE_ICON_MOVE_ANIMATION
static void DrawFocusMoveAnimation(MainMenu *pMe);
#endif

#ifdef FEATURE_FOCUS_ANIMATION
static void DrawFocusIconAnimation(MainMenu *pMe);
#else
static void DrawFocusIcon(MainMenu *pMe);
#endif

static void MoveCursorTo(MainMenu *pMe, int row, int column);

static void setCursor(MainMenu *pMe, int row, int column);

static boolean StartApplet(MainMenu *pMe, int i);

boolean MainMenu_RouteDialogEvt(MainMenu *pMe,
    AEEEvent    eCode,
    uint16  wParam,
    uint32 dwParam
);

NextFSMAction MainMenu_ProcessState(MainMenu *pMe);

void MainMenu_ShowDialog(MainMenu  *pMe,  uint16 dlgResId);

// MAINST_MAIN 状态处理函数
static NextFSMAction MAINST_MAIN_Handler(MainMenu *pMe);
//MAINST_EXIT  状态处理函数
static NextFSMAction MAINST_EXIT_Handler(MainMenu *pMe);

//MAINST_MSGBOX 状态处理函数
#ifdef FEATRUE_SUPPORT_G_SENSOR
static NextFSMAction MAINST_MSGBOX_Handler(MainMenu *pMe);
#endif

static boolean  MainMenu_IconMenuHandler(MainMenu *pMe, AEEEvent eCode, uint16 wParam, uint32 dwParam);

#ifdef FEATRUE_SUPPORT_G_SENSOR
static boolean  MainMenu_MsgBoxHandler(MainMenu *pMe, AEEEvent eCode, uint16 wParam, uint32 dwParam);
static void Mainmenu_DialogTimeout(void *pme);

boolean MainMenu_Set_Shake_Disable(MainMenu *pMe);
boolean MainMenu_Get_Shake_OnOrOff(MainMenu *pMe);
void MainMenu_CloseSportBgRun(MainMenu *pMe);


#endif

void MainMenu_DraImage(MainMenu *pMe,int16 index,int16 imgid);


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
    #if defined (FEATURE_DISP_160X128)
    ICON10_ANI,
    ICON11_ANI,
    ICON12_ANI,
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
        ERR("hELLO......",0,0,0);
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
            ERR("SUCESSS...............FAUKE",0,0,0);
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
	int i;
	boolean iamgeflag = FALSE;
    if (NULL == pMe)
    {
        return EFAILED;
    }
    pMe->m_nRow        = 0;
    pMe->m_nColumn     = 2;
#ifdef FEATURE_ICON_MOVE_ANIMATION
    pMe->m_nPrevRow   = 1;
    pMe->m_nPrevColumn = 1;
    pMe->m_bMoveing = FALSE;
    pMe->m_pDevImage = NULL; 
#endif
    pMe->m_MainSel  = 0;
    pMe->m_MenuSel  = 0;
#ifdef FEATURE_FOCUS_ANIMATION
    pMe->m_nIconAniFrameIdx      = 0;
#endif
    pMe->m_bNormalStart = TRUE;


    // 接口创建及相关初始化
    if (ISHELL_CreateInstance(pMe->m_pShell, AEECLSID_CONFIG,
            (void **)&pMe->m_pConfig) != SUCCESS)
    {
        return EFAILED;
    }
    
    if (ISHELL_CreateInstance(pMe->m_pShell, AEECLSID_DISPLAY, 
            (void **) &pMe->m_pDisplay) != SUCCESS)
    {
        return EFAILED;
    }
//wlh 20090409 add start
	pMe->m_pImageTurn[0] = ISHELL_LoadImage(pMe->m_pShell,ICON1_ANI_1);
	pMe->m_pImageTurn[1] = ISHELL_LoadImage(pMe->m_pShell,ICON2_ANI_1);
	pMe->m_pImageTurn[2] = ISHELL_LoadImage(pMe->m_pShell,ICON3_ANI_1);
	pMe->m_pImageTurn[3] = ISHELL_LoadImage(pMe->m_pShell,ICON4_ANI_1);
	pMe->m_pImageTurn[4] = ISHELL_LoadImage(pMe->m_pShell,ICON5_ANI_1);
	pMe->m_pImageTurn[5] = ISHELL_LoadImage(pMe->m_pShell,ICON6_ANI_1);
	pMe->m_pImageTurn[6] = ISHELL_LoadImage(pMe->m_pShell,ICON7_ANI_1);
	pMe->m_pImageTurn[7] = ISHELL_LoadImage(pMe->m_pShell,ICON8_ANI_1);
	pMe->m_pImageTurn[8] = ISHELL_LoadImage(pMe->m_pShell,ICON9_ANI_1);
    #if defined (FEATURE_DISP_160X128)
    pMe->m_pImageTurn[9] = ISHELL_LoadImage(pMe->m_pShell,ICON10_ANI_1);
	pMe->m_pImageTurn[10] = ISHELL_LoadImage(pMe->m_pShell,ICON11_ANI_1);
	pMe->m_pImageTurn[11] = ISHELL_LoadImage(pMe->m_pShell,ICON12_ANI_1);
    #endif
    pMe->m_pImageFocus   = ISHELL_LoadImage(pMe->m_pShell,ICON_ANI_FOCUSDEFAULT);
    if(pMe->m_pImageFocus == NULL)
    {
        return EFAILED;
    }
    
	for (i = 0; i < MAX_TURN_NUM; i ++)
	{
		if(pMe->m_pImageTurn[i] == NULL)
        {
            ERR(":::::::::::::::%d:::::::::::::",i,0,0);
			iamgeflag = TRUE;
			break;
		}
	}
	if(iamgeflag)
	{
		for (i = 0; i < MAX_TURN_NUM; i ++)
		{
			if(pMe->m_pImageTurn[i] != NULL)
			{
				IImage_Release(pMe->m_pImageTurn[i]);
				pMe->m_pImageTurn[i] = NULL;
			}
		}
		return EFAILED;
	}
//wlh 20090409 add end
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

#ifdef FEATURE_ICON_MOVE_ANIMATION
        if(pMe->m_pDevImage != NULL)
        {
            (void)IBitmap_Release(pMe->m_pDevImage);
            pMe->m_pDevImage = NULL;
        }
#endif

		for (i = 0; i < MAX_TURN_NUM; i ++)
		{
			if(pMe->m_pImageTurn[i] != NULL)
        	{
				IImage_Release(pMe->m_pImageTurn[i]);
				pMe->m_pImageTurn[i] = NULL;
			}
		}
        if(pMe->m_pImageFocus != NULL)
        {
            IImage_Release(pMe->m_pImageFocus);
            pMe->m_pImageFocus = NULL;
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
            pMe->m_bNormalStart = TRUE;
            MainMenu_RunFSM(pMe);
            return TRUE;

        case EVT_APP_STOP:
            {
                int theFocus = 4;
                setCursor( pMe, theFocus / 3, theFocus % 3);
                pMe->m_MainSel  = 0;
                pMe->m_MenuSel  = 0;                  
                (void)ISHELL_CancelTimer( pMe->m_pShell, NULL, pMe);
                pMe->m_eAppStatus = MAINMENU_STOP;
                
                return TRUE;
            }

        case EVT_APP_SUSPEND:
            (void)ISHELL_CancelTimer(pMe->m_pShell, NULL, pMe);
            pMe->m_eAppStatus = MAINMENU_SUSPEND;
            
            return TRUE;
        

        case EVT_APP_RESUME:
            {
                AEEAppStart* as = ( AEEAppStart*)dwParam;
                pMe->m_rc    = as->rc;
                pMe->m_eAppStatus = MAINMENU_RUNNING;

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

        case EVT_KEY_PRESS:
        case EVT_KEY_RELEASE:
        case EVT_COMMAND:
            return MainMenu_RouteDialogEvt(pMe,eCode,wParam,dwParam); 
			
#ifdef FEATURE_LCD_TOUCH_ENABLE//wlh for LCD TOUCH
		case EVT_PEN_UP:
		//case EVT_PEN_DOWN: 
		{
			return MainMenu_RouteDialogEvt(pMe,eCode,wParam,dwParam);
		}
        case EVT_USER:  
			if(eCode == EVT_USER)
			{
				if(dwParam != MAINMENU_SELECT)
				{
					if((wParam == AVK_SELECT) || (wParam == AVK_INFO))
					{
						eCode = EVT_COMMAND;
						return MainMenu_RouteDialogEvt(pMe,eCode,dwParam,0);//注意这里从aeemenu传回了当前的menu id dwParam 转为 EVT_COMMAND 事件时要当wParam 用
					}
				}
				else
				{	
					if((wParam == AVK_SELECT) || (wParam == AVK_INFO))
					{
						eCode = EVT_KEY;
						return MainMenu_RouteDialogEvt(pMe,eCode,wParam,dwParam);
					}
				}
				if(wParam == AVK_CLR)
				{
					eCode = EVT_KEY;
					return MainMenu_RouteDialogEvt(pMe,eCode,wParam,dwParam);
				}
				
			}
#endif//FEATURE_LCD_TOUCH_ENABLE

        default:
            return MainMenu_RouteDialogEvt(pMe,eCode,wParam,dwParam);
    }
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
            
        #ifdef FEATRUE_SUPPORT_G_SENSOR 
        case MAINST_MSGBOX:
            return MAINST_MSGBOX_Handler(pMe);
        #endif
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

        case DLGRET_GAME:
            MOVE_TO_STATE(MAINST_GAME)
            return NFSMACTION_CONTINUE;

        case DLGRET_MEDIA:
            MOVE_TO_STATE(MAINST_PLAYER)
            return NFSMACTION_CONTINUE;

        case DLGRET_DATA:
            MOVE_TO_STATE(MAINST_DATA)
            return NFSMACTION_CONTINUE;
            
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
    MAINST_MSGBOX_Handler

说明:
    MAINST_MSGBOX 状态处理函数。

参数:
    pMe [in]: 指向MAINMENU Applet对象结构的指针。该结构包含小程序的特定信息。

返回值:
    NFSMACTION_CONTINUE: 指示不停状态机。
    NFSMACTION_WAIT: 指示停止状态机。

备注:

==============================================================================*/

#ifdef FEATRUE_SUPPORT_G_SENSOR
static NextFSMAction MAINST_MSGBOX_Handler(MainMenu *pMe)
{
    if (NULL == pMe)
    {
        return NFSMACTION_WAIT;
    }
    switch (pMe->m_eDlgReturn)
    {
        // 进入主界面
        case DLGRET_CREATE:
            MainMenu_ShowDialog(pMe,IDD_MSGBOX_MENU);
            return NFSMACTION_WAIT;

        case DLGRET_GAME:
            MOVE_TO_STATE(MAINST_GAME)
            return NFSMACTION_CONTINUE;

        case DLGRET_CANCELED:
            MOVE_TO_STATE(MAINST_MAIN)
            return NFSMACTION_CONTINUE;

        default:
            MOVE_TO_STATE(MAINST_EXIT)
            return NFSMACTION_CONTINUE;            

    }

}
#endif




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
        DBGPRINTF("Trying to create dialog %d without closing previous one",dlgResId,0,0);
        return;
    }
    
    if (NULL != pMe->m_pDisplay)
    {
        AEEDeviceInfo di={0,};
        
        if (dlgResId == IDD_MAIN_MENU)
        {
            (void)IDISPLAY_SetPrefs(pMe->m_pDisplay, "a:0", STRLEN("a:0"));
        }
        else
        {
            (void)IDISPLAY_SetPrefs(pMe->m_pDisplay, "a:1", STRLEN("a:1"));
        }
        
        ISHELL_GetDeviceInfo(pMe->m_pShell, &di);
        pMe->m_rc.dx = di.cxScreen;
        pMe->m_rc.dy = di.cyScreen;
        IDISPLAY_SetClipRect(pMe->m_pDisplay, &pMe->m_rc);
        calculateScreenParameters(pMe);
    }
    
    nRet = ISHELL_CreateDialog(pMe->m_pShell,MAINMENU_RES_FILE_LANG,dlgResId,NULL);
    if (nRet != SUCCESS)
    {
        DBGPRINTF("Failed to create the dialog %d in the MAINMENU applet.",dlgResId,0,0);
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

    #ifdef FEATRUE_SUPPORT_G_SENSOR
        case IDD_MSGBOX_MENU:
            return MainMenu_MsgBoxHandler(pMe, eCode, wParam, dwParam);
    #endif
        default:
            return FALSE;
    }
}

#ifdef FEATURE_LCD_TOUCH_ENABLE//WLH ADD FOR LCD TOUCH

#include "AEE.h"

/*=============================================================================
FUNCTION:  movepen_anddo

DESCRIPTION: 计算当前图片的坐标，并做触摸响应动作（第一次显示焦点，是焦点则进入）
=============================================================================*/
static movepen_anddo(MainMenu *pMe,uint32 dwParam)
{
	AEERect rc;
	uint16          iconSpaceHorizontal = 0;
    uint16          iconSpaceVertical   = 0;

	int16 wXPos = (int16)AEE_GET_X((const char *)dwParam);
	int16 wYPos = (int16)AEE_GET_Y((const char *)dwParam);

	SETAEERECT(&rc, pMe->m_IconFocus_Pt[0].x, pMe->m_IconFocus_Pt[0].y,pMe->m_IconFocus_Pt[2].x + ICON_ANIMATED_HEIGHT - pMe->m_IconFocus_Pt[0].x , pMe->m_IconFocus_Pt[10].y + ICON_ANIMATED_HEIGHT - pMe->m_IconFocus_Pt[0].y);
	if(!MAINMENU_PT_IN_RECT(wXPos,wYPos,rc))
	{

		AEEDeviceInfo devinfo;
		int nBarH ;
		AEERect rc;

		nBarH = GetBottomBarHeight(pMe->m_pDisplay);
        
		MEMSET(&devinfo, 0, sizeof(devinfo));
		ISHELL_GetDeviceInfo(pMe->m_pShell, &devinfo);
		SETAEERECT(&rc, 0, devinfo.cyScreen-nBarH, devinfo.cxScreen, nBarH);

		if(MAINMENU_PT_IN_RECT(wXPos,wYPos,rc))
		{
			if(wXPos >= rc.x && wXPos < rc.x + (rc.dx/3) )//左
			{
				boolean rt =  ISHELL_PostEvent(pMe->m_pShell,AEECLSID_MAIN_MENU,EVT_USER,AVK_SELECT,MAINMENU_SELECT);
				return rt;
			}
			else if(wXPos >= rc.x + (rc.dx/3)   && wXPos < rc.x + (rc.dx/3)*2 )//左
			{
				boolean rt = ISHELL_PostEvent(pMe->m_pShell,AEECLSID_MAIN_MENU,EVT_USER,AVK_INFO,MAINMENU_SELECT);
				return rt;
			}
			else if(wXPos >= rc.x + (rc.dx/3)*2 && wXPos < rc.x + (rc.dx/3)*3 )//左
			{						
				boolean rt = ISHELL_PostEvent(pMe->m_pShell,AEECLSID_MAIN_MENU,EVT_USER,AVK_CLR,0);
				return rt;
			}
		}
		return TRUE;
	}

	//计算焦点图片的坐标
	if((wXPos > pMe->m_IconFocus_Pt[0].x) && (wXPos < pMe->m_IconFocus_Pt[0].x + ICON_ANIMATED_WIDTH))
	{
		iconSpaceVertical = 0;
	}
	else if((wXPos > pMe->m_IconFocus_Pt[1].x) && (wXPos < pMe->m_IconFocus_Pt[1].x + ICON_ANIMATED_WIDTH))
	{
		iconSpaceVertical = 1;
	}
	else if((wXPos > pMe->m_IconFocus_Pt[2].x) && (wXPos < pMe->m_IconFocus_Pt[2].x + ICON_ANIMATED_WIDTH))
	{
		iconSpaceVertical = 2;
	}

	if((wYPos > pMe->m_IconFocus_Pt[0].y) && (wYPos < pMe->m_IconFocus_Pt[0].y + ICON_ANIMATED_HEIGHT))
	{
		iconSpaceHorizontal = 0;
	}
	else if((wYPos > pMe->m_IconFocus_Pt[4].y) && (wYPos < pMe->m_IconFocus_Pt[4].y + ICON_ANIMATED_HEIGHT))
	{
		iconSpaceHorizontal = 1;
	}
	else if((wYPos > pMe->m_IconFocus_Pt[7].y) && (wYPos < pMe->m_IconFocus_Pt[7].y + ICON_ANIMATED_HEIGHT))
	{
		iconSpaceHorizontal = 2;
	}
	else if((wYPos > pMe->m_IconFocus_Pt[10].y) && (wYPos < pMe->m_IconFocus_Pt[10].y + ICON_ANIMATED_HEIGHT))
	{
		iconSpaceHorizontal = 3;
	}
	
	if((pMe->m_nRow != iconSpaceHorizontal) || (pMe->m_nColumn != iconSpaceVertical))//当前不是焦点则只移动焦点
	{
		DrawMatrix(pMe);

		MoveCursorTo(pMe, iconSpaceHorizontal, iconSpaceVertical);

		return TRUE;
	}
	
	{		
		int i;
		ISHELL_CancelTimer (pMe->m_pShell, (PFNNOTIFY)DrawFocusIconAnimation, pMe);//这里是为了防止动画没有画完而取消画动画的定时器
		if (pMe->m_pAnimate != NULL)
        {
            IIMAGE_Release(pMe->m_pAnimate);
            pMe->m_pAnimate = NULL;
        }
		i = iconSpaceHorizontal * 3 + iconSpaceVertical;
		StartApplet(pMe, i);
	}

	return TRUE;
}
#endif//FEATURE_LCD_TOUCH_ENABLE

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
    
#ifdef FEATURE_LCD_TOUCH_ENABLE//WLH ADD FOR LCD TOUCH
		case EVT_PEN_UP:
			return movepen_anddo(pMe,dwParam);
#endif//FEATURE_LCD_TOUCH_ENABLE

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
                if(pMe->m_nRandomMenu != 0)
                {
                    pMe->m_pImageBg = ISHELL_LoadResImage(pMe->m_pShell, AEE_APPSCOMMONRES_IMAGESFILE, IDI_MENU_BACKGROUND_TRANS);
                }
                else
#endif
                {
                    pMe->m_pImageBg = ISHELL_LoadResImage(pMe->m_pShell, AEE_APPSCOMMONRES_IMAGESFILE, IDI_MENU_BACKGROUND);
                }
            }
            (void) ISHELL_PostEvent( pMe->m_pShell,
                                     AEECLSID_MAIN_MENU,
                                     EVT_USER_REDRAW,
                                     0,
                                     0);
            return TRUE;


        case EVT_USER_REDRAW:
            // 初始整个背景及全部初始图标
            ERR("EVT_USER_REDRAW:::::::::::1111111111",0,0,0);
            DrawMatrix(pMe);
#ifdef FEATURE_ICON_MOVE_ANIMATION
            {
                IBitmap *pTempBmp = NULL;

                IDISPLAY_GetDeviceBitmap(pMe->m_pDisplay, &pTempBmp);
                if(NULL != pTempBmp)
                {
                    if(NULL == pMe->m_pDevImage)
                    {
                        IBITMAP_CreateCompatibleBitmap(pTempBmp, &pMe->m_pDevImage, pMe->m_rc.dx, pMe->m_rc.dy);
                    }
                    IBITMAP_BltIn(pMe->m_pDevImage, 0, 0, pMe->m_rc.dx, pMe->m_rc.dy, pTempBmp, 0, 0, AEE_RO_COPY);
                    IBITMAP_Release(pTempBmp); //added by chengxiao 2009.04.20
                }
            }
#endif
            
            // 绘制聚焦过程动画
            MoveCursorTo(pMe, pMe->m_nRow, pMe->m_nColumn);
            return TRUE;
            
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
#ifdef FEATURE_ICON_MOVE_ANIMATION
                pMe->m_bMoveing = FALSE;
                if(pMe->m_pDevImage != NULL)
                {
                    (void)IBitmap_Release(pMe->m_pDevImage);
                    pMe->m_pDevImage = NULL;
                }
#endif
                ISHELL_CancelTimer(pMe->m_pShell, NULL, (void**)pMe);
            }
            return TRUE;


        case EVT_KEY:
            //ISHELL_CancelTimer(pMe->m_pShell, NULL, (void**)pMe);
            switch( wParam)
            {
                case AVK_CLR:
                    ERR("AVK_CLR:::::::::::::::::::!111111111111",0,0,0);
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
                        StartApplet(pMe, Focus);
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
                case AVK_STAR:
                    {
                        int Focus = (wParam - AVK_1);
                        StartApplet(pMe, Focus);
                    }
                    return TRUE;

                case AVK_0:
                    StartApplet(pMe, 10);
                    return TRUE;
                    
                case AVK_POUND:
                    StartApplet(pMe, 11);
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

/*=============================================================================
FUNCTION:  calculateScreenParameters

DESCRIPTION: 计算焦点图片以及默认图片的坐标

=============================================================================*/
static void calculateScreenParameters(MainMenu *pMe)
{
    AEEImageInfo    imageInfoIcon;
    uint16          iconSpaceHorizontal = 0;
    uint16          iconSpaceVertical   = 0;
    uint8           i                   = 0;

    //计算默认图片的坐标
    imageInfoIcon.cx = ICON_WIDTH;
    imageInfoIcon.cy = ICON_HEIGHT;

    //modified by chengxiao 2009.04.02
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
    //chengxiao modify end 2009.04.02
    DBGPRINTF("pMe->m_rc.dy:: %d,,,,iconSpaceHorizontal::::%d,,iconSpaceVertical:::::%d",pMe->m_rc.dy,iconSpaceHorizontal,iconSpaceVertical);
    for( i = 0; i < MAX_MATRIX_ITEMS; i ++)
    {
        pMe->m_Icondefault_Pt[i].x = iconSpaceHorizontal +
            ( imageInfoIcon.cx + iconSpaceHorizontal) * ( i % MAX_MATRIX_COLS);

        pMe->m_Icondefault_Pt[i].y = TITLEBAR_HEIGHT + iconSpaceVertical +
            ( imageInfoIcon.cy + iconSpaceVertical) * ( i / MAX_MATRIX_COLS);
        //added by chengxiao 2009.04.02
        //计算焦点图片的坐标
        pMe->m_IconFocus_Pt[i].x = pMe->m_Icondefault_Pt[i].x - (ICON_ANIMATED_WIDTH - imageInfoIcon.cx)/2;

        pMe->m_IconFocus_Pt[i].y = pMe->m_Icondefault_Pt[i].y - (ICON_ANIMATED_HEIGHT- imageInfoIcon.cy)/2;
        DBGPRINTF("pMe->m_IconFocus_Pt[%d].x:: %d",i,pMe->m_IconFocus_Pt[i].x);
        DBGPRINTF("pMe->m_IconFocus_Pt[%d].y:: %d",i,pMe->m_IconFocus_Pt[i].y);
        //end added
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
                                                    APPSCOMMON_MENUBG_XPOS, 
                                                    APPSCOMMON_MENUBG_YPOS);
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
FUNCTION:  calculateScreenParameters

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
    ERR("DrawMatrix:::::::::::::::::11111111",0,0,0);
    MainMenu_DrawBackGround(pMe, &pMe->m_rc); //modified by chengxiao 2009.04.10

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

    BarParam.eBBarType = BTBAR_SELECT_BACK;
    DrawBottomBar(pMe->m_pDisplay, &BarParam);//wlh 20090412 add
}

#ifdef FEATURE_ICON_MOVE_ANIMATION
static void DrawFocusMoveAnimation(MainMenu * pMe)
{
    static int nFrame = 0;
    int theFocus = pMe->m_nRow * MAX_MATRIX_COLS + pMe->m_nColumn,
        thePrevFocus = pMe->m_nPrevRow * MAX_MATRIX_COLS + pMe->m_nPrevColumn,
        xOldPos = pMe->m_IconFocus_Pt[thePrevFocus].x + (nFrame)*(pMe->m_IconFocus_Pt[theFocus].x - pMe->m_IconFocus_Pt[thePrevFocus].x)/ICON_ANIMATED_MOVE_FRAME, 
        yOldPos = pMe->m_IconFocus_Pt[thePrevFocus].y + (nFrame)*(pMe->m_IconFocus_Pt[theFocus].y - pMe->m_IconFocus_Pt[thePrevFocus].y)/ICON_ANIMATED_MOVE_FRAME;

    ISHELL_CancelTimer(pMe->m_pShell, (PFNNOTIFY)DrawFocusMoveAnimation, pMe);
    
    if(pMe->m_pAnimate == NULL)
    {
        nFrame = 0;
        pMe->m_pAnimate = ISHELL_LoadImage(pMe->m_pShell, ICON_ANI_1[theFocus]);
    }

    if(nFrame < (ICON_ANIMATED_MOVE_FRAME - 1) && pMe->m_pAnimate != NULL && (theFocus != thePrevFocus))
    {
        AEERect rect = {0};
        int xPos = pMe->m_IconFocus_Pt[thePrevFocus].x + (nFrame + 1)*(pMe->m_IconFocus_Pt[theFocus].x - pMe->m_IconFocus_Pt[thePrevFocus].x)/ICON_ANIMATED_MOVE_FRAME,
            yPos = pMe->m_IconFocus_Pt[thePrevFocus].y + (nFrame + 1)*(pMe->m_IconFocus_Pt[theFocus].y - pMe->m_IconFocus_Pt[thePrevFocus].y)/ICON_ANIMATED_MOVE_FRAME;

        if(pMe->m_pDevImage != NULL)
        {
            IDISPLAY_BitBlt(pMe->m_pDisplay, xOldPos, yOldPos,
                                ICON_ANIMATED_WIDTH, ICON_ANIMATED_HEIGHT, 
                                pMe->m_pDevImage, xOldPos, yOldPos, AEE_RO_COPY);
        }
        pMe->m_bMoveing = TRUE;
        SETAEERECT(&rect, xPos, yPos, ICON_ANIMATED_WIDTH, ICON_ANIMATED_HEIGHT);
        ERR("DrawFocusMoveAnimation:::::::::!1111111111111111",0,0,0);
        Appscommon_ResetBackground(pMe->m_pDisplay,pMe->m_pAnimate, 
#ifdef FEATURE_RANDOM_MENU_COLOR
                                                    pMe->m_nBgColor, 
#else
                                                    APPSCOMMON_BG_COLOR,
#endif
                                                    &rect, rect.x, rect.y);
        IDISPLAY_Update(pMe->m_pDisplay);
        nFrame++;
        ISHELL_SetTimer(pMe->m_pShell, ICON_ANIMATED_MOVE_RATE, (PFNNOTIFY)DrawFocusMoveAnimation, pMe);
    }
    else
    {
        nFrame = 0;
        pMe->m_bMoveing = FALSE;
        if(pMe->m_pDevImage != NULL)
        {
            IDISPLAY_BitBlt(pMe->m_pDisplay, xOldPos, yOldPos,
                                ICON_ANIMATED_WIDTH, ICON_ANIMATED_HEIGHT, 
                                pMe->m_pDevImage, xOldPos, yOldPos, AEE_RO_COPY);
        }
#ifdef FEATURE_FOCUS_ANIMATION
        ERR("DrawFocusMoveAnimation:::::::::222222222222222222",0,0,0);
        DrawFocusIconAnimation(pMe);
#else
        ERR("DrawFocusMoveAnimation:::::::::33333333333333333",0,0,0);
        DrawFocusIcon(pMe);
#endif
    }

}
#endif

#ifdef FEATURE_FOCUS_ANIMATION
/*=============================================================================
FUNCTION:  DrawFocusIconAnimation

DESCRIPTION: 

PARAMETERS:

RETURN VALUE:

COMMENTS:


=============================================================================*/
static void DrawFocusIconAnimation(MainMenu *pMe)
{
    int theFocus = pMe->m_nRow * MAX_MATRIX_COLS + pMe->m_nColumn;
    
    if (NULL == pMe)
    {
        return;
    }
            
    if (pMe->m_nIconAniFrameIdx == 0)
    {
        TitleBar_Param_type  titleBarParms;
        MEMSET( &titleBarParms, 0, sizeof( TitleBar_Param_type));
            
        titleBarParms.dwAlignFlags  = IDF_TEXT_TRANSPARENT | IDF_ALIGN_CENTER | IDF_ALIGN_MIDDLE;
        STRCPY( titleBarParms.strTitleResFile, MAINMENU_RES_FILE_LANG);
        titleBarParms.nTitleResID   = IDS_MAIN_MENU_TITLE_1 + theFocus;

        DrawTitleBar(pMe->m_pDisplay, &titleBarParms);
    }
    
    if(pMe->m_pAnimate == NULL)
    {
        pMe->m_pAnimate = ISHELL_LoadImage(pMe->m_pShell, ICON_ANI_1[theFocus]);
    }

	if( pMe->m_pAnimate != NULL)
   {
        IIMAGE_Draw(pMe->m_pImageFocus,
                         pMe->m_IconFocus_Pt[theFocus].x, 
                        pMe->m_IconFocus_Pt[theFocus].y);
		IIMAGE_Draw(pMe->m_pAnimate,
                         pMe->m_IconFocus_Pt[theFocus].x, 
                        pMe->m_IconFocus_Pt[theFocus].y);
	   IIMAGE_Release(pMe->m_pAnimate);
       pMe->m_pAnimate = NULL;
	   IDISPLAY_UpdateEx(pMe->m_pDisplay, TRUE);
	}
	//wlh 20090410 add end
}
#else
/*=============================================================================
FUNCTION:  DrawFocusIconAnimation

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
    titleBarParms.nTitleResID   = IDS_MAIN_MENU_TITLE_1 + theFocus;

    DrawTitleBar(pMe->m_pDisplay, &titleBarParms);
    
    if(pMe->m_pAnimate == NULL)
    {
        pMe->m_pAnimate = ISHELL_LoadImage(pMe->m_pShell, ICON_ANI_1[theFocus]);
    }

	if( pMe->m_pAnimate != NULL)
   {
        
		IIMAGE_Draw(pMe->m_pAnimate,
                         pMe->m_IconFocus_Pt[theFocus].x, 
                        pMe->m_IconFocus_Pt[theFocus].y);
	   IIMAGE_Release(pMe->m_pAnimate);
       pMe->m_pAnimate = NULL;
	   IDISPLAY_UpdateEx(pMe->m_pDisplay, TRUE);
	}
	}
}
//end added
#endif

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
	int i;
	AEERect turnrect;
	int lengthX,lengthY;//下一个宫格的距离
	int lengthXX,lengthYY;//下一个宫格的距离,绝对值
	int turnnum;//当前要滚动多少次
	int turnlengthx = 0;//每次x移动的距离
	int turnlengthy = 0;//每次y移动的距离
	int turnx = 0;//每次x移动的距离差价
	int turny = 0;//每次y移动的距离差价
	int x,y;
	IImage		*m_pImageTurns[4];//存放当前效果资源
    TitleBar_Param_type  titleBarParms;
    MEMSET( &titleBarParms, 0, sizeof( TitleBar_Param_type));
            
    titleBarParms.dwAlignFlags  = IDF_TEXT_TRANSPARENT | IDF_ALIGN_CENTER | IDF_ALIGN_MIDDLE;
    STRCPY( titleBarParms.strTitleResFile, MAINMENU_RES_FILE_LANG);
    titleBarParms.nTitleResID   = IDS_MAIN_MENU_TITLE_1 + theFocus;
#ifdef FEATURE_APP_NUMBERMANAGER
    if(theFocus == 9)
    {
        titleBarParms.nTitleResID = IDS_MAIN_MENU_TITLE_13;
    }
#endif
	//wlh end
    ERR("MainMenu_IconMenuHandler::::::::2222222",0,0,0);
	if((row != pMe->m_nRow) || (column != pMe->m_nColumn))//避免已经是焦点的情况
	{
 
	  	DrawMatrix(pMe);
		DrawTitleBar(pMe->m_pDisplay, &titleBarParms);	//wlh add 0404
		lengthX = ICON_ANIMATED_WIDTH * (column - pMe->m_nColumn);
	    lengthY = ICON_ANIMATED_HEIGHT* (row - pMe->m_nRow);
	
		turnrect.x	= pMe->m_IconFocus_Pt[theFocus].x;// + startx;// 8 + 4;//(50 - 34)/2 
		turnrect.y  = pMe->m_IconFocus_Pt[theFocus].y;// + starty;//7;//(50-36)/2
		turnrect.dx = ICON_ANIMATED_WIDTH;
		turnrect.dy = ICON_ANIMATED_HEIGHT;
	
		if(lengthX < 0)
		{
			lengthXX = 0 - lengthX;
		}
		else
		{
			lengthXX = lengthX;
		}
		if(lengthY < 0)
		{
			lengthYY = 0 - lengthY;
		}
		else
		{
			lengthYY = lengthY;
		}
	
		if(lengthXX > lengthYY)
		{
		  	turnnum = lengthXX/9;
		}
		else
		{
			turnnum = lengthYY/9;
		}

		turnlengthx = lengthXX/5;
		turnlengthy = lengthYY/5;

		m_pImageTurns[0] = pMe->m_pImageTurn[theFocus];
       // IIMAGE_Draw(pMe->m_pImageFocus,x,y);
		//IIMAGE_Draw(m_pImageTurns[0], x, y);
		IDISPLAY_UpdateEx(pMe->m_pDisplay,FALSE);
		DrawMatrix(pMe);
		DrawTitleBar(pMe->m_pDisplay, &titleBarParms);	//wlh add 0404
    }

    ISHELL_CancelTimer (pMe->m_pShell, NULL, ( void*)&pMe);
    
    // 绘制聚焦后矩阵初始界面
    SETAEERECT(&rect, pMe->m_IconFocus_Pt[theFocus].x, 
                                pMe->m_IconFocus_Pt[theFocus].y, 
                                ICON_ANIMATED_WIDTH, 
                                ICON_ANIMATED_HEIGHT);
#ifdef FEATURE_ICON_MOVE_ANIMATION
    if(pMe->m_bMoveing)
    {
        pMe->m_bMoveing = FALSE;
        if(pMe->m_pDevImage != NULL)
        {
            IDISPLAY_BitBlt(pMe->m_pDisplay, 0, TITLEBAR_HEIGHT,
                            pMe->m_rc.dx, pMe->m_rc.dy - TITLEBAR_HEIGHT, 
                            pMe->m_pDevImage, 0, TITLEBAR_HEIGHT, AEE_RO_COPY);
        }
    }
    else
#endif
    {
        MainMenu_DrawBackGround(pMe, &rect);
    
        if (pMe->m_pImageIcon[theFocus])
        {
            IIMAGE_Draw(pMe->m_pImageIcon[theFocus],
                            pMe->m_Icondefault_Pt[theFocus].x, 
                            pMe->m_Icondefault_Pt[theFocus].y);
        }
    }
    // 开始聚焦动画过程
#ifdef FEATURE_LCD_TOUCH_ENABLE//WLH ADD FOR LCD TOUCH
	if (pMe->m_pAnimate != NULL)
	{
		IIMAGE_Release(pMe->m_pAnimate);
		pMe->m_pAnimate = NULL;
	}
#endif//FEATURE_LCD_TOUCH_ENABLE
    // 开始聚焦动画过程
    setCursor(pMe, row, column);

#ifdef FEATURE_ICON_MOVE_ANIMATION
    DrawFocusMoveAnimation(pMe);
#else //FEATURE_ICON_MOVE_ANIMATION
#ifdef FEATURE_FOCUS_ANIMATION
    DrawFocusIconAnimation(pMe);
#else //FEATURE_FOCUS_ANIMATION
    DrawFocusIcon(pMe);
#endif
#endif
}
/*=============================================================================
FUNCTION:  setCursor

DESCRIPTION: 

PARAMETERS:

RETURN VALUE:

COMMENTS:

SIDE EFFECTS:

SEE ALSO:

=============================================================================*/
static void setCursor(MainMenu *pMe, int row, int column)
{
#ifdef FEATURE_ICON_MOVE_ANIMATION
    pMe->m_nPrevRow   = pMe->m_nRow;
    pMe->m_nPrevColumn = pMe->m_nColumn;
#endif
    pMe->m_nRow        = row;
    pMe->m_nColumn     = column;
#ifdef FEATURE_FOCUS_ANIMATION
    pMe->m_nIconAniFrameIdx      = 0;
#endif
    if(pMe->m_pAnimate != NULL)
    {
        IImage_Release(pMe->m_pAnimate);
        pMe->m_pAnimate  = NULL;
    }
}

#ifdef FEATRUE_SUPPORT_G_SENSOR
/*=============================================================================
FUNCTION:  MainMenu_MsgBoxHandler

DESCRIPTION:  在主菜单中添加对话框
PARAMETERS:

=============================================================================*/

static boolean  MainMenu_MsgBoxHandler(MainMenu *pMe, AEEEvent eCode, uint16 wParam, uint32 dwParam)
{
    static IStatic * pStatic = NULL;

    if (NULL == pStatic)
    {
        AEERect rect = {0};
        if (AEE_SUCCESS != ISHELL_CreateInstance(pMe->m_pShell,
                                                     AEECLSID_STATIC,
                                                     (void **)&pStatic))
        {
            return FALSE;
        }        
        ISTATIC_SetRect(pStatic, &rect);  
    }

    if ((NULL == pStatic) ||(NULL == pMe))
    {
        return FALSE;
    }

    switch(eCode)
    {
        case EVT_DIALOG_INIT:
            return TRUE;

        case EVT_DIALOG_START:
            (void) ISHELL_PostEventEx(pMe->m_pShell, 
                                    EVTFLG_ASYNC,
                                    AEECLSID_MAIN_MENU,
                                    EVT_USER_REDRAW,
                                    0, 
                                    0);

            return TRUE;

        case EVT_USER_REDRAW:
            {
                PromptMsg_Param_type  Msg_Param={0};
                AECHAR  wstrText[MSGBOX_MAXTEXTLEN] = {(AECHAR)'\0'};

                (void)ISHELL_LoadResString(pMe->m_pShell,
                                MAINMENU_RES_FILE_LANG,                                
                                pMe->m_wMsgResID,
                                wstrText,
                                sizeof(wstrText));
                                
                Msg_Param.ePMsgType = MESSAGE_CONFIRM;
                Msg_Param.pwszMsg = wstrText;

                Msg_Param.eBBarType = BTBAR_OK_CANCEL;

                
                DrawPromptMessage(pMe->m_pDisplay, pStatic, &Msg_Param);
            }

            IDISPLAY_UpdateEx(pMe->m_pDisplay, FALSE);
            #if 0
            (void) ISHELL_SetTimer(pMe->m_pShell,
                        PROMPTMSG_TIMER,
                        Mainmenu_DialogTimeout,
                        pMe);
            #endif
            
            return TRUE;

        case EVT_DIALOG_END:
                    #if 0
                     (void) ISHELL_CancelTimer(pMe->m_pShell,Morse_DialogTimeout, pMe);
                    #endif
            ISTATIC_Release(pStatic);
            pStatic = NULL;
            
            return TRUE;

        case EVT_KEY:
            switch( wParam)
            {
                case AVK_CLR:
                    ERR("AVK_CLR:::::::::::::::::::!111111111111",0,0,0);
                    CLOSE_DIALOG(DLGRET_CANCELED)
                    break;
                case AVK_INFO:
                case AVK_SELECT:
                    if(is_g_sportsapp_pedometer_bground_flag())
                    {
                        MainMenu_CloseSportBgRun(pMe);
                    }
                    else if(MainMenu_Get_Shake_OnOrOff(pMe))
                    {
                        MainMenu_Set_Shake_Disable(pMe);
                    }
                    CLOSE_DIALOG(DLGRET_GAME)
                    break;
                    default:
                        return TRUE;

            }
            return TRUE;
        
        case EVT_DISPLAYDIALOGTIMEOUT:
            CLOSE_DIALOG(DLGRET_GAME)
            return TRUE;

        default:
            break;
    }

    return FALSE;
}


#endif


/*=============================================================================
FUNCTION:  StartApplet

DESCRIPTION:  启动applet

PARAMETERS:  如果APPLET 有变动，只需改动次函数

=============================================================================*/
static boolean StartApplet(MainMenu *pMe, int i)
{
    int Result = FALSE;
#ifdef FEATURE_LCD_TOUCH_ENABLE//WLH ADD FOR LCD TOUCH
	if (pMe->m_pAnimate != NULL)
	{
		IIMAGE_Release(pMe->m_pAnimate);
		pMe->m_pAnimate = NULL;
	}
#endif//FEATURE_LCD_TOUCH_ENABLE
    switch(i)
    {
    #if defined (FEATURE_DISP_128X128)
    
    #endif
    #if defined (FEATURE_DISP_160X128)
         case 0:
         {
            
            Result = ISHELL_StartApplet(pMe->m_pShell, AEECLSID_ALARMCLOCK);
            break;
         }
        case 1:
            {
                IContactApp *ca = NULL;
                if(SUCCESS != ISHELL_CreateInstance(pMe->m_pShell,AEECLSID_APP_CONTACT, (void**)&ca))
                {
                    return FALSE;
                }
                else
                {
                    ICONTAPP_MainMenu(ca);
                    IContactApp_Release(ca);
                }
            }
            break;
        case 2:
            Result = ISHELL_StartApplet(pMe->m_pShell, AEECLSID_APP_UTK);
            break;
        case 3:
            Result = ISHELL_StartApplet(pMe->m_pShell, AEECLSID_CALCAPP);
            break;
        case 4:
            Result = ISHELL_StartApplet(pMe->m_pShell, AEECLSID_APP_RECENTCALL);
            break;
        case 5:
            Result = ISHELL_StartApplet(pMe->m_pShell, AEECLSID_WMSAPP);
            break;    
        case 6:
            Result = ISHELL_StartApplet(pMe->m_pShell, AEECLSID_APP_FMRADIO);
            break;
        case 7:
            Result = ISHELL_StartApplet(pMe->m_pShell, AEECLSID_APPMANAGER);
            break;
        case 8:
            Result = ISHELL_StartApplet(pMe->m_pShell, AEECLSID_SCHEDULEAPP);
            break;

        case 9:
            Result = ISHELL_StartApplet(pMe->m_pShell, AEECLSID_MEDIAGALLERY);
            break;
        case 10:
            Result = ISHELL_StartApplet(pMe->m_pShell, AEECLSID_APP_MUSICPLAYER);
            break;
        case 11:
            Result = ISHELL_StartApplet(pMe->m_pShell, AEECLSID_VIDEOPLAYER);
            break;
#endif
        default:
            break;

    }
    setCursor(pMe, i/MAX_MATRIX_COLS, i%MAX_MATRIX_COLS);
    return TRUE;
}


/*=============================================================================
FUNCTION:  StartApplet

DESCRIPTION:  启动applet

PARAMETERS:  如果APPLET 有变动，只需改动次函数

=============================================================================*/
#ifdef FEATRUE_SUPPORT_G_SENSOR
static void Mainmenu_DialogTimeout(void *pme)
{
    MainMenu *pMe = (MainMenu*)pme;

    if (NULL == pMe)
    {
        return;
    }

    (void) ISHELL_PostEvent(pMe->m_pShell,
                            AEECLSID_MAIN_MENU,
                            EVT_DISPLAYDIALOGTIMEOUT,
                            0,
                            0);
}

/*=============================================================================
FUNCTION:  MainMenu_Set_Shake_Disable

DESCRIPTION:  将shake 设置成disable

PARAMETERS:  如果APPLET 有变动，只需改动次函数

=============================================================================*/
boolean MainMenu_Set_Shake_Disable(MainMenu *pMe)
{
    
    dword shake;
    boolean isShakeOn = FALSE;
    (void)ICONFIG_GetItem(pMe->m_pConfig,CFGI_GSENSOR,&shake,sizeof(shake));
    if(shake)
    {
        
        if(shake & OEMNV_SHAKE_MUSIC_MASK)
        {
            isShakeOn = FALSE;                                      
            /*Set Shake Reset*/
            mmi_g_sensor_process(G_SENSOR_SHAKE_DISABLE);

            mmi_g_sensor_process(G_SENSOR_VIDEO_CIRC_DISABLE); 
        }
        
        else if(shake & OEMNV_SHAKE_FM_RADIO_MASK)
        {
            isShakeOn = FALSE;                                      
            mmi_g_sensor_process(G_SENSOR_SHAKE_DISABLE);
                        mmi_g_sensor_process(G_SENSOR_VIDEO_CIRC_DISABLE);
        }
        else if(shake & OEMNV_SHAKE_WALLPAPER_MASK)
        {
            isShakeOn = FALSE;                                      
            mmi_g_sensor_process(G_SENSOR_SHAKE_DISABLE);
                        mmi_g_sensor_process(G_SENSOR_VIDEO_CIRC_DISABLE);          
        
        }
        else if(shake & OEMNV_SHAKE_SNOOZE_ALARM_MASK)
        {
            isShakeOn = FALSE;                                      
            mmi_g_sensor_process(G_SENSOR_SHAKE_DISABLE);
                        mmi_g_sensor_process(G_SENSOR_VIDEO_CIRC_DISABLE);              
        }
        else if(shake & OEMNV_SHAKE_VIDEO_MASK)
        {
            isShakeOn = FALSE;      
            mmi_g_sensor_process(G_SENSOR_VIDEO_CIRC_DISABLE);
                        mmi_g_sensor_process(G_SENSOR_SHAKE_DISABLE);
            
        }

            (void)ICONFIG_SetItem(pMe->m_pConfig,CFGI_SHAKE_MUSIC_CHECK,&isShakeOn,
                            sizeof(isShakeOn));
            (void)ICONFIG_SetItem(pMe->m_pConfig,CFGI_SHAKE_FM_RADIO_CHECK,&isShakeOn,
                                    sizeof(isShakeOn));
            (void)ICONFIG_SetItem(pMe->m_pConfig,CFGI_SHAKE_WALLPAPER_CHECK,&isShakeOn,
                                    sizeof(isShakeOn));
            
            (void)ICONFIG_SetItem(pMe->m_pConfig,CFGI_SHAKE_SNOOZE_ALARM_CHECK,&isShakeOn,
                                    sizeof(isShakeOn));

            (void)ICONFIG_SetItem(pMe->m_pConfig,CFGI_SHAKE_VIDEO_CHECK,&isShakeOn,
                                    sizeof(isShakeOn)); 

            return TRUE;            
    
    }
    else
    {
        return FALSE;
    }
        

    
}

/*=============================================================================
FUNCTION:  MainMenu_Get_Shake_OnOrOff

DESCRIPTION:  将shake 设置成disable

PARAMETERS:  如果APPLET 有变动，只需改动次函数

=============================================================================*/

boolean MainMenu_Get_Shake_OnOrOff(MainMenu *pMe)
{
    dword shake;
    (void)ICONFIG_GetItem(pMe->m_pConfig,CFGI_GSENSOR,&shake,sizeof(shake));

    if(shake)
    {
        return TRUE;
    }
    else
    {
        return FALSE;
    }

}
/*=============================================================================
FUNCTION:  MainMenu_CloseSportBgRun

DESCRIPTION:  将shake 设置成disable

PARAMETERS:  如果APPLET 有变动，只需改动次函数

=============================================================================*/
void MainMenu_CloseSportBgRun(MainMenu *pMe)
{
    (void) ISHELL_PostEvent( pMe->m_pShell,AEECLSID_SPORTSAPP,
                                EVT_CLOSEAPP,
                                0,
                                0);
    
}

#endif


/*=============================================================================
FUNCTION:  MainMenu_MainMenuService

DESCRIPTION:  外部接口调用，快速进入到MAINMENU某一个菜单列表

PARAMETERS:    
            eStype: 启动app的方式

=============================================================================*/

static int  MainMenu_MainMenuService( IMainMenu *pi, MainMenuServiceType eStype)
{
    char  *args = NULL;
    MainMenu *pMe = (MainMenu*)pi;
    int    nRet;  

    if (ISHELL_ActiveApplet(pMe->m_pShell) == AEECLSID_MAIN_MENU)
    {
        // applet is already running
        return EFAILED;
    }    
    
    switch (eStype)
    {
        case STARTMETHOD_PLAYER: 
            args = (char *)MALLOC(sizeof(char));
            if(args)
            {
                args[0] = STARTARGPREFIX_VIEWPLAYER;
            }
            break;
        
        case STARTMETHOD_DATA:
            args = (char *)MALLOC(sizeof(char));
            if(args)
            {
                args[0] = STARTARGPREFIX_VIEWDATA;
            }
            break;

        default:
            break;
    }

    if(args == NULL)
    {
        return EFAILED;
    }

    nRet = ISHELL_StartAppletArgs(pMe->m_pShell, AEECLSID_MAIN_MENU, args);
        
    FREEIF(args);
    args = NULL;
    
    return nRet;    
}
void MainMenu_DraImage(MainMenu *pMe,int16 index,int16 imgid)
{
        
        if(pMe->m_pImageIcon[index] != NULL)
        {
            IIMAGE_Release(pMe->m_pImageIcon[index]);
            pMe->m_pImageIcon[index] = NULL;
            
            pMe->m_pImageIcon[index] = ISHELL_LoadResImage(pMe->m_pShell,
                                                MAINMENU_RES_FILE_IMAGE,
                                                imgid);
        }
}
