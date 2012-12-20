/*===========================================================================

        D A T A  S E R V I C E S  S O C K E T  N E T W O R K   F I L E
                   
DESCRIPTION

 The Data Services sockets network state machine implementation. This 
 contains all the functions used by the sockets library to access the 
 network and communicate with the DS task. (This is an INTERNAL sockets
 file).
 
EXTERNALIZED FUNCTIONS
 
  dssnet_sm_post_event()
    Post events to the DSSNET state machine
  dssnet_sm_init()
    Initialize the DSSNET State machine

Copyright (c) 1998-2011 by QUALCOMM, Incorporated.  All Rights Reserved.
===========================================================================*/

/*===========================================================================

                      EDIT HISTORY FOR FILE

  $PVCSPath: L:/src/asw/MM_DATA/vcs/ps707_dssnet.c_v   1.25   28 Feb 2003 10:27:32   sramacha  $
  $Header: //source/qcom/qct/modem/data/1x/707/main/lite/src/ps707_dssnet.c#15 $ $DateTime: 2011/03/09 07:43:48 $ $Author: nsivakum $

  This section contains comments describing changes made to the module.
  Notice that changes are listed in reverse chronological order.


when        who    what, where, why
--------    ---    ----------------------------------------------------------
02/25/11    ms     Ported MOBILE_IP_DEREG feature.
04/29/09    sn     Ported support for call throttle feature (DCTM).
12/01/08    psng   Fixed compilation error.
11/25/08    psng   Fixed compilation error.
11/24/08    psng   Added support to use legacy NV items if number of valid SIP
                   profiles is zero.
11/20/08    psng   Fixed multiple profile CHAP authentication failure when
                   MS is in RUIM mode.
11/19/08    sn     Fixed compiler warning.
11/14/08    psng   Made Changes to use legacy credentials for MIP and
                   MIP to SIP fallback when OMH is enabled.
07/11/08    sn     Changed FEATURE_DS_SIP_MULTIPLE_PROFILE to 
                   FEATURE_DS_MULTIPLE_PROFILES.
06/18/08    sn     Fixed compiler warnings.
03/13/08    psng   Added authentication support for Multiple sip profile 
11/01/06    an     Featurize ATCOP, RMSM, Async Fax
02/24/06    ssh    Added support for default SN PPP password.
                   (FEATURE_DATA_PPP_DEFAULT_PASSWD)
04/27/04    kvd    Resync PPP (and don't restart PPP) if PPP is already 
                   established when falling back from MIP to SIP.
02/21/04    aku    Tear down call if PPP comes up with MIP when expecting to 
                   come up with SIP instead.
11/07/03    jd     Trash RLP tx queue when PPP resync occurs to avoid sending
                   IP traffic on link before NCP is configured.
11/07/03    aku    Added abort callback to handle call aborts for sockets and
                   all MIP calls
09/10/03    aku    Removed the assert if mobile gets phys link timer expiry
                   event occurs when phys link is already up as this can 
                   happen during a rare race condition.
09/10/03    aku    Allocate ps_iface event buffers at powerup.
08/25/03    aku    Reset work_mip and will_mip values if QCMIP value changes.
06/18/03    aku    Changed MSG_ERROR() to ERR() in process_up_with_mip_ev()
06/03/03    aku    Changed NV_PAP_USER_ID_I to NV_PPP_USER_ID_I.
05/30/03    aku    Backed out change to gracefully terminate MIP calls (done
                   in ds707_pkt_mgr instead).
05/21/03    vas    Added support for HDR to initialize RLP when orig sockets
                   call
05/14/03    aku    Added support to gracefully terminate MIP calls when 
                   dormant (made it backwards compatible with previous 
                   versions).
05/13/03    usb    Handling return value from ppp_start(), moved MIP policy
                   flags from PPP cb to dssnet cb.  
05/05/03    jd     Removed dssnet_get_iface_status, dssnet_set_close_reason
                   Removed dependency on dsiface_def.h
05/03/03    usb    Setting DNS values in 1x iface to default at call 
                   setup and PPP resync (handoff)
03/26/03    mvl    mainlined the CHAP and PAP features.
03/25/03    aku    Ignore (remove assert()) PPP_CLOSE_CMD in CLOSED_STATE.
03/11/03    aku    Tear down iface when dssnet processes a network initiated 
                   PPP/MIP failure.
02/21/03    jd     moved & renamed ppp_auth_get_from_nv() from ps_ppp_auth.c
                   to is707_get_ppp_auth_info_from_nv() in this file
02/12/03    aku    Ignore phy_iface_timer_ev in CLOSED_STATE and transition 
                   to waiting_for_phy_iface_down state on receivng 
                   phy_iface_down_ev in waiting_for_phy_iface_up state.
02/10/03    aku    Deregister PPP events when transitioning to CLOSED state.
02/06/03    jd     renamed ppp_auth_get_info() to ppp_auth_get_from_nv()
01/31/03    jd     moved dssnet_get_auth_from_nv to ps_ppp_auth.c and 
                   renamed to ppp_auth_get_info
01/27/03    jd     Fix app flow control during PPP resync 
12/22/02    aku    Fixed typo in checking for UP_WTIH_SIP state in 
                   dssneti_set_netstate().
12/13/02    jd     Added flow control after PPP resync until MIP registered
11/25/02    aku    Set the close reason to INVALID_PPP_CLOSE always.
11/19/02    jd     Fix MIP perf to calc Um PPP delay only, 
                   moved te2_override_qcmip into dssneti_sm_cb
11/14/02    jd/usb Added MIP support
                   Added qcmip override from RMSM, replaces old "mip_mode"
                   Determine MIP/SIP support after PPP by IPCP IPaddr opt
                   rather than looking at interface bound IP address
                   Allow negotiation of peer (IWF) IP address in MIP 
11/14/02    mvl    Added JCDMA support.
10/10/02    aku    Added support to go routeable first for MIP calls. 
09/18/02    aku    Fixed bug when retrieving IP address from ps_iface.
09/18/02    mvl    Added user_data to ppp event callbacks.  Removed do_mip. 
09/11/02    aku    Added support for MIP.                                   
09/06/02    aku    Fixed bug in registering for ps_iface events and added   
                   checks for ps_iface functions return codes.              
09/05/02    aku    Updated for ps_iface cback buf interface.                
09/03/02    aku    Removed support for DS_PAP_NV feature and 
                   DS_CHAP_NV feature and wrapped file under 
                   FEATURE_DATA_IS707
08/13/02    mvl    Added session handle support for PPP callback.
08/05/02    aku    Added support for getting/setting PPP auth info.
08/02/02    aku    Added interface to notify dssnet that it is a QNC call.
07/30/02    aku    Added support to return dormancy status in 
                   dssnet_get_iface_status(). Also check to see return value 
                   when ps_iface_phys_link_down_cmd() is called.
07/29/02    aku    Enable flow when transitioning to CLOSED_STATE and 
                   register for phys events when transitioning out of CLOSED
                   state and deregister when we transition back to CLOSEd
                   state.
07/23/02    aku    Execute ps_iface commands when transitioning to 
                   CLOSED_STATE only if ps_iface_ptr is not NULL.
                   Set the local ps_iface_ptr when processing
                   DSSNET_PHY_LINK_DOWN_PPP_ABORT_CMD_EV if not already set. 
07/22/02    aku    Register for phys iface event only after issuing command
                   to ps_iface.
                   Added support for DSSNET_PHY_LINK_DOWN_PPP_ABORT_CMD_EV
07/15/02    aku    Removed references to dssdorm
07/14/02    aku    Initial version for multimode PS.
===========================================================================*/

/*===========================================================================

                      INCLUDE FILES FOR MODULE

===========================================================================*/
#include "comdef.h"
#include "target.h"
#include "customer.h"

#ifdef FEATURE_DATA_IS707

#ifndef FEATURE_DATA
#include "dstask.h"
#ifndef FEATURE_DATA_STRIP_ATCOP
#include "dsatcop.h"
#include "dsat.h"
#endif
#include "dsmgr.h"
#else
#include "dstask.h"
#ifndef FEATURE_DATA_STRIP_ATCOP
#include "dsat.h"
#endif
#endif /* FEATURE_DATA */
#include "ps_iface.h"
#include "ps707_dssnet.h"
#include "assert.h"
#include "msg.h"

#include "ps_ppp_fsm.h"

#ifdef FEATURE_DS_MOBILE_IP
#include "dsmip_metasm.h"

#ifndef FEATURE_DATA_STRIP_ATCOP
#include "dsat707vendctab.h"
#include "dsat707mipctab.h"
#endif

#include "ds707_p_rev.h"
#include "ds707_drs.h"
#ifdef FEATURE_DS_MOBILE_IP_PERF
#include "dsmip_perf.h"
#endif /* FEATURE_DS_MOBILE_IP_PERF */

#endif /* FEATURE_DS_MOBILE_IP */

#include "pstimer.h"
#include "dssinternal.h"
#include "ps_svc.h"
#include "ds707_pkt_mgr.h"
#include "ds707_wmk.h"
#include "ps_ppp.h"
#include "dstask.h"
#include "ps_svc.h"
#include "nv.h"
#include "ps_utils.h"
#include "ps_ppp_auth.h"

#ifdef FEATURE_DATA_STRIP_ATCOP
#include "dsatprofile.h"
#endif

#ifdef FEATURE_HDR
#error code not present
#endif /* FEATURE_HDR */

#ifdef FEATURE_CALL_THROTTLE
#include "dsdctm.h"
#endif /* FEATURE_CALL_THROTTLE */

#ifdef FEATURE_DS_MULTIPLE_PROFILES
#include "ds707_data_session_profile.h"
#endif /* FEATURE_DS_MULTIPLE_PROFILES */
/*===========================================================================

            LOCAL DEFINITIONS AND DECLARATIONS FOR MODULE

This section contains local definitions for constants, macros, types,
variables and other items needed by this module.

===========================================================================*/

/*---------------------------------------------------------------------------
  30s for traffic channel to be established/torn down
---------------------------------------------------------------------------*/
#define DSSNETI_PHY_IFACE_TIME 30000

#ifdef FEATURE_DATA_PPP_DEFAULT_PASSWD
/*---------------------------------------------------------------------------
  Definitions for default SN PPP password
  Note: The length of the password, PPP_DEFAULT_SN_PASSWORD_LEN is expected
  to not exceed:
  (1) The maximum size of the NV item to hold it, NV_MAX_PPP_PASSWORD_LENGTH
      (127 bytes); and 
  (2) The size of PPP structure to hold it, PPP_MAX_PASSWD_LEN (127 bytes)
  So exercise care in changing this definition so that it fits.
---------------------------------------------------------------------------*/
#define PPP_DEFAULT_SN_PASSWORD       "0000000000000000"       /* 16 bytes */
#define PPP_DEFAULT_SN_PASSWORD_LEN   (sizeof( PPP_DEFAULT_SN_PASSWORD ) - 1)
#endif /* FEATURE_DATA_PPP_DEFAULT_PASSWD */

/*---------------------------------------------------------------------------
 Type and static variable defining the state of the network.
---------------------------------------------------------------------------*/
typedef enum
{
  DSSNETI_CLOSED_STATE = 0,
  DSSNETI_WAITING_FOR_PHY_IFACE_UP_STATE = 1,
#ifdef FEATURE_DS_MOBILE_IP
  DSSNETI_WAITING_FOR_LINK_LAYER_WITH_MIP_STATE = 2,
#endif /* FEATURE_DS_MOBILE_IP */
  DSSNETI_WAITING_FOR_LINK_LAYER_WITH_SIP_STATE = 3,
#ifdef FEATURE_DS_MOBILE_IP
  DSSNETI_WAITING_FOR_MIP_REG_STATE = 4,
  DSSNETI_NET_IFACE_UP_WITH_MIP_STATE = 5,
#endif /* FEATURE_DS_MOBILE_IP */
  DSSNETI_NET_IFACE_UP_WITH_SIP_STATE = 6,
  DSSNETI_CLOSING_STATE = 7,
  DSSNETI_WAITING_FOR_PHY_IFACE_DOWN_STATE = 8,
  DSSNETI_PPP_RESYNC_STATE = 9,
#ifdef FEATURE_DS_MOBILE_IP
#ifdef FEATURE_DS_MOBILE_IP_DEREG
  DSSNETI_WAITING_FOR_MIP_DEREG_STATE  = 10
#endif /* FEATURE_DS_MOBILE_IP_DEREG */
#endif /* FEATURE_DS_MOBILE_IP */
} dssneti_netstate_enum_type;

/*---------------------------------------------------------------------------
  DSSNET SM control block.
---------------------------------------------------------------------------*/
typedef struct 
{
  dssneti_netstate_enum_type netstate;              /* Current state       */
  ps_iface_type              *ps_iface_ptr;         /* associated ps_iface */
  void                       *phys_iface_up_cback;    /* phys up event buf */
  void                       *phys_iface_down_cback;  /* phys down ev buf  */
  boolean                    disable_lcp_auth;        /* PPP auth needed?  */
  boolean                    te2_call_override_qcmip; /* no sip fallback   */
  boolean                    is_te2_client;       /* set for laptop client */
  uint8                      phy_iface_timer; /* timer for traffic channel */
#ifdef FEATURE_DS_MOBILE_IP
  boolean                    want_mip;        /* try mobile ip first?      */
  boolean                    will_sip;        /* can fallback to simple ip */
  boolean                    work_mip;        /* is mobile ip established? */
#endif /* FEATURE_DS_MOBILE_IP */
  void                       *mip_event_handle;/* MIP event handle         */
} dssneti_sm_cb_type;

LOCAL dssneti_sm_cb_type  dssneti_sm_cb;

/*---------------------------------------------------------------------------
  Boolean to track if state machine has been initialized.
---------------------------------------------------------------------------*/
LOCAL boolean dssneti_sm_initialized = FALSE;

#ifdef FEATURE_CALL_THROTTLE
LOCAL ds_mip_ppp_status mip_ppp_status;
#endif /* FEATURE_CALL_THROTTLE */

#ifdef FEATURE_DS_MOBILE_IP
/*-------------------------------------------------------------------------
  MIP event registration flag
-------------------------------------------------------------------------*/
LOCAL boolean dssneti_mip_event_registered = FALSE;
#endif /* FEATURE_DS_MOBILE_IP */

/*===========================================================================

                    FORWARD  DECLARATIONS FOR MODULE

===========================================================================*/
LOCAL void dssneti_set_netstate
(
  dssneti_netstate_enum_type new_netstate
);

LOCAL void phy_iface_timer_cb
(
  void *
);

LOCAL void dssnet_process_ppp_open_cmd_ev
(
  ps_cmd_enum_type       cmd,                      /* PS command type      */
  void                  *data_ptr                  /* Pointer to user data */
);

LOCAL void dssnet_process_phy_iface_up_ev
(
  ps_cmd_enum_type       cmd,                      /* PS command type      */
  void                  *data_ptr                  /* Pointer to user data */
);

LOCAL void dssnet_process_link_layer_up_with_mip_ev
(
  ps_cmd_enum_type       cmd,                      /* PS command type      */
  void                  *data_ptr                  /* Pointer to user data */
);

LOCAL void dssnet_process_link_layer_up_with_sip_ev
(
  ps_cmd_enum_type       cmd,                      /* PS command type      */
  void                  *data_ptr                  /* Pointer to user data */
);

LOCAL void dssnet_process_mip_up_success_ev
(
  ps_cmd_enum_type       cmd,                      /* PS command type      */
  void                  *data_ptr                  /* Pointer to user data */
);

LOCAL void dssnet_process_mip_failure_ev
(
  ps_cmd_enum_type       cmd,                      /* PS command type      */
  void                  *data_ptr                  /* Pointer to user data */
);

LOCAL void dssnet_process_net_iface_down_ev
(
  ps_cmd_enum_type       cmd,                      /* PS command type      */
  void                  *data_ptr                  /* Pointer to user data */
);

LOCAL void dssnet_process_ppp_close_cmd_ev
(
  ps_cmd_enum_type       cmd,                      /* PS command type      */
  void                  *data_ptr                  /* Pointer to user data */
);

LOCAL void dssnet_process_phy_iface_timer_ev
(
  ps_cmd_enum_type       cmd,                      /* PS command type      */
  void                  *data_ptr                  /* Pointer to user data */
);

LOCAL void dssnet_process_phy_iface_down_ev
(
  ps_cmd_enum_type       cmd,                      /* PS command type      */
  void                  *data_ptr                  /* Pointer to user data */
);

LOCAL void dssnet_process_ppp_resync_ev
(
  ps_cmd_enum_type       cmd,                      /* PS command type      */
  void                  *data_ptr                  /* Pointer to user data */
);

