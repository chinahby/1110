#ifndef AEECARD_H
#define AEECARD_H

/*============================================================================
FILE:  AEECard.h

SERVICES:  AEE ICard Interface

GENERAL DESCRIPTION:
        Base level definitions, typedefs, etc.

        Copyright © 2003-2009 QUALCOMM Incorporated.
               All Rights Reserved.
            QUALCOMM Proprietary/GTDR
*====*====*====*====*====*====*====*====*====*====*====*====*====*====*====*/


/*===========================================================================
                        EDIT HISTORY FOR MODULE

$Header: //source/qcom/qct/modem/uim/icard/rel/07H1_2/inc/AEECard.h#2 $

when       who     what, where, why
--------   ---     ----------------------------------------------------------
06/19/09   rn      Added enum for accessing EF-CFIS.
09/05/08   sp      Fixed warning.
03/20/08   nk      Added OMH EF Support
11/08/07   vs      Added OMH feature support
10/26/07   sun     Added new API to get the ATR asynchronously
01/18/07   sun     Fixed services type for CBMI
04/06/06   sun     Added support for RUIM Lock
02/16/06   sp      Support for EF_EHPLMN
10/20/05   sun     Added support for BT Deregisteration
10/18/05   sun     Added support for status events for BT SAP
08/26/05   sun     Added support for BT SAP
08/07/05   jar     Added AEECARD_PIN_NOT_INIT for PINS when not init
07/22/05   tml     Added SAP support
06/07/05   tml     Added on chip support, PUCT support and Illegal SIM events
06/06/05   tml     Added PROP1 Feature
05/27/05   tml     SIM Lock phone code support
03/17/05   ak      Added CPHS EONS Support
03/16/05   wli     Added 820B EFs
02/28/05   sun     Added files - MWIS/MBDN
01/31/05   tml     Added IModel support
08/30/04   tml     Added Read, Write, FileAttribute APIs
09/01/04   tml     Added file enum
08/19/04   tml     Added ICard_GetServiceAvailableInd API
06/07/04   tml     Added SIM Personalization support
05/18/04   tml     Changed card connection bid
04/16/04   tml     Channel Management Support
12/22/03   tml     API change
11/04/03   tml     Added Pref Slot support
08/20/03   tml     Update operational slot values
07/25/03   tml     Added Dual Slot support
07/18/03   tml     Featurized application bitmap
03/31/03   tml     Added application bitmap support
02/27/03   jar     Added AEECARD_CARD_REINIT to properly handle REFRESH
                   with SIM Init Command Qualifier.
12/11/02   att     Add comments for ICARD_NOTIFIER and AEECARD_NOT_READY
11/02/02   HQ      Use BID files for class IDs.
10/29/02   tml     Add AEECARD_NOT_READY, removed AEECARD_NO_CARD_NV_ONLY
10/03/02   tml     Add AEECARD_NO_CARD_NV_ONLY card status
09/16/02   HQ      Use the real class ID for ICARD_NOTIFIER;
                   header clean-ups.
08/23/02   at      Initial release.
=============================================================================*/


#include "AEE.h"
#include "AEEComdef.h"

/* Class Ids
*/
#include "CARD.BID"
#include "CARD_NOTIFIER.BID"
#include "CARD_MODEL.bid"
#ifdef FEATURE_MMGSDI_CHANNEL_MANAGEMENT
#include "CARDCONNECTION.BID"
#endif /*FAETURE_MMGSDI_CHANNEL_MANAGEMENT */

#define FEATURE_NEW_ICARD
/* remove the feature after icard is mainline in all targets */


#define AEECARD_FILE_ID_LEN 2
#define AEECARD_PUCT_FILE_LEN 5
//*********************************************************************
//
// ICard Interface
//
//*********************************************************************
#define AEECARD_PIN_LEN                8

// Notification Masks...
// These are used when to determine what changed in the status function.
// Users can register for this notification via the INOTIFY interface.
// The below are implemented in the ICARD_INOTIFY class ID.
#define NMASK_CARD_STATUS           0x0001  /* The CARD status has changed. */
#define NMASK_PIN1_STATUS           0x0002  /* The Global PIN1 has changed. */
#define NMASK_PIN2_STATUS           0x0004  /* The Global PIN2 has changed. */
#define NMASK_APP_PIN_STATUS        0x0008  /* An APP Specific PIN has      *
                                             * changed.  The upper 16 bits  *
                                             * specify which PIN.           */
#define NMASK_PIN1_SLOT2_STATUS     0x0010  /* The Global PIN1 for Slot 2 has changed. */
#define NMASK_PIN2_SLOT2_STATUS     0x0020  /* The Global PIN2 for Slot 2 has changed. */
#define NMASK_CARD_SLOT2_STATUS     0x0040  /* The CARD status for Slot 2 has changed. */
#define NMASK_CARD_PERSO_STATUS     0x0080  /* The personalization status for slot 1 */
#define NMASK_CARD_PERSO_SLOT2_STATUS  0x0100  /* The personalization status for slot 2 */
#define NMASK_FILE_READ             0x0200    /* SIM Has read a file */
#define NMASK_BT_PIN1_STATUS        0x0400
#define NMASK_BT_SAP_STATUS         0x0800



// Basic Card States...
#define AEECARD_NO_CARD            0    /* No card is inserted. */
#define AEECARD_INVALID_CARD       1    /* Unusable Card is inserted. */
#define AEECARD_VALID_CARD         2    /* Usable card. */
#define AEECARD_NOT_INIT           3    /* OEMCard is not initialized yet. */
#define AEECARD_NOT_READY          4    /* Hasn't received any notification from MMGSDI */
#define AEECARD_CARD_REINIT        5    /* SIM is reinitializing...keep PIN info as is */
#define AEECARD_AVAIL_CARD         6    /* Card is available but no provisioning info is
                                           coming from the card.  This state is only valid
                                           when there is at least 2 cards in the UE where
                                           the card provides all the provisioning info.
                                           When there is only 1 card in the UE, if no
                                           provisioning info comes from the card, the card
                                           status is going to be AEECARD_NO_CARD */
#define AEECARD_ILLEGAL_CARD             7
#define AEECARD_SAP_CLIENT_CONNECTED     8  /* SAP Connected, the mobile receiving this
                                               event should be acting as Client */
#define AEECARD_SAP_SERVER_CONNECTED     9  /* SAP Connected, the mobile receiving this
                                               event should be acting as Server */
#define AEECARD_SAP_DISCONNECTED        10  /* SAP is disconencted by either client
                                               or server */

// Lock state
#define AEECARD_PIN_UNKNOWN        0
#define AEECARD_PIN_DISABLED       1    /* PIN is not required. */
#define AEECARD_PIN_ENABLED        2    /* PIN is required. */
#define AEECARD_PIN_VERIFIED       3    /* PIN is required. */
#define AEECARD_PIN_BLOCKED        4    /* PIN requires Unlock Key. */
#define AEECARD_PIN_PERM_DISABLED  5    /* PIN cannot be Unblocked. */
#define AEECARD_PIN_NOT_VALID      6    /* PIN functionality not supported. */
#define AEECARD_PIN_NOT_INIT       7    /* PIN Value has not been set       */

// Pin numbers supplied as pin_id.
#define AEECARD_PIN_INVALID   0         /* Invalid PIN ID. */
#define AEECARD_PIN1          1         /* ID for Global PIN 1 */
#define AEECARD_PIN2          2         /* ID for Global PIN 2 */
#define AEECARD_PIN1_SLOT2    3         /* ID for Global PIN 1 for Slot 2 */
#define AEECARD_PIN2_SLOT2    4         /* ID for Global PIN 2 for Slot 2 */
#define AEECARD_PHONE_CODE    5         /* Phone Code */
#define AEECARD_SAP_PIN       6         /* SAP PIN, 16 digits */
#define AEECARD_MAXPIN        6         /* Last PIN we currently support. */


// Perso State
#define AEECARD_PERSO_NO_EVENT                    0
#define AEECARD_PERSO_NETWORK_FAIL                1
#define AEECARD_PERSO_NETWORK_SUBSET_FAIL         2
#define AEECARD_PERSO_SERVICE_PROVIDER_FAIL       3
#define AEECARD_PERSO_CORPORATE_FAIL              4
#define AEECARD_PERSO_SIM_FAIL                    5
#define AEECARD_PERSO_NETWORK_DEACTIVATE          6
#define AEECARD_PERSO_NETWORK_SUBSET_DEACTIVATE   7
#define AEECARD_PERSO_SERVICE_PROVIDER_DEACTIVATE 8
#define AEECARD_PERSO_CORPORATE_DEACTIVATE        9
#define AEECARD_PERSO_SIM_DEACTIVATE              10
#define AEECARD_PERSO_NETWORK_CONTROL_BLOCKED     11
#define AEECARD_PERSO_NETWORK_SUBSET_BLOCKED      12
#define AEECARD_PERSO_SERVICE_PROVIDER_BLOCKED    13
#define AEECARD_PERSO_CORPORATE_BLOCKED           14
#define AEECARD_PERSO_SIM_BLOCKED                 15
#define AEECARD_PERSO_RUIM_NETWORK1_FAIL                16          
#define AEECARD_PERSO_RUIM_NETWORK2_FAIL                17
#define AEECARD_PERSO_RUIM_HRPD_FAIL                    18
#define AEECARD_PERSO_RUIM_SERVICE_PROVIDER_FAIL        19
#define AEECARD_PERSO_RUIM_CORPORATE_FAIL               20
#define AEECARD_PERSO_RUIM_RUIM_FAIL                    21
#define AEECARD_PERSO_RUIM_NETWORK1_DEACTIVATE          22
#define AEECARD_PERSO_RUIM_NETWORK2_DEACTIVATE          23
#define AEECARD_PERSO_RUIM_HRPD_DEACTIVATE              24
#define AEECARD_PERSO_RUIM_SERVICE_PROVIDER_DEACTIVATE  25
#define AEECARD_PERSO_RUIM_CORPORATE_DEACTIVATE         26
#define AEECARD_PERSO_RUIM_RUIM_DEACTIVATE              27
#define AEECARD_PERSO_RUIM_NETWORK1_CONTROL_BLOCKED     28
#define AEECARD_PERSO_RUIM_NETWORK2_CONTROL_BLOCKED     29
#define AEECARD_PERSO_RUIM_HRPD_BLOCKED                 30
#define AEECARD_PERSO_RUIM_SERVICE_PROVIDER_BLOCKED     31
#define AEECARD_PERSO_RUIM_CORPORATE_BLOCKED            32
#define AEECARD_PERSO_RUIM_RUIM_BLOCKED                 33


#define AEECARD_PERSO_FAILED                      34
#define AEECARD_PERSO_PROP1_EVENT                 35
#define AEECARD_PERSO_PROP2_EVENT                 36

// Define the number of PIN attempts remaining
// when we do not have a valid PIN setting.
#define AEECARD_UNKNOWN_TRIES 255

// Application Bitmask
#define AEECARD_NO_APP_MASK    0x00             // There is no application on the card
#define AEECARD_TELECOM_MASK   0x01             // There is a Telecom directory on the card
#define AEECARD_GSM_SIM_MASK   0x02             // There is a GSM application on the card
#define AEECARD_USIM_APP_MASK  0x04             // There is a USIM application on the card
#define AEECARD_RUIM_APP_MASK  0x08             // There is an RUIM application on the card
#define AEECARD_EMV_APP_MASK   0x10             // There is an EMV application on the card

#define AEECARD_GSM_SLOT1_OPERATIONAL     0x01
#define AEECARD_GSM_SLOT2_OPERATIONAL     0x10
#define AEECARD_CDMA_SLOT1_OPERATIONAL    0x02
#define AEECARD_CDMA_SLOT2_OPERATIONAL    0x20
#define AEECARD_UMTS_SLOT1_OPERATIONAL    0x04
#define AEECARD_UMTS_SLOT2_OPERATIONAL    0x40

#define AEECARD_SLOT_NONE                 0
#define AEECARD_SLOT1                     1
#define AEECARD_SLOT2                     2

#define AEECARD_INVALID_APP               0
#define AEECARD_GSM_APP                   1
#define AEECARD_CDMA_APP                  2
#define AEECARD_USIM_APP                  3

/* Preferred language encoding */
#define  AEECARD_LANG_ENCODING_OCTET           0  // Octet, unspecified:
                                                  // 8 bits/char
#define  AEECARD_LANG_ENCODING_EXTPROTMSG   0x01  // Extended Protocal Message
#define  AEECARD_LANG_ENCODING_7BIT         0x02  // 7-bit ASCII: 8 bits/char
#define  AEECARD_LANG_ENCODING_IA5          0x03  // IA5: 7 bits/char
#define  AEECARD_LANG_ENCODING_UNICODE      0x04  // Unicode: 16 bits/char
#define  AEECARD_LANG_ENCODING_SHIFT_JIS    0x05  // Shift-JIS: 8 or 16 bits/char
#define  AEECARD_LANG_ENCODING_KOREAN       0x06  // Korean: 8 or 18 bits/char
#define  AEECARD_LANG_ENCODING_LATINHEBREW  0x07  // Latin/Hebrew: 8 bits/char
#define  AEECARD_LANG_ENCODING_LATIN        0x08  // Latin: 8 bits/char

#define ECMDDISALLOWED  2000
#define ECLSCHANINVALID 2001

/* Personalization feature indicator levels */
#define AEECARD_NW_PERSO_FEATURE                  1
#define AEECARD_NW_SUBSET_PERSO_FEATURE           2
#define AEECARD_SERVICE_PROVIDER_PERSO_FEATURE    3
#define AEECARD_CORPORATE_PERSO_FEATURE           4
#define AEECARD_SIM_USIM_PERSO_FEATURE            5
#define AEECARD_PROP1_PERSO_FEATURE               6
#define AEECARD_RUIM_NW1_PERSO_FEATURE              7
#define AEECARD_RUIM_NW2_PERSO_FEATURE              8
#define AEECARD_RUIM_HRPD_PERSO_FEATURE             9
#define AEECARD_RUIM_SERVICE_PROVIDER_PERSO_FEATURE 10
#define AEECARD_RUIM_CORPORATE_PERSO_FEATURE        11
#define AEECARD_RUIM_RUIM_PERSO_FEATURE             12


#define AEECARD_IMSI_LEN                          9 /* matches GSDI_IMSI_LEN */
typedef enum
{
   AEECARD_NONE = 0x00,

   AEECARD_CHV1_DISABLE,    /* CHV1 disabled function           */
   AEECARD_ADN,             /* GSM Abbreviated Dialling Numbers */
   AEECARD_FDN,             /* Forbidden Dialling Numbers       */
   AEECARD_LND,             /* Last Number Dialled              */
   AEECARD_BDN,             /* Barred Dialing Numbers           */
   AEECARD_LOCAL_PHONEBOOK, /* Local Phonebook, UADF/Phonebook/ADN */
   AEECARD_GLOBAL_PHONEBOOK,/* Global Phonebook on USIM (always present) */
   AEECARD_AOC,             /* Advice of Charge                 */
   AEECARD_OCI_OCT,         /* Outgoing Call information/timer  */
   AEECARD_ICI_ICT,         /* Incoming call information/timer  */

   AEECARD_CCP,             /* Configuration Capability Params  */
   AEECARD_EXT_CCP,         /* extended capability configuration param - in GSM */
   AEECARD_MSISDN,          /* MSISDN                           */
   AEECARD_EXT1,            /* Extension 1                      */
   AEECARD_EXT2,            /* Extension 2                      */
   AEECARD_EXT3,            /* Extension 3                      */
   AEECARD_EXT4,            /* Extension 4                      */
   AEECARD_EXT5,            /* Extension 5                      */
   AEECARD_EXT8,            /* Extension 8                      */
   AEECARD_SDN,             /* Service Dialling Numbers         */

   AEECARD_SMS,             /* Short Message Services           */
   AEECARD_SMSP,            /* SMS Parameters                   */
   AEECARD_SMSR,            /* Short Message Status Reports     */
   AEECARD_GID1,            /* Group Identifier 1               */
   AEECARD_GID2,            /* Group Identifier 2               */
   AEECARD_SPN,             /* Service Provider Name            */
   AEECARD_VGCS_GID_LIST,   /* VGCS Group Identifier List       */
   AEECARD_VBS_GID_LIST,    /* VBS Group Identifier List        */
   AEECARD_ENH_ML_SVC,      /* Enhanced Multi Level precedence and Premption Service */
   AEECARD_AA_EMLPP,        /* Automatic Answer for eMLPP       */

   AEECARD_DATA_DL_SMSCB,   /* Data download via SMS-CB         */
   AEECARD_DATA_DL_SMSPP,   /* Data download via SMS-PP         */
   AEECARD_MENU_SEL,        /* Menu Selection                   */
   AEECARD_CALL_CONTROL,    /* Call Control                     */
   AEECARD_MOSMS_CONTROL,   /* MO SMS control                   */
   AEECARD_PROACTIVE_SIM,   /* Proactive SIM                    */
   AEECARD_USSD,            /* USSD String Data object supported in Call Control */
   AEECARD_RUN_AT_CMD,      /* RUN AT COMMAND command           */
   AEECARD_GPRS_USIM,       /* Call control on GPRS by USIM */
   AEECARD_NETWK_ALTERING_MS, /* Network's Indication of alerting in the MS */

   AEECARD_CBMID_RANGES,    /* Cell Broadcast Message Identifier Ranges */
   AEECARD_CBMI,            /* Cell Broadcast Message Identifier */
   AEECARD_DEPERSON_KEYS,   /* Depersonalization Keys           */
   AEECARD_COOP_NETWK_LIST, /* Co-operative Network List        */
   AEECARD_GPRS,           /* GPRS                              */
   AEECARD_IMAGE,          /* Image (IMG)                       */
   AEECARD_SOLSA,          /* Support of Local Service Area     */
   AEECARD_SP_DISP_INFO,   /* Service Provider Display Information */
   AEECARD_GSM_ACCESS_IN_USIM, /* USIM ADF Access to USIM files for GSM Roaming */
   AEECARD_GSM_SECURITY_CONTEXT, /* GSM Security Context        */

   AEECARD_PLMN_SELECTOR,  /* PLMN Selector                     */
   AEECARD_UPLMN_SEL_WACT, /* User controlled PLMN Selector with Access Technology */
   AEECARD_RPLMN_LACT,     /* RPLMN Last used Access Technology */
   AEECARD_OPLMN_SEL_WACT, /* Operator controlled PLMN Selector with Access Technology */
   AEECARD_HPLMN_WACT,     /* HPLMN Selector with Access Technology */
   AEECARD_OPLMN_LIST,     /* Operator PLMN list                */
   AEECARD_PLMN_NTWRK_NAME,/* PLMN Network Name                 */
   AEECARD_MMS,            /* Multimedia Messaging Service      */
   AEECARD_MMS_USR_P,      /* MMS User Connectivity Parameters  */
   AEECARD_EST,            /* Enable Service Table in USIM      */

   AEECARD_ACL,            /* APN Control list                  */
   AEECARD_CPBCCH,         /* CP BCCH                           */
   AEECARD_INV_SCAN,       /* Investigation Scan                */
   AEECARD_MEXE,           /* MExE info                         */
   AEECARD_MAILBOX_DIAL,   /* Mailbox dialling numbers          */
   AEECARD_MSG_WAIT,       /* Message Wait indication           */
   AEECARD_CALL_FWD_IND    /* Call Forward indication status    */
}
AEECardServiceEnumType;


