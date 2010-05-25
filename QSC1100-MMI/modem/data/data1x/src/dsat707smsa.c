/*===========================================================================

                        D A T A   S E R V I C E S
                A T   C O M M A N D   P R O C E S S O R

DESCRIPTION
  This module executes AT commands. It executes IS-707 specific sms related
  AT commands.

  The cmd is not specified in 707 spec.  We (qcom) add them to automate sms
  testing.

EXTERNALIZED FUNCTIONS

  None

 Copyright (c) 2009 by QUALCOMM Incorporated.
 All Rights Reserved.
 Qualcomm Confidential and Proprietary.

===========================================================================*/


/*===========================================================================

                      EDIT HISTORY FOR FILE

  $Header: //source/qcom/qct/modem/data/common/1xatcop/main/lite/src/dsat707smsa.c#5 $ $DateTime: 2009/04/02 05:26:19 $ $Author: nchagant $
		      
  This section contains comments describing changes made to the module.
  Notice that changes are listed in reverse chronological order.

when       who     what, where, why
--------   ---     ----------------------------------------------------------
02/17/09   nc      Fixing Lint Criticals.
01/28/09   nc      Fixing Compilation Warnings
01/21/09   nc      Added support for SMS Commands. 

===========================================================================*/
#include "comdef.h"
#include "customer.h"
#include "AEEstd.h"

#ifdef FEATURE_CDMA_SMS

#include "dsati.h"
#include "dsatsmsi.h"
#include "dsat707sms.h"
#include "dsat707smsctab.h"
#include "dsatparm.h"
#include "err.h"
#include "wms.h"
#include "string.h"
#include "stdio.h"
#include "dsm.h"

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

/*===========================================================================
            Data types
===========================================================================*/

/* import variable from other files of sms module */
/* Variable to find out what command is being processed currently */
extern sms_cmd_name_e_type dsatsmsi_pres_cmd;

/* WMS client (ATCOP) ID */
extern wms_client_id_type dsatsmsi_client_id;

/* Variable used in QCMGL SM */
extern sms_cmd_sm_type  dsatsmsi_cmd_sm;

/* Variable used QCMSS Command */
extern wms_address_s_type  send_addr;

/* SMS Mobile Terminated message queue */
extern q_type sms_mt_msg_q;         

/* Queue for storing free message buffers */ 
extern q_type sms_mt_msg_free_q;

/* LOCAL DECLARATIONS: */
LOCAL const char cmgr_hdr[] = "$QCMGR: ";
LOCAL const char cmgs_hdr[] = "$QCMGS: ";
LOCAL const char cmss_hdr[] = "$QCMSS: ";

/* Variable to retain  message reference number for send command */
LOCAL dsat_num_item_type  is707_send_mr;

/* Data structure used primarily to replace the <da> */
LOCAL wms_client_message_s_type replace707_da_msg;
LOCAL wms_client_ts_data_s_type temp707_ts_data;

/* Data structure for CDMA Bearer and Transport data */
LOCAL wms_client_bd_s_type           cl_bd_data ;
LOCAL wms_client_ts_data_s_type      cl_ts_data ;

/* LOCAL function prototypes */
LOCAL dsat_result_enum_type is707_send_msg_event_handler 
(
 dsat_mode_enum_type mode,           /* AT command mode    */
 sms_event_info_s_type * event_info  /* Event info ptr    */
);

LOCAL dsat_result_enum_type is707_submit_msg_report_event_handler 
(
 dsat_mode_enum_type mode,           /* AT command mode    */
 sms_event_info_s_type * event_info  /* Event info ptr    */
);

LOCAL dsat_result_enum_type is707_received_msg_event_handler 
(
 dsat_mode_enum_type mode,           /* AT command mode    */
 sms_event_info_s_type * event_info  /* Event info ptr    */
);

LOCAL dsat_result_enum_type is707_read_msg_event_handler 
(
 dsat_mode_enum_type mode,           /* AT command mode    */
 sms_event_info_s_type * event_info  /* Event info ptr    */
);

LOCAL dsat_result_enum_type is707_delete_msg_event_handler 
(
 dsat_mode_enum_type mode,           /* AT command mode    */
 sms_event_info_s_type * event_info  /* Event info ptr    */
);

LOCAL dsat_result_enum_type is707_write_msg_event_handler 
(
 dsat_mode_enum_type mode,           /* AT command mode    */
 sms_event_info_s_type * event_info  /* Event info ptr    */
);

LOCAL void dsat707smsi_send_unsol_mt_msg 
(
 sms_event_info_s_type * event_info     /* Event info ptr    */
);

/* the Message Event Handler lookup table */
/* note that the order of the functions has to match wms_msg_event_e_type */
const msg_handler_tab_type dsat707sms_msg_handler_table[] = 
{
  { SMS_CFG_EVENT_ROUTES,           dsatsmsi_route_cfg_event_handler       },
  { SMS_CFG_EVENT_MEMORY_STATUS,    dsatsmsi_memory_status_cfg_event_handler},
  { SMS_CFG_EVENT_MESSAGE_LIST,     dsatsmsi_message_list_cfg_event_handler },
  { SMS_MSG_EVENT_SEND,             is707_send_msg_event_handler           },
  { SMS_MSG_EVENT_READ,             is707_read_msg_event_handler           },
  { SMS_MSG_EVENT_DELETE,           is707_delete_msg_event_handler         },
  { SMS_MSG_EVENT_RECEIVED_MESSAGE, is707_received_msg_event_handler       },
  { SMS_MSG_EVENT_SUBMIT_REPORT,    is707_submit_msg_report_event_handler  },
  { SMS_MSG_EVENT_WRITE,            is707_write_msg_event_handler          }
};

