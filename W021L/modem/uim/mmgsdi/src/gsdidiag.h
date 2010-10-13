#ifndef GSDIDIAG_H
#define GSDIDIAG_H
/*====*====*====*====*====*====*====*====*====*====*====*====*====*====*====*

                   GSDI DIAG Diagnostic Interface

Supports automated test of the MMGSDI (Multimode Generic Sim Driver
Interface) via DIAG commands

Copyright (c) 2003-2009 by QUALCOMM, Incorporated.  All Rights Reserved.
*====*====*====*====*====*====*====*====*====*====*====*====*====*====*====*/

/*===========================================================================

                              Edit History

 $Header: //source/qcom/qct/modem/uim/mmgsdi/rel/07H1_2/src/gsdidiag.h#11 $ $DateTime: 2009/04/28 18:54:25 $ $Author: jsharma $

when       who     what, where, why
--------   ---     ---------------------------------------------------------
04/16/09   js      Support for gsdi diag perso get feature data command
04/15/09   js      Provision for sending delayed response for mmgsdi api failure
04/06/09   mib     Added support for USIM authenticate command
03/31/09   mib     Added support for onchip_usim
03/31/09   js      Workaround for a misalignment when processing
                   gsdidiag_sim_read() and gsdidiag_sim_write()
03/19/09   js      Support for Enhanced Refresh by file path
02/27/09   mib     Added support for gsdi_sim_get_cphs_information
02/26/09   nb      Updated for MMGSDI PERSO Commands
02/24/09   js      Added command enums for refresh by path
02/13/09   mib     Delayed implementation of gsdi perso commands
08/27/08   SP      Added support for get_dck_num_retries.
08/15/08   tml     Added GSTK Diag support
07/31/08   SSR     Update search for blocking the search directions belonging
                   to Record Pointer.
07/31/08   kk      PKCS15 lookup table updated with gsdidiag enum types
07/20/08   sp      Add support for gsdidiag delayed sim increase
07/17/08   kk      Added support for PKCS 15
06/12/08   ssr     Support of MMGSDI Search functionality
06/23/08   tml     Added featurization for BT in WM
02/15/08   kk      Added support for MMGSDI event handling and other cmds
                   for handling refresh
04/12/07   sun     Increased the buffer size for sim_pin_unblock_new
03/07/07   sun     Added support for Disable with Replacement
03/02/07   nk      Added #define GSDIDAG_NULL_CHAR_LEN
01/19/07   sun     Added support for Checking Service Available
12/19/06   tml     Add support for path transparent and record access
10/16/06   tml     Get All PIN Status diag support
10/11/06   jar     Added a #define for GSDIDIAG_NOT_SUPPORTED
10/11/06   sun     Added support for Universal and Multi USIM
09/05/06   wli     Rearranged perso enum items.
08/20/06   tml     Added App Reset and 3G Session Reset supports
08/07/06   tml     Added ACL support
08/01/06   tml     Added registration for FCN support
06/27/06   jar     Added missed act onchip req type.
06/12/06   tml     Lint and merge onchip sim support
06/02/06   tml     Added BDN support
05/11/06   nk      Removed function prototypes that were causing compiler warnings
04/06/06   sun     Added support for RUIM Lock
04/05/06   tml     lint
03/10/06   jar     Merged in Branched SIM Lock Support
01/04/06   sp      Added support for sap-pin_verify
12/22/05   sp      Fixed Compiler error
12/21/05   sp      Added support for Sap_Card_reader_Status
12/08/05   sp      Added support for Sap_Init, Sap_reset_sim and Sap_Deregister
11/30/05   jar     Support for OTA SIM Deperso
11/30/05   sp      Added support for force_sim_read.
11/17/05   tml     Added Delayed Response Run GSM Algorithm and Verify PIN
                   supports
09/29/05   tml     Client Deregistration support
08/28/05   tml     Added Read/Write/Get File Attributes with uint64 client id
08/25/05   pv      AT+CFUN API support to power down and power up the card.
07/26/05   sst     Added MMGSDIBT support
07/26/05   tml     ISIM Diag support
06/25/07   pv      moved definitions relating to mmgsdi
06/10/05   tml     256 supports and lint fixes
05/10/05   pv      Addeed Sub command to pipe commands to UIM
05/11/05   sst     Lint fixes
05/05/05   sun     Added new command for GSDI REFRESH
05/02/05   jar     Added #defines for SIM Lock Commands
05/01/05   jar     Added support for SIM Lock Commands
10/18/04   sk      Added support for gsdidiag_get_ecc request and
                   response.
10/18/04   sk      Added gsdidiag_sim_usim_ruim search cmd and
                   the request and response structures
10/05/04   sk      Added delayed response architecture for sim_read - added two
                   new structures - gsdidiag_sim_read_req_cmd_type and
                   gsdidiag_rsp_sim_read_payload_type.
09/07/04   jar     Added support for EF-PL under MF which resolves to EF_ELF
                   because they share the same FID.
09/01/04   sk      Added fixes to allow for building without
                   FEATURE_MMGSDI_CHANNEL_MANAGEMENT
08/30/04   sk      Added channel management supporting members to gsdidiag
                   request and response structures.
08/26/04   ss      Added delayed response architecture for generic
                   command and change pin
08/11/04   tml     Added support for ADN, GAS, SNE, EMAIL and GRP
08/11/04   tml     Added support for get unblock key
08/09/04   jar     Added support for HPLMNWACT for GSM
08/09/04   tml     Fixed EF mismatch
06/24/04   tml     Added personalization diag support
03/23/04   jar     Merged in Cheetah Changes.
03/17/04   jar     Merged in Amar's Fix for PIN Status.
01/29/04   tml     Added event notification to diag interface
12/02/03   tml     Added Select diag API
06/27/03   rd      Created first version.
===========================================================================*/
#include "uim.h" /* for uim enum defns*/
#include "gsdi_exp.h"
#include "gsdidiag_uim.h"
#include "mmgsdilib.h"
#include "diagpkt.h"

#ifdef FEATURE_MMGSDI_PERSONALIZATION_NON_QC
#include "simpers.h"


#define GSDI_MAX_SIM_SECURE_BUFFER  (SIMPERS_NV_ITEM_1_SIZE + \
                                     SIMPERS_NV_ITEM_2_SIZE + \
                                     SIMPERS_NV_ITEM_3_SIZE + \
                                     SIMPERS_NV_ITEM_4_SIZE )
#else
#define GSDI_MAX_SIM_SECURE_BUFFER  512
#endif /* FEATURE_MMGSDI_PERSONALIZATION_NON_QC */

#define MMGSDIDIAG_MAX_DATA_BLOCK_LEN 512
#define MMGSDIDIAG_MAX_PIN            34

#define MMGSDIDIAG_MAX_AUTH_DATA      100

#define GSDI_MAX_SW_LIB_VERSION_LEN 0x100
#define GSDI_MAX_KEY_ID_LEN         0x100

#define GSDIDIAG_APP_LABEL_LEN        32

#define GSDIDIAG_PIN1                 0x01
#define GSDIDIAG_PIN2                 0x02
#define GSDIDIAG_PIN_UNIVERSAL        0x03
#define GSDIDIAG_PIN_UNKNOWN          0xFF

#define GSDIDIAG_ENABLED_NOT_VERIFIED 0x01
#define GSDIDIAG_ENABLED_VERIFIED     0x02
#define GSDIDIAG_PIN_DISABLED         0x03
#define GSDIDIAG_PIN_BLOCKED          0x04
#define GSDIDIAG_PIN_PERM_BLOCKED     0x05
#define GSDIDIAG_PIN_STATUS_UNKNOWN   0xFF

#define GSDIDIAG_PIN_OP_VERIFY          0x01
#define GSDIDIAG_PIN_OP_ENABLE          0x02
#define GSDIDIAG_PIN_OP_DISABLE         0x03
#define GSDIDIAG_PIN_OP_CHANGE          0x04
#define GSDIDIAG_PIN_OP_UNBLOCK         0x05
#define GSDIDIAG_PIN_OP_DISABLE_REPLACE 0x06
#define GSDIDIAG_PIN_OP_UNKNOWN         0xFF


#define GSDIDIAG_NOT_REPLACED_BY_UNIV  0x00
#define GSDIDIAG_REPLACED_BY_UNIV      0x01

#define MMGSDIDIAG_PATH_LEN MMGSDI_MAX_PATH_LEN

#define GSDIDAG_NULL_CHAR_LEN          1
#define GSDIDIAG_SIM_FILENAME_OFFSET   22
/*===========================================================================
    Enumerated type:  gsdidiag_commands_enum_type
    List of available GSDI-DIAG Commands
===========================================================================*/
typedef enum
{
    /* GSDIDIAG_MIN_CMD =-1 */
    GSDIDIAG_SIM_READ_CMD                       = 0x00,
    GSDIDIAG_SIM_WRITE_CMD                      = 0x01,
    GSDIDIAG_SIM_INCREASE_CMD                   = 0x02,
    /* reserved */
    GSDIDIAG_GET_FILE_ATTRIBUTES_CMD            = 0x04,
    GSDIDIAG_GET_PIN_STATUS_CMD                 = 0x05,
    GSDIDIAG_VERIFY_PIN_CMD                     = 0x06,
    GSDIDIAG_CHANGE_PIN_CMD                     = 0x07,
    GSDIDIAG_UNBLOCK_PIN_CMD                    = 0x08,
    GSDIDIAG_DISABLE_PIN_CMD                    = 0x09,
    GSDIDIAG_ENABLE_PIN_CMD                     = 0x0A,
    /* reserved */
    GSDIDIAG_GET_SIM_CAPABILITIES_CMD           = 0x0C,
    /* reserved */
    /* reserved */
    GSDIDIAG_ILLEGAL_SIM_CMD                    = 0x0F,
    GSDIDIAG_SIM_SEARCH_CMD                     = 0x10,
    /* reserved */
    GSDIDIAG_SELECT_CMD                         = 0x12,
    /* reserved */
    GSDIDIAG_GET_IMAGE_CMD                      = 0x14,
    GSDIDIAG_SIM_REFRESH_CMD                    = 0x15,
    /* reserved */
    GSDIDIAG_GET_AVAILABLE_APPS_CMD             = 0x17,

    /* Note: GSDIDIAG_SEND_APDU_CMD has the same value as MMGSDIDIAG_REFRESH_REQ_CMD
       so it should not be moved into delayed response as it is, to avoid compilation
       issues */
    GSDIDIAG_SEND_APDU_CMD                      = 0x99,

    GSDIDIAG_ACTIVATE_FEATURE_INDICATOR_CMD     = 0x40,
    GSDIDIAG_DEACTIVATE_FEATURE_INDICATOR_CMD   = 0x41,
    GSDIDIAG_GET_FEATURE_INDICATION_CMD         = 0x42,
    GSDIDIAG_SET_FEATURE_INDICATOR_DATA_CMD     = 0x43,
    GSDIDIAG_GET_FEATURE_INDICATOR_KEY_CMD      = 0x44,
    GSDIDIAG_UNBLOCK_FEATURE_INDICATOR_CMD      = 0x45,
    GSDIDIAG_GET_PERM_FEATURE_INDICATION_CMD    = 0x46,
    GSDIDIAG_PERM_DISABLE_FEATURE_INDICATOR_CMD = 0x47,
    GSDIDIAG_GENERIC_CMD                        = 0x48,
    GSDIDIAG_GET_ATR_CMD                        = 0x49,
    GSDIDIAG_SIM_OPEN_CHANNEL_CMD               = 0x50,
    GSDIDIAG_SIM_CLOSE_CHANNEL_CMD              = 0x51,
    GSDIDIAG_SIM_SEND_CHANNEL_DATA_CMD          = 0x52,
    GSDIDIAG_ATCSIM_RAW_APDU_CMD                = 0x53,
    GSDIDIAG_SIM_USIM_RUIM_SEARCH_CMD           = 0x54,
    GSDIDIAG_GET_ECC_CMD                        = 0x55,
    GSDIDIAG_SECURE_DEVICE_INFO_CMD             = 0x56,
    GSDIDIAG_SIM_SECURE_STORE_CMD               = 0x57,
    GSDIDIAG_ROOT_KEY_WRITE_CMD                 = 0x58,
    GSDIDIAG_ROOT_KEY_READ_CMD                  = 0x59,
    GSDIDIAG_SIM_SECURE_READ_CMD                = 0x5A,
    GSDIDIAG_VERIFY_PHONE_CODE_CMD              = 0x5B,
    GSDIDIAG_CHANGE_PHONE_CODE_CMD              = 0x5C,
    GSDIDIAG_UNBLOCK_PHONE_CODE_CMD             = 0x5D,
    GSDIDIAG_SET_PHONE_CODE_CMD                 = 0x5E,
    GSDIDIAG_GET_LIB_VERSION_CMD                = 0x5F,
    GSDIDIAG_SIM_READ_X_CMD                     = 0x60,
    GSDIDIAG_ACTIVATE_ONCHIP_SIM_CMD            = 0x61,
    GSDIDIAG_WRITE_TRANSPARENT_CMD              = 0x62,
    GSDIDIAG_WRITE_RECORD_CMD                   = 0x63,
    GSDIDIAG_READ_TRANSPARENT_CMD               = 0x64,
    GSDIDIAG_READ_RECORD_CMD                    = 0x65,
    GSDIDIAG_GET_FILE_ATTR_CMD                  = 0x66,
    GSDIDIAG_UIM_CMD                            = 0x67,
    GSDIDIAG_ISIM_AUTHENTICATE_CMD              = 0x68,
    GSDIDIAG_SESSION_OPEN_CMD                   = 0x69,
    GSDIDIAG_SESSION_CLOSE_CMD                  = 0x6A,
    GSDIDIAG_CLIENT_ID_REG_CMD                  = 0x6B,
    GSDIDIAG_SAP_CONNECT_CMD                    = 0x6C,
    GSDIDIAG_SAP_DISCONNECT_CMD                 = 0x6D,
    GSDIDIAG_SAP_GET_ATR_CMD                    = 0x6E,
    GSDIDIAG_SAP_SEND_APDU_CMD                  = 0x6F,
    GSDIDIAG_SAP_POWER_OFF_CMD                  = 0x70,
    GSDIDIAG_SAP_POWER_ON_CMD                   = 0x71,
    GSDIDIAG_CARD_PDOWN_CMD                     = 0x72,
    GSDIDIAG_CARD_PUP_CMD                       = 0x73,
    GSDIDIAG_CARD_STATUS_CMD                    = 0x74,
    MMGSDIDIAG_WRITE_TRANSPARENT_CMD            = 0x75,
    MMGSDIDIAG_WRITE_RECORD_CMD                 = 0x76,
    MMGSDIDIAG_READ_TRANSPARENT_CMD             = 0x77,
    MMGSDIDIAG_READ_RECORD_CMD                  = 0x78,
    MMGSDIDIAG_GET_FILE_ATTR_CMD                = 0x79,
    GSDIDIAG_CLIENT_ID_DEREG_CMD                = 0x7A,
    GSDIDIAG_RUN_GSM_ALGO_CMD                   = 0x7B,
    GSDIDIAG_OTA_DEPERSO_CMD                    = 0x7C,
    /* reserved */
    GSDIDIAG_FORCE_SIM_READ_CMD                 = 0x7D,
    GSDIDIAG_SAP_DEREGISTER_CMD                 = 0x7E,
    GSDIDIAG_SAP_INIT_CMD                       = 0x7F,
    GSDIDIAG_SAP_RESET_SIM_CMD                  = 0x80,
    GSDIDIAG_SAP_CARD_READER_STATUS_CMD         = 0x81,
    GSDIDIAG_SAP_PIN_VERIFY_CMD                 = 0x82,
    MMGSDIDIAG_ACTIVATE_ONCHIP_SIM_CMD          = 0x83,
    GSDIDIAG_DISABLE_BDN_CMD                    = 0x84,
    GSDIDIAG_ENABLE_BDN_CMD                     = 0x85,
    GSDIDIAG_REFRESH_FCN_REG_CMD                = 0x86,
    GSDIDIAG_DISABLE_ACL_CMD                    = 0x87,
    GSDIDIAG_ENABLE_ACL_CMD                     = 0x88,
    GSDIDIAG_REFRESH_COMPLETE_CMD               = 0x89,
    MMGSDIDIAG_SELECT_AID_CMD                   = 0x8A,
    MMGSDIDIAG_VERIFY_PIN_CMD                   = 0x8B,
    MMGSDIDIAG_ENABLE_PIN_CMD                   = 0x8C,
    MMGSDIDIAG_DISABLE_PIN_CMD                  = 0x8D,
    MMGSDIDIAG_CHANGE_PIN_CMD                   = 0x8E,
    MMGSDIDIAG_UNBLOCK_PIN_CMD                  = 0x8F,
    MMGSDIDIAG_GET_ALL_PIN_STATUS_CMD           = 0x90,
    MMGSDIDIAG_WRITE_TRANSPARENT_EXT_CMD        = 0x91,
    MMGSDIDIAG_WRITE_RECORD_EXT_CMD             = 0x92,
    MMGSDIDIAG_READ_TRANSPARENT_EXT_CMD         = 0x93,
    MMGSDIDIAG_READ_RECORD_EXT_CMD              = 0x94,
    GSDIDIAG_CHECK_SERVICE_AVAILABLE_CMD        = 0x95,
    MMGSDIDIAG_DISABLE_PIN_EXT_CMD              = 0x96,
    MMGSDIDIAG_REGISTER_FOR_EVENT_CMD           = 0x97,
    MMGSDIDIAG_OK_TO_REFRESH_CMD                = 0x98,
    MMGSDIDIAG_REFRESH_REQ_CMD                  = 0x99,
    MMGSDIDIAG_UICC_SEARCH_REQ_CMD              = 0x9A,
    MMGSDIDIAG_ICC_SEARCH_REQ_CMD               = 0x9B,
    MMGSDIDIAG_CREATE_PKCS15_TABLE_CMD          = 0x9C,
    GSTKDIAG_GENERIC_CMD                        = 0x9D,
    GSDIDIAG_GET_DCK_NUM_RETRIES_CMD            = 0x9E,
    MMGSDIDIAG_REFRESH_REG_BY_PATH_CMD          = 0x9F,
    MMGSDIDIAG_REFRESH_DEREG_CMD                = 0xA0,
    MMGSDIDIAG_PERSO_ACT_FEATURE_REQ_CMD        = 0xA1,
    MMGSDIDIAG_PERSO_DEACT_FEATURE_REQ_CMD      = 0xA2,
    MMGSDIDIAG_PERSO_GET_FEATURE_IND_CMD        = 0xA3,
    MMGSDIDIAG_PERSO_OTA_DEPERSO_CMD            = 0xA4,
    MMGSDIDIAG_PERSO_SET_DATA_CMD               = 0xA5,
    MMGSDIDIAG_PERSO_GET_DATA_CMD               = 0xA6,
    MMGSDIDIAG_PERSO_PERM_DISABLE_CMD           = 0xA7,
    MMGSDIDIAG_PERSO_GET_DCK_RETRIES_CMD        = 0xA8,
    MMGSDIDIAG_PERSO_UNBLOCK_FEATURE_CMD        = 0xA9,
    MMGSDIDIAG_PERSO_GET_FEATURE_KEY_CMD        = 0xAA,
    GSDIDIAG_SIM_GET_CPHS_INFORMATION_CMD       = 0xAB,
    MMGSDIDIAG_ACTIVATE_ONCHIP_USIM_CMD         = 0xAC,
    MMGSDIDIAG_USIM_AUTHENTICATE_CMD            = 0xAD,
    GSDIDIAG_GET_FEATURE_INDICATOR_DATA_CMD     = 0xAE,
    GSDIDIAG_MAX_CMD                            = 0x7FFFFFFF
} gsdidiag_commands_enum_type;

/*===========================================================================
    Enumerated type:  gsdidiag_directory_enum_type
      Define MF, DF, ADFs Available through GSDI - DIAG Interface
===========================================================================*/

