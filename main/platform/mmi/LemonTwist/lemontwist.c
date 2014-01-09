/** ----------------------------------------------------------------------------
 *lemontwist.c
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
#include "AppComFunc.h"
#include "AEEShell.h"
#include "AEE_OEM.h"
#include "AEEModGen.h"
#include "AEEModTable.h"
#include "AEEMenu.h"
#include "AEEStdLib.h"
#include "AEEAnnunciator.h"
#include "lemontwist.h"
#include "lemontwist_priv.h" 

#include "ContApp.h"

#if defined(FEATURE_WMS_APP)
#include "WMSApp.h"
#endif

#include "Appscommon.h"
#include "appscommon.brh"
#include "appscommonimages.brh"

#if !defined( AEE_SIMULATOR)
#include "AEERUIM.h" 
#include "OEMCFGI.h"
#endif //#if !defined( AEE_SIMULATOR)
#include "err.h"
#define PARAM_NOT_REF(x)

/*==============================================================================

                              本地函数声明

==============================================================================*/
static LemontwistMod gLemontwistMod;
static const VTBL( IModule) gModFuncs =
{
    LemontwistMod_AddRef,
    LemontwistMod_Release,
    LemontwistMod_CreateInstance,
    LemontwistMod_FreeResources
};

static uint32  Lemontwist_AddRef( ILemontwist *pi);

static uint32  Lemontwist_Release ( ILemontwist *pi);

static boolean Lemontwist_HandleEvent( ILemontwist *pi,
    AEEEvent eCode,
    uint16  wParam,
    uint32 dwParam
);




static Lemontwist gLemontwist={0};

static const VTBL( ILemontwist) gLemontwistMethods =
{
    Lemontwist_AddRef,
    Lemontwist_Release,
    Lemontwist_HandleEvent,
};

int LemontwistMod_Load( IShell *pIShell, void *ph, IModule **ppMod);

int LemontwistMod_New( int16 nSize,
    IShell              *pIShell,
    void                *ph,
    IModule             **ppMod,
    PFNMODCREATEINST    pfnMC,
    PFNFREEMODDATA      pfnMF
);

static int Lemontwist_New( IShell *ps, IModule *pIModule, ILemontwist **ppObj);

static int Lemontwist_InitAppData(Lemontwist *pMe);

static void Lemontwist_FreeAppData(Lemontwist *pMe);

static void Lemontwist_RunFSM(Lemontwist *pMe);

static void Lemontwist_POPTimeOut(Lemontwist *pMe);

static int StartApplet(Lemontwist *pMe, int i);

boolean Lemontwist_RouteDialogEvt(Lemontwist *pMe,
    AEEEvent    eCode,
    uint16  wParam,
    uint32 dwParam
);

NextFSMAction Lemontwist_ProcessState(Lemontwist *pMe);

void Lemontwist_ShowDialog(Lemontwist  *pMe,  uint16 dlgResId);

// MAINST_MAIN 状态处理函数
static NextFSMAction MAINST_LEMONTWIST_Handler(Lemontwist *pMe);
static NextFSMAction MAINST_LEMONTWIST_ZONE_Handler(Lemontwist *pMe);
static NextFSMAction MAINST_LEMONTWIST_LIVE_Handler(Lemontwist *pMe);
static NextFSMAction MAINST_LEMONTWIST_PULL_Handler(Lemontwist *pMe);
static NextFSMAction MAINST_LEMONTWIST_EMPLOYMENT_Handler(Lemontwist *pMe);
static NextFSMAction MAINST_LEMONTWIST_VOICE_Handler(Lemontwist *pMe);
static NextFSMAction MAINST_LEMONTWIST_FUN_Handler(Lemontwist *pMe);
static NextFSMAction MAINST_LEMONTWIST_MULTI_Handler(Lemontwist *pMe);
static NextFSMAction MAINST_LEMONTWIST_MVDO_Handler(Lemontwist *pMe);
static NextFSMAction MAINST_LEMONTWIST_HELP_Handler(Lemontwist *pMe);

static NextFSMAction MAINST_LEMONTWIST_LIVE_MSG_Handler(Lemontwist *pMe);



//MAINST_EXIT  状态处理函数
static NextFSMAction LEMONTWIST_EXIT_Handler(Lemontwist *pMe);
static boolean Lemontwist_ListMenuHandler(Lemontwist *pMe, AEEEvent eCode, uint16 wParam, uint32 dwParam);
static boolean Lemontwist_ZoneHandler(Lemontwist *pMe, AEEEvent eCode, uint16 wParam, uint32 dwParam);
static boolean Lemontwist_LiveHandler(Lemontwist *pMe, AEEEvent eCode, uint16 wParam, uint32 dwParam);
static boolean Lemontwist_PullHandler(Lemontwist *pMe, AEEEvent eCode, uint16 wParam, uint32 dwParam);
static boolean Lemontwist_EmploymentHandler(Lemontwist *pMe, AEEEvent eCode, uint16 wParam, uint32 dwParam);
static boolean Lemontwist_VoiceHandler(Lemontwist *pMe, AEEEvent eCode, uint16 wParam, uint32 dwParam);
static boolean Lemontwist_FunHandler(Lemontwist *pMe, AEEEvent eCode, uint16 wParam, uint32 dwParam);
static boolean Lemontwist_MultiHandler(Lemontwist *pMe, AEEEvent eCode, uint16 wParam, uint32 dwParam);
static boolean Lemontwist_MvdoHandler(Lemontwist *pMe, AEEEvent eCode, uint16 wParam, uint32 dwParam);
static boolean Lemontwist_HelpHandler(Lemontwist *pMe, AEEEvent eCode, uint16 wParam, uint32 dwParam);


static boolean Lemontwist_LiveMsgHandler(Lemontwist *pMe, AEEEvent eCode, uint16 wParam, uint32 dwParam);

static __inline void Lemontwist_DrawSoftkey(Lemontwist* pMe, BottomBar_e_Type eSoftkeyType);


/*==============================================================================
                              
                              本地（静态）数据
                              
==============================================================================*/




/*=============================================================================
FUNCTION:  LemontwistMod_Load

DESCRIPTION: 

PARAMETERS:

RETURN VALUE:

COMMENTS:

SIDE EFFECTS:

SEE ALSO:

=============================================================================*/
int LemontwistMod_Load( IShell *pIShell, void *ph, IModule **ppMod)
{
    MSG_FATAL("***zzg LemontwistMod_Load***", 0, 0, 0);
    return LemontwistMod_New( sizeof(LemontwistMod),
                    pIShell,
                    ph,
                    ppMod,
                    NULL,
                    NULL);
}

/*=============================================================================
FUNCTION:  LemontwistMod_New

DESCRIPTION: 

PARAMETERS:

RETURN VALUE:

COMMENTS:

SIDE EFFECTS:

SEE ALSO:

=============================================================================*/
int  LemontwistMod_New( int16 nSize,
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

    MSG_FATAL("***zzg LemontwistMod_New***", 0, 0, 0);

    if( NULL == ppMod)
    {
        return EFAILED;
    }

    MEMSET( &gLemontwistMod, 0, sizeof(LemontwistMod));

    INIT_VTBL( &gLemontwistMod, IModule, gModFuncs);
    gLemontwistMod.referenceCounter = 1;
    *ppMod = ( IModule *)&gLemontwistMod;
    return AEE_SUCCESS;
}

/*=============================================================================
FUNCTION:  LemontwistMod_AddRef

DESCRIPTION: 

PARAMETERS:

RETURN VALUE:

COMMENTS:

SIDE EFFECTS:

SEE ALSO:

=============================================================================*/
static uint32 LemontwistMod_AddRef( IModule *po)
{
    return( ++( ( LemontwistMod *)po)->referenceCounter);
}

/*=============================================================================
FUNCTION:  LemontwistMod_Release

DESCRIPTION: 

PARAMETERS:

RETURN VALUE:

COMMENTS:

SIDE EFFECTS:

SEE ALSO:

=============================================================================*/
static uint32 LemontwistMod_Release( IModule *po)
{
    if ( ( ( LemontwistMod *) po)->referenceCounter == 0)
    {
        
        return 0;
    }
    return( --( ( LemontwistMod *)po)->referenceCounter);
}

/*=============================================================================
FUNCTION:  Lemontwist_CreateInstance

DESCRIPTION: 

PARAMETERS:

RETURN VALUE:

COMMENTS:

SIDE EFFECTS:

SEE ALSO:

=============================================================================*/
static int  LemontwistMod_CreateInstance( IModule *po,
    IShell *pIShell,
    AEECLSID ClsId,
    void **ppObj
)
{
    MSG_FATAL("***zzg LemontwistMod_CreateInstance Start***",0,0,0);
    
    *ppObj = NULL;

    if( ClsId != AEECLSID_LEMON_TWIST || Lemontwist_New( pIShell, po, ( ILemontwist**)ppObj) != SUCCESS)
    {
        return EFAILED;
    }
    MSG_FATAL("LemontwistMod_CreateInstance End",0,0,0);
    return SUCCESS;
}

/*=============================================================================
FUNCTION:  Lemontwist_FreeResources

DESCRIPTION: 

PARAMETERS:

RETURN VALUE:

COMMENTS:

SIDE EFFECTS:

SEE ALSO:

=============================================================================*/
static void LemontwistMod_FreeResources( IModule *po, IHeap *ph, IFileMgr *pfm)
{
}

/*=============================================================================
FUNCTION:  Lemontwist_New

DESCRIPTION: 

PARAMETERS:

RETURN VALUE:

COMMENTS:

SIDE EFFECTS:

SEE ALSO:

=============================================================================*/
static int Lemontwist_New( IShell *ps, IModule *pIModule, ILemontwist **ppObj)
{
    int retVal = SUCCESS;
    MSG_FATAL("***zzg Lemontwist_New Start***",0,0,0);
    if( 0 == gLemontwist.referenceCounter)
    {

        if( pIModule == NULL || ps == NULL)
        {
            return EFAILED;
        }

        MEMSET(&gLemontwist, 0, sizeof(gLemontwist));
        INIT_VTBL(&gLemontwist, ILemontwist, gLemontwistMethods);

       
        gLemontwist.m_pShell     = ps;
        gLemontwist.m_pModule    = pIModule;
        retVal = Lemontwist_InitAppData(&gLemontwist);
        MSG_FATAL("Lemontwist_New retVal=%d",retVal,0,0);
        if(retVal != SUCCESS)
        {
            Lemontwist_FreeAppData((Lemontwist*)&gLemontwist);
            return retVal;
        }
        (void) ISHELL_AddRef( ps);
        (void) IMODULE_AddRef( pIModule);
        gLemontwist.referenceCounter  = 0;

        
    }

    ++ gLemontwist.referenceCounter;
    *ppObj = ( ILemontwist*)&gLemontwist;
    MSG_FATAL("Lemontwist_New retVal=%d",retVal,0,0);
    return retVal;
}

/*==============================================================================
函数:
    Lemontwist_InitAppData

说明:
    初始化 Lemontwist Applet 结构数据缺省值。

参数:
    pMe [in]: 指向Lemontwist Applet 对象结构的指针。该结构包含小程序的特定信息。

返回值:
    EFAILED: 初始化失败。
    SUCCESS: 初始化成功。

备注:

==============================================================================*/
static int Lemontwist_InitAppData(Lemontwist *pMe)
{
    if (NULL == pMe)
    {
        return EFAILED;
    }
    
    pMe->m_MainSel  = 0;
    
    if (ISHELL_CreateInstance(pMe->m_pShell, AEECLSID_DISPLAY, 
            (void **) &pMe->m_pDisplay) != SUCCESS)
    {
        MSG_FATAL("Lemontwist_InitAppData EFAILED",0,0,0);
        return EFAILED;
    }
    
    if (AEE_SUCCESS != ISHELL_CreateInstance(pMe->m_pShell,AEECLSID_RUIM,
                                            (void **)&pMe->m_pIRUIM))
    {
       return FALSE;
    }
    if (AEE_SUCCESS != ISHELL_CreateInstance(pMe->m_pShell,AEECLSID_CONFIG,
                                            (void **)&pMe->m_pConfig))
    {
       return FALSE;
    }
    
	if (AEE_SUCCESS != ISHELL_CreateInstance(pMe->m_pShell,AEECLSID_ANNUNCIATOR,(void **)&pMe->m_pIAnn))
    {
        MSG_FATAL("Lemontwist_InitAppData EFAILED",0,0,0);
        return EFAILED;
    }
    return SUCCESS;
}

