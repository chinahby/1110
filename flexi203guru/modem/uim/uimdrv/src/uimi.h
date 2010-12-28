#ifndef UIMI_H
#define UIMI_H
/*===========================================================================

                        I N T E R N A L   R U I M
                      T A S K   D E F I N I T I O N S

DESCRIPTION
  This contains all the internal declarations for the RUIM which runs
  in the context of the AUTH Task. It also contains the interface
  between the custom ruim modules and the MAIN RUIM state machine.

Copyright (c) 2000-2009      by QUALCOMM, Incorporated.All Rights Reserved.
===========================================================================*/

/*===========================================================================

                      EDIT HISTORY FOR FILE

$PVCSPath: O:/src/asw/COMMON/vcs/uimi.h_v   1.7   11 Oct 2002 13:04:34   ckrishna  $
$Header: //source/qcom/qct/modem/uim/uimdrv/rel/07H1_2/src/uimi.h#7 $ $DateTime: 2009/12/03 07:12:59 $ $Author: ybansal $

when       who     what, where, why
--------   ---     ----------------------------------------------------------
12/02/09   yb      Fix for voltage class switching G.C.F. failure on ULC
08/10/09   rm      Fixes to have separate count for repeated technical problems
08/03/09   yb      Fix to do get response for seek command if a GCF test case
                   is running or NV_ENS_ENABLED_I NV item is enabled.
04/06/09   js      Support for accessing EF-by-path with proprietary DFs
03/25/09   sun     Added support for Virtuim
02/19/09   sun     Removed MBMS Featurization
11/05/08   vs      Update to allow UIM clock stop only after sending the first
                   instruction to the card in passive mode
10/17/08   nb      Removed redundant extern declaration for uim_atr_pps_done,
                   already declared in uimgen.h
10/07/08   vs      Fix to power up UIM LDO after card power down
09/19/08   tml     Re-select MF to force directory maintainance to MF when
                   received a sw1 warning with normal return
08/26/08   kk      Few comments from uim directory management review
08/22/08   vs      Added USB UICC related poll status
08/14/08   kk      Added handling for SELECT EFs with non-generic file ids
08/08/08   vs      Removed feature UIM_CLOCK_STOP_ALLOWED
07/17/08   vs      Added uim_pet_and_wait() prototype
09/10/07   tml     Added MFLO support
08/16/07   jk      Rearranged featuization to support DATA Auth on 7K
07/18/07   sp      Featurize uim_free_reset_auth_info()
07/05/07   sun     Added support for ODD INS Class
05/21/07   jk      Move recovery data structure to uim.h so it can be exported
02/13/07   pv      Localaze uim_poll_timer and uim_poll_time_delay and
                   provide an API uim_set_poll_timer instead.
02/12/07   pv      Rename clear_timeout to clear_timeout_on_response to be
                   more clear as to what it means.
12/11/06   tml     Add uim_reset_dir_for_slot_channel
11/13/06   pv      Externalize uim_poll_timer and uim_poll_time_delay.
                   Bring the definition for UIM_UICC_INSTRUCTION_CLASS1 out of
                   conditional compilation.
09/29/06   pv      Move uim_clk_busy_wait out of FEATURE_UIM_LOG_APDU_TO_EFS
09/28/06   pv      Externalize uim_process_generic_response uim_me_powerup to
                   be used in uim.c
09/25/06   pv      Export uim_clk_busy_wait which is a wrapper around
                   clk_busy_wait.
08/09/06   pv      Featurize the usage of members of nv_uim_config_params_type
                   under FEATURE_UIM_NO_ADDITIONAL_CONFIG_PARAMS for
                   targets not picking up the NV dependencies
07/17/06   jk      Support for APDU logging to EFS
06/12/06   pv      Re-define macros to use the uim_config_param.
                   Define uim_mode_type.
04/24/06   pv      Externalize uim_config_params to be used by any UIM module.
04/19/06   pv      Removed INLINE for uim_get_file_path
                   Featurize out declarations relating to UIM_DEBUG_LOG.
                   Get UIM_PATH_TO_SEL_MF_INDEX out of feature FEATURE_UIM_UICC.
                   Get uim_poll_pending out of FEATURE_UIM_UICC
03/30/06   pv      Externalize bunch of debug variables to keep track of
                   the general health condition of the uim errors
03/09/06   pv      Externlaize uim_get_file_path
03/01/06   pv      Externalize uim_clear_get_response_cache.
02/15/06   pv      changed the signature uim_malloc to use dword instead of
                   int32.  Corrected the value for UIM_USIM_RETURN_NO_DATA
02/14/06   nk      Merge for: Use heap_small when the size of get response
                   allocation is less than 64 bytes and External variables
                   for UIM chaching mechanism
11/14/05   pv      Lint Fixes
10/27/05   pv      Code to turn off the interface if no ATR is received whthin
                   40,000 UIM clock cycles
11/01/05   tml     Combine last sel EF with last sel Dir
09/13/05   jk      Added definition for minimum interface powerdown time
                   between voltage switch
09/06/05   jk      Changes to support only allowing reading UART Status while
                   there is still power to UART
07/27/05   jk      Made work waiting time array for dual slots
06/13/05   pv      Code to support UIM commands with allocated memory.
06/02/05   pv      Code for FEATURE_UIM_ISIM & FEATURE_UIM_SESSION_MANAGEMENT
05/10/05   pv      BT Sim access changes.  Externalized variables and functions
                   to handle power-up, power-down commands
02/02/05   pv      Increased Max Parity Error count from 10 to 100.
01/21/05   wli     Extend work waiting timer value for an out of spec RUIM card.
12/28/04   pv      externalized variables required for time-stamp based timeout
                   mechanism
11/15/04   pv      Added uim_t_1_bgt variable for block guard time
09/01/04   jk      Added support for HRPD AN Authentication services in R-UIM
09/01/04   ck      Added support for CSIM.
05/17/04   ck      Moved some definitions to support additional information
                   in GSDI link established message
                   Added definitions to support streaming
04/16/04   ts      Fixed problems with error condition processing due to bad
                   merge.
03/26/04   ts      Added max receive error conditions and max receive error
                   flags.  Added a transaction timer for excessive null
                   procedure bytes.
03/15/04   ts      Made uim_toggle_instrn_class a global variable so that uim.c
                   can change it when timing out on a USIM instruction class.
01/27/04   ck      Externalized the function uim_gsdi_link_est
12/19/03   jk      Added definitions for R-UIM 3GPD Services.
09/19/03   ck      Added the definitions for start character in direct
                   convention and inverse convention.
09/02/03   ck      Changed the return type for the funtcion uim_set_cdma_slot.
08/14/03   ck      Added support for dual slots.
06/19/03   ck      Externalised the variable uim_overrun_error.
06/03/03   ck      Added support for status UIM_INSTRN_CHANGE and
                   UIM_CONVENTION_CHANGE.
                   Externalised the counters uim_convention_change_count and
                   uim_instrn_toggle_count.
05/01/03   ck      Externalised the static command buffer
04/28/03   ts      Added support for multi-Voltage classes.
04/28/03   ts      Added support for T=1 protocol.
02/26/03   jk      Added WIM and UTIL support
03/04/03   ck      Moved the constants used for processing a select command
                   from uimgen.c
10/24/02   ts      Added references to work waiting time.
10/11/02   ck      Externalized the variable uim_first_inst_class.
08/12/02   ck      Externalized the function uim_reset_dir and added definition
                   to support select by path reference and added definitions to
                   support select by path reference.
07/09/02   ck      Added curr_app member to last selected directory struct.
02/01/02   ts      Added a new status indicating the card does not support 3V
                   technology.
12/19/01   ck      Removed CDMA and GSM DF definitions.
12/04/01   ts      Added support for UIM toolkit.
05/28/01   ck      Added support for R-UIM, GSM and USIM modules.
01/15/01   ck      Created Module from authi.h rev 1.1 from Common archive.

===========================================================================*/

