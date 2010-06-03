/*=============================================================================

FILE: AUXSettings.c

SERVICES: AUX Configuration Menus

GENERAL DESCRIPTION:
Creates and Handles all AUX related menus 

PUBLIC CLASSES AND STATIC FUNCTIONS:


INITIALIZATION AND SEQUENCING REQUIREMENTS:
None.

(c) COPYRIGHT 2007-2008 QUALCOMM Incorporated.
                   All Rights Reserved.

                   QUALCOMM Proprietary
=============================================================================*/
/*===========================================================================
                        EDIT HISTORY FOR MODULE

when       who     what, where, why
--------   ---     ----------------------------------------------------------
04/03/08   cvs     Revert WLAN subscription changes
11/28/07   sg      Don't send WLAN subscription unless Mode Preference is WLAN
10/29/07   sg      Support for setting the BREW Secure clock
09/24/07   snb     Add option to exit from USB MTP mode.
05/15/07   ympw    Increase the font size on VGA display.
03/26/07   ypmw    Add comments for registering ICM events.
03/23/07   ypmw    Fix crash on WLAN mode select when hitting EXIT button
02/30/07   cvs     Fix crash on entry in HDK non-WLAN builds
02/13/07   jas     Fix featurization to remove compiler warnings.
02/09/07   jas     Improve handling of CM events and sending WLAN subscription 
01/31/07   jas     Modify the app to stay in the background.
01/29/07   ypmw    Fix compiler and lint warnings
01/23/07   ypmw    Move WLAN application from Core App to AUX Settings
12/11/06   RT      Ported support for FEATURE_DATA_MULTI_PORTS
10/30/06   RT      Initial Revision

===========================================================================*/

/*===========================================================================

                      INCLUDES

===========================================================================*/
#include "OEMFeatures.h"
#include "AUXSettings.h"

#include "AEE.h"
#include "AEEShell.h"
#include "AEEStdLib.h"
#include "AEEModTable.h"
#include "AEEAppGen.h"
#include "AEEMenu.h"
#include "AppComFunc.h"

#include "AUXMenu.h"

#ifdef FEATURE_WLAN
#include "AUXWLANMenu.h"
#include "AUXWLANUtils.h"
#endif

#include "AUXUtils.h"
#include "auxsettings.brh"
#include "AUXSettings.bid"

#if MIN_BREW_VERSION(4,0)
  #include "AEESecureSysClock.bid"
#else
  #include "AEESecSysClock.bid"
#endif

/*===========================================================================

                      Constant Definitions

===========================================================================*/
/* Dialog Event Handler function pointer structure */
typedef boolean (PFNDLGHANDLER) (
                                 void *pUser,
                                 AEEEvent evt,
                                 uint16 w,
                                 uint32 dw );

char *gpAUXResourceFile = AUXSETTINGS_RES_FILE;

const char AUX_StartSIOCONFIGURATIONS[] = "SIOCONFIGURATIONS";
const char AUX_StartWLAN[] = "WLAN";
const char AUX_StartUSBDEVSW[] = "USBDEVSW";
const char AUX_StartSECTIME[] = "SECURETIME";

/*===========================================================================

                      STATIC DATA

===========================================================================*/
typedef struct{
  uint16 wDialogID;  /* Current Dialog ID */
  PFNDLGHANDLER *dlgHandler; /* Function pointer to Dialog Event Handler */
} DlgEvtHandlerType;

