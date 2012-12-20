#ifndef MMGSDILIB_H
#define MMGSDILIB_H
/*===========================================================================


            M M G S D I   L I B R A R Y   D E F I N I T I O N S

                      A N D   F U N C T I O N S


  This file contains Library function that MMGSDI client can call to access
  various SIM/USIM/RUIM functions

  mmgsdi_read_transparent
    Read data from a Transparent file

  mmgsdi_read_record
    Read data from a record in a Linear Fixed or Cyclic file

  mmgsdi_write_transparent
    Write data into a Transparent file

  mmgsdi_write_record
    Write data into a record Linear Fixed or Cyclic file

  mmgsdi_get_file_attr
    Get File Attribute information for the files

  mmgsdi_cmd
    Getting commnad pointer and copy the data and put into the MMGSDI command
    queue

  mmgsdi_search
    Search for a given pattern in a record file on uicc card.

  mmgsdi_seek
   Search for a given pattern in a record file on icc card.
===========================================================================*/

/*===========================================================================
                        COPYRIGHT INFORMATION

Copyright (c) 2004 - 2010 QUALCOMM, Incorporated and its licensors.  All
Rights Reserved.  QUALCOMM Proprietary.  Export of this technology or software
is regulated by the U.S. Government. Diversion contrary to U.S. law prohibited.

                        EDIT HISTORY FOR MODULE

This section contains comments describing changes made to the module.
Notice that changes are listed in reverse chronological order.

$Header: //source/qcom/qct/modem/uim/su/baselines/qsc1110/rel/3.3.65/uim/mmgsdi/inc/mmgsdilib.h#2 $$ $DateTime: 2011/03/09 23:17:08 $ $Author: sratnu $

when       who     what, where, why
--------   ---     -----------------------------------------------------------
06/07/10   ssr     Add non 3gpd cdma card check
04/15/09   xz      Fix issue of generating RPC code for modem restart
03/19/09   js      Support for Enhanced NAA Refresh by file path
03/02/09   mib     Added original mode in mmgsdi_refresh_evt_info_type
01/21/09   kk      Added channel id to the session_open cnf data
12/09/08   nk      Fixed RPC Meta Comment for mmgsdi_client_id_reg
11/26/08   nb      Removed Featurization Flags for Common Modem Interface
10/29/08   tml     Modem restart client id dereg clean up
10/20/08   sun     Added new MMGSDI return type
10/06/08   js      Removed inclusion of tmc.h
08/15/08   jk      Added Generate Key / VPM API
08/26/08   js      Updated MMGSDI_MAX_PATH_LEN to 5 and made all uim commands
                   to accept path length of 4
07/31/08   SSR     Update search for blocking the search directions belonging
                   to Record Pointer.
07/25/08   tml     Added modem restart oncrpc support
07/21/08   tml     Added qw.h for dword support
07/17/08   kk      Added support for PKCS 15
06/23/08   ssr     Support of MMGSDI Search functionality
06/21/08   kk      Added PKCS15 Support
06/13/08   sun     Added support for OMA BCAST Srv bit
06/03/08   tml     Added sw1 and sw2 for access and status cnfs
04/30/08   kk      Added support for MMS EFs
03/28/08   sun     Added support for OMA Bcast
03/28/08   sk      Fixed warnings
03/20/08   nk      Added OMH EF Support
01/24/08   nk      Added change me esn flag to get jcdma card info
01/10/08   nk      Added Comment for mmgsdi_cave_esn_type
11/19/07   js      Radio Access Technology Balancing support
12/02/07   sun     Added meta comment
11/15/07   sun     Added support for NAA refresh
11/08/07   vs      Added OMH feature support
09/10/07   tml     Added MFLO support
08/15/07   jk      Changes to Support DATA Authentication MMGSDI API
08/14/07   sun     Added new Error codes and MUK_DEL Context
07/02/07   sun     Fixed USIM Auth Contexts
06/01/07   sun     Fixed featurization for SAP
05/22/07   nk      Added support for ESN ME change and removed featurization
                   for uniform rpc support
05/22/07   nk      Removed featurization for rpc
05/17/07   wli     Added support for JCDMA RUIM
05/02/07   sun     Added support for Service Available and USIM Auth
04/06/07   sun     Identify the provisioning app
03/07/07   sun     Added support for Disable with Replacement
02/09/07   sk      Added support for 1000 phonebook entries.
12/19/06   tml     Add support for path transparent and record access
11/21/06   sun     Added Meta Comments for Universal Pin Event
10/25/06   tml     Added pin replacement inital support
09/11/06   jk      Correction to RPC Metacomments
08/25/06   jar     Added support for FEATURE_MMGSDI_CARD_INFO
08/23/06   sun     Fixed Lint Errors
08/20/06   tml     Added App Reset and 3G Session Reset supports
08/16/06   jar     Added support for MMGSDI_TELECOM_ANRC and
                   MMGSDI_TELECOM_ANRC1.  Realigned enums to adjust groupings
                   10.
08/05/06   tml     Added ACL support
07/26/06   sun     Added support for Multi USIM App and MMGSDI PIN
07/20/06   sun     Fixed featurization
06/29/06   tml     RPC support for new functions
06/12/06   pv      Add MMGSDI_UIM_INTERNALLY_RESET return type and
                   MMGSDI_SESSION_CLOSE_EVT event.
05/23/06   tml     BDN supports
05/03/06   sp      Update mmgsdi pin state
04/15/06   tml     Update #define to enum for onchip support
04/13/06   jar     Added ONCHIP SIM SUPPORT
04/05/06   tml     Add Card Removed support, Add NO_FILE_ENUM
03/21/06   tml     added cfis and missing service table support and lint
02/16/06   sp      Support for EF_EHPLMN
01/29/06   tml     Moved mmgsdi_sim_refresh to mmgsdi.h
01/04/06   tml     Updated enum name for SAP and Card powerup and down
12/14/05   tml     MMGSDI Cleanup
12/05/05   sun     Added support for MMGSDI_SAP_CARD_READER_STATUS_REQ and
                   MMGSDI_SAP_RESET_REQ
11/28/05   tml     PNN and OPL support
11/22/05   jk      Changes to support multiprocessor RPC
11/03/05   tml     Fixed header
11/03/05   sun     Added support for refresh
10/26/05   pv      Change MMGSDI_READ_ENTIRE_FILE_SIZE to 0.  A zero when sent
                   down to UIM would result in reading the rest of the file.
10/25/05   tml     Added ISIM File supports
10/20/05   sun     Added support for BT Deregisteration
09/29/05   tml     Client Deregistration support
08/30/05   jar     Added support for Sending Status commands
08/28/05   tml     Compilation fix
08/27/05   tml     Compilation fix
08/26/05   sun     Added support for BT SAP
08/25/05   pv      Support API to power down and power up SIM/USIM Card
07/28/05   sst     Fixed compile errors when SAP feature is not defined
07/26/05   sst     Added MMGSDIBT support
07/12/05   tml     Session Management and ISIM support
2/17/05    tml     Initial Revision


=============================================================================*/

/*=============================================================================

                     INCLUDE FILES FOR MODULE

=============================================================================*/

#include "comdef.h"
#include "msg.h"
#include "err.h"
#include "rex.h"
#include "qw.h"

/*=============================================================================

                       DEFINES DECLARATIONS

=============================================================================*/
/* ----------------------------------------------------------------------------
   DEFINE:      MMGSDI_FILE_ID_SIZE_IN_BYTES

   DESCRIPTION: The File ID size (in bytes) as indicated in 31.102
                11.11/51.011. For example:  0x7F20 is usually split into
                2 bytes 0x7F 0x20.
  -----------------------------------------------------------------------------*/
#define MMGSDI_FILE_ID_SIZE         2

/* ----------------------------------------------------------------------------
   DEFINE:      MMGSDI_FILE_ID_SIZE_IN_BYTES

   DESCRIPTION: The MAX File Size is currently set to 512 Bytes.  Thus, when a
                Client requests the maximum number of bytes to be read, it is
                an indication that the data to be returned is capped at 512.
  -----------------------------------------------------------------------------*/
#define MMGSDI_READ_ENTIRE_FILE_SIZE     0

/* ----------------------------------------------------------------------------
   DEFINE:      MMGSDI_DF_TAG_SIZE_IN_BYTES

   DESCRIPTION: Indicates the size of the DF Tag used when parsing / packing
                the PATH provided into a TLV.
  -----------------------------------------------------------------------------*/
#define MMGSDI_DF_TAG_SIZE_IN_BYTES        1

/* ----------------------------------------------------------------------------
   DEFINE:      MMGSDI_DF_TAG_LEN_SIZE_IN_BYTES

   DESCRIPTION: Indicates the size of the DF Tag Len used when parsing / packing
                the PATH provided into a TLV.  This Len will indicate how many
                bytes are to follow for the "Value" (i.e. PATH Value).
  -----------------------------------------------------------------------------*/
#define MMGSDI_DF_TAG_LEN_SIZE_IN_BYTES    1

/* --------------------------------------------------------------------------
   DEFINE:      MMGSDI_PIN_MIN_LEN

   DESCRIPTION: The minimum PIN Length allowed for PIN Operations.
                The minimum length allowed is 4 bytes.
   -----------------------------------------------------------------------*/
#define MMGSDI_PIN_MIN_LEN 4

/* --------------------------------------------------------------------------
   DEFINE:      MMGSDI_PIN_MAX_LEN

   DESCRIPTION: The maximum PIN Length allowed for PIN Operations.
                The maximum length allowed is 8 bytes.
   -----------------------------------------------------------------------*/
#define MMGSDI_PIN_MAX_LEN 8

/* --------------------------------------------------------------------------
   DEFINE:      MMGSDI_MAX_APP_INFO

   DESCRIPTION: The maximum applications in a USIM card, including GSM/CDMA
                The maximum apps are  9.
   -----------------------------------------------------------------------*/
#define MMGSDI_MAX_APP_INFO 9

/* --------------------------------------------------------------------------
   DEFINE:      MMGSDI_MAX_AID_LEN

   DESCRIPTION: The maximum aid/label length for an app
   -----------------------------------------------------------------------*/
#define MMGSDI_MAX_AID_LEN 32

/* --------------------------------------------------------------------------
   DEFINE:      MMGSDI_MAX_PATH_LEN

   DESCRIPTION: The maximum path length for access
   -----------------------------------------------------------------------*/
#define MMGSDI_MAX_PATH_LEN 5

/* ----------------------------------------------------------------------------
   DEFINE:      MMGSDI_CDMA_SRV, MMGSDI_GSM_SRV,MMGSDI_USIM_SRV

   DESCRIPTION: Defines the Service Categories for Different Technologies
  ---------------------------------------------------------------------------*/
#define MMGSDI_CDMA_SRV  0x100
#define MMGSDI_GSM_SRV   0x200
#define MMGSDI_USIM_SRV  0x400

/* ----------------------------------------------------------------------------
   DEFINE:      MMGSDI_ACTIVATE_ONCHIP_SIM_CONFIG_MAX_SIZE

   DESCRIPTION: Defines the maximum value allowable to be used for configuring
                the ONCHIP SIM CACHE.
  ---------------------------------------------------------------------------*/
#define MMGSDI_ACTIVATE_ONCHIP_SIM_CONFIG_MAX_SIZE 512

/* -------------------------------------------------------------
   DEFINE:      MMGSDI_MFLO_SUBSCRIBER_ID_LEN

   DESCRIPTION: Length of the MediaFLO subscriber ID.  The len is
                specified in the "Qualcomm MediaFLO Authentication
                Manager Applet Functional Specification"
  ----------------------------------------------------------------*/
#define MMGSDI_MFLO_SUBSCRIBER_ID_LEN     0x08

/* -------------------------------------------------------------
   DEFINE:      MMGSDI_MFLO_PUBLIC_KEY_LEN

   DESCRIPTION: Length of the MediaFLO public key.  The len is
                specified in the "Qualcomm MediaFLO Authentication
                Manager Applet Functional Specification"
----------------------------------------------------------------*/
#define MMGSDI_MFLO_PUBLIC_KEY_LEN     0x83

/* -------------------------------------------------------------
   DEFINE:      MMGSDI_MFLO_ACT_PARAMS_LEN

   DESCRIPTION: Length of the MediaFLO activation parameters for
                the initialization procedure.  The len is
                specified in the "Qualcomm MediaFLO Authentication
                Manager Applet Functional Specification"
  ----------------------------------------------------------------*/
#define MMGSDI_MFLO_ACT_PARAMS_LEN     0x88

/* -------------------------------------------------------------
   DEFINE:      MMGSDI_KEY_SIZE

   DESCRIPTION: Length of the key size returned by the Generate
                key / VPM Command on RUIM
----------------------------------------------------------------*/
#define MMGSDI_KEY_SIZE 8

/*===========================================================================
   Enum:      MMGSDI_ONCHIP_SIM_STATE_ENUM_TYPE

   DESCRIPTION:
   MMGSDI_ONCHIP_SIM_NOT_VALID: Defines the value when the ONCHIP SIM is not initialized
                or not running.
   MMGSDI_ONCHIP_SIM_INIT: Defines the value when the ONCHIP SIM is initialized which
                means that the init and populating of the SIM Cache was
                completed.
  =============================================================================*/
typedef enum
{
  MMGSDI_ONCHIP_SIM_NOT_VALID      = 0x0000,
  MMGSDI_ONCHIP_SIM_INIT,
  MMGSDI_ONCHIP_SIM_STATE_MAX_ENUM = 0x7FFF
}mmgsdi_onchip_sim_state_enum_type;

/* ----------------------------------------------------------------------------
   DEFINE:      MMGSDI_MAX_HRPD_CHAP_CHALLENGE_LENGTH

   DESCRIPTION: Defineds the maximum HRPD CHAP CHALLENGE LENGTH
  ---------------------------------------------------------------------------*/
#define MMGSDI_MAX_HRPD_CHAP_CHALLENGE_LENGTH 254

/* ----------------------------------------------------------------------------
   DEFINE:      MMGSDI_3GPD_MAX_MIP_CHALLENGE_LENGTH

   DESCRIPTION: Defineds the maximum 3GPD MIP CHALLENGE LEN
  ---------------------------------------------------------------------------*/
#define MMGSDI_3GPD_MAX_MIP_CHALLENGE_LENGTH       238

/* ----------------------------------------------------------------------------
   DEFINE:      MMGSDI_MAX_3GPD_HASH_RRQ_DATA_LENGTH

   DESCRIPTION: Defineds the maximum 3GPD MIP HASH RRQ DATA LENGTH
  ---------------------------------------------------------------------------*/
#define MMGSDI_MAX_3GPD_HASH_RRQ_DATA_LENGTH   800

/* ----------------------------------------------------------------------------
   DEFINE:      MMGSDI_MAX_3GPD_MN_HA_REG_DATA_LENGTH

   DESCRIPTION: Defineds the maximum 3GPD MIP MN HA DATA LENGTH
  ---------------------------------------------------------------------------*/
#define MMGSDI_MAX_3GPD_MN_HA_REG_DATA_LENGTH  800

/* ----------------------------------------------------------------------------
   DEFINE:      MMGSDI_MAX_3GPD_CHAP_CHALLENGE_LENGTH

   DESCRIPTION: Defineds the maximum 3GPD SIP CHAP CHALLENGE LENGTH
  ---------------------------------------------------------------------------*/
#define MMGSDI_MAX_3GPD_CHAP_CHALLENGE_LENGTH  32

/*----------------------------------------------------------------------------
  DEFINE:      MMGSDI_MAX_SEARCH_RECORD_NUMBER

  DESCRIPTION: Defined the maximum Record number.
  -----------------------------------------------------------------------*/
#define MMGSDI_MAX_SEARCH_RECORD_NUMBER  0xFE

/* ------------------------------------------------------------------------
  DEFINE:      MMGSDI_MAX_ENHANCED_SEARCH_RECORD_OFFSET

  DESCRIPTION: Defined the maximum offset of a record.
  -----------------------------------------------------------------------*/
#define MMGSDI_MAX_ENHANCED_SEARCH_RECORD_OFFSET  0xFE

/* ------------------------------------------------------------------------
  DEFINE:      MMGSDI_MAX_ENHANCED_SEARCH_CHARACTER_VALUE

  DESCRIPTION: Defind the maximum character value .
  -----------------------------------------------------------------------*/
#define MMGSDI_MAX_ENHANCED_SEARCH_CHARACTER_VALUE  0xFF

/* ------------------------------------------------------------------------
  DEFINE:      MMGSDI_MAX_SEEK_PATTERN_LENGTH

  DESCRIPTION: Defined maximum pattern length
  -----------------------------------------------------------------------*/
#define MMGSDI_MAX_SEEK_PATTERN_LENGTH  0x10

/* ----------------------------------------------------------------------------
  DEFINE:      MMGSDI_MAX_PKCS15_TABLE_ENTRIES

  DESCRIPTION: Defined the maximum entries in each pkcs table
               This is not a spec defined value, defined a value little
               greater than to hold all possible PKCS15 EFs
  ---------------------------------------------------------------------------*/
#define MMGSDI_MAX_PKCS15_TABLE_ENTRIES        10

/* ----------------------------------------------------------------------------
  DEFINE:      MMGSDI_CDMA_IMSI_LEN

  DESCRIPTION: Defined the CDMS IMSI FILE SIZE
  ---------------------------------------------------------------------------*/
#define MMGSDI_CDMA_IMSI_LEN   10


/*=============================================================================

                   ENUMERATED DATA DECLARATIONS

=============================================================================*/

/*===========================================================================
   ENUM:      MMGSDI_CNF_ENUM_TYPE

   DESCRIPTION:
     Indication of what type of command confirmation
=============================================================================*/
typedef enum
{
  MMGSDI_CLIENT_ID_REG_CNF,
  MMGSDI_CLIENT_EVT_REG_CNF,
  MMGSDI_CLIENT_ID_DEREG_CNF,
  MMGSDI_CLIENT_EVT_DEREG_CNF,
  MMGSDI_READ_CNF,
  MMGSDI_WRITE_CNF,
  MMGSDI_GET_FILE_ATTR_CNF,
  MMGSDI_SESSION_OPEN_CNF,
  MMGSDI_SESSION_CLOSE_CNF,
  MMGSDI_ISIM_AUTH_CNF,

  MMGSDI_SAP_CONNECT_CNF,
  MMGSDI_SAP_DISCONNECT_CNF,
  MMGSDI_SAP_GET_ATR_CNF,
  MMGSDI_SAP_SEND_APDU_CNF,
  MMGSDI_SEND_APDU_CNF,
  MMGSDI_SAP_POWER_OFF_CNF,
  MMGSDI_SAP_CARD_READER_STATUS_CNF,
  MMGSDI_SAP_POWER_ON_CNF,
  MMGSDI_SAP_RESET_CNF,
  MMGSDI_CARD_PDOWN_CNF,

  MMGSDI_CARD_PUP_CNF,
  MMGSDI_SAP_INIT_CNF,
  MMGSDI_SAP_VERIFY_PIN_CNF,
  MMGSDI_SAP_DEREG_CNF,
  MMGSDI_CARD_STATUS_CNF,
  MMGSDI_REFRESH_CNF,
  MMGSDI_ACTIVATE_ONCHIP_SIM_CNF,
  MMGSDI_DISABLE_BDN_CNF,
  MMGSDI_ENABLE_BDN_CNF,
  MMGSDI_REHABILITATE_CNF,

  MMGSDI_INVALIDATE_CNF,
  MMGSDI_PIN_OPERATION_CNF,
  MMGSDI_SELECT_AID_CNF,
  MMGSDI_GET_ALL_PIN_STATUS_CNF,
  MMGSDI_ACL_OPERATION_CNF,
  MMGSDI_SESSION_DEACTIVATE_CNF,
  MMGSDI_SESSION_SELECT_AID_CNF,
  MMGSDI_SRV_AVAILABLE_CNF,
  MMGSDI_USIM_AUTH_CNF,
  MMGSDI_COMPUTE_IP_AUTH_CNF,

  MMGSDI_RUN_CAVE_CNF,
  MMGSDI_GENERATE_KEY_VPM_CNF,
  MMGSDI_JCDMA_GET_CARD_INFO_CNF,
  MMGSDI_CARD_RESET_CNF,
  MMGSDI_CARD_READER_STATUS_CNF,
  MMGSDI_GET_ATR_CNF,
  MMGSDI_MFLO_INIT_ACT_PARAMS_CNF,
  MMGSDI_MFLO_GET_SUBSCRIBER_ID_CNF,
  MMGSDI_MFLO_GET_PUBLIC_KEY_CNF,
  MMGSDI_MFLO_SIGN_UCAST_CNF,

  MMGSDI_MFLO_VERIFY_UCAST_CNF,
  MMGSDI_CLIENT_INIT_COMP_CNF,
  MMGSDI_BCAST_CNF,
  MMGSDI_SEARCH_CNF,
  MMGSDI_CREATE_PKCS15_TABLE_CNF,
  MMGSDI_MAX_CNF_ENUM = 0x7FFFFFFF
}mmgsdi_cnf_enum_type;

/*===========================================================================
   ENUM:      MMGSDI_RETURN_ENUM_TYPE

   DESCRIPTION:
     Indication of return status of MMGSDI process
=============================================================================*/
typedef enum {
  MMGSDI_SUCCESS                              = 0x00000000,
  MMGSDI_INCORRECT_PARAMS,
  MMGSDI_CMD_QUEUE_FULL,
  MMGSDI_ERROR,
  MMGSDI_ACCESS_DENIED,
  MMGSDI_NOT_FOUND,
  MMGSDI_INCOMPAT_PIN_STATUS,
  MMGSDI_INCORRECT_CODE,
  MMGSDI_CODE_BLOCKED,
  MMGSDI_INCREASE_IMPOSSIBLE,

  MMGSDI_NOT_SUPPORTED,
  MMGSDI_NOT_INIT,
  MMGSDI_SUCCESS_BUT_ILLEGAL_SIM,
  MMGSDI_AUTH_ERROR_INCORRECT_MAC,
  MMGSDI_AUTH_ERROR_GSM_CNTXT_NOT_SUP,
  MMGSDI_SIM_TECHNICAL_PROBLEMS,
  MMGSDI_NO_EF_SELECTED,
  MMGSDI_EF_INCONSISTENT,
  MMGSDI_ERROR_NO_EVENT_NEEDED,
  MMGSDI_PIN_NOT_INITIALIZED,

  MMGSDI_UNKNOWN_INST_CLASS,
  MMGSDI_WARNING_NO_INFO_GIVEN,
  MMGSDI_WARNING_POSSIBLE_CORRUPTION,
  MMGSDI_INCORRECT_LENGTH,
  MMGSDI_UIM_CMD_TIMEOUT,
  MMGSDI_CODE_PERM_BLOCKED,
  MMGSDI_REFRESH_SUCCESS,
  MMGSDI_REFRESH_IN_PROGRESS,
  MMGSDI_CAN_NOT_REFRESH,
  MMGSDI_REFRESH_LATER,

  MMGSDI_PATHS_DECODE_ERROR,
  MMGSDI_SUCCESS_AFTER_CARD_INTERNAL_UPDATE,
  MMGSDI_SIM_BUSY,
  MMGSDI_INVALIDATION_CONTRADICTION_STATUS,
  MMGSDI_INCREASE_MAX_REACHED,
  MMGSDI_AUTH_FAIL,
  MMGSDI_AUTS_FAIL,
  MMGSDI_MEMORY_ERROR_HEAP_EXHAUSTED,
  MMGSDI_UIM_INTERNALLY_RESET,
  MMGSDI_AUTH_ERROR_KEY_FRESHNESS_FAIL,

  MMGSDI_AUTH_ERROR_NO_MEM_SPACE,
  MMGSDI_REF_DATA_NOT_FOUND,
  MMGSDI_LOCKED_INIT_PARAM,
  MMGSDI_APP_NOT_INIT,
  MMGSDI_PUBLIC_KEY_CERT_NOT_INIT,
  MMGSDI_UCAST_AUTH_KEY_NOT_INIT,
  MMGSDI_UCAST_DATA_BLOCK_NOT_INIT,
  MMGSDI_VERIFY_UCAST_FAILED,
  MMGSDI_REFRESH_SUCCESS_NO_AID_ACTIVE,
  MMGSDI_REFRESH_ADDITIONAL_FILES_READ,

  MMGSDI_AUTH_ERROR_CAVE_NOT_RUN,
  MMGSDI_REFRESH_FAIL_INVALID_AID,
  MMGSDI_RPC_ERROR,
  MMGSDI_WRONG_CLASS,
  MMGSDI_MAX_RETURN_ENUM                      = 0x7FFFFFFF
}mmgsdi_return_enum_type;

/*===========================================================================
   ENUM:      MMGSDI_EVENTS_ENUM_TYPE

   DESCRIPTION:
     Indication of return event of MMGSDI process
=============================================================================*/
typedef enum {
  MMGSDI_CARD_INSERTED_EVT = 0x000000000,
  MMGSDI_CARD_ERROR_EVT,
  MMGSDI_CARD_REMOVED_EVT, // TODO New, need to add
  MMGSDI_CARD_INIT_COMPLETED_EVT, /* Prov and no prov */
  MMGSDI_PIN1_EVT,  /* Verified, Blocked, Unblocked, Enabled, Disabled, Changed*/
  MMGSDI_PIN2_EVT,
  MMGSDI_UNIVERSAL_PIN_EVT,
  MMGSDI_TERMINAL_PROFILE_DL_EVT, /* OK, Not OK */
  MMGSDI_REFRESH_EVT, /* SIM_RESET, SIM_INIT, SIM_INIT_FCN, FCN, Failed */
  MMGSDI_FDN_EVT, /* ENABLE, disable */

  MMGSDI_SWITCH_SLOT_EVT, /* Start, Finish */
  MMGSDI_ILLEGAL_CARD_EVT, /* Start, Finish */
  MMGSDI_SAP_CONNECT_EVT, /* Connecting as Server/Client */
  MMGSDI_SAP_DISCONNECT_EVT, /* Disconnecting Server/Client, Disconnect by Server/Client
                                Disconnect Gracefully or Immediately */
  MMGSDI_SAP_PIN_EVT, /* No extra info */
  MMGSDI_SESSION_CLOSE_EVT, /* Session ID, AID */
  MMGSDI_SELECT_AID_EVT, /*AID*/
  MMGSDI_MAX_EVT_ENUM      = 0x7FFFFFFF
}mmgsdi_events_enum_type;

/*===========================================================================
   ENUM:      MMGSDI_SAP_STATE_ENUM_TYPE

   DESCRIPTION:
     Different states involved during SAP connect/disconnect
=============================================================================*/
typedef enum {
  MMGSDI_SAP_CONNECTING       = 0x00000000,
  MMGSDI_SAP_CONNECTED,
  MMGSDI_SAP_CONNECT_FAIL,
  MMGSDI_SAP_DISCONNECTING,
  MMGSDI_SAP_DISCONNECTED,
  MMGSDI_SAP_DISCONNECT_FAIL,
  MMGSDI_MAX_SAP_STATE_ENUM   = 0x7FFFFFFF
}mmgsdi_sap_state_enum_type;


