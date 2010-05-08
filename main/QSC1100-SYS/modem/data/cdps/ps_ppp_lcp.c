/*====*====*====*====*====*====*====*====*====*====*====*====*====*====*====*

                           P S _ P P P _ L C P . C

GENERAL DESCRIPTION
  This module contains the PPP protocol suite LCP (link control protocol)
  functions. The module contains the logic to perform the PPP LCP protocol.

EXTERNALIZED FUNCTIONS
  lcp_save_work_opt()
    This function stores the value of the LCP option that is passed in. It is
    being saved into the work and work_pdv the want values are saved by the
    caller.
  lcp_init()
    This function will initialize the LCP configuration structure

INITIALIZATION AND SEQUENCING REQUIREMENTS

  Copyright 1992-1994 William Allen Simpson
    Licensed to Qualcomm -- 95 Jan 21

  Copyright (c) 1995-2009 QUALCOMM Incorporated. 
  All Rights Reserved.
  Qualcomm Confidential and Proprietary
*====*====*====*====*====*====*====*====*====*====*====*====*====*====*====*/


/*===========================================================================

                        EDIT HISTORY FOR MODULE

  $PVCSPath: MM_DATA/ps_ppp_lcp.c_v   1.12   28 Feb 2003 11:12:56 ubabbar  $
  $Header: //source/qcom/qct/modem/data/cdps/ps/main/lite/src/ps_ppp_lcp.c#5 $
  $Author: nsivakum $ $DateTime: 2009/05/27 05:07:18 $

when        who    what, where, why
--------    ---    ----------------------------------------------------------
04/29/09    sn     Ported support for call throttle feature (DCTM).
06/28/08    pp     Fixed RVCT compiler warnings.
02/22/08    pp     Klocwork fixes.
02/21/08    sn     Merged lint error fix.
04/16/03    mvl    Resetting auth mode to none if C-Rej for auth received.
04/08/03    mvl    Added support to do server style authentication - fixed
                   event ordering (LCP up was after Auth starting).
04/02/03    mvl    Complete modularization of the authentication code.
03/26/03    mvl    removed authentication featurization
03/17/03    js     Added the prefix PS_PPP_EVENT_ to ppp_event_state_type.
03/07/03    js     Added LCP event posting.
02/06/03    mvl    Removed check if resyncing when timer expires, and
                   simplified the timeout hanling.
02/06/03    usb    fixed compilation error by removing LOCAL from variables
                   used outside this file
11/19/02    jd     Fixed compile error for MIP_PERF
11/04/02    mvl    Removed non-auth versions of lcp_opening(). And fixed
                   MTU/MRU mixup.
09/18/02    mvl    Added user_data to ppp event callbacks.
09/12/02    mvl    Mainlined some netmodel code, generified the network model
                   resync callcack.  Removed netmodel featurization.
09/06/02    mvl    fixed assert()s.
08/13/02    mvl    Session handle support and using MTU/MRU from ps iface.
08/06/02    mvl    Reworked authentication.
07/31/02    usb    Renamed psmisc.h to ps_utils.h
07/30/02    mvl    Put TASKLOCK() around invokation of event callbacks.
                   Replaced ppp_stop() with ppp_abort() in timeout functions.
07/25/02    mvl    Removed netmodel dependency.  Updates for PPP renaming.
07/24/02    mvl    Fixed PPP termination bug.  Now when timers expire not
                   talking to DS, but calling ppp_stop().
07/01/02    mvl    Support for new architecture, generating event callbacks.
05/22/02    mvl    Removed non-netmodel ppp_outgoing() calls.
04/17/02    rc     Wrapped code in !FEATURE_DATA_WCDMA_PS as appropriate to
                   build for all targets.
03/04/02    dwp    Move inclusion dsiface.h out of MIP feature.
03/01/02    vas    Updated to allow authentication (PAP/CHAP) parameters to
                   be allocated per iface.
03/01/02    vsk    removed dsm_free_packet() being called in case of
                   ppp_outgoing() returns an error
02/20/02    sjy    Mobile IP performance now featurized under
                   FEATURE_DS_MOBILE_IP_PERF
02/12/02    dwp    Cleanup header inclusion(s) for non DS builds.
02/07/02    sjy    Added support for Mobile IP call performance metrics.
02/05/02    vr     Modified ppp_ready to not do IPCP if configured so
12/18/01    rsl    Updated tracer field.
12/01/01    vas    Changed function prototypes to take the ppp iface as a
                   parameter instead of being hardcoded. Made ppp_do_auth
                   part of ppp ctl blk. lcp_chap_info maintained per iface.
11/12/01    dwp    Add "|| FEATURE_DATA_PS" to whole module, wrap old/new DS
                   Mgr dependencies w/FEATURE_DATA (namely PS to DS cmd i/f)
11/09/01    jay    Reject authentication if we are in a qnc call
10/29/01    igt    Force the ppp state to fsmINITIAL in lcp_init()
09/25/01    snn    Modified some messages in PAP/CHAP area to give more
                   information.
09/20/01    mvl    Reset the value of remote.work to 0 in starting() - to
                   ensure that we don't reject the othe guys desires.
09/13/01    mvl    Modified message to indicate that unsupported option was
                   negotiated.  Added copy of want to work in starting.
08/29/01  pjb/mvl  Modified the values of option bits to support DNS options.
08/19/01    sjy    Referenced the correct LCP configuration values in
                   lcp_powerup_init() for when netmodel feature is not
                   defined
08/15/01    ss     Removed some lines not needed in lcpi_timer_cb().
08/02/01  na/mvl   Fixed functions that were using the global ppp_cb_ptr,
                   general code cleanup
07/30/01    mvl    Auth option is always included in the lcp_accept var
                   because it is defined to 0 if one of the auth features is
                   not defined
                   Completely revamped the initialization so that IPCP can be
                   initialized per interface and with passed in options, or
                   the defaults.
07/29/01    na     lcp_stopping() now calls dsiface_down().
07/27/01    mvl    Modifications to use PS timers, checnged the retry timer
                   to not multiply the header value by 1000.
07/24/01    mvl    compiling out parts of fsm_constants_initializer struct to
                   conserve memory, initing fsm.iface field, changed
                   ppp_close() to pass iface param if MIP is defined and
                   removed obsolete debug macros.
07/23/01    pjb    Return type of dsm_pushdown changed. Change location/
                   prefix of command names.  Changed call to
                   dsm_offset_new_buffer to reserve space for header.
                   Fixed potential memory leak.
07/06/01    mvl    Added Mobile IP support - call dsiface_starting() when LCP
                   starts.
06/08/01    mvl    Added Mobile IP support (reject authentication if in
                   Mobile IP mode) and made formatting changes.
08/25/00    rc     Changed dsm_new_buffer() calls to take pool id as input
                   parameter. So, if pool size is to be passed in as
                   parameter, a new macro DSM_DS_POOL_SIZE is used to return
                   pool id based on pool size.
                   Changed FEATURE_DS_MEM_CHK to FEATURE_DSM_MEM_CHK.
07/22/00    snn    Initialized ppp_chap_retry_counter before every CHAP call.
05/09/00    rc     Added code in lcp_init() so that authentication is not
                   done in async/fax calls.
05/04/00    mvl    Changed calls to ppp_outgoing() to reflect new interface.
03/10/00    snn    Added Support for CHAP. They have been added under the
                   feature DS_CHAP feature
10/28/99    mvl    Added code review changes
09/27/99    snn    Fixed commenting style and added comments.
08/26/99    ak     Fixed warnings for surfc and surfp builds.  msg.h and
                   err.h now included judiciously.
08/20/99    mvl    Added the lcp_save_work_opt() function - used to be in
                   ps_ppp_netmodel.c
08/17/99    smp    Added include files msg.h & err.h.
08/10/99    snn    Added support for PAP timeouts.
08/09/99    mvl    Added network model changes.
07/09/99    snn    Added support for PAP implementation
04/08/99    na     Changed PPP_LCP_PROTOCOL to DSM_LCP_PROTOCOL where the
                   protocol was used in the kind field.
01/27/98    ldg    Removed the DS_SLIM switch.
01/26/98    fkm    FEATURE_xxx Updates (via Automatic Scripts)
07/22/97    na     Added support for PAP - under DS_PAP feature.
06/25/97    jgr    Added ifdef FEATURE_DS over whole file
05/24/96    jjw    Gemini & ISS2 Data Services release.
11/09/95    jjw    IS-99 Compliant Release
===========================================================================*/


