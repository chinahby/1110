#ifndef DS707_PKT_MGR_H
#define DS707_PKT_MGR_H
/*===========================================================================

                        D S 7 0 7 _ P K T _ M G R 
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

 Copyright (c) 2002 - 2010 by QUALCOMM Incorporated.  All Rights Reserved.
===========================================================================*/

/*===========================================================================
                        EDIT HISTORY FOR MODULE

  $PVCSPath: L:/src/asw/MM_DATA/vcs/ds707_pkt_mgr.h_v   1.4   27 Nov 2002 11:02:18   ajithp  $
  $Header: //source/qcom/qct/modem/data/1x/707/main/lite/inc/ds707_pkt_mgr.h#3 $ $DateTime: 2010/01/28 08:30:33 $ $Author: parmjeet $
  
-----------------------------------------------------------------------------  
when        who    what, where, why
--------    ---    -------------------------------------------------------
19/01/10    ps     Added code changes to reset requesting and current
                   profile IDs when IFACE is brought down.
05/30/09    ms     Added Support for EPZID Hysteresis.
11/07/03    ak     Added ds707_pkt_trash_rlp_tx_q prototype
11/07/03    aku    Replaced call to DSSNET_PPP_ABORT_CMD with an abort_f_ptr 
                   which would be registered by dssnet or rmsm. 
09/15/03    ak     When holddown timer expires, see if PZID needs to reorig.
05/02/02    ak     Added function to get last pkt so.
11/27/02    atp    Added ds707_pkt_get_curr_tx_wm().
11/18/02    ak     Updated file header commments.
10/29/02    atp    Added support for SDB.
09/24/02    usb    Added user_data to ppp tx and rx function callbacks  
07/31/02    aku    Externalized function ds707_pkt_is_dormant().
07/24/02    ak     Implementation for enabling/disabling holddown timer.
07/24/02    usb    Error handling from PPP configuration
07/16/02    ak     Removed NET_MODEL featurization.  Changed names of
                   exported funcs from 'pkti' -> 'pkt'.
07/15/02    aku    Exported  functions ds707_pkti_tx_um_SN_data() and
                   ds707_pkti_rx_um_SN_data()
05/10/02    ak     Added extern for signal_processing().  Fixed typo.
02/20/02    ak     First version of file.
===========================================================================*/


/*===========================================================================
                     INCLUDE FILES FOR MODULE
===========================================================================*/

#include "comdef.h"
#include "customer.h"

#ifdef FEATURE_DATA_IS707
#include "cm.h"
#include "ds3gmgr.h"
#include "dstask.h"
#include "ps_iface.h"
#include "rex.h"
#include "ds707.h"

/*===========================================================================
                            TYPEDEFS
===========================================================================*/
typedef void (*ds707_pkt_call_abort_f_ptr)(void); 

/*---------------------------------------------------------------------------
  RLP instance.  This struct defines the data for one call instance.
---------------------------------------------------------------------------*/
typedef struct
{
  word                     rlp_instance;             /* which rlp inst     */
} ds707_rlp_state_type;

/*---------------------------------------------------------------------------
  Traffic channel state info.  Contains info specific to one CM call.
  The RLP for a traffic channel could be either HDR RLP or 1X RLP.
  Data struct allows both to be intialized for a particular call, however,
  the TX & RX watermarks/queues (ie, connections to PPP/laptop) get set up
  for only one of the RLPs based on whether it is a 1X call or HDR call.
---------------------------------------------------------------------------*/
typedef struct
{
  byte                     alloc_sr_id;/* SR_ID allocated to this instance */
  ds707_so_type            so;         /* SO of this call                  */
  cm_call_id_type          call_id;    /* CM call id                       */

  ds707_rlp_state_type     rlp_1x;                   /* 1X RLP             */
  ds707_rlp_state_type     rlp_hdrsn;                /* HDR-SN RLP         */

  dsm_watermark_type      *tx_watermark_ptr;         /* RLP Tx watermark   */
  void                    (*post_rx_func_ptr)(void); /* RLP Rx callback    */
  q_type                  *post_rx_q_ptr;            /* RLP Rx queue       */

} ds707_tc_state_type;