/*===========================================================================
   ENUM:      MMGSDI_FILE_ENUM_TYPE

   DESCRIPTION:
     The following enumeration will be used to access the
     SIM/USIM/RUIM Elementary Files.
     MMGSDI_MAX_FILE_ENUM:  force the enumerator to be of int32 type
     WARNING:  Please note, that the position of these
                 enumerations are used as a index into an internal MMGSDI Table.
                 Only add additional elementary files to the end of this
                 list of enumerations.
=============================================================================*/
typedef enum
{
  /* 0 */
  /* EFs at the MF level */
  MMGSDI_ICCID                         = 0x00000000,  /* ICCID */
  MMGSDI_ELP,                /* Extended Language Preference */
  MMGSDI_DIR,                /* EF DIR for USIM */
  MMGSDI_ARR,                /* Access rule reference */
  /* EFs at CDMA level */
  MMGSDI_CDMA_CC,                  /* Call Count */
  MMGSDI_CDMA_IMSI_M,              /* IMSI_M */
  MMGSDI_CDMA_IMSI_T,              /* IMSI_T */
  MMGSDI_CDMA_TMSI,                /* TMSI */
  MMGSDI_CDMA_ANALOG_HOME_SID,     /* Analog Home SID */
  MMGSDI_CDMA_ANALOG_OP_PARAMS,    /* Analog Operational Parameters */

  /* 10 */
  MMGSDI_CDMA_ANALOG_LOCN_AND_REGN_IND,  /* Analog Locn and Regn Indicators */
  MMGSDI_CDMA_HOME_SID_NID,        /* CDMA Home SID and NID */
  MMGSDI_CDMA_ZONE_BASED_REGN_IND, /* CDMA Zone Based Regn Indicators */
  MMGSDI_CDMA_SYS_REGN_IND,        /* CDMA System/Network Regn Indicators */
  MMGSDI_CDMA_DIST_BASED_REGN_IND, /* CDMA Distance Based Regn Indicators */
  MMGSDI_CDMA_ACCOLC,              /* Access Overload Class */
  MMGSDI_CDMA_CALL_TERM_MODE_PREF, /* Call Termination Mode Preferences */
  MMGSDI_CDMA_SCI,                 /* Suggested Slot Cycle Index */
  MMGSDI_CDMA_ANALOG_CHAN_PREF,    /* Analog Channel Preferences */
  MMGSDI_CDMA_PRL,                 /* Preferred Roaming List */

  /* 20 */
  MMGSDI_CDMA_RUIM_ID,             /* Removable UIM ID */
  MMGSDI_CDMA_SVC_TABLE,           /* CDMA Service Table */
  MMGSDI_CDMA_SPC,                 /* Service Programming Code */
  MMGSDI_CDMA_OTAPA_SPC_ENABLE,    /* OTAPA/SPC-Enable */
  MMGSDI_CDMA_NAM_LOCK,            /* NAM-Lock */
  MMGSDI_CDMA_OTASP_OTAPA_FEATURES,/* OTASP-OTAPA Parameters */
  MMGSDI_CDMA_SERVICE_PREF,        /* Service Preferences */
  MMGSDI_CDMA_ESN_ME,              /* ESN_ME */
  MMGSDI_CDMA_RUIM_PHASE,          /* R-UIM Phase */
  MMGSDI_CDMA_PREF_LANG,           /* Preferred Languages */

  /* 30 */
  MMGSDI_CDMA_UNASSIGNED_1,        /* Not assigned yet */
  MMGSDI_CDMA_SMS,                 /* Short Messages */
  MMGSDI_CDMA_SMS_PARAMS,          /* Short Message Service Parameters */
  MMGSDI_CDMA_SMS_STATUS,          /* SMS Status */
  MMGSDI_CDMA_SUP_SVCS_FEATURE_CODE_TABLE, /* Sup Services Feature Code Table */
  MMGSDI_CDMA_UNASSIGNED_2,        /* Not assigned yet */
  MMGSDI_CDMA_HOME_SVC_PVDR_NAME,  /* CDMA Home Service Provider Name */
  MMGSDI_CDMA_UIM_ID_USAGE_IND,    /* R-UIM ID usage indicator */
  MMGSDI_CDMA_ADM_DATA,            /* CDMA Administrative Data */
  MMGSDI_CDMA_MSISDN,              /* Mobile Directory Number */

  /* 40 */
  MMGSDI_CDMA_MAXIMUM_PRL,         /* Max size of the PRL */
  MMGSDI_CDMA_SPC_STATUS,          /* SPC Status */
  MMGSDI_CDMA_ECC,                 /* Emergency Call Codes */
  MMGSDI_CDMA_3GPD_ME3GPDOPC,      /* Operational Capabilities */
  MMGSDI_CDMA_3GPD_3GPDOPM,        /* Operational Mode */
  MMGSDI_CDMA_3GPD_SIPCAP,         /* Simple IP Capabilities */
  MMGSDI_CDMA_3GPD_MIPCAP,         /* Mobile IP Capabilities */
  MMGSDI_CDMA_3GPD_SIPUPP,         /* Simple IP User Profile Parameters */
  MMGSDI_CDMA_3GPD_MIPUPP,         /* Mobile IP User Profile Parameters */
  MMGSDI_CDMA_3GPD_SIPSP,          /* Simple IP Status Parameters */

  /* 50 */
  MMGSDI_CDMA_3GPD_MIPSP,          /* Mobile IP Status Parameters */
  MMGSDI_CDMA_3GPD_SIPPAPSS,       /* Simple IP PAP SS Parameters */
  MMGSDI_CDMA_UNASSIGNED_3,        /* Reserved */
  MMGSDI_CDMA_UNASSIGNED_4,        /* Reserved */
  MMGSDI_CDMA_PUZL,                /* Preffered User Zone List */
  MMGSDI_CDMA_MAXPUZL,             /* Maximum PUZL */
  MMGSDI_CDMA_MECRP,               /* ME-specific Config REquest Param */
  MMGSDI_CDMA_HRPDCAP,             /* HRPD Access Auth Capability Param */
  MMGSDI_CDMA_HRPDUPP,             /* HRPD Access Auth User Profile Param */
  MMGSDI_CDMA_CSSPR,               /* CUR_SSPR_P_REV */

  /* 60 */
  MMGSDI_CDMA_ATC,                 /* Acceess Terminal Class */
  MMGSDI_CDMA_EPRL,                /* Extended Preffered Roaming List */
  MMGSDI_CDMA_BCSMS_CONFIG,         /* Broadcast SMS Configuration */
  MMGSDI_CDMA_BCSMS_PREF,           /* Broadcast SMS Preferences */
  MMGSDI_CDMA_BCSMS_TABLE,          /* Broadcast SMS Table */
  MMGSDI_CDMA_BCSMS_PARAMS,         /* Broadcast SMS Paramaters */
  MMGSDI_CDMA_MMS_NOTIF,            /* MMS Notification */
  MMGSDI_CDMA_MMS_EXT8,             /* MMS Extension 8 */
  MMGSDI_CDMA_MMS_ICP,              /* MMS Issuer Connectivity Parameters */
  MMGSDI_CDMA_MMS_UP,               /* MMS User Preferences */

  /* 70 */
  MMGSDI_CDMA_SMS_CAP,              /* SMS Capabilities */
  MMGSDI_CDMA_3GPD_IPV6CAP,         /* IPv6 Capabilities */
  MMGSDI_CDMA_3GPD_MIPFLAGS,        /* Mobile IP Flags */
  MMGSDI_CDMA_3GPD_TCPCONFIG,       /* TCP Configurations */
  MMGSDI_CDMA_3GPD_DGC,             /* Data Generic Configurations */
  MMGSDI_CDMA_BROWSER_CP,           /* Browser Connectivity Parameters */
  MMGSDI_CDMA_BROWSER_BM,           /* Browser Bookmarks */
  MMGSDI_CDMA_3GPD_SIPUPPEXT,       /* Simple IP User Profile Parameters Extension */
  MMGSDI_CDMA_MMS_CONFIG,           /* MMS Configuration */
  MMGSDI_CDMA_JAVA_DURL,            /* Java Download URL */

  /* 80 */
  MMGSDI_CDMA_3GPD_MIPUPPEXT,       /* Mobile IP User Profile Parameters Extension */
  MMGSDI_CDMA_BREW_DLOAD,           /* BREW Download */
  MMGSDI_CDMA_BREW_TSID,            /* BREW Teleservice ID */
  MMGSDI_CDMA_BREW_SID,             /* BREW Subscriber ID */
  MMGSDI_CDMA_LBS_XCONFIG,          /* LBS XTRA Configuration */
  MMGSDI_CDMA_LBS_XSURL,            /* LBS XTRA Server URLs */
  MMGSDI_CDMA_LBS_V2CONFIG,         /* LBS V2 Configuration */
  MMGSDI_CDMA_LBS_V2PDEADDR,        /* LBS V2 PDE Address */
  MMGSDI_CDMA_LBS_V2MPCADDR,        /* LBS V2 MPC Address */
  MMGSDI_CDMA_BREW_AEP,             /* Brew Application Execution Policy */

  /* 90 */
  MMGSDI_CDMA_MODEL,                /* Model Info */
  MMGSDI_CDMA_RC,                   /* Root Certificate */
  MMGSDI_CDMA_APP_LABELS,           /* Root Certificate */
  MMGSDI_CDMA_USER_AGENT_STRING,    /* KDDI Agent String */
  MMGSDI_CDMA_GID_REFERENCES,       /* KDDI GID References */
  MMGSDI_CDMA_ME_DOWNLOADABLE_DATA, /* KDDI ME Downloadable Data */
  MMGSDI_CDMA_ME_SETTING_DATA,      /* KDDI ME Setting Date */
  MMGSDI_CDMA_ME_USER_DATA,         /* KDDI User Data */
  MMGSDI_CDMA_RESERVED2,            /* KDDI Reserved */
  MMGSDI_CDMA_RESERVED1,            /* KDDI Reserved */

  /* 100 */
  MMGSDI_CDMA_UIM_SVC_TABLE,        /* KDDI UIM Service Table */
  MMGSDI_CDMA_UIM_FEATURES,         /* KDDI UIM Features */
  MMGSDI_CDMA_HRPD_HRPDUPP,         /* HRPD User Profile Parameters */
  MMGSDI_CDMA_SF_EUIM_ID,           /* Short Form EUIMID */
  /* JCDMA EFs */             /* Certain KDDI EFs are listed as JCDMA EFs*/
  MMGSDI_CDMA_UIM_VERSION,    /* JCDMA signature and version*/
  MMGSDI_CDMA_IMSI_STATUS,    /* JCDMA imsi status*/
  MMGSDI_CDMA_GID1,           /* JCDMA service provider id*/
  MMGSDI_CDMA_GID2,           /* JCDMA user group id */
  /* 3GPD */
  MMGSDI_CDMA_3GPD_OP_CAP,
  /* EFs in DF GSM */
  MMGSDI_GSM_LP,              /* Language Preference */

  /* 110 */
  MMGSDI_GSM_IMSI,            /* IMSI */
  MMGSDI_GSM_KC,              /* Ciphering Key Kc */
  MMGSDI_GSM_PLMN,            /* PLMN selector */
  MMGSDI_GSM_HPLMN,           /* HPLMN search period */
  MMGSDI_GSM_ACM_MAX,         /* ACM Maximum value */
  MMGSDI_GSM_SST,             /* SIM Service table */
  MMGSDI_GSM_ACM,             /* Accumulated call meter */
  MMGSDI_GSM_GID1,            /* Group Identifier Level 1 */
  MMGSDI_GSM_GID2,            /* Group Identifier Level 2 */
  MMGSDI_GSM_SPN,             /* Service Provider Name */

  /* 120 */
  MMGSDI_GSM_PUCT,            /* Price Per Unit and currency table */
  MMGSDI_GSM_CBMI,            /* Cell broadcast message identifier selection */
  MMGSDI_GSM_BCCH,            /* Broadcast control channels */
  MMGSDI_GSM_ACC,             /* Access control class */
  MMGSDI_GSM_FPLMN,           /* Forbidden PLMNs */
  MMGSDI_GSM_LOCI,            /* Location information */
  MMGSDI_GSM_AD,              /* Administrative data */
  MMGSDI_GSM_PHASE,           /* Phase identification */
  MMGSDI_GSM_VGCS,            /* Voice Group Call service */
  MMGSDI_GSM_VGCSS,           /* Voice Group Call service status */

  /* 130 */
  MMGSDI_GSM_VBS,             /* Voice Broadcast service  */
  MMGSDI_GSM_VBSS,            /* Voice Broadcast service status */
  MMGSDI_GSM_EMLPP,           /* Enhanced multi level pre-emption and priority */
  MMGSDI_GSM_AAEM,            /* Automatic Answer for eMLPP service */
  MMGSDI_GSM_CBMID,           /* Cell Broadcast Message id for data dload */
  MMGSDI_GSM_ECC,             /* Emergency Call Codes */
  MMGSDI_GSM_CBMIR,           /* Cell Broadcast Message id range selection */
  MMGSDI_GSM_DCK,             /* De-personalization control keys */
  MMGSDI_GSM_CNL,             /* Co-operative network list */
  MMGSDI_GSM_NIA,             /* Network's indication of alerting */

  /* 140 */
  MMGSDI_GSM_KCGPRS,          /* GPRS ciphering key */
  MMGSDI_GSM_LOCIGPRS,        /* GPRS location information */
  MMGSDI_GSM_SUME,            /* Setup Menu elements */
  MMGSDI_GSM_PLMNWACT,        /* PLMN Selector with Access technology */
  MMGSDI_GSM_OPLMNWACT,       /* Operator controlled PLMNWACT */
  MMGSDI_GSM_HPLMNACT,        /* HPLNMN Access technology */
  MMGSDI_GSM_CPBCCH,          /* CPBCCH information */
  MMGSDI_GSM_INVSCAN,         /* Investigation PLMN Scan */
  MMGSDI_GSM_RPLMNAT,         /* RPLMN  Last used Access Technology */
  MMGSDI_GSM_PNN,

  /* 150 */
  MMGSDI_GSM_OPL,
  MMGSDI_GSM_MBDN,            /* GSM: Mail Box Dialing Number*/
  MMGSDI_GSM_EXT6,            /* GSM: Extension 6*/
  MMGSDI_GSM_MBI,             /* GSM: Mail Box Identifier*/
  MMGSDI_GSM_MWIS,            /* GSM: Message Wating Indication Status*/
  MMGSDI_GSM_EXT1,            /* GSM: Extension 1*/
  MMGSDI_GSM_SPDI,            /* GSM: Service Provider Display Information*/
  /* EFs for CPHS */
  MMGSDI_GSM_CFIS,            /* GSM: Call Forwarding Indication status */
  MMGSDI_GSM_VMWI,            /* CPHS: Voice Mail Waiting Indicator */
  MMGSDI_GSM_SVC_STR_TBL,     /* CPHS: Service String Table */

  /* 160 */
  MMGSDI_GSM_CFF,             /* CPHS: Call Forwarding Flag */
  MMGSDI_GSM_ONS,             /* CPHS: Operator Name String */
  MMGSDI_GSM_CSP,             /* CPHS: Customer Service Profile */
  MMGSDI_GSM_CPHSI,           /* CPHS: CPHS Information */
  MMGSDI_GSM_MN,              /* CPHS: Mailbox Number */
  /* EFs at SoLSA */
  MMGSDI_GSM_SAI,             /* SoLSA access indicator */
  MMGSDI_GSM_SLL,             /* SoLSA LSA list */
  /* EFs at MExE Level */
  MMGSDI_GSM_MEXE_ST,         /* MExE Service table */
  MMGSDI_GSM_ORPK,            /* Operator Root Public Key */
  MMGSDI_GSM_ARPK,            /* Administrator Root Public Key */

  /* 170 */
  MMGSDI_GSM_TPRPK,           /* Third party Root public key */
  /* image file */
  MMGSDI_GSM_IMG,
  MMGSDI_GSM_IMG1INST1,
  MMGSDI_GSM_IMG1INST2,
  MMGSDI_GSM_IMG1INST3,
  MMGSDI_GSM_IMG2INST1,
  MMGSDI_GSM_IMG2INST2,
  MMGSDI_GSM_IMG2INST3,
  MMGSDI_GSM_IMG3INST1,
  MMGSDI_GSM_IMG3INST2,

  /* 180 */
  MMGSDI_GSM_IMG3INST3,
  MMGSDI_IMAGE,
  /* EFs at the telecom DF in GSM and CDMA */
  MMGSDI_TELECOM_ADN,         /* Abbrev Dialing Numbers */
  MMGSDI_TELECOM_FDN,         /* Fixed dialling numbers */
  MMGSDI_TELECOM_SMS,         /* Short messages */
  MMGSDI_TELECOM_CCP,         /* Capability Configuration Parameters */
  MMGSDI_TELECOM_ECCP,        /* Extended CCP */
  MMGSDI_TELECOM_MSISDN,      /* MSISDN */
  MMGSDI_TELECOM_SMSP,        /* SMS parameters */
  MMGSDI_TELECOM_SMSS,        /* SMS Status */

  /* 190 */
  MMGSDI_TELECOM_LND,         /* Last number dialled */
  MMGSDI_TELECOM_SDN,         /* Service Dialling numbers */
  MMGSDI_TELECOM_EXT1,        /* Extension 1 */
  MMGSDI_TELECOM_EXT2,        /* Extension 2 */
  MMGSDI_TELECOM_EXT3,        /* Extension 3 */
  MMGSDI_TELECOM_BDN,         /* Barred Dialing Numbers */
  MMGSDI_TELECOM_EXT4,        /* Extension 4 */
  MMGSDI_TELECOM_SMSR,        /* SMS reports */
  MMGSDI_TELECOM_CMI,         /* Comparison Method Information */
  MMGSDI_TELECOM_SUME,        /* Setup Menu elements */

  /* 200 */
  MMGSDI_TELECOM_ARR,         /* Access Rule reference */
  /* EFs at DF GRAPHICS under DF Telecom */
  MMGSDI_IMAGE_FILE,          /* Image instance data files */
  /* EFs at the DF PHONEBOOK under  DF Telecom */
  MMGSDI_TELECOM_PBR,         /* Phone book reference file */
  MMGSDI_TELECOM_CCP1,        /* Capability Configuration parameters 1 */
  MMGSDI_TELECOM_UID,         /* Unique Identifier */
  MMGSDI_TELECOM_PSC,         /* Phone book synchronization center */
  MMGSDI_TELECOM_CC,          /* Change counter */
  MMGSDI_TELECOM_PUID,        /* Previous Unique Identifier */
  /* EFs selected by path at the telecom DF in GSM and CDMA */
  MMGSDI_TELECOM_IAP,         /* Index Administration File */
  MMGSDI_TELECOM_PBC,         /* Phone Book Control */

  /* 210 */
  MMGSDI_TELECOM_GRP,         /* Grouping File */
  MMGSDI_TELECOM_AAS,         /* Additional Number Alpha String */
  MMGSDI_TELECOM_GAS,         /* Grouping Information Alpha String */
  MMGSDI_TELECOM_ANR,         /* Additional Number for PBR Rec 1    */
  MMGSDI_TELECOM_SNE,         /* Secondary Name Entry */
  MMGSDI_TELECOM_EMAIL,       /* Email */
  MMGSDI_TELECOM_ANRA,        /* Additional Number A for PBR Rec 1  */
  MMGSDI_TELECOM_ANRB,        /* Additional Number B for PBR Rec 1  */
  MMGSDI_TELECOM_ANR1,        /* Additional Number for PBR Rec 2    */
  MMGSDI_TELECOM_ANRA1,       /* Additional Number A1 for PBR Rec 2 */

  /* 220 */
  MMGSDI_TELECOM_ANRB1,       /* Additional Number B1 for PBR Rec 2 */
  MMGSDI_TELECOM_ADN1,        /* Abbreviated Dialing Numbers 1 */
  MMGSDI_TELECOM_PBC1,        /* Phone Book Control 1 */
  MMGSDI_TELECOM_GRP1,        /* Grouping File 1 */
  MMGSDI_TELECOM_SNE1,        /* Secondary Name Entry 1 */
  MMGSDI_TELECOM_UID1,        /* Unique Identifier 1 */
  MMGSDI_TELECOM_EMAIL1,      /* Email 1 */
  MMGSDI_TELECOM_ANR2,        /* Additional Number for PBR Rec 3    */
  MMGSDI_TELECOM_ANRA2,       /* Additional Number A2 for PBR Rec 3 */
  MMGSDI_TELECOM_ANRB2,       /* Additional Number B2 for PBR Rec 3 */

  /* 230 */
  MMGSDI_TELECOM_ANRC2,       /* Additional Number C2 for PBR Rec 3 */
  MMGSDI_TELECOM_ADN2,        /* Abbreviated Dialing Numbers 3 */
  MMGSDI_TELECOM_PBC2,        /* Phone Book Control 3 */
  MMGSDI_TELECOM_GRP2,        /* Grouping File 3 */
  MMGSDI_TELECOM_SNE2,        /* Secondary Name Entry 3 */
  MMGSDI_TELECOM_UID2,        /* Unique Identifier 3 */
  MMGSDI_TELECOM_EMAIL2,      /* Email 3 */
  MMGSDI_TELECOM_IAP2,        /* Index Administration File 3 */
  MMGSDI_TELECOM_ANR3,        /* Additional Number for PBR Rec 3    */
  MMGSDI_TELECOM_ANRA3,       /* Additional Number A3 for PBR Rec 3 */

  /* 240 */
  MMGSDI_TELECOM_ANRB3,       /* Additional Number B3 for PBR Rec 3 */
  MMGSDI_TELECOM_ANRC3,       /* Additional Number C3 for PBR Rec 3 */
  MMGSDI_TELECOM_ADN3,        /* Abbreviated Dialing Numbers 3 */
  MMGSDI_TELECOM_PBC3,        /* Phone Book Control 3 */
  MMGSDI_TELECOM_GRP3,        /* Grouping File 3 */
  MMGSDI_TELECOM_SNE3,        /* Secondary Name Entry 3 */
  MMGSDI_TELECOM_UID3,        /* Unique Identifier 3 */
  MMGSDI_TELECOM_EMAIL3,      /* Email 3 */
  MMGSDI_TELECOM_IAP3,        /* Index Administration File 3 */
  MMGSDI_TELECOM_IAP1,        /* Index Administration File 1 */

  /* 250 */
  MMGSDI_TELECOM_ANRC,        /* Additional Number C  for PBR Rec 1 */
  MMGSDI_TELECOM_ANRC1,       /* Additional Number C  for PBR Rec 2 */
  /* EFs of USIM ADF */
  MMGSDI_USIM_LI,             /* Language Indication */
  MMGSDI_USIM_IMSI,           /* IMSI */
  MMGSDI_USIM_KEYS,           /* Ciphering and Integrity keys */
  MMGSDI_USIM_KEYSPS,         /* C and I keys for packet switched domain */
  MMGSDI_USIM_PLMNWACT,       /* User controlled PLMN selector with access tech */
  MMGSDI_USIM_UPLMNSEL,       /* UPLMN selector */
  MMGSDI_USIM_HPLMN,          /* HPLMN search period */
  MMGSDI_USIM_ACM_MAX,        /* ACM maximum value */

  /* 260 */
  MMGSDI_USIM_UST,            /* USIM Service table */
  MMGSDI_USIM_ACM,            /* Accumulated Call meter */
  MMGSDI_USIM_GID1,           /* Group Identifier Level  */
  MMGSDI_USIM_GID2,           /* Group Identifier Level 2 */
  MMGSDI_USIM_SPN,            /* Service Provider Name */
  MMGSDI_USIM_PUCT,           /* Price Per Unit and Currency table */
  MMGSDI_USIM_CBMI,           /* Cell Broadcast Message identifier selection */
  MMGSDI_USIM_ACC,            /* Access control class */
  MMGSDI_USIM_FPLMN,          /* Forbidden PLMNs */
  MMGSDI_USIM_LOCI,           /* Location information */

  /* 270 */
  MMGSDI_USIM_AD,             /* Administrative data */
  MMGSDI_USIM_CBMID,          /* Cell Broadcast msg identifier for data download */
  MMGSDI_USIM_ECC,            /* Emergency call codes */
  MMGSDI_USIM_CBMIR,          /* Cell broadcast msg identifier range selection */
  MMGSDI_USIM_PSLOCI,         /* Packet switched location information */
  MMGSDI_USIM_FDN,            /* Fixed dialling numbers */
  MMGSDI_USIM_SMS,            /* Short messages */
  MMGSDI_USIM_MSISDN,         /* MSISDN */
  MMGSDI_USIM_SMSP,           /* SMS parameters */
  MMGSDI_USIM_SMSS,           /* SMS Status */

  /* 280 */
  MMGSDI_USIM_SDN,            /* Service dialling numbers */
  MMGSDI_USIM_EXT2,           /* Extension 2 */
  MMGSDI_USIM_EXT3,           /* Extension 3 */
  MMGSDI_USIM_SMSR,           /* SMS reports */
  MMGSDI_USIM_ICI,            /* Incoming call information */
  MMGSDI_USIM_OCI,            /* Outgoing call information */
  MMGSDI_USIM_ICT,            /* Incoming call timer */
  MMGSDI_USIM_OCT,            /* Outgoing call timer */
  MMGSDI_USIM_EXT5,           /* Extension 5 */
  MMGSDI_USIM_CCP2,           /* Capability Configuration Parameters 2 */

  /* 290 */
  MMGSDI_USIM_EMLPP,          /* Enhanced Multi Level Precedence and Priority */
  MMGSDI_USIM_AAEM,           /* Automatic answer for eMLPP service */
  MMGSDI_USIM_GMSI,           /* Group identity */
  MMGSDI_USIM_HIDDENKEY,      /* key for hidden phonebook entries */
  MMGSDI_USIM_BDN,            /* Barred dialling numbers */
  MMGSDI_USIM_EXT4,           /* Extension 4 */
  MMGSDI_USIM_CMI,            /* Comparison Method information */
  MMGSDI_USIM_EST,            /* Enabled services table */
  MMGSDI_USIM_ACL,            /* Access Point Name Control List */
  MMGSDI_USIM_DCK,            /* De-personalization Control Keys */

  /* 300 */
  MMGSDI_USIM_CNL,            /* Co-operative network list */
  MMGSDI_USIM_START_HFN,      /* Init values for Hyper-frame number */
  MMGSDI_USIM_THRESHOLD,      /* Max value of START */
  MMGSDI_USIM_OPLMNWACT,      /* Operator controlled PLMN sel with access tech */
  MMGSDI_USIM_OPLMNSEL,       /* OPLMN selector */
  MMGSDI_USIM_HPLMNWACT,      /* HPLMN selector with access tech */
  MMGSDI_USIM_ARR,            /* Access Rule reference */
  MMGSDI_USIM_RPLMNACT,       /* RPLMN last used access tech */
  MMGSDI_USIM_NETPAR,         /* Network parameters */
  MMGSDI_USIM_PNN,

  /* 310 */
  MMGSDI_USIM_OPL,
  MMGSDI_USIM_MBDN,           /* USIM: Mailbox Dialing Number*/
  MMGSDI_USIM_EXT6,           /* USIM: Extension 6 */
  MMGSDI_USIM_MBI,            /* USIM: Mailbox Identifier*/
  MMGSDI_USIM_MWIS,           /* USIM: Message Waiting Indication Status*/
  MMGSDI_USIM_SPDI,           /* USIM: Service Provider Display Information*/
  MMGSDI_USIM_EHPLMN,         /* Equivalent PLMN */
  MMGSDI_USIM_CFIS,           /* USIM: Call Forwarding Indicator Status */
  /* MBMS*/
  MMGSDI_USIM_GBABP,          /* GBA BootStrapping Parameters */
  MMGSDI_USIM_GBANL,          /* GBA NAF Derivation List Associated with
                                 NAF Derivation Proc*/

  /* 320 */
  MMGSDI_USIM_MSK,            /* MBMS Service Key */
  MMGSDI_USIM_MUK,            /* MBMS User Key */
  /* CPHS */
  MMGSDI_USIM_VMWI,           /* CPHS: Voice Mail Waiting Indicator */
  MMGSDI_USIM_SVC_STR_TBL,    /* CPHS: Service String Table */
  MMGSDI_USIM_CFF,            /* CPHS: Call Forwarding Flag */
  MMGSDI_USIM_ONS,            /* CPHS: Operator Name String */
  MMGSDI_USIM_CSP,            /* CPHS: Customer Service Profile */
  MMGSDI_USIM_CPHSI,          /* CPHS: CPHS Information */
  MMGSDI_USIM_MN,             /* CPHS: Mailbox Number */
  /* EFs selected by path of USIM ADF */
  MMGSDI_USIM_ADN,            /* Abbreviated Dialing Numbers */

  /* 330 */
  MMGSDI_USIM_IAP,            /* Index Administration File */
  MMGSDI_USIM_PBC,            /* Phone Book Control */
  MMGSDI_USIM_EXT1,           /* Extension 1 */
  MMGSDI_USIM_GRP,            /* Grouping File */
  MMGSDI_USIM_AAS,            /* Additional Number Alpha String */
  MMGSDI_USIM_AAS1,           /* Additional Number Alpha String 1 */
  MMGSDI_USIM_GAS,            /* Grouping Information Alpha String */
  MMGSDI_USIM_GAS1,           /* Grouping Information Alpha String */
  MMGSDI_USIM_ANR,            /* Additional Number */
  MMGSDI_USIM_SNE,            /* Secondary Name Entry */

  /* 340 */
  MMGSDI_USIM_EMAIL,          /* Email */
  MMGSDI_USIM_ANRA,           /* Additional Number A */
  MMGSDI_USIM_ANRB,           /* Additional Number B */
  MMGSDI_USIM_ANRC,           /* Additional Number C */
  MMGSDI_USIM_ANR1,           /* Additional Number 1 */
  MMGSDI_USIM_ANRA1,          /* Additional Number A1 */
  MMGSDI_USIM_ANRB1,          /* Additional Number B1 */
  MMGSDI_USIM_ANRC1,          /* Additional Number C1 */
  MMGSDI_USIM_ADN1,           /* Abbreviated Dialing Numbers 1 */
  MMGSDI_USIM_PBC1,           /* Phone Book Control 1 */

  /* 350 */
  MMGSDI_USIM_GRP1,           /* Grouping File 1 */
  MMGSDI_USIM_SNE1,           /* Secondary Name Entry 1 */
  MMGSDI_USIM_UID1,           /* Unique Identifier 1 */
  MMGSDI_USIM_EMAIL1,         /* Email 1 */
  MMGSDI_USIM_IAP1,           /* Index Administration File 1 */
  MMGSDI_USIM_CCP1,           /* Capability Configuration parameters 1 */
  MMGSDI_USIM_UID,            /* Unique Identifier */
  MMGSDI_USIM_ANR2,        /* Additional Number for PBR Rec 3    */
  MMGSDI_USIM_ANRA2,       /* Additional Number A2 for PBR Rec 3 */
  MMGSDI_USIM_ANRB2,       /* Additional Number B2 for PBR Rec 3 */

  /* 360 */
  MMGSDI_USIM_ANRC2,       /* Additional Number C2 for PBR Rec 3 */
  MMGSDI_USIM_ADN2,        /* Abbreviated Dialing Numbers 3 */
  MMGSDI_USIM_PBC2,        /* Phone Book Control 3 */
  MMGSDI_USIM_GRP2,        /* Grouping File 3 */
  MMGSDI_USIM_SNE2,        /* Secondary Name Entry 3 */
  MMGSDI_USIM_UID2,        /* Unique Identifier 3 */
  MMGSDI_USIM_EMAIL2,      /* Email 3 */
  MMGSDI_USIM_IAP2,        /* Index Administration File 3 */
  MMGSDI_USIM_ANR3,        /* Additional Number for PBR Rec 3    */
  MMGSDI_USIM_ANRA3,       /* Additional Number A2 for PBR Rec 3 */

  /* 370 */
  MMGSDI_USIM_ANRB3,       /* Additional Number B2 for PBR Rec 3 */
  MMGSDI_USIM_ANRC3,       /* Additional Number C2 for PBR Rec 3 */
  MMGSDI_USIM_ADN3,        /* Abbreviated Dialing Numbers 3 */
  MMGSDI_USIM_PBC3,        /* Phone Book Control 3 */
  MMGSDI_USIM_GRP3,        /* Grouping File 3 */
  MMGSDI_USIM_SNE3,        /* Secondary Name Entry 3 */
  MMGSDI_USIM_UID3,        /* Unique Identifier 3 */
  MMGSDI_USIM_EMAIL3,      /* Email 3 */
  MMGSDI_USIM_IAP3,        /* Index Administration File 3 */
  /*EFs at DF PHONEBOOK under USIM ADF */
  MMGSDI_USIM_PBR,            /* Phone book reference file */

  /* 380 */
  MMGSDI_USIM_PSC,            /* Phone book synchronization center */
  MMGSDI_USIM_CC,             /* Change counter */
  MMGSDI_USIM_PUID,           /* Previous Unique Identifier */
  /*EFs at DF GSM under USIM ADF */
  MMGSDI_USIM_KC,             /* GSM ciphering key Kc */
  MMGSDI_USIM_KCGPRS,         /* GPRS ciphering key */
  MMGSDI_USIM_CPBCCH,         /* CPBCCH information */
  MMGSDI_USIM_INVSCAN,        /* Investigation scan */
  /*EFs at DF MEXe under USIM ADF */
  MMGSDI_USIM_MEXE_ST,        /* MExE Service table */
  MMGSDI_USIM_ORPK,           /* Operator Root Public Key */
  MMGSDI_USIM_ARPK,           /* Administrator Root Public Key */

  /* 390 */
  MMGSDI_USIM_TPRPK,          /* Third party Root public key */
  /* EFs for MMS Notification */
  MMGSDI_USIM_MMSN,           /* MMS Notification */
  MMGSDI_USIM_MMSICP,         /* MMS Issuer connectivity parameters */
  MMGSDI_USIM_MMSUP,          /* MMS User Preferences */
  MMGSDI_USIM_MMSUCP,         /* MMS User connectivity parameters */
  MMGSDI_USIM_EXT8,           /* Extension 8 */
  /* Efs for PKCS #15 support */
  MMGSDI_PKCS15_ODF,          /* Object Directory File */
  MMGSDI_PKCS15_PRKDF,        /* Private Key Directory File */
  MMGSDI_PKCS15_PUKDF,        /* Public Key Directory File */
  MMGSDI_PKCS15_SKDF,         /* Secret Key Directory File */

  /* 400 */
  MMGSDI_PKCS15_CDF,          /* Certificate Directory File */
  MMGSDI_PKCS15_DODF,         /* Data Object Directory File */
  MMGSDI_PKCS15_AODF,         /* Authentication Object Directory File */
  MMGSDI_PKCS15_TI,           /* Token Information File */
  MMGSDI_PKCS15_US,           /* Unused space Information File */
  /* EFs of DCS1800 DF */
  MMGSDI_DCS1800_IMSI,        /* Imsi */
  MMGSDI_DCS1800_KC,          /* Ciphering Key Kc */
  MMGSDI_DCS1800_PLMN,        /* PLMN selector */
  MMGSDI_DCS1800_SST,         /* SIM Service table */
  MMGSDI_DCS1800_BCCH,        /* Broadcast control channels */

  /* 410 */
  MMGSDI_DCS1800_ACC,         /* Access control class */
  MMGSDI_DCS1800_FPLMN,       /* Forbidden PLMNs */
  MMGSDI_DCS1800_LOCI,        /* Location information */
  MMGSDI_DCS1800_AD,          /* Administrative data */
  /* EFs of WIM DF */
  MMGSDI_WIM_ODF,             /* WIM ODF */
  MMGSDI_WIM_TOKEN_INFO,      /* Generic token info and capabilities */
  MMGSDI_WIM_UNUSED_SPACE,    /* Unused Space */
  /* EFs of ISIM Application */
  MMGSDI_ISIM_IMPI,
  MMGSDI_ISIM_DOMAIN,
  MMGSDI_ISIM_IMPU,

  /* 420 */
  MMGSDI_ISIM_ARR,
  MMGSDI_ISIM_KEYS,
  MMGSDI_ISIM_AD,
  /* EFs for Orange */
  MMGSDI_SIM_7F40_PROP1_DFS,      /* 7F40:  Dynamic Flags Status */
  MMGSDI_SIM_7F40_PROP1_D2FS,     /* 7F40:  Dynamic2 Flag Setting */
  MMGSDI_SIM_7F40_PROP1_CSP2,     /* 7F40:  Customer Service Profile Line2*/
  MMGSDI_SIM_7F40_PROP1_PARAMS,   /* 7F40: EF Parmams, Welcome Message */
  /* EFs of ORANGE DF under USIM ADF */
  MMGSDI_USIM_7F40_PROP1_DFS,     /* Dynamic Flags Status */
  MMGSDI_USIM_7F40_PROP1_D2FS,    /* Dynamic2 Flag Setting */
  MMGSDI_USIM_7F40_PROP1_CSP2,    /* Customer Service Profile Line2*/

  /* 430 */
  MMGSDI_USIM_7F40_PROP1_PARAMS,  /* USIM: EF Parmams, Welcome Message */
  /* EFs under Cingular */
  MMGSDI_SIM_7F66_PROP1_ACT_HPLMN,  /* 7F66: Acting HPLMN */
  MMGSDI_SIM_7F66_PROP1_SPT_TABLE,  /* 7F66: Support Table*/
  /* EFs of Cingular */
  MMGSDI_USIM_7F66_PROP1_ACT_HPLMN,      /* USIM Cingular: Acting HPLMN*/
  MMGSDI_USIM_7F66_PROP1_SPT_TABLE,      /* USIM Cingular: Cingular TST*/
  MMGSDI_USIM_7F66_PROP1_RAT,            /* USIM Radio Access Technology */
  /* EFs of Home Zone */
  MMGSDI_SIM_7F43_PROP1_HZ,
  MMGSDI_SIM_7F43_PROP1_CACHE1,              /* 7F43: UHZI-Cache1*/
  MMGSDI_SIM_7F43_PROP1_CACHE2,              /* 7F43: UHZI-Cache2*/
  MMGSDI_SIM_7F43_PROP1_CACHE3,              /* 7F43: UHZI-Cache3*/

  /* 440 */
  MMGSDI_SIM_7F43_PROP1_CACHE4,              /* 7F43: UHZI-Cache4*/
  MMGSDI_SIM_7F43_PROP1_SUBSCRIBED_LAC_CI_1, /* 7F43: UHZI-SUBSCRIBED_LAC_CI_1*/
  MMGSDI_SIM_7F43_PROP1_SUBSCRIBED_LAC_CI_2, /* 7F43: UHZI-SUBSCRIBED_LAC_CI_2*/
  MMGSDI_SIM_7F43_PROP1_SUBSCRIBED_LAC_CI_3, /* 7F43: UHZI-SUBSCRIBED_LAC_CI_3*/
  MMGSDI_SIM_7F43_PROP1_SUBSCRIBED_LAC_CI_4, /* 7F43: UHZI-SUBSCRIBED_LAC_CI_4*/
  MMGSDI_SIM_7F43_PROP1_TAGS,                /* 7F43: UHZI- Tags*/
  MMGSDI_SIM_7F43_PROP1_SETTINGS,            /* 7F43: UHZI- Settings*/
  /* EFs under MediaFLO/RAN DF */
  MMGSDI_MFLO_RAN_RFR,                       /* RAN File Revision */
  MMGSDI_MFLO_RAN_RF,                        /* Radio Frequency */
  /* EFs under MediaFLO/MFAPP DF */
  MMGSDI_MFLO_MFAPP_MFR,                     /* MediaFLO Application Revision */

  /* 450 */
  MMGSDI_MFLO_MFAPP_BCSID,                   /* Billing and Customer Service provider ID */
  MMGSDI_MFLO_MFAPP_MFT,                     /* MediaFLO Feature Table */
  MMGSDI_MFLO_MFAPP_SUBTYPE,                 /* Subscriber Type */
  MMGSDI_MFLO_MFAPP_SUBPROF,                 /* Subscriber Profile */
  MMGSDI_MFLO_MFAPP_UTSD,                    /* Usage Tracking Service Data */
  MMGSDI_MFLO_MFAPP_UIP,                     /* UI Preference */
  MMGSDI_MFLO_MFAPP_PCSET,                   /* Parental Control Setting */
  MMGSDI_MFLO_MFAPP_MCHSET,                  /* Multi-presentation view Channel Setting */
  MMGSDI_MFLO_MFAPP_RFUD,                    /* Reserved for Future User Data */
  MMGSDI_NO_FILE_ENUM,  /* Last Element of the File, also to indicate that
                           there is no File enum
                           NOTE: Has to be placed right before MAX_FILE_ENUM*/
  /* 460 */
  MMGSDI_MAX_FILE_ENUM                             = 0x7FFFFFFF
} mmgsdi_file_enum_type;

/*===========================================================================
   ENUM:      MMGSDI_DF_ENUM_TYPE

   DESCRIPTION:
     The following enumeration will be used to access the
     SIM/USIM/RUIM Directories.
     MMGSDI_MAX_DF_ENUM:  force the enumerator to be of int32 type
 =============================================================================*/
typedef enum
{
  MMGSDI_MF_DIR           = 0x3F00,
  MMGSDI_GSM_DIR          = 0x7F20,
  MMGSDI_TELECOM_DIR      = 0x7F10,
  MMGSDI_LAST_SEL_ADF_DIR = 0x7FFF,
  MMGSDI_DCS1800_DIR      = 0x7F21,
  MMGSDI_RUIM_DIR         = 0x7F25,
  MMGSDI_MAX_DF_ENUM      = 0x7FFFFFFF
}mmgsdi_df_enum_type;
/*===========================================================================
   ENUM:      MMGSDI_ACCESS_METHOD_ENUM_TYPE

   DESCRIPTION:
     Indication of access method
     MMGSDI_EF_ENUM_ACCESS: Access using EF Enum provided in the
                                        the request: MMGSDI_FILE_ENUM_TYPE
                 MMGSDI_DF_ENUM_ACCESS: Access using the DF Enum provided in
                                        the request.  MMGSDI_DIR_FILE_ENUM_TYPE
                 MMGSDI_BY_PATH_ACCESS: Access using the path provided in the
                                        the request.
=============================================================================*/
typedef enum {
  MMGSDI_EF_ENUM_ACCESS,
  MMGSDI_DF_ENUM_ACCESS,
  MMGSDI_BY_PATH_ACCESS,
  MMGSDI_BY_APP_ID_ACCESS,
  MMGSDI_MAX_ACCESS_METHOD_ENUM = 0x7FFFFFFF
}mmgsdi_access_method_enum_type;

/*===========================================================================
   ENUM:      MMGSDI_TAG_ENUM_TYPE

   DESCRIPTION:
     Enums for Tag value in the MMGSDI TLV structure
     Enumerations for the Directory File (DF) Path provided by the client.
     This will be used to build the TLV for the Path provided by the client.

     NOTE:  This is required to allow the client to pass the information
     with an embedded Application Directory File (ADF)
=============================================================================*/
typedef enum {
  MMGSDI_MF_TAG       = 0x00000000,
  MMGSDI_DF1_TAG,
  MMGSDI_DF2_TAG,
  MMGSDO_DF3_TAG,
  MMGSDI_EF_TAG,
  MMGSDI_ADF_TAG,
  MMGSDI_MAX_TAG_ENUM = 0x7FFFFFFF
}mmgsdi_tag_enum_type;

/*===========================================================================
   ENUM:      MMGSDI_PIN_ENUM_TYPE

   DESCRIPTION:
     The Card Holder Verification Values (CHVs) or Personal
      Identification Numbers (PINs) for a SIM/USIM/RUIM.
                 MMGSDI_PIN1 can be either GSM/RUIM PIN1 or USIM APP1 PIN1
                 MMGSDI_PIN2 can be either GSM/RUIM PIN2 or USIM APP1 PIN2
       ADM pins refer to administrative PINs
=============================================================================*/
typedef enum {
  MMGSDI_PIN1         = 0x00000000,
  MMGSDI_PIN2,
  MMGSDI_UNIVERSAL_PIN,
  MMGSDI_ADM1,
  MMGSDI_ADM2,
  MMGSDI_ADM3,
  MMGSDI_ADM4,
  MMGSDI_ADM5,
  MMGSDI_ADM6,
  MMGSDI_ADM7,
  MMGSDI_ADM8,
  MMGSDI_ADM9,
  MMGSDI_ADM10,
  MMGSDI_ADM11,
  MMGSDI_ADM12,
  MMGSDI_ADM13,
  MMGSDI_ADM14,
  MMGSDI_ADM15,
  MMGSDI_MAX_PIN_ENUM = 0x7FFFFFFF
}mmgsdi_pin_enum_type;

/*===========================================================================
   ENUM:      MMGSDI_SLOT_ID_ENUM_TYPE

   DESCRIPTION:
     Slot ID used to direct the MMGSDI Task and Drivers which Slot to
     communicate with.
      MMGSDI_SLOT_1:          Direct the request to the
                              SIM/USIM/RUIM inserted in Slot 1.
      MMGSDI_SLOT_2:          Direct the request to the
                              SIM/USIM/RUIM inserted in SLot 2.
      MMGSDI_SLOT_AUTOMATIC:  Will direct the request to the slot for which the
                              relevant Subscription is being obtained from.
                              For example, if ME is acquiring GSM with the
                              information form Slot 1, and there are two GSM
                              SIMs in the ME, then the request will be directed
                              to slot 1.
=============================================================================*/
typedef enum {
  MMGSDI_SLOT_1          = 0x00000001,
  MMGSDI_SLOT_2,
  MMGSDI_SLOT_AUTOMATIC,
  MMGSDI_MAX_SLOT_ID_ENUM = 0x7FFFFFFF
}mmgsdi_slot_id_enum_type;


/*===========================================================================
   STRUCTURE:      MMGSDI_APP_ENUM_TYPE

   DESCRIPTION:
     The following enumeration will be used to indicate which app the client is
     talking to.
=============================================================================*/
typedef enum{
 MMGSDI_APP_NONE,
 MMGSDI_APP_SIM,
 MMGSDI_APP_RUIM,
 MMGSDI_APP_USIM,
 MMGSDI_APP_UNKNOWN
}mmgsdi_app_enum_type;


/*===========================================================================
   ENUM:      MMGSDI_FILE_STRUCTURE_ENUM_TYPE

   DESCRIPTION:
     Will be used to indicate the type of file
     LINEAR FIXED   - Indexed using Record Numbers
                      An offset is not allowed
     CYCLIC         - Indexed using Record Numbers
                      An offset is not allowed
                      Some files will allow for Increase
     TRANSPARENT    - Is a flat file where an offset is allowed
                      Indexing by Records is not allowed
     MASTER FILE    - Indication of a Mater File
     DEDICATED FILE - A Dedicated File (DF) allows for a unctional grouping of
                      files. It can be the parent of DFs and/or EFs. DFs are
                      referenced by file identifiers.
=============================================================================*/
typedef enum {
  MMGSDI_LINEAR_FIXED_FILE       = 0x00000000,
  MMGSDI_CYCLIC_FILE,
  MMGSDI_TRANSPARENT_FILE,
  MMGSDI_MASTER_FILE,
  MMGSDI_DEDICATED_FILE,
  MMGSDI_MAX_FILE_STRUCTURE_ENUM = 0x7FFFFFFF
}mmgsdi_file_structure_enum_type;


/*===========================================================================
   ENUM:      MMGSDI_FILE_SECURITY_ENUM_TYPE

   DESCRIPTION:
     The file security access method
       MMGSDI_ALWAYS_ALLOWED - Always allowed for the access
       MMGSDI_NEVER_ALLOWED - Never allowed for the access
       MMGSDI_AND_ALLOWED - Allowed when all the PIN(s) has been verified
       MMGSDI_OR_ALLOWED - Allowed when one of the PIN(s) has been verified
       MMGSDI_SINGLE_ALLOWED - a Single Pin is required to be verified for
                               access
=============================================================================*/
typedef enum {
  MMGSDI_ALWAYS_ALLOWED         = 0x00000000,
  MMGSDI_NEVER_ALLOWED,
  MMGSDI_AND_ALLOWED,
  MMGSDI_OR_ALLOWED,
  MMGSDI_SINGLE_ALLOWED,  //NEED TO HANDLE THIS CASE IN mmgsdi_uicc_rsp_get_file_attr
  MMGSDI_MAX_FILE_SECURITY_ENUM = 0x7FFFFFFF
}mmgsdi_file_security_enum_type;


/*===========================================================================
   ENUM:      MMGSDI_REFRESH_MODE_ENUM_TYPE

   DESCRIPTION:
     This Enum represents the different refresh mode available.
=============================================================================*/
typedef enum {
  MMGSDI_REFRESH_RESET,
  MMGSDI_REFRESH_NAA_INIT,
  MMGSDI_REFRESH_NAA_INIT_FCN,
  MMGSDI_REFRESH_NAA_FCN,
  MMGSDI_REFRESH_NAA_INIT_FULL_FCN,
  MMGSDI_REFRESH_NAA_APP_RESET,
  MMGSDI_REFRESH_3G_SESSION_RESET,
  MMGSDI_REFRESH_RESET_AUTO,
  MMGSDI_REFRESH_MODE_ENUM_MAX = 0x7FFFFFFF
}mmgsdi_refresh_mode_enum_type;

/*===========================================================================
   ENUM:      MMGSDI_REFRESH_ORIG_ENUM_TYPE

   DESCRIPTION:
     This Enum represents the different refresh original request type.
=============================================================================*/
typedef enum {
  MMGSDI_REFRESH_ORIG_REFRESH_REQ,      /*Refresh Command from the card via GSTK*/
  MMGSDI_REFRESH_ORIG_PUP_REQ,          /*Card Power up Request*/
  MMGSDI_REFRESH_ORIG_REG_REQ,          /*Registration for Refresh*/
  MMGSDI_REFRESH_ORIG_DEREG_REQ,        /*Deregistration from Refresh */
  MMGSDI_REFRESH_ORIG_OK_TO_REFRESH_REQ,/*OK_TO_INIT/OK_TO_FCN */
  MMGSDI_REFRESH_ORIG_COMPLETE_REQ,     /*Refresh Complete request*/
  MMGSDI_REFRESH_ORIG_AUTO_REFRESH_REQ, /*Refresh Command triggered by MMGSDI internally*/
  MMGSDI_REFRESH_ORIG_ENUM_MAX = 0x7FFFFFFF
}mmgsdi_refresh_orig_enum_type;

/*===========================================================================
   ENUM:      MMGSDI_REFRESH_STAGE_ENUM_TYPE

   DESCRIPTION:
     This enum indicates the possible stage for each refresh event.
=============================================================================*/
typedef enum {
  MMGSDI_REFRESH_STAGE_START    = 0x00000000,
  MMGSDI_REFRESH_STAGE_IN_PROGRESS_APP_DEACTIVATED,
  MMGSDI_REFRESH_STAGE_IN_PROGRESS_APP_ACTIVATED,
  MMGSDI_REFRESH_STAGE_END_SUCCESS,
  MMGSDI_REFRESH_STAGE_END_FAILED,
  MMGSDI_REFRESH_STAGE_WAIT_FOR_OK_TO_INIT,
  MMGSDI_REFRESH_STAGE_WAIT_FOR_OK_TO_FCN,
  MMGSDI_REFRESH_STAGE_ENUM_MAX = 0x7FFFFFFF
}mmgsdi_refresh_stage_enum_type;

/*===========================================================================
   ENUM:      MMGSDI_PIN_STATUS_ENUM_TYPE

   DESCRIPTION:
     This indicates the different PIN status.
=============================================================================*/
typedef enum {
  MMGSDI_PIN_STATUS_NOT_INITIALIZED,
  MMGSDI_PIN_ENABLED_NOT_VERIFIED,
  MMGSDI_PIN_ENABLED_VERIFIED,
  MMGSDI_PIN_DISABLED,
  MMGSDI_PIN_BLOCKED,
  MMGSDI_PIN_PERM_BLOCKED,
  MMGSDI_PIN_UNBLOCKED,
  MMGSDI_PIN_CHANGED
}mmgsdi_pin_status_enum_type;

