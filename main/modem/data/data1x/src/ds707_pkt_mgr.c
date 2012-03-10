 /*===========================================================================

                        D S 7 0 7 _ P K T _ M G R . C
GENERAL DESCRIPTION
  This file is the controller for 1X packet calls.  It does not have any
  async/fax functionality.

  This module mainly controls the traffic channel portion of the PS_IFACE.
  This module works in conjunction with ds707_rmsm.c and ds707_dssnet.c

  This file mainly concerns itself with interacting with CM/MC, in order to
  bring up and tear down the traffic channel, as well as going into and
  coming out of dormancy.

  This module does not know if the packet call is laptop or sockets, or even
  if it is relay or network.

EXTERNALIZED FUNCTIONS
  DS707_PKT_IS_DORMANT
    Returns if the packet call is dormant.

  DS707_PKT_GET_PREV_CALL_SO
    Returns the service option of the previous packet call.

  DS707_PKT_GET_PREV_SR_ID
    Returns the SR_ID of the previous packet call.

  DS707_PKT_GET_CURR_TX_WM_FROM_RLP
    Returns a pointer to the wmk going to RLP.

  DS707_PKT_GET_CURR_TX_WM
    Returns a pointer to tx wmk from pkt state depending on call instance.

  DS707_PKT_REORIG
    Called when dormant and the RLP tx watermark goes non-empty.  Sets
    signal for DS task to re-originate out of dormancy.

  DS707_PKT_MGR_INIT
    Called at startup.  Initializes the entire 1X pkt system, including
    any supporting modules.

  DS707_PKT_CM_INIT
    Registers with CM for any 1X/707 specific functionality.

  DS707_PKT_PROCESS_CMD
    Main processing loop for any commands which are for 1x pkt.

  DS707_PKT_PROCESS_SIGNALS
    Main processing loop for any signals set for 1x pkt.

  DS707_PKT_GET_IFACE_PTR
    Returns pointer to the 1x pkt ps_iface.

  DS707_PKT_SETUP_RLP
    Called by ds707_rmsm when starting a call.  If in a laptop call, then
    a different set of watermarks are registered with RLP than if in either
    a network or sockets model call.

  DS_VERIFY_SO
    Called by MC before making a packet data call.  Required because system
    P_REV can change between the the origination request to CM and the actual
    request going out over the air.

  DS707_PKT_GET_ACTIVE_IFACE_PTR
    Returns a pointer to the iface, if it is active.  If not active, then
    returns NULL.

  DS707_PKT_SETUP_SN_PPP
    Called when the pkt iface is coming up, to initialize PPP.

  DS707_PKT_RX_UM_SN_DATA
    Called by PPP to pull data from RLP fwd link watermark.

  DS707_PKT_TX_UM_SN_DATA
    Called by PPP to put data into RLP rev link watermark.

  DS707_PKT_ENABLE_HOLDDOWN_TIMER
    Re-starts the dormant(holddown) timer.

INITIALIZATION AND SEQUENCING REQUIREMENTS
  ds707_pkt_mgr_init() must be called at startup.

  Copyright (c) 2002-2010 by QUALCOMM Incorporated.  All Rights Reserved.
===========================================================================*/

/*===========================================================================
                           EDIT HISTORY FOR MODULE

  $PVCSPath: L:/src/asw/MM_DATA/vcs/ds707_pkt_mgr.c_v   1.45   25 Feb 2003 14:25:02   ajithp  $
  $Header: //source/qcom/qct/modem/data/1x/707/main/lite/src/ds707_pkt_mgr.c#15 $ $DateTime: 2010/01/28 08:30:33 $ $Author: parmjeet $

when        who    what, where, why
--------    ---    ----------------------------------------------------------
19/01/10    ps     Added code changes to reset requesting and current
                   profile IDs when IFACE is brought down.
05/30/09    ms     Added Support for EPZID Hysteresis.
04/29/09    sn     Ported support for call throttle feature (DCTM).
01/28/09    ss     Klocwork Fix.
01/23/09    sn     Fixed featurization issues.
11/26/08    ms     Pass the new EPZID to process change handler, donot modify
                   the current one.
07/28/08    psng   Removed call to function ds707_drs_init, as we are not
                   caching NV_DS_MIP_QC_DRS_OPT_I at power up.
07/11/08    sn     Changed FEATURE_DS_SIP_MULTIPLE_PROFILE to 
                   FEATURE_DS_MULTIPLE_PROFILES.
06/24/08    sn     Modified to use updated data session function/variable names.
06/17/08    ms     Compiler warning fix
03/10/08    psng   Added support for Arbiration manager.
02/25/08    sn     Fixed lint error.
02/07/08    sn     Removed FEATURE_SPECIAL_MDR.
10/29/07    sn     Replaced banned string APIs.
10/18/07    sn     Added support for pzid hysteresis under feature 
                   FEATURE_DS_PZID_HYSTERESIS.
07/23/07    mga    Made changes to use boolean dial_str_digits_mode
11/01/06    an     Featurize ATCOP, RMSM, Async Fax
08/30/05    gr     Fix to handle the race condition of simultanneous RLP 
                   init and cleanup while dropping voice during SVD scenario
10/27/04    gr     Added PZID_HYS_CALL_CONNECTED, CALL_ENDED and GO_NULL
                   events to stop/start Hysteresis Activation Timer. HAT
                   now runs independent of HOLDDOWN_TIMER
09/08/04    vr     Added the functionality to register with RLP3 the callback
                   function to notify the Hysteresis engine when PPP data is 
                   exchanged for the first time. It is deregistered when the
                   call is torn down/when the first PPP data exchange happens                    
07/12/04    gr     Added IOCTLs for getting and setting the RLP
                   fill rate
05/12/04    sy     Added support for pzid hysteresis under feature 
                   FEATURE_DS_PZID_HYSTERESIS.
04/22/04    kvd    On call_connect, check for ps_iface state(!=DOWN|DISABLED)
                   before accepting the call, else terminate the call.
11/07/03    ak     Added ds707_pkt_trash_rlp_tx_q api to flush the outgoing
                   RLP queue.  Used by higher layers when reconfiguring link.
11/07/03    aku    Replaced call to DSSNET_PPP_ABORT_CMD with an abort_f_ptr 
                   which would be registered by dssnet or rmsm. 
10/28/03    ak     Fixes for JCDMA service options.  On incoming call, see
                   if packet allowed.  When transitioning in and out of
                   dormancy, now update SO information.
10/01/03    ak     Added PZID delay timer processing.
09/15/03    ak     When holddown timer expires, see if PZID needs to reorig.
09/10/03    ak     Fixed typo when retrieving data counts - retrieve 1x 
                   instance instead of HDR instance.
08/25/03    ak     Fixes to register and deregister PZID, SID, and NID when
                   IFACE state changes.
08/25/03    ak     Fix for retry delays in Network Model & Sockets mode.
08/21/03    sy     Mainlined the code where CM is always informed about the
                   DS call state(earlier it was under jcdma feature).
07/31/03    atp    SDB flags moved to new header file.
07/08/03    atp    Set drs bit 1 always when GoActive is called.
07/07/03    atp    SDB enhancements to pass status notification to app.
07/02/03    ak     Reset dormant/holddown timer whenever going NULL.
06/12/03    rtp    Added back clk frequency changing during data calls.
06/11/03    ak     Add new func to see if ok to orig from dormancy.  Mod of
                   6/2 fix below.
06/02/03    ak     Now can be dormant when IFACE = GOING_DOWN.
05/21/03    vas    Added support for HDR calls.
05/02/03    usb    ps_iface_create() prototype change.
05/02/02    ak     Added function to get last pkt so.  Made prev_so a 16-bit
                   as we allow 16-bit SO's.
04/24/03    atp    Removed clk frequency changing during data calls.
                   Functionality moved to drivers.
04/18/03    ak     Added cmd_ptr param to switch_calls_hdlr interface.  This
                   allows us to handle bypass_alert = TRUE in this situation.
04/11/03    ak     Updates for new RMSM interfaces.
03/31/03    atp    Added error checking for null argument ptrs.
03/20/03    atp    Bug fix in sdb query ioctl.
03/10/03    atp    Added check for race condition in simultaneous incoming/
                   outgoing call scenario.
03/06/03    atp    Added call to register and deregister for clock frequency
                   changing when a data call is started and ended
02/25/03    atp    Added ioctl for enabling/disabling holddown timer for 707.
02/18/03    rsl    Changes relating to cm end_params.
02/12/03    atp    Changed error codes to correspond to dserrno.h rather
                   than errno.h.
02/07/03    usb    Reset snpzid_change to FALSE at the end of the call.
02/06/03    atp    Added SDB support query ioctl for 707.
02/06/03    atp    Moved functionality of dsiface (w.r.t 707 - RLP NAK policy
                   dorm timer, MDR, QOS) to ioctls. dsiface to be deprecated.
02/06/03    jd     renamed ppp_auth_get_info() to ppp_auth_get_from_nv()
01/31/03    jd     Load authentication info for mobile internal CHAP
                   when starting Um PPP
01/31/03    atp    Added GoNull ioctl for 707.
01/29/03    atp    Modified GoActive and GoDormant ioctls.
01/23/03    atp    Added handlers for returning RLP stats, connection info.
01/23/03    usb    Changed Um PPP config for network model SIP calls to
                   set HDLC mode to SW only.
01/22/03    ak     Added event reporting for holddown timer and CTA expiring.
01/21/03    ak     Allocate sr_id here in init function.  Retry delay is
                   now blanket across all SO's.
01/17/03    atp    Added support for 707 ioctls.
01/09/02    ak     For JCDMA, reject incoming pkt so if unknown SO's.
12/30/02    sy     Added changes to support allocation of SR_ID by DS
                   during power-up time.
12/18/02    ak     No longer register for p_rev events on cm_init(), as that
                   clobbered 3gDSMGR's registration with CM for SS events
                   that included changes back and forth between GSM & CDMA.
12/13/02    ak     On incoming calls, need to clear each_enqueue_ptrs on
                   RLP wmk's.
12/12/02    ak     When answering an incoming call, set info_type to CDMA
11/27/02    atp    Added ds707_pkt_get_curr_tx_wm().
11/25/02    usb    Added support for initiating PPP resync on snpzid change
                   reconnect using DRS algorithm.
11/18/02    ak     Updated file header commments.
11/15/02    ak     For JCDMA, reject incoming calls with wrong SO's(4103 and
                   x8020).
11/06/02    ak     Code for extra param in call_ended_hdlr.
10/29/02    atp    Added support for SDB.
10/16/02    ak     Updated to use changed func names in jcdma_m51.c.  Now
                   inits JCDMA M51 and Apprate modules.
10/15/02    ak     Updated for 3G DSMGR handler table.
10/15/02    ak     Updated FEATURE_JCDMA_DS to FEATURE_JCDMA_1X
10/14/02    ak     For JCDMA, if call ends and reason is CLIENT_END and
                   client_id != data, then go NULL.
10/13/02    ak     For JCDMA: tell CM pkt state; on pkt calls, dial_str=NULL,
                   on going NULL, set dorm_timer back to default.
10/01/02    ak     On toggle QNC command, actually toggle qnc value.
09/30/02    ar     Accomodate changed interface to proc_dial_string
09/24/02    atp    Added support for QOS. (1x Release-A feature).
09/24/02    usb    Added user_data to ppp tx and rx function callbacks
09/03/02    ak     Updated for ps_iface cback buf interface.
08/30/02    om     Added early routing and default route insert / delete.
08/20/02    ak     Fixes force_call_down().  Fix for not re-starting holddown
                   timer when orig fails.  process_cmds() no longer assumes
                   pkt_instance is in cmd_ptr.
08/19/02    ak     Updated for multiple calls.
08/13/02    mvl    updated ps_iface state macros for name change
08/09/02    ak     Featurize QOS stuff under IS2000_REL_A
08/02/02    ak     before phys_link_up_ind(), tell dssnet if QNC call.
07/29/02    ak     PZID/SID/NID now ignore holddown timer.
07/31/02    aku    Externalized function ds707_pkt_is_dormant().
07/29/02    ak     Do not assert when call_ended with unknown call_id.  Could
                   happen with simultaneous page and origination.
07/29/02    ak     When starting holddown timer, print out a msg.
07/25/02    mvl    Removed netmdl dependency.  Updates for PPP renaming.
07/24/02    ak     Implementation for enabling/disabling holddown timer.
07/24/02    usb    Handling errors from PPP configuration
07/24/02    ak     When orig'ing, it is possible that we have bad SO.
07/23/02    ak     some re-ordering in call-ended handler.
07/23/02    aku    Notify dssnet to abort PPP in
                   ds707_pkti_call_ended_hdlr()
07/23/02    ak     On iface_down_cback, cleanup the default wmks
07/22/02    ak     Dormancy condition includes "COMING_UP"
07/22/02    ak     iface_down_cback now sends a msg to DS, to make sure
                   in DS task.
07/21/02    na     Removed ASSERT from pkti_iface_down_cback
07/16/02    ak     Removed NET_MODEL featurization.  Changed names of
                   exported funcs from 'pkti' -> 'pkt'.  Alway send cmd
                   on iface cmds.  Function to return active iface.
07/15/02    aku    Added support for sockets.
07/14/02    ak     Lots of clean-up to iface_down and phys_link down
                   logic.
07/13/02    atp    Removed async and fax srvc option initialization
                   from ds707_pkt_mgr_init().
07/12/02    usb    Modified ds707_pkt_setup_SN_ppp() based upon new PPP
                   interface.
07/03/02    ak     Change errno's to ps_iface funcs from uint16 -> int16.
07/03/02    ak     Updated ppp names as they have changed.
07/01/02    ak     De-register each_enqueue_f_ptrs from wm when origing
                   a call.  Also, do not have to netmdl_set_state() any
                   more.
07/01/02    ak     Various fixes for first data call.
05/10/02    ak     Added extern for signal_processing().  Fixed typo.
                   Init RLP's at startup.
02/20/02    ak     First version of file.
===========================================================================*/


/*===========================================================================
                          INCLUDE FILES FOR MODULE
===========================================================================*/

#include "comdef.h"
#include "customer.h"

#ifdef FEATURE_DATA_IS707
#include "AEEstd.h"
#include "cm.h"

#ifdef FEATURE_IS95B_MDR
  #include "mdrrlp.h"
#else
  #include "rlp.h"
#endif

#include "ds_flow_control.h"

#include "ds3gmgr.h"
#include "ds3gmshif.h"
#include "ds3gtimer.h"

#include "ds707.h"
#include "ds707_cta_rlp.h"
#include "ds707_diag.h"
#include "ds707_dorm_timer.h"
#include "ds707_drs.h"
#include "ds707_nid.h"
#include "ds707_p_rev.h"
#include "ds707_pkt_mgr.h"
#ifdef FEATURE_DS_PZID_HYSTERESIS
#include "ds707_pzid.h"
#else
#include "ds707_epzid.h"
#endif /* FEATURE_DS_PZID_HYSTERESIS */
#include "ds707_rrm.h"
#ifdef FEATURE_DATA_IS2000_SDB
#include "ds707_sdb.h"
#include "mccdma.h"
#endif /* FEATURE_DATA_IS2000_SDB */
#ifndef FEATURE_DATA_STRIP_ATCOP
#include "ds707_so_async.h"
#include "ds707_rmsm.h"
#include "dsat.h"
#include "dsat707util.h"
#include "dsat707extctab.h"
#include "dsat707vend.h"
#include "dsat707vendctab.h"
#include "ds707_so_g3fax.h"
#else
#include "dsatprofile.h"
#endif
#include "ds707_sid.h"
#include "ds707_so33.h"
#include "ds707_so_pkt.h"
#include "ds707_so_retrydel.h"
#include "ds707_timer.h"
#include "ds707_wmk.h"
#include "dsrlp.h"
#include "dsrlpi.h"
#include "dserrno.h"
#include "dss_iface_ioctl.h"
#include "dssocket_defs.h"
#include "dstask.h"
#include "dstaski.h"

#include "ps_aclrules.h"
#include "ds707_rt_acl.h"
#include "ps_route.h"

#include "ps_iface.h"
#include "ps707_dssnet.h"
#include "ps_ppp.h"
#include "ps_ppp_defs.h"
#include "ps_ppp_auth.h"
#include "ps_svc.h"

#include "err.h"
#include "msg.h"
#include "event.h"

#include "queue.h"
#include "rex.h"
#include "mccsrid.h"
#include "mccsyobj.h"


#ifdef FEATURE_JCDMA_1X
#include "ds3gmgrint.h"
#include "ds707_jcdma_m51.h"
#include "ds707_jcdma_apprate.h"
#include "ds707_so_jcdma.h"
#endif

#ifdef FEATURE_HDR
#error code not present
#endif /* FEATURE_HDR */
#ifdef FEATURE_DYNAMIC_CLOCK_FREQUENCY_CHANGING
#include "clkregim.h"
#endif /* FEATURE_DYNAMIC_CLOCK_FREQUENCY_CHANGING */
#ifdef FEATURE_DS_PZID_HYSTERESIS
#include "ds707_pzid_hyst.h"
#else
#include "ds707_epzid_hyst.h"
#endif /* FEATURE_DS_PZID_HYSTERESIS */

#ifdef FEATURE_CALL_THROTTLE
#include "dsdctm.h"
#endif /* FEATURE_CALL_THROTTLE */

#ifdef FEATURE_DS_MULTIPLE_PROFILES
#include "ds707_data_session_profile.h"
#include "dstaski.h"
#endif

extern boolean	bIsPPPAuthEnabled; //Add By zzg 2012_03_07

/*===========================================================================
                           FORWARD DECLARATIONS
===========================================================================*/
LOCAL boolean ds707_pkti_orig_hdlr
(
  const byte                           *dial_string,
        uint8                           dial_string_len,
        cm_cdma_orig_params_s_type     *cdma_orig_params_ptr,
        boolean                        *cdma_params_changed,
        cm_gw_cs_orig_params_s_type    *gw_cs_orig_params_ptr,
        boolean                        *gw_cs_params_changed,
        cm_gw_ps_orig_params_s_type    *gw_ps_orig_params_ptr,
        boolean                        *gw_ps_params_changed,
        cm_srv_type_e_type             *cm_srv_type,
        void                           *mode_spec_info_ptr
);

LOCAL void ds707_pkti_call_id_hdlr
(
  cm_call_id_type        cm_call_id,
  void                  *mode_spec_info_ptr
);

LOCAL boolean ds707_pkti_call_conn_hdlr
(
  cm_call_id_type           call_id,
  cm_call_mode_info_s_type  *mode_info_ptr,
  boolean                   *delay_connect,
  cm_end_params_s_type      *end_params
);

LOCAL ds3g_incom_e_type ds707_pkti_inc_call_hdlr
(
  ds_cmd_type            *cmd_ptr,
  cm_call_id_type         cm_call_id,
  cm_ans_params_s_type   *ans_params, /* Answer params sent to Call Mgr    */
  cm_end_params_s_type   *end_params  /* end params if call rejected       */
);

LOCAL void ds707_pkti_end_call_hdlr
(
  cm_call_id_type        call_id,
  cm_end_params_s_type  *end_params,
  boolean               *delay_disconnect,
  void                  *mode_spec_info_ptr
);

LOCAL void ds707_pkti_call_ended_hdlr
(
  ds_cmd_type            *cmd_ptr,
  boolean                 call_was_connected
);

LOCAL ds3g_incom_e_type ds707_pkti_switch_calls_hdlr
(
  ds_cmd_type           *cmd_ptr,
  cm_call_id_type        inc_call_id,
  cm_call_id_type        exist_call_id,
  cm_ans_params_s_type  *ans_params,  /* parms for call that is accepted   */
  cm_end_params_s_type  *end_params_orig, /* end parms for orig call       */
  cm_end_params_s_type  *end_params_inc   /* end parms for inc call        */
);

LOCAL boolean ds707_pkti_force_call_down_hdlr
(
  cm_call_id_type        exist_call_id,
  cm_end_params_s_type  *end_params_ptr
);

LOCAL boolean ds707_pkti_get_call_conn_info_hdlr
(
  cm_call_id_type     call_id,
  byte               *system_information,
  byte                max_system_information_length
);

LOCAL boolean  ds707_pkti_get_data_count_hdlr
(
  cm_call_id_type    call_id,
  uint32            *rx_from_network_byte_cnt,
  uint32            *tx_to_network_byte_cnt
);

LOCAL boolean  ds707_pkti_get_last_sess_count_hdlr
(
  uint32            *rx_from_network_byte_cnt,
  uint32            *tx_to_network_byte_cnt
);

LOCAL void ds707_pkti_um_rlp_fwd_func(void);

LOCAL int ds707_pkt_ioctl
(
  ps_iface_type            *ps_iface_ptr,
  dss_iface_ioctl_type     ioctl_name,
  void                     *argval_ptr,
  sint15                   *ps_errno
);

boolean ds707_pkt_is_um_iface_up
(
  ps_iface_type     *ps_iface_ptr
);


/*===========================================================================
                            TYPEDEFS
===========================================================================*/

#define DSRLPI_MAX_SRID_INDEX DSRLPI_MAX_RSCB_ELEMENTS


/*===========================================================================
                            VARIABLES
===========================================================================*/

/*---------------------------------------------------------------------------
  Flag which indicates if the holddown timer should be used for the next
  dormant session.  This is reset back to TRUE on every origination.  If
  TRUE, then holddown timer is used on next dormant session.  If FALSE,
  holddown timer is not used on next dormant session.
---------------------------------------------------------------------------*/
LOCAL boolean enable_holddown_timer = TRUE;

/*---------------------------------------------------------------------------
  Packet call states.  Each instance encapsulates one traffic channel call
  and it's associate IFACE.
---------------------------------------------------------------------------*/
 ds707_pkt_state_type            ds707_pkt_state[DS707_MAX_PKT_CALLS];

/*---------------------------------------------------------------------------
   Structure to register with 3G DSMGR.  Has all the callbacks for this
   mode-specific handler
---------------------------------------------------------------------------*/
LOCAL ds3g_hdlr_func_tbl_type ds707_pkt_mgr_hdlr_func_tbl =
{
  ds707_pkti_orig_hdlr,           /* call origination handler          */
  ds707_pkti_call_id_hdlr,        /* call id assignment handler        */
  NULL,                           /* CALL_CONF handler                 */
  ds707_pkti_call_conn_hdlr,      /* CALL_CONNECTED handler            */
  NULL,                           /* RAB REL HDLR                      */
  NULL,                           /* RAB EST HDLR                      */
  NULL,                           /* RAB EST REJ HDLR                  */
  NULL,                           /* RAB EST FAIL HDLR                 */
  NULL,                           /* DS COMPLETE_LL_CONN HDLR          */
  NULL,                           /* setup handler                     */
  ds707_pkti_inc_call_hdlr,       /* incoming call handler             */
  NULL,                           /* user or TE answers call handler   */
  ds707_pkti_end_call_hdlr,       /* user or TE ends the call          */
  NULL,                           /* DS COMPLETE LL DISCONN HDLR       */
  ds707_pkti_call_ended_hdlr,     /* CALL_ENDED handler                */
  ds707_pkti_switch_calls_hdlr,   /* switch calls handler              */
  ds707_pkti_force_call_down_hdlr,/* force call down                   */
  ds707_pkti_get_call_conn_info_hdlr,/* get call conn info hdlr        */
  ds707_pkti_get_data_count_hdlr,    /* get data counters info         */
  ds707_pkti_get_last_sess_count_hdlr/* get last call counters info    */
};

