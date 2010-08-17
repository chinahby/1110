#ifndef DSAT_H
#define DSAT_H
/*===========================================================================

                        D A T A   S E R V I C E S

                A T   C O M M A N D   P R O C E S S O R

                E X T E R N A L   H E A D E R   F I L E


DESCRIPTION
  This file contains the definitions of data structures, defined and
  enumerated constants, and function prototypes required for the
  data services AT command processor.

Copyright (c) 2001 - 2009 by QUALCOMM Incorporated.
All Rights Reserved.
Qualcomm Confidential and Proprietary.
===========================================================================*/


/*===========================================================================

                      EDIT HISTORY FOR FILE

  This section contains comments describing changes made to the module.
  Notice that changes are listed in reverse chronological order.

  $PVCSPath: L:/src/asw/MM_DATA/vcs/dsat.h_v   1.10   08 Nov 2002 12:46:24   wxie  $
  $Header: //source/qcom/qct/modem/data/common/commonatcop/main/lite/inc/dsat.h#2 $ $DateTime: 2009/01/22 06:46:48 $ $Author: bhaviks $

when       who     what, where, why
--------   ---     ----------------------------------------------------------
01/02/09   bs      Added support for 1X AT Phonebook commands.
10/15/07 sq/mga/yz Made change to include dstask.h when 
                   FEATURE_DATA_STRIP_ATCOP is defined.
11/01/06   an      Featurize ATCOP, RMSM, Async Fax
05/28/03   sb      Fix for WCDMA CS Calls
05/01/03   sb      Added new result code (DSAT_ATH_CMD) to indicate that we 
                   are processing ATH command.
04/28/03   sb      AT command flow control. Removed FEATURE_DSAT_FLOW_CONTROL
03/28/03   ar      Added dsat_flush_me_msg_queue() prototype.
03/17/03   ar      Add +CHSN command support.
03/07/03   ar      Remove FEATURE_DATA_ETSI_SUPSERV wrappers
01/16/03   wx      I,G modifier support
01/09/03   wx      Added +ES, +ESA support
01/08/03   ar      Removed obsolete dsat_get_cur_umts_cgdcont() prototype
01/07/03   rsl     Removed dsat_qcpma_val, not being used anymore.
12/03/02   ar      Added vendor specific codes to dsat_cme_error_e_type
11/08/02   wx      Export dsat_qcdns_val for ps.
10/30/02   rsl     Made DSAT_CRC_CODE_VOICE common.
09/05/02   sb      ATCOP changes based on new Multi mode Data services design
07/13/02   atp     Added dsat_reg_rsp_fmter() for adding ability to register
                   response formating function.
06/27/02   ar      Added support for Quality of Service profile
04/17/02   rc      Removed FEATURE wrap FEATURE_DS_SOCKETS around qcdns 
                   variables. 
02/21/02   sjd     Changed init service mode parameter to CM system mode type.
08/28/01   sjd     Modifications to support new ATCOP VU structure.
08/10/01   sjd     Initial release to MSM5200 archives.

===========================================================================*/

#include "comdef.h"
#include "customer.h"
#ifdef FEATURE_DATA
#ifndef FEATURE_DATA_STRIP_ATCOP
#include "dstask.h"
#include "sio.h"
#include "cm.h"
#include "dsm.h"
#include "rex.h"
#include "sys.h"

/*===========================================================================

                      PUBLIC DATA DECLARATIONS

===========================================================================*/

typedef byte dsat_string_item_type;
typedef unsigned int dsat_num_item_type;

/*---------------------------------------------------------------------------
    The numerical values in this "enum" are important:  
    The values for OK thru NO_ANSWER must match the numeric codes in EIA-602.                         
    The values for the cellular extention codes must begin with the 
      value MIN_CELL_RES (defined in dsati.h), and increase in sequence.            
    The values for the cellular error codes must being with the value 
      MIN_CELL_ERR (defined in dsati.h), and increase in sequence.

    No specific values are required for ETSI extended format of incoming
    indications, as they are only displayed as strings.
    
    The values for the internal codes must be negative, other than those
    used to identify the range of particular result code types.
---------------------------------------------------------------------------*/

