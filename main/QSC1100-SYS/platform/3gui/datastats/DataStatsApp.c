/*=============================================================================

FILE: DataStatsApp.c

SERVICES: Data Diagnostics Applet

GENERAL DESCRIPTION:

PUBLIC CLASSES AND STATIC FUNCTIONS:

INITIALIZATION AND SEQUENCING REQUIREMENTS:


(c) COPYRIGHT 2002 - 2007 QUALCOMM Incorporated.
                   All Rights Reserved.

                   QUALCOMM Proprietary
=============================================================================*/
/*=============================================================================
						EDIT HISTORY FOR MODULE

$Header: //depot/asic/msmshared/apps/DataStats/DataStatsApp.c#24 $
$DateTime: 2007/06/20 12:12:33 $
$Author: sanjinij $
$Change: 498942 $

This section contains comments describing changes made to the module.
Notice that changes are listed in reverse chronological order.

when       who     what, where, why
--------   ---     ---------------------------------------------------------
06/15/07   sj       Addition of two flags:im_cn_flag and dhcp_pcscf flag
05/05/07   mjm      Updated to account for changes to IPDP interface
09/15/06   sj       Featurized pcscf addr changes with FEATURE_IMS
08/08/06   sj       Added support for PCSCF address
05/24/06   JD       Added call to SelectFonts to increase size on VGA
08/02/05   sun      Added support for IPV6
05/01/05   RI       Added QoS menu.
08/23/04   ak       Added check for call ids
07/12/04   sun      Added support for 16 profiles and to edit profile name
01/19/04   ram      Use brew extension to set PDP profiles.
12/05/03   ram      Fixed buffer overflow.
12/01/03   ram      Lint related changes.
11/20/03   ram      DDTM Support and remove HDR_ABORT_PPP support
11/18/03   ram      Code cleanup
11/12/03   ram      App Launch with arguments
29/10/03   ram      Integration with ICM and code-cleanup.
07/24/03   ram      Created Data Diagnsotics Source File 

=============================================================================*/


/*===========================================================================

INCLUDE FILES FOR MODULE

===========================================================================*/

#include "DataStatsApp.h"
#include "datastatsapp_res.h"

#include "DataStatsDlg.h"

#include "AEEAppGen.h"        // AEEApplet structure and Applet services

#include "OEMFeatures.h"
#ifdef FEATURE_BREW_3_0
# include "AEE_OEMHeap.h"
# include "OEMHeap.h"
#endif

#if defined(FEATURE_WCDMA) || defined(FEATURE_GSM_GPRS)
#include "PDP.BID"
#endif

                    /*  Constant Definitions*/

