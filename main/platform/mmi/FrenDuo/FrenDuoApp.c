/** ----------------------------------------------------------------------------
 *FrenDuoApp.c
 *
 * Copyright(c) 2010  zzg
 * *
 * History:
 *
 * when                 who            what and why
 * ------------- ------------------------------------------
 * 2010_07_19         zzg              create 
 *
 * --------------------------------------------------------
 */


#include "customer.h"
#include "OEMFeatures.h"
#include "OEMClassIds.h"
#ifndef AEECLSID_MAIN_MENU
    #error AEECLSID_MAIN_MENU must be defined
#endif

#ifndef AEE_SIMULATOR
#if defined(FEATURE_WMS_APP) && !defined(FEATURE_WMSAPP_ONLYSUPPORTVMAIL)
#include "WMSApp.h"
#endif
#endif

#include "AEEShell.h"
#include "AEE_OEM.h"
#include "AEEModGen.h"
#include "AEEModTable.h"
#include "AEEMenu.h"
#include "AEEStdLib.h"
#include "AEEAnnunciator.h"
#include "ContApp.h"
#include "Appscommon.h"
#include "appscommon.brh"
#include "appscommonimages.brh"

#include "FrenDuoApp.h"
#include "FrenDuoApp_priv.h"

#if !defined( AEE_SIMULATOR)
#include "AEERUIM.h" 
#include "OEMCFGI.h"
#endif //#if !defined( AEE_SIMULATOR)
#include "err.h"
#define PARAM_NOT_REF(x)

#define FRENDUO_NUM_ONE		L"*550"			
#define FRENDUO_NUM_TWO		L"*55"
/*==============================================================================

                              本地函数声明

==============================================================================*/

static FrenDuoAppMod gFrenDuoAppMod;

static const VTBL( IModule) gModFuncs =
{
    FrenDuoAppMod_AddRef,
    FrenDuoAppMod_Release,
    FrenDuoAppMod_CreateInstance,
    FrenDuoAppMod_FreeResources
};

static uint32  FrenDuoApp_AddRef(IFrenDuoApp *pi);

static uint32  FrenDuoApp_Release (IFrenDuoApp *pi);

static boolean FrenDuoApp_HandleEvent( IFrenDuoApp *pi, AEEEvent eCode, uint16  wParam, uint32 dwParam);

static FrenDuoApp gFrenDuoApp={0};

static const VTBL(IFrenDuoApp) gFrenDuoAppMethods =
{
    FrenDuoApp_AddRef,
    FrenDuoApp_Release,
    FrenDuoApp_HandleEvent
};

int FrenDuoAppMod_Load( IShell *pIShell, void *ph, IModule **ppMod);

int FrenDuoAppMod_New( int16 nSize,
    IShell              *pIShell,
    void                *ph,
    IModule             **ppMod,
    PFNMODCREATEINST    pfnMC,
    PFNFREEMODDATA      pfnMF
);

static int FrenDuoApp_New( IShell *ps, IModule *pIModule, IFrenDuoApp **ppObj);


static int CFrenDuoApp_InitAppData(FrenDuoApp *pMe);

static void CFrenDuoApp_FreeAppData(FrenDuoApp *pMe);

static boolean StartApplet(FrenDuoApp *pMe, int i);


void FrenDuoApp_ShowDialog(FrenDuoApp  *pMe,  uint16 dlgResId);


static boolean  FrenDuoApp_ListMenuHandler(FrenDuoApp *pMe, AEEEvent eCode, uint16 wParam, uint32 dwParam);



/*==============================================================================
                              
                              本地（静态）数据
                              
==============================================================================*/

/*=============================================================================
FUNCTION:  FrenDuoAppMod_Load

DESCRIPTION: 

PARAMETERS:

RETURN VALUE:

COMMENTS:

SIDE EFFECTS:

SEE ALSO:

=============================================================================*/
int FrenDuoAppMod_Load( IShell *pIShell, void *ph, IModule **ppMod)
{
    return FrenDuoAppMod_New( sizeof( FrenDuoAppMod),
                    pIShell,
                    ph,
                    ppMod,
                    NULL,
                    NULL);
}

/*=============================================================================
FUNCTION:  FrenDuoAppMod_New

DESCRIPTION: 

PARAMETERS:

RETURN VALUE:

COMMENTS:

SIDE EFFECTS:

SEE ALSO:

=============================================================================*/
int  FrenDuoAppMod_New( int16 nSize,
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

    MEMSET( &gFrenDuoAppMod, 0, sizeof( FrenDuoApp));

    INIT_VTBL( &gFrenDuoAppMod, IModule, gModFuncs);   
	gFrenDuoAppMod.referenceCounter = 1;
    *ppMod = ( IModule *)&gFrenDuoAppMod;
    return AEE_SUCCESS;
}