typedef enum
{
    GSDIDIAG_MF                                   = 0x00000001,
    GSDIDIAG_TELECOM_DF                           = 0x00000002,
    GSDIDIAG_GSM_DF                               = 0x00000003,
    GSDIDIAG_CDMA_DF                              = 0x00000004,
    GSDIDIAG_DCS_1800_DF                          = 0x00000005,
    GSDIDIAG_GRAPHICS_DF                          = 0x00000006,
    GSDIDIAG_SOLSA_DF                             = 0x00000007,
    GSDIDIAG_MEXE_EF                              = 0x00000008,
    GSDIDIAG_ADF                                  = 0x00000009,
    GSDIDIAG_MAX_DF
} gsdidiag_directory_enum_type;


/*===========================================================================
Enumerated type gsdidiag_index_enum_type

    Defines Index Modes Available for Commands.
    GSDIDIAG_NO_INDEX        :  Applicable to Binary/Transparent Files
    GSDIDIAG_INDEX_ABSOLUTE  :  Applicable to Linear Fixed / Cyclic Files
    GSDIDIAG_INDEX_NEXT      :  Applicable to Cyclic Files
    GSDIDIAG_INDEX_PREVIOUS  :  Applicable to Cyclic Files
===========================================================================*/

typedef enum
{
    GSDIDIAG_NO_INDEX       = 0x00000000,
    GSDIDIAG_INDEX_ABSOLUTE = 0x00000001,
    GSDIDIAG_INDEX_CURRENT  = 0x00000002,
    GSDIDIAG_INDEX_NEXT     = 0x00000003,
    GSDIDIAG_INDEX_PREVIOUS = 0x00000004,
    GSDIDIAG_INDEX_MAX_ENUM = 0x7FFFFFFF
    /* reserved */
} gsdidiag_index_enum_type;


typedef enum {
  GSDIDIAG_NO_SUCH_SRCH_MODE      = 0x00000000,   /* No such search mode */
  GSDIDIAG_SEARCH_SIMPLE          = 0x00000001,   /* Simple search */
  GSDIDIAG_SEARCH_ENHANCED        = 0x00000002,   /* Enhanced Search */
  GSDIDIAG_SEARCH_MODE_MAX_ENUM   = 0x7FFFFFFF
} gsdidiag_search_mode_enum_type;


typedef enum {
  GSDIDIAG_SEARCH_FORWARD   = 0x00000001,  /* Search forward */
  GSDIDIAG_SEARCH_BACKWARD  = 0x00000002,  /* Search backward */
  GSDIDIAG_SEARCH_DIR_MAX_ENUM = 0x7FFFFFFF
} gsdidiag_search_dir_type;

/*===========================================================================
Enumerated type gsdidiag_perso_feature_type

    Defines Personalization feature level
===========================================================================*/
typedef enum
{
    GSDIDIAG_PERSO_NW        = 0x00000000,
    GSDIDIAG_PERSO_NS        = 0x00000001,
    GSDIDIAG_PERSO_SP        = 0x00000002,
    GSDIDIAG_PERSO_CP        = 0x00000003,
    GSDIDIAG_PERSO_SIM       = 0x00000004,
    GSDIDIAG_PERSO_RUIM_NW1  = 0x0000000A,
    GSDIDIAG_PERSO_RUIM_NW2  = 0x0000000B,
    GSDIDIAG_PERSO_RUIM_SP   = 0x0000000C,
    GSDIDIAG_PERSO_RUIM_CP   = 0x0000000D,
    GSDIDIAG_PERSO_RUIM_RUIM = 0x0000000E,
    GSDIDIAG_PERSO_RUIM_HRPD = 0x0000000F,
    GSDIDIAG_PERSO_MAX_ENUM = 0x7FFFFFFF
} gsdidiag_perso_feature_type;

/*===========================================================================
Enumerated type gsdidiag_perso_feature_key_type

    Defines Personalization feature key level
===========================================================================*/
typedef enum
{
  GSDIDIAG_NW_KEY            = 0x00,
  GSDIDIAG_NS_KEY            = 0x01,
  GSDIDIAG_SP_KEY            = 0x02,
  GSDIDIAG_CP_KEY            = 0x03,
  GSDIDIAG_SIM_KEY           = 0x04,
  GSDIDIAG_NW_UNBLOCK_KEY    = 0x05,
  GSDIDIAG_NS_UNBLOCK_KEY    = 0x06,
  GSDIDIAG_SP_UNBLOCK_KEY    = 0x07,
  GSDIDIAG_CP_UNBLOCK_KEY    = 0x08,
  GSDIDIAG_SIM_UNBLOCK_KEY   = 0x09,
  GSDIDIAG_RUIM_NW1_KEY      = 0x0A,
  GSDIDIAG_RUIM_NW2_KEY      = 0x0B,
  GSDIDIAG_RUIM_SP_KEY       = 0x0C,
  GSDIDIAG_RUIM_CP_KEY       = 0x0D,
  GSDIDIAG_RUIM_RUIM_KEY     = 0x0E,
  GSDIDIAG_RUIM_HRPD_KEY     = 0x0F,
  GSDIDIAG_RUIM_NW1_UNBLOCK_KEY = 0x10,
  GSDIDIAG_RUIM_NW2_UNBLOCK_KEY = 0x11,
  GSDIDIAG_RUIM_SP_UNBLOCK_KEY  = 0x12,
  GSDIDIAG_RUIM_CP_UNBLOCK_KEY  = 0x13,
  GSDIDIAG_RUIM_RUIM_UNBLOCK_KEY= 0x14,
  GSDIDIAG_RUIM_HRPD_UNBLOCK_KEY= 0x15,
  GSDIDIAG_MAX_KEY           = 0xFF
} gsdidiag_perso_feature_key_type;

/*===========================================================================
Enumerated type: GSDIDIAG_ERROR_CODES
 These error codes are in addition to the errors returned by gsdi_exp.h.
 Errors specific to GSDIDIAG are in the range 500-1000.
============================================================================*/
#define GSDIDIAG_GET_FN_ERR       (-1)    /* this is an error code used to
                                         represent a get function failed */
#define GSDIDIAG_SUCCESS           0    /* same enumeration as GSDI_SUCCESS */
#define GSDIDIAG_BAD_PARAM         1
#define GSDIDIAG_ACCESS_DENIED     2
#define GSDIDIAG_NOT_SUPPORTED     9

/* GSDIDIAG return values for MMGSDI return type */
#define GSDIDIAG_MMGSDI_INCORRECT_PARAMS    601
#define GSDIDIAG_MMGSDI_CMD_QUEUE_FULL      602
#define GSDIDIAG_MMGSDI_ERROR               603
#define GSDIDIAG_MMGSDI_ACCESS_DENIED       604
#define GSDIDIAG_MMGSDI_NOT_FOUND           605
#define GSDIDIAG_MMGSDI_NOT_SUPPORTED       610
#define GSDIDIAG_MMGSDI_NOT_INIT            611
#define GSDIDIAG_MMGSDI_MAX_RETURN_STATUS   699

/* GSDIDIAG return values for GSDI return type */
#define GSDIDIAG_GSDI_ERROR                 701
#define GSDIDIAG_GSDI_ACCESS_DENIED         702
#define GSDIDIAG_GSDI_NOT_FOUND             703
#define GSDIDIAG_GSDI_INCOMPAT_PIN_STATUS   704
#define GSDIDIAG_GSDI_INCORRECT_CODE        705
#define GSDIDIAG_GSDI_CODE_BLOCKED          706
#define GSDIDIAG_GSDI_INCREASE_IMPOSSIBLE   707
#define GSDIDIAG_GSDI_INCORRECT_PARAMS      708
#define GSDIDIAG_GSDI_NOT_SUPPORTED         709
#define GSDIDIAG_GSDI_NOT_INIT              710
#define GSDIDIAG_GSDI_MAX_RETURN_STATUS     799

/* efs errors */
#define GSDIDIAG_EFS_ERR           500
#define GSDIDIAG_EFS_OPEN_ERR      501
#define GSDIDIAG_EFS_RM_ERR        502
#define GSDIDIAG_EFS_WRITE_ERR     503
#define GSDIDIAG_EFS_READ_ERR      504
#define GSDIDIAG_EFS_NAMETEST_ERR  505

    /* memory related errors */
#define GSDIDIAG_MEMORY_ERR                   550
#define GSDIDIAG_MAX_FILENAME_LEN_EXCEED_ERR  551
#define GSDIDIAG_MAX_ASYNC_REQ_EXCEED_ERR     552
#define GSDIDIAG_INCORRECT_RSP_ERR            553


typedef enum
{
    GSDIDIAG_SIM_INSERTED                 = 0x00000000,
    GSDIDIAG_SIM_REMOVED                  = 0x00000001,
    GSDIDIAG_UIM_DRIVER_ERROR             = 0x00000002,
    GSDIDIAG_CARD_ERROR                   = 0x00000003,
    GSDIDIAG_MEMORY_WARNING               = 0x00000004,
    GSDIDIAG_NO_SIM_EVENT                 = 0x00000005,
    GSDIDIAG_NO_SIM                       = 0x00000006,
    GSDIDIAG_SIM_INIT_COMPLETED           = 0x00000007,
    GSDIDIAG_SIM_INIT_COMPLETED_NO_PROV   = 0x00000008,
    GSDIDIAG_PIN1_VERIFIED                = 0x00000009,
    GSDIDIAG_PIN1_BLOCKED                 = 0x0000000A,
    GSDIDIAG_PIN1_UNBLOCKED               = 0x0000000B,
    GSDIDIAG_PIN1_ENABLED                 = 0x0000000C,
    GSDIDIAG_PIN1_DISABLED                = 0x0000000D,
    GSDIDIAG_PIN1_CHANGED                 = 0x0000000E,
    GSDIDIAG_PIN1_PERM_BLOCKED            = 0x0000000F,
    GSDIDIAG_PIN2_VERIFIED                = 0x00000010,
    GSDIDIAG_PIN2_BLOCKED                 = 0x00000011,
    GSDIDIAG_PIN2_UNBLOCKED               = 0x00000012,
    GSDIDIAG_PIN2_ENABLED                 = 0x00000013,
    GSDIDIAG_PIN2_DISABLED                = 0x00000014,
    GSDIDIAG_PIN2_CHANGED                 = 0x00000015,
    GSDIDIAG_PIN2_PERM_BLOCKED            = 0x00000016,
    GSDIDIAG_OK_FOR_TP_DL                 = 0x00000017,
    GSDIDIAG_NOT_OK_FOR_TP_DL             = 0x00000018,
    GSDIDIAG_REFRESH_SIM_RESET            = 0x00000019,
    GSDIDIAG_REFRESH_SIM_INIT             = 0x0000001A,
    GSDIDIAG_FDN_ENABLE                   = 0x0000001B,
    GSDIDIAG_FDN_DISABLE                  = 0x0000001C,

    GSDIDIAG_SIM_INSERTED_2               = 0x00000100,
    GSDIDIAG_SIM_REMOVED_2                = 0x00000101,
    GSDIDIAG_CARD_ERROR_2                 = 0x00000102,
    GSDIDIAG_MEMORY_WARNING_2             = 0x00000103,
    GSDIDIAG_NO_SIM_EVENT_2               = 0x00000104,
    GSDIDIAG_SIM_INIT_COMPLETED_2         = 0x00000105,
    GSDIDIAG_SIM_INIT_COMPLETED_NO_PROV_2 = 0x00000106,
    GSDIDIAG_PIN1_VERIFIED_2              = 0x00000107,
    GSDIDIAG_PIN1_BLOCKED_2               = 0x00000108,
    GSDIDIAG_PIN1_UNBLOCKED_2             = 0x00000109,
    GSDIDIAG_PIN1_ENABLED_2               = 0x0000010A,
    GSDIDIAG_PIN1_DISABLED_2              = 0x0000010B,
    GSDIDIAG_PIN1_CHANGED_2               = 0x0000010C,
    GSDIDIAG_PIN1_PERM_BLOCKED_2          = 0x0000010D,
    GSDIDIAG_PIN2_VERIFIED_2              = 0x0000010E,
    GSDIDIAG_PIN2_BLOCKED_2               = 0x0000010F,
    GSDIDIAG_PIN2_UNBLOCKED_2             = 0x00000110,
    GSDIDIAG_PIN2_ENABLED_2               = 0x00000111,
    GSDIDIAG_PIN2_DISABLED_2              = 0x00000112,
    GSDIDIAG_PIN2_CHANGED_2               = 0x00000113,
    GSDIDIAG_PIN2_PERM_BLOCKED_2          = 0x00000114,
    GSDIDIAG_OK_FOR_TP_DL_2               = 0x00000115,
    GSDIDIAG_NOT_OK_FOR_TP_DL_2           = 0x00000116,
    GSDIDIAG_REFRESH_SIM_RESET_2          = 0x00000117,
    GSDIDIAG_REFRESH_SIM_INIT_2           = 0x00000118,
    GSDIDIAG_FDN_ENABLE_2                 = 0x00000119,
    GSDIDIAG_FDN_DISABLE_2                = 0x0000011A,
    GSDIDIAG_NO_EVENT_TO_DIAG             = 0x7FFFFFFF

} gsdidiag_card_evt_enum_type;

/*===========================================================================
Enumerated type: mmgsdidiag_file_structure_type
 These are used to indicate the type of file structure
============================================================================*/
typedef enum
{
  MMGSDIDIAG_LINEAR_FIXED_FILE,
  MMGSDIDIAG_CYCLIC_FILE,
  MMGSDIDIAG_TRANSPARENT_FILE,
  MMGSDIDIAG_MAX_FILE_STRUCTURE_ENUM = 0x7FFFFFFF
} mmgsdidiag_file_structure_type;

/*===========================================================================
  ENUM:     GSDIDIAG_ICC_SEARCH_MODE_ENUM_TYPE

  DESCRIPTION: The following enumeration will indicate the search
  direction in a EF for icc card
    GSDIDIAG_ICC_SEARCH_BEGINNING_FORWARDS : Start from first record and move
                                             forward.
    GSDIDIAG_ICC_SEARCH_END_BACKWARDS      : Start from last record and move
                                             backward.
===========================================================================*/
typedef enum {
  GSDIDIAG_ICC_SEARCH_DIRECTION_NONE         = 0x00000000,
  GSDIDIAG_ICC_SEARCH_BEGINNING_FORWARDS,
  GSDIDIAG_ICC_SEARCH_END_BACKWARDS,
  GSDIDIAG_MAX_ICC_SEARCH_DIRECTION_ENUM     = 0x7FFFFFFF
}gsdidiag_icc_search_mode_enum_type;

/*===========================================================================
   ENUM: GSDIDIAG_UICC_SEARCH_ENUM_TYPE

   DESCRIPTION: The following enumeration will indicate the search type.
   GSDIDIAG_ICC_SEARCH Not Used
   GSDIDIAG_UICC_SIMPLE_SEARCH for simple search record on uicc card.
   GSDIDIAG_UICC_ENHANCED_SEARCH for enhanced search record on uicc card.
===========================================================================*/
typedef enum {
  GSDIDIAG_UICC_SEARCH_NONE           = 0x00000000,
  GSDIDIAG_ICC_SEARCH,
  GSDIDIAG_UICC_SIMPLE_SEARCH,
  GSDIDIAG_UICC_ENHANCED_SEARCH,
  GSDIDIAG_MAX_SEARCH_ENUM            = 0x7FFFFFFF
}gsdidiag_uicc_search_enum_type;

/*===========================================================================
  ENUM:      GSDIDIAG_UICC_SEARCH_OPTION_ENUM_TYPE

  DESCRIPTION: the following enumeration will indicate search type in a
  record for uicc card.
    GSDIDIAG_UICC_SEARCH_BY_OFFSET : Searching of the pattern in a record that
                                     start from given offset.
    GSDIDIAG_UICC_SEARCH_BY_CHAR   : Searching of the pattern in a record that
                                     starts with a character value(one byte
                                     information).
  ===========================================================================*/
typedef enum {
  GSDIDIAG_UICC_SEARCH_OPTION_NONE     = 0x00000000,
  GSDIDIAG_UICC_SEARCH_BY_OFFSET,
  GSDIDIAG_UICC_SEARCH_BY_CHAR,
  GSDIDIAG_MAX_UICC_SEARCH_OPTION_ENUM = 0x7FFFFFFF
}gsdidiag_uicc_search_option_enum_type;

/*===========================================================================
  STRUCTURE:      GSDIDIAG_UICC_SEARCH_OFFSET_DATA_TYPE

  DESCRIPTION:
    This structure is used for offset parameters for UICC Enhanced Search.

    offset_type: it indicates search option in a record.
                 Either contains the absolute record offset position or
                 contain a 1 byte character value.
    offset_data: 1 byte information.
                 For record offset its range is start from 0x00 to 0xFE.
                 For character value its range is start from 0x00 to 0xFF.
===========================================================================*/
typedef PACKED struct {
  gsdidiag_uicc_search_option_enum_type  offset_type;
  int32                                  offset_data;
} gsdidiag_uicc_search_offset_data_type;

/*===========================================================================
  ENUM:      GSDIDIAG_UICC_SEARCH_MODE_ENUM_TYPE

  DESCRIPTION: the following enumeration will indicate the search
  direction in a EF for UICC card.
    GSDIDIAG_UICC_SEARCH_FORWARDS_FROM_REC_NUM   : Start forward from given
                                                   record number.
    GSDIDIAG_UICC_SEARCH_BACKWARD_FROM_REC_NUM   : Start backward from give
                                                   record number.
  ===========================================================================*/
typedef enum {
  GSDIDIAG_UICC_SEARCH_DIRECTION_NONE           = 0x00000000,
  GSDIDIAG_UICC_SEARCH_FORWARDS_FROM_REC_NUM,
  GSDIDIAG_UICC_SEARCH_BACKWARD_FROM_REC_NUM,
  GSDIDIAG_MAX_UICC_SEARCH_DIRECTION_ENUM       = 0x7FFFFFFF
}gsdidiag_uicc_search_mode_enum_type;

/*=============================================================================
   Struct gsdidiag_cmd_header_type:
   Structure for the Command Headers.
   uint8 command_code    : Always set to 75
   uint8 subsys_id       : Always set to 33
   uint16 subsys_cmd_code: Set to code for the particular command of type
                           gsdiag_command_enum_type
=============================================================================*/
typedef PACKED struct
{
    uint8 command_code;
    diagpkt_subsys_id_type subsys_id;
    diagpkt_subsys_cmd_code_type subsys_cmd_code;
} gsdidiag_cmd_header_type;

/*=============================================================================
   Struct type  gsdidiag_event_payload_type:
   The structure for the event payload.
   uint32 status:          Status contains the error code from
                           error_codes_enum_type
   uint16 transaction_id:  transaction id. id relating event with
                           corresponding req/rsp
=============================================================================*/
typedef PACKED struct
{
    uint32 status;
    uint16 transaction_id;
} gsdidiag_event_payload_type;

/*=============================================================================
   Struct type  gsdidiag_event_value_payload_type:
   Used for events with an additional uint32 value in payload.
   uint32 status:          Status contains the error code from
                           error_codes_enum_type
   uint16 transaction_id:  transaction id. id relating event with
                           correspoding req-rsp
   uint8  value:           value to be sent in the event. This could be used for
                           sending out data in payload of the event
   uint8  value2:      value to be sent in the event payload, to represent
                 the number of retries
=============================================================================*/
typedef PACKED struct
{
    uint32 status;
    uint16 transaction_id;
    uint8  value;
    uint8  value2;
} gsdidiag_event_value_payload_type;

