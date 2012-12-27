/** ----------------------------------------------------------------------------
 *ServiceApp.c
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

#include "ServiceApp.h"
#include "ServiceApp_priv.h"

#include "OEMNV.h"
#include "OEMSVC.h"
#include "OEMCFGI.h"


#if !defined( AEE_SIMULATOR)
#include "AEERUIM.h" 
#include "OEMCFGI.h"
#endif //#if !defined( AEE_SIMULATOR)
#include "err.h"
#define PARAM_NOT_REF(x)

/*==============================================================================

                              本地函数声明

==============================================================================*/

static ServiceAppMod gServiceAppMod;

static const VTBL( IModule) gModFuncs =
{
    ServiceAppMod_AddRef,
    ServiceAppMod_Release,
    ServiceAppMod_CreateInstance,
    ServiceAppMod_FreeResources
};

static uint32  ServiceApp_AddRef(IServiceApp *pi);

static uint32  ServiceApp_Release (IServiceApp *pi);

static boolean ServiceApp_HandleEvent( IServiceApp *pi, AEEEvent eCode, uint16  wParam, uint32 dwParam);

static ServiceApp gServiceApp={0};

static const VTBL(IServiceApp) gServiceAppMethods =
{
    ServiceApp_AddRef,
    ServiceApp_Release,
    ServiceApp_HandleEvent
};

int ServiceAppMod_Load( IShell *pIShell, void *ph, IModule **ppMod);

int ServiceAppMod_New( int16 nSize,
    IShell              *pIShell,
    void                *ph,
    IModule             **ppMod,
    PFNMODCREATEINST    pfnMC,
    PFNFREEMODDATA      pfnMF
);

static int ServiceApp_New( IShell *ps, IModule *pIModule, IServiceApp **ppObj);


static int CServiceApp_InitAppData(ServiceApp *pMe);

static void CServiceApp_FreeAppData(ServiceApp *pMe);

static boolean StartApplet(ServiceApp *pMe, int i);


void ServiceApp_ShowDialog(ServiceApp  *pMe,  uint16 dlgResId);


static boolean  ServiceApp_ListMenuHandler(ServiceApp *pMe, AEEEvent eCode, uint16 wParam, uint32 dwParam);

static int SetBrowserArr_MainEx(ServiceApp *pMe, char *purl);

/*==============================================================================
                              
                              本地（静态）数据
                              
==============================================================================*/

/*=============================================================================
FUNCTION:  ServiceMod_Load

DESCRIPTION: 

PARAMETERS:

RETURN VALUE:

COMMENTS:

SIDE EFFECTS:

SEE ALSO:

=============================================================================*/
int ServiceAppMod_Load( IShell *pIShell, void *ph, IModule **ppMod)
{
    return ServiceAppMod_New( sizeof( ServiceAppMod),
                    pIShell,
                    ph,
                    ppMod,
                    NULL,
                    NULL);
}

/*=============================================================================
FUNCTION:  ServiceAppMod_New

DESCRIPTION: 

PARAMETERS:

RETURN VALUE:

COMMENTS:

SIDE EFFECTS:

SEE ALSO:

=============================================================================*/
int  ServiceAppMod_New( int16 nSize,
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

    MEMSET( &gServiceAppMod, 0, sizeof( ServiceApp));

    INIT_VTBL( &gServiceAppMod, IModule, gModFuncs);   
	gServiceAppMod.referenceCounter = 1;
    *ppMod = ( IModule *)&gServiceAppMod;
    return AEE_SUCCESS;
}


/*=============================================================================
FUNCTION:  ServiceAppMod_AddRef

DESCRIPTION: 

PARAMETERS:

RETURN VALUE:

COMMENTS:

SIDE EFFECTS:

SEE ALSO:

=============================================================================*/
static uint32 ServiceAppMod_AddRef( IModule *po)
{
    return( ++( ( ServiceAppMod *)po)->referenceCounter);
}

/*=============================================================================
FUNCTION:  ServiceAppMod_Release

DESCRIPTION: 

PARAMETERS:

RETURN VALUE:

COMMENTS:

SIDE EFFECTS:

SEE ALSO:

=============================================================================*/
static uint32 ServiceAppMod_Release( IModule *po)
{
    if ( ( ( ServiceAppMod *) po)->referenceCounter == 0)
    {
        return 0;
    }

    return( --( ( ServiceAppMod *)po)->referenceCounter);
}