/*===========================================================================
   ENUM:      MMGSDI_SAP_MODE_ENUM_TYPE

   DESCRIPTION:
     This indicates the different sap modes possible.
=============================================================================*/
typedef enum {
  MMGSDIBT_SERVER,
  MMGSDIBT_CLIENT,
  MMGSDIBT_MAX_SAP_MODE_ENUM = 0x7FFFFFFF
}mmgsdi_sap_mode_enum_type;

/*===========================================================================
   ENUM:      MMGSDI_DISCONNECT_MODE_ENUM_TYPE

   DESCRIPTION:
     This indicates the different disconnect modes.
=============================================================================*/
typedef enum {
  MMGSDIBT_DISCONNECT_IMMED,
  MMGSDIBT_DISCONNECT_GRACE,
  MMGSDIBT_MAX_DISCONNECT_MODE_ENUM = 0x7FFFFFFF
}mmgsdi_disconnect_mode_enum_type;

/*===========================================================================
   ENUM:      MMGSDI_STATUS_ME_APP_ENUM_TYPE

   DESCRIPTION:
     This will indicate to the card what the Mobile is going to do in
     regards to the status command sent and and for the indicated
     Application ID.

     Only applicable to a UICC.  Ignored if provided for a 2G Card (ICC).

     MMGSDI_STATUS_APP_NONE :          For GSM SIMs (ICC)
     MMGSDI_STATUS_APP_NO_INDICATION:  For UICC Cards.  Indicates that the Mobile
                                       won't init or terminate the application
     MMGSDI_STATUS_APP_INITIALIZED:    For UICC Cards.  Indicates that the Mobile
                                       will init the Application in the Phone's
                                       Image
     MMGSDI_STATUS_APP_TERMINATED:     For UICC Cards.  Indicates that the Mobile
                                       will terminate the app in the Phone's
                                       Image
     MMGSDI_STATUS_APP_MAX:            Forces an Int Size Enumeration
=============================================================================*/
typedef enum {
  MMGSDI_STATUS_APP_NONE          = 0x00000000,
  MMGSDI_STATUS_APP_NO_INDICATION,
  MMGSDI_STATUS_APP_INITIALIZED,
  MMGSDI_STATUS_APP_TERMINATED,
  MMGSDI_STATUS_APP_MAX           = 0x7FFFFFFF
}mmgsdi_status_me_app_enum_type;


/*===========================================================================
   ENUM:      MMGSDI_STATUS_RET_DATA_ENUM_TYPE

   DESCRIPTION:
     This will indicate to the card the structure of the data the
     Mobile equipment is expecting.

     Only applicable to a UICC.  Ignored if provided for a 2G Card (ICC).
     MMGSDI_STATUS_NONE:         For GSM SIMs (ICC)
     MMGSDI_STATUS_DATA_SELECT:  For UICC Cards.  Response parameters and
                                 data are identical to the response parameters
                                 data of the SELECT command.
     MMGSDI_STATUS_TLV_DATA:     For UICC Cards.  The DFNAME TLV-object of the
                                 currently selected application is returned.
     MMGSDI_STATUS_NO_DATA:      No data returned
     MMGSDI_STATUS_MAX:          Forces an Int Size Enumeration
=============================================================================*/
typedef enum
{
  MMGSDI_STATUS_NONE        = 0x00000000,  /* FOR GSM ICC Cards Only */
  MMGSDI_STATUS_DATA_SELECT,
  MMGSDI_STATUS_TLV_DATA,
  MMGSDI_STATUS_NO_DATA,
  MMGSDI_STATUS_MAX         = 0x7FFFFFFF
}mmgsdi_status_ret_data_enum_type;


/*===========================================================================
   ENUM:      MMGSDI_CARD_ERR_INFO_ENUM_TYPE

   DESCRIPTION:
     This will indicate to the client the specific error condition
     encountered.
=============================================================================*/
typedef enum
{
  MMGSDI_CARD_ERR_UNKNOWN_ERROR                     = 0x00000000,
  MMGSDI_CARD_ERR_POLL_ERROR,
  MMGSDI_CARD_ERR_NO_ATR_RCVD_AT_MAX_VOLT,
  MMGSDI_CARD_ERR_NO_ATR_RCVD_AFTER_RESET,
  MMGSDI_CARD_ERR_CORRUPT_ATR_RCVD_MAX_TIMES,
  MMGSDI_CARD_ERR_PPS_TIMED_OUT_MAX_TIMES,
  MMGSDI_CARD_ERR_VOLT_MISMATCH,
  MMGSDI_CARD_ERR_INT_CMD_TIMED_OUT_AFTER_PPS,
  MMGSDI_CARD_ERR_INT_CMD_ERR_EXCEED_MAX_NUMS,
  MMGSDI_CARD_ERR_MAXED_PARITY_ERROR,
  MMGSDI_CARD_ERR_MAXED_RX_BREAK_ERR,
  MMGSDI_CARD_ERR_MAXED_OVERRUN_ERR,
  MMGSDI_CARD_ERR_TRANS_TIMER_EXP,
  MMGSDI_CARD_ERR_PWR_DN_CMD_NOTIFY,
  MMGSDI_CARD_ERR_INT_ERR_IN_PASSIVE_MODE,
  MMGSDI_CARD_ERR_TIMED_OUT_IN_PASSIVE_MODE,
  MMGSDI_CARD_ERR_MAX_PARITY_IN_PASSIVE,
  MMGSDI_CARD_ERR_MAX_RXBRK_IN_PASSIVE,
  MMGSDI_CARD_ERR_MAX_OVERRUN_IN_PASSIVE,
  MMGSDI_CARD_ERR_MAX                               = 0x7FFFFFFF
}mmgsdi_card_err_info_enum_type;

/*===========================================================================
   ENUM:      MMGSDI_CARD_PDOWN_OPTIONS_TYPE

   DESCRIPTION:
     Indication of type of power down required.
=============================================================================*/
typedef enum {
  MMGSDI_CARD_POWER_DOWN_ONLY,
  MMGSDI_CARD_POWER_DOWN_NOTIFY_GSDI,
  MMGSDI_CARD_PDOWN_MAX_ENUM = 0x7FFFFFFF
}mmgsdi_card_pdown_options_enum_type;

/*===========================================================================
   ENUM:      MMGSDI_CARD_PUP_OPTIONS_ENUM_TYPE

   DESCRIPTION:
     Indication of what type of power up is required.
=============================================================================*/
typedef enum {
  MMGSDI_CARD_POWER_UP_NONE,
  MMGSDI_CARD_POWER_UP_SWITCH_UIM_PASSIVE,
  MMGSDI_CARD_POWER_UP_INITIAL_PUP,
  MMGSDI_CARD_PUP_MAX_ENUM = 0x7FFFFFFF
}mmgsdi_card_pup_options_enum_type;

/*===========================================================================
   ENUM:  MMGSDI_PIN_OPERATION_ENUM_TYPE

   DESCRIPTION: This enumeration is used to differentiate between the
                different PIN Operations:


   ENUMERATION:  MMGSDI_PIN_OP_NONE    : No      PIN Operation
                 MMGSDI_PIN_OP_VERIFY  : Verify  PIN Operation
                 MMGSDI_PIN_OP_ENABLE  : Enable  PIN Operation
                 MMGSDI_PIN_OP_DISABLE : Disable PIN Operation
                 MMGSDI_PIN_OP_CHANGE  : Change  PIN Operation
                 MMGSDI_PIN_OP_UNBLOCK : Unblock PIN Operation

  Please note there is no MAX Enumeration due to the fact
  that this enumeration should only be 1 byte.

=============================================================================*/
typedef enum {
  MMGSDI_PIN_OP_NONE    = 0x00,
  MMGSDI_PIN_OP_VERIFY,
  MMGSDI_PIN_OP_ENABLE,
  MMGSDI_PIN_OP_DISABLE,
  MMGSDI_PIN_OP_CHANGE,
  MMGSDI_PIN_OP_UNBLOCK,
  MMGSDI_PIN_OP_DISABLE_AND_REPLACE
}mmgsdi_pin_operation_enum_type;

/*===========================================================================
   ENUM:MMGSDI_ACL_OPERATION_ENUM_TYPE

   DESCRIPTION: This enumeration is used to differentiate between the
                different ACL Operations:


   ENUMERATION:  MMGSDI_ACL_OP_NONE     : No ACL Operation
                 MMGSDI_ACL_OP_ENABLE   : Enable ACL Operation
                 MMGSDI_ACL_OP_DISABLE  : Disable ACL Operation
   Please note there is no MAX Enumeration due to the fact
   that this enumeration should only be 1 byte.

=============================================================================*/
typedef enum {
  MMGSDI_ACL_OP_NONE    = 0x00,
  MMGSDI_ACL_OP_ENABLE,
  MMGSDI_ACL_OP_DISABLE
}mmgsdi_acl_operation_enum_type;

/*===========================================================================
   ENUM: MMGSDI_PIN_REPLACE_ENUM_TYPE

   DESCRIPTION: This enumeration is used to indicate whether the PIN is
                replaced by universal pin or not.  If the PIN has been
                replaced by universal pin, to obtain access to the EFs that
                are protected by this particular PIN, client needs to send
                a verification command for Universal PIN instead

   ENUMERATION:  MMGSDI_PIN_REPLACED_BY_UNIVERSAL
                   PIN has been replaced by Universal PIN
                 MMGSDI_PIN_NOT_REPLACED_BY_UNIVERSAL
                   PIN has not been replaced by Universal PIN
=============================================================================*/
typedef enum {
  MMGSDI_PIN_NOT_REPLACED_BY_UNIVERSAL,
  MMGSDI_PIN_REPLACED_BY_UNIVERSAL
}mmgsdi_pin_replace_enum_type;


/*===========================================================================
   ENUM: MMGSDI_SERVICE_ENUM_TYPE

   DESCRIPTION: This enumeration is used to indicate the different types of
                services available in CST/UST/SST

=============================================================================*/
typedef enum {
   MMGSDI_NONE              = 0x00000000,

   /* 100 */
   MMGSDI_CDMA_SRV_CHV1_DISABLE     = MMGSDI_CDMA_SRV,    /* CHV1 disabled function           */
   MMGSDI_CDMA_SRV_LOCAL_PHONEBOOK, /* Local Phonebook                  */
   MMGSDI_CDMA_SRV_FDN,             /* Forbidden Dialling Numbers       */
   MMGSDI_CDMA_SRV_SMS,             /* Short Message Services           */
   MMGSDI_CDMA_SRV_RFU1,            /* RFU                              */
   MMGSDI_CDMA_SRV_RFU2,            /* RFU                              */
   MMGSDI_CDMA_SRV_RFU3,            /* RFU                              */
   MMGSDI_CDMA_SRV_RFU4,            /* RFU                              */
   MMGSDI_CDMA_SRV_RFU5,            /* RFU                              */
   MMGSDI_CDMA_SRV_EXT1,            /* Extension 1                      */

   /* 110 */
   MMGSDI_CDMA_SRV_EXT2,            /* Extension 2                      */
   MMGSDI_CDMA_SRV_SMSP,            /* Short Message Service Parmeter   */
   MMGSDI_CDMA_SRV_LND,             /* Last Number Dialled              */
   MMGSDI_CDMA_SRV_RFU6,            /* RFU                              */
   MMGSDI_CDMA_SRV_RFU7,            /* RFU                              */
   MMGSDI_CDMA_SRV_RFU8,            /* RFU                              */
   MMGSDI_CDMA_SRV_SPN,             /* Service Provider Name            */
   MMGSDI_CDMA_SRV_SDN,             /* Service Dialling Numbers         */
   MMGSDI_CDMA_SRV_EXT3,            /* Extension 3                      */

   /* 200 */
   MMGSDI_GSM_SRV_CHV1_DISABLE     = MMGSDI_GSM_SRV,/* CHV1 disabled function           */
   MMGSDI_GSM_SRV_ADN,             /* GSM Abbreviated Dialling Numbers */
   MMGSDI_GSM_SRV_FDN,             /* Forbidden Dialling Numbers       */
   MMGSDI_GSM_SRV_SMS,             /* Short Message Services           */
   MMGSDI_GSM_SRV_AOC,             /* Advice of Charge                 */
   MMGSDI_GSM_SRV_CCP,             /* Configuration Capability Params  */
   MMGSDI_GSM_SRV_PLMN,            /* PLMN Selector                    */
   MMGSDI_GSM_SRV_RFU1,            /* RFU                              */
   MMGSDI_GSM_SRV_MSISDN,          /* MSISDN                           */
   MMGSDI_GSM_SRV_EXT1,            /* Extension 1                      */

   /* 210 */
   MMGSDI_GSM_SRV_EXT2,            /* Extension 2                      */
   MMGSDI_GSM_SRV_SMSP,            /* SMS Parameters                   */
   MMGSDI_GSM_SRV_LND,             /* Last Number Dialled              */
   MMGSDI_GSM_SRV_CBMI,            /* Cell Broadcast Message Identifier */
   MMGSDI_GSM_SRV_GID1,            /* Group Identifier 1               */
   MMGSDI_GSM_SRV_GID2,            /* Group Identifier 2               */
   MMGSDI_GSM_SRV_SPN,             /* Service Provider Name            */
   MMGSDI_GSM_SRV_EXT3,            /* Extension 3                      */
   MMGSDI_GSM_SRV_RFU2,            /* RFU                              */
   MMGSDI_GSM_SRV_VGCS,            /* VGCS Group Identifier List       */

   /* 220 */
   MMGSDI_GSM_SRV_VBS,             /* VBS Group Identifier List        */
   MMGSDI_GSM_SRV_ENH_ML_SVC,      /* Enhanced Multi Level precedence and Premption Service */
   MMGSDI_GSM_SRV_AA_EMLPP,        /* Automatic Answer for eMLPP       */
   MMGSDI_GSM_SRV_DATA_DL_SMSCB,   /* Data download via SMS-CB         */
   MMGSDI_GSM_SRV_DATA_DL_SMSPP,   /* Data download via SMS-PP         */
   MMGSDI_GSM_SRV_MENU_SEL,        /* Menu Selector                    */
   MMGSDI_GSM_SRV_CALL_CONTROL,    /* Call Control                     */
   MMGSDI_GSM_SRV_PROACTIVE,       /* Proactive SIM                    */
   MMGSDI_GSM_SRV_CBMIR,           /* Cell Broadcast Message Ranges */
   MMGSDI_GSM_SRV_BDN,             /* Barred Dialing Numbers           */

   /* 230 */
   MMGSDI_GSM_SRV_EXT4,            /* Extension 4*/
   MMGSDI_GSM_SRV_DCK,             /* Depersonalization Keys           */
   MMGSDI_GSM_SRV_CNL,             /* Co-operative Network List        */
   MMGSDI_GSM_SRV_SMSR,            /* Short Message Status Reports     */
   MMGSDI_GSM_SRV_NIA,             /* Network's indication of alerting in the MS */
   MMGSDI_GSM_SRV_MOSMS_CONTROL,   /* MO SMS control                   */
   MMGSDI_GSM_SRV_GPRS,            /* GPRS                             */
   MMGSDI_GSM_SRV_IMAGE,           /* Image (IMG)                      */
   MMGSDI_GSM_SRV_SOLSA,           /* Support of Local Service Area    */
   MMGSDI_GSM_SRV_USSD,            /* USSD String Data object supported in Call Control */

   /* 240 */
   MMGSDI_GSM_SRV_RUN_AT_CMD,      /* RUN AT COMMAND command           */
   MMGSDI_GSM_SRV_UPLMN_SEL_WACT,  /* User controlled PLMN Selector with Access Technology */
   MMGSDI_GSM_SRV_OPLMN_SEL_WACT,  /* Operator controlled PLMN Selector with Access Technology */
   MMGSDI_GSM_SRV_HPLMN_WACT,      /* HPLMN Selector with Access Technology */
   MMGSDI_GSM_SRV_CPBCCH,          /* CP BCCH                          */
   MMGSDI_GSM_SRV_INV_SCAN,        /* Investigation Scan               */
   MMGSDI_GSM_SRV_ECCP,            /* extended capability configuration param - in GSM */
   MMGSDI_GSM_SRV_MEXE,            /* MExE info                         */
   MMGSDI_GSM_SRV_RPLMN_LACT,      /* RPLMN Last used Access Technology */
   MMGSDI_GSM_SRV_PLMN_NTWRK_NAME, /* PLMN Network Name                 */

   /* 250 */
   MMGSDI_GSM_SRV_OPLMN_LIST,      /* Operator PLMN list                */
   MMGSDI_GSM_SRV_MDN,             /* Mailbox dialling numbers          */
   MMGSDI_GSM_SRV_MWI,             /* Message Wait indication           */
   MMGSDI_GSM_SRV_CFI,             /* Call Forward indication status    */
   MMGSDI_GSM_SRV_SPDI,            /* Service Provider Display Information */
   MMGSDI_GSM_SRV_MMS,             /* Multimedia Messaging Service      */
   MMGSDI_GSM_SRV_EXT8,            /* Extension 8 */
   MMGSDI_GSM_SRV_MMS_USR_P,       /* MMS User Connectivity Parameters  */

   /* 400 */
   MMGSDI_USIM_SRV_LOCAL_PHONEBOOK  = MMGSDI_USIM_SRV, /* Local Phonebook, UADF/Phonebook/ADN */
   MMGSDI_USIM_SRV_FDN,             /* Forbidden Dialling Numbers       */
   MMGSDI_USIM_SRV_EXT2,            /* Extension 2                      */
   MMGSDI_USIM_SRV_SDN,             /* Service Dialling Numbers         */
   MMGSDI_USIM_SRV_EXT3,            /* Extension 3                      */
   MMGSDI_USIM_SRV_BDN,             /* Barred Dialing Numbers           */
   MMGSDI_USIM_SRV_EXT4,            /* Extension 4                      */
   MMGSDI_USIM_SRV_OCI_OCT,         /* Outgoing Call information/timer  */
   MMGSDI_USIM_SRV_ICI_ICT,         /* Incoming call information/timer  */
   MMGSDI_USIM_SRV_SMS,             /* Short Message Service            */

   /* 40A OR 0x400 + 10 */
   MMGSDI_USIM_SRV_SMSR,            /* Short Message Status Reports     */
   MMGSDI_USIM_SRV_SMSP,            /* Short Message Service Parameters */
   MMGSDI_USIM_SRV_AOC,             /* Advice of Charge                 */
   MMGSDI_USIM_SRV_CCP,             /* Capability Configuration Parameters */
   MMGSDI_USIM_SRV_CBMI,            /* Cell Broadcast Message Identifier*/
   MMGSDI_USIM_SRV_CBMIR,           /* Cell Broadcast Message Ranges    */
   MMGSDI_USIM_SRV_GID1,            /* Group Identifier 1               */
   MMGSDI_USIM_SRV_GID2,            /* Group Identifier 2               */
   MMGSDI_USIM_SRV_SPN,             /* Service Provider Name            */
   MMGSDI_USIM_SRV_UPLMN_SEL_WACT,  /* User controlled PLMN Selector with Access Technology */

   /* 0x414 OR 0x400+20 */
   MMGSDI_USIM_SRV_MSISDN,          /* MSISDN                           */
   MMGSDI_USIM_SRV_IMAGE,           /* Image (IMG)                      */
   MMGSDI_USIM_SRV_SOLSA,           /* Support of Local Service Area    */
   MMGSDI_USIM_SRV_ENH_ML_SVC,      /* Enhanced Multi Level precedence and Premption Service */
   MMGSDI_USIM_SRV_AA_EMLPP,        /* Automatic Answer for eMLPP       */
   MMGSDI_USIM_SRV_RFU1,            /* RFU                              */
   MMGSDI_USIM_SRV_GSM_ACCESS,      /* USIM ADF Access to USIM files for GSM Roaming */
   MMGSDI_USIM_SRV_DATA_DL_SMSCB,   /* Data download via SMS-CB         */
   MMGSDI_USIM_SRV_DATA_DL_SMSPP,   /* Data download via SMS-PP         */
   MMGSDI_USIM_SRV_CALL_CONTROL,    /* Call Control                     */

   /* 0x41E OR 0x400+30 */
   MMGSDI_USIM_SRV_MOSMS_CONTROL,   /* MO SMS control                   */
   MMGSDI_USIM_SRV_RUN_AT_CMD,      /* RUN AT COMMAND command           */
   MMGSDI_USIM_SRV_SET_TO_1,        /* Shall be set to 1                */
   MMGSDI_USIM_SRV_EST,             /* Enable Service Table in USIM     */
   MMGSDI_USIM_SRV_ACL,             /* APN Control list                 */
   MMGSDI_USIM_SRV_DCK,             /* Depersonalization Keys           */
   MMGSDI_USIM_SRV_CNL,             /* Co-operative Network List        */
   MMGSDI_USIM_SRV_GSM_SECURITY_CONTEXT, /* GSM Security Context        */
   MMGSDI_USIM_SRV_CPBCCH,          /* CP BCCH                          */
   MMGSDI_USIM_SRV_INV_SCAN,        /* Investigation Scan               */

   /* 0x428 OR 0x400+40 */
   MMGSDI_USIM_SRV_MEXE,            /* MExE info                        */
   MMGSDI_USIM_SRV_OPLMN_SEL_WACT,  /* Operator controlled PLMN Selector with Access Technology */
   MMGSDI_USIM_SRV_HPLMN_WACT,      /* HPLMN Selector with Access Technology */
   MMGSDI_USIM_SRV_EXT5,            /* Ext 5                            */
   MMGSDI_USIM_SRV_PLMN_NTWRK_NAME, /* PLMN Network Name                */
   MMGSDI_USIM_SRV_OPLMN_LIST,      /* Operator PLMN list               */
   MMGSDI_USIM_SRV_MDN,             /* Mailbox dialling numbers         */
   MMGSDI_USIM_SRV_MWI,             /* Message Wait indication          */
   MMGSDI_USIM_SRV_CFI,             /* Call Forward indication status   */
   MMGSDI_USIM_SRV_RFU2,            /* Reserved and shall be ignored    */

   /* 0x432 OR 0x400+50 */
   MMGSDI_USIM_SRV_SPDI,            /* Service Provider Display Information */
   MMGSDI_USIM_SRV_RPLMN_LACT,      /* RPLMN Last used Access Technology */
   MMGSDI_USIM_SRV_MMS,             /* Multimedia Messaging Service     */
   MMGSDI_USIM_SRV_EXT8,            /* Extension 8                      */
   MMGSDI_USIM_SRV_GPRS_CC,         /* Call control on GPRS by USIM     */
   MMGSDI_USIM_SRV_MMS_USR_P,       /* MMS User Connectivity Parameters */
   MMGSDI_USIM_SRV_NIA,             /* Network's indication of alerting */
   MMGSDI_USIM_SRV_VGCS,            /* VGCS Group Identifier List       */
   MMGSDI_USIM_SRV_VBS,             /* VBS Group Identifier List        */
   MMGSDI_USIM_SRV_PSEUDONYM,       /* Pseudonym                        */

   /* 0x43C OR 0x400+60 */
   MMGSDI_USIM_SRV_UPLMN_WLAN_ACC,  /* User Controlled PLMNselector for WLAN access */
   MMGSDI_USIM_SRV_OPLMN_WLAN_ACC,  /* Operator Controlled PLMN selector for WLAN access */
   MMGSDI_USIM_SRV_USER_CTRL_WSID,  /* User controlled WSID list        */
   MMGSDI_USIM_SRV_OPER_CTRL_WSID,  /* Operator controlled WSID list    */
   MMGSDI_USIM_SRV_VGCS_SECURITY,   /* VGCS Security                    */
   MMGSDI_USIM_SRV_VBS_SECURITY,    /* VBS Security                     */
   MMGSDI_USIM_SRV_WLAN_REAUTH_ID,  /* WLAN Reauthentication Identity   */
   MMGSDI_USIM_SRV_MM_STORAGE,      /* Multimedia Messaging Storage     */
   MMGSDI_USIM_SRV_GBA,             /* Generic Bootstrapping Architecture */
   MMGSDI_USIM_SRV_MBMS_SECURITY,   /* MBMS security                    */

   /* 0x446 OR 0x400+70 */
   MMGSDI_USIM_SRV_DATA_DL_USSD,    /* Data Downlod via USSD and USSD application mode */
   MMGSDI_USIM_SRV_EHPLMN,          /* EHPLMN                           */
   MMGSDI_USIM_SRV_TP_AFTER_UICC_ACT, /* additional TERMINAL PROFILE after UICC activation */
   MMGSDI_USIM_SRV_EHPLMNI,         /* Equivalent HPLMN Presentation Indicator */
   MMGSDI_USIM_LAST_RPMN_SEL_IND,   /* Last RPLMN Indication Selection*/
   MMGSDI_USIM_SRV_OMA_BCAST,       /* OMA BCAST Srv Profile*/
   MMGSDI_USIM_SRV_GBA_KEY_EST,     /* GBA-based Local Key Establishment Mechanism*/
   MMGSDI_TERMINAL_APPS,            /* Terminal Applications*/
   MMGSDI_SRV_MAX           = 0x7FFFFFFF
}mmgsdi_service_enum_type;

/*===========================================================================
   ENUM: MMGSDI_AUTN_CONTEXT_ENUM_TYPE

   DESCRIPTION: The following enumeration will be used to indicate to uim
                which authentication context is requested by the client.
                Enums map directly to values in Spec 31.102
=============================================================================*/
typedef enum {
  MMGSDI_AUTN_USIM_GSM_CONTEXT                = 0x00,
  MMGSDI_AUTN_USIM_3G_CONTEXT                 = 0x10,
  MMGSDI_AUTN_USIM_VGCS_VBS_CONTEXT           = 0x20,
  MMGSDI_AUTN_USIM_GBA_BOOTSTRAPPING_CONTEXT  = 0x41,
  MMGSDI_AUTN_USIM_GBA_NAF_DERIVATION_CONTEXT = 0x42,
  MMGSDI_AUTN_USIM_MBMS_MSK_UPDATE_CONTEXT    = 0x51,
  MMGSDI_AUTN_USIM_MBMS_SPE_DEL_CONTEXT       = 0x511,
  MMGSDI_AUTN_USIM_MBMS_REC_DEL_CONTEXT       = 0x512,
  MMGSDI_AUTN_USIM_MBMS_MTK_GEN_CONTEXT       = 0x52,
  MMGSDI_AUTN_USIM_MBMS_MSK_DEL_CONTEXT       = 0x53,
  MMGSDI_AUTN_USIM_MBMS_MUK_DEL_CONTEXT       = 0x54,
  MMGSDI_AUTN_ISIM_CONTEXT                    = 0x60,
  MMGSDI_USIM_AUTN_MAX                        = 0x7FFFFFFF
}mmgsdi_autn_context_enum_type;

/*===========================================================================
   ENUM:      MMGSDI_JCDMA_CARD_INFO_ENUM_TYPE

   DESCRIPTION:
     Power-up UIM card information.

   MMGSDI_JCDMA_NON_JCDMA_CARD         Non JCDMA card
   MMGSDI_JCDMA_WHITE_CARD             White card, non-operational
   MMGSDI_JCDMA_GRAY_CARD              Gray card, need IOTA
   MMGSDI_JCDMA_GRAY_CARD_LINE_LOCKED  Gray card, line locked
   MMGSDI_JCDMA_BLACK_CARD             Black card, operational
   MMGSDI_JCDMA_BLACK_CARD_NEW_CARD    New black card, need UHM
   MMGSDI_JCDMA_BLACK_CARD_LINE_LOCKED Black card, line locked
   MMGSDI_JCDMA_BLACK_CARD_MW_LOCKED   Black card, network locked
   MMGSDI_JCDMA_BLACK_CARD_SP_LOCKED   Black card, service provide locked
   MMGSDI_JCDMA_BLACK_CARD_UG_LOCKED   Black card, user group locked
   MMGSDI_JCDMA_UNKNOWN_CARD           Invalid value
=============================================================================*/
typedef enum {
  MMGSDI_JCDMA_NON_JCDMA_CARD,
  MMGSDI_JCDMA_WHITE_CARD,
  MMGSDI_JCDMA_GRAY_CARD,
  MMGSDI_JCDMA_GRAY_CARD_LINE_LOCKED,
  MMGSDI_JCDMA_BLACK_CARD,
  MMGSDI_JCDMA_BLACK_CARD_NEW_CARD,
  MMGSDI_JCDMA_BLACK_CARD_LINE_LOCKED,
  MMGSDI_JCDMA_BLACK_CARD_NW_LOCKED,
  MMGSDI_JCDMA_BLACK_CARD_SP_LOCKED,
  MMGSDI_JCDMA_BLACK_CARD_UG_LOCKED,
  MMGSDI_JCDMA_UNKNOWN_CARD
}mmgsdi_jcdma_card_info_enum_type;

/*===========================================================================
   ENUM: MMGSDI_ESN_ME_CHANGE_ENUM_TYPE

   DESCRIPTION: The following enumeration will indicate if ESN ME had changed
                as defined in spec C.S0023-B
===========================================================================*/
typedef enum {
  MMGSDI_ESN_ME_NO_CHANGE      = 0,
  MMGSDI_ESN_ME_CHANGE         = 1,
  MMGSDI_ESN_ME_CHANGE_NOT_SUPPORTED = 2
}mmgsdi_esn_me_change_enum_type;

/*===========================================================================
   ENUM: MMGSDI_MFLO_UCAST_MSG_ENUM_TYPE

   DESCRIPTION: The following enumeration will indicate the MediaFlo Unicast
                message block type
===========================================================================*/
typedef enum {
  MMGSDI_MFLO_UCAST_MSG_FIRST_BLOCK  = 0x00,
  MMGSDI_MFLO_UCAST_MSG_NEXT_BLOCK   = 0x01,
  MMGSDI_MFLO_UCAST_MSG_LAST_BLOCK   = 0x10,
  MMGSDI_MFLO_UCAST_MSG_SINGLE_BLOCK = 0x11
  // Any other value is invalid
}mmgsdi_mflo_ucast_msg_enum_type;


/*===========================================================================
   ENUM: MMGSDI_BCAST_MODE_ENUM_TYPE

   DESCRIPTION: The following enumeration will indicate the OMA BCAST
                mode type
===========================================================================*/
typedef enum {
  MMGSDI_BCAST_SPE_AUDIT_MODE,
  MMGSDI_BCAST_SPE_SIG_MODE,
  MMGSDI_BCAST_REC_AUDIT_MODE,
  MMGSDI_BCAST_MODE_MAX = 0x7FFFFFFF
}mmgsdi_bcast_mode_enum_type;

/*===========================================================================
   ENUM: MMGSDI_SEARCH_ENUM_TYPE

   DESCRIPTION: The following enumeration will indicate the search type.
   MMGSDI_ICC_SEARCH for seek command on icc card
   MMGSDI_UICC_SIMPLE_SEARCH for simple search record on uicc card.
   MMGSDI_UICC_ENHANCED_SEARCH for enhanced search record on uicc card.
===========================================================================*/
typedef enum {
  MMGSDI_SEARCH_NONE           = 0x00000000,
  MMGSDI_ICC_SEARCH,
  MMGSDI_UICC_SIMPLE_SEARCH,
  MMGSDI_UICC_ENHANCED_SEARCH,
  MMGSDI_MAX_SEARCH_ENUM       = 0x7FFFFFFF
  // Any other value is invalid
}mmgsdi_search_enum_type;

/*===========================================================================
  ENUM:      MMGSDI_SEEK_DIRECTION_ENUM_TYPE

  DESCRIPTION:The following enumeration will indicate the search
  direction for in a EF for icc card
    MMGSDI_SEEK_BEGINNING_FORWARDS : Start from first record and move
                                     forward.
    MMGSDI_SEEK_END_BACKWARDS      : Start from last record and move backward.
===========================================================================*/
typedef enum {
  MMGSDI_SEEK_DIRECTION_NONE         = 0x00000000,
  MMGSDI_SEEK_BEGINNING_FORWARDS,
  MMGSDI_SEEK_END_BACKWARDS,
  MMGSDI_MAX_SEEK_DIRECTION_ENUM     = 0x7FFFFFFF
}mmgsdi_seek_direction_enum_type;

/*===========================================================================
  ENUM:      MMGSDI_SEARCH_DIRECTION_ENUM_TYPE

  DESCRIPTION: the following enumeration will indicate the search
  direction in a EF for UICC card.
    MMGSDI_SEARCH_FORWARD_FROM_REC_NUM           : Start forward from given
                                                   record number.
    MMGSDI_SEARCH_BACKWARD_FROM_REC_NUM          : Start backward from give
                                                   record number.
  ===========================================================================*/
typedef enum {
  MMGSDI_SEARCH_DIRECTION_NONE           = 0x00000000,
  MMGSDI_SEARCH_FORWARD_FROM_REC_NUM,
  MMGSDI_SEARCH_BACKWARD_FROM_REC_NUM,
  MMGSDI_MAX_SEARCH_DIRECTION_ENUM       = 0x7FFFFFFF
}mmgsdi_search_direction_enum_type;

/*===========================================================================
  ENUM:      MMGSDI_SEARCH_OPTION_ENUM_TYPE

  DESCRIPTION: the following enumeration will indicate search type in a
  record for uicc card.
    MMGSDI_SEARCH_BY_OFFSET : Searching of the pattern in a record that
                              start from given offset.
    MMGSDI_SEARCH_BY_CHAR   : Searching of the pattern in a record that
                              starts with a character value(one byte
                              information).
  ===========================================================================*/
typedef enum {
   MMGSDI_SEARCH_OPTION_NONE     = 0x00000000,
   MMGSDI_SEARCH_BY_OFFSET,
   MMGSDI_SEARCH_BY_CHAR,
   MMGSDI_MAX_SEARCH_OPTION_ENUM = 0x7FFFFFFF
}mmgsdi_search_option_enum_type;

/*=============================================================================

                       STRUCTURE DECLARATIONS

=============================================================================*/


/* ----------------------------------------------------------------------------
   STRUCTURE:      MMGSDI_CLIENT_ID_TYPE

   DESCRIPTION:   The client ID that client of new MMGSDI API is required to
                  obtained from the MMGSDI Task.  This Client ID is obtained
                  by using the Library Function:  mmgsdi_register_clinet()
-------------------------------------------------------------------------------*/
typedef uint64 mmgsdi_client_id_type;

/* ----------------------------------------------------------------------------
   STRUCTURE:      MMGSDI_LEN_TYPE

   DESCRIPTION:   General length type definition
-------------------------------------------------------------------------------*/
typedef int32 mmgsdi_len_type;

/* ----------------------------------------------------------------------------
   STRUCTURE:      MMGSDI_CLIENT_DATA_TYPE

   DESCRIPTION:   User Data to Client Information.  This pointer will be
                  accepted as part of the API and returned to the client.  A
                  NULL is ok.  This is received as a void star to allow the
                  client to a pointer to any type of data structures.
-------------------------------------------------------------------------------*/
typedef uint32 mmgsdi_client_data_type;


/* ----------------------------------------------------------------------------
   STRUCTURE:      MMGSDI_PTR_TYPE

   DESCRIPTION:    The generic pointer type.
  ---------------------------------------------------------------------------*/
typedef uint32 mmgsdi_ptr_type;

/* -----------------------------------------------------------------------------
   STRUCTURE:      MMGSDI_DATA_TYPE

   DESCRIPTION:  The generic data structure.
     data_len:    Length of data
     data_ptr:    Data
-------------------------------------------------------------------------------*/
typedef struct {
  mmgsdi_len_type    data_len;
  uint8            * data_ptr;
  /*~ FIELD mmgsdi_data_type.data_ptr VARRAY LENGTH mmgsdi_data_type.data_len */
} mmgsdi_data_type;

/* ----------------------------------------------------------------------------
   STRUCTURE:      MMGSDI_APP_TYPE

   DESCRIPTION:
     The structure contains the application info
-------------------------------------------------------------------------------*/
typedef struct {
  boolean gsm_app;
  boolean usim_app;
  boolean cdma_app;
} mmgsdi_apps_type;

/* ----------------------------------------------------------------------------
   STRUCTURE:      MMGSDI_PIN_INFO_TYPE

   DESCRIPTION:
     This structure is used when returning all the PIN status
     response_header:   Response data and status of GET ALL PIN STATUS REQ
     pin1:              State of the PIN1.
     pin2:              State of the PIN2.
-------------------------------------------------------------------------------*/
typedef struct {
  mmgsdi_pin_enum_type         pin_id;
  mmgsdi_pin_status_enum_type  status;
  mmgsdi_pin_replace_enum_type pin_replacement;
  uint32                       num_retries;
  uint32                       num_unblock_retries;
} mmgsdi_pin_info_type;

typedef struct{
  mmgsdi_len_type    data_len;
  uint8              data_ptr[MMGSDI_MAX_AID_LEN];
}mmgsdi_static_data_type;

/* ----------------------------------------------------------------------------
   STRUCTURE:      MMGSDI_AID_TYPE

   DESCRIPTION:
     The structure contains the AID information.
-------------------------------------------------------------------------------*/
typedef struct {
  mmgsdi_app_enum_type       app_type;
  mmgsdi_static_data_type    aid;
  mmgsdi_static_data_type    label;
} mmgsdi_aid_type;

/* ----------------------------------------------------------------------------
   STRUCTURE:      MMGSDI_APP_INFO_TYPE

   DESCRIPTION:
     The structure contains all app information.
-------------------------------------------------------------------------------*/
typedef struct{
  boolean                cached;
  mmgsdi_pin_info_type   pin1;
  mmgsdi_pin_info_type   pin2;
  mmgsdi_pin_info_type   universal_pin;
  mmgsdi_aid_type        app_data;
  boolean                prov_app;
}mmgsdi_app_info_type;

/* ----------------------------------------------------------------------------
   STRUCTURE:      MMGSDI_OPTION_TYPE

   DESCRIPTION:
     The structure contains the optional parameter(s) info.
     Individual API's optional parameter(s) is(are) defined separately.
     This structure is used as the place holder for these optional parameters
     for all APIs
     tlv_num:   The number of TLVs in the tlvs_ptrs member variable
     tlvs_ptr:  [Tag: size uint32] [Length: size uint32] [variable structure]
-------------------------------------------------------------------------------*/
typedef struct {
  uint32             tlv_num;
  void              *tlvs_ptr;
} mmgsdi_option_type;

/* ----------------------------------------------------------------------------
   STRUCTURE:      MMGSDI_PATH_TYPE

   DESCRIPTION:  This path type provides the entire path for access starting
                 from MF (3F00) to the file or DF or ADF of interest
                 Examples:
                 MF                   -> [3F00]
                 Current ADF          -> [3F00][7FFF]
                 GSM DF               -> [3F00][7F20]
                 EF under MF          -> [3F00][EF]
                 EF under current ADF -> [3F00[7FFF][EF]
                 EF under Level 1 DF  -> [3F00][DF1][EF]
                 EF under Level 2 DF  -> [3F00][DF1][DF2][EF]
                 Limitation: Cannot access DFs or EFs under non current ADF
                 without the ADF being activated prior to sending down the path
  ---------------------------------------------------------------------------*/
typedef struct {
  mmgsdi_len_type       path_len;
  uint16                path_buf[MMGSDI_MAX_PATH_LEN];
} mmgsdi_path_type;


