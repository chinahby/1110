/*=============================================================================

FILE: OEMCARDSESSION.c

SERVICES: OEM Functions for Caard Session Interface.

GENERAL DESCRIPTION:

PUBLIC CLASSES AND STATIC FUNCTIONS:

INITIALIZATION AND SEQUENCING REQUIREMENTS:

        Copyright c 2006 - 2009 QUALCOMM Incorporated.
               All Rights Reserved.
            QUALCOMM Proprietary/GTDR
=============================================================================*/
/*=============================================================================

$Header: //source/qcom/qct/modem/uim/icardsession/rel/07H1_2/src/OEMCardSession.c#3 $

                      EDIT HISTORY FOR FILE

  This section contains comments describing changes made to the module.
  Notice that changes are listed in reverse chronological order.

when         who     what, where, why
----------   ---     ---------------------------------------------------------
06/19/09     rn       Added support for accessing EF-CFF - Call forwarding EF.
03/11/09     nb       Fixed Compilation Warning
12/02/08     yk       Added more ADNs for 1000 phonebook support 
11/05/08     kk       Updated pfnCancel pointer to NULL in the callback pointer
                      during Cancel callback.
10/01/08     nb       Lint fixes
06/21/08     kk       Added PKCS15 Support
04/30/08     kk       Added support for MMS EFs
04/08/08     sun      Only do Client Deregistration after closing sessions
03/28/08     nk       Added OMH EF Support
03/27/08     kk       Added support for handling AUTS failure during ISIM AUTH
02/22/08     kk       handled the client dereg when session model not created
01/17/08     nk       Added support for change me esn flag for JCDMA Card color
                      cnf 
01/04/08     nk       Fixed Compiler Error
12/20/07     nk       Fixed compiler warning
12/03/07     sk       Fixed compilation error
11/30/07     sk       Added support for NAA Refresh
11/20/07     sun      Removed any app related data from ICardSession_Release
11/08/07     vs       Added OMH feature support
09/27/07     tml      Lint fix and ensure cmd status is populated in
                      error cases
09/18/07     tml      Added MFLO support
06/01/07     nk       Added support for JCDMA card type
06/01/07     nk       Fixed write record transparent args to mmgsdi
05/07/07     sun      Added support for Srv Available and USIM Auth
03/07/07     sun      Added support for Disable with Replacement
03/07/07     nk       Fixed compile error
03/01/07     nk       Fixed error in client registration
03/01/07     nk       Fixed Compilation Errors
02/27/07     nk       Added Registration API
02/27/07     sun      Added Support for Read/Write by Path
02/07/07     nk,tml   Fixed slot error, Added MAC failure check in authenticate,
                      get file attribute fix added support for 
                      mmgsdi_single_allowed
01/11/07     tml      Add ISIM EFs supports
08/09/06     SUN      Assign the object Handler to the Cancel function
07/26/06     tml      lint fixes
07/19/06     SUN      Fixed Compile Problems for RPC builds
05/19/06     SUN      Fixed Featurization
05/19/06     SUN      Initial Revision
=============================================================================*/


/*===========================================================================

                     INCLUDE FILES

===========================================================================*/
#include "customer.h"
#include "comdef.h"     // Get DMSS type declarations.
#include "err.h"        // Error functions
#include "OEMCardSession.h"
#include "OEMCardSessionNotifier.h"
#include "OEMCardSessionModel.h"

#ifdef FEATURE_ICARDSESSION_IMODEL
#include "SysModelBase_priv.h"
#endif /* FEATURE_ICARDSESSION_IMODEL */
#include "AEEModel.h"

#ifdef FEATURE_MMGSDI
#include "mmgsdilib.h"
#endif /* FEATURE_MMGSDI */
#include "tmc.h"
#include "intconv.h"

#include "AEECARDSESSION.bid"
#include "AEECARDSESSION_MODEL.bid"
#include "AEECARDSESSION_SLOT2.BID"
#include "AEECriticalSection.h"
#ifdef FEATURE_MMGSDI
static AEECriticalSection gOEMCardCriticalSection={0};
#define OEMCARDSESSION_ENTER_CRITICAL_SECTION    OEMCriticalSection_Enter(&gOEMCardCriticalSection);
#define OEMCARDSESSION_LEAVE_CRITICAL_SECTION    OEMCriticalSection_Leave(&gOEMCardCriticalSection);
#endif /* FEATURE_MMGSDI */

/* Card Reader related Mask */
#define OEMCARDSESSION_CARD_READER_ID_MASK          0x03
#define OEMCARDSESSION_CARD_READER_INFO_MASK        0x01
#define OEMCARDSESSION_CARD_READER_REMOVABLE_SHIFT  0x03
#define OEMCARDSESSION_CARD_READER_PRESENT_SHIFT    0x04
#define OEMCARDSESSION_CARD_READER_ID1SIZE_SHIFT    0x05
#define OEMCARDSESSION_CARD_PRESENT_IN_READER_SHIFT 0x06
#define OEMCARDSESSION_CARD_POWERED_IN_READER_SHIFT 0x07

/*===========================================================================

                    LOCAL/STATIC DATA

===========================================================================*/

ICardSessionCore     *ICardSessionCoreObj      = NULL;

/*===========================================================================

                            INTERFACE FUNCTION TABLE

===========================================================================*/

/* The function supported by CardSession object. */
static const AEEVTBL(ICardSession) gOEMCardSessionFuncs = {
  OEMCardSession_AddRef,
  OEMCardSession_Release,
  OEMCardSession_QueryInterface,
  OEMCardSession_Open,
  OEMCardSession_Close,
  OEMCardSession_Authenticate,
  OEMCardSession_ReadTransparent,
  OEMCardSession_ReadRecord,
  OEMCardSession_WriteTransparent,
  OEMCardSession_WriteRecord,
  OEMCardSession_GetFileAttr,
  OEMCardSession_GetCardAllPinsInfo,
  OEMCardSession_VerifyPin,
  OEMCardSession_EnablePin,
  OEMCardSession_DisablePin,
  OEMCardSession_UnblockPin,
  OEMCardSession_ChangePin,
  OEMCardSession_SelectSubscription,
  OEMCardSession_ReadTransparentByPath,
  OEMCardSession_ReadRecordByPath,
  OEMCardSession_WriteTransparentByPath,
  OEMCardSession_WriteRecordByPath,
  OEMCardSession_Register,
  OEMCardSession_DisablePinExt,
  OEMCardSession_IsServiceAvailable,
  OEMCardSession_USIMAuthenticate,
  OEMCardSession_JCDMACard_GetInfo,
  OEMCardSession_MFLOInitActParams,
  OEMCardSession_MFLOGetSubscriberID,
  OEMCardSession_MFLOGetPublicKey,
  OEMCardSession_MFLOSignUcast,
  OEMCardSession_MFLOVerifyUcast,
  OEMCardSession_GetATR,
  OEMCardSession_GetCardReaderStatus,
  OEMCardSession_SendAPDU,
  OEMCardSession_RegisterForRefresh,
  OEMCardSession_DeRegisterForRefresh,
  OEMCardSession_RefreshComplete,
  OEMCardSession_OkToRefresh,
  OEMCardSession_CreatePkcs15LookupTable
};