/*---------------------------------------------------------------------------
  Overall Packet state, for one CM call.  Contains info about the Traffic
  channel and the ps_iface.
---------------------------------------------------------------------------*/
typedef struct
{
  ds707_tc_state_type        tc_state;
  ps_iface_type              ps_iface;
  acl_type                   acl;
  boolean                    snpzid_change;
  ds707_pkt_call_abort_f_ptr abort_f_ptr; 
} ds707_pkt_state_type;

/*===========================================================================
                            VARIABLES
===========================================================================*/

/*---------------------------------------------------------------------------
  Maximum number of pkt data calls supportable by IS707/IS2000 packet data
  module.  If more than one is ever supported, will have to change a lot of
  code.
---------------------------------------------------------------------------*/  
#define DS707_MAX_PKT_CALLS     1
#define DS707_DEF_PKT_INSTANCE  0

/*---------------------------------------------------------------------------
  FLOW ENABLE/DISABLE MASKs for IFACE
---------------------------------------------------------------------------*/
#define DS707_HOLDDOWN_TIMER_MASK    0x0001

#define DSRLPI_MAX_SRID_INDEX DSRLPI_MAX_RSCB_ELEMENTS

typedef struct
{
  byte  pzid;                    /*   Packet Zone Identifier               */
  word  sid;                     /*   System Identifier                    */ 
  word  nid;                     /*   Network Identifier                   */
#ifdef FEATURE_DS_EPZID_HYSTERESIS
  dword hyst_exp_time;
#endif /* FEATURE_DS_EPZID_HYSTERESIS */
} ds707_epzid_type;

extern ds707_epzid_type ds707_pzid_curr_epzid; 

/*---------------------------------------------------------------------------
  Packet call states.  Each instance encapsulates one traffic channel call
  and it's associate IFACE.
---------------------------------------------------------------------------*/
extern ds707_pkt_state_type    ds707_pkt_state[DS707_MAX_PKT_CALLS];

/*===========================================================================
                       EXTERNAL MACRO DEFINITIONS
===========================================================================*/

/*===========================================================================
MACRO         GET_PS_IFACE_PTR

DESCRIPTION   Given the pkt instance, returns the ps iface_ptr.

DEPENDENCIES  None.

RETURN VALUE  See description.

SIDE EFFECTS  None.
===========================================================================*/
#define GET_PS_IFACE_PTR(x) &(ds707_pkt_state[x].ps_iface)

/*===========================================================================
                       EXTERNAL FUNCTION DEFINITIONS
===========================================================================*/

/*===========================================================================
                        DS707 PKT MGR INITIALIZATION
===========================================================================*/
/*===========================================================================
FUNCTION      DS707_PKT_MGR_INIT

DESCRIPTION   Called once at mobile power-up.  Registers the appropriate 
              functionality with 3G DSMGR.
 
DEPENDENCIES  None.

RETURN VALUE  Signals that DS should look for.

SIDE EFFECTS  None
===========================================================================*/
rex_sigs_type ds707_pkt_mgr_init(void);

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
);
/*===========================================================================
                           3G DSMGR HANDLERS
===========================================================================*/
/*===========================================================================
FUNCTION      DS707_PKT_CALL_ID_HDLR

DESCRIPTION   Called after call originates.  Gives the CM call id to this
              mode specific handler.
 
DEPENDENCIES  None.

RETURN VALUE  None

SIDE EFFECTS  None
===========================================================================*/
void ds707_pkt_call_id_hdlr
(
  cm_call_id_type        cm_call_id,
  void                  *mode_spec_info_ptr
);

/*===========================================================================
FUNCTION      DS707_PKT_ORIG_HDLR

DESCRIPTION   Called from 3G DSMGR when originating a packet data call.
 
DEPENDENCIES  Watermarks to RLP already set up correctly.  Passed in ptrs are
              not NULL.

RETURN VALUE  DS3G_SUCCESS - call can be made.  
              DS3G_FAILURE - call can't be made.

SIDE EFFECTS  None.
===========================================================================*/
boolean ds707_pkt_orig_hdlr
(
  const byte                           *dial_string,                    
        uint8                           dial_string_len,
        cm_cdma_orig_params_s_type     *cdma_orig_params_ptr,
        boolean                        *cdma_params_changed,
        cm_gw_cs_orig_params_s_type    *gw_cs_orig_params_ptr,
        boolean                        *gw_cs_params_changed,
        cm_gw_ps_orig_params_s_type    *gw_ps_orig_params_ptr,
        boolean                        *gw_ps_params_changed,
        void                           *mode_spec_info_ptr
);