LOCAL void dssnet_process_phy_link_down_ppp_abort_cmd_ev
(
  ps_cmd_enum_type       cmd,                      /* PS command type      */
  void                  *data_ptr                  /* Pointer to user data */
);

#ifdef FEATURE_DS_MOBILE_IP
static void dssneti_handle_mip_ev_cback
(
  mip_event_e_type       mip_event,                /* Sucees/Fail          */
  mip_fail_reason_e_type fail_reason,
  void                   *user_data
);

#ifdef FEATURE_DS_MOBILE_IP_DEREG
static void dssneti_process_mip_dereged_ev
(
  ps_cmd_enum_type       cmd,                      /* PS command type      */
 void                   *data_ptr                  /* Pointer to user data */
);
#endif /* FEATURE_DS_MOBILE_IP_DEREG */
#endif /* FEATURE_DS_MOBILE_IP */


/*===========================================================================

                      LOCAL FUNCTION DEFINITIONS

===========================================================================*/
/*===========================================================================
FUNCTION      DSSNETI_PPP_ABORT_CBACK()

DESCRIPTION   Called when the PPP needs to be aborted.  This posts a PPP 
              abort event to the dssnet state machine.

DEPENDENCIES  None

RETURN VALUE  None.

SIDE EFFECTS  None.
===========================================================================*/
void dssneti_ppp_abort_cback
(
  void
)
{
/*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/
  dssnet_sm_post_event(DSSNET_PHY_LINK_DOWN_PPP_ABORT_CMD_EV);
} 


/*===========================================================================
FUNCTION      DSSNETI_HANDLE_PHYS_IFACE_EV_CBACK()

DESCRIPTION   Called when the PHYS IFACE goes up/down.  This posts an 
              appropriate event to the dssnet state machine.

DEPENDENCIES  None

RETURN VALUE  None.

SIDE EFFECTS  None.
===========================================================================*/
void dssneti_handle_phys_iface_ev_cback
(
  ps_iface_type                *iface_ptr,
  ps_iface_event_enum_type      event,
  ps_iface_event_info_u_type    event_info,
  void                         *user_info_ptr
)
{
/*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  MSG_MED("Recd %d ps_iface event in state %d",
           event,dssneti_sm_cb.netstate,0);

  switch(event)
  {
    case IFACE_PHYS_LINK_UP_EV: 
      dssnet_sm_post_event(DSSNET_PHY_IFACE_UP_EV);
      break;

    case IFACE_PHYS_LINK_DOWN_EV: 
      dssnet_sm_post_event(DSSNET_PHY_IFACE_DOWN_EV);
      break;

    case IFACE_ENABLED_EV: 
    case IFACE_DISABLED_EV: 
    case IFACE_DOWN_EV: 
    case IFACE_ROUTEABLE_EV: 
    case IFACE_UP_EV: 
    case IFACE_FLOW_ENABLED_EV: 
    case IFACE_FLOW_DISABLED_EV: 
    case IFACE_ADDR_CHANGED_EV: 
    case IFACE_DELETE_EV: 
    default:
      MSG_ERROR("Unsupported PHYS event %d",event,0,0);
      ASSERT(0);
  }
}

/*===========================================================================
FUNCTION      DSSNETI_HANDLE_PPP_EV_CBACK()

DESCRIPTION   Called when the PPP comes up/down/resync.  This posts an
              appropriate event to the dssnet state machine.

DEPENDENCIES  None

RETURN VALUE  None.

SIDE EFFECTS  None.
===========================================================================*/
void dssneti_handle_ppp_ev_cback
(
  ppp_dev_enum_type       ppp_dev,
  ppp_protocol_e_type     protocol,              /* PPP Protocol           */
  ppp_event_e_type        ppp_event,             /* Down/Up/Resync         */
  void                    *user_data,
  uint32                  session_handle,
  ppp_fail_reason_e_type  ppp_fail_reason
)
{
#ifdef FEATURE_DS_MOBILE_IP
  ppp_dev_opts_type  dev_opts;
#endif /* FEATURE_DS_MOBILE_IP */

/*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  MSG_MED("Recd %d ppp event",ppp_event,0,0);

  ASSERT(ppp_dev == PPP_UM_SN_DEV);
  switch(ppp_event)
  {
    case PPP_UP_EV:
#ifdef FEATURE_DS_MOBILE_IP
      /*---------------------------------------------------------------------
        Determine if PPP has come up with SIP/MIP. If the interface has not
        been assigned an IP address, then PPP has come up MIP, else with SIP. 
      ---------------------------------------------------------------------*/
      (void) ppp_get_protocol_opts (ppp_dev, &dev_opts);

      /*---------------------------------------------------------------------
        get_protocol copies the current WORK values in to the WANT field 
        (in anticipation of starting another PPP), so we example the WANT 
        field to see what was just negotiated.
      ---------------------------------------------------------------------*/
      if (!(dev_opts.ipcp_info.want_mask[FSM_LOCAL] & IPCP_N_1ADDRESS))
      {
        dssnet_sm_post_event(DSSNET_LINK_LAYER_UP_WITH_MIP_EV);
      }
      else
#endif /* FEATURE_DS_MOBILE_IP */
      {
        dssnet_sm_post_event(DSSNET_LINK_LAYER_UP_WITH_SIP_EV);
      }
      break;

    case PPP_DOWN_EV:
#ifdef FEATURE_CALL_THROTTLE
      mip_ppp_status.ppp_fail_reason = ppp_fail_reason;
      mip_ppp_status.ppp_protocol = protocol;
#ifdef FEATURE_DS_MOBILE_IP
      mip_ppp_status.mip_fail_reason = MIP_FAIL_REASON_NONE;
#endif  /* FEATURE_DS_MOBILE_IP */

      MSG_MED("PPP fail reason %d, Protocol %d", ppp_fail_reason, protocol, 0);
#endif /* FEATURE_CALL_THROTTLE */

      dssnet_sm_post_event(DSSNET_NET_IFACE_DOWN_EV);
      break;

    case PPP_RESYNC_EV:
      MSG_MED ("Um resync, trash queued outgoing data",0,0,0);
      ds707_pkt_trash_rlp_tx_q( dssneti_sm_cb.ps_iface_ptr );
      dssnet_sm_post_event(DSSNET_PPP_RESYNC_EV);
      break;

    default:
      MSG_ERROR("Unsupported PPP event %d",ppp_event,0,0);
      ASSERT(0);
  }
} /* dssneti_handle_ppp_ev_cback() */

/*===========================================================================
FUNCTION      DSSNETI_HANDLE_PPP_AUTH_EV_CBACK()

DESCRIPTION   Called when the PPP auth SUCCESS/FAILURE.

DEPENDENCIES  None

RETURN VALUE  None

SIDE EFFECTS  None
===========================================================================*/
void dssneti_handle_ppp_auth_ev_cback
(
  ppp_dev_enum_type       ppp_dev,
  ppp_protocol_e_type     protocol,              /* PPP Protocol           */
  ppp_event_e_type        ppp_event,             /* Down/Up/Resync         */
  void                    *user_data,
  uint32                  session_handle,
  ppp_fail_reason_e_type  ppp_fail_reason
)
{
/*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/
  MSG_MED( "Recd ppp dev %d event %d for dssnet4 0x%x", 
           ppp_dev, ppp_event, user_data );

  MSG_MED( "Protocol %d Fail reason %d", protocol, ppp_fail_reason, 0 );

  ASSERT(ppp_dev == PPP_UM_SN_DEV);
  switch(ppp_event)
  {
    case PPP_FAILURE_EV:
      MSG_HIGH("PPP Auth Failure", 0, 0, 0);

#ifdef FEATURE_CALL_THROTTLE
      if((dssneti_sm_cb.is_te2_client == FALSE) ||
         ((dssneti_sm_cb.is_te2_client == TRUE) && (dsat707_qcmip_val == 2)))
      {
        dctm_post_ppp_call_status( PPP_FAILURE_EV, PPP_PROTOCOL_AUTH, 
                                   PPP_FAIL_REASON_AUTH_FAILURE );
      }
#endif /* FEATURE_CALL_THROTTLE */

      dssneti_set_netstate (DSSNETI_CLOSING_STATE);
      break;
    case PPP_SUCCESS_EV:
      MSG_HIGH("PPP Auth success", 0, 0, 0);
      break;
    default:
      MSG_ERROR("Unknown PPP event on UM for protocol: %x",protocol, 0, 0);
      ASSERT(0);
      break;
  }
} /* dssneti_handle_ppp_auth_ev_cback() */

/*===========================================================================
FUNCTION DSSNETI_PHY_IFACE_TIMER_CB()

DESCRIPTION
  This function is the callback for the expiration of the physical layer
  timer.  It posts the PHY_IFACE_TIMER_EXP event to the state machine.

PARAMETERS
  void *

RETURN VALUE
  None

DEPENDENCIES
  None

SIDE EFFECTS
  None
===========================================================================*/
LOCAL void dssneti_phy_iface_timer_cb
(
  void *param
)
{
  /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  dssnet_sm_post_event(DSSNETI_PHY_IFACE_TIMER_EXP_EV);

} /* dssneti_phy_iface_timer_cb() */

