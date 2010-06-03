/*===========================================================================

FILE: AUXWLANUtils.c

DESCRIPTION
   This file contains utility functions for AUX app.

PUBLIC CLASSES:
   N/A

INITIALIZATION AND SEQUENCING REQUIREMENTS:


       Copyright © 2007-2008 QUALCOMM Incorporated.
                 All Rights Reserved.
               QUALCOMM Proprietary/GTDR
===========================================================================*/
/*===========================================================================
                        EDIT HISTORY FOR MODULE

when       who     what, where, why
--------   ---     ----------------------------------------------------------
09/11/08   sg      Fix compiler warning
09/08/08   rks    set the bss_type_pref from application
03/26/08   cvs     Remove deprecated functions
06/13/07   cvs     Handle LIMITED_REGIONAL like LIMITED service
03/27/07   ypmw    Fix compiler errors
03/26/07   ypmw    Add EAP authentication scheme for automatic wlan mode
03/21/07   jas     Fix bss_type_pref when setting WLAN network selection mode
02/23/07   jas     Refactoring for HDK builds
01/29/07   ypmw    Fix compiler and lint warnings
01/23/07   ypmw    Move WLAN application from Core App to AUX Settings

===========================================================================*/


/*=============================================================================

                INCLUDES AND VARIABLE DEFINITIONS

=============================================================================== */
#include "AUXUtils.h"
#include "BREWVersion.h"
#include "AEEStdLib.h"
#include "AUXWLANUtils.h"
#include "AUXWLANMenu.h"

#define AUX_MAX_STR_SIZE 50
#define AUX_SRV_STATUS_AVAIL(srv_status)  (((srv_status) == AEECM_SRV_STATUS_SRV) || \
                                       ((srv_status) == AEECM_SRV_STATUS_LIMITED) || \
                                       ((srv_status) == AEECM_SRV_STATUS_LIMITED_REGIONAL))

/* The following structure and table enable easy mappings between
 * AEECMSysBandClass values and AEECMBandPref values.
 */
typedef struct
{
  AEECMSysBandClass  ICMBandClass;
  AEECMBandPref      ICMBandPref;
} AUXICMBandMapTableType;

static AUXICMBandMapTableType AUXICMBandMapTable[] =
{
  { AEECM_SYS_BAND_WLAN_US_2400,      AEECM_BAND_PREF_WLAN_2400_US      },
  { AEECM_SYS_BAND_WLAN_JAPAN_2400,   AEECM_BAND_PREF_WLAN_2400_JP      },
  { AEECM_SYS_BAND_WLAN_EUROPE_2400,  AEECM_BAND_PREF_WLAN_2400_ETSI    },
  { AEECM_SYS_BAND_WLAN_FRANCE_2400,  AEECM_BAND_PREF_WLAN_2400_FR      },
  { AEECM_SYS_BAND_WLAN_SPAIN_2400,   AEECM_BAND_PREF_WLAN_2400_SP      },
  { AEECM_SYS_BAND_WLAN_US_5000,      AEECM_BAND_PREF_WLAN_5000_US      },
  { AEECM_SYS_BAND_WLAN_JAPAN_5000,   AEECM_BAND_PREF_WLAN_5000_JP      },
  { AEECM_SYS_BAND_WLAN_EUROPE_5000,  AEECM_BAND_PREF_WLAN_5000_ETSI    },
  { AEECM_SYS_BAND_WLAN_FRANCE_5000,  AEECM_BAND_PREF_WLAN_5000_FR      },
  { AEECM_SYS_BAND_WLAN_SPAIN_5000,   AEECM_BAND_PREF_WLAN_5000_SP      },
};

/*===========================================================================

                      STATIC and PUBLIC FUNCTIONS

===========================================================================*/

/*===========================================================================
FUNCTION AUXUtil_CancelGetWLAN

DESCRIPTION
  Cancel the call to ICM_GetNetworks and free allocated memory.

DEPENDENCIES
  None

SIDE EFFECTS
  None
===========================================================================*/
void AUXWLANUtil_CancelGetWLAN(AUXApp *pMe)
{
  //Reached here because user pressed End key while "Searching...."
  AUX_MSG("AUXWLANUtil_CancelGetWLAN", 0, 0, 0);
  
  if (pMe == NULL) {
    AUX_ERR ("Null pointer", 0,0,0);
    return;
  }

  if(pMe->m_WLAN.m_nNWSelectionMode != APPCOMFUNC_NW_SEL_INACTIVE) {
    pMe->m_WLAN.m_nNWSelectionMode = APPCOMFUNC_NW_SEL_INACTIVE;
    if (pMe->m_WLAN.m_nCallback.pfnCancel) {
      pMe->m_WLAN.m_nCallback.pfnCancel(&pMe->m_WLAN.m_nCallback);
    }
    FREEIF(pMe->m_WLAN.m_pDetailedBSSList);
  }
  
}

/*===========================================================================
FUNCTION GetWLANSecItem

DESCRIPTION
  Gets the current authentication and encryption settings from IWIFI and
  returns the corresponding item ID on the WLAN Security menu.  This item
  ID should match an entry in the menu action table.  (For compatibility
  with the menu action table, it returns uint64, rather than uint16.)

DEPENDENCIES
  None

SIDE EFFECTS
  None
===========================================================================*/
uint64 GetWLANSecItem(AUXApp *pMe)
{

  int         nErrCode;
  WIFIOpt     TmpOpt;
  uint32      nAuthType;
  uint32      nEncrypType;
  uint16      nItemId;

  if (pMe == NULL) {
    AUX_ERR("Null object pointer", 0, 0, 0);
    return 0;
  }

  /* Get the current WLAN auth and encryption values and determine the
   * appropriate menu setting.
   */
  nErrCode = IWIFI_LoadOptions(pMe->m_WLAN.m_pIWIFI, 0, pMe->m_WLAN.m_pIWIFIOpts);
  if (nErrCode != SUCCESS) {
    AUX_ERR("Failed loading WIFI options, %d", nErrCode, 0, 0);
    return FALSE;
  }

  nErrCode = IWIFIOpts_GetOpt(pMe->m_WLAN.m_pIWIFIOpts,
                              AEEWIFI_OPT_AUTHENTICATION_TYPE, &TmpOpt);
  if (nErrCode != SUCCESS) {
    AUX_ERR("Unable to load WIFI auth type: %d", nErrCode, 0, 0);
    return FALSE;
  }
  nAuthType = (uint32)TmpOpt.pVal;

  nErrCode = IWIFIOpts_GetOpt(pMe->m_WLAN.m_pIWIFIOpts,
                              AEEWIFI_OPT_ENCRYPTION_TYPE, &TmpOpt);
  if (nErrCode != SUCCESS) {
    AUX_ERR("Unable to load WIFI encrypt type: %d", nErrCode, 0, 0);
    return FALSE;
  }
  nEncrypType = (uint32)TmpOpt.pVal;

  switch (nAuthType)
  {
    case AEEWIFI_AUTHENTICATION_NONE:
      if (nEncrypType == AEEWIFI_ENCRYPTION_NONE) {
        nItemId = IDL_SECURITY_NONE;
      }
      else
      {
        AUX_ERR("Invalid encryption type with no authentication, %d",nEncrypType, 0, 0);
        return 0;
      }
      break;
    case AEEWIFI_AUTHENTICATION_WEP:
      if (nEncrypType == AEEWIFI_ENCRYPTION_WEP_64bit) {
        nItemId = IDL_SECURITY_WEP64;
      }
      else if (nEncrypType == AEEWIFI_ENCRYPTION_WEP_128bit) {
        nItemId = IDL_SECURITY_WEP128;
      }
      else
      {
        AUX_ERR("Invalid WEP encryption type, %d", nEncrypType, 0, 0);
        return 0;
      }
      break;
    case AEEWIFI_AUTHENTICATION_8021X_PASSWORD:
      nItemId = IDL_SECURITY_WPA_PASSWORD;
      break;
    case AEEWIFI_AUTHENTICATION_8021X_TLS:
      nItemId = IDL_SECURITY_WPA_TLS_CERT;
      break;
    case AEEWIFI_AUTHENTICATION_8021X_MD5:
      nItemId = IDL_SECURITY_WPA_MD5;
      break;
    case AEEWIFI_AUTHENTICATION_PAP:
    case AEEWIFI_AUTHENTICATION_CHAP: 
    case AEEWIFI_AUTHENTICATION_MSCHAPV2:              
    case AEEWIFI_AUTHENTICATION_PEAP:
    case AEEWIFI_AUTHENTICATION_TTLS:
      nItemId = IDL_SECURITY_WPA_EAP;
      break;

    default:
      AUX_ERR("Unsupported WLAN auth type, %d", nAuthType, 0, 0);
      return 0;
  }

  return (uint64)nItemId;
}

/*===========================================================================
FUNCTION GetDhcp

DESCRIPTION
  Get DHCP value

DEPENDENCIES
  None

SIDE EFFECTS
  None
===========================================================================*/
uint64 GetDhcp (AUXApp *pMe)
{

  int     nWIFIErr;
  WIFIOpt tmp_Opt;

  if (pMe == NULL) {
    AUX_ERR ("Null pMe pointer", 0, 0, 0);
    return (uint64)0;
  }
  else if ((pMe->m_WLAN.m_pIWIFI == NULL) || (pMe->m_WLAN.m_pIWIFIOpts == NULL)) {
    AUX_ERR ("Null pointer, %x, %x", pMe->m_WLAN.m_pIWIFI, pMe->m_WLAN.m_pIWIFIOpts, 0);
    return (uint64)0;
  }

  nWIFIErr = IWIFI_LoadOptions(pMe->m_WLAN.m_pIWIFI, 0, pMe->m_WLAN.m_pIWIFIOpts);
  if (SUCCESS == nWIFIErr) {
    nWIFIErr = IWIFIOpts_GetOpt(pMe->m_WLAN.m_pIWIFIOpts,
                                AEEWIFI_OPT_DHCP_ENABLED, &tmp_Opt);
    if (SUCCESS == nWIFIErr) {
      pMe->m_WLAN.m_bDhcpOn = (boolean)(uint32)tmp_Opt.pVal;
    }
    else
    {
      AUX_ERR("Could not get AEEWIFI_OPT_DHCP_ENABLED, %d.",
                nWIFIErr, 0, 0);
    }
  }
  else
  {
    AUX_ERR("Could not load options, %d", nWIFIErr, 0, 0);
  }

  return (uint64)pMe->m_WLAN.m_bDhcpOn;
}

/*===========================================================================
FUNCTION SetDhcp

DESCRIPTION
  assign DCHP value

DEPENDENCIES
  CreateKeyGuardOnDlg

SIDE EFFECTS
  None
===========================================================================*/
boolean SetDhcp(AUXApp *pMe, uint64 nNewVal)
{

  WIFIOpt value_to_set;
  int     nWIFIErr;
  uint32  nNewVal32 = (uint32)nNewVal;

  if (pMe == NULL) {
    AUX_ERR ("Null pMe pointer", 0, 0, 0);
    return 0;
  }
  else if ((pMe->m_WLAN.m_pIWIFI == NULL) || (pMe->m_WLAN.m_pIWIFIOpts == NULL)) {
    AUX_ERR ("Null pointer, %x, %x", pMe->m_WLAN.m_pIWIFI, pMe->m_WLAN.m_pIWIFIOpts, 0);
    return 0;
  }

  pMe->m_WLAN.m_bDhcpOn = (boolean)nNewVal32;
  value_to_set.nId = AEEWIFI_OPT_DHCP_ENABLED;
  value_to_set.pVal = (void *)nNewVal32;

  nWIFIErr = IWIFI_LoadOptions(pMe->m_WLAN.m_pIWIFI, 0, pMe->m_WLAN.m_pIWIFIOpts);
  if (SUCCESS == nWIFIErr) {
    nWIFIErr = IWIFIOpts_AddOpt(pMe->m_WLAN.m_pIWIFIOpts, &value_to_set);
    if (SUCCESS == nWIFIErr) {
      nWIFIErr = IWIFI_CommitOptions(pMe->m_WLAN.m_pIWIFI, 0, pMe->m_WLAN.m_pIWIFIOpts);
      if (SUCCESS == nWIFIErr) {

        nWIFIErr = ICM_SetSubscriptionStatus(pMe->m_WLAN.m_pICM,
                                             AEECM_SYS_MODE_WLAN,
                                             TRUE);
        if (AEE_SUCCESS == nWIFIErr)
        {
          return TRUE;
        }
        else
        {
          AUX_ERR("Failed to change WLAN subs, %d", nWIFIErr, 0, 0);
        }
        
      }
      else
      {
        AUX_ERR("Could not commit AEEWIFI_OPT_DHCP_ENABLED, %d",
                 nWIFIErr, 0, 0);
      }
    }
    else
    {
      AUX_ERR("Could not set AEEWIFI_OPT_DHCP_ENABLED, %d",
               nWIFIErr, 0, 0);
    }
  }
  else
  {
    AUX_ERR("Could not load options, %d", nWIFIErr, 0, 0);
  }

  return FALSE;

}


