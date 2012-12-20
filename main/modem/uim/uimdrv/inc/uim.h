#ifndef UIM_H
#define UIM_H
/*===========================================================================

               E X T E R N A L   U I M  T A S K   D E F I N I T I O N S

DESCRIPTION
  This contains all the declarations for the Authentication Task.

Copyright (c) 2001-2010 by QUALCOMM, Incorporated.  All Rights Reserved.

===========================================================================*/

/*===========================================================================

                      EDIT HISTORY FOR FILE

$PVCSPath: O:/src/asw/COMMON/vcs/uim.h_v   1.19   11 Oct 2002 09:54:00   ckrishna  $
$Header: //source/qcom/qct/modem/uim/su/baselines/qsc1110/rel/3.3.65/uim/uimdrv/inc/uim.h#2 $ $DateTime: 2011/03/09 23:17:08 $ $Author: sratnu $

when       who     what, where, why
--------   ---     ----------------------------------------------------------
06/06/10   ssr     Added support for BCSMS service in cdma service table
06/10/09   yb      Fix to send status word as received in get response to AT COP
                   even if card returns error for envelope sent.
04/29/09   kk      Updated EF file path info for non-spec defined telecom EFs
12/04/08   nb      Fixed QTF compilation problem
12/03/08   nb      Fixed Compilation Error
11/26/08   nb      Removed Featurization Flags for Common Modem Interface.
09/23/08   jk      Polling value changed from 28 s to 25 s to align 1x PCH 
                   and QPCH page decodes
09/17/08   vs      Updated clash entry for poll delay timer SIG
09/15/08   vs      Added clash entry for UIM USB ICCD pre-suspend timer SIG
08/26/08   kk      Few comments from uim directory management review
08/20/08   vs      Added SIG clash entries for poll delay and fallback to ISO
08/14/08   kk      Added handling for SELECT EFs with non-generic file ids
07/22/08   js      Fixed UIM_MAX_PATH_ELEMENTS issue. Updated path length to 4
                   for all UIM commands. MAX path length in MMGSDI is set to 5.
07/17/08   vs      Added support for UIM USB UICC ICCD interface
07/17/08   kk      Added support for PKCS 15
06/21/08   kk      Added PKCS15 Support
04/30/08   kk      Added support for MMS EFs
03/24/08   sun     Added support for OMA BCAST
03/20/08   nk      Added OMH EF and CST Support
01/02/08   jk      Added functions to check and remove memory allocs on cmd q
11/19/07   js      Radio Access Technology Balancing support
11/08/07   vs/nk   Added EUIMID and OMH feature support
10/03/07   jk      Compile Fix for HRPD Off
09/27/07   tml     Lint fixes
09/10/07   tml     Added MFLO support, remove featurization for 1000 PBM
08/16/07   jk      Rearranged featuization to support DATA Auth on 7K
07/02/07   sun     Added support for ODD INS Class
05/21/07   jk      Add API so that others can check if UIM is in Recovery mode
05/02/07   sun     Added support for MBMS
05/02/07   sp      Changed the value of UIM_MAX_POLL_DURATION to prevent
                   overflow.
04/12/07   sun     Featurized Phonebook
02/13/07   pv      Increase the UIM_POLL_OFFSET by 1000 milli seconds. and
                   define the maximum delay between regular wake-up poll
                   timer and always on poll timer.
02/09/07   sk      Added support for 1000 phonebook entries.
11/13/06   pv      Add UIM_PASSIVE as a vote to poll UIM.
10/05/06   pv      Rectify the value for UIM_MAX_EF_BY_PATH_CACHE_ITEMS
09/21/06   sun     Change RPC meta comments for uim_get_xxx_path_from_file_id
                   to allow a null to be returned in the output path pointer.
09/11/06   jk      Changed featurization so that 7200 and 7500 generate identical
                   RPC Files
08/09/06   pv      Featurize the usage of members of nv_uim_config_params_type
                   under FEATURE_UIM_NO_ADDITIONAL_CONFIG_PARAMS for
                   targets not picking up the NV dependencies
07/25/06   jk      Added Signal for EFSLOG
07/05/06   jar     Fixes to allow for more than 4 ANRs to be used.
06/12/06   pv      Add UIM_RECOVER_COMPLETE command.
                   Re-define UIM_MAX_NUM_CMD_ATTEMPTS to use the config param.
                   Define uim_return_type.
                   Add UIM_OPTION_RECOVERY_CMD to options.
                   Extern uim_recovery_cmd.
06/05/06   pv      Add definition for UIM_MAX_BAD_BLOCK_RETRIES to be used
                   while in T=1 protocol to identify maximum re-try count.
04/18/06   nk      Un-featurize code to work when FEATURE_UIM_UICC is not defined
04/19/06   pv      Metadata comments for RPC export for 7200.
04/03/06   pv      Lint Fixes
03/21/06   tml     added cfis support
03/14/06   pv      Added code to wait until uim_mv_init is done by tmc.
03/09/06   pv      Added new command to clear the response cache
03/08/06   jar     Merged in HZI from branch
02/16/06   pv      Added support for UIM_USIM_EHPLMN
02/15/06   pv      Reduced UIM_MAX_USIM_ITEMS to 88
02/14/06   nk      Merged for: Implement a cached select mechanism.  Which
                   would not send down the select to the card if the response
                   is available from the cache.
01/30/06   pv      Add more enemurations to indicate power down reason to GSDI
11/22/05   jk      Metadata comments for RPC export
08/29/05   jk      Added functions to get path to files from EF file ID
08/25/07   pv      Added options to pdown command.
08/04/05   wli     Added comments
08/02/05   wli     Added HOT SWAP feature.
07/22/05   pv      Got definitions for Aggregation of Traffic channel
                   voting bits from uim.c need to use them in uimgen.c
07/18/05   sun     Added support for USIM files in ENS
06/13/05   pv      Code to support UIM commands with allocated memory.
06/08/05   wli     Defined UIM_MAX_NUM_CMD_ATTEMPTS for maximum command retry.
06/02/05   pv      Code for FEATURE_UIM_ISIM & FEATURE_UIM_SESSION_MANAGEMENT
05/10/05   pv      Added code for BT sim access - Code to handle
                   UIM_RESET_SWITCH_UIM_PASSIVE_R, UIM_POWER_UP_UIM_PASSIVE_R,
                   and UIM_POWER_DOWN_R commands and responses
04/25/05   jk      Added FEATURE_UIM_RUIM around new HW ESN issue to fix compile
                   issue for non-RUIM
04/22/05   jk      Added function to return HW ESN for BREW
03/14/05   wli     Added 820B EFs
02/28/05   sun     Added file - EF MWIS
01/14/05   sun     Added support for Cingular Wireless
12/16/04   jk      Merged changes for Orange support from mainline
12/16/04   jk      Fixed CR # 54476 - Streaming APDU size up to 260 bytes
11/22/04   jk      Finished adding support for 3GPD
11/21/04   wli     Dual slot merge and debug.
10/06/04   jar     Merged in 4.2x Additional EFs
09/28/04   wli     Added enum items to status.
09/01/04   jk      Added EF to table for HRPD AN
09/01/04   ck      Added support for CPHS files
09/01/04   ck      Added support for CSIM
06/16/04   ck      Added protocol type to uim_dfs_present structure
06/16/04   wli     Added ORANGE DF and its related EFs.
06/11/04   wli     Added EFs to support KDDI.
05/17/04   ck      Added data structures to support ISO 7816 streaming API.
                   Modified WIM and UTIL APIs to include channel information
                   and removed AID information in the command.
                   Prefixed polling related definitions with UIM
03/26/04   ts      Added a transaction timer signal for signaling the UIM task
                   when detecting excessive null procedure bytes.
12/19/03   jk      Added processing for R-UIM 3GPD Services.
12/09/03   ck      Changed the min poll duration to 5 sec and added a constant
                   UIM_POLL_OFFSET which is 1sec.
10/17/03   ck      Added support to stream APDU.
08/15/03   ck      Added UIM_CDMA_TC and UIM_GW_TC bit to voting interface.
08/14/03   ck      Added support for dual slots.
07/09/03   ck      Changed the polling interval timer from 29 secs to 28 secs
                   as were failing the polling tests in some corner case
                   scenarios.
07/09/03   ck      Removed the featurization around the funtion uim_nv_init()
03/28/03   ts      Changed FEATURE_UIM_USIM feature to FEATURE_UIM_UICC for
                   UICC functions, seperate from USIM.
03/28/03   ts      Externalized uim_process_card response for T=1 protocol.
02/26/03   jk      Added UIM UTIL and WIM functionality
03/04/03   ck      Externalised the function uim_get_enum_value_for_path and
                   added an item UIM_NO_SUCH_ITEM to the uim_items_enum_type
02/20/03   ck      Cast the MAX_POLL_DURATION value to unsigned.
02/19/03   ts      Fixed lint errors from GSTK support code.
02/13/03   wli     Added to support GSTK.
01/17/03   ck      Added uim_aid_type field to all CHV related commands and
                   featurized it under FEATURE_UIM_USIM
01/07/03   ts      Added EFrplmnat
10/25/02   ck      Added an enum UIM_POLL_ERR_S to uim_status_type
10/11/02   ck      Externalized the function uim_nv_init which reads the first
                   instruction class from NV.
07/24/02   ck      Added UIM_TC to uim_voter_type.
07/24/02   ck      Added definitions for Telecom, Solsa and Mexe DFs.
05/21/02   ts      Added support to receive 256 byte transfers from the card.
04/09/02   ck      Modified the featuization around UIM_CLK_OFF_S to include
                   FEATURE_UIM_USIM.
02/11/02   ts      Added more enum constants for EFs that are identified but must
                   selected by the path field in the commands.  This is because
                   their path is determined by the card.
02/13/02   wli     Deleted #ifdef around UIM_xxx_DF definitions.
01/22/02   ck      Added support for clock stop mode.
01/08/02   ck      Wrapped the call to uim_return_cdma_svc_availabililty around
                   the feature FEATURE_UIM_RUIM
12/18/01   ck      Added support for EFs in DCS1800 DF.
12/13/01   ts      Fixed some more comments with regard to feature switches.
12/04/01   ts      Fixed a few comment lines.
11/13/01   ts      Added support for UIM Application Toolkit (UTK).
10/11/01   ck      Enhanced the SEEK interface to include path in the command.
07/27/01   ts      Cleaned up some indents in the code.
07/26/01   wli     Moved some definitions from uimgen.h
07/23/01   ck      Added support for EF ECC, EF SPC STATUS, EF MAX PRL and
                   added a function definition to export the response to
                   store esn_me.
05/28/01   ck      Added support for R-UIM, GSM and USIM modules.
01/15/01   ck      Created module from AUTH header module rev 1.5 from Common
                   archive.


===========================================================================*/

/*===========================================================================

                     INCLUDE FILES FOR MODULE

===========================================================================*/

#include "comdef.h"
#include "queue.h"
#include "cmd.h"
#include "qw.h"
#include "sys.h"

/* <EJECT> */


/*===========================================================================

                        DEFINITION DEPENDANCIES

===========================================================================*/

/* In order for FEATURE_UIM_WIM to be defined, FEATURE_UIM_UTIL must also be
defined.  If FEATURE_UIM_WIM_DEBUG is defined, then FEATURE_UIM_WIM must be
defined */

#if defined(FEATURE_UIM_WIM) && !defined(FEATURE_UIM_UTIL)
#error defined(FEATURE_UIM_WIM) && !defined(FEATURE_UIM_UTIL)
#endif

#if defined(FEATURE_UIM_WIM_DEBUG) && !defined(FEATURE_UIM_WIM)
#error defined(FEATURE_UIM_WIM_DEBUG) && !defined(FEATURE_UIM_WIM)
#endif



/* <EJECT> */
/*===========================================================================

                        DATA DECLARATIONS

===========================================================================*/

#define UIM_CMD_RSP_TIME         2000
#define UIM_COMMIT_RSP_TIME      30000

#ifdef FEATURE_UIM_CDMA
#define UIM_POLL_TIME            25000
#else
#define UIM_POLL_TIME            28000
#endif /* FEATURE_UIM_CDMA */

#define UIM_MAX_NUM_ATTEMPTS        3

#ifndef FEATURE_UIM_NO_ADDITIONAL_CONFIG_PARAMS
  #define UIM_MAX_NUM_CMD_ATTEMPTS    ( 2 + uim_config_params.ext_cmd_additional_re_try_cnt )
#else
  #define UIM_MAX_NUM_CMD_ATTEMPTS    2
#endif /* FEATURE_UIM_NO_ADDITIONAL_CONFIG_PARAMS */

#define UIM_MAX_PATH_ELEMENTS       4
#define UIM_MAX_BAD_BLOCK_RETRIES   2

#define UIM_AID_MAX_NAME_LENGTH  16

#define UIM_MAX_ATR_CHARS        33

/* This table contains the path corresponding to the items in the items table.
   This is a three dimensional table and each record contains the DF1, DF2
   and EF corresponding to the EF. If there is no second level DF, the DF2 is
   the same. If the EF is under the MF, the DF1 and DF2 are set to zero. The
   order of the items is the same as in the uim_items_table array.

   !!!!!! PLEASE TAKE UTMOST CARE WHEN UPDATING THIS TABLE !!!!!! */

/* IMPORTANT!!!!!!!! order in uim_items_table, gsdi_mf_cache_tbl and the enum need to match */
#define UIM_MAX_EF_UNDER_MF_ITEMS       5 // READ ABOVE!

/* IMPORTANT!!!!!!! order in uim_items_table, gsdi_ruim_cache_tbl and the enum need to match */
#define UIM_MAX_RUIM_ITEMS              106 // READ ABOVE!

/* IMPORTANT!!!!!!!! order in uim_items_table, gsdi_gsm_cache_tbl and the enum need to match */
#define UIM_MAX_GSM_ITEMS               62 // READ ABOVE!

#define UIM_MAX_DCS1800_ITEMS           10

/* IMPORTANT!!!!!!!! order in uim_items_table, gsdi_telecom_cache_tbl and the enum need to match */
#define UIM_MAX_TELECOM_ITEMS           28 // READ ABOVE!

/* IMPORTANT!!!!!!!! order in uim_items_table, gsdi_usim_cache_tbl and the enum need to match */
#define UIM_MAX_USIM_ITEMS              102 // READ ABOVE!
#define UIM_MAX_ORANGE_ITEMS            4

#define UIM_MAX_ISIM_ITEMS              6

#define UIM_MAX_CINGULAR_ITEMS          2
#define UIM_MAX_WIM_ITEMS               3

#define UIM_MAX_HZI_ITEMS               11

#define UIM_MAX_MFLO_ITEMS              12

#define UIM_MAX_EF_BY_PATH_ITEMS        1

/* IMPORTANT!!!!!!!! order in uim_items_table, gsdi_ef_cache_tbl and the enum need to match */
#define UIM_MAX_EF_BY_PATH_CACHE_ITEMS  108 // READ ABOVE!


/* This is the size of the buffer created to hold the response from a compute
IP Authentication */
#define UIM_COMPUTE_IP_AUTH_RESPONSE_LENGTH 16


/* Maximum Challenge length for Simple IP */
#define UIM_MAX_3GPD_CHAP_CHALLENGE_LENGTH  32

/* Default Simple IP profile to be used on the card
 NOTE:  Only one Simple IP profile is supported in the ME, although
 multiple profiles are supported on the RUIM.  Profiles can only be changed
 at complile-time by changing this constant. */

#define UIM_CURRENTLY_ACTIVE_SIP_PROFILE 0

/* Maximum Length for MN-HA Registration Data
This is changeable depending on how large the registration data is expected to
be and how many blocks of data the card can accept */
#define UIM_MAX_3GPD_MN_HA_REG_DATA_LENGTH  800

/* Maximum Length for MIP-RRQ Registration Data
This is changeable depending on how large the registration data is expected to
be and how many blocks of data the card can accept */
#define UIM_MAX_3GPD_HASH_RRQ_DATA_LENGTH   800

/* Length of the MN-AAA extention header, used in the MIP-RRQ-HASH command */
#define UIM_3GPD_MN_AAA_EXT_HEADER_LENGTH   8

/* Length of the challenge data for Mobile IP */
#define UIM_3GPD_MIP_CHALLENGE_LENGTH       238

#define UIM_MAX_HRPD_CHAP_CHALLENGE_LENGTH 254

/* OTASP/OTAPA NAM LOCK definitions. */
#define UIM_NAM_LOCK_STATE_MASK  0x1
#define UIM_NAM_LOCK_OTA_MASK    0x4
#define UIM_NL_STATE_LOCK        0x1
#define UIM_NL_STATE_UNLOCK      0x0
#define UIM_OTA_OTAPA            0x4
#define UIM_OTA_OTASP            0x0
#define UIM_OTA_NAM_LOCK         0x2
#define UIM_NAM_LOCK_BITS        0x7

#define UIM_RUN_CAVE_DIGITS     3
#define UIM_GLOBAL_RAND_CHAL     0x00
#define UIM_UNIQUE_RAND_CHAL     0x01


#define UIM_DATA_DO_HEADER 0x4D
#define UIM_DATA_FILE_PATH 0x81
#define UIM_DATA_KEY_REF_PUB_ASYM 0x83
#define UIM_DATA_KEY_DIR_SYM 0x83
#define UIM_DATA_KEY_SESS_SY 0x84
#define UIM_DATA_KEY_REF_PRV_ASYM 0x84
#define UIM_DATA_HASH_CODE 0x90
#define UIM_DATA_RNDM_NUM 0x91
#define UIM_DATA_CHLNG_DATA_ITM 0x94
#define UIM_DATA_RESULT_LEN 0x96

#define UIM_PSO_DATA_PLAIN_VALUE 0x80
#define UIM_PSO_DATA_PADD_IND_CRYPTOGRAM 0x86
#define UIM_PSO_DATA_CRYPTOGRAPHIC_CHECKSUM 0x8E
#define UIM_PSO_DATA_DIGITAL_SIGNATURE 0x9E
#define UIM_PSO_DATA_DIGITAL_SIGNATURE_INPUT 0x9A
#define UIM_PSO_DATA_LE_VALUE 0x96
#define UIM_PSO_DATA_INPUT_TEMPLATE_DS 0xA8


#define UIM_WIM_MSE_P1_COMPUTE 0x41
#define UIM_WIM_MSE_P1_ENCIPHERING 0x81

#define UIM_WIM_MSE_P2_DIGITAL_SIGNATURE 0xB6
#define UIM_WIM_MSE_P2_CONFIDENTIALITY 0xB8
#define UIM_WIM_MSE_P2_CRYPTO_CHECKSUM 0xB4


#define UIM_EF_UNDER_MF_START    0x0
#define UIM_RUIM_EF_START        0x1
#define UIM_GSM_EF_START         0x2
#define UIM_TELECOM_EF_START     0x3
#define UIM_USIM_EF_START        0x4
#define UIM_DCS1800_EF_START     0x5
#define UIM_WIM_EF_START         0x6
#define UIM_ORANGE_EF_START      0x7
#define UIM_CINGULAR_EF_START    0x8
#define UIM_ISIM_EF_START        0x9
#define UIM_HZI_EF_START         0xA
#define UIM_MFLO_EF_START        0xB
#define UIM_EF_PATH_START        0xF

/* Seek and Search parameters */
#define UIM_P2_SEEK_TYPE_1       0x00
#define UIM_P2_SEEK_TYPE_2       0x10

#define UIM_P2_SEEK_BEGINNING_FORWARDS   0
#define UIM_P2_SEEK_END_BACKWARDS        1
#define UIM_P2_SEEK_NEXT_LOCN_FORWARDS   2
#define UIM_P2_SEEK_PREV_LOCN_BACKWARDS  3

/* b1-b3 of P2 */
#define UIM_P2_SRCH_FORWARD  0x04
#define UIM_P2_SRCH_BACKWARD 0x05
#define UIM_P2_SRCH_ENHANCED 0x06

/* b4-b8 of the first search indicaton byte */
#define UIM_SRCH_IND_ABS            0x00
#define UIM_SRCH_IND_CHAR           0x08

/* b1-b3 of the first search indicaton byte */
#define UIM_SRCH_IND_P1_FORWARD     0x04
#define UIM_SRCH_IND_P1_BACKWARD    0x05
#define UIM_SRCH_IND_NEXT           0x06
#define UIM_SRCH_IND_PREV           0x07

/* Bit 2 of first data byte indicates where to start search */
#define UIM_SRCH_MODE_MASK          0x02

/* The length of the search indication bytes */
#define UIM_SRCH_IND_BYTES          2

/* Status parameters */
#define UIM_STATUS_P1_NO_INDICATION     0x00
#define UIM_STATUS_P1_CURR_APP_INIT     0x01
#define UIM_STATUS_P1_TERMN_OF_CURR_APP 0x02


#define UIM_STATUS_P2_RSP_SAME_AS_SELECT  0x00
#define UIM_STATUS_P2_DF_NAME_OF_CURR_APP 0x01
#define UIM_STATUS_P2_RETURN_NONE         0x0C

#define UIM_SPC_STATUS_MASK      0x01

#define UIM_CDMA_DF              0x7F25
#define UIM_GSM_DF               0x7F20
#define UIM_DCS_1800_DF          0x7F21
#define UIM_TELECOM_DF           0x7F10
#define UIM_ORANGE_DF            0x7F40
#define UIM_CINGULAR_DF          0x7F66
#define UIM_GRAPHICS_DF          0x5F50
#define UIM_SOLSA_DF             0x5F70
#define UIM_MEXE_DF              0x5F3C
#define UIM_HZI_DF               0x7F43
#define UIM_MFLO_RAN_DF          0x5F10
#define UIM_MFLO_MFAPP_DF        0x5F50

#define UIM_TERMINAL_PROFILE_DATA_LENGTH    20

/* Mediaflo length #defines */
#define UIM_MFLO_SUBSCRIBER_ID_LEN       0x08
#define UIM_MFLO_PUBLIC_KEY_LEN          0x83
#define UIM_MFLO_INIT_ACT_PARAMS_LEN     0x88

/* These constants define the duration conversion values */
#define MSEC_PER_DSEC 100
#define MSEC_PER_SEC  1000
#define MSECS_PER_MIN 60000

