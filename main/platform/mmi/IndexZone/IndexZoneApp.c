/** ----------------------------------------------------------------------------
 *IndexZoneApp.c
 *
 * Copyright(c) 2012  zzg
 * *
 * History:
 *
 * when                 who            what and why
 * ------------- ------------------------------------------
 * 2012_12_15         zzg              create 
 *
 * --------------------------------------------------------
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
#include "ContApp.h"
#include "Appscommon.h"
#include "appscommon.brh"
#include "appscommonimages.brh"

#include "IndexZoneApp.h"
#include "IndexZoneApp_priv.h"

#if !defined( AEE_SIMULATOR)
#include "AEERUIM.h" 
#include "OEMCFGI.h"
#endif //#if !defined( AEE_SIMULATOR)
#include "err.h"
#define PARAM_NOT_REF(x)

/*==============================================================================

                              本地函数声明

==============================================================================*/

static IndexZoneAppMod gIndexZoneAppMod;

static const VTBL( IModule) gModFuncs =
{
    IndexZoneAppMod_AddRef,
    IndexZoneAppMod_Release,
    IndexZoneAppMod_CreateInstance,
    IndexZoneAppMod_FreeResources
};

static uint32  IndexZoneApp_AddRef(IIndexZoneApp *pi);

static uint32  IndexZoneApp_Release (IIndexZoneApp *pi);

static boolean IndexZoneApp_HandleEvent( IIndexZoneApp *pi, AEEEvent eCode, uint16  wParam, uint32 dwParam);

static IndexZoneApp gIndexZoneApp={0};

static const VTBL(IIndexZoneApp) gIndexZoneAppMethods =
{
    IndexZoneApp_AddRef,
    IndexZoneApp_Release,
    IndexZoneApp_HandleEvent
};

int IndexZoneAppMod_Load( IShell *pIShell, void *ph, IModule **ppMod);

int IndexZoneAppMod_New( int16 nSize,
    IShell              *pIShell,
    void                *ph,
    IModule             **ppMod,
    PFNMODCREATEINST    pfnMC,
    PFNFREEMODDATA      pfnMF
);

static int IndexZoneApp_New( IShell *ps, IModule *pIModule, IIndexZoneApp **ppObj);


static int CIndexZoneApp_InitAppData(IndexZoneApp *pMe);

static void CIndexZoneApp_FreeAppData(IndexZoneApp *pMe);

static boolean StartApplet(IndexZoneApp *pMe, int i);


void IndexZoneApp_ShowDialog(IndexZoneApp  *pMe,  uint16 dlgResId);


static boolean  IndexZoneApp_ListMenuHandler(IndexZoneApp *pMe, AEEEvent eCode, uint16 wParam, uint32 dwParam);



/*==============================================================================
                              
                              本地（静态）数据
                              
==============================================================================*/

/*=============================================================================
FUNCTION:  IndexZoneAppMod_Load

DESCRIPTION: 

PARAMETERS:

RETURN VALUE:

COMMENTS:

SIDE EFFECTS:

SEE ALSO:

=============================================================================*/
int IndexZoneAppMod_Load( IShell *pIShell, void *ph, IModule **ppMod)
{
    return IndexZoneAppMod_New( sizeof( IndexZoneAppMod),
                    pIShell,
                    ph,
                    ppMod,
                    NULL,
                    NULL);
}

/*=============================================================================
FUNCTION:  IndexZoneAppMod_New

DESCRIPTION: 

PARAMETERS:

RETURN VALUE:

COMMENTS:

SIDE EFFECTS:

SEE ALSO:

=============================================================================*/
int  IndexZoneAppMod_New( int16 nSize,
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

    MEMSET( &gIndexZoneAppMod, 0, sizeof( IndexZoneApp));

    INIT_VTBL( &gIndexZoneAppMod, IModule, gModFuncs);   
	gIndexZoneAppMod.referenceCounter = 1;
    *ppMod = ( IModule *)&gIndexZoneAppMod;
    return AEE_SUCCESS;
}