typedef enum
{
  /* EFs at the MF level */
  AEECARD_ICCID,                  /* ICCID */
  AEECARD_ELP,                    /* Extended Language Preference */
  AEECARD_DIR,                    /* EF DIR for USIM */
  AEECARD_ARR,                    /* Access rule reference */
  /* EFs in CDMA DF*/
  AEECARD_CDMA_CC,                  /* Call Count */
  AEECARD_CDMA_IMSI_M,              /* IMSI_M */
  AEECARD_CDMA_IMSI_T,              /* IMSI_T */
  AEECARD_CDMA_TMSI,                /* TMSI */
  AEECARD_CDMA_ANALOG_HOME_SID,     /* Analog Home SID */
  AEECARD_CDMA_ANALOG_OP_PARAMS,    /* Analog Operational Parameters */

  AEECARD_CDMA_ANALOG_LOCN_AND_REGN_IND,  /* Analog Locn and Regn Indicators */
  AEECARD_CDMA_HOME_SID_NID,        /* CDMA Home SID and NID */
  AEECARD_CDMA_ZONE_BASED_REGN_IND, /* CDMA Zone Based Regn Indicators */
  AEECARD_CDMA_SYS_REGN_IND,        /* CDMA System/Network Regn Indicators */
  AEECARD_CDMA_DIST_BASED_REGN_IND, /* CDMA Distance Based Regn Indicators */
  AEECARD_CDMA_ACCOLC,              /* Access Overload Class */
  AEECARD_CDMA_CALL_TERM_MODE_PREF, /* Call Termination Mode Preferences */
  AEECARD_CDMA_SCI,                 /* Suggested Slot Cycle Index */
  AEECARD_CDMA_ANALOG_CHAN_PREF,    /* Analog Channel Preferences */
  AEECARD_CDMA_PRL,                 /* Preferred Roaming List */

  AEECARD_CDMA_RUIM_ID,             /* Removable UIM ID */
  AEECARD_CDMA_CDMA_SVC_TABLE,      /* CDMA Service Table */
  AEECARD_CDMA_SPC,                 /* Service Programming Code */
  AEECARD_CDMA_OTAPA_SPC_ENABLE,    /* OTAPA/SPC-Enable */
  AEECARD_CDMA_NAM_LOCK,            /* NAM-Lock */
  AEECARD_CDMA_OTASP_OTAPA_FEATURES,/* OTASP-OTAPA Parameters */
  AEECARD_CDMA_SERVICE_PREF,        /* Service Preferences */
  AEECARD_CDMA_ESN_ME,              /* ESN_ME */
  AEECARD_CDMA_RUIM_PHASE,          /* R-UIM Phase */
  AEECARD_CDMA_PREF_LANG,           /* Preferred Languages */

  AEECARD_CDMA_UNASSIGNED_1,        /* Not assigned yet */
  AEECARD_CDMA_SMS,                 /* Short Messages */
  AEECARD_CDMA_SMS_PARAMS,          /* Short Message Service Parameters */
  AEECARD_CDMA_SMS_STATUS,          /* SMS Status */
  AEECARD_CDMA_SUP_SVCS_FEATURE_CODE_TABLE, /* Sup Services Feature Code Table */
  AEECARD_CDMA_UNASSIGNED_2,        /* Not assigned yet */
  AEECARD_CDMA_HOME_SVC_PVDR_NAME,  /* CDMA Home Service Provider Name */
  AEECARD_CDMA_UIM_ID_USAGE_IND,    /* R-UIM ID usage indicator */
  AEECARD_CDMA_ADM_DATA,            /* CDMA Administrative Data */
  AEECARD_CDMA_MSISDN,              /* Mobile Directory Number */

  AEECARD_CDMA_MAXIMUM_PRL,         /* Max size of the PRL */
  AEECARD_CDMA_SPC_STATUS,          /* SPC Status */
  AEECARD_CDMA_ECC,                 /* Emergency Call Codes */
  AEECARD_CDMA_3GPD_ME3GPDOPC,      /* Operational Capabilities */
  AEECARD_CDMA_3GPD_3GPDOPM,        /* Operational Mode */
  AEECARD_CDMA_3GPD_SIPCAP,         /* Simple IP Capabilities */
  AEECARD_CDMA_3GPD_MIPCAP,         /* Mobile IP Capabilities */
  AEECARD_CDMA_3GPD_SIPUPP,         /* Simple IP User Profile Parameters */
  AEECARD_CDMA_3GPD_MIPUPP,         /* Mobile IP User Profile Parameters */
  AEECARD_CDMA_3GPD_SIPSP,          /* Simple IP Status Parameters */

  AEECARD_CDMA_3GPD_MIPSP,          /* Mobile IP Status Parameters */
  AEECARD_CDMA_3GPD_SIPPAPSS,       /* Simple IP PAP SS Parameters */
  AEECARD_CDMA_UNASSIGNED_3,        /* Reserved */
  AEECARD_CDMA_UNASSIGNED_4,        /* Reserved */
  AEECARD_CDMA_PUZL,                /* Preffered User Zone List */
  AEECARD_CDMA_MAXPUZL,             /* Maximum PUZL */
  AEECARD_CDMA_MECRP,               /* ME-specific Config REquest Param */
  AEECARD_CDMA_HRPDCAP,             /* HRPD Access Auth Capability Param */
  AEECARD_CDMA_HRPDUPP,             /* HRPD Access Auth User Profile Param */
  AEECARD_CDMA_CSSPR,               /* CUR_SSPR_P_REV */

  AEECARD_CDMA_ATC,                 /* Acceess Terminal Class */
  AEECARD_CDMA_EPRL,                /* Extended Preffered Roaming List */
  AEECARD_CDMA_BCSMS_CONFIG,        /* Broadcast SMS Configuration */
  AEECARD_CDMA_BCSMS_PREF,          /* Broadcast SMS Preferences */
  AEECARD_CDMA_BCSMS_TABLE,         /* Broadcast SMS Table */
  AEECARD_CDMA_BCSMS_PARAMS,        /* Broadcast SMS Paramaters */
  AEECARD_CDMA_MMS_NOTIF,           /* MMS Notification */
  AEECARD_CDMA_MMS_EXT8,            /* MMS Extension 8 */
  AEECARD_CDMA_MMS_ICP,             /* MMS Issuer Connectivity Parameters */
  AEECARD_CDMA_MMS_UP,              /* MMS User Preferences */

  AEECARD_CDMA_SMS_CAP,             /* SMS Capabilities */
  AEECARD_CDMA_3GPD_IPV6CAP,        /* IPv6 Capabilities */
  AEECARD_CDMA_3GPD_MIPFLAGS,       /* Mobile IP Flags */
  AEECARD_CDMA_3GPD_TCPCONFIG,      /* TCP Configurations */
  AEECARD_CDMA_3GPD_DGC,            /* Data Generic Configurations */
  AEECARD_CDMA_BROWSER_CP,          /* Browser Connectivity Parameters */
  AEECARD_CDMA_BROWSER_BM,          /* Browser Bookmarks */
  AEECARD_CDMA_3GPD_SIPUPPEXT,      /* Simple IP User Profile Parameters Extension */
  AEECARD_CDMA_MMS_CONFIG,          /* MMS Configuration */
  AEECARD_CDMA_JAVA_DURL,           /* Java Download URL */

  AEECARD_CDMA_3GPD_MIPUPPEXT,      /* Mobile IP User Profile Parameters Extension */
  AEECARD_CDMA_BREW_DLOAD,          /* BREW Download */
  AEECARD_CDMA_BREW_TSID,           /* BREW Teleservice ID */
  AEECARD_CDMA_BREW_SID,            /* BREW Subscriber ID */
  AEECARD_CDMA_LBS_XCONFIG,         /* LBS XTRA Configuration */
  AEECARD_CDMA_LBS_XSURL,           /* LBS XTRA Server URLs */
  AEECARD_CDMA_LBS_V2CONFIG,        /* LBS V2 Configuration */
  AEECARD_CDMA_LBS_V2PDEADDR,       /* LBS V2 PDE Address */
  AEECARD_CDMA_LBS_V2MPCADDR,       /* LBS V2 MPC Address */
  AEECARD_CDMA_BREW_AEP,            /* BREW Application Execution Policy */

  AEECARD_CDMA_MODEL,               /* Phone Model Info */
  AEECARD_CDMA_RC,                  /* Root Certificate */
  AEECARD_CDMA_APP_LABELS,          /* Application Labels */
  AEECARD_CDMA_USER_AGENT_STRING,   /* KDDI Agent String */
  AEECARD_CDMA_GID2,                /* KDDI GID2 */
  AEECARD_CDMA_GID1,                /* KDDI GID1 */
  AEECARD_CDMA_GID_REFERENCES,      /* KDDI GID References */
  AEECARD_CDMA_IMSI_STATUS,         /* KDDI IMSI Status */
  AEECARD_CDMA_ME_DOWNLOADABLE_DATA, /* KDDI ME Downloadable Data */
  AEECARD_CDMA_ME_SETTING_DATA,     /* KDDI ME Setting Date */

  AEECARD_CDMA_ME_USER_DATA,        /* KDDI User Data */
  AEECARD_CDMA_RESERVED2,           /* KDDI Reserved */
  AEECARD_CDMA_RESERVED1,           /* KDDI Reserved */
  AEECARD_CDMA_UIM_SVC_TABLE,       /* KDDI UIM Service Table */
  AEECARD_CDMA_UIM_FEATURES,        /* KDDI UIM Features */
  AEECARD_CDMA_UIM_VERSION,         /* KDDI UIM Version */
  AEECARD_CDMA_HRPD_HRPDUPP,        /* HRPD User Profile Parameters */
  AEECARD_CDMA_SF_EUIM_ID,          /* Short Form EUIMID */
  /* EFs in DF GSM */
  AEECARD_GSM_LP,              /* Language Preference */
  AEECARD_GSM_IMSI,            /* IMSI */

  AEECARD_GSM_KC,              /* Ciphering Key Kc */
  AEECARD_GSM_PLMN,            /* PLMN selector */
  AEECARD_GSM_HPLMN,           /* HPLMN search period */
  AEECARD_GSM_ACM_MAX,         /* ACM Maximum value */
  AEECARD_GSM_SST,             /* SIM Service table */
  AEECARD_GSM_ACM,             /* Accumulated call meter */
  AEECARD_GSM_GID1,            /* Group Identifier Level 1 */
  AEECARD_GSM_GID2,            /* Group Identifier Level 2 */
  AEECARD_GSM_SPN,             /* Service Provider Name */
  AEECARD_GSM_PUCT,            /* Price Per Unit and currency table */

  AEECARD_GSM_CBMI,            /* Cell broadcast message identifier selection */
  AEECARD_GSM_BCCH,            /* Broadcast control channels */
  AEECARD_GSM_ACC,             /* Access control class */
  AEECARD_GSM_FPLMN,           /* Forbidden PLMNs */
  AEECARD_GSM_LOCI,            /* Location information */
  AEECARD_GSM_AD,              /* Administrative data */
  AEECARD_GSM_PHASE,           /* Phase identification */
  AEECARD_GSM_VGCS,            /* Voice Group Call service */
  AEECARD_GSM_VGCSS,           /* Voice Group Call service status */
  AEECARD_GSM_VBS,             /* Voice Broadcast service  */

  AEECARD_GSM_VBSS,            /* Voice Broadcast service status */
  AEECARD_GSM_EMLPP,           /* Enhanced multi level pre-emption and priority */
  AEECARD_GSM_AAEM,            /* Automatic Answer for eMLPP service */
  AEECARD_GSM_CBMID,           /* Cell Broadcast Message id for data dload */
  AEECARD_GSM_ECC,             /* Emergency Call Codes */
  AEECARD_GSM_CBMIR,           /* Cell Broadcast Message id range selection */
  AEECARD_GSM_DCK,             /* De-personalization control keys */
  AEECARD_GSM_CNL,             /* Co-operative network list */
  AEECARD_GSM_NIA,             /* Network's indication of alerting */
  AEECARD_GSM_KCGPRS,          /* GPRS ciphering key */

  AEECARD_GSM_LOCIGPRS,        /* GPRS location information */
  AEECARD_GSM_SUME,            /* Setup Menu elements */
  AEECARD_GSM_PLMNWACT,        /* PLMN Selector with Access technology */
  AEECARD_GSM_OPLMNWACT,       /* Operator controlled PLMNWACT */
  AEECARD_GSM_HPLMNACT,        /* HPLNMN Access technology */
  AEECARD_GSM_CPBCCH,          /* CPBCCH information */
  AEECARD_GSM_INVSCAN,         /* Investigation PLMN Scan */
  AEECARD_GSM_RPLMNAT,         /* RPLMN  Last used Access Technology */
  AEECARD_GSM_VMWI,            /* CPHS: Voice Mail Waiting Indicator */
  AEECARD_GSM_SVC_STR_TBL,     /* CPHS: Service String Table */

  AEECARD_GSM_CFF,             /* CPHS: Call Forwarding Flag */
  AEECARD_GSM_ONS,             /* CPHS: Operator Name String */
  AEECARD_GSM_CSP,             /* CPHS: Customer Service Profile */
  AEECARD_GSM_CPHSI,           /* CPHS: CPHS Information */
  AEECARD_GSM_MN,              /* CPHS: Mailbox Number */
  AEECARD_GSM_ORANGE_DFS,      /* Dynamic Flags Status */
  AEECARD_GSM_ORANGE_D2FS,     /* Dynamic2 Flag Setting */
  AEECARD_GSM_ORANGE_CSP2,     /* Customer Service Profile Line2*/
  /* EFs at SoLSA */
  AEECARD_GSM_SAI,             /* SoLSA access indicator */
  AEECARD_GSM_SLL,             /* SoLSA LSA list */

  /* EFs at MExE Level */
  AEECARD_GSM_MEXE_ST,         /* MExE Service table */
  AEECARD_GSM_ORPK,            /* Operator Root Public Key */
  AEECARD_GSM_ARPK,            /* Administrator Root Public Key */
  AEECARD_GSM_TPRPK,           /* Third party Root public key */
  AEECARD_GSM_MBDN,            /* Mailbox Dialing Number*/
  AEECARD_GSM_EXT6,            /* Extension 6*/
  AEECARD_GSM_MBI,             /* Mail Box Identifier*/
  AEECARD_GSM_MWIS,            /* Message Waiting Indication Status*/
  AEECARD_GSM_EXT1,            /* Extension 1*/
  AEECARD_GSM_SPDI,            /* Service Provider Display Information*/
  
  AEECARD_GSM_CFIS,            /* GSM: Call Forwarding Indication status */
  /* Image file */
  AEECARD_GSM_IMG,
  AEECARD_GSM_IMG1INST1,
  AEECARD_GSM_IMG1INST2,
  AEECARD_GSM_IMG1INST3,
  AEECARD_GSM_IMG2INST1,
  AEECARD_GSM_IMG2INST2,
  AEECARD_GSM_IMG2INST3,
  AEECARD_GSM_IMG3INST1,
  AEECARD_GSM_IMG3INST2,

  AEECARD_GSM_IMG3INST3,
  /* EFs at the telecom DF in GSM and CDMA */
  AEECARD_TELECOM_ADN,         /* Abbrev Dialing Numbers */
  AEECARD_TELECOM_FDN,         /* Fixed dialling numbers */
  AEECARD_TELECOM_SMS,         /* Short messages */
  AEECARD_TELECOM_CCP,         /* Capability Configuration Parameters */
  AEECARD_TELECOM_ECCP,        /* Extended CCP */
  AEECARD_TELECOM_MSISDN,      /* MSISDN */
  AEECARD_TELECOM_SMSP,        /* SMS parameters */
  AEECARD_TELECOM_SMSS,        /* SMS Status */
  AEECARD_TELECOM_LND,         /* Last number dialled */

  AEECARD_TELECOM_SDN,         /* Service Dialling numbers */
  AEECARD_TELECOM_EXT1,        /* Extension 1 */
  AEECARD_TELECOM_EXT2,        /* Extension 2 */
  AEECARD_TELECOM_EXT3,        /* Extension 3 */
  AEECARD_TELECOM_BDN,         /* Barred Dialing Numbers */
  AEECARD_TELECOM_EXT4,        /* Extension 4 */
  AEECARD_TELECOM_SMSR,        /* SMS reports */
  AEECARD_TELECOM_CMI,         /* Comparison Method Information */
  AEECARD_TELECOM_SUME,        /* Setup Menu elements */
  AEECARD_TELECOM_ARR,         /* Access Rule reference */

  /* EFs at DF GRAPHICS under DF Telecom */
  AEECARD_IMAGE_FILE,          /* Image instance data files */
  /* EFs at the DF PHONEBOOK under  DF Telecom */
  AEECARD_TELECOM_PBR,         /* Phone book reference file */
  AEECARD_TELECOM_CCP1,        /* Capability Configuration parameters 1 */
  AEECARD_TELECOM_UID,         /* Unique Identifier */
  AEECARD_TELECOM_PSC,         /* Phone book synchronization center */
  AEECARD_TELECOM_CC,          /* Change counter */
  AEECARD_TELECOM_PUID,        /* Previous Unique Identifier */
  /* EFs of USIM ADF */
  AEECARD_USIM_LI,             /* Language Indication */
  AEECARD_USIM_IMSI,           /* IMSI */
  AEECARD_USIM_KEYS,           /* Ciphering and Integrity keys */

  AEECARD_USIM_KEYSPS,         /* C and I keys for packet switched domain */
  AEECARD_USIM_PLMNWACT,       /* User controlled PLMN selector with access tech */
  AEECARD_USIM_UPLMNSEL,       /* UPLMN selector */
  AEECARD_USIM_HPLMN,          /* HPLMN search period */
  AEECARD_USIM_ACM_MAX,        /* ACM maximum value */
  AEECARD_USIM_UST,            /* USIM Service table */
  AEECARD_USIM_ACM,            /* Accumulated Call meter */
  AEECARD_USIM_GID1,           /* Group Identifier Level  */
  AEECARD_USIM_GID2,           /* Group Identifier Level 2 */
  AEECARD_USIM_SPN,            /* Service Provider Name */

  AEECARD_USIM_PUCT,           /* Price Per Unit and Currency table */
  AEECARD_USIM_CBMI,           /* Cell Broadcast Message identifier selection */
  AEECARD_USIM_ACC,            /* Access control class */
  AEECARD_USIM_FPLMN,          /* Forbidden PLMNs */
  AEECARD_USIM_LOCI,           /* Location information */
  AEECARD_USIM_AD,             /* Administrative data */
  AEECARD_USIM_CBMID,          /* Cell Broadcast msg identifier for data download */
  AEECARD_USIM_ECC,            /* Emergency call codes */
  AEECARD_USIM_CBMIR,          /* Cell broadcast msg identifier range selection */
  AEECARD_USIM_PSLOCI,         /* Packet switched location information */

  AEECARD_USIM_FDN,            /* Fixed dialling numbers */
  AEECARD_USIM_SMS,            /* Short messages */
  AEECARD_USIM_MSISDN,         /* MSISDN */
  AEECARD_USIM_SMSP,           /* SMS parameters */
  AEECARD_USIM_SMSS,           /* SMS Status */
  AEECARD_USIM_SDN,            /* Service dialling numbers */
  AEECARD_USIM_EXT2,           /* Extension 2 */
  AEECARD_USIM_EXT3,           /* Extension 3 */
  AEECARD_USIM_SMSR,           /* SMS reports */
  AEECARD_USIM_ICI,            /* Incoming call information */

  AEECARD_USIM_OCI,            /* Outgoing call information */
  AEECARD_USIM_ICT,            /* Incoming call timer */
  AEECARD_USIM_OCT,            /* Outgoing call timer */
  AEECARD_USIM_EXT5,           /* Extension 5 */
  AEECARD_USIM_CCP2,           /* Capability Configuration Parameters 2 */
  AEECARD_USIM_EMLPP,          /* Enhanced Multi Level Precedence and Priority */
  AEECARD_USIM_AAEM,           /* Automatic answer for eMLPP service */
  AEECARD_USIM_GMSI,           /* Group identity */
  AEECARD_USIM_HIDDENKEY,      /* key for hidden phonebook entries */
  AEECARD_USIM_BDN,            /* Barred dialling numbers */

  AEECARD_USIM_EXT4,           /* Extension 4 */
  AEECARD_USIM_CMI,            /* Comparison Method information */
  AEECARD_USIM_EST,            /* Enabled services table */
  AEECARD_USIM_ACL,            /* Access Point Name Control List */
  AEECARD_USIM_DCK,            /* De-personalization Control Keys */
  AEECARD_USIM_CNL,            /* Co-operative network list */
  AEECARD_USIM_START_HFN,      /* Init values for Hyper-frame number */
  AEECARD_USIM_THRESHOLD,      /* Max value of START */
  AEECARD_USIM_OPLMNWACT,      /* Operator controlled PLMN sel with access tech */
  AEECARD_USIM_OPLMNSEL,       /* OPLMN selector */

  AEECARD_USIM_HPLMNWACT,      /* HPLMN selector with access tech */
  AEECARD_USIM_ARR,            /* Access Rule reference */
  AEECARD_USIM_RPLMNACT,       /* RPLMN last used access tech */
  AEECARD_USIM_NETPAR,         /* Network parameters */
  AEECARD_USIM_VMWI,           /* CPHS: Voice Mail Waiting Indicator */
  AEECARD_USIM_SVC_STR_TBL,    /* CPHS: Service String Table */
  AEECARD_USIM_CFF,            /* CPHS: Call Forwarding Flag */
  AEECARD_USIM_ONS,            /* CPHS: Operator Name String */
  AEECARD_USIM_CSP,            /* CPHS: Customer Service Profile */
  AEECARD_USIM_CPHSI,          /* CPHS: CPHS Information */

  AEECARD_USIM_MN,             /* CPHS: Mailbox Number */
  /* EFs of ORANGE DF under USIM ADF */
  AEECARD_USIM_ORANGE_DFS,     /* Dynamic Flags Status */
  AEECARD_USIM_ORANGE_D2FS,    /* Dynamic2 Flag Setting */
  AEECARD_USIM_ORANGE_CSP2,    /* Customer Service Profile Line2*/
  /*EFs at DF PHONEBOOK under USIM ADF */
  AEECARD_USIM_PBR,            /* Phone book reference file */
  AEECARD_USIM_CCP1,           /* Capability Configuration parameters 1 */
  AEECARD_USIM_UID,            /* Unique Identifier */
  AEECARD_USIM_PSC,            /* Phone book synchronization center */
  AEECARD_USIM_CC,             /* Change counter */
  AEECARD_USIM_PUID,           /* Previous Unique Identifier */

  /*EFs at DF GSM under USIM ADF */
  AEECARD_USIM_KC,             /* GSM ciphering key Kc */
  AEECARD_USIM_KCGPRS,         /* GPRS ciphering key */
  AEECARD_USIM_CPBCCH,         /* CPBCCH information */
  AEECARD_USIM_INVSCAN,        /* Investigation scan */
  /*EFs at DF MEXe under USIM ADF */
  AEECARD_USIM_MEXE_ST,        /* MExE Service table */
  AEECARD_USIM_ORPK,           /* Operator Root Public Key */
  AEECARD_USIM_ARPK,           /* Administrator Root Public Key */
  AEECARD_USIM_TPRPK,          /* Third party Root public key */
  AEECARD_USIM_MBDN,           /* Mailbox Dialing Number*/
  AEECARD_USIM_EXT6,           /* Extension 6 */

  AEECARD_USIM_MBI,           /* Mailbox Identifier*/
  AEECARD_USIM_MWIS,           /* Message Waiting Indication Status*/
  AEECARD_USIM_SPDI,           /* Service Provider Display Information*/
  AEECARD_USIM_EHPLMN,         /* Equivalent HPLMN */
  /* EFs of DCS1800 DF */
  AEECARD_DCS1800_IMSI,        /* Imsi */
  AEECARD_DCS1800_KC,          /* Ciphering Key Kc */
  AEECARD_DCS1800_PLMN,        /* PLMN selector */
  AEECARD_DCS1800_SST,         /* SIM Service table */
  AEECARD_DCS1800_BCCH,        /* Broadcast control channels */
  AEECARD_DCS1800_ACC,         /* Access control class */

  AEECARD_DCS1800_FPLMN,       /* Forbidden PLMNs */
  AEECARD_DCS1800_LOCI,        /* Location information */
  AEECARD_DCS1800_AD,          /* Administrative data */
  /* EFs selected by path at the telecom DF in GSM and CDMA */
  AEECARD_TELECOM_IAP,         /* Index Administration File */
  AEECARD_TELECOM_PBC,         /* Phone Book Control */
  AEECARD_TELECOM_GRP,         /* Grouping File */
  AEECARD_TELECOM_AAS,         /* Additional Number Alpha String */
  AEECARD_TELECOM_GAS,         /* Grouping Information Alpha String */
  AEECARD_TELECOM_ANR,         /* Additional Number */
  AEECARD_TELECOM_SNE,         /* Secondary Name Entry */

  AEECARD_TELECOM_EMAIL,       /* Email */
  AEECARD_TELECOM_ANRA,        /* Additional Number A */
  AEECARD_TELECOM_ANRB,        /* Additional Number B */
  AEECARD_TELECOM_ANR1,        /* Additional Number C */
  AEECARD_TELECOM_ANRA1,       /* Additional Number A1 */
  AEECARD_TELECOM_ANRB1,       /* Additional Number B1 */
  AEECARD_TELECOM_ADN1,        /* Abbreviated Dialing Numbers 1 */
  AEECARD_TELECOM_PBC1,        /* Phone Book Control 1 */
  AEECARD_TELECOM_GRP1,        /* Grouping File 1 */
  AEECARD_TELECOM_SNE1,        /* Secondary Name Entry 1 */

  AEECARD_TELECOM_UID1,        /* Unique Identifier 1 */
  AEECARD_TELECOM_EMAIL1,      /* Email 1 */
  AEECARD_TELECOM_IAP1,        /* Index Administration File 1 */
  /* EFs selected by path of USIM ADF */
  AEECARD_USIM_ADN,            /* Abbreviated Dialing Numbers */
  AEECARD_USIM_IAP,            /* Index Administration File */
  AEECARD_USIM_PBC,            /* Phone Book Control */
  AEECARD_USIM_EXT1,           /* Extension 1 */
  AEECARD_USIM_GRP,            /* Grouping File */
  AEECARD_USIM_AAS,            /* Additional Number Alpha String */
  AEECARD_USIM_AAS1,           /* Additional Number Alpha String 1 */

  AEECARD_USIM_GAS,            /* Grouping Information Alpha String */
  AEECARD_USIM_GAS1,           /* Grouping Information Alpha String */
  AEECARD_USIM_ANR,            /* Additional Number */
  AEECARD_USIM_SNE,            /* Secondary Name Entry */
  AEECARD_USIM_EMAIL,          /* Email */
  AEECARD_USIM_ANRA,           /* Additional Number A */
  AEECARD_USIM_ANRB,           /* Additional Number B */
  AEECARD_USIM_ANRC,           /* Additional Number C */
  AEECARD_USIM_ANR1,           /* Additional Number 1 */
  AEECARD_USIM_ANRA1,          /* Additional Number A1 */

  AEECARD_USIM_ANRB1,          /* Additional Number B1 */
  AEECARD_USIM_ANRC1,          /* Additional Number C1 */
  AEECARD_USIM_ADN1,           /* Abbreviated Dialing Numbers 1 */
  AEECARD_USIM_PBC1,           /* Phone Book Control 1 */
  AEECARD_USIM_GRP1,           /* Grouping File 1 */
  AEECARD_USIM_SNE1,           /* Secondary Name Entry 1 */
  AEECARD_USIM_UID1,           /* Unique Identifier 1 */
  AEECARD_USIM_EMAIL1,         /* Email 1 */
  AEECARD_USIM_IAP1,           /* Index Administration File 1 */
  /* EFs of WIM DF */
  AEECARD_WIM_ODF,             /* WIM ODF */

  AEECARD_WIM_TOKEN_INFO,      /* Generic token info and capabilities */
  AEECARD_WIM_UNUSED_SPACE,    /* Unused Space */
  /* EFs of ORANGE DF */
  AEECARD_ORANGE_DFS,          /* Dynamic Flags Status */
  AEECARD_ORANGE_D2FS,         /* Dynamic2 Flag Setting */
  AEECARD_ORANGE_CSP2,         /* Customer Service Profile Line2*/
  AEECARD_ORANGE_PARAMS,       /* Welcome Message*/
  AEECARD_GSM_CINGULAR_ACT_HPLMN,/*Acting HPLMN*/
  AEECARD_GSM_CINGULAR_SPT_TABLE,/*Support Table*/
  AEECARD_GSM_PNN,             /* PLMN Network Name */
  AEECARD_GSM_OPL,             /* Operator List */

  AEECARD_USIM_PNN,            /* CPHS: PLMN Network Name */
  AEECARD_USIM_OPL,            /* CPHS: Operator List */
  AEECARD_USIM_CFIS,           /* Call Forwarding Indication status */
  AEECARD_NO_SUCH_ITEM,        /* No such item */
  AEECARD_FILE_ITEM_MAX  = 0xFFFF
}AEECardFileItemEnumType;