/* Minimum polling duration supported by code. */
#define UIM_MIN_POLL_DURATION 5000
#define UIM_POLL_OFFSET 2000
/* Maximum difference between the regular poll timer and wake up poll timer */
#define UIM_WAKE_UP_POLL_OFFSET 1500
#define UIM_MAX_POLL_DURATION (unsigned)(0xFFFFFFFF - UIM_WAKE_UP_POLL_OFFSET)
#define UIM_MIN_TIME_UNIT UIM_TK_TENTHS_SECS
#define UIM_MIN_TIME_INTERVAL (UIM_MIN_POLL_DURATION/MSEC_PER_DSEC)

#define UIM_PARAM_P            64     /* 64 bytes of param p in key exchange */
#define UIM_PARAM_G            20     /* 20 bytes of param g in key exchange */
#define UIM_RAND_SEED          20     /* 20 bytes of seed used in key exchange*/

/* Maximum number of chars in one transaction */
#define UIM_MAX_CHARS                                512
#define UIM_MAX_TRANSACTION_SIZE                     256
#define UIM_MAX_STREAM_ISO7816_APDU_TRANSACTION_SIZE 261
#define UIM_MAX_TRANS_SIZE_OUT                       255
#define UIM_MAX_TRANSACTION_P3                       0x0
#define UIM_MAX_PSO_DATA                             256
#define NUM_BYTES_APDU_HDR                           0x5
/* This is the maximum number of digits */
#define UIM_MAX_CHV_DIGITS                           0x8

/*--------------------------------------------------------------------------
                             Signal Masks
--------------------------------------------------------------------------*/
#define  UIM_RPT_TIMER_SIG                                0x00000001
  /* This signal is set when the uim_rpt_timer expires.
     This timer is used to enable UIM to kick the watchdog on time */

#define  UIM_CMD_Q_SIG                                    0x00000002
  /* This signal is set when an item is put on the uim_cmd_q. */

#define  UIM_CMD_RSP_SIG                                  0x00000004
  /* This signal is set when UIM is done processing a command */

#define  UIM_CMD_RSP_TIMEOUT_SIG                          0x00000008
  /* This signal is set when the UIM Command Response Timer expires */

#define  UIM_POLL_TIMER_SIG                               0x00000010
  /* This signal is set when the UIM Poll Timer expires */

#define  UIM_UNSOLICITED_RSP_SIG                          0x00000020
  /* This signal is set when unexpected data is recd from the UIM */

#define  UIM_STATE_TOGGLE_SIG                             0x00000040
  /* This signal is set when some task votes on powerdown of UIM */

#define  UIM_TRANSACTION_SIG                              0x00000080
  /* This signal is set when some task votes on powerdown of UIM */

#define  UIM_CARD_SWAP_SIG                                0x00000100

#define  UIM_NV_INIT_DONE_SIG                             0x00000200
  /* This signal is set when tmc is done with initializing the NV items
    required for it's normal operation */

#define  UIM_EFSLOG_PURGE_SIG                             0x00000400
  /* This signal is set when the Local EFS Log buffer has reached 1/2 full */

#ifdef FEATURE_INIT_RUIM_SMSandADD_BYUIMTASK
#define  UIMDATA_INIT_SIG                                 0x0800
#endif
/*Following is a list of reserved signals that should not be used*/
/*******Reserved Signals**************/
#define UIM_CLASH_WITH_UIM_USB_ATTACH_SIG                 0x00000800
#define UIM_CLASH_WITH_UIM_USB_ENUMERATION_FAIL_SIG       0x00001000
#define UIM_CLASH_WITH_TASK_OFFLINE_SIG                   0x00002000
#define UIM_CLASH_WITH_TASK_STOP_SIG                      0x00004000
#define UIM_CLASH_WITH_TASK_START_SIG                     0x00008000
#define UIM_CLASH_WITH_UIM_USB_POLL_DELAY_TIMER_SIG       0x00010000
#define UIM_CLASH_WITH_UIM_USB_FALLBACK_TO_ISO_SIG        0x00020000
#define UIM_CLASH_WITH_UIM_USB_PRE_SUSPEND_TIMER_SIG      0x00040000

/*******Additional Reserved Signals**************/
#define UIM_CLASH_WITH_ONCRPC_REPLY_SIG                   0x00800000

/*******Additional Reserved Signals**************/
#define UIM_CLASH_WITH_CAMERA_SIG                         0x04000000
#define UIM_CLASH_WITH_GRP_ACK_SIG                        0x08000000
#define UIM_CLASH_WITH_SEC_OP_COMPLETE_SIG                0x10000000
#define UIM_CLASH_WITH_VS_OP_COMPLETE_SIG                 0x20000000
#define UIM_CLASH_WITH_FS_OP_COMPLETE_SIG                 0x40000000
#define UIM_CLASH_WITH_REX_SLEEP_SIG                      0x80000000

typedef byte uim_instrns_enum_type;

#define SELECT                          ((uim_instrns_enum_type) 0xA4)
#define STATUS                          ((uim_instrns_enum_type) 0xF2)
#define READ_BINARY                     ((uim_instrns_enum_type) 0xB0)
#define UPDATE_BINARY                   ((uim_instrns_enum_type) 0xD6)
#define READ_RECORD                     ((uim_instrns_enum_type) 0xB2)
#define UPDATE_RECORD                   ((uim_instrns_enum_type) 0xDC)
#define SEEK                            ((uim_instrns_enum_type) 0xA2)
#define INCREASE                        ((uim_instrns_enum_type) 0x32)
#define VERIFY_CHV                      ((uim_instrns_enum_type) 0x20)
#define CHANGE_CHV                      ((uim_instrns_enum_type) 0x24)
#define DISABLE_CHV                     ((uim_instrns_enum_type) 0x26)
#define ENABLE_CHV                      ((uim_instrns_enum_type) 0x28)
#define UNBLOCK_CHV                     ((uim_instrns_enum_type) 0x2C)
#define INVALIDATE                      ((uim_instrns_enum_type) 0x04)
#define REHABILITATE                    ((uim_instrns_enum_type) 0x44)
#define RUN_GSM_ALGORITHM               ((uim_instrns_enum_type) 0x88)
#define AUTHENTICATE                    ((uim_instrns_enum_type) 0x88)
#define AUTHENTICATE_ODD_INS            ((uim_instrns_enum_type) 0x89)
#define BCAST_INS                       ((uim_instrns_enum_type) 0x1B)
#define ISIM_AUTHENTICATE               ((uim_instrns_enum_type) 0x88)
#define SLEEP                           ((uim_instrns_enum_type) 0xFA)
#define GET_RESPONSE                    ((uim_instrns_enum_type) 0xC0)
#define TERMINAL_PROFILE                ((uim_instrns_enum_type) 0x10)
#define ENVELOPE                        ((uim_instrns_enum_type) 0xC2)
#define FETCH                           ((uim_instrns_enum_type) 0x12)
#define TERMINAL_RESPONSE               ((uim_instrns_enum_type) 0x14)
#define UPDATE_SSD                      ((uim_instrns_enum_type) 0x84)
#define BS_CHALLENGE                    ((uim_instrns_enum_type) 0x8A)
#define CONFIRM_SSD                     ((uim_instrns_enum_type) 0x82)
#define RUN_CAVE                        ((uim_instrns_enum_type) 0x88)
#define GEN_KEY_VPM                     ((uim_instrns_enum_type) 0x8E)

#ifdef FEATURE_UIM_RUIM_PRE_A1_OP_CODES
#error code not present
#else
#define MS_KEY_REQUEST                  ((uim_instrns_enum_type) 0x50)
#define KEY_GEN_REQUEST                 ((uim_instrns_enum_type) 0x52)
#define CONFIG_REQUEST                  ((uim_instrns_enum_type) 0x54)
#define DOWNLOAD_REQUEST                ((uim_instrns_enum_type) 0x56)
#endif /* FEATURE_UIM_RUIM_PRE_A1_OP_CODES */

#define SSPR_CONFIG_REQUEST             ((uim_instrns_enum_type) 0xEA)
#define SSPR_DOWNLOAD_REQUEST           ((uim_instrns_enum_type) 0xEC)
#define OTAPA_REQUEST                   ((uim_instrns_enum_type) 0xEE)
#define COMMIT                          ((uim_instrns_enum_type) 0xCC)
#define VALIDATE                        ((uim_instrns_enum_type) 0xCE)
#define STORE_ESN_ME                    ((uim_instrns_enum_type) 0xDE)
#define COMPUTE_IP_AUTHENTICATION       ((uim_instrns_enum_type) 0x80)
#define VERIFY_IN_CHAN                  ((uim_instrns_enum_type) 0x20)
#define DISABLE_VER_IN_CHAN             ((uim_instrns_enum_type) 0x26)
#define ENABLE_VER_IN_CHAN              ((uim_instrns_enum_type) 0x28)
#define CHANGE_REF_IN_CHAN              ((uim_instrns_enum_type) 0x24)
#define RESET_RETRY_IN_CHAN             ((uim_instrns_enum_type) 0x2C)
#define MSE                             ((uim_instrns_enum_type) 0x22)
#define PSO                             ((uim_instrns_enum_type) 0x2A)
#define ASK_RANDOM                      ((uim_instrns_enum_type) 0x84)
#define MANAGE_CHANNEL                  ((uim_instrns_enum_type) 0x70)
#define UIM_GENERATE_ASY_KEY_PAIR       ((uim_instrns_enum_type) 0x46)
#define UIM_MFLO_INIT_ACT_PARAMS        ((uim_instrns_enum_type) 0x30)
#define UIM_MFLO_GET_SUBSCRIBER_ID      ((uim_instrns_enum_type) 0x32)
#define UIM_MFLO_GET_PUBLIC_KEY         ((uim_instrns_enum_type) 0x34)
#define UIM_MFLO_SIGN_UCAST             ((uim_instrns_enum_type) 0x36)
#define UIM_MFLO_VERIFY_UCAST           ((uim_instrns_enum_type) 0x38)

/*--------------------------------------------------------------------------
                         UIM REPORT

--------------------------------------------------------------------------*/
/* -------------------------------------------------------------------
** The UIM Task optionally sends a report when it completes
** a command which produces a result which would be useful to the
** task which issued the command.
** ------------------------------------------------------------------- */
typedef enum
{
  UIM_NO_SUCH_REPORT_R,   /* No such report */
  UIM_ACCESS_R,           /* Access an EF from the UIM */
  UIM_SELECT_R,           /* Select a file/directory from the UIM */
  UIM_SEEK_R,             /* Seek from the UIM */
  UIM_INVALIDATE_R,       /* Invalidate */
  UIM_REHABILITATE_R,     /* Rehabilitate an EF */
  UIM_VERIFY_CHV_R,       /* Verify CHV */
  UIM_CHANGE_CHV_R,       /* Change CHV */
  UIM_DISABLE_CHV_R,      /* Disable CHV */
  UIM_ENABLE_CHV_R,       /* Enable CHV */
  UIM_UNBLOCK_CHV_R,      /* Unblock CHV */
  UIM_STATUS_R,           /* Status of the card */
  UIM_TERMINAL_PROFILE_R, /* Terminal Profile */
  UIM_ENVELOPE_R,         /* UIM acknowledgement/result */
  UIM_TERMINAL_RESPONSE_R,/* Terminal Response */
  UIM_POLLING_INTERVAL_R, /* Polling interval and polling off */
  UIM_RESET_R,            /* Warm Reset */
  UIM_RUN_GSM_ALGO_R,     /* RUN GSM algorithm */
  UIM_STORE_ESN_ME_R,     /* Store ESN_ME command */
  UIM_SSD_UPDATE_R,       /* Begin process to update SSD */
  UIM_CONFIRM_SSD_R,      /* Confirm SSD Update */
  UIM_RUN_CAVE_R,         /* Perform auth signature */
  UIM_GENERATE_KEYS_R,    /* Generate CMEA key and VPM */
  UIM_BS_CHAL_R,          /* Do a Base station Challenge */
  UIM_COMMIT_R,           /* Commit Request */
  UIM_MS_KEY_REQ_R,       /* MS Key Request */
  UIM_KEY_GEN_REQ_R,      /* Key Generation Request */
  UIM_VALIDATE_R,         /* Validation Request */
  UIM_CONFIG_REQ_R,       /* Configuration Request */
  UIM_DOWNLOAD_REQ_R,     /* Download Request */
  UIM_SSPR_CONFIG_REQ_R,  /* SSPR Configuration Request */
  UIM_SSPR_DOWNLOAD_REQ_R,/* SSPR Download Reqeust */
  UIM_OTAPA_REQ_R,        /* Otapa Request */
  UIM_AUTHENTICATE_R,     /* Authenticate for USIM */
  UIM_AUTHENTICATE_ODD_INS_R, /*Authenticate for USIM with ODD Ins Class*/
  UIM_MANAGE_CHANNEL_R,   /* Used to Open/Close Logical Channels */
  UIM_VERIFY_IN_CHAN_R,   /* Verifies data sent from ME */
  UIM_ENABLE_VER_REQ_IN_CHAN_R,     /* Enables Verification Requirement */
  UIM_DISABLE_VER_REQ_IN_CHAN_R,    /* Disables Verification Requirement */
  UIM_CHANGE_REF_DATA_IN_CHAN_R,    /* Compares, Conditionally replaces data */
  UIM_RESET_RETRY_COUNTER_IN_CHAN_R,/* Change ref data after reset of counter */
  UIM_MANAGE_SECURITY_ENV_R, /* Performs Restore, Set, Derive Key */
  UIM_PERFORM_SECURITY_OP_R, /* Key Transport, agreement, decipher, compute
                               and verify digital signature, cmpte checksum */
  UIM_ASK_RANDOM_R,       /* Gets Random Number */
  UIM_SELECT_IN_CHAN_R,   /* Select command in Logical Channel */
  UIM_ACCESS_IN_CHAN_R,   /* Access command in Logical Channel */
  UIM_STREAM_APDU_R,      /*  Stream an APDU to the card */
  UIM_STREAM_ISO7816_APDU_R, /* Stream an ISO 7816 APDU to the card */
  UIM_RESET_WITH_TP_R,    /* Warm Reset with optional Terminal Profile */
  UIM_RESET_SWITCH_UIM_PASSIVE_R, /* Warm Reset to make UIM Passive */
  UIM_POWER_UP_UIM_PASSIVE_R,     /* Power up UIM and bring to passive mode */
  UIM_POWER_DOWN_R,               /* Power down the UIM interface */
  UIM_PREF_SLOT_CHANGE_R, /* Preferred Slot change */
  UIM_COMPUTE_IP_AUTH_R,  /* Compute IP Authentication report */
  UIM_ISIM_AUTHENTICATE_R,
  UIM_READ_BIN_CMD_R,     /* Read binary ef from UIM */
  UIM_READ_REC_CMD_R,     /* Read non binary ef from UIM */
  UIM_WRITE_BIN_CMD_R,        /* Write to UIM  */
  UIM_WRITE_REC_CMD_R,        /* Write to UIM  */
  UIM_INC_CMD_R,          /* Increment record*/
  UIM_CACHED_SEL_R,      /* cached select response */
  UIM_CLEAR_CACHE_R,     /* clear cache command response */
  UIM_RECOVER_COMPLETE_R, /*Recovery complete command response */
  UIM_MFLO_INIT_ACT_PARAMS_R,          /* Initialize the Activation Parameters */
  UIM_MFLO_GET_SUBSCRIBER_ID_R,        /* Retrieve the Subscriber ID */
  UIM_MFLO_GET_PUBLIC_KEY_R,           /* Retrieve the Public Key */
  UIM_MFLO_SIGN_UCAST_R,               /* Sign the Unicast Message */
  UIM_MFLO_VERIFY_UCAST_R,             /* Verify the Unicast Message or Signature */
  UIM_STORE_ESN_MEID_ME_R,             /* Store ESN_MEID_ME command */
  UIM_BCAST_R,                         /* Bcast Message*/
  UIM_MAX_R
} uim_report_code_type;

typedef enum {
  UIM_FAIL = 0,                /* FAIL */
  UIM_PASS = 1                 /* PASS */
} uim_rpt_status;

/* Return type that could be used to indicate success/failure */
typedef enum {
  UIM_SUCCESS,
  UIM_INVALID_PARAMS
}uim_return_type;

/* We plan to support a maximum of  2 slots in the future */
typedef enum {
  UIM_SLOT_NONE = 0,
  UIM_SLOT_1  = 1,
  UIM_SLOT_2  = 2,
  UIM_SLOT_AUTOMATIC = 3
} uim_slot_type;

/* This enum defines the first status word                                   */
typedef enum {
  SW1_NORMAL_END = 0x90,  /* Normal ending of the command                    */
  SW1_END_FETCH  = 0x91,  /* Normal ending of the command with extra command */
  SW1_DLOAD_ERR  = 0x9E,  /* SIM data download error */
  SW1_END_RESP   = 0x9F,  /* Normal ending of the command with a response    */
  SW1_BUSY       = 0x93,  /* SIM App toolkit is busy                         */
  SW1_END_RETRY  = 0x92,  /* Command successful with update retries or memory
                             error                                           */
  SW1_REFERENCE  = 0x94,  /* A reference management problem                  */
  SW1_SECURITY   = 0x98,  /* A security problem                              */
  SW1_NULL       = 0x60,  /* NULL procedure byte                             */
  SW1_P3_BAD     = 0x67,  /* Incorrect parameter P3                          */
  SW1_P1_P2_BAD  = 0x6B,  /* Incorrect parameter P1 or P2                    */
  SW1_INS_BAD    = 0x6D,  /* Unknown instruction code                        */
  SW1_CLA_BAD    = 0x6E,  /* Wrong instruction class given in command        */
  SW1_PROBLEM    = 0x6F,  /* Technical problem; no diagnostic given          */
  SW1_SIG_ERROR1 = 0xFF,  /* This is an indication of an error signal from the
                             transmitted byte                                */
  SW1_SIG_ERROR2 = 0xFE,  /* This is an indication of an error signal from the
                             transmitted byte                                */
  SW1_NO_STATUS  = 0x00,   /* This status word is used when a status word cannot
                             be obtained from the card.  It offers to status
                             information */
  SW1_WARNINGS1 = 0x62,     /* Warnings                                      */
  SW1_WARNINGS2 = 0x63,     /* Warnings                                      */
  SW1_EXECUTION_ERR2    = 0x65, /* No info given, state of NV memory changed */
  SW1_CMD_NOT_ALLOWED   = 0x69, /* Command not allowed                       */
  SW1_WRONG_PARAMS      = 0x6A, /* Wrong parameters                          */

  SW1_USIM_RESEND_APDU  = 0x6C, /* Re-send the APDU header                   */
  SW1_EXECUTION_ERR1    = 0x64, /* No info given, NV memory unchanged        */
  SW1_CLA_NOT_SUPPORTED = 0x68, /* functions in CLA not supported            */
  SW1_USIM_END_RESP     = 0x61, /* Normal ending of a command                */

  SW1_UTIL_END_RESP     = 0x61, /* Normal ending of a command                */
  SW1_LENGTH_ERROR      = 0x6C, /* Wrong length Le                           */
  SW1_SE_FAIL           = 0x66 /* Security Environment error/not set        */
} uim_sw1_type;

/* This enum defines the second status word                                  */
typedef enum {
  SW2_NORMAL_END    = 0x00,  /* Normal ending of the command                 */
  SW2_MEM_PROBLEM   = 0x40,  /* Memory problem                               */
  SW2_NO_EF         = 0x00,  /* No EF selected                               */
  SW2_OUT_OF_RANGE  = 0x02,  /* Out of range (invalid address)               */
  SW2_NOT_FOUND     = 0x04,  /* File ID/pattern not found                    */
  SW2_INCONSISTENT  = 0x08,  /* File inconsistent with command               */
  SW2_NO_CHV        = 0x02,  /* No CHV initialized                           */
  SW2_NOT_FULFILLED = 0x04,  /* Access condition not fulfilled               */
  SW2_CONTRADICTION = 0x08,  /* In contradiction with CHV status             */
  SW2_INVALIDATION  = 0x10,  /* In contradiction with invalidation status    */
  SW2_SEQ_PROBLEM   = 0x34,  /* Problem in the sequence of the command       */
  SW2_BLOCKED       = 0x40,  /* Access blocked                               */
  SW2_MAX_REACHED   = 0x50,  /* Increase cannot be performed; Max value
                                reached                                      */
  SW2_ALGORITHM_NOT_SUPPORTED   = 0x84,
  SW2_INVALID_KEY_CHECK_VALUE   = 0x85,
  SW2_PIN_BLOCKED               = 0x83,
  SW2_NO_EF_SELECTED            = 0x86,
  SW2_FILE_NOT_FOUND            = 0x82,
  SW2_REF_DATA_NOT_FOUND        = 0x88,
  SW2_NV_STATE_UNCHANGED        = 0x00,
  SW2_CMD_SUCCESS_INT_X_RETRIES = 0xC0,
  SW2_MEMORY_PROBLEM            = 0x81,
  SW2_SECURITY_NOT_SATISFIED    = 0x82,
  SW2_NO_INFO_GIVEN             = 0x00,
  SW2_LOGICAL_CHAN_NOT_SUPPORTED= 0x81,
  SW2_SECURE_MSG_NOT_SUPPORTED  = 0x82,
  SW2_FILE_STRUCT_INCOMPATIBLE  = 0x81,
  SW2_REF_DATA_INVALIDATED      = 0x84,
  SW2_CONDITIONS_NOT_SATISFIED  = 0x85,
  SW2_BAD_PARAMS_IN_DATA_FIELD  = 0x80,
  SW2_FUNCTION_NOT_SUPPORTED    = 0x81,
  SW2_RECORD_NOT_FOUND          = 0x83,
  SW2_INCONSISTENT_LC           = 0x85,
  SW2_BAD_PARAMS_P1_P2          = 0x86,
  SW2_LC_INCONSISTENT_WITH_P1_P2= 0x87,
  SW2_NOT_ENOUGH_MEMORY_SPACE   = 0x84,
  SW2_NV_STATE_CHANGED          = 0x00,
  SW2_MORE_DATA_AVAILABLE       = 0x10,

  SW2_RET_DATA_MAY_BE_CORRUPTED = 0x81,
  SW2_EOF_REACHED               = 0x82,
  SW2_SEL_EF_INVALIDATED        = 0x83,
  SW2_INCORRECT_FCI_FORMAT      = 0x84,

  SW2_FILE_FILLED_BY_LAST_WRITE = 0x81,

  SW2_APPLN_SPECIFIC_AUTH_ERR   = 0x62,

  SW2_PIN_STATUS_ERR     = 0x85, /* In contradiction with PIN status */
  SW2_PREMSTR_NOT_RDY    = 0x85, /* Pre-master secret not ready */
  SW2_INTRNL_NOT_RDY     = 0x85, /* Internal data not ready */
  SW2_INCORRECT_TAG      = 0x80, /* Incorrect data or tag */
  SW2_UNDEF_99           = 0x99,
  SW2_MORE_DATA          = 0xF1,
  SW2_AUTH_RSP_DATA      = 0xF3, /* Authentication Response Data Available*/

  SW2_UCAST_AUTH_KEY_NOT_INIT                = 0x01,   /* sw1 = 0x69 */
  SW2_UCAST_DATA_BLOCK_NOT_INIT              = 0x02,   /* sw1 = 0x69 */
  SW2_VERIFY_UCAST_FAILED                    = 0x04,   /* sw1 = 0x69 */
  SW2_LOCKED_INIT_PARAM                      = 0x05,   /* sw1 = 0x69 */
  SW2_APPLET_NOT_INIT                        = 0x06,   /* sw1 = 0x69 */
  SW2_PUBLIC_KEY_OR_CERT_NOT_INIT            = 0x07    /* sw1 = 0x69 */

} uim_sw2_type;

