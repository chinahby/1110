/*===========================================================================

FILE: NetUtils.c

DESCRIPTION
   This file contains utility functions for Net app.

PUBLIC CLASSES:
   N/A

INITIALIZATION AND SEQUENCING REQUIREMENTS:


       Copyright © 2006-2007 QUALCOMM Incorporated.
                 All Rights Reserved.
               QUALCOMM Proprietary/GTDR
===========================================================================*/
/*===========================================================================
                        EDIT HISTORY FOR MODULE

$Header: //depot/asic/msmshared/apps/NetSettings/NetUtils.c#7 $

when       who     what, where, why
--------   ---     ----------------------------------------------------------
03/20/07   jas/ak  Adding support for persistent preference values.
09/13/06   jas     Adding header file for JCDMA NV access
09/01/06   jas     Complete the renaming of symbols formerly in CoreApp
09/01/06   jas     Resolve name collision with CoreApp
08/31/06   jas     Eliminating NV_VALUE_ERROR_BOOLEAN
06/20/06   joe     Initial revision

===========================================================================*/


/*=============================================================================

                INCLUDES AND VARIABLE DEFINITIONS

=============================================================================== */
#include "OEMFeatures.h"
#include "AEEStdLib.h"
#include "NetSettings.h"
#include "NetMenu.h"
#include "NetUtils.h"
#include "NetSettings.bid"
#include "AppComFunc.h"
#include "AEEFile.h"
#include "AEECM.h"
#ifdef FEATURE_JCDMA_1X 
#include "nv.h"
#include "sys.h"
#include "uiutils.h"
#endif /* FEATURE_JCDMA_1X */

/*=========================================================================*/
/* Globals and constants */
/*=========================================================================*/

static AEECallback NetUtil_StatusCb;
static AEECMPhError status;

#define NETUTIL_VOICE_AS_DATA_TIME 600


/*=========================================================================*/
/* Function Declarations */
/*=========================================================================*/

/*===========================================================================
FUNCTION NetUtil_SystemPrefErrCb

DESCRIPTION
  Callback function passed to ICM for System Preference Setting

DEPENDENCIES
  None

SIDE EFFECTS
  None
===========================================================================*/
void NetUtil_SystemPrefErrCb(void *pNotifyData)
{
  CNetApp * pMe = (CNetApp *) pNotifyData;

  if (pMe == NULL) {
    NET_ERR ("Null pointer", 0,0,0);
    return;
  }
  ISHELL_PostEventEx(pMe->a.m_pIShell, EVTFLG_ASYNC, AEECLSID_NETSETTINGS,
                       EVT_USER, EVT_SYS_PREF_ERR, status);

  return;
}

#ifdef FEATURE_JCDMA_1X 

/*===========================================================================
FUNCTION NetGetM511Mode

DESCRIPTION
  Get M51 settings from NV, update static variables

DEPENDENCIES
  ui_get_nv

SIDE EFFECTS
  None
===========================================================================*/
uint64 NetGetM511Mode(CNetApp * pMe)
{

  nv_item_type nvi;
  nv_stat_enum_type result;

  if (pMe == NULL) { /* error checking */
    NET_ERR ("Null pointer", 0,0,0);
    return ((uint64) FALSE);
  }

  result = ui_get_nv(NV_JCDMA_M511_MODE_I, &nvi);

  if(result != NV_DONE_S) {
    nvi.jcdma_m511_mode = SYS_JCDMA_M511_PKT;
    (void) ui_put_nv (NV_JCDMA_M511_MODE_I, &nvi);
  }

  return (uint64)(nvi.jcdma_m511_mode);
}

/*===========================================================================
FUNCTION NetGetM512Mode

DESCRIPTION
  Get M51 settings from NV, update static variables

DEPENDENCIES
  ui_get_nv

SIDE EFFECTS
  None
===========================================================================*/
uint64 NetGetM512Mode(CNetApp * pMe)
{

  nv_item_type nvi;
  nv_stat_enum_type result;

  if (pMe == NULL) { /* error checking */
    NET_ERR ("Null pointer", 0,0,0);
    return ((uint64) FALSE);
  }

  result = ui_get_nv(NV_JCDMA_M512_MODE_I, &nvi);

  if(result != NV_DONE_S) {
    nvi.jcdma_m512_mode = SYS_JCDMA_M512_HIGH;
    (void) ui_put_nv (NV_JCDMA_M512_MODE_I, &nvi);
  }

  return (uint64)(nvi.jcdma_m512_mode);
}

/*===========================================================================
FUNCTION NetGetM513Mode

DESCRIPTION
  Get M51 settings from NV, update static variables

DEPENDENCIES
  ui_get_nv

SIDE EFFECTS
  None
===========================================================================*/
uint64 NetGetM513Mode(CNetApp * pMe)
{

  nv_item_type nvi;
  nv_stat_enum_type result;

  if (pMe == NULL) { /* error checking */
    NET_ERR ("Null pointer", 0,0,0);
    return ((uint64) FALSE);
  }

  result = ui_get_nv(NV_JCDMA_M513_MODE_I, &nvi);

  if(result != NV_DONE_S) {
    nvi.jcdma_m513_mode = SYS_JCDMA_M513_AUTO;;
    (void) ui_put_nv (NV_JCDMA_M513_MODE_I, &nvi);
  }

  return (uint64)(nvi.jcdma_m513_mode);
}


/*===========================================================================
FUNCTION NetSetM511Mode

DESCRIPTION
  Save M511 settings in NV, update static variables

DEPENDENCIES
  ui_get_nv

SIDE EFFECTS
  None
===========================================================================*/
boolean NetSetM511Mode (CNetApp * pMe, uint64 value)
{

  nv_item_type nvi;

  if (pMe == NULL) { /* error checking */
    NET_ERR ("Null pointer", 0,0,0);
    return FALSE;
  }

  if( cm_change_m511_mode((sys_jcdma_m511_e_type) value)) {
     nvi.jcdma_m511_mode = (uint8)value;
     (void) ui_put_nv(NV_JCDMA_M511_MODE_I, &nvi);
     return TRUE;
  }
  return FALSE;
}

/*===========================================================================
FUNCTION NetSetM512Mode

DESCRIPTION
  Save M512 settings in NV, update static variables

DEPENDENCIES
  ui_get_nv

SIDE EFFECTS
  None
===========================================================================*/
boolean NetSetM512Mode (CNetApp * pMe, uint64 value)
{

  nv_item_type nvi;

  if (pMe == NULL) { /* error checking */
    NET_ERR ("Null pointer", 0,0,0);
    return FALSE;
  }

  if( cm_change_m512_mode((sys_jcdma_m512_e_type) value)) {
    nvi.jcdma_m512_mode = (uint8)value;
    (void) ui_put_nv(NV_JCDMA_M512_MODE_I, &nvi);
    return TRUE;
  }
  return FALSE;
}


