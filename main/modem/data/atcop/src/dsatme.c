/*====*====*====*====*====*====*====*====*====*====*====*====*====*====*====*

                D A T A   S E R V I C E S

                A T   C O M M A N D   
                
                ( M O B I L E  E Q U I P M E N T  C O M M A N D S )
                
                P R O C E S S I N G

GENERAL DESCRIPTION
  This module executes the AT commands. It mainly executes the common, 
  non-ETSI Mobile Equipment commands.

EXTERNALIZED FUNCTIONS

EXTERNALIZED FUNCTIONS INTERNAL TO DSAT UNIT
  dsat_init_me
    ME initialization function:
  - registers the client call back function for GSDI events.
  - registers as a client of MMGSDI.

  dsatme_gsdi_event_handler
    This is the handler function for CDMA specific GSDI events, ETSI events are
    handled in dsatetsime_gsdi_event_handler.

 dsatme_set_cme_error
    Write Mobile Equipment error to res_buff_ptr.  The Mobile Equipment
    error code and error text are defined in Section 9.2 of 3GPP TS 27.007

  dsatme_exec_cpbs_cmd
    This function takes the result from the command line parser
    and executes it. It executes +CPBS/$QCCPBS command.
    The result is in res_buff_ptr.

  dsatme_exec_cpbr_cmd
    This function takes the result from the command line parser
    and executes it. It executes +CPBR/$QCCPBR command.
    +CPBR command is used to read phonebook entries.

  dsatme_exec_cpbf_cmd
    This function takes the result from the command line parser
    and executes it. It executes +CPBF/$QCCPBF command.
    +CPBF command is used to search phonebook entries.

  dsatme_exec_cpbw_cmd
    This function takes the result from the command line parser
    and executes it. It executes +CPBW/$QCCPBW command.
    +CPBW command is used to write phonebook entry.

  dsatme_cmd_status_handler
    This function is the async event handler for async functions such as
    pbm_file_info, pbm_read, pbm_write, and pbm_find_name.

  dsatme_direct_dial
    Handle atd direct dialing from phonebooks command.  

  dsatme_ld_init_record
    Prepare the ld_rec for saving dialing number into last dial ph book.

  dsatme_ld_connected
    Change ld_rec state to be connected.

  dsatme_ld_save
    Store ld_rec to the last dial ph book.

  dsatme_pbm_cb_cmd_handler
    Processes DS_AT_PBM_CB_CMD cmd infos.

  dsatme_reset_pb
    Reset ph book to default values.

INITIALIZATION AND SEQUENCING REQUIREMENTS

   Copyright (c) 2004-2010 by QUALCOMM Incorporated.
   All Rights Reserved.
   Qualcomm Confidential and Proprietary.
*====*====*====*====*====*====*====*====*====*====*====*====*====*====*====*/


/*===========================================================================

                        EDIT HISTORY FOR MODULE

  This section contains comments describing changes made to the module.
  Notice that changes are listed in reverse chronological order.

  $Header: //source/qcom/qct/modem/data/common/commonatcop/main/lite/src/dsatme.c#11 $

when       who     what, where, why
--------   ---     ----------------------------------------------------------
05/03/10   ad      Fixed qccpbr command . 
03/05/09   bs      Added support for mmgsdi interface.
02/03/09   bs      Fixed compilation errors/warnings.
01/02/09   bs      Added support for 1X AT Phonebook commands.
09/07/08   cs      Fixed featurization issue.
08/12/08   bs      Fixed featurization issue.
07/16/08   bs      Added support for PS SIM INIT notification.
07/09/08   bs      Added support for CDMA NAARefresh.
03/28/08   bs      Fixed Lint High's.
09/21/07 sq/mga/yz Merged entire file and made changes to check 
                   uim_return_cdma_svc_availabililty for UIM_CDMA_SVC_3GPD_EXT 
                   to support FEATURE_UIM_SUPPORT_3GPD. Added 
                   nvruim_data_3pgd_init_extensions_support. Made changes to  
		   post ps_gsdi_event_handler_cmd in dsatme_gsdi_event_handler.
08/29/05   snb     Register as MMGSDI client on initialization.
11/23/04   jk      Changes to Support 3GPD on R-UIM
09/13/04   snb     Fix 6500 build warning.
07/06/04   snb     Register for personalization PIN events.
05/18/04   jk      Added initialization code for 3GPD support on R-UIM card.
01/06/04   snb     Move registration for pin event with GSDI and handling 
                   callback for pin events to common file and add protocol
                   stack re-initialization under feature define.

===========================================================================*/


/*===========================================================================

                     INCLUDE FILES FOR MODULE

===========================================================================*/

#include "comdef.h"
#include "customer.h"
#include "AEEstd.h"

#ifdef FEATURE_DATA
#include "dsatme.h"
#include "dsatctab.h"
#include "err.h"
#include "assert.h"

#ifdef FEATURE_MMGSDI
#include "gsdi_exp.h"
#include "ps_svc.h"
#include "mmgsdi.h"

#ifdef FEATURE_DATA_ETSI_PIN
#error code not present
#endif /* FEATURE_DATA_ETSI_PIN */

#ifdef FEATURE_RUIM_CDMA_REFRESH
#include "dsat707mip.h"
#endif /* FEATURE_RUIM_CDMA_REFRESH */

#if defined(FEATURE_DATA_IS707)
  #include "dsat707util.h"
#endif /* FEATURE_DATA_IS707 */

#ifdef FEATURE_UIM_SUPPORT_3GPD
#include "nvruimi.h"
#include "dstaski.h"
#include "ps_svc.h"
#endif /* FEATURE_UIM_SUPPORT_3GPD */

#endif /* FEATURE_MMGSDI */

#if defined(FEATURE_APP_RECENTCALLS) && defined(FEATURE_ETSI_PBM)
#include "ds_cap.h"
#endif /* defined(FEATURE_APP_RECENTCALLS) && defined(FEATURE_ETSI_PBM) */

#if defined(FEATURE_ETSI_PBM) || defined(FEATURE_DSAT_CDMA_PBM)
#include "err.h"
#include "clk.h"
#endif /* defined(FEATURE_ETSI_PBM) || defined(FEATURE_DSAT_CDMA_PBM) */

/*-------------------------------------------------------------------------
  import definitions:
-------------------------------------------------------------------------*/
/* ATD Dial Table */
extern dsat_dial_type dsati_dial_table[DSAT_MODE_MAX][DSAT_MAX_DIAL_TYPE];

/*--------------------------------------------------------------------------
  Debug macro for this module.
---------------------------------------------------------------------------*/
#define GSDI_STATUS_MASK  0xFF

#ifdef DEBUG_DSATCOP
#define DEBUG( x )  { x }
#else
#define DEBUG( x )
#endif /* DEBUG_DSATCOP */


#ifdef FEATURE_MMGSDI

/*--------------------------------------------------------------------------
  In order to init 3GPD properly, we must refresh NV on GSDI_SIM_INIT_COMPLETED
  Event, which is recieved only if FEATURE_MMGSDI is defined 
---------------------------------------------------------------------------*/

#if defined(FEATURE_UIM_SUPPORT_3GPD) && !defined(FEATURE_MMGSDI)
#error FEATURE_UIM_SUPPORT_3GPD requires FEATURE_MMGSDI for GSDI event
#endif /* FEATURE_UIM_SUPPORT_3GPD && !FEATURE_MMGSDI */


/*===========================================================================

                    REGIONAL DEFINITIONS AND DECLARATIONS

===========================================================================*/

/*---------------------------------------------------------------------------
  ATCOP's MMGSDI client ID: 
  Only needed for SIM card power control functionality.
---------------------------------------------------------------------------*/
mmgsdi_client_id_type dsatme_mmgsdi_client_id = 
                              (mmgsdi_client_id_type)MMGSDI_INVALID_SESSION_ID;

/*===========================================================================

            LOCAL DEFINITIONS AND DECLARATIONS FOR MODULE

  This section contains local definitions for constants, macros, types,
  variables and other items needed by this module.

===========================================================================*/

/*-------------------------------------------------------------------------
    Local definitions:
-------------------------------------------------------------------------*/
#ifdef FEATURE_UIM_SUPPORT_3GPD
#define UIM_SUPPORT_SIMPLE_IP_MASK 0x80
#define UIM_SUPPORT_MOBILE_IP_MASK 0x40
#define UIM_SUPPORT_FALLBACK_MASK 0x20
#endif /* FEATURE_UIM_SUPPORT_3GPD */
#endif /* FEATURE_MMGSDI */
/*-------------------------------------------------------------------------
  Local variables
-------------------------------------------------------------------------*/
#ifdef FEATURE_UIM_SUPPORT_3GPD
/* This struct contains UIM related Global Variables for this file */
LOCAL struct   
{
  uim_cmd_type      uim_cmd;
  byte              operational_capabilities;
  uim_rpt_status    report_status;
  byte              cdma_svc_table_buffer[UIM_CDMA_SVC_TABLE_SIZE];
} dsatme_uim;
#endif /* FEATURE_UIM_SUPPORT_3GPD */

cb_cmd_e_type cb_expected = CB_NONE;
boolean dsat_mask_pin2_state = TRUE;
extern dsat_timer_s_type dsatutil_timer_table[];

#if defined(FEATURE_ETSI_PBM) || defined(FEATURE_DSAT_CDMA_PBM)
#if defined(FEATURE_WCDMA) || defined(FEATURE_GSM)
#error code not present
#endif /* defined(FEATURE_WCDMA) || defined(FEATURE_GSM) */

/* Type of address according to 3GPP TS27.007 Section 6.1 */
#define TA_INTERNATIONAL 145
#define TA_OTHER         129

/* The min and max ph book entry. 
   They are used to check if the current storage's min and max
   index are reasonable */
#define PB_INDEX_MIN       1
#define PB_INDEX_MAX     500
#define PIN_MIN_LEN        4

/* max allowed number length */
#define CPB_MAX_NUM_LENGTH PBM_MAX_NUM_LENGTH

#define DSAT_CPBW_MAX_FIELDS 3
#define CPB_FIELD_TEXT 0
#define CPB_FIELD_NUMBER 1
#define CPB_FIELD_NUMBER_TYPE 2
#define PB_MAX_REC_LEN 500

/* The table to match pbm_device_type to at storage type 
   of 3GPP TS27.007 Section 8.11 */
typedef struct {
  /* pbm enum corresponding to the storage type */
  pbm_device_type device;

 /* storage type of 3GPP TS27.007 Section 8.11 */
  byte storage[CPBS_VAL_LEN + 1];
} pbm_table_type;

typedef dsat_result_enum_type (*pbm_cb_handler_type) 
  (dsat_mode_enum_type, ds_cmd_type *);

typedef struct 
{
  const cb_cmd_e_type           cmd_id;
  const pbm_cb_handler_type     handler;
} pbm_cb_handler_table_type;

/* state of the dialed number to be saved to the last dial ph book */
typedef enum {
  LD_NONE,         /* no dialing                */
  LD_DIALED,       /* number has been dialed    */
  LD_CONNECTED     /* connected                 */
} ld_state_type;

/* type of dialed number to be saved to the last dial ph book */
typedef struct {
  ld_state_type      state;      /* dialed number state             */
  uint32             conn_time;  /* time when the call is connected */
  dsat_pbm_record_s_type  rec;   /* contains the dialed record      */
} ld_rec_type;

#define MAX_PBM_AT_NAME_LEN  8

/* variable for pbm readiness */
boolean dsatme_pbm_ready = FALSE;
boolean dsatme_nv_only = FALSE;

/* me_cpbs_params hold current +CPBS argument across 
   functions */
typedef struct
{
  pbm_device_type  pbm_device;
  byte password[PIN_MAX_LEN + 1];

  /* is_read field identifies whether the current +CPBS cmd 
     is this a read cmd.
     The responses for cpbs set (for example, AT+CPBS="SM")
     and read (for example, AT+CPBS?) are different */
  boolean is_read;  

  char display_name[MAX_PBM_AT_NAME_LEN];
} cpbs_argu_type;


/* file scope variable starts with me */
LOCAL cpbs_argu_type me_cpbs_params;

/* hold infos of current ph book storage 
   Initialize to be the default (SM) in the dsat_cpbs_list*/
me_pb_entry_type me_cpbs_entry =
{
  PBM_DS_SM,
  DEFAULT_CPB_STORAGE,
  -1,
  -1
};

/* save the infos of the default ph book storage,
   we will restore me_cpbs_entry to the default storage after
   ATZ or AT&F
   DEFAULT_CPB_STORAGE is ME, we use macro here to synchronize it with
   dsat_cpbs_storage_valstr */
LOCAL const me_pb_entry_type me_cpbs_default =
{
  PBM_DS_SM,
  DEFAULT_CPB_STORAGE,
  -1,
  -1
};

LOCAL const me_pb_entry_type me_nvonly_cpbs_default =
{
  PBM_EFS,
  DEFAULT_NV_CPB_STORAGE,
  -1,
  -1
};

/* used to remember the last index from pbm_read command in
   order to send the final read results to the response generator */
typedef struct {
  /* The current +CPBR index under processing.
     There is a possibility of re-entering read command handler, if we
     hit high AT->SIO TX watermark count. In such case, we resume the
     reading with next record from current (i.e., current++). */   
  uint16 current;

  /* Maintain last index. Read continues until current index reaches 
     last index. */
  uint16 last;
  char display_name[MAX_PBM_AT_NAME_LEN];
} cpbr_index_type;

LOCAL cpbr_index_type cpbr_index;

/* local variables for direct dialing functions */
typedef struct {
  /* contains the abbtributes of direct dial string             */
  dsati_dial_str_type   dial_str;

  /* contains the original strings after '>'
     It is used as a match str to search for a number in
     ph book after pbm cb unqualifies it as valid <mem><num>.
     It is used to hold the number we fetch from ph book.
     dial_str.num is a pointer pointing to the cmd token, which
     contains the dial number.
     The token recycles while we process direct dialing.  Thus 
     we have to save it here                                    */
  byte                  str[MAX_LINE_SIZE];

  /* contains <mem> of D><mem><n>.  This is for 27007 v370 
     Section 6.3 case 2.
     It contains '\0' if the string after > is not in the 
     pattern of <mem><n>                                        */
  byte                  mem[CPBS_VAL_LEN + 1];

  /* contains index, the <n> of D>mem<n> or D><n>.
     It is used to read the number at location n of <mem> 
     after pbm_file_info's cb function verify the index is 
     in valid range                                             
     It is -1 at init time or the command is D><str>            */
  int                   index;

  /* the best pbm number entry found so far.
     If we found a ph number with name starts with the 
     searching string, we save it here as a candidate.  We
     searching for a exact match.  If we can't found a exact
     match in all the supported ph book, we use this number 
     to dial.                                                   
     If record.number[0] is '\0', this field is not set         */
  dsat_pbm_record_s_type     record;

  /* remember which ph book we are searching, start 
     with current pbs ph book.
     Init to be -1*/
  int8                  srch_pb;

} etsime_dd_type;

/* contains data for direct dialing processing */
LOCAL etsime_dd_type    dd_val;

/* dialed number to be saved to the last dial ph book */
LOCAL ld_rec_type ld_rec;

#define CPBS_SUPPORTED_PHBKS_NUM           9   /* Total number of phone books 
      supported by CPBS as mentioned in the array dsat_cpbs_storage_valstr */
#define SPBS_SUPPORTED_PHBKS_NUM          13   /* Total number of phone books 
      supported by CPBS as mentioned in the array dsat_spbs_storage_valstr */
#define OPTIONAL_PHBKS_NUM                 7   /* Total number of phone books 
      books that can be optional on SIM/USIM */
const dsat_string_item_type opt_phbk_storage_valstr [OPTIONAL_PHBKS_NUM][8] =
{"SM","FD","LD","ON","SD","OW","ME"}; /* optional phone books list on A SIM/USIM */



/*-------------------------------------------------------------------------
    Prototypes for local functions:
-------------------------------------------------------------------------*/

LOCAL dsat_result_enum_type cpbs_process_pin2_status(ds_at_cmd_status_type);

LOCAL dsat_result_enum_type cpbs_process_verify_pin_status(ds_at_cmd_status_type);


/*--------              pbm cmd handlers                         --------*/

LOCAL void cpb_pbm_read_record
(
  uint16                     record_id,
  dsat_pbm_record_s_type     *record_ptr,
  pbm_return_type            *pbm_return_ptr
);

LOCAL dsat_result_enum_type cpb_pbm_read_handler
(
  void
);

LOCAL dsat_result_enum_type cpb_pbm_reenter_read_handler
(
  dsat_mode_enum_type mode,
  ds_cmd_type* cmd_ptr
);

LOCAL dsat_result_enum_type cpb_pbm_read_handler_normal_status
(
  dsat_pbm_record_s_type     * record_ptr,
  dsat_rsp_enum_type         * rsp_ptr
);

LOCAL dsat_result_enum_type cpb_pbm_read_handler_error_status
(
  pbm_return_type  pbm_return
);

LOCAL dsat_result_enum_type cpb_pbm_find_handler
(
  void
);

LOCAL dsat_result_enum_type cpb_pbm_reenter_find_handler
(
  dsat_mode_enum_type mode,
  ds_cmd_type       * cmd_ptr
);

LOCAL dsat_result_enum_type cpb_pbm_find_handler_status_success
(
  dsat_pbm_record_s_type     * record_ptr,
  uint16                       loc_id,
  dsat_rsp_enum_type         * rsp_ptr
);

LOCAL dsat_result_enum_type cpb_pbm_find_handler_status_eof
(
  dsat_rsp_enum_type async_rsp
);

LOCAL dsat_result_enum_type cpb_pbm_find_handler_status_fail
(
  pbm_return_type  pbm_return
);

/*--------    pbm cb cmd handlers & other misc utilities        -------- */

LOCAL dsat_result_enum_type cpb_pbm_cb_ready_handler
(
  dsat_mode_enum_type   mode,
  ds_cmd_type *         cmd_ptr
);

LOCAL dsat_result_enum_type cpb_pbm_cb_ready_file_info_handler
(
  dsat_mode_enum_type   mode,
  ds_cmd_type *         cmd_ptr
);

LOCAL dsat_result_enum_type cpb_pbm_cb_file_info_handler
(
  dsat_mode_enum_type   mode,
  ds_cmd_type *         cmd_ptr
);

LOCAL void cpb_pbm_cb_file_info_handler_output_info
(
  int  used
);

LOCAL dsat_result_enum_type cpb_pbm_cb_file_info_handler_error_status
(
  pbm_return_type   pbm_return
);

LOCAL dsat_result_enum_type cpb_pbm_cb_write_handler
(
  dsat_mode_enum_type   mode,
  ds_cmd_type *         cmd_ptr
);

LOCAL dsat_result_enum_type dd_pbm_file_info_handler
(
  dsat_mode_enum_type   mode,
  ds_cmd_type *         cmd_ptr
);

LOCAL dsat_result_enum_type dd_pbm_find_handler
(
  const byte *          str, 
  pbm_device_type       pbm_device
);

/*--------                pbm call back functions                --------*/

/* pbm ready event */
LOCAL void cpb_pbm_status_cb(boolean);

LOCAL void me_pbm_file_info_cb
(
  pbm_return_type                status,
  pbm_device_type                pbm_device,
  pbm_extended_fileinfo_s_type * pbm_extended_fileinfo
);

LOCAL void me_pbm_write_cb
(
  pbm_writecb_data_s_type * cb_data  
);

LOCAL void me_ld_pbm_write_cb
(
  pbm_writecb_data_s_type * cb_data  
);

/*-------- pbm function call and cb cmd status  processing function --------*/

LOCAL dsat_result_enum_type process_pbm_record_write_return
(
  pbm_return_type   pbm_result,
  dsm_item_type *   err_buff_ptr
);

/*--------      Other ph book local function           --------*/
/* utility functions */

LOCAL boolean get_pbm_device
(
  const byte            * pb,
  pbm_device_type       * dev_ptr
);

LOCAL boolean get_pb_storage
(
  pbm_device_type       device,
  byte                  * storage_ptr
);

/* for $qccpbs command */

LOCAL dsat_result_enum_type cpbs_parse_arguments
(
  const tokens_struct_type * token_ptr,
  cpbs_argu_type           * cpbs_arg_ptr
);

LOCAL dsat_result_enum_type cpbs_process_test_cmd
(
  const dsati_cmd_type *parse_table,    
  dsm_item_type *res_buff_ptr           
);

/* for $qccpbr command */
LOCAL dsat_result_enum_type cpbr_parse_arguments
(
  const tokens_struct_type * token_ptr, 
  uint16                   * index1_ptr,
  uint16                   * index2_ptr,  
  dsm_item_type            * res_buff_ptr
);

/* for $qccpbw command */

LOCAL dsat_result_enum_type cpbw_parse_arguments
(
  const tokens_struct_type * token_ptr,
  dsat_pbm_record_s_type   * record_ptr,
  dsm_item_type            * res_buff_ptr
);

LOCAL boolean cpbw_check_num_string
(
  const byte * in_str,
  byte * out_str
);

LOCAL dsat_result_enum_type me_dd_process_n
(
  const byte    * mem, 
  int           index,
  dsm_item_type * err_buff_ptr
);

LOCAL dsat_result_enum_type me_dd_process_mem_n
(
  const byte * mem, 
  int index,
  dsm_item_type * err_buff_ptr
);

LOCAL dsat_result_enum_type me_dd_process_str
(
  const byte * str, 
  dsm_item_type * err_buff_ptr
);

#if defined(FEATURE_WCDMA) || defined(FEATURE_GSM)
#error code not present
#endif

/* pb book command helper functions */
LOCAL void cpb_process_pbm_error
(
  pbm_return_type   pbm_return, 
  dsm_item_type     *err_buf_ptr
);

LOCAL void me_pbm_cb_submit_ds_cmd
(
  cb_cmd_e_type              cmd,
  pbm_return_type            status,
  ds_at_pbm_file_info_type * info_ptr,
  pbm_record_s_type        * record_ptr
);
#endif /* defined(FEATURE_ETSI_PBM) || defined(FEATURE_DSAT_CDMA_PBM) */

#ifdef FEATURE_MMGSDI

LOCAL void me_register_cb_func ( void );

LOCAL void gsdi_pin_event_cb_func 
(
  gsdi_sim_events_T  event
);

LOCAL void me_mmgsdi_rsp_cb
(
  mmgsdi_return_enum_type mmgsdi_status,
  mmgsdi_cnf_enum_type    cnf_type,
  const mmgsdi_cnf_type * cnf_data_ptr
);

LOCAL void me_mmgsdi_client_evt_reg_cb
(
    const mmgsdi_event_data_type   *mmgsdi_event
);

LOCAL void me_mmgsdi_client_id_reg_cb
( 
  mmgsdi_return_enum_type status,
  mmgsdi_cnf_enum_type    cnf,
  const mmgsdi_cnf_type        *cnf_ptr
);

#ifdef FEATURE_UIM_SUPPORT_3GPD

LOCAL void uim_3gpd_op_cap_report
(
  uim_rpt_type *  report
);

LOCAL void init_uim_operational_capability
(
  void
);

LOCAL void dsatme_uim_check_cdma_svc_table
(
  boolean *support_flag_sip,
  boolean *support_flag_mip,
  boolean *service_3gpd_extensions
);

LOCAL void dsat_uim_3gpd_cdma_svc_init_report
(
  uim_rpt_type *report           /* R-UIM command status report */
);


#endif /* FEATURE_UIM_SUPPORT_3GPD */
#endif /* FEATURE_MMGSDI */

/*-------------------------------------------------------------------------
            Constants:
-------------------------------------------------------------------------*/
#if defined(FEATURE_ETSI_PBM) || defined(FEATURE_DSAT_CDMA_PBM)
#define LAST_DIALED_STR "LD"

/* The table to match pbm_device_type to AT storage type */
LOCAL const pbm_table_type
pbm_table[] = {
  { PBM_DS_SM,              "SM" },
  { PBM_DIALED,             "DC" },
  { PBM_FDN,                "FD" },
  { PBM_LND,                LAST_DIALED_STR },
  { PBM_MISS,               "MC" },
  { PBM_EFS,                "ME" },
  { PBM_RCVD,               "RC" },
  { PBM_ECC,                "EN" },
  { PBM_MSISDN,             "ON" }
};

LOCAL const int pbm_table_size = ARR_SIZE(pbm_table);