/*===========================================================================
FUNCTION DSSNETI_SET_NETSTATE()

DESCRIPTION
  This function is called whenever the netstate changes.
  The variable is set and housekeeping done for each netstate.
  
  Any change to netstate should be through this function only.
  
  This function can be called from the context of the PS task only.

DEPENDENCIES
  Will be executed in the context of the PS task only

RETURN VALUE
  None.

SIDE EFFECTS
  Changes the global netstate.

===========================================================================*/
LOCAL void dssneti_set_netstate
(
  dssneti_netstate_enum_type new_netstate        /* new state to change to */
)
{
  ps_timer_error_type         timer_result;   /* Result of timer calls     */
  ppp_dev_opts_type           ppp_config;           
  int16                       ps_errno;
/*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  /*-------------------------------------------------------------------------
    Changing the state variable.
    This function should always execute in the PS task context, so dont need
    INTLOCKS 
  -------------------------------------------------------------------------*/
  MSG_MED("Transition net state %d to %d", 
           dssneti_sm_cb.netstate, 
           new_netstate,
           0);  
  dssneti_sm_cb.netstate = new_netstate;

  /*-------------------------------------------------------------------------
    Executing state based functionality.
  -------------------------------------------------------------------------*/
  switch (dssneti_sm_cb.netstate)
  {
    case DSSNETI_CLOSED_STATE:
      dssneti_sm_cb.is_te2_client           = FALSE;
#ifdef FEATURE_DS_MOBILE_IP
      /*---------------------------------------------------------------------
        Clear TE2 override of qcmip val
      ---------------------------------------------------------------------*/
      dssneti_sm_cb.te2_call_override_qcmip = FALSE;

      /*---------------------------------------------------------------------
        Cease flow control of sockets and TE2 pending MIP registration
      ---------------------------------------------------------------------*/
      mip_meta_sm_set_in_flow (DS_FLOW_DSSNET_REG_MASK, TRUE);

#endif /* FEATURE_DS_MOBILE_IP */

      /*---------------------------------------------------------------------
        Cancel phy_iface_timer.
      ---------------------------------------------------------------------*/
      timer_result =
            ps_timer_cancel((uint8) dssneti_sm_cb.phy_iface_timer);
      ASSERT(timer_result != PS_TIMER_FAILURE);

      MSG_MED("PPP is down",0,0,0);

      /*---------------------------------------------------------------------
        Set lcp_auth_disabled to FALSE.
      ---------------------------------------------------------------------*/
      dssneti_sm_cb.disable_lcp_auth = FALSE;

      /*---------------------------------------------------------------------
        Set the abort function cback in 707 Packet Manager to NULL
      ---------------------------------------------------------------------*/
      MSG_MED("dssnet setting abort cback to NULL",0,0,0);
      ds707_pkt_set_abort_f_ptr(DS707_DEF_PKT_INSTANCE,
                                NULL);

#ifdef FEATURE_CALL_THROTTLE
      /*---------------------------------------------------------------------
        Set fail reasons to NONE
      ---------------------------------------------------------------------*/
      mip_ppp_status.ppp_fail_reason = PPP_FAIL_REASON_NONE;
      mip_ppp_status.ppp_protocol    = PPP_PROTOCOL_MIN;
#ifdef FEATURE_DS_MOBILE_IP
      mip_ppp_status.mip_fail_reason = MIP_FAIL_REASON_NONE;
#endif  /* FEATURE_DS_MOBILE_IP */
#endif /* FEATURE_CALL_THROTTLE */

      /*---------------------------------------------------------------------
        Check to see if ps_iface_ptr is not set to NULL. 
      ---------------------------------------------------------------------*/
      if(dssneti_sm_cb.ps_iface_ptr != NULL)
      {
        /*-------------------------------------------------------------------
          Enable flow to apps (Sockets is done using the iface. Enable flow 
          so that laptop calls can be setup now).
        -------------------------------------------------------------------*/
        ps_iface_enable_flow(dssneti_sm_cb.ps_iface_ptr, 
                              DS_FLOW_CTRL_DSSNET_MASK);

        /*-------------------------------------------------------------------
          Deregister event callbacks.
        -------------------------------------------------------------------*/
        ps_iface_event_cback_dereg(dssneti_sm_cb.ps_iface_ptr,
                                   IFACE_PHYS_LINK_UP_EV,
                                   dssneti_sm_cb.phys_iface_up_cback);

        ps_iface_event_cback_dereg(dssneti_sm_cb.ps_iface_ptr,
                                   IFACE_PHYS_LINK_DOWN_EV,
                                   dssneti_sm_cb.phys_iface_down_cback);

        /*-------------------------------------------------------------------
          Deregister PPP events.  
        -------------------------------------------------------------------*/
        (void) ppp_reg_event(PPP_UM_SN_DEV,
                             PPP_UP_EV,
                             NULL,
                             NULL);
        (void) ppp_reg_event(PPP_UM_SN_DEV,
                             PPP_DOWN_EV,
                             NULL,
                             NULL);
        (void) ppp_reg_event(PPP_UM_SN_DEV,
                             PPP_RESYNC_EV,
                             NULL,
                             NULL);
        (void) ppp_reg_event(PPP_UM_SN_DEV,
                             PPP_FAILURE_EV,
                             NULL,
                             NULL);

        /*-------------------------------------------------------------------
          Send a ps_iface_down_ind. 
        -------------------------------------------------------------------*/
        ps_iface_down_ind(dssneti_sm_cb.ps_iface_ptr);

        /*-------------------------------------------------------------------
          Set ps_iface_ptr to NULL.
        -------------------------------------------------------------------*/
        dssneti_sm_cb.ps_iface_ptr = NULL;

      }
      break;

    case DSSNETI_WAITING_FOR_PHY_IFACE_UP_STATE:
#ifdef FEATURE_HDR
#error code not present
#endif /* FEATURE_HDR */
        
      /*---------------------------------------------------------------------
        Set the abort function cback in 707 Packet Manager.
      ---------------------------------------------------------------------*/
      MSG_MED("dssnet setting abort_cback",0,0,0);
      ds707_pkt_set_abort_f_ptr(DS707_DEF_PKT_INSTANCE,
                                dssneti_ppp_abort_cback);

      /*---------------------------------------------------------------------
        Bring up the physical link.

        HANDLE RETURN VALUE HERE
      ---------------------------------------------------------------------*/
      ps_iface_phys_link_up_cmd(dssneti_sm_cb.ps_iface_ptr, 
                                &ps_errno, 
                                NULL);

      /*---------------------------------------------------------------------
        Register for PHYS_LINK_UP and PHYS_LINK_DOWN events.
      ---------------------------------------------------------------------*/
      if(ps_iface_event_cback_reg(dssneti_sm_cb.ps_iface_ptr,
                                  IFACE_PHYS_LINK_UP_EV,
                                  dssneti_sm_cb.phys_iface_up_cback) != 0)
      {
        MSG_ERROR("Could not reg IFACE_UP evt",0,0,0);
        ASSERT(0);
      }
    
      if(ps_iface_event_cback_reg(dssneti_sm_cb.ps_iface_ptr,
                                  IFACE_PHYS_LINK_DOWN_EV,
                                  dssneti_sm_cb.phys_iface_down_cback) != 0)
      {
        MSG_ERROR("Could not reg IFACE_DOWN evt",0,0,0);
        ASSERT(0);
      }

      /*---------------------------------------------------------------------
        Disable flow to apps
      ---------------------------------------------------------------------*/
      ps_iface_disable_flow(dssneti_sm_cb.ps_iface_ptr,
                            DS_FLOW_CTRL_DSSNET_MASK);


      /*---------------------------------------------------------------------
        Start the timer for traffic channel establishment.
      ---------------------------------------------------------------------*/
      timer_result = ps_timer_start(dssneti_sm_cb.phy_iface_timer,
                                    DSSNETI_PHY_IFACE_TIME);
      ASSERT(timer_result != PS_TIMER_FAILURE);
      break;

    case DSSNETI_WAITING_FOR_LINK_LAYER_WITH_SIP_STATE:
#ifdef FEATURE_DS_MOBILE_IP
    case DSSNETI_WAITING_FOR_LINK_LAYER_WITH_MIP_STATE:
#endif /* FEATURE_DS_MOBILE_IP */
      /*---------------------------------------------------------------------
        Disable flow to apps
      ---------------------------------------------------------------------*/
      ps_iface_disable_flow(dssneti_sm_cb.ps_iface_ptr, 
                            DS_FLOW_CTRL_DSSNET_MASK);

     /*----------------------------------------------------------------------
       Setup PPP configuration options. 
      ---------------------------------------------------------------------*/
      ppp_get_default_opts(&ppp_config);
      ppp_config.lcp_info.mode = PPP_CP_ACTIVE;
      if(dssneti_sm_cb.disable_lcp_auth == TRUE)
      {
        /*-------------------------------------------------------------------
          Disable PPP auth. 
        -------------------------------------------------------------------*/
        ppp_config.lcp_info.will_mask[FSM_REMOTE] &= ~LCP_N_AP;
        ppp_config.lcp_info.want_mask[FSM_REMOTE] &= ~LCP_N_AP;
        ppp_config.lcp_info.will_mask[FSM_LOCAL] &= ~LCP_N_AP;
        ppp_config.lcp_info.want_mask[FSM_LOCAL] &= ~LCP_N_AP;
      }
      ppp_config.ipcp_info.mode = PPP_CP_ACTIVE;
      ppp_config.hdlc_mode = HDLC_HW_PREF;
      ppp_config.rx_iface_ptr = dssneti_sm_cb.ps_iface_ptr;
#ifdef FEATURE_JCDMA_1X
      ppp_config.setup_timeout = JCDMA_MAX_PPP_SETUP_TIME;
#endif /* FEATURE_JCDMA_1X */
      (void) ppp_reg_event(PPP_UM_SN_DEV,
                           PPP_UP_EV,
                           dssneti_handle_ppp_ev_cback,
                           NULL);
      (void) ppp_reg_event(PPP_UM_SN_DEV,
                           PPP_DOWN_EV,
                           dssneti_handle_ppp_ev_cback,
                           NULL);
      (void) ppp_reg_event(PPP_UM_SN_DEV,
                           PPP_RESYNC_EV,
                           dssneti_handle_ppp_ev_cback,
                           NULL);
      (void) ppp_reg_event(PPP_UM_SN_DEV,
                           PPP_FAILURE_EV,
                           dssneti_handle_ppp_auth_ev_cback,
                           NULL);

      ppp_config.rx_f_ptr = ds707_pkt_rx_um_SN_data;
      ppp_config.tx_f_ptr = ds707_pkt_tx_um_SN_data;

#ifdef FEATURE_DS_MOBILE_IP
      if(dssneti_sm_cb.netstate == 
                    DSSNETI_WAITING_FOR_LINK_LAYER_WITH_MIP_STATE)
      {
        /*-------------------------------------------------------------------
          Disable PPP auth.
        -------------------------------------------------------------------*/
        ppp_config.lcp_info.will_mask[FSM_REMOTE] &= ~LCP_N_AP;
        ppp_config.lcp_info.want_mask[FSM_REMOTE] &= ~LCP_N_AP;
        ppp_config.lcp_info.will_mask[FSM_LOCAL] &= ~LCP_N_AP;
        ppp_config.lcp_info.want_mask[FSM_LOCAL] &= ~LCP_N_AP;
        /*-------------------------------------------------------------------
          Setup IPCP to not negotiate for IP address.
        -------------------------------------------------------------------*/
        ppp_config.ipcp_info.will_mask[FSM_LOCAL] &= ~IPCP_N_1ADDRESS;
        ppp_config.ipcp_info.want_mask[FSM_LOCAL] &= ~IPCP_N_1ADDRESS;

        /*-------------------------------------------------------------------
          If doing MIP, use RM as the bridge device. 
          This will never be used if we are in a sockets call, so it is
          safe to set it here for both MT2 and MT0 calls.
        -------------------------------------------------------------------*/
        ppp_config.bridge_dev = PPP_RM_DEV;
      }
#endif /* FEATURE_DS_MOBILE_IP */

      /*---------------------------------------------------------------------
        Set default DNS values in iface. PPP will update these with the 
        network assigned values if DNS gets negotiated.
      ---------------------------------------------------------------------*/      
      dssneti_sm_cb.ps_iface_ptr->v4_net_info.primary_dns = 
        dsat_qcdns_val[0];
      dssneti_sm_cb.ps_iface_ptr->v4_net_info.secondary_dns = 
        dsat_qcdns_val[1];

      /*---------------------------------------------------------------------
        Configure the PPP auth info for Um and start ppp on the device
      ---------------------------------------------------------------------*/
      is707_get_ppp_auth_info_from_nv( &ppp_config, ps_get_nv_item );
      if(ppp_start(PPP_UM_SN_DEV, &ppp_config) < 0)
      {
        /*-------------------------------------------------------------------
          PPP configuration failed.  Post the appropriate event to cleanup
          call since PPP will not give any other indications. We don't want
          to fallback to SIP in this case, set the will_sip to false.
        -------------------------------------------------------------------*/
#ifdef FEATURE_DS_MOBILE_IP
        dssneti_sm_cb.will_sip = FALSE;
#endif /* FEATURE_DS_MOBILE_IP */

        dssnet_sm_post_event(DSSNET_NET_IFACE_DOWN_EV);
      }
      break;

#ifdef FEATURE_DS_MOBILE_IP
    case DSSNETI_WAITING_FOR_MIP_REG_STATE:
      /*---------------------------------------------------------------------
        Set iface to routeable.  This would prevent non-system sockets from
        being able to use the interface till MIP registration is complete. 
        MIP will be able to use the interface to register as it uses system
        sockets. 
      ---------------------------------------------------------------------*/
      ps_iface_routeable_ind(dssneti_sm_cb.ps_iface_ptr);

     /*---------------------------------------------------------------------
        Flow control non-system sockets and TE2 pending MIP registration
 
        This must be done before enabling the interface flow mask below
        to ensure that no data passes until MIP registration is complete.
      ---------------------------------------------------------------------*/
      mip_meta_sm_set_in_flow (DS_FLOW_DSSNET_REG_MASK, FALSE);

      /*---------------------------------------------------------------------
        Enable flow to apps
      ---------------------------------------------------------------------*/
      ps_iface_enable_flow(dssneti_sm_cb.ps_iface_ptr, 
                            DS_FLOW_CTRL_DSSNET_MASK);

      /*---------------------------------------------------------------------
        Notify MIP meta_sm that iface is up with MIP 
      ---------------------------------------------------------------------*/
      MSG_MED("Iface up, Waiting on MIP registration",0,0,0); 
      mip_meta_sm_post_event(MSM_IFACE_UP_W_MIP_EV);
      break;

    case DSSNETI_NET_IFACE_UP_WITH_MIP_STATE:
#endif /* FEATURE_DS_MOBILE_IP */
    case DSSNETI_NET_IFACE_UP_WITH_SIP_STATE:
      /*---------------------------------------------------------------------
        Send iface up indication.
      ---------------------------------------------------------------------*/
      ps_iface_up_ind(dssneti_sm_cb.ps_iface_ptr);

      /*---------------------------------------------------------------------
        Enable flow to apps
      ---------------------------------------------------------------------*/
      ps_iface_enable_flow(dssneti_sm_cb.ps_iface_ptr, 
                            DS_FLOW_CTRL_DSSNET_MASK);

#ifdef FEATURE_DS_MOBILE_IP
     /*---------------------------------------------------------------------
        Cease flow control of sockets and TE2 pending MIP registration
     ---------------------------------------------------------------------*/
      mip_meta_sm_set_in_flow (DS_FLOW_DSSNET_REG_MASK, TRUE);

      if(dssneti_sm_cb.netstate == DSSNETI_NET_IFACE_UP_WITH_SIP_STATE)
      {
        /*-------------------------------------------------------------------
          Notify MIP meta_sm that iface is up with SIP 
        -------------------------------------------------------------------*/  
        MSG_MED("Iface up with SimpleIP ",0,0,0); 
        mip_meta_sm_post_event(MSM_IFACE_UP_WO_MIP_EV);
      }
      else
      {
        MSG_MED("Iface up with MIP",0,0,0); 
      }
#endif /* FEATURE_DS_MOBILE_IP */
      break;

    case DSSNETI_CLOSING_STATE:
      /*---------------------------------------------------------------------
        Close PPP.
      ---------------------------------------------------------------------*/
      MSG_MED("Closing PPP on Iface" ,0,0,0);
      ppp_stop(PPP_UM_SN_DEV);

      /*---------------------------------------------------------------------
        Disable flow to apps
      ---------------------------------------------------------------------*/
      ps_iface_disable_flow(dssneti_sm_cb.ps_iface_ptr, 
                            DS_FLOW_CTRL_DSSNET_MASK);
      break;

    case DSSNETI_WAITING_FOR_PHY_IFACE_DOWN_STATE:
      MSG_HIGH("Bring down phy iface" ,0 ,0, 0);
      /*---------------------------------------------------------------------
        Send command to bring down the traffic channel.
      ---------------------------------------------------------------------*/
      if( ps_iface_phys_link_down_cmd(dssneti_sm_cb.ps_iface_ptr,
                                      &ps_errno,
                                      NULL) == 0)
      {
        /*-------------------------------------------------------------------
          PPP could have just aborted the physical interface. So the phys
          interface could already be down. In this case transition to 
          the closed state right away.
        -------------------------------------------------------------------*/
        MSG_MED("Phys link is already down",0,0,0);
        dssneti_set_netstate(DSSNETI_CLOSED_STATE);
        break;
      }

      /*---------------------------------------------------------------------
        TODO : Start the timer for traffic channel tear down.
      ---------------------------------------------------------------------*/
      timer_result = ps_timer_start(dssneti_sm_cb.phy_iface_timer,
                                    DSSNETI_PHY_IFACE_TIME);
      ASSERT(timer_result != PS_TIMER_FAILURE);

      /*---------------------------------------------------------------------
        Disable flow to apps
      ---------------------------------------------------------------------*/
      ps_iface_disable_flow(dssneti_sm_cb.ps_iface_ptr, 
                            DS_FLOW_CTRL_DSSNET_MASK);
      break;
       
    case DSSNETI_PPP_RESYNC_STATE:

#ifdef FEATURE_DS_MOBILE_IP
     /*---------------------------------------------------------------------
       Flow control of sockets and TE2 pending MIP registration
     ---------------------------------------------------------------------*/
      mip_meta_sm_set_in_flow (DS_FLOW_DSSNET_REG_MASK, FALSE);
#endif /* FEATURE_DS_MOBILE_IP */

      /*---------------------------------------------------------------------
        Since we are handing off to new PDSN, reset the DNS values to 
        default, PPP will update these again if network assigns new values.
      ---------------------------------------------------------------------*/
      dssneti_sm_cb.ps_iface_ptr->v4_net_info.primary_dns = 
        dsat_qcdns_val[0];
      dssneti_sm_cb.ps_iface_ptr->v4_net_info.secondary_dns = 
        dsat_qcdns_val[1];

      /*---------------------------------------------------------------------
        Disable flow to apps
      ---------------------------------------------------------------------*/
      ps_iface_disable_flow(dssneti_sm_cb.ps_iface_ptr, 
                            DS_FLOW_CTRL_DSSNET_MASK);
      break;

#ifdef FEATURE_DS_MOBILE_IP
#ifdef FEATURE_DS_MOBILE_IP_DEREG
   case DSSNETI_WAITING_FOR_MIP_DEREG_STATE:
      /* send bring down mip event to meta sm*/
      mip_meta_sm_post_event( MSM_BRING_DOWN_MIP_EV );
      break;
#endif /* FEATURE_DS_MOBILE_IP_DEREG */
#endif /* FEATURE_DS_MOBILE_IP */

    default:
      /*---------------------------------------------------------------------
        Sanity Check
      ---------------------------------------------------------------------*/
      /*lint -e506, -e774 */
      ASSERT(0);
      /*lint +e506, +e774 */
  } /* switch on netstate */

} /* dssneti_set_netstate() */


#ifdef FEATURE_DS_MOBILE_IP
/*===========================================================================
FUNCTION DSSNETI_REG_MIP_EVENT()

DESCRIPTION
  This function is used to register MIP events.

DEPENDENCIES
  None.

RETURN VALUE
  None.

SIDE EFFECTS
  None.
===========================================================================*/
void dssneti_reg_mip_event
(
  void
)
{
  /*-------------------------------------------------------------------------
    Register for MIP SUCCESS / FAIL events.
  -------------------------------------------------------------------------*/
  if( !dssneti_mip_event_registered )
  {
    dssneti_sm_cb.mip_event_handle = mip_event_alloc
                                     ( 
                                       dssneti_handle_mip_ev_cback,
                                       NULL 
                                     );
    if( NULL == dssneti_sm_cb.mip_event_handle )
    {
      ERR_FATAL( "Unable to acquire a MIP event handle", 0, 0, 0 );
    }
    else
    {
      (void) mip_event_register( dssneti_sm_cb.mip_event_handle,
                                 MIP_FAILURE_EV_MASK
                               ); 
      (void) mip_event_register( dssneti_sm_cb.mip_event_handle,
                                 MIP_SUCCESS_EV_MASK
                               );
    #ifdef FEATURE_DS_MOBILE_IP_DEREG
      (void) mip_event_register( dssneti_sm_cb.mip_event_handle,
                                 MIP_DEREGED_EV_MASK
                               );                           
    #endif /* FEATURE_DS_MOBILE_IP_DEREG */
    }

    dssneti_mip_event_registered = TRUE;
  }
}

/*===========================================================================
FUNCTION MOBILE_IP_SUPPORT

DESCRIPTION
  Check if mobile can establish mobile IP call. 

  To do MIP in an IS-835 network,
    - p_rev must be greater than or equal to 6, 
    - qcmdr must be equal to 3,
    - MIP meta state machine must be initialized

DEPENDENCIES
  None.

RETURN VALUE
  TRUE  - mobile IP is supported
  FALSE - mobile IP is not supported

SIDE EFFECTS
  None
===========================================================================*/
LOCAL boolean mobile_ip_support()
{
  word   p_rev_index;          /* holds current sys's p_rev index */
  /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  /*-------------------------------------------------------------------------
    Retrieve the currently value of the P_REV being used
  -------------------------------------------------------------------------*/
  p_rev_index = ds_socfg_get_p_rev_index();

  /*-------------------------------------------------------------------------
    Determine whether we can do mip.  
 
  -------------------------------------------------------------------------*/
  if( (p_rev_index >= DS_BS_PREV_6) &&
      (ds_atcop_qcmdr_val == DS_MDR_MODE_SO33_PREF) &&
      (mip_meta_sm_is_initialized() == TRUE)  // can't call directly if in DS task
    )
  {
    MSG_MED("MIP supported. PREV = %d, QCMDR=%d",
            p_rev_index, dsat707_qcmdr_val, 0);
    return TRUE;
  }
  else
  {
    MSG_HIGH("MIP NOT supported. PREV = %d, QCMDR=%d",
              p_rev_index, dsat707_qcmdr_val, 0);
    return FALSE;
  }

} /* mobile_ip_support() */
#endif /* FEATURE_DS_MOBILE_IP */


/*===========================================================================
FUNCTION DSSNET_OVERRIDE_QCMIP()

DESCRIPTION
  Override dsat707_qcmip value with 2.

  RMSM calls this to force dssnet to close in the qcmip=1 case, 
  so it can bring up netmodel SimpleIP TE2 calls on its own.

  RMSM calls this to force_sip_fallback_to_fail_so_rmsm_can_do_it()

DEPENDENCIES
  None.

RETURN VALUE
  None.

SIDE EFFECTS
  None.
===========================================================================*/
void dssnet_override_qcmip
( 
  void
)
{
  dssneti_sm_cb.te2_call_override_qcmip = TRUE;
}