/*=============================================================================
   Struct type  gsdidiag_event_perso_value_payload_type:
   Used for events with an additional uint32 value in payload.
   uint32 status:          Status contains the error code from
                           error_codes_enum_type
   uint16 transaction_id:  transaction id. id relating event with
                           correspoding req-rsp
   uint8  value1:           value to be sent in the event. This could be used for
                           sending out data in payload of the event
   uint8  value2:          value to be sent in the event payload, to represent
                           the indication for NS
   uint8  value3:          value to be sent in the event payload, to represent
                           the indication for SP
   uint8  value4:          value to be sent in the event payload, to represent
                           the indication for CP
   uint8  value5:          value to be sent in the event payload, to represent
                           the indication for SIM/USIM
=============================================================================*/
typedef PACKED struct
{
    uint32 status;
    uint16 transaction_id;
    uint8  value1;
    uint8  value2;
    uint8  value3;
    uint8  value4;
    uint8  value5;
    uint8  value6;
    uint8  value7;
    uint8  value8;
    uint8  value9;
    uint8  value10;
    uint8  value11;
    boolean ruim_valid;
    boolean umts_valid;
} gsdidiag_event_perso_value_payload_type;

/*=============================================================================
   Struct type  mmgsdidiag_event_payload_type:
   The structure for the event payload of MMGSDI.
   mmgsdi_events_enum_type evt: Type of event occured
   uint8 event_payload:  Event payload
=============================================================================*/
typedef PACKED struct
{
    mmgsdi_events_enum_type evt;
    uint8 event_payload[12];
} mmgsdidiag_event_payload_type;

/*============================================================================
Struct   gsdidiag_two_pin_cmd_type
         uint8  sim_pin_id: Sim Pin ID (0x00 -> SIM PIN1, 0x01 -> SIM PIN2)
         char old_sim_pin[GSDI_MAX_PIN_LEN+1]: old sim pin
         char new_sim_pin[GSDI_MAX_PIN_LEN+1]: new sim pin

PURPOSE  Pin information for the generic command request packet.
============================================================================*/
typedef PACKED struct
{
    uint8  sim_pin_id;
    char old_sim_pin[GSDI_MAX_PIN_LEN+1];
    char new_sim_pin[GSDI_MAX_PIN_LEN+1];
} gsdidiag_two_pin_cmd_type;

/*============================================================================
Struct   gsdidiag_one_pin_cmd_type
         uint8  sim_pin_id: Sim Pin ID (0x00 -> SIM PIN1, 0x01 -> SIM PIN2)
         char sim_pin[GSDI_MAX_PIN_LEN+1]: sim pin

PURPOSE  Pin information for the generic command request packet.
============================================================================*/
typedef PACKED struct
{
    uint8  sim_pin_id;
    char sim_pin[GSDI_MAX_PIN_LEN+1];
} gsdidiag_one_pin_cmd_type;

/*=============================================================================
  Struct type  mmgsdidiag_path_type

  int32              path_len;
  uint16             path_buf;

  PURPOSE To hold the path info for a file.
=============================================================================*/
typedef PACKED struct {
  mmgsdi_len_type    path_len;                        /* Length of path */
  uint16             path_buf[MMGSDIDIAG_PATH_LEN];   /* path */
} mmgsdidiag_path_type;

/*=============================================================================
  Struct type  mmgsdidiag_access_type

  int32   access_method;
  int32       file_enum;
  mmgsdidiag_path_type

  PURPOSE Used for writing to a transparent file.
=============================================================================*/
typedef PACKED struct {
  mmgsdidiag_access_enum_type   access_method;  /* Indicated how the union will be interpreted */
  gsdidiag_elementary_file_enum_type       file_enum;      /* File enum to be accessed      */
  mmgsdidiag_path_type                     path;
} mmgsdidiag_access_type;

/*=============================================================================
  Struct type  mmgsdidiag_data_type

  int32              data_len;
  uint8            * data_ptr;

  PURPOSE Used for writing to a transparent file.
=============================================================================*/
typedef PACKED struct {
  mmgsdi_len_type    data_len;         /* Length of data                     */
  uint8            * data_ptr;         /* data                               */
} mmgsdidiag_data_type;

/*=============================================================================
  Struct type  gsdidiag_write_transparent_cmd_type

  uint64                    client_id;
  mmgsdidiag_access_type    access_method;
  int32                     file_enum;
  int32                     offset;
  mmgsdidiag_data_type      write_data
  int32                     client_ref_val;

  PURPOSE Used for writing to a transparent file.
=============================================================================*/
typedef PACKED struct
{
  uint64                    client_id;
  mmgsdidiag_access_type    access_method;
  int32                     offset;
  mmgsdidiag_data_type      write_data;
  int32                     client_ref_val;
}gsdidiag_write_transparent_cmd_type;

/*=============================================================================
  Struct type  gsdidiag_write_record_cmd_type

  uint64                         client_id;
  mmgsdidiag_access_type         access_method;
  mmgsdidiag_file_structure_type file_type;
  int32                          record_number;
  mmgsdidiag_data_type           write_data
  int32                          client_ref_val;

  PURPOSE  Used for writing to a linear/cyclic file.
=============================================================================*/
typedef PACKED struct
{
  uint64                         client_id;
  mmgsdidiag_access_type         access_method;
  mmgsdidiag_file_structure_type file_type;
  int32                          record_number;
  mmgsdidiag_data_type           write_data;
  int32                          client_ref_val;
}gsdidiag_write_record_cmd_type;

/*=============================================================================
  Struct type  gsdidiag_get_file_attr_cmd_type

  uint64   client_id;
  int32    access_method;
  int32    file_enum;
  int32                    client_ref_val;

  PURPOSE Used for getting file attributes.
=============================================================================*/
typedef PACKED struct
{
  uint64                  client_id;
  mmgsdidiag_access_type  access_method;
  int32                   client_ref_val;
}gsdidiag_get_file_attr_cmd_type;


/*=============================================================================
  Struct type  gsdidiag_client_id_reg_cmd_type

  int32                    client_ref_val;

  PURPOSE Used for getting client id.
=============================================================================*/
typedef PACKED struct
{
  int32                   client_ref_val;
}gsdidiag_client_id_reg_cmd_type;

/*=============================================================================
  Struct type  gsdidiag_client_id_dereg_cmd_type

  uint64                   client_id;
  int32                    client_ref_val;

  PURPOSE Used for deregistrating client id.
=============================================================================*/
typedef PACKED struct
{
  uint64                    client_id;
  int32                     client_ref_val;
}gsdidiag_client_id_dereg_cmd_type;

/*=============================================================================
  Struct type  gsdidiag_read_transparent_cmd_type

  uint64                    client_id;
  mmgsdidiag_access_type    access_method;
  int32                     offset;
  int32                     req_data_len;
  int32                     client_ref_val;

  PURPOSE Used for read from a transparent file
=============================================================================*/
typedef PACKED struct
{
  uint64                    client_id;
  mmgsdidiag_access_type    access_method;
  int32                     offset;
  int32                     req_data_len;
  int32                     client_ref_val;
}gsdidiag_read_transparent_cmd_type;

/*=============================================================================
  Struct type  gsdidiag_read_record_cmd_type

  uint64                    client_id;
  mmgsdidiag_access_type    access_method;
  int32                     record_number;
  int32                     req_data_len;
  int32                     client_ref_val;

  PURPOSE Used for read from a cyclic/linear fixed file.
=============================================================================*/
typedef PACKED struct
{
  uint64                    client_id;
  mmgsdidiag_access_type    access_method;
  int32                     record_number;
  int32                     req_data_len;
  int32                     client_ref_val;
}gsdidiag_read_record_cmd_type;


/*=============================================================================
  Struct type  gsdidiag_isim_auth_cmd_type

  uint64                  client_id;
  mmgsdidiag_data_type    auth;
  int32                   client_ref_val;

  PURPOSE Used for ISIM Authentication.
=============================================================================*/
typedef PACKED struct
{
  uint64                  client_id;
  mmgsdidiag_data_type    auth;
  int32                   client_ref_val;
}gsdidiag_isim_auth_cmd_type;

/*=============================================================================
  Struct type  gsdidiag_session_open_cmd_type

  uint64                   client_id;
  int32                    client_ref_val;

  PURPOSE Used for Opening a Session.
=============================================================================*/
typedef PACKED struct
{
  uint64                   client_id;
  int32                    client_ref_val;
}gsdidiag_session_open_cmd_type;

/*=============================================================================
  Struct type  gsdidiag_session_close_cmd_type

  uint64                   client_id;
  int32                    client_ref_val;

  PURPOSE Used for Closing a Session.
=============================================================================*/
typedef PACKED struct
{
  uint64                   client_id;
  int32                    client_ref_val;
}gsdidiag_session_close_cmd_type;

/*=============================================================================
  Struct type  gsdidiag_activate_feature_cmd_type

  uint32                   perso_feature;
  char                     perso_feature_key[GSDI_PERSO_MAX_CK+1];

  PURPOSE Used for activating feature indicator.
=============================================================================*/
typedef PACKED struct
{
  uint32 perso_feature;
  char   perso_feature_key[GSDI_PERSO_MAX_CK+1];
}gsdidiag_activate_feature_cmd_type;

/*=============================================================================
  Struct type  gsdidiag_deactivate_feature_cmd_type

  uint32                   perso_feature;
  char                     perso_feature_key[GSDI_PERSO_MAX_CK+1];

  PURPOSE Used for deactivating feature indicator.
=============================================================================*/
typedef PACKED struct
{
  uint32 perso_feature;
  char   perso_feature_key[GSDI_PERSO_MAX_CK+1];
}gsdidiag_deactivate_feature_cmd_type;

/*=============================================================================
  Struct type  gsdidiag_set_feature_data_cmd_type

  uint32                   perso_feature;
  uint32                   perso_feature_data_len;
  uint8                  * perso_feature_data_ptr;

  PURPOSE Used for setting feature indicator data.
=============================================================================*/
typedef PACKED struct
{
  uint32 perso_feature;
  uint32 perso_feature_data_len;
  uint8  *perso_feature_data_ptr;
}gsdidiag_set_feature_data_cmd_type;

/*=============================================================================
  Struct type  gsdidiag_get_feature_data_cmd_type

  uint32                   perso_feature --> feature id

  PURPOSE Used for getting feature indicator data.
=============================================================================*/
typedef PACKED struct
{
  uint32 perso_feature;
}gsdidiag_get_feature_data_cmd_type;

/*=============================================================================
  Struct type  gsdidiag_get_feature_key_cmd_type

  uint32                  perso_key;

  PURPOSE Used for getting feature indicator key.
=============================================================================*/
typedef PACKED struct
{
  uint32 perso_key;
}gsdidiag_get_feature_key_cmd_type;

/*=============================================================================
  Struct type  gsdidiag_unblock_feature_cmd_type

  uint32                   perso_feature;
  char                     perso_feature_key[GSDI_PERSO_MAX_CK+1];

  PURPOSE Used for unblocking feature indicator.
=============================================================================*/
typedef PACKED struct
{
  uint32 perso_feature;
  char   perso_feature_key[GSDI_PERSO_MAX_CK+1];
}gsdidiag_unblock_feature_cmd_type;

/*=============================================================================
  Struct type  gsdidiag_perm_disable_feature_cmd_type

  uint32                   perso_feature;
  char                     perso_feature_key[GSDI_PERSO_MAX_CK+1];

  PURPOSE Used for disabling feature indicator.
=============================================================================*/
typedef PACKED struct
{
  uint32 perso_feature;
  char   perso_feature_key[GSDI_PERSO_MAX_CK+1];
}gsdidiag_perm_disable_feature_cmd_type;

/*============================================================================
Struct   gsdidiag_subcmd_req_hdr_type
         uint32 aps_as_transport : Indicates whether or not GSDIDIAG Sends the
                                   Command Status as generated by GSDI in the
                                   Response Header or in the Response Data
         uint32 subcommand       : command code for gsdidiag command
         uint8  slot_id          : Slot number for the SIM
         uint8  sim_app_id_len   : Length of the Application ID from which
                                   to retrieve information
         uint8  sim_app_id[GSDI_MAX_APP_ID_LEN]: USIM application identifier

PURPOSE  Subcommand header for generic command request payload
============================================================================*/
typedef PACKED struct
{
    uint32 aps_as_transport;
    gsdidiag_commands_enum_type subcommand;
    uint8  slot_id;
    uint8  sim_app_id_len;
    uint8  sim_app_id[GSDI_MAX_APP_ID_LEN];
}gsdidiag_subcmd_req_hdr_type;

/*=============================================================================
   Struct type  gsdidiag_channel_req_cmd_type:
   Used for channel management functions.
   uint32 channel_id:            Channel Id contains the default channel id.
   uint32 requested_channel_id:  Requested channel id contains the channel id requested
                                 in the channel management function.
   uint32  apdu_len:             Length of the APDU sent in the command.
   byte*  apdu_buffer_p:         The APDU sent in the command.
=============================================================================*/

typedef PACKED struct
{
    int32               channel_id;
    int32               requested_channel_id;
    int32               apdu_len;
    byte*               apdu_buffer_p;
} gsdidiag_channel_req_cmd_type;

/*=============================================================================
  Struct type  gsdidiag_sim_read_req_cmd_type

  uim_items_enum_type  sim_filename,
  gsdi_index_modes_T   index_mode,
  int                  rec_num,
  char*                search_string,
  int                  search_string_len,
  byte*                data_p,
  int                  required_data_len,
  int                  data_offset,
  int*                 rec_num_accessed_p,

  PURPOSE Used for sim_read_req functions.
=============================================================================*/
typedef PACKED struct
{
  gsdidiag_elementary_file_enum_type  sim_filename;
  gsdidiag_index_enum_type            index_mode;
  uint32                  rec_num;
  uint32                  search_string_len;
  char*                   search_string;
  int32                   required_data_len;
  int32                   data_offset;
}gsdidiag_sim_read_req_cmd_type;


/*=============================================================================
  Struct type  gsdidiag_sim_refresh_req_cmd_type

  uint64                                client_id;
  int32                                 refresh_mode;
  uint8                                 notify_in_progress;
  uint32                                num_files;
  uint32                                data_len;
  uint8                                *file_path_ptr;

  PURPOSE Used for refresh
=============================================================================*/
typedef PACKED struct
{
  uint64                                client_id;
  int32                                 refresh_mode;
  uint8                                 notify_in_progress;
  uint32                                num_files;
  uint32                                data_len;
  uint8                                *file_path_ptr;
} gsdidiag_sim_refresh_req_cmd_type;

/*=============================================================================
  Struct type  gsdidiag_refresh_complete_req_cmd_type

  uint64                                client_id;
  int32                                 refresh_mode;
  uint8                                 pass_fail;
  uint32                                num_files;
  gsdidiag_elementary_file_enum_type  * file_list_ptr;

  PURPOSE Used for refresh Complete
=============================================================================*/
typedef PACKED struct
{
  uint64                                client_id;
  int32                                 refresh_mode;
  uint8                                 pass_fail;
  uint32                                num_files;
  gsdidiag_elementary_file_enum_type  * file_list_ptr;
} gsdidiag_refresh_complete_req_cmd_type;


/*=============================================================================
  Struct type  gsdidiag_sim_increase_cmd_req_type

  uint64   client_id;
  int32    access_method;
  int32    file_enum;

  PURPOSE Used for sim_increase command.
=============================================================================*/
typedef PACKED struct
{
  gsdidiag_elementary_file_enum_type  sim_filename;
  uint32                              increase_value;
}gsdidiag_sim_increase_cmd_req_type;

/*=============================================================================
  Struct type  gsdidiag_force_sim_read_req_cmd_type

  uim_items_enum_type  sim_filename,
  gsdi_index_modes_T   index_mode,
  int                  rec_num,
  int                  data_len,
  int                  data_offset,

  PURPOSE Used for force_sim_read_req functions.
=============================================================================*/
typedef PACKED struct
{
  gsdidiag_elementary_file_enum_type  sim_filename;
  gsdidiag_index_enum_type            index_mode;
  uint32                  rec_num;
  int32                   data_len;
  int32                   data_offset;
}gsdidiag_force_sim_read_req_cmd_type;

/*=============================================================================
  Struct type  gsdidiag_sim_usim_ruim_search

  uim_items_enum_type  sim_filename,
  gsdi_index_modes_T   index_mode,
  int                  search_record_num,
  uim_srch_mode        search_mode,
  uim_srch_dir_type    search_direction,
  int                  search_string_len,
  char                *search_string,
  uint32               client_ref,
  gsdi_async_cb        async_cb
  PURPOSE Used for sim_usim_ruim_all_search function.
=============================================================================*/
typedef PACKED struct
{
  gsdidiag_elementary_file_enum_type     sim_filename;
  gsdidiag_index_enum_type               index_mode;
  uint32                                 search_record_num;
  gsdidiag_search_mode_enum_type         search_mode;
  gsdidiag_search_dir_type               search_direction;
  uint32                                 search_string_len;
  char                                  *search_string;
}gsdidiag_sim_usim_ruim_search;

typedef PACKED struct
{
  uint32                len;
  uint8                *data_buffer_p;
}gsdidiag_sim_secure_store_cmd_type;


typedef PACKED struct
{
  uint32                len;
  uint8                *data_buffer_p;
}gsdidiag_root_key_write_req_cmd_type;


typedef PACKED struct
{
    int32                       phone_code_len;
    uint8                      *phone_code_ptr;
}gsdidiag_verify_phone_code_cmd_type;

typedef PACKED struct
{
    int32                       old_phone_code_len;
    uint8                      *old_phone_code_ptr;
    int32                       new_phone_code_len;
    uint8                      *new_phone_code_ptr;
}gsdidiag_change_phone_code_cmd_type;

typedef PACKED struct
{
    int32                       master_key_len;
    uint8                      *master_key_ptr;
    int32                       new_phone_code_len;
    uint8                      *new_phone_code_ptr;
}gsdidiag_unblock_phone_code_cmd_type;

typedef PACKED struct
{
    int32                       phone_code_len;
    uint8                      *phone_code_ptr;
}gsdidiag_set_phone_code_cmd_type;

#if (defined(FEATURE_BT) || defined(FEATURE_WM_BT)) && defined (FEATURE_BT_EXTPF_SAP)
#error code not present
#endif /* (FEATURE_BT || FEATURE_WM_BT) && FEATURE_BT_EXTPF_SAP */

#if (defined(FEATURE_BT) || defined(FEATURE_WM_BT)) && defined (FEATURE_BT_EXTPF_SAP)
#error code not present
#endif /* (FEATURE_BT || FEATURE_WM_BT) && FEATURE_BT_EXTPF_SAP */

/*=============================================================================
   Struct type  gsdidiag_card_pdown_req_cmd_type:
   Used for requesting a power down for the SIM/USIM/RUIM.
   int32 pdown_option:    Power down option as to just power down the CARD or
                          do a power down and notify GSDI about card removal
=============================================================================*/
typedef PACKED struct
{
  uint64                   client_id;
  int32                    pdown_option;
}gsdidiag_card_pdown_req_cmd_type;

/*=============================================================================
   Struct type  gsdidiag_card_pup_req_cmd_type:
   Used for requesting a power up for the SIM/USIM/RUIM.
   int32 pup_option:    Power up option as to power up and switch uim to
                        passivejust more or do a SIM - Reset REFRESH.
=============================================================================*/
typedef PACKED struct
{
  uint64                   client_id;
  int32                    pup_option;
}gsdidiag_card_pup_req_cmd_type;


/*=============================================================================
   Struct type  gsdi_diag_status_req_cmd_type:
   Used for requesting a SAP APDU.
   uint32  apdu_len:             Length of the APDU sent in the command.
   byte*  apdu_buffer_p:         The APDU sent in the command.
=============================================================================*/
typedef PACKED struct
{
  uint64  client_id;
  int32   app_status;
  int32   ret_data_struct;
} gsdi_diag_status_req_cmd_type;

