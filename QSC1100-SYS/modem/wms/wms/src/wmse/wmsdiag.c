/*====*====*====*====*====*====*====*====*====*====*====*====*====*====*====*

           W i r e l e s s     M e s s a g i n g  S e r v i c e s
            - Diagnostic Services

DESCRIPTION
  This file defines the packet handlers for the WMSDIAG interface.  It also
  contains the WMSDIAG logging function definition.

Copyright (c) 1998, 1999, 2000, 2001, 2002, 2003, 2004, 2005, 2006, 2007, 
              2008 by QUALCOMM, Incorporated. All Rights Reserved.

Export of this technology or software is regulated by the U.S. Government.
Diversion contrary to U.S. law prohibited.
*====*====*====*====*====*====*====*====*====*====*====*====*====*====*====*/
/*===========================================================================

                             Edit History


when       who     what, where, why
--------   ---     ----------------------------------------------------------
06/11/08   PMD     Removed wms_bc_*** APIs for CDMA Broadcast
06/04/08   PMD     Removed support for Auto / Transparent SMS 
01/31/08   PMD     Updated Copyright Information to include 2008
01/17/07   HQ      Updated copyright info to include 2007.
10/17/06   PMD     Fixed Lint Warning by removing check
08/09/06   PMD     Deleted Global definition of delay_rsp_id
05/19/06   PMD     Added FEATURE_IWMSDIAG to fix compile warning
05/11/06   PMD     Added Support for New Diag Commands and modified 
                   existing Diag Commands for Delayed Response
05/06/06   HQ      Fixed lint warnings by changing data size and
                   adding data size checking.
04/24/06   HQ      Fixed lint errors.
02/06/06   PMD     Fixed Compile Warnings
01/25/06   VK      Updated Copyright Info
10/20/05   VK      Changed byte = uint8, word = uint16, dword = uint32
10/19/05   VK      Fixed featurization issues
10/09/05   VK      Updated Copyright info
08/24/05   PMD     Added req_pending State Machine for proper Handling.
06/13/05   Rex     Added new wmsappdiag command:
                   wmsappdiag_cb_delete_all
04/19/05   Rex     Added support for deleting all broadcast service id 
10/05/04   Rex     Featurize IWMSDIAG appropriately
09/22/04   Rex     Set req_pending to false when diagpkt_async_release() is
                   called for wmsdiag_get_message_list
06/22/04   Rex     Added new variable delay_rsp_id for delayed response packets
06/02/04   HQ      Changed class to msg_class to fix C++ compilation issue.
05/27/04   Rex     Added new wms diag commands:
                   wmsappdiag_cb_save,
                   wmsappdiag_cb_read,
                   wmsdiag_broadcast_add_delete_serviceid
03/24/04   PMD     Fixed Compile Warnings
03/10/04   Rex     Send response packet even though bearer data failed for
           wmsdiag_set_default_auto_cdma_params_rsp_type
03/03/04   Rex     Resolved wms_get_message_list response packet with incorrect
                   storage_type
02/17/04   Rex     Added checking for max length values
02/04/04   PMD     Cleaned up Lint Warnings in code
12/11/03   Rex     Changed wmsdiag_set_default_auto_cdma_params by removing
                   dcs and relative validity structure fields
12/03/03   Rex     Resolved some lint errors
10/30/03   Rex     Added new wms diag commands:
                   wmsdiag_send_auto_message,
                   wmsdiag_set_default_auto_cdma_params,
                   wmsdiag_set_default_auto_gw_params
09/15/03   Rex     Added support for WMS_SET_ROUTES log packets.
                   Added field (status) to wms_read_cmd response and
                   fields (transaction_id and status) to log packet format.
                   Guard checking for log_rsp null pointer allocation.
                   Remove diagpkt_async_block() and diagpkt_async_release()
                   from wms_set_routes.
08/04/03   PMD     Changes to suppress / remove Lint Errors
08/01/03   SUN     For WMS_CFG_SET_ROUTES, diag should not be blocked,
                   just commit the response
07/29/03   AP      Added support for WMS_AUTO_DISCONNECT_CMD.
06/05/03   HQ      Added #include err.h.
06/04/03   HQ/AP   Fixed checking of service options for orig_end command.
06/03/03   AP      Use utility for mapping tags instead of a duplicate switch.
06/03/03   AP      Added utility to map from a WMS tag to a WMSDIAG tag.
05/28/03   AP      Added command to originate and end a CDMA SMS call.
04/26/03   HQ      Use WMS_CFG_EVENT_ROUTES instead of WMS_CMD_CFG_SET_ROUTES.
03/12/03   AP      Fixed comparison of request_ptr write mode with
                   WMSDIAG_WRITE_MODE_MAX.
01/24/03   AP      Only generate a read log if wmsdiag initates WMS to read a
                   message.
01/20/03   AP      Fixed useage of diagpkt_subsys_alloc to not make this call
                   while processing a registered WMS callback.
01/16/03   HQ      Fixed wrong name for transfer_status_report.
12/10/02   sm      For read event call wms_diag_make_message_read_result_log to
                   generate a log packet.
12/03/02   HQ      Changed signature of diagpkt_async_block();
                   Added log code to log_alloc() call.
11/21/02   AP      Moved diag includes to wmsdiag.h
11/14/02   sm      Created.
===========================================================================*/


#include "comdef.h"
#include "customer.h"

#ifdef FEATURE_WMS_DIAG
#if defined(FEATURE_CDSMS) || defined(FEATURE_GWSMS)
#include "diagcmd.h"
#include "wmsdiag.h"
#include "wms.h"
#include "err.h"

#include "tmc.h"
#include "string.h"

#ifdef FEATURE_IWMSDIAG
#include "OEMWMSDIAG.h"
#endif /* FEATURE_IWMSDIAG */

/*============================================================================
   WMS Internal Useage Defines
============================================================================*/
#define WMSDIAG_REQ_STATE_IDLE       0
#define WMSDIAG_REQ_STATE_COMMAND_CB 1
#define WMSDIAG_REQ_STATE_MESSAGE_CB 2

#define WMSDIAG_INVALID_CLIENT_ID  0xFF

/*===========================================================================
   Static variables used
===========================================================================*/
/*  Initialize our client ID to some bogus value */
static wms_client_id_type wmsdiag_client_id = WMSDIAG_INVALID_CLIENT_ID;

/*  A global pointer used to update the message list */
static  wmsdiag_get_message_list_rsp_type wmsdiag_list;

/*  Initialize a transaction ID for diag log packets */
static wms_transaction_id_type wmsdiag_transaction_ID;

/*  A flag that tells us if the get message list command failed or not */
static boolean wmsdiag_get_message_list_failed = FALSE;

/*  A flag that tells us if the set routes command failed or not */
static boolean wmsdiag_set_routes_failed = FALSE;

/*  A flag that tells us if the read command failed or not */
static boolean wmsdiag_read_failed = FALSE;

/*  A flag that tells us if the read command was initiatiated */
static boolean wmsdiag_initiated_read = FALSE;

/*  The client message structure to be passed to WMS */
static wms_client_message_s_type     client_message;

/*  The client message header to be passed to WMS */
static wms_client_msg_hdr_s_type     msg_hdr;

/*  A flag for keeping track of the memory store requested */
static wms_memory_store_e_type wmsdiag_req_storage;

/*  A state that tells us if request is still pending or not */
static uint8 req_pending = WMSDIAG_REQ_STATE_IDLE;

/*  A state that tells us if Get Bearer Request is still pending or not */
static uint8 get_bearer_req_state = WMSDIAG_REQ_STATE_IDLE;

/* Transacation ID for Get Bearer Pref Command */
static uint32 get_bearer_tid;

/*=============================================================================
    Arrays used internally by the WMS interface to map enumerated types in the
    WMS interface to our interface.  This was done to keep changes to those
    enumerated types transparent to our users.
=============================================================================*/

static wms_memory_store_e_type WMSDIAG_MEM_STORE_VALS [WMSDIAG_MEM_STORE_MAX] =
{
  WMS_MEMORY_STORE_NONE,
  WMS_MEMORY_STORE_RAM_GW,
  WMS_MEMORY_STORE_SIM,
  WMS_MEMORY_STORE_NV_GW,
  WMS_MEMORY_STORE_RAM_CDMA,
  WMS_MEMORY_STORE_RUIM,
  WMS_MEMORY_STORE_NV_CDMA
};

static wms_message_tag_e_type WMSDIAG_MSG_TAG_VALS [WMSDIAG_TAG_MAX] =
{
  WMS_TAG_NONE,
  WMS_TAG_MT_READ,
  WMS_TAG_MT_NOT_READ,
  WMS_TAG_MO_SENT,
  WMS_TAG_MO_NOT_SENT,
  WMS_TAG_MO_SENT_ST_NOT_RECEIVED,
  WMS_TAG_MO_SENT_ST_NOT_STORED,
  WMS_TAG_MO_SENT_ST_STORED,
  WMS_TAG_MO_TEMPLATE,
  WMS_TAG_STATUS_RPT
};

static wms_send_mode_e_type WMSDIAG_SEND_MODE_VALS [WMSDIAG_SEND_MODE_MAX] =
{
  WMS_SEND_MODE_MEMORY_STORE,
  WMS_SEND_MODE_CLIENT_MESSAGE
};

static wms_write_mode_e_type WMSDIAG_WRITE_MODE_VALS [WMSDIAG_WRITE_MODE_MAX] =
{
  WMS_WRITE_MODE_INSERT,
  WMS_WRITE_MODE_REPLACE
};


static wms_message_mode_e_type WMSDIAG_MSG_MODE_VALS [WMSDIAG_MSG_MODE_MAX] =
{
  WMS_MESSAGE_MODE_CDMA,
  WMS_MESSAGE_MODE_GW
};

static wms_digit_mode_e_type WMSDIAG_ADDR_DIGIT_MODE_VALS [WMSDIAG_ADDR_DIGIT_MODE_MAX] =
{
  WMS_DIGIT_MODE_4_BIT,  /* DTMF digits */
  WMS_DIGIT_MODE_8_BIT
};

static wms_number_mode_e_type WMSDIAG_ADDR_NUM_MODE_VALS [WMSDIAG_ADDR_NUM_MODE_MAX] =
{
  WMS_NUMBER_MODE_NONE_DATA_NETWORK,
  WMS_NUMBER_MODE_DATA_NETWORK
};

static wms_format_e_type WMSDIAG_DATA_FORMAT_VALS [WMSDIAG_DATA_FORMAT_MAX] =
{
  WMS_FORMAT_CDMA,              /* IS-95 */
  WMS_FORMAT_ANALOG_CLI,        /* IS-91 */
  WMS_FORMAT_ANALOG_VOICE_MAIL, /* IS-91 */
  WMS_FORMAT_ANALOG_SMS,        /* IS-91 */
  WMS_FORMAT_ANALOG_AWISMS,     /* IS-95 Alert With Information SMS */
  WMS_FORMAT_MWI,               /* Message Waiting Indication as voice mail */
  WMS_FORMAT_GW_PP,             /* GW Point-to-Point SMS */
  WMS_FORMAT_GW_CB              /* GW CB SMS */
};

static wms_route_e_type WMSDIAG_ROUTE_VALS [WMSDIAG_ROUTE_MAX] =
{
  WMS_ROUTE_DISCARD,          /* Discard message */
  WMS_ROUTE_STORE_AND_NOTIFY, /* Store message and notify clients */
  WMS_ROUTE_TRANSFER_ONLY,    /* Don't store msg but send it to clients */
  WMS_ROUTE_TRANSFER_AND_ACK, /* Send the message to clients and ack it */
  WMS_ROUTE_NO_CHANGE,        /* Don't change the current setting */
};
#ifdef FEATURE_GWSMS
#error code not present
#endif /*  FEATURE GWSMS */

/*===========================================================================
FUNCTION WMSDIAG_MAP_TAG_TYPE

DESCRIPTION
  Maps the tag given by WMS to the appropriate tag value of WMSDIAG.

DEPENDENCIES
  None.

RETURN VALUE
  WMSDIAG_MessageTagType return_tag

SIDE EFFECTS
  NONE.

===========================================================================*/
WMSDIAG_MessageTagType wmsdiag_map_tag_type(wms_message_tag_e_type tag)
{
  WMSDIAG_MessageTagType return_tag = WMSDIAG_TAG_INVALID;

  switch(tag)
  {
    case WMS_TAG_NONE:
      return_tag = WMSDIAG_TAG_NONE;
      break;

    case WMS_TAG_MT_READ:
      return_tag = WMSDIAG_TAG_MT_READ;
      break;

    case WMS_TAG_MT_NOT_READ:
      return_tag = WMSDIAG_TAG_MT_NOT_READ;
      break;

    case WMS_TAG_MO_SENT:
      return_tag = WMSDIAG_TAG_MO_SENT;
      break;

    case WMS_TAG_MO_NOT_SENT:
      return_tag = WMSDIAG_TAG_MO_NOT_SENT;
      break;

    case WMS_TAG_MO_SENT_ST_NOT_RECEIVED:
      return_tag = WMSDIAG_TAG_MO_SENT_ST_NOT_RECEIVED;
      break;

    case WMS_TAG_MO_SENT_ST_NOT_STORED:
      return_tag = WMSDIAG_TAG_MO_SENT_ST_NOT_STORED;
      break;

    case WMS_TAG_MO_SENT_ST_STORED:
      return_tag = WMSDIAG_TAG_MO_SENT_ST_STORED;
      break;

    case WMS_TAG_MO_TEMPLATE:
      return_tag = WMSDIAG_TAG_MO_TEMPLATE;
      break;

    case WMS_TAG_STATUS_RPT:
      return_tag = WMSDIAG_TAG_STATUS_RPT;
      break;

    default:
      MSG_ERROR("Invalid tag type = %d", tag,0,0);
      break;
  }

  return return_tag;
}