#ifdef FEATURE_MMGSDI
OEMCardSessionMMGSDIFileEnumTableType OEMCardSessionMMGSDIFileEnumTable[] =
{
   /* No Other Special Sequencing here just add new ENUMs in Groups of Ten */
   /* EFs at the MF level */
  {AEECARDSESSION_ICCID,                            MMGSDI_ICCID},                            /* ICCID */
  {AEECARDSESSION_ELP,                              MMGSDI_ELP},                              /* Extended Language Preference */
  {AEECARDSESSION_DIR,                              MMGSDI_DIR},                              /* EF DIR for USIM */
  {AEECARDSESSION_ARR,                              MMGSDI_ARR},                              /* Access rule reference */

   /* EFs in CDMA DF*/
  {AEECARDSESSION_CDMA_CC,                          MMGSDI_CDMA_CC},                          /* Call Count */
  {AEECARDSESSION_CDMA_IMSI_M,                      MMGSDI_CDMA_IMSI_M},                      /* IMSI_M */
  {AEECARDSESSION_CDMA_IMSI_T,                      MMGSDI_CDMA_IMSI_T},                      /* IMSI_T */
  {AEECARDSESSION_CDMA_TMSI,                        MMGSDI_CDMA_TMSI},                        /* TMSI */
  {AEECARDSESSION_CDMA_ANALOG_HOME_SID,             MMGSDI_CDMA_ANALOG_HOME_SID},             /* Analog Home SID */
  {AEECARDSESSION_CDMA_ANALOG_OP_PARAMS,            MMGSDI_CDMA_ANALOG_OP_PARAMS},            /* Analog Operational Parameters */
  {AEECARDSESSION_CDMA_ANALOG_LOCN_AND_REGN_IND,    MMGSDI_CDMA_ANALOG_LOCN_AND_REGN_IND},    /* Analog Locn and Regn Indicators */
  {AEECARDSESSION_CDMA_HOME_SID_NID,                MMGSDI_CDMA_HOME_SID_NID},                /* CDMA Home SID and NID */
  {AEECARDSESSION_CDMA_ZONE_BASED_REGN_IND,         MMGSDI_CDMA_ZONE_BASED_REGN_IND},         /* CDMA Zone Based Regn Indicators */
  {AEECARDSESSION_CDMA_SYS_REGN_IND,                MMGSDI_CDMA_SYS_REGN_IND},                /* CDMA System/Network Regn Indicators */

  {AEECARDSESSION_CDMA_DIST_BASED_REGN_IND,         MMGSDI_CDMA_DIST_BASED_REGN_IND},         /* CDMA Distance Based Regn Indicators */
  {AEECARDSESSION_CDMA_ACCOLC,                      MMGSDI_CDMA_ACCOLC},                      /* Access Overload Class */
  {AEECARDSESSION_CDMA_CALL_TERM_MODE_PREF,         MMGSDI_CDMA_CALL_TERM_MODE_PREF},         /* Call Termination Mode Preferences */
  {AEECARDSESSION_CDMA_SCI,                         MMGSDI_CDMA_SCI},                         /* Suggested Slot Cycle Index */
  {AEECARDSESSION_CDMA_ANALOG_CHAN_PREF,            MMGSDI_CDMA_ANALOG_CHAN_PREF},            /* Analog Channel Preferences */
  {AEECARDSESSION_CDMA_PRL,                         MMGSDI_CDMA_PRL},                         /* Preferred Roaming List */
  {AEECARDSESSION_CDMA_RUIM_ID,                     MMGSDI_CDMA_RUIM_ID},                     /* Removable UIM ID */
  {AEECARDSESSION_CDMA_CDMA_SVC_TABLE,              MMGSDI_CDMA_SVC_TABLE},                   /* CDMA Service Table */
  {AEECARDSESSION_CDMA_SPC,                         MMGSDI_CDMA_SPC},                         /* Service Programming Code */
  {AEECARDSESSION_CDMA_OTAPA_SPC_ENABLE,            MMGSDI_CDMA_OTAPA_SPC_ENABLE},            /* OTAPA/SPC-Enable */

  {AEECARDSESSION_CDMA_NAM_LOCK,                    MMGSDI_CDMA_NAM_LOCK},                    /* NAM-Lock */
  {AEECARDSESSION_CDMA_OTASP_OTAPA_FEATURES,        MMGSDI_CDMA_OTASP_OTAPA_FEATURES},        /* OTASP-OTAPA Parameters */
  {AEECARDSESSION_CDMA_SERVICE_PREF,                MMGSDI_CDMA_SERVICE_PREF},                /* Service Preferences */
  {AEECARDSESSION_CDMA_ESN_ME,                      MMGSDI_CDMA_ESN_ME},                      /* ESN_ME */
  {AEECARDSESSION_CDMA_RUIM_PHASE,                  MMGSDI_CDMA_RUIM_PHASE},                  /* R-UIM Phase */
  {AEECARDSESSION_CDMA_PREF_LANG,                   MMGSDI_CDMA_PREF_LANG},                   /* Preferred Languages */
  {AEECARDSESSION_CDMA_UNASSIGNED_1,                MMGSDI_CDMA_UNASSIGNED_1},                /* Not assigned yet */
  {AEECARDSESSION_CDMA_SMS,                         MMGSDI_CDMA_SMS},                         /* Short Messages */
  {AEECARDSESSION_CDMA_SMS_PARAMS,                  MMGSDI_CDMA_SMS_PARAMS},                  /* Short Message Service Parameters */
  {AEECARDSESSION_CDMA_SMS_STATUS,                  MMGSDI_CDMA_SMS_STATUS},                  /* SMS Status */

  {AEECARDSESSION_CDMA_SUP_SVCS_FEATURE_CODE_TABLE, MMGSDI_CDMA_SUP_SVCS_FEATURE_CODE_TABLE}, /* Sup Services Feature Code Table */
  {AEECARDSESSION_CDMA_UNASSIGNED_2,                MMGSDI_CDMA_UNASSIGNED_2},                /* Not assigned yet */
  {AEECARDSESSION_CDMA_HOME_SVC_PVDR_NAME,          MMGSDI_CDMA_HOME_SVC_PVDR_NAME},          /* CDMA Home Service Provider Name */
  {AEECARDSESSION_CDMA_UIM_ID_USAGE_IND,            MMGSDI_CDMA_UIM_ID_USAGE_IND},            /* R-UIM ID usage indicator */
  {AEECARDSESSION_CDMA_ADM_DATA,                    MMGSDI_CDMA_ADM_DATA},                    /* CDMA Administrative Data */
  {AEECARDSESSION_CDMA_MSISDN,                      MMGSDI_CDMA_MSISDN},                      /* Mobile Directory Number */
  {AEECARDSESSION_CDMA_MAXIMUM_PRL,                 MMGSDI_CDMA_MAXIMUM_PRL},                 /* Max size of the PRL */
  {AEECARDSESSION_CDMA_SPC_STATUS,                  MMGSDI_CDMA_SPC_STATUS},                  /* SPC Status */
  {AEECARDSESSION_CDMA_ECC,                         MMGSDI_CDMA_ECC},                         /* Emergency Call Codes */
  {AEECARDSESSION_CDMA_3GPD_ME3GPDOPC,              MMGSDI_CDMA_3GPD_ME3GPDOPC},              /* 3GPD Operational Capabilities */

  {AEECARDSESSION_CDMA_3GPD_3GPDOPM,                MMGSDI_CDMA_3GPD_3GPDOPM},                /* 3GPD Operational Mode */
  {AEECARDSESSION_CDMA_3GPD_SIPCAP,                 MMGSDI_CDMA_3GPD_SIPCAP},                 /* 3GPD Simple IP Capabilities */
  {AEECARDSESSION_CDMA_3GPD_MIPCAP,                 MMGSDI_CDMA_3GPD_MIPCAP},                 /* 3GPD Mobile IP Capabilities */
  {AEECARDSESSION_CDMA_3GPD_SIPUPP,                 MMGSDI_CDMA_3GPD_SIPUPP},                 /* 3GPD Simple IP User Profile Parameters */
  {AEECARDSESSION_CDMA_3GPD_MIPUPP,                 MMGSDI_CDMA_3GPD_MIPUPP},                 /* 3GPD Mobile IP User Profile Parameters */
  {AEECARDSESSION_CDMA_3GPD_SIPSP,                  MMGSDI_CDMA_3GPD_SIPSP},                  /* 3GPD Simple IP Status Parameters */
  {AEECARDSESSION_CDMA_3GPD_MIPSP,                  MMGSDI_CDMA_3GPD_MIPSP},                  /* 3GPD Mobile IP Status Parameters */
  {AEECARDSESSION_CDMA_3GPD_SIPPAPSS,               MMGSDI_CDMA_3GPD_SIPPAPSS},               /* 3GPD Simple IP PAP SS Parameters */
  {AEECARDSESSION_CDMA_UNASSIGNED_3,                MMGSDI_CDMA_UNASSIGNED_3},                /* Reserved */
  {AEECARDSESSION_CDMA_UNASSIGNED_4,                MMGSDI_CDMA_UNASSIGNED_4},                /* Reserved */

  {AEECARDSESSION_CDMA_PUZL,                        MMGSDI_CDMA_PUZL},                        /* Preffered User Zone List */
  {AEECARDSESSION_CDMA_MAXPUZL,                     MMGSDI_CDMA_MAXPUZL},                     /* Maximum PUZL */
  {AEECARDSESSION_CDMA_MECRP,                       MMGSDI_CDMA_MECRP},                       /* ME-specific Config REquest Param */
  {AEECARDSESSION_CDMA_HRPDCAP,                     MMGSDI_CDMA_HRPDCAP},                     /* HRPD Access Auth Capability Param */
  {AEECARDSESSION_CDMA_HRPDUPP,                     MMGSDI_CDMA_HRPDUPP},                     /* HRPD Access Auth User Profile Param */
  {AEECARDSESSION_CDMA_CSSPR,                       MMGSDI_CDMA_CSSPR},                       /* CUR_SSPR_P_REV */
  {AEECARDSESSION_CDMA_ATC,                         MMGSDI_CDMA_ATC},                         /* Acceess Terminal Class */
  {AEECARDSESSION_CDMA_EPRL,                        MMGSDI_CDMA_EPRL},                        /* Extended Preffered Roaming List */
  {AEECARDSESSION_CDMA_BCSMS_CONFIG,                MMGSDI_CDMA_BCSMS_CONFIG},                /* Broadcast SMS Configuration */
  {AEECARDSESSION_CDMA_BCSMS_PREF,                  MMGSDI_CDMA_BCSMS_PREF},                  /* Broadcast SMS Preferences */

  {AEECARDSESSION_CDMA_BCSMS_TABLE,                 MMGSDI_CDMA_BCSMS_TABLE},                 /* Broadcast SMS Table */
  {AEECARDSESSION_CDMA_BCSMS_PARAMS,                MMGSDI_CDMA_BCSMS_PARAMS},                /* Broadcast SMS Paramaters */
  {AEECARDSESSION_CDMA_MMS_NOTIF,                   MMGSDI_CDMA_MMS_NOTIF},                   /* MMS Notification */
  {AEECARDSESSION_CDMA_MMS_EXT8,                    MMGSDI_CDMA_MMS_EXT8},                    /* MMS Extension 8 */
  {AEECARDSESSION_CDMA_MMS_ICP,                     MMGSDI_CDMA_MMS_ICP},                     /* MMS Issuer Connectivity Parameters */
  {AEECARDSESSION_CDMA_MMS_UP,                      MMGSDI_CDMA_MMS_UP},                      /* MMS User Preferences */
  {AEECARDSESSION_CDMA_SMS_CAP,                     MMGSDI_CDMA_SMS_CAP},                     /* SMS Capabilities */
  {AEECARDSESSION_CDMA_3GPD_IPV6CAP,                MMGSDI_CDMA_3GPD_IPV6CAP},                /* IPv6 Capabilities */
  {AEECARDSESSION_CDMA_3GPD_MIPFLAGS,               MMGSDI_CDMA_3GPD_MIPFLAGS},               /* Mobile IP Flags */
  {AEECARDSESSION_CDMA_3GPD_TCPCONFIG,              MMGSDI_CDMA_3GPD_TCPCONFIG},              /* TCP Configurations */

  {AEECARDSESSION_CDMA_3GPD_DGC,                    MMGSDI_CDMA_3GPD_DGC},                    /* Data Generic Configurations */
  {AEECARDSESSION_CDMA_BROWSER_CP,                  MMGSDI_CDMA_BROWSER_CP},                  /* Browser Connectivity Parameters */
  {AEECARDSESSION_CDMA_BROWSER_BM,                  MMGSDI_CDMA_BROWSER_BM},                  /* Browser Bookmarks */
  {AEECARDSESSION_CDMA_3GPD_SIPUPPEXT,              MMGSDI_CDMA_3GPD_SIPUPPEXT},              /* Simple IP User Profile Parameters Extension */
  {AEECARDSESSION_CDMA_MMS_CONFIG,                  MMGSDI_CDMA_MMS_CONFIG},                  /* MMS Configuration */
  {AEECARDSESSION_CDMA_JAVA_DURL,                   MMGSDI_CDMA_JAVA_DURL},                   /* Java Download URL */
  {AEECARDSESSION_CDMA_3GPD_MIPUPPEXT,              MMGSDI_CDMA_3GPD_MIPUPPEXT},              /* Mobile IP User Profile Parameters Extension */
  {AEECARDSESSION_CDMA_BREW_DLOAD,                  MMGSDI_CDMA_BREW_DLOAD},                  /* BREW Download */
  {AEECARDSESSION_CDMA_BREW_TSID,                   MMGSDI_CDMA_BREW_TSID},                   /* BREW Teleservice ID */
  {AEECARDSESSION_CDMA_BREW_SID,                    MMGSDI_CDMA_BREW_SID},                    /* BREW Subscriber ID */

  {AEECARDSESSION_CDMA_LBS_XCONFIG,                 MMGSDI_CDMA_LBS_XCONFIG},                 /* LBS XTRA Configuration */
  {AEECARDSESSION_CDMA_LBS_XSURL,                   MMGSDI_CDMA_LBS_XSURL},                   /* LBS XTRA Server URLs */
  {AEECARDSESSION_CDMA_LBS_V2CONFIG,                MMGSDI_CDMA_LBS_V2CONFIG},                /* LBS V2 Configuration */
  {AEECARDSESSION_CDMA_LBS_V2PDEADDR,               MMGSDI_CDMA_LBS_V2PDEADDR},               /* LBS V2 PDE Address */
  {AEECARDSESSION_CDMA_LBS_V2MPCADDR,               MMGSDI_CDMA_LBS_V2MPCADDR},               /* LBS V2 MPC Address */
  {AEECARDSESSION_CDMA_BREW_AEP,                    MMGSDI_CDMA_BREW_AEP},             	      /* BREW Application Execution Policy */
  {AEECARDSESSION_CDMA_MODEL,                       MMGSDI_CDMA_MODEL},              	      /* Model Info */
  {AEECARDSESSION_CDMA_RC,                          MMGSDI_CDMA_RC},                   	      /* Root Certificate */
  {AEECARDSESSION_CDMA_APP_LABELS,                  MMGSDI_CDMA_APP_LABELS},           	      /* Applicaiton Labels */
  {AEECARDSESSION_CDMA_USER_AGENT_STRING,           MMGSDI_CDMA_USER_AGENT_STRING},           /* KDDI Agent String */

  {AEECARDSESSION_CDMA_GID2,                        MMGSDI_CDMA_GID2},                        /* KDDI GID2 */
  {AEECARDSESSION_CDMA_GID1,                        MMGSDI_CDMA_GID1},                        /* KDDI GID1 */
  {AEECARDSESSION_CDMA_GID_REFERENCES,              MMGSDI_CDMA_GID_REFERENCES},              /* KDDI GID References */
  {AEECARDSESSION_CDMA_IMSI_STATUS,                 MMGSDI_CDMA_IMSI_STATUS},                 /* KDDI IMSI Status */
  {AEECARDSESSION_CDMA_ME_DOWNLOADABLE_DATA,        MMGSDI_CDMA_ME_DOWNLOADABLE_DATA},        /* KDDI ME Downloadable Data */
  {AEECARDSESSION_CDMA_ME_SETTING_DATA,             MMGSDI_CDMA_ME_SETTING_DATA},             /* KDDI ME Setting Date */
  {AEECARDSESSION_CDMA_ME_USER_DATA,                MMGSDI_CDMA_ME_USER_DATA},                /* KDDI User Data */
  {AEECARDSESSION_CDMA_RESERVED2,                   MMGSDI_CDMA_RESERVED2},                   /* KDDI Reserved */
  {AEECARDSESSION_CDMA_RESERVED1,                   MMGSDI_CDMA_RESERVED1},                   /* KDDI Reserved */
  {AEECARDSESSION_CDMA_UIM_SVC_TABLE,               MMGSDI_CDMA_UIM_SVC_TABLE},               /* KDDI UIM Service Table */

  {AEECARDSESSION_CDMA_UIM_FEATURES,                MMGSDI_CDMA_UIM_FEATURES},                /* KDDI UIM Features */
  {AEECARDSESSION_CDMA_UIM_VERSION,                 MMGSDI_CDMA_UIM_VERSION},                 /* KDDI UIM Version */
  {AEECARDSESSION_CDMA_HRPD_HRPDUPP,                MMGSDI_CDMA_HRPD_HRPDUPP},                /* HRPD User Profile Parameters */
  {AEECARDSESSION_CDMA_SF_EUIM_ID,                  MMGSDI_CDMA_SF_EUIM_ID},                  /* Short Form EUIMID */
  /* EFs in DF GSM */
  {AEECARDSESSION_GSM_LP,                           MMGSDI_GSM_LP},                           /* Language Preference */
  {AEECARDSESSION_GSM_IMSI,                         MMGSDI_GSM_IMSI},                         /* IMSI */
  {AEECARDSESSION_GSM_KC,                           MMGSDI_GSM_KC},                           /* Ciphering Key Kc */
  {AEECARDSESSION_GSM_PLMN,                         MMGSDI_GSM_PLMN},                         /* PLMN selector */
  {AEECARDSESSION_GSM_HPLMN,                        MMGSDI_GSM_HPLMN},                        /* HPLMN search period */
  {AEECARDSESSION_GSM_ACM_MAX,                      MMGSDI_GSM_ACM_MAX},                      /* ACM Maximum value */

  {AEECARDSESSION_GSM_SST,                          MMGSDI_GSM_SST},                          /* SIM Service table */
  {AEECARDSESSION_GSM_ACM,                          MMGSDI_GSM_ACM},                          /* Accumulated call meter */
  {AEECARDSESSION_GSM_GID1,                         MMGSDI_GSM_GID1},                         /* Group Identifier Level 1 */
  {AEECARDSESSION_GSM_GID2,                         MMGSDI_GSM_GID2},                         /* Group Identifier Level 2 */
  {AEECARDSESSION_GSM_SPN,                          MMGSDI_GSM_SPN},                          /* Service Provider Name */
  {AEECARDSESSION_GSM_PUCT,                         MMGSDI_GSM_PUCT},                         /* Price Per Unit and currency table */
  {AEECARDSESSION_GSM_CBMI,                         MMGSDI_GSM_CBMI},                         /* Cell broadcast message identifier selection */
  {AEECARDSESSION_GSM_BCCH,                         MMGSDI_GSM_BCCH},                         /* Broadcast control channels */
  {AEECARDSESSION_GSM_ACC,                          MMGSDI_GSM_ACC},                          /* Access control class */
  {AEECARDSESSION_GSM_FPLMN,                        MMGSDI_GSM_FPLMN},                        /* Forbidden PLMNs */

  {AEECARDSESSION_GSM_LOCI,                         MMGSDI_GSM_LOCI},                         /* Location information */
  {AEECARDSESSION_GSM_AD,                           MMGSDI_GSM_AD},                           /* Administrative data */
  {AEECARDSESSION_GSM_PHASE,                        MMGSDI_GSM_PHASE},                        /* Phase identification */
  {AEECARDSESSION_GSM_VGCS,                         MMGSDI_GSM_VGCS},                         /* Voice Group Call service */
  {AEECARDSESSION_GSM_VGCSS,                        MMGSDI_GSM_VGCSS},                        /* Voice Group Call service status */
  {AEECARDSESSION_GSM_VBS,                          MMGSDI_GSM_VBS},                          /* Voice Broadcast service  */
  {AEECARDSESSION_GSM_VBSS,                         MMGSDI_GSM_VBSS},                         /* Voice Broadcast service status */
  {AEECARDSESSION_GSM_EMLPP,                        MMGSDI_GSM_EMLPP},                        /* Enhanced multi level pre-emption and priority */
  {AEECARDSESSION_GSM_AAEM,                         MMGSDI_GSM_AAEM},                         /* Automatic Answer for eMLPP service */
  {AEECARDSESSION_GSM_CBMID,                        MMGSDI_GSM_CBMID},                        /* Cell Broadcast Message id for data dload */

  {AEECARDSESSION_GSM_ECC,                          MMGSDI_GSM_ECC},                          /* Emergency Call Codes */
  {AEECARDSESSION_GSM_CBMIR,                        MMGSDI_GSM_CBMIR},                        /* Cell Broadcast Message id range selection */
  {AEECARDSESSION_GSM_DCK,                          MMGSDI_GSM_DCK},                          /* De-personalization control keys */
  {AEECARDSESSION_GSM_CNL,                          MMGSDI_GSM_CNL},                          /* Co-operative network list */
  {AEECARDSESSION_GSM_NIA,                          MMGSDI_GSM_NIA},                          /* Network's indication of alerting */
  {AEECARDSESSION_GSM_KCGPRS,                       MMGSDI_GSM_KCGPRS},                       /* GPRS ciphering key */
  {AEECARDSESSION_GSM_LOCIGPRS,                     MMGSDI_GSM_LOCIGPRS},                     /* GPRS location information */
  {AEECARDSESSION_GSM_SUME,                         MMGSDI_GSM_SUME},                         /* Setup Menu elements */
  {AEECARDSESSION_GSM_PLMNWACT,                     MMGSDI_GSM_PLMNWACT},                     /* PLMN Selector with Access technology */
  {AEECARDSESSION_GSM_OPLMNWACT,                    MMGSDI_GSM_OPLMNWACT},                    /* Operator controlled PLMNWACT */

  {AEECARDSESSION_GSM_HPLMNACT,                     MMGSDI_GSM_HPLMNACT},                     /* HPLNMN Access technology */
  {AEECARDSESSION_GSM_CPBCCH,                       MMGSDI_GSM_CPBCCH},                       /* CPBCCH information */
  {AEECARDSESSION_GSM_INVSCAN,                      MMGSDI_GSM_INVSCAN},                      /* Investigation PLMN Scan */
  {AEECARDSESSION_GSM_RPLMNAT,                      MMGSDI_GSM_RPLMNAT},                      /* RPLMN  Last used Access Technology */
  {AEECARDSESSION_GSM_VMWI,                         MMGSDI_GSM_VMWI},                         /* CPHS: Voice Mail Waiting Indicator */
  {AEECARDSESSION_GSM_SVC_STR_TBL,                  MMGSDI_GSM_SVC_STR_TBL},                  /* CPHS: Service String Table */
  {AEECARDSESSION_GSM_CFF,                          MMGSDI_GSM_CFF},                          /* CPHS: Call Forwarding Flag */
  {AEECARDSESSION_GSM_ONS,                          MMGSDI_GSM_ONS},                          /* CPHS: Operator Name String */
  {AEECARDSESSION_GSM_CSP,                          MMGSDI_GSM_CSP},                          /* CPHS: Customer Service Profile */
  {AEECARDSESSION_GSM_CPHSI,                        MMGSDI_GSM_CPHSI},                        /* CPHS: CPHS Information */

  {AEECARDSESSION_GSM_MN,                           MMGSDI_GSM_MN},                           /* CPHS: Mailbox Number */
  {AEECARDSESSION_SIM_7F40_PROP1_DFS,               MMGSDI_SIM_7F40_PROP1_DFS},                   /* Dynamic Flags Status */
  {AEECARDSESSION_SIM_7F40_PROP1_D2FS,              MMGSDI_SIM_7F40_PROP1_D2FS},                  /* Dynamic2 Flag Setting */
  {AEECARDSESSION_SIM_7F40_PROP1_CSP2,              MMGSDI_SIM_7F40_PROP1_CSP2},                  /* Customer Service Profile Line2*/
  /* EFs at SoLSA */
  {AEECARDSESSION_GSM_SAI,                          MMGSDI_GSM_SAI},                          /* SoLSA access indicator */
  {AEECARDSESSION_GSM_SLL,                          MMGSDI_GSM_SLL},                          /* SoLSA LSA list */
  /* EFs at MExE Level */
  {AEECARDSESSION_GSM_MEXE_ST,                      MMGSDI_GSM_MEXE_ST},                      /* MExE Service table */
  {AEECARDSESSION_GSM_ORPK,                         MMGSDI_GSM_ORPK},                         /* Operator Root Public Key */
  {AEECARDSESSION_GSM_ARPK,                         MMGSDI_GSM_ARPK},                         /* Administrator Root Public Key */
  {AEECARDSESSION_GSM_TPRPK,                        MMGSDI_GSM_TPRPK},                        /* Third party Root public key */
  
  {AEECARDSESSION_GSM_CFIS,                         MMGSDI_GSM_CFIS},                         /* GSM: Call Forwarding Indication status */
  /* image file */
  {AEECARDSESSION_GSM_IMG,                          MMGSDI_GSM_IMG},
  {AEECARDSESSION_GSM_IMG1INST1,                    MMGSDI_GSM_IMG1INST1},
  {AEECARDSESSION_GSM_IMG1INST2,                    MMGSDI_GSM_IMG1INST2},
  {AEECARDSESSION_GSM_IMG1INST3,                    MMGSDI_GSM_IMG1INST3},
  {AEECARDSESSION_GSM_IMG2INST1,                    MMGSDI_GSM_IMG2INST1},
  {AEECARDSESSION_GSM_IMG2INST2,                    MMGSDI_GSM_IMG2INST2},
  {AEECARDSESSION_GSM_IMG2INST3,                    MMGSDI_GSM_IMG2INST3},
  {AEECARDSESSION_GSM_IMG3INST1,                    MMGSDI_GSM_IMG1INST1},
  {AEECARDSESSION_GSM_IMG3INST2,                    MMGSDI_GSM_IMG2INST2},

  {AEECARDSESSION_GSM_IMG3INST3,                    MMGSDI_GSM_IMG3INST3},
  /* EFs at the telecom DF in GSM and CDMA */
  {AEECARDSESSION_TELECOM_ADN,                      MMGSDI_TELECOM_ADN},                      /* Abbrev Dialing Numbers */
  {AEECARDSESSION_TELECOM_FDN,                      MMGSDI_TELECOM_FDN},                      /* Fixed dialling numbers */
  {AEECARDSESSION_TELECOM_SMS,                      MMGSDI_TELECOM_SMS},                      /* Short messages */
  {AEECARDSESSION_TELECOM_CCP,                      MMGSDI_TELECOM_CCP},                      /* Capability Configuration Parameters */
  {AEECARDSESSION_TELECOM_ECCP,                     MMGSDI_TELECOM_ECCP},                     /* Extended CCP */
  {AEECARDSESSION_TELECOM_MSISDN,                   MMGSDI_TELECOM_MSISDN},                   /* MSISDN */
  {AEECARDSESSION_TELECOM_SMSP,                     MMGSDI_TELECOM_SMSP},                     /* SMS parameters */
  {AEECARDSESSION_TELECOM_SMSS,                     MMGSDI_TELECOM_SMSS},                     /* SMS Status */
  {AEECARDSESSION_TELECOM_LND,                      MMGSDI_TELECOM_LND},                      /* Last number dialled */

  {AEECARDSESSION_TELECOM_SDN,                      MMGSDI_TELECOM_SDN},                      /* Service Dialling numbers */
  {AEECARDSESSION_TELECOM_EXT1,                     MMGSDI_TELECOM_EXT1},                     /* Extension 1 */
  {AEECARDSESSION_TELECOM_EXT2,                     MMGSDI_TELECOM_EXT2},                     /* Extension 2 */
  {AEECARDSESSION_TELECOM_EXT3,                     MMGSDI_TELECOM_EXT3},                     /* Extension 3 */
  {AEECARDSESSION_TELECOM_BDN,                      MMGSDI_TELECOM_BDN},                      /* Barred Dialing Numbers */
  {AEECARDSESSION_TELECOM_EXT4,                     MMGSDI_TELECOM_EXT4},                     /* Extension 4 */
  {AEECARDSESSION_TELECOM_SMSR,                     MMGSDI_TELECOM_SMSR},                     /* SMS reports */
  {AEECARDSESSION_TELECOM_CMI,                      MMGSDI_TELECOM_CMI},                      /* Comparison Method Information */
  {AEECARDSESSION_TELECOM_SUME,                     MMGSDI_TELECOM_SUME},                     /* Setup Menu elements */
  {AEECARDSESSION_TELECOM_ARR,                      MMGSDI_TELECOM_ARR},                      /* Access Rule reference */

  /* EFs at the DF PHONEBOOK under  DF Telecom */
  {AEECARDSESSION_TELECOM_PBR,                      MMGSDI_TELECOM_PBR},                      /* Phone book reference file */
  {AEECARDSESSION_TELECOM_CCP1,                     MMGSDI_TELECOM_CCP1},                     /* Capability Configuration parameters 1 */
  {AEECARDSESSION_TELECOM_UID,                      MMGSDI_TELECOM_UID},                      /* Unique Identifier */
  {AEECARDSESSION_TELECOM_PSC,                      MMGSDI_TELECOM_PSC},                      /* Phone book synchronization center */
  {AEECARDSESSION_TELECOM_CC,                       MMGSDI_TELECOM_CC},                       /* Change counter */
  {AEECARDSESSION_TELECOM_PUID,                     MMGSDI_TELECOM_PUID},                     /* Previous Unique Identifier */
  /* EFs of USIM ADF */
  {AEECARDSESSION_USIM_LI,                          MMGSDI_USIM_LI},                          /* Language Indication */
  {AEECARDSESSION_USIM_IMSI,                        MMGSDI_USIM_IMSI},                        /* IMSI */
  {AEECARDSESSION_USIM_KEYS,                        MMGSDI_USIM_KEYS},                        /* Ciphering and Integrity keys */
  {AEECARDSESSION_USIM_KEYSPS,                      MMGSDI_USIM_KEYSPS},                      /* C and I keys for packet switched domain */

  {AEECARDSESSION_USIM_PLMNWACT,                    MMGSDI_USIM_PLMNWACT},                    /* User controlled PLMN selector with access tech */
  {AEECARDSESSION_USIM_UPLMNSEL,                    MMGSDI_USIM_UPLMNSEL},                    /* UPLMN selector */
  {AEECARDSESSION_USIM_HPLMN,                       MMGSDI_USIM_HPLMN},                       /* HPLMN search period */
  {AEECARDSESSION_USIM_ACM_MAX,                     MMGSDI_USIM_ACM_MAX},                     /* ACM maximum value */
  {AEECARDSESSION_USIM_UST,                         MMGSDI_USIM_UST},                         /* USIM Service table */
  {AEECARDSESSION_USIM_ACM,                         MMGSDI_USIM_ACM},                         /* Accumulated Call meter */
  {AEECARDSESSION_USIM_GID1,                        MMGSDI_USIM_GID1},                        /* Group Identifier Level  */
  {AEECARDSESSION_USIM_GID2,                        MMGSDI_USIM_GID2},                        /* Group Identifier Level 2 */
  {AEECARDSESSION_USIM_SPN,                         MMGSDI_USIM_SPN},                         /* Service Provider Name */
  {AEECARDSESSION_USIM_PUCT,                        MMGSDI_USIM_PUCT},                        /* Price Per Unit and Currency table */

  {AEECARDSESSION_USIM_CBMI,                        MMGSDI_USIM_CBMI},                        /* Cell Broadcast Message identifier selection */
  {AEECARDSESSION_USIM_ACC,                         MMGSDI_USIM_ACC},                         /* Access control class */
  {AEECARDSESSION_USIM_FPLMN,                       MMGSDI_USIM_FPLMN},                       /* Forbidden PLMNs */
  {AEECARDSESSION_USIM_LOCI,                        MMGSDI_USIM_LOCI},                        /* Location information */
  {AEECARDSESSION_USIM_AD,                          MMGSDI_USIM_AD},                          /* Administrative data */
  {AEECARDSESSION_USIM_CBMID,                       MMGSDI_USIM_CBMID},                       /* Cell Broadcast msg identifier for data download */
  {AEECARDSESSION_USIM_ECC,                         MMGSDI_USIM_ECC},                         /* Emergency call codes */
  {AEECARDSESSION_USIM_CBMIR,                       MMGSDI_USIM_CBMIR},                       /* Cell broadcast msg identifier range selection */
  {AEECARDSESSION_USIM_PSLOCI,                      MMGSDI_USIM_PSLOCI},                      /* Packet switched location information */
  {AEECARDSESSION_USIM_FDN,                         MMGSDI_USIM_FDN},                         /* Fixed dialling numbers */

  {AEECARDSESSION_USIM_SMS,                         MMGSDI_USIM_SMS},                         /* Short messages */
  {AEECARDSESSION_USIM_MSISDN,                      MMGSDI_USIM_MSISDN},                      /* MSISDN */
  {AEECARDSESSION_USIM_SMSP,                        MMGSDI_USIM_SMSP},                        /* SMS parameters */
  {AEECARDSESSION_USIM_SMSS,                        MMGSDI_USIM_SMSS},                        /* SMS Status */
  {AEECARDSESSION_USIM_SDN,                         MMGSDI_USIM_SDN},                         /* Service dialling numbers */
  {AEECARDSESSION_USIM_EXT2,                        MMGSDI_USIM_EXT2},                        /* Extension 2 */
  {AEECARDSESSION_USIM_EXT3,                        MMGSDI_USIM_EXT3},                        /* Extension 3 */
  {AEECARDSESSION_USIM_SMSR,                        MMGSDI_USIM_SMSR},                        /* SMS reports */
  {AEECARDSESSION_USIM_ICI,                         MMGSDI_USIM_ICI},                         /* Incoming call information */
  {AEECARDSESSION_USIM_OCI,                         MMGSDI_USIM_OCI},                         /* Outgoing call information */

  {AEECARDSESSION_USIM_ICT,                         MMGSDI_USIM_ICT},                         /* Incoming call timer */
  {AEECARDSESSION_USIM_OCT,                         MMGSDI_USIM_OCT},                         /* Outgoing call timer */
  {AEECARDSESSION_USIM_EXT5,                        MMGSDI_USIM_EXT5},                        /* Extension 5 */
  {AEECARDSESSION_USIM_CCP2,                        MMGSDI_USIM_CCP2},                        /* Capability Configuration Parameters 2 */
  {AEECARDSESSION_USIM_EMLPP,                       MMGSDI_USIM_EMLPP},                       /* Enhanced Multi Level Precedence and Priority */
  {AEECARDSESSION_USIM_AAEM,                        MMGSDI_USIM_AAEM},                        /* Automatic answer for eMLPP service */
  {AEECARDSESSION_USIM_GMSI,                        MMGSDI_USIM_GMSI},                        /* Group identity */
  {AEECARDSESSION_USIM_HIDDENKEY,                   MMGSDI_USIM_HIDDENKEY},                   /* key for hidden phonebook entries */
  {AEECARDSESSION_USIM_BDN,                         MMGSDI_USIM_BDN},                         /* Barred dialling numbers */
  {AEECARDSESSION_USIM_EXT4,                        MMGSDI_USIM_EXT4},                        /* Extension 4 */

  {AEECARDSESSION_USIM_CMI,                         MMGSDI_USIM_CMI},                         /* Comparison Method information */
  {AEECARDSESSION_USIM_EST,                         MMGSDI_USIM_EST},                         /* Enabled services table */
  {AEECARDSESSION_USIM_ACL,                         MMGSDI_USIM_ACL},                         /* Access Point Name Control List */
  {AEECARDSESSION_USIM_DCK,                         MMGSDI_USIM_DCK},                         /* De-personalization Control Keys */
  {AEECARDSESSION_USIM_CNL,                         MMGSDI_USIM_CNL},                         /* Co-operative network list */
  {AEECARDSESSION_USIM_START_HFN,                   MMGSDI_USIM_START_HFN},                   /* Init values for Hyper-frame number */
  {AEECARDSESSION_USIM_THRESHOLD,                   MMGSDI_USIM_THRESHOLD},                   /* Max value of START */
  {AEECARDSESSION_USIM_OPLMNWACT,                   MMGSDI_USIM_OPLMNWACT},                   /* Operator controlled PLMN sel with access tech */
  {AEECARDSESSION_USIM_OPLMNSEL,                    MMGSDI_USIM_OPLMNSEL},                    /* OPLMN selector */
  {AEECARDSESSION_USIM_HPLMNWACT,                   MMGSDI_USIM_HPLMNWACT},                   /* HPLMN selector with access tech */

  {AEECARDSESSION_USIM_ARR,                         MMGSDI_USIM_ARR},                         /* Access Rule reference */
  {AEECARDSESSION_USIM_RPLMNACT,                    MMGSDI_USIM_RPLMNACT},                    /* RPLMN last used access tech */
  {AEECARDSESSION_USIM_NETPAR,                      MMGSDI_USIM_NETPAR},                      /* Network parameters */
  {AEECARDSESSION_USIM_VMWI,                        MMGSDI_USIM_VMWI},                        /* CPHS: Voice Mail Waiting Indicator */
  {AEECARDSESSION_USIM_SVC_STR_TBL,                 MMGSDI_USIM_SVC_STR_TBL},                 /* CPHS: Service String Table */
  {AEECARDSESSION_USIM_CFF,                         MMGSDI_USIM_CFF},                         /* CPHS: Call Forwarding Flag */
  {AEECARDSESSION_USIM_ONS,                         MMGSDI_USIM_ONS},                         /* CPHS: Operator Name String */
  {AEECARDSESSION_USIM_CSP,                         MMGSDI_USIM_CSP},                         /* CPHS: Customer Service Profile */
  {AEECARDSESSION_USIM_CPHSI,                       MMGSDI_USIM_CPHSI},                       /* CPHS: CPHS Information */
  {AEECARDSESSION_USIM_MN,                          MMGSDI_USIM_MN},                          /* CPHS: Mailbox Number */

  /* EFs of ORANGE DF under USIM ADF */
  {AEECARDSESSION_USIM_7F40_PROP1_DFS,              MMGSDI_USIM_7F40_PROP1_DFS},              /* Dynamic Flags Status */
  {AEECARDSESSION_USIM_7F40_PROP1_D2FS,             MMGSDI_USIM_7F40_PROP1_D2FS},             /* Dynamic2 Flag Setting */
  {AEECARDSESSION_USIM_7F40_PROP1_CSP2,             MMGSDI_USIM_7F40_PROP1_CSP2},             /* Customer Service Profile Line2*/
  /*EFs at DF PHONEBOOK under USIM ADF */
  {AEECARDSESSION_USIM_PBR,                         MMGSDI_USIM_PBR},                         /* Phone book reference file */
  {AEECARDSESSION_USIM_CCP1,                        MMGSDI_USIM_CCP1},                        /* Capability Configuration parameters 1 */
  {AEECARDSESSION_USIM_UID,                         MMGSDI_USIM_UID},                         /* Unique Identifier */
  {AEECARDSESSION_USIM_PSC,                         MMGSDI_USIM_PSC},                         /* Phone book synchronization center */
  {AEECARDSESSION_USIM_CC,                          MMGSDI_USIM_CC},                          /* Change counter */
  {AEECARDSESSION_USIM_PUID,                        MMGSDI_USIM_PUID},                        /* Previous Unique Identifier */
  /*EFs at DF GSM under USIM ADF */
  {AEECARDSESSION_USIM_KC,                          MMGSDI_USIM_KC},                          /* GSM ciphering key Kc */

  {AEECARDSESSION_USIM_KCGPRS,                      MMGSDI_USIM_KCGPRS},                      /* GPRS ciphering key */
  {AEECARDSESSION_USIM_CPBCCH,                      MMGSDI_USIM_CPBCCH},                      /* CPBCCH information */
  {AEECARDSESSION_USIM_INVSCAN,                     MMGSDI_USIM_INVSCAN},                     /* Investigation scan */
  /*EFs at DF MEXe under USIM ADF */
  {AEECARDSESSION_USIM_MEXE_ST,                     MMGSDI_USIM_MEXE_ST},                     /* MExE Service table */
  {AEECARDSESSION_USIM_ORPK,                        MMGSDI_USIM_ORPK},                        /* Operator Root Public Key */
  {AEECARDSESSION_USIM_ARPK,                        MMGSDI_USIM_ARPK},                        /* Administrator Root Public Key */
  {AEECARDSESSION_USIM_TPRPK,                       MMGSDI_USIM_TPRPK},                       /* Third party Root public key */
  {AEECARDSESSION_USIM_MWIS,                        MMGSDI_USIM_MWIS},                        /* Message Waiting Indication Status*/
  /* EFs related to Bootstrapping under USIM ADF */
  {AEECARDSESSION_USIM_GBABP,                       MMGSDI_USIM_GBABP},                       /* GBA BootStrapping Parameters */

  {AEECARDSESSION_USIM_GBANL,                       MMGSDI_USIM_GBANL},                       /* GBA NAF Derivation List Associated with 
                                                                                                NAF Derivation Proc*/
  /* EFs related to MBMS under USIM ADF */
  {AEECARDSESSION_USIM_MSK,                         MMGSDI_USIM_MSK},                         /* MBMS Service Key */
  {AEECARDSESSION_USIM_MUK,                         MMGSDI_USIM_MUK},                         /* MBMS User Key */
  /* EFs selected by path at the telecom DF in GSM and CDMA */
  {AEECARDSESSION_TELECOM_IAP,                      MMGSDI_TELECOM_IAP},                      /* Index Administration File */
  {AEECARDSESSION_TELECOM_PBC,                      MMGSDI_TELECOM_PBC},                      /* Phone Book Control */
  {AEECARDSESSION_TELECOM_GRP,                      MMGSDI_TELECOM_GRP},                      /* Grouping File */
  {AEECARDSESSION_TELECOM_AAS,                      MMGSDI_TELECOM_AAS},                      /* Additional Number Alpha String */
  {AEECARDSESSION_TELECOM_GAS,                      MMGSDI_TELECOM_GAS},                      /* Grouping Information Alpha String */
  {AEECARDSESSION_TELECOM_ANR,                      MMGSDI_TELECOM_ANR},                      /* Additional Number */
  {AEECARDSESSION_TELECOM_SNE,                      MMGSDI_TELECOM_SNE},                      /* Secondary Name Entry */
  {AEECARDSESSION_TELECOM_EMAIL,                    MMGSDI_TELECOM_EMAIL},                    /* Email */

  {AEECARDSESSION_TELECOM_ANRA,                     MMGSDI_TELECOM_ANRA},                     /* Additional Number A */
  {AEECARDSESSION_TELECOM_ANRB,                     MMGSDI_TELECOM_ANRB},                     /* Additional Number B */
  {AEECARDSESSION_TELECOM_ANRC,                     MMGSDI_TELECOM_ANRC},                     /* Additional Number C */
  {AEECARDSESSION_TELECOM_ANR1,                     MMGSDI_TELECOM_ANR1},                     /* Additional Number 1 */
  {AEECARDSESSION_TELECOM_ANRA1,                    MMGSDI_TELECOM_ANRA1},                    /* Additional Number A1 */
  {AEECARDSESSION_TELECOM_ANRB1,                    MMGSDI_TELECOM_ANRB1},                    /* Additional Number B1 */
  {AEECARDSESSION_TELECOM_ANRC1,                    MMGSDI_TELECOM_ANRC1},                    /* Additional Number C1 */
  {AEECARDSESSION_TELECOM_ADN1,                     MMGSDI_TELECOM_ADN1},                     /* Abbreviated Dialing Numbers 1 */
  {AEECARDSESSION_TELECOM_PBC1,                     MMGSDI_TELECOM_PBC1},                     /* Phone Book Control 1 */
  {AEECARDSESSION_TELECOM_GRP1,                     MMGSDI_TELECOM_GRP1},                     /* Grouping File 1 */

  {AEECARDSESSION_TELECOM_SNE1,                     MMGSDI_TELECOM_SNE1},                     /* Secondary Name Entry 1 */
  {AEECARDSESSION_TELECOM_UID1,                     MMGSDI_TELECOM_UID1},                     /* Unique Identifier 1 */
  {AEECARDSESSION_TELECOM_EMAIL1,                   MMGSDI_TELECOM_EMAIL1},                   /* Email 1 */
  {AEECARDSESSION_TELECOM_IAP1,                     MMGSDI_TELECOM_IAP1},                     /* Index Administration File 1 */
  {AEECARDSESSION_TELECOM_ANR2,                     MMGSDI_TELECOM_ANR2},                     /* Additional Number 2 */
  {AEECARDSESSION_TELECOM_ANRA2,                    MMGSDI_TELECOM_ANRA2},                    /* Additional Number A2 */
  {AEECARDSESSION_TELECOM_ANRB2,                    MMGSDI_TELECOM_ANRB2},                    /* Additional Number B2 */
  {AEECARDSESSION_TELECOM_ANRC2,                    MMGSDI_TELECOM_ANRC2},                    /* Additional Number C2 */
  {AEECARDSESSION_TELECOM_ADN2,                     MMGSDI_TELECOM_ADN2},                     /* Abbreviated Dialing Numbers 2 */
  {AEECARDSESSION_TELECOM_PBC2,                     MMGSDI_TELECOM_PBC2},                     /* Phone Book Control 2 */

  {AEECARDSESSION_TELECOM_GRP2,                     MMGSDI_TELECOM_GRP2},                     /* Grouping File 2 */
  {AEECARDSESSION_TELECOM_SNE2,                     MMGSDI_TELECOM_SNE2},                     /* Secondary Name Entry 2 */
  {AEECARDSESSION_TELECOM_UID2,                     MMGSDI_TELECOM_UID2},                     /* Unique Identifier 2 */
  {AEECARDSESSION_TELECOM_EMAIL2,                   MMGSDI_TELECOM_EMAIL2},                   /* Email 2 */
  {AEECARDSESSION_TELECOM_IAP2,                     MMGSDI_TELECOM_IAP2},                     /* Index Administration File 2 */
  {AEECARDSESSION_TELECOM_ANR3,                     MMGSDI_TELECOM_ANR3},                     /* Additional Number 3 */
  {AEECARDSESSION_TELECOM_ANRA3,                    MMGSDI_TELECOM_ANRA3},                    /* Additional Number A3 */
  {AEECARDSESSION_TELECOM_ANRB3,                    MMGSDI_TELECOM_ANRB3},                    /* Additional Number B3 */
  {AEECARDSESSION_TELECOM_ANRC3,                    MMGSDI_TELECOM_ANRC3},                    /* Additional Number C3 */
  {AEECARDSESSION_TELECOM_ADN3,                     MMGSDI_TELECOM_ADN3},                     /* Abbreviated Dialing Numbers 3 */

  {AEECARDSESSION_TELECOM_PBC3,                     MMGSDI_TELECOM_PBC3},                     /* Phone Book Control 3 */
  {AEECARDSESSION_TELECOM_GRP3,                     MMGSDI_TELECOM_GRP3},                     /* Grouping File 3 */
  {AEECARDSESSION_TELECOM_SNE3,                     MMGSDI_TELECOM_SNE3},                     /* Secondary Name Entry 3 */
  {AEECARDSESSION_TELECOM_UID3,                     MMGSDI_TELECOM_UID3},                     /* Unique Identifier 3 */
  {AEECARDSESSION_TELECOM_EMAIL3,                   MMGSDI_TELECOM_EMAIL3},                   /* Email 3 */
  {AEECARDSESSION_TELECOM_IAP3,                     MMGSDI_TELECOM_IAP3},                     /* Index Administration File 3 */
  /* EFs selected by path of USIM ADF */
  {AEECARDSESSION_USIM_ADN,                         MMGSDI_USIM_ADN},                         /* Abbreviated Dialing Numbers */
  {AEECARDSESSION_USIM_IAP,                         MMGSDI_USIM_IAP},                         /* Index Administration File */
  {AEECARDSESSION_USIM_PBC,                         MMGSDI_USIM_PBC},                         /* Phone Book Control */
  {AEECARDSESSION_USIM_EXT1,                        MMGSDI_USIM_EXT1},                        /* Extension 1 */

  {AEECARDSESSION_USIM_GRP,                         MMGSDI_USIM_GRP},                         /* Grouping File */
  {AEECARDSESSION_USIM_AAS,                         MMGSDI_USIM_AAS},                         /* Additional Number Alpha String */
  {AEECARDSESSION_USIM_AAS1,                        MMGSDI_USIM_AAS1},                        /* Additional Number Alpha String 1 */
  {AEECARDSESSION_USIM_GAS,                         MMGSDI_USIM_GAS},                         /* Grouping Information Alpha String */
  {AEECARDSESSION_USIM_GAS1,                        MMGSDI_USIM_GAS1},                        /* Grouping Information Alpha String */
  {AEECARDSESSION_USIM_ANR,                         MMGSDI_USIM_ANR},                         /* Additional Number */
  {AEECARDSESSION_USIM_SNE,                         MMGSDI_USIM_SNE},                         /* Secondary Name Entry */
  {AEECARDSESSION_USIM_EMAIL,                       MMGSDI_USIM_EMAIL},                       /* Email */
  {AEECARDSESSION_USIM_ANRA,                        MMGSDI_USIM_ANRA},                        /* Additional Number A */
  {AEECARDSESSION_USIM_ANRB,                        MMGSDI_USIM_ANRB},                        /* Additional Number B */

  {AEECARDSESSION_USIM_ANRC,                        MMGSDI_USIM_ANRC},                        /* Additional Number C */
  {AEECARDSESSION_USIM_ANR1,                        MMGSDI_USIM_ANR1},                        /* Additional Number 1 */
  {AEECARDSESSION_USIM_ANRA1,                       MMGSDI_USIM_ANRA1},                       /* Additional Number A1 */
  {AEECARDSESSION_USIM_ANRB1,                       MMGSDI_USIM_ANRB1},                       /* Additional Number B1 */
  {AEECARDSESSION_USIM_ANRC1,                       MMGSDI_USIM_ANRC1},                       /* Additional Number C1 */
  {AEECARDSESSION_USIM_ADN1,                        MMGSDI_USIM_ADN1},                        /* Abbreviated Dialing Numbers 1 */
  {AEECARDSESSION_USIM_PBC1,                        MMGSDI_USIM_PBC1},                        /* Phone Book Control 1 */
  {AEECARDSESSION_USIM_GRP1,                        MMGSDI_USIM_GRP1},                        /* Grouping File 1 */
  {AEECARDSESSION_USIM_SNE1,                        MMGSDI_USIM_SNE1},                        /* Secondary Name Entry 1 */
  {AEECARDSESSION_USIM_UID1,                        MMGSDI_USIM_UID1},                        /* Unique Identifier 1 */

  {AEECARDSESSION_USIM_EMAIL1,                      MMGSDI_USIM_EMAIL1},                      /* Email 1 */
  {AEECARDSESSION_USIM_IAP1,                        MMGSDI_USIM_IAP1},                        /* Index Administration File 1 */
  {AEECARDSESSION_USIM_ANR2,                        MMGSDI_USIM_ANR2},                        /* Additional Number 2 */
  {AEECARDSESSION_USIM_ANRA2,                       MMGSDI_USIM_ANRA2},                       /* Additional Number A2 */
  {AEECARDSESSION_USIM_ANRB2,                       MMGSDI_USIM_ANRB2},                       /* Additional Number B2 */
  {AEECARDSESSION_USIM_ANRC2,                       MMGSDI_USIM_ANRC2},                       /* Additional Number C2 */
  {AEECARDSESSION_USIM_ADN2,                        MMGSDI_USIM_ADN2},                        /* Abbreviated Dialing Numbers 2 */
  {AEECARDSESSION_USIM_PBC2,                        MMGSDI_USIM_PBC2},                        /* Phone Book Control 2 */
  {AEECARDSESSION_USIM_GRP2,                        MMGSDI_USIM_GRP2},                        /* Grouping File 2 */
  {AEECARDSESSION_USIM_SNE2,                        MMGSDI_USIM_SNE2},                        /* Secondary Name Entry 2 */

  {AEECARDSESSION_USIM_UID2,                        MMGSDI_USIM_UID2},                        /* Unique Identifier 2 */
  {AEECARDSESSION_USIM_EMAIL2,                      MMGSDI_USIM_EMAIL2},                      /* Email 2 */
  {AEECARDSESSION_USIM_IAP2,                        MMGSDI_USIM_IAP2},                        /* Index Administration File 2 */
  {AEECARDSESSION_USIM_ANR3,                        MMGSDI_USIM_ANR3},                        /* Additional Number 3 */
  {AEECARDSESSION_USIM_ANRA3,                       MMGSDI_USIM_ANRA3},                       /* Additional Number A3 */
  {AEECARDSESSION_USIM_ANRB3,                       MMGSDI_USIM_ANRB3},                       /* Additional Number B3 */
  {AEECARDSESSION_USIM_ANRC3,                       MMGSDI_USIM_ANRC3},                       /* Additional Number C3 */
  {AEECARDSESSION_USIM_ADN3,                        MMGSDI_USIM_ADN3},                        /* Abbreviated Dialing Numbers 3 */
  {AEECARDSESSION_USIM_PBC3,                        MMGSDI_USIM_PBC3},                        /* Phone Book Control 3 */
  {AEECARDSESSION_USIM_GRP3,                        MMGSDI_USIM_GRP3},                        /* Grouping File 3 */

  {AEECARDSESSION_USIM_SNE3,                        MMGSDI_USIM_SNE3},                        /* Secondary Name Entry 3 */
  {AEECARDSESSION_USIM_UID3,                        MMGSDI_USIM_UID3},                        /* Unique Identifier 3 */
  {AEECARDSESSION_USIM_EMAIL3,                      MMGSDI_USIM_EMAIL3},                      /* Email 3 */
  {AEECARDSESSION_USIM_IAP3,                        MMGSDI_USIM_IAP3},                        /* Index Administration File 3 */
  {AEECARDSESSION_GSM_PNN,                          MMGSDI_GSM_PNN},                          /* CPHS: PLMN Name */
  {AEECARDSESSION_GSM_OPL,                          MMGSDI_GSM_OPL},                          /* CPHS: Operator List */
  {AEECARDSESSION_USIM_PNN,                         MMGSDI_USIM_PNN},                         /* CPHS: PLMN Network Name */
  {AEECARDSESSION_USIM_OPL,                         MMGSDI_USIM_OPL},                         /* CPHS: Operator List */
  {AEECARDSESSION_ISIM_IMPI,                        MMGSDI_ISIM_IMPI},                        /* ISIM: IMPI */
  {AEECARDSESSION_ISIM_DOMAIN,                      MMGSDI_ISIM_DOMAIN},                      /* ISIM: domain */

  {AEECARDSESSION_ISIM_IMPU,                        MMGSDI_ISIM_IMPU},                        /* ISIM: IMPU */
  {AEECARDSESSION_ISIM_ARR,                         MMGSDI_ISIM_ARR},                         /* ISIM: ARR */
  {AEECARDSESSION_ISIM_KEYS,                        MMGSDI_ISIM_KEYS},                        /* ISIM: Keys */
  {AEECARDSESSION_ISIM_AD,                          MMGSDI_ISIM_AD},                          /* ISIM: AD */
  {AEECARDSESSION_MFLO_RAN_RFR,                     MMGSDI_MFLO_RAN_RFR},                     /* RAN Revision */
  {AEECARDSESSION_MFLO_RAN_RF,                      MMGSDI_MFLO_RAN_RF},                      /* Radio Frequency */
  {AEECARDSESSION_MFLO_MFAPP_MFR,                   MMGSDI_MFLO_MFAPP_MFR},                   /* MFlo App Revision */
  {AEECARDSESSION_MFLO_MFAPP_BCSID,                 MMGSDI_MFLO_MFAPP_BCSID},                 /* Billing and Customer Service Provider ID */
  {AEECARDSESSION_MFLO_MFAPP_MFT,                   MMGSDI_MFLO_MFAPP_MFT},                   /* MFlo Feature Table */
  {AEECARDSESSION_MFLO_MFAPP_SUBTYPE,               MMGSDI_MFLO_MFAPP_SUBTYPE},               /* Subscriber Type */

  {AEECARDSESSION_MFLO_MFAPP_SUBPROF,               MMGSDI_MFLO_MFAPP_SUBPROF},               /* Subscriber Profile */
  {AEECARDSESSION_MFLO_MFAPP_UTSD,                  MMGSDI_MFLO_MFAPP_UTSD},                  /* Usage Tracking Service Data */
  {AEECARDSESSION_MFLO_MFAPP_UIP,                   MMGSDI_MFLO_MFAPP_UIP},                   /* UI Preference */
  {AEECARDSESSION_MFLO_MFAPP_PCSET,                 MMGSDI_MFLO_MFAPP_PCSET},                 /* Parental Control Setting */
  {AEECARDSESSION_MFLO_MFAPP_MCHSET,                MMGSDI_MFLO_MFAPP_MCHSET},                /* Multi-presentation view Channel Setting */
  {AEECARDSESSION_MFLO_MFAPP_RFUD,                  MMGSDI_MFLO_MFAPP_RFUD},                  /* Reserved for Future User Data */
  /* EFs for MMS Notification */
  {AEECARDSESSION_USIM_MMSN,                        MMGSDI_USIM_MMSN},                        /* MMS Notification */
  {AEECARDSESSION_USIM_MMSICP,                      MMGSDI_USIM_MMSICP},                      /* MMS Issuer connectivity parameters */
  {AEECARDSESSION_USIM_MMSUP,                       MMGSDI_USIM_MMSUP},                       /* MMS User Preferences */
  {AEECARDSESSION_USIM_MMSUCP,                      MMGSDI_USIM_MMSUCP},                      /* MMS User connectivity parameters */

  {AEECARDSESSION_USIM_CFIS,                        MMGSDI_USIM_CFIS},                         /* Call Forwarding Indication status */
  {AEECARDSESSION_USIM_EXT8,                        MMGSDI_USIM_EXT8},                         /* Extension 8 */
  /* Efs for PKCS #15 support */
  {AEECARDSESSION_PKCS15_ODF,                       MMGSDI_PKCS15_ODF},                       /* Object Directory File */
  {AEECARDSESSION_PKCS15_PRKDF,                     MMGSDI_PKCS15_PRKDF},                     /* Private Key Directory File */
  {AEECARDSESSION_PKCS15_PUKDF,                     MMGSDI_PKCS15_PUKDF},                     /* Public Key Directory File */
  {AEECARDSESSION_PKCS15_SKDF,                      MMGSDI_PKCS15_SKDF},                      /* Secret Key Directory File */
  {AEECARDSESSION_PKCS15_CDF,                       MMGSDI_PKCS15_CDF},                       /* Certificate Directory File */
  {AEECARDSESSION_PKCS15_DODF,                      MMGSDI_PKCS15_DODF},                      /* Data Object Directory File */
  {AEECARDSESSION_PKCS15_AODF,                      MMGSDI_PKCS15_AODF},                      /* Authentication Object Directory File */
  {AEECARDSESSION_PKCS15_TI,                        MMGSDI_PKCS15_TI},                        /* Token Information File */
  
  {AEECARDSESSION_PKCS15_US,                        MMGSDI_PKCS15_US}                         /* Unused space Information File */
};
#endif /* FEATURE_MMGSDI */

/*===========================================================================

                           EVENT HANDLERS

===========================================================================*/


/*===========================================================================

                           Function Prototypes

===========================================================================*/

/*===========================================================================

                   ICardSession FUNCTION DEFINITIONS

===========================================================================*/
/*=============================================================================
FUNCTION: OEMCardSession_New

DESCRIPTION:
  Allocates a new ICardSession object, creates a Core object if it is not
  present, stores the App context and initializes object data.
  A new ICardSession object is created for every ISHELL_CreateInstance().

DEPENDENCIES:
  None

SIDE EFFECTS:
  None

=============================================================================*/
int OEMCardSession_New(
  IShell *pIShell,
  AEECLSID cls,
  void **ppif)
{
  ICardSession *pNew;

  MSG_HIGH("OEMCardSession_New cls 0x%x", cls, 0, 0);

#ifdef AEE_SIMULATOR
  /* Do not create an object is this is being run on the SDK. */
  *ppif = NULL;
  return EUNSUPPORTED;
#endif

  // Check parameters.
  if ((NULL == pIShell) || (NULL == ppif)) {
    MSG_ERROR("Null Input Ptr 0x%x, 0x%x", pIShell, ppif, 0);
    return EBADPARM;
  }

  if ((AEECLSID_CARDSESSION != cls) && (AEECLSID_CARDSESSION_SLOT2 != cls)) {
    MSG_ERROR("Invalid Cls 0x%x", cls, 0, 0);
    return EUNSUPPORTED;
  }

  /* Allocate the object */
  pNew = (ICardSession *)(void*) (AEE_NewClass((IBaseVtbl*)&gOEMCardSessionFuncs,
                                       sizeof(ICardSession)));
  if (NULL == pNew) {
    MSG_ERROR("Unable to alloc new ptr", 0, 0, 0);
    return ENOMEMORY;
  }

#ifndef FEATURE_BREW_3_0
  (void) AEE_LinkSysObject(&pNew->u.m_so);
#endif

  /* Initialize the values. */
  pNew->m_pIShell = pIShell;
  pNew->m_cls = cls;
  pNew->m_uRefs = 1;

  pNew->m_coreObj = OEMCardSessionCore_New();
  if (!pNew->m_coreObj) {
    MSG_ERROR("CoreObj NULL", 0, 0, 0);
    (void) OEMCardSession_Release(pNew);
    return EFAILED;
  }

  pNew->m_pac = AEE_GetAppContext();

  /* Initalize Client Id to 0 to indicate not yet set */
  pNew->m_client_id = OEMCARDESESSION_DEFAULT_CLIENT_ID;

  pNew->m_byChannelStatus = OEMCARDSESSION_NONE;

  if (AEECLSID_CARDSESSION == cls) {
    pNew->m_u8Slot = OEMCARDSESSION_SLOT1;
  }
  else {
    pNew->m_u8Slot = OEMCARDSESSION_SLOT2;
  }

  *ppif = pNew;

  MSG_HIGH("New OEMCardSession Instance 0x%x Created", *ppif, 0, 0);

  return AEE_SUCCESS;
}


