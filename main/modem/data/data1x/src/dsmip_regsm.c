/*===========================================================================

                          D S M I P _ R E G S M . C

DESCRIPTION

 The Data Services Mobile IP Agent Solicitation State Machine source file.

EXTERNALIZED FUNCTIONS
   mip_reg_sm_init()
     Initialize the Registration State Machine.
   mip_reg_post_event()
     Post an event to a Registration State Machine.
   mip_reg_sm_post_event_all()
     Pose an event to ALL of the Registrations State Machines. 
     Initialize the Meta State Machine.
   mip_reg_sm_session_active
     Check if MIP client currently has a valid registration 

Copyright (c) 2000-2011 by QUALCOMM, Incorporated.  All Rights Reserved.
===========================================================================*/
/*===========================================================================

                            EDIT HISTORY FOR FILE

  $PVCSPath: O:/src/asw/COMMON/vcs/dsmip_regsm.c_v   1.39   24 Jan 2003 19:42:24   ubabbar  $
  $Header: //source/qcom/qct/modem/data/1x/mip/main/lite/src/dsmip_regsm.c#4 $ $DateTime: 2011/02/24 23:31:53 $ $Author: msankar $

when        who    what, where, why
--------    ---    ----------------------------------------------------------
02/25/11    ms     Ported MOBILE_IP_DEREG feature.
04/29/09    sn     Ported support for call throttle feature (DCTM).
06/02/08    ms     Fixed Critical/High Lint errors
11/01/06    an     Featurize ATCOP, RMSM, Async Fax
09/23/03    jd     Set maximum ASM retries to initial value (1) on closure, 
                   and set to handoff value (19) when registered.
09/23/03    jd     Mainlined Irfan's changes for cougar branch
09/22/03    jd     Fixed bug where RRQ timeout was divided by 4 instead of 2
                   when sending RRQ upon WRITE_EV after having blocked on 
                   previous attempt.
08/18/03    jd     Featurized Irfan's changes (FEATURE_DATA_CURRENT_PPP_CB)
08/13/03    ifk    rx_iface_ptr moved to IPCP fsm in ps_iface_set_addr().
06/13/03    ss     Featurized call to ds707_rmsm_is_packet_call().
06/11/03    jd     Add RRQ->RRP RTT estimate to registration re-tx timer
06/10/03    ss     Added code to change the PPP mode to PSEUDO_NET upon
                   transitioning to registered state in a laptop call.
05/05/03    jd     Added mip_reg_sm_session_active() external API to tell if 
                   MIP client is registered
04/11/03    ak     Updated to use new RMSM interfaces.
01/24/03    usb    Updated call to dss_iface_ioctl to use new prototype
12/10/02    jd     Remain in re-registration state while awaiting AAM to keep 
                   Um in internal mode so AAM can be received.  Also, do not
                   increase re-RRQ counter and interval during this time.
11/25/02    usb    Wrong arg type used (dss_iface_ioctl_phys_link_state_type
                   instead of dss_iface_ioctl_state_type in IOCTL_GET_STATE.
11/19/02    jd     For MM, check for dormancy via dss_iface_ioctl's
11/14/02    jd     For MM, call rmsm_is_packet_call instead of checking RMSM
                   state directly, dssnet api changes, .
10/23/02    jay    Fixed a Message
10/09/02    jay    Renamed #define
07/10/02    jd     Cleaned up #include's
06/07/02    jd     Mod. MSG to report actual re-registration timer value
05/07/02    jd     Moved session info block to dsmip.c
03/21/02   jd/sjy  Moved much of processing done when entering registered
                   state to after the state variable is set, in order for
                   mip_reg_sm_all_sessions_regd to return the correct state 
03/21/02   jd/sjy  Added mip_reg_sm_all_sessions_regd() to check on status 
                   of re-registrations for all sessions
02/20/02    sjy    Mobile IP performance now featurized under 
                   FEATURE_DS_MOBILE_IP_PERF
02/07/02    sjy    Added support for Mobile IP call performance metrics.
12/11/01    ss     Modified rsmi_transition_state() to cancel reg timer if
                   no data is transferred since previous registration.
11/26/01    vr     Removed flag mip_is_rereg_in_progress. Do not flow control
                   during MIP re-registration.
11/21/01    aku    dssnet_sm_post_event now takes an additional acb_ptr arg
11/21/01    ss     Added code to support sending RRQ only if there has been 
                   some data transfer since last registration.
11/14/01    ss     Added a random offset to the base rrq interval for 
                   NEED_NEW_FA_INFO event to fix the problem caused by
                   stale challenges.
11/05/01    vr     Add a flag mip_is_rereg_in_progress for ppp to determine if
                   re-registration is going on
10/17/01    aku    The force_dormancy variable needs to set to FALSE at the 
                   beginning of a call
10/16/01    jd     Initialize force_domancy flag to avoid forcing dormancy
                   after initial registration.
09/17/01    js     Added header comment block for externalized functions. 
09/07/01    mvl    Modified the valule for Unset HA to be 255.255.255.255
                   rather than 0 - this synchs up with new PST (2.7.26+).
09/06/01    jd     Fixed bug in dsmip_regsm_init where we scribbled on the
                   rxc_dec_free_q when writing app_id into session info. 
08/23/01    mvl    Modified the setting of the Re-RRQ timer so that it cannot
                   be set to less than 50% of the registration lifetime.
08/23/01    aku    Renamed dss_getifacestatus as dss_get_iface_status
08/14/01    aku    Added support for forcing call dormant after re-reg if 
                   call was dormant prior to re-registration.
08/08/01    aku    Added check for 0.0.0.0 home agent as fallback to SIP was
                   taking very long
08/07/01    jd     Allow 0.0.0.0 home agent
08/05/01    jd     Added RRQ holdoff timer support
08/01/01    mvl    Added fix to insure infinite registrations are supported.
                   Added fix to reset retry count and initial timeout when
                   doing re-registrations.
07/27/01    aku    Post event to DSSNET State machine on successful M. IP
                   registration
06/28/01    mvl    Checking if the Pre RRQ is >= lifetime (bug fix)
06/26/01    mvl    Now if we get a solicitation failure in no session state
                   we send Init-RRQ failure rather than Re-RRQ failure
06/12/01    mvl    Updated to reflect latest design
11/29/00    mvl    Created module
===========================================================================*/

/*===========================================================================

                          INCLUDE FILES FOR MODULE

===========================================================================*/
#include "comdef.h"
#include "customer.h"

#ifdef FEATURE_DS_MOBILE_IP

#include "memory.h"
#include "assert.h"
#include "pstimer.h"
#include "msg.h"

#include "dsmip.h"
#include "dsmip_io.h"
#include "dsmip_regsm.h"
#include "dsmip_outmsg.h"
#include "dsmip_solsm.h"
#include "dsmip_metasm.h"
#ifdef FEATURE_DS_MOBILE_IP_PERF
#include "dsmip_perf.h"
#endif /* FEATURE_DS_MOBILE_IP_PERF */

#ifdef FEATURE_DATA_MM
#include "dssocket.h"
#include "ps_ppp.h"
#include "ps_ppp_fsm.h"
#include "ps_ip_addr.h"
#ifndef FEATURE_DATA_STRIP_ATCOP
#include "ds707_rmsm.h"
#endif
#include "ps_mip_compat.h"
#include "ps_iface_defs.h"
#include "dss_iface_ioctl.h"
#else
#ifndef FEATURE_DATA_STRIP_ATCOP
#include "dsmip_rmsm.h"
#endif
#include "dsi.h"
#endif /* FEATURE_DATA_MM */


/*= = = = = = = = = = = = = = = = = = = = = = = = = = = = = = = = = = = = = =
                            INTERNAL DEFINITIONS
= = = = = = = = = = = = = = = = = = = = = = = = = = = = = = = = = = = = = =*/
#define RSMI_INIT_RRQ_TIMEOUT       750   /* millisecs -                   */
#define RSMI_MAX_RETRY_BACKOFF_TIME 16000 /* millisecs -                   */

#ifdef ASSERT
static boolean rsmi_inited = FALSE;
#endif

/*---------------------------------------------------------------------------
  Indicates if re-registration is required, as determined by data transfer 
  happening since the last registration and the corresponding user 
  preference retrieved from NV indicating if this optimization is enabled or
  not.
---------------------------------------------------------------------------*/
boolean mip_is_rereg_reqd = TRUE;

#ifndef T_ARM
/*---------------------------------------------------------------------------
  These need to be kept in sync with the enums which they describe!
---------------------------------------------------------------------------*/
char *rsmi_state_names[RSMI_MAX_STATE] =
{
  "No Session",
  "Initial RRQ",
  "Registered",
  "Re-RRQ"
};

char *rsmi_event_names[RSM_MAX_EV] =
{
  "Exit M.IP",
  "Move",
  "Sol Failed",
  "Immediate Retry",
  "Need new FA info",
  "RRQ Successful",
  "RRQ Failed with Retry",
  "RRQ Failed no Retry",
  "RRQ Failed HA Unavailable",
  "RRQ Timer",
  "Lifetime Timer",
};
#endif /* ! T_ARM */

/*===========================================================================
                            Forward Declarations
===========================================================================*/
void rsmi_rrq_timer_cb
(
  void *param
);

#ifdef FEATURE_DS_MOBILE_IP_DEREG
void rsmi_derrq_timer_cb
(
  void *param
);
#endif /* FEATURE_DS_MOBILE_IP_DEREG */

void rsmi_life_timer_cb
(
  void *param
);

void rsmi_rrq_holdoff_timer_cb
(
  void *param
);