/*===========================================================================
FUNCTION WMSDIAG_LIST_UPDATE_CB

DESCRIPTION
  This function updates the message list requested and
  accepts a cfg_event_info_s_type structure and creates a
  wmsdiag_get_message_list_log_type log packet that contains all the log
  If this function is called, the list was updated correctly.

DEPENDENCIES
  None.

RETURN VALUE
  None.

SIDE EFFECTS
  Sets DIAG_ASYNC_BLOCK_SIG signal.

===========================================================================*/

void wmsdiag_list_update_cb (wms_cfg_event_e_type event,
                             wms_cfg_event_info_s_type *info_ptr)
{

  /*  Loop counter */
  int i;

  /*  If this is not an event pertaining to an updated of the message list, return */
  if (event != WMS_CFG_EVENT_MESSAGE_LIST)
  {
    return;
  }

  if (req_pending != WMSDIAG_REQ_STATE_MESSAGE_CB)
  {
    return;
  }

  MSG_HIGH("info_ptr->message_list.mem_store = %d",info_ptr->message_list.mem_store,0,0);
  MSG_HIGH("wmsdiag_req_storage = %d",wmsdiag_req_storage,0,0);

  /* If the memory store in the info_ptr do not match the request packet, return */
  if (info_ptr->message_list.mem_store != wmsdiag_req_storage)
  {
    return;
  }

  /*  Set up the parameters */
  wmsdiag_list.num_entries = info_ptr->message_list.len;
  wmsdiag_list.storage_type = info_ptr->message_list.mem_store;
  wmsdiag_list.voice_mail_index = info_ptr->message_list.voice_mail_index;
  wmsdiag_list.voice_mail_active = info_ptr->message_list.voice_mail_active;

  for (i = 0; i < (int)(wmsdiag_list.num_entries); i++)
  {
    wmsdiag_list.messages[i].tag_type =
      (uint32)wmsdiag_map_tag_type(info_ptr->message_list.tags[i]);
    wmsdiag_list.messages[i].index = info_ptr->message_list.indices[i];
  }

  req_pending = WMSDIAG_REQ_STATE_IDLE;

  /*  SET THE SIGNAL WHICH SHOULD UNBLOCK DIAG */
  diagpkt_async_release ();
}

/*===========================================================================

FUNCTION WMSDIAG_GET_MESSAGE_LIST_CMD_CB

DESCRIPTION
  Callback for the get_message_list command to ensure that it was sent ok.

DEPENDENCIES
  None.

RETURN VALUE
  None.

SIDE EFFECTS
  Sets DIAG_ASYNC_BLOCK_SIG signal if there was an error to unblock diag. Also sets
  wmsdiag_get_message_list_failed flag to indicate that there was an error,
  if applicable.

===========================================================================*/

void wmsdiag_get_message_list_cmd_cb (wms_cmd_id_e_type cmd,
                          void* user_data,
                          wms_cmd_err_e_type cmd_err)
{
  /*  If this is not pertaining to a message list being read, return */
  if (cmd != WMS_CMD_CFG_GET_MESSAGE_LIST)
  {
    return;
  }

  if (req_pending != WMSDIAG_REQ_STATE_COMMAND_CB)
  {
    return;
  }

  MSG_HIGH("cmd_err = %d",cmd_err,0,0);

  /*  If there was an error */
  if (cmd_err != WMS_CMD_ERR_NONE)
  {
    /*  Indicate that the command failed */
    wmsdiag_get_message_list_failed = TRUE;

    req_pending = WMSDIAG_REQ_STATE_IDLE;
    /*  SET THE SIGNAL WHICH SHOULD UNBLOCK DIAG */
    diagpkt_async_release ();
  }
  /*  If no errors */
  else
  {
    /*  Indicate that the read was ok */
    wmsdiag_get_message_list_failed = FALSE;

    /*  We don't unblock diag here because we should get a cfg event when
        our command actually completes, and this will unblock diag by
    calling wmsdiag_list_update_cb */

    req_pending = WMSDIAG_REQ_STATE_MESSAGE_CB;
  }
}

/*===========================================================================

FUNCTION WMSDIAG_GET_BEARER_PREF_CMD_CB

DESCRIPTION
  Command Callback for Get Domain Pref Command

DEPENDENCIES
  None.

RETURN VALUE
  None.

SIDE EFFECTS
  None.

===========================================================================*/
void wmsdiag_get_bearer_pref_cmd_cb ( wms_cmd_id_e_type cmd,
                                      void* user_data,
                                      wms_cmd_err_e_type cmd_err )
{
  /* Delayed Response Packet */
  wmsdiag_get_bearer_pref_rsp_type *delay_rsp_pkt = NULL;

  /* Always setting it to 1 */
  diagpkt_subsys_delayed_rsp_id_type delay_rsp_id = 1;

  if (cmd != WMS_CMD_CFG_GET_GW_DOMAIN_PREF) 
  {
    /* Ignore other Commands */
    return;
  }

  if (get_bearer_req_state != WMSDIAG_REQ_STATE_COMMAND_CB)
  {
    return;
  }

  /* On Success, Wait for CFG Callback for Delayed Response */
  if (cmd_err != WMS_CMD_ERR_NONE)
  {
    /*  Allocate our packet */
    delay_rsp_pkt = (wmsdiag_get_bearer_pref_rsp_type *) diagpkt_subsys_alloc_v2_delay(
                              (diagpkt_subsys_id_type)DIAG_SUBSYS_WMS, 
                              (diagpkt_subsys_cmd_code_type)WMSDIAG_GET_BEARER_PREF_CMD, 
                              delay_rsp_id, 
                              sizeof(wmsdiag_get_bearer_pref_rsp_type) );

    get_bearer_req_state = WMSDIAG_REQ_STATE_IDLE;

    if (delay_rsp_pkt == NULL)
    {
      MSG_ERROR("Response packet allocation Failed!",0,0,0);
      return;
    }
    else
    {
      /* Delayed Response */
      diagpkt_subsys_set_rsp_cnt(delay_rsp_pkt, 1);
      
      /* Set Error Status */
      diagpkt_subsys_set_status(delay_rsp_pkt, 1);
      
      /* Populate Fields */
      delay_rsp_pkt->transaction_ID  = (uint32)user_data;
      delay_rsp_pkt->sms_domain_pref = WMS_GW_DOMAIN_PREF_MAX32; /* Invalid */
      
      /* Send the Response */
      diagpkt_delay_commit(delay_rsp_pkt);
    }
  }
  else
  {
    /* Wait for Configuration Callback */
    get_bearer_req_state = WMSDIAG_REQ_STATE_MESSAGE_CB;
    get_bearer_tid       = (uint32)user_data;
  }
}

/*===========================================================================

FUNCTION WMSDIAG_GET_BEARER_PREF_CFG_CB

DESCRIPTION
  Configuration Callback for Get Domain Pref Command

DEPENDENCIES
  None.

RETURN VALUE
  None.

SIDE EFFECTS
  None.

===========================================================================*/
void wmsdiag_get_bearer_pref_cfg_cb ( wms_cfg_event_e_type event,
                                      wms_cfg_event_info_s_type *info_ptr)
{
  /* Delayed Response Packet */
  wmsdiag_get_bearer_pref_rsp_type *delay_rsp_pkt = NULL;

  /* Always setting it to 1 */
  diagpkt_subsys_delayed_rsp_id_type delay_rsp_id = 1;

  /*  If this is not an event pertaining to a GW Domain Pref return */
  if (event != WMS_CFG_EVENT_GW_DOMAIN_PREF)
  {
    return;
  }

  if (get_bearer_req_state != WMSDIAG_REQ_STATE_MESSAGE_CB)
  {
    return;
  }

  MSG_HIGH("GW Bearer Pref = %d",info_ptr->gw_domain_pref,0,0);
  
  get_bearer_req_state = WMSDIAG_REQ_STATE_IDLE;

  /*  Allocate our packet */
  delay_rsp_pkt = (wmsdiag_get_bearer_pref_rsp_type *) diagpkt_subsys_alloc_v2_delay(
                              (diagpkt_subsys_id_type)DIAG_SUBSYS_WMS, 
                              (diagpkt_subsys_cmd_code_type)WMSDIAG_GET_BEARER_PREF_CMD, 
                              delay_rsp_id, 
                              sizeof(wmsdiag_get_bearer_pref_rsp_type) );

  if (delay_rsp_pkt == NULL)
  {
    MSG_ERROR("Response packet allocation Failed!",0,0,0);
    return;
  }
  else
  {
    /* Delayed Response */
    diagpkt_subsys_set_rsp_cnt(delay_rsp_pkt, 1);
    
    /* Set Error Status */
    diagpkt_subsys_set_status(delay_rsp_pkt, 1);
    
    /* Populate Fields */
    delay_rsp_pkt->transaction_ID  = get_bearer_tid;
    delay_rsp_pkt->sms_domain_pref = info_ptr->gw_domain_pref; 
    
    /* Send the Response */
    diagpkt_delay_commit(delay_rsp_pkt);
  }
}

/*===========================================================================

FUNCTION WMSDIAG_SET_BEARER_PREF_CMD_CB

DESCRIPTION
  Command Callback for Set Domain Pref Command

DEPENDENCIES
  None.

RETURN VALUE
  None.

SIDE EFFECTS
  None.

===========================================================================*/
void wmsdiag_set_bearer_pref_cmd_cb ( wms_cmd_id_e_type cmd,
                                      void* user_data,
                                      wms_cmd_err_e_type cmd_err )
{
  /* Delayed Response Packet */
  wmsdiag_set_bearer_pref_rsp_type *delay_rsp_pkt = NULL;

  /* Always setting it to 1 */
  diagpkt_subsys_delayed_rsp_id_type delay_rsp_id = 1;

  if (cmd != WMS_CMD_CFG_SET_GW_DOMAIN_PREF) 
  {
    /* Ignore other Commands */
    return;
  }

  /*  Allocate our packet */
  delay_rsp_pkt = (wmsdiag_set_bearer_pref_rsp_type *) diagpkt_subsys_alloc_v2_delay(
                            (diagpkt_subsys_id_type)DIAG_SUBSYS_WMS, 
                            (diagpkt_subsys_cmd_code_type)WMSDIAG_SET_BEARER_PREF_CMD, 
                            delay_rsp_id, 
                            sizeof(wmsdiag_set_bearer_pref_rsp_type) );

  if (delay_rsp_pkt == NULL)
  {
    MSG_ERROR("Response packet allocation Failed!",0,0,0);
    return;
  }
  else
  {
    /* Delayed Response */
    diagpkt_subsys_set_rsp_cnt(delay_rsp_pkt, 1);
    
    /* Set the Status */
    if (cmd_err == WMS_CMD_ERR_NONE)
    {
      diagpkt_subsys_set_status(delay_rsp_pkt, 0);
    }
    else
    {
      diagpkt_subsys_set_status(delay_rsp_pkt, 1);
    }
    
    /* Populate Fields */
    delay_rsp_pkt->transaction_ID  = (uint32)user_data;
    
    /* Send the Response */
    diagpkt_delay_commit(delay_rsp_pkt);
  }
}

/*===========================================================================

FUNCTION WMSDIAG_SET_ROUTES_RESULT_LOG

DESCRIPTION
  This function accepts a cfg_event_info_s_type structure and creates a
  wmsdiag_set_routes_log_type log packet that contains all the log
  information in the format specified in the Diag ICD.

DEPENDENCIES
  None.

RETURN VALUE
  None.

SIDE EFFECTS

===========================================================================*/

void wmsdiag_set_routes_result_log ()
{
  /*  Log response type */
  wmsdiag_set_routes_log_type* log_rsp;

  /*  Allocate the log packet */
  log_rsp = (wmsdiag_set_routes_log_type*)
    log_alloc (LOG_WMS_SET_ROUTES_C, sizeof (wmsdiag_set_routes_log_type));

  if (log_rsp)
  {
    /*  Set the transaction ID */
    log_rsp -> transaction_ID = (uint8) wmsdiag_transaction_ID;


    /*  Set the status */
    if ( wmsdiag_set_routes_failed )
    {
      log_rsp -> status = TRUE;
    }
    else
    {
      log_rsp -> status = FALSE;
    }

    /*  Commit our packet */
    log_commit (log_rsp);
  }
}


/*===========================================================================

FUNCTION WMSDIAG_DISPATCH_CFG_CB

DESCRIPTION
  Calls the proper cfg callback function.

DEPENDENCIES
  None.

RETURN VALUE
  None.

SIDE EFFECTS
  None.

===========================================================================*/