/* re-define lint based on the lint flag.  Could be put to comdef.h */
#ifdef _lint
  #ifdef LOCAL
    #undef LOCAL
  #endif

  #define LOCAL static
#endif
/*===========================================================================

                     INCLUDE FILES FOR MODULE

===========================================================================*/

#include "comdef.h"
#include "uim.h"
#include "uimdrv.h"
#include "nv.h"

#ifdef FEATURE_UIM_DEBUG_LOG
#include "log.h"
#endif /* FEATURE_UIM_DEBUG_LOG */

#ifdef FEATURE_VIRTUAL_UIM_HW
#error code not present
#endif/*FEATURE_VIRTUAL_UIM_HW*/

/* <EJECT> */
/*===========================================================================

                          DEFINITIONS

===========================================================================*/

#if defined(UIM_DEBUG)
   #define UIM_ERROR_MSG ERR_FATAL
#else
   #define UIM_ERROR_MSG MSG_ERROR
#endif

/* <EJECT> */
/*===========================================================================

                        DATA DECLARATIONS

===========================================================================*/

#define UIM_PATH_DF1_INDEX       0
#define UIM_PATH_DF2_INDEX       1
#define UIM_PATH_EF_INDEX        2
#define UIM_PATH_DF3_INDEX       2
#define UIM_PATH_EF2_INDEX       3
#define UIM_NO_FILE_SEL          0xFFFF
#define UIM_APP_SELECTION        0xFFFF

/* DF3 and EF2 indices are applicable in case where 
   we have UIM path to be selected of length 4 with EF
   at path index 3 for example 
   path - 0x7FFF, 0x5Fxx, 0x4Fxx, 8Fxx */

/* 2 bits contain logical channel information */
#define UIM_LOGICAL_CHANNEL_MASK 0x03


/* Attribute of followed data */
#define  UIM_LOG_ATTRIB_INIT  0x00 /* Init value */
#define  UIM_LOG_TSTAMP       0x01 /* Data is time stamp */
#define  UIM_LOG_TX_DATA      0x10 /* Data is ME to UIM */
#define  UIM_LOG_TX_REPEAT    0x30 /* Re-sent data from ME to UIM */
#define  UIM_LOG_RX_DATA      0x80 /* Data is UIM to ME */
#define  UIM_LOG_RX_REPEAT    0xC0 /* Re-sent data from UIM to ME */

/* These define constants used for processing a select command. */
#define UIM_FILE_ID_LEVEL_MASK   0xF000
#define UIM_FILE_ID_LEVEL_SHIFT  12
#define UIM_MF_LEVEL             3
#define UIM_DF_LEVEL_1           7
#define UIM_DF_LEVEL_2           5
#define UIM_EF_UNDER_MF          2
#define UIM_EF_UNDER_1ST_DF      6
#define UIM_EF_UNDER_2ND_DF      4