/*=============================================================================
FUNCTION: OEMCardSession_AddRef

DESCRIPTION:
  Indicates that one or more Client objects are now pointing to this
  ICardSession object.

DEPENDENCIES:
  None

SIDE EFFECTS:
  None

=============================================================================*/
uint32 OEMCardSession_AddRef(
  ICardSession *pMe)
{
  MSG_HIGH("OEMCardSession_AddRef", 0, 0, 0);

  if(NULL != pMe) {
    return(++pMe->m_uRefs);
  }
  return 0;
}


/*=============================================================================
FUNCTION: OEMCardSession_Release

DESCRIPTION:
  Decreases the number of references to the ICardSession object.  If the
  last reference is removed, it deallocates the object.

DEPENDENCIES:
  None

SIDE EFFECTS:
  None

=============================================================================*/
uint32 OEMCardSession_Release(
  ICardSession *pMe)
{
#ifdef FEATURE_MMGSDI
  int                      nReturnStatus = SUCCESS;
  mmgsdi_slot_id_enum_type mmgsdi_slot   = MMGSDI_MAX_SLOT_ID_ENUM;
#endif /* FEATURE_MMGSDI */
  boolean                  session_open  = FALSE;

  MSG_HIGH("OEMCardSession_Release", 0, 0, 0);

  if (NULL == pMe) {
    return(0);
  }

  if (pMe->m_uRefs > 1) {
    return --pMe->m_uRefs;
  }

  MSG_HIGH("Releasing ICardSession Refs = %d",pMe->m_uRefs,0,0);

#ifdef FEATURE_MMGSDI
  /*Close the session if it is still open*/
  if ((OEMCARDSESSION_OPENED  == pMe->m_byChannelStatus) ||
      (OEMCARDSESSION_OPENING == pMe->m_byChannelStatus) ||
      (OEMCARDSESSION_SELECTED== pMe->m_byChannelStatus)) {

    /* convert the slot */
    nReturnStatus = OEMCardSession_MapMMGSDISlot(pMe->m_u8Slot,
                                                &mmgsdi_slot);
    if (SUCCESS != nReturnStatus) {
      return(pMe->m_uRefs);
    }
    (void) mmgsdi_session_management_close (
                     pMe->m_client_id,
                     mmgsdi_slot,
                     OEMCardSessionInternalCb,
                     0);
    session_open = TRUE;
  }
#endif /* FEATURE_MMGSDI */

  /* This function will also release the model object ptr if it exists*/
  OEMCardSession_Free(pMe,session_open);

  if (pMe->m_coreObj) {
    (void) OEMCardSessionCore_Release();
  }

  // Ref count is zero.  Release memory
  FREE(pMe);
  return(0);
}


/*===========================================================================

FUNCTION: OEMCardSession_QueryInterface

DESCRIPTION
  Allows applications to query if ICardSession supports other interfaces
  and then returns a pointer to that new interface supported via ICardSession.
  Currently, we do not support any other interface.

DEPENDENCIES
  None

COMMENTS
  NOTE1: Before querying model interface, the client must register with mmgsdi for 
  client id. This is because the creation of model interface includes the
  event registering of the client with mmgsdi.
  NOTE2: Client*must* query for model interface for each seperate ICardSession 
  Instance it creates.

RETURN VALUE
  none

SIDE EFFECTS
===========================================================================*/
int OEMCardSession_QueryInterface(
  ICardSession *pMe,
  AEECLSID id,
  void **ppNew)
{
  int ret_val = AEE_SUCCESS;

  MSG_HIGH("OEMCardSession_QueryInterface cls 0x%x", id, 0, 0);

  if ((NULL == pMe) || (NULL == ppNew)) {
    return EBADPARM;
  }

  switch (id) {
    case AEECLSID_CARDSESSION:
    case AEECLSID_CARDSESSION_SLOT2:
      if (OEMCardSession_AddRef(pMe)) {
        *ppNew = pMe;
      }
      else {
        *ppNew = NULL;
        ret_val = ECLASSNOTSUPPORT;
      }
      break;
    case AEECLSID_CARDSESSION_MODEL:
      ret_val = OEMCardSessionModel_New(pMe, ppNew);
      break;
    default:
      *ppNew = NULL;
      ret_val = ECLASSNOTSUPPORT;
      break;
  }

  return ret_val;
}


/*---------------------------------------------------------------------------
                           Callback Functions
--------------------------------------------------------------------------- */
#ifdef FEATURE_MMGSDI
/*===========================================================================
FUNCTION: OEMCardSession_ParseSecInfo

DESCRIPTION
  This function parses the access conditions for All types of protocols

DEPENDENCIES
  none

RETURN VALUE
  SUCCESS  - Successful handling of the procedure  
  EFAILED  - General Error 
  EBADPARM - Bad Input Parameters

SIDE EFFECTS
===========================================================================*/
static int OEMCardSession_ParseSecInfo(
  AEECardSessionAccessCondType           *pAccessStruct,
  const mmgsdi_file_security_type        *pAccess)
{
  int32 i =0;

  if ((NULL == pAccessStruct) ||
      (NULL == pAccess)) {
    return EFAILED;
  }

  (void)MEMSET(pAccessStruct, 0x00, sizeof(AEECardSessionAccessCondType));

  switch (pAccess->protection_method)
  {
    case MMGSDI_ALWAYS_ALLOWED:
      pAccessStruct->bAlways  = TRUE;
      break;

    case MMGSDI_NEVER_ALLOWED:
      pAccessStruct->bNever   = TRUE;
      break;

    case MMGSDI_AND_ALLOWED:
      pAccessStruct->bAnd     = TRUE;
      break;

    case MMGSDI_OR_ALLOWED:
    case MMGSDI_SINGLE_ALLOWED:
      pAccessStruct->bAnd     = FALSE;
      break;

    default:
      MSG_ERROR("Cannot Handle protection method - 0x0%x",
                 pAccess->protection_method, 0,0);
      return EBADPARM;
  }

  for (i=0;i<pAccess->num_protection_pin;i++)
  {
    if (NULL == pAccess->protection_pin_ptr) {
      MSG_ERROR("Null Pin Ptr with Num Pin 0x%x",
                 pAccess->num_protection_pin, 0, 0);
      return EBADPARM;
    }
    switch (pAccess->protection_pin_ptr[i]) {
      case MMGSDI_PIN1:
        pAccessStruct->bPin1 = TRUE;
        break;
      case MMGSDI_PIN2:
        pAccessStruct->bPin2 = TRUE;
        break;
      case MMGSDI_UNIVERSAL_PIN:
        pAccessStruct->bPinUniversal = TRUE;
        break;
      default:
        break;
    }
  }
  return SUCCESS;
}


/*===========================================================================

FUNCTION: OEMCardSession_MapFileProtection

DESCRIPTION
  This function maps gsdi_file_protection_T into AEECardFileProtectionType

DEPENDENCIES
  none

RETURN VALUE
  SUCCESS  - Successful handling of the procedure  
  EFAILED  - General Error 
  EBADPARM - Bad Input Parameters

SIDE EFFECTS
===========================================================================*/
static int OEMCardSession_MapFileProtection(
  AEECardSessionFileProtectionType       *pCardFileProtection,
  const mmgsdi_file_security_access_type *pAccess)
{
  int               nReturnStatus;

  if ((NULL == pCardFileProtection) ||
      (NULL == pAccess)) {
    return EFAILED;
  }

  nReturnStatus = OEMCardSession_ParseSecInfo(&(pCardFileProtection->sRead),
                                              &pAccess->read);
  if (SUCCESS != nReturnStatus) {
    MSG_ERROR("Read Sec Parsing Fail 0x%x", nReturnStatus, 0, 0);
    return nReturnStatus;
  }

  nReturnStatus = OEMCardSession_ParseSecInfo(&(pCardFileProtection->sWrite),
                                              &pAccess->write);
  if (SUCCESS != nReturnStatus) {
    MSG_ERROR("Write Sec Parsing Fail 0x%x", nReturnStatus, 0, 0);
    return nReturnStatus;
  }

  nReturnStatus = OEMCardSession_ParseSecInfo(
                    &(pCardFileProtection->sIncrease),
                    &pAccess->increase);
  if (SUCCESS != nReturnStatus) {
    MSG_ERROR("Increase Sec Parsing Fail 0x%x", nReturnStatus, 0, 0);
    return nReturnStatus;
  }

  nReturnStatus = OEMCardSession_ParseSecInfo(
                    &(pCardFileProtection->sInvalidate),
                    &pAccess->invalidate_deactivate);
  if (SUCCESS != nReturnStatus) {
    MSG_ERROR("Invalidate Sec Parsing Fail 0x%x", nReturnStatus, 0, 0);
    return nReturnStatus;
  }

  nReturnStatus = OEMCardSession_ParseSecInfo(
                    &(pCardFileProtection->sRehabilitate),
                    &pAccess->rehabilitate_activate);
  if (SUCCESS != nReturnStatus) {
    MSG_ERROR("Rehabilitate Sec Parsing Fail 0x%x", nReturnStatus, 0, 0);
    return nReturnStatus;
  }

  return SUCCESS;
} /* OEMCardSession_MapFileProtection */


/*=============================================================================
FUNCTION: OEMCardSession_ClientIDDereg

DESCRIPTION:
   This function deregisters the client id

DEPENDENCIES:
  None

SIDE EFFECTS:
  

=============================================================================*/
/*lint -save -esym(818,pMe) suppress could be const */
void OEMCardSession_ClientIDDereg(ICardSession *pMe)
{
  AEECardSessionOwner     *pCardSessionOwnerCurr = NULL;
  AEECardSessionOwner     *pCardSessionOwnerPrev = NULL;
  mmgsdi_return_enum_type  mmgsdi_status         = MMGSDI_SUCCESS;

  /*DeReg Client ID here*/
  mmgsdi_status =  mmgsdi_client_id_dereg (pMe->m_client_id,
                                           OEMCardSessionCmdCb,
                                           0);
  if (MMGSDI_SUCCESS != mmgsdi_status) {
    MSG_ERROR("Could not deregister Client",0,0,0);
  }

  /* Release the model object ptr */
  (void) OEMCardSessionModel_Release(pMe->m_modelObj);

  /* Search for the session information node in the linked list */
  pCardSessionOwnerCurr = pMe->m_coreObj->m_SessionOwner;

  while (pCardSessionOwnerCurr) 
  {
    pCardSessionOwnerPrev = pCardSessionOwnerCurr;
    pCardSessionOwnerCurr = pCardSessionOwnerCurr->pNext;

    if (pCardSessionOwnerPrev->clientID == pMe->m_client_id) 
    {
      MSG_HIGH("Client ID 0x%x", pMe->m_client_id, 0, 0);
      OEMCARDSESSION_REMOVE_NODE(ICardSessionCoreObj->m_SessionOwner, pCardSessionOwnerPrev);
      OEMCARDSESSION_SYS_FREE(pCardSessionOwnerPrev);
      break;
    }
  }/* while (pCardSessionOwnerCurr) */
}/* OEMCardSession_ClientIDDereg */
/*lint -save +esym(818,pMe) suppress could be const */


/*=============================================================================
FUNCTION: OEMCardSession_RegistrationResp

DESCRIPTION:
   This function processes the response for CLIENT_ID_REG_CNF.  It is called 
   by the OEMCardSessionCmdCb function which is called by the mmgsdi request

DEPENDENCIES:
  None

RETURN VALUE
  SUCCESS  - Successful handling of the procedure  
  EFAILED  - General Error 
  EBADPARM - Bad Input Parameters

SIDE EFFECTS:
  Successful client ID Registration will result in a Registration for MMGSDI 
  Events

=============================================================================*/
static int OEMCardSession_RegistrationResp(
  ICardSession                   *pMe,
  const OEMCardSessionAsynCBInfo *pAsynInfo,
  mmgsdi_return_enum_type         status,
  const mmgsdi_cnf_type          *pCnf)
{
  AEECardSessionCmdStatus *pCmdStatus        = NULL;
  uint32                  client_id_upper    = 0;
  uint32                  client_id_lower    = 0;

  if((NULL == pAsynInfo)   ||
     (NULL == pMe      )   || 
     (NULL == pCnf     )) {
    MSG_ERROR("An Input Param Pointer is NULL.",0,0,0);
    return EBADPARM;
  }

  pCmdStatus= (AEECardSessionCmdStatus*)pAsynInfo->pClientData;

  if (NULL == pCmdStatus) {
   MSG_ERROR("CmdStatus Pointer is NULL.",0,0,0);
   return EFAILED;
  }

  if (MMGSDI_SUCCESS == status) {
    pCmdStatus->nCmdStatus= SUCCESS;
    pMe->m_client_id = pCnf->client_id_reg_cnf.response_header.client_id;

    client_id_upper = (uint32)(pMe->m_client_id >> 32);
    client_id_lower = (uint32)(pMe->m_client_id & 0xFFFFFFFF);

    MSG_HIGH("Registration of Client ID successful Upper Client Id 0x%x, Lower Client ID 0x%x",
              client_id_upper,client_id_lower,0);    
  }
  else {
    MSG_ERROR("Client Could not be registered",0,0,0);
    pCmdStatus->nCmdStatus = EFAILED;
  }
  return SUCCESS;
}


/*=============================================================================
FUNCTION: OEMCardSession_EvtRegistrationResp

DESCRIPTION:
   This function processes the response for MMGSDI evt Reg.  It is called 
   by the OEMCardSessionCmdCb function which is called by the mmgsdi request

DEPENDENCIES:
  None

SIDE EFFECTS:
  None

=============================================================================*/
static int OEMCardSession_EvtRegistrationResp(
  const ICardSession             *pMe,
  const OEMCardSessionAsynCBInfo *pAsynInfo,
  mmgsdi_return_enum_type         status,
  const mmgsdi_cnf_type          *pCnf)
{
  AEECardSessionCmdStatus *pCmdStatus  = NULL;

  if((NULL == pAsynInfo)   ||
     (NULL == pMe      )   || 
     (NULL == pCnf     )) {
    MSG_ERROR("An Input Param Pointer is NULL.",0,0,0);
    return EBADPARM;
  }

  pCmdStatus= (AEECardSessionCmdStatus*)pAsynInfo->pClientData;

  if (NULL == pCmdStatus) {
    MSG_ERROR("pCmdStatus Pointer is NULL.",0,0,0);
    return EFAILED;
  }

  if (MMGSDI_SUCCESS == status) {
    pCmdStatus->nCmdStatus= SUCCESS;
    MSG_HIGH("Evt Registration successful",0,0,0);
  }
  else {
    MSG_ERROR("Client Could not be registered for events",0,0,0);
    pCmdStatus->nCmdStatus = EFAILED;
  }
  return SUCCESS;
}

/*===========================================================================

FUNCTION: OEMCardSession_GetOpenResponse

DESCRIPTION
  This function processes the response for OPEN_CNF

DEPENDENCIES
  none

RETURN VALUE
  SUCCESS  - Successful handling of the procedure  
  EFAILED  - General Error 
  EBADPARM - Bad Input Parameters

SIDE EFFECTS
===========================================================================*/
static int OEMCardSession_GetOpenResponse(
  ICardSession                   *pMe,
  const OEMCardSessionAsynCBInfo *pAsynInfo,
  mmgsdi_return_enum_type         status,
  const mmgsdi_cnf_type          *pCnf)
{

  AEECardSessionCmdStatus *pCmdStatus = NULL;

  if((NULL == pAsynInfo)   ||
     (NULL == pMe      )){
    return EBADPARM;
  }

  pCmdStatus= (AEECardSessionCmdStatus*)pAsynInfo->pClientData;

  if (NULL == pCmdStatus) {
   return EFAILED;
  }

  if (MMGSDI_SUCCESS == status) {
    MSG_HIGH("Session opened",0,0,0);
    pCmdStatus->nCmdStatus= SUCCESS;
    pMe->m_byChannelStatus= OEMCARDSESSION_OPENED;
    pMe->m_client_id = pCnf->session_open_cnf.session_id;
  }
  else {
    MSG_ERROR("Session could not be openend",0,0,0);
    pCmdStatus->nCmdStatus = EFAILED;
    pMe->m_byChannelStatus  = OEMCARDSESSION_CLOSED;
  }

  return SUCCESS;
}


/*===========================================================================
FUNCTION: OEMCardSession_GetCloseResponse

DESCRIPTION
  This function processes the response for CLOSE_CNF

DEPENDENCIES
  none

RETURN VALUE
  SUCCESS  - Successful handling of the procedure  
  EFAILED  - General Error 
  EBADPARM - Bad Input Parameters

SIDE EFFECTS
===========================================================================*/
static int OEMCardSession_GetCloseResponse(
  ICardSession                   *pMe,
  const OEMCardSessionAsynCBInfo *pAsynInfo,
  mmgsdi_return_enum_type         status)
{

  AEECardSessionCmdStatus *pCmdStatus;

  if((NULL == pAsynInfo  ) ||
     (NULL == pMe        )) {
    return EBADPARM;
  }

  pCmdStatus= (AEECardSessionCmdStatus*)pAsynInfo->pClientData;

  if (NULL == pCmdStatus) {
   return EFAILED;
  }

  if (MMGSDI_SUCCESS == status) {
    MSG_HIGH("Session closed",0,0,0);
    pCmdStatus->nCmdStatus = SUCCESS;
    pMe->m_byChannelStatus = OEMCARDSESSION_CLOSED;
  }
  else {
    MSG_HIGH("Session could not be closed",0,0,0);
    pCmdStatus->nCmdStatus = EFAILED;
  }

  return SUCCESS;
}


/*===========================================================================

FUNCTION: OEMCardSession_GetAuthResponse

DESCRIPTION
  This function processes the response for ISIM_AUTH_CNF

DEPENDENCIES
  none

RETURN VALUE
  SUCCESS  - Successful handling of the procedure  
  EFAILED  - General Error 
  EBADPARM - Bad Input Parameters

SIDE EFFECTS
===========================================================================*/
static int OEMCardSession_GetAuthResponse(
  const OEMCardSessionAsynCBInfo *pAsynInfo,
  mmgsdi_return_enum_type         status,
  const mmgsdi_cnf_type          *pCnf)
{

  AEECardSessionAuthStatus  *pAuthStatus = NULL;

  if((NULL == pAsynInfo  ) ||
     (NULL == pCnf       )) {
    return EBADPARM;
  }

  pAuthStatus  = (AEECardSessionAuthStatus*)pAsynInfo->pClientData;
  if (NULL == pAuthStatus) {
   return EFAILED;
  }


  if (MMGSDI_SUCCESS == status) {
    MSG_HIGH("ISIM Auth Successful ",0,0,0);
    pAuthStatus->nCmdStatus= SUCCESS;
    pAuthStatus->sRes.nLen = MIN(pCnf->isim_auth_cnf.res.data_len,
                                 AEECARDSESSION_MAX_AUTH_DATA);

    if (pCnf->isim_auth_cnf.res.data_len > 0) {
      MSG_HIGH("Copy 0x%x byte RES", pCnf->isim_auth_cnf.res.data_len, 0, 0);
      (void)MEMCPY((uint8*)pAuthStatus->sRes.pData,
                   pCnf->isim_auth_cnf.res.data_ptr,
                   int32touint32(pAuthStatus->sRes.nLen));
    }

    pAuthStatus->sCk.nLen = MIN(pCnf->isim_auth_cnf.ck.data_len,
                                AEECARDSESSION_MAX_AUTH_DATA);

    if (pCnf->isim_auth_cnf.ck.data_len > 0) {
      MSG_HIGH("Copy 0x%x byte CK", pCnf->isim_auth_cnf.res.data_len, 0, 0);
      (void)MEMCPY((uint8*)pAuthStatus->sCk.pData,
                    pCnf->isim_auth_cnf.ck.data_ptr,
                    int32touint32(pAuthStatus->sCk.nLen));
    }

    pAuthStatus->sIk.nLen = MIN(pCnf->isim_auth_cnf.ik.data_len,
                                AEECARDSESSION_MAX_AUTH_DATA);

    if (pCnf->isim_auth_cnf.ik.data_len > 0) {
      MSG_HIGH("Copy 0x%x byte IK", pCnf->isim_auth_cnf.res.data_len, 0, 0);
      (void)MEMCPY((uint8*)pAuthStatus->sIk.pData,
                  pCnf->isim_auth_cnf.ik.data_ptr,
                  int32touint32(pAuthStatus->sIk.nLen));
    }

    pAuthStatus->sAuts.nLen = MIN(pCnf->isim_auth_cnf.auts.data_len,
                                  AEECARDSESSION_MAX_AUTH_DATA);

    if (pCnf->isim_auth_cnf.auts.data_len > 0) {
      MSG_HIGH("Copy 0x%x byte AUTS", pCnf->isim_auth_cnf.res.data_len, 0, 0);
      (void)MEMCPY((uint8*)pAuthStatus->sAuts.pData,
                    pCnf->isim_auth_cnf.auts.data_ptr,
                    int32touint32(pAuthStatus->sAuts.nLen));
    }

  }
  else if (MMGSDI_AUTH_ERROR_INCORRECT_MAC == status) {
    MSG_ERROR("ISIM Auth MAC Failure ",0,0,0);
    pAuthStatus->nCmdStatus = AEE_NET_EBADMAC;
  }
  else if (MMGSDI_AUTS_FAIL ==  status) {
    MSG_ERROR("ISIM AUTS Failure ",0,0,0);

    /* set the status tp AUTS_FAIL */
    pAuthStatus->nCmdStatus = AEECARDSESSION_AUTS_FAIL;

    /* copy the AUTS data */
    pAuthStatus->sAuts.nLen = MIN(pCnf->isim_auth_cnf.auts.data_len,
                                  AEECARDSESSION_MAX_AUTH_DATA);
    if (pCnf->isim_auth_cnf.auts.data_len > 0) {
      MSG_HIGH("Copy 0x%x byte AUTS", pAuthStatus->sAuts.nLen, 0, 0);
      (void)MEMCPY((uint8*)pAuthStatus->sAuts.pData,
                    pCnf->isim_auth_cnf.auts.data_ptr,
                    int32touint32(pAuthStatus->sAuts.nLen));
    }
  }
  else {
    MSG_ERROR("ISIM Auth Failed ",0,0,0);
    pAuthStatus->nCmdStatus= EFAILED;
  }
  
  return SUCCESS;
}


/*===========================================================================

FUNCTION: OEMCardSession_GetReadResponse

DESCRIPTION
  This function processes the response for READ_CNF

DEPENDENCIES
  none

RETURN VALUE
  SUCCESS  - Successful handling of the procedure  
  EFAILED  - General Error 
  EBADPARM - Bad Input Parameters

SIDE EFFECTS
===========================================================================*/
static int OEMCardSession_GetReadResponse(
  const OEMCardSessionAsynCBInfo *pAsynInfo,
  mmgsdi_return_enum_type         status,
  const mmgsdi_cnf_type          *pCnf)
{

  AEECardSessionReadTpStatus  *pReadTpStatus  = NULL;
  AEECardSessionReadRecStatus *pReadRecStatus = NULL;
  boolean                      bRecFile       = TRUE;
  AEECardSessionData          *pReadData      = NULL;
  int                          nReturnStatus;


  if((NULL == pAsynInfo)   ||
     (NULL == pCnf)) {
    return EBADPARM;
  }

  if (pCnf->read_cnf.accessed_rec_num == 0) {
    bRecFile = FALSE;
  }

  if(FALSE == bRecFile){
    /*Response for Transparent File*/
    pReadTpStatus  = (AEECardSessionReadTpStatus*)pAsynInfo->pClientData;

    if (NULL == pReadTpStatus) {
      return EFAILED;
    }
    pReadData = pReadTpStatus->pReadData;
    pReadTpStatus->dwOffset   = int32touint32(pCnf->read_cnf.accessed_offset);
    pReadTpStatus->nCmdStatus = EFAILED;

  }
  else {
    /*Response for Record Based File*/
    pReadRecStatus  = (AEECardSessionReadRecStatus*)pAsynInfo->pClientData;

    if (NULL == pReadRecStatus) {
      return EFAILED;
    }
    pReadData = pReadRecStatus->pReadData;
    pReadRecStatus->nCmdStatus = EFAILED;
    pReadRecStatus->dwRecNum   =
      int32touint32(pCnf->read_cnf.accessed_rec_num);
  }


  if (MMGSDI_SUCCESS == status) {
    MSG_MED("Read of File 0x%x  Successful , length = %d",
             pCnf->read_cnf.access.file.file_enum,
             pCnf->read_cnf.read_data.data_len,0);

    if (FALSE == bRecFile){
      pReadTpStatus->nCmdStatus = SUCCESS;
      pReadTpStatus->dwOffset   =
        int32touint32(pCnf->read_cnf.accessed_offset);

      nReturnStatus = OEMCardSession_MapFromMMGSDIFileEnum(
                        pCnf->read_cnf.access.file.file_enum,
                        &pReadTpStatus->dwFileName);
      if (SUCCESS != nReturnStatus) {
        MSG_ERROR("Unable to match mmgsdi File 0x%x",
                   pCnf->read_cnf.access.file.file_enum, 0, 0);
        return EFAILED;
      }
    }
    else {
      pReadRecStatus->nCmdStatus = SUCCESS;
      pReadRecStatus->dwRecNum   =
        int32touint32(pCnf->read_cnf.accessed_rec_num);

      nReturnStatus = OEMCardSession_MapFromMMGSDIFileEnum(
                        pCnf->read_cnf.access.file.file_enum,
                        &pReadRecStatus->dwFileName);
      if (SUCCESS != nReturnStatus) {
        MSG_ERROR("Unable to match mmgsdi File 0x%x",
                   pCnf->read_cnf.access.file.file_enum, 0, 0);
        return EFAILED;
      }
    }

    /*The Client has malloced only pReadData->len data, copy only that*/
    if(pReadData->nLen > pCnf->read_cnf.read_data.data_len)
    {
      pReadData->nLen = pCnf->read_cnf.read_data.data_len;
    }

    (void)MEMCPY(pReadData->pData,
                 pCnf->read_cnf.read_data.data_ptr,
                 int32touint32(pReadData->nLen));

  }
  else {
    MSG_ERROR("Read of File 0x%x  Failed ",
              pCnf->read_cnf.access.file.file_enum,0,0);
  }

  return SUCCESS;
}


/*===========================================================================

FUNCTION: OEMCardSession_GetWriteResponse

DESCRIPTION
  This function processes the response for WRITE_CNF

DEPENDENCIES
  none

RETURN VALUE
  SUCCESS  - Successful handling of the procedure  
  EFAILED  - General Error 
  EBADPARM - Bad Input Parameters

SIDE EFFECTS
===========================================================================*/
static int OEMCardSession_GetWriteResponse(
  const OEMCardSessionAsynCBInfo *pAsynInfo,
  mmgsdi_return_enum_type         status,
  const mmgsdi_cnf_type          *pCnf)
{

  AEECardSessionWriteTpStatus  *pWriteTpStatus  = NULL;
  AEECardSessionWriteRecStatus *pWriteRecStatus = NULL;
  boolean                       bRecFile        = TRUE;
  int                           nReturnStatus;


  if((NULL == pAsynInfo  ) ||
     (NULL == pCnf)) {
    return EBADPARM;
  }

  if (pCnf->write_cnf.accessed_rec_num == 0) {
    bRecFile = FALSE;
  }

  if(FALSE == bRecFile){
    /*Response for Transparent File*/
    pWriteTpStatus  = (AEECardSessionWriteTpStatus*)pAsynInfo->pClientData;

    if (NULL == pWriteTpStatus) {
      return EFAILED;
    }
    pWriteTpStatus->dwOffset     =
      int32touint32(pCnf->write_cnf.accessed_offset);
    pWriteTpStatus->nCmdStatus = EFAILED;

    if (MMGSDI_SUCCESS == status){
      nReturnStatus = OEMCardSession_MapFromMMGSDIFileEnum(
                        pCnf->write_cnf.access.file.file_enum,
                        &pWriteTpStatus->dwFileName);
      if (SUCCESS != nReturnStatus) {
        MSG_HIGH("Write to File 0x%x  Fail due to mmgsdi file 0x%x not found",
             pCnf->write_cnf.access.file.file_enum,0,0);
        return EFAILED;
      }
      pWriteTpStatus->nCmdStatus = SUCCESS;
    }

  }
  else {
    /*Response for Record Based File*/
    pWriteRecStatus  = (AEECardSessionWriteRecStatus*)pAsynInfo->pClientData;

    if (NULL == pWriteRecStatus) {
      return EFAILED;
    }
    pWriteRecStatus->dwRecNum      =
      int32touint32(pCnf->write_cnf.accessed_rec_num);
    pWriteRecStatus->nCmdStatus    = EFAILED;

    if (MMGSDI_SUCCESS == status){
      nReturnStatus = OEMCardSession_MapFromMMGSDIFileEnum
                        (pCnf->write_cnf.access.file.file_enum,
                        &pWriteRecStatus->dwFileName);
      if (SUCCESS != nReturnStatus) {
        MSG_HIGH("Write to File 0x%x  Fail due to mmgsdi file 0x%x not found",
             pCnf->write_cnf.access.file.file_enum,0,0);
        return EFAILED;
      }
      pWriteRecStatus->nCmdStatus = SUCCESS;
    }
  }


  if (MMGSDI_SUCCESS == status) {
    MSG_MED("Write to File 0x%x  Successful",
             pCnf->write_cnf.access.file.file_enum,
             0,0);

  }
  else {
    MSG_ERROR("Write to File 0x%x  Failed ",
              pCnf->write_cnf.access.file.file_enum,0,0);
  }

  return SUCCESS;
}


/*===========================================================================

FUNCTION: OEMCardSession_GetFileAttrResp

DESCRIPTION
  This function processes the response for FILE_ATTRIB_CNF

DEPENDENCIES
  none

RETURN VALUE
  SUCCESS  - Successful handling of the procedure  
  EFAILED  - General Error 
  EBADPARM - Bad Input Parameters

SIDE EFFECTS
===========================================================================*/
static int OEMCardSession_GetFileAttrResp(
  const OEMCardSessionAsynCBInfo *pAsynInfo,
  mmgsdi_return_enum_type         status,
  const mmgsdi_cnf_type          *pCnf)
{

  AEECardSessionFileAttribStatus           *pFileAttrib  = NULL;
  int                                       nReturnStatus= SUCCESS;
  const mmgsdi_file_attributes_type        *pGSDIFileAttr= NULL;
  const mmgsdi_linear_fixed_file_info_type *pLinearFile  = NULL;
  const mmgsdi_cyclic_file_info_type       *pCyclicFile  = NULL;
  const mmgsdi_transparent_file_info_type  *pTransparent = NULL;

  if((NULL == pAsynInfo  ) ||
     (NULL == pCnf)) {
    return EBADPARM;
  }

  pFileAttrib  = (AEECardSessionFileAttribStatus*)pAsynInfo->pClientData;

  if (NULL == pFileAttrib) {
    return EFAILED;
  }

  (void)MEMSET(pFileAttrib, 0, sizeof(AEECardSessionFileAttribStatus));

  if (MMGSDI_SUCCESS != status){
      MSG_ERROR("Get File Attrib Resp Failed , status = 0x%x",status,0,0);
      pFileAttrib->nCmdStatus    = EFAILED;
    return SUCCESS;
  }

  pGSDIFileAttr = &pCnf->get_file_attr_cnf.file_attrib;

  pFileAttrib->dwFileSize = int32touint32(pGSDIFileAttr->file_size);


  (void)MEMCPY(pFileAttrib->u8FileID, pGSDIFileAttr->file_id,
               AEECARDSESSION_FILE_ID_LEN);

  switch(pGSDIFileAttr->file_type) {

    case MMGSDI_MASTER_FILE:
      pFileAttrib->dwFileType = AEECARDSESSION_FTYPE_MASTER_FILE;
      break;

    case MMGSDI_DEDICATED_FILE:
      pFileAttrib->dwFileType = AEECARDSESSION_FTYPE_DEDICATED_FILE;
      break;

    case MMGSDI_TRANSPARENT_FILE:
      /*Get the File Access Security Condition for the Transparent File*/
        pFileAttrib->dwFileType      = AEECARDSESSION_FTYPE_ELEMENTARY_FILE;
      pFileAttrib->dwFileStructure = AEECARDSESSION_FSTRUCTURE_TRANSPARENT;
      pTransparent = &pGSDIFileAttr->file_info.transparent_file;

      nReturnStatus = OEMCardSession_MapFileProtection(
                        &(pFileAttrib->file.sTpFileAttrStatus.sFileProtection),
                        &pTransparent->file_security);

      if (TRUE == pTransparent->file_status.file_invalidated){
        pFileAttrib->file.sTpFileAttrStatus.bFileInvalidated = 1;
      }

      if (TRUE == pTransparent->file_status.read_write_when_invalidated) {
         pFileAttrib->file.sTpFileAttrStatus.bRWWhenInvalidated = 1;
      }
      break;

    case MMGSDI_CYCLIC_FILE:
      /*Get the File Access Security Condition for the Cyclic File*/
        pFileAttrib->dwFileType      = AEECARDSESSION_FTYPE_ELEMENTARY_FILE;
      pFileAttrib->dwFileStructure = AEECARDSESSION_FSTRUCTURE_CYCLIC;
      pCyclicFile = &pGSDIFileAttr->file_info.cyclic_file;

      nReturnStatus = OEMCardSession_MapFileProtection(
                        &(pFileAttrib->file.sCyclicFileStatus.sFileProtection),
                        &pCyclicFile->file_security);

      if(TRUE == pCyclicFile->increase_allowed){
        pFileAttrib->file.sCyclicFileStatus.bCyclicIncreaseAllowed = 1;
      }

      if(TRUE == pCyclicFile->file_status.file_invalidated){
        pFileAttrib->file.sCyclicFileStatus.bFileInvalidated = 1;
      }
      if (TRUE == pCyclicFile->file_status.read_write_when_invalidated) {
         pFileAttrib->file.sCyclicFileStatus.bRWWhenInvalidated = 1;
      }

      break;

    case MMGSDI_LINEAR_FIXED_FILE:
      /*Get the File Access Security Condition for the Linear Fixed File*/
        pFileAttrib->dwFileType      = AEECARDSESSION_FTYPE_ELEMENTARY_FILE;
      pFileAttrib->dwFileStructure = AEECARDSESSION_FSTRUCTURE_LINEAR;
      pLinearFile  = &pGSDIFileAttr->file_info.linear_fixed_file;

      pFileAttrib->file.sLinearFileStatus.dwRecordLen     =
        int32touint32(pLinearFile->rec_len);

      pFileAttrib->file.sLinearFileStatus.dwNumberRecords =
        int32touint32(pLinearFile->num_of_rec);

      nReturnStatus = OEMCardSession_MapFileProtection(
                        &(pFileAttrib->file.sLinearFileStatus.sFileProtection),
                        &pLinearFile->file_security);

      if(TRUE == pLinearFile->file_status.file_invalidated ){
        pFileAttrib->file.sLinearFileStatus.bFileInvalidated = 1;
      }

      if (TRUE == pLinearFile->file_status.read_write_when_invalidated) {
         pFileAttrib->file.sLinearFileStatus.bRWWhenInvalidated = 1;
      }
      break;
    default:
      (void)MEMSET(pFileAttrib, 0, sizeof(AEECardSessionFileAttribStatus));
      pFileAttrib->nCmdStatus    = EFAILED;
      return SUCCESS;
  }


  if (SUCCESS == nReturnStatus) {
    MSG_MED("Get File Attrib Successful, File Type 0x0%x",
             pGSDIFileAttr->file_type,
             0,0);
      pFileAttrib->nCmdStatus    = SUCCESS;

  }
  else {
    MSG_ERROR("Get File Attrib UnSuccessful, File Type 0x0%x",
               pGSDIFileAttr->file_type,
               0,0);
    (void)MEMSET(pFileAttrib, 0, sizeof(AEECardSessionFileAttribStatus));
      pFileAttrib->nCmdStatus    = EFAILED;
  }

  return SUCCESS;
}