void wmsdiag_dispatch_cfg_cb (wms_cfg_event_e_type event,
                             wms_cfg_event_info_s_type *info_ptr)
{
  /*  Callback for a message list update */
  if (event ==  WMS_CFG_EVENT_MESSAGE_LIST)
  {
    wmsdiag_list_update_cb (event,info_ptr);
  }
  /*  Callback for a message set routes */
  else if (event == WMS_CFG_EVENT_ROUTES)
  {
    wmsdiag_set_routes_result_log ();
  }
  else if (event == WMS_CFG_EVENT_GW_DOMAIN_PREF)
  {
    wmsdiag_get_bearer_pref_cfg_cb(event, info_ptr);
  }
}

/*===========================================================================

FUNCTION WMS_DIAG_MAKE_MESSAGE_READ_RESULT_LOG

DESCRIPTION
  This function accepts a client_message_s_type structure and creates a
  wmsdiag_read_log_type log that contains all the log information in
  the format specified in the Diag ICD.

DEPENDENCIES
  None.

RETURN VALUE
  None.

SIDE EFFECTS
  None.

===========================================================================*/

void wms_diag_make_message_read_result_log (wms_client_message_s_type wmsdiag_message);



/*===========================================================================

FUNCTION WMSDIAG_DISPATCH_MSG_CB

DESCRIPTION
  Calls the proper msg related callback function.

DEPENDENCIES
  None.

RETURN VALUE
  None.

SIDE EFFECTS
  None.

===========================================================================*/

void wmsdiag_dispatch_msg_cb ( wms_msg_event_e_type event,
                               wms_msg_event_info_s_type *info_ptr,
                               boolean *shared )
{
  /*  Callback for a message read */
  if ((event ==  WMS_MSG_EVENT_READ) && (wmsdiag_initiated_read == TRUE))
  {
    /* reset the global flag */
    wmsdiag_initiated_read = FALSE;

    wms_diag_make_message_read_result_log (info_ptr -> status_info.message);
  }
}

/*===========================================================================

FUNCTION WMSDIAG_BC_ADD_DELETE_CMD_CB

DESCRIPTION
  Command Callback for Add Delete Service ID Command

DEPENDENCIES
  None.

RETURN VALUE
  None.

SIDE EFFECTS
  None.

===========================================================================*/
void wmsdiag_bc_add_delete_cmd_cb ( wms_cmd_id_e_type cmd,
                                void* user_data,
                                wms_cmd_err_e_type cmd_err )
{
  /* Delayed Response Packet */
  wmsdiag_broadcast_add_delete_serviceid_rsp_type *delay_rsp_pkt = NULL;

  /* Always setting it to 1 */
  diagpkt_subsys_delayed_rsp_id_type delay_rsp_id = 1;

  if ( (cmd != WMS_CMD_BC_MM_ADD_SRV) 
       && (cmd != WMS_CMD_BC_MM_DELETE_SRV) )
  {
    /* Ignore other Commands */
    return;
  }

  /*  Allocate our packet */
  delay_rsp_pkt = (wmsdiag_broadcast_add_delete_serviceid_rsp_type *) diagpkt_subsys_alloc_v2_delay(
                             (diagpkt_subsys_id_type)DIAG_SUBSYS_WMS, 
                             (diagpkt_subsys_cmd_code_type)WMSDIAG_BROADCAST_ADD_DELETE_SERVICEID_CMD, 
                             delay_rsp_id, 
                             sizeof(wmsdiag_broadcast_add_delete_serviceid_rsp_type) );

  if (delay_rsp_pkt == NULL)
  {
    MSG_ERROR("Response packet allocation Failed!",0,0,0);
    return;
  }
  else
  {
    /* Delayed Response */
    diagpkt_subsys_set_rsp_cnt(delay_rsp_pkt, 1);
    
    /* Set the Status */
    if (cmd_err == WMS_CMD_ERR_NONE)
    {
      diagpkt_subsys_set_status(delay_rsp_pkt, 0);
    }
    else
    {
      diagpkt_subsys_set_status(delay_rsp_pkt, 1);
    }

    /* Populate Fields */
    delay_rsp_pkt->transaction_ID = (uint8)(((uint32)user_data)&0xFF);
    delay_rsp_pkt->reserved_byte1 = 0;
    delay_rsp_pkt->reserved_byte2 = 0;
    delay_rsp_pkt->reserved_byte3 = 0;

    /* Send the Response */
    diagpkt_delay_commit(delay_rsp_pkt);
  }
}

/*===========================================================================

FUNCTION WMSDIAG_BC_DELETE_ALL_CMD_CB

DESCRIPTION
  Command Callback for Delete All Service IDs Command

DEPENDENCIES
  None.

RETURN VALUE
  None.

SIDE EFFECTS
  None.

===========================================================================*/
void wmsdiag_bc_delete_all_cmd_cb ( wms_cmd_id_e_type cmd,
                                void* user_data,
                                wms_cmd_err_e_type cmd_err )
{
  /* Delayed Response Packet */
  wmsdiag_broadcast_delete_all_services_rsp_type *delay_rsp_pkt = NULL;

  /* Always setting it to 1 */
  diagpkt_subsys_delayed_rsp_id_type delay_rsp_id = 1;

  if (cmd != WMS_CMD_BC_MM_DELETE_ALL_SERVICES) 
  {
    /* Ignore other Commands */
    return;
  }

  /*  Allocate our packet */
  delay_rsp_pkt = (wmsdiag_broadcast_delete_all_services_rsp_type *) diagpkt_subsys_alloc_v2_delay(
                             (diagpkt_subsys_id_type)DIAG_SUBSYS_WMS, 
                             (diagpkt_subsys_cmd_code_type)WMSDIAG_BROADCAST_DELETE_ALL_SERVICES_CMD, 
                             delay_rsp_id, 
                             sizeof(wmsdiag_broadcast_delete_all_services_rsp_type) );

  if (delay_rsp_pkt == NULL)
  {
    MSG_ERROR("Response packet allocation Failed!",0,0,0);
    return;
  }
  else
  {
    /* Delayed Response */
    diagpkt_subsys_set_rsp_cnt(delay_rsp_pkt, 1);
    
    /* Set the Status */
    if (cmd_err == WMS_CMD_ERR_NONE)
    {
      diagpkt_subsys_set_status(delay_rsp_pkt, 0);
    }
    else
    {
      diagpkt_subsys_set_status(delay_rsp_pkt, 1);
    }

    /* Populate Fields */
    delay_rsp_pkt->transaction_ID = (uint8)(((uint32)user_data)&0xFF);
    delay_rsp_pkt->reserved_byte1 = 0;
    delay_rsp_pkt->reserved_byte2 = 0;
    delay_rsp_pkt->reserved_byte3 = 0;

    /* Send the Response */
    diagpkt_delay_commit(delay_rsp_pkt);
  }
}

/*===========================================================================

FUNCTION WMSDIAG_DISPATCH_BC_MM_CB

DESCRIPTION
  Calls the proper multimode broadcast related callback function.

DEPENDENCIES
  None.

RETURN VALUE
  None.

SIDE EFFECTS
  None.

===========================================================================*/

void wmsdiag_dispatch_bc_mm_cb ( wms_bc_mm_event_e_type event,
                                 wms_bc_mm_event_info_s_type *info_ptr )
                                 
{
  /* Currently, we dont use this function */
}


/*===========================================================================

FUNCTION WMSDIAG_CLIENT_INIT

DESCRIPTION
  This procedure performs the necessary API calls to establish a client ID
  for our DM WMS session and map callback functions.

DEPENDENCIES
  None.

RETURN VALUE
  None.

SIDE EFFECTS
  Sets the static variable referencing our client ID.

===========================================================================*/
void wmsdiag_client_init (void)
{
  /*  This will initialize our client ID properly */
  if (wms_client_init (WMS_CLIENT_TYPE_DIAG, &wmsdiag_client_id) != WMS_CLIENT_ERR_NONE)
  {
    MSG_ERROR("WMSDIAG: Error in wms_client_init",0,0,0);
  }

  /*  Activate our client ID */
  if (wms_client_activate (wmsdiag_client_id) != WMS_CLIENT_ERR_NONE)
  {
    MSG_ERROR("WMSDIAG: Error in wms_client_activate",0,0,0);
  }

  /*  Register a callback function for the cfg events */
  /*  The registered fcn is a dispatcher that calls the appropriate fcn given
      an event */
  if (wms_client_reg_cfg_cb (wmsdiag_client_id, wmsdiag_dispatch_cfg_cb) != WMS_CLIENT_ERR_NONE)
  {
    MSG_ERROR("WMSDIAG: Error in wms_client_reg_cfg_cb",0,0,0);
  }

  /*  Register a callback function for the msg events */
  /*  The registered fcn is a dispatcher that calls the appropriate fcn given
      an event.  */
  if (wms_client_reg_msg_cb (wmsdiag_client_id, wmsdiag_dispatch_msg_cb) != WMS_CLIENT_ERR_NONE)
  {
    MSG_ERROR("WMSDIAG: Error in wms_client_reg_msg_cb",0,0,0);
  }

  /*  Register a callback function for the Broadcast events */
  /*  The registered fcn is a dispatcher that calls the appropriate fcn given
      an event.  */
  if (wms_client_reg_bc_mm_cb (wmsdiag_client_id, wmsdiag_dispatch_bc_mm_cb) != WMS_CLIENT_ERR_NONE)
  {
    MSG_ERROR("WMSDIAG: Error in wms_client_reg_bc_mm_cb",0,0,0);
  }

  return;
}

/*===========================================================================

FUNCTION WMSDIAG_SEND_WRITE

DESCRIPTION
  This function  attempts to send a message with the given properties or write
  a message, depending on the parameters passed in the request packet.

DEPENDENCIES
  None.

RETURN VALUE
  Pointer to response packet.

SIDE EFFECTS
  None.

===========================================================================*/

PACKED void * wmsdiag_send_write (
  PACKED void *req_pkt_ptr,  /* pointer to request packet  */
  uint16 pkt_len               /* length of request packet   */
)