/* constants for special category of FILES */
#define UIM_FILE_ID_LEVEL_MASK2       0xFF00
#define UIM_FILE_ID_LEVEL_SHIFT2      8
#define UIM_MF_LEVEL_BYTE             0x3F00
#define UIM_DF_LEVEL_1_BYTE           0x7F00
#define UIM_DF_LEVEL_2_BYTE           0x5F00
#define UIM_EF_UNDER_MF_BYTE          0x2F00
#define UIM_EF_UNDER_1ST_DF_BYTE      0x6F00
#define UIM_EF_UNDER_2ND_DF_BYTE      0x4F00
#define UIM_ADF_LEVEL_BYTE            0x1100

/* This holds the historical characters of the ATR. */
#ifndef FEATURE_VIRTUAL_UIM_HW
#define UIM_MAX_HIST_CHARS       15
#endif/*!FEATURE_VIRTUAL_UIM_HW*/

#define UIM_MF_ID                0x3F00

#if defined (FEATURE_UIM_GSM) || defined (FEATURE_UIM_RUIM)
/* Class of instruction (ISO/IEC 7816-3) */
#define UIM_INSTRUCTION_CLASS    0xA0
#define UIM_INSTRUCTION_CLASS2   0x80
#endif /* FEATURE_UIM_GSM || defined FEATURE_UIM_RUIM */

#ifdef FEATURE_UIM_UTIL
/* This is the main instruction class for UTIL commands.  The logical channel
number (either 1, 2, or 3) is added to this CLA byte while the command is
being built */
#define UIM_UTIL_INSTRUCTION_CLASS1       0x80

/* This is the instruction class for manage channel commands in the util module */
#define UIM_UTIL_INSTRUCTION_CLASS2       0x00
#endif /* FEATURE_UIM_UTIL */

#if defined(FEATURE_UIM_UICC) || defined(FEATURE_UIM_UTIL)
#define UIM_SEL_ADF                     0x1111
#endif /* FEATURE_UIM_UICC || FEATURE_UIM_UTIL */

#define UIM_PATH_TO_SEL_MF_INDEX        0
#define UIM_UICC_INSTRUCTION_CLASS1     0x00

#ifdef FEATURE_UIM_UICC

#define UIM_UICC_INSTRUCTION_CLASS2     0x80
#define UIM_SEL_AID_BY_FILE_ID          0x7FFF

#ifndef FEATURE_VIRTUAL_UIM_HW
#define UIM_USIM_SELECT_BY_FILE_ID      0x00
#define UIM_USIM_SELECT_CHILD_DF        0x01
#define UIM_USIM_SELECT_PARENT_DF       0x03
#define UIM_USIM_SELECT_BY_DF_NAME      0x04
#define UIM_USIM_SELECT_BY_PATH_FROM_MF 0x08
#define UIM_USIM_SELECT_BY_PATH_FROM_DF 0x09

#define UIM_USIM_SESSION_ACTIVATION     0x00
#define UIM_USIM_SESSION_TERMINATION    0x40
#define UIM_USIM_SESSION_NONE           0x60
#endif /* !FEATURE_VIRTUAL_UIM_HW */

#define UIM_USIM_SELECT_FIRST_OCCURENCE 0x00
#define UIM_USIM_SELECT_LAST_OCCURENCE  0x01
#define UIM_USIM_SELECT_NEXT_OCCURENCE  0x02
#define UIM_USIM_SELECT_PREV_OCCURENCE  0x03

#ifndef FEATURE_VIRTUAL_UIM_HW
#define UIM_USIM_RETURN_FCP_TEMPLATE    0x04
#endif /* !FEATURE_VIRTUAL_UIM_HW */

#define UIM_USIM_RETURN_NO_DATA         0x0C

#endif /* FEATURE_UIM_UICC */

#define UIM_ATR_TS_BYTE_INDEX           0
#define UIM_ATR_TS_DIRECT_CONV          0x3B
#define UIM_ATR_TS_INVERSE_CONV         0x3F

#ifndef FEATURE_UIM_NO_ADDITIONAL_CONFIG_PARAMS
  #define UIM_MAX_PARITY_ERR_COUNT        ( 100 + uim_config_params.additional_parity_err_cnt )
  #define UIM_MAX_RX_BREAK_ERR_COUNT      ( 20 + uim_config_params.additional_rx_break_err_cnt )
#else
  #define UIM_MAX_PARITY_ERR_COUNT        100
  #define UIM_MAX_RX_BREAK_ERR_COUNT      20
#endif /* FEATURE_UIM_NO_ADDITIONAL_CONFIG_PARAMS */

#define UIM_MAX_OVERRUN_ERR_COUNT       10

/* Macro for maximum repeated technical problems */
#define UIM_MAX_REPEATED_TECH_PROBLEMS  3

#ifdef FEATURE_UIM_SUPPORT_DUAL_SLOTS
#define UIM_DEFAULT_SLOT_FOR_CDMA      UIM_SLOT_1
#define UIM_DEFAULT_SLOT_FOR_GSM       UIM_SLOT_1
#endif /* FEATURE_UIM_SUPPORT_DUAL_SLOTS */

#if defined(FEATURE_UIM_SUPPORT_3GPD) || defined(FEATURE_UIM_SUPPORT_HRPD_AN)

#ifdef FEATURE_UIM_SUPPORT_3GPD
/* Sizes for the data of APDU blocks */
#ifdef FEATURE_DS_MOBILE_IP
#define UIM_CMPT_IP_AUTH_MAX_MN_HA_BLK_SIZE 254
#define UIM_CMPT_IP_AUTH_MAX_HASH_BLK_SIZE  247
#define UIM_CMPT_IP_AUTH_MAX_MN_AAA_CHALLENGE_SIZE 238
#endif /* FEATURE_DS_MOBILE_IP */