/*===========================================================================

                     INCLUDE FILES FOR MODULE

===========================================================================*/

#include "comdef.h"
#include "customer.h"       /* Customer Specific Features */
#ifdef FEATURE_DATA_PS
#include "assert.h"
#include "dsbyte.h"
#include "dsm.h"
#include "ps_ppp.h"
#include "ps_pppi.h"
#include "ps_ppp_fsm.h"
#include "ps_ppp_lcp.h"
#include "ps_ppp_auth.h"
#include "pstimer.h"
#include "ps_ppp_task.h"
#include "ps_utils.h"
#include "err.h"
#include "msg.h"

#if defined(FEATURE_DS_MOBILE_IP) && defined(FEATURE_DS_MOBILE_IP_PERF)
#include "dsmip_perf.h"
#endif /* FEATURE_DS_MOBILE_IP && FEATURE_DS_MOBILE_IP_PERF */

#include "ps_ppp_events.h" 

/*===========================================================================

            GLOBAL DEFINITIONS AND DECLARATIONS FOR MODULE

===========================================================================*/
lcp_value_type lcp_local_want_cb[PPP_MAX_DEV];
lcp_value_type lcp_local_work_cb[PPP_MAX_DEV];
lcp_value_type lcp_remote_want_cb[PPP_MAX_DEV];
lcp_value_type lcp_remote_work_cb[PPP_MAX_DEV];

/*===========================================================================

            LOCAL DEFINITIONS AND DECLARATIONS FOR MODULE

===========================================================================*/

/*---------------------------------------------------------------------------
  These defaults are defined in the PPP RFCs, and must not be changed
---------------------------------------------------------------------------*/
lcp_value_type lcp_default =
{
  LCP_ACCM_DEFAULT,  /* default ACCM - 0       */
  0,                 /* no authentication      */
  LCP_MRU_DEFAULT,   /* default MRU            */
  0,                 /* no monitoring          */
  0L,                /* no magic number        */
  0                  /* no default CHAP digest */
};

negotiate_t lcp_accept = LCP_N_MRU | LCP_N_ACCM | LCP_N_MAGIC | LCP_N_PFC |
                         LCP_N_ACFC | LCP_N_AP;

/*---------------------------------------------------------------------------
  these lengths are the minimum required for the option
---------------------------------------------------------------------------*/
static uint8 option_length[] =
{
   4,    /* MRU                         */
   6,    /* ACCM                        */
   4,    /* authentication              */
   4,    /* quality monitoring          */
   6,    /* magic number                */
   2,    /* Protocol compression        */
   2     /* Address/Control compression */
};