/*===========================================================================
FUNCTION      DS707_PKT_INC_CALL_HDLR

DESCRIPTION   Called when CM receives a page from BS.  Only time we accept
              a call is when we are dormant.
 
DEPENDENCIES  None.

RETURN VALUE  Will either REJECT the call or tell CM to answer the call.

SIDE EFFECTS  None
===========================================================================*/
ds3g_incom_e_type ds707_pkt_inc_call_hdlr
(
  cm_call_id_type         cm_call_id,
  cm_ans_params_s_type   *ans_params, /* Answer params sent to Call Mgr    */
  cm_end_params_s_type   *end_params  /* end params if call rejected       */
);

/*===========================================================================
FUNCTION      DS707_PKT_SWITCH_CALLS_HDLR

DESCRIPTION   Called when CM receives an incoming page for the same type of
              call as what we are originating.
              
              In this case, we want to end the origination, but accept the
              incoming page.  We must then switch our call_id to the 
              incoming call id.
 
DEPENDENCIES  None.

RETURN VALUE  TRUE  - accept incoming call.  Tell CM to end the existing
                      call.
              FALSE - 3G DSMGR should continue with normal incoming call
                      process.  This modules does nothing.

SIDE EFFECTS  None
===========================================================================*/
boolean ds707_pkt_switch_calls_hdlr
(
  cm_call_id_type        inc_call_id,
  cm_call_id_type        exist_call_id,
  cm_ans_params_s_type  *ans_params,  /* parms for call that is accepted  */
  cm_end_params_s_type  *end_params   /* parms for call that is ended     */
);

/*===========================================================================
FUNCTION      DS707_PKT_CALL_CONN_HDLR

DESCRIPTION   Called by 3G DSMGR when traffic channel connects.
 
DEPENDENCIES  None.

RETURN VALUE  DS3G_SUCCESS - call connected okay
              DS3G_FAILURE - call failed

SIDE EFFECTS  None.
===========================================================================*/
boolean ds707_pkt_call_conn_hdlr
(
  cm_call_id_type           call_id,
  cm_call_mode_info_s_type  *mode_info_ptr,
  boolean                   *delay_connect,
  cm_end_params_s_type      *end_params
);

/*===========================================================================
FUNCTION      DS707_PKT_END_CALL_HDLR

DESCRIPTION   Called by 3G DSMGR before telling CM to end a call.  Only
              thing done here is setting the ORDQ value correctly and
              letting CM know that this was a packet call.

DEPENDENCIES  Called in DS TASK context.

RETURN VALUE  None.

SIDE EFFECTS  None.
===========================================================================*/
void ds707_pkt_end_call_hdlr
(
  cm_call_id_type        call_id,
  cm_end_params_s_type  *end_params,
  void                  *mode_spec_info_ptr
);

/*===========================================================================
FUNCTION      DS707_PKT_CALL_ENDED_HDLR

DESCRIPTION   Called by 3G DSMGR when a call release has been received from
              CM.
 
DEPENDENCIES  None.

RETURN VALUE  None

SIDE EFFECTS  None.
===========================================================================*/
void ds707_pkt_call_ended_hdlr
(
  ds_cmd_type               *cmd_ptr
);

/*===========================================================================
                             PHYS_LINK_DOWN
===========================================================================*/
/*===========================================================================
FUNCTION      DS707_PKT_PHYS_LINK_DOWN_0

DESCRIPTION   Called via the 1x IFACE.  Tears down the traffic channel. 
              Sends a message to the DS task which will cause the 
              channel to be torn down.
              
              Specific to the zero'th 1x pkt interface.
 
DEPENDENCIES  None.

RETURN VALUE  None

SIDE EFFECTS  None.
===========================================================================*/
int ds707_pkt_phys_link_down_0
(
  ps_iface_type     *iface_ptr,
  void              *not_used
);

/*===========================================================================
FUNCTION      DS707_PKT_PHYS_LINK_DOWN_CMD_HDLR

DESCRIPTION   Called in DS TASK context in response to either:

                1) a phys down command on the 1X iface or
                2) AT+CTA timer expiring
                
DEPENDENCIES  Called in DS TASK context.

RETURN VALUE  None.

SIDE EFFECTS  None.
===========================================================================*/
void ds707_pkt_phys_link_down_cmd_hdlr
(
  word pkt_instance
);

