/*===========================================================================

                          D S M I P _ R M S M . C

DESCRIPTION

 The Data Services Mobile IP Rm State Machine source file.
 This state machine manages the Um and Rm interfaces for Mobile IP calls.

EXTERNALIZED FUNCTIONS
   mip_rmsm_init()
     Initialize the Registration State Machine.
   mip_rm_post_event()
     Post an event to a Registration State Machine.
   mip_rm_sm_process_event()
     Process the given Rm state machine event.

Copyright (c) 2001 by QUALCOMM, Incorporated.  All Rights Reserved.
===========================================================================*/
/*===========================================================================

                            EDIT HISTORY FOR FILE

  $PVCSPath: O:/src/asw/COMMON/vcs/dsmip_rmsm.c_v   1.15   11 Feb 2003 09:49:56   vramaswa  $
  $Header: //source/qcom/qct/modem/data/1x/mip/main/lite/src/dsmip_rmsm.c#1 $ $DateTime: 2007/11/06 01:07:58 $ $Author: nsivakum $

when        who    what, where, why
--------    ---    ----------------------------------------------------------
07/01/03    jd     After Rm comes up, reconfigure Um with Rm options to 
                   ensure subsequent Um resync does not change framing.
06/30/03    jd     ppp_get_iface_opts is renamed ppp_get_bridge_iface_opts
06/30/03    jd     For MIP calls, set dsnetmdl framing mismatch after resync 
                   behaviour to drop call instead of end-to-end resync.
06/25/03    vr     Removed modification of hdlc_mode in RmSm. It is now done
                   in dssnet only
02/11/03    vr     Removed turning PPP deframing on when Rm interface comes 
                   down and moved it to dssnet.c when Um interface comes down
09/03/02    jd     Turn HW PPP deframing off during mobile IP laptop calls
07/09/02    mvl    Rm now only requires peer to request IP address.
02/20/02    sjy    Mobile IP performance now featurized under 
                   FEATURE_DS_MOBILE_IP_PERF
02/07/02    sjy    Added support for Mobile IP call performance metrics.
11/17/01    aku    Removed incorrect call to ppp_initiate_resync()
11/09/01    aku    Return NO_CARRIER to laptop if mobile is in sockets call
                   when attempting to originate MIP laptop call
09/17/01    js     Added header comment block for externalized functions. 
09/14/01    js     Modified the C-Req callback function to drop the 
                   packet and then to invoke PPP closures. When entering
                   RMSM_RM_NO_MIP_STATE, set the network model meta state
                   machine to the out of call state.  
09/14/01    aku    Added a C-Req callback on the Rm link and it is registered
                   when both Um and Rm are up.
09/14/01    jd     Added argument to ppp_initiate_resync
08/31/01    mvl    Added Term-Req callback, and register it when Um and Rm
                   are up.
08/23/01    mvl    Remove the initialization of the ppp_asy_in() data
                   structures as the were called incorrectly.
08/15/01    sjy    Removed rmsmi_restore_qcmip_mode(), added comments, 
                   code cleanup.
08/03/01    na     When RM_IFACE_DOWN is rxed - NO_CARRIER is sent to DS.
08/03/01    mvl    Fixed to support Um resyncs for mismatched options.
08/02/01    jd     Added some checks in process_event() to avoid strange 
                   behaviour when we get an event unexpectedly in certain 
                   states
08/02/01    na     Put back bug fix for switching over to Simple IP.
08/02/01    mvl    Fixed bug where when receiving the Um Up with M.IP event
                   was not causing a transition to the Waiting for Rm state.
07/31/01    sjy    The RMSM_RM_WANTS_MIP event is now processed only when in 
                   the RMSMI_RM_NO_MIP_STATE.
07/16/01    sjy    Created module
===========================================================================*/


/*===========================================================================

                          INCLUDE FILES FOR MODULE

===========================================================================*/