LOCAL const status_handler_table_entry_type pbm_status_handler_table[] =
{
#ifdef  FEATURE_MMGSDI   
  {CPBS_MMGSDI_PIN2_STATUS,   cpbs_process_pin2_status},
  {CPBS_MMGSDI_PIN2_VERIFY,   cpbs_process_verify_pin_status},
#endif  /* FEATURE_ETSI_PBM */
  {CB_NONE,                NULL}
};

LOCAL const pbm_cb_handler_table_type pbm_cb_handler_table[] =
{
  { CPB_PBM_CB_READY,           cpb_pbm_cb_ready_handler                },
  { CPB_PBM_CB_READY_FILE_INFO, cpb_pbm_cb_ready_file_info_handler      },
  { CPB_PBM_CB_FILE_INFO,       cpb_pbm_cb_file_info_handler            },
  { CPB_PBM_CB_WRITE,           cpb_pbm_cb_write_handler                },
  { DD_PBM_FILE_INFO,           dd_pbm_file_info_handler                },
  {CB_NONE,                     NULL}
};
#endif /* defined(FEATURE_ETSI_PBM) || defined(FEATURE_DSAT_CDMA_PBM) */
/*-------------------------------------------------------------------------
            Function Definitions: Public functions
-------------------------------------------------------------------------*/

/*--------          Functions exported outside of ATCOP          --------*/


/*--------               INITIALIZATION FUNCTIONS                --------*/
#ifdef FEATURE_MMGSDI
/*===========================================================================

FUNCTION  DSATME_INIT_ME

DESCRIPTION
  ME initialization function. This function does the following:
  - Registers the client call back function for GSDI events,
  - Register as a client of MMGSDI.
  
DEPENDENCIES
  None

RETURN VALUE
  None

SIDE EFFECTS
  None

===========================================================================*/
void dsatme_init_me ( void )
{
  /* Call the registration function to register call back functions */
  me_register_cb_func ( );

  /* Register for MMGSDI client ID:
     Only needed for SIM card power control functionality, on failure
     dsatme_mmgsdi_client_id will remain set invalid */
  if ( MMGSDI_SUCCESS != mmgsdi_client_id_reg(me_mmgsdi_client_id_reg_cb, 0) )
  {
    MSG_ERROR("MMGSDI client registration failed", 0, 0, 0);
  }

  return;
}/* dsatme_init_me */


/*--------     AT mobile equipment cmd handler functions         --------*/

/*===========================================================================

FUNCTION  DSATME_GSDI_EVENT_HANDLER

DESCRIPTION
  This is the handler function for CDMA specific GSDI events, ETSI events are
  handled in dsatetsime_gsdi_event_handler.

DEPENDENCIES
  None

RETURN VALUE
  returns an enum that describes the result of the command execution.
  possible values:
    DSAT_ASYNC_EVENT : async event, contine processing  

SIDE EFFECTS
  None

===========================================================================*/
/* ARGSUSED */
dsat_result_enum_type dsatme_gsdi_event_handler
(
  dsat_mode_enum_type   mode,                /* AT command mode            */
  ds_cmd_type         * cmd_ptr              /* DS Command pointer         */
)
{
  dsat_result_enum_type  result = DSAT_ASYNC_EVENT;

#ifdef FEATURE_UIM_SUPPORT_3GPD
  boolean support_3gpd_sip=FALSE;
  boolean support_3gpd_mip=FALSE;
  boolean service_3gpd_extensions=FALSE;
#endif /* FEATURE_UIM_SUPPORT_3GPD */

  ASSERT(NULL != cmd_ptr);

#ifdef FEATURE_DATA_ETSI_PIN
#error code not present
#endif /* FEATURE_DATA_ETSI_PIN */

  switch (cmd_ptr->cmd.gsdi_event.event)
  {
#ifdef FEATURE_UIM_SUPPORT_3GPD
  /*---------------------------------------------------------------------------
    In order for 3GPD to initialize properly, Data Services must recieve the 
    following event (even in 1x Only Builds).  We should get this event as long
    as FEATURE_MMGSDI is defined - Therefore, in order for 3GPD to work, MMGSDI
    must be defined
  ----------------------------------------------------------------------------*/
    case GSDI_SIM_INIT_COMPLETED:

      dsatme_uim_check_cdma_svc_table(&support_3gpd_sip, &support_3gpd_mip,
                                      &service_3gpd_extensions);
      uim_init_3gpd_preference(support_3gpd_sip, support_3gpd_mip);
      nvruim_data_3gpd_init_extensions_support(service_3gpd_extensions);

      if(UIM_3GPD_MIP_NV_SIP_NV != uim_3gpd_support())
      {
        dsatcmdp_init_config();
        init_uim_operational_capability();
      }
#ifndef FEATURE_MMGSDI
      ps_send_cmd (PS_GSDI_EVENT_HANDLER_CMD, (void*) &(cmd_ptr->cmd.gsdi_event.event));
#endif
      break;
#endif /* FEATURE_UIM_SUPPORT_3GPD */
    
    default:
      MSG_LOW("Unsupported GSDI event: %d",
              cmd_ptr->cmd.gsdi_event.event,0,0);
      break;
  }

  return result;
} /* dsatme_gsdi_event_handler */


/*===========================================================================

FUNCTION DSATME_MMGSDI_REQ_CB

DESCRIPTION
  This function is the callback function for the MMGSDI to return pin status.

DEPENDENCIES
  None

RETURN VALUE
  None

SIDE EFFECTS
  None

===========================================================================*/
void dsatme_mmgsdi_req_cb
(
  mmgsdi_return_enum_type status, /* MMGSDI return status */
  mmgsdi_cnf_enum_type    cnf,    /* Type of command confirmation */
  const mmgsdi_cnf_type *cnf_ptr  /* Pointer to confirmation data */
)
{
  byte data = 0;
  uint16  slot_status = 0;
  cb_cmd_e_type client_data;
  if(cnf_ptr == NULL)
  {
    MSG_ERROR("Cnf ptr is NULL", 0, 0, 0);
    return;
  }
  client_data = (cb_cmd_e_type)cnf_ptr->response_header.client_data;
  
  DEBUG ( ASSERT(cb_expected == (cb_cmd_e_type)cnf_ptr->response_header.client_data) ;);

  switch (cnf_ptr->response_header.response_type)
  {
#if defined(FEATURE_ETSI_PBM) || defined(FEATURE_DSAT_CDMA_PBM)
    case MMGSDI_GET_ALL_PIN_STATUS_CNF:
       switch (cnf_ptr->response_header.client_data) 
       {
          case CPBS_MMGSDI_PIN2_STATUS:
             data = (byte)cnf_ptr->get_all_pin_status_cnf.pin2.status;
             break;
          default:
             break;
       }
#endif /* defined(FEATURE_ETSI_PBM) || defined(FEATURE_DSAT_CDMA_PBM) */
       
    case MMGSDI_PIN_OPERATION_CNF:
      data = (byte) cnf_ptr->pin_operation_cnf.pin_info.num_retries;
      break;
    default:
      ERR("Not a valid mmgsdi cb rsp: %d", 
           cnf_ptr->response_header.response_type, 0, 0);
      break;
  }

#ifdef FEATURE_DUAL_SLOTS
  /* Encode GSDI slot into upper half of status */
  slot_status  = (cnf_ptr->response_header.slot_id<< 8);
  slot_status |= (cnf_ptr->response_header.mmgsdi_status & GSDI_STATUS_MASK);
#else
  slot_status  = (uint16)cnf_ptr->response_header.mmgsdi_status;
#endif /* FEATURE_DUAL_SLOTS */

  submit_cmd_status( client_data, 
                     slot_status,
                     data );

  MSG_HIGH ("MMGSDI callback with status: %d", status, 0, 0);
  cb_expected = CB_NONE;
} /* dsatme_mmgsdi_req_cb */
#endif /* FEATURE_MMGSDI */

/*===========================================================================

FUNCTION  DSAT_SEND_CME_ERROR 

DESCRIPTION
  Sends Mobile Equipment error to TE or PS protocol
  stack based on response routing.  The Mobile Equipment error 
  code and error text are defined in Section 9.2 of 3GPP TS 27.007

  If AT+CMEE is 2, +CME ERROR : <err> result code is enabled
                   to used verbose text err value

DEPENDENCIES
  None
  
RETURN VALUE
  DSAT_CMD_ERR_RSP: for convenience in setting return value this is always 
                    returned.
  
SIDE EFFECTS
  None
===========================================================================*/
dsat_result_enum_type dsat_send_cme_error
(
  dsat_cme_error_e_type error_code
)
{
  /* cmee enables the ME error result code */
  dsm_item_type *raw_rsp_ptr;

  raw_rsp_ptr = dsm_new_buffer( DSM_DS_LARGE_ITEM_POOL );
  if ( raw_rsp_ptr == NULL )
  {
    ERR( "Running out of buffers",0,0,0 );
  }
  else
  {
    dsatme_set_cme_error( error_code, raw_rsp_ptr );

    if( raw_rsp_ptr->used == 0 )
    {
      (void) dsm_free_packet( &raw_rsp_ptr );
    }
    else
    {
      /* Indicate abort command processing has completed if not disabled.
         This function performs no action if abort command processing is
         not currently in progress. */
      if ( dsatcmdp_end_abort_in_send_rsp )
      {
        dsatcmdp_command_abort_complete( );
      }
      /* Format response, send the response to the user as information text, */
      /* clear the regional block indications flag, & free raw_rsp_ptr */
      dsatcmdp_handle_async_cmd_rsp(raw_rsp_ptr, DSAT_COMPLETE_RSP); 

      raw_rsp_ptr = NULL;
    

    }
  }

  return DSAT_CMD_ERR_RSP;
} /* dsat_send_cme_result( ) */

/*===========================================================================

FUNCTION SUBMIT_CMD_STATUS

DESCRIPTION
  This function send DSTASK a status cmd which will be processed by ATCOP.
  
DEPENDENCIES
  None

RETURN VALUE
  None

SIDE EFFECTS
  None
===========================================================================*/
/* ARGSUSED */
LOCAL void submit_cmd_status 
(
  cb_cmd_e_type cmd_id,         /* who submitted this status */
  uint16        status,         /* cmd status */
  byte          data            /* cmd data */
)
{
  ds_cmd_type *cmd_ptr;

  /* tell atcop that the async return is done */
  if( (cmd_ptr = ds_get_cmd_buf()) == NULL)
  {
    ERR_FATAL("No ds command buffer",0,0,0);
  }
  else
  {
    cmd_ptr->hdr.cmd_id = DS_AT_STATUS_CMD;
    cmd_ptr->cmd.status_cmd.cmd_id = (int8) cmd_id;
    cmd_ptr->cmd.status_cmd.cmd_status = (int16)status;
    cmd_ptr->cmd.status_cmd.cmd_data = data;
    ds_put_cmd(cmd_ptr);
  }
} /* submit_cmd_status */

/*===========================================================================

FUNCTION DSATME_CMD_STATUS_HANDLER

DESCRIPTION
  This function is the async event handler for async functions such as
  pbm_file_info, pbm_read, pbm_write, pbm_find_name and gsdi pin requests.

DEPENDENCIES
  None

RETURN VALUE
  None

SIDE EFFECTS
  None
===========================================================================*/
/* ARGSUSED */
dsat_result_enum_type dsatme_cmd_status_handler
(
  dsat_mode_enum_type mode,
  ds_cmd_type * cmd_ptr
)
{
#if defined(FEATURE_ETSI_PBM) || defined(FEATURE_DSAT_CDMA_PBM)
  int index;
#endif /* defined(FEATURE_ETSI_PBM) || defined(FEATURE_DSAT_CDMA_PBM) */
  dsat_result_enum_type result;
  
  ASSERT(NULL != cmd_ptr);

  /* Default action is to return the incoming cmd status */
  result = (dsat_result_enum_type)cmd_ptr->cmd.status_cmd.cmd_status;

/* this should be included only in GSM or WCDMA mode */
#if defined(FEATURE_WCDMA) || defined(FEATURE_GSM)
#error code not present
#endif /* defined(FEATURE_WCDMA) || defined(FEATURE_GSM) */
#if defined(FEATURE_ETSI_PBM) || defined(FEATURE_DSAT_CDMA_PBM)
  /* Check for phonebook cmd status handler */
  for(index = 0; pbm_status_handler_table[index].cmd_id != CB_NONE; index++)
  {
    if( pbm_status_handler_table[index].cmd_id ==
        (cb_cmd_e_type)cmd_ptr->cmd.status_cmd.cmd_id )
    {
      result = 
        pbm_status_handler_table[index].status_handler (cmd_ptr->cmd.status_cmd);
    }
  }
#endif /* defined(FEATURE_ETSI_PBM) || defined(FEATURE_DSAT_CDMA_PBM) */

#ifdef FEATURE_DUAL_SLOTS
  return (result & GSDI_STATUS_MASK);
#else
  return result;
#endif /* FEATURE_DUAL_SLOTS */
} /* dsatme_cmd_status_handler */

#if defined(FEATURE_ETSI_PBM) || defined(FEATURE_DSAT_CDMA_PBM)
/*===========================================================================

FUNCTION  DSATME_INIT_PBM

DESCRIPTION
  PBM initialization function. This function does the following:
  - Initializes ATCOP as one of the clients to PBM.
  - Registers the client call back functions.
  
DEPENDENCIES
  None

RETURN VALUE
  None

SIDE EFFECTS
  None

===========================================================================*/
void dsatme_init_pbm ( void )
{
  nv_stat_enum_type status;
  
  (void) pbm_reg_event_cb(cpb_pbm_status_cb);

  /* Set verbose mode only during powerup (not ATZ/&F reset) 
   * for RESTRICTED command state case. */
  dsat_cmee_val = 2;

  /*-------------------------------------------------------------------------
    Get Primary DNS IP addresses from NV. Look at the status returned. If
    status !=NV_DONE_S, then either NV was never written or it cannot be
    reused. Make IP addresses as NULL if NV was never written.
    Check whether the phone is in RUIM mode or NV mode.
  -------------------------------------------------------------------------*/
  status = dsatutil_get_nv_item( NV_RTRE_CONFIG_I, &ds_nv_item );
  if ( status == NV_DONE_S )
  {
    /*-----------------------------------------------------------------------
      Store number retrieved from NV.
      In NV only PBM does not send ready indication, set the same.
     -----------------------------------------------------------------------*/
     if(ds_nv_item.rtre_config == NV_RTRE_CONFIG_NV_ONLY)
     {
       dsatme_pbm_ready = TRUE;
       dsatme_nv_only = TRUE;
     }
  }
  else 
  {
    MSG_ERROR( "Bad NV_RTRE_CONFIG_I read status %d for Pri DNS NV", status, 0, 0 );
  }
  
  dsatme_reset_pb();
  
  return;
}/* dsatme_init_pbm */

/*===========================================================================

FUNCTION DSATME_PBM_CB_CMD_HANDLER

DESCRIPTION
  Processes DS_AT_PBM_CB_CMD cmd infos.

DEPENDENCIES
  None

RETURN VALUE
  Returns the command running status.
    DSAT_ASYNC_CMD   : waiting for next cb.
    DSAT_ERROR       : if there is error.
    DSAT_CMD_ERR_RSP : if there is cme error.
    DSAT_OK          : if it is a success.

SIDE EFFECTS
  None

===========================================================================*/
/* ARGSUSED */
dsat_result_enum_type dsatme_pbm_cb_cmd_handler
(
  dsat_mode_enum_type   mode,
  ds_cmd_type *         cmd_ptr
)
{
  int index;
  ASSERT(NULL != cmd_ptr);

  /* reset the cb_expect variable */
  cb_expected = CB_NONE;

  for(index = 0; ((pbm_cb_handler_table[index].cmd_id != CB_NONE)||
     (pbm_cb_handler_table[index].cmd_id != CB_MAX)); index++)
  {
    if((int8)pbm_cb_handler_table[index].cmd_id == cmd_ptr->cmd.pbm_info.cmd)
    {
      return pbm_cb_handler_table[index].handler(mode, cmd_ptr);
    }
  }
  
  /* the default action is to ignore the event */
  MSG_ERROR("wrong cmd %d",cmd_ptr->cmd.pbm_info.cmd,0,0);
  return DSAT_ASYNC_EVENT;
} /* dsatme_pbm_cb_cmd_handler */


/*===========================================================================

FUNCTION DSATME_RESET_PB

DESCRIPTION
  Reset ph book to default values.  AT cmd processor call this function
  at init time or when atz, at&f are executed.

DEPENDENCIES
  None

RETURN VALUE
  None

SIDE EFFECTS
  me_cpbs_entry will be set to default if dsat_cpbs_val is 0
  and me_cpbs_entry.device != PBM_DS_SM

===========================================================================*/
void dsatme_reset_pb(void)
{
  MSG_MED("dsatme_reset_pb",0,0,0);
  /* cpbs is pointed to SM or ME storage based on NV or RUIM mode */
  if(dsat_cpbs_val == 0 && 
     me_cpbs_entry.device != me_cpbs_default.device)
  {
    me_cpbs_entry = me_cpbs_default;
  }
  if(dsatme_nv_only == TRUE)
  {
    dsat_cpbs_val = 5;
    me_cpbs_entry = me_nvonly_cpbs_default;
  }
  MSG_MED("dsatme_nv_only = %d, dsat_cpbs_val = %d",
              dsatme_nv_only,dsat_cpbs_val,0);
} /* dsatme_reset_pb */

/*===========================================================================

FUNCTION DSATME_LD_INIT_RECORD

DESCRIPTION
  Prepare the ld_rec for saving dialing number into last dial ph book.

DEPENDENCIES
  None

RETURN VALUE
  None

SIDE EFFECTS
  None
  
===========================================================================*/
void dsatme_ld_init_record
(
  char * number, /* dialed number                         */
  int    type,   /* number type, -1 means "not provided"        */
  char * text    /* text associate with the dialed number */
)
{
  clk_julian_type     julian_time;

  if (number != NULL)
  {
    ld_rec.state = LD_DIALED;

    /* prepare the ld_rec for updating last dial ph book */
    ld_rec.rec.index = -1;
	/* need to verify with CPB_MAX_NUM_LENGTH */
    (void) std_strlcpy(ld_rec.rec.number, number, PBM_NUM_SIZE);

    if(type != -1)
    {
      ld_rec.rec.type= (uint8) type;
    }
    else
    {
      /* type of number is not given, figure it out by the number */
      if(number[0] == '+')
      {
        ld_rec.rec.type = TA_INTERNATIONAL;
      }
      else
      {
        ld_rec.rec.type = TA_OTHER;
      }
    }

    if(text != NULL)
    {
		/* need to verify with PBM_MAX_TEXT_LENGTH */
      (void) memcpy(ld_rec.rec.text, text, PBM_TEXT_SIZE_CHARS);
    }
    else
    {
      ld_rec.rec.text[0] = '\0';
    }

    /* fill out time fields */

    memset(&julian_time, 0, sizeof(clk_julian_type));

#if defined(FEATURE_PM1000) && defined (FEATURE_RTC)
    pm1000_get_rtc(&julian_time);
#elif defined(FEATURE_PMIC_RTC)
    clk_rtc_get_time(&julian_time);
#endif

    ld_rec.rec.time = clk_julian_to_secs(&julian_time);
    ld_rec.rec.duration = 0;

    ld_rec.rec.is_answered = FALSE;
  }
  else
  {
    MSG_LOW("dsatme_ld_init_record called with no dial number",0,0,0);
  }  
} /* dsatme_ld_init_record */


/*===========================================================================

FUNCTION DSATME_LD_CONNECTED

DESCRIPTION
  Change ld_rec state to be connected.

DEPENDENCIES
  None

RETURN VALUE
  None

SIDE EFFECTS
  None
  
===========================================================================*/
void dsatme_ld_connected
(
  void
)
{
  clk_julian_type     julian_time;

  if (ld_rec.state == LD_DIALED)
  {
    ld_rec.state = LD_CONNECTED;

    ld_rec.rec.is_answered = TRUE;

    /* get the current time at the connction */
    memset(&julian_time, 0, sizeof(clk_julian_type));

#if defined(FEATURE_PM1000) && defined (FEATURE_RTC)
    pm1000_get_rtc(&julian_time);
#elif defined(FEATURE_PMIC_RTC)
    clk_rtc_get_time(&julian_time);
#endif

    ld_rec.conn_time = clk_julian_to_secs(&julian_time);
  }

} /* dsatme_ld_connected */


/*===========================================================================

FUNCTION DSATME_LD_SAVE

DESCRIPTION
  Store ld_rec to the last dial ph book.

DEPENDENCIES
  None

RETURN VALUE
  None

SIDE EFFECTS
  None
  
===========================================================================*/
void dsatme_ld_save
(
  cm_call_type_e_type call_type,
  byte csd_rate_adaption
)
{

    uint32 data_sizes[DSAT_CPBW_MAX_FIELDS];
    uint8 *fields_ptr=NULL;
    uint32 fields_ptr_size;
    pbm_field_s_type *ptyped_fields_ptr = NULL;
    uint16 rec_id;
    uint32 num_fields = 0;
    uint16 name[PBM_TEXT_SIZE_CHARS];
    boolean write_flag = FALSE;
    
  if(ld_rec.state != LD_NONE)
  {
    if(ld_rec.state == LD_CONNECTED)
    {
      /* get the duration of the call */
      clk_julian_type     julian_time;
      dword               cur_time;
      
      ASSERT(ld_rec.rec.is_answered);

      /* get current time */
      memset(&julian_time, 0, sizeof(clk_julian_type));

#if defined(FEATURE_PM1000) && defined (FEATURE_RTC)
      pm1000_get_rtc(&julian_time);
#elif defined(FEATURE_PMIC_RTC)
      clk_rtc_get_time(&julian_time);
#endif

      cur_time = clk_julian_to_secs(&julian_time);
      ld_rec.rec.duration = cur_time - ld_rec.conn_time;
    }

    /* best effort write, ignore the return value */
#if defined(FEATURE_APP_RECENTCALLS) && defined(FEATURE_ETSI_PBM)
/* Add entry to call history only if the call is not
       a voice or emergency or CS video call */
    if(CM_CALL_TYPE_VOICE != call_type &&
        CM_CALL_TYPE_EMERGENCY != call_type &&
        !(CM_CALL_TYPE_CS_DATA == call_type && 
        csd_rate_adaption == H223_H245))
    write_flag = TRUE;
#else  /* defined(FEATURE_APP_RECENTCALLS) && defined(FEATURE_ETSI_PBM) */
    write_flag = TRUE;
#endif /* defined(FEATURE_APP_RECENTCALLS) && defined(FEATURE_ETSI_PBM) */

    if(write_flag==TRUE)
    {
      memcpy(name,ld_rec.rec.text,PBM_TEXT_SIZE_BYTES);
      data_sizes[CPB_FIELD_TEXT] = (pbmutils_wstrlen(name)+ 1) * sizeof(uint16);
      data_sizes[CPB_FIELD_NUMBER] =  strlen(ld_rec.rec.number)+ 1;
      data_sizes[CPB_FIELD_NUMBER_TYPE] =  sizeof(uint8);
      num_fields = DSAT_CPBW_MAX_FIELDS;

      if (FALSE == pbm_allocate_fields (&fields_ptr,
                                        &fields_ptr_size,
                                        num_fields,
                                        data_sizes))
      {
        MSG_ERROR("Error: Could not allocate size",0,0,0);
        return;
      }

      ptyped_fields_ptr = (pbm_field_s_type *) fields_ptr;

      pbm_fill_field(&ptyped_fields_ptr[CPB_FIELD_TEXT],PBM_FIELD_NAME,PBM_FT_UCS2,
                    (byte *)name, (uint16) data_sizes[CPB_FIELD_TEXT]);

      pbm_fill_field(&ptyped_fields_ptr[CPB_FIELD_NUMBER],PBM_FIELD_PHONE_GENERIC,
                     PBM_FT_PHONE,(byte *)ld_rec.rec.number, 
                     (uint16) data_sizes[CPB_FIELD_NUMBER]);

      pbm_fill_field(&ptyped_fields_ptr[CPB_FIELD_NUMBER_TYPE],PBM_FIELD_NUMBER_TYPE,
                     PBM_FT_BYTE, (byte *)&ld_rec.rec.type, 
                     (uint16) data_sizes[CPB_FIELD_NUMBER_TYPE]);

      rec_id = PBM_GET_FIRST_AVAILABLE;

      (void) pbm_record_write(PBM_DIALED, &rec_id,(int)PBM_CAT_NONE,
                              (int)num_fields, fields_ptr, (int)fields_ptr_size,
                              me_ld_pbm_write_cb, NULL);

      /* Now its safe to free the fields_ptr */
      pbm_free_fields(&fields_ptr,num_fields);
    }

    /* reset the state */
    ld_rec.state = LD_NONE;
  }

} /* dsatme_ld_save */
/*--------            gsdi function return status handlers       --------*/

