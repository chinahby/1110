/*===========================================================================

FILE: AUXMenu.c

DESCRIPTION
   This file contains the AUX Configurations applet's menu related functions.

PUBLIC CLASSES:
   N/A

INITIALIZATION AND SEQUENCING REQUIREMENTS:
   Functions in this file is being called by AUXApp.c.

       Copyright   2007 QUALCOMM Incorporated.
                    All Rights Reserved.
                   QUALCOMM Proprietary/GTDR
===========================================================================*/
/*===========================================================================
                        EDIT HISTORY FOR MODULE

when       who     what, where, why
--------   ---     ----------------------------------------------------------
10/29/07   sg      Support for setting the BREW Secure clock
10/01/07   cvs     Remove AEECMPhInfo from stack
09/24/07   snb     Provide option to exit from USB MTP mode.
04/04/07   ypmw    Add CGPS feature definition
03/26/07   ypmw    Add EAP authentication scheme for automatic wlan mode
01/29/07   ypmw    Fix compiler and lint warnings
01/23/07   ypmw    Move WLAN application from Core App to AUX Settings
12/12/06   RT      Fixed compile warning in AUXStripFeatureMenuItem
12/11/06   RT      Ported support for FEATURE_DATA_MULTI_PORTS
10/31/06   RT      Initial revision
===========================================================================*/


/*===============================================================================

                INCLUDES AND VARIABLE DEFINITIONS

=============================================================================== */
#include "OEMFeatures.h"
#include "AUXMenu.h"
#include "AEEStdLib.h"    /* contain FREE function */
#include "AEEAppGen.h"    /* Applet helper file */
#include "AEEMenu.h"
#include "AEEShell.h"
#include "BREWVersion.h"

#include "AppComFunc.h"
#include "AUXUtils.h"

#ifdef FEATURE_WLAN
#include "AUXWLANMenu.h"
#include "AUXWLANUtils.h"
#endif /* FEATURE_WLAN */

#if defined(FEATURE_RUNTIME_DEVMAP) && defined(FEATURE_USB_DEFAULT_MTP)
/* USB Device Switching */
#include "usbdc_api.h"
#endif /* FEATURE_RUNTIME_DEVMAP && FEATURE_USB_DEFAULT_MTP */

#include "AUXSettings.bid"
#include "auxsettings.brh"

// DMSS includes
#include "sdevmenu.h"
#include "sdevmap.h"
#include "rdevmap.h"
#include "mobile.h"

#include "ui_base.h"
#include "uiutils.h"

#include "AEEDate.h"
#include "AEETime.h"

#define AUX_TEXT_BUFFER_SIZE  400   // Size of buffer used to hold text strings
#define AUX_TITLE_BUFFER_SIZE  100
#define AUX_MAX_STR_SIZE 50

/* Valid action types */
#define AUX_LAUNCH_DIALOG 0
#define AUX_MENU_SETTING  1

#define AUX_SECURETIMEDONE 0
#define AUX_SECUREDATEDONE 1

typedef uint64 (GetFnPtrType)(AUXApp*);
typedef boolean (SetFnPtrType)(AUXApp*, uint64);


/*=============================================================================

                       DATA STRUCTURE

=============================================================================*/

typedef struct {
  uint16 wDialogID;     /* Current dialog ID */
  uint16 wControlID;    /* Control ID in current dialog */
  uint16 wItemID;       /* Item ID in the current control */
  uint16 wActionType;   /* Type of action on selection */
  uint64 nActionData;   /* DlgID, AppletID or Setting value depending on action type */
  /* The following two parameters should be replaced by config item ID after moving to IConfig */
  GetFnPtrType   *getfnPtr;     /* Function pointer for get method - valid for menu settings */
  SetFnPtrType   *setfnPtr;     /* Function pointer for set method - valid for menu settings */
} MenuDefinitionType;

static const MenuDefinitionType AUXMenuActionTbl[] = {
  
  /* AUX Settings */
  {IDD_AUXSETTINGS, IDC_MENU_AUXSETTINGS, IDL_SIOCONFIGURATION, AUX_LAUNCH_DIALOG, IDD_SIOCONFIGURATION, NULL, NULL},
  {IDD_AUXSETTINGS, IDC_MENU_AUXSETTINGS, IDL_USBDEVICESWITCH, AUX_LAUNCH_DIALOG, IDD_USBDEV_DIALOG, NULL, NULL},
  {IDD_AUXSETTINGS, IDC_MENU_AUXSETTINGS, IDL_SETSECURETIME, AUX_LAUNCH_DIALOG, IDD_SETSECURETIME_VIEW, NULL, NULL},
  {IDD_AUXSETTINGS, IDC_MENU_AUXSETTINGS, IDL_WLAN, AUX_LAUNCH_DIALOG, IDD_WLAN_SETTINGS, NULL, NULL},

  /* SIO Configuration */
  {IDD_SIOCONFIGURATION, IDC_MENU_SIOCONFIGURATION, IDL_PORTMAP, AUX_LAUNCH_DIALOG, IDD_PORTMAP, NULL, NULL},
  {IDD_SIOCONFIGURATION, IDC_MENU_SIOCONFIGURATION, IDL_DSBAUD, AUX_LAUNCH_DIALOG, IDD_DATA_BAUD, NULL, NULL},
  {IDD_SIOCONFIGURATION, IDC_MENU_SIOCONFIGURATION, IDL_DIAGBAUD, AUX_LAUNCH_DIALOG, IDD_DIAG_BAUD, NULL, NULL},

#ifdef FEATURE_WLAN
   /* WLAN Network Settings */
   {IDD_WLAN_SETTINGS, IDC_WLAN_SETTINGS, IDL_SSID, AUX_LAUNCH_DIALOG,
    IDD_SSID_VIEW, NULL, NULL},
   {IDD_WLAN_SETTINGS, IDC_WLAN_SETTINGS, IDL_CHANNEL, AUX_LAUNCH_DIALOG,
    IDD_CHANNEL_VIEW, NULL, NULL},
   {IDD_WLAN_SETTINGS, IDC_WLAN_SETTINGS, IDL_NW_SEL_MODE, AUX_LAUNCH_DIALOG,
    IDD_WLAN_SELECT_NW_SEL_MODE, NULL, NULL},
   /* IDL_WLAN_GENERIC_SCAN is handled explicitly in the dlg handler */
   {IDD_WLAN_SETTINGS, IDC_WLAN_SETTINGS, IDL_WLAN_STATUS, AUX_LAUNCH_DIALOG,
    IDD_WLAN_STATUS, NULL, NULL},
   {IDD_WLAN_SETTINGS, IDC_WLAN_SETTINGS, IDL_WLAN_SECURITY,
    AUX_LAUNCH_DIALOG, IDD_WLAN_SECURITY, NULL, NULL},
   {IDD_WLAN_SETTINGS, IDC_WLAN_SETTINGS, IDL_WLAN_GENERAL_CONFIG, AUX_LAUNCH_DIALOG,
    IDD_WLAN_GENERAL_CONFIG, NULL, NULL},
   {IDD_WLAN_SETTINGS, IDC_WLAN_SETTINGS, IDL_WLAN_MODE_PROFILE, AUX_LAUNCH_DIALOG,
    IDD_WLAN_SELECT_MODE_PROFILE, NULL, NULL},
   {IDD_WLAN_SETTINGS, IDC_WLAN_SETTINGS, IDL_WLAN_START_ADHOC_NETWORK, AUX_LAUNCH_DIALOG,
    IDD_WLAN_START_NEW_ADHOC_NETWORK, NULL, NULL},

   /*  WLAN General Config  */
   {IDD_WLAN_GENERAL_CONFIG, IDC_WLAN_GENERAL_CONFIG, IDL_WLAN_MODE, AUX_LAUNCH_DIALOG,
    IDD_WLAN_SELECT_MODE, NULL, NULL},
   {IDD_WLAN_GENERAL_CONFIG, IDC_WLAN_GENERAL_CONFIG, IDL_WLAN_PREAMBLE, AUX_LAUNCH_DIALOG,
    IDD_WLAN_SELECT_PREAMBLE, NULL, NULL},
   {IDD_WLAN_GENERAL_CONFIG, IDC_WLAN_GENERAL_CONFIG, IDL_WLAN_LISTEN_INTERVAL,
    AUX_LAUNCH_DIALOG, IDD_WLAN_LISTEN_INTERVAL_VIEW, NULL, NULL},
   {IDD_WLAN_GENERAL_CONFIG, IDC_WLAN_GENERAL_CONFIG, IDL_WLAN_IP_CONFIG,
    AUX_LAUNCH_DIALOG, IDD_WLAN_IP_CONFIG, NULL, NULL},
   {IDD_WLAN_GENERAL_CONFIG, IDC_WLAN_GENERAL_CONFIG, IDL_WLAN_RTS_THRESHOLD,
    AUX_LAUNCH_DIALOG, IDD_WLAN_RTS_VIEW, NULL, NULL},
   {IDD_WLAN_GENERAL_CONFIG, IDC_WLAN_GENERAL_CONFIG, IDL_WLAN_SCAN_TIME,
    AUX_LAUNCH_DIALOG, IDD_WLAN_SCAN_TIME_VIEW, NULL, NULL},
   {IDD_WLAN_GENERAL_CONFIG, IDC_WLAN_GENERAL_CONFIG, IDL_WLAN_FRAGMENT,
    AUX_LAUNCH_DIALOG, IDD_WLAN_FRAG_VIEW, NULL, NULL},
   {IDD_WLAN_GENERAL_CONFIG, IDC_WLAN_GENERAL_CONFIG, IDL_WLAN_MAX_TX_POWER,
    AUX_LAUNCH_DIALOG, IDD_WLAN_MAX_TX_POWER_VIEW, NULL, NULL},
   {IDD_WLAN_GENERAL_CONFIG, IDC_WLAN_GENERAL_CONFIG, IDL_WLAN_SCAN_MODE, AUX_LAUNCH_DIALOG,
    IDD_WLAN_SCAN_MODE, NULL, NULL},

  /* WLAN Security Menu - We don't use the standard menu dialog
   * handlers, but these table entries allow us to use the
   * common menu manipulation routines (e.g. selecting the
   * right radio button).  As a result, there is no need for a
   * "set" routine.
   */
  {IDD_WLAN_SECURITY, IDC_MENU_WLAN_SECURITY, IDL_SECURITY_NONE,
   AUX_MENU_SETTING, IDL_SECURITY_NONE, GetWLANSecItem, NULL },
  {IDD_WLAN_SECURITY, IDC_MENU_WLAN_SECURITY, IDL_SECURITY_WEP64,
   AUX_MENU_SETTING, IDL_SECURITY_WEP64, GetWLANSecItem, NULL },
  {IDD_WLAN_SECURITY, IDC_MENU_WLAN_SECURITY, IDL_SECURITY_WEP128,
   AUX_MENU_SETTING, IDL_SECURITY_WEP128, GetWLANSecItem, NULL },
  {IDD_WLAN_SECURITY, IDC_MENU_WLAN_SECURITY, IDL_SECURITY_WPA_PASSWORD,
   AUX_MENU_SETTING, IDL_SECURITY_WPA_PASSWORD, GetWLANSecItem, NULL },
  {IDD_WLAN_SECURITY, IDC_MENU_WLAN_SECURITY, IDL_SECURITY_WPA_MD5,
   AUX_MENU_SETTING, IDL_SECURITY_WPA_MD5, GetWLANSecItem, NULL },
  {IDD_WLAN_SECURITY, IDC_MENU_WLAN_SECURITY, IDL_SECURITY_WPA_TLS_CERT,
   AUX_MENU_SETTING, IDL_SECURITY_WPA_TLS_CERT, GetWLANSecItem, NULL },
  {IDD_WLAN_SECURITY, IDC_MENU_WLAN_SECURITY, IDL_SECURITY_WPA_EAP,
   AUX_MENU_SETTING, IDL_SECURITY_WPA_EAP, GetWLANSecItem, NULL },


  /* WLAN IP Config Menu */
  {IDD_WLAN_IP_CONFIG, IDC_MENU_LAN, IDC_DHCP, AUX_MENU_SETTING,
   TRUE, GetDhcp, SetDhcp},
  {IDD_WLAN_IP_CONFIG, IDC_MENU_LAN, IDC_IP, AUX_MENU_SETTING,
   FALSE, GetDhcp, SetDhcp},

  /* WLAN Mode Menu Global*/
  {IDD_WLAN_SELECT_MODE, IDC_MENU_WLAN_MODE, IDL_WLAN_ADHOC, AUX_MENU_SETTING,
   AEECM_WLAN_BSS_TYPE_ADHOC, GetWLANMode, SetWLANMode},
  {IDD_WLAN_SELECT_MODE, IDC_MENU_WLAN_MODE, IDL_WLAN_INFRA, AUX_MENU_SETTING,
   AEECM_WLAN_BSS_TYPE_INFRA, GetWLANMode, SetWLANMode},
  {IDD_WLAN_SELECT_MODE, IDC_MENU_WLAN_MODE, IDL_WLAN_ANY_MODE, AUX_MENU_SETTING,
   AEECM_WLAN_BSS_TYPE_ANY, GetWLANMode, SetWLANMode},

  /* WLAN Preamble Length Menu */
  {IDD_WLAN_SELECT_PREAMBLE, IDC_MENU_WLAN_PREAMBLE, IDL_WLAN_PREAMBLE_AUTO,
   AUX_MENU_SETTING, AEEWIFI_PREAMBLE_AUTO, GetWLANPreamble, SetWLANPreamble},
  {IDD_WLAN_SELECT_PREAMBLE, IDC_MENU_WLAN_PREAMBLE, IDL_WLAN_PREAMBLE_SHORT,
   AUX_MENU_SETTING, AEEWIFI_PREAMBLE_SHORT, GetWLANPreamble, SetWLANPreamble},
  {IDD_WLAN_SELECT_PREAMBLE, IDC_MENU_WLAN_PREAMBLE, IDL_WLAN_PREAMBLE_LONG,
   AUX_MENU_SETTING, AEEWIFI_PREAMBLE_LONG, GetWLANPreamble, SetWLANPreamble},

  /* WLAN Network Selection Mode Menu */
  {IDD_WLAN_SELECT_NW_SEL_MODE, IDC_MENU_NW_SEL_MODE, IDL_WLAN_NW_SEL_AUTO,
   AUX_MENU_SETTING, AEECM_NETWORK_SEL_MODE_PREF_AUTOMATIC,
   GetWLANNwSelMode, SetWLANNwSelMode},
  {IDD_WLAN_SELECT_NW_SEL_MODE, IDC_MENU_NW_SEL_MODE, IDL_WLAN_NW_SEL_MANUAL,
   AUX_MENU_SETTING, AEECM_NETWORK_SEL_MODE_PREF_MANUAL,
   GetWLANNwSelMode, SetWLANNwSelMode},

  /* WLAN Scan Mode Menu */
  {IDD_WLAN_SCAN_MODE, IDC_MENU_WLAN_SCAN_MODE_SELECT, IDL_WLAN_SCAN_MODE_ACTIVE,
   AUX_MENU_SETTING, AEECM_WLAN_SCAN_PREF_ACTIVE, GetWLANScanMode, SetWLANScanMode},
  {IDD_WLAN_SCAN_MODE, IDC_MENU_WLAN_SCAN_MODE_SELECT, IDL_WLAN_SCAN_MODE_PASSIVE,
   AUX_MENU_SETTING, AEECM_WLAN_SCAN_PREF_PASSIVE, GetWLANScanMode, SetWLANScanMode},
  {IDD_WLAN_SCAN_MODE, IDC_MENU_WLAN_SCAN_MODE_SELECT, IDL_WLAN_SCAN_MODE_AUTO,
   AUX_MENU_SETTING, AEECM_WLAN_SCAN_PREF_AUTO, GetWLANScanMode, SetWLANScanMode},

  /* WLAN Mode Menu Profile */
  {IDD_WLAN_SELECT_MODE_PROFILE, IDC_MENU_WLAN_MODE_PROFILE, IDL_WLAN_ADHOC_PROFILE, AUX_MENU_SETTING,
   AEEWIFI_MODE_ADHOC, GetWLANModeProfile, SetWLANModeProfile},
  {IDD_WLAN_SELECT_MODE_PROFILE, IDC_MENU_WLAN_MODE_PROFILE, IDL_WLAN_INFRA_PROFILE, AUX_MENU_SETTING,
   AEEWIFI_MODE_INFRA, GetWLANModeProfile, SetWLANModeProfile},
  {IDD_WLAN_SELECT_MODE_PROFILE, IDC_MENU_WLAN_MODE_PROFILE, IDL_WLAN_ANY_MODE_PROFILE, AUX_MENU_SETTING,
   AEEWIFI_MODE_ANY, GetWLANModeProfile, SetWLANModeProfile},
#endif // FEATURE_WLAN

#if defined(FEATURE_RUNTIME_DEVMAP) && defined(FEATURE_USB_DEFAULT_MTP)
  /* USB Device Switching */
  {IDD_USBDEV_DIALOG, IDC_USBDEV_MENU, IDL_USBDEV_MODEM, 
    AUX_MENU_SETTING, USBDC_MODEM_DEVICE, AUXGetUSBDevice, AUXSetUSBDevice},
  {IDD_USBDEV_DIALOG, IDC_USBDEV_MENU, IDL_USBDEV_NETWORK, 
    AUX_MENU_SETTING, USBDC_NETWORK_DEVICE, AUXGetUSBDevice, AUXSetUSBDevice},
  {IDD_USBDEV_DIALOG, IDC_USBDEV_MENU, IDL_USBDEV_MTP, 
    AUX_MENU_SETTING, USBDC_MTP_DEVICE, AUXGetUSBDevice, AUXSetUSBDevice},
#endif /* FEATURE_RUNTIME_DEVMAP && FEATURE_USB_DEFAULT_MTP */
};