void rsmi_transition_state
(
  mip_session_info_type *mip_info,
  rsmi_state_type new_state  
);

void rsmi_set_registered_state_timers
(
  mip_session_info_type *session
);


/*= = = = = = = = = = = = = = = = = = = = = = = = = = = = = = = = = = = = = =
                        EXTERNAL FUNCTION DEFINTIONS
= = = = = = = = = = = = = = = = = = = = = = = = = = = = = = = = = = = = = =*/
/*===========================================================================
FUNCTION MIP_REG_SM_INIT()

DESCRIPTION
  This function initializes the agent solicitation state machine.

PARAMETERS
  None

RETURN VALUE
  0 on success
 -1 on failure

DEPENDENCIES
  None

SIDE EFFECTS
  None
===========================================================================*/
int mip_reg_sm_init
(
  void
)
{
  int loop;
  mip_session_info_type* session;

  /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  /*-------------------------------------------------------------------------
    Zero out the entire mip_ses_info_array[]
  -------------------------------------------------------------------------*/
  memset(mip_ses_info_array,
         0,
         MIP_MAX_SESSIONS * sizeof(mip_session_info_type));

  /*-------------------------------------------------------------------------
    Loop over all of the control blocks that need the rsm_info rsmi_inited -
    note that the pointer to the control block is being incremented in the
    for statement.
  -------------------------------------------------------------------------*/
  session = mip_ses_info_array;
  for (loop = 0; loop < MIP_MAX_SESSIONS; loop++, session++)
  {
    /*-----------------------------------------------------------------------
      Get PS timers to support retries and lifetime expiration
    -----------------------------------------------------------------------*/
    session->rsm_info.reg_timer = ps_timer_alloc(rsmi_rrq_timer_cb, session);
    if(session->rsm_info.reg_timer == PS_TIMER_FAILURE)
    {
      MSG_HIGH("Failed to allocate reg timer for RSM!\n", 0, 0, 0);
      return -1;
    }
    MSG_LOW("reg timer handle is %d\n", session->rsm_info.reg_timer, 0, 0);

#ifdef FEATURE_DS_MOBILE_IP_DEREG
    session->rsm_info.dereg_timer = ps_timer_alloc(rsmi_derrq_timer_cb,session);
    if(session->rsm_info.dereg_timer == PS_TIMER_FAILURE)
    {
      MSG_HIGH( "Failed to allocate dereg timer for RSM!\n",0,0,0);
      return -1;
    }
    MSG_LOW( "dereg timer handle is %d\n", session->rsm_info.dereg_timer,0,0);
#endif /* FEATURE_DS_MOBILE_IP_DEREG */

    session->rsm_info.life_timer = ps_timer_alloc(rsmi_life_timer_cb,
                                                  session);
    if(session->rsm_info.life_timer == PS_TIMER_FAILURE)
    {
      MSG_HIGH("Failed to allocate reg timer for RSM!\n", 0, 0, 0);
      return -1;
    }
    MSG_LOW("lifetime timer handle is %d\n",
            session->rsm_info.life_timer, 0, 0);

    session->rsm_info.rrq_holdoff_timer = 
      ps_timer_alloc (rsmi_rrq_holdoff_timer_cb, session);
    if(session->rsm_info.rrq_holdoff_timer == PS_TIMER_FAILURE)
    {
      MSG_HIGH("Failed to allocate RRQ holdoff timer for RSM%d", loop, 0, 0);
      return -1;
    }
    MSG_LOW("lifetime timer handle is %d",
            session->rsm_info.rrq_holdoff_timer, 0, 0);

    /*-----------------------------------------------------------------------
      Set the retry count, holdoff and registration states to their 
      initial values
    -----------------------------------------------------------------------*/
    session->rsm_info.rrq_cnt = 0;
    session->rsm_info.one_second_since_last_rrq = TRUE;
    session->rsm_info.state = RSMI_NO_SESSION_STATE;
    session->retry_interval_offset = 0;
    session->rsm_info.rtt_estimate = 0;

    /*-----------------------------------------------------------------------
      Set initial value of force_dormancy to false
    -----------------------------------------------------------------------*/
    session->rsm_info.force_dormancy = FALSE;

    /*-------------------------------------------------------------------------
      Retrieve the socket app_id from dsmip_io.
    -------------------------------------------------------------------------*/
    session->rsm_info.app_id = mipio_get_app_id();

  } /* for(all MIP control blocks) */

#ifdef ASSERT
  rsmi_inited = TRUE;
#endif
  return 0;

} /* mip_reg_sm_init() */