/*=============================================================================
FUNCTION:  FrenDuoAppMod_AddRef

DESCRIPTION: 

PARAMETERS:

RETURN VALUE:

COMMENTS:

SIDE EFFECTS:

SEE ALSO:

=============================================================================*/
static uint32 FrenDuoAppMod_AddRef( IModule *po)
{
    return( ++( ( FrenDuoAppMod *)po)->referenceCounter);
}

/*=============================================================================
FUNCTION:  FrenDuoAppMod_Release

DESCRIPTION: 

PARAMETERS:

RETURN VALUE:

COMMENTS:

SIDE EFFECTS:

SEE ALSO:

=============================================================================*/
static uint32 FrenDuoAppMod_Release( IModule *po)
{
    if ( ( ( FrenDuoAppMod *) po)->referenceCounter == 0)
    {
        return 0;
    }

    return( --( ( FrenDuoAppMod *)po)->referenceCounter);
}

/*=============================================================================
FUNCTION:  FrenDuoAppMod_CreateInstance

DESCRIPTION: 

PARAMETERS:

RETURN VALUE:

COMMENTS:

SIDE EFFECTS:

SEE ALSO:

=============================================================================*/
static int  FrenDuoAppMod_CreateInstance( IModule *po,
    IShell *pIShell,
    AEECLSID ClsId,
    void **ppObj
)
{
    *ppObj = NULL;

    if( ClsId != AEECLSID_FRENDUO || FrenDuoApp_New( pIShell, po, ( IFrenDuoApp**)ppObj) != SUCCESS)
    {
        return EFAILED;
    }

    return SUCCESS;
}

/*=============================================================================
FUNCTION:  FrenDuoAppMod_FreeResources

DESCRIPTION: 

PARAMETERS:

RETURN VALUE:

COMMENTS:

SIDE EFFECTS:

SEE ALSO:

=============================================================================*/
static void FrenDuoAppMod_FreeResources( IModule *po, IHeap *ph, IFileMgr *pfm)
{
}

/*=============================================================================
FUNCTION:  FrenDuoApp_New

DESCRIPTION: 

PARAMETERS:

RETURN VALUE:

COMMENTS:

SIDE EFFECTS:

SEE ALSO:

=============================================================================*/
static int FrenDuoApp_New( IShell *ps, IModule *pIModule, IFrenDuoApp **ppObj)
{
    int retVal = SUCCESS;
	
    if( 0 == gFrenDuoApp.referenceCounter)
    {
        if( pIModule == NULL || ps == NULL)
        {
            return EFAILED;
        }

        MEMSET(&gFrenDuoApp, 0, sizeof(gFrenDuoApp));
        INIT_VTBL(&gFrenDuoApp, IFrenDuoApp, gFrenDuoAppMethods);

       
        gFrenDuoApp.m_pShell     = ps;
        gFrenDuoApp.m_pModule    = pIModule;
        retVal = CFrenDuoApp_InitAppData(&gFrenDuoApp);
        if(retVal != SUCCESS)
        {
            CFrenDuoApp_FreeAppData((FrenDuoApp*)&gFrenDuoApp);
            return retVal;
        }
        (void) ISHELL_AddRef( ps);
        (void) IMODULE_AddRef( pIModule);
        gFrenDuoApp.referenceCounter  = 0;
        
    }

    ++ gFrenDuoApp.referenceCounter;
    *ppObj = ( IFrenDuoApp*)&gFrenDuoApp;
    
    return retVal;
}

/*==============================================================================
函数:
    CFrenDuoApp_InitAppData

说明:
    初始化 FrenDuoApp Applet 结构数据缺省值。

参数:
    pMe [in]: 指向FrenDuoApp Applet 对象结构的指针。该结构包含小程序的特定信息。

返回值:
    EFAILED: 初始化失败。
    SUCCESS: 初始化成功。

备注:

==============================================================================*/
static int CFrenDuoApp_InitAppData(FrenDuoApp *pMe)
{
    if (NULL == pMe)
    {
        return EFAILED;
    }

    pMe->m_MainSel  = 0;

    if (ISHELL_CreateInstance(pMe->m_pShell, AEECLSID_DISPLAY, (void **) &pMe->m_pDisplay) != SUCCESS)
    {    	
        CFrenDuoApp_FreeAppData(pMe);
        return EFAILED;
    }
	if (AEE_SUCCESS != ISHELL_CreateInstance(pMe->m_pShell,AEECLSID_ANNUNCIATOR,(void **)&pMe->m_pIAnn))
    {    	
        CFrenDuoApp_FreeAppData(pMe);
        return EFAILED;
    }

    if (AEE_SUCCESS != ISHELL_CreateInstance(pMe->m_pShell, AEECLSID_RUIM, (void **)&pMe->m_pIRUIM))
    {
       CFrenDuoApp_FreeAppData(pMe);
       return EFAILED;
    }

    return SUCCESS;
}

