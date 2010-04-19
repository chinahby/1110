/*===========================================================================

FILE: NetMenu.c

DESCRIPTION
   This file contains the Net app's menu related functions.

PUBLIC CLASSES:
   N/A

INITIALIZATION AND SEQUENCING REQUIREMENTS:
   Functions in this file is being called by NetApp.c.

       Copyright   2007 QUALCOMM Incorporated.
                    All Rights Reserved.
                   QUALCOMM Proprietary/GTDR
===========================================================================*/
/*===========================================================================
                        EDIT HISTORY FOR MODULE

$Header: //depot/asic/msmshared/apps/NetSettings/NetMenu.c#8 $

when       who     what, where, why
--------   ---     ----------------------------------------------------------
01/10/08   kac     Added GSM+WCDMA capability
06/04/07   ypmw    Remove PLMN manual selection for HDK build
11/08/06   alb     Added band class 15/16 support.
09/14/06   jas     Adding query of a config item regarding display of manual
                   PLMN search option.
09/01/06   jas     Complete the renaming of symbols formerly in CoreApp
09/01/06   jas     Resolving name collision with CoreApp
06/18/06   joe     Initial revision
===========================================================================*/


/*===============================================================================

                INCLUDES AND VARIABLE DEFINITIONS

=============================================================================== */
#include "OEMFeatures.h"
#include "AEEStdLib.h"    /* contain FREE function */
#include "AEEAppGen.h"    /* Applet helper file */
#include "AEEMenu.h"
#include "AEEShell.h"
#include "AEEText.h"
#include "AEEDate.h"
#include "AEETime.h"
#include "AEEFile.h"          // AEE File Manager Services
#include "AEENet.h"

#include "NetMenu.h"
#include "netsettings.brh"
#include "NetUtils.h"

#include "OEMFeatures.h"
#include "BREWVersion.h"

#include "AppComFunc.h"

// DMSS includes
#include "sdevmenu.h"
#include "sdevmap.h"
#include "rdevmap.h"
#include "mobile.h"

#include "ui_base.h"
#include "uiutils.h"

#define TEXT_BUFFER_SIZE  400   // Size of buffer used to hold text strings
#define TITLE_BUFFER_SIZE  100
#define MAX_STR_SIZE 50

/* Valid action types */
#define LAUNCH_DIALOG 0
#define LAUNCH_APPLET 1
#define MENU_SETTING  2

typedef uint64 (GetFnPtrType)(CNetApp*);
typedef boolean (SetFnPtrType)(CNetApp*, uint64);

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