const unsigned int dsat707sms_msg_handler_table_size = 
ARR_SIZE(dsat707sms_msg_handler_table);


/*===========================================================================

FUNCTION  DSAT707SMS_INIT_SMS

DESCRIPTION
  ETSI specific SMS initialization.
  
DEPENDENCIES
  None

RETURN VALUE
  none

SIDE EFFECTS
  None

===========================================================================*/
void dsat707sms_init_sms ( void )
{
#ifdef FEATURE_THIN_UI
/* This feature define caters to the routing configuration with no UI */
/* Refer to 3g TS 27.005 section 3.4.1 and table 2                    */
  (void) dsat707smsi_route_config ( 1 );
#endif /* FEATURE_THIN_UI */

  /* Initialize the mobile terminated message queue */
  mt_msg_queue_init();
} /* dsat707sms_init_sms */



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
void dsat707sms_flush_sms_indications
(
 void
)
{
  sms_mt_msg_type  *msg_ptr;
  
  /* Check to see the user preferences        */
  /* Flush the indications only if <bfr> is 0 */

  /* The formatting of the response is done based on the current */
  /* functionality that is supported: <mt>: 0, 1 and <ds>: 0, 2  */
  /* We need to check for <mt> and <ds> for formatting the resp  */


  if( (dsat707sms_qcnmi_val[2] == 0) && (dsat707sms_qcnmi_val[0] != 0) )
  {
    /* Flush the queued indications to TE */

    /* Get messages from the queue until the queue is empty */
    while( (msg_ptr = (sms_mt_msg_type *)q_get( &sms_mt_msg_q )) != NULL )
    {
      /* Check to see if user selected the option */
      /* to route DELIVER or STATUS-REPORT        */
      if( (msg_ptr->msg_type == MT_DELIVER) && (dsat707sms_qcnmi_val[1] == 0))
      {
        /* We are in do not send the msg to TE mode    */
        /* Return the message buffer to the free queue */
        MSG_HIGH("Buffer mode, add it to local queue", 0, 0, 0 );
        q_put( &sms_mt_msg_free_q, &msg_ptr->link );
        continue;
      }
      else
      {
        /* Send the indication to TE */
        MSG_HIGH( "Sending the indication to TE", 0, 0, 0 );
        dsatsmsi_send_async_indication_to_te (
                                      msg_ptr->msg_type,
                                      msg_ptr->index,
                                      msg_ptr->mem_store 
                                    );
    
        /* Return the message buffer to the free queue */
        q_put( &sms_mt_msg_free_q, &msg_ptr->link );
      }
    }/* while */
  }
  else if( (dsat707sms_qcnmi_val[2] == 1) && (dsat707sms_qcnmi_val[0] != 0) )
  {
    /* Clear the queued indications */
    while( (msg_ptr = (sms_mt_msg_type *)q_get( &sms_mt_msg_q )) != NULL )
    {
      /* Return the message buffer to the free queue */
      q_put( &sms_mt_msg_free_q, &msg_ptr->link );
    }
  }
  else
  {
    /* Do nothing. Just return */
    return;
  }

  return;
}/* dsat707sms_flush_sms_indications */

/*===========================================================================

FUNCTION DSAT707SMSI_SEND_UNSOL_MT_MSG

DESCRIPTION
  This Function sends a unsolicited Indication to TE on a reception of MT SMS
  without storing it in the TA.

DEPENDENCIES
  None

RETURN VALUE
  None

SIDE EFFECTS
  
===========================================================================*/