/* ----------------------------------------------------------------------------
   STRUCTURE:      MMGSDI_REFRESH_FILE_LIST_TYPE

   DESCRIPTION:    The structure has the information for the files that have been
                   Refreshed or the list of files that the client is interested
                   in receiving notification for
                   From file_list_ptr and file_path_ptr only one of the pointers
                   shall point to valid memory location and other pointer shall 
                   be null. Variable num_files shall be indicate for the number
                   of files enums in file_list_ptr and variable file_path_len
                   will indicate the length of bytes for file_path_ptr.
-------------------------------------------------------------------------------*/
typedef struct {
   uint32                   num_files;
   mmgsdi_file_enum_type   *file_list_ptr;
   uint8                   *file_path_ptr;
   uint32                   file_path_len;
  /*~ FIELD mmgsdi_refresh_file_list_type.file_list_ptr VARRAY LENGTH mmgsdi_refresh_file_list_type.num_files */
  /*~ FIELD mmgsdi_refresh_file_list_type.file_path_ptr VARRAY LENGTH mmgsdi_refresh_file_list_type.file_path_len */
}mmgsdi_refresh_file_list_type;

/* ----------------------------------------------------------------------------
   STRUCTURE:      MMGSDI_SW_TYPE

   DESCRIPTION:
     This structure contains the Status Word returned from the card.  Only
     certain commands will have this set explicitly at the moment:
     mmgsdi_read_cnf_type
     mmgsdi_write_cnf_type
     mmgsdi_status_cnf_type (ICC)
     For other cnf type, currently "present" == FALSE
     For any error generated internally by MMGSDI or UIM during the command
     processing, the "present" field is set to FALSE
     status word is coded as per 51.011 SIM and 102.221 UICC and 3gpp2 RUIM
     spec

     present:     Whether sw1 and sw2 members should be looked at or not
     sw1:         Status word 1
     sw2:         Status word 2
-------------------------------------------------------------------------------*/
typedef struct {
  boolean                  present;
  uint8                    sw1;
  uint8                    sw2;
} mmgsdi_sw_type;

/* ----------------------------------------------------------------------------
   STRUCTURE:      MMGSDI_RESPONSE_HEADER_TYPE

   DESCRIPTION:
     This structure contains the Response Header Information used to direct
     To notify the client.
     client_id:     Client ID
     response_type: Response Command Type
     response_len:  Length of message + header
     payload_len:   Length of the message payload: this includes the size of
                    the structure without the header as well as the size of
                    any pointer data being allocated.
                    Note: The additional data pointer being allocated is not
                    a continuous memory space
     slot_id:       Slot that the request is to be performed on
     client_data:   Client data that was being passed in earlier in the request
     mmgsdi_status: Return status of the request
-------------------------------------------------------------------------------*/
typedef struct {
  mmgsdi_client_id_type     client_id;
  mmgsdi_cnf_enum_type      response_type;
  mmgsdi_len_type           response_len;
  mmgsdi_len_type           payload_len;
  mmgsdi_slot_id_enum_type  slot_id;
  mmgsdi_client_data_type   client_data;
  mmgsdi_return_enum_type   mmgsdi_status;
  mmgsdi_sw_type            status_word;
} mmgsdi_response_header_type;


/* ----------------------------------------------------------------------------
   STRUCTURE:      MMGSDI_CARD_ERR_EVT_INFO_TYPE

   DESCRIPTION:
     This structure contains the information for card error event.
-------------------------------------------------------------------------------*/
typedef struct {
  mmgsdi_slot_id_enum_type       slot;
  mmgsdi_card_err_info_enum_type info;
} mmgsdi_card_err_evt_info_type;


/* ----------------------------------------------------------------------------
   STRUCTURE:      MMGSDI_CARD_REMOVED_EVT_INFO_TYPE

   DESCRIPTION:
     This structure contains the information for card removed event.
-------------------------------------------------------------------------------*/
typedef struct {
  mmgsdi_slot_id_enum_type slot;
} mmgsdi_card_removed_evt_info_type;

/* ----------------------------------------------------------------------------
   STRUCTURE:      MMGSDI_CARD_INSERTED_EVT_INFO_TYPE

   DESCRIPTION:
       This structure contains the information for card inserted event.
       num_aids_avail -   Only applicable if the avail_apps has usim_app in it
       aid_info       -   Only applicable if the avail_apps has usim_app in it

-------------------------------------------------------------------------------*/
typedef struct {
  mmgsdi_slot_id_enum_type      slot;
  uint32                        num_aids_avail;
  mmgsdi_aid_type               aid_info[MMGSDI_MAX_APP_INFO];
} mmgsdi_card_inserted_evt_info_type;

/* ----------------------------------------------------------------------------
   STRUCTURE:      MMGSDI_SESSION_CLOSE_EVT_INFO_TYPE

   DESCRIPTION:
     This structure contains the information for session close event.
-------------------------------------------------------------------------------*/
typedef struct {
  mmgsdi_slot_id_enum_type slot;
  mmgsdi_client_id_type    session_id;
  mmgsdi_data_type         app_id;
} mmgsdi_session_close_evt_info_type;

/* ----------------------------------------------------------------------------
   STRUCTURE:      MMGSDI_CARD_INIT_COMPLETED_EVT_INFO_TYPE

   DESCRIPTION:
     This structure contains the information for card initialization completed
     event.
-------------------------------------------------------------------------------*/
typedef struct {
  mmgsdi_slot_id_enum_type  slot;
  uint32                    num_avail_apps;
  mmgsdi_app_info_type      app_info[MMGSDI_MAX_APP_INFO];
} mmgsdi_card_init_completed_evt_info_type;

/* ----------------------------------------------------------------------------
   STRUCTURE:      MMGSDI_SWITCH_SLOT_EVT_INFO_TYPE

   DESCRIPTION:
     This structure contains the information for switch slot event.
-------------------------------------------------------------------------------*/
typedef struct {
  boolean   start;
  // later: from which slot to which, which tech...
} mmgsdi_switch_slot_evt_info_type;


/* ----------------------------------------------------------------------------
   STRUCTURE:      MMGSDI_FDN_EVT_INFO_TYPE

   DESCRIPTION:
     This structure contains the information for FDN event.
-------------------------------------------------------------------------------*/
typedef struct {
  mmgsdi_slot_id_enum_type slot;
  boolean                  enabled;
} mmgsdi_fdn_evt_info_type;

/* ----------------------------------------------------------------------------
   STRUCTURE:      MMGSDI_ILLEGAL_CARD_EVT_INFO_TYPE

   DESCRIPTION:
     This structure contains the information for illegal card event.
-------------------------------------------------------------------------------*/
typedef struct {
  mmgsdi_slot_id_enum_type slot;
} mmgsdi_illegal_card_evt_info_type;


/* ----------------------------------------------------------------------------
   STRUCTURE:      MMGSDI_REFRESH_EVT_INFO_TYPE

   DESCRIPTION:
     This structure contains refresh event information.
-------------------------------------------------------------------------------*/
typedef struct {
  mmgsdi_slot_id_enum_type       slot;
  mmgsdi_refresh_stage_enum_type stage;
  mmgsdi_refresh_mode_enum_type  mode;
  mmgsdi_aid_type                aid;
  mmgsdi_refresh_file_list_type  refresh_files;
  mmgsdi_refresh_mode_enum_type  orig_mode;
} mmgsdi_refresh_evt_info_type;

/* ----------------------------------------------------------------------------
   STRUCTURE:      MMGSDI_TP_DL_EVT_INFO_TYPE

   DESCRIPTION:
     This structure contains the terminal profile download event information.
-------------------------------------------------------------------------------*/
typedef struct {
  mmgsdi_slot_id_enum_type      slot;
  boolean                       ok_dl;
} mmgsdi_tp_dl_evt_info_type;


/* ----------------------------------------------------------------------------
   STRUCTURE:      MMGSDI_PIN_EVT_INFO_TYPE

   DESCRIPTION:
     This structure contains the information for pin event.
-------------------------------------------------------------------------------*/
typedef struct {
  mmgsdi_slot_id_enum_type      slot;
  uint32                        num_aids;
  mmgsdi_pin_info_type          pin_info;
  mmgsdi_aid_type               aid_type[MMGSDI_MAX_APP_INFO];
} mmgsdi_pin_evt_info_type;

/* ----------------------------------------------------------------------------
   STRUCTURE:      MMGSDI_SAP_CONNECT_EVT_INFO_TYPE

   DESCRIPTION:
     This structure contains the information for sap connect event.
-------------------------------------------------------------------------------*/
typedef struct {
  mmgsdi_slot_id_enum_type      slot;
  mmgsdi_sap_state_enum_type    connect_state;
  mmgsdi_sap_mode_enum_type     sap_mode;
  mmgsdi_sap_mode_enum_type     sap_connect_initiate_by;
} mmgsdi_sap_connect_evt_info_type;

/* ----------------------------------------------------------------------------
   STRUCTURE:      MMGSDI_SAP_DISCONNECT_EVT_INFO_TYPE

   DESCRIPTION:
     This structure contains the information for sap disconnect event.
-------------------------------------------------------------------------------*/
typedef struct {
  mmgsdi_slot_id_enum_type         slot;
  mmgsdi_sap_state_enum_type       disconnect_state;
  mmgsdi_sap_mode_enum_type        sap_mode;
  mmgsdi_sap_mode_enum_type        sap_disconnect_initiate_by;
  mmgsdi_disconnect_mode_enum_type sap_disconnect_type;
} mmgsdi_sap_disconnect_evt_info_type;

#define MMGSDIBT_BD_ADDR_LEN 6
typedef struct {
  uint8 addr[MMGSDIBT_BD_ADDR_LEN];
} mmgsdi_bt_bd_address_type;

/* ----------------------------------------------------------------------------
   STRUCTURE:      MMGSDI_SAP_PIN_EVT_INFO_TYPE

   DESCRIPTION:
     This structure contains the information for sap pin request event.
-------------------------------------------------------------------------------*/
typedef struct {
  mmgsdi_pin_status_enum_type   status;
} mmgsdi_sap_pin_evt_info_type;


/* ----------------------------------------------------------------------------
   STRUCTURE:      MMGSDI_SELECT_AID_EVT_INFO_TYPE

   DESCRIPTION:
     This structure contains the information for sap pin request event.
-------------------------------------------------------------------------------*/
typedef struct {
  mmgsdi_slot_id_enum_type     slot;
  int32                        channel_id;
  mmgsdi_aid_type              app_data;
} mmgsdi_select_aid_evt_info_type;

/* ----------------------------------------------------------------------------
   STRUCTURE:      MMGSDI_EVENT_DATA_TYPE

   DESCRIPTION:
     This structure contains the event to be notified.
     And a union structure that contains the corresponding information for
     the event.
-------------------------------------------------------------------------------*/
typedef struct {
  mmgsdi_events_enum_type  evt;
  mmgsdi_client_id_type    client_id;
  union {
    mmgsdi_pin_evt_info_type                 pin;
    mmgsdi_tp_dl_evt_info_type               terminal_profile;
    mmgsdi_refresh_evt_info_type             refresh;
    mmgsdi_fdn_evt_info_type                 fdn;
    mmgsdi_switch_slot_evt_info_type         switch_slot;
    mmgsdi_card_init_completed_evt_info_type card_init_completed;
    mmgsdi_card_inserted_evt_info_type       card_inserted;
    mmgsdi_card_err_evt_info_type            card_error;
    mmgsdi_card_removed_evt_info_type        card_removed;
    mmgsdi_illegal_card_evt_info_type        illegal_card;
    mmgsdi_sap_connect_evt_info_type         sap_connect;
    mmgsdi_sap_disconnect_evt_info_type      sap_disconnect;
    mmgsdi_sap_pin_evt_info_type             sap_pin;
    mmgsdi_session_close_evt_info_type       session_close;
    mmgsdi_select_aid_evt_info_type          select_aid;
    /*~ IF (_DISC_ == MMGSDI_PIN1_EVT || _DISC_ == MMGSDI_PIN2_EVT || _DISC_== MMGSDI_UNIVERSAL_PIN_EVT ) mmgsdi_event_data_type.data.pin */
    /*~ CASE MMGSDI_TERMINAL_PROFILE_DL_EVT mmgsdi_event_data_type.data.terminal_profile */
    /*~ CASE MMGSDI_REFRESH_EVT mmgsdi_event_data_type.data.refresh */
    /*~ CASE MMGSDI_FDN_EVT mmgsdi_event_data_type.data.fdn */
    /*~ CASE MMGSDI_SWITCH_SLOT_EVT mmgsdi_event_data_type.data.switch_slot */
    /*~ CASE MMGSDI_CARD_INIT_COMPLETED_EVT mmgsdi_event_data_type.data.card_init_completed */
    /*~ CASE MMGSDI_CARD_INSERTED_EVT mmgsdi_event_data_type.data.card_inserted */
    /*~ CASE MMGSDI_CARD_ERROR_EVT mmgsdi_event_data_type.data.card_error */
    /*~ CASE MMGSDI_CARD_REMOVED_EVT mmgsdi_event_data_type.data.card_removed */
    /*~ CASE MMGSDI_ILLEGAL_CARD_EVT mmgsdi_event_data_type.data.illegal_card */
    /*~ CASE MMGSDI_SAP_CONNECT_EVT mmgsdi_event_data_type.data.sap_connect */
    /*~ CASE MMGSDI_SAP_DISCONNECT_EVT mmgsdi_event_data_type.data.sap_disconnect */
    /*~ CASE MMGSDI_SAP_PIN_EVT mmgsdi_event_data_type.data.sap_pin */
    /*~ CASE MMGSDI_SESSION_CLOSE_EVT mmgsdi_event_data_type.data.session_close */
    /*~ CASE MMGSDI_SELECT_AID_EVT mmgsdi_event_data_type.data.select_aid */
  } data;
/*~ FIELD mmgsdi_event_data_type.data DISC mmgsdi_event_data_type.evt */
} mmgsdi_event_data_type;


typedef void mmgsdi_evt_callback_type (const mmgsdi_event_data_type *event);
/*~ PARAM event POINTER */

/* ----------------------------------------------------------------------------
   STRUCTURE:      MMGSDI_ACCESS_TYPE

   DESCRIPTION:  This file indicates which EF/DF/MF/ADF or PATH the client
                 would like to access.
                 access_method: Indicated how the  union will be
                                interpreted
                 file_enum:     access_method == MMGSDI_EF_ENUM_ACCESS
                                (use of file enum for access)
                 df_enum:       access_method == MMGSDI_DF_ENUM_ACCESS
                                (used for accessing MF/DF/ADF)
                 path_type:     access_method == MMGSDI_BY_PATH_ACCESS
                 app_id:        access_method == MMGSDI_BY_APP_ID_ACCESS
  ---------------------------------------------------------------------------*/
typedef struct {
  mmgsdi_access_method_enum_type   access_method;
  union {
    mmgsdi_file_enum_type       file_enum;
    mmgsdi_df_enum_type         df_enum;
    mmgsdi_path_type            path_type;
    mmgsdi_static_data_type     app_id;
    /*~ CASE MMGSDI_EF_ENUM_ACCESS mmgsdi_access_type.file.file_enum */
    /*~ CASE MMGSDI_DF_ENUM_ACCESS mmgsdi_access_type.file.df_enum */
    /*~ CASE MMGSDI_BY_PATH_ACCESS mmgsdi_access_type.file.path_type */
    /*~ CASE MMGSDI_BY_APP_ID_ACCESS mmgsdi_access_type.file.app_id */
  } file;
/*~ FIELD mmgsdi_access_type.file DISC mmgsdi_access_type.access_method */
} mmgsdi_access_type;

/* ----------------------------------------------------------------------------
   STRUCTURE:      MMGSDI_REC_NUM_TYPE

   DESCRIPTION:  The record number for LINEAR FIXED and CYCLIC Files
-------------------------------------------------------------------------------*/
typedef int32 mmgsdi_rec_num_type;

/* ----------------------------------------------------------------------------
   STRUCTURE:      MMGSDI_OFFSET_TYPE

   DESCRIPTION:  The offset that allows the client to recieve data from any
                 part of the data up to the end of the data.  Only applicable
                 to TRANSPARENT Files.
-------------------------------------------------------------------------------*/
typedef int32 mmgsdi_offset_type;

/* ----------------------------------------------------------------------------
   STRUCTURE:      MMGSDI_FILE_SIZE_TYPE

   DESCRIPTION:  Indication of the file size
-------------------------------------------------------------------------------*/
typedef int32 mmgsdi_file_size_type;

/* ----------------------------------------------------------------------------
   STRUCTURE:      MMGSDI_FILE_ID_TYPE

   DESCRIPTION:  The 2 bytes file ID value from the Card
-------------------------------------------------------------------------------*/
typedef uint8  mmgsdi_file_id_type[MMGSDI_FILE_ID_SIZE];

/* ----------------------------------------------------------------------------
   STRUCTURE:      MMGSDI_CAVE_RAND_TYPE_TYPE

   DESCRIPTION:  Rand Type for RUN CAVE Command
-------------------------------------------------------------------------------*/
typedef uint8 mmgsdi_cave_rand_type_type;

/* ----------------------------------------------------------------------------
   STRUCTURE:      MMGSDI_CAVE_RAND_CHAL_TYPE

   DESCRIPTION:  Rand Chal for RUN CAVE Command
-------------------------------------------------------------------------------*/
typedef dword mmgsdi_cave_rand_chal_type;

/* ----------------------------------------------------------------------------
   STRUCTURE:      MMGSDI_CAVE_DIG_LEN_TYPE

   DESCRIPTION:  Digit Length for RUN CAVE Command
-------------------------------------------------------------------------------*/
typedef uint8 mmgsdi_cave_dig_len_type;

/* ----------------------------------------------------------------------------
   STRUCTURE:      MMGSDI_CAVE_DIGITS_TYPE

   DESCRIPTION:  Digits for RUN CAVE Command
-------------------------------------------------------------------------------*/
#define MMGSDI_CAVE_DIGITS_SIZE 3
typedef uint8 mmgsdi_cave_digits_type[MMGSDI_CAVE_DIGITS_SIZE];

/* ----------------------------------------------------------------------------
   STRUCTURE:      MMGSDI_CAVE_PROCSS_CONTROL_TYPE

   DESCRIPTION:  Process Control for RUN CAVE Command
-------------------------------------------------------------------------------*/
typedef uint8 mmgsdi_cave_process_control_type;

/* ----------------------------------------------------------------------------
   STRUCTURE:      MMGSDI_CAVE_ESN_TYPE

   DESCRIPTION:  ESN for RUN CAVE Command
                 Type qword is a two element array with each of size four bytes.
                 Specfic copy and assignment functions are used from qw.h Keep
                 this in consideration if changing data type of
                 mmgsdi_cave_esn_type
-------------------------------------------------------------------------------*/
typedef qword mmgsdi_cave_esn_type;

/* ----------------------------------------------------------------------------
   STRUCTURE:      MMGSDI_CAVE_AUTH_RESP_TYPE

   DESCRIPTION:  AUTH Response for RUN CAVE Command
-------------------------------------------------------------------------------*/
typedef dword mmgsdi_cave_auth_resp_type;

/* ----------------------------------------------------------------------------
   STRUCTURE:      MMGSDI_VPM_OCTET_TYPE

   DESCRIPTION:  Octet of VPM Data for GENERATE KEY / VPM
-------------------------------------------------------------------------------*/
typedef uint8 mmgsdi_single_vpm_octet_type;

/* ----------------------------------------------------------------------------
   STRUCTURE:      MMGSDI_KEY_TYPE

   DESCRIPTION:  Key response for  GENERATE KEY / VPM commnad
-------------------------------------------------------------------------------*/
typedef uint8 mmgsdi_key_type[MMGSDI_KEY_SIZE];

/* ----------------------------------------------------------------------------
   STRUCTURE:      MMGSDI_DATA_TYPEs

   DESCRIPTION:  The data to be written to the card.
-------------------------------------------------------------------------------*/
typedef mmgsdi_data_type mmgsdi_write_data_type;
typedef mmgsdi_data_type mmgsdi_read_data_type;
typedef mmgsdi_data_type mmgsdi_get_file_attr_data_type;
typedef mmgsdi_data_type mmgsdi_send_apdu_data_type;
typedef mmgsdi_data_type mmgsdi_sap_get_atr_data_type;
typedef mmgsdi_data_type mmgsdi_sap_send_apdu_data_type;
typedef mmgsdi_data_type mmgsdi_sap_card_reader_status_data_type;
typedef mmgsdi_data_type mmgsdi_status_data_type;
typedef mmgsdi_data_type mmgsdi_onchip_sim_config_data_type;
typedef mmgsdi_data_type mmgsdi_cmpt_ip_response_data_type;
typedef mmgsdi_data_type mmgsdi_atr_data_type;
typedef mmgsdi_data_type mmgsdi_vpm_octets_type;
typedef mmgsdi_data_type mmgsdi_seek_data_type;
typedef mmgsdi_data_type mmgsdi_search_data_type;

/* ----------------------------------------------------------------------------
   STRUCTURE:      MMGSDI_STATIC_DATA_TYPEs

   DESCRIPTION:  The data declared as static data type.
-------------------------------------------------------------------------------*/
typedef mmgsdi_static_data_type mmgsdi_card_reader_status_data_type;

/* ----------------------------------------------------------------------------
   STRUCTURE:      MMGSDI_FILE_SECURITY_TYPE

   DESCRIPTION:   The file security access information.
     protection_method == MMGSDI_ALWAYS_ALLOWED, => num_protection_pin = 0,
                                                    protection_pin = NULL
     protection_method == MMGSDI_NEVER_ALLOWED,  => num_protection_pin = 0,
                                                    protection_pin = NULL
     protection_method == MMGSDI_AND_ALLOWED,    => more than 1 pin is required
     protection_method == MMGSDI_OR_ALLOWED,     => 1 of the multiple pin is
                                                    required
     protection_method == MMGSDI_SINGLE_ALLOWED, => only 1 pin is required
-------------------------------------------------------------------------------*/
typedef struct {
  mmgsdi_file_security_enum_type   protection_method;
  int32                            num_protection_pin;
  mmgsdi_pin_enum_type           * protection_pin_ptr;
  /*~ FIELD mmgsdi_file_security_type.protection_pin_ptr VARRAY LENGTH mmgsdi_file_security_type.num_protection_pin */
} mmgsdi_file_security_type;

/* --------------------------------------------------------------------------
   STRUCTURE:    MMGSDI_SECURITY_ACCESS_TYPE

   DESCRIPTION:  The file security access information for different operation
     read:                  Read security requirements
     write:                 Write security requirements
     increase:              Increase security requirements
     invalidate_deactivate: Invalidated/Deactivate security requirements
     rehabilitate_activate: Rehabilitate or Activate security requirement
  ---------------------------------------------------------------------------*/
typedef struct {
  mmgsdi_file_security_type read;
  mmgsdi_file_security_type write;
  mmgsdi_file_security_type increase;
  mmgsdi_file_security_type invalidate_deactivate;
  mmgsdi_file_security_type rehabilitate_activate;
} mmgsdi_file_security_access_type;

/* ----------------------------------------------------------------------------
   STRUCTURE:      MMGSDI_FILE_STATUS_TYPE

   DESCRIPTION:    The file status information
     file_invalidated:             Whether the file is Invalidated or not
     read_write_when_invalidated:  Indicate if Read / Write operation is allowed
                                   or not when the file is invalidated
-------------------------------------------------------------------------------*/
typedef struct {
  boolean   file_invalidated;
  boolean   read_write_when_invalidated;
} mmgsdi_file_status_type;

/* ----------------------------------------------------------------------------
   STRUCTURE:      MMGSDI_LINEAR_FIXED_FILE_INFO_TYPE

   DESCRIPTION:    The file attributes data for LINEAR FIXED file
     file_security:    Security Access Condition for the file
     num_of_rec:       Number of record in the file
     rec_len:          Length of each record
     file_status:      Status of the file
-------------------------------------------------------------------------------*/
typedef struct {
  mmgsdi_file_security_access_type  file_security;
  mmgsdi_rec_num_type               num_of_rec;
  mmgsdi_len_type                   rec_len;
  mmgsdi_file_status_type           file_status;
} mmgsdi_linear_fixed_file_info_type;

/* ----------------------------------------------------------------------------
   STRUCTURE:      MMGSDI_CYCLIC_FILE_INFO_TYPE

   DESCRIPTION:    The file attributes data for CYCLIC file
     file_security:    Security Access Condition for the file
     num_of_rec:       Number of record in the file
     rec_len:          Length of each record
     file_status:      Status of the file
     increase_allowed: Increase Operation is allowed or not
-------------------------------------------------------------------------------*/
typedef struct {
  mmgsdi_file_security_access_type  file_security;
  mmgsdi_rec_num_type               num_of_rec;
  mmgsdi_len_type                   rec_len;
  mmgsdi_file_status_type           file_status;
  boolean                           increase_allowed;
} mmgsdi_cyclic_file_info_type;

/* ----------------------------------------------------------------------------
   STRUCTURE:      MMGSDI_TRANSPARENT_FILE_INFO_TYPE

   DESCRIPTION:
     The file attributes data for TRANSPARENT file
     file_security:  Security Access Condition for the file
     file_status:    Status of the file
-------------------------------------------------------------------------------*/
typedef struct {
  mmgsdi_file_security_access_type  file_security;
  mmgsdi_file_status_type           file_status;
} mmgsdi_transparent_file_info_type;

/* ----------------------------------------------------------------------------
   STRUCTURE:      MMGSDI_FILE_ATTRIBUTES_TYPE

   DESCRIPTION:
     The file attributes data as a result of a get file attributes request
     file_size:    Indicates the total size of the file (MF, DF or ER)
     file_id:      ID used in the SIM/USIM/RUIM
     file_type:    Indicates which file_info type should be used

     transparent_file:  Transparent file attributes
     linear_fixed_file: Linear fixed file attributes
     cyclic_file:       Cyclic file attributes
-------------------------------------------------------------------------------*/
typedef struct {
  mmgsdi_file_size_type           file_size;
  mmgsdi_file_id_type             file_id;
  mmgsdi_file_structure_enum_type file_type;
  union {
    mmgsdi_transparent_file_info_type  transparent_file;
    mmgsdi_linear_fixed_file_info_type linear_fixed_file;
    mmgsdi_cyclic_file_info_type       cyclic_file;
    /*~ CASE MMGSDI_TRANSPARENT_FILE mmgsdi_file_attributes_type.file_info.transparent_file */
    /*~ CASE MMGSDI_LINEAR_FIXED_FILE mmgsdi_file_attributes_type.file_info.linear_fixed_file */
    /*~ CASE MMGSDI_CYCLIC_FILE mmgsdi_file_attributes_type.file_info.cyclic_file */
  }file_info;
  /*~ FIELD mmgsdi_file_attributes_type.file_info DISC mmgsdi_file_attributes_type.file_type */
} mmgsdi_file_attributes_type;

/* ----------------------------------------------------------------------------
   STRUCTURE:      MMGSDI_ONCHIP_SIM_STATE

   DESCRIPTION:
     Used to indicate the current state of the onchip sim.
     onchip_sim_state:  0 = MMGSDI_ONCHIP_SIM_NOT_VALID
                        1 = MMGSDI_ONCHIP_SIM_INIT_COMPLETED
-------------------------------------------------------------------------------*/
typedef struct {
  mmgsdi_onchip_sim_state_enum_type     state;
}mmgsdi_onchip_sim_state;

/* ----------------------------------------------------------------------------
   ENUM:      MMGSDI_COMPUTE_IP_OPERATION_ENUM_TYPE

   DESCRIPTION:
     Indicates which type of Compute IP Authentication the user is requesting
-------------------------------------------------------------------------------*/
typedef enum {
  MMGSDI_CMPT_IP_SIP_CHAP       = 0,       /* Perform CHAP operation for SIP */
  MMGSDI_CMPT_IP_MIP_MN_HA      = 1,       /* Calculate MN-HA Authenticator */
  MMGSDI_CMPT_IP_MIP_RRQ        = 2,       /* Calculate MIP-RRQ Hash */
  MMGSDI_CMPT_IP_MIP_MN_AAA     = 3,       /* Calculate MN-AAA Authenticator */
  MMGSDI_CMPT_IP_HRPD_CHAP      = 4,       /* Perform CHAP operation for HRPD */
  MMGSDI_CMPT_IP_MAX_VALUE      = 5
} mmgsdi_compute_ip_operation_enum_type;

/* ----------------------------------------------------------------------------
   STRUCTURE:      MMGSDI_CMPT_IP_SIP_CHAP_DATA

   DESCRIPTION:
     Contains data required for a Simple IP CHAP Calculation
-------------------------------------------------------------------------------*/
typedef struct {
  uint8                 chap_id;                    /* CHAP ID */
  uint8                 nai_entry_index;            /* NAI Entry Index */
  mmgsdi_len_type       chap_challenge_length;      /* Challenge Length */
  uint8                 chap_challenge[MMGSDI_MAX_3GPD_CHAP_CHALLENGE_LENGTH];
} mmgsdi_cmpt_ip_sip_chap_data;

/* ----------------------------------------------------------------------------
   STRUCTURE:      MMGSDI_CMPT_IP_MIP_MN_HA_DATA

   DESCRIPTION:
     Contains data required for a Mobile IP MN-HA Calculation
     registration_data_length shall not be larger than
     MMGSDI_MAX_3GPD_MN_HA_REG_DATA_LENGTH
-------------------------------------------------------------------------------*/
typedef struct {
  uint8            nai_entry_index;            /* NAI Entry Index */
  mmgsdi_len_type  registration_data_length;   /* Registration Length */
  uint8            *registration_data_ptr;      /* Registration Data Ptr */
  /*~ FIELD mmgsdi_cmpt_ip_mip_mn_ha_data.registration_data_ptr VARRAY LENGTH mmgsdi_cmpt_ip_mip_mn_ha_data.registration_data_length */
} mmgsdi_cmpt_ip_mip_mn_ha_data;

/* ----------------------------------------------------------------------------
   STRUCTURE:      MMGSDI_CMPT_IP_MIP_RRQ_DATA

   DESCRIPTION:
     Contains data required for a Mobile IP RRQ Calculation
     rrq_data_length shall not be larger than MMGSDI_MAX_3GPD_HASH_RRQ_DATA_LENGTH
-------------------------------------------------------------------------------*/
typedef struct {
  mmgsdi_len_type  rrq_data_length;                  /* Preceding RRQ Length */
  uint8            *rrq_data_ptr;                    /* Prec MIP-RRQ Data Ptr */
  /*~ FIELD mmgsdi_cmpt_ip_mip_mip_rrq_data.rrq_data_ptr VARRAY LENGTH mmgsdi_cmpt_ip_mip_mip_rrq_data.rrq_data_length */
} mmgsdi_cmpt_ip_mip_mip_rrq_data;

/* ----------------------------------------------------------------------------
   STRUCTURE:      MMGSDI_CMPT_IP_MIP_MN_AAA_DATA
   DESCRIPTION:
     Contains data required for a Mobile IP MN-AAA Calculation
     challenge_length shall not be larger than MMGSDI_3GPD_MAX_MIP_CHALLENGE_LENGTH
-------------------------------------------------------------------------------*/
typedef struct {
  uint8            nai_entry_index;            /* NAI Entry Index */
  mmgsdi_len_type  challenge_length;           /* Challenge Length */
  uint8            *challenge_ptr;             /* Challenge Ptr */
  /*~ FIELD mmgsdi_cmpt_ip_mip_mn_aaa_data.challenge_ptr VARRAY LENGTH mmgsdi_cmpt_ip_mip_mn_aaa_data.challenge_length */
} mmgsdi_cmpt_ip_mip_mn_aaa_data;

/* ----------------------------------------------------------------------------
   STRUCTURE:      MMGSDI_CMPT_IP_HRPD_CHAP_DATA
   DESCRIPTION:
     Contains data required for a an HRPD CHAP Calculation
     chap_challenge_length not to exceed MMGSDI_MAX_HRPD_CHAP_CHALLENGE_LENGTH
-------------------------------------------------------------------------------*/
typedef struct {
  uint8            chap_id;                    /* CHAP ID */
  mmgsdi_len_type  chap_challenge_length;     /* Challenge Length */
  uint8            *chap_challenge_ptr;        /* Chap Challenge Ptr */
  /*~ FIELD mmgsdi_cmpt_ip_hrpd_chap_data.chap_challenge_ptr VARRAY LENGTH mmgsdi_cmpt_ip_hrpd_chap_data.chap_challenge_length */
} mmgsdi_cmpt_ip_hrpd_chap_data;

/* ----------------------------------------------------------------------------
   STRUCTURE:      MMGSDI_COMPUTE_IP_DATA_TYPE

   DESCRIPTION:
     Indicates which type of Compute IP Authentication the user is requesting
-------------------------------------------------------------------------------*/
typedef struct {
  mmgsdi_compute_ip_operation_enum_type operation_type;
  union {
    mmgsdi_cmpt_ip_sip_chap_data      sip_chap_data;
    mmgsdi_cmpt_ip_mip_mn_ha_data     mn_ha_data;
    mmgsdi_cmpt_ip_mip_mip_rrq_data   rrq_data;
    mmgsdi_cmpt_ip_mip_mn_aaa_data    mn_aaa_data;
    mmgsdi_cmpt_ip_hrpd_chap_data     hrpd_chap_data;
    /*~ CASE MMGSDI_CMPT_IP_SIP_CHAP   mmgsdi_compute_ip_data_type.data.sip_chap_data */
    /*~ CASE MMGSDI_CMPT_IP_MIP_MN_HA  mmgsdi_compute_ip_data_type.data.mn_ha_data */
    /*~ CASE MMGSDI_CMPT_IP_MIP_RRQ    mmgsdi_compute_ip_data_type.data.rrq_data */
    /*~ CASE MMGSDI_CMPT_IP_MIP_MN_AAA mmgsdi_compute_ip_data_type.data.mn_aaa_data */
    /*~ CASE MMGSDI_CMPT_IP_HRPD_CHAP  mmgsdi_compute_ip_data_type.data.hrpd_chap_data */
  } data;
/*~ FIELD mmgsdi_compute_ip_data_type.data DISC mmgsdi_compute_ip_data_type.operation_type */
} mmgsdi_compute_ip_data_type;

/* --------------------------------------------------------------------------
  STRUCTURE:      MMGSDI_SEARCH_OFFSET_DATA_TYPE

  DESCRIPTION:
    This structure is used for offset parameters for UICC Enhanced Search.
    offset_type: it indicates search option in a record.
                 Either contains the absolute record offset position or
                 contain a 1 byte character value.
    offset_data: 1 byte information.
                 For record offset its range is start from 0x00 to 0xFE.
                 For character value its range is start from 0x00 to 0xFF.
----------------------------------------------------------------------------*/
typedef struct {
  mmgsdi_search_option_enum_type  offset_type;
  mmgsdi_len_type                 offset_data;
} mmgsdi_search_offset_data_type;

/* ----------------------------------------------------------------------------
   STRUCTURE: MMGSDI_PKCS15_LOOKUP_TABLE_ENTRIES
   DESCRIPTION:
   The mmgsdi_pkcs15_lookup_table consists of two categories
   1. EF Enum ? List of PKCS15 EFs whose path is extracted from EF-ODF
   2. File Path Info ? File path info of the PKCS15 EF
-------------------------------------------------------------------------------*/
typedef struct {
  mmgsdi_file_enum_type      pkcs15_file_enum;
  mmgsdi_path_type           pkcs15_file_path;
} mmgsdi_pkcs15_lookup_table_entries;

/* ----------------------------------------------------------------------------
   STRUCTURE: MMGSDI_PKCS15_LOOKUP_TABLE
   DESCRIPTION:
   The mmgsdi_pkcs15_lookup_table consists of
   1. EF Enum ? List of PKCS15 EFs whose path is extracted from EF-ODF
   2. File Path Info ? File path info of the PKCS15 EF
-------------------------------------------------------------------------------*/
typedef struct {
  uint8                               no_of_pkcs15_lookup_table_entries;
  mmgsdi_pkcs15_lookup_table_entries  pkcs15_lookup_table [MMGSDI_MAX_PKCS15_TABLE_ENTRIES];
} mmgsdi_pkcs15_lookup_table;

/* ----------------------------------------------------------------------------
   STRUCTURE:      MMGSDI_PIN_OPERATION_CNF_TYPE

   DESCRIPTION:
     This structure is used when returning the PIN operation status
     for Verify, Disable, Enable, Change, and Unblock
     response_header:   Response data and status of corresponding PIN REQ
     pin_id:            PIN ID that the operation is being performed on
     pin_status:        State of the PIN.
-------------------------------------------------------------------------------*/
typedef struct {
  mmgsdi_response_header_type    response_header;
  mmgsdi_pin_info_type           pin_info;
  mmgsdi_pin_operation_enum_type pin_op;
} mmgsdi_pin_operation_cnf_type;

/* ----------------------------------------------------------------------------
   STRUCTURE:      MMGSDI_SELECT_AID_CNF_TYPE

   DESCRIPTION:
     This structure is used when returning the Select AID status

     response_header:   Response data and status
     aid:               App ID that the is being selected
-------------------------------------------------------------------------------*/
typedef struct {
  mmgsdi_response_header_type  response_header;
  mmgsdi_aid_type              app_data;
} mmgsdi_select_aid_cnf_type;

/* ----------------------------------------------------------------------------
   STRUCTURE:      MMGSDI_GET_ALL_PIN_STATUS_CNF_TYPE

   DESCRIPTION:
     This structure is used when returning all the PIN status
     response_header:   Response data and status of GET ALL PIN STATUS REQ
     pin1:              State of the PIN1.
     pin2:              State of the PIN2.
     universal_pin      State of the Universal PIN.
-------------------------------------------------------------------------------*/
typedef struct {
  mmgsdi_response_header_type  response_header;
  mmgsdi_pin_info_type         pin1;
  mmgsdi_pin_info_type         pin2;
  mmgsdi_pin_info_type         universal_pin;
} mmgsdi_get_all_pin_status_cnf_type;

/* ----------------------------------------------------------------------------
   STRUCTURE:      MMGSDI_STATUS_CNF_TYPE

   DESCRIPTION:
     This structure is used when returning a card status cnf type.
     response_header:  Read Response Header information
     access:           File read from
     accessed_rec_num: Record number accessed in case of Linear Fixed or
                       Cyclic files
     accessed_offset:  Offset accessed in case of Transparent files
     read_data:        Data read from the card
-------------------------------------------------------------------------------*/
typedef struct {
  mmgsdi_response_header_type response_header;
  mmgsdi_status_data_type     status_data;
} mmgsdi_status_cnf_type;

/* ----------------------------------------------------------------------------
   STRUCTURE:      MMGSDI_READ_CNF_TYPE

   DESCRIPTION:
     This structure is used when returning a read response
     from the MMGSDI Task.
     response_header:  Read Response Header information
     access:           File read from
     accessed_rec_num: Record number accessed in case of Linear Fixed or
                       Cyclic files
     accessed_offset:  Offset accessed in case of Transparent files
     read_data:        Data read from the card
-------------------------------------------------------------------------------*/
typedef struct {
  mmgsdi_response_header_type response_header;
  mmgsdi_access_type          access;
  mmgsdi_rec_num_type         accessed_rec_num;
  mmgsdi_offset_type          accessed_offset;
  mmgsdi_read_data_type       read_data;
} mmgsdi_read_cnf_type;

