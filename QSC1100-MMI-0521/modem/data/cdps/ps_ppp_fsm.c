/*====*====*====*====*====*====*====*====*====*====*====*====*====*====*====*

                           P S _ P P P _ F S M . C

GENERAL DESCRIPTION
 PPP Finite State Machine
 
 Acknowledgements and correction history may be found in PPP.C

EXTERNALIZED FUNCTIONS

INITIALIZATION AND SEQUENCING REQUIREMENTS

  Copyright 1992-1994 William Allen Simpson
    Licensed to Qualcomm -- 95 Jan 21

  Copyright (c) 1995-2009 QUALCOMM Incorporated. 
  All Rights Reserved.
  Qualcomm Confidential and Proprietary.
*====*====*====*====*====*====*====*====*====*====*====*====*====*====*====*/

/*===========================================================================

                        EDIT HISTORY FOR MODULE

  $PVCSPath: L:/src/asw/MM_DATA/vcs/ps_ppp_fsm.c_v   1.6   12 Feb 2003 14:29:52   mlioy  $
  $Header: //source/qcom/qct/modem/data/cdps/ps/main/lite/src/ps_ppp_fsm.c#3 $ $DateTime: 2009/05/27 05:07:18 $ $Author: nsivakum $

when        who    what, where, why
--------    ---    ---------------------------------------------------------- 
04/29/09    sn     Ported support for call throttle feature (DCTM).
02/22/08    pp     Klocwork fixes.
09/12/03    mvl    Added some JCDMA timing changes.
04/29/03    jd     Added back retry counter for sending LCP term-req's
04/18/03    vsk    use fsm->mode instead of fsm->passive mode to set passive
                   mode operation
04/08/03    mvl    fixed fsm_ack().
03/28/03    mvl    Cleanup.
02/06/03    mvl    Changed fsm_timeout() to return TRUE if the timeout
                   happened in the open state - as we don't want to give up
                   when already open.
11/14/02   jd/usb  Only add nak'd option to work if will
11/04/02    mvl    Starting setup timer for resyncs.
09/18/02    mvl    Added user_data to ppp event callbacks.
08/13/02    mvl    Added session handle, and using MTU from iface.
07/30/02    mvl    Put TASKLOCK() around invokation of event callbacks.
07/25/02    mvl    Removed netmodel dependency.  Updates for PPP renaming.
07/15/02    aku    Added pstimer.h and removed dssnet.h
07/11/02    mvl    Now calling resync callbacks only when C-Req received in
                   open state.
07/01/02    mvl    Added support for generating an event rather than calling
                   dssnet state machine directly.
05/22/02    mvl    removed non-netmodel ppp_outgoing() calls.
04/17/02    rc     Wrapped code in !FEATURE_DATA_WCDMA_PS as appropriate to 
                   build for all targets
03/01/02    vr     Added support for PPP passive mode.
03/01/02    vsk    removed dsm_free_packet() being called in case of 
                   ppp_outgoing() returns an error
12/21/01    dwp    Wrap call to dssnet_sm_post_event in !FEATURE_DATA_PS.
11/12/01    aku    Posting event to dssnet when PPP resyncs
11/12/01    dwp    Add "|| FEATURE_DATA_PS" to whole module, include of tcp.h
90/20/01  jd/mvl   Modified the resetting of the nak retry count and
                   configuration retry so it is more selective.
08/29/01  pjb/mvl  Modified the values of option bits to support DNS options.
08/02/01    mvl    general code cleanup
08/01/01    na     Fixed functions that were using the global ppp_cb_ptr
                   Return from fsm_proc() on error.
07/29/01    na     Removed multiple retries when MS sends Term-Req.
                   Also, MS does not send TReq when it receives T-Ack.
07/27/01    mvl    Changed all of the psi_*_timer() to ps_timer_*() calls
07/26/01    aku    Replaced psi_protocol_closed_event with dsiface_down().
07/23/01    mvl    Removed the string arrays with constants names, modified
                   fsm_send() such that ppp_outgoing() is called with the
                   iface in the fsm struct, removed obsolete debug macros and
                   code cleanup.
07/21/01    pjb    Return type of dsm_pushdown changed.  
05/25/01    na     On a resync the retry_cnt was not being set. Now fixed.
05/18/01    mvl    General code cleanup.
08/25/00    rc     Changed dsm_pushdown() to take take pool id as fourth 
                   parameter.Made changes to handle dsm_pushdown() 
                   returning FALSE. 
                   Handled the case when htoncnf() returns NULL.
                   Changed dsm_new_buffer() calls to take pool id as input 
                   parameter. So, a new macro DSM_DS_POOL_SIZE is used to 
                   return pool id based on pool size. Added Macros that 
                   check if there are enough items or if we are running 
                   out of items. 
                   Changed FEATURE_DS_MEM_CHK to FEATURE_DSM_MEM_CHK.
05/04/00    mvl    Changed calls to ppp_outgoing() to reflect new interface.
04/27/00    na     Fixed a memory leak in fsm_request(), when a option that 
                   needed to be rejected was in the middle of a Config-Req.
08/20/99    mvl    Fixed some warnings, and fixed variable names and cleaned
                   up fsm_ack()
08/17/99    smp    Added include files msg.h & err.h.
08/09/99    mvl    Added network model changes
04/08/99    na     Use dsm_kind instead of protocol to tag packets for output
01/27/98    ldg    Removed the DS_SLIM switch.
06/17/97    fkm    FEATURE_xxx Updates (via Automatic Scripts)
02/13/97    na     Removed ps_ppp_tx_wm. CTIA show build.
05/24/96    jjw    Gemini & ISS2 Data Services release.
11/09/95    jjw    IS-99 Compliant Release
===========================================================================*/

/*===========================================================================

                          INCLUDE FILES FOR MODULE

===========================================================================*/
#include "comdef.h"
#include "customer.h"       /* Customer Specific Features */
#if 1 // #ifdef FEATURE_DATA_PS
#include "dsbyte.h"
#include "dsm.h"
#include "ps_ppp.h"
#include "ps_pppi.h"
#include "ps_ppp_fsm.h"
#include "ps_ppp_lcp.h"
#include "err.h"
#include "pstimer.h"
#include "ps_tcp.h"              /* PS_MAX_HDR */

/*=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=

                     GLOBAL DEFINITIONS AND DECLARATIONS

=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=*/
/*---------------------------------------------------------------------------
  The PPP control block - an array as one is needed per instance.
---------------------------------------------------------------------------*/
ppp_type ppp_cb_array[PPP_MAX_DEV];            /* PPP ctrl block array */

/*=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=

                            FORWARD DECLARATIONS

=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=*/
static dsm_item_type  *htoncnf
(
  struct config_hdr *cnf,
  dsm_item_type     *bp
);

static void fsm_makeoptions
(
  ppp_fsm_type    *fsm_ptr,
  void            *vp,
  dsm_item_type  **bpp,
  negotiate_t      negotiating
);

static dsm_item_type  *fsm_makereq
(
  ppp_fsm_type *fsm_ptr
);

static int fsm_configreq
(
  ppp_fsm_type *fsm_ptr
);

static int fsm_termreq
(
  ppp_fsm_type *fsm_ptr
);

static int fsm_termack
(
  ppp_fsm_type *fsm_ptr,
  uint8         identifier
);