/*===========================================================================

LOCAL/STATIC DATA

===========================================================================*/
static const PFNDLGHANDLER DlgEventHandlers[] = {
    NULL,                                               
    CDataStatsApp_CallSummaryDlgHandler,                 /* 01 : IDD_CALL_SUMMARY        */
    CDataStatsApp_CallDetailDlgHandler,                  /* 02 : IDD_CALL_DETAIL         */
    CDataStatsApp_CallEndedDlgHandler,                   /* 03 : IDD_CALL_ENDED          */
    CDataStatsApp_EditCHAPPasswordDlgHandler,            /* 04 : IDD_EDIT_CHAP_PASSWORD  */
    CDataStatsApp_SettingsDlgHandler,                    /* 05 : IDD_SETTINGS            */
    CDataStatsApp_StatisticsDlgHandler,                  /* 06 : IDD_STATISTICS          */
    CDataStatsApp_SelectActiveProfileDlgHandler,         /* 07 : IDD_SELECT_ACTIVE       */
    CDataStatsApp_EditProfileDlgHandler,                 /* 08 : IDD_EDIT_PROFILE        */
    CDataStatsApp_ViewProfileDlgHandler,                 /* 09 : IDD_VIEW_PROFILE        */
    CDataStatsApp_EditAPNDlgHandler,                     /* 10 : IDD_EDIT_APN            */
    CDataStatsApp_EditIPDlgHandler,                      /* 11 : IDD_EDIT_IP             */
    CDataStatsApp_EditSecurityDlgHandler,                /* 12 : IDD_EDIT_SECURITY       */
    CDataStatsApp_EndCallDlgHandler,                     /* 13 : IDD_END_CALL            */
    CDataStatsApp_EndAllDlgHandler,                      /* 14 : IDD_END_ALL             */
    CDataStatsApp_ErrorDlgHandler,                       /* 15 : IDD_ERROR               */
    CDataStatsApp_EditPAPParamsDlgHandler,               /* 16 : IDD_EDIT_PAP_PARAMS     */
    CDataStatsApp_DDTMDlgHandler,                        /* 17 : IDD_DDTM     */
    CDataStatsApp_EditProfileName,                       /* 18 : IDD_EDIT_PROFILE_NAME   */
    CDataStatsApp_EditQoSDlgHandler,                     /* 19 : IDD_EDIT_QOS            */
    CDataStatsApp_EditQoSUMTSDlgHandler,                 /* 20 : IDD_EDIT_QOS_UMTS       */
    CDataStatsApp_EditQoSUMTSTCDlgHandler,               /* 21 : IDD_EDIT_QOS_UMTS_TC    */
    CDataStatsApp_EditQoSUMTSMaxUlBRDlgHandler,          /* 22 : IDD_EDIT_QOS_UMTS_MAX_UL_BR */
    CDataStatsApp_EditQoSUMTSMinUlBRDlgHandler,          /* 23 : IDD_EDIT_QOS_UMTS_MIN_UL_BR */
    CDataStatsApp_EditQoSUMTSMinDlBRDlgHandler,          /* 24 : IDD_EDIT_QOS_UMTS_MIN_UL_BR */
    CDataStatsApp_EditQoSUMTSMaxDlBRDlgHandler,          /* 25 : IDD_EDIT_QOS_UMTS_MAX_DL_BR */
    CDataStatsApp_EditQoSGPRSPrecDlgHandler,             /* 26 : IDD_EDIT_QOS_GPRS_PREC */
    CDataStatsApp_EditQoSGPRSDelayDlgHandler,            /* 27 : IDD_EDIT_QOS_GPRS_DELAY_CLASS */
    CDataStatsApp_EditQoSGPRSDlgHandler,                 /* 28 : IDD_EDIT_QOS_GPRS */
    CDataStatsApp_EditQoSGPRSRelDlgHandler,              /* 29 : IDD_EDIT_QOS_GPRS_REL_CLASS */
    CDataStatsApp_EditQoSGPRSMeanDRDlgHandler,           /* 30 : IDD_EDIT_QOS_GPRS_MEAN_DR */
    CDataStatsApp_EditQoSGPRSPeakDRDlgHandler,           /* 31: IDD_EDIT_QOS_GPRS_PEAK_DR */
    CDataStatsApp_EditPDPTypeDlgHandler,                 /* 32: IDD_PDP_TYPE*/
#ifdef FEATURE_DATA_IMS
    CDataStatsApp_PCSCFReqDlgHandler,                    /* 33: IDD_EDIT_PCSCF*/
    CDataStatsApp_IMCNFlagDlgHandler,                    /* 34: IDD_EDIT_IM_CN_FLAG */
    CDataStatsApp_DHCPPCSCFDlgHandler,                   /* 35: IDD_EDIT_DHCP_PCSCF */    
#endif //FEATURE_DATA_IMS
}; /* End DlgEventHandlers[] */

#if defined(AEE_STATIC)
	static const AEEAppInfo gaiDataStatsApp = 
	{
		AEECLSID_DATASTATS,
		DATASTATSAPP_RES_FILE,
		0,
		0,
		0,
		0,
		0,
		AFLAG_STATIC | AFLAG_PHONE |AFLAG_HIDDEN
	};
#endif /* defined(AEE_STATIC) */

#if defined(AEE_STATIC)
int CDataStatsApp_CreateInstance(AEECLSID ClsId, IShell *pIShell, IModule *po, void **ppObj);
#else
int AEEClsCreateInstance(AEECLSID ClsId, IShell *pIShell, IModule *po, void **ppObj);
#endif