/* These are the P1 codes to specify the option in the APDU */
#define UIM_CMPT_IP_AUTH_P1_CHAP            0

#define UIM_CMPT_IP_AUTH_P1_MN_HA           0x1
#define UIM_CMPT_IP_AUTH_P1_MIP_RRQ         0x2
#define UIM_CMPT_IP_AUTH_P1_MN_AAA          0x3
#endif /* FEATURE_UIM_SUPPORT_3GPD */

#ifdef FEATURE_UIM_SUPPORT_HRPD_AN
#define UIM_CMPT_IP_AUTH_P1_HRPD            0x4
#endif /* FEATURE_UIM_SUPPORT_HRPD_AN */

/* P2 codes to specify which block is being sent */
#define UIM_CMPT_IP_AUTH_P2_FIRST_BLK       0x0
#define UIM_CMPT_IP_AUTH_P2_NEXT_BLK        0x1
#define UIM_CMPT_IP_AUTH_P2_SNGL_BLK        0x2
#define UIM_CMPT_IP_AUTH_P2_LAST_BLK        0x3
#endif /* FEATURE_UIM_SUPPORT_3GPD || FEATURE_UIM_SUPPORT_HRPD_AN */

#ifdef FEATURE_UIM_LOG_TO_EFS
#define UIM_EFSLOG_MAX_BUFFER_SIZE 1000
#define UIM_EFSLOG_MAX_SAVES_TO_EFS 100
#define UIM_EFSLOG_ASCII_CR 0x0D
#define UIM_EFSLOG_ASCII_LF 0x0A
#endif /* FEATURE_UIM_LOG_TO_EFS */

/* The following is the number of milliseconds the interface should remain
powered down when switching voltage classes */

#define UIM_ISO7816_VOLTAGE_SWITCH_PWR_DOWN_DELAY 12
/* 10 Milliseconds per ISO, adding 2 to compensate
Milliseconds for any VCC Rampdown issues. */

#ifdef FEATURE_UIM_CARD_ERR_UIM_LDO_PUP
#error code not present
#endif /* FEATURE_UIM_CARD_ERR_UIM_LDO_PUP */

#ifdef FEATURE_UIM_QSC11X0_DRIVER
/* The following is the additional number of milliseconds the interface should remain
  powered down when switching voltage classes */
  #define UIM_VOLTAGE_CLASS_SWITCHING_DELAY    15
#endif /* FEATURE_UIM_QSC11X0_DRIVER */
/* -------------------------------------------------------------------------
** The following data declarations are representations of the CDMA messages
** found in IS-95A (see sections 6.7 and 7.7).  Each bit in the message is
** represented by a byte in the data structure.
** ------------------------------------------------------------------------ */

typedef PACKED struct
{
  byte randssd[56];     /* Randssd */
  byte proc_control[8]; /* Process Control bits */
  byte esn[32];
  byte esn_pad1[16];
  byte esn_pad2[8];
} uim_update_ssd_req_siz_type;

typedef PACKED struct
{
  byte rands[32];       /* Rands */
} uim_bs_chal_req_siz_type;

typedef PACKED struct
{
  byte authbs[24];      /* Randbs */
} uim_conf_ssd_req_siz_type;

typedef PACKED struct
{
  byte rand_type[8];     /* rand type */
  byte rand[32];         /* rand */
  byte randu[24];        /* rand for unique challenge */
  byte dig_len[8];       /* number of digits */
  byte digits[8];        /* arrray of digits */
  byte proc_control[8];  /* Process control bits */
  byte esn[32];          /* ESN */
  byte esn_pad1[16];     /* padding for ESN */
  byte esn_pad2[8];      /* padding for ESN */
} uim_run_cave_req_siz_type;

typedef PACKED struct {
  byte auth_sig[24];     /* auth signature */
} uim_run_cave_rsp_siz_type;

typedef PACKED struct {
  byte randbs[32];       /* RANDBS */
} uim_bs_chal_rsp_siz_type;

typedef struct {
#ifdef FEATURE_UIM_UICC
  uim_aid_type       adf;
#endif /* FEATURE_UIM_UICC */
  uim_dir_type df;      /* Last first level DF selected */
} uim_df1_type;


typedef struct {
  uim_dir_type file_id;                 /* File ID */
  word file_size;                       /* File size */
  uim_file_structure_type file_struct;  /* Structure of EF */
  word rec_len;                         /* Length of record */
} uim_last_sel_ef_type;

typedef struct {
  uim_df1_type df1;       /* Last first level DF selected */
  uim_dir_type df2;       /* Last second level DF selected */
  uim_dir_type df3;       /* Last third level DF selected */
  uim_dir_type ef;        /* Last EF selected */
  uim_file_type df1_type; /* File type of last selected df */
  boolean      ef_sel;    /* Check to see if an EF has been selected under DF */
#ifdef FEATURE_UIM_UICC
  uim_aid_type curr_app;  /* Current application whose session is open even if
                             we are not inside that AID of that Application */
#endif /* FEATURE_UIM_UICC */
  uim_last_sel_ef_type ef_info;
} uim_last_sel_dir_type;

