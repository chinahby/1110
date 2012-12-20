/*====*====*====*====*====*====*====*====*====*====*====*====*====*====*====*

                              P S _P P P _ A U T H . C

GENERAL DESCRIPTION
  All of the internal PPP protocol suite functionality.

Copyright (c) 1995-2010 QUALCOMM Incorporated. 
All Rights Reserved.
Qualcomm Confidential and Proprietary.
*====*====*====*====*====*====*====*====*====*====*====*====*====*====*====*/


/*===========================================================================

                        EDIT HISTORY FOR MODULE

  $PVCSPath: MM_DATA/ps_ppp_auth.c_v   1.6   21 Feb 2003 20:42:56 jeffd  $
  $Header: //source/qcom/qct/modem/data/cdps/ps/main/lite/src/ps_ppp_auth.c#7 $
  $Author: nsivakum $ $DateTime: 2010/04/20 23:13:35 $

when        who    what, where, why
--------    ---    ----------------------------------------------------------
04/19/10    sn     Merged support for PAP internal auth.
04/29/09    sn     Ported support for call throttle feature (DCTM).
02/05/09    kk     Added a NULL pointer check in 
                   chapi_calc_chap_md5_ruim_response().
11/23/08    scb    Added fix for OMH package to read correct nai_entry_index
06/17/08    pp     Fixed RVCT compiler warnings.
06/09/07    scb    Added FEATURE_UIM_SUPPORT_3GPD authentication support.
21/01/04    aku    Moved some field length defines from .c to .h
05/06/03    usb    For JCDMA check for UM_SN_DEV instead of UM_AN_DEV when
                   auth fails.
04/09/03    mvl    Fixed a bug in PAP authentication, added support for not
                   moving to the network phase if auth info not received.
04/08/03    mvl    Fixed bug for saving chap information.
04/06/03    mvl    Addition of capabilities to support server style
                   authentication for both PAP and CHAP. 
04/02/03    mvl    Complete modularization of the authentication code.
03/26/03    mvl    mainlined the CHAP and PAP features.
03/17/03    js     Added the prefix PS_PPP_EVENT_ to ppp_event_state_type.
03/07/03    js     Added CHAP/PAP events reporting. 
02/21/03    jd     moved & renamed ppp_auth_get_from_nv() from this file to
                   to is707_get_ppp_auth_info_from_nv() in ps707_dssnet.c
02/06/03    jd     Renamed ppp_auth_get_info() to ppp_auth_get_from_nv()
01/31/03    jd     Added ppp_auth_get_info() to read PPP authentication info
                   into the provided ppp_config 
01/27/03    jd     Merged ppp_send_chap_pkt() optimizations from common tip
11/04/02    mvl    Added code to increment the PAP id field.
09/06/02    mvl    fixed merge issue
08/20/02     vr    Free chap_challenge_ptr if NULL username/password
08/02/02    mvl    moved authentication declarations here.
===========================================================================*/


/*===========================================================================

                     INCLUDE FILES FOR MODULE

===========================================================================*/
#include "customer.h"
#include "comdef.h"

#ifdef FEATURE_DATA_PS
#include "dsbyte.h"
#include "md5.h"
#include "ps_ppp_auth.h"
#include "ps_ppp_defs.h"
#include "ps_ppp_events.h"
#include "ps_ppp_ipcp.h"
#include "ps_pppi.h"
#include "pstimer.h"
#include "ran.h"

#if defined(FEATURE_UIM_SUPPORT_3GPD)
#include "uim.h"
#include "psi.h"
#include "ps_utils.h"
#include "nvruimi.h"
#include "auth.h"
#include "mccdma.h"
#include "uimcdma.h"
#include "bit.h"
#endif /* FEATURE_UIM_SUPPORT_3GPD */

/*===========================================================================

                LOCAL DEFINITIONS AND DECLARATIONS FOR MODULE

===========================================================================*/
#if defined(FEATURE_UIM_SUPPORT_3GPD)
/* The following structure is used for authentication operations with the R-UIM */
LOCAL struct
{
  /* CHAP challenge response*/
  byte              challenge_response[UIM_COMPUTE_IP_AUTH_RESPONSE_LENGTH];
  uim_rpt_status    report_status; /* Status of a UIM Command Status */
} ppp_uim_auth;
#endif /* FEATURE_UIM_SUPPORT_3GPD */
/*=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=

                            FORWARD DECLARATIONS

=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=*/
static void authi_send_success
(
  ppp_type *ppp_cb_ptr,
  uint8 code
);

static void papi_send_auth_req
(
  ppp_type *ppp_cb_ptr
);

static boolean papi_save_auth_info
(
  ppp_type       *ppp_cb_ptr,
  dsm_item_type **pap_req_ptr
);

static boolean chapi_save_auth_info
(
  ppp_type       *ppp_cb_ptr,
  dsm_item_type **chap_resp_ptr,
  uint16          length
);

static void chapi_send_resp
(
  ppp_type *ppp_cb_ptr
);

static void chapi_send_challenge
(
  ppp_type *ppp_cb_ptr
);


/*=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=

                        PUBLIC FUNCTION DECLARATIONS

  -------------------------------------------------------------------------

                              GENERAL FUNCTIONS

=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=*/
/*===========================================================================
FUNCTION PPP_AUTH_START()

DESCRIPTION
  This function checks for authentication and kicks it off if it was
  negotiated, otherwise it starts IPCP.

PARAMETERS
  device: the device on which auth is being started.

RETURN VALUE
  None

DEPENDENCIES
  None

SIDE EFFECTS
  None
===========================================================================*/
void ppp_auth_start
(
  ppp_dev_enum_type device
)
{
  ppp_event_payload_type    ppp_event_payload;
  ppp_type *ppp_cb_ptr = ppp_cb_array + device;
/*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/
  /*-------------------------------------------------------------------------
    Now in auth phase: Initialize the retry counter and the active ID.
  -------------------------------------------------------------------------*/
  ppp_cb_ptr->phase = pppAUTHENTICATE;
  ppp_cb_ptr->auth.retry_counter = PPP_AUTH_RETRY_COUNTER_VAL;
  ppp_cb_ptr->auth.active_id = 0;

  /*-------------------------------------------------------------------------
    Are we supposed to authenticate?
  -------------------------------------------------------------------------*/
  MSG_FATAL("ppp_auth_start %d %d %d",ppp_cb_ptr->auth.mode,device,uim_3gpd_support());
  switch(ppp_cb_ptr->auth.mode)
  {
  case PPP_DO_CHAP:
  {
    ppp_event_payload.ppp_event_protocol = PPP_EV_CHAP;

    /*-----------------------------------------------------------------------
      Are we the authenticatee or the authenticator?  If we are the
      authenticator send the challenge - otherwise wait for it.
    -----------------------------------------------------------------------*/
    if(ppp_cb_ptr->auth.flags & PPP_AP_REMOTE)
    {
      ppp_cb_ptr->auth.retry_proc = chapi_send_challenge;
      chapi_send_challenge(ppp_cb_ptr);
    }
    else
    {
      ppp_cb_ptr->auth.retry_proc = chapi_send_resp;
    }
  }
  break;  

  case PPP_DO_PAP:
  {
    ppp_event_payload.ppp_event_protocol = PPP_EV_PAP;

    /*-----------------------------------------------------------------------
      Are we the authenticatee or the authenticator?  If we are the
      authenticator we don't send anything, but wait for a pkt from our peer.
    -----------------------------------------------------------------------*/
    if(ppp_cb_ptr->auth.flags & PPP_AP_LOCAL)
    {
      ppp_cb_ptr->auth.retry_proc = papi_send_auth_req;
      papi_send_auth_req(ppp_cb_ptr);
    }
    else
    {
      ppp_cb_ptr->auth.retry_proc = NULL;
    }
  }
  break;

  default:
    /*-----------------------------------------------------------------------
      NOTE: this is intentionally not last as we want to start IPCP when
      assert()s are disabled.
    -----------------------------------------------------------------------*/
    ASSERT(0);
    /* fall through */

  case PPP_NO_AUTH:
  {
    ppp_event_payload.ppp_event_protocol = PPP_EV_INVALID_PROTO;
    /*-----------------------------------------------------------------------
      there was no authentication required and so is complete - clear flags
      so it doesn't look like it failed.
    -----------------------------------------------------------------------*/
    ppp_cb_ptr->auth.flags = 0;
    ppp_auth_complete(ppp_cb_ptr, PPP_NO_PROTOCOL);
  }
  break;

  } /* switch(auth mode) */

  if(ppp_event_payload.ppp_event_protocol != PPP_EV_INVALID_PROTO)
  {
    ppp_event_payload.ppp_event_device = ppp_cb_ptr->fsm[LCP].device;
    ppp_event_payload.ppp_event_prev_state = PS_PPP_EVENT_UNKNOWN;
    ppp_event_payload.ppp_event_state = PS_PPP_EVENT_STARTING;
    event_report_payload(EVENT_PPP,
                         sizeof(ppp_event_payload),
                         &ppp_event_payload);
  }

} /* ppp_auth_start() */