typedef enum {
  UIM_NO_SUCH_MODE = 0,  /* No such mode */
  UIM_CDMA         = 1,  /* CDMA mode of operation */
  UIM_GSM          = 2,  /* GSM mode of operation */
  UIM_WCDMA        = 3,  /* USIM protocol which is based on UICC */
  UIM_UICC         = 4,  /* UICC protocol */
  UIM_MAXIMUM_MODE = 5
}uim_protocol_type;

/* IMPORTANT: The enumeration and typedef for UIM channels should not be combined
   as this introduces Klocwork errors*/
#ifdef FEATURE_UIM_SESSION_MANAGEMENT
  enum {
    UIM_CHANNEL0,
    UIM_CHANNEL1,
    UIM_CHANNEL2,
    UIM_CHANNEL3,
    UIM_MAX_CHANNELS
  };
#else
  enum {
    UIM_CHANNEL0,
    UIM_MAX_CHANNELS
  };
#endif
typedef uint8 uim_channel_type;

/* Same report for both read_binary and read_record */
typedef struct {
  word  num_bytes_read; /* Number of bytes read from UIM */
  byte *data;           /* Pointer to the data */
}uim_read_rpt_type;

/* Same report for both write_binary and write_record */
typedef struct {
  word  num_bytes_written;
}uim_write_rpt_type;

typedef struct
{
  word  inc_rsp_len;
  byte *data;
}uim_inc_rpt_type;

typedef struct {
  word num_data_bytes;         /* Number of data bytes in the response */
  byte data[UIM_MAX_CHARS];    /* Data */
} uim_select_rpt_type;

typedef struct {
  word num_records;            /* USIM specific- number of records that match */
  byte  data[UIM_MAX_CHARS];   /* Record Numbers that match */
} uim_seek_rpt_type;

typedef struct {
  word num_bytes;              /* Number of bytes of data */
  byte data[UIM_MAX_CHARS];    /* Data */
} uim_status_rpt_type;


typedef struct {               /* UIM_ENVELOPE_R */
  word           data_length;           /* Length of data returned */
  byte           data[UIM_MAX_CHARS];   /* Data returned */
  uim_sw1_type   get_resp_sw1;          /* Status Word 1 */
  uim_sw2_type   get_resp_sw2;          /* Status word 2 */
} uim_envelope_rpt_type;

typedef struct {               /* UIM_TERMINAL_RESPONSE_R */
  boolean  proactive_cmd_pending;
} uim_terminal_response_rpt_type;

typedef struct {                      /* UIM_POLLING_INTERVAL_R */
  dword actual_uim_polling_interval; /* The one uim is using */
} uim_polling_interval_rpt_type;



typedef struct {
  word data_length;            /* Length of data returned */
  byte data[UIM_MAX_CHARS];    /* Data returned */
} uim_run_gsm_algo_rpt_type;

typedef struct {               /* UIM_STORE_ESN_ME_R & UIM_STORE_ESN_MEID_ME_R*/
  byte usage_ind_conf;
} uim_store_esn_rpt_type;

typedef struct {               /* UIM_RUN_CAVE_R */
  /* The response from the UIM is actually 3 bytes in size.  dword is used here
     to better facilitate data usage in the ARM (easier to process dword than
     3 byte field). */
  dword auth;
} uim_run_cave_rpt_type;

typedef struct {               /* UIM_GENERATE_KEYS_R */
  byte data[UIM_MAX_CHARS];
} uim_generate_keys_rpt_type;

typedef struct {               /* UIM_BS_CHAL_R */
  dword randbs;
} uim_bs_chal_rpt_type;

typedef struct {               /* UIM_COMMIT_R */
  byte result;
} uim_commit_rpt_type;

typedef struct {               /* UIM_MS_KEY_REQ_R */
  byte result;
} uim_ms_key_rpt_type;

typedef struct {               /* UIM_KEY_GEN_REQ_R */
  byte result;
  byte ms_result_len;
  byte ms_result[UIM_PARAM_P];
} uim_key_gen_rpt_type;

typedef struct {               /* UIM_VALIDATE_R */
  byte block_id;
  byte result;
} uim_validate_rpt_type;

typedef struct {               /* UIM_CONFIG_REQ_R */
  byte block_id;
  byte block_length;
  byte result;
  byte data[UIM_MAX_CHARS - 3];
} uim_config_rpt_type;

typedef struct {               /* UIM_DOWNLOAD_REQ_R */
  byte block_id;
  byte result;
} uim_download_rpt_type;

typedef struct {               /* UIM_SSPR_CONFIG_REQ_R */
  byte block_id;
  byte result;
  byte block_length;
  byte data[UIM_MAX_CHARS - 3];
} uim_sspr_config_rpt_type;

typedef struct {               /* UIM_SSPR_DOWNLOAD_REQ_R */
  byte block_id;
  byte result;
  word segment_offset;
  byte segment_size;
} uim_sspr_download_rpt_type;

typedef struct {               /* UIM_OTAPA_REQ_R */
  byte result;
  byte nam_lock_ind;
  dword rand_otapa;
} uim_otapa_rpt_type;

typedef struct {               /* UIM_COMPUTE_IP_AUTH_R */
  byte data[UIM_COMPUTE_IP_AUTH_RESPONSE_LENGTH];
} uim_compute_ip_auth_rpt_type;


typedef struct {
  word data_length;            /* Length of data returned */
  byte data[UIM_MAX_CHARS];    /* Data returned */
} uim_authenticate_rpt_type;

typedef struct{
  word  len;
  byte *data;
}uim_authenticate_odd_ins_rpt_type;

typedef struct{
  word  len;
  byte *data;
}uim_bcast_rpt_type;


typedef struct {
  word data_length;            /* Length of data returned */
  byte data[UIM_MAX_CHARS];    /* Data returned */
} uim_isim_authenticate_rpt_type;

typedef struct {
  byte          channel_num;    /* Channel Number */
} uim_manage_ch_rpt_type;

typedef struct {
  word num_data_bytes;         /* Number of data bytes in the response */
  byte data[UIM_MAX_CHARS];    /* Data */
} uim_select_in_chan_rpt_type;

typedef struct {
  word          data_length;    /* Length of Data Returned */
  byte          data[UIM_MAX_CHARS]; /* Data */
} uim_perform_sec_op_rpt_type;

typedef struct {
  word          data_length;    /* Length of data returned */
  byte          data[UIM_MAX_CHARS]; /* Data */
} uim_ask_random_rpt_type;

typedef struct {
  uim_slot_type reset_slot;    /* Slot that was reset */
} uim_reset_rpt_type;

typedef struct {
  word           num_data_bytes;      /* Number of data bytes in the rsp */
  byte data[UIM_MAX_CHARS];    /* Data */
  uim_sw1_type   get_resp_sw1;        /* Status Word 1 */
  uim_sw2_type   get_resp_sw2;        /* Status word 2 */
} uim_stream_iso7816_apdu_rpt_type;

typedef struct {
  byte num_bytes;              /* number of bytes in ATR */
  byte data[UIM_MAX_ATR_CHARS];/* buffer to hold the ATR */
} uim_atr_rpt_type;

/* ---------------------------------------------------------------
   STRUCTURE:      uim_mflo_get_subscriber_id_rpt_type

   DESCRIPTION:
     If the operation was successful, this structure contains the
     return data for the
     UIM_MFLO_GET_SUBSCRIBER_ID_F command.
----------------------------------------------------------------*/
typedef struct {
  uint8   subscriber_id [UIM_MFLO_SUBSCRIBER_ID_LEN];
}uim_mflo_get_subscriber_id_rpt_type;

/* ---------------------------------------------------------------
   STRUCTURE:      uim_mflo_get_public_key_rpt_type

   DESCRIPTION:
     If the operation was successful, this structure contains the
     return data for the
     UIM_MFLO_GET_PUBLIC_KEY_F command.
----------------------------------------------------------------*/
typedef struct {
  uint32  len;
  uint8   public_key [UIM_MFLO_PUBLIC_KEY_LEN];
}uim_mflo_get_public_key_rpt_type;

/* ---------------------------------------------------------------
   STRUCTURE:      uim_mflo_sign_ucast_rpt_type

   DESCRIPTION:
     If the operation was successful, this structure contains the
     return data for the
     UIM_MFLO_SIGN_UCAST_F command.
----------------------------------------------------------------*/
typedef struct {
  uint32  ucast_msg_len;
  uint8  *signed_ucast_msg_ptr;
}uim_mflo_sign_ucast_rpt_type;

typedef struct
{
  cmd_hdr_type      rpt_hdr;
    /* Includes the queue links,  pointer to requesting task TCB,
       signal to set on completion,  and done queue. */
  uim_report_code_type rpt_type;     /* What type of report this is */
  uim_rpt_status    rpt_status;      /* whether the access was OK */
  uim_sw1_type      sw1;             /* Status Word 1 */
  uim_sw2_type      sw2;             /* Status word 2 */
  boolean           cmd_transacted;  /* Whether cmd was attempted to the card */
  uint32            user_data;  /* User defined data copied from cmd header */
  union
  {
     uim_read_rpt_type          read; /*Report for read_bin and read_rec */
     uim_write_rpt_type         write; /*Report for write_bin and write_rec */
     uim_inc_rpt_type           inc; /*Report for inc command */
     uim_select_rpt_type        select;
     uim_seek_rpt_type          seek;
     uim_status_rpt_type        status;

     uim_envelope_rpt_type      envelope;
     uim_terminal_response_rpt_type terminal_response;
     uim_polling_interval_rpt_type  polling_interval;


     uim_store_esn_rpt_type     store_esn;
     boolean                    ssd_update_ok;
     uim_run_cave_rpt_type      run_cave;
     uim_generate_keys_rpt_type generate_keys;
     uim_bs_chal_rpt_type       bs_chal;

     uim_commit_rpt_type        commit;
     uim_ms_key_rpt_type        ms_key;
     uim_key_gen_rpt_type       key_gen;
     uim_validate_rpt_type      validate;
     uim_config_rpt_type        configuration;
     uim_download_rpt_type      download;
     uim_sspr_config_rpt_type   sspr_config;
     uim_sspr_download_rpt_type sspr_download;
     uim_otapa_rpt_type         otapa;

     uim_compute_ip_auth_rpt_type compute_ip_auth;


     uim_run_gsm_algo_rpt_type  run_gsm;

     uim_authenticate_rpt_type          autn;
     uim_authenticate_odd_ins_rpt_type  autn_odd_ins;
     uim_bcast_rpt_type                 bcast;

     uim_isim_authenticate_rpt_type isim_autn;

    uim_select_in_chan_rpt_type select_in_chan;
    uim_manage_ch_rpt_type      channel;
    uim_perform_sec_op_rpt_type sec_op;
    uim_ask_random_rpt_type     ask_rand;

    uim_reset_rpt_type          reset;

    uim_atr_rpt_type            rst_uim_passive;
    uim_stream_iso7816_apdu_rpt_type stream_iso7816_apdu;

    uim_mflo_get_subscriber_id_rpt_type  mflo_get_subscriber_id;
    uim_mflo_get_public_key_rpt_type     mflo_get_public_key;
    uim_mflo_sign_ucast_rpt_type         mflo_sign_ucast;

  } rpt;
} uim_rpt_type;

/*--------------------------------------------------------------------------
                               UIM COMMANDS

--------------------------------------------------------------------------*/
/* The commands are grouped as Generic, GSM, CDMA and USIM. They are
   identified  */

/* Command types */
typedef enum {
  UIM_NO_SUCH_COMMAND_F = 0,     /* No such command */
  UIM_INTERNAL_ME_PUP_F = 0x0100,/* POWER UP UIM due to task start up */
  UIM_INTERNAL_WAKE_UP_F,        /* Power up due to Power management */
  UIM_INTERNAL_POLL_F,           /* Internal command to Poll */
  UIM_INTERNAL_SELECT_F,         /* Internal Select command */
  UIM_ACCESS_F,                  /* Read or Write from UIM */
  UIM_SELECT_F,                  /* Select a file/directory from the UIM */
  UIM_SEEK_F,                    /* Seek from the UIM */
  UIM_INVALIDATE_F,              /* Invalidate an EF */
  UIM_REHABILITATE_F,            /* Rehabilitate an EF */
  UIM_VERIFY_CHV_F,              /* Verify CHV */
  UIM_CHANGE_CHV_F,              /* Change CHV */
  UIM_DISABLE_CHV_F,             /* Disable CHV */
  UIM_ENABLE_CHV_F,              /* Enable CHV */
  UIM_UNBLOCK_CHV_F,             /* Unblock CHV */
  UIM_STATUS_F,                  /* Status of the Card */
  UIM_TERMINAL_PROFILE_F,        /* Send terminal profile to UIM */
  UIM_ENVELOPE_F,                /* Send a toolkit command to the UIM */
  UIM_INTERNAL_FETCH_F,          /* Retrieve a pro-active cmd from the UIM */
  UIM_TERMINAL_RESPONSE_F,       /* Send a response to a pro-active command */
  UIM_POLLING_INTERVAL_F,        /* Send a polling command to UIM */
  UIM_RESET_F,                   /* Perform a warm reset and perform power up */
  UIM_STREAM_APDU_F,             /* Used to stream an APDU to the card */
  UIM_STREAM_ISO7816_APDU_F,     /* Used to stream an ISO 7816 APDU */
  UIM_RESET_WITH_TP_F,           /* Warm Reset with optional Terminal Profile */
  UIM_PREF_SLOT_CHANGE_F,        /* Change the pref slot for app specified */
  UIM_RESET_SWITCH_UIM_PASSIVE_F,  /* Warm reset the uim and make UIM passive
                                    from this point on, make UIM passive
                                    allowing only resets and 7816 APDUs only */
  UIM_POWER_UP_UIM_PASSIVE_F,
  UIM_POWER_DOWN_F,
  UIM_READ_BIN_CMD_F,            /* Read from UIM */
  UIM_READ_REC_CMD_F,            /* Read from UIM */
  UIM_WRITE_BIN_CMD_F,           /* Write to UIM */
  UIM_WRITE_REC_CMD_F,           /* Write to UIM */
  UIM_INC_CMD_F,                 /* Increment record */

  UIM_CACHED_SEL_F,              /* Get the select response from the cache
                                  * of from the card if
                                  */
  UIM_CLEAR_CACHE_F,             /* Command to clear ALL/Selective
                                  * get response cache being maintained by UIM
                                  */
  UIM_RECOVERY_COMPLETE_F,        /* Command to indicate that GSDI has completed
                                  * recovery procedure and to continue
                                  * regular commands
                                  */

  UIM_RUN_GSM_ALGO_F    = 0x0200,/* RUN GSM algorithm */

  UIM_STORE_ESN_ME_F    = 0x0300,/* Store ESN_ME */
  UIM_SSD_UPDATE_F,              /* SSD update */
  UIM_CONFIRM_SSD_F,             /* Confirm SSD */
  UIM_RUN_CAVE_F,                /* RUN Cave */
  UIM_GENERATE_KEYS_F,           /* Generate Keys */
  UIM_BS_CHAL_F,                 /* Base station challenge */
  UIM_MS_KEY_REQ_F,              /* MS Key Request */
  UIM_KEY_GEN_REQ_F,             /* Key Generation Request */
  UIM_COMMIT_F,                  /* OTASP Commit request*/
  UIM_VALIDATE_F,                /* Validation request */
  UIM_CONFIG_REQ_F,              /* Configuration Request */
  UIM_DOWNLOAD_REQ_F,            /* Download Request */
  UIM_SSPR_CONFIG_REQ_F,         /* SSPR Configuration Request */
  UIM_SSPR_DOWNLOAD_REQ_F,       /* SSPR Download Request */
  UIM_OTAPA_REQ_F,               /* OTAPA request */
  UIM_COMPUTE_IP_AUTH_F,         /* Peform 3GPD Hash or Authentication */
  UIM_STORE_ESN_MEID_ME_F,       /* Store ESN_MEID_ME */

  UIM_AUTHENTICATE_F    = 0x0400,/* Authenticate for USIM */
  UIM_AUTHENTICATE_ODD_INS_F,
  UIM_BCAST_F,

  UIM_VERIFY_IN_CHAN_F  = 0x0500,/* Verifies data sent from ME */
  UIM_ENABLE_VER_REQ_IN_CHAN_F,  /* Enables verification requirement */
  UIM_DISABLE_VER_REQ_IN_CHAN_F, /* Disables verifcation requirement */
  UIM_CHANGE_REF_DATA_IN_CHAN_F, /* Compares and conditionally replaces data */
  UIM_RESET_RETRY_COUNTER_IN_CHAN_F,/* Chg ref data after reset of retry ctr */
  UIM_MANAGE_SECURITY_ENV_F,     /* Manage Security Environment */
  UIM_PERFORM_SECURITY_OP_F,     /* Perform Security Operation */
  UIM_ASK_RANDOM_F,              /* Get Random Number */
  UIM_SELECT_IN_CHAN_F,          /* Select Command in Logical Channel */
  UIM_ACCESS_IN_CHAN_F,          /* Access Command in Logical Channel */
  UIM_MANAGE_CHANNEL_F,          /* Used to Open/Close Logical Channel */

  UIM_ISIM_AUTHENTICATE_F = 0x0600,/* Authenticate for ISIM */

  UIM_MFLO_INIT_ACT_PARAMS_F = 0x0700, /* Initialize the Activation Parameters */
  UIM_MFLO_GET_SUBSCRIBER_ID_F,        /* Retrieve the Subscriber ID */
  UIM_MFLO_GET_PUBLIC_KEY_F,           /* Retrieve the Public Key */
  UIM_MFLO_SIGN_UCAST_F,               /* Sign the Unicast Message */
  UIM_MFLO_VERIFY_UCAST_F,             /* Verify the Unicast Message or Signature */

  UIM_MAX_F
} uim_cmd_name_type;

typedef enum {
  UIM_CHV_NONE  = 0,
  UIM_CHV1      = 1,             /* CHV1 */
  UIM_CHV2      = 2,             /* CHV2 */
  UIM_PIN1_APP1 = 3,             /* Pin1 for APPLICATON 1 */
  UIM_PIN1_APP2 = 4,             /* Pin1 for APPLICATON 2 */
  UIM_PIN1_APP3 = 5,             /* Pin1 for APPLICATON 3 */
  UIM_PIN1_APP4 = 6,             /* Pin1 for APPLICATON 4 */
  UIM_PIN1_APP5 = 7,             /* Pin1 for APPLICATON 5 */
  UIM_PIN1_APP6 = 8,             /* Pin1 for APPLICATON 6 */
  UIM_PIN1_APP7 = 9,             /* Pin1 for APPLICATON 7 */
  UIM_PIN1_APP8 = 10,            /* Pin1 for APPLICATON 8 */

  UIM_PIN2_APP1 = 11,            /* Pin2 for APPLICATON 1 */
  UIM_PIN2_APP2 = 12,            /* Pin2 for APPLICATON 2 */
  UIM_PIN2_APP3 = 13,            /* Pin2 for APPLICATON 3 */
  UIM_PIN2_APP4 = 14,            /* Pin2 for APPLICATON 4 */
  UIM_PIN2_APP5 = 15,            /* Pin2 for APPLICATON 5 */
  UIM_PIN2_APP6 = 16,            /* Pin2 for APPLICATON 6 */
  UIM_PIN2_APP7 = 17,            /* Pin2 for APPLICATON 7 */
  UIM_PIN2_APP8 = 18,            /* Pin2 for APPLICATON 8 */
  UIM_UNIVERSAL_PIN = 19,        /* Universal Pin */
  UIM_CHV_MAX   = 20
} uim_chv_type;

