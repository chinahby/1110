#ifndef DSATSMSI_H
#define DSATSMSI_H

/*====*====*====*====*====*====*====*====*====*====*====*====*====*====*====*

                D A T A   S E R V I C E S

                A T   C O M M A N D   
                
                ( S M S  C O M M A N D S )
                
                P R O C E S S I N G

                I N T E R N A L   H E A D E R   F I L E


GENERAL DESCRIPTION
  This file contains the definitions of data structures, defined and
  enumerated constants and function prototypes required for the
  executing the sms commands internally. 

EXTERNALIZED FUNCTIONS

INITIALIZATION AND SEQUENCING REQUIREMENTS

   Copyright (c) 2009 by QUALCOMM Incorporated.
   All Rights Reserved.
   Qualcomm Confidential and Proprietary.
*====*====*====*====*====*====*====*====*====*====*====*====*====*====*====*/


/*===========================================================================

                        EDIT HISTORY FOR MODULE

  This section contains comments describing changes made to the module.
  Notice that changes are listed in reverse chronological order.

  $Header: //source/qcom/qct/modem/data/common/commonatcop/main/lite/inc/dsatsmsi.h#1 $ $DateTime: 2009/01/23 10:36:19 $ $Author: nchagant $

when       who     what, where, why
--------   ---     ----------------------------------------------------------
01/21/09   nc      Added support for SMS Commands.

===========================================================================*/
#include "comdef.h"
#include "customer.h"

#ifdef FEATURE_DATA
#if defined(FEATURE_ETSI_SMS) || defined(FEATURE_CDMA_SMS)
#include "dsatsms.h"
#include "dsati.h" /* need this for dsati_cms_err_e_type visibility */
#include "wms.h"

/*===========================================================================

                     INCLUDE FILES FOR MODULE

===========================================================================*/

/*-------------------------------------------------------------------------
  Enum used to find out what command is being processed currently
---------------------------------------------------------------------------*/
typedef enum
{
#ifdef FEATURE_ETSI_SMS_PS
#error code not present
#endif /* FEATURE_ETSI_SMS_PS */
  SMS_CMGS,
  SMS_CMGW,
  SMS_CMGC,
  SMS_CMSS,
  SMS_CMGD,
  SMS_CMGR,
  SMS_CMGL,
  SMS_CPMS,
  SMS_CNMI,
  SMS_CNMA,
  SMS_CSCB,
  SMS_CMMS,
#ifdef FEATURE_ETSI_SMS_TEMPLATE
#error code not present
#endif /* FEATURE_ETSI_SMS_TEMPLATE */
  SMS_CSAS_CSCB,
  SMS_CRES_CSCB,
  SMS_NONE
} sms_cmd_name_e_type;

/*-------------------------------------------------------------------------
  Structure used in processing +CPMS command
---------------------------------------------------------------------------*/
typedef enum {
  CPMS_NONE = -1,
  CPMS_MEM1 = 0,
  CPMS_MEM2 = 1,
  CPMS_MEM3 = 2,
  CPMS_MAX  = 3
} cpms_mem_e_type;

typedef enum {
  SMS_EVENT_NONE = 0,
  SMS_CFG_EVENT_GW_READY,
  SMS_CFG_EVENT_ROUTES,
  SMS_CFG_EVENT_MEMORY_STATUS,
  SMS_CFG_EVENT_MESSAGE_LIST,
  SMS_CFG_EVENT_LINK_CONTROL,
#ifdef FEATURE_DSAT_SIRIUS
#error code not present
#endif /* FEATURE_DSAT_SIRIUS */

#ifdef FEATURE_ETSI_SMS_PS
#error code not present
#endif /* FEATURE_ETSI_SMS_PS */

#ifdef FEATURE_ETSI_SMS_CB
#error code not present
#endif /* FEATURE_ETSI_SMS_CB */

  SMS_MSG_EVENT_SEND,
  SMS_MSG_EVENT_ACK,
  SMS_MSG_EVENT_READ,
  SMS_MSG_EVENT_WRITE,
  SMS_MSG_EVENT_DELETE,
  SMS_MSG_EVENT_MODIFY_TAG,
  SMS_MSG_EVENT_READ_TEMPLATE,
  SMS_MSG_EVENT_WRITE_TEMPLATE,
  SMS_MSG_EVENT_DELETE_TEMPLATE,
  SMS_MSG_EVENT_RECEIVED_MESSAGE,
  SMS_MSG_EVENT_SUBMIT_REPORT,
  SMS_MSG_EVENT_STATUS_REPORT,
  SMS_MSG_EVENT_MT_MESSAGE_ERROR,
  SMS_MSG_EVENT_READ_STS_REPORT,
  SMS_MSG_EVENT_WRITE_STS_REPORT,
  SMS_EVENT_MAX
}sms_event_e_type;

