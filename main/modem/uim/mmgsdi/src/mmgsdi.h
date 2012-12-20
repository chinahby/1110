#ifndef MMGSDI_H
#define MMGSDI_H
/*===========================================================================


            M M G S D I   D E F I N I T I O N S

                      A N D   F U N C T I O N S


===========================================================================*/

/*===========================================================================
                        COPYRIGHT INFORMATION

Copyright (c) 2004 - 2009 QUALCOMM, Incorporated and its licensors.  All
Rights Reserved.  QUALCOMM Proprietary.  Export of this technology or software
is regulated by the U.S. Government. Diversion contrary to U.S. law prohibited.

                        EDIT HISTORY FOR MODULE

This section contains comments describing changes made to the module.
Notice that changes are listed in reverse chronological order.

$Header: //source/qcom/qct/modem/uim/su/baselines/qsc1110/rel/3.3.65/uim/mmgsdi/src/mmgsdi.h#2 $$ $DateTime: 2011/03/25 06:51:16 $

when       who     what, where, why
--------   ---     -----------------------------------------------------------
03/22/11   ssr     Fixed the MEID cache
03/19/09   js      Support for Enhanced NAA Refresh by file path
02/12/09   kk      Add files to refresh file list for re-reading during FCN
11/18/08   js      Updated featurization for Send APDU related code
11/15/08   js      Removed MFLO featurization for Send APDU related code
10/29/08   tml     Modem restart client id dereg clean up
10/14/08   sun     Increased the refresh timers
10/07/08   kk      Fixed handling of invalid tags and missing tags in create 
                   pkcs15 table
09/06/08   kk      Changed pkcs15 command handling
08/28/08   sun     If command fails in the original state, then return original 
                   error mmgsdi status to client
08/20/08   jk      Support for Generate Key API
07/22/08   js      Renamed MMGSDI_MAX_PATH_LEN to MMGSDI_NUM_ARR_FILES
07/20/08   sp      Add file type to mmgsdi_read_extra_info_type
07/17/08   kk      Added support for PKCS 15
06/12/08   ssr     Support of MMGSDI Search functionality
06/23/08   tml     Added featurization for BT in WM
06/21/08   kk      Added PKCS15 Support
04/08/08   nk      Added CDMA Refresh Support
03/28/08   sun     Added support for OMA Bcast
03/28/08   sk      Fixed warnings
03/13/08   nk      Added CDMA FDN Support
02/21/08   sun     Added CPHS file for FCN registration
01/30/08   nk      Added #defines for JCDMA Default Values
01/29/08   sun     Added new state for refresh. 
12/10/07   sun     Verify PIN Internally when SAP is Disconnected
11/19/07   js      Added Radio Access Technology Balancing support
11/15/07   sun     Added support for NAA refresh
11/08/07   vs/nk   Added EUIMID feature support
10/02/07   tml     Added proprietary app session support
09/10/07   tml     Added MFLO Supports
08/15/07   jk      Changes to Support DATA Authentication MMGSDI API 
07/02/07   sun     Added new #defines for MBMS
05/22/07   nk      Added esn_me_change_flag to mmgsdi_slot_data_type
05/17/07   wli     Added support for JCDMA RUIM
05/07/07   sp      Added mmgsdi_malloc and mmgsdi_free
05/02/07   sun     Added support for Srv Available and USIM Auth APIs
04/26/07   sp      Add new defines for ARR files
04/17/07   sun     Set up a timer if external module other than UIM does not
                   respond
02/21/07   sun     Added cached flag to each of the pins
02/27/07   jk      Reduce MAX_CLIENT_INFO for Memory Reduction
01/17/07   sp      Add MMGSDI_STATE_PIN_EVT_SENT  
                   to mmgsdi states
01/12/07   sun     Determine the SEID and then look for the Record Number 
                   for the ARR File while parsing security attributes
01/11/07   sun     Save the PIN1 Report Status. Need to be used while sending 
                   the confirmation.
12/27/06   sp      Back out pin event changes.
12/12/06   tml     Added member activate to get_file_attr_req_type 
11/06/06   sp      Added new mmgsdi state MMGSDI_STATE_PIN_EVT_SENT 
10/25/06   tml     Added initial support for Universal PIN replacement support
10/02/06   sp      Added me support for fdn, bdn and acl to mmgsdi data
09/08/06   sun     Added new #defines.
08/20/06   tml     Added App Reset and 3G Session Reset supports
08/07/06   tml     Added ACL support
08/07/06   sun     Added pin and valid_pin flag to mmgsdi_app_pin_info
07/26/06   sun     Added support for Multi USIM App and MMGSDI PIN
06/12/06   sun     Added new Sap State
05/23/06   tml     BDN and rehabilitate and invalidate support
04/15/06   tml     Moved onchip data to slot specific global structure
04/13/06   jar     Added ONCHIP SIM Support
03/14/06   sun     Added refresh option to mmgsdi_refresh_req_type
02/14/06   nk      Merged for ARR caching and by path changes
01/29/06   tml     Moved mmgsdi_sim_refresh to mmgsdi.h
01/04/06   tml     Updated enum name for SAP and Card powerup and down
12/14/05   tml     MMGSDI Cleanup
12/05/05   sun     Added support for MMGSDI_SAP_CARD_READER_STATUS_REQ and
                   MMGSDI_SAP_RESET_REQ
11/08/05   pv      rename mmgsdi_check_card_status - mmgsdi_3G_ready_status_ind
11/03/05   tml     Fixed header
11/03/05   sun     Added support for refresh
10/28/05   sun     Fixed SAP Disconnect
10/20/05   sun     Added support for BT Deregisteration
09/29/05   tml     Client Deregistration support
08/30/05   jar     Added support to send/receive Status Commands
08/25/05   pv      Added support for powering down and powerering up SIM/USIM
07/28/05   sst     Fixed compile errors when SAP feature is not defined
07/26/05   sst     Added MMGSDIBT support
07/14/05   tml     Added original request type
07/12/05   tml     Session Management and ISIM support
06/15/05   tml     Removed data pointer for holding read response
02/24/05   sk      Initial version

=============================================================================*/

/*=============================================================================

                     INCLUDE FILES FOR MODULE

=============================================================================*/

#include "comdef.h"
#include "mmgsdilib.h"
#include "uim.h"
#include "cm.h"

/*=============================================================================

                       DATA DECLARATIONS

=============================================================================*/


/*=============================================================================

                       STRUCTURE DECLARATIONS

=============================================================================*/

#ifdef FEATURE_MMGSDI_MEMORY_REDUCTION
#define MMGSDI_MAX_CLIENT_INFO   20
#else
#define MMGSDI_MAX_CLIENT_INFO   50
#endif /* FEATURE_MMGSDI_MAX_CLIENT_INFO */
#define MMGSDI_CLIENT_ID_SHIFT   32
#define MMGSDI_SESSION_ID_MASK 0x00000000FFFFFFFF

#define MMGSDI_INVALID_SESSION_ID 0x00000000FFFFFFFF

#define MMGSDI_MAX_REC_NUM        0xFF
#define MMGSDI_MAX_REC_LEN        0xFF
#define MMGSDI_MAX_APP_ID_LEN     0x10

#define MMGSDI_CMD_QUEUE_MAX_SIZE 30

#define MMGSDIUTIL_VAR_LEN_MAX_NUM 10

#define MMGSDI_MAX_CHANNEL_INFO  4

#define MMGSDIBT_PIN_LEN 16

#define MMGSDI_APP_TEMPLATE_TAG  0x61
#define MMGSDI_APP_ID_TAG        0x4F
#define MMGSDI_APP_LABEL_TAG                   0x50
#define MMGSDI_APP_PATH_TAG                    0x51
#define MMGSDI_APP_CMD_TO_PERFORM_TAG          0x52
#define MMGSDI_APP_DISCRETIONARY_DATA_TAG      0x53
#define MMGSDI_APP_DISCRETIONARY_ASN_OBJ_TAG   0x73


#define MMGSDI_USIM_APP_ID_RID_BYTE_1    0xA0
#define MMGSDI_USIM_APP_ID_RID_BYTE_2    0x87
#define MMGSDI_USIM_APP_COUNTRY_CODE     0x10
#define MMGSDI_USIM_APP_PROVIDER_CODE    0x02
#define MMGSDI_UICC_APP_LABEL_TAG        0x50
#define MMGSDI_UICC_APP_LABEL_MAX_LENGTH 32


#define MMGSDI_TLV_TAG_LEN    0x01
#define MMGSDI_TLV_LEN_LEN    0x01

#define MMGSDI_SW2_CMD_X_TRYS_LEFT        0xC0
#define MMGSDI_MASK_FOR_HIGH_BYTE         0xF0
#define MMGSDI_MASK_FOR_LOW_BYTE          0x0F

#define MMGSDI_NUM_ARR_FILES             3
#define MMGSDI_ARR_UNDER_MF               0x2F06
#define MMGSDI_ARR_FIRST_DF_LEVEL         0x6F06

#define MMGSDI_GET_RSP_CACHE_TABLE_FULL          -1
#define MMGSDI_MAX_AVAIL_GET_RSP_CACHE_ENTRIES 0x04

/*-----------------------------------------------------
ETSI 102.221, Table 9.2 
SEID
'00' The global application PIN is disabled AND (the 
     Universal PIN is either disabled or enabled 
     and used or not known)
'01' The current global application PIN is enabled OR 
     the Universal PIN is enabled but it is not to be 
     used for verification AND the global application 
     PIN is disabled
-----------------------------------------------------*/
#define MMGSDI_SEID_00                    0x00
#define MMGSDI_SEID_01                    0x01

/* Offset for Select Len */
#define MMGSDI_SELECT_APDU_LEN_OFFSET     0x04
/* Min Len for Select APDU */
#define MMGSDI_SELECT_APDU_MIN_LEN        0x05

#ifdef FEATURE_UIM_EUIMID
#define MMGSDI_SF_EUIMID_PHASE            0x3
#endif /* FEATURE_UIM_EUIMID */

#ifdef FEATURE_MMGSDI_ONCHIP_SIM
#error code not present
#endif /* FEATURE_MMGSDI_ONCHIP_SIM */

/* FDN, BDN, ACL support masks */
#define  MMGSDI_FDN_SUPPORTED_MASK      0x0001
#define  MMGSDI_BDN_SUPPORTED_MASK      0x0002
#define  MMGSDI_ACL_SUPPORTED_MASK      0x0004

/*  Bits set for fdn, bdn and acl */
#define  MMGSDI_DEFAULT_ME_CONFIG_VALUE 0x7

/* Minimum length of an APDU Len */
#define MMGSDI_MIN_APDU_LEN             0x4

/*Default Value for No of PIN Retries*/
#define MMGSDI_DEFAULT_PIN_RETRIES 3

extern rex_timer_type mmgsdi_ext_api_timer;

extern rex_timer_type mmgsdi_refresh_timer;

extern rex_timer_type mmgsdi_refresh_auto_trigger_timer;

#define MMGSDI_AUTO_REFRESH_TIMER 2000

#define MMGSDI_REFRESH_OK_TO_INIT_TIMER 10000

#define MMGSDI_REFRESH_OK_TO_FCN_TIMER 10000