typedef enum
{
   /* EFs at the MF level */
  /* (0x0)00 => 0 */
  UIM_ICCID           = (UIM_EF_UNDER_MF_START << 8),/* ICCID */
  UIM_ELP,                    /* Extended Language Preference */
  UIM_DIR,                    /* EF DIR for USIM */
  UIM_ARR,                    /* Access rule reference */

   /* EFs in CDMA DF*/
  /* (0x1)00 => 0 */
  UIM_CDMA_CC         = (UIM_RUIM_EF_START << 8),  /* Call Count */
  UIM_CDMA_IMSI_M,              /* IMSI_M */
  UIM_CDMA_IMSI_T,              /* IMSI_T */
  UIM_CDMA_TMSI,                /* TMSI */
  UIM_CDMA_ANALOG_HOME_SID,     /* Analog Home SID */
  UIM_CDMA_ANALOG_OP_PARAMS,    /* Analog Operational Parameters */
  UIM_CDMA_ANALOG_LOCN_AND_REGN_IND,  /* Analog Locn and Regn Indicators */
  UIM_CDMA_HOME_SID_NID,        /* CDMA Home SID and NID */
  UIM_CDMA_ZONE_BASED_REGN_IND, /* CDMA Zone Based Regn Indicators */
  UIM_CDMA_SYS_REGN_IND,        /* CDMA System/Network Regn Indicators */

  /* (0x1)0A => 10 */
  UIM_CDMA_DIST_BASED_REGN_IND, /* CDMA Distance Based Regn Indicators */
  UIM_CDMA_ACCOLC,              /* Access Overload Class */
  UIM_CDMA_CALL_TERM_MODE_PREF, /* Call Termination Mode Preferences */
  UIM_CDMA_SCI,                 /* Suggested Slot Cycle Index */
  UIM_CDMA_ANALOG_CHAN_PREF,    /* Analog Channel Preferences */
  UIM_CDMA_PRL,                 /* Preferred Roaming List */
  UIM_CDMA_RUIM_ID,             /* Removable UIM ID */
  UIM_CDMA_CDMA_SVC_TABLE,      /* CDMA Service Table */
  UIM_CDMA_SPC,                 /* Service Programming Code */
  UIM_CDMA_OTAPA_SPC_ENABLE,    /* OTAPA/SPC-Enable */

  /* (0x1)14 => 20 */
  UIM_CDMA_NAM_LOCK,            /* NAM-Lock */
  UIM_CDMA_OTASP_OTAPA_FEATURES,/* OTASP-OTAPA Parameters */
  UIM_CDMA_SERVICE_PREF,        /* Service Preferences */
  UIM_CDMA_ESN_ME,              /* ESN_ME */
  UIM_CDMA_RUIM_PHASE,          /* R-UIM Phase */
  UIM_CDMA_PREF_LANG,           /* Preferred Languages */
  UIM_CDMA_UNASSIGNED_1,        /* Not assigned yet */
  UIM_CDMA_SMS,                 /* Short Messages */
  UIM_CDMA_SMS_PARAMS,          /* Short Message Service Parameters */
  UIM_CDMA_SMS_STATUS,          /* SMS Status */

  /* (0x1)1E => 30 */
  UIM_CDMA_SUP_SVCS_FEATURE_CODE_TABLE, /* Sup Services Feature Code Table */
  UIM_CDMA_UNASSIGNED_2,        /* Not assigned yet */
  UIM_CDMA_HOME_SVC_PVDR_NAME,  /* CDMA Home Service Provider Name */
  UIM_CDMA_UIM_ID_USAGE_IND,    /* R-UIM ID usage indicator */
  UIM_CDMA_ADM_DATA,            /* CDMA Administrative Data */
  UIM_CDMA_MSISDN,              /* Mobile Directory Number */
  UIM_CDMA_MAXIMUM_PRL,         /* Max size of the PRL */
  UIM_CDMA_SPC_STATUS,          /* SPC Status */
  UIM_CDMA_ECC,                 /* Emergency Call Codes */
  UIM_CDMA_3GPD_ME3GPDOPC,      /* 3GPD Operational Capabilities */

  /* (0x1)28 => 40 */
  UIM_CDMA_3GPD_3GPDOPM,        /* 3GPD Operational Mode */
  UIM_CDMA_3GPD_SIPCAP,         /* 3GPD Simple IP Capabilities */
  UIM_CDMA_3GPD_MIPCAP,         /* 3GPD Mobile IP Capabilities */
  UIM_CDMA_3GPD_SIPUPP,         /* 3GPD Simple IP User Profile Parameters */
  UIM_CDMA_3GPD_MIPUPP,         /* 3GPD Mobile IP User Profile Parameters */
  UIM_CDMA_3GPD_SIPSP,          /* 3GPD Simple IP Status Parameters */
  UIM_CDMA_3GPD_MIPSP,          /* 3GPD Mobile IP Status Parameters */
  UIM_CDMA_3GPD_SIPPAPSS,       /* 3GPD Simple IP PAP SS Parameters */
  UIM_CDMA_UNASSIGNED_3,        /* Reserved */
  UIM_CDMA_UNASSIGNED_4,        /* Reserved */

  /* (0x1)32 => 50 */
  UIM_CDMA_PUZL,                /* Preffered User Zone List */
  UIM_CDMA_MAXPUZL,             /* Maximum PUZL */
  UIM_CDMA_MECRP,               /* ME-specific Config REquest Param */
  UIM_CDMA_HRPDCAP,             /* HRPD Access Auth Capability Param */
  UIM_CDMA_HRPDUPP,             /* HRPD Access Auth User Profile Param */
  UIM_CDMA_CSSPR,               /* CUR_SSPR_P_REV */
  UIM_CDMA_ATC,                 /* Acceess Terminal Class */
  UIM_CDMA_EPRL,                /* Extended Preffered Roaming List */
  UIM_CDMA_BCSMS_CONFIG,        /* Broadcast SMS Configuration */
  UIM_CDMA_BCSMS_PREF,          /* Broadcast SMS Preferences */

  /* (0x1)3C => 60 */
  UIM_CDMA_BCSMS_TABLE,         /* Broadcast SMS Table */
  UIM_CDMA_BCSMS_PARAMS,        /* Broadcast SMS Paramaters */
  UIM_CDMA_MMS_NOTIF,           /* MMS Notification */
  UIM_CDMA_MMS_EXT8,            /* MMS Extension 8 */
  UIM_CDMA_MMS_ICP,             /* MMS Issuer Connectivity Parameters */
  UIM_CDMA_MMS_UP,              /* MMS User Preferences */
  UIM_CDMA_SMS_CAP,             /* SMS Capabilities */
  UIM_CDMA_3GPD_IPV6CAP,        /* IPv6 Capabilities */
  UIM_CDMA_3GPD_MIPFLAGS,       /* Mobile IP Flags */
  UIM_CDMA_3GPD_TCPCONFIG,      /* TCP Configurations */

  /* (0x1)46 => 70 */
  UIM_CDMA_3GPD_DGC,            /* Data Generic Configurations */
  UIM_CDMA_BROWSER_CP,          /* Browser Connectivity Parameters */
  UIM_CDMA_BROWSER_BM,          /* Browser Bookmarks */
  UIM_CDMA_3GPD_SIPUPPEXT,      /* Simple IP User Profile Parameters Extension */
  UIM_CDMA_MMS_CONFIG,          /* MMS Configuration */
  UIM_CDMA_JAVA_DURL,           /* Java Download URL */
  UIM_CDMA_3GPD_MIPUPPEXT,      /* Mobile IP User Profile Parameters Extension */
  UIM_CDMA_BREW_DLOAD,          /* BREW Download */
  UIM_CDMA_BREW_TSID,           /* BREW Teleservice ID */
  UIM_CDMA_BREW_SID,            /* BREW Subscriber ID */

  /* (0x1)50 => 80 */
  UIM_CDMA_LBS_XCONFIG,         /* LBS XTRA Configuration */
  UIM_CDMA_LBS_XSURL,           /* LBS XTRA Server URLs */
  UIM_CDMA_LBS_V2CONFIG,        /* LBS V2 Configuration */
  UIM_CDMA_LBS_V2PDEADDR,       /* LBS V2 PDE Address */
  UIM_CDMA_LBS_V2MPCADDR,       /* LBS V2 MPC Address */
  UIM_CDMA_BREW_AEP,            /* BREW Application Execution Policy */
  UIM_CDMA_MODEL,               /* Device Model information */
  UIM_CDMA_RC,                  /* Root Certificate */
  UIM_CDMA_APP_LABELS,          /* Application Labels*/
  UIM_CDMA_USER_AGENT_STRING,   /* KDDI Agent String */

  /* (0x1)5A => 90 */
  UIM_CDMA_GID2,                /* KDDI GID2 */
  UIM_CDMA_GID1,                /* KDDI GID1 */
  UIM_CDMA_GID_REFERENCES,      /* KDDI GID References */
  UIM_CDMA_IMSI_STATUS,         /* KDDI IMSI Status */
  UIM_CDMA_ME_DOWNLOADABLE_DATA, /* KDDI ME Downloadable Data */
  UIM_CDMA_ME_SETTING_DATA,     /* KDDI ME Setting Date */
  UIM_CDMA_ME_USER_DATA,        /* KDDI User Data */
  UIM_CDMA_RESERVED2,           /* KDDI Reserved */
  UIM_CDMA_RESERVED1,           /* KDDI Reserved */
  UIM_CDMA_UIM_SVC_TABLE,       /* KDDI UIM Service Table */

  /* (0x1)64 => 90 */
  UIM_CDMA_UIM_FEATURES,        /* KDDI UIM Features */
  UIM_CDMA_UIM_VERSION,         /* KDDI UIM Version */
  UIM_CDMA_HRPD_HRPDUPP,        /* HRPD User Profile Parameters */
  UIM_CDMA_SF_EUIM_ID,          /* Short Form EUIMID */

  /* EFs in DF GSM */
  /* (0x2)00 => 0 */
  UIM_GSM_LP          = (UIM_GSM_EF_START << 8) ,  /* Language Preference */
  UIM_GSM_IMSI,            /* IMSI */
  UIM_GSM_KC,              /* Ciphering Key Kc */
  UIM_GSM_PLMN,            /* PLMN selector */
  UIM_GSM_HPLMN,           /* HPLMN search period */
  UIM_GSM_ACM_MAX,         /* ACM Maximum value */
  UIM_GSM_SST,             /* SIM Service table */
  UIM_GSM_ACM,             /* Accumulated call meter */
  UIM_GSM_GID1,            /* Group Identifier Level 1 */
  UIM_GSM_GID2,            /* Group Identifier Level 2 */

  /* (0x2)0A => 10 */
  UIM_GSM_SPN,             /* Service Provider Name */
  UIM_GSM_PUCT,            /* Price Per Unit and currency table */
  UIM_GSM_CBMI,            /* Cell broadcast message identifier selection */
  UIM_GSM_BCCH,            /* Broadcast control channels */
  UIM_GSM_ACC,             /* Access control class */
  UIM_GSM_FPLMN,           /* Forbidden PLMNs */
  UIM_GSM_LOCI,            /* Location information */
  UIM_GSM_AD,              /* Administrative data */
  UIM_GSM_PHASE,           /* Phase identification */
  UIM_GSM_VGCS,            /* Voice Group Call service */

  /* (0x2)14 => 20 */
  UIM_GSM_VGCSS,           /* Voice Group Call service status */
  UIM_GSM_VBS,             /* Voice Broadcast service  */
  UIM_GSM_VBSS,            /* Voice Broadcast service status */
  UIM_GSM_EMLPP,           /* Enhanced multi level pre-emption and priority */
  UIM_GSM_AAEM,            /* Automatic Answer for eMLPP service */
  UIM_GSM_CBMID,           /* Cell Broadcast Message id for data dload */
  UIM_GSM_ECC,             /* Emergency Call Codes */
  UIM_GSM_CBMIR,           /* Cell Broadcast Message id range selection */
  UIM_GSM_DCK,             /* De-personalization control keys */
  UIM_GSM_CNL,             /* Co-operative network list */

  /* (0x2)1E => 30 */
  UIM_GSM_NIA,             /* Network's indication of alerting */
  UIM_GSM_KCGPRS,          /* GPRS ciphering key */
  UIM_GSM_LOCIGPRS,        /* GPRS location information */
  UIM_GSM_SUME,            /* Setup Menu elements */
  UIM_GSM_PLMNWACT,        /* PLMN Selector with Access technology */
  UIM_GSM_OPLMNWACT,       /* Operator controlled PLMNWACT */
  UIM_GSM_HPLMNACT,        /* HPLNMN Access technology */
  UIM_GSM_CPBCCH,          /* CPBCCH information */
  UIM_GSM_INVSCAN,         /* Investigation PLMN Scan */
  UIM_GSM_RPLMNAT,         /* RPLMN  Last used Access Technology */

  /* (0x2)28 => 40 */
  UIM_GSM_VMWI,            /* CPHS: Voice Mail Waiting Indicator */
  UIM_GSM_SVC_STR_TBL,     /* CPHS: Service String Table */
  UIM_GSM_CFF,             /* CPHS: Call Forwarding Flag */
  UIM_GSM_ONS,             /* CPHS: Operator Name String */
  UIM_GSM_CSP,             /* CPHS: Customer Service Profile */
  UIM_GSM_CPHSI,           /* CPHS: CPHS Information */
  UIM_GSM_MN,              /* CPHS: Mailbox Number */
  UIM_GSM_PNN,             /* PLMN Network Name */
  UIM_GSM_OPL,             /* Operator PLMN List  */

/* EFs at SoLSA */

  UIM_GSM_SAI,             /* SoLSA access indicator */

  /* (0x2)32 => 50 */
  UIM_GSM_SLL,             /* SoLSA LSA list */

/* EFs at MExE Level */
  UIM_GSM_MEXE_ST,         /* MExE Service table */
  UIM_GSM_ORPK,            /* Operator Root Public Key */
  UIM_GSM_ARPK,            /* Administrator Root Public Key */
  UIM_GSM_TPRPK,           /* Third party Root public key */
  UIM_GSM_MBDN,            /* Mail Box Dialing Number*/
  UIM_GSM_EXT6,            /* Extension 6*/
  UIM_GSM_MBI,             /* Mail Box Identifier*/
  UIM_GSM_MWIS,            /* Message Wating Indication Status*/
  UIM_GSM_EXT1,            /* Extension 1*/

  /* (0x2)3C => 60 */
  UIM_GSM_SPDI,            /* Service Provider Display Information*/
  UIM_GSM_CFIS,            /* Call Forwarding Indicator Status */

/* image file */           /* not sure where they are used */
  UIM_GSM_IMG,
  UIM_GSM_IMG1INST1,
  UIM_GSM_IMG1INST2,
  UIM_GSM_IMG1INST3,
  UIM_GSM_IMG2INST1,
  UIM_GSM_IMG2INST2,
  UIM_GSM_IMG2INST3,
  UIM_GSM_IMG3INST1,

  /* (0x2)46 => 70 */
  UIM_GSM_IMG3INST2,
  UIM_GSM_IMG3INST3,

/* EFs at the telecom DF in GSM and CDMA */
  /* (0x3)00 => 0 */
  UIM_TELECOM_ADN     = (UIM_TELECOM_EF_START << 8),/* Abbrev Dialing Numbers */
  UIM_TELECOM_FDN,         /* Fixed dialling numbers */
  UIM_TELECOM_SMS,         /* Short messages */
  UIM_TELECOM_CCP,         /* Capability Configuration Parameters */
  UIM_TELECOM_ECCP,        /* Extended CCP */
  UIM_TELECOM_MSISDN,      /* MSISDN */
  UIM_TELECOM_SMSP,        /* SMS parameters */
  UIM_TELECOM_SMSS,        /* SMS Status */
  UIM_TELECOM_LND,         /* Last number dialled */
  UIM_TELECOM_SDN,         /* Service Dialling numbers */

  /* (0x3)0A => 10 */
  UIM_TELECOM_EXT1,        /* Extension 1 */
  UIM_TELECOM_EXT2,        /* Extension 2 */
  UIM_TELECOM_EXT3,        /* Extension 3 */
  UIM_TELECOM_BDN,         /* Barred Dialing Numbers */
  UIM_TELECOM_EXT4,        /* Extension 4 */
  UIM_TELECOM_SMSR,        /* SMS reports */
  UIM_TELECOM_CMI,         /* Comparison Method Information */
  UIM_TELECOM_SUME,        /* Setup Menu elements */
  UIM_TELECOM_ARR,         /* Access Rule reference */

  /* EFs at DF GRAPHICS under DF Telecom */
  UIM_IMAGE,               /* Image instance data files */

  /* EFs at the DF PHONEBOOK under  DF Telecom */
  /* (0x3)14 => 20 */
  UIM_TELECOM_PBR,         /* Phone book reference file */
  UIM_TELECOM_PSC,         /* Phone book synchronization center */
  UIM_TELECOM_CC,          /* Change counter */
  UIM_TELECOM_PUID,        /* Previous Unique Identifier */

  /* EFs of USIM ADF */
  /* (0x4)00 => 0 */
  UIM_USIM_LI             = ( UIM_USIM_EF_START << 8), /* Language Indication */
  UIM_USIM_IMSI,           /* IMSI */
  UIM_USIM_KEYS,           /* Ciphering and Integrity keys */
  UIM_USIM_KEYSPS,         /* C and I keys for packet switched domain */
  UIM_USIM_PLMNWACT,       /* User controlled PLMN selector with access tech */
  UIM_USIM_UPLMNSEL,       /* UPLMN selector */
  UIM_USIM_HPLMN,          /* HPLMN search period */
  UIM_USIM_ACM_MAX,        /* ACM maximum value */
  UIM_USIM_UST,            /* USIM Service table */
  UIM_USIM_ACM,            /* Accumulated Call meter */

  /* (0x4)0A => 10 */
  UIM_USIM_GID1,           /* Group Identifier Level  */
  UIM_USIM_GID2,           /* Group Identifier Level 2 */
  UIM_USIM_SPN,            /* Service Provider Name */
  UIM_USIM_PUCT,           /* Price Per Unit and Currency table */
  UIM_USIM_CBMI,           /* Cell Broadcast Message identifier selection */
  UIM_USIM_ACC,            /* Access control class */
  UIM_USIM_FPLMN,          /* Forbidden PLMNs */
  UIM_USIM_LOCI,           /* Location information */
  UIM_USIM_AD,             /* Administrative data */
  UIM_USIM_CBMID,          /* Cell Broadcast msg identifier for data download */

  /* (0x4)14 => 20 */
  UIM_USIM_ECC,            /* Emergency call codes */
  UIM_USIM_CBMIR,          /* Cell broadcast msg identifier range selection */
  UIM_USIM_PSLOCI,         /* Packet switched location information */
  UIM_USIM_FDN,            /* Fixed dialling numbers */
  UIM_USIM_SMS,            /* Short messages */
  UIM_USIM_MSISDN,         /* MSISDN */
  UIM_USIM_SMSP,           /* SMS parameters */
  UIM_USIM_SMSS,           /* SMS Status */
  UIM_USIM_SDN,            /* Service dialling numbers */
  UIM_USIM_EXT2,           /* Extension 2 */

  /* (0x4)1E => 30 */
  UIM_USIM_EXT3,           /* Extension 3 */
  UIM_USIM_SMSR,           /* SMS reports */
  UIM_USIM_ICI,            /* Incoming call information */
  UIM_USIM_OCI,            /* Outgoing call information */
  UIM_USIM_ICT,           /* Incoming call timer */
  UIM_USIM_OCT,            /* Outgoing call timer */
  UIM_USIM_EXT5,           /* Extension 5 */
  UIM_USIM_CCP2,           /* Capability Configuration Parameters 2 */
  UIM_USIM_EMLPP,          /* Enhanced Multi Level Precedence and Priority */
  UIM_USIM_AAEM,           /* Automatic answer for eMLPP service */

  /* (0x4)28 => 40 */
  UIM_USIM_GMSI,           /* Group identity */
  UIM_USIM_HIDDENKEY,      /* key for hidden phonebook entries */
  UIM_USIM_BDN,            /* Barred dialling numbers */
  UIM_USIM_EXT4,           /* Extension 4 */
  UIM_USIM_CMI,            /* Comparison Method information */
  UIM_USIM_EST,            /* Enabled services table */
  UIM_USIM_ACL,            /* Access Point Name Control List */
  UIM_USIM_DCK,            /* De-personalization Control Keys */
  UIM_USIM_CNL,            /* Co-operative network list */
  UIM_USIM_START_HFN,      /* Init values for Hyper-frame number */

  /* (0x4)32 => 50 */
  UIM_USIM_THRESHOLD,      /* Max value of START */
  UIM_USIM_OPLMNWACT,      /* Operator controlled PLMN sel with access tech */
  UIM_USIM_OPLMNSEL,       /* OPLMN selector */
  UIM_USIM_HPLMNWACT,      /* HPLMN selector with access tech */
  UIM_USIM_ARR,            /* Access Rule reference */
  UIM_USIM_RPLMNACT,       /* RPLMN last used access tech */
  UIM_USIM_NETPAR,         /* Network parameters */
  UIM_USIM_VMWI,           /* CPHS: Voice Mail Waiting Indicator */
  UIM_USIM_SVC_STR_TBL,    /* CPHS: Service String Table */
  UIM_USIM_CFF,            /* CPHS: Call Forwarding Flag */

  /* (0x4)3C => 60 */
  UIM_USIM_ONS,            /* CPHS: Operator Name String */
  UIM_USIM_CSP,            /* CPHS: Customer Service Profile */
  UIM_USIM_CPHSI,          /* CPHS: CPHS Information */
  UIM_USIM_MN,             /* CPHS: Mailbox Number */
  UIM_USIM_PNN,            /* PLMN Netowrk Operator Name */
  UIM_USIM_OPL,            /* Operator PLMN List */

/* EFs of ORANGE DF under USIM ADF */
  UIM_USIM_ORANGE_DFS,     /* Dynamic Flags Status */
  UIM_USIM_ORANGE_D2FS,    /* Dynamic2 Flag Setting */
  UIM_USIM_ORANGE_CSP2,    /* Customer Service Profile Line2*/
  UIM_USIM_ORANGE_PARAMS,  /* EF Parmams - Welcome Message */
/*EFs at DF PHONEBOOK under USIM ADF */

  /* (0x4)46 => 70 */
  UIM_USIM_PBR,            /* Phone book reference file */
  UIM_USIM_PSC,            /* Phone book synchronization center */
  UIM_USIM_CC,             /* Change counter */
  UIM_USIM_PUID,           /* Previous Unique Identifier */

/*EFs at DF GSM under USIM ADF */

  UIM_USIM_KC,             /* GSM ciphering key Kc */
  UIM_USIM_KCGPRS,         /* GPRS ciphering key */
  UIM_USIM_CPBCCH,         /* CPBCCH information */
  UIM_USIM_INVSCAN,        /* Investigation scan */

/*EFs at DF MEXe under USIM ADF */
  UIM_USIM_MEXE_ST,        /* MExE Service table */
  UIM_USIM_ORPK,           /* Operator Root Public Key */
  UIM_USIM_ARPK,           /* Administrator Root Public Key */

  /* (0x4)50 => 80 */
  UIM_USIM_TPRPK,          /* Third party Root public key */
  UIM_USIM_MBDN,           /* Mailbox Dialing Number*/
  UIM_USIM_EXT6,           /* Extension 6 */
  UIM_USIM_MBI,            /* Mailbox Identifier*/
  UIM_USIM_MWIS,           /* Message Waiting Indication Status*/
  UIM_USIM_SPDI,           /* Service Provider Display Information*/
  UIM_USIM_SPT_TABLE,      /* Cingular TST*/
  UIM_USIM_EHPLMN,         /* Equivalent HPLMN */
  UIM_USIM_CFIS,           /* Call Forwarding Indicator Status */
    /* EFs related to Bootstrapping under USIM ADF */
  UIM_USIM_GBABP,          /* GBA BootStrapping Parameters */

  /* (0x4)5A => 90 */
  UIM_USIM_GBANL,          /* GBA NAF Derivation List Associated with
                              NAF Derivation Proc*/
/* EFs related to MBMS under USIM ADF */
  UIM_USIM_MSK,            /* MBMS Service Key */
  UIM_USIM_MUK,            /* MBMS User Key */
  /* EFs for MMS Notification */
  UIM_USIM_MMSN,           /* MMS Notification */
  UIM_USIM_MMSICP,         /* MMS Issuer connectivity parameters */
  UIM_USIM_MMSUP,          /* MMS User Preferences */
  UIM_USIM_MMSUCP,         /* MMS User connectivity parameters */
  UIM_USIM_EXT8,           /* Extension 8 */
  /* Efs for PKCS #15 support */
  UIM_PKCS15_ODF,          /* Object Directory File */
  UIM_PKCS15_TI,           /* Token Information File */

  /* (0x4)64  => 100 */
  UIM_PKCS15_US,           /* Unused space Information File */

/* EFs of DCS1800 DF */
  UIM_DCS1800_IMSI        = (UIM_DCS1800_EF_START << 8) ,  /* Imsi */
  UIM_DCS1800_KC,          /* Ciphering Key Kc */
  UIM_DCS1800_PLMN,        /* PLMN selector */
  UIM_DCS1800_SST,         /* SIM Service table */
  UIM_DCS1800_BCCH,        /* Broadcast control channels */
  UIM_DCS1800_ACC,         /* Access control class */
  UIM_DCS1800_FPLMN,       /* Forbidden PLMNs */
  UIM_DCS1800_LOCI,        /* Location information */
  UIM_DCS1800_AD,          /* Administrative data */

/* EFs given by path */

  /* (0xF)00 => 0 */
  UIM_EF_BY_PATH      = (UIM_EF_PATH_START << 8),  /* File selection by path */

/* EFs selected by path at the telecom DF in GSM and CDMA */
  UIM_TELECOM_BY_PATH_IAP,         /* Index Administration File */
  UIM_TELECOM_BY_PATH_PBC,         /* Phone Book Control */
  UIM_TELECOM_BY_PATH_GRP,         /* Grouping File */

  UIM_TELECOM_BY_PATH_AAS,         /* Additional Number Alpha String */
  UIM_TELECOM_BY_PATH_GAS,         /* Grouping Information Alpha String */
  UIM_TELECOM_BY_PATH_ANR,         /* Additional Number */
  UIM_TELECOM_BY_PATH_SNE,         /* Secondary Name Entry */
  UIM_TELECOM_BY_PATH_EMAIL,       /* Email */
  UIM_TELECOM_BY_PATH_ANRA,        /* Additional Number A */

  /* (0xF)0A => 10 */
  UIM_TELECOM_BY_PATH_ANRB,        /* Additional Number B */
  UIM_TELECOM_BY_PATH_ANRC,        /* Additional Number C */
  UIM_TELECOM_BY_PATH_ANR1,        /* Additional Number   */
  UIM_TELECOM_BY_PATH_ANRA1,       /* Additional Number A1 */
  UIM_TELECOM_BY_PATH_ANRB1,       /* Additional Number B1 */
  UIM_TELECOM_BY_PATH_ANRC1,       /* Additional Number C1 */
  UIM_TELECOM_BY_PATH_ADN1,        /* Abbreviated Dialing Numbers 1 */
  UIM_TELECOM_BY_PATH_PBC1,        /* Phone Book Control 1 */
  UIM_TELECOM_BY_PATH_GRP1,        /* Grouping File 1 */
  UIM_TELECOM_BY_PATH_SNE1,        /* Secondary Name Entry 1 */

  /* (0xF)14 => 20 */
  UIM_TELECOM_BY_PATH_UID1,        /* Unique Identifier 1 */
  UIM_TELECOM_BY_PATH_EMAIL1,      /* Email 1 */
  UIM_TELECOM_BY_PATH_IAP1,        /* Index Administration File 1 */
  UIM_TELECOM_BY_PATH_ADN,         /* Abbrev Dialing Numbers */
  UIM_TELECOM_BY_PATH_EXT1,        /* Extension 1 */
  UIM_TELECOM_BY_PATH_EXT2,        /* Extension 2 */
  UIM_TELECOM_BY_PATH_UID,         /* Unique Identifier */
  UIM_TELECOM_BY_PATH_CCP1,        /* Capability Configuration parameters 1 */

  // 1000 PBM Support starts here
  UIM_TELECOM_BY_PATH_ADN2,        /* Abbrev Dialing Numbers 3 */
  UIM_TELECOM_BY_PATH_IAP2,        /* Index Administration File 3 */

  /* (0xF)1E => 30 */
  UIM_TELECOM_BY_PATH_PBC2,        /* Phone Book Control 3 */
  UIM_TELECOM_BY_PATH_GRP2,        /* Grouping File 3 */
  UIM_TELECOM_BY_PATH_SNE2,        /* Secondary Name Entry 3 */
  UIM_TELECOM_BY_PATH_UID2,        /* Unique Identifier 3 */
  UIM_TELECOM_BY_PATH_EMAIL2,      /* Email 3 */
  UIM_TELECOM_BY_PATH_ANR2,        /* ANR 3 */
  UIM_TELECOM_BY_PATH_ANRA2,
  UIM_TELECOM_BY_PATH_ANRB2,
  UIM_TELECOM_BY_PATH_ANRC2,
  UIM_TELECOM_BY_PATH_ADN3,        /* Abbrev Dialing Numbers 4 */

  /* (0xF)28 => 40 */
  UIM_TELECOM_BY_PATH_IAP3,        /* Index Administration File 4 */
  UIM_TELECOM_BY_PATH_PBC3,        /* Phone Book Control 4 */
  UIM_TELECOM_BY_PATH_GRP3,        /* Grouping File 4 */
  UIM_TELECOM_BY_PATH_SNE3,        /* Secondary Name Entry 4 */
  UIM_TELECOM_BY_PATH_UID3,        /* Unique Identifier 4 */
  UIM_TELECOM_BY_PATH_EMAIL3,      /* Email 4 */
  UIM_TELECOM_BY_PATH_ANR3,        /* ANR 4 */
  UIM_TELECOM_BY_PATH_ANRA3,
  UIM_TELECOM_BY_PATH_ANRB3,
  UIM_TELECOM_BY_PATH_ANRC3,
  // 1000 USIM Phonebook ends at UIM_TELECOM_BY_PATH_ANRC3

/* EFs selected by path of USIM ADF */
  /* (0xF)32 => 50 */
  UIM_USIM_ADN,            /* Abbreviated Dialing Numbers */
  UIM_USIM_IAP,            /* Index Administration File */
  UIM_USIM_PBC,            /* Phone Book Control */
  UIM_USIM_EXT1,           /* Extension 1 */
  UIM_USIM_GRP,            /* Grouping File */
  UIM_USIM_AAS,            /* Additional Number Alpha String */
  UIM_USIM_AAS1,           /* Additional Number Alpha String 1 */
  UIM_USIM_GAS,            /* Grouping Information Alpha String */
  UIM_USIM_GAS1,           /* Grouping Information Alpha String */
  UIM_USIM_ANR,            /* Additional Number */

  /* (0xF)3C => 60 */
  UIM_USIM_SNE,            /* Secondary Name Entry */
  UIM_USIM_EMAIL,          /* Email */
  UIM_USIM_ANRA,           /* Additional Number A */
  UIM_USIM_ANRB,           /* Additional Number B */
  UIM_USIM_ANRC,           /* Additional Number C */
  UIM_USIM_ANR1,           /* Additional Number 1 */
  UIM_USIM_ANRA1,          /* Additional Number A1 */
  UIM_USIM_ANRB1,          /* Additional Number B1 */
  UIM_USIM_ANRC1,          /* Additional Number C1 */
  UIM_USIM_ADN1,           /* Abbreviated Dialing Numbers 1 */

  /* (0xF)46 => 70 */
  UIM_USIM_PBC1,           /* Phone Book Control 1 */
  UIM_USIM_GRP1,           /* Grouping File 1 */
  UIM_USIM_SNE1,           /* Secondary Name Entry 1 */
  UIM_USIM_UID1,           /* Unique Identifier 1 */
  UIM_USIM_EMAIL1,         /* Email 1 */
  UIM_USIM_IAP1,           /* Index Administration File 1 */
  UIM_USIM_ACT_HPLMN,       /*Acting HPLMN*/
  UIM_USIM_RAT,            /* Radio Access Technology Indicator */
  UIM_USIM_UID,            /* Unique Identifier */
  UIM_USIM_CCP1,           /* Capability Configuration parameters 1 */

  /* (0xF)50 => 80 */
  // USIM 1000 support starts here
  UIM_USIM_ANR2,           /* Additional Number 3 */
  UIM_USIM_ANRA2,          /* Additional Number A3*/
  UIM_USIM_ANRB2,          /* Additional Number B3 */
  UIM_USIM_ANRC2,          /* Additional Number C3 */
  UIM_USIM_ADN2,           /* Abbreviated Dialing Numbers 3 */
  UIM_USIM_PBC2,           /* Phone Book Control 3*/
  UIM_USIM_GRP2,           /* Grouping File 3 */
  UIM_USIM_SNE2,           /* Secondary Name Entry 3 */
  UIM_USIM_UID2,           /* Unique Identifier 3 */
  UIM_USIM_EMAIL2,         /* Email 3 */

  /* (0xF)5A => 90 */
  UIM_USIM_IAP2,           /* Index Administration File 3 */
  UIM_USIM_ANR3,           /* Additional Number 4 */
  UIM_USIM_ANRA3,          /* Additional Number A4 */
  UIM_USIM_ANRB3,          /* Additional Number B4 */
  UIM_USIM_ANRC3,          /* Additional Number C4 */
  UIM_USIM_ADN3,           /* Abbreviated Dialing Numbers 4 */
  UIM_USIM_PBC3,           /* Phone Book Control 4 */
  UIM_USIM_GRP3,           /* Grouping File 4 */
  UIM_USIM_SNE3,           /* Secondary Name Entry 4 */
  UIM_USIM_UID3,           /* Unique Identifier 4 */

  /* (0xF)64 => 100 */
  UIM_USIM_EMAIL3,         /* Email 4 */
  UIM_USIM_IAP3,           /* Index Administration File 4 */
  // 1000 PBM support ends at UIM_USIM_IAP3
  /* EFs for PKCS #15 */
  UIM_PKCS15_PRKDF,        /* Private Key Directory File */
  UIM_PKCS15_PUKDF,        /* Public Key Directory File */
  UIM_PKCS15_SKDF,         /* Secret Key Directory File */
  UIM_PKCS15_CDF,          /* Certificate Directory File */
  UIM_PKCS15_DODF,         /* Data Object Directory File */
  UIM_PKCS15_AODF,         /* Authentication Object Directory File */

  /* EFs of WIM DF */
  UIM_WIM_ODF           = (UIM_WIM_EF_START << 8), /* WIM ODF */
  UIM_WIM_TOKEN_INFO,      /* Generic token info and capabilities */
  UIM_WIM_UNUSED_SPACE,    /* Unused Space */

  /* EFs of ORANGE DF */
  UIM_ORANGE_DFS        = (UIM_ORANGE_EF_START << 8), /* Dynamic Flags Status */
  UIM_ORANGE_D2FS,         /* Dynamic2 Flag Setting */
  UIM_ORANGE_CSP2,         /* Customer Service Profile Line2*/
  UIM_ORANGE_PARAMS,       /* Welcome Message */

  UIM_CINGULAR_ACT_HPLMN      = (UIM_CINGULAR_EF_START << 8), /* Dynamic Flags Status */ /*Acting HPLMN*/
  UIM_CINGULAR_SPT_TABLE,      /*Support Table*/

  /* EFs of ISIM ADF */
  UIM_ISIM_IMPI             = ( UIM_ISIM_EF_START << 8), /* IMS private user identity */
  UIM_ISIM_DOMAIN,          /* Home Network Domain Name */
  UIM_ISIM_IMPU,            /* IMS public user identity */
  UIM_ISIM_ARR,             /* Access Rule Reference */
  UIM_ISIM_KEYS,            /* Ciphering and Integrity Keys for IMS */
  UIM_ISIM_AD,              /* Administrative Data */

  UIM_HZI_HZ                = (UIM_HZI_EF_START << 8),
  UIM_HZI_CACHE1,              /*UHZI-Cache1*/
  UIM_HZI_CACHE2,              /*UHZI-Cache2*/
  UIM_HZI_CACHE3,              /*UHZI-Cache3*/
  UIM_HZI_CACHE4,              /*UHZI-Cache4*/
  UIM_HZI_SUBSCRIBED_LAC_CI_1, /*UHZI-Subscribed_Lac_CI_1*/
  UIM_HZI_SUBSCRIBED_LAC_CI_2, /*UHZI-Subscribed_Lac_CI_2*/
  UIM_HZI_SUBSCRIBED_LAC_CI_3, /*UHZI-Subscribed_Lac_CI_3*/
  UIM_HZI_SUBSCRIBED_LAC_CI_4, /*UHZI-Subscribed_Lac_CI_4*/
  UIM_HZI_TAGS,                /*UHZI- Tags*/

  UIM_HZI_UHZI_SETTINGS,       /*UHZI- Settings*/

  /* EF under RAN DF in MediaFlo Application */
  UIM_MFLO_RAN_RFR    = (UIM_MFLO_EF_START << 8),   /* RAN File Revision */
  UIM_MFLO_RAN_RF,                                  /* Radio Frequency */
  /* EF under MFAPP DF in MediaFlo Application */
  UIM_MFLO_MFAPP_MFR,                               /* MediaFLO application File Revision */
  UIM_MFLO_MFAPP_BCSID,                             /* Billing and Customer Service provider ID */
  UIM_MFLO_MFAPP_MFT,                               /* MediaFLO Feature Table */
  UIM_MFLO_MFAPP_SUBTYPE,                           /* Subcriber Type */
  UIM_MFLO_MFAPP_SUBPROF,                           /* Subcriber Profile */
  UIM_MFLO_MFAPP_UTSD,                              /* Usage Tracking Service Data */
  UIM_MFLO_MFAPP_UIP,                               /* UI Preference */
  UIM_MFLO_MFAPP_PCSET,                             /* Parental Control Setting */

  UIM_MFLO_MFAPP_MCHSET,                            /* Multi-presentation view Channel Setting */
  UIM_MFLO_MFAPP_RFUD,                              /* Reserved for Future User Data */

  UIM_NO_SUCH_ITEM         /* No such item */
} uim_items_enum_type;

