
/*====*====*====*====*====*====*====*====*====*====*====*====*====*====*====*

                        D A T A   S E R V I C E S
                        A T   C O M M A N D   

                        ( S M S  C O M M A N D S )

GENERAL DESCRIPTION
  This module contains the command tables and data definitions required
  to define the SMS related AT modem commands.

  This file contains common sms functions across modes.
  
EXTERNALIZED FUNCTIONS

  Public functions exported outside of sms module.

    dsat_flush_sms_indications
      Flushes buffer of SMS incoming message indications and sends indications
      to TE. Indications are buffered and sent based on AT parameter setting

    dsatsms_abort_cmd
      Called by At command processor to abort an SMS command. This function
      will send an DS CMD to the AT Command processor indicating that the 
      commaand has been aborted.
    
    dsatsms_send_sms_msg
      This function is used to transmit an SMS message (After SMS message 
      entry mode is entered). It calls the mode-specific functions to send 
      SMS message.

    dsatsms_get_msg_fmt_mode
      Returns the current sms message format mode, for example, pdu or text.

    dsatsmsi_cmd_cb_func
      Process the WMS command call back status.

    dsatsms_abt_handler
      Handler function for aborting a SMS command

    dsatsms_err_handler
      When there is an error in processing a SMS AT command, WMS sends a 
      command to handle the error condition. This function is the handler 
      function for the error case.

    dsatsms_init_sms
      SMS initialization function. This function does the following: 
      Initializes ATCOP as one of the clients to WMS, Registers the WMS 
      message and configuration call back functions and Sets up the message 
      routing configuration.

INITIALIZATION AND SEQUENCING REQUIREMENTS
  None

   Copyright (c) 2009 by QUALCOMM Incorporated.
   All Rights Reserved.
   Qualcomm Confidential and Proprietary.
*====*====*====*====*====*====*====*====*====*====*====*====*====*====*====*/

/*===========================================================================

                        EDIT HISTORY FOR MODULE

  This section contains comments describing changes made to the module.
  Notice that changes are listed in reverse chronological order.

  $Header: //source/qcom/qct/modem/data/common/commonatcop/main/lite/src/dsatsms.c#1 $ $DateTime: 2009/01/23 10:36:19 $ $Author: nchagant $

when       who     what, where, why
--------   ---     ----------------------------------------------------------
01/21/09   nc      Added support for SMS Commands.

===========================================================================*/


/*===========================================================================

                     INCLUDE FILES FOR MODULE

===========================================================================*/
#include "comdef.h"
#include "customer.h"
#include "AEEstd.h"

#ifdef FEATURE_DATA
#if defined(FEATURE_ETSI_SMS) || defined(FEATURE_CDMA_SMS)
#include <stdio.h>
#include <string.h>

#include "dsati.h"
#include "dsatsmsi.h"
#include "err.h"
#include "assert.h"
#include "queue.h"

#include "dsatsms.h"
#include "dsatctab.h"
#ifdef FEATURE_ETSI_SMS
#error code not present
#endif /* FEATURE_ETSI_SMS */

#ifdef FEATURE_CDMA_SMS
#include "dsat707sms.h"
#include "dsat707smsctab.h"
#endif /* FEATURE_CDMA_SMS */

#ifdef FEATURE_DSAT_SIRIUS
#error code not present
#endif /* FEATURE_DSAT_SIRIUS */

/*===========================================================================
            REGIONAL DEFINITIONS AND DECLARATIONS FOR MODULE
  This section contains reginal definitions for variables that are used
  in ATCOP SMS SUB SYSTEM
===========================================================================*/

/*--------------------------------------------------------------------------
  Debug macro for this module.
---------------------------------------------------------------------------*/
#ifdef DEBUG_DSATCOP
#define DEBUG( x )  { x }
#else
#define DEBUG( x )
#endif /* DEBUG_DSATCOP */

/*--------------------------------------------------------------------------
  Import variables defined in other files of SMS module
---------------------------------------------------------------------------*/
#ifdef FEATURE_ETSI_SMS
#error code not present
#endif /* FEATURE_ETSI_SMS */

#ifdef FEATURE_CDMA_SMS
extern const msg_handler_tab_type       dsat707sms_msg_handler_table[];
extern const unsigned int               dsat707sms_msg_handler_table_size;
#endif /* FEATURE_CDMA_SMS */

#ifdef FEATURE_ETSI_SMS_TEMPLATE
#error code not present
#endif /* FEATURE_ETSI_SMS_TEMPLATE */

#ifdef FEATURE_DSAT_SIRIUS
#error code not present
#endif /* FEATURE_DSAT_SIRIUS */

/*--------------------------------------------------------------------------
  Variables common for SMS module
---------------------------------------------------------------------------*/
/* WMS client (ATCOP) ID */
wms_client_id_type dsatsmsi_client_id;

/* Variable to find out what command is being processed currently */
sms_cmd_name_e_type dsatsmsi_pres_cmd = SMS_NONE;

/* Variable used in SMS SM */
sms_cmd_sm_type dsatsmsi_cmd_sm;

/* For +CPMS or $QCPMS cmd */
/* remembers the mem being reported */
cpms_mem_status_data_s_type dsatsmsi_mem_report;

/* Flag for wms readiness */
boolean dsatsmsi_wms_ready;

#ifdef FEATURE_ETSI_SMS_CB
#error code not present
#endif /* FEATURE_ETSI_SMS_CB */

/* Watermarks and Queues for SMS. */
/* This WM holds received messages and implements flow control. 
   Actually it just drops MT messages if the watermark is at high 
   as indicated by sms_mt_msg_wm_full */
dsm_watermark_type  dsat_mt_msg_wm;
LOCAL q_type              dsat_mt_msg_q;

/* This one holds all other SMS events and does not perform flow control */
dsm_watermark_type  dsat_sms_wm;
LOCAL q_type              dsat_sms_q;

/* Flag indicating MT message watermark is at high water */
/* Messages received when this flag is TRUE will be lost */
LOCAL boolean sms_mt_msg_wm_full = FALSE;

#ifdef FEATURE_ETSI_SMS
#error code not present
#endif /*  FEATURE_ETSI_SMS */

/*Maximum number of MT messages on the queue*/
#define SMS_MT_MSG_COUNT    15 

/* SMS Mobile Terminated message queue node declaration */
/* This queue is used for STORE and NOTIFY messages     */
LOCAL sms_mt_msg_type mt_msg_buf[SMS_MT_MSG_COUNT];

/* SMS Mobile Terminated message queue */
q_type sms_mt_msg_q;         

/* Queue for storing free message buffers */ 
q_type sms_mt_msg_free_q;


/* Lo Water, Hi Water and Don't Exceed counts for the   */
/* Autodetect watermark used for RX data bytes from WMS */
#define DSAT_FROM_WMS_LOWATER_CNT          (WMS_MAX_LEN * 5)
#define DSAT_FROM_WMS_HIWATER_CNT          (WMS_MAX_LEN * 15)
#define DSAT_FROM_WMS_DONT_EXCEED_CNT      (WMS_MAX_LEN * 30)

/*--------------------------------------------------------------------------
  Local function prototypes
---------------------------------------------------------------------------*/
LOCAL void sms_msg_cb_func 
(
  wms_msg_event_e_type            event,       /* Event ID              */
  const wms_msg_event_info_s_type *event_ptr   /* Pointer to Event info */
);

LOCAL void sms_cfg_cb_func 
(
  wms_cfg_event_e_type            event,     /* Event ID              */
  const wms_cfg_event_info_s_type *event_ptr /* Pointer to Event info */
);

#ifdef FEATURE_ETSI_SMS_CB
#error code not present
#endif /* FEATURE_ETSI_SMS_CB */

LOCAL boolean event_check 
(
  wms_cfg_event_e_type      event,     /* Event ID                   */
  wms_message_tag_e_type    tag        /* Tag : used in message list */
);

LOCAL sms_event_e_type cfg_event_lookup
(
 wms_cfg_event_e_type wms_event /* Event ID */
);

LOCAL sms_event_e_type msg_event_lookup
(
 wms_msg_event_e_type wms_event /* Event ID */
);

LOCAL void setup_sms_watermarks( void );

LOCAL void sms_mt_msg_wm_non_empty_cb ( void );

LOCAL void sms_wm_non_empty_cb ( void );

LOCAL void sms_wm_hiwater_cb ( void );

LOCAL void sms_wm_lowater_cb ( void );

#ifdef FEATURE_ETSI_SMS_TEMPLATE
#error code not present
#endif /* FEATURE_ETSI_SMS_TEMPLATE */

/*---------------------------------------------------------------------------
 * FUNCTION DEFINITIONS                                                     
 *-------------------------------------------------------------------------*/