#define MMGSDI_REFRESH_INIT_COMPLETE_TIMER 120000

#define MMGSDI_REFRESH_FCN_COMPLETE_TIMER 120000

/*Timer for Dependencies on modules other than UIM*/
#define MMGSDI_EXT_TIMER 2000

#define MMGSDI_ICC_SRV_MASK 0x03 /*2 consecutive bits make 1 service*/

#ifdef  FEATURE_UIM_JCDMA_RUIM_SUPPORT 
/* Type of locks */
#define MMGSDI_JCDMA_LINE_LOCK                    0 
#define MMGSDI_JCDMA_NW_LOCK                      1
#define MMGSDI_JCDMA_SP_LOCK                      2
#define MMGSDI_JCDMA_UG_LOCK                      3
/* The value read from EF to see if the card is test card */
#define MMGSDI_JCDMA_UIM_TEST_VERSION             1
#define MMGSDI_JCDMA_UIM_LOCK_IND              0x01
#define MMGSDI_JCDMA_UIM_IMSI_PROGRAMMED       0x80
#define MMGSDI_JCDMA_UIM_IMSI_STATUS           0xF7
#define MMGSDI_JCDMA_UIMID_DEFAULT       0xFFFFFFFF
#define MMGSDI_JCDMA_UIM_SP_UG_DEFAULT         0xFF
#define MMGSDI_JCDMA_8BITS                        8
#define MMGSDI_JCDMA_RUIMID_DEFAULT            0xFF
#define MMGSDI_JCDMA_IMSI_MCC1_DEFAULT          440
#define MMGSDI_JCDMA_IMSI_MCC2_DEFAULT          441
#define MMGSDI_JCDMA_SP_ID_DEFAULT                0
#define MMGSDI_JCDMA_UG_ID_DEFAULT                0
#define MMGSDI_JCDMA_INDICATOR_DEFAULT            1 
#endif  /* FEATURE_UIM_JCDMA_RUIM_SUPPORT */

#ifdef  FEATURE_UIM_RUIM
#define MMGSDI_JCDMA_ESN_ME_BIT                0x01
#endif /*FEATURE_UIM_RUIM */

#ifdef FEATURE_MMGSDI_MBMS
#error code not present
#endif /*FEATURE_MMGSDI_MBMS*/

#ifdef FEATURE_RUIM_CDMA_REFRESH
#define MMGSDI_REFRESH_REG_FILES               92
#else 
#define MMGSDI_REFRESH_REG_FILES               61
#endif /* FEATURE_RUIM_CDMA_REFRESH */

#ifdef FEATURE_MMGSDI_CDMA
#define MMGSDI_CDMA_FDN_FILES       3
#endif /* FEATURE_MMGSDI_CDMA */

/* PKCS15 related definitions */
#ifdef FEATURE_MMGSDI_PKCS15
#error code not present
#endif /* FEATURE_MMGSDI_PKCS15 */

typedef int32  mmgsdi_channel_id_type;

typedef rex_tcb_type mmgsdi_rex_tcb_type;

/* ----------------------------------------------------------------------------
   STRUCT:      MMGSDI_APDU_DATA_GET_RSP_CACHE

   DESCRIPTION:
     This cache will maintain the "Get Response" Buffer which is normally
   done by our UIM Drivers to ensure we don't lose the Get Response Data
   and at the same time still maintain an Active representation of the
   APDU Exchange Protocol as define din 7816-3.

   USAGE:
   This cache is only active for a Client (Session if applicable) when sending
   APDUs.  If the client sends an APDU Command resulting in a GET RESPONSE,
   set of Status words (61XX, 9FXX, etc), the GET Response data is stored.
   The client must issue a Get Response APDU as the subsequent APDU to retrieve
   the data stored.  If the subsequent APDU from the client is not a GET
   Response APDU, the current cache will be cleared.
-------------------------------------------------------------------------------*/
typedef struct {
  mmgsdi_client_id_type     client_id;
  mmgsdi_slot_id_enum_type  slot_id;
  int32                     channel_id;
  mmgsdi_data_type          apdu;
}mmgsdi_apdu_data_get_rsp_cache_type;

typedef struct {
  mmgsdi_file_enum_type EF_name;
  mmgsdi_len_type       rec_num;
  mmgsdi_data_type      record_data;
} mmgsdi_record_cache_type;

typedef struct _mmgsdi_record_cache_node_type {
  struct _mmgsdi_record_cache_node_type *  next_p;
  mmgsdi_record_cache_type                 record_data;
}mmgsdi_record_cache_node_type;

typedef struct {
  mmgsdi_record_cache_node_type * head_p;
  mmgsdi_record_cache_node_type * tail_p;
}mmgsdi_record_cache_node_list_type;

/* ----------------------------------------------------------------------------
   ENUM:      MMGSDI_CMD_ENUM_TYPE

   DESCRIPTION:
     The command enum type that MMGSDI supports
-------------------------------------------------------------------------------*/
typedef enum {
  /* 0 */
  MMGSDI_CLIENT_ID_REG_REQ,
  MMGSDI_CLIENT_EVT_REG_REQ,
  MMGSDI_CLIENT_ID_DEREG_REQ,
  MMGSDI_CLIENT_EVT_DEREG_REQ,
  MMGSDI_READ_REQ,
  MMGSDI_WRITE_REQ,
  MMGSDI_GET_FILE_ATTR_REQ,
  MMGSDI_SESSION_OPEN_REQ,
  MMGSDI_SESSION_CLOSE_REQ,
  MMGSDI_SESSION_DEACTIVATE_REQ,
  /* 10 */
  MMGSDI_SESSION_SELECT_AID_REQ,
  MMGSDI_ISIM_AUTH_REQ,
  MMGSDI_SEND_APDU_REQ,
  MMGSDI_SAP_INIT_REQ,
  MMGSDI_SAP_PIN_VERIFY_REQ,
  MMGSDI_SAP_CONNECT_REQ, 
  MMGSDI_SAP_DISCONNECT_REQ,
  MMGSDI_SAP_GET_ATR_REQ,
  MMGSDI_SAP_SEND_APDU_REQ,
  MMGSDI_SAP_POWER_ON_REQ,
  /* 20 */
  MMGSDI_SAP_POWER_OFF_REQ,
  MMGSDI_SAP_CARD_READER_STATUS_REQ,
  MMGSDI_SAP_RESET_REQ,
  MMGSDI_CARD_PUP_REQ,
  MMGSDI_CARD_PDOWN_REQ,
  MMGSDI_SAP_DEREG_REQ,
  MMGSDI_CARD_STATUS_REQ,
  MMGSDI_REFRESH_REQ,
  MMGSDI_ENABLE_BDN_REQ,
  MMGSDI_DISABLE_BDN_REQ,
  /* 30 */
  MMGSDI_REHABILITATE_REQ,
  MMGSDI_INVALIDATE_REQ,
  MMGSDI_ACTIVATE_ONCHIP_SIM_REQ,
  MMGSDI_PIN_OPERATION_REQ,
  MMGSDI_SELECT_AID_REQ,
  MMGSDI_GET_ALL_PIN_STATUS_REQ,
  MMGSDI_ACL_OPERATION_REQ,
  MMGSDI_SRV_AVAILABLE_REQ,
  MMGSDI_USIM_AUTH_REQ,
  MMGSDI_JCDMA_GET_CARD_INFO_REQ,
    
  /* 40 */
  MMGSDI_COMPUTE_IP_AUTH_REQ,
  MMGSDI_RUN_CAVE_REQ,
  MMGSDI_GENERATE_KEY_VPM_REQ,
  MMGSDI_CARD_READER_STATUS_REQ,
  MMGSDI_GET_ATR_REQ,
  MMGSDI_MFLO_INIT_ACT_PARAMS_REQ, 
  MMGSDI_MFLO_GET_SUBSCRIBER_ID_REQ,
  MMGSDI_MFLO_GET_PUBLIC_KEY_REQ,
  MMGSDI_MFLO_SIGN_UCAST_REQ,
  MMGSDI_MFLO_VERIFY_UCAST_REQ,

  /* 50 */
  MMGSDI_CARD_RESET_REQ,
  MMGSDI_CLIENT_INIT_COMP_REQ,
  MMGSDI_BCAST_REQ,
  MMGSDI_CREATE_PKCS15_TABLE_REQ,
  MMGSDI_SEARCH_REQ,
  MMGSDI_SAP_REPORT_RSP,
  MMGSDI_UIM_REPORT_RSP,
  MMGSDI_CLIENT_ID_REG_RSP,
  MMGSDI_CNF,

  MMGSDI_MAX_CMD_ENUM = 0x7FFFFFFF
} mmgsdi_cmd_enum_type;



/* ----------------------------------------------------------------------------
   ENUM:      MMGSDI_TECH_ENUM_TYPE

   DESCRIPTION:
     The technology enum type
-------------------------------------------------------------------------------*/
typedef enum {
  MMGSDI_TECH_GSM,
  MMGSDI_TECH_UMTS,
  MMGSDI_TECH_CDMA,
  MMGSDI_MAX_TECH_ENUM = 0x7FFFFFFF
} mmgsdi_tech_enum_type;

/* ----------------------------------------------------------------------------
   ENUM:      MMGSDI_DIR_ENUM_TYPE

   DESCRIPTION:
     The enum contains the directory
-------------------------------------------------------------------------------*/
typedef enum {
  MMGSDI_MF_LEVEL = 0x3F00,
  MMGSDI_RUIM_LEVEL = 0x7F25,
  MMGSDI_GSM_LEVEL = 0x7f20,
  MMGSDI_TELECOM_LEVEL = 0x7f10,
  MMGSDI_ADF_LEVEL = 0x7fff,
  MMGSDI_DCS1800_LEVEL = 0x7F21,
  MMGSDI_DEFAULT_LEVEL = 0xFFFF
} mmgsdi_dir_enum_type;


/* ----------------------------------------------------------------------------
   ENUM:      MMGSDI_STATE_ENUM_TYPE

   DESCRIPTION:
     The Card State. Please do not change the order of the states.
-------------------------------------------------------------------------------*/
typedef enum {
  MMGSDI_STATE_NOT_INIT = 0x00000000,
  MMGSDI_STATE_NO_CARD,
  MMGSDI_STATE_CARD_INSERTED,
  MMGSDI_STATE_APP_DETECTED,
  MMGSDI_STATE_PIN_EVT_SENT,
  MMGSDI_STATE_CARD_INIT_COMPLETED,
  MMGSDI_STATE_ENUM_MAX = 0x7FFFFFFF
} mmgsdi_state_enum_type;