/*===========================================================================
FUNCTION NetSetM513Mode

DESCRIPTION
  Save M513 settings in NV, update static variables

DEPENDENCIES
  ui_get_nv

SIDE EFFECTS
  None
===========================================================================*/
boolean NetSetM513Mode (CNetApp * pMe, uint64 value)
{

  nv_item_type nvi;

  if (pMe == NULL) { /* error checking */
    NET_ERR ("Null pointer", 0,0,0);
    return FALSE;
  }

  if( cm_change_m513_mode((sys_jcdma_m513_e_type) value)) {
    nvi.jcdma_m513_mode = (uint8)value;
    (void) ui_put_nv(NV_JCDMA_M513_MODE_I, &nvi);
    return TRUE;
  }
  return FALSE;
}

#endif /* FEATURE_JCDMA_1X */

/*===========================================================================
FUNCTION NetGetForceModePref

DESCRIPTION
  Initialize force mode menu

DEPENDENCIES
  SK UI

SIDE EFFECTS
  None
===========================================================================*/
uint64 NetGetForceModePref(CNetApp *pMe)
{
  AEECMPhInfo   phInfo;
  if (pMe == NULL) {
    NET_ERR ("Null pointer", 0,0,0);
    return (uint64)(FALSE);
  }

  /* Request the lastest phone and serving system information */
  if(ICM_GetPhoneInfo(pMe->m_pICM, &phInfo, sizeof(AEECMPhInfo)) != SUCCESS)
    return (uint64) FALSE;
/*lint -save -e571*/
  return (uint64) (phInfo.mode_pref);
/*lint -restore */
}


/*===========================================================================
FUNCTION NetSetForceModePref

DESCRIPTION

  Change force mode setting.
  Save it and return TRUE if successful, return FALSE otherwise.

DEPENDENCIES
  None

SIDE EFFECTS
  None
===========================================================================*/
boolean NetSetForceModePref(CNetApp *pMe, uint64 nNewSetting)
{
  AEECMPhInfo   phInfo;
  int nReturnStatus = SUCCESS;

  if (pMe == NULL) {
    NET_ERR ("Null pointer", 0,0,0);
    return FALSE;
  }

  /* Request the lastest phone and serving system information */  
  if(SUCCESS !=ICM_GetPhoneInfo(pMe->m_pICM, &phInfo, sizeof(AEECMPhInfo)))
    return FALSE;
  
  /* Check if valid before sending it to CM */
  if (ICM_IsValidModeBandPref(pMe->m_pICM,
                              (AEECMModePref) nNewSetting,
                               phInfo.band_pref)) {

    NetUtil_StatusCb.pfnCancel = NULL;
    NetUtil_StatusCb.pfnNotify = NetUtil_SystemPrefErrCb;
    NetUtil_StatusCb.pNotifyData = pMe;

    /* Send CM command */
    nReturnStatus = ICM_SetSystemPreference(pMe->m_pICM,
                      (AEECMModePref) nNewSetting, AEECM_PREF_TERM_PERMANENT,
                      0, AEECM_GW_ACQ_ORDER_PREF_NO_CHANGE,
                      phInfo.band_pref, AEECM_ROAM_PREF_NO_CHANGE,
                      AEECM_HYBR_PREF_NO_CHANGE, AEECM_SRV_DOMAIN_PREF_NO_CHANGE,
                      AEECM_NETWORK_SEL_MODE_PREF_NO_CHANGE, NULL, /* plmn id */
                      &status, &NetUtil_StatusCb /* error cb */
                      );
    if (nReturnStatus == SUCCESS) {
      return TRUE;
    }
    else {
      return FALSE;
    }
  }
  return FALSE;
}