static const DlgEvtHandlerType AUXDlgEventHandlers[] = {
  {IDD_SIOCONFIGURATION, AUXMenuDlgEventHandler},
  {IDD_PORTMAP,          AUXPortMapDlgEventHandler},
#ifdef FEATURE_DATA_MULTI_PORTS
  {IDD_RUNTIME_PORTMAP,  AUXRunTimeMultiPortMapDlgEventHandler},
#else
  {IDD_RUNTIME_PORTMAP,  AUXRunTimePortMapDlgEventHandler},
#endif
  {IDD_DATA_BAUD,        AUXBaudDlgEventHandler},
  {IDD_DIAG_BAUD,        AUXBaudDlgEventHandler},
  {IDD_GENERAL_MESSAGE,  AUXMessageDlgEventHandler},
#ifdef FEATURE_WLAN
  {IDD_WLAN_SETTINGS,    AUXDynamicMenuDlgEventHandler},
  {IDD_ENTER_SSID,       AUXWLANOptEditDlgHandler},
  {IDD_ENTER_CHANNEL,    AUXWLANOptEditDlgHandler},
  {IDD_SSID_VIEW,        AUXStaticDlgEventHandler},
  {IDD_CHANNEL_VIEW,     AUXStaticDlgEventHandler},
  {IDD_WLAN_IP_CONFIG,   AUXLanDlgEventHandler},
  {IDD_WLAN_STATIC_IP_CONFIG, AUXConfigIpDlgEventHandler},
  {IDD_EDIT_IP,          AUXEditIpDlgEventHandler},
  {IDD_WLAN_SELECT,      AUXApp_WLANSelectEventHandler},
  {IDD_WLAN_STATUS,      AUXApp_WLANStatusEventHandler},
  {IDD_WLAN_SELECT_MODE, AUXWLANModeDlgEventHandler},
  {IDD_WLAN_SELECT_PREAMBLE,      AUXRadioButtonDlgEventHandler},
  {IDD_WLAN_LISTEN_INTERVAL_VIEW, AUXStaticDlgEventHandler},
  {IDD_WLAN_LISTEN_INTERVAL_EDIT, AUXWLANOptEditDlgHandler},
  {IDD_WLAN_RTS_VIEW,             AUXStaticDlgEventHandler},
  {IDD_WLAN_RTS_EDIT,             AUXWLANOptEditDlgHandler},
  {IDD_WLAN_SCAN_TIME_VIEW,       AUXStaticDlgEventHandler},
  {IDD_WLAN_SCAN_TIME_EDIT,       AUXWLANOptEditDlgHandler},
  {IDD_WLAN_FRAG_VIEW,            AUXStaticDlgEventHandler},
  {IDD_WLAN_FRAG_EDIT,            AUXWLANOptEditDlgHandler},
  {IDD_WLAN_SECURITY,             AUXWLANSecDlgEventHandler},
  {IDD_WLAN_SEC_WEP64,            AUXWLANSecEditDlgHandler},
  {IDD_WLAN_SEC_WEP128,           AUXWLANSecEditDlgHandler},
  {IDD_WLAN_SEC_WEP64_KEY_ID,     AUXWLANOptEditDlgHandler},
  {IDD_WLAN_SEC_WEP64_KEY1,       AUXWLANOptEditDlgHandler},
  {IDD_WLAN_SEC_WEP64_KEY2,       AUXWLANOptEditDlgHandler},
  {IDD_WLAN_SEC_WEP64_KEY3,       AUXWLANOptEditDlgHandler},
  {IDD_WLAN_SEC_WEP64_KEY4,       AUXWLANOptEditDlgHandler},
  {IDD_WLAN_SEC_WEP128_KEY_ID,    AUXWLANOptEditDlgHandler},
  {IDD_WLAN_SEC_WEP128_KEY1,      AUXWLANOptEditDlgHandler},
  {IDD_WLAN_SEC_WEP128_KEY2,      AUXWLANOptEditDlgHandler},
  {IDD_WLAN_SEC_WEP128_KEY3,      AUXWLANOptEditDlgHandler},
  {IDD_WLAN_SEC_WEP128_KEY4,      AUXWLANOptEditDlgHandler},
  {IDD_WLAN_SEC_WPA_PSK,          AUXWLANSecEditDlgHandler},
  {IDD_WLAN_SEC_WPA_PSK_PASSWORD, AUXWLANOptEditDlgHandler},
  {IDD_WLAN_SEC_WPA_TLS_CERT,     AUXWLANSecEditDlgHandler},
  {IDD_WLAN_SEC_WPA_TLS_CERT_USERID, AUXWLANOptEditDlgHandler},
  {IDD_WLAN_SEC_WPA_MD5,             AUXWLANSecEditDlgHandler},
  {IDD_WLAN_SEC_WPA_MD5_USERID,      AUXWLANOptEditDlgHandler},
  {IDD_WLAN_SEC_WPA_MD5_PASSWORD,    AUXWLANOptEditDlgHandler},
  {IDD_WLAN_SELECT_MODE_PROFILE,     AUXRadioButtonDlgEventHandler},
  {IDD_WLAN_START_NEW_ADHOC_NETWORK, AUXStaticDlgEventHandler},
  {IDD_WLAN_SELECT_NW_SEL_MODE,      AUXWLANNetSelModeDlgEventHandler},
  {IDD_WLAN_GENERAL_CONFIG,          AUXMenuDlgEventHandler},
  {IDD_WLAN_MAX_TX_POWER_VIEW,       AUXStaticDlgEventHandler},
  {IDD_WLAN_MAX_TX_POWER_EDIT,       AUXWLANOptEditDlgHandler},
  {IDD_WLAN_PERFORM_MANUAL_SCAN_NOW, AUXStaticDlgEventHandler},
  {IDD_WLAN_AUTOSCAN_SEARCHING,      AUXWLANAutoScanDlgEventHandler},
  {IDD_WLAN_AUTOSCAN_NO_ASSOCIATION, AUXWLANNoAssociationDlgEventHandler},
  {IDD_WLAN_SCAN_MODE,               AUXRadioButtonDlgEventHandler},
#endif /*#ifdef FEATURE_WLAN*/
  {IDD_AUXSETTINGS,                  AUXMenuDlgEventHandler},
#if defined(FEATURE_RUNTIME_DEVMAP) && defined(FEATURE_USB_DEFAULT_MTP)
  {IDD_USBDEV_DIALOG,                AUXRadioButtonDlgEventHandler},
#endif
  {IDD_SETSECURETIME_EDIT,           AUXTimeDateDlgEventHandler},
  {IDD_SETSECURETIME_VIEW,           AUXStaticDlgEventHandler},
}; /* End AUXDlgEventHandlers[] */


