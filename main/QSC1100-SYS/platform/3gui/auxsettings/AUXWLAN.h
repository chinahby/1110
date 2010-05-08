#ifndef AUXWLAN_H
#define AUXWLAN_H
/*=============================================================================

FILE: AUXWLAN.h

SERVICES: QCT UI AUX WLAN Configuration menu

GENERAL DESCRIPTION:
.h file for AUXWLAN.

(c) COPYRIGHT 2007 QUALCOMM Incorporated.
                   All Rights Reserved.

                   QUALCOMM Proprietary
=============================================================================*/
/*===========================================================================
                        EDIT HISTORY FOR MODULE

when       who     what, where, why
--------   ---     ----------------------------------------------------------
03/27/07   ypmw    Fix compiler errors
02/23/07   jas     Refactoring for HDK builds
01/23/07   ypmw    Move WLAN application from Core App to AUX Settings

===========================================================================*/

/*=============================================================================

                       INCLUDES

=============================================================================*/
/* Brew libraries */
#include "AUXSettings.h"
#include "AEEShell.h"
#include "AEECM.h"
#include "AEEWIFI.h"
#include "AEEWIFIExt.h"

/*=============================================================================

                       AUXWLAM APP FEATURE_WLAN

=============================================================================*/

/* Types of addresses for LAN */
#define IPADDRESS   0
#define SUBNETMASK  1
#define GATEWAY     2
#define DNS         3
#define IP_BUFFER_SIZE      16      /* size of the IP string */

#define AUXAPP_CONNECTED_DISPLAY_TIME              (2*1000)
#define AUXAPP_NO_WLAN_AP_FOUND_TIME               (5*1000)
#define AUXAPP_WLAN_AUTOSCAN_NO_ASSOCIATION_TIME   (2*1000)
#define APPCOMFUNC_NW_SEL_INACTIVE                 0 /* No network scan in progress. */
#define APPCOMFUNC_NW_SEL_WLAN                     1 /* WLAN scan in progress. */

//The size of AUXAPP_64BIT_WEP_KEY_STR_SIZE is twice of AEECM_64BIT_WEP_KEY_SIZE
//because the number of hex digits that the user can enter is 10 and each one of 
//them is 4 bits hex number.  Thus when converting from str to binary digit, two 
//ascii chars will be stored in one uint8 variable. AEECM_64BIT_WEP_KEY_SIZE is the 
//number of uint8 variables to represent one key.  The convertion from str to binary
//digits can be referred from 
//static int ConvertStrToBinBuf( const char *pszwIn, uint8 *pBuf, uint8 nBufLen );
//in OEMCM.c file.
#define AUXAPP_64BIT_WEP_KEY_STR_SIZE            (2*AEECM_64BIT_WEP_KEY_SIZE+1)
#define AUXAPP_128BIT_WEP_KEY_STR_SIZE           (2*AEECM_128BIT_WEP_KEY_SIZE+1)

typedef struct {
  uint8      default_key;
  AECHAR     key1[AUXAPP_64BIT_WEP_KEY_STR_SIZE];
  AECHAR     key2[AUXAPP_64BIT_WEP_KEY_STR_SIZE];
  AECHAR     key3[AUXAPP_64BIT_WEP_KEY_STR_SIZE];
  AECHAR     key4[AUXAPP_64BIT_WEP_KEY_STR_SIZE];
} AUXAppWLANWEP64AuthInfo;

typedef struct {
  uint8      default_key;
  AECHAR     key1[AUXAPP_128BIT_WEP_KEY_STR_SIZE];
  AECHAR     key2[AUXAPP_128BIT_WEP_KEY_STR_SIZE];
  AECHAR     key3[AUXAPP_128BIT_WEP_KEY_STR_SIZE];
  AECHAR     key4[AUXAPP_128BIT_WEP_KEY_STR_SIZE];
} AUXAppWLANWEP128AuthInfo;

typedef struct {
  uint8      pass_len;
  AECHAR     pass[AEECM_8021X_PASSWD_STR_SIZE];
} AUXAppWLAN8021XPASSAuthInfo;

typedef struct {
  uint8      id_len;
  AECHAR     id[AEECM_8021X_USER_ID_STR_SIZE];
} AUXAppWLANTLSAuthInfo;

typedef struct {
  uint8      id_len;
  AECHAR     id[AEECM_8021X_USER_ID_STR_SIZE];
  uint8      cert_id;
} AUXAppWLANTLSCertAuthInfo;

