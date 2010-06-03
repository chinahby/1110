#ifndef AUXWLANUTILS_H
#define AUXWLANUTILS_H
/*===========================================================================

FILE: AUXWLANUtils.h

DESCRIPTION
   Header file for AUXWLANUtils.c

PUBLIC CLASSES:
   N/A

INITIALIZATION AND SEQUENCING REQUIREMENTS:


       Copyright © 2007 QUALCOMM Incorporated.
                    All Rights Reserved.
                   QUALCOMM Proprietary
===========================================================================*/
/*===========================================================================
                        EDIT HISTORY FOR MODULE

when       who     what, where, why
--------   ---     ----------------------------------------------------------
02/23/07   jas     Refactoring for HDK builds
01/23/07   ypmw    Move WLAN application from Core App to AUX Settings

===========================================================================*/


/*=============================================================================

                INCLUDES AND VARIABLE DEFINITIONS

=============================================================================== */
#include "AUXSettings.h"
#include "AUXUtils.h"

#define AUXAPP_BAD_WLAN_CHANNEL 0xFFFFFFFF


/*=============================================================================

                       FUNCTION DECLARATIONS

=============================================================================*/

/*===========================================================================
FUNCTION GetWLANSecItem

DESCRIPTION
  Gets the current authentication and encryption settings from IWIFI and
  returns the corresponding item ID on the WLAN Security menu.  (For
  compatibility with the menu action table, it returns uint64, rather than
  uint16.)

DEPENDENCIES
  None

SIDE EFFECTS
  None
===========================================================================*/
uint64 GetWLANSecItem(AUXApp *pMe);

/*===========================================================================
FUNCTION GetDhcp

DESCRIPTION
  Get DHCP value

DEPENDENCIES
  None

SIDE EFFECTS
  None
===========================================================================*/
uint64 GetDhcp (AUXApp *pMe);

/*===========================================================================
FUNCTION SetDhcp

DESCRIPTION
  assign DCHP value 

DEPENDENCIES
  

SIDE EFFECTS
  None
===========================================================================*/
boolean SetDhcp(AUXApp *pMe, uint64 nNewVal);

/*===========================================================================
FUNCTION GetWLANMode

DESCRIPTION
  Gets the Global WLAN mode preference

DEPENDENCIES
  None

SIDE EFFECTS
  None
===========================================================================*/
uint64 GetWLANMode(AUXApp * pMe);

/*===========================================================================
FUNCTION SetWLANMode

DESCRIPTION
  Sets the Global WLAN mode preference

DEPENDENCIES
  None

SIDE EFFECTS
  None
===========================================================================*/
boolean SetWLANMode(AUXApp * pMe, uint64 nNewVal);

/*===========================================================================
FUNCTION GetWLANModeProfile

DESCRIPTION
  Gets the WLAN mode for the currently selected profile

DEPENDENCIES
  None

SIDE EFFECTS
  None
===========================================================================*/
uint64 GetWLANModeProfile(AUXApp * pMe);

/*===========================================================================
FUNCTION SetWLANModeProfile

DESCRIPTION
  Sets the WLAN mode for the currently selected profile

DEPENDENCIES
  None

SIDE EFFECTS
  None
===========================================================================*/
boolean SetWLANModeProfile(AUXApp * pMe, uint64 nNewVal);

/*===========================================================================
FUNCTION GetWLANScanMode

DESCRIPTION
  Gets the WLAN scan mode from pMe

DEPENDENCIES
  None

SIDE EFFECTS
  None
===========================================================================*/
uint64 GetWLANScanMode(AUXApp * pMe);

/*===========================================================================
FUNCTION SetWLANScanMode

DESCRIPTION
  Sets the WLAN scan mode to the value in nNewValue

DEPENDENCIES
  None

SIDE EFFECTS
  None
===========================================================================*/
boolean SetWLANScanMode(AUXApp * pMe, uint64 nNewVal);


/*===========================================================================
FUNCTION GetWLANEnc

DESCRIPTION
  Gets the WLAN encryption type from the WIFI options

DEPENDENCIES
  None

SIDE EFFECTS
  None
===========================================================================*/
uint64 GetWLANEnc(AUXApp * pMe);