#ifdef FEATURE_MMGSDI
/*===========================================================================

FUNCTION CPBS_PROCESS_PIN2_STATUS

DESCRIPTION
  Process the pin2 status for cpbs command.

  This function is called after the mmgsdi fullfiled the request to
  get the pin status.  The gsdi reply is in pin_status.

  If status show the ME is pin2 locked, continue to verify the password.
  Otherwise, set the ph book storage.

DEPENDENCIES
  None

RETURN VALUE
  DSAT_ASYNC_CMD if the request is sent to gsdi to verify pin
                 or sent to pbm to set the storage type.
  DSAT_CMD_ERR_RSP if a me error happens when pbm executes the 
                   set storage request.  A cme error is returned
                   to TE.
  DSAT_ERROR if the set storage request is refused by PBM.
             It should not happen.  A system ERR is submitted.
    
SIDE EFFECTS
  None

===========================================================================*/
LOCAL dsat_result_enum_type cpbs_process_pin2_status
(
   ds_at_cmd_status_type cmd_info
)
{
  dsat_result_enum_type result = DSAT_OK;
  boolean               pin2_locked;

  /*-------- Ensure GSDI call was successful --------*/
  if ((int16)MMGSDI_SUCCESS != (cmd_info.cmd_status & (int16)GSDI_STATUS_MASK))
  {
    MSG_HIGH ("MMGSDI call not successful",0,0,0);
    return DSAT_ERROR;
  }

  pin2_locked = (cmd_info.cmd_data != (int16)MMGSDI_PIN_DISABLED &&
                 cmd_info.cmd_data != (int16)MMGSDI_PIN_STATUS_NOT_INITIALIZED);

  /* if the ME is pin2 locked, verify the password */
  if(pin2_locked)
  {
    mmgsdi_return_enum_type mmgsdi_result;
    mmgsdi_data_type pin_data;

    pin_data.data_ptr = (uint8 *)me_cpbs_params.password;
    pin_data.data_len = strlen((char*) me_cpbs_params.password);

    cb_expected = CPBS_MMGSDI_PIN2_VERIFY;
    /* verify the pin, this will unlock the pin2 lock if the password
       is correct */
    VERIFY_PIN( MMGSDI_SLOT_1,
                MMGSDI_PIN2, 
                pin_data, 
                CPBS_MMGSDI_PIN2_VERIFY );

    if (mmgsdi_result == MMGSDI_SUCCESS)
    {
      result = DSAT_ASYNC_CMD;
    }
    else if (mmgsdi_result == MMGSDI_INCORRECT_PARAMS)
    {
      /* If attempted verification of pin2 failed due to incorrect pin
         show this result in next +CPIN command response */
      dsat_mask_pin2_state = FALSE;
      result = dsat_send_cme_error(DSAT_CME_SIM_PIN2_REQUIRED);
    }
    else 
    {
      result = DSAT_ERROR;
    }
  }
  else
  {
    /* call pbm_get_file_info to get the info about the storage 
       to be set */
    cb_expected = CPB_PBM_CB_FILE_INFO;
    /* error cases are also handled in the call-back */
    (void)pbm_extended_file_info(me_cpbs_params.pbm_device,
                                 me_pbm_file_info_cb);
    result = DSAT_ASYNC_CMD;
  }
  
  /* clear the password, we are not allowed to maintain a copy
     of the pin2 code */
  dsatutil_reset_password(&me_cpbs_params.password[0], sizeof(me_cpbs_params.password));
  
  return result;
} /* cpbs_process_pin2_status */


/*===========================================================================

FUNCTION CPBS_PROCESS_VERIFY_PIN_STATUS

DESCRIPTION
  Process the pin2 verification status for cpbs command.
  If status is fine, continue to set the storage type.

DEPENDENCIES
  None

RETURN VALUE
  Returns the command running status.
    DSAT_PARAM : if the parameter is not correct
    DSAT_CMD_ERR_RSP : if there was any problem in executing the command 
                 gsdi_verify_pin
    DSAT_OK : if it is a success.

SIDE EFFECTS
  None

===========================================================================*/
LOCAL dsat_result_enum_type cpbs_process_verify_pin_status
(
   ds_at_cmd_status_type cmd_info
)
{
  dsat_result_enum_type result = DSAT_OK;
  mmgsdi_return_enum_type mmgsdi_result =
    (mmgsdi_return_enum_type )(cmd_info.cmd_status & GSDI_STATUS_MASK);
  
  switch (mmgsdi_result)
  {
  case MMGSDI_SUCCESS:
    /* call pbm_get_file_info to get the info about the storage 
       to be set */
    cb_expected = CPB_PBM_CB_FILE_INFO;
    /* error cases are also handled in the call-back */
    (void) pbm_extended_file_info(me_cpbs_params.pbm_device,
                                  me_pbm_file_info_cb);
    result = DSAT_ASYNC_CMD;
    break;

  case MMGSDI_INCORRECT_CODE:
    result = dsat_send_cme_error(DSAT_CME_INCORRECT_PASSWORD);
    break;

  case MMGSDI_CODE_BLOCKED:
    /* If attempted verification of pin2 failed due to incorrect pin
       show this result in next +CPIN command response */
    dsat_mask_pin2_state = FALSE;
    result = dsat_send_cme_error(DSAT_CME_SIM_PUK2_REQUIRED);
    break;

  default:
    MSG_ERROR("Unsupported MMGSDI status : %d",mmgsdi_result,0,0);
    result = dsat_send_cme_error (DSAT_CME_SIM_FAILURE);
    break;
  }

  return result;
} /* cpbs_process_verify_pin_status */
#endif /* FEATURE_MMGSDI */

/*===========================================================================

FUNCTION DSATME_EXEC_CPBS_CMD

DESCRIPTION
  This function takes the result from the command line parser
  and executes it. It executes +CPBS/$QCCPBS command.
  The result is in res_buff_ptr.

DEPENDENCIES
  None

RETURN VALUE
  returns an enum that describes the result of the command execution.
  possible values:
    DSAT_ERROR :    if there was any problem in executing the command
    DSAT_OK :       if it is a success.
    DSAT_ASYNC_CMD: if this function invokes an async command
    DSAT_CMD_ERR_RSP : if PBM not ready

SIDE EFFECTS
  None

===========================================================================*/
/* ARGSUSED */
dsat_result_enum_type dsatme_exec_cpbs_cmd
(
  dsat_mode_enum_type mode,             /*  AT command mode:            */
  const dsati_cmd_type *parse_table,    /*  Ptr to cmd in parse table   */
  const tokens_struct_type *tok_ptr,    /*  Command tokens from parser  */
  dsm_item_type *res_buff_ptr           /*  Place to put response       */
)
{
  dsat_result_enum_type result = DSAT_OK;

  /* Check the number of arguments.                                        */
  if (tok_ptr->args_found > 2)
  {
    MSG_ERROR ("Too many arguements", 0, 0, 0);
    return(DSAT_ERROR);
  }

  /* process set and read command */
  if( (tok_ptr->op == NA) || 
      (tok_ptr->op == (NA|EQ|AR)) || 
      (tok_ptr->op == (NA | QU)) )
  {
    if(!dsatme_pbm_ready)
    {
      /* PBM subsystem is not ready: our status CB is called by PBM on
         SIM init completed so report SIM busy */
      dsatme_set_cme_error(DSAT_CME_SIM_BUSY, res_buff_ptr);
      return DSAT_CMD_ERR_RSP;
    }

    memset( (void*)me_cpbs_params.display_name, 0, 
            sizeof(me_cpbs_params.display_name) );
    (void)std_strlcpy( me_cpbs_params.display_name, 
                       (char *)parse_table->name,
                       sizeof(me_cpbs_params.display_name));

    if( cpbs_parse_arguments(tok_ptr, &me_cpbs_params) != DSAT_OK)
    {
      result = DSAT_ERROR;
    }
    else /* the arguments are correct */
    {
      /* check if the argument contains password
       the password is used to unlock the pin2 lock */
      if(tok_ptr->args_found == 2)
      {
#ifdef FEATURE_MMGSDI 
        mmgsdi_return_enum_type mmgsdi_status;
        cb_cmd_e_type  client_ref = CPBS_MMGSDI_PIN2_STATUS;
        /* process the pin2 code */
        cb_expected = CPBS_MMGSDI_PIN2_STATUS;
        GET_PIN_STATUS( MMGSDI_SLOT_1, client_ref);

        if (mmgsdi_status == MMGSDI_SUCCESS) 
        {
          result = DSAT_ASYNC_CMD;
        }
        else
        {
          /* MMGSDI command queue was full */
          dsatme_set_cme_error(DSAT_CME_MEMORY_FULL, res_buff_ptr);
          result = DSAT_CMD_ERR_RSP;
        }
#endif /* FEATURE_MMGSDI */ 
      }
      else
      {
        /* call pbm_get_file_info to get the info about the storage 
           to be set */
        cb_expected = CPB_PBM_CB_FILE_INFO;
        /* error cases are also handled in the call-back */
        (void) pbm_extended_file_info(me_cpbs_params.pbm_device,
                                      me_pbm_file_info_cb);
        result = DSAT_ASYNC_CMD;
      }
    }
  }

  /* process the TEST command */
  else if (tok_ptr->op == (NA|EQ|QU))
  {
    if(!dsatme_pbm_ready)
    {
      /* PBM subsystem is not ready: our status CB is called by PBM on
         SIM init completed so report SIM busy */
      dsatme_set_cme_error(DSAT_CME_SIM_BUSY, res_buff_ptr);
      return DSAT_CMD_ERR_RSP;
    }

    result = cpbs_process_test_cmd(
                                   parse_table,
                                   res_buff_ptr
                                   );
  }

  /* command does not make sense */
  else
  {
    result = DSAT_ERROR;
  }

  return result;
} /* dsatme_exec_cpbs_cmd */

/*===========================================================================

FUNCTION DSATME_EXEC_CPBR_CMD

DESCRIPTION
  This function takes the result from the command line parser
  and executes it. It executes +CPBR/$QCCPBR command.
  +CPBR command is used to read phonebook entries.

DEPENDENCIES
  None

RETURN VALUE
  returns an enum that describes the result of the command execution.
  possible values:
    DSAT_OK        : success
    DSAT_ERROR     : if there was any problem in executing the command
    DSAT_ASYNC_CMD : Async call is successfully made to pbm to read
                     ph book number
    DSAT_CMD_ERR_RSP : if PBM not ready

SIDE EFFECTS
  None

===========================================================================*/
/* ARGSUSED */
dsat_result_enum_type dsatme_exec_cpbr_cmd
(
  dsat_mode_enum_type mode,               /*  AT command mode:            */
  const dsati_cmd_type *parse_table,      /*  Ptr to cmd in parse table   */
  const tokens_struct_type *tok_ptr,      /*  Command tokens from parser  */
  dsm_item_type *res_buff_ptr             /*  Place to put response       */
)
{
  dsat_result_enum_type result = DSAT_OK;

  /* the function will write to the beginning of the buffer,
     make sure it is empty */
  ASSERT(res_buff_ptr->used == 0);

  /* Check the number of arguments.                                        */
  if (tok_ptr->args_found > parse_table->compound)
  {
    MSG_ERROR ("Too many arguements", 0, 0, 0);
    return(DSAT_ERROR);
  }

  if(!dsatme_pbm_ready)
  {
    /* PBM subsystem is not ready: our status CB is called by PBM on
       SIM init completed so report SIM busy */
    dsatme_set_cme_error(DSAT_CME_SIM_BUSY, res_buff_ptr);
    return DSAT_CMD_ERR_RSP;
  }

  /* Process execution command */
  if ( tok_ptr->op == (NA|EQ|AR) )
  {
    /* Update cpbr_index.current, cpbr_index.last from command arguments.
	   We have to maintain cpbr_index.last through out because even if 
	   we re-enter reading phone book records, it knows when to stop.*/
    if( (result = cpbr_parse_arguments(tok_ptr, &cpbr_index.current, 
                                       &cpbr_index.last, res_buff_ptr)
		                              ) != DSAT_OK )
    {
      return result;
    }

    memset( (void*)cpbr_index.display_name, 0, 
            sizeof(cpbr_index.display_name) );
    
    (void)std_strlcpy( cpbr_index.display_name, 
                       (char *) parse_table->name,
                       sizeof(cpbr_index.display_name));

    /* Start reading from Phone Book */
    result = cpb_pbm_read_handler();
  }
    /* Process test command */
  else if ( tok_ptr->op == (NA|EQ|QU) )
  {
    char * buff_ptr = (char *) res_buff_ptr->data_ptr;
    
    if (me_cpbs_entry.total < 1) 
    {
      /* When the total and used is not supported, they should be -1 */
      ASSERT(me_cpbs_entry.total == -1);

      /* the location range is not available */
      res_buff_ptr->used = (word) std_strlprintf(buff_ptr, res_buff_ptr->size,
                                                 "%s: ",(char *) parse_table->name);
    }
    else
    {
      res_buff_ptr->used = (word) std_strlprintf(buff_ptr, 
                              res_buff_ptr->size, 
                              "%s: (1-%d),%d,%d",
                              (char *) parse_table->name,
                              me_cpbs_entry.total,
                              CPB_MAX_NUM_LENGTH, 
                              me_cpbs_entry.text_len);
    } 
  }
  /* command does not make sense */
  else
  {
    result = DSAT_ERROR;
  }
  return result;
} /* dsatme_exec_cpbr_cmd () */

/*===========================================================================

FUNCTION DSATME_EXEC_CPBF_CMD

DESCRIPTION
  This function takes the result from the command line parser
  and executes it. It executes +CPBF/$QCCPBF command.
  +CPBF command is used to search phonebook entries.

DEPENDENCIES
  None

RETURN VALUE
  returns an enum that describes the result of the command execution.
  possible values:
    DSAT_OK          : success
    DSAT_ERROR       : if there was any problem in executing the command
    DSAT_ASYNC_CMD   : Async call is successfully made to pbm to find a
                       number in the ph book
    DSAT_CMD_ERR_RSP : if there is cme error.

SIDE EFFECTS
  None

===========================================================================*/
/* ARGSUSED */
dsat_result_enum_type dsatme_exec_cpbf_cmd
(
  dsat_mode_enum_type mode,               /*  AT command mode:            */
  const dsati_cmd_type *parse_table,      /*  Ptr to cmd in parse table   */
  const tokens_struct_type *tok_ptr,      /*  Command tokens from parser  */
  dsm_item_type *res_buff_ptr             /*  Place to put response       */
)
{
  dsat_result_enum_type result = DSAT_OK;

  /* Check the number of arguments.                                        */
  if (tok_ptr->args_found > parse_table->compound)
  {
    MSG_ERROR ("Too many arguements", 0, 0, 0);
    return(DSAT_ERROR);
  }

  if(!dsatme_pbm_ready)
  {
    /* PBM subsystem is not ready: our status CB is called by PBM on
       SIM init completed so report SIM busy */
    dsatme_set_cme_error(DSAT_CME_SIM_BUSY, res_buff_ptr);
    return DSAT_CMD_ERR_RSP;
  }

  /* we don't support find to last dial list because pbm does 
       not support it */
  if(me_cpbs_entry.device == PBM_DS_LD)
  {
    dsatme_set_cme_error(DSAT_CME_OP_NOT_SUPPORTED, res_buff_ptr);
    return DSAT_CMD_ERR_RSP;
  }

  /* Process execution command */
  if ( tok_ptr->op == (NA|EQ|AR) )
  {
    pbm_return_type pbm_result;
    char findtext[PBM_MAX_TEXT_LENGTH];

    memset( (void*)findtext, 0, sizeof(findtext) );

    /* attempt to strip out the "s from the tok_ptr->arg[0] and
       convert from TE alphabet if needed: on failure return ERROR */

    if ( !VALID_TOKEN(0) )
    {
      /* Parameter value error */
      return DSAT_ERROR;
    }

    if ( !dsatutil_strip_quotes_out(tok_ptr->arg[0],
                                    tok_ptr->arg[0],
                                    (PBM_MAX_TEXT_LENGTH * UCS2_CHAR_LEN) + 1) ||
         dsatutil_convert_chset((const char *)tok_ptr->arg[0], 
                                ALPHA_IRA,
                                &findtext[0],
                                ALPHA_PBM_UCS2,
                                PBM_MAX_TEXT_LENGTH, 
                                FALSE) == NULL )
    {
      dsatme_set_cme_error(DSAT_CME_INVALID_CHAR_IN_TEXT, res_buff_ptr);
      return DSAT_ERROR;
    }
    /* Initialize search of the address book */
    pbm_result = pbm_enum_rec_init(me_cpbs_entry.device,
                                   (uint16)PBM_CAT_NONE,
                                   PBM_FIELD_NAME,
                                   (uint8 *)findtext,
                                   (uint16)((pbmutils_wstrlen((uint16*)findtext) + 1) * sizeof(uint16)),
                                   PBM_SEARCHTYPE_STARTSWITH|PBM_STRTYPE_UCS2);

    /* Search initialization successful */
    if (pbm_result == PBM_SUCCESS )
    {
      /* start the search */
      result = cpb_pbm_find_handler();
    }
    else if ( pbm_result == PBM_ERROR_NOT_FOUND )
    {
      result = dsat_send_cme_error(DSAT_CME_NOT_FOUND);
    }
    else
    { /* check for all other possible errors */
      result = cpb_pbm_find_handler_status_fail(pbm_result);
    }
  }
  /* Process test command */
  else if ( tok_ptr->op == (NA|EQ|QU) )
  {
    char * buff_ptr = (char *) res_buff_ptr->data_ptr;

    /* if tlength info is not available */
    if(me_cpbs_entry.text_len == -1)
    {
      res_buff_ptr->used = (word) std_strlprintf(buff_ptr,
                                                 res_buff_ptr->size, 
                                                 "%s",(char *) parse_table->name);
    }
    else
    {
      res_buff_ptr->used = (word) std_strlprintf(buff_ptr,
                                                 res_buff_ptr->size, 
                                                 "%s: %d,%d",(char *) parse_table->name,
                                                 CPB_MAX_NUM_LENGTH, 
                                                 me_cpbs_entry.text_len);
    }
  }
  /* command does not make sense */
  else
  {
    result = DSAT_ERROR;
  }

  return result;
} /* dsatme_exec_cpbf_cmd () */


/*===========================================================================

FUNCTION DSATME_EXEC_CPBW_CMD

DESCRIPTION
  This function takes the result from the command line parser
  and executes it. It executes +CPBW/$QCCPBW command.
  +CPBW command is used to write phonebook entry.

DEPENDENCIES
  None

RETURN VALUE
  returns an enum that describes the result of the command execution.
  possible values:
    DSAT_OK          : success
    DSAT_ASYNC_CMD   : Async call is successfully made to pbm to write a
                       number into the ph book
    DSAT_ERROR       : if there was any problem in executing the command
    DSAT_CMD_ERR_RSP : if there is cme error.

SIDE EFFECTS
  None

===========================================================================*/
/* ARGSUSED */
dsat_result_enum_type dsatme_exec_cpbw_cmd
(
  dsat_mode_enum_type mode,               /*  AT command mode:            */
  const dsati_cmd_type *parse_table,      /*  Ptr to cmd in parse table   */
  const tokens_struct_type *tok_ptr,      /*  Command tokens from parser  */
  dsm_item_type *res_buff_ptr             /*  Place to put response       */
)
{
  dsat_result_enum_type result = DSAT_OK;

  /* Check the number of arguments.                                        */
  if (tok_ptr->args_found > parse_table->compound)
  {
    MSG_ERROR ("Too many arguements", 0, 0, 0);
    return(DSAT_ERROR);
  }

  if(!dsatme_pbm_ready)
  {
    /* PBM subsystem is not ready: our status CB is called by PBM on
       SIM init completed so report SIM busy */
    dsatme_set_cme_error(DSAT_CME_SIM_BUSY, res_buff_ptr);
    return DSAT_CMD_ERR_RSP;
  }

  /* Phone book types for which write is allowed are  "SM", "FD", "ON" & "ME" only */
  if(!((me_cpbs_entry.device == PBM_ADN) ||
      (me_cpbs_entry.device == PBM_FDN) ||
      (me_cpbs_entry.device == PBM_MSISDN) ||
      (me_cpbs_entry.device == PBM_EFS)))
  {
    dsatme_set_cme_error(DSAT_CME_OP_NOT_ALLOWED, res_buff_ptr);
    return DSAT_CMD_ERR_RSP;
  }

  /* process action command */
  if( tok_ptr->op == (NA|EQ|AR) )
  {
    dsat_pbm_record_s_type record;
    pbm_return_type pbm_result;
    uint32 data_sizes[DSAT_CPBW_MAX_FIELDS];
    uint8 *fields_ptr = NULL;
    uint32 fields_ptr_size = 0;
    pbm_field_s_type *ptyped_fields_ptr = NULL;
    uint16 rec_id;
    uint32 num_fields = 0;
    uint16 name[PBM_TEXT_SIZE_CHARS];
    
    if( (result = cpbw_parse_arguments(tok_ptr, &record, res_buff_ptr)) == DSAT_OK )
    {
      /* If number is not provided, then it is for deletion*/
      if (record.number[0] != NULL)
      {
        /* Update the data_sizes sizeof(uint16) factor is to support FULL UCS2
           For string types provide space for NULL character */ 
        memcpy(name,record.text,PBM_TEXT_SIZE_BYTES);
        data_sizes[CPB_FIELD_TEXT] = (pbmutils_wstrlen(name)+ 1) * sizeof(uint16);
        data_sizes[CPB_FIELD_NUMBER] =  strlen(record.number)+ 1;
        data_sizes[CPB_FIELD_NUMBER_TYPE] =  sizeof(uint8);
        num_fields = DSAT_CPBW_MAX_FIELDS;

        if (FALSE == pbm_allocate_fields
               (&fields_ptr, &fields_ptr_size, num_fields, data_sizes))
        {
          MSG_ERROR("Error: Could not allocate size",0,0,0);
          return DSAT_ERROR;
        }
        
        ptyped_fields_ptr = (pbm_field_s_type *) fields_ptr;

        pbm_fill_field(&ptyped_fields_ptr[CPB_FIELD_TEXT], PBM_FIELD_NAME,PBM_FT_UCS2,
                       (byte *)(&record.text[0]), (uint16) data_sizes[CPB_FIELD_TEXT]);

        pbm_fill_field(&ptyped_fields_ptr[CPB_FIELD_NUMBER], PBM_FIELD_PHONE_GENERIC,
                       PBM_FT_PHONE,(byte *)record.number, 
                       (uint16) data_sizes[CPB_FIELD_NUMBER]);

        pbm_fill_field(&ptyped_fields_ptr[CPB_FIELD_NUMBER_TYPE], PBM_FIELD_GSM_TONNPI,
                       PBM_FT_BYTE, (byte *)&record.type, 
                       (uint16) data_sizes[CPB_FIELD_NUMBER_TYPE]);

        /* If valid index is provided, update in that location else get the first 
           avalible index from PBM */
        rec_id = (record.index>0)? 
                  pbm_location_to_record_id(me_cpbs_entry.device,record.index):
                  PBM_GET_FIRST_AVAILABLE;
      }
      else
      {
        /* For deletion fields_ptr should be NULL and num_fields must be zero */
        fields_ptr = NULL;
        num_fields = 0;
        rec_id = pbm_location_to_record_id(me_cpbs_entry.device,record.index);
      }
      
      cb_expected = CPB_PBM_CB_WRITE;
      pbm_result = pbm_record_write(me_cpbs_entry.device, &rec_id,(uint16)PBM_CAT_NONE,
                                    (int)num_fields, fields_ptr, fields_ptr_size,
                                    me_pbm_write_cb, NULL);
      result = process_pbm_record_write_return(pbm_result, res_buff_ptr);

      if(result != DSAT_ASYNC_CMD)
      {
        cb_expected = CB_NONE;
      }
      /* Now its safe to free the fields_ptr */
      pbm_free_fields(&fields_ptr,num_fields);
    }
  }
  
  /* Process test command */
  else if ( tok_ptr->op == (NA|EQ|QU) )
  {
    char * buff_ptr = (char *) res_buff_ptr->data_ptr;
    int total = me_cpbs_entry.total > 0 ? me_cpbs_entry.total : 1;
    char text_len_str[4];

    if(me_cpbs_entry.text_len < 0)
    {
      text_len_str[0] = '\0';
    }
    else
    {
      (void) dsatutil_itoa((word) me_cpbs_entry.text_len, 
                           (byte *)text_len_str, 10);
    }

    res_buff_ptr->used = 
        (word) std_strlprintf(buff_ptr,res_buff_ptr->size, 
                              "%s: (1-%d),%d,(128-255),%s",
                              (char*)parse_table->name,
                              total,
                              CPB_MAX_NUM_LENGTH, text_len_str);
  }
  
  /* command does not make sense */
  else
  {
    result = DSAT_ERROR;
  }

  return result;

} /* dsatme_exec_cpbw_cmd () */
#endif /* defined(FEATURE_ETSI_PBM) || defined(FEATURE_DSAT_CDMA_PBM) */