void dsat707smsi_send_unsol_mt_msg 
(
 sms_event_info_s_type * event_info     /* Event info ptr    */
)
{
  dsm_item_type * res_buff;
  dsm_item_type * formatted_res_buff;
  wms_status_e_type msg_status;
  byte *data_ptr;
  word dsat_sms_len = 0;
  uint8 addr_len=0;
  
  if (event_info == NULL)
  {
    MSG_HIGH("Event_info is Invalid",0,0,0);
    return;
  }
  memset( &cl_bd_data, 0, sizeof(wms_client_bd_s_type) );
  memset( &temp707_ts_data, 0, sizeof(wms_client_ts_data_s_type) );

  event_info->u.mt_msg.u.cdma_message.address.number_of_digits = 
    MIN( event_info->u.mt_msg.u.cdma_message.address.number_of_digits, 
         WMS_ADDRESS_MAX );

   /*
      In 4 Digit Mode addressing as follows:
      Number   |    4 BIT DTMF       |    ASCII conversion  
               |  conversion by WMS  |
      -------- |-----------------    |------------------- 
       *1204#  |   11 01 02 10 04 12 |   66 49 50 65 52 35
   */
  addr_len = 
    MIN( event_info->u.mt_msg.u.cdma_message.address.number_of_digits, 
         WMS_ADDRESS_MAX );
  
  if(event_info->u.mt_msg.u.cdma_message.address.digit_mode 
                                                   == WMS_DIGIT_MODE_4_BIT)
  {
    addr_len = wms_ts_dtmf2ascii(
         addr_len,
         (const uint8 *) &event_info->u.mt_msg.u.cdma_message.address.digits[0],
         (uint8 *) &event_info->u.mt_msg.u.cdma_message.address.digits[0]);
  }
  if( addr_len > WMS_ADDRESS_MAX - 1)
  {
    MSG_HIGH ("ERROR in converting dtmf to ascii as addr_len is %d", addr_len, 0, 0);
    return ;
  }
  event_info->u.mt_msg.u.cdma_message.address.digits[addr_len] = '\0';
  event_info->u.mt_msg.u.cdma_message.address.number_of_digits = addr_len;

  msg_status = wms_ts_decode (&event_info->u.mt_msg.u.cdma_message.raw_ts,
                              &temp707_ts_data );
  if ( msg_status != WMS_OK_S )
  {
    MSG_HIGH ("ERROR in decoding received message ", 0, 0, 0);
    return ;
  }

  
  if((temp707_ts_data.u.cdma.user_data.encoding == WMS_ENCODING_ASCII) ||
               (temp707_ts_data.u.cdma.user_data.encoding == WMS_ENCODING_IA5))
  {
    cl_bd_data.user_data.data_len =
          wms_ts_unpack_ascii((wms_cdma_user_data_s_type*)&temp707_ts_data.u.cdma.user_data,
                              temp707_ts_data.u.cdma.user_data.number_of_digits+1,
                              cl_bd_data.user_data.data);
  }
  if( (res_buff = dsm_new_buffer( DSM_DS_LARGE_ITEM_POOL )) == NULL)
  {
    ERR_FATAL( "Could not get a new DSM item", 0, 0, 0);
  }
  else
  {
    data_ptr = res_buff->data_ptr;
    data_ptr += (word)std_strlprintf((char*)data_ptr,
                                       res_buff->size - res_buff->used,
                                       "$QCMT: ");
    *data_ptr++ = ',';
    
    /* Update the Result with <stat> */
    data_ptr = dsatsmsi_tag_lookup ( 
                       event_info->u.mt_msg.msg_hdr.tag, 0, data_ptr );
    *data_ptr++ = ','; 

    /* Update the Result with <oa> */
    data_ptr += (word)std_strlprintf(
                                (char*)data_ptr, 
                                (res_buff->size - res_buff->used),
                                "\"%s\"",
                                &event_info->u.mt_msg.u.cdma_message.address.digits[0]);

    *data_ptr++ = ','; 
    /* Update the Result with <scts> */
    data_ptr = dsatsmsi_timestamp( temp707_ts_data.u.cdma.mc_time ,data_ptr );
    *data_ptr++ = ',';
    /* Update the Result with <length> */
    data_ptr = dsatutil_itoa((uint32) cl_bd_data.user_data.data_len, data_ptr, 10);
    *data_ptr++ = '\n';

    res_buff->used += (uint16) ( data_ptr - res_buff->data_ptr );
    /* Update the Result with <data> */
    
    if(temp707_ts_data.u.cdma.user_data.encoding == WMS_ENCODING_OCTET ||
       temp707_ts_data.u.cdma.user_data.encoding == WMS_ENCODING_ASCII ||
       temp707_ts_data.u.cdma.user_data.encoding == WMS_ENCODING_GSM_7_BIT_DEFAULT)
    {
    
       dsat_sms_len = dsm_pushdown_tail( &res_buff, (void*)cl_bd_data.user_data.data,
                                         cl_bd_data.user_data.data_len,
                                          DSM_ITEM_POOL(res_buff) );
      MSG_MED("Received SMS length is %d",dsat_sms_len,0,0);
    }
    
    
    /* We are done with filling up raw response buffer */
    /* Format it and send it to TE                     */
    if( (formatted_res_buff = dsm_new_buffer( DSM_DS_LARGE_ITEM_POOL )) 
         == NULL)
    {
      ERR_FATAL( "Could not get a new DSM item", 0, 0, 0);
    }
    /* send the response to the user */
    dsat_fmt_response( &res_buff, DSAT_COMPLETE_RSP, FALSE, 
                       &formatted_res_buff );
    
    if ( formatted_res_buff == NULL )
    {
      ERR_FATAL( "Running out of buffers",0,0,0 );
    }
    
    /* Flush the queue to the Rm interface. */
    dsatcmdp_send_queued_cmd_results( );

    if ( formatted_res_buff->used != 0 )
    {
#ifdef FEATURE_DATA_SERIALIZER
           /* Tell SIOLIB that this is SMS and should be sent to Modem port */
            ds3g_siolib_set_tx_type 
              ( DS3G_SIOLIB_UNSOLICTED_SMS, DS3G_SIOLIB_DATA_PORT );
#endif /* FEATURE_DATA_SERIALIZER */
            dsatrsp_send_response( formatted_res_buff, TRUE );
    }
    else
    {
      dsm_free_packet( &formatted_res_buff );
    }
  }
  return;
}



/*===========================================================================

FUNCTION  IS707_SEND_MSG_EVENT_HANDLER

DESCRIPTION
  Handler function for send message event from WMS.

DEPENDENCIES
  None

RETURN VALUE
  none

SIDE EFFECTS
  None

===========================================================================*/
/* ARGSUSED */
LOCAL dsat_result_enum_type is707_send_msg_event_handler
(
 dsat_mode_enum_type     mode,              /* AT command mode    */
 sms_event_info_s_type * event_info     /* Event info ptr    */
)
{
  wms_client_ts_data_s_type     msg_ptr;
  dsat_result_enum_type         result = DSAT_ASYNC_EVENT;
  wms_status_e_type             decode_status;
  dsm_item_type                 * res_buff;
  
/*-------------------------------------------------------------------------*/

  MSG_HIGH("is707_send_msg_event_handler, pres_cmd: %d",
                dsatsmsi_pres_cmd,0,0);

  if( event_info == NULL )
  {
    ERR_FATAL("Null Event_info",0,0,0);
  }
  
  if ( ( dsatsmsi_pres_cmd == SMS_CMGS ) ||
       ( dsatsmsi_pres_cmd == SMS_CMSS ) )
  {

    event_info->u.msg_status.u.cdma_message.raw_ts.format = WMS_FORMAT_CDMA;
   
    memset( &msg_ptr, 0, sizeof(wms_client_ts_data_s_type) );
    decode_status = wms_ts_decode(
                        &event_info->u.msg_status.u.cdma_message.raw_ts, 
                         &msg_ptr);

    if ( decode_status == WMS_OK_S )
    {
      /* Retain Useful information from thie event            */
      /* Wait for REPORT from sms server and then send it to the user */

      /* for submit status */
      is707_send_mr = msg_ptr.u.cdma.message_id.id_number;

      result = DSAT_ASYNC_CMD;
    }
    else
    {
      if( (res_buff = dsm_new_buffer( DSM_DS_LARGE_ITEM_POOL )) == NULL)
      {
        ERR_FATAL( "Could not get a new DSM item", 0, 0, 0);
      }
      else
      {
        /* We got a error in decoding message */
        res_buff->used = (uint16)dsatsmsi_cms_error(res_buff, (int)DSAT_CMS_UNKNOWN_ERR);
        /* send the response to the user */
        dsatcmdp_handle_async_cmd_rsp(res_buff, DSAT_COMPLETE_RSP);

        result = DSAT_CMD_ERR_RSP;
        /* We are done with the commands, clean up */
        dsatsmsi_pres_cmd = SMS_NONE;
        memset ( &dsatsmsi_cmd_sm, 0, sizeof(sms_cmd_sm_type) );
      }
    }
  } /* end if (dsatsmsi_pres_cmd == SMS_CMGS) */
    
  return result;
}/* is707_send_msg_event_handler */