typedef enum {
  UIM_NO_SUCH_CMD_STATUS = 0,      /* No such command status */
  UIM_CMD_SUCCESS,                 /* Transaction was a success */
  UIM_CMD_ERR,                     /* Transaction was not successful */
#ifdef FEATURE_UIM_TOOLKIT
  UIM_CMD_FETCH,                   /* Command is completed; A proactive command
                                      is pending */
#endif /* FEATURE_UIM_TOOLKIT */
  UIM_CMD_COMPLETED,               /* Command is completed */
  UIM_ICCID_MISMATCH,              /* ICCID mismatch */
  UIM_POLL_ERROR,                  /* Error in polling the card */
  UIM_VOLTAGE_MISMATCH,            /* The card does not support 3V tech */
#if defined( FEATURE_UIM_SUPPORT_MULTIPLE_VOLTAGE_CLASSES )
  UIM_SWITCH_VOLTAGE,              /* Reset the interface and bring up new
                                      voltage */
#endif /* FEATURE_UIM_SUPPORT_MULTIPLE_VOLTAGE_CLASSES */
#ifdef FEATURE_UIM_UICC
  UIM_INSTRN_CHANGE,               /* Change instruction class */
#endif
#ifdef FEATURE_UIM_SUPPORT_INVERSE_CONVENTION
  UIM_CONVENTION_CHANGE,           /* Change the convnetion used in the driver */
#endif
  UIM_CMD_USB_UICC_REPOLL,
  UIM_MAXIMUM_CMD
} uim_cmd_status_type;

typedef enum {
  UIM_NO_SUCH_CMD = 0x0,
  UIM_GENERIC_CMD = 0x1,
  UIM_GSM_CMD     = 0x2,
  UIM_CDMA_CMD    = 0x3,
  UIM_USIM_CMD    = 0x4,
  UIM_UTIL_CMD    = 0x5,
  UIM_ISIM_CMD    = 0x6,
  UIM_MFLO_CMD    = 0x7
} uim_command_mode_type;

#ifndef FEATURE_VIRTUAL_UIM_HW
typedef struct
{
  byte data[ UIM_MAX_HIST_CHARS ];
  byte length;
} uim_hist_char_buf_type;

typedef struct {
  byte num_bytes;              /* number of bytes in ATR */
  byte data[UIM_MAX_ATR_CHARS];/* buffer to hold the ATR */
} uim_atr_buf_type;
#endif /* !FEATURE_VIRTUAL_UIM_HW*/

#ifdef FEATURE_UIM_LOG_TO_EFS
typedef struct {
  char *data;
  int  buffer_size;
} uim_efslog_buffer_type;
#endif /* FEATURE_UIM_LOG_TO_EFS */

/* Variable to indicate if a re-selection to the MF has to be done
 * owing to the failure of the last select command
 */
extern boolean uim_reselect_mf;

/* Variable to indicate if sw1 warning1 that needs to be treated as normal
** end has occured or not */
extern boolean uim_sw1_warnings1_normal_end_occured;

/* Indicates we have received a task off from MC.  We must power the interface
   down. */
extern boolean uim_powering_down_task;

/* Additional Information required for read binary to handle more than 256 */
typedef struct {
  word  num_bytes; /* number of bytes to be read */
  byte *data_ptr; /* ptr to hold the read data */
  word  offset; /* current offset uim would read */
  byte *alloc_mem;
  word  num_bytes_rsp;
} uim_rd_wr_info_type;

/* Structure to hold the get response information that we receive
 * back from the card
 */
typedef struct {
  boolean  is_cached;
  word     data_size;
  byte   * data;
} uim_get_rsp_cache_type;

/* Variable to hold if the current select going down to the driver
 * is a intermediate select ( a df in the path )or the final select
 */
extern boolean uim_intermediate_sel;

/* Variable to indicate to the driver whether or not it should issue
 * a get response if it finds status words indicating a get response
 * expected
 */
extern boolean uim_item_sel_rsp_cached;

/* Variable to hold the cache item to look for or store the
 * get response information
 */
extern uim_get_rsp_cache_type *select_rsp_cache_item_ptr;

extern uim_cmd_type uim_static_cmd_buf;   /* Static command buffer */

/* Historical characters */
extern uim_hist_char_buf_type uim_hist_char_buf[UIM_NUM_DRV_SLOTS];

/* Buffer to hold the ATR */
extern uim_atr_buf_type uim_atr_buf[UIM_NUM_DRV_SLOTS];

extern rex_timer_type uim_delay_timer;    /* Command Response Timer */

extern rex_timer_type uim_cmd_rsp_timer;  /* Command Response Timer */

extern rex_timer_type uim_transaction_rsp_timer;  /* Command Response Timer */

/* Request buffer for a UIM command */
extern uim_req_buf_type uim_req_buf;

/* Response buffer */
extern uim_rsp_buf_type *uim_rsp_buf_ptr;

/* Buffer to hold report to be returned to task issuing command */
extern uim_rpt_type uim_rpt_buf;

/* This array contains the path to be selected */
extern word uim_path_to_be_sel[4];

/* Contains the last selected directories */
extern uim_last_sel_dir_type uim_last_sel_dir[UIM_MAX_CHANNELS][UIM_NUM_DRV_SLOTS];

/* Flag to indicate that a poll is pending */
extern boolean uim_poll_pending;

#if defined (FEATURE_UIM_UICC)
/* First instruction class for the command */
extern nv_uim_first_inst_class_type uim_first_inst_class;

/* Flag to indicate if the instruction class needs to be changed */
extern boolean uim_toggle_instrn_class;