typedef struct {
   uint16 wDialogID;
   uint16 wEditDialogID;
} AUXEditCtlIDTable;

static const AUXEditCtlIDTable AUXLookupEditDlgID[] = {
#ifdef FEATURE_WLAN
  { IDD_SSID_VIEW,                   IDD_ENTER_SSID                  },
  { IDD_CHANNEL_VIEW,                IDD_ENTER_CHANNEL               },
  { IDD_WLAN_LISTEN_INTERVAL_VIEW,   IDD_WLAN_LISTEN_INTERVAL_EDIT   },
  { IDD_WLAN_RTS_VIEW,               IDD_WLAN_RTS_EDIT               },
  { IDD_WLAN_SCAN_TIME_VIEW,         IDD_WLAN_SCAN_TIME_EDIT         },
  { IDD_WLAN_FRAG_VIEW,              IDD_WLAN_FRAG_EDIT              },
  { IDD_WLAN_MAX_TX_POWER_VIEW,      IDD_WLAN_MAX_TX_POWER_EDIT      },
#endif /*#ifdef FEATURE_WLAN*/
  { IDD_SETSECURETIME_VIEW,          IDD_SETSECURETIME_EDIT},
};

typedef struct {
   uint16 wDialogID;
   uint16 wDialogTitle;
   uint16 wCtrlID;
   uint16 wSoftkeyID;
} AUXStaticDlgDefinitionType;

static const AUXStaticDlgDefinitionType AUXStaticDlgs[] = {
#ifdef FEATURE_WLAN
  { IDD_SSID_VIEW, IDS_SSID, IDC_SSID_VIEW_STATIC, IDC_SSID_VIEW_SK },
  { IDD_CHANNEL_VIEW, IDS_CHANNEL_TITLE, IDC_STATIC_CHANNEL_VIEW,
      IDC_SK_CHANNEL_VIEW},
  { IDD_WLAN_LISTEN_INTERVAL_VIEW, IDS_WLAN_LISTEN_INTERVAL,
      IDC_STATIC_WLAN_LISTEN_VIEW, IDC_SK_WLAN_LISTEN_VIEW },
  { IDD_WLAN_RTS_VIEW, IDS_WLAN_RTS_THRESHOLD, IDC_STATIC_WLAN_RTS_VIEW,
      IDC_SK_WLAN_RTS_VIEW },
  { IDD_WLAN_SCAN_TIME_VIEW, IDS_WLAN_SCAN_TIME,
      IDC_STATIC_WLAN_SCAN_TIME_VIEW, IDC_SK_WLAN_SCAN_TIME_VIEW },
  { IDD_WLAN_FRAG_VIEW, IDS_WLAN_FRAGMENT, IDC_STATIC_WLAN_FRAG_VIEW,
      IDC_SK_WLAN_FRAG_VIEW },
  { IDD_WLAN_MAX_TX_POWER_VIEW, IDS_WLAN_MAX_TX_POWER,
    IDC_STATIC_WLAN_MAX_TX_POWER_VIEW, IDC_SK_WLAN_MAX_TX_POWER_VIEW },
  { IDD_WLAN_PERFORM_MANUAL_SCAN_NOW, IDS_WLAN_PERFORM_NETWORK_MANUAL_SCAN_QUESTION,
    IDC_STATIC_WLAN_PERFORM_MANUAL_SCAN_NOW, IDC_SK_WLAN_PERFORM_MANUAL_SCAN_NOW },
  { IDD_WLAN_START_NEW_ADHOC_NETWORK, IDS_WLAN_START_NEW_ADHOC_QUESTION,
    IDC_STATIC_WLAN_START_NEW_ADHOC_NETWORK, IDC_SK_WLAN_START_NEW_ADHOC_NETWORK },
#endif /*#ifdef FEATURE_WLAN*/
  { IDD_SETSECURETIME_VIEW, IDS_TIMEDATEVIEW, IDC_STATIC_SETSECURETIME_VIEW, IDC_SK_SETSECURETIME_VIEW },
};

/*===========================================================================
FUNCTION AUXGetDlgMenuCtlID

DESCRIPTION
  This function matches the dialog ID with a corresponding menu control ID

DEPENDENCIES
  Note: 1. This function should be called in a Menu dialog ONLY
        2. the pIActiveDialog should be test for null pointer in the calling
           function

SIDE EFFECTS
  None
===========================================================================*/
uint16 AUXGetDlgMenuCtlID(AUXApp *pMe, IDialog* pIActiveDialog)
{
  uint16 wDlgID;
  int i;

  if  ( pIActiveDialog == NULL ) {
    return 0;
  }

  wDlgID = IDIALOG_GetID(pIActiveDialog);

  for (i = 0; i< ARR_SIZE(AUXMenuActionTbl); i++) {
    if (AUXMenuActionTbl[i].wDialogID == wDlgID) {
      return AUXMenuActionTbl[i].wControlID;
    }
  }
  AUX_ERR( "AUXGetDlgMenuCtlID(): No matching menu ctl", 0, 0, 0);
  return 0; /* error */
} /* End AUXGetDlgMenuCtlID */

/*===========================================================================
FUNCTION AUXStripFeatureMenuItem

DESCRIPTION
  update the menu to reflect the correspondings under the FEATURE flag

DEPENDENCIES
  None

SIDE EFFECTS
  None
===========================================================================*/
void AUXStripFeatureMenuItem(AUXApp * pMe, IMenuCtl* pICurrentMenu, uint16 wCtrlID)
{
  boolean bReturnStatus;
  CtlAddItem pai;

  switch (wCtrlID)
  {
  case IDC_MENU_SIOCONFIGURATION:
#if !defined (FEATURE_RUNTIME_DEVMAP)
    bReturnStatus = IMENUCTL_DeleteItem(pICurrentMenu, IDL_PORTMAP);
    if (bReturnStatus == FALSE) {
      AUX_ERR ("Failed to delete menu item", 0,0,0);
    }
#endif /* !defined (FEATURE_RUNTIME_DEVMAP)*/

    break;


  /* Port Map runtime menu */
#ifdef FEATURE_RUNTIME_DEVMAP
  case IDC_MENU_PORTMAP:
    /* update the data for the menu */
    (void) MEMSET(&pai, 0, sizeof(pai));
    pai.pszResImage = gpAUXResourceFile;

    /* diag */
    pai.dwData = RDM_DIAG_SRVC;
    bReturnStatus = IMENUCTL_SetItem(pICurrentMenu, IDL_PORTMAP_DIAG, MSIF_DATA, &pai);
    if (bReturnStatus == FALSE) {
      AUX_ERR ("Failed to set menu item", 0,0,0);
    }
    /* data */
#if !defined(FEATURE_DS) && !defined(FEATURE_DATA)
    bReturnStatus = IMENUCTL_DeleteItem(pICurrentMenu, IDL_PORTMAP_DATA);
    if (bReturnStatus == FALSE) {
      AUX_ERR ("Failed to delete menu item", 0,0,0);
    }
#else /*!defined(FEATURE_DS) && !defined(FEATURE_DATA)*/
    pai.dwData = RDM_DATA_SRVC;
    bReturnStatus = IMENUCTL_SetItem(pICurrentMenu, IDL_PORTMAP_DATA, MSIF_DATA, &pai);
    if (bReturnStatus == FALSE) {
      AUX_ERR ("Failed to set menu item", 0,0,0);
    }
#endif /*!defined(FEATURE_DS) && !defined(FEATURE_DATA)*/

    /* rpc */
#ifndef FEATURE_ONCRPC
    bReturnStatus = IMENUCTL_DeleteItem(pICurrentMenu, IDL_PORTMAP_RPC);
    if (bReturnStatus == FALSE) {
      AUX_ERR ("Failed to delete menu item", 0,0,0);
    }
#else /*!FEATURE_ONCRPC*/
    pai.dwData = RDM_RPC_SRVC;
    bReturnStatus = IMENUCTL_SetItem(pICurrentMenu, IDL_PORTMAP_RPC, MSIF_DATA, &pai);
    if (bReturnStatus == FALSE) {
      AUX_ERR ("Failed to set menu item", 0,0,0);
    }
#endif /*!FEATURE_ONCRPC*/

    /* bt */
#ifndef FEATURE_BT
    bReturnStatus = IMENUCTL_DeleteItem(pICurrentMenu, IDL_PORTMAP_BT);
    if (bReturnStatus == FALSE) {
      AUX_ERR ("Failed to delete menu item", 0,0,0);
    }
#else /*!FEATURE_BT*/
    pai.dwData = RDM_BT_HCI_SRVC;
    bReturnStatus = IMENUCTL_SetItem(pICurrentMenu, IDL_PORTMAP_BT, MSIF_DATA, &pai);
    if (bReturnStatus == FALSE) {
      AUX_ERR ("Failed to set menu item", 0,0,0);
    }
#endif /*!FEATURE_BT*/

    /* gpsOne */
#if defined( FEATURE_GPSONE_PE ) || defined( FEATURE_MMGPS ) || defined(FEATURE_CGPS)
    pai.dwData = RDM_NMEA_SRVC;
    bReturnStatus = IMENUCTL_SetItem(pICurrentMenu, IDL_PORTMAP_GPS, MSIF_DATA, &pai);
    if (bReturnStatus == FALSE) {
      AUX_ERR ("Failed to set menu item", 0,0,0);
    }
#else /* FEATURE_GPSONE_PE || FEATURE_MMGPS || FEATURE_CGPS*/
    bReturnStatus = IMENUCTL_DeleteItem(pICurrentMenu, IDL_PORTMAP_GPS);
    if (bReturnStatus == FALSE) {
      AUX_ERR ("Failed to delete menu item", 0,0,0);
    }
#endif /* FEATURE_GPSONE_PE || FEATURE_MMGPS || FEATURE_CGPS*/

    break;  /* end for Port map menu */
#endif /* FEATURE_RUNTIME_DEVMAP */

#if defined(FEATURE_RUNTIME_DEVMAP) && defined(FEATURE_USB_DEFAULT_MTP)
  case IDC_USBDEV_MENU:
#ifndef FEATURE_DATA_RM_NET_DEFAULT_DEVICE
    if (!usbdc_api_device_switch_valid(USBDC_MODEM_DEVICE))
#endif
    {
      if (!IMENUCTL_DeleteItem(pICurrentMenu, IDL_USBDEV_MODEM))
      {
        AUX_ERR ("Failed to delete menu item", 0,0,0);
      }
    }    
#if defined(FEATURE_DATA_RM_NET) && defined(FEATURE_DATA_RM_NET_DEFAULT_DEVICE)
    if (!usbdc_api_device_switch_valid(USBDC_NETWORK_DEVICE))
#endif
    {
      if (!IMENUCTL_DeleteItem(pICurrentMenu, IDL_USBDEV_NETWORK)) 
      {
        AUX_ERR ("Failed to delete menu item", 0,0,0);
      }    
    }
    if (!usbdc_api_device_switch_valid(USBDC_MTP_DEVICE))
    {
      if (!IMENUCTL_DeleteItem(pICurrentMenu, IDL_USBDEV_MTP))
      {
        AUX_ERR ("Failed to delete menu item", 0,0,0);
      }
    }        
    break;
#endif /* FEATURE_RUNTIME_DEVMAP && FEATURE_USB_DEFAULT_MTP */

  case IDC_MENU_AUXSETTINGS:
#if !defined(FEATURE_RUNTIME_DEVMAP) || !defined(FEATURE_USB_DEFAULT_MTP)
    bReturnStatus = IMENUCTL_DeleteItem(pICurrentMenu, IDL_USBDEVICESWITCH);
    if (bReturnStatus == FALSE) {
      AUX_ERR ("Failed to delete menu USB Device Switch item", 0,0,0);
    }
#endif
#ifndef FEATURE_WLAN
    bReturnStatus = IMENUCTL_DeleteItem(pICurrentMenu, IDL_WLAN);
    if (bReturnStatus == FALSE) {
      AUX_ERR ("Failed to delete menu WLAN item", 0,0,0);
    }
#endif
    break;

  default:
    break;
  }
} /* End AUXStripFeatureMenuItem */

/*===========================================================================
FUNCTION AUXGetNextSelectedItemDlgID

DESCRIPTION
  The function is used when a menu control selection is being made.
  It returns the next dialog ID that corresponds to the item selected

DEPENDENCIES
  Note: This function should ONLY be called from a menu dialog which
    will create another dialog

SIDE EFFECTS
  None
===========================================================================*/
static uint16 AUXGetNextSelectedItemDlgID(AUXApp *pMe, uint16 wItemID)
{
  int i;

  for (i = 0; i< ARR_SIZE(AUXMenuActionTbl); i++) {
    if ((AUXMenuActionTbl[i].wItemID == wItemID) &&
        (AUXMenuActionTbl[i].wActionType == AUX_LAUNCH_DIALOG)) {
       return ((uint16) AUXMenuActionTbl[i].nActionData);
    }
  }

  AUX_ERR( "AUXGetNextSelectedItemDlgID(): No next dlgID found", 0, 0, 0);
  return 0; /* error */
} /* End AUXGetNextSelectedItemDlgID */

/*===========================================================================
FUNCTION AUXCreateMenuSelectedItemDlg

DESCRIPTION
  It obtains the current menu selection and create the next level dialog
  accordingly

DEPENDENCIES
  AUXGetDlgMenuCtlID, AUXGetNextSeletedItemDlgID

SIDE EFFECTS
  None
===========================================================================*/
boolean AUXCreateMenuSelectedItemDlg(AUXApp *pMe)
{
  IMenuCtl* pICurrentMenu;
  uint16 wCtrlID;
  uint16 wDlgID;
  uint16 wItemID;
  IDialog* pIActiveDialog;
  int nReturnStatus;

  if(pMe == NULL)
  {
    AUX_ERR("pMe NULL", 0, 0, 0);
    return FALSE;
  }

  pIActiveDialog = ISHELL_GetActiveDialog(pMe->a.m_pIShell);
  if (pIActiveDialog == NULL ) {
    AUX_ERR ("Null pointer ", 0,0,0);
    return FALSE;
  }

  /* Get the menu control */
  wCtrlID = AUXGetDlgMenuCtlID(pMe, pIActiveDialog);
  if(wCtrlID == 0) {
    AUX_ERR ("Null pointer ", 0,0,0);
    return FALSE;
  }

  pICurrentMenu = (IMenuCtl*) IDIALOG_GetControl(pIActiveDialog, (int16) wCtrlID);

  if(pICurrentMenu == NULL) {
    AUX_ERR ("Null pointer ", 0,0,0);
    return FALSE;
  }

  /* Get the selected item */
  wItemID = IMENUCTL_GetSel((IMenuCtl*)pICurrentMenu);

  /* Get next dialog ID */
  wDlgID = AUXGetNextSelectedItemDlgID(pMe, wItemID);

  if(wDlgID == 0) { /* error */
    AUX_ERR ("Null pointer ", 0,0,0);
    return FALSE;
  }

  /* Create next dialog */
  nReturnStatus = ISHELL_CreateDialog(pMe->a.m_pIShell, gpAUXResourceFile,
              wDlgID, NULL);

  return AUXCheckAEEReturnStatus(nReturnStatus);

} /* End AUXCreateMenuSelectedItemDlg */

#ifdef FEATURE_RUNTIME_DEVMAP
/*===========================================================================
FUNCTION AUXRdmEvent_cb

DESCRIPTION
  register call back function for Runtime Dev Map, it will send an event to
  the currently active dialog

DEPENDENCIES
  None

SIDE EFFECTS
  None
===========================================================================*/
void AUXRdmEvent_cb (rdm_assign_status_type status,
                    rdm_service_enum_type service,
                    rdm_device_enum_type device)
{
  IShell *shell_ptr = AEE_GetShell();

  if(NULL == shell_ptr)
  {
    AUX_ERR("shell_ptr NULL", 0, 0, 0);
    return;
  }
  
  switch(status) {
  case RDM_DONE_S:
    (void)ISHELL_PostEventEx(shell_ptr, EVTFLG_ASYNC, AEECLSID_AUXSETTINGS, EVT_USER,
                    EVT_RDM_DONE, 0L);
    break;
  case RDM_NOT_ALLOWED_S:
    (void)ISHELL_PostEventEx(shell_ptr, EVTFLG_ASYNC, AEECLSID_AUXSETTINGS, EVT_USER,
                    EVT_RDM_NOT_ALLOWED, 0L);
    break;
  case RDM_DEVMAP_BUSY_S:
    (void)ISHELL_PostEventEx(shell_ptr, EVTFLG_ASYNC, AEECLSID_AUXSETTINGS, EVT_USER,
                    EVT_RDM_DEVMAP_BUSY, 0L);
    break;
  case RDM_APP_BUSY_S:
    (void)ISHELL_PostEventEx(shell_ptr, EVTFLG_ASYNC, AEECLSID_AUXSETTINGS, EVT_USER,
                    EVT_RDM_APP_BUSY, 0L);
    break;
  default:
    break;
  }
} /* End AUXRdmEvent_cb */