/*===========================================================================

FUNCTION  IS707_SUBMIT_MSG_REPORT_EVENT_HANDLER

DESCRIPTION
  Handler function for submit message report event from WMS

DEPENDENCIES
  None

RETURN VALUE
  none

SIDE EFFECTS
  None

===========================================================================*/
/* ARGSUSED */
LOCAL dsat_result_enum_type is707_submit_msg_report_event_handler
(
 dsat_mode_enum_type     mode,          /* AT command mode    */
 sms_event_info_s_type * event_info     /* Event info ptr    */
)
{
  dsm_item_type * res_buff;
  byte * data_ptr;

  dsat_result_enum_type result = DSAT_ASYNC_EVENT;

/*-------------------------------------------------------------------------*/

  MSG_HIGH("is707_submit_msg_report_event_handler, pres_cmd: %d",
                dsatsmsi_pres_cmd,0,0);

  if( event_info == NULL )
  {
    ERR_FATAL("Null Event_info",0,0,0);
  }
  
  if ( (dsatsmsi_pres_cmd == SMS_CMGS ) ||
       (dsatsmsi_pres_cmd == SMS_CMSS )  )
  {
    if( (res_buff = dsm_new_buffer( DSM_DS_LARGE_ITEM_POOL )) == NULL)
    {
      ERR_FATAL( "Could not get a new DSM item", 0, 0, 0);
    }
    else
    {
      data_ptr =  res_buff->data_ptr;
      if ( event_info->u.submit_report.report_status == WMS_RPT_OK )
      {
        /* We have successfully sent the message */
        /* Just return the message reference number to the user */

        if( dsatsmsi_pres_cmd == SMS_CMGS )
        {
          (void) std_strlcpy((char*) data_ptr, cmgs_hdr , res_buff->size);
          data_ptr += strlen(cmgs_hdr);
        }
        else if (dsatsmsi_pres_cmd == SMS_CMSS )
        {
          (void) std_strlcpy((char*) data_ptr, cmss_hdr , res_buff->size);
          data_ptr += strlen(cmss_hdr);
        }
        data_ptr = dsatsmsi_itoa (is707_send_mr, data_ptr, 10);

        res_buff->used =  (byte)(data_ptr - res_buff->data_ptr);
        result = DSAT_OK;
        dsatsmsi_pres_cmd = SMS_NONE;
        memset ( &dsatsmsi_cmd_sm, 0, sizeof(sms_cmd_sm_type) );
        is707_send_mr = 0;
      }/* if(success) */
      else
      {
        /* We got a error */
        int error_cause;
        if ( (event_info->u.submit_report.report_status 
              == WMS_RPT_RP_ERROR) ||
             (event_info->u.submit_report.report_status 
              == WMS_RPT_CP_ERROR)
             )
        {
          error_cause = 
            (int)event_info->u.submit_report.tl_status;
        }
        else
        {
          /* Internal error                */
          /* Tell TE that it is an unknown */
          error_cause = (int)DSAT_CMS_UNKNOWN_ERR;
        }

        res_buff->used += 
          (byte)dsatsmsi_cms_error(res_buff, error_cause);
        result = DSAT_CMD_ERR_RSP;
        dsatsmsi_pres_cmd = SMS_NONE;
        memset ( &dsatsmsi_cmd_sm, 0, sizeof(sms_cmd_sm_type) );
        is707_send_mr = 0;
      }
    }
  
    /* send the response to the user */
    dsatcmdp_handle_async_cmd_rsp(res_buff, DSAT_COMPLETE_RSP);
  }
  
  return result;
} /* is707_submit_msg_report_event_handler */