/* Instruction toggle counter */
extern byte uim_instrn_toggle_count;
#if defined (FEATURE_UIM_SUPPORT_MULTIPLE_VOLTAGE_CLASSES)
/* Holds whether we got the voltage information from ATR or not */
extern boolean voltage_class_known_from_atr;
#endif

#endif /* FEATURE_UIM_UICC */

/* These hold the parameters for the work waiting time */
extern rex_timer_cnt_type  uim_WI[UIM_NUM_DRV_SLOTS];
extern rex_timer_cnt_type  uim_work_waiting_time[UIM_NUM_DRV_SLOTS];
extern rex_timer_cnt_type  uim_original_work_waiting_time[UIM_NUM_DRV_SLOTS];
extern rex_timer_cnt_type  uim_extended_work_waiting_time[UIM_NUM_DRV_SLOTS];

#ifdef UIM_DRIVER_TIMESTAMP
/* flag to indicate whether the timer is currently ON */
extern boolean uim_is_timer_on;

/* Variable to hold the work waiting time in terms of SLEEP ticks */
extern int32 uim_work_waiting_time_sleep_ticks[UIM_NUM_DRV_SLOTS];
#endif

#if defined(FEATURE_UIM_T_1_SUPPORT)
extern rex_timer_cnt_type  uim_t_1_bgt;
#endif

#if defined( FEATURE_UIM_T_1_SUPPORT )
extern nv_uim_pref_protocol_type uim_nv_pref_protocol; /* Preferred protocol */
#endif /* FEATURE_UIM_T_1_SUPPORT */

#ifdef FEATURE_UIM_SUPPORT_INVERSE_CONVENTION
/* Number of times the convention (inverse/direct) is changed */
extern byte uim_convention_change_count;
#endif

/* Flag to indicate if the driver has encountered an overrun error */
extern boolean uim_overrun_error;

/* Flag to indicate that un-known status words have been received */
extern boolean uim_bad_status_words_error;

extern uim_dfs_present_type uim_dfs_1;      /* Technology present in UIM1 */
/* Flag to indicate if the driver has maxed the parity error count */
extern boolean uim_max_parity_error;

/* Flag to indicate if the driver has maxed the parity error count */
extern boolean uim_max_overrun_error;

/* Flag to indicate if the driver has maxed the parity error count */
extern boolean uim_max_rx_break_error;

#ifdef UIM_DRIVER_TIMESTAMP
/* Flag to indicate if the driver has signaled a timeout signal
 * before the real-timeout because the driver received a byte but the
 * time stamp indicates that exceeded the timeout value
 */
extern boolean uim_timeout_signaled_by_driver;

/* Variable to hold the difference in time-stamps in SLEEP ticks */
extern int32 uim_num_sleep_ticks_elapsed;

#endif /* UIM_DRIVER_TIMESTAMP */

#ifdef FEATURE_UIM_SUPPORT_DUAL_SLOTS
extern uim_dfs_present_type uim_dfs_2;      /* Technology present in UIM2 */

extern uim_slot_type uim_current_slot;      /* Operational slot for commands */

/* Contains the last selected directories */
extern uim_last_sel_dir_type uim_last_sel_dir_2;

/* Slot for polling during call and for toolkit purposes in dual slots */
extern uim_slot_type uim_polling_slot;

/* Preferred slots for CDMA and GSM */
extern uim_slot_type uim_pref_cdma_slot;
extern uim_slot_type uim_pref_gsm_slot;

/* Active slot for Telecom */
extern uim_slot_type uim_telecom_slot;
#endif

#ifdef FEATURE_UIM_SUPPORT_CSIM
extern uim_last_sel_dir_type uim_csim_last_sel_dir[UIM_NUM_DRV_SLOTS];
#endif /* FEATURE_UIM_SUPPORT_CSIM */

extern uim_sw1_type uim_get_resp_sw1; /* Status Word 1 used for Get Resp */
extern uim_sw2_type uim_get_resp_sw2; /* Status word 2 used for Get Resp */

/* Indicates the status of the UIM */
extern uim_status_type uim_status;

#ifdef FEATURE_UIM_SUPPORT_DUAL_SLOTS
extern uim_status_type uim_status_2;

extern void uim_set_uim_status_for_slot(
uim_status_type stat,
uim_slot_type slot
);

#endif

extern uim_voter_type uim_ctl;

/* Indicates if UIM can be powered down */
extern boolean uim_powerdown;

/* Holds the current channel information */
extern uim_channel_type uim_cur_channel;
/* Holds clock info */
extern boolean uim_clock_is_on;
extern boolean uim_clock_is_on_2;
extern boolean clear_timeout_on_response;

/* Max number of bytes can be logged */
#define LOG_UIM_MAX_CHARS   247

/* Time stamp size */
#define LOG_UIM_TSTAMP_SIZE  8

#ifdef FEATURE_UIM_DEBUG_LOG
  /* Define data structure */
  LOG_RECORD_DEFINE(LOG_UIM_DATA_C)
    byte  length;  /* Actual data length */
    byte  data[LOG_UIM_MAX_CHARS + LOG_UIM_TSTAMP_SIZE];
  LOG_RECORD_END

  typedef LOG_UIM_DATA_C_type uim_log_data_type;
  extern uim_log_data_type uim_log_data;
#endif /* FEATURE_UIM_DEBUG_LOG */


/* Debug variable to maintain total parity error count since power_up */
extern dword uim_tot_parity_error_count;

/* Debug variable to maintain total overrun error count since power_up */
extern dword uim_tot_overrun_error_count;