#ifdef FEATURE_MMGSDI_DUAL_SLOT
/*===========================================================================
FUNCTION NetSetForceModePrefSlotCheck

DESCRIPTION

  Change force mode setting.
  Save it and return TRUE if successful, return FALSE otherwise.

DEPENDENCIES
  None

SIDE EFFECTS
  None
===========================================================================*/
uint8 NetSetForceModePrefSlotCheck(CNetApp *pMe, uint64 nNewSetting)
{
  boolean bOkToSetModeGSM = FALSE;
  boolean bOkToSetModeCDMA= FALSE;
  boolean bNeedSwitchSlotGSM = FALSE;
  boolean bNeedSwitchSlotCDMA = FALSE;
  uint16 wDlgID = 0;

  if (pMe == NULL) {
    NET_ERR ("Null pointer", 0,0,0);
    return FALSE;
  }

  if ((pMe->m_dwSecurityMask & NETAPP_SLOT1_GSM_OPERATIONAL_MASK) ==
      NETAPP_SLOT1_GSM_OPERATIONAL_MASK) {
    /* GSM operational slot from 1 */
    if ((pMe->m_dwSecurityMask & NETAPP_SLOT1_APP_GSM_MASK) ==
        NETAPP_SLOT1_APP_GSM_MASK) {
      bNeedSwitchSlotGSM = FALSE;
      bOkToSetModeGSM = TRUE;
    }
    else if ((pMe->m_dwSecurityMask & NETAPP_SLOT2_APP_GSM_MASK) ==
             NETAPP_SLOT2_APP_GSM_MASK) {
      /* slot 2 has GSM available */
      /* switch slot */
      DetermineSelectCardDlg(pMe, &wDlgID, FALSE);
      bNeedSwitchSlotGSM = TRUE;
      bOkToSetModeGSM = FALSE;
    }
    else
    {
      /* no GSM app */
      bNeedSwitchSlotGSM = FALSE;
      bOkToSetModeGSM = FALSE;
    }
  }
  else if ((pMe->m_dwSecurityMask & NETAPP_SLOT2_GSM_OPERATIONAL_MASK) ==
           NETAPP_SLOT2_GSM_OPERATIONAL_MASK) {
    /* GSM operational slot from 2 */
    if ((pMe->m_dwSecurityMask & NETAPP_SLOT2_APP_GSM_MASK) ==
        NETAPP_SLOT2_APP_GSM_MASK) {
      /* has GSM in slot 2, no need to do switch slot */
      bNeedSwitchSlotGSM = FALSE;
      bOkToSetModeGSM = TRUE;
    }
    else if ((pMe->m_dwSecurityMask & NETAPP_SLOT1_APP_GSM_MASK) ==
             NETAPP_SLOT1_APP_GSM_MASK) {
      /* slot 1 has GSM available */
      /* switch slot */
      DetermineSelectCardDlg(pMe, &wDlgID, FALSE);
      bNeedSwitchSlotGSM = TRUE;
      bOkToSetModeGSM = FALSE;
    }
    else
    {
      /* no GSM app */
      bNeedSwitchSlotGSM = FALSE;
      bOkToSetModeGSM = FALSE;
    }
  }
  else
  {
    /* no operational slot set for GSM */
    if ((pMe->m_dwSecurityMask & NETAPP_SLOT1_APP_GSM_MASK) ==
        NETAPP_SLOT1_APP_GSM_MASK) {
      /* has GSM in slot 1 */
      /* slot 1 has GSM available */
      /* switch slot */
      DetermineSelectCardDlg(pMe, &wDlgID, FALSE);
      bNeedSwitchSlotGSM = TRUE;
      bOkToSetModeGSM = FALSE;
    }
    else if ((pMe->m_dwSecurityMask & NETAPP_SLOT2_APP_GSM_MASK) ==
             NETAPP_SLOT2_APP_GSM_MASK) {
      /* slot 2 has GSM available */
      /* switch slot */
      DetermineSelectCardDlg(pMe, &wDlgID, FALSE);
      bNeedSwitchSlotGSM = TRUE;
      bOkToSetModeGSM = FALSE;
    }
    else
    {
      /* no GSM app */
      bNeedSwitchSlotGSM = FALSE;
      bOkToSetModeGSM = FALSE;
    }
  }
  /* CDMA */
  /* check NV RTRE setting */
  if (((pMe->m_rtreConfig == CM_RTRE_CONFIG_RUIM_OR_DROP_BACK) &&
      (pMe->m_rtreControl == CM_RTRE_CONTROL_NV)) ||
      (pMe->m_rtreConfig == CM_RTRE_CONFIG_NV_ONLY)) {
    bNeedSwitchSlotCDMA = FALSE;
    bOkToSetModeCDMA = TRUE;
  }
  else
  {
    if ((pMe->m_dwSecurityMask & NETAPP_SLOT1_CDMA_OPERATIONAL_MASK) ==
        NETAPP_SLOT1_CDMA_OPERATIONAL_MASK) {
      /* CDMA operational slot from 1 */
      if ((pMe->m_dwSecurityMask & NETAPP_SLOT1_APP_RUIM_MASK) ==
          NETAPP_SLOT1_APP_RUIM_MASK) {
        /* has CDMA in slot 1, no need to do switch slot */
        bNeedSwitchSlotCDMA = FALSE;
        bOkToSetModeCDMA = TRUE;
      }
      else if ((pMe->m_dwSecurityMask & NETAPP_SLOT2_APP_RUIM_MASK) ==
               NETAPP_SLOT2_APP_RUIM_MASK) {
        /* slot 2 has CDMA available */
        /* switch slot */
        DetermineSelectCardDlg(pMe, &wDlgID, FALSE);
        bNeedSwitchSlotCDMA = TRUE;
        bOkToSetModeCDMA = FALSE;
      }
      else
      {
        /* no CDMA app */
        bNeedSwitchSlotCDMA = FALSE;
        bOkToSetModeCDMA = FALSE;
      }
    }
    else if ((pMe->m_dwSecurityMask & NETAPP_SLOT2_CDMA_OPERATIONAL_MASK) ==
             NETAPP_SLOT2_CDMA_OPERATIONAL_MASK) {
      /* CDMA operational slot from 2 */
      if ((pMe->m_dwSecurityMask & NETAPP_SLOT2_APP_RUIM_MASK) ==
          NETAPP_SLOT2_APP_RUIM_MASK) {
        /* has CDMA in slot 2, no need to do switch slot */
        bNeedSwitchSlotCDMA = FALSE;
        bOkToSetModeCDMA = TRUE;
      }
      else if ((pMe->m_dwSecurityMask & NETAPP_SLOT1_APP_RUIM_MASK) ==
               NETAPP_SLOT1_APP_RUIM_MASK) {
        /* slot 1 has CDMA available */
        /* switch slot */
        DetermineSelectCardDlg(pMe, &wDlgID, FALSE);
        bNeedSwitchSlotCDMA = TRUE;
        bOkToSetModeCDMA = FALSE;
      }
      else
      {
        /* no CDMA app */
        bNeedSwitchSlotCDMA = FALSE;
        bOkToSetModeCDMA = FALSE;
      }
    }
    else
    {
      /* no operational slot set for CDMA */
      if ((pMe->m_dwSecurityMask & NETAPP_SLOT1_APP_RUIM_MASK) ==
          NETAPP_SLOT1_APP_RUIM_MASK) {
        /* has CDMA in slot 1 */
        /* slot 1 has CDMA available */
        /* switch slot */
        DetermineSelectCardDlg(pMe, &wDlgID, FALSE);
        bNeedSwitchSlotCDMA = TRUE;
        bOkToSetModeCDMA = FALSE;
      }
      else if ((pMe->m_dwSecurityMask & NETAPP_SLOT2_APP_RUIM_MASK) ==
               NETAPP_SLOT2_APP_RUIM_MASK) {
        /* slot 2 has CDMA available */
        /* switch slot */
        DetermineSelectCardDlg(pMe, &wDlgID, FALSE);
        bNeedSwitchSlotCDMA = TRUE;
        bOkToSetModeCDMA = FALSE;
      }
      else
      {
        /* no CDMA app */
        bNeedSwitchSlotCDMA = FALSE;
        bOkToSetModeCDMA = FALSE;
      }
    }
  }

  switch ((cm_mode_pref_e_type)nNewSetting)
  {
    case CM_MODE_PREF_AMPS_ONLY:
    case CM_MODE_PREF_HDR_ONLY:
    case CM_MODE_PREF_CDMA_AMPS_ONLY:
    case CM_MODE_PREF_EMERGENCY:
    case CM_MODE_PREF_DIGITAL_ONLY:
    case CM_MODE_PREF_WCDMA_ONLY:
      if( NetSetForceModePref(pMe, nNewSetting) ) {
        return NETUTILS_FORCE_MODE_SUCCESS;
      }
      else
      {
        return NETUTILS_FORCE_MODE_FAIL;
      }

    case CM_MODE_PREF_AUTOMATIC:
      if (bNeedSwitchSlotCDMA || bNeedSwitchSlotGSM) {
        /* switch slot */
        if (wDlgID == 0) {
          return NETUTILS_FORCE_MODE_FAIL;
        }
        ISHELL_CreateDialog(pMe->a.m_pIShell, gpNetResourceFile, wDlgID, NULL);
        return NETUTILS_PENDING_SLOT_BEFORE_FORCE_MODE;
      }
      else 
      {
        /* both doesn't require switch slot */
        if (!bOkToSetModeGSM && !bOkToSetModeCDMA) {
          return NETUTILS_FORCE_MODE_FAIL;
        }
        else {
          if( NetSetForceModePref(pMe, nNewSetting) ) {
            return NETUTILS_FORCE_MODE_SUCCESS;
          }
          else
          {
            return NETUTILS_FORCE_MODE_FAIL;
          }
        }
      }
    
    case CM_MODE_PREF_GSM_ONLY:
      if (!bNeedSwitchSlotGSM && bOkToSetModeGSM) {
        if( NetSetForceModePref(pMe, nNewSetting) ) {
          return NETUTILS_FORCE_MODE_SUCCESS;
        }
        else
        {
          return NETUTILS_FORCE_MODE_FAIL;
        }
      }
      else if (!bNeedSwitchSlotGSM && !bOkToSetModeGSM) {
        return NETUTILS_FORCE_MODE_FAIL;
      }
      else
      {
        /* switch slot */
        if (wDlgID == 0) {
          return NETUTILS_FORCE_MODE_FAIL;
        }
        ISHELL_CreateDialog(pMe->a.m_pIShell, gpNetResourceFile, wDlgID, NULL);
        return NETUTILS_PENDING_SLOT_BEFORE_FORCE_MODE;
      }
      return NETUTILS_FORCE_MODE_FAIL;
   
    case CM_MODE_PREF_CDMA_ONLY:
      if (!bNeedSwitchSlotCDMA && bOkToSetModeCDMA) {
        if( NetSetForceModePref(pMe, nNewSetting) ) {
          return NETUTILS_FORCE_MODE_SUCCESS;
        }
        else
        {
          return NETUTILS_FORCE_MODE_FAIL;
        }
      }
      else if (!bNeedSwitchSlotCDMA && !bOkToSetModeCDMA) {
        return NETUTILS_FORCE_MODE_FAIL;
      }
      else
      {
        /* switch slot */
        if (wDlgID == 0) {
          return NETUTILS_FORCE_MODE_FAIL;
        }
        ISHELL_CreateDialog(pMe->a.m_pIShell, gpNetResourceFile, wDlgID, NULL);
        return NETUTILS_PENDING_SLOT_BEFORE_FORCE_MODE;
      }   
      return NETUTILS_FORCE_MODE_FAIL;

    default:
      break;
  }
  return NETUTILS_FORCE_MODE_FAIL;
}
#endif /*FEATURE_MMGSDI_DUAL_SLOT */