/*===========================================================================

FUNCTION DSATSMS_SIGNAL_HANDLER

DESCRIPTION
  This function processes both received MT message and SMS signals, 
  which indicate MT message and other SMS events, respectively, have been 
  received from WMS.

DEPENDENCIES
  None

RETURN VALUE
  DSAT_ERROR: if we find an error in procssing the event
  DSAT_OK: if processing of the event is complete and right
  DSAT_ASYNC_CMD : if we still have more events to process
  DSAT_ASYNC_EVENT : if the event is ignored

SIDE EFFECTS
  None

===========================================================================*/
dsat_result_enum_type dsatsms_signal_handler
(
 dsat_mode_enum_type at_mode
)
{
  dsm_item_type * sms_dsm_item_ptr;
  sms_event_info_s_type sms_info;
  const msg_handler_tab_type          * handler_table = NULL;
  int                                   table_size;
  dsati_operating_cmd_mode_type         cmd_mode;
  int                                   index;
  dsat_result_enum_type                 result;

  while ( (sms_dsm_item_ptr = dsm_dequeue( &dsat_mt_msg_wm )) != NULL ||
          (sms_dsm_item_ptr = dsm_dequeue( &dsat_sms_wm )) != NULL )
  {
    (void)dsm_pullup 
      (&sms_dsm_item_ptr, &sms_info, sizeof(sms_event_info_s_type));

    dsm_free_packet( &sms_dsm_item_ptr );
    MSG_HIGH("ATCOP:Freed %d DSM item, signal handler",sms_dsm_item_ptr, 0, 0);
    
    cmd_mode = dsatcmdp_get_operating_cmd_mode();

#ifdef FEATURE_ETSI_SMS 
#error code not present
#endif /* FEATURE_ETSI_SMS */
#ifdef FEATURE_CDMA_SMS 
    if (cmd_mode == CDMA_CMD_MODE)
    {
      handler_table = dsat707sms_msg_handler_table;
      table_size = (int) dsat707sms_msg_handler_table_size;
    }
    else
#endif /* FEATURE_CDMA_SMS */
    {
      MSG_MED("Cannot process this event in this mode: %d",cmd_mode,0,0);
      return DSAT_ASYNC_EVENT;
    }

    for (index = 0; index < table_size; index++)
    {
      if(handler_table[index].event == sms_info.event)
      {
        result = handler_table[index].handler(at_mode, &sms_info);

        if ( result != DSAT_ASYNC_CMD && result != DSAT_ASYNC_EVENT )
        {
          if (dsat_sms_wm.current_cnt != 0)
          {
            (void)rex_set_sigs( &ds_tcb, DS_AT_SMS_SIG );
          }

          if (dsat_mt_msg_wm.current_cnt != 0)
          {
            (void)rex_set_sigs( &ds_tcb, DS_AT_MT_MSG_SIG );
          }

          return result;
        }
      }
    } /* for */
  } /* while */

  return DSAT_ASYNC_EVENT;
}/* dsatsms_signal_handler */

/*===========================================================================

FUNCTION DSAT_FLUSH_SMS_INDICATIONS

DESCRIPTION
  Flushes buffer of SMS incoming message indications and sends indications
  to TE. Indications are buffered and sent based on AT parameter setting.

DEPENDENCIES
  None

RETURN VALUE
  None

SIDE EFFECTS
  
===========================================================================*/
void dsat_flush_sms_indications
(
 void
)
{
  dsati_operating_cmd_mode_type cmd_mode;

  cmd_mode = dsatcmdp_get_operating_cmd_mode();

  DEBUG(
        MSG_MED("dsat_flush_sms_indications, cmd_mode:%d",
                cmd_mode, 0, 0);
        );

#ifdef FEATURE_ETSI_SMS 
#error code not present
#endif /* FEATURE_ETSI_SMS */
#ifdef FEATURE_CDMA_SMS 
  if (cmd_mode == CDMA_CMD_MODE)
  {
    dsat707sms_flush_sms_indications();
    return;
  }
  else
#endif /* FEATURE_CDMA_SMS */
  {
    MSG_ERROR("Wrong cmd_mode %d",cmd_mode,0,0);
    return;
  }

}/* dsat_flush_sms_indications */


/*===========================================================================

FUNCTION DSATSMS_ABORT_CMD

DESCRIPTION
  Called by At command processor to abort an SMS command. A command can be
  aborted if user enters ESC key or if the ATCOP runs out of buffers. This 
  function will send an DS CMD to the AT Command processor indicating that
  the commaand has been aborted.

  If status is 0, the function is called when user enters ESC key.
  If status is 1, the function is called when we run out of buffer
                  or other cmd error.
  If status is other number, there is a cms error.  This function can be
                             called when there is error sending sms 
                             messages.

  Event will contain the following result code:
  DSAT_ERROR: if we are out of buffer for the message to send
  DSAT_OK: if user sent ESC character while writing the message
  A CMS error number: if there is a cms error.

DEPENDENCIES
  None

RETURN VALUE
  None

SIDE EFFECTS
  DS CMD is generated.

===========================================================================*/
void dsatsms_abort_cmd
(
 dsati_cms_err_e_type error /* +CMS ERROR Number */
)
{
  dsat_result_enum_type result_code;
  ds_cmd_type * cmd_ptr;

  /* tell atcop that there is a command abort */
  if( (cmd_ptr = ds_get_cmd_buf()) == NULL)
  {
    ERR_FATAL("No ds command buffer",0,0,0);
  }
  else
  {
    MSG_HIGH ("Aborting SMS command", 0, 0, 0);
    /* check to see what type of abort it is */
    /* CMS_ERROR: Command ERROR              */
    /* CMS_NONE: ESC from user               */
    if(error)
    {
      result_code = DSAT_CMD_ERR_RSP;
      cmd_ptr->cmd.sms_abt.cms_error = (int)error;

    }
    else
    {
      result_code = DSAT_OK;
    }

    cmd_ptr->hdr.cmd_id = DS_AT_SMS_ABT_CMD;
    cmd_ptr->cmd.sms_abt.cmd_status = (int16) result_code;
    ds_put_cmd(cmd_ptr);
  }

}/* dsatsms_abort_cmd */


/*===========================================================================

FUNCTION DSATSMS_SEND_SMS_MSG

DESCRIPTION
  Central place/function call for sending sms message.
  In case of actual message transmission, it is done in 2 phases. First you 
  get the actual command with all the parameters. Parameters are stored and 
  user is sent a prompt to indicate that message has to be typed. Then the 
  message is transmited. This function is used to transmit the actual message.
  It calls the mode-specific functions to send SMS message.

DEPENDENCIES
  None

RETURN VALUE
  None

SIDE EFFECTS
  None

===========================================================================*/
void dsatsms_send_sms_msg
(
 byte  * msg, /* Ptr to the text message that has to be sent */
 int   len    /* Len of the text message that has to be sent */
)
{
  dsati_operating_cmd_mode_type cmd_mode;

  cmd_mode = dsatcmdp_get_operating_cmd_mode();

  DEBUG(
        MSG_MED("dsatsms_send_sms_msg, len: %d, msg: %c%c...",
                len, msg[0], msg[1]);
        );

 #ifdef FEATURE_ETSI_SMS 
#error code not present
#endif /* FEATURE_ETSI_SMS */
#ifdef FEATURE_CDMA_SMS 
  if (cmd_mode == CDMA_CMD_MODE)
  {
    dsat707sms_send_sms_msg ( msg, len );
  }
  else
#endif /* FEATURE_CDMA_SMS */
  {
    MSG_ERROR("We are in wrong mode: %d",cmd_mode,0,0);
    return;
  }
  
}/* dsatsms_send_sms_msg */


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
dsatsms_fmt_mode_e_type dsatsms_get_msg_fmt_mode (void)
{
  dsati_operating_cmd_mode_type cmd_mode;
  dsatsms_fmt_mode_e_type       fmt_mode;

  cmd_mode = dsatcmdp_get_operating_cmd_mode();

  if (cmd_mode == ETSI_CMD_MODE)
  {
    fmt_mode = DSATSMS_TEXT_MODE;
#ifdef FEATURE_ETSI_SMS
#error code not present
#endif /* FEATURE_ETSI_SMS */
  }
  else
  {
    fmt_mode = DSATSMS_TEXT_MODE;
  }

  return fmt_mode;
}/* dsatsms_get_msg_fmt_mode */



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
/* ARGSUSED */
void dsatsmsi_cmd_cb_func
(
  wms_cmd_id_e_type     cmd,   /* Cmd ID            */
  void                  *data, /* User data pointer */
  wms_cmd_err_e_type    status /* Command Status    */
)
{
  ds_cmd_type *cmd_buf;

  DEBUG(
        MSG_MED("dsatsmsi_cmd_cb_func, cmd:%d, status:%d",
                cmd, status, 0);
        MSG_MED("data: %d, %p", *((int *) data), data, 0);
        );

  if (status == WMS_CMD_ERR_NONE) {
    /* If there is no error */
    /* Continue             */
    return;
  }
  
  /* Pass the error status to the TE */

  MSG_HIGH("DS SMS cmd err %d %d", cmd, status, 0);

  if( (cmd_buf = ds_get_cmd_buf()) == NULL)
  {
    ERR_FATAL("No ds command buffer",0,0,0);
  }
  else
  {
    /* send the message to ATCOP */
    cmd_buf->hdr.cmd_id = DS_AT_SMS_ERR_CMD;
    cmd_buf->cmd.sms_cmd.cmd     = cmd;
    cmd_buf->cmd.sms_cmd.cmd_info = status;
    ds_put_cmd(cmd_buf);
  }

  return;
}/* dsatsmsi_cmd_cb_func */ 