static const MenuDefinitionType NetMenuActionTbl[] = {
  /* Network Menu */
  {IDD_NETWORK, IDC_MENU_NETWORK, IDL_MODE_PREF, LAUNCH_DIALOG, IDD_NETWORK_FORCE_MODE,NULL, NULL},
  {IDD_NETWORK, IDC_MENU_NETWORK, IDL_BAND_PREF, LAUNCH_DIALOG, IDD_NETWORK_BAND_PREF,NULL, NULL},
  {IDD_NETWORK, IDC_MENU_NETWORK, IDL_ROAM_PREF, LAUNCH_DIALOG, IDD_NETWORK_ROAM_PREF,NULL, NULL},
#ifndef FEATURE_UIONE_HDK
  {IDD_NETWORK, IDC_MENU_NETWORK, IDL_NETSELECT_PREF, LAUNCH_DIALOG, IDD_NETWORK_SELECT,NULL, NULL},
#endif /* #ifndef FEATURE_UIONE_HDK */
  {IDD_NETWORK, IDC_MENU_NETWORK, IDL_HYBRID_PREF, LAUNCH_DIALOG, IDD_NETWORK_HYBR_PREF,NULL, NULL},
  {IDD_NETWORK, IDC_MENU_NETWORK, IDL_ACQORDER_PREF, LAUNCH_DIALOG, IDD_NETWORK_ACQ_PREF,NULL, NULL},
  {IDD_NETWORK, IDC_MENU_NETWORK, IDL_SERVDOMAIN_PREF, LAUNCH_DIALOG, IDD_NETWORK_DOMAIN_PREF,NULL, NULL},
  {IDD_NETWORK, IDC_MENU_NETWORK, IDL_SET_NAM, LAUNCH_DIALOG, IDD_NETWORK_SET_NAM,NULL, NULL},
  {IDD_NETWORK, IDC_MENU_NETWORK, IDL_AUTO_NAM, LAUNCH_DIALOG, IDD_NETWORK_AUTO_NAM,NULL, NULL},
  {IDD_NETWORK, IDC_MENU_NETWORK, IDL_DATA_FAX, LAUNCH_DIALOG,  IDD_NETWORK_DATA_FAX,NULL, NULL},
  {IDD_NETWORK, IDC_MENU_NETWORK, IDL_M51, LAUNCH_DIALOG, IDD_NETWORK_M51,NULL,NULL},
  {IDD_NETWORK, IDC_MENU_NETWORK, IDL_AVOID_CURR_SYS, LAUNCH_DIALOG, IDD_NETWORK_AVOID_SERVING_SYS,NULL,NULL},
  {IDD_NETWORK, IDC_MENU_NETWORK, IDL_WAKEUP_1X, LAUNCH_DIALOG, IDD_NETWORK_WAKEUP_1X,NULL,NULL},
  {IDD_NETWORK, IDC_MENU_NETWORK, IDL_WAKEUP_HDR, LAUNCH_DIALOG, IDD_NETWORK_WAKEUP_HDR,NULL,NULL},

#ifdef FEATURE_JCDMA_1X
  /* M51 menu */
  {IDD_NETWORK_M51, IDC_MENU_NETWORK_M51, IDL_NETWORK_M511_MODE, LAUNCH_DIALOG, IDD_NETWORK_M511_MODE,NULL, NULL},
  {IDD_NETWORK_M51, IDC_MENU_NETWORK_M51, IDL_NETWORK_M512_MODE, LAUNCH_DIALOG, IDD_NETWORK_M512_MODE,NULL, NULL},
  {IDD_NETWORK_M51, IDC_MENU_NETWORK_M51, IDL_NETWORK_M513_MODE, LAUNCH_DIALOG, IDD_NETWORK_M513_MODE,NULL, NULL},

  /* M511 mode */
  {IDD_NETWORK_M511_MODE, IDC_MENU_NETWORK_M511_MODE, IDL_NETWORK_M511_MODE_PACKET, MENU_SETTING,
   SYS_JCDMA_M511_PKT, NetGetM511Mode, NetSetM511Mode},
  {IDD_NETWORK_M511_MODE, IDC_MENU_NETWORK_M511_MODE, IDL_NETWORK_M511_MODE_ASYNC_FAX, MENU_SETTING,
   SYS_JCDMA_M511_ASYNC_FAX, NetGetM511Mode, NetSetM511Mode},

  /* M512 mode */
  {IDD_NETWORK_M512_MODE, IDC_MENU_NETWORK_M512_MODE, IDL_NETWORK_M512_MODE_STD, MENU_SETTING,
   SYS_JCDMA_M512_STD, NetGetM512Mode, NetSetM512Mode},
  {IDD_NETWORK_M512_MODE, IDC_MENU_NETWORK_M512_MODE, IDL_NETWORK_M512_MODE_HIGH, MENU_SETTING,
   SYS_JCDMA_M512_HIGH, NetGetM512Mode, NetSetM512Mode},

  /* M513 mode */
  {IDD_NETWORK_M513_MODE, IDC_MENU_NETWORK_M513_MODE, IDL_NETWORK_M513_MODE_AUTO, MENU_SETTING,
   SYS_JCDMA_M513_AUTO, NetGetM513Mode, NetSetM513Mode},
  {IDD_NETWORK_M513_MODE, IDC_MENU_NETWORK_M513_MODE, IDL_NETWORK_M513_MODE_19200, MENU_SETTING,
   SYS_JCDMA_M513_AUTO, NetGetM513Mode, NetSetM513Mode},
  {IDD_NETWORK_M513_MODE, IDC_MENU_NETWORK_M513_MODE, IDL_NETWORK_M513_MODE_115200, MENU_SETTING,
   SYS_JCDMA_M513_115200, NetGetM513Mode, NetSetM513Mode},
  {IDD_NETWORK_M513_MODE, IDC_MENU_NETWORK_M513_MODE, IDL_NETWORK_M513_MODE_230400, MENU_SETTING,
   SYS_JCDMA_M513_230400, NetGetM513Mode, NetSetM513Mode},
#endif

  /* Mode Pref */
#ifdef FEATURE_MMGSDI_DUAL_SLOT
  { IDD_NETWORK_FORCE_MODE, IDC_MENU_NETWORK_FORCE_MODE, IDL_MODE_AUTOMATIC, MENU_SETTING,
   AEECM_MODE_PREF_AUTOMATIC, NetGetForceModePref, NetSetForceModePrefSlotCheck},
  {IDD_NETWORK_FORCE_MODE, IDC_MENU_NETWORK_FORCE_MODE, IDL_MODE_AMPS_ONLY, MENU_SETTING,
   AEECM_MODE_PREF_AMPS_ONLY, NetGetForceModePref, NetSetForceModePrefSlotCheck},
  {IDD_NETWORK_FORCE_MODE, IDC_MENU_NETWORK_FORCE_MODE, IDL_MODE_HDR_ONLY, MENU_SETTING,
   AEECM_MODE_PREF_HDR_ONLY, NetGetForceModePref, NetSetForceModePrefSlotCheck},
  {IDD_NETWORK_FORCE_MODE, IDC_MENU_NETWORK_FORCE_MODE, IDL_MODE_CDMA_AMPS, MENU_SETTING,
   AEECM_MODE_PREF_CDMA_AMPS_ONLY, NetGetForceModePref, NetSetForceModePrefSlotCheck},
  {IDD_NETWORK_FORCE_MODE, IDC_MENU_NETWORK_FORCE_MODE, IDL_MODE_CDMA_ONLY, MENU_SETTING,
   AEECM_MODE_PREF_CDMA_ONLY, NetGetForceModePref, NetSetForceModePrefSlotCheck},
  {IDD_NETWORK_FORCE_MODE, IDC_MENU_NETWORK_FORCE_MODE, IDL_MODE_EMERGENCY_ONLY, MENU_SETTING,
   AEECM_MODE_PREF_EMERGENCY, NetGetForceModePref, NetSetForceModePrefSlotCheck},
  {IDD_NETWORK_FORCE_MODE, IDC_MENU_NETWORK_FORCE_MODE, IDL_MODE_CDMA_HDR_ONLY, MENU_SETTING,
   AEECM_MODE_PREF_CDMA_HDR_ONLY, NetGetForceModePref, NetSetForceModePrefSlotCheck},
  {IDD_NETWORK_FORCE_MODE, IDC_MENU_NETWORK_FORCE_MODE, IDL_MODE_GSM_ONLY, MENU_SETTING,
   AEECM_MODE_PREF_GSM_ONLY, NetGetForceModePref, NetSetForceModePrefSlotCheck},
  {IDD_NETWORK_FORCE_MODE, IDC_MENU_NETWORK_FORCE_MODE, IDL_MODE_DIGITAL_ONLY, MENU_SETTING,
   AEECM_MODE_PREF_DIGITAL_ONLY, NetGetForceModePref, NetSetForceModePrefSlotCheck},
  {IDD_NETWORK_FORCE_MODE, IDC_MENU_NETWORK_FORCE_MODE, IDL_MODE_WCDMA_ONLY, MENU_SETTING,
   AEECM_MODE_PREF_WCDMA_ONLY, NetGetForceModePref, NetSetForceModePrefSlotCheck},
  {IDD_NETWORK_FORCE_MODE, IDC_MENU_NETWORK_FORCE_MODE, IDL_MODE_GSM_WCDMA_ONLY, MENU_SETTING,
   AEECM_MODE_PREF_GSM_WCDMA_ONLY, NetGetForceModePref, NetSetForceModePrefSlotCheck},
#ifdef FEATURE_WLAN
  {IDD_NETWORK_FORCE_MODE, IDC_MENU_NETWORK_FORCE_MODE, IDL_MODE_WLAN_ONLY, MENU_SETTING,
   AEECM_MODE_PREF_WLAN_ONLY, NetGetForceModePref, NetSetForceModePrefSlotCheck},
  {IDD_NETWORK_FORCE_MODE, IDC_MENU_NETWORK_FORCE_MODE, IDL_MODE_CDMA_WLAN, MENU_SETTING,
   AEECM_MODE_PREF_CDMA_WLAN, NetGetForceModePref, NetSetForceModePrefSlotCheck},
  {IDD_NETWORK_FORCE_MODE, IDC_MENU_NETWORK_FORCE_MODE, IDL_MODE_CDMA_HDR_WLAN, MENU_SETTING,
   AEECM_MODE_PREF_CDMA_HDR_WLAN, NetGetForceModePref, NetSetForceModePrefSlotCheck},
  {IDD_NETWORK_FORCE_MODE, IDC_MENU_NETWORK_FORCE_MODE, IDL_MODE_HDR_WLAN, MENU_SETTING,
   AEECM_MODE_PREF_HDR_WLAN, NetGetForceModePref, NetSetForceModePrefSlotCheck},
  {IDD_NETWORK_FORCE_MODE, IDC_MENU_NETWORK_FORCE_MODE, IDL_MODE_GSM_WLAN, MENU_SETTING,
   AEECM_MODE_PREF_GSM_WLAN, NetGetForceModePref, NetSetForceModePrefSlotCheck},
  {IDD_NETWORK_FORCE_MODE, IDC_MENU_NETWORK_FORCE_MODE, IDL_MODE_WCDMA_WLAN, MENU_SETTING,
   AEECM_MODE_PREF_WCDMA_WLAN, NetGetForceModePref, NetSetForceModePrefSlotCheck},
  {IDD_NETWORK_FORCE_MODE, IDC_MENU_NETWORK_FORCE_MODE, IDL_MODE_GSM_WCDMA_WLAN, MENU_SETTING,
   AEECM_MODE_PREF_GW_WLAN, NetGetForceModePref, NetSetForceModePrefSlotCheck},
#endif // FEATURE_WLAN
#else
  {IDD_NETWORK_FORCE_MODE, IDC_MENU_NETWORK_FORCE_MODE, IDL_MODE_AUTOMATIC, MENU_SETTING,
   AEECM_MODE_PREF_AUTOMATIC, NetGetForceModePref, NetSetForceModePref},
  {IDD_NETWORK_FORCE_MODE, IDC_MENU_NETWORK_FORCE_MODE, IDL_MODE_AMPS_ONLY, MENU_SETTING,
   AEECM_MODE_PREF_AMPS_ONLY, NetGetForceModePref, NetSetForceModePref},
  {IDD_NETWORK_FORCE_MODE, IDC_MENU_NETWORK_FORCE_MODE, IDL_MODE_HDR_ONLY, MENU_SETTING,
   AEECM_MODE_PREF_HDR_ONLY, NetGetForceModePref, NetSetForceModePref},
  {IDD_NETWORK_FORCE_MODE, IDC_MENU_NETWORK_FORCE_MODE, IDL_MODE_CDMA_AMPS, MENU_SETTING,
   AEECM_MODE_PREF_CDMA_AMPS_ONLY, NetGetForceModePref, NetSetForceModePref},
  {IDD_NETWORK_FORCE_MODE, IDC_MENU_NETWORK_FORCE_MODE, IDL_MODE_CDMA_ONLY, MENU_SETTING,
   AEECM_MODE_PREF_CDMA_ONLY, NetGetForceModePref, NetSetForceModePref},
  {IDD_NETWORK_FORCE_MODE, IDC_MENU_NETWORK_FORCE_MODE, IDL_MODE_EMERGENCY_ONLY, MENU_SETTING,
   AEECM_MODE_PREF_EMERGENCY, NetGetForceModePref, NetSetForceModePref},
  {IDD_NETWORK_FORCE_MODE, IDC_MENU_NETWORK_FORCE_MODE, IDL_MODE_CDMA_HDR_ONLY, MENU_SETTING,
   AEECM_MODE_PREF_CDMA_HDR_ONLY, NetGetForceModePref, NetSetForceModePref},
  {IDD_NETWORK_FORCE_MODE, IDC_MENU_NETWORK_FORCE_MODE, IDL_MODE_GSM_ONLY, MENU_SETTING,
   AEECM_MODE_PREF_GSM_ONLY, NetGetForceModePref, NetSetForceModePref},
  {IDD_NETWORK_FORCE_MODE, IDC_MENU_NETWORK_FORCE_MODE, IDL_MODE_DIGITAL_ONLY, MENU_SETTING,
   AEECM_MODE_PREF_DIGITAL_ONLY, NetGetForceModePref, NetSetForceModePref},
  {IDD_NETWORK_FORCE_MODE, IDC_MENU_NETWORK_FORCE_MODE, IDL_MODE_WCDMA_ONLY, MENU_SETTING,
   AEECM_MODE_PREF_WCDMA_ONLY, NetGetForceModePref, NetSetForceModePref},
  {IDD_NETWORK_FORCE_MODE, IDC_MENU_NETWORK_FORCE_MODE, IDL_MODE_GSM_WCDMA_ONLY, MENU_SETTING,
   AEECM_MODE_PREF_GSM_WCDMA_ONLY, NetGetForceModePref, NetSetForceModePref},
#ifdef FEATURE_WLAN
  {IDD_NETWORK_FORCE_MODE, IDC_MENU_NETWORK_FORCE_MODE, IDL_MODE_WLAN_ONLY, MENU_SETTING,
   AEECM_MODE_PREF_WLAN_ONLY, NetGetForceModePref, NetSetForceModePref},
  {IDD_NETWORK_FORCE_MODE, IDC_MENU_NETWORK_FORCE_MODE, IDL_MODE_CDMA_WLAN, MENU_SETTING,
   AEECM_MODE_PREF_CDMA_WLAN, NetGetForceModePref, NetSetForceModePref},
  {IDD_NETWORK_FORCE_MODE, IDC_MENU_NETWORK_FORCE_MODE, IDL_MODE_CDMA_HDR_WLAN, MENU_SETTING,
   AEECM_MODE_PREF_CDMA_HDR_WLAN, NetGetForceModePref, NetSetForceModePref},
  {IDD_NETWORK_FORCE_MODE, IDC_MENU_NETWORK_FORCE_MODE, IDL_MODE_HDR_WLAN, MENU_SETTING,
   AEECM_MODE_PREF_HDR_WLAN, NetGetForceModePref, NetSetForceModePref},
  {IDD_NETWORK_FORCE_MODE, IDC_MENU_NETWORK_FORCE_MODE, IDL_MODE_GSM_WLAN, MENU_SETTING,
   AEECM_MODE_PREF_GSM_WLAN, NetGetForceModePref, NetSetForceModePref},
  {IDD_NETWORK_FORCE_MODE, IDC_MENU_NETWORK_FORCE_MODE, IDL_MODE_WCDMA_WLAN, MENU_SETTING,
   AEECM_MODE_PREF_WCDMA_WLAN, NetGetForceModePref, NetSetForceModePref},
  {IDD_NETWORK_FORCE_MODE, IDC_MENU_NETWORK_FORCE_MODE, IDL_MODE_GSM_WCDMA_WLAN, MENU_SETTING,
   AEECM_MODE_PREF_GW_WLAN, NetGetForceModePref, NetSetForceModePref},
#endif // FEATURE_WLAN
#endif/*!FEATURE_MMGSDI_DUAL_SLOT */
  /* Band Pref */
//lint -save -e799  constant larger than unsigned long
  {IDD_NETWORK_BAND_PREF, IDC_BAND_PREF, IDL_BAND_ANY, MENU_SETTING,
   AEECM_BAND_PREF_ANY, NetGetBandPref, NetSetBandPref},
  {IDD_NETWORK_BAND_PREF, IDC_BAND_PREF, IDL_CELL_A, MENU_SETTING,
   AEECM_BAND_PREF_CELL_A, NetGetBandPref, NetSetBandPref},
  {IDD_NETWORK_BAND_PREF, IDC_BAND_PREF, IDL_CELL_B, MENU_SETTING,
   AEECM_BAND_PREF_CELL_B, NetGetBandPref, NetSetBandPref},
  {IDD_NETWORK_BAND_PREF, IDC_BAND_PREF, IDL_CELL_ONLY, MENU_SETTING,
   AEECM_BAND_PREF_CELL, NetGetBandPref, NetSetBandPref},
  {IDD_NETWORK_BAND_PREF, IDC_BAND_PREF, IDL_PCS_ONLY, MENU_SETTING,
   AEECM_BAND_PREF_PCS, NetGetBandPref, NetSetBandPref},
  {IDD_NETWORK_BAND_PREF, IDC_BAND_PREF, IDL_BC3, MENU_SETTING,
   AEECM_BAND_PREF_BC3, NetGetBandPref, NetSetBandPref},
  {IDD_NETWORK_BAND_PREF, IDC_BAND_PREF, IDL_BC4, MENU_SETTING,
   AEECM_BAND_PREF_BC4, NetGetBandPref, NetSetBandPref},
  {IDD_NETWORK_BAND_PREF, IDC_BAND_PREF, IDL_BC5, MENU_SETTING,
   AEECM_BAND_PREF_BC5, NetGetBandPref, NetSetBandPref},
  {IDD_NETWORK_BAND_PREF, IDC_BAND_PREF, IDL_BC6, MENU_SETTING,
   AEECM_BAND_PREF_BC6, NetGetBandPref, NetSetBandPref},
  {IDD_NETWORK_BAND_PREF, IDC_BAND_PREF, IDL_BC7, MENU_SETTING,
   AEECM_BAND_PREF_BC7, NetGetBandPref, NetSetBandPref},
  {IDD_NETWORK_BAND_PREF, IDC_BAND_PREF, IDL_BC8, MENU_SETTING,
   AEECM_BAND_PREF_BC8, NetGetBandPref, NetSetBandPref},
  {IDD_NETWORK_BAND_PREF, IDC_BAND_PREF, IDL_BC9, MENU_SETTING,
   AEECM_BAND_PREF_BC9, NetGetBandPref, NetSetBandPref},
  {IDD_NETWORK_BAND_PREF, IDC_BAND_PREF, IDL_BC10, MENU_SETTING,
   AEECM_BAND_PREF_BC10, NetGetBandPref, NetSetBandPref},
  {IDD_NETWORK_BAND_PREF, IDC_BAND_PREF, IDL_BC11, MENU_SETTING,
   AEECM_BAND_PREF_BC11, NetGetBandPref, NetSetBandPref},
  {IDD_NETWORK_BAND_PREF, IDC_BAND_PREF, IDL_BC15, MENU_SETTING,
   AEECM_BAND_PREF_BC15, NetGetBandPref, NetSetBandPref},
  {IDD_NETWORK_BAND_PREF, IDC_BAND_PREF, IDL_BC16, MENU_SETTING,
   AEECM_BAND_PREF_BC16, NetGetBandPref, NetSetBandPref},
#if defined (FEATURE_WCDMA) || defined (FEATURE_GSM)
  {IDD_NETWORK_BAND_PREF, IDC_BAND_PREF, IDL_GSM_PCS_1900, MENU_SETTING,
   AEECM_BAND_PREF_GSM_PCS_1900, NetGetBandPref, NetSetBandPref},
  {IDD_NETWORK_BAND_PREF, IDC_BAND_PREF, IDL_GSM_750, MENU_SETTING,
   AEECM_BAND_PREF_GSM_750, NetGetBandPref, NetSetBandPref},
  {IDD_NETWORK_BAND_PREF, IDC_BAND_PREF, IDL_GSM_DCS_1800, MENU_SETTING,
   AEECM_BAND_PREF_GSM_DCS_1800, NetGetBandPref, NetSetBandPref},
  {IDD_NETWORK_BAND_PREF, IDC_BAND_PREF, IDL_GSM_EGSM_900, MENU_SETTING,
   AEECM_BAND_PREF_GSM_EGSM_900, NetGetBandPref, NetSetBandPref},
  {IDD_NETWORK_BAND_PREF, IDC_BAND_PREF, IDL_GSM_PGSM_900, MENU_SETTING,
   AEECM_BAND_PREF_GSM_PGSM_900, NetGetBandPref, NetSetBandPref},
  {IDD_NETWORK_BAND_PREF, IDC_BAND_PREF, IDL_GSM_850, MENU_SETTING,
   AEECM_BAND_PREF_GSM_850, NetGetBandPref, NetSetBandPref},
  {IDD_NETWORK_BAND_PREF, IDC_BAND_PREF, IDL_GSM_RGSM_900, MENU_SETTING,
   AEECM_BAND_PREF_GSM_RGSM_900, NetGetBandPref, NetSetBandPref},
  {IDD_NETWORK_BAND_PREF, IDC_BAND_PREF, IDL_GSM_450, MENU_SETTING,
   AEECM_BAND_PREF_GSM_450, NetGetBandPref, NetSetBandPref},
  {IDD_NETWORK_BAND_PREF, IDC_BAND_PREF, IDL_GSM_480, MENU_SETTING,
   AEECM_BAND_PREF_GSM_480, NetGetBandPref, NetSetBandPref},
  {IDD_NETWORK_BAND_PREF, IDC_BAND_PREF, IDL_WCDMA_PCS_1900, MENU_SETTING,
   AEECM_BAND_PREF_WCDMA_II_PCS_1900, NetGetBandPref, NetSetBandPref},
  {IDD_NETWORK_BAND_PREF, IDC_BAND_PREF, IDL_WCDMA_IMT_2000, MENU_SETTING,
   AEECM_BAND_PREF_WCDMA_I_IMT_2000, NetGetBandPref, NetSetBandPref},
  {IDD_NETWORK_BAND_PREF, IDC_BAND_PREF, IDL_WCDMA_III_1700, MENU_SETTING,
   AEECM_BAND_PREF_WCDMA_III_1700, NetGetBandPref, NetSetBandPref},
  {IDD_NETWORK_BAND_PREF, IDC_BAND_PREF, IDL_WCDMA_IV_1700, MENU_SETTING,
   AEECM_BAND_PREF_WCDMA_IV_1700, NetGetBandPref, NetSetBandPref},
  {IDD_NETWORK_BAND_PREF, IDC_BAND_PREF, IDL_WCDMA_850, MENU_SETTING,
   AEECM_BAND_PREF_WCDMA_V_850, NetGetBandPref, NetSetBandPref},
  {IDD_NETWORK_BAND_PREF, IDC_BAND_PREF, IDL_WCDMA_800, MENU_SETTING,
   AEECM_BAND_PREF_WCDMA_VI_800, NetGetBandPref, NetSetBandPref},
  {IDD_NETWORK_BAND_PREF, IDC_BAND_PREF, IDL_WCDMA_VII_2600, MENU_SETTING,
   AEECM_BAND_PREF_WCDMA_VII_2600, NetGetBandPref, NetSetBandPref},
  {IDD_NETWORK_BAND_PREF, IDC_BAND_PREF, IDL_WCDMA_VIII_900, MENU_SETTING,
   AEECM_BAND_PREF_WCDMA_VIII_900, NetGetBandPref, NetSetBandPref},
  {IDD_NETWORK_BAND_PREF, IDC_BAND_PREF, IDL_WCDMA_IX_1700, MENU_SETTING,
   AEECM_BAND_PREF_WCDMA_IX_1700, NetGetBandPref, NetSetBandPref},

#endif /* WCDMA || GSM */

#ifdef FEATURE_WLAN
  {IDD_NETWORK_BAND_PREF, IDC_BAND_PREF, IDL_WLAN_2400_US, MENU_SETTING,
   AEECM_BAND_PREF_WLAN_2400_US, NetGetBandPref, NetSetBandPref},
  {IDD_NETWORK_BAND_PREF, IDC_BAND_PREF, IDL_WLAN_2400_JAPAN, MENU_SETTING,
   AEECM_BAND_PREF_WLAN_2400_JP, NetGetBandPref, NetSetBandPref},
  {IDD_NETWORK_BAND_PREF, IDC_BAND_PREF, IDL_WLAN_2400_EUROPE, MENU_SETTING,
   AEECM_BAND_PREF_WLAN_2400_ETSI, NetGetBandPref, NetSetBandPref},
  {IDD_NETWORK_BAND_PREF, IDC_BAND_PREF, IDL_WLAN_2400_SPAIN, MENU_SETTING,
   AEECM_BAND_PREF_WLAN_2400_SP, NetGetBandPref, NetSetBandPref},
  {IDD_NETWORK_BAND_PREF, IDC_BAND_PREF, IDL_WLAN_2400_FRANCE, MENU_SETTING,
   AEECM_BAND_PREF_WLAN_2400_FR, NetGetBandPref, NetSetBandPref},
  {IDD_NETWORK_BAND_PREF, IDC_BAND_PREF, IDL_WLAN_5000_US, MENU_SETTING,
   AEECM_BAND_PREF_WLAN_5000_US, NetGetBandPref, NetSetBandPref},
  {IDD_NETWORK_BAND_PREF, IDC_BAND_PREF, IDL_WLAN_5000_JAPAN, MENU_SETTING,
   AEECM_BAND_PREF_WLAN_5000_JP, NetGetBandPref, NetSetBandPref},
  {IDD_NETWORK_BAND_PREF, IDC_BAND_PREF, IDL_WLAN_5000_EUROPE, MENU_SETTING,
   AEECM_BAND_PREF_WLAN_5000_ETSI, NetGetBandPref, NetSetBandPref},
  {IDD_NETWORK_BAND_PREF, IDC_BAND_PREF, IDL_WLAN_5000_SPAIN, MENU_SETTING,
   AEECM_BAND_PREF_WLAN_5000_SP, NetGetBandPref, NetSetBandPref},
  {IDD_NETWORK_BAND_PREF, IDC_BAND_PREF, IDL_WLAN_5000_FRANCE, MENU_SETTING,
   AEECM_BAND_PREF_WLAN_5000_FR, NetGetBandPref, NetSetBandPref},
  {IDD_NETWORK_BAND_PREF, IDC_BAND_PREF, IDL_WLAN_ANY, MENU_SETTING,
   AEECM_BAND_PREF_WLAN_ANY, NetGetBandPref, NetSetBandPref},
#endif // FEATURE_WLAN
//lint -restore

  /* Roam Pref */
  {IDD_NETWORK_ROAM_PREF, IDC_ROAM_PREF, IDL_ROAM_HOME, MENU_SETTING,
   AEECM_ROAM_PREF_HOME, NetGetRoamPref, NetSetRoamPref},
  {IDD_NETWORK_ROAM_PREF, IDC_ROAM_PREF, IDL_ROAM_AFFIL, MENU_SETTING,
   AEECM_ROAM_PREF_AFFIL, NetGetRoamPref, NetSetRoamPref},
  {IDD_NETWORK_ROAM_PREF, IDC_ROAM_PREF, IDL_ROAM_ANY, MENU_SETTING,
   AEECM_ROAM_PREF_ANY, NetGetRoamPref, NetSetRoamPref},

#if defined (FEATURE_WCDMA) || defined (FEATURE_GSM)
#ifndef FEATURE_UIONE_HDK
  /* Network select */
  {IDD_NETWORK_SELECT, IDC_MENU_NETWORK_SELECT, IDL_NETSELECT_AUTOMATIC, MENU_SETTING,
   AEECM_NETWORK_SEL_MODE_PREF_AUTOMATIC, NetGetNetworkSelect, NULL},
  {IDD_NETWORK_SELECT, IDC_MENU_NETWORK_SELECT, IDL_NETSELECT_MANUAL, MENU_SETTING,
   AEECM_NETWORK_SEL_MODE_PREF_MANUAL, NetGetNetworkSelect, NULL},
  {IDD_NETWORK_SELECT, IDC_MENU_NETWORK_SELECT, IDL_NETSELECT_MANUAL_WRAT, MENU_SETTING,
   AEECM_NETWORK_SEL_MODE_PREF_MANUAL, NetGetNetworkSelect, NULL},
#endif /* #ifndef FEATURE_UIONE_HDK */
#endif /*#if defined (FEATURE_WCDMA) || defined (FEATURE_GSM)*/

  /* Hybrid Pref */
  {IDD_NETWORK_HYBR_PREF, IDC_HYBR_PREF, IDL_HYBR_ON, MENU_SETTING,
   AEECM_HYBR_PREF_ON, NetGetHybridPref, NetSetHybridPref},
  {IDD_NETWORK_HYBR_PREF, IDC_HYBR_PREF, IDL_HYBR_OFF, MENU_SETTING,
   AEECM_HYBR_PREF_OFF, NetGetHybridPref, NetSetHybridPref},

  /* Acq Order Pref */
  {IDD_NETWORK_ACQ_PREF, IDC_ACQ_ORDER_PREF, IDL_ACQ_AUTO, MENU_SETTING,
   AEECM_GW_ACQ_ORDER_PREF_AUTOMATIC, NetGetAcqOrderPref, NetSetAcqOrderPref},
  {IDD_NETWORK_ACQ_PREF, IDC_ACQ_ORDER_PREF, IDL_GSMWCDMA, MENU_SETTING,
   AEECM_GW_ACQ_ORDER_PREF_GSM_WCDMA, NetGetAcqOrderPref, NetSetAcqOrderPref},
  {IDD_NETWORK_ACQ_PREF, IDC_ACQ_ORDER_PREF, IDL_WCDMAGSM, MENU_SETTING,
   AEECM_GW_ACQ_ORDER_PREF_WCDMA_GSM, NetGetAcqOrderPref, NetSetAcqOrderPref},

  /* Service Domain Pref */
  {IDD_NETWORK_DOMAIN_PREF, IDC_DOMAIN_PREF, IDL_DOMAIN_CS, MENU_SETTING,
   AEECM_SRV_DOMAIN_PREF_CS_ONLY, NetGetDomainPref, NetSetDomainPref},
  {IDD_NETWORK_DOMAIN_PREF, IDC_DOMAIN_PREF, IDL_DOMAIN_PS, MENU_SETTING,
   AEECM_SRV_DOMAIN_PREF_PS_ONLY, NetGetDomainPref, NetSetDomainPref},
  {IDD_NETWORK_DOMAIN_PREF, IDC_DOMAIN_PREF, IDL_CS_PS, MENU_SETTING,
   AEECM_SRV_DOMAIN_PREF_CS_PS, NetGetDomainPref, NetSetDomainPref},

  /* Auto NAM */
  {IDD_NETWORK_AUTO_NAM, IDC_MENU_NETWORK_AUTO_NAM, IDL_NETWORK_AUTO_NAM_ON, MENU_SETTING,
   TRUE, NetGetAutoNAM, NetSetAutoNAM},
  {IDD_NETWORK_AUTO_NAM, IDC_MENU_NETWORK_AUTO_NAM, IDL_NETWORK_AUTO_NAM_OFF, MENU_SETTING,
   FALSE, NetGetAutoNAM, NetSetAutoNAM},

  /* Set NAM */
  {IDD_NETWORK_SET_NAM, IDC_MENU_NETWORK_SET_NAM, IDL_NETWORK_SET_NAM1, MENU_SETTING,
   AEECM_NAM_1, NetGetNetworkNAMSettings, NetSetNetworkCurrentNAM},
  {IDD_NETWORK_SET_NAM, IDC_MENU_NETWORK_SET_NAM, IDL_NETWORK_SET_NAM2, MENU_SETTING,
   AEECM_NAM_2, NetGetNetworkNAMSettings, NetSetNetworkCurrentNAM},
  {IDD_NETWORK_SET_NAM, IDC_MENU_NETWORK_SET_NAM, IDL_NETWORK_SET_NAM3, MENU_SETTING,
   AEECM_NAM_3, NetGetNetworkNAMSettings, NetSetNetworkCurrentNAM},
  {IDD_NETWORK_SET_NAM, IDC_MENU_NETWORK_SET_NAM, IDL_NETWORK_SET_NAM4, MENU_SETTING,
   AEECM_NAM_4, NetGetNetworkNAMSettings, NetSetNetworkCurrentNAM},

  /* Data Fax */
  {IDD_NETWORK_DATA_FAX, IDC_MENU_NETWORK_DATA_FAX, IDL_NETWORK_DATA_FAX_OFF, MENU_SETTING,
   AEECM_ANSWER_VOICE_AS_VOICE, NetGetNetworkDataFax, NetSetNetworkDataFax},
  {IDD_NETWORK_DATA_FAX, IDC_MENU_NETWORK_DATA_FAX, IDL_NETWORK_DATA_FAX_FOR_NEXT_CALL, MENU_SETTING,
   AEECM_ANSWER_VOICE_AS_FAX_ONCE, NetGetNetworkDataFax, NetSetNetworkDataFax},
  {IDD_NETWORK_DATA_FAX, IDC_MENU_NETWORK_DATA_FAX, IDL_NETWORK_DATA_FAX_UNTIL_POWERED_OFF, MENU_SETTING,
   AEECM_ANSWER_VOICE_AS_FAX_ALWAYS, NetGetNetworkDataFax, NetSetNetworkDataFax},
  {IDD_NETWORK_DATA_FAX, IDC_MENU_NETWORK_DATA_FAX, IDL_NETWORK_DATA_FAX_MODEM_FOR_NEXT_CALL, MENU_SETTING,
   AEECM_ANSWER_VOICE_AS_MODEM_ONCE, NetGetNetworkDataFax, NetSetNetworkDataFax},
  {IDD_NETWORK_DATA_FAX, IDC_MENU_NETWORK_DATA_FAX, IDL_NETWORK_DATA_FAX_MODEM_UNTIL_POWERED_OFF, MENU_SETTING,
   AEECM_ANSWER_VOICE_AS_MODEM_ALWAYS, NetGetNetworkDataFax, NetSetNetworkDataFax},
};