/*===========================================================================
FUNCTION: OEMCardSession_PinOpResp

DESCRIPTION
  This function processes the response for PIN_OPERATION_CNF

DEPENDENCIES
  none

RETURN VALUE
  SUCCESS  - Successful handling of the procedure  
  EFAILED  - General Error 
  EBADPARM - Bad Input Parameters

SIDE EFFECTS
===========================================================================*/
static int OEMCardSession_PinOpResp(
  const OEMCardSessionAsynCBInfo *pAsynInfo,
  mmgsdi_return_enum_type         status,
  const mmgsdi_cnf_type          *pCnf)
{

  AEECardSessionPinOpStatus        *pPinOp  = NULL;
  int                               nReturnStatus= SUCCESS;

  if((NULL == pAsynInfo  ) ||
     (NULL == pCnf)) {
    return EBADPARM;
  }

  pPinOp  = (AEECardSessionPinOpStatus*)pAsynInfo->pClientData;

  if (NULL == pPinOp) {
    return EFAILED;
  }

  /* Do not memset pPinOp since there are data that are available
     during the calling function */

  if (MMGSDI_SUCCESS != status){
    MSG_ERROR("PIN Op Resp Failed , mmgsdi status = 0x%x",status,0,0);
    pPinOp->nCmdStatus = EFAILED;
  }
  else
  {
    pPinOp->nCmdStatus = SUCCESS;
  }

  nReturnStatus = OEMCardSession_MapFromMMGSDIPinID(
                    pCnf->pin_operation_cnf.pin_info.pin_id,
                    &pPinOp->dwPinID);
  if (SUCCESS != nReturnStatus) {
    pPinOp->nCmdStatus = nReturnStatus;
  }

  nReturnStatus = OEMCardSession_MapFromMMGSDIPinStatus
                    (pCnf->pin_operation_cnf.pin_info,
                    &pPinOp->dwNumPinRetries,
                    &pPinOp->dwNumPukRetries,
                    &pPinOp->dwPinStatus);
  if (SUCCESS != nReturnStatus) {
    pPinOp->nCmdStatus = nReturnStatus;
  }

  nReturnStatus = OEMCardSession_MapFromMMGSDIPinOp(
                    pCnf->pin_operation_cnf.pin_op,
                    &pPinOp->dwPinOp);
  if (SUCCESS != nReturnStatus) {
    pPinOp->nCmdStatus = nReturnStatus;
  }

  return SUCCESS;
}


/*===========================================================================
FUNCTION: OEMCardSession_GetAllPinStatusResp

DESCRIPTION
  This function processes the response for GET_ALL_PIN_STATUS_CNF

DEPENDENCIES
  none

RETURN VALUE
  SUCCESS  - Successful handling of the procedure  
  EFAILED  - General Error 
  EBADPARM - Bad Input Parameters

SIDE EFFECTS
===========================================================================*/
static int OEMCardSession_GetAllPinStatusResp(
  const OEMCardSessionAsynCBInfo *pAsynInfo,
  mmgsdi_return_enum_type         status,
  const mmgsdi_cnf_type          *pCnf)
{

  AEECardSessionCardAllPinsInfo    *pGetAllPinsInfo  = NULL;
  int                               nReturnStatus    = SUCCESS;

  if((NULL == pAsynInfo  ) ||
     (NULL == pCnf)) {
    return EBADPARM;
  }

  pGetAllPinsInfo  = (AEECardSessionCardAllPinsInfo*)pAsynInfo->pClientData;

  if (NULL == pGetAllPinsInfo) {
    return EFAILED;
  }

  /* Do not memset pGetAllPinsInfo since there are data that are
     available during the calling function */

  if (MMGSDI_SUCCESS != status){
    MSG_ERROR("Get All PINs Info Resp Failed , mmsgdi status = 0x%x",
              status,0,0);
    pGetAllPinsInfo->nCmdStatus = EFAILED;
  }
  else
  {
    pGetAllPinsInfo->nCmdStatus = SUCCESS;
  }

  nReturnStatus = OEMCardSession_MapFromMMGSDIPinStatus(
                    pCnf->get_all_pin_status_cnf.pin1,
                    &pGetAllPinsInfo->dwPin1NumRetries,
                    &pGetAllPinsInfo->dwPuk1NumRetries,
                    &pGetAllPinsInfo->dwPin1Status);
  if (SUCCESS != nReturnStatus) {
    pGetAllPinsInfo->nCmdStatus = nReturnStatus;
  }

  nReturnStatus = OEMCardSession_MapFromMMGSDIPinStatus(
                    pCnf->get_all_pin_status_cnf.pin2,
                    &pGetAllPinsInfo->dwPin2NumRetries,
                    &pGetAllPinsInfo->dwPuk2NumRetries,
                    &pGetAllPinsInfo->dwPin2Status);
  if (SUCCESS != nReturnStatus) {
    pGetAllPinsInfo->nCmdStatus = nReturnStatus;
  }

  nReturnStatus = OEMCardSession_MapFromMMGSDIPinStatus(
                    pCnf->get_all_pin_status_cnf.universal_pin,
                    &pGetAllPinsInfo->dwPinUniversalNumRetries,
                    &pGetAllPinsInfo->dwPukUniversalNumRetries,
                    &pGetAllPinsInfo->dwPinUniversalStatus);
  if (SUCCESS != nReturnStatus) {
    pGetAllPinsInfo->nCmdStatus = nReturnStatus;
  }

  return SUCCESS;
}


/*===========================================================================
FUNCTION: OEMCardSession_SelectAidResp

DESCRIPTION
  This function processes the response for SELECT_AID_CNF

DEPENDENCIES
  none

RETURN VALUE
  SUCCESS  - Successful handling of the procedure  
  EFAILED  - General Error 
  EBADPARM - Bad Input Parameters

SIDE EFFECTS
===========================================================================*/
static int OEMCardSession_SelectAidResp(
  const OEMCardSessionAsynCBInfo *pAsynInfo,
  mmgsdi_return_enum_type         status,
  const mmgsdi_cnf_type          *pCnf)
{

  AEECardSessionCmdStatus    *pSelectAid  = NULL;

  if((NULL == pAsynInfo  ) ||
     (NULL == pCnf)) {
    return EBADPARM;
  }

  pSelectAid  = (AEECardSessionCmdStatus*)pAsynInfo->pClientData;

  if (NULL == pSelectAid) {
    return EFAILED;
  }

  /* Do not memset pSelectAid since there are data that are
     available during the calling function */

  if (MMGSDI_SUCCESS != status){
      MSG_ERROR("Select AID Info Resp Failed , mmgsdi status = 0x%x",
                status,0,0);
      pSelectAid->nCmdStatus = EFAILED;
  }
  else
  {
    pSelectAid->nCmdStatus = SUCCESS;
  }

  return SUCCESS;
}


#ifdef FEATURE_MMGSDI_MBMS
#error code not present
#endif /*FEATURE_MMGSDI_MBMS*/


#ifdef FEATURE_UIM_JCDMA_RUIM_SUPPORT
/*===========================================================================
FUNCTION: OEMCardSession_GetJCDMACardInfoResp

DESCRIPTION
  This function processes the response for JCDMA_CARD_GET_CARD_INFO_CNF 

DEPENDENCIES
  none

RETURN VALUE
  SUCCESS  - Successful handling of the procedure  
  EFAILED  - General Error 
  EBADPARM - Bad Input Parameters

SIDE EFFECTS
===========================================================================*/
static int OEMCardSession_GetJCDMACardInfoResp(
  const OEMCardSessionAsynCBInfo *pAsynInfo,
  mmgsdi_return_enum_type         status,
  const mmgsdi_cnf_type          *pCnf)
{

  AEECardSessionJCDMACardInfo      *pJCDMACardInfo   = NULL;

  MSG_HIGH("OEMCardSession_GetJCDMACardInfoResp, processing mmgsdi response", 
           0,0,0);
  if((NULL == pAsynInfo  ) ||
     (NULL == pCnf)) {
    MSG_ERROR("OEMCardSession_GetJCDMACardInfoResp, bad input", 
           0,0,0);
    return EBADPARM;
  }

  pJCDMACardInfo  = (AEECardSessionJCDMACardInfo*)pAsynInfo->pClientData;

  if (NULL == pJCDMACardInfo) {
    MSG_ERROR("OEMCardSession_GetJCDMACardInfoResp, pJCDMACardInfo NULL", 
           0,0,0);
    return EFAILED;
  }

  /* Do not memset pJCDMACardInfo since there are data that are
     available during the calling function */

  if (MMGSDI_SUCCESS != status){
    MSG_ERROR("Get JCDMA Card Info Resp Failed , mmsgdi status = 0x%x",
              status,0,0);
    pJCDMACardInfo->nCmdStatus = EFAILED;
  }
  else
  {
    pJCDMACardInfo->nCmdStatus = OEMCardSession_MapFromMMGSDIESNMEChange(
                                   pCnf->get_jcdma_card_info_cnf.me_esn_change,
                                  &pJCDMACardInfo->nMEESNChange);
    MSG_HIGH("OEMCardSession_MapFromMMGSDIESNMEChange, nCmdStatus is %x", 
             pJCDMACardInfo->nCmdStatus, 0, 0);
    if (SUCCESS == pJCDMACardInfo->nCmdStatus) 
    {
      pJCDMACardInfo->nCmdStatus = OEMCardSession_MapFromMMGSDIJCDMACardInfo(
                                    pCnf->get_jcdma_card_info_cnf.card_info,
                                    &pJCDMACardInfo->nInfo);
      MSG_HIGH("OEMCardSession_GetJCDMACardInfoResp, nCmdStatus is %x", 
                pJCDMACardInfo->nCmdStatus, 0, 0);
    }
    /* If OEMCardSession_MapFromMMGSDIESNMEChange is not a SUCCESS then 
       pJCDMACardInfo->nCmdStatus will have the error code from that function
       we still return SUCCESS because the Resposnse for 
       was processesd JCDMA_CARD_GET_CARD_INFO_CNF */
  }

  return SUCCESS;
}
#endif /* FEATURE_UIM_JCDMA_RUIM_SUPPORT */


#ifdef FEATURE_MMGSDI_MFLO
#error code not present
#endif /*FEATURE_MMGSDI_MFLO*/


/*===========================================================================

FUNCTION: OEMCardSession_GetCardReaderStatusResp

DESCRIPTION
  This function processes the response for CARD_READER_STATUS_CNF

DEPENDENCIES
  none

RETURN VALUE
  SUCCESS  - Successful handling of the procedure  
  EFAILED  - General Error 
  EBADPARM - Bad Input Parameters

SIDE EFFECTS
===========================================================================*/
static int OEMCardSession_GetCardReaderStatusResp(
  const OEMCardSessionAsynCBInfo *pAsynInfo,
  mmgsdi_return_enum_type         status,
  const mmgsdi_cnf_type          *pCnf)
{

  AEECardSessionCardReaderStatus  *pCardReaderStatus   = NULL;
  uint8                            mmgsdiReaderStatus  = 0;

  if((NULL == pAsynInfo  ) ||
     (NULL == pCnf       )) {
    MSG_ERROR("pAsynInfo or pCnf is NULL",0,0,0);
    return EBADPARM;
  }

  pCardReaderStatus  = (AEECardSessionCardReaderStatus*)pAsynInfo->pClientData;
  if (NULL == pCardReaderStatus) {
   MSG_ERROR("pCardReaderStatus is NULL",0,0,0);
   return EFAILED;
  }

  pCardReaderStatus->nCmdStatus = EFAILED;
  if (MMGSDI_SUCCESS == status) {
    MSG_HIGH("Get Card Reader Successful, card status 0x%x ",
      pCnf->card_reader_status_cnf.card_reader_status_data.data_ptr[0],0,0);
    pCardReaderStatus->nCmdStatus = SUCCESS;
    mmgsdiReaderStatus = 
      pCnf->card_reader_status_cnf.card_reader_status_data.data_ptr[0];
    pCardReaderStatus->nCardReaderID = mmgsdiReaderStatus & 
                                       OEMCARDSESSION_CARD_READER_ID_MASK;
    pCardReaderStatus->bRemovable   = (mmgsdiReaderStatus >>
                                       OEMCARDSESSION_CARD_READER_REMOVABLE_SHIFT) & 
                                       OEMCARDSESSION_CARD_READER_INFO_MASK;
    pCardReaderStatus->bPresent     = (mmgsdiReaderStatus  >>
                                       OEMCARDSESSION_CARD_READER_PRESENT_SHIFT) & 
                                       OEMCARDSESSION_CARD_READER_INFO_MASK;
    pCardReaderStatus->bID1Size     = (mmgsdiReaderStatus  >>
                                       OEMCARDSESSION_CARD_READER_ID1SIZE_SHIFT) & 
                                       OEMCARDSESSION_CARD_READER_INFO_MASK;
    pCardReaderStatus->bCardPresent = (mmgsdiReaderStatus >>
                                       OEMCARDSESSION_CARD_PRESENT_IN_READER_SHIFT) & 
                                       OEMCARDSESSION_CARD_READER_INFO_MASK;
    pCardReaderStatus->bCardPowered = (mmgsdiReaderStatus >>
                                       OEMCARDSESSION_CARD_POWERED_IN_READER_SHIFT) & 
                                       OEMCARDSESSION_CARD_READER_INFO_MASK;
  }

  MSG_HIGH("OEMCardSession_GetCardReaderStatusResp status 0x%x", 
            pCardReaderStatus->nCmdStatus, 0, 0);
  return SUCCESS;
}


/*===========================================================================

FUNCTION: OEMCardSession_GetBasicDataResp

DESCRIPTION
  This function processes the response for SIGN_UCAST_CNF, GET_ATR_CNF,
  SEND_APDU_CNF etc

DEPENDENCIES
  none

RETURN VALUE
  SUCCESS  - Successful handling of the procedure  
  EFAILED  - General Error 
  EBADPARM - Bad Input Parameters

SIDE EFFECTS
===========================================================================*/
static int OEMCardSession_GetBasicDataResp(
  const OEMCardSessionAsynCBInfo *pAsynInfo,
  mmgsdi_return_enum_type         status,
  mmgsdi_cnf_enum_type            cnf,
  const mmgsdi_cnf_type          *pCnf)
{

  AEECardSessionCmdDataStatus  *pCmdStatus   = NULL;
  int                           nMsgLen       = 0;
  int32                         cnf_data_len = 0;
  uint8                        *cnf_data_ptr = NULL;

  if((NULL == pAsynInfo  ) ||
     (NULL == pCnf       )) {
    MSG_ERROR("pAsynInfo or pCnf is NULL",0,0,0);
    return EBADPARM;
  }

  pCmdStatus  = (AEECardSessionCmdDataStatus*)pAsynInfo->pClientData;
  if (NULL == pCmdStatus) {
   MSG_ERROR("pCmdStatus is NULL",0,0,0);
   return EFAILED;
  }

  pCmdStatus->nCmdStatus = EFAILED;
  if (MMGSDI_SUCCESS == status) {
    MSG_HIGH("mmgsdi cnf 0x%x Successful ",cnf,0,0);
    pCmdStatus->nCmdStatus = SUCCESS;

    if (pCmdStatus->sData.pData) {
      switch (cnf)
      {
      case MMGSDI_GET_ATR_CNF:
        cnf_data_len = pCnf->get_atr_cnf.atr_data.data_len; 
        cnf_data_ptr = pCnf->get_atr_cnf.atr_data.data_ptr;
        break;
      case MMGSDI_MFLO_SIGN_UCAST_CNF:
        cnf_data_len = pCnf->mflo_sign_ucast_cnf.ucast_msg.data_len; 
        cnf_data_ptr = pCnf->mflo_sign_ucast_cnf.ucast_msg.data_ptr;
        break;
      case MMGSDI_SEND_APDU_CNF:
        cnf_data_len = pCnf->send_apdu_cnf.apdu_data.data_len; 
        cnf_data_ptr = pCnf->send_apdu_cnf.apdu_data.data_ptr;
        break;
      default:
        MSG_ERROR("Unknown Cnf in OEMCardSession_GetBasicDataResp", 0, 0, 0);
        return EFAILED;
      }

      (void) MEMSET(pCmdStatus->sData.pData, 0x00, int32touint32(pCmdStatus->sData.nLen));
      nMsgLen = MIN(pCmdStatus->sData.nLen, cnf_data_len);
      if ((nMsgLen > 0) && cnf_data_ptr) {
        (void) MEMCPY(pCmdStatus->sData.pData, cnf_data_ptr, int32touint32(nMsgLen));
      }
      pCmdStatus->sData.nLen = nMsgLen;
    }
  }

  MSG_HIGH("OEMCardSession_GetBasicDataResp for cnf 0x%x, status 0x%x", 
            cnf, pCmdStatus->nCmdStatus, 0);
  return SUCCESS;
}


/*===========================================================================

FUNCTION: OEMCardSession_RefreshRsp

DESCRIPTION
  This function processes the response for Refresh Reg/ Dereg/ OK to FCN/
  OK To Init / Refresh Complete

DEPENDENCIES
  none

RETURN VALUE
  SUCCESS    - Status information is valid
  EBADPARM   - Bad parameter
  EUNSUPPORT - Not supported

SIDE EFFECTS
===========================================================================*/
static int OEMCardSession_RefreshRsp(
  mmgsdi_cnf_enum_type            cnf,
  const OEMCardSessionAsynCBInfo *pAsynInfo,
  mmgsdi_return_enum_type         status,
  const mmgsdi_cnf_type          *pCnf)
{
  AEECardSessionCmdStatus    *pStatus  = NULL;

  MSG_HIGH("In OEMCardSession_RefreshRsp with cnf type0x%x",cnf,0,0);

  if((NULL == pAsynInfo  ) ||
     (NULL == pCnf)) {
    MSG_ERROR("pAsynInfo or pCnf is NULL",0,0,0);
    return EBADPARM;
  }

  if(cnf != MMGSDI_REFRESH_CNF) {
    MSG_ERROR("Unhandled CNF from MMGSDI",0,0,0);
    return EBADPARM;
  }

  MSG_HIGH("Response received for Refresh Registration 0x%x",pCnf->refresh_cnf.orig_refresh_req,0,0);

  switch(pCnf->refresh_cnf.orig_refresh_req)
  {
  case MMGSDI_REFRESH_ORIG_REG_REQ:
      MSG_HIGH("Response received for Refresh Registration ",0,0,0);
      break;
  case MMGSDI_REFRESH_ORIG_DEREG_REQ:
      MSG_HIGH("Response received for Refresh DeRegistration  ",0,0,0);
      break;
  case MMGSDI_REFRESH_ORIG_OK_TO_REFRESH_REQ:
      MSG_HIGH("Response received for OK to Refresh  ",0,0,0);
      break;
  case MMGSDI_REFRESH_ORIG_COMPLETE_REQ:
      MSG_HIGH("Response received for Refresh Complete  ",0,0,0);
      break;
  default:
      MSG_HIGH("Unhandled Refresh Response  0x%x",pCnf->refresh_cnf.orig_refresh_req,0,0);
      break;
  }

  pStatus  = (AEECardSessionCmdStatus*)pAsynInfo->pClientData;

  if (NULL == pStatus) {
    MSG_ERROR("pStatus is NULL",0,0,0);
    return EFAILED;
  }

  if (MMGSDI_SUCCESS != status) {
    MSG_HIGH("Refresh Rsp is a FAILURE",0,0,0);
    pStatus->nCmdStatus = EFAILED;
  }
  else {
    MSG_HIGH("Refresh Rsp is a SUCCESS",0,0,0);
    pStatus->nCmdStatus = SUCCESS;
  }
  return SUCCESS;
}/* OEMCardSession_RefreshRsp */


/*===========================================================================

FUNCTION: OEMCardSessionFreeCb

DESCRIPTION
  This function is called when OEMCardSession calls MMGSDI during normal
  operation channel management related procedures.
  This will cause a callback to be issued to the caller.

  For the Authentication Response, the RES/CK/IK Lengths and the AUTS Length
  are exclusive of each other

DEPENDENCIES
  none

RETURN VALUE
  none

SIDE EFFECTS
===========================================================================*/
/*lint -e{715} suppress "pCnf not referenced" */
static void OEMCardSessionFreeCb (
  mmgsdi_return_enum_type status,
  mmgsdi_cnf_enum_type    cnf,
  const mmgsdi_cnf_type  *pCnf)
{
  MSG_HIGH("OEMCardSessionFreeCb: Cmd = 0x%x, Status = 0x%x",cnf,status,0);
  return;
}


/*===========================================================================

FUNCTION: OEMCardSessionInternalCb

DESCRIPTION
  This function is called when OEMCardSession calls MMGSDI during release
  procedure and when there is a need to close the session prior to
  releasing the ICardSession object

DEPENDENCIES
  none

RETURN VALUE
  none

SIDE EFFECTS
===========================================================================*/
void OEMCardSessionInternalCb (
  mmgsdi_return_enum_type status,
  mmgsdi_cnf_enum_type    cnf,
  const mmgsdi_cnf_type  *pCnf)
{
  MSG_HIGH("OEMCardSessionInternalCb status 0x%x, cnf 0x%x", status, cnf, 0);

  if (NULL == pCnf) {
    MSG_ERROR("pCnf is NULL",0,0,0);
    return;
  }

#ifdef FEATURE_MMGSDI
  if (MMGSDI_SESSION_CLOSE_CNF == cnf) {
    if (MMGSDI_SUCCESS == status){
      uint64 client_id = pCnf->session_close_cnf.session_id;
      mmgsdi_return_enum_type mmgsdi_status;

      mmgsdi_status =  mmgsdi_client_id_dereg (client_id,
                                               OEMCardSessionFreeCb,
                                               0);
      if (MMGSDI_SUCCESS != mmgsdi_status){
        MSG_ERROR("Could not deregister Client",0,0,0);
      }
    }
  }
#endif /*FEATURE_MMGSDI*/

}


/*===========================================================================

FUNCTION: OEMCardSessionCmdCb

DESCRIPTION
  This function is called when OEMCardSession calls MMGSDI during normal
  operation channel management related procedures.
  This will cause a callback to be issued to the caller.

  For the Authentication Response, the RES/CK/IK Lengths and the AUTS Length
  are exclusive of each other
  RES/CK/IK  will be present when the authentication is performed successfully.
  AUTS will be present when a sync failure occurs.

DEPENDENCIES
  none

RETURN VALUE
  none

SIDE EFFECTS
===========================================================================*/
void OEMCardSessionCmdCb (
  mmgsdi_return_enum_type status,
  mmgsdi_cnf_enum_type    cnf,
  const mmgsdi_cnf_type  *pCnf)
{
  ICardSession                  *pMe;
  OEMCardSessionDataType        *pClientData = NULL;
  OEMCardSessionAsynCBInfo      *pAsynInfo   = NULL;
  AEEObjectHandle                Handler = 0;
  int                            nReturnStatus = SUCCESS;

  MSG_HIGH("OEMCardSessionCmdCb status 0x%x, cnf 0x%x", status, cnf, 0);

  if (NULL == pCnf) {
    return;
  }

  pClientData = (OEMCardSessionDataType*)pCnf->response_header.client_data;

  if (NULL == pClientData) {
    return;
  }

  OEMCARDSESSION_ENTER_CRITICAL_SECTION
  Handler = pClientData->ObjHandler;
  pAsynInfo = (OEMCardSessionAsynCBInfo*)AEEObjectMgr_GetObject(Handler);
  OEMCARDSESSION_LEAVE_CRITICAL_SECTION

  if (NULL == pAsynInfo) {
    MSG_ERROR("pAsyncInfo is NULL", 0, 0, 0);
    FREE( pClientData );
    pClientData = NULL;
    return;
  }

  if (NULL == pAsynInfo->pCB) {
    MSG_ERROR("pAsyncInfo->pCB is NULL", 0, 0, 0);
    FREE( pClientData );
    pClientData = NULL;
    FREE(pAsynInfo);
    return;
  }

  pMe = (ICardSession*)pClientData->m_pCardSession;

  if (NULL == pMe) {
    MSG_ERROR("pMe is NULL", 0, 0, 0);
    FREE( pClientData );
    pClientData = NULL;
    FREE(pAsynInfo);
    return;
  }

  switch (cnf) {
    case MMGSDI_CLIENT_ID_REG_CNF:
      nReturnStatus = OEMCardSession_RegistrationResp(pMe, 
                                                      pAsynInfo,
                                                      status,
                                                      pCnf);
      break;

  case MMGSDI_CLIENT_EVT_REG_CNF:
      nReturnStatus = OEMCardSession_EvtRegistrationResp(pMe, 
                                                         pAsynInfo,
                                                         status,
                                                         pCnf);
      break;

    case MMGSDI_SESSION_OPEN_CNF:
      nReturnStatus = OEMCardSession_GetOpenResponse(pMe,
                                                     pAsynInfo,
                                                     status,
                                                     pCnf);
      break;

    case MMGSDI_SESSION_CLOSE_CNF:
      nReturnStatus = OEMCardSession_GetCloseResponse(pMe,
                                                      pAsynInfo,
                                                      status);
      break;

    case MMGSDI_ISIM_AUTH_CNF:
      nReturnStatus = OEMCardSession_GetAuthResponse(pAsynInfo,
                                                     status,
                                                     pCnf);
      break;

    case MMGSDI_READ_CNF:
      nReturnStatus = OEMCardSession_GetReadResponse(pAsynInfo,
                                                     status,
                                                     pCnf);
      break;


    case MMGSDI_WRITE_CNF:
      nReturnStatus = OEMCardSession_GetWriteResponse(pAsynInfo,
                                                      status,
                                                      pCnf);
      break;

    case MMGSDI_GET_FILE_ATTR_CNF:
      nReturnStatus = OEMCardSession_GetFileAttrResp(pAsynInfo,
                                                     status,
                                                     pCnf);
      break;

    case MMGSDI_PIN_OPERATION_CNF:
      nReturnStatus = OEMCardSession_PinOpResp(pAsynInfo,
                                               status,
                                               pCnf);
      break;
    case MMGSDI_GET_ALL_PIN_STATUS_CNF:
      nReturnStatus = OEMCardSession_GetAllPinStatusResp(pAsynInfo,
                                                         status,
                                                         pCnf);
      break;
    case MMGSDI_SELECT_AID_CNF:
      nReturnStatus = OEMCardSession_SelectAidResp(pAsynInfo,
                                                   status,
                                                   pCnf);
      break;
    case MMGSDI_SRV_AVAILABLE_CNF:
#ifdef FEATURE_MMGSDI_MBMS
#error code not present
#endif /*FEATURE_MMGSDI_MBMS*/
      break;
    case MMGSDI_USIM_AUTH_CNF:
#ifdef FEATURE_MMGSDI_MBMS
#error code not present
#endif /*FEATURE_MMGSDI_MBMS*/
      break;
    case MMGSDI_JCDMA_GET_CARD_INFO_CNF:
#ifdef FEATURE_UIM_JCDMA_RUIM_SUPPORT
      nReturnStatus = OEMCardSession_GetJCDMACardInfoResp(pAsynInfo,
                                                          status,
                                                          pCnf);
#endif /* FEATURE_UIM_JCDMA_RUIM_SUPPORT */
      break;   
    case MMGSDI_MFLO_INIT_ACT_PARAMS_CNF:
#ifdef FEATURE_MMGSDI_MFLO
#error code not present
#endif /* FEATURE_MMGSDI_MFLO */
      break;  
    case MMGSDI_MFLO_GET_SUBSCRIBER_ID_CNF:
#ifdef FEATURE_MMGSDI_MFLO
#error code not present
#endif /* FEATURE_MMGSDI_MFLO */
      break;  
    case MMGSDI_MFLO_GET_PUBLIC_KEY_CNF:
#ifdef FEATURE_MMGSDI_MFLO
#error code not present
#endif /* FEATURE_MMGSDI_MFLO */
      break;  
    case MMGSDI_MFLO_SIGN_UCAST_CNF:
#ifdef FEATURE_MMGSDI_MFLO
#error code not present
#endif /* FEATURE_MMGSDI_MFLO */
      break;  
    case MMGSDI_MFLO_VERIFY_UCAST_CNF:
#ifdef FEATURE_MMGSDI_MFLO
#error code not present
#endif /* FEATURE_MMGSDI_MFLO */
      break;  
    case MMGSDI_GET_ATR_CNF:
      nReturnStatus = OEMCardSession_GetBasicDataResp(pAsynInfo,
                                                      status,
                                                      cnf,
                                                      pCnf);
      break;   
    case MMGSDI_CARD_READER_STATUS_CNF:
      nReturnStatus = OEMCardSession_GetCardReaderStatusResp(pAsynInfo,
                                                             status,
                                                             pCnf);
      break;   
    case MMGSDI_SEND_APDU_CNF:
#ifdef FEATURE_MMGSDI_MFLO
#error code not present
#endif /* FEATURE_MMGSDI_MFLO */
      break;    
    case MMGSDI_REFRESH_CNF:
      nReturnStatus = OEMCardSession_RefreshRsp(cnf,
                                                pAsynInfo,
                                                status,
                                                pCnf);
     break;
   default:
      break;
  }

  /* Validate user callback to ensure its still valid */
  if (SUCCESS == nReturnStatus){
    if (pAsynInfo->pCB->pfnCancel != NULL) {
      AEE_ResumeCallback(pAsynInfo->pCB, 0);
    }
    /* No need to FREE pAsynInfo, since if the CancelCB is not null,
     the proper notification will be performed and then the
     OEMCardSession_Cancel will be called, where the pAsynInfo will
     be freed accordingly.
     In case of CancelCB is already NULL, this implies the the pAsynInfo
     has already been freed */
  }
  else
  {
    MSG_ERROR("Non Successful nReturnStatus 0x%x", nReturnStatus, 0, 0);
    /* Potential side effect is that client will not be notified for this
       failed response parsing command */
    FREE(pAsynInfo);
  }

  FREE( pClientData );
  pClientData = NULL;
}


/*---------------------------------------------------------------------------
                          CARD Session  FUNCTIONS
---------------------------------------------------------------------------*/
/*===========================================================================

FUNCTION: OEMCardSessionInitParams

DESCRIPTION
  This function initialzes the Client Data Parameters

DEPENDENCIES
  none

RETURN VALUE
  none

SIDE EFFECTS
===========================================================================*/
static int OEMCardSessionInitParams(
  ICardSession               *pMe,
  OEMCardSessionSlotEnum      u8SlotID,
  void                       *pData,
  AEECallback                *pUserCb,
  mmgsdi_slot_id_enum_type   *pSlot,
  OEMCardSessionDataType    **pClientData)
{
  int nReturnStatus;
  
  if ((NULL == pMe) || (NULL == pData) ||
      (NULL == pUserCb) || (NULL == pSlot)) {
    MSG_ERROR("Null Input Para", 0, 0, 0);
    return EBADPARM;
  }

  if (OEMCARDESESSION_DEFAULT_CLIENT_ID == pMe->m_client_id) {
    MSG_ERROR("No Client Id Set" ,0,0,0);
    return EINVALIDITEM;
  }
  
  /* convert the slot */
  nReturnStatus = OEMCardSession_MapMMGSDISlot(u8SlotID, pSlot);
  if (SUCCESS != nReturnStatus) {
    return nReturnStatus;
  }

  *pClientData = (OEMCardSessionDataType*)MALLOC(sizeof(OEMCardSessionDataType));
  if (NULL == *pClientData) {
    MSG_ERROR("Unable to Alloc pClientData", 0, 0, 0);
    return ENOMEMORY;
  }

  /* Get and populate the callback */
  nReturnStatus = OEMCardSession_GetLinkSysCallBack(
                    pUserCb,
                    pData,
                    &((*pClientData)->ObjHandler));
  if (SUCCESS != nReturnStatus) {
    FREE( *pClientData );
    *pClientData = NULL;
    return nReturnStatus;
  }

  (*pClientData)->m_pCardSession = pMe;

  return SUCCESS;
}
#endif /*FEATURE_MMGSDI*/