/*=============================================================================
FUNCTION:  IndexZoneAppMod_AddRef

DESCRIPTION: 

PARAMETERS:

RETURN VALUE:

COMMENTS:

SIDE EFFECTS:

SEE ALSO:

=============================================================================*/
static uint32 IndexZoneAppMod_AddRef( IModule *po)
{
    return( ++( ( IndexZoneAppMod *)po)->referenceCounter);
}

/*=============================================================================
FUNCTION:  IndexZoneAppMod_Release

DESCRIPTION: 

PARAMETERS:

RETURN VALUE:

COMMENTS:

SIDE EFFECTS:

SEE ALSO:

=============================================================================*/
static uint32 IndexZoneAppMod_Release( IModule *po)
{
    if ( ( ( IndexZoneAppMod *) po)->referenceCounter == 0)
    {
        return 0;
    }

    return( --( ( IndexZoneAppMod *)po)->referenceCounter);
}

/*=============================================================================
FUNCTION:  IndexZoneAppMod_CreateInstance

DESCRIPTION: 

PARAMETERS:

RETURN VALUE:

COMMENTS:

SIDE EFFECTS:

SEE ALSO:

=============================================================================*/
static int  IndexZoneAppMod_CreateInstance( IModule *po,
    IShell *pIShell,
    AEECLSID ClsId,
    void **ppObj
)
{
    *ppObj = NULL;

    if( ClsId != AEECLSID_INDEX_ZONE || IndexZoneApp_New( pIShell, po, ( IIndexZoneApp**)ppObj) != SUCCESS)
    {
        return EFAILED;
    }

    return SUCCESS;
}

/*=============================================================================
FUNCTION:  IndexZoneAppMod_FreeResources

DESCRIPTION: 

PARAMETERS:

RETURN VALUE:

COMMENTS:

SIDE EFFECTS:

SEE ALSO:

=============================================================================*/
static void IndexZoneAppMod_FreeResources( IModule *po, IHeap *ph, IFileMgr *pfm)
{
}

/*=============================================================================
FUNCTION: IndexZoneApp_New

DESCRIPTION: 

PARAMETERS:

RETURN VALUE:

COMMENTS:

SIDE EFFECTS:

SEE ALSO:

=============================================================================*/
static int IndexZoneApp_New( IShell *ps, IModule *pIModule, IIndexZoneApp **ppObj)
{
    int retVal = SUCCESS;
	
    if( 0 == gIndexZoneApp.referenceCounter)
    {
        if( pIModule == NULL || ps == NULL)
        {
            return EFAILED;
        }

        MEMSET(&gIndexZoneApp, 0, sizeof(gIndexZoneApp));
        INIT_VTBL(&gIndexZoneApp, IIndexZoneApp, gIndexZoneAppMethods);

       
        gIndexZoneApp.m_pShell     = ps;
        gIndexZoneApp.m_pModule    = pIModule;
        retVal = CIndexZoneApp_InitAppData(&gIndexZoneApp);
        if(retVal != SUCCESS)
        {
            CIndexZoneApp_FreeAppData((IndexZoneApp*)&gIndexZoneApp);
            return retVal;
        }
        (void) ISHELL_AddRef( ps);
        (void) IMODULE_AddRef( pIModule);
        gIndexZoneApp.referenceCounter  = 0;
        
    }

    ++ gIndexZoneApp.referenceCounter;
    *ppObj = ( IIndexZoneApp*)&gIndexZoneApp;
    
    return retVal;
}

/*==============================================================================
函数:
    CIndexZoneApp_InitAppData

说明:
    初始化 IndexZoneApp Applet 结构数据缺省值。

参数:
    pMe [in]: 指向IndexZoneApp Applet 对象结构的指针。该结构包含小程序的特定信息。

返回值:
    EFAILED: 初始化失败。
    SUCCESS: 初始化成功。

备注:

==============================================================================*/
static int CIndexZoneApp_InitAppData(IndexZoneApp *pMe)
{
    if (NULL == pMe)
    {
        return EFAILED;
    }

    pMe->m_MainSel  = 0;

    if (ISHELL_CreateInstance(pMe->m_pShell, AEECLSID_DISPLAY, (void **) &pMe->m_pDisplay) != SUCCESS)
    {    	
        CIndexZoneApp_FreeAppData(pMe);
        return EFAILED;
    }
	if (AEE_SUCCESS != ISHELL_CreateInstance(pMe->m_pShell,AEECLSID_ANNUNCIATOR,(void **)&pMe->m_pIAnn))
    {    	
        CIndexZoneApp_FreeAppData(pMe);
        return EFAILED;
    }    

    return SUCCESS;
}