static void fsm_irc
(
  ppp_fsm_type *fsm_ptr
);

static void fsm_zrc
(
  ppp_fsm_type *fsm_ptr
);

static void fsm_opening
(
  ppp_fsm_type *fsm_ptr
);

static int fsm_request
(
  ppp_fsm_type      *fsm_ptr,
  struct config_hdr *config,
  dsm_item_type     *bp
);

static int fsm_ack
(
  ppp_fsm_type      *fsm_ptr,
  struct config_hdr *config,
  dsm_item_type     *bp
);

static int fsm_nak
(
  ppp_fsm_type      *fsm_ptr,
  struct config_hdr *config,
  dsm_item_type     *bp
);

static int fsm_reject
(
  ppp_fsm_type      *fsm_ptr,
  struct config_hdr *config,
  dsm_item_type     *bp
);

/*===========================================================================

            LOCAL DEFINITIONS AND DECLARATIONS FOR MODULE

===========================================================================*/
#ifndef T_ARM
char *fsmStates[] =
{
  "Initial",
  "Starting",
  "Closed",
  "Stopped",
  "Closing",
  "Stopping",
  "Req Sent",
  "Ack Rcvd",
  "Ack Sent",
  "Opened"
};

char *fsmCodes[] =
{
  NULL,
  "Config Request",
  "Config Ack",
  "Config Nak",
  "Config Reject",
  "Termin Request",
  "Termin Ack",
  "Code Reject",
  "Protocol Reject",
  "Echo Request",
  "Echo Reply",
  "Discard Request",
  "Identification",
  "Time Remaining"
};
#endif /* T_ARM */


/*=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=

                             EXTERNAL FUNCTIONS

=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=*/
/*===========================================================================
FUNCTION HTONCNF()

DESCRIPTION
  This function will Convert header in host form to network form

DEPENDENCIES
  None

RETURN VALUE
  None

SIDE EFFECTS
  None
===========================================================================*/
static dsm_item_type  *htoncnf
(
  struct config_hdr *cnf,
  dsm_item_type     *bp
)
{
  register uint8 *cp;
  /* - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - */

  if (dsm_pushdown(&bp, NULL, CONFIG_HDR_LEN, 
           DSM_DS_SMALL_ITEM_POOL) == CONFIG_HDR_LEN)
  {
    cp = bp->data_ptr;
    *cp++ = cnf->code;
    *cp++ = cnf->identifier;
    put16(cp, cnf->length);
  }
  else
  {
    ERR("Out of Memory",0,0,0);
    dsm_free_packet(&bp);
  }

  return bp;

} /* htoncnf() */



/*===========================================================================

FUNCTION FSM_N_OPTION()

DESCRIPTION
  This function will map and option number back to it mask value. 

DEPENDENCIES
  None

RETURN VALUE
  None

SIDE EFFECTS
  None
===========================================================================*/
negotiate_t fsm_n_option
(
  struct fsm_constant_s *fsm_constants_ptr,
  uint8 option
)
{
  uint8  n_option_limit = fsm_constants_ptr->n_option_limit;
  uint8  *n_option = fsm_constants_ptr->n_option;
  uint8  index;

  for(index = 0 ; index < n_option_limit ; index ++)
  {
    if(n_option[index] == option)
    { 
      return index;
    }
  }    
  return 0;
}


/*===========================================================================

FUNCTION NTOHCNF()

DESCRIPTION
  This function will Extract header from incoming packet

DEPENDENCIES
  None

RETURN VALUE
  None

SIDE EFFECTS
  None
===========================================================================*/
int ntohcnf
(
  struct config_hdr *cnf,
  dsm_item_type    **bpp
)
{
  uint8 cnfb[CONFIG_HDR_LEN];
  /* - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - */

  if ( cnf == NULL )
  {
    return -1;
  }

  if ( dsm_pullup( bpp, cnfb, CONFIG_HDR_LEN ) < CONFIG_HDR_LEN )
  {
    return -1;
  }

  cnf->code       = cnfb[0];
  cnf->identifier = cnfb[1];
  cnf->length     = get16(&cnfb[2]);
  return 0;

} /* ntohcnf() */



/*===========================================================================
FUNCTION HTONOPT_COPY()

DESCRIPTION
  This function will onvert configuration option header in host form to
  network form and copy data

DEPENDENCIES
  None

RETURN VALUE
  None

SIDE EFFECTS
  None
===========================================================================*/
void htonopt_copy
(
  dsm_item_type    **bpp,
  struct option_hdr *opt,
  dsm_item_type    **copy_bpp
)
{
  dsm_item_type  *bp = *bpp;
  register uint8 *cp;
  register int used = opt->length - OPTION_HDR_LEN;
  /* - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - */

  if ( bp->used + opt->length > bp->size )
  {
    return;
  }
  cp = bp->data_ptr + bp->used;
  *cp++ = opt->type;
  *cp++ = opt->length;

  while ( used-- > 0 )
  {
    *cp++ = dsm_pull8(copy_bpp);
  }
  bp->used += opt->length;

} /* htonopt_copy() */



/*===========================================================================
FUNCTION NTOHOPT()

DESCRIPTION
  This function will Extract configuration option header

DEPENDENCIES
  None

RETURN VALUE
  None

SIDE EFFECTS
  None
===========================================================================*/
int ntohopt
(
  struct option_hdr *opt,
  dsm_item_type    **bpp
)
{
  char optb[OPTION_HDR_LEN];
  /* - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - */

  if ( opt == NULL )
  {
    return -1;
  }

  if ( dsm_pullup( bpp, optb, OPTION_HDR_LEN ) < OPTION_HDR_LEN )
  {
    return -1;
  }

  opt->type   = optb[0];
  opt->length = optb[1];
  return 0;

} /* ntohopt() */



/*===========================================================================
FUNCTION FSM_TIMER()

DESCRIPTION
  This function will Set a timer in case an expected event does not occur

DEPENDENCIES
  None

RETURN VALUE
  None

SIDE EFFECTS
  None
===========================================================================*/
void fsm_timer
(
  ppp_fsm_type *fsm_ptr
)
{
  /* - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - */

  ps_timer_start(fsm_ptr->timer, fsm_ptr->pdc->timeout);

} /* fsm_timer() */