typedef struct {
   uint16 wDialogID;
   uint16 wDialogTitle;
   uint16 wCtrlID;
   uint16 wSoftkeyID;
} StaticDlgDefinitionType;

static const StaticDlgDefinitionType NetStaticDlgs[] = {
  { IDD_NETWORK_AVOID_SERVING_SYS, IDS_AVOID_CURR_SYSTEM,IDC_STATIC_AVOID_SYS, IDC_SK_AVOID_SYS},
  { IDD_NETWORK_WAKEUP_1X, IDS_WAKEUP_1X,IDC_STATIC_WAKEUP_1X, IDC_SK_WAKEUP_1X},
  { IDD_NETWORK_WAKEUP_HDR, IDS_WAKEUP_HYBRID_HDR,IDC_STATIC_WAKEUP_HDR, IDC_SK_WAKEUP_HDR},

   // Not static Dialog
  { IDD_NETWORK_BAND_PREF,  IDS_BANDPREF, IDC_BAND_PREF, IDC_SK_BAND_PREF  },
};

/****************************************************************************

                          Static functions

****************************************************************************/
static uint16 NetGetNextSelectedItemDlgID(CNetApp *pMe, uint16 wItemID);
static uint16 NetGetDlgMenuCtlID(CNetApp *pMe, IDialog* pIActiveDialog);
static boolean NetCreateMenuSelectedItemDlg(CNetApp *pMe);
boolean NetUpdateCheckBoxSetting(CNetApp *pMe, IMenuCtl *pICurrentMenu);

/*===========================================================================
FUNCTION NetLookupSystemPrefMsgString 

DESCRIPTION
  This routine looks up the appropriate error code for a failed
  system preference change.

DEPENDENCIES
  None

SIDE EFFECTS
  None
===========================================================================*/
uint16 NetLookupSystemPrefMsgString (uint16 wDlgID)
{
  switch (wDlgID)
  {
  case IDD_NETWORK_FORCE_MODE:
    return IDS_MODE_PREF_ERROR;

  case IDD_NETWORK_BAND_PREF:
    return IDS_BAND_PREF_ERROR;

  case IDD_NETWORK_HYBR_PREF:
    /* FEATURE_HDR - Invalid command or hybrid preference selection */
    return IDS_HYBRID_PREF_ERROR;

  case IDD_NETWORK_ROAM_PREF:
    /* Bad roam preference parameter */
    return IDS_ROAM_PREF_ERROR;

  case IDD_NETWORK_DOMAIN_PREF:
    /* Invaild service domain parameter. GSM/WCDMA Only */
    return IDS_DOMAIN_PREF_ERROR;

  case IDD_NETWORK_ACQ_PREF:
    /* Invalid acquisition order preference parameter. GSM/WCDMA Only */
    return IDS_ACQ_ORDER_PREF_ERROR;

  case IDD_NETWORK_SELECT:
    /* Invalid network selection mode parameter. GSM/WCDMA Only */
    return IDS_NETWORK_SEL_ERROR;

  case IDD_NETWORK_AUTO_NAM:
  case IDD_NETWORK_SET_NAM:
    /* Current NAM is not RUIM NAM. CDMA Only */
    return IDS_NAM_ERROR;

  case IDD_NETWORK_DATA_FAX:
    return IDS_ANSWER_VOICE_ERROR;
    
  default:
    return IDS_CHANGE_FAILED;
  }
}

