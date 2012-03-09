/*===========================================================================

                          D S 7 0 7 _ R M S M . C
DESCRIPTION

  The Data Services IS707 Rm State Machine source file. This state machine
  manages the Um and Rm interfaces for certain IS707 packet calls.
  Specifically RMSM is used to support traditional Relay, SIP Network Model
  and Mobile IP type IS707 calls which are a special case of the
  generic multimode PS architecture.

EXTERNALIZED FUNCTIONS
  ds707_rmsm_init()
    Initialize the Rm State Machine.
  ds707_rmsm_post_event()
    Post an event to Rm State Machine.
  ds707_rmsm_process_cmd()  
    Process a cmd posted to Rm State Machine.
  ds707_rmsm_is_packet_call()
    Is RM in IS707 laptop pkt data call.
  ds707_rmsm_is_mip_up()
    Is RM in IS707 laptop MobileIP pkt data call.    
  ds707_rmsm_dial_str_cb()
    Dial string handler for IS707 pkt data calls.
  ds707_rmsm_is_iface_in_use()
    Is IS707 iface in a laptop pkt data call.
  ds707_rmsm_rt_acl_post_proc()
    Post processing function for IS707 iface ACL.

Copyright (c) 2001 - 2009 by QUALCOMM, Incorporated.  All Rights Reserved.
===========================================================================*/
/*===========================================================================

                            EDIT HISTORY FOR FILE
  $Header: //source/qcom/qct/modem/data/1x/707/main/lite/src/ds707_rmsm.c#8 $

when        who    what, where, why
--------    ---    ----------------------------------------------------------
08/31/09    sn     Fixed the issue of laptop/PC not receiving term ACK from AT 
                   when disconnecting UART network model data call.
04/29/09    sn     Ported support for call throttle feature (DCTM).
04/16/09    sn     Merged support for separate NAI for Tethered SIP and MIP 
                   calls. Featurized under FEATURE_DS_MULTIPLE_NAI.
11/24/08    psng   Added support to use legacy NV items if number of valid SIP
                   profiles is zero.
11/20/08    psng   Added multiple profile id look up support for Laptop call
10/08/08    sn     Modified to use TASKLOCK and TASKFREE for call bring up only.
06/18/08    sn     Fixed compiler warnings.
10/21/04    vr     Send UP/ROUTEABLE indication on Rm and Um when a relay
                   model call comes up.
11/11/03    jd     If forcing Um resync and call to ppp_resync() fails,
                   end call by going to WAITING_FOR_UM_DOWN state.
11/11/03    snn    Removed checks for FLOW Control before setting the watermark
                   levels.
11/07/03    jd     Trash RLP tx queue when PPP resync occurs to avoid sending
                   IP traffic on link before NCP is configured.
11/07/03    aku    Added abort callback to handle call aborts for SIP Laptop 
                   calls
07/04/03    usb    Resetting SIO iface IP address after Rm PPP comes up.
                   Starting Rm PPP in passive mode for MIP calls.
06/04/03    usb    Appropriately setting SIO iface and phys link state.
05/21/03    vas    Added support for setting up SIO/PPP <->RLPwatermarks for
                   HDR
05/05/03    usb    Added generic DSM memory mask to enable flow at the end
                   of a call. cleaned up process_rm_wants_pkt_call().
                   ppp_get_dev_setup() is only called if Um and Rm options
                   mismatch during MIP call setup.
05/05/03    usb    Incorporated Code review comments. Moved state enum and
                   RMSM info block from .h file. Moved all state 
                   machine specific variables into info_block. Improved
                   error handling.                   
05/02/03    usb    ps_iface_create() prototype change.
04/11/03    ak     Updated all vars/functions to include ds707_, as there
                   were naming collisions with GPRS.
04/07/03    usb    Renamed ppp_set_resync_opts() to ppp_update_ipcp_config()
03/10/03    jd/usb Renegotiate DNS addresses on Um PPP resync
02/02/03    usb    Updated snooop callback functions with new return values
01/30/03    usb    For relay calls, reset PPP mode at the end of the call,
                   moved rmsm state change call from tear_down_cb to process
                   event functions.
01/20/03    ar     Added dsat_dial_modifier_info_type to 
                   ds707_rmsm_dial_str_cb interface.
11/25/02    usb    Removed code to extract/free meta_info_ptr. Should be 
                   added later when RM sockets are suported.
11/19/02    usb    Added ACL processing and post processing functions to 
                   check if UM iface is in use, added flow event handling 
                   for relay calls
11/14/02    jd/usb Added mobile IP support
                   Optimize call failure scenarios to avoid extraneous events
                   If rm_wants_pkt_call_ev fails before state transition, 
                   dereg any reg'd callbacks.
11/14/02           Renamed RMSM_WAITING_FOR_RM_UM_DOWN_STATE to
                   RMSM_RM_UP_WAITING_FOR_UM_DOWN_STATE
11/14/02    jd     Removed um_mip_changed, added rmsm_is_packet_call()
11/13/02    usb    Miscellaneous fixes - Renamed RLP wms, revised flow 
                   control, fixed flow control masks being used, added 
                   enabling/disabling of ACLs to disallow sockets call.
09/30/02    atp    Added support for Release-A. (Changed wmk count settings).
09/24/02    usb    Added user_data to ppp tx and rx function callbacks
09/18/02    mvl    Added user_data to ppp event callbacks.
09/18/02    usb    Miscellaneous fixes, fixed all TODO's, added bring_up_cmd
                   for 707 iface, better error handling, cleaned up PPP 
                   configuration sequence to fix some edge conditions,
                   removed FEATURE_DS_NET_MODEL, featurized the file
09/11/02    usb    Fixed bug so that orig_tear_down_cmd_cb for 1X is always 
                   restored properly at the end of a call.
09/10/02    ak     Updated for new dial string handler.
09/09/02    ak     Updated sio_func_table for at&D=0 dtr callback.
09/06/02    usb    Updated PPP event handlers prototype
09/06/02    ak     Always clean up rx_queue when going NULL.  Always set
                   RLP to NULL defaults when going NULL.
09/06/02    ak     on sio_change_mode, had watermarks backwards.
09/03/02    ak     Updated for ps_iface cback buf interface.  Added esc_seq
                   handler to sio func table, to handle dtr change when 
                   at&D=1.
08/28/02    usb    PPP down event handling on Um and Rm to handle failures 
                   during PPP resync
08/20/02    ak     Updated for multiple watermarks.
08/13/02    mvl    updated ps_iface state macros for name change. Changed
                   call to ppp_set_mode() as name changed. 
07/30/02    usb    Cleaned up call up termination code to make sure Rm
                   call cleanup happens only after Um
07/25/02    mvl    Updates for PPP renaming.
07/24/02    usb    Handling errors from PPP configuration
07/23/02    usb    Call termination edge case fixes
07/12/02    usb    Actually creating and deleting the SIO iface now
07/12/02    usb    Changes per the new PPP interface
07/11/02    usb    Renamed flow control constants
07/03/02    ak     Change errno's to ps_iface funcs from uint16 -> int16.
07/01/02    ak     Now first chng_mode to PKT and then send CONNECT.
05/14/02    usb    Re-created module (original filename - dsmip_rmsm.c)
===========================================================================*/


/*===========================================================================

                          INCLUDE FILES FOR MODULE

===========================================================================*/

#include "comdef.h"
#include "customer.h"

#ifdef FEATURE_DATA_IS707
#include "assert.h"
#include "err.h"
#include "msg.h"
#include "queue.h"

#include "ds707_rmsm.h"
#include "ps_iface.h"
#include "dsat707extctab.h"
#include "ds3gsiolib.h"
#include "ds707_pkt_mgr.h"
#include "ds707_cta_rlp.h"
#include "rdevmap.h"
#include "ds_flow_control.h"
#include "ds707.h"
#include "dserrno.h"
#include "ps_route.h"
#include "ps_svc.h"
#include "ps_ppp_netmodel.h"
#include "ps_ppp.h"
#include "ps_ppp_fsm.h"

#ifdef FEATURE_DS_MOBILE_IP
#include "ps_mip_task.h"
#include "dsat707mipctab.h"
#include "ps_ppp_snoop.h"
#include "dsmip_io.h"
#include "ps707_dssnet.h"

#ifdef FEATURE_DS_MOBILE_IP_PERF
#include "dsmip_perf.h"
#endif /* FEATURE_DS_MOBILE_IP_PERF */

#endif /* FEATURE_DS_MOBILE_IP */

#ifdef FEATURE_HDR
#error code not present
#endif /* FEATURE_HDR */

#ifdef FEATURE_DS_MULTIPLE_PROFILES
#include "dstaski.h"
#include "ds707_data_session_profile.h"
#endif /* FEATURE_DS_MULTIPLE_PROFILES */

#ifdef FEATURE_CALL_THROTTLE
#include "dsdctm.h"
#endif /* FEATURE_CALL_THROTTLE */


/*= = = = = = = = = = = = = = = = = = = = = = = = = = = = = = = = = = = = = =
                            INTERNAL DEFINITIONS
= = = = = = = = = = = = = = = = = = = = = = = = = = = = = = = = = = = = = =*/

/*---------------------------------------------------------------------------
  Declaration of the states of the state machine.
---------------------------------------------------------------------------*/
typedef enum
{
  DS707_RMSM_MIN_STATE = -1,
  DS707_RMSM_NULL_STATE = 0,                    /* Initial state           */
  DS707_RMSM_WAITING_FOR_UM_UP_RELAY_OR_SIP_STATE = 1, 
                              /* Um coming up - Relay or SIP netmodel call */
  DS707_RMSM_UM_RM_UP_RELAY_OR_SIP_STATE = 2,
                              /* Rm and Um up - Relay or SIP netmodel Call */
  DS707_RMSM_WAITING_FOR_UM_DOWN_STATE = 3,    /* Um going down            */
  DS707_RMSM_WAITING_FOR_UM_UP_MIP_STATE = 4,  /* Um coming up - MIP Call  */
  DS707_RMSM_WAITING_FOR_RM_UP_STATE = 5,      /* PPP coming up on Rm      */
  DS707_RMSM_UM_RM_UP_MIP_STATE = 6,           /* Rm and Um up - MIP call  */
  DS707_RMSM_RESYNCING_UM_STATE = 7,     /* Resyncing Um with Rm options   */
  DS707_RMSM_RM_UP_WAITING_FOR_UM_DOWN_STATE = 8, /* Um going down, Rm up  */
  DS707_RMSM_WAITING_FOR_RM_DOWN_STATE = 9,    /* Rm going down            */
  DS707_RMSM_MAX_STATE
} ds707_rmsmi_state_type;

/*---------------------------------------------------------------------------
  The Rm state machine's information block
---------------------------------------------------------------------------*/
typedef struct
{
  ds707_rmsmi_state_type state;        /* State machine state              */
  boolean inited;                      /* State machine been initialized?  */
  boolean is_in_use;                   /* Is this RM-UM iface pair in call */
  ps_iface_type *IS707pkt_iface_ptr;   /* IS707 iface used for data call   */
  ps_iface_type sio_707pkt_iface;      /* SIO iface used for data call     */
  ps_iface_cmd_f_ptr_type orig_tear_down_f_ptr; /* fn ptr in IS707 iface   */
  ps_iface_cmd_f_ptr_type orig_bring_up_f_ptr;  /* fn ptr in IS707 iface   */
  ppp_dev_opts_type rm_ppp_config;     /* PPP configuration on RM Device   */
  void *iface_down_buf_ptr;            /* Event buf for UM Iface DOWN      */
  void *phys_link_down_buf_ptr;        /* Event buf for UM Phys Link DOWN  */
  void *phys_link_up_buf_ptr;          /* Event buf for UM Phys Link UP    */
  void *flow_enable_buf_ptr;           /* Event buf for UM Flow ON         */
  void *flow_disable_buf_ptr;          /* Event buf for UM Flow OFF        */
  q_type rlp_fwd_q;                    /* RLP FWD Link q, For Relay mode   */
  dsm_watermark_type sio_fwd_wmk;      /* SIO FWD Link wm, SIO to laptop   */
  q_type sio_fwd_wmk_q;                /* Corresponding SIO FWD LINK queue */
  dsm_watermark_type sio_rev_wmk;      /* SIO REV Link wm, laptop to SIO   */
  q_type sio_rev_wmk_q;                /* Corresponding SIO REV LINK queue */
} ds707_rmsmi_info_type;

LOCAL ds707_rmsmi_info_type ds707_rmsmi_info;

/*---------------------------------------------------------------------------
  Function table registered with siolib to manage serial port
---------------------------------------------------------------------------*/
LOCAL ds3g_siolib_func_tbl_type ds707_rmsmi_sio_func_table;

#ifndef T_ARM
/*---------------------------------------------------------------------------
  These names need to be kept in sync with the enums which they describe.
---------------------------------------------------------------------------*/
char *ds707_rmsmi_state_names[DS707_RMSM_MAX_STATE] =
{
  "Null State",
  "Waiting for Um to come up in Relay or SIP mode",
  "Um/Rm up in Relay or SIP mode",
  "Waiting for Um to go down",
  "Waiting for Um to come up in MIP mode",
  "Waiting for Rm to come up",
  "Um/Rm up in MIP mode",
  "Resyncing Um",
  "Rm up, waiting for Um to go down",
  "Waiting for Rm to go down"
};

char *ds707_rmsmi_event_names[DS707_RMSM_MAX_EV] =
{
  "Rm wants packet call",
  "Rm interface is down",
  "Um interface is down",
  "Um physical link is up",
  "Um physical link is down",
  "Um PPP is down",
  "Rm PPP is up",
  "Um PPP is up",
  "Um MIP is up",
  "Um MIP is down"
};
#endif /* T_ARM */


/*===========================================================================
                            Forward Declarations
===========================================================================*/

/*---------------------------------------------------------------------------
  RMSM state management and event processing functions
---------------------------------------------------------------------------*/

LOCAL void ds707_rmsmi_transition_state
(
  ds707_rmsmi_state_type new_state               /* state to tranistion to */
);

LOCAL void ds707_rmsmi_process_rm_wants_pkt_call_ev( void );
LOCAL void ds707_rmsmi_process_rm_iface_down_ev( void );
LOCAL void ds707_rmsmi_process_um_iface_down_ev( void );
LOCAL void ds707_rmsmi_process_um_phys_link_up_ev( void );
LOCAL void ds707_rmsmi_process_um_phys_link_down_ev( void );
LOCAL void ds707_rmsmi_process_um_ppp_down_ev( void );

#ifdef FEATURE_DS_MOBILE_IP
LOCAL void ds707_rmsmi_process_rm_ppp_up_ev( void );
LOCAL void ds707_rmsmi_process_um_mip_up_ev( void );
LOCAL void ds707_rmsmi_process_um_mip_down_ev( void );

LOCAL snoop_cb_ret_val_enum_type ds707_rmsmi_netmdl_term_req_cb
(
  ppp_dev_enum_type device,            /* the arrival interface            */
  uint16 protocol,                     /* protocol of packet               */
  dsm_item_type **item_head_ptr        /* item containing data             */
);

LOCAL snoop_cb_ret_val_enum_type ds707_rmsmi_rm_resync_cb
(
  ppp_dev_enum_type device,            /* the arrival interface            */
  uint16 protocol,                     /* protocol of packet               */
  dsm_item_type **item_head_ptr        /* item containing data             */
);

#ifdef FEATURE_DS_MULTIPLE_NAI
void ds707_rmsm_get_mip_session_info_cb
(
  byte                     active_mip_profile,
  mip_session_info_type *  session
);
#endif /* FEATURE_DS_MULTIPLE_NAI */
#endif /* FEATURE_DS_MOBILE_IP */

/*---------------------------------------------------------------------------
  RMSM callback functions registered with various external modules
---------------------------------------------------------------------------*/

LOCAL dsat_mode_enum_type ds707_rmsmi_process_esc_seq_cb(void);

LOCAL void ds707_rmsmi_process_dtr_sig_cb
(
  boolean is_asserted                /* Flag to indicate current DTR state */
);

LOCAL void ds707_rmsmi_call_abort_cb( void );

LOCAL int rmsmi_707_pkt_bring_up_cb
(
  ps_iface_type *this_iface_ptr,
  void *user_data
);

LOCAL int rmsmi_707_pkt_tear_down_cb
(
  ps_iface_type *this_iface_ptr,
  void *user_data
);

LOCAL void rmsmi_707_pkt_event_handler_cb
(
  ps_iface_type *this_iface_ptr,           /* Pointer to the interface     */
  ps_iface_event_enum_type   event,        /* Event type occurred          */
  ps_iface_event_info_u_type event_info,   /* Event specific info block    */
  void *user_data_ptr           /* Data passed during event registration   */
);

LOCAL void ds707_rmsmi_ppprm_event_handler_cb
(
  ppp_dev_enum_type       ppp_dev,
  ppp_protocol_e_type     protocol,              /* PPP Protocol           */
  ppp_event_e_type        ppp_event,
  void                    *user_data,
  uint32                  session_handle,
  ppp_fail_reason_e_type  ppp_fail_reason
);

LOCAL void ds707_rmsmi_pppum_event_handler_cb
(
  ppp_dev_enum_type       ppp_dev,
  ppp_protocol_e_type     protocol,              /* PPP Protocol           */
  ppp_event_e_type        ppp_event,
  void                    *user_data,
  uint32                  session_handle,
  ppp_fail_reason_e_type  ppp_fail_reason
);

/*---------------------------------------------------------------------------
  SIO watermark configuration and event hanlding functions
---------------------------------------------------------------------------*/

LOCAL void ds707_rmsmi_setup_sio_watermarks
(
  dsm_watermark_type *tx_wm_ptr,
  dsm_watermark_type *rx_wm_ptr
);

LOCAL void ds707_rmsmi_sio_rx_gone_empty_func( void );
LOCAL void ds707_rmsmi_sio_rx_non_empty_func( void );
LOCAL void ds707_rmsmi_sio_rx_lowater_func( void );
LOCAL void ds707_rmsmi_sio_rx_hiwater_func( void );
LOCAL void ds707_rmsmi_sio_tx_rlp_data( void );