/*===========================================================================
FUNCTION GetWLANMode

DESCRIPTION
  Gets the Global WLAN mode preference

DEPENDENCIES
  None

SIDE EFFECTS
  None
===========================================================================*/
uint64 GetWLANMode(AUXApp * pMe)
{
  AEECMPhInfo phInfo;
  int nReturnStatus;

  if (pMe == NULL)
  {
    AUX_ERR ("Null pMe pointer", 0, 0, 0);
    return (uint64)0;
  }

  nReturnStatus = ICM_GetPhoneInfo(pMe->m_WLAN.m_pICM, &phInfo, sizeof(AEECMPhInfo)); 
  if (nReturnStatus != AEE_SUCCESS)
  {
    AUX_ERR("Failed getting phone info from ICM", 0, 0, 0);
    return (uint64)0;
  }

  return (uint64)phInfo.m_wlan_pref.bss_type_pref;
} /* GetWLANMode */

/*===========================================================================
FUNCTION SetWLANMode

DESCRIPTION
  Sets the Global WLAN mode preference

DEPENDENCIES
  None

SIDE EFFECTS
  None
===========================================================================*/
boolean SetWLANMode(AUXApp * pMe, uint64 nNewVal)
{
  AEECMPhError  error;
  AEECMWLANPref sICMWLANData;
  AEECMPhInfo phInfo;
  AEECMSSInfo ssInfo;
  int nReturnStatus;
  int nErrCode;
  uint32  nNewVal32 = (uint32)nNewVal;


  if (pMe == NULL)
  {
    AUX_ERR ("Null pMe pointer", 0, 0, 0);
    return FALSE;
  }

  sICMWLANData.network_sel_mode_pref = AEECM_NETWORK_SEL_MODE_PREF_NO_CHANGE;

  /* For the remaining fields get current info from phInfo member in pMe.
  *  Calling ICM_SetSystemPreference_WLAN below has the side effect of
  *  causing a disconnect and reassociation, so we want to have the
  *  correct information for a reassociation.
  *  If we are in Manual mode, the info in phInfo is current/accurate.
  *  If we are in Automatic mode, we don't care as the preferences
  *  are not used for the reassociation.  Either way, we are not
  *  changing the preferences and will reassociate correctly.
  */

  sICMWLANData.scan_mode = AEECM_WLAN_SCAN_PREF_NO_CHANGE;
  
  nReturnStatus = ICM_GetPhoneInfo(pMe->m_WLAN.m_pICM, &phInfo, sizeof(AEECMPhInfo)); 
  if (nReturnStatus != AEE_SUCCESS)
  {
    AUX_ERR("Failed getting phone info from ICM", 0, 0, 0);
    return FALSE;
  }
  sICMWLANData.encrypt = phInfo.m_wlan_pref.encrypt;
  sICMWLANData.auth = phInfo.m_wlan_pref.auth;
  sICMWLANData.mob_ip_profile_id = phInfo.m_wlan_pref.mob_ip_profile_id;

  /* Now set up the authentication info */
  nErrCode = AUXApp_FillWLANAuthInfo(pMe, &sICMWLANData);
  if (SUCCESS != nErrCode)
  {
    return FALSE;
  }

  sICMWLANData.bss_type_pref = (AEECMWLAN_BSS_TYPE_PREF)nNewVal32;

  nReturnStatus = ICM_GetSSInfo(pMe->m_WLAN.m_pICM, &ssInfo, sizeof(AEECMSSInfo));
  if (AEE_SUCCESS != nReturnStatus)
  {
    AUX_ERR("Failed getting serving system info from ICM", 0, 0, 0);
    return FALSE;
  }

  if (!AUX_SRV_STATUS_AVAIL(ssInfo.wlan_srv_status))
  {
    sICMWLANData.tech_pref = AEECM_WLAN_TECH_PREF_NO_CHANGE;

    /* Populate fields with info from current BSS info */

    if ( phInfo.m_wlan_pref.tech_pref == AEECM_WLAN_TECH_PREF_A )
    {
       sICMWLANData.chan = AEECM_WLAN_5000_CHAN_ANY;
    }
    else
    {
       sICMWLANData.chan = AEECM_WLAN_2400_CHAN_ANY;
    }

    /* One final hack:  If SSID is empty and BSS ID is zero, CM won't
     * accept the request.  The implication is that you can't switch to
     * manual mode if you're not already associated with a system.
     * So, if that condition is met, we'll just put a bogus value in SSID
     * to get CM to accept the switch to the new network selection mode.
     */
    sICMWLANData.bss_id = 0;
    sICMWLANData.ssid[0] = (AECHAR)255;
    sICMWLANData.ssid[1] = (AECHAR)'\0';
    sICMWLANData.bss_type = AEECM_WLAN_BSS_TYPE_ANY;
    sICMWLANData.band = AEECM_BAND_PREF_WLAN_2400_US;
  }
  else
  {
    /* Populate fields with info from current BSS info */
    sICMWLANData.chan = ssInfo.wlan_bss_info.chan;
    sICMWLANData.bss_id = ssInfo.wlan_bss_info.bss_id;

    (void)WSTRLCPY(sICMWLANData.ssid, ssInfo.wlan_bss_info.ssid,
                   AEECM_MAX_SSID_STR_SIZE);
    nErrCode = Convert_ICMTechWLAN_to_ICMWLANTechPref(ssInfo.wlan_bss_info.tech,
                                                        &sICMWLANData.tech_pref);
    if (SUCCESS != nErrCode)
    {
      AUX_ERR("Failed to convert varible from AEECMTechWLAN to AEECMWLANTechPref, %d",
               nErrCode, 0, 0);
      return FALSE;
    }

    /* One final hack:  If SSID is empty and BSS ID is zero, CM won't
    * accept the request.  The implication is that you can't switch to
    * manual mode if you're not already associated with a system.
    * So, if that condition is met, we'll just put a bogus value in SSID
    * to get CM to accept the switch to the new network selection mode.
    */
    if ((WSTRLEN(sICMWLANData.ssid) == 0) && (sICMWLANData.bss_id == 0))
    {
      sICMWLANData.ssid[0] = (AECHAR)255;
      sICMWLANData.ssid[1] = (AECHAR)'\0';
    }

    sICMWLANData.bss_type = phInfo.m_wlan_pref.bss_type;
    sICMWLANData.band = AUXWLANBandClassToPref(ssInfo.wlan_bss_info.band);
  } /* else AUX_SRV_STATUS_AVAIL */

  nReturnStatus = ICM_SetSystemPreference_WLAN(pMe->m_WLAN.m_pICM,
                                               AEECM_MODE_PREF_CURRENT_PLUS_WLAN,
                                               AEECM_PREF_TERM_PERMANENT, 0,
                                               AEECM_GW_ACQ_ORDER_PREF_NO_CHANGE,
                                               AEECM_BAND_PREF_NO_CHANGE,
                                               AEECM_ROAM_PREF_NO_CHANGE,
                                               AEECM_HYBR_PREF_NO_CHANGE,
                                               AEECM_SRV_DOMAIN_PREF_NO_CHANGE,
                                               AEECM_NETWORK_SEL_MODE_PREF_NO_CHANGE,
                                               NULL, &sICMWLANData, &error, NULL);
  if (nReturnStatus != AEE_SUCCESS)
  {
    AUX_ERR("ICM_SetSystemPreference_WLAN failed %d", nReturnStatus,0,0);
    return FALSE;
  }
  return TRUE;
} /* Set WLAN Mode */


/*===========================================================================
FUNCTION GetWLANModeProfile

DESCRIPTION
  Gets the WLAN mode for the currently selected profile

DEPENDENCIES
  None

SIDE EFFECTS
  None
===========================================================================*/
uint64 GetWLANModeProfile(AUXApp * pMe)
{
  WIFIOpt tmp_Opt;
  int   nWIFIErr;

  if (pMe == NULL) {
    AUX_ERR ("Null pMe pointer", 0, 0, 0);
    return (uint64)0;
  }
  else if ((pMe->m_WLAN.m_pIWIFI == NULL) || (pMe->m_WLAN.m_pIWIFIOpts == NULL)) {
    AUX_ERR ("Null pointer, %x, %x", pMe->m_WLAN.m_pIWIFI, pMe->m_WLAN.m_pIWIFIOpts, 0);
    return (uint64)0;
  }

  nWIFIErr = IWIFI_LoadOptions(pMe->m_WLAN.m_pIWIFI, 0, pMe->m_WLAN.m_pIWIFIOpts);
  if (SUCCESS == nWIFIErr) {
    nWIFIErr = IWIFIOpts_GetOpt(pMe->m_WLAN.m_pIWIFIOpts,
                                AEEWIFI_OPT_WLAN_MODE, &tmp_Opt);
    if (SUCCESS == nWIFIErr) {
      pMe->m_WLAN.m_WLANMode = (AEEWIFIModeOption)(uint32)tmp_Opt.pVal;
    }
    else
    {
      AUX_ERR("Could not get AEEWIFI_OPT_WLAN_MODE, %d",
               nWIFIErr, 0, 0);
    }
  }
  else
  {
    AUX_ERR("Could not load options, %d", nWIFIErr, 0, 0);
  }

  return (uint64)pMe->m_WLAN.m_WLANMode;
} /* GetWLANModeProfile */

/*===========================================================================
FUNCTION SetWLANModeProfile

DESCRIPTION
  Sets the WLAN mode for the currently selected profile

DEPENDENCIES
  None

SIDE EFFECTS
  None
===========================================================================*/
boolean SetWLANModeProfile(AUXApp * pMe, uint64 nNewVal)
{
  WIFIOpt value_to_set;
  int   nWIFIErr;
  uint32  nNewVal32 = (uint32)nNewVal;

  if (pMe == NULL) {
    AUX_ERR ("Null pMe pointer", 0, 0, 0);
    return 0;
  }
  else if ((pMe->m_WLAN.m_pIWIFI == NULL) || (pMe->m_WLAN.m_pIWIFIOpts == NULL)) {
    AUX_ERR ("Null pointer, %x, %x", pMe->m_WLAN.m_pIWIFI, pMe->m_WLAN.m_pIWIFIOpts, 0);
    return 0;
  }

  pMe->m_WLAN.m_WLANMode = (AEEWIFIModeOption)nNewVal32;
  value_to_set.nId = AEEWIFI_OPT_WLAN_MODE;
  value_to_set.pVal = (void *)nNewVal32;

  nWIFIErr = IWIFI_LoadOptions(pMe->m_WLAN.m_pIWIFI, 0, pMe->m_WLAN.m_pIWIFIOpts);
  if (SUCCESS == nWIFIErr) {
    nWIFIErr = IWIFIOpts_AddOpt(pMe->m_WLAN.m_pIWIFIOpts, &value_to_set);
    if (SUCCESS == nWIFIErr) {
      nWIFIErr = IWIFI_CommitOptions(pMe->m_WLAN.m_pIWIFI, 0, pMe->m_WLAN.m_pIWIFIOpts);
      if (SUCCESS == nWIFIErr) {
        nWIFIErr = ICM_SetSubscriptionStatus(pMe->m_WLAN.m_pICM,
                                             AEECM_SYS_MODE_WLAN,
                                             TRUE);
        if (AEE_SUCCESS == nWIFIErr)
        {
          return TRUE;
        }
        else
        {
          AUX_ERR("Failed to change WLAN subs, %d", nWIFIErr, 0, 0);
        }
      }
      else
      {
        AUX_ERR("Could not commit AEEWIFI_OPT_WLAN_MODE, %d",
                 nWIFIErr, 0, 0);
      }
    }
    else
    {
      AUX_ERR("Could not set AEEWIFI_OPT_WLAN_MODE, %d",
               nWIFIErr, 0, 0);
    }
  }
  else
  {
    AUX_ERR("Could not load options, %d", nWIFIErr, 0, 0);
  }
  return FALSE;
} /* Set WLAN Mode profile*/