/* ----------------------------------------------------------------------------
   STRUCTURE:      MMGSDI_WRITE_CNF_TYPE

   DESCRIPTION:
     This structure is used when returning a write response
     from the MMGSDI Task.
     response_header:  Write Response Header information
     access:           File written to
     accessed_rec_num: Record number accessed in case of Linear Fixed or
                       Cyclic files
     accessed_offset:  Offset accessed in case of Transparent files
     written_data_len: The len of data written to the card
-------------------------------------------------------------------------------*/
typedef struct {
  mmgsdi_response_header_type response_header;
  mmgsdi_access_type          access;
  mmgsdi_rec_num_type         accessed_rec_num;
  mmgsdi_offset_type          accessed_offset;
  mmgsdi_len_type             written_data_len;
} mmgsdi_write_cnf_type;

/* ----------------------------------------------------------------------------
   STRUCTURE:      MMGSDI_GET_FILE_ATTR_CNF_TYPE

   DESCRIPTION:
     This structure is used when returning a get file attributes response
     from the MMGSDI Task.
     response_header:  Get File Attributes Response Header information
     access:           File that the request was made
     file_attrib:      The Header information
-------------------------------------------------------------------------------*/
typedef struct {
  mmgsdi_response_header_type response_header;
  mmgsdi_access_type          access;
  mmgsdi_file_attributes_type file_attrib;
} mmgsdi_get_file_attr_cnf_type;

/* ----------------------------------------------------------------------------
   STRUCTURE:      MMGSDI_JCDMA_GET_CARD_INFO_CNF_TYPE

   DESCRIPTION:
     This structure is used when returning a get jcdma card info response
     from the MMGSDI Task.
-------------------------------------------------------------------------------*/
typedef struct {
  mmgsdi_response_header_type       response_header;
  mmgsdi_jcdma_card_info_enum_type  card_info;
  mmgsdi_esn_me_change_enum_type    me_esn_change;
} mmgsdi_jcdma_get_card_info_cnf_type;

/* ----------------------------------------------------------------------------
   STRUCTURE:      MMGSDI_CLIENT_REG_ID_CNF_TYPE

   DESCRIPTION:
     This structure is used when returning a get client ID registration response
     from the MMGSDI Task.
     response_header:  Client Register ID Response Header information
-------------------------------------------------------------------------------*/
typedef struct {
  mmgsdi_response_header_type response_header;
} mmgsdi_client_id_reg_cnf_type;

/* ----------------------------------------------------------------------------
   STRUCTURE:      MMGSDI_CLIENT_DEREG_ID_CNF_TYPE

   DESCRIPTION:
     This structure is used notify if the client ID deregistration succeeded
     or failed.
     response_header:  Client Deregister ID Response Header information
-------------------------------------------------------------------------------*/
typedef struct {
  mmgsdi_response_header_type response_header;
} mmgsdi_client_id_dereg_cnf_type;

/* ----------------------------------------------------------------------------
   STRUCTURE:      MMGSDI_CLIENT_EVT_REG_CNF_TYPE

   DESCRIPTION:
     This structure is used when client wants to register for event callback
     from the MMGSDI Task.
     response_header:  Client Register ID Response Header information
-------------------------------------------------------------------------------*/
typedef struct
{
  mmgsdi_response_header_type response_header;
} mmgsdi_client_evt_reg_cnf_type;

/* ----------------------------------------------------------------------------
   STRUCTURE:      MMGSDI_CLIENT_EVT_DEREG_CNF_TYPE

   DESCRIPTION:
     This structure is used when client wants to deregister for event callback
     from the MMGSDI Task.
     response_header:  Client Deregister ID Response Header information
-------------------------------------------------------------------------------*/
typedef struct
{
  mmgsdi_response_header_type response_header;
} mmgsdi_client_evt_dereg_cnf_type;

/* ----------------------------------------------------------------------------
   STRUCTURE:      MMGSDI_CLIENT_INIT_COMP_CNF_TYPE

   DESCRIPTION:
     This structure is used when client completes its initalizations and informs
     MMGSDI
     response_header:  Client Init Complete Response Header information
-------------------------------------------------------------------------------*/
typedef struct
{
  mmgsdi_response_header_type response_header;
} mmgsdi_client_init_comp_cnf_type;

/* ----------------------------------------------------------------------------
   STRUCTURE:      MMGSDI_REFRESH_CNF_TYPE

   DESCRIPTION:
     This structure is used notify if the refresh procedure is successful or
     not.
     response_header:  SUCCESS or FAILURE of the operation
-------------------------------------------------------------------------------*/
typedef struct {
  mmgsdi_response_header_type    response_header;
  mmgsdi_refresh_orig_enum_type  orig_refresh_req;
} mmgsdi_refresh_cnf_type;

/* ----------------------------------------------------------------------------
   STRUCTURE:      MMGSDI_DISABLE_BDN_CNF_TYPE

   DESCRIPTION:
     This structure is used notify if the Disable BDN process succeeded
     or failed.
     response_header:  SUCCESS or FAILURE of the operation
-------------------------------------------------------------------------------*/
typedef struct {
  mmgsdi_response_header_type response_header;
} mmgsdi_disable_bdn_cnf_type;

/* ----------------------------------------------------------------------------
   STRUCTURE:      MMGSDI_ENABLE_BDN_CNF_TYPE

   DESCRIPTION:
     This structure is used notify if the Enable BDN process succeeded
     or failed.
     response_header:  SUCCESS or FAILURE of the operation
-------------------------------------------------------------------------------*/
typedef struct {
  mmgsdi_response_header_type response_header;
} mmgsdi_enable_bdn_cnf_type;

/* ----------------------------------------------------------------------------
   STRUCTURE:      MMGSDI_ACL_OPERATION_CNF_TYPE

   DESCRIPTION:
     This structure is used when returning the ACL operation status
     for Disable, Enable
     response_header:   Response data and status of corresponding PIN REQ
     acl_op:            Operation being performed.
-------------------------------------------------------------------------------*/
typedef struct {
  mmgsdi_response_header_type                response_header;
  mmgsdi_acl_operation_enum_type             acl_op;
} mmgsdi_acl_operation_cnf_type;

/* ----------------------------------------------------------------------------
   STRUCTURE:      MMGSDI_REHABILITATE_CNF_TYPE

   DESCRIPTION:
     This structure is used notify if the rehabilitation process succeeded
     or failed.
     response_header:  SUCCESS or FAILURE of the operation
     access:           File requested for rehabilitation
-------------------------------------------------------------------------------*/
typedef struct {
  mmgsdi_response_header_type response_header;
  mmgsdi_access_type          access;
} mmgsdi_rehabilitate_cnf_type;

/* ----------------------------------------------------------------------------
   STRUCTURE:      MMGSDI_INVALIDATE_CNF_TYPE

   DESCRIPTION:
     This structure is used notify if the invalidation process succeeded
     or failed.
     response_header:  SUCCESS or FAILURE of the operation
     access:           File requested for invalidation
-------------------------------------------------------------------------------*/
typedef struct {
  mmgsdi_response_header_type response_header;
  mmgsdi_access_type          access;
} mmgsdi_invalidate_cnf_type;

/* ----------------------------------------------------------------------------
   STRUCTURE:      MMGSDI_SAP_INIT_CNF_TYPE

   DESCRIPTION:
     This structure is used when returning a SAP connect response
     from the MMGSDI Task.
------------------------------------------------------------------------------*/
typedef struct {
  mmgsdi_response_header_type response_header;
  mmgsdi_sap_mode_enum_type   sap_mode;
} mmgsdi_sap_init_cnf_type;

/* ----------------------------------------------------------------------------
   STRUCTURE:      MMGSDI_SAP_DEREG_CNF_TYPE

   DESCRIPTION:
     This structure is used when returning a SAP connect response
     from the MMGSDI Task.
------------------------------------------------------------------------------*/
typedef struct {
  mmgsdi_response_header_type response_header;
} mmgsdi_sap_dereg_cnf_type;

/* ----------------------------------------------------------------------------
   STRUCTURE:      MMGSDI_SAP_VERIFY_PIN_CNF_TYPE

   DESCRIPTION:
     This structure is used when returning a SAP verify PIN response
     from the MMGSDI Task.
------------------------------------------------------------------------------*/
typedef struct {
  mmgsdi_response_header_type response_header;
} mmgsdi_sap_verify_pin_cnf_type;

/* ----------------------------------------------------------------------------
   STRUCTURE:      MMGSDI_SAP_CONNECT_CNF_TYPE

   DESCRIPTION:
     This structure is used when returning a SAP connect response
     from the MMGSDI Task.
------------------------------------------------------------------------------*/
typedef struct {
  mmgsdi_response_header_type response_header;
} mmgsdi_sap_connect_cnf_type;

/* ----------------------------------------------------------------------------
   STRUCTURE:      MMGSDI_SAP_DISCONNECT_CNF_TYPE

   DESCRIPTION:
     This structure is used when returning a SAP connect response
     from the MMGSDI Task.
------------------------------------------------------------------------------*/
typedef struct {
  mmgsdi_response_header_type         response_header;
  mmgsdi_disconnect_mode_enum_type    disconnect_mode;
} mmgsdi_sap_disconnect_cnf_type;

/* ----------------------------------------------------------------------------
   STRUCTURE:      MMGSDI_GET_ATR_CNF_TYPE

   DESCRIPTION:
     This structure is used when returning a get atr response
     from the MMGSDI Task.
------------------------------------------------------------------------------*/
typedef struct {
  mmgsdi_response_header_type  response_header;
  mmgsdi_atr_data_type         atr_data;
} mmgsdi_get_atr_cnf_type;

/* ----------------------------------------------------------------------------
   STRUCTURE:      MMGSDI_SAP_GET_ATR_CNF_TYPE

   DESCRIPTION:
     This structure is used when returning a SAP get atr response
     from the MMGSDI Task.
------------------------------------------------------------------------------*/
typedef struct {
  mmgsdi_response_header_type  response_header;
  mmgsdi_sap_get_atr_data_type atr_data;
} mmgsdi_sap_get_atr_cnf_type;

/* ----------------------------------------------------------------------------
   STRUCTURE:      MMGSDI_SAP_SEND_APDU_CNF_TYPE

   DESCRIPTION:
     This structure is used when returning a SAP APDU response
     from the MMGSDI Task.
------------------------------------------------------------------------------*/
typedef struct {
  mmgsdi_response_header_type response_header;
  uint32                      max_size;
  int32                       channel_id;
  uint16                      implicit_get_rsp_sw1;
  uint16                      implicit_get_rsp_sw2;
  mmgsdi_send_apdu_data_type  apdu_data;
} mmgsdi_send_apdu_cnf_type;

typedef mmgsdi_send_apdu_cnf_type mmgsdi_sap_send_apdu_cnf_type;

/* ----------------------------------------------------------------------------
   STRUCTURE:      MMGSDI_SAP_POWER_OFF_CNF_TYPE

   DESCRIPTION:
     This structure is used when returning a SAP power off response
     from the MMGSDI Task.
------------------------------------------------------------------------------*/
typedef struct {
  mmgsdi_response_header_type response_header;
} mmgsdi_sap_power_off_cnf_type;

/* ----------------------------------------------------------------------------
   STRUCTURE:      MMGSDI_CARD_READER_STATUS_CNF_TYPE

   DESCRIPTION:
     This structure is used when returning the data to the client
     from the MMGSDI Task.
------------------------------------------------------------------------------*/
typedef struct {
  mmgsdi_response_header_type         response_header;
  mmgsdi_card_reader_status_data_type card_reader_status_data;
} mmgsdi_card_reader_status_cnf_type;

/* ----------------------------------------------------------------------------
   STRUCTURE:      MMGSDI_SAP_CARD_READER_STATUS_CNF_TYPE

   DESCRIPTION:
     This structure is used when returning a SAP card reader status response
     from the MMGSDI Task.
------------------------------------------------------------------------------*/
typedef struct {
  mmgsdi_response_header_type             response_header;
  mmgsdi_sap_card_reader_status_data_type card_reader_status_data;
} mmgsdi_sap_card_reader_status_cnf_type;

/* ----------------------------------------------------------------------------
   STRUCTURE:      MMGSDI_SAP_RESET_CNF_TYPE

   DESCRIPTION:
     This structure is used when returning a SAP reset response
     from the MMGSDI Task.
------------------------------------------------------------------------------*/
typedef struct {
  mmgsdi_response_header_type response_header;
} mmgsdi_sap_reset_cnf_type;

/* ----------------------------------------------------------------------------
   STRUCTURE:      MMGSDI_SAP_POWER_ON_CNF_TYPE

   DESCRIPTION:
     This structure is used when returning a SAP power on response
     from the MMGSDI Task.
------------------------------------------------------------------------------*/
typedef struct {
  mmgsdi_response_header_type    response_header;
} mmgsdi_sap_power_on_cnf_type;

/* ----------------------------------------------------------------------------
   STRUCTURE:      MMGSDI_SESSION_OPEN_CNF_TYPE

   DESCRIPTION:
     This structure is used when returning a open session response
     from the MMGSDI Task.
     response_header:  Client Register ID Response Header information
     session_id:       Session ID returned that user to be used in subsequent
                       requests to communicate to the session
     channel_id:       Channel on which this session was open with the card
-------------------------------------------------------------------------------*/
typedef struct {
  mmgsdi_response_header_type response_header;
  mmgsdi_client_id_type       session_id;
  int32                       channel_id;
} mmgsdi_session_open_cnf_type;

/* ----------------------------------------------------------------------------
   STRUCTURE:      MMGSDI_SESSION_CLOSE_CNF_TYPE

   DESCRIPTION:
     This structure is used when returning a Session Close response
     from the MMGSDI Task.
     response_header:  Client Register ID Response Header information
     session_id:       Session ID returned to indicate which session has been
                       Closed
-------------------------------------------------------------------------------*/
typedef struct {
  mmgsdi_response_header_type response_header;
  mmgsdi_client_id_type       session_id;
} mmgsdi_session_close_cnf_type;

/* ----------------------------------------------------------------------------
   STRUCTURE:      MMGSDI_SESSION_DEACTIVATE_CNF_TYPE

   DESCRIPTION:
     This structure is used when returning a session application deactivation response
     from the MMGSDI Task.
     response_header:  Client Register ID Response Header information
     session_id:       Session ID returned to indicate which session has been
                       Deactivated
     app_id:           Application detail
-------------------------------------------------------------------------------*/
typedef struct {
  mmgsdi_response_header_type response_header;
  mmgsdi_client_id_type       session_id;
  mmgsdi_aid_type             app_id;
} mmgsdi_session_deactivate_cnf_type;

/* ----------------------------------------------------------------------------
   STRUCTURE:      MMGSDI_SESSION_SELECT_AID_CNF_TYPE

   DESCRIPTION:
     This structure is used when returning a session AID selection response
     from the MMGSDI Task.
     response_header:  Client Register ID Response Header information
     session_id:       Session ID returned to indicate which session has been
                       selected
     app_id:           Application detail
-------------------------------------------------------------------------------*/
typedef struct {
  mmgsdi_response_header_type response_header;
  mmgsdi_client_id_type       session_id;
  mmgsdi_aid_type             app_id;
} mmgsdi_session_select_aid_cnf_type;

/* ----------------------------------------------------------------------------
   STRUCTURE:      MMGSDI_ISIM_AUTH_CNF_TYPE

   DESCRIPTION:
     This structure is used when returning an ISIM authentication response
     from the MMGSDI Task.
     response_header:  Read Response Header information
     auth_rsp:         Authentication response
-------------------------------------------------------------------------------*/
typedef struct {
  mmgsdi_response_header_type response_header;
  mmgsdi_data_type            res;
  mmgsdi_data_type            ck;
  mmgsdi_data_type            ik;
  mmgsdi_data_type            auts;
} mmgsdi_isim_auth_cnf_type;

/* ----------------------------------------------------------------------------
   STRUCTURE:      MMGSDI_CARD_PUP_CNF_TYPE

   DESCRIPTION:
     This structure is used when returning an CARD power up response
     from the MMGSDI Task.
     response_header:  Read Response Header information
     option:           Option indicating how the PUP was performed
-------------------------------------------------------------------------------*/
typedef struct
{
  mmgsdi_response_header_type       response_header;
  mmgsdi_card_pup_options_enum_type option;
}mmgsdi_card_pup_cnf_type;

/* ----------------------------------------------------------------------------
   STRUCTURE:      MMGSDI_CARD_PDOWN_CNF_TYPE

   DESCRIPTION:
     This structure is used when returning an CARD power down response
     from the MMGSDI Task.
     response_header:  Read Response Header information
     option:           Option indicating how the PDOWN was performed
-------------------------------------------------------------------------------*/
typedef struct
{
  mmgsdi_response_header_type         response_header;
  mmgsdi_card_pdown_options_enum_type option;
}mmgsdi_card_pdown_cnf_type;


/* ----------------------------------------------------------------------------
   STRUCTURE:      MMGSDI_ACTIVATE_ONCHIP_SIM_CNF_TYPE

   DESCRIPTION:
     This structure is used when returning the ONCHIP SIM Data
     response_header:   Resonse data and status of ACTIVATE ONCHIP SIM REQ
     onchip_sim_state:  State of the onchip SIM.
-------------------------------------------------------------------------------*/
typedef struct {
  mmgsdi_response_header_type  response_header;
  mmgsdi_onchip_sim_state      onchip_sim_state;
} mmgsdi_act_onchip_sim_cnf_type;

/* ----------------------------------------------------------------------------
   STRUCTURE:      MMGSDI_SRV_AVAILABLE_CNF_TYPE

   DESCRIPTION:
     This structure is used when returning the srv available status

     response_header:   Response data and status of corresponding SRV AVAILABLE
     srv_available:     Status of the Service
-------------------------------------------------------------------------------*/
typedef struct {
  mmgsdi_response_header_type                response_header;
  boolean                                    srv_available;
} mmgsdi_srv_available_cnf_type;

/* ----------------------------------------------------------------------------
   STRUCTURE:      MMGSDI_USIM_AUTH_CNF_TYPE

   DESCRIPTION:
     This structure is used when returning the srv available status

     response_header:   Response data and status of corresponding SRV AVAILABLE
     auth_response:     Raw Data from the card
-------------------------------------------------------------------------------*/
typedef struct {
  mmgsdi_response_header_type response_header;
  mmgsdi_data_type            auth_response;
} mmgsdi_usim_auth_cnf_type;

/* ----------------------------------------------------------------------------
   STRUCTURE:      MMGSDI_COMPUTE_IP_AUTH_CNF_TYPE

   DESCRIPTION:
     This structure is used when returning the data as a result of a Compute IP
     Auth Command
-------------------------------------------------------------------------------*/
typedef struct {
  mmgsdi_response_header_type            response_header;
  mmgsdi_cmpt_ip_response_data_type      cmpt_ip_response_data;
} mmgsdi_compute_ip_auth_cnf_type;

/* ----------------------------------------------------------------------------
   STRUCTURE:      MMGSDI_RUN_CAVE_CNF_TYPE

   DESCRIPTION:
     This structure is used when returning the data as a result of a RUN CAVE
     command
-------------------------------------------------------------------------------*/
typedef struct {
  mmgsdi_response_header_type            response_header;
  mmgsdi_cave_auth_resp_type             run_cave_response_data;
} mmgsdi_run_cave_cnf_type;

/* ----------------------------------------------------------------------------
   STRUCTURE:      MMGSDI_GENERATE_KEY_VPM_CNF_TYPE

   DESCRIPTION:
     This structure is used when returning the data as a result of a GENERATE
     KEY / VPM TYPE command
-------------------------------------------------------------------------------*/
typedef struct {
  mmgsdi_response_header_type            response_header;
  mmgsdi_key_type                        key_data;
  mmgsdi_vpm_octets_type                 octet_data;
} mmgsdi_generate_key_vpm_cnf_type;

/* ---------------------------------------------------------------
   STRUCTURE:      MMGSDI_MFLO_INIT_ACT_PARAMS_CNF_TYPE

   DESCRIPTION:
     It contains the confirmation for initialize activation
     parameters for MediaFLO ADF.
----------------------------------------------------------------*/
typedef struct {
  mmgsdi_response_header_type    response_header;
} mmgsdi_mflo_init_act_params_cnf_type;

/* ---------------------------------------------------------------
   STRUCTURE:      MMGSDI_MFLO_GET_SUBSCRIBER_ID_CNF_TYPE

   DESCRIPTION:
     It contains the confirmation for get subscriber Id for MediaFLO
     ADF, the len is specified in the “Qualcomm MediaFLO
     Authentication Manager Applet Functional Specification?
----------------------------------------------------------------*/
typedef struct {
  mmgsdi_response_header_type    response_header;
  mmgsdi_static_data_type        subscriber_id;
} mmgsdi_mflo_get_subscriber_id_cnf_type;

/* ---------------------------------------------------------------
   STRUCTURE:      MMGSDI_MFLO_GET_PUBLIC_KEY_CNF_TYPE

   DESCRIPTION:
     It contains the confirmation for get public key for MediaFLO
     ADF, the len is specified in the “Qualcomm MediaFLO
     Authentication Manager Applet Functional Specification?
----------------------------------------------------------------*/
typedef struct {
  mmgsdi_response_header_type    response_header;
  mmgsdi_data_type               public_key;
} mmgsdi_mflo_get_public_key_cnf_type;

/* ---------------------------------------------------------------
   STRUCTURE:      MMGSDI_MFLO_SIGN_UCAST_CNF_TYPE

   DESCRIPTION:
     This contains the confirmation data for the Sign Unicast
     Message MediaFLO command.
----------------------------------------------------------------*/
typedef struct {
  mmgsdi_response_header_type    response_header;
  mmgsdi_data_type               ucast_msg;
} mmgsdi_mflo_sign_ucast_cnf_type;

/* ---------------------------------------------------------------
   STRUCTURE:      MMGSDI_MFLO_VERIFY_UCAST_CNF_TYPE

   DESCRIPTION:
     It contains the confirmation for verification for unicast
     Message and signature.
----------------------------------------------------------------*/
typedef struct {
  mmgsdi_response_header_type    response_header;
} mmgsdi_mflo_verify_ucast_cnf_type;

/* ----------------------------------------------------------------------------
   STRUCTURE:      MMGSDI_CARD_RESET_CNF_TYPE

   DESCRIPTION:
     This structure is used when returning the CARD RESET data to the client
     from the MMGSDI Task.
------------------------------------------------------------------------------*/
typedef struct {
  mmgsdi_response_header_type         response_header;
} mmgsdi_card_reset_cnf_type;

/* ----------------------------------------------------------------------------
   STRUCTURE:      MMGSDI_BCAST_CNF_TYPE

   DESCRIPTION:
     This structure is used when returning bcast confirmation

     response_header:   Response data and status of corresponding BCAST_REQ
     bcast_response:    Raw Data from the card
-------------------------------------------------------------------------------*/
typedef struct {
  mmgsdi_response_header_type response_header;
  mmgsdi_data_type            bcast_response;
} mmgsdi_bcast_cnf_type;

/* --------------------------------------------------------------------------
  STRUCTURE:      MMGSDI_SEARCH_CNF_TYPE
  DESCRIPTION:
    This structure is used when returning search/seek response
    from the MMGSDI Task.
    response_header : Read Response Header information
    access          : File search from
    search_data     : List of Record numbers [0x01,0x02..] returned from
                      search and seek command
-----------------------------------------------------------------------------*/
typedef struct {
  mmgsdi_response_header_type response_header;
  mmgsdi_access_type          access;
  mmgsdi_search_data_type     searched_record_nums;
} mmgsdi_search_cnf_type;

/* ----------------------------------------------------------------------------
   STRUCTURE:      MMGSDI_CREATE_PKCS15_TABLE_CNF_TYPE

   DESCRIPTION:
    This structure is used when returning a create PKCS15 Table response
     from the MMGSDI Task.
     response_header: Create PKCS15 Table Response Header information
     pkcs15_lookup_table:    PKCS15 look up table
-------------------------------------------------------------------------------*/
typedef struct {
  mmgsdi_response_header_type response_header;
  mmgsdi_pkcs15_lookup_table  pkcs15_lookup_table;
} mmgsdi_create_pkcs15_table_cnf_type;

/* ----------------------------------------------------------------------------
   STRUCTURE:      MMGSDI_CNF_TYPE

   DESCRIPTION:
     The confirmation data for the request
     response_header:   Response Header information
     client_id_reg_cnf: Client ID registration confirmation
     read_cnf:          Read confirmation
     write_cnf:         Write confirmation
     get_file_attr_cnf: Get File Attributes confirmation
-------------------------------------------------------------------------------*/
typedef union {
  mmgsdi_response_header_type                response_header;
  mmgsdi_client_id_reg_cnf_type              client_id_reg_cnf;
  mmgsdi_client_evt_reg_cnf_type             client_evt_reg_cnf;
  mmgsdi_client_id_dereg_cnf_type            client_id_dereg_cnf;
  mmgsdi_client_evt_dereg_cnf_type           client_evt_dereg_cnf;
  mmgsdi_read_cnf_type                       read_cnf;
  mmgsdi_write_cnf_type                      write_cnf;
  mmgsdi_get_file_attr_cnf_type              get_file_attr_cnf;
  mmgsdi_session_open_cnf_type               session_open_cnf;
  mmgsdi_session_close_cnf_type              session_close_cnf;
  mmgsdi_session_deactivate_cnf_type         session_deactivate_cnf;
  mmgsdi_session_select_aid_cnf_type         session_select_aid_cnf;
  mmgsdi_isim_auth_cnf_type                  isim_auth_cnf;
  mmgsdi_send_apdu_cnf_type                  send_apdu_cnf;
  mmgsdi_get_atr_cnf_type                    get_atr_cnf;
  mmgsdi_card_reader_status_cnf_type         card_reader_status_cnf;
  mmgsdi_sap_connect_cnf_type                sap_connect_cnf;
  mmgsdi_sap_disconnect_cnf_type             sap_disconnect_cnf;
  mmgsdi_sap_get_atr_cnf_type                sap_get_atr_cnf;
  mmgsdi_sap_send_apdu_cnf_type              sap_send_apdu_cnf;
  mmgsdi_sap_power_on_cnf_type               sap_power_on_cnf;
  mmgsdi_sap_power_off_cnf_type              sap_power_off_cnf;
  mmgsdi_sap_reset_cnf_type                  sap_reset_cnf;
  mmgsdi_sap_card_reader_status_cnf_type     sap_card_reader_status_cnf;
  mmgsdi_sap_init_cnf_type                   sap_init_cnf;
  mmgsdi_sap_dereg_cnf_type                  sap_dereg_cnf;
  mmgsdi_sap_verify_pin_cnf_type             sap_verify_pin_cnf;
  mmgsdi_card_pup_cnf_type                   card_pup_cnf;
  mmgsdi_card_pdown_cnf_type                 card_pdown_cnf;
  mmgsdi_status_cnf_type                     status_cnf;
  mmgsdi_refresh_cnf_type                    refresh_cnf;
  mmgsdi_disable_bdn_cnf_type                disable_bdn_cnf;
  mmgsdi_enable_bdn_cnf_type                 enable_bdn_cnf;
  mmgsdi_acl_operation_cnf_type              acl_operation_cnf;
  mmgsdi_rehabilitate_cnf_type               rehab_cnf;
  mmgsdi_invalidate_cnf_type                 invalidate_cnf;
  mmgsdi_act_onchip_sim_cnf_type             act_onchip_cnf;
  mmgsdi_pin_operation_cnf_type              pin_operation_cnf;
  mmgsdi_get_all_pin_status_cnf_type         get_all_pin_status_cnf;
  mmgsdi_select_aid_cnf_type                 select_aid_cnf;
  mmgsdi_srv_available_cnf_type              srv_available_cnf;
  mmgsdi_usim_auth_cnf_type                  usim_auth_cnf;
  mmgsdi_jcdma_get_card_info_cnf_type        get_jcdma_card_info_cnf;
  mmgsdi_compute_ip_auth_cnf_type            compute_ip_cnf;
  mmgsdi_run_cave_cnf_type                   run_cave_cnf;
  mmgsdi_generate_key_vpm_cnf_type           generate_key_vpm_cnf;
  mmgsdi_mflo_init_act_params_cnf_type       mflo_init_act_params_cnf;
  mmgsdi_mflo_get_subscriber_id_cnf_type     mflo_get_subscriber_id_cnf;
  mmgsdi_mflo_get_public_key_cnf_type        mflo_get_public_key_cnf;
  mmgsdi_mflo_sign_ucast_cnf_type            mflo_sign_ucast_cnf;
  mmgsdi_mflo_verify_ucast_cnf_type          mflo_verify_ucast_cnf;
  mmgsdi_client_init_comp_cnf_type           client_init_comp_cnf;
  mmgsdi_card_reset_cnf_type                 card_reset_cnf;
  mmgsdi_bcast_cnf_type                      bcast_cnf;
  mmgsdi_search_cnf_type                     search_cnf;
  mmgsdi_create_pkcs15_table_cnf_type        create_pkcs15_table_cnf;
 /*~ CASE MMGSDI_CLIENT_ID_REG_CNF          mmgsdi_cnf_type.client_id_reg_cnf    */
 /*~ CASE MMGSDI_CLIENT_EVT_REG_CNF         mmgsdi_cnf_type.client_evt_reg_cnf   */
 /*~ CASE MMGSDI_CLIENT_ID_DEREG_CNF        mmgsdi_cnf_type.client_id_dereg_cnf  */
 /*~ CASE MMGSDI_CLIENT_EVT_DEREG_CNF       mmgsdi_cnf_type.client_evt_dereg_cnf */
 /*~ CASE MMGSDI_READ_CNF                   mmgsdi_cnf_type.read_cnf             */
 /*~ CASE MMGSDI_WRITE_CNF                  mmgsdi_cnf_type.write_cnf            */
 /*~ CASE MMGSDI_GET_FILE_ATTR_CNF          mmgsdi_cnf_type.get_file_attr_cnf    */
 /*~ CASE MMGSDI_SESSION_OPEN_CNF           mmgsdi_cnf_type.session_open_cnf     */
 /*~ CASE MMGSDI_SESSION_CLOSE_CNF          mmgsdi_cnf_type.session_close_cnf    */
 /*~ CASE MMGSDI_SESSION_DEACTIVATE_CNF     mmgsdi_cnf_type.session_deactivate_cnf */
 /*~ CASE MMGSDI_SESSION_SELECT_AID_CNF     mmgsdi_cnf_type.session_select_aid_cnf */
 /*~ CASE MMGSDI_ISIM_AUTH_CNF              mmgsdi_cnf_type.isim_auth_cnf        */
 /*~ CASE MMGSDI_SEND_APDU_CNF              mmgsdi_cnf_type.send_apdu_cnf        */
 /*~ CASE MMGSDI_SAP_CONNECT_CNF            mmgsdi_cnf_type.sap_connect_cnf      */
 /*~ CASE MMGSDI_SAP_DISCONNECT_CNF         mmgsdi_cnf_type.sap_disconnect_cnf   */
 /*~ CASE MMGSDI_SAP_GET_ATR_CNF            mmgsdi_cnf_type.sap_get_atr_cnf      */
 /*~ CASE MMGSDI_SAP_SEND_APDU_CNF          mmgsdi_cnf_type.sap_send_apdu_cnf    */
 /*~ CASE MMGSDI_SAP_POWER_ON_CNF           mmgsdi_cnf_type.sap_power_on_cnf     */
 /*~ CASE MMGSDI_SAP_POWER_OFF_CNF          mmgsdi_cnf_type.sap_power_off_cnf    */
 /*~ CASE MMGSDI_SAP_RESET_CNF              mmgsdi_cnf_type.sap_reset_cnf        */
 /*~ CASE MMGSDI_SAP_CARD_READER_STATUS_CNF mmgsdi_cnf_type.sap_card_reader_status_cnf    */
 /*~ CASE MMGSDI_SAP_INIT_CNF               mmgsdi_cnf_type.sap_init_cnf         */
 /*~ CASE MMGSDI_SAP_DEREG_CNF              mmgsdi_cnf_type.sap_dereg_cnf        */
 /*~ CASE MMGSDI_SAP_VERIFY_PIN_CNF         mmgsdi_cnf_type.sap_verify_pin_cnf   */
 /*~ CASE MMGSDI_CARD_PUP_CNF               mmgsdi_cnf_type.card_pup_cnf         */
 /*~ CASE MMGSDI_CARD_PDOWN_CNF             mmgsdi_cnf_type.card_pdown_cnf       */
 /*~ CASE MMGSDI_CARD_STATUS_CNF            mmgsdi_cnf_type.status_cnf           */
 /*~ CASE MMGSDI_REFRESH_CNF                mmgsdi_cnf_type.refresh_cnf          */
 /*~ CASE MMGSDI_DISABLE_BDN_CNF            mmgsdi_cnf_type.disable_bdn_cnf      */
 /*~ CASE MMGSDI_ENABLE_BDN_CNF             mmgsdi_cnf_type.enable_bdn_cnf       */
 /*~ CASE MMGSDI_ACL_OPERATION_CNF          mmgsdi_cnf_type.acl_operation_cnf  */
 /*~ CASE MMGSDI_REHABILITATE_CNF           mmgsdi_cnf_type.rehab_cnf            */
 /*~ CASE MMGSDI_INVALIDATE_CNF             mmgsdi_cnf_type.invalidate_cnf       */
 /*~ CASE MMGSDI_ACTIVATE_ONCHIP_SIM_CNF    mmgsdi_cnf_type.act_onchip_cnf   */
 /*~ CASE MMGSDI_PIN_OPERATION_CNF          mmgsdi_cnf_type.pin_operation_cnf    */
 /*~ CASE MMGSDI_SELECT_AID_CNF             mmgsdi_cnf_type.select_aid_cnf       */
 /*~ CASE MMGSDI_GET_ALL_PIN_STATUS_CNF     mmgsdi_cnf_type.get_all_pin_status_cnf*/
 /*~ CASE MMGSDI_SRV_AVAILABLE_CNF          mmgsdi_cnf_type.srv_available_cnf    */
 /*~ CASE MMGSDI_USIM_AUTH_CNF              mmgsdi_cnf_type.usim_auth_cnf        */
 /*~ CASE MMGSDI_JCDMA_GET_CARD_INFO_CNF    mmgsdi_cnf_type.get_jcdma_card_info_cnf */
 /*~ CASE MMGSDI_COMPUTE_IP_AUTH_CNF        mmgsdi_cnf_type.compute_ip_cnf       */
 /*~ CASE MMGSDI_RUN_CAVE_CNF               mmgsdi_cnf_type.run_cave_cnf         */
 /*~ CASE MMGSDI_GENERATE_KEY_VPM_CNF       mmgsdi_cnf_type.generate_key_vpm_cnf */
 /*~ CASE MMGSDI_GET_ATR_CNF                mmgsdi_cnf_type.get_atr_cnf          */
 /*~ CASE MMGSDI_CARD_READER_STATUS_CNF     mmgsdi_cnf_type.card_reader_status_cnf    */
 /*~ CASE MMGSDI_MFLO_INIT_ACT_PARAMS_CNF   mmgsdi_cnf_type.mflo_init_act_params_cnf*/
 /*~ CASE MMGSDI_MFLO_GET_SUBSCRIBER_ID_CNF mmgsdi_cnf_type.mflo_get_subscriber_id_cnf*/
 /*~ CASE MMGSDI_MFLO_GET_PUBLIC_KEY_CNF    mmgsdi_cnf_type.mflo_get_public_key_cnf   */
 /*~ CASE MMGSDI_MFLO_SIGN_UCAST_CNF        mmgsdi_cnf_type.mflo_sign_ucast_cnf   */
 /*~ CASE MMGSDI_MFLO_VERIFY_UCAST_CNF      mmgsdi_cnf_type.mflo_verify_ucast_cnf */
 /*~ CASE MMGSDI_CLIENT_INIT_COMP_CNF       mmgsdi_cnf_type.client_init_comp_cnf */
 /*~ CASE MMGSDI_CARD_RESET_CNF             mmgsdi_cnf_type.card_reset_cnf */
 /*~ CASE MMGSDI_BCAST_CNF                  mmgsdi_cnf_type.bcast_cnf */
 /*~ CASE MMGSDI_SEARCH_CNF                 mmgsdi_cnf_type.search_cnf */
 /*~ CASE MMGSDI_CREATE_PKCS15_TABLE_CNF    mmgsdi_cnf_type.create_pkcs15_table_cnf */
 /*~ DEFAULT mmgsdi_cnf_type.response_header */
} mmgsdi_cnf_type;
/*~ TYPE mmgsdi_cnf_type DISC (_OBJ_.response_header.response_type) */



/* ----------------------------------------------------------------------------
   STRUCTURE:      MMGSDI_CALLBACK_TYPE

   DESCRIPTION:    The client callback function.  The parameter to the
                   callback function contains the result of the procedures
                   performed as the result of the client's request.
-------------------------------------------------------------------------------*/
typedef void (*mmgsdi_callback_type) (mmgsdi_return_enum_type status,
                                      mmgsdi_cnf_enum_type    cnf,
                                      const mmgsdi_cnf_type  *cnf_ptr);
/*~ PARAM cnf_ptr POINTER */

/*=============================================================================

                          FUNCTIONS

=============================================================================*/
/* ----------------------------------------------------------------------------
   FUNCTION:      MMGSDI_SELECT_AID

   DESCRIPTION:
     This function will let the client select from a list of currently available app IDs within the USIM Card.

   DEPENDENCIES:

   LIMITATIONS:

   RETURN VALUE:
     mmgsdi_return_enum_type
     MMGSDI_SUCCESS:          The command structure was properly generated
                              and queued onto the MMGSDI Command Queue.
     MMGSDI_INCORRECT_PARAMS: The parameters supplied to the API are not
                              within appropriate ranges.
     MMGSDI_CMD_QUEUE_FULL:   The command was not Queued to the MMGSDI Task
                              because the max number of commands are already
                              queued.

   SIDE EFFECTS:
-------------------------------------------------------------------------------*/
mmgsdi_return_enum_type  mmgsdi_select_aid (
  mmgsdi_client_id_type              client_id,
  mmgsdi_slot_id_enum_type           card_slot,
  mmgsdi_data_type                   aid,
  mmgsdi_callback_type               response_cb_ptr,
  mmgsdi_client_data_type            client_ref
);
/*~ FUNCTION mmgsdi_select_aid      */
/*~ ONERROR return MMGSDI_RPC_ERROR */


/*===========================================================================
FUNCTION MMGSDI_GET_ALL_PIN_STATUS

DESCRIPTION
  This function will buid a request to the MMGSDI Task to request the PIN
  status for all relevant pins applicable to the client ID provided.
  If the client ID provided is in a session, then the PIN IDs applicable
  to the application in the session will be returned.

PARAMS:
  client_id:         Client Id of the caller
  card_slot:         Physical slot for multiple slot targets.
  response_cb_ptr:   Response callback.
  client_ref:        User Data returned upon completion of this cmd.

===========================================================================*/
mmgsdi_return_enum_type mmgsdi_get_all_pin_status (
  mmgsdi_client_id_type              client_id,
  mmgsdi_slot_id_enum_type           card_slot,
  mmgsdi_callback_type               response_cb_ptr,
  mmgsdi_client_data_type            client_ref
); /* mmgsdi_get_all_pin_status */
/*~ FUNCTION mmgsdi_get_all_pin_status */
/*~ ONERROR return MMGSDI_RPC_ERROR    */