LOCAL void ds707_rmsmi_sio_tx_ppp_data
(
  dsm_item_type **item_head_ptr,
  void *user_data
);

LOCAL dsm_item_type* ds707_rmsmi_sio_rx_ppp_data( void *user_data );

LOCAL void ds707_rmsmi_call_abort_cback(void);

/*= = = = = = = = = = = = = = = = = = = = = = = = = = = = = = = = = = = = = =
                        EXTERNAL FUNCTION DEFINTIONS
= = = = = = = = = = = = = = = = = = = = = = = = = = = = = = = = = = = = = =*/

/*===========================================================================
  FUNCTION DS707_RMSM_INIT()

  DESCRIPTION
    This function initializes the state machine.

  PARAMETERS
    iface_ptr: Pointer to the interface which is used by this instance
               of Rm state machine to bring up pkt calls.

  RETURN VALUE
    TRUE on success
    FALSE on failure

  DEPENDENCIES
    Should only be called once at powerup

  SIDE EFFECTS
    None
===========================================================================*/
boolean ds707_rmsm_init
(
  ps_iface_type *iface_ptr
)
{
/*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/
  /*-------------------------------------------------------------------------
    RMSM should be initialized only once.  
    Note the "if" check below assumes that ds707_rmsmi_info is a defined on
    heap so that all its members get inited to 0 (FALSE).    
  -------------------------------------------------------------------------*/  
  if(ds707_rmsmi_info.inited == TRUE)
  {
    MSG_HIGH("RMSM already initialized", 0, 0, 0);
    return TRUE;
  }

  ds707_rmsmi_info.state = DS707_RMSM_NULL_STATE;  
  ds707_rmsmi_info.is_in_use = FALSE;

  /*-------------------------------------------------------------------------
    All packet data laptp calls require two interfaces. SIO iface is created
    and managed by RMSM. Note that iface is left DISABLED and bring up and 
    tear down are not registered since sockets origination over SIO is not 
    supported by this RMSM. IS707 iface which is managed by 707 pkt 
    controller is passed to this funtion and is stored for further usage.
  -------------------------------------------------------------------------*/
  ps_iface_create( &ds707_rmsmi_info.sio_707pkt_iface, SIO_IFACE, NULL );
  
  if(PS_IFACE_IS_VALID(iface_ptr) == FALSE)
  {
    ERR_FATAL("Invalid iface 0x%x for IS707", iface_ptr, 0, 0);
    return FALSE;
  }
  ds707_rmsmi_info.IS707pkt_iface_ptr = iface_ptr;

  /*-------------------------------------------------------------------------
    Initialize the sio callbacks.  Packet data always pays attention to
    DTR signal, regardless of AT&D setting.  Which means that if AT&D is
    de-asserted by the laptop, then the mobile always ends the data session
    (goes NULL), irrespective of the AT&D value.
  -------------------------------------------------------------------------*/
  memset(&ds707_rmsmi_sio_func_table, 0, sizeof(ds707_rmsmi_sio_func_table));

  ds707_rmsmi_sio_func_table.at_escape_sequence_handler = 
                                              ds707_rmsmi_process_esc_seq_cb;
  ds707_rmsmi_sio_func_table.dtr_changed_sig_handler    = 
                                              ds707_rmsmi_process_dtr_sig_cb;
  ds707_rmsmi_sio_func_table.dtr_changed_sig_handler_ampd0 = 
                                              ds707_rmsmi_process_dtr_sig_cb;
  
  /*-------------------------------------------------------------------------
    Initialize the watermarks and queues used to hook up to SIO in relay and
    network model.
  -------------------------------------------------------------------------*/
  (void)q_init( &ds707_rmsmi_info.rlp_fwd_q );

  (void)q_init( &ds707_rmsmi_info.sio_rev_wmk_q );
  (void)q_init( &ds707_rmsmi_info.sio_fwd_wmk_q );

  ds707_rmsmi_info.sio_rev_wmk.current_cnt = 0;
  ds707_rmsmi_info.sio_fwd_wmk.current_cnt = 0;

  ds707_rmsmi_info.sio_rev_wmk.q_ptr       = &ds707_rmsmi_info.sio_rev_wmk_q;
  ds707_rmsmi_info.sio_fwd_wmk.q_ptr       = &ds707_rmsmi_info.sio_fwd_wmk_q;

  /*-------------------------------------------------------------------------
    Initialize the ps_iface cback buf_ptrs for IS707 iface event handling
  -------------------------------------------------------------------------*/
  ds707_rmsmi_info.iface_down_buf_ptr     = ps_iface_alloc_event_cback_buf(
                                              rmsmi_707_pkt_event_handler_cb,
                                              NULL
                                            );

  ds707_rmsmi_info.phys_link_down_buf_ptr = ps_iface_alloc_event_cback_buf(
                                              rmsmi_707_pkt_event_handler_cb,
                                              NULL
                                            );

  ds707_rmsmi_info.phys_link_up_buf_ptr   = ps_iface_alloc_event_cback_buf(
                                              rmsmi_707_pkt_event_handler_cb,
                                              NULL
                                            );

  ds707_rmsmi_info.flow_enable_buf_ptr    = ps_iface_alloc_event_cback_buf(
                                              rmsmi_707_pkt_event_handler_cb,
                                              NULL
                                            );

  ds707_rmsmi_info.flow_disable_buf_ptr   = ps_iface_alloc_event_cback_buf(
                                              rmsmi_707_pkt_event_handler_cb,
                                              NULL
                                            );

  if ( ( ds707_rmsmi_info.iface_down_buf_ptr == NULL )     ||
       ( ds707_rmsmi_info.phys_link_down_buf_ptr == NULL ) ||  
       ( ds707_rmsmi_info.phys_link_up_buf_ptr == NULL )   ||  
       ( ds707_rmsmi_info.flow_enable_buf_ptr == NULL )    ||  
       ( ds707_rmsmi_info.flow_disable_buf_ptr == NULL ) ) 
  {
    ERR_FATAL("Event buf allocation failed", 0, 0, 0);
    return FALSE;
  }

  ds707_rmsmi_info.inited = TRUE;
  return TRUE;
} /* ds707_rmsm_init() */


/*===========================================================================
  FUNCTION DS707_RMSM_IS_PACKET_CALL()

  DESCRIPTION
    Returns true if we're in a packet call

  PARAMETERS
    None

  RETURN VALUE
    None

  DEPENDENCIES
    None

  SIDE EFFECTS
    None
===========================================================================*/
boolean ds707_rmsm_is_packet_call
(
  void
)
{
/*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/
  /*-------------------------------------------------------------------------
    We are in a packet call if rmsm state is non-null 
  -------------------------------------------------------------------------*/
  return (ds707_rmsmi_info.state == DS707_RMSM_NULL_STATE) ? FALSE : TRUE;
} /* ds707_rmsm_is_packet_call() */


/*===========================================================================
  FUNCTION DS707_RMSM_IS_MIP_UP()

  DESCRIPTION
    Returns true if we're in a laptop MobileIP pkt data call

  PARAMETERS
    None

  RETURN VALUE
    TRUE - If laptop Mobile IP call is up
    FALSE - Otherwise

  DEPENDENCIES
    None

  SIDE EFFECTS
    None
===========================================================================*/
boolean ds707_rmsm_is_mip_up
(
  void
)
{
/*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/
  return (ds707_rmsmi_info.state == DS707_RMSM_UM_RM_UP_MIP_STATE) ?
    TRUE : FALSE;
} /* ds707_rmsm_is_mip_up() */


/*===========================================================================
  FUNCTION DS707_RMSM_POST_EVENT()

  DESCRIPTION
    This function receives an event posted to RMSM and enqueues the
    corresponding command to be processed in the DS task.

  PARAMETERS
    event: The event that is being posted to the state machine.

  RETURN VALUE
    None

  DEPENDENCIES
    RM State Machine should have already been initialized.

  SIDE EFFECTS
    None
===========================================================================*/
void ds707_rmsm_post_event
(
  ds707_rmsm_event_type event
)
{
  ds_cmd_type *cmd_ptr;                          /* DS Task cmd buf        */
/*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  if(ds707_rmsmi_info.inited != TRUE)
  {
    ASSERT(0);
    ERR("Cann't post ev %d, RMSM not inited", event, 0, 0);
    return;
  }

#ifdef T_ARM
  MSG_HIGH( "Event %d posted to RMSM in state %d",
            event, ds707_rmsmi_info.state, 0 );
#else
  MSG_HIGH( "'%s' event posted to RMSM in state %d.",
            ds707_rmsmi_event_names[event], ds707_rmsmi_info.state, 0 );
#endif

  if( ( cmd_ptr = ds_get_cmd_buf() ) == NULL )
  {
    ERR_FATAL( "RMSM out of DS task cmd bufs", 0, 0, 0 );
    return;
  }

  /*-------------------------------------------------------------------------
    Post cmd to DS task corresponding to each event
  -------------------------------------------------------------------------*/
  switch( event )
  {
    case DS707_RMSM_RM_WANTS_PKT_CALL_EV:
      cmd_ptr->hdr.cmd_id = DS_707_RMSM_RM_WANTS_PKT_CALL_CMD;
      break;

    case DS707_RMSM_RM_IFACE_DOWN_EV:
      cmd_ptr->hdr.cmd_id = DS_707_RMSM_RM_IFACE_DOWN_CMD;
      break;

    case DS707_RMSM_UM_IFACE_DOWN_EV:
      cmd_ptr->hdr.cmd_id = DS_707_RMSM_UM_IFACE_DOWN_CMD;
      break;

    case DS707_RMSM_UM_PHYS_LINK_UP_EV:
      cmd_ptr->hdr.cmd_id = DS_707_RMSM_UM_PHYS_LINK_UP_CMD;
      break;

    case DS707_RMSM_UM_PHYS_LINK_DOWN_EV:
      cmd_ptr->hdr.cmd_id = DS_707_RMSM_UM_PHYS_LINK_DOWN_CMD;
      break;

    case DS707_RMSM_UM_PPP_DOWN_EV:
      cmd_ptr->hdr.cmd_id = DS_707_RMSM_UM_PPP_DOWN_CMD;
      break;

#ifdef FEATURE_DS_MOBILE_IP
    case DS707_RMSM_RM_PPP_UP_EV:
      cmd_ptr->hdr.cmd_id = DS_707_RMSM_RM_PPP_UP_CMD;
      break;

    case DS707_RMSM_UM_MIP_UP_EV:
      cmd_ptr->hdr.cmd_id = DS_707_RMSM_UM_MIP_UP_CMD;
      break;

    case DS707_RMSM_UM_MIP_DOWN_EV:
      cmd_ptr->hdr.cmd_id = DS_707_RMSM_UM_MIP_DOWN_CMD;
      break;
#endif /* FEATURE_DS_MOBILE_IP */

    default:
      ASSERT(0);
      ERR( "Invalid RMSM event %d, ignoring", event, 0, 0 );
      return;
  } /* switch(event) */

  ds_put_cmd( cmd_ptr );

} /* ds707_rmsm_post_event() */


/*===========================================================================
  FUNCTION DS707_RMSM_PROCESS_CMD()

  DESCRIPTION
    This function receives a cmd posted to RMSM in DS task and calls
    an appropriate function to handle it.

  PARAMETERS
    event: The event that is being posted to the state machine.

  RETURN VALUE
    ds_cmd_ptr: Cmd buffer passed with the cmd

  DEPENDENCIES
    RM State Machine should have already been initialized

  SIDE EFFECTS
    None
===========================================================================*/
void ds707_rmsm_process_cmd
(
  ds_cmd_type *ds_cmd_ptr
)
{
  ds_cmd_enum_type cmd;
/* - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - */

  cmd  = ds_cmd_ptr->hdr.cmd_id;

  if(ds707_rmsmi_info.inited != TRUE)
  {
    ASSERT(0);
    ERR("Cann't post cmd %d, RMSM not inited", cmd, 0, 0);
    return;
  }

  MSG_MED( "Recvd cmd %d in state %d", cmd, ds707_rmsmi_info.state, 0 );

  switch( cmd )
  {
    case DS_707_RMSM_RM_WANTS_PKT_CALL_CMD:
      /*---------------------------------------------------------------------
        Laptop wants to initiate a pkt data call
      ---------------------------------------------------------------------*/
      ds707_rmsmi_process_rm_wants_pkt_call_ev();
      break;

    case DS_707_RMSM_RM_IFACE_DOWN_CMD:
      /*---------------------------------------------------------------------
        Laptop is disconnected or RM PPP terminated, RM iface is down.
        Terminate call on UM also.
      ---------------------------------------------------------------------*/
      ds707_rmsmi_process_rm_iface_down_ev();
      break;

    case DS_707_RMSM_UM_IFACE_DOWN_CMD:
      /*---------------------------------------------------------------------
        Call terminated on UM, cleanup RM and go NULL.
      ---------------------------------------------------------------------*/
      ds707_rmsmi_process_um_iface_down_ev();
      break;

    case DS_707_RMSM_UM_PHYS_LINK_UP_CMD:
      /*---------------------------------------------------------------------
        Traffic channel established on Um, used for Relay/network model call        
      ---------------------------------------------------------------------*/
      ds707_rmsmi_process_um_phys_link_up_ev();
      break;

    case DS_707_RMSM_UM_PHYS_LINK_DOWN_CMD:
      /*---------------------------------------------------------------------
        Traffic channel is down, terminate the call if this happens in the 
        middle of a call setup.
      ---------------------------------------------------------------------*/
      ds707_rmsmi_process_um_phys_link_down_ev();
      break;
    
    case DS_707_RMSM_UM_PPP_DOWN_CMD:
      /*---------------------------------------------------------------------
        UM PPP is down, this indicates the PPP resync failure on UM,
        cleanup RM and go NULL.
      ---------------------------------------------------------------------*/
      ds707_rmsmi_process_um_ppp_down_ev();
      break;

#ifdef FEATURE_DS_MOBILE_IP
    case DS_707_RMSM_RM_PPP_UP_CMD:
      /*---------------------------------------------------------------------
        PPP is up on RM during a MIP call setup. Compare Um and Rm options,
        resync Um if they mismatch.
      ---------------------------------------------------------------------*/
      ds707_rmsmi_process_rm_ppp_up_ev();
      break;

    case DS_707_RMSM_UM_MIP_UP_CMD:
      /*---------------------------------------------------------------------
        Um is up with MIP. Bring up Rm now.
      ---------------------------------------------------------------------*/
      ds707_rmsmi_process_um_mip_up_ev();
      break;

      /*---------------------------------------------------------------------
        MIP is down on Um, cleanup RM and go NULL. 
      ---------------------------------------------------------------------*/
    case DS_707_RMSM_UM_MIP_DOWN_CMD:
      ds707_rmsmi_process_um_mip_down_ev();
      break;
#endif /* FEATURE_DS_MOBILE_IP */

    default:
      ASSERT(0);            
      ERR("Invald RMSM cmd %d, ignoring", cmd, 0, 0 );
      return;
  } /* switch( cmd ) */
} /* ds707_rmsm_process_cmd() */


/*===========================================================================
  FUNCTION DS707_RMSM_DIAL_STR_CB()

  DESCRIPTION
    This function processes the dial string it has registered for and
    initiates the appropriate data call over IS707_PKT_IFACE.

  PARAMETERS
    dial_string: The dial string to be processed
    digit_mode: Flag to tell whether dial string contains digits only

  RETURN VALUE
    Abort handler callback.  If there is an error, then kills the call
    via DCD and returns NULL.

  DEPENDENCIES
    None

  SIDE EFFECTS
    None
===========================================================================*/
dsat_call_abort_cb_type ds707_rmsm_dial_str_cb
(
  const byte *dial_string,
  boolean    digit_mode,
  dsat_dial_modifier_info_type  * modifiers
)
{
/*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/



  /*-------------------------------------------------------------------------
    Register for desired events (dtr signal) with solib.  If the
    registration fails drop the call since siolib is already in use.
    Otherwise indicate to siolib that we are in call now.
  -------------------------------------------------------------------------*/
  if( ds3g_siolib_register_callback_func( &ds707_rmsmi_sio_func_table ) == 
      FALSE )
  {
    MSG_ERROR("Call setup failed, port unavailable", 0, 0, 0);
    ds3g_siolib_set_cd_state( DS3G_SIOLIB_REMOTE_CARRIER_OFF );
    dsat_send_result_code( DSAT_NO_CARRIER );
    return(NULL);
  }
  
  ds3g_siolib_set_call_coming_up( TRUE );

  MSG_HIGH( "Pkt dial string recvd, posting DS707_RMSM_RM_WANTS_PKT_CALL_EV",
              0, 0, 0 );
  ds707_rmsm_post_event( DS707_RMSM_RM_WANTS_PKT_CALL_EV );

  return(ds707_rmsmi_call_abort_cb);

} /* ds707_rmsm_dial_str_cb() */


/*= = = = = = = = = = = = = = = = = = = = = = = = = = = = = = = = = = = = = =
                          LOCAL FUNCTION DEFINTIONS
= = = = = = = = = = = = = = = = = = = = = = = = = = = = = = = = = = = = = =*/

/*===========================================================================
  FUNCTION DS707_RMSMI_PROCESS_RM_WANTS_PKT_CALL_EV()

  DESCRIPTION
    This function processes DS707_RMSM_RM_WANTS_PKT_CALL_EV event. Changes
    state to DS707_RMSM_WAITING_FOR_UM_UP_RELAY_OR_SIP_STATE or
    DS707_RMSM_WAITING_FOR_UM_UP_MIP_STATE.

  PARAMETERS
    None

  RETURN VALUE
    None

  DEPENDENCIES
    ds707_rmsm_init() has to have been called first.

  SIDE EFFECTS
    None
===========================================================================*/
LOCAL void ds707_rmsmi_process_rm_wants_pkt_call_ev
(
  void
)
{
#ifdef FEATURE_DS_MULTIPLE_PROFILES
  int8              profile_id = DATA_SESSION_PROFILE_ID_INVALID;
  nv_stat_enum_type nv_status;   /* status from NV call */
  nv_item_type      nv_item;
#endif
/*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

#ifdef T_ARM
  MSG_MED( "Recvd RM_WANTS_PKT_CALL_EV, state %d",
           ds707_rmsmi_info.state, 0, 0 );
#else
  MSG_MED( "Recvd RM_WANTS_PKT_CALL_EV, state '%s'\n",
           ds707_rmsmi_state_names[ds707_rmsmi_info.state], 0, 0 );
#endif

  switch( ds707_rmsmi_info.state )
  {
    case DS707_RMSM_NULL_STATE:
    {
#if defined(FEATURE_DS_MOBILE_IP) && defined(FEATURE_DS_MULTIPLE_NAI)
      if ( dsat707_qcmip_val > 0 )
      {
        /*---------------------------------------------------------------------
          For laptop MIP calls NAI is different from sockets calls. Register
          for mip_config_session callbacks to be called at the completion
          of MIP config session so that the NAIs may be updated.
        ---------------------------------------------------------------------*/
        mip_reg_ses_info_cb( ds707_rmsm_get_mip_session_info_cb );
      }
#endif /* FEATURE_DS_MOBILE_IP && FEATURE_DS_MULTIPLE_NAI */

      /*---------------------------------------------------------------------
        Bring up a data call if we have coverage (IFACE state != DISABLED)
        and if there is no call currently active over IS707 packet 
        (IFACE state == DOWN) or async interface (since all async calls are
        laptop calls, an earlier check on SIOLIB availability suffices 
        for now). Note that context lock is required because sockets calls 
        originate in PS task context.
        So we need to make sure that if we decide to originate,
        IFACE state should be set to something other than DOWN (and DISABLED)
        (to prevent async call) and disable 707 pkt iface ACLs (to prevent 
        sockets call) before freeingup the locks. This ensures only one type
        of call originates at a time on each interface.
      ---------------------------------------------------------------------*/

      if(ps_iface_state(ds707_rmsmi_info.IS707pkt_iface_ptr) != IFACE_DOWN)
      {
        MSG_HIGH("IS707 iface 0x%x in use or not available",
                 ds707_rmsmi_info.IS707pkt_iface_ptr, 0, 0);
        ds3g_siolib_set_cd_state( DS3G_SIOLIB_REMOTE_CARRIER_OFF );
        dsat_send_result_code( DSAT_NO_CARRIER );
        ds3g_siolib_set_call_coming_up( FALSE );
        ds3g_siolib_deregister_callback_func( &ds707_rmsmi_sio_func_table );
      }
      else
      {
        /*-------------------------------------------------------------------
          IS707 pkt iface is available to originate a data call.
        -------------------------------------------------------------------*/

        /*-------------------------------------------------------------------
          Save the tear_down_cmd_cb and bring_up_cmd_cb from 1X iface 
          registered by 1X Pkt Mgr so that it can be restored later when RMSM 
          goes to NULL. This is required since we will be registering a 
          different callback for Relay and SIP Network Model calls.
          THIS MUST HAPPEN BEFORE RMSM GOES TO NULL STATE
        -------------------------------------------------------------------*/
        ds707_rmsmi_info.orig_bring_up_f_ptr = 
          ds707_rmsmi_info.IS707pkt_iface_ptr->bring_up_cmd_f_ptr;
        ds707_rmsmi_info.orig_tear_down_f_ptr = 
          ds707_rmsmi_info.IS707pkt_iface_ptr->tear_down_cmd_f_ptr;
  
        if(ps_iface_event_cback_reg( ds707_rmsmi_info.IS707pkt_iface_ptr,
                                     IFACE_FLOW_ENABLED_EV,
                                     ds707_rmsmi_info.flow_enable_buf_ptr
                                   ) < 0 )
        {
          MSG_ERROR("FLOW_ENABLED_EV reg failed", 0, 0, 0 );
          ds707_rmsmi_transition_state(DS707_RMSM_NULL_STATE);
        }
  
        if(ps_iface_event_cback_reg( ds707_rmsmi_info.IS707pkt_iface_ptr,
                                     IFACE_FLOW_DISABLED_EV,
                                     ds707_rmsmi_info.flow_disable_buf_ptr
                                   ) < 0 )
        {
          MSG_ERROR("FLOW_DISABLED_EV reg failed", 0, 0, 0 );
          ds707_rmsmi_transition_state(DS707_RMSM_NULL_STATE);
        }

        ps_iface_enable_ind( &ds707_rmsmi_info.sio_707pkt_iface );

        /*---------------------------------------------------------------------
          SIO_IOCTL_INBOUND_FLOW_ENABLE enqueues command and does not enter 
          into critical section and so it is safe to use TASKLOCK() here.
        ---------------------------------------------------------------------*/
        TASKLOCK();

        netmdl_set_meta_state( PPP_OOC_META_STATE );

#ifdef FEATURE_DS_MOBILE_IP
        if ( dsat707_qcmip_val > 0 )
        {
          /*-------------------------------------------------------------------
            Originate a MIP network model IS707 pkt call
          -------------------------------------------------------------------*/
          netmdl_set_resync_behaviour(NETMDL_RESYNC_DROP_CALL);
          ds707_rmsmi_transition_state(DS707_RMSM_WAITING_FOR_UM_UP_MIP_STATE);
        }
        else
#endif /* FEATURE_DS_MOBILE_IP */
        {
          /*-------------------------------------------------------------------
            Originate a Relay or SIP network model IS707 pkt call
          -------------------------------------------------------------------*/
          ds707_rmsmi_transition_state(
            DS707_RMSM_WAITING_FOR_UM_UP_RELAY_OR_SIP_STATE);
        }

        TASKFREE();

#ifdef FEATURE_DS_MULTIPLE_PROFILES
        if( (num_valid_profiles > 0)
  #ifdef FEATURE_DS_MOBILE_IP
            && (dsat707_qcmip_val == 0)
  #endif
          )
        {
          profile_id=ds_get_app_profile_index(TETHERED_MODE_APP_TYPE);
          if( profile_id != DATA_SESSION_PROFILE_ID_INVALID )
          {
            MSG_HIGH("Requesting app is tethered mode, profile id = %d",
                     profile_id, 0, 0);
            /*--------------------------------------------------------------------------
              Laptop is not considered for arbitration, hence current profile ID is 
              set to laptop profile ID here. This is to pick the credentials
              appropriately for authentication. 
            -------------------------------------------------------------------------*/
            ds707_data_session_set_requesting_profile(
              ds707_rmsmi_info.IS707pkt_iface_ptr,profile_id );
            ds707_data_session_set_current_profile( 
              ds707_rmsmi_info.IS707pkt_iface_ptr );

	    nv_item.ds_sip_active_profile_index = profile_id;
            nv_status = dsi_put_nv_item( NV_DS_SIP_ACTIVE_PROFILE_INDEX_I, &nv_item );
            if( nv_status == NV_DONE_S )
            {
              MSG_HIGH( "Wrote active profile ID %d to NV", profile_id, 0, 0 );
            }
	    else
            {
              MSG_ERROR( "Active Profile Index NV write failed", 0, 0, 0 );
            }   
          }
        }
#endif
      }
      break;
    }

