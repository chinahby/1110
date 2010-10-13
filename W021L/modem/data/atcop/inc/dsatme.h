#ifndef DSATME_H
#define DSATME_H
/*====*====*====*====*====*====*====*====*====*====*====*====*====*====*====*

                D A T A   S E R V I C E S

                A T   C O M M A N D   
                
                ( M o b i l e  E q u i p m e n t  C O M M A N D S )
                
                P R O C E S S I N G

                E X T E R N A L   H E A D E R   F I L E


GENERAL DESCRIPTION
  This file contains the definitions of data structures, defined and
  enumerated constants and function prototypes required for the
  executing the mobile equipment commands.

EXTERNALIZED FUNCTIONS

INITIALIZATION AND SEQUENCING REQUIREMENTS

   Copyright (c) 2003 - 2009 by QUALCOMM Incorporated.
   All Rights Reserved.
   Qualcomm Confidential and Proprietary.
*====*====*====*====*====*====*====*====*====*====*====*====*====*====*====*/


/*===========================================================================

                        EDIT HISTORY FOR MODULE

  This section contains comments describing changes made to the module.
  Notice that changes are listed in reverse chronological order.

  $Header: //source/qcom/qct/modem/data/common/commonatcop/main/lite/inc/dsatme.h#6 $

when       who     what, where, why
--------   ---     ----------------------------------------------------------
02/03/09   bs      Fixed compilation errors/warnings.
01/02/09   bs      Added support for 1X AT Phonebook commands.
08/12/08   bs      Fixed featurization issue.
07/09/08   bs      Added support for CDMA NAARefresh.
09/21/07 sq/mga/yz Merged entire file to support FEATURE_UIM_SUPPORT_3GPD.
08/29/05   snb     Add MMGSDI client ID variable.
01/06/03   snb     Move registration for pin event with GSDI and handling 
                   callback for pin events to common file and add protocol
                   stack re-initialization under feature define.

===========================================================================*/
#include "comdef.h"
#include "customer.h"

#ifdef FEATURE_DATA
#include "dsati.h"

#if defined(FEATURE_ETSI_PBM) || defined(FEATURE_DSAT_CDMA_PBM)
#include "uiupbm.h"
#endif /* defined(FEATURE_ETSI_PBM) || defined(FEATURE_DSAT_CDMA_PBM) */

#ifdef FEATURE_MMGSDI_PERSONALIZATION
#define PIN_MAX_LEN GSDI_PERSO_MAX_CK
#elif defined(FEATURE_MMGSDI) /* FEATURE_MMGSDI_PERSONALIZATION */
#define PIN_MAX_LEN GSDI_MAX_PIN_LEN
#else /* FEATURE_MMGSDI */
#define PIN_MAX_LEN DSAT_PIN_LENGTH
#endif

#ifdef FEATURE_MMGSDI
#include "mmgsdilib.h"
#endif /* FEATURE_MMGSDI */

#define DSAT_PIN_LENGTH 8

/* Note which callback function submitted async cmd status cmd.
   This determine the action to take when the status handler function 
   is called.
   
   The variable of this type identifies the ME cmd processing status before 
   calling the async commands of a subsystem.  It determines 
   the next action to take after the async cmd cb is called.
   
   Note which callback function is expected to be called for debug purpose.
   Make sure the right callback is called at the right time.
   Usually you should not care about it unless you want the callback safty
   check too.  Then you need make sure the cb_expected is set correctly.
   Hint, looking for ASSERT(cb_expected == xxx)
*/

/* turn off complaint about CB_MAX not used. It is needed to handle the feature
   combinations */
/*lint -e749 */
typedef enum
{
  CB_NONE = 0,            /* no callback function expected to be 
                             called                                     */
#if defined(FEATURE_ETSI_PBM) || defined(FEATURE_DSAT_CDMA_PBM)
  CPB_PBM_CB_READY,       /* inform atcop that pbm is ready             */
  CPB_PBM_CB_READY_FILE_INFO,
                          /* atcop calls pbm_file_info to get the info 
                             of the default ph book storage type.  It
                             is not for an AT cmd                       */
  CPB_PBM_CB_FILE_INFO,   /* cb return's the request pbm device 
                             information for cpbs                       */
  CPB_PBM_CB_WRITE,       /* cb return's status if the write action 
                             succeeds                                   */
  /* the pbm cmd group for direct dialing                               */
  DD_PBM_FILE_INFO,     /* direct dialing processes ATD><mem><n>.  
                           It calls pbm_file_info to get the valid 
                           index range of ph book type <mem>            */
#ifdef FEATURE_MMGSDI
  CPBS_MMGSDI_PIN2_STATUS,   /* cpbs command requested pin status          */
  CPBS_MMGSDI_PIN2_VERIFY,   /* cpbs command requested pin verification    */
#endif /* FEATURE_MMGSDI */
#endif /* defined(FEATURE_ETSI_PBM) || defined(FEATURE_DSAT_CDMA_PBM) */
#ifdef FEATURE_UIM_SUPPORT_3GPD
  MMGSDI_CDMA_RUIM_OP_WRITE,
  MMGSDI_CDMA_RUIM_OP_READ,
#endif /*  FEATURE_UIM_SUPPORT_3GPD */
  CB_MAX 
} cb_cmd_e_type;