/*===========================================================================
FUNCTION DSSNET_SET_TE_CLIENT()

DESCRIPTION
  This is a flag set by RMSM to allow dssnet to check if the
  call is a laptop call or a sockets call.

DEPENDENCIES
  None.

RETURN VALUE
  None.

SIDE EFFECTS
  None.
===========================================================================*/
void dssnet_set_te_client
(
  void
)
{
/*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/
  dssneti_sm_cb.is_te2_client = TRUE;

  MSG_MED("Setting te2 client to TRUE", 0, 0, 0 ); // For debugging - Remove later.
} /* dssnet_set_te_client() */

/*===========================================================================
FUNCTION DSSNET_PROCESS_PPP_OPEN_CMD_EV()

DESCRIPTION
  This function processes the DSSNET_PPP_OPEN_CMD_EV event. Changes State to 
  DSSNETI_WAITING_FOR_PHY_IFACE_UP_STATE

DEPENDENCIES
  None.

RETURN VALUE
  None.

SIDE EFFECTS
  None.
===========================================================================*/
LOCAL void dssnet_process_ppp_open_cmd_ev
(
  ps_cmd_enum_type       cmd,                      /* PS command type      */
  void                  *data_ptr                  /* Pointer to user data */
)
{
#ifdef FEATURE_DS_MOBILE_IP
  uint32 qcmip;                 /* effective qcmip value to use for this call */
#endif /* FEATURE_DS_MOBILE_IP */
/*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/
  
  MSG_MED("PPP_OPEN_CMD_EV recd in %d state",
          dssneti_sm_cb.netstate,0,0);

  switch(dssneti_sm_cb.netstate)
  {
    case DSSNETI_CLOSED_STATE: 

      /*---------------------------------------------------------------------
        Retrieve the ps_iface_ptr from the 1x controller.
      ---------------------------------------------------------------------*/
      dssneti_sm_cb.ps_iface_ptr = 
        ds707_pkt_get_iface_ptr(DS707_DEF_PKT_INSTANCE);
      ASSERT(dssneti_sm_cb.ps_iface_ptr != NULL);

#ifdef FEATURE_DS_MOBILE_IP
      /*---------------------------------------------------------------------
        Extra logic for SIP fallback
      ---------------------------------------------------------------------*/
      if (dssneti_sm_cb.te2_call_override_qcmip)
      {
        qcmip = 2; 
      }
      else
      {
        qcmip = dsat707_qcmip_val;  
      }

      /*---------------------------------------------------------------------
        Set Um PPP device MIP settings according to QCMIP value
        QCMIP = 0  -> SIP only
        QCMIP = 1  -> MIP wanted, SIP if MIP fails for some reason
        QCMIP = 2  -> MIP or nothing
      ---------------------------------------------------------------------*/
      dssneti_sm_cb.will_sip = (qcmip == 2) ? FALSE : TRUE;

      if (qcmip > 0)
      {
        dssneti_sm_cb.want_mip = TRUE;

        /*-------------------------------------------------------------------
          If the network doesn't support MIP and the mobile is configured 
          to do PPP with mobile IP only, do not continue.
        -------------------------------------------------------------------*/
        if (!mobile_ip_support() && !dssneti_sm_cb.will_sip)
        {
          MSG_HIGH("Can't try SIP, ending call",0,0,0);
          goto _STOP_NOW;
        }

        /*-------------------------------------------------------------------
          If mobile is configured to attempt mobile IP, 
          configure the MIP session settings before starting PPP.
        -------------------------------------------------------------------*/
        if (mip_config_session() == FALSE) // can't call this directly if dssnet moves to DS
        {
          MSG_ERROR("MIP session configuration failed",0,0,0);

          /*  If SimpleIP is not an option, abort now */
          if (!dssneti_sm_cb.will_sip)
          {
            goto _STOP_NOW;
          }

          /* fallback to SimpleIP */
          MSG_MED("Trying SimpleIP",0,0,0);
          dssneti_sm_cb.work_mip = FALSE;
        }
        else
        {
          /* Attempt MIP once physical link is up */
          dssneti_sm_cb.work_mip = TRUE; /* this will be set to FALSE by dssnet if mip orig fails */
        }
      }
      else
      {
        dssneti_sm_cb.work_mip = FALSE;
        dssneti_sm_cb.want_mip = FALSE;
      }
#endif /* FEATURE_DS_MOBILE_IP */

      /*---------------------------------------------------------------------
        MIP or Simple IP is acceptable to both the mobile and network,
        so continue (wait for physical link to come up).
      ---------------------------------------------------------------------*/
      dssneti_set_netstate(DSSNETI_WAITING_FOR_PHY_IFACE_UP_STATE);
      break;

#ifdef FEATURE_DS_MOBILE_IP 
_STOP_NOW:
    {
      /*---------------------------------------------------------------------
        Transition to DSSNETI_CLOSED_STATE. 

        This will perform all processing required to gracefully close the 
        sockets subsystem, like notifying apps and resetting the 
        ppp_active flag.
      ---------------------------------------------------------------------*/
      dssneti_set_netstate(DSSNETI_CLOSED_STATE);
      break;
    }
#endif /* FEATURE_DS_MOBILE_IP */

    case DSSNETI_NET_IFACE_UP_WITH_SIP_STATE: 
#ifdef FEATURE_DS_MOBILE_IP
    case DSSNETI_NET_IFACE_UP_WITH_MIP_STATE: 
#endif /* FEATURE_DS_MOBILE_IP */
      /*-------------------------------------------------------------
        (Notify application?) Note that network is already open.
      -------------------------------------------------------------*/
      MSG_MED("PPP is already up.", 0,0,0);
      break;

    case DSSNETI_WAITING_FOR_PHY_IFACE_UP_STATE:
    case DSSNETI_WAITING_FOR_LINK_LAYER_WITH_SIP_STATE: 
    case DSSNETI_CLOSING_STATE: 
    case DSSNETI_WAITING_FOR_PHY_IFACE_DOWN_STATE:
    case DSSNETI_PPP_RESYNC_STATE:
#ifdef FEATURE_DS_MOBILE_IP
    case DSSNETI_WAITING_FOR_LINK_LAYER_WITH_MIP_STATE: 
    case DSSNETI_WAITING_FOR_MIP_REG_STATE: 
#endif /* FEATURE_DS_MOBILE_IP */
      break;

    default:
      /*---------------------------------------------------------------------
        Sanity Check
      ---------------------------------------------------------------------*/
      /*lint -e506, -e774 */
      ASSERT(0);
      /*lint +e506, +e774 */
  } /* end switch */

} /* dssnet_process_ppp_open_cmd_ev() */

/*===========================================================================
FUNCTION DSSNET_PROCESS_PHY_IFACE_UP_EV()

DESCRIPTION
  This function processes the DSSNET_PHY_IFACE_UP_EV event. 

  State will be changed to 
  DSSNETI_WAITING_FOR_LINK_LAYER_WITH_MIP_STATE or 
  DSSNETI_WAITING_FOR_LINK_LAYER_WITH_SIP_STATE
  based on the PPP device's configuration for MIP and 
  whether MIP is supported by the network.

DEPENDENCIES
  None.

RETURN VALUE
  None.

SIDE EFFECTS
  None.
===========================================================================*/
LOCAL void dssnet_process_phy_iface_up_ev
(
  ps_cmd_enum_type       cmd,                      /* PS command type      */
  void                  *data_ptr                  /* Pointer to user data */
)
{
  ps_timer_error_type timer_result;
/*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/
  MSG_MED("PHY_IFACE_UP_EV recd in %d state",
          dssneti_sm_cb.netstate,0,0);

  switch(dssneti_sm_cb.netstate)
  {
    case DSSNETI_WAITING_FOR_PHY_IFACE_UP_STATE:
      /*---------------------------------------------------------------------
       Traffic channel is establised. Cancel the timer
      ---------------------------------------------------------------------*/
      timer_result = ps_timer_cancel(dssneti_sm_cb.phy_iface_timer);
      ASSERT(timer_result != PS_TIMER_FAILURE);

      /*---------------------------------------------------------------------
        Change the state based on the mip mode setting of the device. 
        If the network doesn't support MIP or device is set to try SimpleIP,
        change state to DSSNETI_WAITING_FOR_LINK_LAYER_WITH_SIP_STATE. 
        If the network supports MIP and the device is configured to try MIP,
        change state to DSSNETI_WAITING_FOR_LINK_LAYER_WITH_MIP_STATE. 
      ---------------------------------------------------------------------*/
#ifdef FEATURE_DS_MOBILE_IP
      MSG_MED ("MIP: support %d, want %d, try %d", 
               mobile_ip_support(),
               dssneti_sm_cb.want_mip,
               dssneti_sm_cb.work_mip);

      if( dssneti_sm_cb.work_mip && mobile_ip_support() )
      {
        MSG_MED ("Waiting for link up to do MIP",0,0,0);
        dssneti_set_netstate( DSSNETI_WAITING_FOR_LINK_LAYER_WITH_MIP_STATE);
      }
      else if (dssneti_sm_cb.will_sip)
      {
        dssneti_sm_cb.work_mip = FALSE; /* in case no more mip support */
        MSG_MED ("Waiting for link up to do SimpleIP",0,0,0);
        dssneti_set_netstate( DSSNETI_WAITING_FOR_LINK_LAYER_WITH_SIP_STATE);
      }
      else 
      {
        /*---------------------------------------------------------------
          If SimpleIP is not an option, abort now
        ---------------------------------------------------------------*/
        MSG_MED( "Can't try SIP, ending call",0,0,0);
        dssneti_set_netstate(DSSNETI_CLOSED_STATE);
        break;
      }
#else 
      {
        MSG_MED ("Waiting for link up to do SimpleIP",0,0,0);
        dssneti_set_netstate( DSSNETI_WAITING_FOR_LINK_LAYER_WITH_SIP_STATE);
      }
#endif /* FEATURE_DS_MOBILE_IP */
      break;

    case DSSNETI_CLOSED_STATE:
    case DSSNETI_WAITING_FOR_LINK_LAYER_WITH_SIP_STATE: 
    case DSSNETI_NET_IFACE_UP_WITH_SIP_STATE: 
    case DSSNETI_CLOSING_STATE: 
    case DSSNETI_WAITING_FOR_PHY_IFACE_DOWN_STATE:
    case DSSNETI_PPP_RESYNC_STATE:
#ifdef FEATURE_DS_MOBILE_IP
    case DSSNETI_WAITING_FOR_LINK_LAYER_WITH_MIP_STATE: 
    case DSSNETI_WAITING_FOR_MIP_REG_STATE: 
    case DSSNETI_NET_IFACE_UP_WITH_MIP_STATE: 
#ifdef FEATURE_DS_MOBILE_IP_DEREG
    case DSSNETI_WAITING_FOR_MIP_DEREG_STATE:
#endif /* FEATURE_DS_MOBILE_IP_DEREG */
#endif /* FEATURE_DS_MOBILE_IP */
      /*---------------------------------------------------------------------
        Ignore event.
      ---------------------------------------------------------------------*/
      MSG_MED("Ignoring PHY_IFACE_UP_EV in %d state",
               dssneti_sm_cb.netstate,0,0);
      break;

    default:
      /*---------------------------------------------------------------------
        Sanity Check
      ---------------------------------------------------------------------*/
      /*lint -e506, -e774 */
      ASSERT(0);
      /*lint +e506, +e774 */
  } /* switch */

} /* dssnet_process_phy_iface_up_ev() */

/*===========================================================================
FUNCTION DSSNET_PROCESS_LINK_LAYER_UP_WITH_MIP_EV()

DESCRIPTION
  This function processes the DSSNET_LINK_LAYER_UP_WITH_MIP_EV. Changes state 
  to DSSNETI_WAITING_FOR_MIP_REG_STATE.

DEPENDENCIES
  None.

RETURN VALUE
  None.

SIDE EFFECTS
  None.
===========================================================================*/
LOCAL void dssnet_process_link_layer_up_with_mip_ev
(
  ps_cmd_enum_type       cmd,                      /* PS command type      */
  void                  *data_ptr                  /* Pointer to user data */
)
{
  int16  tear_down_errno;
/*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  MSG_MED("LINK_LAYER_UP_WITH_MIP_EV rx'd in %d state.", 
          dssneti_sm_cb.netstate,0,0);

  switch(dssneti_sm_cb.netstate)
  {
    case DSSNETI_CLOSED_STATE:
    case DSSNETI_WAITING_FOR_PHY_IFACE_UP_STATE:
    case DSSNETI_WAITING_FOR_PHY_IFACE_DOWN_STATE:
    case DSSNETI_CLOSING_STATE:
      /*---------------------------------------------------------------------
        Ignore event.     
      ---------------------------------------------------------------------*/
      MSG_MED("Ignoring LINK_LAYER_UP_WITH_MIP_EV in %d state.",
               dssneti_sm_cb.netstate,0,0);
      break;

    case DSSNETI_PPP_RESYNC_STATE:
      MSG_MED("PPP resync is done",0,0,0);
#ifdef FEATURE_DS_MOBILE_IP
    case DSSNETI_WAITING_FOR_LINK_LAYER_WITH_MIP_STATE:
      /*---------------------------------------------------------------------
        Transition to DSSNETI_WAITING_FOR_MIP_REG_STATE state
      ---------------------------------------------------------------------*/
      MSG_MED("Transition to WAITING_FOR_MIP_REG state",0,0,0);

      dssneti_reg_mip_event();
      dssneti_set_netstate(DSSNETI_WAITING_FOR_MIP_REG_STATE);
#endif /* FEATURE_DS_MOBILE_IP */
      break;

    case DSSNETI_WAITING_FOR_LINK_LAYER_WITH_SIP_STATE:
      /*---------------------------------------------------------------------
        This should not happen in the normal case. But a malforming PPP 
        implementation on the PDSN side could result in the mobile not being
        assigned an IP address when trying to establish a SIP session. We 
        handle this case by tearing down the call.
      ---------------------------------------------------------------------*/
      MSG_ERROR("Unexpected UP_WITH_MIP_EV in %d state. Tearing down call",  
                dssneti_sm_cb.netstate,0,0);

#ifdef FEATURE_CALL_THROTTLE
      dctm_post_ppp_call_status( PPP_FAILURE_EV, PPP_PROTOCOL_IPCP, 
                                 PPP_FAIL_REASON_NONE );
#endif /* FEATURE_CALL_THROTTLE */

      ps_iface_tear_down_cmd(dssneti_sm_cb.ps_iface_ptr,
                             &tear_down_errno,
                             NULL);
      break;

    case DSSNETI_NET_IFACE_UP_WITH_SIP_STATE:
#ifdef FEATURE_DS_MOBILE_IP
    case DSSNETI_WAITING_FOR_MIP_REG_STATE:
    case DSSNETI_NET_IFACE_UP_WITH_MIP_STATE:
#ifdef FEATURE_DS_MOBILE_IP_DEREG
    case DSSNETI_WAITING_FOR_MIP_DEREG_STATE:
#endif /* FEATURE_DS_MOBILE_IP_DEREG */
#endif /* FEATURE_DS_MOBILE_IP */
      /*---------------------------------------------------------------------
        Unexpected event. Report an error.
      ---------------------------------------------------------------------*/
      ERR("Unexpected UP_WITH_MIP_EV in %d state",  
          dssneti_sm_cb.netstate,0,0);
      /*lint -e506, -e774 */
      ASSERT(0);
      /*lint +e506, +e774 */
      break;

    default:
      /*---------------------------------------------------------------------
        Sanity Check
      ---------------------------------------------------------------------*/
      /*lint -e506, -e774 */
      ASSERT(0);
      /*lint +e506, +e774 */
  } /* end switch */

} /* dssnet_process_link_layer_up_with_mip_ev() */