/*===========================================================================
FUNCTION MIP_REG_SM_POST_EVENT()

DESCRIPTION
  This function posts an event to the registration state machine.

PARAMETERS
  mip_info: pointer to the M.IP info block array
  event: The event that is being posted to the state machine.

RETURN VALUE
  None

DEPENDENCIES
  mip_reg_sm_init() has to have been called first.

SIDE EFFECTS
  None
===========================================================================*/
void mip_reg_sm_post_event
(
  mip_session_info_type *session,
  mip_reg_sm_event_type event
)
{
  ps_timer_error_type timer_result;

  /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

#ifdef ASSERT
  ASSERT(rsmi_inited == TRUE);
#endif /* ASSERT */

#ifndef T_ARM
  MSG_MED("'%s' event posted to Reg SM %d.\n",
          rsmi_event_names[event],
          abs(session - mip_ses_info_array),
          0);
#else
  MSG_MED("Event %d posted to Reg SM %d.\n",
          event,
          abs(session - mip_ses_info_array), 0);
#endif

  /*-------------------------------------------------------------------------
    Take action based on event received and which state we are currently in.
  -------------------------------------------------------------------------*/
  switch(event)
  {
    /*-----------------------------------------------------------------------
                               RSM_EXIT_MIP_EV

      Exit M.IP: Return to the No Session state
    -----------------------------------------------------------------------*/
  case RSM_EXIT_MIP_EV:
    switch(session->rsm_info.state)
    {
    case RSMI_NO_SESSION_STATE:
      /* ignore */
      MSG_MED("Got Exit M.IP Event in No Session state!\n", 0, 0, 0);
      break;
#ifdef FEATURE_DS_MOBILE_IP_DEREG
    case RSMI_DE_RRQ_STATE:
#endif /* FEATURE_DS_MOBILE_IP_DEREG */
    case RSMI_RE_RRQ_STATE:
    case RSMI_REGISTERED_STATE:
#ifdef FEATURE_DS_MOBILE_IP_DEREG
    case RSMI_DEREGISTERED_STATE:
#endif /* FEATURE_DS_MOBILE_IP_DEREG */
    case RSMI_INIT_RRQ_STATE:
      rsmi_transition_state(session, RSMI_NO_SESSION_STATE);
      break;

    default:
      ASSERT(0);

    } /* switch(rsm state) */
    break;

#ifdef FEATURE_DS_MOBILE_IP_DEREG
    /*-----------------------------------------------------------------------
                               RSMI_DEREG_MIP_EV

      DSSNET asked to deregister the MIP state machine.
        No Session/De RRQ/Deregistered/Init RRQ: ignore
        Re RRQ/Registered: proceed to De RRQ state
    -----------------------------------------------------------------------*/
  case RSMI_DEREG_MIP_EV:
    switch(session->rsm_info.state)
    {
      case RSMI_RE_RRQ_STATE:
        /* cancel the re-registration time */
        timer_result = ps_timer_cancel(session->rsm_info.reg_timer);
        ASSERT(timer_result != PS_TIMER_FAILURE);
        /* fall through intentional */

      case RSMI_REGISTERED_STATE:
        session->rsm_info.rrq_cnt = 0;
        rsmi_transition_state(session, RSMI_DE_RRQ_STATE);
        break;

      case RSMI_NO_SESSION_STATE: 
      case RSMI_DE_RRQ_STATE:
      case RSMI_DEREGISTERED_STATE:
      case RSMI_INIT_RRQ_STATE:
        /* ignore event and flag an error */
        MSG_ERROR( "Got unexpected Deregister M.IP Event!",0,0,0);
        break;

      default:
        ASSERT(0);
    } /* switch(rsm state) */
    break;
#endif /* FEATURE_DS_MOBILE_IP_DEREG */

    /*-----------------------------------------------------------------------
                           RSM_NEED_NEW_FA_INFO_EV

      Our current FA info is invalid, need to get new info.
        No Session/Registered state: ignore
        Re RRQ: return to registered
        Init RRQ: return to no session
    -----------------------------------------------------------------------*/
  case RSM_NEED_NEW_FA_INFO_EV:
    switch(session->rsm_info.state)
    {
    case RSMI_RE_RRQ_STATE:
      /*--------------------------------------------------------------------
        mip_sol_sm_get_random_byte() returns a random number between 0..255. 
        We multiply the return value by 20 to get a random offset in the
        range of 0..~5000 ms. Note that we assign a random offset to 
        retry_interval_offset in RSMI_INIT_RRQ_STATE also.
      ---------------------------------------------------------------------*/
      session->retry_interval_offset = mip_sol_sm_get_random_byte() * 20;
      /* Remain in re-reg state while awaiting AAM */
      break;

    case RSMI_INIT_RRQ_STATE:
      session->retry_interval_offset = mip_sol_sm_get_random_byte() * 20;
      rsmi_transition_state(session, RSMI_NO_SESSION_STATE);
      break;

#ifdef FEATURE_DS_MOBILE_IP_DEREG
    case RSMI_DE_RRQ_STATE:
      MSG_ERROR( "new fa info ev in de-rrq state!",0,0,0);
      rsmi_transition_state(session, RSMI_DEREGISTERED_STATE);
      break;
#endif /* FEATURE_DS_MOBILE_IP_DEREG */

    case RSMI_NO_SESSION_STATE:
    case RSMI_REGISTERED_STATE:
#ifdef FEATURE_DS_MOBILE_IP_DEREG
    case RSMI_DEREGISTERED_STATE:
#endif /* FEATURE_DS_MOBILE_IP_DEREG */
      /* ignore */
#ifndef T_ARM
      MSG_MED("Got Need new FA info event in %s state\n",
              rsmi_state_names[session->rsm_info.state], 0, 0);
#else
      MSG_MED("Got Need new FA info event event in state %d\n",
              session->rsm_info.state, 0, 0);
#endif
      break;

    default:
      ASSERT(0);
    } /* switch(rsm state) */
    break;


    /*-----------------------------------------------------------------------
                               RSM_RRQ_SUCC_EV

      The Registration Request Succeeded:
        Initial RRQ/Re-RRQ: Go to to the Registered state
        No Session/Registered: ignore 
    -----------------------------------------------------------------------*/
  case RSM_RRQ_SUCC_EV:
    switch(session->rsm_info.state)
    {
    case RSMI_RE_RRQ_STATE:
    case RSMI_INIT_RRQ_STATE:
      session->rsm_info.rtt_estimate = 0;
      MSG_MED ("Set RRP latency estimate to %d", 
               session->rsm_info.rtt_estimate, 0, 0);
      rsmi_transition_state(session, RSMI_REGISTERED_STATE);
      break;

    case RSMI_NO_SESSION_STATE:
    case RSMI_REGISTERED_STATE:
#ifdef FEATURE_DS_MOBILE_IP_DEREG
    case RSMI_DEREGISTERED_STATE:
#endif /* FEATURE_DS_MOBILE_IP_DEREG */
      /* ignore */
#ifndef T_ARM
      MSG_MED("Got RRQ Successful event in %s state\n",
              rsmi_state_names[session->rsm_info.state], 0, 0);
#else
      MSG_MED("Got RRQ Successful event in state %d\n",
              session->rsm_info.state, 0, 0);
#endif
      break;

#ifdef FEATURE_DS_MOBILE_IP_DEREG
    case RSMI_DE_RRQ_STATE:
      MSG_MED( "MIP deregistered successfully",0,0,0);
      rsmi_transition_state(session, RSMI_DEREGISTERED_STATE);
      break;
#endif /* FEATURE_DS_MOBILE_IP_DEREG */

    default:
      ASSERT(0);
    } /* switch(rsm state) */
    break;


    /*-----------------------------------------------------------------------
                          RSM_RRQ_FAIL_NO_RETRY_EV

      Registration Request failed no retry is to happen:
        Initial RRQ: Post the Init-RRQ failed event to the meta state machine
                     and go to the No session state.
        Re-RRQ: Post the Re-RRQ failed event to the meta state machine and
                go to the No Session state.
        No Session/Registered: ignore 
    -----------------------------------------------------------------------*/
  case RSM_RRQ_FAIL_NO_RETRY_EV:
    switch(session->rsm_info.state)
    {
    case RSMI_INIT_RRQ_STATE:
      /*---------------------------------------------------------------------
          The state transition is happening before the event is posted to
          avoid entering the No Session state twice (and doing all that
          work).  (This is an optimization because we know how the
          interaction with the meta state machine works.)
      ---------------------------------------------------------------------*/
      rsmi_transition_state(session, RSMI_NO_SESSION_STATE);
      mip_meta_sm_post_event(MSM_INIT_RRQ_FAILED_EV);
      break;

    case RSMI_RE_RRQ_STATE:
      goto Re_RRQ_2_No_Session;
    

    case RSMI_NO_SESSION_STATE:
    case RSMI_REGISTERED_STATE:
#ifdef FEATURE_DS_MOBILE_IP_DEREG
    case RSMI_DEREGISTERED_STATE:
#endif /* FEATURE_DS_MOBILE_IP_DEREG */
      /* ignore */
#ifndef T_ARM
      MSG_MED("Got RRQ Failed no Retry event in %s state\n",
              rsmi_state_names[session->rsm_info.state], 0, 0);
#else
      MSG_MED("Got RRQ Failed no Retry event in state %d\n",
              session->rsm_info.state, 0, 0);
#endif
      break;

#ifdef FEATURE_DS_MOBILE_IP_DEREG
    case RSMI_DE_RRQ_STATE:
      MSG_MED( "MIP deregistration timed out.",0,0,0);
      rsmi_transition_state(session, RSMI_DEREGISTERED_STATE);
      break;
#endif /* FEATURE_DS_MOBILE_IP_DEREG */

    default:
      ASSERT(0);
    } /* switch(rsm state) */
    break;


    /*-----------------------------------------------------------------------
                           RSM_RRQ_FAIL_W_RETRY_EV

      Registration Request failed a retry should happen:  ensure that this
        Initial RRQ: reset count to 0 - fall through
        Re-RRQ: reset retry timeout, return to current state
        Registered/No Session: ignore
    -----------------------------------------------------------------------*/
  case RSM_RRQ_FAIL_W_RETRY_EV:
    switch(session->rsm_info.state)
    {
      case RSMI_INIT_RRQ_STATE:
#ifdef FEATURE_DS_MOBILE_IP_DEREG
      case RSMI_DE_RRQ_STATE:
#endif /* FEATURE_DS_MOBILE_IP_DEREG */
        session->rsm_info.rrq_cnt = 0;
        /* fall through */

      case RSMI_RE_RRQ_STATE:
        session->rsm_info.re_rrq_timeout =  session->base_retry_interval;
        rsmi_transition_state(session, session->rsm_info.state);
        break;
      
      case RSMI_NO_SESSION_STATE:
      case RSMI_REGISTERED_STATE:
#ifdef FEATURE_DS_MOBILE_IP_DEREG
      case RSMI_DEREGISTERED_STATE:
#endif /* FEATURE_DS_MOBILE_IP_DEREG */
        /* ignoree */
#ifndef T_ARM
        MSG_MED("Got RRQ Failed with Retry event in %s state\n",
                rsmi_state_names[session->rsm_info.state], 0, 0);
#else
        MSG_MED("Got RRQ Failed with Retry event in state %d\n",
                session->rsm_info.state, 0, 0);
#endif
        break;

    default:
      ASSERT(0);
    } /* switch(rsm state) */
    break;


    /*-----------------------------------------------------------------------
                                 RSM_MOVE_EV

      Move event
        Initial RRQ: reset count to 0 - fall through
        Re-RRQ: reset retry timeout, return to current state
        Registered State: reset the retry count to 0, reset the timeout
            value, cancel the lifetime timer (we have moved) and go to RRQ in
            Progress state.
        No Session State: go to RRQ in Progress state.
    -----------------------------------------------------------------------*/
  case RSM_MOVE_EV:
    switch(session->rsm_info.state)
    {
    case RSMI_INIT_RRQ_STATE:
      session->rsm_info.rrq_cnt = 0;
      /* fall through */

    case RSMI_RE_RRQ_STATE:
      session->rsm_info.re_rrq_timeout = session->base_retry_interval;
      rsmi_transition_state(session, session->rsm_info.state);
      break;
      
    case RSMI_REGISTERED_STATE:
      session->rsm_info.re_rrq_timeout = session->base_retry_interval;
      rsmi_transition_state(session, RSMI_RE_RRQ_STATE);
      break;

    case RSMI_NO_SESSION_STATE:
      rsmi_transition_state(session, RSMI_INIT_RRQ_STATE);
      break;
#ifdef FEATURE_DS_MOBILE_IP_DEREG
    case RSMI_DE_RRQ_STATE:
      MSG_MED( "MIP deregistration timeout.",0,0,0);
      rsmi_transition_state(session, RSMI_DEREGISTERED_STATE);
      break;

    case RSMI_DEREGISTERED_STATE:
      MSG_HIGH( "Ignore RSM_MOVE in DEREGISTERED state",0,0,0);
      break;
#endif /* FEATURE_DS_MOBILE_IP_DEREG */

    default:
      ASSERT(0);
    } /* switch(rsm state) */
    break;


    /*-----------------------------------------------------------------------
                             RSM_IMMED_RETRY_EV

      We can send a RRQ immediately - DSM buffers freed up for writes
        Initial RRQ: decrement retry count (make sure this is valid) and fall
                     through 
        Re-RRQ: reduce the rrq timeout (will be re-increased on state 
                transition), stop RRQ timer and return to current state
        De-RRQ: reduce the rrq timeout (will be re-increased on state 
                transition), stop RRQ timer and return to current state
        No Session/Registered/Deregistered: ignore
    -----------------------------------------------------------------------*/  
  case RSM_IMMED_RETRY_EV:
    switch(session->rsm_info.state)
    {
    case RSMI_INIT_RRQ_STATE:
      if(session->rsm_info.rrq_cnt > 0)
      {
        session->rsm_info.rrq_cnt -= 1;
      }
      else
      {
        ASSERT(0);
      }
      /* fall through */

    case RSMI_RE_RRQ_STATE:
      session->rsm_info.re_rrq_timeout /= MIP_RRQ_RETRY_BACKOFF_MULT;
      timer_result = ps_timer_cancel(session->rsm_info.reg_timer);
      ASSERT(timer_result != PS_TIMER_FAILURE);
      rsmi_transition_state(session, session->rsm_info.state);
      break;

#ifdef FEATURE_DS_MOBILE_IP_DEREG
    case RSMI_DE_RRQ_STATE:
      if(session->rsm_info.rrq_cnt > 0)
      {
        session->rsm_info.rrq_cnt -= 1;
      }
      else
      {
        ASSERT(0);
      }
      session->rsm_info.re_rrq_timeout /= MIP_RRQ_RETRY_BACKOFF_MULT;
      timer_result = ps_timer_cancel(session->rsm_info.dereg_timer);
      ASSERT(timer_result != PS_TIMER_FAILURE);
      rsmi_transition_state(session, RSMI_DE_RRQ_STATE);

#ifdef FEATURE_DS_MOBILE_IP_PERF
      /*---------------------------------------------------------------------
        This is the immediate retry event while we are in the RE_RRQ state.
        In this case we want to start the re-reg Delay.
      ---------------------------------------------------------------------*/
      if( (qw_cmp(mip_perf_info[PERF_MIP_DEREG_DELAY].ts_start, ts_null)==0)
          && ( session->rsm_info.state == RSMI_DE_RRQ_STATE) )
      {    
        /*-------------------------------------------------------------------
          Notify M.IP call performance that M.IP de-registration is starting
        -------------------------------------------------------------------*/
        mip_perf_delay(&mip_perf_info[PERF_MIP_DEREG_DELAY],
                       PERF_MIP_DEREG_DELAY,
                       PERF_START_TS);
      }
#endif /* FEATURE_DS_MOBILE_IP_PERF */
      break;

    case RSMI_DEREGISTERED_STATE:
#endif /* FEATURE_DS_MOBILE_IP_DEREG */
    case RSMI_NO_SESSION_STATE:
    case RSMI_REGISTERED_STATE:
      /* ignore */
#ifndef T_ARM
      MSG_MED("Got Immediate Retry event in %s state\n",
              rsmi_state_names[session->rsm_info.state], 0, 0);
#else
      MSG_MED("Got Immediate Retry event in state %d\n",
              session->rsm_info.state, 0, 0);
#endif
      break;

    default:
      ASSERT(0);
    } /* switch(rsm state) */
    break;
    
    /*-----------------------------------------------------------------------
                           RSM_RRQ_FAIL_HA_UNAVAIL_EV

      The registration failed because the HA was unavailable
        Re-RRQ: post Re-RRQ failed event to MetaSM, goto No Session
        Init-RRQ: reset retry count, reset timer and increment ha index
        No Session/Registered: ignore
    -----------------------------------------------------------------------*/
  case RSM_RRQ_FAIL_HA_UNAVAIL_EV:
    switch(session->rsm_info.state)
    {
    case RSMI_INIT_RRQ_STATE:
      session->rsm_info.rrq_cnt = 0;
      session->rsm_info.re_rrq_timeout = session->base_retry_interval;
      session->curr_ha_index += 1;
      rsmi_transition_state(session, RSMI_INIT_RRQ_STATE);
      break;

    case RSMI_RE_RRQ_STATE:
      goto Re_RRQ_2_No_Session;

#ifdef FEATURE_DS_MOBILE_IP_DEREG
    case RSMI_DE_RRQ_STATE:
      MSG_MED( "MIP HA unavailable. Continue tear-down",0,0,0);
      rsmi_transition_state(session, RSMI_DEREGISTERED_STATE);
      break;

    case RSMI_DEREGISTERED_STATE:
#endif /* FEATURE_DS_MOBILE_IP_DEREG */
    case RSMI_NO_SESSION_STATE:
    case RSMI_REGISTERED_STATE:
      /* ignore */
#ifndef T_ARM
      MSG_MED("Got HA unavailable event in %s state\n",
              rsmi_state_names[session->rsm_info.state], 0, 0);
#else
      MSG_MED("Got HA unavailable event in state %d\n",
              session->rsm_info.state, 0, 0);
#endif
      break;

    default:
      ASSERT(0);
    } /* switch(rsm state) */
    break;


    /*-----------------------------------------------------------------------
                            RSMI_RRQ_TIMER_EXP_EV

      The registration timer expired - time to send a new registration
        Registered State/Re-RRQ: go to the Re-RRQ state
        Initial RRQ: return to the Initial RRQ State
        No Session State: assert()
    -----------------------------------------------------------------------*/
  case RSMI_RRQ_TIMER_EXP_EV:
    switch(session->rsm_info.state)
    {
    case RSMI_REGISTERED_STATE:
      MSG_MED ("Begin MIP re-registration",0,0,0);
      session->rsm_info.rrq_cnt = 0;
      session->rsm_info.re_rrq_timeout = session->base_retry_interval;
      /* fall through */

    case RSMI_RE_RRQ_STATE:
      rsmi_transition_state(session, RSMI_RE_RRQ_STATE);
      break;

    case RSMI_INIT_RRQ_STATE:
      rsmi_transition_state(session, RSMI_INIT_RRQ_STATE);
      break;

#ifdef FEATURE_DS_MOBILE_IP_DEREG
    case RSMI_DE_RRQ_STATE:
      MSG_MED( "RRQ Timer expired.  Resend deregistration",0,0,0);
      rsmi_transition_state(session, RSMI_DE_RRQ_STATE);
      break;

    case RSMI_DEREGISTERED_STATE:
      /*---------------------------------------------------------------------
        RRQ_TIMER_EXP_EV received while in DEREGISTERED state. - Ignore.
      ---------------------------------------------------------------------*/
      MSG_MED( "RRQ Timer expired while in DEREGISTERED state",0,0,0);
      break;
#endif /* FEATURE_DS_MOBILE_IP_DEREG */

    case RSMI_NO_SESSION_STATE:
      /*---------------------------------------------------------------------
        RRQ_TIMER_EXP_EV received while in NO_SESSION state. - Ignore.
      ---------------------------------------------------------------------*/
      MSG_MED( "RRQ Timer expired while in NO_SESSION state",0,0,0);
      break;

    default:
      ASSERT(0);

    } /* switch(rsm state) */
    break;


    /*-----------------------------------------------------------------------
                           RSMI_LIFE_TIMER_EXP_EV

      Lifetime timer expired
        Init-RRQ/No Session: assert()
        Re-RRQ: transition to No Session 
        Registered: throw error and go to No Session
    -----------------------------------------------------------------------*/
  case RSMI_LIFE_TIMER_EXP_EV:
    switch(session->rsm_info.state)
    {
    case RSMI_REGISTERED_STATE:
      MSG_ERROR("Got Life Timer Expired event in Registered state!\n", 0,0,0);
      /* fall through */

    case RSMI_RE_RRQ_STATE:
      goto Re_RRQ_2_No_Session;

#ifdef FEATURE_DS_MOBILE_IP_DEREG
    case RSMI_DE_RRQ_STATE:
      /*---------------------------------------------------------------------
        Deinstall IP filters 
      ---------------------------------------------------------------------*/
      MSG_ERROR( "Life Timer expired in de-rrq state!",0,0,0);
      rsmi_transition_state(session, RSMI_DEREGISTERED_STATE);
      break;

    case RSMI_DEREGISTERED_STATE:
      /*---------------------------------------------------------------------
        LIFE_TIMER_EXP_EV received while in DEREGISTERED state. - Ignore.
      ---------------------------------------------------------------------*/
      MSG_HIGH( "Life Timer expired while in DEREGISTERED state",0,0,0);
      break;
#endif /* FEATURE_DS_MOBILE_IP_DEREG */


    case RSMI_NO_SESSION_STATE:
      /*---------------------------------------------------------------------
        LIFE_TIMER_EXP_EV received while in NO_SESSION state. - Ignore.
      ---------------------------------------------------------------------*/
      MSG_ERROR( "Life Timer expired while in NO_SESSION state" ,0,0,0);
      break;

    case RSMI_INIT_RRQ_STATE:
    default:
      ASSERT(0);

    } /* switch(rsm state) */
    break;


    /*-----------------------------------------------------------------------
                           RSM_SOL_FAILED_EV

      Solicitation failed
        No Session/Re-RRQ/Registered: post Re-RRQ failure to Meta SM go to no
           session
        Init-RRQ: post Init-RRQ failure to Meta SM go to no session
    -----------------------------------------------------------------------*/
  case RSM_SOL_FAILED_EV:
    MSG_MED( "Solicitation Failed", 0, 0, 0 );
    session->mip_fail_reason = MIP_FAIL_REASON_SOL_TIMEOUT;

    switch(session->rsm_info.state)
    {
    case RSMI_INIT_RRQ_STATE:
      /*---------------------------------------------------------------------
        MVL - is this really still needed?  This event should never happen in
              this state...

        The state transition is happening before the event is posted to avoid
        entering the No Session state twice (and doing all that work).  (This
        is an optimization because we know how the interaction with the meta
        state machine works.)
      ---------------------------------------------------------------------*/
    case RSMI_NO_SESSION_STATE:
      rsmi_transition_state(session, RSMI_NO_SESSION_STATE);
      mip_meta_sm_post_event(MSM_INIT_RRQ_FAILED_EV);
      break;

    Re_RRQ_2_No_Session:
    /* Fall through */
    case RSMI_RE_RRQ_STATE:
    case RSMI_REGISTERED_STATE:
      /*---------------------------------------------------------------------
        The state transition is happening before the event is posted to avoid
        entering the No Session state twice (and doing all that work).  (This
        is an optimization because we know how the interaction with the meta
        state machine works.)
      ---------------------------------------------------------------------*/
      rsmi_transition_state(session, RSMI_NO_SESSION_STATE);
      mip_meta_sm_post_event(MSM_RE_RRQ_FAILED_EV);
      break;

#ifdef FEATURE_DS_MOBILE_IP_DEREG
    case RSMI_DE_RRQ_STATE:
      rsmi_transition_state(session, RSMI_DEREGISTERED_STATE);
      break;

    case RSMI_DEREGISTERED_STATE:
      /*---------------------------------------------------------------------
        SOL_FAILED_EV received while in DEREGISTERED state. - Ignore.
      ---------------------------------------------------------------------*/
      MSG_HIGH( "Ignore Sol Failed event while in DEREGISTERED state",0,0,0);
      break;
#endif /* FEATURE_DS_MOBILE_IP_DEREG */

    default:
      ASSERT(0);

    } /* switch(rsm state) */
    break;
    

    /*-----------------------------------------------------------------------
                                   DEFAULT
    -----------------------------------------------------------------------*/
  default:
    ASSERT(0);

  } /* switch(event) */

} /* mip_reg_sm_post_event() */