/*===========================================================================

FUNCTION  IS707_RECEIVED_MSG_EVENT_HANDLER

DESCRIPTION
  Handler function for received message (incoming) event from WMS

DEPENDENCIES
  None

RETURN VALUE
  none

SIDE EFFECTS
  None

===========================================================================*/
/* ARGSUSED */
LOCAL dsat_result_enum_type is707_received_msg_event_handler
(
 dsat_mode_enum_type     mode,          /* AT command mode    */
 sms_event_info_s_type * event_info     /* Event info ptr    */
)
{
  wms_message_index_type  msg_index;
  wms_memory_store_e_type mem_store;

/*-------------------------------------------------------------------------*/

  MSG_HIGH("is707_received_msg_event_handler, pres_cmd",
                dsatsmsi_pres_cmd,0,0);
  
  if( event_info == NULL )
  {
    ERR_FATAL("Null Event_info",0,0,0);
  }
  
  mem_store = event_info->u.mt_msg.msg_hdr.mem_store;
  msg_index = event_info->u.mt_msg.msg_hdr.index;

  
  if ( ( !dsatcmdp_block_indications() ) && ( dsatcmdp_at_state == DSAT_CMD ) )
  {
    /* ATCoP is not Busy */
    if( ( dsat707sms_qcnmi_val[0] == 0 ) && ( dsat707sms_qcpms_val[1] != 2 ) )
    {
      /* <mode> equals 0 Hence we need to buffer */
      queue_mt_msg( MT_DELIVER, msg_index, mem_store, 0 );
      return DSAT_ASYNC_EVENT;
    }
    else if( dsat707sms_qcnmi_val[1]== 0 )
    {
      /* User selected the option not to route */
      return DSAT_ASYNC_EVENT;
    }
    else
    {
      /* Check to see if we have any outstanding notifications */
      /* If yes, send them to TE so that they are in order     */
      dsat707sms_flush_sms_indications( );
      
      if( dsat707sms_qcnmi_val[1] == 2 )
      {
        dsat707smsi_send_unsol_mt_msg(event_info);
      }
      else
      {
        /* Send the present indications directly to TE */
        dsatsmsi_send_async_indication_to_te ( MT_DELIVER, msg_index, mem_store );
      }
      return DSAT_ASYNC_EVENT;
    }
  }
  else
  {
    /* ATCoP is Busy */
    if ( dsat707sms_qcnmi_val[0] == 1 )
    {
      /* User selected the option to ignore notifications */ 
      return DSAT_ASYNC_EVENT;
    }
    else
    {
      if( dsat707sms_qcpms_val[1] == 2 )
      {
        MSG_HIGH("Not Buffering as RAM in CDMA is not supported",0,0,0);
        return DSAT_ASYNC_EVENT;
      }
      /* buffer the indication based on user preference */
      /* it will be flushed to TE at a later stage      */
      queue_mt_msg ( MT_DELIVER, msg_index, mem_store, 0 );
  
      /* Tell ATCOP not to send any result code */
      return DSAT_ASYNC_EVENT;
    }
  }
} /* is707_received_msg_event_handler */


