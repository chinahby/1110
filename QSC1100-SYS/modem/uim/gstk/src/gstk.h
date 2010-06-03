#ifndef GSTK_H
#define GSTK_H
/*===========================================================================


                  G S T K    M O D U L E     H E A D E R


GENERAL DESCRIPTION

  This header file is the gstk module header protocol and function
  prototypes.

EXTERNALIZED FUNCTIONS
  gstk_task_put_cmd_buf
    This function put the command onto gstk task command queue

  gstk_cmd
    This function allows clients to put the gstk_cmd onto the GSTK command
    queue.

  gstk_power_down
    This function flushes GSTK queue, vote UIM off and Ack TMC

  gstk_process_state
    This function dispatches the command to the current state of the GSTK state
    machine.

  gstk_build_timer_expire_envelope
    This function build an envelope command when the timer was started and
    expired

  gstk_rex_timer_elapsed_expire_cb
    This function will add the elapsed time value to the time field of the
    timer management table and restart the timer_elapsed

  gstk_rex_timer_expire_cb
    This function builds the timer expiration envelope command to be sent
    to the card


INITIALIZATION AND SEQUENCING REQUIREMENTS


===========================================================================*/

/*===========================================================================
                        COPYRIGHT INFORMATION

Copyright (c) 2003-2009 QUALCOMM, Incorporated and its licensors.  All Rights
Reserved.  QUALCOMM Proprietary.  Export of this technology or software
is regulated by the U.S. Government. Diversion contrary to U.S. law prohibited.

                        EDIT HISTORY FOR MODULE

This section contains comments describing changes made to the module.
Notice that changes are listed in reverse chronological order.

$Header: //source/qcom/qct/modem/uim/gstk/rel/07H1_2/src/gstk.h#8 $

when       who     what, where, why
--------   ---     ----------------------------------------------------------
04/15/09   xz      Support client id clean up of modem restart
03/31/09   yb      Fixed Number of TPs expected for Non UI builds with ESTK 
                   enabled
03/26/09   xz      Fix the issue of TP DL failing if GSTK fails to get app_id 
                   when parse client TP and simplify TP download procedure
03/13/09   gg      Define class bit masks using bitwise OR of cmd bit masks
02/26/09   gg      Introduced Class B event registration
02/03/09   yb      Added secondary support for cell id information when
                   rr_get_cell_id returns NULL in gstk_mo_sms_ctrl_ind()
01/16/09   xz      Add gstk_priv_data to store GSTK private data
10/22/08   sk      Removed gstk_terminal_profile_data_from_client
10/16/08   ssr     Fixed GSTK_TP_EIGHTEENTH_BYTE value for RUIM Enablement
10/13/08   yb      Removed Lint Errors
10/06/08   yb      Sending BIP envelope after TR while BIP is working
10/01/08   sk      Increased Terminal Profile download wait time if
                   FEATURE_ESTK is turned on
09/24/08   xz      Removed FEATURE_GSTK_SINGLE_CLIENT_RAW_FMT
09/08/08   sk      Added support for FEATURE_ESTK
09/02/08   sk      Removed Featurization FEATURE_GSTK_SINGLE_CLIENT_RAW_FMT
08/29/08   yb      Fixed alignment
08/28/08   sk      Added fix for enhanced registration
08/28/08   yb      Added support for longer text string in terminal response
08/22/08   sk      Added support to send end session to only those clients
                   that are involved in the current proactive session 
08/21/08   sk      Added support for enhanced GSTK registration that includes 
                   support for multiple clients to perform terminal profile 
                   download and support for clients to register for a subset of 
                   proactive command in GSTK or RAW format. 
08/08/08   xz      Added support for raw proactive command, raw terminal response
                   and raw envelope command.
05/06/08   sk      Added support for event download envelope retry
04/04/08   sk      Fixed access technology enum
03/28/08   sk      Fixed warnings
02/06/08   sk      Support for PLI UTRAN NMR
11/29/07   sk      Added support for differentiating SAT and USAT TP dl
11/07/07   sk      Added support for NAA Refresh
11/20/07   tml     Added support to allow delay in sending TR until client
                   responded back for certain internal command that was originally
                   handled by GSTK
10/04/07   sk      Added UTRAN measurement tag
08/07/07   sk      Fixed compilation / warning
08/01/07   sk      Fixed lint error
07/14/07   sk      Added support to requeue sms pp envelope if there is a 
                   pending send sms tr
06/06/07   st      Added GSTK_DCTSTK_SIG for data card toolkit support in UNDP-1
05/07/07   jar     Increased the GSTK Timeout Value to 5 mins for WM builds
04/23/07   nk      Added gstk_offset_memcpy function
04/12/07   jk      Added GSTK MALLOC and FREE functions
03/16/07   tml     Fixed AID tag value
02/27/07   jk      Lower Command Buffer Size for Memory Reduction
01/26/07   tml     Fixed AID tag value
01/17/07   tml     Fix TP timeout for features
01/03/06   tml     Increase TP timeout time for WINCE target
08/25/06   sk      Fixed defintion for TP 18th byte
08/25/06   sk      Featurized uim recovery
08/17/06   sk      Defined 18th and 23rd byte of Terminal Profile
06/24/06   sk      Lint fixes
06/17/06   jar     Added FEATURE_GSTK_FOR_NON_LTK_BUILD around RRC Includes
                   to support VSIM in LTK.
06/13/06   sk      UIM recovery support
06/06/06   sk      Added support for network search mode
05/15/06   tml     GPRS Call Control support
05/03/06   tml     compilation fix
05/03/06   tml     lint and Add PDP Content Act Param Tag
04/20/06   sk      Added nv header file.
04/04/06   sk      Added support to obtain battery status, ESN, MEID, IMEISV.
                   Added prototypes for functions that obtain cell id info from
                   RRC and RR.
                   Added support for immediate digit response and variable
                   timeout in get inkey cmd.
                   Added support for 1X related location information.
01/18/06   sk      Added new macro for Refresh Type
11/14/05   sk      Fixed lint errors
11/03/05   sk      Fixed compilation warning
10/18/05   tml     Added protocol info
10/10/05   tml     Added Curr Rat and Next Curr Rat support
10/07/05   tml     Added plmn and lac in rrc cell change ind type
09/30/05   sk      Changed prototype for gstk_rrc_cell_change_ind_cb()
                   Added two new tables gstk_loci_table and rrc_loci_table
                   for maintaining plmn/lac/cell_id info from mm and rrc
                   respectively.
                   Defined two new macros GSTK_MAX_PLMN_LEN and
                   GSTK_MAX_LAC_LEN
09/26/05   tml     Updated RRC cell id received variable to ensure location
                   status for normal service is only sent when both MM and RRC
                   info are received
09/20/05   tml     Update Access Technology tag value
09/20/05   tml     RRC callback registration and hanlding to allow proper
                   cell id caching and location status event when cell ID
                   changed during normal service
07/20/05   tml     Added extended cell id support
07/19/05   sk      Added new macro GSTK_LOWER_NIBBLE_MASK
07/13/05   sk      Added Access Technology support
06/09/05   sun     Check for flag before processing commands
05/11/05   sst     Lint fixes
04/06/05   tml     Reduce proactive command cache size to 6
03/05/05   tml     Added tag comprehension support
02/02/05   sk      Added new enum type gstk_search_string_type
02/02-05   sk      Added macros to support Network Access Name
12/06/04   sk      Changed the 'get_channel_status' terminal response structure
                   to include status of all the open and dropped channels.
12/04/04   tml     move dcs enum to exp.h
11/23/04  sk/tml   Added SAT/USAT data support
08/30/04   tml     Added gstk_profile_received
08/19/04   tml     Added cell broadcast support
08/18/04   tml     Added gstk_dcs_enum_type
08/10/04   tml     Added GSTK_SEND_POSTPONE_ENVELOPE signal handling
08/03/04   tml     Added comment for timer_value_tlv format
06/14/04   tml     Added GSTK_GENERATED_END_SES and GSTK_GENERATED_END_SES_INDEX
05/11/04   tml     Added lang notification, timer management, send dtmf,
                   timer expiration, browser term evt dl and lang selection
                   evt dl supports
04/16/04   tml     linted
03/05/04   tml     Allowed single slot to operate individually in Dual Slot
                   solution
03/01/04   tml     Merged from branch: Fixes for location status env command
09/13/03   tml     Changed slot_id_type to slot_id_enum_type
09/12/03   tml     Increased GSTK cmd pool size and GSTK address len
09/04/03   tml     Dual slot support
05/16/03   tml     linted
04/28/03   tml     Added GSDI access signal
04/02/03   tml     Added UTK support
03/20/03   tml     Added GSTK_TLV_LENGTH_1_BYTE_OFFSET_LIMIT and
                   GSTK_2_BYTE_LENGTH_FIRST_VALUE, GSTK_TAG_LENGTH_LEN
03/06/03   tml     Prefix enum with GSTK_
02/28/03   jar     Increased Terminal Response from 1 second to 2 Minutes.
                   1000 -> 12000.
02/11/03   tml     Remove GSTK_UIM_REPORT_SIG
02/07/03  tml/jar  Initial Version



===========================================================================*/

/*===========================================================================

                     INCLUDE FILES FOR MODULE

===========================================================================*/
#include "cmd.h"
#include "gsdi_exp.h"
#include "mmgsdilib.h"

#include "cm.h"
#include "rex.h"
#include "intconv.h"

/*===========================================================================

                        DATA DECLARATIONS

===========================================================================*/

#include "gstki.h"
#include "gstk_exp.h"

#if defined(FEATURE_WCDMA) && !defined (FEATURE_GSTK_FOR_NON_LTK_BUILD)
#error code not present
#endif /* FEATURE_WCDMA && !defined (FEATURE_GSTK_FOR_NON_LTK_BUILD */

#include "nv.h"

/* Client type and client ID for GSTK such that it can use the GSTK
library functions */
#define GSTK_CLIENT_ID  1

#define GSTK_MAX_BCD_ADDRESS_LEN               100 /* TP_BCD_NO_LENGTH */

#define GSTK_CELL_BROADCAST_PAGE_LEN           88

/* Number of terminal profile downloads expected from clients */
#ifdef FEATURE_ESTK
#error code not present
#else /* FEATURE_ESTK */
  #ifdef FEATURE_UI_NO_TP
    #define GSTK_EXPECTED_NUM_OF_TP_DL          0x01
  #else /* FEATURE_UI_NO_TP */
    #define GSTK_EXPECTED_NUM_OF_TP_DL          0x02
  #endif /* FEATURE_UI_NO_TP */
#endif /* FEATURE_ESTK */

#define GSTK_GENERATED_END_SES              0xFF
#define GSTK_GENERATED_END_SES_INDEX        0x06

/* BER-Tag values used to describe the envelope contents [ME -> CARD]*/
#define GSTK_PROACTIVE_SIM_COMMAND_TAG      0xD0
#define GSTK_SMS_PP_DOWNLOAD_TAG            0xD1
#define GSTK_CELL_BROADCAST_DOWNLOAD_TAG    0xD2
#define GSTK_MENU_SELECTION_TAG             0xD3
#define GSTK_CALL_CONTROL_TAG               0xD4
#define GSTK_MO_SHORT_MESSAGE_CONTROL_TAG   0xD5
#define GSTK_EVENT_DOWNLOAD_TAG             0xD6
#define GSTK_TIMER_EXPIRY_TAG               0xD7
#define GSTK_TERMINAL_RESPONSE_TAG          0xFF

/* general data object lengths */
#define GSTK_COMMAND_DETAILS_LEN            0x03
#define GSTK_DEVICE_IDENTITY_LEN            0x02
#define GSTK_GEN_RESULT_LEN                 0x01
#define GSTK_DURATION_LEN                   0x02
#define GSTK_ITEM_IDENTIFIER_LEN            0x01
#define GSTK_LOCATION_INFORMATION_LEN       0x07
#define GSTK_CDMA_LOCATION_INFORMATION_LEN  0x0F
#define GSTK_HELP_REQUEST_LEN               0x00
#define GSTK_EVENT_LIST_LEN                 0x01
#define GSTK_LOCATION_STATUS_LEN            0x01
#define GSTK_DATE_TIME_AND_TIME_ZONE_LEN    0x07
#define GSTK_TIMING_ADVANCE_LEN             0x02
#define GSTK_LANGUAGE_LEN                   0x02
#define GSTK_ACCESS_TECHNOLOGY_LEN          0x01
#define GSTK_BATTERY_STATUS_LEN             0x01
#define GSTK_NW_SEARCH_MODE_LEN             0x01
#define GSTK_BC_REPEAT_IND_LEN              0x01
#define GSTK_RESPONSE_LEN_LEN               0x02
#define GSTK_ICON_ID_LEN                    0x02
#define GSTK_TONE_LEN                       0x01
#define GSTK_BROWSER_ID_LEN                 0x01
#define GSTK_BROWSER_TERMINATION_CAUSE_LEN  0x01
#define GSTK_TIMER_VALUE_LEN                0x03
#define GSTK_TIMER_IDENTITY_LEN             0x01
#define GSTK_CHANNEL_STATUS_LEN             0x02
#define GSTK_CHANNEL_DATA_LEN_LEN           0x01
#define GSTK_BUFFER_SIZE_LEN                0x02
#define GSTK_CSD_BEARER_PARAM_LEN           0x03
#define GSTK_GPRS_BEARER_PARAM_LEN          0x06
#define GSTK_SIM_ME_INTERFACE_TRANSPORT_LVL_LEN 0x03
#define GSTK_UTRAN_NMR_QUAL_LEN             0x01
#define GSTK_ERROR_RESP_RESULT_LEN          0x02

/* tag index location in command */
/* index location for command details and
** device id is based on total size of the
** length field == 1 */
#define GSTK_PROACTIVE_CMD_TAG_INDEX    0
#define GSTK_COMMAND_DETAILS_TAG_INDEX  2
#define GSTK_DEVICE_ID_TAG_INDEX        7

/* GSTK capabilities */
#define GSTK_TP_LEN                     9    /* Length of terminal profile  */
#define GSTK_MAX_RAW_SMS_LEN            176
#ifdef FEATURE_CAT_REL6
#define GSTK_MAX_EVENT_LIST_LEN         15   /* based on Standard, there are only 15 events
                                             ** can be specified at this time
                                             */
#else
#define GSTK_MAX_EVENT_LIST_LEN         13   /* based on Standard, there are only 13 events
                                             ** can be specified at this time
                                             */
#endif /* #ifdef FEATURE_CAT_REL6 */

#define GSTK_MAX_TEXT_STRING_LEN        256