/*===========================================================================
FUNCTION MIP_REG_SM_POST_EVENT_ALL()

DESCRIPTION
  This function posts an event to ALL of the registration state machines.

PARAMETERS
  event: The event to be posted to the state machines.

RETURN VALUE
  None

DEPENDENCIES
  None

SIDE EFFECTS
  None
===========================================================================*/
void mip_reg_sm_post_event_all
(
  mip_reg_sm_event_type event
)
{
  int loop;

  /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  for(loop = 0; loop < MIP_MAX_SESSIONS; loop += 1)
  {
    mip_reg_sm_post_event(mip_ses_info_array + loop, event);
  }
} /* mip_reg_sm_post_event_all() */



/*= = = = = = = = = = = = = = = = = = = = = = = = = = = = = = = = = = = = = =
                          LOCAL FUNCTION DEFINTIONS
= = = = = = = = = = = = = = = = = = = = = = = = = = = = = = = = = = = = = =*/
/*===========================================================================
FUNCTION RSMI_RRQ_TIMER_CB()

DESCRIPTION
  This function is the callback for the expiration of the registration
  request timer.

PARAMETERS
  param: this is passed in as a void* put it should point to the relevant
  Mobile IP session info block.

RETURN VALUE
  None

DEPENDENCIES
  None

SIDE EFFECTS
  None
===========================================================================*/
void rsmi_rrq_timer_cb
(
  void *param
)
{
/*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

#ifdef FEATURE_DS_MOBILE_IP_PERF
  /*-------------------------------------------------------------------------
    If we are starting re-registration, notify the MIP performance module
  -------------------------------------------------------------------------*/
  if ( ((mip_session_info_type *) param)->rsm_info.state == 
       RSMI_REGISTERED_STATE )
  {    
    mip_perf_delay(&mip_perf_info[PERF_MIP_REREG_DELAY],
                   PERF_MIP_REREG_DELAY,
                   PERF_START_TS);
  }
#endif /* FEATURE_DS_MOBILE_IP_PERF */
  
  mip_reg_sm_post_event((mip_session_info_type*)param, RSMI_RRQ_TIMER_EXP_EV);

} /* rsmi_rrq_timer_cb() */