/*===========================================================================
FUNCTION AUXIsRunTimePortMapAvail

DESCRIPTION
  Check if a message dlg or a menu dlg should be displayed next for the port
  map selection

DEPENDENCIES
  rdm_get_valid_ports

SIDE EFFECTS
  None
===========================================================================*/
static boolean AUXIsRunTimePortMapAvail(AUXApp *pMe)
{
  rdm_service_display_type devices[RDM_DEV_MAX];
  boolean bStatus;
  int nValidDev = 0;
  int i;

  bStatus = rdm_get_valid_ports ((rdm_service_enum_type) pMe->m_nCurrSrvc, devices);

  if (!bStatus) {
    AUX_ERR ("No valid ports", 0,0,0);
    return FALSE;
  }

  for (i=0; i < RDM_DEV_MAX ; i++) {
    if (devices[i].display && devices[i].device != RDM_SRVC_NOT_ALLOWED) {
      nValidDev++;
    }
  }

  if (nValidDev == 0) {  /* nothing to show - must be an invalid selection */
    AUX_ERR ("Invalid selection", 0,0,0);
    return FALSE;
  }

  return TRUE;
} /* End AUXIsRunTimePortMapAvail */

#ifdef FEATURE_DATA_MULTI_PORTS
/*=============================================================================
FUNCTION:  AUXGetRunTimeMultiPortMapCheckBoxVal

DESCRIPTION: Gets RunTimePortMap Check Box Items 

RETURN VALUE:
   None

COMMENTS:

SIDE EFFECTS:

SEE ALSO:

=============================================================================*/
static boolean AUXGetRunTimeMultiPortMapCheckBoxVal(rdm_service_display_type* devices,
                                                 uint16 devId)
{ 
  int i;

  for (i = RDM_FIRST_DEV; i < RDM_DEV_MAX ; i++) {
    if (devices[i].device == devId)
      return devices[i].current;
  }
  return FALSE;
  
}

/*=============================================================================
FUNCTION:  AUXSetRunTimeMultiPortMapCheckBoxVal

DESCRIPTION: Sets RunTimePortMap Items on/off

RETURN VALUE:
   None

COMMENTS:

SIDE EFFECTS:

SEE ALSO:

=============================================================================*/
void AUXSetRunTimeMultiPortMapCheckBoxVal(IMenuCtl   *pMe,
                 uint16      listId,
                 boolean     bSel)
{
   CtlAddItem cai;

   (void) MEMSET(&cai, 0, sizeof(cai));

   /* Use the same resource file as the rest of AUXApp */
   cai.pszResImage = gpAUXResourceFile;
   cai.wImage = bSel ? IDB_CHECK_ON : IDB_CHECK_OFF;

   (void) IMENUCTL_SetItem(pMe, listId, MSIF_IMAGE, &cai);

   IMENUCTL_SetStyle(pMe, NULL, NULL);
   (void)IMENUCTL_Redraw (pMe);
}

/*===========================================================================
FUNCTION AUXInitRuntimeMultiPortMapDlg

DESCRIPTION
  initialize portmap submenu dlg if FEATURE_RUNTIME_DEVMAP

DEPENDENCIES
  rdm_get_valid_ports, DisplayMessageDlg, AUXSetDefaultMenuLook

SIDE EFFECTS
  None
===========================================================================*/
static boolean AUXInitRuntimeMultiPortMapDlg(AUXApp *pMe, uint16 wDlgID, IDialog* pIActiveDialog)
{
  int i;
  char* pszDevString = NULL;
  boolean bStatus;
  boolean bRetVal = TRUE;
  CtlAddItem pai;
  IMenuCtl* pICurrentMenu;
  uint32 nDevLen;
  int nValidDev = 0;
    
  do
  {
    if(NULL == pMe)
    {
      AUX_ERR("pMe NULL", 0, 0, 0);
      bRetVal = FALSE;
      break;   
    }

    if((wDlgID != IDD_RUNTIME_PORTMAP) || (pIActiveDialog == NULL)) { /* error */
      AUX_ERR ("Null pointer %d, %x", wDlgID,pIActiveDialog,0);
      bRetVal = FALSE;
      break;
    }

    pICurrentMenu = (IMenuCtl*) IDIALOG_GetControl(pIActiveDialog, IDC_MENU_RUNTIME_PORTMAP);
    if(pICurrentMenu == NULL) { /* error */
      AUX_ERR ("Null pointer %x", pICurrentMenu,0,0);
      bRetVal = FALSE;
      break;
    }
    
    /* initialize pszDevString */
    pszDevString = MALLOC(AUX_TEXT_BUFFER_SIZE*sizeof(char));
    if (pszDevString == NULL) {
      AUX_ERR ("Null pointer %x", pszDevString,0,0);
      bRetVal = FALSE;
      break;
    }

    /* initialize m_pRdmDeviceList */
    pMe->m_pRdmDeviceList = (rdm_service_display_type*)REALLOC((void*)pMe->m_pRdmDeviceList,
      RDM_DEV_MAX*sizeof(rdm_service_display_type));
    if (pMe->m_pRdmDeviceList == NULL) {
      AUX_ERR ("Null pointer %x", pMe->m_pRdmDeviceList,0,0);
      bRetVal = FALSE;
      break;
    }
    bStatus = rdm_get_valid_ports ((rdm_service_enum_type) pMe->m_nCurrSrvc, 
                                                      pMe->m_pRdmDeviceList);
    if (!bStatus) {  /* error */
      /* return FALSE so that the menu dialog creation fail */
      /* shouldn't be here since IsRunTimePortMapAvail() already covered this case */
      AUX_ERR ("Invalid status", 0,0,0);
      bRetVal = FALSE;
      break;
    }

    nValidDev = 0;

    (void) MEMSET(&pai, 0, sizeof(pai));
    pai.pszResImage = gpAUXResourceFile;
    pai.pText = NULL;

    /* no need to Delete all for the menu control since there is no item currently
    on that menu in the resource editor */
    for (i = RDM_FIRST_DEV; i < RDM_DEV_MAX ; i++) {
      //if service is multi port data service, then null dev is not included.
      if (((rdm_service_enum_type) pMe->m_nCurrSrvc == RDM_DATA_SRVC) &&
        (i == RDM_NULL_DEV))
        continue;

      if (pMe->m_pRdmDeviceList[i].display && 
        pMe->m_pRdmDeviceList[i].device != RDM_SRVC_NOT_ALLOWED) {

        (void) rdm_get_device_str (pMe->m_pRdmDeviceList[i].device, 
                                                      pszDevString); /* get the device string */

        /* get the actual length of the device name */
        nDevLen = STRLEN(pszDevString);

        /* set the CtlAddItem */
        pai.pText = (AECHAR*) REALLOC((void*)pai.pText, (nDevLen+1)*sizeof(AECHAR));
        if (pai.pText == NULL) {
          AUX_ERR ("Null pointer %x", pai.pText,0,0);
          bRetVal = FALSE;
          break;
        }
        (void) STRTOWSTR(pszDevString, (AECHAR*)pai.pText, (int) ((nDevLen+1)*sizeof(AECHAR))); /* convert to AECHAR */
        
        //When current service is RDM_DATA_SRVC, multi-ports are supported.
        //In other cases, single port is still used.
        if ((rdm_service_enum_type) pMe->m_nCurrSrvc == RDM_DATA_SRVC)
          pai.wImage = IDB_CHECK_OFF;
        else
          pai.wImage = IDB_RADIO_UNFILLED;
        
        //The base of wItemID starts from 1.
        pai.wItemID = (uint16) i+1;
        pai.dwData = pMe->m_pRdmDeviceList[i].device;

        if(pMe->m_pRdmDeviceList[i].current) { /* currently selected item */
          if ((rdm_service_enum_type) pMe->m_nCurrSrvc == RDM_DATA_SRVC)
            pai.wImage = IDB_CHECK_ON;
          else
            pai.wImage = IDB_RADIO_FILLED;  
        }
        (void)IMENUCTL_AddItemEx(pICurrentMenu, &pai);

        // check if SetSel is allowed before adding the item!!!!!!!!!!!!!!!!!!
        if(pMe->m_pRdmDeviceList[i].current) {
          IMENUCTL_SetSel(pICurrentMenu, pai.wItemID);
        }
        nValidDev++;
      }
    }

    if (nValidDev == 0) {  /* nothing to show - must be an invalid selection */
      /* return FALSE so that the menu dialog creation fail */
      /* should not get here since the IsRunTimePortMapAvail() should take care of this */
      AUX_ERR ("No valid device", 0,0,0);
      bRetVal = FALSE;
      break;
    }

    /* runtime port map menu avaliable */
    AUXSetDefaultMenuLook(pMe->a.m_pIShell, pICurrentMenu);
  } while(0);
  
  FREEIF(pai.pText);
  FREEIF(pszDevString);

  return bRetVal;
} /* End InitRuntimeMultiPortMapDlg */

#else  /* ifdef FEATURE_DATA_MULTI_PORTS */

/*===========================================================================
FUNCTION AUXInitRuntimePortMapDlg

DESCRIPTION
  initialize portmap submenu dlg if FEATURE_RUNTIME_DEVMAP

DEPENDENCIES
  rdm_get_valid_ports, DisplayMessageDlg, AUXSetDefaultMenuLook

SIDE EFFECTS
  None
===========================================================================*/
static boolean AUXInitRuntimePortMapDlg(AUXApp *pMe, uint16 wDlgID, IDialog* pIActiveDialog)
{
  int i;
  rdm_service_display_type devices[RDM_DEV_MAX];
  char* pszDevString;
  boolean bStatus;
  CtlAddItem pai;
  IMenuCtl* pICurrentMenu;
  uint32 nDevLen;
  int nValidDev = 0;

  if(NULL == pMe)
  {
    AUX_ERR("pMe NULL", 0, 0, 0);
    return FALSE;
  }

  if((wDlgID != IDD_RUNTIME_PORTMAP) || (pIActiveDialog == NULL)) { /* error */
    AUX_ERR ("Null pointer", 0,0,0);
    return FALSE;
  }

  pICurrentMenu = (IMenuCtl*) IDIALOG_GetControl(pIActiveDialog, IDC_MENU_RUNTIME_PORTMAP);
  if(pICurrentMenu == NULL) { /* error */
    AUX_ERR ("Null pointer", 0,0,0);
    return FALSE;
  }

  bStatus = rdm_get_valid_ports ((rdm_service_enum_type) pMe->m_nCurrSrvc, devices);

  if (!bStatus) {  /* error */
    /* return FALSE so that the menu dialog creation fail */
    /* shouldn't be here since IsRunTimePortMapAvail() already covered this case */
    AUX_ERR ("Invalid status", 0,0,0);
    return FALSE;
  }

  nValidDev = 0;

  (void) MEMSET(&pai, 0, sizeof(pai));
  pai.pszResImage = gpAUXResourceFile;

  /* initialize pszDevString */
  pszDevString = (char*) MALLOC(AUX_TEXT_BUFFER_SIZE*sizeof( char ));


  /* no need to Delete all for the menu control since there is no item currently
     on that menu in the resource editor */
  for (i=0; i < RDM_DEV_MAX ; i++) {
    if (devices[i].display && devices[i].device != RDM_SRVC_NOT_ALLOWED) {

      (void) rdm_get_device_str (devices[i].device, pszDevString); /* get the device string */

      if (pszDevString == NULL) {
        AUX_ERR ("Null pointer", 0,0,0);
        return FALSE;
      }

      /* get the actual length of the device name */
      nDevLen = STRLEN(pszDevString);

      /* set the CtlAddItem */
      pai.pText = (AECHAR*) MALLOC((nDevLen+1)*sizeof(AECHAR));
      (void) STRTOWSTR(pszDevString, (AECHAR*)pai.pText, (int) ((nDevLen+1)*sizeof(AECHAR))); /* convert to AECHAR */

      pai.wImage = IDB_RADIO_UNFILLED;
      pai.wItemID = (uint16) i+1;
      pai.dwData = devices[i].device;

      if(devices[i].current) { /* currently selected item */
        pai.wImage = IDB_RADIO_FILLED;
      }
      (void)IMENUCTL_AddItemEx(pICurrentMenu, &pai);

      // check if SetSel is allowed before adding the item!!!!!!!!!!!!!!!!!!
      if(devices[i].current) {
        IMENUCTL_SetSel(pICurrentMenu, pai.wItemID);
      }
      nValidDev++;
    }
  }

  if (nValidDev == 0) {  /* nothing to show - must be an invalid selection */
    /* return FALSE so that the menu dialog creation fail */
    /* should not get here since the IsRunTimePortMapAvail() should take care of this */
    AUX_ERR ("No valid device", 0,0,0);
    return FALSE;
  }

  /* runtime port map menu avaliable */
  AUXSetDefaultMenuLook(pMe->a.m_pIShell, pICurrentMenu);
  return TRUE;

} /* End AUXInitRuntimePortMapDlg */

#endif /* ifdef FEATURE_DATA_MULTI_PORTS */


/*===========================================================================
FUNCTION AUXInitPortMapDlg

DESCRIPTION
  initialize portmap menu dlg events

DEPENDENCIES
  AUXStripFeatureMenuItem, DynamicPortMap, AUXSetDefaultMenuLook

SIDE EFFECTS
  None
===========================================================================*/
static boolean AUXInitPortMapDlg(AUXApp *pMe, uint16 wDlgID, IDialog* pIActiveDialog)
{
  IMenuCtl* pICurrentMenu;

  if(NULL == pMe)
  {
    AUX_ERR("pMe NULL", 0, 0, 0);
    return FALSE;
  }

  if((pIActiveDialog == NULL) || (wDlgID != IDD_PORTMAP)) {
    AUX_ERR ("Null pointer", 0,0,0);
    return FALSE;
  }

  pICurrentMenu = (IMenuCtl*) IDIALOG_GetControl(pIActiveDialog, IDC_MENU_PORTMAP);

  if(pICurrentMenu == NULL) {
    AUX_ERR ("Null pointer", 0,0,0);
    return FALSE;
  }

  AUXStripFeatureMenuItem(pMe, pICurrentMenu, IDC_MENU_PORTMAP);

  /* Set the menu style */
  AUXSetDefaultMenuLook(pMe->a.m_pIShell, pICurrentMenu);
  return TRUE;

} /* End AUXInitPortMapDlg */
#endif /* defined (FEATURE_RUNTIME_DEVMAP) */

/*===========================================================================
FUNCTION AUXIsBaudRateAvail

DESCRIPTION
  check if baud rate for data or diag is avaliable, this helps to determine
  whether a display message or a menu dlg should be display next

DEPENDENCIES
  ui_get_nv, sdm_ui_get_table_ds_baud, sdm_ui_get_table_diag_baud

SIDE EFFECTS
  None
===========================================================================*/
static boolean AUXIsBaudRateAvail(uint16 wItemID)
{
#if defined (FEATURE_DIAG_UI_BAUD) || defined (FEATURE_DS_UI_BAUD)
  nv_stat_enum_type     nvstat = NV_NOTACTIVE_S;
  sdm_ui_baud_menu_type *baud_ptr;
  nv_item_type nvi;


  if(wItemID == IDL_DSBAUD) {
#ifdef FEATURE_DS_DEFAULT_BITRATE_NV
      nvstat = ui_get_nv( NV_DS_DEFAULT_BAUDRATE_I, &nvi );
#endif
      baud_ptr = sdm_ui_get_table_ds_baud( nvstat, &nvi );
  }
  else { /* diag baud rate */
#ifdef FEATURE_DIAG_DEFAULT_BITRATE_NV
      nvstat = ui_get_nv( NV_DIAG_DEFAULT_BAUDRATE_I, &nvi );
#endif
#ifdef FEATURE_DIAG_UI_BAUD
      baud_ptr = sdm_ui_get_table_diag_baud( (nv_stat_enum_type)nvstat,
                                             (nv_item_type*)&nvi );
#endif
  }

  if(baud_ptr == NULL) { /* display "Feature Not avaliable */
    AUX_ERR ("Null pointer", 0,0,0);
    return FALSE;
  }

  return TRUE;
#else
  return FALSE;
#endif
} /* End AUXIsBaudRateAvail */