int CDataStatsApp_Load(IShell   *ps, 
						   void     *pHelpers, 
						   IModule **pMod);


/*=============================================================================
    
    FORWARD DECLARATIONS OF STATIC FUNCTIONS

=============================================================================*/
static boolean CDataStatsApp_InitAppData(IApplet * pi);

static boolean CDataStatsApp_HandleEvent(CDataStatsApp  *pMe, 
									  AEEEvent     eCode, 
									  uint16       wParam, 
									  uint32       dwParam);


static void CDataStatsApp_FreeAppData(CDataStatsApp *pMe);


static boolean CDataStatsApp_OnAppStart(CDataStatsApp  *pMe, 
									 uint16       wParam, 
									 uint32       dwParam);

static boolean CDataStatsApp_OnAppStop(CDataStatsApp  *pMe, 
									 uint16       wParam, 
									 uint32       dwParam);

static void CDataStatsApp_DrawSplash(CDataStatsApp * pMe);
/*===========================================================================

                      STATIC and PUBLIC FUNCTIONS

===========================================================================*/

#if defined(AEE_STATIC)
/*=============================================================================
FUNCTION: CDataStatsApp_GetModInfo

DESCRIPTION:  Called by BREW to retrieve information about this module

PARAMETERS:
*ps: 
**ppClasses: 
**pApps: 
*pnApps: 
*pwMinPriv: 

RETURN VALUE:
PFNMODENTRY: 

COMMENTS:

SIDE EFFECTS:

SEE ALSO:

=============================================================================*/
PFNMODENTRY CDataStatsApp_GetModInfo(IShell      *ps,
								  AEECLSID   **ppClasses,
								  AEEAppInfo **pApps, 
								  uint16      *pnApps,
								  uint16      *pwMinPriv)
/*lint -esym(715,ps) */
/*lint -esym(715,pwMinPriv) */
/*lint -esym(715,ppClasses) */
{
	
  
	*pApps = (AEEAppInfo *)&gaiDataStatsApp;
	*pnApps = 1; 
	return (PFNMODENTRY) CDataStatsApp_Load;
}

/*=============================================================================
FUNCTION: CDataStatsApp_Load

DESCRIPTION:  Load the module into memory

PARAMETERS:
*ps: 
*pHelpers: 
**pMod: 

RETURN VALUE:
int: 

COMMENTS:

SIDE EFFECTS:

SEE ALSO:

=============================================================================*/
int CDataStatsApp_Load(IShell   *ps, 
						   void     *pHelpers, 
						   IModule **pMod)
{
	return AEEStaticMod_New(sizeof(AEEMod),
		ps,
		pHelpers,
		pMod,
		(PFNMODCREATEINST) CDataStatsApp_CreateInstance,
		(PFNFREEMODDATA) NULL);
}

#endif /* defined(AEE_STATIC) */


/*=============================================================================
FUNCTION: CDataStatsApp_CreateInstance

DESCRIPTION:  Called by BREW to create an instance of an object contained
in this module.

PARAMETERS:
ClsId: 
*pIShell: 
*po: 
**ppObj: 

RETURN VALUE:
int: 

COMMENTS:

SIDE EFFECTS:

SEE ALSO:

=============================================================================*/
#if defined(AEE_STATIC)
int CDataStatsApp_CreateInstance(AEECLSID ClsId, IShell *pIShell, IModule *po, void **ppObj)
#else
int AEEClsCreateInstance(AEECLSID ClsId, IShell *pIShell, IModule *po, void **ppObj)
#endif
{
	*ppObj = NULL;

	if(ClsId == AEECLSID_DATASTATS)
	{
    
        if (AEEApplet_New(sizeof(CDataStatsApp), ClsId, pIShell, po,
							(IApplet**)ppObj, (AEEHANDLER) CDataStatsApp_HandleEvent,
							(PFNFREEAPPDATA) CDataStatsApp_FreeAppData) == TRUE) 
		{
			if(CDataStatsApp_InitAppData((IApplet*)*ppObj) == TRUE)
			{
				return (AEE_SUCCESS);
			}
			else
			{
				IAPPLET_Release(((IApplet *) *ppObj));
				*ppObj = NULL;
				return (EFAILED);
			}
		}
		else
		{
			return (ENOMEMORY);
		}
	}
	return (EFAILED);
}