/*===========================================================================
FUNCTION GetWLANScanMode

DESCRIPTION
  Gets the WLAN scan mode from pMe

DEPENDENCIES
  None

SIDE EFFECTS
  None
===========================================================================*/
uint64 GetWLANScanMode(AUXApp * pMe)
{
  AEECMPhInfo phInfo;
  int nReturnStatus;

  if (pMe == NULL)
  {
    AUX_ERR ("Null pMe pointer", 0, 0, 0);
    return (uint64)AEECM_WLAN_SCAN_PREF_PASSIVE;
  }
  
  nReturnStatus = ICM_GetPhoneInfo(pMe->m_WLAN.m_pICM, &phInfo, sizeof(AEECMPhInfo)); 
  if (nReturnStatus != AEE_SUCCESS)
  {
    AUX_ERR("Failed getting phone info from ICM", 0, 0, 0);
    return (uint64)AEECM_WLAN_SCAN_PREF_PASSIVE;
  }
  
  return (uint64)(phInfo.m_wlan_pref.scan_mode);
} /* GetWLANScanMode */

/*===========================================================================
FUNCTION SetWLANScanMode

DESCRIPTION
  Sets the WLAN scan mode to the value in nNewVal

DEPENDENCIES
  None

SIDE EFFECTS
  None
===========================================================================*/
boolean SetWLANScanMode(AUXApp * pMe, uint64 nNewVal)
{
  int nReturnStatus;
  AEECMWLANPref sICMWLANData;
  AEECMPhError error;
  AEECMSSInfo ssInfo;
  AEECMPhInfo phInfo;
  int nErrCode;

  if (pMe == NULL)
  {
    AUX_ERR ("Null pMe pointer", 0, 0, 0);
    return FALSE;
  }

  if (pMe->m_WLAN.m_pICM == NULL)
  {
    AUX_ERR ("Null pMe->m_WLAN.m_pICM pointer", 0, 0, 0);
    return FALSE;
  }
  /* Set the network selection mode to no change */
  sICMWLANData.network_sel_mode_pref = AEECM_NETWORK_SEL_MODE_PREF_NO_CHANGE;

  nReturnStatus = ICM_GetSSInfo(pMe->m_WLAN.m_pICM, &ssInfo, sizeof(AEECMSSInfo));
  if (AEE_SUCCESS != nReturnStatus)
  {
    AUX_ERR("Failed getting serving system info from ICM", 0, 0, 0);
    return FALSE;
  }

  /* Populate fields with info from current BSS info */
  sICMWLANData.chan = ssInfo.wlan_bss_info.chan;
  sICMWLANData.bss_id = ssInfo.wlan_bss_info.bss_id;
  sICMWLANData.bss_type = ssInfo.wlan_bss_info.bss_type;
  sICMWLANData.bss_type_pref = (AEECMWLAN_BSS_TYPE_PREF)GetWLANMode(pMe);
  (void)WSTRLCPY(sICMWLANData.ssid, ssInfo.wlan_bss_info.ssid,
                 AEECM_MAX_SSID_STR_SIZE);
  nErrCode = Convert_ICMTechWLAN_to_ICMWLANTechPref(ssInfo.wlan_bss_info.tech,
                                                        &sICMWLANData.tech_pref);
  if (SUCCESS != nErrCode)
  {
    AUX_ERR("Failed to convert varible from AEECMTechWLAN to AEECMWLANTechPref, %d",
             nErrCode, 0, 0);
    return FALSE;
  }

  if (!AUX_SRV_STATUS_AVAIL(ssInfo.wlan_srv_status))
  {
    sICMWLANData.band = AEECM_BAND_PREF_WLAN_2400_US;
  }
  else
  {
    sICMWLANData.band = AUXWLANBandClassToPref(ssInfo.wlan_bss_info.band);
  }

  /* For the remaining fields get current info from phInfo member in pMe.
  *  Calling ICM_SetSystemPreference_WLAN below has the side effect of
  *  causing a disconnect and reassociation, so we want to have the
  *  correct information for a reassociation.
  *  If we are in Manual mode, the info in phInfo is current/accurate.
  *  If we are in Automatic mode, we don't care as the preferences
  *  are not used for the reassociation.  Either way, we are not
  *  changing the preferences and will reassociate correctly.
  */

  nReturnStatus = ICM_GetPhoneInfo(pMe->m_WLAN.m_pICM, &phInfo, sizeof(AEECMPhInfo)); 
  if (nReturnStatus != AEE_SUCCESS)
  {
    AUX_ERR("Failed getting phone info from ICM", 0, 0, 0);
    return FALSE;
  }

  sICMWLANData.encrypt = phInfo.m_wlan_pref.encrypt;
  sICMWLANData.auth = phInfo.m_wlan_pref.auth;
  sICMWLANData.mob_ip_profile_id = phInfo.m_wlan_pref.mob_ip_profile_id;

  /* Now set up the authentication info */
  nErrCode = AUXApp_FillWLANAuthInfo(pMe, &sICMWLANData);
  if (SUCCESS != nErrCode)
  {
    return FALSE;
  }

  /* set scan mode field to new value */
  sICMWLANData.scan_mode = (AEECMWLANScanPref)nNewVal;

  nReturnStatus = 
    ICM_SetSystemPreference_WLAN(pMe->m_WLAN.m_pICM,
                                 AEECM_MODE_PREF_CURRENT_PLUS_WLAN,
                                 AEECM_PREF_TERM_PERMANENT, 0,
                                 AEECM_GW_ACQ_ORDER_PREF_NO_CHANGE,
                                 AEECM_BAND_PREF_NO_CHANGE,
                                 AEECM_ROAM_PREF_NO_CHANGE,
                                 AEECM_HYBR_PREF_NO_CHANGE,
                                 AEECM_SRV_DOMAIN_PREF_NO_CHANGE,
                                 AEECM_NETWORK_SEL_MODE_PREF_NO_CHANGE,
                                 NULL, &sICMWLANData, &error, NULL);

  if (nReturnStatus != AEE_SUCCESS)
  {
    AUX_ERR("ICM_SetSystemPreference_WLAN failed %d", nReturnStatus,0,0);
    return FALSE;
  }

  return TRUE;
} /* SetWLANScanMode */


/*===========================================================================
FUNCTION GetWLANEnc

DESCRIPTION
  Gets the WLAN encryption type from the WIFI options

DEPENDENCIES
  None

SIDE EFFECTS
  None
===========================================================================*/
uint64 GetWLANEnc(AUXApp * pMe)
{
  WIFIOpt tmp_Opt;
  int   nWIFIErr;

  if (pMe == NULL) {
    AUX_ERR ("Null pMe pointer", 0, 0, 0);
    return (uint64)0;
  }
  else if ((pMe->m_WLAN.m_pIWIFI == NULL) || (pMe->m_WLAN.m_pIWIFIOpts == NULL)) {
    AUX_ERR ("Null pointer, %x, %x", pMe->m_WLAN.m_pIWIFI, pMe->m_WLAN.m_pIWIFIOpts, 0);
    return (uint64)0;
  }

  nWIFIErr = IWIFI_LoadOptions(pMe->m_WLAN.m_pIWIFI, 0, pMe->m_WLAN.m_pIWIFIOpts);
  if (SUCCESS == nWIFIErr) {
    nWIFIErr = IWIFIOpts_GetOpt(pMe->m_WLAN.m_pIWIFIOpts,
                                AEEWIFI_OPT_ENCRYPTION_TYPE, &tmp_Opt);
    if (SUCCESS == nWIFIErr) {
      pMe->m_WLAN.m_WLANEnc = (AEEWIFIEncryptionOption)(uint32)tmp_Opt.pVal;
    }
    else
    {
      AUX_ERR("Could not get AEEWIFI_OPT_ENCRYPTION_TYPE, %d",
               nWIFIErr, 0, 0);
    }
  }
  else
  {
    AUX_ERR("Could not load options, %d", nWIFIErr, 0, 0);
  }
  return (uint64)pMe->m_WLAN.m_WLANEnc;
} /* GetWLANEnc */

/*===========================================================================
FUNCTION GetWLANAuth

DESCRIPTION
  Gets the WLAN authorization type from the WIFI options

DEPENDENCIES
  None

SIDE EFFECTS
  None
===========================================================================*/
uint64 GetWLANAuth(AUXApp * pMe)
{
  WIFIOpt tmp_Opt;
  int   nWIFIErr;

  if (pMe == NULL) {
    AUX_ERR ("Null pMe pointer", 0, 0, 0);
    return (uint64)0;
  }
  else if ((pMe->m_WLAN.m_pIWIFI == NULL) || (pMe->m_WLAN.m_pIWIFIOpts == NULL)) {
    AUX_ERR ("Null pointer, %x, %x", pMe->m_WLAN.m_pIWIFI, pMe->m_WLAN.m_pIWIFIOpts, 0);
    return (uint64)0;
  }

  nWIFIErr = IWIFI_LoadOptions(pMe->m_WLAN.m_pIWIFI, 0, pMe->m_WLAN.m_pIWIFIOpts);
  if (SUCCESS == nWIFIErr) {
    nWIFIErr = IWIFIOpts_GetOpt(pMe->m_WLAN.m_pIWIFIOpts,
                                AEEWIFI_OPT_AUTHENTICATION_TYPE, &tmp_Opt);
    if (SUCCESS == nWIFIErr) {
      pMe->m_WLAN.m_WLANAuth = (AEEWIFIAuthenticationOption)(uint32)tmp_Opt.pVal;
    }
    else
    {
      AUX_ERR("Could not get AEEWIFI_OPT_AUTHENTICATION_TYPE, %d",
               nWIFIErr, 0, 0);
    }
  }
  else
  {
    AUX_ERR("Could not load options, %d", nWIFIErr, 0, 0);
  }
  return (uint64)pMe->m_WLAN.m_WLANAuth;
} /* GetWLANAuth */

/*===========================================================================
FUNCTION GetWLANPreamble

DESCRIPTION
  Gets the WLAN preamble length from the WIFI options

DEPENDENCIES
  None

SIDE EFFECTS
  None
===========================================================================*/
uint64 GetWLANPreamble(AUXApp * pMe)
{
  WIFIOpt tmp_Opt;
  int   nWIFIErr;

  if (pMe == NULL) {
    AUX_ERR ("Null pMe pointer", 0, 0, 0);
    return (uint64)0;
  }
  else if ((pMe->m_WLAN.m_pIWIFI == NULL) || (pMe->m_WLAN.m_pIWIFIOpts == NULL)) {
    AUX_ERR ("Null pointer, %x, %x", pMe->m_WLAN.m_pIWIFI, pMe->m_WLAN.m_pIWIFIOpts, 0);
    return (uint64)0;
  }

  nWIFIErr = IWIFI_LoadOptions(pMe->m_WLAN.m_pIWIFI, 0, pMe->m_WLAN.m_pIWIFIOpts);
  if (SUCCESS == nWIFIErr) {
    nWIFIErr = IWIFIOpts_GetOpt(pMe->m_WLAN.m_pIWIFIOpts,
                                AEEWIFI_OPT_PREAMBLE_TYPE, &tmp_Opt);
    if (SUCCESS == nWIFIErr) {
      pMe->m_WLAN.m_WLANPreamble = (AEEWIFIPreambleOption)(uint32)tmp_Opt.pVal;
    }
    else
    {
      AUX_ERR("Could not get AEEWIFI_OPT_PREAMBLE_TYPE, %d",
               nWIFIErr, 0, 0);
    }
  }
  else
  {
    AUX_ERR("Could not load options, %d", nWIFIErr, 0, 0);
  }

  return (uint64)pMe->m_WLAN.m_WLANPreamble;
} /* GetWLANPreamble */