/*==============================================================================
函数:
    LemontwistFreeAppData

说明:
    释放 Lemontwist Applet 相关资源。

参数:
    pMe [in]: Lemontwist Applet 对象结构的指针。该结构包含小程序的特定信息。

返回值:
    none

备注:

==============================================================================*/
static void Lemontwist_FreeAppData(Lemontwist *pMe)
{
    if (NULL == pMe)
    {
        return;
    }

    pMe->m_eAppStatus = LEMONTWIST_STOP; 
    
    if (pMe->m_pDisplay != NULL)
    {
        (void) IDISPLAY_Release(pMe->m_pDisplay);
        pMe->m_pDisplay = NULL;
    }
	if(pMe->m_pIAnn)
    {
        IANNUNCIATOR_Release(pMe->m_pIAnn);
        pMe->m_pIAnn = NULL;
    }
    
    if (pMe->m_pConfig)
    {
        ICONFIG_Release(pMe->m_pConfig);
        pMe->m_pConfig = NULL;
    }
    if (pMe->m_pIRUIM != NULL)
    {
        IRUIM_Release(pMe->m_pIRUIM);
        pMe->m_pIRUIM = NULL;
    }

}

/*=============================================================================
FUNCTION:  Lemontwist_AddRef

DESCRIPTION: 

PARAMETERS:

RETURN VALUE:

COMMENTS:

SIDE EFFECTS:

SEE ALSO:

=============================================================================*/
static uint32  Lemontwist_AddRef( ILemontwist *pi)
{
    return ( ++gLemontwist.referenceCounter);
}

/*=============================================================================
FUNCTION:  Lemontwist_Release

DESCRIPTION: 

PARAMETERS:

RETURN VALUE:

COMMENTS:

SIDE EFFECTS:

SEE ALSO:

=============================================================================*/
static uint32  Lemontwist_Release( ILemontwist *pi)
{
    register Lemontwist *pMe = (Lemontwist*)pi;
   
    if (pMe->referenceCounter == 0)
    {
        return 0;
    }
    
    if( --pMe->referenceCounter)
    {
        return pMe->referenceCounter;
    }


    Lemontwist_FreeAppData(pMe);

    (void) ISHELL_Release(pMe->m_pShell);
    (void) IMODULE_Release(pMe->m_pModule);
   
    return 0;
}

/*==============================================================================
函数:
    Lemontwist_RunFSM

说明:
    运行有限状态机引擎。

参数:
    pMe [in]: 指向Application Applet对象结构的指针。该结构包含小程序的特定信息。
       
返回值:
    无。

备注:

==============================================================================*/
static void Lemontwist_RunFSM(Lemontwist *pMe)
{
    NextFSMAction nextFSMAction = NFSMACTION_WAIT;
    for ( ; ; )
    {
        nextFSMAction = Lemontwist_ProcessState(pMe);

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
/*==============================================================================
函数:
     Lemontwist_RouteDialogEvt
说明:
       函数将BREW事件路由给当前活动对话框的事件处理函数。

参数:
       pMe [In]: 指向Lemontwist Applet对象结构的指针,该结构包含小程序的特定信息.
       eCode [in]：事件代码。
       wParam [in]：与事件eCode关联的数据。
       dwParam [in]：与事件eCode关联的数据。

返回值:
       TRUE：传入事件被处理。
       FALSE：传入事件没被处理。

备注:

==============================================================================*/
boolean Lemontwist_RouteDialogEvt(Lemontwist *pMe,
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
        case IDD_LEMONTWIST_DIALOGS:
            return Lemontwist_ListMenuHandler(pMe, eCode, wParam, dwParam);  
        case IDD_LEMONTWIST_ZONE:
            return Lemontwist_ZoneHandler(pMe, eCode, wParam, dwParam);
        case IDD_LEMONTWIST_LIVE:
            return Lemontwist_LiveHandler(pMe, eCode, wParam, dwParam);
        case IDD_LEMONTWIST_PULL:
            return Lemontwist_PullHandler(pMe, eCode, wParam, dwParam);  
        case IDD_LEMONTWIST_EMPLOYMENT:
            return Lemontwist_EmploymentHandler(pMe, eCode, wParam, dwParam);  
        case IDD_LEMONTWIST_VOICE:
            return Lemontwist_VoiceHandler(pMe, eCode, wParam, dwParam);  
        case IDD_LEMONTWIST_FUN:
            return Lemontwist_FunHandler(pMe, eCode, wParam, dwParam);  
        case IDD_LEMONTWIST_MULTI:
            return Lemontwist_MultiHandler(pMe, eCode, wParam, dwParam);  
        case IDD_LEMONTWIST_MVDO:
            return Lemontwist_MvdoHandler(pMe, eCode, wParam, dwParam);  
        case IDD_LEMONTWIST_HELP:
            return Lemontwist_HelpHandler(pMe, eCode, wParam, dwParam);   
        case IDD_LEMONTWIST_LIVE_MSG:
            return Lemontwist_LiveMsgHandler(pMe, eCode, wParam, dwParam);    
        default:
            return FALSE;
    }
}


/*=============================================================================
FUNCTION:  Lemontwist_HandleEvent

DESCRIPTION: 

PARAMETERS:

RETURN VALUE:

COMMENTS:

SIDE EFFECTS:

SEE ALSO:

=============================================================================*/
static boolean Lemontwist_HandleEvent( ILemontwist *pi,
    AEEEvent eCode,
    uint16  wParam,
    uint32 dwParam
)
{
    Lemontwist *pMe = (Lemontwist*)pi;
    AEEAppStart* as = NULL;
    AEEDeviceInfo di; 
    
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
            pMe->m_currState  = LEMONTWIST_MAIN;
            pMe->m_eDlgReturn = DLGRET_CREATE;
            pMe->m_eAppStatus = LEMONTWIST_RUNNING;
            if(pMe->m_pIAnn != NULL)
            {
                IANNUNCIATOR_SetFieldIsActiveEx(pMe->m_pIAnn,FALSE); 
            }
            Lemontwist_RunFSM(pMe);
            return TRUE;

        case EVT_APP_STOP:
            {
                if (pMe->m_pDisplay != NULL)
                {
                    (void) IDISPLAY_Release(pMe->m_pDisplay);
                    pMe->m_pDisplay = NULL;
                }
                pMe->m_eAppStatus = LEMONTWIST_STOP;
                
                return TRUE;
            }

        case EVT_APP_SUSPEND:
          
            pMe->m_eAppStatus = LEMONTWIST_SUSPEND;
            
            return TRUE;
        

        case EVT_APP_RESUME:
            {
                AEEAppStart* as = ( AEEAppStart*)dwParam;
                
                ISHELL_GetDeviceInfo(pMe->m_pShell,&di); 
                pMe->m_rc    = as->rc;
                pMe->m_rc.dy = di.cyScreen;
                pMe->m_eAppStatus = LEMONTWIST_RUNNING;
                Lemontwist_RunFSM(pMe); 
                return TRUE;
            }

        case EVT_DIALOG_INIT:
           
            pMe->m_pActiveIDlg = (IDialog*)dwParam;
            pMe->m_pActivedlgID = wParam;
            
            return Lemontwist_RouteDialogEvt(pMe, eCode, wParam, dwParam);

        case EVT_DIALOG_START:
            
            return Lemontwist_RouteDialogEvt(pMe, eCode, wParam, dwParam);

        case EVT_USER_REDRAW:
            return Lemontwist_RouteDialogEvt(pMe, eCode, wParam, dwParam);

        case EVT_DIALOG_END:
		 
            (void)Lemontwist_RouteDialogEvt(pMe, eCode, wParam, dwParam);
            pMe->m_pActiveIDlg = NULL;
            pMe->m_pActivedlgID = 0;
            if(pMe->m_eAppStatus == LEMONTWIST_RUNNING)
            {
                Lemontwist_RunFSM(pMe);
            }
            return TRUE;
#ifdef FEATURE_LCD_TOUCH_ENABLE
		case EVT_USER:
			{
				if((wParam == AVK_SELECT) || (wParam == AVK_INFO))
				{
					if(dwParam != 0)
					{
						eCode = EVT_COMMAND;
						wParam = dwParam;
						dwParam = 0;
					}
					else
					{
						eCode = EVT_KEY;
					}
				}
				else if(wParam == AVK_CLR)
				{
					eCode = EVT_KEY;
				}
				else if(wParam == AVK_DOWN)//wlh 换桌面图片
				{
					eCode = EVT_KEY;
				}
				else if (wParam == AVK_POUND)
				{
					eCode = EVT_KEY_PRESS;
				}
				return Lemontwist_RouteDialogEvt(pMe,eCode,wParam,dwParam);
			}
#endif

			
        case EVT_PEN_UP:
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
            return Lemontwist_RouteDialogEvt(pMe, eCode, wParam, dwParam);

        case EVT_KEY_PRESS:
        case EVT_KEY_RELEASE:
        case EVT_COMMAND:
            return Lemontwist_RouteDialogEvt(pMe, eCode, wParam, dwParam); 
        default:
            return Lemontwist_RouteDialogEvt(pMe, eCode, wParam, dwParam);
    }
}

/*==============================================================================
函数:
   Lemontwist_ProcessState

说明:
    根据 pMe->m_currState, 调用相应的状态处理函数。

参数:
    pMe [in]: 指向Game Applet对象结构的指针。该结构包含小程序的特定信息。

返回值:
    NFSMACTION_CONTINUE: 指示不停状态机。
    NFSMACTION_WAIT: 指示停止状态机。

备注:

==============================================================================*/
NextFSMAction Lemontwist_ProcessState(Lemontwist *pMe)
{
    NextFSMAction retVal = NFSMACTION_WAIT;
    
    if (NULL == pMe)
    {
        return retVal;
    }
    MSG_FATAL("***zzg Lemontwist_ProcessState=%d***",pMe->m_currState,0,0);
    switch(pMe->m_currState)
    {
        case LEMONTWIST_MAIN:
            return MAINST_LEMONTWIST_Handler(pMe);
        case LEMONTWIST_ZONE:
            return MAINST_LEMONTWIST_ZONE_Handler(pMe);
        case LEMONTWIST_LIVE:
            return MAINST_LEMONTWIST_LIVE_Handler(pMe);   
        case LEMONTWIST_PULL:
            MSG_FATAL("***zzg LEMONTWIST_PULL***", 0, 0, 0);
            return MAINST_LEMONTWIST_PULL_Handler(pMe);
        case LEMONTWIST_EMPLOYMENT:
            return MAINST_LEMONTWIST_EMPLOYMENT_Handler(pMe);
        case LEMONTWIST_VOICE:
            return MAINST_LEMONTWIST_VOICE_Handler(pMe);
        case LEMONTWIST_FUN:
            return MAINST_LEMONTWIST_FUN_Handler(pMe);
        case LEMONTWIST_MULTI:
            return MAINST_LEMONTWIST_MULTI_Handler(pMe);
        case LEMONTWIST_MVDO:
            return MAINST_LEMONTWIST_MVDO_Handler(pMe);
        case LEMONTWIST_HELP:
            return MAINST_LEMONTWIST_HELP_Handler(pMe);    
        case LEMONTWIST_LIVE_MSG:
            return MAINST_LEMONTWIST_LIVE_MSG_Handler(pMe);      
        case LEMONTWIST_EXIT:
            return LEMONTWIST_EXIT_Handler(pMe);  
        default:
            break;

    }
    return retVal;
}