/*===========================================================================

FUNCTION  DSATETSIME_SET_CME_ERROR

DESCRIPTION
  Write Mobile Equipment error to res_buff_ptr.  The Mobile Equipment error
  code and error text are defined in Section 9.2 of 3GPP TS 27.007

  If AT+CMEE is 0, +CME ERROR : <err> result code is disabled
                   ERROR is used
  If AT+CMEE is 1, +CME ERROR : <err> result code is enabled
                   to used numeric err value
  If AT+CMEE is 2, +CME ERROR : <err> result code is enabled
                   to used verbose text err value

DEPENDENCIES
  res_buff_ptr point to a good dsm buffer with enough space left 
  (at lease 60 bytes) to write error code.
  
RETURN VALUE
  None
  
SIDE EFFECTS
  None
===========================================================================*/
void dsatme_set_cme_error
(
  dsat_cme_error_e_type error_code,
  dsm_item_type         *res_buff_ptr
)
{
  /* err code table according to Section 9.2 of 3GPP TS 27.007 */
  static const struct {
    dsat_cme_error_e_type cme_code;
    char * cme_text;
  } cme_table[] = {
    DSAT_CME_PHONE_FAILURE,            "phone failure",
    DSAT_CME_NO_CONNECTION,            "no connection to phone",
    DSAT_CME_ADAPTOR_LINK_RESERVED,    "phone adaptor link reserved",
    DSAT_CME_OP_NOT_ALLOWED,           "operation not allowed",
    DSAT_CME_OP_NOT_SUPPORTED,         "operation not supported",
    DSAT_CME_PH_SIM_PIN_REQUIRED,      "PH-SIM PIN required",
    DSAT_CME_PH_FSIM_PIN_REQUIRED,     "PH-FSIM PIN required",
    DSAT_CME_PH_FSIM_PUK_REQUIRED,     "PH-FSIM PUK required",
    DSAT_CME_SIM_NOT_INSERTED,         "SIM not inserted",
    DSAT_CME_SIM_PIN_REQUIRED,         "SIM PIN required",
    DSAT_CME_SIM_PUK_REQUIRED,         "SIM PUK required",
    DSAT_CME_SIM_PIN2_REQUIRED,        "SIM PIN2 required",
    DSAT_CME_SIM_PUK2_REQUIRED,        "SIM PUK2 required",
    DSAT_CME_SIM_FAILURE,              "SIM failure",
    DSAT_CME_SIM_BUSY,                 "SIM busy",
    DSAT_CME_SIM_WRONG,                "SIM wrong",
#ifdef FEATURE_DUAL_SLOTS
    DSAT_CME_SIM_NOT_INSERTED_2,        "SIM2 not inserted",
    DSAT_CME_SIM_PIN_REQUIRED_2,        "SIM2 PIN required",
    DSAT_CME_SIM_PUK_REQUIRED_2,        "SIM2 PUK required",
    DSAT_CME_SIM_PIN2_REQUIRED_2,       "SIM2 PIN2 required",
    DSAT_CME_SIM_PUK2_REQUIRED_2,       "SIM2 PUK2 required",
    DSAT_CME_SIM_FAILURE_2,             "SIM2 failure",
    DSAT_CME_SIM_BUSY_2,                "SIM2 busy",
    DSAT_CME_SIM_WRONG_2,               "SIM2 wrong",
#endif /* FEATURE_DUAL_SLOTS */
    DSAT_CME_INCORRECT_PASSWORD,       "incorrect password",
    DSAT_CME_MEMORY_FULL,              "memory full",
    DSAT_CME_INVALID_INDEX,            "invalid index",
    DSAT_CME_NOT_FOUND,                "not found",
    DSAT_CME_MEMORY_FAILURE,           "memory failure",
    DSAT_CME_TEXT_STRING_TOO_LONG,     "text string too long",
    DSAT_CME_INVALID_CHAR_IN_TEXT,     "invalid characters in text string",
    DSAT_CME_DIAL_STRING_TOO_LONG,     "dial string too long",
    DSAT_CME_INVALID_CHAR_IN_DIAL_STRING,
                                        "invalid characters in dial string",
    DSAT_CME_NO_NETWORK_SERVICE,       "no network service",
    DSAT_CME_NETWORK_TIMEOUT,          "network timeout",
    DSAT_CME_NETWORK_NOT_ALLOWED, "network not allowed - emergency calls only",
    DSAT_CME_NET_PERSON_PIN_REQUIRED,  "network personalization PIN required",
    DSAT_CME_NET_PERSON_PUK_REQUIRED,  "network personalization PUK required",

    DSAT_CME_NET_SUBSET_PERSON_PIN_REQUIRED,
                               "network subset personalization PIN required",

    DSAT_CME_NET_SUBSET_PERSON_PUK_REQUIRED, 
                               "network subset personalization PUK required",

    DSAT_CME_SP_PERSON_PIN_REQUIRED,   
                               "service provider personalization PIN required",

    DSAT_CME_SP_PERSON_PUK_REQUIRED,   
                               "service provider personalization PUK required",

    DSAT_CME_CORP_PERSON_PIN_REQUIRED, 
                               "corporate personalization PIN required",

    DSAT_CME_CORP_PERSON_PUK_REQUIRED, 
                               "corporate personalization PUK required",

    DSAT_CME_UNKNOWN,                  "unknown",
    DSAT_CME_ILLEGAL_MS,               "Illegal MESSAGE",
    DSAT_CME_ILLEGAL_ME,               "Illegal ME",
    DSAT_CME_GPRS_serv_not_allowed,    "GPRS services not allowed",
    DSAT_CME_PLMN_NOT_ALLOWED,         "PLMN not allowed",
    DSAT_CME_LOCATION_NOT_ALLOWED,     "Location area not allowed",
    DSAT_CME_ROAMING_NOT_ALLOWED,  "Roaming not allowed in this location area",
    DSAT_CME_SERV_OP_NOT_SUPPORTED,    "service option not supported",

    DSAT_CME_REQ_SERV_OP_NOT_SUBSCRIBED, 
                                    "requested service option not subscribed",

    DSAT_CME_SERV_OP_TEMP_OUT_OF_ORDER,  
                                    "service option temporarily out of order",

    DSAT_CME_unspecified_GPRS_error,   "unspecified GPRS error",
    DSAT_CME_PDP_AUTH_FAILURE,         "PDP authentication failure",
    DSAT_CME_INVALID_MOBILE_CLASS,     "invalid mobile class",

    DSAT_CME_SUPS_NETWORK_REJECTED,    "network rejected request",
    DSAT_CME_SUPS_RETRY_OPERATION,     "retry operation",
    DSAT_CME_SUPS_DFL_INVALID_NUMBER,  "invalid deflected to number",
    DSAT_CME_SUPS_DFL_SELF_NUMBER,     "deflected to own number",
    DSAT_CME_SUPS_UNKNOWN_NUMBER,      "unknown subscriber",
    DSAT_CME_SUPS_SERVICE_UNAVAIL,     "service not available",
    DSAT_CME_SUPS_UNKNOWN_CLASS,       "unknown class specified",
    DSAT_CME_SUPS_UNKNOWN_NETMSG,      "unknown network message",
#ifdef FEATURE_SECONDARY_PDP
    DSAT_CME_INVALID_PARAM_COMB,       "invalid parameter combination",
    DSAT_CME_MIN_TFT_PER_ADDR,         "minimum TFTs per PDP address violated",
    DSAT_CME_TFT_EPI_DUPLICATE,        "TFT precedence index not unique",  
#endif /* FEATURE_SECONDARY_PDP */
    DSAT_CME_MAX,                      NULL
  };
  
  char * buff_ptr = (char *) (res_buff_ptr->data_ptr + res_buff_ptr->used);
  /* cmee enables the ME error result code */
  if( dsat_cmee_val == 1 || dsat_cmee_val == 2 )
  {
    int index = 0;
    char * cme_string = NULL;

    /* search for the cme table for the error code/text */
    do
    {
      cme_string = cme_table[index].cme_text;
    } while (cme_table[index++].cme_code != error_code &&
             cme_string != NULL);
  
    if(cme_string == NULL)
    {
      ERR("Incorrect cme code %d", error_code, 0, 0);
      return;
    }
  
    if (dsat_cmee_val == 1)
    {
      res_buff_ptr->used += (word)std_strlprintf(buff_ptr,
                                           res_buff_ptr->size - res_buff_ptr->used,
                                           "+CME ERROR: %d",
                                           error_code);
    }
    else
    {
      res_buff_ptr->used += (word)std_strlprintf(buff_ptr, 
                                          res_buff_ptr->size - res_buff_ptr->used,
                                          "+CME ERROR: %s",
                                          cme_string);
    }
  }
  else /* AT+CMEE = 0 */
  {
    res_buff_ptr->used += (word)std_strlprintf(buff_ptr, 
                                          res_buff_ptr->size - res_buff_ptr->used,
                                          "ERROR");
  }

} /* dsatme_set_cme_error( ) */

#if defined(FEATURE_ETSI_PBM) || defined(FEATURE_DSAT_CDMA_PBM)
/*--------              pbm cb cmd handlers                      --------*/
/*===========================================================================

FUNCTION CPB_PBM_READ_RECORD

DESCRIPTION
  This function is used to read record from pbm.

DEPENDENCIES
  None

RETURN VALUE
  None

SIDE EFFECTS
  None
===========================================================================*/
LOCAL void cpb_pbm_read_record
(
  uint16                     record_id,
  dsat_pbm_record_s_type     *record_ptr,
  pbm_return_type            *pbm_return_ptr
)
{
  uint8 fields_buf[PB_MAX_REC_LEN];
  uint32 fields_ptr_size;
  uint16 category;
  pbm_field_s_type *fields_ptr;
  int num_fields;
  uint16 i;

  fields_ptr_size = (uint32)pbm_calculate_fields_size_from_id(record_id);
  fields_ptr = (pbm_field_s_type *)fields_buf;
  if(fields_ptr_size == 0)
  {
    *pbm_return_ptr = pbm_record_read (record_id,
                                         &category,
                                         &num_fields,
                                         NULL,
                                         fields_ptr_size );

  }
  else
  {  
    *pbm_return_ptr = pbm_record_read (record_id,
                                       &category,
                                       &num_fields,
                                       fields_buf,
                                       fields_ptr_size );
  }
  if(*pbm_return_ptr == PBM_SUCCESS)
  {
    /* Fill in the record with default values. */
    record_ptr->index = pbm_record_id_to_location(record_id);
    memset(record_ptr, 0, sizeof(dsat_pbm_record_s_type));
    record_ptr->type = TA_OTHER;
    /* Process the information from the buffer */
    for ( i = 0; i < num_fields ; i++)
    {
      switch (fields_ptr[i].field_id)
      {
        case PBM_FIELD_HARDCODED_ECC:
        case PBM_FIELD_PHONE_GENERIC:
          memcpy(record_ptr->number,
                 (const void *)pbm_get_field_data_ptr(fields_ptr,i),
                 fields_ptr[i].data_len );
          break;
        case PBM_FIELD_NAME:
          memcpy(record_ptr->text,
                 (const void *)pbm_get_field_data_ptr(fields_ptr,i),
                 fields_ptr[i].data_len );
          break;
        case PBM_FIELD_GSM_TONNPI:
          memcpy(&(record_ptr->type),
                 (const void *)pbm_get_field_data_ptr(fields_ptr,i),
                 fields_ptr[i].data_len );
          break;
        default: 
          /* Rest of the fields are ignored, since not required for CPBR */
          break;
      }
    }
    if (record_ptr->number[0] == '+')
      record_ptr->type = TA_INTERNATIONAL;
  }
} /* cpb_pbm_read_record */

/*===========================================================================

FUNCTION CPB_PBM_READ_HANDLER

DESCRIPTION
  This function is called during CPBR execution for reading PB records
  and sends the response to terminal.

DEPENDENCIES
  None

RETURN VALUE
  DSAT_OK          : If reading of all records complete.
  DSAT_ASYNC_CMD   : Additional entries remain to be read.
  DSAT_CMD_ERR_RSP : If there is a CME ERROR.
  DSAT_ERROR       : Read action fails due to non-CME errors.

SIDE EFFECTS
  None
===========================================================================*/
LOCAL dsat_result_enum_type cpb_pbm_read_handler
(
  void
)
{
  /* Remember if the the record is at the beginning, middle, or end
     of the data */
  static dsat_rsp_enum_type async_resp = DSAT_START_OF_RSP;
  dsat_result_enum_type result = DSAT_OK;

  /* For each index from current to last perform pbm_record_read.*/
  for ( ;cpbr_index.current <= cpbr_index.last; ++cpbr_index.current )
  {
    uint16 rec_id;
    pbm_return_T pbm_return;
    dsat_pbm_record_s_type record;

    rec_id = pbm_location_to_record_id(me_cpbs_entry.device,cpbr_index.current);
    cpb_pbm_read_record(rec_id, &record, &pbm_return);

    if ( pbm_return == PBM_SUCCESS )
    {
      result = cpb_pbm_read_handler_normal_status(&record, &async_resp);
    }
    /* If no record found until last index, send error response 
       else return DSAT_OK */
    else if ( pbm_return == PBM_ERROR_NOT_FOUND )
    {
      if ( cpbr_index.current == cpbr_index.last )
      {
        result = (async_resp == DSAT_START_OF_RSP)?
          dsat_send_cme_error(DSAT_CME_NOT_FOUND):DSAT_OK;
      }
    }
    else
    {
      result = cpb_pbm_read_handler_error_status(pbm_return);
      break;
    }

    /* Check for SIO TX Water mark high case : This is needed because +CPBR 
	   command can pump huge data to SIO for transmitting out */
    if( result == DSAT_ASYNC_CMD )
    {
      /* Additional entries remain to be read */
#ifdef FEATURE_DSAT_FLOW_CONTROL
      if ( dsati_tx_wm_high )
      {
        /* High watermark has been hit on dsat to sio queue: 
           set up handler to re-enter PB entry reading once queue hits low
           watermark */
        dsati_to_sio_low_handler = cpb_pbm_reenter_read_handler;
        return result;
      }
#endif /* FEATURE_DSAT_FLOW_CONTROL */

      /* If DSM large item free count has gone below a threshold quantity... */
      if ( DSM_POOL_FREE_CNT(DSM_DS_LARGE_ITEM_POOL) < DSAT_FREE_ITEM_TRIGGER )
      {
        dsat_timer_s_type *t_ptr = &dsatutil_timer_table[DSAT_TIMER_FREE_ITEM];

        /* Set timer expiry handler function */
        dsati_handle_free_item_timer_expiry = cpb_pbm_reenter_read_handler;

        /* Set timer and await its expiration */
        (void) rex_set_timer( t_ptr->timer_ptr, t_ptr->duration );
        t_ptr->is_running = TRUE;
        return DSAT_ASYNC_CMD;
      }
    }/* (result == DSAT_ASYNC_CMD) */

  /* continue reading next index */
  }/* for (cpbr_index.current .. )*/

  /* +cpbr finishes. reset static variable async_rsp */
  async_resp = DSAT_START_OF_RSP;

  return result;
} /* cpb_pbm_read_handler */

/*===========================================================================

FUNCTION CPB_PBM_REENTER_READ_HANDLER

DESCRIPTION
  Local function that is used to reenter reading PB entries once DSAT to SIO
  queue has hit low watermark (or) DSM large item threshold timer expiry.

DEPENDENCIES
  none

RETURN VALUE
  DSAT_OK          : If reading of all records complete.
  DSAT_ASYNC_CMD   : Additional entries remain to be read.
  DSAT_CMD_ERR_RSP : If there is a CME ERROR.
  DSAT_ERROR       : Read action fails due to non-CME errors.

SIDE EFFECTS
  None
===========================================================================*/
/* ARGSUSED */
LOCAL dsat_result_enum_type cpb_pbm_reenter_read_handler
(
  dsat_mode_enum_type mode,
  ds_cmd_type* cmd_ptr
)
{
  dsat_result_enum_type result;

  /* Reset handler function pointers */
#ifdef FEATURE_DSAT_FLOW_CONTROL
  /* Handler function for low event, now not neccesary */
  dsati_to_sio_low_handler = NULL;
#endif /* FEATURE_DSAT_FLOW_CONTROL */
  dsati_handle_free_item_timer_expiry = NULL;

  /* continue reading next index */
  ++cpbr_index.current;

  MSG_MED("cpb_pbm_cb_reenter_read_handler: reading record from %d",
          cpbr_index.current, 0, 0);

  result = cpb_pbm_read_handler ();

  return result;
}/* cpb_pbm_reenter_read_handler */

/*===========================================================================

FUNCTION CPB_PBM_READ_HANDLER_NORMAL_STATUS

DESCRIPTION
  This function is used to process the normal status for pbm_record_read

DEPENDENCIES
  None

RETURN VALUE
  DSAT_OK          : If reached end of reading records.
  DSAT_ASYNC_CMD   : Additional entries remain to be read.
  DSAT_CMD_ERR_RSP : If there is a CME ERROR.

SIDE EFFECTS
  None 
===========================================================================*/
/* ARGSUSED */
LOCAL dsat_result_enum_type cpb_pbm_read_handler_normal_status
(
  dsat_pbm_record_s_type  *record_ptr,
  dsat_rsp_enum_type * rsp_ptr      /* if this record is at the beginning, 
                                       middle, or end of all the records
                                       to be read                         */
)
{
  dsm_item_type * raw_rsp_ptr = NULL;
  char name[PBM_TEXT_SIZE_CHARS];
  dsat_result_enum_type result = DSAT_ASYNC_CMD;

  if (record_ptr != NULL)
  {
    memset(name, '\0', PBM_TEXT_SIZE_CHARS);
    if( NULL == dsatutil_convert_chset( record_ptr->text, 
                                        ALPHA_PBM_UCS2,
                                        name,
                                        ALPHA_IRA,
                                        sizeof(name),
                                        TRUE)
                                      )
    {
      return dsat_send_cme_error(DSAT_CME_INVALID_CHAR_IN_TEXT);
    }

    if( (raw_rsp_ptr = dsm_new_buffer( DSM_DS_LARGE_ITEM_POOL )) == NULL)
    {
      ERR_FATAL( "Running out of DSM large items", 0, 0, 0);
    }

    raw_rsp_ptr->used = (word) std_strlprintf((char *)raw_rsp_ptr->data_ptr,
                                     raw_rsp_ptr->size,
                                     "%s: %d,\"%s\",%d,\"%s\"\n",
                                     cpbr_index.display_name,
                                     cpbr_index.current,
                                     record_ptr->number,
                                     record_ptr->type,
                                     name);

    /* send the async response to terminal */
    dsatcmdp_handle_async_cmd_rsp(raw_rsp_ptr, *rsp_ptr);

    /* note that we have sent out record */
    if(*rsp_ptr == DSAT_START_OF_RSP)
    {
      *rsp_ptr = DSAT_INTERMEDIATE_RSP;
    }
  }
  else 
  {
    /* the record is empty */
    /* This block only executed if read of a single entry results in
       NULL record. If multi-entry read returns a NULL record it is
       simply ignored */
    if(cpbr_index.last == cpbr_index.current &&
       *rsp_ptr == DSAT_START_OF_RSP)
    {
      /* no record is read */
      return dsat_send_cme_error(DSAT_CME_NOT_FOUND);
    }
  }

  /* Check for end of reading records */
  if(cpbr_index.last == cpbr_index.current)
  {
    result = DSAT_OK;
  }

  return result;

} /* cpb_pbm_read_handler_normal_status */

/*===========================================================================

FUNCTION CPB_PBM_FIND_HANDLER

DESCRIPTION
  This function is called by +CPBF to return the results of pbm_find
  function call.

DEPENDENCIES
  None

RETURN VALUE
  DSAT_OK          : find action succeeds
  DSAT_ASYNC_CMD   : continue searching for next number
  DSAT_CMD_ERR_RSP : if there is a CME ERROR
  DSAT_ERROR       : find action fails due to non-cme errors

SIDE EFFECTS
  None
===========================================================================*/
LOCAL dsat_result_enum_type cpb_pbm_find_handler
(
  void
)
{
  /* Remember if the the record is the beginning, middle, or end
     of the data */
  static dsat_rsp_enum_type async_rsp = DSAT_START_OF_RSP;

  dsat_result_enum_type result = DSAT_OK;
  pbm_return_type pbm_result;
  dsat_pbm_record_s_type record;
  uint16 rec_id; /* record id */
  uint16 loc_id; /* location id */

  /* Get the next record from PBM which matches the search criteria */
  while ( (pbm_result = pbm_enum_next_rec_id(&rec_id))== PBM_SUCCESS )
  {
    loc_id = pbm_record_id_to_location(rec_id);
    cpb_pbm_read_record(rec_id, &record, &pbm_result);

    /* success in reading the record */
    if (pbm_result == PBM_SUCCESS )
    {
      result = cpb_pbm_find_handler_status_success(&record,
                                                   loc_id,
                                                   &async_rsp
                                                  );

      /* Check for SIO TX Water mark high case : This is added as a
         precautionary measure.

         Some insights:
         If search takes some definite amount of time, it is very unlikely
         that we exceed AT->SIO high watermark count.
         In real use-case it is unlikely that every entry in phone 
         book matches with the serach criteria and tirgger huge data to SIO 
         for transmitting out. */
	     
      /* If additional entries remain to be read during fine, we re-enter 
         through cpb_pbm_reenter_find_handler, which inturn calls this 
         function */
#ifdef FEATURE_DSAT_FLOW_CONTROL
      if ( dsati_tx_wm_high )
      {
        /* High watermark has been hit on dsat to sio queue: 
           set up handler to re-enter PB entry reading once queue hits low
           watermark */
        dsati_to_sio_low_handler = cpb_pbm_reenter_find_handler;
        return result;
      }
#endif /* FEATURE_DSAT_FLOW_CONTROL */

      /* If DSM large item free count has gone below a threshold quantity... */
      if ( DSM_POOL_FREE_CNT(DSM_DS_LARGE_ITEM_POOL) < DSAT_FREE_ITEM_TRIGGER )
      {
        dsat_timer_s_type *t_ptr = &dsatutil_timer_table[DSAT_TIMER_FREE_ITEM];

        /* Set timer expiry handler function */
        dsati_handle_free_item_timer_expiry = cpb_pbm_reenter_find_handler;

        /* Set timer and await its expiration */
        (void) rex_set_timer( t_ptr->timer_ptr, t_ptr->duration );
        t_ptr->is_running = TRUE;
        return DSAT_ASYNC_CMD;
      }
    } /* if (pbm_result == PBM_SUCCESS ) */
  } /* while (pbm_result == PBM_SUCCESS) */

  /* Reached end of search */
  if (pbm_result == PBM_EOF)
  {
    result = cpb_pbm_find_handler_status_eof(async_rsp);
  }
  else /* All remaining error cases */
  {
    result = cpb_pbm_find_handler_status_fail(pbm_result);
  }

  if (result != DSAT_ASYNC_CMD)
  {
    /* Reset the static variable as we are done with finding entries and 
	   reporting. */
    async_rsp = DSAT_START_OF_RSP;
  }

  return result;
} /* cpb_pbm_find_handler */