#ifdef FEATURE_DS_MOBILE_IP
    case DS707_RMSM_WAITING_FOR_UM_UP_MIP_STATE:
    case DS707_RMSM_WAITING_FOR_RM_UP_STATE:
    case DS707_RMSM_UM_RM_UP_MIP_STATE:
    case DS707_RMSM_RESYNCING_UM_STATE:
    case DS707_RMSM_RM_UP_WAITING_FOR_UM_DOWN_STATE:
#endif /* FEATURE_DS_MOBILE_IP */

    case DS707_RMSM_WAITING_FOR_RM_DOWN_STATE:
    case DS707_RMSM_WAITING_FOR_UM_UP_RELAY_OR_SIP_STATE:
    case DS707_RMSM_UM_RM_UP_RELAY_OR_SIP_STATE:
    case DS707_RMSM_WAITING_FOR_UM_DOWN_STATE:
    {
      /*---------------------------------------------------------------------
        Its error to get this event in all other states.
      ---------------------------------------------------------------------*/
      ERR("Unexpected RM_WANTS_RELAY_OR_SIP_EV in %d state",
          ds707_rmsmi_info.state, 0, 0);
      break;
    }

    default:
    {
      /*---------------------------------------------------------------------
        Invalid state
      ---------------------------------------------------------------------*/
      ERR("Invalid RMSM state %d", ds707_rmsmi_info.state, 0, 0);
      ASSERT( 0 );
    }
  } /* switch( ds707_rmsmi_info.state ) */
} /* ds707_rmsmi_process_rm_wants_pkt_call_ev() */


/*===========================================================================
  FUNCTION DS707_RMSMI_PROCESS_RM_IFACE_DOWN_EV()

  DESCRIPTION
    This function processes DS707_RMSM_RM_IFACE_DOWN_EV event. Changes state to
    DS707_RMSM_WAITING_FOR_UM_DOWN_STATE.

  PARAMETERS
    None

  RETURN VALUE
    None

  DEPENDENCIES
    ds707_rmsm_init() has to have been called first.

  SIDE EFFECTS
    None
===========================================================================*/
LOCAL void ds707_rmsmi_process_rm_iface_down_ev
(
  void
)
{
  int16               ps_iface_errno;
#ifdef FEATURE_CALL_THROTTLE
  ppp_link_phase_type um_ppp_phase = pppDEAD;
  ppp_link_phase_type rm_ppp_phase = pppDEAD;
#endif /* FEATURE_CALL_THROTTLE */
/*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

#ifdef T_ARM
  MSG_MED( "Recvd RM_IFACE_DOWN_EV, state %d", ds707_rmsmi_info.state, 0, 0 );
#else
  MSG_MED( "Recvd RM_IFACE_DOWN_EV, state '%s'\n",
           ds707_rmsmi_state_names[ds707_rmsmi_info.state], 0, 0 );
#endif

  switch( ds707_rmsmi_info.state )
  {
    case DS707_RMSM_WAITING_FOR_UM_UP_RELAY_OR_SIP_STATE:
    case DS707_RMSM_UM_RM_UP_RELAY_OR_SIP_STATE:
    {
      /* Post SIP fail only for Network Model Call */
#ifdef FEATURE_CALL_THROTTLE /*dctm 2.0*/
      if( dsat707_crm_val == DS_CRM_VAL_NETMODEL_MODE )
      {
        if(ds707_rmsmi_info.state == DS707_RMSM_UM_RM_UP_RELAY_OR_SIP_STATE)
        {
          um_ppp_phase = ppp_get_phase(PPP_UM_SN_DEV);
          rm_ppp_phase = ppp_get_phase(PPP_RM_DEV);
          MSG_MED( "PPP phase for UM %d RM %d", um_ppp_phase, rm_ppp_phase, 0 );

          /*-------------------------------------------------------------------
            If PPP Auth fails for tethered network model call, DTR drop is 
            received. To differentiate DTR drop meant for PPP auth failure 
            and other reasons, we check PPP phase and send failure reason to 
            CM for DCTM.
          -------------------------------------------------------------------*/
          if( (um_ppp_phase != pppNETWORK && um_ppp_phase != pppTERMINATE) || 
              (rm_ppp_phase != pppNETWORK && rm_ppp_phase != pppTERMINATE) )
          {
            MSG_MED("post dctm ev", 0, 0, 0);
            dctm_post_sip_fail();
          }
        }
        else
        {
          MSG_MED("post dctm ev", 0, 0, 0);
          dctm_post_sip_fail();
        }
      }
#endif /* FEATURE_CALL_THROTTLE */

      /*---------------------------------------------------------------------
        Since laptop has terminated the call, we need to cleanup Um before
        completely terminating the call. Tear down the 707 pkt iface first.
        Checking for the return code as the iface could already be down.
      ---------------------------------------------------------------------*/
      if ( ps_iface_tear_down_cmd(ds707_rmsmi_info.IS707pkt_iface_ptr,
                                  &ps_iface_errno,
                                  NULL ) == 0 )
      {
        /*-------------------------------------------------------------------
          If the IS707 iface is already down we should cleanup the call
          on Rm since we won't get any other notification from IS707
          controller
        -------------------------------------------------------------------*/
        ds707_rmsmi_transition_state( DS707_RMSM_NULL_STATE );
      }
      else
      {
        /*-------------------------------------------------------------------
          Otherwsie wait for IS707 iface to cleanup
        -------------------------------------------------------------------*/
        ds707_rmsmi_transition_state( DS707_RMSM_WAITING_FOR_UM_DOWN_STATE );
      }

      break;
    }

#ifdef FEATURE_DS_MOBILE_IP
    case DS707_RMSM_WAITING_FOR_UM_UP_MIP_STATE:
    case DS707_RMSM_WAITING_FOR_RM_UP_STATE:
#ifdef FEATURE_CALL_THROTTLE
    {
      /*----------------------------------------------------------------------
        DTR drop during PPP negotiation / MIP registration - throttle
      ----------------------------------------------------------------------*/
      MSG_MED("RM iface down in %d state, post dctm mip ev", 
               ds707_rmsmi_info.state, 0, 0);
      dctm_post_mip_call_status( MIP_FAILURE_EV, MIP_FAIL_REASON_NONE );
      /* fall through */
    }
#endif /* FEATURE_CALL_THROTTLE */

    case DS707_RMSM_UM_RM_UP_MIP_STATE:
    case DS707_RMSM_RESYNCING_UM_STATE:
    {
      MIP_SEND_CMD(MIPIO_PPP_CLOSE_CMD, NULL);
    }
    /* fall through */

    case DS707_RMSM_RM_UP_WAITING_FOR_UM_DOWN_STATE:
    {
      ds707_rmsmi_transition_state( DS707_RMSM_WAITING_FOR_UM_DOWN_STATE );
      break;
    }
#endif /* FEATURE_DS_MOBILE_IP */

    case DS707_RMSM_WAITING_FOR_RM_DOWN_STATE:
    {
      ds707_rmsmi_transition_state( DS707_RMSM_NULL_STATE );
      break;
    }

    case DS707_RMSM_NULL_STATE:
    case DS707_RMSM_WAITING_FOR_UM_DOWN_STATE:
    {
      /*---------------------------------------------------------------------
        Ignore event in all other states.
      ---------------------------------------------------------------------*/
      MSG_HIGH( "Ignoring RM_IFACE_DOWN_EV in %d state",
                ds707_rmsmi_info.state, 0, 0 );
      break;
    }

    default:
    {
      /*---------------------------------------------------------------------
        Invalid state
      ---------------------------------------------------------------------*/
      ERR("Invalid RMSM state %d", ds707_rmsmi_info.state, 0, 0);
      ASSERT( 0 );
    }
  } /* switch( ds707_rmsmi_info.state ) */
} /* ds707_rmsmi_process_rm_iface_down_ev() */


/*===========================================================================
  FUNCTION DS707_RMSMI_PROCESS_UM_IFACE_DOWN_EV()

  DESCRIPTION
    This function processes DS707_RMSM_UM_IFACE_DOWN_EV event. Changes state to
    DS707_RMSM_NULL_STATE.

  PARAMETERS
    None

  RETURN VALUE
    None

  DEPENDENCIES
    ds707_rmsm_init() has to have been called first.

  SIDE EFFECTS
    None
===========================================================================*/
LOCAL void ds707_rmsmi_process_um_iface_down_ev
(
  void
)
{
/*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/
  ppp_link_phase_type rm_ppp_phase = pppDEAD;

#ifdef T_ARM
  MSG_MED( "Recvd UM_IFACE_DOWN_EV, state %d", ds707_rmsmi_info.state, 0, 0 );
#else
  MSG_MED( "Recvd UM_IFACE_DOWN_EV, state '%s'\n",
           ds707_rmsmi_state_names[ds707_rmsmi_info.state], 0, 0 );
#endif

  switch( ds707_rmsmi_info.state )
  {
    case DS707_RMSM_WAITING_FOR_UM_UP_RELAY_OR_SIP_STATE:
    case DS707_RMSM_UM_RM_UP_RELAY_OR_SIP_STATE:
    {
      /*-------------------------------------------------------------------
        The 707 pkt iface is down now, we should also terminate the call
        on Rm. For Relay and Nemodel SIP calls there isn't any PPP running
        on SIO iface.  Only final cleanup needs to be performed.
      -------------------------------------------------------------------*/
      ds707_rmsmi_transition_state( DS707_RMSM_NULL_STATE );
      break;
    }

    case DS707_RMSM_WAITING_FOR_UM_DOWN_STATE:
    {
      rm_ppp_phase = ppp_get_phase( PPP_RM_DEV );

      MSG_MED( "PPP(RM) phase is %d", rm_ppp_phase, 0, 0 );

      if( rm_ppp_phase == pppDEAD )
      {
        /* PPP(RM) interface is already down */
        ds707_rmsmi_transition_state( DS707_RMSM_NULL_STATE );
      }
      else
      {
        /* PPP(RM) interface is still up */
        ds707_rmsmi_transition_state( DS707_RMSM_WAITING_FOR_RM_DOWN_STATE );
      }

      break;
    }

    case DS707_RMSM_NULL_STATE:
    case DS707_RMSM_WAITING_FOR_RM_DOWN_STATE:
#ifdef FEATURE_DS_MOBILE_IP
    case DS707_RMSM_WAITING_FOR_UM_UP_MIP_STATE:
    case DS707_RMSM_WAITING_FOR_RM_UP_STATE:
    case DS707_RMSM_UM_RM_UP_MIP_STATE:
    case DS707_RMSM_RESYNCING_UM_STATE:
    case DS707_RMSM_RM_UP_WAITING_FOR_UM_DOWN_STATE:
#endif /* FEATURE_DS_MOBILE_IP */

    {
      /*---------------------------------------------------------------------
        Ignore event in all other states.
      ---------------------------------------------------------------------*/
      MSG_HIGH( "Ignoring UM_IFACE_DOWN_EV in %d state",
                ds707_rmsmi_info.state, 0, 0 );
      break;
    }

    default:
    {
      /*---------------------------------------------------------------------
        Invalid state
      ---------------------------------------------------------------------*/
      ERR("Invalid RMSM state %d", ds707_rmsmi_info.state, 0, 0);
      ASSERT( 0 );
    }
  } /* switch( ds707_rmsmi_info.state ) */
} /* ds707_rmsmi_process_um_iface_down_ev() */


/*===========================================================================
  FUNCTION DS707_RMSMI_PROCESS_UM_PHYS_LINK_UP_EV()

  DESCRIPTION
    This function processes DS707_RMSM_UM_PHYS_LINK_UP_EV event. Changes state to
    DS707_RMSM_UM_RM_UP_RELAY_OR_SIP_STATE.

  PARAMETERS
    None

  RETURN VALUE
    None

  DEPENDENCIES
    ds707_rmsm_init() has to have been called first.

  SIDE EFFECTS
    None
===========================================================================*/
LOCAL void ds707_rmsmi_process_um_phys_link_up_ev
(
  void
)
{
/*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

#ifdef T_ARM
  MSG_MED( "Recvd UM_PHYS_LINK_UP_EV, state %d", ds707_rmsmi_info.state, 0, 0 );
#else
  MSG_MED( "Recvd UM_PHYS_LINK_UP_EV, state '%s'\n",
           ds707_rmsmi_state_names[ds707_rmsmi_info.state], 0, 0 );
#endif

  switch( ds707_rmsmi_info.state )
  {
    case DS707_RMSM_WAITING_FOR_UM_UP_RELAY_OR_SIP_STATE:
    {
      /*---------------------------------------------------------------------
        Since Relay/network model calls are up at this point, phys link
        going up or down only means dormancy from this point onwards. RMSM
        does not care about dormancy, hence dereg those events.
      ---------------------------------------------------------------------*/
      ps_iface_event_cback_dereg( ds707_rmsmi_info.IS707pkt_iface_ptr,
                                  IFACE_PHYS_LINK_DOWN_EV,
                                  ds707_rmsmi_info.phys_link_down_buf_ptr );

      ps_iface_event_cback_dereg( ds707_rmsmi_info.IS707pkt_iface_ptr,
                                  IFACE_PHYS_LINK_UP_EV,
                                  ds707_rmsmi_info.phys_link_up_buf_ptr );

      ds707_rmsmi_transition_state( DS707_RMSM_UM_RM_UP_RELAY_OR_SIP_STATE );
      break;
    }

    case DS707_RMSM_NULL_STATE:
    case DS707_RMSM_UM_RM_UP_RELAY_OR_SIP_STATE:
    case DS707_RMSM_WAITING_FOR_UM_DOWN_STATE:
    case DS707_RMSM_WAITING_FOR_RM_DOWN_STATE:

#ifdef FEATURE_DS_MOBILE_IP
    case DS707_RMSM_WAITING_FOR_UM_UP_MIP_STATE:
    case DS707_RMSM_WAITING_FOR_RM_UP_STATE:
    case DS707_RMSM_UM_RM_UP_MIP_STATE:
    case DS707_RMSM_RESYNCING_UM_STATE:
    case DS707_RMSM_RM_UP_WAITING_FOR_UM_DOWN_STATE:
#endif /* FEATURE_DS_MOBILE_IP */

    {
      /*---------------------------------------------------------------------
        Ignore event in all other states.
      ---------------------------------------------------------------------*/
      MSG_HIGH( "Ignoring UM_IFACE_UP_EV in %d state",
                ds707_rmsmi_info.state, 0, 0 );
      break;
    }

    default:
    {
      /*---------------------------------------------------------------------
        Invalid state
      ---------------------------------------------------------------------*/
      ERR("Invalid RMSM state %d", ds707_rmsmi_info.state, 0, 0);
      ASSERT( 0 );
    }
  } /* switch( ds707_rmsmi_info.state ) */
} /* ds707_rmsmi_process_um_phys_link_up_ev() */


/*===========================================================================
  FUNCTION DS707_RMSMI_PROCESS_UM_PHYS_LINK_DOWN_EV()

  DESCRIPTION
    This function processes DS707_RMSM_UM_PHYS_LINK_DOWN_EV event. Changes state
    to DS707_RMSM_NULL_STATE.

  PARAMETERS
    None

  RETURN VALUE
    None

  DEPENDENCIES
    ds707_rmsm_init() has to have been called first.

  SIDE EFFECTS
    None
===========================================================================*/
LOCAL void ds707_rmsmi_process_um_phys_link_down_ev
(
  void
)
{
/*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

#ifdef T_ARM
  MSG_MED( "Recvd UM_PHYS_LINK_DOWN_EV, state %d", ds707_rmsmi_info.state, 0, 0 );
#else
  MSG_MED( "Recvd UM_PHYS_LINK_DOWN_EV, state '%s'\n",
           ds707_rmsmi_state_names[ds707_rmsmi_info.state], 0, 0 );
#endif

  switch( ds707_rmsmi_info.state )
  {
    case DS707_RMSM_WAITING_FOR_UM_UP_RELAY_OR_SIP_STATE:
    {
      /*---------------------------------------------------------------------
        Call origination failed, move the 707 iface to down state. This
        will allow 707 controller to perform any remaining cleanup before
        the call on Rm could be cleaned up (in the down event callback)
      ---------------------------------------------------------------------*/
      ps_iface_down_ind( ds707_rmsmi_info.IS707pkt_iface_ptr );
      break;
    }

    case DS707_RMSM_NULL_STATE:
    case DS707_RMSM_UM_RM_UP_RELAY_OR_SIP_STATE:
    case DS707_RMSM_WAITING_FOR_UM_DOWN_STATE:
    case DS707_RMSM_WAITING_FOR_RM_DOWN_STATE:

#ifdef FEATURE_DS_MOBILE_IP
    case DS707_RMSM_WAITING_FOR_UM_UP_MIP_STATE:
    case DS707_RMSM_WAITING_FOR_RM_UP_STATE:
    case DS707_RMSM_UM_RM_UP_MIP_STATE:
    case DS707_RMSM_RESYNCING_UM_STATE:
    case DS707_RMSM_RM_UP_WAITING_FOR_UM_DOWN_STATE:
#endif /* FEATURE_DS_MOBILE_IP */

    {
      /*---------------------------------------------------------------------
        Ignore event in all other states.
      ---------------------------------------------------------------------*/
      MSG_HIGH( "Ignoring UM_PHYS_LINK_DOWN_EV in %d state",
                ds707_rmsmi_info.state, 0, 0 );
      break;
    }

    default:
    {
      /*---------------------------------------------------------------------
        Invalid state
      ---------------------------------------------------------------------*/
      ERR("Invalid RMSM state %d", ds707_rmsmi_info.state, 0, 0);
      ASSERT( 0 );
    }
  } /* switch( ds707_rmsmi_info.state ) */
} /* ds707_rmsmi_process_um_phys_link_down_ev() */


/*===========================================================================
  FUNCTION DS707_RMSMI_PROCESS_UM_PPP_DOWN_EV()

  DESCRIPTION
    This function processes DS707_RMSM_UM_PPP_DOWN_EV event. Changes state to
    DS707_RMSM_WAITING_FOR_UM_DOWN_STATE.

  PARAMETERS
    None

  RETURN VALUE
    None

  DEPENDENCIES
    ds707_rmsm_init() has to have been called first.

  SIDE EFFECTS
    None
===========================================================================*/
LOCAL void ds707_rmsmi_process_um_ppp_down_ev
(
  void
)
{
  int16  ps_iface_errno;
/*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

#ifdef T_ARM
  MSG_MED( "Recvd UM_PPP_DOWN_EV, state %d", ds707_rmsmi_info.state, 0, 0 );
#else
  MSG_MED( "Recvd UM_PPP_DOWN_EV, state '%s'\n",
           ds707_rmsmi_state_names[ds707_rmsmi_info.state], 0, 0 );
#endif

  switch( ds707_rmsmi_info.state )
  {
    case DS707_RMSM_UM_RM_UP_RELAY_OR_SIP_STATE:
    {
      /*---------------------------------------------------------------------
        Since laptop has terminated the call, we need to cleanup Um before
        completely terminating the call. Tear down the 707 pkt iface first.
        Checking for the return code as the iface could already be down.
      ---------------------------------------------------------------------*/
      if ( ps_iface_tear_down_cmd(ds707_rmsmi_info.IS707pkt_iface_ptr,
                                  &ps_iface_errno,
                                  NULL) == 0 )
      {
        /*-------------------------------------------------------------------
          If the IS707 iface is already down we should cleanup the call
          on Rm since we won't get any other notification from IS707
          controller
        -------------------------------------------------------------------*/
        ds707_rmsmi_transition_state( DS707_RMSM_NULL_STATE );
      }
      else
      {
        /*-------------------------------------------------------------------
          Otherwsie wait for IS707 iface to cleanup
        -------------------------------------------------------------------*/
        ds707_rmsmi_transition_state( DS707_RMSM_WAITING_FOR_UM_DOWN_STATE );
      }

      break;
    }

#ifdef FEATURE_DS_MOBILE_IP
    case DS707_RMSM_UM_RM_UP_MIP_STATE:
    {
      ds707_rmsmi_transition_state( DS707_RMSM_RM_UP_WAITING_FOR_UM_DOWN_STATE );
      break;
    }

    case DS707_RMSM_WAITING_FOR_UM_UP_MIP_STATE:
    case DS707_RMSM_WAITING_FOR_RM_UP_STATE:
    case DS707_RMSM_RESYNCING_UM_STATE:
    case DS707_RMSM_RM_UP_WAITING_FOR_UM_DOWN_STATE:
#endif /* FEATURE_DS_MOBILE_IP */
    case DS707_RMSM_WAITING_FOR_UM_UP_RELAY_OR_SIP_STATE:
    case DS707_RMSM_NULL_STATE:
    case DS707_RMSM_WAITING_FOR_UM_DOWN_STATE:
    case DS707_RMSM_WAITING_FOR_RM_DOWN_STATE:
    {
      /*---------------------------------------------------------------------
        Ignore event in all other states.
      ---------------------------------------------------------------------*/
      MSG_HIGH( "Ignoring RM_IFACE_DOWN_EV in %d state",
                ds707_rmsmi_info.state, 0, 0 );
      break;
    }

    default:
    {
      /*---------------------------------------------------------------------
        Invalid state
      ---------------------------------------------------------------------*/
      ERR("Invalid RMSM state %d", ds707_rmsmi_info.state, 0, 0);
      ASSERT( 0 );
    }
  } /* switch( ds707_rmsmi_info.state ) */
} /* ds707_rmsmi_process_um_ppp_down_ev() */


#ifdef FEATURE_DS_MOBILE_IP
/*===========================================================================
  FUNCTION DS707_RMSMI_PROCESS_RM_PPP_UP_EV()

  DESCRIPTION
    This function processes DS707_RMSM_RM_PPP_UP_EV event. Changes state to
    DS707_RMSM_UM_RM_UP_MIP_STATE or DS707_RMSM_RESYNCING_UM_STATE.

  PARAMETERS
    None

  RETURN VALUE
    None

  DEPENDENCIES
    ds707_rmsm_init() has to have been called first.

  SIDE EFFECTS
    None
===========================================================================*/
LOCAL void ds707_rmsmi_process_rm_ppp_up_ev
(
  void
)
{
  ppp_dev_opts_type dev_opts;
  ip_addr_type       ip_addr;
/*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

#ifdef T_ARM
  MSG_MED( "Recvd RM_PPP_UP_EV, state %d", ds707_rmsmi_info.state, 0, 0 );
#else
  MSG_MED( "Recvd RM_PPP_UP_EV, state '%s'\n",
           ds707_rmsmi_state_names[ds707_rmsmi_info.state], 0, 0 );
#endif

  switch( ds707_rmsmi_info.state )
  {
    case DS707_RMSM_NULL_STATE:
    case DS707_RMSM_WAITING_FOR_UM_UP_RELAY_OR_SIP_STATE:
    case DS707_RMSM_UM_RM_UP_RELAY_OR_SIP_STATE:
    case DS707_RMSM_WAITING_FOR_UM_DOWN_STATE:
    case DS707_RMSM_WAITING_FOR_UM_UP_MIP_STATE:
    case DS707_RMSM_UM_RM_UP_MIP_STATE:
    case DS707_RMSM_RESYNCING_UM_STATE:
    case DS707_RMSM_RM_UP_WAITING_FOR_UM_DOWN_STATE:
    case DS707_RMSM_WAITING_FOR_RM_DOWN_STATE:
    {
      /*---------------------------------------------------------------------
        Ignore event in all other states.
      ---------------------------------------------------------------------*/
      MSG_HIGH( "Ignoring RM_IFACE_UP_EV in %d state",
                ds707_rmsmi_info.state, 0, 0 );
      break;
    }

    case DS707_RMSM_WAITING_FOR_RM_UP_STATE:
    {
      /*---------------------------------------------------------------------
        PPP sets the PDSN IP address into SIO iface since that address is 
        advertized as the peer address to TE2. Reset SIO iface address to 0 
        so that any stack write to this address may not get loopbacked.
      ---------------------------------------------------------------------*/
      ip_addr.type    = IPV4_ADDR;
      ip_addr.addr.v4 = 0;
      ps_iface_set_addr(&ds707_rmsmi_info.sio_707pkt_iface, &ip_addr);

      /*---------------------------------------------------------------------
        Set up options to be set to Um either for immediate or future resync.
        Copy the working options from Rm to Um will so any handoffs/resyncs
        etc. will use the current options.
      ---------------------------------------------------------------------*/
      ppp_get_bridge_dev_prot_opts (PPP_RM_DEV, &dev_opts);

      dev_opts.ipcp_info.will_mask[FSM_REMOTE] |= IPCP_N_1ADDRESS;
      dev_opts.ipcp_info.want_mask[FSM_REMOTE] |= IPCP_N_1ADDRESS;
      dev_opts.ipcp_info.will_mask[FSM_LOCAL] |= IPCP_N_PRIMARY_DNS |
                                                 IPCP_N_SECONDARY_DNS;
      dev_opts.ipcp_info.want_mask[FSM_LOCAL] |= IPCP_N_PRIMARY_DNS |
                                                 IPCP_N_SECONDARY_DNS;
      /*---------------------------------------------------------------------
        Strip the IP Addr option from local - we get IP addr from MIP reg.
      ---------------------------------------------------------------------*/
      dev_opts.ipcp_info.will_mask[FSM_LOCAL] &= ~IPCP_N_1ADDRESS;
      dev_opts.ipcp_info.want_mask[FSM_LOCAL] &= ~IPCP_N_1ADDRESS;

      dev_opts.ipcp_info.want_vals[FSM_LOCAL].primary_dns = 0;
      dev_opts.ipcp_info.work_vals[FSM_LOCAL].primary_dns = 0;
      dev_opts.ipcp_info.want_vals[FSM_LOCAL].secondary_dns = 0;
      dev_opts.ipcp_info.work_vals[FSM_LOCAL].secondary_dns = 0;
      /*---------------------------------------------------------------------
        get_peer_data_cback() should look in the iface cb to get the peer
        IP address, the following line will break it otherwise
      ---------------------------------------------------------------------*/
      dev_opts.ipcp_info.want_vals[FSM_REMOTE].address = 0;
      dev_opts.ipcp_info.work_vals[FSM_REMOTE].address = 0;

      /*---------------------------------------------------------------------
        Check if the PPP options negotiated on Rm match those negotiated
        previously on the Um.
      ---------------------------------------------------------------------*/
      if( netmdl_compare_lcp_opts() == TRUE &&
          netmdl_compare_ipcp_opts() == TRUE )
      {
        /*-------------------------------------------------------------------
          Rm came up with options compatible with Um.
          Set the Um IPCP options to match the Rm so a subsequent resync 
          will be compatible with Rm.
        -------------------------------------------------------------------*/
        MSG_MED("RM/UM negotiated opts match", 0, 0, 0);
        ppp_update_ipcp_config(PPP_UM_SN_DEV, &dev_opts);

        /*-------------------------------------------------------------------
          Transition to MIP up state
        -------------------------------------------------------------------*/
        ds707_rmsmi_transition_state( DS707_RMSM_UM_RM_UP_MIP_STATE );
      }
      else
      {
        /*-------------------------------------------------------------------
          Rm came up with options incompatible with Um.
          Modify the Um PPP options to use the negotiated Rm options and 
          force a Um PPP resync.
        -------------------------------------------------------------------*/
        MSG_MED("RM/UM negotiated opts differ, resyncing Um", 0, 0, 0);
        ppp_get_dev_setup (PPP_UM_SN_DEV, &dev_opts);
        dev_opts.lcp_info.mode = PPP_CP_ACTIVE;
        dev_opts.ipcp_info.mode = PPP_CP_ACTIVE;
        
#ifdef FEATURE_CALL_THROTTLE
        /*-------------------------------------------------------------------
          MIP call fail due to option mismatch - Post Fail event to DCTM.
        -------------------------------------------------------------------*/
        MSG_MED("PPP options mismatch. Post FAILURE ev to CM for DCTM.",0,0,0);
        dctm_post_mip_call_status(MIP_FAILURE_EV, MIP_FAIL_REASON_PDSN_FAILURE);
#endif /* FEATURE_CALL_THROTTLE */
        
        /*-------------------------------------------------------------------
          Start PPP on Um and transition to resyncing state
        -------------------------------------------------------------------*/
        if (ppp_resync(PPP_UM_SN_DEV, PPP_PROTOCOL_LCP, &dev_opts) == -1)
        {
          /*-----------------------------------------------------------------
            If Um resync fails for some reason, kill the call
          -----------------------------------------------------------------*/
          MSG_MED ("Couldn't resync Um, kill call",0,0,0);
          ds707_rmsmi_transition_state( 
            DS707_RMSM_RM_UP_WAITING_FOR_UM_DOWN_STATE );
        }
        else
        {
          ds707_rmsmi_transition_state( DS707_RMSM_RESYNCING_UM_STATE );
        }
      }
      break;
    }

    default:
    {
      /*---------------------------------------------------------------------
        Invalid state
      ---------------------------------------------------------------------*/
      ERR("Invalid RMSM state %d", ds707_rmsmi_info.state, 0, 0);
      ASSERT( 0 );
    }
  } /* switch( ds707_rmsmi_info.state ) */
} /* ds707_rmsmi_process_rm_ppp_up_ev() */