/*---------------------------------------------------------------------------
  This information is used in dormant re-origination state. So before
  originating a call, we want to  to know service option of the call before
  it went dormant.
---------------------------------------------------------------------------*/
LOCAL uint16 ds707_pkt_prev_data_call_so;
LOCAL byte ds707_pkt_prev_sr_id;

extern uint8 dsrlp_last_fill_nak_gap;

/*===========================================================================
                       INTERNAL MACRO DEFINITIONS
===========================================================================*/
/*===========================================================================
MACRO         VERIFY_PKT_INSTANCE

DESCRIPTION   Makes sure the packet instance is an okay value.

DEPENDENCIES  None.

RETURN VALUE  None.  Will ERR_FATAL if the value is no good.

SIDE EFFECTS  None.
===========================================================================*/
#define VERIFY_PKT_INSTANCE(x) ASSERT(((uint32)x) < DS707_MAX_PKT_CALLS);

/*===========================================================================
MACRO         GET_PKT_STATE_FROM_CALL_INFO

DESCRIPTION   Given the call_info_ptr, which is often passed to this
              module from 3G DSMGR, extracts out the pkt state instance.

DEPENDENCIES  The input 'x' is assumed to be of type:
                ds707_pkt_call_info_type

RETURN VALUE  Returns the packet instance ID, which is the index into the
              array of pkt_staes.

SIDE EFFECTS  None.
===========================================================================*/
#define GET_PKT_INST_FROM_CALL_INFO(x)\
 (uint32)(((ds707_pkt_call_info_type *)x)->pkt_instance)


/*===========================================================================
MACRO         GET_TC_STATE_PTR

DESCRIPTION   Given the pkt instance, returns the tc_state_ptr.

DEPENDENCIES  None.

RETURN VALUE  See description.

SIDE EFFECTS  None.
===========================================================================*/
#define GET_TC_STATE_PTR(x) &(ds707_pkt_state[x].tc_state)

/*===========================================================================
MACRO         GET_ACL_PTR

DESCRIPTION   Given the pkt instance, returns the ACL pointer.

DEPENDENCIES  None.

RETURN VALUE  See description.

SIDE EFFECTS  None.
===========================================================================*/
#define GET_ACL_PTR(x) &(ds707_pkt_state[x].acl)


/*===========================================================================
MACRO         GET_SNPZID_CHANGE_FLAG_PTR

DESCRIPTION   Given the pkt instance, returns the pointer to flag that tells
              whether reconnected due a change in SID/NID/PZID

DEPENDENCIES  None.

RETURN VALUE  See description.

SIDE EFFECTS  None.
===========================================================================*/
#define GET_SNPZID_CHANGE_FLAG_PTR(x) &(ds707_pkt_state[x].snpzid_change)


/*===========================================================================
MACRO         GET_ABORT_F_PTR

DESCRIPTION   Given the pkt instance, returns the abort function ptr.

DEPENDENCIES  None.

RETURN VALUE  See description.

SIDE EFFECTS  None.
===========================================================================*/
#define GET_ABORT_F_PTR(x) (ds707_pkt_state[x].abort_f_ptr)



///////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////

ds707_epzid_type ds707_pzid_curr_epzid;
/*===========================================================================
FUNCTION      DS707_PROCESS CHANGED_EPZID

DESCRIPTION   This function is the callback registered with mccsyobj_system,
              that gets called when a Pkt Zone ID change has been detected
              while the mobile is idle or active.  Mobile could be active
              in either a Vox call (and could be either SVD enabled or not)
              or could be active in any type of data call.
       
              This function follows procedures from IS-707.A.5 sec.2.2.7. 
              If the new PZID was 0, it resets PZID based reconnection.
              It adds the new PZID to the visited PZID list.
              If the new PZID is not on the list of the PZIDs, this function
              sends a command to DS task to originate a packet call.
                                   
              This function is called from the context of the MC task.
  
DEPENDENCIES  None

RETURN VALUE  None

SIDE EFFECTS  None
===========================================================================*/
LOCAL void ds707_process_changed_epzid
(
  
  ds707_epzid_type *new_epzid_ptr         /* New Enhanced Pkt Zone ID      */
)
{
  ds707_epzid_type curr_epzid;
/* - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - */
  
  if(new_epzid_ptr == NULL)
  {
  	MSG_ERROR("Invalid pointer passed: %x", new_epzid_ptr,0,0);
    ASSERT(0);
	  return;
  }
  
  curr_epzid.sid = ds707_pzid_curr_epzid.sid;
  curr_epzid.nid = ds707_pzid_curr_epzid.nid;
  curr_epzid.pzid= ds707_pzid_curr_epzid.pzid;

  #ifdef FEATURE_DATA_EPZID
  ds707_epzid_changed_cb(&curr_epzid, new_epzid_ptr);
  #else
  ds707_sid_change_handler (curr_epzid.sid, new_epzid_ptr->sid );
  ds707_nid_change_handler (curr_epzid.nid,new_epzid_ptr->nid);
  ds707_pzid_change_cb(curr_epzid.pzid, new_epzid_ptr->pzid);
  #endif /* FEATURE_DATA_EPZID */
   /*---------------------------------------------------------------------
  First save the Input EPZID for comparison during the next EPZID change. 
  ------------------------------------------------------------------------*/
  ds707_pzid_curr_epzid.sid = new_epzid_ptr->sid;
  ds707_pzid_curr_epzid.nid = new_epzid_ptr->nid;
  ds707_pzid_curr_epzid.pzid = new_epzid_ptr->pzid;

} /* epzidi_process_changed_epzid() */



/*===========================================================================
FUNCTION      EPZIDI_CHANGE_CB

DESCRIPTION   This function is the callback registered with mccsyobj_system,
              that gets called when a Pkt Zone ID change has been detected
              while the mobile is idle or active.  Mobile could be active
              in either a Vox call (and could be either SVD enabled or not)
              or could be active in any type of data call.
              
              Sends a command to DS, so that actual processing is done in 
              DS task context.
              
              This function is called from the context of the MC task.
  
DEPENDENCIES  None

RETURN VALUE  None

SIDE EFFECTS  None
===========================================================================*/
void ds707_epzid_change_cb
(
  word sid,                         /* System   Identifier */
  word nid,                         /* Network  Identifier */
  byte pzid                         /* Pkt Zone Identifier */
)
{
  ds707_epzid_type new_epzid;
/*-----------------------------------------------------------------------*/
  new_epzid.pzid  =  pzid;
  new_epzid.sid  =  sid; 
  new_epzid.nid  =  nid;
  
  ds707_process_changed_epzid( &new_epzid );


} /* epzidi_change_cb() */

/*===========================================================================
FUNCTION      DS707_EPZID_REGISTER

DESCRIPTION   This function register's the EPZID change call back with MC

DEPENDENCIES  None.

RETURN VALUE  None

SIDE EFFECTS  None
===========================================================================*/
LOCAL void ds707_epzid_register 
(
  void
)
{
  ds707_epzid_type curr_epzid;
/*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  /*-----------------------------------------------------------------------
    Register EPZID change handler with mccsyobj_system
  -----------------------------------------------------------------------*/
  mccsyobj_reg_epzid_change_handler(ds707_epzid_change_cb);
  

 /*-----------------------------------------------------------------------
   It's possible the registration happened after the EPZID change callback
   would have occured (meaning we don't get a callback for the current
   EPZID).  So make sure current PZID is in the list
 -----------------------------------------------------------------------*/
  mccsyobj_get_epzid( &curr_epzid.sid, &curr_epzid.nid, &curr_epzid.pzid );

  /*---------------------------------------------------------------
  First save Save the Input EPZID for comparison 
  during the next EPZID change. 
  ---------------------------------------------------------------*/
  ds707_pzid_curr_epzid.sid = curr_epzid.sid ;
  ds707_pzid_curr_epzid.nid = curr_epzid.nid ;
  ds707_pzid_curr_epzid.pzid = curr_epzid.pzid ;
#ifdef FEATURE_DATA_EPZID
  ds707_epzid_reg_recon();
#else
  ds707_pzid_reg_recon();
#endif /* FEATURE_DATA_EPZID */
} /* epzidi_register */

/*===========================================================================
FUNCTION      DS707_EPZID_MGR_DEREG_RECON

DESCRIPTION   This function deregisters PZID based reconnection for packet
              calls.  It sets the PZID change handlers to NULL and resets
              the visited PZID list.
   
              This function is called when (see sec.2.2.7 of IS-707.A.5):
              (a) A packet call (sockets/relay/network) is ended (packet
              service goes into Inactive state) - not called when packet data
              service goes dormant.
   
              This function can be called from the DS task

DEPENDENCIES  None

RETURN VALUE  None

SIDE EFFECTS  None
===========================================================================*/
void  ds707_epzid_dereg_recon
(
  void
)
{
/*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  MSG_MED(" Stopping  EPZID based reconnection", 0, 0, 0);
  /*-------------------------------------------------------------------------
    Deregister PZID change handler with mccsyobj_system
  -------------------------------------------------------------------------*/
  mccsyobj_reg_epzid_change_handler( NULL);

  ds707_pzid_dereg_recon();
}  /* epzidi_dereg_recon() */

 

