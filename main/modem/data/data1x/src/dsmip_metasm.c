/*===========================================================================

                         D S M I P _ M E T A S M . C

DESCRIPTION

 The Data Services Mobile IP Meta State Machine source file.

EXTERNALIZED FUNCTIONS
   mip_meta_sm_init()
     Initialize the Meta State Machine.
   mip_meta_post_event()
     Post an event to the Meta State Machine.

Copyright (c) 2000-2011 by QUALCOMM, Incorporated.  All Rights Reserved.
===========================================================================*/
/*===========================================================================

                      EDIT HISTORY FOR FILE

  $PVCSPath: O:/src/asw/COMMON/vcs/dsmip_metasm.c_v   1.51   12 Feb 2003 19:38:24   akuzhiyi  $
  $Header: //source/qcom/qct/modem/data/1x/mip/main/lite/src/dsmip_metasm.c#4 $ $DateTime: 2011/02/24 23:31:53 $ $Author: msankar $

when        who    what, where, why
--------    ---    ----------------------------------------------------------
02/25/11    ms     Ported MOBILE_IP_DEREG feature.
04/29/09    sn     Ported support for call throttle feature (DCTM).
06/02/08    ms     Fixed Critical/High Lint errors
11/10/06    an     Fixed compilation errors for 6020
11/01/06    an     Featurize ATCOP, RMSM, Async Fax
09/23/03    jd     Mainlined Irfan's changes for 6100 branch
08/18/03    jd     Featurized Irfan's changes (FEATURE_DATA_CURRENT_PPP_CB)
08/13/03    ifk    rx_iface_ptr moved to IPCP fsm in ps_iface_set_addr().
05/18/03    aku    Call mipio_ppp_close on transition to closed state to
                   ensure MIP ppp instance is closed for SimpleIP fallback.
05/14/03    jd     For multimode, call mipio_ppp_open instead of pppopen
                   directly to make sure 1x SN iface is forced
05/05/03    jd     Hard code UM_SN_IFACE for ioctls() in 
                   mip_meta_sm_set_in_flow() for proper MIP call termination
05/05/03    usb    Removed access to RMSM private info block for MM_DATA.
04/18/03    ak     Featurized new RMSM interface usage.
04/11/03    ak     Updated to use new RMSM interfaces.
02/12/03    aku    Added support for flow controlling non system sockets on a 
                   per interface basis.
12/29/02    jd     Moved mip_perf_init() into mip_meta_sm_init()
11/25/02    usb    Calling pppopen for MM also.
11/19/02    jd     for MM, instead of pppopen, set output iface ptr in acb
11/16/02    jd     Remove pppopen call by MIP during sockets call for MM
11/14/02    jd     Removed netmodel_mode from msmi_info (unused)
                   In MM builds, call ps_iface_set_addr instead of dsiface_;
                   call ds3g_siol_set_inbound flow instead of ds_set_in...;
                   changed API to post events to dssnet
10/09/02    jay    Changed #define MAX_NAI_LENGTH to MAX_NAI_LEN
08/19/02    jay    Clear the DMU state at the end of the call.
07/18/02    jay    Initialize DMU
07/11/02    jd     Removed featurization under FEATURE_DATA_PS since affected
                   code is under FEATURE_DS_MOBILE_IP and this is never 
                   turned on for non-IS-2000 targets.
06/03/02    jay    Renamed ds_fill_mip_atcop_val to ds_atcop_fill_mip_val
05/10/02    jd     moved to dsmip.c:
                   msmi_get_nv_info            -> mipi_get_nv_info
                   mip_meta_sm_reg_ses_info_cb -> mip_reg_ses_info_cb
                   mip_meta_sm_reg_pwd_info_cb -> mip_reg_pwd_info_cb
05/10/02    jd     moved session info array, ui session & password data
                   strutures, mip_meta_sm_config_session to dsmip.c in order
                   to seperate configuration functionality from metasm and
                   solve build dependency issues
04/17/02    jd     ds_fill_mip_vals now calls psi_get_nv_item - fixes bug we
                   were in dsi_wait (waiting on DS signal) instead of psi_
04/17/02    jd     Use global ps_nv_item when retrieving global NV item
03/21/02   jd/sjy  Check to see if all MIP sessions are registered before
                   setting Um interface to pseudonet mode.
02/20/02    sjy    Mobile IP performance now featurized under 
                   FEATURE_DS_MOBILE_IP_PERF
02/12/02    dwp    Cleanup header inclusion(s) for non DS builds.
02/11/02    jay    Erase passwd info entered by the user through callback
                   at the end of the call.
02/07/02    sjy    Added support for Mobile IP call performance metrics.
02/07/02    vsk    Fixed  header inclusion so that it builds for all targets
                   more code wrapped in FEATURE_DATA_PS
01/24/02    jay    Copy profile info from NV into atcop_val at the end of a
                   M.IP call.
01/23/02    jd     Cleaned up some comments.
01/17/02    jay    Erase the shadow nv at the end of MIP call.
                   Use shadow NV in the session block.
01/02/02    aku    Added calls to MIPIO_OPEN_IFACE() and 
                   mipio_pppclose()
11/27/01    mvl    Made sure that default values are used for NV items that
                   are supposed to have defaults when the item is not set.
11/26/01    mvl    Changed the calculation of max reg time such that it
                   allows for an extra RRQ attempt.  Some clean up.
11/21/01    aku    dssnet_sm_post_event now takes an additional acb_ptr arg
11/21/01    ss     Added code for reading new NV item: ds_mip_rrq_if_tfrk
10/24/01    js     Added definition for dsmip_info which includes all MIP
                   related information 
10/09/01    js     Added externalized function mip_meta_sm_config_session(), 
                   so that sessions configuration can be performed prior to 
                   the traffic channel is established. Modified 
                   msmi_get_nv_info() to handle the case where NV retrieval 
                   fails.  
09/17/01    js     Added header comment block for externalized functions.  
09/16/01    jd     Added accessor functions to set aforementioned callbacks
09/16/01    jd     Added registerable UI callbacks for NAI and password 
09/16/01    jd     Removed duplicate line of code that saves nai_length 
09/11/01    mvl    Changed some variable names for clarity.  Modified
                   algorithm so that the unset HA address is 255.255.255.255
                   - also added code to set primary address to 0 in NV if it
                   is unset.
08/24/01    mvl    Changed the calculation of the base retry interval so that
                   it starts at 1s.  Removed the assert() in post event if
                   not initialized - now just return.
08/23/01    aku    Clearing ma_info struct on ppp resync to ensure that M. IP 
                   reregisters
08/15/01    sjy    Renamed RMSMI_UM_RM_UP_STATE to new state name
                   RMSM_UM_RM_UP_STATE
07/31/01    mvl    Changed to way the set_in_flow() function works with
                   respect to setting the routing on the Um interface.
07/30/01    na     regsm, solsm & ma_info are initialized on transitions to
                   OPEN state.
07/29/01    jd     Added accessor function to see if mip is active
07/27/01    aku    Notify DSSNET state machine on transtioning to 
                   CLOSED_STATE
07/07/01    mvl    Initializing the PDSN DB
06/26/01    mvl    Check that the shared secrets actually contain information
                   otherwise the session info block is invalid.
06/21/01    mvl    added the function to flow control sockets
06/20/01    mvl    Added check such that the netmodel mode is never set to
                   NONE when restoring the old mode.
06/19/01    jd     Added assignment of MN-AAA spi to the session info block 
06/12/01    mvl    Completely rewrote to support latest design - which
                   includes updates to the NV read function.
05/23/01    sy     Added function msmi_get_nv_info() for retrieving NV items
01/15/01    mvl    Created module
===========================================================================*/