/*===========================================================================
FUNCTION SetWLANPreamble

DESCRIPTION
  Sets the WLAN preamble length in the WIFI optinos

DEPENDENCIES
  None

SIDE EFFECTS
  None
===========================================================================*/
boolean SetWLANPreamble(AUXApp * pMe, uint64 nNewVal)
{
  WIFIOpt value_to_set;
  int   nWIFIErr;
  uint32  nNewVal32 = (uint32)nNewVal;

  if (pMe == NULL) {
    AUX_ERR ("Null pMe pointer", 0, 0, 0);
    return 0;
  }
  else if ((pMe->m_WLAN.m_pIWIFI == NULL) || (pMe->m_WLAN.m_pIWIFIOpts == NULL)) {
    AUX_ERR ("Null pointer, %x, %x", pMe->m_WLAN.m_pIWIFI, pMe->m_WLAN.m_pIWIFIOpts, 0);
    return 0;
  }

  pMe->m_WLAN.m_WLANPreamble = (AEEWIFIPreambleOption)nNewVal32;
  value_to_set.nId = AEEWIFI_OPT_PREAMBLE_TYPE;
  value_to_set.pVal = (void *)nNewVal32;

  nWIFIErr = IWIFI_LoadOptions(pMe->m_WLAN.m_pIWIFI, 0, pMe->m_WLAN.m_pIWIFIOpts);
  if (SUCCESS == nWIFIErr) {
    nWIFIErr = IWIFIOpts_AddOpt(pMe->m_WLAN.m_pIWIFIOpts, &value_to_set);
    if (SUCCESS == nWIFIErr) {
      nWIFIErr = IWIFI_CommitOptions(pMe->m_WLAN.m_pIWIFI, 0, pMe->m_WLAN.m_pIWIFIOpts);
      if (SUCCESS == nWIFIErr) {
        nWIFIErr = ICM_SetSubscriptionStatus(pMe->m_WLAN.m_pICM,
                                             AEECM_SYS_MODE_WLAN,
                                             TRUE);
        if (AEE_SUCCESS == nWIFIErr)
        {
          return TRUE;
        }
        else
        {
          AUX_ERR("Failed to change WLAN subs, %d", nWIFIErr, 0, 0);
        }
      }
      else
      {
        AUX_ERR("Could not commit AEEWIFI_OPT_PREAMBLE_TYPE, %d",
                 nWIFIErr, 0, 0);
      }
    }
    else
    {
      AUX_ERR("Could not set AEEWIFI_OPT_PREAMBLE_TYPE, %d",
               nWIFIErr, 0, 0);
    }
  }
  else
  {
    AUX_ERR("Could not load options, %d", nWIFIErr, 0, 0);
  }
  return FALSE;
} /* SetWLANPreamble */

/*===========================================================================
FUNCTION GetWLANNwSelMode

DESCRIPTION
  Gets the WLAN network selection mode from ICM data

DEPENDENCIES
  None

SIDE EFFECTS
  None
===========================================================================*/
uint64 GetWLANNwSelMode(AUXApp * pMe)
{
  AEECMNwSelModePref  nNwSelMode;
  AEECMPhInfo phInfo;
  int nReturnStatus;

  if (pMe == NULL) {
    AUX_ERR ("Null pMe pointer", 0, 0, 0);
    return (uint64)AEECM_NETWORK_SEL_MODE_PREF_MANUAL;
  }

  nReturnStatus = ICM_GetPhoneInfo(pMe->m_WLAN.m_pICM, &phInfo, sizeof(AEECMPhInfo)); 
  if (nReturnStatus != AEE_SUCCESS)
  {
    AUX_ERR("Failed getting phone info from ICM", 0, 0, 0);
    return (uint64)AEECM_NETWORK_SEL_MODE_PREF_MANUAL;
  }
  
  nNwSelMode = phInfo.m_wlan_pref.network_sel_mode_pref;
  return (uint64)nNwSelMode;
}

/*===========================================================================
FUNCTION SetWLANNwSelMode

DESCRIPTION
  Sets the WLAN network selection mode in CM

DEPENDENCIES
  None

SIDE EFFECTS
  None
===========================================================================*/
boolean SetWLANNwSelMode(AUXApp * pMe, uint64 nNewVal)
{
  AEECMNwSelModePref nNwSelMode = (AEECMNwSelModePref)nNewVal;
  int                nErrCode;
  int                nReturnStatus;
  AEECMWLANPref      sICMWLANData;
  AEECMPhError       error;
  AEECMSSInfo        ssInfo;

  if (pMe == NULL) {
    AUX_ERR ("Null pMe pointer", 0, 0, 0);
    return FALSE;
  }
  else if ((nNwSelMode != AEECM_NETWORK_SEL_MODE_PREF_AUTOMATIC) &&
           (nNwSelMode != AEECM_NETWORK_SEL_MODE_PREF_MANUAL))
  {
    AUX_ERR ("Invalid Scan Mode value, %d", nNwSelMode, 0, 0);
    return FALSE;
  }

  /* Set the network selection mode to the requested value */
  sICMWLANData.network_sel_mode_pref = nNwSelMode;

  /* Populate fields with info from current BSS info */
 
  nReturnStatus = ICM_GetSSInfo(pMe->m_WLAN.m_pICM, &ssInfo, sizeof(AEECMSSInfo));
  if (AEE_SUCCESS != nReturnStatus)
  {
    AUX_ERR("Failed getting serving system info from ICM", 0, 0, 0);
    return FALSE;
  }

  sICMWLANData.chan = ssInfo.wlan_bss_info.chan;
  sICMWLANData.bss_id = ssInfo.wlan_bss_info.bss_id;
  sICMWLANData.bss_type = ssInfo.wlan_bss_info.bss_type;
  sICMWLANData.bss_type_pref = AEECM_WLAN_BSS_TYPE_PREF_NO_CHANGE;
  (void)WSTRLCPY(sICMWLANData.ssid, ssInfo.wlan_bss_info.ssid,
                 AEECM_MAX_SSID_STR_SIZE);
  nErrCode = Convert_ICMTechWLAN_to_ICMWLANTechPref(ssInfo.wlan_bss_info.tech,
                                                        &sICMWLANData.tech_pref);
  if (SUCCESS != nErrCode)
  {
    AUX_ERR("Failed to convert varible from AEECMTechWLAN to AEECMWLANTechPref, %d",
             nErrCode, 0, 0);
    return FALSE;
  }
  if (!AUX_SRV_STATUS_AVAIL(ssInfo.wlan_srv_status))
  {
    sICMWLANData.band = AEECM_BAND_PREF_WLAN_2400_US;
  }
  else
  {
    sICMWLANData.band = AUXWLANBandClassToPref(ssInfo.wlan_bss_info.band);
  }
  
  /* Populate remaining fields with default values */
  sICMWLANData.scan_mode = AEECM_WLAN_SCAN_PREF_NO_CHANGE;
  sICMWLANData.encrypt = AEECM_WLAN_ENC_NONE;
  sICMWLANData.auth = AEECM_WLAN_AUTH_NONE;
  sICMWLANData.mob_ip_profile_id = 0;

  /* Now set up the authentication info */
  nErrCode = AUXApp_FillWLANAuthInfo(pMe, &sICMWLANData);
  if (SUCCESS != nErrCode)
  {
    return FALSE;
  }

  /* One final hack:  If SSID is empty and BSS ID is zero, CM won't
   * accept the request.  The implication is that you can't switch to
   * manual mode if you're not already associated with a system.
   * So, if that condition is met, we'll just put a bogus value in SSID
   * to get CM to accept the switch to the new network selection mode.
   */
  if ((WSTRLEN(sICMWLANData.ssid) == 0) && (sICMWLANData.bss_id == 0)) {
    sICMWLANData.ssid[0] = (AECHAR)255;
    sICMWLANData.ssid[1] = (AECHAR)'\0';
  }

  nErrCode = ICM_SetSystemPreference_WLAN(pMe->m_WLAN.m_pICM,
                                         AEECM_MODE_PREF_NO_CHANGE,
                                         AEECM_PREF_TERM_PERMANENT, 0,
                                         AEECM_GW_ACQ_ORDER_PREF_NO_CHANGE,
                                         AEECM_BAND_PREF_NO_CHANGE,
                                         AEECM_ROAM_PREF_NO_CHANGE,
                                         AEECM_HYBR_PREF_NO_CHANGE,
                                         AEECM_SRV_DOMAIN_PREF_NO_CHANGE,
                                         AEECM_NETWORK_SEL_MODE_PREF_NO_CHANGE,
                                         NULL, &sICMWLANData, &error, NULL);

  if (nErrCode != SUCCESS)
  {
    return FALSE;
  }

  
  return TRUE;

}


/*===========================================================================
FUNCTION SaveIP

DESCRIPTION
  Saves the IP value

DEPENDENCIES


SIDE EFFECTS
  None
===========================================================================*/
boolean SaveIP(AUXApp *pMe)
{
  IDialog *pIDialog;
  ITextCtl *pITextCtl;
  AECHAR tempBuf[IP_BUFFER_SIZE+1];

  if (pMe == NULL) {
    AUX_ERR ("Null pMe pointer", 0, 0, 0);
    return FALSE;
  }

  pIDialog = ISHELL_GetActiveDialog(pMe->a.m_pIShell);
  if(pIDialog == NULL) {
      AUX_ERR("pDialog NULL", 0, 0, 0);
      return FALSE;
  }

  pITextCtl = (ITextCtl*) IDIALOG_GetControl(pIDialog, IDC_TEXT_EDIT_IP);
  if(pITextCtl == NULL) {
      AUX_ERR("pITextCtl NULL", 0, 0, 0);
      return FALSE;
  }

  (void)ITEXTCTL_GetText(pITextCtl, tempBuf, sizeof(tempBuf));

  //lint -save -esym(645, tempBuf) "tempBuf" may not have been initialized
  switch(pMe->m_WLAN.m_IpTypeSelected)
  {
  case IPADDRESS:
    (void)WSTRLCPY(pMe->m_WLAN.m_IpAddr, tempBuf, ARR_SIZE(pMe->m_WLAN.m_IpAddr));
    break;

  case SUBNETMASK:
    (void)WSTRLCPY(pMe->m_WLAN.m_SubnetMask, tempBuf, ARR_SIZE(pMe->m_WLAN.m_SubnetMask));
    break;

  case GATEWAY:
    (void)WSTRLCPY(pMe->m_WLAN.m_Gateway, tempBuf, ARR_SIZE(pMe->m_WLAN.m_Gateway));
    break;

  case DNS:
    (void)WSTRLCPY(pMe->m_WLAN.m_Dns, tempBuf, ARR_SIZE(pMe->m_WLAN.m_Dns));
    break;

  default:
    AUX_ERR("Unknown selection", 0, 0, 0);
    return FALSE;
  }
  return SaveLan(pMe);
}