/*===========================================================================
FUNCTION MMGSDI_VERIFY_PIN

DESCRIPTION
  This function will buid a request to the MMGSDI Task to verify the PIN
  provided.  The PIN provided will have to be applicable to the Client
  ID Provided.

  A failure to verify could result in a blocked PIN ID if the maximum
  number of attempts to retry are exhausted.

PARAMS:
  client_id:         Client Id of the caller
  card_slot:         Physical slot for multiple slot targets.
  pin_id:            The PIN Identification to be verified.
  pin_data:          Will contain the len and code of the PIN.
  client_ref:        User Data returned upon completion of this cmd.

This function performs a pin verification procedure against the Card for either PIN1 or PIN2.

DEPENDENCIES
  None

RETURN VALUE
  mmgsdi_return_enum_type
  MMGSDI_SUCCESS:          The command structure was properly generated
                              and queued onto the MMGSDI Command Queue.
     MMGSDI_INCORRECT_PARAMS: The parameters supplied to the API are not
                              within appropriate ranges.
     MMGSDI_CMD_QUEUE_FULL:   The command was not Queued to the MMGSDI Task
                              because the max number of commands are already
                              queued.


SIDE EFFECTS
  None
===========================================================================*/
mmgsdi_return_enum_type mmgsdi_verify_pin (
  mmgsdi_client_id_type           client_id,
  mmgsdi_slot_id_enum_type        card_slot,
  mmgsdi_pin_enum_type            pin_id,
  mmgsdi_data_type                pin_data,
  mmgsdi_callback_type            response_cb_ptr,
  mmgsdi_client_data_type         client_ref
); /* mmgsdi_verify_pin */
/*~ FUNCTION mmgsdi_verify_pin      */
/*~ ONERROR return MMGSDI_RPC_ERROR */


/*===========================================================================
FUNCTION MMGSDI_DISABLE_PIN

DESCRIPTION
  This function will buid a request to the MMGSDI Task to disable the PIN
  provided.  The PIN provided will have to be applicable to the Client
  ID Provided.

  A failure to disable could result in a blocked PIN ID if the maximum
  number of attempts to retry are exhausted.

PARAMS:
  client_id:         Client Id of the caller
  card_slot:         Physical slot for multiple slot targets.
  pin_id:            The PIN Identification to be verified.
  pin_data:          Will contain the len and code of the PIN.
  response_cb_ptr:   Response callback.
  client_ref:        User Data returned upon completion of this cmd.

===========================================================================*/
mmgsdi_return_enum_type mmgsdi_disable_pin (
  mmgsdi_client_id_type              client_id,
  mmgsdi_slot_id_enum_type           card_slot,
  mmgsdi_pin_enum_type               pin_id,
  mmgsdi_data_type                   pin_data,
  mmgsdi_callback_type               response_cb_ptr,
  mmgsdi_client_data_type            client_ref
); /* mmgsdi_disable_pin */
/*~ FUNCTION mmgsdi_disable_pin     */
/*~ ONERROR return MMGSDI_RPC_ERROR */


/*===========================================================================
FUNCTION MMGSDI_ENABLE_PIN

DESCRIPTION
  This function will buid a request to the MMGSDI Task to enable the PIN
  provided.  The PIN provided will have to be applicable to the Client
  ID Provided.

  A failure to enable could result in a blocked PIN ID if the maximum
  number of attempts to retry are exhausted.

PARAMS:
  client_id:         Client Id of the caller
  card_slot:         Physical slot for multiple slot targets.
  pin_id:            The PIN Identification to be verified.
  pin_data:          Will contain the len and code of the PIN.
  response_cb_ptr:   Response callback.
  client_ref:        User Data returned upon completion of this cmd.

===========================================================================*/
mmgsdi_return_enum_type mmgsdi_enable_pin (
  mmgsdi_client_id_type              client_id,
  mmgsdi_slot_id_enum_type           card_slot,
  mmgsdi_pin_enum_type               pin_id,
  mmgsdi_data_type                   pin_data,
  mmgsdi_callback_type               response_cb_ptr,
  mmgsdi_client_data_type            client_ref
); /* mmgsdi_enable_pin */
/*~ FUNCTION mmgsdi_enable_pin      */
/*~ ONERROR return MMGSDI_RPC_ERROR */


/*===========================================================================
FUNCTION MMGSDI_CHANGE_PIN

DESCRIPTION
  This function will buid a request to the MMGSDI Task to change the PIN
  provided.  The PIN provided will have to be applicable to the Client
  ID Provided.

  A failure to change could result in a blocked PIN ID if the maximum
  number of attempts to retry are exhausted.

PARAMS:
  client_id:         Client Id of the caller
  card_slot:         Physical slot for multiple slot targets.
  pin_id:            The PIN Identification to be verified.
  pin_data:          Will contain the len and code of the current PIN.
  new_pin_data:      Will contain the len and code of the new PIN.
  response_cb_ptr:   Response callback.
  client_ref:        User Data returned upon completion of this cmd.


This function performs a change  procedure against the Card for either PIN1 or PIN2.

DEPENDENCIES
None

RETURN VALUE
mmgsdi_return_enum_type
MMGSDI_SUCCESS:          The command structure was properly generated
                         and queued onto the MMGSDI Command Queue.
MMGSDI_INCORRECT_PARAMS: The parameters supplied to the API are not
                         within appropriate ranges.
MMGSDI_CMD_QUEUE_FULL:   The command was not Queued to the MMGSDI Task
                         because the max number of commands are already
                         queued.
SIDE EFFECTS
  None
===================================================================*/
mmgsdi_return_enum_type  mmgsdi_change_pin(
  mmgsdi_client_id_type         client_id,
  mmgsdi_slot_id_enum_type      card_slot,
  mmgsdi_pin_enum_type          pin_id,
  mmgsdi_data_type              pin_data,
  mmgsdi_data_type              new_pin_data,
  mmgsdi_callback_type          response_cb_ptr,
  mmgsdi_client_data_type       client_ref
);
/*~ FUNCTION mmgsdi_change_pin      */
/*~ ONERROR return MMGSDI_RPC_ERROR */


/*===========================================================================
FUNCTION MMGSDI_UNBLOCK_PIN

DESCRIPTION
  This function will buid a request to the MMGSDI Task to unblock the PIN
  provided.  The PIN provided will have to be applicable to the Client
  ID Provided.

  A failure to unblock could result in a permanently blocked PIN ID if
  the maximum number of attempts to unblock are exhausted.

PARAMS:
  client_id:         Client Id of the caller
  card_slot:         Physical slot for multiple slot targets.
  pin_id:            The PIN Identification to be verified.
  puk_data:          Will contain the len and code of the PUK.
  new_pin_data:      Will contain the len and code of the new PIN.
  response_cb_ptr:   Response callback.
  client_ref:        User Data returned upon completion of this cmd.

===========================================================================*/
mmgsdi_return_enum_type  mmgsdi_unblock_pin(
  mmgsdi_client_id_type              client_id,
  mmgsdi_slot_id_enum_type           card_slot,
  mmgsdi_pin_enum_type               pin_id,
  mmgsdi_data_type                   puk_data,
  mmgsdi_data_type                   new_pin_data,
  mmgsdi_callback_type               response_cb_ptr,
  mmgsdi_client_data_type            client_ref
); /* mmgsdi_unblock_pin */
/*~ FUNCTION mmgsdi_unblock_pin     */
/*~ ONERROR return MMGSDI_RPC_ERROR */


/* ============================================================================
   FUNCTION:      MMGSDI_READ_RECORD

   DESCRIPTION:
     This function will provide access to any file in the SIM/USIM/RUIM.
     This library function is intended to encapsulate the details required
     to access a file from a SIM/USIM/RUIM. The client using this library
     function will only be able to access files that require a record
     number.

     Behavior for the function for the following file types:

     TRANSPARENT FILE:  Will return an Error.
     CYCLIC FILE:       Will access the record of the file provided in the
                        Library call.
     LINEAR FIXED FILE: Will access the record of the file provided in the
                        Library call.

   DEPENDENCIES:


   LIMITATIONS:
     TRANSPARENT FILE:  Will return an Error when used to access a transparent
                        File.

   RETURN VALUE:
     mmgsdi_returns_T

     MMGSDI_SUCCESS:          The command structure was properly generated
                              and queued onto the MMGSDI Command Queue.
     MMGSDI_INCORRECT_PARAMS: The parameters supplied to the API are not
                              within appropriate ranges.
     MMGSDI_CMD_QUEUE_FULL:   The command was not Queued to the MMGSDI Task
                              because the max number of commands are already
                              queued.

   SIDE EFFECTS:
  ========================================================================== */
mmgsdi_return_enum_type  mmgsdi_read_record (
  mmgsdi_client_id_type              client_id,
  mmgsdi_slot_id_enum_type           card_slot,
  mmgsdi_file_enum_type              file_name,
  mmgsdi_rec_num_type                record_number,
  mmgsdi_len_type                    request_length,
  mmgsdi_callback_type               response_cb_ptr,
  mmgsdi_client_data_type            client_ref
);
/*~ FUNCTION mmgsdi_read_record     */
/*~ ONERROR return MMGSDI_RPC_ERROR */


/*===========================================================================
   FUNCTION:      MMGSDI_READ_RECORD_EXT

   DESCRIPTION:
     This function will provide access to any file in the SIM/USIM/RUIM.
     It will allow file Enum access and Path access.

     Behavior for the function for the following file types:

     TRANSPARENT FILE:  Will return an Error.
     CYCLIC FILE:       Will access the record of the file provided in the
                        Library call.
     LINEAR FIXED FILE: Will access the record of the file provided in the
                        Library call.

     For Access type:
     MMGSDI_EF_ENUM_ACCESS: Will return the contents as required in the Library
                            Call.
     MMGSDI_BY_PATH_ACCESS: Will return the contents as required in the Library
                            Call.  Note, there is no check whether the path
                            passed in is transparent file or not.  Caller is
                            expected to have the knowledge of whether the file
                            to access is transparent or not.
     others:                Returns an Error.

   DEPENDENCIES:


   LIMITATIONS:
     TRANSPARENT FILE:  Will return an Error when used to access a transparent
                        File.

   RETURN VALUE:
     mmgsdi_returns_T

     MMGSDI_SUCCESS:          The command structure was properly generated
                              and queued onto the MMGSDI Command Queue.
     MMGSDI_INCORRECT_PARAMS: The parameters supplied to the API are not
                              within appropriate ranges.
     MMGSDI_CMD_QUEUE_FULL:   The command was not Queued to the MMGSDI Task
                              because the max number of commands are already
                              queued.

   SIDE EFFECTS:
===========================================================================*/
mmgsdi_return_enum_type  mmgsdi_read_record_ext (
  mmgsdi_client_id_type              client_id,
  mmgsdi_slot_id_enum_type           card_slot,
  mmgsdi_access_type                 file_access,
  mmgsdi_rec_num_type                record_number,
  mmgsdi_len_type                    request_length,
  mmgsdi_callback_type               response_cb_ptr,
  mmgsdi_client_data_type            client_ref
);
/*~ FUNCTION mmgsdi_read_record_ext */
/*~ ONERROR return MMGSDI_RPC_ERROR */


/* ----------------------------------------------------------------------------
   FUNCTION:      MMGSDI_READ_TRANSPARENT

   DESCRIPTION:
     This function will provide read access to any Transparent File. This
     library function is intended to encapsulate the details required to read
     a file to the SIM/USIM/RUIM. The client using this library function will
     only be able to access files that has a flat data structure.

     Behavior for the function for the following file types:

     TRANSPARENT FILE:  Will return the contents as required in the Library
                        Call.
     CYCLIC FILE:       Returns an Error.
     LINEAR FIXED FILE: Returns an Error.

   DEPENDENCIES:
     mmgsdi_client_id_reg() should be called prior to this function

   LIMITATIONS:
     CYCLIC or LINEAR FIXED FILE: Will return an Error when used to access a
                                  CYCLIC FILE or CYCLIC FILE.

   RETURN VALUE:
     mmgsdi_return_enum_type

     MMGSDI_SUCCESS:          The command structure was properly generated
                              and queued onto the MMGSDI Command Queue.
     MMGSDI_INCORRECT_PARAMS: The parameters supplied to the API are not
                              within appropriate ranges.
     MMGSDI_CMD_QUEUE_FULL:   The command was not Queued to the MMGSDI Task
                              because the max number of commands are already
                              queued.

   SIDE EFFECTS:
     None
  ========================================================================== */
mmgsdi_return_enum_type  mmgsdi_read_transparent (
  mmgsdi_client_id_type    client_id,
  mmgsdi_slot_id_enum_type card_slot,
  mmgsdi_file_enum_type    file_name,
  mmgsdi_offset_type       offset,
  mmgsdi_len_type          req_len,
  mmgsdi_callback_type     response_cb_ptr,
  mmgsdi_client_data_type  client_ref
);
/*~ FUNCTION mmgsdi_read_transparent */
/*~ ONERROR return MMGSDI_RPC_ERROR  */


/*===========================================================================
   FUNCTION:      MMGSDI_READ_TRANSPARENT_EXT

   DESCRIPTION:
     This function will provide read access to any Transparent File. It will
     allow file Enum access and Path access.

     Behavior for the function for the following file types:

     TRANSPARENT FILE:  Will return the contents as required in the Library
                        Call.
     CYCLIC FILE:       Returns an Error.
     LINEAR FIXED FILE: Returns an Error.

     For Access type:
     MMGSDI_EF_ENUM_ACCESS: Will return the contents as required in the Library
                            Call.
     MMGSDI_BY_PATH_ACCESS: Will return the contents as required in the Library
                            Call.  Note, there is no check whether the path
                            passed in is transparent file or not.  Caller is
                            expected to have the knowledge of whether the file
                            to access is transparent or not.
     others:                Returns an Error.

   DEPENDENCIES:
     mmgsdi_client_id_reg() should be called prior to this function

   LIMITATIONS:
     CYCLIC or LINEAR FIXED FILE: Will return an Error when used to access a
                                  CYCLIC FILE or CYCLIC FILE.

   RETURN VALUE:
     mmgsdi_return_enum_type

     MMGSDI_SUCCESS:          The command structure was properly generated
                              and queued onto the MMGSDI Command Queue.
     MMGSDI_INCORRECT_PARAMS: The parameters supplied to the API are not
                              within appropriate ranges.
     MMGSDI_CMD_QUEUE_FULL:   The command was not Queued to the MMGSDI Task
                              because the max number of commands are already
                              queued.

   SIDE EFFECTS:
     None
===========================================================================*/
mmgsdi_return_enum_type  mmgsdi_read_transparent_ext (
  mmgsdi_client_id_type    client_id,
  mmgsdi_slot_id_enum_type card_slot,
  mmgsdi_access_type       file_access,
  mmgsdi_offset_type       offset,
  mmgsdi_len_type          req_len,
  mmgsdi_callback_type     response_cb_ptr,
  mmgsdi_client_data_type  client_ref
);
/*~ FUNCTION mmgsdi_read_transparent_ext */
/*~ ONERROR return MMGSDI_RPC_ERROR      */


/* ----------------------------------------------------------------------------
   FUNCTION:      MMGSDI_WRITE_RECORD

   DESCRIPTION:
     This function will provide write access to any record in a Linear Fixed
     Or Cyclic File. This library function is intended to encapsulate the
     details required to write a file to the SIM/USIM/RUIM. The client using
     this library function will only be able to access files that require a
     record number.

     Behavior for the function for the following file types:

     TRANSPARENT FILE:  Will return an Error.
     CYCLIC FILE:       Will write the record of the file provided in the
                        Library call.
     LINEAR FIXED FILE: Will write the record of the file provided in the
                        Library call.

   DEPENDENCIES:


   LIMITATIONS:
     TRANSPARENT FILE:  Will return an Error when used to access a transparent
                        File.

   RETURN VALUE:
     mmgsdi_return_enum_type

     MMGSDI_SUCCESS:          The command structure was properly generated
                              and queued onto the MMGSDI Command Queue.
     MMGSDI_INCORRECT_PARAMS: The parameters supplied to the API are not
                              within appropriate ranges.
     MMGSDI_CMD_QUEUE_FULL:   The command was not Queued to the MMGSDI Task
                              because the max number of commands are already
                              queued.

   SIDE EFFECTS:

  ========================================================================== */
mmgsdi_return_enum_type  mmgsdi_write_record (
  mmgsdi_client_id_type              client_id,
  mmgsdi_slot_id_enum_type           card_slot,
  mmgsdi_file_enum_type              file_name,
  mmgsdi_rec_num_type                record_number,
  mmgsdi_write_data_type             write_data,
  mmgsdi_callback_type               response_cb_ptr,
  mmgsdi_client_data_type            client_ref
);
/*~ FUNCTION mmgsdi_write_record    */
/*~ ONERROR return MMGSDI_RPC_ERROR */


/*===========================================================================
   FUNCTION:      MMGSDI_WRITE_RECORD_EXT

   DESCRIPTION:
     This function will provide write access to any record in a Linear Fixed
     Or Cyclic File.
     It will allow file Enum access and Path access.

     Behavior for the function for the following file types:

     TRANSPARENT FILE:  Will return an Error.
     CYCLIC FILE:       Will write the record of the file provided in the
                        Library call.
     LINEAR FIXED FILE: Will write the record of the file provided in the
                        Library call.

     For Access type:
     MMGSDI_EF_ENUM_ACCESS: Will return the contents as required in the Library
                            Call.
     MMGSDI_BY_PATH_ACCESS: Will return the contents as required in the Library
                            Call.  Note, there is no check whether the path
                            passed in is transparent file or not.  Caller is
                            expected to have the knowledge of whether the file
                            to access is transparent or not.
     others:                Returns an Error.

     For Record type:
     MMGSDI_LINEAR_FIXED_FILE:       Will use ABSOULTE access to card
     MMGSDI_CYCLIC_FILE:             Will use PREVIOUS access to card
     MMGSDI_TRANSPARENT_FILE:        Return an Error
     MMGSDI_MASTER_FILE:             Return an Error
     MMGSDI_DEDICATED_FILE:          Return an Error
     MMGSDI_MAX_FILE_STRUCTURE_ENUM: Return an Error

   DEPENDENCIES:


   LIMITATIONS:
     TRANSPARENT FILE:  Will return an Error when used to access a transparent
                        File.

   RETURN VALUE:
     mmgsdi_returns_T

     MMGSDI_SUCCESS:          The command structure was properly generated
                              and queued onto the MMGSDI Command Queue.
     MMGSDI_INCORRECT_PARAMS: The parameters supplied to the API are not
                              within appropriate ranges.
     MMGSDI_CMD_QUEUE_FULL:   The command was not Queued to the MMGSDI Task
                              because the max number of commands are already
                              queued.

   SIDE EFFECTS:
===========================================================================*/
mmgsdi_return_enum_type  mmgsdi_write_record_ext (
  mmgsdi_client_id_type              client_id,
  mmgsdi_slot_id_enum_type           card_slot,
  mmgsdi_access_type                 file_access,
  mmgsdi_file_structure_enum_type    record_type,
  mmgsdi_rec_num_type                record_number,
  mmgsdi_write_data_type             write_data,
  mmgsdi_callback_type               response_cb_ptr,
  mmgsdi_client_data_type            client_ref
);
/*~ FUNCTION mmgsdi_write_record_ext */
/*~ ONERROR return MMGSDI_RPC_ERROR  */


/* ----------------------------------------------------------------------------
   FUNCTION:      MMGSDI_WRITE_TRANSPARENT

   DESCRIPTION:
     This function will provide write access to any Transparent File. This
     library function is intended to encapsulate the details required to write
     a file to the SIM/USIM/RUIM. The client using this library function will
     only be able to access files that has a flat data structure.

     Behavior for the function for the following file types:

     TRANSPARENT FILE:  write the data provided in the Library call.
     CYCLIC FILE:       Returns an Error.
     LINEAR FIXED FILE: Returns an Error.

   DEPENDENCIES:
     mmgsdi_client_id_reg() should be called prior to this function


   LIMITATIONS:
     CYCLIC or LINEAR FIXED FILE: Will return an Error when used to access a
                                  Record based File.

   RETURN VALUE:
     mmgsdi_return_enum_type

     MMGSDI_SUCCESS:          The command structure was properly generated
                              and queued onto the MMGSDI Command Queue.
     MMGSDI_INCORRECT_PARAMS: The parameters supplied to the API are not
                              within appropriate ranges.
     MMGSDI_CMD_QUEUE_FULL:   The command was not Queued to the MMGSDI Task
                              because the max number of commands are already
                              queued.

   SIDE EFFECTS:
     MMGSDI cache will be updated if the requested data is located in the cache
  ========================================================================== */
mmgsdi_return_enum_type  mmgsdi_write_transparent (
  mmgsdi_client_id_type    client_id,
  mmgsdi_slot_id_enum_type card_slot,
  mmgsdi_file_enum_type    file_name,
  mmgsdi_offset_type       offset,
  mmgsdi_write_data_type   write_data,
  mmgsdi_callback_type     response_cb_ptr,
  mmgsdi_client_data_type  client_ref
);
/*~ FUNCTION mmgsdi_write_transparent */
/*~ ONERROR return MMGSDI_RPC_ERROR   */


/*===========================================================================
   FUNCTION:      MMGSDI_WRITE_TRANSPARENT_EXT

   DESCRIPTION:
     This function will provide write access to any Transparent File.
     It will allow file Enum access and Path access.

     Behavior for the function for the following file types:

     TRANSPARENT FILE:  write the data provided in the Library call.
     CYCLIC FILE:       Returns an Error.
     LINEAR FIXED FILE: Returns an Error.

     For Access type:
     MMGSDI_EF_ENUM_ACCESS: Will return the contents as required in the Library
                            Call.
     MMGSDI_BY_PATH_ACCESS: Will return the contents as required in the Library
                            Call.  Note, there is no check whether the path
                            passed in is transparent file or not.  Caller is
                            expected to have the knowledge of whether the file
                            to access is transparent or not.
     others:                Returns an Error.

   DEPENDENCIES:


   LIMITATIONS:
     TRANSPARENT FILE:  Will return an Error when used to access a transparent
                        File.

   RETURN VALUE:
     mmgsdi_returns_T

     MMGSDI_SUCCESS:          The command structure was properly generated
                              and queued onto the MMGSDI Command Queue.
     MMGSDI_INCORRECT_PARAMS: The parameters supplied to the API are not
                              within appropriate ranges.
     MMGSDI_CMD_QUEUE_FULL:   The command was not Queued to the MMGSDI Task
                              because the max number of commands are already
                              queued.

   SIDE EFFECTS:
===========================================================================*/
mmgsdi_return_enum_type  mmgsdi_write_transparent_ext (
  mmgsdi_client_id_type    client_id,
  mmgsdi_slot_id_enum_type card_slot,
  mmgsdi_access_type       file_access,
  mmgsdi_offset_type       offset,
  mmgsdi_write_data_type   write_data,
  mmgsdi_callback_type     response_cb_ptr,
  mmgsdi_client_data_type  client_ref
);
/*~ FUNCTION mmgsdi_write_transparent_ext */
/*~ ONERROR return MMGSDI_RPC_ERROR       */


/* ----------------------------------------------------------------------------
   FUNCTION:      MMGSDI_GET_FILE_ATTR

   DESCRIPTION:
     This function will provide get file attributes to any file in the
     SIM/USIM/RUIM.

     Behavior for the function for the following file types:

     TRANSPARENT FILE:  Will get information on the characteristics of
                        Transparent file
     CYCLIC FILE:       Will get information on the characteristics of
                        Cyclic file.
     LINEAR FIXED FILE: Will get information on the characteristics of
                        Linear Fixed file.
     MASTER FILE:       Will get information on the Master File
     DEDICATED FILE:    Will get information on the Dedicated File

   DEPENDENCIES:


   LIMITATIONS:


   RETURN VALUE:
     mmgsdi_return_enum_type

     MMGSDI_SUCCESS:          The command structure was properly generated
                              and queued onto the MMGSDI Command Queue.
     MMGSDI_INCORRECT_PARAMS: The parameters supplied to the API are not
                              within appropriate ranges.
     MMGSDI_CMD_QUEUE_FULL:   The command was not Queued to the MMGSDI Task
                              because the max number of commands are already
                              queued.

   SIDE EFFECTS:

-------------------------------------------------------------------------------*/
mmgsdi_return_enum_type  mmgsdi_get_file_attr (
  mmgsdi_client_id_type    client_id,
  mmgsdi_slot_id_enum_type card_slot,
  mmgsdi_access_type       file_name,
  mmgsdi_callback_type     response_cb_ptr,
  mmgsdi_client_data_type  client_ref
);
/*~ FUNCTION mmgsdi_get_file_attr   */
/*~ ONERROR return MMGSDI_RPC_ERROR */


/* ============================================================================
   FUNCTION:      MMGSDI_CLIENT_ID_FORCE_CLEANUP

   DESCRIPTION:
     This function performs cleanup for client id when RPC has indicated that
     the application processor client(s) has/have been terminated.

   PARAMETERS:
     response_cb_ptr: Input  Callback that maps to the client id which enables
                             MMGSDI to perform corresponding client id 
                             deregistration on the client's behalf.

   DEPENDENCIES:
     None

   LIMITATIONS:
     None

   RETURN VALUE:
     None

   SIDE EFFECTS:
     Will result in the client id(s) that were linked to the corresponding
     response_cb_ptr during the mmgsdi_client_id_reg to be deregistered.
  ========================================================================== */
void mmgsdi_client_id_force_cleanup(mmgsdi_callback_type response_cb_ptr);
/*~ FUNCTION mmgsdi_client_id_force_cleanup */


/* ============================================================================
   FUNCTION:      MMGSDI_CLIENT_ID_REG

   DESCRIPTION:
     This function allows client registration for the use of MMGSDI API.
     The response to this request will return client ID to the calling task that
     is required to be used for subsequent MMGSDI API calls.

   PARAMETERS:
   mmgsdi_callback_type response_cb_ptr:    Input.  Callback for MMGSDI to call
                                                    upon completion of the
                                                    registration request.
   mmgsdi_client_data_type client_ref_ptr:  Input.  User Data returned to the user
                                                    upon completion of the command.


   DEPENDENCIES:
     None

   LIMITATIONS:
     None

   RETURN VALUE:
     mmgsdi_return_enum_type

     MMGSDI_SUCCESS:          The command structure was properly generated
                              and queued onto the MMGSDI Command Queue.
     MMGSDI_INCORRECT_PARAMS: The parameters supplied to the API are not
                              within appropriate ranges.
     MMGSDI_CMD_QUEUE_FULL:   The command was not Queued to the MMGSDI Task
                              because the max number of commands are already
                              queued.

   SIDE EFFECTS:
     Will result in your callback getting called everytime MMGSDI Needs to notify
     any client of any events.
  ========================================================================== */
mmgsdi_return_enum_type  mmgsdi_client_id_reg (
  mmgsdi_callback_type       response_cb_ptr,
  mmgsdi_client_data_type    client_ref
);
/*~ FUNCTION mmgsdi_client_id_reg RELEASE_FUNC mmgsdi_client_id_force_cleanup(response_cb_ptr) */
/*~ ONERROR return MMGSDI_RPC_ERROR */


/* ============================================================================
   FUNCTION:      MMGSDI_CLIENT_EVT_REG

   DESCRIPTION:
     This function allows client registration for new event callback that will
     contain additional info based on specific events being sent.
     The response to this request will return client ID to the calling task.

   PARAMETERS:
   mmgsdi_client_id_type      client_id:    Input.  Client ID
   mmgsdi_callback_type * evt_cb_ptr:       Input.  Pointer to callback for event
                                                    notifications.  If NULL is passed
                                                    the event callback will be
                                                    deregistered.
   mmgsdi_callback_type response_cb_ptr:    Input.  Callback for MMGSDI to call
                                                    upon completion of the
                                                    registration request.
   mmgsdi_client_data_type client_ref_ptr:  Input.  User Data returned to the user
                                                    upon completion of the command.


   DEPENDENCIES:
     None

   LIMITATIONS:
     None

   RETURN VALUE:
     mmgsdi_return_enum_type

     MMGSDI_SUCCESS:          The command structure was properly generated
                              and queued onto the MMGSDI Command Queue.
     MMGSDI_INCORRECT_PARAMS: The parameters supplied to the API are not
                              within appropriate ranges.
     MMGSDI_CMD_QUEUE_FULL:   The command was not Queued to the MMGSDI Task
                              because the max number of commands are already
                              queued.

   SIDE EFFECTS:
     Will result in your callback getting called everytime MMGSDI Needs to notify
     any client of any events.
  ========================================================================== */
mmgsdi_return_enum_type  mmgsdi_client_evt_reg (
  mmgsdi_client_id_type      client_id,
  mmgsdi_evt_callback_type * evt_cb_ptr,
  mmgsdi_callback_type       response_cb_ptr,
  mmgsdi_client_data_type    client_ref
);
/*~ FUNCTION mmgsdi_client_evt_reg  */
/*~ PARAM evt_cb_ptr POINTER        */
/*~ ONERROR return MMGSDI_RPC_ERROR */


/* ============================================================================
   FUNCTION:      MMGSDI_CLIENT_ID_DEREG

   DESCRIPTION:
     This function allows client deregistration for the use of MMGSDI API.

   PARAMETERS:
   mmgsdi_client_id_type      client_id:    Input.  Client ID
   mmgsdi_callback_type response_cb_ptr:    Input.  Callback for MMGSDI to call
                                                    upon completion of the
                                                    registration request.
   mmgsdi_client_data_type client_ref_ptr:  Input.  User Data returned to the user
                                                    upon completion of the command.


   DEPENDENCIES:
     None

   LIMITATIONS:
     None

   RETURN VALUE:
     mmgsdi_return_enum_type

     MMGSDI_SUCCESS:          The command structure was properly generated
                              and queued onto the MMGSDI Command Queue.
     MMGSDI_INCORRECT_PARAMS: The parameters supplied to the API are not
                              within appropriate ranges.
     MMGSDI_CMD_QUEUE_FULL:   The command was not Queued to the MMGSDI Task
                              because the max number of commands are already
                              queued.

   SIDE EFFECTS:
     Will result in your callback getting called everytime MMGSDI Needs to notify
     any client of any events.
  ========================================================================== */
mmgsdi_return_enum_type  mmgsdi_client_id_dereg (
  mmgsdi_client_id_type      client_id,
  mmgsdi_callback_type       response_cb_ptr,
  mmgsdi_client_data_type    client_ref
);
/*~ FUNCTION mmgsdi_client_id_dereg */
/*~ ONERROR return MMGSDI_RPC_ERROR */


/* ============================================================================
   FUNCTION:      MMGSDI_CLIENT_EVT_DEREG

   DESCRIPTION:
     This function allows client deregistration for new event callback that will
     contain additional info based on specific events being sent.
     The response to this request will return client ID to the calling task.

   PARAMETERS:
   mmgsdi_client_id_type      client_id:    Input.  Client ID
   mmgsdi_callback_type * evt_cb_ptr:       Input.  Pointer to callback for event
                                                    notifications.  If NULL is passed
                                                    the event callback will be
                                                    deregistered.
   mmgsdi_callback_type response_cb_ptr:    Input.  Callback for MMGSDI to call
                                                    upon completion of the
                                                    registration request.
   mmgsdi_client_data_type client_ref_ptr:  Input.  User Data returned to the user
                                                    upon completion of the command.


   DEPENDENCIES:
     None

   LIMITATIONS:
     None

   RETURN VALUE:
     mmgsdi_return_enum_type

     MMGSDI_SUCCESS:          The command structure was properly generated
                              and queued onto the MMGSDI Command Queue.
     MMGSDI_INCORRECT_PARAMS: The parameters supplied to the API are not
                              within appropriate ranges.
     MMGSDI_CMD_QUEUE_FULL:   The command was not Queued to the MMGSDI Task
                              because the max number of commands are already
                              queued.

   SIDE EFFECTS:
     Will result in your callback getting called everytime MMGSDI Needs to notify
     any client of any events.
  ========================================================================== */
mmgsdi_return_enum_type  mmgsdi_client_evt_dereg (
  mmgsdi_client_id_type      client_id,
  mmgsdi_evt_callback_type * evt_cb_ptr,
  mmgsdi_callback_type       response_cb_ptr,
  mmgsdi_client_data_type    client_ref
);
/*~ FUNCTION mmgsdi_client_evt_dereg */
/*~ ONERROR return MMGSDI_RPC_ERROR  */


/* ============================================================================
FUNCTION:      MMGSDI_SESSION_MANAGEMENT_OPEN

DESCRIPTION:
  This function allows the client to open a Session to a an Application
  on the card.  The session opened will point to the USIM Application for
  which the Application Identifier provided Pertains to.

  Upon successful completion of this command, the client_id will have an
  associated logical channel and an associated UICC Application Identifier.

  Description of the parameters:
  mmgsdi_client_id_type    client_id:      Input.  Client ID
  mmgsdi_slot_id_enum_type card_slot:      Input.  Physical slot for multiple
                                                   slot targets.
  mmgsdi_data_type         app_id_data:    Input.  Application Identifier
  mmgsdi_client_data_type  client_ref_ptr: Input.  User Data returned upon
                                                  completion of this cmd.

DEPENDENCIES:
  The client must have a valid client ID.  If the client ID is not part
  of the client_id_table, and failure will be returned in the response.
  Achived by calling mmgsdi_client_id_reg().

LIMITATIONS:
  None.

RETURN VALUE:
  mmgsdi_return_enum_type:

    MMGSDI_SUCCESS:          The command structure was properly generated
                             and queued onto the MMGSDI Command Queue.
    MMGSDI_INCORRECT_PARAMS: The parameters supplied to the API are not
                             within appropriate ranges.
    MMGSDI_CMD_QUEUE_FULL:   The command was not Queued to the MMGSDI Task
                             because the max number of commands are already
                             queued.

SIDE EFFECTS:
  Will result in your callback getting called everytime MMGSDI Needs to notify
  any client of any events.
  ========================================================================== */
mmgsdi_return_enum_type  mmgsdi_session_management_open (
  mmgsdi_client_id_type    client_id,
  mmgsdi_slot_id_enum_type card_slot,
  mmgsdi_data_type         app_id_data,
  mmgsdi_callback_type     response_cb_ptr,
  mmgsdi_client_data_type  client_ref
);
/*~ FUNCTION mmgsdi_session_management_open */
/*~ ONERROR return MMGSDI_RPC_ERROR         */


/* ============================================================================
FUNCTION:      MMGSDI_SESSION_MANAGEMENT_CLOSE

DESCRIPTION:
  This function is used to close the currently active session with the
  UICC Application communicating over a UICC Logical Channel.

  When called, the normal UICC Application Deactivation procedures are
  performed on the Application ID Associated with the Client ID.

  The logical channel associated with the client ID will also be released.

  Upon successful completion of this command, the client_id will be valid
  and existent in the client_id_table but the logical channel and
  application id will be removed.

  Description of the parameters:
  mmgsdi_client_id_type    client_id:      Input.  Client ID
  mmgsdi_slot_id_enum_type card_slot:      Input.  Physical slot for multiple
                                                   slot targets.
  mmgsdi_client_data_type  client_ref:     Input.  User Data returned upon
                                                  completion of this cmd.

DEPENDENCIES:
  The client must have a valid client ID.  If the client ID is not part
  of the client_id_table, and failure will be returned in the response.
  Achived by calling mmgsdi_client_id_reg().

  The client must also have a valid session open.
  Achived by calling mmgsdi_session_management_open.

LIMITATIONS:
  None.

RETURN VALUE:
  mmgsdi_return_enum_type:

    MMGSDI_SUCCESS:          The command structure was properly generated
                             and queued onto the MMGSDI Command Queue.
    MMGSDI_INCORRECT_PARAMS: The parameters supplied to the API are not
                             within appropriate ranges.
    MMGSDI_CMD_QUEUE_FULL:   The command was not Queued to the MMGSDI Task
                             because the max number of commands are already
                             queued.

SIDE EFFECTS:
  Will result in your callback getting called everytime MMGSDI Needs to notify
  any client of any events.
  ========================================================================== */
mmgsdi_return_enum_type  mmgsdi_session_management_close (
  mmgsdi_client_id_type    client_id,
  mmgsdi_slot_id_enum_type card_slot,
  mmgsdi_callback_type     response_cb_ptr,
  mmgsdi_client_data_type  client_ref
);
/*~ FUNCTION mmgsdi_session_management_close */
/*~ ONERROR return MMGSDI_RPC_ERROR          */


/* ============================================================================
FUNCTION:      MMGSDI_ISIM_AUTHENTICATE

DESCRIPTION:
  This function is used to perform a "Network" Initiated Authentication with
  the ISIM Application.  This is only valid and applicable when an ISIM
  Application is available as specified in 31.103.  This function will
  perform a check for the ISIM Application before sending the command.

  Description of the parameters:
  mmgsdi_client_id_type    client_id:      Input.  Client Id of the caller
  mmgsdi_slot_id_enum_type card_slot:      Input.  Physical slot for multiple
                                                   slot targets.
  mmgsdi_data_type         auth_req:       Input.  Length and Data of auth
                                                   challenge.
  mmgsdi_client_data_type  client_ref:     Input.  User Data returned upon
                                                   completion of this cmd.

DEPENDENCIES:
  The client must have a valid client ID.  If the client ID is not part
  of the client_id_table, and failure will be returned in the response.
  Achived by calling mmgsdi_client_id_reg().

  The client must also have a valid session open.
  Achived by calling mmgsdi_session_management_open.

LIMITATIONS:
  None.

RETURN VALUE:
  mmgsdi_return_enum_type:

    MMGSDI_SUCCESS:          The command structure was properly generated
                             and queued onto the MMGSDI Command Queue.
    MMGSDI_INCORRECT_PARAMS: The parameters supplied to the API are not
                             within appropriate ranges.
    MMGSDI_CMD_QUEUE_FULL:   The command was not Queued to the MMGSDI Task
                             because the max number of commands are already
                             queued.

SIDE EFFECTS:
  Will result in your callback getting called everytime MMGSDI Needs to notify
  any client of any events.
  ========================================================================== */
mmgsdi_return_enum_type  mmgsdi_isim_authenticate (
  mmgsdi_client_id_type    client_id,
  mmgsdi_slot_id_enum_type card_slot,
  mmgsdi_data_type         auth_req,
  mmgsdi_callback_type     response_cb_ptr,
  mmgsdi_client_data_type  client_ref
);
/*~ FUNCTION mmgsdi_isim_authenticate */
/*~ ONERROR return MMGSDI_RPC_ERROR   */


/* ============================================================================
FUNCTION MMGSDI_SAP_CONNECT

DESCRIPTION
  This function attempts to perform a SAP connect

PARAMS:
  client_id:       Client Id of the caller
  card_slot:       Physical slot for multiple slot targets.
  sap_mode:        Client/Server.
  response_cb_ptr: Response callback.
  client_ref:      User Data returned upon completion of this cmd.

DEPENDENCIES:
  The client must have a valid client ID.  If the client ID is not part
  of the client_id_table, and failure will be returned in the response.
  Achived by calling mmgsdi_client_id_reg().

LIMITATIONS:
  None.

RETURN VALUE:
  mmgsdi_return_enum_type:

    MMGSDI_SUCCESS:          The command structure was properly generated
                             and queued onto the MMGSDI Command Queue.
    MMGSDI_INCORRECT_PARAMS: The parameters supplied to the API are not
                             within appropriate ranges.
    MMGSDI_CMD_QUEUE_FULL:   The command was not Queued to the MMGSDI Task
                             because the max number of commands are already
                             queued.

SIDE EFFECTS:
  None
  ========================================================================== */
mmgsdi_return_enum_type mmgsdi_sap_connect(
    mmgsdi_client_id_type     client_id,
    mmgsdi_slot_id_enum_type  card_slot,
    mmgsdi_sap_mode_enum_type sap_mode,
    mmgsdi_callback_type      response_cb_ptr,
    mmgsdi_client_data_type   client_ref
);
/*~ FUNCTION mmgsdi_sap_connect     */
/*~ ONERROR return MMGSDI_RPC_ERROR */