typedef enum
{
  /*-------------------------------------------------------------------------
    Numbered per ITU-T V.25 ter
  -------------------------------------------------------------------------*/
  DSAT_OK             = 0,    /*  AT: OK            */
  DSAT_CONNECT        = 1,    /*  AT: CONNECT       */
  DSAT_RING           = 2,    /*  AT: RING          */
  DSAT_NO_CARRIER     = 3,    /*  AT: NO CARRIER    */
  DSAT_ERROR          = 4,    /*  AT: ERROR         */
  DSAT_NO_DIALTONE    = 6,    /*  AT: NO DIAL TONE  */
  DSAT_BUSY           = 7,    /*  AT: BUSY          */
  DSAT_NO_ANSWER      = 8,    /*  AT: NO ANSWER     */
  DSAT_MAX_BASIC      = 9,    /*  End of basic result codes                */

  DSAT_CRC_CODE_VOICE = 31,  /* normal voice                       */

#if defined(FEATURE_WCDMA) || defined(FEATURE_GSM)  
#error code not present
#endif /* defined(FEATURE_WCDMA) || defined(FEATURE_GSM) */

  /*-------------------------------------------------------------------------
     Not reported to TE
     Note that DSAT_CXT_ORIG is defined as -13 and not as -12  
     because some builds used -12 for other definition.
  -------------------------------------------------------------------------*/
  DSAT_ATH_CMD        = -18,  /*  internal: hangup                   */
  DSAT_ASYNC_EVENT    = -17,  /*  internal: async event, not associated
                                            with a command              */
  DSAT_ASYNC_CMD      = -16,  /*  internal: async command in progress   */
  DSAT_CMD_ERR_RSP    = -15,  /*  internal: send cmd specific error
                                            rsp only and no result code */

  DSAT_NO_RSP         = -14,  /*  internal: send no response to cmd  */
  DSAT_CXT_ORIG       = -13,  /*  internal: originate w/ unknown cmd */

  DSAT_DO_QCOTC       = -11,  /*  internal: open traffic channel     */
  DSAT_DO_DM          = -10,  /*  internal: change to DM mode        */
  DSAT_DO_QCDAM       = -9,   /*  internal: analog dial for PCMCIA   */
  DSAT_DO_HARD_PARSE  = -8,   /*  internal: parse cmd w/o delimiter  */
  DSAT_DO_CHV         = -7,   /*  internal: cellular hang up voice   */
  DSAT_DO_CDV         = -6,   /*  internal: cellular dial voice      */
  DSAT_TEST           = -5,   /*  internal: remote test command      */
  DSAT_PARAM          = -4,   /*  internal: parameter out of range   */
  DSAT_UNREC          = -3,   /*  internal: unrecognized command     */
  DSAT_DO_ANS         = -2,   /*  internal: answer  (auto or ATA)    */
  DSAT_DO_ORIG        = -1    /*  internal: originate data call      */

} dsat_result_enum_type;


/*===========================================================================
                 AT Command Processor Data Structures
===========================================================================*/

/*---------------------------------------------------------------------------
     The following enumerated type represents the states of ATCOP: command, 
     online data, and online command. The ME is in command state when the 
     transport layer is not in the ESTABLISHED state. When in command state
     or online command state, all AT commands received on Rm are processed.
     When the ME is in the online data state, the ME passes all data received
     on the Rm directly to the IWF.  
---------------------------------------------------------------------------*/
typedef enum
{
  DSAT_CMD, 
  DSAT_ONLINE_DATA,
  DSAT_ONLINE_CMD
} dsat_mode_enum_type;

/*===========================================================================
                AT Command Response Generator Data Structures
===========================================================================*/

/*---------------------------------------------------------------------------
  The enum type for specifying the type of response data passed into the
  format response function.  Used to determine if header and/or trailer
  should be added to response.
---------------------------------------------------------------------------*/
typedef enum
{
  DSAT_START_OF_RSP,        /* Start of response - add header only        */
  DSAT_END_OF_RSP,          /* End of response - add trailer only         */
  DSAT_INTERMEDIATE_RSP,    /* Intermediate response - add no header or
                               trailer                                    */
  DSAT_COMPLETE_RSP         /* Complete response - add header and trailer */
} dsat_rsp_enum_type;

#if defined(FEATURE_WCDMA) || defined(FEATURE_GSM)
#error code not present
#endif /* defined(FEATURE_WCDMA) || defined(FEATURE_GSM) */