/*---------------------------------------------------------------------------
  these are the PPP options numbers based on the option mask. (N's above)
----------------------------------------------------------------------------*/
static uint8 option_number[] =
{
  LCP_MRU,
  LCP_ACCM,
  LCP_AP,
  LCP_QMP,
  LCP_MAGIC,
  LCP_PFC,
  LCP_ACFC
};

/*=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=--
                            FORWARD DECLARATIONS
=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=--*/
static void lcp_option
(
  ppp_fsm_type   *fsm_ptr,
  void           *vp,
  dsm_item_type **bpp,
  uint8           o_type
);

static int lcp_check
(
  ppp_fsm_type      *fsm_ptr,
  struct option_hdr *ohp,
  dsm_item_type    **bpp,
  int                request
);

static int lcp_reject
(
  ppp_fsm_type      *fsm_ptr,
  struct option_hdr *ohp
);

static void lcp_starting
(
  ppp_fsm_type *fsm_ptr
);

static void lcp_stopping
(
  ppp_fsm_type *fsm_ptr
);

static void lcp_closing
(
  ppp_fsm_type *fsm_ptr
);

static void lcp_opening
(
  ppp_fsm_type *fsm_ptr
);

static void lcp_free
(
  ppp_fsm_type *fsm_ptr
);

void lcpi_timer_cb
(
  void *arg_ptr
);

/*---------------------------------------------------------------------------
  LCP constants for the FSM
---------------------------------------------------------------------------*/
static struct fsm_constant_s lcp_constants =
{
#ifndef T_ARM
  "LCP",                    /* Name of protocol                     */
  fsmCodes,                 /* names of codes                       */
#endif /* T_ARM */
  PPP_LCP_PROTOCOL,         /* Protocol number                      */
  DSM_PS_LCP_PROTOCOL,      /* The DSM kind field for this protocol */
  0x0FFE,                   /* codes 1-11 recognized                */
                                                                      
  LCP,                      /* Finite State Machine index           */ 
  LCP_REQ_TRY,              /* # tries for configure                */
  LCP_NAK_TRY,              /* # tries for nak substitutes          */
  LCP_TERM_TRY,             /* # tries for terminate                */
  LCP_TIMEOUT,              /* Time for timeouts (milliseconds)     */
                                                                      
  LCP_OPTION_LIMIT,         /* maximum option number                */
  LCP_N_OPTION_LIMIT,       /* maximum mask number                  */
  option_number,            /* mask table                           */

  lcp_free,                 /* to free structure                    */
  fsm_down,                 /* layer layer down event               */

  lcp_starting,             /* this layer start: enter starting     */
  lcp_opening,              /* this layer opening: enter opening    */
  lcp_closing,              /* this layer closing: enter closing    */  
  lcp_stopping,             /* this layers stopping: leave opened   */

  lcp_option,               /* add option to message                */
  lcp_check,                /* validate option and store if good    */
  lcp_reject                /* process rx'ed config reject          */
};


/*=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=

                             EXTERNAL FUNCTIONS

=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=*/
/*===========================================================================
FUNCTION LCP_POWERUP_INIT()

DESCRIPTION
  This function is called at system initialization time.  It initializes the
  FSM for LCP on the interface specified, this includes getting a timer.

RETURN VALUE
  int: -1 on failure
        0 on success

DEPENDENCIES
  None

SIDE EFFECTS
  None
===========================================================================*/
int lcp_powerup_init
(
  ppp_dev_enum_type device /* the interface to initialize */
)
{
  ppp_fsm_type   *fsm_ptr;
  /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/
  if(device >= PPP_MAX_DEV)
  {
    ASSERT(0);
    return -1;
  }

  fsm_ptr = &(ppp_cb_array[device].fsm[LCP]);

  /*-------------------------------------------------------------------------
    reset the FSM for this interface, and LCP
  -------------------------------------------------------------------------*/
  memset(fsm_ptr, 0, sizeof(ppp_fsm_type));

  /*-------------------------------------------------------------------------
    Initialize the protocol dependent constants, variable pointer and the
    work and want value pointers to their allocated memory.
  -------------------------------------------------------------------------*/
  fsm_ptr->pdc = &lcp_constants;
  fsm_ptr->pdv = NULL;
  fsm_ptr->local.want_pdv = &(lcp_local_want_cb[device]);
  fsm_ptr->local.work_pdv = &(lcp_local_work_cb[device]);
  fsm_ptr->remote.want_pdv = &(lcp_remote_want_cb[device]);
  fsm_ptr->remote.work_pdv = &(lcp_remote_work_cb[device]);

  /*-------------------------------------------------------------------------
    Initialize the device field to the device passed in
  -------------------------------------------------------------------------*/
  fsm_ptr->device = device;

  /*-------------------------------------------------------------------------
    Get the timer for this instance of LCP - return 0 if OK or -1 if failed
  -------------------------------------------------------------------------*/
  fsm_ptr->timer = ps_timer_alloc(lcpi_timer_cb, ppp_cb_array + device);

  if(fsm_ptr->timer == PS_TIMER_FAILURE)
  {
    return -1;
  }
  else
  {
    return 0;
  }

} /* lcp_powerup_init() */