/*=============================================================================
  Struct type  gsdidiag_run_gsm_algo_req_cmd_type

  int32                    rand_len;
  uint8*                   rand_ptr;

  PURPOSE Used for Sending Run GSM Algorithm.
=============================================================================*/
typedef PACKED struct
{
  int32         rand_len;
  uint8*        rand_ptr;
} gsdidiag_run_gsm_algo_req_cmd_type;

/*=============================================================================
  Struct type  gsdidiag_ota_deperso_req_cmd_type

  uint32                    len;
  uint8*                    data_buffer_p;

  PURPOSE Used for OTQ Depersonalization.
=============================================================================*/
typedef PACKED struct
{
  uint32                len;
  uint8                *data_buffer_p;
}gsdidiag_ota_deperso_req_cmd_type;

#ifdef FEATURE_MMGSDI_ONCHIP_SIM
#error code not present
#endif /* FEATURE_MMGSDI_ONCHIP_SIM */
/*=============================================================================
  Struct type  gsdidiag_bdn_cmd_type

  uint64                    client_id;

  PURPOSE Used for Disable or Enable BDN.
=============================================================================*/
typedef PACKED struct
{
  uint64                    client_id;
}gsdidiag_bdn_cmd_type;

/*=============================================================================
  Struct type  gsdidiag_acl_cmd_type

  uint64                    client_id;

  PURPOSE Used for Disable or Enable ACL.
=============================================================================*/
typedef PACKED struct
{
  uint64                    client_id;
}gsdidiag_acl_cmd_type;

/*=============================================================================
  Struct type  gsdidiag_get_sim_cap_cmd_type

  uint64                    client_id;

  PURPOSE Used for getting the SIM capabilities.
=============================================================================*/
typedef PACKED struct
{
  uint64                    client_id;
}gsdidiag_get_sim_cap_cmd_type;

/*=============================================================================
  Struct type  gsdidiag_select_aid_req_cmd_type

  uint64                    client_id;
  PURPOSE Used for Selecting AID.
=============================================================================*/
typedef PACKED struct
{
  uint64                              client_id;
}gsdidiag_select_aid_req_cmd_type;

/*=============================================================================
  Struct type  gsdidiag_mmgsdi_pin_op_req_cmd_type

  uint64                    client_id;
  uint32                    pin_id;
  mmgsdidiag_data_type      pin;
  mmgsdidiag_data_type      additional_pin;
  uint32                    replace_pin

  PURPOSE Used for MMGSDI PIN functions: Verify, Enable, Disable, Change, and
          Unblock.
=============================================================================*/
typedef PACKED struct
{
  uint64                              client_id;
  uint32                              pin_id;
  mmgsdidiag_data_type                pin;
  mmgsdidiag_data_type                additional_pin;
  uint32                              replace_pin;
}gsdidiag_mmgsdi_pin_op_req_cmd_type;

/*=============================================================================
  Struct type  gsdidiag_mmgsdi_get_all_pin_status_req_cmd_type

  uint64                    client_id;

  PURPOSE Used for MMGSDI GET ALL PIN STATUS function.
=============================================================================*/
typedef PACKED struct
{
  uint64                              client_id;
}gsdidiag_mmgsdi_get_all_pin_status_req_cmd_type;

/*=============================================================================
  Struct type  gsdidiag_refresh_fcn_reg_cmd_type

  uint64                                client_id;
  uint32                                num_file;
  gsdidiag_elementary_file_enum_type   *file_list_ptr;

  PURPOSE Used for Register FCN refresh list
=============================================================================*/
typedef PACKED struct
{
  uint64                              client_id;
  int32                               num_file;
  gsdidiag_elementary_file_enum_type *file_list_ptr;
} gsdidiag_refresh_fcn_reg_cmd_type;

/*============================================================================
PACKET   Request gsdidiag_service_check_req_cmd_type
         gsdidiag_cmd_header_type header;

ID       GSDIDIAG_CHECK_SERVICE_AVAILABLE

PURPOSE  This packet is used to find out if the service is available

RESPONSE gsdidiag_service_check_payload_type.
         Response contains the service available. This is a synchronous req.
============================================================================*/
typedef PACKED struct
{
  /* Note: The request pointer from DIag has the client ID as well
           but it is not being used since this is a GSDI API call*/

  int32                    service;
} gsdidiag_service_check_req_cmd_type;

/*=============================================================================
   Struct type  gsdidiag_service_check_payload_type:
   The structure for the service check payload.

    byte srv_bitmap: Bit Map of the service Available

=============================================================================*/
typedef PACKED struct
{
    byte   svr_bitmap;
} gsdidiag_service_check_payload_type;

/*=============================================================================
   Struct type  gsdidiag_client_evt_reg_req_type:
   The structure for the client event register request payload

    uint64 client_id: client Identifier

=============================================================================*/
typedef PACKED struct
{
    uint64 client_id;
}gsdidiag_client_evt_reg_req_type;

/*=============================================================================
   Struct type  gsdidiag_ok_to_refresh_req_type:
   The structure for ok to refresh request payload

    uint64 client_id: client Identifier

=============================================================================*/
typedef PACKED struct
{
    uint64 client_id;
}gsdidiag_ok_to_refresh_req_cmd_type;

/*=============================================================================
   Struct type  gsdidiag_register_for_refresh_req_cmd_type:
   The structure for register for refresh request payload

    uint64                             client_id      : client Identifier
    int32                              vote_for_init  : vote for init
    uint32                             path_len       : path len changed
    gsdidiag_elementary_file_enum_type *file_list_ptr : list of refresh files
                                                        (enums)
    uint8                              *file_path_ptr : list of refresh files
                                                        (file paths)
    uint32                             num_file_enums : To indicate number of
                                                        enum files used by
                                                        clients for refresh reg.
=============================================================================*/
typedef PACKED struct
{
  uint64                             client_id;
  int32                              vote_for_init;
  uint32                              path_len;
  gsdidiag_elementary_file_enum_type *file_list_ptr;
  uint8                              *file_path_ptr;
  uint32                             num_file_enums;
}gsdidiag_register_for_refresh_req_cmd_type;

/*=============================================================================
   Struct type  gsdidiag_deregister_for_refresh_req_cmd_type:
   The structure for deregister for refresh request payload

    uint64                             client_id      : client Identifier
    int32                              vote_for_init  : vote for init
    uint32                             path_len       : path len changed
    gsdidiag_elementary_file_enum_type *file_list_ptr : list of refresh files
                                                        (enums)
    uint8                              *file_path_ptr : list of refresh files
                                                        (file paths)
    uint32                             num_file_enums : To indicate number of
                                                        enum files used by
                                                        clients for refresh dereg.
=============================================================================*/
typedef PACKED struct
{
  uint64                              client_id;
  int32                               vote_for_init;
  uint32                              path_len;
  gsdidiag_elementary_file_enum_type  *file_list_ptr;
  uint8                               *file_path_ptr;
  uint32                              num_file_enums;
}gsdidiag_deregister_for_refresh_req_cmd_type;

/*=============================================================================
   Struct type  gsdidiag_search_cmd_type:
   The structure for the search payload.
     client_id              :
     access                 : File Information
     search_type            : Search Type for UICC Search (UICC Enhacned
                              or UICC Simple Search)
     rec_num                : Record number for UICC Search (Record number
                              form where search will start in a File.)
     uicc_search_direction  : Search direction for UICC Search
     icc_search_direction   : Search direction for ICC Search
     enhanced_search_offset : Record offset for UICC Enhanced Search (Record
                              offset from where search will start in a record.
     search_pattern         : Data to be search
     client_ref_val         : Client Referenc value
=============================================================================*/
typedef PACKED struct {
  uint64                              client_id;
  mmgsdidiag_access_type              access;
  gsdidiag_uicc_search_enum_type      search_type;
  int32                               rec_num;
  PACKED union
  {
    gsdidiag_uicc_search_mode_enum_type   uicc_search_direction;
    gsdidiag_icc_search_mode_enum_type    icc_search_direction;
  }search_direction;
  gsdidiag_uicc_search_offset_data_type  enhanced_search_offset;
  mmgsdidiag_data_type                   search_pattern;
  int32                                  client_ref_val;
} gsdidiag_search_req_cmd_type;

/*=============================================================================
   Struct type  gsdidiag_create_pkcs15_lookup_table_req_cmd_type:
   The structure for create pkcs15 lookup table req cmd payload

    uint64 client_id: client Identifier

=============================================================================*/
typedef PACKED struct
{
  uint64 client_id;
}gsdidiag_create_pkcs15_lookup_table_req_cmd_type;

/*============================================================================
Struct   gsdidiag_req_payload_type
         gsdidiag_subcmd_req_hdr_type : subcommand header for request payload
         request : Union structure of variable request payload type
PURPOSE  Payload for the gsdidiag_generic_cmd_req_type.
============================================================================*/
typedef PACKED struct
{
    gsdidiag_subcmd_req_hdr_type sub_cmd_hdr;
    PACKED union
    {
      gsdidiag_two_pin_cmd_type            change_pin;
      gsdidiag_one_pin_cmd_type            verify_pin;
      gsdidiag_channel_req_cmd_type        channel_req;
      gsdidiag_sim_read_req_cmd_type       sim_read_req;
      gsdidiag_force_sim_read_req_cmd_type force_sim_read_req;
      gsdidiag_sim_usim_ruim_search        sim_usim_ruim_search_req;
      gsdidiag_uim_cmd_req_type            uim_cmd_req;
      gsdidiag_sim_secure_store_cmd_type   sim_secure_store_req;
      gsdidiag_root_key_write_req_cmd_type root_key_write_req;
      gsdidiag_verify_phone_code_cmd_type  verify_phone_code_req;
      gsdidiag_change_phone_code_cmd_type  change_phone_code_req;
      gsdidiag_unblock_phone_code_cmd_type unblock_phone_code_req;
      gsdidiag_set_phone_code_cmd_type     set_phone_code_req;
      gsdidiag_sim_refresh_req_cmd_type    refresh_req;
      gsdidiag_refresh_complete_req_cmd_type refresh_complete_req;
      gsdidiag_read_transparent_cmd_type   read_transparent_req;
      gsdidiag_read_record_cmd_type        read_record_req;
      gsdidiag_write_transparent_cmd_type  write_transparent_req;
      gsdidiag_write_record_cmd_type       write_record_req;
      gsdidiag_get_file_attr_cmd_type      get_file_attr_req;
      gsdidiag_isim_auth_cmd_type          isim_auth_req;
      gsdidiag_session_open_cmd_type       session_open_req;
      gsdidiag_session_close_cmd_type      session_close_req;
      gsdidiag_client_id_reg_cmd_type      client_id_reg_req;
      gsdidiag_client_id_dereg_cmd_type    client_id_dereg_req;
#if (defined(FEATURE_BT) || defined(FEATURE_WM_BT)) && defined (FEATURE_BT_EXTPF_SAP)
#error code not present
#endif  /* (FEATURE_BT || FEATURE_WM_BT) && FEATURE_BT_EXTPF_SAP */
      gsdidiag_card_pdown_req_cmd_type     card_pdown_req;
      gsdidiag_card_pup_req_cmd_type       card_pup_req;
      gsdi_diag_status_req_cmd_type        status_req;
      gsdidiag_run_gsm_algo_req_cmd_type   run_gsm_algo_req;
      gsdidiag_ota_deperso_req_cmd_type    ota_req;
      gsdidiag_bdn_cmd_type                bdn_req;
      gsdidiag_acl_cmd_type                acl_req;
      gsdidiag_get_sim_cap_cmd_type        sim_cap_req;
      gsdidiag_refresh_fcn_reg_cmd_type    refresh_fcn_reg_req;
#ifdef FEATURE_MMGSDI_ONCHIP_SIM
#error code not present
#endif /* FEATURE_MMGSDI_ONCHIP_SIM */
      gsdidiag_select_aid_req_cmd_type     select_aid_req;
      gsdidiag_mmgsdi_pin_op_req_cmd_type  pin_op_req;
      gsdidiag_mmgsdi_get_all_pin_status_req_cmd_type  get_all_pin_status_req;
      gsdidiag_service_check_req_cmd_type  service_check_req;
      gsdidiag_client_evt_reg_req_type     client_evt_reg_req;
      gsdidiag_ok_to_refresh_req_cmd_type  ok_to_refresh_req;
      gsdidiag_register_for_refresh_req_cmd_type       refresh_reg_req;
      gsdidiag_deregister_for_refresh_req_cmd_type     refresh_dereg_req;
      gsdidiag_search_req_cmd_type         search_req;
      gsdidiag_sim_increase_cmd_req_type   sim_increase_req;
      gsdidiag_create_pkcs15_lookup_table_req_cmd_type create_pkcs15_lookup_table_req;
      gsdidiag_activate_feature_cmd_type       activate_feature_indicator_req;
      gsdidiag_deactivate_feature_cmd_type     deactivate_feature_indicator_req;
      gsdidiag_set_feature_data_cmd_type       set_feature_indicator_data_req;
      gsdidiag_get_feature_data_cmd_type       get_feature_indicator_data_req;
      gsdidiag_get_feature_key_cmd_type        get_feature_indicator_key_req;
      gsdidiag_unblock_feature_cmd_type        unblock_feature_indicator_req;
      gsdidiag_perm_disable_feature_cmd_type   perm_disable_feature_indicator_req;
    }request;
}gsdidiag_req_payload_type;

/*============================================================================
PACKET   Request gsdidiag_generic_cmd_req_type
    gsdidiag_cmd_header_type:   header for the request
    gsdidiag_req_payload_type:  payload for the request

ID       GSDIDIAG_GENERIC_CMD

PURPOSE  This packet is  the request for the generic command.

RESPONSE   gsdidiag_generic_cmd_rsp_type.
============================================================================*/
typedef PACKED struct
{
  gsdidiag_cmd_header_type          cmd_header;
  gsdidiag_req_payload_type         req_payload;
}gsdidiag_generic_cmd_req_type;

/*============================================================================
Struct   gsdidiag_rsp_header_type
         uint8  command_code    : message ID (The DM sets CMD_CODE to
                            128 for this message.)
         uint8  subsys_id       : subsystem identifier
         uint16 subsys_cmd_code : command code for given subsystem
         uint32 status          : command status
         uint16 delayed_rsp_id  : delayed response identifier
         uint16 rsp_count       : Response count (0-immediate; 1-delayed)
         uint32 aps_as_transport: Indicates whether or not GSDIDIAG Sends the
                                  Command Status as generated by GSDI in the
                                  Response Header or in the Response Data

PURPOSE  Header for the generic command response packet
============================================================================*/
typedef PACKED struct
{
  diagpkt_subsys_header_v2_type header;
  uint32                        aps_as_transport;
} gsdidiag_rsp_header_type;

/*============================================================================
Struct   gsdidiag_subcmd_rsp_hdr_type
         uint32 subcommand  : command code for gsdidiag command
         uint32 gsdi_status : status for gsdi command

PURPOSE  Subcommand header for the generic command response payload

REQUEST  gsdidiag_subcmd_req_hdr_type
============================================================================*/
typedef PACKED struct
{
   gsdidiag_commands_enum_type subcommand;
   PACKED union
   {
     mmgsdi_return_enum_type mmgsdi_status;
     gsdi_returns_T          gsdi_status;
   }st;
}gsdidiag_subcmd_rsp_hdr_type;



/*============================================================================
Struct   gsdidiag_rsp_data_payload_type
         uint32 len    : length of payload returned
         uint8  buffer : response buffer returned.

PURPOSE  Subcommand header for the generic command response payload

REQUEST  gsdidiag_rsp_data_payload_type
============================================================================*/
typedef PACKED struct
{
  uint32 len;
  uint8  buffer[GSDI_MAX_DATA_BLOCK_LEN * 2];
}gsdidiag_rsp_data_payload_type;

/*============================================================================
Struct   gsdidiag_rsp_channel_data_payload_type
         int32                          channel_id  : channel id returned
         gsdidiag_rsp_data_payload_type payload     : response payload

PURPOSE  Subcommand header for the generic command response payload

RESPONSE  gsdidiag_rsp_channel_data_payload_type
============================================================================*/
typedef PACKED struct
{
  int32                          channel_id;
  gsdidiag_rsp_data_payload_type payload;
}gsdidiag_rsp_channel_data_payload_type;

/*============================================================================
Struct   gsdidiag_rsp_atcsim_payload_type
         int32                          channel_id  : channel id returned
         uint16              implicit_get_rsp_sw1   : Status Word 1 for ATCSIM
         uint16              implicit_get_rsp_sw2   : Status word 2 for ATCSIM
         gsdidiag_rsp_data_payload_type payload     : response payload

PURPOSE  Subcommand header for the generic command response payload

RESPONSE  gsdidiag_rsp_atcsim_payload_type
============================================================================*/
typedef PACKED struct
{
  int32               channel_id;
  uint16              implicit_get_rsp_sw1;    /* Status Word 1 for ATCSIM */
  uint16              implicit_get_rsp_sw2;    /* Status word 2 for ATCSIM */
  gsdidiag_rsp_data_payload_type payload;
}gsdidiag_rsp_atcsim_payload_type;

/*============================================================================
Struct   gsdidiag_rsp_sim_read_payload_type
         word sim_filename                          : file name
         byte rec_num_accessed                      : record no accessed
         gsdidiag_rsp_data_payload_type payload     : response payload

PURPOSE  Subcommand header for the generic command response payload

RESPONSE  gsdidiag_rsp_sim_read_payload_type
============================================================================*/
typedef PACKED struct
{
  gsdidiag_elementary_file_enum_type sim_filename;
  byte                               rec_num_accessed;
  gsdidiag_rsp_data_payload_type     payload;
}gsdidiag_rsp_sim_read_payload_type;

/*============================================================================
Struct   gsdidiag_ecc_type
         byte emer_call_code[3];
         byte alpha_id[10];
         byte emer_srvc_category;

PURPOSE  Subcommand header for the generic command response payload

RESPONSE  gsdidiag_ecc_type
============================================================================*/
typedef PACKED struct
{
  byte emer_call_code[GSDI_ECC_LEN];
  byte alpha_id[GSDI_ALPHA_ID_LEN];
  byte emer_srvc_category;
}gsdidiag_ecc_type;

/*============================================================================
Struct   gsdidiag_rsp_sim_get_ecc_type
         int32         num_of_rec;
         ecc_info_T    ecc_p[];

PURPOSE  Subcommand header for the generic command response payload

RESPONSE  gsdidiag_rsp_sim_get_ecc_type
============================================================================*/
typedef PACKED struct
{
  int32                num_of_rec;
  gsdidiag_ecc_type    ecc_p[GSDI_MAX_ECC_RECORDS];
}gsdidiag_rsp_sim_get_ecc_type;

/*============================================================================
Struct   gsdidiag_rsp_write_transparent_payload_type
  mmgsdidiag_access_enum_type         access_method: mean of access
  gsdidiag_elementary_file_enum_type  file_enum:     file
  mmgsdidiag_path_type                path:          path
  int32                               len:           len written
PURPOSE  Subcommand header for the generic command response payload

RESPONSE  gsdidiag_rsp_sim_read_payload_type
============================================================================*/
typedef PACKED struct
{
  mmgsdidiag_access_enum_type         access_method;
  gsdidiag_elementary_file_enum_type  file_enum;
  mmgsdidiag_path_type                path;
  int32                               len;
}gsdidiag_rsp_write_transparent_payload_type;

/*============================================================================
Struct   gsdidiag_rsp_write_record_payload_type
  mmgsdidiag_access_enum_type         access_method:    mean of access
  gsdidiag_elementary_file_enum_type  file_enum:        file
  mmgsdidiag_path_type                path:             path
  int32                               rec_num_accessed: rec accessed
  int32                               len:              len written

PURPOSE  Subcommand header for the generic command response payload

RESPONSE  gsdidiag_rsp_sim_read_payload_type
============================================================================*/
typedef PACKED struct
{
  mmgsdidiag_access_enum_type        access_method;
  gsdidiag_elementary_file_enum_type file_enum;
  mmgsdidiag_path_type               path;
  int32                              rec_num_accessed;
  int32                              len;
}gsdidiag_rsp_write_record_payload_type;