/*===========================================================================
FUNCTION PPP_AUTH_COMPLETE()

DESCRIPTION
  This function completes the PPP authentication phase which includes
  cleaning up of any data that was stored.  If authentication was
  sucessful it starts the PPP network phase.  Note that if the protocol
  passed in is PPP_NO_PROTOCOL this indicates that no authentication is to be
  attemted and that is why authentication is complete.

PARAMETERS
  ppp_cb_ptr: ppp control block pointer for completed authentication
  protocol:   protocol which completed (for protocol specific cleanup).  

RETURN VALUE
  None

DEPENDENCIES
  None

SIDE EFFECTS
  None
===========================================================================*/
void ppp_auth_complete
(
  ppp_type *ppp_cb_ptr,
  uint16    protocol
)
{
  ppp_event_payload_type ppp_event_payload;
/*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  /*-------------------------------------------------------------------------
    Authentication complete - cancel the auth timer.
  -------------------------------------------------------------------------*/
  ps_timer_cancel(ppp_cb_ptr->fsm[LCP].timer);

  /*-------------------------------------------------------------------------
    do protocol specific handling - free data and specify protocol for event
  -------------------------------------------------------------------------*/
  switch(protocol)
  {
  case PPP_CHAP_PROTOCOL:
    /*-----------------------------------------------------------------------
      Free the packet in chap_challenge_ptr.
    -----------------------------------------------------------------------*/
    dsm_free_packet(&(ppp_cb_ptr->auth.chap_challenge_ptr));
    ppp_event_payload.ppp_event_protocol = PPP_EV_CHAP;
    break;
  
  case PPP_PAP_PROTOCOL:
    ppp_event_payload.ppp_event_protocol = PPP_EV_PAP;
    break;
  
  case PPP_NO_PROTOCOL:
    ppp_event_payload.ppp_event_protocol = PPP_EV_INVALID_PROTO;
    break;

  default:
    ASSERT(0);
    MSG_ERROR("Invalid protocol auth %d!", protocol, 0, 0);
    return;
  } /* switch(protocol) */

  /*-------------------------------------------------------------------------
    Fill in the fileds of ppp_event_payload and post the CHAP open event. 
  -------------------------------------------------------------------------*/
  if(ppp_event_payload.ppp_event_protocol != PPP_EV_INVALID_PROTO)
  {
    ppp_event_payload.ppp_event_device = ppp_cb_ptr->fsm[LCP].device;
    ppp_event_payload.ppp_event_prev_state = PS_PPP_EVENT_UNKNOWN;
    if(ppp_cb_ptr->auth.flags & PPP_AP_FAILED)
    {
      ppp_event_payload.ppp_event_state = PS_PPP_EVENT_CLOSED;
    }
    else
    {
      ppp_event_payload.ppp_event_state = PS_PPP_EVENT_OPEN;
    }
    event_report_payload(EVENT_PPP, 
                         sizeof(ppp_event_payload),
                         &ppp_event_payload);
  }

  /*-------------------------------------------------------------------------
    If auth was successful tell ipcp to start.
  -------------------------------------------------------------------------*/
  if(!(ppp_cb_ptr->auth.flags & PPP_AP_FAILED))
  {
    ipcp_start(ppp_cb_ptr);
  }
  else
  {
    /*-------------------------------------------------------------------------
      PPP auth failed. Hence post PPP failure command for DS to process it.
    -------------------------------------------------------------------------*/
    MSG_MED("PPP auth failed", 0, 0, 0);

    TASKLOCK();
    if(ppp_cb_ptr->event_info[PPP_FAILURE_EV].cback != NULL)
    {
      ppp_cb_ptr->event_info[PPP_FAILURE_EV].cback(
        ppp_cb_ptr->fsm[LCP].device,
        PPP_PROTOCOL_AUTH,
        PPP_FAILURE_EV,
        ppp_cb_ptr->event_info[PPP_FAILURE_EV].user_data,
        ppp_cb_ptr->session_handle,
        PPP_FAIL_REASON_AUTH_FAILURE);
    }
    TASKFREE();
  }
} /* ppp_auth_complete() */



/*===========================================================================
FUNCTION PPP_AUTH_TIMEOUT()

DESCRIPTION
  This function is the equivalent of fsm_timeout, but since the auth code
  doesn't use the FSM it has its own function.  It is called when an auth
  timeout occurs.  If there are still retries left, then it retries send the
  appropriate auth packet.  Otherwise if there are no requests left, then it
  closes the PPP link.

PARAMETERS
  ppp_cb_ptr: ppp control block pointer for timeout.

RETURN VALUE
  TRUE:  Informs the calling function to retry
  FALSE: Informs calling function, retries are over and to
         terminate the call.

DEPENDENCIES
  Same as dependencies of the functions ppp_chap_timeout

SIDE EFFECTS
  If number of the retries are non-zero then an authentication packet is
  transmitted.
===========================================================================*/
boolean ppp_auth_timeout
(
  ppp_type *ppp_cb_ptr
)
{
  ppp_dev_enum_type device;
  /* - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - */

  device = ppp_cb_ptr->fsm[LCP].device;
  /*-------------------------------------------------------------------------
    print a message indicating which protocol timed out - this should
    completely dissapear if MSG_MED is undefined.
  -------------------------------------------------------------------------*/
   switch(ppp_cb_ptr->auth.mode)
   {
   case PPP_DO_PAP:
     MSG_MED("dev %d PAP timeout occured", device, 0, 0);
     break;

   case PPP_DO_CHAP:
     MSG_MED("dev %d CHAP timeout occured", device, 0, 0);
     break;

   default:
     MSG_MED("dev %d Auth timeout with no auth!", device, 0, 0);
     return FALSE;
   } /* switch(auth type) */

  /*-------------------------------------------------------------------------
    make sure that there are still retries available - if not call the
    function for the retry (registered in auth_start()).
  -------------------------------------------------------------------------*/
  if((ppp_cb_ptr->auth.retry_counter--) != 0)
  {
    ppp_cb_ptr->auth.retry_proc(ppp_cb_ptr);
    return TRUE;
  }

  /*-------------------------------------------------------------------------
    If no more retries, then terminate the PPP connection by closing the link
    and cancelling the timer.
  -------------------------------------------------------------------------*/
  else
  {
    MSG_HIGH( "dev %d auth exhausted reties. Closing PPP", device, 0, 0);
    ppp_cb_ptr->auth.flags |= PPP_AP_FAILED;
    ppp_auth_complete(ppp_cb_ptr, PPP_CHAP_PROTOCOL);

    /*----------------------------------------------------------------------
      Close the LCP and this will initiate the link Termination
    ----------------------------------------------------------------------*/
    ppp_abort(device);

    return FALSE;
  }
} /* ppp_auth_timeout() */