/*===========================================================================

                          INCLUDE FILES FOR MODULE

===========================================================================*/

#include "comdef.h"
#include "customer.h"

#ifdef FEATURE_DS_MOBILE_IP
#include "dsmip.h"
#include "nv.h"
#include "dsmip_metasm.h"
#include "dsmip_solsm.h"
#include "dsmip_io.h"
#include "msg.h"
#include "dsmip_regsm.h"
#include "dsmip_pdsn_db.h" 
#include "assert.h"

#ifdef FEATURE_DS_MOBILE_IP_PERF
#include "dsmip_perf.h"
#endif /* FEATURE_DS_MOBILE_IP_PERF */


#ifdef FEATURE_DS_MOBILE_IP_DMU
#include "dsmip_dmu.h"
#endif /* FEATURE_DS_MOBILE_IP_DMU */

#ifdef FEATURE_DATA_MM
#include "ds707_rmsm.h"
#include "dsat707mip.h"
#include "ps_ppp.h"
#include "ps_ip_addr.h"
#include "ps_ppp_fsm.h"
#include "ps_utils.h"
#include "ps_iface.h"
#include "ds3gsiolib.h"
#include "ps_mip_compat.h"
#else
#include "psi.h"
#include "dsmip_rmsm.h"
#include "dsi.h"
#include "dsatcop.h"
#endif /* FEATURE_DATA_MM */