/*===========================================================================
FUNCTION FSM_SEND()

DESCRIPTION
  This function will Send a packet to the remote host

DEPENDENCIES
  None

RETURN VALUE
  0 on success
 -1 on failure

SIDE EFFECTS
  None
===========================================================================*/
int fsm_send
(
  ppp_fsm_type   *fsm_ptr,
  uint8           code,
  uint8           identifier,
  dsm_item_type  *bp
)
{
  struct config_hdr hdr;
  ppp_type *ppp_cb_ptr;
  /* - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - */

  if( fsm_ptr == NULL || fsm_ptr->device >= PPP_MAX_DEV )
  {
    MSG_ERROR("Invalid args", 0, 0, 0);
    dsm_free_packet(&bp);
    return -1;
  }

  ppp_cb_ptr = &(ppp_cb_array[fsm_ptr->device]);
  
  switch( hdr.code = code )
  {
  case CONFIG_REQ:
  case TERM_REQ:
  case ECHO_REQ:
    /*-----------------------------------------------------------------------
      Save ID field for match against replies from remote host
    -----------------------------------------------------------------------*/
    fsm_ptr->lastid = ppp_cb_ptr->id;
    /* fallthru */

  case PROT_REJ:
  case DISCARD_REQ:
    /*-----------------------------------------------------------------------
      Use a unique ID field value
    -----------------------------------------------------------------------*/
    hdr.identifier = ppp_cb_ptr->id++;
    break;

  case CONFIG_ACK:
  case CONFIG_NAK:
  case CONFIG_REJ:
  case TERM_ACK:
  case CODE_REJ:
  case ECHO_REPLY:
    /*-----------------------------------------------------------------------
      Use ID sent by remote host
    -----------------------------------------------------------------------*/
    hdr.identifier = identifier;
    break;

  default:
    /*-----------------------------------------------------------------------
      we're in trouble
    -----------------------------------------------------------------------*/
    return -1;
  };

  switch( code )
  {
  case ECHO_REQ:
  case ECHO_REPLY:
  case DISCARD_REQ:
  {
    register lcp_value_type *vp = (lcp_value_type*)fsm_ptr->local.work_pdv;

    if (dsm_pushdown(&bp,
                     NULL,
                     sizeof(vp->magic_number), 
                     DSM_DS_SMALL_ITEM_POOL) < sizeof(vp->magic_number))
    {
      ERR("Out of Memory",0,0,0);
      dsm_free_packet(&bp);
      return -1;
    }
    put32(bp->data_ptr, vp->magic_number);
  }
  };

  hdr.length = dsm_length_packet(bp) + CONFIG_HDR_LEN;

  /*-------------------------------------------------------------------------
    Prepend header to packet data
  -------------------------------------------------------------------------*/
  bp = htoncnf(&hdr,bp);
  if (bp == NULL)
  {
    return -1;
  }

  ppp_cb_ptr->OutNCP[fsm_ptr->pdc->fsmi]++;

  /*---------------------------------------------------------------------
    Set the packet priority to the highest then enqueue to PPP ready-for-
    TX watermark.
  ---------------------------------------------------------------------*/
#ifdef FEATURE_DSM_MEM_CHK
  /*-------------------------------------------------------------------------
   Set tracer to indicate where the item is being stored
  -------------------------------------------------------------------------*/
  bp->tracer = DSM_PPP_TX_WM;
#endif
  bp->priority = DSM_HIGHEST;
  bp->kind     = fsm_ptr->pdc->dsm_kind;

  /*-------------------------------------------------------------------------
    Send the built up packet
  -------------------------------------------------------------------------*/
  if(pppi_outgoing(fsm_ptr->device, 0, &bp, NULL) == -1)
  {
    /*---------------------------------------------------------------------
      pppi_outgoing() frees the packet in case of an error
    ---------------------------------------------------------------------*/
    MSG_HIGH( "pppi_outgoing() Error", 0, 0, 0);
  }

  return 0;
} /* fsm_send() */


/*===========================================================================
FUNCTION FSM_MAKEOPTIONS()

DESCRIPTION
  This function will Build a list of options

DEPENDENCIES
  None

RETURN VALUE
  None

SIDE EFFECTS
  None
===========================================================================*/
static void fsm_makeoptions
(
  ppp_fsm_type    *fsm_ptr,
  void            *v_ptr,
  dsm_item_type  **item_head_ptr,
  negotiate_t      negotiating
)
{
  register uint8 o_type;
  /* - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - */

  for ( o_type = 0; o_type <= fsm_ptr->pdc->n_option_limit; o_type++ )
  {
    if (negotiating & (1 << o_type))
    {
      (*fsm_ptr->pdc->option)(fsm_ptr,
                              v_ptr, 
                              item_head_ptr, 
                              fsm_ptr->pdc->n_option[o_type]);
    }
  }
} /* fsm_makeoptions() */



/*===========================================================================
FUNCTION FSM_MAKEREQ()

DESCRIPTION
  This function will Build a request to send to remote host

DEPENDENCIES
  None

RETURN VALUE
  None

SIDE EFFECTS
  None
===========================================================================*/
static dsm_item_type  *fsm_makereq
(
  ppp_fsm_type *fsm_ptr
)
{
  dsm_item_type  *req_bp = NULL;
  ppp_type *ppp_cb_ptr = ppp_cb_array + (fsm_ptr->device);
  /* - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - */

  req_bp = dsm_offset_new_buffer(
    DSM_DS_POOL_SIZE(ppp_cb_ptr->rx_iface_ptr->v4_net_info.mtu),
    PS_MAX_HDR);
  if(req_bp != NULL )
  {
    fsm_makeoptions( fsm_ptr,
                     fsm_ptr->local.work_pdv,
                     &req_bp,
                     fsm_ptr->local.work);
  }
  return(req_bp);

} /* fsm_makereq() */



/*===========================================================================
FUNCTION FSM_CONFIGREQ()

DESCRIPTION
  This function will send a Configure Request

DEPENDENCIES
  None

RETURN VALUE
  None

SIDE EFFECTS
  None
===========================================================================*/
static int fsm_configreq
(
  ppp_fsm_type *fsm_ptr
)
{
  dsm_item_type  *bp;
  /* - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - */

  if ( fsm_ptr->retry_request == 0 )
  {
    return -1;
  }

  fsm_ptr->retry_request--;
  fsm_timer(fsm_ptr);

  bp = fsm_makereq(fsm_ptr);
  return( fsm_send(fsm_ptr, CONFIG_REQ, 0, bp) );

} /* fsm_configreq() */



/*===========================================================================
FUNCTION FSM_TERMREQ()

DESCRIPTION
  This function will send a Terminate Request

DEPENDENCIES
  None

RETURN VALUE
  None

SIDE EFFECTS
  None
===========================================================================*/
static int fsm_termreq
(
  ppp_fsm_type *fsm_ptr
)
{
  /* - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - */

  MSG_LOW ("Send term_req",0,0,0);
  if ( fsm_ptr->retry_request == 0 )
  {
    return -1;
  }

  fsm_ptr->retry_request--;
#ifdef FEATURE_JCDMA_1X
  fsm_ptr->pdc->timeout = LCP_TERM_TO;
#else
  fsm_ptr->pdc->timeout *= LCP_TERM_MULT;
#endif /* FEATURE_JCDMA_1X */
  fsm_timer(fsm_ptr);
#ifdef FEATURE_JCDMA_1X
  fsm_ptr->pdc->timeout = LCP_TIMEOUT;
#else
  fsm_ptr->pdc->timeout /= LCP_TERM_MULT;
#endif /* FEATURE_JCDMA_1X */
  return( fsm_send(fsm_ptr, TERM_REQ, 0, NULL) );

} /* fsm_termreq() */



/*===========================================================================
FUNCTION FSM_TERMACK()

DESCRIPTION
  This function will send a Terminate Ack

DEPENDENCIES
  None

RETURN VALUE
  None

SIDE EFFECTS
  None
===========================================================================*/
static int fsm_termack
(
  ppp_fsm_type *fsm_ptr,
  uint8         identifier
)
{
  /* - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - */

  MSG_MED ("fsm_termack (dev %d)", fsm_ptr->device,0,0);
  return( fsm_send(fsm_ptr, TERM_ACK, identifier, NULL) );

} /* fsm_termack() */