/*=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=

                        PROTOCOL PROCESSING FUNCTIONS

=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=*/
/*===========================================================================
FUNCTION CHAP_PROC()

DESCRIPTION
  This function does the protocol processing for CHAP.

PARAMETERS
  device:       the PPP interface the packet arrived on
  item_ref_ptr: reference to pointer to dsm chain containing the CHAP pkt

RETURN VALUE
  None

DEPENDENCIES
  None

SIDE EFFECTS
  None
===========================================================================*/
void chap_proc
(
  ppp_dev_enum_type   device,
  dsm_item_type     **item_ref_ptr
)
{
  ppp_config_hdr_type auth_hdr;
  ppp_type *ppp_cb_ptr;
  uint8 challenge_len;
/*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/
  if(device >= PPP_MAX_DEV)
  {
    ASSERT(0);
    MSG_ERROR("cannot process CHAP for device %d!", device, 0, 0);
    return;
  }

  ppp_cb_ptr = ppp_cb_array + device;

  /*-------------------------------------------------------------------------
    The rxed message format looks like:
      Code: 1 Octet (the message type)
      Identifier: 1 Octet
      Length: 2 Octets
  
    *item_ref_ptr will be pointing to the code field so starting here extract
    the first three fields.
  -------------------------------------------------------------------------*/
  auth_hdr.code       = dsm_pull8(item_ref_ptr);
  auth_hdr.identifier = dsm_pull8(item_ref_ptr);
  auth_hdr.length     = dsm_pull16(item_ref_ptr);

  /*-------------------------------------------------------------------------
    do procession based on the message code.
  -------------------------------------------------------------------------*/
  switch(auth_hdr.code)
  {
  case PPP_CHAP_CHALLENGE:
  {
    /*-----------------------------------------------------------------------
      Recieved Challenge. Process the Challenge and genarate the Response.
    -----------------------------------------------------------------------*/
    MSG_MED("dev %d recieved CHAP challenge", device, 0, 0);

    /*-----------------------------------------------------------------------
      store the id of this packet as the active on:  for use in retries.
    -----------------------------------------------------------------------*/
    ppp_cb_ptr->auth.active_id = auth_hdr.identifier;

    /*-----------------------------------------------------------------------
      Free the packet in chap_challenge_ptr if it already exists
    -----------------------------------------------------------------------*/
    if(ppp_cb_ptr->auth.chap_challenge_ptr != NULL)
    {
      dsm_free_packet(&(ppp_cb_ptr->auth.chap_challenge_ptr));
    }

    /*-----------------------------------------------------------------------
      Extract the length of the challenge from the packet.  Obtain an
      dsm_buffer for the chap_challenge_ptr.  This buffer will be freed after
      receiving SUCCESS, FAILURE or all the time outs occur.  Store in the
      used field of the chap_challenge_ptr.
    -----------------------------------------------------------------------*/
    challenge_len = dsm_pull8(item_ref_ptr);
    MSG_LOW("dev %d CHAP chal_len = %d", device, challenge_len, 0);

    ppp_cb_ptr->auth.chap_challenge_ptr =
      dsm_offset_new_buffer(DSM_DS_POOL_SIZE(challenge_len + PPP_HDR_LEN),
                            PPP_HDR_LEN);
    if(ppp_cb_ptr->auth.chap_challenge_ptr == NULL)
    {
      /*---------------------------------------------------------------------
        give up at this point - but don't bail - when we get another
        challenge there may be more memory.
      ---------------------------------------------------------------------*/
      MSG_HIGH("dev %d chap_proc() alloc failed",device, 0, 0);
      break;
    }

    ppp_cb_ptr->auth.chap_challenge_ptr->used = challenge_len;
    ASSERT(ppp_cb_ptr->auth.chap_challenge_ptr->used <=
           ppp_cb_ptr->auth.chap_challenge_ptr->size);

    /*-----------------------------------------------------------------------
      Copy the challenge from incoming packet into the area pointed by
      chap_challenge_ptr. The data can be linked through several dsm_items so
      only dsm_extract() should be used.  No offset is needed as the data
      pointer is now pointing to the challenge.
    -----------------------------------------------------------------------*/
    dsm_extract(*item_ref_ptr,
                0,
                ppp_cb_ptr->auth.chap_challenge_ptr->data_ptr,
                ppp_cb_ptr->auth.chap_challenge_ptr->used);

    /*-----------------------------------------------------------------------
      Transmit the CHAP packet after building the response.
    -----------------------------------------------------------------------*/
    chapi_send_resp(ppp_cb_ptr);
  } /* CHAP_CHALLENGE */
  break;

  case PPP_CHAP_RESPONSE:
  {
    /*-----------------------------------------------------------------------
      Got response - make sure that we are trying to authenticate the
      peer - if not free packet and bail out.
    -----------------------------------------------------------------------*/
    if(!(ppp_cb_ptr->auth.flags & PPP_AP_REMOTE))
    {
      MSG_HIGH("dev %d got CHAP response when not authenticating peer!",
               device, 0, 0);
      break;
    }

    /*-----------------------------------------------------------------------
      make sure that the id in the header is the current active one.
    -----------------------------------------------------------------------*/
    else if(auth_hdr.identifier != ppp_cb_ptr->auth.active_id)
    {
      MSG_MED("dev %d CHAP response has wrong ID %d != %d - dicarding",
              device,
              auth_hdr.identifier,
              ppp_cb_ptr->auth.active_id);
      break;
    }

    /*-----------------------------------------------------------------------
      Save the authentication information and send the success message.
      NOTE: it is not necessary to stop the retry timer as ps_timers run in
      PS context and so cannot interrupt us.
    -----------------------------------------------------------------------*/
    else
    {
      if(chapi_save_auth_info(ppp_cb_ptr,
                              item_ref_ptr,
                              auth_hdr.length-PPP_HDR_LEN))
      {
        authi_send_success(ppp_cb_ptr, PPP_CHAP_SUCCESS);
      }
      /*---------------------------------------------------------------------
        call auth_complete() - if success is lost, peer should send another
        response with old packet information, which will contain the same
        information - also since chap can happen whenever it doesn't matter
        if we are in the IPCP stage.
      ---------------------------------------------------------------------*/
      ppp_auth_complete(ppp_cb_ptr, PPP_CHAP_PROTOCOL);
    }
  } /* CHAP_RESPONSE */
  break;

  case PPP_CHAP_SUCCESS:
  {
    /*-----------------------------------------------------------------------
      Got CHAP Success packet - authentication is complete.
    -----------------------------------------------------------------------*/
    MSG_MED( "dev %d got CHAP Success", device, 0, 0);
    ppp_auth_complete(ppp_cb_ptr, PPP_CHAP_PROTOCOL);

#ifdef FEATURE_HDR_AN_AUTH
#error code not present
#endif /* FEATURE_HDR_AN_AUTH */
  } /* CHAP_SUCCESS */
  break;

  case PPP_CHAP_FAILURE:
  {
    /*-----------------------------------------------------------------------
      Got CHAP Failure packet - authentication is complete, but failed.
    -----------------------------------------------------------------------*/
    MSG_HIGH( "dev %d got CHAP Failure.", device, 0, 0);
    ppp_cb_ptr->auth.flags |= PPP_AP_FAILED;
    ppp_auth_complete(ppp_cb_ptr, PPP_CHAP_PROTOCOL);

#ifdef FEATURE_HDR_AN_AUTH
#error code not present
#endif /* FEATURE_HDR_AN_AUTH */

#ifdef FEATURE_JCDMA_1X
    /*-----------------------------------------------------------------------
      When authentication fails on UM SN device, JCDMA requires that the 
      mobile actively terminates the call.
    -----------------------------------------------------------------------*/
    if(device == PPP_UM_SN_DEV)
    {
      ppp_stop(ppp_cb_ptr->fsm[LCP].device);
    }
#endif /* FEATURE_JCDMA_1X */
  } /* CHAP_FAILURE */
  break;

  default:
    MSG_MED ("dev %d got unknown CHAP msg %d.", device, auth_hdr.code, 0);

  } /* switch(chap msg type) */

  /*-------------------------------------------------------------------------
    As the processing is complete - free the incoming packet.
  -------------------------------------------------------------------------*/
  dsm_free_packet(item_ref_ptr);

} /* chap_proc() */