/*---------------------------------------------------------------------------
  Make sure that the definition of NAI length is reasonable - does this work
  with acc?
---------------------------------------------------------------------------*/
#if MAX_NAI_LEN < NV_MAX_NAI_LENGTH
#error "MAX_NAI_LEN is too small!  Must be at least the same as NV_MAX_NAI_LEN"
#endif

/*= = = = = = = = = = = = = = = = = = = = = = = = = = = = = = = = = = = = = =
                            INTERNAL DEFINITIONS
= = = = = = = = = = = = = = = = = = = = = = = = = = = = = = = = = = = = = =*/
#ifndef regsm_test

#ifndef T_ARM
/*---------------------------------------------------------------------------
  These need to be kept in sync with the enums which they describe!
---------------------------------------------------------------------------*/
char *msmi_state_names[MSMI_MAX_STATE] =
{
  "Closed State",
  "Open State"
};

char *msmi_event_names[MSM_MAX_EV] =
{
  "Iface Up with M.IP",
  "Iface Up without M.IP",
  "Iface down",
  "M.IP Init RRQ failed",
  "M.IP Re-RRQ failed",
};
#endif /* ! T_ARM */


/*===========================================================================

                               INTERNAL DATA

===========================================================================*/

/*---------------------------------------------------------------------------
  The control block of the meta state machine.
---------------------------------------------------------------------------*/
msmi_info_type msmi_info;

/*---------------------------------------------------------------------------
  MIP Info structure - for debugging 
  - contains pointers to all MIP data structures.
---------------------------------------------------------------------------*/
struct
{
   mip_session_info_type        *mip_ses_info_array_vals;
#ifndef FEATURE_DATA_MM
   rmsmi_info_type              *rmsmi_info_vals;
#endif
   mip_pwd_info_type            *mip_pwd_info_vals;
   msmi_info_type               *msmi_info_vals;
   ssmi_info_type               *ssmi_info_vals;
} dsmip_info;


/*===========================================================================

                           FORWARD DECLARATIONS

===========================================================================*/
LOCAL void msmi_transition_state
(
  msmi_state_type new_state  
);



/*===========================================================================

                          EXTERNALIZED FUNCTIONS

===========================================================================*/