#ifdef FEATURE_DS_MOBILE_IP_DEREG
/*===========================================================================
FUNCTION RSMI_DERRQ_TIMER_CB()

DESCRIPTION
  This function is the callback for the expiration of the deregistration
  request timer.

PARAMETERS
  param: this is passed in as a void* put it should point to the relevant
  Mobile IP session info block.

RETURN VALUE
  None

DEPENDENCIES
  None

SIDE EFFECTS
  None
===========================================================================*/
void rsmi_derrq_timer_cb
(
  void *param
)
{
/*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

#ifdef FEATURE_DS_MOBILE_IP_PERF
  /*-------------------------------------------------------------------------
    If we are starting re-registration, notify the MIP performance module
  -------------------------------------------------------------------------*/
  if ( ( ( (mip_session_info_type *) param)->rsm_info.state == 
               RSMI_REGISTERED_STATE ) ||
       ( ( (mip_session_info_type *) param)->rsm_info.state == 
               RSMI_RE_RRQ_STATE ) ) 
  { 
    {
      mip_perf_delay(&mip_perf_info[PERF_MIP_REREG_DELAY],
                     PERF_MIP_DEREG_DELAY,
                     PERF_START_TS);
    }
  }
#endif /* FEATURE_DS_MOBILE_IP_PERF */
  
  mip_reg_sm_post_event((mip_session_info_type*)param,RSMI_RRQ_TIMER_EXP_EV);

} /* rsmi_derrq_timer_cb() */
#endif /* FEATURE_DS_MOBILE_IP_DEREG */


/*===========================================================================
FUNCTION RSMI_LIFE_TIMER_CB()

DESCRIPTION
  This function is the callback for the expiration of the registration
  request timer.

PARAMETERS
  param: this is passed in as a void* put it should point to the relevant
  Mobile IP session info block.

RETURN VALUE
  None

DEPENDENCIES
  None

SIDE EFFECTS
  None
===========================================================================*/
void rsmi_life_timer_cb
(
  void *param
)
{
  /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/
  mip_reg_sm_post_event((mip_session_info_type*)param,
                        RSMI_LIFE_TIMER_EXP_EV);

} /* rsmi_life_timer_cb() */


/*===========================================================================
FUNCTION RSMI_RRQ_HOLDOFF_TIMER_CB()

DESCRIPTION
  This function is the callback for the expiration of the registration
  holdoff timer.  It sets the flag that indicates it has been at least
  one second since the last RRQ.

PARAMETERS
  param: pstimer passes a pointer to the session info block corresponding  
         to the timer that expires, which was provided when the timer was 
         allocated.

RETURN VALUE
  None

DEPENDENCIES
  None

SIDE EFFECTS
  This session's one_second_since_last_rrq flag is set asynchronously.  
  This must be the only place where this value is written.
===========================================================================*/
void rsmi_rrq_holdoff_timer_cb
(
  void *param
)
{
  /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/
  ((mip_session_info_type*)param)->rsm_info.one_second_since_last_rrq = TRUE;

} /* rsmi_rrq_holdoff_timer_cb() */