/*===========================================================================
FUNCTION LCP_INIT()

DESCRIPTION
  This function will initialize the LCP configuration structure for the given
  interface (Rm or Um).

DEPENDENCIES
  None

RETURN VALUE
  None

SIDE EFFECTS
  None
===========================================================================*/
void lcp_init
(
  ppp_dev_enum_type  device,   /* device to initalize         */
  ppp_dev_opts_type *opts_ptr   /* options to initialize with */
)
{
  ppp_fsm_type *fsm_ptr;
  lcp_value_type *lwant_ptr;
  lcp_value_type *lwork_ptr;
  lcp_value_type *rwant_ptr;
  lcp_value_type *rwork_ptr;
/* - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - */
  if(device >= PPP_MAX_DEV ||
     opts_ptr == NULL)
  {
    ASSERT(0);
    return;
  }

  fsm_ptr = &(ppp_cb_array[device].fsm[Lcp]);
  lwant_ptr = (lcp_value_type*)fsm_ptr->local.want_pdv;
  lwork_ptr = (lcp_value_type*)fsm_ptr->local.work_pdv;
  rwant_ptr = (lcp_value_type*)fsm_ptr->remote.want_pdv;
  rwork_ptr = (lcp_value_type*)fsm_ptr->remote.work_pdv;

  /*-------------------------------------------------------------------------
    Zeroing out the variables for this interface.
  -------------------------------------------------------------------------*/
  memset(lwant_ptr, 0, sizeof(lcp_value_type));
  memset(rwant_ptr, 0, sizeof(lcp_value_type));
  memset(lwork_ptr, 0, sizeof(lcp_value_type));
  memset(rwork_ptr, 0, sizeof(lcp_value_type));

  fsm_ptr->state = fsmINITIAL;

#ifdef FEATURE_DS_MOBILE_IP_PERF
  /*-------------------------------------------------------------------------
    Notify M.IP call performance that Um/Rm PPP negotiation is starting.
  -------------------------------------------------------------------------*/
  if(device == PPP_UM_SN_DEV)
  {
    mip_perf_delay(&mip_perf_info[PERF_UM_PPP_DELAY],
                   PERF_UM_PPP_DELAY,
                   PERF_START_TS);
  }
  else if(device == PPP_RM_DEV)
  {
    mip_perf_delay(&mip_perf_info[PERF_RM_PPP_DELAY],
                   PERF_RM_PPP_DELAY,
                   PERF_START_TS);
  }
#endif /* FEATURE_DS_MOBILE_IP_PERF */

  /*-------------------------------------------------------------------------
    Use passed in options to initialize things.
  -----------------------------------------------------------------------*/
  memcpy(lwant_ptr,
         &(opts_ptr->lcp_info.want_vals[FSM_LOCAL]),
         sizeof(lcp_value_type));
  memcpy(rwant_ptr,
         &(opts_ptr->lcp_info.want_vals[FSM_REMOTE]),
         sizeof(lcp_value_type));
  memcpy(lwork_ptr,
         &(opts_ptr->lcp_info.work_vals[FSM_LOCAL]),
         sizeof(lcp_value_type));
  memcpy(rwork_ptr,
         &(opts_ptr->lcp_info.work_vals[FSM_REMOTE]),
         sizeof(lcp_value_type));
  fsm_ptr->local.want = opts_ptr->lcp_info.want_mask[FSM_LOCAL];
  fsm_ptr->remote.want = opts_ptr->lcp_info.want_mask[FSM_REMOTE];
  fsm_ptr->local.will = opts_ptr->lcp_info.will_mask[FSM_LOCAL];
  fsm_ptr->remote.will = opts_ptr->lcp_info.will_mask[FSM_REMOTE];

  /*-------------------------------------------------------------------------
    Make sure that work is setup so that we negotiate for what we want.
  -------------------------------------------------------------------------*/
  fsm_ptr->local.work = fsm_ptr->local.want;
  fsm_ptr->remote.work = fsm_ptr->remote.want;

  fsm_init(fsm_ptr);

} /* lcp_init() */



/*===========================================================================
FUNCTION LCP_SAVE_WORK_OPT()

DESCRIPTION
   This function stores the value of the LCP option that is passed in. It is
   being saved into the work and work_pdv the want values are saved by the
   caller.

DEPENDENCIES
  lcp_init() and ipcp_init() should have been called before this function.

RETURN VALUE
  None.

SIDE EFFECTS
  None
===========================================================================*/
void lcp_save_work_opt
(
  ppp_fsm_side_type *opts,       /* pointer to where the values are stored */
  byte               opt_num,                         /* the option number */
  byte              *opt_val                    /* the value of the option */
)
{
  /*-------------------------------------------------------------------------
    Get a handle on the two LCP option storing structs.
  -------------------------------------------------------------------------*/
  lcp_value_type *work_opts = (lcp_value_type*)(opts->work_pdv);

  /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/
  /*-------------------------------------------------------------------------
    Determine the option type
  -------------------------------------------------------------------------*/
  switch(opt_num)
  {
    case LCP_MRU: /* Max Receive Unit */
      work_opts->mru = ntohs((*((uint16*)opt_val)));
      opts->work |= LCP_N_MRU;
      break;

    case LCP_ACCM: /* Async Char Control Mask */
      work_opts->accm = ntohl((*((uint32*)opt_val)));
      opts->work |= LCP_N_ACCM;
      break;

    case LCP_AP: /* Authentication Protocol */
      work_opts->authentication = ntohs((*((uint16*)opt_val)));
      opts->work |= LCP_N_AP;
      break;

    case LCP_QMP: /* quility monitoring protocol */
      work_opts->monitoring = ntohs((*((uint16*)opt_val)));
      opts->work |= LCP_N_QMP;
      break;

    case LCP_MAGIC: /* Magic Number */
      work_opts->magic_number = ntohl((*((uint32*)opt_val)));
      opts->work |= LCP_N_MAGIC;
      break;

    case LCP_PFC: /* Proto Field Compression */
      opts->work |= LCP_N_PFC;
      break;

    case LCP_ACFC: /* Address & Control Field Compression */
      opts->work |= LCP_N_ACFC;
      break;

    default:
      MSG_MED("Got unknown option %d - ignoring", opt_num, 0, 0);

  } /* switch(option) */

} /*  lcp_save_work_opt() */