/*===========================================================================
FUNCTION PAP_PROC()

DESCRIPTION
  This function does the protocol processing for PAP.

PARAMETERS
  device:       the PPP interface the packet arrived on
  item_ref_ptr: reference to pointer to dsm chain containing the PAP pkt.

RETURN VALUE
  None

DEPENDENCIES
  None

SIDE EFFECTS
  None
===========================================================================*/
void pap_proc
(
  ppp_dev_enum_type   device,
  dsm_item_type     **item_ref_ptr
)
{
  ppp_config_hdr_type auth_hdr;
  ppp_type     *ppp_cb_ptr;
/*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/
  if(device >= PPP_MAX_DEV)
  {
    ASSERT(0);
    MSG_ERROR("cannot process PAP for device %d!", device, 0, 0);
    return;
  }

  ppp_cb_ptr = ppp_cb_array + device;

  /*-------------------------------------------------------------------------
    The rxed message format looks like:
      Code: 1 Octet (the message type)
      Identifier: 1 Octet
      Length: 2 Octets
  
    *item_ref_ptr will be pointing to the code field so starting here extract
    the first three fields.
  -------------------------------------------------------------------------*/
  auth_hdr.code   = dsm_pull8(item_ref_ptr);
  auth_hdr.identifier = dsm_pull8(item_ref_ptr);
  auth_hdr.length = dsm_pull16(item_ref_ptr);
  
  /*-------------------------------------------------------------------------
    do procession based on the message code.
  -------------------------------------------------------------------------*/
  switch(auth_hdr.code)
  {
  case PPP_PAP_REQ:
  {
    /*-----------------------------------------------------------------------
      got PAP request - make sure that we are trying to authenticate the
      peer.
    -----------------------------------------------------------------------*/
    if(!(ppp_cb_ptr->auth.flags & PPP_AP_REMOTE))
    {
      MSG_HIGH("dev %d got PAP Req when not authenticating peer!",
               device, 0, 0);
      break;
    }

    /*-----------------------------------------------------------------------
      Save the ID and auth information and send the success message if the
      same is successful.
      NOTE: it is not necessary to stop the retry timer as ps_timers run in
      PS context and so cannot interrupt us.
    -----------------------------------------------------------------------*/
    else
    {
      ppp_cb_ptr->auth.active_id = auth_hdr.identifier;
      if(papi_save_auth_info(ppp_cb_ptr, item_ref_ptr) == TRUE)
      {
        authi_send_success(ppp_cb_ptr, PPP_PAP_ACK);
      }
      /*---------------------------------------------------------------------
        don't call auth_complete() as will move to network phase, and peer
        may not receive success.  Since PAP is rejected in the network phase
        we need to stay in the authenticate phase until we received IPCP
        packet (which will move us to the network phase).
      ---------------------------------------------------------------------*/
    }
  } /* PPP_PAP_REQ */
  break;

  case PPP_PAP_NAK:
  {
    /*-----------------------------------------------------------------------
      Got a PAP Nak, auth is completed - but has failed.
    -----------------------------------------------------------------------*/
    MSG_MED("dev %d got PAP Nak.", device, 0, 0);
    ppp_cb_ptr->auth.flags |= PPP_AP_FAILED;
    ppp_auth_complete(ppp_cb_ptr, PPP_PAP_PROTOCOL);

#ifdef FEATURE_JCDMA_1X
    /*---------------------------------------------------------------------
      When authentication fails, JCDMA requires that the mobile actively
      terminate the call.
    ---------------------------------------------------------------------*/
    ppp_stop(ppp_cb_ptr->fsm[LCP].device);
#endif /* FEATURE_JCDMA_1X */
  } /* PAP_NAK */
  break;

  case PPP_PAP_ACK:
    /*-----------------------------------------------------------------------
      Recieved Auth-Ack. Authentication is complete.
    -----------------------------------------------------------------------*/
    ppp_auth_complete(ppp_cb_ptr, PPP_PAP_PROTOCOL);
    break;

  default:
    MSG_MED ("dev %d unknown PAP message %d.", device, auth_hdr.identifier, 0);

  } /* switch(pap msg type) */

  dsm_free_packet(item_ref_ptr);

} /* pap_proc() */

/*===========================================================================
FUNCTION PAP_INTERNAL_PROC()

DESCRIPTION
  This function does the protocol processing for PAP and reads the credentials 
  for auth from NV/RUIM.

PARAMETERS
  device:       the PPP interface the packet arrived on
  item_ref_ptr: reference to pointer to dsm chain containing the PAP pkt.

RETURN VALUE
  None

DEPENDENCIES
  None

SIDE EFFECTS
  None
===========================================================================*/
void pap_internal_proc
(
  ppp_dev_enum_type   device,
  dsm_item_type     **item_ref_ptr
)
{
  ppp_type *          ppp_cb_ptr;
  ppp_config_hdr_type auth_hdr;

  ppp_cb_ptr = ppp_cb_array + device;

  /*-------------------------------------------------------------------------
    The rxed message format looks like:
      Code: 1 Octet (the message type)
      Identifier: 1 Octet
      Length: 2 Octets
  
    *item_ref_ptr will be pointing to the code field so starting here extract
    the first three fields.
  -------------------------------------------------------------------------*/
  auth_hdr.code        = dsm_pull8(item_ref_ptr);
  auth_hdr.identifier  = dsm_pull8(item_ref_ptr);

  /*-----------------------------------------------------------------------
        store the id of this packet as the active on:  for use in retries.
  -----------------------------------------------------------------------*/
  ppp_cb_ptr->auth.active_id = auth_hdr.identifier;
  papi_send_auth_req(ppp_cb_ptr);
  
  /*-------------------------------------------------------------------------
    We are freeing the item here with the expectation that the Mobile is
    generating the response, so this one is meaningless.
  -------------------------------------------------------------------------*/
  dsm_free_packet(item_ref_ptr);

} /* pap_internal_proc() */