typedef enum {
  UIM_MFLO_UCAST_MSG_FIRST_BLOCK  = 0x00,
  UIM_MFLO_UCAST_MSG_NEXT_BLOCK   = 0x01,
  UIM_MFLO_UCAST_MSG_LAST_BLOCK   = 0x10,
  UIM_MFLO_UCAST_MSG_SINGLE_BLOCK = 0x11
  // Any other value is invalid
}uim_mflo_ucast_msg_enum_type;

/* For backward compatibility, since the items have been re-named,
 * we are defining them to be equal to the new items
 */
#define UIM_TELECOM_IAP UIM_TELECOM_BY_PATH_IAP         /* Index Administration File */
#define UIM_TELECOM_PBC UIM_TELECOM_BY_PATH_PBC         /* Phone Book Control */
#define UIM_TELECOM_GRP UIM_TELECOM_BY_PATH_GRP         /* Grouping File */
#define UIM_TELECOM_AAS UIM_TELECOM_BY_PATH_AAS         /* Additional Number Alpha String */
#define UIM_TELECOM_GAS UIM_TELECOM_BY_PATH_GAS
#define UIM_TELECOM_ANR UIM_TELECOM_BY_PATH_ANR
#define UIM_TELECOM_SNE UIM_TELECOM_BY_PATH_SNE          /* Secondary Name Entry */
#define UIM_TELECOM_EMAIL UIM_TELECOM_BY_PATH_EMAIL      /* Email */
#define UIM_TELECOM_ANRA UIM_TELECOM_BY_PATH_ANRA        /* Additional Number A */
#define UIM_TELECOM_ANRB UIM_TELECOM_BY_PATH_ANRB        /* Additional Number B */
#define UIM_TELECOM_ANRC UIM_TELECOM_BY_PATH_ANRC        /* Additional Number C */
#define UIM_TELECOM_ANR1 UIM_TELECOM_BY_PATH_ANR1        /* Additional Number   */
#define UIM_TELECOM_ANRA1 UIM_TELECOM_BY_PATH_ANRA1      /* Additional Number A1 */
#define UIM_TELECOM_ANRB1 UIM_TELECOM_BY_PATH_ANRB1      /* Additional Number B1 */
#define UIM_TELECOM_ANRC1 UIM_TELECOM_BY_PATH_ANRC1      /* Additional Number C */
#define UIM_TELECOM_ADN1 UIM_TELECOM_BY_PATH_ADN1        /* Abbreviated Dialing Numbers 1 */
#define UIM_TELECOM_PBC1 UIM_TELECOM_BY_PATH_PBC1        /* Phone Book Control 1 */
#define UIM_TELECOM_GRP1 UIM_TELECOM_BY_PATH_GRP1        /* Grouping File 1 */
#define UIM_TELECOM_SNE1 UIM_TELECOM_BY_PATH_SNE1        /* Secondary Name Entry 1 */
#define UIM_TELECOM_UID1 UIM_TELECOM_BY_PATH_UID1        /* Unique Identifier 1 */
#define UIM_TELECOM_EMAIL1 UIM_TELECOM_BY_PATH_EMAIL1    /* Email 1 */
#define UIM_TELECOM_IAP1 UIM_TELECOM_BY_PATH_IAP1        /* Index Administration File 1 */
#define UIM_TELECOM_CCP1 UIM_TELECOM_BY_PATH_CCP1        /* Capability Configuration parameters 1 */
#define UIM_TELECOM_UID  UIM_TELECOM_BY_PATH_UID         /* Unique Identifier */

#ifdef FEATURE_USIM_1000_PBM
#define UIM_TELECOM_ADN2   UIM_TELECOM_BY_PATH_ADN2      /* Abbrev Dialing Numbers 3 */
#define UIM_TELECOM_IAP2   UIM_TELECOM_BY_PATH_IAP2      /* Index Administration File 3 */
#define UIM_TELECOM_PBC2   UIM_TELECOM_BY_PATH_PBC2      /* Phone Book Control 3 */
#define UIM_TELECOM_GRP2   UIM_TELECOM_BY_PATH_GRP2      /* Grouping File 3 */
#define UIM_TELECOM_SNE2   UIM_TELECOM_BY_PATH_SNE2      /* Secondary Name Entry 3 */
#define UIM_TELECOM_UID2   UIM_TELECOM_BY_PATH_UID2      /* Unique Identifier 3 */
#define UIM_TELECOM_EMAIL2 UIM_TELECOM_BY_PATH_EMAIL2    /* Email 3 */
#define UIM_TELECOM_ANR2   UIM_TELECOM_BY_PATH_ANR2      /* ANR 3 */
#define UIM_TELECOM_ANRA2  UIM_TELECOM_BY_PATH_ANRA2
#define UIM_TELECOM_ANRB2  UIM_TELECOM_BY_PATH_ANRB2
#define UIM_TELECOM_ANRC2  UIM_TELECOM_BY_PATH_ANRC2

#define UIM_TELECOM_ADN3   UIM_TELECOM_BY_PATH_ADN3       /* Abbrev Dialing Numbers 3 */
#define UIM_TELECOM_IAP3   UIM_TELECOM_BY_PATH_IAP3       /* Index Administration File 3 */
#define UIM_TELECOM_PBC3   UIM_TELECOM_BY_PATH_PBC3       /* Phone Book Control 3 */
#define UIM_TELECOM_GRP3   UIM_TELECOM_BY_PATH_GRP3       /* Grouping File 3 */
#define UIM_TELECOM_SNE3   UIM_TELECOM_BY_PATH_SNE3       /* Secondary Name Entry 3 */
#define UIM_TELECOM_UID3   UIM_TELECOM_BY_PATH_UID3       /* Unique Identifier 3 */
#define UIM_TELECOM_EMAIL3 UIM_TELECOM_BY_PATH_EMAIL3     /* Email 3 */
#define UIM_TELECOM_ANR3   UIM_TELECOM_BY_PATH_ANR3       /* ANR 3 */
#define UIM_TELECOM_ANRA3  UIM_TELECOM_BY_PATH_ANRA3
#define UIM_TELECOM_ANRB3  UIM_TELECOM_BY_PATH_ANRB3
#define UIM_TELECOM_ANRC3  UIM_TELECOM_BY_PATH_ANRC3
#endif /*FEATURE_USIM_1000_PBM*/