/* Debug variable to maintain total rx_break error count since power_up */
extern dword uim_tot_rx_break_error_count;


/* Debug variable to maintain total parity error count while in idle state */
extern dword uim_idle_parity_error_count;

/* Debug variable to maintain total overrun error count while in idle state */
extern dword uim_idle_overrun_error_count;

/* Debug variable to maintain total rx_break error count while in idle state */
extern dword uim_idle_rx_break_error_count;

/* Varible to hold configurable parameters stored in the NV */
extern nv_uim_config_params_type uim_config_params;

/* Indicates if the mobile is to be powered up and is set to FALSE when
   the power up is complete */
extern boolean uim_me_powerup;

/* Indicates whether UIM clock stop is allowed in passive mode */
extern boolean uim_passive_clk_stop_allowed;

/* Variable to indicate that GCF testing is being done */
extern boolean uim_gcf_testing_flag;

/* Variable to indicate that nv item NV_ENS_ENABLED_I is enabled */
extern boolean uim_nv_ens_enabled_flag;

/* <EJECT> */
/*===========================================================================

                      PUBLIC FUNCTION DECLARATIONS

===========================================================================*/

/*=========================================================================*/

/* <EJECT> */
/*===========================================================================

FUNCTION UIM_UNSOLICITED_RSP_CALLBACK

DESCRIPTION
  This procedure processes the response the unsolicited response that has
  been recd from the UIM.

DEPENDENCIES
  None

RETURN VALUE
  None

SIDE EFFECTS
  Sets the signal UIM_UNSOLICITED_RSP_SIG

===========================================================================*/

extern void uim_unsolicited_rsp_callback
(
    void
);

/* <EJECT> */
/*===========================================================================

FUNCTION UIM_COMMAND_RESPONSE_CALLBACK

DESCRIPTION
  This procedure initiates the processing of the response that has been recd
  from the UIM.

DEPENDENCIES
  None

RETURN VALUE
  None

SIDE EFFECTS
  Sets the UIM_CMD_RSP_SIG.

===========================================================================*/

extern void uim_command_response_callback
(
    void
);

/* <EJECT> */
/*===========================================================================

FUNCTION UIM_RESET_DIR

DESCRIPTION
  This procedure resets all the members of the last selected directory
  to their defaults.

DEPENDENCIES
  None

RETURN VALUE
  None

SIDE EFFECTS
  Resets the directory structure.

===========================================================================*/
extern void uim_reset_dir
(
    void
);

/*===========================================================================

FUNCTION UIM_RESET_DIR_FOR_SLOT_CHANNEL

DESCRIPTION
  This procedure resets all the members of the last selected directory
  to their defaults for the respective slot on the respective channel.

DEPENDENCIES
  None

RETURN VALUE
  None

SIDE EFFECTS
  Resets the directory structure for the slot specified.

===========================================================================*/
extern void uim_reset_dir_for_slot_channel
(
  uim_drv_slot_type drv_slot,
  uim_channel_type  channel
);

#ifdef FEATURE_UIM_SUPPORT_DUAL_SLOTS
/* <EJECT> */
/*===========================================================================

FUNCTION UIM_RESET_DIR_FOR_SLOT

DESCRIPTION
  This procedure resets all the members of the last selected directory
  to their defaults for the respective slot.

DEPENDENCIES
  None

RETURN VALUE
  None

SIDE EFFECTS
  Resets the directory structure for the slot specified.

===========================================================================*/
void uim_reset_dir_for_slot
(
  uim_slot_type slot
);

/* <EJECT> */
/*===========================================================================

FUNCTION UIM_SET_CDMA_SLOT

DESCRIPTION
  This procedure sets the slot for CDMA app based on an internal logic

DEPENDENCIES
  The NV items for Preferred slots for application must be valid.

RETURN VALUE
  None

SIDE EFFECTS
  Sets the slot for CDMA application

===========================================================================*/
extern uim_slot_type uim_set_cdma_slot
(
  void
);

/* <EJECT> */
/*===========================================================================

FUNCTION UIM_SET_GSM_SLOT

DESCRIPTION
  This procedure sets the slot for GSM app based on an internal logic

DEPENDENCIES
  The NV items for Preferred slots for CDMA and GSM apps must be valid.

RETURN VALUE
  None

SIDE EFFECTS
  Sets the slots for GSM applications.

===========================================================================*/
extern void uim_set_gsm_slot
(
  boolean uim_powerup
);

#endif /* FEATURE_UIM_SUPPORT_DUAL_SLOTS */

/* <EJECT> */
/*===========================================================================

FUNCTION UIM_GSDI_LINK_EST

DESCRIPTION
  This procedure sends a link established command to GSDI

DEPENDENCIES
  None

RETURN VALUE
  None

SIDE EFFECTS
  None
===========================================================================*/
extern void uim_gsdi_link_est
(
  void
);

/*===========================================================================

FUNCTION UIM_GSDI_LINK_EST

DESCRIPTION
  This procedure can be used to report error for the commands still
  remaining in the UIM command queue.

DEPENDENCIES
  None

RETURN VALUE
  None

SIDE EFFECTS
  None
===========================================================================*/
extern void uim_flush_command
(
uim_flush_type flush_type
);

/*===========================================================================

FUNCTION UIM_MALLOC

DESCRIPTION
  The uim_malloc called TMC alloc with different heap_size based the size
  of the allocation required.

DEPENDENCIES
  None.

RETURN VALUE
  None.

SIDE EFFECTS
  a pointer is allocated.
===========================================================================*/
void * uim_malloc(dword size);