/*==============================================================================
函数:
    IndexZoneApp_FreeAppData

说明:
    释放 IndexZoneApp Applet 相关资源。

参数:
    pMe [in]: 指向IndexZoneApp Applet 对象结构的指针。该结构包含小程序的特定信息。

返回值:
    none

备注:

==============================================================================*/
static void CIndexZoneApp_FreeAppData(IndexZoneApp *pMe)
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
}

/*=============================================================================
FUNCTION:  IndexZone_AddRef

DESCRIPTION: 

PARAMETERS:

RETURN VALUE:

COMMENTS:

SIDE EFFECTS:

SEE ALSO:

=============================================================================*/
static uint32  IndexZoneApp_AddRef( IIndexZoneApp *pi)
{
    return ( ++gIndexZoneApp.referenceCounter);
}

/*=============================================================================
FUNCTION:  IndexZoneApp_Release

DESCRIPTION: 

PARAMETERS:

RETURN VALUE:

COMMENTS:

SIDE EFFECTS:

SEE ALSO:

=============================================================================*/
static uint32  IndexZoneApp_Release( IIndexZoneApp *pi)
{
    register IndexZoneApp *pMe = (IndexZoneApp*)pi;

    if (pMe->referenceCounter == 0)
    {
        return 0;
    }
    
    if( --pMe->referenceCounter)
    {
        return pMe->referenceCounter;
    }


    CIndexZoneApp_FreeAppData(pMe);

    (void) ISHELL_Release(pMe->m_pShell);
    (void) IMODULE_Release(pMe->m_pModule);

    return 0;
}

/*=============================================================================
FUNCTION:  IndexZone_HandleEvent

DESCRIPTION: 

PARAMETERS:

RETURN VALUE:

COMMENTS:

SIDE EFFECTS:

SEE ALSO:

=============================================================================*/
static boolean IndexZoneApp_HandleEvent( IIndexZoneApp *pi,
    AEEEvent eCode,
    uint16  wParam,
    uint32 dwParam
)
{
    IndexZoneApp *pMe = (IndexZoneApp*)pi;
    AEEAppStart* as = NULL;
    AEEDeviceInfo di; 

    ISHELL_GetDeviceInfo(pMe->m_pShell,&di); 
    switch ( eCode)
    {
		case EVT_APP_START:
		{
			IndexZoneApp_ShowDialog(pMe, IDD_INDEXZONE_LIST);					
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
            
            return IndexZoneApp_ListMenuHandler(pMe, eCode, wParam, dwParam);

        case EVT_USER:
        case EVT_DIALOG_START:

            return IndexZoneApp_ListMenuHandler(pMe, eCode, wParam, dwParam);
       
        case EVT_USER_REDRAW:
            return IndexZoneApp_ListMenuHandler(pMe, eCode, wParam, dwParam);
			
        case EVT_DIALOG_END:
		  
            (void) IndexZoneApp_ListMenuHandler(pMe, eCode, wParam, dwParam);
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
            return IndexZoneApp_ListMenuHandler(pMe, eCode, wParam, dwParam);

        case EVT_KEY_PRESS:
        case EVT_KEY_RELEASE:
        case EVT_COMMAND:
            return IndexZoneApp_ListMenuHandler(pMe, eCode, wParam, dwParam);
        default:
            return IndexZoneApp_ListMenuHandler(pMe, eCode, wParam, dwParam);
    }
}


/*==============================================================================
函数:
       IndexZoneApp_ShowDialog

说明:
    函数由状态处理函数用来唤起相应对话框资源ID的对话框。

参数:
    pMe [in]: 指向 IndexZoneAppApplet 对象结构的指针。该结构包含小程序的特定信息。
    dlgResId [in]: 对话框资源ID，函数根据该ID创建对话框。

返回值:
    none

备注:

==============================================================================*/
void IndexZoneApp_ShowDialog(IndexZoneApp  *pMe,  uint16 dlgResId)
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
    nRet = ISHELL_CreateDialog(pMe->m_pShell,INDEXZONE_RES_FILE_LANG,dlgResId,NULL);
    if (nRet != SUCCESS)
    {
        MSG_FATAL("Failed to create the dialog %d in the MAINMENU applet.",dlgResId,0,0);
    }
}
/*=============================================================================
FUNCTION:  IndexZoneApp_ListMenuHandler

DESCRIPTION:   列表菜单

PARAMETERS:

=============================================================================*/