/*===========================================================================
FUNCTION MIP_META_SM_INIT()

DESCRIPTION
  This function initializes the meta state machine.

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
int mip_meta_sm_init
(
  void
)
{
/*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  /*-------------------------------------------------------------------------
    Initialize Mobile IP i/o module
  -------------------------------------------------------------------------*/
  if(mipio_init() == FALSE)
  {
    MSG_ERROR("M.IP initialization failed!", 0, 0, 0);
    return -1;
  }

  /*-------------------------------------------------------------------------
    Initialize all of the mobile IP state machines 
  -------------------------------------------------------------------------*/
  if(mip_sol_sm_init() < 0)
  {
    /*-----------------------------------------------------------------------
      Timer allocation probably failed
    -----------------------------------------------------------------------*/
    MSG_ERROR("M.IP SolSM init failed!", 0, 0, 0);
    return -1;
  }

  if(mip_reg_sm_init() < 0)
  {
    /*-----------------------------------------------------------------------
      Timer allocation probably failed
    -----------------------------------------------------------------------*/
    MSG_ERROR("M.IP RegSM init failed!", 0, 0, 0);
    return -1;
  }
  pdsn_db_init();

#ifdef FEATURE_DS_MOBILE_IP_DMU
  /*-------------------------------------------------------------------------
    Initialize DMU
  -------------------------------------------------------------------------*/
  dmu_init();
#endif /* FEATURE_DS_MOBILE_IP_DMU */

#ifdef FEATURE_DS_MOBILE_IP_PERF
  /*-------------------------------------------------------------------------
    Initialize MIP performance
  -------------------------------------------------------------------------*/
  mip_perf_init();
#endif /* FEATURE_DS_MOBILE_IP_PERF */

  /*-------------------------------------------------------------------------
    Set the initial state to its initial value
  -------------------------------------------------------------------------*/
  msmi_info.state = MSMI_CLOSED_STATE;
  msmi_info.inited = TRUE;

  /*------------------------------------------------------------------------
    Initialize the dsmip_info data structure
  ------------------------------------------------------------------------*/
  dsmip_info.mip_ses_info_array_vals    = &mip_ses_info_array[0];
#ifndef FEATURE_DATA_MM
  dsmip_info.rmsmi_info_vals            = &rmsmi_info;
#endif
  dsmip_info.mip_pwd_info_vals          = &mip_pwd_info;
  dsmip_info.msmi_info_vals             = &msmi_info;
  dsmip_info.ssmi_info_vals             = &ssmi_info;

  return 0;

} /* mip_meta_sm_init() */