/*===========================================================================
FUNCTION SaveLan

DESCRIPTION
  Saves the all the LAN IP values

DEPENDENCIES


SIDE EFFECTS
  None
===========================================================================*/
boolean SaveLan(AUXApp *pMe)
{
  WIFIOpt tmp_Opt;
  int   nWIFIErr;

  if (pMe == NULL) {
    AUX_ERR ("Null pMe pointer", 0, 0, 0);
    return FALSE;
  }
  else if ((pMe->m_WLAN.m_pIWIFI == NULL) || (pMe->m_WLAN.m_pIWIFIOpts == NULL)) {
    AUX_ERR ("Null pointer, %x, %x", pMe->m_WLAN.m_pIWIFI, pMe->m_WLAN.m_pIWIFIOpts, 0);
    return FALSE;
  }

  nWIFIErr = IWIFI_LoadOptions(pMe->m_WLAN.m_pIWIFI, 0, pMe->m_WLAN.m_pIWIFIOpts);
  if (SUCCESS != nWIFIErr) {
    AUX_ERR("Unable to load options, %d", nWIFIErr, 0, 0);
    return FALSE;
  }

  /* Set the IP item from the object. */
  tmp_Opt.nId = AEEWIFI_OPT_STATIC_IP;
  tmp_Opt.pVal = pMe->m_WLAN.m_IpAddr;

  nWIFIErr = IWIFIOpts_AddOpt(pMe->m_WLAN.m_pIWIFIOpts, &tmp_Opt);
  if (SUCCESS != nWIFIErr) {
     AUX_ERR("Could not set AEEWIFI_OPT_STATIC_IP, %d",
              nWIFIErr, 0, 0);
  }

  /* Set the DNS item from the object. */
  tmp_Opt.nId = AEEWIFI_OPT_DNS_ADDRESS;
  tmp_Opt.pVal = pMe->m_WLAN.m_Dns;
  nWIFIErr = IWIFIOpts_AddOpt(pMe->m_WLAN.m_pIWIFIOpts, &tmp_Opt);
  if (SUCCESS != nWIFIErr) {
     AUX_ERR("Could not set AEEWIFI_OPT_DNS_ADDRESS, %d",
              nWIFIErr, 0, 0);
  }

  /* Set the subnet item from the object. */
  tmp_Opt.nId = AEEWIFI_OPT_SUBNET_MASK;
  tmp_Opt.pVal = pMe->m_WLAN.m_SubnetMask;
  nWIFIErr = IWIFIOpts_AddOpt(pMe->m_WLAN.m_pIWIFIOpts, &tmp_Opt);
  if (SUCCESS != nWIFIErr) {
     AUX_ERR("Could not set AEEWIFI_OPT_SUBNET_MASK, %d",
              nWIFIErr, 0, 0);
  }

  /* Set the gateway item from the object. */
  tmp_Opt.nId = AEEWIFI_OPT_DEFAULT_GATEWAY;
  tmp_Opt.pVal = pMe->m_WLAN.m_Gateway;
  nWIFIErr = IWIFIOpts_AddOpt(pMe->m_WLAN.m_pIWIFIOpts, &tmp_Opt);
  if (SUCCESS != nWIFIErr) {
     AUX_ERR("Could not set AEEWIFI_OPT_DEFAULT_GATEWAY, %d",
              nWIFIErr, 0, 0);
  }

  nWIFIErr = IWIFI_CommitOptions(pMe->m_WLAN.m_pIWIFI, 0, pMe->m_WLAN.m_pIWIFIOpts);
  if (SUCCESS != nWIFIErr) {
    AUX_ERR("Could not commit LAN options, %d", nWIFIErr, 0, 0);
  }
  else
  {
    nWIFIErr = ICM_SetSubscriptionStatus(pMe->m_WLAN.m_pICM,
                                         AEECM_SYS_MODE_WLAN,
                                         TRUE);
    if (AEE_SUCCESS != nWIFIErr)
    {
      AUX_ERR("Failed to change WLAN subs, %d", nWIFIErr, 0, 0);
    }
  }
  return TRUE;
}

/*===========================================================================
FUNCTION ConvertICMWLANChanToIWIFI

DESCRIPTION
  Used to convert ICM WLAN channel values to IWIFI channel values.

RETURN VALUE
  AUXAPP_BAD_WLAN_CHANNEL, if conversion failed
  IWIFI value for the channel otherwise

DEPENDENCIES
  None

SIDE EFFECTS
  None

===========================================================================*/
uint32 ConvertICMWLANChanToIWIFI(AEECMWLANChannel  WLANChannel,
                                 AEECMSysBandClass Band)
{
  switch (Band)
  {
    case AEECM_SYS_BAND_WLAN_US_2400:
    case AEECM_SYS_BAND_WLAN_JAPAN_2400:
    case AEECM_SYS_BAND_WLAN_EUROPE_2400:
    case AEECM_SYS_BAND_WLAN_FRANCE_2400:
    case AEECM_SYS_BAND_WLAN_SPAIN_2400:
      switch (WLANChannel)
      {
        case AEECM_WLAN_2400_CHAN_1:
          return 1;
        case AEECM_WLAN_2400_CHAN_2:
          return 2;
        case AEECM_WLAN_2400_CHAN_3:
          return 3;
        case AEECM_WLAN_2400_CHAN_4:
          return 4;
        case AEECM_WLAN_2400_CHAN_5:
          return 5;
        case AEECM_WLAN_2400_CHAN_6:
          return 6;
        case AEECM_WLAN_2400_CHAN_7:
          return 7;
        case AEECM_WLAN_2400_CHAN_8:
          return 8;
        case AEECM_WLAN_2400_CHAN_9:
          return 9;
        case AEECM_WLAN_2400_CHAN_10:
          return 10;
        case AEECM_WLAN_2400_CHAN_11:
          return 11;
        case AEECM_WLAN_2400_CHAN_12:
          return 12;
        case AEECM_WLAN_2400_CHAN_13:
          return 13;
        case AEECM_WLAN_2400_CHAN_14:
          return 14;
        default:
          AUX_ERR("Invalid 2.4GHz channel value, %d", WLANChannel, 0, 0);
          break;
      }
      break;

    case AEECM_SYS_BAND_WLAN_US_5000:
    case AEECM_SYS_BAND_WLAN_JAPAN_5000:
    case AEECM_SYS_BAND_WLAN_EUROPE_5000:
    case AEECM_SYS_BAND_WLAN_FRANCE_5000:
    case AEECM_SYS_BAND_WLAN_SPAIN_5000:
      switch (WLANChannel)
      {
        case AEECM_WLAN_5000_CHAN_36:
         return 1;
        case AEECM_WLAN_5000_CHAN_40:
         return 2;
        case AEECM_WLAN_5000_CHAN_44:
         return 3;
        case AEECM_WLAN_5000_CHAN_48:
         return 4;
        case AEECM_WLAN_5000_CHAN_52:
         return 5;
        case AEECM_WLAN_5000_CHAN_56:
         return 6;
        case AEECM_WLAN_5000_CHAN_60:
         return 7;
        case AEECM_WLAN_5000_CHAN_64:
         return 8;
        case AEECM_WLAN_5000_CHAN_149:
         return 9;
        case AEECM_WLAN_5000_CHAN_153:
         return 10;
        case AEECM_WLAN_5000_CHAN_157:
         return 11;
        case AEECM_WLAN_5000_CHAN_161:
         return 12;
        default:
          AUX_ERR("Invalid 5.0GHz channel value, %d", WLANChannel, 0, 0);
          break;
      }
      break;

    default:
      AUX_ERR("Invalid WLAN band class, %d; channel, %d", Band,
               WLANChannel, 0);
      break; 
  }
  return AUXAPP_BAD_WLAN_CHANNEL;
}

/*===========================================================================
FUNCTION Convert_IWIFIAuth_to_ICMAuth

DESCRIPTION
  AEEWIFI_OPT_AUTHENTICATION_TYPE ==> AEECMWLANAuthType
      
RETURN VALUE
  EFAILED if conversion failed
  EBADPARM if pAeeCMVal is NULL
  SUCCESS otherwise

DEPENDENCIES


SIDE EFFECTS
  None
===========================================================================*/
int Convert_IWIFIAuth_to_ICMAuth(AEEWIFIAuthenticationOption nWIFIVal,
                              AEECMWLANAuthType *pAEECMVal)
{
  if (pAEECMVal == NULL) {
    AUX_ERR ("Null pAEECMVal pointer in Convert_IWIFIAuth_to_ICMAuth.", 0, 0, 0);
    return EBADPARM;
  }

  switch (nWIFIVal)
  {
    case AEEWIFI_AUTHENTICATION_NONE:
      *pAEECMVal = AEECM_WLAN_AUTH_NONE;
      return SUCCESS;

    case AEEWIFI_AUTHENTICATION_WEP:
      *pAEECMVal = AEECM_WLAN_AUTH_WEP;
      return SUCCESS;

    case AEEWIFI_AUTHENTICATION_8021X_PASSWORD:
      *pAEECMVal = AEECM_WLAN_AUTH_8021X_PASSWORD;
      return SUCCESS;

    case AEEWIFI_AUTHENTICATION_8021X_TLS:
      *pAEECMVal = AEECM_WLAN_AUTH_8021X_TLS_CERT;
      return SUCCESS;

    case AEEWIFI_AUTHENTICATION_8021X_MD5:
      *pAEECMVal = AEECM_WLAN_AUTH_8021X_MD5;
      return SUCCESS;

    case AEEWIFI_AUTHENTICATION_PAP:
      *pAEECMVal = AEECM_WLAN_AUTH_PAP;
      return SUCCESS;

    case AEEWIFI_AUTHENTICATION_CHAP:
      *pAEECMVal = AEECM_WLAN_AUTH_CHAP;
      return SUCCESS;

    case AEEWIFI_AUTHENTICATION_MSCHAPV2:
      *pAEECMVal = AEECM_WLAN_AUTH_MSCHAPV2;
      return SUCCESS;

    case AEEWIFI_AUTHENTICATION_PEAP:
      *pAEECMVal = AEECM_WLAN_AUTH_PEAP;
      return SUCCESS;

    case AEEWIFI_AUTHENTICATION_TTLS:
      *pAEECMVal = AEECM_WLAN_AUTH_TTLS;
      return SUCCESS;

    default:
      break;
  }
  return EFAILED;
}

/*===========================================================================
FUNCTION Convert_IWIFIEncry_to_ICMEncry

DESCRIPTION
   AEEWIFIEncryptionOption ==> AEECMWLANEncType

RETURN VALUE
  EFAILED if conversion failed
  EBADPARM if pAeeCMVal is NULL
  SUCCESS otherwise

DEPENDENCIES


SIDE EFFECTS
  None
===========================================================================*/
int Convert_IWIFIEncry_to_ICMEncry(AEEWIFIEncryptionOption nWIFIVal, 
                                AEECMWLANEncType *pAEECMVal)
{
  if (pAEECMVal == NULL) {
    AUX_ERR ("Null pAEECMVal pointer in Convert_IWIFIEncry_to_ICMEncry.", 0, 0, 0);
    return EBADPARM;
  }

  switch (nWIFIVal)
  {
    case AEEWIFI_ENCRYPTION_NONE:
      *pAEECMVal = AEECM_WLAN_ENC_NONE;
      return SUCCESS;

    case AEEWIFI_ENCRYPTION_WEP_64bit:
      *pAEECMVal = AEECM_WLAN_ENC_WEP_64BIT;
      return SUCCESS;

    case AEEWIFI_ENCRYPTION_WEP_128bit:
      *pAEECMVal = AEECM_WLAN_ENC_WEP_128BIT;
      return SUCCESS;

    case AEEWIFI_ENCRYPTION_TKIP:
      *pAEECMVal = AEECM_WLAN_ENC_TKIP;
      return SUCCESS;

    case AEEWIFI_ENCRYPTION_AES:
      *pAEECMVal = AEECM_WLAN_ENC_AES;
      return SUCCESS;

    default:
      break;
  }
  return EFAILED;
}