/* ----------------------------------------------------------------------------
   ENUM:      MMGSDI_PIN_ENUM_TYPE

   DESCRIPTION:
     The Card Holder Verification Values (CHVs) or Personal
      Identification Numbers (PINs) for a SIM/USIM/RUIM.
      ADM pins refer to administrative PINs
-------------------------------------------------------------------------------*/
typedef enum {
  MMGSDI_APP1_PIN1           = 0x00000000,
  MMGSDI_APP1_PIN2,
  MMGSDI_APP2_PIN1,
  MMGSDI_APP2_PIN2,
  MMGSDI_APP3_PIN1,
  MMGSDI_APP3_PIN2,
  MMGSDI_APP4_PIN1,
  MMGSDI_APP4_PIN2,
  MMGSDI_APP5_PIN1,
  MMGSDI_APP5_PIN2,
  MMGSDI_APP6_PIN1,
  MMGSDI_APP6_PIN2,
  MMGSDI_APP7_PIN1,
  MMGSDI_APP7_PIN2,
  MMGSDI_APP8_PIN1,
  MMGSDI_APP8_PIN2,
  MMGSDI_SESSION_UNIVERSAL_PIN,
  MMGSDI_SESSION_ADM1,
  MMGSDI_SESSION_ADM2,
  MMGSDI_SESSION_ADM3,
  MMGSDI_SESSION_ADM4,
  MMGSDI_SESSION_ADM5,
  MMGSDI_SESSION_ADM6,
  MMGSDI_SESSION_ADM7,
  MMGSDI_SESSION_ADM8,
  MMGSDI_SESSION_ADM9,
  MMGSDI_SESSION_ADM10,
  MMGSDI_SESSION_ADM11,
  MMGSDI_SESSION_ADM12,
  MMGSDI_SESSION_ADM13,
  MMGSDI_SESSION_ADM14,
  MMGSDI_SESSION_ADM15,
  MMGSDI_SESSION_MAX_PIN_ENUM = 0x7FFFFFFF
} mmgsdi_session_pin_enum_type;

#ifdef FEATURE_MMGSDI_ONCHIP_SIM
#error code not present
#endif /* FEATURE_MMGSDI_ONCHIP_SIM */

typedef enum {
    MMGSDI_KEYREF_ALWAYS = 0x00,
    MMGSDI_PIN_APPL_1 = 0x01,
    MMGSDI_PIN_APPL_2 = 0x02,
    MMGSDI_PIN_APPL_3 = 0x03,
    MMGSDI_PIN_APPL_4 = 0x04,
    MMGSDI_PIN_APPL_5 = 0x05,
    MMGSDI_PIN_APPL_6 = 0x06,
    MMGSDI_PIN_APPL_7 = 0x07,
    MMGSDI_PIN_APPL_8 = 0x08,
    MMGSDI_UNIVERSAL_PIN_KEY = 0x11,
    MMGSDI_SEC_PIN_APPL_1 = 0x81,
    MMGSDI_SEC_PIN_APPL_2 = 0x82,
    MMGSDI_SEC_PIN_APPL_3 = 0x83,
    MMGSDI_SEC_PIN_APPL_4 = 0x84,
    MMGSDI_SEC_PIN_APPL_5 = 0x85,
    MMGSDI_SEC_PIN_APPL_6 = 0x86,
    MMGSDI_SEC_PIN_APPL_7 = 0x87,
    MMGSDI_SEC_PIN_APPL_8 = 0x88,
    MMGSDI_CHV1_KEY       = 0xFD, /*Not from 102.221*/
    MMGSDI_CHV2_KEY       = 0xFE, /*Not from 102.221*/
    MMGSDI_NEVER          = 0xFF
}mmgsdi_key_ref_values_enum_type;

/* ----------------------------------------------------------------------------
   ENUM:      MMGSDI_REFRESH_STATE_ENUM_TYPE

   DESCRIPTION:
     This enum specifies the current state of refresh
-------------------------------------------------------------------------------*/
typedef enum {
  MMGSDI_REFRESH_READY_STATE,
  MMGSDI_REFRESH_WAIT_FOR_OK_TO_INIT_STATE,
  MMGSDI_REFRESH_WAIT_FOR_OK_TO_FCN_STATE,
  MMGSDI_REFRESH_COMP_IN_PROGRESS_STATE,
  MMGSDI_REFRESH_WAIT_FOR_INIT_COMPLETE_STATE,
  MMGSDI_REFRESH_WAIT_FOR_FCN_COMPLETE_STATE,
  MMGSDI_REFRESH_MAX_STATE = 0x7FFFFFFF
} mmgsdi_refresh_state_enum_type;

typedef struct{
  mmgsdi_key_ref_values_enum_type   key_ref;
  mmgsdi_pin_enum_type              pin_id;
  mmgsdi_pin_status_enum_type       status;
  mmgsdi_pin_replace_enum_type      pin_replacement;
  uint32                            num_retries;
  uint32                            num_unblock_retries;
  mmgsdi_static_data_type           pin_data;
  boolean                           valid_pin; /*the PIN Data may not be valid*/
  boolean                           cached;
}mmgsdi_app_pin_info_type;

/* ----------------------------------------------------------------------------
   STRUCTURE:      MMGSDI_INT_APP_INFO_TYPE

   DESCRIPTION:
     The structure contains the application information on the card.
-------------------------------------------------------------------------------*/
typedef struct{
  boolean                     app_selected;
  boolean                     proprietary_app;
  mmgsdi_app_pin_info_type   *pin1;
  mmgsdi_app_pin_info_type    pin2;
  mmgsdi_app_pin_info_type   *universal_pin;
  mmgsdi_aid_type             app_data;
}mmgsdi_int_app_info_type;

/* ----------------------------------------------------------------------------
   STRUCTURE:      MMGSDI_ME_CAPABILITY_TYPE

   DESCRIPTION:
     The structure contains the me capabilities to support fdn, bdn & acl.
-------------------------------------------------------------------------------*/
typedef struct {
  boolean   me_supports_fdn;
  boolean   me_supports_bdn;
  boolean   me_supports_acl;
}mmgsdi_me_capability_type;

/* ----------------------------------------------------------------------------
   STRUCTURE:      MMGSDI_SLOT_DATA_TYPE

   DESCRIPTION:
     The structure contains the global MMGSDI data for slot
-------------------------------------------------------------------------------*/
typedef struct {
  mmgsdi_state_enum_type                mmgsdi_state;
#if (defined(FEATURE_BT) || defined(FEATURE_WM_BT)) && defined (FEATURE_BT_EXTPF_SAP)
#error code not present
#endif /*(FEATURE_BT || FEATURE_WM_BT) && (FEATURE_BT_EXTPF_SAP)*/
  mmgsdi_apps_type                      applications;
  mmgsdi_apps_type                      operational_apps;
  mmgsdi_int_app_info_type             *app_info_ptr;
  uint32                                sel_aid_index;
#ifdef FEATURE_MMGSDI_ONCHIP_SIM
#error code not present
#endif /* FEATURE_MMGSDI_ONCHIP_SIM */
#ifdef FEATURE_UIM_JCDMA_RUIM_SUPPORT
  mmgsdi_jcdma_card_info_enum_type      jcdma_card_info; 
#endif /* FEATURE_UIM_JCDMA_RUIM_SUPPORT */
  mmgsdi_esn_me_change_enum_type        esn_me_change_flag;
  mmgsdi_refresh_state_enum_type        refresh_state;
} mmgsdi_slot_data_type;

/* ----------------------------------------------------------------------------
   STRUCTURE:      MMGSDI_GENERIC_DATA_TYPE

   DESCRIPTION:
     The structure contains general global data for MMGSDI
-------------------------------------------------------------------------------*/
typedef struct {
  boolean                    task_stopped;
  mmgsdi_client_id_type      client_id;
  cm_client_id_type          mmgsdi_cm_id;
  boolean                    any_call_present;
  mmgsdi_me_capability_type  me_capabilities;
} mmgsdi_generic_data_type;

/* ----------------------------------------------------------------------------
   STRUCTURE:      MMGSDI_SW_STATUS_TYPE

   DESCRIPTION:
     The structure contains the sw1 and sw2 returned
-------------------------------------------------------------------------------*/
typedef struct {
  uint8        sw1;
  uint8        sw2;
} mmgsdi_sw_status_type;

/* ----------------------------------------------------------------------------
   STRUCTURE:      MMGSDI_INTERNAL_REFRESH_FILE_LIST_TYPE

   DESCRIPTION:    The structure has the information for the files that have been
                   Refreshed or the list of files that the client is interested
                   in receiving notification for
                   From file_list_ptr and file_path_ptr only one of the pointers
                   shall point to valid memory location and other pointer shall 
                   be null. Variable num_files shall be indicate for the number
                   of files enums in file_list_ptr and variable num_file_paths
                   will indicate the number of file paths in file_path_ptr.
-------------------------------------------------------------------------------*/
typedef struct {
   uint32                   num_files;
   mmgsdi_file_enum_type   *file_list_ptr;
   mmgsdi_path_type        *file_path_ptr;
   uint32                   num_file_paths;
}mmgsdi_internal_refresh_file_list_type;

/* ----------------------------------------------------------------------------
   STRUCTURE:      MMGSDI_REQUEST_HEADER_TYPE

   DESCRIPTION:
     This structure contains the Request Header Information used to direct
     the MMGSDI Task as to what type of command is contained within the payload
     of the request.
     client_id:      Client ID
     response_type:  Request Command Type
     response_len:   Length of message + header
     payload_len:    Length of the message payload: this includes the size of
                     the structure without the header as well as the size of
                     any pointer data being allocated.
                     Note: The additional data pointer being allocated is not
                     a continuous memory space
     request_id_ptr: Request ID (task_tcb_ptr)
     slot_id:        Slot that the request is to be performed on
     client_data:    Client data that will be returned in the response
     response_cb:    Response Callback Pointer
-------------------------------------------------------------------------------*/
typedef struct {
  mmgsdi_client_id_type     client_id;
  mmgsdi_cmd_enum_type      orig_request_type;
  mmgsdi_cmd_enum_type      request_type;
  mmgsdi_len_type           request_len;
  mmgsdi_len_type           payload_len;
  mmgsdi_rex_tcb_type      *request_id_ptr;
  mmgsdi_slot_id_enum_type  slot_id;
  mmgsdi_client_data_type   client_data;
  mmgsdi_callback_type      response_cb;
} mmgsdi_request_header_type;


/* ----------------------------------------------------------------------------
   STRUCTURE:      MMGSDI_READ_REQ_TYPE

   DESCRIPTION:
     This structure is used when generating a read request
     for the MMGSDI Task.  Based on the parameters, this
     command can be used for accessing the data for LINEAR FIXED,
     CYCLIC and TRANSPARENT files.
     request_header: Read Request Header information
     access:         File to be read by name
     rec_num:        Record number to be accessed in case of Linear Fixed or
                     Cyclic files
     offset:         Offset to Transparent files to be accessed
     data_len:       Length of data requested to be read
     file_type:      Linear Fixed, Cyclic or Transparent file
-------------------------------------------------------------------------------*/
typedef struct {
  mmgsdi_request_header_type      request_header;
  mmgsdi_access_type              access;
  mmgsdi_rec_num_type             rec_num;
  mmgsdi_offset_type              offset;
  mmgsdi_len_type                 data_len;
  mmgsdi_file_structure_enum_type file_type;
} mmgsdi_read_req_type;