{
  /*  Response pointer */
  wmsdiag_send_write_rsp_type *rsp_ptr  = NULL;

  /*  Cast the request pointer to proper type */
  wmsdiag_send_write_req_type *req_ptr  =
      (wmsdiag_send_write_req_type *) req_pkt_ptr;

  /*  Flag to check message mode */
  boolean is_CDSMS = TRUE;

#ifdef FEATURE_CDSMS
  static wms_cdma_message_s_type      cdma_message;
  static wms_address_s_type           cdma_message_addr;
  static wms_subaddress_s_type        cdma_message_subaddr;
  static wms_raw_ts_data_s_type       cdma_raw_data;
#endif /* FEATURE_CDSMS */

#ifdef FEATURE_GWSMS
#error code not present
#endif /* FEATURE_GWSMS */

  /*  Loop Counter */
  uint32 i;

  uint8 A = 0;  /*  Number of digits in the address */
  uint8 S = 0;  /*  Number of digits in the subaddress */
  uint32 D = 0;  /*  Length of the data in bytes */

  /*  Verify that the parameters passed were within the specified ranges */
  if ( req_ptr -> send_mode          >= WMSDIAG_SEND_MODE_MAX ||
       req_ptr -> write_mode         >= WMSDIAG_WRITE_MODE_MAX ||
       req_ptr -> msg_mode           >= WMSDIAG_MSG_MODE_MAX ||
       req_ptr -> tag_type           >= WMSDIAG_TAG_MAX ||
       req_ptr -> storage_type       >= WMSDIAG_MEM_STORE_MAX ||
       req_ptr -> addr_digit_mode    >= WMSDIAG_ADDR_DIGIT_MODE_MAX ||
       req_ptr -> addr_num_mode      >= WMSDIAG_ADDR_NUM_MODE_MAX ||
       req_ptr -> data_format        >= WMSDIAG_DATA_FORMAT_MAX ||
       req_ptr -> addr_num_digits    >  WMSDIAG_ADDRESS_MAX ||
       req_ptr -> subaddr_num_digits >  WMSDIAG_SUBADDRESS_MAX ||
       req_ptr -> user_data_length   >  WMSDIAG_MAX_LEN )
  {
    return (diagpkt_err_rsp (DIAG_BAD_PARM_F, req_pkt_ptr, pkt_len));
  }

  /*  Initialize the message mode flag */
  if (WMSDIAG_MSG_MODE_VALS [req_ptr -> msg_mode] == WMS_MESSAGE_MODE_CDMA)
  {
    is_CDSMS = TRUE;
  }
  else
  {
    is_CDSMS = FALSE;
  }

  #ifdef FEATURE_GWSMS
#error code not present
  #endif

  /*  If no client ID has been established, do so now. */

  if (wmsdiag_client_id == WMSDIAG_INVALID_CLIENT_ID)
  {
    wmsdiag_client_init ();
  }

  /*  Initialize counters for the number of address/subaddress digits as well as the data length */
  A = (uint8)req_ptr -> addr_num_digits;
  S = (uint8)req_ptr -> subaddr_num_digits;
  D = req_ptr -> user_data_length;

  if( A+S+D > WMSDIAG_DEFAULT_DATA_BUF_MAX )
  {
    return (diagpkt_err_rsp (DIAG_BAD_PARM_F, req_pkt_ptr, pkt_len));
  }

  /*  Allocate our packet */
  rsp_ptr = (wmsdiag_send_write_rsp_type*) diagpkt_subsys_alloc (DIAG_SUBSYS_WMS, WMSDIAG_SEND_WRITE_CMD,
                        sizeof (wmsdiag_send_write_rsp_type));

  if (rsp_ptr)
  {
    /*  Set the message header variables */
    msg_hdr.message_mode = WMSDIAG_MSG_MODE_VALS [req_ptr -> msg_mode];
    msg_hdr.tag = WMSDIAG_MSG_TAG_VALS [req_ptr -> tag_type];
    msg_hdr.mem_store = WMSDIAG_MEM_STORE_VALS [req_ptr -> storage_type];
    msg_hdr.index = req_ptr -> index;

    /*  Set the CDMA message variables */
  #ifdef FEATURE_CDSMS
    if (is_CDSMS)
    {

      cdma_message.is_mo = req_ptr -> is_MO;
      cdma_message.teleservice = (wms_teleservice_e_type) req_ptr -> teleservice_type;

      /*  Set the address information */
      cdma_message_addr.digit_mode = WMSDIAG_ADDR_DIGIT_MODE_VALS [req_ptr -> addr_digit_mode];
      cdma_message_addr.number_mode = WMSDIAG_ADDR_NUM_MODE_VALS [req_ptr -> addr_num_mode];
      cdma_message_addr.number_type = (wms_number_type_e_type) req_ptr -> addr_num_type;
      cdma_message_addr.number_plan = (wms_number_plan_e_type) req_ptr -> addr_num_plan_type;
      cdma_message_addr.number_of_digits = A;
      for (i = 0; i < A ; i++)
      {
        cdma_message_addr.digits [i] = req_ptr -> data [i];
      }

      cdma_message.address = cdma_message_addr;

      /*  Set the subaddress information */
      cdma_message_subaddr.type = (wms_subaddress_type_e_type) req_ptr -> subaddr_type;
      cdma_message_subaddr.odd = req_ptr -> subaddr_odd_or_even;
      cdma_message_subaddr.number_of_digits = S;

      for (i = 0; i < S; i++)
      {
        cdma_message_subaddr.digits[i] = req_ptr->data[i + A];
      }

      cdma_message.subaddress = cdma_message_subaddr;

      cdma_message.is_tl_ack_requested = req_ptr -> tl_ack_needed;
      cdma_message.is_service_present = req_ptr -> service_present;
      cdma_message.service = (wms_service_e_type) req_ptr -> service_type;

      cdma_raw_data.format = WMSDIAG_DATA_FORMAT_VALS [req_ptr -> data_format];
      cdma_raw_data.len = D;

      for (i = 0; i < D; i++)
      {
        cdma_raw_data.data [i] = req_ptr -> data [i + A + S];
      }

      cdma_message.raw_ts = cdma_raw_data;

      client_message.msg_hdr = msg_hdr;
      client_message.u.cdma_message = cdma_message;

      /*  Send out our packet now */
      diagpkt_commit (rsp_ptr);

      /*  If they want to just send it over the air */
      if (req_ptr -> write_message == 0)
      {
        (void)wms_msg_send (wmsdiag_client_id, NULL, (void *) req_ptr -> transaction_ID,
                  WMSDIAG_SEND_MODE_VALS [req_ptr -> send_mode],
                  &client_message);
      }
      /*  If they want to write it to memory */
      else
      {
        (void)wms_msg_write (wmsdiag_client_id, NULL, (void *) req_ptr -> transaction_ID,
                     WMSDIAG_WRITE_MODE_VALS [req_ptr -> write_mode],
                     &client_message);
      }
    } /* if (is_CDSMS) */

  #endif /* FEATURE_CDSMS */

  #ifdef FEATURE_GWSMS
#error code not present
  #endif /* FEATURE_GWSMS */
  }

  return NULL;
}

/*===========================================================================

FUNCTION WMSDIAG_CLEAR

DESCRIPTION
  This function  attempts to clear a message with the given index from
  the specified memory store.

DEPENDENCIES
  None.

RETURN VALUE
  Pointer to response packet.

SIDE EFFECTS
  None.

===========================================================================*/


PACKED void * wmsdiag_clear (
  PACKED void *req_pkt_ptr,  /* pointer to request packet  */
  uint16 pkt_len               /* length of request packet   */
)

{
  /*  Response Pointer */
  wmsdiag_clear_rsp_type *rsp_ptr  = NULL;

  /*  Cast request pointer to proper type */
  wmsdiag_clear_req_type *req_ptr  =
      (wmsdiag_clear_req_type *) req_pkt_ptr;

  /*  Verify that the parameters passed were within the specified ranges */
  if ( req_ptr -> storage_type >= WMSDIAG_MEM_STORE_MAX)
  {
    return (diagpkt_err_rsp (DIAG_BAD_PARM_F, req_pkt_ptr, pkt_len));
  }

  /*  If no client ID has been established, do so now. */

  if (wmsdiag_client_id == WMSDIAG_INVALID_CLIENT_ID)
  {
    wmsdiag_client_init ();
  }

  /*  Allocate our packet */
  rsp_ptr = (wmsdiag_clear_rsp_type*) diagpkt_subsys_alloc (DIAG_SUBSYS_WMS, WMSDIAG_CLEAR_CMD,
                        sizeof (wmsdiag_clear_rsp_type));

  if (rsp_ptr)
  {
    /*  Send out our packet now */
    diagpkt_commit (rsp_ptr);


    /*  Call the WMS API function to clear the message */
    (void)wms_msg_delete (wmsdiag_client_id, NULL, (void *) req_ptr -> transaction_ID,
                  WMSDIAG_MEM_STORE_VALS [req_ptr -> storage_type],
                  req_ptr -> index);
  }

  return NULL;

}

/*===========================================================================

FUNCTION WMSDIAG_CLEAR_ALL

DESCRIPTION
  This function attempts to clear all messages from the specified memory
  store.

DEPENDENCIES
  None.

RETURN VALUE
  Pointer to response packet.

SIDE EFFECTS
  None.

===========================================================================*/


PACKED void * wmsdiag_clear_all (
  PACKED void *req_pkt_ptr,  /* pointer to request packet  */
  uint16 pkt_len               /* length of request packet   */
)

{
  /*  Response Pointer */
  wmsdiag_clear_all_rsp_type *rsp_ptr  = NULL;

  /*  Cast copy of request pointer to proper type */
  wmsdiag_clear_all_req_type *req_ptr  =
      (wmsdiag_clear_all_req_type *) req_pkt_ptr;

  /*  Verify that the parameters passed were within the specified ranges */
  if ( req_ptr -> storage_type >= WMSDIAG_MEM_STORE_MAX)
  {
    return (diagpkt_err_rsp (DIAG_BAD_PARM_F, req_pkt_ptr, pkt_len));
  }

  /*  If no client ID has been established, do so now. */

  if (wmsdiag_client_id == WMSDIAG_INVALID_CLIENT_ID)
  {
    wmsdiag_client_init ();
  }

  /*  Allocate our packet */
  rsp_ptr = (wmsdiag_clear_all_rsp_type*) diagpkt_subsys_alloc (DIAG_SUBSYS_WMS, WMSDIAG_CLEAR_ALL_CMD,
                        sizeof (wmsdiag_clear_all_rsp_type));

  if (rsp_ptr)
  {
    /*  Send out our packet now */
    diagpkt_commit (rsp_ptr);

    /*  Call the WMS API function to clear the messages in the given memory store */
    (void)wms_msg_delete_all (wmsdiag_client_id, NULL, (void *) req_ptr -> transaction_ID,
                  WMSDIAG_MEM_STORE_VALS [req_ptr -> storage_type],
                  WMSDIAG_MSG_TAG_VALS [0]);
  }

  return NULL;

}

/*===========================================================================

FUNCTION WMSDIAG_GET_MESSAGE_LIST

DESCRIPTION
  This function returns all messages from the specified memory
  store.

DEPENDENCIES
  None.

RETURN VALUE
  Pointer to response packet.

SIDE EFFECTS
  Will cause a diag_block until DIAG_ASYNC_BLOCK_SIG is set.  This will be done
  by either the CFG callback function or the CMD callback.

===========================================================================*/

PACKED void * wmsdiag_get_message_list (
  PACKED void *req_pkt_ptr,  /* pointer to request packet  */
  uint16 pkt_len               /* length of request packet   */
)

{
  /*  Response pointer */
  wmsdiag_get_message_list_rsp_type *rsp_ptr  = NULL;

  /*  Cast a copy of the request pointer to the proper type */
  wmsdiag_get_message_list_req_type *req_ptr  =
      (wmsdiag_get_message_list_req_type *) req_pkt_ptr;

  /*  Variable to represent the final size of our packet */
  uint32 packet_size;

  /*  Loop counter */
  uint32 i;

  /* Drop re-entrant function calls if a request is still pending */
  if (req_pending != WMSDIAG_REQ_STATE_IDLE)
  {
    return rsp_ptr;
  }

  /*  Verify that the parameters passed were within the specified ranges */
  if ( req_ptr -> storage_type >= WMSDIAG_MEM_STORE_MAX)
  {
    return (diagpkt_err_rsp (DIAG_BAD_PARM_F, req_pkt_ptr, pkt_len));
  }

  wmsdiag_req_storage = (wms_memory_store_e_type) req_ptr -> storage_type;
  MSG_HIGH("wmsdiag_req_storage = %d",wmsdiag_req_storage,0,0);

  /*  If no client ID has been established, do so now. */

  if (wmsdiag_client_id == WMSDIAG_INVALID_CLIENT_ID)
  {
    wmsdiag_client_init ();
  }

  req_pending = WMSDIAG_REQ_STATE_COMMAND_CB;

  /*  Call the WMS API function to get the message list */
  (void)wms_cfg_get_message_list (wmsdiag_client_id, wmsdiag_get_message_list_cmd_cb,
                            (void *) req_ptr -> transaction_ID,
                            (wms_memory_store_e_type)req_ptr -> storage_type, WMSDIAG_MSG_TAG_VALS [0]);

  /*  ASYNC WAIT ON A SIGNAL */
  diagpkt_async_block ();

  MSG_HIGH("async block has been released",0,0,0);
  /*  Once the signal is set, we can update the response pointer parameters */
  /*  Make a flag and set it if the cmd callback showed that there was an error, if the flag is set, err pkt */

  /*  Allocate our packet */
  if (wmsdiag_get_message_list_failed)
  {
  /*  If the cmd failed, we have no messages to hold, so we don't allocate space for them */
    packet_size = FPOS (wmsdiag_get_message_list_rsp_type, messages);
  }
  else
  {
  /*  If the cmd succeeded, allocate space for the packet + messages */
    packet_size = FPOS (wmsdiag_get_message_list_rsp_type, messages) +
                sizeof (WMSDIAG_MessageListEntryType) * (wmsdiag_list.num_entries );
  }

  rsp_ptr = (wmsdiag_get_message_list_rsp_type*) diagpkt_subsys_alloc (DIAG_SUBSYS_WMS, WMSDIAG_GET_MESSAGE_LIST_CMD, packet_size);

  if (rsp_ptr)
  {
    /*  Set up the parameters */
    rsp_ptr->num_entries = wmsdiag_list.num_entries;
    rsp_ptr->storage_type = wmsdiag_list.storage_type;
    rsp_ptr->voice_mail_index = wmsdiag_list.voice_mail_index;
    rsp_ptr->voice_mail_active = wmsdiag_list.voice_mail_active;

    /*  If the get message list command failed, set the status */
    if (wmsdiag_get_message_list_failed)
    {
      rsp_ptr -> status = 1;
    }
    /*  If not, set the status and the message parameters */
    else
    {
      rsp_ptr -> status = 0;


      for (i = 0; i < (wmsdiag_list.num_entries); i++)
      {
        rsp_ptr->messages[i].tag_type = wmsdiag_list.messages[i].tag_type;
        rsp_ptr->messages[i].index = wmsdiag_list.messages[i].index;
      }
    }
  }

  return (rsp_ptr);

}


/*===========================================================================

FUNCTION WMSDIAG_SET_MESSAGE_TAG

DESCRIPTION
  This function alters the tag of the selected message.

DEPENDENCIES
  None.

RETURN VALUE
  Pointer to response packet.

SIDE EFFECTS
  None.

===========================================================================*/


PACKED void * wmsdiag_set_message_tag (
  PACKED void *req_pkt_ptr,  /* pointer to request packet  */
  uint16 pkt_len               /* length of request packet   */
)