/*==============================================================================
函数:
    Lemontwist_MAIN_Handler

说明:
   LEMONTWIST_MAIN 状态处理函数。

参数:
    pMe [in]: 指向LEMONTWIST Applet对象结构的指针。该结构包含小程序的特定信息。

返回值:
    NFSMACTION_CONTINUE: 指示不停状态机。
    NFSMACTION_WAIT: 指示停止状态机。

备注:

==============================================================================*/
static NextFSMAction MAINST_LEMONTWIST_Handler(Lemontwist *pMe)
{
	boolean locksel;
    if (NULL == pMe)
    {
        return NFSMACTION_WAIT;
    }

    MSG_FATAL("***zzg MAINST_LEMONTWIST_Handler m_eDlgReturn=%d***", pMe->m_eDlgReturn, 0, 0);
	
	switch (pMe->m_eDlgReturn)
	{
    	// 进入主界面
    	case DLGRET_CREATE:
        	Lemontwist_ShowDialog(pMe, IDD_LEMONTWIST_DIALOGS);
        	return NFSMACTION_WAIT;
        case DLGRET_ZONE:
            MOVE_TO_STATE(LEMONTWIST_ZONE)
        	return NFSMACTION_CONTINUE;
        case DLGRET_LIVE:
            MOVE_TO_STATE(LEMONTWIST_LIVE)
        	return NFSMACTION_CONTINUE;       
        case DLGRET_PULL:
            MSG_FATAL("***zzg DLGRET_PULL***", 0, 0, 0);
            MOVE_TO_STATE(LEMONTWIST_PULL)
        	return NFSMACTION_CONTINUE;  
        case DLGRET_EMPLOYMENT:
            MOVE_TO_STATE(LEMONTWIST_EMPLOYMENT)
        	return NFSMACTION_CONTINUE;  
        case DLGRET_VOICE:
            MOVE_TO_STATE(LEMONTWIST_VOICE)
        	return NFSMACTION_CONTINUE;  
        case DLGRET_FUN:
            MOVE_TO_STATE(LEMONTWIST_FUN)
        	return NFSMACTION_CONTINUE;  
        case DLGRET_MULTI:
            MOVE_TO_STATE(LEMONTWIST_MULTI)
        	return NFSMACTION_CONTINUE;  
        case DLGRET_MVDO:
            MOVE_TO_STATE(LEMONTWIST_MVDO)
        	return NFSMACTION_CONTINUE;  
        case DLGRET_LIVE_MSG:
            MOVE_TO_STATE(LEMONTWIST_LIVE_MSG)
        	return NFSMACTION_CONTINUE;  
        case DLGRET_HELP:
            MOVE_TO_STATE(LEMONTWIST_HELP)
        	return NFSMACTION_CONTINUE;     
    	default:
        	MOVE_TO_STATE(LEMONTWIST_EXIT)
        	return NFSMACTION_CONTINUE;
	}
	
}

static NextFSMAction MAINST_LEMONTWIST_ZONE_Handler(Lemontwist *pMe)
{
	boolean locksel;
    if (NULL == pMe)
    {
        return NFSMACTION_WAIT;
    }
	{
    	switch (pMe->m_eDlgReturn)
    	{
        	// 进入主界面
        	case DLGRET_CREATE:
            	Lemontwist_ShowDialog(pMe, IDD_LEMONTWIST_ZONE);
            	return NFSMACTION_WAIT;
            
        	default:
            	MOVE_TO_STATE(LEMONTWIST_MAIN)
            	return NFSMACTION_CONTINUE;
    	}
	}
}

static NextFSMAction MAINST_LEMONTWIST_LIVE_Handler(Lemontwist *pMe)
{
	boolean locksel;
    if (NULL == pMe)
    {
        return NFSMACTION_WAIT;
    }
	{
    	switch (pMe->m_eDlgReturn)
    	{
        	// 进入主界面
        	case DLGRET_CREATE:
            	Lemontwist_ShowDialog(pMe, IDD_LEMONTWIST_LIVE);
            	return NFSMACTION_WAIT;
            case DLGRET_PULL:
                MOVE_TO_STATE(LEMONTWIST_PULL)
                return NFSMACTION_CONTINUE;
            case DLGRET_EMPLOYMENT:
                MOVE_TO_STATE(LEMONTWIST_EMPLOYMENT)
                return NFSMACTION_CONTINUE;
            case DLGRET_VOICE:
                MOVE_TO_STATE(LEMONTWIST_VOICE)
                return NFSMACTION_CONTINUE;
            case DLGRET_FUN:
                MOVE_TO_STATE(LEMONTWIST_FUN)
                return NFSMACTION_CONTINUE;   
            case DLGRET_MULTI:
                MOVE_TO_STATE(LEMONTWIST_MULTI)
                return NFSMACTION_CONTINUE;
            case DLGRET_MVDO:
                MOVE_TO_STATE(LEMONTWIST_MVDO)
                return NFSMACTION_CONTINUE;
            case DLGRET_HELP:
                MOVE_TO_STATE(LEMONTWIST_HELP)
                return NFSMACTION_CONTINUE;               
        	default:
            	MOVE_TO_STATE(LEMONTWIST_MAIN)
            	return NFSMACTION_CONTINUE;
    	}
	}
}

static NextFSMAction MAINST_LEMONTWIST_PULL_Handler(Lemontwist *pMe)
{
    if (NULL == pMe)
    {
        return NFSMACTION_WAIT;
    }

    MSG_FATAL("***zzg MAINST_LEMONTWIST_PULL_Handler***", 0, 0, 0);
	
	switch (pMe->m_eDlgReturn)
	{
    	// 进入主界面
    	case DLGRET_CREATE:
        	Lemontwist_ShowDialog(pMe, IDD_LEMONTWIST_PULL);
        	return NFSMACTION_WAIT;
        case DLGRET_LIVE_MSG:
            MOVE_TO_STATE(LEMONTWIST_LIVE_MSG)
            return NFSMACTION_CONTINUE; 
    	default:
        	MOVE_TO_STATE(LEMONTWIST_LIVE)
        	return NFSMACTION_CONTINUE;
	}	
}

static NextFSMAction MAINST_LEMONTWIST_EMPLOYMENT_Handler(Lemontwist *pMe)
{
	boolean locksel;
    if (NULL == pMe)
    {
        return NFSMACTION_WAIT;
    }
	{
    	switch (pMe->m_eDlgReturn)
    	{
        	// 进入主界面
        	case DLGRET_CREATE:
            	Lemontwist_ShowDialog(pMe, IDD_LEMONTWIST_EMPLOYMENT);
            	return NFSMACTION_WAIT;
            case DLGRET_LIVE_MSG:
                MOVE_TO_STATE(LEMONTWIST_LIVE_MSG)
                return NFSMACTION_CONTINUE;
        	default:
            	MOVE_TO_STATE(LEMONTWIST_LIVE)
            	return NFSMACTION_CONTINUE;
    	}
	}
}

static NextFSMAction MAINST_LEMONTWIST_VOICE_Handler(Lemontwist *pMe)
{
	boolean locksel;
    if (NULL == pMe)
    {
        return NFSMACTION_WAIT;
    }
	{
    	switch (pMe->m_eDlgReturn)
    	{
        	// 进入主界面
        	case DLGRET_CREATE:
            	Lemontwist_ShowDialog(pMe, IDD_LEMONTWIST_VOICE);
            	return NFSMACTION_WAIT;
            case DLGRET_LIVE_MSG:
                MOVE_TO_STATE(LEMONTWIST_LIVE_MSG)
                return NFSMACTION_CONTINUE;
        	default:
            	MOVE_TO_STATE(LEMONTWIST_LIVE)
            	return NFSMACTION_CONTINUE;
    	}
	}
}

static NextFSMAction MAINST_LEMONTWIST_FUN_Handler(Lemontwist *pMe)
{
	boolean locksel;
    if (NULL == pMe)
    {
        return NFSMACTION_WAIT;
    }
	{
    	switch (pMe->m_eDlgReturn)
    	{
        	// 进入主界面
        	case DLGRET_CREATE:
            	Lemontwist_ShowDialog(pMe, IDD_LEMONTWIST_FUN);
            	return NFSMACTION_WAIT;
            case DLGRET_LIVE_MSG:
                MOVE_TO_STATE(LEMONTWIST_LIVE_MSG)
                return NFSMACTION_CONTINUE;
        	default:
            	MOVE_TO_STATE(LEMONTWIST_LIVE)
            	return NFSMACTION_CONTINUE;
    	}
	}
}

static NextFSMAction MAINST_LEMONTWIST_MULTI_Handler(Lemontwist *pMe)
{
	boolean locksel;
    if (NULL == pMe)
    {
        return NFSMACTION_WAIT;
    }
	{
    	switch (pMe->m_eDlgReturn)
    	{
        	// 进入主界面
        	case DLGRET_CREATE:
            	Lemontwist_ShowDialog(pMe, IDD_LEMONTWIST_MULTI);
            	return NFSMACTION_WAIT;
            case DLGRET_LIVE_MSG:
                MOVE_TO_STATE(LEMONTWIST_LIVE_MSG)
                return NFSMACTION_CONTINUE;
        	default:
            	MOVE_TO_STATE(LEMONTWIST_LIVE)
            	return NFSMACTION_CONTINUE;
    	}
	}
}

static NextFSMAction MAINST_LEMONTWIST_MVDO_Handler(Lemontwist *pMe)
{
	boolean locksel;
    if (NULL == pMe)
    {
        return NFSMACTION_WAIT;
    }
	{
    	switch (pMe->m_eDlgReturn)
    	{
        	// 进入主界面
        	case DLGRET_CREATE:
            	Lemontwist_ShowDialog(pMe, IDD_LEMONTWIST_MVDO);
            	return NFSMACTION_WAIT;
            case DLGRET_LIVE_MSG:
                MOVE_TO_STATE(LEMONTWIST_LIVE_MSG)
                return NFSMACTION_CONTINUE;
        	default:
            	MOVE_TO_STATE(LEMONTWIST_LIVE)
            	return NFSMACTION_CONTINUE;
    	}
	}
}

static NextFSMAction MAINST_LEMONTWIST_HELP_Handler(Lemontwist *pMe)
{
	boolean locksel;
    if (NULL == pMe)
    {
        return NFSMACTION_WAIT;
    }
	{
    	switch (pMe->m_eDlgReturn)
    	{
        	// 进入主界面
        	case DLGRET_CREATE:
            	Lemontwist_ShowDialog(pMe, IDD_LEMONTWIST_HELP);
            	return NFSMACTION_WAIT;
            case DLGRET_LIVE_MSG:
                MOVE_TO_STATE(LEMONTWIST_LIVE_MSG)
                return NFSMACTION_CONTINUE;
        	default:
            	MOVE_TO_STATE(LEMONTWIST_LIVE)
            	return NFSMACTION_CONTINUE;
    	}
	}
}


static NextFSMAction MAINST_LEMONTWIST_LIVE_MSG_Handler(Lemontwist *pMe)
{
	boolean locksel;
    if (NULL == pMe)
    {
        return NFSMACTION_WAIT;
    }
	{
    	switch (pMe->m_eDlgReturn)
    	{
        	// 进入主界面
        	case DLGRET_CREATE:
            	Lemontwist_ShowDialog(pMe, IDD_LEMONTWIST_LIVE_MSG);
            	return NFSMACTION_WAIT;
            
        	default:
            	MOVE_TO_STATE(LEMONTWIST_LIVE)
            	return NFSMACTION_CONTINUE;
    	}
	}
}