#include "comdef.h"
#include "customer.h"
#ifdef FEATURE_DS_MOBILE_IP
#include "assert.h"
#include "ds.h"
#include "dsatcop.h"
#include "dsi.h"
#include "dsiface.h"
#include "dsmip_io.h"
#ifdef FEATURE_DS_MOBILE_IP_PERF
#include "dsmip_perf.h"
#endif /* FEATURE_DS_MOBILE_IP_PERF */
#include "dsmip_rmsm.h"
#include "dsnetmdl.h"
#include "ds_snoop.h"
#include "ppp.h"
#include "pppipcp.h"
#include "pppvalue.h"
#include "ps.h"
#include "err.h"
#include "msg.h"


/*= = = = = = = = = = = = = = = = = = = = = = = = = = = = = = = = = = = = = =
                            INTERNAL DEFINITIONS
= = = = = = = = = = = = = = = = = = = = = = = = = = = = = = = = = = = = = =*/

/*---------------------------------------------------------------------------
  These names need to be kept in sync with the enums which they describe.
---------------------------------------------------------------------------*/
char *rmsmi_state_names[RMSM_MAX_STATE] =
{
  "Rm in no M.IP",
  "Waiting for Um to come up",
  "Waiting for Rm to come up",
  "Um/Rm up",
  "Resyncing Um"
};

char *rmsmi_event_names[RMSM_MAX_EV] =
{
  "Rm wants M.IP",
  "Rm interface is down",
  "Rm interface is up",
  "Um interface is down",
  "Um interface is up",
  "Um M.IP is up"
};

/*---------------------------------------------------------------------------
  The control block of the Rm state machine.
---------------------------------------------------------------------------*/
rmsmi_info_type rmsmi_info;

/*===========================================================================
                            Forward Declarations
===========================================================================*/
void rmsmi_transition_state
(
  mip_rmsm_state_type new_state  
);

boolean rmsmi_netmdl_term_req_callback
(
  iface_stack_enum_type iface,         /* the arrival interface            */
  uint16 protocol,                     /* protocol of packet               */
  dsm_item_type **item_head_ptr        /* item containing data             */
);

boolean rmsmi_rm_resync_callback
(
  iface_stack_enum_type iface,         /* the arrival interface            */
  uint16 protocol,                     /* protocol of packet               */
  dsm_item_type **item_head_ptr        /* item containing data             */
);


/*= = = = = = = = = = = = = = = = = = = = = = = = = = = = = = = = = = = = = =
                        EXTERNAL FUNCTION DEFINTIONS
= = = = = = = = = = = = = = = = = = = = = = = = = = = = = = = = = = = = = =*/
/*===========================================================================
  FUNCTION MIP_RM_SM_INIT()

  DESCRIPTION
    This function initializes the state machine.

  PARAMETERS
    None

  RETURN VALUE
    TRUE on success
    FALSE on failure

  DEPENDENCIES
    None

  SIDE EFFECTS
    None
===========================================================================*/
boolean mip_rm_sm_init
(
  void
)
{
  rmsmi_info.state = RMSM_RM_NO_MIP_STATE;
  rmsmi_info.um_always_up = FALSE;
  rmsmi_info.um_mip_changed = FALSE;
  rmsmi_info.inited = TRUE;

  return TRUE;
} /* mip_rm_sm_init() */


/*===========================================================================
  FUNCTION MIP_RM_SM_POST_EVENT()

  DESCRIPTION
    This function receives posted event and calls ps_cmd() for the command
    to be queued and run in the ps task.

  PARAMETERS
    event: The event that is being posted to the state machine.

  RETURN VALUE
    None

  DEPENDENCIES
    mip_rm_sm_init() has to have been called first.

  SIDE EFFECTS
    None
===========================================================================*/
void mip_rm_sm_post_event
(
  mip_rm_sm_event_type event
)
{
  ps_cmd_type cmd_type;              /* command type to be sent to PS task */
/* - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - */
 
#ifdef T_ARM
  MSG_MED("Event %d posted to Rm SM in state %d",
          event,
          rmsmi_info.state, 0);
#else
  MSG_MED("'%s' event posted to Rm SM in state %d.", 
          rmsmi_event_names[event],
          rmsmi_info.state, 0);
#endif
  
  if (rmsmi_info.inited == FALSE)
  {
    /*-----------------------------------------------------------------
      Ignore this event in all other states
    -----------------------------------------------------------------*/
    MSG_MED("Rm SM not inited, ignoring event.", 0, 0, 0);
    return;
  }

  /*-------------------------------------------------------------------------
    Take action based on event received and which state we are currently 
    in.
  -------------------------------------------------------------------------*/  
  switch(event)
  {
    case RMSM_RM_WANTS_MIP:
      cmd_type.cmd_name = PS_RM_WANTS_MIP_CMD;
      break;
  
    case RMSM_RM_IFACE_DOWN:
      cmd_type.cmd_name = PS_RM_IFACE_DOWN_CMD;
      break;
      
    case RMSM_RM_IFACE_UP:
      cmd_type.cmd_name = PS_RM_IFACE_UP_CMD;
      break;            
      
    case RMSM_UM_MIP_DOWN:
      cmd_type.cmd_name = PS_UM_MIP_DOWN_CMD;
      break;
  
    case RMSM_UM_IFACE_UP:
      cmd_type.cmd_name = PS_UM_IFACE_UP_CMD;
      break;
  
    case RMSM_UM_MIP_UP:
      cmd_type.cmd_name = PS_UM_MIP_UP_CMD;
      break;

    default:
      MSG_MED("Got unknown Rm SM event, ignoring", 0, 0, 0);
      return;
  } /* switch(event) */   
  ps_cmd(&cmd_type);

} /* mip_rm_sm_post_event() */