/*---------------------------------------------------------------------------
  The enum type for +CME Error Codes from 3GPP TS27.007 9.2
---------------------------------------------------------------------------*/
typedef enum
{
  DSAT_CME_NO_ERROR              =  -1,  /* RESERVED */
  DSAT_CME_PHONE_FAILURE         =   0,  /* phone failure */
  DSAT_CME_NO_CONNECTION         =   1,  /* no connection to phone */
  DSAT_CME_ADAPTOR_LINK_RESERVED =   2,  /* phone adaptor link reserved */
  DSAT_CME_OP_NOT_ALLOWED        =   3,  /* operation not allowed */
  DSAT_CME_OP_NOT_SUPPORTED      =   4,  /* operation not supported */
  DSAT_CME_PH_SIM_PIN_REQUIRED   =   5,  /* PH-SIM PIN required */
  DSAT_CME_PH_FSIM_PIN_REQUIRED  =   6,  /* PH-FSIM PIN required */
  DSAT_CME_PH_FSIM_PUK_REQUIRED  =   7,  /* PH-FSIM PUK required */
  DSAT_CME_SIM_NOT_INSERTED      =  10,  /* SIM not inserted */
  DSAT_CME_SIM_PIN_REQUIRED      =  11,  /* SIM PIN required */
  DSAT_CME_SIM_PUK_REQUIRED      =  12,  /* SIM PUK required */
  DSAT_CME_SIM_FAILURE           =  13,  /* SIM failure */
  DSAT_CME_SIM_BUSY              =  14,  /* SIM busy */
  DSAT_CME_SIM_WRONG             =  15,  /* SIM wrong */
  DSAT_CME_INCORRECT_PASSWORD    =  16,  /* incorrect password */
  DSAT_CME_SIM_PIN2_REQUIRED     =  17,  /* SIM PIN2 required */
  DSAT_CME_SIM_PUK2_REQUIRED     =  18,  /* SIM PUK2 required */
  DSAT_CME_MEMORY_FULL           =  20,  /* memory full */
  DSAT_CME_INVALID_INDEX         =  21,  /* invalid index */
  DSAT_CME_NOT_FOUND             =  22,  /* not found */
  DSAT_CME_MEMORY_FAILURE        =  23,  /* memory failure */
  DSAT_CME_TEXT_STRING_TOO_LONG  =  24,  /* text string too long */
  DSAT_CME_INVALID_CHAR_IN_TEXT  =  25,  /* invalid characters in text string */
  DSAT_CME_DIAL_STRING_TOO_LONG  =  26,  /* dial string too long */
  DSAT_CME_INVALID_CHAR_IN_DIAL_STRING
    =  27,  /* invalid characters in dial string */
  DSAT_CME_NO_NETWORK_SERVICE    =  30,  /* no network service */
  DSAT_CME_NETWORK_TIMEOUT       =  31,  /* network timeout */
  DSAT_CME_NETWORK_NOT_ALLOWED   =  32,  /* network not allowed - emergency 
                                             calls only */
  DSAT_CME_NET_PERSON_PIN_REQUIRED= 40,  /* network personalization PIN 
                                             required */
  DSAT_CME_NET_PERSON_PUK_REQUIRED= 41,  /* network personalization PUK 
                                             required */
  DSAT_CME_NET_SUBSET_PERSON_PIN_REQUIRED
                                  =  42,  /* network subset personalization 
                                             PIN required */
  DSAT_CME_NET_SUBSET_PERSON_PUK_REQUIRED
                                  =  43,  /* network subset personalization PUK
                                             required */
  DSAT_CME_SP_PERSON_PIN_REQUIRED=  44,  /* service provider personalization 
                                             PIN required */
  DSAT_CME_SP_PERSON_PUK_REQUIRED=  45,  /* service provider personalization 
                                             PUK required */
  DSAT_CME_CORP_PERSON_PIN_REQUIRED
                                  =  46,  /* corporate personalization PIN 
                                             required */
  DSAT_CME_CORP_PERSON_PUK_REQUIRED
                                  =  47,  /* corporate personalization PUK
                                             required */
  DSAT_CME_UNKNOWN               = 100,  /* unknown */
  DSAT_CME_ILLEGAL_MS            = 103,  /* Illegal Mem_Store */
  DSAT_CME_ILLEGAL_ME            = 106,  /* Illegal ME */
  DSAT_CME_GPRS_serv_not_allowed = 107,  /* GPRS services not allowed */
  DSAT_CME_PLMN_NOT_ALLOWED      = 111,  /* PLMN not allowed */
  DSAT_CME_LOCATION_NOT_ALLOWED  = 112,  /* Location area not allowed */
  DSAT_CME_ROAMING_NOT_ALLOWED   = 113,  /* Roaming not allowed in this 
                                             location area */
  DSAT_CME_SERV_OP_NOT_SUPPORTED = 132,  /* service option not supported */
  DSAT_CME_REQ_SERV_OP_NOT_SUBSCRIBED
                                  = 133,  /* requested service option not 
                                             subscribed */
  DSAT_CME_SERV_OP_TEMP_OUT_OF_ORDER
                                  = 134,  /* service option temporarily 
                                             out of order */
  DSAT_CME_unspecified_GPRS_error= 148,  /* unspecified GPRS error */
  DSAT_CME_PDP_AUTH_FAILURE      = 149,  /* PDP authentication failure */
  DSAT_CME_INVALID_MOBILE_CLASS  = 150,  /* invalid mobile class */
  
  /* Qualcomm specific error codes */
  DSAT_CME_SUPS_NETWORK_REJECTED = 257,  /* network rejected supserv request */
  DSAT_CME_SUPS_RETRY_OPERATION  = 258,  /* retry operation */
  DSAT_CME_SUPS_DFL_INVALID_NUMBER  = 259,  /* invalid deflected to number */
  DSAT_CME_SUPS_DFL_SELF_NUMBER  = 260,  /* deflected to own number */
  DSAT_CME_SUPS_UNKNOWN_NUMBER   = 261,  /* unknown subscriber */
  DSAT_CME_SUPS_SERVICE_UNAVAIL  = 262,  /* service not available */
  DSAT_CME_SUPS_UNKNOWN_CLASS    = 263,  /* unknown class */
  DSAT_CME_SUPS_UNKNOWN_NETMSG   = 264,  /* unknown network message */
  
  DSAT_CME_MAX                           /* atcop internal only */
} dsat_cme_error_e_type;


/*---------------------------------------------------------------------------
  The following function pointer type provides mode specific AT commmand
  response routing functionality.  It defines a function type that routes
  responses based on operating mode and AT command state.

  rsp_ptr               - Pointer to response buffer to send
  at_state              - AT command state: command, online data, or online
                          command
  rearm_autodetect      - Flag to indicate if SIO autodetect mode should
                          rearmed when response is sent by SIO
---------------------------------------------------------------------------*/
typedef void (*dsat_routing_func_type)
(
  dsm_item_type *rsp_ptr,               
  const dsat_mode_enum_type at_state,
  const boolean rearm_autodetect         
);