/*===========================================================================

FUNCTION CPB_PBM_REENTER_FIND_HANDLER

DESCRIPTION
  Local function that is used to reenter finding PB entries once DSAT to SIO
  queue has hit low watermark.

DEPENDENCIES
  none

RETURN VALUE
  DSAT_OK          : Reached end of finding entries.
  DSAT_ASYNC_CMD   : Continue searching for next number.
  DSAT_CMD_ERR_RSP : If there is a CME ERROR.
  DSAT_ERROR       : Find action failed due to non-cme errors.

SIDE EFFECTS
  None
===========================================================================*/
/* ARGSUSED */
LOCAL dsat_result_enum_type cpb_pbm_reenter_find_handler
(
  dsat_mode_enum_type mode,
  ds_cmd_type* cmd_ptr
)
{
  dsat_result_enum_type result;

  /* Reset handler function pointers */
#ifdef FEATURE_DSAT_FLOW_CONTROL
  /* Handler function for low event, now not neccesary */
  dsati_to_sio_low_handler = NULL;
#endif /* FEATURE_DSAT_FLOW_CONTROL */
  dsati_handle_free_item_timer_expiry = NULL;

  MSG_MED("cpb_pbm_cb_reenter_find_handler: start finding again!", 0, 0, 0);

  /* Continue finding next record */
  result = cpb_pbm_find_handler();

  return result;
}/* cpb_pbm_reenter_find_handler */

/*===========================================================================

FUNCTION CPB_PBM_FIND_HANDLER_STATUS_SUCCESS

DESCRIPTION
  This function is called by cpb_pbm_find_handler when finding a record
  succeeds. This function is used to report the output to terminal.
  
DEPENDENCIES
  None

RETURN VALUE
  DSAT_ASYNC_CMD : Continue searching for next number.

SIDE EFFECTS
  None
===========================================================================*/
LOCAL dsat_result_enum_type cpb_pbm_find_handler_status_success
(
  dsat_pbm_record_s_type  * record_ptr,/* a record found */
  uint16               loc_id,         /* location id */
  dsat_rsp_enum_type * rsp_ptr         /* if the record is the beginning, 
                                       middle, or end of all the record found */
)
{
  dsm_item_type * raw_rsp_ptr = NULL;
  char name[PBM_TEXT_SIZE_CHARS];
  
  if (record_ptr != NULL && record_ptr->number[0] != '\0')
  {
    memset(name, '\0', PBM_TEXT_SIZE_CHARS);
    if( NULL == dsatutil_convert_chset( record_ptr->text, 
                                       ALPHA_PBM_UCS2,
                                       name,
                                       ALPHA_IRA,
                                       sizeof(name),
                                       TRUE))
    {
      return dsat_send_cme_error(DSAT_CME_INVALID_CHAR_IN_TEXT);
    }
	
    if( (raw_rsp_ptr = dsm_new_buffer( DSM_DS_SMALL_ITEM_POOL )) == NULL)
    {
      ERR_FATAL( "Running out of DSM small items", 0, 0, 0);
    }


    raw_rsp_ptr->used=(word)std_strlprintf((char *)raw_rsp_ptr->data_ptr,
                                     raw_rsp_ptr->size,
                                     "$QCCPBF: %d,\"%s\",%d,\"%s\"\n",
                                     loc_id,
                                     record_ptr->number,
                                     record_ptr->type,
                                     name);


    /* Send the async response to terminal */
    dsatcmdp_handle_async_cmd_rsp(raw_rsp_ptr, *rsp_ptr);
    *rsp_ptr = DSAT_INTERMEDIATE_RSP;
  }

  return DSAT_ASYNC_CMD;
} /* cpb_pbm_find_handler_status_success */


/*===========================================================================

FUNCTION CPB_PBM_FIND_HANDLER_STATUS_EOF

DESCRIPTION
  This function is called by cpb_pbm_find_handler when the
  pbm_find_name_cb function's cmd status is PBM_EOF.

DEPENDENCIES
  None

RETURN VALUE
  DSAT_OK          : Search completes.
  DSAT_CMD_ERR_RSP : Found no entries.
  
SIDE EFFECTS
  None
===========================================================================*/
LOCAL dsat_result_enum_type cpb_pbm_find_handler_status_eof
(
  dsat_rsp_enum_type async_rsp
)
{
  dsat_result_enum_type result;

  /* If there is nothing in the result buffer. */
  /* Instead of output nothing, we do CME error */
  if (async_rsp == DSAT_START_OF_RSP)
  {
    result = dsat_send_cme_error(DSAT_CME_NOT_FOUND);
  }
  else
  {
    result = DSAT_OK;
  }

  return result;
} /* cpb_pbm_find_handler_status_eof */

/*===========================================================================

FUNCTION CPB_PBM_FIND_HANDLER_STATUS_FAIL

DESCRIPTION
  This function is called by cpb_pbm_find_handler when the
  pbm_find_name_cb function's cmd status indicate PBM_ERRORs.

DEPENDENCIES
  None

RETURN VALUE
  DSAT_CMD_ERR_RSP : If there is a CME ERROR
  DSAT_ERROR       : For all unexpected PBM errors.

SIDE EFFECTS
  None
===========================================================================*/
/* ARGSUSED */
LOCAL dsat_result_enum_type cpb_pbm_find_handler_status_fail
(
  pbm_return_T status
)
{
  dsat_result_enum_type result;

  switch (status)
  {
    case PBM_ERROR:
    case PBM_ERROR_NO_SIM:
    case PBM_ERROR_PH_SIM_REQ:
    case PBM_ERROR_SIM_PIN_REQ:
    case PBM_ERROR_SIM_PUC_REQ:
    case PBM_ERROR_SIM_FAIL:
    case PBM_ERROR_PIN2_REQ:
    case PBM_ERROR_PUC2_REQ:
    case PBM_ERROR_NOT_FOUND:
    case PBM_ERROR_TEXT_TOO_LONG:
    case PBM_ERROR_INVALID_CHAR:
      /* mobile equipment error encountered */
      cpb_process_pbm_error(status, NULL);
      result = DSAT_CMD_ERR_RSP;
      break;
          
    case PBM_ERROR_INDEX_INVALID:
    case PBM_ERROR_MEM_FULL:
    case PBM_ERROR_NUMBER_TOO_LONG:
    case PBM_ERROR_NOT_READY:
    case PBM_ERROR_NOT_AVAILABLE:
      /* pbm should not return this result at this time */
      ERR("Wrong pbm_result: %d", status, 0, 0);
      result = DSAT_ERROR;
      break;
        
    default:
      /* appease lint */
      result = DSAT_ERROR;
      ERR("Wrong pbm_result: %d", status, 0, 0);
  }

  return result;
} /* cpb_pbm_find_handler_status_fail */

/*===========================================================================

FUNCTION CPB_PBM_CB_READY_HANDLER

DESCRIPTION
  Process CPB_PBM_CB_READY pbm cb cmd.

  It set dsatme_pbm_ready according to the cmd status.
  If the status is TRUE, it calls pbm_file_info to get the ph book
  info of the default storage type.

DEPENDENCIES
  None

RETURN VALUE
  DSAT_ASYNC_EVENT : unsolicited events.

SIDE EFFECTS
  None

===========================================================================*/
/* ARGSUSED */
LOCAL dsat_result_enum_type cpb_pbm_cb_ready_handler
(
  /* ATCOP states, command, online data, or online command */
  dsat_mode_enum_type   mode,

  /* point to the ds cmd item to be handled */
  ds_cmd_type *         cmd_ptr
)
{
  /* we only support the READY event */
  if(cmd_ptr->cmd.pbm_info.status)
  {
    /* pbm is ready */
    dsatme_pbm_ready = TRUE;
    
    /* call pbm_get_file_info to get the info about the default storage */
    cb_expected = CPB_PBM_CB_READY_FILE_INFO;
    /* error cases are also handled in the call-back */
   (void) pbm_extended_file_info(me_cpbs_entry.device,
                                 me_pbm_file_info_cb);
  }
  else
  {
    /* pbm is not ready */
    dsatme_pbm_ready = FALSE;
  }

  return DSAT_ASYNC_EVENT;
} /* cpb_pbm_cb_ready_handler */


/*===========================================================================

FUNCTION CPB_PBM_CB_READY_FILE_INFO_HANDLER

DESCRIPTION
  This function processes the file_info returned from pbm just after
  pbm gets ready.

  If the pbm_file_info command succeeds, update the ph book info of
  the default ph number storage type.

DEPENDENCIES
  None

RETURN VALUE
  DSAT_ASYNC_EVENT

SIDE EFFECTS
  None

===========================================================================*/
/* ARGSUSED */
LOCAL dsat_result_enum_type cpb_pbm_cb_ready_file_info_handler
(
  /* ATCOP states, command, online data, or online command */
  dsat_mode_enum_type   mode,

  /* point to the ds cmd item to be handled */
  ds_cmd_type *         cmd_ptr
)
{
  /* command pbm_file_info status */
  pbm_return_type   status = cmd_ptr->cmd.pbm_info.status;
  
  /* file info */
  ds_at_pbm_file_info_type * info_ptr = 
    &cmd_ptr->cmd.pbm_info.cmd_info.file_info;

  /*-------- the command succeeds --------*/
  if(status == PBM_SUCCESS)
  {
    /* save the file info to the default storage */
    ASSERT(me_cpbs_entry.device == info_ptr->pbm_device);
    me_cpbs_entry.total = (int16) info_ptr->total;
    me_cpbs_entry.text_len = (int16) info_ptr->text_len;
  } /* if(status == PBM_SUCCESS) */
  /* the command fails. the storage did not get set correctly */
  else
  {
    ERR("pbm_file_info cb with status: %d",status,0,0);
  }

  return DSAT_ASYNC_EVENT;
} /* cpb_pbm_cb_ready_file_info_handler */


/*===========================================================================

FUNCTION CPB_PBM_CB_FILE_INFO_HANDLER

DESCRIPTION
  This function processes the file_info returned from pbm.

  If the pbm_file_info command succeeds, update the content of 
  val_ptr (dsat_cpbs_val) of cpbs command and save the storage info in 
  current cpbs entry, me_cpbs_entry.

DEPENDENCIES
  None

RETURN VALUE
  

SIDE EFFECTS
  None

===========================================================================*/
/* ARGSUSED */
LOCAL dsat_result_enum_type cpb_pbm_cb_file_info_handler
(
  /* ATCOP states, command, online data, or online command */
  dsat_mode_enum_type   mode,

  /* point to the ds cmd item to be handled */
  ds_cmd_type *         cmd_ptr
)

{
  dsat_result_enum_type result;

  /* command pbm_file_info status */
  pbm_return_type    status = cmd_ptr->cmd.pbm_info.status;
  
  /* file info */
  ds_at_pbm_file_info_type * info_ptr = 
    &cmd_ptr->cmd.pbm_info.cmd_info.file_info;

  /*-------- the command succeeds --------*/
  if(status == PBM_SUCCESS)
  {
    int index;
    byte storage[CPBS_VAL_LEN + 1];
    boolean find_index = FALSE;
    
    /* get the AT storage by pbm_device type */
    if(get_pb_storage(info_ptr->pbm_device, storage))
    {
      /* update the cpbs parameter */
      for (index = 0; dsat_cpbs_storage_valstr[index][0] != '\0'; index++)
      {
        if(!strcmp( (char *) dsat_cpbs_storage_valstr[index], 
                    (char *) storage))
        {
          find_index = TRUE;
          break;
        }
      }

      if (find_index)
      {
        if( (info_ptr->pbm_device == PBM_DS_ME) &&
            (info_ptr->total == 0) )
        {
          ERR("pbm_device %d not supported, returned total rec size=0",info_ptr->pbm_device,0,0);
          result = DSAT_ERROR;
          cb_expected = CB_NONE;
          return result;
        }

        dsat_cpbs_val = (unsigned int) index;

        /* update the current storage info for +cpbs, +cpbr, +cpbf, and +cpbw */
        me_cpbs_entry.device = info_ptr->pbm_device;
        (void) std_strlcpy((char *)me_cpbs_entry.storage,
                           (char *)storage,
                           sizeof(me_cpbs_entry.storage));
        
        me_cpbs_entry.total = (int16) info_ptr->total;
        me_cpbs_entry.text_len = (int16) info_ptr->text_len;

        /* if the +cpbs command is read command, output the storage info */
        if (me_cpbs_params.is_read)
        {
          cpb_pbm_cb_file_info_handler_output_info(info_ptr->used);
        }
        result = DSAT_OK;
      }
      else
      {
        /* could not find a storage in supported at storage list for
           the returned pbm_device                                      */
        MSG_ERROR("pbm_device %d not supported",info_ptr->pbm_device,0,0);
        result = DSAT_ERROR;
      }
    }
    else
    {
      /* could not map the pbm_device to a AT storage */
      ERR("No storage for pbm_device %d",info_ptr->pbm_device,0,0);
      result = DSAT_ERROR;
    }

  } /* if(status == PBM_SUCCESS) */

  /* the command fails. the storage did not get set correctly */
  else 
  {
    /* return error */
    result = cpb_pbm_cb_file_info_handler_error_status(status);
  }
  cb_expected = CB_NONE;
  return result;
} /* cpb_pbm_cb_file_info_handler */


/*===========================================================================

FUNCTION CPB_PBM_CB_FILE_INFO_HANDLER_OUTPUT_INFO

DESCRIPTION
  This function is called by me_pbm_file_info_cb to output the 
  storage info if the +cpbs command is read command.

  This function makes me_pbm_file_info_cb shorter.

DEPENDENCIES
  None

RETURN VALUE
  None

SIDE EFFECTS
  None

===========================================================================*/
/* ARGSUSED */
LOCAL void cpb_pbm_cb_file_info_handler_output_info(int used)
{
  dsm_item_type * raw_rsp_ptr;
  char * buff_ptr;

  if( (raw_rsp_ptr = dsm_new_buffer( DSM_DS_LARGE_ITEM_POOL )) == 
      NULL)
  {
    ERR( "Running out of items", 0, 0, 0);
    return;
  }

  buff_ptr = (char *) raw_rsp_ptr->data_ptr;
        
  /* if current storage's used or total value does not make sense,
     don't output them */
  if (used < 0 || me_cpbs_entry.total < 1) 
  {
    /* When the total and used is not supported, they should be -1 */
    ASSERT(me_cpbs_entry.total == -1);

    raw_rsp_ptr->used = (word) std_strlprintf(buff_ptr,
                                              raw_rsp_ptr->size,
                                              "%s: \"%s\"",
                                              me_cpbs_params.display_name,
                                              me_cpbs_entry.storage);
  }
  else
  {
    raw_rsp_ptr->used = (word) std_strlprintf(buff_ptr,
                                       raw_rsp_ptr->size,
                                       "%s: \"%s\",%d,%d", 
                                       me_cpbs_params.display_name,
                                       me_cpbs_entry.storage,
                                       used,
                                       me_cpbs_entry.total);
  }
        
  dsatcmdp_handle_async_cmd_rsp(raw_rsp_ptr, DSAT_COMPLETE_RSP);
} /* cpb_pbm_cb_file_info_handler_output_info */


/*===========================================================================

FUNCTION CPB_PBM_FILE_INFO_HANDLER_ERROR_STATUS

DESCRIPTION
  This function is called by me_pbm_file_info_cb to process error status.

DEPENDENCIES
  None

RETURN VALUE
  DSAT_ERROR:           non ME pbm error encountered
  DSAT_CMD_ERR_RSP:     a ME pbm error is reported to TE

SIDE EFFECTS
  None

===========================================================================*/
/* ARGSUSED */
dsat_result_enum_type cpb_pbm_cb_file_info_handler_error_status
(
  pbm_return_type  status
)
{
  dsat_result_enum_type result;

  /* handles the error code */
  switch (status)
  {
    /* mobile equipment error encountered */

    case PBM_ERROR_PIN2_REQ:
    case PBM_ERROR_PUC2_REQ:
      /* If attempted verification of pin2 failed due to incorrect pin
         show this result in next +CPIN command response */
      dsat_mask_pin2_state = FALSE;
      /* -fallthrough */
    case PBM_ERROR_PH_SIM_REQ:
    case PBM_ERROR_SIM_PIN_REQ:
    case PBM_ERROR_SIM_PUC_REQ:
    case PBM_ERROR:
    case PBM_ERROR_NO_SIM:
    case PBM_ERROR_SIM_FAIL:
      /* translate pbm error into cme error and 
         output it to TE */
      cpb_process_pbm_error(status, NULL);
      result = DSAT_CMD_ERR_RSP;
      break;
          
    case PBM_EOF:
    case PBM_ERROR_INDEX_INVALID:
    case PBM_ERROR_MEM_FULL:
    case PBM_ERROR_NOT_FOUND:
    case PBM_ERROR_TEXT_TOO_LONG:
    case PBM_ERROR_NUMBER_TOO_LONG:
    case PBM_ERROR_INVALID_CHAR:
    case PBM_ERROR_NOT_READY:
    case PBM_ERROR_NOT_AVAILABLE:
      /* pbm should not return this result at this time */
      MSG_ERROR("Wrong pbm_result: %d", status, 0, 0);
      result = DSAT_ERROR;
      break;
        
    default:
      /* appease lint */
      result = DSAT_ERROR;
      MSG_ERROR("Wrong pbm_result: %d", status, 0, 0);
  }
  
  return result;
} /* cpb_pbm_cb_file_info_handler_error_status */


/*===========================================================================

FUNCTION DSATETSIME_CPB_PBM_CB_READ_HANDLER_ERROR_STATUS

DESCRIPTION
  This function is to process error status for pbm_record_read.

DEPENDENCIES
  None

RETURN VALUE
  DSAT_ERROR:           non ME pbm error encountered
  DSAT_CMD_ERR_RSP:     a ME pbm error is reported to TE

SIDE EFFECTS
  None
===========================================================================*/
dsat_result_enum_type cpb_pbm_read_handler_error_status
(
  pbm_return_type status          /*  command pbm_read status  */
)
{
  dsat_result_enum_type result;
  
  /* handles the error code */
  switch (status)
  {
    /* mobile equipment error encountered */
    case PBM_ERROR:
    case PBM_ERROR_NO_SIM:
    case PBM_ERROR_PH_SIM_REQ:
    case PBM_ERROR_SIM_PIN_REQ:
    case PBM_ERROR_SIM_PUC_REQ:
    case PBM_ERROR_SIM_FAIL:
    case PBM_ERROR_PIN2_REQ:
    case PBM_ERROR_PUC2_REQ:
    case PBM_ERROR_INDEX_INVALID:
      cpb_process_pbm_error(status, NULL);
      result = DSAT_CMD_ERR_RSP;
      break;
          
    case PBM_EOF:
    case PBM_ERROR_MEM_FULL:
    case PBM_ERROR_TEXT_TOO_LONG:
    case PBM_ERROR_NUMBER_TOO_LONG:
    case PBM_ERROR_INVALID_CHAR:
    case PBM_ERROR_NOT_READY:
    case PBM_ERROR_NOT_AVAILABLE:
      /* pbm should not return this result at this time */
      MSG_ERROR("Wrong pbm_result: %d", status, 0, 0);
      result = DSAT_ERROR;
      break;
        
    default:
      /* appease lint */
      result = DSAT_ERROR;
      MSG_ERROR("Wrong pbm_result: %d", status, 0, 0);
  }

  return result;
  
} /* cpb_pbm_read_handler_error_status */


/*===========================================================================

FUNCTION CPB_PBM_CB_WRITE_HANDLER

DESCRIPTION
  This function is called by pbm to return the results of pbm_write
  function call.

  This function is passed to pbm with pbm_write function call.

DEPENDENCIES
  None

RETURN VALUE
  DSAT_OK          : if write action succeeds
  DSAT_CMD_ERR_RSP : if there is a CME ERROR
  DSAT_ERROR       : if write action fails

SIDE EFFECTS
  None
===========================================================================*/
/* ARGSUSED */
LOCAL dsat_result_enum_type cpb_pbm_cb_write_handler
(
  /* ATCOP states, command, online data, or online command */
  dsat_mode_enum_type   mode,

  /* point to the ds cmd item to be handled */
  ds_cmd_type *         cmd_ptr
)
{
  dsat_result_enum_type result;

  /* command pbm_write status */
  pbm_return_type      status = cmd_ptr->cmd.pbm_info.status;

  /*-------- process the pbm_write cmd status --------*/
  if(status == PBM_SUCCESS)
  {
    result = DSAT_OK;
  }
  else
  {
    /* handles the error code */
    switch (status)
    {
      case PBM_ERROR:
      case PBM_ERROR_NO_SIM:
      case PBM_ERROR_PH_SIM_REQ:
      case PBM_ERROR_SIM_PIN_REQ:
      case PBM_ERROR_SIM_PUC_REQ:
      case PBM_ERROR_SIM_FAIL:
      case PBM_ERROR_PIN2_REQ:
      case PBM_ERROR_PUC2_REQ:
      case PBM_ERROR_TEXT_TOO_LONG:
      case PBM_ERROR_INVALID_CHAR:
      case PBM_ERROR_NUMBER_TOO_LONG:
      case PBM_ERROR_MEM_FULL:
      case PBM_ERROR_INDEX_INVALID:
        /* mobile equipment error encountered */
        cpb_process_pbm_error(status, NULL);
        result = DSAT_CMD_ERR_RSP;
        break;
          
      case PBM_ERROR_NOT_FOUND:
      case PBM_ERROR_NOT_READY:
      case PBM_ERROR_NOT_AVAILABLE:
        /* pbm should not return this result at this time */
        ERR("Wrong pbm_result: %d", status, 0, 0);
        result = DSAT_ERROR;
        break;
          
      default:
        /* appease lint */
       result = DSAT_ERROR;
	   cb_expected = CB_NONE;
       ERR("Wrong pbm_result: %d", status, 0, 0);
    }
  }

  return result;
} /* cpb_pbm_cb_write_handler */


/*===========================================================================

FUNCTION DD_PBM_FILE_INFO_HANDLER

DESCRIPTION
  The handler function for DD_PBM_FILE_INFO.  The pbm_file_info call 
  back function returned the info of the querried ph book type.

  This function process the info of the ph book type.

  If dd_val.index is in the correct range of the index, get the 
  number from the ph book.
  If not, we treat the D> string as D>STR case.

DEPENDENCIES
  None

RETURN VALUE
  DSAT_ASYNC_CMD   : seaching a number in ph book asynchrously
  DSAT_ERROR       : fail to get a number in ph book
  DSAT_CMD_ERR_RSP : fail to get a number in ph book due to a CME ERROR

SIDE EFFECTS
  None
  
===========================================================================*/
/* ARGSUSED */
LOCAL dsat_result_enum_type dd_pbm_file_info_handler
(
  /* ATCOP states, command, online data, or online command */
  dsat_mode_enum_type   mode,

  /* point to the ds cmd item to be handled */
  ds_cmd_type *         cmd_ptr
)
{
  /* pbm_file_info call status                                          */
  pbm_return_type          status;
  /* number of total entries of dd.mem                                  */
  int                           total;

  dsat_result_enum_type         result;

  /*-------- get pbm_file_info call status --------*/
  status = cmd_ptr->cmd.pbm_info.status;

  /* the command succeeds */
  if(status == PBM_SUCCESS)
  {
    /* we gets the info about dd_val.mem, it can't be NULL */
    ASSERT(dd_val.mem[0] != '\0');

    /* See what the number of total entries is.  The valid index is
     from 1 to it.                                                      */
    total = cmd_ptr->cmd.pbm_info.cmd_info.file_info.total;

    if(dd_val.index > 0 && dd_val.index <= total)
    {
      /* the index is valid */
      result = me_dd_process_n(dd_val.mem, dd_val.index, NULL);
    }
    else
    {
      /* the index is not valid, treat the D> string as search string */
      result = me_dd_process_str(dd_val.str, NULL);
    }
  }
  else
  {
    result = cpb_pbm_cb_file_info_handler_error_status(status);
  }

  return result;
} /* dd_pbm_file_info_handler */