/*===========================================================================
FUNCTION AUXInitBaudDlg

DESCRIPTION
  build dynamic DS and Diag baud menu

DEPENDENCIES
  ui_get_nv, sdm_ui_get_table_ds_baud, sdm_ui_get_table_diag_baud

SIDE EFFECTS
  None
===========================================================================*/
static boolean AUXInitBaudDlg(AUXApp *pMe, uint16 wDlgID, IDialog* pIActiveDialog)
{
#if defined (FEATURE_DIAG_UI_BAUD) || defined (FEATURE_DS_UI_BAUD)
  IMenuCtl* pICurrentMenu;
  int i;
  word wCurrentSelection;
  int nNumItems;
  nv_stat_enum_type     nvstat = NV_NOTACTIVE_S;
  CtlAddItem pai;
  sdm_ui_baud_menu_type *baud_ptr;
  nv_item_type nvi;
  uint32 nLen;

  if(pMe == NULL || pIActiveDialog == NULL) {
    AUX_ERR ("Null pointer", 0,0,0);
    return FALSE;
  }

  if((wDlgID != IDD_DATA_BAUD) && (wDlgID != IDD_DIAG_BAUD)) { /* error */
    AUX_ERR ("Null pointer", 0,0,0);
    return FALSE;
  }

  if(wDlgID == IDD_DATA_BAUD) {
    pICurrentMenu = (IMenuCtl*) IDIALOG_GetControl(pIActiveDialog, IDC_MENU_DATABAUD);
  }
  else {
    pICurrentMenu = (IMenuCtl*) IDIALOG_GetControl(pIActiveDialog, IDC_MENU_DIAGBAUD);
  }

  if (pICurrentMenu == NULL) {
    AUX_ERR ("Null pointer", 0,0,0);
    return FALSE;
  }

  if(wDlgID == IDD_DATA_BAUD) {
#ifdef FEATURE_DS_DEFAULT_BITRATE_NV
    nvstat = ui_get_nv( NV_DS_DEFAULT_BAUDRATE_I, &nvi );
#endif
    baud_ptr = sdm_ui_get_table_ds_baud( nvstat, &nvi );
  }
  else {
#ifdef FEATURE_DIAG_DEFAULT_BITRATE_NV
    nvstat = ui_get_nv( NV_DIAG_DEFAULT_BAUDRATE_I, &nvi );
#endif
#ifdef FEATURE_DIAG_UI_BAUD
    baud_ptr = sdm_ui_get_table_diag_baud( nvstat, (nv_item_type*)&nvi );
#endif
  }

  if(baud_ptr == NULL) { /* error, IsBaudRateAvail() should have checked this already */
    AUX_ERR ("Null pointer", 0,0,0);
    return FALSE;
  }

  nNumItems = baud_ptr->num_items;
  wCurrentSelection = baud_ptr->current_index;

  (void) MEMSET(&pai, 0, sizeof(pai));
  pai.pszResImage = gpAUXResourceFile;
  for (i = 0; i< nNumItems && i < 10; i++) {

    nLen = STRLEN(baud_ptr->items[i]->item_string);
    pai.pText = (AECHAR*) MALLOC((nLen+1)*sizeof(AECHAR));

    (void) STRTOWSTR(baud_ptr->items[i]->item_string, (AECHAR*)pai.pText, (int)((nLen+1)*sizeof(AECHAR)));

    pai.wImage = IDB_RADIO_UNFILLED;
    if (wCurrentSelection == i) {
      pai.wImage = IDB_RADIO_FILLED;
    }
    pai.wItemID = (uint16) i+1;
    pai.dwData = (uint32) i;
    (void)IMENUCTL_AddItemEx(pICurrentMenu, &pai);

    // check if SetSel is allowed before adding the item!!!!!!!!!!!!!!!!!!
    if (wCurrentSelection == i) {
      IMENUCTL_SetSel(pICurrentMenu, pai.wItemID);
    }
  }
  AUXSetDefaultMenuLook(pMe->a.m_pIShell, pICurrentMenu);
  return TRUE;
#else
  return FALSE;
#endif
} /* End AUXInitBaudDlg */


#if defined (FEATURE_RUNTIME_DEVMAP)
/*===========================================================================
FUNCTION AUXPortMapDlgEventHandler

DESCRIPTION
  handle portmap menu dlg events

DEPENDENCIES
  None

SIDE EFFECTS
  None
===========================================================================*/
boolean AUXPortMapDlgEventHandler(void *pUser, AEEEvent evt, uint16 w, uint32 dw)
{
  AUXApp * pMe = (AUXApp*) pUser;
  int nReturnStatus;
  uint32 dwData;
  IMenuCtl* pICurrentMenu;
  uint32 wDlgID;
  IDialog *pIActiveDialog;

  if (pMe == NULL) { /* error checking */
    AUX_ERR ("Null pointer", 0,0,0);
    return FALSE;
  }

#ifdef FEATURE_BREW_3_0
  // do not want to handle autorepeat key events
  if((evt == EVT_KEY) && ((dw & KB_AUTOREPEAT) != 0))
  {
    return FALSE;
  }
#endif

  /* Reset Timer */
  if(evt == EVT_DIALOG_START || evt == EVT_KEY_PRESS || evt == EVT_COMMAND) {
  }

  switch (evt) {

  case EVT_DIALOG_START:
    AUX_MSG ("PortMapDlg: EVT_DIALOG_START", 0, 0, 0);
    return AUXInitPortMapDlg(pMe, w, (IDialog*)dw);

  case EVT_KEY:
    AUX_MSG ("PortMapDlg: EVT_KEY", 0, 0, 0);
    switch(w) {
    case AVK_CLR:
      nReturnStatus = ISHELL_EndDialog(pMe->a.m_pIShell);
      return AUXCheckAEEReturnStatus(nReturnStatus);
    
    /* end key */
    case AVK_END:
      (void)ISHELL_CloseApplet(pMe->a.m_pIShell, FALSE);
      return TRUE;

    /* digit keys */
    case AVK_0:
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
    case AVK_POUND:
    /* navigation keys */
    case AVK_UP:
    case AVK_DOWN:
    case AVK_LEFT:
    case AVK_RIGHT:
    case AVK_SELECT:
    /* send key */
    case AVK_SEND:
    case AVK_MESSAGE:  /* message, menu and mute not sure..!!!! */
    case AVK_MENU:
    case AVK_MUTE:
      return TRUE; /* handled keys */

    default:
      return FALSE; /* pass to core app to handle */
    }

  case EVT_COMMAND:

    AUX_MSG ("PortMapDlg: EVT_COMMAND, w=%d, dw=%d", w, dw, 0);

    pIActiveDialog = ISHELL_GetActiveDialog(pMe->a.m_pIShell);
    if (pIActiveDialog == NULL) { /* error */
      return FALSE;
    }
    wDlgID = IDIALOG_GetID(pIActiveDialog);
    if(wDlgID != IDD_PORTMAP) {
      AUX_ERR("BAD DLG",0,0,0);
      return FALSE;
    }
    pICurrentMenu = (IMenuCtl*) IDIALOG_GetControl(pIActiveDialog, IDC_MENU_PORTMAP);

    (void)IMENUCTL_GetItemData(pICurrentMenu, w, &dwData);

    pMe->m_nCurrSrvc = dwData;
    if(AUXIsRunTimePortMapAvail(pMe)) {
      nReturnStatus = ISHELL_CreateDialog(pMe->a.m_pIShell,
                                        gpAUXResourceFile, IDD_RUNTIME_PORTMAP,
                                        NULL);
      return AUXCheckAEEReturnStatus(nReturnStatus);
    }
    else {
      AUXDisplayMessageDlg(pMe, IDS_INVALID_SERVICE, 3);
      return TRUE;
    }


  case EVT_DIALOG_END:
    AUX_MSG ("PortMapDlg: EVT_DIALOG_END", 0, 0, 0);
    return TRUE;

  default: /* Other events that have not been handled by the handler */
      return FALSE;
  }
} /* End AUXPortMapDlgEventHandler */


#ifdef FEATURE_DATA_MULTI_PORTS
/*===========================================================================
FUNCTION AUXRunTimeMultiPortMapDlgEventHandler

DESCRIPTION
  handle runtime multi portmap menu dlg events

DEPENDENCIES
  rdm_assign_port

SIDE EFFECTS
  None
===========================================================================*/
boolean AUXRunTimeMultiPortMapDlgEventHandler(void *pUser, AEEEvent evt, uint16 w, uint32 dw)
{
  AUXApp * pMe = (AUXApp*) pUser;
  int nReturnStatus;
  uint32 dwData;
  IMenuCtl* pICurrentMenu;
  uint32 wDlgID;
  IDialog *pIActiveDialog;
  boolean setBox;
  
  if (NULL == pMe) { /* error checking */
    return FALSE;
  }

#ifdef FEATURE_BREW_3_0
  // do not want to handle autorepeat key events
  if((evt == EVT_KEY) && ((dw & KB_AUTOREPEAT) != 0))
  {
    return FALSE;
  }
#endif

  /* Reset Timer */
  if(evt == EVT_DIALOG_START || evt == EVT_KEY_PRESS || evt == EVT_COMMAND) {
  }

  switch (evt) {

  case EVT_DIALOG_START:
    AUX_MSG ("RunTmPortMpDlg: EVT_DLG_START", 0, 0, 0);
    if (pMe->m_nCurrSrvc == (unsigned long) RDM_NULL_SRVC) { /* error */
      AUX_ERR ("Null service", 0,0,0);
      return FALSE;
    }

    return AUXInitRuntimeMultiPortMapDlg(pMe, w, (IDialog*)dw);

  case EVT_KEY:
    AUX_MSG ("RunTmPortMpDlg: EVT_KEY", 0, 0, 0);
    if (pMe->m_nCurrSrvc == (unsigned long) RDM_NULL_SRVC) {
      /* after user has made a selection and then presses any key stroke */
      /* cancel the display idle menu dlg timer since we won't go back to idle
      until we receive event from RDM */
      AUXDisplayMessageDlg(pMe, IDS_CHANGING_DEV_NOW, 3);
      switch(w) {

      /* digit keys */
      case AVK_0:
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
      case AVK_POUND:
      /* navigation keys */
      case AVK_UP:
      case AVK_DOWN:
      case AVK_LEFT:
      case AVK_RIGHT:
      case AVK_SELECT:
      /* send key */
      case AVK_SEND:
      /* clear key */
      case AVK_CLR:
      /* end key */
      case AVK_END:
      case AVK_MESSAGE:  /* message, menu and mute not sure..!!!! */
      case AVK_MENU:
      case AVK_MUTE:
        return TRUE; /* handled keys */

      default:
        return FALSE; /* pass to core app to handle */
      }
    }
    else { /* user hasn't made any selection and wants to exit this dialog */
      switch(w) {
      case AVK_CLR:
        nReturnStatus = ISHELL_EndDialog(pMe->a.m_pIShell);
        return AUXCheckAEEReturnStatus(nReturnStatus);
      case AVK_END:
        (void)ISHELL_CloseApplet(pMe->a.m_pIShell, FALSE);
        return TRUE;

      /* digit keys */
      case AVK_0:
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
      case AVK_POUND:
      /* navigation keys */
      case AVK_UP:
      case AVK_DOWN:
      case AVK_LEFT:
      case AVK_RIGHT:
      case AVK_SELECT:
      /* send key */
      case AVK_SEND:
      case AVK_MESSAGE:  /* message, menu and mute not sure..!!!! */
      case AVK_MENU:
      case AVK_MUTE:
        return TRUE; /* handled keys */

      default:
        return FALSE; /* pass to core app to handle */
      }
    }

  case EVT_COMMAND:

    AUX_MSG ("RunTmPortMpDlg: EVT_CMD", 0, 0, 0);
    if (pMe->m_nCurrSrvc != (unsigned long) RDM_NULL_SRVC) {
      /* no timer is associated with this, since the event from RDM will trigger the ending
         of this dialog */
      /* cancel the display idle menu dlg timer since we won't go back to idle until we receive event from RDM */

      pIActiveDialog = ISHELL_GetActiveDialog(pMe->a.m_pIShell);
      if (pIActiveDialog == NULL) { /* error */
        return FALSE;
      }
      wDlgID = IDIALOG_GetID(pIActiveDialog);
      if(wDlgID != IDD_RUNTIME_PORTMAP) {
        AUX_ERR("BAD DLG",0,0,0);
        return FALSE;
      }
      pICurrentMenu = (IMenuCtl*) IDIALOG_GetControl(pIActiveDialog, IDC_MENU_RUNTIME_PORTMAP);

      IMENUCTL_GetItemData(pICurrentMenu, w, &dwData);
      
      //When current service is RDM_DATA_SRVC, multi-ports are supported.
      //In other cases, single port is still used.
      if ((rdm_service_enum_type) pMe->m_nCurrSrvc == RDM_DATA_SRVC)
      { 
        setBox = !AUXGetRunTimeMultiPortMapCheckBoxVal(pMe->m_pRdmDeviceList, dwData);
        AUXSetRunTimeMultiPortMapCheckBoxVal (pICurrentMenu, w, setBox);
          
        /* call rdm to assign port */
        if (setBox) {
          (void) rdm_assign_port ((rdm_service_enum_type) pMe->m_nCurrSrvc,
                                (rdm_device_enum_type) dwData, AUXRdmEvent_cb);
        } else {
          (void) rdm_close_device ((rdm_service_enum_type) pMe->m_nCurrSrvc,
                                (rdm_device_enum_type) dwData, AUXRdmEvent_cb);
        }
      }
      else
      { /* update the radio button image */
        AUXInitMenuIcons(pICurrentMenu);
        AUXSetMenuIcon (pICurrentMenu, w, TRUE);
        /* call rdm to assign port */
        (void) rdm_assign_port ((rdm_service_enum_type) pMe->m_nCurrSrvc,
                                (rdm_device_enum_type) dwData,
                               AUXRdmEvent_cb);
      }
      pMe->m_nCurrSrvc = (unsigned long) RDM_NULL_SRVC;
    }
    else { /* user is trying to make another selection while we are waiting for RDM to respond */
      /* cancel the display idle menu dlg timer since we won't go back to idle until we receive event from RDM */
      AUXDisplayMessageDlg(pMe, IDS_CHANGING_DEV_NOW, 3);
    }
    return TRUE;


  case EVT_USER:
    AUX_MSG ("RunTmPortMpDlg: EVT_USER", 0, 0, 0);
    switch(w) {
    case EVT_MESSAGE_END:
      if (dw == IDS_INVALID_SERVICE ) {
        /* End the port map dialog after Invalid service display */
        nReturnStatus = ISHELL_EndDialog(pMe->a.m_pIShell);
        return AUXCheckAEEReturnStatus(nReturnStatus);
      }
      else if (dw == IDS_CHANGING_DEV_NOW ) {
        /* cancel the display idle menu dlg timer since we won't go back to idle until we receive event from RDM */
        return TRUE;
      }
      return FALSE;

    case EVT_RDM_DONE:
      /* Successful portmapping */
      MSG_HIGH ("Portmap successful !!!", 0,0,0);
      nReturnStatus = ISHELL_EndDialog(pMe->a.m_pIShell);
      return AUXCheckAEEReturnStatus(nReturnStatus);

    case EVT_RDM_NOT_ALLOWED:
    case EVT_RDM_DEVMAP_BUSY:
    case EVT_RDM_APP_BUSY:
      /* Did not go through - bail out */
      MSG_HIGH ("Portmap failed !!!", 0,0,0);
      nReturnStatus = ISHELL_EndDialog(pMe->a.m_pIShell);
      return AUXCheckAEEReturnStatus(nReturnStatus);

    default:
      return FALSE;
    }


  case EVT_DIALOG_END:
    AUX_MSG ("RunTmPortMpDlg: EVT_DLG_END", 0, 0, 0);
    pMe->m_nCurrSrvc = (unsigned long) RDM_NULL_SRVC;
    FREEIF(pMe->m_pRdmDeviceList);
    pMe->m_pRdmDeviceList = NULL;
    return TRUE;

  default: /* Other events that have not been handled by the handler */
      return FALSE;
  }
} /* End RunTimeMultiPortMapDlgEventHandler */

#else /* ifdef FEATURE_DATA_MULTI_PORTS */