typedef void (*dsat_fmting_func_type)
(
  dsm_item_type **in_ptr_ptr,         /* Raw response  in chained
                                         buffers                           */
  dsat_rsp_enum_type type_of_rsp,     /* Type of response data in
                                         in_ptr_ptr                        */
  boolean is_result_code,             /* Flag to indicate if response is a
                                         result code or information
                                         response                          */
  dsm_item_type **out_ptr_ptr,        /* Location to store formatted
                                         response, output will NOT be NULL
                                         terminated                        */
  dsat_result_enum_type result        /* Result code                       */
);

/* Multi Mode Support */

/* Enum for type of call */
typedef enum
{
  DSAT_DEFAULT_DIAL_TYPE = 0, /* Circuit Call */
  DSAT_PKT_DIAL_TYPE = 1,     /* Packet Call  */
  DSAT_MAX_DIAL_TYPE
}dsat_dial_str_enum_type;

/* Call abort callback */
typedef void (*dsat_call_abort_cb_type) ( void );

/* Incoming call callback */
typedef dsat_call_abort_cb_type (*dsat_incom_answer_cb_type) ( void );

/* Call hangup callback */
typedef void (*dsat_call_hangup_cb_type) ( void );

/* ATZ callback */
typedef void (*dsat_atz_cb_type) ( void );

/* enum that describes what type of dial modifier is present 
   in the dial string
*/
typedef enum
{
  DSAT_CID_DIAL_MODIFIER    = 0x0001,
  DSAT_CLI_DIAL_MODIFIER    = 0x0002,
  DSAT_CUG_DIAL_MODIFIER    = 0x0004,
  DSAT_CLI_AND_CUG_MODIFIER = 0x0008
} dsat_dial_modifier_e_type;

/* CLI Dial modifier */
typedef struct
{
  boolean                       cli_enabled;
} dsat_cli_modifier_type;

/* CUG Dial modifier */
typedef struct
{
  boolean                       cug_enabled;
  boolean                       cug_index_enabled;
  uint8                         cug_index_val;
  boolean                       suppress_pref_cug;
  boolean                       suppress_outgoing_access;
} dsat_cug_modifier_type;

/* CID Dial modifier */
typedef struct
{
  uint8                         cid_val;
} dsat_cid_modifier_type;

/* Structure that is sent to the mode specific call handlers */
typedef struct 
{
  boolean modifier_present;
  dsat_dial_modifier_e_type modifier;
  struct
  {
    dsat_cli_modifier_type  cli;
    dsat_cug_modifier_type  cug;
    dsat_cid_modifier_type  cid;
   } modifier_info;
} dsat_dial_modifier_info_type;

/* Call Handler prototype used by ATD to make a call */
typedef dsat_call_abort_cb_type (*dsat_dial_type)
(
  const byte                    *dial_string,    /* dial string         */
  boolean                       digit_mode,      /* non-digit or digits */
  dsat_dial_modifier_info_type  * modifiers      /* dial modifier info   */
);

/* Data structure that holds the function */ 
/* pointers for  ATA, ATH call backs      */
typedef struct
{
  dsat_call_hangup_cb_type  hangup_cb;
  dsat_atz_cb_type          atz_cb;
  dsat_incom_answer_cb_type answer_cb;
} dsat_dial_string_type;

/*---------------------------------------------------------------------------
     Specifies dial character validation result
---------------------------------------------------------------------------*/  
typedef enum
{
  DSAT_DIAL_EMPTY,          /*  Empty                    */
  DSAT_DIAL_DIGIT,          /*  Digits                   */
  DSAT_DIAL_ASCII,          /*  ASCII chars              */
  DSAT_DIAL_ASCII_ABC,      /*  ASCII A,B, or C          */
  DSAT_DIAL_ASCII_D,        /*  ASCII D                  */
  DSAT_DIAL_ASCII_E,        /*  ASCII E                  */
  DSAT_DIAL_ALLOWED,        /*  OTHER ALLOWED CHARACTERS */
  DSAT_DIAL_SEMICOLON,      /*  SEMICOLON                */
  DSAT_DIAL_STAR,           /*  ASTERIX                  */
  DSAT_DIAL_POUND,          /*  POUND                    */
  DSAT_DIAL_PLUS,           /*  PLUS                     */
  DSAT_DIAL_STRIP,          /*  Stripped chars           */
  DSAT_DIAL_IGNORE,         /*  Insignificant chars      */
  DSAT_DIAL_ERROR,          /*  has illegal chars        */
  DSAT_DIAL_MAX             /*  Internal use only        */
} dsat_dial_val_e_type;

/*===========================================================================
      Definitions of the parameters set by the AT commands
===========================================================================*/

/*---------------------------------------------------------------------------
  AT parameters common to all modes of operation.
---------------------------------------------------------------------------*/
typedef union 
{
  dsat_string_item_type * string_item;
  dsat_num_item_type    num_item;
} dsat_mixed_param_val_type;