/*===========================================================================
FUNCTION RSMI_TRANSITION_STATE()

DESCRIPTION
  This function changes the state of the registration state machine and
  performs the actions associated with entry into the state.

PARAMETERS
  None

RETURN VALUE
  None

DEPENDENCIES
  None

SIDE EFFECTS
  None
===========================================================================*/
void rsmi_transition_state
(
  mip_session_info_type *session,
  rsmi_state_type new_state  
)
{
#ifdef FEATURE_DATA_MM
  ip_addr_type                          ip_addr;
  dss_id_info_type                      id;
  dss_iface_ioctl_id_type               ioctl_id;
  sint15                                ps_errno;
  dss_iface_ioctl_phys_link_state_type  phys_state;
  dss_iface_ioctl_state_type            iface_state;
#else
  dss_iface_status_type iface_status;
#endif /* FEATURE_DATA_MM */
  ps_timer_error_type timer_result;

  /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

#ifndef T_ARM
  MSG_MED("RegSM %d from '%s' to '%s'\n",
          MIP_SES_PTR_2_INDEX(session),
          rsmi_state_names[session->rsm_info.state],
          rsmi_state_names[new_state]);
#else
  MSG_MED("RegSM %d from %d to %d\n",
          MIP_SES_PTR_2_INDEX(session),
          session->rsm_info.state,
          new_state);
#endif
  
  /*-------------------------------------------------------------------------
    Perform action associated with entering into this state.
  -------------------------------------------------------------------------*/
  switch(new_state)
  {
    /*-----------------------------------------------------------------------
                               RSMI_NO_SESSION_STATE

      Reset the FA information
      Cancel reg timer
      Set current home addr to provisioned value
      Set current rev tun pref to provisioned value
    -----------------------------------------------------------------------*/
  case RSMI_NO_SESSION_STATE:
    /*-----------------------------------------------------------------------
      Since the MIP session has ended, if this is the last active session,
      restore the max solicitation retries to the initial value for the next
      call setup.
    -----------------------------------------------------------------------*/
    /* if (mip_in_session()) - add this when multiple sessions supported */
    MIP_SOL_SM_SET_MAX_RETRIES( SSM_INITIAL_SOL_RETRIES );

    mip_meta_sm_set_in_flow(
      DS_FLOW_MIP_REG1_MASK >> MIP_SES_PTR_2_INDEX(session),
      TRUE);
    timer_result = ps_timer_cancel(session->rsm_info.reg_timer);
#ifdef FEATURE_DS_MOBILE_IP_DEREG
    timer_result = ps_timer_cancel(session->rsm_info.dereg_timer);
#endif /* FEATURE_DS_MOBILE_IP_DEREG */
    timer_result = ps_timer_cancel(session->rsm_info.life_timer);
    session->curr_ha_index = 0;
    session->reg_lifetime = 0;
    session->rsm_info.rrq_cnt = 0;
    session->rsm_info.re_rrq_timeout = session->base_retry_interval;
    session->rsm_info.rtt_estimate = 0;
    MSG_MED ("Set RRP latency estimate to %d", 
             session->rsm_info.rtt_estimate, 0, 0);
    break;

    /*-----------------------------------------------------------------------
                             RSMI_INIT_RRQ_STATE

      Set the force_dormancy variable to FALSE at the begining of a call. We
      want to force the mobile to go dormant only on re-regsistrations and  
      not during intial call setup
    -----------------------------------------------------------------------*/
  case RSMI_INIT_RRQ_STATE:
    session->rsm_info.force_dormancy = FALSE;
    MSG_LOW("Set force_dormancy val to %d",
               session->rsm_info.force_dormancy,0,0);

    break;

    /*-----------------------------------------------------------------------
                             RSMI_RE_RRQ_STATE
                                   
      If the previous state was not RSMI_RE_RRQ_STATE, determine if the call 
      was dormant prior to sending the RRQ
    -----------------------------------------------------------------------*/
  case RSMI_RE_RRQ_STATE:

#ifdef FEATURE_DS_MOBILE_IP_PERF
    /*-----------------------------------------------------------------------
      For PZID change initiated handoff, start reregistration timer here
    -----------------------------------------------------------------------*/
    if ( session->rsm_info.state == RSMI_REGISTERED_STATE &&
         qw_cmp(mip_perf_info[PERF_MIP_REREG_DELAY].ts_start, ts_null) == 0 )
    {    
      /*---------------------------------------------------------------------
        Notify M.IP call performance that M.IP re-registration is starting.
      ---------------------------------------------------------------------*/
      mip_perf_delay(&mip_perf_info[PERF_MIP_REREG_DELAY],
                     PERF_MIP_REREG_DELAY,
                     PERF_START_TS);
    }
#endif /* FEATURE_DS_MOBILE_IP_PERF */

    /*-----------------------------------------------------------------------
      Check if we were dormant before the re-reg timer expired for the 1st
      time.  Save this state to be restored after reg. is complete.
    -----------------------------------------------------------------------*/
    if(session->rsm_info.state != RSMI_RE_RRQ_STATE)
    {
#ifndef FEATURE_DATA_MM
      dss_get_iface_status(IFACE_UM_PPP_STACK, &iface_status);
      if(iface_status.call_state == IFACE_DORMANT)
      {
        session->rsm_info.force_dormancy = TRUE;
      }
      else
      {
        session->rsm_info.force_dormancy = FALSE;
      }
#else
  
      id.type = DSS_APP_ID;
      id.info.app_id = mipio_get_app_id();

      if( (dss_get_iface_id(id, &ioctl_id) < 0) ||
          (dss_iface_ioctl(&ioctl_id,
                           DSS_IFACE_IOCTL_GET_PHYS_LINK_STATE,
                           &phys_state,
                           &ps_errno ) < 0) ||
          (dss_iface_ioctl(&ioctl_id,
                           DSS_IFACE_IOCTL_GET_STATE,
                           &iface_state,
                           &ps_errno ) < 0) )
      {
        /*-------------------------------------------------------------------
          To be safe don't go dormant   
        -------------------------------------------------------------------*/
        MSG_ERROR("Unable to get iface %d:%d state", 
                  ioctl_id.name, ioctl_id.instance, 0); 
        session->rsm_info.force_dormancy = FALSE;
      }
      else if((iface_state == IFACE_UP || 
               iface_state == IFACE_ROUTEABLE) &&
              (phys_state == PHYS_LINK_DOWN || 
               phys_state == PHYS_LINK_GOING_DOWN))
      {
        session->rsm_info.force_dormancy = TRUE;
      }
      else
      {
        session->rsm_info.force_dormancy = FALSE;
      }
#endif /* FEATURE_DATA_MM */
      MSG_MED("Dormancy mode is %d",session->rsm_info.force_dormancy,0,0);
    }
    break;

#ifdef FEATURE_DS_MOBILE_IP_DEREG
    /*-----------------------------------------------------------------------
                             RSMI_DE_RRQ_STATE
                                   
    -----------------------------------------------------------------------*/
  case RSMI_DE_RRQ_STATE:
#ifdef FEATURE_DS_MOBILE_IP_PERF
    /*---------------------------------------------------------------------
      Notify M.IP call performance that M.IP re-registration has stopped.
    ---------------------------------------------------------------------*/
    if (session->rsm_info.state == RSMI_RE_RRQ_STATE)
    {  
      mip_perf_delay(&mip_perf_info[PERF_MIP_REREG_DELAY],
                     PERF_MIP_REREG_DELAY,
                     PERF_END_TS);
    }

    /*-----------------------------------------------------------------------
      This should only happen if there was data traffic since last de-reg so
      we check to see if we are transitioning from REGISTERED_STATE or 
      RE_RRQ_STATE.
    -----------------------------------------------------------------------*/
    if ( ( (session->rsm_info.state == RSMI_REGISTERED_STATE) || 
           (session->rsm_info.state == RSMI_RE_RRQ_STATE) ) &&
         qw_cmp(mip_perf_info[PERF_MIP_DEREG_DELAY].ts_start, ts_null)==0 )
    {    
      /*---------------------------------------------------------------------
        Notify M.IP call performance that M.IP de-registration is starting.
      ---------------------------------------------------------------------*/
      mip_perf_delay(&mip_perf_info[PERF_MIP_DEREG_DELAY],
                     PERF_MIP_DEREG_DELAY,
                     PERF_START_TS);
    }
#endif /* FEATURE_DS_MOBILE_IP_PERF */
    break;
#endif /* FEATURE_DS_MOBILE_IP_DEREG */

    /*-----------------------------------------------------------------------
                            RSMI_REGISTERED_STATE

      comments inlined below
    -----------------------------------------------------------------------*/
  case RSMI_REGISTERED_STATE:
#ifdef FEATURE_DS_MOBILE_IP_PERF
    /*-----------------------------------------------------------------------
      Notify M.IP call performance that M.IP re-registration is complete
      only if we are in the re-rrq state.  If we are in the initial-rrq
      state, notify M.IP call performance that M.IP registration is 
      complete.
    -----------------------------------------------------------------------*/
    if (session->rsm_info.state == RSMI_RE_RRQ_STATE)
    {  
      mip_perf_delay(&mip_perf_info[PERF_MIP_REREG_DELAY],
                     PERF_MIP_REREG_DELAY,
                     PERF_END_TS);  

      /*---------------------------------------------------------------------
        If M.IP registration was due to a handoff, notify M.IP call 
        performance that M.IP handoff is complete.
      ---------------------------------------------------------------------*/      
      if (qw_cmp(mip_perf_info[PERF_TOTAL_HANDOFF_DELAY].ts_start, 
                 ts_null) != 0)       
      {      
        mip_perf_delay(&mip_perf_info[PERF_TOTAL_HANDOFF_DELAY],
                       PERF_TOTAL_HANDOFF_DELAY,
                       PERF_END_TS);        
      }
    }
    else if (session->rsm_info.state == RSMI_INIT_RRQ_STATE) 
    {
      mip_perf_delay(&mip_perf_info[PERF_MIP_INIT_REG_DELAY],
                     PERF_MIP_INIT_REG_DELAY,
                     PERF_END_TS);

      /*---------------------------------------------------------------------
        For sockets (MT0) calls, call is up now -> notify MIP performance.
      ---------------------------------------------------------------------*/
#ifndef FEATURE_DATA_MM
      if (rmsmi_info.state == RMSM_RM_NO_MIP_STATE)
#else
      if (!ds707_rmsm_is_packet_call())
#endif /* FEATURE_DATA_MM */
      {
        mip_perf_delay(&mip_perf_info[PERF_TOTAL_CALL_SETUP_DELAY],
                       PERF_TOTAL_CALL_SETUP_DELAY,
                       PERF_END_TS);
      }
    }
#endif /* FEATURE_DS_MOBILE_IP_PERF */
    break;

#ifdef FEATURE_DS_MOBILE_IP_DEREG
    /*-----------------------------------------------------------------------
                            RSMI_DEREGISTERED_STATE
    -----------------------------------------------------------------------*/
  case RSMI_DEREGISTERED_STATE:
#ifdef FEATURE_DS_MOBILE_IP_PERF
    /*-----------------------------------------------------------------------
      Notify M.IP call performance that M.IP de-registration is complete
    -----------------------------------------------------------------------*/
    mip_perf_delay(&mip_perf_info[PERF_MIP_DEREG_DELAY],
                   PERF_MIP_DEREG_DELAY,
                   PERF_END_TS);  

#endif /* FEATURE_DS_MOBILE_IP_PERF */
    break;
#endif /* FEATURE_DS_MOBILE_IP_DEREG */

    /*-----------------------------------------------------------------------
                                   default

      This should NEVER happen!
    -----------------------------------------------------------------------*/
  default:
    ASSERT(0);
  } /* switch( new state ) */

  /*-------------------------------------------------------------------------
    Transition into state
  -------------------------------------------------------------------------*/
  session->rsm_info.state = new_state;

  /*-------------------------------------------------------------------------
    Perform actions that occur after state transition
  -------------------------------------------------------------------------*/
  switch(new_state)
  {
    /*-----------------------------------------------------------------------
                            RSMI_NO_SESSION_STATE

      Nothing to do
    -----------------------------------------------------------------------*/
  case RSMI_NO_SESSION_STATE:
#ifdef FEATURE_DS_MOBILE_IP_DEREG
    mip_perf_delay(&mip_perf_info[PERF_MIP_DEREG_DELAY],
                   PERF_MIP_DEREG_DELAY,
                   PERF_CANCEL_TS);
#endif /* FEATURE_DS_MOBILE_IP_DEREG */
    break;

    /*-----------------------------------------------------------------------
                            RSMI_REGISTERED_STATE

      comments inlined below
    -----------------------------------------------------------------------*/
  case RSMI_REGISTERED_STATE:

    /*-----------------------------------------------------------------------
      Now that we are registered, increase the max solicitation retries 
      so we are more tolerant of interruptions (e.g. registration/SMS/etc.
      in hybrid devices) during a handoff once we already have a 
      registration.
    -----------------------------------------------------------------------*/
    MIP_SOL_SM_SET_MAX_RETRIES( SSM_HANDOFF_SOL_RETRIES );

    /*-----------------------------------------------------------------------
      If we are in a laptop call and all the sessions are registered, set 
      the PPP mode to PSEUDO_NET. This needs to be done irrespecive of the 
      flow control.
    -----------------------------------------------------------------------*/
#ifdef FEATURE_DATA_MM
    if ( ds707_rmsm_is_packet_call() && mip_reg_sm_all_sessions_regd() )
#else
    if ( (rmsmi_info.state != RMSM_RM_NO_MIP_STATE) &&
         mip_reg_sm_all_sessions_regd() )
#endif
    {
      netmdl_set_mode(IFACE_UM_PPP_STACK, PPP_PSEUDO_NET_MODE);
    }

    /*-----------------------------------------------------------------------
      Turn on inbound flow both to the SIO (get out of internal mode) and
      from the SIO
    -----------------------------------------------------------------------*/
    // The call to sm_internal_flow below should set the Um into
    // internal mode regardless of transition from controlled->flow
    // since Um was set to internal without flow controlling

    // This call must be made after setting the state to registered, since 
    // in_flow checks to see if all mip sessions are registered before 
    // setting the Um back to pseudo-network mode.

    mip_meta_sm_set_in_flow(
      DS_FLOW_MIP_REG1_MASK >> MIP_SES_PTR_2_INDEX(session),
      TRUE);

    MSG_MED( "MIP SUCCESS, calling callbacks", 0, 0, 0 );
    mip_call_event_callback(MIP_SUCCESS_EV);

    /*-----------------------------------------------------------------------
      Cancel the registration and lifetime timers
    -----------------------------------------------------------------------*/
    timer_result = ps_timer_cancel(session->rsm_info.reg_timer);
    ASSERT(timer_result != PS_TIMER_FAILURE);
    timer_result = ps_timer_cancel(session->rsm_info.life_timer);
    ASSERT(timer_result != PS_TIMER_FAILURE);

    /*-----------------------------------------------------------------------
      Set the timers
    -----------------------------------------------------------------------*/
    rsmi_set_registered_state_timers(session);

    /*-----------------------------------------------------------------------
      Set the IP address of the mobile to that in the RRP
      (could change from requested static IP or be assigned dynamically)
    -----------------------------------------------------------------------*/
#ifdef FEATURE_DATA_MM
#ifdef FEATURE_DATA_PS_UNSUPPORTED
#error code not present
#endif /* FEATURE_DATA_PS_UNSUPPORTED */
    ip_addr.type = IPV4_ADDR;
    ip_addr.addr.v4 = htonl(session->home_addr);
    MSG_MED ("Set IP addr to %x", ip_addr.addr.v4,0,0);
    ps_iface_set_addr (ppp_cb_array[PPP_UM_SN_DEV].rx_iface_ptr, &ip_addr);
#else
    dsiface_set_ip_addr(IFACE_UM_PPP_STACK, htonl(session->home_addr));
#endif /* FEATURE_DATA_MM */
    session->rsm_info.rrq_cnt = 0;
    session->rsm_info.re_rrq_timeout = session->base_retry_interval;

    /*-----------------------------------------------------------------------
      Release the call if the call was dormant prior to start of 
      re-registration
    -----------------------------------------------------------------------*/
    if(session->rsm_info.force_dormancy)
    {
      MSG_HIGH("Registration done. Forcing dormancy",0,0,0);
      dss_force_dormancy(session->rsm_info.app_id);
    }
    else
    {
      MSG_MED("Call was not dormant prior to reg",0,0,0);
    }
    /*-----------------------------------------------------------------------
      Reset rereg_reqd flag since we want to know if any data transfer takes 
      place after the registration. When any data is transferred, PPP code 
      would set the flag. Note that this behavior is configurable in NV and 
      rrq_if_tfrk stores this preference.
    -----------------------------------------------------------------------*/
    if (session->rrq_if_tfrk == TRUE)
    {
      mip_is_rereg_reqd = FALSE;
    }
    break;
  
    /*-----------------------------------------------------------------------
                              RSMI_INIT_RRQ_STATE

      Turn off inbound flow both to the SIO (get out of internal mode) and
      from the SIO and if another RRQ is to be sent send it and start timer,
      otherwise post the RRQ Failed w/o Retry event.
    -----------------------------------------------------------------------*/
  case RSMI_INIT_RRQ_STATE:

    mip_meta_sm_set_in_flow(
      DS_FLOW_MIP_REG1_MASK >> MIP_SES_PTR_2_INDEX(session),
      FALSE);

    if(session->rsm_info.rrq_cnt >= session->rsm_info.max_rrqs)
    {
      MSG_MED("HA %d not responding trying next",session->curr_ha_index,0,0);
      session->curr_ha_index += 1;
      session->rsm_info.rrq_cnt = 0;
      session->rsm_info.re_rrq_timeout = session->base_retry_interval;
    }

    if (session->curr_ha_index < MAX_HA_ADDRS  &&
        session->ha_addrs[session->curr_ha_index] != UNSET_HA_ADDR)
    {
      if (!mip_sol_sm_active())
      {
        mip_outmsg_send_rrq(session);
      }
      /*---------------------------------------------------------------------
        We add an offset to the rrq_timeout to avoid the stale challenge 
        problem caused by the request going out at fixed intervals
      ---------------------------------------------------------------------*/
      timer_result = ps_timer_start(session->rsm_info.reg_timer,
                                    session->rsm_info.re_rrq_timeout + 
                                    session->retry_interval_offset + 
                                    session->rsm_info.rtt_estimate);
      ASSERT(timer_result != PS_TIMER_FAILURE);

      MSG_MED ("Set registration timer for %dms\n", 
               session->rsm_info.re_rrq_timeout +
               session->retry_interval_offset +
               session->rsm_info.rtt_estimate, 
               0, 0);
      session->retry_interval_offset = 0;

      if (!mip_sol_sm_active())
      {
        session->rsm_info.rrq_cnt += 1;
        session->rsm_info.re_rrq_timeout *= MIP_RRQ_RETRY_BACKOFF_MULT;
      }
    }
    else
    {
      MSG_MED("Init RRQ failed - giving up", 0, 0, 0);

      session->mip_fail_reason = MIP_FAIL_REASON_RRQ_TIMEOUT;
      mip_reg_sm_post_event(session, RSM_RRQ_FAIL_NO_RETRY_EV);
    }
    break;


    /*-----------------------------------------------------------------------
                              RSMI_RE_RRQ_STATE

      Change Um stack to internal mode, send RRQ, start RRQ timer, 
      and increment backoff if it hasn't hit the ceiling.
    -----------------------------------------------------------------------*/
  case RSMI_RE_RRQ_STATE:

    timer_result = ps_timer_cancel(session->rsm_info.reg_timer);
    ASSERT(timer_result != PS_TIMER_FAILURE);

    /*---------------------------------------------------------------------
      We want to send the RRQ only when rereg_reqd flag is set, i.e., 
      either the rrr_if_tfrk optimization is off or optimization is on and 
      some data has been transferred since previous registration.
    ---------------------------------------------------------------------*/
    if ( mip_is_rereg_reqd == TRUE )
    {
      netmdl_set_mode(IFACE_UM_PPP_STACK, PPP_INTERNAL_MODE);
      if (!mip_sol_sm_active())
      {
        mip_outmsg_send_rrq(session);
      }

      /*---------------------------------------------------------------------
        We add an offset to the rrq_timeout to avoid the stale challenge 
        problem caused by the request going out at fixed intervals
      ---------------------------------------------------------------------*/
      timer_result = ps_timer_start(session->rsm_info.reg_timer,
                                    session->rsm_info.re_rrq_timeout +
                                    session->retry_interval_offset +
                                    session->rsm_info.rtt_estimate);
      ASSERT(timer_result != PS_TIMER_FAILURE);

      MSG_MED ("Set registration timer for %dms\n", 
               session->rsm_info.re_rrq_timeout +
               session->retry_interval_offset +
               session->rsm_info.rtt_estimate,
               0, 0);
      session->retry_interval_offset = 0;

      if (!mip_sol_sm_active())
      {
        /*-------------------------------------------------------------------
          Calculate the RRQ timeout for the next RRQ sent (next time 
          through this state transition, not the current RRQ timeout).
        -------------------------------------------------------------------*/
        if(session->rsm_info.re_rrq_timeout <= RSMI_MAX_RETRY_BACKOFF_TIME)
        {
          session->rsm_info.re_rrq_timeout *= MIP_RRQ_RETRY_BACKOFF_MULT;
        }
      }
    }
    else
    {
      MSG_MED ("No IP traffic since last reg, don't rereg",0,0,0);
    }
    break;

#ifdef FEATURE_DS_MOBILE_IP_DEREG
    /*-----------------------------------------------------------------------
                              RSMI_DE_RRQ_STATE

      Change Um stack to internal mode, send De-RRQ, start De-RRQ timer, 
      and increment backoff if it hasn't hit the ceiling.
    -----------------------------------------------------------------------*/
  case RSMI_DE_RRQ_STATE:
    timer_result = ps_timer_cancel(session->rsm_info.dereg_timer);
    ASSERT(timer_result != PS_TIMER_FAILURE);

    if(session->rsm_info.rrq_cnt < session->rsm_info.max_derrqs)
    {
      netmdl_set_mode(IFACE_UM_PPP_STACK, PPP_INTERNAL_MODE);

      if (!mip_sol_sm_active())
      {
        mip_outmsg_send_dereg_rrq(session);
      }

      /*---------------------------------------------------------------------
        We add an offset to the rrq_timeout to avoid the stale challenge 
        problem caused by the request going out at fixed intervals
      ---------------------------------------------------------------------*/
      timer_result = ps_timer_start(session->rsm_info.dereg_timer,
                                    session->rsm_info.re_rrq_timeout +
                                    session->retry_interval_offset +
                                    session->rsm_info.rtt_estimate);
      ASSERT(timer_result != PS_TIMER_FAILURE);

      MSG_MED( "Set registration timer for %dms\n", 
               session->rsm_info.re_rrq_timeout +
               session->retry_interval_offset +
               session->rsm_info.rtt_estimate,0,0);
      session->retry_interval_offset = 0;

      if (!mip_sol_sm_active())
      {
        session->rsm_info.rrq_cnt += 1;
        session->rsm_info.re_rrq_timeout *= MIP_RRQ_RETRY_BACKOFF_MULT;
      }
    }
    else
    {
      MSG_MED( "Deregistration RRQ failed - giving up",0,0,0);
      mip_reg_sm_post_event(session, RSM_RRQ_FAIL_NO_RETRY_EV);
    }
    break;

    /*-----------------------------------------------------------------------
                            RSMI_DEREGISTERED_STATE

      comments inlined below
    -----------------------------------------------------------------------*/
  case RSMI_DEREGISTERED_STATE:

    /*-----------------------------------------------------------------------
      Cancel the registration and life timer. 
    -----------------------------------------------------------------------*/
    timer_result = ps_timer_cancel(session->rsm_info.dereg_timer);
    ASSERT(timer_result != PS_TIMER_FAILURE);

    timer_result = ps_timer_cancel(session->rsm_info.life_timer);
    ASSERT(timer_result != PS_TIMER_FAILURE);

    /*-----------------------------------------------------------------------
      Post an event to the DSSNET State machine that M. IP deregistration has
      completed successfully
    -----------------------------------------------------------------------*/
    mip_call_event_callback(MIP_DEREGED_EV);

    break;
#endif /* FEATURE_DS_MOBILE_IP_DEREG */

    /*-----------------------------------------------------------------------
                                   default

      This should NEVER happen!
    -----------------------------------------------------------------------*/
  default:
    ASSERT(0);
  } /* switch( new state ) */

} /* rsmi_transition_state() */