/*===========================================================================
FUNCTION DSSNET_PROCESS_LINK_LAYER_UP_WITH_SIP_EV()

DESCRIPTION
  This function processes the DSSNET_LINK_LAYER_UP_WITH_SIP_EV. Changes state 
  to DSSNETI_NET_IFACE_UP_WITH_SIP_STATE or DSSNETI_CLOSING_STATE, based on 
  the qcmip value

DEPENDENCIES
  None.

RETURN VALUE
  None.

SIDE EFFECTS
  None.
===========================================================================*/
LOCAL void dssnet_process_link_layer_up_with_sip_ev
(
  ps_cmd_enum_type       cmd,                      /* PS command type      */
  void                  *data_ptr                  /* Pointer to user data */
)
{
#ifdef FEATURE_DS_MOBILE_IP
  int16  tear_down_errno;
#endif /* FEATURE_DS_MOBILE_IP */
/*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/
  MSG_MED("LINK_LAYER_UP_WITH_SIP_EV recd in %d state", 
          dssneti_sm_cb.netstate,0,0);

  switch(dssneti_sm_cb.netstate)
  {
    case DSSNETI_CLOSED_STATE:
    case DSSNETI_WAITING_FOR_PHY_IFACE_UP_STATE:
    case DSSNETI_WAITING_FOR_PHY_IFACE_DOWN_STATE:
    case DSSNETI_CLOSING_STATE:
      /*---------------------------------------------------------------------
        Ignore event.
      ---------------------------------------------------------------------*/
      MSG_MED("Ignoring LINK_LAYER_UP_WITH_SIP_EV in %d state",
               dssneti_sm_cb.netstate,0,0);
      break;

#ifdef FEATURE_DS_MOBILE_IP
    case DSSNETI_WAITING_FOR_LINK_LAYER_WITH_MIP_STATE:
      /*---------------------------------------------------------------------
        see if we will do sip fallback (qcmip = 1)
        if so, transition to DSSNETI_NET_IFACE_UP_WITH_SIP_STATE. 
        if not, transition to DSSNETI_CLOSING_STATE.
      ---------------------------------------------------------------------*/
      dssneti_sm_cb.work_mip = FALSE;
      if (dssneti_sm_cb.will_sip)
      {
        /*-------------------------------------------------------------------
          Transition to DSSNETI_NET_IFACE_UP_WITH_SIP_STATE

          FUTURE: when falling back to SIP, notify MIP to close ppp and
                  clean up state
        -------------------------------------------------------------------*/
        MSG_MED ("Fall back to Simple IP",0,0,0);

#ifdef FEATURE_CALL_THROTTLE
        dctm_post_ppp_call_status( PPP_SUCCESS_EV, PPP_PROTOCOL_IPCP,
                                   PPP_FAIL_REASON_NONE );
#endif /* FEATURE_CALL_THROTTLE */

        dssneti_set_netstate(DSSNETI_NET_IFACE_UP_WITH_SIP_STATE);
      }
      else if (dssneti_sm_cb.want_mip)
      {
#ifdef FEATURE_CALL_THROTTLE
        /* --------------------------------------------------------------------
          This case will be executed if this is laptop MIP call and if this is 
          a MIP only sockets calls. This only posts failure if this is a 
          MIP only sockets and laptop MIP calls.
        ---------------------------------------------------------------------*/
        if((dssneti_sm_cb.is_te2_client == FALSE) ||
           ((dssneti_sm_cb.is_te2_client == TRUE) && (dsat707_qcmip_val == 2)))
        {
          MSG_MED("Posting MIP FAILURE event for DCTM", 0, 0, 0 );
          dctm_post_mip_call_status( MIP_FAILURE_EV, 
                                     MIP_FAIL_REASON_PDSN_FAILURE );
        }
#endif /* FEATURE_CALL_THROTTLE */

        ps_iface_tear_down_cmd(dssneti_sm_cb.ps_iface_ptr,
                               &tear_down_errno,
                               NULL);
      }
      else
      {
        /*-------------------------------------------------------------------
          qcmip is QCMIP_SIP_ONLY. We should never run into this case. 
          Report error
        -------------------------------------------------------------------*/
        MSG_ERROR("Up for SimpleIP, but prov'd for MIP only!",0,0,0);
        /*lint -e506, -e774 */
        ASSERT(0);
        /*lint +e506, +e774 */
      }
      break;

    case DSSNETI_WAITING_FOR_MIP_REG_STATE:
#ifdef FEATURE_DS_MOBILE_IP_DEREG
    case DSSNETI_WAITING_FOR_MIP_DEREG_STATE:
#endif /* FEATURE_DS_MOBILE_IP_DEREG */
    case DSSNETI_NET_IFACE_UP_WITH_MIP_STATE:
#endif /* FEATURE_DS_MOBILE_IP */
    case DSSNETI_NET_IFACE_UP_WITH_SIP_STATE:
      /*---------------------------------------------------------------------
        Unexpected event. Report an error.
      ---------------------------------------------------------------------*/
      MSG_ERROR("Unexpected UP_WITH_SIP_EV in %d state",
                 dssneti_sm_cb.netstate,0,0);
      /*lint -e506, -e774 */
      ASSERT(0);
      /*lint +e506, +e774 */
      break;
      
    case DSSNETI_PPP_RESYNC_STATE:
      MSG_MED("PPP resync is done on iface",0,0,0);
    case DSSNETI_WAITING_FOR_LINK_LAYER_WITH_SIP_STATE:
#ifdef FEATURE_DS_MOBILE_IP
      if (!dssneti_sm_cb.will_sip)
      {
        ps_iface_tear_down_cmd(dssneti_sm_cb.ps_iface_ptr,
                               &tear_down_errno,
                               NULL);
      }
      else
#endif /* FEATURE_DS_MOBILE_IP */
      {
        /*-------------------------------------------------------------------
          Transition to DSSNETI_NET_IFACE_UP_WITH_SIP_STATE

          FUTURE: when falling back to SIP, notify MIP to close ppp and
                  clean up state
        -------------------------------------------------------------------*/
        MSG_MED ("Fall back to Simple IP",0,0,0);

#ifdef FEATURE_CALL_THROTTLE
        dctm_post_ppp_call_status( PPP_SUCCESS_EV, PPP_PROTOCOL_IPCP,
                                   PPP_FAIL_REASON_NONE );
#endif /* FEATURE_CALL_THROTTLE */

        dssneti_set_netstate(DSSNETI_NET_IFACE_UP_WITH_SIP_STATE);
      }
      break;
      
    default:
      /*---------------------------------------------------------------------
        Sanity Check
      ---------------------------------------------------------------------*/
      /*lint -e506, -e774 */
      ASSERT(0);
      /*lint +e506, +e774 */
  } /* end switch */

} /* dssnet_process_link_layer_up_with_sip_ev() */

/*===========================================================================
FUNCTION DSSNET_PROCESS_MIP_UP_SUCCESS_EV()

DESCRIPTION
  This function processes the DSSNET_MIP_UP_SUCCESS_EV. Changes state to
  DSSNET_NET_IFACE_UP_WITH_MIP_STATE

DEPENDENCIES
  None.

RETURN VALUE
  None.

SIDE EFFECTS
  None.
===========================================================================*/
LOCAL void dssnet_process_mip_up_success_ev
(
  ps_cmd_enum_type       cmd,                      /* PS command type      */
  void                  *data_ptr                  /* Pointer to user data */
)
{
/*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  MSG_MED("MIP_UP_SUCCESS_EV recd in %d state", 
          dssneti_sm_cb.netstate,0,0);

  switch(dssneti_sm_cb.netstate)
  {
    case DSSNETI_CLOSED_STATE:
    case DSSNETI_WAITING_FOR_PHY_IFACE_UP_STATE:
    case DSSNETI_WAITING_FOR_LINK_LAYER_WITH_SIP_STATE:
    case DSSNETI_NET_IFACE_UP_WITH_SIP_STATE:
    case DSSNETI_WAITING_FOR_PHY_IFACE_DOWN_STATE:
    case DSSNETI_CLOSING_STATE:
    case DSSNETI_PPP_RESYNC_STATE:
#ifdef FEATURE_DS_MOBILE_IP
    case DSSNETI_WAITING_FOR_LINK_LAYER_WITH_MIP_STATE:
    case DSSNETI_NET_IFACE_UP_WITH_MIP_STATE:
#ifdef FEATURE_DS_MOBILE_IP_DEREG
    case DSSNETI_WAITING_FOR_MIP_DEREG_STATE:
#endif /* FEATURE_DS_MOBILE_IP_DEREG */
#endif /* FEATURE_DS_MOBILE_IP */
      /*---------------------------------------------------------------------
        Ignore event.
      ---------------------------------------------------------------------*/
      MSG_MED("Ignoring MIP_UP_SUCCESS_EV in %d state", 
               dssneti_sm_cb.netstate,0,0);
      break;

#ifdef FEATURE_DS_MOBILE_IP
    case DSSNETI_WAITING_FOR_MIP_REG_STATE:
      /*---------------------------------------------------------------------
        Transition to DSSNETI_NET_IFACE_UP_WITH_MIP_STATE
      ---------------------------------------------------------------------*/
#ifdef FEATURE_CALL_THROTTLE
      /* ----------------------------------------------------------------------
         For Laptop calls MIP success will be reported from RMSM on receiving 
         UM iFace up.
      -----------------------------------------------------------------------*/
      if (dssneti_sm_cb.is_te2_client == FALSE)
      {
        MSG_MED("Posting MIP SUCCESS event for DCTM", 0, 0, 0 );
        dctm_post_mip_call_status( MIP_SUCCESS_EV, MIP_FAIL_REASON_NONE );
      }
#endif /* FEATURE_CALL_THROTTLE */

      dssneti_set_netstate(DSSNETI_NET_IFACE_UP_WITH_MIP_STATE);
      break;
#endif /* FEATURE_DS_MOBILE_IP */

    default:
      /*---------------------------------------------------------------------
        Sanity Check
      ---------------------------------------------------------------------*/
      /*lint -e506, -e774 */
      ASSERT(0);
      /*lint +e506, +e774 */
  } /* end switch */

} /* dssnet_process_mip_up_success_ev() */

/*===========================================================================
FUNCTION DSSNET_PROCESS_MIP_FAILURE_EV()

DESCRIPTION
  This function processes the DSSNET_MIP_FAILURE_EV. Changes state to 
  DSSNETI_WAITING_FOR_LINK_LAYER_WITH_SIP_STATE or DSSNETI_CLOSING_STATE 
  based on qcmip value
  

DEPENDENCIES
  None.

RETURN VALUE
  None.

SIDE EFFECTS
  None.
===========================================================================*/
LOCAL void dssnet_process_mip_failure_ev
(
  ps_cmd_enum_type       cmd,                      /* PS command type      */
  void                  *data_ptr                  /* Pointer to user data */
)
{
  int16  tear_down_errno;
#ifdef FEATURE_DS_MOBILE_IP
  ppp_dev_opts_type           ppp_config;  
#endif /* FEATURE_DS_MOBILE_IP */
/*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  MSG_MED("MIP_FAILURE_EV recd in %d state", 
           dssneti_sm_cb.netstate,0,0);

  switch(dssneti_sm_cb.netstate)
  {
    case DSSNETI_CLOSED_STATE:
    case DSSNETI_WAITING_FOR_PHY_IFACE_UP_STATE:
    case DSSNETI_WAITING_FOR_LINK_LAYER_WITH_SIP_STATE:
    case DSSNETI_WAITING_FOR_PHY_IFACE_DOWN_STATE:
    case DSSNETI_CLOSING_STATE:
      /*---------------------------------------------------------------------
        Ignore event. 
      ---------------------------------------------------------------------*/
      MSG_MED("Ignoring MIP_FAILURE_EV in %d state",
               dssneti_sm_cb.netstate,0,0);
      break;

#ifdef FEATURE_DS_MOBILE_IP
    case DSSNETI_WAITING_FOR_LINK_LAYER_WITH_MIP_STATE:
    case DSSNETI_WAITING_FOR_MIP_REG_STATE:
      dssneti_sm_cb.work_mip = FALSE; /* MIP failed */
      /*---------------------------------------------------------------------
        Check to see the value of QCMIP. It qcmip==QCMIP_MIP_PREFERRED, 
        transition to DSSNETI_WAITING_FOR_LINK_LAYER_WITH_SIP_STATE. If 
        qcmip==QCMIP_MIP_ONLY, transition to DSSNETI_CLOSING_STATE.
      ---------------------------------------------------------------------*/
      if (dssneti_sm_cb.will_sip)
      {
        MSG_MED("Fallback to SIP" ,0,0,0);
        /*-------------------------------------------------------------------
         Setup PPP configuration options. 
        -------------------------------------------------------------------*/
        ppp_get_default_opts(&ppp_config);
        ppp_config.lcp_info.mode = PPP_CP_ACTIVE;
        if(dssneti_sm_cb.disable_lcp_auth == TRUE)
        {
          /*-----------------------------------------------------------------
            Disable PPP auth. 
          -----------------------------------------------------------------*/
          ppp_config.lcp_info.will_mask[FSM_REMOTE] &= ~LCP_N_AP;
          ppp_config.lcp_info.want_mask[FSM_REMOTE] &= ~LCP_N_AP;
          ppp_config.lcp_info.will_mask[FSM_LOCAL] &= ~LCP_N_AP;
          ppp_config.lcp_info.want_mask[FSM_LOCAL] &= ~LCP_N_AP;
        }

        ppp_config.ipcp_info.mode = PPP_CP_ACTIVE;
        ppp_config.hdlc_mode = HDLC_SW;
        ppp_config.rx_iface_ptr = dssneti_sm_cb.ps_iface_ptr;
  #ifdef FEATURE_JCDMA_1X
        ppp_config.setup_timeout = JCDMA_MAX_PPP_SETUP_TIME;
  #endif /* FEATURE_JCDMA_1X */
        ppp_config.rx_f_ptr = ds707_pkt_rx_um_SN_data;
        ppp_config.tx_f_ptr = ds707_pkt_tx_um_SN_data;
  
        /*-------------------------------------------------------------------
          Set default DNS values in iface. PPP will update these with the 
          network assigned values if DNS gets negotiated.
        -------------------------------------------------------------------*/      
        dssneti_sm_cb.ps_iface_ptr->v4_net_info.primary_dns = 
          dsat_qcdns_val[0];
        dssneti_sm_cb.ps_iface_ptr->v4_net_info.secondary_dns = 
          dsat_qcdns_val[1];
  
        /*-------------------------------------------------------------------
          Configure the PPP auth info for Um and start ppp on the device
        -------------------------------------------------------------------*/
        is707_get_ppp_auth_info_from_nv( &ppp_config, ps_get_nv_item );

        /*-------------------------------------------------------------------
          Start PPP on Um and transition to resyncing state
        -------------------------------------------------------------------*/
        if (ppp_resync(PPP_UM_SN_DEV, PPP_PROTOCOL_LCP, &ppp_config) == -1)
        {
          /*-----------------------------------------------------------------
            If Um resync fails for some reason, kill the call
          -----------------------------------------------------------------*/
          MSG_MED ("Couldn't resync Um, kill call",0,0,0);
          dssneti_set_netstate (DSSNETI_CLOSING_STATE);
        }
        else
        {
          dssneti_set_netstate (DSSNETI_PPP_RESYNC_STATE);
        }
      }
      else if(dssneti_sm_cb.want_mip)
      {
#ifdef FEATURE_CALL_THROTTLE
        /*--------------------------------------------------------------------
          This case will be executed if this is laptop MIP call and if this is 
          a MIP only sockets calls. This only posts failure if this is a 
          MIP only sockets or laptop MIP call.
        ---------------------------------------------------------------------*/
        if((dssneti_sm_cb.is_te2_client == FALSE) ||
           ((dssneti_sm_cb.is_te2_client == TRUE) && (dsat707_qcmip_val == 2)))
        {
          MSG_MED("Posting MIP FAILURE event for DCTM", 0, 0, 0 );
          dctm_post_mip_call_status( MIP_FAILURE_EV, 
                                     mip_ppp_status.mip_fail_reason );
        }
#endif /* FEATURE_CALL_THROTTLE */

        ps_iface_tear_down_cmd(dssneti_sm_cb.ps_iface_ptr,
                               &tear_down_errno,
                               NULL);
      }
      else
      {
        /*-------------------------------------------------------------------
          qcmip is QCMIP_SIP_ONLY. We should never run into this case. 
          Report error
        -------------------------------------------------------------------*/
        MSG_ERROR("In UP_WITH_MIP state and qcmip is QCMIP_SIP_ONLY",0,0,0);
        /*lint -e506, -e774 */
        ASSERT(0);
        /*lint +e506, +e774 */
      }
      break;
#endif /* FEATURE_DS_MOBILE_IP */

#ifdef FEATURE_DS_MOBILE_IP
    case DSSNETI_NET_IFACE_UP_WITH_MIP_STATE:
#endif /* FEATURE_DS_MOBILE_IP */

    case DSSNETI_NET_IFACE_UP_WITH_SIP_STATE:
    case DSSNETI_PPP_RESYNC_STATE:
      /*---------------------------------------------------------------------
        Transition to DSSNETI_CLOSING_STATE. 
      ---------------------------------------------------------------------*/
      ps_iface_tear_down_cmd(dssneti_sm_cb.ps_iface_ptr,
                             &tear_down_errno,
                             NULL);
      break;

#ifdef FEATURE_DS_MOBILE_IP
#ifdef FEATURE_DS_MOBILE_IP_DEREG
    case DSSNETI_WAITING_FOR_MIP_DEREG_STATE:
      MSG_ERROR("dssnet unexpected MIP_FAILURE_EV in %d state",
                dssneti_sm_cb.netstate,0,0);
      break;
#endif /* FEATURE_DS_MOBILE_IP_DEREG */
#endif /* FEATURE_DS_MOBILE_IP */

    default:
      /*---------------------------------------------------------------------
        Sanity Check
      ---------------------------------------------------------------------*/
      /*lint -e506, -e774 */
      ASSERT(0);
      /*lint +e506, +e774 */
  } /* end switch */

} /* dssnet_process_mip_failure_ev() */