/*=============================================================================
FUNCTION: CDataStatsApp_InitAppData

DESCRIPTION:
   Initializes the application data

PARAMETERS:
   *pi:

RETURN VALUE:
    None

COMMENTS:

SIDE EFFECTS:

SEE ALSO:

=============================================================================*/
static boolean CDataStatsApp_InitAppData(IApplet * pi)
{
  /*lint -e740 */
  CDataStatsApp * pMe = (CDataStatsApp*) pi;
  /*lint +e740 */
  pMe->m_deviceInfo.wStructSize = sizeof(pMe->m_deviceInfo);
  ISHELL_GetDeviceInfo(pMe->a.m_pIShell, &pMe->m_deviceInfo);
  /* Increase font sizes on VGA displays */
  AppComFunc_SelectFonts(pMe->a.m_pIDisplay);
  
  return TRUE;
}
/*=============================================================================
FUNCTION: CDataStatsApp_HandleEvent

DESCRIPTION:  Dialer Applet event handler

PARAMETERS:
*pMe: CDataStatsApp object pointer
eCode: Event code
wParam: word event parameter
dwParam: dword event parameter

RETURN VALUE:
boolean: TRUE if the event was handled 

COMMENTS:

SIDE EFFECTS:

SEE ALSO:

=============================================================================*/
static boolean CDataStatsApp_HandleEvent(CDataStatsApp *pMe, 
									  AEEEvent    eCode, 
									  uint16      wParam, 
									  uint32      dwParam)
{
  switch (eCode)
  {
  case EVT_APP_START:
	  return CDataStatsApp_OnAppStart(pMe, wParam, dwParam);

  case EVT_APP_STOP:
    return CDataStatsApp_OnAppStop(pMe, wParam, dwParam);
      
  case EVT_DIALOG_INIT:
    /*lint -e539*/
    if (DlgEventHandlers[wParam] != NULL)
    {
      IDIALOG_SetEventHandler((IDialog *) dwParam, DlgEventHandlers[wParam], pMe);
      IDIALOG_SetProperties((IDialog *) dwParam, DLG_HANDLE_ALL_EVENTS );
      return TRUE;
	  }
	  return FALSE;
    /*lint +e539*/          
  case EVT_KEY:
    switch(wParam)
    {
    case AVK_END:
    case AVK_CLR:
        return CheckAEEReturnStatus(ISHELL_CloseApplet(pMe->a.m_pIShell, FALSE));
    
    default:
        CDataStatsApp_DrawSplash(pMe);    
    }
    return TRUE;

  case EVT_NOTIFY:

    if (((AEENotify *) dwParam)->cls == AEECLSID_CM_NOTIFIER)
    {
      AEECMNotifyInfo *EventInfo = ((AEENotify *)dwParam)->pData;
      if (EventInfo->event_data.call.call_id < MAX_NUM_DATA_CALLS) {
        switch(EventInfo->event)
        {
          case AEECM_EVENT_CALL_END:
          case AEECM_EVENT_CALL_CONNECT:
          case AEECM_EVENT_CALL_ENTER_DORMANT:
          case AEECM_EVENT_CALL_EXIT_DORMANT:
            CDataStatsApp_UpdateSummaryMenu((void*)pMe, EventInfo);
            break;

          default:
            break;
        }
      }
      else
	  {
          MSG_ERROR("Call Id greater than Max Call Ids",0,0,0);
      }
    }
    break;

  case EVT_USER:
  {
    IDialog *pIDialog;
    uint16 wDlgID;

    pIDialog = ISHELL_GetActiveDialog(pMe->a.m_pIShell);
    /* there can be no Active dialog at this point! */

    if(pIDialog)
    {  /* if there is an existing dialog, pass the event to the dialog */
      wDlgID = IDIALOG_GetID(pIDialog);

      if ( wDlgID == 0)
      {
        return FALSE;
      }
      /* find the appropriate event handler */
      if(wDlgID < ARR_SIZE(DlgEventHandlers))
      { /* check for array bound */
        if(DlgEventHandlers[wDlgID] != NULL)
        {
          if(DlgEventHandlers[wDlgID](pMe, eCode, wParam, dwParam))
          {
            /* event handled by dialog event handler */
            return TRUE;
          }
          else
          {  /* dialog didn't handle this event, require app to handle it */
            /* require handling by app's event handler */
            return FALSE;
          }
        }
      }
      return FALSE; /* error */
    }
    else
    {
      return FALSE;
    }
  }

  default:
    break;
  }

  return FALSE;
}