#ifdef AEE_STATIC
static AEEAppInfo gaiAUXSettings = {AEECLSID_AUXSETTINGS,
                    AUXSETTINGS_RES_FILE,
                    IDS_AUXAPP_TITLE,
                    0,0,0,0 , AFLAG_PHONE | AFLAG_STATIC | AFLAG_HIDDEN};
#endif /* AEE_STATIC */

/*===========================================================================

                      FUNCTION PROTOTYPES

===========================================================================*/
#ifdef FEATURE_WLAN
void AUXApp_SendWLANSubsAvail(AUXApp *pMe);
#endif

// Copy of RELEASEIF from AEEUsrSysClock.c
#define RELEASEIF(p) if (p) {IQI_Release((IQueryInterface *)(p)); (p) = NULL;}

/*===========================================================================

                      STATIC and PUBLIC FUNCTIONS

===========================================================================*/
/*===========================================================================
FUNCTION AUXSettings_FreeAppData

DESCRIPTION
  Release the application data

DEPENDENCIES
  none

SIDE EFFECTS
  None
===========================================================================*/
void AUXSettings_FreeAppData(IApplet* po)
{
  AUXApp *pMe = (AUXApp*)po;

  if(NULL == pMe)
  {
    AUX_ERR("pMe NULL", 0, 0, 0);
    return;
  }

#ifdef FEATURE_RUNTIME_DEVMAP
  FREEIF(pMe->m_pRdmDeviceList);
#endif

#ifdef FEATURE_WLAN
  if (pMe->m_WLAN.m_pICM) 
  {
    (void)ICM_Release(pMe->m_WLAN.m_pICM);
    pMe->m_WLAN.m_pICM = NULL;
  }
  
  if (pMe->m_WLAN.m_pIWIFI) 
  {
    (void)IWIFI_Release(pMe->m_WLAN.m_pIWIFI);
    pMe->m_WLAN.m_pIWIFI = NULL;
  }

  if (pMe->m_WLAN.m_pIWIFIOpts) 
  {
    (void)IWIFIOpts_Release(pMe->m_WLAN.m_pIWIFIOpts);
    pMe->m_WLAN.m_pIWIFIOpts = NULL;
  }
  FREEIF(pMe->m_WLAN.m_psICMWLANData);
  pMe->m_WLAN.m_psICMWLANData = NULL;

  FREEIF(pMe->m_WLAN.m_pWLANAuthInfo);
  pMe->m_WLAN.m_pWLANAuthInfo = NULL;
#endif /*#ifdef FEATURE_WLAN*/

  /* Secure ISYSCLOCK */
  RELEASEIF(pMe->m_pISecSysClock);
} /* End AUXSettings_FreeAppData */