/*===========================================================================
FUNCTION Convert_IWIFIBand_to_ICMBand

DESCRIPTION
  AEEWIFIBandOption ==> AEECMBandPref

RETURN VALUE
  EFAILED if conversion failed
  EBADPARM if pAeeCMVal is NULL
  SUCCESS otherwise

DEPENDENCIES


SIDE EFFECTS
  None
===========================================================================*/
int Convert_IWIFIBand_to_ICMBand(AEEWIFIBandOption nWIFIVal, 
                              AEECMBandPref *pAEECMVal)
{
  if (pAEECMVal == NULL) {
    AUX_ERR ("Null pAEECMVal pointer in Convert_IWIFIBand_to_ICMBand.", 0, 0, 0);
    return EBADPARM;
  }

  switch (nWIFIVal)
  {
    case AEEWIFI_BAND_2400MHZ_US:
      *pAEECMVal = AEECM_BAND_PREF_WLAN_2400_US;
      return SUCCESS;

    case AEEWIFI_BAND_2400MHZ_EUROPE:
      *pAEECMVal = AEECM_BAND_PREF_WLAN_2400_ETSI;
      return SUCCESS;

    case AEEWIFI_BAND_2400MHZ_FRANCE:
      *pAEECMVal = AEECM_BAND_PREF_WLAN_2400_FR;
      return SUCCESS;

    case AEEWIFI_BAND_2400MHZ_SPAIN:
      *pAEECMVal = AEECM_BAND_PREF_WLAN_2400_SP;
      return SUCCESS;

    case AEEWIFI_BAND_2400MHZ_JAPAN:
      *pAEECMVal = AEECM_BAND_PREF_WLAN_2400_JP;
      return SUCCESS;

    case AEEWIFI_BAND_5000MHZ_US:
      *pAEECMVal = AEECM_BAND_PREF_WLAN_5000_US;
      return SUCCESS;

    case AEEWIFI_BAND_5000MHZ_EUROPE:
      *pAEECMVal = AEECM_BAND_PREF_WLAN_5000_ETSI;
      return SUCCESS;

    case AEEWIFI_BAND_5000MHZ_FRANCE:
      *pAEECMVal = AEECM_BAND_PREF_WLAN_5000_FR;
      return SUCCESS;

    case AEEWIFI_BAND_5000MHZ_SPAIN:
      *pAEECMVal = AEECM_BAND_PREF_WLAN_5000_SP;
      return SUCCESS;

    case AEEWIFI_BAND_5000MHZ_JAPAN:
      *pAEECMVal = AEECM_BAND_PREF_WLAN_5000_JP;
      return SUCCESS;

    case AEEWIFI_BAND_ANY:
       *pAEECMVal = AEECM_BAND_PREF_WLAN_ANY;
      return SUCCESS;

    default:
      break;
  }
  return EFAILED;
}

/*===========================================================================
FUNCTION Convert_ICMTechWLAN_to_ICMWLANTechPref

DESCRIPTION
  AEECMTechWLAN ==> AEECMWLANTechPref

RETURN VALUE
  EFAILED if conversion failed
  EBADPARM if pAeeCMVal is NULL
  SUCCESS otherwise

DEPENDENCIES


SIDE EFFECTS
  None
===========================================================================*/
int Convert_ICMTechWLAN_to_ICMWLANTechPref(AEECMTechWLAN nAEECMTechWLANVal,
                                               AEECMWLANTechPref *pAEECMWLANTechPrefVal)
{
  if (pAEECMWLANTechPrefVal == NULL) {
    AUX_ERR ("Null pAEECMWLANTechPrefVal pointer in Convert_ICMTechWLAN_to_ICMWLANTechPref.", 0, 0, 0);
    return EBADPARM;
  }

  switch (nAEECMTechWLANVal)
  {
    case AEECM_TECH_NONE:
      *pAEECMWLANTechPrefVal = AEECM_WLAN_TECH_PREF_NONE;
      return SUCCESS;

    case AEECM_TECH_WLAN_80211A:
      *pAEECMWLANTechPrefVal = AEECM_WLAN_TECH_PREF_A;
      return SUCCESS;

    case AEECM_TECH_WLAN_80211B:
      *pAEECMWLANTechPrefVal = AEECM_WLAN_TECH_PREF_B;
      return SUCCESS;

    case AEECM_TECH_WLAN_80211G:
      *pAEECMWLANTechPrefVal = AEECM_WLAN_TECH_PREF_G;
      return SUCCESS;

    default:
      break;
  }
  return EFAILED;
}