/* max length of data that requires 2 bytes to indicate length field */
#define GSTK_TLV_LENGTH_1_BYTE_OFFSET_LIMIT    0x7F
#define GSTK_2_BYTE_LENGTH_FIRST_VALUE         0x81
#define GSTK_TAG_LENGTH_LEN                    0x02

#define GSTK_LOWER_NIBBLE_MASK                 (uint8)0x0F

#ifdef FEATURE_GSTK_DUAL_SLOTS

/* Masks for slot initialization, toolkit capability, and card error */

/* slot 1 initialized */
#define GSTK_SLOT1_INITIALIZED_MASK     (uint8)0x01

/* slot 2 initialized */
#define GSTK_SLOT2_INITIALIZED_MASK     (uint8)0x02

/* slot 1 toolkit capability */
#define GSTK_SLOT1_TK_CAPABLE_MASK      (uint8)0x04

/* slot 2 toolkit capability */
#define GSTK_SLOT2_TK_CAPABLE_MASK      (uint8)0x08

/* slot 1 card error */
#define GSTK_SLOT1_CARD_ERROR_MASK      (uint8)0x10

/* slot 2 card error */
#define GSTK_SLOT2_CARD_ERROR_MASK      (uint8)0x20

#endif /* FEATURE_GSTK_DUAL_SLOTS */
/* Mask to find if PID contains STK bound SM */
#define GSTK_STK_SM 0x3F

/* mask and shift value for TON and NPI */
#define GSTK_TON_SHIFT_VALUE 0x04
#define GSTK_TON_MASK_VALUE 0x70
#define GSTK_NPI_MASK_VALUE 0x0F

#define GSTK_TASK_CMD_Q_SIG                   0x00000001
#define GSTK_RPT_TIMER_SIG                    0x00000002
#define GSTK_DO_TP_DL_SIG                     0x00000004
#define GSTK_VIRTUIM_WAIT_SIG                 0x00000008
#define GSTK_TP_TIMER_SIG                     0x00000010
#define GSTK_TERM_RSP_TIMER_SIG1              0x00000020
#define GSTK_TERM_RSP_TIMER_SIG2              0x00000040
#define GSTK_TERM_RSP_TIMER_SIG3              0x00000080
#define GSTK_TERM_RSP_TIMER_SIG4              0x00000100
#define GSTK_TERM_RSP_TIMER_SIG5              0x00000200
#define GSTK_TERM_RSP_TIMER_SIG6              0x00000400
#define GSTK_TERM_RSP_TIMER_SIG7              0x00000800
#define GSTK_TERM_RSP_TIMER_SIG8              0x00001000
#define GSTK_CLASH_WITH_TASK_OFFLINE_SIG      0x00002000 /* RESERVED SIGNAL */
#define GSTK_CLASH_WITH_TASK_STOP_SIG         0x00004000 /* RESERVED SIGNAL */
#define GSTK_CLASH_WITH_TASK_START_SIG        0x00008000 /* RESERVED SIGNAL */
#define GSTK_GSDI_CMD_RSP_SIG                 0x00010000
#define GSTK_RETRY_ENVELOPE_SIG               0x00020000
#define GSTK_SEND_POSTPONE_ENVELOPE_SIG       0x00040000
#define GSTK_NV_SIG                           0x00080000
#ifdef FEATURE_DCTSTK_SUPPORT
#error code not present
#endif /* FEATURE_DCTSTK_SUPPORT */
#define GSTK_TIMER_FOR_ENV_RETRY_SIG          0x00200000
#ifdef  FEATURE_ESTK
#error code not present
#endif /* FEATURE_ESTK */
#define GSTK_CLASH_WITH_ONCRPC_REPLY_SIG      0x00800000 /* RESERVED SIGNAL */
#define GSTK_CLASH_WITH_CAMERA_SIG            0x04000000 /* RESERVED SIGNAL */
#define GSTK_CLASH_WITH_GRP_ACK_SIG           0x08000000 /* RESERVED SIGNAL */
#define GSTK_CLASH_WITH_SEC_OP_COMPLETE_SIG   0x10000000 /* RESERVED SIGNAL */
#define GSTK_CLASH_WITH_VS_OP_COMPLETE_SIG    0x20000000 /* RESERVED SIGNAL */
#define GSTK_CLASH_WITH_FS_OP_COMPLETE_SIG    0x40000000 /* RESERVED SIGNAL */
#define GSTK_CLASH_WITH_REX_SLEEP_SIG         0x80000000 /* RESERVED SIGNAL */

#ifdef FEATURE_GSTK_STATE_TESTER
#define GSTK_STATE_TESTER_TIMER_SIG           0x00800000
#define GSTK_STATE_TESTER_NEXT_TEST_TIMER_SIG 0x00400000
#endif /*FEATURE_GSTK_STATE_TESTER */

#ifdef FEATURE_GSTK_MEMORY_REDUCTION
#define GSTK_NUM_CMD_BUFS  8
#else
#define GSTK_NUM_CMD_BUFS  11
#endif /* FEATURE_GSTK_MEMORY_REDUCTION */

/* command details reference table */
#define GSTK_MAX_PROACTIVE_COMMAND 8
#define GSTK_CMD_DETAILS_REF_TABLE_FULL 0xFF
#define GSTK_NO_MORE_TLV_OFFSET -1

/* client table */
/* LIMITATION: the max should be less than 32 */
#define GSTK_MAX_CLIENT 16

#define GSTK_HELP_AVAILABLE_BOOL_SHIFT 7
/* timer value */
#if defined(FEATURE_APPS_IMAGE_WINMOB) || defined(FEATURE_ESTK)
#define GSTK_TP_CMD_WAIT_TIME   900000
#else
#if defined(FEATURE_GSTK_STATE_TESTER) || \
    defined(FEATURE_GSTK_TEST_AUTOMATION)
#define GSTK_TP_CMD_WAIT_TIME   10000
#else
#define GSTK_TP_CMD_WAIT_TIME   2000
#endif /*FEATURE_GSTK_STATE_TESTER || FEATURE_GSTK_TEST_AUTOMATION */
#endif /* FEATURE_APPS_IMAGE_WINMOB || FEATURE_ESTK */

#ifndef FEATURE_APPS_IMAGE_WINMOB
#define GSTK_TERM_RSP_WAIT_TIME 120000 /* 2 Minutes */
#else
#define GSTK_TERM_RSP_WAIT_TIME 600000 /* 5 Minutes */
#endif /* FEATURE_APPS_IMAGE_WINMOB */
#define GSTK_ENV_RETRY_CMD_WAIT_TIME   10000 /* 10 sec */

/* SET UP MENU BIT MASKS */
#define GSTK_SETUP_MENU_SEL_PREF_BITMASK    0x01
#define GSTK_SETUP_MENU_RFU_MASK            0x7E
#define GSTK_SETUP_MENU_HELP_AVAIL_BITMASK  0x80
#define GSTK_SETUP_MENU_MENU_TITLE_TAG      0x85

/* SELECT ITEM BIT MASKS */
#define GSTK_SEL_ITEM_PRESENTATION_BITMASK        0x01
#define GSTK_SEL_ITEM_PRESENTATION_AS_NAV_BITMASK 0x02
#define GSTK_SEL_ITEM_SEL_PREF_BITMASK            0x04
#define GSTK_SEL_ITEM_HELP_AVAIL_BITMASK          0x80

/* Call control/ MO SMS control term profile index and mask */
#define GSTK_CC_TERM_PROFILE_INDEX           0x01
#define GSTK_CC_TERM_PROFILE_MASK            0x02
#define GSTK_MO_SMS_CTRL_TERM_PROFILE_INDEX  0x01
#define GSTK_MO_SMS_CTRL_TERM_PROFILE_MASK   0x08
#define GSTK_SMS_PP_TERM_PROFILE_INDEX       0x00
#define GSTK_SMS_PP_TERM_PROFILE_MASK        0x02
#define GSTK_CELL_BROADCAST_TERM_PROFILE_INDEX 0x00
#define GSTK_CELL_BROADCAST_TERM_PROFILE_MASK  0x04

/* MANDATORY AND OPTIONAL FIELD SPECIFIER */
#define MANDATORY 0
#define OPTIONAL  1


/* APN Network  ID and APN Operator ID */
#define GSTK_APN_NET_ID 0
#define GSTK_APN_OP_ID 1


/* Min and Max no of bytes in an APN - Spec 11.14 */
#define GSTK_MIN_APN_LEN 1
#define GSTK_MAX_APN_LEN 100

/* Min and Max len of the APN Operator Id labels */
#define GSTK_MIN_OP_LABEL_LEN 0x04
#define GSTK_MAX_OP_LABEL_LEN 0x06

#define RRC_LOCI_UPDATE            1
#define MM_GSTK_LOCI_UPDATE        2

/* SIM Refresh types */
#define GSTK_SIM_REFRESH_RESET 0x04

/* Array size for SAT TP cache */
#define GSTK_SAT_TERMINAL_PROFILE_SIZE  0x02

/* Array size for USAT TP cache */
#define GSTK_USAT_TERMINAL_PROFILE_SIZE 0x06

/* Max number of cells to be measured in UTRAN NMR */
#define GSTK_MAX_UTRAN_NMR_NO_OF_CELLS  0x06

#define GSTK_MIN(A,B) ( (A) < (B) ? (A):(B)) 

/************Defining GSTK_TP_EIGHTEENTH_BYTE ************/

#ifdef FEATURE_CAT_REL6
#ifdef FEATURE_CCAT
#define FEATURE_GSTK_CATREL6_WITH_CCAT
#else /* #ifdef FEATURE_CCAT */
#define FEATURE_GSTK_CATREL6_WITHOUT_CCAT
#endif /* #ifdef FEATURE_CCAT */
#endif /* #ifdef FEATURE_CAT_REL6 */

#if (defined FEATURE_GSM || defined FEATURE_WCDMA)
#error code not present
#endif /* #if (defined FEATURE_GSM || defined FEATURE_WCDMA) */

/* TP 18th byte when CAT_REL6, CCAT , WCDMA, GSM and GPRS are defined */

#if (defined FEATURE_GSTK_CATREL6_WITH_CCAT) && (defined FEATURE_GSTK_GSM_UMTS_WITH_GPRS)
#error code not present
#endif

/* TP 18th byte when CAT_REL6, CCAT , WCDMA, GSM are defined, GPRS is not supported
*/

#if (defined FEATURE_GSTK_CATREL6_WITH_CCAT) && (defined FEATURE_GSTK_GSM_UMTS_WITHOUT_GPRS)
#error code not present
#endif

/* TP 18th byte when CAT_REL6, WCDMA, GSM and GPRS are defined, CCAT is not supported
*/

#if (defined FEATURE_GSTK_CATREL6_WITHOUT_CCAT) && (defined FEATURE_GSTK_GSM_UMTS_WITH_GPRS)
#error code not present
#endif

/* TP 18th byte when CAT_REL6, WCDMA, GSM are defined, CCAT and GPRS are not supported
*/

#if (defined FEATURE_GSTK_CATREL6_WITHOUT_CCAT) && (defined FEATURE_GSTK_GSM_UMTS_WITHOUT_GPRS)
#error code not present
#else /* #if (defined FEATURE_GSTK_CATREL6_WITHOUT_CCAT) && (defined FEATURE_GSTK_GSM_UMTS_WITHOUT_GPRS)*/

/* TP 18th byte when WCDMA, GSM are defined with/without GPRS support,
   CCAT and CAT_REL6 are not supported
*/

#if !(defined FEATURE_CAT_REL6)

#ifdef FEATURE_GSTK_GSM_UMTS_WITH_GPRS
#error code not present
#endif

#ifdef FEATURE_GSTK_GSM_UMTS_WITHOUT_GPRS
#error code not present
#endif

#if !(defined FEATURE_GSM) && !(defined FEATURE_WCDMA)
#define GSTK_TP_EIGHTEENTH_BYTE  0x00
#endif
#else /* #if !(defined FEATURE_CAT_REL6) */

/* TP 18th byte when CAT_REL6 and CCAT are defined,GSM WCDMA and GPRS are not supported
*/

#if !(defined FEATURE_GSM) && !(defined FEATURE_WCDMA) && (defined FEATURE_GSTK_CATREL6_WITH_CCAT)
#define GSTK_TP_EIGHTEENTH_BYTE  0x90
#endif /* #if !(defined FEATURE_GSM) && !(defined FEATURE_WCDMA) && (defined FEATURE_GSTK_CATREL6_WITH_CCAT) */
/* TP 18th byte when CAT_REL6 is defined and CCAT, GSM, WCDMA and GPRS are not supported
*/
#if !(defined FEATURE_GSM) && !(defined FEATURE_WCDMA) && (defined FEATURE_GSTK_CATREL6_WITHOUT_CCAT)
#define GSTK_TP_EIGHTEENTH_BYTE  0x00
#endif /* #if !(defined FEATURE_GSM) && !(defined FEATURE_WCDMA) && (defined FEATURE_GSTK_CATREL6_WITHOUT_CCAT) */

#endif /* #if !(defined FEATURE_CAT_REL6) */
#endif /* #if (defined FEATURE_GSTK_CATREL6_WITHOUT_CCAT) && (defined FEATURE_GSTK_GSM_UMTS_WITHOUT_GPRS)*/

/************Defining GSTK_TP_TWENTY_THIRD_BYTE ************/

/* TP 23rd byte when CAT_REL6 and CCAT are defined,GSM WCDMA and GPRS are not supported
*/

/* For MEID */
#ifdef FEATURE_GSTK_CATREL6_WITH_CCAT
#define GSTK_TP_TWENTY_THIRD_BYTE 0x60
#endif /* FEATURE_GSTK_CATREL6_WITH_CCAT */

/* TP 23rd byte when CAT_REL6 / CCAT are not defined,GSM WCDMA and GPRS are not supported
*/

#if (defined FEATURE_GSTK_CATREL6_WITHOUT_CCAT) || !(defined FEATURE_CAT_REL6)
#define GSTK_TP_TWENTY_THIRD_BYTE 0x40
#endif /* FEATURE_GSTK_CATREL6_WITHOUT_CCAT || !(defined FEATURE_CATREL6) */

/* According to 11.14, 31.11 and 102 223, the terminal response 
 * has following format:
 * byte | description
 * 1    | command details tag (0x81)
 * 2    | the length of command details (0x03 - 3 bytes)
 * 3    | command number
 * 4    | command type
 * ...
 *
 * Hence, the third byte is command number and the fourth byte is the type of 
 * command that the terminal response corresponds to. We are doing sanity check
 * of those two fields agains the item referred by cmd_detail_ref_id in 
 * command_details_ref_table.
 */
/* define byte offset according to above */
#define GSTK_CMD_DETAILS_TAG_OFFSET     0x00
#define GSTK_CMD_DETAILS_LEN_OFFSET     0x01
#define GSTK_CMD_NUMBER_OFFSET          0x02
#define GSTK_CMD_TYPE_OFFSET            0x03