/*===========================================================================
FUNCTION RSMI_SET_REGISTERED_STATE_TIMERS()

DESCRIPTION
  This function sets both the lifetime and re-rrq timers for the registered
  state.

PARAMETERS
  session: pointer for the session info block in question.

RETURN VALUE
  None

DEPENDENCIES
  PS timers has to have been initialized, as does the Registration State
  Machine.

SIDE EFFECTS
  None
===========================================================================*/
void rsmi_set_registered_state_timers
(
  mip_session_info_type *session
)
{
  uint32  rrq_time;
  uint32  ms_lifetime;
  ps_timer_error_type  timer_result;

  /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  /*-------------------------------------------------------------------------
      Only set the lifetime and the RRQ timers if the lifetime is not
      infinite (0xFFFF) per RFC2002 S2.1.1
  -------------------------------------------------------------------------*/
  if(session->reg_lifetime < 0xFFFF)
  {
    ms_lifetime = session->reg_lifetime*1000;
    timer_result = ps_timer_start(session->rsm_info.life_timer,
                                  ms_lifetime);
    ASSERT(timer_result != PS_TIMER_FAILURE);
    MSG_MED("Set Lifetime timer for %ds\n", session->reg_lifetime, 0, 0);

    /*-----------------------------------------------------------------------
      if the pre-re-registration time or the provisioned max time to register
      is more than half of the registration lifetime, then set the RRQ timer
      to half of the lifetime
    -----------------------------------------------------------------------*/
    if(session->rsm_info.pre_re_rrq_time > (ms_lifetime/2) ||
       session->max_reg_time > (ms_lifetime/2))
    {
      MSG_MED("RRQ timer = lifetime/2\n",0,0,0);
      rrq_time = (ms_lifetime/2);
    }

    /*-----------------------------------------------------------------------
      else if the pre-re-registration time is 0 then set the RRQ timer to the
      lifetime minus the provisioned max time to register
    -----------------------------------------------------------------------*/
    else if(session->rsm_info.pre_re_rrq_time == 0)
    {
      MSG_MED("RRQ timer = lifetime - max reg time\n",0,0,0);
      rrq_time = ms_lifetime - session->max_reg_time;
    }

    /*-----------------------------------------------------------------------
      else (the pre-rrq time is good) then set the rrq timer to the lifetime
      minus the pre-rrq time.
    -----------------------------------------------------------------------*/
    else 
    {
      rrq_time = ms_lifetime - session->rsm_info.pre_re_rrq_time;
      MSG_MED("(re-RRQ timer -= pre_re_rrq_time) = %d", rrq_time,0,0);
    }

    timer_result = ps_timer_start(session->rsm_info.reg_timer, rrq_time);
    ASSERT(timer_result != PS_TIMER_FAILURE);
    MSG_LOW("Set RRQ timer for %ds\n", rrq_time/1000, 0, 0);
  }
  else
  {
    MSG_MED("Infinite reg - no RRQ timer\n", 0, 0, 0);
  }

} /* rsmi_set_registered_state_timers() */