{
  /*  Response Pointer */
  wmsdiag_set_message_tag_rsp_type *rsp_ptr  = NULL;

  /*  Copy of request pointer cast to proper type */
  wmsdiag_set_message_tag_req_type *req_ptr  =
      (wmsdiag_set_message_tag_req_type *) req_pkt_ptr;

  /*  Verify that the parameters passed were within the specified ranges */
  if ( req_ptr -> storage_type >= WMSDIAG_MEM_STORE_MAX ||
       req_ptr -> tag_type     >= WMSDIAG_TAG_MAX)
  {
    return (diagpkt_err_rsp (DIAG_BAD_PARM_F, req_pkt_ptr, pkt_len));
  }

  /*  If no client ID has been established, do so now. */

  if (wmsdiag_client_id == WMSDIAG_INVALID_CLIENT_ID)
  {
    wmsdiag_client_init ();
  }

  /*  Allocate our packet */
  rsp_ptr = (wmsdiag_set_message_tag_rsp_type*) diagpkt_subsys_alloc (DIAG_SUBSYS_WMS, WMSDIAG_SET_MESSAGE_TAG_CMD,
                        sizeof (wmsdiag_set_message_tag_rsp_type));
  if (rsp_ptr)
  {
    /*  Send out our packet now */
    diagpkt_commit (rsp_ptr);

    /*  Call the WMS API function to alter the tag */
    (void)wms_msg_modify_tag (wmsdiag_client_id, NULL, (void *) req_ptr -> transaction_ID,
                  WMSDIAG_MEM_STORE_VALS [req_ptr -> storage_type],
                  req_ptr -> index,
                  WMSDIAG_MSG_TAG_VALS [req_ptr -> tag_type]);
  }

  return NULL;

}

/*===========================================================================

FUNCTION WMSDIAG_READ

DESCRIPTION
  This function allows the user to read a message.  The response only
  indicates that a read was attempted.

DEPENDENCIES
  None.

RETURN VALUE
  Pointer to response packet.

SIDE EFFECTS
  None.

===========================================================================*/


PACKED void * wmsdiag_read (
  PACKED void *req_pkt_ptr,  /* pointer to request packet  */
  uint16 pkt_len               /* length of request packet   */
)

{
  /*  Response pointer */
  wmsdiag_read_rsp_type *rsp_ptr  = NULL;

  /*  Copy of request pointer cast to proper type */
  wmsdiag_read_req_type *req_ptr  =
      (wmsdiag_read_req_type *) req_pkt_ptr;

  wms_status_e_type st = WMS_OK_S;

  /* WMS DIAG is initiating this read command */
  wmsdiag_initiated_read = TRUE;

  wmsdiag_transaction_ID = req_ptr->transaction_ID;

  /*  Verify that the parameters passed were within the specified ranges */
  if ( req_ptr -> storage_type >= WMSDIAG_MEM_STORE_MAX)
  {
    return (diagpkt_err_rsp (DIAG_BAD_PARM_F, req_pkt_ptr, pkt_len));
  }

  /*  If no client ID has been established, do so now. */
  if (wmsdiag_client_id == WMSDIAG_INVALID_CLIENT_ID)
  {
    wmsdiag_client_init ();
  }

  /*  Allocate our packet */
  rsp_ptr = (wmsdiag_read_rsp_type*) diagpkt_subsys_alloc (DIAG_SUBSYS_WMS, WMSDIAG_READ_CMD,
                        sizeof (wmsdiag_read_rsp_type));

  if (rsp_ptr)
  {
    /*  Call the WMS API function to read a message */
    st = wms_msg_read (wmsdiag_client_id, NULL, (void *) req_ptr -> transaction_ID,
                  (wms_memory_store_e_type)req_ptr -> storage_type, req_ptr -> index);

    if (st != WMS_OK_S)
    {
      rsp_ptr -> status = 1;
      wmsdiag_read_failed = TRUE;
      MSG_ERROR("DTC command failed",0,0,0);
    }
    else
    {
      rsp_ptr -> status = 0;
      wmsdiag_read_failed = FALSE;
    }

    /*  Send out our packet now */
    diagpkt_commit (rsp_ptr);
  }

  return NULL;

}

/*===========================================================================

FUNCTION WMS_DIAG_MAKE_MESSAGE_READ_RESULT_LOG

DESCRIPTION
  This function accepts a client_message_s_type structure and creates a
  wmsdiag_read_log_type log that contains all the log information in
  the format specified in the Diag ICD.

DEPENDENCIES
  None.

RETURN VALUE
  None.

SIDE EFFECTS
  None.

===========================================================================*/

void wms_diag_make_message_read_result_log (wms_client_message_s_type wmsdiag_message )
{

  /*  Log response type */
  wmsdiag_read_log_type* log_rsp;

  uint32 A = 0;  /*  Number of digits in the address */
  uint32 S = 0;  /*  Number of digits in the subaddress */
  uint32 D = 0;  /*  Length of the data in bytes */
  uint32 i;  /*  Loop counter */
  uint32 log_size; /*  Eventually set to size of our log */
  uint32 switch_val = 0; /*  Temp value used for switch statements */
  boolean is_CDSMS = TRUE; /*  Flag for message mode type */

  /*
    Allocate the log packet, it is too large now, but we will shorten
  it later when we know exactly how large the payload is.
  */
  log_rsp = (wmsdiag_read_log_type*)
    log_alloc (LOG_WMS_READ_C, sizeof (wmsdiag_read_log_type));

  if (log_rsp)
  {
    /*  Set the transaction ID */
    log_rsp -> transaction_ID = (uint8) wmsdiag_transaction_ID;

    /*  Set the status */
    if ( wmsdiag_read_failed )
    {
      log_rsp -> status = TRUE;
    }
    else
    {
      log_rsp -> status = FALSE;
    }

    /*  Set the message mode */
    switch (wmsdiag_message.msg_hdr.message_mode)
    {
    case WMS_MESSAGE_MODE_CDMA:
      log_rsp -> msg_mode = WMSDIAG_MSG_MODE_CDMA;
      is_CDSMS = TRUE;
      break;

    case WMS_MESSAGE_MODE_GW:
      log_rsp -> msg_mode = WMSDIAG_MSG_MODE_GW;
      is_CDSMS = FALSE;
      break;

    default:
      log_rsp -> msg_mode = WMSDIAG_MSG_MODE_INVALID;
      break;
    }

    #ifdef FEATURE_CDSMS
    if (is_CDSMS)
    {
      /*  Set the counters showing the size of the address, subaddress and data */
      A = wmsdiag_message.u.cdma_message.address.number_of_digits;
      S = wmsdiag_message.u.cdma_message.subaddress.number_of_digits;
      D = wmsdiag_message.u.cdma_message.raw_ts.len;
    }
    #endif
    #ifdef FEATURE_GWSMS
#error code not present
    #endif

    /*  Set the tag type */
    log_rsp->tag_type = (uint32)wmsdiag_map_tag_type(wmsdiag_message.msg_hdr.tag);

    /*  Set the memory store type */
    switch (wmsdiag_message.msg_hdr.mem_store)
    {
    case WMS_MEMORY_STORE_NONE:
      log_rsp -> storage_type = WMSDIAG_MEM_STORE_NONE;
      break;

    case WMS_MEMORY_STORE_RAM_CDMA:
      log_rsp -> storage_type = WMSDIAG_MEM_STORE_RAM_CDMA;
      break;

    case WMS_MEMORY_STORE_RUIM:
      log_rsp -> storage_type = WMSDIAG_MEM_STORE_RUIM;
      break;

    case WMS_MEMORY_STORE_NV_CDMA:
      log_rsp -> storage_type = WMSDIAG_MEM_STORE_NV_CDMA;
      break;

    case WMS_MEMORY_STORE_RAM_GW:
      log_rsp -> storage_type = WMSDIAG_MEM_STORE_RAM_GW;
      break;

    case WMS_MEMORY_STORE_SIM:
      log_rsp -> storage_type = WMSDIAG_MEM_STORE_SIM;
      break;

    case WMS_MEMORY_STORE_NV_GW:
      log_rsp -> storage_type = WMSDIAG_MEM_STORE_NV_GW;
      break;

    default:
      log_rsp -> storage_type = WMSDIAG_MEM_STORE_INVALID;
      break;
    }

    /*  Set the index */
    log_rsp -> index = wmsdiag_message.msg_hdr.index;

    #ifdef FEATURE_CDSMS

    if (is_CDSMS)
    {
      /*  Set whether its MO or MT */
      log_rsp -> is_MO = wmsdiag_message.u.cdma_message.is_mo;

      /*  Set the teleservice type */
      log_rsp -> teleservice_type = wmsdiag_message.u.cdma_message.teleservice;
    }

    #endif /* FEATURE CDSMS */

    /*  Set the address digit mode */
    #ifdef FEATURE_CDSMS
    if (is_CDSMS)
    {
      switch_val = (wmsdiag_message.u.cdma_message.address.digit_mode);
    }
    #endif
    #ifdef FEATURE_GWSMS
#error code not present
    #endif

    switch (switch_val)
    {
    case WMS_DIGIT_MODE_4_BIT:
      log_rsp -> addr_digit_mode = WMSDIAG_ADDR_DIGIT_MODE_4_BIT;
      break;

    case WMS_DIGIT_MODE_8_BIT:
      log_rsp -> addr_digit_mode = WMSDIAG_ADDR_DIGIT_MODE_8_BIT;
      break;

    default:
      log_rsp -> addr_digit_mode = WMSDIAG_ADDR_DIGIT_MODE_INVALID;
      break;
    }

    /*  Set the address number mode */
    #ifdef FEATURE_CDSMS
    if (is_CDSMS)
    {
      switch_val = (wmsdiag_message.u.cdma_message.address.number_mode);
    }
    #endif
    #ifdef FEATURE_GWSMS
#error code not present
    #endif

    switch (switch_val)
    {
    case WMS_NUMBER_MODE_NONE_DATA_NETWORK:
      log_rsp -> addr_num_mode = WMSDIAG_ADDR_NUM_MODE_NOT_DATA_NTWK;
      break;

    case WMS_NUMBER_MODE_DATA_NETWORK:
      log_rsp -> addr_num_mode = WMSDIAG_ADDR_NUM_MODE_DATA_NTWK;
      break;

    default:
      log_rsp -> addr_num_mode = WMSDIAG_ADDR_NUM_MODE_INVALID;
      break;
    }

    /*  Set the address number type */
    #ifdef FEATURE_CDSMS
    if (is_CDSMS)
    {
      log_rsp -> addr_num_type = wmsdiag_message.u.cdma_message.address.number_type;
    }
    #endif
    #ifdef FEATURE_GWSMS
#error code not present
    #endif

    /*  Set the address number plan */
    #ifdef FEATURE_CDSMS
    if (is_CDSMS)
    {
      log_rsp -> addr_num_plan_type = wmsdiag_message.u.cdma_message.address.number_plan;
    }
    #endif
    #ifdef FEATURE_GWSMS
#error code not present
    #endif

    /*  Set the address number of digits */
    log_rsp -> addr_num_digits = (uint16)A;

    /*  Set the address digits */
    for (i = 0; i < A; i++)
    {
    #ifdef FEATURE_CDSMS
    if(is_CDSMS)
    {
      log_rsp -> data [i] = wmsdiag_message.u.cdma_message.address.digits [i];
    }
    #endif
    #ifdef FEATURE_GWSMS
#error code not present
    #endif
    }

    #ifdef FEATURE_CDSMS

    if (is_CDSMS)
    {

      /*  Set the subaddress type */
      log_rsp -> subaddr_type = wmsdiag_message.u.cdma_message.subaddress.type;

      /*  Set whether an odd or even number of digits in subaddr */
      log_rsp -> subaddr_odd_or_even = wmsdiag_message.u.cdma_message.subaddress.odd;

      /*  Set the subaddress number of digits */
      log_rsp -> subaddr_num_digits = (uint16)S;

      /*  Set the subaddress digits */
      for (i = 0; i < S; i++)
      {
        log_rsp->data[i + A] = wmsdiag_message.u.cdma_message.subaddress.digits [i];
      }

      /*  Set if ACK needed */
      log_rsp -> tl_ack_needed = wmsdiag_message.u.cdma_message.is_tl_ack_requested;

      /*  Set if service is present or not */
      log_rsp -> service_present = wmsdiag_message.u.cdma_message.is_service_present;

      /*  Set the service type */
      log_rsp -> service_type = wmsdiag_message.u.cdma_message.service;

    }

    #endif /*  FEATURE_CDSMS */

    /*  Set the data format */
    #ifdef FEATURE_CDSMS
    switch_val = (wmsdiag_message.u.cdma_message.raw_ts.format);
    #endif
    #ifdef FEATURE_GWSMS
#error code not present
    #endif
    switch (switch_val)
    {
    case WMS_FORMAT_CDMA:
      log_rsp -> data_format = WMSDIAG_DATA_FORMAT_CDMA;
      break;

    case WMS_FORMAT_ANALOG_CLI:
      log_rsp -> data_format = WMSDIAG_DATA_FORMAT_ANALOG_CLI;
      break;

    case WMS_FORMAT_ANALOG_VOICE_MAIL:
      log_rsp -> data_format = WMSDIAG_DATA_FORMAT_ANALOG_VOICE_MAIL;
      break;

    case WMS_FORMAT_ANALOG_SMS:
      log_rsp -> data_format = WMSDIAG_DATA_FORMAT_ANALOG_SMS;
      break;

    case WMS_FORMAT_ANALOG_AWISMS:
      log_rsp -> data_format = WMSDIAG_DATA_FORMAT_ANALOG_AWISMS;
      break;

    case WMS_FORMAT_MWI:
      log_rsp -> data_format = WMSDIAG_DATA_FORMAT_MWI;
      break;

    case WMS_FORMAT_GW_PP:
      log_rsp -> data_format = WMSDIAG_DATA_FORMAT_GW_PP;
      break;

    case WMS_FORMAT_GW_CB:
      log_rsp -> data_format = WMSDIAG_DATA_FORMAT_GW_CB;
      break;

    default:
      log_rsp -> data_format = WMSDIAG_DATA_FORMAT_INVALID;
      break;
    }

    /*  Set the data length */
    log_rsp -> user_data_length = D;

    /*  Set the raw data */
    for (i = 0; i < D; i ++)
    {
      #ifdef FEATURE_CDSMS
      if(is_CDSMS)
      {
        log_rsp->data[i + A + S] = wmsdiag_message.u.cdma_message.raw_ts.data [i];
      }
      #endif
      #ifdef FEATURE_GWSMS
#error code not present
      #endif
    }

    /*  Calculate the size of our log packet */
    log_size = FPOS (wmsdiag_read_log_type, data) + A + S + D;

    /*  Adjust the size accordingly */
    log_shorten (log_rsp, log_size);

    /*  Commit our packet */
    log_commit  (log_rsp);
  }

  return;

}