/*===========================================================================
                             PHYS_LINK_UP
===========================================================================*/
/*===========================================================================
FUNCTION      DS707_PKT_PHYS_LINK_UP_0

DESCRIPTION   Called via the 1x IFACE.  Brings up the 1X traffic channel.
              Specific to the zero'th 1X pkt interface.
              
              Since this is called by some module external to 1X, DRS_BIT
              is always set to TRUE (always data to be sent).
 
DEPENDENCIES  Watermarks to RLP already set up correctly.

RETURN VALUE  None.

SIDE EFFECTS  None.
===========================================================================*/
int ds707_pkt_phys_link_up_0
(
  ps_iface_type    *iface_ptr,
  void             *drs_bit
);

/*===========================================================================
FUNCTION      DS707_PKT_PHYS_LINK_UP_CMD_HDLR

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
void ds707_pkt_phys_link_up_cmd_hdlr
(
  boolean     drs_bit,
  word        pkt_instance
);

/*===========================================================================
                                IFACE_UP
===========================================================================*/
/*===========================================================================
FUNCTION      DS707_PKT_IFACE_UP_0

DESCRIPTION   Called via the 1x IFACE.  Brings up the 1X iface.
              Currently does nothing (UNFINISHED).
              
DEPENDENCIES  None

RETURN VALUE  None.

SIDE EFFECTS  None.
===========================================================================*/
int ds707_pkt_iface_up_0
(
  ps_iface_type    *iface_ptr,
  void             *not_used
);

/*===========================================================================
FUNCTION      DS707_PKT_IFACE_UP_CMD_HDLR

DESCRIPTION   Called in DS TASK context in response to an iface up command
              on the 1X iface.
                  
DEPENDENCIES  Called in DS TASK context.

RETURN VALUE  None.

SIDE EFFECTS  None.
===========================================================================*/
void ds707_pkt_iface_up_cmd_hdlr
(
  word        pkt_instance
);

/*===========================================================================
                                IFACE_DOWN
===========================================================================*/
/*===========================================================================
FUNCTION      DS707_PKT_IFACE_DOWN_0

DESCRIPTION   Called via the 1x IFACE.  Tears down the 1X iface.
              
DEPENDENCIES  None

RETURN VALUE  None.

SIDE EFFECTS  None.
===========================================================================*/
int ds707_pkt_iface_down_0
(
  ps_iface_type    *iface_ptr,
  void             *not_used
);

/*===========================================================================
FUNCTION      DS707_PKT_IFACE_DOWN_CMD_HDLR

DESCRIPTION   Called in DS TASK context in response to an iface down command
              on the 1X iface.
                  
DEPENDENCIES  Called in DS TASK context.  Called only when PS_IFACE is up.

RETURN VALUE  None.

SIDE EFFECTS  None.
===========================================================================*/
void ds707_pkt_iface_down_cmd_hdlr
(
  word        pkt_instance
);

/*===========================================================================
                             OTHER FUNCTIONS
===========================================================================*/
/*===========================================================================
FUNCTION      DS707_CTA_TIMER_EXPIRED

DESCRIPTION   Called when AT+CTA timer expires.  Two options - either
              go dormant or end the session completely.

DEPENDENCIES  None.

RETURN VALUE  None.

SIDE EFFECTS  None.
===========================================================================*/
void ds707_cta_timer_expired
(
  word        pkt_instance
);

/*===========================================================================
FUNCTION      DS707_PZSNID_CHANGED

DESCRIPTION   Called when either PZID, SID, or NID has changed, requiring
              mobile to come out of dormancy.

DEPENDENCIES  runs in DS TASK context.

RETURN VALUE  None.

SIDE EFFECTS  None.
===========================================================================*/
void ds707_pzsnid_changed
(
  ds_cmd_enum_type    cmd,
  word                pkt_instance
);

/*===========================================================================
FUNCTION      DS707_PKT_REORIG

DESCRIPTION   Called when the PS to RLP watermark goes non-empty when
              dormant.

DEPENDENCIES  None

RETURN VALUE  None.

SIDE EFFECTS  None.
===========================================================================*/
void ds707_pkt_reorig(void);