typedef enum {
  UIM_NO_SUCH_STATUS_S,          /* No such status */
  UIM_INVALID_S,                 /* Recd the command in an invalid State */
  UIM_RECEIVED_S,                /* Recd the command */
  UIM_UNINITIALIZED_S,           /* UIM is not initialized yet */
  UIM_POWERED_DOWN_S,            /* UIM is powered down */
  UIM_CLK_OFF_S,                 /* UIM does not have a clock */
  UIM_INITIALIZED_S,             /* UIM is initialized */
  UIM_ERR_S,                     /* UIM is faulty */
  UIM_PROTOCOL_ERR_S,            /* Protocol in the command is invalid */
  UIM_POLL_ERR_S,               /* Poll error */
  UIM_RESET_INIT_S,              /* Internal reset is about to start */
  UIM_RESET_COMPLETE_S,          /* Internal reset is finished */
  UIM_NO_ATR_RECEIVED_WITH_MAX_VOLTAGE_S,
  UIM_NO_ATR_RECEIVED_AFTER_INT_RESET_S,
  UIM_CORRUPT_ATR_RCVD_MAX_TIMES_S,
  UIM_PPS_TIMED_OUT_MAX_TIMES_S,
  UIM_VOLTAGE_MISMATCH_S,
  UIM_INTERNAL_CMD_TIMED_OUT_AFTER_PPS_S,
  UIM_INT_CMD_ERR_EXCEED_MAX_ATTEMPTS_S,
  UIM_MAXED_PARITY_ERROR_S,
  UIM_MAXED_RX_BREAK_ERROR_S,
  UIM_MAXED_OVERRUN_ERROR_S,
  UIM_TRANSACTION_TIMER_EXPIRED_S,
  UIM_POWER_DOWN_CMD_NOTIFICATION_S,
  UIM_INT_CMD_ERR_IN_PASSIVE_MODE_S,
  UIM_CMD_TIMED_OUT_IN_PASSIVE_MODE_S,
  UIM_MAX_PARITY_IN_PASSIVE_S,
  UIM_MAX_RXBRK_IN_PASSIVE_S,
  UIM_MAX_OVERRUN_IN_PASSIVE_S,
  UIM_MAXIMUM_S
} uim_status_type;

typedef enum {
  UIM_FLUSH_ONLY,        /* Flush only this command from the Q */
  UIM_FLUSH_ALL          /* Flush all the commands in the q */
} uim_flush_type;

/* Indicate command handling options */
typedef enum {
  UIM_NONE = 0x00,    /* Disabled */
  UIM_AUTH = 0x01,    /* uim power-down bit for Auth task */
  UIM_MC   = 0x02,    /* uim power-down bit for MC task */
  UIM_PROACTIVE_UIM = 0x4, /* power up bit if the UIM is pro active */
  UIM_UI   = 0x8,     /* uim power down bit for UI task */
  UIM_USIM_SESSION = 0x10, /* bit indicating a USIM session */
  UIM_SESSION = 0x20, /* any module that wants to execute a series of cmds
                         without powering down between commands */
  UIM_TC   = 0x40,    /* uim powerdown bit for traffic channel */
  UIM_CDMA_TC = 0x80, /* uim voting for CDMA traffic channel */
  UIM_GW_TC = 0x100,  /* uim voting for GSM/WCDMA traffic channel */
  UIM_PASSIVE = 0x200 /* uim voting for presence detection in passive mode */
} uim_voter_type;

/* Aggregation of Traffic channel voting bits */
#define UIM_CDMA_TRAF_CHAN (UIM_CDMA_TC)
#define UIM_GW_TRAF_CHAN ( (int)UIM_TC | (int)UIM_GW_TC)
#define UIM_TRAF_CHAN ( (int)UIM_CDMA_TRAF_CHAN | UIM_GW_TRAF_CHAN )

/* Type for holding data about proactive UIM */
typedef struct {
  boolean proactive_uim;
  boolean init;
} uim_proactive_uim_data_type;

/* Indicate command handling options */
typedef enum {
  UIM_OPTION_NONE           = 0x0,        /* No options used */
  UIM_OPTION_ALWAYS_RPT     = 0x1,        /* Always report   */
  UIM_OPTION_RECOVERY_CMD   = 0x2        /* Recovery command */
} uim_option_type;

/* Indication for presence of CDMA DF and/or GSM DF. */
typedef struct {
  uim_protocol_type protocol;       /* UICC or GSM protocol */
  boolean cdma_df_present;          /* Presence of CDMA DF */
  boolean gsm_df_present;           /* Presence of GSM  DF */
} uim_dfs_present_type;

/* The common Header for all the commands */
typedef struct {
  cmd_hdr_type          cmd_hdr;       /* command header */
  uim_cmd_name_type     command;       /* The command */
  uim_status_type       status;        /* Status of the R-UIM */
  uim_option_type       options;       /* Indicate command handling options */
  uim_protocol_type     protocol;      /* Protocol of the command */
  uim_slot_type         slot;          /* Slot to execute the command */
  void                  (*rpt_function)( uim_rpt_type * );
                                  /* Pointer to Function to send report */
  byte                  cmd_count;
  uint32                user_data;     /* User defined data */
  uim_channel_type      channel;
} uim_hdr_type;

/* ACCESS_F command */

typedef enum {
  UIM_READ_F     = 0,    /* Read item from UIM */
  UIM_WRITE_F    = 1,    /* Write item to UIM */
  UIM_INC_F      = 2     /* Increment item in UIM */
} uim_access_type;

typedef enum {
  UIM_CURRENT    = 0, /* Current mode */
  UIM_NEXT       = 1, /* access the next record in a record structure */
  UIM_PREVIOUS   = 2, /* access the previous record in a record structure */
  UIM_ABSOLUTE   = 3  /* Absolute mode of record access */
} uim_record_mode_type;

typedef enum {
  UIM_ACTIVATION_OR_RESET  = 0,   /* Activation or reset of an USIM session */
  UIM_TERMINATION = 1    /* Termination of an USIM session */
} uim_session_control_type;

typedef struct {
  byte data[UIM_AID_MAX_NAME_LENGTH];
  byte aid_length;
} uim_aid_type;

typedef struct {
  uim_hdr_type    hdr;            /* Command header */
  uim_items_enum_type item;       /* Item to be accessed */
  uim_access_type access;         /* Type of access, Read or Write */
  uim_record_mode_type rec_mode;  /* Record Mode for record access */
  word            num_bytes;      /* Number of bytes to be read or written or
                                     incremented */
  word            num_bytes_rsp;  /* Number of bytes returned */
  word            num_records_rsp;/* Number of records in the response */
  word            offset;         /* Offset into the EF */
  word            path[UIM_MAX_PATH_ELEMENTS];
                                  /* Path for accessing an item */
  void            *data_ptr;      /* Pointer for data */
  uim_aid_type     aid;  /* AID for UICC */
} uim_access_cmd_type;

typedef struct
{
  uim_hdr_type        hdr;        /* Command header */
  uim_items_enum_type item;       /* Item to be accessed */
  word                path[UIM_MAX_PATH_ELEMENTS];
                                  /* Path for accessing an item */
  word    offset;  /* Offset to write from */
  word    len;     /* Length of data to be written */
  byte   *data;    /* Pointer to data to be written */

  uim_aid_type     aid;  /* AID for UICC */
}uim_write_bin_cmd_type;

typedef struct
{
  uim_hdr_type          hdr;        /* Command header */
  uim_items_enum_type   item;       /* Item to be accessed */
  word                  path[UIM_MAX_PATH_ELEMENTS];
                                  /* Path for accessing an item */
  uim_record_mode_type  recmode; /* mode for record access */
  byte                  rec_num; /* Record number to write to */

  word                  len;  /* length of data to be written */
  byte                 *data;    /* Datat to write */

  uim_aid_type     aid;  /* AID for UICC */
}uim_write_rec_cmd_type;

typedef struct
{
  uim_hdr_type        hdr;        /* Command header */
  uim_items_enum_type item;       /* Item to be accessed */
  word                path[UIM_MAX_PATH_ELEMENTS];
                                  /* Path for accessing an item */
  word    offset;  /* Offset to read from */
  word    len;     /* Length of data to be read */

  uim_aid_type     aid;  /* AID for UICC */
}uim_read_bin_cmd_type;

typedef struct
{
  uim_hdr_type        hdr;        /* Command header */
  uim_items_enum_type item;       /* Item to be accessed */
  word                path[UIM_MAX_PATH_ELEMENTS];
                                  /* Path for accessing an item */
  uim_record_mode_type recmode; /* mode for record access */
  byte                 rec_num; /* Record number to read from */

  uim_aid_type     aid;  /* AID for UICC */
}uim_read_rec_cmd_type;


typedef struct
{
  uim_hdr_type        hdr;        /* Command header */
  uim_items_enum_type item;       /* Item to be accessed */
  word                path[UIM_MAX_PATH_ELEMENTS];
                                  /* Path for accessing an item */
  word                len;     /* Length of data to be written */
  byte               *data;    /* Pointer to data to be written */

  uim_aid_type     aid;  /* AID for UICC */
}uim_inc_cmd_type;



/* SELECT_F command */
typedef enum {
  UIM_NO_SUCH_FILE = 0,    /* No such file */
  UIM_MF  = 1,             /* Master File */
  UIM_DF  = 2,             /* Dedicated File */
  UIM_EF  = 3,             /* Elementary File */
  UIM_ADF = 4,             /* Application DF */
  UIM_MAXIMUM_FILE = 5
} uim_file_type;

typedef  unsigned short     uim_dir_type;      /* Unsigned 16 bit value */

typedef struct {
  uim_hdr_type     hdr;        /* Command header */
  uim_file_type    file_type;  /* File type to be selected */
  uim_dir_type     dir;        /* MF,DF to be accessed if file type is MF ,DF */
  uim_items_enum_type item;    /* EF to be accessed if file type is EF*/
  word             path[UIM_MAX_PATH_ELEMENTS];
                               /* Selection of an EF by the specified path */
  uim_aid_type     aid;        /* AID for UICC */
  uim_session_control_type session; /* session control for UICC */
} uim_select_cmd_type;

/* SEEK_F command */
typedef enum {
  UIM_SEEK_TYPE_1 = UIM_P2_SEEK_TYPE_1,  /* Seek Type 1 */
  UIM_SEEK_TYPE_2 = UIM_P2_SEEK_TYPE_2   /* Seek Type 2 */
} uim_seek_type;

typedef enum {
  UIM_SEEK_BEGINNING_FORWARDS = UIM_P2_SEEK_BEGINNING_FORWARDS,
  UIM_SEEK_END_BACKWARDS      = UIM_P2_SEEK_END_BACKWARDS,
  UIM_SEEK_NEXT_LOCN_FORWARDS = UIM_P2_SEEK_NEXT_LOCN_FORWARDS,
  UIM_SEEK_PREV_LOCN_BACKWARDS= UIM_P2_SEEK_PREV_LOCN_BACKWARDS
} uim_seek_mode_type;

typedef enum {
  UIM_SRCH_FORWARD   = UIM_P2_SRCH_FORWARD,  /* Search forward */
  UIM_SRCH_BACKWARD  = UIM_P2_SRCH_BACKWARD  /* Search backward */
} uim_srch_dir_type;

typedef enum {
   UIM_SRCH_P1_FORWARD  = UIM_SRCH_IND_P1_FORWARD,
   UIM_SRCH_P1_BACKWARD = UIM_SRCH_IND_P1_BACKWARD,
   UIM_SRCH_NEXT        = UIM_SRCH_IND_NEXT,
   UIM_SRCH_PREVIOUS    = UIM_SRCH_IND_PREV
} uim_srch_record_mode_type;

typedef enum {
   UIM_SRCH_ABSOLUTE  = UIM_SRCH_IND_ABS,
   UIM_SRCH_CHARACTER = UIM_SRCH_IND_CHAR
} uim_srch_offset_type;

typedef enum {
  UIM_NO_SUCH_SRCH_MODE   = 0,   /* No such search mode */
  UIM_SIMPLE_SRCH         = 1,   /* Simple search */
  UIM_ENHANCED_SRCH       = 2,   /* Enhanced Search */
  UIM_MAXIMUM_SRCH_MODE   = 3
} uim_srch_mode;

typedef struct {
 /* Generic search/seek fields */
  uim_hdr_type   hdr;                    /* Command header */
  uim_items_enum_type item;              /* EF to be searched */
  word    path[UIM_MAX_PATH_ELEMENTS]; /* Path for accessing an item */
  byte    srch_pattern[UIM_MAX_CHARS ];  /* Search string */
  byte    num_bytes;   /* Number of bytes in search pattern */
                                  /* Max 16 for 11.11 */

  /* For remotability purposes, the seek and search specific fields are not
     featurized */

  /* 11.11 SEEK specific fields */
  uim_seek_type         seek_type;    /* Type 1 or Type 2 Seek */
  uim_seek_mode_type    mode;         /*  Mode used to SEEK */

  /* USIM specific SRCH fields */
  uim_aid_type          aid;          /* AID for USIM */
  uim_srch_mode         srch_mode;    /* Simple/enhanced Search */
  byte                  rec_number;   /* Record Number to search from */
  uim_srch_dir_type     srch_dir;     /* Search types */

  /* Enhanced Mode specific */
  uim_srch_offset_type  offset_type;
                          /* offset as absolute position or character */
  byte                  offset;
                          /* Offset within the record to begin search from */
  uim_srch_record_mode_type  srch_rec_mode_type;  /* Record Mode to search */
} uim_seek_cmd_type;

/* UIM_INVALIDATE_F command */

typedef struct {
  uim_hdr_type   hdr;              /* Command header */
  uim_items_enum_type item;        /* EF to invalidate */
  word path[UIM_MAX_PATH_ELEMENTS]; /* Path for accessing an item */
  uim_aid_type   aid;              /* AID for USIM */
} uim_invalidate_cmd_type;

/* UIM_REHABILITATE_F command */

typedef struct {
  uim_hdr_type   hdr;              /* Command header */
  uim_items_enum_type item;        /* EF to invalidate */
  word path[UIM_MAX_PATH_ELEMENTS]; /* Path for accessing an item */
  uim_aid_type   aid;              /* AID for USIM */
} uim_rehabilitate_cmd_type;

/* UIM_VERIFY_CHV_F command */

typedef struct {
  uim_hdr_type   hdr;              /* Command header */
  uim_chv_type   chv;              /* CHV1 or CHV2 */
  byte           chv_digits[UIM_MAX_CHV_DIGITS];      /* CHV digits */
  uim_aid_type   aid;              /* AID for USIM */
} uim_verify_chv_cmd_type;

/* UIM_CHANGE_CHV_F command */

typedef struct {
 uim_hdr_type   hdr;               /* Command header */
 uim_chv_type   chv;               /* CHV1 or CHV2 */
 byte           chv_digits[UIM_MAX_CHV_DIGITS];      /* CHV digits */
 byte           new_chv_digits[UIM_MAX_CHV_DIGITS];  /* New CHV digits */
 byte           param_p2;          /* P2 for a USIM Verify */
 uim_aid_type   aid;               /* AID for USIM */
} uim_change_chv_cmd_type;

/* UIM_DISABLE_CHV_F command */

typedef struct {
 uim_hdr_type   hdr;                    /* Command header */
 uim_chv_type   chv;                    /* CHV1 or CHV2 */
 byte           chv_digits[UIM_MAX_CHV_DIGITS];  /* CHV digits */
 byte           param_p2;               /* P2 for a USIM Verify */
 uim_aid_type   aid;                    /* AID for USIM */
} uim_disable_chv_cmd_type;

/* UIM_ENABLE_CHV_F command */

typedef struct {
 uim_hdr_type   hdr;                    /* Command header */
 uim_chv_type   chv;                    /* CHV1 or CHV2 */
 byte           chv_digits[UIM_MAX_CHV_DIGITS];  /* CHV digits */
 byte           param_p2;               /* P2 for a USIM Verify */
 uim_aid_type   aid;                    /* AID for USIM */
} uim_enable_chv_cmd_type;

/* UIM_UNBLOCK_CHV_F command */

typedef struct {
  uim_hdr_type   hdr;                   /* Command header */
  uim_chv_type   chv;                   /* CHV1 or CHV2 */
  byte           chv_digits[UIM_MAX_CHV_DIGITS];      /* CHV digits */
  byte           new_chv_digits[UIM_MAX_CHV_DIGITS];  /* New CHV digits */
  byte           param_p2;              /* P2 for a USIM Verify */
  uim_aid_type   aid;                    /* AID for USIM */
} uim_unblock_chv_cmd_type;

/* Internal command for power up */
typedef struct {
  uim_hdr_type   hdr;        /* Command header */
} uim_int_type;

typedef enum {
  UIM_NO_INDICATION = UIM_STATUS_P1_NO_INDICATION,
  UIM_CURR_APP_INITIALIZED = UIM_STATUS_P1_CURR_APP_INIT,
  UIM_TERMINATION_OF_CURR_APP = UIM_STATUS_P1_TERMN_OF_CURR_APP
} uim_status_indication_type;

typedef enum {
  UIM_RETURN_RSP_IDENTICAL_TO_SELECT = UIM_STATUS_P2_RSP_SAME_AS_SELECT,
  UIM_RETURN_DF_NAME_OF_CURR_APP = UIM_STATUS_P2_DF_NAME_OF_CURR_APP,
  UIM_RETURN_NONE = UIM_STATUS_P2_RETURN_NONE
} uim_status_return_data_type;

/* UIM_STATUS_F command */
typedef struct {
  uim_hdr_type   hdr;        /* Command header */
  uim_status_indication_type status;
  uim_status_return_data_type return_data;
} uim_status_cmd_type;


/* UIM_TERMINAL_PROFILE_F command */

typedef struct {
  uim_hdr_type   hdr;               /* Command header */
  byte           num_bytes;         /* Number of bytes in profile */
  byte data[UIM_TERMINAL_PROFILE_DATA_LENGTH]; /* Terminal Profile */
} uim_terminal_profile_cmd_type;

/* UIM_ENVELOPE_F command */

typedef struct {
  uim_hdr_type hdr;                 /* Command header */
  byte         num_bytes;           /* Number of bytes in envelope cmd*/
  byte         data[UIM_MAX_CHARS]; /* Envelope command */
  byte         offset;              /* Offset of envelope command in buffer */
} uim_envelope_cmd_type;

/* UIM_INTERNAL_FETCH_F command */

typedef struct {
  uim_hdr_type hdr;                 /* Command header */
  byte         command_size;        /* Proactive command size */
} uim_fetch_cmd_type;

/* UIM_TERMINAL_RESPONSE_F comamnd */

typedef struct {
  uim_hdr_type hdr;                   /* Command header */
  byte         num_bytes;             /* Number of bytes in Term response*/
  byte         data [UIM_MAX_CHARS];  /* Terminal_response */
} uim_terminal_response_cmd_type;

typedef enum {               /* POLLING INTERVAL OR POLLING OFF */
  POLLING_INTERVAL = 0,
  POLLING_OFF =1
} uim_polling_cmd_enum_type;

typedef struct {
  uim_hdr_type hdr;                       /* Command header */
  uim_polling_cmd_enum_type command_type; /* Polling interval or polling off */
  uint32  polling_interval_in_ms;
} uim_polling_interval_cmd_type;

typedef struct {
  uim_hdr_type          hdr;             /* Command header */
  boolean               terminal_profile;/* Optional terminal prof dload */
#if defined( FEATURE_UIM_SKIP_GSM_IF_RUIM_ONLY ) && \
    defined( FEATURE_UIM_RUIM )                  && \
    defined( FEATURE_UIM_GSM )
#error code not present
#endif /* FEATURE_UIM_SKIP_GSM_IF_RUIM_ONLY      &&
          FEATURE_UIM_RUIM                       &&
          FEATURE_UIM_GSM */
} uim_reset_with_tp_cmd_type;

/* Store ESN of ME */
typedef struct {                     /* UIM_STORE_ESN_ME_F */
  uim_hdr_type    hdr;               /* Command header */
  byte            len_and_usage;     /* Length and usage of ESN */
  dword           esn;               /* ESN */
} uim_store_esn_cmd_type;

typedef struct {                     /* UIM_STORE_ESN_ME_F */
  uim_hdr_type    hdr;               /* Command header */
  byte            len_and_usage;     /* Length and usage of MEID */
  qword           meid;              /* MEID */
  boolean         meid_available;    /* Can MEID be read from NV */
} uim_store_meid_cmd_type;

/* Update SSD command */
typedef struct {                     /* UIM_SSD_UPDATE_F */
  uim_hdr_type    hdr;               /* Command header */
  qword           randssd;           /* 56 bits from BS for SSD Generation */
  byte            process_control;   /* process control bits */
  dword           esn;               /* ESN ( 7 bytes per standard ) */
} uim_ssd_update_cmd_type;

/* Confirm SSD update */
typedef struct {                     /* UIM_CONFIRM_SSD_F */
  uim_hdr_type    hdr;               /* Command header */
  dword           authbs;            /* Auth Signature from Base Station */
} uim_confirm_ssd_cmd_type;

/* Run CAVE command */
typedef struct {                     /* UIM_RUN_CAVE_F */
  uim_hdr_type    hdr;               /* Command header */
  byte            rand_type;         /* Rand/Randu     */
  dword           rand_chal;         /* 32 bits from BS for Auth Signature */
  byte            dig_len;           /* digit length expressed in bits */
  byte            digits[UIM_RUN_CAVE_DIGITS];
                                     /* maximum of 6 BCD coded digits */
  byte            process_control;   /* process control bits */
  qword           esn;               /* ESN ( 7 bytes per standard ) */
} uim_run_cave_cmd_type;

/* Generate CMEA key and VPM */
typedef struct {                     /* UIM_GENERATE_KEYS_F */
  uim_hdr_type    hdr;               /* Command header */
  byte            vpm_first_octet;   /* First octet of VPM to be output */
  byte            vpm_last_octet;    /* Last  octet of VPM to be output */
} uim_generate_keys_cmd_type;

/* Base Station Challenge command */
typedef struct {                     /* UIM_BS_CHAL_F */
  uim_hdr_type    hdr;               /* Command header */
  dword           randseed;          /* Random number generator seed */
} uim_bs_chal_cmd_type;

/* Commit command */
typedef struct {                     /* UIM_COMMIT_F */
  uim_hdr_type    hdr;               /* Command header */
} uim_commit_cmd_type;