/*===========================================================================

FUNCTION: OEMCardSession_Open

DESCRIPTION

DEPENDENCIES
  none

RETURN VALUE
  none

SIDE EFFECTS
===========================================================================*/
/*lint -e{715} suppress "pMe, pCmdStatus, pAppID, pUserCB not referenced" */
/*lint -save -esym(818,pCmdStatus,pUserCB) suppress could be const */
int OEMCardSession_Open(
  ICardSession             *pMe,
  AEECardSessionCmdStatus  *pCmdStatus,
  const AEECardSessionData *pAppID,
  AEECallback              *pUserCB)
{
#ifdef FEATURE_MMGSDI
  OEMCardSessionDataType   *pClientData;
  mmgsdi_slot_id_enum_type  mmgsdi_slot   = MMGSDI_SLOT_1;
  int                       nReturnStatus = SUCCESS;
  mmgsdi_return_enum_type   mmgsdi_status = MMGSDI_ERROR;
  mmgsdi_data_type          mmgsdi_data;

  if ((NULL == pAppID) || (NULL == pAppID->pData) || (NULL == pMe)) {
    MSG_ERROR("Null Input Ptr 0x%x 0x%x", pAppID, pMe, 0);
    return EBADPARM;
  }

  MSG_HIGH("OEMCardSession_Open, Channel Status = %d",
           pMe->m_byChannelStatus,0,0);

  if ((OEMCARDSESSION_OPENED   == pMe->m_byChannelStatus) ||
     (OEMCARDSESSION_OPENING  == pMe->m_byChannelStatus) ||
     (OEMCARDSESSION_SELECTED == pMe->m_byChannelStatus)) {
    /* has already opened the channel */
    MSG_ERROR("Session already open",0,0,0);
    return EFAILED;
  }

  nReturnStatus = OEMCardSessionInitParams(pMe,
                                           pMe->m_u8Slot,
                                           (void*)pCmdStatus,
                                           pUserCB,
                                           &mmgsdi_slot,
                                           &pClientData);
  if (SUCCESS != nReturnStatus) {
    return nReturnStatus;
  }

  pCmdStatus->nCmdStatus = SUCCESS;

  pMe->m_byChannelStatus = OEMCARDSESSION_OPENING;

  mmgsdi_data.data_len = pAppID->nLen;
  mmgsdi_data.data_ptr = pAppID->pData;

  mmgsdi_status = mmgsdi_session_management_open (
                      pMe->m_client_id,
                      mmgsdi_slot,
                      mmgsdi_data,
                      OEMCardSessionCmdCb,
                      (mmgsdi_client_data_type)pClientData);
  if (MMGSDI_SUCCESS != mmgsdi_status) {
    OEMCardSession_CleanUpAsynObject(pClientData->ObjHandler);
    FREE( pClientData );
    pClientData = NULL;
    MSG_ERROR("Open Session Failed status = 0x%x",mmgsdi_status,0,0);
    return EFAILED;
  }

  return SUCCESS;
#else
  return EUNSUPPORTED;
#endif /* FEATURE_MMGSDI */
} /* OEMCardSession_Open */
/*lint -save +esym(818,pCmdStatus,pUserCB) suppress could be const */


/*===========================================================================
FUNCTION: OEMCardSession_Close

DESCRIPTION
  This function closes a  logical channel.

DEPENDENCIES
  none

RETURN VALUE
  none

SIDE EFFECTS

===========================================================================*/
/*lint -e{715} suppress "pMe, pCmdStatus, pUserCB not referenced" */
/*lint -save -esym(818,pCmdStatus,pUserCB) suppress could be const */
int OEMCardSession_Close(
  ICardSession            *pMe,
  AEECardSessionCmdStatus *pCmdStatus,
  AEECallback             *pUserCB)
{
#ifdef FEATURE_MMGSDI
  OEMCardSessionDataType   *pClientData   = NULL;
  mmgsdi_slot_id_enum_type  mmgsdi_slot   = MMGSDI_SLOT_1;
  int                       nReturnStatus = SUCCESS;
  mmgsdi_return_enum_type   mmgsdi_status = MMGSDI_ERROR;

  if (NULL == pMe) {
    return EBADPARM;
  }

  MSG_HIGH("OEMCardSession_Close, Channel Status = %d",
           pMe->m_byChannelStatus,0,0);

  if ((pMe->m_byChannelStatus != OEMCARDSESSION_OPENED) &&
     (pMe->m_byChannelStatus != OEMCARDSESSION_OPENING) &&
     (pMe->m_byChannelStatus != OEMCARDSESSION_SELECTED)) {
    MSG_ERROR("No Session open for this object",0,0,0);
    return EFAILED;
  }

  nReturnStatus = OEMCardSessionInitParams(pMe,
                                           pMe->m_u8Slot,
                                           (void*)pCmdStatus,
                                           pUserCB,
                                           &mmgsdi_slot,
                                           &pClientData);
  if (SUCCESS != nReturnStatus) {
    return nReturnStatus;
  }

  /* initialize to success */
  pCmdStatus->nCmdStatus = SUCCESS;

  mmgsdi_status =  mmgsdi_session_management_close (
                     pMe->m_client_id,
                     mmgsdi_slot,
                     OEMCardSessionCmdCb,
                     (mmgsdi_client_data_type)pClientData);
  if (MMGSDI_SUCCESS != mmgsdi_status) {
    OEMCardSession_CleanUpAsynObject(pClientData->ObjHandler);
    FREE(pClientData );
    pClientData = NULL;
    MSG_ERROR("Close Session Failed status = 0x%x",mmgsdi_status,0,0);
    return EFAILED;
  }

  return SUCCESS;
#else
  return EUNSUPPORTED;
#endif /* FEATURE_MMGSDI */
}
/*lint -save +esym(818,pCmdStatus,pUserCB) suppress could be const */


/*===========================================================================

FUNCTION: OEMCardSession_Authenticate

DESCRIPTION
  This function makes an async call to authenticate ISIM.

DEPENDENCIES
  none

RETURN VALUE
  EBADPARM        - Incorrect input parameters
  EFAILED         - Authentication Queue Failure
  SUCCESS         - Authentication Queue Success
  EUNSUPPORTED    - Authentication Not Supported

SIDE EFFECTS
===========================================================================*/
/*lint -e{715} suppress "pMe, pAuthData, pAuthStatus, pUserCB not referenced" */
/*lint -save -esym(818,pAuthStatus,pUserCB) suppress could be const */
int OEMCardSession_Authenticate(
  ICardSession             *pMe,
  const AEECardSessionData *pAuthData,
  AEECardSessionAuthStatus *pAuthStatus,
  AEECallback              *pUserCB)
{
#ifdef FEATURE_MMGSDI
  mmgsdi_return_enum_type  mmgsdi_status= MMGSDI_ERROR;
  mmgsdi_slot_id_enum_type mmgsdi_slot;
  int                      nReturnStatus;
  OEMCardSessionDataType   *pClientData;
  mmgsdi_data_type         mmgsdi_data;


  if ((NULL == pAuthStatus)              ||
      (NULL == pAuthStatus->sAuts.pData) ||
      (NULL == pAuthStatus->sIk.pData)   ||
      (NULL == pAuthStatus->sCk.pData)   ||
      (NULL == pAuthStatus->sRes.pData)  ||
      (NULL == pAuthData)) {
    return EBADPARM;
  }


  nReturnStatus = OEMCardSessionInitParams(pMe,
                                           pMe->m_u8Slot,
                                           (void*)pAuthStatus,
                                           pUserCB,
                                           &mmgsdi_slot,
                                           &pClientData);
  if (SUCCESS != nReturnStatus) {
    return nReturnStatus;
  }
  MSG_HIGH("OEMCardSession_Authenticate",0,0,0);


  pAuthStatus->nCmdStatus = SUCCESS;

  mmgsdi_data.data_len = pAuthData->nLen;
  mmgsdi_data.data_ptr = pAuthData->pData;

  mmgsdi_status = mmgsdi_isim_authenticate (
                    pMe->m_client_id,
                    mmgsdi_slot,
                    mmgsdi_data,
                    OEMCardSessionCmdCb,
                    (mmgsdi_client_data_type)pClientData);
  if (MMGSDI_SUCCESS != mmgsdi_status) {
    OEMCardSession_CleanUpAsynObject(pClientData->ObjHandler);
    FREE(pClientData );
    pClientData = NULL;
    MSG_ERROR("Auth Failed status = 0x%x",mmgsdi_status,0,0);
    return EFAILED;
  }

  return SUCCESS;
#else
  return EUNSUPPORTED;
#endif /* FEATURE_MMGSDI */
}
/*lint -save +esym(818,pAuthStatus,pUserCB) suppress could be const */


/*===========================================================================

FUNCTION: OEMCardSession_ReadTransparent

DESCRIPTION
  This function makes an async call to read the binary/transparent Files.

DEPENDENCIES
  none

RETURN VALUE
  none

SIDE EFFECTS
===========================================================================*/
/*lint -e{715} suppress "pMe, dwFileName, dwOffset, dwReqLen, pReadStatus, pUserCB not referenced" */
/*lint -save -esym(818,pReadStatus,pUserCB) suppress could be const */
int OEMCardSession_ReadTransparent(
  ICardSession                *pMe,
  uint32                      dwFileName,
  uint32                      dwOffset,
  uint32                      dwReqLen,
  AEECardSessionReadTpStatus *pReadStatus,
  AEECallback                *pUserCB)
{
#ifdef FEATURE_MMGSDI
  mmgsdi_return_enum_type   mmgsdi_status = MMGSDI_ERROR;
  mmgsdi_slot_id_enum_type  mmgsdi_slot;
  mmgsdi_file_enum_type     mmgsdi_file;
  OEMCardSessionDataType    *pClientData;
  int                       nReturnStatus;


  nReturnStatus = OEMCardSessionInitParams(pMe,
                                           pMe->m_u8Slot,
                                           (void*)pReadStatus,
                                           pUserCB,
                                           &mmgsdi_slot,
                                           &pClientData);
  if (SUCCESS != nReturnStatus) {
    return nReturnStatus;
  }

  nReturnStatus = OEMCardSession_MapFileEnum(&mmgsdi_file,dwFileName);
  if (SUCCESS != nReturnStatus) {
    return nReturnStatus;
  }

  pReadStatus->nCmdStatus = SUCCESS;

  MSG_HIGH("OEMCardSession_ReadTransparent file=0x%x",mmgsdi_file,0,0);

  mmgsdi_status = mmgsdi_read_transparent (
                     pMe->m_client_id,
                     mmgsdi_slot,
                     mmgsdi_file,
                     uint32toint32(dwOffset),
                     uint32toint32(dwReqLen),
                     OEMCardSessionCmdCb,
                     (mmgsdi_client_data_type)pClientData);
  if (MMGSDI_SUCCESS != mmgsdi_status) {
    OEMCardSession_CleanUpAsynObject(pClientData->ObjHandler);
    FREE(pClientData );
    pClientData = NULL;
    MSG_ERROR("Read Transparent Failed status = 0x%x",mmgsdi_status,0,0);
    return EFAILED;
  }

  return SUCCESS;
#else
  return EUNSUPPORTED;
#endif /* FEATURE_MMGSDI */
}
/*lint -save +esym(818,pReadStatus,pUserCB) suppress could be const */


/*===========================================================================

FUNCTION: OEMCardSession_ReadRecord

DESCRIPTION
  This function makes an async call to MMGSDI to read aa record based file

DEPENDENCIES
  none

RETURN VALUE
  none

SIDE EFFECTS
===========================================================================*/
/*lint -e{715} suppress "pMe, dwFileName, dwRecNum, dwReqLen, pReadStatus, pUserCB not referenced" */
/*lint -save -esym(818,pCmdStatus,pUserCB) suppress could be const */
int OEMCardSession_ReadRecord(
  ICardSession                *pMe,
  uint32                       dwFileName,
  uint32                       dwRecNum,
  uint32                       dwReqLen,
  AEECardSessionReadRecStatus *pCmdStatus,
  AEECallback                 *pUserCB)
{
#ifdef FEATURE_MMGSDI
  mmgsdi_return_enum_type   mmgsdi_status = MMGSDI_ERROR;
  mmgsdi_slot_id_enum_type  mmgsdi_slot;
  mmgsdi_file_enum_type     mmgsdi_file;
  OEMCardSessionDataType    *pClientData;
  int                       nReturnStatus;


  nReturnStatus = OEMCardSessionInitParams(pMe,
                                           pMe->m_u8Slot,
                                           (void*)pCmdStatus,
                                           pUserCB,
                                           &mmgsdi_slot,
                                           &pClientData);
  if (SUCCESS != nReturnStatus) {
    return nReturnStatus;
  }

  nReturnStatus = OEMCardSession_MapFileEnum(&mmgsdi_file, dwFileName);
  if (SUCCESS != nReturnStatus) {
    return nReturnStatus;
  }

  pCmdStatus->nCmdStatus = SUCCESS;

  MSG_HIGH("OEMCardSession_ReadRecord file=0x%x",mmgsdi_file,0,0);

  mmgsdi_status =  mmgsdi_read_record (pMe->m_client_id,
                                       mmgsdi_slot,
                                       mmgsdi_file,
                                       uint32toint32(dwRecNum),
                                       uint32toint32(dwReqLen),
                                       OEMCardSessionCmdCb,
                                       (mmgsdi_client_data_type)pClientData);
  if (MMGSDI_SUCCESS != mmgsdi_status) {
    OEMCardSession_CleanUpAsynObject(pClientData->ObjHandler);
    FREE(pClientData );
    pClientData = NULL;
    MSG_ERROR("Read Record Failed status = 0x%x",mmgsdi_status,0,0);
    return EFAILED;
  }

  return SUCCESS;
#else
  return EUNSUPPORTED;
#endif /* FEATURE_MMGSDI */
}
/*lint -save +esym(818,pCmdStatus,pUserCB) suppress could be const */


/*===========================================================================

FUNCTION: OEMCardSession_WriteTransparent

DESCRIPTION
  This function makes an async call to MMGSDI to write to a transparent/
  binary file

DEPENDENCIES
  none

RETURN VALUE
  none

SIDE EFFECTS
===========================================================================*/
/*lint -e{715} suppress "pMe, dwFileName, dwOffset, pWriteData, pWriteStatus, pUserCB not referenced" */
/*lint -save -esym(818,pWriteStatus,pUserCB) suppress could be const */
int OEMCardSession_WriteTransparent(
  ICardSession                *pMe,
  uint32                       dwFileName,
  uint32                       dwOffset,
  const AEECardSessionData    *pWriteData,
  AEECardSessionWriteTpStatus *pWriteStatus,
  AEECallback                 *pUserCB)
{
#ifdef FEATURE_MMGSDI
  mmgsdi_return_enum_type   mmgsdi_status = MMGSDI_ERROR;
  mmgsdi_slot_id_enum_type  mmgsdi_slot;
  mmgsdi_file_enum_type     mmgsdi_file;
  OEMCardSessionDataType    *pClientData;
  mmgsdi_data_type          mmgsdi_data;
  int                       nReturnStatus;

  if (NULL == pWriteData)
    return EBADPARM;

  nReturnStatus = OEMCardSessionInitParams(pMe,
                                           pMe->m_u8Slot,
                                           (void*)pWriteStatus,
                                           pUserCB,
                                           &mmgsdi_slot,
                                           &pClientData);
  if (SUCCESS != nReturnStatus) {
    return nReturnStatus;
  }

  nReturnStatus = OEMCardSession_MapFileEnum(&mmgsdi_file,dwFileName);
  if (SUCCESS != nReturnStatus) {
    return nReturnStatus;
  }

  pWriteStatus->nCmdStatus = SUCCESS;

  MSG_HIGH("OEMCardSession_WriteTransparent file=0x%x",mmgsdi_file,0,0);

  mmgsdi_data.data_len = pWriteData->nLen;
  mmgsdi_data.data_ptr = pWriteData->pData;

  mmgsdi_status =  mmgsdi_write_transparent (
                     pMe->m_client_id,
                     mmgsdi_slot,
                     mmgsdi_file,
                     uint32toint32(dwOffset),
                     mmgsdi_data,
                     OEMCardSessionCmdCb,
                     (mmgsdi_client_data_type)pClientData);
  if (MMGSDI_SUCCESS != mmgsdi_status) {
    OEMCardSession_CleanUpAsynObject(pClientData->ObjHandler);
    FREE(pClientData );
    pClientData = NULL;
    MSG_ERROR("OEMCardSession_WriteTransparent Failed status = 0x%x",mmgsdi_status,0,0);
    return EFAILED;
  }

  return SUCCESS;
#else
  return EUNSUPPORTED;
#endif /* FEATURE_MMGSDI */
}
/*lint -save +esym(818,pWriteStatus,pUserCB) suppress could be const */


/*===========================================================================

FUNCTION: OEMCardSession_WriteRecord

DESCRIPTION
  This function makes an async call to MMGSDI to write to a record based file.
DEPENDENCIES
  none

RETURN VALUE
  none

SIDE EFFECTS
===========================================================================*/
/*lint -e{715} suppress "pMe, dwFileName, dwRecNum, pWriteData, pWriteStatus, pUserCB not referenced" */
/*lint -save -esym(818,pCmdStatus,pUserCB) suppress could be const */
int OEMCardSession_WriteRecord(
  ICardSession                 *pMe,
  uint32                        dwFileName,
  uint32                        dwRecNum,
  const AEECardSessionData     *pWriteData,
  AEECardSessionWriteRecStatus *pCmdStatus,
  AEECallback                  *pUserCB)
{
#ifdef FEATURE_MMGSDI
  mmgsdi_return_enum_type   mmgsdi_status = MMGSDI_ERROR;
  mmgsdi_slot_id_enum_type  mmgsdi_slot;
  mmgsdi_file_enum_type     mmgsdi_file;
  OEMCardSessionDataType    *pClientData;
  mmgsdi_data_type          mmgsdi_data;
  int                       nReturnStatus;

  if (NULL == pWriteData)
    return EBADPARM;

  nReturnStatus = OEMCardSessionInitParams(pMe,
                                           pMe->m_u8Slot,
                                           (void*)pCmdStatus,
                                           pUserCB,
                                           &mmgsdi_slot,
                                           &pClientData);
  if (SUCCESS != nReturnStatus) {
    return nReturnStatus;
  }

  nReturnStatus = OEMCardSession_MapFileEnum(&mmgsdi_file,dwFileName);
  if (SUCCESS != nReturnStatus) {
    return nReturnStatus;
  }

  pCmdStatus->nCmdStatus = SUCCESS;

  MSG_HIGH("OEMCardSession_WriteRecord file=0x%x",mmgsdi_file,0,0);

  mmgsdi_data.data_len = pWriteData->nLen;
  mmgsdi_data.data_ptr = pWriteData->pData;

  mmgsdi_status =  mmgsdi_write_record (pMe->m_client_id,
                                        mmgsdi_slot,
                                        mmgsdi_file,
                                        uint32toint32(dwRecNum),
                                        mmgsdi_data,
                                        OEMCardSessionCmdCb,
                                        (mmgsdi_client_data_type)pClientData);
  if (MMGSDI_SUCCESS != mmgsdi_status) {
    OEMCardSession_CleanUpAsynObject(pClientData->ObjHandler);
    FREE(pClientData );
    pClientData = NULL;
    MSG_ERROR("OEMCardSession_WriteRecord Failed status = 0x%x",
              mmgsdi_status,0,0);
    return EFAILED;
  }

  return SUCCESS;
#else
  return EUNSUPPORTED;
#endif /* FEATURE_MMGSDI */
}
/*lint -save +esym(818,pCmdStatus,pUserCB) suppress could be const */


/*===========================================================================

FUNCTION: OEMCardSession_GetFileAttr

DESCRIPTION
     This method provides the service to get the Attribute information on a
     file or directory on the SIM/USIM

RETURN VALUE
  SUCCESS - Status information is valid
  EBADPARM -  Bad parameter
  EUNSUPPORTED - Not supported

SIDE EFFECTS
  NONE
===========================================================================*/
/*lint -e{715} suppress "pMe, sFilePath, pAttribStatus, pUserCB not referenced" */
/*lint -save -esym(818,pAttribStatus,pUserCB) suppress could be const */
int OEMCardSession_GetFileAttr(
  ICardSession                  *pMe,
  AEECardSessionFilePathType     sFilePath,
  AEECardSessionFileAttribStatus*pAttribStatus,
  AEECallback                   *pUserCB)
{
#ifdef FEATURE_MMGSDI
  mmgsdi_return_enum_type   mmgsdi_status = MMGSDI_ERROR;
  mmgsdi_slot_id_enum_type  mmgsdi_slot;
  OEMCardSessionDataType    *pClientData;
  int                       nReturnStatus;
  mmgsdi_access_type        mmgsdi_access;

  if (NULL == pAttribStatus)
    return EBADPARM;

  nReturnStatus = OEMCardSessionInitParams(pMe,
                                           pMe->m_u8Slot,
                                           (void*)pAttribStatus,
                                           pUserCB,
                                           &mmgsdi_slot,
                                           &pClientData);
  if (SUCCESS != nReturnStatus) {
    return nReturnStatus;
  }

  if(sFilePath.dwFileIndication == AEECARDSESSION_EF_FILE_NAME)
  {
    mmgsdi_access.access_method  = MMGSDI_EF_ENUM_ACCESS;
    nReturnStatus = OEMCardSession_MapFileEnum(&mmgsdi_access.file.file_enum,
                                               sFilePath.dwFileName);
    if (SUCCESS != nReturnStatus) {
      return nReturnStatus;
    }
  }
  else
    return EUNSUPPORTED;

  pAttribStatus->nCmdStatus = SUCCESS;

  MSG_HIGH("OEMCardSession_GetFileAttr file=0x%x",
           mmgsdi_access.file.file_enum,0,0);

  mmgsdi_status = mmgsdi_get_file_attr(pMe->m_client_id,
                                       mmgsdi_slot,
                                       mmgsdi_access,
                                       OEMCardSessionCmdCb,
                                       (mmgsdi_client_data_type)pClientData);
  if (MMGSDI_SUCCESS != mmgsdi_status) {
    OEMCardSession_CleanUpAsynObject(pClientData->ObjHandler);
    FREE(pClientData );
    pClientData = NULL;
    MSG_ERROR("OEMCardSession_WriteRecord Failed status = 0x%x",
              mmgsdi_status,0,0);
    return EFAILED;
  }

  return SUCCESS;
#else
  return EUNSUPPORTED;
#endif /* FEATURE_MMGSDI */
}
/*lint -save +esym(818,pAttribStatus,pUserCB) suppress could be const */


/*===========================================================================

FUNCTION: OEMCardSession_GetCardAllPinsInfo

DESCRIPTION
  Get All the Card PIN information relating to the ICardSession object

RETURN VALUE
  SUCCESS - Status information is valid
  EBADPARM -  Bad parameter
  EUNSUPPORTED - Not supported

SIDE EFFECTS
  NONE
===========================================================================*/
/*lint -e{715} suppress "pMe, pCmdStatus, pUserCB not referenced" */
/*lint -save -esym(818,pCmdStatus,pUserCB) suppress could be const */
int OEMCardSession_GetCardAllPinsInfo(
  ICardSession                  *pMe,
  AEECardSessionCardAllPinsInfo *pCmdStatus,
  AEECallback                   *pUserCB)
{
#ifdef FEATURE_MMGSDI
  mmgsdi_return_enum_type   mmgsdi_status = MMGSDI_NOT_SUPPORTED;
  mmgsdi_slot_id_enum_type  mmgsdi_slot   = MMGSDI_MAX_SLOT_ID_ENUM;
  OEMCardSessionDataType   *pClientData   = NULL;
  int                       nReturnStatus = SUCCESS;

  MSG_HIGH("OEMCardSession_GetCardAllPinsInfo",0,0,0);

  if (NULL == pCmdStatus) {
    return EBADPARM;
  }

  nReturnStatus = OEMCardSessionInitParams(pMe,
                                           pMe->m_u8Slot,
                                           (void*)pCmdStatus,
                                           pUserCB,
                                           &mmgsdi_slot,
                                           &pClientData);
  if (SUCCESS != nReturnStatus) {
    return nReturnStatus;
  }

  pCmdStatus->nCmdStatus = SUCCESS;

  mmgsdi_status = mmgsdi_get_all_pin_status (
                     pMe->m_client_id,
                     mmgsdi_slot,
                     OEMCardSessionCmdCb,
                     (mmgsdi_client_data_type)pClientData);
  if (MMGSDI_SUCCESS != mmgsdi_status) {
    OEMCardSession_CleanUpAsynObject(pClientData->ObjHandler);
    FREE(pClientData );
    pClientData = NULL;
    MSG_ERROR("OEMCardSession_GetCardAllPinsInfo Failed status = 0x%x",
      mmgsdi_status,0,0);
  }

  return OEMCardSession_MapFromMMGSDIStatus(mmgsdi_status);
#else
  return EUNSUPPORTED;
#endif /* FEATURE_MMGSDI */
}
/*lint -save +esym(818,pCmdStatus,pUserCB) suppress could be const */


/*===========================================================================

FUNCTION: OEMCardSession_VerifyPin

DESCRIPTION
  Verify the PIN for the particular application accessed by the
  ICardSession object.

RETURN VALUE
  SUCCESS - Status information is valid
  EBADPARM -  Bad parameter
  EUNSUPPORTED - Not supported

SIDE EFFECTS
  NONE
===========================================================================*/
/*lint -e{715} suppress "pMe, dwPinID, pPin, pCmdStatus, pUserCB not referenced" */
/*lint -save -esym(818,pCmdStatus,pUserCB) suppress could be const */
int OEMCardSession_VerifyPin(
  ICardSession                  *pMe,
  uint32                         dwPinID,
  const AEECardSessionData      *pPin,
  AEECardSessionPinOpStatus     *pCmdStatus,
  AEECallback                   *pUserCB)
{
#ifdef FEATURE_MMGSDI
  mmgsdi_return_enum_type   mmgsdi_status = MMGSDI_NOT_SUPPORTED;
  mmgsdi_slot_id_enum_type  mmgsdi_slot   = MMGSDI_MAX_SLOT_ID_ENUM;
  OEMCardSessionDataType    *pClientData  = NULL;
  int                       nReturnStatus = SUCCESS;
  mmgsdi_pin_enum_type      mmgsdi_pin_id = MMGSDI_MAX_PIN_ENUM;
  mmgsdi_data_type          pin_data;

  MSG_HIGH("OEMCardSession_VerifyPin ID 0x%x",dwPinID,0,0);

  if ((NULL == pCmdStatus) || (NULL == pPin)) {
    return EBADPARM;
  }

  if (NULL == pPin->pData) {
    return EBADPARM;
  }

  nReturnStatus = OEMCardSessionInitParams(pMe,
                                           pMe->m_u8Slot,
                                           (void*)pCmdStatus,
                                           pUserCB,
                                           &mmgsdi_slot,
                                           &pClientData);
  if (SUCCESS != nReturnStatus) {
    return nReturnStatus;
  }

  nReturnStatus = OEMCardSession_MapPinID(&mmgsdi_pin_id, dwPinID);
  if (SUCCESS != nReturnStatus) {
    return nReturnStatus;
  }

  pCmdStatus->nCmdStatus = SUCCESS;

  pin_data.data_len = pPin->nLen;
  pin_data.data_ptr = pPin->pData;

  mmgsdi_status =  mmgsdi_verify_pin (pMe->m_client_id,
                                      mmgsdi_slot,
                                      mmgsdi_pin_id,
                                      pin_data,
                                      OEMCardSessionCmdCb,
                                      (mmgsdi_client_data_type)pClientData);
  if (MMGSDI_SUCCESS != mmgsdi_status) {
    OEMCardSession_CleanUpAsynObject(pClientData->ObjHandler);
    FREE(pClientData );
    pClientData = NULL;
    MSG_ERROR("OEMCardSession_VerifyPin Failed status = 0x%x",
              mmgsdi_status,0,0);
  }

  return OEMCardSession_MapFromMMGSDIStatus(mmgsdi_status);
#else
  return EUNSUPPORTED;
#endif /* FEATURE_MMGSDI */
}
/*lint -save +esym(818,pCmdStatus,pUserCB) suppress could be const */


/*===========================================================================

FUNCTION: OEMCardSession_EnablePin

DESCRIPTION
  Enable the PIN for the particular application accessed by the
  ICardSession object.

RETURN VALUE
  SUCCESS - Status information is valid
  EBADPARM -  Bad parameter
  EUNSUPPORTED - Not supported

SIDE EFFECTS
  NONE
===========================================================================*/
/*lint -e{715} suppress "pMe, dwPinID, pPin, pCmdStatus, pUserCB not referenced" */
/*lint -save -esym(818,pCmdStatus,pUserCB) suppress could be const */
int OEMCardSession_EnablePin(
  ICardSession                  *pMe,
  uint32                         dwPinID,
  const AEECardSessionData      *pPin,
  AEECardSessionPinOpStatus     *pCmdStatus,
  AEECallback                   *pUserCB)
{
#ifdef FEATURE_MMGSDI
  mmgsdi_return_enum_type   mmgsdi_status = MMGSDI_NOT_SUPPORTED;
  mmgsdi_slot_id_enum_type  mmgsdi_slot   = MMGSDI_MAX_SLOT_ID_ENUM;
  OEMCardSessionDataType    *pClientData  = NULL;
  int                       nReturnStatus = SUCCESS;
  mmgsdi_pin_enum_type      mmgsdi_pin_id = MMGSDI_MAX_PIN_ENUM;
  mmgsdi_data_type          pin_data;

  MSG_HIGH("OEMCardSession_EnablePin ID 0x%x",dwPinID,0,0);

  if ((NULL == pCmdStatus) || (NULL == pPin)) {
    return EBADPARM;
  }

  if (NULL == pPin->pData) {
    return EBADPARM;
  }

  nReturnStatus = OEMCardSessionInitParams(pMe,
                                           pMe->m_u8Slot,
                                           (void*)pCmdStatus,
                                           pUserCB,
                                           &mmgsdi_slot,
                                           &pClientData);
  if (SUCCESS != nReturnStatus) {
    return nReturnStatus;
  }

  nReturnStatus = OEMCardSession_MapPinID(&mmgsdi_pin_id, dwPinID);
  if (SUCCESS != nReturnStatus) {
    return nReturnStatus;
  }

  pCmdStatus->nCmdStatus = SUCCESS;

  pin_data.data_len = pPin->nLen;
  pin_data.data_ptr = pPin->pData;

  mmgsdi_status =  mmgsdi_enable_pin (pMe->m_client_id,
                                      mmgsdi_slot,
                                      mmgsdi_pin_id,
                                      pin_data,
                                      OEMCardSessionCmdCb,
                                      (mmgsdi_client_data_type)pClientData);
  if (MMGSDI_SUCCESS != mmgsdi_status) {
    OEMCardSession_CleanUpAsynObject(pClientData->ObjHandler);
    FREE(pClientData );
    pClientData = NULL;
    MSG_ERROR("OEMCardSession_EnablePin Failed status = 0x%x",
              mmgsdi_status,0,0);
  }

  return OEMCardSession_MapFromMMGSDIStatus(mmgsdi_status);
#else
  return EUNSUPPORTED;
#endif /* FEATURE_MMGSDI */
}
/*lint -save +esym(818,pCmdStatus,pUserCB) suppress could be const */


/*===========================================================================

FUNCTION: OEMCardSession_DisablePinExt

DESCRIPTION
  Disable the PIN for the particular application accessed by the
  ICardSession object and provides the user with replacement option

RETURN VALUE
  SUCCESS - Status information is valid
  EBADPARM -  Bad parameter
  EUNSUPPORTED - Not supported

SIDE EFFECTS
  NONE
===========================================================================*/
/*lint -e{715} suppress "pMe, dwPinID, pPin, pCmdStatus, pUserCB not referenced" */
/*lint -save -esym(818,pCmdStatus,pUserCB) suppress could be const */
int OEMCardSession_DisablePinExt(
  ICardSession                  *pMe,
  uint32                         dwPinID,
  const AEECardSessionData      *pPin,
  uint32                         dwReplace,
  AEECardSessionPinOpStatus     *pCmdStatus,
  AEECallback                   *pUserCB)
{
#ifdef FEATURE_MMGSDI
  mmgsdi_return_enum_type      mmgsdi_status = MMGSDI_NOT_SUPPORTED;
  mmgsdi_slot_id_enum_type     mmgsdi_slot   = MMGSDI_MAX_SLOT_ID_ENUM;
  OEMCardSessionDataType       *pClientData  = NULL;
  int                          nReturnStatus = SUCCESS;
  mmgsdi_pin_enum_type         mmgsdi_pin_id = MMGSDI_MAX_PIN_ENUM;
  mmgsdi_data_type             pin_data;
  mmgsdi_pin_replace_enum_type mmgsdi_replace_option = MMGSDI_PIN_NOT_REPLACED_BY_UNIVERSAL;


  MSG_HIGH("OEMCardSession_DisablePinExt ID 0x%x",dwPinID,0,0);

  if ((NULL == pCmdStatus) || (NULL == pPin)) {
    return EBADPARM;
  }

  if (NULL == pPin->pData) {
    return EBADPARM;
  }

  nReturnStatus = OEMCardSession_MapPinID(&mmgsdi_pin_id, dwPinID);
  if (SUCCESS != nReturnStatus) {
    MSG_ERROR("Could not map to MMGSDI PIN",0,0,0);
    return nReturnStatus;
  }

  nReturnStatus = OEMCardSession_MapReplaceOption(&mmgsdi_replace_option, dwReplace);
  if (SUCCESS != nReturnStatus) {
    MSG_ERROR("Could not map to MMGSDI Replace Option",0,0,0);
    return nReturnStatus;
  }

  nReturnStatus = OEMCardSessionInitParams(pMe,
                                           pMe->m_u8Slot,
                                           (void*)pCmdStatus,
                                           pUserCB,
                                           &mmgsdi_slot,
                                           &pClientData);
  if (SUCCESS != nReturnStatus) {
    return nReturnStatus;
  }

  pCmdStatus->nCmdStatus = SUCCESS;

  pin_data.data_len = pPin->nLen;
  pin_data.data_ptr = pPin->pData;

  mmgsdi_status =  mmgsdi_disable_pin_ext(pMe->m_client_id,
                                          mmgsdi_slot,
                                          mmgsdi_pin_id,
                                          mmgsdi_replace_option,
                                          pin_data,
                                          OEMCardSessionCmdCb,
                                          (mmgsdi_client_data_type)pClientData);
  if (MMGSDI_SUCCESS != mmgsdi_status) {
    OEMCardSession_CleanUpAsynObject(pClientData->ObjHandler);
    FREE(pClientData );
    pClientData = NULL;
    MSG_ERROR("OEMCardSession_DisablePin Failed status = 0x%x",
              mmgsdi_status,0,0);
  }

  return OEMCardSession_MapFromMMGSDIStatus(mmgsdi_status);
#else
  return EUNSUPPORTED;
#endif /* FEATURE_MMGSDI */
}
/*lint -save +esym(818,pCmdStatus,pUserCB) suppress could be const */


/*===========================================================================

FUNCTION: OEMCardSession_DisablePin

DESCRIPTION
  Disable the PIN for the particular application accessed by the
  ICardSession object.

RETURN VALUE
  SUCCESS - Status information is valid
  EBADPARM -  Bad parameter
  EUNSUPPORTED - Not supported

SIDE EFFECTS
  NONE
===========================================================================*/
/*lint -e{715} suppress "pMe, dwPinID, pPin, pCmdStatus, pUserCB not referenced" */
/*lint -save -esym(818,pCmdStatus,pUserCB) suppress could be const */
int OEMCardSession_DisablePin(
  ICardSession                  *pMe,
  uint32                         dwPinID,
  const AEECardSessionData      *pPin,
  AEECardSessionPinOpStatus     *pCmdStatus,
  AEECallback                   *pUserCB)
{
  return OEMCardSession_DisablePinExt(pMe,
                                      dwPinID, 
                                      pPin, 
                                      AEECARDSESSION_PIN_NOT_REPLACED_BY_UNIV,
                                      pCmdStatus,
                                      pUserCB);
  
}
/*lint -save +esym(818,pCmdStatus,pUserCB) suppress could be const */