/*===========================================================================
  FUNCTION MIP_RM_SM_PROCESS_EVENT()

  DESCRIPTION
    This function processes the given Rm state machine event.

  PARAMETERS
    event: The event that is being posted to the state machine.

  RETURN VALUE
    None

  DEPENDENCIES
    mip_rm_sm_init() has to have been called first.

  SIDE EFFECTS
    None
===========================================================================*/
void mip_rm_sm_process_event
(
  mip_rm_sm_event_type event
)
{
  ds_cmd_type ds_cmd_buf;            /* command type to be sent to DS task */
  ppp_iface_opts_type iface_opts;    /* ppp options specific to interface  */
  mipio_pppopen_ret_val result;      /* result of mipio_ppp_open()         */
/* - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - */

#ifdef T_ARM  
  MSG_MED("Processing Rm SM event %d in state %d.", 
          event, rmsmi_info.state, 0);
#else
  MSG_MED("Processing Rm SM '%s' event in state %d.", 
          rmsmi_event_names[event], rmsmi_info.state, 0);
#endif

  switch(event)
  {   
    /*-----------------------------------------------------------------------
                               RMSM_RM_WANTS_MIP
  
        Rm requesting M.IP, so bring up Um interface with M.IP enabled.
    -----------------------------------------------------------------------*/
    case RMSM_RM_WANTS_MIP:
      switch(rmsmi_info.state)
      {
        case RMSM_RM_NO_MIP_STATE:
          ppp_cb_array[IFACE_RM_PPP_STACK].qcmip_mode = 0;
          /*-----------------------------------------------------------------
            If QCMIP is 1, enable M.IP on the Um by setting qcmip_mode on 
            the Um to 2.  Currently, Um only supports a qcmip_mode value of 
            0 or 2.
          -----------------------------------------------------------------*/
          if (ds_atcop_qcmip_val == 1)
          {
            rmsmi_info.um_mip_changed = TRUE;
            ppp_cb_array[IFACE_UM_PPP_STACK].qcmip_mode = 2;
          }

          /*-----------------------------------------------------------------
            Bring up M.IP on Um.
          -----------------------------------------------------------------*/
          result = mipio_ppp_open();
          if (result == PPP_OPEN_FAIL)
          {
            ERR("failed to open PPP", 0, 0, 0);
            mip_rm_sm_post_event(RMSM_UM_MIP_DOWN);
          }
          else if(result == PPP_IN_SOCKET_CALL)
          {
            /*---------------------------------------------------------------
              Mobile is in a socket call. Send NO_CARRIER back to the TE
            ---------------------------------------------------------------*/
            ds_cmd_buf.cmd_name = DS_RETURN_NO_CARRIER;
            ds_cmd(&ds_cmd_buf);
            /*---------------------------------------------------------------
              Transition back to RMSM_RM_NO_MIP_STATE so that the qcmip_mode
              value is restored to its original value
            ---------------------------------------------------------------*/
            rmsmi_transition_state(RMSM_RM_NO_MIP_STATE);
          }
          else
          { 
            /*---------------------------------------------------------------
              Tell netmodel to drop call if PPP resync results in framing
              option mismatch rather than do end-to-end resync.
            ---------------------------------------------------------------*/
            netmdl_set_resync_behaviour(NETMDL_RESYNC_DROP_CALL);

            /*---------------------------------------------------------------
              Successful call to mipio_ppp_open()
            ---------------------------------------------------------------*/
            rmsmi_transition_state(RMSM_WAITING_FOR_UM_UP_STATE);
          }
          break;

        default:
          /*-----------------------------------------------------------------
            Ignore this event in all other states
          -----------------------------------------------------------------*/
          MSG_MED("Got unexpected event, ignoring.", 0, 0, 0);
          break;
      } /* switch(rmsmi_info.state) */
      break;
    
    /*-----------------------------------------------------------------------
                               RMSM_RM_IFACE_DOWN
  
        Rm interface is down, so tell Um that Rm is down and close Um if
        provisioned.                                                                       
    -----------------------------------------------------------------------*/
    case RMSM_RM_IFACE_DOWN:
      switch(rmsmi_info.state)
      {
        case RMSM_WAITING_FOR_UM_UP_STATE:
        case RMSM_WAITING_FOR_RM_UP_STATE:
        case RMSM_UM_RM_UP_STATE:
        case RMSM_RESYNCING_UM_STATE:
          /*-----------------------------------------------------------------
            Tell Um that Rm is down, close Um if provisioned.
          -----------------------------------------------------------------*/
          if (!rmsmi_info.um_always_up) 
          {
            mipio_ppp_close();
          }
  
          /*-----------------------------------------------------------------
            Send NO_CARRIER to the DS task.
          -----------------------------------------------------------------*/
          ds_cmd_buf.cmd_name = DS_RETURN_NO_CARRIER;
          ds_cmd(&ds_cmd_buf);
  
          rmsmi_transition_state(RMSM_RM_NO_MIP_STATE);
          break;

        default:
          /*-----------------------------------------------------------------
            Ignore this event in all other states
          -----------------------------------------------------------------*/
          MSG_MED("Got unexpected event, ignoring.", 0, 0, 0);
          break;
      } /* switch(rmsmi_info.state) */
      break;
    
    /*-----------------------------------------------------------------------
                                RMSM_RM_IFACE_UP
  
        Compare Rm and Um options.  If Rm came up with same options as 
        Um, transition to RMSM_UM_RM_UP_STATE, else we need to resync Um 
        with Rm options.   
    -----------------------------------------------------------------------*/
    case RMSM_RM_IFACE_UP:
      switch(rmsmi_info.state)
      {
        case RMSM_WAITING_FOR_RM_UP_STATE:
          /*-----------------------------------------------------------------
            Retrieve the Rm options and modify them for an immediate or 
            future Um resync to ensure framing continuity. 
          -----------------------------------------------------------------*/
          ppp_get_bridge_iface_opts(IFACE_RM_PPP_STACK, &iface_opts);

          /*---------------------------------------------------------------
            Take Rm options, less: 
            - local  1ADDRESS (don't get an IP address for mobile)
            + remote 1ADDRESS (IWF can send IP even if Rm didn't want it)
            + local  PRI_DNS  (ok to get primary DNS address again)
            + local  SEC_DNS  (ok to get secondary DNS address again)
          ---------------------------------------------------------------*/
          iface_opts.ipcp_opt_mask[FSM_LOCAL]  &= ~IPCP_N_1ADDRESS;
          iface_opts.ipcp_opt_mask[FSM_LOCAL]  |=  IPCP_N_PRIMARY_DNS;
          iface_opts.ipcp_opt_mask[FSM_LOCAL]  |=  IPCP_N_SECONDARY_DNS;
          iface_opts.ipcp_opt_mask[FSM_REMOTE] |=  IPCP_N_1ADDRESS;

          /*---------------------------------------------------------------
            Set want values:
            + Primary and Secondary DNS addresses from NV 
            + Primary and Secondary DNS addresses from NV 
          ---------------------------------------------------------------*/
          iface_opts.ipcp_opt_vals[FSM_LOCAL].primary_dns = 
            dsiface_get_dns_addr(IFACE_UM_PPP_STACK, 
                                 IFACE_IPv4_DNS_PRIMARY);

          iface_opts.ipcp_opt_vals[FSM_LOCAL].secondary_dns = 
            dsiface_get_dns_addr(IFACE_UM_PPP_STACK, 
                                 IFACE_IPv4_DNS_SECONDARY);

          /*-----------------------------------------------------------------
            Compare the Rm and Um LCP/IPCP options.
          -----------------------------------------------------------------*/
          if (netmdl_compare_lcp_opts() == TRUE 
              && netmdl_compare_ipcp_opts() == TRUE)  
          {
            /*---------------------------------------------------------------
              Update Um PPP will/want opts & vals to prepare for PPP resync
            ---------------------------------------------------------------*/
            MSG_MED("RM/UM negotiated opts match, reconfig Um", 0, 0, 0);

            /*---------------------------------------------------------------
              Reconfigure the Um interface PPP stack with resync options
            ---------------------------------------------------------------*/
            ppp_reconfig_for_resync(IFACE_UM_PPP_STACK, &iface_opts);

            /*---------------------------------------------------------------
              Rm came up with same options as Um, so go to Um/Rm up state. 
            ---------------------------------------------------------------*/
            rmsmi_transition_state(RMSM_UM_RM_UP_STATE);
          }
          else
          {
            /*---------------------------------------------------------------
              Rm is up with different options from Um, so resync Um 
              immediately to get framing compatible with Rm.
            ---------------------------------------------------------------*/
            MSG_MED("RM/UM negotiated opts differ, resyncing Um", 0, 0, 0);

            /*---------------------------------------------------------------
              Start the Um PPP resync
            ---------------------------------------------------------------*/
            ppp_start(IFACE_UM_PPP_STACK, &iface_opts, TRUE);
  
            /*---------------------------------------------------------------
              Go to resyncing UM state
            ---------------------------------------------------------------*/
            rmsmi_transition_state(RMSM_RESYNCING_UM_STATE);
          }
          break;

        default:
          /*-----------------------------------------------------------------
            Ignore this event in all other states
          -----------------------------------------------------------------*/
          MSG_MED("Got unexpected event, ignoring.", 0, 0, 0);
          break;
      } /* switch(rmsmi_info.state) */
      break;
  
    /*-----------------------------------------------------------------------
                             RMSM_UM_MIP_DOWN 
                             
        Um M.IP interface is down.  If in RMSM_WAITING_FOR_UM_UP_STATE, 
        processing is based on the value of QCMIP: if it is 1, return 
        CONNECT and tell Um to come up with S.IP through relay; if it is 2, 
        we cannot drop to S.IP, so return NO_CARRIER.  
        For all other states, close PPP on Rm.
    -----------------------------------------------------------------------*/
    case RMSM_UM_MIP_DOWN:
      switch(rmsmi_info.state)
      {
        case RMSM_WAITING_FOR_UM_UP_STATE:
          /*-----------------------------------------------------------------
            Checking atcop value here because qcmip_mode might have been 
            temporarily changed to 2.
          -----------------------------------------------------------------*/
          switch (ds_atcop_qcmip_val)
          {
            case 1:
              /*-------------------------------------------------------------
                M.IP preferred, so return CONNECT.  Send cmd to DS task to 
                do this - DCD will be set there.
              -------------------------------------------------------------*/
              ds_cmd_buf.cmd_name = DS_RETURN_CONNECT;
              break;
      
            case 2:
              /*-------------------------------------------------------------
                M.IP only, so return NO CARRIER.
              -------------------------------------------------------------*/ 
              ds_cmd_buf.cmd_name = DS_RETURN_NO_CARRIER;
              break;
      
            default:
              ASSERT(0);
          }
          ds_cmd(&ds_cmd_buf);
          rmsmi_transition_state(RMSM_RM_NO_MIP_STATE);
          break;  
  
        case RMSM_WAITING_FOR_RM_UP_STATE:
        case RMSM_UM_RM_UP_STATE:
        case RMSM_RESYNCING_UM_STATE:
          /*-----------------------------------------------------------------
            Tell Rm to close PPP.
          -----------------------------------------------------------------*/
          ppp_close(IFACE_RM_PPP_STACK);
          rmsmi_transition_state(RMSM_RM_NO_MIP_STATE);
          break;

        default:
          /*-----------------------------------------------------------------
            Ignore this event in all other states
          -----------------------------------------------------------------*/
          MSG_MED("Got unexpected event, ignoring.", 0, 0, 0);
          break;
      } /* switch(rmsmi_info.state) */
      break;
  
    /*-----------------------------------------------------------------------
                                 RMSM_UM_IFACE_UP 
                          
        If Um is up with Rm options, transition to RMSM_UM_RM_UP_STATE.
        Otherwise, Um and Rm options do not agree so close PPP and drop 
        RLP.                         
    -----------------------------------------------------------------------*/
    case RMSM_UM_IFACE_UP:
      switch(rmsmi_info.state)
      {
        case RMSM_RESYNCING_UM_STATE:
          /*-----------------------------------------------------------------
            Compare Um and Rm options    
          -----------------------------------------------------------------*/
          if (netmdl_compare_lcp_opts() == TRUE 
              && netmdl_compare_ipcp_opts() == TRUE)  
          {
            /*---------------------------------------------------------------
              Um is up with the same options as Rm, so transition to Um/Rm
              up state.
            ---------------------------------------------------------------*/
            netmdl_set_mode(IFACE_RM_PPP_STACK, PPP_PSEUDO_NET_MODE);
            netmdl_set_mode(IFACE_UM_PPP_STACK, PPP_PSEUDO_NET_MODE);
            rmsmi_transition_state(RMSM_UM_RM_UP_STATE);
          }
          else
          {
            /*---------------------------------------------------------------
              Um is not up with Rm options, so close PPP and close Um.
            ---------------------------------------------------------------*/
            ppp_close(IFACE_RM_PPP_STACK);
            mipio_ppp_close();       
            rmsmi_transition_state(RMSM_RM_NO_MIP_STATE);         
          }
  
        default:
          /*-----------------------------------------------------------------
            Ignore this event in all other states
          -----------------------------------------------------------------*/
          MSG_MED("Got unexpected event, ignoring.", 0, 0, 0);
          break;
      } /* switch(state) */
      break;

    /*-----------------------------------------------------------------------
                                  RMSM_UM_MIP_UP
    
        Um M.IP is up.  Indicate to Rm to start by returning CONNECT,
        and calling ppp_start().                           
    -----------------------------------------------------------------------*/
    case RMSM_UM_MIP_UP:
      switch(rmsmi_info.state)
      {
        case RMSM_WAITING_FOR_UM_UP_STATE:
          /*-----------------------------------------------------------------
            When Um MIP is up, send DS_RETURN_CONNECT.    
          -----------------------------------------------------------------*/
          ds_cmd_buf.cmd_name = DS_RETURN_CONNECT;
          ds_cmd(&ds_cmd_buf);
        
          /*-----------------------------------------------------------------
            Setup Rm iface_cb to internal mode.
          -----------------------------------------------------------------*/
          netmdl_set_mode(IFACE_RM_PPP_STACK, PPP_INTERNAL_MODE);
      
          /*-----------------------------------------------------------------
            Get configuration for Rm PPP based on Um PPP settings
          -----------------------------------------------------------------*/
          MSG_MED("Starting up the Rm PPP", 0, 0, 0);
          ppp_get_bridge_iface_opts(IFACE_UM_PPP_STACK, &iface_opts);

          /*-----------------------------------------------------------------
            Add the IPCP 1ADDRESS option and IP address obtained from MIP
            registration to the Rm PPP configuration
          -----------------------------------------------------------------*/
          ipcp_set_ip_addr_opt(&iface_opts,
                               dsiface_get_ip_addr(IFACE_UM_PPP_STACK),
                               FSM_REMOTE);

          /*-----------------------------------------------------------------
            Initiate the starting of PPP session on Rm.
          -----------------------------------------------------------------*/
          ppp_start(IFACE_RM_PPP_STACK, &iface_opts, FALSE);
          rmsmi_transition_state (RMSM_WAITING_FOR_RM_UP_STATE);
          break;
      
        default:
          /*-----------------------------------------------------------------
            Ignore this event in all other states
          -----------------------------------------------------------------*/
          MSG_MED("Got unexpected event, ignoring.", 0, 0, 0);
          break;
      }
      break;

    default:
      ASSERT(0);
  } /* switch(event) */
} /* mip_rm_sm_process_event() */