/* It tells the event call back function what memory the returned
   status is about, and if it is the last mem of cpms set command */
typedef struct {
  cpms_mem_e_type mem;        /* current mem */
  boolean         is_read;    /* TRUE if the call back function 
                                         is called for a cpms read.
                                 FALSE if it is for a cpms write. */
  boolean       mem3_changed; /* a flag to tell if cpms cmd
                                 set a new mem3 type 
                                 If mem3 changed, we need to
                                 set a new route. */
} cpms_mem_status_data_s_type;

/*-------------------------------------------------------------------------
  Enum used by +CMMS command 
-------------------------------------------------------------------------*/
typedef enum
{
  SMS_LINK_CONTROL_DISABLED,
  SMS_LINK_CONTROL_ENABLED_ONE,
  SMS_LINK_CONTROL_ENABLED_ALL,
  SMS_LINK_CONTROL_MAX  /* Internal use only */
} sms_link_control_e_type;


/*-------------------------------------------------------------------------
  Enum used to find out what incoming unsolicited response is being queued
---------------------------------------------------------------------------*/
typedef enum
{
  MT_DELIVER,
  MT_STATUS_REPORT
} sms_mt_msg_e_type;

/*-------------------------------------------------------------------------
  SMS Mobile Terminated message queue node definition
---------------------------------------------------------------------------*/ 
typedef struct
{
 q_link_type               link;       /* Queue link type      */
 sms_mt_msg_e_type         msg_type;   /* Type of message      */
 wms_message_index_type    index;      /* Index Number         */
 wms_memory_store_e_type   mem_store;  /* Type of Memory Store */
 wms_transaction_id_type   tid;        /* Transaction ID       */
}sms_mt_msg_type;

/*----------------------------------------------------------------------------
  Data structure that is used to know the state of the cmd (w/multiple events)
----------------------------------------------------------------------------*/
typedef struct
{
  byte                      num_of_stat;        /* "all", 0 or 4          */
  wms_message_tag_e_type    stat_list[4];       /* list for present cmd   */
  byte                      present_stat;       /* Present stat           */ 
  uint16                    num_of_indices;     /* Total indices          */
  word                      present_index;      /* present index          */
  boolean                   is_test;            /* True if a TEST cmd     */
  boolean                   one_index;          /* Just one index         */
  boolean                   list_begin;         /* ind. beginning of list */ 
  wms_message_index_type    indices[WMS_MESSAGE_LIST_MAX];
} sms_cmd_sm_type;

/*-------------------------------------------------------------------------
  Types of sms message format mode.  
  Refer to 3GPP TS27.005 3.2.3 Message Format +CMGF
---------------------------------------------------------------------------*/
typedef enum
{
  DSATSMS_TEXT_MODE,
  DSATSMS_PDU_MODE
} dsatsms_fmt_mode_e_type;

#define MT_VAL_0 0

#ifdef FEATURE_ETSI_NOUI
#error code not present
#endif /* FEATURE_ETSI_NOUI */

#define DS_VAL_0 0

/* Data structure to hold SMS Message List     */
/* Used in receiving the information from      */
/* Msg list event (WMS_CFG_EVENT_MESSAGE_LIST) */
typedef struct {
  wms_memory_store_e_type     mem_store; /* which memory store */
  wms_message_tag_e_type      tag;       /* what msgs are included in list */
  uint8                       len;       /* how many indices returned */
  wms_message_index_type      indices[WMS_MESSAGE_LIST_MAX];
}sms_msg_list_s_type;

/* Data structure to hold SMS Message status information */
/* Used in receiving just the info we need (filter)      */
typedef struct
{
  void                        *user_data;
  wms_client_msg_hdr_s_type   msg_hdr;
  
  union sms_msg_info_u
  {
    wms_gw_message_s_type       gw_message;
    wms_cdma_message_s_type     cdma_message;
    wms_gw_template_s_type      gw_template;
  }u;
} sms_msg_status_info_s_type;