/*===========================================================================

                             INTERNAL FUNCTIONS

===========================================================================*/
/*===========================================================================
FUNCTION LCP_OPTION()

DESCRIPTION
  This function will add the given option to the DSM item passed in.

DEPENDENCIES
  None

RETURN VALUE
  None

SIDE EFFECTS
  None
===========================================================================*/
static void lcp_option
(
  ppp_fsm_type    *fsm_ptr,
  void            *vp,
  dsm_item_type **item_head_ptr,
  uint8           opt_num
)
{
  ppp_type       *ppp_cb_ptr = ppp_cb_array + fsm_ptr->device;
  lcp_value_type *value_p = (lcp_value_type *)vp;
  dsm_item_type  *bp = *item_head_ptr;
  register uint8 *cp;
  int n_type   = fsm_n_option(fsm_ptr->pdc,opt_num);
  int opt_len  = option_length[n_type];
  register int used = opt_len - OPTION_HDR_LEN;
  /* - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - */

  /*-------------------------------------------------------------------------
    this only tests for the minimum option length
  -------------------------------------------------------------------------*/
  if ( bp->used + opt_len > bp->size )
  {
    return;
  }
  cp = bp->data_ptr + bp->used;
  *cp++ = opt_num;
  /*-------------------------------------------------------------------------
    Note this length will be modified at the bottom if the option is longer
    than indicated by the default values - as it is if CHAP is enabled.
  -------------------------------------------------------------------------*/
  *cp++ = opt_len;

  /*-------------------------------------------------------------------------
    Determine the option type
  -------------------------------------------------------------------------*/
  switch( opt_num )
  {
    case LCP_MRU: /* Max Receive Unit */
      put16(cp, value_p->mru);
      used -= 2;
      break;

    case LCP_ACCM: /* Async Char Control Mask */
      put32(cp, value_p->accm);
      used -= 4;
      break;

    case LCP_AP: /* Authentication Protocol */
      cp = put16(cp, value_p->authentication);
      used -= 2;
      if(value_p->authentication == PPP_CHAP_PROTOCOL)
      {
        ppp_cb_ptr->auth.mode = PPP_DO_CHAP;

        /*-------------------------------------------------------------------
          Copy the CHAP digest. The digest algorithm number is appended after
          the CHAP protocol.
        -------------------------------------------------------------------*/
        *cp++ = value_p->chap_digest;
        used--;
      }
      else
      {
        ppp_cb_ptr->auth.mode = PPP_DO_PAP;
      }
      MSG_MED(" Make PPP cfg-nak: ask PAP/CHAP authen.",0,0,0);
      break;

    case LCP_MAGIC: /* Magic Number */
      put32(cp, value_p->magic_number);
      used -= 4;
      break;

    case LCP_PFC: /* Proto Field Compression */
      break;

    case LCP_ACFC: /* Address & Control Field Compression */
      break;
  };

  opt_len                    -= used;
  bp->data_ptr[bp->used + 1]  = opt_len;       /* length may be modified */
  bp->used                   += opt_len;

} /* lcp_option() */