/*===========================================================================

FUNCTION  DSATSMS_ABT_HANDLER

DESCRIPTION
  Handler function for aborting a SMS command

DEPENDENCIES
  None

RETURN VALUE
  returns an enum that describes the result of the command execution.
  DSAT_ERROR: if we are out of buffer for the message to send
  DSAT_OK: if user sent ESC character while writing the message

SIDE EFFECTS
  None

===========================================================================*/
/* ARGSUSED */
dsat_result_enum_type dsatsms_abt_handler
( 
 dsat_mode_enum_type mode, /* AT command mode    */
 ds_cmd_type * cmd_ptr     /* DS Command pointer */
)
{
 dsm_item_type * res_buff;

  DEBUG(
        MSG_MED("dsatsms_abt_handler, error:%d", 
                cmd_ptr->cmd.sms_abt.cms_error, 0, 0);
        );

  dsatsmsi_pres_cmd = SMS_NONE;

  /* Check to see if we need to send +CMSS ERROR */
  if ( (dsat_result_enum_type) cmd_ptr->cmd.sms_abt.cmd_status == 
       DSAT_CMD_ERR_RSP )
  {
    if( (res_buff = dsm_new_buffer( DSM_DS_LARGE_ITEM_POOL )) == NULL)
    {
      MSG_ERROR( "Could not get a new DSM item", 0, 0, 0);
      return DSAT_ERROR;
    }
    else
    {
      res_buff->used = (byte) dsatsmsi_cms_error(
                                    res_buff, 
                                    cmd_ptr->cmd.sms_abt.cms_error
                                    );
      /* send the response to the user */
      dsatcmdp_handle_async_cmd_rsp(res_buff, DSAT_COMPLETE_RSP);
      return DSAT_CMD_ERR_RSP;
    }
  } else if(DSAT_ERROR ==
            (dsat_result_enum_type)cmd_ptr->cmd.sms_abt.cmd_status) {
    return DSAT_ERROR;
  } else
      {
    /* just return the result code */
    return (dsat_result_enum_type) cmd_ptr->cmd.sms_abt.cmd_status;
  }

}/* dsatsms_abt_handler */

/*===========================================================================

FUNCTION  DSATSMS_ERR_HANDLER

DESCRIPTION
  When there is an error in processing a SMS AT command, WMS sends a command
  to handle the error condition. This function is the handler function for the
  error case.
  
DEPENDENCIES
  None

RETURN VALUE
  returns an enum that describes the result of the command execution.

SIDE EFFECTS
  None

===========================================================================*/
/* ARGSUSED */
dsat_result_enum_type dsatsms_err_handler
( 
 dsat_mode_enum_type mode, /* AT command mode    */
 ds_cmd_type * cmd_ptr     /* DS Command pointer */
)
{
  dsat_result_enum_type result = DSAT_OK;
#if defined(FEATURE_WCDMA) || defined(FEATURE_GSM)
#error code not present
#endif /* defined(FEATURE_WCDMA) || defined(FEATURE_GSM) */

  if( dsatsmsi_pres_cmd == SMS_NONE )
  {
    result = DSAT_ASYNC_EVENT;
  }
  else if(((dsatsmsi_pres_cmd == SMS_CMGD) || (dsatsmsi_pres_cmd == SMS_CMGR))
     &&
     ( cmd_ptr->cmd.sms_cmd.cmd_info == WMS_CMD_ERR_MSG_EMPTY_MESSAGE ) 
    )
  {
    /* We got reading or deleting an empty slot */
    /* Just return OK */
    dsatsmsi_pres_cmd = SMS_NONE;
    memset ( &dsatsmsi_cmd_sm, 0, sizeof(sms_cmd_sm_type) );
    result = DSAT_OK;
  }
  else if ( (( dsatsmsi_pres_cmd==SMS_CSCB )|| 
            ( dsatsmsi_pres_cmd == SMS_CSAS_CSCB )||
            ( dsatsmsi_pres_cmd == SMS_CRES_CSCB )) &&
            ( cmd_ptr->cmd.sms_cmd.cmd_info == WMS_CMD_ERR_BC_BAD_SRV_ID ) )
  {
    /* We got reading or deleting an empty slot */
    /* Just return OK */
    dsatsmsi_pres_cmd = SMS_NONE;
    memset ( &dsatsmsi_cmd_sm, 0, sizeof(sms_cmd_sm_type) );
    result = DSAT_OK;
  }
  else
  {
    dsm_item_type * res_buff;

    if( (res_buff = dsm_new_buffer( DSM_DS_LARGE_ITEM_POOL )) == NULL)
    {
      ERR_FATAL( "Could not get a new DSM item", 0, 0, 0);
    }
    else
    {
      dsati_cms_err_e_type cms_err = DSAT_CMS_UNKNOWN_ERR;

      /* We got an error for WMS */
      if(
         /* User was trying to send a message of non-delivery type,
            not WMS_TAG_MO_NOT_SENT or WMS_TAG_MO_SENT */
         (dsatsmsi_pres_cmd==SMS_CMSS && 
           cmd_ptr->cmd.sms_cmd.cmd_info == WMS_CMD_ERR_MSG_TAG) 

         ||

          /* Process WMS_CMD_ERR_MSG_SEND_MODE error since the 
             message to be sent was too long */
          ( ( (dsatsmsi_pres_cmd == SMS_CMGS) || 
              (dsatsmsi_pres_cmd == SMS_CMSS) ) &&
            ( cmd_ptr->cmd.sms_cmd.cmd_info == WMS_CMD_ERR_MSG_SEND_MODE )
          ) 
        )
      {
#ifdef FEATURE_ETSI_SMS
#error code not present
#endif /* FEATURE_ETSI_SMS */
      }
      else
      {
        /* Map wms_cmd_err_e_type enum values to +CMS ERRORs */
        switch (cmd_ptr->cmd.sms_cmd.cmd_info) {
          case WMS_CMD_ERR_MSG_MEMORY_STORE:
                cms_err = DSAT_CMS_MEM_FAILURE;
            break;
            
          case WMS_CMD_ERR_MSG_SIM_WRITE:
          case WMS_CMD_ERR_MSG_SIM_READ:
          case WMS_CMD_ERR_MSG_SIM_DELETE:
#ifdef FEATURE_CDMA_SMS
          case WMS_CMD_ERR_MSG_RUIM_WRITE:
          case WMS_CMD_ERR_MSG_RUIM_READ:
          case WMS_CMD_ERR_MSG_RUIM_DELETE:
#endif /* FEATURE_CDMA_SMS */

                cms_err = DSAT_CMS_SIM_FAILURE;
                break;
            
          case WMS_CMD_ERR_MSG_INDEX: 
          case WMS_CMD_ERR_MSG_EMPTY_MESSAGE:
                cms_err = DSAT_CMS_INVALID_INDEX;
                break;
            
          case WMS_CMD_ERR_MSG_MEMORY_FULL:
          case WMS_CMD_ERR_MSG_RAM_FULL:
          case WMS_CMD_ERR_MSG_SIM_FULL:
          case WMS_CMD_ERR_MSG_NV_FULL:
                cms_err = DSAT_CMS_MEM_FULL;
                break;

          case WMS_CMD_ERR_MSG_TPDU_TYPE:
            cms_err = DSAT_CMS_OP_NOT_ALLOWED;
            break;

          case WMS_CMD_ERR_MSG_TRANSACTION_ID:
            cms_err = DSAT_CMS_CNMA_NOT_EXP;
            break;
          
          case WMS_CMD_ERR_BC_DUPLICATE_SRV:
          case WMS_CMD_ERR_BC_CANNOT_ACTIVATE:
          case WMS_CMD_ERR_BC_CANNOT_ACTIVATE_ALL:
            cms_err = DSAT_CMS_OP_NOT_ALLOWED;
            break;
            
          default:
                cms_err = DSAT_CMS_UNKNOWN_ERR;
            break;
        }
      }
      res_buff->used = (uint16)dsatsmsi_cms_error ( res_buff, (int)cms_err );

      result = DSAT_CMD_ERR_RSP;

#if defined(FEATURE_ETSI_SMS) && defined(FEATURE_ETSI_SMS_TEMPLATE)
#error code not present
#endif /* FEATURE_ETSI_SMS && FEATURE_ETSI_SMS_TEMPLATE */
      
      /* send the response to the user */
      dsatcmdp_handle_async_cmd_rsp(res_buff, DSAT_COMPLETE_RSP);
      dsatsmsi_pres_cmd = SMS_NONE;
      memset ( &dsatsmsi_cmd_sm, 0, sizeof(sms_cmd_sm_type) );
    }
  }

  return result;
} /* dsatsms_err_handler */

#ifdef FEATURE_ETSI_SMS_CB 
#error code not present
#endif /* FEATURE_ETSI_SMS_CB */


/*===========================================================================

FUNCTION    MT_MSG_QUEUE_INIT

DESCRIPTION
  Initialize the mobile terminted message queue and the free message queue, 
  and link the message items onto the free message queue.
  
DEPENDENCIES
  None

RETURN VALUE
  None

SIDE EFFECTS
  None

===========================================================================*/
void mt_msg_queue_init( void )
{
  uint8    i;                                                /* Loop index */

  /*  Initialize the mobile terminted message queue and the free message  */
  /*  queue, and link the message items onto the free message queue.      */  
  
  (void) q_init( &sms_mt_msg_q );
  (void) q_init( &sms_mt_msg_free_q );

  for( i = 0; i < SMS_MT_MSG_COUNT; i++ )
  {
    (void) q_link( &mt_msg_buf[i], &mt_msg_buf[i].link );
    q_put( &sms_mt_msg_free_q, &mt_msg_buf[i].link );
  }

} /* mt_msg_queue_init */