/*============================================================================
Struct   gsdidiag_rsp_read_transparent_payload_type
  mmgsdidiag_access_enum_type         access_method:    mean of access
  gsdidiag_elementary_file_enum_type  file_enum:        file
  mmgsdidiag_path_type                path:             path
  gsdidiag_rsp_data_payload_type      payload:          data read

PURPOSE  Subcommand header for the generic command response payload

RESPONSE  gsdidiag_rsp_sim_read_payload_type
============================================================================*/
typedef PACKED struct
{
  mmgsdidiag_access_enum_type        access_method;
  gsdidiag_elementary_file_enum_type file_enum;
  mmgsdidiag_path_type               path;
  gsdidiag_rsp_data_payload_type     payload;
}gsdidiag_rsp_read_transparent_payload_type;

/*============================================================================
Struct   gsdidiag_rsp_read_record_payload_type
  mmgsdidiag_access_enum_type         access_method:    mean of access
  gsdidiag_elementary_file_enum_type  file_enum:        file
  mmgsdidiag_path_type                path:             path
  int32                               rec_num_accessed: rec accessed
  gsdidiag_rsp_data_payload_type      payload:          data read

PURPOSE  Subcommand header for the generic command response payload

RESPONSE  gsdidiag_rsp_sim_read_payload_type
============================================================================*/
typedef PACKED struct
{
  mmgsdidiag_access_enum_type        access_method;
  gsdidiag_elementary_file_enum_type file_enum;
  mmgsdidiag_path_type               path;
  int32                              rec_num_accessed;
  gsdidiag_rsp_data_payload_type     payload;
}gsdidiag_rsp_read_record_payload_type;

/*============================================================================
Struct   gsdidiag_rsp_get_file_attr_payload_type
         word sim_filename                          : file name
         byte rec_num_accessed                      : record no accessed
         gsdidiag_rsp_data_payload_type payload     : response payload

PURPOSE  Subcommand header for the generic command response payload

RESPONSE  gsdidiag_rsp_sim_read_payload_type
============================================================================*/
typedef PACKED struct
{
  mmgsdidiag_access_enum_type access_method;
  gsdidiag_elementary_file_enum_type file_enum;
  int32 file_size;
  int32 file_id;
  uint32 file_type;
  int32 rec_len;
  int32 no_of_rec;
  int32 cf_increase_allowed;
  int32 file_invalid_flg;
  int32 rw_allowed_when_invalid_flg;
  int32 read_cond;
  int32 read_no_of_pin_req;
  int32 read_pin_info[MMGSDIDIAG_MAX_PIN];
  int32 write_cond;
  int32 write_no_of_pin_req;
  int32 write_pin_info[MMGSDIDIAG_MAX_PIN];
  int32 increase_cond;
  int32 increase_no_of_pin_req;
  int32 increase_pin_info[MMGSDIDIAG_MAX_PIN];
  int32 validate_cond;
  int32 validate_no_of_pin_req;
  int32 validate_pin_info[MMGSDIDIAG_MAX_PIN];
  int32 invalidate_cond;
  int32 invalidate_no_on_pin_req;
  int32 invalidate_pin_info[MMGSDIDIAG_MAX_PIN];
}gsdidiag_rsp_get_file_attr_payload_type;


/*============================================================================
Struct   gsdidiag_rsp_isim_auth_payload_type
         word sim_filename                          : file name
         byte rec_num_accessed                      : record no accessed
         gsdidiag_rsp_data_payload_type payload     : response payload

PURPOSE  Subcommand header for the generic command response payload

RESPONSE  gsdidiag_rsp_isim_auth_payload_type
============================================================================*/
typedef PACKED struct
{
  int32 res_len;
  uint8 res[MMGSDIDIAG_MAX_AUTH_DATA];
  int32 ck_len;
  uint8 ck[MMGSDIDIAG_MAX_AUTH_DATA];
  int32 ik_len;
  uint8 ik[MMGSDIDIAG_MAX_AUTH_DATA];
  int32 auts_len;
  uint8 auts[MMGSDIDIAG_MAX_AUTH_DATA];
}gsdidiag_rsp_isim_auth_payload_type;


/*============================================================================
Struct   gsdidiag_rsp_session_open_payload_type
         uint32 session_id_high     upper 32 bits of session ID
         uint32 session_id_low      lower 32 bits of session ID

PURPOSE  Subcommand header for the generic command response payload

RESPONSE  gsdidiag_rsp_session_open_payload_type
============================================================================*/
typedef PACKED struct
{
  uint32 session_id_high;
  uint32 session_id_low;
}gsdidiag_rsp_session_open_payload_type;

/*============================================================================
Struct   gsdidiag_rsp_session_close_payload_type
         uint32 session_id_high     upper 32 bits of session ID
         uint32 session_id_low      lower 32 bits of session ID

PURPOSE  Subcommand header for the generic command response payload

RESPONSE  gsdidiag_rsp_session_close_payload_type
============================================================================*/
typedef PACKED struct
{
  uint32 session_id_high;
  uint32 session_id_low;
}gsdidiag_rsp_session_close_payload_type;

/*============================================================================
Struct   gsdidiag_rsp_activate_feature_payload_type
         uint32 perso_feature     perso feature
         uint32 num_retries       number of retries

PURPOSE  Subcommand header for the generic command response payload

RESPONSE  gsdidiag_rsp_activate_feature_payload_type
============================================================================*/
typedef PACKED struct
{
  uint32 perso_feature;
  uint32 num_retries;
}gsdidiag_rsp_activate_feature_payload_type;

/*============================================================================
Struct   gsdidiag_rsp_deactivate_feature_payload_type
         uint32 perso_feature     perso feature
         uint32 num_retries       number of retries

PURPOSE  Subcommand header for the generic command response payload

RESPONSE  gsdidiag_rsp_deactivate_feature_payload_type
============================================================================*/
typedef PACKED struct
{
  uint32 perso_feature;
  uint32 num_retries;
}gsdidiag_rsp_deactivate_feature_payload_type;

/*============================================================================
Struct   gsdidiag_rsp_get_feature_payload_type
         uint8 umts_locks_present   umts locks present
         uint8 cdma_locks_present   cdma locks present
         uint8 nw_feature           umts nw feature enabled
         uint8 ns_feature           umts ns feature enabled
         uint8 sp_feature           umts sp feature enabled
         uint8 cp_feature           umts cp feature enabled
         uint8 sim_feature          umts sim feature enabled
         uint8 ruim_nw1_feature     cdma nw1 feature enabled
         uint8 ruim_nw2_feature     cdma nw2 feature enabled
         uint8 ruim_sp_feature      cdma sp feature enabled
         uint8 ruim_cp_feature      cdma cp feature enabled
         uint8 ruim_ruim_feature    cdma ruim feature enabled
         uint8 ruim_hrpd_feature    cdma hrpd feature enabled

PURPOSE  Subcommand header for the generic command response payload

RESPONSE  gsdidiag_rsp_get_feature_payload_type
============================================================================*/
typedef PACKED struct
{
  uint8 umts_locks_present;
  uint8 cdma_locks_present;
  uint8 sim_nw_feature;
  uint8 sim_ns_feature;
  uint8 sim_sp_feature;
  uint8 sim_cp_feature;
  uint8 sim_sim_feature;
  uint8 ruim_nw1_feature;
  uint8 ruim_nw2_feature;
  uint8 ruim_sp_feature;
  uint8 ruim_cp_feature;
  uint8 ruim_ruim_feature;
  uint8 ruim_hrpd_feature;
}gsdidiag_rsp_get_feature_payload_type;

/*============================================================================
Struct   gsdidiag_rsp_get_feature_key_payload_type
         int32 imei_len           length of imei
         uint8 imei               imei value
         int32 key_len            length of key
         uint8 key                key value

PURPOSE  Subcommand header for the generic command response payload

RESPONSE  gsdidiag_rsp_get_feature_key_payload_type
============================================================================*/
typedef PACKED struct
{
  int32 imei_len;
  uint8 imei[GSDI_PERSO_MAX_IMEI_LEN + 1];
  int32 key_len;
  uint8 key[GSDI_PERSO_MAX_CK + 1];
}gsdidiag_rsp_get_feature_key_payload_type;

/*============================================================================
Struct   gsdidiag_rsp_unblock_feature_payload_type
         uint32 perso_feature     perso feature
         uint32 num_retries       number of retries

PURPOSE  Subcommand header for the generic command response payload

RESPONSE  gsdidiag_rsp_unblock_feature_payload_type
============================================================================*/
typedef PACKED struct
{
  uint32 perso_feature;
  uint32 num_retries;
}gsdidiag_rsp_unblock_feature_payload_type;

/*============================================================================
Struct   gsdidiag_rsp_get_perm_feature_payload_type
         uint8 umts_locks_present   umts locks present
         uint8 cdma_locks_present   cdma locks present
         uint8 nw_feature           umts nw feature enabled
         uint8 ns_feature           umts ns feature enabled
         uint8 sp_feature           umts sp feature enabled
         uint8 cp_feature           umts cp feature enabled
         uint8 sim_feature          umts sim feature enabled
         uint8 ruim_nw1_feature     cdma nw1 feature enabled
         uint8 ruim_nw2_feature     cdma nw2 feature enabled
         uint8 ruim_sp_feature      cdma sp feature enabled
         uint8 ruim_cp_feature      cdma cp feature enabled
         uint8 ruim_ruim_feature    cdma ruim feature enabled
         uint8 ruim_hrpd_feature    cdma hrpd feature enabled

PURPOSE  Subcommand header for the generic command response payload

RESPONSE  gsdidiag_rsp_get_perm_feature_payload_type
============================================================================*/
typedef PACKED struct
{
  uint8 umts_locks_present;
  uint8 cdma_locks_present;
  uint8 sim_nw_feature;
  uint8 sim_ns_feature;
  uint8 sim_sp_feature;
  uint8 sim_cp_feature;
  uint8 sim_sim_feature;
  uint8 ruim_nw1_feature;
  uint8 ruim_nw2_feature;
  uint8 ruim_sp_feature;
  uint8 ruim_cp_feature;
  uint8 ruim_ruim_feature;
  uint8 ruim_hrpd_feature;
}gsdidiag_rsp_get_perm_feature_payload_type;

/*============================================================================
Struct   gsdidiag_rsp_perm_disable_feature_payload_type
         uint32 perso_feature     perso feature
         uint32 num_retries       number of retries

PURPOSE  Subcommand header for the generic command response payload

RESPONSE  gsdidiag_rsp_perm_disable_feature_payload_type
============================================================================*/
typedef PACKED struct
{
  uint32 perso_feature;
  uint32 num_retries;
}gsdidiag_rsp_perm_disable_feature_payload_type;


/*============================================================================
Struct   gsdidiag_rsp_get_feature_data_payload_type
          uint32 feature_data_len      : Length of feature data
          uint8  feature_data          : Feature data buffer

PURPOSE  Payload for get feature data response

RESPONSE  gsdidiag_rsp_get_feature_data_payload_type
============================================================================*/
typedef PACKED struct
{
  uint32 feature_data_len;
  uint8 feature_data[GSDI_PERSO_MAX_DATA_LEN];
}gsdidiag_rsp_get_feature_data_payload_type;


/*============================================================================
Struct   gsdidiag_rsp_client_id_reg_payload_type
         uint32 client_id_high   upper 32 bits of client ID
         uint32 client_id_low    lower 32 bits of client ID

PURPOSE  Subcommand header for the generic command response payload

RESPONSE  gsdidiag_rsp_client_id_reg_payload_type
============================================================================*/
typedef PACKED struct
{
  uint32 client_id_high;
  uint32 client_id_low;
}gsdidiag_rsp_client_id_reg_payload_type;

/*============================================================================
Struct   gsdidiag_rsp_client_id_dereg_payload_type
         uint32 client_id_high   upper 32 bits of client ID
         uint32 client_id_low    lower 32 bits of client ID

PURPOSE  Subcommand header for the generic command response payload

RESPONSE  gsdidiag_rsp_client_id_dereg_payload_type
============================================================================*/
typedef PACKED struct
{
  uint32 client_id_high;
  uint32 client_id_low;
}gsdidiag_rsp_client_id_dereg_payload_type;

/*============================================================================
Struct   gsdidiag_root_key_rsp_type
         int32         num_of_rec;
         ecc_info_T    ecc_p[];

PURPOSE  Subcommand header for the generic command response payload

RESPONSE  gsdidiag_root_key_rsp_type
============================================================================*/
typedef PACKED struct
{
  int32                key_len;
  uint8                key[GSDI_MAX_SIM_SECURE_BUFFER];
}gsdidiag_root_key_rsp_type;

/*============================================================================
Struct   gsdidiag_sim_secure_read_rsp_type
         int32                 data_len;
         uint8                 data[GSDI_MAX_SIM_SECURE_BUFFER];

PURPOSE  Subcommand header for the generic command response payload

RESPONSE  gsdidiag_sim_secure_read_rsp_type
============================================================================*/
typedef PACKED struct
{
  int32                 data_len;
  uint8                 data[GSDI_MAX_SIM_SECURE_BUFFER];
}gsdidiag_sim_secure_read_rsp_type;

/*============================================================================
Struct   gsdidiag_get_lib_version_rsp_type
         int32                 lib_version_len;
         uint8                 lib_version[GSDI_MAX_SW_LIB_VERSION_LEN];

PURPOSE  Subcommand header for the generic command response payload

RESPONSE  gsdidiag_get_lib_version_rsp_type
============================================================================*/
typedef PACKED struct
{
  int32                 lib_version_len;
  uint8                 lib_version[GSDI_MAX_SW_LIB_VERSION_LEN];
}gsdidiag_get_lib_version_rsp_type;

/*============================================================================
Struct   gsdidiag_ota_deperso_rsp_type
         int32                 data_len;
         uint8                 data[GSDI_MAX_SIM_SECURE_BUFFER];

PURPOSE  Subcommand header for the generic command response payload

RESPONSE  gsdidiag_ota_deperso_rsp_type
============================================================================*/
typedef PACKED struct
{
  int32                 data_len;
  uint8                 data[GSDI_MAX_SIM_SECURE_BUFFER];
}gsdidiag_ota_deperso_rsp_type;

/*============================================================================
Struct   gsdidiag_get_sim_cap_rsp_type
         FDN, BDN, ACL enabled or not
         IMSI invalidated or not
         PB capabilities: SIM/USIM global/USIM local present or not,
                          multi-entries or not
         General SIM capability unknown or not

PURPOSE  Subcommand header for the generic command response payload

RESPONSE  gsdidiag_get_sim_cap_rsp_type
============================================================================*/
typedef PACKED struct
{
  boolean               fdn_enabled;
  boolean               bdn_enabled;
  boolean               acl_enabled;
  boolean               imsi_invalidated;
  boolean               sim_pb_present;
  boolean               sim_pb_mult_entries;
  boolean               sim_pb_status_unknown;
  boolean               usim_pb_global_present;
  boolean               usim_pb_global_mult_entries;
  boolean               usim_pb_global_status_unknown;
  boolean               usim_pb_local_present;
  boolean               usim_pb_local_mult_entries;
  boolean               usim_pb_local_status_unknown;
  boolean               unknown_cap;
}gsdidiag_get_sim_cap_rsp_type;

#if (defined(FEATURE_BT) || defined(FEATURE_WM_BT)) && defined (FEATURE_BT_EXTPF_SAP)
#error code not present
#endif  /* (FEATURE_BT || FEATURE_WM_BT) && FEATURE_BT_EXTPF_SAP */

#if (defined(FEATURE_BT) || defined(FEATURE_WM_BT)) && defined (FEATURE_BT_EXTPF_SAP)
#error code not present
#endif  /* (FEATURE_BT || FEATURE_WM_BT) && FEATURE_BT_EXTPF_SAP */

#if (defined(FEATURE_BT) || defined(FEATURE_WM_BT)) && defined (FEATURE_BT_EXTPF_SAP)
#error code not present
#endif  /* (FEATURE_BT || FEATURE_WM_BT) && FEATURE_BT_EXTPF_SAP */

/*============================================================================
Struct   gsdidiag_rsp_card_pdown_payload_type
         int32   pdown_option:     Power Down Option

PURPOSE  To hold the payload info for power down
============================================================================*/
typedef PACKED struct
{
  int32 pdown_option;
}gsdidiag_rsp_card_pdown_payload_type;

/*============================================================================
Struct   gsdidiag_rsp_card_pup_payload_type
         int32   pup_option:   Power up Option

PURPOSE  To hold the payload info for power up
============================================================================*/
typedef PACKED struct
{
  int32 pup_option;
}gsdidiag_rsp_card_pup_payload_type;

/*============================================================================
Struct   gsdidiag_rsp_sim_read_payload_type
         word sim_filename                          : file name
         byte rec_num_accessed                      : record no accessed
         gsdidiag_rsp_data_payload_type payload     : response payload

PURPOSE  Subcommand header for the generic command response payload

RESPONSE  gsdidiag_rsp_sim_read_payload_type
============================================================================*/
typedef PACKED struct
{
  gsdidiag_rsp_data_payload_type     payload;
}gsdidiag_rsp_status_payload_type;

typedef PACKED struct
{
  gsdidiag_rsp_data_payload_type     payload;
}gsdidiag_rsp_run_gsm_algo_payload_type;


#ifdef FEATURE_MMGSDI_ONCHIP_SIM
#error code not present
#endif /* FEATURE_MMGSDI_ONCHIP_SIM */

/* ===========================================================================
Struct    gsdidiag_rsp_select_aid_payload_type
          uint8[] : app ID
PURPOSE   Payload for the gsdidiag_select_aid_req_cmd_type
============================================================================*/
typedef PACKED struct
{
  int32     data_len;
  uint8     data_ptr[GSDIDIAG_APP_LABEL_LEN];
}
gsdidiag_rsp_select_aid_payload_type;

/* ===========================================================================
Struct    gsdidiag_rsp_pin_op_payload_type
          Return response with PIN ID, PIN Status, Retries and PIN Operation
PURPOSE   Payload for the gsdidiag_mmgsdi_pin_op_req_cmd_type
============================================================================*/
typedef PACKED struct
{
  uint32     pin_id;
  uint8      pin_status;
  uint32     num_retries;
  uint32     num_unblock_retries;
  uint8      pin_op;
}gsdidiag_rsp_pin_op_payload_type;

/* ===========================================================================
Struct    gsdidiag_rsp_get_all_pin_status_payload_type
          Return response with PIN Status, Retries for all the pins
PURPOSE   Payload for the gsdidiag_mmgsdi_get_all_pin_status_req_cmd_type
============================================================================*/
typedef PACKED struct
{
  uint8      pin1_status;
  uint32     pin1_num_retries;
  uint32     pin1_num_unblock_retries;
  uint8      pin2_status;
  uint32     pin2_num_retries;
  uint32     pin2_num_unblock_retries;
  uint8      pin_univ_status;
  uint32     pin_univ_num_retries;
  uint32     pin_univ_num_unblock_retries;
}gsdidiag_rsp_get_all_pin_status_payload_type;

/*============================================================================
Struct   gsdidiag_rsp_search_payload_type

PURPOSE  Subcommand header for the search command response payload
         access_method : EF Access or Path Access
         file_enum     : File Name
         path          : Path of File
         payload       : Response Data contains list of record number
============================================================================*/
typedef PACKED struct
{
  mmgsdidiag_access_enum_type        access_method;
  gsdidiag_elementary_file_enum_type file_enum;
  mmgsdidiag_path_type               path;
  gsdidiag_rsp_data_payload_type     payload;
} gsdidiag_rsp_search_payload_type;