/*===========================================================================

FUNCTION WMSDIAG_SET_ROUTES

DESCRIPTION
  Sets the routes for all WMS message classes.

DEPENDENCIES
  None.

RETURN VALUE
  Pointer to response packet.

SIDE EFFECTS
  Uses a diag_block until the DIAG_ASYNC_BLOCK_SIG is set.  This will be done
  by either the CFG callback event being generated or the CMD callback producing
  an error.

===========================================================================*/


PACKED void * wmsdiag_set_routes (
  PACKED void *req_pkt_ptr,  /* pointer to request packet  */
  uint16 pkt_len               /* length of request packet   */
)

{
  /*  Response pointer */
  wmsdiag_set_routes_rsp_type *rsp_ptr  = NULL;

  /*  Copy of request pointer cast to proper type */
  wmsdiag_set_routes_req_type *req_ptr  =
      (wmsdiag_set_routes_req_type *) req_pkt_ptr;

  /*  The routing info we will pass to the WMS API */
  wms_routes_s_type wmsdiag_routes;

  wms_status_e_type st = WMS_OK_S;

  wmsdiag_transaction_ID = req_ptr->transaction_ID;

  /*  Verify that the PP parameters passed were within the specified ranges */
  if ( req_ptr -> wmsdiag_msg_class_cdma_storage_pp >= WMSDIAG_MEM_STORE_MAX ||
       req_ptr -> wmsdiag_msg_class_cdma_route_pp   >= WMSDIAG_ROUTE_MAX||
       req_ptr -> wmsdiag_msg_class_none_storage_pp >= WMSDIAG_MEM_STORE_MAX ||
       req_ptr -> wmsdiag_msg_class_none_route_pp   >= WMSDIAG_ROUTE_MAX||
       req_ptr -> wmsdiag_msg_class0_storage_pp >= WMSDIAG_MEM_STORE_MAX ||
       req_ptr -> wmsdiag_msg_class0_route_pp   >= WMSDIAG_ROUTE_MAX||
       req_ptr -> wmsdiag_msg_class1_storage_pp >= WMSDIAG_MEM_STORE_MAX ||
       req_ptr -> wmsdiag_msg_class1_route_pp   >= WMSDIAG_ROUTE_MAX||
       req_ptr -> wmsdiag_msg_class2_storage_pp >= WMSDIAG_MEM_STORE_MAX ||
       req_ptr -> wmsdiag_msg_class2_route_pp   >= WMSDIAG_ROUTE_MAX||
       req_ptr -> wmsdiag_msg_class3_storage_pp >= WMSDIAG_MEM_STORE_MAX ||
       req_ptr -> wmsdiag_msg_class3_route_pp   >= WMSDIAG_ROUTE_MAX)

  {
    return (diagpkt_err_rsp (DIAG_BAD_PARM_F, req_pkt_ptr, pkt_len));
  }

#ifdef FEATURE_CDSMS_BROADCAST
  /*  Verify that the BC parameters passed were within the specified ranges */
  if ( req_ptr -> wmsdiag_msg_class_cdma_storage_bc >= WMSDIAG_MEM_STORE_MAX ||
       req_ptr -> wmsdiag_msg_class_cdma_route_bc   >= WMSDIAG_ROUTE_MAX||
       req_ptr -> wmsdiag_msg_class_none_storage_bc >= WMSDIAG_MEM_STORE_MAX ||
       req_ptr -> wmsdiag_msg_class_none_route_bc   >= WMSDIAG_ROUTE_MAX||
       req_ptr -> wmsdiag_msg_class0_storage_bc >= WMSDIAG_MEM_STORE_MAX ||
       req_ptr -> wmsdiag_msg_class0_route_bc   >= WMSDIAG_ROUTE_MAX||
       req_ptr -> wmsdiag_msg_class1_storage_bc >= WMSDIAG_MEM_STORE_MAX ||
       req_ptr -> wmsdiag_msg_class1_route_bc   >= WMSDIAG_ROUTE_MAX||
       req_ptr -> wmsdiag_msg_class2_storage_bc >= WMSDIAG_MEM_STORE_MAX ||
       req_ptr -> wmsdiag_msg_class2_route_bc   >= WMSDIAG_ROUTE_MAX||
       req_ptr -> wmsdiag_msg_class3_storage_bc >= WMSDIAG_MEM_STORE_MAX ||
       req_ptr -> wmsdiag_msg_class3_route_bc   >= WMSDIAG_ROUTE_MAX)

  {
    return (diagpkt_err_rsp (DIAG_BAD_PARM_F, req_pkt_ptr, pkt_len));
  }
#endif /* FEATURE_CDSMS_BROADCAST */


  /*  If no client ID has been established, do so now. */

  if (wmsdiag_client_id == WMSDIAG_INVALID_CLIENT_ID)
  {
    wmsdiag_client_init ();
  }

  /*  Allocate our packet */
  rsp_ptr = (wmsdiag_set_routes_rsp_type*) diagpkt_subsys_alloc (DIAG_SUBSYS_WMS, WMSDIAG_SET_ROUTES_CMD,
                        sizeof (wmsdiag_set_routes_rsp_type));
  if (rsp_ptr)
  {
    /*  Set all the PP routes */
    wmsdiag_routes.pp_routes [WMS_MESSAGE_CLASS_0].route =
        WMSDIAG_ROUTE_VALS [ req_ptr -> wmsdiag_msg_class0_route_pp ];
    wmsdiag_routes.pp_routes [WMS_MESSAGE_CLASS_0].mem_store =
        WMSDIAG_MEM_STORE_VALS [ req_ptr -> wmsdiag_msg_class0_storage_pp ];

    wmsdiag_routes.pp_routes [WMS_MESSAGE_CLASS_1].route =
        WMSDIAG_ROUTE_VALS [ req_ptr -> wmsdiag_msg_class1_route_pp ];
    wmsdiag_routes.pp_routes [WMS_MESSAGE_CLASS_1].mem_store =
        WMSDIAG_MEM_STORE_VALS [ req_ptr -> wmsdiag_msg_class1_storage_pp ];

    wmsdiag_routes.pp_routes [WMS_MESSAGE_CLASS_2].route =
        WMSDIAG_ROUTE_VALS [ req_ptr -> wmsdiag_msg_class2_route_pp ];
    wmsdiag_routes.pp_routes [WMS_MESSAGE_CLASS_2].mem_store =
        WMSDIAG_MEM_STORE_VALS [ req_ptr -> wmsdiag_msg_class2_storage_pp ];

    wmsdiag_routes.pp_routes [WMS_MESSAGE_CLASS_3].route =
        WMSDIAG_ROUTE_VALS [ req_ptr -> wmsdiag_msg_class3_route_pp ];
    wmsdiag_routes.pp_routes [WMS_MESSAGE_CLASS_3].mem_store =
        WMSDIAG_MEM_STORE_VALS [ req_ptr -> wmsdiag_msg_class3_storage_pp ];

    wmsdiag_routes.pp_routes [WMS_MESSAGE_CLASS_NONE].route =
        WMSDIAG_ROUTE_VALS [ req_ptr -> wmsdiag_msg_class_none_route_pp ];
    wmsdiag_routes.pp_routes [WMS_MESSAGE_CLASS_NONE].mem_store =
        WMSDIAG_MEM_STORE_VALS [ req_ptr -> wmsdiag_msg_class_none_storage_pp ];

    wmsdiag_routes.pp_routes [WMS_MESSAGE_CLASS_CDMA].route =
        WMSDIAG_ROUTE_VALS [ req_ptr -> wmsdiag_msg_class_cdma_route_pp ];
    wmsdiag_routes.pp_routes [WMS_MESSAGE_CLASS_CDMA].mem_store =
        WMSDIAG_MEM_STORE_VALS [ req_ptr -> wmsdiag_msg_class_cdma_storage_pp ];

#ifdef FEATURE_CDSMS_BROADCAST
    /*  Set all the BC routes */
    wmsdiag_routes.bc_routes [WMS_MESSAGE_CLASS_0].route =
        WMSDIAG_ROUTE_VALS [ req_ptr -> wmsdiag_msg_class0_route_bc ];
    wmsdiag_routes.bc_routes [WMS_MESSAGE_CLASS_0].mem_store =
        WMSDIAG_MEM_STORE_VALS [ req_ptr -> wmsdiag_msg_class0_storage_bc ];

    wmsdiag_routes.bc_routes [WMS_MESSAGE_CLASS_1].route =
        WMSDIAG_ROUTE_VALS [ req_ptr -> wmsdiag_msg_class1_route_bc ];
    wmsdiag_routes.bc_routes [WMS_MESSAGE_CLASS_1].mem_store =
        WMSDIAG_MEM_STORE_VALS [ req_ptr -> wmsdiag_msg_class1_storage_bc ];

    wmsdiag_routes.bc_routes [WMS_MESSAGE_CLASS_2].route =
        WMSDIAG_ROUTE_VALS [ req_ptr -> wmsdiag_msg_class2_route_bc ];
    wmsdiag_routes.bc_routes [WMS_MESSAGE_CLASS_2].mem_store =
        WMSDIAG_MEM_STORE_VALS [ req_ptr -> wmsdiag_msg_class2_storage_bc ];

    wmsdiag_routes.bc_routes [WMS_MESSAGE_CLASS_3].route =
        WMSDIAG_ROUTE_VALS [ req_ptr -> wmsdiag_msg_class3_route_bc ];
    wmsdiag_routes.bc_routes [WMS_MESSAGE_CLASS_3].mem_store =
        WMSDIAG_MEM_STORE_VALS [ req_ptr -> wmsdiag_msg_class3_storage_bc ];

    wmsdiag_routes.bc_routes [WMS_MESSAGE_CLASS_NONE].route =
        WMSDIAG_ROUTE_VALS [ req_ptr -> wmsdiag_msg_class_none_route_bc ];
    wmsdiag_routes.bc_routes [WMS_MESSAGE_CLASS_NONE].mem_store =
        WMSDIAG_MEM_STORE_VALS [ req_ptr -> wmsdiag_msg_class_none_storage_bc ];

    wmsdiag_routes.bc_routes [WMS_MESSAGE_CLASS_CDMA].route =
        WMSDIAG_ROUTE_VALS [ req_ptr -> wmsdiag_msg_class_cdma_route_bc ];
    wmsdiag_routes.bc_routes [WMS_MESSAGE_CLASS_CDMA].mem_store =
        WMSDIAG_MEM_STORE_VALS [ req_ptr -> wmsdiag_msg_class_cdma_storage_bc ];

#endif /* FEATURE_CDSMS_BROADCAST */

#ifdef FEATURE_GWSMS
#error code not present
#endif

    /*  Call the WMS API function to set the routes */
    st = wms_cfg_set_routes (wmsdiag_client_id, NULL, (void *) req_ptr -> transaction_ID,
                        &wmsdiag_routes);

    if(st != WMS_OK_S)
    {
      rsp_ptr -> status = 1;
      wmsdiag_set_routes_failed = TRUE;
      MSG_ERROR("DTC command failed",0,0,0);
    }
    else
    {
      rsp_ptr -> status = 0;
      wmsdiag_set_routes_failed = FALSE;
    }

    /*  Send out our packet now */
    diagpkt_commit (rsp_ptr);
  }

  return NULL;

}

/*===========================================================================

FUNCTION WMSDIAG_ORIG_END_SMS_CALL

DESCRIPTION
  This function attempts to originate or end a SMS call.

DEPENDENCIES
  None.

RETURN VALUE
  Pointer to response packet.

SIDE EFFECTS
  None.

===========================================================================*/