/*===========================================================================

FUNCTION  DSATSMS_INIT_SMS

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
void dsatsms_init_sms ( void )
{
  wms_client_err_e_type result;
  nv_item_type item;

  /* Initializes necessary watermarks first */
  setup_sms_watermarks();

  /* Initializes ATCOP as one of the clients to WMS               */
  /* Rest of the functionality needs the client to be initialized */
  result = wms_client_init (  
                                WMS_CLIENT_TYPE_ATCOP,
                                &dsatsmsi_client_id
                              );

  /* This should not happen, raise an error */
  if (result != WMS_CLIENT_ERR_NONE)
  {
    MSG_ERROR( "Could not initialize with WMS", 0, 0, 0 );
  }

  /* Register the configuration call back function */
  result = wms_client_reg_cfg_cb ( dsatsmsi_client_id,
                                   (wms_cfg_event_cb_type)sms_cfg_cb_func );

  if (result != WMS_CLIENT_ERR_NONE)
  {
    MSG_ERROR( "Could not register call back function with reg_cfg_cb", 0, 0, 0 );
  }
  /* Register the message call back function */
  result = wms_client_reg_msg_cb ( dsatsmsi_client_id,
                                   (wms_msg_event_cb_type)sms_msg_cb_func );
  if (result != WMS_CLIENT_ERR_NONE)
  {
    MSG_ERROR( "Could not register call back function with reg_msg_cb", 0, 0, 0 );
  }

#ifdef FEATURE_ETSI_SMS_CB 
#error code not present
#endif /* FEATURE_ETSI_SMS_CB */

  /* This should not happen, raise an error */
  if (result != WMS_CLIENT_ERR_NONE)
  {
    MSG_ERROR( "Could not register call back function with WMS", 0, 0, 0 );
  }

  result = wms_client_activate ( dsatsmsi_client_id );

  if (result != WMS_CLIENT_ERR_NONE)
  {
    MSG_ERROR( "Could not initialize with WMS", 0, 0, 0 );
  }

  /* Get the MEM3 value from NV (in sync with NV) and use it for CPMS read command 
     MEM1 and MEM2 are defaulted to ME at boot up.*/
  if ( dsatutil_get_nv_item ( NV_SMS_CFG_ROUTING_I ,&item ) == NV_DONE_S )
  {
    /*  NV item gets changed only when the cpms_val[2] is modified. 
        cpms_val[0],cpms_val[1] remains same. At boot up we need 
        to get only the cpms_val[2] from NV */
#ifdef FEATURE_ETSI_SMS
#error code not present
#endif
#ifdef FEATURE_CDMA_SMS
    /* Check if value in NV is valid, If not set <mem3> to default */
    if ( item.sms_cfg_routing.pp_mem_stores[5] < 3)
    {
      dsat707sms_qcpms_val[2] = item.sms_cfg_routing.pp_mem_stores[5];
    }
    else 
    {
      dsat707sms_qcpms_val[2] = 0 ;
    }
#endif /* FEATURE_ETSI_SMS */
  }
  else
  {
    MSG_ERROR (" Could'nt get the NV_SMS_CFG_ROUTING_I; Default MEM3 is taken for CPMS",0,0,0);
  }
#ifdef FEATURE_ETSI_SMS
#error code not present
#endif /* FEATURE_ETSI_SMS */

#ifdef FEATURE_CDMA_SMS
  dsat707sms_init_sms();
#endif /* FEATURE_CDMA_SMS */

  return;
}/* dsatsms_init_sms */

/**********************LOCAL UTILITY FUNCTIONS******************************/

/**********************CALL BACK FUNCTIONS**********************************/

/*===========================================================================
FUNCTION  SMS_MSG_CB_FUNC

DESCRIPTION
  SMS Message event callback function

DEPENDENCIES

RETURNS
  None

SIDE EFFECTS
  Adds command in DS command buffer
===========================================================================*/
LOCAL void sms_msg_cb_func
(
  wms_msg_event_e_type            event,       /* Event ID              */
  const wms_msg_event_info_s_type *event_ptr   /* Pointer to Event info */
)
{
  sms_event_info_s_type event_info;
  dsm_item_type * ds_cmd_buff = NULL;
  dsm_watermark_type * wm_to_use = &dsat_sms_wm;

  MSG_FATAL("***zzg sms_msg_cb_func event=%d***", event, 0, 0);

  if( event_ptr == NULL )
  {
    ERR_FATAL("Null event_ptr",0,0,0);
  }

  DEBUG(
        MSG_MED("sms_msg_cb_func, event:%d", event,0,0);
        );
  
  memset ( &event_info, 0, sizeof( sms_event_info_s_type ) );
  
  if ( dsatsmsi_pres_cmd == SMS_NONE )
  {
    boolean check = FALSE;

    /* Not processing any command */
    if ( event == WMS_MSG_EVENT_RECEIVED_MESSAGE ||
         event == WMS_MSG_EVENT_STATUS_REPORT ||
         event == WMS_MSG_EVENT_MT_MESSAGE_ERROR
#ifdef FEATURE_ETSI_SMS_TEMPLATE
#error code not present
#endif /* FEATURE_ETSI_SMS_TEMPLATE */
        )
    {
      check = TRUE;
    }
    
    if ( !check )
    {
      /* Received an no use event */
      return;
    }
  }

  event_info.event = msg_event_lookup(event);

  if ( event_info.event == SMS_EVENT_NONE )
  {
    MSG_ERROR("unable to map, event:%d", event,0,0);
    return;
  }
    
  if ( event == WMS_MSG_EVENT_SEND ||
       event == WMS_MSG_EVENT_READ ||
       event == WMS_MSG_EVENT_WRITE            ||
       event == WMS_MSG_EVENT_READ_STS_REPORT  ||
       event == WMS_MSG_EVENT_WRITE_STS_REPORT   )
  {
    event_info.u.msg_status.msg_hdr = event_ptr->status_info.message.msg_hdr;
    switch ( event_info.u.msg_status.msg_hdr.message_mode )
    {
      case WMS_MESSAGE_MODE_CDMA:
        event_info.u.msg_status.u.cdma_message = 
          event_ptr->status_info.message.u.cdma_message;
        break;
      case WMS_MESSAGE_MODE_GW:
        event_info.u.msg_status.u.gw_message = 
        event_ptr->status_info.message.u.gw_message;
        break;
      default:
        MSG_ERROR( "Bad message mode: %d", 
             event_info.u.msg_status.msg_hdr.message_mode ,0,0 );
        return;
    }
    event_info.u.msg_status.user_data = event_ptr->status_info.user_data;

    wm_to_use = &dsat_sms_wm;
  }
  else if ( ( event == WMS_MSG_EVENT_RECEIVED_MESSAGE ) ||
            ( event == WMS_MSG_EVENT_STATUS_REPORT ) )
  {
    /* Drop received message if watermark is full */
    if (sms_mt_msg_wm_full)
    {
      return;
    }

    event_info.u.mt_msg.msg_hdr = event_ptr->mt_message_info.message.msg_hdr;
    switch ( event_info.u.mt_msg.msg_hdr.message_mode )
    {
      case WMS_MESSAGE_MODE_CDMA:
        event_info.u.mt_msg.u.cdma_message = 
          event_ptr->mt_message_info.message.u.cdma_message;
        break;
      case WMS_MESSAGE_MODE_GW:
        event_info.u.mt_msg.u.gw_message = 
        event_ptr->mt_message_info.message.u.gw_message;
        break;
      default:
        MSG_ERROR( "Bad message mode: %d", 
             event_info.u.msg_status.msg_hdr.message_mode ,0,0 );
        return;
    }
    event_info.u.mt_msg.transaction_id = 
    event_ptr->mt_message_info.transaction_id;

    wm_to_use = &dsat_mt_msg_wm;
  }
  else if ( event == WMS_MSG_EVENT_SUBMIT_REPORT )
  {
    int i;

    /* Drop received message if watermark is full */
    if (sms_mt_msg_wm_full)
    {
      return;
    }

    event_info.u.submit_report.report_status = 
      event_ptr->submit_report_info.report_status;
    event_info.u.submit_report.tpdu_type = 
      event_ptr->submit_report_info.tpdu_type;
    event_info.u.submit_report.u.tp_cause = 
      event_ptr->submit_report_info.u.error.tp_cause;
    event_info.u.submit_report.tl_status =
      event_ptr->submit_report_info.cause_info.tl_status;
    event_info.u.submit_report.u.ack.sm_len = 
      event_ptr->submit_report_info.u.ack.user_data.sm_len;
    event_info.u.submit_report.u.ack.timestamp = 
      event_ptr->submit_report_info.u.ack.timestamp;

    if ( ( event_ptr->submit_report_info.report_status == WMS_RPT_OK ) &&
         ( event_ptr->submit_report_info.tpdu_type != WMS_TPDU_NONE ) )
    {
      for ( i=0; 
            ((i<event_info.u.submit_report.u.ack.sm_len) && (i<WMS_MAX_LEN)); 
            i++ )
      {
        event_info.u.submit_report.u.ack.sm_data[i] =
        event_ptr->submit_report_info.u.ack.user_data.sm_data[i];
      }
    }

    wm_to_use = &dsat_mt_msg_wm;
  }
  else if ( event == WMS_MSG_EVENT_MT_MESSAGE_ERROR )
  {
    event_info.u.err_report = event_ptr->mt_message_error.report_status;

    wm_to_use = &dsat_sms_wm;
  }
  else if ( event == WMS_MSG_EVENT_READ_TEMPLATE )
  {
    event_info.u.msg_status.u.gw_template = 
      event_ptr->status_info.message.u.gw_template;
    event_info.u.msg_status.msg_hdr.mem_store = 
      event_ptr->status_info.message.msg_hdr.mem_store;
  }
  
  /* Place the information in a DSM item */
  ds_cmd_buff = dsm_new_buffer( DSM_DS_LARGE_ITEM_POOL );
  
  if ( ds_cmd_buff == NULL )
  {
    ERR_FATAL( "Could not get a new DSM item", 0, 0, 0);
  }

  MSG_HIGH("Allocated %d DSM item for SMS MSG Event", ds_cmd_buff, 0, 0);
  
  if ( dsm_pushdown_tail( &ds_cmd_buff, &event_info,
                          sizeof(sms_event_info_s_type),
                          DSM_DS_LARGE_ITEM_POOL
                        ) != sizeof(sms_event_info_s_type) )
  {
    ASSERT(0);
  }

  if (ds_cmd_buff)
  {
    dsm_enqueue(wm_to_use, &ds_cmd_buff);
  }

  return;
}/* sms_msg_cb_func */