/*===========================================================================
FUNCTION LCP_CHECK()

DESCRIPTION
  This function will Check the options, updating the working values.

DEPENDENCIES
  None

RETURN VALUE
  Returns -1 if ran out of data, ACK/NAK/REJ as appropriate.

SIDE EFFECTS
  None
===========================================================================*/
static int lcp_check
(
  ppp_fsm_type      *fsm_ptr,
  struct option_hdr *ohp,
  dsm_item_type    **bpp,
  int                request
)
{
  lcp_value_type * lworkp; 
  lcp_value_type * rworkp; 
  ppp_fsm_side_type * side_p;
  lcp_value_type * s_want_ptr;
  lcp_value_type * s_work_ptr;
  int used = 0;
  int option_result = CONFIG_ACK;   /* Assume good values   */
  int n_type = 0;
  int16 rx_chap_digest; /* chap digest from peer */

  /* - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - */

  if( bpp == NULL )
  {
    MSG_ERROR("Invalid DSM", 0, 0, 0);
    return -1;
  }

  if( fsm_ptr == NULL || 
      ohp == NULL || 
      ohp->length < OPTION_HDR_LEN ||
      fsm_ptr->device >= PPP_MAX_DEV )
  {
    MSG_ERROR("Invalid args", 0, 0, 0);
    dsm_free_packet(bpp);
    return -1;
  }

  used = ohp->length - OPTION_HDR_LEN; 
  n_type = fsm_n_option(fsm_ptr->pdc,ohp->type);
  lworkp = (lcp_value_type *)fsm_ptr->local.work_pdv;
  rworkp = (lcp_value_type *)fsm_ptr->remote.work_pdv;
  side_p =
     (ppp_fsm_side_type *)(request ? &(fsm_ptr->remote) : &(fsm_ptr->local));
  s_want_ptr = (lcp_value_type *)side_p->want_pdv;
  s_work_ptr = (lcp_value_type *)side_p->work_pdv;

  /*-------------------------------------------------------------------------
    see if we allow this option
  -------------------------------------------------------------------------*/
  if (ohp->type > LCP_OPTION_LIMIT        ||
      ohp->length < option_length[n_type] ||
      !((side_p->will | side_p->want) & (1 << n_type)))
  {
    /*-----------------------------------------------------------------------
      toss any bytes in option
    -----------------------------------------------------------------------*/
    while( used-- > 0 )
    {
      if ( dsm_pull8(bpp) == -1 )
      {
        return -1;
      }
    }
    return CONFIG_REJ;

  } /* if(unsupported option) */

  switch(ohp->type)
  {
  case LCP_MRU: /* max receive unit */
    s_work_ptr->mru = dsm_pull16(bpp);
    used -= 2;

    /*-----------------------------------------------------------------------
      Check if new value is appropriate
   -----------------------------------------------------------------------*/
    if(s_work_ptr->mru < LCP_MRU_LO)
    {
      s_work_ptr->mru   = LCP_MRU_LO;
      option_result = CONFIG_NAK;
    }
    else if (s_work_ptr->mru > LCP_MRU_HI)
    {
      s_work_ptr->mru = LCP_MRU_HI;
      option_result = CONFIG_NAK;
    }
      
    if(request                    &&
       (side_p->want & LCP_N_MRU) &&
       s_work_ptr->mru > s_want_ptr->mru)
    {
      /*---------------------------------------------------------------------
        send a smaller packet anyway
      -----------------------------------------------------------------------*/
      s_work_ptr->mru = s_want_ptr->mru;
    }
    break;

  case LCP_ACCM: /* async control character map */
    s_work_ptr->accm = dsm_pull32(bpp);
    used -= 4;

    /*-----------------------------------------------------------------------
      Remote host may ask to escape more control characters than we require,
      but must escape at least the control chars that we require.
    -----------------------------------------------------------------------*/
    if ((side_p->want & LCP_N_ACCM) &&
        s_work_ptr->accm != (s_work_ptr->accm | s_want_ptr->accm))
    {
      s_work_ptr->accm |= s_want_ptr->accm;
      option_result = CONFIG_NAK;
    }
    break;

  case LCP_AP: /* Authentication protocol */
    s_work_ptr->authentication = dsm_pull16(bpp); /* get the protocol */
    used -= 2;

    /*-----------------------------------------------------------------------
      Check if requested authentication protocol is PAP or CHAP or NONE
    -----------------------------------------------------------------------*/
    switch (s_work_ptr->authentication)
    {
    case PPP_PAP_PROTOCOL:
      /*---------------------------------------------------------------------
        Setting it to send PAP pkts in auth. phase.
      ---------------------------------------------------------------------*/
      ppp_cb_array[fsm_ptr->device].auth.mode = PPP_DO_PAP;
      MSG_MED(" Received Config. Req. with PAP option",0,0,0);
      break;

    case PPP_CHAP_PROTOCOL:
      /*---------------------------------------------------------------------
        Setting it to send CHAP pkts in auth. phase.
      ---------------------------------------------------------------------*/
      ppp_cb_array[fsm_ptr->device].auth.mode  = PPP_DO_CHAP;

      /*---------------------------------------------------------------------
        In the incoming packet the CHAP protocol has algorithm field attached
        to it, i.e. C223 for CHAP and 05 for MD5 or 80 for MD4.  Since the
        current implementation implements only MD5, check if the algorithm is
        MD5.

        Since we pulled out one more byte from the incoming packet reduce the
        used field by one.

        If the algorithm is not MD5, then send a Config NAK to the base
        station and request for the MD5.
      ---------------------------------------------------------------------*/
      rx_chap_digest = dsm_pull8(bpp);
      if(rx_chap_digest == -1)
      {
        return -1;
      }
      used--;
      if((s_work_ptr->chap_digest = (uint8)rx_chap_digest) != PPP_CHAP_MD5)
      {
        s_work_ptr->chap_digest = PPP_CHAP_MD5;
        option_result = CONFIG_NAK;
      }
      MSG_MED("dev %d got C-req with chap digest: %d",
              fsm_ptr->device,
              rx_chap_digest,
              0);
      break;

    default:
      /*---------------------------------------------------------------------
        Got an unrecognized authentication protocol.  Ask for CHAP/PAP in the
        nak and Send a config_nak which asks for CHAP as the authentication
        protocol.
      ---------------------------------------------------------------------*/
      MSG_MED("Rxd Unknown Auth proto:0x%x ",s_work_ptr->authentication,0,0);
      s_work_ptr->authentication = PPP_CHAP_PROTOCOL;
      s_work_ptr->chap_digest    = PPP_CHAP_MD5;
      option_result = CONFIG_NAK;
      break;
    } /* switch(auth type in req) */
    break; /* case LCP_AP */

  case LCP_MAGIC: /* magic number */
    s_work_ptr->magic_number = dsm_pull32(bpp);
    used -= 4;

    /*-----------------------------------------------------------------------
      Ensure that magic numbers are different
    -----------------------------------------------------------------------*/
    if ( s_work_ptr->magic_number == 0L )
    {
      s_work_ptr->magic_number = (uint32)s_work_ptr;
      option_result = CONFIG_NAK;
    } else if ( rworkp->magic_number == lworkp->magic_number )
    {
      s_work_ptr->magic_number += msclock();
      option_result = CONFIG_NAK;
    }
    break;

  case LCP_PFC:  /* protocol field compression */
  case LCP_ACFC: /* addr/control field compression */
    /* nothing to store here */
    break;

  default:
    option_result = CONFIG_REJ;
    break;
  } /* switch(option type) */

  if ( used < 0 )
  {
    return -1;
  }

  /*-------------------------------------------------------------------------
    if there are extra bytes in option toss them.
  -------------------------------------------------------------------------*/
  if ( used > 0 )
  {
    while( used-- > 0 )
    {
      if ( dsm_pull8(bpp) == -1 )
      {
        return -1;
      }
    }
  }

  return (option_result);

} /* lcp_check() */