/*===========================================================================
FUNCTION MIP_META_SM_POST_EVENT()

DESCRIPTION
  This function posts an event to the Agent solicitation state machine.

PARAMETERS
  event: The event that is being posted to the state machine.

RETURN VALUE
  None

DEPENDENCIES
  mip_meta_sm_init() has to have been called first.

SIDE EFFECTS
  None
===========================================================================*/
void mip_meta_sm_post_event
(
  mip_meta_sm_event_type event
)
{
#ifdef FEATURE_DATA_MM
  ip_addr_type  ip_addr;
#endif /* FEATURE_DATA_MM */
  int loop;
  /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  /*-------------------------------------------------------------------------
    If the state machine is not initialized then return immediately
  -------------------------------------------------------------------------*/
  if(msmi_info.inited != TRUE)
  {
    MSG_MED("Meta SM not initialized!", 0, 0, 0);
    return;
  }

#ifndef T_ARM
  MSG_MED("'%s' event posted to Meta SM.\n", msmi_event_names[event], 0, 0);
#else
  MSG_MED("Event %d posted to Meta SM.\n", event, 0, 0);
#endif

  /*-------------------------------------------------------------------------
    Take action based on event received and which state we are currently in.
  -------------------------------------------------------------------------*/
  switch(event)
  {
    /*-----------------------------------------------------------------------
                             MSM_IFACE_UP_W_MIP_EV

      PPP up with MIP supported - transition to the OPEN state if
      transitioning from the closed state initialize IO.
    -----------------------------------------------------------------------*/
  case MSM_IFACE_UP_W_MIP_EV:
    switch(msmi_info.state)
    {
    case MSMI_CLOSED_STATE:
      /*---------------------------------------------------------------------
        Reset the MA info.
      ---------------------------------------------------------------------*/
      memset(&ma_info, 0, sizeof(mip_ma_info_type));
      
      /*---------------------------------------------------------------------
        Set the UM interface (mobile's) source IP address to the value 
        stored in session info block.
      ---------------------------------------------------------------------*/
#ifdef FEATURE_DATA_MM
      ip_addr.type = IPV4_ADDR;
      ip_addr.addr.v4 = htonl(INDEX_2_MIP_SES_PTR(0)->home_addr);
      MSG_MED ("Set IP addr to %x", ip_addr.addr.v4,0,0);
      ps_iface_set_addr (ppp_cb_array[PPP_UM_SN_DEV].rx_iface_ptr, &ip_addr);
#else
      dsiface_set_ip_addr(IFACE_UM_PPP_STACK,
                          htonl(INDEX_2_MIP_SES_PTR(0)->home_addr));
#endif /* FEATURE_DATA_MM */
      /* fall through */

    case MSMI_OPEN_STATE:
      /*---------------------------------------------------------------------
        PPP is established, kick the solicitation state machine to get 
        the ball rolling.
      ---------------------------------------------------------------------*/
      mip_sol_sm_post_event(SSM_SOL_TRIGGER_EV);
      msmi_transition_state(MSMI_OPEN_STATE);
      break;

    default:
      ASSERT(0);
    } /* switch(state) */
    break;


    /*-----------------------------------------------------------------------
                           MSM_IFACE_UP_WO_MIP_EV
                              MSM_IFACE_DOWN_EV

      Either the interface went down, or it doesn't support Mobile IP -
      transition to the CLOSED state if OPEN otherwise ignore either event.
    -----------------------------------------------------------------------*/
  case MSM_IFACE_UP_WO_MIP_EV:
  case MSM_IFACE_DOWN_EV:
    switch(msmi_info.state)
    {
    case MSMI_CLOSED_STATE:
#ifndef T_ARM
      MSG_MED("%s event in Closed state!\n", msmi_event_names[event], 0, 0);
#else
      MSG_MED("Got %d in Closed state\n", event, 0, 0);
#endif
      break;

    case MSMI_OPEN_STATE:
      msmi_transition_state(MSMI_CLOSED_STATE);
      break;

    default:
      ASSERT(0);
    } /* switch(state) */
    break;


  /*-------------------------------------------------------------------------
                            MSM_MIP_RRQ_FAILED_EV
                            MSM_RE_RRQ_FAILED_EV

    check if there are any active M.IP registrations - if there are do
    nothing - if not do what needs to be done to Exit M.IP Mode and
    transition to the Closed state.  If already in in the closed state do
    nothing.
  -------------------------------------------------------------------------*/
  case MSM_INIT_RRQ_FAILED_EV:
  case MSM_RE_RRQ_FAILED_EV:
    switch(msmi_info.state)
    {
    case MSMI_CLOSED_STATE:
#ifndef T_ARM
      MSG_MED("%s event in Closed state\n", msmi_event_names[event], 0, 0);
#else
      MSG_MED("RRQ failed(%d) event in Closed state\n", event, 0, 0);
#endif      
      break;

    case MSMI_OPEN_STATE:
      for(loop = 0; loop < MIP_MAX_SESSIONS; loop += 1)
      {
        /*-------------------------------------------------------------------
          check if any session is active do nothing
        -------------------------------------------------------------------*/
        if(mip_ses_info_array[loop].reg_lifetime > 0)
        {
          MSG_MED("M.IP Ses %d active, ignoring event\n", loop, 0, 0);
          return;
        }
      }

      /*---------------------------------------------------------------------
        Reset the interface - if this was a Re_RRQ we don't want to force the
        link down (i.e. the second argument to mip_reset())
      ---------------------------------------------------------------------*/
      MSG_MED("No active sessions, shutting down\n",0,0,0);
      msmi_transition_state(MSMI_CLOSED_STATE);
      break;

    default:
      ASSERT(0);
    } /* switch(state) */
    break;

    /*-------------------------------------------------------------------------
                              MSM_MIP_BRING_DOWN_CMD

      Physical link up. wiating for MIP PPP to come up - transition to the
      OPENING state if transitioning from the closed state initialize IO.
    -------------------------------------------------------------------------*/
  case MSM_BRING_DOWN_MIP_EV:
    switch(msmi_info.state)
    {
      case MSMI_OPEN_STATE:
        mip_reg_sm_post_event_all(RSMI_DEREG_MIP_EV);
        break;
      case MSMI_CLOSED_STATE:
        MSG_HIGH("mip bring down cmd rcvd in %d state",msmi_info.state,0,0);
        break;
      default:
        /*lint -save -e506, -e774 */
        ASSERT(0);
        /*lint -restore */
    }
    break;

  default:
    ASSERT(0);
  } /* switch(event) */

} /* mip_meta_sm_post_event() */