/*===========================================================================
FUNCTION FSM_IRC()

DESCRIPTION
  This function will Initialize the Restart Counter

DEPENDENCIES
  None

RETURN VALUE
  None

SIDE EFFECTS
  None
===========================================================================*/
static void fsm_irc_rx_ack
(
  ppp_fsm_type *fsm_ptr
)
{
  /* - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - */
  fsm_ptr->retry_request = fsm_ptr->try_configure;

} /* fsm_irc_rx_ack() */

static void fsm_irc_tx_ack
(
  ppp_fsm_type *fsm_ptr
)
{
  /* - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - */
  fsm_ptr->retry_nak = fsm_ptr->try_nak;

} /* fsm_irc_tx_ack() */

static void fsm_irc
(
  ppp_fsm_type *fsm_ptr
)
{
  /* - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - */
  fsm_irc_tx_ack(fsm_ptr);
  fsm_irc_rx_ack(fsm_ptr);

} /* fsm_irc() */

/*===========================================================================
FUNCTION FSM_ZRC()

DESCRIPTION
  This function will restart the Zero counter

DEPENDENCIES
  None

RETURN VALUE
  None

SIDE EFFECTS
  None
===========================================================================*/
static void fsm_zrc
(
  ppp_fsm_type *fsm_ptr
)
{
  /* - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - */

  fsm_ptr->retry_request = 0;
  fsm_timer(fsm_ptr);

} /* fsm_zrc() */



/*===========================================================================
FUNCTION FSM_OPENING()

DESCRIPTION
  This function will Configuration negotiation complete

DEPENDENCIES
  None

RETURN VALUE
  None

SIDE EFFECTS
  None
===========================================================================*/
static void fsm_opening
(
  ppp_fsm_type *fsm_ptr
)
{
  /* - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - */

  ps_timer_cancel(fsm_ptr->timer);

  fsm_ptr->state = fsmOPENED;
  (*fsm_ptr->pdc->opening)(fsm_ptr);

} /* fsm_opening() */



/*===========================================================================
FUNCTION FSM_REQUEST()

DESCRIPTION
  This function will Check configuration options requested by the remote host

DEPENDENCIES
  None

RETURN VALUE
  None

SIDE EFFECTS
  None
===========================================================================*/
static int fsm_request
(
  ppp_fsm_type      *fsm_ptr,
  struct config_hdr *config,
  dsm_item_type     *bp
)
{
  struct option_hdr option;                       /* option header storage */
  dsm_item_type    *reply_bp = NULL;              /* reply packet          */
  dsm_item_type    *option_bp = NULL;             /* option copy           */
  ppp_type         *ppp_cb_ptr = ppp_cb_array + (fsm_ptr->device);
  sint31            signed_length = config->length;
  int               option_result ;               /* option reply          */
  int               remaining;          
  negotiate_t       desired;                      /* desired to negotiate  */
  uint8             reply_result = CONFIG_ACK;    /* reply to request      */

  /* - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - */

  fsm_ptr->remote.work = FALSE;    /* clear all options */

  reply_bp = dsm_offset_new_buffer(
    DSM_DS_POOL_SIZE(ppp_cb_ptr->rx_iface_ptr->v4_net_info.mtu),
    PS_MAX_HDR);
  if(reply_bp == NULL )
  {
    dsm_free_packet(&bp);
    return -1;
  }

  MSG_MED("fsm_request(device %d) retry_nak = %d",
          fsm_ptr->device,
          fsm_ptr->retry_nak, 0);
  /* Process options requested by remote host */
  while (signed_length > 0  &&  ntohopt(&option, &bp) != -1)
  {
    if ( (signed_length -= option.length) < 0 )
    {
      dsm_free_packet(&bp);
      dsm_free_packet(&reply_bp);
      return -1;
    }

    /* save rest of option for later */
    if ( (remaining = option.length - OPTION_HDR_LEN) > 0 )
    {
      dsm_dup_packet( &option_bp, bp, 0, remaining );
      if ( option_bp == NULL )
      {
        dsm_free_packet(&bp);
        dsm_free_packet(&reply_bp);
        return -1;
      }
    }

    if ( (option_result = (*fsm_ptr->pdc->check)( fsm_ptr, &option,
        &bp, TRUE )) < 0 )
    {
      dsm_free_packet(&bp);
      dsm_free_packet(&reply_bp);
      dsm_free_packet(&option_bp);
      return -1;
    }

    if ( option_result < reply_result )
    {
      dsm_free_packet(&option_bp);
      continue;
    }
    else if ( option_result > reply_result )
    {
      /* Discard current list of replies */
      reply_bp->used = 0;
      reply_result = option_result;
    }

    switch ( option_result )
    {
    case CONFIG_ACK:
      fsm_ptr->remote.work |= (1 << fsm_n_option(fsm_ptr->pdc, 
                                                 option.type));
      htonopt_copy( &reply_bp, &option, &option_bp );
      break;

    case CONFIG_NAK:
      fsm_ptr->remote.work |= (1 << fsm_n_option(fsm_ptr->pdc, 
                         option.type));
      (*fsm_ptr->pdc->option)(fsm_ptr,
                              fsm_ptr->remote.work_pdv,
                              &reply_bp,
                              option.type );
      break;

    case CONFIG_REJ:
      htonopt_copy( &reply_bp, &option, &option_bp );
      break;
    };
    dsm_free_packet( &option_bp );
  }

  /* Now check for any missing options which are desired */
  if ( fsm_ptr->retry_nak > 0 &&
       (desired = fsm_ptr->remote.want & ~fsm_ptr->remote.work) != 0 )
  {
    switch ( reply_result )
    {
    case CONFIG_ACK:
      reply_bp->used = 0;
      reply_result = CONFIG_NAK;
      /* fallthru */

    case CONFIG_NAK:
      fsm_makeoptions( fsm_ptr, fsm_ptr->remote.want_pdv,
          &reply_bp, desired );
      fsm_ptr->retry_nak--;
      break;

    case CONFIG_REJ:
      /* do nothing */
      break;
    };
  }
  else if ( reply_result == CONFIG_NAK )
  {
    /* if too many NAKs, reject instead */
    if ( fsm_ptr->retry_nak > 0 )
    {
      fsm_ptr->retry_nak--;
    }
    else
    {
      reply_result = CONFIG_REJ;
    }
  }

  if (reply_result == CONFIG_ACK)
  {
    fsm_irc_tx_ack(fsm_ptr);
  }

  /* Send ACK/NAK/REJ to remote host */
  fsm_send(fsm_ptr, reply_result, config->identifier, reply_bp);
  dsm_free_packet(&bp);
  return (reply_result != CONFIG_ACK);

} /* fsm_request() */