/* ----------------------------------------------------------------------------
   STRUCTURE:      MMGSDI_WRITE_REQ_TYPE

   DESCRIPTION:
     This structure is used when generating a write request
     for the MMGSDI Task.  Based on the parameters, this
     command can be used for accessing the data for LINEAR FIXED,
     CYCLIC or TRANSPARENT files
     request_header: Write Request Header information
     access:         File to be written by name
     rec_num:        Record number to be written in case of Linear Fixed or
                     Cyclic files
     offset:         Offset to Transparent files to be written
     file_type:      Linear Fixed, Cyclic or Transparent file
     data:           Data to be written to the file
-------------------------------------------------------------------------------*/
typedef struct {
  mmgsdi_request_header_type      request_header;
  mmgsdi_access_type              access;
  mmgsdi_rec_num_type             rec_num;
  mmgsdi_offset_type              offset;
  mmgsdi_file_structure_enum_type file_type;
  mmgsdi_write_data_type          data;
  mmgsdi_client_data_type         int_client_data;
} mmgsdi_write_req_type;

/* ----------------------------------------------------------------------------
   STRUCTURE:      MMGSDI_CLIENT_ID_REG_REQ_TYPE

   DESCRIPTION:
     This structure is used when generating a client_id_reg request
     for the MMGSDI Task.
     request_header: Client ID Registration Request Header information
     evt_cb_ptr:     Event Callback function pointer that client wants to
                     receive event notification from.
-------------------------------------------------------------------------------*/
typedef struct {
  mmgsdi_request_header_type    request_header;
  mmgsdi_evt_callback_type    * evt_cb_ptr;
  boolean                       notify_init;
} mmgsdi_client_id_reg_req_type;

/* ----------------------------------------------------------------------------
   STRUCTURE:      MMGSDI_CLIENT_ID_DEREG_REQ_TYPE

   DESCRIPTION:
     This structure is used when generating a client_id_dereg request
     for the MMGSDI Task.
     request_header: Client ID Deregistration Request Header information
     evt_cb_ptr:     Event Callback function pointer that client wants to
                     remove from receiving event notification from.
-------------------------------------------------------------------------------*/
typedef struct {
  mmgsdi_request_header_type    request_header;
  mmgsdi_evt_callback_type    * evt_cb_ptr;
} mmgsdi_client_id_dereg_req_type;

/* --------------------------------------------------------------------------
   STRUCTURE:      MMGSDI_PIN_OPERATION_REQ_TYPE

   DESCRIPTION:    This structure is used to build a PIN Operations request
                   To the MMGSDI Task.  PIN Operations are classified as:
                       Verify PIN, Disable PIN, Enable PIN, Change PIN, Unblock
                   PIN.

   MEMBERS:
     mmgsdi_pin_operation_type pin_op :  PIN Operation
     mmgsdi_pin_enum_type      pin_id :  PIN ID for operation.  Applicable to
                                        :  Verify,Enable,Disable,Change,Unblock
     mmgsdi_pin_data_type      pin_puk:  PIN or PUK.  Applicable to
                                        :  Verify,Enable,Disable,Change,Unblock
     mmgsdi_pin_data_type      new_pin:  New PIN.  Applicable only to
                                        :  Change,Unblock
---------------------------------------------------------------------------*/
typedef struct {
  mmgsdi_request_header_type      request_header;
  mmgsdi_pin_operation_enum_type  pin_op;
  mmgsdi_pin_enum_type            pin_id;
  mmgsdi_static_data_type         pin_puk;
  mmgsdi_static_data_type         new_pin;
} mmgsdi_pin_operation_req_type;

/* --------------------------------------------------------------------------
   STRUCTURE:      MMGSDI_SEL_AID_REQ_TYPE

   DESCRIPTION:    This structure is used to build a Select AID Request
                   To the MMGSDI Task.

   MEMBERS:
---------------------------------------------------------------------------*/
typedef struct {
  mmgsdi_request_header_type      request_header;
  mmgsdi_static_data_type         app_data;
} mmgsdi_select_aid_req_type;


/* --------------------------------------------------------------------------
   STRUCTURE:      MMGSDI_GET_ALL_PIN_STATUS_REQ_TYPE

   DESCRIPTION:    This structure is used to build a PIN Status request
                   To the MMGSDI Task.

   MEMBERS:
---------------------------------------------------------------------------*/
typedef struct {
  mmgsdi_request_header_type      request_header;
} mmgsdi_get_all_pin_status_req_type;

/* ----------------------------------------------------------------------------
   STRUCTURE:      MMGSDI_GET_FILE_ATTR_REQ_TYPE

   DESCRIPTION:
     This structure is used when generating a get_file_attributes request
     for the MMGSDI Task.  Based on the parameters, this
     command can be used for getting file attributes for LINEAR FIXED,
     CYCLIC or TRANSPARENT files and master file and
     dedicated files
     request_header: Get File Attributes Request Header information
     access:         File to be accessed by name
-------------------------------------------------------------------------------*/
typedef struct {
  mmgsdi_request_header_type request_header;
  mmgsdi_access_type         access;
  boolean                    activate_aid;
  mmgsdi_client_data_type    int_client_data;
} mmgsdi_get_file_attr_req_type;

/* ----------------------------------------------------------------------------
   STRUCTURE:      MMGSDI_SESSION_OPEN_REQ_TYPE

   DESCRIPTION:
     This structure is used when generating a session_open request
     for the MMGSDI Task.
     request_header: Session Open Request Header information
     app_id_data:    application ID to have the session opened with
-------------------------------------------------------------------------------*/
typedef struct {
  mmgsdi_request_header_type request_header;
  mmgsdi_static_data_type    app_id_data;
} mmgsdi_session_open_req_type;

/* ----------------------------------------------------------------------------
   STRUCTURE:      MMGSDI_GET_FILE_ATTR_REQ_TYPE

   DESCRIPTION:
     This structure is used when generating a session_close request
     for the MMGSDI Task.
     request_header: Session Close Request Header information
-------------------------------------------------------------------------------*/
typedef struct {
  mmgsdi_request_header_type request_header;
} mmgsdi_session_close_req_type;

/* ----------------------------------------------------------------------------
   STRUCTURE:      MMGSDI_STATUS_REQ_TYPE

   DESCRIPTION:
     This structure is used when generating a status request
     for the MMGSDI Task.
     request_header: Status Request Header information
     me_app_status  :  Status of the UICC Application maintained by the ME
     ret_data_struct:  Structure of Return Data Requested
-------------------------------------------------------------------------------*/
typedef struct {
  mmgsdi_request_header_type       request_header;
  mmgsdi_status_me_app_enum_type   me_app_status;
  mmgsdi_status_ret_data_enum_type ret_data_struct;
} mmgsdi_status_req_type;

#if (defined(FEATURE_BT) || defined(FEATURE_WM_BT)) && defined (FEATURE_BT_EXTPF_SAP)
#error code not present
#endif /*(FEATURE_BT || FEATURE_WM_BT) && (FEATURE_BT_EXTPF_SAP)*/

#if (defined(FEATURE_BT) || defined(FEATURE_WM_BT)) && defined (FEATURE_BT_EXTPF_SAP)
#error code not present
#endif /*(FEATURE_BT || FEATURE_WM_BT) && (FEATURE_BT_EXTPF_SAP)*/

/* ----------------------------------------------------------------------------
   STRUCTURE:      MMGSDI_GET_ATR_REQ_TYPE

   DESCRIPTION:
     This structure is used when generating a get atr request
     for the MMGSDI Task.
------------------------------------------------------------------------------*/
typedef struct {
  mmgsdi_request_header_type request_header;
}mmgsdi_get_atr_req_type;

#if (defined(FEATURE_BT) || defined(FEATURE_WM_BT)) && defined (FEATURE_BT_EXTPF_SAP)
#error code not present
#endif /*(FEATURE_BT || FEATURE_WM_BT) && (FEATURE_BT_EXTPF_SAP)*/

/* ----------------------------------------------------------------------------
   STRUCTURE:      MMGSDI_SEND_APDU_REQ_TYPE

   DESCRIPTION:
     This structure is used when generating a SAP APDU request
     for the MMGSDI Task.
------------------------------------------------------------------------------*/
typedef struct {
  mmgsdi_request_header_type request_header;
  int32                      channel_id;
  mmgsdi_send_apdu_data_type data;
} mmgsdi_send_apdu_req_type;

#if (defined(FEATURE_BT) || defined(FEATURE_WM_BT)) && defined (FEATURE_BT_EXTPF_SAP)
#error code not present
#endif /*(FEATURE_BT || FEATURE_WM_BT) && (FEATURE_BT_EXTPF_SAP)*/

#if (defined(FEATURE_BT) || defined(FEATURE_WM_BT)) && defined (FEATURE_BT_EXTPF_SAP)
#error code not present
#endif /*(FEATURE_BT || FEATURE_WM_BT) && (FEATURE_BT_EXTPF_SAP)*/

#if (defined(FEATURE_BT) || defined(FEATURE_WM_BT)) && defined (FEATURE_BT_EXTPF_SAP)
#error code not present
#endif /* (defined(FEATURE_BT) || defined(FEATURE_WM_BT)) && defined (FEATURE_BT_EXTPF_SAP) */

/* ----------------------------------------------------------------------------
   STRUCTURE:      MMGSDI_CARD_READER_STATUS_REQ_TYPE

   DESCRIPTION:
     This structure is used when generating a card reader status request
     for the MMGSDI Task.
------------------------------------------------------------------------------*/
typedef struct {
  mmgsdi_request_header_type request_header;
}mmgsdi_card_reader_status_req_type;

#if (defined(FEATURE_BT) || defined(FEATURE_WM_BT)) && defined (FEATURE_BT_EXTPF_SAP)
#error code not present
#endif /*(FEATURE_BT || FEATURE_WM_BT) && (FEATURE_BT_EXTPF_SAP)*/

/* ----------------------------------------------------------------------------
   STRUCTURE:      MMGSDI_CARD_PUP_REQ_TYPE

   DESCRIPTION:
     This structure is used when generating a power up CARD request
     for the MMGSDI Task.
------------------------------------------------------------------------------*/
typedef struct {
  mmgsdi_request_header_type              request_header;
  mmgsdi_card_pup_options_enum_type       option;
}mmgsdi_card_pup_req_type;

/* ----------------------------------------------------------------------------
   STRUCTURE:      MMGSDI_CARD_RESET_REQ_TYPE

   DESCRIPTION:
     This structure is used when generating a Card reset request
     for the MMGSDI Task.
------------------------------------------------------------------------------*/
typedef struct {
  mmgsdi_request_header_type request_header;
  uim_protocol_type          protocol;
} mmgsdi_card_reset_req_type;

/* ----------------------------------------------------------------------------
   STRUCTURE:      MMGSDI_CARD_PDOWN_REQ_TYPE

   DESCRIPTION:
     This structure is used when generating a power down CARD request
     for the MMGSDI Task.
------------------------------------------------------------------------------*/
typedef struct {
  mmgsdi_request_header_type              request_header;
  mmgsdi_card_pdown_options_enum_type     option;
}mmgsdi_card_pdown_req_type;

/* ----------------------------------------------------------------------------
   STRUCTURE:      MMGSDI_BDN_REQ_TYPE

   DESCRIPTION:
     This structure is used when generating a BDN enable or disable request
     for the MMGSDI Task.
     request_header: BDN Request Header information
-------------------------------------------------------------------------------*/
typedef struct {
  mmgsdi_request_header_type       request_header;
} mmgsdi_bdn_req_type;