/*===========================================================================
FUNCTION MIP_REG_SM_IMMED_REGISTRATION_ALL

DESCRIPTION
  This function posts IMMED_RETRY_EV to every registration state machine
  which is in RE_RRQ_STATE.

PARAMETERS
  None

RETURN VALUE
  None

DEPENDENCIES
  None

SIDE EFFECTS
  None
===========================================================================*/

void mip_reg_sm_immed_registration_all
(
  void
)
{
  int index;

  /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  for(index = 0; index < MIP_MAX_SESSIONS; index++)
  {
    if ( mip_ses_info_array[index].rsm_info.state == RSMI_RE_RRQ_STATE )
    {
      mip_reg_sm_post_event(mip_ses_info_array + index, RSM_IMMED_RETRY_EV);
    }
  }
} /* mip_reg_sm_immed_registration_all() */

/*===========================================================================
FUNCTION MIP_REG_SM_ALL_SESSIONS_REGD

DESCRIPTION
  Checks if all sessions are registered

PARAMETERS
  None

RETURN VALUE
  TRUE if all sessions are registered
  FALSE otherwise

DEPENDENCIES
  None

SIDE EFFECTS
  None
===========================================================================*/
boolean mip_reg_sm_all_sessions_regd
(
  void
)
{
  int     index;
/*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  for(index = 0; index < MIP_MAX_SESSIONS; index++)
  {
    if ( mip_ses_info_array[index].rsm_info.state != RSMI_REGISTERED_STATE )
    {
      return FALSE; 
    }
  }

  return TRUE;
} /* mip_reg_sm_all_sessions_regd */


/*===========================================================================
FUNCTION MIP_REG_SM_SESSION_ACTIVE()

DESCRIPTION

  This function checks if Mobile IP session currently has a valid 
  registration (registered or reregistering) on the Um interface.

  This function executes in the context of the application task.

DEPENDENCIES
  None.

PARAMETERS
  None.

RETURN VALUE

  TRUE:  If MIP is active
  FALSE: If MIP is not active

SIDE EFFECTS
  None.
===========================================================================*/
boolean mip_reg_sm_session_active
(
  mip_session_info_type *  session
)
{
/*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  /*-------------------------------------------------------------------------
    Hard coded to check MIP session #0 for now.  
    Change if multiple MIP sessions are supported.
  -------------------------------------------------------------------------*/
  session = &mip_ses_info_array[0];

  if( session->rsm_info.state == RSMI_REGISTERED_STATE ||
      session->rsm_info.state == RSMI_RE_RRQ_STATE )
  {
    return( TRUE );
  }
  else
  {
    return( FALSE );
  }

} /* mip_reg_sm_session_active() */


/*===========================================================================
FUNCTION MIP_REG_SM_SET_RTT_ESTIMATION

DESCRIPTION
  Sets the estimated RTT between sending RRQ and RRP based on the current 
  rrq_timeout timer value.

PARAMETERS
  None

RETURN VALUE
  none

DEPENDENCIES
  None

SIDE EFFECTS
  None
===========================================================================*/
void mip_reg_sm_set_rtt_estimation
(
  mip_session_info_type * session
)
{
  sint31  reg_timer_left;
/*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  if (ps_timer_is_running(session->rsm_info.reg_timer))
  {
    /*-----------------------------------------------------------------------
      Set the RTT estimation based on the time left in the re-RRQ timer
    -----------------------------------------------------------------------*/
    MSG_MED ("Re_rrq_to %d  offset %d  rtt_est %d", 
             session->rsm_info.re_rrq_timeout,
             session->retry_interval_offset,
             session->rsm_info.rtt_estimate);

    reg_timer_left = ps_timer_remaining(session->rsm_info.reg_timer);
    if (reg_timer_left == -1)
    {
      MSG_ERROR ("Reg timer doesn't exist! reset RTT estimate",0,0,0);
      session->rsm_info.rtt_estimate = 0;
      return;
    }

    session->rsm_info.rtt_estimate = (session->rsm_info.re_rrq_timeout /
                                      MIP_RRQ_RETRY_BACKOFF_MULT) +
                                     session->retry_interval_offset +
                                     session->rsm_info.rtt_estimate -
                                     reg_timer_left;
  }
  else
  {
    /*-----------------------------------------------------------------------
      Registration timer isn't running, so reset the RTT estimate to 0
    -----------------------------------------------------------------------*/
    session->rsm_info.rtt_estimate = 0;
  }

  MSG_MED ("Set RRP latency estimate to %d", 
           session->rsm_info.rtt_estimate, 0, 0);

} /* mip_reg_sm_set_rtt_estimation() */

#endif /* FEATURE_DS_MOBILE_IP */