/*===========================================================================

FUNCTION  IS707_READ_MSG_EVENT_HANDLER

DESCRIPTION
  Handler function for read message (incoming) event from WMS

DEPENDENCIES
  None

RETURN VALUE
  none

SIDE EFFECTS
  None

===========================================================================*/
/* ARGSUSED */
LOCAL dsat_result_enum_type is707_read_msg_event_handler
(
 dsat_mode_enum_type mode, /* AT command mode    */
 sms_event_info_s_type * event_info     /* Event info ptr    */
)
{
  dsm_item_type                * di_ptr;
  dsat_result_enum_type          result = DSAT_ASYNC_EVENT;
  byte                         * data_ptr = NULL;
  unsigned int                   used_val = 0;
  dsat_rsp_enum_type             resp_state;
  word                           dsat_sms_len = 0;
  uint8 addr_len=0;
/*-------------------------------------------------------------------------*/

  MSG_HIGH("is707_read_msg_event_handler, pres_cmd: %d",
                dsatsmsi_pres_cmd,0,0);

  if( event_info == NULL )
  {
    ERR_FATAL("Null Event_info",0,0,0);
  }

  if(( dsatsmsi_pres_cmd == SMS_CMGR )||
     ( dsatsmsi_pres_cmd == SMS_CMGL) )
  {
    if(dsat707sms_qcmgf_val == 0)
    {
      MSG_HIGH("PDU Mode Not supported in CDMA ",0,0,0);

      result = DSAT_CMD_ERR_RSP;
      dsatsmsi_pres_cmd = SMS_NONE;
      memset ( &dsatsmsi_cmd_sm, 0, sizeof(sms_cmd_sm_type) );
      return result;
    }
    if( (di_ptr = dsm_new_buffer( DSM_DS_LARGE_ITEM_POOL ))
        == NULL)
    {
      ERR_FATAL( "Could not get a new DSM item", 0, 0, 0);
    }
    data_ptr = di_ptr->data_ptr;

     /*
        In 4 Digit Mode addressing as follows:
        Number   |    4 BIT DTMF       |    ASCII conversion  
                 |  conversion by WMS  |
        -------- |-----------------    |------------------- 
         *1204#  |   11 01 02 10 04 12 |   66 49 50 65 52 35
     */
    addr_len = 
      MIN( event_info->u.msg_status.u.cdma_message.address.number_of_digits, 
           WMS_ADDRESS_MAX );

    if(event_info->u.msg_status.u.cdma_message.address.digit_mode 
                                                     == WMS_DIGIT_MODE_4_BIT)
    {
      addr_len = wms_ts_dtmf2ascii(
           addr_len,
           (const uint8 *) &event_info->u.msg_status.u.cdma_message.address.digits[0],
           (uint8 *) &event_info->u.msg_status.u.cdma_message.address.digits[0]);
    }
    if( addr_len > WMS_ADDRESS_MAX - 1 )
    {
      MSG_HIGH ("ERROR in converting dtmf to ascii as addr_len is %d", addr_len, 0, 0);
      result = DSAT_CMD_ERR_RSP;
      dsatsmsi_pres_cmd = SMS_NONE;
      memset ( &dsatsmsi_cmd_sm, 0, sizeof(sms_cmd_sm_type) );
      return result;
    }
    event_info->u.msg_status.u.cdma_message.address.digits[addr_len] = '\0';
    event_info->u.msg_status.u.cdma_message.address.number_of_digits = addr_len;
    
    memset( &cl_bd_data, 0, sizeof(wms_client_bd_s_type) );
    memset( &cl_ts_data, 0, sizeof(wms_client_ts_data_s_type) );

    event_info->u.msg_status.u.cdma_message.raw_ts.format = WMS_FORMAT_CDMA;
    
    /* write sms message body to the response buffer */
    /* Convert the Raw ts data to Client ts data */
    if (wms_ts_decode(&event_info->u.msg_status.u.cdma_message.raw_ts, &cl_ts_data)
        != WMS_OK_S)
    {
      /* We got an error while reading the message */
      /* Tell TE about it                          */
      used_val = dsatsmsi_cms_error(di_ptr, (int)DSAT_CMS_UNKNOWN_ERR);
      di_ptr->used = (byte)used_val;
      result = DSAT_CMD_ERR_RSP;
      /* Send the response to the user */
      if ( dsatsmsi_cmd_sm.list_begin )
      {
        resp_state = DSAT_COMPLETE_RSP;
        dsatsmsi_cmd_sm.list_begin = FALSE;
      }
      else
      {
        resp_state = DSAT_END_OF_RSP;
      }
      dsatcmdp_handle_async_cmd_rsp(di_ptr, resp_state);
      (void) dsm_free_buffer(di_ptr);

      /* We are done with the commands, clean up */
      dsatsmsi_pres_cmd = SMS_NONE;
      memset ( &dsatsmsi_cmd_sm, 0, sizeof(sms_cmd_sm_type) );
      return result;
    }
    
    /* Unpack the User Payload from Raw Data */
    if((cl_ts_data.u.cdma.user_data.encoding == WMS_ENCODING_ASCII) ||
             (cl_ts_data.u.cdma.user_data.encoding == WMS_ENCODING_IA5))
    {
      cl_bd_data.user_data.data_len =
         wms_ts_unpack_ascii(
                     (wms_cdma_user_data_s_type*)&cl_ts_data.u.cdma.user_data,
                                cl_ts_data.u.cdma.user_data.number_of_digits+1,
                                cl_bd_data.user_data.data);
    }
    MSG_HIGH(" Encoding Scheme found %d",
                   cl_ts_data.u.cdma.user_data.encoding,0,0);
    
    di_ptr->used = (uint16)(data_ptr - di_ptr->data_ptr);

    if((!dsatsmsi_cmd_sm.list_begin) && ( dsatsmsi_pres_cmd == SMS_CMGL ))
    {
      /* This is not the first message             */
      /* Seperate the next message with a <CR><LF> */
      *data_ptr++ = '\n';
    }
    
    if (dsatsmsi_pres_cmd == SMS_CMGR)
    {
      data_ptr += (word)std_strlprintf((char*)data_ptr, di_ptr->size,
                                "$QCMGR: ");
    }
    else if (dsatsmsi_pres_cmd == SMS_CMGL)
    {
      data_ptr += (word)std_strlprintf((char*)data_ptr, di_ptr->size,
                                "$QCMGL: ");
      /* write <index> to the result buffer */
      data_ptr = dsatutil_itoa( 
                    dsatsmsi_cmd_sm.indices[dsatsmsi_cmd_sm.present_index - 1],
                    data_ptr,10);
      *data_ptr++ = ',';
    }
    /* Update the Result with <stat> */
    data_ptr = dsatsmsi_tag_lookup ( 
                       event_info->u.msg_status.msg_hdr.tag, 0, data_ptr );
    *data_ptr++ = ','; 
    /* Update the Result with <oa> */
    data_ptr += (word)std_strlprintf((char*)data_ptr, di_ptr->size,
                                "\"%s\"",
                                event_info->u.msg_status.u.cdma_message.address.digits);
                                
   /* data_ptr = dsat707smsi_addr_to_str( 
                       event_info->u.msg_status.u.cdma_message.address,
                       &type_of_addr,
                       data_ptr );*/

    *data_ptr++ = ','; 
    /* Update the Result with <scts> */
    data_ptr = dsatsmsi_timestamp( cl_ts_data.u.cdma.mc_time ,data_ptr );
    *data_ptr++ = ',';

    if( event_info->u.msg_status.u.cdma_message.address.number_type 
                                          == WMS_NUMBER_INTERNATIONAL)
    {
      data_ptr += (word)std_strlprintf((char*)data_ptr, di_ptr->size,
                                ",145,");
    }
    else
    {
      data_ptr += (word)std_strlprintf((char*)data_ptr, di_ptr->size,
                                ",129,");
    }

    /* Update the Result with <length> */
    data_ptr = dsatutil_itoa((uint32) cl_bd_data.user_data.data_len, data_ptr, 10);
    *data_ptr++ = '\n';

    di_ptr->used += (uint16) ( data_ptr - di_ptr->data_ptr );
    /* Update the Result with <data> */
    if(cl_ts_data.u.cdma.user_data.encoding == WMS_ENCODING_OCTET ||
       cl_ts_data.u.cdma.user_data.encoding == WMS_ENCODING_ASCII ||
       cl_ts_data.u.cdma.user_data.encoding == WMS_ENCODING_GSM_7_BIT_DEFAULT)
    {
       dsat_sms_len = dsm_pushdown_tail( &di_ptr, (void*)cl_bd_data.user_data.data,
                                         cl_bd_data.user_data.data_len,
                                          DSM_ITEM_POOL(di_ptr) );
      MSG_MED("Received SMS length is %d",dsat_sms_len,0,0);
    }
    /* CMGL specific Processing */
    if ( dsatsmsi_pres_cmd == SMS_CMGL )
    {
      /* check to see if we keep going or dump the result */
      if(dsatsmsi_cmd_sm.present_index >= dsatsmsi_cmd_sm.num_of_indices)
      {
        if (dsatsmsi_cmd_sm.num_of_stat <= dsatsmsi_cmd_sm.present_stat)
        {
          /* We are done with the commands, clean up */
          if ( dsatsmsi_cmd_sm.list_begin )
          {
            resp_state = DSAT_COMPLETE_RSP;
            dsatsmsi_cmd_sm.list_begin = FALSE;
          }
          else
          {
            resp_state = DSAT_END_OF_RSP;
          }
          dsatsmsi_pres_cmd = SMS_NONE;
          memset ( &dsatsmsi_cmd_sm, 0, sizeof(sms_cmd_sm_type) );
          result = DSAT_OK;
        }
        else
        {
          /* We have more stat to read */
          (void)dsatsmsi_get_msg_list();
          result = DSAT_ASYNC_CMD;
          if ( dsatsmsi_cmd_sm.list_begin )
          {
            resp_state = DSAT_START_OF_RSP;
            dsatsmsi_cmd_sm.list_begin = FALSE;
          }
          else
          {
            resp_state = DSAT_INTERMEDIATE_RSP;
          }
        }
      }
      else
      {
        /* We are not done with reading the list */
        (void)dsatsmsi_read_msg_list();
        /* Let ATCOP know that we are not done yet */
        if ( dsatsmsi_cmd_sm.list_begin )
        {
          resp_state = DSAT_START_OF_RSP;
          dsatsmsi_cmd_sm.list_begin = FALSE;
        }
        else
        {
          resp_state = DSAT_INTERMEDIATE_RSP;
        }
        result = DSAT_ASYNC_CMD;
      }
      dsatcmdp_handle_async_cmd_rsp(di_ptr, resp_state);
      return result;
    }

    dsatcmdp_handle_async_cmd_rsp(di_ptr, DSAT_COMPLETE_RSP);

    /* Check to see if we need to modify the tag of the msg */
    if(event_info->u.msg_status.msg_hdr.tag == WMS_TAG_MT_NOT_READ)
    {
      /* We got a REC UNREAD tag */
      /* Modify it to REC READ   */
      (void) wms_msg_modify_tag(
                                dsatsmsi_client_id,
                                dsatsmsi_cmd_cb_func,
                                (void*)&dsatsmsi_pres_cmd,
                                event_info->u.msg_status.msg_hdr.mem_store,
                                event_info->u.msg_status.msg_hdr.index,
                                WMS_TAG_MT_READ
                                );
    }

    /* We are done with the commands, clean up */
    memset ( &dsatsmsi_cmd_sm, 0, sizeof(sms_cmd_sm_type) );
    dsatsmsi_pres_cmd = SMS_NONE;
    result = DSAT_OK;

  }
  else if ( dsatsmsi_pres_cmd == SMS_CMSS )
  {

    wms_status_e_type msg_status;
    wms_send_mode_e_type   send_mode;
    
    /* We are reading a message in order to replace the */
    /* existing <da> with a new one that user gave      */
    memset ( &replace707_da_msg, 0, sizeof(wms_client_message_s_type));

    replace707_da_msg.u.cdma_message.raw_ts = 
                         event_info->u.msg_status.u.cdma_message.raw_ts;
    
    send_mode = WMS_SEND_MODE_CLIENT_MESSAGE;
  
    replace707_da_msg.msg_hdr = event_info->u.msg_status.msg_hdr;
    replace707_da_msg.msg_hdr.message_mode = WMS_MESSAGE_MODE_CDMA;

    if( dsat707sms_qcsmp_val[0].num_item < 8 )
    {
      replace707_da_msg.u.cdma_message.teleservice = 
            dsat707smsi_teleservice_num_to_enum(
               (char *) qcsmp_tid_valstr[dsat707sms_qcsmp_val[0].num_item]);
    }
    else
    {
      result = DSAT_ERROR;
      return result;
    }
    replace707_da_msg.u.cdma_message.subaddress.number_of_digits = 0;
    replace707_da_msg.u.cdma_message.is_tl_ack_requested = FALSE;
    replace707_da_msg.u.cdma_message.is_service_present = FALSE;
    replace707_da_msg.u.cdma_message.address = send_addr;
  
    msg_status = wms_msg_send( dsatsmsi_client_id,
                               dsatsmsi_cmd_cb_func,
                               (void*)&dsatsmsi_pres_cmd,
                               send_mode,
                               &replace707_da_msg );
  
    if( msg_status != WMS_OK_S )
    {
      result = DSAT_ERROR;
    }
    else
    {
      result = DSAT_ASYNC_CMD;
    }
  
    return result;
    
  }

  return result;

} /* is707_read_msg_event_handler */