/*===========================================================================
FUNCTION FSM_ACK()

DESCRIPTION
  This function will Process configuration ACK sent by remote host - this is
  done by comparing the last request with this ACK.

DEPENDENCIES
  None

RETURN VALUE
  None

SIDE EFFECTS
  None
===========================================================================*/
static int fsm_ack
(
  ppp_fsm_type      *fsm_ptr,                  /* pointer to the fsm block */
  struct config_hdr *config_hdr,     /* pointer to the header for this ACK */
  dsm_item_type     *item_ptr    /* pointer to the dsm_item containing ACK */
)
{
  dsm_item_type *request_item_ptr; /* item pointer for the original request*/
  int ret_val = 0;                              /* was an error generated? */
  sint15 req_char;    /* character from request for comparison to ack_char */ 
  sint15 ack_char;  /* character from the ack for comparison with req_char */
  /* - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - */

  /*-------------------------------------------------------------------------
    ID field must match last request we sent
  -------------------------------------------------------------------------*/
  if (config_hdr->identifier != fsm_ptr->lastid)
  {
    dsm_free_packet(&item_ptr);
    return -1;
  }

  /*-------------------------------------------------------------------------
    Get a copy of last request we sent
  -------------------------------------------------------------------------*/
  request_item_ptr = fsm_makereq(fsm_ptr);

  /*-------------------------------------------------------------------------
    Overall buffer length should match
  -------------------------------------------------------------------------*/
  if (config_hdr->length != dsm_length_packet(request_item_ptr))
  {
    ret_val = -1;
  }
  else
  {
    /*-----------------------------------------------------------------------
      Make sure Each byte matches, but first dup one of the packets so that
      there is something to compare.
    -----------------------------------------------------------------------*/
    while((req_char = dsm_pull8(&request_item_ptr)) != -1)
    {
      if ((ack_char = dsm_pull8(&item_ptr)) == -1 || ack_char != req_char )
      {
        ret_val = -1;;
        break;
      }
    }
  }

  dsm_free_packet(&request_item_ptr);
  dsm_free_packet(&item_ptr);

  return ret_val;

} /* fsm_ack() */



/*===========================================================================
FUNCTION FSM_NAK()

DESCRIPTION
  This function will Process configuration NAK sent by remote host

DEPENDENCIES
  None

RETURN VALUE
  None

SIDE EFFECTS
  None
===========================================================================*/
static int fsm_nak
(
  ppp_fsm_type      *fsm_ptr,
  struct config_hdr *config,
  dsm_item_type     *bp
)
{
  sint31 signed_length = config->length;
  struct option_hdr option;
  int last_option = 0;
  int result;
  /* - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - */

  /* ID field must match last request we sent */
  if (config->identifier != fsm_ptr->lastid)
  {
    dsm_free_packet(&bp);
    return -1;
  }

  /* First, process in order.  Then, process extra "important" options */
  while (signed_length > 0  &&  ntohopt(&option, &bp) != -1)
  {
    if ((signed_length -= option.length) < 0)
    {
      dsm_free_packet(&bp);
      return -1;
    }
    if ( option.type > fsm_ptr->pdc->option_limit )
    {
      /* option is allowed, but ignored */
      continue;
    }
    else if ( !(fsm_ptr->local.work & (1 << fsm_n_option(fsm_ptr->pdc, 
                             option.type))))
    {
      if ( fsm_ptr->local.will & 
           (1 << fsm_n_option(fsm_ptr->pdc, option.type)) )
      {
        fsm_ptr->local.work |= 
           (1 << fsm_n_option(fsm_ptr->pdc, option.type));
      }
      last_option = fsm_ptr->pdc->option_limit + 1;
    }
    else if ( option.type < last_option )
    {
      /* assumes options are always in numerical order */
      dsm_free_packet(&bp);
      return -1;
    }
    else
    {
      last_option = option.type;
    }
    if ( (result =
          (*fsm_ptr->pdc->check)( fsm_ptr, &option,
        &bp, FALSE )) < 0 )
    {
      dsm_free_packet(&bp);
      return -1;
    }
    /* update the negotiation status */
    if ( result == CONFIG_REJ )
    {
      fsm_ptr->local.work &= ~(1 << fsm_n_option(fsm_ptr->pdc, 
                           option.type));
    }
  }
  dsm_free_packet(&bp);
  return 0;

} /* fsm_nak() */



/*===========================================================================
FUNCTION FSM_REJECT()

DESCRIPTION
  This function will Process configuration Reject sent by remote host

DEPENDENCIES
  None

RETURN VALUE
  None

SIDE EFFECTS
  None
===========================================================================*/
static int fsm_reject
(
  ppp_fsm_type      *fsm_ptr,
  struct config_hdr *config,
  dsm_item_type     *bp
)
{
  sint31 signed_length = config->length;
  struct option_hdr option;
  int last_option = 0;
  /* - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - */

  /* ID field must match last request we sent */
  if (config->identifier != fsm_ptr->lastid)
  {
    dsm_free_packet(&bp);
    return -1;
  }

  /* Process in order, checking for errors */
  while (signed_length > 0  &&  ntohopt(&option, &bp) != -1)
  {
    register int k;

    if ((signed_length -= option.length) < 0)
    {
      dsm_free_packet(&bp);
      return -1;
    }

    if ( option.type > fsm_ptr->pdc->option_limit )
    {
      dsm_free_packet(&bp);
      return -1;
    }
    else if ( !(fsm_ptr->local.work & (1 << fsm_n_option(fsm_ptr->pdc, 
                               option.type))))
    {
      dsm_free_packet(&bp);
      return -1;
    }
    else if ( option.type < last_option )
    {
      /* assumes options are always in numerical order */
      dsm_free_packet(&bp);
      return -1;
    }

    /* currently, contents of options are not checked.
     * just check that the data is all there.
     */
    k = option.length - OPTION_HDR_LEN;
    if ( dsm_pullup( &bp, NULL, k ) != k )
    {
      dsm_free_packet(&bp);
      return -1;
    }
    if ( (*fsm_ptr->pdc->reject)(fsm_ptr, &option) != 0 )
    {
      dsm_free_packet(&bp);
      return -1;
    }

    fsm_ptr->local.work &= ~(1 << fsm_n_option(fsm_ptr->pdc, 
                         option.type));
  }

  dsm_free_packet(&bp);
  return 0;

} /* fsm_reject() */



