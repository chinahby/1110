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
#include "appscommonimages.brh"
#ifdef FEATURE_RANDOM_MENU_REND
#include "DisplayRendUtils.h"
#endif
#if !defined( AEE_SIMULATOR)
#include "AEERUIM.h" 
#include "OEMCFGI.h"
#endif //#if !defined( AEE_SIMULATOR)

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

static int StartApplet(MainMenu *pMe, int i);

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
static boolean  MainMenu_ListMenuHandler(MainMenu *pMe, AEEEvent eCode, uint16 wParam, uint32 dwParam);



/*==============================================================================
                              
                              本地（静态）数据
                              
==============================================================================*/



//wlh 20090410 add  end 移动效果
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

    pMe->m_MainSel  = 0;
    
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

	if (pMe->m_pIAnn != NULL)
	{
		IANNUNCIATOR_SetFieldIsActiveEx(pMe->m_pIAnn,FALSE);
	}
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
            if (as->pszArgs != NULL)
            {
                switch(as->pszArgs[0])
                {

                    case STARTARGPREFIX_VIEWDATA:
                        pMe->m_currState = MAINST_DATA;
                        break;

                    default:
                        break;
                }
            }
            MainMenu_RunFSM(pMe);
            return TRUE;

        case EVT_APP_STOP:
            {        
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
       //         ERR("MainMenu_HandleEvent EVT_APP_RESUME",0,0,0);
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

        case EVT_KEY_PRESS:
        case EVT_KEY_RELEASE:
        case EVT_COMMAND:
            return MainMenu_RouteDialogEvt(pMe,eCode,wParam,dwParam); 
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
            MainMenu_ShowDialog(pMe, IDD_LIST_MENU);
            return NFSMACTION_WAIT;
            
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
 //   ERR("MAINST_EXIT_Handler Start",0,0,0);
    (void) ISHELL_CloseApplet(pMe->m_pShell, FALSE);
 //   ERR("MAINST_EXIT_Handler End",0,0,0);
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
 //   ERR("MainMenu_ShowDialog Start",0,0,0);
    // 每次最多打开一个对话框
    if (ISHELL_GetActiveDialog(pMe->m_pShell) != NULL)
    {
        // 如果当前已经有对话框被打开，直接返回
        return;
    }
    
    if (NULL != pMe->m_pDisplay)
    {
        AEEDeviceInfo di={0,};
        (void)IDISPLAY_SetPrefs(pMe->m_pDisplay, "a:1", STRLEN("a:1"));
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
        case IDD_LIST_MENU:
            return MainMenu_ListMenuHandler(pMe, eCode, wParam, dwParam);

        default:
            return FALSE;
    }
}