/*===========================================================================
FUNCTION  SMS_CFG_CB_FUNC

DESCRIPTION
  SMS configuration event callback function

DEPENDENCIES

RETURNS
  None

SIDE EFFECTS
  Adds command in DS command buffer
===========================================================================*/
LOCAL void sms_cfg_cb_func 
(
  wms_cfg_event_e_type            event,     /* Event ID              */
  const wms_cfg_event_info_s_type *event_ptr /* Pointer to Event info */
)
{
  sms_event_info_s_type event_info;
  dsm_item_type * ds_cmd_buff;

  ASSERT (event_ptr != NULL);

  DEBUG(
        MSG_MED("sms_cfg_cb_func, event:%d", event,0,0);
        );

  if ( ( dsatsmsi_pres_cmd == SMS_NONE ) && 
        ( !event_check(event, event_ptr->message_list.tag) ) )
  {
    /* Only process config events related to a command or approved by
       event_check() */
    return;
  }

  memset ( &event_info, 0, sizeof( sms_event_info_s_type ) );

  event_info.event = cfg_event_lookup(event);

  switch ( event_info.event )
  {
    case SMS_CFG_EVENT_MESSAGE_LIST:
      {
        uint32 i;
        event_info.u.msg_list.tag = event_ptr->message_list.tag;
        event_info.u.msg_list.len = (uint16)event_ptr->message_list.len;
        event_info.u.msg_list.mem_store = event_ptr->message_list.mem_store;
        for ( i = 0; i < event_ptr->message_list.len; i++ )
        {
          event_info.u.msg_list.indices[i] = 
            (uint16) event_ptr->message_list.indices[i];
        }
      }
      break;

    case SMS_CFG_EVENT_MEMORY_STATUS:
      event_info.u.mem_status = event_ptr->memory_status;
      break;

#ifdef FEATURE_ETSI_SMS
#error code not present
#endif /* FEATURE_ETSI_SMS */

#ifdef FEATURE_DSAT_SIRIUS
#error code not present
#endif /* FEATURE_DSAT_SIRIUS */

    case SMS_EVENT_NONE:
      MSG_MED("unable to map, event:%d", event,0,0);
      return;

    default:
      break;
  }

  /* Place the information in a DSM item */
  ds_cmd_buff = dsm_new_buffer( DSM_DS_LARGE_ITEM_POOL );
  
  if ( ds_cmd_buff == NULL )
  {
    ERR_FATAL( "Could not get a new DSM item", 0, 0, 0);
  }

  if ( dsm_pushdown_tail( &ds_cmd_buff, &event_info,
                          sizeof(sms_event_info_s_type),
                          DSM_DS_LARGE_ITEM_POOL
                        ) != sizeof(sms_event_info_s_type) )
  {
    /* dsm push down failed! */
    ASSERT(0);
  }

  if (ds_cmd_buff)
  {
    dsm_enqueue(&dsat_sms_wm, &ds_cmd_buff);
  }

  MSG_HIGH("Allocated %d DSM item for SMS SUBMIT", ds_cmd_buff, 0, 0);
  
  return;
}/* sms_cfg_cb_func */

#ifdef FEATURE_ETSI_SMS_CB
#error code not present
#endif /* FEATURE_ETSI_SMS_CB */

/*===========================================================================

FUNCTION  DSATSMSI_GW_READY_CFG_EVENT_HANDLER

DESCRIPTION
  Handler function for gw ready configuration event from WMS

DEPENDENCIES
  None

RETURN VALUE
  returns an enum that describes the result of the command execution.
  Possible values are:
    DSAT_ASYNC_EVENT : the event is not related to an AT command

SIDE EFFECTS
  None
===========================================================================*/
/* ARGSUSED */
dsat_result_enum_type dsatsmsi_gw_ready_cfg_event_handler
(
 dsat_mode_enum_type mode, /* AT command mode    */
  sms_event_info_s_type  *event_info   /* Event info ptr    */
)
{
  /* WMS is ready now */
  dsatsmsi_wms_ready = TRUE;

#ifdef FEATURE_ETSI_SMS_TEMPLATE  
#error code not present
#endif /* FEATURE_ETSI_SMS_TEMPLATE */

#ifdef FEATURE_ETSI_SMS_PS
#error code not present
#endif /* FEATURE_ETSI_SMS_PS */

  return DSAT_ASYNC_EVENT;
}/* dsatsmsi_gw_ready_cfg_event_handler */

/*===========================================================================

FUNCTION  DSATSMS_ROUTE_CFG_EVENT_HANDLER

DESCRIPTION
  Handler function for route configuration event from WMS

DEPENDENCIES
  None

RETURN VALUE
  none

SIDE EFFECTS
  None

===========================================================================*/
/* ARGSUSED */
dsat_result_enum_type dsatsmsi_route_cfg_event_handler
(
 dsat_mode_enum_type mode, /* AT command mode    */
  sms_event_info_s_type  *event_info   /* Event info ptr    */
)
{
  dsat_result_enum_type result;

  DEBUG(
        MSG_MED("route_cfg_event_handler, pres_cmd:%d",
                dsatsmsi_pres_cmd,0,0);
        );

  if( dsatsmsi_pres_cmd == SMS_CPMS ||
      dsatsmsi_pres_cmd == SMS_CNMI)
  {
#ifdef FEATURE_ETSI_SMS_CB
#error code not present
#endif /* FEATURE_ETSI_SMS_CB */
    {
      MSG_HIGH("ATCOP SMS: Route Config Event Handler", 0, 0, 0);
      dsatsmsi_pres_cmd = SMS_NONE;
      result = DSAT_OK;
    }
  }
  else
  {
    result = DSAT_ASYNC_EVENT;
  }

  return result;
}/* dsatsmsi_route_cfg_event_handler */