typedef dsat_result_enum_type (*status_handler_type) (ds_at_cmd_status_type);

typedef struct 
{
  const cb_cmd_e_type cmd_id;
  const status_handler_type status_handler;
} status_handler_table_entry_type;

#ifdef FEATURE_MMGSDI
/*---------------------------------------------------------------------------
  ATCOP's MMGSDI client ID: 
  Only needed for SIM card power control functionality.
---------------------------------------------------------------------------*/
extern mmgsdi_client_id_type dsatme_mmgsdi_client_id;

  #define VERIFY_PIN( slot, pin_id, pin_data, client_ref ) \
    mmgsdi_result = mmgsdi_verify_pin(dsatme_mmgsdi_client_id, \
                                      slot, \
                                      pin_id, \
                                      pin_data, \
                                      dsatme_mmgsdi_req_cb, \
                                      (mmgsdi_client_data_type)client_ref);
  
  #define GET_PIN_STATUS( slot, client_ref ) \
    mmgsdi_status = mmgsdi_get_all_pin_status \
                            (dsatme_mmgsdi_client_id, \
                             slot, \
                             dsatme_mmgsdi_req_cb,\
                             (mmgsdi_client_data_type)client_ref); 
#endif /* FEATURE_MMGSDI */

/* Length of storage type string */
#define CPBS_VAL_LEN 2

#if defined(FEATURE_ETSI_PBM) || defined(FEATURE_DSAT_CDMA_PBM)
/* the structure contains infos about a ph book storage type */
typedef struct {
  /* pbm enum corresponding to the atcop storage type */
  pbm_device_type device; 

  /* ATCOP ph book storage type corresponding to the pbm enum.
     This field seems redundant with device.  It is here to save
     a lot of conversion between ATCOP storage type string and 
     pbm device enum. */
  byte storage[CPBS_VAL_LEN + 1]; 

  /* total locations in current ph book, -1 means pbm does
     not reveal the information about the storage */
  int16 total;

  /* maximum text length allowed for the current 
     storage type */
  int16 text_len;
} me_pb_entry_type;

typedef struct 
{
  int     index;                /* Logical index across multiple phonebook sets */
  char    number[PBM_NUM_SIZE]; /* + and null for string */
  uint8   type;                 /* type of address octet in integer format */
  char    text[PBM_TEXT_SIZE_BYTES ];  /* null terminated string */
  uint32  time;                        /* Date and time in clock seconds */
  uint32  duration;                    /* in seconds, USIM supports up to 0x00FFFFFF */
  boolean is_answered;                 /* TRUE = answered, FALSE = not answered */ 
} dsat_pbm_record_s_type;
#endif /* defined(FEATURE_ETSI_PBM) || defined(FEATURE_DSAT_CDMA_PBM) */


/*===========================================================================

                        PUBLIC DATA DECLARATIONS

===========================================================================*/
LOCAL void submit_cmd_status 
(
  cb_cmd_e_type cmd_id,
  uint16         status,
  byte          data
);

/*===========================================================================

FUNCTION DSATME_CMD_STATUS_HANDLER

DESCRIPTION
  This function is the async event handler for async functions such as
  pbm_select and pbm_write.

DEPENDENCIES

RETURN VALUE
  None

SIDE EFFECTS
  None
===========================================================================*/
dsat_result_enum_type dsatme_cmd_status_handler
(
  dsat_mode_enum_type mode,
  ds_cmd_type * cmd_ptr
);

/*-------------------------------------------------------------------------
            PUBLIC FUNCTION DECLARATIONS
-------------------------------------------------------------------------*/
#ifdef FEATURE_MMGSDI
/*===========================================================================

FUNCTION DSATME_MMGSDI_REQ_CB

DESCRIPTION
  This function is the callback function for the MMGSDI to return
  pin status.

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
);

/*===========================================================================

FUNCTION  DSATME_INIT_ME

DESCRIPTION
  ME initialization function. This function does the following:
  - Initializes ATCOP as one of the clients to GSDI.
  - Registers the client call back functions.
  
DEPENDENCIES
  None

RETURN VALUE
  None

SIDE EFFECTS
  None

===========================================================================*/
void dsatme_init_me ( void );


/*===========================================================================

FUNCTION  DSATME_GSDI_EVENT_HANDLER

DESCRIPTION
  This function is the handler function for the GSDI events when 
  FEATURE_DATA_ETSI_PIN is not defined.

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
);

#endif /* FEATURE_MMGSDI */
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
);
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
void dsatme_init_pbm ( void );