/*==============================================================================
函数:
    Lemontwist_EXIT_Handler

说明:
    Lemontwist_EXIT 状态处理函数。

参数:
    pMe [in]: 指向Lemontwist Applet对象结构的指针。该结构包含小程序的特定信息。

返回值:
    NFSMACTION_CONTINUE: 指示不停状态机。
    NFSMACTION_WAIT: 指示停止状态机。

备注:

==============================================================================*/
static NextFSMAction LEMONTWIST_EXIT_Handler(Lemontwist * pMe)
{
    (void) ISHELL_CloseApplet(pMe->m_pShell, FALSE);
    return NFSMACTION_WAIT;
} 
/*
       Lemontwist_ShowDialog

说明:
    函数由状态处理函数用来唤起相应对话框资源ID的对话框。

参数:
    pMe [in]: 指向 MAINMENU Applet 对象结构的指针。该结构包含小程序的特定信息。
    dlgResId [in]: 对话框资源ID，函数根据该ID创建对话框。

返回值:
    none

备注:

==============================================================================*/
void Lemontwist_ShowDialog(Lemontwist  *pMe,  uint16 dlgResId)
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
       (void)IDISPLAY_SetPrefs(pMe->m_pDisplay, "a:1", STRLEN("a:1"));
        ISHELL_GetDeviceInfo(pMe->m_pShell, &di);
        pMe->m_rc.dx = di.cxScreen;
        pMe->m_rc.dy = di.cyScreen;
        IDISPLAY_SetClipRect(pMe->m_pDisplay, &pMe->m_rc);
    }
    
    nRet = ISHELL_CreateDialog(pMe->m_pShell,LEMONTWIST_RES_FILE_LANG,dlgResId,NULL);
    if (nRet != SUCCESS)
    {
    }
    
}


/*=============================================================================
FUNCTION: Lemontwist_ListMenuHandler

DESCRIPTION:   列表菜单

PARAMETERS:

=============================================================================*/

static boolean Lemontwist_ListMenuHandler(Lemontwist *pMe, AEEEvent eCode, uint16 wParam, uint32 dwParam)
{
    PARAM_NOT_REF(dwParam)
    IMenuCtl *pMenu = (IMenuCtl*)IDIALOG_GetControl(pMe->m_pActiveIDlg,IDC_LEMONTWIST);
        
    if (pMenu == NULL)
    {
        return FALSE;
    }

    switch (eCode)
    {
        case EVT_DIALOG_INIT:
			#if 0
            IMENUCTL_SetTitle(pMenu, LEMONTWIST_RES_FILE_LANG, IDS_LEMONTWIST_TITLE, NULL);
			#else
		    {
		  		AECHAR WTitle[40] = {0};
				(void)ISHELL_LoadResString(pMe->m_pShell,
                        LEMONTWIST_RES_FILE_LANG,                                
                        IDS_LEMONTWIST_TITLE,
                        WTitle,
                        sizeof(WTitle));
                if(pMe->m_pIAnn != NULL)
                {
				    IANNUNCIATOR_SetFieldTextEx(pMe->m_pIAnn,WTitle,FALSE);
                }
		    }
			#endif
            IANNUNCIATOR_Redraw(pMe->m_pIAnn); 
			
            IMENUCTL_AddItem(pMenu, LEMONTWIST_RES_FILE_LANG,IDS_LEMON_LIVE, IDS_LEMON_LIVE, NULL, 0);
            IMENUCTL_AddItem(pMenu, LEMONTWIST_RES_FILE_LANG,IDS_LEMON_ZONE, IDS_LEMON_ZONE, NULL, 0);
            IMENUCTL_AddItem(pMenu, LEMONTWIST_RES_FILE_LANG,IDS_INTERNET, IDS_INTERNET, NULL, 0);

            return TRUE;
            
        case EVT_DIALOG_START:
            {  
				IMENUCTL_SetProperties(pMenu, MP_UNDERLINE_TITLE|MP_WRAPSCROLL|MP_BIND_ITEM_TO_NUMBER_KEY|MP_ACTIVE_NO_REDRAW);
				
                IMENUCTL_SetOemProperties( pMenu, OEMMP_USE_MENU_STYLE);
                IMENUCTL_SetBottomBarType(pMenu,BTBAR_SELECT_BACK);
                IMENUCTL_SetSel(pMenu, pMe->m_MainSel);
            }
            return TRUE;
            
        case EVT_DIALOG_END:
            return TRUE;

        case EVT_KEY:
            switch(wParam)
            {
                case AVK_3:                
                    StartApplet(pMe, IDS_INTERNET);
                    return TRUE;
                    
                case AVK_CLR:
                    CLOSE_DIALOG(DLGRET_CANCELED)
                    return TRUE;
                    
                default:
                    break;
          
            }
            return TRUE;
            
        case EVT_COMMAND:         
            pMe->m_MainSel = IMENUCTL_GetSel(pMenu);
            switch (wParam)
            {
                case IDS_LEMON_ZONE:       //声音设置
                {
                    CLOSE_DIALOG(DLGRET_ZONE)
                    return TRUE;
                }
                case IDS_LEMON_LIVE:
                {
                    CLOSE_DIALOG(DLGRET_LIVE)
                    return TRUE;
                }
                case IDS_INTERNET:                
                    StartApplet(pMe, IDS_INTERNET);
                    return TRUE;
                default:
                    return TRUE;
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
						boolean rt =  ISHELL_PostEvent(pMe->m_pShell,AEECLSID_LEMON_TWIST,EVT_KEY,AVK_SELECT,0);
						return rt;
					}
					else if(wXPos >= rc.x + (rc.dx/3)   && wXPos < rc.x + (rc.dx/3)*2 )//左
					{
						 boolean rt = ISHELL_PostEvent(pMe->m_pShell,AEECLSID_LEMON_TWIST,EVT_KEY,AVK_INFO,0);
						 return rt;
					}
					else if(wXPos >= rc.x + (rc.dx/3)*2 && wXPos < rc.x + (rc.dx/3)*3 )//左
					{						
						 boolean rt = ISHELL_PostEvent(pMe->m_pShell,AEECLSID_LEMON_TWIST,EVT_KEY,AVK_CLR,0);
						 return rt;
					}
				}

			}
			break;
#endif
        default:
            break;
    }             
    return FALSE;
}