/* This structure contains Status Word returned from the card */
typedef struct {
  uint8   sw1;
  uint8   sw2;
}
AEECardStatusWord;

typedef struct
{
  boolean bGSMSvcOn;
  boolean bCDMASvcOn;
  boolean bUMTSSvcOn;
}
AEECardServiceIndType;

/* This structure is used to define the status of a PIN.  */
typedef struct
{
  uint8 lock_state;        /* Lock state of this PIN */
  uint8 tries_remaining;   /* Number of unlock tries remaining. */
} AEECardPinStatus;

/* This structure is used to define the PIN code being sent.  */
typedef struct
{
  uint8    code_len;       /* Number of characters in the PIN */
  AECHAR  *code;           /* The string of characters making the PIN */
} AEECardPinCode;


/* When a PIN command has finished execution, a call back function
 * is invoked.  The following structure is passed to the callback
 * function indicating the results of the command.
 */
typedef struct
{
  int nCmdStatus;              /* SUCCESS/EFAILED/etc */
  int nPinID;                  /* PIN ID for the action that is completed*/
  AEECardStatusWord  sStatus;  /* Status word returned from the Card */
}
AEECardPinCmdStatus;

typedef struct
{
  int nCmdStatus;              /* SUCCESS/EFAILED/etc */
  int nSlotID;                 /* Slot ID for the action that is completed*/
  AEECardFileItemEnumType fileName;
  uint32  dwData;
}
AEECardGenericCmdStatus;

typedef struct {
   int    nSize;
   uint8  *pATR;
}
AEECardATR;

/* This structure contains application, operatonal slot
 * and general slot information */
typedef struct {
  boolean slot1_present;        /* TRUE Slot has Card Inserted */
  uint8   apps1_enabled;        /* BITMAP Of Apps Available    */
  boolean slot2_present;        /* TRUE Slot has Card Inserted */
  uint8   apps2_enabled;        /* BITMAP Of Apps Available    */
  uint8   operational_slot; /* Bits [0-1] -> Slot Number for GSM App */
                                            /* Bits [2-3] -> Slot Number for CDMA App */
  boolean onchip_card_activated;
} AEECardApp;

/* This structure contains information on FDN, BDN, ACL and IMSI status */
typedef struct {
  boolean  fdn_enabled;     /* Is FDN enabled? */
  boolean  bdn_enabled;     /* Is BDN enabled? */
  boolean  acl_enabled;     /* Is ACL enabled? */
  boolean  imsi_invalidated;/* Is IMSI invalidate? */
  boolean  unknown_capabilities; /* Unknown capability */
} AEESimCapabilities;


/* Forward typedef reference of the objects. */
typedef struct ICardConnection ICardConnection;

/* This structure contains APDU response from the card */
typedef struct {
   int    nSize;               /* nSize equals the size of the
                                   response data + 2 (sw1 and sw2) */
   uint8 *pAPDUData;           /* sw1 and sw2 will be the last
                                   2 bytes of the pAPDUData */
}
AEECardAPDUResp;

/* This structure is the command status data structure that ICardConnection
    will populate upon processing the Send command */
typedef struct
{
  ICardConnection *pCardConnection;
  int nCmdStatus;              /* SUCCESS if there are data returned
                                   by the card */
  uint32  dwClientData;        /* Data passed in by the client if
                                   exists */
  int     nSize;               /* Response APDU Data */
  uint32  dwSeqNum;
}
AEECardConnectionCmdStatus;

/* This structure is the command status data structure that ICardConnection
    will populate upon processing the Open command */
typedef struct
{
  ICardConnection *pCardConnection;
  int nCmdStatus;              /* Success if the card is able to
                                   process the request */
  uint8 wSlotID;               /* Slot ID for the open channel
                                    AEECARD_SLOT1
                                    AEECARD_SLOT2 */
  AEECardStatusWord sw;        /* status word, application is
                                  required to check the status word
                                  to determine if the Open command
                                  is successful or not */
}
AEECardConnectionOpenStatus;

/* This structure is the command status data structure that ICardConnection
    will populate upon processing the Close command */
typedef struct
{
  ICardConnection *pCardConnection;
  int nCmdStatus;              /* Success if the card is able to
                                   process the request */
  AEECardStatusWord sw;        /* status word, application is
                                   required to check the status word
                                   to determine if the Close command
                                   is successful or not */
}
AEECardConnectionCloseStatus;

/* This structure contains APDU Cmd to be sent to the card */
typedef struct
{
  int    nSize;                /* Size of the APDU command */
  uint8 *pAPDUData;            /* Pointer to APDU command
                                   The expected APDU format is
                                   defined as in ISO 7816 Part 4:
                                   Cla Ins P1 P2 [lc] [Data] [le] */
  uint32  dwClientData;        /* Data passed in by the client if
                                   exists */
}
AEECardAPDUCmd;


typedef struct
{
  int   nPersoFeatureInd;
  int   nSize;
  AECHAR *pPersoData;
}
AEECardPersoFeatureInfo;


typedef struct
{
  int   nCmdStatus;              /* SUCCESS/EFAILED/etc */
  int   nSlotID;                 /* Slot ID for the action that is completed*/
  int   nPersoFeatureID;         /* Personalization Feature Indicator*/
  uint8 byRetries;               /* Number of retries */
}
AEECardDeactivatePersoCmdStatus;


typedef struct
{
  int               nCmdStatus;
  int               nSlotID;       /* Slot ID for the action that is completed*/
  boolean           bNWIndStatus;  /* Network Indicator Status  */
  boolean           bNSIndStatus;  /* NW Subset Indicator Status*/
  boolean           bSPIndStatus;  /* Servic Provider Ind Status*/
  boolean           bCPIndStatus;  /* Corporate Ind Status      */
  boolean           bSIMIndStatus; /* SIM Indicator Status      */
  boolean           bRUIMNW1IndStatus;  /* Network1 Indicator Status  */
  boolean           bRUIMNW2IndStatus;  /* Network2 Indicator Status  */
  boolean           bRUIMHRPDIndStatus;  /* HRPD Indicator Status*/
  boolean           bRUIMSPIndStatus;  /* Servic Provider Ind Status*/
  boolean           bRUIMCPIndStatus;  /* Corporate Ind Status      */
  boolean           bRUIMRUIMIndStatus; /* RUIM Indicator Status      */


  boolean           bProp1IndStatus; /* Propertory Indicator Status */
}
AEECardGetPersoFeatureCmdStatus;

/* This enum is used to indicate if the file information provided is in
** the AEECardFileItemEnumType format or raw file path format as specified
** in spec 31.102, 11.11, IS820 */
typedef enum {
  AEECARD_FILE_NONE,
  AEECARD_FILE_NAME,
  AEECARD_FILE_PATH,
  AEECARD_FILE_IND_MAX = 0xFFFF
}
AEECardFileIndEnumType;