/*===========================================================================
  FUNCTION DS707_RMSMI_PROCESS_UM_MIP_UP_EV()

  DESCRIPTION
    This function processes DS707_RMSM_UM_MIP_UP_EV event. Changes state to
    DS707_RMSM_WAITING_FOR_RM_UP_STATE.

  PARAMETERS
    None

  RETURN VALUE
    None

  DEPENDENCIES
    ds707_rmsm_init() has to have been called first.

  SIDE EFFECTS
    None
===========================================================================*/
LOCAL void ds707_rmsmi_process_um_mip_up_ev
(
  void
)
{
/*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

#ifdef T_ARM
  MSG_MED( "Recvd UM_MIP_UP_EV, state %d", ds707_rmsmi_info.state, 0, 0 );
#else
  MSG_MED( "Recvd UM_MIP_UP_EV, state '%s'\n",
           ds707_rmsmi_state_names[ds707_rmsmi_info.state], 0, 0 );
#endif

  switch( ds707_rmsmi_info.state )
  {
    case DS707_RMSM_WAITING_FOR_UM_UP_MIP_STATE:
    {
      /*---------------------------------------------------------------------
        Um PPP up and mobile IP registration complete, let apps know
      ---------------------------------------------------------------------*/
      ps_iface_routeable_ind( ds707_rmsmi_info.IS707pkt_iface_ptr );

      /*---------------------------------------------------------------------
        Now bring up Rm PPP
      ---------------------------------------------------------------------*/
      ds707_rmsmi_transition_state( DS707_RMSM_WAITING_FOR_RM_UP_STATE );
      break;
    }

    case DS707_RMSM_RESYNCING_UM_STATE:
    {
      ps_iface_routeable_ind( ds707_rmsmi_info.IS707pkt_iface_ptr );
      /*---------------------------------------------------------------------
        Check if options match after Um resync
      ---------------------------------------------------------------------*/
      if( netmdl_compare_lcp_opts() == TRUE &&
          netmdl_compare_ipcp_opts() == TRUE )
      {
        /*-------------------------------------------------------------------
          If so, all is well.  Go to MIP up state.
        -------------------------------------------------------------------*/
        ds707_rmsmi_transition_state( DS707_RMSM_UM_RM_UP_MIP_STATE );
      }
      else
      {
        /*-------------------------------------------------------------------
          If not, we give up.  Drop the call.
        -------------------------------------------------------------------*/
        MSG_MED ("Um & Rm PPP option mismatch, kill call",0,0,0);
        ds707_rmsmi_transition_state( DS707_RMSM_RM_UP_WAITING_FOR_UM_DOWN_STATE );
      }
      break;
    }

    case DS707_RMSM_UM_RM_UP_MIP_STATE:
    {
      /*---------------------------------------------------------------------
        Put the Um back in routeable (dssnet had set it to UP)
        This will hapen everytime MIP rereg happens due to a PPP resync.
        Note the PPP resync after the call is setup is handled through 
        netmodel. 
      ---------------------------------------------------------------------*/
      ps_iface_routeable_ind( ds707_rmsmi_info.IS707pkt_iface_ptr );
      break;
    }

    /*-----------------------------------------------------------------------
      Ignore event in all other states.
    -----------------------------------------------------------------------*/
    case DS707_RMSM_NULL_STATE:
    case DS707_RMSM_WAITING_FOR_UM_UP_RELAY_OR_SIP_STATE:
    case DS707_RMSM_UM_RM_UP_RELAY_OR_SIP_STATE:
    case DS707_RMSM_WAITING_FOR_UM_DOWN_STATE:
    case DS707_RMSM_WAITING_FOR_RM_UP_STATE:
    case DS707_RMSM_RM_UP_WAITING_FOR_UM_DOWN_STATE:
    case DS707_RMSM_WAITING_FOR_RM_DOWN_STATE:
    {
      /*---------------------------------------------------------------------
        Ignore event in all other states.
      ---------------------------------------------------------------------*/
      MSG_HIGH( "Ignoring RM_IFACE_UP_EV in %d state",
                ds707_rmsmi_info.state, 0, 0 );
      break;
    }

    default:
    {
      /*---------------------------------------------------------------------
        Invalid state
      ---------------------------------------------------------------------*/
      ERR("Invalid RMSM state %d", ds707_rmsmi_info.state, 0, 0);
      ASSERT( 0 );
    }
  } /* switch( ds707_rmsmi_info.state ) */
} /* ds707_rmsmi_process_um_mip_up_ev() */

/*===========================================================================
  FUNCTION DS707_RMSMI_PROCESS_UM_MIP_DOWN_EV()

  DESCRIPTION
    This function processes DS707_RMSM_UM_MIP_DOWN_EV event. Changes state to
    DS707_RMSM_WAITING_FOR_RM_DOWN_STATE or DS707_RMSM_NULL_STATE.

  PARAMETERS
    None

  RETURN VALUE
    None

  DEPENDENCIES
    ds707_rmsm_init() has to have been called first.

  SIDE EFFECTS
    None
===========================================================================*/
LOCAL void ds707_rmsmi_process_um_mip_down_ev
(
  void
)
{
/*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

#ifdef T_ARM
  MSG_MED( "Recvd UM_MIP_DOWN_EV, state %d", ds707_rmsmi_info.state, 0, 0 );
#else
  MSG_MED( "Recvd UM_MIP_DOWN_EV, state '%s'\n",
           ds707_rmsmi_state_names[ds707_rmsmi_info.state], 0, 0 );
#endif

  switch( ds707_rmsmi_info.state )
  {
    case DS707_RMSM_NULL_STATE:
    case DS707_RMSM_WAITING_FOR_UM_UP_RELAY_OR_SIP_STATE:
    case DS707_RMSM_UM_RM_UP_RELAY_OR_SIP_STATE:
    case DS707_RMSM_WAITING_FOR_RM_DOWN_STATE:
    {
      /*---------------------------------------------------------------------
        Ignore event in all other states.
      ---------------------------------------------------------------------*/
      MSG_HIGH( "Ignoring UM_MIP_DOWN_EV in %d state",
                ds707_rmsmi_info.state, 0, 0 );
      break;
    }

    case DS707_RMSM_WAITING_FOR_RM_UP_STATE:
    case DS707_RMSM_UM_RM_UP_MIP_STATE:
    case DS707_RMSM_RESYNCING_UM_STATE:
    case DS707_RMSM_RM_UP_WAITING_FOR_UM_DOWN_STATE:
    {
      ds707_rmsmi_transition_state( DS707_RMSM_WAITING_FOR_RM_DOWN_STATE );
      break;
    }

    case DS707_RMSM_WAITING_FOR_UM_UP_MIP_STATE:
    {
      if (dsat707_qcmip_val == 2)
      {
        ds707_rmsmi_transition_state( DS707_RMSM_NULL_STATE );
      }
      else /* dsat707_qcmip_val == 1 */
      {
        /*-------------------------------------------------------------------
          Restore netmdl meta state to OOC and fallback to SIP
        -------------------------------------------------------------------*/
        MSG_MED ("Fall back to Simple IP",0,0,0);
        netmdl_set_meta_state( PPP_OOC_META_STATE );
        ds707_rmsmi_transition_state( 
          DS707_RMSM_WAITING_FOR_UM_UP_RELAY_OR_SIP_STATE );
      }
      break;
    }

    case DS707_RMSM_WAITING_FOR_UM_DOWN_STATE:
    {
      ds707_rmsmi_transition_state( DS707_RMSM_NULL_STATE );
      break;
    }

    default:
    {
      /*---------------------------------------------------------------------
        Invalid state
      ---------------------------------------------------------------------*/
      ERR("Invalid RMSM state %d", ds707_rmsmi_info.state, 0, 0);
      ASSERT( 0 );
    }
  } /* switch( ds707_rmsmi_info.state ) */
} /* ds707_rmsmi_process_um_mip_down_ev() */