extern dsat_num_item_type dsat_amp_c_val;
extern dsat_num_item_type dsat_amp_d_val;
extern dsat_num_item_type dsat_amp_f_val;
extern dsat_num_item_type dsat_flo_val;
extern dsat_num_item_type dsat_icf_val[];
extern dsat_num_item_type dsat_ifc_val[];
extern dsat_num_item_type dsat_ipr_val;
extern dsat_num_item_type dsat_last_ipr_val;
extern dsat_num_item_type dsat_e_val;
extern dsat_num_item_type dsat_l_val;
extern dsat_num_item_type dsat_m_val;
extern dsat_num_item_type dsat_o_val;
extern dsat_num_item_type dsat_q_val;
extern dsat_num_item_type dsat_s0_val;
extern dsat_num_item_type dsat_s10_val;
extern dsat_num_item_type dsat_s11_val;
extern dsat_num_item_type dsat_s3_val;
extern dsat_num_item_type dsat_s4_val;
extern dsat_num_item_type dsat_s5_val;
extern dsat_num_item_type dsat_s6_val;
extern dsat_num_item_type dsat_s7_val;
extern dsat_num_item_type dsat_s8_val;
extern dsat_num_item_type dsat_s9_val;
extern dsat_num_item_type dsat_v_val;
extern dsat_num_item_type dsat_x_val;
extern dsat_num_item_type dsat_z_val;

extern uint32 dsat_qcdns_val[];

extern dsat_num_item_type dsat_qcdmr_val;

/*---------------------------------------------------------------------------
  AT parameters specific to ETSI mode of operation.
---------------------------------------------------------------------------*/
#if defined(FEATURE_WCDMA) || defined(FEATURE_GSM)
#error code not present
#endif /* defined(FEATURE_WCDMA) || defined(FEATURE_GSM) */

#if defined(FEATURE_DATA_GCSD) || defined(FEATURE_DATA_WCDMA_CS)
extern dsat_num_item_type dsat_ds_val[];
extern dsat_num_item_type dsat_crlp_val[];
#endif /* defined(FEATURE_DATA_GCSD) || defined(FEATURE_DATA_WCDMA_CS) */

extern dsat_num_item_type dsat_fclass_val;

#ifdef FEATURE_DATA_GCSD_FAX
#error code not present
#endif  /* FEATURE_DATA_GCSD_FAX */

#if defined(FEATURE_DSAT_V80) && defined(FEATURE_WCDMA)
#error code not present
#endif /* defined(FEATURE_DSAT_V80) && defined(FEATURE_WCDMA) */

/*===========================================================================

                      PUBLIC FUNCTION DECLARATIONS

===========================================================================*/


/*===========================================================================

FUNCTION DSAT_PROCESS_SIO_COMMAND

DESCRIPTION
  This function builds the AT command line from data obtained from the 
  watermark item passed in. The command line begins with the "AT"
  characters (either case or mixed case, but with nothing in between), 
  and is terminated with the end of line character stored in modem 
  register S3.  The backspace character (in modem register S5) is processed.

  When a complete line has been accumulated, ds_atcop( ) is called.
  The line will have been edited per the backspace (above), and the AT
  removed.  The end of line character will have been replaced with the null
  character, '\0'.  Any spaces will still be present. The eighth bit of all 
  characters will have been set to zero.

  If the character combination, "A/" or "a/" is encountered while looking
  for the beginning of a command line, the previous command line, if any,
  will be immediately passed to ds_atcop( ).

  If the command line buffer overflows, the data will be discarded and
  an error flag set.  The error will be reported when the end of line
  character in S3 is encountered.

  Input data will be echoed to the serial port.  Data is echoed at the end
  of each input data item, when a small data item (used for echo output) is
  filled, or when the S3 character is found, or the buffer overflows,
  whichever comes first.  The echo is controlled by the 'ATE' flag.

DEPENDENCIES
  None

RETURN VALUE
  Abort call result; TRUE if call should be aborted, FALSE otherwise.

SIDE EFFECTS
  Buffer accumulates data until a full line is found.

  See also ds_atcop which has many side effects.

===========================================================================*/
extern boolean dsat_process_sio_command
(
  dsat_mode_enum_type at_state,     /* Command, online data, or online
                                           command state */
  dsm_watermark_type *wm_item_ptr       /* Pointer to watermark item to
                                           retrieve command line data from */
);


/*===========================================================================

FUNCTION  DSAT_INIT 

DESCRIPTION
  Initializes ATCOP queues and internal variables, as well as all AT
  parameters to their default values.

DEPENDENCIES
  This function must be called once on data services task startup.

RETURN VALUE
  Signal mask defining REX signals the AT command processor is waiting
  for.

SIDE EFFECTS
  Initializes ATCOP internal variables and queues.

===========================================================================*/
extern rex_sigs_type dsat_init( void );


/*===========================================================================

FUNCTION DSAT_INIT_SERVICE_MODE

DESCRIPTION
  Initializes the AT command processor to new operating mode.  AT
  command processing functions are selected from table based on
  new operating mode.

DEPENDENCIES
  Must be called each time operating service mode changes.

RETURN VALUE
  None

SIDE EFFECTS
  None

===========================================================================*/
extern void dsat_init_service_mode
(
  sys_sys_mode_e_type service_mode   /* Service mode - GSM, WCDMA, CDMA ... */
);