/*===========================================================================

FUNCTION DD_PBM_FIND_HANDLER

DESCRIPTION
  If we find an exact match, stop searching and dial the number out.
  Otherwise, continue the search.

DEPENDENCIES
  None

RETURN VALUE
  DSAT_NO_RSP           We find the number, and dial it out.
  DSAT_ERROR            There is an error.
  DSAT_CMD_ERR_RSP      There is cme error.
  DSAT_OK               The search went fine.
  DSAT_ASYNC_CMD        The number is dialed out with an async cmd.

SIDE EFFECTS
  None
  
===========================================================================*/
/* ARGSUSED */
LOCAL dsat_result_enum_type dd_pbm_find_handler
(
  const byte * srh_str, 
  pbm_device_type pbm_device
)
{

  dsat_result_enum_type result = DSAT_OK;

  /* pbm_find_name_next call return     */
  pbm_return_type      pbm_result;

  /* the found entry                    */
  dsat_pbm_record_s_type      record;
  uint16 rec_id;
  uint16 converted_len;

  converted_len = (uint16)((pbmutils_wstrlen((uint16*)srh_str) + 1) * sizeof(uint16));
  if(converted_len > PBM_TEXT_SIZE_BYTES)
  {
    MSG_ERROR("Invalid length",0,0,0);
    return DSAT_ERROR;
  }
  pbm_result = pbm_enum_rec_init(pbm_device,
                                 (int)PBM_CAT_NONE,
                                 PBM_FIELD_NAME,
                                 (uint8 *)srh_str,
                                 converted_len,
                                 PBM_SEARCHTYPE_STARTSWITH | PBM_STRTYPE_UCS2);
  if (pbm_result == PBM_SUCCESS )
  {
    while ((pbm_result = pbm_enum_next_rec_id(&rec_id))== PBM_SUCCESS )
    {
      cpb_pbm_read_record(rec_id, &record, &pbm_result);
      if (pbm_result == PBM_SUCCESS )
      {
        if(dd_val.record.number[0] == '\0')
        {
          dd_val.record = record;
        }
        if(!memcmp((void *)srh_str, (void *)record.text,converted_len))
        {
          /* we found a exact match, dial the number out */
          dd_val.record = record;
          dd_val.dial_str.num = (byte *) dd_val.record.number;
          dd_val.dial_str.len = (uint16) strlen((char *) dd_val.dial_str.num);
#if defined(FEATURE_WCDMA) || defined(FEATURE_GSM)
#error code not present
#endif
        }
      }
    }
  }
  return result;
} /* dd_pbm_find_handler */

/*--------                pbm call back functions                --------*/

/*===========================================================================

FUNCTION CPB_PBM_STATUS_CB

DESCRIPTION
  Pbm status report call back function.  Pbm calls this function
  to inform its status, ready or not ready.

  DEPENDENCIES
  PBM is checking only for the availability of ADN phone books
  and the status is sent solely based on that. So, neglecting
  status sent by PBM. From here on for Phone book access commands,
  the status sent by the PBM at the time of invocation is final. 

RETURN VALUE
  None
    
SIDE EFFECTS
  None

===========================================================================*/
LOCAL void cpb_pbm_status_cb(boolean status)
{
  me_pbm_cb_submit_ds_cmd(CPB_PBM_CB_READY, (pbm_return_type) TRUE,
                          NULL, NULL);
} /* cpb_pbm_status_cb */


/*===========================================================================

FUNCTION ME_PBM_FILE_INFO_CB

DESCRIPTION
  This function is called by pbm to return the results of pbm_file_info
  function call.

  This function is passed to pbm with pbm_file_info function call.

  If the pbm_file_info command succeeds, update the content of 
  val_ptr (dsat_cpbs_val) of cpbs command and save the storage info in 
  current cpbs entry, me_cpbs_entry.

DEPENDENCIES
  None

RETURN VALUE
  None

SIDE EFFECTS
  None

===========================================================================*/
LOCAL void me_pbm_file_info_cb
(
  pbm_return_type    status,           /* command pbm_file_info status    */
  pbm_device_type pbm_device,       /* the storage type be selected    */
  pbm_extended_fileinfo_s_type * pbm_extended_fileinfo /*Response*/
)
{
  ds_at_pbm_file_info_type  file_info;
  ASSERT(NULL != pbm_extended_fileinfo);

  DEBUG(MSG_HIGH("pbm_device %d, used %d, total, %d",
                 pbm_device, pbm_extended_fileinfo->records_used,
                 pbm_extended_fileinfo->number_of_records); );

  file_info.pbm_device = pbm_device;
  file_info.used = pbm_extended_fileinfo->records_used;
  file_info.total = pbm_extended_fileinfo->number_of_records;
  file_info.text_len = pbm_extended_fileinfo->text_len;
  me_pbm_cb_submit_ds_cmd(cb_expected, status, &file_info, NULL);
} /* me_pbm_file_info_cb */


/*===========================================================================

FUNCTION ME_PBM_WRITE_CB

DESCRIPTION
  This function is called by pbm to return the results of pbm_record_write
  function call.

  This function is passed to pbm with pbm_record_write function call.

DEPENDENCIES
  None

RETURN VALUE
  None

SIDE EFFECTS
  None
===========================================================================*/
/* ARGSUSED */
LOCAL void me_pbm_write_cb
(
  pbm_writecb_data_s_type *cb_data   /* pbm_record_write callback structure */
)
{
  if(cb_data == NULL)
  {
    MSG_ERROR("cb_data is NULL", 0, 0, 0);
    return;
  }
  DEBUG(MSG_HIGH("me_pbm_write_cb status %d",cb_data->ret,0, 0); );
  
  me_pbm_cb_submit_ds_cmd(cb_expected, cb_data->ret, NULL, NULL);
} /* me_pbm_write_cb */
/*===========================================================================

FUNCTION ME_LD_PBM_WRITE_CB

DESCRIPTION
  This function is called by pbm to return the results of pbm_record_write
  function call for last dial.

  This function is passed to pbm with pbm_record_write function call.

DEPENDENCIES
  None

RETURN VALUE
  None

SIDE EFFECTS
  None
===========================================================================*/
/* ARGSUSED */
LOCAL void me_ld_pbm_write_cb
(
  pbm_writecb_data_s_type *cb_data   /* pbm_record_write callback structure */
)
{
  if(cb_data == NULL)
  {
    MSG_ERROR("cb_data is NULL", 0, 0, 0);
    return;
  }
  DEBUG(MSG_HIGH("me_ld_pbm_write_cb status %d",cb_data->ret,0, 0); );
} /* me_ld_pbm_write_cb */

/*-------- pbm function call and cb cmd status  processing function --------*/

/*===========================================================================

FUNCTION PROCESS_PBM_RECORD_WRITE_RETURN

DESCRIPTION
  Process the function return status of pbm_write call.

DEPENDENCIES
  None

RETURN VALUE
  DSAT_ASYNC_CMD   : async call pbm_write succeeds
  DSAT_CMD_ERR_RSP : if there is a CME ERROR
  DSAT_ERROR       : if pbm_write call fails

SIDE EFFECTS
  None
  
===========================================================================*/
LOCAL dsat_result_enum_type process_pbm_record_write_return
(
  /* return from pbm_find_location */
  pbm_return_type pbm_result,

  /* reporting error if there is any */
  dsm_item_type * err_buff_ptr
)
{
  dsat_result_enum_type result;

  switch (pbm_result) 
  {
    case PBM_SUCCESS:
      /* pbm_write is an async command */
      result = DSAT_ASYNC_CMD;
      break;
    case PBM_ERROR_NOT_FOUND:
      /* the only time "not found" returned on write attempt is when */ 
      /* trying to delete an already empty index. */ 
      result = DSAT_OK;
      break;
    case PBM_ERROR:
    case PBM_ERROR_NO_SIM:
    case PBM_ERROR_PH_SIM_REQ:
    case PBM_ERROR_INDEX_INVALID:
    case PBM_ERROR_SIM_FAIL:
    case PBM_ERROR_TEXT_TOO_LONG:
    case PBM_ERROR_NUMBER_TOO_LONG:
    case PBM_ERROR_INVALID_CHAR:
    case PBM_ERROR_MEM_FULL:
      /* mobile equipment error encountered */
      cpb_process_pbm_error(pbm_result, err_buff_ptr);
      result = DSAT_CMD_ERR_RSP;
      cb_expected = CB_NONE;
      break;
    case PBM_EOF:
    case PBM_ERROR_SIM_PIN_REQ:
    case PBM_ERROR_SIM_PUC_REQ:
    case PBM_ERROR_PIN2_REQ:
    case PBM_ERROR_PUC2_REQ:
    case PBM_ERROR_NOT_READY:
    case PBM_ERROR_NOT_AVAILABLE:
    case PBM_ERROR_RESTRICTED:
    case PBM_ERROR_INVALID_REC:
      /* pbm should not return this result at this time */
      ERR("Wrong pbm_result: %d", pbm_result, 0, 0);
      result = DSAT_ERROR;
      cb_expected = CB_NONE;
      break;
    default:
      /* appease lint */
      result = DSAT_ERROR;
      cb_expected = CB_NONE;
      ERR("Wrong pbm_result %d", pbm_result, 0, 0);
  }
  return result;
} /* process_pbm_record_write_return */
/*--------      Other ph book local function           --------*/
/* utility functions */

/*===========================================================================

FUNCTION GET_PBM_DEVICE

DESCRIPTION
  Map an ATCOP ph book storage type (2 char string) to pbm device type
  (an enum).

DEPENDENCIES
  None

RETURN VALUE
  TRUE : Successful.
  FALSE: Fail, did not find a pbm device type for the storage type.

SIDE EFFECTS
  None

===========================================================================*/
LOCAL boolean get_pbm_device
(
  /* Input, an ATCOP ph book storage type (2 char string, such as "SM" )*/
  const byte            * pb,

  /* Output, pbm device type (an enum, such as PBM_DS_SM) */
  pbm_device_type       * dev_ptr
)
{
  int index;
  
  if(!dev_ptr)
  {
    ERR("NULL dev_ptr pointer",0,0,0);
    return FALSE;
  }

#ifdef FEATURE_MMGSDI
  /* If looking for last dialed PB device we must determine current card
     SIM/USIM, which could change depending on $QCSLOT setting */
  if( !dsatutil_strcmp_ig_sp_case(pb, (const byte *)LAST_DIALED_STR) )
  {
    /* For GSDI query of card */  
    gsdi_card_apps_T apps_available;
    byte apps_enabled;

    /* Make Call to GSDI to retrieve current 
    ** APPs on the Card(s): GSM DF / USIM ADF */
    gsdi_get_apps_available(&apps_available);
    apps_enabled = apps_available.apps_enabled;

    if (( (apps_enabled & GSDI_GSM_SIM_MASK) == GSDI_GSM_SIM_MASK )||
        ((apps_enabled & GSDI_RUIM_APP_MASK) == GSDI_RUIM_APP_MASK ))
    {
      *dev_ptr = PBM_LND;
      return TRUE;
    }
    else if ( (apps_enabled & GSDI_USIM_APP_MASK) == GSDI_USIM_APP_MASK) 
    {
      *dev_ptr = PBM_OCI;
      return TRUE;
    }
    else
    {
      *dev_ptr = PBM_LND;
      return TRUE;
    }
  }
  else
#endif /* FEATURE_MMGSDI */
  {
    /*-------- search pbm device for pb in static pbm_table --------*/
    for (index = 0; index < pbm_table_size; index++)
    {
      if(!dsatutil_strcmp_ig_sp_case(pb, pbm_table[index].storage))
      {
        *dev_ptr = pbm_table[index].device;
        return TRUE;
      }
    }
  }

  return FALSE;
} /* get_pbm_device */


/*===========================================================================

FUNCTION GET_PB_STORAGE

DESCRIPTION
  Map a pbm device type (an enum) to an ATCOP ph book storage 
  type (2 char string).

DEPENDENCIES
  None

RETURN VALUE
  TRUE : Successful.
  FALSE: Fail, did not find a storage for the pbm device type.

SIDE EFFECTS
  None

===========================================================================*/
LOCAL boolean get_pb_storage
(
  /* Input, pbm device type (an enum, such as PBM_DS_SM) */
  pbm_device_type       device,

  /* Output, an ATCOP ph book storage type (2 char string, such as "SM" ) */
  byte                  * storage_ptr
)
{
  int index;

  if(!storage_ptr)
  {
    ERR("NULL pb pointer",0,0,0);
    return FALSE;
  }

  {
    /* If looking for a device that would map to "LD" storage we must 
       determine current card SIM/USIM, which could change depending on 
       $QCSLOT setting */
#ifdef FEATURE_MMGSDI
    if( device == PBM_LND ||
        device == PBM_OCI )
    {
      gsdi_card_apps_T apps_available;
      byte apps_enabled;

      /* Make Call to GSDI to retrieve current 
      ** APPs on the Card(s): GSM DF / USIM ADF */
      gsdi_get_apps_available(&apps_available);
      apps_enabled = apps_available.apps_enabled;

      /* If last dialed device type for which querying agrees with card type
         or if NV last dialed device type return "LD" storage else return 
         false. */
      if ( ( (apps_enabled & GSDI_GSM_SIM_MASK) == GSDI_GSM_SIM_MASK &&
             device == PBM_LND ) ||
           ( (apps_enabled & GSDI_RUIM_APP_MASK) == GSDI_RUIM_APP_MASK &&
             device == PBM_LND ) ||
           ( (apps_enabled & GSDI_USIM_APP_MASK) == GSDI_USIM_APP_MASK &&
             device == PBM_OCI ) )
      {
        (void)std_strlcpy( (char *)storage_ptr, LAST_DIALED_STR,
                           sizeof(storage_ptr));
        return TRUE;
      }
      else
      {
        return FALSE;
      }
    }
    /*-------- else search pbm device for pb in pbm_table --------*/ 
    else
#endif /* FEATURE_MMGSDI */
    {
      for (index = 0; index < pbm_table_size; index++)
      {
        if(pbm_table[index].device == device)
        {
          (void) std_strlcpy((char *) storage_ptr,
                             (char *) pbm_table[index].storage,
                             sizeof(storage_ptr));
          return TRUE;
        }
      }
    }
  }
  
  return FALSE;
} /* get_pb_storage */

/* cpbs local functions */

/*===========================================================================

FUNCTION CPBS_PARSE_ARGUMENTS

DESCRIPTION
  Parse the +cpbs argument into cpbs_argu_type data structure so that
  we can continue on the command after we receive the reply for pin_status,
  and pin_verification from GSDI.

DEPENDENCIES
  None

RETURN VALUE
  Returns the command running status.
    DSAT_PARAM : if the parameter is not correct
    DSAT_ERROR : if the storage type is not supported
    DSAT_OK    : if it is a success.

SIDE EFFECTS
  None

===========================================================================*/
LOCAL dsat_result_enum_type cpbs_parse_arguments
(
  const tokens_struct_type * tok_ptr,
  cpbs_argu_type           * cpbs_argu
)
{
  byte first_argu_no_quote[CPBS_VAL_LEN + 1];
  int index;
  dsat_result_enum_type result = DSAT_OK;

  /* this is a read command */
  if( tok_ptr->op == (NA | QU) )
  {
    /* dsat_cpbs_val might be reset by atz at&f command.
       If that is the case, we need reset me_cpbs_entry 
       here */

    /* use current device type */
    cpbs_argu->pbm_device = me_cpbs_entry.device;
    
    cpbs_argu->password[0] = '\0';
    cpbs_argu->is_read = TRUE;
  }
  else /* set command */
  {
    cpbs_argu->is_read = FALSE;
    if(tok_ptr->args_found == 0)
    {
      /* the AT cmd has no argument, use the default entry in the 
         dsat_cpbs_list */
      const dsat_string_item_type * default_storage;
      default_storage = dsat_cpbs_storage_valstr[dsat_cpbs_list.default_v];
      if(dsatme_nv_only == TRUE)
      {
        default_storage = dsat_cpbs_storage_valstr[5];
      }
      if (!get_pbm_device(default_storage, &cpbs_argu->pbm_device))
      {
        /* can't find a pbm device for the default storage type */
        ERR("No pbm_device for %c%c",default_storage[0],default_storage[1],0);
        result = DSAT_ERROR;
      }
    }
    else
    {
      /* strip out of the quotes from the first argument */
      if (!dsatutil_strip_quotes_out(tok_ptr->arg[0], first_argu_no_quote, 
                                     CPBS_VAL_LEN + 1) )
      {
        result = DSAT_PARAM;
      }
      else
      {
        /* determine the pbm device type from the first argument */
        if(!get_pbm_device(first_argu_no_quote, &cpbs_argu->pbm_device))
        {
          /* the AT storage (first argument) can't be mapped to a pbm_device
             the first argument is not correct */
          result = DSAT_ERROR;
        }
      }
    }

    /* check if the tokens contains password
     the password is used to unlock the pin2 lock */
    if(result == DSAT_OK && tok_ptr->args_found >= 2)
    {
      int pin2_len = (int) strlen((char *) tok_ptr->arg[1]);

      /* the pin2 is 4 to 8 long +2 \"s 
         the first char is \", last char is \" */
      if( pin2_len < PIN_MIN_LEN + 2 || pin2_len > PIN_MAX_LEN + 2 ||
          tok_ptr->arg[1][0] != '"' ||
          tok_ptr->arg[1][pin2_len - 1] != '"')
      {
        MSG_MED("The cpbs password parameter (%s) is wrong",tok_ptr->arg[1],0,0);
        result = DSAT_PARAM;
      }
      else /* The code passes the argument check */
      {
        dsatutil_reset_password(cpbs_argu->password, sizeof(cpbs_argu->password));
      
        /* get the string between \" and \" of the second argument*/
        for (index = 0; index < pin2_len - 2; index++)
        {
          cpbs_argu->password[index] = tok_ptr->arg[1][index+1];
        }
      }
    } /* if(tok_ptr->args_found >= 2) */
  }

  return result;
} /* cpbs_parse_arguments */


/*===========================================================================

FUNCTION cpbs_process_test_cmd

DESCRIPTION
  This function process the test command for +CPBS. This function displays 
  the phone books list supported by the SIM and ME.

DEPENDENCIES
  None

RETURN VALUE
  returns an enum that describes the result of the command execution.
  possible values:
    DSAT_ERROR :    if there was any problem in executing the command
    DSAT_OK :       if it is a success.

SIDE EFFECTS
  None

===========================================================================*/
LOCAL dsat_result_enum_type cpbs_process_test_cmd
(
  const dsati_cmd_type *parse_table,    /*  Ptr to cmd in parse table   */
  dsm_item_type *res_buff_ptr           /*  Place to put response       */
)
{
  pbm_return_type pbm_return;
  pbm_device_type pbm_dev;
  unsigned short index=0, index2=0;
  boolean first;
  boolean find_index = FALSE;
  
  (void)std_strlcpy(me_cpbs_params.display_name,
                    ((char *)(parse_table->name)),
                    MAX_PBM_AT_NAME_LEN);

  res_buff_ptr->used = (word) std_strlprintf((char *)res_buff_ptr->data_ptr,
                                             res_buff_ptr->size,
                                             "%s: (",
                                             me_cpbs_params.display_name);

  first = TRUE;
 /* update the cpbs parameter */
  for (index = 0; index < CPBS_SUPPORTED_PHBKS_NUM; index++)
  {
    for (index2 = 0; index2 < OPTIONAL_PHBKS_NUM; index2++)
    {
      if(!strcmp( (char *) dsat_cpbs_storage_valstr[index], 
                  (char *) opt_phbk_storage_valstr[index2]))
      {
        find_index = TRUE;
        break;
      }
    }

    if(find_index && (index2 < OPTIONAL_PHBKS_NUM))
    {
      if (!get_pbm_device(opt_phbk_storage_valstr[index2], &pbm_dev))
      {
        /* can't find a pbm device for the default storage type */
        MSG_ERROR( "No pbm_device mapped", 0, 0, 0);
        return DSAT_ERROR;
      }
      pbm_return = pbm_find_location(pbm_dev,
                                     1, 
                                     NULL, NULL);
      MSG_MED("pbm returns %d for pbm_dev %d",
                  pbm_return,pbm_dev,0);
      if(( pbm_return == PBM_SUCCESS )||
         ( pbm_return == PBM_ERROR_NOT_FOUND ))
      {
        res_buff_ptr->used += (word) std_strlprintf(
                         (char *)res_buff_ptr->data_ptr + res_buff_ptr->used,
                         res_buff_ptr->size - res_buff_ptr->used,
                         first?"\"%s\"":",\"%s\"",
                         dsat_cpbs_storage_valstr[index]);
        first = FALSE;
      }
      else if ((pbm_return == PBM_ERROR_NOT_AVAILABLE) ||
               (pbm_return == PBM_ERROR_NOT_READY) ||
               (pbm_return == PBM_ERROR_NO_SIM))
      {
        /* skip the corresponding phone book from the display of 
           supported phone books list */
      }
      else
      {
        return DSAT_ERROR;
      }
    }
    else
    {
      res_buff_ptr->used += (word) std_strlprintf(
                           (char *)res_buff_ptr->data_ptr + res_buff_ptr->used,
                           res_buff_ptr->size - res_buff_ptr->used,
                           first?"\"%s\"":",\"%s\"",
                           dsat_cpbs_storage_valstr[index]);
       first = FALSE;
    }

    find_index = FALSE;

  }

  find_index = FALSE;


  res_buff_ptr->used += (word) std_strlprintf((char *)
                        res_buff_ptr->data_ptr + res_buff_ptr->used,
                        res_buff_ptr->size - res_buff_ptr->used,
                        ")");
  return DSAT_OK;

} /* cpbs_process_test_cmd */

/*===========================================================================

FUNCTION CPBR_PARSE_ARGUMENTS

DESCRIPTION
  Parse the +cpbr argument into index1_ptr and index2_ptr.
  Report error to res_buff_ptr if there is any.

DEPENDENCIES
  None

RETURN VALUE
  Returns the command running status.
    DSAT_ERROR       : if the parameter is not correct
    DSAT_CMD_ERR_RSP : if there is a cme error
    DSAT_OK          : if it is a success.

SIDE EFFECTS
  None

===========================================================================*/
dsat_result_enum_type cpbr_parse_arguments
(
  const tokens_struct_type * tok_ptr,
  uint16                      * index1_ptr,
  uint16                      * index2_ptr,
  dsm_item_type            * res_buff_ptr  /* for reporting error */
)
{
  dsat_result_enum_type result = DSAT_OK;
  uint16 arg1, arg2;

    /* get the indexes to read the ph book from index1 to index2 */
  if ( dsatutil_atoi((dsat_num_item_type *) &arg1, 
                     tok_ptr->arg[0], 10) != ATOI_OK )
  {
    /* if index1 can not be converted into a legal number,
       this is invalid parameter */
    MSG_MED("CPBR index1 %s must be a positive number",tok_ptr->arg[0],0,0);
    return DSAT_ERROR;
  }
  if( ( (me_cpbs_entry.total != -1) && 
        ((int16) arg1 > me_cpbs_entry.total))
      || (arg1 < PB_INDEX_MIN) || (arg1 > PB_INDEX_MAX) )
  {
    dsatme_set_cme_error(DSAT_CME_INVALID_INDEX, res_buff_ptr);
    return DSAT_CMD_ERR_RSP;
  }

  if (tok_ptr->args_found > 1)
  {
    if ( dsatutil_atoi((dsat_num_item_type *) &arg2, 
                       tok_ptr->arg[1], 10) != ATOI_OK )
    {
      /* if index2 can not be converted into a legal number,
         this is invalid parameter */
      MSG_MED("CPBR 2nd arg %s must be a positive number",tok_ptr->arg[1],0,0);
      return DSAT_ERROR;
    }
      
    /* if the index is out of PBM allowed range, output a cme error */
    if( ( (me_cpbs_entry.total != -1) && 
          ((int16) arg2 > me_cpbs_entry.total) )
        || (arg2 < PB_INDEX_MIN) || (arg2 > PB_INDEX_MAX) )
    {
      dsatme_set_cme_error(DSAT_CME_INVALID_INDEX, res_buff_ptr);
      return DSAT_CMD_ERR_RSP;
    }
  }
  else
  {
    /* only give index 1 */
    arg2 = arg1;
  }

  *index1_ptr = MIN(arg1, arg2);
  *index2_ptr = MAX(arg1, arg2);

  return result;
  
} /* cpbr_parse_arguments */