/*===========================================================================
FUNCTION Convert_ICMWLAN_BSS_TYPE_PREF_to_IWIFIModeOpt

DESCRIPTION
  AEECMWLAN_BSS_TYPE_PREF ==> AEEWIFIModeOption

RETURN VALUE
  EFAILED if conversion failed
  EBADPARM if pAeeCMVal is NULL
  SUCCESS otherwise

DEPENDENCIES


SIDE EFFECTS
  None
===========================================================================*/
int Convert_ICMWLAN_BSS_TYPE_PREF_to_IWIFIModeOpt(AEECMWLAN_BSS_TYPE_PREF nBssTypePref,
                                                  AEEWIFIModeOption *pWIFIModeOpt)
{
  if (pWIFIModeOpt == NULL) {
    AUX_ERR ("Null pWIFIModeOpt pointer in Convert_ICMWLAN_BSS_TYPE_PREF_to_IWIFIModeOpt.", 0, 0, 0);
    return EBADPARM;
  }

  switch (nBssTypePref)
  {
    case AEECM_WLAN_BSS_TYPE_PREF_ANY:
      *pWIFIModeOpt = AEEWIFI_MODE_ANY;
      return SUCCESS;

    case AEECM_WLAN_BSS_TYPE_PREF_ADHOC:
      *pWIFIModeOpt = AEEWIFI_MODE_ADHOC;
      return SUCCESS;

    case AEECM_WLAN_BSS_TYPE_PREF_INFRA:
      *pWIFIModeOpt = AEEWIFI_MODE_INFRA;
      return SUCCESS;

    default:
      break;
  }
  return EFAILED;
}
/*===========================================================================
FUNCTION AUXApp_FillWLANAuthInfo

DESCRIPTION
  Gets authentication info from IWIFI and copies it into the ICM WLAN
  Pref structure.  This routine assumes that the authentication and
  encryption types have already been populated.

DEPENDENCIES
  Assumes that the caller has already loaded the options in the
  pMe->m_WLAN.m_pIWIFIOpts.

SIDE EFFECTS
  None
===========================================================================*/
int AUXApp_FillWLANAuthInfo(AUXApp *pMe, AEECMWLANPref *WLANPref)
{

  WIFIOpt TmpOpt;
  int     nWIFIErr;
  uint8  *pKeyBuffer;
  int32   nOptId;

  if (pMe == NULL || WLANPref == NULL) {
    AUX_ERR ("Null pointer", 0,0,0);
    return EFAILED;
  }

  if (WLANPref->auth == AEECM_WLAN_AUTH_WEP)
  {
    if (WLANPref->encrypt == AEECM_WLAN_ENC_WEP_64BIT)
    {
      /* Get the 64-bit WEP key values from IWIFI and copy them */
#if MIN_BREW_VERSIONEx(3,1,4)
      nOptId = AEEWIFI_OPT_WEP_64BIT_KEY_ID;
#else
      nOptId = AEEWIFI_OPT_WEP_KEY_ID;
#endif
      nWIFIErr = IWIFIOpts_GetOpt(pMe->m_WLAN.m_pIWIFIOpts, nOptId, &TmpOpt);
      if (nWIFIErr != SUCCESS)
      {
        AUX_ERR("Unable to load WIFI option %d, err %d", nOptId, nWIFIErr, 0);
        return EFAILED;
      }
      else if ((uint32)TmpOpt.pVal > 3)
      {
        AUX_ERR("WEP64 key ID is invalid %d", (uint32)TmpOpt.pVal, 0, 0);
        return EFAILED;
      }
      else
      {
        WLANPref->auth_info.wep64_info.default_key =
                                               (uint8)(uint32)TmpOpt.pVal;
      }

      /* Initialize the keys to zeros; we'll only fill the one that
       * is in use.
       */
      (void)MEMSET(WLANPref->auth_info.wep64_info.key1, 0,
                   sizeof(WLANPref->auth_info.wep64_info.key1));
      (void)MEMSET(WLANPref->auth_info.wep64_info.key2, 0,
                   sizeof(WLANPref->auth_info.wep64_info.key2));
      (void)MEMSET(WLANPref->auth_info.wep64_info.key3, 0,
                   sizeof(WLANPref->auth_info.wep64_info.key3));
      (void)MEMSET(WLANPref->auth_info.wep64_info.key4, 0,
                   sizeof(WLANPref->auth_info.wep64_info.key4));

#if ! MIN_BREW_VERSIONEx(3,1,4)
      /* Earlier BREW versions only have one WEP key option.  Later
       * versions have separate keys, which are selected below.
       */
      nOptId = AEEWIFI_OPT_WEP_KEY;
#endif

      /* Select the location for copying the key and the correct WIFI option */
      switch (WLANPref->auth_info.wep64_info.default_key)
      {
        case 0:  /* key 1 */
          pKeyBuffer = WLANPref->auth_info.wep64_info.key1;
#if MIN_BREW_VERSIONEx(3,1,4)
          nOptId = AEEWIFI_OPT_WEP_64BIT_KEY_0;
#endif
          break;

        case 1:  /* key 2 */
          pKeyBuffer = WLANPref->auth_info.wep64_info.key2;
#if MIN_BREW_VERSIONEx(3,1,4)
          nOptId = AEEWIFI_OPT_WEP_64BIT_KEY_1;
#endif
          break;

        case 2:  /* key 3 */
          pKeyBuffer = WLANPref->auth_info.wep64_info.key3;
#if MIN_BREW_VERSIONEx(3,1,4)
          nOptId = AEEWIFI_OPT_WEP_64BIT_KEY_2;
#endif
          break;

        case 3:  /* key 4 */
        default: /* Default won't happen, but we have it for completeness */
          pKeyBuffer = WLANPref->auth_info.wep64_info.key4;
#if MIN_BREW_VERSIONEx(3,1,4)
          nOptId = AEEWIFI_OPT_WEP_64BIT_KEY_3;
#endif
          break;

      }

      /* Now get the selected key. */
      nWIFIErr = IWIFIOpts_GetOpt(pMe->m_WLAN.m_pIWIFIOpts, nOptId, &TmpOpt);
      if (nWIFIErr != SUCCESS)
      {
        AUX_ERR("Unable to load WIFI option %d, err %d", nOptId, nWIFIErr, 0);
        return EFAILED;
      }

      /* Convert the key from the wide string stored in IWIFI to the
       * binary value required for ICM.
       */
      nWIFIErr = AUXConvertWStrToBinBuf((AECHAR *)TmpOpt.pVal, pKeyBuffer,
                                 sizeof(WLANPref->auth_info.wep64_info.key1));
      if (nWIFIErr == -1)
      {
        AUX_ERR("Failed converting WEP key", 0, 0, 0);
      }

    }
    else if (WLANPref->encrypt == AEECM_WLAN_ENC_WEP_128BIT)
    {
#if MIN_BREW_VERSIONEx(3,1,4)
      nOptId = AEEWIFI_OPT_WEP_128BIT_KEY_ID;
#else
      nOptId = AEEWIFI_OPT_WEP_KEY_ID;
#endif
      /* Get the 128-bit WEP key values from IWIFI and copy them */
      nWIFIErr = IWIFIOpts_GetOpt(pMe->m_WLAN.m_pIWIFIOpts, nOptId, &TmpOpt);
      if (nWIFIErr != SUCCESS)
      {
        AUX_ERR("Unable to load WIFI option %d, err %d", nOptId, nWIFIErr, 0);
        return EFAILED;
      }
      else if ((uint32)TmpOpt.pVal > 3)
      {
        AUX_ERR("WEP128 key ID is invalid %d", (uint32)TmpOpt.pVal, 0, 0);
        return EFAILED;
      }
      else
      {
        WLANPref->auth_info.wep128_info.default_key =
                                               (uint8)(uint32)TmpOpt.pVal;
      }

      /* Initialize the keys to zeros; we'll only fill the one that
       * is in use.
       */
      (void)MEMSET(WLANPref->auth_info.wep128_info.key1, 0,
                   sizeof(WLANPref->auth_info.wep128_info.key1));
      (void)MEMSET(WLANPref->auth_info.wep128_info.key2, 0,
                   sizeof(WLANPref->auth_info.wep128_info.key2));
      (void)MEMSET(WLANPref->auth_info.wep128_info.key3, 0,
                   sizeof(WLANPref->auth_info.wep128_info.key3));
      (void)MEMSET(WLANPref->auth_info.wep128_info.key4, 0,
                   sizeof(WLANPref->auth_info.wep128_info.key4));

#if ! MIN_BREW_VERSIONEx(3,1,4)
      /* Earlier BREW versions only have one WEP key option.  Later
       * versions have separate keys, which are selected below.
       */
      nOptId = AEEWIFI_OPT_WEP_KEY;
#endif

      /* Select the location for copying the key and the correct WIFI option */
      switch (WLANPref->auth_info.wep128_info.default_key)
      {
        case 0:  /* key 1 */
          pKeyBuffer = WLANPref->auth_info.wep128_info.key1;
#if MIN_BREW_VERSIONEx(3,1,4)
          nOptId = AEEWIFI_OPT_WEP_128BIT_KEY_0;
#endif
          break;

        case 1:  /* key 2 */
          pKeyBuffer = WLANPref->auth_info.wep128_info.key2;
#if MIN_BREW_VERSIONEx(3,1,4)
          nOptId = AEEWIFI_OPT_WEP_128BIT_KEY_1;
#endif
          break;

        case 2:  /* key 3 */
          pKeyBuffer = WLANPref->auth_info.wep128_info.key3;
#if MIN_BREW_VERSIONEx(3,1,4)
          nOptId = AEEWIFI_OPT_WEP_128BIT_KEY_2;
#endif
          break;

        case 3:  /* key 4 */
        default: /* Default won't happen, but we have it for completeness */
          pKeyBuffer = WLANPref->auth_info.wep128_info.key4;
#if MIN_BREW_VERSIONEx(3,1,4)
          nOptId = AEEWIFI_OPT_WEP_128BIT_KEY_3;
#endif
          break;
      }

      /* Now get the selected key. */
      nWIFIErr = IWIFIOpts_GetOpt(pMe->m_WLAN.m_pIWIFIOpts, nOptId, &TmpOpt);
      if (nWIFIErr != SUCCESS)
      {
        AUX_ERR("Unable to load WIFI option %d, err %d", nOptId, nWIFIErr, 0);
        return EFAILED;
      }

      /* Convert the key from the wide string stored in IWIFI to the
       * binary value required for ICM.
       */
      nWIFIErr = AUXConvertWStrToBinBuf((AECHAR *)TmpOpt.pVal, pKeyBuffer,
                                 sizeof(WLANPref->auth_info.wep128_info.key1));
      if (nWIFIErr == -1)
      {
        AUX_ERR("Failed converting WEP key", 0, 0, 0);
      }
    }
    else
    {
      AUX_ERR("Unsupported WEP encryption type, %d", WLANPref->encrypt, 0, 0);
      return EFAILED;
    }
  }
  else if (WLANPref->auth == AEECM_WLAN_AUTH_8021X_PASSWORD)
  {
    /* First get the 8021X Password info */
    nWIFIErr = IWIFIOpts_GetOpt(pMe->m_WLAN.m_pIWIFIOpts, AEEWIFI_OPT_8021X_PASSWORD,
                                &TmpOpt);
    if (nWIFIErr != SUCCESS)
    {
      AUX_ERR("Unable to load WIFI option %d, err %d",
               AEEWIFI_OPT_8021X_PASSWORD, nWIFIErr, 0);
      return EFAILED;
    }

    WLANPref->auth_info.passwd_info.pass_len =
                                  (uint8)WSTRLEN((AECHAR *)TmpOpt.pVal);

    /* Make sure our destination password buffer has enough room for the password.
     * Note that the value stored in IWIFI is AECHARs, and the ICM value
     * must be chars.  If this isn't right, something is *really* messed
     * up, so we're just going to error and give up.
     */
    if (WLANPref->auth_info.passwd_info.pass_len >
        sizeof(WLANPref->auth_info.passwd_info.pass))
    {
      AUX_ERR("Stored 8021X PW is larger than ICM buffer.", 0, 0, 0);
      return EFAILED;
    }

    (void)WSTRTOSTR((AECHAR *)TmpOpt.pVal,
                    WLANPref->auth_info.passwd_info.pass,
                    sizeof(WLANPref->auth_info.passwd_info.pass));
  }
  else if (WLANPref->auth == AEECM_WLAN_AUTH_8021X_TLS_CERT)
  {

    /* First get the 8021X ID info */
    nWIFIErr = IWIFIOpts_GetOpt(pMe->m_WLAN.m_pIWIFIOpts, AEEWIFI_OPT_8021X_ID,
                                &TmpOpt);
    if (nWIFIErr != SUCCESS)
    {
      AUX_ERR("Unable to load WIFI option %d, err %d",
               AEEWIFI_OPT_8021X_ID, nWIFIErr, 0);
      return EFAILED;
    }

    WLANPref->auth_info.tls_cert_info.id_len =
                                 (uint8)WSTRLEN((AECHAR *)TmpOpt.pVal);

    /* Make sure our destination id buffer has enough room for the id.
     * Note that the value stored in IWIFI is AECHARs, and the ICM value
     * must be chars.  If this isn't right, something is *really* messed
     * up, so we're just going to error and give up.
     */
    if (WLANPref->auth_info.tls_cert_info.id_len >
        (int)sizeof(WLANPref->auth_info.tls_cert_info.id))
    {
      AUX_ERR("Stored 8021X ID is larger than ICM buffer.", 0, 0, 0);
      return EFAILED;
    }

    (void)WSTRTOSTR((AECHAR *)TmpOpt.pVal,
                    WLANPref->auth_info.tls_cert_info.id,
                    sizeof(WLANPref->auth_info.tls_cert_info.id));
  }
  else if (WLANPref->auth == AEECM_WLAN_AUTH_8021X_MD5)
  {

    /* First get the 8021X ID info */
    nWIFIErr = IWIFIOpts_GetOpt(pMe->m_WLAN.m_pIWIFIOpts, AEEWIFI_OPT_8021X_ID,
                                &TmpOpt);
    if (nWIFIErr != SUCCESS)
    {
      AUX_ERR("Unable to load WIFI option %d, err %d",
               AEEWIFI_OPT_8021X_ID, nWIFIErr, 0);
      return EFAILED;
    }

    WLANPref->auth_info.md5_info.id_len =
                                 (uint8)WSTRLEN((AECHAR *)TmpOpt.pVal);

    /* Make sure our destination buffer has enough room for the id.
     * Note that the value stored in IWIFI is AECHARs, and the ICM value
     * must be chars.  If this isn't right, something is *really* messed
     * up, so we're just going to error and give up.
     */
    if (WLANPref->auth_info.md5_info.id_len >
        (int)sizeof(WLANPref->auth_info.md5_info.id))
    {
      AUX_ERR("Stored 8021X ID is larger than ICM buffer.", 0, 0, 0);
      return EFAILED;
    }

    (void)WSTRTOSTR((AECHAR *)TmpOpt.pVal,
                    WLANPref->auth_info.md5_info.id,
                    sizeof(WLANPref->auth_info.md5_info.id));

    /* Next get the 8021X Password info */
    nWIFIErr = IWIFIOpts_GetOpt(pMe->m_WLAN.m_pIWIFIOpts,
                                AEEWIFI_OPT_8021X_PASSWORD, &TmpOpt);
    if (nWIFIErr != SUCCESS)
    {
      AUX_ERR("Unable to load WIFI option %d, err %d",
               AEEWIFI_OPT_8021X_PASSWORD, nWIFIErr, 0);
      return EFAILED;
    }

    WLANPref->auth_info.md5_info.pass_len =
                                  (uint8)WSTRLEN((AECHAR *)TmpOpt.pVal);

    /* Make sure our destination buffer has enough room for the password.
     * Note that the value stored in IWIFI is AECHARs, and the ICM value
     * must be chars.  If this isn't right, something is *really* messed
     * up, so we're just going to error and give up.
     */
    if (WLANPref->auth_info.md5_info.pass_len >
        sizeof(WLANPref->auth_info.md5_info.pass))
    {
      AUX_ERR("Stored 8021X PW is larger than ICM buffer.", 0, 0, 0);
      return EFAILED;
    }

    (void)WSTRTOSTR((AECHAR *)TmpOpt.pVal,
                    WLANPref->auth_info.md5_info.pass,
                    sizeof(WLANPref->auth_info.md5_info.pass));
  }
  else if ((WLANPref->auth == AEECM_WLAN_AUTH_NONE) ||
           (WLANPref->auth == AEECM_WLAN_AUTH_PAP) ||
           (WLANPref->auth == AEECM_WLAN_AUTH_CHAP) ||
           (WLANPref->auth == AEECM_WLAN_AUTH_MSCHAPV2) ||
           (WLANPref->auth == AEECM_WLAN_AUTH_PEAP) ||
           (WLANPref->auth == AEECM_WLAN_AUTH_TTLS))
  {
    /* There's no authentication info to fill in, so just return */
    return AEE_SUCCESS;
  }
  else
  {
    AUX_ERR("Unsupported authentication type, %d", WLANPref->auth, 0, 0);
    return EFAILED;
  }
  return AEE_SUCCESS;
}