#ifdef FEATURE_DS_MOBILE_IP
/*===========================================================================
FUNCTION DSSNETI_HANDLE_MIP_EV_CBACK()

DESCRIPTION
  Called when the MIP call come sup successfully or fails.
  This posts an appropriate event to the dssnet4 state machine.

DEPENDENCIES
  None

RETURN VALUE
  None

SIDE EFFECTS
  None
===========================================================================*/
static void dssneti_handle_mip_ev_cback
(
  mip_event_e_type        mip_event,             /* Down/Up/Resync         */
  mip_fail_reason_e_type  mip_fail_reason,
  void                    *user_data
)
{
/*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/
  MSG_MED( "Rcvd MIP event %d with fail reason %d", 
           mip_event, mip_fail_reason, 0 );

  switch(mip_event)
  {
    case MIP_SUCCESS_EV:
    {
      /*-----------------------------------------------------------------------
        Post an event to the DSSNET State machine that M. IP registration has
        completed successfully.
      -----------------------------------------------------------------------*/
      (void) dssnet_sm_post_event(DSSNET_MIP_UP_SUCCESS_EV);
      break;
    }

#ifdef FEATURE_DS_MOBILE_IP
#ifdef FEATURE_DS_MOBILE_IP_DEREG
    case MIP_DEREGED_EV:
    {
      /*lint -e534 return value not required */
      (void)dssnet_sm_post_event( DSSNET_MIP_DEREGED_EV);
      /*lint -restore*/
      break;
    }
#endif /* FEATURE_DS_MOBILE_IP_DEREG */
#endif /* FEATURE_DS_MOBILE_IP */

    case MIP_FAILURE_EV:
    {
#ifdef FEATURE_CALL_THROTTLE
      mip_ppp_status.mip_fail_reason = mip_fail_reason;
      mip_ppp_status.ppp_fail_reason = PPP_FAIL_REASON_NONE;
      mip_ppp_status.ppp_protocol = PPP_PROTOCOL_MIN;
#endif /* FEATURE_CALL_THROTTLE */

      /*-----------------------------------------------------------------------
        Post an event to the DSSNET State machine that M. IP registration has
        failed.
      -----------------------------------------------------------------------*/
      (void) dssnet_sm_post_event(DSSNET_MIP_FAILURE_EV);
      break;
    }
    default:
    {
      MSG_ERROR("dssnet4 0x%x got unsupported MIP event %d ", mip_event, 0, 0);
      ASSERT(0);
    }
  }
} /* dssneti_handle_mip_ev_cback() */

#ifdef FEATURE_DS_MOBILE_IP_DEREG
/*===========================================================================
FUNCTION DSSNET4_PROCESS_MIP_DEREGED_EV()

DESCRIPTION
  This function processes the DSSNET_MIP_DEREGED_EV. Changes state to
  DSSNETI_LINK_CLOSING_STATE if in DSSNETI_WAITING_FOR_MIP_DEREG_STATE

DEPENDENCIES
  None

RETURN VALUE
  None

SIDE EFFECTS
  None
===========================================================================*/
LOCAL void dssneti_process_mip_dereged_ev
(
  ps_cmd_enum_type       cmd,                      /* PS command type      */
  void                  *data_ptr                  /* Pointer to user data */
)
{
  MSG_MED("MIP_DEREGED_EV recd in %d state",dssneti_sm_cb.netstate,0,0);

  switch(dssneti_sm_cb.netstate)
  {
    case DSSNETI_WAITING_FOR_MIP_DEREG_STATE:
      dssneti_set_netstate(DSSNETI_CLOSING_STATE);
      break;

    case DSSNETI_CLOSING_STATE:
    case DSSNETI_CLOSED_STATE:
      MSG_HIGH("Ignoring MIP_DEREG in CLOSING states",0,0,0);
      break;

    case DSSNETI_NET_IFACE_UP_WITH_MIP_STATE:
    case DSSNETI_WAITING_FOR_LINK_LAYER_WITH_MIP_STATE:
    case DSSNETI_WAITING_FOR_LINK_LAYER_WITH_SIP_STATE:
    case DSSNETI_WAITING_FOR_MIP_REG_STATE:
    case DSSNETI_PPP_RESYNC_STATE:
      /* Fall through */

    default:
      MSG_ERROR("dssnet4 unexpected MIP_DEREGED_EV in %d state",
                dssneti_sm_cb.netstate,0,0);
      /*---------------------------------------------------------------------
        Sanity Check
      ---------------------------------------------------------------------*/
      ASSERT(0);
  }
}
#endif /* FEATURE_DS_MOBILE_IP_DEREG */
#endif /* FEATURE_DS_MOBILE_IP */

/*===========================================================================
FUNCTION DSSNET_PROCESS_NET_IFACE_DOWN_EV()

DESCRIPTION
  This function processes the DSSNET_NET_IFACE_DOWN_EV. Changes state to 
  DSSNETI_WAITING_FOR_LINK_LAYER_WITH_SIP_STATE or 
  DSSNETI_WAITING_FOR_PHY_IFACE_DOWN_STATE based on qcmip value

DEPENDENCIES
  None.

RETURN VALUE
  None.

SIDE EFFECTS
  None.
===========================================================================*/
LOCAL void dssnet_process_net_iface_down_ev
(
  ps_cmd_enum_type       cmd,                      /* PS command type      */
  void                  *data_ptr                  /* Pointer to user data */
)
{
  sint15 tear_down_errno;
/*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  MSG_MED("NET_IFACE_DOWN_EV recd in %d state", 
          dssneti_sm_cb.netstate,0,0);

  switch(dssneti_sm_cb.netstate)
  {
    case DSSNETI_CLOSED_STATE:
    case DSSNETI_WAITING_FOR_PHY_IFACE_UP_STATE:
    case DSSNETI_WAITING_FOR_PHY_IFACE_DOWN_STATE:
      /*---------------------------------------------------------------------
        Ignore event.
      ---------------------------------------------------------------------*/
      MSG_MED("Ignoring NET_IFACE_DOWN_EV in %d state",
               dssneti_sm_cb.netstate,0,0);
      break;

#ifdef FEATURE_DS_MOBILE_IP
    case DSSNETI_WAITING_FOR_LINK_LAYER_WITH_MIP_STATE:
    case DSSNETI_WAITING_FOR_MIP_REG_STATE:
      dssneti_sm_cb.work_mip = FALSE;
      /*---------------------------------------------------------------------
        Check to see the value of QCMIP. It qcmip==QCMIP_MIP_PREFERRED, 
        transition to DSSNETI_WAITING_FOR_LINK_LAYER_WITH_SIP_STATE. If 
        qcmip==QCMIP_MIP_ONLY, transition to 
        DSSNETI_WAITING_FOR_PHY_IFACE_DOWN_STATE.
      ---------------------------------------------------------------------*/
      if (dssneti_sm_cb.will_sip)
      {
        dssneti_set_netstate( DSSNETI_WAITING_FOR_LINK_LAYER_WITH_SIP_STATE);
      }
      else if (dssneti_sm_cb.want_mip)
      {    
#ifdef FEATURE_CALL_THROTTLE
        if(mip_ppp_status.ppp_fail_reason != PPP_FAIL_REASON_NONE)
        {
          MSG_MED("Posting PPP FAILURE event for DCTM.", 0, 0, 0 );
          dctm_post_ppp_call_status( PPP_FAILURE_EV, 
                                     mip_ppp_status.ppp_protocol, 
                                     mip_ppp_status.ppp_fail_reason );
        }
#endif /* FEATURE_CALL_THROTTLE */

        /*-------------------------------------------------------------------
          Bring down the ps_iface. This is necessary as otherwise the iface 
          state would not change from COMING_UP to GOING_DOWN and as a 
          consequence, the mobile would send out the wrong release reason.  
          Since PPP is already down, it won't give any more indications.
          Hence change state to bring down the traffic channel.          
        -------------------------------------------------------------------*/
        ps_iface_tear_down_cmd(dssneti_sm_cb.ps_iface_ptr,
                               &tear_down_errno,
                               NULL);
        dssneti_set_netstate( DSSNETI_WAITING_FOR_PHY_IFACE_DOWN_STATE);
      }
      else
      {
        /*-------------------------------------------------------------------
          qcmip is QCMIP_SIP_ONLY. We should never run into this case. 
          Report error
        -------------------------------------------------------------------*/
        MSG_ERROR("In UP_WITH_MIP state and qcmip is QCMIP_SIP_ONLY",0,0,0);
        /*lint -e506, -e774 */
        ASSERT(0);
        /*lint +e506, +e774 */
      }
      break;

#endif /* FEATURE_DS_MOBILE_IP */

    case DSSNETI_WAITING_FOR_LINK_LAYER_WITH_SIP_STATE:
#ifdef FEATURE_CALL_THROTTLE
      if(mip_ppp_status.ppp_fail_reason != PPP_FAIL_REASON_NONE)
      {
#ifdef FEATURE_DS_MOBILE_IP
        if((dssneti_sm_cb.is_te2_client == FALSE) ||
           ((dssneti_sm_cb.is_te2_client == TRUE) && (dsat707_qcmip_val == 2)))
#endif /* FEATURE_DS_MOBILE_IP */
        {
          MSG_MED("Posting PPP FAILURE event for DCTM", 0, 0, 0 );
          dctm_post_ppp_call_status( PPP_FAILURE_EV, 
                                     mip_ppp_status.ppp_protocol, 
                                     mip_ppp_status.ppp_fail_reason );
        }
      }
#endif /* FEATURE_CALL_THROTTLE */
      /* Fall through */

    case DSSNETI_NET_IFACE_UP_WITH_SIP_STATE:
    case DSSNETI_PPP_RESYNC_STATE:
#ifdef FEATURE_DS_MOBILE_IP
    case DSSNETI_NET_IFACE_UP_WITH_MIP_STATE:
#ifdef FEATURE_DS_MOBILE_IP_DEREG
    case DSSNETI_WAITING_FOR_MIP_DEREG_STATE:
#endif /* FEATURE_DS_MOBILE_IP_DEREG */
#endif /* FEATURE_DS_MOBILE_IP */
      /*-------------------------------------------------------------------
        Bring down the ps_iface. This is necessary as otherwise the iface 
        state would not change from COMING_UP to GOING_DOWN and as a 
        consequence, the mobile would send out the wrong release reason.  
        Since PPP is already down, it won't give any more indications.
        Hence change state to bring down the traffic channel.
      -------------------------------------------------------------------*/
      ps_iface_tear_down_cmd(dssneti_sm_cb.ps_iface_ptr,
                             &tear_down_errno,
                             NULL);

      /* fall through */

    case DSSNETI_CLOSING_STATE:
      /*---------------------------------------------------------------------
        Transition to DSSNETI_WAITING_FOR_PHY_IFACE_DOWN_STATE
      ---------------------------------------------------------------------*/
      dssneti_set_netstate(DSSNETI_WAITING_FOR_PHY_IFACE_DOWN_STATE);
      break;

    default:
      /*---------------------------------------------------------------------
        Sanity Check
      ---------------------------------------------------------------------*/
      /*lint -e506, -e774 */
      ASSERT(0);
      /*lint +e506, +e774 */
  } /* end switch */

} /* dssnet_process_net_iface_down_ev() */

/*===========================================================================
FUNCTION DSSNET_PROCESS_PPP_CLOSE_CMD_EV()

DESCRIPTION
  This function processes the DSSNET_PPP_CLOSE_CMD_EV. Changes state to 
  DSSNETI_CLOSING_STATE

DEPENDENCIES
  None.

RETURN VALUE
  None.

SIDE EFFECTS
  None
===========================================================================*/
LOCAL void dssnet_process_ppp_close_cmd_ev
(
  ps_cmd_enum_type       cmd,                      /* PS command type      */
  void                  *data_ptr                  /* Pointer to user data */
)
{
/*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  MSG_MED("PPP_CLOSE_CMD_EV recd in %d state",
          dssneti_sm_cb.netstate,0,0);

  switch(dssneti_sm_cb.netstate)
  {
    case DSSNETI_CLOSING_STATE:
    case DSSNETI_WAITING_FOR_PHY_IFACE_DOWN_STATE:
    case DSSNETI_CLOSED_STATE:
      /*---------------------------------------------------------------------
        Ignore event. 
      ---------------------------------------------------------------------*/
      MSG_MED("Ignoring PPP_CLOSE_CMD_EV in %d state",
               dssneti_sm_cb.netstate,0,0);
      break;

    case DSSNETI_WAITING_FOR_PHY_IFACE_UP_STATE:
      /*-------------------------------------------------------------------
        If current state is WAITING_FOR_PHY_IFACE_UP_STATE, transition to
        WAITING_FOR_PHY_IFACE_DOWN_STATE. 
      -------------------------------------------------------------------*/
      dssneti_set_netstate(DSSNETI_WAITING_FOR_PHY_IFACE_DOWN_STATE);
      break;

    case DSSNETI_WAITING_FOR_LINK_LAYER_WITH_SIP_STATE:
    case DSSNETI_NET_IFACE_UP_WITH_SIP_STATE:
    case DSSNETI_PPP_RESYNC_STATE:
#ifdef FEATURE_DS_MOBILE_IP
    case DSSNETI_WAITING_FOR_LINK_LAYER_WITH_MIP_STATE:
    case DSSNETI_WAITING_FOR_MIP_REG_STATE:
#endif // FEATURE_DS_MOBILE_IP
      /*-------------------------------------------------------------------
        Transition to CLOSING_STATE in all other cases 
      -------------------------------------------------------------------*/
      dssneti_set_netstate(DSSNETI_CLOSING_STATE);
      break;

#ifdef FEATURE_DS_MOBILE_IP
    case DSSNETI_NET_IFACE_UP_WITH_MIP_STATE:
#ifdef FEATURE_DS_MOBILE_IP_DEREG
     dssneti_set_netstate(DSSNETI_WAITING_FOR_MIP_DEREG_STATE);
#else
     dssneti_set_netstate(DSSNETI_CLOSING_STATE);
#endif /* FEATURE_DS_MOBILE_IP_DEREG */ 
     break;
#endif /* FEATURE_DS_MOBILE_IP */

#ifdef FEATURE_DS_MOBILE_IP
#ifdef FEATURE_DS_MOBILE_IP_DEREG
    case DSSNETI_WAITING_FOR_MIP_DEREG_STATE:
      MSG_ERROR( "Got PPP_CLOSE in MIPDEREG STATE",0,0,0);
      break;
#endif /* FEATURE_DS_MOBILE_IP_DEREG */
#endif /* FEATURE_DS_MOBILE_IP */

    default:
      /*---------------------------------------------------------------------
        Sanity Check
      ---------------------------------------------------------------------*/
      /*lint -e506, -e774 */
      ASSERT(0);
      /*lint +e506, +e774 */

  } /* end switch */

} /* dssnet_process_ppp_close_cmd_ev() */

/*===========================================================================
FUNCTION DSSNET_PROCESS_PHY_IFACE_TIMER_EV()

DESCRIPTION
  This function processes the DSSNETI_PHY_IFACE_TIMER_EXP_EV. Changes state to
  DSSNETI_CLOSED_STATE

DEPENDENCIES
  None.

RETURN VALUE
  None.

SIDE EFFECTS
  None
===========================================================================*/
LOCAL void dssnet_process_phy_iface_timer_ev
(
  ps_cmd_enum_type       cmd,                      /* PS command type      */
  void                  *data_ptr                  /* Pointer to user data */
)
{
/*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  MSG_MED("PHY_LAYER_TIMER_EXP_EV recd in %d state",
          dssneti_sm_cb.netstate,0,0);

  switch(dssneti_sm_cb.netstate)
  {
    case DSSNETI_CLOSED_STATE:
      /*---------------------------------------------------------------------
        There is a possibility that the timer fires when dssnet is in the 
        CLOSED state. Ignore this event if it occurs.  
      ---------------------------------------------------------------------*/
      break;

    case DSSNETI_WAITING_FOR_LINK_LAYER_WITH_SIP_STATE:
    case DSSNETI_NET_IFACE_UP_WITH_SIP_STATE:
    case DSSNETI_CLOSING_STATE:
    case DSSNETI_PPP_RESYNC_STATE:
#ifdef FEATURE_DS_MOBILE_IP
    case DSSNETI_WAITING_FOR_LINK_LAYER_WITH_MIP_STATE:
    case DSSNETI_WAITING_FOR_MIP_REG_STATE:
    case DSSNETI_NET_IFACE_UP_WITH_MIP_STATE:
#endif /* FEATURE_DS_MOBILE_IP */
      /*---------------------------------------------------------------------
        Ignore event.
      ---------------------------------------------------------------------*/
      MSG_ERROR("Unexpected PHY_IFACE_TIMER_EXP_EV in %d state",
                 dssneti_sm_cb.netstate,0,0);
      break;

    case DSSNETI_WAITING_FOR_PHY_IFACE_UP_STATE:
      /*---------------------------------------------------------------------
        Transition to DSSNETI_WAITING_FOR_PHY_IFACE_DOWN_STATE.
      ---------------------------------------------------------------------*/
      dssneti_set_netstate(DSSNETI_WAITING_FOR_PHY_IFACE_DOWN_STATE);
      break;

    case DSSNETI_WAITING_FOR_PHY_IFACE_DOWN_STATE:
      /*---------------------------------------------------------------------
        Transition to DSSNETI_CLOSED_STATE.
      ---------------------------------------------------------------------*/
      dssneti_set_netstate(DSSNETI_CLOSED_STATE);
      break;

    default:
      /*---------------------------------------------------------------------
        Sanity Check
      ---------------------------------------------------------------------*/
      /*lint -e506, -e774 */
      ASSERT(0);
      /*lint +e506, +e774 */
  } /* end switch */

} /* dssnet_process_phy_iface_timer_ev() */