#ifdef FEATURE_GSTK_ENH_MULTI_CLIENT_SUPPORT
#error code not present
#endif /* FEATURE_GSTK_ENH_MULTI_CLIENT_SUPPORT */

/*--------------------------------------------------------------------------
                             Type Defs
--------------------------------------------------------------------------*/


/* gstk state */
typedef enum {
    GSTK_START_S,
    GSTK_TERM_PROFILE_WAIT_S,
    GSTK_GSDI_SIG_WAIT_S,
    GSTK_TERM_PROFILE_RSP_WAIT_S,
    GSTK_IDLE_S,
    GSTK_NO_SIM_S,
    GSTK_POWER_DOWN_S
}
gstk_state_enum_type;

/* gstk envelope processing substate
   NOTE: Currently applicable to only **event download** envelopes 
*/
typedef enum {
    GSTK_SUB_ST_PENDING_EVT_DL_NONE,
    GSTK_SUB_ST_PENDING_EVT_DL_IN_QUEUE,
    GSTK_SUB_ST_PENDING_EVT_DL_RSP_FROM_CARD
}
gstk_env_proc_substate_enum_type;

/* toolkit support enum */
typedef enum {
    GSTK_UNKNOWN_TK_CAPABLE,
    GSTK_NO_TK_CAPABLE,
    GSTK_BOTH_TK_CAPABLE,
    GSTK_ONE_TK_CAPABLE
}
gstk_toolkit_cap_enum_type;

/* timer type */
typedef enum {
    GSTK_TP_TIMER = 0,
    GSTK_TR_TIMER1,
    GSTK_TR_TIMER2,
    GSTK_TR_TIMER3,
    GSTK_TR_TIMER4,
    GSTK_TR_TIMER5,
    GSTK_TR_TIMER6,
    GSTK_TR_TIMER7,
    GSTK_TR_TIMER8,
    GSTK_TR_TIMER9,
    GSTK_TR_TIMER10,
    GSTK_TR_TIMER11,
    GSTK_TR_TIMER12,
    GSTK_TR_TIMER13,
    GSTK_TR_TIMER14,
    GSTK_TR_TIMER15,
    GSTK_TR_TIMER16,
    GSTK_TR_TIMER_INVALID
}
gstk_timer_enum_type;

typedef enum
{
  GSTK_SEARCH_BEGIN_STR =  0,
  GSTK_SEARCH_END_STR,
  GSTK_SEARCH_SINGLE_CHAR,
  GSTK_SEARCH_MAX       = 0xFF
}
gstk_search_string_type;

/* This is used for substration from the channel ID provided by the SIM */
#define GSTK_CHANNEL_ID_BASE      0x20
/* This is the max Channel ID value according to spec */
#define GSTK_CHANNEL_ID_MAX_VALUE 0x27

/* -------------------------------------------------------------------------
                            Terminal Profile Type Defs
---------------------------------------------------------------------------*/

/* List of Tags, e.g., 0x01 or 0x81 = same tag */
typedef enum
{
    GSTK_NO_TAG                                               = 0x00,
    GSTK_COMMAND_DETAILS_TAG                                  = 0x01,
    GSTK_DEVICE_IDENTITY_TAG                                  = 0x02,
    GSTK_RESULT_TAG                                           = 0x03,
    GSTK_DURATION_TAG                                         = 0x04,
    GSTK_ALPHA_IDENTIFIER_TAG                                 = 0x05,
    GSTK_ADDRESS_TAG                                          = 0x06,
    GSTK_CAPABILITY_CONFIGURATION_PARAMETERS_TAG              = 0x07,
    GSTK_CALLED_PARTY_SUB_ADDRESS_TAG                         = 0x08,
    GSTK_SS_STRING_TAG                                        = 0x09,
    GSTK_USSD_STRING_TAG                                      = 0x0A,
    GSTK_SMS_TPDU_TAG                                         = 0x0B,
    GSTK_CELL_BROADCAST_PAGE_TAG                              = 0x0C,
    GSTK_TEXT_STRING_TAG                                      = 0x0D,
    GSTK_TONE_TAG                                             = 0x0E,
    GSTK_ITEM_TAG                                             = 0x0F,
    GSTK_ITEM_IDENTIFIER_TAG                                  = 0x10,
    GSTK_RESPONSE_LENGTH_TAG                                  = 0x11,
    GSTK_FILE_LIST_TAG                                        = 0x12,
    GSTK_LOCATION_INFORMATION_TAG                             = 0x13,
    GSTK_IMEI_TAG                                             = 0x14,
    GSTK_HELP_REQUEST_TAG                                     = 0x15,
    GSTK_NETWORK_MEASUREMENT_RESULTS_TAG                      = 0x16,
    GSTK_DEFAULT_TEXT_TAG                                     = 0x17,
    GSTK_ITEMS_NEXT_ACTION_INDICATOR_TAG                      = 0x18,
    GSTK_EVENT_LIST_TAG                                       = 0x19,
    GSTK_CAUSE_TAG                                            = 0x1A,
    GSTK_LOCATION_STATUS_TAG                                  = 0x1B,
    GSTK_TRANSACTION_IDENTIFIER_TAG                           = 0x1C,
    GSTK_BCCH_CHANNEL_LIST_TAG                                = 0x1D,
    GSTK_ICON_IDENTIFIER_TAG                                  = 0x1E,
    GSTK_ITEM_ICON_IDENTIFIER_LIST_TAG                        = 0x1F,

    GSTK_CARD_READER_STATUS_TAG                               = 0x20,
    GSTK_CARD_ATR_TAG                                         = 0x21,
    GSTK_C_APDU_TAG                                           = 0x22,
    GSTK_R_APDU_TAG                                           = 0x23,
    GSTK_TIMER_IDENTIFIER_TAG                                 = 0x24,
    GSTK_TIMER_VALUE_TAG                                      = 0x25,
    GSTK_DATE_TIME_AND_TIME_ZONE_TAG                          = 0x26,
    GSTK_CALL_CONTROL_REQUESTED_ACTION_TAG                    = 0x27,
    GSTK_AT_COMMAND_TAG                                       = 0x28,
    GSTK_AT_RESPONSE_TAG                                      = 0x29,
    GSTK_BC_REPEAT_INDICATOR_TAG                              = 0x2A,
    GSTK_IMMEDIATE_RESPONSE_TAG                               = 0x2B,
    GSTK_DTMF_STRING_TAG                                      = 0x2C,
    GSTK_LANGUAGE_TAG                                         = 0x2D,
    GSTK_TIMING_ADVANCE_TAG                                   = 0x2E,
    GSTK_AID_TAG                                              = 0x2F,

    GSTK_BROWSER_IDENTITY_TAG                                 = 0x30,
    GSTK_URL_TAG                                              = 0x31,
    GSTK_BEARER_TAG                                           = 0x32,
    GSTK_PROVISIONING_REF_FILE_TAG                            = 0x33,
    GSTK_BROWSER_TERMINATION_CAUSE_TAG                        = 0x34,
    GSTK_BEARER_DESCRIPTION_TAG                               = 0x35,
    GSTK_CHANNEL_DATA_TAG                                     = 0x36,
    GSTK_CHANNEL_DATA_LEN_TAG                                 = 0x37,
    GSTK_CHANNEL_STATUS_TAG                                   = 0x38,
    GSTK_BUFFER_SIZE_TAG                                      = 0x39,
    GSTK_CARD_READER_IDENTIFIER_TAG                           = 0x3A,

    GSTK_USIM_ME_INTERFACE_TRANSPORT_LEVEL_TAG                = 0x3C,

    GSTK_OTHER_ADDRESS_TAG                                    = 0x3E,
    GSTK_ACCESS_TECHNOLOGY_TAG                                = 0x3F,

    GSTK_ESN_TAG                                              = 0x46,
    GSTK_NETWORK_ACCESS_NAME_TAG                              = 0x47,
    GSTK_CDMA_SMS_TPDU_TAG                                    = 0x48,

    GSTK_PDP_CONTEXT_ACTIVATION_PARAM_TAG                     = 0x52,

    GSTK_IMEISV_TAG                                           = 0x62,
    GSTK_BATTERY_STATUS_TAG                                   = 0x63,
    GSTK_NW_SEARCH_MODE_TAG                                   = 0x65,
    GSTK_UTRAN_MEASUREMENT_TAG                                 = 0x69,
    GSTK_MEID_TAG                                             = 0x6D
    /* 0x3B, 0x3D and 0x40-0x46, 0x49-0x4F, 0x50-0x51, 0x53-0x61 not used */
}
tags_enum_type;

typedef enum
{
  GSTK_TLV_PARSING_ERROR,
  GSTK_TLV_PARSING_CONTINUE,
  GSTK_TLV_PARSING_DONE
}
gstk_parsing_result_enum_type;

#define GSTK_TIMER_ELAPSED 1800000 /* 30 minutes */
typedef struct
{
   rex_timer_type timer;
   uint32         time; /* millisecond */
   boolean        timer_started;
   boolean        pending;
   boolean        waiting_rsp;
   rex_timer_type timer_elapsed;
} gstk_rex_timer_type;

/*----------------------------------------------------------------------------
         SIMPLE TLVs that are required for the Terminal Response
-----------------------------------------------------------------------------*/

/* 8.1, 8.14 */
/* map to gstk_address_type */
typedef struct
{
   uint8              tag;                           /* GSTK_ADDRESS_TAG,
                                                       GSTK_SS_STRING_TAG */
   uint8              length;                        /*Q: various size length */
   uint8              ton;                           /* coded in ADN EF */
   uint8              address[GSTK_MAX_BCD_ADDRESS_LEN];
                                                    /* coded as for ADN EF, may
                                                       include DTMF separators
                                                       and DTMF digits */
}
gstk_address_tag_tlv_type;

/* 8.2 */
/* map to gstk_string_type, ignore dcs */
typedef struct
{
    uint8             tag;                           /* GSTK_ALPHA_IDENTIFIER_TAG */
    uint8             length;
    uint8             *alpha;
}
gstk_alpha_tlv_type;

/* 8.3 */
/* map to gstk_subaddress_type */
typedef struct
{
    uint8             tag;                           /* GSTK_CALLED_PARTY_SUB_ADDRESS_TAG */
    uint8             length;                        /*Q: various size length!!! */
    uint8             subaddress[GSTK_MAX_SUBADDR_LEN];         /* include all information specified
                                                          ** in 3G 24.008 except the information element id
                                                          ** and length
                                                          */
}
gstk_subaddress_tlv_type;

/* 8.4 */
/* map to gstk_cap_config_type */
typedef struct
{
    uint8             tag;                                 /* GSTK_CAPABILITY_CONFIGURATION_PARAMETERS_TAG */
    uint8             length; /*Q: various size length!!! */
    uint8             ccp[GSTK_MAX_CCP_LEN];                    /* Capability Configuration Parameters */
               /* ccp is coded as in EF-CCP */
               /* Questionssss What kind of parsing is being done and by whom in the current EF-CCP read?
                  which module is using this information?
                  if the parsing function exists, can other layers use this api to parse?
                  what kind of format is expected for ccp before it is being sent down to lower layer,
                  who expects that? cm? nas????
                  */
}
gstk_cap_config_tlv_type;

/* 8.5 */
/* map to gstk_cb_page_type */
typedef struct
{
  uint8                                    tag;                 /* GSTK_CELL_BROADCAST_PAGE_TAG */
  uint8                                    length;
  uint8                                   *cb_page;
}
gstk_cell_broadcast_page_tag_tlv_type;

/* 8.6 */
typedef struct
{
    uint8             tag;                                 /* GSTK_COMMAND_DETAILS_TAG */
    uint8             length;                              /* GSTK_COMMAND_DETAILS_LEN */
    uint8             command_number;
    uint8             type_of_command;                     /* Type of Command     */
    uint8             qualifier; /*Q: need to return what was in the command before? */
}
gstk_command_details_tlv_type;

/* 8.7 */
typedef enum
{
  /* values based on standard */
   GSTK_NO_DEVICE        = 0x00,
   GSTK_KEYPAD_DEVICE    = 0x01,
   GSTK_DISPLAY_DEVICE   = 0x02,
   GSTK_EARPIECE_DEVICE  = 0x03,
   GSTK_CHANNEL_1_DEVICE = 0x21,
   GSTK_CHANNEL_2_DEVICE = 0x22,
   GSTK_CHANNEL_3_DEVICE = 0x23,
   GSTK_CHANNEL_4_DEVICE = 0x24,
   GSTK_CHANNEL_5_DEVICE = 0x25,
   GSTK_CHANNEL_6_DEVICE = 0x26,
   GSTK_CHANNEL_7_DEVICE = 0x27,
   GSTK_UICC_SIM_DEVICE  = 0x81,
   GSTK_ME_DEVICE        = 0x82,
   GSTK_NETWORK_DEVICE   = 0x83,
   GSTK_MAX_DEVICE       = 0xFF
}
device_identity_e;

typedef struct
{
   uint8              tag;                               /* GSTK_DEVICE_IDENTITY_TAG */
   uint8              device_tag_length;                 /* GSTK_DEVICE_IDENTITY_LEN */
   uint8              source_device;
   uint8              target_device;
}
gstk_device_identities_tag_tlv_type;

/* 8.8 */
/* map to gstk_duration_type */
typedef struct
{
   uint8               tag;                              /* GSTK_DURATION_TAG */
   uint8               length;                           /* GSTK_DURATION_LEN */
   uint8               time_unit;                        /* GSTK_DURATION_MINUTES,
                                                           GSTK_DURATION_SECONDS,
                                                           GSTK_DURATION_TENTHS_OF_SECONDS */
   uint8               time_interval;                    /* 0x00 - reserved
                                                           0x01 - 1 unit
                                                           0x02 - 2 units
                                                            ...
                                                           0xFF - 255 units */
}
gstk_duration_tlv_type;

/* 8.10 */
typedef struct
{
   uint8               tag;                             /* GSTK_ITEM_IDENTIFIER_TAG */
   uint8               length;                          /* GSTK_ITEM_IDENTIFIER_LEN */
   uint8               identifier;                      /* value between 0x01 - 0xFF,
                                                          null item = 0x00 */
}
gstk_item_identifier_tlv_type;

/* 8.12 */
/* map to gstk_general_result_enum_type and gstk_additonal_info_code_enum_type
** in regular terminal response from the gstk_send_terminal_response()
** the same structure is used in terminal response of a set up call/send ss or send ussd
** which has a new call type, in that case, the result, and additional_info can be
** obtained from the members of the gstk_ussd_rsp_extra_param_type and
** gstk_call_ss_rsp_extra_param_type in the gstk_send_terminal_response()
*/
typedef struct
{
    uint8                                    result_tag;           /* GSTK_RESULT_TAG */
    uint8                                    result_tag_length; /*Q: variable size length!!! */
    uint8                                    result;               /* defined in gstk_exp.h */
    uint8                                    *additional_info;      /* defined in gstk_exp.h */
}
gstk_result_tag_tlv_type;