/* Generate public key command */
typedef struct {                     /* UIM_MS_KEY_REQ_F */
  uim_hdr_type    hdr;               /* Command header */
  byte      randseed[UIM_RAND_SEED]; /* Seed used to generate TRUE random no */
  byte      a_key_p_rev;             /* A-KEY-P_REV supported by BS */
  byte      param_p_len;             /* Length of Parameter P */
  byte      param_g_len;             /* Length of Parameter G */
  byte      param_p[UIM_PARAM_P];    /* Param P */
  byte      param_g[UIM_PARAM_G];    /* Param G */
} uim_ms_key_cmd_type;

/* Key generation request */
typedef struct {                     /* UIM_KEY_GEN_REQ_F */
  uim_hdr_type    hdr;               /* Command header */
  byte      bs_result_len;           /* BS Result length */
  byte      bs_result[UIM_PARAM_P];  /* BS Result */
} uim_key_gen_cmd_type;

/* Validate command */
typedef struct {                     /* UIM_VALIDATE_F */
  uim_hdr_type    hdr;               /* Command header */
  byte      block_id;                /* Block ID of the block */
  byte      block_length;            /* Length of the block */
  byte      data[UIM_MAX_CHARS];     /* data in the block */
} uim_validation_cmd_type;

/* Configuration request */
typedef struct {                     /* UIM_CONFIG_REQ_F */
  uim_hdr_type    hdr;               /* Command header */
  byte      block_id;                /* Block ID of the block */
} uim_config_cmd_type;

/* Download request */
typedef struct {                     /* UIM_DOWNLOAD_REQ_F */
  uim_hdr_type    hdr;               /* Command header */
  byte      block_id;                /* Block ID of the block */
  byte      block_len;               /* Length of the block */
  byte      data[UIM_MAX_CHARS];     /* data in the block */
} uim_download_cmd_type;

/* SSPR configuration request */
typedef struct {                     /* UIM_SSPR_CONFIG_REQ_F */
  uim_hdr_type    hdr;               /* Command header */
  byte      block_id;                /* Block ID of the block */
  word      req_offset;              /* Requested offset in the PRL */
  byte      req_max_size;            /* Requested Max size in the response */
} uim_sspr_config_cmd_type;

/* SSPR Download request */
typedef struct {                     /* UIM_SSPR_DOWNLOAD_REQ_F */
  uim_hdr_type    hdr;               /* Command header */
  byte      block_id;                /* Block ID of the block */
  byte      block_len;               /* Length of the block */
  byte      data[UIM_MAX_CHARS];     /* data in the block */
} uim_sspr_download_cmd_type;

/* OTAPA command */
typedef struct {                     /* UIM_OTAPA_REQ_F */
  uim_hdr_type    hdr;               /* Command header */
  byte      start_stop;              /* OTAPA Start Stop */
  dword     randseed;                /* Random seed */
} uim_otapa_cmd_type;



typedef enum {
  UIM_CMPT_IP_CHAP       = 0,       /* Perform CHAP operation */
  UIM_CMPT_IP_MN_HA_AUTH = 1,      /* Calculate MN-HA Authenticator */
  UIM_CMPT_IP_MIP_RRQ    = 2,       /* Calculate MIP-RRQ Hash */
  UIM_CMPT_IP_MN_AAA_AUTH = 3,       /* Calculate MN-AAA Authenticator */
  UIM_CMPT_IP_HRPD_AN_CHAP = 4
} uim_compute_ip_operation_enum_type;

typedef struct {
  byte  chap_id;                    /* CHAP ID */
  byte  nai_entry_index;            /* NAI Entry Index */
  byte  chap_challenge_length;      /* Challenge Length */
  byte  chap_challenge[UIM_MAX_3GPD_CHAP_CHALLENGE_LENGTH]; /* Chap Challenge */
} uim_compute_ip_data_chap_type;

typedef struct {
  byte  nai_entry_index;            /* NAI Entry Index */
  word  registration_data_length;   /* Registration Length */
  byte  registration_data[UIM_MAX_3GPD_MN_HA_REG_DATA_LENGTH];
                                    /* Registration Data */
} uim_compute_ip_data_mn_ha_type;

typedef struct {
  word  rrq_data_length;                             /* Preceding RRQ Length */
  byte  rrq_data[UIM_MAX_3GPD_HASH_RRQ_DATA_LENGTH]; /* Prec MIP-RRQ Data */
} uim_compute_ip_data_mip_rrq_hash_type;

typedef struct {
  byte  nai_entry_index;            /* NAI Entry Index */
  byte  challenge_length;           /* Challenge Length */
  byte  challenge[UIM_3GPD_MIP_CHALLENGE_LENGTH];   /* Challenge */
} uim_compute_ip_data_mn_aaa_type;

typedef struct {
  byte  chap_id;                    /* CHAP ID */
  byte  chap_challenge_length;      /* Challenge Length */
  byte  chap_challenge[UIM_MAX_HRPD_CHAP_CHALLENGE_LENGTH]; /* Chap Challenge */
} uim_compute_ip_data_hrpd_type;

typedef union {
  uim_compute_ip_data_chap_type         chap_data;     /* CHAP Operation */
  uim_compute_ip_data_mn_ha_type        mn_ha_data;    /* MN-HA Operation */
  uim_compute_ip_data_mip_rrq_hash_type mip_rrq_data;  /* RRQ Hash Operation */
  uim_compute_ip_data_mn_aaa_type       mn_aaa_data;   /* MN-AAA Operation */
  uim_compute_ip_data_hrpd_type         hrpd_data;     /* HRPD Operation */
} uim_compute_ip_data_type;

typedef struct {
  uim_hdr_type                       hdr;               /* Command Header */
  uim_compute_ip_operation_enum_type cmpt_ip_operation; /* Operaion Type */
  uim_compute_ip_data_type           cmpt_ip_data;      /* Data for Command */
} uim_compute_ip_auth_cmd_type;



/* Types of return values for 3GPD control check from Data Services */
typedef enum {
  UIM_3GPD_MIP_RUIM_SIP_RUIM=0,    /* ME should use R-UIM for 3GPD */
  UIM_3GPD_MIP_NV_SIP_NV=1,      /* ME should use NV/SW for 3GPD  */
  UIM_3GPD_MIP_RUIM_SIP_NV=2,     /* ME only supports MIP 3GPD */
  UIM_3GPD_MIP_NV_SIP_RUIM=3      /* ME only supports SIP 3GPD */
} uim_3gpd_support_status;

/* Types of return values for HRPD control check from Data Services */
typedef enum {
  UIM_AN_HRPD_SUPPORT=0,         /* ME should use R-UIM for HRPD   */
  UIM_AN_HRPD_NO_SUPPORT=1,      /* ME should use NV/SW for HRPD  */
  UIM_AN_HRPD_USE_CAVE=2         /* ME should use Cave Algo for HRPD*/
} uim_an_hrpd_support_status;


/* RUN_GSM_ALGORITHM_F command */

typedef struct {
  uim_hdr_type   hdr;        /* Command header */
  byte rand[16];             /* Input Rand */
} uim_run_gsm_algo_type;

/* USIM AUTHENTICATE_F command */

typedef enum {
 UIM_AUTN_NO_INFO_GIVEN,
 UIM_AUTN_GLOBAL_REF_DATA,
 UIM_AUTN_SPECIFIC_REF_DATA
}uim_autn_ref_data_type;

typedef struct {
  uim_hdr_type   hdr;             /* Command header */
  uim_aid_type   aid;             /* AID for USIM */
  byte ref_data_number;           /* Reference data number */
  uim_autn_ref_data_type ref_data_type; /* Data type of reference */
  byte num_bytes;                 /* Number of data bytes */
  byte data[UIM_MAX_CHARS];       /* Application specific Input */
} uim_authenticate_type;

typedef struct {
  uim_hdr_type   hdr;             /* Command header */
  uim_aid_type   aid;             /* AID for USIM */
  byte           ref_data_number; /* Reference data number */
  uim_autn_ref_data_type ref_data_type; /* Data type of reference */
  word           len;             /* length of data  */
  byte           *data;           /* Auth Data */
} uim_authenticate_odd_ins_type;


typedef struct {
  uim_hdr_type   hdr;             /* Command header */
  uim_aid_type   aid;             /* AID for USIM */
  byte           ref_data_number; /* Reference data number */
  word           len;             /* length of data  */
  byte           *data;           /* Auth Data */
} uim_bcast_type;


typedef enum {
 UIM_AUTN_ISIM_RESERVED_CONTEXT=0,
 UIM_AUTN_ISIM_3G_CONTEXT=1
}uim_isim_autn_context_type;

typedef struct {
  uim_hdr_type   hdr;             /* Command header */
  uim_aid_type   aid;             /* AID for ISIM */
  uim_isim_autn_context_type auth_context;              /* Authentication context */
  byte num_bytes;                 /* Number of data bytes */
  byte data[UIM_MAX_CHARS];       /* Application specific Input */
} uim_isim_authenticate_type;


typedef enum {
  UIM_MC_OPEN   = 0,    /* Manage Channel Open */
  UIM_MC_CLOSE  = 1 /* Manage Channel Close */
}uim_mc_action_type;

/* Manage Channel Command */
typedef struct {
  uim_hdr_type  hdr;            /* Command Header */
  byte          channel;         /* Channel to perform the operation in */
  byte          op_close_channel;/* Channel to open or close */
  uim_mc_action_type mc_action;  /* Manage Channel Open or Close */
} uim_manage_channel_cmd_type;

/* Select in Channel Command */
typedef struct {
  uim_hdr_type     hdr;        /* Command header */
  byte             channel;    /* Logical channel to use for the command */
  uim_file_type    file_type;  /* File type to be selected */
  uim_dir_type     dir;        /* MF,DF to be accessed if file type is MF ,DF */
  uim_items_enum_type item;    /* EF to be accessed if file type is EF*/
  uim_aid_type     aid;        /* AID to be selected if type is App */
  word             path[UIM_MAX_PATH_ELEMENTS];
                               /* Selection of an EF by the specified path */
} uim_select_in_chan_cmd_type;

/* Access in Channel Command */
typedef struct {
  uim_hdr_type    hdr;            /* Command header */
  uim_access_type access;         /* Type of access, Read or Write */
  byte            channel;        /* Logical channel to use for the command */
  word            num_bytes;      /* Number of bytes to be read or written or
                                     incremented */
  word            num_bytes_rsp;  /* Number of bytes returned */
  word            offset;         /* Offset into the EF */
  void            *data_ptr;      /* Pointer for data */
} uim_access_in_chan_cmd_type;


/* Verify command */
typedef struct {
  uim_hdr_type      hdr;        /* Command Header */
  byte              channel;    /* Logical channel to use for the command */
  byte              ref_qual;   /* Qualifier of Reference Data */
  byte              ver_data[UIM_MAX_CHV_DIGITS];   /* Verification Data */
} uim_verify_in_chan_cmd_type;

/* Enable Verification Requirement Command */

typedef struct {
  uim_hdr_type      hdr;        /* Command Header */
  byte              channel;    /* Logical channel to use for the command */
  byte              ref_qual;   /* Qualifier of Reference Data */
  byte              ver_data[UIM_MAX_CHV_DIGITS];
                                /* Verification Data */
} uim_enable_ver_req_in_chan_cmd_type;

/* Disable Verification Requirement Command */

typedef struct {
  uim_hdr_type      hdr;        /* Command Header */
  byte              channel;    /* Logical channel to use for the command */
  byte              ref_qual;   /* Qualifier of Reference Data */
  byte              ver_data[UIM_MAX_CHV_DIGITS];
                                /* Verification Data */
} uim_disable_ver_req_in_chan_cmd_type;

/* Change reference data Command */

typedef struct {
  uim_hdr_type      hdr;        /* Command Header */
  byte              channel;    /* Logical channel to use for the command */
  byte              ref_qual;   /* Qualifier of Reference Data */
  byte              ver_data[UIM_MAX_CHV_DIGITS];
                                /* Verification Data */
  byte              new_ref_data[UIM_MAX_CHV_DIGITS];
                                /* New Reference Data */
} uim_change_ref_data_in_chan_cmd_type;

/* Reset Retry Command */

typedef struct {
  uim_hdr_type      hdr;        /* Command header */
  byte              channel;    /* Logical channel to use for the command */
  byte              ref_qual;   /* Qualifier of Reference Data */
  byte              unblk_data[UIM_MAX_CHV_DIGITS];
                                /* Unblock Data */
  byte              new_ref_data[UIM_MAX_CHV_DIGITS];
                                /* New Reference Data */
} uim_reset_retry_in_chan_cmd_type;

/* Manage Security Environment Command */

typedef enum {
  UIM_MSE_RESTORE = 0,          /* Restore security environment */
  UIM_MSE_SET     = 1,          /* Set a component of a security environment */
  UIM_MSE_DERIVE_KEY = 2       /* Derive a key */
} uim_mse_operation_type;


typedef enum {
  UIM_MSE_COMPUTATION       = UIM_WIM_MSE_P1_COMPUTE,   /* Computation and deciphering */
  UIM_MSE_ENCIPHER          = UIM_WIM_MSE_P1_ENCIPHERING /* Enciphering/verication */
} uim_mse_set_p1_type;

typedef enum {
  UIM_DIGITAL_SIGNATURE = UIM_WIM_MSE_P2_DIGITAL_SIGNATURE, /* Digital Signature Template */
  UIM_CONFIDENTIALITY   = UIM_WIM_MSE_P2_CONFIDENTIALITY,   /* Confidentiality Template */
  UIM_CRYPTO_CHECKSUM   = UIM_WIM_MSE_P2_CRYPTO_CHECKSUM    /* Crypto Checksum Template */
} uim_crt_type;

typedef struct {
  byte      se_number;      /* Security Environment Number */
} uim_mse_data_restore_type;

typedef struct {
  uim_mse_set_p1_type   set_p1;     /* P1 for MSE set type */
  uim_crt_type          crt;        /* Related CRT tag */
  byte                  data_length;    /* Length of CRDO data field */
  byte                  data[UIM_MAX_CHARS];    /* CRDOs */
} uim_mse_data_set_type;

typedef struct {
  byte                  secret_key_ref;  /* Secret Key Reference */
  byte                  seed_length;    /* Length of CRDO data field */
  byte                  seed[UIM_MAX_CHARS-5]; /* CRDOs and data */
} uim_mse_data_derive_type;

typedef union {
  uim_mse_data_restore_type restore_data; /* Store data for a restore mse */
  uim_mse_data_set_type     set_data;     /* Store data for a set mse */
  uim_mse_data_derive_type  derive_data;  /* Store data for a derive mse */
} uim_mse_data_type;

typedef struct {
  uim_hdr_type            hdr;               /* Command header */
  byte                    channel;    /* Logical channel to use for the cmd */
  uim_mse_operation_type  operation;        /* Set, Restore, or derive key */
  uim_mse_data_type       mse_data;         /* Operation specific data */
} uim_manage_sec_env_cmd_type;

/* Perform Security Operation Command Type */


typedef enum {
  UIM_PSO_ENCIPHER      = 0,    /* Encipher */
  UIM_PSO_DECIPHER      = 1,    /* Decipher, Application Level */
  UIM_PSO_COMPUTE_DIG_SIG=2,    /* Compute Digital Signature */
  UIM_PSO_VERIFY        = 3,    /* Verify Digital Signature */
  UIM_PSO_COMPUTE_CHECKSUM=4   /* Compute Crypto Checksum */
} uim_pso_operation_type;

typedef struct {
  uim_hdr_type      hdr;        /* Command header */
  byte              channel;    /* Logical channel to use for the cmd */
  byte              data_length; /* Length of Data Field */
  uim_pso_operation_type operation; /* Type of PSO operation */
  byte              data[UIM_MAX_PSO_DATA]; /* Data Field */
} uim_perform_sec_op_cmd_type;


/* Ask Random Command Type */

typedef struct {
  uim_hdr_type      hdr;        /* Command Header */
  byte              channel;    /* Logical channel to use for the cmd */
  byte              data_length;/* Length of Data Field */
} uim_ask_random_cmd_type;


typedef enum {
  UIM_GSM_APP,
  UIM_CDMA_APP,
  UIM_TELECOM_APP
} uim_app_type;

typedef struct {
  uim_hdr_type              hdr;  /* Command header */
  uim_app_type              app;  /* App  */
  uim_slot_type             slot; /* Slot for the DF */
} uim_slot_change_cmd_type;

/* APDU Stream command type */
typedef struct {
  uim_hdr_type      hdr;
  byte              uim_class;  /* UIM class of instruction */
  byte              instrn;     /* Instruction type         */
  byte              p1;         /* input parameter 1        */
  byte              p2;         /* input parameter 2        */
  byte              p3;         /* input parameter 3        */
  byte              data[UIM_MAX_CHARS];
} uim_stream_apdu_type;

/* APDU Stream command type - extended interface*/
typedef struct {
  uim_hdr_type      hdr;
  word              num_cmd_bytes; /* Number of command bytes */
  byte              cmd_data[UIM_MAX_STREAM_ISO7816_APDU_TRANSACTION_SIZE];/* Source data */
} uim_stream_iso7816_apdu_type;

/* options as to whether or not to notify GSDI upon power down */
typedef enum {
  UIM_OPTION_NO_NOTIFY_PDOWN, /* Not Notify GSDI about the POWER down */
  UIM_OPTION_NOTIFY_PDOWN /* Notify GSDI about the POWER down */
} uim_pdown_option_type;

/* Power down command type*/
typedef struct {
  uim_hdr_type          hdr;
  uim_pdown_option_type pdown_option;
} uim_power_down_cmd_type;


/* options as to whether the entire cache shall be erased or the cache
 * shall be selectively erased for the items listed
 */
typedef enum {
  UIM_OPTION_CLEAR_CACHE_ALL,
  UIM_OPTION_CLEAR_CACHE_SELECTIVE
} uim_clear_cache_options_type;

/* Clear cache command type */
typedef struct {
  uim_hdr_type                  hdr;
  uim_clear_cache_options_type  clr_cache_option;
  word                          num_items;
  uim_items_enum_type          *item_list;
} uim_clear_cache_cmd_type;

/* ---------------------------------------------------------------
   STRUCTURE:      uim_mflo_init_act_params_cmd_type

   DESCRIPTION:
     Encrypted authentication key to be used in the initialize
     Activation parameters procedure.
----------------------------------------------------------------*/
typedef struct {
  uim_hdr_type  hdr;
  int32         act_params_len;
  uint8         act_params[UIM_MFLO_INIT_ACT_PARAMS_LEN];
}uim_mflo_init_act_params_cmd_type;

/* ---------------------------------------------------------------
   STRUCTURE:      uim_mflo_ucast_cmd_type

   DESCRIPTION:
     Input parameters for the Signing or Verification of the Unicast
     Message procedure.
   ----------------------------------------------------------------*/
typedef struct {
  uim_hdr_type                            hdr;
  uim_mflo_ucast_msg_enum_type            msg_type;
  uint32                                  ucast_msg_len;
  uint8                                  *ucast_msg_ptr;
}uim_mflo_ucast_cmd_type;

typedef union {
  uim_hdr_type                hdr;        /* Generic header */
  uim_access_cmd_type         access_uim; /* Read from or Write to UIM */
  uim_write_bin_cmd_type      write_bin;  /* write to uim bin file response in rpt */
  uim_write_rec_cmd_type      write_rec;  /* write to uim rec file response in rpt */

  uim_read_bin_cmd_type       read_bin;   /* Read bin file with result data in rpt */
  uim_read_rec_cmd_type       read_rec;   /* Read rec file with result data in rpt */

  uim_inc_cmd_type            inc;        /* Increment,result data in rpt */
  uim_int_type                uim_int ;   /* Init UIM */
  uim_select_cmd_type         select;     /* Select a file/directory  */
  uim_seek_cmd_type           seek;       /* SEEK access */
  uim_invalidate_cmd_type     invalidate; /* Invalidate an EF */
  uim_rehabilitate_cmd_type   rehab;      /* Rehabilitate an EF */
  uim_verify_chv_cmd_type     verify_chv; /* Verify CHV */
  uim_change_chv_cmd_type     change_chv; /* Change CHV */
  uim_disable_chv_cmd_type    disable_chv;/* Disable CHV */
  uim_enable_chv_cmd_type     enable_chv; /* Enable CHV */
  uim_unblock_chv_cmd_type    unblk_chv;  /* Unblock CHV */
  uim_status_cmd_type         status;     /* Status of the Card */
  uim_terminal_profile_cmd_type terminal_profile; /* Send a terminal profile */
  uim_envelope_cmd_type       envelope;   /* Appl toolkit command */
  uim_fetch_cmd_type          fetch;      /* Fetch a proactive command */
  uim_terminal_response_cmd_type terminal_response; /* Proactive command rsp */
  uim_polling_interval_cmd_type  polling; /* Polling interval or Polling off */
  uim_stream_apdu_type        stream_apdu;    /* Stream APDU */
  uim_stream_iso7816_apdu_type stream_iso7816_apdu;/* Stream ISO 7816 APDU */

  uim_store_esn_cmd_type      store_esn;      /* Store ESN */
  uim_store_meid_cmd_type     store_meid;     /* Store MEID */
  uim_ssd_update_cmd_type     ssd_update;     /* Update SSD */
  uim_confirm_ssd_cmd_type    confirm_ssd;    /* Confirm SSD */
  uim_run_cave_cmd_type       run_cave;       /* Run CAVE */
  uim_generate_keys_cmd_type  gen_keys;       /* Generate CMEA key and VPM */
  uim_bs_chal_cmd_type        bs_chal;        /* Base station challenge */
  uim_commit_cmd_type         commit;         /* Commit Request */
  uim_ms_key_cmd_type         ms_key_req;     /* MS Key Request */
  uim_key_gen_cmd_type        key_gen;        /* Key Generation Request */
  uim_validation_cmd_type     validate;       /* Validation Request */
  uim_config_cmd_type         configuration;  /* Configuration Request */
  uim_download_cmd_type       download;       /* Download Request */
  uim_sspr_config_cmd_type    sspr_config;    /* SSPR Configuration Request */
  uim_sspr_download_cmd_type  sspr_download;  /* SSPR Download Request */
  uim_otapa_cmd_type          otapa_req;      /* OTAPA Request */

  uim_compute_ip_auth_cmd_type compute_ip_auth; /* Compute IP Authentication */

  uim_run_gsm_algo_type       run_gsm;    /* RUN GSM ALGORITHM */
  uim_authenticate_type          autn;         /* Authenticate for USIM */

  uim_authenticate_odd_ins_type  autn_odd_ins; /* Authenticate for USIM with ODD Ins Classs*/
  uim_bcast_type                 bcast;        /* Bcast Command*/

  uim_isim_authenticate_type  isim_autn;  /* Authenticate for ISIM */

  uim_verify_in_chan_cmd_type           verification_in_chan; /* Verification */
  uim_enable_ver_req_in_chan_cmd_type   enable_ver_in_chan;   /* Enable verification */
  uim_disable_ver_req_in_chan_cmd_type  disable_ver_in_chan; /* Disable verification */
  uim_change_ref_data_in_chan_cmd_type  change_ref_in_chan;  /* Change Reference Data */
  uim_reset_retry_in_chan_cmd_type      reset_retry_in_chan; /* Reset Retry counter */
  uim_select_in_chan_cmd_type           select_in_chan; /* Select cmd in L Ch*/
  uim_access_in_chan_cmd_type           access_in_chan; /* Access cmd in L Ch*/
  uim_manage_channel_cmd_type           manage_channel;

  uim_manage_sec_env_cmd_type  mse;         /* MSE */
  uim_perform_sec_op_cmd_type  pso;         /* Perform Sec Operation */
  uim_ask_random_cmd_type      ask_random;  /* Ask Random */

  uim_reset_with_tp_cmd_type   reset_with_tp; /* Reset card and optionally send TP */
  uim_slot_change_cmd_type     slot_chg;    /* Slot change request */
  uim_power_down_cmd_type      pdown;
  uim_clear_cache_cmd_type     clear_cache;
  uim_mflo_init_act_params_cmd_type      mflo_init_act_params;
  uim_mflo_ucast_cmd_type                mflo_sign_ucast;
  uim_mflo_ucast_cmd_type                mflo_verify_ucast;
} uim_cmd_type;