PACKED void * wmsdiag_orig_end_sms_call (
  PACKED void *req_pkt_ptr,  /* pointer to request packet  */
  uint16 pkt_len               /* length of request packet   */
)
{
  /*  Response Pointer */
  wmsdiag_orig_end_sms_call_rsp_type *rsp_ptr  = NULL;

  /*  Cast copy of request pointer to proper type */
  wmsdiag_orig_end_sms_call_req_type *req_ptr  =
      (wmsdiag_orig_end_sms_call_req_type *) req_pkt_ptr;

  wms_status_e_type st = WMS_OK_S;

  if( req_ptr->is_orig == TRUE      &&
      req_ptr->so != WMS_DC_SO_AUTO &&
      req_ptr->so != WMS_DC_SO_6    &&
      req_ptr->so != WMS_DC_SO_14 )
  {
    return (diagpkt_err_rsp (DIAG_BAD_PARM_F, req_pkt_ptr, pkt_len));
  }

  /*  If no client ID has been established, do so now. */
  if (wmsdiag_client_id == WMSDIAG_INVALID_CLIENT_ID)
  {
    wmsdiag_client_init ();
  }

  /*  Allocate our packet */
  rsp_ptr = (wmsdiag_orig_end_sms_call_rsp_type*) diagpkt_subsys_alloc (DIAG_SUBSYS_WMS, WMSDIAG_ORIG_END_SMS_CALL_CMD,
                        sizeof (wmsdiag_orig_end_sms_call_rsp_type));

  if (rsp_ptr)
  {
    /*  Send out our packet now */
    diagpkt_commit (rsp_ptr);

    /* originate or end ? */
    if(req_ptr->is_orig)
    {
      st = wms_dc_connect(wmsdiag_client_id, NULL, NULL,
                          (wms_dc_so_e_type)req_ptr->so);
    }
    else
    {
      st = wms_dc_disconnect(wmsdiag_client_id, NULL, NULL);
    }

    if(st != WMS_OK_S)
    {
      MSG_ERROR("DTC command failed",0,0,0);
    }
  }

  return NULL;
}

/*===========================================================================

FUNCTION WMSDIAG_ENABLE_DISABLE_AUTO_DISCONNECT

DESCRIPTION
  This function attempts to enable or disable auto disconnect of a SMS call.

DEPENDENCIES
  None.

RETURN VALUE
  Pointer to response packet.

SIDE EFFECTS
  None.

===========================================================================*/


PACKED void * wmsdiag_enable_disable_auto_disconnect (
  PACKED void *req_pkt_ptr,  /* pointer to request packet  */
  uint16 pkt_len               /* length of request packet   */
)
{
  /*  Response Pointer */
  wmsdiag_enable_disable_auto_disconnect_rsp_type *rsp_ptr  = NULL;

  /*  Cast copy of request pointer to proper type */
  wmsdiag_enable_disable_auto_disconnect_req_type *req_ptr  =
      (wmsdiag_enable_disable_auto_disconnect_req_type *) req_pkt_ptr;

  wms_status_e_type st = WMS_OK_S;

  if( req_ptr->enable_auto_disconnect == TRUE  &&
      req_ptr->timeout == NULL )
  {
    return (diagpkt_err_rsp (DIAG_BAD_PARM_F, req_pkt_ptr, pkt_len));
  }

  /*  If no client ID has been established, do so now. */
  if (wmsdiag_client_id == WMSDIAG_INVALID_CLIENT_ID)
  {
    wmsdiag_client_init ();
  }

  /*  Allocate our packet */
  rsp_ptr = (wmsdiag_enable_disable_auto_disconnect_rsp_type*)
            diagpkt_subsys_alloc (DIAG_SUBSYS_WMS, WMSDIAG_AUTO_DISCONNECT_CMD,
                      sizeof (wmsdiag_enable_disable_auto_disconnect_rsp_type));

  if (rsp_ptr)
  {
    /*  Send out our packet now */
    diagpkt_commit (rsp_ptr);

    /* enable or disable ? */
    if(req_ptr->enable_auto_disconnect == TRUE)
    {
      st = wms_dc_enable_auto_disconnect(wmsdiag_client_id, NULL, NULL,
                                         req_ptr->timeout);
    }
    else
    {
      st = wms_dc_disable_auto_disconnect(wmsdiag_client_id, NULL, NULL);
    }

    if(st != WMS_OK_S)
    {
      MSG_ERROR("DTC command failed",0,0,0);
    }
  }

  return NULL;
}

/*===========================================================================
FUNCTION wmsdiag_broadcast_add_delete_serviceid

DESCRIPTION
  Function to process the WMSDIAG_BROADCAST_ADD_DELET_SERVICEID_CMD

DEPENDENCIES
  none

RETURN VALUE
  None.

SIDE EFFECTS
  None.
===========================================================================*/
PACKED void * wmsdiag_broadcast_add_delete_serviceid (
  PACKED void *req_pkt_ptr,  /* pointer to request packet  */
  uint16 pkt_len             /* length of request packet   */
)
{
  wmsdiag_broadcast_add_delete_serviceid_rsp_type *delay_rsp_pkt  = NULL;

  /* Always setting it to 1 */
  diagpkt_subsys_delayed_rsp_id_type delay_rsp_id = 1;

  /* Cast copy of request pointer to proper type */
  wmsdiag_broadcast_add_delete_serviceid_req_type *req_ptr  =
        (wmsdiag_broadcast_add_delete_serviceid_req_type *) req_pkt_ptr;

  wms_status_e_type st = WMS_OK_S;

  /* set to default value */
  wms_message_mode_e_type msg_mode = WMS_MESSAGE_MODE_MAX;

  static wms_bc_mm_service_info_s_type   service_info;

  static wms_bc_mm_srv_id_type           service_id;

  int i;

  /*  If no client ID has been established, do so now. */
  if (wmsdiag_client_id == WMSDIAG_INVALID_CLIENT_ID)
  {
    wmsdiag_client_init ();
  }

  /*  Allocate our packet */
  delay_rsp_pkt = (wmsdiag_broadcast_add_delete_serviceid_rsp_type*) diagpkt_subsys_alloc_v2_delay (
                             (diagpkt_subsys_id_type)DIAG_SUBSYS_WMS, 
                             (diagpkt_subsys_cmd_code_type)WMSDIAG_BROADCAST_ADD_DELETE_SERVICEID_CMD, 
                             delay_rsp_id,
                             sizeof(wmsdiag_broadcast_add_delete_serviceid_rsp_type) );

  if (delay_rsp_pkt != NULL)
  {
    /* Store the service id information depending on CDMA or GW mode */
    if (req_ptr->msg_mode == WMSDIAG_MSG_MODE_CDMA)
    {
      service_info.srv_id.bc_srv_id.service = (wms_service_e_type) req_ptr->service_category;
      service_info.srv_id.bc_srv_id.language = (wms_language_e_type) req_ptr->language;

      service_id.bc_srv_id.service =  (wms_service_e_type) req_ptr->service_category;
      service_id.bc_srv_id.language = (wms_language_e_type) req_ptr->language;

      msg_mode = WMS_MESSAGE_MODE_CDMA;
    }
    else if (req_ptr->msg_mode == WMSDIAG_MSG_MODE_GW)
    {
      service_info.srv_id.gw_cb_range.from = req_ptr->serviceid_from;
      service_info.srv_id.gw_cb_range.to = req_ptr->serviceid_to;

      service_id.gw_cb_range.from = req_ptr->serviceid_from;
      service_id.gw_cb_range.to = req_ptr->serviceid_to;

      msg_mode = WMS_MESSAGE_MODE_GW;
    }

    /* add a new service id */
    if (req_ptr->add_or_delete_serviceid == 0)
    {
      service_info.alert = (wms_bc_alert_e_type) req_ptr->alert_option;
      service_info.priority = (wms_priority_e_type) req_ptr->priority;
      service_info.selected = req_ptr->is_selected;
      service_info.max_messages = req_ptr->max_messages;
      service_info.label_encoding = (wms_user_data_encoding_e_type) req_ptr->label_encoding;

      /* label */
      for (i = 0; i < WMS_BC_MM_SRV_LABEL_SIZE; i++)
      {
        service_info.label[i] = req_ptr->label[i];
      }

      st = wms_bc_mm_add_services(wmsdiag_client_id, wmsdiag_bc_add_delete_cmd_cb, (void *) req_ptr->transaction_ID,
                                    msg_mode, 1, &service_info);
    }
    /* delete an old service id */
    else if (req_ptr->add_or_delete_serviceid == 1)
    {
      st = wms_bc_mm_delete_services(wmsdiag_client_id, wmsdiag_bc_add_delete_cmd_cb, (void *) req_ptr->transaction_ID,
                                      msg_mode, 1, &service_id);
    }

    /* Immediate Response */
    diagpkt_subsys_set_rsp_cnt(delay_rsp_pkt, 0);

    /* Set the Status */
    if (st != WMS_OK_S)
    {
      diagpkt_subsys_set_status(delay_rsp_pkt, 1);
      MSG_ERROR("Add/Delete Broadcast Service ID failed",0,0,0);
    }
    else
    {
      diagpkt_subsys_set_status(delay_rsp_pkt, 0);
    }

    /* Populate Fields */
    delay_rsp_pkt->transaction_ID = req_ptr->transaction_ID;
    delay_rsp_pkt->reserved_byte1 = 0;
    delay_rsp_pkt->reserved_byte2 = 0;
    delay_rsp_pkt->reserved_byte3 = 0;
    
    /* Send the Response */
    diagpkt_delay_commit(delay_rsp_pkt);
  }
  else
  {
    MSG_ERROR("Response Packet Allocation Failed!",0,0,0);
  }

  return NULL;
}


#ifdef FEATURE_IWMSDIAG
/*===========================================================================
FUNCTION wmsappdiag_cb_save

DESCRIPTION
  Function to process the WMSAPPDIAG_CB_SAVE_CMD

DEPENDENCIES
  none

RETURN VALUE
  None.

SIDE EFFECTS
  None.
===========================================================================*/
PACKED void * wmsappdiag_cb_save (
  PACKED void *req_pkt_ptr,    /* pointer to request packet  */
  uint16 pkt_len               /* length of request packet   */
)
{
#ifdef FEATURE_GWSMS_BROADCAST
#error code not present
#endif /* FEATURE_GWSMS_BROADCAST */

  return NULL;
}

/*===========================================================================
FUNCTION wmsappdiag_cb_read

DESCRIPTION
  Function to process the WMSAPPDIAG_CB_READ_CMD

DEPENDENCIES
  none

RETURN VALUE
  None.

SIDE EFFECTS
  None.
===========================================================================*/
PACKED void * wmsappdiag_cb_read (
  PACKED void *req_pkt_ptr,    /* pointer to request packet  */
  uint16 pkt_len               /* length of request packet   */
)
{
#ifdef FEATURE_GWSMS_BROADCAST
#error code not present
#endif /* FEATURE_GWSMS_BROADCAST */

  return NULL;
}

#endif /* FEATURE_IWMSDIAG */

/*===========================================================================
FUNCTION wmsdiag_broadcast_delete_all_services

DESCRIPTION
  Function to process the WMSDIAG_BROADCAST_DELETE_ALL_SERVICES_CMD

DEPENDENCIES
  none

RETURN VALUE
  None.

SIDE EFFECTS
  None.
===========================================================================*/
PACKED void * wmsdiag_broadcast_delete_all_services (
  PACKED void *req_pkt_ptr,  /* pointer to request packet  */
  uint16 pkt_len             /* length of request packet   */
)
{
  wmsdiag_broadcast_delete_all_services_rsp_type *delay_rsp_pkt  = NULL;

  /* Always setting it to 1 */
  diagpkt_subsys_delayed_rsp_id_type delay_rsp_id = 1;

  /* Cast copy of request pointer to proper type */
  wmsdiag_broadcast_delete_all_services_req_type *req_ptr  =
        (wmsdiag_broadcast_delete_all_services_req_type *) req_pkt_ptr;

  wms_status_e_type st = WMS_OK_S;

  /* set to default value */
  wms_message_mode_e_type msg_mode = WMS_MESSAGE_MODE_MAX;

  /*  If no client ID has been established, do so now. */
  if (wmsdiag_client_id == WMSDIAG_INVALID_CLIENT_ID)
  {
    wmsdiag_client_init ();
  }

  /*  Allocate our packet */
  delay_rsp_pkt = (wmsdiag_broadcast_delete_all_services_rsp_type*) diagpkt_subsys_alloc_v2_delay (
                             (diagpkt_subsys_id_type)DIAG_SUBSYS_WMS, 
                             (diagpkt_subsys_cmd_code_type)WMSDIAG_BROADCAST_DELETE_ALL_SERVICES_CMD, 
                             delay_rsp_id,
                             sizeof(wmsdiag_broadcast_delete_all_services_rsp_type) );

  if (delay_rsp_pkt != NULL)
  {
    /* Store the service id information depending on CDMA or GW mode */
    if (req_ptr->msg_mode == WMSDIAG_MSG_MODE_CDMA)
    {
      msg_mode = WMS_MESSAGE_MODE_CDMA;
    }
    else if (req_ptr->msg_mode == WMSDIAG_MSG_MODE_GW)
    {
      msg_mode = WMS_MESSAGE_MODE_GW;
    }

    st = wms_bc_mm_delete_all_services( wmsdiag_client_id, wmsdiag_bc_delete_all_cmd_cb, 
                                        (void *)req_ptr->transaction_ID, msg_mode );
    
    /* Immediate Response */
    diagpkt_subsys_set_rsp_cnt(delay_rsp_pkt, 0);

    /* Set the Status */
    if (st != WMS_OK_S)
    {
      diagpkt_subsys_set_status(delay_rsp_pkt, 1);
      MSG_ERROR("Add/Delete Broadcast Service ID failed",0,0,0);
    }
    else
    {
      diagpkt_subsys_set_status(delay_rsp_pkt, 0);
    }

    /* Populate Fields */
    delay_rsp_pkt->transaction_ID = req_ptr->transaction_ID;
    delay_rsp_pkt->reserved_byte1 = 0;
    delay_rsp_pkt->reserved_byte2 = 0;
    delay_rsp_pkt->reserved_byte3 = 0;
    
    /* Send the Response */
    diagpkt_delay_commit(delay_rsp_pkt);
  }
  else
  {
    MSG_ERROR("Response Packet Allocation Failed!",0,0,0);
  }

  return NULL;
}