/* Data structure to hold incoming SMS Message (MT)  */
/* Used in receiving just the info we need (filter)  */
typedef struct
{
  wms_client_msg_hdr_s_type   msg_hdr;
  union sms_msg_mt_info_u
  {
    wms_gw_message_s_type       gw_message;
    wms_cdma_message_s_type     cdma_message;
  }u;
  
  wms_transaction_id_type     transaction_id;
} sms_mt_msg_info_s_type;

/* Data structure to hold incoming SMS Message (MT)  */
/* Used in receiving just the info we need (filter)  */
typedef struct{
  uint16                         sm_len;
  uint8                          sm_data[WMS_MAX_LEN];
  wms_timestamp_s_type           timestamp;                
} sms_submit_report_ack_type;

/* Data structure to hold Submit Report for MT Msg   */
/* Used in receiving just the info we need (filter)  */
typedef struct
{
  wms_report_status_e_type    report_status;
  wms_gw_tpdu_type_e_type     tpdu_type;
  wms_cdma_tl_status_e_type   tl_status;

  union sms_submit_report_info_u
  {
    sms_submit_report_ack_type    ack;
    wms_tp_cause_e_type           tp_cause;
  } u;
} sms_submit_report_s_type;

/* Data structure to SMS Message list */
typedef struct{
  wms_memory_store_e_type     mem_store; /* which memory store */
  wms_message_tag_e_type      tag;       /* what msgs are included in list */
  /* These are actually uint32's in WMS code but they currently use only 500
     SMSes and increasing them to uint32's unneccesarily increases the size
     of our variables */
  uint16                      len;       /* how many indices returned */
  uint16                      indices[WMS_MESSAGE_LIST_MAX];
} sms_message_list_s_type;

typedef struct{
  sms_event_e_type                  event;
  union sms_info {
    sms_msg_status_info_s_type      msg_status;
    sms_mt_msg_info_s_type          mt_msg;
    sms_submit_report_s_type        submit_report;
    sms_message_list_s_type         msg_list;
    wms_report_status_e_type        err_report;
    wms_memory_status_s_type        mem_status;
    wms_gw_domain_pref_e_type       gw_domain;
    wms_cfg_link_control_mode_e_type link_mode;
#ifdef FEATURE_DSAT_SIRIUS
#error code not present
#endif /* FEATURE_DSAT_SIRIUS */
  } u;
} sms_event_info_s_type;

typedef dsat_result_enum_type (* sms_msg_event_f_type)
(
  dsat_mode_enum_type    mode,         /* AT command mode   */   
  sms_event_info_s_type  *event_info   /* Event info ptr    */
);

/* The msg event handler table type */
typedef struct {
  sms_event_e_type            event;
  sms_msg_event_f_type        handler;
} msg_handler_tab_type;

/*-------------------------------------------------------------------------
            PUBLIC FUNCTION DECLARATIONS
-------------------------------------------------------------------------*/
/*===========================================================================

FUNCTION    DSATSMSI_CMD_CB_FUNC

DESCRIPTION
  Process the WMS command call back status.
  
DEPENDENCIES
  None

RETURN VALUE
  None

SIDE EFFECTS
  None

===========================================================================*/
void dsatsmsi_cmd_cb_func
(
  wms_cmd_id_e_type     cmd,   /* Cmd ID            */
  void                    *data, /* User data pointer */
  wms_cmd_err_e_type    status /* Command Status    */
);

/*===========================================================================

FUNCTION    DSATSMSI_DELETE_MESSAGES

DESCRIPTION
  This function is used for +CMGD/$QCMGD command when a <delflag> is present.
  Tries to get the list of message with that particular <delflag> in memory.
  
DEPENDENCIES
  None

RETURN VALUE
  None

SIDE EFFECTS
  None

===========================================================================*/
dsat_result_enum_type dsatsmsi_delete_messages
(
  const byte *  del_flag                  /* pointer to the delete flag */
);