/*===========================================================================

FUNCTION: OEMCardSession_UnblockPin

DESCRIPTION
  Unblock the PIN for the particular application accessed by the
  ICardSession object.
  The New Pin value will also need to be provided in this operation.

RETURN VALUE
  SUCCESS - Status information is valid
  EBADPARM -  Bad parameter
  EUNSUPPORTED - Not supported

SIDE EFFECTS
  NONE
===========================================================================*/
/*lint -e{715} suppress "pMe, dwPinID, pPuk, pNewPin, pCmdStatus, pUserCB not referenced" */
/*lint -save -esym(818,pCmdStatus,pUserCB) suppress could be const */
int OEMCardSession_UnblockPin(
  ICardSession                  *pMe,
  uint32                         dwPinID,
  const AEECardSessionData      *pPuk,
  const AEECardSessionData      *pNewPin,
  AEECardSessionPinOpStatus     *pCmdStatus,
  AEECallback                   *pUserCB)
{
#ifdef FEATURE_MMGSDI
  mmgsdi_return_enum_type   mmgsdi_status = MMGSDI_NOT_SUPPORTED;
  mmgsdi_slot_id_enum_type  mmgsdi_slot   = MMGSDI_MAX_SLOT_ID_ENUM;
  OEMCardSessionDataType    *pClientData  = NULL;
  int                       nReturnStatus = SUCCESS;
  mmgsdi_pin_enum_type      mmgsdi_pin_id = MMGSDI_MAX_PIN_ENUM;
  mmgsdi_data_type          new_pin_data;
  mmgsdi_data_type          puk_data;

  MSG_HIGH("OEMCardSession_UnblockPin ID 0x%x",dwPinID,0,0);

  if ((NULL == pCmdStatus) || (NULL == pPuk) || (NULL == pNewPin)) {
    return EBADPARM;
  }

  if ((NULL == pPuk->pData) || (NULL == pNewPin->pData)) {
    return EBADPARM;
  }

  nReturnStatus = OEMCardSessionInitParams(pMe,
                                           pMe->m_u8Slot,
                                           (void*)pCmdStatus,
                                           pUserCB,
                                           &mmgsdi_slot,
                                           &pClientData);
  if (SUCCESS != nReturnStatus) {
    return nReturnStatus;
  }

  nReturnStatus = OEMCardSession_MapPinID(&mmgsdi_pin_id, dwPinID);
  if (SUCCESS != nReturnStatus) {
    return nReturnStatus;
  }

  pCmdStatus->nCmdStatus = SUCCESS;

  new_pin_data.data_len = pNewPin->nLen;
  new_pin_data.data_ptr = pNewPin->pData;
  puk_data.data_len = pPuk->nLen;
  puk_data.data_ptr = pPuk->pData;

  mmgsdi_status =  mmgsdi_unblock_pin (pMe->m_client_id,
                                       mmgsdi_slot,
                                       mmgsdi_pin_id,
                                       puk_data,
                                       new_pin_data,
                                       OEMCardSessionCmdCb,
                                       (mmgsdi_client_data_type)pClientData);
  if (MMGSDI_SUCCESS != mmgsdi_status) {
    OEMCardSession_CleanUpAsynObject(pClientData->ObjHandler);
    FREE(pClientData );
    pClientData = NULL;
    MSG_ERROR("OEMCardSession_UnblockPin Failed status = 0x%x",
              mmgsdi_status,0,0);
  }

  return OEMCardSession_MapFromMMGSDIStatus(mmgsdi_status);
#else
  return EUNSUPPORTED;
#endif /* FEATURE_MMGSDI */
}
/*lint -save +esym(818,pCmdStatus,pUserCB) suppress could be const */


/*===========================================================================

FUNCTION: OEMCardSession_ChangePin

DESCRIPTION
  Change the PIN for the particular application accessed by the ICardSession object.
  The New Pin value will also need to be provided in this operation.

RETURN VALUE
  SUCCESS - Status information is valid
  EBADPARM -  Bad parameter
  EUNSUPPORTED - Not supported

SIDE EFFECTS
  NONE
===========================================================================*/
/*lint -e{715} suppress "pMe, dwPinID, pOldPin, pNewPin, pCmdStatus, pUserCB not referenced" */
/*lint -save -esym(818,pCmdStatus,pUserCB) suppress could be const */
int OEMCardSession_ChangePin(
  ICardSession                  *pMe,
  uint32                         dwPinID,
  const AEECardSessionData      *pOldPin,
  const AEECardSessionData      *pNewPin,
  AEECardSessionPinOpStatus     *pCmdStatus,
  AEECallback                   *pUserCB)
{
#ifdef FEATURE_MMGSDI
  mmgsdi_return_enum_type   mmgsdi_status = MMGSDI_NOT_SUPPORTED;
  mmgsdi_slot_id_enum_type  mmgsdi_slot   = MMGSDI_MAX_SLOT_ID_ENUM;
  OEMCardSessionDataType    *pClientData  = NULL;
  int                       nReturnStatus = SUCCESS;
  mmgsdi_pin_enum_type      mmgsdi_pin_id = MMGSDI_MAX_PIN_ENUM;
  mmgsdi_data_type          new_pin_data;
  mmgsdi_data_type          old_pin_data;

  MSG_HIGH("OEMCardSession_ChangePin ID 0x%x",dwPinID,0,0);

  if ((NULL == pCmdStatus) || (NULL == pOldPin) || (NULL == pNewPin)) {
    return EBADPARM;
  }

  if ((NULL == pOldPin->pData) || (NULL == pNewPin->pData)) {
    return EBADPARM;
  }

  nReturnStatus = OEMCardSessionInitParams(pMe,
                                           pMe->m_u8Slot,
                                           (void*)pCmdStatus,
                                           pUserCB,
                                           &mmgsdi_slot,
                                           &pClientData);
  if (SUCCESS != nReturnStatus) {
    return nReturnStatus;
  }

  nReturnStatus = OEMCardSession_MapPinID(&mmgsdi_pin_id, dwPinID);
  if (SUCCESS != nReturnStatus) {
    return nReturnStatus;
  }

  pCmdStatus->nCmdStatus = SUCCESS;

  new_pin_data.data_len = pNewPin->nLen;
  new_pin_data.data_ptr = pNewPin->pData;
  old_pin_data.data_len = pOldPin->nLen;
  old_pin_data.data_ptr = pOldPin->pData;

  mmgsdi_status =  mmgsdi_change_pin (pMe->m_client_id,
                                      mmgsdi_slot,
                                      mmgsdi_pin_id,
                                      old_pin_data,
                                      new_pin_data,
                                      OEMCardSessionCmdCb,
                                      (mmgsdi_client_data_type)pClientData);
  if (MMGSDI_SUCCESS != mmgsdi_status) {
    OEMCardSession_CleanUpAsynObject(pClientData->ObjHandler);
    FREE(pClientData );
    pClientData = NULL;
    MSG_ERROR("OEMCardSession_ChangePin Failed status = 0x%x",
              mmgsdi_status,0,0);
  }

  return OEMCardSession_MapFromMMGSDIStatus(mmgsdi_status);
#else
  return EUNSUPPORTED;
#endif /* FEATURE_MMGSDI */
}
/*lint -save +esym(818,pCmdStatus,pUserCB) suppress could be const */


/*===========================================================================

FUNCTION: OEMCardSession_SelectSubscription

DESCRIPTION
  The function selects the subscription.  This function is applicable to all
  technologies.
  This function should only be called when a technology initialization is
  required or when a technology switch is required.
  i.e., UMTS App1 -> UMTS App2 is not applicable without a SIM INSERTED Card
        Event
  i.e., GSM -> CDMA is applicable

RETURN VALUE
  SUCCESS - Status information is valid
  EBADPARM -  Bad parameter
  EUNSUPPORTED - Not supported

SIDE EFFECTS
  NONE
===========================================================================*/
/*lint -e{715} suppress "pMe, pSub, pCmdStatus, pUserCB not referenced" */
/*lint -save -esym(818,pCmdStatus,pUserCB) suppress could be const */
int OEMCardSession_SelectSubscription(
  ICardSession                  *pMe,
  const AEECardSessionSubData   *pSub,
  AEECardSessionCmdStatus       *pCmdStatus,
  AEECallback                   *pUserCB)
{
#ifdef FEATURE_MMGSDI
  mmgsdi_return_enum_type   mmgsdi_status = MMGSDI_NOT_SUPPORTED;
  mmgsdi_slot_id_enum_type  mmgsdi_slot   = MMGSDI_MAX_SLOT_ID_ENUM;
  OEMCardSessionDataType    *pClientData  = NULL;
  int                       nReturnStatus = SUCCESS;
  mmgsdi_data_type          sAid;

  MSG_HIGH("ICARDSESSION_SelectSubscription ",0,0,0);

  if ((NULL == pCmdStatus) || (NULL == pSub)) {
    return EBADPARM;
  }

  if (AEECARDSESSION_SUB_UICC != pSub->dwTech) {
    return EUNSUPPORTED;
  }

  if (( NULL == pSub->pAid ) ||
      ( 0 == pSub->pAid->nAidLen )) {
    return EBADPARM;
  }

  nReturnStatus = OEMCardSessionInitParams(pMe,
                                           pMe->m_u8Slot,
                                           (void*)pCmdStatus,
                                           pUserCB,
                                           &mmgsdi_slot,
                                           &pClientData);
  if (SUCCESS != nReturnStatus) {
    return nReturnStatus;
  }

  pCmdStatus->nCmdStatus = SUCCESS;

  sAid.data_len = pSub->pAid->nAidLen;
  sAid.data_ptr = pSub->pAid->aAid;

  MSG_HIGH("AID Len 0x%x", sAid.data_len, 0, 0);

  mmgsdi_status = mmgsdi_select_aid (pMe->m_client_id,
                                      mmgsdi_slot,
                                      sAid,
                                      OEMCardSessionCmdCb,
                                      (mmgsdi_client_data_type)pClientData);
  if (MMGSDI_SUCCESS != mmgsdi_status) {
    OEMCardSession_CleanUpAsynObject(pClientData->ObjHandler);
    FREE(pClientData );
    pClientData = NULL;
    MSG_ERROR("ICARDSESSION_SelectSubscription Failed status = 0x%x",
              mmgsdi_status,0,0);
  }

  return OEMCardSession_MapFromMMGSDIStatus(mmgsdi_status);
#else
  return EUNSUPPORTED;
#endif /* FEATURE_MMGSDI */
}
/*lint -save +esym(818,pCmdStatus,pUserCB) suppress could be const */


/*===========================================================================

FUNCTION: OEMCardSession_ReadTransparentByPath

DESCRIPTION
  This function makes an async call to read the binary/transparent Files.

DEPENDENCIES
  none

RETURN VALUE
  none

SIDE EFFECTS
===========================================================================*/
/*lint -e{715} suppress "pMe, dwFileName, dwOffset, dwReqLen, pReadStatus, pUserCB not referenced" */
/*lint -save -esym(818,pReadStatus,pUserCB) suppress could be const */
int OEMCardSession_ReadTransparentByPath(
  ICardSession                *pMe,
  AEECardSessionFilePathType  sFilePath,
  uint32                      dwOffset,
  uint32                      dwReqLen,
  AEECardSessionReadTpStatus *pReadStatus,
  AEECallback                *pUserCB)
{
#ifdef FEATURE_MMGSDI
  mmgsdi_return_enum_type   mmgsdi_status = MMGSDI_ERROR;
  mmgsdi_slot_id_enum_type  mmgsdi_slot   = MMGSDI_SLOT_1;
  OEMCardSessionDataType    *pClientData  = NULL;
  int                       nReturnStatus = SUCCESS;
  mmgsdi_access_type        file_access;
  
  switch (sFilePath.dwFileIndication)
  {
    case AEECARDSESSION_FILE_PATH:
      nReturnStatus = OEMCardSession_MapFileAccess(sFilePath,
                                                  &file_access);
      if(nReturnStatus != SUCCESS){
        MSG_ERROR("Could not convert to MMGSDI File Path",0,0,0);
        return nReturnStatus;
      }
      break;

    case AEECARDSESSION_EF_FILE_NAME:
      return OEMCardSession_ReadTransparent(pMe,
                                            sFilePath.dwFileName,
                                            dwOffset,
                                            dwReqLen,
                                            pReadStatus,
                                            pUserCB);
    default:
      MSG_ERROR("Unknown File Indication 0x%x",sFilePath.dwFileIndication,0,0);
      return EBADPARM;
  }

  nReturnStatus = OEMCardSessionInitParams(pMe,
                                           pMe->m_u8Slot,
                                           (void*)pReadStatus,
                                           pUserCB,
                                           &mmgsdi_slot,
                                           &pClientData);
  if (SUCCESS != nReturnStatus) {
    MSG_ERROR("Input Parameters Incorrect, status 0x%x",nReturnStatus,0,0);
    return nReturnStatus;
  }


  pReadStatus->nCmdStatus = SUCCESS;

  MSG_HIGH("OEMCardSession_ReadTransparent",0,0,0);

  mmgsdi_status = mmgsdi_read_transparent_ext (
                     pMe->m_client_id,
                     mmgsdi_slot,
                     file_access,
                     uint32toint32(dwOffset),
                     uint32toint32(dwReqLen),
                     OEMCardSessionCmdCb,
                     (mmgsdi_client_data_type)pClientData);
  if (MMGSDI_SUCCESS != mmgsdi_status) {
    OEMCardSession_CleanUpAsynObject(pClientData->ObjHandler);
    FREE(pClientData );
    pClientData = NULL;
    MSG_ERROR("Read TransparentByPath Failed status = 0x%x",mmgsdi_status,0,0);
    return EFAILED;
  }

  return SUCCESS;
#else
  return EUNSUPPORTED;
#endif /* FEATURE_MMGSDI */
}
/*lint -save +esym(818,pReadStatus,pUserCB) suppress could be const */


/*===========================================================================

FUNCTION: OEMCardSession_ReadRecordByPath

DESCRIPTION
  This function makes an async call to MMGSDI to read a record based file

DEPENDENCIES
  none

RETURN VALUE
  none

SIDE EFFECTS
===========================================================================*/
/*lint -e{715} suppress "pMe, dwFileName, dwRecNum, dwReqLen, pReadStatus, pUserCB not referenced" */
/*lint -save -esym(818,pCmdStatus,pUserCB) suppress could be const */
int OEMCardSession_ReadRecordByPath(
  ICardSession                *pMe,
  AEECardSessionFilePathType   sFilePath,
  uint32                       dwRecNum,
  uint32                       dwReqLen,
  AEECardSessionReadRecStatus *pCmdStatus,
  AEECallback                 *pUserCB)
{
#ifdef FEATURE_MMGSDI
  mmgsdi_return_enum_type   mmgsdi_status = MMGSDI_ERROR;
  mmgsdi_slot_id_enum_type  mmgsdi_slot   = MMGSDI_SLOT_1;
  OEMCardSessionDataType    *pClientData  = NULL;
  int                       nReturnStatus = SUCCESS;
  mmgsdi_access_type        file_access;

  switch (sFilePath.dwFileIndication)
  {
    case AEECARDSESSION_FILE_PATH:
      nReturnStatus = OEMCardSession_MapFileAccess(sFilePath,
                                                  &file_access);
      if(nReturnStatus != SUCCESS){
        MSG_ERROR("Could not convert to MMGSDI File Path",0,0,0);
        return nReturnStatus;
      }
      break;

    case AEECARDSESSION_EF_FILE_NAME:
      return OEMCardSession_ReadRecord(pMe,
                                       sFilePath.dwFileName,
                                       dwRecNum,
                                       dwReqLen,
                                       pCmdStatus,
                                       pUserCB);
    default:
      MSG_ERROR("Unknown File Indication 0x%x",sFilePath.dwFileIndication,0,0);
      return EBADPARM;
  }

  nReturnStatus = OEMCardSessionInitParams(pMe,
                                           pMe->m_u8Slot,
                                           (void*)pCmdStatus,
                                           pUserCB,
                                           &mmgsdi_slot,
                                           &pClientData);
  if (SUCCESS != nReturnStatus) {
    MSG_ERROR("Input Parameters Incorrect, status 0x%x",nReturnStatus,0,0);
    return nReturnStatus;
  }

  pCmdStatus->nCmdStatus = SUCCESS;

  MSG_HIGH("OEMCardSession_ReadRecord",0,0,0);

  mmgsdi_status =  mmgsdi_read_record_ext(pMe->m_client_id,
                                       mmgsdi_slot,
                                       file_access,
                                       uint32toint32(dwRecNum),
                                       uint32toint32(dwReqLen),
                                       OEMCardSessionCmdCb,
                                       (mmgsdi_client_data_type)pClientData);
  if (MMGSDI_SUCCESS != mmgsdi_status) {
    OEMCardSession_CleanUpAsynObject(pClientData->ObjHandler);
    FREE(pClientData );
    pClientData = NULL;
    MSG_ERROR("Read RecordByPath Failed status = 0x%x",mmgsdi_status,0,0);
    return EFAILED;
  }

  return SUCCESS;
#else
  return EUNSUPPORTED;
#endif /* FEATURE_MMGSDI */
}
/*lint -save +esym(818,pCmdStatus,pUserCB) suppress could be const */


/*===========================================================================

FUNCTION: OEMCardSession_WriteTransparentByPath

DESCRIPTION
  This function makes an async call to MMGSDI to write to a transparent/
  binary file

DEPENDENCIES
  none

RETURN VALUE
  none

SIDE EFFECTS
===========================================================================*/
/*lint -e{715} suppress "pMe, dwFileName, dwOffset, pWriteData, pWriteStatus, pUserCB not referenced" */
/*lint -save -esym(818,pWriteStatus,pUserCB) suppress could be const */
int OEMCardSession_WriteTransparentByPath(
  ICardSession                *pMe,
  AEECardSessionFilePathType   sFilePath,
  uint32                       dwOffset,
  const AEECardSessionData    *pWriteData,
  AEECardSessionWriteTpStatus *pWriteStatus,
  AEECallback                 *pUserCB)
{
#ifdef FEATURE_MMGSDI
  mmgsdi_return_enum_type   mmgsdi_status = MMGSDI_ERROR;
  mmgsdi_slot_id_enum_type  mmgsdi_slot   = MMGSDI_SLOT_1;
  OEMCardSessionDataType    *pClientData  = NULL;
  int                       nReturnStatus = SUCCESS;
  mmgsdi_data_type          mmgsdi_data;
  mmgsdi_access_type        file_access;

  if (NULL == pWriteData)
  {
    MSG_ERROR("pWriteData is NULL",0,0,0);
    return EBADPARM;
  } 

  
  switch (sFilePath.dwFileIndication)
  {
    case AEECARDSESSION_FILE_PATH:
      nReturnStatus = OEMCardSession_MapFileAccess(sFilePath,
                                                  &file_access);
      if(nReturnStatus != SUCCESS){
        MSG_ERROR("Could not convert to MMGSDI File Path",0,0,0);
        return nReturnStatus;
      }
      break;

    case AEECARDSESSION_EF_FILE_NAME:
      return OEMCardSession_WriteTransparent(pMe,
                                       sFilePath.dwFileName,
                                       dwOffset,
                                       pWriteData,
                                       pWriteStatus,
                                       pUserCB);
    default:
      MSG_ERROR("Unknown File Indication 0x%x",sFilePath.dwFileIndication,0,0);
      return EBADPARM;
  }

  nReturnStatus = OEMCardSessionInitParams(pMe,
                                           pMe->m_u8Slot,
                                           (void*)pWriteStatus,
                                           pUserCB,
                                           &mmgsdi_slot,
                                           &pClientData);
  if (SUCCESS != nReturnStatus) {
    MSG_ERROR("Input Parameters Incorrect, status 0x%x",nReturnStatus,0,0);
    return nReturnStatus;
  }

  pWriteStatus->nCmdStatus = SUCCESS;

  MSG_HIGH("OEMCardSession_WriteTransparent",0,0,0);

  mmgsdi_data.data_len = pWriteData->nLen;
  mmgsdi_data.data_ptr = pWriteData->pData;

  mmgsdi_status =  mmgsdi_write_transparent_ext (
                     pMe->m_client_id,
                     mmgsdi_slot,
                     file_access,
                     uint32toint32(dwOffset),
                     mmgsdi_data,
                     OEMCardSessionCmdCb,
                     (mmgsdi_client_data_type)pClientData);
  if (MMGSDI_SUCCESS != mmgsdi_status) {
    OEMCardSession_CleanUpAsynObject(pClientData->ObjHandler);
    FREE(pClientData );
    pClientData = NULL;
    MSG_ERROR("OEMCardSession_WriteTransparentByPath Failed status = 0x%x",mmgsdi_status,0,0);
    return EFAILED;
  }

  return SUCCESS;
#else
  return EUNSUPPORTED;
#endif /* FEATURE_MMGSDI */
}
/*lint -save +esym(818,pWriteStatus,pUserCB) suppress could be const */


/*===========================================================================

FUNCTION: OEMCardSession_WriteRecordByPath

DESCRIPTION
  This function makes an async call to MMGSDI to write to a record based file.
  This function will attempt to write to a Linear Fixed File
DEPENDENCIES
  none

RETURN VALUE
  none

SIDE EFFECTS
===========================================================================*/
/*lint -e{715} suppress "pMe, dwFileName, dwRecNum, pWriteData, pWriteStatus, pUserCB not referenced" */
/*lint -save -esym(818,pCmdStatus,pUserCB) suppress could be const */
int OEMCardSession_WriteRecordByPath(
  ICardSession                 *pMe,
  AEECardSessionFilePathType    sFilePath,
  uint32                        dwRecNum,
  const AEECardSessionData     *pWriteData,
  AEECardSessionWriteRecStatus *pCmdStatus,
  AEECallback                  *pUserCB)
{
#ifdef FEATURE_MMGSDI
  mmgsdi_return_enum_type   mmgsdi_status = MMGSDI_ERROR;
  mmgsdi_slot_id_enum_type  mmgsdi_slot   = MMGSDI_SLOT_1;
  OEMCardSessionDataType    *pClientData  = NULL;
  int                       nReturnStatus = SUCCESS;
  mmgsdi_data_type          mmgsdi_data;
  mmgsdi_access_type        file_access;

  if (NULL == pWriteData)
  {
    MSG_ERROR("pWriteData is NULL",0,0,0);
    return EBADPARM;
  }

  switch (sFilePath.dwFileIndication)
  {
    case AEECARDSESSION_FILE_PATH:
      nReturnStatus = OEMCardSession_MapFileAccess(sFilePath,
                                                  &file_access);
      if(nReturnStatus != SUCCESS){
        MSG_ERROR("Could not convert to MMGSDI File Path",0,0,0);
        return nReturnStatus;
      }
      break;

    case AEECARDSESSION_EF_FILE_NAME:
      return OEMCardSession_WriteRecord(pMe,
                                       sFilePath.dwFileName,
                                       dwRecNum,
                                       pWriteData,
                                       pCmdStatus,
                                       pUserCB);
    default:
      MSG_ERROR("Unknown File Indication 0x%x",sFilePath.dwFileIndication,0,0);
      return EBADPARM;
  }
  
  nReturnStatus = OEMCardSessionInitParams(pMe,
                                           pMe->m_u8Slot,
                                           (void*)pCmdStatus,
                                           pUserCB,
                                           &mmgsdi_slot,
                                           &pClientData);
  if (SUCCESS != nReturnStatus) {
    MSG_ERROR("Input Parameters Incorrect, status 0x%x",nReturnStatus,0,0);
    return nReturnStatus;
  }

  pCmdStatus->nCmdStatus = SUCCESS;

  MSG_HIGH("OEMCardSession_WriteRecord",0,0,0);

  mmgsdi_data.data_len = pWriteData->nLen;
  mmgsdi_data.data_ptr = pWriteData->pData;

  mmgsdi_status =  mmgsdi_write_record_ext(pMe->m_client_id,
                                        mmgsdi_slot,
                                        file_access,
                                        MMGSDI_LINEAR_FIXED_FILE,
                                        uint32toint32(dwRecNum),
                                        mmgsdi_data,
                                        OEMCardSessionCmdCb,
                                        (mmgsdi_client_data_type)pClientData);
  if (MMGSDI_SUCCESS != mmgsdi_status) {
    OEMCardSession_CleanUpAsynObject(pClientData->ObjHandler);
    FREE(pClientData );
    pClientData = NULL;
    MSG_ERROR("OEMCardSession_WriteRecordByPath Failed status = 0x%x",
              mmgsdi_status,0,0);
    return EFAILED;
  }

  return SUCCESS;
#else
  return EUNSUPPORTED;
#endif /* FEATURE_MMGSDI */
}
/*lint -save +esym(818,pCmdStatus,pUserCB) suppress could be const */

/*===========================================================================

FUNCTION: OEMCardSession_IsServiceAvailable

DESCRIPTION
  The function finds out if the service is available in the card

RETURN VALUE
  SUCCESS   - Status information is valid
  EBADPARM  -  Bad parameter
  EUNSUPPORTED - Not supported

SIDE EFFECTS
  NONE
===========================================================================*/
/*lint -e{715} suppress "pMe, pCmdStatus, pUserCB not referenced" */
/*lint -save -esym(818,pMe,pCmdStatus,pUserCB) suppress could be const */
int OEMCardSession_IsServiceAvailable(
  ICardSession                  *pMe,
  uint32                         dwSrvType,
  AEECardSessionCmdStatus       *pCmdStatus,
  AEECallback                   *pUserCB)
{
#if defined (FEATURE_MMGSDI) && defined (FEATURE_MMGSDI_MBMS)
#error code not present
#else
  return EUNSUPPORTED;
#endif /* defined (FEATURE_MMGSDI) && defined (FEATURE_MMGSDI_MBMS) */
}


/*===========================================================================

FUNCTION: OEMCardSession_USIMAuthenticate

DESCRIPTION
  The function Sends a USIM Authenticate command to the card

RETURN VALUE
  SUCCESS   - Status information is valid
  EBADPARM  -  Bad parameter
  EUNSUPPORTED - Not supported

SIDE EFFECTS
  NONE
===========================================================================*/
/*lint -e{715} suppress "pMe, pAuthData, pAuthStatus, pUserCB not referenced" */
/*lint -save -esym(818,pMe,pAuthStatus,pUserCB) suppress could be const */
int OEMCardSession_USIMAuthenticate(
  ICardSession                  *pMe,
  const AEECardSessionData      *pAuthData,
  uint32                         dwContextType,
  AEECardSessionUSIMAuthStatus  *pAuthStatus,
  AEECallback                   *pUserCB)
{
#if defined (FEATURE_MMGSDI) && defined (FEATURE_MMGSDI_MBMS)
#error code not present
#else
  return EUNSUPPORTED;
#endif /* defined (FEATURE_MMGSDI) && defined (FEATURE_MMGSDI_MBMS) */
}

/*lint -save +esym(818,pAuthStatus,pUserCB) suppress could be const */


/*===========================================================================

FUNCTION: OEMCardSession_JCDMACard_GetInfo

DESCRIPTION
  This function provides card information on color, lock status, and insertion
  change. This function must be called after sim_init_completed

RETURN VALUE
  SUCCESS - Status information is valid
  EBADPARM -  Bad parameter
  EUNSUPPORTED - Not supported

SIDE EFFECTS
  If card type is invalid then subscription will not be sent and therefore
  the phone will not be able to register to a network.
===========================================================================*/
/*lint -e{715} suppress "pMe, pCmdStatus, pUserCB not referenced" */
/*lint -save -esym(818,pCmdStatus,pUserCB) suppress could be const */
int OEMCardSession_JCDMACard_GetInfo(
  ICardSession                  *pMe,
  AEECardSessionJCDMACardInfo   *pCmdStatus,
  AEECallback                   *pUserCB)
{
#if defined(FEATURE_MMGSDI) && defined(FEATURE_UIM_JCDMA_RUIM_SUPPORT)

  mmgsdi_return_enum_type   mmgsdi_status = MMGSDI_NOT_SUPPORTED;
  mmgsdi_slot_id_enum_type  mmgsdi_slot   = MMGSDI_MAX_SLOT_ID_ENUM;
  OEMCardSessionDataType    *pClientData  = NULL;
  int                       nReturnStatus = SUCCESS;

  MSG_HIGH("ICARDSESSION_JCDMACard_GetInfo ",0,0,0);

  if (NULL == pCmdStatus) {
    MSG_ERROR("ICARDSESSION_JCDMACard_GetInfo: pCmdStatus is NULL",0,0,0);
    return EBADPARM;
  }

  nReturnStatus = OEMCardSessionInitParams(pMe,
                                           pMe->m_u8Slot,
                                           (void*)pCmdStatus,
                                           pUserCB,
                                           &mmgsdi_slot,
                                           &pClientData);
  if (SUCCESS != nReturnStatus) {
    MSG_ERROR("ICARDSESSION_JCDMACard_GetInfo: OEMCardSessionInitParams Failed: %x",
               nReturnStatus,0,0);
    return nReturnStatus;
  }

  pCmdStatus->nCmdStatus = SUCCESS;

  mmgsdi_status = mmgsdi_jcdma_get_card_info (pMe->m_client_id,
                                              mmgsdi_slot,
                                              OEMCardSessionCmdCb,
                                              (mmgsdi_client_data_type)pClientData);

  if (MMGSDI_SUCCESS != mmgsdi_status) {
    OEMCardSession_CleanUpAsynObject(pClientData->ObjHandler);
    FREE(pClientData );
    pClientData = NULL;
    MSG_ERROR("ICARDSESSION_JCDMACard_GetInfo Failed status = 0x%x",
              mmgsdi_status,0,0);
  }

  return OEMCardSession_MapFromMMGSDIStatus(mmgsdi_status);
#else
  return EUNSUPPORTED;
#endif /* FEATURE_MMGSDI && FEATURE_UIM_JCDMA_RUIM_SUPPORT*/
}
/*lint -save +esym(818,pCmdStatus,pUserCB) suppress could be const */


/*===========================================================================

FUNCTION: OEMCardSession_MFLOInitActParams

DESCRIPTION
  The function Sends a MFLO Init Act Params

RETURN VALUE
  SUCCESS   - Status information is valid
  EBADPARM  -  Bad parameter
  EUNSUPPORTED - Not supported

SIDE EFFECTS
  NONE
===========================================================================*/
/*lint -e{715} suppress "pMe, pActParams, pOption, pInitActParamsStatus, pUserCB not referenced" */
/*lint -save -esym(818,pInitActParamsStatus,pUserCB) suppress could be const */
int OEMCardSession_MFLOInitActParams(
  ICardSession                 *pMe, 
  const AEECardSessionData     *pActParams,
  const AEECardSessionOption   *pOption,
  AEECardSessionCmdStatus      *pInitActParamsStatus,
  AEECallback                  *pUserCB)
{
#if defined (FEATURE_MMGSDI) && defined (FEATURE_MMGSDI_MFLO)
#error code not present
#else
  return EUNSUPPORTED;
#endif /* defined (FEATURE_MMGSDI) && defined (FEATURE_MMGSDI_MFLO) */
}
/*lint -save +esym(818,pInitActParamsStatus,pUserCB) suppress could be const */


/*===========================================================================

FUNCTION: OEMCardSession_MFLOGetSubscriberID

DESCRIPTION
  The function Sends a MFLO Get Subscriber ID

RETURN VALUE
  SUCCESS   - Status information is valid
  EBADPARM  -  Bad parameter
  EUNSUPPORTED - Not supported

SIDE EFFECTS
  NONE
===========================================================================*/
/*lint -e{715} suppress "pMe, pOption, pSubscriberIdStatus, pUserCB not referenced" */
/*lint -save -esym(818,pSubscriberIdStatus,pUserCB) suppress could be const */
int OEMCardSession_MFLOGetSubscriberID(
  ICardSession                          *pMe,
  const AEECardSessionOption            *pOption,
  AEECardSessionMFLOSubIdStatus         *pSubscriberIdStatus,
  AEECallback                           *pUserCB)
{
#if defined (FEATURE_MMGSDI) && defined (FEATURE_MMGSDI_MFLO)
#error code not present
#else
  return EUNSUPPORTED;
#endif /* defined (FEATURE_MMGSDI) && defined (FEATURE_MMGSDI_MFLO) */
}
/*lint -save +esym(818,pSubscriberIdStatus,pUserCB) suppress could be const */


/*===========================================================================

FUNCTION: OEMCardSession_MFLOGetPublicKey

DESCRIPTION
  The function Sends a MFLO Get Public Key

RETURN VALUE
  SUCCESS   - Status information is valid
  EBADPARM  -  Bad parameter
  EUNSUPPORTED - Not supported

SIDE EFFECTS
  NONE
===========================================================================*/
/*lint -e{715} suppress "pMe, pOption, pPublicKeyStatus, pUserCB not referenced" */
/*lint -save -esym(818,pPublicKeyStatus,pUserCB) suppress could be const */
int OEMCardSession_MFLOGetPublicKey(
  ICardSession                          *pMe,
  const AEECardSessionOption            *pOption,
  AEECardSessionMFLOPubKeyStatus        *pPublicKeyStatus,
  AEECallback                           *pUserCB)
{
#if defined (FEATURE_MMGSDI) && defined (FEATURE_MMGSDI_MFLO)
#error code not present
#else
  return EUNSUPPORTED;
#endif /* defined (FEATURE_MMGSDI) && defined (FEATURE_MMGSDI_MFLO) */
}
/*lint -save +esym(818,pPublicKeyStatus,pUserCB) suppress could be const */


/*===========================================================================

FUNCTION: OEMCardSession_MFLOSignUcast

DESCRIPTION
  The function Sends a MFLO Sign UCast Message

RETURN VALUE
  SUCCESS   - Status information is valid
  EBADPARM  -  Bad parameter
  EUNSUPPORTED - Not supported

SIDE EFFECTS
  NONE
===========================================================================*/
/*lint -e{715} suppress "pMe, dwMsgType, pMsg, pOption, pSignUcastStatus, pUserCB not referenced" */
/*lint -save -esym(818,pSignUcastStatus,pUserCB) suppress could be const */
int OEMCardSession_MFLOSignUcast(
  ICardSession                      *pMe,
  uint32                             dwMsgType,
  const AEECardSessionData          *pMsg,
  const AEECardSessionOption        *pOption,
  AEECardSessionCmdDataStatus       *pSignUcastStatus,
  AEECallback                       *pUserCB)
{
#if defined (FEATURE_MMGSDI) && defined (FEATURE_MMGSDI_MFLO)
#error code not present
#else
  return EUNSUPPORTED;
#endif /* defined (FEATURE_MMGSDI) && defined (FEATURE_MMGSDI_MFLO) */
}
/*lint -save +esym(818,pSignUcastStatus,pUserCB) suppress could be const */