/* 8.13 */
/* map to gstk_sms_tpdu_type */
typedef struct
{
    uint8             tag;                               /* GSTK_SMS_TPDU_TAG */
    uint8             length;                       /*Q: variable size length!!! */
    uint8             sms_tpdu[GSTK_MAX_RAW_SMS_LEN];
}
gstk_sms_tpdu_tag_tlv_type;

/* 8.15, 8.17 */
/* map to gstk_string_type, gstk_ussd_string_type */
typedef struct
{
   uint8              tag;                          /* GSTK_TEXT_STRING_TAG, GSTK_USSD_STRING_TAG */
   uint8              length;                       /* null text string => length == 0 */
   /* Q: variable size length */
   uint8              data_coding_scheme;           /* as in SMS data coding scheme in TS 23.038 */
   uint8              *text;
}
gstk_string_tlv_type;

/* 8.19 */
/* Used in Event Download for Location Status, MO SMS Control and CC
** GSTK will request the mcc, mnc, lac and cell id from RR, RRC or MM
** accordingly
*/

typedef struct
{
   /* based on 3G 24.008 */
   uint8                        mcc_and_mnc[3];
   uint8                        loc_area_code[2];
   uint8                        cell_id[2];        /* Converted from a 16-bit word */
   uint8                        ext_cell_id[2];
}
gstk_location_info_tlv_rsp_type;

typedef struct
{
   /* based on CCAT 3GPP2 C.S0035*/
   uint8                        mcc[2];
   uint8                        imsi_11_12;
   uint8                        sid[2];
   uint8                        nid[2];
   uint8                        base_id[2];
   uint8                        base_lat[3];
   uint8                        base_long[3];
}
gstk_cdma_location_info_tlv_rsp_type;

/* map to gstk_location_info_rsp_type */
typedef struct
{
   uint8                            tag;             /* GSTK_LOCATION_INFORMATION_TAG */
   uint8                            length;          /* GSTK_LOCATION_INFORMATION_LEN */
   gstk_location_info_tlv_rsp_type  location_info;
}
gstk_location_information_tlv_type;

typedef struct
{
   uint8                                  tag;             /* GSTK_LOCATION_INFORMATION_TAG */
   uint8                                  length;          /* GSTK_LOCATION_INFORMATION_LEN */
   gstk_cdma_location_info_tlv_rsp_type   cdma_loc_info;
}
gstk_cdma_location_information_tlv_type;

/* 8.20 */
typedef struct
{
   uint8                         tag;            /* GSTK_IMEI_TAG */
   uint8                         length;         /* GSTK_IMEI_LEN */
   uint8                         imei[GSTK_IMEI_LEN]; /* based on 3G 24.008 */
}
gstk_imei_tlv_type;

typedef struct
{
   uint8                         tag;                /* GSTK_ESN_TAG */
   uint8                         length;             /* GSTK_ESN_LEN */
   uint8                         esn[GSTK_ESN_LEN];  /* based on CAT Spec */
}
gstk_esn_tlv_type;

typedef struct
{
   uint8                         tag;                      /* GSTK_IMEISV_TAG */
   uint8                         length;                   /* GSTK_IMEISV_LEN */
   uint8                         imeisv[GSTK_IMEI_SV_LEN]; /* based on CAT spec */
}
gstk_imeisv_tlv_type;

typedef struct
{
   uint8                         tag;                 /* GSTK_MEID_TAG */
   uint8                         length;              /* GSTK_MEID_LEN */
   uint8                         meid[GSTK_MEID_LEN]; /* based on CAT Spec */
}
gstk_meid_tlv_type;

/* 8.21 */
typedef struct
{
   uint8                         tag;             /* GSTK_HELP_REQUEST_TAG */
   uint8                         length;          /* GSTK_HELP_REQUEST_LEN */
}
gstk_help_request_tlv_type;

/* 8.22 */
typedef struct
{
   uint8                         tag;                 /* GSTK_NETWORK_MEASUREMENT_RESULTS_TAG */
   uint8                         length;              /* GSTK_NMR_LEN */
   uint8                         nmr[GSTK_NMR_LEN];   /* based on 3G 24.008 starting at octet 2 */
}
gstk_nmr_tlv_type;

/* 8.25 */

/* Set Up event proactive command request is transformed to various registration to various clients
** in GSTK, thus, there is no req_type for setup_evt proactive cmd, and GSTK will build the TR based
** on whether the registration with the client's event notification function is successful or not */


/* The possible event that the set up event list
** command that the SIM/USIM can request the ME
** to monitor
*/
typedef enum
{
   /* values based on standard */
   GSTK_MT_CALL                        = 0x00,
   GSTK_CALL_CONNECTED                 = 0x01,
   GSTK_CALL_DISCONNECTED              = 0x02,
   GSTK_LOCATION_STATUS                = 0x03,
   GSTK_USER_ACTIVITY                  = 0x04,
   GSTK_IDLE_SCRN_AVAILABLE            = 0x05,
   GSTK_CARD_READER_STATUS             = 0x06,
   GSTK_LANGUAGE_SELECTION             = 0x07,
   GSTK_BROWSER_TERMINATION            = 0x08,
   GSTK_DATA_AVAILABLE                 = 0x09,
   GSTK_CHANNEL_STATUS                 = 0x0A,
   GSTK_ACCESS_TECH                    = 0x0B,
   GSTK_NW_SEARCH_MODE                 = 0x0E
}
gstk_evt_list_code_enum_type;

/* map to gstk_evt_list_type */

typedef struct
{
   uint8                         tag;             /* GSTK_EVENT_LIST_TAG */
   uint8                         length;          /* variable size length field
                                                 ** according to standard
                                                 ** however, at this point,
                                                 ** since GSTK_MAX_EVENT_LIST_LEN == 11
                                                 ** we can safely set this member
                                                 ** as a uint8
                                                 ** Changes will require when more
                                                 ** events are supported in the
                                                 ** standard */
   uint8                         event_list[GSTK_MAX_EVENT_LIST_LEN]; /* map to gstk_evt_list_code_enum_type */
}
gstk_evt_list_tlv_type;


/* 8.26 */
/* map to gstk_cause_type */
typedef struct
{
   uint8                         tag;             /* GSTK_CAUSE_TAG */
   uint8                         length;          /* valid values: 0, [2, 30] */
   uint8                         *cause;
    /* coded according to Cause call control information element in 3G 24.008,
       starting at octet 3, i.e., remove the  length and IEI information from 3G 24.008
       - radio link timeout is indicated by the cause data object having a value part of 0 length */

}
gstk_cause_tlv_type;

/* 8.27 */


/* map to gstk_location_status_enum_type */
typedef struct
{
   uint8                           tag;             /* GSTK_LOCATION_STATUS_TAG */
   uint8                           length;          /* GSTK_LOCATION_STATUS_LEN */
   uint8                           location_status;  /* = gstk_location_status_enum_type */
                                                   /* GSTK_NORMAL_SERVICE
                                                   ** GSTK_LIMITED_SERVICE
                                                   ** GSTK_NO_SERVICE
                                                   */
}
gstk_location_status_tlv_type;

/* 8.28 */
/* map to gstk_transaction_id_type */
typedef struct
{
   uint8                           tag;             /* GSTK_TRANSACTION_IDENTIFIER_TAG */
   uint8                           length;
   uint8                           *transaction_list;
        /* - each transaction id shall not appear more than once in the list
           - bits 1-4: RFU
             bits 5-7: TI value
             bits 8  : TI flag
           - TI value and flag are coded as defined in 3G 24.007
        */
}
gstk_transaction_id_tlv_type;

/* 8.29 */
typedef struct
{
   uint8                         tag;            /* GSTK_BCCH_CHANNEL_LIST_TAG */
   uint8                         length;
   uint8                         *channel_list;
         /* list of absolute RF channels for BCCH carriers,
         ** it consists of 1-3 BCCH channel sub lists
         ** each ARFCN is represented by 10 bits, with spare bits == 0
         ** section 8.29
         */
}
gstk_bcch_ch_list_tlv_type;

typedef struct
{
     gstk_nmr_tlv_type                  network_measurement;
     gstk_bcch_ch_list_tlv_type         bcch_ch_list;
}
gstk_nmr_bcch_tlvs_type;

typedef struct
{
   uint8                         tag;
   uint8                         length;
   uint8                         *measurement_report_ptr;
}
gstk_nmr_utran_tlv_type;

/* 8.42 */
/* map to gstk_bc_repeat_ind_enum_type */
typedef struct
{
    uint8                         tag;             /* GSTK_BC_REPEAT_INDICATOR_TAG */
    uint8                         length;          /* GSTK_BC_REPEAT_IND_LEN */
    uint8                         bc_repeat_indicator;
}
gstk_bc_repeat_ind_tlv_type;

/* 8.30 */
/* map to gstk_cc_req_action_type
** Procedure:
** length = gstk_cc_req_action_type's length
** type casting based on gstk_cc_req_action_type's call_type
** after casting, gstk_address_string_tlv will be mapped to the correct
** gstk_address_string from the gstk_cc_req_action_type
** map the rest according too
**
** When this tlv is used in the TR of setup call, send ss and send ussd,
** the card can base on the total length field to determine if these
** info are included, and if they are included, the extra result
** field in the corresponding terminal response will
** indicate the response of the CC initiated command.
** and the tag field of the gstk_address_string_tlv union can be used
** to determine what kind of
** call we are switching to
**
** SIM/USIM's CC envelope response is also expressed in this format,
** in that case, GSTK do not populate this structure, instead it will
** populate the gstk_cc_req_action_type for the client directly
** based on the Tag value of the "union", the gstk_cc_req_action_type'
** call type can be set correctly as well as other field
*/
typedef struct
{
   uint8                           tag;            /* GSTK_CALL_CONTROL_REQUESTED_ACTION_TAG */
   uint8                           length;         /* Q:  variable size len */
   /* cc requested action, starting from the first optional tlv in the
   ** envelope response to a Call Control envelope response */

   /* Optional in the envelope response to CC */

   union
    {
        gstk_address_tag_tlv_type          address;         /* set up call */
        gstk_address_tag_tlv_type          ss_string;       /* send ss     */
        gstk_string_tlv_type               ussd_string;     /* send ussd   */
    }gstk_address_string_tlv;

   gstk_subaddress_tlv_type         subaddress;      /* for set up call only */
   gstk_cap_config_tlv_type         ccp1;            /* for set up call only */
   gstk_cap_config_tlv_type         ccp2;            /* for set up call only */
   gstk_alpha_tlv_type              alpha;
   gstk_bc_repeat_ind_tlv_type      bc_repeat_ind;   /* avaliable only if ccp2 is presented */

   /* section 7.3.1.6 response data */
}
gstk_cc_req_action_tlv_type;

/* 8.39 */
/* map to gstk_date_time_rsp_type */
typedef struct
{
   uint8                         tag;            /* GSTK_DATE_TIME_AND_TIME_ZONE_TAG */
   uint8                         length;         /* GSTK_DATE_TIME_AND_TIME_ZONE_LEN */
   gstk_date_time_rsp_type       date_time;      /* for time zone field, FF indicates unknown value */
}
gstk_date_time_tlv_type;

/* 8.41 */
/* map to at_rsp_type */
typedef struct
{
   uint8                         tag;             /* GSTK_AT_RESPONSE_TAG */
/* JAR
** Compiler Error
** Undefined
*/
//   gstk_at_rsp_type             at_rsp;
   /* rsp string */
}
gstk_at_rsp_tlv_type;

/* 8.45 */
typedef struct
{
   uint8                         tag;            /* GSTK_LANGUAGE_TAG */
   uint8                         length;         /* GSTK_LANGUAGE_LEN */
   uint8                         lang_code[2];   /* coded in SMS default 7-bit alphabet, bit 8 = 0 */
}
gstk_lang_tlv_type;

/* 8.46 */
/* GSM technology only */
/* map to gstk_time_Advance_rsp_type */
typedef struct
{
   uint8                         tag;            /* GSTK_TIMING_ADVANCE_TAG */
   uint8                         length;         /* GSTK_TIMING_ADVANCE_LEN */
   gstk_time_advance_rsp_type    timing_advance;
}
gstk_timing_advance_tlv_type;

typedef struct
{
    uint8                         tag;
    uint8                         length;
    uint8                         access_technology;
}
gstk_access_technology_tlv_type;

typedef struct
{
    uint8                         tag;
    uint8                         length;
    uint8                         battery_status;
}
gstk_battery_status_tlv_type;

/* 8.37 */
typedef struct
{
   uint8                         tag;            /* GSTK_TIMER_IDENTITY_TAG */
   uint8                         length;         /* GSTK_TIMER_IDENTITY_LEN */
   uint8                         timer_id;       /* [1, 8], other values == RFU */
}
gstk_timer_id_tlv_type;

/* 8.38 */
typedef struct
{
   uint8                         tag;            /* GSTK_TIMER_VALUE_TAG */
   uint8                         length;         /* GSTK_TIMER_VALUE_LEN */
   gstk_timer_value_type         timer_value;    /* The timer value in the structure
                                                    has to be packed accordinging to
                                                    24.030, even gstk_timer_value_type
                                                    doesn't represent the value in
                                                    24.030 format */
}
gstk_timer_value_tlv_type;

/* 8.51 */
typedef struct
{
   uint8                         tag;            /* GSTK_BROWSER_TERMINATION_CAUSE_TAG */
   uint8                         length;         /* GSTK_BROWSER_TERMINATION_CAUSE_LEN */
   uint8                         cause;          /* 0 = User Terminated
                                                    1 = Error Terminated */
}
gstk_browser_term_cause_tlv_type;

/* 8.52 */
typedef struct
{
    uint8                                    tag;                              /* GSTK_BEARER_DESCRIPTION_TAG */
    uint8                                    bearer_description_length;        /* variable */
    uint8                                    bearer_type;
    uint8                                   *bearer_param;
}
gstk_bearer_description_tlv_type;

/* 8.53 */
typedef struct
{
    uint8                                    tag;                   /* GSTK_CHANNEL_DATA_TAG */
    uint8                                    ch_data_length;        /* variable */
    uint8                                   *ch_data;
}
gstk_ch_data_tlv_type;

/* 8.54 */
typedef struct
{
    uint8                                    tag;                       /* GSTK_CHANNEL_DATA_LEN_TAG */
    uint8                                    ch_data_len_length;        /* GSTK_CHANNEL_DATA_LEN_LEN */
    uint8                                    ch_data_len;
}
gstk_ch_data_len_tlv_type;