/*===========================================================================

FUNCTION    DSATSMSI_GET_MEM_VAL

DESCRIPTION
  Returns the current sms memory storage setting.
  It returns dsat_cpms_val[mem_index]/
             dsat707sms_qcpms_val[mem_index]
  depending on modes.

DEPENDENCIES
  None

RETURN VALUE
  Returns a dsat_num_item_type.

SIDE EFFECTS
  None

===========================================================================*/
dsat_num_item_type dsatsmsi_get_mem_val
(
  cpms_mem_e_type mem_index   /* which memory (<mem[1-3]) setting we are 
                                 looking for                             */
);

/*===========================================================================

FUNCTION    DSATSMSI_GET_MEM_LIST

DESCRIPTION
  Returns the current sms memory storage setting.
  It returns dsat_cpms_list[mem_index]/
             dsat707sms_qcpms_list[mem_index]
  depending on modes.

DEPENDENCIES
  None

RETURN VALUE
  Returns a def_list_type array.

SIDE EFFECTS
  None

===========================================================================*/
const def_list_type * dsatsmsi_get_mem_list
(
  cpms_mem_e_type mem_index   /* which memory (<mem[1-3]) setting we are 
                                 looking for                             */
);

/*===========================================================================

FUNCTION    DSATETSISMSI_CMS_ERROR

DESCRIPTION
  Fills a response buffer with a +CMS ERROR.
  
DEPENDENCIES
  None

RETURN VALUE
  returns number of bytes that are used in filling up the response buffer.

SIDE EFFECTS
  None

===========================================================================*/
unsigned int dsatsmsi_cms_error
(
  const dsm_item_type *res_buff_ptr,             /*  Place to put response       */
  int                  error                     /* Error Number             */
);

/*===========================================================================

FUNCTION    DSATSMSI_STRIP_QUOTES

DESCRIPTION
  Strips out quotes from the string that is wrapped in quotes.
  Resultant string will be placed in the out pointer.
  Gives an error if the string does not begin or end with a quote
  
DEPENDENCIES
  None

RETURN VALUE
  Boolean : 
    TRUE : if successful in stripping the quotes out
    FALSE : if there is any error

SIDE EFFECTS
  None

===========================================================================*/
boolean dsatsmsi_strip_quotes
(
  const byte * in_ptr,   /* Pointer to the string to be processed  */
  byte       * out_ptr   /* Pointer to the resultant string buffer */
);

/*===========================================================================

FUNCTION    DSATSMSI_MEMORY_LOOKUP

DESCRIPTION
  Converts the type of preferred memory storage into a required enum.
  
DEPENDENCIES
  None

RETURN VALUE
  returns the memory type enum:
  WMS_MEMORY_STORE_RAM
  WMS_MEMORY_STORE_SIM
  WMS_MEMORY_STORE_NV

SIDE EFFECTS
  None

===========================================================================*/
wms_memory_store_e_type dsatsmsi_memory_lookup
( 
  int mem_val,                   /* value of the mem index to be mapped */ 
  cpms_mem_e_type mem_type       /* which memory (<mem[1-3]>) to mapped */
);

/*===========================================================================

FUNCTION DSATSMSI_DELETE_MSG_LIST

DESCRIPTION
  Local function that is used to delete a message from the list of indices.
  Handles one list or lists of multiple <stat>s

DEPENDENCIES
  none

RETURN VALUE
  none

SIDE EFFECTS

===========================================================================*/
dsat_result_enum_type dsatsmsi_delete_msg_list(void );

/*===========================================================================

FUNCTION    DSATSMSI_TAG_LOOKUP

DESCRIPTION
  Converts the message TAG type (wms_message_tag_e_type) to a string on
  text mode or to an integer on a pdu mode and writes the result into the
  result buffer
  
DEPENDENCIES
  None

RETURN VALUE
  returns a pointer to the end of result buffer 

SIDE EFFECTS
  None

===========================================================================*/
byte * dsatsmsi_tag_lookup
( 
  wms_message_tag_e_type tag,   /* TAG enum             */
  boolean mode,                 /* mode: PDU or TEXT    */
  byte * res_ptr                /* ptr to result buffer */
);