/* ----------------------------------------------------------------------------
   STRUCTURE:      MMGSDI_ACL_OPERATION_REQ_TYPE

   DESCRIPTION:
     This structure is used when generating a ACL enable or disable request
     for the MMGSDI Task.
     request_header: ACL Request Header information
-------------------------------------------------------------------------------*/
typedef struct {
  mmgsdi_request_header_type       request_header;
  mmgsdi_acl_operation_enum_type   acl_op;
} mmgsdi_acl_operation_req_type;

/* ----------------------------------------------------------------------------
   STRUCTURE:      MMGSDI_REHABILITATE_REQ_TYPE

   DESCRIPTION:
     This structure is used when generating a rehabilitate request
     for the MMGSDI Task.
     request_header: Rehabilitate Request Header information
     access:         File to Rehabilitate
-------------------------------------------------------------------------------*/
typedef struct {
  mmgsdi_request_header_type       request_header;
  mmgsdi_access_type               access;
} mmgsdi_rehabilitate_req_type;

/* ----------------------------------------------------------------------------
   STRUCTURE:      MMGSDI_INVALIDATE_REQ_TYPE

   DESCRIPTION:
     This structure is used when generating a invalidate request
     for the MMGSDI Task.
     request_header: Rehabilitate Request Header information
     access:         File to Invalidate
-------------------------------------------------------------------------------*/
typedef struct {
  mmgsdi_request_header_type       request_header;
  mmgsdi_access_type               access;
} mmgsdi_invalidate_req_type;

/* ----------------------------------------------------------------------------
   STRUCTURE:      MMGSDI_SRV_AVAILABLE_REQ_TYPE

   DESCRIPTION:
     This structure is used when finding out if the service is enabled in
     CST/SST/UST
-------------------------------------------------------------------------------*/
typedef struct {
  mmgsdi_request_header_type       request_header;
  mmgsdi_service_enum_type         service;
} mmgsdi_srv_available_req_type;

/* ----------------------------------------------------------------------------
   STRUCTURE:      MMGSDI_AUTH_REQ_TYPE

   DESCRIPTION:
     This structure is used when generating an USIM authentication request
     for the MMGSDI Task.
     request_header: Read Request Header information
     auth_req:       authentication request
-------------------------------------------------------------------------------*/
typedef struct {
  mmgsdi_request_header_type           request_header;
  mmgsdi_autn_context_enum_type        auth_context;
  mmgsdi_data_type                     auth_req;
} mmgsdi_auth_req_type;

/* ----------------------------------------------------------------------------
   STRUCTURE:      MMGSDI_COMPUTE_IP_REQ_TYPE

   DESCRIPTION:
     This structure is used when generating COMPUTE IP request
     for the MMGSDI Task.
     request_header:        Request Header information
     compute_ip_data:       authentication request
-------------------------------------------------------------------------------*/
typedef struct {
  mmgsdi_request_header_type           request_header;
  mmgsdi_compute_ip_data_type          compute_ip_data;
} mmgsdi_compute_ip_req_type;

/* ----------------------------------------------------------------------------
   STRUCTURE:      MMGSDI_RUN_CAVE_REQ_TYPE

   DESCRIPTION:
     This structure is used when generating RUN CAVE request
     for the MMGSDI Task.
     request_header:        Request Header information
     rand_type:             Random Type for RUN CAVE Operation  
     rand_chal:             Random Challenge for RUN CAVE Operation
     dig_len:               Digit Length for RUN CAVE
     digits:                Digits for RUN CAVE
     process_control:       Process Control for RUN CAVE
     esn:                   ESN For Run CAVE calculation
-------------------------------------------------------------------------------*/
typedef struct {
  mmgsdi_request_header_type            request_header;
  mmgsdi_cave_rand_type_type            rand_type;
  mmgsdi_cave_rand_chal_type            rand_chal;
  mmgsdi_cave_dig_len_type              dig_len;
  mmgsdi_cave_digits_type               digits;
  mmgsdi_cave_process_control_type      process_control;
  mmgsdi_cave_esn_type                  esn;
} mmgsdi_run_cave_req_type;

/* ----------------------------------------------------------------------------
   STRUCTURE:      MMGSDI_GENERATE_KEY_REQ_TYPE

   DESCRIPTION:
     This structure is used when generating a GENERATE KEY request
     for the MMGSDI Task.
     request_header:        Request Header information
     first_octet:           First Octet for generate key Operation  
     last_octet:            Last Octet for Generate Key Operation
-------------------------------------------------------------------------------*/
typedef struct {
  mmgsdi_request_header_type            request_header;
  mmgsdi_single_vpm_octet_type          first_octet;
  mmgsdi_single_vpm_octet_type          last_octet;
} mmgsdi_generate_key_req_type;

/* ----------------------------------------------------------------------------
   STRUCTURE:      MMGSDI_CLIENT_INIT_COMPLETE_REQ_TYPE

   DESCRIPTION:
     This structure is used to communicate to MMGSDI that the client 
     has completed its init. Used in Refresh
     request_header: Request Header information
-------------------------------------------------------------------------------*/
typedef struct {
  mmgsdi_request_header_type    request_header;
} mmgsdi_client_init_complete_req_type;

#ifdef FEATURE_MMGSDI_MFLO
#error code not present
#endif /* FEATURE_MMGSDI_MFLO */

/*--------------------------------------------------------------------------
  STRUCTURE:      MMGSDI_SEARCH_REQ_TYPE

  DESCRIPTION:
    This structure is used when generating a search or seek request
    for the MMGSDI Task.  Based on the parameters, this
    command can be used for accessing the data for LINEAR FIXED,
    CYCLIC files
    request_header    : Search Request Header information
    access            : File to be search 
    search type       : ICC_SEARCH, UICC_SIMPLE and UICC_ENHANCED Search
    rec_num           : It is used for UICC SIMPLE and ENHANCED Search. 
                        It will ignore for ICC_SEARCH.
                        It indicate the record number from where search will
                        start in a record file.
                        Valid Range for record number:-
                        0x00         : Current Record
                        0x01 to 0xFE : Physical Record number.
                        
    Seek Direction    : Search direction for ICC Search.
    Search Direction  : Search Direction for UICC Search.
    offset            : It is used for UICC ENHANCED SEARCH. It indicate the
                        record offset from where, search will start in 
                        a record.
                        Valid Range: 
                         0x00-0xFF for MMGSDI_SEARCH_BY_CHAR type.
                         0x00–0xFE for MMGSDI_SEARCH_BY_OFFSET type. 
    data              : data to search.
--------------------------------------------------------------------------*/
typedef struct {
  mmgsdi_request_header_type        request_header;
  mmgsdi_access_type                access;
  mmgsdi_search_enum_type           search_type;
  mmgsdi_rec_num_type               rec_num;
  mmgsdi_seek_direction_enum_type   seek_direction;
  mmgsdi_search_direction_enum_type search_direction;
  mmgsdi_search_offset_data_type    enhanced_search_offset;
  mmgsdi_search_data_type           data;
} mmgsdi_search_req_type;

/* ----------------------------------------------------------------------------
   STRUCTURE:      MMGSDI_UIM_REPORT_RSP_TYPE

   DESCRIPTION:
     The structure contains the raw data return from UIM as a result of a
     request sent from MMGSDI
-------------------------------------------------------------------------------*/
typedef struct {
  mmgsdi_request_header_type  request_header;  /* Request header information   */
  boolean                     is_uim_success;  /* UIM_PASS = TRUE,
                                                  UIM_FAIL = FALSE             */
  int32                       uim_rpt_code;    /* map to uim_report_code_type  */
  mmgsdi_sw_status_type       status_word;     /* sw1 and sw2 from the Card    */
  boolean                     cmd_transacted;  /* whether the cmd was sent to
                                                  the card or not              */
  int32                       rsp_data_index;  /* index into
                                                  mmgsdi_client_req_table      */
  mmgsdi_data_type            rsp_data;
  mmgsdi_return_enum_type     mmgsdi_error_status;
} mmgsdi_uim_report_rsp_type;

/* ----------------------------------------------------------------------------
   STRUCTURE:      MMGSDI_CLIENT_ID_REG_RSP_TYPE

   DESCRIPTION:
     The structure contains the client reg/dereg response
-------------------------------------------------------------------------------*/
typedef struct {
  mmgsdi_request_header_type  request_header;  /* Request header information   */
  mmgsdi_return_enum_type     status;          /* Registration success/failed  */
  int32                       rsp_data_index;  /* index into
                                                  mmgsdi_client_req_table      */
} mmgsdi_client_id_reg_rsp_type;

/* ----------------------------------------------------------------------------
   STRUCTURE:      MMGSDI_GENERIC_RSP_TYPE

   DESCRIPTION:
     The structure contains the generic response information
-------------------------------------------------------------------------------*/
typedef struct {
  mmgsdi_request_header_type  request_header;  /* Request header information   */
  mmgsdi_return_enum_type     status;          /* Registration success/failed  */
  int32                       rsp_data_index;  /* index into
                                                  mmgsdi_client_req_table      */
} mmgsdi_generic_rsp_type;

/* ----------------------------------------------------------------------------
   STRUCTURE:      MMGSDI_REFRESH_CARD_REQ_TYPE

   DESCRIPTION:
     This structure is used when a refresh is issued to MMGSDI
     refresh_files: FCN 
     refresh_mode : Refresh Mode
     num_files    : Number of files in FCN
     AID          : Application ID
  -------------------------------------------------------------------------------*/
typedef struct{
  mmgsdi_refresh_mode_enum_type      refresh_mode;
  mmgsdi_data_type                   refresh_files;
  uint32                             num_files;
  mmgsdi_aid_type                    aid;
}
mmgsdi_refresh_card_req_type;

/* ----------------------------------------------------------------------------
   STRUCTURE:      MMGSDI_REFRESH_REG_REQ_TYPE

   DESCRIPTION:
     This structure is used for refresh registration/deregistration
     refresh_files: FCN registration
     vote_for_init: Vote for Terminating app
  -------------------------------------------------------------------------------*/
typedef struct{
  mmgsdi_internal_refresh_file_list_type    refresh_files;
  boolean                                   vote_for_init;
}mmgsdi_refresh_reg_req_type;

/* ----------------------------------------------------------------------------
   STRUCTURE:      MMGSDI_REFRESH_REQ_TYPE

   DESCRIPTION:
     This structure is used when generating a status request
     for the MMGSDI Task.
     request_header: Status Request Header information
     refresh_req   : Original Refresh Request
     refresh_files : Files to be refreshed/Files registered for fcn(optional)
     aid           : Application ID

     For refresh_req:
       MMGSDI_REFRESH_ORIG_REFRESH_REQ:
       MMGSDI_REFRESH_ORIG_AUTO_REFRESH_REQ:
       MMGSDI_REFRESH_ORIG_PUP_REQ:
         card_req : Refresh command sent by card
    
       MMGSDI_REFRESH_ORIG_REG_REQ:
       MMGSDI_REFRESH_ORIG_DEREG_REQ:
          reg_req : Vote to Terminate app and FCN request

       MMGSDI_REFRESH_ORIG_OK_TO_INIT_REQ:
       MMGSDI_REFRESH_ORIG_OK_TO_FCN_REQ:
          ok_req:  Ok to Refresh Request

       MMGSDI_REFRESH_ORIG_COMPLETE_REQ:
          complete_req: Pass/Fail

  -------------------------------------------------------------------------------*/