/*=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=

                             INTERNAL FUNCTIONS

=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=*/
/*===========================================================================
FUNCTION authi_send_success()

DESCRIPTION
  This function builds and transmitts the auth success message.  It uses the
  code field to differentiate between PAP and CHAP (as the messages are
  almost the same).

PARAMETERS
  ppp_cb_ptr: ppp_control block pointer
  code: the code to transmit.

RETURN VALUE
  None

DEPENDENCIES
  None

SIDE EFFECTS
  the PPP_AP_SUCCESS flag is added to ppp_cb_ptr->auth.flags
===========================================================================*/
static void authi_send_success
(
  ppp_type *ppp_cb_ptr,
  uint8     code
)
{
  dsm_item_type *auth_success;
  uint8 *insert_ptr;
  int success_len;
/*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/
  /*-------------------------------------------------------------------------
    determine the length of the packet - if the code indicates PAP then add
    one for the msg_length field - which we will set to 0.
  -------------------------------------------------------------------------*/
  success_len =  PPP_AUTH_CODE_SIZE
    + PPP_AUTH_ID_SIZE
    + PPP_AUTH_LENGTH_SIZE;

  if(code == PPP_PAP_ACK)
  {
    success_len++;
  }

  /*-------------------------------------------------------------------------
    Get an item that is big enough to fit the entire packet.  If it fails
    don't do anything special just return - the client presumably will
    retransmit response.
  -------------------------------------------------------------------------*/
  auth_success =
    dsm_offset_new_buffer(DSM_DS_POOL_SIZE(success_len + PPP_HDR_LEN),
                          PPP_HDR_LEN);
  if(auth_success == NULL)
  {
    MSG_ERROR("dev %d auth-ack mem alloc failed",
              ppp_cb_ptr->fsm[LCP].device, 0, 0);
    return;
  }

  /*-------------------------------------------------------------------------
    Fill in the following fields in the dsm item:
        used field with the total response length;
        Priority as Highest.
  -------------------------------------------------------------------------*/
  auth_success->used     = success_len;
  auth_success->priority = DSM_HIGHEST;
#ifdef FEATURE_DSM_MEM_CHK
  auth_success->tracer = DSM_PPP_TX_WM;
#endif

  /*-------------------------------------------------------------------------
    Both PAP and CHAP success messages look like:

    +-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
    |     Code      |  Identifier   |            Length             |
    +-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
    |  Msg-Length   |  Message  ...
    +-+-+-+-+-+-+-+-+-+-+-+-+-

    Note for CHAP msg-length doesn't exist and is part of the message field -
    the code field will allow us to omit this.
  -------------------------------------------------------------------------*/
  insert_ptr    = auth_success->data_ptr;
  *insert_ptr++ = code;
  *insert_ptr++ = ppp_cb_ptr->auth.active_id; /* this was stored on rx */
  insert_ptr    = put16(insert_ptr, success_len);

  /*-------------------------------------------------------------------------
    if this is a PAP auth-ack then set the msg-length to zero, in either case
    set the kind field to the correct protocol type.
  -------------------------------------------------------------------------*/
  if(code == PPP_PAP_ACK)
  {
    *insert_ptr++ = 0;
    auth_success->kind = DSM_PS_PAP_PROTOCOL;
  }
  else
  {
    auth_success->kind = DSM_PS_CHAP_PROTOCOL;
  }

  /*-------------------------------------------------------------------------
    we are done - so transmit the packet - if it fails we wait for the client
    to retrnsmit generating another success message.
  -------------------------------------------------------------------------*/
  if(pppi_outgoing(ppp_cb_ptr->fsm[LCP].device,
                   0,
                   &auth_success,
                   NULL) == -1)
  {
    MSG_HIGH("dev %d sending auth success failed!",
              ppp_cb_ptr->fsm[LCP].device, 0, 0);
  }
  else
  {
    ppp_cb_ptr->auth.flags |= PPP_AP_SUCCESS;
  }

} /* authi_send_success() */



/*===========================================================================
FUNCTION papi_send_auth_req()

DESCRIPTION
  Builds and transfers the PAP packet.

DEPENDENCIES
  None

RETURN VALUE
  None

SIDE EFFECTS
  None
===========================================================================*/
static void papi_send_auth_req
(
  ppp_type *ppp_cb_ptr
)
{
  dsm_item_type *auth_req;          /* Pointer for the PAP packet          */
  int pap_pkt_len;                  /* Length of the PAP Packet            */
  uint8 *insert_ptr;                /* Pointer to the data in the packet   */
/*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  /*-----------------------------------------------------------------------
    Since id-len and passwd-len do not include their own size, we add 2
  -----------------------------------------------------------------------*/
  pap_pkt_len = PPP_AUTH_CODE_SIZE
    + PPP_AUTH_ID_SIZE
    + PPP_AUTH_LENGTH_SIZE
    + ppp_cb_ptr->auth.info.user_id_len
    + ppp_cb_ptr->auth.info.passwd_len + 2;

  /*-------------------------------------------------------------------------
    Get an item that is big enough to fit the entire packet.  If the
    allocation fails skip forward to the timer - this will trigger another
    attempt.
  -------------------------------------------------------------------------*/
  auth_req =
    dsm_offset_new_buffer(DSM_DS_POOL_SIZE(pap_pkt_len + PPP_HDR_LEN),
                          PPP_HDR_LEN);
  if(auth_req == NULL)
  {
    MSG_ERROR("dev %d auth-req mem alloc failed",
              ppp_cb_ptr->fsm[LCP].device, 0, 0);
    goto give_up;
  }

  /*-------------------------------------------------------------------------
    Fill in the following fields in the dsm item:
        used field with the total response length;
        Kind with CHAP and
        Priority as Highest.
  -------------------------------------------------------------------------*/
  auth_req->used     = pap_pkt_len;
  auth_req->kind     = DSM_PS_PAP_PROTOCOL;
  auth_req->priority = DSM_HIGHEST;
#ifdef FEATURE_DSM_MEM_CHK
  auth_req->tracer = DSM_PPP_TX_WM;
#endif

  /*-----------------------------------------------------------------------
    An auth request looks like:

      +-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
      | 1 (the value) |  Identifier   |            Length             |
      +-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
      | Peer-ID Length|  Peer-Id ...
      +-+-+-+-+-+-+-+-+-+-+-+-+
      | Passwd-Length |  Password  ...
      +-+-+-+-+-+-+-+-+-+-+-+-+-+

    fill in the various fields.
  -----------------------------------------------------------------------*/
  insert_ptr    = auth_req->data_ptr;
  *insert_ptr++ = PPP_PAP_REQ;
  *insert_ptr++ = ++ppp_cb_ptr->auth.active_id;
  insert_ptr    = put16( insert_ptr, pap_pkt_len);

  /*-------------------------------------------------------------------------
    insert the peer id length and the id itself, as well as the password
    length and the password.
  -------------------------------------------------------------------------*/
  *insert_ptr++ = ppp_cb_ptr->auth.info.user_id_len;
  memcpy(insert_ptr,
         ppp_cb_ptr->auth.info.user_id_info,
         ppp_cb_ptr->auth.info.user_id_len);
  insert_ptr += ppp_cb_ptr->auth.info.user_id_len;
  *insert_ptr++ = ppp_cb_ptr->auth.info.passwd_len;
  memcpy(insert_ptr,
         ppp_cb_ptr->auth.info.passwd_info,
         ppp_cb_ptr->auth.info.passwd_len);

  /*-------------------------------------------------------------------------
    we are done - so transmit the packet
  -------------------------------------------------------------------------*/
  if(pppi_outgoing(ppp_cb_ptr->fsm[LCP].device,
                   PPP_PAP_PROTOCOL,
                   &auth_req,
                   NULL) == -1)
  {
    /*---------------------------------------------------------------------
      pppi_outgoing() frees the packet in case of an error - let the timer
      to trigger another attempt.
    ---------------------------------------------------------------------*/
    MSG_HIGH("dev %d sending PAP auth req failed!",
              ppp_cb_ptr->fsm[LCP].device, 0, 0);
  }

  /*-------------------------------------------------------------------------
    Start the retry timer.
    NOTE: we use the LCP timer as authentication doesn't have it's own timer.
  --------------------------------------------------------------------------*/
 give_up:
  ps_timer_start(ppp_cb_ptr->fsm[LCP].timer, PPP_AUTH_TIMER_VAL);

} /* papi_send_auth_req() */



/*===========================================================================
FUNCTION PAPI_SAVE_AUTH_INFO()

DESCRIPTION
  This function will parse the PAP packet and store the authentication
  information that was contained with in.  It assumes that the first byte in
  the item is the length.

PARAMETERS
  ppp_cb_ptr: pointer to the relevant PPP control block
  pap_req_ptr: pointer to the dsm_item containing the pap request.

RETURN VALUE
  TRUE  on success
  FALSE on failure

DEPENDENCIES
  None

SIDE EFFECTS
  None
===========================================================================*/
static boolean papi_save_auth_info
(
  ppp_type       *ppp_cb_ptr,
  dsm_item_type **pap_req_ref_ptr
)
{
  ppp_auth_info_type *auth_info_ptr;
  uint8 length;
/*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  auth_info_ptr = &(ppp_cb_ptr->auth.info);
  /*-------------------------------------------------------------------------
    A PAP auth request looks like:

      +-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
      |     Code      |  Identifier   |            Length             |
      +-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
      | Peer-ID Length|  Peer-Id ...
      +-+-+-+-+-+-+-+-+-+-+-+-+
      | Passwd-Length |  Password  ...
      +-+-+-+-+-+-+-+-+-+-+-+-+-+

    The code, ID and length have been stripped off at this point so we need
    to extract and copy the remaining fields.
    NOTE: if the passed in username or password is bigger than we can handle
    we return an error.
  -------------------------------------------------------------------------*/
  length = dsm_pull8(pap_req_ref_ptr);
  if(PPP_MAX_USER_ID_LEN < length)
  {
    return FALSE;
  }
  if(dsm_pullup(pap_req_ref_ptr,
                auth_info_ptr->user_id_info,
                length) != length)
  {
    return FALSE;
  }
  auth_info_ptr->user_id_len = length;

  length = dsm_pull8(pap_req_ref_ptr);
  if(PPP_MAX_PASSWD_LEN < length)
  {
    return FALSE;
  }
  if(dsm_pullup(pap_req_ref_ptr,
                auth_info_ptr->passwd_info,
                length) != length)
  {
    return FALSE;
  }
  auth_info_ptr->passwd_len = length;

  return TRUE;
} /* papi_save_auth_info() */