/*===========================================================================

FUNCTION  IS707_DELETE_MSG_EVENT_HANDLER

DESCRIPTION
  Handler function for delete message event from WMS

DEPENDENCIES
  None

RETURN VALUE
  none

SIDE EFFECTS
  None

===========================================================================*/
/* ARGSUSED */
LOCAL dsat_result_enum_type is707_delete_msg_event_handler
(
 dsat_mode_enum_type     mode,          /* AT command mode    */
 sms_event_info_s_type * event_info     /* Event info ptr    */
)
{
  wms_memory_store_e_type mem_store;
  wms_message_tag_e_type tag;
  wms_status_e_type msg_status;
  dsat_result_enum_type result = DSAT_ASYNC_CMD;
/*-------------------------------------------------------------------------*/

  MSG_HIGH("is707_delete_msg_event_handler, pres_cmd: %d",
                dsatsmsi_pres_cmd,0,0);

  if( event_info == NULL )
  {
    ERR_FATAL("Null Event_info",0,0,0);
  }

  if ( dsatsmsi_pres_cmd == SMS_NONE)
  {
    /* unwanted event */
    return DSAT_ASYNC_EVENT;
  }
  MSG_HIGH("ATCOP SMS: Delete Message Event Handler", 0, 0, 0);

  /* Check to see if we are deleting just one index */
  if(dsatsmsi_pres_cmd == SMS_CMGD && (dsatsmsi_cmd_sm.one_index == 1))
  {
    /* We got a CMGD with one index */
    /* We are done.                 */
    /* Tell ATCOP we are OK         */
    dsatsmsi_pres_cmd = SMS_NONE;
    memset ( &dsatsmsi_cmd_sm, 0, sizeof(sms_cmd_sm_type) );
    result = DSAT_OK;
  }
  /* We are deleting with a wild card */
  else 
  {
    /* Are we done with the present stat */
    if ( dsatsmsi_cmd_sm.num_of_indices != 0 )
    {
      mem_store = dsatsmsi_memory_lookup((int)dsat707sms_qcpms_val[0], CPMS_MEM1);
      tag = dsatsmsi_cmd_sm.stat_list[dsatsmsi_cmd_sm.present_stat-1];
      msg_status = wms_cfg_get_message_list(
                                            dsatsmsi_client_id,
                                            dsatsmsi_cmd_cb_func,
                                            (void *)&dsatsmsi_pres_cmd,
                                            mem_store,
                                            tag
                                           );
      /* Always read from the tip */
      dsatsmsi_cmd_sm.present_index = 0;
      if (msg_status == WMS_OK_S)
      {
        /* still more to come */
        /* tell ATCOP about that */
        result = DSAT_ASYNC_CMD;
      }
    }
    else if ( dsatsmsi_cmd_sm.num_of_stat > 1 ) /* Any more stats to delete */
    {
      if(dsatsmsi_cmd_sm.present_stat < dsatsmsi_cmd_sm.num_of_stat)
      {
        mem_store = dsatsmsi_memory_lookup((int)dsat707sms_qcpms_val[0], 
                                              CPMS_MEM1);
        tag = dsatsmsi_cmd_sm.stat_list[dsatsmsi_cmd_sm.present_stat];
        dsatsmsi_cmd_sm.present_stat++;
        
        msg_status = wms_cfg_get_message_list(
                                                dsatsmsi_client_id,
                                                dsatsmsi_cmd_cb_func,
                                                (void *)&dsatsmsi_pres_cmd,
                                                mem_store,
                                                tag
                                               );
        /* Tell ATCOP that there is mode to come */
        result = DSAT_ASYNC_CMD;
      }
      else
      {
        /* We are finally done */
        /* tell ATCOP we are done */
        result =  DSAT_OK;
      }
    }
    else
    {
      /* this is a case of just single index delete or */
      /* we are done deleting messages                 */
      result = DSAT_OK;
    }
  }
  return result;
}/* is707_delete_msg_event_handler */