/*==============================================================================
函数:
    FrenDuoApp_FreeAppData

说明:
    释放 FrenDuoApp Applet 相关资源。

参数:
    pMe [in]: 指向FrenDuoApp Applet 对象结构的指针。该结构包含小程序的特定信息。

返回值:
    none

备注:

==============================================================================*/
static void CFrenDuoApp_FreeAppData(FrenDuoApp *pMe)
{
    if (NULL == pMe)
    {
        return;
    }
    
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

    if (pMe->m_pIRUIM != NULL)
    {
        IRUIM_Release(pMe->m_pIRUIM);
        pMe->m_pIRUIM = NULL;
    }

}

/*=============================================================================
FUNCTION:  FrenDuoApp_AddRef

DESCRIPTION: 

PARAMETERS:

RETURN VALUE:

COMMENTS:

SIDE EFFECTS:

SEE ALSO:

=============================================================================*/
static uint32  FrenDuoApp_AddRef( IFrenDuoApp *pi)
{
    return ( ++gFrenDuoApp.referenceCounter);
}

/*=============================================================================
FUNCTION:  FrenDuoApp_Release

DESCRIPTION: 

PARAMETERS:

RETURN VALUE:

COMMENTS:

SIDE EFFECTS:

SEE ALSO:

=============================================================================*/
static uint32  FrenDuoApp_Release( IFrenDuoApp *pi)
{
    register FrenDuoApp *pMe = (FrenDuoApp*)pi;

    if (pMe->referenceCounter == 0)
    {
        return 0;
    }
    
    if( --pMe->referenceCounter)
    {
        return pMe->referenceCounter;
    }


    CFrenDuoApp_FreeAppData(pMe);

    (void) ISHELL_Release(pMe->m_pShell);
    (void) IMODULE_Release(pMe->m_pModule);

    return 0;
}

/*=============================================================================
FUNCTION:  FrenDuoApp_HandleEvent

DESCRIPTION: 

PARAMETERS:

RETURN VALUE:

COMMENTS:

SIDE EFFECTS:

SEE ALSO:

=============================================================================*/
static boolean FrenDuoApp_HandleEvent( IFrenDuoApp *pi,
    AEEEvent eCode,
    uint16  wParam,
    uint32 dwParam
)
{
    FrenDuoApp *pMe = (FrenDuoApp*)pi;
    AEEAppStart* as = NULL;
    AEEDeviceInfo di; 

    ISHELL_GetDeviceInfo(pMe->m_pShell,&di); 
    switch ( eCode)
    {
		case EVT_APP_START:
		{
			FrenDuoApp_ShowDialog(pMe, IDD_FRENDUO_LIST);					
			return TRUE;
		}
        case EVT_APP_STOP:
            {
                if (pMe->m_pDisplay != NULL)
                {
                    (void) IDISPLAY_Release(pMe->m_pDisplay);
                    pMe->m_pDisplay = NULL;
                }
                
                return TRUE;
            }

        case EVT_APP_RESUME:
            {
                AEEAppStart* as = ( AEEAppStart*)dwParam;
                pMe->m_rc    = as->rc;
                pMe->m_rc.dy = di.cyScreen;             
                
                return TRUE;
            }

        case EVT_DIALOG_INIT:
            pMe->m_pActiveIDlg = (IDialog*)dwParam;
            pMe->m_pActivedlgID = wParam;
            
            return FrenDuoApp_ListMenuHandler(pMe, eCode, wParam, dwParam);

        case EVT_USER:
        case EVT_DIALOG_START:

            return FrenDuoApp_ListMenuHandler(pMe, eCode, wParam, dwParam);
       
        case EVT_USER_REDRAW:
            return FrenDuoApp_ListMenuHandler(pMe, eCode, wParam, dwParam);
			
        case EVT_DIALOG_END:
		  
            (void) FrenDuoApp_ListMenuHandler(pMe, eCode, wParam, dwParam);
            pMe->m_pActiveIDlg = NULL;
            pMe->m_pActivedlgID = 0;            
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
            return FrenDuoApp_ListMenuHandler(pMe, eCode, wParam, dwParam);

        case EVT_KEY_PRESS:
        case EVT_KEY_RELEASE:
        case EVT_COMMAND:
            return FrenDuoApp_ListMenuHandler(pMe, eCode, wParam, dwParam);
        default:
            return FrenDuoApp_ListMenuHandler(pMe, eCode, wParam, dwParam);
    }
}