/*===========================================================================
FUNCTION NetReadSetting

DESCRIPTION
  Read setting from GetMethod and return the item ID of corresponding list entry

DEPENDENCIES
  None

SIDE EFFECTS
  None
===========================================================================*/
static uint16 NetReadSetting(CNetApp * pMe, uint16 wDlgID)
{
  int i, j;
  uint64 val=0;

/*lint -save -e737 -e574*/
  for (i=0; i < ARR_SIZE(NetMenuActionTbl); i++) {
    if (NetMenuActionTbl[i].wDialogID == wDlgID) {
      /* got a match */
      if (NetMenuActionTbl[i].wActionType != MENU_SETTING) {
        /* Not a menu setting - here in error */
        NET_ERR ("Invalid action for dialog %d", wDlgID, 0, 0);
        return 0;
      }

      if (NetMenuActionTbl[i].getfnPtr == NULL) {
        /* Null Get fn pointer */
        NET_ERR ("Invalid function pointer %d", wDlgID, 0, 0);
        return 0;
      }

      val = (*NetMenuActionTbl[i].getfnPtr)(pMe);

      for (j=i; (j < ARR_SIZE(NetMenuActionTbl)) && (NetMenuActionTbl[j].wDialogID == wDlgID); j++) {
        if (NetMenuActionTbl[j].nActionData == val) {
          return NetMenuActionTbl[j].wItemID;
        }
      }
      /* No match found */
      NET_ERR ("No match for  dialog %d", wDlgID, 0, 0);
      return 0;
    }
  }
/*lint -restore*/
  /* No match found */
  NET_ERR ("No match for  dialog %d", wDlgID, 0, 0);
  return 0;

} /* End NetReadSetting */

/*===========================================================================
FUNCTION NetReadAndSetCheckBoxItem

DESCRIPTION
  Read setting from GetMethod and return the item ID of corresponding list entry

DEPENDENCIES
  None

SIDE EFFECTS
  None
===========================================================================*/
static void NetReadAndSetCheckBoxItem(CNetApp * pMe, uint16 wDlgID)
{
  int i, j;
  uint64 val=0;

/*lint -save -e737 -e574*/
  for (i=0; i < ARR_SIZE(NetMenuActionTbl); i++) {
    if (NetMenuActionTbl[i].wDialogID == wDlgID) {
      /* got a match */
      if (NetMenuActionTbl[i].wActionType != MENU_SETTING) {
        /* Not a menu setting - here in error */
        NET_ERR ("Invalid action for dialog %d", wDlgID, 0, 0);
        return ;
      }

      if (NetMenuActionTbl[i].getfnPtr == NULL) {
        /* Null Get fn pointer */
        NET_ERR ("Invalid function pointer %d", wDlgID, 0, 0);
        return ;
      }

      val = (*NetMenuActionTbl[i].getfnPtr)(pMe);

      for (j=i; (j < ARR_SIZE(NetMenuActionTbl)) && (NetMenuActionTbl[j].wDialogID == wDlgID); j++) {
        if ((NetMenuActionTbl[j].nActionData & val) == NetMenuActionTbl[j].nActionData) {
            IDialog *pIDialog = ISHELL_GetActiveDialog(pMe->a.m_pIShell);

            IMenuCtl* pICurrentMenu =
                (IMenuCtl*) IDIALOG_GetControl(pIDialog, (int16) NetMenuActionTbl[j].wControlID);

            SetCheckBoxItem(pICurrentMenu,NetMenuActionTbl[j].wItemID,TRUE);
        }
      }
      return ;
    }
  }
/*lint -restore */
  
  /* No match found */
  NET_ERR ("No match for  dialog %d", wDlgID, 0, 0);
  return ;

} /* End NetReadAndSetCheckBoxItem */

/*===========================================================================
FUNCTION NetGetDlgSkMenuCtlID

DESCRIPTION
  The function returns the softkey menu control for a given dialog ID

DEPENDENCIES
  None

SIDE EFFECTS
  None
===========================================================================*/
static uint16 NetGetDlgSkMenuCtlID(uint16 wDlgID)
{
  int i;

  if ( wDlgID == 0 ) {
    return 0;
  }

/*lint -save -e737 -e574*/
  for (i = 0; i < ARR_SIZE(NetStaticDlgs); i++) {
    if (NetStaticDlgs[i].wDialogID == wDlgID) {
      return NetStaticDlgs[i].wSoftkeyID;
    }
  }
/*lint -restore*/
  NET_ERR( "NetGetDlgSkMenuCtlID(): No matching soft key ctl", 0, 0, 0);
  return 0; /* error */

} /* End NetGetDlgSkMenuCtlID */