/* ----------------------------------------------------------------------------
   STRUCTURE: MMGSDIDIAG_PKCS15_LOOKUP_TABLE_ENTRIES
   DESCRIPTION:
   The mmgsdidiag_pkcs15_lookup_table consists of two categories
   1. EF Enum – List of PKCS15 EFs whose path is extracted from EF-ODF
   2. File Path Info – File path info of the PKCS15 EF
-------------------------------------------------------------------------------*/
typedef PACKED struct {
  gsdidiag_elementary_file_enum_type  pkcs15_file_enum;
  mmgsdidiag_path_type                pkcs15_file_path;
} mmgsdidiag_pkcs15_lookup_table_entries;

/* ----------------------------------------------------------------------------
   STRUCTURE: MMGSDIDIAG_PKCS15_LOOKUP_TABLE
   DESCRIPTION:
   The mmgsdidiag_pkcs15_lookup_table consists of
   1. EF Enum – List of PKCS15 EFs whose path is extracted from EF-ODF
   2. File Path Info – File path info of the PKCS15 EF
-------------------------------------------------------------------------------*/
typedef PACKED struct {
  uint8                                   no_of_pkcs15_lookup_table_entries;
  mmgsdidiag_pkcs15_lookup_table_entries  pkcs15_lookup_table [MMGSDI_MAX_PKCS15_TABLE_ENTRIES];
} mmgsdidiag_pkcs15_lookup_table;

/* ===========================================================================
Struct    gsdidiag_create_pkcs15_table_payload_type
          Return response with pkcs15 lookup table
PURPOSE   Payload for the gsdidiag_create_pkcs15_lookup_table_req_cmd_type
============================================================================*/
typedef PACKED struct
{
  mmgsdidiag_pkcs15_lookup_table pkcs_table;
}gsdidiag_create_pkcs15_table_payload_type;

/* ===========================================================================
Struct    gsdidiag_rsp_sim_increase_payload_type
          byte new_value_len
          byte data[GSDI_MAX_DATA_BLOCK_LEN]
PURPOSE   Payload for the gsdidiag_rsp_sim_increase_payload_type
============================================================================*/
typedef PACKED struct
{
  uint8            new_value_len;                /* new increased value len */
  uint8            data[GSDI_MAX_DATA_BLOCK_LEN];/* new increased value data*/
}
gsdidiag_rsp_sim_increase_payload_type;

/* ===========================================================================
Struct    gsdidiag_rsp_get_dck_num_retries_payload_type
PURPOSE   Payload for the gsdidiag_rsp_get_dck_num_retries_payload_type
============================================================================*/
typedef PACKED struct
{
  int32            nw_num_retries;
  int32            ns_num_retries;
  int32            sp_num_retries;
  int32            cp_num_retries;
  int32            sim_num_retries;
} gsdidiag_rsp_dck_num_retries_payload_type;

/*============================================================================
Struct   gsdidiag_rsp_sim_get_cphs_info_payload_type
         uint8 capable                             cphs capable
         uint8 phase                               cphs phase
         uint8 csp                                 status of csp
         uint8 sst                                 status of sst
         uint8 mbn                                 status of mbn
         uint8 ops                                 status of ops
         uint8 ifn                                 status of ifn
         uint8 csp_call_offering_cfu               csp call offering CFU
         uint8 csp_call_offering_cfb               csp call offering CFB
         uint8 csp_call_offering_cfnry             csp call offering CFNRy
         uint8 csp_call_offering_cfnrc             csp call offering CFNRc
         uint8 csp_call_offering_ct                csp call offering CT
         uint8 csp_call_restrict_boac              csp call restrict BOAC
         uint8 csp_call_restrict_boic              csp call restrict BOIC
         uint8 csp_call_restrict_boic_exhc         csp call restrictBOIC exHC
         uint8 csp_call_restrict_baic              csp call restrict BAIC
         uint8 csp_call_restrict_bic_roam          csp call restrict BIC roam
         uint8 csp_other_supps_mpty                csp other supps MPTY
         uint8 csp_other_supps_cug                 csp other supps CUG
         uint8 csp_other_supps_aoc                 csp other supps AOC
         uint8 csp_other_supps_pref_cug            csp other supps pref CUG
         uint8 csp_other_supps_cug_oa              csp other supps CUG OA
         uint8 csp_call_compl_hold                 csp call compl hold
         uint8 csp_call_compl_cw                   csp call compl CW
         uint8 csp_call_compl_ccbs                 csp call compl CCBS
         uint8 csp_call_compl_user_user_sig        csp call compl user signaling
         uint8 csp_teleservices_sm_mt              csp teleservices SM/MT
         uint8 csp_teleservices_sm_mo              csp teleservices SM/MO
         uint8 csp_teleservices_sm_cb              csp teleservices SM/CB
         uint8 csp_teleservices_reply_path         csp teleservices Reply path
         uint8 csp_teleservices_del_conf           csp teleservices del conf
         uint8 csp_teleservices_protocol_id        csp teleservices protocol id
         uint8 csp_teleservices_validity_period    csp teleservices validity period
         uint8 csp_cphs_teleservices_als           csp teleservices ALS
         uint8 csp_cphs_features_sst               csp features SST
         uint8 csp_number_identify_clip            csp number identify CLIP
         uint8 csp_number_identify_colr            csp number identify COLR
         uint8 csp_number_identify_colp            csp number identify COLP
         uint8 csp_number_identify_mci             csp number identify MCI
         uint8 csp_number_identify_cli_send        csp number identify CLI send
         uint8 csp_number_identify_cli_block       csp number identify CLI block
         uint8 csp_p2plus_srv_gprs                 csp phase2+ service GPRS
         uint8 csp_p2plus_srv_hscsd                csp phase2+ service HSCSD
         uint8 csp_p2plus_srv_voice_grp_call       csp phase2+ service voice group call
         uint8 csp_p2plus_srv_voice_bcast_svc      csp phase2+ service voice bcast
         uint8 csp_p2plus_srv_muli_sub_prof        csp phase2+ service GPRS muli sub prof
         uint8 csp_p2plus_srv_multiband            csp phase2+ service GPRS multiband
         uint8 csp_va_plmn_mode                    csp VA plmn mode
         uint8 csp_va_vps                          csp VA VPS
         uint8 csp_va_sm_mo_paging                 csp VA SM/MO paging
         uint8 csp_va_sm_mo_email                  csp VA SM/MO email
         uint8 csp_va_fax                          csp VA FAX
         uint8 csp_va_data                         csp VA data
         uint8 csp_va_language                     csp VA language
         uint8 csp_information_numbers             csp information numbers

PURPOSE  Subcommand header for the generic command response payload

RESPONSE  gsdidiag_rsp_sim_get_cphs_info_payload_type
============================================================================*/
typedef PACKED struct
{
  uint8 capable;
  uint8 phase;
  uint8 csp;
  uint8 sst;
  uint8 mbn;
  uint8 ops;
  uint8 ifn;
  uint8 csp_call_offering_cfu;
  uint8 csp_call_offering_cfb;
  uint8 csp_call_offering_cfnry;
  uint8 csp_call_offering_cfnrc;
  uint8 csp_call_offering_ct;
  uint8 csp_call_restrict_boac;
  uint8 csp_call_restrict_boic;
  uint8 csp_call_restrict_boic_exhc;
  uint8 csp_call_restrict_baic;
  uint8 csp_call_restrict_bic_roam;
  uint8 csp_other_supps_mpty;
  uint8 csp_other_supps_cug;
  uint8 csp_other_supps_aoc;
  uint8 csp_other_supps_pref_cug;
  uint8 csp_other_supps_cug_oa;
  uint8 csp_call_compl_hold;
  uint8 csp_call_compl_cw;
  uint8 csp_call_compl_ccbs;
  uint8 csp_call_compl_user_user_sig;
  uint8 csp_teleservices_sm_mt;
  uint8 csp_teleservices_sm_mo;
  uint8 csp_teleservices_sm_cb;
  uint8 csp_teleservices_reply_path;
  uint8 csp_teleservices_del_conf;
  uint8 csp_teleservices_protocol_id;
  uint8 csp_teleservices_validity_period;
  uint8 csp_cphs_teleservices_als;
  uint8 csp_cphs_features_sst;
  uint8 csp_number_identify_clip;
  uint8 csp_number_identify_colr;
  uint8 csp_number_identify_colp;
  uint8 csp_number_identify_mci;
  uint8 csp_number_identify_cli_send;
  uint8 csp_number_identify_cli_block;
  uint8 csp_p2plus_srv_gprs;
  uint8 csp_p2plus_srv_hscsd;
  uint8 csp_p2plus_srv_voice_grp_call;
  uint8 csp_p2plus_srv_voice_bcast_svc;
  uint8 csp_p2plus_srv_muli_sub_prof;
  uint8 csp_p2plus_srv_multiband;
  uint8 csp_va_plmn_mode;
  uint8 csp_va_vps;
  uint8 csp_va_sm_mo_paging;
  uint8 csp_va_sm_mo_email;
  uint8 csp_va_fax;
  uint8 csp_va_data;
  uint8 csp_va_language;
  uint8 csp_information_numbers;
}gsdidiag_rsp_sim_get_cphs_info_payload_type;

/*============================================================================
Struct   gsdidiag_rsp_payload_type
    gsdidiag_subcmd_rsp_hdr_type: subcommand header for response payload
         uint8 num_retries             : length of data to follow

PURPOSE  Payload for the gsdidiag_generic_cmd_rsp_type.
============================================================================*/
typedef PACKED struct
{
    gsdidiag_subcmd_rsp_hdr_type sub_cmd_hdr;
    PACKED union
    {
       gsdidiag_rsp_data_payload_type               data;
       gsdidiag_rsp_channel_data_payload_type       channel_data;
       gsdidiag_rsp_atcsim_payload_type             atcsim_data;
       gsdidiag_rsp_sim_read_payload_type           sim_read_data;
       gsdidiag_rsp_sim_get_ecc_type                ecc_data;
       gsdidiag_rsp_uim_cmd_type                    uim_data;
       uint8                                        num_retries;
       byte                                         rec_num_accessed;
       gsdidiag_root_key_rsp_type                   root_key_write;
       gsdidiag_root_key_rsp_type                   root_key_read;
       gsdidiag_sim_secure_read_rsp_type            sim_secure_read;
       gsdidiag_get_lib_version_rsp_type            lib_version;

       gsdidiag_rsp_write_transparent_payload_type  write_transparent_data;
       gsdidiag_rsp_write_record_payload_type       write_record_data;
       gsdidiag_rsp_read_transparent_payload_type   read_transparent_data;
       gsdidiag_rsp_read_record_payload_type        read_record_data;
       gsdidiag_rsp_get_file_attr_payload_type      get_file_attr_data;
       gsdidiag_rsp_isim_auth_payload_type          isim_auth;
       gsdidiag_rsp_session_open_payload_type       session_open;
       gsdidiag_rsp_session_close_payload_type      session_close;
       gsdidiag_rsp_client_id_reg_payload_type      client_id_reg_data;
       gsdidiag_rsp_client_id_dereg_payload_type    client_id_dereg_data;

#if (defined(FEATURE_BT) || defined(FEATURE_WM_BT)) && defined (FEATURE_BT_EXTPF_SAP)
#error code not present
#endif /* (FEATURE_BT || FEATURE_WM_BT) && FEATURE_BT_EXTPF_SAP */
       gsdidiag_rsp_card_pdown_payload_type         card_pdown;
       gsdidiag_rsp_card_pup_payload_type           card_pup;
       gsdidiag_rsp_status_payload_type             status_data;
       gsdidiag_rsp_run_gsm_algo_payload_type       run_gsm_algo;
       gsdidiag_ota_deperso_rsp_type                ota_data;
       gsdidiag_get_sim_cap_rsp_type                sim_cap_data;
#ifdef FEATURE_MMGSDI_ONCHIP_SIM
#error code not present
#endif /* FEATURE_MMGSDI_ONCHIP_SIM */
       gsdidiag_rsp_select_aid_payload_type         select_aid_data;
       gsdidiag_rsp_pin_op_payload_type             pin_op_data;
       gsdidiag_rsp_get_all_pin_status_payload_type get_all_pin_status_data;
       gsdidiag_service_check_payload_type          srv_available;
       gsdidiag_rsp_search_payload_type             search_data;
       gsdidiag_create_pkcs15_table_payload_type    create_pkcs15_table_data;
       gsdidiag_rsp_sim_increase_payload_type       sim_increase;
       gsdidiag_rsp_dck_num_retries_payload_type    dck_num_retries;
       gsdidiag_rsp_activate_feature_payload_type      activate_feature_indicator;
       gsdidiag_rsp_deactivate_feature_payload_type    deactivate_feature_indicator;
       gsdidiag_rsp_get_feature_payload_type           get_feature_indication;
       gsdidiag_rsp_get_feature_key_payload_type       get_feature_indicator_key;
       gsdidiag_rsp_unblock_feature_payload_type       unblock_feature_indicator;
       gsdidiag_rsp_get_perm_feature_payload_type      get_perm_feature_indication;
       gsdidiag_rsp_perm_disable_feature_payload_type  perm_disable_feature_indicator;
       gsdidiag_rsp_get_feature_data_payload_type      get_feature_data;
       gsdidiag_rsp_sim_get_cphs_info_payload_type  get_cphs_info;
    }response;
}gsdidiag_rsp_payload_type;



/*============================================================================
PACKET   Request gsdidiag_generic_cmd_rsp_type
    gsdidiag_rsp_header_type:   header for the response
    gsdidiag_rsp_payload_type:  payload for the response

ID       GSDIDIAG_GENERIC_CMD

PURPOSE  This packet is as the response for the generic command.

REQUEST   gsdidiag_generic_cmd_req_type.
============================================================================*/
typedef PACKED struct
{
    gsdidiag_rsp_header_type  rsp_header;
    gsdidiag_rsp_payload_type rsp_payload;
}gsdidiag_generic_cmd_rsp_type;


/*============================================================================
PACKET   Request gsdidiag_sim_get_file_attributes
         uint8 sim_slot_number  :   Specifies to which slot this command goes to
         uint8 sim_app_id_len   :   Specifies the length of the optional APP ID
         uint8 sim_add_id       :   Buffer of max 16 bytes for an APP ID
         uint32 sim_elementary_file:Filename for which the attributes are
                                    needed.
         char efs_filename[1]   :   File on EFS, where this information is
                                    to be stored.

ID       GSDIDIAG_GET_FILE_ATTRIBUTES

PURPOSE  This packet is used to retrieve the physical characteristics of the
         Elementary File (EF), Directory File (DF), Master File (MF), or
         Application Directory File (ADF).

RESPONSE gsdidiag_sim_get_file_attributes_cmd_rsp_type
         Response contains the status, and transaction id.
         Future event EVENT_GSDI_GET_FILE_ATTRIBUTES
         will represent that the file attrbites are stored in an efs file.
============================================================================*/
typedef PACKED struct
{
    gsdidiag_cmd_header_type header;
    uint8  sim_slot_number;
    uint8  sim_app_id_len;
    uint8  sim_app_id[GSDI_MAX_APP_ID_LEN];
    gsdidiag_elementary_file_enum_type sim_elementary_file;
    char   efs_filename[1];
} gsdidiag_sim_get_file_attributes_cmd_req_type;

/*============================================================================
PACKET   Response gsdidiag_sim_get_file_attributes
         gsdidiag_cmd_header_type header: Header
         uint32 status           : Status of the request
         uint8 sim_slot_number   : Specifies to which slot this command goes to
         uint8 sim_app_id_len    : Specifies the length of the optional APP ID
         uint8 sim_add_id        : Buffer of max 16 bytes for an APP ID
         uint32 sim_elementary_file:  Filename for which the attributes are
                                      needed.
         char efs_filename[1]    : Filename for which the file attributes is to be
                                   stored in.

ID       GSDIDIAG_GET_FILE_ATTRIBUTES

PURPOSE  This packet is used to retrieve the physical characteristics of the
         Elementary File (EF), Directory File (DF), Master File (MF), or
         Application Directory File (ADF).


REQUEST  gsdidiag_sim_get_file_attributes_cmd_req_type
         Future event EVENT_GSDI_GET_FILE_ATTRIBUTES
         will represent that the file attributes are stored in an efs file.
============================================================================*/
typedef PACKED struct
{
    gsdidiag_cmd_header_type header;
    uint32 status;
    uint16 transaction_id;
    uint8  sim_slot_number;
    uint8  sim_app_id_len;
    uint8  sim_app_id[GSDI_MAX_APP_ID_LEN];
    gsdidiag_elementary_file_enum_type sim_elementary_file;
    char efs_filename[1];

} gsdidiag_sim_get_file_attributes_cmd_rsp_type;

/*============================================================================

PACKET   gsdidiag_sim_read_cmd_req_type
    gsdidiag_cmd_header_type header: Header
    uint8  sim_slot_number         : Specifies to which sim slot this command goes to
    uint8  sim_app_id_len          : Specifies the length of the optional APP ID
    uint8  sim_app_id[GSDI_MAX_APP_ID_LEN]          : Buffer of max 16 bytes for an APP ID
    uint32 sim_elementary_file     : File to be read
    uint8  simple_req              : Represents if read parameters  are user
                                     specified values, or the phone calculates
                                     appropriate values. Also represents user
                                     has to deal with chunking.
                                     0: Use the user specified values  for
                                        SIM_INDEX_MODE, SIM_OFFSET,SIM_REC_NUM.
                                        User has to deal with chunking.
                                     1: Phone uses appropriate values. Also
                                        returns the complete file.
                                        User is transparent to chunking.

    uint8  sim_index_mode          : Mode in which read is to be executed
    uint16 sim_offset              : Offset in the filename
    uint16 sim_rec_num             : Record number to read
    uint16 sim_data_len            : Length of data to read
    char   efs_filename[1]         : File on EFs to store read data to

ID      GSDIDIAG_SIM_READ_CMD

PURPOSE  Read sim data

RESPONSE gsdidiag_sim_read_cmd_rsp_type.
         Rsp contains status and transaction_id. Response indicates that request
         has been queued. Future event EVENT_GSDI_SIM_READ will indicate that
         the required information has been written to an efs file.

============================================================================*/
typedef PACKED struct
{
    gsdidiag_cmd_header_type header;
    uint8  sim_slot_number;
    uint8  sim_app_id_len;
    uint8  sim_app_id[GSDI_MAX_APP_ID_LEN];
    gsdidiag_elementary_file_enum_type sim_elementary_file;
    uint8  simple_req;
    uint8  sim_index_mode;
    uint16 sim_offset;
    uint16 sim_rec_num;
    uint16 sim_data_len;
    char   efs_filename[1];
} gsdidiag_sim_read_cmd_req_type;

/*============================================================================
PACKET   gsdidiag_sim_read_cmd_rsp_type
    uint32 status          : Status of the request
    uint16 transaction_id  : Specifies to which slot this command goes to
    uint8  sim_slot_number : Specifies to which slot this command goes to
    uint8  sim_app_id_len  : Specifies the length of the optional APP ID
    uint8  sim_app_id[GSDI_MAX_APP_ID_LEN]  : Buffer of max 16 bytes for an APP ID
    uint32 sim_elementary_file: File to be read
    uint8  simple_req      : Represents if read parameters  are user
                            specified values, or the phone calculates
                            appropriate values. Also represents user
                            has to deal with chunking.
                            0: Use the user specified values  for
                               SIM_INDEX_MODE, SIM_OFFSET,SIM_REC_NUM.
                               User has to deal with chunking.
                            1: Phone uses appropriate values. Also
                               returns the complete file.
                               User is transparent to chunking.

    uint8  sim_index_mode  : Mode in which read is to be executed
    uint16 sim_offset      : Offset in the file, to start the read from
    uint16 sim_rec_num     : Record number to read
    uint16 sim_data_len    : Length of data to read
    char   efs_filename[1] : File on EFS to store read data to

ID      GSDIDIAG_SIM_READ_CMD

PURPOSE  Read sim data

REQUEST: gsdidiag_sim_read_cmd_req_type.
         Future event EVENT_GSDI_SIM_READ will indicate that
         the required information has been written to an efs file.

============================================================================*/
typedef PACKED struct
{
    gsdidiag_cmd_header_type header;
    uint32 status;
    uint16 transaction_id;
    uint8  sim_slot_number;
    uint8  sim_app_id_len;
    uint8  sim_app_id[GSDI_MAX_APP_ID_LEN];
    gsdidiag_elementary_file_enum_type sim_elementary_file;
    uint8  simple_req;
    uint8  sim_index_mode;
    uint16 sim_offset;
    uint16 sim_rec_num;
    uint16 sim_data_len;
    char   efs_filename[1];
} gsdidiag_sim_read_cmd_rsp_type;