/*=============================================================================
FUNCTION: CDataStatsApp_FreeAppData

DESCRIPTION:  Free applet resources

PARAMETERS:
*pMe: CDataStatsApp object pointer 

RETURN VALUE:
None

COMMENTS:

SIDE EFFECTS:

SEE ALSO:

=============================================================================*/
static void CDataStatsApp_FreeAppData(CDataStatsApp *pMe)
{
  int i;

  if(NULL == pMe)
  {
    DATASTATS_ERR("pMe NULL");
    return;
  }
  
  ISHELL_RegisterNotify(pMe->a.m_pIShell, AEECLSID_DATASTATS,
    AEECLSID_CM_NOTIFIER, 0);

  /*lint -e740 */
  ICM_Release(pMe->m_pICM);
#if defined(FEATURE_WCDMA) || defined(FEATURE_GSM_GPRS)
  IPDP_Release(pMe->m_pIPDP);
#endif
  /*lint +e740 */
  
  for( i = 0; i < MAX_NUM_DATA_CALLS; i++)
  {
    FREEIF(pMe->m_pEndedCalls[i]);
  }
    
}

static boolean CDataStatsApp_OnAppStop(CDataStatsApp  *pMe, 
									 uint16       wParam, 
									 uint32       dwParam)
/*lint -esym(715,wParam) */
/*lint -esym(715,dwParam) */
{
  FREEIF(pMe->szError);
  FREEIF(pMe->szSystemInfo);
#if defined(FEATURE_WCDMA) || defined(FEATURE_GSM_GPRS)
  FREEIF(pMe->apn);
  FREEIF(pMe->password);
  FREEIF(pMe->username);
  FREEIF(pMe->profileText);
#endif
  return TRUE;
}
/*=============================================================================
FUNCTION: CDialer_OnAppStart

DESCRIPTION:  EVT_APP_START event handler

PARAMETERS:
*pMe: CDataStatsApp object pointer 
wParam: word event parameter
dwParam: dword event parameter

RETURN VALUE:
boolean: TRUE if the event was handled 

COMMENTS:

SIDE EFFECTS:

SEE ALSO:

=============================================================================*/
static boolean CDataStatsApp_OnAppStart(CDataStatsApp  *pMe, 
									 uint16       wParam, 
									 uint32       dwParam)