// ////////////////////////////////////////////////////////////////////////////
/*===========================================================================
                       INTERNAL FUNCTION DEFINITIONS
===========================================================================*/
/*===========================================================================
FUNCTION      DS707_PKTI_GET_PKT_STATE_INDEX_CID


DESCRIPTION   Given a CM call id, will get the pkt state index.


DEPENDENCIES  None


RETURN VALUE  If found, will return the index.  If not found, then will
              return DS707_MAX_PKT_CALLS.


SIDE EFFECTS  None.
===========================================================================*/
LOCAL uint32 ds707_pkti_get_pkt_state_index_cid
(
  cm_call_id_type       call_id
)
{
  int                    i;
  ds707_tc_state_type   *tc_state_ptr;
/*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/
  for (i = 0; i < DS707_MAX_PKT_CALLS; i++)
  {
    tc_state_ptr = GET_TC_STATE_PTR(i);
    if (tc_state_ptr->call_id == call_id)
    {
      return(i);
    }
  }


  return(DS707_MAX_PKT_CALLS);

} /* ds707_pkti_get_pkt_state_index_cid() */


/*===========================================================================
FUNCTION      DS707_PKTI_OK_ORIG_FROM_DORM

DESCRIPTION   Returns TRUE if it is okay to originate out of dormant state.

              This is different than being dormant, as we allow MS to orig
              in the IFACE_GOING_DOWN state, but don't accept incoming calls
              in that state.

DEPENDENCIES  None.

RETURN VALUE  See description.

SIDE EFFECTS  None.
===========================================================================*/
boolean ds707_pkti_ok_orig_from_dorm
(
  ps_iface_type      *iface_ptr
)
{
  if (ps_iface_phys_link_state(iface_ptr) == PHYS_LINK_DOWN)
  {
    if ((ps_iface_state(iface_ptr) == IFACE_UP)        ||
        (ps_iface_state(iface_ptr) == IFACE_ROUTEABLE) ||
        (ps_iface_state(iface_ptr) == IFACE_COMING_UP) ||
        (ps_iface_state(iface_ptr) == IFACE_GOING_DOWN)
       )
    {
      return(TRUE);
    }
  }
  return(FALSE);
} /* ds707_pkti_ok_orig_from_dorm() */

/*===========================================================================
FUNCTION      DS707_PKTI_USE_QNC

DESCRIPTION   Returns TRUE if QNC is enabled and should be used.

DEPENDENCIES  None.

RETURN VALUE  TRUE - use QNC SO's.  FALSE - use regular PKT SO's.

SIDE EFFECTS  None.
===========================================================================*/
LOCAL boolean ds707_pkti_use_qnc(void)
{
 /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/
  #if !defined(FEATURE_DATA_STRIP_ATCOP) && defined(FEATURE_DS_QNC)
  word    p_rev_index;                  /* holds current sys's p_rev index */
  p_rev_index = ds707_p_rev_get_index();

  if ( (dsat707_qcqnc_val == TRUE)                      &&
       ( ( (p_rev_index == DS707_P_REV_3)               &&
           (dsat707_qcmdr_val <= DS_MDR_MODE_SO33_PREF)
         )                                                ||
         ( (dsat707_qcmdr_val == DS_MDR_MODE_NO_MDR)     &&
           ( (p_rev_index == DS707_P_REV_5) ||
             (p_rev_index == DS707_P_REV_6)
           )
         )
       )
     )
  {
    return TRUE;
  }
  else
  #endif /* !FEATURE_DATA_STRIP_ATCOP && FEATURE_DS_QNC */
  {
    return FALSE;
  }
} /* ds707_pkti_use_qnc() */

/*===========================================================================
FUNCTION      DS707_PKTI_GET_STATE_PTRS_CID

DESCRIPTION   Given a CM call id, will see if a tc_state and iface_state
              correspond.  If it does, then the input pointers are changed to
              the found values.  Else the values are left unchanged.

DEPENDENCIES  None

RETURN VALUE  None

SIDE EFFECTS  None.
===========================================================================*/
LOCAL void ds707_pkti_get_state_ptrs_cid
(
  cm_call_id_type       call_id,
  ds707_tc_state_type       **tc_state_ptr_ptr,
  ps_iface_type       **iface_ptr_ptr
)
{
  int                  i;
  ds707_tc_state_type       *tc_state_ptr;
  ps_iface_type       *iface_ptr;
/*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/
  for (i = 0; i < DS707_MAX_PKT_CALLS; i++)
  {
    tc_state_ptr = GET_TC_STATE_PTR(i);
    iface_ptr    = GET_PS_IFACE_PTR(i);
    if (tc_state_ptr->call_id == call_id)
    {
      *tc_state_ptr_ptr = tc_state_ptr;
      *iface_ptr_ptr    = iface_ptr;
      return;
    }
  }
} /* ds707_pkti_get_state_ptrs_cid() */

/*===========================================================================
FUNCTION      DS707_PKTI_CAN_GO_DORMANT

DESCRIPTION   Returns TRUE if iface can go dormant.  Else returns FALSE

DEPENDENCIES  None.

RETURN VALUE  See description.

SIDE EFFECTS  None.
===========================================================================*/
LOCAL boolean ds707_pkti_can_go_dormant
(
  ds707_tc_state_type      *tc_state_ptr,
  ps_iface_type      *iface_ptr
)
{
  if (((ps_iface_state(iface_ptr) == IFACE_UP)        ||
       (ps_iface_state(iface_ptr) == IFACE_ROUTEABLE) ||
       (ps_iface_state(iface_ptr) == IFACE_COMING_UP)
      )
      #ifndef FEATURE_ASYNC_DATA_NOOP
                        &&
      (ds707_so_async_so_is_async(tc_state_ptr->so) == FALSE)
      #endif
     )
  {
    return(TRUE);
  }
  else
  {
    return(FALSE);
  }
} /* ds707_pkti_can_go_dormant() */

/*===========================================================================
FUNCTION      DS707_PKTI_SET_RLP_DEFAULTS

DESCRIPTION   Sets up the pkt_state_info with the default RLP TX and RX
              settings.

DEPENDENCIES  None.

RETURN VALUE  See description.

SIDE EFFECTS  None.
===========================================================================*/
LOCAL void ds707_pkti_set_rlp_defaults
(
  ds707_tc_state_type   *tc_state_ptr
)
{
/* - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - */
  tc_state_ptr->post_rx_q_ptr    = &ds707_wmk_um_SN_fwd_q;
  tc_state_ptr->tx_watermark_ptr = &ds707_wmk_um_SN_rev_wmk;
  tc_state_ptr->post_rx_func_ptr = ds707_pkti_um_rlp_fwd_func;
} /* ds707_pkti_set_rlp_defaults() */

/*===========================================================================
FUNCTION      DS707_PKTI_DEREG_WMK_REORIG_CBACKS

DESCRIPTION   Called when need to de-register the RLP watermark callbacks
              which can trigger us out of dormancy.  Called when either
              we originate, get an incoming call, or connect, or go NULL.

DEPENDENCIES  None.

RETURN VALUE  None.

SIDE EFFECTS  None.
===========================================================================*/
LOCAL void ds707_pkti_dereg_wmk_reorig_cbacks
(
  ds707_tc_state_type   *tc_state_ptr
)
{
/* - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - */
  /*-------------------------------------------------------------------------
    Set the each_enqueue wmk cbacks to NULL, which will prevent dormant
    reoriginations when data goes to RLP.  Clear signal in-case incoming
    data is showing up at the same time...
  -------------------------------------------------------------------------*/
  INTLOCK();
  TASKLOCK();
  tc_state_ptr->tx_watermark_ptr->each_enqueue_func_ptr = NULL;
  rex_clr_sigs(&ds_tcb, DS_PKT_REORIG_SIG);
  TASKFREE();
  INTFREE();

} /* ds707_pkti_dereg_wmk_reorig_cbacks() */

/*===========================================================================
FUNCTION      DS707_PKTI_REG_RLP_SRVC

DESCRIPTION   Registers parameters with RLP.

DEPENDENCIES  None.

RETURN VALUE  None.

SIDE EFFECTS  None.
===========================================================================*/
LOCAL void ds707_pkti_reg_rlp_srvc
(
  ds707_tc_state_type   *tc_state_ptr
)
{
/* - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - */
  /*-------------------------------------------------------------------------
    Register with both types of RLP.  This is okay, as MUX will only
    use one.
  -------------------------------------------------------------------------*/
  ds707_wmk_setup_wmks();

  rlp_reg_srvc(tc_state_ptr->tx_watermark_ptr,
               tc_state_ptr->post_rx_func_ptr,
               tc_state_ptr->post_rx_q_ptr);

  dsrlp_reg_srvc(tc_state_ptr->rlp_1x.rlp_instance,
                 tc_state_ptr->tx_watermark_ptr,
                 tc_state_ptr->post_rx_func_ptr,
                 tc_state_ptr->post_rx_q_ptr);

#ifdef FEATURE_HDR
#error code not present
#endif /* FEATURE_HDR */
} /* ds707_pkti_reg_rlp_srvc() */

/*===========================================================================
FUNCTION      DS707_PKTI_PHYS_LINK_DOWN_CLEANUP

DESCRIPTION   Called when the IFACE phys_link goes down.  This cleans up
              RLP, empties the watermarks, sets the callbacks for dormancy.

              Third parameter can_go_dormant indicates only if it is okay
              for call to go dormant.  Call may still not go dormant, if
              QNC or IFACE not up.

DEPENDENCIES  None.

RETURN VALUE  None.

SIDE EFFECTS  None.
===========================================================================*/
LOCAL void ds707_pkti_phys_link_cleanup
(
  ds707_tc_state_type       *tc_state_ptr,
  ps_iface_type             *iface_ptr,
  boolean                    can_go_dormant,
  boolean                    was_failed_orig
)
{
  dsm_item_type             *item_ptr;
  dword                      dorm_timer;
  byte                       sr_id;
  boolean                   *snpzid_change_ptr;
  boolean                    cleanup_for_going_null = FALSE; 
/* - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - */

  ASSERT(IS_IN_DS_TASK());

  MSG_HIGH("DS707 PKT phys link down cleanup",0,0,0);

  /*-------------------------------------------------------------------------
    When no traffic channel, then IDLE timer should not run.  do this by
    setting a reserved bit in the IDLE bitmask whenever there is no phys
    link.
  -------------------------------------------------------------------------*/
  ds707_cta_reset_timer(DS707_CTA_STOP_TIMER, DS707_INVALID_SO);

  /*-------------------------------------------------------------------------
    Set the cleanup for going NULL to TRUE only when the pkt data session
    is going NULL. Otherwise if the packet data session is going dormant,
    set the cleanup for going NULL to FALSE
  -------------------------------------------------------------------------*/
  if ((ds707_pkti_can_go_dormant(tc_state_ptr,iface_ptr) == TRUE) &&
      (can_go_dormant == TRUE))
  {
    cleanup_for_going_null = FALSE;
  }
  else
  {
    cleanup_for_going_null = TRUE;
  }

#ifdef FEATURE_HDR
#error code not present
#endif /* FEATURE_HDR */

  {
    /*-------------------------------------------------------------------------
     Cleanup 1X RLP.
    -------------------------------------------------------------------------*/
    sr_id = dsrlp_get_sr_id(tc_state_ptr->rlp_1x.rlp_instance);
    dsrlp_cleanup(sr_id, cleanup_for_going_null);
  }

  /*-------------------------------------------------------------------------
    Empty SN queues (Rev Link)
  -------------------------------------------------------------------------*/
  if (tc_state_ptr->post_rx_q_ptr != NULL)
  {
    while ((item_ptr =
              (dsm_item_type *)q_get(tc_state_ptr->post_rx_q_ptr))
                                                                     != NULL)
    {
      (void)dsm_free_packet(&item_ptr);
    }
  }

  /*-------------------------------------------------------------------------
    Empty 1X/HDR SN watermarks (Fwd Link)
  -------------------------------------------------------------------------*/
  ASSERT(tc_state_ptr->tx_watermark_ptr != NULL);

  dsm_empty_queue(tc_state_ptr->tx_watermark_ptr);

  /*-------------------------------------------------------------------------
      If call is going dormant, set watermark callback to initiate leaving
      dormancy.  See if holddown timer is required.
  -------------------------------------------------------------------------*/
    if (cleanup_for_going_null == FALSE)
    {
      INTLOCK();
        TASKLOCK();
          tc_state_ptr->tx_watermark_ptr->each_enqueue_func_ptr =
                                                            ds707_pkt_reorig;
        TASKFREE();
      INTFREE();

    /*-----------------------------------------------------------------------
      Use holddown timer only when not a failed orig.   Inthis case a
      failed orig is where DS filtered the orig (i.e, orig did not get sent
      to CM).
      
      If no holddown timer, do it for 50-msec, for system to "clean itself
      up".  Otherwise use normal holddown timer.
    -----------------------------------------------------------------------*/
      ps_iface_disable_flow(iface_ptr, DS_HOLDDOWN_TIMER_MASK);

      dorm_timer = (dword)ds707_dorm_timer_get_timer();
    if ((dorm_timer > 0)                && 
        (enable_holddown_timer == TRUE) &&
        (was_failed_orig == FALSE)
       )
    {
        MSG_HIGH("Start holddown timer %d msecs",dorm_timer,0,0);
        ds3gi_start_timer(DS3G_TIMER_HOLDDOWN, dorm_timer);
      }
      else
      {
        MSG_HIGH("Start 50-msec call end timer",0,0,0);
        ds3gi_start_timer(DS3G_TIMER_HOLDDOWN, 50);
      }
    /*-----------------------------------------------------------------------
      Post a call ended event to the EPZID hysteresis engine. This 
        should start the HAT if the hysteresis engine is still in the
      INIT state. Expiry of HAT would grow the EPZID hysteresis list 
    -----------------------------------------------------------------------*/
#ifdef FEATURE_DS_PZID_HYSTERESIS
      ds707_pzid_hysteresis_process_event(PZID_HYS_CALL_ENDED_EVENT);
#else
      ds707_epzid_hysteresis_process_event(EPZID_HYS_CALL_ENDED_EVENT);
#endif /* FEATURE_DS_PZID_HYSTERESIS  */
    }
    else
    {
    /*-----------------------------------------------------------------------
        Cannot go dormant.  For sanity, make sure holddown timer not active
        and the each_enqueue ptrs are NULL.

        Clear the re-orig signal.  Due to message passing, it could become
        possible that phys_link goes down before IFACE goes down, presenting
        an IFACE that looks dormant, but is not dormant.
    -----------------------------------------------------------------------*/
      INTLOCK();
        TASKLOCK();
          tc_state_ptr->tx_watermark_ptr->each_enqueue_func_ptr = NULL;
          rex_clr_sigs(&ds_tcb, DS_PKT_REORIG_SIG);
        TASKFREE();
      INTFREE();

      MSG_HIGH("Stop holddown timer",0,0,0);
      ds3gi_stop_timer(DS3G_TIMER_HOLDDOWN);
      ps_iface_enable_flow(iface_ptr, DS_HOLDDOWN_TIMER_MASK);

#ifdef FEATURE_HDR
#error code not present
#endif /* FEATURE_HDR */
    }

  /*-------------------------------------------------------------------------
    Perform a little housekeeping.
  -------------------------------------------------------------------------*/
  tc_state_ptr->call_id = CM_CALL_ID_INVALID;
  tc_state_ptr->so      = DS707_INVALID_SO;

  snpzid_change_ptr = GET_SNPZID_CHANGE_FLAG_PTR(
                        (uint32)(iface_ptr->client_data_ptr));
  *snpzid_change_ptr = FALSE;

} /* ds707_pkti_phys_link_down_cleanup() */

/*===========================================================================
                           3G DSMGR HANDLERS
===========================================================================*/
/*===========================================================================
FUNCTION      DS707_PKTI_CALL_ID_HDLR

DESCRIPTION   Called after call originates.  Gives the CM call id to this
              mode specific handler.

DEPENDENCIES  None.

RETURN VALUE  None

SIDE EFFECTS  None
===========================================================================*/
LOCAL void ds707_pkti_call_id_hdlr
(
  cm_call_id_type        cm_call_id,
  void                  *mode_spec_info_ptr
)
{
  uint32                   pkt_instance;
  ds707_tc_state_type      *tc_state_ptr;
/* - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - */
  pkt_instance = GET_PKT_INST_FROM_CALL_INFO(mode_spec_info_ptr);
  VERIFY_PKT_INSTANCE(pkt_instance);

  tc_state_ptr = GET_TC_STATE_PTR(pkt_instance);
  tc_state_ptr->call_id = cm_call_id;
} /* ds707_pkti_call_id_hdlr() */

/*===========================================================================
FUNCTION      DS707_PKTI_GET_ORIG_SO

DESCRIPTION   Returns the SO to originate with.

DEPENDENCIES  None

RETURN VALUE  Service option to originate with.

SIDE EFFECTS  None.
===========================================================================*/
LOCAL ds707_so_type ds707_pkti_get_orig_so(void)
{
  ds707_so_type                orig_so;          /* SO to orig with        */
  ds707_rateset_type           rateset;          /* originating rateset    */
/* - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - */
#ifdef FEATURE_DS_IS707A_CMUX
  rateset = (ds707_rateset_type)
                       (dsat707_cmux_rev_table[(dsat707_cmux_val[1])] - 1);
#else
  rateset = (ds707_rateset_type)(dsat707_cmux_val - 1);
#endif
  #ifndef FEATURE_ASYNC_DATA_NOOP
  if (ds707_pkti_use_qnc() == TRUE)
  {
    orig_so = ds707_so_async_get_orig(rateset);
    /* were we prev dormant or MIP still active?  If so, have to end that session first,
       before originating.  UNFINISHED */
  }
  else
  #endif
  {
    orig_so = ds707_so_pkt_get_orig(rateset);
  }

  return(orig_so);
}


/*===========================================================================
FUNCTION      DS707_PKTI_ORIG_HDLR

DESCRIPTION   Called from 3G DSMGR when originating a packet data call.

DEPENDENCIES  Watermarks to RLP already set up correctly.  Passed in ptrs are
              not NULL.

RETURN VALUE  DS3G_SUCCESS - call can be made.
              DS3G_FAILURE - call can't be made.

SIDE EFFECTS  None.
===========================================================================*/
LOCAL boolean ds707_pkti_orig_hdlr
(
  const byte                           *dial_string,
        uint8                           dial_string_len,
        cm_cdma_orig_params_s_type     *cdma_orig_params_ptr,
        boolean                        *cdma_params_changed,
        cm_gw_cs_orig_params_s_type    *gw_cs_orig_params_ptr,
        boolean                        *gw_cs_params_changed,
        cm_gw_ps_orig_params_s_type    *gw_ps_orig_params_ptr,
        boolean                        *gw_ps_params_changed,
        cm_srv_type_e_type             *cm_srv_type,
        void                           *mode_spec_info_ptr
)
{
  ds707_so_type                orig_so;          /* SO to orig with        */
  ds707_pkt_call_info_type    *orig_info_ptr;    /* ptr to orig info       */
  ps_iface_type               *iface_ptr;        /* ptr to 1x iface        */
  #ifdef FEATURE_IS2000_REL_A_SVD
  ds707_tc_state_type         *tc_state_ptr;
  #endif /* FEATURE_IS2000_REL_A_SVD */
/* - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - */
  MSG_HIGH("DS707 PKT ORIG CALL HDLR",0,0,0);

  orig_info_ptr = (ds707_pkt_call_info_type *)mode_spec_info_ptr;

  VERIFY_PKT_INSTANCE(orig_info_ptr->pkt_instance);

  iface_ptr    = GET_PS_IFACE_PTR(orig_info_ptr->pkt_instance);
  #ifdef FEATURE_IS2000_REL_A_SVD
  tc_state_ptr = GET_TC_STATE_PTR(orig_info_ptr->pkt_instance);
  #endif /* FEATURE_IS2000_REL_A_SVD */

  orig_so = ds707_pkti_get_orig_so();

  if (orig_so == DS707_INVALID_SO)
  {
    ASSERT(0);
    ERR("Bad mobile settings - no SO to orig with",0,0,0);
    return(DS3G_FAILURE);
  }

  cdma_orig_params_ptr->srv_opt        = (uint16)orig_so;
  cdma_orig_params_ptr->activate_code  = CM_OTASP_ACT_CODE_NONE;
  cdma_orig_params_ptr->drs_bit        = orig_info_ptr->drs_bit;
  cdma_orig_params_ptr->sr_id_included = FALSE;
  cdma_orig_params_ptr->qos_parms_incl = FALSE;


  /*-----------------------------------------------------------------------
    Originate with service type set to Automatic. This means that if it is
    possible to originate the Packet Data call on an HDR system, that will
    be attempted. In 1X only systems, the CM will originate the call on a
    CDMA system.
  -----------------------------------------------------------------------*/
#ifdef FEATURE_HDR
#error code not present
#else
  *cm_srv_type                         = CM_SRV_TYPE_CDMA_SPECIFIC;
#endif /* FEATURE_HDR */
#ifdef FEATURE_IS2000_REL_A_SVD
  if (tc_state_ptr->alloc_sr_id != MCCSRID_INVALID_SRID)
  {
    /*-----------------------------------------------------------------------
      SR_ID was reserved at power up time. So make the SR_ID flag TRUE so
      that MC will use the allocated SR ID.
    -----------------------------------------------------------------------*/
    cdma_orig_params_ptr->sr_id_included = TRUE;
    cdma_orig_params_ptr->sr_id          = tc_state_ptr->alloc_sr_id;
  }
#endif /* FEATURE_IS2000_REL_A_SVD */


  if (orig_so == CAI_SO_PPP_PKT_DATA_3G)
  {
#ifdef FEATURE_IS2000_REL_A
    ds707_so33_orig_qos(cdma_orig_params_ptr);
#endif /* FEATURE_IS2000_REL_A */
  }

  if (ds707_so_retrydel_is_delayed() == TRUE)
  {
    MSG_HIGH("Pkt data calls are delayed",0,0,0);
    return(DS3G_FAILURE);
  }
  else if (((DS_FLOW_CTRL_IS_BIT_SET(ps_iface_get_flow_mask(iface_ptr),
                                     DS_HOLDDOWN_TIMER_MASK)) == TRUE)    &&
           (orig_info_ptr->use_hdown_timer == TRUE)
          )
  {
    MSG_HIGH("Cant orig - pkt HOLDDOWN TIMER",0,0,0);
    return(DS3G_FAILURE);
  }
  else
  {
    *cdma_params_changed = TRUE;

    MSG_HIGH("Orig PKT call  SO  %d   MDR  %d",orig_so,dsat707_qcmdr_val,0);
#ifdef  FEATURE_DS_QNC
    MSG_HIGH("               QNC %d",dsat707_qcqnc_val,0,0);
#endif
    MSG_HIGH("               QCSO %d",0,dsat707_qcso_val,0);
    MSG_HIGH("               DRS %d    CRM %d",orig_info_ptr->drs_bit,
                                               dsat707_crm_val, 
                                               0);
    return(DS3G_SUCCESS);
  }
} /* ds707_pkti_orig_hdlr() */

/*===========================================================================
FUNCTION      DS707_PKTI_INC_CALL_HDLR

DESCRIPTION   Called when CM receives a page from BS.  Only time we accept
              a call is when we are dormant.

DEPENDENCIES  None.

RETURN VALUE  Will either REJECT the call or tell CM to answer the call.

SIDE EFFECTS  None
===========================================================================*/
LOCAL ds3g_incom_e_type ds707_pkti_inc_call_hdlr
(
  ds_cmd_type            *cmd_ptr,
  cm_call_id_type         cm_call_id,
  cm_ans_params_s_type   *ans_params, /* Answer params sent to Call Mgr    */
  cm_end_params_s_type   *end_params  /* end params if call rejected       */
)
{
  ds707_tc_state_type    *tc_state_ptr;
  ps_iface_type          *iface_ptr;
  ds3g_incom_e_type       ret_val;
  boolean                 accept_call = TRUE; /* JCDMA may change this val */

#ifdef FEATURE_JCDMA_1X
  uint16                  inc_srv_opt;
#endif /* FEATURE_JCDMA_1X */

/* - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - */

  MSG_HIGH("DS707 Pkt inc call hdlr",0,0,0);

  tc_state_ptr = GET_TC_STATE_PTR(DS707_DEF_PKT_INSTANCE);
  iface_ptr    = GET_PS_IFACE_PTR(DS707_DEF_PKT_INSTANCE);


#ifdef FEATURE_JCDMA_1X
  inc_srv_opt = cmd_ptr->cmd.call_info.mode_info.info.cdma_call.srv_opt;
#endif /* FEATURE_JCDMA_1X */

#ifdef FEATURE_JCDMA_1X
  accept_call = ds707_so_jcdma_verify_inc_pkt_so(inc_srv_opt);
#endif /* FEATURE_JCDMA_1X */

  if ((ds707_pkt_is_dormant(iface_ptr) == TRUE) && 
      (accept_call == TRUE)
     )
  {
      if (cmd_ptr->cmd.call_info.mode_info.info.cdma_call.alert_ans_bypass
                                                                     == TRUE)
      {
      /*---------------------------------------------------------------------
          Alert_ans_bypass is set, which means we (DS) cannot answer the
          call.  Tell 3G DSMGR to do nothing.
      ---------------------------------------------------------------------*/
        MSG_HIGH("alert ans bypass - do nothing",0,0,0);
        ret_val = DS3G_DO_NOTHING;
      }
      else
      {
        MSG_HIGH("answer incoming call",0,0,0);
        ans_params->info_type = CM_CALL_MODE_INFO_CDMA;
        ans_params->ans_params.cdma_ans.call_type = CM_CALL_TYPE_PS_DATA;
        ds707_pkti_dereg_wmk_reorig_cbacks(tc_state_ptr);
        ret_val = DS3G_ANSWER_CALL;
      }
      tc_state_ptr->call_id = cm_call_id;
      ds707_pkti_reg_rlp_srvc(tc_state_ptr);
    }
#ifdef FEATURE_HDR
#error code not present
#endif /* FEATURE_HDR */
  else
  {
    MSG_HIGH("reject incoming call",0,0,0);
    end_params->call_id = cm_call_id;
    end_params->info_type = CM_CALL_MODE_INFO_CDMA;
    end_params->end_params.cdma_end.end_reason_included = TRUE;
#ifdef FEATURE_JCDMA_1X
    /*-----------------------------------------------------------------------
      JCDMA may have said reject dormant call.  In this case, still say
      dormant in ORDQ val
    -----------------------------------------------------------------------*/
    if (ds707_pkt_is_dormant(iface_ptr) == TRUE)
    {
      end_params->end_params.cdma_end.end_reason = CAI_REL_NORMAL;
    }
    else
#endif /* FEATURE_JCDMA_1X */
    {
    end_params->end_params.cdma_end.end_reason = CAI_REL_SRV_INACT;
    }
    ret_val = DS3G_REJECT_CALL;
  }

  return(ret_val);
} /* ds707_pkti_inc_call_hdlr() */

/*===========================================================================
FUNCTION      DS707_PKTI_SWITCH_CALLS_HDLR

DESCRIPTION   Called when CM receives an incoming page for the same type of
              call as what we are originating.

              In this case, we want to end the origination, but accept the
              incoming page.  We must then switch our call_id to the
              incoming call id.

DEPENDENCIES  None.

RETURN VALUE  Always tell 3G DSMGR to accept incoming call.

SIDE EFFECTS  None
===========================================================================*/
LOCAL ds3g_incom_e_type ds707_pkti_switch_calls_hdlr
(
  ds_cmd_type           *cmd_ptr,
  cm_call_id_type        inc_call_id,
  cm_call_id_type        exist_call_id,
  cm_ans_params_s_type  *ans_params,      /* ans parms for inc call        */
  cm_end_params_s_type  *end_params_orig, /* end parms for orig call       */
  cm_end_params_s_type  *end_params_inc   /* end parms for inc call        */
)
{
  ps_iface_type             *iface_ptr     = NULL;
  ds707_tc_state_type       *tc_state_ptr  = NULL;
  ds3g_incom_e_type          ret_val;
/*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  ds707_pkti_get_state_ptrs_cid(exist_call_id, &tc_state_ptr, &iface_ptr);

  if ((tc_state_ptr != NULL)                                &&
      (iface_ptr    != NULL)                                &&
      (ps_iface_phys_link_state(iface_ptr) == PHYS_LINK_COMING_UP)
     )
  {
    if (cmd_ptr->cmd.call_info.mode_info.info.cdma_call.alert_ans_bypass
                                                                     == TRUE)
    {
      /*---------------------------------------------------------------------
        Alert_ans_bypass is set, which means we (DS) cannot answer the
        call.
      ---------------------------------------------------------------------*/
      ret_val = DS3G_DO_NOTHING;
      MSG_HIGH("Do nothing inc call, but end orig call",0,0,0);
    }
    else
    {
      /*---------------------------------------------------------------------
        No bypass alert, so okay to accept incoming call.
      ---------------------------------------------------------------------*/
      ans_params->ans_params.cdma_ans.call_type = CM_CALL_TYPE_PS_DATA;
      ans_params->info_type = CM_CALL_MODE_INFO_CDMA;
      ret_val = DS3G_ANSWER_CALL;
      MSG_HIGH("ans inc call, end orig call",0,0,0);
    }
    /*---------------------------------------------------------------------
      We have a call in "coming-up state", which means origination.  Not
      necessary to be dormant.  We are already accepting incoming call,
      so need to end the orig'ing call.  Set end_params so that old call is
      not kept dormant in BS.

      Also, switch state_ptr to use the call_id of the incoming call.
    ---------------------------------------------------------------------*/
    tc_state_ptr->call_id = inc_call_id;

    end_params_orig->call_id   = exist_call_id;
    end_params_orig->info_type = CM_CALL_MODE_INFO_CDMA;
    end_params_orig->end_params.cdma_end.end_reason_included = TRUE;
    end_params_orig->end_params.cdma_end.end_reason = CAI_REL_SRV_INACT;
    return(ret_val);
  }
  else
  {
    /*-----------------------------------------------------------------------
      We have no old call with the call id shown.  Get rid of this call too.
    -----------------------------------------------------------------------*/
    end_params_inc->call_id   = inc_call_id;
    end_params_inc->info_type = CM_CALL_MODE_INFO_CDMA;
    end_params_inc->end_params.cdma_end.end_reason_included = TRUE;
    end_params_inc->end_params.cdma_end.end_reason = CAI_REL_SRV_INACT;
    MSG_HIGH("reject inc call",0,0,0);
    return(DS3G_REJECT_CALL);
  }
} /* ds707_pkti_switch_calls_hdlr() */

/*===========================================================================
FUNCTION      DS707_PKTI_FORCE_CALL_DOWN_HDLR

DESCRIPTION   Called by 3G DSMGR when a call must be forced down, but need
              end_params filled out.

DEPENDENCIES  Called in DS TASK context.  Is called autonomously.

RETURN VALUE  TRUE. there was a call to tear down.  FALSE - no call to tear
              down.

SIDE EFFECTS  None.
===========================================================================*/
boolean ds707_pkti_force_call_down_hdlr
(
  cm_call_id_type        call_id,
  cm_end_params_s_type  *end_params_ptr
)
{
  int                          i;
  ps_iface_type               *iface_ptr;
  ds707_tc_state_type         *tc_state_ptr;
  ds707_pkt_call_info_type     mode_spec_info;
  boolean                      delay_disconnect;
  ds707_pkt_call_abort_f_ptr   abort_f_ptr;
/*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/
  memset(&mode_spec_info, 0, sizeof(ds707_pkt_call_info_type));
  for (i = 0; i < DS707_MAX_PKT_CALLS; i++)
  {
    iface_ptr    = GET_PS_IFACE_PTR(i);
    tc_state_ptr = GET_TC_STATE_PTR(i);
    abort_f_ptr  = GET_ABORT_F_PTR(i);

    if (tc_state_ptr->call_id == call_id)
    {
      mode_spec_info.pkt_instance = i;
      ds707_pkti_end_call_hdlr(call_id,
                               end_params_ptr,
                               &delay_disconnect,
                               &mode_spec_info);

      /*---------------------------------------------------------------------
        If call cannot go dormant, then send a message to DSSNET, so that it
        ends any PPP issues.  Also cleanup the phys_link state, indicating
        that the call cannot go dormant.
      ---------------------------------------------------------------------*/
      if (ds707_pkti_can_go_dormant(tc_state_ptr, iface_ptr) == FALSE)
      {
        ds707_pkti_phys_link_cleanup(tc_state_ptr, iface_ptr, FALSE, FALSE);
        ps_iface_phys_link_down_ind(iface_ptr);


        /*-------------------------------------------------------------------
          Let CM/UI know that the packet call has gone NULL
        -------------------------------------------------------------------*/
        ds3g_msh_set_pkt_state(CM_PACKET_STATE_NULL);


        if(abort_f_ptr != NULL)
        {
          abort_f_ptr();
        }
        else
        {
          ASSERT(0);
          ERR("No abort handler registered",0,0,0);
        }
      }
      else
      {
        ds707_pkti_phys_link_cleanup(tc_state_ptr, iface_ptr, TRUE, FALSE);
        ps_iface_phys_link_down_ind(iface_ptr);

        /*-------------------------------------------------------------------
          Let CM/UI know that the packet call has gone dormant
        -------------------------------------------------------------------*/
        ds3g_msh_set_pkt_state(CM_PACKET_STATE_DORMANT);

#ifdef FEATURE_JCDMA_1X
        ds707_so_pkt_recal();
#endif
      }
      #ifdef FEATURE_DYNAMIC_CLOCK_FREQUENCY_CHANGING
        clk_regime_deregister_for_cpu_resource
         (
           CLKRGM_CDMA_DATA_APPS
        );

      #endif /* FEATURE_DYNAMIC_CLOCK_FREQUENCY_CHANGING */

      return(DS3G_SUCCESS);
    }
  }

  return(DS3G_FAILURE);
} /* ds707_pkti_force_call_down_hdlr() */