/*===========================================================================
FUNCTION MIP_META_SM_OPEN()

DESCRIPTION
  Accessor function to see if Mobile IP is active or coming up.

PARAMETERS
  None

RETURN VALUE
  TRUE - mip is active or coming up
  FALSE - not active or coming up

DEPENDENCIES
  mip_meta_sm_init() has to have been called first.

SIDE EFFECTS
  None
===========================================================================*/
boolean mip_meta_sm_open
(
  void
)
{
  return (msmi_info.state == MSMI_OPEN_STATE ? TRUE : FALSE);
}


/*===========================================================================
FUNCTION MIP_META_SM_SET_IN_FLOW()

DESCRIPTION
  This function enables and disables flow control to both sockets and the
  serial port.  It also modifies the Um state - if flow is being disabled it
  then the Um mode is saved and changed to internal mode - when the flow is
  enabled the Um mode is restored.

PARAMETERS
  mask:    the flow control mask
  enabled: whether flow should be enabled

RETURN VALUE
  None

DEPENDENCIES
  mip_meta_sm_init() has to have been called first.

SIDE EFFECTS
  None
===========================================================================*/
void mip_meta_sm_set_in_flow
(
  uint32 mask,
  boolean enabled
)
{
  static uint32 flow_mask = 0;
#ifdef FEATURE_DATA_MM
  dss_iface_ioctl_id_type ioctl_id;
#endif /* FEATURE_DATA_MM */
/*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  /*-------------------------------------------------------------------------
    Flow sockets applications and the SIO
  -------------------------------------------------------------------------*/
#ifdef FEATURE_DATA_MM
  ioctl_id.name     = DSS_IFACE_IOCTL_CDMA_SN;
  ioctl_id.instance = 0;
  dssocki_set_non_sys_socket_flow(mask, enabled, &ioctl_id);
  ds3g_siolib_set_inbound_flow(mask, enabled ? DS_FLOW_ENABLE : DS_FLOW_DISABLE );
#else
  dssocki_set_non_sys_socket_flow(mask, enabled);
  ds_set_inbound_flow((uint16) mask, enabled);
#endif /* FEATURE_DATA_MM */

  /*-------------------------------------------------------------------------
    if data flow is enabled then we need to make sure that no one else needs
    it disabled - remove flag from the mask and if the mask is clear then
    change the flow.
  -------------------------------------------------------------------------*/
  if(enabled == TRUE)
  {
     flow_mask &= ~mask;
     MSG_LOW ("clear 0x%x mask, meta_sm flow now %x", mask, flow_mask,0);
     
     //nish: maybe we should only do this when Um is also UP - not
     //when it is being torn down.
#ifdef FEATURE_DATA_MM
    if ( (flow_mask == 0) &&
         (ds707_rmsm_is_mip_up()) &&
          mip_reg_sm_all_sessions_regd() )
#else
    if ( (flow_mask == 0) &&
         (rmsmi_info.state == RMSM_UM_RM_UP_STATE) &&
          mip_reg_sm_all_sessions_regd() )
#endif
    {
      netmdl_set_mode(IFACE_UM_PPP_STACK, PPP_PSEUDO_NET_MODE);
    }
  }
  else
  {
    /*-----------------------------------------------------------------------
      Data flow is being disabled - note who is doing it by setting flag in 
      the mask.  If the netmodel mode is None then we should store it (it is
      unintialized otherwise leave it alone - this is to make sure that
      multiple calls doesn't zap the original stored state.
    -----------------------------------------------------------------------*/
    flow_mask |= mask;
    MSG_LOW ("set 0x%x mask, meta_sm flow now %x", mask, flow_mask,0);

    netmdl_set_mode(IFACE_UM_PPP_STACK, PPP_INTERNAL_MODE);
  }

} /* mip_meta_sm_set_in_flow */