/*===========================================================================
FUNCTION AUXSettings_InitAppData

DESCRIPTION
  Initializing the application data

DEPENDENCIES
  none

SIDE EFFECTS
  None
===========================================================================*/
boolean AUXSettings_InitAppData(IApplet* po)
{
  int nReturnStatus;

  AEEDeviceInfo di;
  AUXApp *pMe = (AUXApp*)po;
  if(NULL == pMe)
  {
    AUX_ERR("pMe NULL", 0, 0, 0);
    return FALSE;
  }

  pMe->m_bAppIsRunning = FALSE;

  pMe->m_MessageID = 0;
  pMe->m_pszMsgString = NULL;

  /* Secure ISYSCLOCK object */
  pMe->m_pISecSysClock = NULL;
#if MIN_BREW_VERSION(4,0)
     nReturnStatus = ISHELL_CreateInstance(pMe->a.m_pIShell, AEECLSID_SecureSysClock, (void**)&pMe->m_pISecSysClock);
#else
     nReturnStatus = ISHELL_CreateInstance(pMe->a.m_pIShell, AEECLSID_SECSYSCLOCK, (void**)&pMe->m_pISecSysClock);
#endif

  if ((nReturnStatus != SUCCESS) || (pMe->m_pISecSysClock == NULL))
  {
    RELEASEIF(pMe->m_pISecSysClock);
    pMe->m_pISecSysClock = NULL;
    AUX_ERR("Unable to create for ISECSYSCLOCK, 0x%x", nReturnStatus, 0, 0);
    return FALSE;
  }

  /* Initializing cxScreen */
  ISHELL_GetDeviceInfo(pMe->a.m_pIShell, &di);
  pMe->m_cxScreen = di.cxScreen;

#ifdef FEATURE_WLAN
  do
  {
    /* Create the Call Manager object. */
    nReturnStatus = ISHELL_CreateInstance(pMe->a.m_pIShell,
                                 AEECLSID_CM,
                                 (void **) &pMe->m_WLAN.m_pICM);

    if(nReturnStatus != SUCCESS || pMe->m_WLAN.m_pICM == NULL) 
    {
      AUX_ERR("Fail to create an ICM instance: %d", nReturnStatus, 0, 0);
      nReturnStatus = EFAILED;
      break;
    }

    /* Create the WIFI object. */
    nReturnStatus = ISHELL_CreateInstance(pMe->a.m_pIShell,
                                 AEECLSID_WIFI,
                                 (void **) &pMe->m_WLAN.m_pIWIFI);

    if ((nReturnStatus != SUCCESS) || (pMe->m_WLAN.m_pIWIFI == NULL)) 
    {
      AUX_ERR("Fail to create an IWIFI instance: %d", nReturnStatus, 0, 0);
      nReturnStatus = EFAILED;
      break;
    }

    /* Create the WIFI Options object. */
    nReturnStatus = ISHELL_CreateInstance(pMe->a.m_pIShell,
                                 AEECLSID_WIFI_OPTS,
                                 (void **) &pMe->m_WLAN.m_pIWIFIOpts);

    if ((nReturnStatus != SUCCESS) || (pMe->m_WLAN.m_pIWIFIOpts == NULL)) 
    {
      nReturnStatus = EFAILED;
      AUX_ERR("Fail to create an IWIFIOpts instance: %d", nReturnStatus, 0, 0);
      break;
    }

    pMe->m_WLAN.m_pWLANStatusDlg = NULL;

    /* Set default values, and then look a the IWIFI extension */
    nReturnStatus = Convert_ICMWLAN_BSS_TYPE_PREF_to_IWIFIModeOpt(
      (AEECMWLAN_BSS_TYPE_PREF)GetWLANMode(pMe), &pMe->m_WLAN.m_WLANMode);
    if (nReturnStatus != SUCCESS)
    {
      AUX_ERR("Failed to convert varible from AEECMWLAN_BSS_TYPE_PREF to AEEWIFIModeOption, %d",
               nReturnStatus, 0, 0);
      break;
    }
    pMe->m_WLAN.m_WLANEnc = (AEEWIFIEncryptionOption)GetWLANEnc(pMe);
    pMe->m_WLAN.m_WLANAuth = (AEEWIFIAuthenticationOption)GetWLANAuth(pMe);
    pMe->m_WLAN.m_bWIFIOptsLoaded = FALSE;
    pMe->m_WLAN.m_bAuthKeysUpdated = FALSE;
    pMe->m_WLAN.m_bNetSelModeUpdated = FALSE;
    pMe->m_WLAN.m_pWLANSettingsDialog = NULL;
    (void) MEMSET(pMe->m_WLAN.m_currentSSID, '\0', (AEECM_MAX_SSID_STR_SIZE)*sizeof(AECHAR));
    pMe->m_WLAN.m_psICMWLANData = NULL;
    pMe->m_WLAN.m_bWLANManualScanAssociationRequest = FALSE;
    pMe->m_WLAN.m_bWLANAdhocAssociationRequest = FALSE;
    pMe->m_WLAN.m_WLANBandPref = AEECM_BAND_PREF_NONE;
    pMe->m_WLAN.m_pWLANAuthInfo = NULL;
    pMe->m_WLAN.m_nNWSelectionMode = APPCOMFUNC_NW_SEL_INACTIVE;
  } while(0);

  if (nReturnStatus != SUCCESS)
  {
    AUXSettings_FreeAppData(po);
    return FALSE;
  }
#endif /*ifdef FEATURE_WLAN*/

  //This will increase the font size on VGA displays
  AppComFunc_SelectFonts(pMe->a.m_pIDisplay);

  return TRUE;
} /* End AUXSettings_InitAppData */