/* ============================================================================
FUNCTION MMGSDI_SAP_DISCONNECT

DESCRIPTION
  This function attempts to perform a SAP disconnect

PARAMS:
  client_id:       Client Id of the caller
  card_slot:       Physical slot for multiple slot targets.
  sap_mode:        Client/Server.
  disconnect_mode: Graceful/Immediate.
  response_cb_ptr: Response callback.
  client_ref:      User Data returned upon completion of this cmd.

DEPENDENCIES:
  The client must have a valid client ID.  If the client ID is not part
  of the client_id_table, and failure will be returned in the response.
  Achived by calling mmgsdi_client_id_reg().

LIMITATIONS:
  None.

RETURN VALUE:
  mmgsdi_return_enum_type:

    MMGSDI_SUCCESS:          The command structure was properly generated
                             and queued onto the MMGSDI Command Queue.
    MMGSDI_INCORRECT_PARAMS: The parameters supplied to the API are not
                             within appropriate ranges.
    MMGSDI_CMD_QUEUE_FULL:   The command was not Queued to the MMGSDI Task
                             because the max number of commands are already
                             queued.

SIDE EFFECTS:
  None
  ========================================================================== */
mmgsdi_return_enum_type mmgsdi_sap_disconnect(
  mmgsdi_client_id_type               client_id,
  mmgsdi_slot_id_enum_type            card_slot,
  mmgsdi_sap_mode_enum_type           sap_mode,
  mmgsdi_disconnect_mode_enum_type    disconnect_mode,
  mmgsdi_callback_type                response_cb_ptr,
  mmgsdi_client_data_type             client_ref
);
/*~ FUNCTION mmgsdi_sap_disconnect  */
/*~ ONERROR return MMGSDI_RPC_ERROR */


/* ============================================================================
FUNCTION MMGSDI_SAP_GET_ATR

DESCRIPTION:
  This is the API used to retrieve the ATR Information which pertains to the
  card(s) currently inserted in the ME.

PARAMS:
  client_id:       Client Id of the caller
  card_slot:       Physical slot for multiple slot targets.
  response_cb_ptr: Response callback.
  client_ref:      User Data returned upon completion of this cmd.

DEPENDENCIES:
  The client must have a valid client ID.  If the client ID is not part
  of the client_id_table, and failure will be returned in the response.
  Achived by calling mmgsdi_client_id_reg().

LIMITATIONS:
  None.

RETURN VALUE:
  mmgsdi_return_enum_type:

    MMGSDI_SUCCESS:          The command structure was properly generated
                             and queued onto the MMGSDI Command Queue.
    MMGSDI_INCORRECT_PARAMS: The parameters supplied to the API are not
                             within appropriate ranges.
    MMGSDI_CMD_QUEUE_FULL:   The command was not Queued to the MMGSDI Task
                             because the max number of commands are already
                             queued.

SIDE EFFECTS:
  None
  ========================================================================== */
mmgsdi_return_enum_type mmgsdi_sap_get_atr(
  mmgsdi_client_id_type    client_id,
  mmgsdi_slot_id_enum_type card_slot,
  mmgsdi_callback_type     response_cb_ptr,
  mmgsdi_client_data_type  client_ref
);
/*~ FUNCTION mmgsdi_sap_get_atr     */
/*~ ONERROR return MMGSDI_RPC_ERROR */


/* ============================================================================
FUNCTION MMGSDI_SAP_SEND_APDU

DESCRIPTION:
  This is the API used to send an APDU command to the SIM.

PARAMS:
  client_id:       Client Id of the caller
  card_slot:       Physical slot for multiple slot targets.
  channel_id:      The channel id.
  apdu_data:       The APDU to send to the card.
  response_cb_ptr: Response callback.
  client_ref:      User Data returned upon completion of this cmd.

DEPENDENCIES:
  The client must have a valid client ID.  If the client ID is not part
  of the client_id_table, and failure will be returned in the response.
  Achived by calling mmgsdi_client_id_reg().

LIMITATIONS:
  None.

RETURN VALUE:
  mmgsdi_return_enum_type:

    MMGSDI_SUCCESS:          The command structure was properly generated
                             and queued onto the MMGSDI Command Queue.
    MMGSDI_INCORRECT_PARAMS: The parameters supplied to the API are not
                             within appropriate ranges.
    MMGSDI_CMD_QUEUE_FULL:   The command was not Queued to the MMGSDI Task
                             because the max number of commands are already
                             queued.

SIDE EFFECTS:
  None
  ========================================================================== */
mmgsdi_return_enum_type mmgsdi_sap_send_apdu(
  mmgsdi_client_id_type          client_id,
  mmgsdi_slot_id_enum_type       card_slot,
  int32                          channel_id,
  mmgsdi_sap_send_apdu_data_type apdu_data,
  mmgsdi_callback_type           response_cb_ptr,
  mmgsdi_client_data_type        client_ref
);
/*~ FUNCTION mmgsdi_sap_send_apdu   */
/*~ ONERROR return MMGSDI_RPC_ERROR */


/* ============================================================================
FUNCTION MMGSDI_SAP_POWER_ON

DESCRIPTION
  This function attempts to perform a SIM power on.

PARAMS:
  client_id:       Client Id of the caller
  card_slot:       Physical slot for multiple slot targets.
  sap_mode:        Client/Server.
  response_cb_ptr: Response callback.
  client_ref:      User Data returned upon completion of this cmd.

DEPENDENCIES:
  The client must have a valid client ID.  If the client ID is not part
  of the client_id_table, and failure will be returned in the response.
  Achived by calling mmgsdi_client_id_reg().

LIMITATIONS:
  None.

RETURN VALUE:
  mmgsdi_return_enum_type:

    MMGSDI_SUCCESS:          The command structure was properly generated
                             and queued onto the MMGSDI Command Queue.
    MMGSDI_INCORRECT_PARAMS: The parameters supplied to the API are not
                             within appropriate ranges.
    MMGSDI_CMD_QUEUE_FULL:   The command was not Queued to the MMGSDI Task
                             because the max number of commands are already
                             queued.

SIDE EFFECTS:
  None
  ========================================================================== */
mmgsdi_return_enum_type mmgsdi_sap_power_on(
    mmgsdi_client_id_type     client_id,
    mmgsdi_slot_id_enum_type  card_slot,
    mmgsdi_sap_mode_enum_type sap_mode,
    mmgsdi_callback_type      response_cb_ptr,
    mmgsdi_client_data_type   client_ref
);
/*~ FUNCTION mmgsdi_sap_power_on    */
/*~ ONERROR return MMGSDI_RPC_ERROR */


/* ============================================================================
FUNCTION MMGSDI_SAP_POWER_OFF

DESCRIPTION
  This function attempts to perform a SIM power off.

PARAMS:
  client_id:       Client Id of the caller
  card_slot:       Physical slot for multiple slot targets.
  sap_mode:        Client/Server.
  response_cb_ptr: Response callback.
  client_ref:      User Data returned upon completion of this cmd.

DEPENDENCIES:
  The client must have a valid client ID.  If the client ID is not part
  of the client_id_table, and failure will be returned in the response.
  Achived by calling mmgsdi_client_id_reg().

LIMITATIONS:
  None.

RETURN VALUE:
  mmgsdi_return_enum_type:

    MMGSDI_SUCCESS:          The command structure was properly generated
                             and queued onto the MMGSDI Command Queue.
    MMGSDI_INCORRECT_PARAMS: The parameters supplied to the API are not
                             within appropriate ranges.
    MMGSDI_CMD_QUEUE_FULL:   The command was not Queued to the MMGSDI Task
                             because the max number of commands are already
                             queued.

SIDE EFFECTS:
  None
  ========================================================================== */
mmgsdi_return_enum_type mmgsdi_sap_power_off(
    mmgsdi_client_id_type     client_id,
    mmgsdi_slot_id_enum_type  card_slot,
    mmgsdi_sap_mode_enum_type sap_mode,
    mmgsdi_callback_type      response_cb_ptr,
    mmgsdi_client_data_type   client_ref
);
/*~ FUNCTION mmgsdi_sap_power_off   */
/*~ ONERROR return MMGSDI_RPC_ERROR */


/* ============================================================================
FUNCTION MMGSDI_SAP_CARD_READER_STATUS

DESCRIPTION
  This function attempts to perform a SIM card reader status

PARAMS:
  client_id:       Client Id of the caller
  card_slot:       Physical slot for multiple slot targets.
  sap_mode:        Client/Server.
  response_cb_ptr: Response callback.
  client_ref:      User Data returned upon completion of this cmd.

DEPENDENCIES:
  The client must have a valid client ID.  If the client ID is not part
  of the client_id_table, and failure will be returned in the response.
  Achived by calling mmgsdi_client_id_reg().

LIMITATIONS:
  None.

RETURN VALUE:
  mmgsdi_return_enum_type:

    MMGSDI_SUCCESS:          The command structure was properly generated
                             and queued onto the MMGSDI Command Queue.
    MMGSDI_INCORRECT_PARAMS: The parameters supplied to the API are not
                             within appropriate ranges.
    MMGSDI_CMD_QUEUE_FULL:   The command was not Queued to the MMGSDI Task
                             because the max number of commands are already
                             queued.

SIDE EFFECTS:
  None
  ========================================================================== */
mmgsdi_return_enum_type mmgsdi_sap_card_reader_status(
    mmgsdi_client_id_type     client_id,
    mmgsdi_slot_id_enum_type  card_slot,
    mmgsdi_sap_mode_enum_type sap_mode,
    mmgsdi_callback_type      response_cb_ptr,
    mmgsdi_client_data_type   client_ref
);
/*~ FUNCTION mmgsdi_sap_card_reader_status */
/*~ ONERROR return MMGSDI_RPC_ERROR        */


/* ============================================================================
FUNCTION MMGSDI_SAP_RESET

DESCRIPTION
  This function attempts to perform a SIM reset.

PARAMS:
  client_id:       Client Id of the caller
  card_slot:       Physical slot for multiple slot targets.
  sap_mode:        Client/Server.
  response_cb_ptr: Response callback.
  client_ref:      User Data returned upon completion of this cmd.

DEPENDENCIES:
  The client must have a valid client ID.  If the client ID is not part
  of the client_id_table, and failure will be returned in the response.
  Achived by calling mmgsdi_client_id_reg().

LIMITATIONS:
  None.

RETURN VALUE:
  mmgsdi_return_enum_type:

    MMGSDI_SUCCESS:          The command structure was properly generated
                             and queued onto the MMGSDI Command Queue.
    MMGSDI_INCORRECT_PARAMS: The parameters supplied to the API are not
                             within appropriate ranges.
    MMGSDI_CMD_QUEUE_FULL:   The command was not Queued to the MMGSDI Task
                             because the max number of commands are already
                             queued.

SIDE EFFECTS:
  None
  ========================================================================== */
mmgsdi_return_enum_type mmgsdi_sap_reset(
    mmgsdi_client_id_type     client_id,
    mmgsdi_slot_id_enum_type  card_slot,
    mmgsdi_sap_mode_enum_type sap_mode,
    mmgsdi_callback_type      response_cb_ptr,
    mmgsdi_client_data_type   client_ref
);
/*~ FUNCTION mmgsdi_sap_reset       */
/*~ ONERROR return MMGSDI_RPC_ERROR */


/* ============================================================================
FUNCTION MMGSDI_SAP_PIN_VERIFY

DESCRIPTION
  This function attempts to perform a SAP PIN Verification.

PARAMS:
  client_id:       Client Id of the caller
  card_slot:       Physical slot for multiple slot targets.
  sap_pin:         SAP Pin.
  response_cb_ptr: Response callback.
  client_ref:      User Data returned upon completion of this cmd.

DEPENDENCIES:
  The client must have a valid client ID.  If the client ID is not part
  of the client_id_table, and failure will be returned in the response.
  Achived by calling mmgsdi_client_id_reg().

LIMITATIONS:
  None.

RETURN VALUE:
  mmgsdi_return_enum_type:

    MMGSDI_SUCCESS:          The command structure was properly generated
                             and queued onto the MMGSDI Command Queue.
    MMGSDI_INCORRECT_PARAMS: The parameters supplied to the API are not
                             within appropriate ranges.
    MMGSDI_CMD_QUEUE_FULL:   The command was not Queued to the MMGSDI Task
                             because the max number of commands are already
                             queued.

SIDE EFFECTS:
  None
  ========================================================================== */
mmgsdi_return_enum_type mmgsdi_sap_pin_verify(
  mmgsdi_client_id_type     client_id,
  mmgsdi_slot_id_enum_type  card_slot,
  mmgsdi_bt_bd_address_type bd_addr,
  mmgsdi_data_type          sap_pin,
  mmgsdi_callback_type      response_cb_ptr,
  mmgsdi_client_data_type   client_ref
);
/*~ FUNCTION mmgsdi_sap_pin_verify  */
/*~ ONERROR return MMGSDI_RPC_ERROR */


/* ============================================================================
FUNCTION MMGSDI_SAP_INITIALIZE

DESCRIPTION
  This function attempts to perform a SAP initialization.  This will set
  the mobile to be either SAP Client mode or SAP Server mode.

PARAMS:
  client_id:       Client Id of the caller
  card_slot:       Physical slot for multiple slot targets.
  sap_mode:        Client/Server.
  response_cb_ptr: Response callback.
  client_ref:      User Data returned upon completion of this cmd.

DEPENDENCIES:
  The client must have a valid client ID.  If the client ID is not part
  of the client_id_table, and failure will be returned in the response.
  Achived by calling mmgsdi_client_id_reg().

LIMITATIONS:
  None.

RETURN VALUE:
  mmgsdi_return_enum_type:

    MMGSDI_SUCCESS:          The command structure was properly generated
                             and queued onto the MMGSDI Command Queue.
    MMGSDI_INCORRECT_PARAMS: The parameters supplied to the API are not
                             within appropriate ranges.
    MMGSDI_CMD_QUEUE_FULL:   The command was not Queued to the MMGSDI Task
                             because the max number of commands are already
                             queued.

SIDE EFFECTS:
  None
  ========================================================================== */
mmgsdi_return_enum_type mmgsdi_sap_initialize(
  mmgsdi_client_id_type     client_id,
  mmgsdi_slot_id_enum_type  card_slot,
  mmgsdi_sap_mode_enum_type sap_mode,
  mmgsdi_callback_type      response_cb_ptr,
  mmgsdi_client_data_type   client_ref
);
/*~ FUNCTION mmgsdi_sap_initialize  */
/*~ ONERROR return MMGSDI_RPC_ERROR */


/* ============================================================================
FUNCTION MMGSDI_SAP_DEREGISTER

DESCRIPTION
  This function attempts to perform a SAP deregistration.

PARAMS:
  client_id:       Client Id of the caller
  card_slot:       Physical slot for multiple slot targets.
  sap_mode:        Client/Server.
  response_cb_ptr: Response callback.
  client_ref:      User Data returned upon completion of this cmd.

DEPENDENCIES:
  The client must have a valid client ID.  If the client ID is not part
  of the client_id_table, and failure will be returned in the response.
  Achived by calling mmgsdi_client_id_reg().

LIMITATIONS:
  None.

RETURN VALUE:
  mmgsdi_return_enum_type:

    MMGSDI_SUCCESS:          The command structure was properly generated
                             and queued onto the MMGSDI Command Queue.
    MMGSDI_INCORRECT_PARAMS: The parameters supplied to the API are not
                             within appropriate ranges.
    MMGSDI_CMD_QUEUE_FULL:   The command was not Queued to the MMGSDI Task
                             because the max number of commands are already
                             queued.

SIDE EFFECTS:
  None
  ========================================================================== */
mmgsdi_return_enum_type mmgsdi_sap_deregister
(
  mmgsdi_client_id_type     client_id,
  mmgsdi_slot_id_enum_type  card_slot,
  mmgsdi_callback_type      response_cb_ptr,
  mmgsdi_client_data_type   client_ref
);
/*~ FUNCTION mmgsdi_sap_deregister  */
/*~ ONERROR return MMGSDI_RPC_ERROR */


/* ============================================================================
FUNCTION MMGSDI_CARD_PUP

DESCRIPTION
  This function attempts to perform a CARD power up.

PARAMS:
  client_id:       Client Id of the caller
  card_slot:       Physical slot for multiple slot targets.
  response_cb_ptr: Response callback.
  pup_option:      Pup and switch to passive or Refresh reset pup
  client_ref:      User Data returned upon completion of this cmd.

DEPENDENCIES:
  The client must have a valid client ID.  If the client ID is not part
  of the client_id_table, and failure will be returned in the response.
  Achived by calling mmgsdi_client_id_reg().

LIMITATIONS:
  None.

RETURN VALUE:
  mmgsdi_return_enum_type:

    MMGSDI_SUCCESS:          The command structure was properly generated
                             and queued onto the MMGSDI Command Queue.
    MMGSDI_INCORRECT_PARAMS: The parameters supplied to the API are not
                             within appropriate ranges.
    MMGSDI_CMD_QUEUE_FULL:   The command was not Queued to the MMGSDI Task
                             because the max number of commands are already
                             queued.

SIDE EFFECTS:
  None
  ========================================================================== */
mmgsdi_return_enum_type mmgsdi_card_pup(
    mmgsdi_client_id_type             client_id,
    mmgsdi_slot_id_enum_type          card_slot,
    mmgsdi_callback_type              response_cb_ptr,
    mmgsdi_card_pup_options_enum_type pup_option,
    mmgsdi_client_data_type           client_ref
);
/*~ FUNCTION mmgsdi_card_pup        */
/*~ ONERROR return MMGSDI_RPC_ERROR */


/* ============================================================================
FUNCTION MMGSDI_CARD_PDOWN

DESCRIPTION
  This function attempts to perform a CARD power down.

PARAMS:
  client_id:       Client Id of the caller
  card_slot:       Physical slot for multiple slot targets.
  response_cb_ptr: Response callback.
  pdown_option:    Quite Power down / Power down and notify GSDI
  client_ref:      User Data returned upon completion of this cmd.

DEPENDENCIES:
  The client must have a valid client ID.  If the client ID is not part
  of the client_id_table, and failure will be returned in the response.
  Achived by calling mmgsdi_client_id_reg().

LIMITATIONS:
  None.

RETURN VALUE:
  mmgsdi_return_enum_type:

    MMGSDI_SUCCESS:          The command structure was properly generated
                             and queued onto the MMGSDI Command Queue.
    MMGSDI_INCORRECT_PARAMS: The parameters supplied to the API are not
                             within appropriate ranges.
    MMGSDI_CMD_QUEUE_FULL:   The command was not Queued to the MMGSDI Task
                             because the max number of commands are already
                             queued.

SIDE EFFECTS:
  None
  ========================================================================== */
mmgsdi_return_enum_type mmgsdi_card_pdown(
    mmgsdi_client_id_type                  client_id,
    mmgsdi_slot_id_enum_type               card_slot,
    mmgsdi_callback_type                   response_cb_ptr,
    mmgsdi_card_pdown_options_enum_type    pdown_option,
    mmgsdi_client_data_type                client_ref
);
/*~ FUNCTION mmgsdi_card_pdown      */
/*~ ONERROR return MMGSDI_RPC_ERROR */


/* ============================================================================
FUNCTION MMGSDI_SEND_CARD_STATUS

DESCRIPTION
  GSM 11.11 functionality:
    This function returns information concerning the current directory.
    A current EF is not affected by the STATUS function. It is also used to
    give an opportunity for a pro active SIM to indicate that the SIM
    wants to issue a SIM Application Toolkit command to the ME.

    Status and return type is not required when using a GSM SIM Card.

  UMTS 31.102 Functionality:
    This function returns information concerning the current directory or current
    application.  In addition, according to the application specification,
    it may be used to indicate to the application in the UICC that its
    session activation procedure has been successfully executed or that
    its termination procedure will be executed.
    NOTE: These indications may be used to synchronize the applications in the
    terminal and in the UICC.

    A status and return type is required when using a UICC Card with a USIM
    application.

PARAMS:
  client_id:       Client Id of the caller
  card_slot:       Physical slot for multiple slot targets.
  me_app_status:   Indicates that status of the application in the handset
  ret_data_struct: Structure in which the data should be returned in
  response_cb_ptr: Response callback.
  client_ref:      User Data returned upon completion of this cmd.

DEPENDENCIES:
  The client must have a valid client ID.  If the client ID is not part
  of the client_id_table, and failure will be returned in the response.
  Achived by calling mmgsdi_client_id_reg().

LIMITATIONS:
  None.

RETURN VALUE:
  mmgsdi_return_enum_type:

    MMGSDI_SUCCESS:          The command structure was properly generated
                             and queued onto the MMGSDI Command Queue.
    MMGSDI_INCORRECT_PARAMS: The parameters supplied to the API are not
                             within appropriate ranges.
    MMGSDI_CMD_QUEUE_FULL:   The command was not Queued to the MMGSDI Task
                             because the max number of commands are already
                             queued.

SIDE EFFECTS:
  None
  ========================================================================== */
mmgsdi_return_enum_type mmgsdi_send_card_status(
    mmgsdi_client_id_type            client_id,
    mmgsdi_slot_id_enum_type         card_slot,
    mmgsdi_status_me_app_enum_type   me_app_status,
    mmgsdi_status_ret_data_enum_type ret_data_struct,
    mmgsdi_callback_type             response_cb_ptr,
    mmgsdi_client_data_type          client_ref
);
/*~ FUNCTION mmgsdi_send_card_status */
/*~ ONERROR return MMGSDI_RPC_ERROR  */


/* ============================================================================
FUNCTION MMGSDI_ACTIVATE_ONCHIP_SIM

DESCRIPTION
  This function will build a MMGSDI_ACTIVATE_ONCHIP_SIM_REQ and queue it to
  the MMGSDI Task for further processing.

PARAMS:
  client_id:         Client Id of the caller
  card_slot:         Physical slot for multiple slot targets.
  onchip_sim_config: TLV Data Len and TLV Data.
  response_cb_ptr:   Response callback.
  client_ref:        User Data returned upon completion of this cmd.

DEPENDENCIES:
  The client must have a valid client ID.  If the client ID is not part
  of the client_id_table, and failure will be returned in the response.
  Achived by calling mmgsdi_client_id_reg().

LIMITATIONS:
  None.

RETURN VALUE:
  mmgsdi_return_enum_type:

    MMGSDI_SUCCESS:          The command structure was properly generated
                             and queued onto the MMGSDI Command Queue.
    MMGSDI_INCORRECT_PARAMS: The parameters supplied to the API are not
                             within appropriate ranges.
    MMGSDI_CMD_QUEUE_FULL:   The command was not Queued to the MMGSDI Task
                             because the max number of commands are already
                             queued.

SIDE EFFECTS:
  Access to a real card will not be possible without a power cycle
  ========================================================================== */
mmgsdi_return_enum_type mmgsdi_activate_onchip_sim (
  mmgsdi_client_id_type              client_id,
  mmgsdi_slot_id_enum_type           card_slot,
  mmgsdi_onchip_sim_config_data_type onchip_sim_config,
  mmgsdi_callback_type               response_cb_ptr,
  mmgsdi_client_data_type            client_ref
); /* mmgsdi_activate_onchip_sim */
/*~ FUNCTION mmgsdi_activate_onchip_sim */
/*~ ONERROR return MMGSDI_RPC_ERROR     */


/* ============================================================================
FUNCTION MMGSDI_ENABLE_BDN

DESCRIPTION
  This function enables the BDN

PARAMS:
  client_id:       Client Id of the caller
  response_cb_ptr: Response callback.
  client_ref:      User Data returned upon completion of this cmd.

DEPENDENCIES:
  The client must have a valid client ID.  If the client ID is not part
  of the client_id_table, and failure will be returned in the response.
  Achived by calling mmgsdi_client_id_reg().
  PIN2 needed to be verified before a successful enabling or disabling of
  BDN according to specification

LIMITATIONS:
  None.

RETURN VALUE:
  mmgsdi_return_enum_type:

    MMGSDI_SUCCESS:          The command structure was properly generated
                             and queued onto the MMGSDI Command Queue.
    MMGSDI_INCORRECT_PARAMS: The parameters supplied to the API are not
                             within appropriate ranges.
    MMGSDI_CMD_QUEUE_FULL:   The command was not Queued to the MMGSDI Task
                             because the max number of commands are already
                             queued.

SIDE EFFECTS:
  Will enable the BDN if success
  ========================================================================== */
mmgsdi_return_enum_type mmgsdi_enable_bdn (
  mmgsdi_client_id_type              client_id,
  mmgsdi_slot_id_enum_type           card_slot,
  mmgsdi_callback_type               response_cb_ptr,
  mmgsdi_client_data_type            client_ref
);
/*~ FUNCTION mmgsdi_enable_bdn      */
/*~ ONERROR return MMGSDI_RPC_ERROR */


/* ============================================================================
FUNCTION MMGSDI_DISABLE_BDN

DESCRIPTION
  This function disables the BDN

PARAMS:
  client_id:       Client Id of the caller
  response_cb_ptr: Response callback.
  client_ref:      User Data returned upon completion of this cmd.

DEPENDENCIES:
  The client must have a valid client ID.  If the client ID is not part
  of the client_id_table, and failure will be returned in the response.
  Achived by calling mmgsdi_client_id_reg().
  PIN2 needed to be verified before a successful enabling or disabling of
  BDN according to specification

LIMITATIONS:
  None.

RETURN VALUE:
  mmgsdi_return_enum_type:

    MMGSDI_SUCCESS:          The command structure was properly generated
                             and queued onto the MMGSDI Command Queue.
    MMGSDI_INCORRECT_PARAMS: The parameters supplied to the API are not
                             within appropriate ranges.
    MMGSDI_CMD_QUEUE_FULL:   The command was not Queued to the MMGSDI Task
                             because the max number of commands are already
                             queued.

SIDE EFFECTS:
  Will disable the BDN if success
  ========================================================================== */
mmgsdi_return_enum_type mmgsdi_disable_bdn (
  mmgsdi_client_id_type              client_id,
  mmgsdi_slot_id_enum_type           card_slot,
  mmgsdi_callback_type               response_cb_ptr,
  mmgsdi_client_data_type            client_ref
);
/*~ FUNCTION mmgsdi_disable_bdn     */
/*~ ONERROR return MMGSDI_RPC_ERROR */


/* ============================================================================
FUNCTION MMGSDI_ENABLE_ACL

DESCRIPTION
  This function enables the ACL

PARAMS:
  client_id:       Client Id of the caller
  response_cb_ptr: Response callback.
  client_ref:      User Data returned upon completion of this cmd.

DEPENDENCIES:
  The client must have a valid client ID.  If the client ID is not part
  of the client_id_table, and failure will be returned in the response.
  Achived by calling mmgsdi_client_id_reg().
  PIN2 needed to be verified before a successful enabling or disabling of
  ACL according to specification

LIMITATIONS:
  None.

RETURN VALUE:
  mmgsdi_return_enum_type:

    MMGSDI_SUCCESS:          The command structure was properly generated
                             and queued onto the MMGSDI Command Queue.
    MMGSDI_INCORRECT_PARAMS: The parameters supplied to the API are not
                             within appropriate ranges.
    MMGSDI_CMD_QUEUE_FULL:   The command was not Queued to the MMGSDI Task
                             because the max number of commands are already
                             queued.

SIDE EFFECTS:
    None
  ========================================================================== */
mmgsdi_return_enum_type mmgsdi_enable_acl (
  mmgsdi_client_id_type              client_id,
  mmgsdi_slot_id_enum_type           card_slot,
  mmgsdi_callback_type               response_cb_ptr,
  mmgsdi_client_data_type            client_ref
);
/*~ FUNCTION mmgsdi_enable_acl      */
/*~ ONERROR return MMGSDI_RPC_ERROR */


/* ============================================================================
FUNCTION MMGSDI_DISABLE_ACL

DESCRIPTION
  This function disables the ACL

PARAMS:
  client_id:       Client Id of the caller
  response_cb_ptr: Response callback.
  client_ref:      User Data returned upon completion of this cmd.

DEPENDENCIES:
  The client must have a valid client ID.  If the client ID is not part
  of the client_id_table, and failure will be returned in the response.
  Achived by calling mmgsdi_client_id_reg().
  PIN2 needed to be verified before a successful enabling or disabling of
  ACL according to specification

LIMITATIONS:
  None.

RETURN VALUE:
  mmgsdi_return_enum_type:

    MMGSDI_SUCCESS:          The command structure was properly generated
                             and queued onto the MMGSDI Command Queue.
    MMGSDI_INCORRECT_PARAMS: The parameters supplied to the API are not
                             within appropriate ranges.
    MMGSDI_CMD_QUEUE_FULL:   The command was not Queued to the MMGSDI Task
                             because the max number of commands are already
                             queued.

SIDE EFFECTS:
    None
  ========================================================================== */
mmgsdi_return_enum_type mmgsdi_disable_acl (
  mmgsdi_client_id_type              client_id,
  mmgsdi_slot_id_enum_type           card_slot,
  mmgsdi_callback_type               response_cb_ptr,
  mmgsdi_client_data_type            client_ref
);
/*~ FUNCTION mmgsdi_disable_acl     */
/*~ ONERROR return MMGSDI_RPC_ERROR */


/*===========================================================================
FUNCTION MMGSDI_DISABLE_PIN_EXT

DESCRIPTION
  This function will build a request to the MMGSDI Task to disable the PIN
  provided and replace it with the PIN ID given in new_pin_id.
  The PIN provided will have to be applicable to the Client
  ID Provided.

  A failure to disable could result in a blocked PIN ID if the maximum
  number of attempts to retry are exhausted.

PARAMS:
  client_id:         Client Id of the caller
  card_slot:         Physical slot for multiple slot targets.
  pin_id:            The PIN Identification to be disabled.
  replace_pin:       The replacement option
  pin_data:          Will contain the len and code of the PIN.
  response_cb_ptr:   Response callback.
  client_ref:        User Data returned upon completion of this cmd.

RETURN VALUE:
  mmgsdi_return_enum_type:

    MMGSDI_SUCCESS:          The command structure was properly generated
                             and queued onto the MMGSDI Command Queue.
    MMGSDI_INCORRECT_PARAMS: The parameters supplied to the API are not
                             within appropriate ranges.
    MMGSDI_CMD_QUEUE_FULL:   The command was not Queued to the MMGSDI Task
                             because the max number of commands are already
                             queued.

SIDE EFFECTS:
    If PIN1 is replaced by UNIV PIN, then all read /write operations
    will check Univ PIN Status instead of PIN1.
===========================================================================*/
mmgsdi_return_enum_type mmgsdi_disable_pin_ext (
  mmgsdi_client_id_type              client_id,
  mmgsdi_slot_id_enum_type           card_slot,
  mmgsdi_pin_enum_type               pin_id,
  mmgsdi_pin_replace_enum_type       replace_pin,
  mmgsdi_data_type                   pin_data,
  mmgsdi_callback_type               response_cb_ptr,
  mmgsdi_client_data_type            client_ref
); /* mmgsdi_disable_pin_ext */
/*~ FUNCTION mmgsdi_disable_pin_ext */
/*~ ONERROR return MMGSDI_RPC_ERROR */


/*===========================================================================
FUNCTION MMGSDI_IS_SERVICE_AVAILABLE

DESCRIPTION
  This function finds out if the service is available or not given the
  service_type

PARAMS:
  client_id:       Client Id of the caller
  card_slot:       Card Slot
  srvc_type:       Service that the client is interested in
  response_cb_ptr: Response callback.
  client_ref:      User Data returned upon completion of this cmd.

DEPENDENCIES:
  The client must have a valid client ID.  If the client ID is not part
  of the client_id_table, and failure will be returned in the response.
  Achived by calling mmgsdi_client_id_reg().

LIMITATIONS:
  None.

RETURN VALUE:
  mmgsdi_return_enum_type:

    MMGSDI_SUCCESS:          The command structure was properly generated
                             and queued onto the MMGSDI Command Queue.
    MMGSDI_INCORRECT_PARAMS: The parameters supplied to the API are not
                             within appropriate ranges.
    MMGSDI_CMD_QUEUE_FULL:   The command was not Queued to the MMGSDI Task
                             because the max number of commands are already
                             queued.

SIDE EFFECTS:
    None
===========================================================================*/
mmgsdi_return_enum_type  mmgsdi_is_service_available (
  mmgsdi_client_id_type               client_id,
  mmgsdi_slot_id_enum_type            card_slot,
  mmgsdi_service_enum_type            srvc_type,
  mmgsdi_callback_type                response_cb_ptr,
  mmgsdi_client_data_type             client_ref
);
/*~ FUNCTION mmgsdi_is_service_available */
/*~ ONERROR return MMGSDI_RPC_ERROR      */


/*===========================================================================
FUNCTION:      MMGSDI_USIM_AUTHENTICATE

DESCRIPTION:
  This function is used to perform a "Network" Initiated Authentication with
  the USIM Application.  This is only valid and applicable when an USIM
  Application is available . This function will perform a check for the USIM
  Application before sending the command.

  Description of the parameters:
  mmgsdi_client_id_type    client_id:      Input.  Client Id of the caller
  mmgsdi_slot_id_enum_type card_slot:      Input.  Physical slot for multiple
                                                   slot targets.
  mmgsdi_data_type         auth_req:       Input.  Length and Data of auth
                                                   challenge.
  mmgsdi_client_data_type  client_ref:     Input.  User Data returned upon
                                                   completion of this cmd.

DEPENDENCIES:
  The client must have a valid client ID.  If the client ID is not part
  of the client_id_table, and failure will be returned in the response.
  Achived by calling mmgsdi_client_id_reg().

  The client must also have a valid session open if the USIM Application
  is not the default application
  Achived by calling mmgsdi_session_management_open.

LIMITATIONS:
  None.

RETURN VALUE:
  mmgsdi_return_enum_type:

    MMGSDI_SUCCESS:          The command structure was properly generated
                             and queued onto the MMGSDI Command Queue.
    MMGSDI_INCORRECT_PARAMS: The parameters supplied to the API are not
                             within appropriate ranges.
    MMGSDI_CMD_QUEUE_FULL:   The command was not Queued to the MMGSDI Task
                             because the max number of commands are already
                             queued.

SIDE EFFECTS:
  None
===========================================================================*/
mmgsdi_return_enum_type  mmgsdi_usim_authenticate (
  mmgsdi_client_id_type               client_id,
  mmgsdi_slot_id_enum_type            card_slot,
  mmgsdi_autn_context_enum_type       auth_context,
  mmgsdi_data_type                    auth_req,
  mmgsdi_callback_type                response_cb_ptr,
  mmgsdi_client_data_type             client_ref
);
/*~ FUNCTION mmgsdi_usim_authenticate */
/*~ ONERROR return MMGSDI_RPC_ERROR   */


/*===========================================================================
FUNCTION MMGSDI_JCDMA_GET_CARD_INFO

DESCRIPTION
  This function will buid a request to the MMGSDI Task to request the jcdma
  card information.

PARAMS:
  client_id:         Client Id of the caller
  card_slot:         Physical slot for multiple slot targets.
  response_cb_ptr:   Response callback.
  client_ref:        User Data returned upon completion of this cmd.

DEPENDENCIES:
  PIN1 verification is done before calling this function.

LIMITATIONS:

RETURN VALUE:
   mmgsdi_return_enum_type
SIDE EFFECTS:
===========================================================================*/
mmgsdi_return_enum_type mmgsdi_jcdma_get_card_info (
  mmgsdi_client_id_type              client_id,
  mmgsdi_slot_id_enum_type           card_slot,
  mmgsdi_callback_type               response_cb_ptr,
  mmgsdi_client_data_type            client_ref
);
/*~ FUNCTION mmgsdi_jcdma_get_card_info */
/*~ ONERROR return MMGSDI_RPC_ERROR     */


/*===========================================================================
FUNCTION:      MMGSDI_JCDMA_CARD_GET_INFO

DESCRIPTION:
  This function returns card info.

DEPENDENCIES:

LIMITATIONS:

RETURN VALUE:
   mmgsdi_jcdma_card_info_enum_type
SIDE EFFECTS:
===========================================================================*/
mmgsdi_jcdma_card_info_enum_type mmgsdi_jcdma_card_get_info(void);
/*~ FUNCTION mmgsdi_jcdma_card_get_info   */


/*===========================================================================
FUNCTION MMGSDI_GET_ESN_ME_CHANGE_FLAG

DESCRIPTION
  This function will return the esn ME change flag obtained during post pin1
  init

PARAMS:
  none

DEPENDENCIES:

LIMITATIONS:

RETURN VALUE:
   mmgsdi_esn_me_change_enum_type:
     MMGSDI_ESN_ME_NO_CHANGE
     MMGSDI_ESN_ME_CHANGE
     MMGSDI_ESN_ME_CHANGE_NOT_SUPPORTED

SIDE EFFECTS:
===========================================================================*/
mmgsdi_esn_me_change_enum_type mmgsdi_get_esn_me_change_flag (
  mmgsdi_slot_id_enum_type slot
);
/*~ FUNCTION mmgsdi_get_esn_me_change_flag */


/*===========================================================================
FUNCTION MMGSDI_COMPUTE_IP_AUTH

DESCRIPTION
  This function will build a request to the MMGSDI Task to perform a compute
  IP authentication command for 3GPD SIMPLE IP CHAP, Mobile IP, or HRPD
  CHAP Authentication.

PARAMS:
  client_id:         Client Id of the caller
  card_slot:         Physical slot for multiple slot targets.
  compute_ip_data:   DATA Required for Compuite IP Command
  response_cb_ptr:   Response callback.
  client_ref:        User Data returned upon completion of this cmd.

RETURN VALUE:
  mmgsdi_return_enum_type:

    MMGSDI_SUCCESS:          The command structure was properly generated
                             and queued onto the MMGSDI Command Queue.
    MMGSDI_INCORRECT_PARAMS: The parameters supplied to the API are not
                             within appropriate ranges.
    MMGSDI_CMD_QUEUE_FULL:   The command was not Queued to the MMGSDI Task
                             because the max number of commands are already
                             queued.

SIDE EFFECTS:

===========================================================================*/
mmgsdi_return_enum_type mmgsdi_compute_ip_auth (
  mmgsdi_client_id_type              client_id,
  mmgsdi_slot_id_enum_type           card_slot,
  mmgsdi_compute_ip_data_type        compute_ip_data,
  mmgsdi_callback_type               response_cb_ptr,
  mmgsdi_client_data_type            client_ref
); /* mmgsdi_compute_ip_auth */
/*~ FUNCTION mmgsdi_compute_ip_auth */
/*~ ONERROR return MMGSDI_RPC_ERROR */