/*===========================================================================
FUNCTION NetStripFeatureMenuItem

DESCRIPTION
  update the menu to reflect the correspondings under the FEATURE flag

DEPENDENCIES
  None

SIDE EFFECTS
  None
===========================================================================*/
static void NetStripFeatureMenuItem(CNetApp * pMe, IMenuCtl* pICurrentMenu, uint16 wCtrlID)
{
  boolean bReturnStatus;
  uint16 i;
#if defined (FEATURE_WCDMA) || defined (FEATURE_GSM)
  boolean bShowManPLMNSel;
#endif

  switch (wCtrlID)
  {
#if defined (FEATURE_WCDMA) || defined (FEATURE_GSM)
  case IDC_MENU_NETWORK_SELECT:
    /* Check the config item for whether or not Manual PLMN selection
     * should be available to the user.
     */
    bShowManPLMNSel = TRUE;
    if (pMe->m_pIConfig != NULL)
    {
      (void)ICONFIG_GetItem(pMe->m_pIConfig, CFGI_MANUAL_PLMN_SEL_ALLOWED,
                            (void *)&bShowManPLMNSel, sizeof(bShowManPLMNSel));
    }

    if (bShowManPLMNSel == FALSE)
    {
      bReturnStatus = IMENUCTL_DeleteItem(pICurrentMenu, IDL_NETSELECT_MANUAL);
      if (bReturnStatus == FALSE)
      {
        NET_ERR ("Failed to delete menu item", 0,0,0);
      }
      bReturnStatus = IMENUCTL_DeleteItem(pICurrentMenu, IDL_NETSELECT_MANUAL_WRAT);
      if (bReturnStatus == FALSE)
      {
        NET_ERR ("Failed to delete menu item", 0,0,0);
      }
    }

#if defined FEATURE_UIONE_HDK 
    //The reason that manual PLMN is being defined out is because HDK already has
    //a main menu option for this and the netsettings one does not work.
    //The reason maual w/ RAT is kept because HDK does not have such an option.
    else
    {
      bReturnStatus = IMENUCTL_DeleteItem(pICurrentMenu, IDL_NETSELECT_MANUAL);
      if (bReturnStatus == FALSE)
      {
        NET_ERR ("Failed to delete menu item", 0,0,0);
      }
    }
#endif /* FEATURE_UIONE_HDK */

    break;
#endif /* FEATURE_WCDMA || FEATURE_GSM */

  case IDC_MENU_NETWORK_SET_NAM:
    /* 3G UI has only menu items for NAM 1 - 4.
       If NV_MAX_NAMS > 4 then gotta update the resource file and
       menu / dialog code that handles NAMs */

    if(NV_MAX_NAMS > 4)
    {
      NET_ERR("Too many NAMS.", 0, 0, 0);
      return;
    }

    /* Remove all menu items and then selectively add what's needed,
       based on featurization / other factors */
    bReturnStatus = IMENUCTL_DeleteAll(pICurrentMenu);
    if (bReturnStatus == FALSE) {
      NET_ERR ("Failed to delete menu item", 0,0,0);
    }


    /* Add NAMs 1 through 4.
       !!! Make sure resource ids are consecutive !!! */
    for(i = 0; i < NV_MAX_NAMS; i++ ) {

      /* Add menu item for NAM #i */
      bReturnStatus = IMENUCTL_AddItem(pICurrentMenu,
                                       gpNetResourceFile,
                                       (uint16)(IDS_NAM1 + i),
                                       (uint16)(IDL_NETWORK_SET_NAM1 + i),
                                       0,
                                       0);
      if (bReturnStatus == FALSE) {
        NET_ERR ("Failed to delete menu item", 0,0,0);
      }

    }
    break;  // IDC_MENU_NETWORK_SET_NAM

  case IDC_MENU_NETWORK:
#if ((defined (FEATURE_CDMA1X)) && (NV_MAX_NAMS <= 1 )) ||\
    (!defined (FEATURE_CDMA1X))
    /* Delete NAM menus for UMTS targets and for single NAM builds */
    bReturnStatus = IMENUCTL_DeleteItem(pICurrentMenu, IDL_SET_NAM);
    if (bReturnStatus == FALSE) {
      NET_ERR ("Failed to delete menu item", 0,0,0);
    }
    bReturnStatus = IMENUCTL_DeleteItem(pICurrentMenu, IDL_AUTO_NAM);
    if (bReturnStatus == FALSE) {
      NET_ERR ("Failed to delete menu item", 0,0,0);
    }
#endif
#if (!defined (FEATURE_CDMA1X))
    bReturnStatus = IMENUCTL_DeleteItem(pICurrentMenu, IDL_AVOID_CURR_SYS);
    if (bReturnStatus == FALSE) {
      NET_ERR ("Failed to delete menu item", 0,0,0);
    }
    bReturnStatus = IMENUCTL_DeleteItem(pICurrentMenu, IDL_WAKEUP_1X);
    if (bReturnStatus == FALSE) {
      NET_ERR ("Failed to delete menu item", 0,0,0);
    }
#endif
#ifndef FEATURE_HDR
    bReturnStatus = IMENUCTL_DeleteItem(pICurrentMenu, IDL_WAKEUP_HDR);
    if (bReturnStatus == FALSE) {
      NET_ERR ("Failed to delete menu item", 0,0,0);
    }
#endif /* FEATURE_HDR */

#ifndef FEATURE_JCDMA_1X
    bReturnStatus = IMENUCTL_DeleteItem(pICurrentMenu, IDL_M51);
    if (bReturnStatus == FALSE) {
      NET_ERR ("Failed to delete menu item", 0,0,0);
    }
#endif /* FEATURE_JCDMA_1X */

#ifndef FEATURE_HDR
    bReturnStatus = IMENUCTL_DeleteItem(pICurrentMenu, IDL_HYBRID_PREF);
    if (bReturnStatus == FALSE) {
      NET_ERR ("Failed to delete menu item", 0,0,0);
    }
#endif /* FEATURE_HDR */

#if !defined(FEATURE_WCDMA) || !defined(FEATURE_GSM)
    bReturnStatus = IMENUCTL_DeleteItem(pICurrentMenu, IDL_ACQORDER_PREF);
    if (bReturnStatus == FALSE) {
      NET_ERR ("Failed to delete menu item", 0,0,0);
    }
#endif /* FEATURE_WCDMA */

#if !defined (FEATURE_WCDMA) && !defined (FEATURE_GSM)
    bReturnStatus = IMENUCTL_DeleteItem(pICurrentMenu, IDL_SERVDOMAIN_PREF);
    if (bReturnStatus == FALSE) {
      NET_ERR ("Failed to delete menu item", 0,0,0);
    }
#endif

#if defined(FEATURE_UIONE_HDK) || (!defined (FEATURE_WCDMA) && !defined (FEATURE_GSM))
    bReturnStatus = IMENUCTL_DeleteItem(pICurrentMenu, IDL_NETSELECT_PREF);
    if (bReturnStatus == FALSE) {
      NET_ERR ("Failed to delete menu item", 0,0,0);
    }
#endif
    break;  /* IDC_MENU_NETWORK */

  case IDC_BAND_PREF:
#if !defined(FEATURE_CDMA1X)
    bReturnStatus = IMENUCTL_DeleteItem(pICurrentMenu, IDL_CELL_A);
    if (bReturnStatus == FALSE) {
      NET_ERR ("Failed to delete menu item", 0,0,0);
    }
    bReturnStatus = IMENUCTL_DeleteItem(pICurrentMenu, IDL_CELL_B);
    if (bReturnStatus == FALSE) {
      NET_ERR ("Failed to delete menu item", 0,0,0);
    }
    bReturnStatus = IMENUCTL_DeleteItem(pICurrentMenu, IDL_CELL_ONLY);
    if (bReturnStatus == FALSE) {
      NET_ERR ("Failed to delete menu item", 0,0,0);
    }
    bReturnStatus = IMENUCTL_DeleteItem(pICurrentMenu, IDL_PCS_ONLY);
    if (bReturnStatus == FALSE) {
      NET_ERR ("Failed to delete menu item", 0,0,0);
    }
    bReturnStatus = IMENUCTL_DeleteItem(pICurrentMenu, IDL_BC3);
    if (bReturnStatus == FALSE) {
      NET_ERR ("Failed to delete menu item", 0,0,0);
    }
    bReturnStatus = IMENUCTL_DeleteItem(pICurrentMenu, IDL_BC4);
    if (bReturnStatus == FALSE) {
      NET_ERR ("Failed to delete menu item", 0,0,0);
    }
    bReturnStatus = IMENUCTL_DeleteItem(pICurrentMenu, IDL_BC5);
    if (bReturnStatus == FALSE) {
      NET_ERR ("Failed to delete menu item", 0,0,0);
    }
    bReturnStatus = IMENUCTL_DeleteItem(pICurrentMenu, IDL_BC6);
    if (bReturnStatus == FALSE) {
      NET_ERR ("Failed to delete menu item", 0,0,0);
    }
    bReturnStatus = IMENUCTL_DeleteItem(pICurrentMenu, IDL_BC7);
    if (bReturnStatus == FALSE) {
      NET_ERR ("Failed to delete menu item", 0,0,0);
    }
    bReturnStatus = IMENUCTL_DeleteItem(pICurrentMenu, IDL_BC8);
    if (bReturnStatus == FALSE) {
      NET_ERR ("Failed to delete menu item", 0,0,0);
    }
    bReturnStatus = IMENUCTL_DeleteItem(pICurrentMenu, IDL_BC9);
    if (bReturnStatus == FALSE) {
      NET_ERR ("Failed to delete menu item", 0,0,0);
    }
    bReturnStatus = IMENUCTL_DeleteItem(pICurrentMenu, IDL_BC10);
    if (bReturnStatus == FALSE) {
      NET_ERR ("Failed to delete menu item", 0,0,0);
    }
    bReturnStatus = IMENUCTL_DeleteItem(pICurrentMenu, IDL_BC11);
    if (bReturnStatus == FALSE) {
      NET_ERR ("Failed to delete menu item", 0,0,0);
    }
    bReturnStatus = IMENUCTL_DeleteItem(pICurrentMenu, IDL_BC15);
    if (bReturnStatus == FALSE) {
      NET_ERR ("Failed to delete menu item", 0,0,0);
    }
    bReturnStatus = IMENUCTL_DeleteItem(pICurrentMenu, IDL_BC16);
    if (bReturnStatus == FALSE) {
      NET_ERR ("Failed to delete menu item", 0,0,0);
    }
#endif

#ifndef FEATURE_GSM
    bReturnStatus = IMENUCTL_DeleteItem(pICurrentMenu, IDL_GSM_PCS_1900);
    if (bReturnStatus == FALSE) {
      NET_ERR ("Failed to delete menu item", 0,0,0);
    }

    bReturnStatus = IMENUCTL_DeleteItem(pICurrentMenu, IDL_GSM_450);
    if (bReturnStatus == FALSE) {
      NET_ERR ("Failed to delete menu item", 0,0,0);
    }

    bReturnStatus = IMENUCTL_DeleteItem(pICurrentMenu, IDL_GSM_480);
    if (bReturnStatus == FALSE) {
      NET_ERR ("Failed to delete menu item", 0,0,0);
    }

    bReturnStatus = IMENUCTL_DeleteItem(pICurrentMenu, IDL_GSM_750);
    if (bReturnStatus == FALSE) {
      NET_ERR ("Failed to delete menu item", 0,0,0);
    }

    bReturnStatus = IMENUCTL_DeleteItem(pICurrentMenu, IDL_GSM_850);
    if (bReturnStatus == FALSE) {
      NET_ERR ("Failed to delete menu item", 0,0,0);
    }

    bReturnStatus = IMENUCTL_DeleteItem(pICurrentMenu, IDL_GSM_DCS_1800);
    if (bReturnStatus == FALSE) {
      NET_ERR ("Failed to delete menu item", 0,0,0);
    }

    bReturnStatus = IMENUCTL_DeleteItem(pICurrentMenu, IDL_GSM_EGSM_900);
    if (bReturnStatus == FALSE) {
      NET_ERR ("Failed to delete menu item", 0,0,0);
    }

    bReturnStatus = IMENUCTL_DeleteItem(pICurrentMenu, IDL_GSM_PGSM_900);
    if (bReturnStatus == FALSE) {
      NET_ERR ("Failed to delete menu item", 0,0,0);
    }

    bReturnStatus = IMENUCTL_DeleteItem(pICurrentMenu, IDL_GSM_RGSM_900);
    if (bReturnStatus == FALSE) {
      NET_ERR ("Failed to delete menu item", 0,0,0);
    }

#endif

#ifndef FEATURE_WCDMA
    bReturnStatus = IMENUCTL_DeleteItem(pICurrentMenu, IDL_WCDMA_PCS_1900);
    if (bReturnStatus == FALSE) {
      NET_ERR ("Failed to delete menu item", 0,0,0);
    }
    bReturnStatus = IMENUCTL_DeleteItem(pICurrentMenu, IDL_WCDMA_IMT_2000);
    if (bReturnStatus == FALSE) {
      NET_ERR ("Failed to delete menu item", 0,0,0);
    }
    bReturnStatus = IMENUCTL_DeleteItem(pICurrentMenu, IDL_WCDMA_III_1700);
    if (bReturnStatus == FALSE) {
      NET_ERR ("Failed to delete menu item", 0,0,0);
    }
    bReturnStatus = IMENUCTL_DeleteItem(pICurrentMenu, IDL_WCDMA_IV_1700);
    if (bReturnStatus == FALSE) {
      NET_ERR ("Failed to delete menu item", 0,0,0);
    }
    bReturnStatus = IMENUCTL_DeleteItem(pICurrentMenu, IDL_WCDMA_850);
    if (bReturnStatus == FALSE) {
      NET_ERR ("Failed to delete menu item", 0,0,0);
    }
    bReturnStatus = IMENUCTL_DeleteItem(pICurrentMenu, IDL_WCDMA_800);
    if (bReturnStatus == FALSE) {
      NET_ERR ("Failed to delete menu item", 0,0,0);
    }
    bReturnStatus = IMENUCTL_DeleteItem(pICurrentMenu, IDL_WCDMA_VII_2600);
    if (bReturnStatus == FALSE) {
      NET_ERR ("Failed to delete menu item", 0,0,0);
    }
    bReturnStatus = IMENUCTL_DeleteItem(pICurrentMenu, IDL_WCDMA_VIII_900);
    if (bReturnStatus == FALSE) {
      NET_ERR ("Failed to delete menu item", 0,0,0);
    }
    bReturnStatus = IMENUCTL_DeleteItem(pICurrentMenu, IDL_WCDMA_IX_1700);
    if (bReturnStatus == FALSE) {
      NET_ERR ("Failed to delete menu item", 0,0,0);
    }
#endif

#ifndef FEATURE_WLAN
    bReturnStatus = IMENUCTL_DeleteItem(pICurrentMenu, IDL_WLAN_2400_US);
    if (bReturnStatus == FALSE) {
      NET_ERR ("Failed to delete menu item", 0,0,0);
    }
    bReturnStatus = IMENUCTL_DeleteItem(pICurrentMenu, IDL_WLAN_2400_JAPAN);
    if (bReturnStatus == FALSE) {
      NET_ERR ("Failed to delete menu item", 0,0,0);
    }
    bReturnStatus = IMENUCTL_DeleteItem(pICurrentMenu, IDL_WLAN_2400_EUROPE);
    if (bReturnStatus == FALSE) {
      NET_ERR ("Failed to delete menu item", 0,0,0);
    }
    bReturnStatus = IMENUCTL_DeleteItem(pICurrentMenu, IDL_WLAN_2400_FRANCE);
    if (bReturnStatus == FALSE) {
      NET_ERR ("Failed to delete menu item", 0,0,0);
    }
    bReturnStatus = IMENUCTL_DeleteItem(pICurrentMenu, IDL_WLAN_2400_SPAIN);
    if (bReturnStatus == FALSE) {
      NET_ERR ("Failed to delete menu item", 0,0,0);
    }
    bReturnStatus = IMENUCTL_DeleteItem(pICurrentMenu, IDL_WLAN_5000_US);
    if (bReturnStatus == FALSE) {
      NET_ERR ("Failed to delete menu item", 0,0,0);
    }
    bReturnStatus = IMENUCTL_DeleteItem(pICurrentMenu, IDL_WLAN_5000_EUROPE);
    if (bReturnStatus == FALSE) {
      NET_ERR ("Failed to delete menu item", 0,0,0);
    }
    bReturnStatus = IMENUCTL_DeleteItem(pICurrentMenu, IDL_WLAN_5000_JAPAN);
    if (bReturnStatus == FALSE) {
      NET_ERR ("Failed to delete menu item", 0,0,0);
    }
    bReturnStatus = IMENUCTL_DeleteItem(pICurrentMenu, IDL_WLAN_5000_FRANCE);
    if (bReturnStatus == FALSE) {
      NET_ERR ("Failed to delete menu item", 0,0,0);
    }
    bReturnStatus = IMENUCTL_DeleteItem(pICurrentMenu, IDL_WLAN_5000_SPAIN);
    if (bReturnStatus == FALSE) {
      NET_ERR ("Failed to delete menu item", 0,0,0);
    }
    bReturnStatus = IMENUCTL_DeleteItem(pICurrentMenu, IDL_WLAN_ANY);
    if (bReturnStatus == FALSE) {
      NET_ERR ("Failed to delete menu item", 0,0,0);
    }
#endif // ! FEATURE_WLAN
    break;

  case IDC_MENU_NETWORK_FORCE_MODE:
#ifndef FEATURE_AMPS
    bReturnStatus = IMENUCTL_DeleteItem(pICurrentMenu, IDL_MODE_AMPS_ONLY);
    if (bReturnStatus == FALSE) {
      NET_ERR ("Failed to delete menu item", 0,0,0);
    }
#endif

#ifndef FEATURE_HDR
    bReturnStatus = IMENUCTL_DeleteItem(pICurrentMenu, IDL_MODE_HDR_ONLY);
    if (bReturnStatus == FALSE) {
      NET_ERR ("Failed to delete menu item", 0,0,0);
    }
#endif

#ifndef FEATURE_HDR
    bReturnStatus = IMENUCTL_DeleteItem(pICurrentMenu, IDL_MODE_CDMA_HDR_ONLY);
    if (bReturnStatus == FALSE) {
      NET_ERR ("Failed to delete menu item", 0,0,0);
    }
#endif

#if !((defined(FEATURE_CDMA1X)) && defined (FEATURE_AMPS))
    bReturnStatus = IMENUCTL_DeleteItem(pICurrentMenu, IDL_MODE_CDMA_AMPS);
    if (bReturnStatus == FALSE) {
      NET_ERR ("Failed to delete menu item", 0,0,0);
    }
#endif
#if !defined(FEATURE_CDMA1X)
    bReturnStatus = IMENUCTL_DeleteItem(pICurrentMenu, IDL_MODE_CDMA_ONLY);
    if (bReturnStatus == FALSE) {
      NET_ERR ("Failed to delete menu item", 0,0,0);
    }
#endif
#ifndef FEATURE_GSM
    bReturnStatus = IMENUCTL_DeleteItem(pICurrentMenu, IDL_MODE_GSM_ONLY);
    if (bReturnStatus == FALSE) {
      NET_ERR ("Failed to delete menu item", 0,0,0);
    }
#endif
#ifndef FEATURE_WCDMA
    bReturnStatus = IMENUCTL_DeleteItem(pICurrentMenu, IDL_MODE_WCDMA_ONLY);
    if (bReturnStatus == FALSE) {
      NET_ERR ("Failed to delete menu item", 0,0,0);
    }
#endif
#ifndef FEATURE_WLAN
    bReturnStatus = IMENUCTL_DeleteItem(pICurrentMenu, IDL_MODE_WLAN_ONLY);
    if (bReturnStatus == FALSE) {
      NET_ERR ("Failed to delete menu item", 0,0,0);
    }
    bReturnStatus = IMENUCTL_DeleteItem(pICurrentMenu, IDL_MODE_CDMA_WLAN);
    if (bReturnStatus == FALSE) {
      NET_ERR ("Failed to delete menu item", 0,0,0);
    }
    bReturnStatus = IMENUCTL_DeleteItem(pICurrentMenu, IDL_MODE_CDMA_HDR_WLAN);
    if (bReturnStatus == FALSE) {
      NET_ERR ("Failed to delete menu item", 0,0,0);
    }
    bReturnStatus = IMENUCTL_DeleteItem(pICurrentMenu, IDL_MODE_HDR_WLAN);
    if (bReturnStatus == FALSE) {
      NET_ERR ("Failed to delete menu item", 0,0,0);
    }
    bReturnStatus = IMENUCTL_DeleteItem(pICurrentMenu, IDL_MODE_GSM_WLAN);
    if (bReturnStatus == FALSE) {
      NET_ERR ("Failed to delete menu item", 0,0,0);
    }
    bReturnStatus = IMENUCTL_DeleteItem(pICurrentMenu, IDL_MODE_WCDMA_WLAN);
    if (bReturnStatus == FALSE) {
      NET_ERR ("Failed to delete menu item", 0,0,0);
    }
    bReturnStatus = IMENUCTL_DeleteItem(pICurrentMenu, IDL_MODE_GSM_WCDMA_WLAN);
    if (bReturnStatus == FALSE) {
      NET_ERR ("Failed to delete menu item", 0,0,0);
    }
#else
#ifndef FEATURE_CDMA1X
    bReturnStatus = IMENUCTL_DeleteItem(pICurrentMenu, IDL_MODE_CDMA_WLAN);
    if (bReturnStatus == FALSE) {
      NET_ERR ("Failed to delete menu item", 0,0,0);
    }
    bReturnStatus = IMENUCTL_DeleteItem(pICurrentMenu, IDL_MODE_CDMA_HDR_WLAN);
    if (bReturnStatus == FALSE) {
      NET_ERR ("Failed to delete menu item", 0,0,0);
    }
#endif  // FEATURE_CDMA1X
#ifndef FEATURE_HDR
    bReturnStatus = IMENUCTL_DeleteItem(pICurrentMenu, IDL_MODE_HDR_WLAN);
    if (bReturnStatus == FALSE) {
      NET_ERR ("Failed to delete menu item", 0,0,0);
    }
    bReturnStatus = IMENUCTL_DeleteItem(pICurrentMenu, IDL_MODE_CDMA_HDR_WLAN);
    if (bReturnStatus == FALSE) {
      // may have been removed if also !CDMA1X - so message rather than core error 
      NET_MSG ("Failed to delete menu item CDMA_HDR", 0,0,0);
    }
#endif // FEATURE_HDR
#ifndef FEATURE_WCDMA
    bReturnStatus = IMENUCTL_DeleteItem(pICurrentMenu, IDL_MODE_WCDMA_WLAN);
    if (bReturnStatus == FALSE) {
      NET_ERR ("Failed to delete menu item", 0,0,0);
    }
    bReturnStatus = IMENUCTL_DeleteItem(pICurrentMenu, IDL_MODE_GSM_WCDMA_WLAN);
    if (bReturnStatus == FALSE) {
      NET_ERR ("Failed to delete menu item", 0,0,0);
    }
    bReturnStatus = IMENUCTL_DeleteItem(pICurrentMenu, IDL_MODE_GSM_WCDMA_ONLY);
    if (bReturnStatus == FALSE) {
      NET_ERR ("Failed to delete menu item", 0,0,0);
    }
#endif // FEATURE_WCDMA
#ifndef FEATURE_GSM
    bReturnStatus = IMENUCTL_DeleteItem(pICurrentMenu, IDL_MODE_GSM_WLAN);
    if (bReturnStatus == FALSE) {
      NET_ERR ("Failed to delete menu item", 0,0,0);
    }
    bReturnStatus = IMENUCTL_DeleteItem(pICurrentMenu, IDL_MODE_GSM_WCDMA_WLAN);
    if (bReturnStatus == FALSE) {
      // may have been removed if also !WCDMA - so message rather than core error 
      NET_MSG ("Failed to delete menu item GSM_WLAN", 0,0,0);
    }
    bReturnStatus = IMENUCTL_DeleteItem(pICurrentMenu, IDL_MODE_GSM_WCDMA_ONLY);
    if (bReturnStatus == FALSE) {
      NET_ERR ("Failed to delete menu item", 0,0,0);
    }
#endif // FEATURE_GSM
#endif  // ! FEATURE_WLAN
    break;

  default:
    break;
  }
} /* End NetStripFeatureMenuItem */