/*= = = = = = = = = = = = = = = = = = = = = = = = = = = = = = = = = = = = = =
                          LOCAL FUNCTION DEFINTIONS
= = = = = = = = = = = = = = = = = = = = = = = = = = = = = = = = = = = = = =*/
/*===========================================================================
FUNCTION MSMI_TRANSITION_STATE()

DESCRIPTION
  This function effectuates the transition to the state that is passed in

PARAMETERS
  new_state: the state to transition to.

RETURN VALUE
  None

DEPENDENCIES
  mip_meta_sm_init() has to have been called first.

SIDE EFFECTS
  None
===========================================================================*/
LOCAL void msmi_transition_state
(
  msmi_state_type new_state  
)
{
#ifndef FEATURE_DATA_MM
  sint15 l_errno;
#endif /* FEATURE_DATA_MM */
/*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

#ifndef T_ARM
  MSG_MED("Meta SM from '%s' to '%s' state\n",
          msmi_state_names[msmi_info.state],
          msmi_state_names[new_state],
          0);
#else
  MSG_MED("Meta SM from %d to %d state\n", msmi_info.state, new_state, 0);
#endif

  /*-------------------------------------------------------------------------
    Perform action associated before transitioning into new state.
  -------------------------------------------------------------------------*/
  switch(new_state)
  {
    /*-----------------------------------------------------------------------
                                 MSMI_CLOSED
    
      Nothing to do in this case
    -----------------------------------------------------------------------*/
  case MSMI_CLOSED_STATE:
    break;

    /*-----------------------------------------------------------------------
                                 MSMI_OPEN_STATE

      If the current state is MSMI_OPEN_STATE, this indicates a PPP resync. 
      Reset the MA info so that M. IP re-registers.
    -----------------------------------------------------------------------*/
  case MSMI_OPEN_STATE:

    /*-----------------------------------------------------------------------
      For MIP sockets calls, MIP does not call dss_pppopen(). But 
      dss_pppopen() is called here so that MIP can get Network events,
      which is required for proper cleanup of MIP sockets calls.
      (For laptop MIP calls, this would be the second time 
      that MIP is calling dss_pppopen(), but it would be ignored).
    -----------------------------------------------------------------------*/

    /*-----------------------------------------------------------------------
      HANDLE ERROR CODE FROM PPPOPEN AND TAKE APPROPRIATE ACTION
    -----------------------------------------------------------------------*/

#ifdef FEATURE_DATA_MM
    mipio_ppp_open();
#else
    MIPIO_OPEN_IFACE();
#endif /* FEATURE_DATA_MM */

    if(msmi_info.state == MSMI_OPEN_STATE)
    {
      MSG_MED("PPP resync. Initiate M. IP re-reg",0,0,0);
      memset(&ma_info, 0, sizeof(mip_ma_info_type));
    }
#ifdef FEATURE_DS_MOBILE_IP_PERF
    else
    {
      /*---------------------------------------------------------------------
        Notify M.IP call performance that initial M.IP registration is 
        starting.
      ---------------------------------------------------------------------*/
      mip_perf_delay(&mip_perf_info[PERF_MIP_INIT_REG_DELAY],
                     PERF_MIP_INIT_REG_DELAY,
                     PERF_START_TS); 
    }
#endif /* FEATURE_DS_MOBILE_IP_PERF */
    break;

  default:
    ASSERT(0);
  } /* switch( new state ) */

  msmi_info.state = new_state;

  /*-------------------------------------------------------------------------
    Perform action associated with entering into new state.
  -------------------------------------------------------------------------*/
  switch(new_state)
  {
    /*-----------------------------------------------------------------------
                                 MSMI_CLOSED

      Entering into the Closed state:
      - reset the FA info
      - post the Exit MIP event to all Sol and Reg state machines
      - set the netmodel mode variable back to None
      - Reset the IP address to 0
      - disable notification for interface going down.
      - erase shadow nv.
      - erase the pwd info that is passed by the user through the
        callback function
    -----------------------------------------------------------------------*/
  case MSMI_CLOSED_STATE:
    memset(&ma_info, 0, sizeof(mip_ma_info_type));
    mip_sol_sm_post_event(SSM_EXIT_MIP_EV);
    mip_reg_sm_post_event_all(RSM_EXIT_MIP_EV);
#ifndef FEATURE_DATA_MM
    dsiface_set_ip_addr(IFACE_UM_PPP_STACK, 0);
#endif /* FEATURE_DATA_MM */

    /*-----------------------------------------------------------------------
      Notify DSSNET State machine of M. IP failure.
    -----------------------------------------------------------------------*/
    MSG_MED( "MIP Failure, calling callbacks", 0, 0, 0 );
    mip_call_event_callback(MIP_FAILURE_EV);

    /*-----------------------------------------------------------------------
      Erase the shadow nv items when the call has ended and copy the nv
      items into atcop_val for display.
    -----------------------------------------------------------------------*/
    ds_atcop_erase_shadow_nv();
    ds_atcop_fill_mip_vals(psi_get_nv_item, &ps_nv_item);
    
#ifdef FEATURE_DS_MOBILE_IP_DMU
    /*-----------------------------------------------------------------------
      Erase the passwd info that the user has passed though the callback
      function when the call has ended.
    -----------------------------------------------------------------------*/
    INDEX_2_MIP_SES_PTR(0)->dmu = FALSE;
#endif /* FEATURE_DS_MOBILE_IP_DMU */

    /*-----------------------------------------------------------------------
      Erase the passwd info that the user has passed though the callback
      function when the call has ended.
    -----------------------------------------------------------------------*/
    memset(&mip_pwd_info,0,sizeof(mip_pwd_info));

    break;

    /*-----------------------------------------------------------------------
                                  MSMI_OPEN

      Entering into the Open state:
        make sure that our provisioning information is valid, otherwise post
        event that will cause things to fail.
    -----------------------------------------------------------------------*/
  case MSMI_OPEN_STATE:
    /*-----------------------------------------------------------------------
      Check to see if the session info was retrieved successfully from NV,
      If not, post failure to self. 
    -----------------------------------------------------------------------*/
    if (INDEX_2_MIP_SES_PTR(0)->valid != TRUE)
    {
      MSG_MED("Session info block invalid - closing!\n",0,0,0);
      mip_meta_sm_post_event(MSM_INIT_RRQ_FAILED_EV);
    }
    break;

  default:
    ASSERT(0);
  } /* switch( new state ) */

} /* msmi_transition_state() */

#endif /* regsm_test */


/*===========================================================================
FUNCTION MIP_META_SM_IS_INITIALIZED

DESCRIPTION
  Accessor function to see if MIP subsystem is initialized.

PARAMETERS
  None

RETURN VALUE
  TRUE  - yes, the MIP subsystem is initialized.
  FALSE - no, the MIP subsystem is not initialized.

DEPENDENCIES
  None

SIDE EFFECTS
  None
===========================================================================*/
boolean mip_meta_sm_is_initialized
(
  void
)
{
/*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/
  /*-------------------------------------------------------------------------
    Return initialized boolean from meta state machine internal info struct
  -------------------------------------------------------------------------*/
  return (msmi_info.inited);
} /* mip_meta_sm_is_initialized() */

#endif /* FEATURE_DS_MOBILE_IP */