/*===========================================================================
FUNCTION NetGetBandPref

DESCRIPTION
  Initialize band pref menu

DEPENDENCIES
  Various Get and Set method for the variables in NetUtils.c

SIDE EFFECTS
  None
===========================================================================*/
uint64 NetGetBandPref(CNetApp *pMe)
{
  AEECMPhInfo   phInfo;
  if (pMe == NULL) {
    NET_ERR ("Null pointer", 0,0,0);
    return (uint64)(AEECM_BAND_PREF_NONE);
  }

  /* Request the lastest phone and serving system information */
  if(SUCCESS != ICM_GetPhoneInfo(pMe->m_pICM, &phInfo, sizeof(AEECMPhInfo)))
    return (uint64)(AEECM_BAND_PREF_NONE);
/*lint -save -e571*/
  return (uint64) (phInfo.band_pref);
/*lint -restore */
}


/*===========================================================================
FUNCTION NetSetBandPref

DESCRIPTION

  Change band pref setting.

DEPENDENCIES
  None

SIDE EFFECTS
  None
===========================================================================*/
boolean NetSetBandPref(CNetApp *pMe, uint64 nNewSetting)
{
  AEECMPhInfo   phInfo;
 
  int nReturnStatus;
  if (pMe == NULL) {
    NET_ERR ("Null pointer", 0,0,0);
    return FALSE;
  }

  /* Request the lastest phone and serving system information */
  if(SUCCESS != ICM_GetPhoneInfo(pMe->m_pICM, &phInfo, sizeof(AEECMPhInfo)))
    return FALSE;

  /* Check if valid before sending it to CM */
  if (ICM_IsValidModeBandPref(pMe->m_pICM,
                              phInfo.mode_pref,
                              (AEECMBandPref) nNewSetting)) {
    NetUtil_StatusCb.pfnCancel = NULL;
    NetUtil_StatusCb.pfnNotify = NetUtil_SystemPrefErrCb;
    NetUtil_StatusCb.pNotifyData = pMe;
    /* Send CM command */
    nReturnStatus = ICM_SetSystemPreference(pMe->m_pICM,
                      AEECM_MODE_PREF_NO_CHANGE, AEECM_PREF_TERM_PERMANENT,
                      0, AEECM_GW_ACQ_ORDER_PREF_NO_CHANGE,
                      (AEECMBandPref) nNewSetting, AEECM_ROAM_PREF_NO_CHANGE,
                      AEECM_HYBR_PREF_NO_CHANGE, AEECM_SRV_DOMAIN_PREF_NO_CHANGE,
                      AEECM_NETWORK_SEL_MODE_PREF_NO_CHANGE, NULL, /* plmn id */
                      &status, &NetUtil_StatusCb /* error cb */
                      );
    if (nReturnStatus == SUCCESS) {
      return TRUE;
    }
    else {
      return FALSE;
    }
  }
  return FALSE;
}

/*===========================================================================
FUNCTION NetGetDomainPref

DESCRIPTION
  Initialize domain pref menu

DEPENDENCIES
  Various Get and Set method for the variables in NetUtils.c

SIDE EFFECTS
  None
===========================================================================*/
uint64 NetGetDomainPref(CNetApp *pMe)
{
  AEECMPhInfo   phInfo;
  if (pMe == NULL) {
    NET_ERR ("Null pointer", 0,0,0);
    return (uint64)(FALSE);
  }

  /* Request the lastest phone and serving system information */
  if(SUCCESS != ICM_GetPhoneInfo(pMe->m_pICM, &phInfo, sizeof(AEECMPhInfo)))
    return (uint64) FALSE;
/*lint -save -e571*/
  return (uint64) (phInfo.srv_domain_pref);
/*lint -restore */
}