/* <EJECT> */
/*===========================================================================

FUNCTION UIM_FREE

DESCRIPTION
  The uim_free free the pointer from the regular tmc_heap or the tmc_heap_small
  based on the pointer address.

DEPENDENCIES
  None.

RETURN VALUE
  None.

SIDE EFFECTS
  the pointer is freed.
===========================================================================*/
void uim_free(void * ptr);


/*===========================================================================

FUNCTION UIM_NOTIFY_ERRROR

DESCRIPTION
  This function notifies the higher layer (GSDI) if UIM encounters an
  unexpected error during power up or an internal wake up and passes the
  status of the UIM.

DEPENDENCIES
  None

RETURN VALUE
  None

SIDE EFFECTS
  None

===========================================================================*/
extern void uim_notify_error
(
  uim_status_type uim_status_param
  #ifdef FEATURE_UIM_SUPPORT_DUAL_SLOTS
  ,uim_slot_type uim_slot_param
  #endif /* FEATURE_UIM_SUPPORT_DUAL_SLOTS */
);


/*===========================================================================

FUNCTION UIM_CLEAR_GET_RESPONSE_CACHE

DESCRIPTION
  This procedure clears all the cache maintained by uim to cache the
  get response data.

DEPENDENCIES
  None

RETURN VALUE
  None

SIDE EFFECTS
  All the cache is cleared.  Which would mean that the selects from now
  on for the first time would do a get response

===========================================================================*/
extern void uim_clear_get_response_cache(void);

/*===========================================================================

FUNCTION UIM_GET_FILE_PATH

DESCRIPTION
  This function takes as parameters the table index and the item index of an
  EF and returns the full path to the EF.

DEPENDENCIES
  The path is determined from the item in the command.  Some items are
  identified as EFs selected by using the path field of the command.  This
  function will return the path field when these items are found in the
  item field of the command.

RETURN VALUE
  Returns a pointer to the path of the item.

SIDE EFFECTS
  Will return the path field of the command for those items that are selected
  by using the command path.

  Will update uim_item_sel_rsp_cached to indicate whether or not the driver
  needs to do a get response based on whether or not the cache
  for the response is populated.

  Exports the concerned pointer which holds or supposed to hold the
  select response

===========================================================================*/
extern const word* uim_get_file_path
(
uim_items_enum_type item,    /* EF to be accessed if file type is EF*/
word const *path
/* Selection of an EF by the specified path */
);

#ifdef FEATURE_UIM_LOG_APDU_TO_EFS

/*===========================================================================

FUNCTION UIM_EFSLOG_APDU_TO_BUFFER

DESCRIPTION
  This function takes a character to be stored to the buffer and an attribute.
  It converts the character to ascii, and checks the attribute to see if it
  differs from the previous character's attribute.  If it does not, then it
  simply places the ASCII value of the character in the next space in the buffer
  (One character will become two ASCII bytes).  If the attribute is different,
  it will place a new line character '\n' followed by the attribute tag, a colon,
  and then the character (in ASCII format).

DEPENDENCIES
  uim_efslog_buffer_init must have already been called

RETURN VALUE
  void

SIDE EFFECTS
  Alters the uim_efslog_buffer

===========================================================================*/
void uim_efslog_apdu_to_buffer
(
  byte attribute,
  byte character
);

#endif /* FEATURE_UIM_LOG_APDU_TO_EFS */

/*===========================================================================

FUNCTION uim_clk_busy_wait

DESCRIPTION
  This function is a wrapper around clk_busy_wait to avoid data conversion
  issues for different targets and to validate the supplied values.

DEPENDENCIES
  None

RETURN VALUE
  None

SIDE EFFECTS
  None

===========================================================================*/
void uim_clk_busy_wait(dword busy_wait_time);

/*===========================================================================

FUNCTION UIM_PROCESS_GENERIC_RESPONSE

DESCRIPTION
  This procedure processes the response that has been recd from the UIM
  at the completion of the command processing and releases the command
  buffer for a generic command.

DEPENDENCIES
  None

RETURN VALUE
  None

SIDE EFFECTS
  Releases the command buffer.

===========================================================================*/
extern void uim_process_generic_response
(
  uim_rsp_buf_type const *rsp_ptr,
  uim_cmd_type *cmd_ptr
);

/*===========================================================================

FUNCTION UIM_SET_POLL_TIMER

DESCRIPTION
  This function should be used to set the time to trigger a poll.

DEPENDENCIES
  None.

RETURN VALUE
  void

SIDE EFFECTS
  Sets the uim_poll_time_delay to the value passed in.
  Sets a second timer based on feature definition to ensure
  wake-up.

===========================================================================*/
void uim_set_poll_timer(
  rex_timer_cnt_type  time
);

#if defined (FEATURE_UIM_USIM)
#error code not present
#endif /*defined (FEATURE_UIM_USIM)*/


/*===========================================================================

FUNCTION UIM_PET_AND_WAIT

DESCRIPTION
  This procedure accepts rex_sigs_type mask and keeps waiting for any of
  the signals in the mask specified.  Also, if it is time to report to
  the dog, the dog would be pet.

DEPENDENCIES
  None

RETURN VALUE
  rex_sigs_type

SIDE EFFECTS
  None

===========================================================================*/
rex_sigs_type uim_pet_and_wait
(
  rex_sigs_type sigs_to_wait_for
);

#endif /* UIMI_H */