/*===========================================================================
FUNCTION CHAPI_SAVE_AUTH_INFO()

DESCRIPTION
  This function will parse the CHAP packet and store the authentication
  information that was contained with in.  It assumes that the first byte in
  the item is the length.

PARAMETERS
  ppp_cb_ptr: pointer to the relevant PPP control block
  chap_resp_ptr: pointer to the dsm_item containing the chap request.
  length: the total length of the chap packet - needed to get the name.

RETURN VALUE
  TRUE  on success
  FALSE on failure

DEPENDENCIES
  None

SIDE EFFECTS
  None
===========================================================================*/
static boolean chapi_save_auth_info
(
  ppp_type       *ppp_cb_ptr,
  dsm_item_type **chap_resp_ref_ptr,
  uint16          length
)
{
  ppp_auth_info_type *auth_info_ptr;
  uint8 hash_len;
/*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  auth_info_ptr = &(ppp_cb_ptr->auth.info);
  /*-------------------------------------------------------------------------
    A CHAP response looks like:

      +-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
      |     Code      |  Identifier   |            Length             |
      +-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
      |  Hash length  |  Hash
      +-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
      |  Client name ...
      +-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+

    The code, ID and length have been stripped off at this point so we need
    to extract and copy the remaining fields.  The hash length and hash are
    stored in the password section.
  -------------------------------------------------------------------------*/
  hash_len = dsm_pull8(chap_resp_ref_ptr);
  if(PPP_MAX_PASSWD_LEN < hash_len)
  {
    return FALSE;
  }
  if(dsm_pullup(chap_resp_ref_ptr,
                auth_info_ptr->passwd_info,
                hash_len) != hash_len)
  {
    return FALSE;
  }
  auth_info_ptr->passwd_len = hash_len;

  /*-------------------------------------------------------------------------
    calculate the length of the client name field (i.e. the user ID).
  -------------------------------------------------------------------------*/
  length -= PPP_CHAP_VALUE_SIZE + hash_len;
  if(PPP_MAX_USER_ID_LEN < length)
  {
    return FALSE;
  }
  if(dsm_pullup(chap_resp_ref_ptr,
                auth_info_ptr->user_id_info,
                length) != length)
  {
    return FALSE;
  }
  auth_info_ptr->user_id_len = length;

  return TRUE;

} /* chapi_save_auth_info() */


#if defined(FEATURE_UIM_SUPPORT_3GPD)
/*===========================================================================
FUNCTION CHAPI_UIM_CALC_CHAP_RESPONSE_REPORT()

DESCRIPTION
  Retrieves the 16-byte Hash/CHAP Response from the uim report, and copies
  into variables local to the file.  This is a callback function which
  is called from the uim context.

DEPENDENCIES
  None

RETURN VALUE
  None

SIDE EFFECTS
  Sets the ppp_uim_auth.report_status and ppp_uim_auth.challenge_response.
===========================================================================*/
static void chapi_uim_calc_chap_response_report
(
  uim_rpt_type *report
)

{
/*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/
  /*-------------------------------------------------------------------------
    Copy the uim report status into a variable local to the file
  -------------------------------------------------------------------------*/
  ppp_uim_auth.report_status = report->rpt_status;
  if(ppp_uim_auth.report_status == UIM_PASS)
  {
    /*-----------------------------------------------------------------------
      The uim calc chap command was successful.  This copies the data from
      the uim report
    -----------------------------------------------------------------------*/
    memcpy(ppp_uim_auth.challenge_response,
           report->rpt.compute_ip_auth.data,
           UIM_COMPUTE_IP_AUTH_RESPONSE_LENGTH);
  }
  else
  {
    /*-----------------------------------------------------------------------
      The UIM command failed
    -----------------------------------------------------------------------*/
    MSG_HIGH("UIM-CHAP FAILURE", 0, 0, 0);
  }

  /*-------------------------------------------------------------------------
    Set the signal to indicate to the PS task that the
    uim command has completed
  -------------------------------------------------------------------------*/

  (void) rex_set_sigs( &ps_tcb, 1 << PS_UIM_CMD_SIGNAL );

} /* chapi_uim_calc_chap_response_report() */

#endif /* FEATURE_UIM_SUPPORT_3GPD */


/*===========================================================================
FUNCTION CHAPI_CALC_CHAP_MD5_SW_RESPONSE()

DESCRIPTION
  Generates the 16-byte Hash/CHAP Response.

DEPENDENCIES
  None

RETURN VALUE
  None

SIDE EFFECTS
  None
===========================================================================*/
static byte * chapi_calc_chap_md5_sw_response
(
  ppp_type *ppp_cb_ptr,
  dsm_item_type *response_ptr,
  int response_len
)
{
  MD5_CTX context; /* will contain the seed for making MD5 output. */
  byte* insert_ptr;
  uint8 msg_id = ppp_cb_ptr->auth.active_id;
/* - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  insert_ptr    = response_ptr->data_ptr;
  *insert_ptr++ = PPP_CHAP_RESPONSE;
  *insert_ptr++ = msg_id;
  insert_ptr    = put16(insert_ptr, (uint16)response_len);
  *insert_ptr++ = PPP_CHAP_MD5_SIZE;

  /*-------------------------------------------------------------------------
    Initialize MD5 with a new context, then generate hash.  Values are:
      CHAP ID
      CHAP/PAP password
      CHAP challenge
  --------------------------------------------------------------------------*/
  MD5Init(&context);
  MD5Update(&context, &(msg_id), 1);
  MD5Update(&context,
            (unsigned char *) ppp_cb_ptr->auth.info.passwd_info,
            ppp_cb_ptr->auth.info.passwd_len);
  /*--------------------------------------------------------------------------
    NOTE:  We are currently limited to a single DSM item to hold the
           CHAP challenge value.  We may need to traverse a dsm chain here.
  --------------------------------------------------------------------------*/
  MD5Update(&context,
            ppp_cb_ptr->auth.chap_challenge_ptr->data_ptr,
            ppp_cb_ptr->auth.chap_challenge_ptr->used);

  /*--------------------------------------------------------------------------
    The hash has been computed
    CHAP responses look like:

      +-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
      | 2 (the value) |  Identifier   |            Length             |
      +-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
      | 16 (hash len) |  Hash value
      +-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
      |  username ...
      +-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+

    fill in the fields.
  --------------------------------------------------------------------------*/
  MD5Final(insert_ptr, &context);

  /*--------------------------------------------------------------------------
    Finalize MD5 hash, providing the CHAP response and insert
  --------------------------------------------------------------------------*/
  insert_ptr += PPP_CHAP_MD5_SIZE; /* move ptr forward by hash length */
  return insert_ptr;

} /* chapi_calc_chap_md5_sw_response */