/*===========================================================================
FUNCTION AUXRunTimePortMapDlgEventHandler

DESCRIPTION
  handle runtime portmap menu dlg events

DEPENDENCIES
  InitRuntimePortMapDlg, AUXInitMenuIcons, AUXSetMenuIcon, rdm_assign_port

SIDE EFFECTS
  None
===========================================================================*/
boolean AUXRunTimePortMapDlgEventHandler(void *pUser, AEEEvent evt, uint16 w, uint32 dw)
{
  AUXApp * pMe = (AUXApp*) pUser;
  int nReturnStatus;
  uint32 dwData;
  IMenuCtl* pICurrentMenu;
  uint32 wDlgID;
  IDialog *pIActiveDialog;

  if (pMe == NULL) { /* error checking */
    return FALSE;
  }

#ifdef FEATURE_BREW_3_0
  // do not want to handle autorepeat key events
  if((evt == EVT_KEY) && ((dw & KB_AUTOREPEAT) != 0))
  {
    return FALSE;
  }
#endif

  /* Reset Timer */
  if(evt == EVT_DIALOG_START || evt == EVT_KEY_PRESS || evt == EVT_COMMAND) {
  }

  switch (evt) {

  case EVT_DIALOG_START:
    AUX_MSG ("RunTimePortMapDlg: EVT_DLG_START", 0, 0, 0);
    if (pMe->m_nCurrSrvc == (unsigned long) RDM_NULL_SRVC) { /* error */
      AUX_ERR ("Null service", 0,0,0);
      return FALSE;
    }

    return AUXInitRuntimePortMapDlg(pMe, w, (IDialog*)dw);

  case EVT_KEY:
    AUX_MSG ("RunTimePortMapDlg: EVT_KEY, key=%d", w, 0, 0);
    if (pMe->m_nCurrSrvc == (unsigned long) RDM_NULL_SRVC) {
      /* after user has made a selection and then presses any key stroke */
      /* cancel the display idle menu dlg timer since we won't go back to idle
      until we receive event from RDM */
      AUXDisplayMessageDlg(pMe, IDS_CHANGING_DEV_NOW, 3);
      switch(w) {
      /* digit keys */
      case AVK_0:
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
      case AVK_POUND:
      /* navigation keys */
      case AVK_UP:
      case AVK_DOWN:
      case AVK_LEFT:
      case AVK_RIGHT:
      case AVK_SELECT:
      /* send key */
      case AVK_SEND:
      /* clear key */
      case AVK_CLR:
      /* end key */
      case AVK_END:
      case AVK_MESSAGE:  /* message, menu and mute not sure..!!!! */
      case AVK_MENU:
      case AVK_MUTE:
        return TRUE; /* handled keys */

      default:
        return FALSE; /* pass to core app to handle */
      }
    }
    else { /* user hasn't made any selection and wants to exit this dialog */
      switch(w) {
      case AVK_CLR:
        nReturnStatus = ISHELL_EndDialog(pMe->a.m_pIShell);
        return AUXCheckAEEReturnStatus(nReturnStatus);
      case AVK_END:
        (void)ISHELL_CloseApplet(pMe->a.m_pIShell, FALSE);
        return TRUE;

      /* digit keys */
      case AVK_0:
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
      case AVK_POUND:
      /* navigation keys */
      case AVK_UP:
      case AVK_DOWN:
      case AVK_LEFT:
      case AVK_RIGHT:
      case AVK_SELECT:
      /* send key */
      case AVK_SEND:
      case AVK_MESSAGE:  /* message, menu and mute not sure..!!!! */
      case AVK_MENU:
      case AVK_MUTE:
        return TRUE; /* handled keys */

      default:
        return FALSE; /* pass to core app to handle */
      }
    }

  case EVT_COMMAND:

    AUX_MSG ("RunTimePortMapDlg: EVT_COMMAND, w=%d, dw=%d", w, dw, 0);
    if (pMe->m_nCurrSrvc != (unsigned long) RDM_NULL_SRVC) {
      /* no timer is associated with this, since the event from RDM will trigger the ending
         of this dialog */
      /* cancel the display idle menu dlg timer since we won't go back to idle until we receive event from RDM */

      pIActiveDialog = ISHELL_GetActiveDialog(pMe->a.m_pIShell);
      if (pIActiveDialog == NULL) { /* error */
        return FALSE;
      }
      wDlgID = IDIALOG_GetID(pIActiveDialog);
      if(wDlgID != IDD_RUNTIME_PORTMAP) {
        AUX_ERR("BAD DLG",0,0,0);
        return FALSE;
      }
      pICurrentMenu = (IMenuCtl*) IDIALOG_GetControl(pIActiveDialog, IDC_MENU_RUNTIME_PORTMAP);

      (void)IMENUCTL_GetItemData(pICurrentMenu, w, &dwData);
      /* update the radio button image */
      AUXInitMenuIcons(pICurrentMenu);
      AUXSetMenuIcon (pICurrentMenu, w, TRUE);
      /* call rdm to assign port */
      (void) rdm_assign_port ((rdm_service_enum_type) pMe->m_nCurrSrvc,
                              (rdm_device_enum_type) dwData,
                             AUXRdmEvent_cb);
      pMe->m_nCurrSrvc = (unsigned long) RDM_NULL_SRVC;
    }
    else { /* user is trying to make another selection while we are waiting for RDM to respond */
      /* cancel the display idle menu dlg timer since we won't go back to idle until we receive event from RDM */
      AUXDisplayMessageDlg(pMe, IDS_CHANGING_DEV_NOW, 3);
    }
    return TRUE;


  case EVT_USER:
    AUX_MSG ("RunTimePortMapDlg: EVT_USER", 0, 0, 0);
    switch(w) {
    case EVT_MESSAGE_END:
      if (dw == IDS_INVALID_SERVICE ) {
        /* End the port map dialog after Invalid service display */
        nReturnStatus = ISHELL_EndDialog(pMe->a.m_pIShell);
        return AUXCheckAEEReturnStatus(nReturnStatus);
      }
      else if (dw == IDS_CHANGING_DEV_NOW ) {
        /* cancel the display idle menu dlg timer since we won't go back to idle until we receive event from RDM */
        return TRUE;
      }
      return FALSE;

    case EVT_RDM_DONE:
      /* Successful portmapping */
      MSG_HIGH ("Portmap successful !!!", 0,0,0);
      nReturnStatus = ISHELL_EndDialog(pMe->a.m_pIShell);
      return AUXCheckAEEReturnStatus(nReturnStatus);

    case EVT_RDM_NOT_ALLOWED:
    case EVT_RDM_DEVMAP_BUSY:
    case EVT_RDM_APP_BUSY:
      /* Did not go through - bail out */
      MSG_HIGH ("Portmap failed !!!", 0,0,0);
      nReturnStatus = ISHELL_EndDialog(pMe->a.m_pIShell);
      return AUXCheckAEEReturnStatus(nReturnStatus);

    default:
      return FALSE;
    }


  case EVT_DIALOG_END:
    AUX_MSG ("RunTimePortMapDlg: EVT_DLG_END", 0, 0, 0);
    pMe->m_nCurrSrvc = (unsigned long) RDM_NULL_SRVC;
    return TRUE;

  default: /* Other events that have not been handled by the handler */
      return FALSE;
  }
} /* End AUXRunTimePortMapDlgEventHandler */

#endif /* ifdef FEATURE_DATA_MULTI_PORTS */
#endif /* ifdef FEATURE_RUNTIME_DEVMAP */


/*===========================================================================
FUNCTION AUXBaudDlgEventHandler

DESCRIPTION
  handle DS and Data baud menu dlg events

DEPENDENCIES
  None

SIDE EFFECTS
  None
===========================================================================*/
boolean AUXBaudDlgEventHandler(void *pUser, AEEEvent evt, uint16 w, uint32 dw)
{
  AUXApp * pMe = (AUXApp*) pUser;
  int nReturnStatus;
  IDialog * pIActiveDialog;
  uint16 wDlgID;
  uint32 dwData;
  IMenuCtl* pICurrentMenu;

  if (pMe == NULL) { /* error checking */
    AUX_ERR ("Null pointer", 0,0,0);
    return FALSE;
  }

#ifdef FEATURE_BREW_3_0
  // do not want to handle autorepeat key events
  if((evt == EVT_KEY) && ((dw & KB_AUTOREPEAT) != 0))
  {
    return FALSE;
  }
#endif

  /* Reset Timer */
  if(evt == EVT_DIALOG_START || evt == EVT_KEY_PRESS || evt == EVT_COMMAND) {
  }

  switch (evt) {

  case EVT_DIALOG_START:
    AUX_MSG ("BaudDlg: EVT_DIALOG_START", 0, 0, 0);
    /* if invalid service display, this function will return FALSE */
    return AUXInitBaudDlg(pMe, w, (IDialog*)dw);

  case EVT_KEY:
    AUX_MSG ("BaudDlg: EVT_KEY", 0, 0, 0);
    switch(w) {
    case AVK_CLR:
      nReturnStatus = ISHELL_EndDialog(pMe->a.m_pIShell);
      return AUXCheckAEEReturnStatus(nReturnStatus);
    case AVK_END:
      (void)ISHELL_CloseApplet(pMe->a.m_pIShell, FALSE);
      return TRUE;

    /* digit keys */
    case AVK_0:
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
    case AVK_POUND:
    /* navigation keys */
    case AVK_UP:
    case AVK_DOWN:
    case AVK_LEFT:
    case AVK_RIGHT:
    case AVK_SELECT:
    /* send key */
    case AVK_SEND:
    case AVK_MESSAGE:  /* message, menu and mute not sure..!!!! */
    case AVK_MENU:
    case AVK_MUTE:
      return TRUE; /* handled keys */

    default:
      return FALSE; /* pass to core app to handle */
    }

  case EVT_COMMAND:
    AUX_MSG ("BaudDlg: EVT_COMMAND, w=%d, dw=%d", w, dw, 0);
    pIActiveDialog = ISHELL_GetActiveDialog(pMe->a.m_pIShell);
    if (pIActiveDialog == NULL) { /* error */
      return FALSE;
    }
    wDlgID = IDIALOG_GetID(pIActiveDialog);
    if(wDlgID == IDD_DATA_BAUD) {
      pICurrentMenu = (IMenuCtl*) IDIALOG_GetControl(pIActiveDialog, IDC_MENU_DATABAUD);
      (void)IMENUCTL_GetItemData((IMenuCtl*)pICurrentMenu, w, &dwData);
      sdm_ui_set_ds_baud((uint8)(dwData));
    }
    else if(wDlgID == IDD_DIAG_BAUD){
      pICurrentMenu = (IMenuCtl*) IDIALOG_GetControl(pIActiveDialog, IDC_MENU_DIAGBAUD);
      (void)IMENUCTL_GetItemData((IMenuCtl*)pICurrentMenu, w, &dwData);
#ifdef FEATURE_DIAG_UI_BAUD
      sdm_ui_set_diag_baud((uint8)(dwData));
#endif
    }
    else { /* error */
      return FALSE;
    }

    /* No need to update the radio button since a message is being displayed immediately */
    AUXDisplayMessageDlg(pMe, IDS_BAUD_CHANGE_CONF, 3);
    return TRUE;


  case EVT_USER:
    AUX_MSG ("BaudDlg: EVT_USER", 0, 0, 0);
    if(w == EVT_MESSAGE_END) {
      /* End the port map dialog after displaying
         portmap reset required message for
         static dev map feature */
      nReturnStatus = ISHELL_EndDialog(pMe->a.m_pIShell);
      return AUXCheckAEEReturnStatus(nReturnStatus);
    }
    return FALSE;


  case EVT_DIALOG_END:
    AUX_MSG ("BaudDlg: EVT_DLG_END", 0, 0, 0);
    return TRUE;

  default: /* Other events that have not been handled by the handler */
      return FALSE;
  }
} /* End AUXBaudDlgEventHandler */


/*===========================================================================
FUNCTION AUXMenuDlgEventHandler

DESCRIPTION
  handle generic menu dlg events

DEPENDENCIES
  None

SIDE EFFECTS
  None
===========================================================================*/
boolean AUXMenuDlgEventHandler(void *pUser, AEEEvent evt, uint16 w, uint32 dw)
{
  AUXApp * pMe = (AUXApp*) pUser;
  IMenuCtl* pICurrentMenu;
  uint16 wCtrlID;
  boolean bReturnValue;

  if (pMe == NULL) { /* error checking */
    AUX_ERR ("Null pointer", 0,0,0);
    return FALSE;
  }

#ifdef FEATURE_BREW_3_0
  // do not want to handle autorepeat key events
  if((evt == EVT_KEY) && ((dw & KB_AUTOREPEAT) != 0))
  {
    return FALSE;
  }
#endif

  switch (evt) {

  case EVT_DIALOG_START:
    AUX_MSG ("MenuDlg: EVT_DIALOG_START", 0, 0, 0);
    wCtrlID = AUXGetDlgMenuCtlID(pMe, (IDialog*)dw);
    if(wCtrlID == 0) {
      AUX_ERR ("Null pointer", 0,0,0);
      return FALSE;
    }
    pICurrentMenu = (IMenuCtl*)IDIALOG_GetControl((IDialog*)dw, (int16) wCtrlID);
    if(pICurrentMenu) {
      /* remove certain items based on featurization */
      AUXStripFeatureMenuItem(pMe, pICurrentMenu, wCtrlID);
      AUXSetDefaultMenuLook(pMe->a.m_pIShell, pICurrentMenu);
      return TRUE;
    }
    AUX_ERR ("Null pointer", 0,0,0);
    return FALSE; /* no menu control ---> error */

  case EVT_KEY:
    AUX_MSG ("MenuDlg: EVT_KEY", 0, 0, 0);
    switch(w) {
    case AVK_CLR:
      AUXEndCurrentDlg(pMe);
      return TRUE;
    
    case AVK_END:
      (void)ISHELL_CloseApplet(pMe->a.m_pIShell, FALSE);
      return TRUE; // This shouldn't be executed?

    /* digit keys */
    case AVK_0:
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
    case AVK_POUND:
    /* navigation keys */
    case AVK_UP:
    case AVK_DOWN:
    case AVK_LEFT:
    case AVK_RIGHT:
    case AVK_SELECT:
    /* send key */
    case AVK_SEND:
    case AVK_MESSAGE:  /* message, menu and mute not sure..!!!! */
    case AVK_MENU:
    case AVK_MUTE:
      return TRUE; /* Digit keys should be ignored */

    default:
      return FALSE;
    }

  case EVT_COMMAND:
    AUX_MSG ("MenuDlg: EVT_COMMAND, w=%d, dw=%d", w, dw, 0);

    if (w == IDL_DSBAUD || w == IDL_DIAGBAUD) {
      if(!AUXIsBaudRateAvail(w)) {
        AUXDisplayMessageDlg(pMe, IDS_FEATURE_UNAVAILABLE, 3);
        return TRUE;
      }
    }

    bReturnValue = AUXCreateMenuSelectedItemDlg(pMe);
    if (bReturnValue == FALSE) {
      AUX_ERR ("Could not create dialog", 0, 0, 0);
    }
    return TRUE;

  case EVT_DIALOG_END:
    AUX_MSG ("MenuDlg: EVT_DIALOG_END", 0, 0, 0);
    return TRUE;

  default: /* Other events that have not been handled by the handler */
    break;
  }

  return FALSE;
} /* End AUXMenuDlgEventHandler */

/*===========================================================================
FUNCTION AUXMessageDlgEventHandler

DESCRIPTION
  handle message dialog events

DEPENDENCIES
  None

SIDE EFFECTS
  None
===========================================================================*/
boolean AUXMessageDlgEventHandler(void *pUser, AEEEvent evt, uint16 w, uint32 dw)
{
  IStatic * pIStatic;
  IDialog * pIDialog;
  uint16 wDlgID;

  AUXApp * pMe = (AUXApp*) pUser;

  if (pMe == NULL) { /* error checking */
    AUX_ERR ("Null pointer", 0,0,0);
    return FALSE;
  }

#ifdef FEATURE_BREW_3_0
  // do not want to handle autorepeat key events
  if((evt == EVT_KEY) && ((dw & KB_AUTOREPEAT) != 0)) {
    return FALSE;
  }
#endif

  switch (evt) {
  case EVT_DIALOG_START:
    pIStatic = (IStatic*) IDIALOG_GetControl((IDialog*)dw, IDC_STATIC_MESSAGE);
    if(pIStatic != NULL) {
      if (pMe->m_MessageID != 0) {
        AECHAR *pBuf=NULL;
        (void) AUXSetGeneralStaticTextStyle(pIStatic);
        pBuf = MALLOC (AUX_MAX_STR_SIZE*sizeof(AECHAR));
        if (pBuf == NULL) {
          AUX_ERR ("No memory", 0,0,0);
          return FALSE;
        }
        if (ISHELL_LoadResString (pMe->a.m_pIShell, gpAUXResourceFile, pMe->m_MessageID,
                                  pBuf, AUX_MAX_STR_SIZE * sizeof(AECHAR)) != 0) {
          ISTATIC_SetText (pIStatic, NULL, pBuf, AEE_FONT_BOLD, AEE_FONT_BOLD);
          IDIALOG_Redraw ((IDialog*)dw);
        }
        FREE (pBuf);
      }
      else {
        if (pMe->m_pszMsgString != NULL) {
          ISTATIC_SetText (pIStatic, NULL, pMe->m_pszMsgString, AEE_FONT_BOLD,
                            AEE_FONT_BOLD);
          IDIALOG_Redraw ((IDialog*)dw);
        }
      }
    }
    else {
      AUX_ERR("FAILED to get pIStatic for control id",0,0,0);
    }
    return TRUE;

  case EVT_KEY:

    if (w == AVK_END) { /* go all the way back to idle screen */
      pIDialog = ISHELL_GetActiveDialog(pMe->a.m_pIShell);
      if (pIDialog == NULL) {
        return FALSE; /* error */
      }
      wDlgID = IDIALOG_GetID(pIDialog);
      if (wDlgID == 0) {
        return FALSE;
      }

      (void)ISHELL_CancelTimer(pMe->a.m_pIShell, (PFNNOTIFY) AUXEndMessageDlg, pMe);
      AUXEndMessageDlg(pMe);

      return TRUE;
    }

    switch(w)
    {
    /* digit keys */
    case AVK_0:
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
    case AVK_POUND:
    /* navigation keys */
    case AVK_UP:
    case AVK_DOWN:
    case AVK_LEFT:
    case AVK_RIGHT:
    case AVK_SELECT:
    /* send key */
    case AVK_SEND:
    /* clear key */
    case AVK_CLR:
    case AVK_MESSAGE:  /* message, menu and mute not sure..!!!! */
    case AVK_MENU:
    case AVK_MUTE:
      /* these key events will make the dialog goes away and also cancel the timer
      if this action is taken before the timer is being triggered */
      (void)ISHELL_CancelTimer(pMe->a.m_pIShell, (PFNNOTIFY) AUXEndMessageDlg, pMe);
      AUXEndMessageDlg(pMe);
      return TRUE;

    default:
      return FALSE; /* let core app handles these events */
    }

  case EVT_COMMAND:
    return TRUE;

  case EVT_DIALOG_END:
    (void)ISHELL_CancelTimer(pMe->a.m_pIShell, (PFNNOTIFY) AUXEndMessageDlg, pMe);
    /* Clear all variables and free memory if any */
    pMe->m_MessageID = 0;
    if (pMe->m_pszMsgString) {
      FREE (pMe->m_pszMsgString);
      pMe->m_pszMsgString = NULL;
    }

    return TRUE;

  default: /* Other events that have not been handled by the handler */
    return FALSE;
  }
} /*End AUXMessageDlgEventHandler*/