/*===========================================================================
FUNCTION NetInitRadioButtonDlg

DESCRIPTION
  This function update the radio button menu item list based on featurization,
  update the radio button for the selected item based on NV value

DEPENDENCIES
  NetGetDlgMenuCtlID, NetStripFeatureMenuItem,
  SetMenuIcon, SetDefaultMenuLook

SIDE EFFECTS
  None
===========================================================================*/
static boolean NetInitRadioButtonDlg(CNetApp *pMe, uint16 wDlgID, IDialog* pIActiveDialog)
{
  IMenuCtl* pICurrentMenu;
  uint16 wCtrlID, wItemID;

  if ( pIActiveDialog == NULL ) {
    NET_ERR ("Null pointer", 0,0,0);
    return FALSE;
  }

  /* Get the control ID */
  wCtrlID = NetGetDlgMenuCtlID(pMe, pIActiveDialog);
  if(wCtrlID == 0) { /* error */
    NET_ERR ("Null pointer", 0,0,0);
    return FALSE;
  }

  pICurrentMenu = (IMenuCtl*) IDIALOG_GetControl(pIActiveDialog, (int16) wCtrlID);
  if (pICurrentMenu == NULL) {
    NET_ERR ("Null pointer", 0,0,0);
    return FALSE;
  }

  /* Remove or update items from the menu based on featurization requirement */
  NetStripFeatureMenuItem(pMe, pICurrentMenu, wCtrlID);

  /* Get the current setting from static variable or NV */
  wItemID = NetReadSetting(pMe, wDlgID);

  if(wItemID == 0) { /* error */
    NET_ERR ("Null pointer", 0,0,0);
    NetDisplayMessageDlg(pMe, IDS_READ_FAILED, 3);
    return FALSE;
  }

  /* Initialize radio button images with IDB_RADIO_UNFILLED */
  InitMenuIcons(pICurrentMenu);

  /* Update the radio button for the currently selected option */
  SetMenuIcon(pICurrentMenu, wItemID, TRUE);

  /* Set the menu style */
  SetDefaultMenuLook(pMe->a.m_pIShell, pICurrentMenu);
  return TRUE;

} /* End NetInitRadioButtonDlg */


/*===========================================================================
FUNCTION NetInitCheckBoxDlg

DESCRIPTION
  This function update the check box menu items list based on featurization,
  update the Check box item for the selected item based on NV value

DEPENDENCIES
  NetGetDlgMenuCtlID, NetStripFeatureMenuItem,
  SetMenuIcon, SetDefaultMenuLook

SIDE EFFECTS
  None
===========================================================================*/
static boolean NetInitCheckBoxDlg(CNetApp *pMe, uint16 wDlgID, IDialog* pIActiveDialog)
{
  IMenuCtl* pICurrentMenu;
  IMenuCtl* pISkMenu= NULL;
  uint16 wCtrlID, wSkCtrlID;

  if ( pIActiveDialog == NULL ) {
    NET_ERR ("Null pointer", 0,0,0);
    return FALSE;
  }

  /* Get the control ID */
  wCtrlID = NetGetDlgMenuCtlID(pMe, pIActiveDialog);
  if(wCtrlID == 0) { /* error */
    NET_ERR ("Null pointer", 0,0,0);
    return FALSE;
  }

  pICurrentMenu = (IMenuCtl*) IDIALOG_GetControl(pIActiveDialog, (int16) wCtrlID);
  if (pICurrentMenu == NULL) {
    NET_ERR ("Null pointer", 0,0,0);
    return FALSE;
  }

  wSkCtrlID = NetGetDlgSkMenuCtlID(wDlgID);
  if(wSkCtrlID != 0)
  {
    pISkMenu = (IMenuCtl*)IDIALOG_GetControl(pIActiveDialog, (int16) wSkCtrlID);
    if(pISkMenu != NULL)
    {
      SetDefaultSoftkeyLook(pMe->a.m_pIShell, pISkMenu);
    }
  }


  /* Remove or update items from the menu based on featurization requirement */
  NetStripFeatureMenuItem(pMe, pICurrentMenu, wCtrlID);
  
  /* Initialize radio button images with IDB_CHECK_OFF */
  InitCheckBox(pICurrentMenu);

  /* Get the current setting from static variable or NV */
  NetReadAndSetCheckBoxItem(pMe, wDlgID);

  /* Set the menu style */
  SetDefaultMenuLook(pMe->a.m_pIShell, pICurrentMenu);
  return TRUE;

} /* End NetInitCheckBoxDlg */

/*===========================================================================
FUNCTION NetUpdateSetting

DESCRIPTION
  Update menu setting static variable and NV values if the selected Item is
  NV related.
  Based on whether the change of setting took place, update the display of
  radio buttons.

DEPENDENCIES
  NetReadSetting, InitMenuIcons, SetMenuIcon

SIDE EFFECTS
  None
===========================================================================*/
static boolean NetUpdateSetting(CNetApp *pMe,
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
    NET_ERR ("Null pointer", 0,0,0);
    return FALSE;
  }

  wDlgID = IDIALOG_GetID(pIActiveDialog);
  if (wDlgID == 0) { /*error checking */
    NET_ERR ("Null pointer", 0,0,0);
    return FALSE;
  }

  wCtrlID = NetGetDlgMenuCtlID(pMe, pIActiveDialog);
  if(wCtrlID == 0) {
    NET_ERR ("Null pointer", 0,0,0);
    return FALSE;
  }

  pICurrentMenu= (IMenuCtl*) IDIALOG_GetControl(pIActiveDialog, (int16) wCtrlID);
  if(pICurrentMenu == NULL) {
    NET_ERR ("Null pointer", 0,0,0);
    return FALSE;
  }

  if (WriteNetSetting(pMe, wDlgID, wSelectedItemID)) {

    /* Initialize radio button images with IDB_RADIO_UNFILLED */
    InitMenuIcons (pICurrentMenu);

    /* Update radio button image to reflect new setting */
    SetMenuIcon (pICurrentMenu, wSelectedItemID, TRUE);

    return TRUE;
  }
  return FALSE;
} /* End NetUpdateSetting */

/*===========================================================================
FUNCTION NetGetStaticDlgTitleResStringID

DESCRIPTION
  This function matches dialog id with a resource string ID used for static
  control title

DEPENDENCIES
  None

SIDE EFFECTS
  None
===========================================================================*/
static uint16 NetGetStaticDlgTitleResStringID(CNetApp *pMe, uint16 wDlgID)
{
  int i;

  if ( wDlgID == 0 ) {
    return 0;
  }

/*lint -save -e737 -e574*/
  for (i = 0; i< ARR_SIZE(NetStaticDlgs); i++) {
    if (NetStaticDlgs[i].wDialogID == wDlgID) {
      return NetStaticDlgs[i].wDialogTitle;
    }
  }
/*lint -restore*/
  NET_ERR( "NetGetStaticDlgTitleResStringID(): No matching dialog", 0, 0, 0);
  return 0; /* error */
} /* End NetGetStaticDlgTitleResStringID */

/*===========================================================================
FUNCTION NetSetStaticDlgTitle

DESCRIPTION
  Based on IDD, assign title for the static dialog

DEPENDENCIES
  Note: Still need to set the title even if it was created in the resource editor,
        because of the ISTATIC_SetText function call later on

SIDE EFFECTS
  None
===========================================================================*/
static void NetSetStaticDlgTitle(CNetApp *pMe, uint16 wDlgID, AECHAR *title, int nLen)
{

  uint16 wStrID = 0;
  int nResult = 0;

  wStrID = NetGetStaticDlgTitleResStringID(pMe,wDlgID);

  nResult = ISHELL_LoadResString(pMe->a.m_pIShell,
                          gpNetResourceFile,
                          wStrID,
                          title,
                          nLen*((int)sizeof(AECHAR)));
  if(nResult == 0) {
    NET_ERR("Failed to set title for static dlg %d",wDlgID,0,0);
  }

} /* End NetSetStaticDlgTitle */

/*===========================================================================
FUNCTION NetSetStaticDlgContent

DESCRIPTION
  Based on IDD, assign content for the static dialog

DEPENDENCIES
  GetNVBannerSetting

SIDE EFFECTS
  None
===========================================================================*/
static void NetSetStaticDlgContent(CNetApp *pMe, uint16 wDlgID, AECHAR* pszContent, int nLen)
{
  switch(wDlgID)
  {
    case IDD_NETWORK_AVOID_SERVING_SYS:
    case IDD_NETWORK_WAKEUP_1X:
    case IDD_NETWORK_WAKEUP_HDR:
      pszContent[0] = (AECHAR)'\0';
      break;

    default: /* error */
      pszContent[0] = (AECHAR)'\0';
      NET_ERR("SetStaticDlgContent(): no matching IDD", 0, 0, 0);
      break;
  }
} /* End NetSetStaticDlgContent */

/*===========================================================================
FUNCTION NetGetDlgStaticCtlID

DESCRIPTION
  return the control for the static dlg

DEPENDENCIES
  None

SIDE EFFECTS
  None
===========================================================================*/
static uint16 NetGetDlgStaticCtlID(uint16 wDlgID)
{

  int i;

  if ( wDlgID == 0 ) {
    return 0;
  }

/*lint -save -e737 -e574*/
  for (i = 0; i< ARR_SIZE(NetStaticDlgs); i++) {
    if (NetStaticDlgs[i].wDialogID == wDlgID) {
      return NetStaticDlgs[i].wCtrlID;
    }
  }
/*lint -restore*/

  NET_ERR( "NetGetDlgStaticCtlID(): No matching static ctl", 0, 0, 0);
  return 0; /* error */

} /* End NetGetDlgStaticCtlID */

/*===========================================================================
FUNCTION NetInitStaticDlg

DESCRIPTION
  Set the static dialog title, content and style

DEPENDENCIES
  SetStaticDlgTitle, SetStaticDlgContent, SetGeneralStaticTextStyle,
  SetStaticTextCtlSize

SIDE EFFECTS
  None
===========================================================================*/
static boolean NetInitStaticDlg(CNetApp *pMe, uint16 wDlgID, IDialog* pIDialog)
{
  IStatic* pIStaticText;
  IMenuCtl* pIMenu;
  AECHAR title[TITLE_BUFFER_SIZE];
  AECHAR psContent[TEXT_BUFFER_SIZE];
  uint16 wStaticCtrlID;
  uint16 wMenuCtrlID;

  wStaticCtrlID = NetGetDlgStaticCtlID(wDlgID);
  wMenuCtrlID = NetGetDlgSkMenuCtlID(wDlgID);

  if ((wStaticCtrlID == 0) || (wMenuCtrlID == 0)) { /* error */
    NET_ERR ("Null pointer", 0,0,0);
    return FALSE;
  }

  if(pIDialog == NULL) { /* error */
    NET_ERR ("Null pointer", 0,0,0);
    return FALSE;
  }

  pIStaticText = (IStatic*)IDIALOG_GetControl(pIDialog, (int16) wStaticCtrlID);
  pIMenu = (IMenuCtl*)IDIALOG_GetControl(pIDialog, (int16) wMenuCtrlID);

  if((pIStaticText == NULL) || (pIMenu == NULL)) { /* error */
    NET_ERR ("Null pointer", 0,0,0);
    return FALSE;
  }

  /* Set the title and content */
/*lint -save -e737 -e574*/
  NetSetStaticDlgTitle(pMe, wDlgID, title, ARR_SIZE(title));
  NetSetStaticDlgContent(pMe, wDlgID, psContent, ARR_SIZE(psContent));
/*lint -restore*/

  ISTATIC_SetText(pIStaticText, title, psContent, AEE_FONT_BOLD, AEE_FONT_NORMAL);

  /* Set the softkey menu style */
  SetDefaultMenuLook(pMe->a.m_pIShell, pIMenu);

  /* Get the newly defined softkey size and use that to set the static size */
  (void) NetSetStaticTextCtlSize(pIStaticText, (IControl*)pIMenu);
  (void) NetSetGeneralStaticTextStyle(pIStaticText);  /* set the static text style */

  /* Force the control to redraw */
  return ICONTROL_Redraw((IControl*)pIStaticText);

} /*End NetInitStaticDlg */


/****************************************************************************

                          Exported functions

****************************************************************************/

/*===========================================================================
FUNCTION WriteNetSetting

DESCRIPTION
  Save the setting of given item through Set methods.
  Return TRUE if the setting was changed and FALSE otherwise.

DEPENDENCIES
  Various SetXXXSetting functions

SIDE EFFECTS
  None
===========================================================================*/
boolean WriteNetSetting(CNetApp *pMe, uint16 wDlgID, uint16 wItemID)
{
  int i, j;

/*lint -save -e737 -e574*/
  for (i=0; i < ARR_SIZE(NetMenuActionTbl); i++) {
    if (NetMenuActionTbl[i].wDialogID == wDlgID) {
      /* got a match */
      if (NetMenuActionTbl[i].wActionType != MENU_SETTING) {
        /* Not a menu setting - here in error */
        NET_ERR ("Invalid action for dialog %d", wDlgID, 0, 0);
        return FALSE;
      }


      for (j=i; (j < ARR_SIZE(NetMenuActionTbl)) && (NetMenuActionTbl[j].wDialogID == wDlgID); j++) {
        if (NetMenuActionTbl[j].wItemID == wItemID) {
          if (NetMenuActionTbl[j].setfnPtr == NULL) {
            /* Null Get fn pointer */
            NET_ERR ("Invalid function pointer %d", wDlgID, 0, 0);
            return FALSE;
          }
          if ((*NetMenuActionTbl[j].setfnPtr)(pMe, NetMenuActionTbl[j].nActionData))
            return TRUE;
          return FALSE;
        }
      }
/*lint -restore */

      /* No match found */
      NET_ERR ("No match for  dialog %d", wDlgID, 0, 0);
      return FALSE;
    }
  }
  /* No match found */
  NET_ERR ("No match for  dialog %d", wDlgID, 0, 0);
  return FALSE;

} /* End WriteNetSetting */