/*===========================================================================

FUNCTION CPBW_PARSE_ARGUMENTS

DESCRIPTION
  Parse the +cpbw argument into pbm_record_s_type.

DEPENDENCIES
  None

RETURN VALUE
  Returns the command running status.
    DSAT_ERROR       : if the parameter is not correct
    DSAT_CMD_ERR_RSP : if there is a cme error
    DSAT_OK          : if it is a success.

SIDE EFFECTS
  None

===========================================================================*/
LOCAL dsat_result_enum_type cpbw_parse_arguments
(
  const tokens_struct_type * tok_ptr,
  dsat_pbm_record_s_type        * record_ptr,
  dsm_item_type            * res_buff_ptr  /* for reporting error */
)
{
  int index_to_write;
      
  /* Get the index to write */
  if (tok_ptr->arg[0][0] == '\0')
  {
    index_to_write = -1;
  }
  else
  {
    if ( dsatutil_atoi((dsat_num_item_type *)&index_to_write, 
                       tok_ptr->arg[0], 10) != ATOI_OK)
    {
      MSG_MED("CPBW's first argument (%s) should be a positive number",
              tok_ptr->arg[0],0,0);
      return DSAT_ERROR;
    }
    if( ( (me_cpbs_entry.total != -1) && 
          ((int16) index_to_write > me_cpbs_entry.total))
        || (index_to_write < PB_INDEX_MIN) 
        || (index_to_write > PB_INDEX_MAX) )
    {
      dsatme_set_cme_error(DSAT_CME_INVALID_INDEX, res_buff_ptr);
      return DSAT_CMD_ERR_RSP;
    }
  }
    
  record_ptr->index = (int16) index_to_write;
    
  /* if the number field is ignored, the record_ptr->index
     entry is erased */
  if( (tok_ptr->args_found < 2) || (tok_ptr->arg[1][0] == '\0') )
  {
    record_ptr->number[0] = '\0';
  }
  else /* user input a number */
  {
    /* strip out the "s from the tok_ptr->arg[1] */
    if ( !dsatutil_strip_quotes_out( tok_ptr->arg[1], 
                                     tok_ptr->arg[1], MAX_LINE_SIZE))
    {
      return DSAT_ERROR;
    }

    /* strip out format dividers and err on illegal chars in the number 
       string.  It is done here, not in pbm because pbm_record_s_type
       can't hold a number more than PBM_MAX_NUM_LENGTH (CPB_MAX_NUM_LENGTH)
       long.  Thus, a legal number of PBM_MAX_NUM_LENGTH long with ()- in will 
       exceed PBM_MAX_NUM_LENGTH and fail on length check.
     */
    if( !cpbw_check_num_string(tok_ptr->arg[1], tok_ptr->arg[1]) )
    {
      dsatme_set_cme_error(DSAT_CME_INVALID_CHAR_IN_DIAL_STRING, res_buff_ptr);
      return DSAT_CMD_ERR_RSP;
    }

    /* check length */
    if(strlen((char *) tok_ptr->arg[1]) > CPB_MAX_NUM_LENGTH)
    {
      dsatme_set_cme_error(DSAT_CME_DIAL_STRING_TOO_LONG, res_buff_ptr);
      return DSAT_CMD_ERR_RSP;
    }

    /* copy it into record_ptr->number */
    (void) std_strlcpy(record_ptr->number,
                       (char *)tok_ptr->arg[1],
                       PBM_NUM_SIZE);
  }

  /* get the address type */
  if( (tok_ptr->args_found < 3) || (tok_ptr->arg[2][0] == '\0') )
  {
    /* user does not give a number type */
    if(record_ptr->number[0] == '+')
    {
      record_ptr->type = TA_INTERNATIONAL;
    }
    else
    {
      record_ptr->type = TA_OTHER;
    }
  }
  else /* user input a type */
  {
    int addr_type;
        
    if ( dsatutil_atoi((dsat_num_item_type *) &addr_type, 
                       tok_ptr->arg[2], 10) != ATOI_OK )
    {
      MSG_MED("cpbw incorrect address type %s",tok_ptr->arg[2],0,0);
      return DSAT_ERROR;
    }

    /* ui can't handle a CPB_MAX_NUM_LENGTH international number since UI 
       always add + sign before the number.  This makes it 
       CPB_MAX_NUM_LENGTH + 1 long */
    if ( (addr_type == TA_INTERNATIONAL) && 
         (record_ptr->number[0] != '+') &&
         (strlen(record_ptr->number) > (CPB_MAX_NUM_LENGTH - 1)) )
    {
      dsatme_set_cme_error(DSAT_CME_DIAL_STRING_TOO_LONG, res_buff_ptr);
      return DSAT_CMD_ERR_RSP;
    }

    /* type is allowed from 128-255 */
    if(addr_type < 128 || addr_type > 255)
    {
      /* wrong parameter */
      dsatme_set_cme_error(DSAT_CME_INVALID_INDEX, res_buff_ptr);
      return DSAT_CMD_ERR_RSP;
    }
    
    record_ptr->type = (uint8) addr_type;
  }

  /* get the fourth argument, ph text */
  if( (tok_ptr->args_found < 4) || (tok_ptr->arg[3][0] == '\0'))
  {
    record_ptr->text[0] = '\0';
  }
  else /* user give a text */
  {
    /* If me_cpbs_entry.text_len is -1 it means maximum supported length
       could not be determined for this device in this case use default */
    int max_text_len = me_cpbs_entry.text_len < 0 ? 
      PBM_MAX_TEXT_LENGTH : me_cpbs_entry.text_len;

    /* What length of <text> parm would be once quotes are stripped and
       it is converted to ME alphabet */
    uint16 converted_len = (uint16)(strlen((char *) tok_ptr->arg[3]) - 2);

    /* Verify converted <text> will be of correct length */
    if ( converted_len > max_text_len )
    {
      dsatme_set_cme_error(DSAT_CME_TEXT_STRING_TOO_LONG,
                               res_buff_ptr);
      return DSAT_CMD_ERR_RSP;
    }

    /* Attempt to strip out the "s from the tok_ptr->arg[3] and
       convert from TE to ME alphabet return ERROR on failure */
    if( !dsatutil_strip_quotes_out(tok_ptr->arg[3],
                                   tok_ptr->arg[3], 
                                   (uint16)strlen((char *)tok_ptr->arg[3])) ||
         dsatutil_convert_chset((const char *)tok_ptr->arg[3], 
                                ALPHA_IRA,
                                record_ptr->text,
                                ALPHA_PBM_UCS2,
                                PBM_MAX_TEXT_LENGTH, 
                                FALSE) == NULL )
    {
      return DSAT_ERROR;
    }
  }

  return DSAT_OK;

} /* cpbw_parse_arguments */


/*===========================================================================

FUNCTION CPBW_CHECK_NUM_STRING

DESCRIPTION
  The input is supposed to be a null-terminated byte string.
  This function strip out fomat dividers, ie ()-.
  The function will return FALSE if there are illegal chars in
  the in_str.  The illegal chars are anything not 0-9, #, *, +.
  
  + sign is legal if it is at the beginning of the num.

DEPENDENCIES
  Caller makes sure there are enough space allocated for output string.
  in_str is null-terminated.

RETURN VALUE
  TRUE check passed
  FALSE if there is illegal chars here

SIDE EFFECTS
  None

===========================================================================*/
LOCAL boolean cpbw_check_num_string
(
  const byte * in_str,
  byte * out_str
)
{
  const byte *in_ptr = in_str;
  byte *out_ptr = out_str;

  /* Handle special case of + */
  if(*in_ptr == '+')
  {
    *out_ptr = *in_ptr;
    in_ptr++;
    out_ptr++;
  }

  while(*in_ptr)
  {
    /* strip out format divider */
    if ( (*in_ptr == '(') || (*in_ptr == ')') || (*in_ptr == '-') )
    {
      in_ptr++;
      /* ignore (,),- */
      continue;
    }

    /* check illegal chars */
    if ( (*in_ptr != '#') && (*in_ptr != '*') && 
         ((*in_ptr < '0') || (*in_ptr > '9') ) )
    {
      return FALSE;
    }
    
    *out_ptr = *in_ptr;
    in_ptr++;
    out_ptr++;
  }
  
  *out_ptr = 0;
  return TRUE;
} /* cpbw_check_num_string */


/*===========================================================================

FUNCTION DSATME_CPB_CME_ERROR_LOOKUP

DESCRIPTION
  Map pbm return error type to a cme error code.

DEPENDENCIES
  None

RETURN VALUE
  Returns the cme error code if found.
  Returns DSAT_CME_MAX if not found.

SIDE EFFECTS
  None

===========================================================================*/
/* ARGSUSED */
dsat_cme_error_e_type
dsatme_cpb_cme_error_lookup(pbm_return_type result)
{
  int index;

  static struct {
    pbm_return_type pbm_result;
    dsat_cme_error_e_type me_error_code;
  } error_code_table[] =
    {
      { PBM_ERROR_PH_SIM_REQ, DSAT_CME_PH_SIM_PIN_REQUIRED },
      { PBM_ERROR_NO_SIM,     DSAT_CME_SIM_NOT_INSERTED    },
      { PBM_ERROR_SIM_PIN_REQ, DSAT_CME_SIM_PIN_REQUIRED   },
      { PBM_ERROR_SIM_PUC_REQ, DSAT_CME_SIM_PUK_REQUIRED   },
      { PBM_ERROR_SIM_FAIL,    DSAT_CME_SIM_FAILURE        },
      { PBM_ERROR_PIN2_REQ,    DSAT_CME_SIM_PIN2_REQUIRED  },
      { PBM_ERROR_PUC2_REQ,    DSAT_CME_SIM_PUK2_REQUIRED  },
      { PBM_ERROR_MEM_FULL,    DSAT_CME_MEMORY_FULL        },
      { PBM_ERROR_INDEX_INVALID, DSAT_CME_INVALID_INDEX    },

      /* when entry not found for pbm_search */
      { PBM_ERROR_NOT_FOUND,     DSAT_CME_NOT_FOUND        },

      /* when the text too long for pbm_write or pbm_search */
      { PBM_ERROR_TEXT_TOO_LONG, DSAT_CME_TEXT_STRING_TOO_LONG },

      /* when the number too long for pbm_write */
      { PBM_ERROR_NUMBER_TOO_LONG, DSAT_CME_DIAL_STRING_TOO_LONG},

      /* when there is invalid char in number for pbm_write */
      { PBM_ERROR_INVALID_CHAR, DSAT_CME_INVALID_CHAR_IN_DIAL_STRING},

      { PBM_ERROR,              DSAT_CME_UNKNOWN}
    };

  for (index = 0;; index++) {
    if(error_code_table[index].pbm_result == result)
    {
      return error_code_table[index].me_error_code;
    }
    
    /* result not found and run to the end of error code table */
    else if(error_code_table[index].pbm_result == PBM_ERROR)
    {
        return DSAT_CME_MAX;
    }
  }

} /* dsatme_cpb_cme_error_lookup */


/*===========================================================================

FUNCTION CPB_PROCESS_PBM_ERROR

DESCRIPTION
  Process pbm error, translate them into cme error and output it.

  err_buff_ptr: write error to this buffer.
                If err_buff_ptr is NULL.  The CME error is sent 
                to dsatcmdp to process.

DEPENDENCIES
  None

RETURN VALUE
  None

SIDE EFFECTS
  None
===========================================================================*/
LOCAL void cpb_process_pbm_error(pbm_return_type pbm_err, 
                                 dsm_item_type * err_buff_ptr)
{
  dsm_item_type * res_buff_ptr;
  dsat_cme_error_e_type error_code = dsatme_cpb_cme_error_lookup(pbm_err);

  if(error_code == DSAT_CME_MAX)
  {
    /* error code not found, use DSAT_CME_UNKNOWN */
    ERR("unrecognized pbm_return_type error %d",pbm_err,0,0);
    error_code = DSAT_CME_UNKNOWN;
  }
  
  if(err_buff_ptr != NULL)
  {
    dsatme_set_cme_error(error_code, err_buff_ptr);
  }
  else
  {
    if( (res_buff_ptr = dsm_new_buffer( DSM_DS_LARGE_ITEM_POOL ))
        == NULL )
    {
      ERR( "Running out of items", 0, 0, 0);
      return;
    }

    dsatme_set_cme_error(error_code, res_buff_ptr);
    dsatcmdp_handle_async_cmd_rsp( res_buff_ptr, DSAT_COMPLETE_RSP );
  }
} /* cpb_process_pbm_error */


/*===========================================================================

FUNCTION ME_PBM_CB_SUBMIT_DS_CMD

DESCRIPTION
  Submit a ds cmd to process pbm cb result in ds_task.

DEPENDENCIES
  None

RETURN VALUE
  None

SIDE EFFECTS
  None
===========================================================================*/
LOCAL void me_pbm_cb_submit_ds_cmd
(
  cb_cmd_e_type   cmd,                   /* cmd id                   */
  pbm_return_type status,                /*  command status          */

                                         /* for CPB_PBM_CB_FILE_INFO */
  ds_at_pbm_file_info_type * info_ptr,   /* file info                */


  pbm_record_s_type * record_ptr         /* for CPB_PBM_CB_READ,
                                            CPB_PBM_CB_FIND          */
)
{
  ds_cmd_type *cmd_buf;

  if( (cmd_buf = ds_get_cmd_buf()) == NULL)
  {
    ERR_FATAL("No ds command buffer",0,0,0);
  }
  else
  {
    /* send the message to ATCOP */
    cmd_buf->hdr.cmd_id = DS_AT_PBM_CB_CMD;
    cmd_buf->cmd.pbm_info.cmd     = (int8) cmd;
    cmd_buf->cmd.pbm_info.status     = status;

    /* record_ptr or  info_ptr contain good information only when 
       status is PBM_SUCCESS */
    if(status == PBM_SUCCESS)
    {
      if(cmd == CPB_PBM_CB_READY_FILE_INFO ||
         cmd == CPB_PBM_CB_FILE_INFO ||
         cmd == DD_PBM_FILE_INFO)
      {
        if (info_ptr == NULL)
        {
          ERR("NULL info_ptr",0,0,0);
          memset(&cmd_buf->cmd.pbm_info.cmd_info.file_info, 0, 
                 sizeof(ds_at_pbm_file_info_type));
        }
        else
        {
          cmd_buf->cmd.pbm_info.cmd_info.file_info = *info_ptr;
        }
      }
    }

    ds_put_cmd(cmd_buf);
  }
} /* me_pbm_cb_submit_ds_cmd */


/*===========================================================================

FUNCTION ME_DD_PROCESS_N

DESCRIPTION
  Process direct dialing when D> string contains a number.

DEPENDENCIES
  None

RETURN VALUE
  DSAT_ASYNC_CMD   : async call make to pbm to get a ph number
  DSAT_CMD_ERR_RSP : if there is a cme error
  DSAT_ERROR       : if there is an error
  DSAT_NO_RSP      : We find the number, and dial it out.
  DSAT_OK
SIDE EFFECTS
  None
  
===========================================================================*/
LOCAL dsat_result_enum_type me_dd_process_n
(
  /* the ph book memory to get the number from                  */
  const byte    * mem, 

  /* the index of the ph number in the ph book memory           */
  int           index,

  /* reporting error if there is any */
  dsm_item_type * err_buff_ptr
)
{
  pbm_return_type     pbm_result;
  dsat_result_enum_type result = DSAT_OK;
  pbm_device_type       device;
  uint16 record_id;

  /*-------- get pbm device type of the ph book memory ---------*/
  if (!get_pbm_device(mem, &device))
  {
    ERR("Wrong mem, %c%c, %d", mem[0], mem[1], mem[2]);
    /* can't map mem to a valid pbm device */
    return DSAT_ERROR;
  }

  record_id = pbm_location_to_record_id(device, index); 
  cpb_pbm_read_record( record_id, &dd_val.record, &pbm_result);

  if (pbm_result == PBM_SUCCESS )
  {
    /* dial it out */
    dd_val.dial_str.num = (byte *) dd_val.record.number;
    dd_val.dial_str.len = (uint16) strlen((char *) dd_val.dial_str.num);
#if defined(FEATURE_WCDMA) || defined(FEATURE_GSM)
#error code not present
#endif
  }
  else if (pbm_result == PBM_ERROR_NOT_FOUND)
  {
    cpb_process_pbm_error(pbm_result, NULL);
    result = DSAT_CMD_ERR_RSP;
  }
  else
  {
    result = cpb_pbm_read_handler_error_status(pbm_result);
  }
  return result;
  
} /* me_dd_process_n */


/*===========================================================================

FUNCTION ME_DD_PROCESS_MEM_N

DESCRIPTION
  Process direct dialing when D> string contains mem<n>.

DEPENDENCIES
  None

RETURN VALUE
  DSAT_ASYNC_CMD   : async call make to pbm to get a ph number
  DSAT_CMD_ERR_RSP : if there is a cme error
  DSAT_ERROR       : if there is an error

SIDE EFFECTS
  None
  
===========================================================================*/
/* ARGSUSED */
LOCAL dsat_result_enum_type me_dd_process_mem_n
(
  /* the ph book memory to get the number from                  */
  const byte * mem, 

  /* the index of the ph number in the ph book memory           */
  int index,

  /* reporting error if there is any */
  dsm_item_type * err_buff_ptr
)
{
  pbm_device_type       pbm_device;

  /*-------- get file info of the memory mem            --------*/

  /* the index should be the same as in dd_val */
  ASSERT(index == dd_val.index);
  
  if (!get_pbm_device(mem, &pbm_device))
  {
    ERR("Wrong mem, %c%c, %d", mem[0], mem[1], mem[2]);
    /* can't map mem to a valid pbm device */
    return DSAT_ERROR;
  }
  cb_expected = DD_PBM_FILE_INFO;
  /* error cases are also handled in the call-back */
  (void)pbm_extended_file_info(pbm_device,
                               me_pbm_file_info_cb);
  return DSAT_ASYNC_CMD;
} /* me_dd_process_mem_n */


/*===========================================================================

FUNCTION ME_DD_PROCESS_STR

DESCRIPTION
  Process direct dialing when D> string contains str, pattern to 
  search names in the phone book.

DEPENDENCIES
  None

RETURN VALUE
  DSAT_ASYNC_CMD   : async call make to pbm to get a ph number
  DSAT_CMD_ERR_RSP : if there is a cme error
  DSAT_ERROR       : if there is an error

SIDE EFFECTS
  None
  
===========================================================================*/
LOCAL dsat_result_enum_type me_dd_process_str
(
  /* string pattern to search the names in the phone book                  */
  const byte * str, 

  /* reporting error if there is any                                       */
  dsm_item_type * err_buff_ptr
)
{
  dsat_result_enum_type result = DSAT_ERROR;

  pbm_device_type       pbm_device;

  /* copy the content of str here since str is const                    */
  char  srh_str[MAX_LINE_SIZE+1];

    dd_val.srch_pb = (int8) dsat_cpbs_val;
    dd_val.record.number[0] = '\0';
    memset(srh_str, '\0', MAX_LINE_SIZE + 1);
    /* Convert from TE character set, per +CSCA setting, to PBM's internal 
       chset, fail on inconvertible character */
    (void)dsatutil_convert_chset( (const char *)str, 
                                  ALPHA_IRA,
                                  srh_str,
                                  ALPHA_PBM_UCS2, 
                                  MAX_LINE_SIZE + 1,
                                  FALSE );



  /* see if we are done with the last ph book */
  while(dsat_cpbs_storage_valstr[dd_val.srch_pb][0] != '\0')
  {
    /* search the ph book */
    if (!get_pbm_device(dsat_cpbs_storage_valstr[dd_val.srch_pb], &pbm_device))
    {
      ERR("Wrong mem",0,0,0);
      /* can't map mem to a valid pbm device */
      return DSAT_ERROR;
    }

    /* Perform search in the PBM */
    result = dd_pbm_find_handler((byte *)srh_str, pbm_device);
    /* prepare the last dial number for saving to the last dial ph book */
    if( (result == DSAT_NO_RSP) || (result == DSAT_ASYNC_CMD) )
    {
      /* the number was dialed, init the ld_rec */
      dsatme_ld_init_record(dd_val.record.number, dd_val.record.type, 
                                dd_val.record.text);
      return result;
    }

    /* Next Phone book */
    if (dd_val.srch_pb == (int8) dsat_cpbs_val)
    {
      /* we are done with current ph book, continue searching the 
         first non-current ph book */
      dd_val.srch_pb = 0;
      if(dd_val.srch_pb == (int8) dsat_cpbs_val)
      {
        dd_val.srch_pb++;
      }
    }
    else
    {
      /* continue searching the next non-current ph book */
      dd_val.srch_pb++;
      if(dd_val.srch_pb == (int8) dsat_cpbs_val)
      {
        dd_val.srch_pb++;
      }
    }
  }
  /* see if we are done with the last ph book */
  if(dsat_cpbs_storage_valstr[dd_val.srch_pb][0] == '\0')
  {
    /* reset dd_val.srch_pb */
    dd_val.srch_pb = -1;

    if(dd_val.record.number[0] == '\0')
    {
      /* we are done with all the phone book but not found a 
         match*/
      cpb_process_pbm_error(PBM_ERROR_NOT_FOUND, err_buff_ptr);
      result = DSAT_CMD_ERR_RSP;
    }
    else
    {
      /* we found a match, dial it out                    */
      dd_val.dial_str.num = (byte *) dd_val.record.number;
      dd_val.dial_str.len = (uint16) strlen((char *) dd_val.dial_str.num);
#if defined(FEATURE_WCDMA) || defined(FEATURE_GSM)
#error code not present
#endif
    }
  }

  return result;
} /* me_dd_process_str */
#endif /* defined(FEATURE_ETSI_PBM) || defined(FEATURE_DSAT_CDMA_PBM) */
#ifdef FEATURE_MMGSDI
/*-------------------------------------------------------------------------
            Function Definitions: Internal functions
-------------------------------------------------------------------------*/


/*===========================================================================

FUNCTION  ME_REGISTER_CB_FUNC

DESCRIPTION
  Registers the callback functions.with other tasks.
  
DEPENDENCIES
  None

RETURN VALUE
  None

SIDE EFFECTS
  None

===========================================================================*/
LOCAL void me_register_cb_func ( void )
{
  gsdi_returns_T gsdi_status = GSDI_SUCCESS;

  /* Register the GSDI event callback function */
  gsdi_status = gsdi_reg_callback ( gsdi_pin_event_cb_func );

  /* This should not happen, raise an error */
  if (gsdi_status != GSDI_SUCCESS)
  {
    MSG_ERROR( "Could not register PIN event callback function with GSDI", 
               0, 0, 0 );
  }

#if defined(FEATURE_DATA_ETSI_PIN) && defined(FEATURE_MMGSDI_PERSONALIZATION)
#error code not present
#endif /* FEATURE_DATA_ETSI_PIN && FEATURE_MMGSDI_PERSONALIZATION */

  return;
}/* me_register_cb_func */ 



/*===========================================================================
FUNCTION  GSDI_PIN_EVENT_CB_FUNC 

DESCRIPTION
  GSDI PIN status callback function

DEPENDENCIES
  None

RETURNS
  None

SIDE EFFECTS
  Adds command in DS command buffer

===========================================================================*/
LOCAL void gsdi_pin_event_cb_func 
(
  gsdi_sim_events_T             event            /* Event ID              */
)
{
#if defined(FEATURE_DATA_ETSI_PIN) || defined(FEATURE_UIM_SUPPORT_3GPD)
  ds_cmd_type * cmd_buf;
#endif /* defined(FEATURE_DATA_ETSI_PIN) || defined(FEATURE_UIM_SUPPORT_3GPD) */
    
  /* Filter events of interest to ATCOP */
  switch (event)
  {
#ifdef FEATURE_DATA_ETSI_PIN
#error code not present
#endif /* FEATURE_DATA_ETSI_PIN */

#if defined(FEATURE_DATA_ETSI_PIN) || defined(FEATURE_UIM_SUPPORT_3GPD)
  case GSDI_SIM_INIT_COMPLETED:
    if( (cmd_buf = ds_get_cmd_buf()) == NULL)
    {
      ERR_FATAL("No ds command buffer",0,0,0);
    }
    else
    {
      /* send this command to ATCOP for PIN event */
      cmd_buf->hdr.cmd_id = DS_AT_GSDI_INFO_CMD;
      cmd_buf->cmd.gsdi_event.event = event;
      ds_put_cmd(cmd_buf);
    }
    break;
#endif /* defined(FEATURE_DATA_ETSI_PIN) || defined(FEATURE_UIM_SUPPORT_3GPD) */

  default:
    /* Ignore event */
    break;
  }
}  /* gsdi_pin_event_cb_func */