/*===========================================================================

FUNCTION DSAT_PROCESS_CMD_LINE

DESCRIPTION
  This function parses the AT command line, which may contain multiple
  commands, and processes the commands. Command line does not include
  leading "AT" and is NULL terminated.  Command processing is dependent on
  AT mode: command, online data, or online commmand.  

  Command response and command line result codes are generated.

DEPENDENCIES
  None

RETURN VALUE
  None

SIDE EFFECTS
  Token queue is emptied.

===========================================================================*/
extern void dsat_process_cmd_line
(
  dsat_mode_enum_type at_state,     /* Command, online data, or online
                                           command */
  byte *cmd_line_ptr                    /* Pointer to NULL terminated
                                           command line. */
);


/*===========================================================================

FUNCTION DSAT_PROCESS_ASYNC_CMD

DESCRIPTION
  Asynchronous ATCOP command handler.  Processes asynchronous ATCOP commands
  received by data services task dispatcher.  Function looks up commands in
  mode specific command table.  Command processing function corresponding to
  current command and operating mode is called from table. 

DEPENDENCIES
  None

RETURN VALUE
  None

SIDE EFFECTS
  None

===========================================================================*/
extern void dsat_process_async_cmd
(
  ds_cmd_type *cmd_ptr          /* Pointer to command */
);


/*===========================================================================

FUNCTION DSAT_PROCESS_ASYNC_SIGNAL

DESCRIPTION
  Asynchronous ATCOP REX signal handler.  Processes asynchronous ATCOP
  signals received by data services task dispatcher.  Function looks up
  signals in mode specific signal table.  Signal processing function
  corresponding to current signal and operating mode is called from table. 

DEPENDENCIES
  None

RETURN VALUE
  None

SIDE EFFECTS
  None

===========================================================================*/
extern void dsat_process_async_signal
(
  rex_sigs_type set_sigs            /* ATCOP signals that are set */
);


/*===========================================================================

FUNCTION DSAT_NV_SYNC

DESCRIPTION
  This function initializes from NV the ATCOP defaults for
  those parameters stored in NV.

DEPENDENCIES
  None

RETURN VALUE
  None

SIDE EFFECTS
  None

===========================================================================*/
extern void dsat_nv_sync( void );


/*===========================================================================

FUNCTION DSAT_REG_RSP_ROUTER

DESCRIPTION
  This function registers a function that provides AT response routing
  based on operating service mode and AT command state: command mode,
  online data, or online command mode.  Registered function routes AT
  command response to either SIO or PS protocol stack.  

  If service type is IS-707 related and AT operating mode is online data
  mode, responses are routed to PS protocol stack, otherwise they are
  routed to SIO.  This provides support for IS-707 command reflection in
  future releases and makes it independent of ATCOP operation.  If no
  routing function is registered, response is sent directly to SIO.

  The response routing function may be deregistered by calling this
  function with the function pointer equal to NULL.

DEPENDENCIES
  None

RETURN VALUE
  None

SIDE EFFECTS
  None

===========================================================================*/
extern void dsat_reg_rsp_router
(
  dsat_routing_func_type func_ptr   /* Function providing mode specific
                                       AT command response routing or
                                       NULL to deregister routing
                                       function. */
);


/*===========================================================================

FUNCTION DSAT_REG_RSP_FMTER

DESCRIPTION
  This function registers a function that provides AT response formating
  based on operating service mode and AT command state: command mode,
  online data, or online command mode.

  If service type is IS-707 related and AT operating mode is online data
  mode, responses to reflected AT commands have to be formated with
  EIA-617 codes. So the default fmt_response function cannot be used and
  the 707 mode-specific handler should use this function to register a
  function that does appropriate formatting.

  The response formating function may be deregistered by calling this
  function with the function pointer equal to NULL.

DEPENDENCIES
  None

RETURN VALUE
  None

SIDE EFFECTS
  None

===========================================================================*/
extern void dsat_reg_rsp_fmter
(
  dsat_fmting_func_type func_ptr    /* Function providing mode specific
                                       AT command response formating or
                                       NULL to deregister function         */
);


/*===========================================================================

FUNCTION DSAT_FMT_RESPONSE

DESCRIPTION
  This function formats the response according whether or not
  the mode is verbose and whether or not the response is a result
  code or informational text.  

  The dsm item passed in as in_ptr_ptr can contain chained dsm items with
  the dsm "used" value defining the length of each item.  These chained
  items are assumed to contain a NULL terminated string, which will be
  formatted up until the dsm "used" value is reached for each buffer
  or a NULL character is encountered, whichever occurs first.  *in_ptr_ptr
  will be returned as NULL to indicate the dsm input items have been freed.

  The type of data passed in the dsm item pointed to by in_ptr_ptr is
  specified by the type_of_rsp parameter.  This controls response
  formatting and identifies if header, trailer, both header and trailer,
  or no header and trailer should be added to the response.  This allows
  formatting of partial responses, as oppossed to only supporting
  formatting of complete responses, to reduce DSM buffer usage. See enum
  type definition for details.

  The dsm item passed in as out_ptr_ptr is filled with the formatted
  response.  If there is not enough room in this dsm item, the full item
  is queued to send as a command line response and additional dsm
  items are allocated, with out_ptr_ptr pointing to the last item
  allocated.  out_ptr_ptr may be returned as NULL, if no more dsm
  buffers are available. 

DEPENDENCIES
  None

RETURN VALUE
  None

SIDE EFFECTS
  None

===========================================================================*/
extern void dsat_fmt_response
(
  dsm_item_type **in_ptr_ptr,         /* Response to format in chained
                                         buffers                           */
  dsat_rsp_enum_type type_of_rsp,     /* Type of response data in 
                                         in_ptr_ptr                        */
  boolean result_code,                /* Flag to indicate if response is a
                                         result code or information
                                         response                          */
  dsm_item_type **out_ptr_ptr         /* Location to store formatted
                                         response, output will NOT be NULL
                                         terminated                        */
);