static boolean IndexZoneApp_ListMenuHandler(IndexZoneApp *pMe, AEEEvent eCode, uint16 wParam, uint32 dwParam)
{
    PARAM_NOT_REF(dwParam)
    IMenuCtl *pMenu = (IMenuCtl*)IDIALOG_GetControl(pMe->m_pActiveIDlg,IDC_INDEXZONE_LIST);
        
    if (pMenu == NULL)
    {
        return FALSE;
    }

    switch (eCode)
    {
        case EVT_DIALOG_INIT:	
			//IANNUNCIATOR_SetHasTitleText(pMe->m_pIAnn, TRUE);
			IANNUNCIATOR_SetFieldIsActiveEx(pMe->m_pIAnn,FALSE); 
			
            //IMENUCTL_AddItem(pMenu, INDEXZONE_RES_FILE_LANG,IDS_INDEX_ZONE_UTK, IDS_INDEX_ZONE_UTK, NULL, 0);  
			IMENUCTL_AddItem(pMenu, INDEXZONE_RES_FILE_LANG,IDS_INDEX_ZONE_GAMES, IDS_INDEX_ZONE_GAMES, NULL, 0); 
			
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
                          INDEXZONE_RES_FILE_LANG,
                          IDS_INDEX_ZONE_UTK + i - 1,
                          pstr,
                          sizeof(pstr));
                    

                    WSTRLCAT(pwsz,pstr,sizeof(pwsz));
					
                    ERR("IndexZone_ListMenuHandler::%d pwsz::%s",i,pwsz,0);
                    {
                        IMENUCTL_SetItemText(pMenu, IDS_INDEX_ZONE_UTK + i - 1, NULL, NULL, pwsz);
                    }                    
                }                
				
                IMENUCTL_SetProperties(pMenu, MP_UNDERLINE_TITLE|MP_WRAPSCROLL);
                IMENUCTL_SetOemProperties( pMenu, OEMMP_USE_MENU_STYLE);
                IMENUCTL_SetBottomBarType(pMenu,BTBAR_SELECT_BACK);
                IMENUCTL_SetSel(pMenu, pMe->m_MainSel);
                (void) ISHELL_PostEvent(pMe->m_pShell, AEECLSID_INDEX_ZONE, EVT_USER_REDRAW,0,0);
            }
            return TRUE;
            
        case EVT_USER_REDRAW:			
			{				
		  		AECHAR WTitle[40] = {0};
				
				(void)ISHELL_LoadResString(pMe->m_pShell,
                        INDEXZONE_RES_FILE_LANG,                                
                        IDS_INDEX_ZONE_TITLE,
                        WTitle,
                        sizeof(WTitle));
                if(pMe->m_pIAnn != NULL)
                {                								
				    IANNUNCIATOR_SetFieldText(pMe->m_pIAnn,WTitle);
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
				{
					ISHELL_StartApplet(pMe->m_pShell, AEECLSID_APP_UTK);	
					break;
				}
                case AVK_2:
				{
					ISHELL_StartApplet(pMe->m_pShell, AEECLSID_GAME);	
					break;
				}	
                //case AVK_3:               

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
                case IDS_INDEX_ZONE_UTK:
                {
					ISHELL_StartApplet(pMe->m_pShell, AEECLSID_APP_UTK);	
                    return TRUE;
                }
				case IDS_INDEX_ZONE_GAMES:
				{
					ISHELL_StartApplet(pMe->m_pShell, AEECLSID_GAME);	
                    return TRUE;
                }	
				default:
				{
					break;
				}
            }
            return TRUE;       
            
        default:
            break;
    }             
    return FALSE;
}