#ifdef FEATURE_DS_MULTIPLE_NAI
/*===========================================================================
FUNCTION DS707_RMSM_GET_MIP_SESSION_INFO_CB()

DESCRIPTION
  Function registered with MIP to be called at the end of MIP config session
  to update NAI information for laptop calls.

PARAMETERS
  session: pointer to the mip session control block where info is stored
  profile: profile number associated with the mip session.

RETURN VALUE
  None.

DEPENDENCIES
  None

SIDE EFFECTS
  None
===========================================================================*/
void ds707_rmsm_get_mip_session_info_cb
(
  byte                     active_mip_profile,
  mip_session_info_type *  session
)
{
/*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  mip_reg_ses_info_cb( NULL );

  if(mip_get_rm_nai(session,active_mip_profile)!=active_mip_profile)
  {
    MSG_ERROR("Error reading tethered NAIs from NV", 0, 0, 0);
    return;
  }
} /* ds707_rmsm_get_mip_session_info_cb() */
#endif /* FEATURE_DS_MULTIPLE_NAI */

#endif /* FEATURE_DS_MOBILE_IP */


/*===========================================================================
  FUNCTION DS707_RMSMI_TRANSITION_STATE()

  DESCRIPTION
    This function effects the transition to the state that is passed in.

  PARAMETERS
    new_state: the state to transition to

  RETURN VALUE
    None

  DEPENDENCIES
    Its the caller's responsibility that it initiates only valid state 
    transitions. This function does not check if a state transition is 
    valid or not.

  SIDE EFFECTS
    None
===========================================================================*/
LOCAL void ds707_rmsmi_transition_state
(
  ds707_rmsmi_state_type new_state
)
{
#ifdef FEATURE_DS_MOBILE_IP
  ppp_dev_opts_type  dev_opts;
  ip_addr_type       ip_addr;
#endif /* FEATURE_DS_MOBILE_IP */
  dsm_item_type *item_ptr;
  route_acl_id_type acl_id;       /* ACL identifier to reenable IS707 ACLs */
  int16  ps_iface_errno;
/*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

#ifdef T_ARM
  MSG_FATAL( "RMSM state goes %d to %d", ds707_rmsmi_info.state, new_state, 0 );
#else
  MSG_FATAL( "RMSM state goes '%s' to '%s'\n",
           ds707_rmsmi_state_names[ds707_rmsmi_info.state],
           ds707_rmsmi_state_names[new_state],
           0
         );
#endif

  ds707_rmsmi_info.state = new_state;

  /*-------------------------------------------------------------------------
    Perform action associated with entering into new state.
  -------------------------------------------------------------------------*/
  switch( new_state )
  {
    case DS707_RMSM_NULL_STATE:
    {
      /*---------------------------------------------------------------------
        Pkt call is terminating, Perform all the necessary cleanup
      ---------------------------------------------------------------------*/
      ds3g_siolib_set_inbound_flow( DS_FLOW_707_RMSM_MASK, DS_FLOW_DISABLE );

      ds3g_siolib_set_cd_state( DS3G_SIOLIB_REMOTE_CARRIER_OFF );
      ds3g_siolib_change_mode( SIO_DS_AUTODETECT_MODE, NULL, NULL, NULL );
      dsat_send_result_code( DSAT_NO_CARRIER );

      /*---------------------------------------------------------------------
        Deregister all the events with IS707_PKT_IFACE
      ---------------------------------------------------------------------*/

      ps_iface_event_cback_dereg( ds707_rmsmi_info.IS707pkt_iface_ptr,
                                  IFACE_DOWN_EV,
                                  ds707_rmsmi_info.iface_down_buf_ptr );

      ps_iface_event_cback_dereg( ds707_rmsmi_info.IS707pkt_iface_ptr,
                                  IFACE_PHYS_LINK_DOWN_EV,
                                  ds707_rmsmi_info.phys_link_down_buf_ptr );

      ps_iface_event_cback_dereg( ds707_rmsmi_info.IS707pkt_iface_ptr,
                                  IFACE_PHYS_LINK_UP_EV,
                                  ds707_rmsmi_info.phys_link_up_buf_ptr );

      ps_iface_event_cback_dereg( ds707_rmsmi_info.IS707pkt_iface_ptr,
                                  IFACE_FLOW_ENABLED_EV,
                                  ds707_rmsmi_info.flow_enable_buf_ptr );

      ps_iface_event_cback_dereg( ds707_rmsmi_info.IS707pkt_iface_ptr,
                                  IFACE_FLOW_DISABLED_EV,
                                  ds707_rmsmi_info.flow_disable_buf_ptr );

      /*---------------------------------------------------------------------
        Restore the tear down cmd callback to the original function
      ---------------------------------------------------------------------*/
      ds707_rmsmi_info.IS707pkt_iface_ptr->bring_up_cmd_f_ptr = 
        ds707_rmsmi_info.orig_bring_up_f_ptr;
      ds707_rmsmi_info.IS707pkt_iface_ptr->tear_down_cmd_f_ptr = 
        ds707_rmsmi_info.orig_tear_down_f_ptr;

      /*---------------------------------------------------------------------
        For network mode, cleanup PPP on RM and deregister callbacks
      ---------------------------------------------------------------------*/
      if( dsat707_crm_val == DS_CRM_VAL_NETMODEL_MODE )
      {
        ppp_reg_event( PPP_RM_DEV, PPP_DOWN_EV, NULL, NULL );
        ppp_reg_event( PPP_UM_SN_DEV, PPP_DOWN_EV, NULL, NULL );
        ppp_reg_event( PPP_UM_SN_DEV, PPP_RESYNC_EV, NULL, NULL );
        ppp_abort( PPP_RM_DEV );
      }
      /*---------------------------------------------------------------------
        For relay mode, set the PPP mode to NONE for both UM and RM, 
        there isn't anything else to cleanup on PPP
      ---------------------------------------------------------------------*/
      else
      {
        ppp_set_mode( PPP_UM_SN_DEV, PPP_NONE_MODE );
        ppp_set_mode( PPP_RM_DEV, PPP_NONE_MODE );
      }

      netmdl_set_meta_state( PPP_OOC_META_STATE );

      /*---------------------------------------------------------------------
        Cleanup RLP rx queue and restore original queues/wms on IS707 iface
        for all calls.  Always cleanup local watermarks.
      ---------------------------------------------------------------------*/
      ds707_pkt_setup_rlp( NULL, NULL, NULL, DS707_DEF_PKT_INSTANCE );
        
      while ( (item_ptr =
              (dsm_item_type *) q_get(&ds707_rmsmi_info.rlp_fwd_q)
              ) != NULL )
      {
        dsm_free_packet( &item_ptr );
      }
      dsm_empty_queue(&ds707_rmsmi_info.sio_rev_wmk);
      dsm_empty_queue(&ds707_rmsmi_info.sio_fwd_wmk);

      /*---------------------------------------------------------------------
        Set the resync behaviour back to default setting
      ---------------------------------------------------------------------*/
      netmdl_set_resync_behaviour (NETMDL_RESYNC_DFLT);

      ps_iface_phys_link_down_ind( &ds707_rmsmi_info.sio_707pkt_iface );
      ps_iface_disabled_ind( &ds707_rmsmi_info.sio_707pkt_iface );

      /*---------------------------------------------------------------------
        Restore SIO and 707 pkt interfaces to correct state
      ---------------------------------------------------------------------*/
      acl_id.if_ptr = ds707_rmsmi_info.IS707pkt_iface_ptr;
      route_acl_control( ROUTE_ACL_IFACE_ID, acl_id, TRUE );
      ds707_rmsmi_info.is_in_use = FALSE;
      /*---------------------------------------------------------------------
        Indicate to siolib that we are not in call anymore, deregister
        all the event handlers
      ---------------------------------------------------------------------*/
      ds3g_siolib_set_call_coming_up( FALSE );

      ds3g_siolib_deregister_callback_func( &ds707_rmsmi_sio_func_table );

      /*---------------------------------------------------------------------
        Enforce flow enable by passing the override mask to make sure next
        call can come up. Only reason to keep the flow disabed is if we are 
        out of memory.
      ---------------------------------------------------------------------*/
      ds3g_siolib_set_inbound_flow( (DS_FLOW_ALL_MASK & 
                                    ~DS_FLOW_DSM_LEVEL_MASKS),
                                    DS_FLOW_ENABLE );

      /*---------------------------------------------------------------------
        Set the abort cback in ds707 pkt manager to NULL.
      ---------------------------------------------------------------------*/
      MSG_MED("RMSM setting abort cback to NULL",0,0,0);
      ds707_pkt_set_abort_f_ptr(DS707_DEF_PKT_INSTANCE, NULL);

      break;
    }

#ifdef FEATURE_DS_MOBILE_IP
    case DS707_RMSM_WAITING_FOR_UM_UP_MIP_STATE:
    {

      /*---------------------------------------------------------------------
        Set netmdl meta state to INIT
      ---------------------------------------------------------------------*/
      netmdl_set_meta_state( PPP_INIT_META_STATE );

      /*---------------------------------------------------------------------
        For laptop MIP calls fallback to SIP is handled by RMSM since it 
        executes a different state machine. Tell dssnet to fail call
        if MIP fails and not to fallback to SIP on ite own.
      ---------------------------------------------------------------------*/
      dssnet_override_qcmip();
      dssnet_set_te_client();
      MIP_SEND_CMD( MIPIO_PPP_OPEN_CMD, NULL );
      break;
    }
#endif /* FEATURE_DS_MOBILE_IP */

#ifdef FEATURE_DS_MOBILE_IP
    case DS707_RMSM_WAITING_FOR_RM_UP_STATE:
    {   
      /*---------------------------------------------------------------------
        Change the sio mode to PKT. 
      ---------------------------------------------------------------------*/
      ds707_rmsmi_setup_sio_watermarks(
        &ds707_rmsmi_info.sio_fwd_wmk,
        &ds707_rmsmi_info.sio_rev_wmk);
      
      /*---------------------------------------------------------------------
        Copy the IP address from the Um interface to the IPCP remote 
        working values as the IP address to be assigned to the TE2.
        Set will and want to negotiate IP address on the Rm.
      ---------------------------------------------------------------------*/
      ppp_get_default_opts (&dev_opts);

      dev_opts.rx_f_ptr       = ds707_rmsmi_sio_rx_ppp_data;
      dev_opts.tx_f_ptr       = ds707_rmsmi_sio_tx_ppp_data;
      dev_opts.bridge_dev     = PPP_UM_SN_DEV;
      dev_opts.rx_iface_ptr   = &ds707_rmsmi_info.sio_707pkt_iface;
      dev_opts.lcp_info.mode  = PPP_CP_PASSIVE;
      dev_opts.ipcp_info.mode = PPP_CP_ACTIVE;
      dev_opts.hdlc_mode      = HDLC_SW;

      ppp_get_bridge_dev_prot_opts(PPP_UM_SN_DEV, &dev_opts);

      /*---------------------------------------------------------------------
        Assign the IP address obtained on the Um to the peer on the Rm
        interface and give a private address to the local side of Rm
        interface.Assign the DNS addresses, obtained over Um interface, to 
        the Rm interface.
      ---------------------------------------------------------------------*/
      ip_addr.type = IPV4_ADDR;
      ps_iface_get_peer_addr(ds707_rmsmi_info.IS707pkt_iface_ptr, &ip_addr);
      ppp_set_v4_server_opts(
        &dev_opts,
        ip_addr.addr.v4,
        PS_IFACE_IP_V4_ADDR(ds707_rmsmi_info.IS707pkt_iface_ptr),
        ds707_rmsmi_info.IS707pkt_iface_ptr->v4_net_info.primary_dns,
        ds707_rmsmi_info.IS707pkt_iface_ptr->v4_net_info.secondary_dns
      );

      /*---------------------------------------------------------------------
        Register for desired events with PPP on RM, don't bother with 
        resync on Rm, this should be handled by netmodel
      ---------------------------------------------------------------------*/
      ppp_reg_event( PPP_RM_DEV,
                     PPP_DOWN_EV,
                     ds707_rmsmi_ppprm_event_handler_cb,
                     NULL );

      ppp_reg_event( PPP_RM_DEV,
                     PPP_UP_EV,
                     ds707_rmsmi_ppprm_event_handler_cb,
                     NULL );

      /*---------------------------------------------------------------------
        Change the sio mode to PKT and Assert DCD and return CONNECT to 
        start PPP on Rm
      ---------------------------------------------------------------------*/
      ds3g_siolib_change_mode( SIO_DS_PKT_MODE,
                               &ds707_rmsmi_info.sio_rev_wmk,
                               &ds707_rmsmi_info.sio_fwd_wmk,
                               NULL );
      ds3g_siolib_set_cd_state( DS3G_SIOLIB_REMOTE_CARRIER_ON );
      dsat_send_result_code( DSAT_CONNECT );

      if( ppp_start( PPP_RM_DEV, &dev_opts ) < 0 )
      {
        MSG_ERROR("PPP RM config failed, posting RM_IFACE_DOWN", 0, 0, 0 );
        ds707_rmsm_post_event( DS707_RMSM_RM_IFACE_DOWN_EV );
        break;
      }

      break;
    }
#endif /* FEATURE_DS_MOBILE_IP */

    case DS707_RMSM_WAITING_FOR_RM_DOWN_STATE:
    {
      /*---------------------------------------------------------------------
        Stop PPP on Rm 
      ---------------------------------------------------------------------*/
      MSG_MED( "Stopping PPP on RM", 0, 0, 0 );
      ppp_stop( PPP_RM_DEV );
      break;
    }

#ifdef FEATURE_DS_MOBILE_IP
    case DS707_RMSM_RM_UP_WAITING_FOR_UM_DOWN_STATE:
    {
      MIP_SEND_CMD( MIPIO_PPP_CLOSE_CMD, NULL );
      break;
    }
#endif /* FEATURE_DS_MOBILE_IP */

#ifdef FEATURE_DS_MOBILE_IP
    case DS707_RMSM_RESYNCING_UM_STATE:
    {
      break;
    }
#endif /* FEATURE_DS_MOBILE_IP */

#ifdef FEATURE_DS_MOBILE_IP
    case DS707_RMSM_UM_RM_UP_MIP_STATE:
    {
      /*---------------------------------------------------------------------
        Put SIO iface in UP state and also put phys link state to UP
        so that PPP can cleanup gracefully during call termination.
      ---------------------------------------------------------------------*/
      ps_iface_up_ind( &ds707_rmsmi_info.sio_707pkt_iface );
      ps_iface_phys_link_up_ind( &ds707_rmsmi_info.sio_707pkt_iface );

      /*---------------------------------------------------------------------
        Rm is up with same options as Um: set Um and Rm to pseudo network and
        put the PPP meta state machine in the up state.
      ---------------------------------------------------------------------*/
      ppp_set_mode( PPP_RM_DEV, PPP_PSEUDO_NET_MODE );
      ppp_set_mode( PPP_UM_SN_DEV, PPP_PSEUDO_NET_MODE );
      netmdl_set_meta_state( PPP_UP_META_STATE );

#ifdef FEATURE_CALL_THROTTLE
      /*----------------------------------------------------------------------
        MIP call up successfully. Post SUCCESS event to DCTM.
      ----------------------------------------------------------------------*/
      MSG_MED("MIP call up. Posting SUCCESS event to CM for DCTM.", 0, 0, 0);
      dctm_post_mip_call_status( MIP_SUCCESS_EV, MIP_FAIL_REASON_NONE );
#endif /* FEATURE_CALL_THROTTLE */

#ifdef FEATURE_DS_MOBILE_IP_PERF
      if ( qw_cmp( mip_perf_info[PERF_TOTAL_CALL_SETUP_DELAY].ts_start,
                 ts_null ) != 0)
      {
        /*-------------------------------------------------------------------
          Notify M.IP call performance that M.IP call setup is complete for
          network model calls.
        -------------------------------------------------------------------*/
        mip_perf_delay( &mip_perf_info[PERF_TOTAL_CALL_SETUP_DELAY],
                        PERF_TOTAL_CALL_SETUP_DELAY,
                        PERF_END_TS );
      }
#endif /* FEATURE_DS_MOBILE_IP_PERF */

      /*---------------------------------------------------------------------
        Register theTerm-Req callback that causes the interface it is called
        on to get torn down for both interfaces and both LCP and IPCP.
        NOTE: this MUST be done AFTER the meta state has been set to UP - this
        guarantees that these callbacks are not over written by setting the
        meta state.
      ---------------------------------------------------------------------*/
      (void)snoop_reg_event( PPP_RM_DEV,
                             PPP_LCP_PROTOCOL,
                             TERM_REQ,
                             ds707_rmsmi_netmdl_term_req_cb );

      (void)snoop_reg_event( PPP_UM_SN_DEV,
                             PPP_LCP_PROTOCOL,
                             TERM_REQ,
                             ds707_rmsmi_netmdl_term_req_cb );

      (void)snoop_reg_event( PPP_RM_DEV,
                             PPP_IPCP_PROTOCOL,
                             TERM_REQ,
                             ds707_rmsmi_netmdl_term_req_cb );

      (void)snoop_reg_event( PPP_UM_SN_DEV,
                             PPP_IPCP_PROTOCOL,
                             TERM_REQ,
                             ds707_rmsmi_netmdl_term_req_cb );

      /*---------------------------------------------------------------------
        Register the ds707_rmsmi_rm_resync_cb. This would ensure that the
        call is released on detecting a PPP resync on the Rm link
      ---------------------------------------------------------------------*/
      (void)snoop_reg_event( PPP_RM_DEV,
                             PPP_LCP_PROTOCOL,
                             CONFIG_REQ,
                             ds707_rmsmi_rm_resync_cb );

      (void)snoop_reg_event( PPP_RM_DEV,
                             PPP_IPCP_PROTOCOL,
                             CONFIG_REQ,
                             ds707_rmsmi_rm_resync_cb );
      break;
    }
#endif /* FEATURE_DS_MOBILE_IP */

    case DS707_RMSM_WAITING_FOR_UM_UP_RELAY_OR_SIP_STATE:
    {
      /*---------------------------------------------------------------------
        Disable ACL so that sockets do not get 1X iface thru early 
        routing. This will prevent sockets from originating while we are 
        in laptop packet call.
        Note: ACL's are not disbaled for MIP laptop calls since MIP is a 
        socket application, hence early routng must work.
      ---------------------------------------------------------------------*/
      acl_id.if_ptr = ds707_rmsmi_info.IS707pkt_iface_ptr;
      route_acl_control( ROUTE_ACL_IFACE_ID, acl_id, FALSE );

      /*---------------------------------------------------------------------
        Set the abort cback in ds707 pkt manager. Note that for MIP Laptop
        calls, dssnet handles this functionality.
      ---------------------------------------------------------------------*/
      MSG_MED("RMSM setting abort cback",0,0,0);
      ds707_pkt_set_abort_f_ptr(DS707_DEF_PKT_INSTANCE,
                                ds707_rmsmi_call_abort_cback);

      /*---------------------------------------------------------------------
        Ask IS707_PKT_CTRL to initialize RLP.  For Relay mode calls, DS707_RMSM
        provides config info to IS707_PKT_CTRL which is used to
        initialize RLP. For SIP Network Model calls IS707_PKT_CTRL uses its
        own configuration (1X generic pkt config) for RLP.
      ---------------------------------------------------------------------*/
      if( dsat707_crm_val == DS_CRM_VAL_NETMODEL_MODE )
      {
        ds707_pkt_setup_rlp( NULL, NULL, NULL, DS707_DEF_PKT_INSTANCE );

#ifdef FEATURE_HDR
#error code not present
#endif /* FEATURE_HDR */

      }
      else
      {
#ifdef FEATURE_HDR
#error code not present
#endif /* FEATURE_HDR */
        {
          /*------------------------------------------------------------------
            For relay calls SIO Rx wm is for RLP transmissions.
          -----------------------------------------------------------------*/
          ds707_pkt_setup_rlp( &ds707_rmsmi_info.sio_rev_wmk,
                               ds707_rmsmi_sio_tx_rlp_data,
                               &ds707_rmsmi_info.rlp_fwd_q,
                               DS707_DEF_PKT_INSTANCE );
        }

      }

#ifdef FEATURE_HDR
#error code not present
#endif /* FEATURE_HDR */

      /*---------------------------------------------------------------------
        For Relay and Network model calls we register our own callback
        to bring up and tear down IS707_PKT_IFACE.  The original callback is 
        already saved and is restored when RMSM goes to NULL.
      ---------------------------------------------------------------------*/
      ds707_rmsmi_info.IS707pkt_iface_ptr->bring_up_cmd_f_ptr = 
        rmsmi_707_pkt_bring_up_cb;
      ds707_rmsmi_info.IS707pkt_iface_ptr->tear_down_cmd_f_ptr = 
        rmsmi_707_pkt_tear_down_cb;

      /*---------------------------------------------------------------------
        Register for desired events with IS707_PKT_IFACE
      ---------------------------------------------------------------------*/
      if ( ps_iface_event_cback_reg( ds707_rmsmi_info.IS707pkt_iface_ptr,
                                     IFACE_DOWN_EV,
                                     ds707_rmsmi_info.iface_down_buf_ptr 
                                   ) < 0 )
      {
        MSG_ERROR("IFACE_DOWN_EV reg failed", 0, 0, 0 );
        ds707_rmsm_post_event( DS707_RMSM_UM_IFACE_DOWN_EV );
        break;
      }

      if ( ps_iface_event_cback_reg( ds707_rmsmi_info.IS707pkt_iface_ptr,
                                     IFACE_PHYS_LINK_DOWN_EV,
                                     ds707_rmsmi_info.phys_link_down_buf_ptr 
                                   ) < 0 ) 
      {
        MSG_ERROR("PHYS_LINK_DOWN_EV reg failed", 0, 0, 0 );
        ds707_rmsm_post_event( DS707_RMSM_UM_IFACE_DOWN_EV );
        break;
      }

      if ( ps_iface_event_cback_reg( ds707_rmsmi_info.IS707pkt_iface_ptr,
                                     IFACE_PHYS_LINK_UP_EV,
                                     ds707_rmsmi_info.phys_link_up_buf_ptr 
                                   ) < 0 ) 
      {
        MSG_ERROR("PHYS_LINK_UP_EV reg failed", 0, 0, 0 );
        ds707_rmsm_post_event( DS707_RMSM_UM_IFACE_DOWN_EV );
        break;
      }

      if( ps_iface_bring_up_cmd( ds707_rmsmi_info.IS707pkt_iface_ptr,
                                 &ps_iface_errno,
                                 NULL ) < 0 )
      {
        if( ps_iface_errno != DS_EWOULDBLOCK )
        {
          /*-----------------------------------------------------------------
            Bringing up failed, post appropriate event to cleanup RMSM and 
            current pkt data call 
          -----------------------------------------------------------------*/
          MSG_ERROR("UM bringup failed, posting UM_IFACE_DOWN", 0, 0, 0 );
          ds707_rmsm_post_event( DS707_RMSM_UM_IFACE_DOWN_EV );
          break;
        }
      }
      else
      {
        /*-----------------------------------------------------------------
          Iface cannot already be up
        -----------------------------------------------------------------*/
        ERR_FATAL("Iface 0x%x already up during call setup",
                  ds707_rmsmi_info.IS707pkt_iface_ptr, 0, 0);
      }

      break;
    }

    case DS707_RMSM_UM_RM_UP_RELAY_OR_SIP_STATE:
    {

      /*---------------------------------------------------------------------
        Move SIO iface to UP state since nothing special needs to be done. 
        Move IS707 to Routeable state since we are in a laptop call. 
        Put the phys link state on SIO iface to UP so that PPP can 
        gracefully terminate during call  termination.
        Now configure PPP on both the interfaces.
      ---------------------------------------------------------------------*/

      ps_iface_routeable_ind( ds707_rmsmi_info.IS707pkt_iface_ptr );
      ps_iface_up_ind( &ds707_rmsmi_info.sio_707pkt_iface );
      ps_iface_phys_link_up_ind( &ds707_rmsmi_info.sio_707pkt_iface );

      if( dsat707_crm_val == DS_CRM_VAL_NETMODEL_MODE )
      {
        ppp_get_default_opts( &ds707_rmsmi_info.rm_ppp_config );
        ds707_rmsmi_info.rm_ppp_config.rx_f_ptr       = 
          ds707_rmsmi_sio_rx_ppp_data;
        ds707_rmsmi_info.rm_ppp_config.tx_f_ptr       = 
          ds707_rmsmi_sio_tx_ppp_data;
        ds707_rmsmi_info.rm_ppp_config.rx_iface_ptr   = 
          &ds707_rmsmi_info.sio_707pkt_iface;
        ds707_rmsmi_info.rm_ppp_config.lcp_info.mode  = PPP_CP_PASSIVE;
        ds707_rmsmi_info.rm_ppp_config.ipcp_info.mode = PPP_CP_PASSIVE;
        ds707_rmsmi_info.rm_ppp_config.hdlc_mode      = HDLC_SW;
        ds707_rmsmi_info.rm_ppp_config.bridge_dev     = PPP_UM_SN_DEV;

        if( ppp_start( PPP_RM_DEV, &ds707_rmsmi_info.rm_ppp_config ) < 0 )
        {
          MSG_ERROR("PPP RM config failed, posting RM_IFACE_DOWN", 0, 0, 0 );
          ds707_rmsm_post_event( DS707_RMSM_RM_IFACE_DOWN_EV );
          break;
        }

        if( ds707_pkt_setup_SN_ppp( DS707_DEF_PKT_INSTANCE ) < 0 )
        {
          MSG_ERROR("PPP UM config failed, posting UM_PPP_DOWN", 0, 0, 0 );
          ds707_rmsm_post_event( DS707_RMSM_UM_PPP_DOWN_EV );
          break;
        }

        ppp_reg_event( PPP_RM_DEV,
                       PPP_DOWN_EV,
                       ds707_rmsmi_ppprm_event_handler_cb,
                       NULL );

        ppp_reg_event( PPP_UM_SN_DEV,
                       PPP_DOWN_EV,
                       ds707_rmsmi_pppum_event_handler_cb,
                       NULL );

        ppp_reg_event( PPP_UM_SN_DEV,
                       PPP_RESYNC_EV,
                       ds707_rmsmi_pppum_event_handler_cb,
                       NULL );

        ppp_set_mode( PPP_UM_SN_DEV, PPP_PSEUDO_NET_MODE );
        ppp_set_mode( PPP_RM_DEV, PPP_PSEUDO_NET_MODE );
        netmdl_set_meta_state( PPP_INIT_META_STATE );
      }
      else
      {
        /*-------------------------------------------------------------------
          For Relay calls, the call is pretty much up since PPP is handled in
          TE2 only.  Change the iface and PPP states appropriately.
        -------------------------------------------------------------------*/
        ps_iface_routeable_ind( ds707_rmsmi_info.IS707pkt_iface_ptr );
        ps_iface_up_ind( &ds707_rmsmi_info.sio_707pkt_iface );
        ps_iface_phys_link_up_ind( &ds707_rmsmi_info.sio_707pkt_iface );

        ppp_set_mode( PPP_UM_SN_DEV, PPP_RELAY_MODE );
        ppp_set_mode( PPP_RM_DEV, PPP_RELAY_MODE );

#ifdef FEATURE_CALL_THROTTLE
        MSG_MED("Posting PPP SUCCESS ev to CM for DCTM.",0,0,0);
        dctm_post_ppp_call_status( PPP_SUCCESS_EV, PPP_PROTOCOL_IPCP, 
                                   PPP_FAIL_REASON_NONE );
#endif /* FEATURE_CALL_THROTTLE */
      }

      ds707_rmsmi_setup_sio_watermarks(
        &ds707_rmsmi_info.sio_fwd_wmk,
        &ds707_rmsmi_info.sio_rev_wmk);

      ds3g_siolib_change_mode( SIO_DS_PKT_MODE,
                               &ds707_rmsmi_info.sio_rev_wmk,
                               &ds707_rmsmi_info.sio_fwd_wmk,
                               NULL );

      dsat_send_result_code( DSAT_CONNECT );

      ds3g_siolib_set_cd_state( DS3G_SIOLIB_REMOTE_CARRIER_ON );

      break;
    }

    case DS707_RMSM_WAITING_FOR_UM_DOWN_STATE:
    {
      break;
    }

    default:
    {
      ERR("Invalid state %d", new_state, 0, 0);
      ASSERT(0);
    }
  } /* switch(new state) */
} /* ds707_rmsmi_transition_state() */


/*===========================================================================
  FUNCTION DS707_RMSMI_CALL_ABORT_CB()

  DESCRIPTION
    This function is registered as abort handler with ATCOP. If ATCOP
    receives an AT cmd while processing an abortable cmd, this function gets
    called.  It is registered along with the dial string handler. It posts
    appropriate event to ds707_rmsm which initiates the call termination.

  PARAMETERS
    None

  RETURN VALUE
    None

  DEPENDENCIES
    None

  SIDE EFFECTS
    None
===========================================================================*/
LOCAL void ds707_rmsmi_call_abort_cb
(
  void
)
{
/*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  MSG_HIGH( "Call aborted, posting DS707_RMSM_RM_IFACE_DOWN_EV", 0, 0, 0);
  ds707_rmsm_post_event( DS707_RMSM_RM_IFACE_DOWN_EV );
} /* ds707_rmsmi_call_abort_cb() */


/*===========================================================================
  FUNCTION DS707_RMSMI_PROCESS_ESC_SEQ_CB

  DESCRIPTION
    This function is called when DTR goes low and AT&D = 1.  This should do
    the same action as the dtr_sig_cb.

  PARAMETERS
    None

  RETURN VALUE
    Always return DSAT_ONLINE_DATA, which implies 

  DEPENDENCIES
    None

  SIDE EFFECTS
    None
===========================================================================*/
LOCAL dsat_mode_enum_type ds707_rmsmi_process_esc_seq_cb(void)
{
/*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/
  ds707_rmsmi_process_dtr_sig_cb(FALSE);
  return(DSAT_ONLINE_DATA);
} /* ds707_rmsmi_process_esc_seq_cb() */


/*===========================================================================
  FUNCTION DS707_RMSMI_PROCESS_DTR_SIG_CB()

  DESCRIPTION
    This function processes the change in dtr state.  If DTR is deasserted
    appropriate event is posted to ds707_rmsm which initiates the call termination.
    DTR assert is ignored for now.

  PARAMETERS
    is_asserted: flag which indicates current DTR state -
                 TRUE: DTR asserted, FALSE: DTR deasserted

  RETURN VALUE
    None

  DEPENDENCIES
    None

  SIDE EFFECTS
    None
===========================================================================*/
LOCAL void ds707_rmsmi_process_dtr_sig_cb
(
  boolean is_asserted
)
{
/*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  /*-------------------------------------------------------------------------
    Currently only DTR deassert is handled, post appropriate event
  -------------------------------------------------------------------------*/

  if ( is_asserted == FALSE )
  {
    MSG_HIGH( "DTR dropped, posting DS707_RMSM_RM_IFACE_DOWN_EV", 0, 0, 0);
    ds707_rmsm_post_event( DS707_RMSM_RM_IFACE_DOWN_EV );
  }
} /* ds707_rmsmi_process_dtr_sig_cb() */


#ifdef FEATURE_DS_MOBILE_IP
/*===========================================================================
FUNCTION   DS707_RMSMI_NETMDL_TERM_REQ_CB()

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
  SNOOP_CB_PROCESS_PKT: this callback requires that the packets it
                       processes be alwyas sent up the stack for further
                       processing.

SIDE EFFECTS
  This will cause the receiving link to be torn down.
===========================================================================*/
LOCAL snoop_cb_ret_val_enum_type ds707_rmsmi_netmdl_term_req_cb
(
  ppp_dev_enum_type device,            /* the arrival interface            */
  uint16 protocol,                     /* protocol of packet               */
  dsm_item_type **item_head_ptr        /* item containing data             */
)
{
/*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  ASSERT(item_head_ptr != NULL && *item_head_ptr != NULL);
  ASSERT((device == PPP_RM_DEV) || (device == PPP_UM_SN_DEV));
  ASSERT((protocol == PPP_IPCP_PROTOCOL) || (protocol == PPP_LCP_PROTOCOL));

  ppp_set_mode(device, PPP_INTERNAL_MODE);

  return SNOOP_CB_PROCESS_PKT;

} /* ds707_rmsmi_netmdl_term_req_cb() */

/*===========================================================================
FUNCTION   DS707_RMSMI_RM_RESYNC_CB()

DESCRIPTION
  This callback function is called when an LCP/IPCP C-Req is received on the
  Rm link i.e. when a PPP resync is detected on the Rm link once the call
  has been established. Drop the packet in such event.

DEPENDENCIES
  This callback should be registered only after PPP is up on both Um and
  Rm links

RETURN VALUE
  SNOOP_CB_IGNORE_PKT: this callback never requires that the packets it
                       processes be re-examined

  Packets need to be looked at again when new callbacks have been registered
  which should be called with the packet in question.

SIDE EFFECTS
  None
===========================================================================*/
LOCAL snoop_cb_ret_val_enum_type ds707_rmsmi_rm_resync_cb
(
  ppp_dev_enum_type device,            /* the arrival interface            */
  uint16 protocol,                     /* protocol of packet               */
  dsm_item_type **item_head_ptr        /* item containing data             */
)
{
/*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/
  /*lint -e527 -esym(613, item_head_ptr) */
  ASSERT(item_head_ptr != NULL && *item_head_ptr != NULL);
  ASSERT(device == PPP_RM_DEV);
  ASSERT((protocol == PPP_IPCP_PROTOCOL) || (protocol == PPP_LCP_PROTOCOL));
  /*lint +e527 */

  /*-------------------------------------------------------------------------
    Drop the dsm_item
  -------------------------------------------------------------------------*/
  dsm_free_packet(item_head_ptr);

  /*-------------------------------------------------------------------------
    Close PPP on RM.
  -------------------------------------------------------------------------*/
  ppp_stop(device);

  return SNOOP_CB_IGNORE_PKT;

} /* ds707_rmsmi_rm_resync_cb() */
#endif /* FEATURE_DS_MOBILE_IP */


/*===========================================================================
FUNCTION   RMSMI_707_PKT_BRING_UP_CB()

DESCRIPTION
  This callback function is the cmd handler for bringing up 707 iface
  during a Relay or Network Model call.  Since a special handling is
  required for bringing up Relay or Netmodel SIP call, RMSM provides its
  own cmd handler rather than using the one provided by IS707 controller

DEPENDENCIES
  None

RETURN VALUE
  -1 for failure, 0 for success

SIDE EFFECTS
  None
===========================================================================*/
LOCAL int rmsmi_707_pkt_bring_up_cb
(
  ps_iface_type *this_iface_ptr,
  void *user_data
)
{
  int16  ps_iface_errno;
/*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  MSG_MED( "iface 0x%x bring up cmd", this_iface_ptr, 0, 0 );

  ASSERT(ds707_rmsmi_info.state != DS707_RMSM_NULL_STATE);
  ASSERT(this_iface_ptr == ds707_rmsmi_info.IS707pkt_iface_ptr)

  /*-------------------------------------------------------------------------
    We are bringing up the 707 pkt iface for Relay or Network model SIP
    call here. Since PPP connection is between TE2-IWF, all we need
    to do is bring up the traffic channel.
  -------------------------------------------------------------------------*/

  if( ( ps_iface_phys_link_up_cmd( ds707_rmsmi_info.IS707pkt_iface_ptr,
                                   &ps_iface_errno,
                                   NULL
                                 ) < 0 ) && 
      ( ps_iface_errno != DS_EWOULDBLOCK ) )
  {  
    /*---------------------------------------------------------------------
      Traffic channel could not be brought up, bring down iface and 
      terminate the call
    ---------------------------------------------------------------------*/
    ps_iface_down_ind( ds707_rmsmi_info.IS707pkt_iface_ptr );
  }
  
  return 0;

} /* rmsmi_707_pkt_bring_up_cb */


/*===========================================================================
FUNCTION   RMSMI_707_PKT_TEAR_DOWN_CB()

DESCRIPTION
  This callback function is the cmd handler for tearing down 707 iface
  during a Relay or Network Model call.  Since a special handling is
  required for tearing down Relay or Netmodel SIP call, RMSM provides its
  own cmd handler rather than using the one provided by IS707 controller

DEPENDENCIES
  None

RETURN VALUE
  -1 for failure, 0 for success

SIDE EFFECTS
  None
===========================================================================*/
LOCAL int rmsmi_707_pkt_tear_down_cb
(
  ps_iface_type *this_iface_ptr,
  void *user_data
)
{
  int16  ps_iface_errno;
/*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  MSG_MED( "iface 0x%x tear down cmd", this_iface_ptr, 0, 0 );

  ASSERT( ds707_rmsmi_info.state != DS707_RMSM_NULL_STATE );
  ASSERT(this_iface_ptr == ds707_rmsmi_info.IS707pkt_iface_ptr)

  /*-------------------------------------------------------------------------
    We are tearing down the 707 pkt iface for Relay or Network model SIP
    call here. Since PPP connection is between TE2-IWF, all we need
    to do is bring down the traffic channel.
  -------------------------------------------------------------------------*/

  if( ( ps_iface_phys_link_down_cmd( ds707_rmsmi_info.IS707pkt_iface_ptr,
                                     &ps_iface_errno,
                                     NULL) == 0 ) ||
      ps_iface_errno != DS_EWOULDBLOCK )
  {
    /*-----------------------------------------------------------------------
      Physical link is already down, move the 707 iface to down state.
      This will allow 707 controller to perform any remaining cleanup before
      the call on Rm could be cleaned up (in the down event callback)
    -----------------------------------------------------------------------*/
    ps_iface_down_ind( ds707_rmsmi_info.IS707pkt_iface_ptr );
  }

  return 0;

} /* rmsmi_707_pkt_tear_down_cb */


/*===========================================================================
FUNCTION   RMSMI_707_PKT_EVENT_HANDLER_CB()

DESCRIPTION
  This callback function is called when one of the registered ps_iface events
  occur on IS707_PKT_IFACE.  Registraion is done for various events based
  upon the  call type during the call setup type. As part of event handling
  appropriate event is posted to RMSM.

DEPENDENCIES
  None

RETURN VALUE
  None

SIDE EFFECTS
  None
===========================================================================*/
LOCAL void rmsmi_707_pkt_event_handler_cb
(
  ps_iface_type             *this_iface_ptr,
  ps_iface_event_enum_type   event,
  ps_iface_event_info_u_type event_info,
  void                      *user_data_ptr
)
{
/*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  MSG_MED( "Recvd iface ev %d from 1x i/f 0x%x", event, this_iface_ptr, 0 );

  /*-------------------------------------------------------------------------
    Process flow control events first since they may get posted during data
    transfer
  -------------------------------------------------------------------------*/

  switch( event )
  {
    case IFACE_FLOW_ENABLED_EV:
    {
      /*---------------------------------------------------------------------
        For netmodel call send signal to PS to resume pulling PPP packets 
        from SIO rx_wm, laptop flow will be enabled by rx_wm lo water func.
        For relay calls we do need to enable the flow to handle cases like 
        flow enable due to holddown timer etc. 
      ---------------------------------------------------------------------*/
      if ( dsat707_crm_val == DS_CRM_VAL_NETMODEL_MODE )
      {
        PS_SET_SIGNAL( PS_PPP_RM_RX_SIGNAL );
      }
      else
      {
        ds3g_siolib_set_inbound_flow(DS_FLOW_707_PKT_MASK, DS_FLOW_ENABLE);
      }

      break;
    }

    case IFACE_FLOW_DISABLED_EV:
    {
      /*---------------------------------------------------------------------
        For netmodel calls we don't need to do anything when UM flow is 
        disabled, rx function will stop pulling PPP packets from SIO rx_wm. 
        rx_wm hi water function will disable laptop flow.
        For relay calls we do need to disable flow to handle cases like 
        flow disable due to holddown timer etc.
      ---------------------------------------------------------------------*/
      if ( dsat707_crm_val != DS_CRM_VAL_NETMODEL_MODE )
      {
        ds3g_siolib_set_inbound_flow(DS_FLOW_707_PKT_MASK, DS_FLOW_DISABLE);
      }

      break;
    }

    case IFACE_DOWN_EV:
    {
      /*---------------------------------------------------------------------
        UM iface is down. This event can also be generated when we register 
        for events since that happens during call setup ie UM is in DOWN 
        state. So check the previous state before taking any action.
      ---------------------------------------------------------------------*/
      if( event_info.state != IFACE_DOWN )
      {
        MSG_HIGH( "IS707 iface down, posting DS707_RMSM_UM_IFACE_DOWN_EV",
                  0, 0, 0 );
        ds707_rmsm_post_event( DS707_RMSM_UM_IFACE_DOWN_EV );
      }
      else
      {
        MSG_MED( "Ignoring event %d", event, 0, 0 );
      }

      break;
    }

    case IFACE_PHYS_LINK_DOWN_EV:
    {
      /*---------------------------------------------------------------------
        Traffic channel on UM is down. This event can also be generated 
        when we register for events since that happens during call setup ie 
        UM phys link is in DOWN state. So check the previous state before 
        taking any action.
      ---------------------------------------------------------------------*/
      if ( event_info.link_state != PHYS_LINK_DOWN )
      {
        MSG_HIGH( "IS707 phys link down, posting DS707_RMSM_UM_PHYS_LINK_DOWN_EV",
                  0, 0, 0 );
        ds707_rmsm_post_event( DS707_RMSM_UM_PHYS_LINK_DOWN_EV );
      }
      else
      {
        MSG_MED( "Ignoring event %d", event, 0, 0 );
      }
      break;
    }

    case IFACE_PHYS_LINK_UP_EV:
    {
      /*---------------------------------------------------------------------
        Traffic channel on UM has come up. This event is used for Relay and
        network model call setup.
      ---------------------------------------------------------------------*/
      MSG_HIGH( "IS707 phys link up, posting DS707_RMSM_UM_PHYS_LINK_UP_EV",
                0, 0, 0 );
      ds707_rmsm_post_event( DS707_RMSM_UM_PHYS_LINK_UP_EV );
      break;
    }

    default:
    {
      ERR("Unregistered event %d recvd, ignoring", event, 0, 0);
      break;
    }
  } /* switch( event ) */
} /* rmsmi_707_pkt_event_handler_cb() */


/*===========================================================================
FUNCTION   DS707_RMSMI_PPPRM_EVENT_HANDLER_CB()

DESCRIPTION
  This callback function is called when one of the registered PPP events
  occur on RM PPP device. As part of event handling appropriate event is 
  posted to RMSM.

DEPENDENCIES  
  None

RETURN VALUE
  None

SIDE EFFECTS
  None
===========================================================================*/
LOCAL void ds707_rmsmi_ppprm_event_handler_cb
(
  ppp_dev_enum_type       ppp_dev,
  ppp_protocol_e_type     protocol,              /* PPP Protocol           */
  ppp_event_e_type        ppp_event,
  void                    *user_data,
  uint32                  session_handle,
  ppp_fail_reason_e_type  ppp_fail_reason
)
{
/*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/
  MSG_MED( "Recd %d ppp event", ppp_event, 0, 0 );

  ASSERT( ppp_dev == PPP_RM_DEV );
  switch(ppp_event)
  {
    /*-----------------------------------------------------------------------
      Currently we only handle the down event, we should not be getting 
      other events
    -----------------------------------------------------------------------*/
    case PPP_DOWN_EV:
      ds707_rmsm_post_event( DS707_RMSM_RM_IFACE_DOWN_EV );
      break;

    case PPP_UP_EV:
      ds707_rmsm_post_event( DS707_RMSM_RM_PPP_UP_EV );
      break;

    case PPP_RESYNC_EV:
      ERR("PPP cb with unreg event %d", ppp_event, 0, 0);
      break;

    default:
      ERR("Unsupported PPP event %d", ppp_event, 0, 0);
      ASSERT(0);
      break;
  }
} /* ds707_rmsmi_ppprm_event_handler_cb */


/*===========================================================================
FUNCTION   DS707_RMSMI_PPPUM_EVENT_HANDLER_CB()

DESCRIPTION
  This callback function is called when one of the registered PPP events
  occur on UM PPP device. As part of event handling appropriate event is 
  posted to RMSM.

DEPENDENCIES  
  None

RETURN VALUE
  None

SIDE EFFECTS
  None
===========================================================================*/
LOCAL void ds707_rmsmi_pppum_event_handler_cb
(
  ppp_dev_enum_type       ppp_dev,
  ppp_protocol_e_type     protocol,              /* PPP Protocol           */
  ppp_event_e_type        ppp_event,
  void                    *user_data,
  uint32                  session_handle,
  ppp_fail_reason_e_type  ppp_fail_reason
)
{
/*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/
  MSG_MED( "Recd %d ppp event", ppp_event, 0, 0 );

  ASSERT( ppp_dev == PPP_UM_SN_DEV );
  switch(ppp_event)
  {
    /*-----------------------------------------------------------------------
      Currently we only hanlde the down event, we should not be getting 
      other events
    -----------------------------------------------------------------------*/
    case PPP_DOWN_EV:
      ds707_rmsm_post_event( DS707_RMSM_UM_PPP_DOWN_EV );
      break;

    case PPP_RESYNC_EV:
      MSG_MED ("Um resync, trash queued outgoing data",0,0,0);
      ds707_pkt_trash_rlp_tx_q( ds707_rmsmi_info.IS707pkt_iface_ptr );
      break;

    case PPP_UP_EV:
      ERR("PPP cb with unreg event %d", ppp_event, 0, 0);
      break;

    default:
      ERR("Unsupported PPP event %d", ppp_event, 0, 0);
      ASSERT(0);
      break;
  }
} /* ds707_rmsmi_pppum_event_handler_cb */


/*===========================================================================
  FUNCTION DS707_RMSMI_SETUP_SIO_WATERMARKS()

  DESCRIPTION
    This function configures the SIO packet mode watermark items
    to operate for the 1X calls handled by RMSM.

  PARAMETERS
    *tx_wm_ptr - ptr to sio transmit (to laptop) watermark
    *rx_wm_ptr - ptr to sio receive (from laptop) watermark

  RETURN VALUE
    None

  DEPENDENCIES
    Hi/Lo Watermark Settings: The Lo and Hi watermark values and the don't
    exceed values for each watermark is inter-dependent with the DSM small
    and large item counts (DSM_LARGE_ITEM_CNT and DSM_SMALL_ITEM_CNT) in the
    DSM module. Any change to lo/hi watermark or don't exceed values can
    cause ERR_FATALs due to insufficient memory items available.

  SIDE EFFECTS
    None
===========================================================================*/
LOCAL void ds707_rmsmi_setup_sio_watermarks
(
  dsm_watermark_type *tx_wm_ptr,
  dsm_watermark_type *rx_wm_ptr
)
{
  boolean is_device_bt = FALSE;                    /* is device Bluetooth? */
  sio_ioctl_param_type ds707_rmsmi_sio_ioctl_param;
/*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  /*-------------------------------------------------------------------------
    Make sure that the watermarks passed in are empty
  -------------------------------------------------------------------------*/
  if(tx_wm_ptr->current_cnt != 0)
  {
    ERR("Tx WM 0x%x not empty, %d bytes", 
        tx_wm_ptr, tx_wm_ptr->current_cnt, 0);
    dsm_empty_queue(tx_wm_ptr);
  }
  if(rx_wm_ptr->current_cnt != 0)
  {
    ERR("Rx WM 0x%x not empty, %d bytes", 
        rx_wm_ptr, rx_wm_ptr->current_cnt, 0);
    dsm_empty_queue(rx_wm_ptr);
  }


  if( rdm_get_device( RDM_DATA_SRVC ) == RDM_BT_NA_DEV )
  {
    is_device_bt = TRUE;
  }

  /*-------------------------------------------------------------------------
                             SETUP SIO TX WATERMARK
  -------------------------------------------------------------------------*/

  /*-------------------------------------------------------------------------
    This WaterMark Queue is to be used for the purposes of SIO to laptop 
    transmisssion. 
    Since over the air link is slower than SIO TX bitrate,flow control is 
    not necessary. Hence not setting and lo and hi levels.
  -------------------------------------------------------------------------*/

  tx_wm_ptr->lo_watermark          = 0;
  tx_wm_ptr->hi_watermark          = 0;

#ifdef FEATURE_HDR
#error code not present
#endif /* FEATURE_HDR */
  {
#ifdef FEATURE_IS2000_REL_A
    /*-----------------------------------------------------------------------
      FCH + SCH = (30 * 21) + ( 8 * 30 * 93) = 22950
      (21 is size of FCH PDU and 93 is size of SCH PDU)
      (30 is RTT * number of NAK rounds. RTT is 10; Num Nak rounds = 3)
    -----------------------------------------------------------------------*/
    tx_wm_ptr->dont_exceed_cnt     = 23000;
#else
    /*-----------------------------------------------------------------------
      This value is based on the RLP 3 resequencing queue, which
      assumes up to 270 dsm items.  Assuming a 16x f-sch, then the
      8/9ths of the frames are 43 bytes of payload, and 1/9th are 21
      bytes.  The value below factors that in:
  
                    (30 * 21) + (240 * 43) = 10950
    -----------------------------------------------------------------------*/
    tx_wm_ptr->dont_exceed_cnt       = 10950;
#endif /* FEATURE_IS2000_REL_A */
  }
  tx_wm_ptr->gone_empty_func_ptr   = NULL;
  tx_wm_ptr->non_empty_func_ptr    = NULL;
  tx_wm_ptr->lowater_func_ptr      = NULL;
  tx_wm_ptr->hiwater_func_ptr      = NULL;
  tx_wm_ptr->each_enqueue_func_ptr = NULL;

  /*-------------------------------------------------------------------------
                             SETUP SIO RX WATERMARK
  -------------------------------------------------------------------------*/

  /*-------------------------------------------------------------------------
    Configure the watermark used for receiving data from SIO.
    to SIO. 
    SIO watermark use should be synchonized with the ISR used by the 
    serial driver. Also for bluetooth, provide more cushion between hi and 
    don't exceed levels.
    Removed checking for the flow control before setting the watermark 
    levels. Observe that the watermark levels are same for software 
    Flowcontrol and hardware flowcontrol.
  -------------------------------------------------------------------------*/
  ds707_rmsmi_sio_ioctl_param.flow_ctl.tx_flow = SIO_FCTL_BEST;
  ds707_rmsmi_sio_ioctl_param.flow_ctl.rx_flow = SIO_FCTL_BEST;

  ds3g_siolib_ioctl( SIO_IOCTL_GET_FLOW_CTL, &ds707_rmsmi_sio_ioctl_param );

  if ( ( ds3g_siolib_is_baud_valid( (sio_bitrate_type) dsat_ipr_val ) ) &&
       ( dsat_ipr_val >= SIO_BITRATE_115200 ) &&
       ( dsat_ipr_val <  SIO_BITRATE_BEST ) 
     )
  {
    rx_wm_ptr->lo_watermark        = 1500;
    rx_wm_ptr->hi_watermark        = 5000;

    if( is_device_bt == TRUE )
    {
      rx_wm_ptr->dont_exceed_cnt   = 10000;
    }
    else
    {
      rx_wm_ptr->dont_exceed_cnt   = 6000;
    }
  }
  else
  {
    rx_wm_ptr->lo_watermark        = 400;
    rx_wm_ptr->hi_watermark        = 1300;

    if( is_device_bt == TRUE )
    {
      rx_wm_ptr->dont_exceed_cnt   = 4000;
    }
    else
    {
      rx_wm_ptr->dont_exceed_cnt   = 2000;
    }
  }

  rx_wm_ptr->lowater_func_ptr      = ds707_rmsmi_sio_rx_lowater_func;
  rx_wm_ptr->hiwater_func_ptr      = ds707_rmsmi_sio_rx_hiwater_func;
  rx_wm_ptr->gone_empty_func_ptr   = ds707_rmsmi_sio_rx_gone_empty_func;
  rx_wm_ptr->non_empty_func_ptr    = ds707_rmsmi_sio_rx_non_empty_func;
  rx_wm_ptr->each_enqueue_func_ptr = NULL;

#ifdef FEATURE_DSM_MEM_CHK
  /*-------------------------------------------------------------------------
    Reset the total_rcvd_cnt, as a packet data call can really set this
    number very high.
  -------------------------------------------------------------------------*/
  tx_wm_ptr->total_rcvd_cnt        = 0;
  rx_wm_ptr->total_rcvd_cnt        = 0;
#endif
} /* ds707_rmsmi_setup_sio_watermarks() */


/*===========================================================================
  FUNCTION DS707_RMSMI_SIO_RX_GONE_EMPTY_FUNC()

  DESCRIPTION
    This function performs the 'Now Empty' event processing for the PACKET
    DATA SERVICES shared queue. This function is called when the SIO RX
    data stream watermark item goes empty. It clears its idle bit which
    allows IS707_PKT_CTRL to start cta dormancy if needed.

  PARAMETERS
    None

  RETURN VALUE
    None

  DEPENDENCIES
    None

  SIDE EFFECTS
    None
===========================================================================*/
LOCAL void ds707_rmsmi_sio_rx_gone_empty_func( void )
{
/*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/
  ds707_cta_clear_idle_bit( DS707_CTA_SIO_RX );
} /* ds707_rmsmi_sio_rx_gone_empty_func() */


/*===========================================================================
  FUNCTION DS707_RMSMI_SIO_RX_NON_EMPTY_FUNC()

  DESCRIPTION
    This function performs the 'Non Empty' event processing for the PACKET
    DATA SERVICES shared queue. This function is called when the SIO RX
    data stream watermark item gets its first item enqueued. It sets its
    idle bit which allows IS707_PKT_CTRL to end cta dormancy if needed.

  PARAMETERS
    None

  RETURN VALUE
    None

  DEPENDENCIES
    None

  SIDE EFFECTS
    None
===========================================================================*/
LOCAL void ds707_rmsmi_sio_rx_non_empty_func( void )
{
/*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/
  ds707_cta_set_idle_bit( DS707_CTA_SIO_RX );

  /*-------------------------------------------------------------------------
    Send signal to PS to start pulling stuff off the SIO.
  -------------------------------------------------------------------------*/
  if ( ( dsat707_crm_val == DS_CRM_VAL_NETMODEL_MODE ) &&
       ( PS_IFACE_FLOW_ENABLED( ds707_rmsmi_info.IS707pkt_iface_ptr ) ) )
  {
    PS_SET_SIGNAL( PS_PPP_RM_RX_SIGNAL );
  }
} /* ds707_rmsmi_sio_rx_non_empty_func() */


/*===========================================================================
  FUNCTION DS707_RMSMI_SIO_RX_LOWATER_FUNC()

  DESCRIPTION
   This function enables inbound serial flow, when the lowater point in the
   packet watermark is reached.

   This function is called from RLP TX processing, which occurs in the decode
   ISR.

  PARAMETERS
    None

  RETURN VALUE
    None

  DEPENDENCIES
    None

  SIDE EFFECTS
    None
===========================================================================*/
LOCAL void ds707_rmsmi_sio_rx_lowater_func( void )
{
/*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/
  ds3g_siolib_set_inbound_flow( DS_FLOW_PKT_WM_MASK, DS_FLOW_ENABLE );
} /* ds707_rmsmi_sio_rx_lowater_func() */


/*===========================================================================
  FUNCTION DS707_RMSMI_SIO_RX_HIWATER_FUNC()

  DESCRIPTION
   This function disables inbound serial flow, when the hiwater point in the
   packet watermark is reached.

   This is called from the serial RX ISR.

  PARAMETERS
    None

  RETURN VALUE
    None

  DEPENDENCIES
    None

  SIDE EFFECTS
    None
===========================================================================*/
LOCAL void ds707_rmsmi_sio_rx_hiwater_func( void )
{
/*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/
  ds3g_siolib_set_inbound_flow( DS_FLOW_PKT_WM_MASK, DS_FLOW_DISABLE );
}  /* ds707_rmsmi_sio_rx_hiwater_func() */


/*===========================================================================
  FUNCTION DS707_RMSMI_SIO_TX_RLP_DATA()

  DESCRIPTION
    This function performs the processing of Received RLP Data frames for the
    Packet Data Service. All processed RX data frames will be queued for
    Serial TX.

    This function will be invoked in Packet mode or PMA active mode by RLP RX
    processing upon reception of error free Data frame(s).

  PARAMETERS
    None

  DEPENDENCIES
    1) The 'data_ptr' field in each RLP data frame must have previously been
       set to the correct location of the RLP payload data.
    2) The queued data frames contain valid length field.
    3) Must be in PKT srvc mode.

  RETURN VALUE
    None.

  SIDE EFFECTS
    None.

===========================================================================*/
LOCAL void ds707_rmsmi_sio_tx_rlp_data( void )
{
  dsm_item_type     *item_ptr;             /* pointer to DSM item          */
/*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  /*-------------------------------------------------------------------------
    Remove each item from Completed Data queue and pass to Serial Driver.
  -------------------------------------------------------------------------*/
  while( ( item_ptr = ( dsm_item_type *)q_get( &ds707_rmsmi_info.rlp_fwd_q ) 
         ) != NULL )
  {
    ds3g_siolib_setup_tx( item_ptr, FALSE );
  }
} /* ds707_rmsmi_sio_tx_rlp_data() */


/*===========================================================================
FUNCTION DS707_RMSMI_SIO_TX_PPP_DATA

DESCRIPTION
  This function is called to transmit a packet coming on Um PPP
  interface onto Rm interface.

DEPENDENCIES
  None

RETURN VALUE
  None.

SIDE EFFECTS
  None.

===========================================================================*/
LOCAL void ds707_rmsmi_sio_tx_ppp_data
(
  dsm_item_type **item_head_ptr,
  void *user_data
)
{
/*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/
  ds3g_siolib_setup_tx( *item_head_ptr, FALSE );
  *item_head_ptr = NULL;

} /* ds707_rmsmi_sio_tx_ppp_data() */


/*===========================================================================
FUNCTION DS707_RMSMI_SIO_RX_PPP_DATA

DESCRIPTION
  This function is called to receive the next PPP packet, coming on the Rm
  interface, for rx processing.

DEPENDENCIES
  None

RETURN VALUE
  None.

SIDE EFFECTS
  None.

===========================================================================*/
LOCAL dsm_item_type* ds707_rmsmi_sio_rx_ppp_data( void *user_data )
{
/*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  if ( PS_IFACE_FLOW_ENABLED( ds707_rmsmi_info.IS707pkt_iface_ptr ) )
  {
    return dsm_dequeue( &ds707_rmsmi_info.sio_rev_wmk );
  }
  else
  {
    MSG_MED ("Flow disabled!",0,0,0);
    return NULL;
  }
} /* ds707_rmsmi_sio_rx_ppp_data() */


/*===========================================================================
FUNCTION DS707_RMSMI_CALL_ABORT_CBACK

DESCRIPTION
  This function is called to receive abort the call.

DEPENDENCIES
  None

RETURN VALUE
  None.

SIDE EFFECTS
  None.

===========================================================================*/
LOCAL void ds707_rmsmi_call_abort_cback
( 
  void 
)
{
/*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/
  MSG_MED("rmsmi call abort function called",0,0,0);
 
  ps_iface_down_ind(ds707_rmsmi_info.IS707pkt_iface_ptr);

}/* ds707_rmsmi_call_abort_cback() */

/*===========================================================================
FUNCTION DS707_RMSM_IS_IFACE_IN_USE

DESCRIPTION
  This function returns whether IS707 iface is already in a call or not ie 
  iface is being used by another entity or not.

DEPENDENCIES
  None

RETURN VALUE
  TRUE if in use, FALSE otherwise

SIDE EFFECTS
  None.

===========================================================================*/
boolean ds707_rmsm_is_iface_in_use
(
  void *this_if_ptr
)
{
  ps_iface_type *iface_ptr;
/*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/
  
  iface_ptr = (ps_iface_type*) this_if_ptr;
  
  if(iface_ptr != ds707_rmsmi_info.IS707pkt_iface_ptr)
  {
    ERR("Invalid iface 0x%x to ACL fn", iface_ptr, 0, 0);
    return FALSE;
  }

  if ( ( ds707_rmsmi_info.state == DS707_RMSM_NULL_STATE ) ||
       ( ds707_rmsmi_info.is_in_use == FALSE ) )
  {
    return FALSE;
  }

  return TRUE;

} /* ds707_rmsm_is_iface_in_use() */


/*===========================================================================
FUNCTION DS707_RMSM_RT_ACL_POST_PROC

DESCRIPTION
  This function performs the post processing for the ACL lookup for IS707 
  iface.  It sets the in use flag to true if the iface is in a call.  This 
  allows only one application (presumably MIPIO) to use this iface whne we are 
  in laptop call.  This check also works fine during  simple IP laptop call 
  since ACLs are anyway disabled and we don't even come here in that case.

DEPENDENCIES
  None

RETURN VALUE
  None.

SIDE EFFECTS
  None.

===========================================================================*/
void ds707_rmsm_rt_acl_post_proc
(
  acl_ipfilter_info_type *ipfilter_info_ptr,
  acl_policy_info_type *policy_info_ptr,
  void *this_if_ptr
)
{
  ps_iface_type *iface_ptr;
/*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/
  
  iface_ptr = (ps_iface_type*) this_if_ptr;

  if(iface_ptr != ds707_rmsmi_info.IS707pkt_iface_ptr)
  {
    ERR("Invalid iface 0x%x to ACL fn", iface_ptr, 0, 0);
    return;
  }
  
  if(ds707_rmsmi_info.state != DS707_RMSM_NULL_STATE)
  {
    ds707_rmsmi_info.is_in_use = TRUE;
  }
  
  return;

} /* ds707_rmsm_rt_acl_post_proc() */

/*===========================================================================
FUNCTION      DS707_PKT_IS_RM_IFACE_UP

DESCRIPTION   Returns TRUE if the RM iface is up.

DEPENDENCIES  phys_link should be up or coming up.

RETURN VALUE  See description.

SIDE EFFECTS  None.
===========================================================================*/
boolean ds707_rmsm_is_rm_iface_up
(
  void
)
{
  /*--------------------------------------------------------------------
    The checks for IFACE_UP is for sockets, for IFACE_ROUTEABLE is for
    network model laptop calls
  --------------------------------------------------------------------*/
  if( (ps_iface_state (&ds707_rmsmi_info.sio_707pkt_iface) == IFACE_ROUTEABLE) ||
      (ps_iface_state (&ds707_rmsmi_info.sio_707pkt_iface) == IFACE_UP) )
  {
    return(TRUE);
  }
  else
  {
    return(FALSE);
  }
} /* ds707_rmsm_is_rm_iface_up */

#endif /* FEATURE_DATA_IS707 */