/*===========================================================================
FUNCTION FSM_PROC()

DESCRIPTION
  This function will process the incoming PPP packet

DEPENDENCIES
  None

RETURN VALUE
  None

SIDE EFFECTS
  None
===========================================================================*/
void fsm_proc
(
  ppp_fsm_type  *fsm_ptr,
  dsm_item_type *bp
)
{
  struct config_hdr hdr;
  uint16 rejected;
  ppp_type *ppp_cb_ptr;
  ppp_protocol_e_type protocol;

  /* - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - */

  if( bp == NULL )
  {
    MSG_ERROR("Invalid dsm", 0, 0, 0);
    return;
  }

  if ( fsm_ptr == NULL || 
       fsm_ptr->pdc == NULL || 
       fsm_ptr->device >= PPP_MAX_DEV )
  {
    MSG_ERROR("Invalid args", 0, 0, 0 ); 
    dsm_free_packet( &bp);
    return;
  }

  ppp_cb_ptr = &(ppp_cb_array[fsm_ptr->device]);

  if(ntohcnf(&hdr, &bp) == -1)
  {
    ERR("if%d Short pkt st=%d proto=0x%x", 
        fsm_ptr->device,
        fsm_ptr->state,
        fsm_ptr->pdc->protocol);
    dsm_free_packet( &bp);
    return;
  }

  hdr.length -= CONFIG_HDR_LEN;    /* Length includes envelope */
  dsm_trim_packet(&bp, hdr.length);        /* Trim off padding */

  MSG_MED("fsm_proc(dev %d state %d) for msg %d",
          fsm_ptr->device,
          fsm_ptr->state,
          hdr.code);

  switch(hdr.code)
  {
  case CONFIG_REQ:
    switch(fsm_ptr->state)
    {
    case fsmOPENED:
      protocol = (ppp_protocol_e_type)(fsm_ptr - ppp_cb_ptr->fsm);

      TASKLOCK();
      /*--------------------------------------------------------------------
        The mobile received a LCP/IPCP C_REQ in the OPENED state. This
        indicates a PPP resync - if there is a callback registered call it.
      --------------------------------------------------------------------*/
      if(ppp_cb_ptr->event_info[PPP_RESYNC_EV].cback != NULL)
      {
        MSG_MED ("PPP resync from network",0,0,0);
        ppp_cb_ptr->event_info[PPP_RESYNC_EV].cback(
          fsm_ptr->device,
          protocol,
          PPP_RESYNC_EV,
          ppp_cb_ptr->event_info[PPP_RESYNC_EV].user_data,
          ppp_cb_ptr->session_handle,
          PPP_FAIL_REASON_NONE
          );
      }
      TASKFREE();

      (*fsm_ptr->pdc->closing)(fsm_ptr);
      (*fsm_ptr->pdc->starting)(fsm_ptr);
      fsm_irc(fsm_ptr);
      /*---------------------------------------------------------------------
        We are resync'ing so if the timer is valid and the timeout is set,
        start the setup timer.
      ---------------------------------------------------------------------*/
      if(ppp_cb_ptr->setup_timer != PS_TIMER_FAILURE &&
         ppp_cb_ptr->setup_timeout > 0)
      {
        ps_timer_start(ppp_cb_ptr->setup_timer,
                       ppp_cb_ptr->setup_timeout);
      }
      fsm_configreq(fsm_ptr);
      fsm_ptr->state =
        ((fsm_request(fsm_ptr, &hdr, bp) == 0) ? fsmACK_Sent : fsmREQ_Sent);
      break;

    case fsmSTARTING:  /* can't happen? */
    case fsmSTOPPED:
      fsm_irc(fsm_ptr);
      (*fsm_ptr->pdc->starting)(fsm_ptr);
      fsm_configreq(fsm_ptr);
      /* fallthru */

    case fsmREQ_Sent:
    case fsmACK_Sent:
      fsm_ptr->state =
      (fsm_request(fsm_ptr, &hdr, bp) == 0)
        ? fsmACK_Sent : fsmREQ_Sent;
      break;

    case fsmACK_Rcvd:
      if (fsm_request(fsm_ptr, &hdr, bp) == 0)
      {
        fsm_opening(fsm_ptr);
      }
      break;

    case fsmINITIAL:  /* can't happen? */
    case fsmCLOSED:
      /* Don't accept any connections */
      fsm_termack(fsm_ptr, hdr.identifier);
      /* fallthru */

    case fsmCLOSING:
    case fsmSTOPPING:
      /* We are attempting to close connection; */
      /* wait for timeout to resend a Terminate Request */
      dsm_free_packet(&bp);
      break;
    };
    break;

  case CONFIG_ACK:
    switch(fsm_ptr->state)
    {
    case fsmREQ_Sent:
      if (fsm_ack(fsm_ptr, &hdr, bp) == 0)
      {
        fsm_irc_rx_ack(fsm_ptr);
        fsm_ptr->state = fsmACK_Rcvd;
      }
      break;

    case fsmACK_Sent:
      if (fsm_ack(fsm_ptr, &hdr, bp) == 0)
      {
        fsm_irc(fsm_ptr);
        fsm_opening(fsm_ptr);
      }
      break;

    case fsmOPENED:   /* Unexpected event? */
      (*fsm_ptr->pdc->closing)(fsm_ptr);
      (*fsm_ptr->pdc->starting)(fsm_ptr);
      /* fallthru */

    case fsmACK_Rcvd:  /* Unexpected event? */
      dsm_free_packet(&bp);
      fsm_configreq(fsm_ptr);
      fsm_ptr->state = fsmREQ_Sent;
      break;

    case fsmINITIAL:  /* can't happen? */
    case fsmSTARTING:  /* can't happen? */
    case fsmCLOSED:
    case fsmSTOPPED:
      /* Out of Sync */
      fsm_termack(fsm_ptr, hdr.identifier);
      /* fallthru */

    case fsmCLOSING:
    case fsmSTOPPING:
      /* We are attempting to close connection; */
      /* wait for timeout to resend a Terminate Request */
      dsm_free_packet(&bp);
      break;
    };
    break;

  case CONFIG_NAK:
    switch(fsm_ptr->state)
    {
    case fsmREQ_Sent:
    case fsmACK_Sent:
      /* Update our request to reflect NAKed options */
      if ( fsm_nak(fsm_ptr, &hdr, bp) == 0 )
      {
        fsm_irc(fsm_ptr);
        /* Send updated config request */
        fsm_configreq(fsm_ptr);
        /* stay in same state */
      }
      break;

    case fsmOPENED:   /* Unexpected event? */
      (*fsm_ptr->pdc->closing)(fsm_ptr);
      (*fsm_ptr->pdc->starting)(fsm_ptr);
      /* fallthru */

    case fsmACK_Rcvd:  /* Unexpected event? */
      dsm_free_packet(&bp);
      fsm_configreq(fsm_ptr);
      fsm_ptr->state = fsmREQ_Sent;
      break;

    case fsmINITIAL:  /* can't happen? */
    case fsmSTARTING:  /* can't happen? */
    case fsmCLOSED:
    case fsmSTOPPED:
      /* Out of Sync */
      fsm_termack(fsm_ptr, hdr.identifier);
      /* fallthru */

    case fsmCLOSING:
    case fsmSTOPPING:
      /* We are attempting to close connection; */
      /* wait for timeout to resend a Terminate Request */
      dsm_free_packet(&bp);
      break;
    };
    break;

  case CONFIG_REJ:
    switch(fsm_ptr->state)
    {
    case fsmREQ_Sent:
    case fsmACK_Sent:
      /* Update our request to reflect REJECTed options */
      if ( fsm_reject(fsm_ptr, &hdr, bp) == 0 )
      {
        fsm_irc(fsm_ptr);
        /* Send updated config request */
        fsm_configreq(fsm_ptr);
        /* stay in same state */
      }
      break;

    case fsmOPENED:   /* Unexpected event? */
      (*fsm_ptr->pdc->closing)(fsm_ptr);
      (*fsm_ptr->pdc->starting)(fsm_ptr);
      /* fallthru */

    case fsmACK_Rcvd:  /* Unexpected event? */
      dsm_free_packet(&bp);
      fsm_configreq(fsm_ptr);
      fsm_ptr->state = fsmREQ_Sent;
      break;

    case fsmINITIAL:  /* can't happen? */
    case fsmSTARTING:  /* can't happen? */
    case fsmCLOSED:
    case fsmSTOPPED:
      /* Out of Sync */
      fsm_termack(fsm_ptr, hdr.identifier);
      /* fallthru */

    case fsmCLOSING:
    case fsmSTOPPING:
      /* We are attempting to close connection; */
      /* wait for timeout to resend a Terminate Request */
      dsm_free_packet(&bp);
      break;
    };
    break;

  case TERM_REQ:
    switch(fsm_ptr->state)
    {
    case fsmOPENED:
      if (LCP == fsm_ptr->pdc->fsmi && pppAUTHENTICATE == ppp_cb_ptr->phase)
      {
        fsm_ptr->fail_reason = PPP_FAIL_REASON_AUTH_FAILURE;
      }
      (*fsm_ptr->pdc->closing)(fsm_ptr);
      fsm_zrc(fsm_ptr);
      fsm_termack(fsm_ptr, hdr.identifier);
      fsm_ptr->state = fsmSTOPPING;
      break;

    case fsmACK_Rcvd:
    case fsmACK_Sent:
      fsm_ptr->state = fsmREQ_Sent;
      /* fallthru */

    case fsmREQ_Sent:
    case fsmCLOSING:
    case fsmSTOPPING:
      /* waiting for timeout */
      /* fallthru */

    case fsmINITIAL:  /* can't happen? */
    case fsmSTARTING:  /* can't happen? */
    case fsmCLOSED:
    case fsmSTOPPED:
      /* Unexpected, but make them happy */
      fsm_termack(fsm_ptr, hdr.identifier);
      break;
    };
    dsm_free_packet(&bp);
    break;

  case TERM_ACK:
    switch(fsm_ptr->state)
    {
    case fsmCLOSING:
      ps_timer_cancel( fsm_ptr->timer );
      (*fsm_ptr->pdc->stopping)(fsm_ptr);
      fsm_ptr->state = fsmCLOSED;
      break;

    case fsmSTOPPING:
      ps_timer_cancel(fsm_ptr->timer );
      (*fsm_ptr->pdc->stopping)(fsm_ptr);
      fsm_ptr->state = fsmSTOPPED;
      break;

    case fsmOPENED:
      /* Remote host has abruptly closed connection */
      (*fsm_ptr->pdc->closing)(fsm_ptr);
      (*fsm_ptr->pdc->starting)(fsm_ptr);
      fsm_configreq(fsm_ptr);
      fsm_ptr->state = fsmREQ_Sent;
      break;

    case fsmACK_Rcvd:
      fsm_ptr->state = fsmREQ_Sent;
      /* fallthru */

    case fsmREQ_Sent:
    case fsmACK_Sent:
      /* waiting for timeout */
      /* fallthru */

    case fsmINITIAL:  /* can't happen? */
    case fsmSTARTING:  /* can't happen? */
    case fsmCLOSED:
    case fsmSTOPPED:
      /* Unexpected, but no action needed */
      break;
    };
    dsm_free_packet(&bp);
    break;

  case CODE_REJ:
    /* In general, treat all rejects as if they are benign */
    switch(fsm_ptr->state)
    {
    case fsmACK_Rcvd:
      fsm_ptr->state = fsmREQ_Sent;
      break;
    };
    dsm_free_packet(&bp);
    break;

  case PROT_REJ:
    /* In general, treat all rejects as if they are benign
     */
    rejected = dsm_pull16( &bp );

    switch ( rejected )
    {
    case PPP_IPCP_PROTOCOL:
      fsm_down( &(ppp_cb_ptr->fsm[IPcp]) );
      break;
    };
    switch(fsm_ptr->state)
    {
    case fsmACK_Rcvd:
      fsm_ptr->state = fsmREQ_Sent;
      break;
    };
    dsm_free_packet(&bp);
    break;

  case ECHO_REQ:
    switch(fsm_ptr->state)
    {
    case fsmOPENED:
      fsm_send( fsm_ptr, ECHO_REPLY, hdr.identifier, bp );
      break;

    default:
      /* ignore */
      dsm_free_packet(&bp);
      break;
    };
    break;

  case ECHO_REPLY:
  case DISCARD_REQ:
  case IDENTIFICATION:
  case TIME_REMAINING:
    dsm_free_packet(&bp);
    break;

  default:
    hdr.length += CONFIG_HDR_LEN;  /* restore length */
    bp = htoncnf( &hdr, bp );  /* put header back on */
    if (bp != NULL)
    {
      fsm_send( fsm_ptr, CODE_REJ, hdr.identifier, bp );
    }
    break;
  };

  MSG_MED("leaving fsm_proc(state = %d)", fsm_ptr->state, 0, 0);

} /* fsm_proc() */