/*===========================================================================

FUNCTION  is707_WRITE_MSG_EVENT_HANDLER

DESCRIPTION
  Handler function for write message event from WMS

DEPENDENCIES
  None

RETURN VALUE
  returns an enum that describes the result of the command execution.
  Possible values are:
    DSAT_OK : if the command has been successfully executed
    DSAT_ASYNC_EVENT : If no result code is needed (e.g. unwanted event)
    DSAT_CMD_ERR_RSP : if +CMS ERROR is being generated

SIDE EFFECTS
  None
===========================================================================*/
/* ARGSUSED */
LOCAL dsat_result_enum_type is707_write_msg_event_handler 
(
 dsat_mode_enum_type      mode,        /* AT command mode    */
  sms_event_info_s_type  *event_info   /* Event info ptr    */
)
{
  uint32 return_val;
  dsm_item_type * res_buff;
  byte * data_ptr;
  byte cmgw_name_ptr[] = "$QCMGW: ";
  int i=0;
  byte c;
  dsat_result_enum_type result = DSAT_ASYNC_EVENT;
  
  if( event_info == NULL )
  {
    ERR_FATAL("Null Event_info",0,0,0);
  }

  MSG_HIGH("is707_write_msg_event_handler, pres_cmd: %d",
                dsatsmsi_pres_cmd,0,0);
  if ( dsatsmsi_pres_cmd != SMS_CMGW )
  {
    /* unwanted event */
    return DSAT_ASYNC_EVENT;
  }

  /* We need to send index at which the message was stored */
  return_val = event_info->u.msg_status.msg_hdr.index;
  
  if( (res_buff = dsm_new_buffer( DSM_DS_LARGE_ITEM_POOL )) == NULL)
  {
    ERR_FATAL( "Could not get a new DSM item", 0, 0, 0);
  }
  else
  {
    data_ptr = res_buff->data_ptr;

    while((c=cmgw_name_ptr[i++]) != '\0')
    {
        *data_ptr++ = c;
    }

    data_ptr = dsatsmsi_itoa ((word)return_val, data_ptr, 10);
    /* update the used val */
    res_buff->used = (byte)(data_ptr - res_buff->data_ptr);

    /* send the response to the user */
    dsatcmdp_handle_async_cmd_rsp(res_buff, DSAT_COMPLETE_RSP);
    /* We are done with the commands, clean up */
    dsatsmsi_pres_cmd = SMS_NONE;
    memset ( &dsatsmsi_cmd_sm, 0, sizeof(sms_cmd_sm_type) );
    result = DSAT_OK;
  }
  return result;
} /* is707_write_msg_event_handler */


#endif /* FEATURE_CDMA_SMS */