/*=============================================================================
FUNCTION:  ServiceAppMod_CreateInstance

DESCRIPTION: 

PARAMETERS:

RETURN VALUE:

COMMENTS:

SIDE EFFECTS:

SEE ALSO:

=============================================================================*/
static int  ServiceAppMod_CreateInstance( IModule *po,
    IShell *pIShell,
    AEECLSID ClsId,
    void **ppObj
)
{
    *ppObj = NULL;

    if( ClsId != AEECLSID_SERVICE || ServiceApp_New( pIShell, po, ( IServiceApp**)ppObj) != SUCCESS)
    {
        return EFAILED;
    }

    return SUCCESS;
}

/*=============================================================================
FUNCTION: ServiceAppMod_FreeResources

DESCRIPTION: 

PARAMETERS:

RETURN VALUE:

COMMENTS:

SIDE EFFECTS:

SEE ALSO:

=============================================================================*/
static void ServiceAppMod_FreeResources( IModule *po, IHeap *ph, IFileMgr *pfm)
{
}

/*=============================================================================
FUNCTION: ServiceApp_New

DESCRIPTION: 

PARAMETERS:

RETURN VALUE:

COMMENTS:

SIDE EFFECTS:

SEE ALSO:

=============================================================================*/
static int ServiceApp_New( IShell *ps, IModule *pIModule, IServiceApp **ppObj)
{
    int retVal = SUCCESS;
	
    if( 0 == gServiceApp.referenceCounter)
    {
        if( pIModule == NULL || ps == NULL)
        {
            return EFAILED;
        }

        MEMSET(&gServiceApp, 0, sizeof(gServiceApp));
        INIT_VTBL(&gServiceApp, IServiceApp, gServiceAppMethods);

       
        gServiceApp.m_pShell     = ps;
        gServiceApp.m_pModule    = pIModule;
        retVal = CServiceApp_InitAppData(&gServiceApp);
        if(retVal != SUCCESS)
        {
            CServiceApp_FreeAppData((ServiceApp*)&gServiceApp);
            return retVal;
        }
        (void) ISHELL_AddRef( ps);
        (void) IMODULE_AddRef( pIModule);
        gServiceApp.referenceCounter  = 0;
        
    }

    ++ gServiceApp.referenceCounter;
    *ppObj = ( IServiceApp*)&gServiceApp;
    
    return retVal;
}

/*==============================================================================
函数:
    CServiceApp_InitAppData

说明:
    初始化 ServiceApp Applet 结构数据缺省值。

参数:
    pMe [in]: 指向ServiceApp Applet 对象结构的指针。该结构包含小程序的特定信息。

返回值:
    EFAILED: 初始化失败。
    SUCCESS: 初始化成功。

备注:

==============================================================================*/
static int CServiceApp_InitAppData(ServiceApp *pMe)
{
    if (NULL == pMe)
    {
        return EFAILED;
    }

    pMe->m_MainSel  = 0;

    if (ISHELL_CreateInstance(pMe->m_pShell, AEECLSID_DISPLAY, (void **) &pMe->m_pDisplay) != SUCCESS)
    {    	
        CServiceApp_FreeAppData(pMe);
        return EFAILED;
    }
	if (AEE_SUCCESS != ISHELL_CreateInstance(pMe->m_pShell,AEECLSID_ANNUNCIATOR,(void **)&pMe->m_pIAnn))
    {    	
        CServiceApp_FreeAppData(pMe);
        return EFAILED;
    }    

    return SUCCESS;
}

/*==============================================================================
函数:
    ServiceApp_FreeAppData

说明:
    释放 ServiceApp Applet 相关资源。

参数:
    pMe [in]: 指向ServiceApp Applet 对象结构的指针。该结构包含小程序的特定信息。

返回值:
    none

备注:

==============================================================================*/
static void CServiceApp_FreeAppData(ServiceApp *pMe)
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
FUNCTION:  ServiceApp_AddRef

DESCRIPTION: 

PARAMETERS:

RETURN VALUE:

COMMENTS:

SIDE EFFECTS:

SEE ALSO:

=============================================================================*/
static uint32  ServiceApp_AddRef( IServiceApp *pi)
{
    return ( ++gServiceApp.referenceCounter);
}

/*=============================================================================
FUNCTION:  ServiceApp_Release

DESCRIPTION: 

PARAMETERS:

RETURN VALUE:

COMMENTS:

SIDE EFFECTS:

SEE ALSO:

=============================================================================*/
static uint32  ServiceApp_Release( IServiceApp *pi)
{
    register ServiceApp *pMe = (ServiceApp*)pi;

    if (pMe->referenceCounter == 0)
    {
        return 0;
    }
    
    if( --pMe->referenceCounter)
    {
        return pMe->referenceCounter;
    }


    CServiceApp_FreeAppData(pMe);

    (void) ISHELL_Release(pMe->m_pShell);
    (void) IMODULE_Release(pMe->m_pModule);

    return 0;
}