/*===========================================================================

FUNCTION AUXSettings_HandleEvent

DESCRIPTION
   This is the EventHandler for this app. All events to this app are handled in this
   function. All APPs must supply an Event Handler.

   AUXSettings creates a menu dialogue in the EVT_APP_START case, and therefore this handler
   is only the active event handler during AUXSettings startup.
   
PROTOTYPE:
   boolean AUXSettings_HandleEvent(IApplet * pi, AEEEvent eCode, uint16 wParam, uint32 dwParam)

PARAMETERS:
   pi: Pointer to the AEEApplet structure. This structure contains information specific
   to this applet. It was initialized during the AppCreateInstance() function.

   ecode: Specifies the Event sent to this applet

   wParam, dwParam: Event specific data.

DEPENDENCIES
   none

RETURN VALUE
   TRUE: If the app has processed the event
   FALSE: If the app did not process the event

SIDE EFFECTS
   none

COMMENTS for registering ICM events

   The condition to send wlan subscription status is when the phone is online. To be notified 
   if the phone is online is by registering an ICM phone event. Thus, once the phone sends the 
   WLAN subscription, it no longer needs to subscribe for phone events  The ICM SS event will 
   be registered during EVT_APP_START and EVT_APP_STOP. m_bAppIsRunning indicates if the applet 
   is running.  
   
   EVT_APP_START:
     if (phone: online)
       register(NMASK_CM_SS)
     else
       register(NMASK_CM_PHONE, NMASK_CM_SS)
     m_bAppIsRunning = TRUE

   EVT_APP_STOP:
     if (phone: online)
       register(0)
     else
       register(NMASK_CM_PHONE)
     m_bAppIsRunning = FALSE

   EVT_NOTIFY & NMASK_SHELL_INIT
     if (phone: online)
       AUXApp_SendWLANSubsAvail
     else
       register(NMASK_CM_PHONE)

   EVT_NOTIFY & NMASK_CM_PHONE
     AUXApp_SendWLANSubsAvail
     if (m_bAppIsRunning)
       register(NMASK_CM_SS)
     else
       register(0)
===========================================================================*/
static boolean AUXSettings_HandleEvent(AUXApp * pMe, AEEEvent eCode, uint16 wParam, uint32 dwParam)
{
  IDialog *pIDialog;
  uint16 wDlgID;
  int i, nErrCode;

  if (pMe == NULL)
  {
   AUX_ERR("pMe NULL", 0, 0, 0);
   return FALSE;
  }

#ifdef FEATURE_BREW_3_0
  // do not want to handle autorepeat key events
  if((eCode == EVT_KEY) && ((dwParam & KB_AUTOREPEAT) != 0))
  {
    return FALSE;
  }
#endif

  switch (eCode)
  {
    case EVT_APP_START:
    {
      AEEAppStart *aStart = (AEEAppStart*)dwParam;

      if (aStart == NULL)
      {
        AUX_ERR("aStart NULL", 0, 0, 0);
        return FALSE;
      }

      // Start the AUX Configuration dialog
      AUX_MSG ("Got EVT_APP_START", 0, 0, 0);
      
#ifdef FEATURE_WLAN
      if ((aStart->pszArgs != NULL) && 
          STRNCMP(aStart->pszArgs, 
                  AUX_StartWLAN, 
                  STRLEN(AUX_StartWLAN)) == 0)
      {
        nErrCode = ISHELL_CreateDialog(pMe->a.m_pIShell, gpAUXResourceFile,
                                       IDD_WLAN_SETTINGS, NULL);
        if(nErrCode != SUCCESS)
        {
          AUX_ERR("Unable to create Dialog %d: %d", IDD_WLAN_SETTINGS, nErrCode, 0);
          return FALSE;
        }
        pMe->m_RootDialogID = IDD_WLAN_SETTINGS;
      }
      else
#endif /* FEATURE_WLAN */
#if defined(FEATURE_RUNTIME_DEVMAP) && defined(FEATURE_USB_DEFAULT_MTP)
      if ((aStart->pszArgs != NULL) && 
          STRNCMP(aStart->pszArgs, 
                  AUX_StartUSBDEVSW, 
                  STRLEN(AUX_StartUSBDEVSW)) == 0)
      {
        nErrCode = ISHELL_CreateDialog(pMe->a.m_pIShell, gpAUXResourceFile,
                                       IDD_USBDEV_DIALOG, NULL);
        if(nErrCode != SUCCESS)
        {
          AUX_ERR("Unable to create Dialog %d: %d", IDD_USBDEV_DIALOG, nErrCode, 0);
          return FALSE;
        }
        pMe->m_RootDialogID = IDD_USBDEV_DIALOG;
      }
      else
#endif
      if ((aStart->pszArgs != NULL) && 
          STRNCMP(aStart->pszArgs, 
                  AUX_StartSIOCONFIGURATIONS, 
                  STRLEN(AUX_StartSIOCONFIGURATIONS)) == 0)
      {
        nErrCode = ISHELL_CreateDialog(pMe->a.m_pIShell, gpAUXResourceFile,
                                       IDD_SIOCONFIGURATION, NULL);

        if(nErrCode != SUCCESS)
        {
          AUX_ERR("Unable to create Dialog %d: %d", IDD_SIOCONFIGURATION, nErrCode, 0);
          return FALSE;
        }
        pMe->m_RootDialogID = IDD_SIOCONFIGURATION;

      }
      else if ((aStart->pszArgs != NULL) && 
               STRNCMP(aStart->pszArgs, 
                       AUX_StartSECTIME, 
                       STRLEN(AUX_StartSECTIME)) == 0)
      {
        nErrCode = ISHELL_CreateDialog(pMe->a.m_pIShell, gpAUXResourceFile,
                                       IDD_SETSECURETIME_VIEW, NULL);
        if(nErrCode != SUCCESS)
        {
          AUX_ERR("Unable to create Dialog %d: %d", IDD_SETSECURETIME_VIEW, nErrCode, 0);
          return FALSE;
        }
        pMe->m_RootDialogID = IDD_SETSECURETIME_VIEW;
      }
      else
      {
        nErrCode = ISHELL_CreateDialog(pMe->a.m_pIShell, gpAUXResourceFile,
                                          IDD_AUXSETTINGS, NULL);
        if(nErrCode != SUCCESS)
        {
          AUX_ERR("Unable to create Dialog %d: %d", IDD_AUXSETTINGS, nErrCode, 0);
          return FALSE;
        }
        pMe->m_RootDialogID = IDD_AUXSETTINGS;
      }

#ifdef FEATURE_RUNTIME_DEVMAP
      pMe->m_nCurrSrvc = (unsigned long) RDM_NULL_SRVC;
      pMe->m_pRdmDeviceList = NULL;
#endif
#ifdef FEATURE_WLAN
      {
        /* If WLAN is on, we need to setup our CM registration, regardless
         * of how we were launched.
         */
        uint32 nCMRegMask = NMASK_CM_SS;
  
        /* If we're not online yet, register for Phone events, so that we
         * can detect when we go online.
         */
        if (! AUX_PhoneIsOnline(pMe->a.m_pIShell))
        {
          nCMRegMask |= NMASK_CM_PHONE;
        }
  
        /* Register with Call Manager Notifier to receive notifiers. */
        nErrCode = ISHELL_RegisterNotify(pMe->a.m_pIShell,
                                         AEECLSID_AUXSETTINGS,
                                         AEECLSID_CM_NOTIFIER,
                                         nCMRegMask);
        if (nErrCode != SUCCESS)
        {
          AUX_ERR("AUXSettings failed to register cm for notification 0x%x: %d", 
                  nCMRegMask, nErrCode, 0);
        }
      }
#endif /* FEATURE_WLAN */

      pMe->m_bAppIsRunning = TRUE;
      return TRUE;
    }
    case EVT_DIALOG_INIT:
    {
      AUX_MSG ("Got EVT_DIALOG_INIT", 0, 0, 0);
      AUX_MSG ("Creating Dialog %d", wParam, 0, 0);
      
      for(i=0; i<ARR_SIZE(AUXDlgEventHandlers); i++) 
      {
        if(AUXDlgEventHandlers[i].wDialogID == wParam) 
        {
          IDIALOG_SetEventHandler((IDialog *) dwParam, (*AUXDlgEventHandlers[i].dlgHandler), pMe);
          IDIALOG_SetProperties((IDialog *) dwParam, DLG_HANDLE_ALL_EVENTS);
          return TRUE;
        }
      }
      ERR_FATAL("Dialog Handler not found in AUXSettings_HandleEvent",0,0,0);
    }
    case EVT_APP_STOP:
    {
#ifdef FEATURE_WLAN
      uint32   nCMRegMask = 0;
      int      nErrCode;

      /* If we're currently offline, stay registered for phone events.
       * Otherwise, just unregister entirely from CM notifiers.
       */
      if (! AUX_PhoneIsOnline(pMe->a.m_pIShell))
      {
        nCMRegMask |= NMASK_CM_PHONE;
      }
      nErrCode = ISHELL_RegisterNotify(pMe->a.m_pIShell,
                                       AEECLSID_AUXSETTINGS,
                                       AEECLSID_CM_NOTIFIER,
                                       nCMRegMask);
      if (nErrCode != SUCCESS)
      {
        AUX_ERR("AUXSettings failed to register cm for notification %d: 0x%x", 
                nErrCode, nCMRegMask, 0);
      }
#endif /* FEATURE_WLAN */

      AUX_MSG ("Got EVT_APP_STOP", 0, 0, 0);
      pMe->m_bAppIsRunning = FALSE;
      return TRUE;
    }
    case EVT_APP_SUSPEND:
    {
      AUX_MSG ("Got EVT_APP_SUSPEND", 0, 0, 0);
      return TRUE;
    }
    case EVT_APP_RESUME:
    {
      AUX_MSG ("Got EVT_APP_RESUME", 0, 0, 0);
      return TRUE;
    }
    case EVT_COMMAND:
    {
      AUX_MSG ("Got EVT_COMMAND", 0, 0, 0);
      return TRUE;
    }
    case EVT_NOTIFY:
    { 
#ifdef FEATURE_WLAN
      AEENotify *notif;

      if (dwParam == NULL)
      {
        AUX_ERR("dwParam NULL", 0, 0, 0);
        return FALSE;
      }
      notif = (AEENotify *) dwParam;
      AUX_MSG ("Got EVT_NOTIFY", 0, 0, 0);

      /* Handle shell init notifier. */
      if ((notif->cls == AEECLSID_SHELL) &&
          (notif->dwMask & NMASK_SHELL_INIT))
      {
        /* If we're already online, send the WLAN Subscription Available.
         * Otherwise, register for Phone events so that we can detecte
         * transition to online state.
         */
        if (AUX_PhoneIsOnline(pMe->a.m_pIShell))
        {
          AUXApp_SendWLANSubsAvail(pMe);
        }
        else
        {
          nErrCode = ISHELL_RegisterNotify(pMe->a.m_pIShell,
                                           AEECLSID_AUXSETTINGS,
                                           AEECLSID_CM_NOTIFIER,
                                           NMASK_CM_PHONE);
          if (nErrCode != SUCCESS)
          {
            AUX_ERR("AUXSettings failed to register cm for notification %d: %d", 
                    NMASK_CM_PHONE, nErrCode, 0);
          }
        }

        return TRUE;
      }
      else if ((notif->cls == AEECLSID_CM_NOTIFIER) &&
               ((notif->dwMask & NMASK_CM_PHONE) ||
                (notif->dwMask & NMASK_CM_SS))) 
      {
        return AUXApp_ProcessICMEvents(pMe, wParam, dwParam);
      }
      else
      {
        AUX_ERR("Receive an invalide notification", 0, 0, 0);
        return FALSE;
      }
#endif /*#ifdef FEATURE_WLAN*/
    }
    case EVT_USER:
    {
      AUX_MSG ("Got EVT_USER", 0, 0, 0);
      pIDialog = ISHELL_GetActiveDialog(pMe->a.m_pIShell);
      /* there can be no Active dialog at this point! */

      if (pIDialog) 
      {  /* if there is an existing dialog, pass the event to the dialog */
        wDlgID = IDIALOG_GetID(pIDialog);

        if ((wDlgID >= ARR_SIZE(AUXDlgEventHandlers)) || (0 == wDlgID) ) 
        {  /* error checking */
          AUX_ERR("wDlgID out of bounds %d", wDlgID, 0, 0);
          return FALSE;
        }

        /* find the appropriate event handler */
        for(i = 0; i < ARR_SIZE(AUXDlgEventHandlers); i++) 
        {
          if (AUXDlgEventHandlers[i].wDialogID == wDlgID) 
          {
            if ((*AUXDlgEventHandlers[i].dlgHandler)(pMe, eCode, wParam, dwParam)) 
            {
              /* event handled by dialog event handler */
              return TRUE;
            }
          }
        }
      }
      break;
    }
    case EVT_KEY:
    {
      AUX_MSG ("Got EVT_KEY", 0, 0, 0);
      return TRUE;
    }
    default:
    {
      break;
    }
  }
  
  return FALSE;
  
}  /* End AUXSettings_HandleEvent */