/*===========================================================================

FUNCTION AUXSetDefaultMenuLook

DESCRIPTION

PROTOTYPE:


PARAMETERS:

DEPENDENCIES
  none

RETURN VALUE

SIDE EFFECTS
  none
===========================================================================*/
void AUXSetDefaultMenuLook( IShell *ps, IMenuCtl *pm)
{
  AEEItemStyle sstyle;
  AEEItemStyle ustyle;
  AEEMenuColors col;

  ISHELL_GetItemStyle( ps, AEE_IT_MENU, &ustyle, &sstyle );

  /* selection style */
  sstyle.ft = AEE_FT_RAISED;
  sstyle.xOffset = 0;
  sstyle.yOffset = 0;
  sstyle.roImage = AEE_RO_TRANSPARENT;
  ustyle.ft = AEE_FT_NONE;
  ustyle.xOffset = 2;
  ustyle.yOffset = 0;
  ustyle.roImage = AEE_RO_TRANSPARENT;
  IMENUCTL_SetStyle(pm, &ustyle, &sstyle);

  /* colors */
  col.cSelText= RGB_BLACK;
  col.wMask = MC_BACK | MC_SEL_BACK| MC_SEL_TEXT;
  col.cBack = MAKE_RGB(255, 255, 204);

  col.cSelBack = MAKE_RGB(153, 204, 204);

  IMENUCTL_SetColors(pm, &col);

}

/*=============================================================================
FUNCTION:  AUXGetResourcePath

DESCRIPTION: Prepares the appropriate path using canonical form depending on
 the version of BREW

PARAMETERS:
  None

RETURN VALUE:
   Valid char * if sucessful. 
   NULL: If unsucessful

COMMENTS:
  The pointer returned needs to be FREE'ed by the caller.

SIDE EFFECTS:

SEE ALSO:

=============================================================================*/
char * AUXGetResourcePath()
{
  char   *     pszResFile;
  AEEDeviceInfo   di;
  char        szCoreClsID[20];
  IShell  *    pIShell; 

  di.wStructSize = sizeof(AEEDeviceInfo);
  pIShell = (IShell *)  AEE_GetShell();
  if(!pIShell)
  {
      MSG_ERROR("Could not get IShell", 0, 0, 0);
      return NULL;
  }

  ISHELL_GetDeviceInfo(pIShell, &di);
  pszResFile = (char*) MALLOC(di.wMaxPath);
  if(!pszResFile)
  {
      MSG_ERROR("Malloc failed", 0, 0, 0);
      return NULL;
  }


  #if MIN_BREW_VERSION(3,0)
  // Prepare the path for the resource file to be fs:/~COREAPP_CLASSID/coreapp.bar
  // This is needed because the context of the application calling this method might
  // not be the same as Core App.
  (void)SNPRINTF(szCoreClsID, sizeof(szCoreClsID), "%d",AEECLSID_AUXSETTINGS);
  (void)STRLCPY(pszResFile, "fs:/~", di.wMaxPath);
  (void)STRLCAT(pszResFile, szCoreClsID, di.wMaxPath);
  (void)STRLCAT(pszResFile, "/", di.wMaxPath);
  (void)STRLCAT(pszResFile, gpAUXResourceFile, di.wMaxPath);
  #else
  (void)STRLCPY(pszResFile, gpAUXResourceFile, di.wMaxPath);
  #endif // MIN_BREW_VERSION(3,0) 

  return pszResFile;
}

/*=============================================================================
FUNCTION:  AUXInitMenuIcons

DESCRIPTION: Adds IDB_RADIO_UNFILLED images to every menu item in the
             specified menu.

PARAMETERS:
   *pm:

RETURN VALUE:
   None

COMMENTS:

SIDE EFFECTS:

SEE ALSO:

=============================================================================*/
void AUXInitMenuIcons(IMenuCtl *pm)
{
   CtlAddItem cai;
   int        i;
   char *    pszResFile;
   
   pszResFile = AUXGetResourcePath();
   
   if(!pszResFile)
     return;

   (void) MEMSET(&cai, 0, sizeof(cai));
   cai.pszResImage = pszResFile;
   cai.wImage = IDB_RADIO_UNFILLED;

   for (i = (IMENUCTL_GetItemCount(pm) -1); i >= 0; i--) {
      (void) IMENUCTL_SetItem(pm,
                              IMENUCTL_GetItemID(pm, i),
                              MSIF_IMAGE,
                              &cai);
   }

   FREEIF(pszResFile);
   pszResFile = NULL;


   /* IMENUCTL_SetItem() does not recalculate the new maximum height of
    * the menu control's items when a menu item is changed.  This causes a
    * problem if an image is added to a menu item that is larger than the
    * current maximum height of the menu items.
    *
    * For example, say the current maximum menu item height is 10 pixels and
    * the display height is 40 pixels.  The menu control can show 4 menu items
    * at a time.  Now IMENUCTL_SetItem() is used to add a 20 pixel high image
    * to one of the menu items.  The menu control can now only display three
    * items on the display at one time.  However it will still try to display
    * four items.  If the fourth item in the menu is currently selected, the
    * menu selection will not be visible.
    */
   // The workaround for this bug is to call an IMenuCtl method that
   // will indirectly cause the menu control to recalculate the maximum height
   // of the menu items (as there is no IMenuCtl method to do this directly).
   // Invoking IMENUCTL_SetStyle() will NULL style parameters works nicely
   // because it has no effect besides performing the necessary recalculation.
   IMENUCTL_SetStyle(pm, NULL, NULL);
}


/*=============================================================================
FUNCTION:  AUXSetMenuIcon

DESCRIPTION: Sets Radio Buttons on/off

PARAMETERS:
   *pm:
   listId:
   bSel:

RETURN VALUE:
   None

COMMENTS:

SIDE EFFECTS:

SEE ALSO:

=============================================================================*/
void AUXSetMenuIcon(IMenuCtl   *pm,
                 uint16      listId,
                 boolean     bSel)
{
   CtlAddItem cai;
   char *    pszResFile;

   pszResFile = AUXGetResourcePath();

   if(!pszResFile)
     return;

   (void) MEMSET(&cai, 0, sizeof(cai));
   cai.pszResImage = pszResFile;
   cai.wImage = bSel ? IDB_RADIO_FILLED : IDB_RADIO_UNFILLED;

   (void) IMENUCTL_SetItem(pm, listId, MSIF_IMAGE, &cai);
   FREEIF(pszResFile);
   pszResFile = NULL;
   
   if (bSel) {
     IMENUCTL_SetSel (pm, listId);
   }
   IMENUCTL_SetStyle(pm, NULL, NULL);
   (void)IMENUCTL_Redraw (pm);
}

#ifdef FEATURE_WLAN
/*===========================================================================

FUNCTION AUXSetDefaultSoftkeyLook

DESCRIPTION

PROTOTYPE:


PARAMETERS:

DEPENDENCIES
  none

RETURN VALUE

SIDE EFFECTS
  none
===========================================================================*/
void AUXSetDefaultSoftkeyLook(IShell *ps, IMenuCtl *pm)
{
  AEEItemStyle sstyle, ustyle;
  AEEMenuColors col;

  ISHELL_GetItemStyle( ps, AEE_IT_MENU, &ustyle, &sstyle);


  /* selection style */
  sstyle.ft = AEE_FT_RAISED;
  sstyle.roImage = AEE_RO_TRANSPARENT;
  ustyle.ft = AEE_FT_NONE;
  ustyle.roImage = AEE_RO_TRANSPARENT;
  IMENUCTL_SetStyle(pm, &ustyle, &sstyle);

  /* colors */
  col.cSelText= RGB_BLACK;
  col.wMask = MC_BACK | MC_SEL_BACK| MC_SEL_TEXT;
  col.cBack = MAKE_RGB(255, 255, 204);

  col.cSelBack = MAKE_RGB(153, 204, 204);

  IMENUCTL_SetColors(pm, &col);
}
#endif /*#ifdef FEATURE_WLAN*/

/*===========================================================================
FUNCTION AUXGetNextEditDlgID

DESCRIPTION
  The function is used when a static dlg is to be followed by a textctl dialog.
  It returns the next TextCtl dialog with the given Static control dialog ID

DEPENDENCIES
  Note: should be ONLY called from static dialog only

SIDE EFFECTS
  None
===========================================================================*/
static uint16 AUXGetNextEditDlgID(uint16 wStaticDlgID)
{

  int i;
  int nCount;

  if ( wStaticDlgID == 0 ) {
    return 0;
  }

  nCount = ARR_SIZE(AUXLookupEditDlgID);
  for (i = 0; i< nCount; i++) {
    if (AUXLookupEditDlgID[i].wDialogID == wStaticDlgID) {
      return AUXLookupEditDlgID[i].wEditDialogID;
    }
  }
  AUX_ERR( "AUXGetNextEditDlgID(): No matching edit ctl", 0, 0, 0);
  return 0; /* error */

} /* End AUXGetNextEditDlgID */

/*===========================================================================
FUNCTION AUXGetDlgSkMenuCtlID

DESCRIPTION
  The function returns the softkey menu control for a given dialog ID

DEPENDENCIES
  None

SIDE EFFECTS
  None
===========================================================================*/
static uint16 AUXGetDlgSkMenuCtlID(uint16 wDlgID)
{
  int i;

  if ( wDlgID == 0 ) {
    return 0;
  }

  for (i = 0; i < ARR_SIZE(AUXStaticDlgs); i++) {
    if (AUXStaticDlgs[i].wDialogID == wDlgID) {
      return AUXStaticDlgs[i].wSoftkeyID;
    }
  }

  AUX_ERR( "AUXGetDlgSkMenuCtlID(): No matching soft key ctl", 0, 0, 0);
  return 0; /* error */

} /* End AUXGetDlgSkMenuCtlID */

/*===========================================================================
FUNCTION AUXGetStaticDlgTitleResStringID

DESCRIPTION
  This function matches dialog id with a resource string ID used for static
  control title

DEPENDENCIES
  None

SIDE EFFECTS
  None
===========================================================================*/
static uint16 AUXGetStaticDlgTitleResStringID(AUXApp *pMe, uint16 wDlgID)
{
  int i;

  if ( wDlgID == 0 ) {
    return 0;
  }
  for (i = 0; i< ARR_SIZE(AUXStaticDlgs); i++) {
    if (AUXStaticDlgs[i].wDialogID == wDlgID) {
      return AUXStaticDlgs[i].wDialogTitle;
    }
  }
  AUX_ERR( "AUXGetStaticDlgTitleResStringID(): No matching dialog", 0, 0, 0);
  return 0; /* error */
} /* End AUXGetStaticDlgTitleResStringID */

/*===========================================================================
FUNCTION AUXGetDlgStaticCtlID

DESCRIPTION
  return the control for the static dlg

DEPENDENCIES
  None

SIDE EFFECTS
  None
===========================================================================*/
static uint16 AUXGetDlgStaticCtlID(uint16 wDlgID)
{

  int i;

  if ( wDlgID == 0 ) {
    return 0;
  }
  for (i = 0; i< ARR_SIZE(AUXStaticDlgs); i++) {
    if (AUXStaticDlgs[i].wDialogID == wDlgID) {
      return AUXStaticDlgs[i].wCtrlID;
    }
  }
  AUX_ERR( "AUXGetDlgStaticCtlID(): No matching static ctl", 0, 0, 0);
  return 0; /* error */

} /* End AUXGetDlgStaticCtlID */

/*===========================================================================
FUNCTION AUXCreateTextEditDlg

DESCRIPTION
  Get next dialog ID and create the new dialog

DEPENDENCIES
  Note: This function should ONLY be called when the current dialog has a
        IStatic control. ?? for now, may make it more generalize later
  AUXGetNextEditDlgID

SIDE EFFECTS
  None
===========================================================================*/
static boolean AUXCreateTextEditDlg(AUXApp* pMe)
{
  IDialog* pIActiveDialog;
  uint16 wDlgID;
  uint16 wCurrentDlgID;
  int nReturnStatus;

  pIActiveDialog = ISHELL_GetActiveDialog(pMe->a.m_pIShell);
  if(pIActiveDialog == NULL) {
    AUX_ERR ("Null pointer", 0,0,0);
    return FALSE;
  }

  wCurrentDlgID = IDIALOG_GetID(pIActiveDialog);
  if (wCurrentDlgID == 0) { /*error checking */
    AUX_ERR ("Null pointer", 0,0,0);
    return FALSE;
  }

  /* Get next dialog ID */
  wDlgID = AUXGetNextEditDlgID(wCurrentDlgID);
  if(wDlgID == 0) { /* error */
    AUX_ERR ("Null pointer", 0,0,0);
    return FALSE;
  }

  nReturnStatus = ISHELL_CreateDialog(pMe->a.m_pIShell, gpAUXResourceFile,
              wDlgID, NULL);
  return AUXCheckAEEReturnStatus(nReturnStatus);
} /* End AUXCreateTextEditDlg */

/*===========================================================================
FUNCTION AUXSetStaticDlgTitle

DESCRIPTION
  Based on IDD, assign title for the static dialog

DEPENDENCIES
  Note: Still need to set the title even if it was created in the resource editor,
        because of the ISTATIC_SetText function call later on

SIDE EFFECTS
  None
===========================================================================*/
static void AUXSetStaticDlgTitle(AUXApp *pMe, uint16 wDlgID, AECHAR *title, int nLen)
{

  uint16 wStrID = 0;
  int nResult = 0;
  
  if(pMe == NULL)
  {
    AUX_ERR("pMe NULL", 0, 0, 0);
    return;
  }

  wStrID = AUXGetStaticDlgTitleResStringID(pMe, wDlgID);
  nResult = ISHELL_LoadResString(pMe->a.m_pIShell,
                          gpAUXResourceFile,
                          wStrID,
                          title,
                          nLen*((int)sizeof(AECHAR)));
  if(nResult == 0) {
    AUX_ERR("Failed to set title for static dlg %d",wDlgID,0,0);
  }

} /* End AUXSetStaticDlgTitle */

/*===========================================================================
FUNCTION AUXSetStaticDlgContent

DESCRIPTION
  Based on IDD, assign content for the static dialog

DEPENDENCIES

SIDE EFFECTS
  None
===========================================================================*/
static void AUXSetStaticDlgContent(AUXApp *pMe, uint16 wDlgID, AECHAR* pszContent, int nLen)
{
  if(pMe == NULL)
  {
    AUX_ERR("pMe NULL", 0, 0, 0);
    return;
  }

  switch(wDlgID)
  {
#ifdef FEATURE_WLAN
    case IDD_WLAN_PERFORM_MANUAL_SCAN_NOW:
      pszContent[0] = (AECHAR)'\0';
      break;

    case IDD_WLAN_START_NEW_ADHOC_NETWORK:
      pszContent[0] = (AECHAR)'\0';
      break;

    case IDD_SSID_VIEW:
    case IDD_CHANNEL_VIEW:
    case IDD_WLAN_LISTEN_INTERVAL_VIEW:
    case IDD_WLAN_RTS_VIEW:
    case IDD_WLAN_SCAN_TIME_VIEW:
    case IDD_WLAN_FRAG_VIEW:
    case IDD_WLAN_MAX_TX_POWER_VIEW:
    {
      const WLANEditDlgDataType *pDlgData = NULL;  /* Data for this dialog */
      int                        nErr;             /* Return code */
      uint16                     nEditDlgId = 0;   /* Edit dlg for the opt */

      pszContent[0] = (AECHAR)'\0';

      if ((!pMe->m_WLAN.m_pIWIFI) || (!pMe->m_WLAN.m_pIWIFIOpts))
      {
        AUX_ERR("Null ptr, %x, %x", pMe->m_WLAN.m_pIWIFI, pMe->m_WLAN.m_pIWIFIOpts, 0);
        return;
      }

      /* Get the corresponding "edit" dialog, so that we can lookup the
       * info we need.
       */
      nEditDlgId = AUXGetNextEditDlgID(wDlgID);
      if (0 == nEditDlgId)
      {
        /* If we couldn't find an entry for this dialog, we can't continue. */
        AUX_ERR("No Dialog entry in lookup table for dialog %d.",
                 wDlgID, 0, 0);
        return;
      }

      pDlgData = AUXGetWLANEditDlgTbl(nEditDlgId);

      if (pDlgData == NULL)
      {
        /* If we couldn't find an entry for this dialog, we can't continue. */
        AUX_ERR("No Dialog entry in table for dialog %d.", wDlgID, 0, 0);
        return;
      }

      /* Load the Object with the current settings. */
      nErr = IWIFI_LoadOptions(pMe->m_WLAN.m_pIWIFI, 0, pMe->m_WLAN.m_pIWIFIOpts);
      if (SUCCESS == nErr)
      {
        WIFIOpt tmp_Opt;

        /* Get the SSID item from the object. */
        nErr = IWIFIOpts_GetOpt(pMe->m_WLAN.m_pIWIFIOpts, pDlgData->nWIFIOptId,
                                &tmp_Opt);
        if (SUCCESS == nErr)
        {
          if (pDlgData->TextInputMode == AEE_TM_NUMBERS)
          {
            char   *pszOptString;
            int     nOptVal = (int)tmp_Opt.pVal;

            pszOptString = (char *)MALLOC(pDlgData->nTextFieldSz + 1);
            if (! pszOptString)
            {
              AUX_ERR("No memory for option %d buffer.", pDlgData->nWIFIOptId,
                       0, 0);
              return;
            }

            if (wDlgID == IDD_CHANNEL_VIEW)
            {
              if (nOptVal == AEEWIFI_CHANNEL_ANY)
              {
                nOptVal = AUX_WLAN_CHANNEL_ANY;
              }
            }

            (void)SNPRINTF(pszOptString, pDlgData->nTextFieldSz + 1, "%d",
                           (uint32)nOptVal);

            /* Copy the contents. */
            (void)STRTOWSTR(pszOptString, pszContent, nLen);
            FREE(pszOptString);
          }
          else if (pDlgData->TextInputMode == AEE_TM_LETTERS)
          {
            /* Copy the contents. */
            (void)WSTRNCOPYN(pszContent, nLen, tmp_Opt.pVal, -1);
          }
          else
          {
            AUX_ERR("Unsupported text input type, %d.",
                     pDlgData->TextInputMode, 0, 0);
            return;
          }
        }
        else
        {
          AUX_ERR("Couldn't get option %d, err %d", pDlgData->nWIFIOptId,
                   nErr, 0);
          return;
        }
      }
      else
      {
        AUX_ERR("Couldn't load options, %d, err %d", pDlgData->nWIFIOptId,
                 nErr, 0);
        return;
      }
    }
    break;
#endif /*#ifdef FEATURE_WLAN*/

    case IDD_SETSECURETIME_VIEW:
    {
      JulianType time_d;
      (void) AUXGetSecureTimeofDay (pMe, &time_d);
      (void) AUXFormatTimeDate (time_d, pszContent, nLen);
      break;
    }

    default: /* error */
      pszContent[0] = (AECHAR)'\0';
      AUX_ERR("AUXSetStaticDlgContent(): no matching IDD", 0, 0, 0);
      break;
  }
} /* End AUXSetStaticDlgContent */