/*=============================================================================
FUNCTION:  Service_HandleEvent

DESCRIPTION: 

PARAMETERS:

RETURN VALUE:

COMMENTS:

SIDE EFFECTS:

SEE ALSO:

=============================================================================*/
static boolean ServiceApp_HandleEvent( IServiceApp *pi,
    AEEEvent eCode,
    uint16  wParam,
    uint32 dwParam
)
{
    ServiceApp *pMe = (ServiceApp*)pi;
    AEEAppStart* as = NULL;
    AEEDeviceInfo di; 

    ISHELL_GetDeviceInfo(pMe->m_pShell,&di); 
    switch ( eCode)
    {
		case EVT_APP_START:
		{
			ServiceApp_ShowDialog(pMe, IDD_SERVICE_LIST);					
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
            
            return ServiceApp_ListMenuHandler(pMe, eCode, wParam, dwParam);

        case EVT_USER:
        case EVT_DIALOG_START:

            return ServiceApp_ListMenuHandler(pMe, eCode, wParam, dwParam);
       
        case EVT_USER_REDRAW:
            return ServiceApp_ListMenuHandler(pMe, eCode, wParam, dwParam);
			
        case EVT_DIALOG_END:
		  
            (void) ServiceApp_ListMenuHandler(pMe, eCode, wParam, dwParam);
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
            return ServiceApp_ListMenuHandler(pMe, eCode, wParam, dwParam);

        case EVT_KEY_PRESS:
        case EVT_KEY_RELEASE:
        case EVT_COMMAND:
            return ServiceApp_ListMenuHandler(pMe, eCode, wParam, dwParam);
        default:
            return ServiceApp_ListMenuHandler(pMe, eCode, wParam, dwParam);
    }
}


/*==============================================================================
函数:
       ServiceApp_ShowDialog

说明:
    函数由状态处理函数用来唤起相应对话框资源ID的对话框。

参数:
    pMe [in]: 指向 ServiceAppApplet 对象结构的指针。该结构包含小程序的特定信息。
    dlgResId [in]: 对话框资源ID，函数根据该ID创建对话框。

返回值:
    none

备注:

==============================================================================*/
void ServiceApp_ShowDialog(ServiceApp  *pMe,  uint16 dlgResId)
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
    nRet = ISHELL_CreateDialog(pMe->m_pShell,SERVICE_RES_FILE_LANG,dlgResId,NULL);
    if (nRet != SUCCESS)
    {
        MSG_FATAL("Failed to create the dialog %d in the MAINMENU applet.",dlgResId,0,0);
    }
}
/*=============================================================================
FUNCTION:  ServiceApp_ListMenuHandler

DESCRIPTION:   列表菜单

PARAMETERS:

=============================================================================*/