/*===========================================================================
FUNCTION      DS707_PKT_IS_PZID_ORIG_ALLOWED

DESCRIPTION   Returns TRUE or FALSE, indicating if an orig is allowed on the
              specified IFACE.
              
              Done solely based on holddown and retry delay timers.

DEPENDENCIES  None.

RETURN VALUE  TRUE - can originate.  FALSE - cannot originate.

SIDE EFFECTS  None.
===========================================================================*/
boolean ds707_pkt_is_pzid_orig_allowed
(
  uint32 pkt_instance
);

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
);

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
);

/*===========================================================================
FUNCTION      DS707_PKT_GET_PKT_STATE_PTR_INST

DESCRIPTION   Given a packet state instance, returns the pointer to the
              pkt state.
 
DEPENDENCIES  None.

RETURN VALUE  pointer to a pkt state

SIDE EFFECTS  None.
===========================================================================*/
ps_iface_type* ds707_pkt_get_iface_ptr_inst
(
  word pkt_instance
);

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
);

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
);

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
);

/*===========================================================================
FUNCTION      DS_VERIFY_SO

DESCRIPTION   Called by MC.  Asks DS if the originating SO is correct.
 
DEPENDENCIES  None

RETURN VALUE  None

SIDE EFFECTS  None.
===========================================================================*/
word ds_verify_so
(
  word so_to_verify           /* SO to verify                              */
);

/*===========================================================================
FUNCTION      DS707_PKT_GET_ACTIVE_IFACE_PTR

DESCRIPTION   Called to indicate if a call is active (i.e., moving data).

DEPENDENCIES  None

RETURN VALUE  Pointer to the active data call.  Else, if no call is active,
              returns NULL.

SIDE EFFECTS  None
===========================================================================*/
ps_iface_type* ds707_pkt_get_active_iface_ptr(void);

/*===========================================================================
FUNCTION      DS707_PKT_SETUP_SN_PPP

DESCRIPTION   Called when IFACE is coming up, to intialize PPP.
 
DEPENDENCIES  None

RETURN VALUE  TRUE for successful PPP configuration, FALSE otherwise

SIDE EFFECTS  None
===========================================================================*/
int ds707_pkt_setup_SN_ppp
(
  uint32     pkt_instance
);

/*===========================================================================
FUNCTION      DS707_PKT_RX_UM_SN_DATA

DESCRIPTION   Called by PPP to get forward-link data from RLP.  Specific to
              HDR SN/1x-Data.

DEPENDENCIES  None.

RETURN VALUE  A pointer to a DSM item.  DSM item may be a packet chain.
              If there is no data to return, then returns NULL.

SIDE EFFECTS  None.
===========================================================================*/
dsm_item_type* ds707_pkt_rx_um_SN_data
(
  void *user_data
);
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
);

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
);

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
);

/*===========================================================================
FUNCTION      DS707_PKT_IS_DORMANT

DESCRIPTION   Returns TRUE if iface is dormant.  Else returns FALSE.

DEPENDENCIES  None.

RETURN VALUE  See description.

SIDE EFFECTS  None.
===========================================================================*/
LOCAL boolean ds707_pkt_is_dormant
(
  ps_iface_type      *iface_ptr
);

/*===========================================================================
FUNCTION      DS707_PKT_GET_PREV_CALL_SO

DESCRIPTION   This function will return SO of previous call.

DEPENDENCIES  None

RETURN VALUE  None.

SIDE EFFECTS  None.
===========================================================================*/
word ds707_pkt_get_prev_call_so
(
  void
);


/*===========================================================================
FUNCTION      DS707_PKT_GET_PREV_SR_ID

DESCRIPTION   This function will return SR ID of previous call.

DEPENDENCIES  None

RETURN VALUE  None.

SIDE EFFECTS  None.
===========================================================================*/
byte ds707_pkt_get_prev_sr_id
(
  void
);

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
);

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
);

/*===========================================================================
FUNCTION      DS707_PKT_GET_LAST_SO

DESCRIPTION   Returns the last pkt SO that connected.

DEPENDENCIES  None.

RETURN VALUE  None.

SIDE EFFECTS  None,
===========================================================================*/
uint16 ds707_pkt_get_last_so(void);

#endif /* FEATURE_DATA_IS707 */
#endif /* DS707_PKT_MGR_H    */