#ifdef FEATURE_UIM_SUPPORT_3GPD
/*===========================================================================
FUNCTION CHAPI_CALC_CHAP_MD5_RUIM_RESPONSE()

DESCRIPTION
  Generates the 16-byte Hash/CHAP Response by sending a command to the
  RUIM with the chap challenge and profile # as input.

DEPENDENCIES
  None

RETURN VALUE
  None

SIDE EFFECTS
  None
===========================================================================*/
static byte * chapi_calc_chap_md5_ruim_response
(
  ppp_type *       ppp_cb_ptr,
  dsm_item_type *  response_ptr,
  int              response_len
)
{
  byte* insert_ptr;
  uint8 msg_id = ppp_cb_ptr->auth.active_id;
  uim_cmd_type *ppp_uim_cmd;
  if ( ( ppp_uim_cmd = (uim_cmd_type*)q_get(&uim_free_q) ) == NULL )
  {
    MSG_HIGH("RUIM command queue full",0,0,0);
    return NULL;
  }	
/* - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  /*------------------------------------------------------------------------
    Build the CHAP response in the provided DSM item
  ------------------------------------------------------------------------*/
//TODO: Replace with dsm_pushdown_packed
  insert_ptr    = response_ptr->data_ptr;
  *insert_ptr++ = PPP_CHAP_RESPONSE;
  *insert_ptr++ = msg_id;
  insert_ptr    = put16(insert_ptr,(uint16)response_len);
  *insert_ptr++ = PPP_CHAP_MD5_SIZE;

  ppp_uim_cmd->compute_ip_auth.cmpt_ip_operation = UIM_CMPT_IP_CHAP;
  ppp_uim_cmd->compute_ip_auth.cmpt_ip_data.chap_data.chap_id = msg_id;

  /*------------------------------------------------------------------------
      This Value Should Be Changed If More Than One Index Is Supported
  ------------------------------------------------------------------------*/

  ppp_uim_cmd->compute_ip_auth.cmpt_ip_data.chap_data.nai_entry_index =
           ppp_cb_ptr->auth.info.nai_entry_index;
  ppp_uim_cmd->compute_ip_auth.cmpt_ip_data.chap_data.chap_challenge_length
                            = ppp_cb_ptr->auth.chap_challenge_ptr->used;
//TODO: Chap challenge length check
//TODO: dsm_extract
  memcpy(ppp_uim_cmd->compute_ip_auth.cmpt_ip_data.chap_data.chap_challenge,
                            ppp_cb_ptr->auth.chap_challenge_ptr->data_ptr,
                            ppp_cb_ptr->auth.chap_challenge_ptr->used);
  ppp_uim_cmd->hdr.command = UIM_COMPUTE_IP_AUTH_F;
  ppp_uim_cmd->hdr.cmd_hdr.task_ptr = NULL;
  ppp_uim_cmd->hdr.cmd_hdr.sigs = 0;
  ppp_uim_cmd->hdr.rpt_function = chapi_uim_calc_chap_response_report;
  ppp_uim_cmd->hdr.protocol = UIM_CDMA;
  ppp_uim_cmd->hdr.options = UIM_OPTION_ALWAYS_RPT;
  ppp_uim_cmd->hdr.slot = UIM_SLOT_AUTOMATIC;

  /*--------------------------------------------------------------------------
      Send the command to the R-UIM:
      Clear the "command done signal"
      Send the command
      Wait for the command to be done
  --------------------------------------------------------------------------*/
  PS_CLR_SIGNAL(PS_UIM_CMD_SIGNAL);

  uim_cmd( ppp_uim_cmd );
  MSG_MED("Blocking PS Task on UIM cmd signal",0,0,0);
  psi_wait((rex_sigs_type) 1 << PS_UIM_CMD_SIGNAL);
  MSG_MED("Unblocking PS TASK on UIM cmd signal",0,0,0);
  //TODO: Put ppp_uim_cmd back into the free q
  if(ppp_uim_auth.report_status == UIM_PASS)
  {
    //TODO: dsm_pushdown_tail
    memcpy(insert_ptr,
           &ppp_uim_auth.challenge_response,
           UIM_COMPUTE_IP_AUTH_RESPONSE_LENGTH);
  }
  else
  {
    MSG_HIGH("UIM-CHAP FAILURE", 0, 0, 0);
    return NULL;
  }

  /*--------------------------------------------------------------------------
    Finalize MD5 hash, providing the CHAP response and insert
  --------------------------------------------------------------------------*/
  insert_ptr += PPP_CHAP_MD5_SIZE; /* move ptr forward by hash length */
  return insert_ptr;
} /* chapi_calc_chap_md5_ruim_response() */
#endif /* FEATURE_UIM_SUPPORT_3GPD */

/*===========================================================================
FUNCTION CHAPI_SEND_RESP()

DESCRIPTION
  Builds and transfers the CHAP packet.

DEPENDENCIES
  None

RETURN VALUE
  None

SIDE EFFECTS
  None
===========================================================================*/
static void chapi_send_resp
(
  ppp_type *ppp_cb_ptr
)
{
  dsm_item_type *response_ptr;
  byte *insert_ptr;
  int response_len;

/* - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - */

  /*-------------------------------------------------------------------------
    Check if the user_id or secret lengths are 0. If yes, then return without
    transmitting any data - assume the peer will send another challenge or
    tear down the link.  Free the chap challenge packet.
  -------------------------------------------------------------------------*/
  if( ppp_cb_ptr->auth.info.user_id_len == 0 ||
      ppp_cb_ptr->auth.info.passwd_len == 0 
  /*------------------------------------------------------------------------
    The following check is for SN calls.  If Simple IP is supported in the 
    R-UIM, allow a zero password 
    -----------------------------------------------------------------------*/
#ifdef FEATURE_UIM_SUPPORT_3GPD
      && (UIM_3GPD_MIP_NV_SIP_NV == uim_3gpd_support() ||
       UIM_3GPD_MIP_RUIM_SIP_NV == uim_3gpd_support())
#endif /* FEATURE_UIM_SUPPORT_3GPD */
  )
  {
    MSG_HIGH("dev %d empty username or secret",
             ppp_cb_ptr->fsm[LCP].device,0,0);
    dsm_free_packet(&(ppp_cb_ptr->auth.chap_challenge_ptr));
    return;
  }

  /*-------------------------------------------------------------------------
    Calculate the length of the response packet.
  -------------------------------------------------------------------------*/
  response_len =  PPP_AUTH_CODE_SIZE
    + PPP_AUTH_ID_SIZE
    + PPP_AUTH_LENGTH_SIZE
    + PPP_CHAP_VALUE_SIZE
    + PPP_CHAP_MD5_SIZE
    + ppp_cb_ptr->auth.info.user_id_len;

  /*-------------------------------------------------------------------------
    Get a dsm_item to store the response that will be generated. If there is
    no memory to allocate, then return to inform that allocation failed.
    Free the challenge pointer.  Assume we will be rechallenged or the peer
    will terminate the connection.
  -------------------------------------------------------------------------*/
  response_ptr =
    dsm_offset_new_buffer(DSM_DS_POOL_SIZE(response_len + PPP_HDR_LEN),
                          PPP_HDR_LEN);
  if(response_ptr == NULL)
  {
    MSG_ERROR("dev %d CHAP resp mem alloc failed",
              ppp_cb_ptr->fsm[LCP].device, 0, 0);
    dsm_free_packet(&(ppp_cb_ptr->auth.chap_challenge_ptr));
    return;
  }

  /*-------------------------------------------------------------------------
    Fill in the following fields in the dsm item:
        used field with the total response length;
        Kind with CHAP and
        Priority as Highest.
  -------------------------------------------------------------------------*/
  response_ptr->used     = response_len;
  response_ptr->kind     = DSM_PS_CHAP_PROTOCOL;
  response_ptr->priority = DSM_HIGHEST;
#ifdef FEATURE_DSM_MEM_CHK
  response_ptr->tracer   = DSM_CHAP_RESP;
#endif

  /*-------------------------------------------------------------------------
    Call the appropriate function to calculate the CHAP response
  -------------------------------------------------------------------------*/
#ifdef FEATURE_UIM_SUPPORT_3GPD
  if((UIM_3GPD_MIP_RUIM_SIP_RUIM == uim_3gpd_support() ||
      UIM_3GPD_MIP_NV_SIP_RUIM == uim_3gpd_support())
    )
  {
    /*-----------------------------------------------------------------------
      If using RUIM, go to the RUIM to get the CHAP response
      This DOES NOT CHECK if it an SN Stream, only that it is NOT an AN Stream.
      If Streams other than SN and AN begin to seek authentication functionality
      through chapi_send_resp, then some additional checks might be needed 
      here in the future
    -----------------------------------------------------------------------*/
    insert_ptr = chapi_calc_chap_md5_ruim_response( ppp_cb_ptr,
                                                    response_ptr,
                                                    response_len);
  }
  else
#endif /* FEATURE_UIM_SUPPORT_3GPD */
  {
    /*-----------------------------------------------------------------------
      Use the software MD-5 algorithm to calculate CHAP response
    -----------------------------------------------------------------------*/
    insert_ptr = chapi_calc_chap_md5_sw_response( ppp_cb_ptr,
                                                  response_ptr,
                                                  response_len );
  }

  /*-------------------------------------------------------------------------
    Check if the calculation failed.  If so, do the right thing.
  -------------------------------------------------------------------------*/
  if (insert_ptr == NULL)
  {
    MSG_ERROR("dev %d CHAP resp calculation failed",
              ppp_cb_ptr->fsm[LCP].device, 0, 0);
    dsm_free_packet(&(ppp_cb_ptr->auth.chap_challenge_ptr));
    return;
  }

  /*-------------------------------------------------------------------------
    Copy the user_id into the response packet.
  -------------------------------------------------------------------------*/
  memcpy(insert_ptr,
         ppp_cb_ptr->auth.info.user_id_info,
         ppp_cb_ptr->auth.info.user_id_len);

  /*-------------------------------------------------------------------------
    we are done - so transmit the packet
  -------------------------------------------------------------------------*/
  if(pppi_outgoing(ppp_cb_ptr->fsm[LCP].device,
                   PPP_CHAP_PROTOCOL,
                   &response_ptr,
                   NULL) == -1)
  {
    /*---------------------------------------------------------------------
      pppi_outgoing() frees the packet in case of an error - let the timer
      to trigger another attempt.
    ---------------------------------------------------------------------*/
    MSG_HIGH("dev %d sending CHAP reponse failed!",
              ppp_cb_ptr->fsm[LCP].device, 0, 0);
  }

  /*-------------------------------------------------------------------------
    Start the retry timer.
    NOTE: we use the LCP timer as authentication doesn't have it's own timer.
  --------------------------------------------------------------------------*/
  ps_timer_start(ppp_cb_ptr->fsm[LCP].timer, PPP_AUTH_TIMER_VAL);

} /* chapi_send_resp() */