/* 8.55 */
typedef struct
{
    uint8                                    tag;                       /* GSTK_BUFFER_SIZE_TAG */
    uint8                                    buffer_size_length;        /* GSTK_BUFFER_SIZE_LEN */
    uint8                                    buffer_size[GSTK_BUFFER_SIZE_LEN];
}
gstk_buffer_size_tlv_type;

/* 8.56 */
typedef struct
{
    uint8                                    tag;                     /* GSTK_CHANNEL_STATUS_TAG */
    uint8                                    ch_status_length;        /* GSTK_CHANNEL_STATUS_LEN */
    uint8                                    ch_status[GSTK_CHANNEL_STATUS_LEN];
}
gstk_ch_status_tlv_type;


/*
** Structure common to all terminal responses
*/

typedef struct
{
   gstk_command_details_tlv_type        command_details;
   gstk_device_identities_tag_tlv_type  device_id;
   gstk_result_tag_tlv_type             result;
}
gstk_terminal_response_header_type;

/*
** Generic
*/
typedef struct
{
 //  gstk_hdr_type                      message_header;
   uint8                               num_bytes;
   gstk_terminal_response_header_type  data;
}
gstk_generic_terminal_response_msg_type;

/*
** Get Inkey or Get Input TR
*/
/* Questions: need message_header? queue_id? client_id?
** replace by D0/D1, etc to indicate to UIM that it is a
** TERMINAL_RESPONSE,
** if gstk is not synchronous response with uim,
them client_id or user_data passed in won't be
tracked all the way down andup?  only during the
instant that GSTK processes the commands?  i.e., to check
for validity? */
/* Questions to UIM: do they look at client_id or user_data if
** we pass down?  i.e., will they return in their confirmation the same info that
gstk passed down? */
typedef struct
{
   uint8                               num_bytes;
   gstk_terminal_response_header_type  header;      /* additional info required when command_result =
                                                   ** GSTK_ME_CURRENTLY_UNABLE_TO_PROCESS_COMMAND
                                                   ** in that case, the additonal info
                                                   ** = gstk_additional_info_code_enum_type
                                                   */
   gstk_string_tlv_type                text_string; /* map to gstk_string_type the clients pass
                                                   ** in the gstk_send_terminal_responsed()
                                                   */
}
gstk_get_input_terminal_response_type;

typedef struct
{
   uint8                               num_bytes;
   gstk_terminal_response_header_type  header;      /* additional info required when command_result =
                                                   ** GSTK_ME_CURRENTLY_UNABLE_TO_PROCESS_COMMAND
                                                   ** in that case, the additonal info
                                                   ** = gstk_additional_info_code_enum_type
                                                   */
   gstk_string_tlv_type                text_string; /* map to gstk_string_type the clients pass
                                                   ** in the gstk_send_terminal_responsed()
                                                   */
   gstk_duration_tlv_type              duration;
}
gstk_get_inkey_terminal_response_type;

/*
** Select Item TR
*/
typedef struct
{
   uint8                               num_bytes;
   gstk_terminal_response_header_type  header;      /* additional info required when command_result =
                                                   ** GSTK_ME_CURRENTLY_UNABLE_TO_PROCESS_COMMAND
                                                   ** in that case, the additonal info
                                                   ** = gstk_additional_info_code_enum_type
                                                   */
   gstk_item_identifier_tlv_type       item_chosen; /* map to the select_item_chosen_item
                                                   ** in the gstk_send_terminal_response()
                                                   */
}
select_item_terminal_response_type;

typedef struct
{
   uint8                               num_bytes;
   gstk_terminal_response_header_type  header;  /* gstk_additional_info is required only
                                               ** when command_result =
                                               ** GSTK_ME_CURRENTLY_UNABLE_TO_PROCESS_COMMAND
                                               ** the additional_info then should be one of the value
                                               ** in gstk_additional_info_code_enum_type */

   /* based on the match between qualifier in the command detail and
   ** gstk_local_info_enum_type, GSTK will make the correct cast to
   ** the right structure in the union
   */
   union{
     gstk_location_information_tlv_type location_information;
     gstk_cdma_location_information_tlv_type cdma_location_information;
     gstk_imei_tlv_type                 imei;
     gstk_imeisv_tlv_type               imeisv;
     gstk_esn_tlv_type                  esn;
     gstk_meid_tlv_type                 meid;
     gstk_nmr_bcch_tlvs_type            nmr_bcch;
     gstk_nmr_utran_tlv_type            nmr_utran;
     gstk_date_time_tlv_type            date_time_zone;
     gstk_lang_tlv_type                 language;
     gstk_timing_advance_tlv_type       timing_advance;
     gstk_access_technology_tlv_type    access_technology;
     gstk_battery_status_tlv_type       battery_status;
   }local_info_rsp;
}
provide_local_info_terminal_response_type;

typedef struct
{
   uint32                              num_bytes;
   gstk_terminal_response_header_type  header;
         /* for send USSD
            additional info required when command_result =
            GSTK_ME_CURRENTLY_UNABLE_TO_PROCESS_COMMAND : gstk_additional_info_code_enum_type
            GSTK_NETWORK_CURRENTLY_UNABLE_TO_PROCESS_COMMAND
                - first byte = cause value of the
                               Cause information from network, bit 8 = 1
            GSTK_USSD_RETURN_ERROR
                - first byte = error value given in the Facility (Return result)
                               information element returned by the network
            GSTK_INTERACTION_WITH_CALL_CONTROL_BY_SIM_TEMPORARY_PROBLEM
                - first byte: 0x00 = no specific cause can be given
                              0x01 = action not allowed
                              0x02 = the type of request has changed       */
   gstk_string_tlv_type               text_string; /* from client response in the
                                                   ** gstk_send_terminal_response()'s
                                                   ** gstk_ussd_rsp_extra_param_type
                                                   */
   /* the following 2 tlv structure types are only required if
   ** the send USSD command has been changed to another type
   ** of command due to call control, i.e., send USSD
   ** is being changed to send SS or set up call commands
   */
   gstk_cc_req_action_tlv_type        cc_req_action;
   gstk_result_tag_tlv_type           result2_for_cc;  /* result for the new type of request
                                                       ** from network that is
                                                       ** initiated by the CC */
     /* when the new call type is SS
        additional info is required when the command_result =
            SUCCESSFUL command results
                - first byte = SS return operation code
                  rest =       SS return parameters
                from the return result component from the network
            GSTK_ME_CURRENTLY_UNABLE_TO_PROCESS_COMMAND : gstk_additional_info_code_enum_type
            GSTK_NETWORK_CURRENTLY_UNABLE_TO_PROCESS_COMMAND
                - first byte = cause value of the
                               Cause information from network, bit 8 = 1
            GSTK_SS_RETURN_ERROR
                - first byte = error value given in the Facility (return result)
                               information element returned by the network
            GSTK_INTERACTION_WITH_CALL_CONTROL_BY_SIM_TEMPORARY_PROBLEM
                - first byte: 0x00 = no specific cause can be given
                              0x01 = action not allowed
                              0x02 = the type of request has changed
     */
     /* when the new call type is Set Up Call
        the additional info is required when command_result =
            GSTK_ME_CURRENTLY_UNABLE_TO_PROCESS_COMMAND = gstk_additional_info_code_enum_type
            GSTK_NETWORK_CURRENTLY_UNABLE_TO_PROCESS_COMMAND
                - first byte = cause value of the
                               Cause information from network, bit 8 = 1
            GSTK_INTERACTION_WITH_CALL_CONTROL_BY_SIM_TEMPORARY_PROBLEM
                - first byte: 0x00 = no specific cause can be given
                              0x01 = action not allowed
                              0x02 = the type of request has changed
     */
}
send_ussd_terminal_response_type;

typedef struct
{
   uint8                               num_bytes;
   gstk_terminal_response_header_type  header;
        /* additional info regular for send SS is required when command_result =
            SUCCESSFUL command results
                - first byte = SS return operation code
                  rest =       SS return parameters
                from the return result component from the network
            GSTK_ME_CURRENTLY_UNABLE_TO_PROCESS_COMMAND : gstk_additional_info_code_enum_type
            GSTK_NETWORK_CURRENTLY_UNABLE_TO_PROCESS_COMMAND
                - first byte = cause value of the
                               Cause information from network, bit 8 = 1
            GSTK_SS_RETURN_ERROR
                - first byte = error value given in the Facility (return result)
                               information element returned by the network
            GSTK_INTERACTION_WITH_CALL_CONTROL_BY_SIM_TEMPORARY_PROBLEM
                - first byte: 0x00 = no specific cause can be given
                              0x01 = action not allowed
                              0x02 = the type of request has changed
          the same applies in the result2 parameters in this structure if the setup
          call is being changed to send SS
        */
        /* additional info for regular setup call when command result =
            GSTK_ME_CURRENTLY_UNABLE_TO_PROCESS_COMMAND = gstk_additional_info_code_enum_type
            GSTK_NETWORK_CURRENTLY_UNABLE_TO_PROCESS_COMMAND
                - first byte = cause value of the
                               Cause information from network, bit 8 = 1
            GSTK_INTERACTION_WITH_CALL_CONTROL_BY_SIM_TEMPORARY_PROBLEM
                - first byte: 0x00 = no specific cause can be given
                              0x01 = action not allowed
                              0x02 = the type of request has changed
          the same applies in the result2 parameters in this structure if the send ss call
          is being changed to setup call
        */

   /* the following 2 tlv structure types are only required if
   ** the send SS or set up call command has been changed to another
   ** type of command due to call control, i.e., send SS
   ** is being changed to send USSD or set up call commands
   ** or a setup call command is being changed to send SS or send USSD
   */
   gstk_cc_req_action_tlv_type        cc_req_action;
   gstk_result_tag_tlv_type           result2_for_cc;
   /* result for the new call type response from the network */
   /* When the call type changed to USSD, additional info is required when the command_result =
            SUCCESSFUL command results
                - first byte = SS return operation code
                  rest =       SS return parameters
                from the return result component from the network
            GSTK_ME_CURRENTLY_UNABLE_TO_PROCESS_COMMAND : gstk_additional_info_code_enum_type
            GSTK_NETWORK_CURRENTLY_UNABLE_TO_PROCESS_COMMAND
                - first byte = cause value of the
                               Cause information from network, bit 8 = 1
            GSTK_SS_RETURN_ERROR
                - first byte = error value given in the Facility (return result)
                               information element returned by the network
            GSTK_INTERACTION_WITH_CALL_CONTROL_BY_SIM_TEMPORARY_PROBLEM
                - first byte: 0x00 = no specific cause can be given
                              0x01 = action not allowed
                              0x02 = the type of request has changed
    */
    /* when the call change to setup call or send SS, additional info is similar to what is
       being specified in the first result tag
    */

   /* this structure is required only if the send SS or set up call
   ** command is being modified to a send USSD after call control
   ** process in the SIM/USIM
   */
   gstk_string_tlv_type          cc_mod_text2;
}
setup_call_send_ss_terminal_response_type;

typedef struct
{
   uint8                               num_bytes;
   gstk_terminal_response_header_type  header;
     /* additional info is required when command_result =
              GSTK_ME_CURRENTLY_UNABLE_TO_PROCESS_COMMAND
        the additional info then will be = gstk_additional_info_code_enum_type */
   gstk_duration_tlv_type              duration;
}
polling_interval_terminal_response_type;

typedef struct
{
   uint8                               num_bytes;
   gstk_terminal_response_header_type  header;
     /* additional info is required when command_result =
              GSTK_ME_CURRENTLY_UNABLE_TO_PROCESS_COMMAND
        the additional info then will be = gstk_additional_info_code_enum_type */
   gstk_at_rsp_tlv_type                at_rsp;
}
run_at_cmd_terminal_response_type;

/* additional info for the following proactive command terminal response types
** are only required if the command_result =
              GSTK_ME_CURRENTLY_UNABLE_TO_PROCESS_COMMAND
        the additional info then will be = gstk_additional_info_code_enum_type */
typedef gstk_generic_terminal_response_msg_type display_text_terminal_response_type;
typedef gstk_generic_terminal_response_msg_type refresh_terminal_response_type;
typedef gstk_generic_terminal_response_msg_type play_tone_terminal_response_type;
typedef gstk_generic_terminal_response_msg_type setup_menu_terminal_response_type;
typedef gstk_generic_terminal_response_msg_type more_time_terminal_response_type;
typedef gstk_generic_terminal_response_msg_type setup_evt_list_terminal_response_type;
typedef gstk_generic_terminal_response_msg_type setup_idle_mode_txt_terminal_response_type;
typedef gstk_generic_terminal_response_msg_type lang_notification_terminal_response_type;

/* additional result for send DTMF terminal response is required when command_result =
              GSTK_ME_CURRENTLY_UNABLE_TO_PROCESS_COMMAND = gstk_additional_info_code_enum_type
              if call ends before completion of send DTMF, additional_info should
              be set to GSTK_NOT_IN_SPEECH_CALL
*/
typedef gstk_generic_terminal_response_msg_type send_dtmf_terminal_response_type;

typedef struct
{
   uint8                               num_bytes;
   gstk_terminal_response_header_type  header;
     /* additional info is required when command_result =
              GSTK_ME_CURRENTLY_UNABLE_TO_PROCESS_COMMAND
        the additional info then will be = gstk_additional_info_code_enum_type */
   gstk_timer_id_tlv_type              timer_id;
   gstk_timer_value_tlv_type           timer_value;
}
timer_management_terminal_response_type;

/* additional result info for launch browser terminal response is required when command_result =
            GSTK_ME_CURRENTLY_UNABLE_TO_PROCESS_COMMAND = gstk_additional_info_code_enum_type
            GSTK_NETWORK_CURRENTLY_UNABLE_TO_PROCESS_COMMAND
                - first byte = cause value of the
                               Cause information from network, bit 8 = 1
            GSTK_LAUNCH_BROWSER_GENERIC_ERROR
                - 00 = no specific cause can be given
                  01 = bearer unavailable
                  02 = browser unavailable
                  03 = ME unable to read the provisioning data
*/
typedef gstk_generic_terminal_response_msg_type launch_browser_terminal_response_type;

/* additional result info for send sms termainl response is required when command_result =
            GSTK_ME_CURRENTLY_UNABLE_TO_PROCESS_COMMAND : gstk_additional_info_code_enum_type
            GSTK_NETWORK_CURRENTLY_UNABLE_TO_PROCESS_COMMAND
                - first byte = cause value of the
                               Cause information from network, bit 8 = 1
            GSTK_SMS_RP_ERROR
                - first byte = cause value in the RP-Cause element of the
                               RP-ERROR message returned by the network,
                               bit 8 = 0
            GSTK_INTERACTION_WITH_CALL_CONTROL_BY_SIM_TEMPORARY_PROBLEM
                - first byte: 0x00 = no specific cause can be given
                              0x01 = action not allowed
                              0x02 = the type of request has changed       */