/*= = = = = = = = = = = = = = = = = = = = = = = = = = = = = = = = = = = = = =
                          LOCAL FUNCTION DEFINTIONS
= = = = = = = = = = = = = = = = = = = = = = = = = = = = = = = = = = = = = =*/
/*===========================================================================
  FUNCTION RMSMI_TRANSITION_STATE()

  DESCRIPTION
    This function effects the transition to the state that is passed in.

  PARAMETERS
    new_state: the state to transition to.

  RETURN VALUE
    None

  DEPENDENCIES
    mip_rm_sm_init() has to have been called first.

  SIDE EFFECTS
    None
===========================================================================*/
void rmsmi_transition_state
(
  mip_rmsm_state_type new_state  
)
{
/*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/
#ifdef T_ARM
  MSG_MED("Rm SM from %d to %d state\n", rmsmi_info.state, new_state, 0);
#else
  MSG_MED("Rm SM from '%s' to '%s' state\n",
          rmsmi_state_names[rmsmi_info.state],
          rmsmi_state_names[new_state],
          0);
#endif
  
  rmsmi_info.state = new_state;

  /*-------------------------------------------------------------------------
    Perform action associated with entering into new state.
  -------------------------------------------------------------------------*/
  switch(new_state)
  {
  case RMSM_WAITING_FOR_RM_UP_STATE:
  case RMSM_WAITING_FOR_UM_UP_STATE:
  case RMSM_RESYNCING_UM_STATE:
    break;

  case RMSM_RM_NO_MIP_STATE:
    /*-----------------------------------------------------------------------
      Restore original value of qcmip_mode on Um.
      Set the meta state machine to the out of call state, doing so will
      deregister any registered callback functions.
    -----------------------------------------------------------------------*/
    if (rmsmi_info.um_mip_changed == TRUE)
    {
      ppp_cb_array[IFACE_UM_PPP_STACK].qcmip_mode = ds_atcop_qcmip_val;
      netmdl_set_meta_state (PPP_OOC_META_STATE);
    }
  
    /*-----------------------------------------------------------------------
      Set the resync behaviour back to default setting
    -----------------------------------------------------------------------*/
    netmdl_set_resync_behaviour (NETMDL_RESYNC_DFLT);
    break;

  case RMSM_UM_RM_UP_STATE:
    /*-----------------------------------------------------------------------
      Rm is up with same options as Um: set Um and Rm to pseudo network and
      put the PPP meta state machine in the up state.
    -----------------------------------------------------------------------*/
    netmdl_set_mode(IFACE_RM_PPP_STACK, PPP_PSEUDO_NET_MODE);
    netmdl_set_mode(IFACE_UM_PPP_STACK, PPP_PSEUDO_NET_MODE);
    netmdl_set_meta_state(PPP_UP_META_STATE);

#ifdef FEATURE_DS_MOBILE_IP_PERF
    if (qw_cmp(mip_perf_info[PERF_TOTAL_CALL_SETUP_DELAY].ts_start, 
               ts_null) != 0)       
    { 
      /*---------------------------------------------------------------------
        Notify M.IP call performance that M.IP call setup is complete for
        network model calls.
      ---------------------------------------------------------------------*/
      mip_perf_delay(&mip_perf_info[PERF_TOTAL_CALL_SETUP_DELAY],
                     PERF_TOTAL_CALL_SETUP_DELAY,
                     PERF_END_TS);
    }
#endif /* FEATURE_DS_MOBILE_IP_PERF */

    /*-----------------------------------------------------------------------
      Register theTerm-Req callback that causes the interface it is called on
      to get torn down for both interfaces and both LCP and IPCP.
      NOTE: this MUST be done AFTER the meta state has been set to UP - this
      guarantees that these callbacks are not over written by setting the
      meta state.
    -----------------------------------------------------------------------*/
    (void)snoop_reg_event(IFACE_RM_PPP_STACK,
                          PPP_LCP_PROTOCOL,
                          TERM_REQ,
                          rmsmi_netmdl_term_req_callback);
    (void)snoop_reg_event(IFACE_UM_PPP_STACK,
                          PPP_LCP_PROTOCOL,
                          TERM_REQ,
                          rmsmi_netmdl_term_req_callback);
    (void)snoop_reg_event(IFACE_RM_PPP_STACK,
                          PPP_IPCP_PROTOCOL,
                          TERM_REQ,
                          rmsmi_netmdl_term_req_callback);
    (void)snoop_reg_event(IFACE_UM_PPP_STACK,
                          PPP_IPCP_PROTOCOL,
                          TERM_REQ,
                          rmsmi_netmdl_term_req_callback);

    /*-----------------------------------------------------------------------
      Register the rmsmi_rm_resync_callback. This would ensure that the
      call is released on detecting a PPP resync on the Rm link
    -----------------------------------------------------------------------*/
    (void)snoop_reg_event(IFACE_RM_PPP_STACK,
                          PPP_LCP_PROTOCOL,
                          CONFIG_REQ,
                          rmsmi_rm_resync_callback);
    (void)snoop_reg_event(IFACE_RM_PPP_STACK,
                          PPP_IPCP_PROTOCOL,
                          CONFIG_REQ,
                          rmsmi_rm_resync_callback);
    break;

  default:
    ASSERT(0);
  } /* switch(new state) */
} /* rmsmi_transition_state() */