/*===========================================================================

FUNCTION    DSATSMSI_STR_TO_ADDR

DESCRIPTION
  Checks to see if <toda> field is specified by the user.
  If not, parses the <da> string for address length, type of number and 
  numbering plan.
  If <toda> filed is specified, <toda> is split into type of number
  and numberign plan.
  
DEPENDENCIES
  None

RETURN VALUE
  return a boolean to indicate success or failure in the operation.

SIDE EFFECTS
  None

===========================================================================*/
boolean dsatsmsi_str_to_addr
(
  wms_address_s_type      * addr,  /* Pointer to address structure to 
                                      be filled                             */
  byte                    * da,    /* Pointer to destination address        */
  const byte              * toda,  /* Pointer to type of address            */
  dsatsms_fmt_mode_e_type sms_mode /* SMS mode Enum                         */
 );

/*===========================================================================

FUNCTION DSATSMSI_ITOA

DESCRIPTION
  This function converts an integer to ASCII, using a specified
  radix. The resulting string is null terminated.

DEPENDENCIES
  The function does not work correctly if r < 2 or r > 36.
  (r is the radix argument.)

  The length of the returned string depends on the value and the
  radix.  The worst case is the value 0xFFFF and a radix of 2,
  which returns 17 bytes (16 one's and the null terminator.)
  The array pointed to by 's' must be large enough for the returned
  string.

RETURN VALUE
  A pointer to the null terminator is returned.

SIDE EFFECTS

===========================================================================*/
byte * dsatsmsi_itoa
(
  uint32 v,       /*  Value to convert to ASCII     */
  byte *s,        /*  Pointer to string for result  */
  word r          /*  Conversion radix              */
);

/*===========================================================================

FUNCTION    DSATSMSI_SEND_ASYNC_INDICATION_TO_TE

DESCRIPTION
  If ATCOP get an unsolicited asynchronous event, This function formats 
  the data that needs to be sent to TE. It handles both SMS-DELIVERs and
  SMS-STATUS-REPORTs.
  
DEPENDENCIES
  None

RETURN VALUE
  Returns number of bytes that are used in formatting the result buffer

SIDE EFFECTS
  None

===========================================================================*/
void dsatsmsi_send_async_indication_to_te
(
  sms_mt_msg_e_type       msg_type,   /* Type of message       */
  wms_message_index_type  msg_index,  /* Index Number          */
  wms_memory_store_e_type mem_store   /* Type of Memory Store  */
);

#ifdef FEATURE_ETSI_SMS 
#error code not present
#endif /* FEATURE_ETSI_SMS */

#ifdef FEATURE_CDMA_SMS
/*===========================================================================

FUNCTION DSAT707SMS_FLUSH_SMS_INDICATIONS

DESCRIPTION
  Flushes buffer of SMS incoming message indications and sends indications
  to TE. Indications are buffered and sent based on AT parameter setting

DEPENDENCIES
  None

RETURN VALUE
  None

SIDE EFFECTS

===========================================================================*/
void dsat707sms_flush_sms_indications( void );


/*===========================================================================

FUNCTION  DSAT707SMS_INIT_SMS

DESCRIPTION
  SMS initialization function. This function does the following:
  - Initializes ATCOP as one of the clients to WMS.
  - Registers the WMS message and configuration call back functions.
  - Activates ATCOP asa client
  - Sets up the message routing configuration.
  
DEPENDENCIES
  None

RETURN VALUE
  none

SIDE EFFECTS
  None

===========================================================================*/

void dsat707sms_init_sms(void);

#endif /* FEATURE_CDMA_SMS */
/*===========================================================================

FUNCTION DSATETSISMS_SEND_SMS_MSG

DESCRIPTION
  Central place/function call for sending sms message.
  In case of actual message transmission, it is done in 2 phases. First you 
  get the actual command with all the parameters. Parameters are stored and 
  user is sent a prompt to indicate that message has to be typed. Then the 
  message is transmited. This function is used to transmit the actual message.
  It is used in both text mode as well as PDU mode.

DEPENDENCIES
  None

RETURN VALUE
  None

SIDE EFFECTS
  None

===========================================================================*/
void dsatetsisms_send_sms_msg
(
  byte  * msg, /* Ptr to the text message that has to be sent */
  int   len    /* Len of the text message that has to be sent */
);

/*===========================================================================

FUNCTION    DSATSMS_GET_MSG_FMT_MODE

DESCRIPTION
  Returns the current sms message format mode, for example, pdu or text.

DEPENDENCIES
  None

RETURN VALUE
  Returns a dsatsms_fmt_mode_e_type enum.

SIDE EFFECTS
  None

===========================================================================*/
dsatsms_fmt_mode_e_type dsatsms_get_msg_fmt_mode (void);