/*===========================================================================
FUNCTION LCP_REJECT()

DESCRIPTION
  This function will process configuration reject sent by remote host -
  currently doesn't do anything.

DEPENDENCIES
  None

RETURN VALUE
  None

SIDE EFFECTS
  None
===========================================================================*/
static int lcp_reject
(
  ppp_fsm_type      *fsm_ptr,
  struct option_hdr *opt_hdr_ptr
)
{
  ppp_type *ppp_cb_ptr = (ppp_type*)fsm_ptr; /* assumes fsm is first!      */
/*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  /*-------------------------------------------------------------------------
    if the option that was rejected is authentication - then set the auth
    mode to NO_AUTH.  In all other cases do nothing.
  -------------------------------------------------------------------------*/
  if(opt_hdr_ptr->type == LCP_AP)
  {
    ppp_cb_ptr->auth.mode = PPP_NO_AUTH;
  }

  return 0;
} /* lcp_reject() */



/*===========================================================================
FUNCTION LCP_STARTING()

DESCRIPTION
  This function will Prepare to begin configuration exchange

DEPENDENCIES
  None

RETURN VALUE
  None

SIDE EFFECTS
  None
===========================================================================*/
static void lcp_starting
(
  ppp_fsm_type *fsm_ptr
)
{
  ppp_type *ppp_cb_ptr;
  ppp_event_payload_type ppp_event_payload;

  if( fsm_ptr == NULL || 
      fsm_ptr->local.want_pdv == NULL ||
      fsm_ptr->local.work_pdv == NULL ||
      fsm_ptr->remote.want_pdv == NULL ||
      fsm_ptr->device >= PPP_MAX_DEV )
  {
    MSG_ERROR("Invalid args", 0, 0, 0);
    return;
  }

  ppp_cb_ptr = &(ppp_cb_array[fsm_ptr->device]);
  /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - */

  /*-------------------------------------------------------------------------
    Fill in the fileds of ppp_event_payload and post the event
  -------------------------------------------------------------------------*/
  ppp_event_payload.ppp_event_device     = fsm_ptr->device;
  ppp_event_payload.ppp_event_protocol   = PPP_EV_LCP;
  ppp_event_payload.ppp_event_prev_state = PS_PPP_EVENT_UNKNOWN;
  ppp_event_payload.ppp_event_state      = PS_PPP_EVENT_STARTING;
  event_report_payload(EVENT_PPP, 
                       sizeof(ppp_event_payload),
                       &ppp_event_payload);

  ppp_cb_ptr->phase = pppESTABLISH;

  fsm_ptr->local.work = fsm_ptr->local.want;

  fsm_ptr->remote.work = FALSE;

} /* lcp_starting() */


/*===========================================================================
FUNCTION LCP_OPENING()

DESCRIPTION
  This function will be called after configuration negotiation is complete
  and PPP completes the Establish phase. It can go into the network or the
  authentication phase after that.

 If the network negotiated authentication or we need to authenticate the
 peer, then ppp will transition to the authentication phase. Otherwise, PPP
 will transition to the network phase without doing authentication.

DEPENDENCIES
  None

RETURN VALUE
  None

SIDE EFFECTS
  None
===========================================================================*/
static void lcp_opening
(
  ppp_fsm_type *fsm_ptr /* ptr to a FSM structure */
)
{
  ppp_type *ppp_cb_ptr;
  lcp_value_type *rmt_work_ptr = (lcp_value_type*)fsm_ptr->remote.work_pdv;
  ppp_event_payload_type  ppp_event_payload;
/*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  if(fsm_ptr == NULL || fsm_ptr->device >= PPP_MAX_DEV )
  {
    MSG_ERROR("Invalid fsm_ptr", 0, 0, 0);
    return;
  }

  MSG_MED("dev %d LCP Up!",fsm_ptr->device, 0, 0);
  ppp_cb_ptr = &(ppp_cb_array[fsm_ptr->device]);
  ppp_cb_ptr->upsince = msclock();

  /*-------------------------------------------------------------------------
    Set the default value for the remote MRU if it was not negotiated
  -------------------------------------------------------------------------*/
  if(!(fsm_ptr->remote.work & LCP_N_MRU))
  {
    rmt_work_ptr->mru = ppp_cb_ptr->rx_iface_ptr->v4_net_info.mtu;
  }

  /*-------------------------------------------------------------------------
    If it has changed, set new Max Transmission Unit for outgoing packets
  -------------------------------------------------------------------------*/
  if(ppp_cb_ptr->rx_iface_ptr->v4_net_info.mtu != rmt_work_ptr->mru)
  {
    ppp_cb_ptr->rx_iface_ptr->v4_net_info.mtu = rmt_work_ptr->mru;
  }

  /*-------------------------------------------------------------------------
    Delay transition to next phase to allow remote ACCM to settle
  -------------------------------------------------------------------------*/
  pppi_discard(fsm_ptr->device, NULL);

  /*-------------------------------------------------------------------------
    Fill in the fileds of ppp_event_payload
  -------------------------------------------------------------------------*/
  ppp_event_payload.ppp_event_device = fsm_ptr->device;
  ppp_event_payload.ppp_event_protocol = PPP_EV_LCP;
  ppp_event_payload.ppp_event_prev_state = PS_PPP_EVENT_UNKNOWN;
  ppp_event_payload.ppp_event_state = PS_PPP_EVENT_OPEN;
  event_report_payload(EVENT_PPP, 
                       sizeof(ppp_event_payload),
                       &ppp_event_payload);

  /*-------------------------------------------------------------------------
    kick off authentication phase
  -------------------------------------------------------------------------*/
  ppp_auth_start(fsm_ptr->device);

} /* lcp_opening() */