/*===========================================================================
FUNCTION   RMSM_NETMDL_TERM_REQ_CALLBACK()

DESCRIPTION
  This is the Term-Req callback that is used to tear down whichever link the
  Term-Req is received on.  It places the recieving interface into internal
  mode.

DEPENDENCIES
  This function expects that the first four bytes of the PPP packet are in
  the first DSM item. For this information to be useful it is expected that
  the FF03 has been removed from the front and that any leading 00 s have
  been removed from the protocol ID.

RETURN VALUE
  FALSE: This implies that the calling function need not process any thing
         more.

SIDE EFFECTS
  This will cause the receiving link to be torn down.
===========================================================================*/
boolean rmsmi_netmdl_term_req_callback
(
  iface_stack_enum_type iface,         /* the arrival interface            */
  uint16 protocol,                     /* protocol of packet               */
  dsm_item_type **item_head_ptr        /* item containing data             */
)
{
  /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  netmdl_set_mode(iface, PPP_INTERNAL_MODE);

  return FALSE;

} /* rmsmi_netmdl_term_req_callback() */


/*===========================================================================
FUNCTION   RMSMI_RM_RESYNC_CALLBACK()

DESCRIPTION
  This callback function is called when an LCP/IPCP C-Req is received on the
  Rm link i.e. when a PPP resync is detected on the Rm link once the call
  has been established. Drop the packet in such event.  

DEPENDENCIES
  This callback should be registered only after PPP is up on both Um and
  Rm links

RETURN VALUE
  Always returns FALSE: this implies that the calling function need not
                        to process anything more.  

SIDE EFFECTS
  None
===========================================================================*/
boolean rmsmi_rm_resync_callback
(
  iface_stack_enum_type iface,         /* the arrival interface            */
  uint16 protocol,                     /* protocol of packet               */
  dsm_item_type **item_head_ptr        /* item containing data             */
)
{
  /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/
  /*lint -e527 -esym(613, item_head_ptr) */
  ASSERT(item_head_ptr != NULL && *item_head_ptr != NULL);
  ASSERT(iface == IFACE_RM_PPP_STACK);
  ASSERT((protocol == PPP_IPCP_PROTOCOL) || (protocol == PPP_LCP_PROTOCOL));
  /*lint +e527 */

  MSG_HIGH ("Rm PPP resync!  drop call",0,0,0);

  /*-------------------------------------------------------------------------
    Drop the dsm_item
  -------------------------------------------------------------------------*/
  dsm_free_packet(item_head_ptr);

  /*-------------------------------------------------------------------------
    Close PPP on RM. 
  -------------------------------------------------------------------------*/
  ppp_close(iface);

  return FALSE;

} /* rmsmi_rm_resync_callback() */

#endif /* FEATURE_DS_MOBILE_IP */