typedef struct {
  mmgsdi_request_header_type         request_header;
  mmgsdi_refresh_orig_enum_type      refresh_req_type;

  union{
    mmgsdi_refresh_card_req_type     card_req;
    mmgsdi_refresh_reg_req_type      reg_req;
    boolean                          pass_fail;
    boolean                          ok_req;
  }refresh;
} mmgsdi_refresh_req_type;

/* ----------------------------------------------------------------------------
   STRUCTURE:      MMGSDI_BCAST_REQ_TYPE

   DESCRIPTION:
     This structure is used when generating an OMA BCAST request
     for the MMGSDI Task.
     request_header: Bcast Request Header information
     bcast_mode:     MMGSDI_BCAST_SPE_AUDIT_MODE/ 
                     MMGSDI_BCAST_SPE_SIG_MODE/
                     MMGSDI_BCAST_REC_AUDIT_MODE
     bcast_req:      bcast request
-------------------------------------------------------------------------------*/
typedef struct {
  mmgsdi_request_header_type           request_header;
  mmgsdi_bcast_mode_enum_type          bcast_mode;
  mmgsdi_data_type                     bcast_req;
} mmgsdi_bcast_req_type;

/* ----------------------------------------------------------------------------
   UNION:      MMGSDI_RSP_TYPE

   DESCRIPTION:
     This union contains the response type
     uim_report_rsp:      Response from UIM
     client_id_reg_rsp:   Response for Client ID/Evt Reg and Dereg
     generic_rsp:         Generic Response
-------------------------------------------------------------------------------*/
typedef union {
  mmgsdi_uim_report_rsp_type       uim_report_rsp;
  mmgsdi_client_id_reg_rsp_type    client_id_reg_rsp;
  mmgsdi_generic_rsp_type          generic_rsp;
} mmgsdi_rsp_type;

/* ----------------------------------------------------------------------------
   STRUCTURE:      MMGSDI_WRITE_EXTRA_INFO_TYPE

   DESCRIPTION:
     The structure contains the write extra info
-------------------------------------------------------------------------------*/
typedef struct
{
  mmgsdi_access_type               access;
  mmgsdi_len_type                  rec_num;
  mmgsdi_len_type                  offset;
  mmgsdi_data_type                 data;
  mmgsdi_client_data_type          int_client_data;
} mmgsdi_write_extra_info_type;


/* ----------------------------------------------------------------------------
   STRUCTURE:      MMGSDI_READ_EXTRA_INFO_TYPE

   DESCRIPTION:
     The structure contains the read extra info
-------------------------------------------------------------------------------*/
typedef struct
{
  mmgsdi_access_type               access;
  mmgsdi_len_type                  offset;
  mmgsdi_len_type                  rec_num;
  mmgsdi_file_structure_enum_type  file_type;
} mmgsdi_read_extra_info_type;


/* ----------------------------------------------------------------------------
   STRUCTURE:      MMGSDI_REHABILITATE_EXTRA_INFO_TYPE

   DESCRIPTION:
     The structure contains the rehabilitate extra info
-------------------------------------------------------------------------------*/
typedef struct
{
  mmgsdi_access_type               access;
} mmgsdi_rehabilitate_extra_info_type;

/* ----------------------------------------------------------------------------
   STRUCTURE:      MMGSDI_INVALIDATE_EXTRA_INFO_TYPE

   DESCRIPTION:
     The structure contains the invalidate extra info
-------------------------------------------------------------------------------*/
typedef struct
{
  mmgsdi_access_type               access;
} mmgsdi_invalidate_extra_info_type;


/* ----------------------------------------------------------------------------
   STRUCTURE:      MMGSDI_GET_FILE_INFO_EXTRA_INFO_TYPE

   DESCRIPTION:
     The structure contains the get file attribute extra info
-------------------------------------------------------------------------------*/
typedef struct
{
  mmgsdi_access_type               access;
  mmgsdi_client_data_type          int_client_data;
} mmgsdi_get_file_info_extra_info_type;


#if (defined(FEATURE_BT) || defined(FEATURE_WM_BT)) && defined (FEATURE_BT_EXTPF_SAP)
#error code not present
#endif /*(FEATURE_BT || FEATURE_WM_BT) && (FEATURE_BT_EXTPF_SAP)*/

/* --------------------------------------------------------------------------
   STRUCTURE:      MMGSDI_NUM_RETRY_ENUM_TYPE

   DESCRIPTION:    The request to get the number of retries or the
                   number of unblock retries..

   MEMBERS:
---------------------------------------------------------------------------*/
typedef enum
{
  MMGSDI_RETRY_NONE,
  MMGSDI_GET_NUM_RETRIES,
  MMGSDI_GET_NUM_UNBLOCK_RETRIES
}mmgsdi_num_retry_enum_type;


/* --------------------------------------------------------------------------
   STRUCTURE:      MMGSDI_PIN_EXTRA_INFO_TYPE

   DESCRIPTION:    This structure is used to help build a PIN Operations
                   request to the MMGSDI Task.

   MEMBERS:
     mmgsdi_pin_operation_type orig_pin_op :  PIN Operation
     mmgsdi_pin_info_type      pin_info    :  PIN Information
     mmgsdi_num_retry_enum_type retry_type :  Type of Retry # being requested
---------------------------------------------------------------------------*/

typedef struct
{
  mmgsdi_pin_operation_enum_type  orig_pin_op;
  mmgsdi_app_pin_info_type        pin_info;
  mmgsdi_num_retry_enum_type      retry_type;
  mmgsdi_return_enum_type         pin1_report_status;
}mmgsdi_pin_extra_info_type;

#if (defined(FEATURE_BT) || defined(FEATURE_WM_BT)) && defined (FEATURE_BT_EXTPF_SAP)
#error code not present
#endif /*(FEATURE_BT || FEATURE_WM_BT) && (FEATURE_BT_EXTPF_SAP)*/

/* ----------------------------------------------------------------------------
   STRUCTURE:      MMGSDI_SEND_APDU_EXTRA_INFO_TYPE

   DESCRIPTION:
     The structure contains the send APDU extra info
------------------------------------------------------------------------------*/
typedef struct
{
  int32            channel_id;
  mmgsdi_data_type data;
} mmgsdi_send_apdu_extra_info_type;

#if (defined(FEATURE_BT) || defined(FEATURE_WM_BT)) && defined (FEATURE_BT_EXTPF_SAP)
#error code not present
#endif /*(FEATURE_BT || FEATURE_WM_BT) && (FEATURE_BT_EXTPF_SAP)*/

/* ----------------------------------------------------------------------------
   STRUCTURE:      MMGSDI_CARD_PUP_EXTRA_INFO_TYPE

   DESCRIPTION:
     The structure contains the card_pup extra info
------------------------------------------------------------------------------*/
typedef struct
{
  mmgsdi_card_pup_options_enum_type       option;
}mmgsdi_card_pup_extra_info_type;

/* ----------------------------------------------------------------------------
   STRUCTURE:      MMGSDI_CARD_PDOWN_EXTRA_INFO_TYPE

   DESCRIPTION:
     The structure contains the card_pdown extra info
------------------------------------------------------------------------------*/
typedef struct
{
  mmgsdi_card_pdown_options_enum_type     option;
}mmgsdi_card_pdown_extra_info_type;

/* ----------------------------------------------------------------------------
   STRUCTURE:      MMGSDI_CARD_REFRESH_EXTRA_INFO_TYPE

   DESCRIPTION:
     The structure contains the card_pdown extra info
------------------------------------------------------------------------------*/
typedef struct
{
  mmgsdi_refresh_stage_enum_type        stage;
  boolean                               send_evt;
  mmgsdi_refresh_orig_enum_type         refresh_req_type;
  mmgsdi_refresh_mode_enum_type         mode;
}mmgsdi_card_refresh_extra_info_type;

/* ----------------------------------------------------------------------------
   STRUCTURE:      MMGSDI_CLIENT_REQ_EXTRA_INFO_TYPE

   DESCRIPTION:
     The union contains the extra info that is required to be stored to be
     used in building the confirmation
-------------------------------------------------------------------------------*/

#ifdef FEATURE_MMGSDI_ONCHIP_SIM
#error code not present
#endif /* FEATURE_MMGSDI_ONCHIP_SIM */

/* --------------------------------------------------------------------------
   STRUCTURE:      MMGSDI_ACL_OPERATION_EXTRA_INFO_TYPE

   DESCRIPTION:    This structure is used to help build a ACL Operation
                   request to the MMGSDI Task.  (It is needed in case of
                   a failed attempt to process ACL with WRITE accses

   MEMBERS:
     mmgsdi_acl_operation_enum_type acl_op :  ACL Operation
---------------------------------------------------------------------------*/
typedef struct
{
  mmgsdi_acl_operation_enum_type      acl_op;
}mmgsdi_acl_operation_extra_info_type;

/* --------------------------------------------------------------------------
   STRUCTURE:      MMGSDI_SRV_AVAILABLE_EXTRA_INFO_TYPE

   DESCRIPTION:    This structure is used to help build a SRV AVAILABLE
                   request to the MMGSDI Task. 
   MEMBERS:
     boolean       srv_available
---------------------------------------------------------------------------*/
typedef struct
{
  boolean         srv_available;
}mmgsdi_srv_available_extra_info_type;

#ifdef FEATURE_MMGSDI_MFLO
#error code not present
#endif /* FEATURE_MMGSDI_MFLO */

/* --------------------------------------------------------------------
   STRUCTURE:      MMGSDI_SEARCH_EXTRA_INFO_TYPE

   DESCRIPTION:
     The structure contains the search extra info.
     access: File Information
---------------------------------------------------------------------*/
typedef struct
{
  mmgsdi_access_type               access;
} mmgsdi_search_extra_info_type;

/* --------------------------------------------------------------------------
   STRUCTURE:      MMGSDI_SESSION_CNF_ADD_INFO_TYPE

   DESCRIPTION:    This structure is used to help build a various session
                   Operation confirmation additional param to the MMGSDI Task.

   MEMBERS:
     mmgsdi_client_id_type session_id :  session Id that was being deactivated
     mmgsdi_aid_type       app_info   :  application detail
---------------------------------------------------------------------------*/
typedef struct
{
  mmgsdi_client_id_type               session_id;
  mmgsdi_aid_type                     app_info;
} mmgsdi_session_cnf_add_info_type;

#ifdef FEATURE_MMGSDI_PKCS15
#error code not present
#endif /* FEATURE_MMGSDI_PKCS15 */