/* Queue for other tasks to get auth command buffers from */
extern q_type uim_free_q;

typedef enum {
  UIM_NORMAL_MODE,
  UIM_PASSIVE_MODE,
  UIM_RECOVERY_MODE
} uim_mode_type;

/* Variable indicating the mode in which UIM is operating */
extern uim_mode_type uim_mode;

/* Variable to hold the previous mode */
extern uim_mode_type uim_save_mode;

/* Return type structure for service allocation and activation */
typedef struct {
  boolean allocated;    /* If the service is activated */
  boolean activated;    /* If the service is allocated */
} uim_svc_table_return_type;

/* CDMA service table service items */
typedef enum {
  UIM_CDMA_SVC_NO_SUCH_ITEM,
  UIM_CDMA_SVC_CHV_DISABLE,
  UIM_CDMA_SVC_ADN,
  UIM_CDMA_SVC_FDN,
  UIM_CDMA_SVC_SMS,
  UIM_CDMA_SVC_EXT1,
  UIM_CDMA_SVC_EXT2,
  UIM_CDMA_SVC_SMS_PARAMS,
  UIM_CDMA_SVC_LND,
  UIM_CDMA_SVC_HOME_SVC_PROVIDER_NAME,
  UIM_CDMA_SVC_SDN,
  UIM_CDMA_SVC_EXT3,
  UIM_CDMA_SVC_SMS_PP_DOWNLOAD,
  UIM_CDMA_SVC_MENU_SEL,
  UIM_CDMA_SVC_PROACTIVE_UIM,
  UIM_CDMA_SVC_3GPD_SIP,
  UIM_CDMA_SVC_3GPD_MIP,
  UIM_CDMA_SVC_3GPD_MSG_EXT,
  UIM_CDMA_SVC_BCSMS,
  UIM_CDMA_SVC_HRPD,
  UIM_CDMA_SVC_RC,
  UIM_CDMA_SVC_BROWSER,
  UIM_CDMA_SVC_JAVA,
  UIM_CDMA_SVC_BREW,
  UIM_CDMA_SVC_LBS,
  UIM_CDMA_IPV6,
  UIM_CDMA_SVC_MAX_ITEM
}uim_cdma_svc_table_enum_type;

typedef struct {
  uim_rpt_status status;
  byte rsp_data;
} uim_store_esn_me_rsp_type;

/* Store the response to STORE_ESN_ME command */
extern uim_store_esn_me_rsp_type uim_store_esn_me_rsp;

/* Masks for bits in CDMA service table */
#define  UIM_CDMA_SVC_CHV_DISABLE_ALLOCATED_MASK        0x01
#define  UIM_CDMA_SVC_ADN_ALLOCATED_MASK                0x04
#define  UIM_CDMA_SVC_FDN_ALLOCATED_MASK                0x10
#define  UIM_CDMA_SVC_SMS_ALLOCATED_MASK                0x40
#define  UIM_CDMA_SVC_EXT1_ALLOCATED_MASK               0x04
#define  UIM_CDMA_SVC_EXT2_ALLOCATED_MASK               0x10
#define  UIM_CDMA_SVC_SMS_PARAMS_ALLOCATED_MASK         0x40
#define  UIM_CDMA_SVC_LND_ALLOCATED_MASK                0x01
#define  UIM_CDMA_SVC_HOME_SVC_PVDR_NAME_ALLOCATED_MASK 0x01
#define  UIM_CDMA_SVC_SDN_ALLOCATED_MASK                0x04
#define  UIM_CDMA_SVC_EXT3_ALLOCATED_MASK               0x10
#define  UIM_CDMA_SVC_SMS_PP_DOWNLOAD_ALLOCATED_MASK    0x04
#define  UIM_CDMA_SVC_MENU_SEL_ALLOCATED_MASK           0x10
#define  UIM_CDMA_SVC_PROACTIVE_UIM_ALLOCATED_MASK      0x01
#define  UIM_CDMA_SVC_3GPD_MSG_ALLOCATED_MASK           0x10
#define  UIM_CDMA_SVC_3GPD_SIP_ALLOCATED_MASK           0x40
#define  UIM_CDMA_SVC_3GPD_MIP_ALLOCATED_MASK           0x04
#define  UIM_CDMA_SVC_BCSMS_ALLOCATED_MASK              0x04
#define  UIM_CDMA_SVC_HRPD_ALLOCATED_MASK                0x01
#define  UIM_CDMA_SVC_RC_ALLOCATED_MASK                  0x40
#define  UIM_CDMA_SVC_BROWSER_ALLOCATED_MASK             0x01
#define  UIM_CDMA_SVC_JAVA_ALLOCATED_MASK                0x04
#define  UIM_CDMA_SVC_BREW_ALLOCATED_MASK                0x10
#define  UIM_CDMA_SVC_LBS_ALLOCATED_MASK                 0x40
#define  UIM_CDMA_IPV6_ALLOCATED_MASK                    0x10

/* Masks for bits in CDMA service table */
#define  UIM_CDMA_SVC_CHV_DISABLE_ACTIVATED_MASK        0x02
#define  UIM_CDMA_SVC_ADN_ACTIVATED_MASK                0x08
#define  UIM_CDMA_SVC_FDN_ACTIVATED_MASK                0x20
#define  UIM_CDMA_SVC_SMS_ACTIVATED_MASK                0x80
#define  UIM_CDMA_SVC_EXT1_ACTIVATED_MASK               0x08
#define  UIM_CDMA_SVC_EXT2_ACTIVATED_MASK               0x20
#define  UIM_CDMA_SVC_SMS_PARAMS_ACTIVATED_MASK         0x80
#define  UIM_CDMA_SVC_LND_ACTIVATED_MASK                0x02
#define  UIM_CDMA_SVC_HOME_SVC_PVDR_NAME_ACTIVATED_MASK 0x02
#define  UIM_CDMA_SVC_SDN_ACTIVATED_MASK                0x08
#define  UIM_CDMA_SVC_EXT3_ACTIVATED_MASK               0x20
#define  UIM_CDMA_SVC_SMS_PP_DOWNLOAD_ACTIVATED_MASK    0x08
#define  UIM_CDMA_SVC_MENU_SEL_ACTIVATED_MASK           0x20
#define  UIM_CDMA_SVC_PROACTIVE_UIM_ACTIVATED_MASK      0x02
#define  UIM_CDMA_SVC_3GPD_MSG_ACTIVATED_MASK           0x20
#define  UIM_CDMA_SVC_3GPD_SIP_ACTIVATED_MASK           0x80
#define  UIM_CDMA_SVC_3GPD_MIP_ACTIVATED_MASK           0x08
#define  UIM_CDMA_SVC_BCSMS_ACTIVATED_MASK              0x08
#define UIM_CDMA_SVC_HRPD_ACTIVATED_MASK                0x02
#define  UIM_CDMA_SVC_RC_ACTIVATED_MASK                 0x80
#define  UIM_CDMA_SVC_BROWSER_ACTIVATED_MASK            0x02
#define  UIM_CDMA_SVC_JAVA_ACTIVATED_MASK               0x08
#define  UIM_CDMA_SVC_BREW_ACTIVATED_MASK               0x20
#define  UIM_CDMA_SVC_LBS_ACTIVATED_MASK                0x80
#define  UIM_CDMA_IPV6_ACTIVATED_MASK                   0x20

/* Size of CDMA service table */
#define UIM_CDMA_SVC_TABLE_SIZE  12

/* This holds the polling duration of the Proactive UIM */
extern dword uim_proactive_time_delay;
/* <EJECT> */
/*===========================================================================

FUNCTION UIM_GET_EF_UNDER_MF_PATH_FROM_FILE_ID

DESCRIPTION
  This function takes as parameters the file id.  Sets paramter path
  to the path of the File.

DEPENDENCIES


RETURN VALUE
  TRUE if the File ID was found

SIDE EFFECTS
  Will return the path field of the command for those items that are selected
  by using the command path.

===========================================================================*/

extern boolean uim_get_ef_under_mf_path_from_file_id
(
word file_id,
word ** path
);
/*~ FUNCTION uim_get_ef_under_mf_path_from_file_id */
/*~ PARAM OUT path POINTER
    *path VARRAY LENGTH (UIM_MAX_PATH_ELEMENTS - 1) */

/* <EJECT> */
/*===========================================================================

FUNCTION UIM_GET_TELECOM_PATH_FROM_FILE_ID

DESCRIPTION
  This function takes as parameters the file id.  Sets paramter path
  to the path of the File.

DEPENDENCIES


RETURN VALUE
  TRUE if the File ID was found

SIDE EFFECTS
  Will return the path field of the command for those items that are selected
  by using the command path.

===========================================================================*/

extern boolean uim_get_telecom_path_from_file_id
(
word file_id,
word ** path
);
/*~ FUNCTION uim_get_telecom_path_from_file_id */
/*~ PARAM OUT path POINTER
    *path VARRAY LENGTH (UIM_MAX_PATH_ELEMENTS -1) */

/* <EJECT> */

/*===========================================================================

FUNCTION UIM_GET_USIM_PATH_FROM_FILE_ID

DESCRIPTION
  This function takes as parameters the file id.  Sets paramter path
  to the path of the File.

DEPENDENCIES


RETURN VALUE
  TRUE if the File ID was found

SIDE EFFECTS
  Will return the path field of the command for those items that are selected
  by using the command path.

===========================================================================*/

extern boolean uim_get_usim_path_from_file_id
(
word file_id,
word ** path
);
/*~ FUNCTION uim_get_usim_path_from_file_id */
/*~ PARAM OUT path POINTER
    *path VARRAY LENGTH (UIM_MAX_PATH_ELEMENTS - 1) */

/* Hot swap status */
typedef enum {
  UIM_CARD_UNCHANGED,
  UIM_CARD_REMOVED,
  UIM_CARD_INSERTED
}uim_card_swap_enum_type;
/*===========================================================================

FUNCTION UIM_CARD_SWAP_DETECTED

DESCRIPTION
  This function is called when hardware detects a card removal or insertion.

DEPENDENCIES
  None

RETURN VALUE
  None

SIDE EFFECTS

===========================================================================*/
extern void uim_card_swap_detected(uim_slot_type            slot,
              uim_card_swap_enum_type status);

/* <EJECT> */
/*===========================================================================

FUNCTION UIM_GET_UIM_STATUS

DESCRIPTION
  This procedure retrieves the state of UIM.

DEPENDENCIES
  None

RETURN VALUE
  Returns status of type uim_status_type.

SIDE EFFECTS
  None

===========================================================================*/
extern uim_status_type uim_get_uim_status( void );
/* <EJECT> */
/*===========================================================================

FUNCTION UIM_GET_ESN_ME ( )

DESCRIPTION
  This function returns the HW ME ESN cached by TMC at power up.

DEPENDENCIES
  None

RETURN VALUE
  The HW ME ESN.  If the ESN was not cached properly, 0 is returned

SIDE EFFECTS
  None

===========================================================================*/
extern dword uim_get_esn_me ( void );
/*~ FUNCTION uim_get_esn_me */

/* <EJECT> */
/*===========================================================================

FUNCTION UIM_GET_UIM_STATUS_2

DESCRIPTION
  This procedure retrieves the current state of UIM in slot 2.

DEPENDENCIES
  None

RETURN VALUE
  Returns uim status_2.

SIDE EFFECTS
  None

===========================================================================*/
uim_status_type uim_get_uim_status_2( void );

/* <EJECT> */
/*===========================================================================

FUNCTION UIM_GET_SLOT_FOR_APP

DESCRIPTION
  This procedure retrieves the current slot corresponding to the application.

DEPENDENCIES
  None

RETURN VALUE
  Returns boolean.

SIDE EFFECTS
  None

===========================================================================*/
uim_slot_type uim_get_slot_for_app
(
  uim_app_type app
);


/* <EJECT> */
/*===========================================================================

FUNCTION UIM_RETURN_CDMA_SVC_AVAILABILITY

DESCRIPTION
  This function checks availabilities for a specified service.

DEPENDENCIES
  The CDMA service table has to be read first.

RETURN VALUE
  TRUE :   The service is activated.
  FALSE:   The service is unavailable.

SIDE EFFECTS
  None.

===========================================================================*/
extern uim_svc_table_return_type uim_return_cdma_svc_availabililty (
 uim_cdma_svc_table_enum_type item,  /* The service to be checked*/
 const byte                   *table /* Pointer to the service table */
);
/*~ FUNCTION uim_return_cdma_svc_availabililty */
/*~ PARAM IN table ARRAY UIM_CDMA_SVC_TABLE_SIZE */

/* <EJECT> */
/*===========================================================================

FUNCTION UIM_POWER_CONTROL

DESCRIPTION
  This procedure changes the state of UIM.

DEPENDENCIES
  Time delay to be used.

RETURN VALUE
  None.

SIDE EFFECTS
  Internally sets the UIM_TOGLLE_STATE_SIG

===========================================================================*/
extern dword uim_power_control ( uim_voter_type mask, boolean uim_reqd );
/*~ FUNCTION uim_power_control */

/* <EJECT> */
/*===========================================================================

FUNCTION UIM_SET_PROACTIVE_UIM

DESCRIPTION
  This procedure is called by UI to indicate to UIM if the underlying UIM
  is pro-active or not.

DEPENDENCIES
  None

RETURN VALUE
  None.

SIDE EFFECTS
  Internally sets the UIM buffer.
===========================================================================*/
extern void uim_set_proactive_uim ( boolean proactive_uim );


/*===========================================================================

FUNCTION UIM_NOTIFY_SYS_MODE

DESCRIPTION
  This procedure notifies the current mode to set the active slot information.

DEPENDENCIES
  None

RETURN VALUE
  None.

SIDE EFFECTS
  Sets the uim_active_slot information.

===========================================================================*/
void uim_notify_sys_mode ( sys_sys_mode_e_type sys_mode );
/*===========================================================================

FUNCTION UIM_SET_PROACTIVE_UIM_2

DESCRIPTION
  This procedure is called by UI/any entity that reads the CDMA servive table
  to indicate to UIM task if the underlying UIM is pro-active or not.

DEPENDENCIES
  None

RETURN VALUE
  None.

SIDE EFFECTS
  Internally sets the UIM buffer.
===========================================================================*/
void uim_set_proactive_uim_2 ( boolean proactive_uim );

/* <EJECT> */
/*===========================================================================

FUNCTION UIM_GET_ENUM_VALUE_FOR_PATH

DESCRIPTION
  This procedure takes as input a path that corresponds to an enum type in the
  items table and returns the enum value of the item

DEPENDENCIES
  None

RETURN VALUE
  Enum value corresponding to the input path.

SIDE EFFECTS
  None
===========================================================================*/
extern word uim_get_enum_value_for_path (word const *path);

/*===========================================================================

FUNCTION UIM_NV_INIT

DESCRIPTION
  It reads the NV item UIM_FIRST_INST_CLASS. This function should be called
  once at power-up, preferably in tmc_task_init().

DEPENDENCIES
  NV task has started.

RETURN VALUE
  None.

SIDE EFFECTS
  Saves the data into the local variable uim_first_inst_class

===========================================================================*/
extern void uim_nv_init(
  rex_tcb_type  *task_ptr,              /* Calling task's TCB pointer */
  rex_sigs_type task_wait_sig,          /* Task signal to wait for when
                                          reading NV */
  void          (*task_wait_handler)( rex_sigs_type )
                                       /* Task's wait function to be called
                                          when reading NV */
);

/* <EJECT> */
/*===========================================================================

FUNCTION UIM_GET_STORE_ESN_ME_RESPONSE

DESCRIPTION
  This procedure returns the response to store esn me command.

DEPENDENCIES
  None

RETURN VALUE
  The response to store esn me command.

SIDE EFFECTS
  None.
===========================================================================*/
uim_store_esn_me_rsp_type uim_get_store_esn_me_response (void);

/* <EJECT> */
/*===========================================================================

FUNCTION UIM_PROCESS_CARD_RESPONSE                EXTERNAL

DESCRIPTION
  This procedure processes the response received from the UIM card and changes
  the UIM server control variables based upon the processing status.

DEPENDENCIES
  This function uses global variables defined for the UIM server to determine
  the proper processing of the card response.

RETURN VALUE
  cmd_ptr value.

SIDE EFFECTS
  The control variables of the UIM server are adjusted to reflect the change
  in the operation of the UIM server as a result of the processing on the
  response.

===========================================================================*/

extern uim_cmd_type *uim_process_card_response
(
 uim_cmd_type  *cmd_ptr, /* pointer to command */
 rex_sigs_type *i_mask   /* Mask of signals interested in */
);

/* <EJECT> */
/*===========================================================================

FUNCTION UIM_TASK

DESCRIPTION
  This procedure is the entrance procedure for the UIM task.

DEPENDENCIES
  None

RETURN VALUE
  Does not return.

SIDE EFFECTS
  None

===========================================================================*/

extern void uim_task (
  dword dummy
    /* Parameter required for REX.  Tell lint to ignore it. */
    /*lint -esym(715,dummy) */
);

/* <EJECT> */
/*===========================================================================

FUNCTION UIM_CMD

DESCRIPTION
  The UIM_cmd pointed to by the parameter passed in is queued up for UIM.

DEPENDENCIES
  None.

RETURN VALUE
  None.

SIDE EFFECTS
  None.
===========================================================================*/
extern void uim_cmd (
  uim_cmd_type *cmd_ptr                    /* the command to queue up */
);

/*===========================================================================

FUNCTION UIM_RECOVERY_CMD

DESCRIPTION
  The uim_cmd pointed to by the parameter passed in is queued up for UIM
  as a recovery command after placing it at the head of the command queue.

DEPENDENCIES
  uim_mode should already be in UIM_RECOVERY_MODE

RETURN VALUE
  uim_return_type indicating success/failure/reason

SIDE EFFECTS
  Sets the UIM_CMD_Q_SIG.
===========================================================================*/
extern uim_return_type uim_recovery_cmd(
  uim_cmd_type *cmd_ptr /* the command to queue up */
);


/*===========================================================================

FUNCTION UIM_GET_MODE

DESCRIPTION
  This function returns the UIM Mode.

DEPENDENCIES

RETURN VALUE
  uim_mode_type indicating the current mode

SIDE EFFECTS
===========================================================================*/
extern uim_mode_type uim_get_mode(
  void
);


/*===========================================================================

FUNCTION UIM_SAFE_TO_FREE

DESCRIPTION
  This function takes a pointer to a tmc allocated piece of memory, and checks
  to see whether it is on the uim_cmd_q.  If it is, it is not considered safe
  to free.

DEPENDENCIES

RETURN VALUE
  FALSE if not safe to free (on the command q), TRUE if safe

SIDE EFFECTS
===========================================================================*/
extern boolean uim_safe_to_free(
  const void* cmd_ptr
);


/*===========================================================================

FUNCTION UIM_REMOVE_COMMAND_FROM_UIM_CMD_Q

DESCRIPTION
  Checks to see if a pointer to a command is on the uim_cmd_q, and if it is,
  removes it.  This is not a function to be used widely, but is used if a
  a client (mmgsdi) has abandoned this command as a presumed failure, and
  wants to deallocate the memory associated with this command.  This procedure
  is used to prevent a corrupted uim_cmd_q in the event that mmgsdi deallocates
  a command which uim still intends to process.  It mostly will happen with
  bad UIM HW, where there is a reoccuring recovery scenario, and UIM and MMGSDI
  are having trouble keeping their states synched due to constant resets.

DEPENDENCIES

RETURN VALUE
  None

SIDE EFFECTS
===========================================================================*/
extern void uim_remove_command_from_uim_cmd_q(
  const uim_cmd_type* cmd_ptr
);


/*===========================================================================

FUNCTION UIM_USB_ENUMERATION_FAILED

DESCRIPTION
  This function is called by the USB driver when either the USB connect fails
  or the USB ICCD interface enumeration fails

DEPENDENCIES
  None

RETURN VALUE
  None

SIDE EFFECTS
  None

===========================================================================*/
extern void uim_usb_enumeration_failed
(
  void
);


#endif /* UIM_H */