#ifdef FEATURE_RUIM_CDMA_REFRESH
/*===========================================================================
FUNCTION DSATME_MMGSDI_REFRESH_RUIM_FCN_READ

DESCRIPTION
  Handles the asynchronous response of FCN refesh.
  It reads the EF's present in FCN indication.
  
DEPENDENCIES
  None

RETURN VALUE
  None

SIDE EFFECTS
  None

===========================================================================*/
void dsatme_mmgsdi_refresh_ruim_fcn_read
(
  ds_cmd_type         * cmd_ptr              /* DS Command pointer   */
)
{
  boolean pass_fail = TRUE;
  uint8 index =0;
  mmgsdi_return_enum_type mmgsdi_status = MMGSDI_ERROR;
  dsat_fcn_ind_type fcn_indication = {0}; 
  mmgsdi_option_type mmgsdi_option = {0};
  
  MSG_MED("In function dsatme_mmgsdi_refresh_ruim_fcn_read()",0,0,0);

  ASSERT(NULL != cmd_ptr);

  fcn_indication = cmd_ptr->cmd.mmgsdi_event_info.u.fcn_ind;

  if((0 == fcn_indication.num_files)||(fcn_indication.num_files > DSAT_MAX_NUM_RUIM_REFRESH_FILES))
  {
    MSG_MED("MMGSDI refresh complete has %d files pass_fail = FALSE",fcn_indication.num_files,0,0);
    pass_fail = FALSE;
  }
  else
  {
    MSG_MED("MMGSDI refresh complete has %d number of files",fcn_indication.num_files,0,0);
    while((index < fcn_indication.num_files)&& (FALSE != pass_fail))
    {
      switch(fcn_indication.file_list[index])
      {
        case MMGSDI_CDMA_3GPD_3GPDOPM:
        {
          /*-------------------------------------------------------------------------
            Get Mobile IP preference setting from NV.
          -------------------------------------------------------------------------*/
          MSG_MED("getting NV_DS_QCMIP_I",0,0,0);
#ifdef FEATURE_DS_MOBILE_IP
          /* if dsat707_qcmip_update is called with TRUE
           * it will read the QCMIP value from NV and update 
           * the global variable dsat707_qcmip_val. 
           */
          pass_fail = dsat707_qcmip_update(TRUE);
          MSG_MED(" dsat707_qcmip_update() returned %d",(int)pass_fail,0,0);
#endif /* FEATURE_DS_MOBILE_IP */
          break;
        }
        case MMGSDI_CDMA_3GPD_MIPSP:
        {
          /*-------------------------------------------------------------------------
            Get Mobile IP active profile from NV.
          -------------------------------------------------------------------------*/
          MSG_MED("getting NV_DS_MIP_ACTIVE_PROF_I",0,0,0);
#ifdef FEATURE_DS_MOBILE_IP
          /* if dsat707_qcmipp_update is called with FALSE
           * it will read the QCMIPP value from NV and update 
           * the global variable dsat707_qcmipp_val. 
           */
          pass_fail = dsat707_qcmipp_update(FALSE);
          MSG_MED(" dsat707_qcmipp_update() returned %d",(int)pass_fail,0,0);
#endif /* FEATURE_DS_MOBILE_IP */
          break;
        }
        default:
          MSG_ERROR("Unknown file in file list ,pass_fail = FALSE",0,0,0);
          pass_fail=FALSE;
          break;
      }/* Switch end */
      index++;
    }/* While end */

  }
  MSG_MED("Sending MMGSDI Refresh complete pass_fail(%d)",(int)pass_fail,0,0);
  mmgsdi_status = mmgsdi_refresh_complete(dsatme_mmgsdi_client_id,
                                         MMGSDI_SLOT_1,
                                         pass_fail,
                                         mmgsdi_option,
                                         me_mmgsdi_rsp_cb,
                                         0);
  if ( mmgsdi_status != MMGSDI_SUCCESS )
  {
    MSG_ERROR("ATCoP failed to post REFRESH COMPLETE to MMGSDI",0,0,0);
  }
} /* dsatme_mmgsdi_refresh_ruim_fcn_read */

/*==============================================================================
FUNCTION  DSATME_RUIM_MMGSDI_REGISTER_FOR_REFRESH

DESCRIPTION
  Will Register ATCoP for File Change Notifications for RUIM NV.

PARAMETERS

DEPENDENCIES
  None.

RETURN VALUE

SIDE EFFECTS

==============================================================================*/
void dsatme_ruim_mmgsdi_register_for_refresh
(
  void
)
{
  mmgsdi_return_enum_type mmgsdi_status = MMGSDI_ERROR;
  mmgsdi_option_type mmgsdi_option = {0};
  mmgsdi_refresh_file_list_type refresh_files;
  mmgsdi_file_enum_type file_list[DSAT_MAX_NUM_RUIM_REFRESH_FILES] = 
                     {MMGSDI_CDMA_3GPD_3GPDOPM,
                      MMGSDI_CDMA_3GPD_MIPSP}; /* Corresponds to 
    NV items NV_DS_MIP_ACTIVE_PROF_I & NV_DS_QCMIP_I */

  refresh_files.num_files = DSAT_MAX_NUM_RUIM_REFRESH_FILES;
  refresh_files.file_list_ptr = &file_list[0];

  MSG_MED("Sending request for file change notification",0,0,0);
  /* Register with MMGSDI for File Change Notification using the file list
    in the array above   */
  mmgsdi_status = mmgsdi_register_for_refresh(dsatme_mmgsdi_client_id,
                                  MMGSDI_SLOT_1,
                                  refresh_files,
                                  FALSE,
                                  mmgsdi_option,
                                  me_mmgsdi_rsp_cb,
                                  0);

  /* Check to see if REQ for Registration
     was OK.  */
  if ( mmgsdi_status != MMGSDI_SUCCESS)
  {
    MSG_ERROR("Failed to Request Refresh FCN Registration",0,0,0);
  }
} /* dsatme_ruim_mmgsdi_register_for_refresh */

#endif /* FEATURE_RUIM_CDMA_REFRESH */

/*===========================================================================
FUNCTION me_mmgsdi_client_id_reg_cb

DESCRIPTION
  This function handles the MMGSDI client ID registration command callback.  
  It will assign the global client ID value which will be used in mmgsdi 
  function calls

ARGUMENTS
  

DEPENDENCIES
  None

DEPENDENCIES
  None

RETURN VALUE
  None

SIDE EFFECTS
  None
===========================================================================*/
LOCAL void me_mmgsdi_client_id_reg_cb
( 
  mmgsdi_return_enum_type status,
  mmgsdi_cnf_enum_type    cnf,
  const mmgsdi_cnf_type        *cnf_ptr
)
{
  mmgsdi_return_enum_type mmgsdi_status = MMGSDI_ERROR;
  if (status == MMGSDI_SUCCESS)
  {
    if (cnf == MMGSDI_CLIENT_ID_REG_CNF)
    {
      dsatme_mmgsdi_client_id = 
                          cnf_ptr->client_id_reg_cnf.response_header.client_id;
      /* Register for MMGSDI events */
      mmgsdi_status = mmgsdi_client_evt_reg(dsatme_mmgsdi_client_id, 
                                        me_mmgsdi_client_evt_reg_cb, 
                                        me_mmgsdi_rsp_cb,
                                        0);
      if (mmgsdi_status != MMGSDI_SUCCESS)
      {
        MSG_ERROR("mmgsdi_client_evt_reg failed %d", mmgsdi_status, 0, 0);
      }
    }
  }
  else
  {
    MSG_ERROR("MMGSDI status is not success",0,0,0);
  }
} /* me_mmgsdi_client_id_reg_cb */

/*===========================================================================
FUNCTION ME_MMGSDI_RSP_CB

DESCRIPTION
  This function handles the MMGSDI client event response callback.  

ARGUMENTS

DESCRIPTION
    This function is the callback function used for a register for refresh command.

DEPENDENCIES
    None

DEPENDENCIES
  None

RETURN VALUE
    None

SIDE EFFECTS
  None
===========================================================================*/
LOCAL void me_mmgsdi_rsp_cb
(
  mmgsdi_return_enum_type mmgsdi_status,
  mmgsdi_cnf_enum_type    cnf_type,
  const mmgsdi_cnf_type * cnf_data_ptr
)
{
  if ( mmgsdi_status == MMGSDI_SUCCESS)
  {
    MSG_HIGH("ATCoP MMGSDI request successful",0,0,0);
  }
  else
  {
    MSG_ERROR("ATCoP MMGSDI request Failed Code: %d",
    mmgsdi_status,0,0);
  }
}/* me_mmgsdi_rsp_cb() */

/*===========================================================================

FUNCTION  DSATME_MMGSDI_EVENT_HANDLER

DESCRIPTION
  This function is the handler function for the MMGSDI SIM events

DEPENDENCIES
  None

RETURN VALUE
  returns an enum that describes the result of the command execution.
  possible values:
    DSAT_ASYNC_EVENT : async event, contine processing  

SIDE EFFECTS
  None

===========================================================================*/
/* ARGSUSED */
dsat_result_enum_type dsatme_mmgsdi_event_handler
(
  dsat_mode_enum_type   mode,                /* AT command mode            */
  ds_cmd_type         * cmd_ptr              /* DS Command pointer         */
)
{
  MSG_HIGH("In function dsatme_mmgsdi_event_handler()",0,0,0);
  ASSERT(NULL != cmd_ptr);
  switch (cmd_ptr->cmd.mmgsdi_event_info.mmgsdi_event)
  {
    case MMGSDI_CARD_INIT_COMPLETED_EVT:
      /* Initialize NV items */
      dsat707_nv_sync();
#ifdef FEATURE_RUIM_CDMA_REFRESH
    /* Register with MMGSDI for refresh handler */
    dsatme_ruim_mmgsdi_register_for_refresh();
#endif /*FEATURE_RUIM_CDMA_REFRESH */
      ps_send_cmd (PS_GSDI_EVENT_HANDLER_CMD,
        (void*) &(cmd_ptr->cmd.mmgsdi_event_info.mmgsdi_event));
      break;
#ifdef FEATURE_RUIM_CDMA_REFRESH
    case MMGSDI_REFRESH_EVT:
      dsatme_mmgsdi_refresh_ruim_fcn_read(cmd_ptr);
      break;
#endif /* FEATURE_RUIM_CDMA_REFRESH */
    default:
      /* appease lint */
      break;
  }
  return DSAT_ASYNC_EVENT;
} /* dsatme_mmgsdi_event_handler */

/*==============================================================================

FUNCTION ME_MMGSDI_CLIENT_EVT_REG_CB

DESCRIPTION
  Function that gets called by MMGSDI when a FCN arrives

RETURN VALUE

  void

DEPENDENCIES
  None

SIDE EFFECTS
  None
==============================================================================*/
LOCAL void me_mmgsdi_client_evt_reg_cb
(
    const mmgsdi_event_data_type   *mmgsdi_event
)
{
  ds_cmd_type         *cmd_ptr;
  mmgsdi_return_enum_type mmgsdi_status = MMGSDI_ERROR;
  mmgsdi_option_type mmgsdi_option;
  
  if(mmgsdi_event == NULL)
  {
    MSG_ERROR("mmgsdi_event is a NULL pointer",0,0,0);
    return;
  }
  memset((void*)&mmgsdi_option,0x0,sizeof(mmgsdi_option_type));
  MSG_MED("Received MMGSDI event %d",mmgsdi_event->evt,0,0);

  switch(mmgsdi_event->evt)
  {
    case MMGSDI_CARD_INIT_COMPLETED_EVT:
      cmd_ptr = ds_get_cmd_buf();
      if (cmd_ptr != NULL)
      {
        ds_at_mmgsdi_event_type *cmd_rd_ptr = &cmd_ptr->cmd.mmgsdi_event_info;
        MSG_MED("Received SIM INIT completed %d",
                mmgsdi_event->data.refresh.refresh_files.num_files,0,0);
        /* Fill in command type */
        cmd_ptr->hdr.cmd_id = DS_AT_MMGSDI_INFO_CMD;
        cmd_rd_ptr->mmgsdi_event = mmgsdi_event->evt;
        cmd_rd_ptr->cmd_status =(int16) MMGSDI_SUCCESS;

        ds_put_cmd(cmd_ptr);
      }
      else
      {
        ERR_FATAL("No ds command buffer",0,0,0);
      }
      break;
#ifdef FEATURE_RUIM_CDMA_REFRESH
    case MMGSDI_REFRESH_EVT:
      if(mmgsdi_event->data.refresh.stage == MMGSDI_REFRESH_STAGE_WAIT_FOR_OK_TO_FCN)
      {
        /* Invoke OK to refresh */
        mmgsdi_status = mmgsdi_ok_to_refresh(dsatme_mmgsdi_client_id, 
                                   MMGSDI_SLOT_1, 
                                   TRUE,
                                   mmgsdi_option,
                                   me_mmgsdi_rsp_cb,
                                   0);
        if(mmgsdi_status != MMGSDI_SUCCESS)
        {
          MSG_ERROR("mmgsdi_ok to refresh failed with return status %d",
                    mmgsdi_status, 0, 0);
        }
      }
      else if(mmgsdi_event->data.refresh.stage == MMGSDI_REFRESH_STAGE_START)
      {
        if((mmgsdi_event->data.refresh.refresh_files.file_list_ptr == NULL)||
           (mmgsdi_event->data.refresh.refresh_files.num_files == 0))
        {
          MSG_ERROR("null ptr: me_mmgsdi_refresh_file_change_notification_cb",0,0,0);
          return;
        }
        cmd_ptr = ds_get_cmd_buf();

        if (cmd_ptr != NULL)
        {
          ds_at_mmgsdi_event_type *cmd_rd_ptr = &cmd_ptr->cmd.mmgsdi_event_info;
          MSG_MED("Received file change notification %d",
                  mmgsdi_event->data.refresh.refresh_files.num_files,0,0);
          /* Fill in command type */
          cmd_ptr->hdr.cmd_id = DS_AT_MMGSDI_INFO_CMD;
          cmd_rd_ptr->mmgsdi_event = mmgsdi_event->evt;
          cmd_rd_ptr->cmd_status =(int16) MMGSDI_SUCCESS;

          /* Copy data to command buffer */
          cmd_rd_ptr->u.fcn_ind.num_files = 
          mmgsdi_event->data.refresh.refresh_files.num_files;
          memcpy(cmd_rd_ptr->u.fcn_ind.file_list,
                 mmgsdi_event->data.refresh.refresh_files.file_list_ptr,
                 MIN(mmgsdi_event->data.refresh.refresh_files.num_files,
                 DSAT_MAX_NUM_RUIM_REFRESH_FILES) * 
                 sizeof(uim_items_enum_type));

          ds_put_cmd(cmd_ptr);
        }
        else
        {
          ERR_FATAL("No ds command buffer",0,0,0);
        }
      }
      break;
#endif /* FEATURE_RUIM_CDMA_REFRESH */
    default:
      break;
  }
} /* me_mmgsdi_client_evt_reg_cb */

#endif /* FEATURE_MMGSDI */

#ifdef FEATURE_UIM_SUPPORT_3GPD
/*===========================================================================

FUNCTION UIM_3GPD_OP_CAP_REPORT                             INTERNAL FUNCTION

DESCRIPTION
  Accept status reports from R-UIM and prints out message


DEPENDENCIES


RETURN VALUE
  None

SIDE EFFECTS
  None

===========================================================================*/
LOCAL void uim_3gpd_op_cap_report
(
  uim_rpt_type *report           /* R-UIM command status report */
)
{
  if(report->rpt_status == UIM_PASS)
  {
    MSG_LOW("3GPD Operational Capabilities Set on Card",0,0,0);
  }
  else
  {
    MSG_HIGH("3GPD Operational Capabilities NOT SET ON CARD!",0,0,0);
  }

} /* uim_3gpd_op_cap_report */



/*===========================================================================

FUNCTION init_uim_operational_capability

DESCRIPTION
  This function initializes the operational capability elementary file
  on an IS-820 Release A card to indicate 3GPD capabilities supported based
  on the features defined.
  It creates and issues a UIM ACCESS command to write to the file.

DEPENDENCIES
  None

RETURN VALUE
  None

SIDE EFFECTS
  None

===========================================================================*/

LOCAL void init_uim_operational_capability
(
  void
)
{

 dsatme_uim.operational_capabilities =0;
#ifdef FEATURE_DATA_PS
  dsatme_uim.operational_capabilities |= UIM_SUPPORT_SIMPLE_IP_MASK;
#ifdef FEATURE_DS_MOBILE_IP
  dsatme_uim.operational_capabilities |= UIM_SUPPORT_FALLBACK_MASK;
  dsatme_uim.operational_capabilities |= UIM_SUPPORT_MOBILE_IP_MASK;
#endif /* FEATURE_DS_MOBILE_IP */
#endif /* FEATURE_DATA_PS */


  dsatme_uim.uim_cmd.access_uim.num_bytes   = 1;
  dsatme_uim.uim_cmd.access_uim.data_ptr    = &dsatme_uim.operational_capabilities;
  dsatme_uim.uim_cmd.access_uim.offset      = 0;
  dsatme_uim.uim_cmd.access_uim.item        = UIM_CDMA_3GPD_ME3GPDOPC;
  dsatme_uim.uim_cmd.access_uim.access      = UIM_WRITE_F;
  dsatme_uim.uim_cmd.access_uim.hdr.command = UIM_ACCESS_F;
  dsatme_uim.uim_cmd.access_uim.hdr.cmd_hdr.task_ptr
                                      = NULL;
  dsatme_uim.uim_cmd.access_uim.hdr.cmd_hdr.sigs
                                      = 0;
  dsatme_uim.uim_cmd.access_uim.hdr.cmd_hdr.done_q_ptr
                                      = NULL;
  dsatme_uim.uim_cmd.hdr.rpt_function       = uim_3gpd_op_cap_report;
  dsatme_uim.uim_cmd.hdr.protocol           = UIM_CDMA;
  dsatme_uim.uim_cmd.access_uim.hdr.options = UIM_OPTION_ALWAYS_RPT;
  dsatme_uim.uim_cmd.access_uim.hdr.slot    = UIM_SLOT_AUTOMATIC;

  uim_cmd( &dsatme_uim.uim_cmd );
  return;

} /* init_uim_operational_capability */

 /*===========================================================================

FUNCTION DSAT_UIM_3GPD_CDMA_SVC_INIT_REPORT                  INTERNAL FUNCTION

DESCRIPTION
  Accept status reports from R-UIM and prints out message for CDMA SVC table 
  check


DEPENDENCIES


RETURN VALUE
  None

SIDE EFFECTS
  None

===========================================================================*/
LOCAL void dsat_uim_3gpd_cdma_svc_init_report
(
  uim_rpt_type *report           /* R-UIM command status report */
)
{ 
  dsatme_uim.report_status = report->rpt_status;

  if(report->rpt_status == UIM_PASS)
  {
    MSG_LOW("CDMA service table read",0,0,0);
  }
  else
  {
    MSG_HIGH("Could Not Read CDMA Service Table!",0,0,0);
  }
  (void)rex_set_sigs( &ds_tcb, DS_UIM_CMD_SIG );
} /* dsat_uim_3gpd_cdma_svc_init_report */

/*===========================================================================

FUNCTION DSATME_UIM_CHECK_CDMA_SVC_TABLE
                                        
DESCRIPTION
  This function issues a read to the CDMA service table to see if 3GPD is 
  supported on the R-UIM.  It is called during initialization.
  
PARAMETERS

  boolean   *support_flag       a pointer to a boolean type to be set if support
                                is available
  
DEPENDENCIES
  None.

RETURN VALUE

                                                 
SIDE EFFECTS


===========================================================================*/ 
void dsatme_uim_check_cdma_svc_table(boolean *support_sip, 
                                     boolean *support_mip,
                                     boolean *service_3gpd_extensions)
{
  uim_svc_table_return_type support;

#ifdef FEATURE_UIM_RUN_TIME_ENABLE
  if (NV_RTRE_CONTROL_USE_RUIM != nv_rtre_control())
  {
    *support_sip = FALSE;
    *support_mip = FALSE;
    *service_3gpd_extensions=FALSE;
    return;
  }
  memset(dsatme_uim.cdma_svc_table_buffer, 0, UIM_CDMA_SVC_TABLE_SIZE);
#endif /* FEATURE_UIM_RUN_TIME_ENABLE */
  /* Read the CDMA service table */
  dsatme_uim.uim_cmd.access_uim.num_bytes        = UIM_CDMA_SVC_TABLE_SIZE;
  dsatme_uim.uim_cmd.access_uim.data_ptr    = dsatme_uim.cdma_svc_table_buffer;
  dsatme_uim.uim_cmd.access_uim.offset= 0;
  dsatme_uim.uim_cmd.access_uim.item             = UIM_CDMA_CDMA_SVC_TABLE;
  dsatme_uim.uim_cmd.access_uim.access           = UIM_READ_F;

  dsatme_uim.uim_cmd.access_uim.hdr.command                 = UIM_ACCESS_F;
  dsatme_uim.uim_cmd.access_uim.hdr.cmd_hdr.task_ptr        = NULL;
  dsatme_uim.uim_cmd.access_uim.hdr.cmd_hdr.sigs            = 0;
  dsatme_uim.uim_cmd.access_uim.hdr.cmd_hdr.done_q_ptr      = NULL;
  dsatme_uim.uim_cmd.hdr.rpt_function       = dsat_uim_3gpd_cdma_svc_init_report;
  dsatme_uim.uim_cmd.hdr.protocol           = UIM_CDMA;
  dsatme_uim.uim_cmd.access_uim.hdr.options = UIM_OPTION_ALWAYS_RPT;
  dsatme_uim.uim_cmd.access_uim.hdr.slot    = UIM_SLOT_AUTOMATIC;

  /*-----------------------------------------------------------------------
    Send the command to the R-UIM:
    Clear the "command done signal"
    Send the command
    Wait for the command to be done
  -----------------------------------------------------------------------*/
  (void) rex_clr_sigs( rex_self(), DS_UIM_CMD_SIG );

  uim_cmd( &dsatme_uim.uim_cmd );
  MSG_MED("BLOCKING DS TASK ON UIM CMD SIGNAL - UIM READ",0,0,0);
  ASSERT(rex_self() == &ds_tcb);
  (void)dsi_wait(DS_UIM_CMD_SIG);
  MSG_MED("UNBLOCKING DS TASK ON UIM CMD SIGNAL - UIM READ",0,0,0);
  if(dsatme_uim.report_status == UIM_PASS)
  {
    MSG_HIGH("UIM-READ Success", 0, 0, 0);
  }
  else
  {
    MSG_HIGH("UIM-READ FAILURE", 0, 0, 0);
    /* Failed reading the service table */
    MSG_HIGH("Could not read CDMA Service Table", 0, 0, 0);
   return;
  }

  support = uim_return_cdma_svc_availabililty (UIM_CDMA_SVC_3GPD_SIP,
                            dsatme_uim.cdma_svc_table_buffer);

  if(support.allocated && support.activated)
  {
    *support_sip = TRUE;
  }
  else
  {
    *support_sip = FALSE;
  }
#ifdef FEATURE_UIM_3GPD_MOBILE_IP
  support = uim_return_cdma_svc_availabililty (UIM_CDMA_SVC_3GPD_MIP,
                            dsatme_uim.cdma_svc_table_buffer);

  if(support.allocated && support.activated)
  {
    *support_mip = TRUE;
  }
  else
  {
    *support_mip = FALSE;
  }
#else
  *support_mip = FALSE;
#endif /* FEATURE_UIM_3GPD_MOBILE_IP */
  support = uim_return_cdma_svc_availabililty (UIM_CDMA_SVC_3GPD_MSG_EXT,
                                               dsatme_uim.cdma_svc_table_buffer);
  if(support.allocated && support.activated)
  {
    *service_3gpd_extensions = TRUE;
  }
  else 
  {
    *service_3gpd_extensions = FALSE;
  }
  
  return;
} /* dsatme_check_cdma_svc_table */
#endif  /* FEATURE_UIM_SUPPORT_3GPD */

#endif /* FEATURE_DATA */