/*===========================================================================
FUNCTION AUXInitStaticDlg

DESCRIPTION
  Set the static dialog title, content and style

DEPENDENCIES
  AUXSetStaticDlgTitle, AUXSetStaticDlgContent, AUXSetGeneralStaticTextStyle,
  AUXSetStaticTextCtlSize

SIDE EFFECTS
  None
===========================================================================*/
static boolean AUXInitStaticDlg(AUXApp *pMe, uint16 wDlgID, IDialog* pIDialog)
{
  IStatic* pIStaticText;
  IMenuCtl* pIMenu;
  AECHAR title[AUX_TITLE_BUFFER_SIZE];
  AECHAR psContent[AUX_TEXT_BUFFER_SIZE];
  uint16 wStaticCtrlID;
  uint16 wMenuCtrlID;

  if(pMe == NULL)
  {
    AUX_ERR("pMe NULL", 0, 0, 0);
    return FALSE;
  }

  wStaticCtrlID = AUXGetDlgStaticCtlID(wDlgID);
  wMenuCtrlID = AUXGetDlgSkMenuCtlID(wDlgID);

  if ((wStaticCtrlID == 0) || (wMenuCtrlID == 0)) { /* error */
    AUX_ERR ("Null pointer", 0,0,0);
    return FALSE;
  }

  if(pIDialog == NULL) { /* error */
    AUX_ERR ("Null pointer", 0,0,0);
    return FALSE;
  }

  pIStaticText = (IStatic*)IDIALOG_GetControl(pIDialog, (int16) wStaticCtrlID);
  pIMenu = (IMenuCtl*)IDIALOG_GetControl(pIDialog, (int16) wMenuCtrlID);

  if((pIStaticText == NULL) || (pIMenu == NULL)) { /* error */
    AUX_ERR ("Null pointer", 0,0,0);
    return FALSE;
  }

  /* Set the title and content */
  AUXSetStaticDlgTitle(pMe, wDlgID, title, ARR_SIZE(title));
  AUXSetStaticDlgContent(pMe, wDlgID, psContent, ARR_SIZE(psContent));

  (void)ISTATIC_SetText(pIStaticText, title, psContent, AEE_FONT_BOLD, AEE_FONT_NORMAL);

  /* Set the softkey menu style */
  AUXSetDefaultMenuLook(pMe->a.m_pIShell, pIMenu);

  /* Get the newly defined softkey size and use that to set the static size */
  (void) AUXSetStaticTextCtlSize(pIStaticText, (IControl*)pIMenu);
  (void) AUXSetGeneralStaticTextStyle(pIStaticText);  /* set the static text style */

  /* Force the control to redraw */
  return ICONTROL_Redraw((IControl*)pIStaticText);

} /*End AUXInitStaticDlg */

/*===========================================================================
FUNCTION AUXStaticDlgEventHandler

DESCRIPTION
  handle static dlg events

DEPENDENCIES
  AUXInitStaticDlg, CreateTextEditDlg,

SIDE EFFECTS
  None
===========================================================================*/
boolean AUXStaticDlgEventHandler(void *pUser, AEEEvent evt, uint16 w, uint32 dw)
{
  AUXApp * pMe = (AUXApp*) pUser;
  int nReturnStatus;
#ifdef FEATURE_WLAN
  AEECMPhInfo *pPhInfo;
  AEECMWLANPref  sICMWLANData;
  AEECMPhError   error;
#endif /*#ifdef FEATURE_WLAN*/

  if (pMe == NULL) { /* error checking */
    AUX_ERR ("Null pointer", 0,0,0);
    return FALSE;
  }

#ifdef FEATURE_BREW_3_0
  // do not want to handle autorepeat key events
  if((evt == EVT_KEY) && ((dw & KB_AUTOREPEAT) != 0))
  {
    return FALSE;
  }
#endif

  /* Reset Timer */
  if(evt == EVT_DIALOG_START || evt == EVT_KEY || evt == EVT_COMMAND) {
    (void)ISHELL_CancelTimer(pMe->a.m_pIShell, (PFNNOTIFY) AUXDisplayIdleMenuDlg, pMe);
    nReturnStatus = ISHELL_SetTimer(pMe->a.m_pIShell,
                    AUXAPPTIMEOUT,
                    (PFNNOTIFY) AUXDisplayIdleMenuDlg,
                    (uint32*) pMe);
    if (nReturnStatus != SUCCESS)
    {
      AUX_ERR("Fail to set AUXAPPTIMEOUT: %d", nReturnStatus, 0, 0);
    }
  }


  switch(evt) {
  case EVT_DIALOG_START:
    AUX_MSG ("StaticDlg: EVT_DIALOG_START", 0, 0, 0);
      return AUXInitStaticDlg(pMe, w, (IDialog*)dw);

  case EVT_KEY:
    AUX_MSG ("StaticDlg: EVT_KEY", 0, 0, 0);
    switch(w) {
    case AVK_CLR:
      AUXEndCurrentDlg(pMe);
      return TRUE;

    /* digit keys */
    case AVK_0:
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
    case AVK_POUND:
    /* navigation keys */
    case AVK_UP:
    case AVK_DOWN:
    case AVK_LEFT:
    case AVK_RIGHT:
    case AVK_SELECT:
    /* send key */
    case AVK_SEND:
    case AVK_MESSAGE:  /* message, menu and mute not sure..!!!! */
    case AVK_MENU:
    case AVK_MUTE:
      return TRUE;

    default:
      return FALSE;
    }

  case EVT_COMMAND:
    AUX_MSG ("StaticDlg: EVT_COMMAND, w=%d, dw=%d", w, dw, 0);
    if(
#ifdef FEATURE_WLAN
       w == IDL_SSID_VIEW_EDIT || w == IDL_CHANNEL_VIEW_EDIT ||
       w == IDL_WLAN_LISTEN_VIEW_EDIT ||  w == IDL_WLAN_RTS_VIEW_EDIT ||
       w == IDL_WLAN_SCAN_TIME_VIEW_EDIT || w == IDL_WLAN_FRAGMENT_VIEW_EDIT ||
       w == IDL_WLAN_MAX_TX_POWER_VIEW_EDIT || 
#endif /*#ifdef FEATURE_WLAN*/
       w == IDL_SETSECURETIME_VIEW_SET)
    {
      return AUXCreateTextEditDlg(pMe); /* go to next text editing dialog */
    }
#ifdef FEATURE_WLAN
    else if ( w == IDL_SSID_VIEW_OK ||
              w == IDL_CHANNEL_VIEW_OK ||
              w == IDL_WLAN_LISTEN_VIEW_OK ||
              w == IDL_WLAN_RTS_VIEW_OK ||
              w == IDL_WLAN_SCAN_TIME_VIEW_OK ||
              w == IDL_WLAN_FRAGMENT_VIEW_OK ||
              w == IDL_WLAN_MAX_TX_POWER_VIEW_OK )
    {
      nReturnStatus = ISHELL_EndDialog(pMe->a.m_pIShell);
      return AUXCheckAEEReturnStatus(nReturnStatus);
      /* user is satisfied with the current setting, end the dialog */
    }
    else if (w == IDL_SK_WLAN_PERFORM_MANUAL_SCAN_NOW_YES) 
    { /* start manual search dlg */
      nReturnStatus = ISHELL_CreateDialog(pMe->a.m_pIShell, gpAUXResourceFile,
                                          IDD_WLAN_SELECT, NULL);
      return AUXCheckAEEReturnStatus(nReturnStatus);
    }
    else if (w == IDL_SK_WLAN_PERFORM_MANUAL_SCAN_NOW_NO) 
    {
      AUXPopDlg(pMe, IDD_WLAN_SETTINGS);
      return TRUE;
    }
    else if (w == IDL_SK_WLAN_START_NEW_ADHOC_NETWORK_OK) 
    {
      /* start new adhoc network */
      pMe->m_WLAN.m_bWLANAdhocAssociationRequest = FALSE;

      pPhInfo = MALLOC(sizeof(AEECMPhInfo));
      if (pPhInfo == NULL)
      {
        AUX_ERR("Malloc PhInfo failed", 0, 0, 0);
        return FALSE;
      }  

      nReturnStatus = ICM_GetPhoneInfo(pMe->m_WLAN.m_pICM, pPhInfo, sizeof(AEECMPhInfo)); 
      if (nReturnStatus != AEE_SUCCESS)
      {
        AUX_ERR("Failed getting phone info from ICM", 0, 0, 0);
        FREEIF(pPhInfo);
        return FALSE;
      }  
      (void)MEMCPY(&sICMWLANData, &(pPhInfo->m_wlan_pref), sizeof(AEECMWLANPref));
      FREEIF(pPhInfo);

      sICMWLANData.bss_type = AEECM_WLAN_BSS_TYPE_ADHOC_START;

      /* WLAN code prefers that the bss_id be set to zero. It will set
       * its own, unique, random bss_id as it starts the adhoc network */
      sICMWLANData.bss_id = 0;

      /* Call ICM_SetSystemPreferences_WLAN  */
      
      (void)ICM_SetSystemPreference_WLAN(pMe->m_WLAN.m_pICM,
                                         AEECM_MODE_PREF_CURRENT_PLUS_WLAN,
                                         AEECM_PREF_TERM_PERMANENT, 0,
                                         AEECM_GW_ACQ_ORDER_PREF_NO_CHANGE,
                                         AEECM_BAND_PREF_NO_CHANGE,
                                         AEECM_ROAM_PREF_NO_CHANGE,
                                         AEECM_HYBR_PREF_NO_CHANGE,
                                         AEECM_SRV_DOMAIN_PREF_NO_CHANGE,
                                         AEECM_NETWORK_SEL_MODE_PREF_NO_CHANGE,
                                         NULL, &sICMWLANData, &error, NULL);
      
      AUXPopDlg (pMe, IDD_WLAN_SETTINGS);
      return TRUE;
    }
    else if (w == IDL_SK_WLAN_START_NEW_ADHOC_NETWORK_CANCEL) 
    {
      pMe->m_WLAN.m_bWLANAdhocAssociationRequest = FALSE;
      AUXPopDlg (pMe, IDD_WLAN_SETTINGS);
      return TRUE;
    }
#endif /*#ifdef FEATURE_WLAN*/
    else if (w == IDL_SETSECURETIME_VIEW_OK)
    {  
       AUXEndCurrentDlg(pMe);
       return TRUE;
    }
    return FALSE;

  case EVT_DIALOG_END:
    AUX_MSG ("StaticDlg: EVT_DIALOG_END", 0, 0, 0);
    (void)ISHELL_CancelTimer(pMe->a.m_pIShell, (PFNNOTIFY) AUXDisplayIdleMenuDlg, pMe);
    nReturnStatus = ISHELL_SetTimer(pMe->a.m_pIShell,
                    AUXAPPTIMEOUT,
                    (PFNNOTIFY) AUXDisplayIdleMenuDlg,
                    (uint32*) pMe);
    if (nReturnStatus != SUCCESS)
    {
      AUX_ERR("Fail to set AUXAPPTIMEOUT: %d", nReturnStatus, 0, 0);
    }
    return TRUE;

  default: /* Other events that have not been handled by the handler */
    return FALSE;
  }
} /* End AUXStaticDlgEventHandler */

#if defined(FEATURE_WLAN) || \
    (defined(FEATURE_RUNTIME_DEVMAP) && defined(FEATURE_USB_DEFAULT_MTP))
/*===========================================================================
FUNCTION AUXReadSetting

DESCRIPTION
  Read setting from GetMethod and return the item ID of corresponding list entry

DEPENDENCIES
  None

SIDE EFFECTS
  None
===========================================================================*/
static uint16 AUXReadSetting(AUXApp *pMe, uint16 wDlgID)
{
  int i, j;
  uint64 val=0;
  
  for (i=0; i < ARR_SIZE(AUXMenuActionTbl); i++) {
    if (AUXMenuActionTbl[i].wDialogID == wDlgID) {
      /* got a match */
      if (AUXMenuActionTbl[i].wActionType != AUX_MENU_SETTING) {
        /* Not a menu setting - here in error */
        AUX_ERR ("Invalid action for dialog %d", wDlgID, 0, 0);
        return 0;
      }

      if (AUXMenuActionTbl[i].getfnPtr == NULL) {
        /* Null Get fn pointer */
        AUX_ERR ("Invalid function pointer %d", wDlgID, 0, 0);
        return 0;
      }

      val = (*AUXMenuActionTbl[i].getfnPtr)(pMe);

      for (j=i; (j < ARR_SIZE(AUXMenuActionTbl)) && (AUXMenuActionTbl[j].wDialogID == wDlgID); j++) {
        if (AUXMenuActionTbl[j].nActionData == val) {
          return AUXMenuActionTbl[j].wItemID;
        }
      }
      /* No match found */
      AUX_ERR ("No match for  dialog %d", wDlgID, 0, 0);
      return 0;
    }
  }
  /* No match found */
  AUX_ERR ("No match for  dialog %d", wDlgID, 0, 0);
  return 0;

} /* End AUXReadSetting */

/*===========================================================================
FUNCTION AUXWriteSetting

DESCRIPTION
  Save the setting of given item through Set methods.
  Return TRUE if the setting was changed and FALSE otherwise.

DEPENDENCIES
  Various SetXXXSetting functions

SIDE EFFECTS
  None
===========================================================================*/
static boolean AUXWriteSetting(AUXApp *pMe, uint16 wDlgID, uint16 wItemID)
{
  int i, j;
  for (i=0; i < ARR_SIZE(AUXMenuActionTbl); i++) {
    if (AUXMenuActionTbl[i].wDialogID == wDlgID) {
      /* got a match */
      if (AUXMenuActionTbl[i].wActionType != AUX_MENU_SETTING) {
        /* Not a menu setting - here in error */
        AUX_ERR ("Invalid action for dialog %d", wDlgID, 0, 0);
        return FALSE;
      }


      for (j=i; (j < ARR_SIZE(AUXMenuActionTbl)) && (AUXMenuActionTbl[j].wDialogID == wDlgID); j++) {
        if (AUXMenuActionTbl[j].wItemID == wItemID) {
          if (AUXMenuActionTbl[j].setfnPtr == NULL) {
            /* Null Get fn pointer */
            AUX_ERR ("Invalid function pointer %d", wDlgID, 0, 0);
            return FALSE;
          }
          if ((*AUXMenuActionTbl[j].setfnPtr)(pMe, AUXMenuActionTbl[j].nActionData))
            return TRUE;
          return FALSE;
        }
      }

      /* No match found */
      AUX_ERR ("No match for  dialog %d", wDlgID, 0, 0);
      return FALSE;
    }
  }
  /* No match found */
  AUX_ERR ("No match for  dialog %d", wDlgID, 0, 0);
  return FALSE;

} /* End AUXWriteSetting */

/*===========================================================================
FUNCTION AUXInitRadioButtonDlg

DESCRIPTION
  This function update the radio button menu item list based on featurization,
  update the radio button for the selected item based on NV value

DEPENDENCIES
  AUXGetDlgMenuCtlID, AUXStripFeatureMenuItem,
  AUXSetMenuIcon, AUXSetDefaultMenuLook

SIDE EFFECTS
  None
===========================================================================*/
boolean AUXInitRadioButtonDlg(AUXApp *pMe, uint16 wDlgID, IDialog* pIActiveDialog)
{
  IMenuCtl* pICurrentMenu;
  uint16 wCtrlID, wItemID;

  if ( pMe == NULL || pIActiveDialog == NULL ) {
    AUX_ERR ("Null pointer", 0,0,0);
    return FALSE;
  }

  /* Get the control ID */
  wCtrlID = AUXGetDlgMenuCtlID(pMe, pIActiveDialog);
  if(wCtrlID == 0) { /* error */
    AUX_ERR ("Null pointer", 0,0,0);
    return FALSE;
  }

  pICurrentMenu = (IMenuCtl*) IDIALOG_GetControl(pIActiveDialog, (int16) wCtrlID);
  if (pICurrentMenu == NULL) {
    AUX_ERR ("Null pointer", 0,0,0);
    return FALSE;
  }

  /* Remove or update items from the menu based on featurization requirement */
  AUXStripFeatureMenuItem(pMe, pICurrentMenu, wCtrlID);

  /* Get the current setting from static variable or NV */
  wItemID = AUXReadSetting(pMe, wDlgID);

  if(wItemID == 0) { /* error */
    AUX_ERR ("Null pointer", 0,0,0);
    AUXDisplayMessageDlg(pMe, IDS_READ_FAILED, 3);
    return FALSE;
  }

  /* Initialize radio button images with IDB_RADIO_UNFILLED */
  AUXInitMenuIcons (pICurrentMenu);

  /* Update the radio button for the currently selected option */
  AUXSetMenuIcon(pICurrentMenu, wItemID, TRUE);

  /* Set the menu style */
  AUXSetDefaultMenuLook(pMe->a.m_pIShell, pICurrentMenu);
  return TRUE;

} /* End AUXInitRadioButtonDlg */