/*==============================================================================
函数:
       FrenDuoApp_ShowDialog

说明:
    函数由状态处理函数用来唤起相应对话框资源ID的对话框。

参数:
    pMe [in]: 指向 FrenDuoAppApplet 对象结构的指针。该结构包含小程序的特定信息。
    dlgResId [in]: 对话框资源ID，函数根据该ID创建对话框。

返回值:
    none

备注:

==============================================================================*/
void FrenDuoApp_ShowDialog(FrenDuoApp  *pMe,  uint16 dlgResId)
{
    int nRet;

    // 每次最多打开一个对话框
    if (ISHELL_GetActiveDialog(pMe->m_pShell) != NULL)
    {
        // 如果当前已经有对话框被打开，直接返回
        MSG_FATAL("Trying to create dialog %d without closing previous one",dlgResId,0,0);
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
    nRet = ISHELL_CreateDialog(pMe->m_pShell,FRENDUO_RES_FILE_LANG,dlgResId,NULL);
    if (nRet != SUCCESS)
    {
        MSG_FATAL("Failed to create the dialog %d in the MAINMENU applet.",dlgResId,0,0);
    }
}
/*=============================================================================
FUNCTION:  FrenDuoApp_ListMenuHandler

DESCRIPTION:   列表菜单

PARAMETERS:

=============================================================================*/

static boolean FrenDuoApp_ListMenuHandler(FrenDuoApp *pMe, AEEEvent eCode, uint16 wParam, uint32 dwParam)
{
    PARAM_NOT_REF(dwParam)
    IMenuCtl *pMenu = (IMenuCtl*)IDIALOG_GetControl(pMe->m_pActiveIDlg,IDC_FRENDUO_LIST);
        
    if (pMenu == NULL)
    {
        return FALSE;
    }

    switch (eCode)
    {
        case EVT_DIALOG_INIT:	
			//IANNUNCIATOR_SetHasTitleText(pMe->m_pIAnn, TRUE);
			IANNUNCIATOR_SetFieldIsActiveEx(pMe->m_pIAnn,FALSE); 
					
            IMENUCTL_AddItem(pMenu, FRENDUO_RES_FILE_LANG,IDS_FRENDUO_TITLE_1, IDS_FRENDUO_TITLE_1, NULL, 0);
            IMENUCTL_AddItem(pMenu, FRENDUO_RES_FILE_LANG,IDS_FRENDUO_TITLE_2, IDS_FRENDUO_TITLE_2, NULL, 0);
            IMENUCTL_AddItem(pMenu, FRENDUO_RES_FILE_LANG,IDS_FRENDUO_TITLE_3, IDS_FRENDUO_TITLE_3, NULL, 0);
        
            return TRUE;
            
        case EVT_DIALOG_START:
            {  
				int i;
				
				for (i = 1; i <= MAX_MATRIX_ITEMS; i++)
                {
                    AECHAR pwsz[67] = {0};
                    AECHAR pstr[64] = {0};
                    AECHAR wsFmt[5] = {0};
    
                    (void)STRTOWSTR("%d. ",wsFmt,sizeof(wsFmt));
                    WSPRINTF(pwsz,sizeof(pwsz),wsFmt,i);
                    
                    ISHELL_LoadResString( pMe->m_pShell,
                          FRENDUO_RES_FILE_LANG,
                          IDS_FRENDUO_TITLE_1 + i - 1,
                          pstr,
                          sizeof(pstr));
                    

                    WSTRLCAT(pwsz,pstr,sizeof(pwsz));
					
                    ERR("Application_ListMenuHandler::%d pwsz::%s",i,pwsz,0);
                    {
                        IMENUCTL_SetItemText(pMenu, IDS_FRENDUO_TITLE_1 + i - 1, NULL, NULL, pwsz);
                    }                    
                }                
				
                IMENUCTL_SetProperties(pMenu, MP_UNDERLINE_TITLE|MP_WRAPSCROLL);
                IMENUCTL_SetOemProperties( pMenu, OEMMP_USE_MENU_STYLE);
                IMENUCTL_SetBottomBarType(pMenu,BTBAR_SELECT_BACK);
                IMENUCTL_SetSel(pMenu, pMe->m_MainSel);
                (void) ISHELL_PostEvent(pMe->m_pShell, AEECLSID_FRENDUO, EVT_USER_REDRAW,0,0);
            }
            return TRUE;
            
        case EVT_USER_REDRAW:			
			{				
		  		AECHAR WTitle[40] = {0};
				
				(void)ISHELL_LoadResString(pMe->m_pShell,
                        FRENDUO_RES_FILE_LANG,                                
                        IDS_FRENDUO_TITLE,
                        WTitle,
                        sizeof(WTitle));
                if(pMe->m_pIAnn != NULL)
                {                								
				    IANNUNCIATOR_SetFieldTextEx(pMe->m_pIAnn,WTitle,FALSE);
                }
		    }
		
            (void)IMENUCTL_Redraw(pMenu);
            return TRUE;

        case EVT_DIALOG_END:			
			//ISHELL_CloseApplet(pMe->m_pShell, FALSE);
            return TRUE;
		
        case EVT_KEY:
            switch(wParam)
            {
                case AVK_1:
                case AVK_2:
                case AVK_3:
               
                    {
                        int Focus = (wParam - AVK_1);
                        StartApplet(pMe, Focus);
                    }
                    return TRUE;

                case AVK_CLR:
					ISHELL_CloseApplet(pMe->m_pShell, FALSE);                    
                    return TRUE;
                    
                default:
                    break;
          
            }
            return TRUE;
            
        case EVT_COMMAND:
            pMe->m_MainSel = wParam;
            switch (wParam)
            {   
                case IDS_FRENDUO_TITLE_1:
                case IDS_FRENDUO_TITLE_2:
                case IDS_FRENDUO_TITLE_3:
                {
					StartApplet(pMe, wParam - IDS_FRENDUO_TITLE_1);
                    return TRUE;
                }
				default:
				{
					break;
				}
            }
            return TRUE;
        case EVT_USER:
        {
            if ((wParam == 1) && (dwParam == 1))
            {
                StartApplet(pMe, 2);
            }
            break;
        }
            
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
static boolean StartApplet(FrenDuoApp *pMe, int i)
{
    int Result = FALSE;

	if (SUCCESS != ISHELL_CreateInstance(pMe->m_pShell, AEECLSID_DIALER, (void **)&pMe->m_pCallApp))
    {
        return EFAILED;
    }

    switch(i)
    {
        case 0:
        {
			//Calling to *550, Then Send SMS To:551, Content SMS: "Status"			
			AECHAR StrCallNum[] = FRENDUO_NUM_ONE;
			
			 // Make a voice call            
            ICallApp_CallNumber(pMe->m_pCallApp, StrCallNum);

            if (pMe->m_pCallApp != NULL) 
            {
                ICallApp_Release(pMe->m_pCallApp);
                pMe->m_pCallApp = NULL;
            }       		
			
            break;
        }
        case 1:
        {
			//Calling to *55, Then Send SMS To:551, Content SMS: "Status"				
			AECHAR StrCallNum[] = FRENDUO_NUM_TWO;

            // Make a voice call            
            ICallApp_CallNumber(pMe->m_pCallApp, StrCallNum);

            if (pMe->m_pCallApp != NULL) 
            {
                ICallApp_Release(pMe->m_pCallApp);
                pMe->m_pCallApp = NULL;
            }      			
            break;            
		}
        case 2:
        {
			//Send SMS To:551, Content SMS: "Status"			
            IWmsApp *pIWmsApp = NULL;

            if (IsRunAsUIMVersion())
            {
                if (IRUIM_IsCardConnected(pMe->m_pIRUIM))
                {
                    if (SUCCESS != ISHELL_CreateInstance(pMe->m_pShell, AEECLSID_WMSAPP, (void**)&pIWmsApp))
                    {
                    	return EFAILED;
                    }

                    if (SUCCESS == IWmsApp_SendSpecMessageEx(pIWmsApp))
                    {
                    	MSG_FATAL("FrenDuoApp: IWmsApp_SendSpecMessageEx  SUCCESS!",0,0,0);
                    }
                    else
                    {
                    	MSG_FATAL("FrenDuoApp: IWmsApp_SendSpecMessageEx  FAILED!",0,0,0);
                    	(void)IWmsApp_Release(pIWmsApp);
                    	pIWmsApp = NULL;
                    	return EFAILED;
                    }

                    (void)IWmsApp_Release(pIWmsApp);
                    pIWmsApp = NULL;
                    return SUCCESS;  
                }
                else
                {
                    break;
                }
            }    
			
			          	
        }
        default:
        {
			break;
        }
    }
    return TRUE;
}