static boolean Lemontwist_ZoneHandler(Lemontwist * pMe,AEEEvent eCode,uint16 wParam,uint32 dwParam)
{  
    IMenuCtl *pMenu = (IMenuCtl*)IDIALOG_GetControl(pMe->m_pActiveIDlg,IDC_LEMONTWIST_ZONE);
        
    if (pMenu == NULL)
    {
        return FALSE;
    }

    switch (eCode)
    {
        case EVT_DIALOG_INIT:
			#if 0
            IMENUCTL_SetTitle(pMenu, LEMONTWIST_RES_FILE_LANG, IDS_LEMONTWIST_TITLE, NULL);
			#else
		    {
		  		AECHAR WTitle[40] = {0};
				(void)ISHELL_LoadResString(pMe->m_pShell,
                        LEMONTWIST_RES_FILE_LANG,                                
                        IDS_LEMON_ZONE,
                        WTitle,
                        sizeof(WTitle));
                if(pMe->m_pIAnn != NULL)
                {
				    IANNUNCIATOR_SetFieldTextEx(pMe->m_pIAnn,WTitle,FALSE);
                }
		    }
			#endif
            IANNUNCIATOR_Redraw(pMe->m_pIAnn);
            
 
			IMENUCTL_AddItem(pMenu, LEMONTWIST_RES_FILE_LANG,IDS_LEMON_ZONE_LIST_LEMON_ZONE, IDS_LEMON_ZONE_LIST_LEMON_ZONE, NULL, 0);
            IMENUCTL_AddItem(pMenu, LEMONTWIST_RES_FILE_LANG,IDS_LEMON_ZONE_LIST_TWITTER, IDS_LEMON_ZONE_LIST_TWITTER, NULL, 0);
            IMENUCTL_AddItem(pMenu, LEMONTWIST_RES_FILE_LANG,IDS_LEMON_ZONE_LIST_FACEBOOK, IDS_LEMON_ZONE_LIST_FACEBOOK, NULL, 0);
            IMENUCTL_AddItem(pMenu, LEMONTWIST_RES_FILE_LANG,IDS_LEMON_ZONE_LIST_YAHOO_MAIL, IDS_LEMON_ZONE_LIST_YAHOO_MAIL, NULL, 0);
            IMENUCTL_AddItem(pMenu, LEMONTWIST_RES_FILE_LANG,IDS_LEMON_ZONE_LIST_YAHOO_FRONT_PAGE, IDS_LEMON_ZONE_LIST_YAHOO_FRONT_PAGE, NULL, 0);

            return TRUE;
            
        case EVT_DIALOG_START:
            {  
				IMENUCTL_SetProperties(pMenu, MP_UNDERLINE_TITLE|MP_WRAPSCROLL|MP_BIND_ITEM_TO_NUMBER_KEY|MP_ACTIVE_NO_REDRAW);
				
                IMENUCTL_SetOemProperties( pMenu, OEMMP_USE_MENU_STYLE);
                IMENUCTL_SetBottomBarType(pMenu,BTBAR_SELECT_BACK);
                IMENUCTL_SetSel(pMenu, pMe->m_MainSel);
            }
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
                    StartApplet(pMe, IMENUCTL_GetItemID(pMenu, (wParam - AVK_1)));
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

static boolean Lemontwist_LiveHandler(Lemontwist *pMe, AEEEvent eCode, uint16 wParam, uint32 dwParam)
{
    IMenuCtl *pMenu = (IMenuCtl*)IDIALOG_GetControl(pMe->m_pActiveIDlg,IDC_LEMONTWIST_LIVE);
        
    if (pMenu == NULL)
    {
        return FALSE;
    }

    switch (eCode)
    {
        case EVT_DIALOG_INIT:
        {    
	  		AECHAR WTitle[40] = {0};
			(void)ISHELL_LoadResString(pMe->m_pShell,
                    LEMONTWIST_RES_FILE_LANG,                                
                    IDS_LEMON_LIVE,
                    WTitle,
                    sizeof(WTitle));
            if(pMe->m_pIAnn != NULL)
            {
			    IANNUNCIATOR_SetFieldTextEx(pMe->m_pIAnn,WTitle,FALSE);
            }
			
            IANNUNCIATOR_Redraw(pMe->m_pIAnn);      
    
			IMENUCTL_AddItem(pMenu, LEMONTWIST_RES_FILE_LANG,IDS_LEMON_LIVE_LIST_PULL_SERVICES, IDS_LEMON_LIVE_LIST_PULL_SERVICES, NULL, 0);
            IMENUCTL_AddItem(pMenu, LEMONTWIST_RES_FILE_LANG,IDS_LEMON_LIVE_LIST_EMPLOYMENT_NEWS, IDS_LEMON_LIVE_LIST_EMPLOYMENT_NEWS, NULL, 0);
            IMENUCTL_AddItem(pMenu, LEMONTWIST_RES_FILE_LANG,IDS_LEMON_LIVE_LIST_VOICE_CHAT, IDS_LEMON_LIVE_LIST_VOICE_CHAT, NULL, 0);
            IMENUCTL_AddItem(pMenu, LEMONTWIST_RES_FILE_LANG,IDS_LEMON_LIVE_LIST_FUN_LIVE, IDS_LEMON_LIVE_LIST_FUN_LIVE, NULL, 0);
            IMENUCTL_AddItem(pMenu, LEMONTWIST_RES_FILE_LANG,IDS_LEMON_LIVE_LIST_MULTIMEDIA, IDS_LEMON_LIVE_LIST_MULTIMEDIA, NULL, 0);
            IMENUCTL_AddItem(pMenu, LEMONTWIST_RES_FILE_LANG,IDS_LEMON_LIVE_LIST_MVDO, IDS_LEMON_LIVE_LIST_MVDO, NULL, 0);
            IMENUCTL_AddItem(pMenu, LEMONTWIST_RES_FILE_LANG,IDS_LEMON_LIVE_LIST_HELP, IDS_LEMON_LIVE_LIST_HELP, NULL, 0);            

            return TRUE;
        }    
        case EVT_DIALOG_START:
            {  
                IMENUCTL_SetProperties(pMenu, MP_UNDERLINE_TITLE|MP_WRAPSCROLL|MP_BIND_ITEM_TO_NUMBER_KEY|MP_ACTIVE_NO_REDRAW);
				
                IMENUCTL_SetOemProperties( pMenu, OEMMP_USE_MENU_STYLE);
                IMENUCTL_SetBottomBarType(pMenu,BTBAR_SELECT_BACK);
                IMENUCTL_SetSel(pMenu, pMe->m_MainSel);
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
            
        case EVT_COMMAND:
            pMe->m_MainSel = IMENUCTL_GetSel(pMenu);
            switch (wParam)
            {    
                case IDS_LEMON_LIVE_LIST_PULL_SERVICES:       
                {
                    MSG_FATAL("***zzg IDS_LEMON_LIVE_LIST_PULL_SERVICES***", 0, 0, 0);
                    CLOSE_DIALOG(DLGRET_PULL)
                    return TRUE;
                }
                case IDS_LEMON_LIVE_LIST_EMPLOYMENT_NEWS:
                {
                    CLOSE_DIALOG(DLGRET_EMPLOYMENT)
                    return TRUE;
                }
                case IDS_LEMON_LIVE_LIST_VOICE_CHAT:                
                {
                    CLOSE_DIALOG(DLGRET_VOICE)
                    return TRUE;
                }
                case IDS_LEMON_LIVE_LIST_FUN_LIVE:
                {
                    CLOSE_DIALOG(DLGRET_FUN)
                    return TRUE;
                }
                case IDS_LEMON_LIVE_LIST_MULTIMEDIA:                
                {
                    CLOSE_DIALOG(DLGRET_MULTI)
                    return TRUE;
                }
                case IDS_LEMON_LIVE_LIST_MVDO:                
                {
                    CLOSE_DIALOG(DLGRET_MVDO)
                    return TRUE;
                }
                case IDS_LEMON_LIVE_LIST_HELP:                
                {
                    CLOSE_DIALOG(DLGRET_HELP)
                    return TRUE;
                }
                default:
                    return TRUE;
            }
            return TRUE;
        
        default:
            break;
    }             
    return FALSE;
}

static boolean Lemontwist_PullHandler(Lemontwist *pMe, AEEEvent eCode, uint16 wParam, uint32 dwParam)
{
    IMenuCtl *pMenu = (IMenuCtl*)IDIALOG_GetControl(pMe->m_pActiveIDlg,IDC_LEMONTWIST_PULL);
        
    if (pMenu == NULL)
    {
        return FALSE;
    }

    switch (eCode)
    {
        case EVT_DIALOG_INIT:			
        {      
	  		AECHAR WTitle[40] = {0};
			(void)ISHELL_LoadResString(pMe->m_pShell,
                    LEMONTWIST_RES_FILE_LANG,                                
                    IDS_LEMON_LIVE_LIST_PULL_SERVICES,
                    WTitle,
                    sizeof(WTitle));
            if(pMe->m_pIAnn != NULL)
            {
			    IANNUNCIATOR_SetFieldTextEx(pMe->m_pIAnn,WTitle,FALSE);
            }
			
            IANNUNCIATOR_Redraw(pMe->m_pIAnn);      
    
			IMENUCTL_AddItem(pMenu, LEMONTWIST_RES_FILE_LANG,IDS_LEMON_PULL_CRICKET, IDS_LEMON_PULL_CRICKET, NULL, 0);
            IMENUCTL_AddItem(pMenu, LEMONTWIST_RES_FILE_LANG,IDS_LEMON_PULL_BREAKING, IDS_LEMON_PULL_BREAKING, NULL, 0);
            IMENUCTL_AddItem(pMenu, LEMONTWIST_RES_FILE_LANG,IDS_LEMON_PULL_HEALTH, IDS_LEMON_PULL_HEALTH, NULL, 0);
            IMENUCTL_AddItem(pMenu, LEMONTWIST_RES_FILE_LANG,IDS_LEMON_PULL_JOKES, IDS_LEMON_PULL_JOKES, NULL, 0);
            IMENUCTL_AddItem(pMenu, LEMONTWIST_RES_FILE_LANG,IDS_LEMON_PULL_BEAUTY, IDS_LEMON_PULL_BEAUTY, NULL, 0);
            IMENUCTL_AddItem(pMenu, LEMONTWIST_RES_FILE_LANG,IDS_LEMON_PULL_SAI, IDS_LEMON_PULL_SAI, NULL, 0);
            IMENUCTL_AddItem(pMenu, LEMONTWIST_RES_FILE_LANG,IDS_LEMON_PULL_ASTROLOGY, IDS_LEMON_PULL_ASTROLOGY, NULL, 0);
            IMENUCTL_AddItem(pMenu, LEMONTWIST_RES_FILE_LANG,IDS_LEMON_PULL_SHAYARI, IDS_LEMON_PULL_SHAYARI, NULL, 0);
    
            return TRUE;
        }    
        case EVT_DIALOG_START:
            {  
				IMENUCTL_SetProperties(pMenu, MP_UNDERLINE_TITLE|MP_WRAPSCROLL|MP_BIND_ITEM_TO_NUMBER_KEY|MP_ACTIVE_NO_REDRAW);				
                IMENUCTL_SetOemProperties( pMenu, OEMMP_USE_MENU_STYLE);
                IMENUCTL_SetBottomBarType(pMenu,BTBAR_SELECT_BACK);
                IMENUCTL_SetSel(pMenu, pMe->m_MainSel);
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
            
        case EVT_COMMAND:
            
            pMe->m_MainSel = IMENUCTL_GetSel(pMenu);

            MSG_FATAL("***zzg Lemontwist_PullHandler m_MainSel = %d, wParam=%d***", pMe->m_MainSel, wParam, 0);
            
            pMe->m_pActiveitem = wParam;
            
            switch (wParam)
            {    
                case IDS_LEMON_PULL_CRICKET:       
                {
                    CLOSE_DIALOG(DLGRET_LIVE_MSG)
                    return TRUE;
                }   
                case IDS_LEMON_PULL_BREAKING:       
                {
                    CLOSE_DIALOG(DLGRET_LIVE_MSG)
                    return TRUE;
                }
                case IDS_LEMON_PULL_HEALTH:       
                {
                    CLOSE_DIALOG(DLGRET_LIVE_MSG)
                    return TRUE;
                }
                case IDS_LEMON_PULL_JOKES:       
                {
                    CLOSE_DIALOG(DLGRET_LIVE_MSG)
                    return TRUE;
                }
                case IDS_LEMON_PULL_BEAUTY:       
                {
                    CLOSE_DIALOG(DLGRET_LIVE_MSG)
                    return TRUE;
                }
                case IDS_LEMON_PULL_SAI:       
                {
                    CLOSE_DIALOG(DLGRET_LIVE_MSG)
                    return TRUE;
                }
                case IDS_LEMON_PULL_ASTROLOGY:       
                {
                    CLOSE_DIALOG(DLGRET_LIVE_MSG)
                    return TRUE;
                }
                case IDS_LEMON_PULL_SHAYARI:       
                {
                    CLOSE_DIALOG(DLGRET_LIVE_MSG)
                    return TRUE;
                }
                default:
                    return TRUE;
            }
            return TRUE;
        
        default:
            break;
    }             
    return FALSE;
}

static boolean Lemontwist_EmploymentHandler(Lemontwist *pMe, AEEEvent eCode, uint16 wParam, uint32 dwParam)
{
    IMenuCtl *pMenu = (IMenuCtl*)IDIALOG_GetControl(pMe->m_pActiveIDlg,IDC_LEMONTWIST_EMPLOYMENT);
        
    if (pMenu == NULL)
    {
        return FALSE;
    }

    switch (eCode)
    {
        case EVT_DIALOG_INIT:			
        {      
	  		AECHAR WTitle[40] = {0};
			(void)ISHELL_LoadResString(pMe->m_pShell,
                    LEMONTWIST_RES_FILE_LANG,                                
                    IDS_LEMON_LIVE_LIST_EMPLOYMENT_NEWS,
                    WTitle,
                    sizeof(WTitle));
            if(pMe->m_pIAnn != NULL)
            {
			    IANNUNCIATOR_SetFieldTextEx(pMe->m_pIAnn,WTitle,FALSE);
            }
			
            IANNUNCIATOR_Redraw(pMe->m_pIAnn);      
    
			IMENUCTL_AddItem(pMenu, LEMONTWIST_RES_FILE_LANG,IDS_LEMON_EMPLOYMENT, IDS_LEMON_EMPLOYMENT, NULL, 0);
    
            return TRUE;
        }    
        case EVT_DIALOG_START:
            {  
				IMENUCTL_SetProperties(pMenu, MP_UNDERLINE_TITLE|MP_WRAPSCROLL|MP_BIND_ITEM_TO_NUMBER_KEY|MP_ACTIVE_NO_REDRAW);				
                IMENUCTL_SetOemProperties( pMenu, OEMMP_USE_MENU_STYLE);
                IMENUCTL_SetBottomBarType(pMenu,BTBAR_SELECT_BACK);
                IMENUCTL_SetSel(pMenu, pMe->m_MainSel);
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
            
        case EVT_COMMAND:
            pMe->m_MainSel = IMENUCTL_GetSel(pMenu);

            pMe->m_pActiveitem = wParam;
            
            switch (wParam)
            {    
                case IDS_LEMON_EMPLOYMENT:       
                {
                    CLOSE_DIALOG(DLGRET_LIVE_MSG)
                    return TRUE;
                }                
                default:
                    return TRUE;
            }
            return TRUE;
        
        default:
            break;
    }             
    return FALSE;
}

static boolean Lemontwist_VoiceHandler(Lemontwist *pMe, AEEEvent eCode, uint16 wParam, uint32 dwParam)
{
    IMenuCtl *pMenu = (IMenuCtl*)IDIALOG_GetControl(pMe->m_pActiveIDlg,IDC_LEMONTWIST_VOICE);
        
    if (pMenu == NULL)
    {
        return FALSE;
    }

    switch (eCode)
    {
        case EVT_DIALOG_INIT:			
        {      
	  		AECHAR WTitle[40] = {0};
			(void)ISHELL_LoadResString(pMe->m_pShell,
                    LEMONTWIST_RES_FILE_LANG,                                
                    IDS_LEMON_LIVE_LIST_VOICE_CHAT,
                    WTitle,
                    sizeof(WTitle));
            if(pMe->m_pIAnn != NULL)
            {
			    IANNUNCIATOR_SetFieldTextEx(pMe->m_pIAnn,WTitle,FALSE);
            }
			
            IANNUNCIATOR_Redraw(pMe->m_pIAnn);      
    
			IMENUCTL_AddItem(pMenu, LEMONTWIST_RES_FILE_LANG,IDS_LEMON_VOICE_CHAT, IDS_LEMON_VOICE_CHAT, NULL, 0);
    
            return TRUE;
        }
        case EVT_DIALOG_START:
            {  
				IMENUCTL_SetProperties(pMenu, MP_UNDERLINE_TITLE|MP_WRAPSCROLL|MP_BIND_ITEM_TO_NUMBER_KEY|MP_ACTIVE_NO_REDRAW);				
                IMENUCTL_SetOemProperties( pMenu, OEMMP_USE_MENU_STYLE);
                IMENUCTL_SetBottomBarType(pMenu,BTBAR_SELECT_BACK);
                IMENUCTL_SetSel(pMenu, pMe->m_MainSel);
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
            
        case EVT_COMMAND:
            pMe->m_MainSel = IMENUCTL_GetSel(pMenu);

            pMe->m_pActiveitem = wParam;
            
            switch (wParam)
            {    
                case IDS_LEMON_VOICE_CHAT:       
                {
                    CLOSE_DIALOG(DLGRET_LIVE_MSG)
                    return TRUE;
                }                
                default:
                    return TRUE;
            }
            return TRUE;
        
        default:
            break;
    }             
    return FALSE;
}

static boolean Lemontwist_FunHandler(Lemontwist *pMe, AEEEvent eCode, uint16 wParam, uint32 dwParam)
{
    IMenuCtl *pMenu = (IMenuCtl*)IDIALOG_GetControl(pMe->m_pActiveIDlg,IDC_LEMONTWIST_FUN);
        
    if (pMenu == NULL)
    {
        return FALSE;
    }

    switch (eCode)
    {
        case EVT_DIALOG_INIT:			
        {      
	  		AECHAR WTitle[40] = {0};
			(void)ISHELL_LoadResString(pMe->m_pShell,
                    LEMONTWIST_RES_FILE_LANG,                                
                    IDS_LEMON_LIVE_LIST_FUN_LIVE,
                    WTitle,
                    sizeof(WTitle));
            if(pMe->m_pIAnn != NULL)
            {
			    IANNUNCIATOR_SetFieldTextEx(pMe->m_pIAnn,WTitle,FALSE);
            }
			
            IANNUNCIATOR_Redraw(pMe->m_pIAnn);      
    
			IMENUCTL_AddItem(pMenu, LEMONTWIST_RES_FILE_LANG,IDS_LEMON_FUNLIVE_JOKES, IDS_LEMON_FUNLIVE_JOKES, NULL, 0);
            IMENUCTL_AddItem(pMenu, LEMONTWIST_RES_FILE_LANG,IDS_LEMON_FUNLIVE_MEHFIL, IDS_LEMON_FUNLIVE_MEHFIL, NULL, 0);
            IMENUCTL_AddItem(pMenu, LEMONTWIST_RES_FILE_LANG,IDS_LEMON_FUNLIVE_LOVE, IDS_LEMON_FUNLIVE_LOVE, NULL, 0);
            IMENUCTL_AddItem(pMenu, LEMONTWIST_RES_FILE_LANG,IDS_LEMON_FUNLIVE_ART, IDS_LEMON_FUNLIVE_ART, NULL, 0);
            IMENUCTL_AddItem(pMenu, LEMONTWIST_RES_FILE_LANG,IDS_LEMON_FUNLIVE_CRICKET, IDS_LEMON_FUNLIVE_CRICKET, NULL, 0);
            IMENUCTL_AddItem(pMenu, LEMONTWIST_RES_FILE_LANG,IDS_LEMON_FUNLIVE_DAAL, IDS_LEMON_FUNLIVE_DAAL, NULL, 0);
            
            return TRUE;
        }    
        case EVT_DIALOG_START:
            {  
				IMENUCTL_SetProperties(pMenu, MP_UNDERLINE_TITLE|MP_WRAPSCROLL|MP_BIND_ITEM_TO_NUMBER_KEY|MP_ACTIVE_NO_REDRAW);				
                IMENUCTL_SetOemProperties( pMenu, OEMMP_USE_MENU_STYLE);
                IMENUCTL_SetBottomBarType(pMenu,BTBAR_SELECT_BACK);
                IMENUCTL_SetSel(pMenu, pMe->m_MainSel);
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
            
        case EVT_COMMAND:
            pMe->m_MainSel = IMENUCTL_GetSel(pMenu);

            pMe->m_pActiveitem = wParam;
            
            switch (wParam)
            {    
                case IDS_LEMON_FUNLIVE_JOKES:       
                {
                    CLOSE_DIALOG(DLGRET_LIVE_MSG)
                    return TRUE;
                } 
                case IDS_LEMON_FUNLIVE_MEHFIL:       
                {
                    CLOSE_DIALOG(DLGRET_LIVE_MSG)
                    return TRUE;
                } 
                case IDS_LEMON_FUNLIVE_LOVE:       
                {
                    CLOSE_DIALOG(DLGRET_LIVE_MSG)
                    return TRUE;
                } 
                case IDS_LEMON_FUNLIVE_ART:       
                {
                    CLOSE_DIALOG(DLGRET_LIVE_MSG)
                    return TRUE;
                } 
                case IDS_LEMON_FUNLIVE_CRICKET:       
                {
                    CLOSE_DIALOG(DLGRET_LIVE_MSG)
                    return TRUE;
                } 
                case IDS_LEMON_FUNLIVE_DAAL:       
                {
                    CLOSE_DIALOG(DLGRET_LIVE_MSG)
                    return TRUE;
                }                   
                default:
                    return TRUE;
            }
            return TRUE;
        
        default:
            break;
    }             
    return FALSE;
}

static boolean Lemontwist_MultiHandler(Lemontwist *pMe, AEEEvent eCode, uint16 wParam, uint32 dwParam)
{
    IMenuCtl *pMenu = (IMenuCtl*)IDIALOG_GetControl(pMe->m_pActiveIDlg,IDC_LEMONTWIST_MULTI);
        
    if (pMenu == NULL)
    {
        return FALSE;
    }

    switch (eCode)
    {
        case EVT_DIALOG_INIT:			
        {      
	  		AECHAR WTitle[40] = {0};
			(void)ISHELL_LoadResString(pMe->m_pShell,
                    LEMONTWIST_RES_FILE_LANG,                                
                    IDS_LEMON_LIVE_LIST_MULTIMEDIA,
                    WTitle,
                    sizeof(WTitle));
            if(pMe->m_pIAnn != NULL)
            {
			    IANNUNCIATOR_SetFieldTextEx(pMe->m_pIAnn,WTitle,FALSE);
            }
			
            IANNUNCIATOR_Redraw(pMe->m_pIAnn);      
    
			IMENUCTL_AddItem(pMenu, LEMONTWIST_RES_FILE_LANG,IDS_LEMON_MULTI_MODELS, IDS_LEMON_MULTI_MODELS, NULL, 0);
            IMENUCTL_AddItem(pMenu, LEMONTWIST_RES_FILE_LANG,IDS_LEMON_MULTI_FUNNY, IDS_LEMON_MULTI_FUNNY, NULL, 0);
            IMENUCTL_AddItem(pMenu, LEMONTWIST_RES_FILE_LANG,IDS_LEMON_MULTI_FASHION, IDS_LEMON_MULTI_FASHION, NULL, 0);
            IMENUCTL_AddItem(pMenu, LEMONTWIST_RES_FILE_LANG,IDS_LEMON_MULTI_DEVOTIONAL, IDS_LEMON_MULTI_DEVOTIONAL, NULL, 0);
            IMENUCTL_AddItem(pMenu, LEMONTWIST_RES_FILE_LANG,IDS_LEMON_MULTI_AMAZING, IDS_LEMON_MULTI_AMAZING, NULL, 0);
    
            return TRUE;
        }    
        case EVT_DIALOG_START:
            {  
				IMENUCTL_SetProperties(pMenu, MP_UNDERLINE_TITLE|MP_WRAPSCROLL|MP_BIND_ITEM_TO_NUMBER_KEY|MP_ACTIVE_NO_REDRAW);				
                IMENUCTL_SetOemProperties( pMenu, OEMMP_USE_MENU_STYLE);
                IMENUCTL_SetBottomBarType(pMenu,BTBAR_SELECT_BACK);
                IMENUCTL_SetSel(pMenu, pMe->m_MainSel);
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
            
        case EVT_COMMAND:
            pMe->m_MainSel = IMENUCTL_GetSel(pMenu);

            pMe->m_pActiveitem = wParam;
            
            switch (wParam)
            {                 
                case IDS_LEMON_MULTI_MODELS:       
                {
                    CLOSE_DIALOG(DLGRET_LIVE_MSG)
                    return TRUE;
                } 
                case IDS_LEMON_MULTI_FUNNY:       
                {
                    CLOSE_DIALOG(DLGRET_LIVE_MSG)
                    return TRUE;
                }
                case IDS_LEMON_MULTI_FASHION:       
                {
                    CLOSE_DIALOG(DLGRET_LIVE_MSG)
                    return TRUE;
                }
                case IDS_LEMON_MULTI_DEVOTIONAL:       
                {
                    CLOSE_DIALOG(DLGRET_LIVE_MSG)
                    return TRUE;
                }
                case IDS_LEMON_MULTI_AMAZING:       
                {
                    CLOSE_DIALOG(DLGRET_LIVE_MSG)
                    return TRUE;
                }
                default:
                    return TRUE;
            }
            return TRUE;
        
        default:
            break;
    }             
    return FALSE;
}

static boolean Lemontwist_MvdoHandler(Lemontwist *pMe, AEEEvent eCode, uint16 wParam, uint32 dwParam)
{
    IMenuCtl *pMenu = (IMenuCtl*)IDIALOG_GetControl(pMe->m_pActiveIDlg,IDC_LEMONTWIST_MVDO);
        
    if (pMenu == NULL)
    {
        return FALSE;
    }

    switch (eCode)
    {
        case EVT_DIALOG_INIT:			
        {
	  		AECHAR WTitle[40] = {0};
			(void)ISHELL_LoadResString(pMe->m_pShell,
                    LEMONTWIST_RES_FILE_LANG,                                
                    IDS_LEMON_LIVE_LIST_MVDO,
                    WTitle,
                    sizeof(WTitle));
            if(pMe->m_pIAnn != NULL)
            {
			    IANNUNCIATOR_SetFieldTextEx(pMe->m_pIAnn,WTitle,FALSE);
            }
			
            IANNUNCIATOR_Redraw(pMe->m_pIAnn);      
    
			IMENUCTL_AddItem(pMenu, LEMONTWIST_RES_FILE_LANG,IDS_LEMON_MVDO, IDS_LEMON_MVDO, NULL, 0);
    
            return TRUE;
        }
        case EVT_DIALOG_START:
            {  
				IMENUCTL_SetProperties(pMenu, MP_UNDERLINE_TITLE|MP_WRAPSCROLL|MP_BIND_ITEM_TO_NUMBER_KEY|MP_ACTIVE_NO_REDRAW);				
                IMENUCTL_SetOemProperties( pMenu, OEMMP_USE_MENU_STYLE);
                IMENUCTL_SetBottomBarType(pMenu,BTBAR_SELECT_BACK);
                IMENUCTL_SetSel(pMenu, pMe->m_MainSel);
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
            
        case EVT_COMMAND:
            pMe->m_MainSel = IMENUCTL_GetSel(pMenu);

            pMe->m_pActiveitem = wParam;
            
            switch (wParam)
            {  
                case IDS_LEMON_MVDO:       
                {
                    CLOSE_DIALOG(DLGRET_LIVE_MSG)
                    return TRUE;
                }                
                default:
                    return TRUE;
            }
            return TRUE;
        
        default:
            break;
    }             
    return FALSE;
}

static boolean Lemontwist_HelpHandler(Lemontwist *pMe, AEEEvent eCode, uint16 wParam, uint32 dwParam)
{
    IMenuCtl *pMenu = (IMenuCtl*)IDIALOG_GetControl(pMe->m_pActiveIDlg,IDC_LEMONTWIST_HELP);
        
    if (pMenu == NULL)
    {
        return FALSE;
    }

    switch (eCode)
    {
        case EVT_DIALOG_INIT:			
        {      
	  		AECHAR WTitle[40] = {0};
			(void)ISHELL_LoadResString(pMe->m_pShell,
                    LEMONTWIST_RES_FILE_LANG,                                
                    IDS_LEMON_LIVE_LIST_HELP,
                    WTitle,
                    sizeof(WTitle));
            if(pMe->m_pIAnn != NULL)
            {
			    IANNUNCIATOR_SetFieldTextEx(pMe->m_pIAnn,WTitle,FALSE);
            }
			
            IANNUNCIATOR_Redraw(pMe->m_pIAnn);      
    
			IMENUCTL_AddItem(pMenu, LEMONTWIST_RES_FILE_LANG,IDS_LEMON_LIVE_HELP, IDS_LEMON_LIVE_HELP, NULL, 0);
    
            return TRUE;
        }
        case EVT_DIALOG_START:
            {  
				IMENUCTL_SetProperties(pMenu, MP_UNDERLINE_TITLE|MP_WRAPSCROLL|MP_BIND_ITEM_TO_NUMBER_KEY|MP_ACTIVE_NO_REDRAW);				
                IMENUCTL_SetOemProperties( pMenu, OEMMP_USE_MENU_STYLE);
                IMENUCTL_SetBottomBarType(pMenu,BTBAR_SELECT_BACK);
                IMENUCTL_SetSel(pMenu, pMe->m_MainSel);
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
            
        case EVT_COMMAND:
            pMe->m_MainSel = IMENUCTL_GetSel(pMenu);

            pMe->m_pActiveitem = wParam;
            
            switch (wParam)
            {    
                case IDS_LEMON_LIVE_HELP:       
                {
                    CLOSE_DIALOG(DLGRET_LIVE_MSG)
                    return TRUE;
                }                
                default:
                    return TRUE;
            }
            return TRUE;
        
        default:
            break;
    }             
    return FALSE;
}


static boolean Lemontwist_LiveMsgHandler(Lemontwist *pMe, AEEEvent eCode, uint16 wParam, uint32 dwParam)
{
    static IStatic * pStatic = NULL;

    pStatic = (IStatic *)IDIALOG_GetControl(pMe->m_pActiveIDlg, IDC_LEMONTWIST_LIVE_MSG);

    switch (eCode)
    {
        case EVT_DIALOG_INIT:			
		{
            AEERect  rc;                  
            AEEDeviceInfo DevInfo;

	  		AECHAR WTitle[40] = {0};
			(void)ISHELL_LoadResString(pMe->m_pShell,
                    LEMONTWIST_RES_FILE_LANG,                                
                    IDS_LEMON_LIVE,
                    WTitle,
                    sizeof(WTitle));
            if (pMe->m_pIAnn != NULL)
            {
			    IANNUNCIATOR_SetFieldTextEx(pMe->m_pIAnn,WTitle,FALSE);
            }

            IANNUNCIATOR_Redraw(pMe->m_pIAnn); 

           

           ISHELL_GetDeviceInfo(pMe->m_pShell, &DevInfo);

           rc = pMe->m_rc;  
           rc.dy =  DevInfo.cyScreen - rc.y - GetBottomBarHeight(pMe->m_pDisplay);
                      
           MSG_FATAL("***zzg Lemontwist_LiveMsgHandler rc:x=%d, y=%d***",rc.x,rc.y,0);
           MSG_FATAL("***zzg Lemontwist_LiveMsgHandler rc:dx=%d, dy=%d***",rc.dx,rc.dy,0);
          
           ISTATIC_SetRect(pStatic, &rc);
           ISTATIC_SetProperties(pStatic, ST_CENTERTITLE | ST_UNDERLINE | ST_GRAPHIC_BG);    //ST_NOSCROLL
           ISTATIC_SetBackGround(pStatic, AEE_APPSCOMMONRES_IMAGESFILE, IDB_BACKGROUND);

           return TRUE;
        }     
        case EVT_DIALOG_START:
        {  
            AECHAR * pTemp;   
            AECHAR   ActivateTEXT[1200]; 
            uint16   ActivateTextid;

            MSG_FATAL("***zzg Lemontwist_LiveMsgHandler m_pActiveitem=%d***", pMe->m_pActiveitem, 0, 0);            

            switch(pMe->m_pActiveitem)
            {
                case IDS_LEMON_PULL_CRICKET:
                {
                    ActivateTextid = IDS_LEMON_PULL_CRICKET_INFO;
                    break;
                }
                
                case IDS_LEMON_PULL_BREAKING:
                {
                    ActivateTextid = IDS_LEMON_PULL_BREAKING_INFO;
                    break;
                }    
                
                case IDS_LEMON_PULL_HEALTH:
                {
                    ActivateTextid = IDS_LEMON_PULL_HEALTH_INFO;
                    break;
                }    
                
                case IDS_LEMON_PULL_JOKES:    
                {
                    ActivateTextid = IDS_LEMON_PULL_JOKES_INFO;
                    break;
                }    
                    
                case IDS_LEMON_PULL_BEAUTY:
                {
                    ActivateTextid = IDS_LEMON_PULL_BEAUTY_INFO;
                    break;
                }    
                    
                case IDS_LEMON_PULL_SAI:      
                {
                    ActivateTextid = IDS_LEMON_PULL_SAI_INFO;
                    break;
                }    
                    
                case IDS_LEMON_PULL_ASTROLOGY:
                {
                    ActivateTextid = IDS_LEMON_PULL_ASTROLOGY_INFO;
                    break;
                }    
                    
                case IDS_LEMON_PULL_SHAYARI:  
                {
                    ActivateTextid = IDS_LEMON_PULL_SHAYARI_INFO;
                    break;
                }    
                    
                case IDS_LEMON_EMPLOYMENT:
                {
                    ActivateTextid = IDS_LEMON_EMPLOYMENT_INFO;
                    break;
                }    
                    
                case IDS_LEMON_VOICE_CHAT:
                {
                    ActivateTextid = IDS_LEMON_VOICE_CHAT_INFO;
                    break;
                }    
                    
                case IDS_LEMON_FUNLIVE_JOKES:
                {
                    ActivateTextid = IDS_LEMON_FUNLIVE_JOKES_INFO;
                    break;
                }    
                    
                case IDS_LEMON_FUNLIVE_MEHFIL:
                {
                    ActivateTextid = IDS_LEMON_FUNLIVE_MEHFIL_INFO;
                    break;
                }    
                    
                case IDS_LEMON_FUNLIVE_LOVE:
                {
                    ActivateTextid = IDS_LEMON_FUNLIVE_LOVE_INFO;
                    break;
                }    
                    
                case IDS_LEMON_FUNLIVE_ART:
                {
                    ActivateTextid = IDS_LEMON_FUNLIVE_ART_INFO;
                    break;
                }    
                    
                case IDS_LEMON_FUNLIVE_CRICKET:
                {
                    ActivateTextid = IDS_LEMON_FUNLIVE_CRICKET_INFO;
                    break;
                }    
                    
                case IDS_LEMON_FUNLIVE_DAAL:  
                {
                    ActivateTextid = IDS_LEMON_FUNLIVE_DAAL_INFO;
                    break;
                }    
                    
                case IDS_LEMON_MULTI_MODELS:
                {
                    ActivateTextid = IDS_LEMON_MULTI_MODELS_INFO;
                    break;
                }    
                    
                case IDS_LEMON_MULTI_FUNNY:
                {
                    ActivateTextid = IDS_LEMON_MULTI_FUNNY_INFO;
                    break;
                }    
                    
                case IDS_LEMON_MULTI_FASHION:
                {
                    ActivateTextid = IDS_LEMON_MULTI_FASHION_INFO;
                    break;
                }    
                    
                case IDS_LEMON_MULTI_DEVOTIONAL:
                {
                    ActivateTextid = IDS_LEMON_MULTI_DEVOTIONAL_INFO;
                    break;
                }    
                    
                case IDS_LEMON_MULTI_AMAZING:
                {
                    ActivateTextid = IDS_LEMON_MULTI_AMAZING_INFO;
                    break;
                }    
                    
                case IDS_LEMON_MVDO:
                {
                    ActivateTextid = IDS_LEMON_MVDO_INFO;
                    break;
                }    
                    
                case IDS_LEMON_LIVE_HELP:    
                {
                    ActivateTextid = IDS_LEMON_LIVE_HELP_INFO;
                    break;
                }    
                    
                default:
                    break;                    
            }

            (void)ISHELL_LoadResString(pMe->m_pShell, 
                                      LEMONTWIST_RES_FILE_LANG, 
                                      ActivateTextid, 
                                      ActivateTEXT, 
                                      sizeof(ActivateTEXT));           
                                   
           pTemp = WSTRCHR(ActivateTEXT,(AECHAR)'\\');
           
           while (pTemp)
           {
               *pTemp = (AECHAR)'\n';
               pTemp  = WSTRCHR(ActivateTEXT, (AECHAR)'\\');
           }// replace '\' in the string to '\n'.
          
            (void)ISTATIC_SetText(pStatic, 
                                 NULL, 
                                 ActivateTEXT,
                                 AEE_FONT_NORMAL,
                                 AEE_FONT_NORMAL);  

            ISTATIC_SetActive(pStatic, TRUE);
            ISTATIC_Redraw(pStatic);     

            FREE(pTemp);
            
            (void) ISHELL_PostEvent( pMe->m_pShell,
                                     AEECLSID_LEMON_TWIST,
                                     EVT_USER_REDRAW,
                                     0,
                                     0);
            return TRUE;
        }

        case EVT_USER_REDRAW:
        {
           if (pMe->m_pActiveitem == IDS_LEMON_LIVE_HELP)
           {
                Lemontwist_DrawSoftkey(pMe, BTBAR_BACK);    
           }
           else
           {
                Lemontwist_DrawSoftkey(pMe, BTBAR_ACTIVATE_BACK);    
           } 
           IDISPLAY_Update(pMe->m_pDisplay);
           return TRUE;
        }
        
        case EVT_DIALOG_END:
        {
            return TRUE;
        }

        case EVT_KEY:
            switch(wParam)
            {   
               case AVK_SELECT:
               {
                    MSG_FATAL("****IRUIM_IsCardConnected(pMe->m_pIRUIM)=%d",IRUIM_IsCardConnected(pMe->m_pIRUIM),0,0);

                    if (FALSE==IRUIM_IsCardConnected(pMe->m_pIRUIM))
                    {
                          PromptMsg_Param_type m_PromptMsg;
                          MEMSET(&m_PromptMsg,0,sizeof(PromptMsg_Param_type));
                          m_PromptMsg.nMsgResID= IDS_NOUIM_CARD;   
                          m_PromptMsg.ePMsgType = MESSAGE_WARNNING;
                          STRLCPY(m_PromptMsg.strMsgResFile, LEMONTWIST_RES_FILE_LANG,MAX_FILE_NAME);
                          m_PromptMsg.eBBarType = BTBAR_NONE;
                          DrawPromptMessage(pMe->m_pDisplay,pStatic,&m_PromptMsg);
                          IDISPLAY_UpdateEx(pMe->m_pDisplay,TRUE);
                          (void)ISHELL_SetTimer(pMe->m_pShell,2000, (PFNNOTIFY)Lemontwist_POPTimeOut,pMe);
                          return FALSE;
                    }
                                      
                    {
                        char SMSINFO[MAS_BREWSETINT_STRING] = {0};
                        char SMSNUM[MAS_BREWSETINT_STRING] = {0};    

                        MSG_FATAL("***zzg pMe->m_pActiveitem final = %d***", pMe->m_pActiveitem, 0, 0);

                        switch(pMe->m_pActiveitem)
                        {
                            case IDS_LEMON_PULL_CRICKET:
                            {
                                STRCPY(SMSINFO,"LMPCRI");
                                STRCPY(SMSNUM,"13725575946");   //58558
                                break;
                            }
                            case IDS_LEMON_PULL_BREAKING:
                            {
                                STRCPY(SMSINFO,"LMPNEWS");
                                STRCPY(SMSNUM,"58558");
                                break;
                            }
                            case IDS_LEMON_PULL_HEALTH:
                            {
                                STRCPY(SMSINFO,"LMPHEALTH");
                                STRCPY(SMSNUM,"58558");
                                break;
                            }
                            case IDS_LEMON_PULL_JOKES:
                            {
                                STRCPY(SMSINFO,"LMPNJ");
                                STRCPY(SMSNUM,"58558");
                                break;
                            }
                            case IDS_LEMON_PULL_BEAUTY:
                            {
                                STRCPY(SMSINFO,"LMPBTIP");
                                STRCPY(SMSNUM,"58558");
                                break;
                            }
                            case IDS_LEMON_PULL_SAI:
                            {
                                STRCPY(SMSINFO,"LMPSBAB");
                                STRCPY(SMSNUM,"58558");
                                break;
                            }
                            case IDS_LEMON_PULL_ASTROLOGY:
                            {
                                STRCPY(SMSINFO,"LMPASTR");
                                STRCPY(SMSNUM,"58558");
                                break;
                            }
                            case IDS_LEMON_PULL_SHAYARI:
                            {
                                STRCPY(SMSINFO,"LMPMET");
                                STRCPY(SMSNUM,"58558");
                                break;
                            }
                            case IDS_LEMON_EMPLOYMENT:
                            {
                                STRCPY(SMSINFO,"LMSUBEMPLOYMENTNEWS");
                                STRCPY(SMSNUM,"58558");
                                break;
                            }
                            case IDS_LEMON_VOICE_CHAT:
                            {
                                STRCPY(SMSINFO,"LMSUBVOICECHAT");
                                STRCPY(SMSNUM,"58558");
                                break;
                            }
                            case IDS_LEMON_FUNLIVE_JOKES:
                            {
                                STRCPY(SMSINFO,"LMJOKES");
                                STRCPY(SMSNUM,"58558");
                                break;
                            }
                            case IDS_LEMON_FUNLIVE_MEHFIL:
                            {
                                STRCPY(SMSINFO,"LMSUBMET");
                                STRCPY(SMSNUM,"58558");
                                break;
                            }
                            case IDS_LEMON_FUNLIVE_LOVE:
                            {
                                STRCPY(SMSINFO,"LMSUBLOVEGURU");
                                STRCPY(SMSNUM,"58558");
                                break;
                            }
                            case IDS_LEMON_FUNLIVE_ART:
                            {
                                STRCPY(SMSINFO,"LMSUBARTOFLIVING");
                                STRCPY(SMSNUM,"58558");
                                break;
                            }
                            case IDS_LEMON_FUNLIVE_CRICKET:
                            {
                                STRCPY(SMSINFO,"LMSUBCRICKET");
                                STRCPY(SMSNUM,"58558");
                                break;
                            }
                            case IDS_LEMON_FUNLIVE_DAAL:
                            {
                                STRCPY(SMSINFO,"LMSUBDAALMEINKALA");
                                STRCPY(SMSNUM,"58558");
                                break;
                            }
                            case IDS_LEMON_MULTI_MODELS:
                            {
                                STRCPY(SMSINFO,"LMSUBMODELSMMS");
                                STRCPY(SMSNUM,"57886");
                                break;
                            }
                            case IDS_LEMON_MULTI_FUNNY:
                            {
                                STRCPY(SMSINFO,"LMSUBFUNNYMMS");
                                STRCPY(SMSNUM,"57886");
                                break;
                            }
                            case IDS_LEMON_MULTI_FASHION:
                            {
                                STRCPY(SMSINFO,"LMSUBFASHIONMMS");
                                STRCPY(SMSNUM,"57886");
                                break;
                            }
                            case IDS_LEMON_MULTI_DEVOTIONAL:
                            {
                                STRCPY(SMSINFO,"LMSUBDEVMMS");
                                STRCPY(SMSNUM,"57886");
                                break;
                            }
                            case IDS_LEMON_MULTI_AMAZING:
                            {
                                STRCPY(SMSINFO,"LMSUBFUNNYMMS");
                                STRCPY(SMSNUM,"57886");
                                break;
                            }
                            case IDS_LEMON_MVDO:
                            {
                                STRCPY(SMSINFO,"LMSUBMVDO");
                                STRCPY(SMSNUM,"58558");
                                break;
                            }
                            case IDS_LEMON_LIVE_HELP:
                            {  
                                break;
                            }
                            default:
                                break;                            
                        }                         
                      
                        ICONFIG_SetItem(pMe->m_pConfig, CFGI_LEMON_TWIST_SMSINFO,SMSINFO, sizeof(SMSINFO)); 
                        ICONFIG_SetItem(pMe->m_pConfig, CFGI_LEMON_TWIST_NUM,SMSNUM, sizeof(SMSNUM)); 

                        if (pMe->m_pActiveitem == IDS_LEMON_LIVE_HELP)
                        {
                            CLOSE_DIALOG(DLGRET_LIVE)
                        }
                        else
                        {
                            if(Lemontwist_SendSms(pMe)==SUCCESS)
                            {
                               CLOSE_DIALOG(DLGRET_LIVE)
                            }
                        }
                    }
                return TRUE; 
               }
            case AVK_CLR:
                CLOSE_DIALOG(DLGRET_LIVE)
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

static __inline void Lemontwist_DrawSoftkey(Lemontwist* pMe, BottomBar_e_Type eSoftkeyType)
{
   if(pMe)
   {
      BottomBar_Param_type BarParam={0};
      BarParam.eBBarType = eSoftkeyType;
      DrawBottomBar(pMe->m_pDisplay, &BarParam);
      IDISPLAY_Update(pMe->m_pDisplay);
   }
}


int Lemontwist_SendSms(Lemontwist *pme)
{
    int  result = SUCCESS;
    IWmsApp *pIWmsApp = NULL;
	AECHAR  wstrType[2] = {(AECHAR)LEMONTWIST_MSG, 0};
	
    MSG_FATAL("***zzg START Lemontwist_SendSms***",0,0,0);
    
    if (pme == NULL)
    {
        return EFAILED;
    }
    
    result = ISHELL_CreateInstance(pme->m_pShell,
                                 AEECLSID_WMSAPP,
                                 (void **) &pIWmsApp);
    if ((result == SUCCESS) && (NULL != pIWmsApp))
    {
        result = IWmsApp_SendSpecMessage(pIWmsApp, wstrType);
        IWmsApp_Release(pIWmsApp);
    }
    
    MSG_FATAL("END CoreApp_MobileTracker==%d",result,0,0);
    return result;
   
   
}



static void Lemontwist_POPTimeOut(Lemontwist *pMe)
{
    if (NULL == pMe)
    {
        return;
    }

    CLOSE_DIALOG(DLGRET_LIVE)
}


/*=============================================================================
FUNCTION:  StartApplet

DESCRIPTION:  启动applet

PARAMETERS:  如果APPLET 有变动，只需改动次函数

=============================================================================*/
static int StartApplet(Lemontwist *pMe, int i)
{
    int Result = EUNSUPPORTED;
    switch(i)
    {            
        case IDS_LEMON_ZONE_LIST_LEMON_ZONE:
        {
            AECHAR WBrowserTitle[40] = {0};
    		(void)ISHELL_LoadResString(pMe->m_pShell,
    									LEMONTWIST_RES_FILE_LANG, 							   
    									IDS_INTERNET,
    									WBrowserTitle,
    									sizeof(WBrowserTitle));
    		if(pMe->m_pIAnn != NULL)
    		{
    			IANNUNCIATOR_SetFieldTextEx(pMe->m_pIAnn,WBrowserTitle,FALSE);
    			IANNUNCIATOR_Redraw(pMe->m_pIAnn);
    		}

            Result = SetBrowserArr_Main(pMe->m_pShell,(char*)"http://lemon.momagic.mobi/");
            break;            
        }

        case IDS_LEMON_ZONE_LIST_TWITTER:
        {
            AECHAR WBrowserTitle[40] = {0};
    		(void)ISHELL_LoadResString(pMe->m_pShell,
    									LEMONTWIST_RES_FILE_LANG, 							   
    									IDS_INTERNET,
    									WBrowserTitle,
    									sizeof(WBrowserTitle));
    		if(pMe->m_pIAnn != NULL)
    		{
    			IANNUNCIATOR_SetFieldTextEx(pMe->m_pIAnn,WBrowserTitle,FALSE);
    			IANNUNCIATOR_Redraw(pMe->m_pIAnn);
    		}

            Result = SetBrowserArr_Main(pMe->m_pShell,(char*)"http://m.twitter.com");
            break;             
        }

        case IDS_LEMON_ZONE_LIST_FACEBOOK:
        {
            AECHAR WBrowserTitle[40] = {0};
    		(void)ISHELL_LoadResString(pMe->m_pShell,
    									LEMONTWIST_RES_FILE_LANG, 							   
    									IDS_INTERNET,
    									WBrowserTitle,
    									sizeof(WBrowserTitle));
    		if(pMe->m_pIAnn != NULL)
    		{
    			IANNUNCIATOR_SetFieldTextEx(pMe->m_pIAnn,WBrowserTitle,FALSE);
    			IANNUNCIATOR_Redraw(pMe->m_pIAnn);
    		}

            Result = SetBrowserArr_Main(pMe->m_pShell,(char*)"http://m.facebook.com");
            break;             
        }

        case IDS_LEMON_ZONE_LIST_YAHOO_MAIL:
        {
            AECHAR WBrowserTitle[40] = {0};
    		(void)ISHELL_LoadResString(pMe->m_pShell,
    									LEMONTWIST_RES_FILE_LANG, 							   
    									IDS_INTERNET,
    									WBrowserTitle,
    									sizeof(WBrowserTitle));
    		if(pMe->m_pIAnn != NULL)
    		{
    			IANNUNCIATOR_SetFieldTextEx(pMe->m_pIAnn,WBrowserTitle,FALSE);
    			IANNUNCIATOR_Redraw(pMe->m_pIAnn);
    		}

            Result = SetBrowserArr_Main(pMe->m_pShell,(char*)"http://in.m.yahoo.com/mail?.tsrc=lemon&.sep=launchers");
            break; 
            break;            
        }

        case IDS_LEMON_ZONE_LIST_YAHOO_FRONT_PAGE:
        {   
            AECHAR WBrowserTitle[40] = {0};
    		(void)ISHELL_LoadResString(pMe->m_pShell,
    									LEMONTWIST_RES_FILE_LANG, 							   
    									IDS_INTERNET,
    									WBrowserTitle,
    									sizeof(WBrowserTitle));
    		if(pMe->m_pIAnn != NULL)
    		{
    			IANNUNCIATOR_SetFieldTextEx(pMe->m_pIAnn,WBrowserTitle,FALSE);
    			IANNUNCIATOR_Redraw(pMe->m_pIAnn);
    		}

            Result = SetBrowserArr_Main(pMe->m_pShell,(char*)"http://in.m.yahoo.com/?.tsrc=lemon&.sep=launchers");
            break;          
        }
        
		case IDS_INTERNET:
        {
    		AECHAR WBrowserTitle[40] = {0};
    		(void)ISHELL_LoadResString(pMe->m_pShell,
    									LEMONTWIST_RES_FILE_LANG, 							   
    									IDS_INTERNET,
    									WBrowserTitle,
    									sizeof(WBrowserTitle));
    		if(pMe->m_pIAnn != NULL)
    		{
    			IANNUNCIATOR_SetFieldTextEx(pMe->m_pIAnn,WBrowserTitle,FALSE);
    			IANNUNCIATOR_Redraw(pMe->m_pIAnn);
    		}
          
            Result = SetBrowserArr_Main(pMe->m_pShell,(char*)"http://Google.co.in");
        
            break;
        }
       
		default:
			break;
        
    }
    return Result;
}