static boolean ServiceApp_ListMenuHandler(ServiceApp *pMe, AEEEvent eCode, uint16 wParam, uint32 dwParam)
{
    PARAM_NOT_REF(dwParam)
    IMenuCtl *pMenu = (IMenuCtl*)IDIALOG_GetControl(pMe->m_pActiveIDlg,IDC_SERVICE_LIST);
        
    if (pMenu == NULL)
    {
        return FALSE;
    }

    switch (eCode)
    {
        case EVT_DIALOG_INIT:	
			//IANNUNCIATOR_SetHasTitleText(pMe->m_pIAnn, TRUE);
			IANNUNCIATOR_SetFieldIsActiveEx(pMe->m_pIAnn,FALSE); 
			
            IMENUCTL_AddItem(pMenu, SERVICE_RES_FILE_LANG,IDS_SERVICE_UTK, IDS_SERVICE_UTK, NULL, 0); 
			IMENUCTL_AddItem(pMenu, SERVICE_RES_FILE_LANG,IDS_SERVICE_BROWSER, IDS_SERVICE_BROWSER, NULL, 0); 
			
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
                          SERVICE_RES_FILE_LANG,
                          IDS_SERVICE_UTK + i - 1,
                          pstr,
                          sizeof(pstr));
                    

                    WSTRLCAT(pwsz,pstr,sizeof(pwsz));
					
                    ERR("Service_ListMenuHandler::%d pwsz::%s",i,pwsz,0);
                    {
                        IMENUCTL_SetItemText(pMenu, IDS_SERVICE_UTK + i - 1, NULL, NULL, pwsz);
                    }                    
                }                
				
                IMENUCTL_SetProperties(pMenu, MP_UNDERLINE_TITLE|MP_WRAPSCROLL);
                IMENUCTL_SetOemProperties( pMenu, OEMMP_USE_MENU_STYLE);
                IMENUCTL_SetBottomBarType(pMenu,BTBAR_SELECT_BACK);
                IMENUCTL_SetSel(pMenu, pMe->m_MainSel);
                (void) ISHELL_PostEvent(pMe->m_pShell, AEECLSID_SERVICE, EVT_USER_REDRAW,0,0);
            }
            return TRUE;
            
        case EVT_USER_REDRAW:			
			{				
		  		AECHAR WTitle[40] = {0};
				
				(void)ISHELL_LoadResString(pMe->m_pShell,
                        SERVICE_RES_FILE_LANG,                                
                        IDS_SERVICE_TITLE,
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
					MSG_FATAL("IDS_MAIN_MENU_SERVICES...........22222222222222",0,0,0);
					OEM_SetUCBROWSER_ADSAccount();
					SetBrowserArr_MainEx(pMe,(char*)""); //ISHELL_StartApplet(pMe->m_pShell, AEECLSID_UCWEB);   
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
                case IDS_SERVICE_UTK:
                {
					ISHELL_StartApplet(pMe->m_pShell, AEECLSID_APP_UTK);	
                    return TRUE;
                }
				case IDS_SERVICE_BROWSER:
				{
					MSG_FATAL("IDS_MAIN_MENU_SERVICES...........22222222222222",0,0,0);
					OEM_SetUCBROWSER_ADSAccount();
					SetBrowserArr_MainEx(pMe,(char*)""); //ISHELL_StartApplet(pMe->m_pShell, AEECLSID_UCWEB);
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

static int SetBrowserArr_MainEx(ServiceApp *pMe,char *purl)
{
	int Result = EUNSUPPORTED;
	PppAccounts Account;
    char username[PPP_MAX_USER_ID_LEN] = {0};
    char password[PPP_MAX_PASSWD_LEN] = {0};
	char access_point[40] = {0};
	char urlCan[1024] = {0};

	if(SUCCESS == OEM_GetPppAccounts(&Account, DS_WAP20_TYPE))
    {
    	MSG_FATAL("read uim card..................",0,0,0);
    	MEMCPY(username, Account.user_id_info, STRLEN(Account.user_id_info));	
    	MEMCPY(password, Account.passwd_info, STRLEN(Account.passwd_info));	
    }
    else
    {
    	MSG_FATAL("read uim cefs..................",0,0,0);

    	OEM_GetConfig(CFGI_BREWSET_USENAME,&username,sizeof(username));
		OEM_GetConfig(CFGI_BREWSET_PASSWORD,&password,sizeof(password));
    }
	MSG_FATAL("STRLEN(purl)===========%d",STRLEN(purl),0,0);
	DBGPRINTF("SetBrowserArr_MainEx_ADSAccountusername1 =%s", username);  
    DBGPRINTF("SetBrowserArr_MainEx_ADSAccountuserpassword1 =%s", password);
	if(purl && STRLEN(purl)>1)
	{
        SPRINTF(urlCan,"call_ucweb:setexternurl:%s\2\3",purl);
		//STRCAT(urlCan,"useragent:BREW-Applet/0x20068888 (BREW/3.1.5.20; DeviceId: 8976509865757e; Lang: hi; Profile/MIDP-2.0_Configuration/CLDC-1.1) ucweb-squid\2\3");
	}
	else
	{
		//STRCPY(urlCan,"useragent:BREW-Applet/0x20068888 (BREW/3.1.5.20; DeviceId: 8976509865757e; Lang: hi; Profile/MIDP-2.0_Configuration/CLDC-1.1) ucweb-squid\2\3");
	}
    
	if(STRISTR (username,"mts"))
	{
		STRCPY(access_point,"10.50.5.140:8080");
		MSG_FATAL("mst................",0,0,0);
	}
	else if(STRISTR (username,"tata")||STRISTR (username,"wapuser"))
	{
		STRCPY(access_point,"172.23.252.15:9401");
		MSG_FATAL("tata................",0,0,0);
	}
	else if(STRISTR (username,"reliance"))
	{
		STRCPY(access_point,"http://wapgw.ricinfo.com:8080");
		MSG_FATAL("reliance................",0,0,0);
	}
	else if(STRISTR (username,"vmi"))
	{
		STRCPY(access_point,"172.23.142.15:9401");
		MSG_FATAL("vmi................",0,0,0);
	}
	STRCAT(urlCan,access_point);
	DBGPRINTF("urlCan==%s", urlCan);
    if(urlCan[0])
    {
	    Result = ISHELL_StartAppletArgs(pMe->m_pShell, AEECLSID_UCWEB, (char*)urlCan);
    }
    else
    {
        Result = ISHELL_StartApplet(pMe->m_pShell, AEECLSID_UCWEB);
    }

	return Result;	

	
}