/*===========================================================================
FUNCTION AUXUpdateSetting

DESCRIPTION
  Update menu setting static variable and NV values if the selected Item is
  NV related.
  Based on whether the change of setting took place, update the display of
  radio buttons.

DEPENDENCIES
  AUXReadSetting, AUXInitMenuIcons, AUXSetMenuIcon

SIDE EFFECTS
  None
===========================================================================*/
boolean AUXUpdateSetting(AUXApp *pMe,
                         uint16 wSelectedItemID,
                         IMenuCtl* pICurrentMenu)
{
  IDialog* pIActiveDialog;

  uint16 wDlgID;
  uint16 wCtrlID;

  if ((wSelectedItemID == 0) || (pMe == NULL))
  {
    /* error checking */
    return FALSE;
  }

  pIActiveDialog = ISHELL_GetActiveDialog(pMe->a.m_pIShell);
  if(pIActiveDialog == NULL) {
    AUX_ERR ("Null pointer", 0,0,0);
    return FALSE;
  }

  wDlgID = IDIALOG_GetID(pIActiveDialog);
  if (wDlgID == 0) { /*error checking */
    AUX_ERR ("Null pointer", 0,0,0);
    return FALSE;
  }

  wCtrlID = AUXGetDlgMenuCtlID(pMe, pIActiveDialog);
  if(wCtrlID == 0) {
    AUX_ERR ("Null pointer", 0,0,0);
    return FALSE;
  }

  pICurrentMenu= (IMenuCtl*) IDIALOG_GetControl(pIActiveDialog, (int16) wCtrlID);
  if(pICurrentMenu == NULL) {
    AUX_ERR ("Null pointer", 0,0,0);
    return FALSE;
  }


  if(AUXWriteSetting(pMe, wDlgID, wSelectedItemID)) {

    /* Initialize radio button images with IDB_RADIO_UNFILLED */
    AUXInitMenuIcons (pICurrentMenu);

    /* Update radio button image to reflect new setting */
    AUXSetMenuIcon (pICurrentMenu, wSelectedItemID, TRUE);

    return TRUE;
  }
  return FALSE;
} /* End AUXUpdateSetting */

/*===========================================================================
FUNCTION AUXRadioButtonDlgEventHandler

DESCRIPTION
  Handle radio button event

DEPENDENCIES
  None

SIDE EFFECTS
  None
===========================================================================*/
boolean AUXRadioButtonDlgEventHandler(void *pUser, AEEEvent evt, uint16 w, uint32 dw)
{
  int nReturnStatus;
  AUXApp * pMe = (AUXApp*) pUser;
  IDialog *pIDialog;
  IMenuCtl* pICurrentMenu;
  uint16 wCtrlID;

  if ( pMe == NULL ) { /* error checking */
    AUX_ERR ("Null pointer", 0,0,0);
    return FALSE;
  }

#ifdef FEATURE_BREW_3_0
  // do not want to handle autorepeat key events
  if((evt == EVT_KEY) && ((dw & KB_AUTOREPEAT) != 0))
  {
    return FALSE;
  }
#endif

  /* Reset Timer */
  if(evt == EVT_DIALOG_START || evt == EVT_KEY_PRESS || evt == EVT_COMMAND) {
    /* disable the timer that brings screen back to idle */
    (void)ISHELL_CancelTimer(pMe->a.m_pIShell, (PFNNOTIFY) AUXDisplayIdleMenuDlg, pMe);
    /* reset the idle screen timer */
    nReturnStatus = ISHELL_SetTimer(pMe->a.m_pIShell,
                    AUXAPPTIMEOUT,
                    (PFNNOTIFY) AUXDisplayIdleMenuDlg,
                    (uint32*) pMe);
    if (nReturnStatus != SUCCESS)
    {
      AUX_ERR("Fail to set AUXAPPTIMEOUT: %d", nReturnStatus, 0, 0);
    }
  }

  switch (evt) {
  case EVT_DIALOG_START:
    AUX_MSG ("RadioButnDlg: EVT_DIALOG_START", 0, 0, 0);
    return AUXInitRadioButtonDlg(pMe, w, (IDialog*)dw);

  case EVT_KEY:
    AUX_MSG ("RadioButnDlg: EVT_KEY", 0, 0, 0);
    switch(w) {
    case AVK_CLR:
      /* disable the timer that will bring the user to the previous screen */
      (void)ISHELL_CancelTimer(pMe->a.m_pIShell, (PFNNOTIFY) AUXEndCurrentDlg, pMe);

      nReturnStatus = ISHELL_EndDialog(pMe->a.m_pIShell);
      return AUXCheckAEEReturnStatus(nReturnStatus);

    /* digit keys */
    case AVK_0:
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
    case AVK_POUND:
    /* navigation keys */
    case AVK_UP:
    case AVK_DOWN:
    case AVK_LEFT:
    case AVK_RIGHT:
    case AVK_SELECT:
    /* send key */
    case AVK_SEND:
    case AVK_MESSAGE:  /* message, menu and mute not sure..!!!! */
    case AVK_MENU:
    case AVK_MUTE:
      return TRUE; /* ignore all other keys */

    case AVK_END:
    case AVK_INFO:  /* power down */
      /* disable the timer that will bring the user to the previous screen */
      (void)ISHELL_CancelTimer(pMe->a.m_pIShell, (PFNNOTIFY) AUXEndCurrentDlg, pMe);
      return FALSE;

    default:
      return FALSE;
    }

  case EVT_COMMAND:
    AUX_MSG ("RadioButnDlg: EVT_COMMAND, w=%d, dw=%d", w, dw, 0);
    /* user makes (new) selection */
    /* disable the timer that shows users their newly selected option */
    (void)ISHELL_CancelTimer(pMe->a.m_pIShell, (PFNNOTIFY) AUXEndCurrentDlg, pMe);
    pIDialog = ISHELL_GetActiveDialog(pMe->a.m_pIShell);
    if(pIDialog == NULL) {
      AUX_ERR ("Null pointer", 0,0,0);
      return FALSE;
    }

    wCtrlID = AUXGetDlgMenuCtlID(pMe, pIDialog);
    if(wCtrlID == 0) {
      AUX_ERR ("Null pointer ", 0,0,0);
      return FALSE;
    }
    pICurrentMenu= (IMenuCtl*) IDIALOG_GetControl(pIDialog, (int16)wCtrlID);
    if(pICurrentMenu == NULL) {
      AUX_ERR ("Null pointer", 0,0,0);
      return FALSE;
    }

    if (AUXUpdateSetting(pMe, w, pICurrentMenu)) {
      /* Let the update image be shown for 2 seconds before going back to
      the previous screen */
      nReturnStatus = ISHELL_SetTimer(pMe->a.m_pIShell,
                                      AUX_SELECTIONVERIFICATIONTIME,
                                      (PFNNOTIFY) AUXEndCurrentDlg,
                                      (uint32*) pMe);
      if (nReturnStatus != SUCCESS)
      {
        AUX_ERR("Fail to set AUX_SELECTIONVERIFICATIONTIME: %d", nReturnStatus, 0, 0);
      }
    }
    else {
      IDialog* pIActiveDialog;
      uint16 wDlgID;
      uint16 wStringID = IDS_CHANGE_FAILED;

      /* Display appropriate error message */
      pIActiveDialog = ISHELL_GetActiveDialog(pMe->a.m_pIShell);
      if(pIActiveDialog == NULL) {
        AUX_ERR ("Null pointer", 0,0,0);
        return FALSE;
      }

      wDlgID = IDIALOG_GetID(pIActiveDialog);
      if (wDlgID == 0) { /*error checking */
        AUX_ERR ("Null pointer", 0,0,0);
        return FALSE;
      }
      wStringID = IDS_CHANGE_FAILED;
      AUXDisplayMessageDlg(pMe, wStringID, 3);
    }
    return TRUE;   
   
  case EVT_DIALOG_END:
    AUX_MSG ("RadioButnDlg: EVT_DIALOG_END", 0, 0, 0);
    (void)ISHELL_CancelTimer(pMe->a.m_pIShell, (PFNNOTIFY) AUXDisplayIdleMenuDlg, pMe);
    nReturnStatus = ISHELL_SetTimer(pMe->a.m_pIShell,
                    AUXAPPTIMEOUT,
                    (PFNNOTIFY) AUXDisplayIdleMenuDlg,
                    (uint32*) pMe);
    if (nReturnStatus != SUCCESS)
    {
      AUX_ERR("Fail to set AUXAPPTIMEOUT: %d", nReturnStatus, 0, 0);
    }
    return TRUE;

  default: /* Other events that have not been handled by the handler */
    return FALSE;
  }
} /* End RadioButtonDlgEventHandler */

#endif /* FEATURE_WLAN || 
         (FEATURE_RUNTIME_DEVMAP && FEATURE_USB_DEFAULT_MTP) */


/*===========================================================================
FUNCTION AUXInitTimeDateDlg

DESCRIPTION
  Initialize Time/Date editing dialog

DEPENDENCIES
  None

SIDE EFFECTS
  None
===========================================================================*/
static boolean AUXInitTimeDateDlg(AUXApp *pMe)
{
  AEERect rc;
  IDateCtl* pIDate;
  ITimeCtl* pITime;
  IMenuCtl* pISoftKeyMenu;
  IDialog*  pIActiveDialog;
  JulianType date;
  int msecs;

  pIActiveDialog = ISHELL_GetActiveDialog(pMe->a.m_pIShell);

  if(pIActiveDialog == NULL)
  {
    AUX_ERR ("Null pointer", 0,0,0);
    return FALSE;
  }
  pIDate = (IDateCtl*)IDIALOG_GetControl(pIActiveDialog, IDC_SECURE_DATE);
  pITime = (ITimeCtl*)IDIALOG_GetControl(pIActiveDialog , IDC_SECURE_TIME);
  pISoftKeyMenu = (IMenuCtl*)IDIALOG_GetControl(pIActiveDialog, 
                                                IDC_SK_SETSECURETIME_EDIT);

  if ((pIDate == NULL) || (pITime == NULL) || (pISoftKeyMenu == NULL))
  {
    AUX_ERR ("Null pointer", 0,0,0);
    return FALSE;
  }

  // Set the rect for date and time
  // Hardcoded values... pretty bad !!!!
  rc.x = 2;
  rc.y = 20;
  rc.dx = (int16) pMe->m_cxScreen-5;
  rc.dy = 60;
  IDATECTL_SetRect(pIDate, &rc);
  rc.y += rc.dy;
  ITIMECTL_SetRect(pITime, &rc);

  // Setup the controls
  (void) AUXGetSecureTimeofDay (pMe, &date);

  IDATECTL_SetDate(pIDate, date.wYear, date.wMonth, date.wDay);
  msecs = ((date.wHour * 60*60)+(date.wMinute*60)+(date.wSecond))*1000;
  ITIMECTL_SetTime(pITime, msecs);

  ITIMECTL_EnableCommand(pITime, TRUE, AUX_SECURETIMEDONE);
  IDATECTL_EnableCommand(pIDate, TRUE, AUX_SECUREDATEDONE);

  IDATECTL_SetActive(pIDate, TRUE);
  ITIMECTL_SetActive(pITime, FALSE);
  IMENUCTL_SetActive(pISoftKeyMenu, FALSE);
  SetDefaultSoftkeyLook(pMe->a.m_pIShell, pISoftKeyMenu);
  IMENUCTL_Redraw(pISoftKeyMenu);
  ITIMECTL_Redraw(pITime);
  IDIALOG_SetFocus(pIActiveDialog, IDC_SECURE_DATE);
  IDATECTL_Redraw(pIDate);
  return TRUE;

}
/*===========================================================================
FUNCTION AUXTimeDateDlgEventHandler

DESCRIPTION
  time date edit dlg events

DEPENDENCIES
  None

SIDE EFFECTS
  None
===========================================================================*/
boolean AUXTimeDateDlgEventHandler(void *pUser, AEEEvent evt, uint16 w, uint32 dw)
/*lint -esym(715,dw)*/
{
  AUXApp *pMe = (AUXApp*) pUser;
  IDateCtl* pIDate;
  ITimeCtl* pITime;
  int nReturnStatus;
  IDialog*  pIActiveDialog;

  if (pMe == NULL)
  {
    AUX_ERR ("Null pointer", 0,0,0);
    return FALSE;
  }

#ifdef FEATURE_BREW_3_0
  // do not want to handle autorepeat key events
  if((evt == EVT_KEY) && ((dw & KB_AUTOREPEAT) != 0))
  {
    return FALSE;
  }
#endif

  /* Reset Timer */
  if(evt == EVT_DIALOG_START || evt == EVT_KEY || evt == EVT_COMMAND)
  {
    ISHELL_CancelTimer(pMe->a.m_pIShell, 0, pMe);
    nReturnStatus = ISHELL_SetTimer(pMe->a.m_pIShell,
                                    AUXAPPTIMEOUT,
                                    (PFNNOTIFY) AUXDisplayIdleMenuDlg,
                                    (uint32*) pMe);

    if (nReturnStatus != SUCCESS)
    {
       AUX_ERR("Fail to set AUXAPPTIMEOUT: %d", nReturnStatus, 0, 0);
    }
  }

  switch (evt) {
  case EVT_DIALOG_START:
    return AUXInitTimeDateDlg(pMe);

  case EVT_KEY:
    switch(w) {
    case AVK_CLR:
      AUXEndCurrentDlg(pMe);
      return TRUE;

    /* digit keys */
    case AVK_0:
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
    case AVK_POUND:
    /* navigation keys */
    case AVK_UP:
    case AVK_DOWN:
    case AVK_LEFT:
    case AVK_RIGHT:
    case AVK_SELECT:
    /* send key */
    case AVK_SEND:
    case AVK_MESSAGE:  /* message, menu and mute not sure..!!!! */
    case AVK_MENU:
    case AVK_MUTE:
      return TRUE; /* handled keys */

    default:
      return FALSE; /* ignored others */
    }

  case EVT_COMMAND:
    AUX_MSG ("AUXTimeDateDlgEventHandler:EVT_COMMAND, w=%d, dw=%d", w, dw, 0);
    pIActiveDialog = ISHELL_GetActiveDialog(pMe->a.m_pIShell);
    if(pIActiveDialog == NULL)
    {
      AUX_ERR ("Null pointer", 0,0,0);
      return FALSE;
    }

    pIDate = (IDateCtl*)IDIALOG_GetControl(pIActiveDialog, IDC_SECURE_DATE);
    pITime = (ITimeCtl*)IDIALOG_GetControl(pIActiveDialog , IDC_SECURE_TIME);
    if ((pIDate == NULL) || (pITime == NULL))
    {
      AUX_ERR ("Null pointer", 0,0,0);
      return FALSE;
    }

    switch(w) {
      case AUX_SECUREDATEDONE:
        /* Switch focus to time control */
        IDATECTL_SetActive(pIDate, FALSE);
        ITIMECTL_SetActive(pITime, TRUE);
        IDATECTL_Redraw(pIDate);
        IDIALOG_SetFocus(pIActiveDialog, IDC_SECURE_TIME);
        ITIMECTL_Redraw(pITime);
        return TRUE;

      case AUX_SECURETIMEDONE:
        /* Switch focus to softkey */
        IDATECTL_SetActive(pIDate, FALSE);
        ITIMECTL_SetActive(pITime, FALSE);
        ITIMECTL_Redraw(pITime);
        IDATECTL_Redraw(pIDate);
        IDIALOG_SetFocus(pIActiveDialog, IDC_SK_SETSECURETIME_EDIT);
        return TRUE;

      case IDL_SETSECURETIME_EDIT_OK:
        /* Save time and date */
        {
          JulianType date;
          int minutes;
          int year, month, day;

          IDATECTL_GetDate(pIDate, &year, &month, &day);
          date.wYear = (uint16) year;
          date.wMonth = (uint16) month;
          date.wDay = (uint16) day;
          date.wWeekDay = IDATECTL_GetDayOfWeek(pIDate);

          minutes = (ITIMECTL_GetTime(pITime))/(60*1000);
          if (minutes > 0)
          {
            date.wHour = (unsigned short) minutes/60;
            date.wMinute = (unsigned short) minutes%60;
          }
          date.wSecond = 0;
          (void) AUXSetSecureTimeofDay(pMe, &date);
          AUXPopDlg(pMe, IDD_SETSECURETIME_VIEW);
          AUXEndCurrentDlg(pMe);
          return TRUE;
        }

      default:
        break;
    }
    return FALSE;

  case EVT_DIALOG_END:
    return TRUE;

  default: // Other events that have not been handled by the handler
    return FALSE;
  }
}