/*===========================================================================
FUNCTION      DS707_PKTI_CALL_CONN_HDLR

DESCRIPTION   Called by 3G DSMGR when traffic channel connects.

DEPENDENCIES  None.

RETURN VALUE  DS3G_SUCCESS - call connected okay
              DS3G_FAILURE - call failed

SIDE EFFECTS  None.
===========================================================================*/
LOCAL boolean ds707_pkti_call_conn_hdlr
(
  cm_call_id_type            call_id,
  cm_call_mode_info_s_type  *mode_info_ptr,
  boolean                   *delay_connect,
  cm_end_params_s_type      *end_params
)
{
  boolean                    ret_val = DS3G_FAILURE;
  ps_iface_type             *iface_ptr    = NULL;
  ds707_tc_state_type       *tc_state_ptr = NULL;
  boolean                   *snpzid_change_ptr;
/*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/
  MSG_HIGH("DS707 PKT CALL CONNECTED HDLR",0,0,0);

  *delay_connect = FALSE;   /* always FALSE.  For wcdma/gsm only           */

  ds707_pkti_get_state_ptrs_cid(call_id, &tc_state_ptr, &iface_ptr);

  if ((tc_state_ptr == NULL) || (iface_ptr == NULL))
  {
    ERR("conn call with unknown id %d",call_id, 0, 0);
  }
  else if ( ((ps_iface_phys_link_state(iface_ptr) != PHYS_LINK_COMING_UP)  &&
             (ds707_pkt_is_dormant(iface_ptr) == FALSE)) ||
            (ps_iface_state(iface_ptr) == IFACE_DOWN) ||
            (ps_iface_state(iface_ptr) == IFACE_DISABLED)
          )
  {
    /*-----------------------------------------------------------------------
      Phone was not dormant (covers case where we get an incoming page) &&
      phone was not orig'ing (covers case where we initiated the call).
      Can't connect.
    -----------------------------------------------------------------------*/
    ERR("incorrect phys link state",0,0,0);
  }
  else
  {

      #ifdef FEATURE_DYNAMIC_CLOCK_FREQUENCY_CHANGING
        clk_regime_register_for_cpu_resource
         (
           CLKRGM_CDMA_DATA_APPS
        );

     #endif /* FEATURE_DYNAMIC_CLOCK_FREQUENCY_CHANGING */


/*    if ( (dsrlp_is_inited(tc_state_ptr->rlp_1x.rlp_instance) == FALSE))
    {
      dsrlp_init((byte)DSRLP_FIXED_SR_ID_INDEX,
                 (byte)cfg_info->con[so_index].sr_id, 
                  DSRLP_REV_3, 
                  FALSE
                );
    } */

    /*-----------------------------------------------------------------------
      Call came up.  RLP is configured either on incoming page or on
      mobile origination.
    -----------------------------------------------------------------------*/
    tc_state_ptr->so = mode_info_ptr->info.cdma_call.srv_opt;
    ds707_pkt_prev_data_call_so = tc_state_ptr->so;

    /*-----------------------------------------------------------------------
      Note that the ds707_pkt_prev_sr_id is used only in 1X calls while
      coming out of dormancy to verify that the sr id is the same as what we
      had before we went dormant. For HDR the previous sr id does not make
      any sense. If a HDR call gets connected, then the ds707_pkt_prev_sr_id
      value does not have a valid value.
    -----------------------------------------------------------------------*/
    ds707_pkt_prev_sr_id = dsrlp_get_sr_id(tc_state_ptr->rlp_1x.rlp_instance);
    ret_val = DS3G_SUCCESS;
  }

  if (ret_val == DS3G_FAILURE)
  {
    /*-----------------------------------------------------------------------
      Hardcode params to kill this call (no dormancy).  Do not have to
      do a phys_link_down_ind() yet, as CM will send a call_cmd_end() when
      TC is gone.
    -----------------------------------------------------------------------*/
    end_params->call_id = call_id;
    end_params->info_type = CM_CALL_MODE_INFO_CDMA;
    end_params->end_params.cdma_end.end_reason_included = TRUE;
    end_params->end_params.cdma_end.end_reason = CAI_REL_SRV_INACT;
  }
  else
  {
    /*-----------------------------------------------------------------------
      Call has come up.  For sanity, diable holddown timer and enable flow
      on IFACE.

      Enable the idle timer.  Called function does error checking on function
      to see if IDLE timer should be enabled.

      Also tell DSSNET if in a QNC call.  When in QNC, we do not do LCP
      auth (it is rejected).

      Then call phys_link_indication().
    -----------------------------------------------------------------------*/
    MSG_HIGH("Stop holddown timer",0,0,0);
    ds3gi_stop_timer(DS3G_TIMER_HOLDDOWN);
    ps_iface_enable_flow(iface_ptr, DS_HOLDDOWN_TIMER_MASK);
    /*-----------------------------------------------------------------------
        Post a call connected event to the PZID hysteresis engine. This 
      should stop the HAT if it has been running. 
    -----------------------------------------------------------------------*/
#ifdef FEATURE_DS_PZID_HYSTERESIS
      ds707_pzid_hysteresis_process_event(PZID_HYS_CALL_CONNECTED_EVENT);
#else
      ds707_epzid_hysteresis_process_event(EPZID_HYS_CALL_CONNECTED_EVENT);
#endif /* FEATURE_DS_PZID_HYSTERESIS  */

    ds707_cta_reset_timer(DS707_CTA_START_TIMER, tc_state_ptr->so);
    
    #ifndef FEATURE_ASYNC_DATA_NOOP
    dssnet_set_in_qnc_call(ds707_so_async_so_is_async(tc_state_ptr->so));
    #else
    /* As QNC is not supported seeting the value to FALSE */
#ifdef CUST_EDITION
    MSG_FATAL("ds707_so_async_so_is_async %d %d",tc_state_ptr->so,bIsPPPAuthEnabled,0);
#ifdef FEATURE_VERSION_W208S
    dssnet_set_in_qnc_call(!bIsPPPAuthEnabled);
#else
	dssnet_set_in_qnc_call(FALSE);
#endif
#else
    dssnet_set_in_qnc_call(FALSE);
#endif
    #endif

#ifdef FEATURE_IS2000_REL_A
    ds707_so33_process_qos ( mode_info_ptr );
#endif /* FEATURE_IS2000_REL_A */

    /*-----------------------------------------------------------------------
      When traffic channel comes up, tell CM that the pkt state is connected.
    -----------------------------------------------------------------------*/
    ds3g_msh_set_pkt_state(CM_PACKET_STATE_CONNECT);

    ds707_pkti_dereg_wmk_reorig_cbacks(tc_state_ptr);

    ps_iface_phys_link_up_ind(iface_ptr);

#ifdef FEATURE_CALL_THROTTLE
      /*-----------------------------------------------------------------------
        If the call is coming out of dormancy we  post call_success to DCTM
        as PPP is already UP hence the call is successful. For the first call
        we wait till PPP comes up before we report SUCCESS to DCTM module.
        Without this fix if the previous re-orig failed at system level for
        some reason, the call remains throttled.
      -----------------------------------------------------------------------*/
      /*--------------------------------------------------------------------
        Upon Call origination check if the returning from a DORMANT call
        IFACE_UP is for sockets calls and IFACE_ROUTEABLE is for relay and
        network model calls
      --------------------------------------------------------------------*/
      if ( (ds707_pkt_is_um_iface_up(iface_ptr) == TRUE) ||
           (ds707_rmsm_is_rm_iface_up() == TRUE) )
      {
        MSG_HIGH("ds707_pkt_mgr: posting success to DCTM", 0, 0, 0);
#ifdef FEATURE_DS_MOBILE_IP
        if (ds707_drs_is_in_mip_call() == TRUE)
        {
          dctm_post_mip_call_status( MIP_SUCCESS_EV, MIP_FAIL_REASON_NONE );
        }
        else
#endif /* FEATURE_DS_MOBILE_IP */
        {
          dctm_post_ppp_call_status( PPP_SUCCESS_EV, PPP_PROTOCOL_IPCP, 
                                     PPP_FAIL_REASON_NONE );
        }
      }
#endif /* FEATURE_CALL_THROTTLE */

#ifdef FEATURE_HDR
#error code not present
#endif /* FEATURE_HDR */

    /*-----------------------------------------------------------------------
      If we came out of dormancy due to SID/NID/PZID change, we might have
      to intiate a PPP resync.
    -----------------------------------------------------------------------*/
    snpzid_change_ptr = GET_SNPZID_CHANGE_FLAG_PTR(
                          (uint32)(iface_ptr->client_data_ptr));

    if(*snpzid_change_ptr)
    {
      ds707_drs_handle_snpzid_change_recon();
      *snpzid_change_ptr = FALSE;
    }
  }
  return(ret_val);
} /* ds707_pkti_call_conn_hdlr() */

/*===========================================================================
FUNCTION      DS707_PKTI_END_CALL_HDLR

DESCRIPTION   Called by 3G DSMGR before telling CM to end a call.  Only
              thing done here is setting the ORDQ value correctly and
              letting CM know that this was a packet call.

DEPENDENCIES  Called in DS TASK context.

RETURN VALUE  None.

SIDE EFFECTS  None.
===========================================================================*/
LOCAL void ds707_pkti_end_call_hdlr
(
  cm_call_id_type        call_id,
  cm_end_params_s_type  *end_params,
  boolean               *delay_disconnect,
  void                  *mode_spec_info_ptr
)
{
  ds707_tc_state_type   *tc_state_ptr;
  ps_iface_type         *iface_ptr;
  uint32                 pkt_instance;
/* - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - */
  MSG_HIGH("DS707 PKT END CALL HDLR",0,0,0);

  pkt_instance = GET_PKT_INST_FROM_CALL_INFO(mode_spec_info_ptr);
  VERIFY_PKT_INSTANCE(pkt_instance);

  tc_state_ptr = GET_TC_STATE_PTR(pkt_instance);
  iface_ptr    = GET_PS_IFACE_PTR(pkt_instance);

  if (tc_state_ptr->call_id != call_id)
  {
    ERR("CALL IDs DO NOT MATCH. STILL CONTINUE",0,0,0);
  }

  *delay_disconnect     = FALSE;
  end_params->call_id = call_id;
  end_params->info_type = CM_CALL_MODE_INFO_CDMA;

  end_params->end_params.cdma_end.end_reason_included = TRUE;
  /*-------------------------------------------------------------------------
    Assumption is that if DTR is de-asserted, then the IFACE state is not
    up nor is it routeable.  These values are set before we get here.
  -------------------------------------------------------------------------*/
  if (ds707_pkti_can_go_dormant(tc_state_ptr,iface_ptr) == TRUE)
  {
    end_params->end_params.cdma_end.end_reason = CAI_REL_NORMAL;
  }
  else
  {
    end_params->end_params.cdma_end.end_reason = CAI_REL_SRV_INACT;
  }

} /* ds707_pkti_end_call_hdlr() */


/*===========================================================================
FUNCTION      DS707_PKTI_CALL_ENDED_HDLR

DESCRIPTION   Called by 3G DSMGR when a call release has been received from
              CM.

DEPENDENCIES  None.

RETURN VALUE  None

SIDE EFFECTS  None.
===========================================================================*/
LOCAL void ds707_pkti_call_ended_hdlr
(
  ds_cmd_type            *cmd_ptr,
  boolean                 call_was_connected
)
{
  ds707_tc_state_type       *tc_state_ptr = NULL;
  ps_iface_type             *iface_ptr    = NULL;
  boolean                    ui_end_key_forces_null = FALSE;
  ds707_pkt_call_abort_f_ptr abort_f_ptr;
  uint32                     pkt_instance;
/*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/
  MSG_HIGH("DS707 PKT CALL ENDED HDLR",0,0,0);

  ASSERT(IS_IN_DS_TASK());


  #ifdef FEATURE_DYNAMIC_CLOCK_FREQUENCY_CHANGING
     clk_regime_deregister_for_cpu_resource
      (
        CLKRGM_CDMA_DATA_APPS
     );

  #endif /* FEATURE_DYNAMIC_CLOCK_FREQUENCY_CHANGING */

  pkt_instance = ds707_pkti_get_pkt_state_index_cid
                                             (
                                               cmd_ptr->cmd.call_info.call_id
                                             );
  if (pkt_instance < DS707_MAX_PKT_CALLS)
  {
    MSG_MED("Packet instance is %d",pkt_instance,0,0);
    tc_state_ptr = GET_TC_STATE_PTR(pkt_instance);
    iface_ptr    = GET_PS_IFACE_PTR(pkt_instance);
    abort_f_ptr  = GET_ABORT_F_PTR(pkt_instance);
  }
  else
  {
    ERR("Could not end call with id %d",
         cmd_ptr->cmd.call_info.call_id, 0, 0);
    return;
  }

  /*-------------------------------------------------------------------------
     At this point, we know traffic channel is gone.  Have to know if the
     IFACE should go dormant or not.  Then we need to clean up the pkt_state
     info for the next call.

     QNC_HO case
     Register some JCDMA stuff
  -------------------------------------------------------------------------*/

#ifdef FEATURE_JCDMA_1X
   if ((cmd_ptr->cmd.call_info.end_status == CM_CALL_END_CLIENT_END) &&
       (cmd_ptr->cmd.call_info.client_id != ds3gi_cm_client_id)
      )
   {
     MSG_HIGH("UI %d ended pkt call - go NULL",
              cmd_ptr->cmd.call_info.client_id, 0, 0);
     ui_end_key_forces_null = TRUE;
   }
#endif /* FEATURE_JCDMA_1X */

  /*-------------------------------------------------------------------------
    If call cannot go dormant, then send a message to DSSNET, so that it
    ends any PPP issues.  Also cleanup the phys_link state, indicating that
    the call cannot go dormant.
  -------------------------------------------------------------------------*/
  if ((ds707_pkti_can_go_dormant(tc_state_ptr, iface_ptr) == FALSE) ||
      (cmd_ptr->cmd.call_info.end_status == CM_CALL_END_REL_SO_REJ) ||
      (ui_end_key_forces_null == TRUE)
     )
  {
    /*-----------------------------------------------------------------------
      In most cases clean up RLP.
    -----------------------------------------------------------------------*/
    if ( cmd_ptr->cmd.call_info.end_status != CM_CALL_END_INCOM_CALL )
    {
      ds707_pkti_phys_link_cleanup(tc_state_ptr, iface_ptr, FALSE, FALSE);
    }
    else
    {
#ifdef FEATURE_HDR
#error code not present
#endif /* FEATURE_HDR */
      /*---------------------------------------------------------------------
        Outgoing call ends due to incoming call.
        Tasklock needed as context switch by MC to do dsrlp_init() might
        happen after we evaluate dsrlp_is_inited() and before calling cleanup
      ---------------------------------------------------------------------*/
      TASKLOCK();
      if ( (dsrlp_is_inited(tc_state_ptr->rlp_1x.rlp_instance) == TRUE))
      {
        /* Do nothing */
      }
      else
      {
        ds707_pkti_phys_link_cleanup(tc_state_ptr, iface_ptr, FALSE, FALSE);
      }
      TASKFREE();
    }
    ps_iface_phys_link_down_ind(iface_ptr);

    /*-------------------------------------------------------------------
      Let CM/UI know that the packet call has gone NULL
    -------------------------------------------------------------------*/
    ds3g_msh_set_pkt_state(CM_PACKET_STATE_NULL);

    if(abort_f_ptr != NULL)
    {
      abort_f_ptr();
    }
    else
    {
      // ASSERT(0);
      ERR("No abort handler registered",0,0,0);
    }
  }
  else
  {
    /*-----------------------------------------------------------------------
      In most cases clean up RLP.
    -----------------------------------------------------------------------*/
    if ( cmd_ptr->cmd.call_info.end_status != CM_CALL_END_INCOM_CALL )
    {
      ds707_pkti_phys_link_cleanup(tc_state_ptr, iface_ptr, TRUE, FALSE);
    }
    else
    {
#ifdef FEATURE_HDR
#error code not present
#endif /* FEATURE_HDR */
      /*---------------------------------------------------------------------
        Outgoing call ends due to incoming call.
        Tasklock needed as context switch by MC to do dsrlp_init() might
        happen after we evaluate dsrlp_is_inited() and before calling cleanup
      ---------------------------------------------------------------------*/
      TASKLOCK();
      if ( (dsrlp_is_inited(tc_state_ptr->rlp_1x.rlp_instance) == TRUE))
      {
        /* Do nothing */
      }
      else
      {
        ds707_pkti_phys_link_cleanup(tc_state_ptr, iface_ptr, TRUE, FALSE);
      }
      TASKFREE();
    }
    ps_iface_phys_link_down_ind(iface_ptr);

    /*-------------------------------------------------------------------
      Let CM/UI know that the packet call has gone dormant
    -------------------------------------------------------------------*/
    ds3g_msh_set_pkt_state(CM_PACKET_STATE_DORMANT);
    /*--------------------------------------------------------------
      Since we are going dormant, register the callback with RLP3 to 
      notify the PZID hysterisis engine of data transfer. The PZID
      hysterisis engine will be notified the first time a PPP packet
      is received or sent via RLP3.
    --------------------------------------------------------------*/
#ifdef FEATURE_DS_PZID_HYSTERESIS
    dsrlp_reg_ppp_data_transfer_notify_func
    (
      DSRLP_FIXED_SR_ID_INDEX,
      ds707_pzid_rlp_data_transfer_cb
    );
#else /* FEATURE_DS_EPZID_HYSTERESIS */
    dsrlp_reg_ppp_data_transfer_notify_func
    (
      DSRLP_FIXED_SR_ID_INDEX,
      ds707_epzid_rlp_data_transfer_cb
    );
#endif /* FEATURE_DS_PZID_HYSTERESIS  */

#ifdef FEATURE_JCDMA_1X
    ds707_so_pkt_recal();
#endif
  }

} /* ds707_pkti_call_ended_hdlr() */