typedef gstk_generic_terminal_response_msg_type send_sms_terminal_response_type;

/*
** Open Channel TR
*/
typedef struct
{
   uint8                               num_bytes;
   gstk_terminal_response_header_type  header;      /* additional info required when command_result =
                                                   ** GSTK_ME_CURRENTLY_UNABLE_TO_PROCESS_COMMAND
                                                   ** in that case, the additonal info
                                                   ** = gstk_additional_info_code_enum_type
                                                   */
   gstk_ch_status_tlv_type             ch_status;  /* map to the ch_status in the open_ch_extra_param
                                                   ** in the gstk_send_terminal_response()
                                                   */
   gstk_bearer_description_tlv_type    bearer_description;  /* map to the bearer description in the
                                                            ** open_ch_extra_param in the
                                                            ** gstk_send_terminal_response() */
   gstk_buffer_size_tlv_type           buffer_size; /* map to the buffer size in the
                                                    ** open_ch_extra_param in the
                                                    ** gstk_send_terminal_response() */
}
open_ch_terminal_response_type;

/*
** Close Channel TR
*/
typedef gstk_generic_terminal_response_msg_type close_ch_terminal_response_type;

/*
** Send Data TR
*/
typedef struct
{
   uint8                               num_bytes;
   gstk_terminal_response_header_type  header;      /* additional info required when command_result =
                                                   ** GSTK_ME_CURRENTLY_UNABLE_TO_PROCESS_COMMAND
                                                   ** in that case, the additonal info
                                                   ** = gstk_additional_info_code_enum_type
                                                   */
   gstk_ch_data_len_tlv_type           ch_data_len;   /* map to the channel data len in the
                                                      ** send_data_extra_param in the
                                                      ** gstk_send_terminal_response() */
}
send_data_terminal_response_type;

/*
** Receive Data TR
*/
typedef struct
{
   uint8                               num_bytes;
   gstk_terminal_response_header_type  header;      /* additional info required when command_result =
                                                   ** GSTK_ME_CURRENTLY_UNABLE_TO_PROCESS_COMMAND
                                                   ** in that case, the additonal info
                                                   ** = gstk_additional_info_code_enum_type
                                                   */
   gstk_ch_data_tlv_type               ch_data;       /* map to the channel data in the
                                                      ** receive_data_extra_param */
   gstk_ch_data_len_tlv_type           ch_data_remaining_len;   /* map to the channel data len in the
                                                      ** receive_data_extra_param in the
                                                      ** gstk_send_terminal_response() */
}
receive_data_terminal_response_type;

/*
** Get channel Status TR
*/
typedef struct
{
   uint8                               num_bytes;
   gstk_terminal_response_header_type  header;      /* additional info required when command_result =
                                                   ** GSTK_ME_CURRENTLY_UNABLE_TO_PROCESS_COMMAND
                                                   ** in that case, the additonal info
                                                   ** = gstk_additional_info_code_enum_type
                                                   */
   /* map to the ch_status in the get_ch_status_extra_param
   ** in the gstk_send_terminal_response()
   */
   gstk_ch_status_tlv_type             ch_status[GSTK_MAX_GPRS_CHANNELS];
}
get_ch_status_terminal_response_type;

/*
** STK command related types
*/

typedef struct
{
   uint8      data_buffer[GSTK_TP_LEN];
   uint8      data_len;
   uint8      data_offset;
}
gstk_terminal_profile_type;


/*----------------------- Envelope Command (ME -> SIM/USIM) -------------------------------*/


typedef struct
{
   uint8                                 tag;               /* GSTK_EVENT_DOWNLOAD_TAG */
   uint8                                 length;            /* length of the entire evt dl cmd */
   gstk_device_identities_tag_tlv_type   device_id;
   gstk_evt_list_tlv_type                evt_list;          /* the tlv should be of length length since
                                                           ** only 1 event can be notified at a time
                                                           */
}
gstk_envelope_event_dl_cmd_header_type;

typedef gstk_envelope_event_dl_cmd_header_type gstk_generic_envelope_event_dl_command_type;

/* based on gstk_send_envelope_evt_download_command's gstk_evt_dl_ind_type */
typedef struct
{
   gstk_envelope_event_dl_cmd_header_type   event_download_hdr;
   gstk_transaction_id_tlv_type             transacton_id;  /* 1 transaction id in the SETUP
                                                            message from the network */
   /* C */
   gstk_address_tag_tlv_type                address;        /* include when Calling Party BCD number
                                                            is received by ME in the SETUP message */
   gstk_subaddress_tlv_type                 subaddress;     /* include when Calling Party Subaddress
                                                            is received by ME in the SETUP message */
}
gstk_envelope_mt_evt_dl_command_type;

/* based on gstk_send_envelope_evt_download_command's gstk_evt_dl_ind_type */
typedef struct
{
   gstk_envelope_event_dl_cmd_header_type   event_download_hdr;
   /* for "near end" disconnection: source = ME
   ** for "far end" disconnecton:   source = NETWORK
   */
   gstk_transaction_id_tlv_type             transacton_id;
}
gstk_envelope_call_connected_evt_dl_command_type;

/* based on gstk_send_envelope_evt_download_command's gstk_evt_dl_ind_type */
typedef struct
{
   gstk_envelope_event_dl_cmd_header_type   event_download_hdr;
   /* for "near end" disconnection: source = ME
   ** for "far end" disconnecton:   source = NETWORK
   */
   gstk_transaction_id_tlv_type             transacton_id;
   /* Optional */
   gstk_cause_tlv_type                      cause;
}
gstk_envelope_call_disconnected_evt_dl_command_type;

/* based on event information from various client layers */
typedef struct
{
   gstk_envelope_event_dl_cmd_header_type   event_download_hdr;
   gstk_location_status_tlv_type            location_st;
   /* C, required if location status == NORMAL SERVICE */
   union
   {
     gstk_location_information_tlv_type       gsm_umts_location_info;
     gstk_cdma_location_information_tlv_type  cdma_location_info;
   }location_info;
}
gstk_envelope_location_status_evt_dl_command_type;

/* based on event information from various client layers */
typedef struct
{
   gstk_envelope_event_dl_cmd_header_type   event_download_hdr;
   gstk_lang_tlv_type                       language;
}
gstk_envelope_language_evt_dl_command_type;

/* based on event information from various client layers */
typedef struct
{
   gstk_envelope_event_dl_cmd_header_type   event_download_hdr;
   gstk_access_technology_tlv_type          access_tech;
}
gstk_envelope_access_tech_evt_dl_command_type;

/* based on event information from various client layers */
typedef struct
{
   gstk_envelope_event_dl_cmd_header_type   event_download_hdr;
   gstk_browser_term_cause_tlv_type         cause;
}
gstk_envelope_browser_termination_evt_dl_command_type;

/* based on event information from various client layers */
typedef struct
{
   gstk_envelope_event_dl_cmd_header_type   event_download_hdr;
   gstk_ch_status_tlv_type                  ch_status;
   gstk_ch_data_len_tlv_type                ch_data_len;
}
gstk_envelope_data_avail_evt_dl_command_type;

/* based on event information from various client layers */
typedef struct
{
   gstk_envelope_event_dl_cmd_header_type   event_download_hdr;
   gstk_ch_status_tlv_type                  ch_status;
}
gstk_envelope_ch_status_evt_dl_command_type;

/* based on timer id and timer value for the timer expiration */
typedef struct
{
   uint8                                    tag;                 /* GSTK_TIMER_EXPIRY_TAG */
   uint8                                    length;
   gstk_device_identities_tag_tlv_type      device_id;
   gstk_timer_id_tlv_type                   timer_id;
   gstk_timer_value_tlv_type                timer_value;
}
gstk_envelope_timer_expiration_command_type;
/* based on the gstk_cmd_enum_type within gstk_evt_dl_ind_type in
** the gstk_send_envelope_evt_download_command */
typedef gstk_generic_envelope_event_dl_command_type gstk_envelope_user_act_evt_dl_command_type;
typedef gstk_generic_envelope_event_dl_command_type gstk_envelope_idle_scrn_evt_dl_command_type;

/* based on gstk_send_envelope_menu_sel_command's help_requested and item_chosen params */
typedef struct
{
   uint8                                    tag;                 /* GSTK_MENU_SELECTION_TAG */
   uint8                                    length;
   gstk_device_identities_tag_tlv_type      device_id;
   gstk_item_identifier_tlv_type            item_chosen;
   /* Optional */
   gstk_help_request_tlv_type               request_help;
}
gstk_envelope_menu_selection_command_type;

/* based on gstk_send_envelope_sms_pp_dl_command's sms_tpdu and address params */
typedef struct
{
   uint8                                    tag;                 /* GSTK_SMS_PP_DOWNLOAD_TAG */
   uint8                                    length;
   gstk_device_identities_tag_tlv_type      device_id;
   /* Optional */
   gstk_address_tag_tlv_type                address;
   /* Mandatory */
   gstk_sms_tpdu_tag_tlv_type               tpdu;
}
gstk_envelope_sms_pp_download_command_type;

/* based on gstk_envelope_cell_broadcast_download_command_type's cb page params */
typedef struct
{
   uint8                                    tag;                 /* GSTK_SMS_PP_DOWNLOAD_TAG */
   uint8                                    length;
   gstk_device_identities_tag_tlv_type      device_id;
   /* Mandatory */
   gstk_cell_broadcast_page_tag_tlv_type    cb_page;
}
gstk_envelope_cell_broadcast_download_command_type;

/* based on gstk_cc_ind_type from the gstk_send_envelope_cc_command */
typedef struct
{
   uint8                                    tag;                 /* GSTK_CALL_CONTROL_TAG */
   uint8                                    length;
   gstk_device_identities_tag_tlv_type      device_id;
   union {
     gstk_address_tag_tlv_type                address;         /* set up call */
     gstk_address_tag_tlv_type                ss_string;       /* send ss     */
     gstk_string_tlv_type                     ussd_string;     /* send ussd   */
   }gstk_address_string_tlv;
   gstk_cap_config_tlv_type                 ccp1;
   /* Optional */
   gstk_subaddress_tlv_type                 subaddress;
   /* Mandatory location information will be populated in GSTK */
   gstk_location_information_tlv_type       location_info;
   /* Optional */
   gstk_cap_config_tlv_type                 ccp2;
}
gstk_envelope_cc_command_type;

/* based on gstk_mo_sms_ctrl_ind_type from the gstk_send_envelope_mo_sms_ctrl_command */
typedef struct
{
   uint8                                    tag;                 /* GSTK_CALL_CONTROL_TAG */
   uint8                                    length;
   gstk_device_identities_tag_tlv_type      device_id;
   gstk_address_tag_tlv_type                address1;            /* rp_destination_address */
   gstk_address_tag_tlv_type                address2;            /* tp_destination_address */
   /* Mandatory location information will be populated in GSTK */
   gstk_location_information_tlv_type       location_info;
}
gstk_envelope_mo_sms_ctrl_command_type;

/* Used internally in GSTK: represent the Device ID source and destination*/
typedef struct
{
  device_identity_e  src;
  device_identity_e  dest;
} gstk_device_id_type;

/* Used internally in GSTK: represent the general location information*/
typedef struct
{
  gstk_local_info_gen_enum_type       location_info_tech;
  union
  {
    gstk_location_info_rsp_type   gsm_umts_loc_info;
    gstk_cdma_location_info_type  cdma_loc_info;
  } location_info;
}gstk_location_info_gen_type;

/* Used internally in GSTK: contains the index into the cmd details table,
   the command response type expected and the command number.
*/
typedef struct
{
  uint32                        ref_id;
  uint32                        command_rsp_type;
  uint8                         command_number;
}gstk_cmd_info_type;

/* Used internally in GSTK: contains the result and additional result values.
*/
typedef struct
{
   gstk_general_result_enum_type      command_result;
   gstk_additional_info_ptr_type      result_additional_info;
}gstk_result_type;

typedef struct
{
   gstk_device_id_type                     dev_id_data;
   gstk_cmd_info_type                      cmd_info_data;
   gstk_result_type                        result_info_data;
}gstk_cmd_term_rsp_common_type;

/* Used internally in GSTK: represent union structure for different TLV that GSTK is
   responsible for building when sending the terminal response or envelope
   command to the card */
typedef union
{
    gstk_device_id_type                     dev_id_data;
    gstk_location_info_gen_type             location_info_data;
    gstk_pdp_context_act_param_type         pdp_context_act_param_data;
    gstk_cmd_info_type                      cmd_info_data;
    gstk_result_type                        result_info_data;
    gstk_nw_search_mode_enum_type           nw_search_mode_data;
    gstk_evt_list_code_enum_type            evt_list_info_data;
}
gstk_cmd_to_uim_type;

typedef struct
{
    boolean                                 free_slot;
    gstk_command_details_tlv_type           command_details;
    device_identity_e                       destination_dev_id;
    gstk_cmd_enum_type                      expected_cmd_rsp_type;
      /* only _CNF in the gstk_cmd_enum_type is valid in the xpected_cmd_rsp_type
      ** field.  This provides an additional checking mechanism between the
      ** proactive command and terminal response
      ** e.g., command_details.type_of_command = CMD_STK_DISPLAY_TEXT
      ** the expected_cmd_rsp_type = GSTK_DISPLAY_TEXT_CNF */
    rex_timer_type                          tp_rpt_time;
    boolean                                 partial_comprehension;
#if defined(FEATURE_APPS_IMAGE_WINMOB) || defined(FEATURE_GSTK_ENH_MULTI_CLIENT_SUPPORT)
    uint32                                  pending_responses;
#endif /* FEATURE_APPS_IMAGE_WINMOB) || FEATURE_GSTK_ENH_MULTI_CLIENT_SUPPORT */
#ifdef FEATURE_GSTK_ENH_MULTI_CLIENT_SUPPORT
#error code not present
#endif /* FEATURE_GSTK_ENH_MULTI_CLIENT_SUPPORT */
}
gstk_cmd_details_ref_table_type;

typedef struct
{
    boolean                                   free;
    gstk_client_type                          client_type;
    uint32                                    user_data;
    /* NOTE: evt_reg_bitmask is not used if FEATURE_ESTK if defined */
    uint64                                    evt_reg_bitmask;
    gstk_client_reg_cb_func_format_enum_type  evt_cb_type;
    union
    {
      gstk_evt_cb_funct_type                  client_cb;
      gstk_toolkit_evt_cb_func_type           new_client_cb;
    }evt_cb;
    gstk_client_init_cb_func_type             client_init_cmd_cb;
}
gstk_client_table_type;