/*===========================================================================
FUNCTION FSM_TIMEOUT()

DESCRIPTION
  This function will Timeout while waiting for reply from remote host

DEPENDENCIES
  None

RETURN VALUE
  TRUE:  if it has not given up - may try another send...
  FALSE: if it has given up

SIDE EFFECTS
  None
===========================================================================*/
boolean fsm_timeout
(
  ppp_fsm_type *fsm_ptr
)
{
  /* - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - */

  switch(fsm_ptr->state)
  {
  case fsmREQ_Sent:
  case fsmACK_Sent:
    if (fsm_ptr->retry_request > 0)
    {
      fsm_configreq(fsm_ptr);
      return TRUE;
    }
    else
    {
      MSG_LOW( "Request Retry Exceeded, Stopping LCP", 0, 0, 0);
      (*fsm_ptr->pdc->stopping)(fsm_ptr);
      fsm_ptr->state = fsmSTOPPED;
      return FALSE;
    }

  case fsmACK_Rcvd:
    if (fsm_ptr->retry_request > 0)
    {
      fsm_configreq(fsm_ptr);
      fsm_ptr->state = fsmREQ_Sent;
      return TRUE;
    }
    else
    {
      MSG_LOW( "Request Retry Exceeded, Stopping LCP", 0, 0, 0);
      (*fsm_ptr->pdc->stopping)(fsm_ptr);
      fsm_ptr->state = fsmSTOPPED;
      return FALSE;
    }

  case fsmCLOSING:
    if (fsm_ptr->retry_request > 0)
    {
      /*---------------------------------------------------------------------
        Timeout after tx Term-Req.  Haven't hit LCP_TERM_TRY's, so try again
      ---------------------------------------------------------------------*/
      fsm_termreq(fsm_ptr);
      return TRUE;
    }
    else
    {
      /*---------------------------------------------------------------------
      The mobile timed out while expecting a T-Ack. Give up on PPP and call
      LCP stopping.
      ---------------------------------------------------------------------*/
      MSG_LOW( "Terminate Retry Exceeded, Stopping LCP", 0, 0, 0);
      MSG_MED("if%d CLOSING timeout; PPP Done", fsm_ptr->device, 0, 0);
      (*fsm_ptr->pdc->stopping)(fsm_ptr);
      fsm_ptr->state = fsmCLOSED;
      return FALSE;
    }

  case fsmSTOPPING:
    /*-----------------------------------------------------------------------
      The mobile timed out after sending a T-ACK in response to the peer's
      T-Req.
    -----------------------------------------------------------------------*/
      MSG_MED( "if%d STOPPING timeout: PPP Done", fsm_ptr->device, 0, 0);
      (*fsm_ptr->pdc->stopping)(fsm_ptr);
      fsm_ptr->state = fsmSTOPPED;
      return FALSE;

  case fsmOPENED:
    /*-----------------------------------------------------------------------
      Timer expired but PPP protocol is now opened (This can happen, there is
      a very small timing window). Do not perform any work as the protocol
      is now Opened.
    -----------------------------------------------------------------------*/
    MSG_HIGH( "Timer Expired, PPP prtcl Open", 0, 0, 0);
    return TRUE;

  default:  /* can't happen */
    ERR_FATAL( "Invalid PPP State", 0, 0, 0);
    return FALSE;
  }
} /* fsm_timeout() */