/*===========================================================================

FUNCTION DSATME_EXEC_CPBS_CMD

DESCRIPTION
  This function takes the result from the command line parser
  and executes it. It executes +CPBS command.
  The result is in res_buff_ptr.

DEPENDENCIES

RETURN VALUE
  returns an enum that describes the result of the command execution.
  possible values:
    DSAT_ERROR : if there was any problem in executing the command
    DSAT_OK : if it is a success.
    DSAT_ASYNC_CMD : Async call is successfully made to pbm to set
                     a particular storage.

SIDE EFFECTS
  None

===========================================================================*/
dsat_result_enum_type dsatme_exec_cpbs_cmd
(
  dsat_mode_enum_type mode,     /*  AT command mode:            */
  const dsati_cmd_type *parse_table, /*  Ptr to cmd in parse table */
  const tokens_struct_type *tok_ptr,      /*  Command tokens from parser  */
  dsm_item_type *res_buff_ptr  /*  Place to put response       */
);
/*===========================================================================

FUNCTION DSATME_EXEC_CPBR_CMD

DESCRIPTION
  This function takes the result from the command line parser
  and executes it. It executes +CPBR command.
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

SIDE EFFECTS
  None

===========================================================================*/
dsat_result_enum_type dsatme_exec_cpbr_cmd
(
  dsat_mode_enum_type mode,               /*  AT command mode:            */
  const dsati_cmd_type *parse_table,      /*  Ptr to cmd in parse table */
  const tokens_struct_type *tok_ptr,      /*  Command tokens from parser  */
  dsm_item_type *res_buff_ptr             /*  Place to put response       */
);

/*===========================================================================

FUNCTION DSATME_EXEC_CPBF_CMD

DESCRIPTION
  This function takes the result from the command line parser
  and executes it. It executes +CPBF command.
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
dsat_result_enum_type dsatme_exec_cpbf_cmd
(
  dsat_mode_enum_type mode,               /*  AT command mode:            */
  const dsati_cmd_type *parse_table,      /*  Ptr to cmd in parse table */
  const tokens_struct_type *tok_ptr,      /*  Command tokens from parser  */
  dsm_item_type *res_buff_ptr             /*  Place to put response       */
);

/*===========================================================================

FUNCTION DSATME_EXEC_CPBW_CMD

DESCRIPTION
  This function takes the result from the command line parser
  and executes it. It executes +CPBW command.
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
dsat_result_enum_type dsatme_exec_cpbw_cmd
(
  dsat_mode_enum_type mode,               /*  AT command mode:            */
  const dsati_cmd_type *parse_table,      /*  Ptr to cmd in parse table */
  const tokens_struct_type *tok_ptr,      /*  Command tokens from parser  */
  dsm_item_type *res_buff_ptr             /*  Place to put response       */
);

/*===========================================================================

FUNCTION DSATME_RESET_PB

DESCRIPTION
  Reset ph book to default values.

DEPENDENCIES
  None

RETURN VALUE
  None

SIDE EFFECTS
  me_cpbs_entry will be set to default if dsat_cpbs_val is 0
  and me_cpbs_entry.device != PBM_DS_SM

===========================================================================*/
void dsatme_reset_pb(void);

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
dsat_result_enum_type dsatme_pbm_cb_cmd_handler
(
  dsat_mode_enum_type   mode,
  ds_cmd_type *         cmd_ptr
);

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
);

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
);

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
);
#endif /* defined(FEATURE_ETSI_PBM) || defined(FEATURE_DSAT_CDMA_PBM) */

#ifdef FEATURE_MMGSDI

/*===========================================================================

FUNCTION  DSATME_RUIM_MMGSDI_EVENT_HANDLER

DESCRIPTION
  This function is the handler function for the MMGSDI RUIM events

DEPENDENCIES
  None

RETURN VALUE
  returns an enum that describes the result of the command execution.
  possible values:
    DSAT_ASYNC_EVENT : async event, continue processing.

SIDE EFFECTS
  None

===========================================================================*/
dsat_result_enum_type dsatme_mmgsdi_event_handler
(
  dsat_mode_enum_type   mode,                /* AT command mode            */
  ds_cmd_type         * cmd_ptr              /* DS Command pointer         */
);

#ifdef FEATURE_RUIM_CDMA_REFRESH
/*==============================================================================
FUNCTION  DSATME_RUIM_MMGSDI_REGISTER_FOR_REFRESH

DESCRIPTION
  This function registers the required files of RUIM NV used in ATCoP for file
  change notifications.

PARAMETERS

DEPENDENCIES
  None.

RETURN VALUE

SIDE EFFECTS

==============================================================================*/
void dsatme_ruim_mmgsdi_register_for_refresh
(
  void
);

#endif /* FEATURE_RUIM_CDMA_REFRESH */

#endif /* FEATURE_MMGSDI */

#endif /* FEATURE_DATA */

#endif /* DSATME_H */