/*===========================================================================
FUNCTION GetWLANAuth

DESCRIPTION
  Gets the WLAN authorization type from the WIFI options

DEPENDENCIES
  None

SIDE EFFECTS
  None
===========================================================================*/
uint64 GetWLANAuth(AUXApp * pMe);

/*===========================================================================
FUNCTION GetWLANPreamble

DESCRIPTION
  Gets the WLAN preamble length from the WIFI options

DEPENDENCIES
  None

SIDE EFFECTS
  None
===========================================================================*/
uint64 GetWLANPreamble(AUXApp * pMe);

/*===========================================================================
FUNCTION SetWLANPreamble

DESCRIPTION
  Sets the WLAN preamble length in the WIFI optinos

DEPENDENCIES
  None

SIDE EFFECTS
  None
===========================================================================*/
boolean SetWLANPreamble(AUXApp * pMe, uint64 nNewVal);


/*===========================================================================
FUNCTION GetWLANNwSelMode

DESCRIPTION
  Gets the WLAN network selection mode from ICM

DEPENDENCIES
  None

SIDE EFFECTS
  None
===========================================================================*/
uint64 GetWLANNwSelMode(AUXApp * pMe);

/*===========================================================================
FUNCTION SetWLANNwSelMode

DESCRIPTION
  Sets the WLAN network selection mode in CM

DEPENDENCIES
  None

SIDE EFFECTS
  None
===========================================================================*/
boolean SetWLANNwSelMode(AUXApp * pMe, uint64 nNewVal);


/*===========================================================================
FUNCTION SaveIP

DESCRIPTION
  Saves the IP value 

DEPENDENCIES
  

SIDE EFFECTS
  None
===========================================================================*/
boolean SaveIP(AUXApp *pMe);

/*===========================================================================
FUNCTION SaveLan

DESCRIPTION
  Saves all the LAN IP values 

DEPENDENCIES
  

SIDE EFFECTS
  None
===========================================================================*/
boolean SaveLan(AUXApp *pMe);

/*===========================================================================
FUNCTION ConvertICMWLANChanToIWIFI

DESCRIPTION
  Used to convert ICM WLAN channel values to IWIFI channel values.

RETURN VALUE
  0xFFFF if conversion failed
  IWIFI value for the channel otherwise

DEPENDENCIES
  None

SIDE EFFECTS
  None

===========================================================================*/
uint32 ConvertICMWLANChanToIWIFI(AEECMWLANChannel  WLANChannel,
                                 AEECMSysBandClass Band);

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
                              AEECMWLANAuthType *nAEECMVal);

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
                                AEECMWLANEncType *pAEECMVal);

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
                              AEECMBandPref *pAEECMVal);

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
                                               AEECMWLANTechPref *pAEECMWLANTechPrefVal);

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
                                                  AEEWIFIModeOption *pWIFIModeOpt);


/*===========================================================================
FUNCTION AUXApp_FillWLANAuthInfo

DESCRIPTION
  Gets authentication info from IWIFI and copies it into the ICM WLAN
  Pref structure.  This routine assumes that the authentication and
  encryption types have already been populated.

DEPENDENCIES
  Assumes that the caller has already loaded the options in the
  pMe->m_pIWIFIOpts.

SIDE EFFECTS
  None
===========================================================================*/
int AUXApp_FillWLANAuthInfo(AUXApp *pMe, AEECMWLANPref *WLANPref);

/*===========================================================================
FUNCTION AUXUtil_GetWLANCb

DESCRIPTION

  List of avialable PLMNs is sent by ICM with this callback.
  

DEPENDENCIES
  None

SIDE EFFECTS
  None
===========================================================================*/
void AUXUtil_GetWLANCb(AUXApp *pMe);

/*===========================================================================
FUNCTION AUXWLANUtil_CancelGetWLAN

DESCRIPTION
  Cancel the call to ICM_GetNetworks and free allocated memory.

DEPENDENCIES
  None

SIDE EFFECTS
  None
===========================================================================*/
void AUXWLANUtil_CancelGetWLAN(AUXApp *pMe);

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
AEECMBandPref AUXWLANBandClassToPref(AEECMSysBandClass BandClass);

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
void CheckWLANAutoScanNewAssociation(void *pUser);

#endif /* AUXWLANUTIL_H */