/*===========================================================================

FUNCTION  DSATSMS_MEMORY_STATUS_CFG_EVENT_HANDLER

DESCRIPTION
  Handler function for memory status configuration event from WMS

DEPENDENCIES
  None

RETURN VALUE
  none

SIDE EFFECTS
  None

===========================================================================*/
/* ARGSUSED */
dsat_result_enum_type dsatsmsi_memory_status_cfg_event_handler
(
  dsat_mode_enum_type mode, /* AT command mode    */
  sms_event_info_s_type  *event_info   /* Event info ptr    */
)
{
  dsat_result_enum_type result = DSAT_ASYNC_EVENT;
#ifdef FEATURE_ETSI_SMS
#error code not present
#endif /* FEATURE_ETSI_SMS */

  MSG_HIGH("ATCOP SMS: Memory Status Config Event Handler", 0, 0, 0);

#ifdef FEATURE_ETSI_SMS
#error code not present
#endif /* FEATURE_ETSI_SMS */

#ifdef FEATURE_DSAT_SIRIUS
#error code not present
#endif /* FEATURE_DSAT_SIRIUS */

  if( dsatsmsi_pres_cmd == SMS_CPMS )
  {

    dsm_item_type * raw_rsp_ptr;
    /* the pointer to write to the rsp dsm_item */
    char * buff_ptr; 
    dsat_rsp_enum_type type_of_rsp;

    wms_memory_status_s_type * mem_status_ptr = 
      &event_info->u.mem_status;

    wms_status_e_type status;
    wms_memory_store_e_type mem_store;

    ASSERT((wms_cfg_event_e_type)event_info->event == WMS_CFG_EVENT_MEMORY_STATUS);
    /* MO template is also added now. The side effects is out of scope; 
       Please use a merciful return and ignore processing if,  MO template 
       causes any  trouble. */
    ASSERT((mem_status_ptr->tag == WMS_TAG_NONE)||
           (mem_status_ptr->tag == WMS_TAG_STATUS_RPT) ||
           (mem_status_ptr->tag == WMS_TAG_MO_TEMPLATE ));

    MSG_MED("cmd entered is %d",dsatsmsi_pres_cmd,0,0);
    /* Allocate a dsm_item and write the status to the dsm buffer */
    if ( (raw_rsp_ptr = dsm_new_buffer(DSM_DS_LARGE_ITEM_POOL)) == NULL )
    {
      MSG_ERROR("Running out of items",0,0,0);
      dsatsmsi_pres_cmd = SMS_NONE;
      /* tell cmd processor to error */
      return DSAT_ERROR;
    }

    buff_ptr = (char *) raw_rsp_ptr->data_ptr;
   

    if(dsatsmsi_mem_report.mem == CPMS_MEM1)
    {
      /* write the cmd info into the respond buffer */
      if(dsatcmdp_get_operating_cmd_mode() == ETSI_CMD_MODE )
      {
        raw_rsp_ptr->used += (word) std_strlprintf(buff_ptr,
                                           raw_rsp_ptr->size - raw_rsp_ptr->used,
                                           "+CPMS: ");
      }
      else
      {
        raw_rsp_ptr->used += (word) std_strlprintf(buff_ptr,
                                           raw_rsp_ptr->size - raw_rsp_ptr->used,
                                           "$QCPMS: ");
      }
      buff_ptr += raw_rsp_ptr->used;
    }
  
    /* this is for read command */
    if(dsatsmsi_mem_report.is_read)
    {
      const def_list_type * list_ptr = 
        dsatsmsi_get_mem_list(dsatsmsi_mem_report.mem);

      /* sanity check */
      if ( list_ptr == NULL )
      {
        /* Its a bigger problem, we are neither in ETSI mode, nor in CDMA mode */
        MSG_ERROR("Wrong operating mode  ",0,0,0);
        dsatsmsi_pres_cmd = SMS_NONE;
        return DSAT_ERROR;
      }

      raw_rsp_ptr->used += (word) std_strlprintf(buff_ptr,
               raw_rsp_ptr->size - raw_rsp_ptr->used,"\"%s\",%ld,%ld,", 
               list_ptr->list_v[dsatsmsi_get_mem_val(dsatsmsi_mem_report.mem)],
               mem_status_ptr->used_tag_slots,
               mem_status_ptr->max_slots);
    }
    else /* for write command */
    {
       raw_rsp_ptr->used += (word) std_strlprintf(buff_ptr,
                                    raw_rsp_ptr->size - raw_rsp_ptr->used,
                                    "%ld,%ld,", 
                                    mem_status_ptr->used_tag_slots,
                                    mem_status_ptr->max_slots);
    }
  
   

    /* queue the result */
    if(dsatsmsi_mem_report.mem == CPMS_MEM1)
    {
      type_of_rsp = DSAT_START_OF_RSP;
    }
    else if(dsatsmsi_mem_report.mem == CPMS_MEM2)
    {
      type_of_rsp = DSAT_INTERMEDIATE_RSP;
    }
    else if(dsatsmsi_mem_report.mem == CPMS_MEM3)
    {
      /* remove the last , */
      raw_rsp_ptr->used--;
      type_of_rsp = DSAT_END_OF_RSP;
    }
    else
    {
      MSG_ERROR("Wrong cpms mem %d",dsatsmsi_mem_report.mem,0,0);
      dsatsmsi_pres_cmd = SMS_NONE;
      return DSAT_ERROR;
    }
    
    dsatcmdp_handle_async_cmd_rsp(raw_rsp_ptr, type_of_rsp);

    if( dsatsmsi_mem_report.mem == CPMS_MEM3 )
    {
      dsatsmsi_mem_report.mem = CPMS_NONE;
      /* set route if mem3 changed */
      if( dsatsmsi_mem_report.mem3_changed )
      {
        /* Now set the routing configuration */
#ifdef FEATURE_ETSI_SMS
#error code not present
#endif /* FEATURE_ETSI_SMS */

#ifdef FEATURE_CDMA_SMS 
      if ( dsatcmdp_get_operating_cmd_mode() == CDMA_CMD_MODE )
      {
        result = dsat707smsi_route_config ( MT_VAL_0 );
      }
#endif /* FEATURE_CDMA_SMS */

      }
      else
      {
        dsatsmsi_pres_cmd = SMS_NONE;
        result = DSAT_OK;
      }
    }
    /* continue to report next memory status */
    else
    {
      dsatsmsi_mem_report.mem++;
    
      /* set mem_store */
      /* issue: hold to use lookup_mem function */
      mem_store = dsatsmsi_memory_lookup(
                  (int) dsatsmsi_get_mem_val(dsatsmsi_mem_report.mem), 
                                           dsatsmsi_mem_report.mem);
#ifdef FEATURE_ETSI_SMS
#error code not present
#endif /* FEATURE_ETSI_SMS */
      {
        status = wms_cfg_get_memory_status(dsatsmsi_client_id,
                                           dsatsmsi_cmd_cb_func,
                                           &dsatsmsi_pres_cmd,
                                           mem_store,
                                           WMS_TAG_NONE);
      }

      /* If the command is not accepted, the command callback will
       not be called, the mem event will not be invoked */
      if(status == WMS_OK_S)
      {
        result = DSAT_ASYNC_CMD;
      }
      else
      {
        result = DSAT_ERROR;
        dsatsmsi_pres_cmd = SMS_NONE;
        dsatsmsi_mem_report.mem = CPMS_NONE;
      }

    }
  }
  else
  {
    /* Neglect this information */
    result = DSAT_ASYNC_EVENT;
  }

  return result;
} /* dsatsmsi_memory_status_cfg_event_handler */