/* This structure indicates how ICard should interpret the
** File parameter in the calling function */
typedef struct {
  uint32                  dwStructureSize;   /* Indicate the size of the structure */
  AEECardFileIndEnumType  nFileIndication;   /* Is the user passing in file enum name or
                                                by path */
  AEECardFileItemEnumType nFileName;         /* file name when
                                                nfileIndication == AEECARD_FILE_NAME */
  uint32                  nAIDLen;           /* length of Application ID pointer */
  uint8                   *pAID;             /* application pointer in hex value */
                                             /* Applicable to USIM only.
                                                When nAIDLen == 0 and pAID == NULL, the
                                                application ID is default to USIM application */
  uint32                  nPathLen;          /* length of path */
  uint8                   *pPath;            /* Path location per spec 31.102,11.11, IS820 */
}
AEECardFilePathType;


/* This structure is used to pass data between ICard and the application
** a) if (pDataLen == NULL)
**      { function call will return error }
** b) if (pData == NULL)
**      { function call will return size of data available for copying }
** c) if (pData != NULL)
**      { ICard will copy *pDataLen amount of data to the pData buffer }
**     if ICard has less than *pDataLen amount of data, it will copy the
**     minumum number of bytes into the pData buffer
**     *pDataLen upon return of a function will reflect the exact number
**     of bytes that are being copied to the application
*/
typedef struct {
  int    *DataLen;
  uint8  *pData;
}
AEECardDataBufType;


/* This enum indicates if the file is MF, DF or EF */
typedef enum {
  AEECARD_FTYPE_MASTER_FILE,
  AEECARD_FTYPE_DEDICATED_FILE,
  AEECARD_FTYPE_ELEMENTARY_FILE,
  AEECARD_FTYPE_MAX = 0xFFFF
}
AEECardFileType;

/* This enum indicates the EF structure */
typedef enum {
  AEECARD_FSTRUCTURE_TRANSPARENT,
  AEECARD_FSTRUCTURE_LINEAR,
  AEECARD_FSTRUCTURE_CYCLIC,
  AEECARD_FSTRUCTURE_MAX = 0xFFFF
}
AEECardFileStructureType;


typedef struct {
  uint32     dwStructureSize;    /* Indicate the size of the structure */
  flg        nAnd:1;             /* Indicate if the following access key condition is an "AND" or "OR" operation */
  flg        nAlways:1;          /* Access is always */
  flg        nApp1OrGSMPin1:1;   /* Access is allowed after application 1 or GSM Pin1 verification */
  flg        nApp2Pin1:1;        /* Access is allowed after application 2 Pin1 verification */
  flg        nApp3Pin1:1;        /* Access is allowed after application 3 Pin1 verification */
  flg        nApp4Pin1:1;        /* Access is allowed after application 4 Pin1 verification */
  flg        nApp5Pin1:1;        /* Access is allowed after application 5 Pin1 verification */
  flg        nApp6Pin1:1;        /* Access is allowed after application 6 Pin1 verification */
  flg        nApp7Pin1:1;        /* Access is allowed after application 7 Pin1 verification */
  flg        nApp8Pin1:1;        /* Access is allowed after application 8 Pin1 verification */
  flg        nApp1OrGSMPin2:1;   /* Access is allowed after application 1 or GSM Pin2 verification */
  flg        nApp2Pin2:1;        /* Access is allowed after application 2 Pin2 verification */
  flg        nApp3Pin2:1;        /* Access is allowed after application 3 Pin2 verification */
  flg        nApp4Pin2:1;        /* Access is allowed after application 4 Pin2 verification */
  flg        nApp5Pin2:1;        /* Access is allowed after application 5 Pin2 verification */
  flg        nApp6Pin2:1;        /* Access is allowed after application 6 Pin2 verification */
  flg        nApp7Pin2:1;        /* Access is allowed after application 7 Pin2 verification */
  flg        nApp8Pin2:1;        /* Access is allowed after application 8 Pin2 verification */
  flg        nPinUniversal:1;    /* Access is allowed after universal Pin verification */
  flg        nADM1:1;            /* Access is allowed after ADM1 verification */
  flg        nADM2:1;            /* Access is allowed after ADM2 verification */
  flg        nADM3:1;            /* Access is allowed after ADM3 verification */
  flg        nADM4:1;            /* Access is allowed after ADM4 verification */
  flg        nADM5:1;            /* Access is allowed after ADM5 verification */
  flg        nADM6:1;            /* Access is allowed after ADM6 verification */
  flg        nADM7:1;            /* Access is allowed after ADM7 verification */
  flg        nADM8:1;            /* Access is allowed after ADM8 verification */
  flg        nADM9:1;            /* Access is allowed after ADM9 verification */
  flg        nADM10:1;           /* Access is allowed after ADM10 verification */
  flg        nADM11:1;           /* Access is allowed after ADM11 verification */
  flg        nNever:1;           /* Access is never allowed */
}
AEECardAccessConditionType;

/* This structure indicates protection indication of the various
** access condition.  The value of each condition is defined
** per 102.221, 11.11, IS820
 */
typedef struct {
  AEECardAccessConditionType     sRead;         /* Access condition for Read operation */
  AEECardAccessConditionType     sWrite;        /* Access condition for Write operation */
  AEECardAccessConditionType     sIncrease;     /* Access condition for Increase operation */
  AEECardAccessConditionType     sInvalidate;   /* Access condition for Invalidate/Deactivate operation */
  AEECardAccessConditionType     sRehabilitate; /* Access condition for Rehabilitate/Activate operation */
}
AEECardFileProtectionType;


/* This structure indicates the Get File Attribute Response
** nCmdStatus: Indicates if the Get File Attribute was successful or not
** nSlotID: Indicates the slot that the command is performed on
** sFileAttribData: The FileAttribData returned by a successful Get
**                  File Attribute command */
typedef struct
{
    uint32                    dwStructureSize;                         /* Size of the structure */
    int                       nCmdStatus;                              /* SUCCESS/EFAILED/EUNSUPPORTED */
    int                       nSlotID;                                 /* slot ID of the operation */
    uint8                     byFileID[ AEECARD_FILE_ID_LEN ];         /* File ID as specified in spec 31.102 */
    AEECardFileType           nFileType;                               /* MF, DF or EF */
    AEECardFileStructureType  nFileStructure;                          /* Cyclic, linear or transparent EF */
    int                       nFileSize;                               /* size of the file or directory */
    int                       nRecordLen;                              /* length of record for cyclic and linear
                                                                          file.  If the file is transparent,
                                                                          nRecordLen == nfileSize */
    int                       nNumberRecords;                          /* number of record */
    flg                       bCyclicIncreaseAllowed:1;                /* Increase operation is allowed or not for
                                                                          the file */
    AEECardFileProtectionType nFileProtection;                         /* File security access condition */
    flg                       bFileInvalidated:1;                      /* File has been invalidated or not */
    flg                       bReadWriteAllowedWhenInvalidated:1;      /* Indicate if Read/write operation is allowed
                                                                          when the file is invalidate/deactivate */
}
AEECardFileAttribStatus;

/* This structure indicates CPHS Call Offering Services */
typedef struct {
  uint32 dwStructureSize;    /* Size of structure */
  flg    bCFU:1;             /* Call forwarding unconditional */
  flg    bCFB:1;             /* Call forwarding on User Busy */
  flg    bCFNRy:1;           /* Call forwarding onNo reply */
  flg    bCFNRc:1;           /* Call forwarding on User Not reachable */
  flg    bCT:1;              /* Call transfer */
}
AEECardCPHSCallOfferType;

/* This structure indicates CPHS Call Restriction Services */
typedef struct {
  uint32 dwStructureSize;    /* Size of structure */
  flg    bBOAC:1;            /* Barring of All outgoing calls */
  flg    bBOIC:1;            /* Barring of outgoing International calls */
  flg    bBOICexHC:1;        /* Barring of outgoing International calls except directed to the home PLMN country */
  flg    bBAIC:1;            /* Barring of all incoming calls when roaming outside the home PLMN country */
  flg    bBICRoam:1;
}
AEECardCPHSCallRestrictType;

/* This structure indicates CPHS Other Supplementary Service Services */
typedef struct {
  uint32 dwStructureSize;    /* Size of structure */
  flg    bMPTY:1;            /* Multiparty service */
  flg    bCUG:1;             /* Closed user group */
  flg    bAoC:1;             /* Advice of Charge */
  flg    bPreferredCUG:1;    /* Preferential CUG */
  flg    bCUGOA:1;           /* CUG outgoing access */
}
AEECardCPHSOtherSSType;

/* This structure indicates CPHS Call Completion Services */
typedef struct {
  uint32 dwStructureSize;        /* Size of structure */
  flg    bHold:1;                /* Call holding */
  flg    bCW:1;                  /* Call waiting */
  flg    bCCBS:1;                /* Completion of call to busy subscriber */
  flg    bUserSignalling:1;      /* use of user signalling */
}
AEECardCPHSCallCompleteType;

/* This structure indicates CPHS Teleservices Services */
typedef struct {
  uint32 dwStructureSize;        /* Size of structure */
  flg    bSMS_MT:1;              /* Mobile terminated SMS */
  flg    bSMS_MO:1;              /* Mobile originated SMS */
  flg    bSMS_CB:1;              /* Cell Broadcast SMS */
  flg    bReplyPath:1;           /* Restricts menu options for the ability to set reply path active on outgoing SMS */
  flg    bDeliveryConf:1;        /* SMS delivery confirmation */
  flg    bProtocolID:1;          /* Restricts menu for SMS protocol ID options */
  flg    bValidityPeriod:1;      /* restrict menu for SMS validity period options */
}
AEECardCPHSTeleServiceType;

/* This structure indicates CPHS CPHS Teleservices Services */
typedef struct {
  uint32 dwStructureSize;        /* Size of structure */
  flg    bALS:1;                 /* Alternate line selection */
}
AEECardCPHSCPHSTeleServiceType;

/* This structure indicates CPHS Number Identification Services */
typedef struct {
  uint32 dwStructureSize;        /* Size of structure */
  flg    bCLIP:1;                /* Calling line identification presentation */
  flg    bCoLR:1;                /* Connected line identification restriction */
  flg    bCoLP:1;                /* Connected line identification presentation */
  flg    bMCI:1;                 /* Malicious call indicator */
  flg    bCLISend:1;             /* CLI per call mode-default block CLI-menu to send CLI */
  flg    bCLIBlock:1;            /* CLI per call mode-default send CLI-menu to block CLI */
}
AEECardCPHSNumberIDType;

/* This structure indicates CPHS Phase 2+ service Services */
typedef struct {
  uint32 dwStructureSize;              /* Size of structure */
  flg    bGPRS:1;                      /* menu related to GPRS functionality */
  flg    bHSCSD:1;                     /* menu related to high speed circuit switched data functionality */
  flg    bVoiceGroupCall:1;            /* ASCI voice group call menu */
  flg    bVoiceBroadcastService:1;     /* ASCI voice broadcast service menu */
  flg    bMultipleSubscriberProfile:1; /* Phase 2+ multiple subscriber profile menu */
  flg    bMultipleBand:1;              /* Restrict menu to GSM 900/1800 or 1900 band selection */
}
AEECardCPHSPhaseSvcType;

/* This structure indicates CPHS Value Added service Services */
typedef struct {
  uint32 dwStructureSize;              /* Size of structure */
  flg    bPLMNMode:1;                  /* restrict menu options for manual PLMN selection */
  flg    bVPS:1;                       /* restrict menu options for voice mail or other similar menu */
  flg    bSMS_MO_Paging:1;             /* restrict menu for ability to send SMS with Paging */
  flg    bSMS_MO_Email:1;              /* restrict menu for ability to send SMS via E-mail */
  flg    bFax:1;                       /* restrict menu for Fax calls */
  flg    bData:1;                      /* restrict menu for Data calls */
  flg    bLanguage:1;                  /* restrict menu for changing language */
}
AEECardCPHSValueAddedSvcType;


/* This structure indicates CPHS Services Groups */
typedef struct {
  uint32                            dwStructureSize;        /* Size of structure */
  flg                               CSP_Available:1;        /* customer service profile available or not */
  AEECardCPHSCallOfferType          sCallOffering;          /* call offering menu access indication */
  AEECardCPHSCallRestrictType       sCallRestriction;       /* call restriction menu access indication */
  AEECardCPHSOtherSSType            sOtherSuppSvc;          /* other supplemenary service menu access indication */
  AEECardCPHSCallCompleteType       sCallCompletion;        /* call completion menu access indication */
  AEECardCPHSTeleServiceType        sTeleService;           /* teleservice menu access indication */
  AEECardCPHSCPHSTeleServiceType    sCPHSTeleService;       /* CPHS tele service menu access indication */
  flg                               sCPHSServiceFeature:1;
  AEECardCPHSNumberIDType           sNumberIdentification;  /* number identification */
  AEECardCPHSPhaseSvcType           sPhase2PlusSvc;         /* phase 2+ related info */
  AEECardCPHSValueAddedSvcType      sValueAddedSvc;         /* value added svc info */
  flg                               bInformationNumbers:1;  /* information number info */
}
AEECardCPHSCoreSvcProfileDataBufType;


/* This structure indicates the service options available
** in the CPHS Services Table.
** The member will be set to TRUE only if the service
** table indicates that the service is Activated and Allocated */
typedef struct {
  uint32 dwStructureSize;            /* Size of structure */
  flg    bOpNameShortform:1;         /* operator name short form support or not */
  flg    bMailboxNumbers:1;          /* mailbox number support or not */
  flg    bServiceStringTable:1;      /* service string table support or not */
  flg    bCustomerServiceProfile:1;  /* customer service profile support or not */
  flg    bInformationNumbers:1;      /* information number support or not */
}
AEECardCPHSSvcTableDataBufType;


/* Enum indicating whether the SIM supports Phase 1 or Phase 2 CPHS */
typedef enum {
  AEECARD_CPHS_NO_SUPPORT,
  AEECARD_CPHS_PHASE_1_SUPPORT,
  AEECARD_CPHS_PHASE_2_SUPPORT,
  AEECARD_CPHS_SUPPORT_MAX = 0xFFFF
}AEECardCPHSPhaseEnumType;

/* CPHS Status
** nCmdStatus: Indicating the result of the GetCPHSInfo command
** nSlotID: Slot for the operation
** bCPHSSupported: Indicating if the Mobile supports CPHS
** nCPHSPhase: Indicating the SIM support for CPHS
** sCPHSSvcTableData: The CPHS Service Table information
** sCSPData: The CSP service support */
typedef struct {
  uint32                               dwStructureSize;
  int                                  nCmdStatus;
  int                                  nSlotID;
  flg                                  bCPHSSupported:1;
  AEECardCPHSPhaseEnumType             nCPHSPhase;
  AEECardCPHSSvcTableDataBufType       sCPHSSvcTableData;
  AEECardCPHSCoreSvcProfileDataBufType sCSPData;
}
AEECardCPHSStatus;


typedef struct{
  AEECardFileItemEnumType filename;
  uint8 *data;
  uint32 length;
}AEECardFileContents;

typedef struct AEEPuctInfo
{
  float  ppu;                /* Price per unit in coded currency */
  uint8  currencyCode[3];    /* Currency code as defined in GSM 03.38 */
} AEEPuctInfo;


typedef struct{
  int   nCmdStatus;
  int32 nNW_NumRetries;
  int32 nNS_NumRetries;
  int32 nSP_NumRetries;
  int32 nCP_NumRetries;
  int32 nSIM_NumRetries;
  int32 nRUIMNW1_NumRetries;
  int32 nRUIMNW2_NumRetries;
  int32 nRUIMHRPD_NumRetries;
  int32 nRUIMSP_NumRetries;
  int32 nRUIMCP_NumRetries;
  int32 nRUIMRUIM_NumRetries;

  int32 nProp1_NumRetries;
}AEECardDCKNumRetries;

typedef struct{
  int nCmdStatus;
  int32 nStatus;     /* AEECARD_PIN_UNKNOWN  or AEECARD_PIN_ENABLED or AEECARD_PIN_NOT_VALID*/
  int32 nNumRetries;
}AEECardPhoneCodeStatus;


typedef enum{
  AEECARD_SAP_SERVER,
  AEECARD_SAP_CLIENT
}AEECardSAPMode;

typedef enum{
  AEECARD_DISC_IMMED,
  AEECARD_DISC_GRACE
}AEECardSAPDisconnectMode;


typedef enum{
  AEECARD_SAP_CONNECTED_STATE,
  AEECARD_SAP_CONNECTING_STATE,
  AEECARD_SAP_CONNECTION_FAIL_STATE,  
  AEECARD_SAP_DISCONNECTED_STATE, 
  AEECARD_SAP_DISCONNECTING_STATE, 
  AEECARD_SAP_DISCONNECTION_FAIL_STATE, 
  AEECARD_SAP_CONNECTION_NONE_STATE
}AEECardSAPConnState;

typedef struct{
  int                  nCmdStatus;
  AEECardSAPConnState  ConnectState;
}AEECardSAPCmdStatus;

typedef struct{
  AEECardSAPConnState connectState;
  AEECardSAPDisconnectMode disconnectType;
}AEECardSapStatus;
/* ----------------------------------------------------------------------------
** MAX Number of Files GSDI can perform File Change Notification for
** ---------------------------------------------------------------------------*/
#define AEECARD_MAX_FILE_CHANGE_NOTIFICATIONS 37