/*===========================================================================
FUNCTION FSM_PASSIVE_WAKEUP()

DESCRIPTION
  This function will perform the lower layer Up Event for the case when the
  FSM is in passive mode. It will force the fsm to send a config request to
  the peer (ie: wakeup in active mode)

DEPENDENCIES
  None

RETURN VALUE
  None

SIDE EFFECTS
  None
===========================================================================*/
void fsm_passive_wakeup
(
  ppp_fsm_type *fsm_ptr
)
{
  if ( fsm_ptr->pdc == NULL || fsm_ptr->mode != PPP_CP_PASSIVE)
  {
    return;
  }

  switch ( fsm_ptr->state )
  {
  case fsmSTARTING:
    fsm_irc(fsm_ptr);
    (*fsm_ptr->pdc->starting)(fsm_ptr);
    fsm_configreq(fsm_ptr);
    fsm_ptr->state = fsmREQ_Sent;
    break;

  default:
    /* Do nothing */
    break;
  }
} /* fsm_passive_wakeup() */



/*===========================================================================
FUNCTION FSM_UP()

DESCRIPTION
  This function will perform the lower layer Up Event

DEPENDENCIES
  None

RETURN VALUE
  None

SIDE EFFECTS
  None
===========================================================================*/
void fsm_up
(
  ppp_fsm_type *fsm_ptr
)
{
  /* - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - */

  if ( fsm_ptr->pdc == NULL || fsm_ptr->mode == PPP_CP_PASSIVE )
  {
    return;
  }

  MSG_MED("fsm_up(device %d)", fsm_ptr->device, 0, 0);
  switch ( fsm_ptr->state )
  {
  case fsmINITIAL:
    (*fsm_ptr->pdc->starting)(fsm_ptr);
    fsm_ptr->state = fsmCLOSED;
    break;

  case fsmSTARTING:
    fsm_irc(fsm_ptr);
    (*fsm_ptr->pdc->starting)(fsm_ptr);
    fsm_configreq(fsm_ptr);
    fsm_ptr->state = fsmREQ_Sent;
    break;

  default:
    /* already up */
    break;
  };
} /* fsm_up() */



/*===========================================================================
FUNCTION FSM_DOWN()

DESCRIPTION
  This function will perform the Lower Layer Down Event

DEPENDENCIES
  None

RETURN VALUE
  None

SIDE EFFECTS
  None
===========================================================================*/
void fsm_down
(
  ppp_fsm_type *fsm_ptr
)
{
  /* - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - */

  if ( fsm_ptr->pdc == NULL )
  {
    return;
  }

  switch ( fsm_ptr->state )
  {
  case fsmCLOSED:
    fsm_ptr->state = fsmINITIAL;
    break;

  case fsmSTOPPED:
    /*TLS*/
    fsm_ptr->state = fsmSTARTING;
    break;

  case fsmCLOSING:
    ps_timer_cancel( fsm_ptr->timer );
    fsm_ptr->state = fsmINITIAL;
    break;

  case fsmSTOPPING:
  case fsmREQ_Sent:
  case fsmACK_Rcvd:
  case fsmACK_Sent:
    ps_timer_cancel( fsm_ptr->timer );
    fsm_ptr->state = fsmSTARTING;
    break;

  case fsmOPENED:
    (*fsm_ptr->pdc->closing)(fsm_ptr);
    fsm_ptr->state = fsmSTARTING;
    break;

  default:
    /* already down */
    break;
  };
} /* fsm_down() */



/*===========================================================================
FUNCTION FSM_OPEN()

DESCRIPTION
  This function will Administratively Open the link

DEPENDENCIES
  None

RETURN VALUE
  None

SIDE EFFECTS
  None
===========================================================================*/
void fsm_open
(
  ppp_fsm_type *fsm_ptr
)
{
  /* - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - */

  MSG_MED("fsm_open(dev %d  state %d)",fsm_ptr->device,fsm_ptr->state,0);
  if ( fsm_ptr->pdc == NULL )
  {
    return;
  }

  switch ( fsm_ptr->state )
  {
  case fsmINITIAL:
    /*TLS*/
    fsm_ptr->state = fsmSTARTING;
    break;

  case fsmCLOSED:
    fsm_irc(fsm_ptr);
    (*fsm_ptr->pdc->starting)(fsm_ptr);
    fsm_configreq(fsm_ptr);
    fsm_ptr->state = fsmREQ_Sent;
    break;

  case fsmCLOSING:
    fsm_ptr->state = fsmSTOPPING;
    break;

  default:
    /* already open */
    break;
  };
} /* fsm_open() */



/*===========================================================================
FUNCTION FSM_CLOSE()

DESCRIPTION
  This function will Administratively Close the link

DEPENDENCIES
  None

RETURN VALUE
  None

SIDE EFFECTS
  None
===========================================================================*/
void fsm_close
(
  ppp_fsm_type *fsm_ptr
)
{
  /* - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - */

  if ( fsm_ptr->pdc == NULL )
  {
    return;
  }

  switch ( fsm_ptr->state )
  {
  case fsmSTARTING:
    (*fsm_ptr->pdc->stopping)(fsm_ptr);
    fsm_ptr->state = fsmINITIAL;
    break;

  case fsmSTOPPED:
    fsm_ptr->state = fsmCLOSED;
    break;

  case fsmSTOPPING:
    fsm_ptr->state = fsmCLOSING;
    break;

  case fsmOPENED:
    (*fsm_ptr->pdc->closing)(fsm_ptr);
    /* fallthru */
  case fsmREQ_Sent:
  case fsmACK_Rcvd:
  case fsmACK_Sent:
    fsm_ptr->retry_request = fsm_ptr->try_terminate;
    fsm_termreq(fsm_ptr);
    fsm_ptr->state = fsmCLOSING;
    break;

  default:
    /* already closed */
    break;
  };
} /* fsm_close() */



/*===========================================================================
FUNCTION FSM_INIT()

DESCRIPTION
  This function will Initialize the fsm for this protocol

DEPENDENCIES
  None

RETURN VALUE
  None

SIDE EFFECTS
  None
===========================================================================*/
void fsm_init
(
  ppp_fsm_type *fsm_ptr
)
{
  /* - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - */

  fsm_ptr->try_configure = fsm_ptr->pdc->try_configure;
  fsm_ptr->try_nak = fsm_ptr->pdc->try_nak;
  fsm_ptr->try_terminate = fsm_ptr->pdc->try_terminate;

  fsm_ptr->fail_reason = PPP_FAIL_REASON_NONE;
} /* fsm_init() */



/*===========================================================================
FUNCTION FSM_FREE()

DESCRIPTION
  This function is used to free the fsm data structure.

RETURN VALUE
  None

DEPENDENCIES
  None

SIDE EFFECTS
  None
===========================================================================*/
void fsm_free
(
  ppp_fsm_type *fsm_ptr
)
{
  /* - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - */

  if (fsm_ptr->pdc != NULL)
  {
    if (fsm_ptr->pdc->free != NULL)
    {
      (*fsm_ptr->pdc->free)(fsm_ptr);
    }
    fsm_ptr->pdc = NULL;
  }
} /* fsm_free() */

#endif /* FEATURE_DATA_PS */