/*===========================================================================
FUNCTION MMGSDI_RUN_CAVE_ALGO

DESCRIPTION
  This function will build a request to the MMGSDI Task to perform a run cave
  operation in the UIM

PARAMS:
  client_id:         Client Id of the caller
  card_slot:         Physical slot for multiple slot targets.
  rand_type:         Type of Random Challenge
  rand_chal:         Rand Chal for CAVE Algo
  dig_len:           Length of digits provided
  digits:            Digits for CAVE calculation
  process_control:   Process Control for CAVE
  esn:               ESN for CAVE Calculation
  response_cb_ptr:   Response callback.
  client_ref:        User Data returned upon completion of this cmd.

RETURN VALUE:
  mmgsdi_return_enum_type:

    MMGSDI_SUCCESS:          The command structure was properly generated
                             and queued onto the MMGSDI Command Queue.
    MMGSDI_INCORRECT_PARAMS: The parameters supplied to the API are not
                             within appropriate ranges.
    MMGSDI_CMD_QUEUE_FULL:   The command was not Queued to the MMGSDI Task
                             because the max number of commands are already
                             queued.

SIDE EFFECTS:

===========================================================================*/
mmgsdi_return_enum_type mmgsdi_run_cave_algo (
  mmgsdi_client_id_type              client_id,
  mmgsdi_slot_id_enum_type           card_slot,
  mmgsdi_cave_rand_type_type         rand_type,
  mmgsdi_cave_rand_chal_type         rand_chal,
  mmgsdi_cave_dig_len_type           dig_len,
  mmgsdi_cave_digits_type            digits,
  mmgsdi_cave_process_control_type   process_control,
  mmgsdi_cave_esn_type               esn,
  mmgsdi_callback_type               response_cb_ptr,
  mmgsdi_client_data_type            client_ref
); /* mmgsdi_run_cave_algo */
/*~ FUNCTION mmgsdi_run_cave_algo   */
/*~ ONERROR return MMGSDI_RPC_ERROR */


/*===========================================================================
FUNCTION MMGSDI_GENERATE_KEY_VPM

DESCRIPTION
  This function will build a request to the MMGSDI Task to perform a generate
  key command in the UIM

PARAMS:
  client_id:         Client Id of the caller
  card_slot:         Physical slot for multiple slot targets.
  first_octet:       First Octet of VPM to be output
  last_octet:        Last Octet of VPM to be output
  response_cb_ptr:   Response callback.
  client_ref:        User Data returned upon completion of this cmd.

RETURN VALUE:
  mmgsdi_return_enum_type:

    MMGSDI_SUCCESS:          The command structure was properly generated
                             and queued onto the MMGSDI Command Queue.
    MMGSDI_INCORRECT_PARAMS: The parameters supplied to the API are not
                             within appropriate ranges.
    MMGSDI_CMD_QUEUE_FULL:   The command was not Queued to the MMGSDI Task
                             because the max number of commands are already
                             queued.
    MMGSDI_AUTH_ERROR_CAVE_NOT_RUN:  The command cannot be called unless
                             the RUN CAVE command has already been run with
                             SAVE Registers turned on

SIDE EFFECTS:

===========================================================================*/
mmgsdi_return_enum_type mmgsdi_generate_key_vpm (
  mmgsdi_client_id_type              client_id,
  mmgsdi_slot_id_enum_type           card_slot,
  mmgsdi_single_vpm_octet_type       first_octet,
  mmgsdi_single_vpm_octet_type       last_octet,
  mmgsdi_callback_type               response_cb_ptr,
  mmgsdi_client_data_type            client_ref
); /* mmgsdi_generate_key_vpm */
/*~ FUNCTION mmgsdi_generate_key_vpm */
/*~ ONERROR return MMGSDI_RPC_ERROR */


/* ============================================================================
FUNCTION MMGSDI_SEND_APDU

DESCRIPTION:
  This is the API used to send an APDU command to the Card inserted in the
  slot indicated.

PARAMS:
  client_id:       Client Id of the caller
  card_slot:       Physical slot for multiple slot targets.
  apdu_data:       The APDU to send to the card.
  option:          Additional optional parameters for this API. Currently none
  response_cb_ptr: Response callback.
  client_ref:      User Data returned upon completion of this cmd.

DEPENDENCIES:
  The client must have a valid client ID.  If the client ID is not part
  of the client_id_table, and failure will be returned in the response.
  Achieved by calling mmgsdi_client_id_reg().

LIMITATIONS:
  None.

RETURN VALUE:
  mmgsdi_return_enum_type:

    MMGSDI_SUCCESS:          The command structure was properly generated
                             and queued onto the MMGSDI Command Queue.
    MMGSDI_INCORRECT_PARAMS: The parameters supplied to the API are not
                             within appropriate ranges.
    MMGSDI_CMD_QUEUE_FULL:   The command was not Queued to the MMGSDI Task
                             because the max number of commands are already
                             queued.

SIDE EFFECTS:
  None
  ========================================================================== */
mmgsdi_return_enum_type mmgsdi_send_apdu(
  mmgsdi_client_id_type          client_id,
  mmgsdi_slot_id_enum_type       card_slot,
  mmgsdi_send_apdu_data_type     apdu_data,
  mmgsdi_option_type             option,
  mmgsdi_callback_type           response_cb_ptr,
  mmgsdi_client_data_type        client_ref
);
/*~ FUNCTION mmgsdi_send_apdu       */
/*~ ONERROR return MMGSDI_RPC_ERROR */


/* ============================================================================
FUNCTION MMGSDI_GET_ATR

DESCRIPTION:
  This is the API used to retrieve the ATR Information which pertains to the
  card(s) currently inserted in the ME.

PARAMS:
  client_id:       Client Id of the caller
  card_slot:       Physical slot for multiple slot targets.
  option:          Additional optional parameters for this API. Currently none
  response_cb_ptr: Response callback.
  client_ref:      User Data returned upon completion of this cmd.

DEPENDENCIES:
  The client must have a valid client ID.  If the client ID is not part
  of the client_id_table, and failure will be returned in the response.
  Achieved by calling mmgsdi_client_id_reg().

LIMITATIONS:
  None.

RETURN VALUE:
  mmgsdi_return_enum_type:

    MMGSDI_SUCCESS:          The command structure was properly generated
                             and queued onto the MMGSDI Command Queue.
    MMGSDI_INCORRECT_PARAMS: The parameters supplied to the API are not
                             within appropriate ranges.
    MMGSDI_CMD_QUEUE_FULL:   The command was not Queued to the MMGSDI Task
                             because the max number of commands are already
                             queued.

SIDE EFFECTS:
  None
  ========================================================================== */
mmgsdi_return_enum_type mmgsdi_get_atr(
  mmgsdi_client_id_type    client_id,
  mmgsdi_slot_id_enum_type card_slot,
  mmgsdi_option_type       option,
  mmgsdi_callback_type     response_cb_ptr,
  mmgsdi_client_data_type  client_ref
);
/*~ FUNCTION mmgsdi_get_atr         */
/*~ ONERROR return MMGSDI_RPC_ERROR */


/* ============================================================================
FUNCTION MMGSDI_GET_CARD_READER_STATUS

DESCRIPTION
  This Function is used to retrieve the CARD Reader Status.  This interferace
  is occassionally referred to as a card reader.

PARAMS:
  client_id:       Client Id of the caller
  card_slot:       Physical slot for multiple slot targets.
  option:          Additional optional parameters for this API. Currently none
  response_cb_ptr: Response callback.
  client_ref:      User Data returned upon completion of this cmd.

DEPENDENCIES:
  The client must have a valid client ID.  If the client ID is not part
  of the client_id_table, and failure will be returned in the response.
  Achieved by calling mmgsdi_client_id_reg().

LIMITATIONS:
  None.

RETURN VALUE:
  mmgsdi_return_enum_type:

    MMGSDI_SUCCESS:          The command structure was properly generated
                             and queued onto the MMGSDI Command Queue.
    MMGSDI_INCORRECT_PARAMS: The parameters supplied to the API are not
                             within appropriate ranges.
    MMGSDI_CMD_QUEUE_FULL:   The command was not Queued to the MMGSDI Task
                             because the max number of commands are already
                             queued.

SIDE EFFECTS:
  None
  ========================================================================== */
mmgsdi_return_enum_type mmgsdi_get_card_reader_status(
  mmgsdi_client_id_type     client_id,
  mmgsdi_slot_id_enum_type  card_slot,
  mmgsdi_option_type        option,
  mmgsdi_callback_type      response_cb_ptr,
  mmgsdi_client_data_type   client_ref
);
/*~ FUNCTION mmgsdi_get_card_reader_status */
/*~ ONERROR return MMGSDI_RPC_ERROR        */


/*==================================================================
FUNCTION MMGSDI_MFLO_INIT_ACT_PARAMS

DESCRIPTION
  This function will build a MMGSDI_MFLO_INIT_ACT_PARAMS request
  and queue it to the MMGSDI Task for further processing.

PARAMS:
  client_id:         Client Id of the caller
  card_slot:         Physical slot for multiple slot targets.
  act_params:        Activation parameters, should be encrypted.
  option:            Additional optional parameters for this API.
                     Currently none
  response_cb_ptr:   Response callback.
  client_ref:        User Data returned upon completion of this cmd.

DEPENDENCIES:
  The client must have a valid client ID.  If the client ID is not part
  of the client_id_table, and failure will be returned in the response.
  Achived by calling mmgsdi_client_id_reg().

LIMITATIONS:
  None.

RETURN VALUE:
  mmgsdi_return_enum_type:

    MMGSDI_SUCCESS:          The command structure was properly generated
                             and queued onto the MMGSDI Command Queue.
    MMGSDI_INCORRECT_PARAMS: The parameters supplied to the API are not
                             within appropriate ranges.
    MMGSDI_CMD_QUEUE_FULL:   The command was not Queued to the MMGSDI Task
                             because the max number of commands are already
                             queued.

SIDE EFFECTS:
  None
===================================================================*/
mmgsdi_return_enum_type mmgsdi_mflo_init_act_params (
  mmgsdi_client_id_type              client_id,
  mmgsdi_slot_id_enum_type           card_slot,
  mmgsdi_data_type                   act_params,
  mmgsdi_option_type                 option,
  mmgsdi_callback_type               response_cb_ptr,
  mmgsdi_client_data_type            client_ref
);
/*~ FUNCTION mmgsdi_mflo_init_act_params */
/*~ ONERROR return MMGSDI_RPC_ERROR      */


/*==================================================================
FUNCTION MMGSDI_MFLO_GET_SUBSCRIBER_ID

DESCRIPTION
  This function will build a MMGSDI_MFLO_GET_SUBSCRIBER_ID request
  and queue it to the MMGSDI Task for further processing.

PARAMS:
  client_id:         Client Id of the caller
  card_slot:         Physical slot for multiple slot targets.
  option:            Additional optional parameters for this API.
                     Currently none
  response_cb_ptr:   Response callback.
  client_ref:        User Data returned upon completion of this cmd.

DEPENDENCIES:
  The client must have a valid client ID.  If the client ID is not part
  of the client_id_table, and failure will be returned in the response.
  Achived by calling mmgsdi_client_id_reg().

LIMITATIONS:
  None.

RETURN VALUE:
  mmgsdi_return_enum_type:

    MMGSDI_SUCCESS:          The command structure was properly generated
                             and queued onto the MMGSDI Command Queue.
    MMGSDI_INCORRECT_PARAMS: The parameters supplied to the API are not
                             within appropriate ranges.
    MMGSDI_CMD_QUEUE_FULL:   The command was not Queued to the MMGSDI Task
                             because the max number of commands are already
                             queued.

SIDE EFFECTS:
  None
===================================================================*/
mmgsdi_return_enum_type mmgsdi_mflo_get_subscriber_id (
  mmgsdi_client_id_type              client_id,
  mmgsdi_slot_id_enum_type           card_slot,
  mmgsdi_option_type                 option,
  mmgsdi_callback_type               response_cb_ptr,
  mmgsdi_client_data_type            client_ref
);
/*~ FUNCTION mmgsdi_mflo_get_subscriber_id */
/*~ ONERROR return MMGSDI_RPC_ERROR        */


/*==================================================================
FUNCTION MMGSDI_MFLO_GET_PUBLIC_KEY

DESCRIPTION
  This function will build a MMGSDI_MFLO_GET_PUBLIC_KEY request
  and queue it to the MMGSDI Task for further processing.

PARAMS:
  client_id:         Client Id of the caller
  card_slot:         Physical slot for multiple slot targets.
  option:            Additional optional parameters for this API.
                     Currently none
  response_cb_ptr:   Response callback.
  client_ref:        User Data returned upon completion of this cmd.

DEPENDENCIES:
  The client must have a valid client ID.  If the client ID is not part
  of the client_id_table, and failure will be returned in the response.
  Achived by calling mmgsdi_client_id_reg().

LIMITATIONS:
  None.

RETURN VALUE:
  mmgsdi_return_enum_type:

    MMGSDI_SUCCESS:          The command structure was properly generated
                             and queued onto the MMGSDI Command Queue.
    MMGSDI_INCORRECT_PARAMS: The parameters supplied to the API are not
                             within appropriate ranges.
    MMGSDI_CMD_QUEUE_FULL:   The command was not Queued to the MMGSDI Task
                             because the max number of commands are already
                             queued.

SIDE EFFECTS:
  None
===================================================================*/
mmgsdi_return_enum_type mmgsdi_mflo_get_public_key (
  mmgsdi_client_id_type              client_id,
  mmgsdi_slot_id_enum_type           card_slot,
  mmgsdi_option_type                 option,
  mmgsdi_callback_type               response_cb_ptr,
  mmgsdi_client_data_type            client_ref
);
/*~ FUNCTION mmgsdi_mflo_get_public_key */
/*~ ONERROR return MMGSDI_RPC_ERROR     */


/*==================================================================
FUNCTION MMGSDI_MFLO_SIGN_UCAST

DESCRIPTION
  This function will build a MMGSDI_MFLO_SIGN_UCAST request
  and queue it to the MMGSDI Task for further processing.

PARAMS:
  client_id:         Client Id of the caller
  card_slot:         Physical slot for multiple slot targets.
  msg_type:          Type of unicast message block.
  msg:               Message to be signed
  option:            Additional optional parameters for this API.
                     Currently none
  response_cb_ptr:   Response callback.
  client_ref:        User Data returned upon completion of this cmd.

DEPENDENCIES:
  The client must have a valid client ID.  If the client ID is not part
  of the client_id_table, and failure will be returned in the response.
  Achived by calling mmgsdi_client_id_reg().

LIMITATIONS:
  None.

RETURN VALUE:
  mmgsdi_return_enum_type:

    MMGSDI_SUCCESS:          The command structure was properly generated
                             and queued onto the MMGSDI Command Queue.
    MMGSDI_INCORRECT_PARAMS: The parameters supplied to the API are not
                             within appropriate ranges.
    MMGSDI_CMD_QUEUE_FULL:   The command was not Queued to the MMGSDI Task
                             because the max number of commands are already
                             queued.

SIDE EFFECTS:
  None
===================================================================*/
mmgsdi_return_enum_type mmgsdi_mflo_sign_ucast (
  mmgsdi_client_id_type                     client_id,
  mmgsdi_slot_id_enum_type                  card_slot,
  mmgsdi_mflo_ucast_msg_enum_type           msg_type,
  mmgsdi_data_type                          msg,
  mmgsdi_option_type                        option,
  mmgsdi_callback_type                      response_cb_ptr,
  mmgsdi_client_data_type                   client_ref
);
/*~ FUNCTION mmgsdi_mflo_sign_ucast */
/*~ ONERROR return MMGSDI_RPC_ERROR */


/*==================================================================
FUNCTION MMGSDI_MFLO_VERIFY_UCAST

DESCRIPTION
  This function will build a MMGSDI_MFLO_VERIFY_UCAST request
  and queue it to the MMGSDI Task for further processing.

PARAMS:
  client_id:         Client Id of the caller
  card_slot:         Physical slot for multiple slot targets.
  msg_type:          Type of unicast message block.
  msg:               Message to be verified.
  option:            Additional optional parameters for this API.
                     Currently none
  response_cb_ptr:   Response callback.
  client_ref:        User Data returned upon completion of this cmd.

DEPENDENCIES:
  The client must have a valid client ID.  If the client ID is not part
  of the client_id_table, and failure will be returned in the response.
  Achived by calling mmgsdi_client_id_reg().

LIMITATIONS:
  None.

RETURN VALUE:
  mmgsdi_return_enum_type:

    MMGSDI_SUCCESS:          The command structure was properly generated
                             and queued onto the MMGSDI Command Queue.
    MMGSDI_INCORRECT_PARAMS: The parameters supplied to the API are not
                             within appropriate ranges.
    MMGSDI_CMD_QUEUE_FULL:   The command was not Queued to the MMGSDI Task
                             because the max number of commands are already
                             queued.

SIDE EFFECTS:
  None
===================================================================*/
mmgsdi_return_enum_type mmgsdi_mflo_verify_ucast (
  mmgsdi_client_id_type                     client_id,
  mmgsdi_slot_id_enum_type                  card_slot,
  mmgsdi_mflo_ucast_msg_enum_type           msg_type,
  mmgsdi_data_type                          msg,
  mmgsdi_option_type                        option,
  mmgsdi_callback_type                      response_cb_ptr,
  mmgsdi_client_data_type                   client_ref
);
/*~ FUNCTION mmgsdi_mflo_verify_ucast */
/*~ ONERROR return MMGSDI_RPC_ERROR   */


/*=============================================================================
    FUNCTION MMGSDI_REGISTER_FOR_REFRESH

    Description
      This function allows the client register for File change notifications
      for specific files  and/or vote for refresh ( i.e. vote for either taking
      the call stack down or terminating application that the client is
      interested in). When the FCN request is received, then MMGSDI will send
      an event MMGSDI_REFRESH_EVT with mode = MMGSDI_REFRESH_FCN with the file
      list that has changed. MMGSDI will filter out the file list based on what
      the client registered for.
      Some FCNs on default could potentially result in bringing the protocol
      stack down - if the client is interested in these files, then they should
      also register for voting explicitly on default app.

    DEPENDENCIES:
      The client must have a valid client ID.  If the client ID is not part
      of the client_id_table, and failure will be returned in the response.
      Achived by calling mmgsdi_client_id_reg().

      To receive events from MMGSDI, client should register for events also with
      MMGSDI by calling mmgsdi_client_evt_reg.


    PARAMS:
      client_id:       Client Id of the caller
      card_slot:       Physical slot for multiple slot targets.
      refresh_files:   Files that the Client is interested in getting
                       notification for
      vote_for_init:   Vote to ok bringing the call stack down/terminating
                       the application
      option:          optional parameter, not used right now
      response_cb_ptr: Callback to this command
      client_ref:      User Data returned upon completion of this cmd.

    LIMITATIONS:
      This API will be limited to the session on which the client is.
      If the client is interested in default app, then the appropriate client
      id should be used.

    RETURN VALUE:
      mmgsdi_return_enum_type:

        MMGSDI_SUCCESS:          The command structure was properly generated
                                 and queued onto the MMGSDI Command Queue.
        MMGSDI_INCORRECT_PARAMS: The parameters supplied to the API are not
                                 within appropriate ranges.
        MMGSDI_CMD_QUEUE_FULL:   The command was not Queued to the MMGSDI Task
                                 because the max number of commands are already
                                 queued.

    SIDE EFFECTS:
      None
    SIDE EFFECTS:
      Will result in your callback getting called everytime MMGSDI Needs to
      notify any client of any events.
=============================================================================*/
mmgsdi_return_enum_type mmgsdi_register_for_refresh (
   mmgsdi_client_id_type              client_id,
   mmgsdi_slot_id_enum_type           card_slot,
   mmgsdi_refresh_file_list_type      refresh_files,
   boolean                            vote_for_init,
   mmgsdi_option_type                 option,
   mmgsdi_callback_type               response_cb_ptr,
   mmgsdi_client_data_type            client_ref);
/*~ FUNCTION mmgsdi_register_for_refresh */
/*~ ONERROR return MMGSDI_RPC_ERROR      */


/*=============================================================================
    FUNCTION MMGSDI_DEREGISTER_FOR_REFRESH

    Description
      This function allows the client deregister for File change notifications
      for specific files or voting or both

    DEPENDENCIES:
      The client must have a valid client ID.  If the client ID is not part
      of the client_id_table, and failure will be returned in the response.
      Achived by calling mmgsdi_client_id_reg().

    PARAMS:
      client_id:        Client Id of the caller
      card_slot:        Physical slot for multiple slot targets.
      Refresh_files:    Files that the Client is no longer interested in
                        getting notification for
      vote_for_init:    Deregister from Voting,
                        FALSE ? NO changes to Current Registration,
                        TRUE ? DeRegister from Voting
      option       :   optional parameter, not used right now
      Response_cb_ptr:  Callback to this command
      client_ref:       User Data returned upon completion of this cmd.

    LIMITATIONS:
      None.

    RETURN VALUE:
      mmgsdi_return_enum_type:

        MMGSDI_SUCCESS:          The command structure was properly generated
                                 and queued onto the MMGSDI Command Queue.
        MMGSDI_INCORRECT_PARAMS: The parameters supplied to the API are not
                                 within appropriate ranges.
        MMGSDI_CMD_QUEUE_FULL:   The command was not Queued to the MMGSDI Task
                                 because the max number of commands are already
                                 queued.

    SIDE EFFECTS:
         Will result in your callback getting called everytime MMGSDI Needs to
         notify any client of any events.
=============================================================================*/
mmgsdi_return_enum_type mmgsdi_deregister_for_refresh (
   mmgsdi_client_id_type              client_id,
   mmgsdi_slot_id_enum_type           card_slot,
   mmgsdi_refresh_file_list_type      refresh_files,
   boolean                            vote_for_init,
   mmgsdi_option_type                 option,
   mmgsdi_callback_type               response_cb_ptr,
   mmgsdi_client_data_type            client_ref);
/*~ FUNCTION mmgsdi_deregister_for_refresh */
/*~ ONERROR return MMGSDI_RPC_ERROR        */


/*=============================================================================
    FUNCTION MMGSDI_REFRESH_COMPLETE

    DESCRIPTION
      This function allows the client to inform MMGSDI when it has finished all
      the files that were refreshed.

    DEPENDENCIES:
      The client must have a valid client ID.  If the client ID is not part of
      the client_id_table, and failure will be returned in the response.
      Achieved by calling mmgsdi_client_id_reg(). The client should have also
      registered for FCNs and should have also received a notification for FCN
      before they call this function.

    PARAMS:
      client_id:      Client Id of the caller
      card_slot:      Physical slot for multiple slot targets.
      pass_fail:      Boolean indicating whether client was able to
                      successfully read all the files that were refreshed
                      This could also be applicable to init cases, where the
                      client would inform MMGSDI that it was able to perform
                      its init properly
      option:         optional parameter, not used right now
      response_cb_ptr:Callback to this command
      client_ref:     User Data returned upon completion of this cmd.

    LIMITATIONS:
      None.

    RETURN VALUE:
      mmgsdi_return_enum_type:

        MMGSDI_SUCCESS:          The command structure was properly generated
                                 and queued onto the MMGSDI Command Queue.
        MMGSDI_INCORRECT_PARAMS: The parameters supplied to the API are not
                                 within appropriate ranges.
        MMGSDI_CMD_QUEUE_FULL:   The command was not Queued to the MMGSDI Task
                                 because the max number of commands are already
                                 queued.

    SIDE EFFECTS:
         Will result in your callback getting called everytime MMGSDI Needs to
         notify any client of any events.
=============================================================================*/
mmgsdi_return_enum_type mmgsdi_refresh_complete (
   mmgsdi_client_id_type              client_id,
   mmgsdi_slot_id_enum_type           card_slot,
   boolean                            pass_fail,
   mmgsdi_option_type                 option,
   mmgsdi_callback_type               response_cb_ptr,
   mmgsdi_client_data_type            client_ref);
/*~ FUNCTION mmgsdi_refresh_complete */
/*~ ONERROR return MMGSDI_RPC_ERROR  */


/*=============================================================================
    FUNCTION MMGSDI_OK_TO_REFRESH

    DESCRIPTION
      This function will have to be called by clients if they get a Refresh
      event (REFRESH_STAGE_WAIT_FOR_OK_TO_INIT,
      REFRESH_STAGE_WAIT_FOR_OK_TO_FCN). Unless this API is called, the refresh
      procedures will not start. With this API call, the client will be able to
      inform MMGSDI if it is ok to terminate the application that they are
      communicating with OR it is ok to continue with File Change.

    DEPENDENCIES:
      The client must have a valid client ID.  If the client ID is not part of
      the client_id_table, and failure will be returned in the response.
      Achieved by calling mmgsdi_client_id_reg(). The client should have also
      registered for FCNs and should have also received a notification for FCN
      before they call this function.
      The client must register with mmgsdi to receive events.

    PARAMS:
      client_id:       Client Id of the caller
      card_slot:       Physical slot for multiple slot targets.
      Ok_to_refresh:   Client informs MMGSDI if it is ok to continue with
                       refresh or not. For clients on default application this
                       would imply bringing the stack down and for others it
                       would apply to app termination
      option:          optional parameter, not used right now
      Response_cb_ptr: Callback to this command
      client_ref:      User Data returned upon completion of this cmd.

    LIMITATIONS:
      None.

    RETURN VALUE:
      mmgsdi_return_enum_type:

    MMGSDI_SUCCESS:          The command structure was properly generated
                             and queued onto the MMGSDI Command Queue.
    MMGSDI_INCORRECT_PARAMS: The parameters supplied to the API are not
                             within appropriate ranges.
    MMGSDI_CMD_QUEUE_FULL:   The command was not Queued to the MMGSDI Task
                             because the max number of commands are already
                             queued.

    SIDE EFFECTS:
      Will result in your callback getting called everytime MMGSDI Needs to
      notify any client of any events.
=============================================================================*/
mmgsdi_return_enum_type mmgsdi_ok_to_refresh (
   mmgsdi_client_id_type          client_id,
   mmgsdi_slot_id_enum_type       card_slot,
   boolean                        ok_to_refresh,
   mmgsdi_option_type             option,
   mmgsdi_callback_type           response_cb_ptr,
   mmgsdi_client_data_type        client_ref);
/*~ FUNCTION mmgsdi_ok_to_refresh   */
/*~ ONERROR return MMGSDI_RPC_ERROR */


/* =============================================================================
   FUNCTION:      MMGSDI_CLIENT_ID_REG_SET_NOTIFY

   DESCRIPTION:
     This function allows client registration for the use of MMGSDI API.
     In addition to client registration, it also allows client to tell
     MMGSDI that MMGSDI will be notified whenever the client finishes
     its initialization. This will be useful in determining when to send
     TR for Refresh.

     For clients that do not want to inform MMGSDI, can still use this API and
     set notify_init to FALSE.

     The response to this request will return client ID to the calling task that
     is required to be used for subsequent MMGSDI API calls.

   PARAMETERS:
     notify_init:        Client will also inform MMGSDI when its init completes
     option:             Optional parameter, not used right now
     response_cb_ptr:    Callback for MMGSDI to call upon completion of the
                         registration request.
     client_ref_ptr:     User Data returned to the user upon completion of
                         the command.

   DEPENDENCIES:
     None

   LIMITATIONS:
     None

   RETURN VALUE:
     mmgsdi_return_enum_type

     MMGSDI_SUCCESS:          The command structure was properly generated
                              and queued onto the MMGSDI Command Queue.
     MMGSDI_INCORRECT_PARAMS: The parameters supplied to the API are not
                              within appropriate ranges.
     MMGSDI_CMD_QUEUE_FULL:   The command was not Queued to the MMGSDI Task
                              because the max number of commands are already
                              queued.

   SIDE EFFECTS:
     Will result in your callback getting called everytime MMGSDI Needs to
     notify any client of any events.
     The longer the client init takes, the longer it could take to send the
     TR for refresh. Also, if the client init takes very long, then it could
     result in the timer expiring and thus sending the TR at the wrong time.
=============================================================================*/
mmgsdi_return_enum_type  mmgsdi_client_id_reg_set_notify (
  boolean                    notify_init,
  mmgsdi_option_type         option,
  mmgsdi_callback_type       response_cb_ptr,
  mmgsdi_client_data_type    client_ref
);
/*~ FUNCTION mmgsdi_client_id_reg_set_notify */
/*~ ONERROR return MMGSDI_RPC_ERROR          */


/*=============================================================================
   FUNCTION:      MMGSDI_CLIENT_INIT_COMPLETE

   DESCRIPTION:
     This function will be called by the clients when they finish their
     initialization. This will be applicable to only those clients that
     register using  mmgsdi_client_id_reg_set_notify and notify_init set to
     TRUE.

   PARAMETERS:
     client_id:          Client Id of the caller
     slot_id:            Slot ID that the client completes with
     option:             Optional field, not used now
     response_cb_ptr:    Callback for MMGSDI to call  upon completion of the
                         registration request.
     client_ref_ptr:     User Data returned to the user upon completion of
                         the command.

   DEPENDENCIES:
     None

   LIMITATIONS:
     None

   RETURN VALUE:
     mmgsdi_return_enum_type

     MMGSDI_SUCCESS:          The command structure was properly generated
                              and queued onto the MMGSDI Command Queue.
     MMGSDI_INCORRECT_PARAMS: The parameters supplied to the API are not
                              within appropriate ranges.
     MMGSDI_CMD_QUEUE_FULL:   The command was not Queued to the MMGSDI Task
                              because the max number of commands are already
                              queued.

   SIDE EFFECTS:
     Will result in your callback getting called everytime MMGSDI Needs to
     notify any client of any events.
=============================================================================*/
mmgsdi_return_enum_type  mmgsdi_client_init_complete (
  mmgsdi_client_id_type      client_id,
  mmgsdi_slot_id_enum_type   slot_id,
  mmgsdi_option_type         option,
  mmgsdi_callback_type       response_cb_ptr,
  mmgsdi_client_data_type    client_ref
);
/*~ FUNCTION mmgsdi_client_init_complete */
/*~ ONERROR return MMGSDI_RPC_ERROR      */


/*===========================================================================
   FUNCTION MMGSDI_NAA_REFRESH

   DESCRIPTION
     This API is used by the Generic SIM Toolkit Task to request that a REFRESH
     be performed on the Mobile.  There are 5 GSM SIM Refresh Modes and 7 USIM
     Refresh Modes (as defined by the enum mmgsdi_refresh_mode_enum_type).

     MMGSDI_REFRESH_NAA_INIT_FULL_FCN      (SIM/USIM)
     MMGSDI_REFRESH_FCN                    (SIM/USIM)
     MMGSDI_REFRESH_NAA_INIT_FCN           (SIM/USIM)
     MMGSDI_REFRESH_NAA_INIT               (SIM/USIM)
     MMGSDI_REFRESH_RESET                  (SIM/USIM)
     MMGSDI_REFRESH_NAA_APP_RESET          (USIM)
     MMGSDI_REFRESH_3G_SESSION_RESET       (USIM)

   PARAMETERS:
     client_id:          Client ID of the caller
     card_slot:          Slot on which refresh has to be performed
     aid:                Application ID (only applicable to 3G)
     refresh_mode:       Refresh Mode as sent by the card
     refresh_files:      Refresh Files if any (optional)
     num_files:          Number of refresh files
     option:             Not used yet
     response_cb_ptr:    Callback for MMGSDI to call  upon completion of the
                         registration request.
     client_ref_ptr:     User Data returned to the user upon completion of
                         the command.
   DEPENDENCIES
     None

   LIMITATIONS:
     None

   RETURN VALUE
     mmgsdi_return_enum_type

   SIDE EFFECTS
     Depending on the Mode requested, various events are generated as a result
     of the REFRESH Peformed.
===========================================================================*/
mmgsdi_return_enum_type mmgsdi_naa_refresh(
  mmgsdi_client_id_type              client_id,
  mmgsdi_slot_id_enum_type           card_slot,
  mmgsdi_aid_type                    aid,
  mmgsdi_refresh_mode_enum_type      refresh_mode,
  mmgsdi_data_type                   refresh_files,
  uint8                              num_files,
  mmgsdi_option_type                 option,
  mmgsdi_callback_type               response_cb_ptr,
  mmgsdi_client_data_type            client_ref
);
/*~ FUNCTION mmgsdi_naa_refresh     */
/*~ ONERROR return MMGSDI_RPC_ERROR */


/*===========================================================================
   FUNCTION MMGSDI_OMA_BCAST

   DESCRIPTION
     This API is used by the Generic SIM Toolkit Task to send a OMA BCAST
     command as specified in 3GPP/ETSI for OMA BCAST Key Management System

     Modes of the command -
     MMGSDI_BCAST_SPE_AUTDIT_MODE
     MMGSDI_BCAST_SPE_SIG_MODE
     MMGSDI_BCAST_REC_AUDIT_MODE

   PARAMETERS:
     client_id:          Client ID of the caller
     card_slot:          Slot on which command has to be sent
     mode:               Mode in which the command will operate
     bcast_req:          Input data
     option:             Not used yet
     response_cb_ptr:    Callback for MMGSDI to call  upon completion of the
                         request.
     client_ref_ptr:     User Data returned to the user upon completion of
                         the command.
   DEPENDENCIES
     None

   LIMITATIONS:
     None

   RETURN VALUE
     mmgsdi_return_enum_type

   SIDE EFFECTS
     None.
===========================================================================*/
mmgsdi_return_enum_type mmgsdi_oma_bcast(
  mmgsdi_client_id_type              client_id,
  mmgsdi_slot_id_enum_type           card_slot,
  mmgsdi_bcast_mode_enum_type        mode,
  mmgsdi_data_type                   bcast_req,
  mmgsdi_option_type                 option,
  mmgsdi_callback_type               response_cb_ptr,
  mmgsdi_client_data_type            client_ref
);
/*~ FUNCTION mmgsdi_oma_bcast       */
/*~ ONERROR return MMGSDI_RPC_ERROR */


/*===========================================================================
   FUNCTION MMGSDI_CREATE_PKCS15_LOOKUP_TABLE

   DESCRIPTION
     Client invokes this API to create a PKCS15 lookup table which is further
     used to access PKCS15 EFs. The lookup table created by mmgsdi is shared
     with the client in the response call back pointer provided by the client.

   PARAMETERS:
     client_id:          Client ID of the caller
     card_slot:          Slot on which command has to be sent
     option:             Not used yet
     response_cb_ptr:    Callback for MMGSDI to call  upon completion of the
                         request.
     client_ref:         User Data returned to the user upon completion of
                         the command.
   DEPENDENCIES
     None

   LIMITATIONS:
     None

   RETURN VALUE
     mmgsdi_return_enum_type

   SIDE EFFECTS
     None.
===========================================================================*/
mmgsdi_return_enum_type  mmgsdi_create_pkcs15_lookup_table(
  mmgsdi_client_id_type    client_id,
  mmgsdi_slot_id_enum_type card_slot,
  mmgsdi_option_type       option,
  mmgsdi_callback_type     response_cb_ptr,
  mmgsdi_client_data_type  client_ref
);
/*~ FUNCTION mmgsdi_create_pkcs15_lookup_table */
/*~ ONERROR return MMGSDI_RPC_ERROR            */


/*===========================================================================
  FUNCTION MMGSDI_SEEK

  DESCRIPTION:
    This function performs GSM Seek command on card.
    The input parameters contain following information.

  PARAMETRES:
    client_id        : Client ID of the caller
    card_slot        : Slot on which command has to be sent
    file_access      : File Information
    seek_direction   : Search Direction
    seek_pattern     : Search Data
    option           : RFU
    response_cb_ptr  : Callback for MMGSDI to call  upon completion of the
                       request.
    client_ref       : User Data returned to the user upon completion of
                       the command.
  DEPENDENCIES
    None

  LIMITATIONS:
    None

  RETURN VALUE
    mmgsdi_return_enum_type
    MMGSDI_SUCCESS          : The command structure was properly generated
                              and queued onto the MMGSDI Command Queue.
    MMGSDI_INCORRECT_PARAMS : The parameters supplied to the API are
                              not within appropriate ranges.
    MMGSDI_CMD_QUEUE_FULL   : The command was not queued to the MMGSDI Task
                              because the max numbers of commands are already
                              queued.
    MMGSDI_NOT_SUPPORTED    : when card is not ICC card.

  SIDE EFFECTS
    None.
===========================================================================*/
mmgsdi_return_enum_type mmgsdi_seek(
  mmgsdi_client_id_type           client_id,
  mmgsdi_slot_id_enum_type        card_slot,
  mmgsdi_access_type              file_access,
  mmgsdi_seek_direction_enum_type seek_direction,
  mmgsdi_seek_data_type           seek_pattern,
  mmgsdi_option_type              option,
  mmgsdi_callback_type            response_cb_ptr,
  mmgsdi_client_data_type         client_ref
);
/*~ FUNCTION mmgsdi_seek            */
/*~ ONERROR return MMGSDI_RPC_ERROR */


/*===========================================================================
  FUNCTION MMGSDI_SEARCH

  DESCRIPTION
    This function performs Search command on UICC card.
    The input parameters contain following information.

  PARAMETER:
    client_id                 : Client ID of the caller
    card_slot                 : Slot on which command has to be sent
    file_access               : File Information
    search_type               : UICC Search Type
                                Valid Range:
                                  MMGSDI_UICC_ENHANCE_SEARCH
                                  MMGSDI_UICC_SIMPLE_SEARCH
    search_record_num         : Record Number, from where search will
                                start in file.
                                Valid Range:
                                  0x00: Current Record
                                  0x01-0xFE: Physical record Number
    search_direction          : Search Direction
    enhance_search_offset_data: Offset, from where search start in a record.
                                Valid Range for offset data
                                  0x00-0xFF for MMGSDI_SEARCH_BY_CHAR type.
                                  0x00-0xFE for MMGSDI_SEARCH_BY_OFFSET type.
    search_pattern            : Data to be search
    option                    : RFU
    response_cb_ptr           : Callback for MMGSDI to call  upon completion
                                of the request.
    client_ref                : User Data returned to the user upon completion
                                of the command.

  DEPENDENCIES
    None

  LIMITATIONS:
    None

  RETURN VALUE
    mmgsdi_return_enum_type
    MMGSDI_SUCCESS           : The command structure was properly generated
                               and queued onto the MMGSDI Command Queue.
    MMGSDI_INCORRECT_PARAMS  : The parameters supplied to the API are
                               not within appropriate ranges.
    MMGSDI_CMD_QUEUE_FULL    : The command was not queued to the MMGSDI Task
                               because the max numbers of commands are already
                               queued.
    MMGSDI_NOT_SUPPORTED     : when card is not UICC card.

  SIDE EFFECTS
    None.
===========================================================================*/
mmgsdi_return_enum_type mmgsdi_search(
  mmgsdi_client_id_type             client_id,
  mmgsdi_slot_id_enum_type          card_slot,
  mmgsdi_access_type                file_access,
  mmgsdi_search_enum_type           search_type,
  mmgsdi_rec_num_type               search_record_num,
  mmgsdi_search_direction_enum_type search_direction,
  mmgsdi_search_offset_data_type    enhanced_search_offset_data,
  mmgsdi_search_data_type           search_pattern,
  mmgsdi_option_type                option,
  mmgsdi_callback_type              response_cb_ptr,
  mmgsdi_client_data_type           client_ref
);
/*~ FUNCTION mmgsdi_search          */
/*~ ONERROR return MMGSDI_RPC_ERROR */


/* ==========================================================================
  FUNCTION MMGSDI_CONVERT_PATH_TO_FILE_ENUM

  DESCRIPTION
    This function, on providing the complete path, returns the mmgsdi file
    enum

  PARAMETER
    path_type             : Path information for which mmgsdi file enum
                            to be determined.
    mmgsdi_file_enum_ptr  : pointer to mmgsdi file enum type, result is
                            stored in this pointer.

  DEPENDENCIES
    None

  LIMITATIONS
    NONE

  RETURN VALUE
    mmgsdi_return_enum_type
    MMGSDI_SUCCESS
    MMGSDI_ERROR

  SIDE EFFECTS
    NONE
----------------------------------------------------------------------------*/
mmgsdi_return_enum_type mmgsdi_convert_path_to_file_enum(
  mmgsdi_path_type       path_type,
  mmgsdi_file_enum_type *mmgsdi_file_enum_ptr
);


#endif /* MMGSDILIB_H */