/*===========================================================================

FUNCTION  DSAT_SEND_RESULT_CODE

DESCRIPTION
  Sends ITU-T V.25 ter basic code or mode specific extended result code
  to TE or PS protocol stack based on response routing provided if ATQ
  is 0. Also, modifies ATCOP internal state variables.

  If AT+CRC is 0, only the basic result codes are returned.
  The extended RING codes are translated to the basic code.
  Basic result codes are returned if ATQ is 0.

  Extended cellular result codes are returned if AT+CRC is 1.

DEPENDENCIES
  None
  
RETURN VALUE
  None
  
SIDE EFFECTS
  May modify ATCOP internal state variables and SIO preprocessor mode.
===========================================================================*/
extern void dsat_send_result_code
(
  dsat_result_enum_type result_code         /* Command result code */
);

#if defined(FEATURE_GSM)
#error code not present
#endif /* defined(FEATURE_GSM) */

#if defined(FEATURE_WCDMA) || defined(FEATURE_GSM)
#error code not present
#endif /* defined(FEATURE_WCDMA) || defined(FEATURE_GSM) */


/*===========================================================================

FUNCTION  DSAT_SEND_CME_ERROR 

DESCRIPTION
  Sends Mobile Equipment error to TE or PS protocol
  stack based on response routing.  The Mobile Equipment error 
  code and error text are defined in Section 9.2 of 3GPP TS 27.007

  If AT+CMEE is 0, +CME ERROR : <err> result code is disabled
                   ERROR is used
  If AT+CMEE is 1, +CME ERROR : <err> result code is enabled
                   to used numeric err value
  If AT+CMEE is 2, +CME ERROR : <err> result code is enabled
                   to used verbose text err value

DEPENDENCIES
  None

RETURN VALUE
  None

SIDE EFFECTS
  None
===========================================================================*/
extern dsat_result_enum_type dsat_send_cme_error
(
  dsat_cme_error_e_type error_code
);


/*===========================================================================

FUNCTION DSAT_IS_STATUS

DESCRIPTION
  This function returns TRUE if the argument string begins with
  the numeric character (1-8)  or the word corresponding to the
  result specified by the second arg.  Trailing characters are ignored.
  Other wise if returns FALSE.

DEPENDENCIES
  None

RETURN VALUE
  See description.

SIDE EFFECTS
  None

===========================================================================*/
extern boolean dsat_is_status
(
  byte *a_ptr,
  dsat_result_enum_type result
);



#ifdef FEATURE_ETSI_SMS
#error code not present
#endif /* FEATURE_ETSI_SMS */

#ifdef FEATURE_DSAT_KEYPAD_EMULATION
/*===========================================================================

FUNCTION    DSAT_FLUSH_ME_MSG_QUEUE

DESCRIPTION
  Flushes buffer of unsolicited result codes and sends responses
  to TE. Notifications are buffered and sent based on AT +CMER 
  parameter settings.
  
DEPENDENCIES
  None

RETURN VALUE
  Returns an enum that describes the result of the execution.
  Possible values:
    DSAT_ERROR : if there was any problem in execution
    DSAT_OK : if it is a success.

SIDE EFFECTS
  None
===========================================================================*/
dsat_result_enum_type dsat_flush_me_msg_queue ( void );
#endif /* FEATURE_DSAT_KEYPAD_EMULATION */

#ifdef FEATURE_DSAT_FLOW_CONTROL

/*===========================================================================

FUNCTION  DSAT_TO_SIO_LOWATER_CB

DESCRIPTION
  This is the call back function when ds_to_sio watermark hits low.

  It sets dsati_tx_wm_high flag to be FALSE and inform ds task with
  DS_AT_TO_SIO_LOW_CMD cmd.

DEPENDENCIES
  None

RETURN VALUE
  none.

SIDE EFFECTS
  None
===========================================================================*/
void dsat_to_sio_lowater_cb ( void );


/*===========================================================================

FUNCTION  DSAT_TO_SIO_HIWATER_CB

DESCRIPTION
  This is the call back function when ds_to_sio watermark hits high.

  It sets dsati_tx_wm_high flag to be TRUE.

DEPENDENCIES
  None

RETURN VALUE
  none.

SIDE EFFECTS
  None
===========================================================================*/
void dsat_to_sio_hiwater_cb ( void );