/* Forward typedef reference of the objects. */
typedef struct ICard ICard;
AEEINTERFACE(ICard)
{
   INHERIT_IQueryInterface(ICard);

   int   (*GetCardStatus)             (ICard *po, uint8 *pCardStatus);
   int   (*GetPinStatus)              (ICard *po, uint8 pin_id, AEECardPinStatus *pPinStatus);
   int   (*VerifyPin)                 (ICard *po, uint8 pin_id, AEECardPinCode *pPin,
                                       AEECardPinCmdStatus *pCmdStatus, AEECallback *pUserCB);
   int   (*UnblockPin)                (ICard *po, uint8 pin_id, AEECardPinCode *pPuk, AEECardPinCode *pPin,
                                       AEECardPinCmdStatus *pCmdStatus, AEECallback *pUserCB);
   int   (*ChangePin)                 (ICard *po, uint8 pin_id, AEECardPinCode *pPin, AEECardPinCode *pNew_pin,
                                       AEECardPinCmdStatus *pCmdStatus, AEECallback *pUserCB);
   int   (*DisablePin)                (ICard *po, uint8 pin_id, AEECardPinCode *ppin,
                                       AEECardPinCmdStatus *pCmdStatus, AEECallback *pUserCB);
   int   (*EnablePin)                 (ICard *po, uint8 pin_id, AEECardPinCode *ppin,
                                       AEECardPinCmdStatus *pCmdStatus, AEECallback *pUserCB);
   int   (*GetCardID)                 (ICard *po, char *pID, int *pnLen);
   int   (*GetCardApps)               (ICard *po, AEECardApp *pCardApp);
   int   (*GetCardStatusSlot)         (ICard *po, uint8 wSlot, uint8 *pCardStatus);
   int   (*EnableFDNSlot)             (ICard *po, uint8 wSlot, boolean bPresent, AEECardPinCode *pPin,
                                       AEECardGenericCmdStatus *pCmdStatus, AEECallback *pUserCB);
   int   (*DisableFDNSlot)            (ICard *po, uint8 wSlot, boolean bPresent, AEECardPinCode *pPin,
                                       AEECardGenericCmdStatus *pCmdStatus, AEECallback *pUserCB);
   int   (*GetSimCapabilitySlot)      (ICard *po, uint8 wSlot, AEESimCapabilities *pSimCap);
   int   (*ChangeUserSlotPref)        (ICard *po, uint8 wNewSlot, uint8 wApp,
                                       AEECardGenericCmdStatus *pCmdStatus, AEECallback *pUserCB);
   int   (*GetUserSlotPref)           (ICard *po,  uint8 wApp, uint8 *pwSlot);
   int   (*GetATRSize)                (ICard *po, uint8 wSlot, int *nSize);
   int   (*GetATR)                    (ICard *po, uint8 wSlot, AEECardATR *pATR);
   int   (*ActivatePersoFeatureInd)   (ICard *po, uint8 wSlot, int nFeatureID, AEECardPinCode *pKey, AEECardDeactivatePersoCmdStatus *pCmdStatus, AEECallback *pUserCB);
   int   (*DeactivatePersoFeatureInd) (ICard *po, uint8 wSlot, int nFeatureID, AEECardPinCode *pKey, AEECardDeactivatePersoCmdStatus *pCmdStatus, AEECallback *pUserCB);
   int   (*GetPersoFeatureInd)        (ICard *po, uint8 wSlot, AEECardGetPersoFeatureCmdStatus *pCmdStatus, AEECallback *pUserCB);
   int   (*SetPersoFeatureData)       (ICard *po, uint8 wSlot, AEECardPersoFeatureInfo *pFeatureInd, AEECardGenericCmdStatus *pCmdStatus, AEECallback *pUserCB);
   int   (*GetPersoStatus)            (ICard *po, uint8 bySlot, uint8 *pPersoStatus);
   int   (*GetServiceAvailableInd)    (ICard *po, uint8 bySlot, AEECardServiceEnumType dwService, AEECardServiceIndType *pSvcInd);
   int   (*GetFileAttributes)         (ICard *po, uint8 bySlot, AEECardFilePathType sFilePath, AEECardFileAttribStatus *pCmdStatus, AEECallback *pUserCB);
   int   (*ReadRecord)                (ICard *po, uint8 bySlot, AEECardFilePathType sFilePath, int nRecordNum, AEECardGenericCmdStatus *pCmdStatus, AEECallback *pUserCB);
   int   (*ReadBinary)                (ICard *po, uint8 bySlot, AEECardFilePathType sFilePath, int nStartOffset, int nNumBytes, AEECardGenericCmdStatus *pCmdStatus, AEECallback *pUserCB);
   int   (*WriteRecord)               (ICard *po, uint8 bySlot, AEECardFilePathType sFilePath, int nRecordNum, AEECardDataBufType *pWrittenData, AEECardGenericCmdStatus *pCmdStatus, AEECallback *pUserCB);
   int   (*WriteBinary)               (ICard *po, uint8 bySlot, AEECardFilePathType sFilePath, int nStartOffset, AEECardDataBufType *pWrittenData, AEECardGenericCmdStatus *pCmdStatus, AEECallback *pUserCB);
   int   (*GetCPHSInfo)               (ICard *po, uint8 bySlot, AEECardCPHSStatus *pCmdStatus, AEECallback *pUserCB);
   int   (*GetRespData)               (ICard *po, uint32 dwSeqNum, AEECardDataBufType *pData);
   int   (*RegisterForFCN)            (ICard *po, uint32 dwClsID, uint32 dwNumFiles, AEECardFileItemEnumType *pFileList);
   int   (*EncodePUCT)                (ICard *po, AEEPuctInfo *pInPUCTInfo, uint8 *pOutEncodedBuff);
   int   (*DecodePUCT)                (ICard *po, uint8 *pInEncodedBuff, AEEPuctInfo *pOutPUCTInfo);
   int   (*UnblockPersoDCK)           (ICard *po, uint8 wSlot, int nFeatureID, AEECardPinCode *pKey, AEECardDeactivatePersoCmdStatus *pCmdStatus, AEECallback *pUserCB);
   int   (*GetPersoDCKNumRetries)     (ICard *po, uint8 wSlot, AEECardDCKNumRetries *pCmdStatus, AEECallback *pUserCB);
   int   (*GetPhoneCodeStatus)        (ICard *po, uint8 wSlot, AEECardPhoneCodeStatus *pCmdStatus, AEECallback *pUserCB);
   int   (*SetPhoneCode)              (ICard *po, AEECardPinCode *pPin, AEECardGenericCmdStatus *pCmdStatus, AEECallback *pUserCB);
   int   (*SAPConnect)                (ICard *po, uint8 wSlot, AEECardSAPMode SapMode, AEECardSAPCmdStatus *pSAPCmdStatus, AEECallback *pUserCB);
   int   (*SAPDisconnect)             (ICard *po, uint8 wSlot, AEECardSAPMode SapMode, AEECardSAPDisconnectMode DisconnectMode, AEECardSAPCmdStatus *pSAPCmdStatus, AEECallback *pUserCB);
   int   (*SAPDeregister)             (ICard *po, uint8 wSlot, AEECardSAPCmdStatus *pSAPCmdStatus, AEECallback *pUserCB);
   int   (*GetATRAsync)               (ICard *po, uint8 wSlot, AEECardATR *pATR, AEECallback *pUserCB);
};


/* The following defines make for easier naming of the function */
#define ICARD_AddRef(p)    AEEGETPVTBL(p,ICard)->AddRef(p)
#define ICARD_Release(p)   AEEGETPVTBL(p,ICard)->Release(p)
#define ICARD_QueryInterface(p,clsid,pp) \
           AEEGETPVTBL(p,ICard)->QueryInterface(p,clsid,pp)
#define ICARD_GetCardStatus(p,pStatus) \
           AEEGETPVTBL(p,ICard)->GetCardStatus(p,pStatus)
#define ICARD_GetPinStatus(p,pinID,pPinStatus) \
           AEEGETPVTBL(p,ICard)->GetPinStatus(p,pinID,pPinStatus)
#define ICARD_VerifyPin(p,pinID,szPin,pStatus,pCB) \
           AEEGETPVTBL(p,ICard)->VerifyPin(p,pinID,szPin,pStatus,pCB)
#define ICARD_UnblockPin(p,pinID,szPuk,szPin,pStatus,pCB) \
           AEEGETPVTBL(p,ICard)->UnblockPin(p,pinID,szPuk,szPin,pStatus,pCB)
#define ICARD_ChangePin(p,pinID,szOldPin,szNewPin,pStatus,pCB) \
           AEEGETPVTBL(p,ICard)->ChangePin(p,pinID,szOldPin,szNewPin,pStatus,pCB)
#define ICARD_EnablePin(p,pinID,szPin,pStatus,pCB) \
           AEEGETPVTBL(p,ICard)->EnablePin(p,pinID,szPin,pStatus,pCB)
#define ICARD_DisablePin(p,pinID,szPin,pStatus,pCB) \
           AEEGETPVTBL(p,ICard)->DisablePin(p,pinID,szPin,pStatus,pCB)
#define ICARD_GetCardID(p,pID,pLen) \
           AEEGETPVTBL(p,ICard)->GetCardID(p,pID,pLen)
#define ICARD_GetCardApps(p,pCardApp) \
           AEEGETPVTBL(p,ICard)->GetCardApps(p,pCardApp)
#define ICARD_GetCardStatusSlot(p,wSlot,pStatus) \
           AEEGETPVTBL(p,ICard)->GetCardStatusSlot(p,wSlot,pStatus)
#define ICARD_EnableFDNSlot(p,wSlot,bPresent,szPin,pStatus,pCB) \
           AEEGETPVTBL(p,ICard)->EnableFDNSlot(p,wSlot,bPresent,szPin,pStatus,pCB)
#define ICARD_DisableFDNSlot(p,wSlot,bPresent,szPin,pStatus,pCB) \
           AEEGETPVTBL(p,ICard)->DisableFDNSlot(p,wSlot,bPresent,szPin,pStatus,pCB)
#define ICARD_GetSimCapabilitySlot(p,wSlot,pSimCap) \
           AEEGETPVTBL(p,ICard)->GetSimCapabilitySlot(p,wSlot,pSimCap)
#define ICARD_ChangeUserSlotPref(p,wNewSlot,wApp,pStatus,pCB) \
           AEEGETPVTBL(p,ICard)->ChangeUserSlotPref(p,wNewSlot,wApp,pStatus,pCB)
#define ICARD_GetUserSlotPref(p,wApp,pwSlot) \
           AEEGETPVTBL(p,ICard)->GetUserSlotPref(p,wApp,pwSlot)
#define ICARD_GetATRSize(p,wSlot,nSize) \
           AEEGETPVTBL(p,ICard)->GetATRSize(p,wSlot,nSize)
#define ICARD_GetATR(p,wSlot,pATR) \
           AEEGETPVTBL(p,ICard)->GetATR(p,wSlot,pATR)
#define ICARD_ActivatePersoFeatureInd(p,wSlot,nFeatureID,pKey,pCmdStatus,pUserCB) \
           AEEGETPVTBL(p,ICard)->ActivatePersoFeatureInd(p,wSlot,nFeatureID,pKey,pCmdStatus,pUserCB)
#define ICARD_DeactivatePersoFeatureInd(p,wSlot,nFeatureID,pKey,pCmdStatus,pUserCB) \
           AEEGETPVTBL(p,ICard)->DeactivatePersoFeatureInd(p,wSlot,nFeatureID,pKey,pCmdStatus,pUserCB)
#define ICARD_GetPersoFeatureInd(p,wSlot,pCmdStatus,pUserCB) \
           AEEGETPVTBL(p,ICard)->GetPersoFeatureInd(p,wSlot,pCmdStatus,pUserCB)
#define ICARD_SetPersoFeatureData(p,wSlot,pFeatureInd,pCmdStatus,pUserCB) \
           AEEGETPVTBL(p,ICard)->SetPersoFeatureData(p,wSlot,pFeatureInd,pCmdStatus,pUserCB)
#define ICARD_GetPersoStatus(p,bySlot,pPersoStatus) \
           AEEGETPVTBL(p,ICard)->GetPersoStatus(p,bySlot,pPersoStatus)
#define ICARD_GetServiceAvailableInd(p,bySlot,dwService,pSvcInd) \
           AEEGETPVTBL(p,ICard)->GetServiceAvailableInd(p,bySlot,dwService,pSvcInd)
#define ICARD_GetFileAttributes(p,bySlot,sFilePath,pCmdStatus,pUserCB) \
           AEEGETPVTBL(p,ICard)->GetFileAttributes(p,bySlot,sFilePath,pCmdStatus,pUserCB)
#define ICARD_ReadRecord(p,bySlot,sFilePath,nRecordNum,pCmdStatus,pUserCB) \
           AEEGETPVTBL(p,ICard)->ReadRecord(p,bySlot,sFilePath,nRecordNum,pCmdStatus,pUserCB)
#define ICARD_ReadBinary(p,bySlot,sFilePath,nStartOffset,nNumBytes,pCmdStatus,pUserCB) \
           AEEGETPVTBL(p,ICard)->ReadBinary(p,bySlot,sFilePath,nStartOffset,nNumBytes,pCmdStatus,pUserCB)
#define ICARD_WriteRecord(p,bySlot,sFilePath,nRecordNum,pWrittenData,pCmdStatus,pUserCB) \
           AEEGETPVTBL(p,ICard)->WriteRecord(p,bySlot,sFilePath,nRecordNum,pWrittenData,pCmdStatus,pUserCB)
#define ICARD_WriteBinary(p,bySlot,sFilePath,nStartOffset,pWrittenData,pCmdStatus,pUserCB) \
           AEEGETPVTBL(p,ICard)->WriteBinary(p,bySlot,sFilePath,nStartOffset,pWrittenData,pCmdStatus,pUserCB)
#define ICARD_GetCPHSInfo(p,bySlot,pCmdStatus,pUserCB) \
           AEEGETPVTBL(p,ICard)->GetCPHSInfo(p,bySlot,pCmdStatus,pUserCB)
#define ICARD_GetRespData(p,dwSeqNum,pData) \
           AEEGETPVTBL(p,ICard)->GetRespData(p,dwSeqNum,pData)
#define ICARD_RegisterForFCN(p,dwClsID,dwNumFiles,pFileList) \
           AEEGETPVTBL(p,ICard)->RegisterForFCN(p,dwClsID,dwNumFiles,pFileList)
#define ICARD_EncodePUCT(p,pPuctInfo,pEncodedBuff) \
           AEEGETPVTBL(p,ICard)->EncodePUCT(p,pPuctInfo,pEncodedBuff)
#define ICARD_DecodePUCT(p,pEncodedBuff,pPuctInfo) \
           AEEGETPVTBL(p,ICard)->DecodePUCT(p,pEncodedBuff,pPuctInfo)
#define ICARD_UnblockPersoDCK(p,wSlot,nFeatureID,pKey,pCmdStatus,pUserCB) \
           AEEGETPVTBL(p,ICard)->UnblockPersoDCK(p,wSlot,nFeatureID,pKey,pCmdStatus,pUserCB)
#define ICARD_GetPersoDCKNumRetries(p,wSlot,pCmdStatus,pUserCB) \
           AEEGETPVTBL(p,ICard)->GetPersoDCKNumRetries(p,wSlot,pCmdStatus,pUserCB)
#define ICARD_GetPhoneCodeStatus(p,wSlot,pCmdStatus,pUserCB) \
           AEEGETPVTBL(p,ICard)->GetPhoneCodeStatus(p,wSlot,pCmdStatus,pUserCB)
#define ICARD_SetPhoneCode(p,pPin,pCmdStatus,pUserCB) \
           AEEGETPVTBL(p,ICard)->SetPhoneCode(p,pPin,pCmdStatus,pUserCB)
#define ICARD_SAPConnect(p,wSlot,SapMode,pSAPCmdStatus,pUserCB) \
           AEEGETPVTBL(p,ICard)->SAPConnect(p,wSlot,SapMode,pSAPCmdStatus,pUserCB)
#define ICARD_SAPDisconnect(p,wSlot,SapMode,DisconnectMode,pSAPCmdStatus,pUserCB) \
           AEEGETPVTBL(p,ICard)->SAPDisconnect(p,wSlot,SapMode,DisconnectMode,pSAPCmdStatus,pUserCB)
#define ICARD_SAPDeregister(p,wSlot,pSAPCmdStatus,pUserCB) \
           AEEGETPVTBL(p,ICard)->SAPDeregister(p,wSlot,pSAPCmdStatus,pUserCB)
#define ICARD_GetATRAsync(p,wSlot,pATR,pUserCB) \
           AEEGETPVTBL(p,ICard)->GetATRAsync(p,wSlot,pATR, pUserCB)



AEEINTERFACE(ICardConnection)
{
   INHERIT_IQueryInterface(ICardConnection);

   int   (*Open)        (ICardConnection *po, uint8 wSlotID, AEECardConnectionOpenStatus *pCmdStatus,
                         AEECallback *pUserCB);
   int   (*Close)       (ICardConnection *po, AEECardConnectionCloseStatus *pCmdStatus,
                         AEECallback *pUserCB);
   int   (*SendAPDU)    (ICardConnection *po, AEECardAPDUCmd *pAPDU, uint32 dwClientData,
                         AEECardConnectionCmdStatus *pCmdStatus, AEECallback *pUserCB);
   int   (*GetAPDURsp)  (ICardConnection *po, uint32 dwSeqNum, AEECardAPDUResp *pAPDU);

};

/* The following defines make for easier naming of the function */
#define ICARDCONNECTION_AddRef(p)    AEEGETPVTBL(p,ICardConnection)->AddRef(p)
#define ICARDCONNECTION_Release(p)   AEEGETPVTBL(p,ICardConnection)->Release(p)
#define ICARDCONNECTION_QueryInterface(p,clsid,pp) \
           AEEGETPVTBL(p,ICardConnection)->QueryInterface(p,clsid,pp)
#define ICARDCONNECTION_Open(p,wSlotID,pStatus,pCB) \
           AEEGETPVTBL(p,ICardConnection)->Open(p,wSlotID,pStatus,pCB)
#define ICARDCONNECTION_Close(p,pStatus,pCB) \
           AEEGETPVTBL(p,ICardConnection)->Close(p,pStatus,pCB)
#define ICARDCONNECTION_SendAPDU(p,pAPDU,dwData,pStatus,pCB) \
           AEEGETPVTBL(p,ICardConnection)->SendAPDU(p,pAPDU,dwData,pStatus,pCB)
#define ICARDCONNECTION_GetAPDURsp(p,dwSeqNum,pAPDU) \
           AEEGETPVTBL(p,ICardConnection)->GetAPDURsp(p,dwSeqNum,pAPDU)