/*===========================================================================
FUNCTION NetSetDomainPref

DESCRIPTION

  Change domain pref setting.

DEPENDENCIES
  None

SIDE EFFECTS
  None
===========================================================================*/
boolean NetSetDomainPref(CNetApp *pMe, uint64 nNewSetting)
{
  int nReturnStatus;

  if (pMe == NULL) {
    NET_ERR ("Null pointer", 0,0,0);
    return FALSE;
  }

  NetUtil_StatusCb.pfnCancel = NULL;
  NetUtil_StatusCb.pfnNotify = NetUtil_SystemPrefErrCb;
  NetUtil_StatusCb.pNotifyData = pMe;
  /* Send CM command */
  nReturnStatus = ICM_SetSystemPreference(pMe->m_pICM,
                    AEECM_MODE_PREF_NO_CHANGE, AEECM_PREF_TERM_PERMANENT,
                    0, AEECM_GW_ACQ_ORDER_PREF_NO_CHANGE,
                    AEECM_BAND_PREF_NO_CHANGE, AEECM_ROAM_PREF_NO_CHANGE,
                    AEECM_HYBR_PREF_NO_CHANGE, (AEECMServDomPref) nNewSetting,
                    AEECM_NETWORK_SEL_MODE_PREF_NO_CHANGE, NULL, /* plmn id */
                    &status, &NetUtil_StatusCb /* error cb */
                    );
  if (nReturnStatus == SUCCESS) {
    return TRUE;
  }
  else {
    return FALSE;
  }
}

/*===========================================================================
FUNCTION NetGetAcqOrderPref

DESCRIPTION
  Initialize acquistion order

DEPENDENCIES
  Various Get and Set method for the variables in NetUtils.c

SIDE EFFECTS
  None
===========================================================================*/
uint64 NetGetAcqOrderPref(CNetApp *pMe)
{
  AEECMPhInfo   phInfo;
  if (pMe == NULL) {
    NET_ERR ("Null pointer", 0,0,0);
    return (uint64)(FALSE);
  }

  /* Request the lastest phone and serving system information */
  if(SUCCESS != ICM_GetPhoneInfo(pMe->m_pICM, &phInfo, sizeof(AEECMPhInfo)))
    return (uint64) FALSE;

  /*lint -save -e571*/
  return (uint64) (phInfo.acq_order_pref);
  /*lint -restore */
}

/*===========================================================================
FUNCTION NetSetAcqOrderPref

DESCRIPTION

  Change acquistion order setting.

DEPENDENCIES
  None

SIDE EFFECTS
  None
===========================================================================*/
boolean NetSetAcqOrderPref(CNetApp *pMe, uint64 nNewSetting)
{
  int nReturnStatus;

  if (pMe == NULL) {
    NET_ERR ("Null pointer", 0,0,0);
    return FALSE;
  }

  NetUtil_StatusCb.pfnCancel = NULL;
  NetUtil_StatusCb.pfnNotify = NetUtil_SystemPrefErrCb;
  NetUtil_StatusCb.pNotifyData = pMe;

  /* Send CM command */
  nReturnStatus = ICM_SetSystemPreference(pMe->m_pICM,
                    AEECM_MODE_PREF_NO_CHANGE, AEECM_PREF_TERM_PERMANENT,
                    0, (AEECMAcqOrderPref) nNewSetting,
                    AEECM_BAND_PREF_NO_CHANGE, AEECM_ROAM_PREF_NO_CHANGE,
                    AEECM_HYBR_PREF_NO_CHANGE, AEECM_SRV_DOMAIN_PREF_NO_CHANGE,
                    AEECM_NETWORK_SEL_MODE_PREF_NO_CHANGE, NULL, /* plmn id */
                    &status, &NetUtil_StatusCb /* error cb */
                    );
  if (nReturnStatus == SUCCESS) {
    return TRUE;
  }
  else {
    return FALSE;
  }
}

/*===========================================================================
FUNCTION NetGetRoamPref

DESCRIPTION
  Initialize roam pref

DEPENDENCIES
  Various Get and Set method for the variables in NetUtils.c

SIDE EFFECTS
  None
===========================================================================*/
uint64 NetGetRoamPref(CNetApp *pMe)
{
  AEECMPhInfo   phInfo;
  if (pMe == NULL) {
    NET_ERR ("Null pointer", 0,0,0);
    return (uint64)(AEECM_ROAM_PREF_NONE);
  }

  /* Request the lastest phone and serving system information */
  if(SUCCESS != ICM_GetPhoneInfo(pMe->m_pICM, &phInfo, sizeof(AEECMPhInfo)))
    return (uint64)(AEECM_ROAM_PREF_NONE);
/*lint -save -e571*/
  return (uint64) (phInfo.roam_pref);
/*lint -restore */ 
}

/*===========================================================================
FUNCTION NetSetRoamPref

DESCRIPTION

  Change roam pref setting.

DEPENDENCIES
  None

SIDE EFFECTS
  None
===========================================================================*/
boolean NetSetRoamPref(CNetApp *pMe, uint64 nNewSetting)
{
  int nReturnStatus;

  if (pMe == NULL) {
    NET_ERR ("Null pointer", 0,0,0);
    return FALSE;
  }

  NetUtil_StatusCb.pfnCancel = NULL;
  NetUtil_StatusCb.pfnNotify = NetUtil_SystemPrefErrCb;
  NetUtil_StatusCb.pNotifyData = pMe;

  /* Send CM command */
  nReturnStatus = ICM_SetSystemPreference(pMe->m_pICM,
                    AEECM_MODE_PREF_NO_CHANGE, AEECM_PREF_TERM_PERMANENT,
                    0, AEECM_GW_ACQ_ORDER_PREF_NO_CHANGE,
                    AEECM_BAND_PREF_NO_CHANGE, (AEECMRoamPref ) nNewSetting,
                    AEECM_HYBR_PREF_NO_CHANGE, AEECM_SRV_DOMAIN_PREF_NO_CHANGE,
                    AEECM_NETWORK_SEL_MODE_PREF_NO_CHANGE, NULL, /* plmn id */
                    &status, &NetUtil_StatusCb /* error cb */
                    );
  if (nReturnStatus == SUCCESS) {
    return TRUE;
  }
  else {
    return FALSE;
  }
}