/*===========================================================================

FUNCTION: OEMCardSession_MFLOVerifyUcast

DESCRIPTION
  The function Sends a MFLO Verify UCast Message

RETURN VALUE
  SUCCESS   - Status information is valid
  EBADPARM  -  Bad parameter
  EUNSUPPORTED - Not supported

SIDE EFFECTS
  NONE
===========================================================================*/
/*lint -e{715} suppress "pMe, dwMsgType, pMsg, pOption, pVerifyUcastStatus, pUserCB not referenced" */
/*lint -save -esym(818,pVerifyUcastStatus,pUserCB) suppress could be const */
int OEMCardSession_MFLOVerifyUcast(
  ICardSession                      *pMe,
  uint32                             dwMsgType,
  const AEECardSessionData          *pMsg,
  const AEECardSessionOption        *pOption,
  AEECardSessionCmdStatus           *pVerifyUcastStatus,
  AEECallback                       *pUserCB)
{
#if defined (FEATURE_MMGSDI) && defined (FEATURE_MMGSDI_MFLO)
#error code not present
#else
  return EUNSUPPORTED;
#endif /* defined (FEATURE_MMGSDI) && defined (FEATURE_MMGSDI_MFLO) */
}
/*lint -save +esym(818,pVerifyUcastStatus,pUserCB) suppress could be const */


/*===========================================================================

FUNCTION: OEMCardSession_GetATR

DESCRIPTION
  The function Get ATR of the card

RETURN VALUE
  SUCCESS   - Status information is valid
  EBADPARM  -  Bad parameter
  EUNSUPPORTED - Not supported

SIDE EFFECTS
  NONE
===========================================================================*/
/*lint -e{715} suppress "pMe, pOption, pSubscriberIdStatus, pUserCB not referenced" */
/*lint -save -esym(818,pGetATRStatus,pUserCB) suppress could be const */
int OEMCardSession_GetATR(
  ICardSession                 *pMe,
  const AEECardSessionOption   *pOption,
  AEECardSessionCmdDataStatus  *pGetATRStatus,
  AEECallback                  *pUserCB)
{
#if defined (FEATURE_MMGSDI)
  mmgsdi_return_enum_type         mmgsdi_status = MMGSDI_NOT_SUPPORTED;
  mmgsdi_slot_id_enum_type        mmgsdi_slot   = MMGSDI_MAX_SLOT_ID_ENUM;
  OEMCardSessionDataType         *pClientData   = NULL;
  int                             nReturnStatus = SUCCESS;
  mmgsdi_option_type              option;

  MSG_HIGH("ICARDSESSION_GetATR ",0,0,0);

  (void) MEMSET(&option, 0x00, sizeof(mmgsdi_option_type));

  nReturnStatus = OEMCardSessionInitParams(pMe,
                                           pMe->m_u8Slot,
                                           (void*)pGetATRStatus,
                                           pUserCB,
                                           &mmgsdi_slot,
                                           &pClientData);
  if (SUCCESS != nReturnStatus) {
    return nReturnStatus;
  }
  
  pGetATRStatus->nCmdStatus = SUCCESS;

  mmgsdi_status = mmgsdi_get_atr (pMe->m_client_id,
                                  mmgsdi_slot,
                                  option,
                                  OEMCardSessionCmdCb,
                                  (mmgsdi_client_data_type)pClientData);

  if (MMGSDI_SUCCESS != mmgsdi_status) {
    OEMCardSession_CleanUpAsynObject(pClientData->ObjHandler);
    FREE(pClientData );
    pClientData = NULL;
    MSG_ERROR("ICARDSESSION_GetATR Failed status = 0x%x",
              mmgsdi_status,0,0);
  }

  return OEMCardSession_MapFromMMGSDIStatus(mmgsdi_status);
#else
  return EUNSUPPORTED;
#endif /* defined (FEATURE_MMGSDI) */
}
/*lint -save +esym(818,pGetATRStatus,pUserCB) suppress could be const */


/*===========================================================================

FUNCTION: OEMCardSession_GetCardReaderStatus

DESCRIPTION
  The function Get Card Reader Status of the card

RETURN VALUE
  SUCCESS   - Status information is valid
  EBADPARM  -  Bad parameter
  EUNSUPPORTED - Not supported

SIDE EFFECTS
  NONE
===========================================================================*/
/*lint -e{715} suppress "pMe, pOption, pGetCardReaderStatus, pUserCB not referenced" */
/*lint -save -esym(818,pGetCardReaderStatus,pUserCB) suppress could be const */
int OEMCardSession_GetCardReaderStatus(
  ICardSession                   *pMe,
  const AEECardSessionOption     *pOption,
  AEECardSessionCardReaderStatus *pGetCardReaderStatus,
  AEECallback                    *pUserCB)
{
#if defined (FEATURE_MMGSDI)
  mmgsdi_return_enum_type         mmgsdi_status = MMGSDI_NOT_SUPPORTED;
  mmgsdi_slot_id_enum_type        mmgsdi_slot   = MMGSDI_MAX_SLOT_ID_ENUM;
  OEMCardSessionDataType         *pClientData   = NULL;
  int                             nReturnStatus = SUCCESS;
  mmgsdi_option_type              option;

  MSG_HIGH("ICARDSESSION_GetCardReaderStatus ",0,0,0);

  (void) MEMSET(&option, 0x00, sizeof(mmgsdi_option_type));

  nReturnStatus = OEMCardSessionInitParams(pMe,
                                           pMe->m_u8Slot,
                                           (void*)pGetCardReaderStatus,
                                           pUserCB,
                                           &mmgsdi_slot,
                                           &pClientData);
  if (SUCCESS != nReturnStatus) {
    return nReturnStatus;
  }
  
  pGetCardReaderStatus->nCmdStatus = SUCCESS;

  mmgsdi_status = mmgsdi_get_card_reader_status (pMe->m_client_id,
                                                 mmgsdi_slot,
                                                 option,
                                                 OEMCardSessionCmdCb,
                                                 (mmgsdi_client_data_type)pClientData);

  if (MMGSDI_SUCCESS != mmgsdi_status) {
    OEMCardSession_CleanUpAsynObject(pClientData->ObjHandler);
    FREE(pClientData );
    pClientData = NULL;
    MSG_ERROR("ICARDSESSION_GetCardReaderStatus Failed status = 0x%x",
              mmgsdi_status,0,0);
  }

  return OEMCardSession_MapFromMMGSDIStatus(mmgsdi_status);
#else
  return EUNSUPPORTED;
#endif /* defined (FEATURE_MMGSDI) */
}
/*lint -save +esym(818,pGetCardReaderStatus,pUserCB) suppress could be const */


/*===========================================================================

FUNCTION: OEMCardSession_SendAPDU

DESCRIPTION
  The function Sends APDU to the card

RETURN VALUE
  SUCCESS   - Status information is valid
  EBADPARM  -  Bad parameter
  EUNSUPPORTED - Not supported

SIDE EFFECTS
  NONE
===========================================================================*/
/*lint -e{715} suppress "pMe, pAPDU, pOption, pSendAPDUStatus, pUserCB not referenced" */
/*lint -save -esym(818,pSendAPDUStatus,pUserCB) suppress could be const */
int OEMCardSession_SendAPDU(
  ICardSession                      *pMe,
  const AEECardSessionData          *pAPDU,
  const AEECardSessionOption        *pOption,
  AEECardSessionCmdDataStatus       *pSendAPDUStatus,
  AEECallback                       *pUserCB)
{
#if defined (FEATURE_MMGSDI) && defined (FEATURE_MMGSDI_MFLO)
#error code not present
#else
  return EUNSUPPORTED;
#endif /* defined (FEATURE_MMGSDI) && defined (FEATURE_MMGSDI_MFLO) */
}
/*lint -save +esym(818,pSendAPDUStatus,pUserCB) suppress could be const */


/*===========================================================================

                    ICardSessionCore functions

===========================================================================*/


/*=============================================================================
FUNCTION: OEMCardSessionCore_AddRef

DESCRIPTION:


DEPENDENCIES:
  None

SIDE EFFECTS:
  None

=============================================================================*/
static uint32 OEMCardSessionCore_AddRef()
{
  ICardSessionCore *pMe = ICardSessionCoreObj;

  MSG_HIGH("OEMCardSessionCore_AddRef", 0, 0, 0);

  if (NULL != pMe) {
    ++pMe->m_uRefs;
    return pMe->m_uRefs;
  }
  else
    return 0;
}


/*=============================================================================
FUNCTION: OEMCardSessionCore_New

DESCRIPTION:


DEPENDENCIES:
  None

SIDE EFFECTS:
  None

=============================================================================*/
ICardSessionCore * OEMCardSessionCore_New(void)
{

  MSG_HIGH("OEMCardSessionCore_New", 0, 0, 0);

  if (NULL != ICardSessionCoreObj) {
    (void) OEMCardSessionCore_AddRef();
  }
  else {

    /* The OEMCardSession_MALLOC gives us zero-initialized memory, so
     * everything starts out as zero/FALSE.
     */
    MSG_HIGH("Init Core Object",0,0,0);
    ICardSessionCoreObj = (ICardSessionCore *)MALLOC(sizeof(ICardSessionCore));
    if (NULL == ICardSessionCoreObj)
      return NULL;

    ICardSessionCoreObj->m_uRefs = 1;

    ICardSessionCoreObj->m_SessionOwner = NULL;

  }
  return ICardSessionCoreObj;
}


/*=============================================================================
FUNCTION: OEMCardSessionCore_Release

DESCRIPTION:
   This function frees up the Coreobject.

DEPENDENCIES:
  None

SIDE EFFECTS:
  None

=============================================================================*/
uint32 OEMCardSessionCore_Release(void)
{
  ICardSessionCore *pMe = ICardSessionCoreObj;

  MSG_HIGH("OEMCardSessionCore_Release", 0, 0, 0);

  if (NULL == pMe){
    return 0;
  }

  if (pMe->m_uRefs > 1) {
    return --pMe->m_uRefs;
  }

  // Ref count is zero.  Release memory
  OEMCardSessionCore_FreeOnAEEExit(pMe);
  return(0);
}


/*=============================================================================
FUNCTION: OEMCardSessionCore_FreeOnAEEExit

DESCRIPTION:
  This function is called by ICardSessionNotifier if it exits in response to
  BREW exiting. Hopefully the object reference is zero by now and somebody
  has already freed the core object. Just in case that has not happened, we do
  it here.

DEPENDENCIES:
  None

SIDE EFFECTS:
  None

=============================================================================*/
void OEMCardSessionCore_FreeOnAEEExit(
  void* pUser)
{
  MSG_HIGH("OEMCardSessionCore_FreeOnAEEExit 0x%x", pUser, 0, 0);

  if (NULL != ICardSessionCoreObj) {
    OEM_Free(ICardSessionCoreObj);
    ICardSessionCoreObj = NULL;
    return;
  }
}

/*=============================================================================
FUNCTION: OEMCardSession_Register

DESCRIPTION:
  API to register for Client ID with MMGSDI.
  Every app will need to do a client registration of its own.

DEPENDENCIES:
  None

SIDE EFFECTS:
  None

=============================================================================*/
/*lint -e{715} suppress "pMe, pCmdStatus, pUserCB not referenced" */
/*lint -save -esym(818,pCmdStatus,pUserCB suppress could be const */
int OEMCardSession_Register(
  ICardSession            *pMe,
  AEECardSessionCmdStatus *pCmdStatus,
  AEECallback             *pUserCB)
{
#ifdef FEATURE_MMGSDI
  mmgsdi_return_enum_type   mmgsdi_status = MMGSDI_ERROR;
  OEMCardSessionDataType   *pClientData   = NULL;
  int                       nReturnStatus = SUCCESS;

  if (NULL == pMe || 
      NULL == pCmdStatus ||
      NULL == pUserCB) 
  {
    MSG_HIGH("NULL Pointer to IcardSession passed in", 0, 0, 0);
    return EBADPARM;
  }

  MSG_HIGH("OEMCardSession_Register", 0, 0, 0);

  pClientData = (OEMCardSessionDataType*)MALLOC(sizeof(OEMCardSessionDataType));
  if (NULL == pClientData) {
    MSG_ERROR("Unable to Alloc pClientData", 0, 0, 0);
    return ENOMEMORY;
  }

  /* Get and populate the callback */
  nReturnStatus = OEMCardSession_GetLinkSysCallBack(
                    pUserCB,
                    pCmdStatus,
                    &((pClientData)->ObjHandler));
  if (SUCCESS != nReturnStatus) {
    FREE( pClientData );
    pClientData = NULL;
    return nReturnStatus;
  }

  pClientData->m_pCardSession = pMe;

  /* initialize to success */
  pCmdStatus->nCmdStatus = SUCCESS;

  mmgsdi_status = mmgsdi_client_id_reg (OEMCardSessionCmdCb,
                                        (mmgsdi_client_data_type)pClientData);

  if (MMGSDI_SUCCESS != mmgsdi_status) {
    OEMCardSession_CleanUpAsynObject(pClientData->ObjHandler);
    FREE( pClientData );
    pClientData = NULL;
    MSG_ERROR("Unable to get MMGSDI client id 0x%x", mmgsdi_status, 0, 0);
    return EFAILED;
  }

  return SUCCESS;
#else
  return EUNSUPPORTED;
#endif /*FEATURE_MMGSDI*/
}
/*lint -save +esym(818,pCmdStatus,pUserCB) suppress could be const */


/*===========================================================================

                    ICardSession Utility functions

===========================================================================*/
/*=============================================================================
FUNCTION: OEMCardSession_Free

DESCRIPTION:


DEPENDENCIES:
  None

SIDE EFFECTS:
  None

=============================================================================*/
void OEMCardSession_Free(ICardSession *pMe, boolean session_open)
{
  AEECardSessionOwner       *curr          = NULL;
  AEECardSessionOwner       *prev          = NULL;
#ifdef FEATURE_MMGSDI
  boolean                   flag           = TRUE;
  mmgsdi_return_enum_type    mmgsdi_status = MMGSDI_ERROR;
#endif /* FEATURE_MMGSDI */

  MSG_HIGH("OEMCardSession_Free 0x%x", pMe, 0, 0);

  if ((NULL == pMe) || (NULL == pMe->m_coreObj)){
    return;
  }

  /* Release the model object ptr */
  (void) OEMCardSessionModel_Release(pMe->m_modelObj);

  curr = pMe->m_coreObj->m_SessionOwner;

  while (curr) {
    prev = curr;
    curr = curr->pNext;

    if (prev->clientID == pMe->m_client_id) {
      MSG_HIGH("Client ID 0x%x", pMe->m_client_id, 0, 0);

      OEMCARDSESSION_REMOVE_NODE(ICardSessionCoreObj->m_SessionOwner, prev);


#ifdef FEATURE_MMGSDI
      /*Dereg Client ID here only if there is no session, mmgsdi will
        reject client dereg*/
      if(!session_open)
      {
        mmgsdi_status =  mmgsdi_client_id_dereg (pMe->m_client_id,
                                                 OEMCardSessionFreeCb,
                                                 0);
        if (MMGSDI_SUCCESS != mmgsdi_status){
          MSG_ERROR("Could not deregister Client",0,0,0);
        }
      }
      flag = FALSE;
#endif /*FEATURE_MMGSDI*/

      OEMCARDSESSION_SYS_FREE(prev);
      break;
    }
  }
#ifdef FEATURE_MMGSDI
  if (curr == NULL && flag != FALSE)
  {
    /* session_model is not created*/
      /*DeReg Client ID only*/
      mmgsdi_status =  mmgsdi_client_id_dereg (pMe->m_client_id,
                                               OEMCardSessionFreeCb,
                                               0);
      if (MMGSDI_SUCCESS != mmgsdi_status){
        MSG_ERROR("Could not deregister Client",0,0,0);
      }
  }  
#endif /*FEATURE_MMGSDI*/
}



#ifdef FEATURE_MMGSDI
/*===========================================================================
FUNCTION: OEMCardSession_MapMMGSDISlot

DESCRIPTION
  Map AEECardSession slot to MMGSDI Slot enum

DEPENDENCIES
  none

RETURN VALUE
  none

SIDE EFFECTS

===========================================================================*/
int OEMCardSession_MapMMGSDISlot(
  OEMCardSessionSlotEnum    u8SlotID,
  mmgsdi_slot_id_enum_type *mmgsdi_slot)
{
  MSG_HIGH("OEMCardSession_MapMMGSDISlot Card Slot 0x%x", u8SlotID, 0, 0);

  if (NULL == mmgsdi_slot) {
    return EBADPARM;
  }

  *mmgsdi_slot = MMGSDI_SLOT_1;

  switch (u8SlotID) {
    case OEMCARDSESSION_SLOT1:
      MSG_HIGH("MMGSDI_SLOT_1", 0, 0, 0);
      *mmgsdi_slot = MMGSDI_SLOT_1;
      break;
    case OEMCARDSESSION_SLOT2:
#ifdef FEATURE_MMGSDI_DUAL_SLOT
      MSG_HIGH("MMGSDI_SLOT_2", 0, 0, 0);
      *mmgsdi_slot = MMGSDI_SLOT_2;
#endif/*FEATURE_MMGSDI_DUAL_SLOT */
      break;
    default:
      MSG_HIGH("Unknown OEMCardSession Slot 0x%x", u8SlotID, 0, 0);
      *mmgsdi_slot = MMGSDI_SLOT_1;
      return EBADPARM;
  }
  return SUCCESS;
} /* OEMCardSession_MapMMGSDISlot */


/*===========================================================================
FUNCTION: OEMCardSession_MapFromMMGSDIFileEnum

DESCRIPTION
  Map AEECardSession File Enum to MMGSDI File Enum

DEPENDENCIES
  none

RETURN VALUE
  none

SIDE EFFECTS

===========================================================================*/
int OEMCardSession_MapFromMMGSDIFileEnum(
  mmgsdi_file_enum_type  sFileEnum,
  uint32                *pCardFileEnum)
{
  int i = 0;
  int nTableSize = ARR_SIZE(OEMCardSessionMMGSDIFileEnumTable);

  MSG_HIGH("OEMCardSession_MapFromMMGSDIFileEnum MMGSDI File 0x%x",
           sFileEnum, 0, 0);

  if (NULL == pCardFileEnum) {
    return EBADPARM;
  }

  for (i = 0; i < nTableSize; i++) {
    if (sFileEnum == OEMCardSessionMMGSDIFileEnumTable[i].nMMGSDIFile) {
      *pCardFileEnum = OEMCardSessionMMGSDIFileEnumTable[i].nCardFile;
      MSG_HIGH("AEECardSession File Enum 0x%x", *pCardFileEnum, 0, 0);
      return SUCCESS;
    }
  }

  MSG_ERROR("MMGSDI File Enum Map Error", 0, 0, 0);
  return EBADITEM;
}


/*===========================================================================
FUNCTION: OEMCardSession_MapFileEnum

DESCRIPTION
  Map AEECardSession File Enum to GSDI File Enum

DEPENDENCIES
  none

RETURN VALUE
  none

SIDE EFFECTS

===========================================================================*/
int OEMCardSession_MapFileEnum(
  mmgsdi_file_enum_type *pFileEnum,
  uint32                 dwCardFileEnum)
{
  int i = 0;
  int nTableSize = ARR_SIZE(OEMCardSessionMMGSDIFileEnumTable);

  MSG_HIGH("OEMCardSession_MapFileEnum Card File 0x%x",
           dwCardFileEnum, 0, 0);

  if (NULL == pFileEnum) {
    return EBADPARM;
  }

  for (i = 0; i < nTableSize; i++) {
    if (dwCardFileEnum == OEMCardSessionMMGSDIFileEnumTable[i].nCardFile) {
      *pFileEnum = OEMCardSessionMMGSDIFileEnumTable[i].nMMGSDIFile;
      MSG_HIGH("MMGSDI File Enum 0x%x", *pFileEnum, 0, 0);
      return SUCCESS;
    }
  }

  MSG_ERROR("AEECardSession File Enum Map Error", 0, 0, 0);
  return EBADITEM;
}


/*===========================================================================
FUNCTION: OEMCardSession_MapFileAccess

DESCRIPTION
  Map AEECardSession File Access to GSDI File Access

DEPENDENCIES
  none

RETURN VALUE
  EBADPARM - Parameters provided were incorrect
  SUCCESS  - Could Successfully map the path

SIDE EFFECTS

===========================================================================*/
int OEMCardSession_MapFileAccess(
  AEECardSessionFilePathType sFilePath,
  mmgsdi_access_type         *pFileAccess)
{
  int i = 0, j=0;
  
  if (NULL == pFileAccess) {
    return EBADPARM;
  }

  pFileAccess->access_method = MMGSDI_BY_PATH_ACCESS;

  /*Check for File length and also make sure that we have 2 
    ytes for every file listed in the file path*/
  if (sFilePath.sPath.nLen == 0 ||
     sFilePath.sPath.nLen > MMGSDI_MAX_PATH_LEN*2 ||
     sFilePath.sPath.nLen%2 != 0) {
    MSG_ERROR("File Path Len is wrong 0x%x",sFilePath.sPath.nLen,0,0);
    return EBADPARM;
  }
  
  
  pFileAccess->file.path_type.path_len = sFilePath.sPath.nLen/2;
  
  for (i=0;i<sFilePath.sPath.nLen;i++) {
    (void) MEMCPY(&pFileAccess->file.path_type.path_buf[j++],
           &sFilePath.sPath.pData[i],
           sizeof(uint16));
    i++;
  }
  return SUCCESS;
}


/*===========================================================================
FUNCTION: OEMCardSession_MapFromMMGSDIStatus

DESCRIPTION
  Map MMGSDI status to Brew status

DEPENDENCIES
  none

RETURN VALUE
  none

SIDE EFFECTS

===========================================================================*/
int OEMCardSession_MapFromMMGSDIStatus(
  mmgsdi_return_enum_type sStatus)
{
  switch (sStatus)
  {
    case MMGSDI_SUCCESS:
      return SUCCESS;
    case MMGSDI_INCORRECT_PARAMS:
      return EBADPARM;
    case MMGSDI_NOT_SUPPORTED:
      return EUNSUPPORTED;
    default:
      return EFAILED;
  }
}


/*===========================================================================
FUNCTION: OEMCardSession_MapPinID

DESCRIPTION
  Map AEECardSession Pin ID to MMGSDI Pin ID

DEPENDENCIES
  none

RETURN VALUE
  none

SIDE EFFECTS

===========================================================================*/
int OEMCardSession_MapPinID(
  mmgsdi_pin_enum_type *pPinIDEnum,
  uint32                dwPinIDEnum)
{
  MSG_HIGH("OEMCardSession_MapPinID Card File 0x%x",
           dwPinIDEnum, 0, 0);

  if (NULL == pPinIDEnum)
    return EBADPARM;

  switch (dwPinIDEnum)
  {
    case AEECARDSESSION_PIN_1:
      MSG_HIGH("MMGSDI_PIN1", 0, 0, 0);
      *pPinIDEnum = MMGSDI_PIN1;
      break;
    case AEECARDSESSION_PIN_2:
      MSG_HIGH("MMGSDI_PIN2", 0, 0, 0);
      *pPinIDEnum = MMGSDI_PIN2;
      break;
    case AEECARDSESSION_PIN_UNIVERSAL:
      MSG_HIGH("MMGSDI_PIN_UNIVERSAL", 0, 0, 0);
      *pPinIDEnum = MMGSDI_UNIVERSAL_PIN;
      break;
    default:
      MSG_ERROR("Invalid AEECardSession PIN 0x%x", dwPinIDEnum, 0, 0);
      return EUNSUPPORTED;
  }
  return SUCCESS;
}

/*===========================================================================
FUNCTION: OEMCardSession_MapReplaceOption

DESCRIPTION
  Map AEECardSession Replace Option to MMGSDI Replace Option

DEPENDENCIES
  none

RETURN VALUE
  none

SIDE EFFECTS

===========================================================================*/
int OEMCardSession_MapReplaceOption(
  mmgsdi_pin_replace_enum_type   *pReplaceEnum,
  uint32                         dwReplaceEnum)
{
  MSG_HIGH("OEMCardSession_MapReplaceOption Replace 0x%x",
           dwReplaceEnum, 0, 0);

  if (NULL == pReplaceEnum)  
  {
    MSG_ERROR("pReplaceEnum ptr is NULL",0,0,0);
    return EBADPARM;
  }

  switch (dwReplaceEnum)
  {
    case AEECARDSESSION_PIN_REPLACE_BY_UNIV:
      MSG_HIGH("Replace with Universal", 0, 0, 0);
      *pReplaceEnum = MMGSDI_PIN_REPLACED_BY_UNIVERSAL;
      break;
    case AEECARDSESSION_PIN_NOT_REPLACED_BY_UNIV:
      MSG_HIGH("No Replacement", 0, 0, 0);
      *pReplaceEnum = MMGSDI_PIN_NOT_REPLACED_BY_UNIVERSAL;
      break;
    default:
      MSG_ERROR("Invalid AEECardSession Replace 0x%x", dwReplaceEnum, 0, 0);
      return EUNSUPPORTED;
  }
  return SUCCESS;
}

/*===========================================================================
FUNCTION: OEMCardSession_MapFromMMGSDIPinID

DESCRIPTION
  Map MMGSDI Pin ID to AEECardSession Pin ID

DEPENDENCIES
  none

RETURN VALUE
  none

SIDE EFFECTS

===========================================================================*/
int OEMCardSession_MapFromMMGSDIPinID(
  mmgsdi_pin_enum_type sPinIDEnum,
  uint32              *pCardPinIDEnum)
{
  MSG_HIGH("OEMCardSession_MapFromMMGSDIPinID MMGSDI Pin 0x%x",
           sPinIDEnum, 0, 0);

  if (NULL == pCardPinIDEnum) {
    MSG_ERROR("Null Input Parm", 0, 0, 0);
    return EBADPARM;
  }

  switch (sPinIDEnum)
  {
    case MMGSDI_PIN1:
      MSG_HIGH("AEECARDSESSION_PIN_1", 0, 0, 0);
      *pCardPinIDEnum = AEECARDSESSION_PIN_1;
      break;
    case MMGSDI_PIN2:
      MSG_HIGH("AEECARDSESSION_PIN_2", 0, 0, 0);
      *pCardPinIDEnum = AEECARDSESSION_PIN_2;
      break;
    case MMGSDI_UNIVERSAL_PIN:
      MSG_HIGH("AEECARDSESSION_PIN_UNIVERSAL", 0, 0, 0);
      *pCardPinIDEnum = AEECARDSESSION_PIN_UNIVERSAL;
      break;
    default:
      MSG_ERROR("Invalid MMGSDI Pin 0x%x", sPinIDEnum, 0, 0);
      return EBADITEM;
  }
  return SUCCESS;
}


/*===========================================================================
FUNCTION: OEMCardSession_MapFromMMGSDIPinStatus

DESCRIPTION
  Map MMGSDI Pin Status to AEECardSession Pin Status

DEPENDENCIES
  none

RETURN VALUE
  none

SIDE EFFECTS

===========================================================================*/
int OEMCardSession_MapFromMMGSDIPinStatus(
  mmgsdi_pin_info_type        sPinInfo,
  uint32                     *pCardPinRetries,
  uint32                     *pCardPukRetries,
  uint32                     *pCardPinStatusEnum)
{
  MSG_HIGH(
    "OEMCardSession_MapFromMMGSDIPinStatus st 0x%x, # retries 0x%x, unblock retries 0x%x",
    sPinInfo.status, sPinInfo.num_retries, sPinInfo.num_unblock_retries);

  if (NULL == pCardPinStatusEnum) {
    MSG_ERROR("Null Input Parm", 0, 0, 0);
    return EBADPARM;
  }

  *pCardPinStatusEnum = AEECARDSESSION_PIN_NOT_INITIALIZED;

  switch (sPinInfo.status)
  {
    case MMGSDI_PIN_ENABLED_NOT_VERIFIED:
      MSG_HIGH("AEECARDSESSION_PIN_ENABLED_NOT_VERIFIED", 0, 0, 0);
      *pCardPinStatusEnum = AEECARDSESSION_PIN_ENABLED_NOT_VERIFIED;
      break;
    case MMGSDI_PIN_ENABLED_VERIFIED:
    case MMGSDI_PIN_CHANGED:
    case MMGSDI_PIN_UNBLOCKED:
      MSG_HIGH("AEECARDSESSION_PIN_ENABLED_VERIFIED", 0, 0, 0);
      *pCardPinStatusEnum = AEECARDSESSION_PIN_ENABLED_VERIFIED;
      break;
    case MMGSDI_PIN_DISABLED:
      MSG_HIGH("AEECARDSESSION_PIN_DISABLED", 0, 0, 0);
      *pCardPinStatusEnum = AEECARDSESSION_PIN_DISABLED;
      break;
    case MMGSDI_PIN_BLOCKED:
      MSG_HIGH("AEECARDSESSION_PIN_BLOCKED", 0, 0, 0);
      *pCardPinStatusEnum = AEECARDSESSION_PIN_BLOCKED;
      break;
    case MMGSDI_PIN_PERM_BLOCKED:
      MSG_HIGH("AEECARDSESSION_PIN_PERM_BLOCKED", 0, 0, 0);
      *pCardPinStatusEnum = AEECARDSESSION_PIN_PERM_BLOCKED;
      break;
    case MMGSDI_PIN_STATUS_NOT_INITIALIZED:
      MSG_HIGH("AEECARDSESSION_PIN_NOT_INITIALIZED", 0, 0, 0);
      *pCardPinStatusEnum = AEECARDSESSION_PIN_NOT_INITIALIZED;
      break;
  }
  *pCardPinRetries = sPinInfo.num_retries;
  *pCardPukRetries = sPinInfo.num_unblock_retries;

  return SUCCESS;
}


/*===========================================================================
FUNCTION: OEMCardSession_MapFromMMGSDIPinOp

DESCRIPTION
  Map MMGSDI Pin Operation to AEECardSession Pin Operation

DEPENDENCIES
  none

RETURN VALUE
  none

SIDE EFFECTS

===========================================================================*/
int OEMCardSession_MapFromMMGSDIPinOp(
  mmgsdi_pin_operation_enum_type sPinOpEnum,
  uint32                        *pCardPinOpEnum)
{
  MSG_HIGH("OEMCardSession_MapFromMMGSDIPinOp MMGSDI Pin Op 0x%x",
           sPinOpEnum, 0, 0);

  if (NULL == pCardPinOpEnum) {
    MSG_ERROR("Null Input Parm", 0, 0, 0);
    return EBADPARM;
  }

  switch (sPinOpEnum)
  {
    case MMGSDI_PIN_OP_VERIFY:
      MSG_HIGH("AEECARDSESSION_PIN_VERIFY_OP", 0, 0, 0);
      *pCardPinOpEnum = AEECARDSESSION_PIN_VERIFY_OP;
      break;
    case MMGSDI_PIN_OP_ENABLE:
      MSG_HIGH("AEECARDSESSION_PIN_ENABLE_OP", 0, 0, 0);
      *pCardPinOpEnum = AEECARDSESSION_PIN_ENABLE_OP;
      break;
    case MMGSDI_PIN_OP_DISABLE:
    case MMGSDI_PIN_OP_DISABLE_AND_REPLACE:
      MSG_HIGH("AEECARDSESSION_PIN_DISABLE_OP or MMGSDI_PIN_OP_DISABLE_AND_REPLACE",
                0, 0, 0);
      *pCardPinOpEnum = AEECARDSESSION_PIN_DISABLE_OP;
      break;
    case MMGSDI_PIN_OP_CHANGE:
      MSG_HIGH("AEECARDSESSION_PIN_CHANGE_OP", 0, 0, 0);
      *pCardPinOpEnum = AEECARDSESSION_PIN_CHANGE_OP;
      break;
    case MMGSDI_PIN_OP_UNBLOCK:
      MSG_HIGH("AEECARDSESSION_PIN_UNBLOCK_OP", 0, 0, 0);
      *pCardPinOpEnum = AEECARDSESSION_PIN_UNBLOCK_OP;
      break;
    case MMGSDI_PIN_OP_NONE:
      MSG_ERROR("Invalid MMGSDI Op MMGSDI_PIN_OP_NONE", 0, 0, 0);
      return EUNSUPPORTED;
    default:
      break;
  }
  return SUCCESS;
}


/*===========================================================================

FUNCTION: OEMCardSession_CleanUpAsynObject

DESCRIPTION
  Performs the clean up process for the object manager handler, the content
  of the object.  boolean bCBCancel indicates if the CALLBACK_Cancel should
  be called for the AEECallback * within the OEMCard_AsynCBInfo structure

DEPENDENCIES
  none

RETURN VALUE
  none

SIDE EFFECTS
===========================================================================*/
void OEMCardSession_CleanUpAsynObject(
  AEEObjectHandle Handler)
{
  OEMCardSessionAsynCBInfo *pAsynInfo = NULL;

  OEMCARDSESSION_ENTER_CRITICAL_SECTION
  pAsynInfo = (OEMCardSessionAsynCBInfo*)AEEObjectMgr_GetObject(Handler);
  if (NULL == pAsynInfo) {
    /* Unregister the handler */
    (void)AEEObjectMgr_Unregister(Handler);
    OEMCARDSESSION_LEAVE_CRITICAL_SECTION
    return;
  }

  if (NULL != pAsynInfo->pCB) {
    CALLBACK_Cancel((AEECallback*)pAsynInfo->pCB);
  }
  FREE(pAsynInfo);
  OEMCARDSESSION_LEAVE_CRITICAL_SECTION

  /* The rest of the clean up is being taken care by OEMCardSession_Cancel */
} /* OEMCardSession_CleanUpAsynObject */


/*===========================================================================

FUNCTION: OEMCardSession_Cancel

DESCRIPTION
   Cancels the user callback

DEPENDENCIES
  none

RETURN VALUE
  none

SIDE EFFECTS
===========================================================================*/
static void OEMCardSession_Cancel(
/*lint -save -esym(818,pcb) suppress could be const */
  AEECallback *pcb)
{
  OEMCardSessionAsynCBInfo *pAsyncInfo = NULL;
  AEEObjectHandle     Handler;

  if (NULL != pcb) {
    OEMCARDSESSION_ENTER_CRITICAL_SECTION
    Handler= (AEEObjectHandle)pcb->pCancelData;
    pAsyncInfo = (OEMCardSessionAsynCBInfo*)AEEObjectMgr_GetObject(Handler);

    if (NULL != pAsyncInfo) {
      (void)AEEObjectMgr_Unregister(pAsyncInfo->Handler);
      if (NULL != pAsyncInfo->pCB) {
        AEECallback *pAsyncCB = (AEECallback*)(pAsyncInfo->pCB);
        pAsyncCB->pCancelData = NULL;
      }
      FREE(pAsyncInfo);
      pAsyncInfo = NULL; /* setting pcb->pCB->pfnCancel = NULL; */
    }
    pcb->pfnCancel = NULL;
    OEMCARDSESSION_LEAVE_CRITICAL_SECTION
  }
}