/*===========================================================================

FUNCTION: AEEClsCreateInstance

DESCRIPTION
  This function is invoked while the app is being loaded. All Modules must provide this
  function. Ensure to retain the same name and parameters for this function.
  In here, the module must verify the ClassID and then invoke the AEEApplet_New() function
  that has been provided in AEEAppGen.c.

   After invoking AEEApplet_New(), this function can do app specific initialization. In this
   example, a generic structure is provided so that app developers need not change app specific
   initialization section every time except for a call to InitAppData(). This is done as follows:
   InitAppData() is called to initialize AppletData instance. It is app developers
   responsibility to fill-in app data initialization code of InitAppData(). App developer
   is also responsible to release memory allocated for data contained in AppletData -- this can be
   done in FreeAppData().

PROTOTYPE:
  int AEEAppCreateInstance(AEECLSID clsID, IShell* pIShell, IModule* pIModule,IApplet** ppApplet)

PARAMETERS:
  clsID: [in]: Specifies the ClassID of the applet which is being loaded

  pIShell: [in]: Contains pointer to the IShell interface.

  pIModule: pin]: Contains pointer to the IModule interface to the current module to which
  this app belongs

  ppApplet: [out]: On return, *ppApplet must point to a valid AEEApplet structure. Allocation
  of memory for this structure and initializing the base data members is done by AEEApplet_New().

DEPENDENCIES
  none

RETURN VALUE
  SUCCESS: If the app needs to be loaded and if AEEApplet_New() invocation was successful
  EFAILED: If the app does not need to be loaded or if errors occurred in AEEApplet_New().
  If this function returns FALSE, the app will not be loaded.

SIDE EFFECTS
  none
===========================================================================*/
#if defined(AEE_STATIC)
int AUXSettings_CreateInstance(AEECLSID ClsId,IShell * pIShell,IModule * pMod,void ** ppObj)
#else
int AEEClsCreateInstance(AEECLSID ClsId,IShell * pIShell,IModule * pMod,void ** ppObj)
#endif
{
   *ppObj = NULL;

   if(AEEApplet_New( sizeof(AUXApp),                    /* Size of our private class */
                     ClsId,                               /* Our class ID */
                     pIShell,                             /* Shell interface */
                     pMod,                                /* Module instance */
                     (IApplet**)ppObj,                    /* Return object */
                     (AEEHANDLER)AUXSettings_HandleEvent,     /* Our event handler */
                     (PFNFREEAPPDATA)AUXSettings_FreeAppData))/* Cleanup function */
   {
     if(AUXSettings_InitAppData((IApplet*)*ppObj)) {
       return(AEE_SUCCESS);
     }
     //Release the applet. This will free the memory allocated for the applet when
     (void)IAPPLET_Release(((IApplet*)*ppObj));
   }
  return (EFAILED);
} /* End AUXSettings_CreateInstance / AEEClsCreateInstance */