#endif /* FEATURE_DSAT_FLOW_CONTROL */

/*===========================================================================

FUNCTION DSAT_REG_ATZ_CB

DESCRIPTION
  This function is used to register a call back function for ATZ.

DEPENDENCIES
  None

RETURN VALUE
  None

SIDE EFFECTS
  None

===========================================================================*/
void dsat_reg_atz_cb
(
  dsat_atz_cb_type atz_cb                                 /* ATZ call back */
);

/*===========================================================================

FUNCTION DSAT_SIO_IS_BUSY

DESCRIPTION
  This function is used by SIOLib to tell ATCoP that SIO is busy now and 
  Mode Specific Handlers are in charge of SIO.

  This is called when Mode Specific Handlers register their SIO Handlers.

DEPENDENCIES
  None

RETURN VALUE
  None

SIDE EFFECTS
  ATCOP stops sending the AT command results to the TE. It will resume
  its normal operatioon only when dsat_sio_is_free() is called.

===========================================================================*/
void dsat_sio_is_busy ( void );

/*===========================================================================

FUNCTION DSAT_SIO_IS_FREE

DESCRIPTION
  This function is used by SIOLib to tell ATCoP that SIO is free now. ATCoP
  Can now resume its normal operation.

  This is called when Mode Specific Handlers de-register their SIO Handlers.
  i.e. when the serial port is no longer in a call.

DEPENDENCIES
  None

RETURN VALUE
  None

SIDE EFFECTS
  In ETSI mode, the queued SMS indications are flushed.

===========================================================================*/
void dsat_sio_is_free ( void );

/*===========================================================================

FUNCTION DSAT_REGISTER_HANDLERS

DESCRIPTION
  This function is used to register a call back functions.
  It is used to register call back funtions for ATA and ATH.

  The call back handler can be NULL. If it is not an incoming call, 
  then ATA handler will be NULL. 

DEPENDENCIES
  None

RETURN VALUE
  None

SIDE EFFECTS
  None

===========================================================================*/
void dsat_register_handlers
(
  dsat_incom_answer_cb_type  call_answer_cb, /* Answer call back       */
  dsat_call_hangup_cb_type call_hangup_cb    /* Hangup call back */
);

/*===========================================================================

FUNCTION DSAT_DEREGISTER_HANDLERS

DESCRIPTION
  This function is used to deregister the call back functions.
  It resets all the registered handlers to NULL.

DEPENDENCIES
  None

RETURN VALUE
  None

SIDE EFFECTS
  None

===========================================================================*/
void dsat_deregister_handlers ( void );

/*===========================================================================

FUNCTION DSAT_INIT_CB_HANDLERS

DESCRIPTION
  Initialize all the registered call handlers to NULL
  Called from dsat_init() after every reset.

DEPENDENCIES
  None

RETURN VALUE
  None

SIDE EFFECTS
  None

===========================================================================*/
void dsat_init_cb_handlers(void);

/*===========================================================================

FUNCTION DSAT_PROC_DIAL_STR

DESCRIPTION
  Parses a dial string, looking for allowed non-digits, and
  illegal non-digits.  Digits, and allowed non-digits are copied
  to the output.  Illegal characters terminate processing.  All
  other characters are ignore.  Upper and lower case are treated
  as being the same.

  Since the output will be no longer than the input, the output
  string may be the same as the input string.

DEPENDENCIES
  None

RETURN VALUE
  Returns enum value indicating dial string content or error

SIDE EFFECTS
  None

===========================================================================*/
dsat_dial_val_e_type dsat_proc_dial_str
(
  byte * in_ptr,                /*  Input string, null terminated   */
  byte * out_ptr                /*  Output string, null terminated  */
);
#else
#include "dsatprofile.h"
#include "dstask.h"
#endif /* FEATURE_DATA_STRIP_ATCOP */

/*===========================================================================

FUNCTION DSAT_CHANGE_BAUD

DESCRIPTION
  This function is called by other modules to write the value to the current 
  baud rate variable, ds_atcop_ipr_val. Whenever the baud rate is changed
  it should also be entered into the NV if FEATURE_DS_DEFAULT_BITRATE_NV
  is defined. So this function also writes into the NV if that feature is
  defined.

  The second parameter in this function is a boolean type. It informs 
  the function if the Baud rate should be changed NOW or later (later
  implies, after sending acknowledgement). This is needed because, when 
  UI changes the baud rate, the baud rate should take effect immediately. 
  But when an AT command changes the baud rate, the baud rate should be 
  changed after an "OK" is sent to the TE2. 

DEPENDENCIES
  This code does not check for auto baud enabled or not. So, it is the 
  responsibility of the calling function to ascertain that if 
  FEATURE_AUTOBAUD is not enabled then value 0 should not be passed.

RETURN VALUE
  None

SIDE EFFECTS
  Modifies the value of the dsat_ipr_val and its corresponding value in 
  the NV, ds_default_baud in the NV.    
===========================================================================*/
extern void dsat_change_baud
(
    sio_bitrate_type new_baud_rate,     /* Requested baud rate   */
    boolean change_baud_now             /* Immediate or post ack */
);

#endif /* FEATURE_DATA */

#endif /* DSAT_H */