/*===========================================================================

FUNCTION: OEMCardSession_GetLinkSysCallBack

DESCRIPTION
  Performs the AEE_LinkSysCallback operation.  Set the user passed in data
  in the reserve field

DEPENDENCIES
  none

RETURN VALUE
  none

SIDE EFFECTS
===========================================================================*/
int OEMCardSession_GetLinkSysCallBack(
  AEECallback     *pUserCB,
  void            *pCmdStatus,
  AEEObjectHandle *pHandler)
{
  OEMCardSessionAsynCBInfo  *pAsynInfo = NULL;
  int                  nRet;

  if (NULL == pHandler) {
    return EFAILED;
  }

  OEMCARDSESSION_ENTER_CRITICAL_SECTION
  pAsynInfo = (OEMCardSessionAsynCBInfo*)MALLOC(
                sizeof(OEMCardSessionAsynCBInfo));
  if (NULL == pAsynInfo) {
    OEMCARDSESSION_LEAVE_CRITICAL_SECTION
    return ENOMEMORY;
  }

  pAsynInfo->pCB = (AEECallback*)AEE_LinkSysCallback(pUserCB);
  if (pAsynInfo->pCB != NULL) {
    pAsynInfo->pClientData = pCmdStatus;
    pAsynInfo->pCB->pfnCancel = (PFNCBCANCEL)OEMCardSession_Cancel;
    pAsynInfo->Handler = 0;

    nRet = AEEObjectMgr_Register(pAsynInfo, &pAsynInfo->Handler);
    if (nRet != SUCCESS) {
      CALLBACK_Cancel(pAsynInfo->pCB);
      FREE(pAsynInfo);
      pAsynInfo = NULL;
      OEMCARDSESSION_LEAVE_CRITICAL_SECTION
      return nRet;
    }
    pAsynInfo->pCB->pCancelData = (void*)pAsynInfo->Handler;
  }
  else {
    FREE(pAsynInfo);
    pAsynInfo = NULL;
    OEMCARDSESSION_LEAVE_CRITICAL_SECTION
    return EFAILED;
  }
  *pHandler = pAsynInfo->Handler;
   OEMCARDSESSION_LEAVE_CRITICAL_SECTION
   return SUCCESS;
} /* OEMCard_GetLinkSysCallBack */


#ifdef FEATURE_UIM_JCDMA_RUIM_SUPPORT
/*===========================================================================
FUNCTION: OEMCardSession_MapFromMMGSDIJCDMACardInfo

DESCRIPTION
  Map MMGSDI JCDMA Card Info to AEECardSession JCDMA Card Info

DEPENDENCIES
  none

RETURN VALUE
  EBADPARM: bad input paramter
  SUCCESS:  successful mapping from mmgsdi value to icardesession card value
  EBADITEM: invalid card type returned by mmgsdi

SIDE EFFECTS
===========================================================================*/
int OEMCardSession_MapFromMMGSDIJCDMACardInfo(
  mmgsdi_jcdma_card_info_enum_type nMMGSDIJCDMACardInfoEnum,
  uint32                           *pJCDMACardInfoEnum)
{
  MSG_HIGH("OEMCardSession_MapFromMMGSDIJCDMACardInfo MMGSDI JCDMA Card Info 0x%x",
           nMMGSDIJCDMACardInfoEnum, 0, 0);

  if (NULL == pJCDMACardInfoEnum) {
    MSG_ERROR("Null Input Parm", 0, 0, 0);
    return EBADPARM;
  }

  switch (nMMGSDIJCDMACardInfoEnum)
  {
    case MMGSDI_JCDMA_NON_JCDMA_CARD:
      MSG_HIGH("AEECARDSESSION_JCDMA_NON_JCDMA_CARD", 0, 0, 0);
      *pJCDMACardInfoEnum = AEECARDSESSION_JCDMA_NON_JCDMA_CARD;
      break;
    case MMGSDI_JCDMA_WHITE_CARD:
      MSG_HIGH("AEECARDSESSION_JCDMA_WHITE_CARD", 0, 0, 0);
      *pJCDMACardInfoEnum = AEECARDSESSION_JCDMA_WHITE_CARD;
      break;
    case MMGSDI_JCDMA_GRAY_CARD:
      MSG_HIGH("AEECARDSESSION_JCDMA_GRAY_CARD", 0, 0, 0);
      *pJCDMACardInfoEnum = AEECARDSESSION_JCDMA_GRAY_CARD;
      break;
    case MMGSDI_JCDMA_GRAY_CARD_LINE_LOCKED:
      MSG_HIGH("AEECARDSESSION_JCDMA_GRAY_CARD_LINE_LOCKED", 0, 0, 0);
      *pJCDMACardInfoEnum = AEECARDSESSION_JCDMA_GRAY_CARD_LINE_LOCKED;
      break;
    case MMGSDI_JCDMA_BLACK_CARD:
      MSG_HIGH("AEECARDSESSION_JCDMA_BLACK_CARD", 0, 0, 0);
      *pJCDMACardInfoEnum = AEECARDSESSION_JCDMA_BLACK_CARD;
      break;
    case MMGSDI_JCDMA_BLACK_CARD_NEW_CARD:
      MSG_HIGH("AEECARDSESSION_JCDMA_BLACK_CARD_NEW_CARD", 0, 0, 0);
      *pJCDMACardInfoEnum = AEECARDSESSION_JCDMA_BLACK_CARD_NEW_CARD;
      break;
    case MMGSDI_JCDMA_BLACK_CARD_LINE_LOCKED:
      MSG_HIGH("AEECARDSESSION_JCDMA_BLACK_CARD_LINE_LOCKED", 0, 0, 0);
      *pJCDMACardInfoEnum = AEECARDSESSION_JCDMA_BLACK_CARD_LINE_LOCKED;
      break;
    case MMGSDI_JCDMA_BLACK_CARD_NW_LOCKED:
      MSG_HIGH("AEECARDSESSION_JCDMA_BLACK_CARD_NW_LOCKED", 0, 0, 0);
      *pJCDMACardInfoEnum = AEECARDSESSION_JCDMA_BLACK_CARD_NW_LOCKED;
      break;   
    case MMGSDI_JCDMA_BLACK_CARD_SP_LOCKED:
      MSG_HIGH("AEECARDSESSION_JCDMA_BLACK_CARD_SP_LOCKED", 0, 0, 0);
      *pJCDMACardInfoEnum = AEECARDSESSION_JCDMA_BLACK_CARD_SP_LOCKED;
      break;
    case MMGSDI_JCDMA_BLACK_CARD_UG_LOCKED:
      MSG_HIGH("AEECARDSESSION_JCDMA_BLACK_CARD_UG_LOCKED", 0, 0, 0);
      *pJCDMACardInfoEnum = AEECARDSESSION_JCDMA_BLACK_CARD_UG_LOCKED;
      break;
    case MMGSDI_JCDMA_UNKNOWN_CARD:  
      MSG_HIGH("AEECARDSESSION_JCDMA_UNKNOWN_CARD", 0, 0, 0);
      *pJCDMACardInfoEnum = AEECARDSESSION_JCDMA_UNKNOWN_CARD;
      break;
    default:
      MSG_ERROR("Invalid Card Type",0,0,0);
      return EBADITEM;    
  }
  return SUCCESS;
}/* OEMCardSession_MapFromMMGSDIJCDMACardInfo */


/*===========================================================================
FUNCTION: OEMCardSession_MapFromMMGSDIESNMEChangeType

DESCRIPTION
  Map MMGSDI ESN ME Change Type to AEECardSession ME Change ESN values

DEPENDENCIES
  none

RETURN VALUE
  EBADPARM: bad input paramter
  SUCCESS:  successful mapping from mmgsdi value to icardesession card value
  EBADITEM: invalid card type returned by mmgsdi

SIDE EFFECTS
===========================================================================*/
int OEMCardSession_MapFromMMGSDIESNMEChange(
  mmgsdi_esn_me_change_enum_type   nMMGSDIESNMEChangeEnum,
  uint32                           *pESNMEChangeEnum)
{
  MSG_HIGH("OEMCardSession_MapFromMMGSDIESNMEChange MMGSDI ESN ME Change 0x%x",
           nMMGSDIESNMEChangeEnum, 0, 0);

  if (NULL == pESNMEChangeEnum) {
    MSG_ERROR("Null Input Parm", 0, 0, 0);
    return EBADPARM;
  }

  switch (nMMGSDIESNMEChangeEnum)
  {
    case MMGSDI_ESN_ME_NO_CHANGE:
      MSG_HIGH("AEECARDSESSION_ESN_ME_NO_CHANGE", 0, 0, 0);
      *pESNMEChangeEnum = AEECARDSESSION_ESN_ME_NO_CHANGE;
      break;
    case MMGSDI_ESN_ME_CHANGE:
      MSG_HIGH("AEECARDSESSION_ESN_ME_CHANGE", 0, 0, 0);
      *pESNMEChangeEnum = AEECARDSESSION_ESN_ME_CHANGE;
      break;
    case MMGSDI_ESN_ME_CHANGE_NOT_SUPPORTED:
      MSG_HIGH("AEECARDSESSION_ESN_ME_CHANGE_NOT_SUPPORTED", 0, 0, 0);
      *pESNMEChangeEnum = AEECARDSESSION_ESN_ME_CHANGE_NOT_SUPPORTED;
      break;
    default:
      MSG_ERROR("Invalid ESN ME Change Value",0,0,0);
      return EBADITEM;	  
  }
  return SUCCESS;
}/* OEMCardSession_MapFromMMGSDIESNMEChange */
#endif /* FEATURE_UIM_JCDMA_RUIM_SUPPORT */


#ifdef FEATURE_MMGSDI_MFLO
#error code not present
#endif /* FEATURE_MMGSDI_MFLO */
#endif /* FEATURE_MMGSDI */

/*=============================================================================
FUNCTION: OEMCardSession_RegisterForRefresh

DESCRIPTION:  
    This API will allow the ICardSession Client to Register for File
    Change notifications and  Vote for Init for the current session
      
PROTOTYPE:
    int OEMCardSession_RegisterForRefresh(
                              ICardSession                 *p,
                              AEECardSessionCmdStatus      *pCmdStatus,
                              AEECardSessionRefreshData     sRefreshData,
                              boolean                       voteForInit,
                              AEECardSessionOption         *pOption,
                              AEECallback                  *pUserCB)

PARAMETERS:
    *p           : [Input] ICardSession interface pointer
    *pCmdStatus  : [Input] Command status
     sRefreshData: [Input] Refresh Files that the client is interested in
     voteForInit : [Input] Register for Client vote for init
    *pOption     : [Input] Optional Parameter pointer (currently not in use)
    *pUserCB     : [Input/Output] App Callback   for this command only.       

RETURN VALUE:
  SUCCESS    - Status information is valid
  EBADPARM   - Bad parameter
  EUNSUPPORT - Not supported

COMMENTS:
  Assuming the client had registered for vote for init with MMGSDI earlier 
  through this API, if the client calls this API again with vote for init 
  flag set to zero, MMGSDI will deregister the client from vote for init.

SIDE EFFECTS:
  Voting request will be sent to the client at the time of Refresh. 
  Client will be required to call ICardSession_OkToRefresh
  seperately as response for each of the events Vote for Init and 
  Vote for FCN that it may receive.

SEE ALSO:
  None
=============================================================================*/
/*lint -save -esym(818,pOption) suppress could be const */
/*lint -e{715} suppress "pOption not referenced" */
int OEMCardSession_RegisterForRefresh (
   ICardSession                    *pMe,
   AEECardSessionCmdStatus         *pCmdStatus,
   AEECardSessionRefreshData        sRefreshData,
   boolean                          vote_for_init,
   AEECardSessionOption            *pOption,
   AEECallback                     *pUserCB)
{
#ifdef FEATURE_MMGSDI
  mmgsdi_return_enum_type            mmgsdi_status = MMGSDI_ERROR;
  OEMCardSessionDataType             *pClientData  = NULL;
  int                                nReturnStatus = SUCCESS;
  mmgsdi_refresh_file_list_type      refresh_files ;
  mmgsdi_option_type                 mmgsdi_option;
  uint32                             i             =  0;
  mmgsdi_slot_id_enum_type           mmgsdi_slot   = MMGSDI_SLOT_1;   

  MSG_HIGH("OEMCardSession_RegisterForRefresh", 0, 0, 0);

  /* Parameter check */
  if (NULL == pMe || 
      NULL == pUserCB) {
    MSG_HIGH("NULL Pointer to IcardSession passed in", 0, 0, 0);
    return EBADPARM;
  }
  if((0 < sRefreshData.nNumOfFiles) && 
     (NULL == sRefreshData.pFileList)) {
    MSG_ERROR("Register FCN file ptr is NULL", 0, 0, 0);
    return EBADPARM;
  }

  /* Intialize structures */
  (void)MEMSET(&refresh_files, 0, sizeof(mmgsdi_refresh_file_list_type));
  (void)MEMSET(&mmgsdi_option, 0, sizeof(mmgsdi_option_type));

  /* Client Reference */
  pClientData = (OEMCardSessionDataType*)MALLOC(sizeof(OEMCardSessionDataType));
  if (NULL == pClientData) {
    MSG_ERROR("Unable to Alloc pClientData", 0, 0, 0);
    return ENOMEMORY;
  }

  /* Get and populate the callback */
  nReturnStatus = OEMCardSession_GetLinkSysCallBack(
                    pUserCB,
                    pCmdStatus,
                    &((pClientData)->ObjHandler));
  if (SUCCESS != nReturnStatus) {
    FREE( pClientData );
    return nReturnStatus;
  }

  pClientData->m_pCardSession = pMe;

  refresh_files.num_files = sRefreshData.nNumOfFiles;

  if( sRefreshData.nNumOfFiles > 0) {
    /* Allocate memory for the Refresh Files*/
    refresh_files.file_list_ptr = 
        (mmgsdi_file_enum_type*)MALLOC(sizeof(mmgsdi_file_enum_type)*sRefreshData.nNumOfFiles);
    if (NULL == refresh_files.file_list_ptr) {
      MSG_ERROR("Unable to Alloc file_list_ptr", 0, 0, 0);
      OEMCardSession_CleanUpAsynObject(pClientData->ObjHandler);
      FREE( pClientData );
      return ENOMEMORY;
    }

    for(i=0;i<sRefreshData.nNumOfFiles;i++)
    {
      nReturnStatus = OEMCardSession_MapFileEnum(&(refresh_files.file_list_ptr[i]), sRefreshData.pFileList[i]);
      if (SUCCESS != nReturnStatus) {
        MSG_ERROR("Unable to Copy FCN file_list_ptr", 0, 0, 0);
        OEMCardSession_CleanUpAsynObject(pClientData->ObjHandler);
        FREE( pClientData );
        FREE( refresh_files.file_list_ptr );
        return nReturnStatus;
      }
    }
  }

  /* convert the slot */
  nReturnStatus = OEMCardSession_MapMMGSDISlot(pMe->m_u8Slot,&mmgsdi_slot);
  if (SUCCESS != nReturnStatus) {
    OEMCardSession_CleanUpAsynObject(pClientData->ObjHandler);
    FREE( pClientData );
    FREEIF( refresh_files.file_list_ptr );
    return nReturnStatus;
  }


  mmgsdi_status = mmgsdi_register_for_refresh (pMe->m_client_id,
                                               mmgsdi_slot,
                                               refresh_files,
                                               vote_for_init,
                                               mmgsdi_option,
                                               OEMCardSessionCmdCb,
                                               (mmgsdi_client_data_type)pClientData);

  if (MMGSDI_SUCCESS != mmgsdi_status) {
    OEMCardSession_CleanUpAsynObject(pClientData->ObjHandler);
    FREE( pClientData );
    FREEIF( refresh_files.file_list_ptr );
    MSG_ERROR("Unable to Register for Refresh 0x%x", mmgsdi_status, 0, 0);
    return EFAILED;
  }
  return SUCCESS;
#else
  return EUNSUPPORTED;
#endif /*FEATURE_MMGSDI*/
}/* OEMCardSession_RegisterForRefresh */
/*lint -save +esym(818,pOption) suppress could be const */


/*=============================================================================
FUNCTION: OEMCardSession_DeRegisterForRefresh

DESCRIPTION:  
  This API will allow the ICardSession Client to Deregister for File
  Change notifications and Vote for Init for the current session
      
PROTOTYPE:
    int OEMCardSession_DeRegisterForRefresh(
                              ICardSession                 *p,
                              AEECardSessionCmdStatus      *pCmdStatus,
                              AEECardSessionRefreshData    sRefreshData,
                              boolean                      voteForInit,
                              AEECardSessionOption         *pOption,
                              AEECallback                  *pUserCB)

PARAMETERS:
    *p           : [Input] ICardSession interface pointer
    *pCmdStatus  : [Input] command status
     sRefreshData: [Input] DeRefresh Files that the client 
                           registered for FCN earlier
     voteForInit : [Input] Deregister for Client vote for init
    *pOption     : [Input] Optional Parameter pointer (currently not in use)
    *pUserCB     : [Input/Output] App Callback          

RETURN VALUE:
  SUCCESS    - Status information is valid
  EBADPARM   - Bad parameter
  EUNSUPPORT - Not supported

COMMENTS:
  If the session is closed or the ICardSession instance is destroyed, then 
  IcardSession will internally deregister FCN with MMGSDI.

SIDE EFFECTS:
  None

SEE ALSO:
  None
=============================================================================*/
/*lint -save -esym(818,pOption) suppress could be const */
/*lint -e{715} suppress "pOption not referenced" */
int OEMCardSession_DeRegisterForRefresh (
   ICardSession                    *pMe,
   AEECardSessionCmdStatus         *pCmdStatus,
   AEECardSessionRefreshData        sRefreshData,
   boolean                          vote_for_init,
   AEECardSessionOption            *pOption,
   AEECallback                     *pUserCB)
{
#ifdef FEATURE_MMGSDI
  mmgsdi_return_enum_type            mmgsdi_status = MMGSDI_ERROR;
  OEMCardSessionDataType             *pClientData  = NULL;
  int                                nReturnStatus = SUCCESS;
  uint32                             i             = 0;
  mmgsdi_slot_id_enum_type           mmgsdi_slot   = MMGSDI_SLOT_1; 
  mmgsdi_refresh_file_list_type      refresh_files;
  mmgsdi_option_type                 mmgsdi_option;

  MSG_HIGH("OEMCardSession_DeRegisterForRefresh", 0, 0, 0);

  /* Parameter check */
  if (NULL == pMe || 
      NULL == pUserCB) {
    MSG_HIGH("NULL Pointer to IcardSession passed in", 0, 0, 0);
    return EBADPARM;
  }
  if((0 < sRefreshData.nNumOfFiles) && 
     (NULL == sRefreshData.pFileList)) {
    MSG_HIGH("Register FCN file ptr is NULL", 0, 0, 0);
    return EBADPARM;
  }

 /* Intialize structures */
  (void)MEMSET(&refresh_files, 0, sizeof(mmgsdi_refresh_file_list_type));
  (void)MEMSET(&mmgsdi_option, 0, sizeof(mmgsdi_option_type));

  /* Client Reference */
  pClientData = (OEMCardSessionDataType*)MALLOC(sizeof(OEMCardSessionDataType));
  if (NULL == pClientData) {
    MSG_ERROR("Unable to Alloc pClientData", 0, 0, 0);
    return ENOMEMORY;
  }

  /* Get and populate the callback */
  nReturnStatus = OEMCardSession_GetLinkSysCallBack(
                    pUserCB,
                    pCmdStatus,
                    &((pClientData)->ObjHandler));
  if (SUCCESS != nReturnStatus) {
    FREE( pClientData );
    return nReturnStatus;
  }

  pClientData->m_pCardSession = pMe;

  if(0 < sRefreshData.nNumOfFiles) {
    /* Allocate memory for the Refresh Files*/
    refresh_files.file_list_ptr = (mmgsdi_file_enum_type*)MALLOC(sizeof(uint32)*sRefreshData.nNumOfFiles);
    if (NULL == refresh_files.file_list_ptr) {
      MSG_ERROR("Unable to Alloc file_list_ptr", 0, 0, 0);
      OEMCardSession_CleanUpAsynObject(pClientData->ObjHandler);
      FREE( pClientData );
      return ENOMEMORY;
    }
    /* Copy the Refresh Files into mmgsdi structure from client structure*/
    for(i=0;i<sRefreshData.nNumOfFiles;i++)
    {
      nReturnStatus = OEMCardSession_MapFileEnum(&(refresh_files.file_list_ptr[i]), sRefreshData.pFileList[i]);
      if (SUCCESS != nReturnStatus) {
        MSG_ERROR("Unable to Copy FCN file_list_ptr", 0, 0, 0);
        OEMCardSession_CleanUpAsynObject(pClientData->ObjHandler);
        FREE( pClientData );
        FREE( refresh_files.file_list_ptr );
        return nReturnStatus;
      }
    }
  }

  /* convert the slot */
  nReturnStatus = OEMCardSession_MapMMGSDISlot(pMe->m_u8Slot,&mmgsdi_slot);
  if (SUCCESS != nReturnStatus) {
    OEMCardSession_CleanUpAsynObject(pClientData->ObjHandler);
    FREE(pClientData);
    FREEIF(refresh_files.file_list_ptr);
    return nReturnStatus;
  }

  mmgsdi_status = mmgsdi_deregister_for_refresh(pMe->m_client_id,
                                                mmgsdi_slot,
                                                refresh_files,
                                                vote_for_init,
                                                mmgsdi_option,
                                                OEMCardSessionCmdCb,
                                                (mmgsdi_client_data_type)pClientData);

  if (MMGSDI_SUCCESS != mmgsdi_status) {
    OEMCardSession_CleanUpAsynObject(pClientData->ObjHandler);
    FREE( pClientData );
    FREEIF(refresh_files.file_list_ptr);
    MSG_ERROR("Unable to deregister for Refresh 0x%x", mmgsdi_status, 0, 0);
    return EFAILED;
  }
  return SUCCESS;
#else
  return EUNSUPPORTED;
#endif /*FEATURE_MMGSDI*/
}/* OEMCardSession_DeRegisterForRefresh */
/*lint -save +esym(818,pOption) suppress could be const */


/*=============================================================================
FUNCTION: OEMCardSession_RefreshComplete

DESCRIPTION:  
  This API will allow the ICardSession Client to send Refresh Complete status
  to MMGSDI. 
      
PROTOTYPE:
    int OEMCardSession_RefreshComplete(
                               ICardSession                 *p,
                               AEECardSessionCmdStatus      *pCmdStatus,
                               boolean                       dwStatus,
                               AEECardSessionOption         *pOption,
                               AEECallback                  *pUserCB)

PARAMETERS:
    *p           : [Input] ICardSession interface pointer
    *pCmdStatus  : [Input] Command Status
     dwStatus    : [Input] Status of the files that the client read OR
                           Status of the init procedure , client carried out.
    *pOption     : [Input] Optional Parameter pointer (currently not in use)
    *pUserCB     : [Input/Output] App Callback          

RETURN VALUE:
  SUCCESS    - Status information is valid
  EBADPARM   - Bad parameter
  EUNSUPPORT - Not supported

COMMENTS:
  None

SIDE EFFECTS:
  None

SEE ALSO:
  None
=============================================================================*/
/*lint -save -esym(818,pOption) suppress could be const */
/*lint -e{715} suppress "pOption not referenced" */
int OEMCardSession_RefreshComplete (
   ICardSession                    *pMe,
   AEECardSessionCmdStatus         *pCmdStatus,
   boolean                          dwStatus,
   AEECardSessionOption            *pOption,
   AEECallback                     *pUserCB)
{
#ifdef FEATURE_MMGSDI
  mmgsdi_return_enum_type            mmgsdi_status = MMGSDI_ERROR;
  OEMCardSessionDataType             *pClientData  = NULL;
  int                                nReturnStatus = SUCCESS;
  mmgsdi_slot_id_enum_type           mmgsdi_slot   = MMGSDI_SLOT_1; 
  mmgsdi_option_type                 mmgsdi_option;

  MSG_HIGH("OEMCardSession_RefreshComplete", 0, 0, 0);

  /* Parameter check */
  if (NULL == pMe || 
      NULL == pUserCB) {
    MSG_HIGH("NULL Pointer to IcardSession passed in", 0, 0, 0);
    return EBADPARM;
  }

  (void)MEMSET(&mmgsdi_option, 0, sizeof(mmgsdi_option_type));

  /* Client Reference */
  pClientData = (OEMCardSessionDataType*)MALLOC(sizeof(OEMCardSessionDataType));
  if (NULL == pClientData) {
    MSG_ERROR("Unable to Alloc pClientData", 0, 0, 0);
    return ENOMEMORY;
  }

  /* Get and populate the callback */
  nReturnStatus = OEMCardSession_GetLinkSysCallBack(
                    pUserCB,
                    pCmdStatus,
                    &((pClientData)->ObjHandler));
  if (SUCCESS != nReturnStatus) {
    FREE( pClientData );
    return nReturnStatus;
  }

  pClientData->m_pCardSession = pMe;

  MSG_HIGH("Result of Client Re-read of files after Refresh 0x%x", dwStatus,0,0);

  /* convert the slot */
  nReturnStatus = OEMCardSession_MapMMGSDISlot(pMe->m_u8Slot,&mmgsdi_slot);
  if (SUCCESS != nReturnStatus) {
    OEMCardSession_CleanUpAsynObject(pClientData->ObjHandler);
    FREE(pClientData);
    return nReturnStatus;
  }

  mmgsdi_status = mmgsdi_refresh_complete(pMe->m_client_id,
                                          mmgsdi_slot,
                                          dwStatus,
                                          mmgsdi_option,
                                          OEMCardSessionCmdCb,
                                          (mmgsdi_client_data_type)pClientData);

  if (MMGSDI_SUCCESS != mmgsdi_status) {
    OEMCardSession_CleanUpAsynObject(pClientData->ObjHandler);
    FREE( pClientData );
    MSG_ERROR("Unable to send Refresh Complete 0x%x", mmgsdi_status, 0, 0);
    return EFAILED;
  }
  return SUCCESS;
#else
  return EUNSUPPORTED;
#endif /*FEATURE_MMGSDI*/
}/* OEMCardSession_RefreshComplete */
/*lint -save +esym(818,pOption) suppress could be const */


/*=============================================================================
FUNCTION: OEMCardSession_OkToRefresh

DESCRIPTION:  
  With this API the client is REQUIRED to inform ICardSession whenever a 
  refresh event is received with AEECARDSESSION_MODEL_REFRESH_STAGE_OK_TO_INIT or 
  AEECARDSESSION_MODEL_REFRESH_STAGE_OK_TO_FCN. If the client did not register for 
  voting, then this API need not be called.
      
PROTOTYPE:
    int OEMCardSession_OkToRefresh(
                                  ICardSession                 *p,
                                  AEECardSessionCmdStatus      *pCmdStatus,
                                  boolean                       okToRefresh,
                                  AEECardSessionOption         *pOption,
                                  AEECallback                  *pUserCB)

PARAMETERS:
    *p           : [Input] ICardSession interface pointer
    *pCmdStatus  : [Input] Command Status
     okToRefresh : [Input] Is it ok to bring down call stack or terminate app
    *pOption     : [Input] Optional parameter pointer (currently not in use)
    *pUserCB     : [Input/Output] App Callback          

RETURN VALUE:
  SUCCESS    - Status information is valid
  EBADPARM   - Bad parameter
  EUNSUPPORT - Not supported

COMMENTS:
  None

SIDE EFFECTS:
  None

SEE ALSO:
  None
=============================================================================*/
/*lint -save -esym(818,pOption) suppress could be const */
/*lint -e{715} suppress "pOption not referenced" */
int OEMCardSession_OkToRefresh (
   ICardSession             *pMe,
   AEECardSessionCmdStatus  *pCmdStatus,
   boolean                   okToRefresh,
   AEECardSessionOption     *pOption,
   AEECallback              *pUserCB)
{
#ifdef FEATURE_MMGSDI
  mmgsdi_return_enum_type            mmgsdi_status = MMGSDI_ERROR;
  OEMCardSessionDataType             *pClientData  = NULL;
  int                                nReturnStatus = SUCCESS;
  mmgsdi_slot_id_enum_type           mmgsdi_slot   = MMGSDI_SLOT_1; 
  mmgsdi_option_type                 mmgsdi_option;

  MSG_HIGH("OEMCardSession_OkToRefresh", 0, 0, 0);

  /* Parameter check */
  if (NULL == pMe || 
      NULL == pUserCB) {
    MSG_HIGH("NULL Pointer to IcardSession passed in", 0, 0, 0);
    return EBADPARM;
  }

  (void)MEMSET(&mmgsdi_option, 0, sizeof(mmgsdi_option_type));

  /* Client Reference */
  pClientData = (OEMCardSessionDataType*)MALLOC(sizeof(OEMCardSessionDataType));
  if (NULL == pClientData) {
    MSG_ERROR("Unable to Alloc pClientData", 0, 0, 0);
    return ENOMEMORY;
  }

  /* Get and populate the callback */
  nReturnStatus = OEMCardSession_GetLinkSysCallBack(
                    pUserCB,
                    pCmdStatus,
                    &((pClientData)->ObjHandler));
  if (SUCCESS != nReturnStatus) {
    FREE( pClientData );
    return nReturnStatus;
  }

  pClientData->m_pCardSession = pMe;

   /* convert the slot */
  nReturnStatus = OEMCardSession_MapMMGSDISlot(pMe->m_u8Slot,&mmgsdi_slot);
  if (SUCCESS != nReturnStatus) {
    OEMCardSession_CleanUpAsynObject(pClientData->ObjHandler);
    FREE(pClientData);
    return nReturnStatus;
  }

  MSG_HIGH("client OK to Refresh value 0x%x", okToRefresh,0,0);

  mmgsdi_status = mmgsdi_ok_to_refresh (pMe->m_client_id,
                                        mmgsdi_slot,
                                        okToRefresh,
                                        mmgsdi_option,
                                        OEMCardSessionCmdCb, 
                                        (mmgsdi_client_data_type)pClientData);

  if (MMGSDI_SUCCESS != mmgsdi_status) {
    OEMCardSession_CleanUpAsynObject(pClientData->ObjHandler);
    FREE( pClientData );
    MSG_ERROR("Unable to send OK to refresh 0x%x", mmgsdi_status, 0, 0);
    return EFAILED;
  }

  return SUCCESS;
#else
  return EUNSUPPORTED;
#endif /*FEATURE_MMGSDI*/
}/* OEMCardSession_OkToRefresh */
/*lint -save +esym(818,pOption) suppress could be const */

/*lint -save +esym(818,pCmdStatus,pUserCB) suppress could be const */


/*=============================================================================
FUNCTION: OEMCardSession_CreatePkcs15LookupTable

DESCRIPTION:  
  This API will allow ICARDSESSION client to trigger MMGSDI to create a PKCS15 
  lookup table.
      
PROTOTYPE:
    int OEMCardSession_CreatePkcs15LookupTable(
                                  ICardSession                 *p,
                                  AEECardSessionCmdStatus      *pCmdStatus,
                                  AEECardSessionOption         *pOption,
                                  AEECallback                  *pUserCB)

PARAMETERS:
    *p           : [Input] ICardSession interface pointer
    *pCmdStatus  : [Input] Command Status
    *pOption     : [Input] Optional parameter pointer (currently not in use)
    *pUserCB     : [Input/Output] App Callback          

RETURN VALUE:
  SUCCESS    - Status information is valid
  EBADPARM   - Bad parameter
  EUNSUPPORT - Not supported

COMMENTS:
  None

SIDE EFFECTS:
  None

SEE ALSO:
  None
=============================================================================*/
/*lint -save -esym(818,pOption) suppress could be const */
/*lint -e{715} suppress "pOption not referenced" */
int OEMCardSession_CreatePkcs15LookupTable (
   ICardSession             *pMe,
   AEECardSessionCmdStatus  *pCmdStatus,
   AEECardSessionOption     *pOption,
   AEECallback              *pUserCB)
{
#ifdef FEATURE_MMGSDI
  mmgsdi_return_enum_type            mmgsdi_status = MMGSDI_ERROR;
  OEMCardSessionDataType             *pClientData  = NULL;
  int                                nReturnStatus = SUCCESS;
  mmgsdi_slot_id_enum_type           mmgsdi_slot   = MMGSDI_SLOT_1; 
  mmgsdi_option_type                 mmgsdi_option;

  MSG_HIGH("OEMCardSession_CreatePkcs15lookupTable", 0, 0, 0);

  /* Parameter check */
  if (NULL == pMe || 
      NULL == pUserCB) {
    MSG_HIGH("NULL Pointer to IcardSession passed in", 0, 0, 0);
    return EBADPARM;
  }

  (void)MEMSET(&mmgsdi_option, 0, sizeof(mmgsdi_option_type));

  /* Client Reference */
  pClientData = (OEMCardSessionDataType*)MALLOC(sizeof(OEMCardSessionDataType));
  if (NULL == pClientData) {
    MSG_ERROR("Unable to Alloc pClientData", 0, 0, 0);
    return ENOMEMORY;
  }

  /* Get and populate the callback */
  nReturnStatus = OEMCardSession_GetLinkSysCallBack(
                    pUserCB,
                    pCmdStatus,
                    &((pClientData)->ObjHandler));
  if (SUCCESS != nReturnStatus) {
    FREE( pClientData );
    return nReturnStatus;
  }

  pClientData->m_pCardSession = pMe;

   /* convert the slot */
  nReturnStatus = OEMCardSession_MapMMGSDISlot(pMe->m_u8Slot,&mmgsdi_slot);
  if (SUCCESS != nReturnStatus) {
    OEMCardSession_CleanUpAsynObject(pClientData->ObjHandler);
    FREE(pClientData);
    return nReturnStatus;
  }

  mmgsdi_status = mmgsdi_create_pkcs15_lookup_table (pMe->m_client_id,
                                        mmgsdi_slot,
                                        mmgsdi_option,
                                        OEMCardSessionCmdCb, 
                                        (mmgsdi_client_data_type)pClientData);

  if (MMGSDI_SUCCESS != mmgsdi_status) {
    OEMCardSession_CleanUpAsynObject(pClientData->ObjHandler);
    FREE( pClientData );
    MSG_ERROR("Unable to Invoke Create PKCS15 lookup table 0x%x", mmgsdi_status, 0, 0);
    return EFAILED;
  }

  return SUCCESS;
#else
  return EUNSUPPORTED;
#endif /*FEATURE_MMGSDI*/
}/* OEMCardSession_CreatePkcs15LookupTable */
/*lint -save +esym(818,pOption) suppress could be const */

/*lint -save +esym(818,pCmdStatus,pUserCB) suppress could be const */