#ifdef FEATURE_GSTK_ENH_MULTI_CLIENT_SUPPORT
#error code not present
#endif /* FEATURE_GSTK_ENH_MULTI_CLIENT_SUPPORT */

/*  GSTK_RETRY_TYPE 
**  This structure contains the command retry information that is used 
**  in the envelope details reference table.
*/
typedef struct
{
  boolean retry_flag;        /* Should retry be attempted for this envelope */
  int32   retry_counter;     /* retry counter (modified as retries are attempted by gstk */
  int32   max_no_of_retries; /* max number of retries to attempt */      
}
gstk_retry_type;

/*  GSTK_PENDING_ENV_INFO_TYPE 
**  This structure contains the retry and processing state of the event download
**  envelope that is currently being processed.
*/
typedef struct
{
    gstk_env_proc_substate_enum_type  gstk_env_state;
    gstk_retry_type                   retry_info;
    /* actual payload bytes sent through uim */
    gstk_generic_data_type            env_data_bytes;
    /* Refers to the envelope reference id in the envelope reference table */
    uint32                            env_ref_id;
    /* Timer to retry envelope command to the card - 15 seconds timer */
    rex_timer_type                    gstk_env_retry_timer;
}
gstk_pending_env_info_type;

typedef struct
{
    boolean                                 free;
    gstk_client_id_type                     client_id;
    gstk_cmd_enum_type                      expected_cmd_rsp_type;
      /* only _RSP for the envelope response are the valid values */
    gstk_evt_cb_funct_type                  env_cb_ptr;
    uint32                                  user_data;
}
gstk_env_cmd_ref_table_type;

typedef struct
{
    boolean                                 near_end;
    gstk_transaction_id_type                transaction_id;
}
gstk_transaction_id_direction_type;

typedef struct
{
    boolean                      is_loc_info_valid;
    gstk_cdma_location_info_type loc_info;
}
gstk_cdma_loc_info_cache_type;

/*===========================================================================
  
  STRUCT: GSTK_PRIV_DATA_TYPE

  PARAMETERS: 
    setup_evt_list_cnf_cache: the cache of setup evt list terminal response
    client_tp_ptr           : the cache of TPs sent by GSTK clients

  PURPOSE:
    The structure is to hold GSTK private data
==========================================================================*/
typedef struct
{
#if defined(FEATURE_APPS_IMAGE_WINMOB) || (defined(FEATURE_GSTK_ENH_MULTI_CLIENT_SUPPORT) && defined(FEATURE_GSTK_STREAMING_APDU_SUPPORT))
  gstk_setup_evt_list_cnf_type    setup_evt_list_cnf_cache;
#endif /* FEATURE_APPS_IMAGE_WINMOB || (FEATURE_GSTK_ENH_MULTI_CLIENT_SUPPORT && FEATURE_GSTK_STREAMING_APDU_SUPPORT) */
  gstk_terminal_profile_cmd_type  *client_tp_ptr[GSTK_EXPECTED_NUM_OF_TP_DL];
}
gstk_priv_data_type;

#ifdef FEATURE_GSTK_DUAL_SLOTS

/* Information about slot initialization (last 2 bits), toolkit capability
** (middle 2 bits), and card error (first 2 bits). See dual slot masks defined
** in gstk.h.
*/
extern uint8 gstk_toolkit_info;

/* Client type and client ID for GSTK such that it can use the GSTK
library functions */
static gstk_client_type GSTK_GSTK_TYPE;

/* Current toolkit slot */
extern gstk_slot_id_enum_type gstk_curr_tk_slot;

/* Preferred toolkit slot */
extern gstk_slot_id_enum_type gstk_preferred_tk_slot;

#endif /* FEATURE_GSTK_DUAL_SLOTS */

/* CDMA location information cache */
extern gstk_cdma_loc_info_cache_type cdma_location_info_cache;

extern gsdi_card_apps_T gstk_gsdi_card_mode;
#ifdef FEATURE_GSTK_DUAL_SLOTS
#define GSTK_NUM_SLOT 2
#else
#define GSTK_NUM_SLOT 1
#endif /* FEATURE_GSTK_DUAL_SLOTS */

extern gsdi_protocol_enum_type gstk_gsdi_protocol_slot[GSTK_NUM_SLOT];

extern uint8 gstk_terminal_profile_cache[GSTK_TERMINAL_PROFILE_SIZE];
#ifdef FEATURE_GSTK_TEST_AUTOMATION
#error code not present
#endif /* FEATURE_GSTK_TEST_AUTOMATION */
extern gstk_cmd_details_ref_table_type command_details_ref_table[GSTK_MAX_PROACTIVE_COMMAND];
extern gstk_client_table_type gstk_client_table[GSTK_MAX_CLIENT];

#ifdef FEATURE_GSTK_ENH_MULTI_CLIENT_SUPPORT
#error code not present
#endif /* FEATURE_GSTK_ENH_MULTI_CLIENT_SUPPORT */

extern rex_timer_type gstk_tp_wait_timer;  /* 2 seconds timer*/
//extern rex_timer_type gstk_term_rsp_wait_timer;  /* 5 seconds timer*/
extern gstk_rex_timer_type gstk_timer_management[GSTK_MAX_TIMER_SUPPORTED];

#ifdef FEATURE_GSTK_STATE_TESTER
extern rex_timer_type gstk_state_tester_timer; /* 5 seconds timer */
extern rex_timer_type gstk_state_tester_next_test_timer; /* 1 second timer */
#endif /*FEATURE_GSTK_STATE_TESTER */

/* envelope payload, retry, state info of the currently processed envelope */
extern gstk_pending_env_info_type gstk_curr_envelope_info;

/* Flag to indicate if current proactive session involves UI */
extern boolean gstk_is_curr_session_ui_related;

/* envelope command request arrays */
extern gstk_env_cmd_ref_table_type envelope_cmd_ref_table[GSTK_MAX_PROACTIVE_COMMAND];

/* event list */
extern uint8 gstk_evt_list[GSTK_MAX_EVENT_LIST_LEN];

/* client id from cm */
extern cm_client_id_type gstk_client_id_from_cm;

extern boolean gstk_mm_state_is_idle;
extern boolean gstk_cm_call_is_connected;
#ifndef FEATURE_NO_UIM_RECOVERY_SUPPORT
extern boolean gstk_gsdi_internal_sim_reset;
#endif /* FEATURE_NO_UIM_RECOVERY_SUPPORT */

extern uint8 gstk_num_of_tp_dl_received;

/* Table to maintain RRC PLMN/LAC/Cell information */
typedef struct
{
  uint8 plmn_id[GSTK_MAX_PLMN_LEN];
  uint8 lac[GSTK_MAX_LAC_LEN];
  gstk_cell_id_type cell_info;
}gstk_rrc_plmn_lac_cellInfo_table;

/* Table to maintain MM PLMN/LAC/Location Status information */
typedef struct
{
  uint8 plmn_id[GSTK_MAX_PLMN_LEN];
  uint8 lac[GSTK_MAX_LAC_LEN];
  gstk_cell_id_type cell_info;
  gstk_location_status_enum_type loc_status;
}gstk_plmn_lac_cellInfo_table;

#ifdef FEATURE_WCDMA
#error code not present
#endif /* FEATURE_WCDMA */

#if defined(FEATURE_GSM) || defined(FEATURE_WCDMA)
#error code not present
#endif /* defined(FEATURE_GSM) || defined(FEATURE_WCDMA) */

extern uint8 gstk_curr_mcc_mnc[3];
extern uint8 gstk_curr_lac[2];
extern gstk_cell_id_type gstk_curr_cell_id;
extern gstk_location_status_enum_type gstk_curr_location_status;
extern gstk_app_enum_type gstk_curr_app_type;
extern uint8 gstk_next_curr_mcc_mnc[3];
extern uint8 gstk_next_curr_lac[2];
extern gstk_cell_id_type gstk_next_curr_cell_id;
extern gstk_location_status_enum_type gstk_next_curr_location_status;
extern gstk_access_technology_type gstk_curr_rat;
extern gstk_access_technology_type gstk_next_curr_rat;

extern boolean gstk_mm_param_received;
extern boolean gstk_pending_location_evt_envelope;
#ifdef FEATURE_ENHANCED_NW_SELECTION
extern boolean gstk_continue_task;
#endif
extern boolean gstk_profile_received;

/* mmgsdi client id */
extern mmgsdi_client_id_type gstk_mmgsdi_client_id;

/* This variable is set to TRUE when gstk allows UIM to fetch proactive command
   from card. And it is set to FALSE when gstk sends Terminal Response to UIM.
   It is used to check if a proactive command is in progress from card and TR
   has not been sent to it */
extern boolean gstk_proactive_cmd_in_progress;

/* This variable is set to TRUE when gstk deques a BIP Envelope command from its
   command queue. And it is set to FALSE when receives an Envelope response for
   BIP Envelope command.
   It is used to check if a BIP Envelope is in progress from gstk and it has not
   yet received and Envelope response for that. */
extern boolean gstk_bip_env_in_progress;

/* gstk critical section */
extern rex_crit_sect_type gstk_crit_sect; 

/* gstk private data */
extern gstk_priv_data_type gstk_priv_data;

/*===========================================================================
FUNCTION GSTK_WAIT

DESCRIPTION
  Main wait routine for GSTK task.

DEPENDENCIES
  None

RETURN VALUE
  A rex_sigs_type value indicating all signals set (the current state
  of all signals).  The calling unit can then check for the reason for
  exiting the wait.

===========================================================================*/
extern rex_sigs_type  gstk_wait( rex_sigs_type wait_sigs );

/*===========================================================================
FUNCTION GSTK_PROCESS_STATE

DESCRIPTION

  This function dispatches the command to the current state of the GSTK state
  machine.

PARAMETERS
  cmd_p: [Input] Pointer to the command to be processed by various the
                 GSTK state machine

DEPENDENCIES
  None

RETURN VALUE
  gstk_status_enum_type

COMMENTS
  None

SIDE EFFECTS
  None

SEE ALSO
  None
===========================================================================*/
extern gstk_status_enum_type gstk_process_state ( gstk_cmd_type * cmd_p );

/*===========================================================================
FUNCTION gstk_power_down

DESCRIPTION

  This function
  - flushes GSTK queue
  - vote UIM off
  - Ack TMC

PARAMETERS
  None

DEPENDENCIES
  None

RETURN VALUE
  gstk_status_enum_type

COMMENTS
  None

SIDE EFFECTS
  None

SEE ALSO
  None
===========================================================================*/
extern gstk_status_enum_type gstk_power_down ( void );

/*===========================================================================
FUNCTION GSTK_TASK_PUT_CMD_BUF

DESCRIPTION
  This function puts the item at the end of the queue and sets the GSTK cmd 
  queue signal

DEPENDENCIES
  None

RETURN VALUE
  Does not return.
===========================================================================*/
extern void gstk_task_put_cmd_buf (
  gstk_task_cmd_type *cmd_ptr
);

/*===========================================================================
FUNCTION GSTK_TASK_PUT_CMD_BUF_TO_HEAD

DESCRIPTION
  This function puts the item at the front of the queue and sets the GSTK cmd 
  queue signal

DEPENDENCIES
  None

RETURN VALUE
  Does not return.
===========================================================================*/
extern void gstk_task_put_cmd_buf_to_head (
  gstk_task_cmd_type *cmd_ptr
);

/*===========================================================================
FUNCTION gstk_build_timer_expired_cmd

DESCRIPTION

  This function build the timer expiration command based on the passed in
  timer type and put onto the command queue

PARAMETERS
  timer_type - [INPUT] gstk_timer_enum_type, use to build the command
  timer_sig  - [INPUT] The rex signal corresponding to the timer

DEPENDENCIES
  None

RETURN VALUE
  gstk_status_enum_type

COMMENTS
  None

SIDE EFFECTS
  None

SEE ALSO
  None
===========================================================================*/
extern gstk_status_enum_type gstk_build_timer_expired_cmd(
  gstk_timer_enum_type timer_type,
  rex_sigs_type        timer_sig);

/*===========================================================================
FUNCTION gstk_rex_timer_elapsed_expire_cb

DESCRIPTION
  This function will add the elapsed time value to the time field of the
  timer management table and restart the timer_elapsed

PARAMETERS
  param: [Input] Specifies which timer has expired

DEPENDENCIES
  None

RETURN VALUE
  None

COMMENTS
  None

SIDE EFFECTS
  None

SEE ALSO
  None
===========================================================================*/
extern void gstk_rex_timer_elapsed_expire_cb( unsigned long param);

/*===========================================================================
FUNCTION gstk_rex_timer_expire_cb

DESCRIPTION
  This function trigger the timer expiration event envelope command.
  This will only build an envelope command when the timer was started and
  expired

PARAMETERS
  param: [Input] Specifies which timer has expired

DEPENDENCIES
  None

RETURN VALUE
  None

COMMENTS
  None

SIDE EFFECTS
  None

SEE ALSO
  None
===========================================================================*/
extern void gstk_rex_timer_expire_cb ( unsigned long param);

/*===========================================================================
FUNCTION gstk_build_timer_expire_envelope

DESCRIPTION
  This function build an envelope command when the timer was started and
  expired

PARAMETERS
  param: [Input] Specifies which timer has expired

DEPENDENCIES
  None

RETURN VALUE
  None

COMMENTS
  None

SIDE EFFECTS
  None

SEE ALSO
  None
===========================================================================*/
extern void gstk_build_timer_expire_envelope( unsigned long param);

/*===========================================================================
FUNCTION gstk_set_additional_info

DESCRIPTION
  Sets gstk_additional_info_type->additional_info to field.

PARAMETERS
  rec: The record to set the field in.
  out: The field used to set the record with.

DEPENDENCIES
  None

RETURN VALUE
  None

COMMENTS
  None

SIDE EFFECTS
  None

SEE ALSO
  None
===========================================================================*/
extern void gstk_set_additional_info
(
  gstk_additional_info_type*          rec,
  gstk_additional_info_code_enum_type field
);

/*===========================================================================
FUNCTION gstk_set_command_id

DESCRIPTION
  Sets the command id.

PARAMETER
  header: The header to set the command_id for.
  command_id: The value to set the command_id to.

DEPENDENCIES
  None

RETURN VALUE
  None

COMMENTS
  None

SIDE EFFECTS
  None

SEE ALSO
  None
===========================================================================*/
extern void gstk_set_command_id(gstk_hdr_type* header,
                                gstk_cmd_enum_type command_id);