#ifdef AEE_STATIC
/*=============================================================================
FUNCTION: AUXSettings_Load

DESCRIPTION:
    This function loads the application.

PROTOTYPE:
static int AUXSettings_Load(IShell *ps, void * pHelpers, IModule ** pMod)

PARAMETERS:
--> ps: Pointer to ISHELL interface
--> pHelpers:
--> pMod:

RETURN VALUE:


COMMENTS:


SIDE EFFECTS:
none

SEE ALSO:
none
=============================================================================*/
int AUXSettings_Load(IShell *ps, void * pHelpers, IModule ** pMod)
{
   return(AEEStaticMod_New((int16)(sizeof(AEEMod)),ps,pHelpers,pMod,
          AUXSettings_CreateInstance,NULL));
} /* End AUXSettings_Load */



/*=============================================================================
FUNCTION: AUXSettings_GetModInfo

DESCRIPTION:
    Gets the AUXSettings Info since it is a static application.

PROTOTYPE:
    PFNMODENTRY AUXSettings_GetModInfo(IShell * ps, AEECLSID ** ppClasses,
                                 AEEAppInfo ** pApps, uint16 * pnApps,
                   uint16 * pwMinPriv)

PARAMETERS:
--> ps: Pointer to ISHELL interface
--> ppClasses:
--> pApps:
--> pnApps:
--> pwMinPriv:

RETURN VALUE:
 A pointer to the function to load the Application.

COMMENTS:


SIDE EFFECTS:
none

SEE ALSO:
none
=============================================================================*/
PFNMODENTRY AUXSettings_GetModInfo(IShell * ps, AEECLSID ** ppClasses,
                               AEEAppInfo ** pApps,
                               uint16 * pnApps,uint16 * pwMinPriv)
{
   *pApps = (AEEAppInfo *)&gaiAUXSettings;
   *pnApps = 1;
   *pwMinPriv = PL_SYSTEM;   /* AUXSettings has system priviliges */
   return((PFNMODENTRY)AUXSettings_Load);
} /* End AUXSettings_GetModInfo */