#ifdef FEATURE_ETSI_SMS
#error code not present
#endif /* FEATURE_ETSI_SMS */

#ifdef FEATURE_ETSI_SMS_PS
#error code not present
#endif /* FEATURE_ETSI_SMS_PS */

/*===========================================================================

FUNCTION DSATSMSI_READ_MSG_LIST

DESCRIPTION
  Local function that is used to read a message from the list of indices.
  Handles one list or lists of multiple <stat>s

DEPENDENCIES
  none

RETURN VALUE
  none

SIDE EFFECTS

===========================================================================*/
boolean dsatsmsi_read_msg_list
(
  void 
);

/*===========================================================================

FUNCTION DSATSMSI_GET_MSG_LIST

DESCRIPTION
  Local function that is used to get a message list.
  Handles one list or lists of multiple <stat>s

DEPENDENCIES
  none

RETURN VALUE
  none

SIDE EFFECTS

===========================================================================*/
boolean dsatsmsi_get_msg_list
(
  void 
);

dsat_result_enum_type dsatsmsi_gw_ready_cfg_event_handler
(
  dsat_mode_enum_type    mode,         /* AT command mode   */   
  sms_event_info_s_type  *event_info   /* Event info ptr    */
);

dsat_result_enum_type dsatsmsi_route_cfg_event_handler 
(
  dsat_mode_enum_type    mode,         /* AT command mode   */   
  sms_event_info_s_type  *event_info   /* Event info ptr    */
);

dsat_result_enum_type dsatsmsi_memory_status_cfg_event_handler 
(
  dsat_mode_enum_type    mode,         /* AT command mode   */   
  sms_event_info_s_type  *event_info   /* Event info ptr    */
);

dsat_result_enum_type dsatsmsi_message_list_cfg_event_handler
(
  dsat_mode_enum_type    mode,         /* AT command mode   */   
  sms_event_info_s_type  *event_info   /* Event info ptr    */
);

dsat_result_enum_type dsatsmsi_gw_domain_pref_event_handler
(
  dsat_mode_enum_type    mode,         /* AT command mode   */   
  sms_event_info_s_type  *event_info   /* Event info ptr    */
);

dsat_result_enum_type dsatsmsi_link_ctrl_event_handler
(
  dsat_mode_enum_type mode,             /* AT command mode   */
  sms_event_info_s_type  *event_info   /* Event info ptr    */
);

#ifdef FEATURE_ETSI_SMS_CB 
#error code not present
#endif /* FEATURE_ETSI_SMS_CB */

#ifdef FEATURE_DSAT_SIRIUS
#error code not present
#endif /* FEATURE_DSAT_SIRIUS */

/*===========================================================================

FUNCTION    DSATSMSI_TIMESTAMP

DESCRIPTION
  Converts the message SC timestamp type (wms_timestamp_s_type) to a 
  string. It converts it into "yy/MM/dd,hh:mm:ss+/-zz" format.
  writes the result into the result buffer.
  
DEPENDENCIES
  None

RETURN VALUE
  returns a pointer to the end of result buffer 

SIDE EFFECTS
  None
===========================================================================*/
byte * dsatsmsi_timestamp
( 
 wms_timestamp_s_type time,  /* Pointer to Time Stamp        */
 byte                *res_ptr /* Pointer to the result buffer */
);

/*===========================================================================

FUNCTION    QUEUE_MT_MSG

DESCRIPTION
  Adds unsolicited response result code to a queue.
  If the queue is full then the first element is deleted and the new
  element is replaces it.
  
DEPENDENCIES
  None

RETURN VALUE
  None

SIDE EFFECTS
  None

===========================================================================*/

void queue_mt_msg
(
 sms_mt_msg_e_type       msg_type,     /* Type of message      */
 wms_message_index_type  msg_index,    /* Index Number         */
 wms_memory_store_e_type mem_store,    /* Type of Memory Store */
 wms_transaction_id_type tid           /* Transaction ID       */
);


#endif /* defined(FEATURE_ETSI_SMS) || defined(FEATURE_CDMA_SMS) */
#endif /* FEATURE_DATA */

#endif /* DSATSMSI_H */