typedef struct {
  uint8      id_len;
  AECHAR     id[AEECM_8021X_USER_ID_STR_SIZE];
  uint8      pass_len;
  AECHAR     pass[AEECM_8021X_PASSWD_STR_SIZE];
} AUXAppWLANMD5AuthInfo;

typedef union {
  AUXAppWLANWEP64AuthInfo      wep64_info;
  AUXAppWLANWEP128AuthInfo     wep128_info;
  AUXAppWLAN8021XPASSAuthInfo  passwd_info;
  AUXAppWLANTLSCertAuthInfo    tls_cert_info;
  AUXAppWLANMD5AuthInfo        md5_info;
} AUXAppWLANAuthInfo;


typedef struct
{
  ICM                  *m_pICM;
  IWIFI                *m_pIWIFI;
  IWIFIOpts            *m_pIWIFIOpts;
  boolean               m_bIPsUpdated;    /* TRUE==>IWIFI updated, but not pMe */
  boolean               m_bWLANSearching; /* Waiting for WLAN system info
                                           * from CM.
                                           */
  IDialog              *m_pWLANStatusDlg; /* Ptr to the status dialog */
  AEECMWLANBSSInfoList *m_pDetailedBSSList;
  AEEWIFIModeOption           m_WLANMode;
  AEEWIFIEncryptionOption     m_WLANEnc;
  AEEWIFIAuthenticationOption m_WLANAuth;
  AEEWIFIPreambleOption       m_WLANPreamble;
  boolean      m_bDhcpOn;         /* Are we using DHCP? (vs static IP) */
  int          m_IpTypeSelected;  /* Current IP type being edited */
  AECHAR       m_IpAddr[IP_BUFFER_SIZE+1];
  AECHAR       m_SubnetMask[IP_BUFFER_SIZE+1];
  AECHAR       m_Gateway[IP_BUFFER_SIZE+1];
  AECHAR       m_Dns[IP_BUFFER_SIZE+1];
  AEECMWLAN_SSID  m_currentSSID;

  /* m_bWIFIOptsLoaded is used to manage the WIFI/WIFIOpts interfaces
   * while modifying authentication keys.  In particular, it should be
   * set to TRUE when editing the keys begins.  When the changes are
   * committed or the key modification is cancelled, it should be reset
   * to FALSE.  While modifying the keys, if this flag is TRUE, the
   * options should *NOT* be loaded again.  Otherwise, any changes
   * stored in the IWIFIOpts will be lost.
   */
  boolean      m_bWIFIOptsLoaded;

  /* m_bAuthKeysUpdated is used to indicate if the authentication data
   * screen needs to be updated because the user edited key data.
   */
  boolean               m_bAuthKeysUpdated;

  /* m_bNetSelModeUpdated is used to indicate if the network selection mode
   * screen needs to be updated because the user changed the mode.
   */
  boolean               m_bNetSelModeUpdated;
  IDialog               *m_pWLANSettingsDialog;

/* m_psICMWLANData and  m_WLANBandPref are used to hold WLAN AP 
* information while moving among security dialogs during manual 
* scan and association. 
*/
  AEECMWLANPref         *m_psICMWLANData;
  AEECMBandPref         m_WLANBandPref;

/* m_bWLANManualScanAssociationRequest is used to control the AP
*  association request in WLANSecDlgEventHandler. The request 
*  depends on whether a manual or automatic association is needed. 
*  m_bWLANManualScanAssociationRequest is set to true when entering
*  the WLAN Security dialog from the Manual Scan dialog. It is set 
*  to false when entering the WLAN Security dialog from the WLAN 
*  Settings dialog (the path for an automatic association request).
*/
  boolean               m_bWLANManualScanAssociationRequest;

/* m_nAuthType and m_nEncrypType store WLAN security settings before 
 * editing.  These are used to revert to original settings if the edit
 * changes are cancelled
*/
  AEEWIFIAuthenticationOption     m_nAuthType;
  AEEWIFIEncryptionOption         m_nEncrypType;
  AEEWIFIAuthenticationOption     m_nprevAuthType;
  AEEWIFIEncryptionOption         m_nprevEncrypType;
  AUXAppWLANAuthInfo              *m_pWLANAuthInfo;

/* m_bWLANAdhocAssociationRequest is used to control the AP
*  association request in WLANSecDlgEventHandler. The request 
*  depends on whether a adhoc association is needed. 
*/
  boolean               m_bWLANAdhocAssociationRequest;
  uint16                m_nNWSelectionMode;     /* Network Selection Mode */
  AEECallback           m_nCallback;            /*GetNetworks callback.*/
  AEECMPhError          m_phError;              /* Error in ICM_GetNetworks */

} AUXWLAN;

#endif /* AUXWLAN_H */