/*===========================================================================
FUNCTION DSSNET_PROCESS_PHY_IFACE_DOWN_EV()

DESCRIPTION
  This function processes the DSSNET_PHY_IFACE_DOWN_EV. Changes state to
  DSSNETI_CLOSED_STATE

DEPENDENCIES
  None.

RETURN VALUE
  None.

SIDE EFFECTS
  None
===========================================================================*/
LOCAL void dssnet_process_phy_iface_down_ev
(
  ps_cmd_enum_type       cmd,                      /* PS command type      */
  void                  *data_ptr                  /* Pointer to user data */
)
{
  ps_timer_error_type timer_result;
/*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  MSG_MED("PHY_LAYER_DOWN_EV recd in %d state",
          dssneti_sm_cb.netstate,0,0);

  switch(dssneti_sm_cb.netstate)
  {
    case DSSNETI_CLOSED_STATE:
    case DSSNETI_WAITING_FOR_LINK_LAYER_WITH_SIP_STATE:
    case DSSNETI_NET_IFACE_UP_WITH_SIP_STATE:
    case DSSNETI_CLOSING_STATE:
    case DSSNETI_PPP_RESYNC_STATE:
#ifdef FEATURE_DS_MOBILE_IP
    case DSSNETI_WAITING_FOR_LINK_LAYER_WITH_MIP_STATE:
    case DSSNETI_WAITING_FOR_MIP_REG_STATE:
    case DSSNETI_NET_IFACE_UP_WITH_MIP_STATE:
#endif /* FEATURE_DS_MOBILE_IP */
      /*---------------------------------------------------------------------
        Ignore event.
      ---------------------------------------------------------------------*/
      MSG_MED("Ignoring PHY_IFACE_DOWN_EV in %d state",
               dssneti_sm_cb.netstate,0,0);
      break;

    case DSSNETI_WAITING_FOR_PHY_IFACE_UP_STATE:
      /*---------------------------------------------------------------------
        Transition to waiting_for_phy_iface_down_state.  
      ---------------------------------------------------------------------*/
      dssneti_set_netstate(DSSNETI_WAITING_FOR_PHY_IFACE_DOWN_STATE);
      break;

    case DSSNETI_WAITING_FOR_PHY_IFACE_DOWN_STATE:
      /*---------------------------------------------------------------------
       Traffic channel is torn down. Cancel the timer
      ---------------------------------------------------------------------*/
      timer_result = 
             ps_timer_cancel(dssneti_sm_cb.phy_iface_timer);
      ASSERT(timer_result != PS_TIMER_FAILURE);
      /*---------------------------------------------------------------------
        Transition to DSSNETI_CLOSED_STATE.
      ---------------------------------------------------------------------*/
      dssneti_set_netstate(DSSNETI_CLOSED_STATE);
      break;

#ifdef FEATURE_DS_MOBILE_IP
#ifdef FEATURE_DS_MOBILE_IP_DEREG
    case DSSNETI_WAITING_FOR_MIP_DEREG_STATE:
      /*---------------------------------------------------------------------
        Call ppp_abort().
      ---------------------------------------------------------------------*/
      (void)ppp_abort(PPP_UM_SN_DEV);
      /*---------------------------------------------------------------------
       Traffic channel is torn down. Cancel the timer
      ---------------------------------------------------------------------*/
      timer_result = ps_timer_cancel(dssneti_sm_cb.phy_iface_timer);
      ASSERT(timer_result != PS_TIMER_FAILURE);
      /*---------------------------------------------------------------------
        Transition to DSSNETI_CLOSED_STATE.
      ---------------------------------------------------------------------*/
      dssneti_set_netstate(DSSNETI_CLOSED_STATE);
      break;
#endif /* FEATURE_DS_MOBILE_IP_DEREG */
#endif /* FEATURE_DS_MOBILE_IP */

    default:
      /*---------------------------------------------------------------------
        Sanity Check
      ---------------------------------------------------------------------*/
      /*lint -e506, -e774 */
      ASSERT(0);
      /*lint +e506, +e774 */
  } /* end switch */

} /* dssnet_process_phy_iface_down_ev() */

/*===========================================================================
FUNCTION DSSNET_PROCESS_PPP_RESYNC_EV()

DESCRIPTION
  This function processes the DSSNET_PPP_RESYNC_EV event. Changes State
  to DSSNETI_PPP_RESYNC_STATE

DEPENDENCIES
  None.

RETURN VALUE
  None.

SIDE EFFECTS
  None.
===========================================================================*/
LOCAL void dssnet_process_ppp_resync_ev
(
  ps_cmd_enum_type       cmd,                      /* PS command type      */
  void                  *data_ptr                  /* Pointer to user data */
)
{
/*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  MSG_MED("PPP_RESYNC_EV recd in %d state",
          dssneti_sm_cb.netstate,0,0);

  switch(dssneti_sm_cb.netstate)
  {
    case DSSNETI_CLOSED_STATE:
    case DSSNETI_WAITING_FOR_PHY_IFACE_UP_STATE:
    case DSSNETI_WAITING_FOR_PHY_IFACE_DOWN_STATE:
    case DSSNETI_WAITING_FOR_LINK_LAYER_WITH_SIP_STATE:
    case DSSNETI_CLOSING_STATE:
    case DSSNETI_PPP_RESYNC_STATE:
#ifdef FEATURE_DS_MOBILE_IP
    case DSSNETI_WAITING_FOR_LINK_LAYER_WITH_MIP_STATE:
#endif /* FEATURE_DS_MOBILE_IP */
      /*---------------------------------------------------------------------
        Ignore event.
      ---------------------------------------------------------------------*/
      MSG_MED("Ignoring PPP_RESYNC_EV in %d state",
               dssneti_sm_cb.netstate,0,0);
      break;

#ifdef FEATURE_DS_MOBILE_IP
    case DSSNETI_NET_IFACE_UP_WITH_MIP_STATE:
    case DSSNETI_WAITING_FOR_MIP_REG_STATE:
      /* fall through */
#endif /* FEATURE_DS_MOBILE_IP */

    case DSSNETI_NET_IFACE_UP_WITH_SIP_STATE:
      /*---------------------------------------------------------------------
        Transition to DSSNETI_PPP_RESYNC_STATE.
      ---------------------------------------------------------------------*/
      dssneti_set_netstate(DSSNETI_PPP_RESYNC_STATE);
      break;

#ifdef FEATURE_DS_MOBILE_IP
#ifdef FEATURE_DS_MOBILE_IP_DEREG
    case DSSNETI_WAITING_FOR_MIP_DEREG_STATE:
      MSG_MED( "got LINK_RESYNC_EV in MIP_DEREG state, closing PPP",0,0,0);
      dssneti_set_netstate(DSSNETI_CLOSING_STATE);
      break;
#endif /* FEATURE_DS_MOBILE_IP_DEREG */
#endif /* FEATURE_DS_MOBILE_IP */

    default:
      /*---------------------------------------------------------------------
        Sanity Check
      ---------------------------------------------------------------------*/
      /*lint -e506, -e774 */
      ASSERT(0);
      /*lint +e506, +e774 */
  } /* end switch */

} /* dssnet_process_ppp_resync_ev() */

/*===========================================================================
FUNCTION DSSNET_PROCESS_PHY_LINK_DOWN_PPP_ABORT_CMD_EV()

DESCRIPTION
  This function processes the DSSNET_PPP_RESYNC_EV event. Changes State
  to DSSNETI_PPP_RESYNC_STATE

DEPENDENCIES
  None.

RETURN VALUE
  None.

SIDE EFFECTS
  None.
===========================================================================*/
LOCAL void dssnet_process_phy_link_down_ppp_abort_cmd_ev
(
  ps_cmd_enum_type       cmd,                      /* PS command type      */
  void                  *data_ptr                  /* Pointer to user data */
)
{
/*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  MSG_MED("PPP_ABORT_CMD_EV recd in %d state",
          dssneti_sm_cb.netstate,0,0);

  switch(dssneti_sm_cb.netstate)
  {
    case DSSNETI_WAITING_FOR_PHY_IFACE_UP_STATE:
    case DSSNETI_WAITING_FOR_PHY_IFACE_DOWN_STATE:
    case DSSNETI_WAITING_FOR_LINK_LAYER_WITH_SIP_STATE:
    case DSSNETI_NET_IFACE_UP_WITH_SIP_STATE:
    case DSSNETI_CLOSING_STATE:
    case DSSNETI_PPP_RESYNC_STATE:
#ifdef FEATURE_DS_MOBILE_IP
    case DSSNETI_WAITING_FOR_LINK_LAYER_WITH_MIP_STATE:
    case DSSNETI_WAITING_FOR_MIP_REG_STATE:
    case DSSNETI_NET_IFACE_UP_WITH_MIP_STATE:
#endif /* FEATURE_DS_MOBILE_IP */
      /*---------------------------------------------------------------------
        Call ppp_abort().
      ---------------------------------------------------------------------*/
      ppp_abort(PPP_UM_SN_DEV);

      /*---------------------------------------------------------------------
        If the local ps_iface_ptr is NULL, set it. 
      ---------------------------------------------------------------------*/
      if(dssneti_sm_cb.ps_iface_ptr == NULL)
      {
        /*-------------------------------------------------------------------
          Retrieve the ps_iface_ptr from the 1x controller.
        -------------------------------------------------------------------*/
        dssneti_sm_cb.ps_iface_ptr =
                              ds707_pkt_get_iface_ptr(DS707_DEF_PKT_INSTANCE);
        ASSERT(dssneti_sm_cb.ps_iface_ptr != NULL);
      }

      /*---------------------------------------------------------------------
        Transition to DSSNETI_CLOSED_STATE.
      ---------------------------------------------------------------------*/
      dssneti_set_netstate(DSSNETI_CLOSED_STATE);
      break;

    case DSSNETI_CLOSED_STATE:

      MSG_ERROR("PPP ABORT called in Closed state",0,0,0);
      break;

    default:
      /*---------------------------------------------------------------------
        Sanity Check
      ---------------------------------------------------------------------*/
      /*lint -e506, -e774 */
      ASSERT(0);
      /*lint +e506, +e774 */
  } /* end switch */

} /* dssnet_process_phy_link_down_ppp_abort_cmd_ev() */

/*===========================================================================

                      EXTERNAL FUNCTION DEFINITIONS

===========================================================================*/

/*===========================================================================
FUNCTION DSSNET_SM_POST_EVENT()

DESCRIPTION
  This function is used to post events to the dssnet_sm by other modules.  

DEPENDENCIES
  None.

RETURN VALUE
  None.

SIDE EFFECTS
  None.
===========================================================================*/
void dssnet_sm_post_event
(
  dssnet_sm_event_type  event                         /* dssnet event type */
)
{
  ps_cmd_enum_type cmd_type;                                /* ps cmd type */
/*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  MSG_MED ("dssnet event %d in state %d.",
           event, dssneti_sm_cb.netstate,0);

  ASSERT(event < DSSNET_NUM_OF_EV);

  /*-------------------------------------------------------------------------
    Verify that this state machine has been initialied.
  -------------------------------------------------------------------------*/
  if(!dssneti_sm_initialized)
  {
    MSG_ERROR("DSSNET s/m not initialized",0,0,0);
  }
  
  /*-------------------------------------------------------------------------
    Initialize command name to -1.
  -------------------------------------------------------------------------*/
  cmd_type = (ps_cmd_enum_type)-1;

  /*-------------------------------------------------------------------------
    Based on the event type, set the appropriate PS cmd 
  -------------------------------------------------------------------------*/
  switch(event)
  {
    case DSSNET_PPP_OPEN_CMD_EV:
      cmd_type = PS_DSSNET_PPP_OPEN_CMD;
      break;

    case DSSNET_PHY_IFACE_UP_EV:
      cmd_type = PS_DSSNET_PHY_IFACE_UP_CMD;
      break;
     
    case DSSNETI_PHY_IFACE_TIMER_EXP_EV:
      cmd_type = PS_DSSNET_PHY_IFACE_TIMER_EXP_CMD;
      break;
    case DSSNET_LINK_LAYER_UP_WITH_MIP_EV:
      cmd_type = PS_DSSNET_LINK_LAYER_UP_WITH_MIP_CMD;
      break;

    case DSSNET_LINK_LAYER_UP_WITH_SIP_EV:
      cmd_type = PS_DSSNET_LINK_LAYER_UP_WITH_SIP_CMD;
      break;

    case DSSNET_MIP_UP_SUCCESS_EV:
      cmd_type = PS_DSSNET_MIP_UP_SUCCESS_CMD;
      break;

    case DSSNET_MIP_FAILURE_EV:
      cmd_type = PS_DSSNET_MIP_FAILURE_CMD;
      break;

#ifdef FEATURE_DS_MOBILE_IP
#ifdef FEATURE_DS_MOBILE_IP_DEREG
    case DSSNET_MIP_DEREGED_EV:
      cmd_type = PS_DSSNET_MIP_DEREGED_EV_CMD;
      break;
#endif /* FEATURE_DS_MOBILE_IP_DEREG */
#endif /* FEATURE_DS_MOBILE_IP */

    case DSSNET_NET_IFACE_DOWN_EV:
      cmd_type = PS_DSSNET_NET_IFACE_DOWN_CMD;
      break;

    case DSSNET_PPP_CLOSE_CMD_EV:
      cmd_type = PS_DSSNET_PPP_CLOSE_CMD;
      break;

    case DSSNET_PHY_IFACE_DOWN_EV:
      cmd_type = PS_DSSNET_PHY_IFACE_DOWN_CMD;
      break;
     
    case DSSNET_PPP_RESYNC_EV:
#ifdef FEATURE_DS_MOBILE_IP_PERF
      /*-------------------------------------------------------------------------
        Notify M.IP call performance that Um PPP negotiation is starting.
      -------------------------------------------------------------------------*/
      if (qw_cmp(mip_perf_info[PERF_UM_PPP_DELAY].ts_start, ts_null) == 0)
      {
        mip_perf_delay(&mip_perf_info[PERF_UM_PPP_DELAY],
                       PERF_UM_PPP_DELAY,
                       PERF_START_TS);
      }
#endif /* FEATURE_DS_MOBILE_IP_PERF */
      cmd_type = PS_DSSNET_PPP_RESYNC_CMD;
      break;

    case DSSNET_PHY_LINK_DOWN_PPP_ABORT_CMD_EV:
      cmd_type = PS_DSSNET_PHY_LINK_DOWN_PPP_ABORT_CMD;
      break;

    default:
      /*---------------------------------------------------------------------
        Sanity Check
      ---------------------------------------------------------------------*/
      /*lint -e506, -e774 */
      MSG_ERROR("Unknown DSSNET event",0,0,0);
      ASSERT(0);
      /*lint +e506, +e774 */
  } /* end switch */

  /*-------------------------------------------------------------------------
    Enqueue cmd with PS task if it has been set to a valid value 
  -------------------------------------------------------------------------*/
  if(cmd_type != (ps_cmd_enum_type)-1)
  {
    ps_send_cmd(cmd_type, NULL);
  } /* end if(cmd_type) */
  
} /* dssnet_sm_post_event() */