/*lint -esym(715,wParam) */
{
	
  int i;
  AEEDeviceInfo       pdi;
  const char *pszArg = ((AEEAppStart *) dwParam)->pszArgs;
  /* Get the device information */
  ISHELL_GetDeviceInfo(pMe->a.m_pIShell, &pdi);
  pMe->m_cxWidth = pdi.cxScreen;
  pMe->m_cyHeight = pdi.cyScreen;
  
  pMe->szError = (AECHAR*) MALLOC(MAX_ERROR_STR_LEN*sizeof(AECHAR));
  pMe->szSystemInfo = (char*) MALLOC(MAX_SYSTEM_INFORMATION_LENGTH);
  if((pMe->szError == NULL) || (pMe->szSystemInfo == NULL))
  {
    MSG_ERROR("Malloc Failed",0, 0, 0);
    FREEIF(pMe->szError);
    FREEIF(pMe->szSystemInfo);
    return FALSE;
  }
#if defined(FEATURE_WCDMA) || defined(FEATURE_GSM_GPRS)
  pMe->pTabController = NULL;
  pMe->apn = (AECHAR*) MALLOC((AEEPDP_MAX_APN_STRING_LEN+1)*sizeof(AECHAR));
  pMe->password = (byte*) MALLOC(AEEPDP_MAX_QCPDP_STRING_LEN+1);
  pMe->username = (byte*) MALLOC(AEEPDP_MAX_QCPDP_STRING_LEN+1);
  if((pMe->apn == NULL)|| (pMe->password == NULL) || (pMe->username == NULL))
  {
    MSG_ERROR("Malloc Failed",0, 0, 0);
    FREEIF(pMe->szError);
    FREEIF(pMe->szSystemInfo);
    FREEIF(pMe->apn);
    FREEIF(pMe->password);
    FREEIF(pMe->username);
    return FALSE;
  }

#endif
  // Create the ICallManager object.
  if(ISHELL_CreateInstance(pMe->a.m_pIShell,AEECLSID_CALLMANAGER, 
                        (void **) &pMe->m_pICM) != SUCCESS)
  {
    DATASTATS_ERR("ICM Create Failed");
    FREEIF(pMe->szError);
    FREEIF(pMe->szSystemInfo);
#if defined(FEATURE_WCDMA) || defined(FEATURE_GSM_GPRS)
    FREEIF(pMe->apn);
    FREEIF(pMe->password);
    FREEIF(pMe->username);
#endif
    return FALSE;
  }
  if (pMe->m_pICM == NULL) 
  {
	  DATASTATS_ERR("Null pointer");
    FREEIF(pMe->szError);
    FREEIF(pMe->szSystemInfo);
#if defined(FEATURE_WCDMA) || defined(FEATURE_GSM_GPRS)
    FREEIF(pMe->apn);
    FREEIF(pMe->password);
    FREEIF(pMe->username);
#endif
    return FALSE;
  }

    // Register with ICallManager to recieve events
  ISHELL_RegisterNotify(pMe->a.m_pIShell, AEECLSID_DATASTATS,
      AEECLSID_CM_NOTIFIER, NMASK_CM_DATA_CALL);

  // Create the IPDP object.
#if defined(FEATURE_WCDMA) || defined(FEATURE_GSM_GPRS)
  if(ISHELL_CreateInstance(pMe->a.m_pIShell,AEECLSID_PDP, 
                        (void **) &pMe->m_pIPDP) != SUCCESS)
  {
    DATASTATS_ERR("IPDP Create Failed");
    FREEIF(pMe->szError);
    FREEIF(pMe->szSystemInfo);
    FREEIF(pMe->apn);
    FREEIF(pMe->password);
    FREEIF(pMe->username);
    ICM_Release(pMe->m_pICM);
    return FALSE;
  }
  if (pMe->m_pIPDP == NULL) 
  {
	  DATASTATS_ERR("Null pointer");
    FREEIF(pMe->szError);
    FREEIF(pMe->szSystemInfo);
    FREEIF(pMe->apn);
    FREEIF(pMe->password);
    FREEIF(pMe->username);
    ICM_Release(pMe->m_pICM);
    return FALSE;
  }
  pMe->profileText = (AECHAR*) MALLOC((IPDP_GetMaxProfileNameLen(pMe->m_pIPDP)+1)*sizeof(AECHAR));
  if(NULL == pMe->profileText)
  {
    MSG_ERROR("Malloc Failed",0, 0, 0);
    FREEIF(pMe->szError);
    FREEIF(pMe->szSystemInfo);
    FREEIF(pMe->apn);
    FREEIF(pMe->password);
    FREEIF(pMe->username);
    ICM_Release(pMe->m_pICM);
    return FALSE;
  }
#endif

  for( i = 0; i < MAX_NUM_DATA_CALLS; i++)
  {
    pMe->m_pEndedCalls[i] = NULL;
  }

  if(pszArg != NULL)
  {
    pMe->splashDrawn = TRUE;
    pMe->call_id = (AEECMCallID) (ATOI(pszArg));
    ISHELL_CreateDialog(pMe->a.m_pIShell, DATASTATSAPP_RES_FILE, IDD_CALL_DETAIL, NULL); 
  }
  else 
  {
    pMe->splashDrawn = TRUE;
    CDataStatsApp_DrawSplash(pMe);
  }
    
  return TRUE;

}
//Callback function.
void ImageLoadCallback(void *pUser, IImage *pImage, AEEImageInfo *po, int nErr)
/*lint -esym(715,pImage) */
/*lint -esym(715,po) */
{
  int x, y;
  AEEImageInfo   ii;
  CDataStatsApp *pMe = (CDataStatsApp*) pUser;
  if(nErr != 0 ) //SUCCESS?
  {
     DBGPRINTF("Error loading image");
    return;
  }
  if (pMe->pSplash)
    {
      /*lint -e620*/
      /*lint -e740 */
      IDISPLAY_ClearScreen(pMe->a.m_pIDisplay);
      //IIMAGE_SetFrameCount(pMe->pSplash, 4);
      //IIMAGE_SetAnimationRate(pMe->pSplash, 500);
      IIMAGE_GetInfo(pMe->pSplash, &ii);
      x = (pMe->m_cxWidth - ii.cxFrame)/2;
      y = (pMe->m_cyHeight - ii.cy)/2;
      
      IIMAGE_Start(pMe->pSplash,x,y);
      /*lint +e740 */
      /*lint +e620*/
    }

}