/*===========================================================================
FUNCTION CHAPI_SEND_CHALLENGE()

DESCRIPTION
  This function builds a chap challenge and sends it out.

PARAMETERS
  ppp_cb_ptr: ppp control block

RETURN VALUE
  None

DEPENDENCIES
  None

SIDE EFFECTS
  if there isn't enough memory then PPP will be closed.
===========================================================================*/
static void chapi_send_challenge
(
  ppp_type *ppp_cb_ptr
)
{
  dsm_item_type *chal_ptr;
  uint8 *insert_ptr;
  ppp_auth_info_type *auth_info_ptr;
  uint32 random_value;
  int chal_msg_len;
  int loop;

/*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/
  ASSERT(ppp_cb_ptr != NULL);

  auth_info_ptr = &(ppp_cb_ptr->auth.info);
  /*-------------------------------------------------------------------------
    calculate the total message length.
  -------------------------------------------------------------------------*/
  chal_msg_len = PPP_AUTH_CODE_SIZE
    + PPP_AUTH_ID_SIZE
    + PPP_AUTH_LENGTH_SIZE
    + PPP_CHAP_VALUE_SIZE
    + PPP_CHAP_CHAL_LEN
    + auth_info_ptr->challenge_name_len;

  /*-------------------------------------------------------------------------
    get a dsm item to build the challenge packet in.
  -------------------------------------------------------------------------*/
  chal_ptr =
    dsm_offset_new_buffer(DSM_DS_POOL_SIZE(chal_msg_len + PPP_HDR_LEN),
                          PPP_HDR_LEN);

  /*-------------------------------------------------------------------------
    if the allocation  fails skip forward to the timer - this will trigger
    another attempt.
  -------------------------------------------------------------------------*/
  if(chal_ptr == NULL)
  {
    MSG_ERROR("dev %d CHAP challenge mem alloc failed",
              ppp_cb_ptr->fsm[LCP].device, 0, 0);
    goto give_up;
  }

  /*-------------------------------------------------------------------------
    Fill in the following fields in the dsm item:
        used field with the total response length;
        Kind with CHAP and
        Priority as Highest.
  -------------------------------------------------------------------------*/
  chal_ptr->used     = chal_msg_len;
  chal_ptr->kind     = DSM_PS_CHAP_PROTOCOL;
  chal_ptr->priority = DSM_HIGHEST;
#ifdef FEATURE_DSM_MEM_CHK
  chal_ptr->tracer   = DSM_PPP_TX_WM;
#endif /* FEATURE_DSM_MEM_CHK */

  /*-------------------------------------------------------------------------
    Chap challenge looks like the following:

     +-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
     | 1 (the value) |  Identifier   |            Length             |
     +-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
     |  chal len     |  challenge
     +-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
     |  MS name
     +-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+

   The challenge length will always be 16 bytes long.  Fill in all of the
   fields preceeding the challenge.
  -------------------------------------------------------------------------*/
  insert_ptr    = chal_ptr->data_ptr;
  *insert_ptr++ = PPP_CHAP_CHALLENGE;               /* code                */
  *insert_ptr++ = ++ppp_cb_ptr->auth.active_id;     /* id field            */
  insert_ptr    = put16(insert_ptr, chal_msg_len);  /* total length of pkt */
  *insert_ptr++ = PPP_CHAP_CHAL_LEN;                /* challenge length    */

  /*-------------------------------------------------------------------------
    Build the challenge in the auth info field: first set the length to the
    length we are filling - then loop over the entire size of the challenge
    area writting the 4 bytes we get from ran_next(). Then copy it into the
    challenge packet.
  -------------------------------------------------------------------------*/
  auth_info_ptr->challenge_len = PPP_CHAP_CHAL_LEN;
  for(loop = 0; loop < PPP_CHAP_CHAL_LEN ; loop += 4)
  {
    random_value = ran_next();
    memcpy(auth_info_ptr->challenge_info + loop, &random_value, 4);
  }
  memcpy(insert_ptr, auth_info_ptr->challenge_info, PPP_CHAP_CHAL_LEN);
  insert_ptr += PPP_CHAP_CHAL_LEN;

  /*-------------------------------------------------------------------------
    copy the challenge name and increment the used field appropriately
  -------------------------------------------------------------------------*/
  memcpy(insert_ptr,
         auth_info_ptr->challenge_name,
         auth_info_ptr->challenge_name_len);
  insert_ptr += auth_info_ptr->challenge_name_len;

  /*-------------------------------------------------------------------------
    we are done - so transmit the packet
  -------------------------------------------------------------------------*/
  if(pppi_outgoing(ppp_cb_ptr->fsm[LCP].device,
                   PPP_CHAP_PROTOCOL,
                   &chal_ptr,
                   NULL) == -1)
  {
    /*---------------------------------------------------------------------
      pppi_outgoing() frees the packet in case of an error - let the timer
      to trigger another attempt.
    ---------------------------------------------------------------------*/
    MSG_HIGH("dev %d sending CHAP challenge failed!",
              ppp_cb_ptr->fsm[LCP].device, 0, 0);
  }

  /*-------------------------------------------------------------------------
    Start the retry timer.
    NOTE: we use the LCP timer as authentication doesn't have it's own timer.
  --------------------------------------------------------------------------*/
 give_up:
  ps_timer_start(ppp_cb_ptr->fsm[LCP].timer, PPP_AUTH_TIMER_VAL);

} /* chapi_send_challenge() */

#endif /* FEATURE_DATA_PS */