/*===========================================================================
FUNCTION NetGetHybridPref

DESCRIPTION
  Initialize hybrid pref

DEPENDENCIES
  Various Get and Set method for the variables in NetUtils.c

SIDE EFFECTS
  None
===========================================================================*/
uint64 NetGetHybridPref(CNetApp *pMe)
{
  AEECMPhInfo   phInfo;
  if (pMe == NULL) {
    NET_ERR ("Null pointer", 0,0,0);
    return (uint64)(AEECM_HYBR_PREF_NONE);
  }

  /* Request the lastest phone and serving system information */
  if(SUCCESS != ICM_GetPhoneInfo(pMe->m_pICM, &phInfo, sizeof(AEECMPhInfo)))
    return (uint64)(AEECM_HYBR_PREF_NONE);
/*lint -save -e571*/
  return (uint64) (phInfo.hybr_pref);
/*lint -restore */
}

/*===========================================================================
FUNCTION NetSetHybridPref

DESCRIPTION

  Change hybrid pref setting.

DEPENDENCIES
  None

SIDE EFFECTS
  None
===========================================================================*/
boolean NetSetHybridPref(CNetApp *pMe, uint64 nNewSetting)
{
  int nReturnStatus;

  if (pMe == NULL) {
    NET_ERR ("Null pointer", 0,0,0);
    return FALSE;
  }

  NetUtil_StatusCb.pfnCancel = NULL;
  NetUtil_StatusCb.pfnNotify = NetUtil_SystemPrefErrCb;
  NetUtil_StatusCb.pNotifyData = pMe;

  /* Send CM command */
  nReturnStatus = ICM_SetSystemPreference(pMe->m_pICM,
                    AEECM_MODE_PREF_NO_CHANGE, AEECM_PREF_TERM_PERMANENT,
                    0, AEECM_GW_ACQ_ORDER_PREF_NO_CHANGE,
                    AEECM_BAND_PREF_NO_CHANGE, AEECM_ROAM_PREF_NO_CHANGE,
                    (AEECMHybrPref) nNewSetting, AEECM_SRV_DOMAIN_PREF_NO_CHANGE,
                    AEECM_NETWORK_SEL_MODE_PREF_NO_CHANGE, NULL, /* plmn id */
                    &status, &NetUtil_StatusCb /* error cb */
                    );
  if (nReturnStatus == SUCCESS) {
    return TRUE;
  }
  else {
    return FALSE;
  }
}

/*===========================================================================
FUNCTION NetGetNetworkSelect

DESCRIPTION
  Get Network -> Select menu setting.

DEPENDENCIES
  SK UI

SIDE EFFECTS
  None
===========================================================================*/
uint64 NetGetNetworkSelect(CNetApp *pMe)
{
  AEECMPhInfo   phInfo;
  if (pMe == NULL) {
    NET_ERR ("Null pointer", 0,0,0);
    return (uint64)(FALSE);
  }

  /* Request the lastest phone and serving system information */
  if(SUCCESS != ICM_GetPhoneInfo(pMe->m_pICM, &phInfo, sizeof(AEECMPhInfo)))
    return (uint64) (FALSE);
/*lint -save -e571*/
  return (uint64) (phInfo.network_sel_mode_pref);
/*lint -restore*/
}

/*===========================================================================
FUNCTION NetGetAutoNAM

DESCRIPTION
  Save Network -> Auto NAM menu setting in NetApp

DEPENDENCIES
  None

SIDE EFFECTS
  None
===========================================================================*/
uint64 NetGetAutoNAM(CNetApp *pMe)
{
  AEECMPhInfo   phInfo;
  if (pMe == NULL) {
    NET_ERR ("Null pointer", 0,0,0);
    return (uint64)(FALSE);
  }

  /* Request the lastest phone and serving system information */
  if(SUCCESS != ICM_GetPhoneInfo(pMe->m_pICM, &phInfo, sizeof(AEECMPhInfo)))
    return (uint64) FALSE;
  if (phInfo.nam_sel == AEECM_NAM_AUTO)
    return (uint64)TRUE;
  return (uint64)FALSE;
}

/*===========================================================================
FUNCTION NetSetAutoNAM

DESCRIPTION
  Save Network -> Auto NAM menu setting in NetApp

DEPENDENCIES
  None

SIDE EFFECTS
  None
===========================================================================*/
boolean NetSetAutoNAM(CNetApp *pMe, uint64 nVal)
{
  AEECMPhInfo   phInfo;
  if (pMe == NULL) {
    NET_ERR ("Null pointer", 0,0,0);
    return FALSE;
  }

  if (nVal == (uint64)TRUE) {
    ICM_SetNAMSel(pMe->m_pICM, AEECM_NAM_AUTO);
  }
  else {
    /* Request the lastest phone and serving system information */
    if(SUCCESS != ICM_GetPhoneInfo(pMe->m_pICM, &phInfo, sizeof(AEECMPhInfo)))
      return (uint64)FALSE;
    ICM_SetNAMSel(pMe->m_pICM, phInfo.curr_nam);
  }
  return TRUE;
}

/*===========================================================================
FUNCTION NetGetNetworkDataFax

DESCRIPTION
  Get Network -> Data/Fax menu setting from SK UI.

DEPENDENCIES
  SK UI

SIDE EFFECTS
  None
===========================================================================*/
uint64 NetGetNetworkDataFax(CNetApp *pMe)
{
  AEECMPhInfo   phInfo;
  if (pMe == NULL) {
    NET_ERR ("Null pointer", 0,0,0);
    return (uint64)(FALSE);
  }

  /* Request the lastest phone and serving system information */
  if(SUCCESS != ICM_GetPhoneInfo(pMe->m_pICM, &phInfo, sizeof(AEECMPhInfo)))
    return (uint64)FALSE;
/*lint -save -e571*/
  return (uint64) (phInfo.answer_voice);
/*lint -restore*/
}

/*===========================================================================
FUNCTION NetSetNetworkDataFax

DESCRIPTION
  Set Network -> Data/Fax menu setting in 3G UI.

DEPENDENCIES
  None

SIDE EFFECTS
  None
===========================================================================*/
boolean NetSetNetworkDataFax(CNetApp *pMe, uint64 nNewSetting)
{
  uint32 dwDuration = 0;

  if (pMe == NULL) {
    NET_ERR ("Null pointer", 0, 0,0);
    return FALSE;
  }

  switch ((AEECMAnsVoicePref)nNewSetting)
  {
    case AEECM_ANSWER_VOICE_AS_FAX_ONCE:
    case AEECM_ANSWER_VOICE_AS_MODEM_ONCE:
      dwDuration = NETUTIL_VOICE_AS_DATA_TIME;  // timeout after 10 minutes
      break;

    default:
      dwDuration = 0;   // not used, put a dummy value
      break;
  }
  ICM_SetAnswerVoicePref(pMe->m_pICM,
                         (AEECMAnsVoicePref) nNewSetting,
                          dwDuration);

  return TRUE;
}