/************************************************************************************************/
/***                          This function will draw splash screen.                          ***/
/************************************************************************************************/
static void CDataStatsApp_DrawSplash(CDataStatsApp * pMe)
{
  IDialog *pIDialog;

  pIDialog = ISHELL_GetActiveDialog(pMe->a.m_pIShell);

  if(pIDialog  != NULL) // You are already in some dialog.
    return;

  // The following 'If statement' is entered only after the splash timer runs out...
  if (pMe->splashDrawn)
  {
    if(pMe->pSplash)
    {
      IIMAGE_Stop(pMe->pSplash);
      /*lint -e740 */
      IBASE_Release((IBase*) pMe->pSplash);
      /*lint +e740 */
    }
    ISHELL_CancelTimer(pMe->a.m_pIShell, (PFNNOTIFY)CDataStatsApp_DrawSplash, pMe);
    ISHELL_CreateDialog(pMe->a.m_pIShell, DATASTATSAPP_RES_FILE, IDD_CALL_SUMMARY, NULL); 
    return;
  }

  // Draw the splash screen, set the timer.
  // The timer callback calls this function and creates main menu dialog.
  {
    /*lint -e740 */
    pMe->pSplash = ISHELL_LoadResImage(pMe->a.m_pIShell, DATASTATSAPP_RES_FILE, IDB_SPLASH);
    IIMAGE_Start(pMe->pSplash, 0, 0);
    /*lint +e740 */
    //IIMAGE_Notify(pMe->pSplash , ImageLoadCallback, (void*)pMe);

   /*
    if (pMe->pSplash)
    {
      IIMAGE_GetInfo(pMe->pSplash, &ii);
      IDISPLAY_ClearScreen(pMe->a.m_pIDisplay);
      IIMAGE_SetFrameCount(pMe->pSplash, 4);
      IIMAGE_SetAnimationRate(pMe->pSplash, 500);
      IIMAGE_GetInfo(pMe->pSplash, &ii);
      x = (pMe->m_cxWidth - ii.cxFrame)/2;
      y = (pMe->m_cyHeight - ii.cy)/2;
      
      IIMAGE_Start(pMe->pSplash,x,y);
    
    }*/

    pMe->splashDrawn = TRUE;
    ISHELL_SetTimer(pMe->a.m_pIShell, 3000, (PFNNOTIFY)CDataStatsApp_DrawSplash, pMe);
  }  
}