/*===========================================================================
FUNCTION LCP_CLOSING()

DESCRIPTION
  This function will Close higher levels in preparation for link shutdown

DEPENDENCIES
  None

RETURN VALUE
  None

SIDE EFFECTS
  None
===========================================================================*/
static void lcp_closing
(
  ppp_fsm_type *fsm_ptr
)
{
  ppp_type *ppp_cb_ptr; 
  int i;
  /* - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - */

  if(fsm_ptr == NULL || fsm_ptr->device >= PPP_MAX_DEV )
  {
    MSG_ERROR("Invalid fsm_ptr", 0, 0, 0);
    return;
  }

  ppp_cb_ptr = &(ppp_cb_array[fsm_ptr->device]);

  ppp_cb_ptr->phase = pppTERMINATE;

  for(i = LCP + 1; i < fsmi_Size; i++)
  {
    ppp_fsm_type *fsm_ptr = &(ppp_cb_ptr->fsm[i]);

    if(fsm_ptr->pdc != NULL && fsm_ptr->pdc->down != NULL)
    {
      (*fsm_ptr->pdc->down)(fsm_ptr);
    }
  }
} /* lcp_closing() */



/*===========================================================================
FUNCTION LCP_STOPPING()

DESCRIPTION
  This function is called when a Term-Req packet is received.

DEPENDENCIES
  None

RETURN VALUE
  None

SIDE EFFECTS
  None
===========================================================================*/
static void lcp_stopping
(
  ppp_fsm_type *fsm_ptr
)
{
  ppp_event_payload_type ppp_event_payload; 
  /* - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - */

  /*-------------------------------------------------------------------------
    Fill in the fileds of ppp_event_payload and generate the event
  -------------------------------------------------------------------------*/
  ppp_event_payload.ppp_event_device = fsm_ptr->device;
  ppp_event_payload.ppp_event_protocol = PPP_EV_LCP;
  ppp_event_payload.ppp_event_prev_state = PS_PPP_EVENT_UNKNOWN;
  ppp_event_payload.ppp_event_state = PS_PPP_EVENT_CLOSING;

  event_report_payload(EVENT_PPP, 
                       sizeof(ppp_event_payload),
                       &ppp_event_payload);

  /*-------------------------------------------------------------------------
    Call ppp_abort() to do final clean up.
  -------------------------------------------------------------------------*/
  ppp_abort(fsm_ptr->device);

} /* lcp_stopping() */


/*===========================================================================
FUNCTION LCPI_TIMER_CB()

DESCRIPTION
  This function handles events for an LCP timeout.  As the same timer is used
  for authentication this must handle that case also.

RETURN VALUE
  None

DEPENDENCIES
  None

SIDE EFFECTS
  None
===========================================================================*/
void lcpi_timer_cb
(
  void *arg_ptr
)
{
  ppp_type *ppp_cb_ptr = (ppp_type*)arg_ptr;

  /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  MSG_MED("dev %d LCP Timer Expired state=%d",
          ppp_cb_ptr->fsm[LCP].device,
          ppp_cb_ptr->fsm[LCP].state,
          0);

  /*-------------------------------------------------------------------------
    LCP/Auth timer has expired, check whether the timer expired during Auth
    or LCP.
  -------------------------------------------------------------------------*/
  if(ppp_cb_ptr->phase == pppAUTHENTICATE)
  {
    if(ppp_auth_timeout(ppp_cb_ptr) == FALSE)
    {
      MSG_MED("dev %d auth timed out - quitting!",
              ppp_cb_ptr->fsm[LCP].device,0,0);
      ppp_abort(ppp_cb_ptr->fsm[LCP].device);
    }
  }
  else
  /*-------------------------------------------------------------------------
    It is in LCP establish phase - just call timeout as that will take the
    appropriate action (i.e. lcp_stopping() will be called and the call will
    be torn down).
  -------------------------------------------------------------------------*/
  {
    if(fsm_timeout(&(ppp_cb_ptr->fsm[Lcp])) == FALSE)
    {
      MSG_MED("LCP on Dev %d failed to establish!", 
              ppp_cb_ptr->fsm[LCP].device,0,0);

      ppp_cb_ptr->fsm[LCP].fail_reason = PPP_FAIL_REASON_TIMEOUT;
    }
  }
} /* lcpi_timer_cb() */



/*===========================================================================
FUNCTION LCP_FREE()

DESCRIPTION
  This function will

DEPENDENCIES
  None

RETURN VALUE
  None

SIDE EFFECTS
  None
===========================================================================*/
static void lcp_free
(
  ppp_fsm_type *fsm_ptr
)
{
  /* nothing to do */

} /* lcp_free() */

#endif /* FEATURE_DATA_PS */