#endif  /* AEE_STATIC */

#ifdef FEATURE_WLAN
/*=============================================================================
FUNCTION: AUXApp_SendWLANSubsAvail

DESCRIPTION:
  Sends the WLAN Subscription Available command to ICM

SIDE EFFECTS:
  None.

=============================================================================*/
void AUXApp_SendWLANSubsAvail(AUXApp *pMe)
{
  int     nReturnStatus;

  if ((pMe == NULL) || (pMe->m_WLAN.m_pICM == NULL))
  {
    // Print pMe so we know which pointer was NULL.
    AUX_ERR("NULL pointer. pMe=0x%x", pMe, 0, 0);
    return;
  }

  nReturnStatus = ICM_SetSubscriptionStatus(pMe->m_WLAN.m_pICM,
                                            AEECM_SYS_MODE_WLAN,
                                            TRUE);
  if (nReturnStatus != AEE_SUCCESS)
  {
    AUX_ERR("Failed to change WLAN subs, %d.", nReturnStatus, 0, 0);
  }
}

/*=============================================================================
FUNCTION: AUXApp_ProcessICMEvents

DESCRIPTION:
  Process ICM Notifier events

SIDE EFFECTS:
  None.

=============================================================================*/
boolean AUXApp_ProcessICMEvents(AUXApp *pMe, uint16 wParam, uint32 dwParam)
{
  AEECMNotifyInfo *event_info;
  
  if (dwParam == NULL)
  {
    AUX_ERR("dwParam NULL", 0, 0, 0);
    return FALSE;
  }
  
  event_info = (((AEENotify *) dwParam)->pData);
  if (event_info == NULL)
  {
    AUX_ERR("event_info NULL", 0, 0, 0);
    return FALSE;
  }

  AUX_MSG("AUXSettings: received ICM event. Mask 0x%x",
          ((AEENotify*)dwParam)->dwMask, 0, 0);

  switch (((AEENotify*)dwParam)->dwMask)
  {
    case NMASK_CM_SS:

      AUX_MSG("AUXApp: Got ICM SS event 0x%x", event_info->event, 0, 0);
      switch (event_info->event)
      {
        case AEECM_EVENT_SS_SRV_CHANGED:
          AUX_MSG("Service Status = %d",
                   event_info->event_data.ss.ss_info.srv_status, 0, 0);
          CheckWLANAutoScanNewAssociation(pMe);
          break;

        default: 
          break;
      }
      return TRUE;
    
    case NMASK_CM_PHONE:
      AUX_MSG("AUXSettings: Got ICM Phone event 0x%x", event_info->event, 0, 0);
      switch (event_info->event)
      {
        case AEECM_EVENT_PH_OPRT_MODE:  /* Operating mode was changed */
          AUX_MSG("Phone operating mode = %d",
                  event_info->event_data.ph.oprt_mode, 0, 0);
          if (event_info->event_data.ph.oprt_mode == AEECM_OPRT_MODE_ONLINE)
          {
            int     nReturnStatus;
            uint32  nCMRegMask = 0;

            AUXApp_SendWLANSubsAvail(pMe);

            if (TRUE == pMe->m_bAppIsRunning)
            {
              nCMRegMask |= NMASK_CM_SS;
            }
            nReturnStatus = ISHELL_RegisterNotify(pMe->a.m_pIShell,
                                                  AEECLSID_AUXSETTINGS,
                                                  AEECLSID_CM_NOTIFIER,
                                                  nCMRegMask);
            if (nReturnStatus != SUCCESS)
            {
              AUX_ERR("AUXSettings failed to register for CM notification 0x%x: %d", 
                      nCMRegMask, nReturnStatus, 0);
            }
          }
          break;

        default:
          break;
      }
      return TRUE;

    default:
      break;
  }
  return FALSE;
}
#endif /*#ifdef FEATURE_WLAN*/