/*===========================================================================
FUNCTION      DS707_PKTI_GET_CALL_CONN_INFO_HDLR

DESCRIPTION   Called by 3G DSMGR when UI invokes ds_get_call_information()
              to get call info Eg: "CDMA" as a NULL terminated string.

DEPENDENCIES  None.

RETURN VALUE  boolean: indicates if the call connection info is valid.

SIDE EFFECTS  None.
===========================================================================*/
LOCAL boolean ds707_pkti_get_call_conn_info_hdlr
(
  cm_call_id_type     call_id,
  byte               *system_information,
  byte                max_system_information_length
)
{
  static char               *sys_info     = "CDMA";
  boolean                    ret_val      = FALSE;
  ps_iface_type             *iface_ptr    = NULL;
  ds707_tc_state_type       *tc_state_ptr = NULL;
/*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  MSG_LOW ("ds707_pkti_get_call_conn_info_hdlr()",0,0,0);

  ds707_pkti_get_state_ptrs_cid(call_id, &tc_state_ptr, &iface_ptr);

  if ((tc_state_ptr == NULL) || (iface_ptr == NULL))
  {
    ERR("get callconn info unknown id %d",call_id, 0, 0);
    ret_val = FALSE;
  }
  else
  {
    if (max_system_information_length > strlen (sys_info))
    {
      std_strlcpy((char *)system_information, 
                  sys_info, 
                  max_system_information_length);
      ret_val = TRUE;
    }
    else
    {
      ERR (" Get callconn info: Strlen exceeded",0,0,0 );
      ret_val = FALSE;
    }
  }

  return ret_val;
} /* ds707_pkti_get_call_conn_info_hdlr() */


/*===========================================================================
FUNCTION      DS707_PKTI_GET_DATA_COUNT_HDLR

DESCRIPTION   Called by 3G DSMGR when UI invokes
              ds_get_current_session_data_counters() to get total count of
              bytes transmitted and received by RLP for the current call.

DEPENDENCIES  None.

RETURN VALUE  boolean:  indicates if the count is valid.

SIDE EFFECTS  None.
===========================================================================*/
LOCAL boolean  ds707_pkti_get_data_count_hdlr
(
  cm_call_id_type    call_id,
  uint32            *rx_from_network_byte_cnt,
  uint32            *tx_to_network_byte_cnt
)
{
  boolean                    ret_val         = FALSE;
  ps_iface_type             *iface_ptr       = NULL;
  ds707_tc_state_type       *tc_state_ptr    = NULL;
  dsrlp_stats_info_type     *dsrlp_stats_ptr = NULL;
  rlp_ext_stats_type         rlp_stats;
/*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  MSG_LOW ("ds707_pkti_get_data_count_hdlr()",0,0,0);

  ds707_pkti_get_state_ptrs_cid(call_id, &tc_state_ptr, &iface_ptr);

  if ((tc_state_ptr == NULL) || (iface_ptr == NULL))
  {
    ERR("get data count unknown id %d",call_id, 0, 0);
    ret_val = FALSE;
  }
  else
  {
    if ( tc_state_ptr->so == CAI_SO_PPP_PKT_DATA_3G )
    {
      dsrlp_get_stats (tc_state_ptr->rlp_1x.rlp_instance, &dsrlp_stats_ptr );
      if ( dsrlp_stats_ptr != NULL )
      {
        *rx_from_network_byte_cnt = dsrlp_stats_ptr->rx_total_bytes;
        *tx_to_network_byte_cnt   = dsrlp_stats_ptr->tx_total_bytes;
        ret_val = TRUE;
      }
      else
      {
        ERR ("Get data count: RLP stats ptr NULL", 0, 0, 0);
        ret_val = FALSE;
      }
    }
    else
    {
      rlp_get_stats ( &rlp_stats );
      *rx_from_network_byte_cnt = rlp_stats.rx_data_cnt;
      *tx_to_network_byte_cnt   = rlp_stats.tx_data_cnt;
      ret_val = TRUE;
    }
  }

  return ret_val;
} /* ds707_pkti_get_data_count_hdlr() */


/*===========================================================================
FUNCTION      DS707_PKTI_GET_LAST_SESS_COUNT_HDLR

DESCRIPTION   Called by 3G DSMGR when UI invokes
              ds_get_last_session_data_counters() to get total count of
              bytes transmitted and received by RLP for the last call which
              is being released or current call.

DEPENDENCIES  None.

RETURN VALUE  boolean:  indicates if the count is valid.

SIDE EFFECTS  None.
===========================================================================*/
LOCAL boolean  ds707_pkti_get_last_sess_count_hdlr
(
  uint32            *rx_from_network_byte_cnt,
  uint32            *tx_to_network_byte_cnt
)
{
  boolean                    ret_val         = FALSE;
  ds707_tc_state_type       *tc_state_ptr    = NULL;
  dsrlp_stats_info_type     *dsrlp_stats_ptr = NULL;
  rlp_ext_stats_type         rlp_stats;
/*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  MSG_LOW ("ds707_pkti_get_last_sess_count_hdlr()",0,0,0);

  tc_state_ptr = GET_TC_STATE_PTR ( DS707_DEF_PKT_INSTANCE );

  if ( ds707_pkt_prev_data_call_so == CAI_SO_PPP_PKT_DATA_3G )
  {
    dsrlp_get_stats ( tc_state_ptr->rlp_1x.rlp_instance, &dsrlp_stats_ptr );
    if ( dsrlp_stats_ptr != NULL )
    {
      *rx_from_network_byte_cnt = dsrlp_stats_ptr->rx_total_bytes;
      *tx_to_network_byte_cnt   = dsrlp_stats_ptr->tx_total_bytes;
      ret_val = TRUE;
    }
    else
    {
      ERR ("Get last sess count: RLP stats ptr NULL", 0, 0, 0);
      ret_val = FALSE;
    }
  }
#ifdef FEATURE_HDR
#error code not present
#endif /* FEATURE_HDR */
  else
  {
    rlp_get_stats ( &rlp_stats );
    *rx_from_network_byte_cnt = rlp_stats.rx_data_cnt;
    *tx_to_network_byte_cnt   = rlp_stats.tx_data_cnt;
    ret_val = TRUE;
  }

  return ret_val;
} /* ds707_pkti_get_last_sess_count_hdlr() */


/*===========================================================================
                             PHYS_LINK_DOWN
===========================================================================*/

/*===========================================================================
FUNCTION      DS707_PKTI_PHYS_LINK_DOWN_CMD_HDLR

DESCRIPTION   Called in DS TASK context in response to either:

                1) a phys down command on the 1X iface or
                2) AT+CTA timer expiring

DEPENDENCIES  Called in DS TASK context.

RETURN VALUE  None.

SIDE EFFECTS  None.
===========================================================================*/
LOCAL void ds707_pkti_phys_link_down_cmd_hdlr
(
  uint32 pkt_instance
)
{
  ds707_pkt_call_info_type     mode_spec_info;
/* - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - */
  MSG_HIGH("DS707 PKT Phys link down cmd hdlr",0,0,0);

  ASSERT(IS_IN_DS_TASK());

  VERIFY_PKT_INSTANCE(pkt_instance);
  mode_spec_info.pkt_instance = pkt_instance;

  if (ds3g_msh_hangup_call(ds707_pkt_state[pkt_instance].tc_state.call_id,
                           &mode_spec_info) == DS3G_FAILURE)
  {
    /*-----------------------------------------------------------------------
      The call has already ended.  Do nothing here, as the call_ended handler
      has been/will be called.  That will do most of the work.
    -----------------------------------------------------------------------*/
    MSG_HIGH("Call already ended",0,0,0);
  }

  /*-------------------------------------------------------------------------
    Wait for the CALL_RELEASED command from 3G DSMGR before saying phys
    link is down.  PS_IFACE should have already set the phys_link to
    "going down" when the phys_link_down_cmd was called.
  -------------------------------------------------------------------------*/
} /* ds707_pkti_phys_link_down_cmd_hdlr() */

/*===========================================================================
FUNCTION      DS707_PKTI_PHYS_LINK_DOWN

DESCRIPTION   Called via the 1x IFACE.  Tears down the traffic channel.
              Sends a message to the DS task which will cause the
              channel to be torn down.

              Specific to the zero'th 1x pkt interface.

DEPENDENCIES  None.

RETURN VALUE  None

SIDE EFFECTS  None.
===========================================================================*/
LOCAL int ds707_pkti_phys_link_down
(
  ps_iface_type    *iface_ptr,
  void             *not_used
)
{
  ds_cmd_type     *cmd_ptr;                      /* DS Task msg            */
/*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/
  MSG_HIGH("DS707 PHYS LINK DOWN CMD",0,0,0);

  if( (cmd_ptr = ds_get_cmd_buf()) == NULL )
  {
    ERR_FATAL( "DS707 cant get cmd buf from DS task", 0, 0, 0 );
  }
  else
  {
    cmd_ptr->hdr.cmd_id = DS_707_PKT_PHYS_LINK_DOWN_CMD;
    cmd_ptr->cmd.ds707_data.pkt_instance =
                                        (uint32)(iface_ptr->client_data_ptr);

    ds_put_cmd(cmd_ptr);
  }

  return(0);
} /* ds707_pkti_phys_link_down() */



/*===========================================================================
                             PHYS_LINK_UP
===========================================================================*/

/*===========================================================================
FUNCTION      DS707_PKTI_PHYS_LINK_UP_CMD_HDLR

DESCRIPTION   Called in DS TASK context in response to either:

                1) a phys up command on the 1X iface or
                2) SID/NID/PZID event.

              All traffic channel originations must come through this
              function.

DEPENDENCIES  Called in DS TASK context.  Assumes that PS_IFACE has already
              set phys_link state to COMING_UP.

RETURN VALUE  None.

SIDE EFFECTS  None.
===========================================================================*/
LOCAL void ds707_pkti_phys_link_up_cmd_hdlr
(
  boolean     drs_bit,
  boolean     use_hdown_timer,
  uint32      pkt_instance
)
{
  ds707_pkt_call_info_type     mode_spec_orig_info;
  ds707_tc_state_type          *tc_state_ptr;
  ps_iface_type               *iface_ptr;
  dsat_dial_val_e_type         dial_str_digits_type;
  byte                         dial_str[NV_MAX_PKT_ORIG_DIGITS];
  boolean  	               dial_str_digits_mode;
/* - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - */
  MSG_HIGH("DS707 PKT PHYS LINK UP CMD HDLR",0,0,0);

  ASSERT(IS_IN_DS_TASK());

  VERIFY_PKT_INSTANCE(pkt_instance);

  mode_spec_orig_info.drs_bit         = drs_bit;
  mode_spec_orig_info.use_hdown_timer = use_hdown_timer;
  mode_spec_orig_info.pkt_instance    = pkt_instance;

  tc_state_ptr = GET_TC_STATE_PTR(pkt_instance);
  iface_ptr    = GET_PS_IFACE_PTR(pkt_instance);

#ifdef FEATURE_JCDMA_1X
  /*-------------------------------------------------------------------------
    Dial strings for pkt calls in JCDMA are always zero-lenght NULL
  -------------------------------------------------------------------------*/
  dial_str_digits_type = DSAT_DIAL_EMPTY;
  dial_str[0] = '\0';
#else
  #ifndef FEATURE_DATA_STRIP_ATCOP
  dial_str_digits_type = dsat_proc_dial_str(dsat707_pkt_orig_str,
                                            dial_str);
  #else
  /* Directly copying #777 in the dial string */
  dial_str_digits_type = DSAT_DIAL_DIGIT;
  std_strlcpy((char *) dial_str, 
              (char *) dsat707_pkt_orig_str, 
              strlen((char *)dsat707_pkt_orig_str) + 1);
  #endif
#endif

  ds707_pkti_reg_rlp_srvc(tc_state_ptr);

  /*-------------------------------------------------------------------
    Determine the digit mode
    TRUE = dial str has non-digit chars
    FALSE = dial str has only digits
  -------------------------------------------------------------------*/

  if ((dial_str_digits_type == DSAT_DIAL_DIGIT) || 
      (dial_str_digits_type == DSAT_DIAL_EMPTY))
  {
    dial_str_digits_mode = FALSE;
  }
  else
  {
    dial_str_digits_mode = TRUE;
  }
  
  if (ds3g_msh_initiate_call(DS_CALL_TYPE_PKT,
                             SYS_SYS_MODE_CDMA,
                             (const byte *)(dial_str),
                             dial_str_digits_mode,
                             &mode_spec_orig_info
                            ) == DS3G_FAILURE)
  {
    ERR("Could not originate pkt call",0,0,0);
    ds707_pkti_phys_link_cleanup(tc_state_ptr, iface_ptr, TRUE, TRUE);
    ps_iface_phys_link_down_ind(iface_ptr);

    /*-------------------------------------------------------------------
      Let CM/UI know that the packet call has gone dormant
    -------------------------------------------------------------------*/
    ds3g_msh_set_pkt_state(CM_PACKET_STATE_DORMANT);
#ifdef FEATURE_JCDMA_1X
    ds707_so_pkt_recal();
#endif
  }
  else
  {
    /*-----------------------------------------------------------------------
      We are originating a call.  Clear the enqueue func ptrs, in case this
      is a dormant reorig.  Clear the re-orig sig, as it's already started.

      Also clear the holddown timer, in case it's still running.  Also
      re-set the holddown_timer_enable variable for the next dormant
      session.
    -----------------------------------------------------------------------*/
    ds707_pkti_dereg_wmk_reorig_cbacks(tc_state_ptr);

    MSG_HIGH("Stop holddown timer",0,0,0);
    ds3gi_stop_timer(DS3G_TIMER_HOLDDOWN);
    ps_iface_enable_flow(iface_ptr, DS_HOLDDOWN_TIMER_MASK);

    ds707_pkt_enable_holddown_timer(TRUE);
  }
} /* ds707_pkti_phys_link_up_cmd_hdlr() */

/*===========================================================================
FUNCTION      DS707_PKTI_PHYS_LINK_UP

DESCRIPTION   Called via the 1x IFACE.  Brings up the 1X traffic channel.
              Specific to the zero'th 1X pkt interface.

              Since this is called by some module external to 1X, DRS_BIT
              is always set to TRUE (always data to be sent).

DEPENDENCIES  Watermarks to RLP already set up correctly.

RETURN VALUE  None.

SIDE EFFECTS  None.
===========================================================================*/
LOCAL int ds707_pkti_phys_link_up
(
  ps_iface_type    *iface_ptr,
  void             *info_ptr
)
{
  ds_cmd_type               *cmd_ptr;                     /* DS Task msg            */
  ds707_pkt_call_info_type  *pkt_info_ptr;
/*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  pkt_info_ptr = (ds707_pkt_call_info_type *)info_ptr;

  if( (cmd_ptr = ds_get_cmd_buf()) == NULL )
  {
    ERR_FATAL( "DS707 can't get cmd buf from DS task", 0, 0, 0 );
  }
  else
  {
    cmd_ptr->hdr.cmd_id = DS_707_PKT_PHYS_LINK_UP_CMD;
    cmd_ptr->cmd.ds707_data.pkt_instance =
                                        (uint32)(iface_ptr->client_data_ptr);

    if (pkt_info_ptr == NULL)
    {
      /*---------------------------------------------------------------------
        In the default case, we have data to send, and we should pay
        attention to the holddown timer
      ---------------------------------------------------------------------*/
      cmd_ptr->cmd.ds707_data.drs_bit         = TRUE;
      cmd_ptr->cmd.ds707_data.use_hdown_timer = TRUE;
    }
    else
    {
      cmd_ptr->cmd.ds707_data.drs_bit         = pkt_info_ptr->drs_bit;
      cmd_ptr->cmd.ds707_data.use_hdown_timer = pkt_info_ptr->use_hdown_timer;
    }

    MSG_HIGH("Phys link up cmd DRS %d Use Hdown %d",
              cmd_ptr->cmd.ds707_data.drs_bit,
              cmd_ptr->cmd.ds707_data.use_hdown_timer,
              0
            );

    ds_put_cmd(cmd_ptr);
  }

  return(0);
} /* ds707_pkti_phys_link_up */

/*===========================================================================
                            IFACE_ROUTEABLE
===========================================================================*/
/*===========================================================================
FUNCTION      DS707_PKTI_IFACE_ROUTEABLE_IND_CBACK

DESCRIPTION   Called when the IFACE goes routeable.  This will re-register the
              SID/NID/PZID recon.
              
              Note that in certain cases (especially with MobIP), the IFACE
              can switch back and forth from ROUTEABLE and UP many times
              during a single data session.

DEPENDENCIES  Registered once, at startup.

RETURN VALUE  None.

SIDE EFFECTS  None.
===========================================================================*/
LOCAL void ds707_pkti_iface_routeable_ind_cback
(
  ps_iface_type                *iface_ptr,
  ps_iface_event_enum_type      event,
  ps_iface_event_info_u_type    event_info,
  void                         *user_info_ptr
)
{
  ds_cmd_type      *cmd_ptr;                     /* DS Task msg            */
/* - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - */
  MSG_HIGH("DS707 IFACE routeable ind cback",0,0,0);

  /*-----------------------------------------------------------------------
    Could get this callback on registration.  Make sure we are
    transitioning to iface_up state, as this is only registered at
    startup.
  -----------------------------------------------------------------------*/
  if (event_info.state == IFACE_ROUTEABLE)
  {
    MSG_HIGH("Ignore iface_routeable_ind",0,0,0);
    return;
  }

  if( (cmd_ptr = ds_get_cmd_buf()) == NULL )
  {
    ERR_FATAL( "DS707 can't get cmd buf from DS task", 0, 0, 0 );
  }
  else
  {
    cmd_ptr->hdr.cmd_id = DS_707_PKT_IFACE_ROUTEABLE_IND_CBACK_CMD;
    cmd_ptr->cmd.ds707_data.pkt_instance =
                                        (uint32)(iface_ptr->client_data_ptr);

    ds_put_cmd(cmd_ptr);
  }
} /* ds707_pkti_iface_routeable_ind_cback() */

/*===========================================================================
FUNCTION      DS707_PKTI_IFACE_ROUTEABLE_IND_CBACK_CMD_HDLR

DESCRIPTION   Called when the IFACE ROUTEABLE CBACK cmd is processed.  This
              registers the PZID, SID, and NID reconnection handlers.

              Note that in certain cases (especially with MobIP), the IFACE
              can switch back and forth from ROUTEABLE and UP many times
              during a single data session.

DEPENDENCIES  Registered once, at startup.

RETURN VALUE  None.

SIDE EFFECTS  None.
===========================================================================*/
LOCAL void ds707_pkti_iface_routeable_ind_cback_cmd_hdlr
(
  uint32        pkt_instance
)
{
/* - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - */
  MSG_HIGH("ds707 pkt iface routeable ind cmd hdlr",0,0,0);

  ASSERT(IS_IN_DS_TASK());

  VERIFY_PKT_INSTANCE(pkt_instance);

  /*-----------------------------------------------------------------------
    When IFACE goes routeable, register PZID reconnection handler with MC.
    Do this so PZID doesn't collect PZIDs in its table while NULL.  Also
    register SID and NID.
  -----------------------------------------------------------------------*/
  ds707_epzid_register();
} /* ds707_pkti_iface_routeable_cback_cmd_hdlr() */

/*===========================================================================
                                IFACE_UP
===========================================================================*/

/*===========================================================================
FUNCTION      DS707_PKTI_IFACE_UP_CMD_HDLR

DESCRIPTION   Called in DS TASK context in response to an iface up command
              on the 1X iface.  Currently just adds a default route.

DEPENDENCIES  Called in DS TASK context.

RETURN VALUE  None.

SIDE EFFECTS  None.
===========================================================================*/
LOCAL void ds707_pkti_iface_up_cmd_hdlr
(
  uint32        pkt_instance
)
{
  ip_addr_type      def_addr;
  ip_addr_type      zero_addr;
#ifdef FEATURE_DS_MULTIPLE_PROFILES
  nv_stat_enum_type nv_status;             /* status from NV call          */
  nv_item_type      nv_item;
#endif
/*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  VERIFY_PKT_INSTANCE(pkt_instance);
  ASSERT(IS_IN_DS_TASK());

#ifdef FEATURE_DS_MULTIPLE_PROFILES
  nv_item.ds_sip_active_profile_index = 
                        ds707_data_session[pkt_instance].current_profile_id;
  nv_status = dsi_put_nv_item( NV_DS_SIP_ACTIVE_PROFILE_INDEX_I, &nv_item );

  if( nv_status != NV_DONE_S )
  {
    MSG_ERROR( "Active Profile Index NV write failed", 0, 0, 0 );
  }
#endif

#ifdef FEATURE_DS_SOCKETS      /* UNFINISHED */
  dssnet_sm_post_event(DSSNET_PPP_OPEN_CMD_EV);
#endif /* FEATURE_DS_SOCKETS */

  /* Add a default route via this interface */
  def_addr.type = IPV4_ADDR;
  def_addr.addr.v4 = ROUTE_DEFAULT_DST;
  zero_addr.type = IPV4_ADDR;
  zero_addr.addr.v4 = 0x0;

  if ( route_add(ROUTE_DEFAULT,
                 def_addr,
                 zero_addr,
                 zero_addr,
                 GET_PS_IFACE_PTR(pkt_instance),
                 1,
                 NULL)
       == -1 )
  {
    MSG_ERROR("1x default route failed",0,0,0);
  }

} /* ds707_pkti_iface_up_cmd_hdlr() */

/*===========================================================================
FUNCTION      DS707_PKTI_IFACE_UP

DESCRIPTION   Called via the 1x IFACE.  Sends a command to DS task to bring
              up the 1X iface.

DEPENDENCIES  None

RETURN VALUE  None.

SIDE EFFECTS  None.
===========================================================================*/
LOCAL int ds707_pkti_iface_up
(
  ps_iface_type    *iface_ptr,
  void             *not_used
)
{
  ds_cmd_type      *cmd_ptr;                     /* DS Task msg            */
/*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/
  MSG_HIGH("ds707 pkt iface up cmd",0,0,0);

  if( (cmd_ptr = ds_get_cmd_buf()) == NULL )
  {
    ERR_FATAL( "DS707 can't get cmd buf from DS task", 0, 0, 0 );
  }
  else
  {
    cmd_ptr->hdr.cmd_id = DS_707_PKT_IFACE_UP_CMD;
    cmd_ptr->cmd.ds707_data.pkt_instance =
                                        (uint32)(iface_ptr->client_data_ptr);

#ifdef FEATURE_DS_MULTIPLE_PROFILES
    ds707_data_session_set_current_profile(iface_ptr);
#endif

    ds_put_cmd(cmd_ptr);
  }

  return(0);
} /* ds707_pkti_iface_up() */

/*===========================================================================
FUNCTION      DS707_PKTI_IFACE_UP_IND_CBACK

DESCRIPTION   Called when the IFACE comes up.  This will re-register the
              PZID, SID, and NID reconnection handlers with MC.

              Note that in certain cases (especially with MobIP), the IFACE
              can switch back and forth from ROUTEABLE and UP many times
              during a single data session.
              
DEPENDENCIES  Registered once, at startup.

RETURN VALUE  None.

SIDE EFFECTS  None.
===========================================================================*/
LOCAL void ds707_pkti_iface_up_ind_cback
(
  ps_iface_type                *iface_ptr,
  ps_iface_event_enum_type      event,
  ps_iface_event_info_u_type    event_info,
  void                         *user_info_ptr
)
{
  ds_cmd_type      *cmd_ptr;                     /* DS Task msg            */
/* - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - */
  MSG_HIGH("DS707 IFACE up ind cback",0,0,0);

  /*-----------------------------------------------------------------------
    Could get this callback on registration.  Make sure we are
    transitioning to iface_up state, as this is only registered at
    startup.
  -----------------------------------------------------------------------*/
  if (event_info.state == IFACE_UP)
  {
    MSG_HIGH("Ignore iface_up_ind",0,0,0);
    return;
  }

  if( (cmd_ptr = ds_get_cmd_buf()) == NULL )
  {
    ERR_FATAL( "DS707 can't get cmd buf from DS task", 0, 0, 0 );
  }
  else
  {
    cmd_ptr->hdr.cmd_id = DS_707_PKT_IFACE_UP_IND_CBACK_CMD;
    cmd_ptr->cmd.ds707_data.pkt_instance =
                                        (uint32)(iface_ptr->client_data_ptr);

    ds_put_cmd(cmd_ptr);
  }
} /* ds707_pkti_iface_up_ind_cback() */

/*===========================================================================
FUNCTION      DS707_PKTI_IFACE_UP_IND_CBACK_CMD_HDLR

DESCRIPTION   Called when the IFACE UP_IND CBACK cmd is processed.  This
              re-registers the SID, NID, and PZID handlers.

              Note that in certain cases (especially with MobIP), the IFACE
              can switch back and forth from ROUTEABLE and UP many times
              during a single data session.
              
DEPENDENCIES  Registered once, at startup.

RETURN VALUE  None.

SIDE EFFECTS  None.
===========================================================================*/
LOCAL void ds707_pkti_iface_up_ind_cback_cmd_hdlr
(
  uint32        pkt_instance
)
{
/* - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - */
  MSG_HIGH("ds707 pkt iface up ind cmd hdlr",0,0,0);

  ASSERT(IS_IN_DS_TASK());

  VERIFY_PKT_INSTANCE(pkt_instance);

  /*-----------------------------------------------------------------------
    When IFACE comes up, register PZID reconnection handler with MC.  Do
    this so PZID doesn't collect PZIDs in its table while NULL.
  -----------------------------------------------------------------------*/
  ds707_epzid_register();  
  
} /* ds707_pkti_iface_up_ind_cback_cmd_hdlr() */

/*===========================================================================
                                IFACE_DOWN
===========================================================================*/
/*===========================================================================
FUNCTION      DS707_PKTI_IFACE_DOWN_CMD_HDLR

DESCRIPTION   Called in DS TASK context in response to an iface down command
              on the 1X iface.

DEPENDENCIES  Called in DS TASK context.  Called only when PS_IFACE is up.

RETURN VALUE  None.

SIDE EFFECTS  None.
===========================================================================*/
LOCAL void ds707_pkti_iface_down_cmd_hdlr
(
  uint32        pkt_instance
)
{
/*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/
  MSG_HIGH("In pkt707 iface down hdlr",0,0,0);

  ASSERT(IS_IN_DS_TASK());

  VERIFY_PKT_INSTANCE(pkt_instance);

  /*-------------------------------------------------------------------------
    Need to tear down PPP layer.  When done, PPP will notify DS (dssnet) that
    it is done, at which point it will be safe to bring down the traffic
    channel.
  -------------------------------------------------------------------------*/
#ifdef FEATURE_DS_SOCKETS
  dssnet_sm_post_event(DSSNET_PPP_CLOSE_CMD_EV);
#endif /* FEATURE_DS_SOCKETS */

  /*-------------------------------------------------------------------------
   Reset the pzid hysteresis state machine. 
  -------------------------------------------------------------------------*/
#ifdef FEATURE_DS_PZID_HYSTERESIS
  ds707_pzid_hysteresis_process_event( PZID_HYS_NULL_EVENT );
#else
  ds707_epzid_hysteresis_process_event( EPZID_HYS_NULL_EVENT );
#endif /* FEATURE_DS_PZID_HYSTERESIS */
  /*--------------------------------------------------------------
    Since we are terminating the data session, deregister the
    callback with RLP3 to notify the EPZID hysterisis engine of 
    data transfer (if it was previously registered).

    Since the the PPP data transfer notify function will be called
    in the Rx task context and we are setting it to NULL here in 
    a lower priority (DS) task context, I don't think we need a 
    TASKLOCK. Add a TASKLOCK here if needed.
  --------------------------------------------------------------*/
  dsrlp_reg_ppp_data_transfer_notify_func
  (
    DSRLP_FIXED_SR_ID_INDEX,
    NULL
  );

} /* ds707_pkti_iface_down_cmd_hdlr() */

/*===========================================================================
FUNCTION      DS707_PKTI_IFACE_DOWN

DESCRIPTION   Called via the 1x IFACE.  Tears down the 1X iface.

DEPENDENCIES  None

RETURN VALUE  None.

SIDE EFFECTS  None.
===========================================================================*/
LOCAL int ds707_pkti_iface_down
(
  ps_iface_type    *iface_ptr,
  void             *not_used
)
{
  ds_cmd_type      *cmd_ptr;                     /* DS Task msg            */
/*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/
  MSG_HIGH("DS707 pkt IFACE down cmd",0,0,0);

  if( (cmd_ptr = ds_get_cmd_buf()) == NULL )
  {
    ERR_FATAL( "DS707 can't get cmd buf from DS task", 0, 0, 0 );
  }
  else
  {
    cmd_ptr->hdr.cmd_id = DS_707_PKT_IFACE_DOWN_CMD;
    cmd_ptr->cmd.ds707_data.pkt_instance =
                                        (uint32)(iface_ptr->client_data_ptr);

#ifdef FEATURE_DS_MULTIPLE_PROFILES
    ds707_data_session_info_init(iface_ptr);
#endif

    ds_put_cmd(cmd_ptr);
  }
  return(0);
} /* ds707_pkti_iface_down() */

/*===========================================================================
FUNCTION      DS707_PKTI_IFACE_DOWN_IND_CBACK

DESCRIPTION   Called when the IFACE goes down.  This cancels the re-orig
              holddown timer and may also cause the phys link to go down.

DEPENDENCIES  Registered once, at startup.

RETURN VALUE  None.

SIDE EFFECTS  None.
===========================================================================*/
LOCAL void ds707_pkti_iface_down_ind_cback
(
  ps_iface_type                *iface_ptr,
  ps_iface_event_enum_type      event,
  ps_iface_event_info_u_type    event_info,
  void                         *user_info_ptr
)
{
  ds_cmd_type      *cmd_ptr;                     /* DS Task msg            */
/* - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - */
  MSG_HIGH("DS707 IFACE down ind cback",0,0,0);

  /*-----------------------------------------------------------------------
    Could get this callback on registration.  Make sure we are
    transitioning to iface_down state, as this is only registered at
    startup.
  -----------------------------------------------------------------------*/
  if (event_info.state == IFACE_DOWN)
  {
    MSG_HIGH("Ignore iface_down_ind",0,0,0);
    return;
  }

  if( (cmd_ptr = ds_get_cmd_buf()) == NULL )
  {
    ERR_FATAL( "DS707 can't get cmd buf from DS task", 0, 0, 0 );
  }
  else
  {
    cmd_ptr->hdr.cmd_id = DS_707_PKT_IFACE_DOWN_IND_CBACK_CMD;
    cmd_ptr->cmd.ds707_data.pkt_instance =
                                        (uint32)(iface_ptr->client_data_ptr);

    ds_put_cmd(cmd_ptr);
  }
} /* ds707_pkti_iface_down_ind_cback() */

/*===========================================================================
FUNCTION      DS707_PKTI_IFACE_DOWN_IND_CBACK_CMD_HDLR

DESCRIPTION   Called when the IFACE DOWN CBACK cmd is processed.  This
              cancels the re-orig holddown timer and may also cause the
              phys link to go down.

              UNFINISHED - tear down PPP?

DEPENDENCIES  Registered once, at startup.

RETURN VALUE  None.

SIDE EFFECTS  None.
===========================================================================*/
LOCAL void ds707_pkti_iface_down_ind_cback_cmd_hdlr
(
  uint32        pkt_instance
)
{
  ps_iface_type             *iface_ptr;        /* ptr to 1x iface          */
/* - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - */
  MSG_HIGH("ds707 pkt iface down ind cmd hdlr",0,0,0);

  ASSERT(IS_IN_DS_TASK());

  VERIFY_PKT_INSTANCE(pkt_instance);
  iface_ptr    = GET_PS_IFACE_PTR(pkt_instance);

  /*-----------------------------------------------------------------------
    Take the default route through this interface out
  -----------------------------------------------------------------------*/
  route_flush( iface_ptr );

  /*-----------------------------------------------------------------------
    Abort PPP.
  -----------------------------------------------------------------------*/
  if (dsat707_crm_val == DS_CRM_VAL_NETMODEL_MODE)
  {
    ppp_abort( PPP_UM_SN_DEV );
  }

  /*-----------------------------------------------------------------------
    Stop holddown timer.  Reset holddown timer also.
  -----------------------------------------------------------------------*/
  MSG_HIGH("Stop & reset holddown timer",0,0,0);
  ds707_dorm_timer_reset_val();
  ds3gi_stop_timer(DS3G_TIMER_HOLDDOWN);
  ps_iface_enable_flow(iface_ptr, DS_HOLDDOWN_TIMER_MASK);

  /*-----------------------------------------------------------------------
    Post a go null event to the EPZID hysteresis engine. This 
    should stop the HAT if it has already been running.
  -----------------------------------------------------------------------*/
#ifdef FEATURE_DS_PZID_HYSTERESIS
      ds707_pzid_hysteresis_process_event(PZID_HYS_GO_NULL_EVENT);
#else
  ds707_epzid_hysteresis_process_event(EPZID_HYS_GO_NULL_EVENT);
#endif /* FEATURE_DS_PZID_HYSTERESIS */
  /*-----------------------------------------------------------------------
    For sanity, clean default RLP watermarks.  Do not need to clean up
    the watermarks that were assigned to us;  if they were the SIO
    watermarks, then chaning SIO back to AUTODETECT cleans those up.  RMSM
    cleans up the rx_q associated with that scenario also.  So only clean
    up the PS-to-RLP watermarks.
  -----------------------------------------------------------------------*/
  ds707_wmk_cleanup_wmks();

  /*-----------------------------------------------------------------------
    When IFACE goes down, deregister PZID reconnection handler with MC.
    Also deregister SID and NID reconnections.
  -----------------------------------------------------------------------*/
  ds707_epzid_dereg_recon();

#ifdef FEATURE_JCDMA_1X
  ds707_so_pkt_recal();
#endif
} /* ds707_pkti_iface_down_ind_cback_cmd_hdlr() */


/*===========================================================================
FUNCTION      DS707_PKT_IOCTL

DESCRIPTION   Called by ps_iface to do ioctls specific to 1x pkt iface.

DEPENDENCIES  Registered once, at startup.

RETURN VALUE   0 - on success
              -1 - on failure

SIDE EFFECTS  None.
===========================================================================*/
LOCAL int ds707_pkt_ioctl
(
  ps_iface_type            *iface_ptr,
  dss_iface_ioctl_type     ioctl_name,
  void                     *argval_ptr,
  sint15                   *ps_errno
)
{
  ds707_pkt_call_info_type  pkt_info;
  int16                     ioctl_errno;
  int16                     ret_val;
  ds707_tc_state_type      *tc_state_ptr;
  uint32                    pkt_instance = DS707_DEF_PKT_INSTANCE;
  unsigned int                                tmp_mdr_val;
  unsigned int                                tmp_cta_val;
  uint8                                       tmp_fill_rate;
  dss_iface_ioctl_707_rlp_opt_type           *tmp_rlp_opt_ptr;
#ifdef FEATURE_IS2000_REL_A
  byte                                        tmp_qos_val;
#endif /* FEATURE_IS2000_REL_A */
#ifdef FEATURE_DATA_IS2000_SDB
  dss_iface_ioctl_707_sdb_support_query_type *tmp_sdb_query_ptr;
  uint32                                      tmp_sdb_flags;
#endif /* FEATURE_DATA_IS2000_SDB */
  boolean                   tmp_enable_holddown_val = TRUE;
/*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  MSG_MED ("707 pkt ioctl: %d", ioctl_name, 0, 0);

  *ps_errno = 0;

  pkt_instance = (uint32) ( iface_ptr->client_data_ptr );
  VERIFY_PKT_INSTANCE(pkt_instance);

  tc_state_ptr = GET_TC_STATE_PTR ( pkt_instance );

  /*-------------------------------------------------------------------------
    Based on the ioctl name, perform the appropriate actions.
  -------------------------------------------------------------------------*/
  switch (ioctl_name)
  {
    /*-----------------------------------------------------------------------
      Go active.
    -----------------------------------------------------------------------*/
    case DSS_IFACE_IOCTL_GO_ACTIVE:
      MSG_HIGH ("707 pkt ioctl - go active", 0, 0, 0);
      #ifndef FEATURE_ASYNC_DATA_NOOP
      if (ds707_so_async_so_is_async(tc_state_ptr->so) == TRUE)
      {
        ERR ("ioctl goactive fail. Async so %d", tc_state_ptr->so, 0, 0 );
        *ps_errno = DS_EFAULT;
        return -1;
      }
      #endif
      /*---------------------------------------------------------------------
        This struct is passed onto the iface.
      ---------------------------------------------------------------------*/
      pkt_info.drs_bit         = TRUE;
      pkt_info.use_hdown_timer = TRUE;
      if (ds707_pkti_ok_orig_from_dorm(iface_ptr) == TRUE)
      {
        if ((DS_FLOW_CTRL_IS_BIT_SET ( ps_iface_get_flow_mask ( iface_ptr ),
                                       DS_HOLDDOWN_TIMER_MASK )) == FALSE )
        {
          ret_val = ps_iface_phys_link_up_cmd ( iface_ptr,
                                                &ioctl_errno,
                                                &pkt_info );
          if ((ret_val < 0) && ( ioctl_errno != DS_EWOULDBLOCK))
          {
            ERR("ioctl goactive fail,%d phys_link ret" , ioctl_errno, 0, 0);
            *ps_errno = DS_EFAULT;
            return -1;
          }
        }
        else
        {
          ERR ("ioctl goactive fail holddown timer", 0, 0, 0);
          *ps_errno = DS_EFAULT;
          return -1;
        }
      }
      else
      {
        MSG_HIGH("ioctl goactive fail iface %d phys %d",
                ps_iface_state(iface_ptr),
                ps_iface_phys_link_state(iface_ptr),
                0
               );
        *ps_errno = DS_EFAULT;
        return -1;
      }
      break;


    /*-----------------------------------------------------------------------
      Go dormant.
    -----------------------------------------------------------------------*/
    case DSS_IFACE_IOCTL_GO_DORMANT:
      MSG_HIGH ("707 pkt ioctl - go dormant", 0, 0, 0);
      if ( ds707_pkti_can_go_dormant ( tc_state_ptr, iface_ptr ) == TRUE )
      {
        ret_val = ps_iface_phys_link_down_cmd ( iface_ptr,
                                                &ioctl_errno,
                                                NULL );
        if ((ret_val < 0) && ( ioctl_errno != DS_EWOULDBLOCK))
        {
          ERR("ioctl godorm fail, %d phys_link ret", ioctl_errno, 0, 0);
          *ps_errno = DS_EFAULT;
          return -1;
        }
      }
      else
      {
        ERR ("ioctl godorm fail iface%d phys%d, so%d",
             ps_iface_state(iface_ptr),
             ps_iface_phys_link_state(iface_ptr),
             tc_state_ptr->so
            );
        *ps_errno = DS_EFAULT;
        return -1;
      }
      break;

    /*-----------------------------------------------------------------------
      Go Null.
    -----------------------------------------------------------------------*/
    case DSS_IFACE_IOCTL_GO_NULL:
      MSG_HIGH ("707 pkt ioctl - go NULL", 0, 0, 0);
      ret_val = ps_iface_tear_down_cmd ( iface_ptr, &ioctl_errno, NULL );
      if ((ret_val < 0) && (ioctl_errno != DS_EWOULDBLOCK))
      {
        ERR("ioctl gonull fail, %d iface_tear ret", ioctl_errno, 0, 0);
        *ps_errno = DS_EFAULT;
        return -1;
      }
      break;

    /*-----------------------------------------------------------------------
      Get QCMDR value.
    -----------------------------------------------------------------------*/
    case DSS_IFACE_IOCTL_707_GET_MDR:
      MSG_HIGH ("707 pkt ioctl - get mdr=%d", dsat707_qcmdr_val, 0, 0);
      if ( argval_ptr != NULL )
      {
      (* (unsigned int *) argval_ptr ) = dsat707_qcmdr_val;
      }
      else
      {
        ERR("ioctl getmdr failed. Null arg ptr", 0, 0, 0);
        *ps_errno = DS_EFAULT;
        return -1;
      }
      break;

    /*-----------------------------------------------------------------------
      Set QCMDR value.
    -----------------------------------------------------------------------*/
    case DSS_IFACE_IOCTL_707_SET_MDR:
      if ( argval_ptr != NULL )
      {
      tmp_mdr_val = (* (unsigned int *) argval_ptr );
      }
      else
      {
        ERR("ioctl setmdr failed. Null arg ptr", 0, 0, 0);
        *ps_errno = DS_EFAULT;
        return -1;
      }
      MSG_HIGH ("707 pkt ioctl - set mdr=%d", tmp_mdr_val, 0, 0);
      if ( tmp_mdr_val > (DS_MDR_MODE_MAX -1)  )

      {
        ERR("ioctl set mdr fail %d out of range", tmp_mdr_val, 0, 0);
        *ps_errno = DS_EFAULT;
        return -1;
      }
      else
      {
        if ( dsat707_set_mdr_val ( tmp_mdr_val ) == FALSE )
        {
          ERR("dsat707_set_mdr_val(%d) fail ", tmp_mdr_val, 0, 0);
          *ps_errno = DS_EFAULT;
          return -1;
        }
      }
      break;

    /*-----------------------------------------------------------------------
      Get dorm timer value.
    -----------------------------------------------------------------------*/
    case DSS_IFACE_IOCTL_707_GET_DORM_TIMER:
      MSG_HIGH ("707 pkt ioctl - get dorm=%d", dsat707_cta_val, 0, 0);
      if ( argval_ptr != NULL )
      {
      (* (unsigned int *) argval_ptr ) = dsat707_cta_val;
      }
      else
      {
        ERR("ioctl getdorm failed. Null arg ptr", 0, 0, 0);
        *ps_errno = DS_EFAULT;
        return -1;
      }
      break;

    /*-----------------------------------------------------------------------
      Set dorm timer value.
    -----------------------------------------------------------------------*/
    case DSS_IFACE_IOCTL_707_SET_DORM_TIMER:
      if ( argval_ptr != NULL )
      {
      tmp_cta_val = (* (unsigned int *) argval_ptr );
      }
      else
      {
        ERR("ioctl setdorm failed. Null arg ptr", 0, 0, 0);
        *ps_errno = DS_EFAULT;
        return -1;
      }
      MSG_HIGH ("707 pkt ioctl - set dorm=%d", tmp_cta_val, 0, 0);
      if ( tmp_cta_val >  255  )
      {
        ERR("ioctl set dorm fail %d out of range", tmp_cta_val, 0, 0);
        *ps_errno = DS_EFAULT;
        return -1;
      }
      else
      {
        dsat707_cta_val = tmp_cta_val;
      }
      break;

    /*-----------------------------------------------------------------------
      Get RLP current NAK policy.
    -----------------------------------------------------------------------*/
    case DSS_IFACE_IOCTL_707_GET_RLP_ALL_CURR_NAK:
      MSG_HIGH ("707 pkt ioctl - get rlp curr nak", 0, 0, 0);
      if ( argval_ptr != NULL )
      {
      tmp_rlp_opt_ptr = ( dss_iface_ioctl_707_rlp_opt_type *) argval_ptr;
      }
      else
      {
        ERR("ioctl get rlp curr nak failed. Null arg ptr", 0, 0, 0);
        *ps_errno = DS_EFAULT;
        return -1;
      }

      if ( dsrlp_get_all_cur_naks (
                                   tmp_rlp_opt_ptr->rscb_index,
                         (byte *)&(tmp_rlp_opt_ptr->nak_rounds_fwd),
                         (byte *)&(tmp_rlp_opt_ptr->naks_per_round_fwd),
                         (byte *)&(tmp_rlp_opt_ptr->nak_rounds_rev),
                         (byte *)&(tmp_rlp_opt_ptr->naks_per_round_rev)
                                  ) == FALSE )
      {
        ERR("ioctl get rlp curr nak fail", 0, 0, 0);
        *ps_errno = DS_EFAULT;
        return -1;
      }
      break;

    /*-----------------------------------------------------------------------
      Set RLP current NAK policy.
    -----------------------------------------------------------------------*/
    case DSS_IFACE_IOCTL_707_SET_RLP_ALL_CURR_NAK:
      MSG_HIGH ("707 pkt ioctl - set rlp curr nak", 0, 0, 0);
      if ( argval_ptr != NULL )
      {
      tmp_rlp_opt_ptr = ( dss_iface_ioctl_707_rlp_opt_type *) argval_ptr;
      }
      else
      {
        ERR("ioctl set rlp curr nak failed. Null arg ptr", 0, 0, 0);
        *ps_errno = DS_EFAULT;
        return -1;
      }
      if (dsrlp_set_all_cur_naks(
                                  tmp_rlp_opt_ptr->rscb_index,
                        (byte *)&(tmp_rlp_opt_ptr->nak_rounds_fwd),
                        (byte *)&(tmp_rlp_opt_ptr->naks_per_round_fwd),
                        (byte *)&(tmp_rlp_opt_ptr->nak_rounds_rev),
                        (byte *)&(tmp_rlp_opt_ptr->naks_per_round_rev)
                                ) == FALSE )
      {
        ERR("ioctl set rlp curr nak fail", 0, 0, 0);
        *ps_errno = DS_EFAULT;
        return -1;
      }
      break;

    /*-----------------------------------------------------------------------
      Get RLP default NAK policy.
    -----------------------------------------------------------------------*/
    case DSS_IFACE_IOCTL_707_GET_RLP_ALL_DEF_NAK:
      MSG_HIGH ("707 pkt ioctl - get rlp default nak", 0, 0, 0);
      if ( argval_ptr != NULL )
      {
      tmp_rlp_opt_ptr = ( dss_iface_ioctl_707_rlp_opt_type *) argval_ptr;
      }
      else
      {
        ERR("ioctl get rlp def nak failed. Null arg ptr", 0, 0, 0);
        *ps_errno = DS_EFAULT;
        return -1;
      }
      if ( dsrlp_get_all_def_naks (
                         (byte *)&(tmp_rlp_opt_ptr->nak_rounds_fwd),
                         (byte *)&(tmp_rlp_opt_ptr->naks_per_round_fwd),
                         (byte *)&(tmp_rlp_opt_ptr->nak_rounds_rev),
                         (byte *)&(tmp_rlp_opt_ptr->naks_per_round_rev)
                                   ) == FALSE )
      {
        ERR("ioctl get rlp default nak fail", 0, 0, 0);
        *ps_errno = DS_EFAULT;
        return -1;
      }
      break;

    /*-----------------------------------------------------------------------
      Set RLP default NAK policy.
    -----------------------------------------------------------------------*/
    case DSS_IFACE_IOCTL_707_SET_RLP_ALL_DEF_NAK:
      MSG_HIGH ("707 pkt ioctl - set rlp default nak", 0, 0, 0);
      if ( argval_ptr != NULL )
      {
      tmp_rlp_opt_ptr = ( dss_iface_ioctl_707_rlp_opt_type *) argval_ptr;
      }
      else
      {
        ERR("ioctl set rlp def nak failed. Null arg ptr", 0, 0, 0);
        *ps_errno = DS_EFAULT;
        return -1;
      }

      if ( dsrlp_set_all_def_naks (
                         (byte *)&(tmp_rlp_opt_ptr->nak_rounds_fwd),
                         (byte *)&(tmp_rlp_opt_ptr->naks_per_round_fwd),
                         (byte *)&(tmp_rlp_opt_ptr->nak_rounds_rev),
                         (byte *)&(tmp_rlp_opt_ptr->naks_per_round_rev)
                                  ) == FALSE )
      {
        ERR("ioctl set rlp default nak fail", 0, 0, 0);
        *ps_errno = DS_EFAULT;
        return -1;
      }
      break;

    /*-----------------------------------------------------------------------
      Get RLP negotiated NAK policy.
    -----------------------------------------------------------------------*/
    case DSS_IFACE_IOCTL_707_GET_RLP_ALL_NEG_NAK:
      MSG_HIGH ("707 pkt ioctl - get rlp neg nak", 0, 0, 0);
      if ( argval_ptr != NULL )
      {
      tmp_rlp_opt_ptr = ( dss_iface_ioctl_707_rlp_opt_type *) argval_ptr;
      }
      else
      {
        ERR("ioctl get rlp neg nak failed. Null arg ptr", 0, 0, 0);
        *ps_errno = DS_EFAULT;
        return -1;
      }

      if ( dsrlp_get_all_neg_naks(
                         (byte *)&(tmp_rlp_opt_ptr->nak_rounds_fwd),
                         (byte *)&(tmp_rlp_opt_ptr->naks_per_round_fwd),
                         (byte *)&(tmp_rlp_opt_ptr->nak_rounds_rev),
                         (byte *)&(tmp_rlp_opt_ptr->naks_per_round_rev)
                                 ) == FALSE )
      {
        ERR("ioctl get rlp negotiated nak fail", 0, 0, 0);
        *ps_errno = DS_EFAULT;
        return -1;
      }
      break;


    /*-----------------------------------------------------------------------
      Get RLP current FILL rate.
    -----------------------------------------------------------------------*/
    case DSS_IFACE_IOCTL_707_GET_RLP_FILL_RATE:
      MSG_HIGH ("707 pkt ioctl - get rlp fill rate", 0, 0, 0);
      if ( argval_ptr != NULL )
      {
        (* (uint8 *) argval_ptr ) = dsrlp_last_fill_nak_gap;
      }
      else
      {
        ERR("ioctl get rlp fill rate failed. Null arg ptr", 0, 0, 0);
        *ps_errno = DS_EFAULT;
        return -1;
      }

      break;

    /*-----------------------------------------------------------------------
      Set RLP current RLP FILL RATE
    -----------------------------------------------------------------------*/
    case DSS_IFACE_IOCTL_707_SET_RLP_FILL_RATE:
      MSG_HIGH ("707 pkt ioctl - set rlp fill rate", 0, 0, 0);
      if ( argval_ptr != NULL )
      {
        /*-------------------------------------------------------------------
	  Will be overriden by rlp_init if this was set while the
	  RLP is not initialized. Lower limit of FILL RATE is set
          to a minimum of every 20 frames 
        -------------------------------------------------------------------*/
        tmp_fill_rate = (* ( uint8 *) argval_ptr );
        if ( (tmp_fill_rate < 20) || 
             (tmp_fill_rate > DSRLPI_LAST_FILL_NAK_GAP))
        {
          ERR("ioctl set rlp fill rate failed. MIN 20 MAX %d INPUT %d", 
                               DSRLPI_LAST_FILL_NAK_GAP, tmp_fill_rate, 0);     
                                                       
          *ps_errno = DS_EFAULT;
          return -1;
        }
        else
        {
          dsrlp_last_fill_nak_gap = tmp_fill_rate;
        }
      }
      else
      {
        ERR("ioctl set rlp fill rate failed. Null arg ptr", 0, 0, 0);
        *ps_errno = DS_EFAULT;
        return -1;
      }
      break;

#ifdef FEATURE_IS2000_REL_A
    /*-----------------------------------------------------------------------
      Get desired QOS non-assured priority adjustment.
    -----------------------------------------------------------------------*/
    case DSS_IFACE_IOCTL_707_GET_RLP_QOS_NA_PRI:
      /*---------------------------------------------------------------------
        If desired values have been set in QOS, the API should return the
        current desired value. But if it hasn't been set at all, the API
        returns the default desired value, which in this case is the same
        as standard default value.
      ---------------------------------------------------------------------*/
      if ((dsrlp_get_desired_qos_non_assur_pri ( &tmp_qos_val )) == FALSE)
      {
        tmp_qos_val = DSRLP_QOS_NON_ASSUR_DEFAULT_PRI_ADJ;
      }
      if ( argval_ptr != NULL )
      {
      (*(byte *) argval_ptr ) = tmp_qos_val;
      }
      else
      {
        ERR("ioctl get qos_na_pri failed. Null arg ptr", 0, 0, 0);
        *ps_errno = DS_EFAULT;
        return -1;
      }
      MSG_HIGH ("707 pkt ioctl - get qos_na_pri=%d", tmp_qos_val, 0, 0);
      break;

    /*-----------------------------------------------------------------------
      Set desired QOS non-assured priority adjustment.
    -----------------------------------------------------------------------*/
    case DSS_IFACE_IOCTL_707_SET_RLP_QOS_NA_PRI:
      if ( argval_ptr != NULL )
      {
      tmp_qos_val = (* (byte *) argval_ptr );
      }
      else
      {
        ERR("ioctl set qos_na_pri failed. Null arg ptr", 0, 0, 0);
        *ps_errno = DS_EFAULT;
        return -1;
      }

      MSG_HIGH ("707 pkt ioctl - set qos_na_pri=%d", tmp_qos_val, 0, 0);
      if ( tmp_qos_val >  DS707_QOS_DESIRED_PRI_MAX )
      {
        ERR("ioctl set qos_na_pri fail %d out of range", tmp_qos_val, 0, 0);
        *ps_errno = DS_EFAULT;
        return -1;
      }
      else
      {
        dsat707_qcqospri_val = tmp_qos_val;
        dsrlp_set_desired_qos_non_assur_pri ( tmp_qos_val );
      }
      break;

#endif /* FEATURE_IS2000_REL_A */

#ifdef FEATURE_DATA_IS2000_SDB
    /*-----------------------------------------------------------------------
      SDB support query.
    -----------------------------------------------------------------------*/
    case DSS_IFACE_IOCTL_707_SDB_SUPPORT_QUERY:
      /*---------------------------------------------------------------------
        The argument structure contains two fields. The first is input field
        flags which is the same as that provided to the socket write to
        specify SDB (Eg: MSG_EXPEDITE, MSG_FAST_EXPEDITE, etc).
        The second field is the output, when ioctl returns. It's boolean
        and says True if SDB can be supported.  Both BS and MS capability is
        checked.
      ---------------------------------------------------------------------*/
      if ( argval_ptr != NULL )
      {
      tmp_sdb_query_ptr =
           ( dss_iface_ioctl_707_sdb_support_query_type * ) argval_ptr;
      }
      else
      {
        ERR("ioctl sdb query failed. Null arg ptr", 0, 0, 0);
        *ps_errno = DS_EFAULT;
        return -1;
      }
      MSG_HIGH ("707 pkt ioctl - sdbflags=%d",tmp_sdb_query_ptr->flags,0,0);
      tmp_sdb_query_ptr->can_do_sdb = FALSE;
      tmp_sdb_query_ptr->can_do_sdb = mcc_sdb_supported();
      if ( tmp_sdb_query_ptr->can_do_sdb == TRUE )
      {
        if ( (tmp_sdb_query_ptr->flags & MSG_EXPEDITE)
             == MSG_EXPEDITE)
        {
          tmp_sdb_flags = MSG_EXPEDITE;
        }
        else if ((tmp_sdb_query_ptr->flags & MSG_FAST_EXPEDITE)
                  == MSG_FAST_EXPEDITE)
        {
          tmp_sdb_flags = MSG_FAST_EXPEDITE;
        }
        else
        {
          tmp_sdb_flags = tmp_sdb_query_ptr->flags;
        }
        tmp_sdb_query_ptr->can_do_sdb =
           ds707_sdb_supports_SDB_now ( tmp_sdb_flags );
      }
      break;
#endif /* FEATURE_DATA_IS2000_SDB */

    /*-----------------------------------------------------------------------
      Enable holddown timer.
    -----------------------------------------------------------------------*/
    case DSS_IFACE_IOCTL_707_ENABLE_HOLDDOWN:
      MSG_HIGH ("707 pkt ioctl - enable holddown", 0, 0, 0);
      if ( argval_ptr != NULL )
      {
      tmp_enable_holddown_val = ( * (boolean *) argval_ptr );
      }
      else
      {
        ERR("ioctl enable holddown failed. Null arg ptr", 0, 0, 0);
        *ps_errno = DS_EFAULT;
        return -1;
      }

      ds707_pkt_enable_holddown_timer ( tmp_enable_holddown_val );
      break;

    /*-----------------------------------------------------------------------
      Default.
    -----------------------------------------------------------------------*/
    default:
      ERR ("No such 707pkt ioctl: %d", ioctl_name, 0, 0 );
      *ps_errno = DS_EOPNOTSUPP;
      return -1;

  } /* switch */

  return 0;

} /* ds707_pkt_ioctl() */

/*===========================================================================
                 FUNCTIONS FOR PPP/RLP INTERACTION
===========================================================================*/
/*===========================================================================
FUNCTION      DS707_PKTI_UM_RLP_FWD_FUNC

DESCRIPTION   Called (via function pointer) by RLP when data is received on
              the reverse link.  Used by both SN and AN RLP sessions.  This
              will signal PS task that there is reverse link RLP/PPP data
              to process.

DEPENDENCIES  None.

RETURN VALUE  None.

SIDE EFFECTS  None.
===========================================================================*/
void ds707_pkti_um_rlp_fwd_func(void)
{
  PS_SET_SIGNAL(PS_PPP_UM_SN_RX_SIGNAL);
} /* ds707_pkti_um_rlp_fwd_func() */

/*===========================================================================
                             OTHER FUNCTIONS
===========================================================================*/
/*===========================================================================
FUNCTION      DS707_PKTI_CTA_TIMER_EXPIRED

DESCRIPTION   Called when AT+CTA timer expires.  Two options - either
              go dormant or end the session completely.

DEPENDENCIES  None.

RETURN VALUE  None.

SIDE EFFECTS  None.
===========================================================================*/
LOCAL void ds707_pkti_cta_timer_expired
(
  uint32        pkt_instance
)
{
  ds707_tc_state_type   *tc_state_ptr;
  ps_iface_type         *iface_ptr;
  int16                  errno;
  int16                  ret_val;
/* - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - */
  VERIFY_PKT_INSTANCE(pkt_instance);

  tc_state_ptr = GET_TC_STATE_PTR(pkt_instance);
  iface_ptr    = GET_PS_IFACE_PTR(pkt_instance);

  #ifndef FEATURE_ASYNC_DATA_NOOP
  if (ds707_so_async_so_is_async(tc_state_ptr->so) == TRUE)
  {
    /*-----------------------------------------------------------------------
      QNC calls cannot go dormant.  Set CTA timer so that it never expires
    -----------------------------------------------------------------------*/
    ERR("QNC call cannot go dormant",0,0,0);
    ds707_cta_reset_timer(DS707_CTA_STOP_TIMER, tc_state_ptr->so);
    return;
  }
  #endif 

  /*-------------------------------------------------------------------------
    First makes sure that there is an active data session to work on.
  -------------------------------------------------------------------------*/
  if ((ps_iface_phys_link_state(iface_ptr) == PHYS_LINK_UP) &&
      ((ps_iface_state(iface_ptr) == IFACE_UP)         ||
       (ps_iface_state(iface_ptr) == IFACE_ROUTEABLE)  ||
       (ps_iface_state(iface_ptr) == IFACE_COMING_UP)
      )
     )
  {
    ret_val = ps_iface_phys_link_down_cmd(iface_ptr,
                                          &errno,
                                          NULL);

    if ((ret_val < 0) && (errno != DS_EWOULDBLOCK))
    {
      ERR("Cant tear down iface or phys link %d",errno,0,0);
    }
  }
  else
  {
    ERR("AT+CTA timer expired with no data call",0,0,0);
  }
} /* ds707_pkti_cta_timer_expired() */


/*===========================================================================
FUNCTION      DS707_PKTI_PZSNID_CHANGED

DESCRIPTION   Called when either PZID, SID, or NID has changed, requiring
              mobile to come out of dormancy.

DEPENDENCIES  runs in DS TASK context.

RETURN VALUE  None.

SIDE EFFECTS  None.
===========================================================================*/
LOCAL void ds707_pkti_pzsnid_changed
(
  ds_cmd_enum_type    cmd,
  uint32              pkt_instance
)
{
  ps_iface_type            *iface_ptr;          /* ptr to 1x iface         */
  ds707_pkt_call_info_type  pkt_info;
  int16                     errno;
  int16                     ret_val;
  boolean                  *snpzid_change_ptr;
/* - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - */
  VERIFY_PKT_INSTANCE(pkt_instance);
  iface_ptr = GET_PS_IFACE_PTR(pkt_instance);

  /*-------------------------------------------------------------------------
    This struct is passed onto the iface.  PZID/SID/NID normallly have no
    data to send.  PZID/SID/NID reconnections override the holddown timer
  -------------------------------------------------------------------------*/
  pkt_info.drs_bit         = FALSE;
  pkt_info.use_hdown_timer = FALSE;

  if (ds707_pkti_ok_orig_from_dorm(iface_ptr) == TRUE)
  {
    snpzid_change_ptr = GET_SNPZID_CHANGE_FLAG_PTR(pkt_instance);
    pkt_info.drs_bit = ds707_drs_get_mip_drs_bit();
    *snpzid_change_ptr = pkt_info.drs_bit;
    ret_val = ps_iface_phys_link_up_cmd(iface_ptr, &errno, &pkt_info);

    if ((ret_val < 0) && (errno != DS_EWOULDBLOCK))
    {
      ERR("ps_iface reorig failed %d", errno, 0, 0);
    }
  }
  else
  {
    MSG_HIGH("No PZSNID reorig. not dorm-iface %d phys %d",
              ps_iface_state(iface_ptr),
              ps_iface_phys_link_state(iface_ptr),
              0
            );
  }
} /* ds707_pkti_pzsnid_changed() */

/*===========================================================================
FUNCTION      DS707_PKTI_REORIG_SIG_HDLR

DESCRIPTION   Called when the signal to re-originate is set.  Usually set
              when the RLP Tx watermark goes non-empty in a dormant mode.

DEPENDENCIES  None

RETURN VALUE  None.

SIDE EFFECTS  None.
===========================================================================*/
void ds707_pkti_reorig_sig_hdlr(void)
{
  ps_iface_type             *iface_ptr;         /* ptr to 1x iface         */
  ds707_tc_state_type       *tc_state_ptr;
  int16                      errno;
  int16                      ret_val;
  int                        i;
/* - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - */
  for (i = 0; i < DS707_MAX_PKT_CALLS; i++)
  {
    tc_state_ptr = GET_TC_STATE_PTR(i);
    iface_ptr    = GET_PS_IFACE_PTR(i);

    if ((ds707_pkti_ok_orig_from_dorm(iface_ptr) == TRUE) &&
        (tc_state_ptr->tx_watermark_ptr->current_cnt > 0)
       )
    {
      break;
    }
  }

  /*-------------------------------------------------------------------
    Clear the DS signal.  Do not want to do it after the dsm_empty_q()
    calls, because it is possible a packet comes in between the end
    of the dsm_empty q and the start of the rex_clr_sigs().  Doing it
    here, well before the emptying of the dsm watermarks, reduces
    the possibility that we have to process a stale signal.
  -------------------------------------------------------------------*/
  rex_clr_sigs(&ds_tcb, DS_PKT_REORIG_SIG);

  if (i == DS707_MAX_PKT_CALLS)
  {
    ERR("No dormant call needing re-orig",0,0,0);
  }
  else
  {
#ifdef FEATURE_DATA_IS2000_SDB
    if ( ds707_sdb_process_txq() == TRUE )
    {
      MSG_HIGH("SDB Packet processed for tx",0,0,0);
      return;
    }
#endif /* FEATURE_DATA_IS2000_SDB */
    if ((DS_FLOW_CTRL_IS_BIT_SET(ps_iface_get_flow_mask(iface_ptr),
                                 DS_HOLDDOWN_TIMER_MASK)) == FALSE)
    {
      ret_val = ps_iface_phys_link_up_cmd(iface_ptr, &errno, NULL);
      if ((ret_val < 0) && (errno != DS_EWOULDBLOCK))
      {
        ERR("PS_IFACE reorig failed %d", errno, 0, 0);
        dsm_empty_queue(tc_state_ptr->tx_watermark_ptr);
      }
    }
    else
    {
      MSG_HIGH("Cant reorig with holddown timer",0,0,0);
      dsm_empty_queue(tc_state_ptr->tx_watermark_ptr);
    }
  }
} /* ds707_pkti_reorig_sig_hdlr() */

/*===========================================================================
                       EXTERNAL FUNCTION DEFINITIONS
===========================================================================*/

/*===========================================================================
FUNCTION      DS707_PKT_IS_DORMANT

DESCRIPTION   Returns TRUE if iface is dormant.  Else returns FALSE.

DEPENDENCIES  None.

RETURN VALUE  See description.

SIDE EFFECTS  None.
===========================================================================*/
boolean ds707_pkt_is_dormant
(
  ps_iface_type      *iface_ptr
)
{
  if (ps_iface_phys_link_state(iface_ptr) == PHYS_LINK_DOWN)
  {
    if ((ps_iface_state(iface_ptr) == IFACE_UP)        ||
        (ps_iface_state(iface_ptr) == IFACE_ROUTEABLE) ||
        (ps_iface_state(iface_ptr) == IFACE_COMING_UP)
       )
    {
      return(TRUE);
    }
  }
  return(FALSE);
} /* ds707_pkt_is_dormant() */


/*===========================================================================
FUNCTION      DS707_PKT_GET_PREV_CALL_SO

DESCRIPTION   This function will return SO of previous call.

DEPENDENCIES  None

RETURN VALUE  None.

SIDE EFFECTS  None.
===========================================================================*/
word ds707_pkt_get_prev_call_so (void)
{
/*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/
  return ( ds707_pkt_prev_data_call_so );
} /* ds707_pkt_get_prev_call_so */


/*===========================================================================
FUNCTION      DS707_PKT_GET_PREV_SR_ID

DESCRIPTION   This function will return SR ID of previous call.

DEPENDENCIES  None

RETURN VALUE  None.

SIDE EFFECTS  None.
===========================================================================*/
byte ds707_pkt_get_prev_sr_id (void)
{
/*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/
  return ( ds707_pkt_prev_sr_id );
} /* ds707_pkt_get_prev_sr_id() */


/*===========================================================================
FUNCTION      DS707_PKT_GET_CURR_TX_WM_FROM_RLP

DESCRIPTION   This function gets current tx watermark.

DEPENDENCIES  This function assumes its only a CDMA 1x data call.
              So, it looks at the RLP state array and returns pointer to the
              Watermark.

PARAMETERS    Current SR_ID Index value

RETURN VALUE  If SR_ID_INDEX is a valid value it returns the pointer to the
              current TX watermark,  Else, it returns NULL.

SIDE EFFECTS
===========================================================================*/
dsm_watermark_type  *ds707_pkt_get_curr_tx_wm_from_RLP
(
  byte sr_id_idx
)
{
/*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  /*-------------------------------------------------------------------------
    Check if the input parameters are valid
  -------------------------------------------------------------------------*/
  if (sr_id_idx >= DSRLPI_MAX_SRID_INDEX)
  {
    MSG_HIGH("Invalid SR_ID Index value",sr_id_idx,0,0);
    return( (dsm_watermark_type*)(NULL));
  }
  /*-------------------------------------------------------------------------
    For the given RLP session, return the tx watermark
    Assumption is that the return value can be NULL sometimes and it is the
    responsibility of the caller to check for NULL.
  -------------------------------------------------------------------------*/
  return (dsrlpi_rscb_array[sr_id_idx].cfg.tx_wm_ptr);
} /* ds707_pkt_get_curr_tx_wm_from_RLP */


/*===========================================================================
FUNCTION      DS707_PKT_GET_CURR_TX_WM

DESCRIPTION   This function gets current tx watermark for call instance.

DEPENDENCIES  None.

PARAMETERS    Call instance.

RETURN VALUE  Pointer to  current tx wmk.

SIDE EFFECTS  None.
===========================================================================*/
dsm_watermark_type  *ds707_pkt_get_curr_tx_wm
(
  uint32 pkt_instance
)
{
  ds707_tc_state_type             *tc_state_ptr;
/*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  VERIFY_PKT_INSTANCE(pkt_instance);

  tc_state_ptr = GET_TC_STATE_PTR(pkt_instance);

  return (tc_state_ptr->tx_watermark_ptr); //Nagesh

} /* ds707_pkt_get_curr_tx_wm */


/*===========================================================================
FUNCTION      DS707_PKT_REORIG

DESCRIPTION   Called when the RLP TX watermark goes non-empty when
              dormant.  Sets a signal so that the DS task goes ahead
              and processes this function.

DEPENDENCIES  None

RETURN VALUE  None.

SIDE EFFECTS  None.
===========================================================================*/
void ds707_pkt_reorig(void)
{
  rex_set_sigs(&ds_tcb, DS_PKT_REORIG_SIG);
} /* ds707_pkt_reorig() */

/*===========================================================================
                        DS707 PKT MGR INITIALIZATION
===========================================================================*/
/*===========================================================================
FUNCTION      DS707_PKT_MGR_INIT

DESCRIPTION   Called once at mobile power-up.  Registers the appropriate
              functionality with 3G DSMGR.

DEPENDENCIES  None.

RETURN VALUE  Rex signals this mode specific handler is interested in.

SIDE EFFECTS  None
===========================================================================*/
rex_sigs_type ds707_pkt_mgr_init(void)
{
  uint32                     i;
  ps_iface_type             *iface_ptr;        /* ptr to 1x iface          */
  acl_type                  *my_acl_ptr;
  ds707_tc_state_type       *tc_state_ptr;
  void                      *cback_ptr;        /* returned by ps_iface func*/
  boolean                   *snpzid_change_ptr;
/* - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - */

  /*-------------------------------------------------------------------------
     Register with 3G DSMGR
  -------------------------------------------------------------------------*/
  ds3g_register_mode_sp_handler(SYS_SYS_MODE_CDMA,
                                DS_CALL_TYPE_PKT,
                                &ds707_pkt_mgr_hdlr_func_tbl);


  /*-------------------------------------------------------------------------
    Initialize RLP's.
  -------------------------------------------------------------------------*/
  rlp_init();
  dsrlp_startup();

  /*-------------------------------------------------------------------------
     Register resource request modules (SCRM & Control/Hold) with MC/CM.
  -------------------------------------------------------------------------*/
  ds707_rrm_reg_funcs();

  /*-------------------------------------------------------------------------
     Set up the various service options with MC/CM.  These implicitly do the
     JCDMA registrations (if that is enabled).
  -------------------------------------------------------------------------*/
  ds707_so_pkt_init();

  ds707_so_retrydel_init();

  /*-------------------------------------------------------------------------
     Register SO 33 initializers with MC/CM.  Must happen after the
     SO's are initialized.
  -------------------------------------------------------------------------*/
  ds707_so33_reg_initializers();

  /*-------------------------------------------------------------------------
     Initialize Timers, Watermarks, and the RMSM state machine module.
     Set CTA timer to not be active.
  -------------------------------------------------------------------------*/
  ds707_wmk_init();

  ds707_timer_init();
  ds707_cta_reset_timer(DS707_CTA_STOP_TIMER, DS707_INVALID_SO);
 
#ifdef FEATURE_JCDMA_1X
  /*-------------------------------------------------------------------------
    Initialize M51 and apprate JCDMA modules.  Must be done after SO's have
    been initialized.
  -------------------------------------------------------------------------*/
  ds707_jcdma_m51_init();
  ds707_jcdma_apprate_init();
#endif /* FEATURE_JCDMA_1X */

  /*-------------------------------------------------------------------------
    Register query functions with DIAG.
  -------------------------------------------------------------------------*/
  ds707_diag_init();

  /*-------------------------------------------------------------------------
    Initialize each packet instance.
  -------------------------------------------------------------------------*/
  for (i = 0; i < DS707_MAX_PKT_CALLS; i++)
  {
    tc_state_ptr      = GET_TC_STATE_PTR(i);
    iface_ptr         = GET_PS_IFACE_PTR(i);
    my_acl_ptr        = GET_ACL_PTR(i);
    snpzid_change_ptr = GET_SNPZID_CHANGE_FLAG_PTR(i);

    *snpzid_change_ptr = FALSE;

    tc_state_ptr->call_id = CM_CALL_ID_INVALID;
    tc_state_ptr->so      = DS707_INVALID_SO;

#ifdef FEATURE_IS2000_REL_A_SVD
    /*-----------------------------------------------------------------------
       Allocate sr_id for DS during the power up time.
    -----------------------------------------------------------------------*/
    tc_state_ptr->alloc_sr_id = mccsrid_allocate_srid(SRID_OWNER_DS);

    if (tc_state_ptr->alloc_sr_id == MCCSRID_INVALID_SRID)
    {
      ERR("DS could not allocate SR_ID", 0, 0, 0);
    }
    else
    {
      MSG_HIGH("SR_ID %d allocated for DS", tc_state_ptr->alloc_sr_id, 0, 0);
    }
#endif /* FEATURE_IS2000_REL_A_SVD */

    /*-----------------------------------------------------------------------
      If IS2000 ever offers two packet traffic channels at once, this will
      have to be modified, to allow for other RLP instances.
    -----------------------------------------------------------------------*/
    tc_state_ptr->rlp_1x.rlp_instance = DSRLP_FIXED_SR_ID_INDEX;

#ifdef FEATURE_HDR
#error code not present
#endif /* FEATURE_HDR */

    /*-----------------------------------------------------------------------
      Initialize RLP for this pkt_state.
    -----------------------------------------------------------------------*/
    ds707_pkti_set_rlp_defaults(tc_state_ptr);

    /*-----------------------------------------------------------------------
      Initialize the PS_IFACE info for this pkt instance.  Set the user
      pointer to point to the pkt_state_ptr.
    -----------------------------------------------------------------------*/
    my_acl_ptr->acl_fptr = ds707_rt_acl;
    ps_iface_create(iface_ptr, CDMA_SN_IFACE, my_acl_ptr);
    iface_ptr->client_data_ptr = (void *)i;

#ifdef FEATURE_DS_MULTIPLE_PROFILES
   /*-----------------------------------------------------------------------
    Intialize the data session block
   -----------------------------------------------------------------------*/
    ds707_data_session_info_init(iface_ptr);
    ds707_data_session_init();
#endif	/* FEATURE_DS_MULTIPLE_PROFILES */

    /*-----------------------------------------------------------------------
      If we ever support multiple RM PPP interfaces, then RMSM will need to
      be modified.
    -----------------------------------------------------------------------*/
    #ifndef FEATURE_DATA_STRIP_ATCOP
    ds707_rmsm_init(iface_ptr);
    #else
    MSG_HIGH("RMSM not supported!", 0, 0, 0);
    #endif

    /*-----------------------------------------------------------------------
      Initialize and register the iface down ind callback info.
    -----------------------------------------------------------------------*/
    cback_ptr = ps_iface_alloc_event_cback_buf(ds707_pkti_iface_down_ind_cback,
                                               (void *)i
                                              );

    ps_iface_event_cback_reg(iface_ptr,
                             IFACE_DOWN_EV,
                             cback_ptr
                            );

    /*-----------------------------------------------------------------------
      Initialize and register the iface up ind callback info.
    -----------------------------------------------------------------------*/
    cback_ptr = ps_iface_alloc_event_cback_buf(ds707_pkti_iface_up_ind_cback,
                                               (void *)i
                                              );

    ps_iface_event_cback_reg(iface_ptr,
                             IFACE_UP_EV,
                             cback_ptr
                            );

    /*-----------------------------------------------------------------------
      Initialize and register the iface routeable ind callback info.
    -----------------------------------------------------------------------*/
    cback_ptr = ps_iface_alloc_event_cback_buf
                                      ( 
                                        ds707_pkti_iface_routeable_ind_cback,
                                        (void *)i
                                      );

    ps_iface_event_cback_reg(iface_ptr,
                             IFACE_ROUTEABLE_EV,
                             cback_ptr
                            );
    /*-----------------------------------------------------------------------
      These bring up/tear down the traffic channel.  However, in the case of
      the tear down, it also cleans up the PPP state (i.e., no dormancy).
    -----------------------------------------------------------------------*/
    iface_ptr->bring_up_cmd_f_ptr   = ds707_pkti_iface_up;
    iface_ptr->tear_down_cmd_f_ptr  = ds707_pkti_iface_down;

    /*-----------------------------------------------------------------------
      These bring up/tear down the traffic channel.  However, the mobile
      maintains its PPP state (unless QNC).
    -----------------------------------------------------------------------*/
    iface_ptr->phys_link_down_cmd_f_ptr = ds707_pkti_phys_link_down;
    iface_ptr->phys_link_up_cmd_f_ptr   = ds707_pkti_phys_link_up;

    /*-----------------------------------------------------------------------
      This is for doing ioctls specific to 1x pkt interface.
    -----------------------------------------------------------------------*/
    iface_ptr->iface_ioctl_f_ptr = ds707_pkt_ioctl;

    ps_iface_enable_ind(iface_ptr);
#ifdef FEATURE_JCDMA_1X
    iface_ptr->graceful_dormant_close = FALSE;
#else
    iface_ptr->graceful_dormant_close = TRUE;
#endif /* FEATURE_JCDMA_1X */
  }

  /*-------------------------------------------------------------------------
    Currently no signals we are interested in.
  -------------------------------------------------------------------------*/
  return((rex_sigs_type)DS_PKT_REORIG_SIG);
} /* ds707_pkt_mgr_init() */


/*===========================================================================
FUNCTION      DS707_PKT_CM_INIT

DESCRIPTION   Called once at startup.  Registers with CM for things specific
              to IS-707.

DEPENDENCIES  None.

RETURN VALUE  None.

SIDE EFFECTS  None.
===========================================================================*/
void ds707_pkt_cm_init
(
  cm_client_id_type        cm_id
)
{
/*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/
#ifdef FEATURE_HDR_HANDOFF
#error code not present
#endif /* FEATURE_HDR_HANDOFF */

#ifdef FEATURE_DATA_IS2000_SDB
  /*-------------------------------------------------------------------------
    Register with CM for getting following SDB information:
    Mobile Terminated messages,
    Status of Mobile Originated Messages (to convey to application).

       client:           Requesting client i.e. DS
      *dbm_event_func:   Pointer to a callback function to notify the client
                         of dbm events
       event_reg_type:   Is this a registration or de-reg. DS registers once
                         and never deregisters.cm_dbm_event_e_type
       from_dbm_event:   register from this event (including). So any events
                         in the enum type from CM_DBM_EVENT_MO_STATUS to
                         to_dbm_event will be informed to DS.
       to_dbm_event:     To this event (including). CM_DBM_EVENT_MT_MSG
      *dbm_cmd_err_func: Pointer to a callback function to notify the
                         client of dbm command errors of OTHER clients.
                         DS currently does not care. So, choose NULL.

  -------------------------------------------------------------------------*/
  cm_client_dbm_event_reg(cm_id,
                          ds707_sdb_DBM_event_handler,
                          CM_CLIENT_EVENT_REG,
                          CM_DBM_EVENT_MO_STATUS,
                          CM_DBM_EVENT_MT_MSG,
                          NULL);

  /*-------------------------------------------------------------------------
    Register with CM for getting following SDB information:
  -------------------------------------------------------------------------*/
  cm_client_dbm_class_reg(cm_id,
                          CM_CLIENT_EVENT_REG,
                          CM_DBM_CLASS_SDB,
                          CM_DBM_CLASS_SDB
                          );

#endif /* FEATURE_DATA_IS2000_SDB */

}/* ds707_pkt_cm_init() */


/*===========================================================================
FUNCTION      DS707_PKT_IS_PZID_ORIG_ALLOWED

DESCRIPTION   Returns TRUE or FALSE, indicating if an orig is allowed on the
              specified IFACE.
              
              Done solely based on holddown timer.

DEPENDENCIES  None.

RETURN VALUE  TRUE - can originate.  FALSE - cannot originate.

SIDE EFFECTS  None.
===========================================================================*/
boolean ds707_pkt_is_pzid_orig_allowed
(
  uint32 pkt_instance
)
{
  ps_iface_type               *iface_ptr;        /* ptr to 1x iface        */
/* - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - */
  VERIFY_PKT_INSTANCE(pkt_instance);

  iface_ptr    = GET_PS_IFACE_PTR(pkt_instance);

  if ((ps_iface_phys_link_state(iface_ptr) == PHYS_LINK_DOWN) &&
      ((DS_FLOW_CTRL_IS_BIT_SET(ps_iface_get_flow_mask(iface_ptr),
                                            DS_HOLDDOWN_TIMER_MASK)) == TRUE)
     )
  {
    return(FALSE);
  }
  else
  {
    return(TRUE);
  }
} /* ds707_pkt_is_pzid_orig_allowed() */


/*===========================================================================
FUNCTION      DS707_PKT_PROCESS_CMD

DESCRIPTION   Called by 3G DSMGR, to handle commands specific to this
              mode handler.

              There is no featurization of commands here.  Functionality
              should be FEATURE'ized elsewhere, so that these commands are
              generated appopriately.  For example, the SID module only
              generates its command if the SID FEATURE's is defined.

DEPENDENCIES  None.

RETURN VALUE  None

SIDE EFFECTS  Can result in various actions, such as call bring up/tear
              down, etc.
===========================================================================*/
void ds707_pkt_process_cmd
(
  ds_cmd_type   *ds_cmd_ptr
)
{
  ps_iface_type               *iface_ptr;        /* ptr to 1x iface        */
/* - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - */
  switch(ds_cmd_ptr->hdr.cmd_id)
  {
    case DS_707_PKT_PZID_CHANGE_CMD:
      MSG_HIGH("Try reconn on PZID change",0,0,0);
      ds707_pkti_pzsnid_changed(ds_cmd_ptr->hdr.cmd_id,
                                DS707_DEF_PKT_INSTANCE);
      break;

    case DS_707_PKT_SID_CHANGE_CMD:
      MSG_HIGH("Try reconn on SID change",0,0,0);
      ds707_pkti_pzsnid_changed(ds_cmd_ptr->hdr.cmd_id,
                                DS707_DEF_PKT_INSTANCE);
      break;

#ifdef FEATURE_DS_PZID_HYSTERESIS
    case DS_707_PKT_PZID_HYS_DATA_READY_CMD:
      ds707_pzid_hysteresis_process_event( PZID_HYS_DATA_READY_EVENT );
      break;

    case DS_707_PKT_PZID_HYS_SDB_DATA_CMD:
      ds707_pzid_hysteresis_process_event( PZID_HYS_SDB_DATA_EVENT );
      break;
#else   
    case DS_707_PKT_PZID_HYS_DATA_READY_CMD:
      ds707_epzid_hysteresis_process_event( EPZID_HYS_DATA_READY_EVENT );
      break;

    case DS_707_PKT_PZID_HYS_SDB_DATA_CMD:
      ds707_epzid_hysteresis_process_event( EPZID_HYS_SDB_DATA_EVENT );
      break;
#endif /* FEATURE_DS_PZID_HYSTERESIS  */

    case DS_707_PKT_NID_CHANGE_CMD:
      MSG_HIGH("Try reconn on NID change",0,0,0);
      ds707_pkti_pzsnid_changed(ds_cmd_ptr->hdr.cmd_id,
                                DS707_DEF_PKT_INSTANCE);
      break;

    case DS_707_PKT_CTA_TIMER_EXPIRED_CMD:
      MSG_HIGH("AT+CTA expired... go dormant",0,0,0);
      event_report(EVENT_INACTIVITY_TIMER_EXPIRED);
      ds707_pkti_cta_timer_expired(DS707_DEF_PKT_INSTANCE);
      break;

    case DS_707_PKT_HOLDDOWN_TIMER_EXPIRED_CMD:
      MSG_HIGH("Holddown timer expired",0,0,0);
      if (ds707_dorm_timer_get_timer() > 0)
      {
        event_report(EVENT_DORMANT_TIMER_EXPIRED);
      }
      
      iface_ptr = GET_PS_IFACE_PTR(DS707_DEF_PKT_INSTANCE);
      ps_iface_enable_flow(iface_ptr, DS_HOLDDOWN_TIMER_MASK);

#ifndef FEATURE_DATA_EPZID
      ds707_pzid_check_for_new_pzid();
#else
      ds707_epzid_check_for_new_epzid();
#endif /* FATURE_DATA_PZID */

      break;

    case DS_707_PKT_PZID_DELAY_TIMER_EXPIRED_CMD:
    #ifndef FEATURE_DATA_EPZID
      ds707_pzid_process_delay_timer_expired();
    #else 
      ds707_epzid_process_delay_timer_expired();
    #endif /* FEATURE_DATA_PZID */
      break;

#ifdef FEATURE_DS_PZID_HYSTERESIS
    case DS_707_PKT_PZID_HYSTERESIS_TIMER_EXPIRED_CMD:
      MSG_HIGH("PZID Hysteresis timer expired",0,0,0);
      ds707_pzid_hysteresis_process_event(PZID_HYS_TIMER_EXPIRED_EVENT);
	  break;
#else
    case DS_707_PKT_PZID_HYSTERESIS_TIMER_EXPIRED_CMD:
      MSG_HIGH("EPZID Hysteresis timer expired",0,0,0);
      ds707_epzid_hysteresis_process_event(EPZID_HYS_TIMER_EXPIRED_EVENT);
    break;
#endif /* FEATURE_DS_PZID_HYSTERESIS */

    case DS_707_PKT_IFACE_UP_CMD:
      VERIFY_PKT_INSTANCE(ds_cmd_ptr->cmd.ds707_data.pkt_instance);
      ds707_pkti_iface_up_cmd_hdlr(ds_cmd_ptr->cmd.ds707_data.pkt_instance);
      break;

    case DS_707_PKT_PHYS_LINK_UP_CMD:
      VERIFY_PKT_INSTANCE(ds_cmd_ptr->cmd.ds707_data.pkt_instance);
      ds707_pkti_phys_link_up_cmd_hdlr
                               (
                                  ds_cmd_ptr->cmd.ds707_data.drs_bit,
                                  ds_cmd_ptr->cmd.ds707_data.use_hdown_timer,
                                  ds_cmd_ptr->cmd.ds707_data.pkt_instance
                               );
      break;

    case DS_707_PKT_PHYS_LINK_DOWN_CMD:
      VERIFY_PKT_INSTANCE(ds_cmd_ptr->cmd.ds707_data.pkt_instance);
      ds707_pkti_phys_link_down_cmd_hdlr(
                                    ds_cmd_ptr->cmd.ds707_data.pkt_instance);
      break;

    case DS_707_PKT_IFACE_DOWN_CMD:
      VERIFY_PKT_INSTANCE(ds_cmd_ptr->cmd.ds707_data.pkt_instance);
      ds707_pkti_iface_down_cmd_hdlr(
                                    ds_cmd_ptr->cmd.ds707_data.pkt_instance);
      break;

    case DS_707_PKT_IFACE_DOWN_IND_CBACK_CMD:
      VERIFY_PKT_INSTANCE(ds_cmd_ptr->cmd.ds707_data.pkt_instance);
      ds707_pkti_iface_down_ind_cback_cmd_hdlr(
                                    ds_cmd_ptr->cmd.ds707_data.pkt_instance);
      break;

    case DS_707_PKT_IFACE_UP_IND_CBACK_CMD:
      VERIFY_PKT_INSTANCE(ds_cmd_ptr->cmd.ds707_data.pkt_instance);
      ds707_pkti_iface_up_ind_cback_cmd_hdlr(
                                    ds_cmd_ptr->cmd.ds707_data.pkt_instance);
      break;

    case DS_707_PKT_IFACE_ROUTEABLE_IND_CBACK_CMD:
      VERIFY_PKT_INSTANCE(ds_cmd_ptr->cmd.ds707_data.pkt_instance);
      ds707_pkti_iface_routeable_ind_cback_cmd_hdlr(
                                    ds_cmd_ptr->cmd.ds707_data.pkt_instance);
      break;

    case DS_707_TOGGLE_QNC_ENABLE_CMD:
#ifdef FEATURE_DS_QNC
      dsat707_qcqnc_val = !dsat707_qcqnc_val;
#endif /* FEATURE_DS_QNC */
      #ifndef FEATURE_DATA_STRIP_ATCOP
      dsat707_nv_sync();
      #else
      dsatprofile_nv_sync();
      #endif
      ds707_so_pkt_recal();
      break;

    default:
      ERR("Unknown cmd to ds707_pkt %d", ds_cmd_ptr->hdr.cmd_id, 0, 0);
      break;
  }
} /* ds707_pkt_process_cmd() */

/*===========================================================================
FUNCTION      DS707_PKT_PROCESS_SIGNALS

DESCRIPTION   Processes sigals set by other parts of the system.  Currently,
              no signals should be set for this mode handler.

DEPENDENCIES  None.

RETURN VALUE  None

SIDE EFFECTS  None
===========================================================================*/
void ds707_pkt_process_signals
(
  rex_sigs_type   sigs
)
{
/* - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - */
  if (sigs & DS_PKT_REORIG_SIG)
  {
    ds707_pkti_reorig_sig_hdlr();
  }
  else
  {
    ASSERT(0);
    ERR("unknown signal to ds707_pkt 0x%x",sigs,0,0);
  }
}

/*===========================================================================
FUNCTION      DS707_PKT_GET_IFACE_PTR

DESCRIPTION   Given a packet state instance, returns the pointer to the
              pkt state.

DEPENDENCIES  None.

RETURN VALUE  pointer to a pkt state

SIDE EFFECTS  None.
===========================================================================*/
ps_iface_type* ds707_pkt_get_iface_ptr
(
  uint32 pkt_instance
)
{
/* - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - */
  if (pkt_instance < DS707_MAX_PKT_CALLS)
  {
    return(GET_PS_IFACE_PTR(pkt_instance));
  }
  else
  {
    ASSERT(0);
    ERR("Bad pkt instance",0,0,0);
    return(NULL);
  }
} /* ds707_pkt_get_iface_ptr() */


/*===========================================================================
FUNCTION      DS707_PKT_SET_ABORT_F_PTR

DESCRIPTION   Sets the abort function ptr for the specified pkt instance.

DEPENDENCIES  None.

RETURN VALUE  None.

SIDE EFFECTS  None.
===========================================================================*/
void  ds707_pkt_set_abort_f_ptr
(
  uint32                     pkt_instance,
  ds707_pkt_call_abort_f_ptr abort_f_ptr
)
{
/* - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - */
  if (pkt_instance < DS707_MAX_PKT_CALLS)
  {
    ds707_pkt_state[pkt_instance].abort_f_ptr = abort_f_ptr;
  }
  else
  {
    ASSERT(0);
    ERR("Bad pkt instance",0,0,0);
    return;
  }

} /* ds707_pkt_set_abort_f_ptr() */

/*===========================================================================
FUNCTION      DS707_PKT_SETUP_RLP

DESCRIPTION   Sets up RLP with a TX watermark and RLP RX func and RLP RX
              queue.  If the inputs are NULL, then use defaults as
              defined by this module.  Else use the ones passed into this
              function.

DEPENDENCIES  None

RETURN VALUE  None

SIDE EFFECTS  None.
===========================================================================*/
void ds707_pkt_setup_rlp
(
  dsm_watermark_type  *tx_watermark_ptr,         /* Tx wm ptr for txmit frs*/
  void                (*post_rx_func_ptr)(void), /* rx cback for rx frames */
  q_type              *post_rx_q_ptr,            /* queue for rx'ed frames */
  uint32               pkt_instance              /* which pkt instance?    */
)
{
  ds707_tc_state_type *tc_state_ptr;
/* - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - */
  VERIFY_PKT_INSTANCE(pkt_instance);
  tc_state_ptr = GET_TC_STATE_PTR(pkt_instance);

  if ((tx_watermark_ptr == NULL) &&
      (post_rx_func_ptr == NULL) &&
      (post_rx_q_ptr    == NULL)
     )
  {
    ds707_pkti_set_rlp_defaults(tc_state_ptr);
  }
  else
  {
      tc_state_ptr->tx_watermark_ptr = tx_watermark_ptr;
      tc_state_ptr->post_rx_func_ptr = post_rx_func_ptr;
      tc_state_ptr->post_rx_q_ptr    = post_rx_q_ptr;
  }
} /* ds707_pkt_setup_rlp() */

/*===========================================================================
FUNCTION      DS_VERIFY_SO

DESCRIPTION   Called by MC.  Asks DS if the originating SO is correct.

DEPENDENCIES  Called in task other than DS.  Necessary, as this is called
              even when doing an async/fax call.  Do not want to override
              those SO's.  We only initiate on phys_link_up, which sets
              pkt iface phys state = COMING_UP.  phys_link state set after
              phys_link_cmd called, which all happens under TASKLOCK.  So
              if called from other task, then ensured that phsy_link ==
              COMING_UP (and may not still be DOWN but soon will be
              COMING_UP).

RETURN VALUE  service option to orig with.

SIDE EFFECTS  None.
===========================================================================*/
word ds_verify_so
(
  word so_to_verify           /* SO to verify                              */
)
{
  ds707_so_type               cdma_so;   /* Service option for CDMA origs  */
  ps_iface_type              *iface_ptr; /* ptr to 1x iface                */
/*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  iface_ptr = GET_PS_IFACE_PTR(DS707_DEF_PKT_INSTANCE);

  if (ps_iface_phys_link_state(iface_ptr) != PHYS_LINK_COMING_UP)
  {
    /*-----------------------------------------------------------------------
      Called from something other than pkt call.
    -----------------------------------------------------------------------*/
    return(so_to_verify);
  }

  cdma_so = ds707_pkti_get_orig_so();

  if (cdma_so != so_to_verify)
  {
    MSG_HIGH("Pkt SO changed %d -> %d", so_to_verify, cdma_so, 0);
  }
  return((word)cdma_so);
} /* ds_verify_so() */


/*===========================================================================
FUNCTION      DS707_PKT_GET_ACTIVE_IFACE_PTR

DESCRIPTION   Called to indicate if a call is active (i.e., moving data).

DEPENDENCIES  None

RETURN VALUE  Pointer to the active data call.  Else, if no call is active,
              returns NULL.

SIDE EFFECTS  None
===========================================================================*/
ps_iface_type* ds707_pkt_get_active_iface_ptr(void)
{
  int                    i;
  ps_iface_type         *iface_ptr;
/*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/
  for (i = 0; i < DS707_MAX_PKT_CALLS; i++)
  {
    iface_ptr    = GET_PS_IFACE_PTR(i);
    if ((ps_iface_phys_link_state(iface_ptr) == PHYS_LINK_UP)  ||
        (ps_iface_phys_link_state(iface_ptr) == PHYS_LINK_COMING_UP))
    {
      return(iface_ptr);
    }
  }
  return(NULL);
} /* ds707_pkt_get_active_iface_ptr() */



/*===========================================================================
                 FUNCTIONS FOR PPP/RLP INTERACTION
===========================================================================*/
/*===========================================================================
FUNCTION      DS707_PKT_SETUP_SN_PPP

DESCRIPTION   Called when IFACE is coming up, to intialize PPP for the
              SN PPP.  This the interface used for 1X.

DEPENDENCIES  None

RETURN VALUE  TRUE for successful PPP config, FALSE otherwise

SIDE EFFECTS  None
===========================================================================*/
int ds707_pkt_setup_SN_ppp
(
  uint32     pkt_instance
)
{
  static ppp_dev_opts_type    ppp_config_info; /* UNFINISHED - shouldnt be static */
/*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/
  ppp_get_default_opts( &ppp_config_info );

  MSG_FATAL("ds707_pkt_setup_SN_ppp",0,0,0);
  is707_get_ppp_auth_info_from_nv( &ppp_config_info, dsi_get_nv_item );

  ppp_config_info.rx_f_ptr       = ds707_pkt_rx_um_SN_data;
  ppp_config_info.tx_f_ptr       = ds707_pkt_tx_um_SN_data;
  ppp_config_info.rx_iface_ptr   = GET_PS_IFACE_PTR(pkt_instance);
  ppp_config_info.lcp_info.mode  = PPP_CP_PASSIVE;
  ppp_config_info.ipcp_info.mode = PPP_CP_ACTIVE;
  ppp_config_info.hdlc_mode      = HDLC_SW;
  ppp_config_info.bridge_dev     = PPP_RM_DEV;

  return ppp_start( PPP_UM_SN_DEV, &ppp_config_info );
} /* ds707_pkt_setup_SN_ppp() */

/*===========================================================================
FUNCTION      DS707_PKT_RX_UM_SN_DATA

DESCRIPTION   Called by PPP to get forward-link data from RLP.  Specific to
              HDR SN/1x-Data.

DEPENDENCIES  None.

RETURN VALUE  A pointer to a DSM item.  DSM item may be a packet chain.
              If there is no data to return, then returns NULL.

SIDE EFFECTS  None.
===========================================================================*/
dsm_item_type* ds707_pkt_rx_um_SN_data(void *user_data)
{
  ds707_tc_state_type       *tc_state_ptr;
/*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/
  tc_state_ptr = GET_TC_STATE_PTR(DS707_DEF_PKT_INSTANCE);
  return((dsm_item_type *)q_get(tc_state_ptr->post_rx_q_ptr));
} /* ds707_pkt_rx_um_SN_data() */

/*===========================================================================
FUNCTION      DS707_PKT_TX_UM_SN_DATA

DESCRIPTION   Called by PPP to transmit rev-link data over RLP to the base
              station.  This is for the SN link/1X-data.

DEPENDENCIES  None.

RETURN VALUE  None.

SIDE EFFECTS  None.
===========================================================================*/
void ds707_pkt_tx_um_SN_data
(
  dsm_item_type   **dsm_item_ptr_ptr,
  void *user_data
)
{
  ds707_tc_state_type       *tc_state_ptr;
/*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  tc_state_ptr = GET_TC_STATE_PTR(DS707_DEF_PKT_INSTANCE);
  dsm_enqueue(tc_state_ptr->tx_watermark_ptr, dsm_item_ptr_ptr);

} /* ds707_pkt_tx_um_SN_data() */


/*===========================================================================
FUNCTION      DS707_PKT_TRASH_RLP_TX_Q

DESCRIPTION   This function is registered with PS_IFACE.  When user calls
              this (via PS_IFACE), this will go ahead an empty the RLP
              TX watermark of all items (i.e, rev link).  By "empty" we
              mean that all data will be trashed.

              This trashes the tx watermarks of all RLP instances.
              
              THIS EXECUTES IN CALLER's CONTEXT.  May not be in DS TASK.

DEPENDENCIES  None.

RETURN VALUE  See description.

SIDE EFFECTS  None.
===========================================================================*/
void ds707_pkt_trash_rlp_tx_q
(
  ps_iface_type    *iface_ptr
)
{
  uint32                i;
  dsm_watermark_type*   temp_wmk_ptr;
/* - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - */

  MSG_MED ("Trashing 1x RLP trasmit queue",0,0,0);
  i = (uint32)(iface_ptr->client_data_ptr);
  VERIFY_PKT_INSTANCE(i);

  temp_wmk_ptr = ds707_pkt_state[i].tc_state.tx_watermark_ptr;
  if (temp_wmk_ptr != NULL)
  {
    MSG_MED (" dumped %d buffers, %d bytes",
             q_cnt( temp_wmk_ptr->q_ptr ),
             temp_wmk_ptr->current_cnt,
             0);
    dsm_empty_queue(temp_wmk_ptr);
  }
  else
  {
    MSG_MED (" (empty)",0,0,0);
  }

} /* ds707_pkt_trash_rlp_tx_q() */


/*===========================================================================
FUNCTION      DS707_PKT_ENABLE_HOLDDOWN_TIMER

DESCRIPTION   Sockets call this function to enable or disable dormancy timer

DEPENDENCIES  If multiple calls of this function are made, principle used
              is last caller wins.

              Input TRUE  - use the holddown timer when going dormant.
                    FALSE - do not use the holddown timer when dormant.

RETURN VALUE  None.

SIDE EFFECTS  None
===========================================================================*/
void ds707_pkt_enable_holddown_timer
(
  boolean is_enabled
)
{
/*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/
  enable_holddown_timer = is_enabled;

} /*  ds707_pkt_enable_holddown_timer() */


/*===========================================================================
FUNCTION      DS707_PKT_GET_LAST_SO

DESCRIPTION   Returns the last pkt SO that connected.

DEPENDENCIES  None.

RETURN VALUE  None.

SIDE EFFECTS  None,
===========================================================================*/
uint16 ds707_pkt_get_last_so(void)
{
/*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/
  return(ds707_pkt_prev_data_call_so);
} /*  ds707_pkt_get_last_so() */

/*===========================================================================
FUNCTION      DS707_PKT_IS_UM_IFACE_UP

DESCRIPTION   Returns TRUE if iface is UP, ROUTEABLE.

DEPENDENCIES  phys_link should be up or coming up.

RETURN VALUE  See description.

SIDE EFFECTS  None.
===========================================================================*/
boolean ds707_pkt_is_um_iface_up
(
ps_iface_type    *ps_iface_ptr
)
{
  ps_iface_state_enum_type    iface_state;

  if (ps_iface_ptr == NULL)
  {
    MSG_ERROR("NULL iface pointer passed in", 0, 0, 0);
    return(FALSE);
  }

  iface_state = ps_iface_state(ps_iface_ptr);

  /*--------------------------------------------------------------------
    The checks for IFACE_UP is for sockets
  --------------------------------------------------------------------*/
  if ( 
      (iface_state == IFACE_UP) || 
      (iface_state == IFACE_ROUTEABLE)
     )
  {
    return(TRUE);
  }
  else
  {
    return(FALSE);
  }
}

#endif /* FEATURE_DATA_IS707 */