/*===========================================================================
FUNCTION AUXUtil_GetWLANCb

DESCRIPTION

  List of available wireless LANs is sent by ICM with this callback.


DEPENDENCIES
  None

SIDE EFFECTS
  None
===========================================================================*/
void AUXUtil_GetWLANCb(AUXApp *pMe)
{
  IDialog *pIDialog;
  IMenuCtl *pIMenuCtl = NULL;
  IStatic *pIStaticCtl = NULL;
  uint16 array_offset = 0;
  uint16 array_offset2;
  AEERect CtrlRect;
  char buf[AUX_MAX_STR_SIZE];
  AECHAR wBuf[AUX_MAX_STR_SIZE];
  AECHAR wMenuBuf[AUX_MAX_STR_SIZE];
  AEECMWLANBSSInfoList * pCopyDetailedBSSList;
  uint32 nBufSize;
  AECHAR *pWLANTechTypeMessage;
  AEEDeviceInfo di;

  AUX_MSG("Get WLAN Cb", 0, 0, 0);

  if (pMe == NULL) {
    AUX_ERR ("Null pointer", 0,0,0);
    return;
  }

  //First Make sure we are in the previous "Searching" Dialog.
  pIDialog = ISHELL_GetActiveDialog(pMe->a.m_pIShell);
  if ((pIDialog == NULL) || (IDD_WLAN_SELECT != IDIALOG_GetID(pIDialog)) ||
      (pMe->m_WLAN.m_bWLANSearching == FALSE)) {
    AUX_ERR("GetWLANCb in non-search state %d %d %x", pMe->m_WLAN.m_bWLANSearching,
             pIDialog?IDIALOG_GetID(pIDialog):0, pIDialog);
    pMe->m_WLAN.m_bWLANSearching = FALSE;
    return;
  }
  pMe->m_WLAN.m_bWLANSearching = FALSE;

  /* Correct dialog and state.  But no results were found.  End the
   * current dialog and display the error message. */
  if((pMe->m_WLAN.m_pDetailedBSSList == NULL) || (pMe->m_WLAN.m_pDetailedBSSList->length == 0)) {
    AUX_ERR("Empty BSS List", 0, 0, 0);
    pMe->m_WLAN.m_nNWSelectionMode = APPCOMFUNC_NW_SEL_INACTIVE;

    //Now display the message.
    (void)ISHELL_EndDialog(pMe->a.m_pIShell);
    AUXDisplayMessageDlg(pMe, IDS_NO_WLAN_NETWORKS, 3);
    return;
  }

  /* Remove the searching static control, create the list in the Menu Ctl. */
  pIStaticCtl = (IStatic*) IDIALOG_GetControl(pIDialog, IDC_STATIC_WLAN_SEARCHING);
  if ( pIStaticCtl == NULL ) { /* error checking */
    AUX_ERR ("Null pointer for Static Control", 0,0,0);
  }
  else
  {
    ISTATIC_SetActive(pIStaticCtl, FALSE);
  }

  pIMenuCtl = (IMenuCtl *) IDIALOG_GetControl(pIDialog, IDC_LIST_AVAIL_NETWORKS);
  if ( pIMenuCtl == NULL ) { /* error checking */
    AUX_ERR ("Null pointer for Menu Control", 0,0,0);
    (void)ISHELL_EndDialog(pMe->a.m_pIShell);
    return;
  }

  (void) IMENUCTL_DeleteAll(pIMenuCtl);
  ISHELL_GetDeviceInfo(pMe->a.m_pIShell,&di);
  CtrlRect.y = CtrlRect.x = 0;
  CtrlRect.dx = (int16)di.cxScreen;
  CtrlRect.dy = (int16)di.cyScreen;
  IMENUCTL_SetRect(pIMenuCtl, &CtrlRect);

  pCopyDetailedBSSList = (AEECMWLANBSSInfoList *)MALLOC(sizeof(AEECMWLANBSSInfoList));

  if(pCopyDetailedBSSList == NULL) {
    AUX_ERR("MALLOC Failed", 0, 0, 0);
    //Now display the message.
    (void)ISHELL_EndDialog(pMe->a.m_pIShell);
    AUXDisplayMessageDlg(pMe, IDS_NO_WLAN_NETWORKS, 3);
    return;
  }

  (void)MEMCPY( pCopyDetailedBSSList, pMe->m_WLAN.m_pDetailedBSSList, sizeof(AEECMWLANBSSInfoList) );

  /*  Check for duplicate SSIDs for best RSSI and filter accordingly.    */
  /*  Consider adding global user filters here                           */
  for (array_offset = 0; array_offset < pCopyDetailedBSSList->length; array_offset++)
  {
    for ( array_offset2 = array_offset+1; array_offset2 < pCopyDetailedBSSList->length; array_offset2++) {
      if (0 == WSTRNCMP(pCopyDetailedBSSList->info[array_offset].ssid,
                pCopyDetailedBSSList->info[array_offset2].ssid,
                AEECM_MAX_SSID_STR_SIZE )) {
        /*  compare RSSI values, (-DB) lower number is stronger signal          */
        /*  WARNING:  the empty AP's SSID will indicate to the loop immediately */
        /*  below to skip the AP, and not add it to the menu items              */
        if ( pCopyDetailedBSSList->info[array_offset2].rssi <
             pCopyDetailedBSSList->info[array_offset].rssi  ) {
          pCopyDetailedBSSList->info[array_offset].ssid[0] = (AECHAR)'\0';
        }
        else {
          pCopyDetailedBSSList->info[array_offset2].ssid[0] = (AECHAR)'\0';
        }
      }
    }
  }

  for (array_offset = 0; array_offset < pCopyDetailedBSSList->length; array_offset++)
  {
    /* Ignore BSS List members with empty SSID strings.  Use SSID, tech, and RSSI */
    /* for menu string. Send constructed string to IMENUCTL_AddItem.              */
    if (0 != WSTRLEN(pCopyDetailedBSSList->info[array_offset].ssid)) {

      switch (pCopyDetailedBSSList->info[array_offset].tech ) {
         case AEECM_TECH_WLAN_80211A:
           (void) ISHELL_LoadResDataEx(pMe->a.m_pIShell, gpAUXResourceFile,
                                       IDS_WLAN_TECH_A, RESTYPE_STRING,
                                       (void*)-1, &nBufSize);
           nBufSize = (nBufSize+1)*sizeof(AECHAR);
           pWLANTechTypeMessage = (AECHAR*) MALLOC(nBufSize);
           if(pWLANTechTypeMessage == NULL)
           {
             AUX_ERR("Unable to allocate", 0, 0, 0);
             return;
           }
           (void) ISHELL_LoadResString(pMe->a.m_pIShell, gpAUXResourceFile,
                              IDS_WLAN_TECH_A, pWLANTechTypeMessage, (int)nBufSize);
           FREE(pWLANTechTypeMessage);
         break;

         case AEECM_TECH_WLAN_80211B:
           (void) ISHELL_LoadResDataEx(pMe->a.m_pIShell, gpAUXResourceFile,
                                       IDS_WLAN_TECH_B, RESTYPE_STRING,
                                       (void*)-1, &nBufSize);
           nBufSize = (nBufSize+1)*sizeof(AECHAR);
           pWLANTechTypeMessage = (AECHAR*) MALLOC(nBufSize);
           if(pWLANTechTypeMessage == NULL)
           {
             AUX_ERR("Unable to allocate", 0, 0, 0);
             return;
           }
           (void) ISHELL_LoadResString(pMe->a.m_pIShell, gpAUXResourceFile,
                                       IDS_WLAN_TECH_B, pWLANTechTypeMessage, (int)nBufSize);
           FREE(pWLANTechTypeMessage);
         break;

         case  AEECM_TECH_WLAN_80211G:
           (void) ISHELL_LoadResDataEx(pMe->a.m_pIShell, gpAUXResourceFile,
                                       IDS_WLAN_TECH_G, RESTYPE_STRING,
                                       (void*)-1, &nBufSize);
           nBufSize = (nBufSize+1)*sizeof(AECHAR);
           pWLANTechTypeMessage = (AECHAR*) MALLOC(nBufSize);
           if(pWLANTechTypeMessage == NULL)
           {
             AUX_ERR("Unable to allocate", 0, 0, 0);
             return;
           }
           (void) ISHELL_LoadResString(pMe->a.m_pIShell, gpAUXResourceFile,
                                       IDS_WLAN_TECH_G, pWLANTechTypeMessage, (int)nBufSize);
           FREE(pWLANTechTypeMessage);
         break;

         default:
           (void) ISHELL_LoadResDataEx(pMe->a.m_pIShell, gpAUXResourceFile,
                                       IDS_WLAN_TECH_UNKNOWN, RESTYPE_STRING,
                                       (void*)-1, &nBufSize);
           nBufSize = (nBufSize+1)*sizeof(AECHAR);
           pWLANTechTypeMessage = (AECHAR*) MALLOC(nBufSize);
           if(pWLANTechTypeMessage == NULL)
           {
              AUX_ERR("Unable to allocate", 0, 0, 0);
              return;
           }
           (void) ISHELL_LoadResString(pMe->a.m_pIShell, gpAUXResourceFile,
                                       IDS_WLAN_TECH_UNKNOWN, pWLANTechTypeMessage, (int)nBufSize);
           FREE(pWLANTechTypeMessage);
         break;
      }

      (void)WSTRLCPY(wMenuBuf, pCopyDetailedBSSList->info[array_offset].ssid,
                     AEECM_MAX_SSID_STR_SIZE);
      (void)WSTRLCAT(wMenuBuf, pWLANTechTypeMessage, ARR_SIZE(wMenuBuf));
  
      (void)SNPRINTF(buf, sizeof(buf), " %d",
                    AUXDBToLevel(pCopyDetailedBSSList->info[array_offset].rssi) - AUX_WLAN_NO_BARS);
      (void)STRTOWSTR(buf, wBuf, sizeof(AECHAR)* AUX_MAX_STR_SIZE);
      (void)WSTRLCAT(wMenuBuf, wBuf, ARR_SIZE(wMenuBuf));

      if( !IMENUCTL_AddItem(pIMenuCtl, gpAUXResourceFile, 0, array_offset,
                            wMenuBuf,
                            (uint32) &pMe->m_WLAN.m_pDetailedBSSList->info[array_offset])) {
        char pszSSID[AEECM_MAX_SSID_STR_SIZE];
        (void)WSTRTOSTR(pMe->m_WLAN.m_pDetailedBSSList->info[array_offset].ssid, pszSSID,
                        sizeof(pszSSID));
        AUX_ERR("Unable to add item %d (%c%c) to list", array_offset, pszSSID[0], pszSSID[1]);
      }
    }
  }
  FREEIF(pCopyDetailedBSSList);
  IMENUCTL_SetActive(pIMenuCtl, TRUE);
}

/*=============================================================================
FUNCTION: AUXWLANBandClassToPref

DESCRIPTION:
  Converts a WLAN-related ICM band class to a band preference.  If the
  band class cannot be converted, returns AEECM_BAND_PREF_NONE.
  
DEPENDENCIES:
  None

SIDE EFFECTS:
  None

=============================================================================*/
AEECMBandPref AUXWLANBandClassToPref(AEECMSysBandClass BandClass)
{
  int nPrefIdx;

  for (nPrefIdx = 0; nPrefIdx < ARRAY_SIZE(AUXICMBandMapTable); nPrefIdx++)
  {
    if (BandClass == AUXICMBandMapTable[nPrefIdx].ICMBandClass)
    {
      return AUXICMBandMapTable[nPrefIdx].ICMBandPref;
    }
  }

  MSG_MED("Invalid band class, 0x%x", BandClass, 0, 0);
  return AEECM_BAND_PREF_NONE;
}

/*===========================================================================
FUNCTION CheckWLANAutoScanNewAssociation

DESCRIPTION
  Checks to see if a CM service change event resulted in a new WLAN AP 
  association.  If so, it updates the display accordingly

DEPENDENCIES
  None

SIDE EFFECTS
  None
===========================================================================*/
void CheckWLANAutoScanNewAssociation(void *pUser)
{
  AUXApp *pMe = (AUXApp*) pUser;
  IDialog * pIDialog;
  IStatic * pIStaticText = NULL;
  uint32 wID = IDC_WLAN_AUTOSCAN_SEARCHING;
  uint32 nBufSize = 0;
  uint16 wDlgID = 0;
  AECHAR *pMessage;
  AEECMSSInfo ssInfo;
  int nReturnStatus;


  if(pMe == NULL)
  {
    AUX_ERR("NULL pointer", 0, 0, 0);
    return;
  }

  nReturnStatus = ICM_GetSSInfo(pMe->m_WLAN.m_pICM, &ssInfo, sizeof(AEECMSSInfo));
  if (AEE_SUCCESS != nReturnStatus)
  {
    AUX_ERR("Failed getting serving system info from ICM", 0, 0, 0);
    return;
  }

  /* check if SSID for associated AP has changed  */
  if (0 == WSTRCMP(pMe->m_WLAN.m_currentSSID, 
                   ssInfo.wlan_bss_info.ssid)) {
    return;
  }

  pIDialog = ISHELL_GetActiveDialog(pMe->a.m_pIShell);
  if (pIDialog == NULL) {
    AUX_ERR("NULL pointer", 0, 0, 0);
    return;
  }

  wDlgID = IDIALOG_GetID(pIDialog);
  if (IDD_WLAN_AUTOSCAN_SEARCHING != wDlgID) {
    return;
  }

  (void) ISHELL_LoadResDataEx(pMe->a.m_pIShell, gpAUXResourceFile,
                              IDS_CONNECT, RESTYPE_STRING, (void*)-1, 
                              &nBufSize);

  nBufSize = (nBufSize + AEECM_MAX_SSID_STR_SIZE + 1 )*sizeof(AECHAR);

  pMessage = (AECHAR*) MALLOC(nBufSize);
  if(pMessage == NULL)
  {
     AUX_ERR("Unable to allocate", 0, 0, 0);
     return;
  }

  (void)ISHELL_CancelTimer(pMe->a.m_pIShell, 
                     (PFNNOTIFY) WLANAutoScanConnectFailed, pMe);
       
  (void)WSTRLCPY(pMe->m_WLAN.m_currentSSID, 
                 ssInfo.wlan_bss_info.ssid,
                 AEECM_MAX_SSID_STR_SIZE);

  (void) ISHELL_LoadResString(pMe->a.m_pIShell, gpAUXResourceFile,
                              IDS_CONNECT, pMessage, (int)nBufSize);
  (void )WSTRLCAT(pMessage, ssInfo.wlan_bss_info.ssid, nBufSize/sizeof(AECHAR));

  pIStaticText = (IStatic*)IDIALOG_GetControl(pIDialog, (int16)wID);
  if (pIStaticText != NULL) {
    (void)ISTATIC_SetText (pIStaticText, NULL, pMessage,
                           AEE_FONT_BOLD, AEE_FONT_BOLD);
    /* set the static text style */
    (void) AUXSetGeneralStaticTextStyle(pIStaticText);  
    /* Force the control to redraw */
    (void)ICONTROL_Redraw((IControl*)pIStaticText);
  }
  (void)ISHELL_SetTimer(pMe->a.m_pIShell, AUXAPP_CONNECTED_DISPLAY_TIME, 
                  (PFNNOTIFY) AUXEndCurrentDlg, (void*) pMe);
  FREE(pMessage);
 }