/*===========================================================================

FUNCTION  DSATSMS_MESSAGE_LIST_CFG_EVENT_HANDLER

DESCRIPTION
  Handler function for message list configuration event from WMS

DEPENDENCIES
  None

RETURN VALUE
  none

SIDE EFFECTS
  None

===========================================================================*/
/* ARGSUSED */
dsat_result_enum_type dsatsmsi_message_list_cfg_event_handler
(
 dsat_mode_enum_type mode, /* AT command mode    */
  sms_event_info_s_type  *event_info   /* Event info ptr    */
)
{
  dsat_result_enum_type result = DSAT_OK;
  dsm_item_type * res_buff;
  byte * data_ptr;
  int num_of_indices;
  int loop = 0;
  
  DEBUG(
        MSG_MED("memory_list_cfg...ndler, pres_cmd:%d ",
                dsatsmsi_pres_cmd, 0, 0);
        );

  if ( dsatsmsi_pres_cmd != SMS_CMGL &&
       dsatsmsi_pres_cmd != SMS_CMGD )
  {
#ifdef FEATURE_ETSI_SMS_TEMPLATE
#error code not present
#endif /* FEATURE_ETSI_SMS_TEMPLATE */
    {
      /* unwanted event */
      return DSAT_ASYNC_EVENT;
    }
  }

  MSG_HIGH("ATCOP SMS: Message List Config Event Handler", 0, 0, 0);

  /* Store the list if it is for a CMGL command */
  if ( dsatsmsi_pres_cmd == SMS_CMGL )
  {
    if ( event_info->u.msg_list.tag != WMS_TAG_NONE )
    {
      dsatsmsi_cmd_sm.num_of_indices = event_info->u.msg_list.len;
      dsatsmsi_cmd_sm.present_index = 0;
      MSG_MED("cmd with tag %d,number of indices %d,",
               event_info->u.msg_list.tag,dsatsmsi_cmd_sm.num_of_indices,0);
      if( event_info->u.msg_list.len == 0 )
      {
        /* We got a zero length list                 
         * Check to see is we got more stats to read */
        if ( ! dsatsmsi_get_msg_list() )
        {
          /* We are done with the list, clean up */
          if ( dsatsmsi_cmd_sm.list_begin )
          {
            dsatsmsi_cmd_sm.list_begin = FALSE;
          }
          else
          {
            if( (res_buff = dsm_new_buffer( DSM_DS_LARGE_ITEM_POOL )) == NULL)
            {
              ERR_FATAL( "Could not get a new DSM item", 0, 0, 0);
            }
            else
            {
              res_buff->used = 0;
              dsatcmdp_handle_async_cmd_rsp(res_buff, DSAT_END_OF_RSP);
            }
          }
          dsatsmsi_pres_cmd = SMS_NONE;
#ifdef FEATURE_DSAT_SIRIUS
#error code not present
#endif /* FEATURE_DSAT_SIRIUS */
          memset ( &dsatsmsi_cmd_sm, 0, sizeof(sms_cmd_sm_type) );
          return DSAT_OK;
        }
        else
        {
          return DSAT_ASYNC_CMD;
        }
      }
    
      for( loop = 0 ; loop < dsatsmsi_cmd_sm.num_of_indices ; loop++ )
      {
        dsatsmsi_cmd_sm.indices[loop] = 
          event_info->u.msg_list.indices[loop];
      }
    
      /* Read the message list */
      (void)dsatsmsi_read_msg_list();

      /* Tell ATCOP that there is more to come */
      result = DSAT_ASYNC_CMD;
    }
    else
    {
      result = DSAT_ASYNC_EVENT;
    }
  } /* (dsatsmsi_pres_cmd == SMS_CMGL) */
  else if(( dsatsmsi_pres_cmd == SMS_CMGD ) && ( !dsatsmsi_cmd_sm.one_index ))
  {
    if ( dsatsmsi_cmd_sm.is_test == 0 )
    {
      /* Check to see if this the message list we requested 
       * WMS sends the message_list_config_event_handler with
       * tag WMS_TAG_STATUS_RPT when we request for a MO_SENT 
       * tagged messages. We should not further process that event
       */

      if( ( event_info->u.msg_list.tag != WMS_TAG_NONE ) &&
          ( event_info->u.msg_list.tag != WMS_TAG_STATUS_RPT ))
      {
        /* We are deleting with a del flag           */
        /* Get the list and iterate through the list */
        dsatsmsi_cmd_sm.num_of_indices = event_info->u.msg_list.len;
        dsatsmsi_cmd_sm.present_index = 0;
    
        for( loop = 0 ; loop < dsatsmsi_cmd_sm.num_of_indices ; loop++ )
        {
          dsatsmsi_cmd_sm.indices[loop] = 
            event_info->u.msg_list.indices[loop];
        }
    
        /* got the list            */
        /* now delete the messages */
        result = dsatsmsi_delete_msg_list();

        if ( result == DSAT_OK )
        {
          /* We are done with the commands, clean up */
          /* send the response to the user */
          dsatsmsi_pres_cmd = SMS_NONE;
          memset ( &dsatsmsi_cmd_sm, 0, sizeof(sms_cmd_sm_type) );
          return result;
        }
        else if ( result == DSAT_CMD_ERR_RSP )
        {
          if( (res_buff = dsm_new_buffer( DSM_DS_LARGE_ITEM_POOL )) == NULL)
          {
            ERR_FATAL( "Could not get a new DSM item", 0, 0, 0);
          }
          else
          {
            /* we got an error, format a +CMS ERROR */
            res_buff->used += (byte)dsatsmsi_cms_error(res_buff,
                                                       (int)DSAT_CMS_UNKNOWN_ERR);
            /* send the response to the user */
              dsatcmdp_handle_async_cmd_rsp(res_buff, DSAT_COMPLETE_RSP);
            /* We are done with the commands, clean up */
            dsatsmsi_pres_cmd = SMS_NONE;
            memset ( &dsatsmsi_cmd_sm, 0, sizeof(sms_cmd_sm_type) );
            return result;
          }
        }
        else
        {
          /* we got some more work to do */
          return result;
        }
      } /* if (! TAG_NONE) */
      else
      {
        return DSAT_ASYNC_EVENT;
      }
    } /* if(dsatsmsi_cmd_sm.is_test == 0) */
    else
    {
      /* this is a CMGD TEST command */
      if( (res_buff = dsm_new_buffer( DSM_DS_LARGE_ITEM_POOL )) == NULL)
      {
        ERR_FATAL( "Could not get a new DSM item", 0, 0, 0);
      }
      else
      {
        data_ptr = res_buff->data_ptr;

        num_of_indices = event_info->u.msg_list.len;

        if ( dsatcmdp_get_operating_cmd_mode() == CDMA_CMD_MODE )
        {
          res_buff->used = (word)std_strlprintf((char*)data_ptr,
                                                res_buff->size,
                                                "$QCMGD: ");
        }
        else
        {
          res_buff->used = (word)std_strlprintf((char*)data_ptr,
                                                res_buff->size,
                                                "+CMGD: ");
        }

        data_ptr +=res_buff->used;
         
        *data_ptr++ = '(';

        for( loop = 0 ; loop < num_of_indices ; loop++ )
        {
          data_ptr = dsatutil_itoa(event_info->u.msg_list.indices[loop],
                                   data_ptr,
                                   10);
          *data_ptr++ = ',';
        }
      
        /* Overwrite last ',' from the buffer */
        if (num_of_indices > 0 )
        {
          data_ptr--;
        }

        *data_ptr++ = ')';
        res_buff->used = (byte)(data_ptr - res_buff->data_ptr);

        *data_ptr++ = ',';
        res_buff->used++;
        res_buff->used+= (word)std_strlprintf((char*)data_ptr,
                                              res_buff->size - res_buff->used,
                                              "(0-4)");

        
        /* send the response to the user */
        dsatcmdp_handle_async_cmd_rsp(res_buff,DSAT_COMPLETE_RSP);
        /* We are done with the commands, clean up */
        dsatsmsi_pres_cmd = SMS_NONE;
        memset ( &dsatsmsi_cmd_sm, 0, sizeof(sms_cmd_sm_type) );
        result = DSAT_OK;
      }
    }
  } /* (dsatsmsi_pres_cmd == SMS_CMGD) */
  /* Its OK to neglect rest of the message list notifications */
  else 
  {
    result = DSAT_ASYNC_EVENT;
  }

  return result;
} /* dsatsmsi_message_list_cfg_event_handler */

#ifdef FEATURE_ETSI_SMS
#error code not present
#endif /* FEATURE_ETSI_SMS */

#ifdef FEATURE_ETSI_SMS_PS
#error code not present
#endif /* FEATURE_ETSI_SMS_PS */

/*===========================================================================
FUNCTION  EVENT_CHECK

DESCRIPTION
  Checks to see if the event we received is WMS_CFG_EVENT_GW_READY or
  WMS_CFG_EVENT_GW_DOMAIN_PREF

DEPENDENCIES
  None

RETURNS
  TRUE :if the event is WMS_CFG_EVENT_GW_DOMAIN_PREF or WMS_CFG_EVENT_GW_READY
  FALSE :if it is not.

SIDE EFFECTS
  None
===========================================================================*/
/* ARGSUSED */
LOCAL boolean event_check 
(
  wms_cfg_event_e_type      event,     /* Event ID                   */
  wms_message_tag_e_type    tag        /* Tag : used in message list */
)
{
  if ( event == WMS_CFG_EVENT_GW_READY ||
       event == WMS_CFG_EVENT_GW_DOMAIN_PREF ||
       event == WMS_CFG_EVENT_LINK_CONTROL  
#ifdef FEATURE_ETSI_SMS_TEMPLATE
#error code not present
#endif /* FEATURE_ETSI_SMS_TEMPLATE */
      ) 
  {
    return TRUE;
  }
  else
  {
    return FALSE;
  }
}/* event_check */

/*===========================================================================
FUNCTION  CFG_EVENT_LOOKUP

DESCRIPTION
  Maps the wms_cfg_event_e_type to sms_event_e_type

DEPENDENCIES
  None

RETURNS
  sms_event_e_type

SIDE EFFECTS
  None
===========================================================================*/
sms_event_e_type cfg_event_lookup
( 
 wms_cfg_event_e_type wms_event /* Event ID */
)
{
  sms_event_e_type atcop_event;

  switch ( wms_event )
  {
  case WMS_CFG_EVENT_GW_READY:
    atcop_event = SMS_CFG_EVENT_GW_READY;
    break;
  case WMS_CFG_EVENT_ROUTES:
    atcop_event = SMS_CFG_EVENT_ROUTES;
    break;
  case WMS_CFG_EVENT_MEMORY_STATUS:
    atcop_event = SMS_CFG_EVENT_MEMORY_STATUS;
    break;
  case WMS_CFG_EVENT_MESSAGE_LIST:
    atcop_event = SMS_CFG_EVENT_MESSAGE_LIST;
    break;
  case WMS_CFG_EVENT_LINK_CONTROL:
    atcop_event = SMS_CFG_EVENT_LINK_CONTROL;
    break; 
#ifdef FEATURE_ETSI_SMS_PS
#error code not present
#endif /* FEATURE_ETSI_SMS_PS */
#ifdef FEATURE_DSAT_SIRIUS
#error code not present
#endif /* FEATURE_DSAT_SIRIUS */
  default:
    atcop_event = SMS_EVENT_NONE;
    break;
  }

  return atcop_event;
}