/*===========================================================================
FUNCTION NetGetNetworkNAMSettings

DESCRIPTION
  Get Network -> Auto / Set NAM menu setting

DEPENDENCIES
  None

SIDE EFFECTS
  None
===========================================================================*/
uint64 NetGetNetworkNAMSettings(CNetApp *pMe)
{
  AEECMPhInfo   phInfo;
  if (pMe == NULL) {
    NET_ERR ("Null pointer", 0,0,0);
    return (uint64)(FALSE);
  }

  /* Request the lastest phone and serving system information */
  if(SUCCESS != ICM_GetPhoneInfo(pMe->m_pICM, &phInfo, sizeof(AEECMPhInfo)))
    return (uint64) FALSE;
/*lint -save -e571*/
  if (phInfo.nam_sel == AEECM_NAM_AUTO) {
    return (uint64) (phInfo.curr_nam);
  }
  else {
    return (uint64) (phInfo.nam_sel);
  }
/*lint -restore*/
}

/*===========================================================================
FUNCTION NetSetNetworkCurrentNAM

DESCRIPTION
  Change Network -> Set NAM menu setting

DEPENDENCIES
  SK UI

SIDE EFFECTS
  None
===========================================================================*/
boolean NetSetNetworkCurrentNAM(CNetApp *pMe, uint64 nNewSetting)
{
  if (pMe == NULL) {
    NET_ERR ("Null pointer", 0,0,0);
    return FALSE;
  }

  ICM_SetNAMSel(pMe->m_pICM,
                   (AEECMNAM) nNewSetting);

  return TRUE;
}

/*===========================================================================
FUNCTION NetSetStaticTextCtlSize

DESCRIPTION
  Set the rect for static control when there are one other control presented
  in the dialog

DEPENDENCIES
  None

SIDE EFFECTS
  None
===========================================================================*/
boolean NetSetStaticTextCtlSize(IStatic *pICurrentStatic, IControl *pIOtherCtrl)
{
  AEERect CtrlRect;
  AEERect StaticRect;

  if ( (pICurrentStatic == NULL) || (pIOtherCtrl == NULL)) { /* error checking */
    return FALSE;
  }

  /* Set static control size */
  ICONTROL_GetRect(pIOtherCtrl, &CtrlRect);
  ISTATIC_GetRect(pICurrentStatic, &StaticRect);

  StaticRect.dy = CtrlRect.y - 1;
  ISTATIC_SetRect(pICurrentStatic, &StaticRect);
  return TRUE;
} /*End NetSetStaticTextCtlSize*/

/*===========================================================================
FUNCTION NetSetGeneralStaticTextStyle

DESCRIPTION
  Set the standard textctl dialog style

DEPENDENCIES
  None

SIDE EFFECTS
  None
===========================================================================*/
boolean NetSetGeneralStaticTextStyle(IStatic *pIStaticText)
{
  ISTATIC_SetProperties(pIStaticText, ST_CENTERTEXT | ST_CENTERTITLE );
  ISTATIC_SetActive(pIStaticText, TRUE);
  return TRUE;
} /*End NetSetGeneralStaticTextStyle */


/*===========================================================================
FUNCTION NetAvoidCurrentSystem

DESCRIPTION
  This function will tell ICM to avoid the current serving system, for 
  the number of seconds specified in avoid_time

DEPENDENCIES
  None

SIDE EFFECTS
  Serving system is avoided
===========================================================================*/
boolean NetAvoidCurrentSystem(CNetApp * pMe, uint32 avoid_time)
{
  int retval;

  if (pMe == NULL) {
    NET_ERR ("Null pMe pointer", 0,0,0);
    return FALSE;
  }
  if (pMe->m_pICM == NULL) {
    NET_ERR ("Null pMe->m_pICM pointer", 0,0,0);
    return FALSE;
  }

  retval = ICM_AvoidServingSystem(pMe->m_pICM, AEECM_SS_AVOID_SYS_SRV, avoid_time);
  if (AEE_SUCCESS != retval) {
    NET_ERR ("Failed to avoid current serving system, %d", retval,0,0);
    return FALSE;
  }
  return TRUE;
} /* End NetAvoidCurrentSystem */

/*===========================================================================
FUNCTION NetEndCurrentDlg

DESCRIPTION
  End the current dialog

DEPENDENCIES
  None

SIDE EFFECTS
  None
===========================================================================*/
void NetEndCurrentDlg(CNetApp *pMe)
{
  int nReturnStatus;
  IDialog * pIActiveDialog;
  uint16 wDlgID;

  pIActiveDialog = ISHELL_GetActiveDialog(pMe->a.m_pIShell);
  if (pIActiveDialog != NULL) {
    wDlgID = IDIALOG_GetID(pIActiveDialog);
    if (wDlgID == 0) { /* error */
      return;
    }

    if (wDlgID == IDD_NETWORK)  // If we're at the base dialog, end the app
    {
      ISHELL_CloseApplet(pMe->a.m_pIShell, FALSE);
    }
    
    nReturnStatus = ISHELL_EndDialog(pMe->a.m_pIShell);
    (void)CheckAEEReturnStatus(nReturnStatus);
  }
} /*End NetEndCurrentDlg */

/*===========================================================================
FUNCTION NetSetNetworkAutoSelection

DESCRIPTION

  Change network select setting.
  Save it and return TRUE if request to CM has been sent,
  return FALSE otherwise.

DEPENDENCIES
  None

SIDE EFFECTS
  None
===========================================================================*/
boolean NetSetNetworkAutoSelection(CNetApp *pMe,
                                   AEECMModePref mode_pref,
                                   AEECMPrefTerm term_pref,
                                   AEECMAcqOrderPref acq_order,
                                   AEECMBandPref band_pref,
                                   AEECMRoamPref roam_pref,
                                   AEECMHybrPref hyb_pref,
                                   AEECMServDomPref srv_domain_pref)
{

  int nReturnStatus;
  
  if (pMe == NULL) {
    NET_ERR ("Null pointer", 0,0,0);
    return FALSE;
  }
  NetUtil_StatusCb.pfnCancel = NULL;
  NetUtil_StatusCb.pfnNotify = NetUtil_SystemPrefErrCb;
  NetUtil_StatusCb.pNotifyData = pMe;

  /* Send CM command */
  nReturnStatus = ICM_SetSystemPreference(pMe->m_pICM,
                    mode_pref, term_pref,
                    0, acq_order,
                    band_pref, roam_pref,
                    hyb_pref, srv_domain_pref,
                    AEECM_NETWORK_SEL_MODE_PREF_AUTOMATIC, NULL, /* plmn id */
                    &status, &NetUtil_StatusCb /* error cb */
                    );
  if (nReturnStatus == SUCCESS) {
    return TRUE;
  }
  else {
    return FALSE;
  }
}