/*============================================================================
PACKET   Request gsdidiag_sim_write_cmd_req_type
         uint8 sim_slot_number:  Specifies slot that the command goes to.
         uint8 sim_app_id_len:   Specifies the length of the optional APP ID
         uint8 sim_app_id:       Buffer of max 16 bytes for an APP ID
         uint32 sim_elementary_file:  Filename for which the attributes are
                                  needed.
         char efs_filename[1]:  Filename from which the write data is got from

ID       GSDIDIAG_SIM_WRITE_CMD

PURPOSE  This packet is used to write data into SIM.


RESPONSE gsdidiag_sim_write_cmd_rsp_type
         Response contains the status, and transaction id.
         Response indicates that request has been queued.
         Future event EVENT_GSDI_GET_FILE_ATTRIBUTES
         will represent that the file attrbites are stored in an efs file.
============================================================================*/
typedef PACKED struct
{
    gsdidiag_cmd_header_type header;
    uint8  sim_slot_number;
    uint8  sim_app_id_len;
    uint8  sim_app_id[GSDI_MAX_APP_ID_LEN];
    gsdidiag_elementary_file_enum_type sim_elementary_file;
    uint8  simple_req;
    uint8  sim_index_mode;
    uint16 sim_offset;
    uint16 sim_rec_num;
    uint16 sim_data_len;
    char   efs_filename[1];
} gsdidiag_sim_write_cmd_req_type;

/*============================================================================
PACKET   Response gsdidiag_sim_write_cmd_rsp_type
         uint32 status         :  Status of the cmd request.
         uint16 transaction_id :  transaction id for the request/response.
         uint8 sim_slot_number :  Specifies slot that the command goes to.
         uint8 sim_app_id_len  :  Specifies the length of the optional APP ID
         uint8 sim_app_id      :       Buffer of max 16 bytes for an APP ID
         uint32 sim_elementary_file:  Filename for which the attributes are
                                  needed.
         char efs_filename[1]  :  Filename from which the write data is got from

ID       GSDIDIAG_SIM_WRITE_CMD

PURPOSE  This packet is response for write req into SIM. The Req stastus field
          signifies if the request was sucessfully queued to GSDI task

REQUEST  gsdidiag_sim_write_cmd_req_type
         Response contains the status, and transaction id.
         Future event EVENT_GSDI_SIM_WRITE represents that the write has
         been completed.
============================================================================*/
typedef PACKED struct
{
    gsdidiag_cmd_header_type header;
    uint32 status;
    uint16 transaction_id;
    uint8  sim_slot_number;
    uint8  sim_app_id_len;
    uint8  sim_app_id[GSDI_MAX_APP_ID_LEN];
    gsdidiag_elementary_file_enum_type sim_elementary_file;
    uint8  simple_req;
    uint8  sim_index_mode;
    uint16 sim_offset;
    uint16 sim_rec_num;
    uint16 sim_data_len;
    char   efs_filename[1];
} gsdidiag_sim_write_cmd_rsp_type;


/*============================================================================
PACKET   Response  gsdidiag_increase_cmd_rsp_type
         uint32 status:
         uint16 transaction_id:
         uint8 sim_slot_number:  Specifies to which slot this command goes to
         uint32 sim_elementary_file: file on which value resides
         uint32 increase_value: Value by which to increase
         char efs_filename[1]:  Filename for which the increased value is to be
                                 stored to

ID       GSDIDIAG_SIM_INCREASE_CMD

PURPOSE  This packet is used to increase the contents of a value on the
         sim card.

REQUEST gsdidiag_sim_get_file_attributes_cmd_rsp_type
         Response contains the status, and transaction id.
         Future event EVENT_SIM_INCREASE
         will represent that the increase value is stored in an efs file..
============================================================================*/
typedef PACKED struct
{
    gsdidiag_cmd_header_type header;
    uint32 status;
    uint16 transaction_id;
    uint8  sim_slot_number;
    gsdidiag_elementary_file_enum_type sim_elementary_file;
    uint32 increase_value;
    char   efs_filename[1];
} gsdidiag_sim_increase_cmd_rsp_type;

/*============================================================================
PACKET   gsdidiag_get_pin_status_cmd
    gsdidiag_cmd_header_type header: header for the request
    uint8  sim_slot_number         : Specifies to which slot this command goes to
    uint8  sim_app_id_len          : Specifies the length of the optional APP ID
    uint8  sim_app_id[GSDI_MAX_APP_ID_LEN]          : Buffer of max 16 bytes for an APP ID
    uint8  sim_pin_id              : pin id

ID       GSDIDIAG_GET_PIN_STATUS

PURPOSE  This packet is used to retrieve status of a pin.

RESPONSE  gsdidiag_get_pin_status_cmd_rsp_type.
          Future event EVENT_GSDI_PIN_STATUS will indicate pin status.
============================================================================*/
typedef PACKED struct
{
    gsdidiag_cmd_header_type header;
    uint8  sim_slot_number;
    uint8  sim_app_id_len;
    uint8  sim_app_id[GSDI_MAX_APP_ID_LEN];
    uint8  sim_pin_id;
} gsdidiag_get_pin_status_cmd_req_type;

/*============================================================================
PACKET   gsdidiag_get_pin_status_rsp_type
    gsdidiag_cmd_header_type header: header for the response
    uint32 status         :  Status of the cmd request.
    uint16 transaction_id :  transaction id for the request/response.
    uint8  sim_slot_number: Specifies to which slot this command goes to
    uint8  sim_app_id_len : Specifies the length of the optional APP ID
    uint8  sim_app_id[GSDI_MAX_APP_ID_LEN] : Buffer of max 16 bytes for an APP ID
    uint8  sim_pin_id     : pin id

ID       GSDIDIAG_GET_PIN_STATUS

PURPOSE  This packet is used to retrieve status of a pin.

REQUEST   gsdidiag_get_pin_status_cmd_req_type.
          Future event EVENT_GSDI_PIN_STATUS will indicate pin status.
============================================================================*/
typedef PACKED struct
{
    gsdidiag_cmd_header_type header;
    uint32 status;
    uint16 transaction_id;
    uint8  sim_slot_number;
    uint8  sim_app_id_len;
    uint8  sim_app_id[GSDI_MAX_APP_ID_LEN];
    uint8  sim_pin_id;
} gsdidiag_get_pin_status_cmd_rsp_type;

/*============================================================================
PACKET   Request gsdidiag_pin_data_req_type
                 typedefed to
                 gsdidiag_verify_pin_cmd_req_type
                 gsdidiag_enable_pin_req_type
                 gsdidiag_disable_pin_req_type

         uint8   sim_slot_number:  Specifies to which slot this command goes to
         uint8   sim_app_id_len :  Specifies the length of the optional APP ID
         uint8   sim_app_id     :  Buffer of max 16 bytes for an APP ID
         uint32  sim_pin_id     :  The id of the pin.
         char    sim_pin        :  Pin value.

ID       GSDIDIAG_VERIFY_PIN
         GSDIDIAG_ENABLE_PIN
         GSDIDIAG_DISABLE_PIN

PURPOSE  These packets are used to verify, enable or diable pins respectively.

RESPONSE gsdidiag_sim_verify_pin_cmd_rsp_type,
         gsdidiag_sim_enable_pin_cmd_rsp_type,
         gsdidiag_sim_disable_pin_cmd_rsp_type
         Response contains the status, and transaction id.
         Future event EVENT_GSDI_VERIFY_PIN,
                      EVENT_GSDI_ENABLE_PIN,
                      EVENT_GSDI_DISABLE_PIN
         will  denote success of the req
============================================================================*/
typedef PACKED struct
{
    gsdidiag_cmd_header_type header;
    uint8  sim_slot_number;
    uint8  sim_app_id_len;
    uint8  sim_app_id[GSDI_MAX_APP_ID_LEN];
    uint8  sim_pin_id;
    char   sim_pin[1];
} gsdidiag_pin_data_req_type;


/* req packet for verify pin */
typedef gsdidiag_pin_data_req_type gsdidiag_verify_pin_cmd_req_type;
/* req packet for enable pin */
typedef gsdidiag_pin_data_req_type gsdidiag_enable_pin_cmd_req_type;
/* req packet for disable pin */
typedef gsdidiag_pin_data_req_type gsdidiag_disable_pin_cmd_req_type;

/*============================================================================
PACKET   Response gsdidiag_pin_data_req_type
                 typedefed to
                 gsdidiag_verify_pin_cmd_rsp_type
                 gsdidiag_enable_pin_cmd_rsp_type
                 gsdidiag_disable_pin_cmd_rsp_type

         uint8   sim_slot_number:  Specifies to which slot this command goes to
         uint8   sim_app_id_len :  Specifies the length of the optional APP ID
         uint8   sim_app_id     :  Buffer of max 16 bytes for an APP ID
         uint32  sim_pin_id     :  The id of the pin.
         char    sim_pin        :  Pin value.

ID       GSDIDIAG_VERIFY_PIN
         GSDIDIAG_ENABLE_PIN
         GSDIDIAG_DISABLE_PIN

PURPOSE  These packets are used to verify, enable or diable pins respectively.

RESPONSE ggsdidiag_sim_verify_pin_cmd_rsp_type,
         gsdidiag_sim_enable_pin_cmd_rsp_type,
         gsdidiag_sim_disable_pin_cmd_rsp_type
         Response contains the status, and transaction id.
         Future event EVENT_GSDI_VERIFY_PIN,
                      EVENT_GSDI_ENABLE_PIN,
                      EVENT_GSDI_DISABLE_PIN
         will  denote success of the req
============================================================================*/

typedef PACKED struct
{
    gsdidiag_cmd_header_type header;
    uint32 status;
    uint16 transaction_id;
    uint8  sim_slot_number;
    uint8  sim_app_id_len;
    uint8  sim_app_id[GSDI_MAX_APP_ID_LEN];
    uint8  sim_pin_id;
    char   sim_pin[1];
} gsdidiag_pin_data_rsp_type;

/* rsp packet for verify_pin */
typedef gsdidiag_pin_data_rsp_type gsdidiag_verify_pin_cmd_rsp_type;

/* rsp packet for enable pin */
typedef gsdidiag_pin_data_rsp_type gsdidiag_enable_pin_cmd_rsp_type;

/* rsp packet for disable pin */
typedef gsdidiag_pin_data_rsp_type gsdidiag_disable_pin_cmd_rsp_type;

/*============================================================================
PACKET   Request gsdidiag_unblock_pin_cmd_req_type
         gsdidiag_cmd_header_type header:
         uint8 sim_slot_number: Specifies to which slot this command goes to
         uint8 sim_app_id_len : Specifies the length of the optional APP ID
         uint8 sim_app_id[GSDI_MAX_APP_ID_LEN] : Buffer of max 16 bytes for an APP ID
         uint8 sim_pin_id     : Pin id
         uint8 sim_unblock_pin[8]: Unblock pin
         uint8 sim_new_pin[8] :  New pin

ID       GSDIDIAG_UNBLOCK_PIN

PURPOSE  This packet is used to unblock a blocked pin

RESPONSE gsdidiag_sim_unblock_pin_rsp_type
         Response contains the status, and transaction id.
         Future event EVENT_UNBLOCK_PIN denoted the success of the request.
============================================================================*/
typedef PACKED struct
{
    gsdidiag_cmd_header_type header;
    uint8 sim_slot_number;
    uint8 sim_app_id_len;
    uint8 sim_app_id[GSDI_MAX_APP_ID_LEN];
    uint8 sim_pin_id;
    char  sim_pin_unblock_new[18]; /* two pins are passed
                                     sim_unblock_pin[] and sim_new_pin[].
                                     Each pin is null terminated */
} gsdidiag_unblock_pin_cmd_req_type;

/*============================================================================
PACKET   Response gsdidiag_unblock_pin_cmd_rsp_type
         gsdidiag_cmd_header_type header:
         uint32 status        : Status of the request
         uint16 transaction_id: Transaction id of the request
         uint8 sim_slot_number: Specifies to which slot this command goes to
         uint8 sim_app_id_len : Specifies the length of the optional APP ID
         uint8 sim_app_id[GSDI_MAX_APP_ID_LEN] : Buffer of max 16 bytes for an APP ID
         uint8 sim_pin_id     : Pin id
         uint8 sim_unblock_pin_new[]: Unblock pin\nNew pin\n

ID       GSDIDIAG_UNBLOCK_PIN

PURPOSE  This packet is used to unblock a blocked pin

REQUEST  gsdidiag_sim_unblock_pin_req_type
         Response contains the status, and transaction id.
         Future event EVENT_UNBLOCK_PIN denoted the success of the request.
============================================================================*/
typedef PACKED struct
{
    gsdidiag_cmd_header_type header;
    uint32 status;
    uint16 transaction_id;
    uint8 sim_slot_number;
    uint8 sim_app_id_len;
    uint8 sim_app_id[GSDI_MAX_APP_ID_LEN];
    uint8 sim_pin_id;
    char  sim_pin_unblock_new[1]; /* two pins are contained in this passed
                                    sim_unblock_pin[] and sim_new_pin[].
                                    each pin is null terminated */

} gsdidiag_unblock_pin_cmd_rsp_type;

/*============================================================================
PACKET   Request gsdidiag_get_available_apps_cmd_req_type
         gsdidiag_cmd_header_type header;
         uint8 sim_slot_number;Specifies to which slot this command goes to

ID       GSDIDIAG_GET_AVAILABLE_APPS

PURPOSE  This packet is used to retrieve the available apps

RESPONSE gsdidiag_sim_get_file_attributes_cmd_rsp_type.
         Response contains the apps available. This is a synchronous req.
============================================================================*/
typedef PACKED struct
{
    gsdidiag_cmd_header_type header;
} gsdidiag_get_available_apps_cmd_req_type;

/*============================================================================
PACKET   Request gsdidiag_get_available_apps_cmd_rsp_type
         gsdidiag_cmd_header_type header: header
         uint32 status         : Status of the request
         uint16 transaction_id : Transaction id representin if it a synchonrous
                                 or an asynchrnous request. This is an
                                 synchnrouns request. Value will be 0.
         uint8  sim_slot_number: Specifies to which slot this command went to
         uint8  apps_available : output apps available

ID       GSDIDIAG_GET_AVAILABLE_APPS

PURPOSE  This packet is used to retrieve the available apps

REQUEST  gsdidiag_get_available_apps_cmd_rsp_type
         Contains the apps available of this SIM card.
============================================================================*/
typedef PACKED struct{
    uint8 card_inserted;
    uint8 apps_available;
} slot_info;

typedef PACKED struct
{
    gsdidiag_cmd_header_type header;
    uint32 status;
    uint16 transaction_id;
    uint8  operational_slot;
    uint8  num_slots;
    slot_info slots_info[1]; /* allocating just one slot. if more slots are present,
                                then memory will be allocated in the response packet*/
} gsdidiag_get_available_apps_cmd_rsp_type;

/*============================================================================
PACKET   Request gsdidiag_sim_send_apdu_req_type
         gsdidiag_cmd_header_type header;
         uint8  sim_slot_number;Specifies to which slot this command goes to
         uint8  sim_app_id_len: Buffer of max 16 bytes for an APP ID
         uint8  sim_app_id[GSDI_MAX_APP_ID_LEN]: Buffer of max 16 bytes for an APP ID
         uint32 apdu_len      : Length of the apdu
         uint8  apdu_data[1] : Data buffer holding the apdu data

ID       GSDIDIAG_EXECUTE_APDU

PURPOSE  This sends an apdu to the sim driver interface.

RESPONSE gsdidiag_sim_get_file_attributes_cmd_rsp_type
         Response contains the status, and transaction id.
         Future event EVENT_GSDI_EXECUTE_APDU
         will represent that the file attrbites are stored in an efs file.
============================================================================*/
typedef PACKED struct
{
    gsdidiag_cmd_header_type header;
    uint8  sim_slot_number;
    uint8  sim_app_id_len;
    uint8  sim_app_id[GSDI_MAX_APP_ID_LEN];
    uint32 apdu_len;
    uint8  apdu_data[1];
} gsdidiag_sim_send_apdu_req_type;

/*============================================================================
PACKET   Request gsdidiag_sim_send_apdu_rsp_type
         uint32 status         : Status of the request
         uint16 transaction_id : Transaction id relating the req/rsp with event
         gsdidiag_cmd_header_type header:
         uint8  sim_slot_number: Specifies to which slot this command goes to
         uint8  sim_app_id_len : Specifies the length of the optional APP ID
         uint8  sim_app_id[GSDI_MAX_APP_ID_LEN] : Buffer of max 16 bytes for an APP ID
         uint32 apdu_len       : Length of the APDU
         uint8  apdu_data[1]  : Buffer of the APDU

ID       GSDIDIAG_EXECUTE_APDU

PURPOSE  Sends APDU to the gsdi task

REQUEST  gsdidiag_sim_send_apdu_rsp_type
         Future event EVENT_GSDI_EXECUTE_APDU  will be sent after completion
         of the execution of the function.
============================================================================*/
typedef PACKED struct
{
    gsdidiag_cmd_header_type header;
    uint32 status;
    uint16 transaction_id;
    uint8  sim_slot_number;
    uint8  sim_app_id_len;
    uint8  sim_app_id[GSDI_MAX_APP_ID_LEN];
    uint32 apdu_len;
    uint8  apdu_data[1];
} gsdidiag_sim_send_apdu_rsp_type;


/*============================================================================

PACKET   gsdidiag_select_cmd_req_type
    gsdidiag_cmd_header_type header: Header
    uint8  sim_slot_number : Specifies to which slot this command goes to
    uint8  sim_app_id_len  : Specifies the length of the optional APP ID
    uint8  sim_app_id[GSDI_MAX_APP_ID_LEN]  : Buffer of max 16 bytes for an APP ID
    uint32 sim_elementary_file: File to be selected
    uint32 sim_directory   :  Master, Dedicated, Application Dedicated File
                              to be selected
    uint8  sim_elementary_file_path_len: Specifies the length of the
                                         elementary file path
    uint8  sim_elementary_file_path: Complete File path starting from 3F00
                                     that the user wants to select
                                     e.g., 3F007F106F3A
    uint8  sim_directory_path_len: Specifies the length of the directory path
    uint8  sim_directory_path: Complete DF path starting from 3F00 that the
                               user wants to select
                               e.g., 3F007F205F31
    char   efs_filename[1] : File on EFS to store read data to

ID      GSDIDIAG_SELECT_CMD

PURPOSE  Select MF, ADF, DF, EF from the sim

RESPONSE gsdidiag_select_cmd_rsp_type.
         Rsp contains status and transaction_id. Response indicates that request
         has been queued. Future event EVENT_GSDI_SELECT will indicate that
         the required information has been written to an efs file.

============================================================================*/
typedef PACKED struct
{
    gsdidiag_cmd_header_type header;
    uint8  sim_slot_number;
    uint8  sim_app_id_len;
    uint8  sim_app_id[GSDI_MAX_APP_ID_LEN];
    gsdidiag_elementary_file_enum_type sim_elementary_file;
    uint32 sim_directory;
    uint8  sim_elementary_file_path_len;
    uint8  sim_elementary_file_path[2*UIM_MAX_PATH_ELEMENTS];
    uint8  sim_directory_path_len;
    uint8  sim_directory_path[2*UIM_MAX_PATH_ELEMENTS];
    char   efs_filename[1];
} gsdidiag_select_cmd_req_type;