typedef union
{
  mmgsdi_write_extra_info_type               write_data;
  mmgsdi_read_extra_info_type                read_data;
  mmgsdi_get_file_info_extra_info_type       get_file_attr_data;
  mmgsdi_send_apdu_extra_info_type           send_apdu_data;
#if (defined(FEATURE_BT) || defined(FEATURE_WM_BT)) && defined (FEATURE_BT_EXTPF_SAP)
#error code not present
#endif /*(FEATURE_BT || FEATURE_WM_BT) && (FEATURE_BT_EXTPF_SAP)*/
  mmgsdi_card_pup_extra_info_type            pup_data;
  mmgsdi_card_pdown_extra_info_type          pdown_data;
  mmgsdi_card_refresh_extra_info_type        refresh_data;
  mmgsdi_rehabilitate_extra_info_type        rehab_data;
  mmgsdi_invalidate_extra_info_type          invalidate_data;
  mmgsdi_pin_extra_info_type                 pin_data;
  mmgsdi_acl_operation_extra_info_type       acl_operation_data;
  mmgsdi_srv_available_extra_info_type       srv_available_data;
#ifdef FEATURE_MMGSDI_MFLO
#error code not present
#endif /* FEATURE_MMGSDI_MFLO */
  mmgsdi_search_extra_info_type              search_data;
#ifdef FEATURE_MMGSDI_PKCS15
#error code not present
#endif /* FEATURE_MMGSDI_PKCS15 */
} mmgsdi_client_req_extra_info_type;

/* ----------------------------------------------------------------------------
   STRUCTURE:      MMGSDI_UIM_CLIENT_REQ_USER_DATA_TYPE

   DESCRIPTION:
     The structure will be tabled in MMGSDI where its index will be passed to
     UIM for processing.
-------------------------------------------------------------------------------*/
typedef struct {
  mmgsdi_client_id_type                   client_id;
  mmgsdi_cmd_enum_type                    orig_request_type;
  mmgsdi_cmd_enum_type                    request_type;
  mmgsdi_rex_tcb_type                   * request_tcb_ptr;
  mmgsdi_slot_id_enum_type                slot_id;
  mmgsdi_client_data_type                 client_data;
  mmgsdi_callback_type                    response_cb;
  uim_cmd_type                          * uim_cmd_ptr;  // To be removed
  mmgsdi_client_req_extra_info_type     * extra_param_ptr;
  uint32                                  process_seq;
} mmgsdi_uim_client_req_user_data_type;

/* ----------------------------------------------------------------------------
   STRUCTURE:      MMGSDI_REFRESH_REG_DATA_TYPE

   DESCRIPTION:
     Data to represent the refresh data that the client has registered with
-------------------------------------------------------------------------------*/
typedef struct{
  mmgsdi_internal_refresh_file_list_type  refresh_files;
  boolean                                 vote_for_init;
  boolean                                 reg_by_path;
}mmgsdi_refresh_reg_data_type;

/* ----------------------------------------------------------------------------
   STRUCTURE:      MMGSDI_SESSION_PIN_INFO_TYPE

   DESCRIPTION:
     The structure contains the session pin information.
-------------------------------------------------------------------------------*/
typedef struct {
  mmgsdi_session_pin_enum_type  id;
  mmgsdi_pin_status_enum_type   status;
} mmgsdi_session_pin_info_type;

/* ----------------------------------------------------------------------------
   STRUCTURE:      MMGSDI_SESSION_INFO_TYPE

   DESCRIPTION:
     The structure contains session information.
-------------------------------------------------------------------------------*/
typedef struct {
  mmgsdi_client_id_type          session_id;
  mmgsdi_channel_id_type         channel_id;
  mmgsdi_slot_id_enum_type       slot;
  mmgsdi_int_app_info_type      *app_info_ptr;
  mmgsdi_refresh_reg_data_type   refresh_data;
} mmgsdi_session_info_type;

/* ----------------------------------------------------------------------------
   STRUCTURE:      MMGSDI_CLIENT_ID_REG_TABLE_TYPE

   DESCRIPTION:
     The structure contains the sw1 and sw2 returned
-------------------------------------------------------------------------------*/
typedef struct {
  mmgsdi_client_id_type      client_id;
  mmgsdi_evt_callback_type * evt_cb_ptr;
  mmgsdi_rex_tcb_type      * client_tcb_ptr;
  mmgsdi_len_type            num_session;
  mmgsdi_session_info_type   session_info[MMGSDI_MAX_CHANNEL_INFO];
  mmgsdi_session_info_type   default_app;
  
  /* Notify Init should only be used for default app, for 
     other app the refresh_complete command should be sent
     once the client's init is complete*/
  boolean                    notify_init;
  boolean                    init_complete;
  mmgsdi_callback_type       cmd_rsp_cb_ptr; /* Use to match with client_id_reg 
                                                for clean up purpose */
} mmgsdi_client_id_reg_table_type;

/* ----------------------------------------------------------------------------
   STRUCTURE:      MMGSDI_UIM_ADDITIONAL_PARAMS_TYPE

   DESCRIPTION:
     Additional parameter used between uicc to uim_uicc
     functions

-------------------------------------------------------------------------------*/
typedef struct{
  int32 path_len;
  word *path_array_ptr;
} mmgsdi_uim_additional_params_type;

/* ----------------------------------------------------------------------------
   STRUCTURE:      MMGSDI_REFRESH_NODE_TYPE

   DESCRIPTION:
     Global Data populated when a refresh command is received
     stage                : Refresh Stage for each node
     refresh_files        : Files as recived in FCN from the card
     rsp_received         : Boolean to determine if client has responded
-------------------------------------------------------------------------------*/
typedef struct _mmgsdi_refresh_node_type{
  mmgsdi_client_id_type                     client_id;
  mmgsdi_refresh_stage_enum_type            stage;
  mmgsdi_internal_refresh_file_list_type    refresh_files;
  boolean                                   rsp_received;
  struct _mmgsdi_refresh_node_type *next_ptr;
}mmgsdi_refresh_node_type;

/* ----------------------------------------------------------------------------
   STRUCTURE:      MMGSDI_REFRESH_INFO_TYPE

   DESCRIPTION:
     Global Data populated when a refresh command is received
     request_header       : header information stored from the original request
     mode                 : Refresh Mode as received from the card
     aid                  : Application ID as received from the card
     refresh_data         : Refresh data as received by the card,
                            contents may get modified based on the mode
                            and details of the command.
     additional_files_read: boolean to track if files other than FCN files 
                            have been read
     node_ptr             : head to the linked list of all refresh nodes
     refresh_status       : Final refresh status that will be send to the card
     orig_mode            : Original mode as received from the card
     
-------------------------------------------------------------------------------*/
typedef struct{
  mmgsdi_request_header_type    request_header;
  mmgsdi_refresh_mode_enum_type mode;
  mmgsdi_aid_type               aid;
  mmgsdi_refresh_reg_data_type  refresh_data;
  boolean                       additional_files_read;
  mmgsdi_refresh_node_type      *node_ptr;
  mmgsdi_return_enum_type       refresh_status;
  mmgsdi_refresh_mode_enum_type orig_mode;
}mmgsdi_refresh_info_type;

/* ----------------------------------------------------------------------------
   STRUCTURE:      MMGSDI_TASK_CNF_TYPE

   DESCRIPTION:
     Confirmation to client task command type

-------------------------------------------------------------------------------*/
typedef struct {
  mmgsdi_callback_type            client_req_cb;
  mmgsdi_cnf_type                 mmgsdi_cnf;
} mmgsdi_task_cnf_type;

#ifdef FEATURE_UIM_JCDMA_RUIM_SUPPORT 
/* --------------------------------------------------------------------------
   STRUCTURE:      MMGSDI_JCDMA_GET_CARD_INFO_REQ_TYPE

   DESCRIPTION:    This structure is used to build a jcdama get card info
                   request to the MMGSDI Task.

   MEMBERS:        mmgsdi_request_header_type      request_header

---------------------------------------------------------------------------*/
typedef struct {
  mmgsdi_request_header_type      request_header;
} mmgsdi_jcdma_get_card_info_req_type;
#endif  /* FEATURE_UIM_JCDMA_RUIM_SUPPORT */


#ifdef FEATURE_MMGSDI_PKCS15 
#error code not present
#endif  /* FEATURE_MMGSDI_PKCS15 */

/* ----------------------------------------------------------------------------
   STRUCTURE:      MMGSDI_TASK_CMD_DATA_TYPE

   DESCRIPTION:
     MMGSDI command type, to be put into the mmgsdi response command queue

-------------------------------------------------------------------------------*/
typedef struct {
  mmgsdi_cmd_enum_type         cmd_enum;
  union {
    /* Request */  
    mmgsdi_client_id_reg_req_type              client_id_reg_req;
    mmgsdi_client_id_dereg_req_type            client_id_dereg_req;
    mmgsdi_pin_operation_req_type              pin_operation_req;
    mmgsdi_read_req_type                       read_req;
    mmgsdi_write_req_type                      write_req;
    mmgsdi_get_file_attr_req_type              get_file_attr_req;
    mmgsdi_session_open_req_type               session_open_req;
    mmgsdi_session_close_req_type              session_close_req;
    mmgsdi_status_req_type                     status_req;
    mmgsdi_send_apdu_req_type                  send_apdu_req;
    mmgsdi_bdn_req_type                        enable_bdn_req;
    mmgsdi_bdn_req_type                        disable_bdn_req;
    mmgsdi_acl_operation_req_type              acl_operation_req;
    mmgsdi_rehabilitate_req_type               rehab_req;
    mmgsdi_invalidate_req_type                 invalidate_req;
#if (defined(FEATURE_BT) || defined(FEATURE_WM_BT)) && defined (FEATURE_BT_EXTPF_SAP)
#error code not present
#endif /*(FEATURE_BT || FEATURE_WM_BT) && (FEATURE_BT_EXTPF_SAP)*/
    mmgsdi_card_pup_req_type                   card_pup_req;
    mmgsdi_card_pdown_req_type                 card_pdown_req;
    mmgsdi_refresh_req_type                    refresh_req;
#ifdef FEATURE_MMGSDI_ONCHIP_SIM
#error code not present
#endif /* FEATURE_MMGSDI_ONCHIP_SIM */
    mmgsdi_select_aid_req_type                 select_aid_req;
    mmgsdi_get_all_pin_status_req_type         pin_status_req;
    mmgsdi_srv_available_req_type              srv_available_req;
    mmgsdi_auth_req_type                       auth_req;
    mmgsdi_compute_ip_req_type                 compute_ip_req;
    mmgsdi_run_cave_req_type                   run_cave_req;
    mmgsdi_generate_key_req_type               generate_key_req;
    mmgsdi_card_reset_req_type                 card_reset_req;
    mmgsdi_card_reader_status_req_type         card_reader_status_req;
    mmgsdi_get_atr_req_type                    get_atr_req;
#ifdef FEATURE_MMGSDI_MFLO
#error code not present
#endif /* FEATURE_MMGSDI_MFLO */
    mmgsdi_bcast_req_type                      bcast_req_type;
    /* Response */
    mmgsdi_uim_report_rsp_type                 uim_report_rsp;
    mmgsdi_client_id_reg_rsp_type              client_id_reg_rsp;
    mmgsdi_generic_rsp_type                    sap_report_rsp;
    mmgsdi_task_cnf_type                       mmgsdi_cnf;
#ifdef FEATURE_UIM_JCDMA_RUIM_SUPPORT 
    mmgsdi_jcdma_get_card_info_req_type        jcdma_card_info_req;
#endif /* FEATURE_UIM_JCDMA_RUIM_SUPPORT */
    mmgsdi_search_req_type                     search_req;
#ifdef FEATURE_MMGSDI_PKCS15
#error code not present
#endif /* FEATURE_MMGSDI_PKCS15 */
  } cmd;
} mmgsdi_task_cmd_data_type;