/*=============================================================================
FUNCTION:  MainMenu_ListMenuHandler

DESCRIPTION:   列表菜单

PARAMETERS:

=============================================================================*/
static boolean MainMenu_ListMenuHandler(MainMenu *pMe, AEEEvent eCode, uint16 wParam, uint32 dwParam)
{
    PARAM_NOT_REF(dwParam)
    IMenuCtl *pMenu = (IMenuCtl*)IDIALOG_GetControl(pMe->m_pActiveIDlg,IDC_MENU_LIST);
        
    if (pMenu == NULL)
    {
        return FALSE;
    }

    switch (eCode)
    {
        case EVT_DIALOG_INIT:
						//add by yangdecai
			{
				AECHAR WTitle[40] = {0};
				(void)ISHELL_LoadResString(pMe->m_pShell,
                        MAINMENU_RES_FILE_LANG,                                
                        IDS_MENU_LIST,
                        WTitle,
                        sizeof(WTitle));
				IANNUNCIATOR_SetFieldText(pMe->m_pIAnn,WTitle);				
				IANNUNCIATOR_Redraw(pMe->m_pIAnn);
            }
     //       ERR("MainMenu_ListMenuHandler EVT_DIALOG_INIT",0,0,0);
            //IMENUCTL_SetTitle(pMenu, MAINMENU_RES_FILE_LANG, IDS_MENU_LIST, NULL);                
            #if defined (FEATURE_VERSION_C337) || defined(FEATURE_VERSION_IC241A_MMX)
			IMENUCTL_AddItem(pMenu, MAINMENU_RES_FILE_LANG,IDS_PHONEBOOK, IDS_PHONEBOOK, NULL, 0);
			#else
            IMENUCTL_AddItem(pMenu, MAINMENU_RES_FILE_LANG,IDS_MAIN_MENU_CONTACTS, IDS_MAIN_MENU_CONTACTS, NULL, 0);
			#endif
            IMENUCTL_AddItem(pMenu, MAINMENU_RES_FILE_LANG,IDS_MAIN_MENU_RECENTCALLS, IDS_MAIN_MENU_RECENTCALLS, NULL, 0);
            IMENUCTL_AddItem(pMenu, MAINMENU_RES_FILE_LANG,IDS_MAIN_MENU_MESSAGES, IDS_MAIN_MENU_MESSAGES, NULL, 0);
            IMENUCTL_AddItem(pMenu, MAINMENU_RES_FILE_LANG,IDS_MAIN_MENU_UTK, IDS_MAIN_MENU_UTK, NULL, 0);
            IMENUCTL_AddItem(pMenu, MAINMENU_RES_FILE_LANG,IDS_MAIN_MENU_ALARM, IDS_MAIN_MENU_ALARM, NULL, 0);			
            IMENUCTL_AddItem(pMenu, MAINMENU_RES_FILE_LANG,IDS_MAIN_MENU_FM, IDS_MAIN_MENU_FM, NULL, 0);
            IMENUCTL_AddItem(pMenu, MAINMENU_RES_FILE_LANG,IDS_MAIN_MENU_SETTINGS, IDS_MAIN_MENU_SETTINGS, NULL, 0);

           // IDIALOG_SetProperties((IDialog *)dwParam, DLG_NOT_REDRAW_AFTER_START);
            return TRUE;
            
        case EVT_DIALOG_START:
            {  
                int i;
                uint16 wID;
                AECHAR pwsz[67] = {0};
                AECHAR pstr[64] = {0};
                AECHAR wsFmt[5] = {0};
                
                (void)STRTOWSTR("%d. ",wsFmt,sizeof(wsFmt));
                for (i=0;i<IMENUCTL_GetItemCount(pMenu);i++)
                {
                    wID = IMENUCTL_GetItemID(pMenu, i);
                    WSPRINTF(pwsz,sizeof(pwsz),wsFmt,i+1);
                    
                    ISHELL_LoadResString( pMe->m_pShell,
                          MAINMENU_RES_FILE_LANG,
                          wID,
                          pstr,
                          sizeof(pstr));
                    WSTRLCAT(pwsz,pstr,sizeof(pwsz));
                    IMENUCTL_SetItemText(pMenu, wID, NULL, NULL, pwsz);
                }
                IMENUCTL_SetProperties(pMenu, MP_UNDERLINE_TITLE|MP_WRAPSCROLL);
                IMENUCTL_SetOemProperties( pMenu, OEMMP_USE_MENU_STYLE);
                IMENUCTL_SetBottomBarType(pMenu,BTBAR_SELECT_BACK);
                IMENUCTL_SetSel(pMenu, pMe->m_MainSel);
                (void) ISHELL_PostEvent(pMe->m_pShell, AEECLSID_MAIN_MENU, EVT_USER_REDRAW,0,0);
            }
            return TRUE;
            
        case EVT_USER_REDRAW:
            //(void)IMENUCTL_Redraw(pMenu);    //dele by yangdecai
            return TRUE;
            
        case EVT_DIALOG_END:
            return TRUE;

        case EVT_KEY:
            switch(wParam)
            {
                case AVK_1:
                case AVK_2:
                case AVK_3:
                case AVK_4:
                case AVK_5:
                case AVK_6:
                case AVK_7:
                case AVK_8:
                case AVK_9:
                    StartApplet(pMe, IMENUCTL_GetItemID(pMenu, (wParam - AVK_1)));
                    return TRUE;
                case AVK_STAR:
                    StartApplet(pMe, IMENUCTL_GetItemID(pMenu, 9));
                    return TRUE;
                case AVK_0:
                    StartApplet(pMe, IMENUCTL_GetItemID(pMenu, 10));
                    return TRUE;
                case AVK_POUND:
                    StartApplet(pMe, IMENUCTL_GetItemID(pMenu, 11));
                    return TRUE;
             
                case AVK_CLR:
                    CLOSE_DIALOG(DLGRET_CANCELED)
                    return TRUE;
                    
                default:
                    break;
          
            }
            return TRUE;
            
        case EVT_COMMAND:
            pMe->m_MainSel = wParam;
            StartApplet(pMe, wParam);
            return TRUE;
            
        default:
            break;
    }             
    return FALSE;
}




/*=============================================================================
FUNCTION:  StartApplet

DESCRIPTION:  启动applet

PARAMETERS:  如果APPLET 有变动，只需改动次函数

=============================================================================*/
static int StartApplet(MainMenu *pMe, int i)
{
    int Result = EUNSUPPORTED;
    switch(i)
    {
    	#if defined (FEATURE_VERSION_C337) || defined(FEATURE_VERSION_IC241A_MMX)
		case IDS_PHONEBOOK:
		#endif
        case IDS_MAIN_MENU_CONTACTS:
        {
            IContactApp *ca = NULL;
            if(SUCCESS != ISHELL_CreateInstance(pMe->m_pShell, AEECLSID_APP_CONTACT, (void**)&ca))
            {
                return FALSE;
            }
            else
            {
                ICONTAPP_MainMenu(ca);
                IContactApp_Release(ca);
            }
            break;
        }
        case IDS_MAIN_MENU_RECENTCALLS:
            Result = ISHELL_StartApplet(pMe->m_pShell, AEECLSID_APP_RECENTCALL);
            break;
        case IDS_MAIN_MENU_MESSAGES:
            Result = ISHELL_StartApplet(pMe->m_pShell, AEECLSID_WMSAPP);
            break;
        case IDS_MAIN_MENU_UTK:
			Result = ISHELL_StartApplet(pMe->m_pShell, AEECLSID_APP_UTK);
            break;   

        case IDS_MAIN_MENU_ALARM:
            Result = ISHELL_StartApplet(pMe->m_pShell, AEECLSID_ALARMCLOCK);
            break;
		case IDS_MAIN_MENU_FM:
			Result = ISHELL_StartApplet(pMe->m_pShell, AEECLSID_APP_FMRADIO);
			break;

        case IDS_MAIN_MENU_SETTINGS:			
            Result = ISHELL_StartApplet(pMe->m_pShell, AEECLSID_APP_SETTINGMENU);
            break;			

    }
    return Result;
}