/*===========================================================================
FUNCTION UpdateNetCheckBoxSetting

DESCRIPTION
  Save the setting of given item through Set methods.
  Return TRUE if the setting was changed and FALSE otherwise.

DEPENDENCIES
  Various SetXXXSetting functions

SIDE EFFECTS
  None
===========================================================================*/
boolean UpdateNetCheckBoxSetting(CNetApp *pMe, IMenuCtl *pICurrentMenu)
{
  int i, j;

  IDialog* pIActiveDialog = (IDialog*)ISHELL_GetActiveDialog(pMe->a.m_pIShell);

  uint16 wDlgID = IDIALOG_GetID(pIActiveDialog);
  uint64 nNewSetting =0;
  boolean setValue = FALSE;
  boolean matchFound = FALSE;
  SetFnPtrType   *setfnPtr = NULL;

/*lint -save -e737 -e574*/
  for (i=0; i < ARR_SIZE(NetMenuActionTbl); i++) {
    if (NetMenuActionTbl[i].wDialogID == wDlgID) {
      /* got a match */
      if (NetMenuActionTbl[i].wActionType != MENU_SETTING) {
        /* Not a menu setting - here in error */
        NET_ERR ("Invalid action for dialog %d", wDlgID, 0, 0);
        return FALSE;
      }
      for (j=i; (j < ARR_SIZE(NetMenuActionTbl)) && (NetMenuActionTbl[j].wDialogID == wDlgID); j++) {

        if (NetMenuActionTbl[j].setfnPtr == NULL) {
          /* Null Get fn pointer */
          NET_ERR ("Invalid function pointer %d", wDlgID, 0, 0);
          return FALSE;
        }
        /*If Check box is set, then unset it*/
        setValue = GetCheckBoxVal(pICurrentMenu, NetMenuActionTbl[j].wItemID);
        if(setValue)
        {
           nNewSetting |= NetMenuActionTbl[j].nActionData;
        }

        setfnPtr = NetMenuActionTbl[j].setfnPtr;
        matchFound = TRUE;
      }
      if(matchFound)
         break;
    }
  }
/*lint -restore*/

  if(matchFound)
  {
    return ((setfnPtr)(pMe, nNewSetting));
  }

  /* No match found */
  NET_ERR ("No match for  dialog %d", wDlgID, 0, 0);
  return FALSE;

}/* End UpdateNetCheckBoxSetting */

/*==========================================================================

                        EVENT HANDLERS

=========================================================================*/