/*===========================================================================
FUNCTION DSSNET_SM_INIT()

DESCRIPTION
  This function is is used to initialize the dssnet state machine control 
  block. Called once during phone powerup initialization.

DEPENDENCIES
  None.

RETURN VALUE
  None.

SIDE EFFECTS
  None.
===========================================================================*/
void dssnet_sm_init
(
  void
)
{
  int  retval;
/*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  if(dssneti_sm_initialized == TRUE)
  {
    MSG_HIGH("DSSNET state machine already initialized", 0, 0, 0);
    return;
  }

  memset(&dssneti_sm_cb, 0, sizeof(dssneti_sm_cb));

  /*-------------------------------------------------------------------------
    Initialize control block. Allocate the phys_iface_up timers. 
  -------------------------------------------------------------------------*/
  retval = ps_timer_alloc(dssneti_phy_iface_timer_cb, NULL);
  if(retval == PS_TIMER_FAILURE)
  {
    ERR("Failed to allocate PS timer for DSSNET SM!", 0, 0, 0);
  }

  dssneti_sm_cb.phy_iface_timer  = (uint8) retval;
  dssneti_sm_cb.netstate         = DSSNETI_CLOSED_STATE;
  dssneti_sm_cb.ps_iface_ptr     = NULL;
  dssneti_sm_cb.disable_lcp_auth = FALSE;
  dssneti_sm_cb.te2_call_override_qcmip = FALSE;
  dssneti_sm_cb.is_te2_client    = FALSE;

  /*-------------------------------------------------------------------------
    Register the command handlers for DSSNET related events with PS.
  -------------------------------------------------------------------------*/
  ps_set_cmd_handler(PS_DSSNET_PPP_OPEN_CMD, 
                     dssnet_process_ppp_open_cmd_ev);
  ps_set_cmd_handler(PS_DSSNET_PHY_IFACE_UP_CMD, 
                     dssnet_process_phy_iface_up_ev);
  ps_set_cmd_handler(PS_DSSNET_LINK_LAYER_UP_WITH_MIP_CMD, 
                     dssnet_process_link_layer_up_with_mip_ev);
  ps_set_cmd_handler(PS_DSSNET_LINK_LAYER_UP_WITH_SIP_CMD, 
                     dssnet_process_link_layer_up_with_sip_ev);
  ps_set_cmd_handler(PS_DSSNET_MIP_UP_SUCCESS_CMD, 
                     dssnet_process_mip_up_success_ev);
  ps_set_cmd_handler(PS_DSSNET_MIP_FAILURE_CMD, 
                     dssnet_process_mip_failure_ev);
  ps_set_cmd_handler(PS_DSSNET_NET_IFACE_DOWN_CMD, 
                     dssnet_process_net_iface_down_ev);
  ps_set_cmd_handler(PS_DSSNET_PPP_CLOSE_CMD, 
                     dssnet_process_ppp_close_cmd_ev);
  ps_set_cmd_handler(PS_DSSNET_PHY_IFACE_TIMER_EXP_CMD, 
                     dssnet_process_phy_iface_timer_ev);
  ps_set_cmd_handler(PS_DSSNET_PHY_IFACE_DOWN_CMD, 
                     dssnet_process_phy_iface_down_ev);
  ps_set_cmd_handler(PS_DSSNET_PPP_RESYNC_CMD, 
                     dssnet_process_ppp_resync_ev);
  ps_set_cmd_handler(PS_DSSNET_PHY_LINK_DOWN_PPP_ABORT_CMD, 
                     dssnet_process_phy_link_down_ppp_abort_cmd_ev);
#ifdef FEATURE_DS_MOBILE_IP
#ifdef FEATURE_DS_MOBILE_IP_DEREG
  ps_set_cmd_handler(PS_DSSNET_MIP_DEREGED_EV_CMD, 
                     dssneti_process_mip_dereged_ev);
#endif /* FEATURE_DS_MOBILE_IP_DEREG */
#endif /* FEATURE_DS_MOBILE_IP */

  /*-------------------------------------------------------------------------
    Allocate ps_iface event buffers
  -------------------------------------------------------------------------*/
  if((dssneti_sm_cb.phys_iface_up_cback =
       ps_iface_alloc_event_cback_buf(dssneti_handle_phys_iface_ev_cback,
                                      NULL)) == NULL)
  {
    ERR_FATAL("Could not alloc evt buf",0,0,0);
  }

  if((dssneti_sm_cb.phys_iface_down_cback =
       ps_iface_alloc_event_cback_buf(dssneti_handle_phys_iface_ev_cback,
                                      NULL)) == NULL)
  {
    ERR_FATAL("Could not alloc evt buf",0,0,0);
  }

  /*-------------------------------------------------------------------------
    This state machine is now initialized
  -------------------------------------------------------------------------*/
  dssneti_sm_initialized = TRUE;
  MSG_MED("DSSNET state machine initialized",0,0,0);

} /* dssnet_sm_init() */


/*===========================================================================
FUNCTION IS707_GET_PPP_AUTH_INFO_FROM_NV

DESCRIPTION
  This function retrieves the 1x PPP user ID and password from NV
  and stores them in the provided PPP configuration structure.

DEPENDENCIES
  The NV task must be started and running.

RETURN VALUE
  None

SIDE EFFECTS
  Each call to 'nv_get' results in a Wait and watchdog kicking
===========================================================================*/
void is707_get_ppp_auth_info_from_nv
(
  ppp_dev_opts_type *ppp_config,
  nv_stat_enum_type (* nv_get)(nv_items_enum_type, nv_item_type *)
)
{
  nv_stat_enum_type    nv_status;     /* status from NV call */
  nv_item_type         nv_item;
  boolean              is_omh_enabled = FALSE;
  boolean              passwd_read_failed = FALSE;
  uint8                index = 0;	
/*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/
#ifdef FEATURE_DS_MULTIPLE_PROFILES
  is_omh_enabled = TRUE;
#endif /* FEATURE_DS_OMH */
  MSG_FATAL( "is707_get_ppp_auth_info_from_nv %d %d %d",is_omh_enabled , dsat707_qcmip_val, num_valid_profiles );
  if ( (is_omh_enabled == TRUE)
#ifdef FEATURE_DS_MOBILE_IP
       &&(dsat707_qcmip_val == 0)
#endif
#ifdef FEATURE_DS_MULTIPLE_PROFILES
       && (num_valid_profiles > 0)
#endif
     )
  {
    /*-------------------------------------------------------------------------
      Read the active profile from NV. 
    -------------------------------------------------------------------------*/
    nv_status = nv_get(NV_DS_SIP_ACTIVE_PROFILE_INDEX_I, &nv_item );
    if( nv_status == NV_DONE_S )
    {
      MSG_MED( "NV read success, active profile index is %d", 
               nv_item.ds_sip_active_profile_index, 0, 0 );
    }
    else
    {
      MSG_MED( "NV read failed for active profile index", 0, 0, 0 );
      ppp_config->auth_info.user_id_len = 0;
      ppp_config->auth_info.passwd_len = 0;
      return;
    }

    index = nv_item.ds_sip_active_profile_index;

    /*---------------------------------------------------------------------------
      Loading active profile index into provided PPP config. This facilitates to 
      Calculate CHAP response from RUIM.  
    ----------------------------------------------------------------------------*/ 	
#ifdef FEATURE_UIM_SUPPORT_3GPD
    ppp_config->auth_info.nai_entry_index=index;
#endif /* FEATURE_UIM_SUPPORT_3GPD */

    /*-------------------------------------------------------------------------
    	Read the PPP NAI ID from NV FOR ACTIVE PROFILE.
     -------------------------------------------------------------------------*/
    nv_item.ds_sip_nai_info.index=index;
    nv_status = nv_get( NV_DS_SIP_NAI_INFO_I, &nv_item );

    /*-------------------------------------------------------------------------
    	If NV read succeeds, load the user ID into the provided ppp config
     -------------------------------------------------------------------------*/
    if( nv_status == NV_DONE_S)
    {
      ppp_config->auth_info.user_id_len = nv_item.ds_sip_nai_info.nai_length;
      memcpy( ppp_config->auth_info.user_id_info,
              (char *)nv_item.ds_sip_nai_info.nai,
              ppp_config->auth_info.user_id_len );
      MSG_MED( "User id succesfully read",0 , 0, 0 );
    }
    /*-------------------------------------------------------------------------
      If NV was never written then the length of the User_id is set to 0.
      This will ensure that no user ID is included in the AP response.
    -------------------------------------------------------------------------*/
    else
    {
      /*-----------------------------------------------------------------------
        Set the user_id length to 0.
      -----------------------------------------------------------------------*/
      ppp_config->auth_info.user_id_len = 0;
      MSG_ERROR( "PPP user ID never written", 0, 0, 0 );
    }
#ifdef CUST_EDITION
    /*-------------------------------------------------------------------------
      Read the PPP User ID from NV. 
    -------------------------------------------------------------------------*/
    nv_status = nv_get( NV_PPP_USER_ID_I, &nv_item );

    /*-------------------------------------------------------------------------
      If NV read succeeds, load the user ID into the provided ppp config
    -------------------------------------------------------------------------*/
    if( nv_status == NV_DONE_S)
    {
       if(nv_item.ppp_user_id.user_id_len > 0)
       {
          ppp_config->auth_info.user_id_len = nv_item.ppp_user_id.user_id_len;
          memcpy( ppp_config->auth_info.user_id_info,
                  (char *)nv_item.ppp_user_id.user_id,
                   ppp_config->auth_info.user_id_len );
       }
       else
       {
          MSG_ERROR( "PPP user ID NV_DS_SIP_NAI_INFO_I", 0, 0, 0 );
       }
    }
#endif

    /*-------------------------------------------------------------------------
      Read the PPP password from NV. 
    -------------------------------------------------------------------------*/
    nv_item.ds_sip_ppp_ss_info.index=index;
    nv_status = nv_get(NV_DS_SIP_PPP_SS_INFO_I,&nv_item );

    /*-------------------------------------------------------------------------
      If NV read succeeded, load the ss info  into the provided ppp config
    -------------------------------------------------------------------------*/
    if( nv_status == NV_DONE_S)
    {
      ppp_config->auth_info.passwd_len = nv_item.ds_sip_ppp_ss_info.ss_length;
      memcpy( ppp_config->auth_info.passwd_info,
              (char *)nv_item.ds_sip_ppp_ss_info.ss,
              ppp_config->auth_info.passwd_len );
      MSG_MED( " password  succesfully read",0 , 0, 0 );
    }
    else
    {
      passwd_read_failed = TRUE; /* PPP Password NV was never written */
    }	
  }
  else
  {
    /*-------------------------------------------------------------------------
      Read the PPP User ID from NV. 
    -------------------------------------------------------------------------*/
    nv_status = nv_get( NV_PPP_USER_ID_I, &nv_item );

    /*-------------------------------------------------------------------------
      If NV read succeeds, load the user ID into the provided ppp config
    -------------------------------------------------------------------------*/
    if( nv_status == NV_DONE_S)
    {
      ppp_config->auth_info.user_id_len = nv_item.ppp_user_id.user_id_len;
      memcpy( ppp_config->auth_info.user_id_info,
              (char *)nv_item.ppp_user_id.user_id,
               ppp_config->auth_info.user_id_len );
    }
    /*-------------------------------------------------------------------------
      If NV was never written then the length of the User_id is set to 0.
      This will ensure that no user ID is included in the AP response.
    -------------------------------------------------------------------------*/
    else
    {
      /*-----------------------------------------------------------------------
         Set the user_id length to 0.
      -----------------------------------------------------------------------*/
      ppp_config->auth_info.user_id_len = 0;
      MSG_ERROR( "PPP user ID never written", 0, 0, 0 );
    }

    /*-------------------------------------------------------------------------
      Read the PPP password from NV. 
    -------------------------------------------------------------------------*/
    nv_status = nv_get( NV_PPP_PASSWORD_I, &nv_item );

    /*-------------------------------------------------------------------------
      If NV read succeeded, load the password into the provided ppp config
    -------------------------------------------------------------------------*/
    if( nv_status == NV_DONE_S)
    {
      ppp_config->auth_info.passwd_len = nv_item.ppp_password.password_len;
      memcpy( ppp_config->auth_info.passwd_info,
              (char *)nv_item.ppp_password.password,
              ppp_config->auth_info.passwd_len );
    }
    else
    {
      passwd_read_failed = TRUE; /* PPP Password NV was never written */
    }
  }
  
  MSG_FATAL( "is707_get_ppp_auth_info_from_nv %d %d %d",ppp_config->auth_info.user_id_info[0] , ppp_config->auth_info.user_id_info[1], ppp_config->auth_info.user_id_info[2] );
  MSG_FATAL( "is707_get_ppp_auth_info_from_nv %d %d %d",ppp_config->auth_info.user_id_info[3] , ppp_config->auth_info.user_id_info[4], ppp_config->auth_info.user_id_len );
  MSG_FATAL( "is707_get_ppp_auth_info_from_nv %d %d %d",ppp_config->auth_info.passwd_info[0] , ppp_config->auth_info.passwd_info[1], ppp_config->auth_info.passwd_info[2] );
  MSG_FATAL( "is707_get_ppp_auth_info_from_nv %d %d %d",ppp_config->auth_info.passwd_info[3] , ppp_config->auth_info.passwd_info[4], ppp_config->auth_info.passwd_len );
  /*-------------------------------------------------------------------------
    If NV was never written then the length of the password is set to 0.
    This will ensure that no password is used in calculating AP response.
  -------------------------------------------------------------------------*/
  if ( passwd_read_failed )
  {
#ifdef FEATURE_DATA_PPP_DEFAULT_PASSWD
    
    MSG_MED( "SN PPP Password not provisioned, using default", 0, 0, 0 );
    /*-----------------------------------------------------------------------
      Write the default password to NV
    -----------------------------------------------------------------------*/
    if( PPP_DEFAULT_SN_PASSWORD_LEN <= NV_MAX_PPP_PASSWORD_LENGTH )
    {
      #ifdef FEATURE_DS_MULTIPLE_PROFILES
        nv_item.ds_sip_ppp_ss_info.ss_length = PPP_DEFAULT_SN_PASSWORD_LEN;
        memcpy( (char *)nv_item.ds_sip_ppp_ss_info.ss, PPP_DEFAULT_SN_PASSWORD, 
                PPP_DEFAULT_SN_PASSWORD_LEN );
    
        nv_status = psi_put_nv_item(NV_DS_SIP_PPP_SS_INFO_I, &nv_item );
      #else
        nv_item.ppp_password.password_len = PPP_DEFAULT_SN_PASSWORD_LEN;
        memcpy( (char *)nv_item.ppp_password.password,
                PPP_DEFAULT_SN_PASSWORD,
                PPP_DEFAULT_SN_PASSWORD_LEN );
    
        nv_status = psi_put_nv_item( NV_PPP_PASSWORD_I, &nv_item );
      #endif

      if( nv_status != NV_DONE_S )
      {
        MSG_ERROR( "Failed to write default SN PPP password to NV", 0, 0, 0);
      }
    }
    else
    {    
      MSG_ERROR( "Default PPP pwd too long to fit in NV. Skipping NV Write", 
                 0, 0, 0 );
    }

    /*-----------------------------------------------------------------------
      Whether the NV write succeeded or not, set PPP config appropriately
    -----------------------------------------------------------------------*/
    if( PPP_DEFAULT_SN_PASSWORD_LEN <= PPP_MAX_PASSWD_LEN )
    {
      ppp_config->auth_info.passwd_len = PPP_DEFAULT_SN_PASSWORD_LEN;
      memcpy( ppp_config->auth_info.passwd_info,
              PPP_DEFAULT_SN_PASSWORD,
              PPP_DEFAULT_SN_PASSWORD_LEN );
    }
    else
    {
      ppp_config->auth_info.passwd_len = 0;
      MSG_ERROR( "Default PPP password too long to fit in PPP config. "
                 "SN PPP password not configured", 0, 0, 0);
    }
#else
    /*-----------------------------------------------------------------------
      Set the user_id length as 0.
    -----------------------------------------------------------------------*/
    ppp_config->auth_info.passwd_len = 0;
    MSG_ERROR( "PPP password never written", 0, 0, 0 );
#endif /* FEATURE_DATA_PPP_DEFAULT_PASSWD */
  }
} /* is707_get_ppp_auth_info_from_nv() */
  

/*===========================================================================
FUNCTION DSSNET_SET_IN_QNC_CALL()

DESCRIPTION
  This function notifies dssnet if the mobile is in a QNC call. Called by
  ds707_pkt_mgr.

DEPENDENCIES
  None.

PARAMETERS
  boolean     in_qnc_call

RETURN VALUE
  None.

SIDE EFFECTS
  None.
===========================================================================*/
void dssnet_set_in_qnc_call
(
  boolean in_qnc_call
)
{
/*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/
  dssneti_sm_cb.disable_lcp_auth = in_qnc_call;
} /* dssnet_set_in_qnc_call() */

#endif /* FEATURE_DATA_IS707 */