#ifdef FEATURE_IWMSDIAG
/*===========================================================================
FUNCTION wmsappdiag_cb_delete_msg

DESCRIPTION
  Function to process the WMSAPPDIAG_CB_DELETE_MSG

DEPENDENCIES
  none

RETURN VALUE
  None.

SIDE EFFECTS
  None.
===========================================================================*/
PACKED void * wmsappdiag_cb_delete_msg (
  PACKED void *req_pkt_ptr,  /* pointer to request packet  */
  uint16 pkt_len             /* length of request packet   */
)
{
#ifdef FEATURE_GWSMS_BROADCAST
#error code not present
#endif /* FEATURE_GWSMS_BROADCAST */

  return NULL;
}

/*===========================================================================
FUNCTION wmsappdiag_cb_delete_all_msgs

DESCRIPTION
  Function to process the WMSAPPDIAG_CB_DELETE_ALL_MSGS

DEPENDENCIES
  none

RETURN VALUE
  None.

SIDE EFFECTS
  None.
===========================================================================*/
PACKED void * wmsappdiag_cb_delete_all_msgs (
  PACKED void *req_pkt_ptr,  /* pointer to request packet  */
  uint16 pkt_len             /* length of request packet   */
)
{
#ifdef FEATURE_GWSMS_BROADCAST
#error code not present
#endif /* FEATURE_GWSMS_BROADCAST */

  return NULL;
}
#endif /* FEATURE_IWMSDIAG */

/*===========================================================================
FUNCTION wmsdiag_get_bearer_pref

DESCRIPTION
  Function to process the WMSDIAG_GET_BEARER_PREF_CMD

DEPENDENCIES
  none

RETURN VALUE
  None.

SIDE EFFECTS
  None.
===========================================================================*/
PACKED void * wmsdiag_get_bearer_pref (
  PACKED void *req_pkt_ptr,  /* pointer to request packet  */
  uint16 pkt_len             /* length of request packet   */
)
{
  wmsdiag_get_bearer_pref_rsp_type *delay_rsp_pkt  = NULL;

  /* Always setting it to 1 */
  diagpkt_subsys_delayed_rsp_id_type delay_rsp_id = 1;

  /* Cast copy of request pointer to proper type */
  wmsdiag_get_bearer_pref_req_type *req_ptr  =
        (wmsdiag_get_bearer_pref_req_type *) req_pkt_ptr;

  wms_status_e_type st = WMS_OK_S;

  /* Drop re-entrant function calls if a request is still pending */
  if (get_bearer_req_state != WMSDIAG_REQ_STATE_IDLE)
  {
    return (diagpkt_err_rsp (DIAG_BAD_PARM_F, req_pkt_ptr, pkt_len));
  }

  /*  If no client ID has been established, do so now. */
  if (wmsdiag_client_id == WMSDIAG_INVALID_CLIENT_ID)
  {
    wmsdiag_client_init ();
  }

  /*  Allocate our packet */
  delay_rsp_pkt = (wmsdiag_get_bearer_pref_rsp_type*) diagpkt_subsys_alloc_v2_delay (
                             (diagpkt_subsys_id_type)DIAG_SUBSYS_WMS, 
                             (diagpkt_subsys_cmd_code_type)WMSDIAG_GET_BEARER_PREF_CMD, 
                             delay_rsp_id,
                             sizeof(wmsdiag_get_bearer_pref_rsp_type) );

  if (delay_rsp_pkt != NULL)
  {
    get_bearer_req_state = WMSDIAG_REQ_STATE_COMMAND_CB;

    st = wms_cfg_get_gw_domain_pref( wmsdiag_client_id, wmsdiag_get_bearer_pref_cmd_cb, 
                                     (void *)req_ptr->transaction_ID );
    
    /* Immediate Response */
    diagpkt_subsys_set_rsp_cnt(delay_rsp_pkt, 0);

    /* Set the Status */
    if (st != WMS_OK_S)
    {
      diagpkt_subsys_set_status(delay_rsp_pkt, 1);
      MSG_ERROR("Get Bearer Pref failed",0,0,0);
    }
    else
    {
      diagpkt_subsys_set_status(delay_rsp_pkt, 0);
    }

    /* Populate Fields */
    delay_rsp_pkt->transaction_ID = req_ptr->transaction_ID;
    delay_rsp_pkt->sms_domain_pref = WMS_GW_DOMAIN_PREF_MAX32; /* Invalid */

    /* Send the Response */
    diagpkt_delay_commit(delay_rsp_pkt);
  }
  else
  {
    MSG_ERROR("Response Packet Allocation Failed!",0,0,0);
  }

  return NULL;
}

/*===========================================================================
FUNCTION wmsdiag_set_bearer_pref

DESCRIPTION
  Function to process the WMSDIAG_SET_BEARER_PREF_CMD

DEPENDENCIES
  none

RETURN VALUE
  None.

SIDE EFFECTS
  None.
===========================================================================*/
PACKED void * wmsdiag_set_bearer_pref (
  PACKED void *req_pkt_ptr,  /* pointer to request packet  */
  uint16 pkt_len             /* length of request packet   */
)
{
  wmsdiag_set_bearer_pref_rsp_type *delay_rsp_pkt  = NULL;

  /* Always setting it to 1 */
  diagpkt_subsys_delayed_rsp_id_type delay_rsp_id = 1;

  /* Cast copy of request pointer to proper type */
  wmsdiag_set_bearer_pref_req_type *req_ptr  =
        (wmsdiag_set_bearer_pref_req_type *) req_pkt_ptr;

  wms_status_e_type st = WMS_OK_S;

  if ( req_ptr->sms_domain_pref >= WMS_GW_DOMAIN_PREF_MAX)
  {
    return (diagpkt_err_rsp (DIAG_BAD_PARM_F, req_pkt_ptr, pkt_len));
  }

  /*  If no client ID has been established, do so now. */
  if (wmsdiag_client_id == WMSDIAG_INVALID_CLIENT_ID)
  {
    wmsdiag_client_init ();
  }

  /*  Allocate our packet */
  delay_rsp_pkt = (wmsdiag_set_bearer_pref_rsp_type*) diagpkt_subsys_alloc_v2_delay (
                             (diagpkt_subsys_id_type)DIAG_SUBSYS_WMS, 
                             (diagpkt_subsys_cmd_code_type)WMSDIAG_SET_BEARER_PREF_CMD, 
                             delay_rsp_id,
                             sizeof(wmsdiag_set_bearer_pref_rsp_type) );

  if (delay_rsp_pkt != NULL)
  {
    st = wms_cfg_set_gw_domain_pref( wmsdiag_client_id, wmsdiag_set_bearer_pref_cmd_cb, 
                                     (void *)req_ptr->transaction_ID, 
                                     (wms_gw_domain_pref_e_type)req_ptr->sms_domain_pref );
    
    /* Immediate Response */
    diagpkt_subsys_set_rsp_cnt(delay_rsp_pkt, 0);

    /* Set the Status */
    if (st != WMS_OK_S)
    {
      diagpkt_subsys_set_status(delay_rsp_pkt, 1);
      MSG_ERROR("Set Bearer Pref failed",0,0,0);
    }
    else
    {
      diagpkt_subsys_set_status(delay_rsp_pkt, 0);
    }

    /* Populate Fields */
    delay_rsp_pkt->transaction_ID = req_ptr->transaction_ID;

    /* Send the Response */
    diagpkt_delay_commit(delay_rsp_pkt);
  }
  else
  {
    MSG_ERROR("Response Packet Allocation Failed!",0,0,0);
  }

  return NULL;
}

/*  Map the diag functions to the proper commands */
static const diagpkt_user_table_entry_type wmsdiag_tbl[] =
{
  {WMSDIAG_SEND_WRITE_CMD, WMSDIAG_SEND_WRITE_CMD, wmsdiag_send_write},
  {WMSDIAG_CLEAR_CMD, WMSDIAG_CLEAR_CMD, wmsdiag_clear},
  {WMSDIAG_CLEAR_ALL_CMD, WMSDIAG_CLEAR_ALL_CMD, wmsdiag_clear_all},
  {WMSDIAG_GET_MESSAGE_LIST_CMD, WMSDIAG_GET_MESSAGE_LIST_CMD, wmsdiag_get_message_list},
  {WMSDIAG_READ_CMD, WMSDIAG_READ_CMD, wmsdiag_read},
  {WMSDIAG_SET_MESSAGE_TAG_CMD, WMSDIAG_SET_MESSAGE_TAG_CMD, wmsdiag_set_message_tag},
  {WMSDIAG_SET_ROUTES_CMD, WMSDIAG_SET_ROUTES_CMD, wmsdiag_set_routes},
  {WMSDIAG_ORIG_END_SMS_CALL_CMD, WMSDIAG_ORIG_END_SMS_CALL_CMD, wmsdiag_orig_end_sms_call},
  {WMSDIAG_AUTO_DISCONNECT_CMD, WMSDIAG_AUTO_DISCONNECT_CMD, wmsdiag_enable_disable_auto_disconnect},
};

static const diagpkt_user_table_entry_type wmsdiag_tbl_delay_rsp[] =
{
  {WMSDIAG_BROADCAST_ADD_DELETE_SERVICEID_CMD, WMSDIAG_BROADCAST_ADD_DELETE_SERVICEID_CMD, wmsdiag_broadcast_add_delete_serviceid},
  {WMSDIAG_BROADCAST_DELETE_ALL_SERVICES_CMD, WMSDIAG_BROADCAST_DELETE_ALL_SERVICES_CMD, wmsdiag_broadcast_delete_all_services},
  {WMSDIAG_GET_BEARER_PREF_CMD, WMSDIAG_GET_BEARER_PREF_CMD, wmsdiag_get_bearer_pref},
  {WMSDIAG_SET_BEARER_PREF_CMD, WMSDIAG_SET_BEARER_PREF_CMD, wmsdiag_set_bearer_pref},
};


#ifdef FEATURE_IWMSDIAG

/*  Map the diag functions to the proper commands */
static const diagpkt_user_table_entry_type wmsappdiag_tbl_delay_rsp[] =
{
  {WMSAPPDIAG_CB_SAVE_CMD, WMSAPPDIAG_CB_SAVE_CMD, wmsappdiag_cb_save},
  {WMSAPPDIAG_CB_READ_CMD, WMSAPPDIAG_CB_READ_CMD, wmsappdiag_cb_read},
  {WMSAPPDIAG_CB_DELETE_MSG_CMD, WMSAPPDIAG_CB_DELETE_MSG_CMD, wmsappdiag_cb_delete_msg},
  {WMSAPPDIAG_CB_DELETE_ALL_MSGS_CMD, WMSAPPDIAG_CB_DELETE_ALL_MSGS_CMD, wmsappdiag_cb_delete_all_msgs},
};

#endif /* FEATURE_IWMSDIAG */

/*===========================================================================
FUNCTION wmsdiag_init

DESCRIPTION
  Initialize the WMS Diag Module

DEPENDENCIES
  none

RETURN VALUE
  None.

SIDE EFFECTS
  None.
===========================================================================*/
void wmsdiag_init (void)
{
  DIAGPKT_DISPATCH_TABLE_REGISTER (DIAG_SUBSYS_WMS, wmsdiag_tbl);
  DIAGPKT_DISPATCH_TABLE_REGISTER_V2_DELAY(DIAG_SUBSYS_CMD_VER_2_F, DIAG_SUBSYS_WMS, wmsdiag_tbl_delay_rsp);
#ifdef FEATURE_IWMSDIAG
  DIAGPKT_DISPATCH_TABLE_REGISTER_V2_DELAY(DIAG_SUBSYS_CMD_VER_2_F, DIAG_SUBSYS_WMS, wmsappdiag_tbl_delay_rsp);
#endif /* FEATURE_IWMSDIAG */
}

#endif /* if defined(FEATURE_CDSMS) || defined(FEATURE_GWSMS) */

#endif /* FEATURE_WMS_DIAG */