/* ----------------------------------------------------------------------------
   STRUCTURE:      MMGSDI_TASK_CMD_DATA_TYPE

   DESCRIPTION:
     The common command header for all commands received by MMGSDI task
-------------------------------------------------------------------------------*/
typedef struct {
  q_link_type                 link;
  mmgsdi_task_cmd_data_type   cmd;
}mmgsdi_task_cmd_type;

#ifdef  FEATURE_UIM_JCDMA_RUIM_SUPPORT 
/* Card type and lock codes */
typedef struct {                 
  boolean                           lock_enabled;
  mmgsdi_jcdma_card_info_enum_type  card_type;
  uint16                            imsi_mcc;
  uint32                            ruim_id;
  uint8                             sp_id;
  uint8                             ug_id;
}mmgsdi_jcdma_card_attr_type;
#endif  /* FEATURE_UIM_JCDMA_RUIM_SUPPORT */


/* ----------------------------------------------------------------------------
   STRUCTURE:      MMGSDI_MEID_DATA_TYPE

   DESCRIPTION:
     Data Structure to hold MEID value.
-------------------------------------------------------------------------------*/

typedef struct {
  qword                      meid;
  boolean                    meid_available;
} mmgsdi_meid_data_type;


/*=============================================================================

                                   MMGSDI MACROS
                        MACRO Definitions used through MMGSDI

=============================================================================*/


/* ----------------------------------------------------------------------------
   FUNCTION:      MMGSDI_CMD

   DESCRIPTION:
     This function allows client to put command into the MMGSDI command queue.

   DEPENDENCIES:
     None

   LIMITATIONS:
     Parameter check for specific commands are not performed

   RETURN VALUE:
     mmgsdi_return_enum_type

     MMGSDI_SUCCESS:          The command structure was properly generated
                              and queued onto the MMGSDI Command Queue.
     MMGSDI_CMD_QUEUE_FULL:   The command was not Queued to the MMGSDI Task
                              because the max number of commands are already
                              queued.

   SIDE EFFECTS:
     None
-------------------------------------------------------------------------------*/
mmgsdi_return_enum_type  mmgsdi_cmd ( mmgsdi_task_cmd_type * task_cmd_ptr  );


extern mmgsdi_client_id_reg_table_type mmgsdi_client_id_reg_table[MMGSDI_MAX_CLIENT_INFO];

extern mmgsdi_uim_client_req_user_data_type mmgsdi_client_req_table_info[MMGSDI_MAX_CLIENT_INFO];

extern mmgsdi_slot_data_type    mmgsdi_data_slot1;
#ifdef FEATURE_MMGSDI_DUAL_SLOT
extern mmgsdi_slot_data_type    mmgsdi_data_slot2;
#endif /* FEATURE_MMGSDI_DUAL_SLOT */

extern mmgsdi_generic_data_type mmgsdi_generic_data;

extern mmgsdi_int_app_info_type mmgsdi_app_pin_table[MMGSDI_MAX_APP_INFO];

extern mmgsdi_record_cache_node_list_type mmgsdi_record_cache_list_node;

extern mmgsdi_app_pin_info_type mmgsdi_app_pin1_table[MMGSDI_MAX_APP_INFO];
extern mmgsdi_app_pin_info_type mmgsdi_app_univ_pin_table[1];

extern mmgsdi_refresh_info_type *mmgsdi_refresh_info_ptr;

/* The list of files that we have refreshed for*/
extern  mmgsdi_file_enum_type  mmgsdi_refresh_reg_file_list[MMGSDI_REFRESH_REG_FILES];

#if defined(FEATURE_MMGSDI_GSM) || defined(FEATURE_MMGSDI_UMTS)
#error code not present
#endif /* FEATURE_MMGSDI_GSM || FEATURE_MMGSDI_UMTS */


/* ----------------------------------------------------------------------------
   FUNCTION:      MMGSDI_HANDLE_REG_FOR_CLIENT_ID_CB

   DESCRIPTION:
     This function provides internal callback function pointer to internal
     processing that MMGSDI does not care for the callback information returned

   DEPENDENCIES:
     None

   LIMITATIONS:
     Parameter check for specific commands are not performed

   RETURN VALUE:
     None

   SIDE EFFECTS:
     None
-------------------------------------------------------------------------------*/
void mmgsdi_handle_reg_for_client_id_cb(
  mmgsdi_return_enum_type status,
  mmgsdi_cnf_enum_type    cnf,
  const mmgsdi_cnf_type  *cnf_ptr);

/* ----------------------------------------------------------------------------
FUNCTION MMGSDI_REG_FOR_CLIENT_ID

DESCRIPTION
  Will register for a Client ID so that the MMGSDI APIs can be called.

DEPENDENCIES
  None

RETURN VALUE
  gsdi_returns_T

SIDE EFFECTS
  None
-------------------------------------------------------------------------------*/
mmgsdi_return_enum_type mmgsdi_reg_for_client_id(
  void
);

/* ----------------------------------------------------------------------------
FUNCTION MMGSDI_3G_READY_STATUS_IND

DESCRIPTION
  Function used to Queue Back a Command to send a status indicating that the
  ME is ready for a 3G session if it is a UICC card.  The MMGSDI Task will
  process the command at a later time.  This would be translated to a regular
  status command for a non UICC card

DEPENDENCIES
  None

RETURN VALUE
  gsdi_returns_T

SIDE EFFECTS
  None
-------------------------------------------------------------------------------*/
mmgsdi_return_enum_type mmgsdi_3G_ready_status_ind(
  mmgsdi_slot_id_enum_type slot
);


/* ----------------------------------------------------------------------------
FUNCTION MMGSDI_BUILD_REFRESH_REQ

DESCRIPTION
  This API is used by the Generic SIM Toolkit Task to request that a REFRESH
  be performed on the Mobile.  There are 5 GSM SIM Refresh Modes and 7 USIM
  Refresh Modes (as defined by the enum gsdi_refresh_modes_enum_type).

  MMGSDI_REFRESH_NAA_INIT_FULL_FCN      (SIM/USIM)
  MMGSDI_REFRESH_FCN                    (SIM/USIM)
  MMGSDI_REFRESH_NAA_INIT_FCN           (SIM/USIM)
  MMGSDI_REFRESH_NAA_INIT               (SIM/USIM)
  MMGSDI_REFRESH_RESET                  (SIM/USIM)
  MMGSDI_REFRESH_NAA_APP_RESET          (USIM)
  MMGSDI_REFRESH_3G_SESSION_RESET       (USIM)


DEPENDENCIES
  None

RETURN VALUE
  mmgsdi_return_enum_type.

SIDE EFFECTS
  Depending on the Mode requested, various events are generated as a result
  of the REFRESH Peformed.
-------------------------------------------------------------------------------*/
mmgsdi_return_enum_type mmgsdi_build_refresh_req(
  mmgsdi_client_id_type              client_id,
  mmgsdi_slot_id_enum_type           card_slot,
  const mmgsdi_refresh_req_type      *req_ptr,
  mmgsdi_callback_type               response_cb_ptr,
  mmgsdi_client_data_type            client_ref
);

/* ----------------------------------------------------------------------------
FUNCTION MMGSDI_EVT_CB

DESCRIPTION
  Internal Event Callback function registered during mmgsdi_init so that
  MMGSDI can receive event notification

DEPENDENCIES
  None

RETURN VALUE
  void

SIDE EFFECTS
  State variables may be updated as needed
===========================================================================*/
void mmgsdi_evt_cb (
  const mmgsdi_event_data_type *event_ptr
);


#ifdef FEATURE_MMGSDI_NV_ME_CONFIG
/* ----------------------------------------------------------------------------
FUNCTION MMGSDI_GET_ME_CONFIG_PARAM_FROM_NV

DESCRIPTION
This function gets the me capabilities for fdn, bdn and acl.
Reads the nv_item and sets the global data. If the nv_item is not enabled, then
the function writes the defaul value to the nv. If the nv_item could not be
read, a failure is return.

DEPENDENCIES
  None

RETURN VALUE
 mmgsdi_return_enum_type
 MMGSDI_SUCCESS : Upon successful read of nv item
 MMGSDI_ERROR   : Upon a nv read/write failure

SIDE EFFECTS
 If nv operation is not succesful, this function returns error and can put
 the me to no card.
-------------------------------------------------------------------------------*/
mmgsdi_return_enum_type mmgsdi_get_me_config_params_from_nv(void);
#endif /* FEATURE_MMGSDI_NV_ME_CONFIG */


/*===========================================================================
FUNCTION MMGSDI_BUILD_POST_PIN1_CMD

DESCRIPTION
  Function to build the GSDI Internal Request and put into the GSDI
  command queue so that GSDI can be kicked into the post pin1 initialization
  process if needed

DEPENDENCIES
  None

RETURN VALUE
  void

SIDE EFFECTS
  None.
===========================================================================*/
void mmgsdi_build_post_pin1_cmd(mmgsdi_slot_id_enum_type slot);


/*===========================================================================
FUNCTION MMGSDI_TIMER_EXPIRY

DESCRIPTION
This function builds a response if an external module that MMGSDI is 
dependent on does not respond

DEPENDENCIES
  None

RETURN VALUE
  None

SIDE EFFECTS
 Builds an Error Response to MMGSDI Client

===========================================================================*/
void mmgsdi_timer_expiry (unsigned long req);


/*===========================================================================

FUNCTION MMGSDI_MALLOC

DESCRIPTION
  mmgsdi_malloc calls mem_malloc to request memory from the tmc_heap or tmc_small_heap
  depending on whether or not the request greater than 64KB if 
  FEATURE_MMGSDI_USE_SMALL_HEAP is defined and FEATURE_LOW_MEMORY_USAGE is not.

DEPENDENCIES
  None.

RETURN VALUE
  None.

SIDE EFFECTS
  a pointer is allocated.
===========================================================================*/
void * mmgsdi_malloc(dword size);


/*===========================================================================

FUNCTION MMGSDI_FREE

DESCRIPTION
  The mmgsdi_free frees the pointer from the regular tmc_heap or the tmc_heap_small
  based on the pointer address.

DEPENDENCIES
  None.

RETURN VALUE
  None.

SIDE EFFECTS
  the pointer is freed.
===========================================================================*/
void mmgsdi_free(void * ptr);


/*===========================================================================
FUNCTION MMGSDI_GET_MEID

DESCRIPTION
  It returns cached MEID data.

DEPENDENCIES
  None

RETURN VALUE
  mmgsdi_meid_data_type

SIDE EFFECTS
  None.
===========================================================================*/
mmgsdi_meid_data_type mmgsdi_get_meid(void);

/*===========================================================================

FUNCTION MMGSDI_CACHE_MEID_DATA

DESCRIPTION
  Caching MEID data from NV memory.

DEPENDENCIES
  None.

RETURN VALUE
  None.

SIDE EFFECTS
  None.
===========================================================================*/
void mmgsdi_cache_meid_data(void);

#endif /* MMGSDI_H */