#ifdef FEATURE_ENHANCED_NW_SELECTION
/*===========================================================================
FUNCTION gstk_gsdi_continue_task

DESCRIPTION
  Sets the flag.

PARAMETER
  boolean val.

DEPENDENCIES
  None

RETURN VALUE
  None

COMMENTS
  None

SIDE EFFECTS
  None

SEE ALSO
  None
===========================================================================*/
extern void gstk_gsdi_continue_task(boolean);
#endif

#ifdef FEATURE_STOP_ENV_CMD_PROCESSING
/*===========================================================================
FUNCTION   gstk_check_for_pending_pro_cmd

DESCRIPTION
  This function builds the envelope that has been postponed due to
    terminal response that is required to be sent down first

PARAMETERS
  env_cmd_index: [Input] Index into the envelope command reference table

DEPENDENCIES
  gstk command buffer is available

RETURN VALUE
  gstk_status_enum_type

COMMENTS
  None

SIDE EFFECTS
  None

SEE ALSO
  None
===========================================================================*/
boolean gstk_check_for_pending_pro_cmd(void);
#endif /*  FEATURE_STOP_ENV_CMD_PROCESSING */

#ifdef FEATURE_STOP_SMS_PP_ON_PENDING_SMS_TR
/*===========================================================================
FUNCTION   gstk_check_for_pending_send_sms_term_resp

DESCRIPTION
  This function builds the envelope that has been postponed due to
    terminal response that is required to be sent down first

PARAMETERS
  none

DEPENDENCIES

RETURN VALUE
  boolean TRUE  - if a send sms terminal response is pending
          FALSE - if send sms terminal response is not pending

COMMENTS
  None

SIDE EFFECTS
  None

SEE ALSO
  None
===========================================================================*/
boolean gstk_check_for_pending_send_sms_term_resp(void);
#endif /*  FEATURE_STOP_SMS_PP_ON_PENDING_SMS_TR */

/*===========================================================================
FUNCTION   gstk_cm_ss_event_cb

DESCRIPTION
  Serving system (SS) event callback function, called when an SS event is happening.
  This function populates the various call related indication types and
  puts the command in the GSTK command queue.

PARAMETERS
  ss_event: [Input] CM serving system event
  ss_info_ptr: [Input] Pointer to the ss info struct

DEPENDENCIES
  gstk command buffer is available

RETURN VALUE
  None

COMMENTS
  None

SIDE EFFECTS
  None

SEE ALSO
  None
===========================================================================*/
extern void gstk_cm_ss_event_cb (
  cm_ss_event_e_type         ss_event,
  const cm_mm_ss_info_s_type *ss_info_ptr
);

/*===========================================================================
FUNCTION   gstk_cm_access_tech_cb

DESCRIPTION
  Callback function called as immediate response when GSTK sends a request
  for Access Technology to CM as part of the Proactive Command Provide Local
  Info.

PARAMETERS
  void*                 data_block_ptr,
  cm_ss_cmd_e_type      cmd,
  cm_ss_cmd_err_e_type  cmd_err

DEPENDENCIES
  gstk command buffer is available

RETURN VALUE
  None

COMMENTS
  None

SIDE EFFECTS
  None

SEE ALSO
  None
===========================================================================*/
extern void gstk_cm_access_tech_cb (
  const void*           data_block_ptr,
  cm_ss_cmd_e_type      cmd,
  cm_ss_cmd_err_e_type  cmd_err
);

/*===========================================================================
FUNCTION gstk_uim_polling_report

DESCRIPTION

  This is the callback function for polling response from UIM.
  UIM will call this function when it finishes it's polling operation

PARAMETERS
  report: [Input] Pointer to the UIM polling report

DEPENDENCIES
  None

RETURN VALUE
  None

COMMENTS
  None

SIDE EFFECTS
  None

SEE ALSO
  None
===========================================================================*/
extern void gstk_uim_polling_report (uim_rpt_type *report);

/*===========================================================================
FUNCTION gstk_get_rr_cell_id_info

DESCRIPTION
  Function used to query RR for the Cell Id.

PARAMETER
  Input:   const uint8       * mcc_mnc_p      : Current PLMN Information
  Input:   const uint8       *lac_p           : Current LAC  Information
  Input:   gstk_cell_id_type * cell_id_info_p : Structure to populated
  Output:  cell_id_info_p->cell_id            : cell id
  Output:  cell_id_info_p->cell_id_len        : lenght of cell id

DEPENDENCIES
  RR has to be able to provide a valid Cell ID:

RETURN VALUE
  boolean:  TRUE:  Cell ID information is set.  It may be default data
            FALSE: Cell ID information is not check.  An error occurred.
COMMENTS
  None

SIDE EFFECTS
  None

SEE ALSO
  None
===========================================================================*/
extern boolean gstk_get_rr_cell_id_info(
  const uint8       * mcc_mnc_p,
  const uint8       * lac_p,
  gstk_cell_id_type * cell_id_info_p
);

/*===========================================================================
FUNCTION gstk_get_rrc_cell_id_info

DESCRIPTION
  Function used to query RRC Table information populated by RRC.
PARAMETER
  Input:   uint8  * mcc_mnc_p                 : Current PLMN Information
  Input:   uint8  * lac_p                     : Current LAC  Information
  Input:   gstk_cell_id_type * cell_id_info_p : Structure to populated
  Output:  cell_id_info_p->cell_id            : cell id
  Output:  cell_id_info_p->cell_id_len        : lenght of cell id

DEPENDENCIES
  RRC would have populated the RRC Loci Table

RETURN VALUE
  boolean:  TRUE:  Cell ID information is set.  It may be default data
            FALSE: Cell ID information is not check.  An error occurred.

COMMENTS
  None

SIDE EFFECTS
  None

SEE ALSO
  None
===========================================================================*/
extern boolean gstk_get_rrc_cell_id_info (
  const uint8       * mcc_mnc_p,
  const uint8       * lac_p,
  gstk_cell_id_type * cell_id_info_p
);

/*===========================================================================
FUNCTION GSTK_GET_NV
DESCRIPTION
  Get an item from the nonvolatile memory.

RETURN VALUE
  gstk_status_enum_type
    GSTK_SUCCESS : if NV_DONE
    GSTK_ERROR   : otherwise.

DEPENDENCIES
  This routine is not reentrant.  Shouldn't be a problem, as it doesn't exit
  till we're done.
===========================================================================*/
gstk_status_enum_type gstk_get_nv(
  nv_items_enum_type item,
  nv_item_type *data_ptr
);

#ifdef FEATURE_CAT_REL6
#ifdef FEATURE_CCAT
/*===========================================================================
FUNCTION gstk_cache_1x_location_info

DESCRIPTION
  Caches 1x location information

RETURN VALUE
  None

DEPENDENCIES
  None
===========================================================================*/
void gstk_cache_1x_location_info(gstk_cdma_location_info_type loc_info);

/*===========================================================================
FUNCTION gstk_cache_1x_logstk_copy_1x_info_from_cmcation_info

DESCRIPTION
  Copies 1x related system information from the cm ss info ptr to
  the gstk task cmd ptr.

RETURN VALUE
  SUCCESS
  FAIL

DEPENDENCIES
  None
===========================================================================*/
gstk_status_enum_type gstk_copy_1x_info_from_cm(
  gstk_cdma_location_info_type   *cdma_loc_info,
  const cm_mm_ss_info_s_type     *ss_info_ptr
);
#endif /* #ifdef FEATURE_CCAT */

/*===========================================================================
FUNCTION   gstk_cm_network_search_mode_event_cb

DESCRIPTION
  Callback function called as immediate response when GSTK sends a request
  for Network Search Mode to CM as part of the Proactive Command Provide Local
  Info.

PARAMETERS
  void*                 data_block_ptr,
  cm_ss_cmd_e_type      cmd,
  cm_ss_cmd_err_e_type  cmd_err

DEPENDENCIES
  gstk command buffer is available

RETURN VALUE
  None

COMMENTS
  None

SIDE EFFECTS
  None

SEE ALSO
  None
===========================================================================*/
extern void gstk_cm_network_search_mode_event_cb (
  void*                 data_block_ptr,
  cm_ph_cmd_e_type      cmd,
  cm_ph_cmd_err_e_type  cmd_err
);

/*===========================================================================
FUNCTION   gstk_cm_ph_event_cb

DESCRIPTION
  Phone (PH) event callback function, called when a PH event is happening.
  This function populates the preferred network search mode and
  puts the command in the GSTK command queue.

PARAMETERS
  ph_event: [Input] CM phone event
  ph_info_ptr: [Input] Pointer to the ph info struct

DEPENDENCIES
  gstk command buffer is available

RETURN VALUE
  None

COMMENTS
  None

SIDE EFFECTS
  None

SEE ALSO
  None
===========================================================================*/
extern void gstk_cm_ph_event_cb (
  cm_ph_event_e_type          ph_event,
  const cm_ph_info_s_type    *ph_info_ptr
);
#endif /* FEATURE_CCAT_REL6 */

/*===========================================================================

FUNCTION GSTK_MALLOC

DESCRIPTION
  gstk_malloc calls mem_malloc to request memory from the tmc_heap or tmc_small_heap
  depending on whether or not the request greater than 64KB if
  FEATURE_GSTK_USE_SMALL_HEAP is defined and FEATURE_LOW_MEMORY_USAGE is not.

DEPENDENCIES
  None.

RETURN VALUE
  None.

SIDE EFFECTS
  a pointer is allocated.
===========================================================================*/
void * gstk_malloc(dword size);

/* <EJECT> */
/*===========================================================================

FUNCTION GSTK_FREE

DESCRIPTION
  The gstk_free frees the pointer from the regular tmc_heap or the tmc_heap_small
  based on the pointer address.

DEPENDENCIES
  None.

RETURN VALUE
  None.

SIDE EFFECTS
  the pointer is freed.
===========================================================================*/
void gstk_free(void * ptr);

/*===========================================================================

FUNCTION gstk_offset_memcpy

DESCRIPTION
  gstk_offset_memcpy first checks if there is sufficient memory malloced
  before calling memcpy

PARAMETERS
  dest_ptr:   Pointer to copy data to.
  src_ptr: Pointer to copy data from.
  dest_offset:        offset from where to start copying data to
  copy_size:     size of data to copy
  dest_max_size:      size of destination buffer
  src_max_size: size of source buffer, if 0 then no check on source size done

DEPENDENCIES
  None.

RETURN VALUE
  None

SIDE EFFECTS
  data is copied to the destination buffer
===========================================================================*/
void  gstk_byte_offset_memcpy(
    byte *dest_ptr,
    const void *src_ptr,
    int dest_offset,
    size_t copy_size,
    size_t dest_max_size,
    size_t src_max_size);

/*===========================================================================

FUNCTION gstk_memcpy

DESCRIPTION
  gstk_memcpy first checks if the size requested does not exceed the source
  size and that the size of the data to be copied does not exceed the
  max data size the destination buffer can accomodate before preceeding with
  the memcpy

PARAMETERS
  dest_ptr:   Pointer to copy data to.
  src_ptr: Pointer to copy data from.
  copy_size:     size of data to copy
  dest_max_size:      size of destination buffer
  src_max_size: size of source buffer, if 0 then no check on source size done

DEPENDENCIES
  None.

RETURN VALUE
  None

SIDE EFFECTS
  data is copied to the destination buffer
===========================================================================*/
void  gstk_memcpy(
    void *dest_ptr,
    const void *src_ptr,
    size_t copy_size,
    size_t dest_max_size,
    size_t src_max_size);

/*===========================================================================
FUNCTION gstk_handle_mmgsdi_refresh_cnf

DESCRIPTION
  This is the function that is called from gstk callback from MMGSDI to
  process refresh cnf.

PARAMETERS
  mmgsdi_return_enum_type  mmgsdi_status     - MMGSDI status
  mmgsdi_refresh_cnf_type *refresh_cnf_ptr   - Refresh conf

DEPENDENCIES
  None

RETURN VALUE
  None
COMMENTS
  None

SIDE EFFECTS
  None

SEE ALSO
  None
===========================================================================*/
gstk_status_enum_type gstk_handle_mmgsdi_refresh_cnf (
  mmgsdi_return_enum_type        mmgsdi_status,
  const mmgsdi_refresh_cnf_type *refresh_cnf_ptr
);

/*===========================================================================
FUNCTION GSTK_MMGSDI_CMD_CB

DESCRIPTION
  GSTK Callback for Client Id and Refresh from MMGSDI

PARAMETER
  status  [Input]: status of command returned from MMGSDI
  cnf     [Input]: confirmation type 
  cnf_ptr [Input]: confirmation pointer

DEPENDENCIES
  None

RETURN VALUE
  void

COMMENTS
  None

SIDE EFFECTS
  None

SEE ALSO
  None
===========================================================================*/
void gstk_mmgsdi_cmd_cb(
  mmgsdi_return_enum_type status,
  mmgsdi_cnf_enum_type    cnf,
  const mmgsdi_cnf_type  *cnf_ptr
);

/*===========================================================================

FUNCTION gstk_get_and_store_curr_app_type

DESCRIPTION
  This function stores the current app type obtained from MMGSDI

PARAMETERS
  
DEPENDENCIES
  None.

RETURN VALUE
  None

SIDE EFFECTS
  None
===========================================================================*/
void gstk_get_and_store_curr_app_type( void);

#ifdef FEATURE_GSTK_ENH_MULTI_CLIENT_SUPPORT
#error code not present
#endif /* FEATURE_GSTK_ENH_MULTI_CLIENT_SUPPORT */

/*===========================================================================
FUNCTION   gstk_fill_error_response

DESCRIPTION 
  This function fills uim cmd ptr with error response when the length of 
  terminal response exceeds the max chars limit for uim.

PARAMETERS
  uim_cmd_ptr          :  Pointer to UIM command
  STK_response_header  :  Terminal Response header type
  byte_offset          :  offset
DEPENDENCIES
  None

RETURN VALUE
  gstk_status_enum_type

SIDE EFFECTS
  None

SEE ALSO
  None
===========================================================================*/
extern gstk_status_enum_type gstk_fill_error_response(
  uim_cmd_type                        *uim_cmd_ptr,
  gstk_terminal_response_header_type  STK_response_header,
  int                                 byte_offset
);

#ifdef FEATURE_ESTK
#error code not present
#endif /* FEATURE_ESTK */

/*===========================================================================
FUNCTION   gstk_is_first_cmd_bip_env

DESCRIPTION 
  This function checks if the first command in gstk command queue is BIP 
  Evt Dl or not.

PARAMETERS
  None

DEPENDENCIES
  gstk command queue should be created in advance

RETURN VALUE
  boolean

SIDE EFFECTS
  This will determine if fetch can be handeled by UIM or not. Also it will
  be used to check if BIP envelope can be processed by gstk or not.

SEE ALSO
  None
===========================================================================*/
extern boolean gstk_is_first_cmd_bip_env(void);

#endif /* GSTK_H */