/*===========================================================================
FUNCTION  MSG_EVENT_LOOKUP

DESCRIPTION
  Maps the wms_msg_event_e_type to sms_event_e_type

DEPENDENCIES
  None

RETURNS
  sms_event_e_type

SIDE EFFECTS
  None
===========================================================================*/
sms_event_e_type msg_event_lookup
(
 wms_msg_event_e_type wms_event /* Event ID */
)
{
  sms_event_e_type atcop_event;

  switch ( wms_event )
  {
    case WMS_MSG_EVENT_SEND:
      atcop_event = SMS_MSG_EVENT_SEND;
      break;
    case WMS_MSG_EVENT_ACK:
      atcop_event = SMS_MSG_EVENT_ACK;
      break;
    case WMS_MSG_EVENT_READ:
      atcop_event = SMS_MSG_EVENT_READ;
      break;
    case WMS_MSG_EVENT_WRITE:
      atcop_event = SMS_MSG_EVENT_WRITE;
      break;
    case WMS_MSG_EVENT_DELETE:
      atcop_event = SMS_MSG_EVENT_DELETE;
      break;
    case WMS_MSG_EVENT_MODIFY_TAG:
      atcop_event = SMS_MSG_EVENT_MODIFY_TAG;
      break;
    case WMS_MSG_EVENT_READ_TEMPLATE:
      atcop_event = SMS_MSG_EVENT_READ_TEMPLATE;
      break;
    case WMS_MSG_EVENT_WRITE_TEMPLATE:
      atcop_event = SMS_MSG_EVENT_WRITE_TEMPLATE;
      break;
    case WMS_MSG_EVENT_DELETE_TEMPLATE:
      atcop_event = SMS_MSG_EVENT_DELETE_TEMPLATE;
      break;
    case WMS_MSG_EVENT_RECEIVED_MESSAGE:
      atcop_event = SMS_MSG_EVENT_RECEIVED_MESSAGE;
      break;
    case WMS_MSG_EVENT_SUBMIT_REPORT:
      atcop_event = SMS_MSG_EVENT_SUBMIT_REPORT;
      break;
    case WMS_MSG_EVENT_STATUS_REPORT:
      atcop_event = SMS_MSG_EVENT_STATUS_REPORT;
      break;
    case WMS_MSG_EVENT_MT_MESSAGE_ERROR:
      atcop_event = SMS_MSG_EVENT_MT_MESSAGE_ERROR;
      break;
    case WMS_MSG_EVENT_READ_STS_REPORT:
      atcop_event = SMS_MSG_EVENT_READ_STS_REPORT;
      break;
    case WMS_MSG_EVENT_WRITE_STS_REPORT:
      atcop_event = SMS_MSG_EVENT_WRITE_STS_REPORT;
      break;
    default:
      atcop_event = SMS_EVENT_NONE;
      break;
  }

  return atcop_event;
  }

#ifdef FEATURE_ETSI_SMS_CB
#error code not present
#endif /* FEATURE_ETSI_SMS_CB */

/*===========================================================================
FUNCTION  SETUP_SMS_WATERMARKS

DESCRIPTION
  This function initializes the watermarks used for Receiving SMS data
  from WMS. The lowater, hiwater and don't exceed counts are configured. 
  Queue to be used by WM is also initialized

DEPENDENCIES
  None

RETURNS
  None

SIDE EFFECTS
  None
===========================================================================*/
LOCAL void setup_sms_watermarks( void )
{

  /*-------------------------------------------------------------------------
    Initialize, set Don't Exceed count and register the associated queue 
    for storing MT messages.
  -------------------------------------------------------------------------*/
  dsm_queue_init(&dsat_mt_msg_wm, DSAT_FROM_WMS_DONT_EXCEED_CNT, &dsat_mt_msg_q);

  /*-------------------------------------------------------------------------
    Set the Lo Water and Hi Water counts for the 
    MT message watermark.
  -------------------------------------------------------------------------*/
  dsat_mt_msg_wm.lo_watermark    =  DSAT_FROM_WMS_LOWATER_CNT;
  dsat_mt_msg_wm.hi_watermark    =  DSAT_FROM_WMS_HIWATER_CNT;

  /*-------------------------------------------------------------------------
    Initialize the watermark internal byte counts for the MT message watermark.
  -------------------------------------------------------------------------*/
  dsat_mt_msg_wm.current_cnt = 0;
#ifdef FEATURE_DSM_MEM_CHK
  dsat_mt_msg_wm.highest_cnt = 0;
  dsat_mt_msg_wm.total_rcvd_cnt = 0;
#endif /* FEATURE_DSM_MEM_CHK */

  /*-------------------------------------------------------------------------
    Register callback functions for the MT message watermark.
  -------------------------------------------------------------------------*/
  /*lint -save -e64 suppress error 64*/
  dsat_mt_msg_wm.lowater_func_ptr      = sms_wm_lowater_cb;
  dsat_mt_msg_wm.hiwater_func_ptr      = sms_wm_hiwater_cb;
  dsat_mt_msg_wm.each_enqueue_func_ptr = NULL;
  dsat_mt_msg_wm.gone_empty_func_ptr   = NULL;
  dsat_mt_msg_wm.non_empty_func_ptr    = sms_mt_msg_wm_non_empty_cb;
  /*lint -restore suppress error 64*/

  /*-------------------------------------------------------------------------
    Initialize, set Don't Exceed count and register the associated queue 
    for storing other than MT messages.
  -------------------------------------------------------------------------*/
  dsm_queue_init(&dsat_sms_wm, DSAT_FROM_WMS_DONT_EXCEED_CNT, &dsat_sms_q);

  /*-------------------------------------------------------------------------
    Set up the watermark and queue for SMS events other than 
    MT message from WMS. This watermark will not be flow controlled.
  -------------------------------------------------------------------------*/
  dsat_sms_wm.lo_watermark    =  0;
  dsat_sms_wm.hi_watermark    =  DSAT_FROM_WMS_DONT_EXCEED_CNT;
  dsat_sms_wm.current_cnt = 0;
#ifdef FEATURE_DSM_MEM_CHK
  dsat_sms_wm.highest_cnt = 0;
  dsat_sms_wm.total_rcvd_cnt = 0;
#endif /* FEATURE_DSM_MEM_CHK */

  dsat_sms_wm.lowater_func_ptr      = NULL;
  dsat_sms_wm.hiwater_func_ptr      = NULL;
  dsat_sms_wm.each_enqueue_func_ptr = NULL;
  dsat_sms_wm.gone_empty_func_ptr   = NULL;
  /*lint -save -e64 suppress error 64*/
  dsat_sms_wm.non_empty_func_ptr    = sms_wm_non_empty_cb;
  /*lint -restore suppress error 64*/

} /* setup_sms_watermarks */
 
/* <EJECT> */
/*===========================================================================

FUNCTION SMS_WM_HIWATER_CB

DESCRIPTION
  This is the high watermark function registered with the actual watermark.
  It is added to deregister WMS callbacks once watermark goes full.
         
DEPENDENCIES
  None
  
RETURN VALUE
  None
    
SIDE EFFECTS
  None

===========================================================================*/
void sms_wm_hiwater_cb ( void )
  {
  /* Set flag indicating received messages should be dropped */
  sms_mt_msg_wm_full = TRUE;
  }

/* <EJECT> */
/*===========================================================================

FUNCTION SMS_WM_LOWATER_CB

DESCRIPTION
  This is the low watermark function registered with the actual watermark.
  It is added to re-register for WMS callbacks once watermark goes low.
         
DEPENDENCIES
  None
  
RETURN VALUE
  None
    
SIDE EFFECTS
  None

===========================================================================*/
void sms_wm_lowater_cb ( void )
  {
  /* Set flag indicating received messages should no longer be dropped */
  sms_mt_msg_wm_full = FALSE;
  }


/*===========================================================================

FUNCTION SMS_MT_MSG_WM_NON_EMPTY_CB

DESCRIPTION
  Callback function invoked when the received message watermark goes non-empty.

  This function simply sets the received MT message signal.

DEPENDENCIES
  None

RETURN VALUE
  None

SIDE EFFECTS
  None

===========================================================================*/
LOCAL void sms_mt_msg_wm_non_empty_cb ( void )
{

  (void)rex_set_sigs( &ds_tcb, DS_AT_MT_MSG_SIG );

} /* sms_mt_msg_wm_non_empty_cb */


/*===========================================================================

FUNCTION SMS_WM_NON_EMPTY_CB

DESCRIPTION
  Callback function invoked when the SMS event watermark item goes 
  non-empty.

  This function simply sets the other, non-"MT message" SMS event signal.

DEPENDENCIES
  None

RETURN VALUE
  None

SIDE EFFECTS
  None

===========================================================================*/
LOCAL void sms_wm_non_empty_cb ( void )
{

  (void)rex_set_sigs( &ds_tcb, DS_AT_SMS_SIG );

} /* sms_wm_non_empty_cb */

#ifdef FEATURE_ETSI_SMS_TEMPLATE
#error code not present
#endif /* FEATURE_ETSI_SMS_TEMPLATE */

#ifdef FEATURE_DSAT_SIRIUS
#error code not present
#endif /* FEATURE_DSAT_SIRIUS */

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
 sms_mt_msg_e_type       msg_type,   /* Type of message      */
 wms_message_index_type  msg_index,  /* Index Number         */
 wms_memory_store_e_type mem_store,  /* Type of Memory Store */
 wms_transaction_id_type tid         /* Transaction ID       */
)
{
  sms_mt_msg_type * msg_ptr;

  /* Get a command buffer from the free command queue */
  if( (msg_ptr = q_get( &sms_mt_msg_free_q )) == NULL )
  {
    /* No free command buffers available                */
    /* Get the tip, start adding the new element at tip */

    /* Get the first element in the actual queue */
    msg_ptr = q_get( &sms_mt_msg_q );

    /* Add data to it */
    msg_ptr->msg_type = msg_type;
    msg_ptr->index = msg_index;
    msg_ptr->mem_store = mem_store;
    msg_ptr->tid = tid;
   
    /* Put the message on the message queue */
    q_put( &sms_mt_msg_q, &msg_ptr->link );

  }
  else
  {
    msg_ptr->msg_type = msg_type;
    msg_ptr->index = msg_index;
    msg_ptr->mem_store = mem_store;
    msg_ptr->tid = tid;

    /* Put the message on the message queue */
    q_put( &sms_mt_msg_q, &msg_ptr->link );
  }

  return;
} /* add_mt_msg_to_queue */



#endif /* defined(FEATURE_ETSI_SMS) || defined(FEATURE_CDMA_SMS) */
#endif /* FEATURE_DATA */