/*===========================================================================
FUNCTION NetMenuDlgEventHandler

DESCRIPTION
  handle generic menu dlg events

DEPENDENCIES
  None

SIDE EFFECTS
  None
===========================================================================*/
boolean NetMenuDlgEventHandler(void *pUser, AEEEvent evt, uint16 w, uint32 dw)
{
  CNetApp * pMe = (CNetApp*) pUser;
  IMenuCtl* pICurrentMenu;
  IDialog *pIDialog;
  uint16 wCtrlID;
  int nReturnStatus;
  boolean bReturnValue;

  if (pMe == NULL) { /* error checking */
    NET_ERR ("Null pointer", 0,0,0);
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
    NET_MSG ("MenuDlg: EVT_DIALOG_START", 0, 0, 0);
    wCtrlID = NetGetDlgMenuCtlID(pMe, (IDialog*)dw);
    if(wCtrlID == 0) {
      NET_ERR ("Null pointer", 0,0,0);
      return FALSE;
    }
    pICurrentMenu = (IMenuCtl*)IDIALOG_GetControl((IDialog*)dw, (int16) wCtrlID);
    if(pICurrentMenu) {
      /* remove certain items based on featurization */
      NetStripFeatureMenuItem(pMe, pICurrentMenu, wCtrlID);
      SetDefaultMenuLook(pMe->a.m_pIShell, pICurrentMenu);
      return TRUE;
    }
    NET_ERR ("Null pointer", 0,0,0);
    return FALSE; /* no menu control ---> error */

  case EVT_KEY:
    NET_MSG ("MenuDlg: EVT_KEY", 0, 0, 0);
    switch(w) {
    case AVK_CLR:
      pIDialog = ISHELL_GetActiveDialog(pMe->a.m_pIShell);
      if(IDIALOG_GetID(pIDialog) == IDD_NETWORK)
      {
        // If we're at the lowest level dialog, close the app, return to coreapp.
        ISHELL_CloseApplet(pMe->a.m_pIShell, FALSE);
        return TRUE; // This shouldn't be executed?
        }
      nReturnStatus = ISHELL_EndDialog(pMe->a.m_pIShell); // shouldn't we void the return value and return true?
      return CheckAEEReturnStatus(nReturnStatus);
    case AVK_END:
      ISHELL_CloseApplet(pMe->a.m_pIShell, FALSE);
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
    NET_MSG ("MenuDlg: EVT_COMMAND", 0, 0, 0);

    bReturnValue = NetCreateMenuSelectedItemDlg(pMe);
    if (bReturnValue == FALSE) {
      NET_ERR ("Could not create dialog", 0, 0, 0);
    }
    return TRUE;

  case EVT_DIALOG_END:
    NET_MSG ("MenuDlg: EVT_DIALOG_END", 0, 0, 0);
    return TRUE;

  default: /* Other events that have not been handled by the handler */
    break;
  }
  //The compiler wants this, lint doesn't.
  return FALSE; //lint !e527
} /* End MenuDlgEventHandler */

/*===========================================================================
FUNCTION NetRadioButtonDlgEventHandler

DESCRIPTION
  Handle radio button event

DEPENDENCIES
  None

SIDE EFFECTS
  None
===========================================================================*/
boolean NetRadioButtonDlgEventHandler(void *pUser, AEEEvent evt, uint16 w, uint32 dw)
{
  int nReturnStatus;
  CNetApp * pMe = (CNetApp*) pUser;
  IDialog *pIDialog;
  IMenuCtl* pICurrentMenu;
  uint16 wCtrlID;

  if ( pMe == NULL ) { /* error checking */
    NET_ERR ("Null pointer", 0,0,0);
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
    NET_MSG ("RadioButnDlg: EVT_DIALOG_START", 0, 0, 0);
    return NetInitRadioButtonDlg(pMe, w, (IDialog*)dw);

  case EVT_KEY:
    NET_MSG ("RadioButnDlg: EVT_KEY", 0, 0, 0);
    switch(w) {
    case AVK_CLR:
      nReturnStatus = ISHELL_EndDialog(pMe->a.m_pIShell);
      return CheckAEEReturnStatus(nReturnStatus);

    case AVK_END:
          ISHELL_CloseApplet(pMe->a.m_pIShell, FALSE);
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
      return TRUE; /* ignore all other keys */
    case AVK_INFO:  /* power down */
    default:
      return FALSE;
    }

  case EVT_COMMAND:
    NET_MSG ("RadioButnDlg: EVT_COMMAND", 0, 0, 0);    

    pIDialog = ISHELL_GetActiveDialog(pMe->a.m_pIShell);
    if(pIDialog == NULL) {
      NET_ERR ("Null pointer", 0,0,0);
      return FALSE;
    }

#if defined (FEATURE_WCDMA) || defined (FEATURE_GSM)
    if(IDIALOG_GetID(pIDialog) == IDD_NETWORK_SELECT)
    {
      if(w == IDL_NETSELECT_AUTOMATIC ||
         w == IDL_NETSELECT_MANUAL ||
         w == IDL_NETSELECT_MANUAL_WRAT)
      {
        if(NetAppSelectNetwork(pMe, w) != SUCCESS)
          NET_ERR ("Could not signal coreapp", 0, 0, 0);        
        return TRUE; // if SelectNetwork is successful, app will close      
      }
    }
#endif

    /* user makes (new) selection */

    wCtrlID = NetGetDlgMenuCtlID(pMe, pIDialog);
    if(wCtrlID == 0) {
      NET_ERR ("Null pointer ", 0,0,0);
      return FALSE;
    }
    pICurrentMenu= (IMenuCtl*) IDIALOG_GetControl(pIDialog, (int16)wCtrlID);
    if(pICurrentMenu == NULL) {
      NET_ERR ("Null pointer", 0,0,0);
      return FALSE;
    }

    if(NetUpdateSetting(pMe, w, pICurrentMenu))
    {
      nReturnStatus = ISHELL_SetTimer(pMe->a.m_pIShell,
                                      NETSELECTIONVERIFICATIONTIME,
                                      (PFNNOTIFY) NetEndCurrentDlg,
                                      (uint32*) pMe);    
      return TRUE;
    }
    else
    {
      IDialog* pIActiveDialog;
      uint16 wDlgID;
      uint16 wStringID = IDS_CHANGE_FAILED;

      /* Display appropriate error message */
      pIActiveDialog = ISHELL_GetActiveDialog(pMe->a.m_pIShell);
      if(pIActiveDialog == NULL) {
        NET_ERR ("Null pointer", 0,0,0);
        return FALSE;
      }

      wDlgID = IDIALOG_GetID(pIActiveDialog);
      if (wDlgID == 0) { /*error checking */
        NET_ERR ("Null pointer", 0,0,0);
        return FALSE;
      }
      wStringID = NetLookupSystemPrefMsgString(wDlgID);
      NetDisplayMessageDlg(pMe, wStringID, 3);
      return TRUE;
    }

  case EVT_DIALOG_END:
    NET_MSG ("RadioButnDlg: EVT_DIALOG_END", 0, 0, 0);
    return TRUE;

  default: /* Other events that have not been handled by the handler */
    return FALSE;
  }
} /* End NetRadioButtonDlgEventHandler */


/*===========================================================================
FUNCTION NetCheckBoxDlgEventHandler

DESCRIPTION
  Handle check box event

DEPENDENCIES
  None

SIDE EFFECTS
  None
===========================================================================*/
boolean NetCheckBoxDlgEventHandler(void *pUser, AEEEvent evt, uint16 w, uint32 dw)
{
  int nReturnStatus;
  CNetApp * pMe = (CNetApp*) pUser;
  IMenuCtl* pIMenu;
  IDialog *pIDialog;

  if ( pMe == NULL ) { /* error checking */
    NET_ERR ("Null pointer", 0,0,0);
    return FALSE;
  }

#ifdef FEATURE_BREW_3_0
  // do not want to handle autorepeat key events
  if((evt == EVT_KEY) && ((dw & KB_AUTOREPEAT) != 0))
  {
    return FALSE;
  }
#endif

  pIDialog = ISHELL_GetActiveDialog(pMe->a.m_pIShell);
  pIMenu = (IMenuCtl*) IDIALOG_GetControl(pIDialog, (int16) NetGetDlgMenuCtlID(pMe, pIDialog));

  switch (evt) {
  case EVT_DIALOG_START:
    NET_MSG ("CheckBoxDlg: EVT_DIALOG_START", 0, 0, 0);
    return NetInitCheckBoxDlg(pMe, w, (IDialog*)dw);

  case EVT_KEY:
    NET_MSG ("CheckBoxDlg: EVT_KEY", 0, 0, 0);
    switch(w) {
    case AVK_CLR:
      nReturnStatus = ISHELL_EndDialog(pMe->a.m_pIShell);
      return CheckAEEReturnStatus(nReturnStatus);
    case AVK_END:
      ISHELL_CloseApplet(pMe->a.m_pIShell, FALSE);
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
      return TRUE; /* ignore all other keys */

    case AVK_INFO:  /* power down */
      return FALSE;

    default:
      return FALSE;
    }

  case EVT_COMMAND:
    NET_MSG ("CheckBoxDlg: EVT_COMMAND", 0, 0, 0);
    /* user makes (new) selection */

    if(IMENUCTL_IsActive(pIMenu))// Sk Is Not Active
    {
       uint16 wItemID = IMENUCTL_GetSel(pIMenu);
       boolean setBox = !GetCheckBoxVal(pIMenu, wItemID);
       SetCheckBoxItem (pIMenu, wItemID, setBox);
       return TRUE; // Do nothing
    }
    else if (NetUpdateCheckBoxSetting(pMe,pIMenu))
    {
      /*Sk Is Active*/
      nReturnStatus = ISHELL_SetTimer(pMe->a.m_pIShell,
                                      NETSELECTIONVERIFICATIONTIME,
                                      (PFNNOTIFY) NetEndCurrentDlg,
                                      (uint32*) pMe);
    }
    else
    {
      IDialog* pIActiveDialog;
      uint16 wDlgID;

      /* Display appropriate error message */
      pIActiveDialog = ISHELL_GetActiveDialog(pMe->a.m_pIShell);
      if(pIActiveDialog == NULL) {
        NET_ERR ("Null pointer", 0,0,0);
        return FALSE;
      }

      wDlgID = IDIALOG_GetID(pIActiveDialog);
      if (wDlgID == 0) { /*error checking */
        NET_ERR ("Null pointer", 0,0,0);
        return FALSE;
      }
      if ((wDlgID == IDD_NETWORK_FORCE_MODE) || (wDlgID == IDD_NETWORK_BAND_PREF)) {
        NetDisplayMessageDlg(pMe, IDS_MODE_BAND_PREF_INVALID, 3);
      }
      else {
        NetDisplayMessageDlg(pMe, IDS_CHANGE_FAILED, 3);
      }
    }
    
    return TRUE;

  case EVT_DIALOG_END:
    NET_MSG ("CheckBoxDlg: EVT_DIALOG_END", 0, 0, 0);
    return TRUE;

  default: /* Other events that have not been handled by the handler */
    return FALSE;
  }  
} /* End NetCheckBoxDlgEventHandler */


/*===========================================================================
FUNCTION NetStaticDlgEventHandler

DESCRIPTION
  handle static dlg events

DEPENDENCIES
  NetInitStaticDlg, CreateTextEditDlg,

SIDE EFFECTS
  None
===========================================================================*/
boolean NetStaticDlgEventHandler(void *pUser, AEEEvent evt, uint16 w, uint32 dw)
{
  CNetApp * pMe = (CNetApp*) pUser;
  int nReturnStatus;

  if (pMe == NULL) { /* error checking */
    NET_ERR ("Null pointer", 0,0,0);
    return FALSE;
  }

#ifdef FEATURE_BREW_3_0
  // do not want to handle autorepeat key events
  if((evt == EVT_KEY) && ((dw & KB_AUTOREPEAT) != 0))
  {
    return FALSE;
  }
#endif

  switch(evt) {
  case EVT_DIALOG_START:
    NET_MSG ("StaticDlg: EVT_DIALOG_START", 0, 0, 0);
      return NetInitStaticDlg(pMe, w, (IDialog*)dw);

  case EVT_KEY:
    NET_MSG ("StaticDlg: EVT_KEY", 0, 0, 0);
    switch(w) {
    case AVK_CLR:
      nReturnStatus = ISHELL_EndDialog(pMe->a.m_pIShell);
      return CheckAEEReturnStatus(nReturnStatus);
    case AVK_END:
      ISHELL_CloseApplet(pMe->a.m_pIShell, FALSE);
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
    NET_MSG ("StaticDlg: EVT_COMMAND", 0, 0, 0);

    if (w == IDL_AVOID_SYS_OK) { /* avoid current serving system */
      boolean bReturnStatus;
      bReturnStatus = NetAvoidCurrentSystem(pMe, 0);
      if (bReturnStatus == FALSE) {
        NET_ERR("Couldnt avoid current system",0,0,0);
      }
      nReturnStatus = ISHELL_EndDialog(pMe->a.m_pIShell);
      return CheckAEEReturnStatus(nReturnStatus);
    }
    else if (w == IDL_WAKEUP_1X_OK) { /* wakeup 1x */
      nReturnStatus = ICM_WakeupFromStandby(pMe->m_pICM);
      if (nReturnStatus != AEE_SUCCESS) {
        NET_ERR("ICM_WakeupFromStandby failed %d", nReturnStatus,0,0);
      }
      nReturnStatus = ISHELL_EndDialog(pMe->a.m_pIShell);
      return CheckAEEReturnStatus(nReturnStatus);
    }
    else if (w == IDL_WAKEUP_HDR_OK) { /* wakeup HDR */ // mark
      nReturnStatus = ICM_SetSystemPreference(pMe->m_pICM, AEECM_MODE_PREF_CDMA_ONLY,
                             AEECM_PREF_TERM_PWR_CYCLE, 0,
                             AEECM_GW_ACQ_ORDER_PREF_NO_CHANGE,
                             AEECM_BAND_PREF_NO_CHANGE, AEECM_ROAM_PREF_NO_CHANGE,
                             AEECM_HYBR_PREF_NO_CHANGE,
                             AEECM_SRV_DOMAIN_PREF_NO_CHANGE,
                             AEECM_NETWORK_SEL_MODE_PREF_NO_CHANGE,
                             NULL,
                             NULL,
                             NULL);
      if (nReturnStatus != AEE_SUCCESS) {
        NET_ERR("ICM_SetSystemPreference failed %d", nReturnStatus,0,0);
      }

      nReturnStatus = ICM_SetSystemPreference(pMe->m_pICM, AEECM_MODE_PREF_PERSISTENT,
                             AEECM_PREF_TERM_PERMANENT, 0,
                             AEECM_GW_ACQ_ORDER_PREF_NO_CHANGE,
                             AEECM_BAND_PREF_NO_CHANGE, AEECM_ROAM_PREF_NO_CHANGE,
                             AEECM_HYBR_PREF_NO_CHANGE,
                             AEECM_SRV_DOMAIN_PREF_NO_CHANGE,
                             AEECM_NETWORK_SEL_MODE_PREF_NO_CHANGE,
                             NULL,
                             NULL,
                             NULL);
      if (nReturnStatus != AEE_SUCCESS) {
        NET_ERR("ICM_SetSystemPreference failed %d", nReturnStatus,0,0);
      }
      nReturnStatus = ISHELL_SetTimer(pMe->a.m_pIShell,
                                      NETSELECTIONVERIFICATIONTIME,
                                      (PFNNOTIFY) NetEndCurrentDlg,
                                      (uint32*) pMe);
      return TRUE;
    }
    
    return FALSE;

  case EVT_DIALOG_END:
    NET_MSG ("StaticDlg: EVT_DIALOG_END", 0, 0, 0);
    return TRUE;

  default: /* Other events that have not been handled by the handler */
    return FALSE;
  }

} /* End NetStaticDlgEventHandler */

/*===========================================================================
FUNCTION NetMessageDlgEventHandler

DESCRIPTION
  handle message dialog events

DEPENDENCIES
  None

SIDE EFFECTS
  None
===========================================================================*/
boolean NetMessageDlgEventHandler(void *pUser, AEEEvent evt, uint16 w, uint32 dw)
{
  IStatic * pIStatic;
  IDialog * pIDialog;
  uint16 wDlgID;

  CNetApp * pMe = (CNetApp*) pUser;

  if (pMe == NULL) { /* error checking */
    NET_ERR ("Null pointer", 0,0,0);
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
        (void) NetSetGeneralStaticTextStyle(pIStatic);
        pBuf = MALLOC (MAX_STR_SIZE*sizeof(AECHAR));
        if (pBuf == NULL) {
          NET_ERR ("No memory", 0,0,0);
          return FALSE;
        }
        if (ISHELL_LoadResString (pMe->a.m_pIShell, gpNetResourceFile, pMe->m_MessageID,
                                  pBuf, MAX_STR_SIZE * sizeof(AECHAR)) != 0) {
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
      NET_ERR("FAILED to get pIStatic for control id",0,0,0);
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

      /* ending the message NOW -> cancel end message timer */
      ISHELL_CancelTimer(pMe->a.m_pIShell, (PFNNOTIFY) NetEndMessageDlg, pMe);
      NetEndMessageDlg(pMe);

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
      ISHELL_CancelTimer(pMe->a.m_pIShell, (PFNNOTIFY) NetEndMessageDlg, pMe);
      NetEndMessageDlg(pMe);
      return TRUE;

    default:
      return FALSE; /* let core app handles these events */
    }

  case EVT_COMMAND:
    return TRUE;

  case EVT_DIALOG_END:
    ISHELL_CancelTimer(pMe->a.m_pIShell, (PFNNOTIFY) NetEndMessageDlg, pMe);
    /* Clear all variables and free memory if any */
    pMe->m_MessageID = 0;
    if (pMe->m_pszMsgString) {
      FREE (pMe->m_pszMsgString);
      pMe->m_pszMsgString = NULL;
    }

    return TRUE;

  default: /* Other events that have not been handled by the handler */
    return TRUE;
  }
} /*End NetMessageDlgEventHandler*/

/*===========================================================================
FUNCTION NetCreateMenuSelectedItemDlg

DESCRIPTION
  It obtains the current menu selection and create the next level dialog
  accordingly

DEPENDENCIES
  NetGetDlgMenuCtlID, GetNextSeletedItemDlgID

SIDE EFFECTS
  None
===========================================================================*/
static boolean NetCreateMenuSelectedItemDlg(CNetApp *pMe)
{
  IMenuCtl* pICurrentMenu;
  uint16 wCtrlID;
  uint16 wDlgID;
  uint16 wItemID;
  IDialog* pIActiveDialog;
  int nReturnStatus;

  pIActiveDialog = ISHELL_GetActiveDialog(pMe->a.m_pIShell);
  if (pIActiveDialog == NULL ) {
    NET_ERR ("Null pointer ", 0,0,0);
    return FALSE;
  }

  /* Get the menu control */
  wCtrlID = NetGetDlgMenuCtlID(pMe, pIActiveDialog);
  if(wCtrlID == 0) {
    NET_ERR ("Null pointer ", 0,0,0);
    return FALSE;
  }

  pICurrentMenu = (IMenuCtl*) IDIALOG_GetControl(pIActiveDialog, (int16) wCtrlID);

  if(pICurrentMenu == NULL) {
    NET_ERR ("Null pointer ", 0,0,0);
    return FALSE;
  }

  /* Get the selected item */
  wItemID = IMENUCTL_GetSel((IMenuCtl*)pICurrentMenu);

  /* Get next dialog ID */
  wDlgID = NetGetNextSelectedItemDlgID(pMe, wItemID);

  if(wDlgID == 0) { /* error */
    NET_ERR ("Null pointer ", 0,0,0);
    return FALSE;
  }

  /* Create next dialog */
  nReturnStatus = ISHELL_CreateDialog(pMe->a.m_pIShell, gpNetResourceFile,
              wDlgID, NULL);

  return CheckAEEReturnStatus(nReturnStatus);

} /* End NetCreateMenuSelectedItemDlg */

/*===========================================================================
FUNCTION NetGetDlgMenuCtlID

DESCRIPTION
  This function matches the dialog ID with a corresponding menu control ID

DEPENDENCIES
  Note: 1. This function should be called in a Menu dialog ONLY
        2. the pIActiveDialog should be test for null pointer in the calling
           function

SIDE EFFECTS
  None
===========================================================================*/
static uint16 NetGetDlgMenuCtlID(CNetApp *pMe, IDialog* pIActiveDialog)
/*lint -esym(715,pMe)*/
{
  uint16 wDlgID;
  int i;

  if  ( pIActiveDialog == NULL ) {
    return 0;
  }

  wDlgID = IDIALOG_GetID(pIActiveDialog);

/*lint -save -e737 -e574*/
  for (i = 0; i< ARR_SIZE(NetMenuActionTbl); i++) {
    if (NetMenuActionTbl[i].wDialogID == wDlgID) {
      return NetMenuActionTbl[i].wControlID;
    }
  }
/*lint -restore*/
  NET_ERR( "NetGetDlgMenuCtlID(): No matching menu ctl", 0, 0, 0);
  return 0; /* error */
} /* End NetGetDlgMenuCtlID */

/*===========================================================================
FUNCTION GetNextSelectedItemDlgID

DESCRIPTION
  The function is used when a menu control selection is being made.
  It returns the next dialog ID that corresponds to the item selected

DEPENDENCIES
  Note: This function should ONLY be called from a menu dialog which
    will create another dialog

SIDE EFFECTS
  None
===========================================================================*/
static uint16 NetGetNextSelectedItemDlgID(CNetApp *pMe, uint16 wItemID)
/*lint -esym(715,pMe)*/
{
  int i;
  
/*lint -save -e737 -e574*/
  for (i = 0; i< ARR_SIZE(NetMenuActionTbl); i++) {
    if ((NetMenuActionTbl[i].wItemID == wItemID) &&
        (NetMenuActionTbl[i].wActionType == LAUNCH_DIALOG)) {
       return ((uint16) NetMenuActionTbl[i].nActionData);
    }
  }
/*lint -restore */
  NET_ERR( "NetGetNextSelectedItemDlgID(): No next dlgID found", 0, 0, 0);
  return 0; /* error */
} /* End NetGetNextSelectedItemDlgID */

/*===========================================================================
FUNCTION NetUpdateCheckBoxSetting

DESCRIPTION
  Save the setting of given item through Set methods.
  Return TRUE if the setting was changed and FALSE otherwise.

DEPENDENCIES
  Various SetXXXSetting functions

SIDE EFFECTS
  None
===========================================================================*/
boolean NetUpdateCheckBoxSetting(CNetApp *pMe, IMenuCtl *pICurrentMenu)
{
  int i, j;

  IDialog* pIActiveDialog = (IDialog*)ISHELL_GetActiveDialog(pMe->a.m_pIShell);

  uint16 wDlgID = IDIALOG_GetID(pIActiveDialog);
  uint64 nNewSetting =0;
  boolean setValue = FALSE;
  boolean matchFound = FALSE;
  SetFnPtrType   *setfnPtr = NULL;

/*lint -save -e737 -e574*/
  for (i=0; i < ARR_SIZE(NetMenuActionTbl); i++) {
    if (NetMenuActionTbl[i].wDialogID == wDlgID) {
      /* got a match */
      if (NetMenuActionTbl[i].wActionType != MENU_SETTING) {
        /* Not a menu setting - here in error */
        NET_ERR ("Invalid action for dialog %d", wDlgID, 0, 0);
        return FALSE;
      }
      for (j=i; (j < ARR_SIZE(NetMenuActionTbl)) && (NetMenuActionTbl[j].wDialogID == wDlgID); j++) {

        if (NetMenuActionTbl[j].setfnPtr == NULL) {
          /* Null Get fn pointer */
          NET_ERR ("Invalid function pointer %d", wDlgID, 0, 0);
          return FALSE;
        }
        /*If Check box is set, then unset it*/
        setValue = GetCheckBoxVal(pICurrentMenu, NetMenuActionTbl[j].wItemID);
        if(setValue)
        {
           nNewSetting |= NetMenuActionTbl[j].nActionData;
        }

        setfnPtr = NetMenuActionTbl[j].setfnPtr;
        matchFound = TRUE;
      }
      if(matchFound)
         break;
    }
  }
/*lint -restore*/

  if(matchFound)
  {
    return ((setfnPtr)(pMe, nNewSetting));
  }

  /* No match found */
  NET_ERR ("No match for  dialog %d", wDlgID, 0, 0);
  return FALSE;

}/* End NetUpdateCheckBoxSetting */