/*============================================================================
PACKET   gsdidiag_select_cmd_rsp_type
    uint32 status          : Status of the request
    uint16 transaction_id  : Specifies to which slot this command goes to
    uint8  sim_slot_number : Specifies to which slot this command goes to
    uint8  sim_app_id_len  : Specifies the length of the optional APP ID
    uint8  sim_app_id[GSDI_MAX_APP_ID_LEN]  : Buffer of max 16 bytes for an APP ID
    uint32 sim_elementary_file: File to be selected
    uint32 sim_directory   :  Master, Dedicated, Application Dedicated File
                              to be selected
    uint8  sim_elementary_file_path_len: Specifies the length of the
                                         elementary file path
    uint8  sim_elementary_file_path: Complete File path starting from 3F00
                                     that the user wants to select
                                     e.g., 3F007F106F3A
    uint8  sim_directory_path_len: Specifies the length of the directory path
    uint8  sim_directory_path: Complete DF path starting from 3F00 that the
                               user wants to select
                               e.g., 3F007F205F31
    char   efs_filename[1] : File on EFS to store read data to

ID      GSDIDIAG_SELECT_CMD

PURPOSE  Select MF, ADF, DF, EF from the sim

RESPONSE gsdidiag_select_cmd_rsp_type.
         Rsp contains status and transaction_id. Response indicates that request
         has been queued. Future event EVENT_GSDI_SELECT will indicate that
         the required information has been written to an efs file.

============================================================================*/
typedef PACKED struct
{
    gsdidiag_cmd_header_type header;
    uint32 status;
    uint16 transaction_id;
    uint8  sim_slot_number;
    uint8  sim_app_id_len;
    uint8  sim_app_id[GSDI_MAX_APP_ID_LEN];
    gsdidiag_elementary_file_enum_type sim_elementary_file;
    uint32 sim_directory;
    uint8  sim_elementary_file_path_len;
    uint8  sim_elementary_file_path[2*UIM_MAX_PATH_ELEMENTS];
    uint8  sim_directory_path_len;
    uint8  sim_directory_path[2*UIM_MAX_PATH_ELEMENTS];
    char   efs_filename[1];
} gsdidiag_select_cmd_rsp_type;


/*============================================================================
PACKET   Request gsdidiag_act_feature_ind_req_type
         gsdidiag_cmd_header_type header;
         uint8  sim_slot_number: Specifies to which slot this command goes to
         uint8  perso_feature : Specifies the Personalization feature to be
                                activated
         char  perso_feature_key[1]  : Buffer of the key

ID       GSDIDIAG_ACTIVATE_FEATURE_INDICATOR_CMD

PURPOSE  Activate personalization feature

RESPONSE gsdidiag_act_feature_ind_rsp_type
         Response contains the status, and transaction id.
         Future event EVENT_GSDI_ACT_FEATURE_IND
         will indicate if the activate was successful or not with the number of
         retries left for the control key
============================================================================*/
typedef PACKED struct
{
    gsdidiag_cmd_header_type header;
    uint8  sim_slot_number;
    uint8  perso_feature;
    char   perso_feature_key[1];
} gsdidiag_act_feature_ind_req_type;

/*============================================================================
PACKET   Request gsdidiag_act_feature_ind_rsp_type
         uint32 status         : Status of the request
         uint16 transaction_id : Transaction id relating the req/rsp with event
         gsdidiag_cmd_header_type header:
         uint8  sim_slot_number: Specifies to which slot this command goes to
         uint8  perso_feature : Specifies the Personalization feature to be
                                activated
         char  perso_feature_key[1]  : Buffer of the key

ID       GSDIDIAG_ACTIVATE_FEATURE_INDICATOR_CMD

PURPOSE  Activate personalization feature

REQUEST  gsdidiag_act_feature_ind_rsp_type
         Future event EVENT_GSDI_ACT_FEATURE_IND  will be sent after
         completion of the execution of the function.
============================================================================*/
typedef PACKED struct
{
    gsdidiag_cmd_header_type header;
    uint32 status;
    uint16 transaction_id;
    uint8  sim_slot_number;
    uint8  perso_feature;
    char   perso_feature_key[1];
} gsdidiag_act_feature_ind_rsp_type;


/*============================================================================
PACKET   Request gsdidiag_deact_feature_ind_req_type
         gsdidiag_cmd_header_type header;
         uint8  sim_slot_number: Specifies to which slot this command goes to
         uint8  perso_feature : Specifies the Personalization feature to be
                                deactivated
         char   perso_feature_key[1]  : Buffer of the key

ID       GSDIDIAG_DEACTIVATE_FEATURE_INDICATOR_CMD

PURPOSE  Deactivate personalization feature

RESPONSE gsdidiag_deact_feature_ind_rsp_type
         Response contains the status, and transaction id.
         Future event EVENT_GSDI_DEACT_FEATURE_IND
         will indicate if the deactivate was successful or not with the number of
         retries left for the control key
============================================================================*/
typedef PACKED struct
{
    gsdidiag_cmd_header_type header;
    uint8  sim_slot_number;
    uint8  perso_feature;
    char   perso_feature_key[1];
} gsdidiag_deact_feature_ind_req_type;

/*============================================================================
PACKET   Request gsdidiag_deact_feature_ind_rsp_type
         uint32 status         : Status of the request
         uint16 transaction_id : Transaction id relating the req/rsp with event
         gsdidiag_cmd_header_type header:
         uint8  sim_slot_number: Specifies to which slot this command goes to
         uint8  perso_feature : Specifies the Personalization feature to be
                                deactivated
         char   perso_feature_key[1]  : Buffer of the key

ID       GSDIDIAG_DEACTIVATE_FEATURE_INDICATOR_CMD

PURPOSE  Deactivate personalization feature

REQUEST  gsdidiag_deact_feature_ind_rsp_type
         Future event EVENT_GSDI_DEACT_FEATURE_IND  will be sent after
         completion of the execution of the function.
============================================================================*/
typedef PACKED struct
{
    gsdidiag_cmd_header_type header;
    uint32 status;
    uint16 transaction_id;
    uint8  sim_slot_number;
    uint8  perso_feature;
    char   perso_feature_key[1];
} gsdidiag_deact_feature_ind_rsp_type;


/*============================================================================
PACKET   Request gsdidiag_get_feature_ind_req_type
         gsdidiag_cmd_header_type header;
         uint8  sim_slot_number: Specifies to which slot this command goes to

ID       GSDIDIAG_GET_FEATURE_INDICATION_CMD

PURPOSE  Get personalization feature indication

RESPONSE gsdidiag_get_feature_ind_rsp_type
         Response contains the status, and transaction id.
         Future event EVENT_GSDI_GET_FEATURE_IND
         will indicate if the get feature indication was successful or not
         the event will contain values of each feature level indication
============================================================================*/
typedef PACKED struct
{
    gsdidiag_cmd_header_type header;
    uint8  sim_slot_number;
} gsdidiag_get_feature_ind_req_type;

/*============================================================================
PACKET   Request gsdidiag_get_feature_ind_rsp_type
         uint32 status         : Status of the request
         uint16 transaction_id : Transaction id relating the req/rsp with event
         gsdidiag_cmd_header_type header:
         uint8  sim_slot_number: Specifies to which slot this command goes to

ID       GSDIDIAG_GET_FEATURE_INDICATION_CMD

PURPOSE  Get personalization feature indication

REQUEST  gsdidiag_get_feature_ind_rsp_type
         Future event EVENT_GSDI_GET_FEATURE_IND  will be sent after
         completion of the execution of the function.
============================================================================*/
typedef PACKED struct
{
    gsdidiag_cmd_header_type header;
    uint32 status;
    uint16 transaction_id;
    uint8  sim_slot_number;
} gsdidiag_get_feature_ind_rsp_type;



/*============================================================================
PACKET   Request gsdidiag_set_feature_ind_data_req_type
         gsdidiag_cmd_header_type header;
         uint8  sim_slot_number: Specifies to which slot this command goes to
         uint8  perso_feature : Specifies the Personalization feature to be
                                deactivated
         char   perso_feature_data[1]  : Buffer of the data

ID       GSDIDIAG_SET_FEATURE_INDICATOR_DATA_CMD

PURPOSE  Set personalization feature indicator data

RESPONSE gsdidiag_set_feature_ind_data_rsp_type
         Response contains the status, and transaction id.
         Future event EVENT_GSDI_SET_FEATURE_IND_DATA
         will indicate if the set feature indicator data was successful or not
============================================================================*/
typedef PACKED struct
{
    gsdidiag_cmd_header_type header;
    uint8  sim_slot_number;
    uint8  perso_feature;
    char   perso_feature_data[1];
} gsdidiag_set_feature_ind_data_req_type;

/*============================================================================
PACKET   Request gsdidiag_set_feature_ind_data_rsp_type
         uint32 status         : Status of the request
         uint16 transaction_id : Transaction id relating the req/rsp with event
         gsdidiag_cmd_header_type header:
         uint8  sim_slot_number: Specifies to which slot this command goes to
         uint8  perso_feature : Specifies the Personalization feature to be
                                deactivated
         char   perso_feature_data[1]  : Buffer of the data

ID       GSDIDIAG_SET_FEATURE_INDICATOR_DATA_CMD

PURPOSE  Setet personalization feature indicator data

REQUEST  gsdidiag_set_feature_ind_data_rsp_type
         Future event EVENT_GSDI_SET_FEATURE_IND_DATA  will be sent after
         completion of the execution of the function.
============================================================================*/
typedef PACKED struct
{
    gsdidiag_cmd_header_type header;
    uint32 status;
    uint16 transaction_id;
    uint8  sim_slot_number;
    uint8  perso_feature;
    char   perso_feature_data[1];
} gsdidiag_set_feature_ind_data_rsp_type;

/*============================================================================
PACKET   Request gsdidiag_unblock_feature_ind_req_type
         gsdidiag_cmd_header_type header;
         uint8  sim_slot_number: Specifies to which slot this command goes to
         uint8  perso_feature : Specifies the Personalization feature to be
                                unblocked
         char   perso_feature_key[1]  : Buffer of the key

ID       GSDIDIAG_UNBLOCK_FEATURE_INDICATOR_CMD

PURPOSE  Unblock personalization feature control key for a particular feature

RESPONSE gsdidiag_unblock_feature_ind_rsp_type
         Response contains the status, and transaction id.
         Future event EVENT_GSDI_UNBLOCK_FEATURE_IND
         will indicate if the unblock was successful or not with the number of
         retries left for the unblock control key
============================================================================*/
typedef PACKED struct
{
    gsdidiag_cmd_header_type header;
    uint8  sim_slot_number;
    uint8  perso_feature;
    char   perso_feature_key[1];
} gsdidiag_unblock_feature_ind_req_type;

/*============================================================================
PACKET   Request gsdidiag_unblock_feature_ind_rsp_type
         uint32 status         : Status of the request
         uint16 transaction_id : Transaction id relating the req/rsp with event
         gsdidiag_cmd_header_type header:
         uint8  sim_slot_number: Specifies to which slot this command goes to
         uint8  perso_feature : Specifies the Personalization feature to be
                                unblocked
         char   perso_feature_key[1]  : Buffer of the key

ID       GSDIDIAG_UNBLOCK_FEATURE_INDICATOR_CMD

PURPOSE  Unblock personalization feature control key for a particular feature

REQUEST  gsdidiag_unblock_feature_ind_rsp_type
         Future event EVENT_GSDI_UNBLOCK_FEATURE_IND  will be sent after
         completion of the execution of the function.
============================================================================*/
typedef PACKED struct
{
    gsdidiag_cmd_header_type header;
    uint32 status;
    uint16 transaction_id;
    uint8  sim_slot_number;
    uint8  perso_feature;
    char   perso_feature_key[1];
} gsdidiag_unblock_feature_ind_rsp_type;

/*============================================================================
PACKET   Request gsdidiag_get_feature_key_req_type
         gsdidiag_cmd_header_type header;
         uint8  sim_slot_number: Specifies to which slot this command goes to
         uint8  feature        : Feature indicator that the key is requested
         char   efs_filename[1]:

ID       GSDIDIAG_GET_FEATURE_INDICATOR_KEY_CMD

PURPOSE  Get personalization feature indication

RESPONSE gsdidiag_get_feature_key_rsp_type
         Response contains the status, and transaction id.
         Future event EVENT_GSDI_GET_CONTROL_KEY
         will indicate if the get feature indication was successful or not
         the event will contain values of each feature level indication
============================================================================*/
typedef PACKED struct
{
    gsdidiag_cmd_header_type header;
    uint8  sim_slot_number;
    uint8  feature;
    char   efs_filename[1];
} gsdidiag_get_feature_key_req_type;

/*============================================================================
PACKET   Request gsdidiag_get_feature_key_rsp_type
         uint32 status         : Status of the request
         uint16 transaction_id : Transaction id relating the req/rsp with event
         gsdidiag_cmd_header_type header:
         uint8  sim_slot_number: Specifies to which slot this command goes to
         uint8  feature        : Feature indicator that the key is requested
         char   efs_filename[1]:

ID       GSDIDIAG_GET_FEATURE_INDICATOR_KEY_CMD

PURPOSE  Get personalization feature indication

REQUEST  gsdidiag_get_feature_key_rsp_type
         Future event EVENT_GSDI_GET_CONTROL_KEY  will be sent after
         completion of the execution of the function.
============================================================================*/
typedef PACKED struct
{
    gsdidiag_cmd_header_type header;
    uint32 status;
    uint16 transaction_id;
    uint8  sim_slot_number;
    uint8  feature;
    char   efs_filename[1];
} gsdidiag_get_feature_key_rsp_type;

/*============================================================================
PACKET   Request gsdidiag_get_perm_feature_ind_req_type
         gsdidiag_cmd_header_type header;
         uint8  sim_slot_number: Specifies to which slot this command goes to

ID       GSDIDIAG_GET_PERM_FEATURE_INDICATION_CMD

PURPOSE  Get personalization feature indication

RESPONSE gsdidiag_get_perm_feature_ind_rsp_type
         Response contains the status, and transaction id.
         Future event EVENT_GSDI_GET_CONTROL_KEY
         will indicate if the get feature indication was successful or not
         the event will contain values of each feature level indication
============================================================================*/
typedef PACKED struct
{
    gsdidiag_cmd_header_type header;
    uint8  sim_slot_number;
} gsdidiag_get_perm_feature_ind_req_type;

/*============================================================================
PACKET   Request gsdidiag_get_perm_feature_ind_rsp_type
         uint32 status         : Status of the request
         uint16 transaction_id : Transaction id relating the req/rsp with event
         gsdidiag_cmd_header_type header:
         uint8  sim_slot_number: Specifies to which slot this command goes to

ID       GSDIDIAG_GET_PERM_FEATURE_INDICATION_CMD

PURPOSE  Get personalization feature indication

REQUEST  gsdidiag_get_perm_feature_ind_rsp_type
         Future event EVENT_GSDI_GET_CONTROL_KEY  will be sent after
         completion of the execution of the function.
============================================================================*/
typedef PACKED struct
{
    gsdidiag_cmd_header_type header;
    uint32 status;
    uint16 transaction_id;
    uint8  sim_slot_number;
} gsdidiag_get_perm_feature_ind_rsp_type;

/*============================================================================
PACKET   Request gsdidiag_disable_perm_feature_ind_req_type
         gsdidiag_cmd_header_type header;
         uint8  sim_slot_number: Specifies to which slot this command goes to
         uint8  perso_feature : Specifies the Personalization feature to be
                                unblocked
         char   perso_feature_key[1]  : Buffer of the key

ID       GSDIDIAG_PERM_DISABLE_FEATURE_INDICATOR_CMD

PURPOSE  Permanently disable personalization feature indication

RESPONSE gsdidiag_disable_perm_feature_ind_rsp_type
         Response contains the status, and transaction id.
         Future event EVENT_GSDI_PERM_DISBALE_FEATURE_IND
         will indicate if the permanently disable the feature indication
         is successful or not, an event will contain feature level and number
         of retries of the verify control key
============================================================================*/
typedef PACKED struct
{
    gsdidiag_cmd_header_type header;
    uint8  sim_slot_number;
    uint8  perso_feature;
    char   perso_feature_key[1];
} gsdidiag_disable_perm_feature_ind_req_type;

/*============================================================================
PACKET   Request gsdidiag_disable_perm_feature_ind_rsp_type
         uint32 status         : Status of the request
         uint16 transaction_id : Transaction id relating the req/rsp with event
         gsdidiag_cmd_header_type header:
         uint8  sim_slot_number: Specifies to which slot this command goes to
         uint8  perso_feature : Specifies the Personalization feature to be
                                unblocked
         char   perso_feature_key[1]  : Buffer of the key

ID       GSDIDIAG_PERM_DISABLE_FEATURE_INDICATOR_CMD

PURPOSE  Permanently disable personalization feature indication

REQUEST  gsdidiag_disable_perm_feature_ind_rsp_type
         Future event EVENT_GSDI_PERM_DISBALE_FEATURE_IND  will be sent after
         completion of the execution of the function.
============================================================================*/
typedef PACKED struct
{
    gsdidiag_cmd_header_type header;
    uint32 status;
    uint16 transaction_id;
    uint8  sim_slot_number;
    uint8  perso_feature;
    char   perso_feature_key[1];
} gsdidiag_disable_perm_feature_ind_rsp_type;

/*============================================================================
PACKET   Request gsdidiag_get_feature_ind_data_req_type
         gsdidiag_cmd_header_type header;
         uint8  sim_slot_number: Specifies to which slot this command goes to
         uint8  feature        : Feature indicator that the key is requested
         char   efs_filename[1]: File name for perso data file in EFS

ID       GSDIDIAG_GET_FEATURE_INDICATOR_DATA_CMD

PURPOSE  Get personalization feature indicator data

RESPONSE gsdidiag_get_feature_ind_data_rsp_type
         Response contains the status, and transaction id.
         Future event EVENT_GSDI_GET_FEATURE_INDICATOR_DATA
         will indicate if the get feature indication was successful or not
         the event will contain values of each feature level indication
============================================================================*/
typedef PACKED struct
{
  gsdidiag_cmd_header_type header;
  uint8                    sim_slot_number;
  uint8                    feature;
  char                     efs_filename[1];
} gsdidiag_get_feature_ind_data_req_type;

/*============================================================================
PACKET   Request gsdidiag_get_feature_ind_data_rsp_type
         gsdidiag_cmd_header_type header:
         uint32 status         : Status of the request
         uint16 transaction_id : Transaction id relating the req/rsp with event
         uint8  sim_slot_number: Specifies to which slot this command goes to
         uint8  feature        : Feature indicator that the key is requested
         char   efs_filename[1]: File name for perso data file in EFS

ID       GSDIDIAG_GET_FEATURE_INDICATOR_DATA_CMD

PURPOSE  Get personalization feature indicator data

REQUEST  gsdidiag_get_feature_ind_data_rsp_type
         Future event EVENT_GSDI_GET_FEATURE_INDICATOR_DATA  will be sent after
         completion of the execution of the function.
============================================================================*/
typedef PACKED struct
{
  gsdidiag_cmd_header_type header;
  uint32                   status;
  uint16                   transaction_id;
  uint8                    sim_slot_number;
  uint8                    feature;
  char                     efs_filename[1];
} gsdidiag_get_feature_ind_data_rsp_type;

/*===========================================================================
FUNCTION gsdidiag_event_notification

DESCRIPTION
  This function is called when GSDI is going to notify Diag of any event
  related to the card.

ARGUMENTS
  gsdi_sim_events_T, the event to be notified

DEPENDENCIES
  None

RETURN VALUE
  None

SIDE EFFECTS
  None
===========================================================================*/
void gsdidiag_event_notification (gsdi_sim_events_T event);

#endif /*  GSDIDIAG_H */