/*=======================================================================
  INTERFACES   DOCUMENTATION
=======================================================================
Interface Name: ICard

Description:
   ICard is a simple interface to the SIM/USIM layer.
It provides the following services:

- Retrieving status of the Card and relavent parameters.
- Locking/Unlocking the card.
- Allowing access to changing the PIN codes.
- Notifying clients of PIN events.

The ICard interface is obtained via the ISHELL_CreateInstance mechanism.

The following defines are used to communicate the card status
from AEECARD to the Applications.  When the application requests to
be notified of a change in card state, the pdata field in AEENotify
structure will contain a pointer to a uint8 containing the values
specified below.
AEECARD_NO_CARD       -- Indicates No card is inserted.
AEECARD_INVALID_CARD  -- Indicates an Unusable Card is inserted.
AEECARD_VALID_CARD    -- Indicates a Usable card is present.
AEECARD_NOT_INIT      -- Indicates OEMCard has not fully initialized yet.
                         A notification will follow with updated information.
                         Request the Card status information after the
                         notifcation or use the info from the notification.
AEECARD_NOT_READY     -- Indicates OEMCard has not received any notification
                         from MMGSDI yet.  The card status is initialized to
                         this value on power up.
AEECARD_CARD_REINIT   -- Indicates OEMCard has received notification that the
                         SIM Initialization procedures are about to be executed
                         again.  Must send notify CM that service is not available.
AEECARD_AVAIL_CARD    -- Indicates OEMCard has received a notification that the
                         card is presented but no provisioning information will
                         be coming from that card.

Pin codes can have the following lock states.
AEECARD_PIN_UNKNOWN        PIN does not make sense (ie, no CARD)
AEECARD_PIN_DISABLED       PIN is not required.
AEECARD_PIN_ENABLED        PIN is required.
AEECARD_PIN_VERIFIED       PIN is required.
AEECARD_PIN_BLOCKED        Pin requires Unlock Key.
AEECARD_PIN_PERM_DISABLED  Pin cannot be Unblocked.
AEECARD_PIN_NOT_VALID      Pin functionality not supported.

// Pin ids are identified using.
AEECARD_PIN_INVALID   Invalid PIN ID.
AEECARD_PIN1          ID for Global PIN 1
AEECARD_PIN2          ID for Global PIN 2
AEECARD_PIN1_SLOT2    ID for Global PIN 1 for Slot 2
AEECARD_PIN2_SLOT2    ID for Global PIN 2 for Slot 2
AEECARD_PHONE_CODE    Phone Code
AEECARD_SAP_PIN       SAP PIN (16 digits)
AEECARD_MAXPIN        Last PIN we currently support.

// When CARD cannot determine the number of tries, it shall return
// the following to indicate the tries_remaining field is not valid.
AEECARD_UNKNOWN_TRIES

Application Bitmask indicates what DFs are presented on the Card
AEECARD_NO_APP_MASK     -- There is no application on the card
AEECARD_TELECOM_MASK    -- There is a Telecom directory on the card
AEECARD_GSM_SIM_MASK    -- There is a GSM application on the card
AEECARD_USIM_APP_MASK   -- There is an USIM application on the card
AEECARD_RUIM_APP_MASK   -- There is a RUIM application on the card
AEECARD_EMV_APP_MASK    -- There is an EMV application on the card

AEECARD_GSM_SLOT1_OPERATIONAL       -- GSM subscription info should be retrieved from slot 1
AEECARD_GSM_SLOT2_OPERATIONAL       -- GSM subscription info should be retrieved from slot 2
AEECARD_CDMA_SLOT1_OPERATIONAL      -- CDMA subscription info should be retrieved from slot 1
AEECARD_CDMA_SLOT2_OPERATIONAL      -- CDMA subscription info should be retrieved from slot 2
AEECARD_UMTS_SLOT1_OPERATIONAL      -- UMTS subscription info should be retrieved from slot 1
AEECARD_UMTS_SLOT2_OPERATIONAL      -- UMTS subscription info should be retrieved from slot 2

AEECARD_SLOT_NONE                   -- No specific slot (only valid in user preferred slot functions )
AEECARD_SLOT1                       -- Slot1
AEECARD_SLOT2                       -- Slot2

AEECARD_GSM_APP                     -- Indicates the action is to be performed on GSM application
AEECARD_CDMA_APP                    -- Indicates the action is to be performed on CDMA application

ECMDDISALLOWED                      -- Indicates command is not allowed,
                                       currently used by ICARDCONNECTION_SendAPDU

--- This structure is used to define the status of a PIN.  It is used
--- in two different instances.  The first is when the application has
--- registered to be notified when the PIN status is updated.  In that
--- case, this structure is sent as the pdata field in the AEENotify
--- structure.  The second place this structure is used is within the
--- structure defined below for the results of a PIN code.
typedef struct
{
  uint8 lock_state;           --- Lock state of this PIN ---
  uint8 tries_remaining;      --- Number of unlock tries remaining. ---
} AEECardPinStatus;

--- This structure is used to define the PIN code being sent.  ---
typedef struct
{
  uint8    code_len;    --- Number of characters in the PIN ---
  AECHAR  *code;        --- The string of characters making the PIN ---
} AEECardPinCode;

--- This structure is used to pass the information back to client
--- upon the completion of any PIN related processes
--- To retrieve the Pin status information, user is required to
--- use ICard_GetPinStatus function upon their callbacks are
--- being called after the pin actions.
typedef struct
{
  int nCmdStatus;              --- SUCCESS/EFAILED/etc
  int nPinID;                  --- PIN ID for the action that is completed
  AEECardStatusWord  sStatus;  --- Status word returned from the Card
}
AEECardPinCmdStatus;

--- This structure is used to pass the information back to client
--- upon the completion of any non PIN related processes
typedef struct
{
  int nCmdStatus;              --- SUCCESS/EFAILED/etc
  int nSlotID;                 --- Slot ID for the action that is completed
  uint32  dwData;              --- Data to be passed back to user
                                   Setting User Preferred Slot:
                                      - TRUE if there will be a refresh action
                                      - FALSE if there will not be a refresh action
}
AEECardGenericCmdStatus;

--- This structure indicates what are the information available from the card. ---
typedef struct {
  boolean slot1_present;        --- If there is card in slot 1 ---
  uint8   apps1_enabled;          --- Bitmap of what Apps are available in slot 1 ---
  boolean slot2_present;          --- If there is card in slot 2 ---
  uint8   apps2_enabled;          --- Bitmap of what apps are available in slot 2 ---
  uint8   operational_slot;   --- Indicates which slot should the GSM and CDMA ---
                              --- provisioning info should come from ---
                              --- Bits [0-1] -> Slot Number for GSM App provisioning info ---
                                              --- Bits [2-3] -> Slot Number for CDMA App provisioning info ---
} AEECardApp;

--- This structure contains information on FDN, BDN, ACL and IMSI status ---
typedef struct {
  boolean  fdn_enabled;          --- Is FDN enabled? ---
  boolean  bdn_enabled;          --- Is BDN enabled? ---
  boolean  acl_enabled;          --- Is ACL enabled? ---
  boolean  imsi_invalidated;     --- Is IMSI invalidate? ---
  boolean  unknown_capabilities; --- Unknown capability ---
} AEESimCapabilities;

--- This structure contains the ATR information ---
typedef struct {
   int    nSize;
   uint8  *pATR;
}
AEECardATR;

--- This structure contains Status Word returned from the card ---
typedef struct {
  uint8   sw1;
  uint8   sw2;
} AEECardStatusWord;

--- This structure contains APDU response from the card ---
typedef struct {
   int    nSize;
   uint8 *pAPDUData;       --- status word is appended at the end of the resp data
} AEECardAPDUResp;

--- This structure is the command status data structure that ICardConnection
    will populate upon processing the Send command ---
typedef struct
{
  ICardConnection *pCardConnection;   --- The connection pointer ---
  int nCmdStatus;                     --- SUCCESS if there are data returned
                                          by the card ---
  uint32  dwClientData;               --- Data passed in by the client if
                                          exists ---
  int     nSize;                      --- Response APDU Data ---
  uint32  dwSeqNum;                   --- SeqNum provided to application
                                          such that a correct mapping of
                                          GetAPDURsp APDU responses when called ---

}
AEECardConnectionCmdStatus;


--- This structure is the command status data structure that ICardConnection
    will populate upon processing the Open command ---
typedef struct
{
  ICardConnection *pCardConnection;
  int nCmdStatus;              --- Success if the card is able to
                                   process the request ---
  uint8 wSlotID;               --- Slot ID for the open channel
                                    AEECARD_SLOT1
                                    AEECARD_SLOT2 ---
  AEECardStatusWord sw;        --- status word, application is
                                  required to check the status word
                                  to determine if the Open command
                                  is successful or not ---
}
AEECardConnectionOpenStatus;


--- This structure is the command status data structure that ICardConnection
    will populate upon processing the Close command ---
typedef struct
{
  ICardConnection *pCardConnection;
  int nCmdStatus;              --- Success if the card is able to
                                   process the request ---
  AEECardStatusWord sw;        --- status word, application is
                                   required to check the status word
                                   to determine if the Close command
                                   is successful or not ---

}
AEECardConnectionCloseStatus;

--- This structure contains APDU Cmd to be sent to the card ---
typedef struct
{
  int    nSize;                --- Size of the APDU command ---
  uint8 *pAPDUData;            --- Pointer to APDU command
                                   The expected APDU format is
                                   defined as in ISO 7816 Part 4:
                                   Cla Ins P1 P2 [lc] [Data] [le] ---
  uint32  dwClientData;        --- Data passed in by the client if
                                   exists ---
}
AEECardAPDUCmd;

=======================================================================
=======================================================================
=======================================================================
Interface Name: ICardNotifier

Description:
  The ICard Notifier allows applications to register for notifications
  of changes in the SIM card's status and in changes for a particular
  PIN status on the current SIM card (ie, the PIN is verified).
  Applications should use ISHELL_RegisterNotify() to register for
  event callbacks.

// Notification Masks...
// Users can register for this notification via the INOTIFIER interface.
#define NMASK_CARD_STATUS           0x0001   The CARD status has changed.
#define NMASK_PIN1_STATUS           0x0002   The Global PIN1 has changed.
#define NMASK_PIN2_STATUS           0x0004   The Global PIN2 has changed.
#define NMASK_PIN1_SLOT2_STATUS     0x0010   The Global PIN1 for Slot 2 has changed.
#define NMASK_PIN2_SLOT2_STATUS     0x0020   The Global PIN2 for Slot 2 has changed.
#define NMASK_CARD_SLOT2_STATUS     0x0040   The CARD status for Slot 2 has changed.
#define NMASK_CARD_PERSO_STATUS     0x0080   The Card personalization status for slot 1 has changed.
#define NMASK_CARD_PERSO_SLOT2_STATUS  0x0100   The Card personalization status for slot 2 has changed.

The following defines are used to communicate the card status
from AEECARD to the Applications.  When the application requests to
be notified of a change in card state, the pdata field in AEENotify
structure will contain a pointer to a uint8 containing the values
specified below.  These are identical to the states defined in
ICard above.

AEECARD_NO_CARD       -- Indicates No card is inserted.
AEECARD_INVALID_CARD  -- Indicates an Unusable Card is inserted.
AEECARD_VALID_CARD    -- Indicates a Usable card is present.
AEECARD_NOT_INIT      -- Indicates OEMCard has not fully initialized yet.
                         A notification will follow with updated information.
                         Request the Card status information after the
                         notifcation or use the info from the notification.
AEECARD_NOT_READY     -- Indicates OEMCard has not received any notification
                         from MMGSDI yet.  The card status is initialized to
                         this value on power up.
AEECARD_CARD_REINIT   -- Indicates OEMCard has received notification that the
                         SIM Initialization procedures are about to be executed
                         again.  Must send notify CM that service is not available.
AEECARD_AVAIL_CARD    -- Indicates OEMCard has received a notification that the
                         card is presented but no provisioning information will
                         be coming from that card.


The following defins are for Personalizaton States
AEECARD_PERSO_NO_EVENT                     -- No event is being generated
AEECARD_PERSO_NETWORK_FAIL                 -- Network Personalization failed during power up
AEECARD_PERSO_NETWORK_SUBSET_FAIL          -- Network Subset Personalization failed during power up
AEECARD_PERSO_SERVICE_PROVIDER_FAIL        -- Service Provider Personalization failed during power up
AEECARD_PERSO_CORPORATE_FAIL               -- Corporate Personalization failed during power up
AEECARD_PERSO_SIM_FAIL                     -- SIM/USIM Personalization failed during power up
AEECARD_PERSO_NETWORK_DEACTIVATE           -- Network Personalization is Deactivated successfully
AEECARD_PERSO_NETWORK_SUBSET_DEACTIVATE    -- Network Subset Personalization is Deactivated successfully
AEECARD_PERSO_SERVICE_PROVIDER_DEACTIVATE  -- Service Provider Personalization is Deactivated successfully
AEECARD_PERSO_CORPORATE_DEACTIVATE         -- Corporate Personalization is Deactivated successfully
AEECARD_PERSO_SIM_DEACTIVATE               -- SIM/USIM Personalization is Deactivated successfully
AEECARD_PERSO_NETWORK_CONTROL_BLOCKED      -- Network Personalization Control Key is blocked
AEECARD_PERSO_NETWORK_SUBSET_BLOCKED       -- Network Subset Personalization Control Key is blocked
AEECARD_PERSO_SERVICE_PROVIDER_BLOCKED     -- Service Provider Personalization Control Key is blocked
AEECARD_PERSO_CORPORATE_BLOCKED            -- Corporate Personalization Control Key is blocked
AEECARD_PERSO_SIM_BLOCKED                  -- SIM/USIM Personalization Control Key is blocked
AEECARD_PERSO_FAILED                       -- Initial Personalization Powerup Sequence Failed


The following defines indicate the various Personalization feature indicator levels
AEECARD_NW_PERSO_FEATURE                   -- Network Personalization
AEECARD_NW_SUBSET_PERSO_FEATURE            -- Network Subset Personalization
AEECARD_SERVICE_PROVIDER_PERSO_FEATURE     -- Service Provider Personalization
AEECARD_CORPORATE_PERSO_FEATURE            -- Corporate Personalization
AEECARD_SIM_USIM_PERSO_FEATURE             -- SIM/USIM Personalization

typedef enum
{
   AEECARD_NONE
   AEECARD_CHV1_DISABLE,    -- CHV1 disabled function
   AEECARD_ADN,             -- GSM Abbreviated Dialling Numbers
   AEECARD_FDN,             -- Forbidden Dialling Numbers
   AEECARD_LND,             -- Last Number Dialled
   AEECARD_BDN,             -- Barred Dialing Numbers
   AEECARD_LOCAL_PHONEBOOK, -- Local Phonebook, UADF/Phonebook/ADN
   AEECARD_GLOBAL_PHONEBOOK,-- Global Phonebook on USIM (always present)
   AEECARD_AOC,             -- Advice of Charge
   AEECARD_OCI_OCT,         -- Outgoing Call information/timer
   AEECARD_ICI_ICT,         -- Incoming call information/timer

   AEECARD_CCP,             -- Configuration Capability Params
   AEECARD_EXT_CCP,         -- extended capability configuration param - in GSM
   AEECARD_MSISDN,          -- MSISDN
   AEECARD_EXT1,            -- Extension 1
   AEECARD_EXT2,            -- Extension 2
   AEECARD_EXT3,            -- Extension 3
   AEECARD_EXT4,            -- Extension 4
   AEECARD_EXT5,            -- Extension 5
   AEECARD_EXT8,            -- Extension 8
   AEECARD_SDN,             -- Service Dialling Numbers

   AEECARD_SMS,             -- Short Message Services
   AEECARD_SMSP,            -- SMS Parameters
   AEECARD_SMSR,            -- Short Message Status Reports
   AEECARD_GID1,            -- Group Identifier 1
   AEECARD_GID2,            -- Group Identifier 2
   AEECARD_SPN,             -- Service Provider Name
   AEECARD_VGCS_GID_LIST,   -- VGCS Group Identifier List
   AEECARD_VBS_GID_LIST,    -- VBS Group Identifier List
   AEECARD_ENH_ML_SVC,      -- Enhanced Multi Level precedence and Premption Service
   AEECARD_AA_EMLPP,        -- Automatic Answer for eMLPP

   AEECARD_DATA_DL_SMSCB,   -- Data download via SMS-CB
   AEECARD_DATA_DL_SMSPP,   -- Data download via SMS-PP
   AEECARD_MENU_SEL,        -- Menu Selection
   AEECARD_CALL_CONTROL,    -- Call Control
   AEECARD_MOSMS_CONTROL,   -- MO SMS control
   AEECARD_PROACTIVE_SIM,   -- Proactive SIM
   AEECARD_USSD,            -- USSD String Data object supported in Call Control
   AEECARD_RUN_AT_CMD,      -- RUN AT COMMAND command
   AEECARD_GPRS_USIM,       -- Call control on GPRS by USIM
   AEECARD_NETWK_ALTERING_MS, -- Network's Indication of alerting in the MS

   AEECARD_CBMID_RANGES,    -- Cell Broadcast Message Identifier Ranges
   AEECARD_CBMID,           -- Cell Broadcast Message Identifier
   AEECARD_DEPERSON_KEYS,   -- Depersonalization Keys
   AEECARD_COOP_NETWK_LIST, -- Co-operative Network List
   AEECARD_GPRS,            -- GPRS
   AEECARD_IMAGE,           -- Image (IMG)
   AEECARD_SOLSA,           -- Support of Local Service Area
   AEECARD_SP_DISP_INFO,    -- Service Provider Display Information
   AEECARD_GSM_ACCESS_IN_USIM, -- USIM ADF Access to USIM files for GSM Roaming
   AEECARD_GSM_SECURITY_CONTEXT, -- GSM Security Context

   AEECARD_PLMN_SELECTOR,  -- PLMN Selector
   AEECARD_UPLMN_SEL_WACT, -- User controlled PLMN Selector with Access Technology
   AEECARD_RPLMN_LACT,     -- RPLMN Last used Access Technology
   AEECARD_OPLMN_SEL_WACT, -- Operator controlled PLMN Selector with Access Technology
   AEECARD_HPLMN_WACT,     -- HPLMN Selector with Access Technology
   AEECARD_OPLMN_LIST,     -- Operator PLMN list
   AEECARD_PLMN_NTWRK_NAME,-- PLMN Network Name
   AEECARD_MMS,            -- Multimedia Messaging Service
   AEECARD_MMS_USR_P,      -- MMS User Connectivity Parameters
   AEECARD_EST,            -- Enable Service Table in USIM

   AEECARD_ACL,            -- APN Control list
   AEECARD_CPBCCH,         -- CP BCCH
   AEECARD_INV_SCAN,       -- Investigation Scan
   AEECARD_MEXE,           -- MExE info
   AEECARD_MAILBOX_DIAL,   -- Mailbox dialling numbers
   AEECARD_MSG_WAIT,       -- Message Wait indication
   AEECARD_CALL_FWD_IND,   -- Call Forward indication status
}
AEECardServiceEnumType;


--- The following structure is used to define the status of a PIN.
--- It is used when the application has registered to be notified
--- when the PIN status is updated.  In that case, the mask indicates
--- which PIN and this structure is sent as the pdata field in the
--- AEENotify structure.   See the INOTIFIER interface for details.
typedef struct
{
  uint8 lock_state;           --- Lock state of this PIN ---
  uint8 tries_remaining;      --- Number of unlock tries remaining. ---
} AEECardPinStatus;

The lock states for pin codes are identical to the ICard interface
above and are:
AEECARD_PIN_UNKNOWN        PIN does not make sense (ie, no CARD)
AEECARD_PIN_DISABLED       PIN is not required.
AEECARD_PIN_ENABLED        PIN is required.
AEECARD_PIN_VERIFIED       PIN is required.
AEECARD_PIN_BLOCKED        Pin requires Unlock Key.
AEECARD_PIN_PERM_DISABLED  Pin cannot be Unblocked.
AEECARD_PIN_NOT_VALID      PIN functionality not supported.

--- The following structure indicates the Personalization Feature Information
--- for setting the Personalization data
typedef struct
{
  int   nPersoFeatureInd;  --- Which Personalization Feature Level
  int   nSize;             --- Size of the data
  AECHAR *pPersoData;      --- Personalization data.  The data is represented
                               in AECHAR format.  Multiple entry is separated
                               by space
}
AEECardPersoFeatureInfo;

--- The following structure is the command response structure for
--- Activate and Deactivate Personalization
typedef struct
{
  int   nCmdStatus;              --- SUCCESS/EFAILED/etc
  int   nSlotID;                 --- Slot ID for the action that is completed
  int   nPersoFeatureID;         --- Personalization Feature Indicator
  uint8 byRetries;               --- Number of retries
}
AEECardDeactivatePersoCmdStatus;

--- The following structure is the command response structure for
--- Get Feature indication
typedef struct
{
  int               nCmdStatus;
  int               nSlotID;       --- Slot ID for the action that is completed
  boolean           bNWIndStatus;  --- Network Indicator Status
  boolean           bNSIndStatus;  --- NW Subset Indicator Status
  boolean           bSPIndStatus;  --- Servic Provider Ind Status
  boolean           bCPIndStatus;  --- Corporate Ind Status
  boolean           bSIMIndStatus; --- SIM Indicator Status
  boolean           bProp1IndStatus; --- Propertory Indicator Status
}
AEECardGetPersoFeatureCmdStatus;

--- The following structure is used to indicate whether a service
--- is supported by the SIM/USIM or RUIM
typedef struct
{
  boolean bGSMSvcOn;
  boolean bCDMASvcOn;
  boolean bUMTSSvcOn;
} AEECardServiceIndType;

=======================================================================

Function: ICARD_GetCardStatus()

Description:
   This method is obtains the current status of the SIM/USIM/RUIM device.
Applications can also choose to register to be notified to receive updated
CardStatus information on any changes via the ISHELL_RegisterNotify function.

Prototype:

   int ICARD_GetCardStatus(ICard * po, uint8 * ps);

Parameters:
   po: [Input] Pointer to the ICard object
   ps: [Input/Output] Pointer to status information to be filled.
  *ps is set to one of the following:
    AEECARD_NO_CARD            No card is inserted.
    AEECARD_INVALID_CARD       Unusable Card is inserted.
    AEECARD_VALID_CARD         The card is now usable.
    AEECARD_NOT_INIT           Temporary condition indicating ICard is
                               still determining the state of the card or
                               GSDI is still initializing.  If the user
                               registered for the notifications, an event
                               will be generated at the completion of this
                               process.
    AEECARD_CARD_REINIT        Indicates OEMCard has received notification that the
                               SIM Initialization procedures are about to be executed
                               again.  Must send notify CM that service is not available.
    AEECARD_AVAIL_CARD         Indicates OEMCard has received a notification that the
                               card is presented but no provisioning information will
                               be coming from that card.

Return Value:

  SUCCESS - Status information is valid
  EBADPARM -  Bad parameter

Comments:
   None

Side Effects:
   None

See Also:
   None
=======================================================================

FUNCTION: ICARD_GetPinStatus

DESCRIPTION
  Gets the status of the requested PIN.  The PIN status is stored in OEM
  cache, thus we just get the information out of the local variable.

DEPENDENCIES
  none

Prototype:

   int ICARD_GetPinStatus(ICard * po, uint8 pin_id,
                          AEECardPinStatus *pPinStatus);

Parameters:
   po: [Input] Pointer to the ICard object
   pin_id: [Input] Which AEEPIN
   pPinStatus: [Input/Output] Pointer to status information to be filled.

RETURN VALUE
  SUCCESS -  if we understand the pin_id.
  EBADPARM - if there was an error in an input parameter.

SIDE EFFECTS
  None
=======================================================================

Function: ICARD_VerifyPin()

Description:
   This method is called to verify the specified PIN.  This function
   should only be called when the PIN itself is enabled.  If a PIN is
   disabled, this function will return an error.

Prototype:

   int ICARD_VerifyPin(ICard * po, uint8 pin_id, AEECardPinCode *pPin,
                        AEECardPinCmdStatus *pCmdStatus, AEECallback *pUserCB );

Parameters:
   po:         [Input] Pointer to the ICard object
   pin_id:     [Input] Which PIN we are interested in.
   pPin:       [Input] The PIN code we are attempting to verify.
   pCmdStatus: [Input] On callback contains the status of the command
   pUserCB:      [Input] Call back function which indicates results.


Return Value:
   SUCCESS - Status information is valid
   EBADPARM -  Bad parameter
   EFAILED - Unable to process the command
   EUNSUPPORTED - This command is not supported

Comments:
   None.

Side Effects:
   If the Verify succeeds and the PIN was locked, a status event is
   sent to interested applications indicating the change.  If the
   verify fails, there is a possibility the PIN may become blocked.
   If that is the case, the status event is sent indicating that change.

See Also:
   None
=======================================================================

Function: ICARD_UnblockPin()

Description:
   If the PIN has been blocked, this method is used to unblock the PIN.
   In this case, a Pin Unlock Key (PUK) along with a new PIN are required.

Prototype:

   int ICARD_UnblockPin(ICard * po, uint8 pin_id, AEECardPinCode *pPuk,
                         AEECardPinCode *pPin, AEECardPinCmdStatus *pCmdStatus,
                         AEECallback *pUserCB);

Parameters:
   po:         [Input] Pointer to the ICard object
   pin_id:     [Input] Indicates which PIN is of interest.
   pPuk:       [Input] The PUK we are attempting to verify.
   pPin:       [Input] The new PIN code which will be set if the PUK passes.
   pCmdStatus: [Input] On callback contains the status of the command
   pUserCB:      [Input] Callback function to indicate the results of the
                       function.

Return Value:
   SUCCESS - Status information is valid
   EBADPARM -  Bad parameter
   EFAILED - Unable to process the command
   EUNSUPPORTED - This command is not supported

Comments:
   None

Side Effects:
   If the command succeeds, the PIN will be changed to the pin supplied
   in pin_string.  If the command fails, the PIN may become permanently
   disabled, in which case, a notification will be sent to all registered
   clients.

See Also:
   None
=======================================================================

Function: ICARD_ChangePin()

Description:
   This method is used to change the specified PIN.
   If the command succeeds the PIN is changed.  If the command
   fails, there is a chance the PIN may become blocked.  If that
   happens, the notification is sent to all registered users.

Prototype:

   int ICARD_ChangePin(ICard * po, uint8 pin_id, AEECardPinCode *pPin,
                        AEECardPinCode *pNew_pin, AEECardPinCmdStatus *pCmdStatus,
                        AEECallback *pUserCB);

Parameters:
   po:        [Input] Pointer to the ICard object
   pin_id:    [Input] Which PIN we are interested in changing.
   pPin:      [Input] The old value for the pin being changed.
   pNew_pin:  [Input] The new value for the pin being changed.
   pCmdStatus:[Input] On callback contains the status of the command
   pUserCB:     [Input] User function to be called at completion.

Return Value:
   SUCCESS - Status information is valid
   EBADPARM -  Bad parameter
   EFAILED - Unable to process the command
   EUNSUPPORTED - This command is not supported

Comments:
   None

Side Effects:
   None

See Also:
   None
=======================================================================
Function: ICARD_EnablePin()

Description:
   This method is used to have the SIM enable the associated
   PIN.

Prototype:

   int ICARD_EnablePin(ICard *po, uint8 pin_id, AEECardPinCode *pPin,
                        AEECardPinCmdStatus *pCmdStatus, AEECallback *pUserCB);

Parameters:
   po:        [Input] Pointer to the ICard object
   pin_id:    [Input] Which PIN we are interested in enabling.
   pPin:       [Input] The value for the pin being enabled.
   pCmdStatus:[Input] On callback contains the status of the command.
   pUserCB:     [Input] User function to be called at completion.

Return Value:
   SUCCESS - Status information is valid
   EBADPARM -  Bad parameter
   EFAILED - Unable to process the command
   EUNSUPPORTED - This command is not supported

Comments:
   None

Side Effects:
   None

See Also:
   None
=======================================================================
Function: ICARD_DisablePin()

Description:
   This method is used to have the SIM disable the associated
   PIN.

Prototype:

   int ICARD_DisablePin(ICard * po, uint8 pin_id, AEECardPinCode *pPin,
                         AEECardPinCmdStatus *pCmdStatus, AEECallback *pUserCB);

Parameters:
   po:        [Input] Pointer to the ICard object
   pin_id:    [Input] Which PIN we are interested in disabling.
   pPin:       [Input] The value for the pin being disabled.
   pCmdStatus:[Input] On callback contains the status of the command.
   pUserCB:     [Input] User function to be called at completion.

Return Value:
   SUCCESS - Status information is valid
   EBADPARM -  Bad parameter
   EFAILED - Unable to process the command
   EUNSUPPORTED - This command is not supported

Comments:
   None

Side Effects:
   None

See Also:
   None
=============================================================================
Function:  ICARD_GetCardID()

Description:
  This function returns the identification number of the card.

Prototype:
   int ICARD_GetCardID(ICard *pICard, char *pID, int *pnLen)

Parameters:
   pICard:  [in].  Pointer to the ICard Interface object.
   pID:  [in/out].  Card ID.  If set to NULL when called, then pnLen will
                    contain the required size of the ID when the function
                    returns.
   pnLen:  [in/out].  If pID is NULL when this function is called, pnLen will
                      return the number of bytes required to hold the entire ID
                      (including NULL termination).  Otherwise, pnLen should be
                      set to the number of bytes requested and will return the
                      number of bytes actually provided.

Return Value:
   AEE_SUCCESS if no problem occurred, EFAILED otherwise.

Comments:
   None.

Side Effects:
   None.

See Also:
   Return to the List of functions
=============================================================================
FUNCTION: ICARD_GetCardApps()

DESCRIPTION
  Query GSDI for the inserted card's applications/DF presented.

Prototype:
   int ICARD_GetCardApps(ICard *pICard, AEECardApp *pCardApp)

Parameters:
   pICard:  [in].  Pointer to the ICard Interface object.
   pCardApp:  [in/out].  Card Applications existed on the Card.

RETURN VALUE
  SUCCESS if all went well, EBADPARM if there was a parameter error.
  EUNSUPPORTED is there is no MMGSDI

Comments:
   None.

Side Effects:
   None.

See Also:
   Return to the List of functions

===========================================================================
Function: ICARD_GetCardStatusSlot()

Description:
   This method is obtains the current status of the SIM/USIM/RUIM device
   at the indiated card slot from the UE.

Prototype:

   int   ICARD_GetCardStatusSlot(ICard * po, uint8 nSlot, uint8 * ps);

Parameters:
   po: [Input] Pointer to the ICard object
   wSlot: [Input] Indicates which slot the card status is to be retrieved from
   ps: [Input/Output] Pointer to status information to be filled.
  *ps is set to one of the following:
    AEECARD_NO_CARD            No card is inserted.
    AEECARD_INVALID_CARD       Unusable Card is inserted.
    AEECARD_VALID_CARD         The card is now usable.
    AEECARD_NOT_INIT           Temporary condition indicating ICARD is
                               still determining the state of the card or
                               GSDI is still initializing.  If the user
                               registered for the notifications, an event
                               will be generated at the completion of this
                               process.
    AEECARD_CARD_REINIT        Indicates OEMCard has received notification that the
                               SIM Initialization procedures are about to be executed
                               again.  Must send notify CM that service is not available.
    AEECARD_AVAIL_CARD         Indicates OEMCard has received a notification that the
                               card is presented but no provisioning information will
                               be coming from that card.

Return Value:

  SUCCESS - Status information is valid
  EBADPARM -  Bad parameter

Comments:
   None

Side Effects:
   None

See Also:
   None
===========================================================================
Function: ICARD_EnableFDNSlot

Description:
  Send the pin 2 information to GSDI in order to enable the FDN entries

Prototype:

   int   ICARD_EnableFDNSlot(ICard *po, uint8 wSlot, boolean bPresent, AEECardPinCode *pPin,
                                  AEECardGenericCmdStatus *pCmdStatus, AEECallback *pUserCB);

Parameters:
   po: [Input] Pointer to the ICARD object
   wSlot: [Input] Indicates which slot the card status is to be retrieved from
   bPresent: [Input] Indicates if pin2 value is presented in this function
                     call or not
   pPin:       [Input] The value for the pin2.
   pCmdStatus: [Input] On callback contains the status of the command
   pUserCB:      [Input] Call back function which indicates results.


Return Value:
   SUCCESS - Status information is valid
   EBADPARM -  Bad parameter
   EFAILED - Unable to process the command
   EUNSUPPORTED - This command is not supported

Comments:
   None

Side Effects:
   None

See Also:
   None
===========================================================================
Function: ICARD_DisableFDNSlot

Description:
  Send the pin 2 information to GSDI in order to disable the FDN entries

Prototype:

   int   ICARD_DisableFDNSlot(ICard *po, uint8 wSlot, boolean bPresent, AEECardPinCode *pPin,
                                  AEECardGenericCmdStatus *pCmdStatus, AEECallback *pUserCB);

Parameters:
   po: [Input] Pointer to the ICARD object
   wSlot: [Input] Indicates which slot the card status is to be retrieved from
   bPresent: [Input] Indicates if pin2 value is presented in this function
                     call or not
   pPin:       [Input] The value for the pin2.
   pCmdStatus: [Input] On callback contains the status of the command
   pUserCB:      [Input] Call back function which indicates results.

Return Value:
   SUCCESS - Status information is valid
   EBADPARM -  Bad parameter
   EFAILED - Unable to process the command
   EUNSUPPORTED - This command is not supported

Comments:
   None

Side Effects:
   None

See Also:
   None
===========================================================================
FUNCTION: ICARD_GetPINStatus

DESCRIPTION
  Gets the status of the requested PIN.  The PIN status is stored in OEM
  cache, thus we just get the information out of the local variable.

DEPENDENCIES
  none

Prototype:

   int    ICARD_GetPinStatus(ICard * po, uint8 pin_id,
                          AEECardPinStatus *pPinStatus);

Parameters:
   po: [Input] Pointer to the ICard object
   pin_id: [Input] Which AEEPIN
   pPinStatus: [Input/Output] Pointer to status information to be filled.

RETURN VALUE
  SUCCESS -  if we understand the pin_id.
  EBADPARM - if there was an error in an input parameter.

SIDE EFFECTS
  None
=======================================================================

Function: ICARD_GetSimCapabilitySlot()

Description:
   This method returns whether the FDN/BDN/ACL has been enabled
   and whether IMSI has been invalidated or not.

Prototype:

   int   ICARD_GetSimCapabilitySlot(ICard * po, uint8 wSlot,
                         AEESimCapabilities *pSimCap);

Parameters:
   po        : [Input] Pointer to the ICard object
   wSlot     : [Input] Which slot is requested by the user
   pSimCap   : [Input/Output] Sim Capabilities for the requested slot


Return Value:
  SUCCESS - Function is in progress
  EBADPARM -  Invalid number

Comments:
   None.

Side Effects:
   None

See Also:
   None

=======================================================================

Function: ICARD_ChangeUserSlotPref()

Description:
   This method changes the current user slot preference setting to
   the input value.

Prototype:

   int   ICARD_ChangeUserSlotPref(ICard *po, uint8 wNewSlot, uint8 wApp,
                                  AEECardGenericCmdStatus *pCmdStatus, AEECallback *pUserCB);


Parameters:
   po        : [Input] Pointer to the ICARD object
   wNewSlot  : [Input] Which slot the user wants to change to
   wApp      : [Input] Which technology the user wants the slot change
                       to be performed on
   pCmdStatus: [Input] On callback contains the status of the command
   pUserCB:      [Input] Call back function which indicates results.

Return Value:
   SUCCESS - Status information is valid
   EBADPARM -  Bad parameter
   EFAILED - Unable to process the command
   EUNSUPPORTED - This command is not supported

Comments:
   None.

Side Effects:
   None

See Also:
   None

=======================================================================

Function: ICARD_GetUserSlotPref()

Description:
   This method returns the current user slot preference setting.

Prototype:

   int   ICARD_GetUserSlotPref(ICard *po, uint8 wApp, uint8* pwSlot);


Parameters:
   po        : [Input] Pointer to the ICard object
   wApp      : [Input] Which technology the user wants to get the slot
                       preference from
   pwSlot    : [Input/Output] Which slot is the user current preference
                              slot setting for the indicated technology

Return Value:
   SUCCESS - Status information is valid
   EBADPARM -  Bad parameter
   EFAILED - Unable to process the command
   EUNSUPPORTED - This command is not supported

Comments:
   None.

Side Effects:
   None

See Also:
   None

=======================================================================

FUNCTION: ICARD_GetATRSize

DESCRIPTION
  returns the size of the SIM's ATR

Prototype:

   int   ICARD_GetATRSize(ICard *po, uint8 wSlot, int *nSize)

Parameters:
   po        : [Input] Pointer to the ICard object
   wSlot     : [Input] Which slot is the user current preference
                       slot setting for the indicated technology
   nSize     : [Input/Output] size of the ATR


Return Value:
  SUCCESS - Status information is valid, nSize will indicate whether there
            is any ATR present.  nSize == 0 indicates that there is no
            card present
  EBADPARM -  Bad parameter
  EFAILED - Request made before ICard could cache the ATR
  EUNSUPPORTED - This command is not supported

Comments:
   None.

Side Effects:
   None

See Also:
   None
===========================================================================

FUNCTION: ICARD_GetATR

DESCRIPTION
  returns the SIM's ATR

Prototype:

   int   ICARD_GetATR(ICard *po, uint8 wSlot, AEECardATR *pATR)

Parameters:
   po        : [Input] Pointer to the ICard object
   wSlot     : [Input] Which slot is the user current preference
                       slot setting for the indicated technology
   pATR      : [Input/Output] ATR.
                pATR->nSize will be updated to the
                MIN [sizeof(pATR->pATR), ATR size returned by card]

Return Value:
  SUCCESS - Status information is valid
  EBADPARM -  Bad parameter
  EFAILED - Unable to process the command
  EUNSUPPORTED - This command is not supported

Comments:
   None.

Side Effects:
   None

See Also:
   None
===========================================================================

FUNCTION: ICARDCONNECTION_Open

DESCRIPTION
  Open a logical channel at the specific slot

Prototype:

   int   ICARDCONNECTION_Open(ICardConnection *po, uint8 wSlotID,
                              AEECardConnectionOpenStatus *pCmdStatus,
                              AEECallback *pUserCB)

Parameters:
   po        : [Input] Pointer to the ICardConnection object
   wSlotID   : [Input] Which slot is the user current preference
                       slot setting for the indicated technology
   pCmdStatus: [Input/Output] Cmd status to be filled when the process
                       is finished by lower layer
   pUserCB   : [Input/Output] Application callback to be called when
                           the Open Channel is performed

Return Value:
  SUCCESS - Status information is valid
  EBADPARM -  Bad parameter
  EFAILED - Unable to process the command
  EUNSUPPORTED - This command is not supported

Comments:
   None.

Side Effects:
   None

See Also:
   None
===========================================================================


FUNCTION: ICARDCONNECTION_Close

DESCRIPTION
  Close a logical channel at the specific slot

Prototype:

   int   ICARDCONNECTION_Close(ICardConnection *po,
                               AEECardConnectionCloseStatus *pCmdStatus,
                              AEECallback *pUserCB)

Parameters:
   po        : [Input] Pointer to the ICardConnection object
   wSlotID   : [Input] Which slot is the user current preference
                       slot setting for the indicated technology
   pCmdStatus: [Input/Output] Cmd status to be filled when the process
                       is finished by lower layer
   pUserCB   : [Input/Output] Application callback to be called when
                           the Close Channel is performed

Return Value:
  SUCCESS - Status information is valid
  EBADPARM -  Bad parameter
  EFAILED - Unable to process the command
  EUNSUPPORTED - This command is not supported

Comments:
   None.

Side Effects:
   None

See Also:
   None
===========================================================================


FUNCTION: ICARDCONNECTION_SendAPDU

DESCRIPTION
  Send the APDU using the connection object

Prototype:

   int   ICARDCONNECTION_SendAPDU(ICardConnection *po, AEECardAPDUCmd *pAPDU,
                         uint32 dwClientData, AEECardConnectionCmdStatus *pCmdStatus,
                         AEECallback *pUserCB)

Parameters:
   po          : [Input] Pointer to the ICardConnection object
   pAPDU       : [Input] the APDU to be sent to the card
   dwClientData: [Input] client user data
   pCmdStatus  : [Input/Output] Cmd status to be filled when the process
                       is finished by lower layer
   pUserCB     : [Input/Output] Application callback to be called when
                           the Send APDU is performed

Return Value:
  SUCCESS - Status information is valid
  EBADPARM -  Bad parameter
  EFAILED - Unable to process the command
  EUNSUPPORTED - This command is not supported
  ECMDDISALLOWED - ENVELOPE, MANAGE CHANNEL, GET RESPONSE, FETCH, TERMINAL PROFILE
                   TERMINAL RESPONSE are not allowed by this function

Comments:
   None.

Side Effects:
   None

See Also:
   None

===========================================================================


FUNCTION: ICARDCONNECTION_GetAPDURsp

DESCRIPTION
  Get the APDU response data as a result of a previous SendAPDU

Prototype:

   int   ICARDCONNECTION_GetAPDURsp(ICardConnection *po, uint32 dwSeqNum,
                                      AEECardAPDUResp *pAPDU)

Parameters:
   po        : [Input] Pointer to the ICardConnection object
   dwSeqNum  : [Input] the Sequence number for the APDU response that application
                       is trying to get.  This is being returned to the
                       application in the user callback processing of
                       ICARDCONNECTION_SendAPDU function call
   pAPDU     : [Input/Output] pointer to APDU response.


Return Value:
  SUCCESS - Status information is valid
  EBADPARM -  Bad parameter
  EFAILED - Unable to process the command
  EUNSUPPORTED - This command is not supported

Comments:
   None.

Side Effects:
   None

See Also:
   None


===========================================================================
FUNCTION: ICARD_ActivatePersoFeatureInd

DESCRIPTION
  Activate the personalization feature

Prototype:

   int   ICARD_ActivatePersoFeatureInd(ICard *po, uint8 wSlot, int nFeatureID,
                 AEECardPinCode *pKey, AEECardDeactivatePersoCmdStatus *pCmdStatus,
                 AEECallback *pUserCB)

Parameters:
   po        : [Input] Pointer to the ICard object
   wSlot     : [Input] Slot to which to operate on
   nFeatureID: [Input] Which personalization feature to which the activation is
                       to be performed
   pKey      : [Input] Key value
   pCmdStatus: [Input/Output] Command response status
   pUserCB   : [Input/Output] User Callback


Return Value:
  SUCCESS - Status information is valid
  EBADPARM -  Bad parameter
  EFAILED - Unable to process the command
  EUNSUPPORTED - This command is not supported

Comments:
   None.

Side Effects:
   None

See Also:
   None

===========================================================================
FUNCTION: ICARD_DeactivatePersoFeatureInd

DESCRIPTION
  Deactivate the personalization feature

Prototype:

   int   ICARD_DeactivatePersoFeatureInd(ICard *po, uint8 wSlot, int nFeatureID,
                 AEECardPinCode *pKey, AEECardDeactivatePersoCmdStatus *pCmdStatus,
                 AEECallback *pUserCB)

Parameters:
   po        : [Input] Pointer to the ICard object
   wSlot     : [Input] Slot to which to operate on
   nFeatureID: [Input] Which personalization feature to which the deactivation is
                       to be performed
   pKey      : [Input] Key value
   pCmdStatus: [Input/Output] Command response status
   pUserCB   : [Input/Output] User Callback

Return Value:
  SUCCESS - Status information is valid
  EBADPARM -  Bad parameter
  EFAILED - Unable to process the command
  EUNSUPPORTED - This command is not supported

Comments:
   None.

Side Effects:
   None

See Also:
   None

===========================================================================
FUNCTION: ICARD_GetPersoFeatureInd

DESCRIPTION
  Get the Personalization feature indicator

Prototype:

   int   ICARD_GetPersoFeatureInd(ICard *po, uint8 wSlot,
                                  AEECardGetPersoFeatureCmdStatus *pCmdStatus,
                                  AEECallback *pUserCB)

Parameters:
   po        : [Input] Pointer to the ICard object
   wSlot     : [Input] Slot to which to operate on
   pCmdStatus: [Input/Output] Command response status
   pUserCB   : [Input/Output] User Callback

Return Value:
  SUCCESS - Status information is valid
  EBADPARM -  Bad parameter
  EFAILED - Unable to process the command
  EUNSUPPORTED - This command is not supported

Comments:
   None.

Side Effects:
   None

See Also:
   None

===========================================================================
FUNCTION: ICARD_SetPersoFeatureData

DESCRIPTION
  Set the Personalization feature indicator

Prototype:

   int   ICARD_SetPersoFeatureData(ICard *po, uint8 wSlot,
                                   AEECardPersoFeatureInfo *pFeatureInd,
                                   AEECardGenericCmdStatus *pCmdStatus, AEECallback *pUserCB)
Parameters:
   po        : [Input] Pointer to the ICard object
   wSlot     : [Input] Slot to which to operate on
   pFeatureInd:[Input] Feature Data
   pCmdStatus: [Input/Output] Command response status
   pUserCB   : [Input]Output] User Callback


Return Value:
  SUCCESS - Status information is valid
  EBADPARM -  Bad parameter
  EFAILED - Unable to process the command
  EUNSUPPORTED - This command is not supported

Comments:
   None.

Side Effects:
   None

See Also:
   None

===========================================================================
FUNCTION: ICARD_GetPersoStatus()

Description:
   This method is obtains the current perso status of the SIM/USIM/RUIM device
   at the indiated card slot from the UE.

Prototype:

   int   ICARD_GetPersoStatus(ICard *po, uint8 bySlot, uint8 *pPersoStatus)

Parameters:
   po        : [Input] Pointer to the ICard object
   bySlot    : [Input] Slot to which to operate on
   pPersoStatus:[Input/Output] Pointer to status information to be filled.



Return Value:
  SUCCESS - Status information is valid
  EBADPARM -  Bad parameter
  EFAILED - Unable to process the command
  EUNSUPPORTED - This command is not supported

Comments:
   None.

Side Effects:
   None

See Also:
   None

===========================================================================
FUNCTION: ICARD_GetServiceAvailableInd()

Description:
   This method is obtains the service indicaton that the client request.  It
   indicates if the service is available in UMTS/GSM/CDMA

Prototype:

   int   ICARD_GetServiceAvailableInd(ICard *po, uint8 bySlot,
                                      AEECardServiceEnumType dwService,
                                      AEECardServiceIndType *pSvcInd)

Parameters:
   po: [Input] Pointer to the ICard object
   bySlot: [Input] Indicates which slot the card status is to be retrieved from
   dwService: [input] Indicates the service that is being query for
   pSvcInd: [Input/Output] Pointer to result of the service indication.  Caller
                           should only read this data if the return status is
                           SUCCESS

Return Value:
  SUCCESS - Status information is valid
  EBADPARM -  Bad parameter
  EUNSUPPORT - Not supported

Comments:
   None.

Side Effects:
   None

See Also:
   None

===========================================================================
FUNCTION: ICARD_GetFileAttributes()

Description:
  This method provides the service to get the Attribute information on a
  file or directory on the SIM/USIM

Prototype:

  int   ICARD_GetFileAttributes(ICard *po,
                                uint8 bySlot,
                                AEECardFilePathType sFilePath,
                                AEECardFileAttribStatus *pCmdStatus,
                                AEECallback *pUserCB);

Parameters:
  po        : [Input] Pointer to the ICard object
  bySlot    : [Input] Indicates which slot the GetFileAttributes access is to be
                      retrieved from
  sFilePath : [Input] Indicates the file of interest
  pCmdStatus: [Input/Output] Command response status
  pUserCB   : [Input/Output] User Callback

Return Value:
  SUCCESS - Status information is valid
  EBADPARM -  Bad parameter
  EUNSUPPORT - Not supported

Comments:
  Data read is returned in Asynchronous fashion.  In the pUserCB, ICard
  will populate pCmdStatus with the file attribute information

Side Effects:
   None

See Also:
   None

===========================================================================
FUNCTION: ICARD_ReadRecord()

Description:
  This function provides the Read Record API to the client

Prototype:

  int   ICARD_ReadRecord(ICard *po,
                         uint8 bySlot,
                         AEECardFilePathType sFilePath,
                         int nRecordNum,
                         AEECardGenericCmdStatus *pCmdStatus,
                         AEECallback *pUserCB);

Parameters:
  po        : [Input] Pointer to the ICard object
  bySlot    : [Input] Indicates which slot the access is to be
                      retrieved from
  sFilePath : [Input] Indicates the file of interest
  nRecordNum: [Input] The record to be read (nRecordNum has to be greater than 0)
  pCmdStatus: [Input/Output] Command response status
  pUserCB   : [Input/Output] User Callback

Return Value:
  SUCCESS - Status information is valid
  EBADPARM -  Bad parameter
  EUNSUPPORT - Not supported

Comments:
  Data read is returned in Asynchronous fashion.  In the pUserCB, ICard
  will indicate if the process is Success/Fail.  In case of Success, client
  is required to use ICARD_GetRespData to retrieve the Record Data

Side Effects:
   None

See Also:
   None

===========================================================================
FUNCTION: ICARD_ReadBinary()

Description:
  This function provides the Read Binary File API to the client

Prototype:

  int   ICARD_ReadBinary(ICard *po,
                         uint8 bySlot,
                         AEECardFilePathType sFilePath,
                         int nStartOffset,
                         int nNumBytes,
                         AEECardGenericCmdStatus *pCmdStatus,
                         AEECallback *pUserCB);

Parameters:
  po          : [Input] Pointer to the ICard object
  bySlot      : [Input] Indicates which slot the access is to be
                        retrieved from
  sFilePath   : [Input] Indicates the file of interest
  nStartOffset: [Input] The start offset of the read
  nNumbytes   : [Input] Number of bytes to be read
  pCmdStatus  : [Input/Output] Command response status
  pUserCB     : [Input/Output] User Callback

Return Value:
  SUCCESS - Status information is valid
  EBADPARM -  Bad parameter
  EUNSUPPORT - Not supported

Comments:
  Data read is returned in Asynchronous fashion.  In the pUserCB, ICard
  will indicate if the process is Success/Fail.  In case of Success, client
  is required to use ICARD_GetRespData to retrieve the Binary Data

Side Effects:
   None

See Also:
   None



===========================================================================
FUNCTION: ICARD_WriteRecord()

Description:
  This function provides the Write Record File API to the client

Prototype:

  int   ICARD_WriteRecord(ICard *po,
                          uint8 bySlot,
                          AEECardFilePathType sFilePath,
                          int nRecordNum,
                          AEECardDataBufType *pWrittenData,
                          AEECardGenericCmdStatus *pCmdStatus,
                          AEECallback *pUserCB);

Parameters:
  po          : [Input] Pointer to the ICard object
  bySlot      : [Input] Indicates which slot the access is to be
                        retrieved from
  sFilePath   : [Input] Indicates the file of interest
  nRecordNum  : [Input] The record to be read (nRecordNum has to be greater than 0)
  pWrittenData: [Input] Data to be written to the Card
  pCmdStatus  : [Input/Output] Command response status
  pUserCB     : [Input/Output] User Callback

Return Value:
  SUCCESS - Status information is valid
  EBADPARM -  Bad parameter
  EUNSUPPORT - Not supported

Comments:
  Data written is returned in Asynchronous fashion.  In the pUserCB, ICard
  will indicate if the process is Success/Fail.

Side Effects:
   None

See Also:
   None

===========================================================================
FUNCTION: ICARD_WriteBinary()

Description:
  This function provides the Write Binary File API to the client

Prototype:

  int   ICARD_WriteBinary(ICard *po,
                          uint8 bySlot,
                          AEECardFilePathType sFilePath,
                          int nStartOffset,
                          AEECardDataBufType *pWrittenData,
                          AEECardGenericCmdStatus *pCmdStatus,
                          AEECallback *pUserCB);

Parameters:
  po          : [Input] Pointer to the ICard object
  bySlot      : [Input] Indicates which slot the access is to be
                        retrieved from
  sFilePath   : [Input] Indicates the file of interest
  nStartOffset: [Input] The offset location that the data is to be written to
  pWrittenData: [Input] Data to be written to the Card
  pCmdStatus  : [Input/Output] Command response status
  pUserCB     : [Input/Output] User Callback

Return Value:
  SUCCESS - Status information is valid
  EBADPARM -  Bad parameter
  EUNSUPPORT - Not supported

Comments:
  Data written is returned in Asynchronous fashion.  In the pUserCB, ICard
  will indicate if the process is Success/Fail.

Side Effects:
   None

See Also:
   None

===========================================================================
FUNCTION: ICARD_GetCPHSInfo()

Description:
  This function returns the CPHS information to user

Prototype:

  int   ICARD_GetCPHSInfo(ICard *po,
                          uint8 bySlot,
                          AEECardCPHSStatus *pCmdStatus,
                          AEECallback *pUserCB);

Parameters:
  po          : [Input] Pointer to the ICard object
  bySlot      : [Input] Indicates which slot the access is to be
                        retrieved from
  pCmdStatus  : [Input/Output] Command response status
  pUserCB     : [Input/Output] User Callback

Return Value:
  SUCCESS - Status information is valid
  EBADPARM -  Bad parameter
  EUNSUPPORT - Not supported

Comments:
  Data read is returned in Asynchronous fashion.  In the pUserCB, ICard
  will populate the pCmdStatus

Side Effects:
   None

See Also:
   None

===========================================================================
FUNCTION: ICARD_GetRespData()

Description:
  This function returns the response data to user

Prototype:

  int   ICARD_GetRespData(ICard *po,
                          uint32 dwSeqNum,
                          AEECardDataBufType *pData);

Parameters:
  po          : [Input] Pointer to the ICard object
  dwSeqNum    : [Input] Sequence number of the data to be retrieved
  pData       : [Input/Output] Data

Return Value:
  SUCCESS - Status information is valid
  EBADPARM -  Bad parameter
  EUNSUPPORT - Not supported

Comments:
  Data read is returned in Synchronous fashion.  The dwSeqNum is returned in the
  dwData member of the AEECardGenericCmdStatus structure

  if (pData->pDataLen == NULL)
  { function call will return error }
  if (pData->pData == NULL)
  { function call will return size of data available for copying }
  if (pData->pData != NULL)
  { ICard will copy *(pData->pDataLen) amount of data to the pData->pData buffer }
    if ICard has less than *(pData->pDataLen) amount of data, it will copy the
    minumum number of bytes into the pData->pData buffer
    *(pData->pDataLen) upon return of a function will reflect the exact number
    of bytes that are being copied to the application

Side Effects:
   None

See Also:
   None

===========================================================================

FUNCTION: ICARD_GetATRAsync

DESCRIPTION
  returns the SIM's ATR Asnchronously

Prototype:

   int   ICARD_GetATRAsync(ICard *po, uint8 wSlot, AEECardATR *pATR, AEECallback *pUserCB)

Parameters:
   po        : [Input] Pointer to the ICard object
   wSlot     : [Input] Which slot is the user current preference
                       slot setting for the indicated technology
   pATR      : [Input/Output] ATR.
                pATR->nSize will be updated to the
                MIN [sizeof(pATR->pATR), ATR size returned by card]
                The original size will be provided by the client but if it 
                is different from the size of card, then it will be updated.
   pUserCB   : AEECallback

Return Value:
  SUCCESS - Status information is valid
  EBADPARM -  Bad parameter
  EFAILED - Unable to process the command
  EUNSUPPORTED - This command is not supported

Comments:
   None.

Side Effects:
   None

See Also:
   None
===========================================================================*/

#endif    /* AEECARD_H */