/*===========================================================================
FUNCTION NetDisplayMessageDlg

DESCRIPTION
  Create a message dialog based in resource id of string
  if nDisplaySeconds is zero, dialog will only clear on key input

DEPENDENCIES
  None

SIDE EFFECTS
  None
===========================================================================*/
void NetDisplayMessageDlg(CNetApp *pMe, uint16 nStringID, int nDisplaySeconds)
{
  int nReturnStatus;

  pMe->m_MessageID = nStringID;
  nReturnStatus = ISHELL_CreateDialog(pMe->a.m_pIShell, gpNetResourceFile,
                  IDD_GENERAL_MESSAGE, NULL);

  if(CheckAEEReturnStatus(nReturnStatus)) {

    if (nDisplaySeconds > 0) {
      nReturnStatus = ISHELL_SetTimer(pMe->a.m_pIShell,
                      (int) nDisplaySeconds*1000,
                      (PFNNOTIFY) NetEndMessageDlg,
                      (uint32*) pMe);
      (void)CheckAEEReturnStatus(nReturnStatus);
    }
  }
} /*End NetDisplayMessageDlg*/

/*===========================================================================
FUNCTION NetEndMessageDlg

DESCRIPTION
  End a message dialog

DEPENDENCIES
  None

SIDE EFFECTS
  Send an EVT_MESSAGE_END event
===========================================================================*/
void NetEndMessageDlg(CNetApp *pMe)
{
  int nReturnStatus;
  IDialog * pIActiveDialog;
  uint16 wDlgID;

  pIActiveDialog = ISHELL_GetActiveDialog(pMe->a.m_pIShell);
  if (pIActiveDialog != NULL) {
    wDlgID = IDIALOG_GetID(pIActiveDialog);
    if (wDlgID == 0) { /* error */
      return;
    }
    if (wDlgID == IDD_GENERAL_MESSAGE) {
      nReturnStatus = ISHELL_EndDialog(pMe->a.m_pIShell);
      (void)CheckAEEReturnStatus(nReturnStatus);
      ISHELL_PostEventEx(pMe->a.m_pIShell, EVTFLG_ASYNC, AEECLSID_NETSETTINGS,
        EVT_USER, EVT_MESSAGE_END, wDlgID);
    }
    else {
      NET_ERR("NetEndMessageDlg(): End non Message Dlg",
              0, 0, 0);
    }
  }
} /*End NetEndMessageDlg*/

/*=============================================================================
FUNCTION: NetAppSelectNetwork

DESCRIPTION:  Process select network radio button dialogue input. If the choice
is automatic, handle it in NetApp. Otherwise, post an event to CoreApp instructing
it to handle the manual dialogues. CoreApp has all the info needed to handle them,
while NetApp does not.

PARAMETERS: dwParm is the currently selected dialog. This will be passed to
NetworkSelectSetting so that it can handle the event properly.

RETURN VALUE:
   SUCCESS - if successful

COMMENTS:
   This function assumes that CoreApp is the next running  application. When CoreApp
   resumes it will handle this event and bring up the correct dialogue. If CoreApp
   is not the next running application, the dialogue will launch whenever CoreApp
   resumes.

SIDE EFFECTS:
  None

=============================================================================*/
int NetAppSelectNetwork(CNetApp* pMe, uint32 dwParm)
{
  IDialog  *pIDialog;
  IMenuCtl *pICurrentMenu;
  uint32    nCoreSelMode;
    
  if(dwParm == IDL_NETSELECT_AUTOMATIC)
  {
    pIDialog = ISHELL_GetActiveDialog(pMe->a.m_pIShell);
    if(pIDialog == NULL)
    {
        NET_ERR("pIDialog NULL", 0, 0, 0);
        return EFAILED;
    }
    
    pICurrentMenu = (IMenuCtl*) IDIALOG_GetControl(pIDialog, IDC_MENU_NETWORK_SELECT);
    if(pICurrentMenu == NULL)
    {
        return EFAILED;
    }
    
    if (NetSetNetworkAutoSelection(pMe,
                                   AEECM_MODE_PREF_PERSISTENT,
                                   AEECM_PREF_TERM_PERMANENT,
                                   AEECM_GW_ACQ_ORDER_PREF_PERSISTENT,
                                   AEECM_BAND_PREF_PERSISTENT,
                                   AEECM_ROAM_PREF_PERSISTENT,
                                   AEECM_HYBR_PREF_PERSISTENT,
                                   AEECM_SRV_DOMAIN_PREF_PERSISTENT) == TRUE)
    {
      //Change the menu icon.
      /* Initialize radio button images with IDB_RADIO_UNFILLED */
      InitMenuIcons (pICurrentMenu);

      /* Update the radio button for the currently selected option */
      SetMenuIcon(pICurrentMenu, dwParm, TRUE);
      ISHELL_SetTimer(pMe->a.m_pIShell,
                          NETSELECTIONVERIFICATIONTIME,
                          (PFNNOTIFY) NetEndCurrentDlg,
                          (uint32*) pMe);
      return SUCCESS;
    }
    else
    {
      return EFAILED;
    }
  }
  else
  {
    if (dwParm == IDL_NETSELECT_MANUAL)
    {
      nCoreSelMode = APPCOMFUNC_NW_SEL_MAN_PLMN;
    }
    else if (dwParm ==  IDL_NETSELECT_MANUAL_WRAT)
    {
      nCoreSelMode = APPCOMFUNC_NW_SEL_MAN_PLMN_WRAT;
    }
    else
    { 
      NET_ERR("Invalid NW selection mode, %d.", dwParm, 0, 0);
      return EFAILED;
    }
    AppComFunc_DisplaySelectNetworkDialog(pMe->a.m_pIShell, nCoreSelMode);
    ISHELL_CloseApplet(pMe->a.m_pIShell, FALSE);
  }
  
  return SUCCESS;
}

