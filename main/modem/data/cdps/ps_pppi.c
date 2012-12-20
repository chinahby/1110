
/*====*====*====*====*====*====*====*====*====*====*====*====*====*====*====*

                              P S _ P P P I . C

GENERAL DESCRIPTION
  All of the internal PPP protocol suite functionality.

EXTERNALIZED FUNCTIONS

INITIALIZATION AND SEQUENCING REQUIREMENTS

 Copyright 1992-1994 William Allen Simpson
   Licensed to Qualcomm -- 95 Jan 21

 Copyright (c) 1995-2009 QUALCOMM Incorporated. 
 All Rights Reserved.
 Qualcomm Confidential and Proprietary
*====*====*====*====*====*====*====*====*====*====*====*====*====*====*====*/


/*===========================================================================

                        EDIT HISTORY FOR MODULE

  $PVCSPath: L:/src/asw/MM_DATA/vcs/ps_pppi.c_v   1.32   13 Feb 2003 20:13:04   jeffd  $
  $Header: //source/qcom/qct/modem/data/cdps/ps/main/lite/src/ps_pppi.c#6 $ $DateTime: 2011/04/12 15:12:44 $ $Author: mganjam $

when        who    what, where, why
--------    ---    ----------------------------------------------------------
04/29/09    sn     Ported support for call throttle feature (DCTM).
11/27/08    pp     Removed in-correct ASSERTs from pppi_hdlc_unframe_init.
06/24/08    pp     Fixed RVCT Compiler warnings.
02/22/08    pp     Klocwork fixes.
10/18/07    sn     Removed the 'cookie' parameter validation as 'dsm_item_type' 
                   structure no longer supports 'cookie' parameter.
11/01/06    an     Featurize ATCOP, RMSM, Async Fax
03/08/04    usb    Setting ps_iface ptr to Null only in PPP task context.
08/22/03    usb    Initialize LCP and IPCP fsm state before starting PPP.
06/11/03    usb    Fixed various bugs in HDLC sw and hw unframing.
                   Discard an unframed PPP packet if its less than 3 bytes.
06/10/03    ss     Changed the code to correctly fwd packets to laptop in a 
                   laptop call during MIP re-registrations, etc.
05/28/05    usb    Set rx_iface_ptr to NULL in pppi_abort to avoid 
                   inconsistency if somebody calls ppp_start followed
                   by ppp_stop without getting a DOWN event.
04/18/04    vsk    Initialize auth.mode to PPP_NO_AUTH at powerup 
                   removed call to fsm_passive_wakeup in pppi_incoming.
                   passive mode is handled in ppp_fsm when a cfg req comes in
04/18/03    vsk    Handle PPP_INTERNAL_SNOOP mode also in pppi_hdlc_sw_unframe
                   check for fsm.mode rather than fsm.passive_mode for passive
                   mode operation 
04/11/03    kvd    Moved logging of ppp frames to inside if( ppp_pkt_size >0)
                   in ppp_hdlc_hw_unframe as otherwise it was logging ERR
                   messages unnecessarily.
04/11/03    ak     Updates for new RMSM interfaces.
04/08/03    mvl    Added support for not moving to the network phase if auth
                   info not received.
04/07/03    usb    Split second half of pppi_incoming() (protocol 
                   processig part) into pppi_receive().
04/02/03    mvl    Complete modularization of the authentication code.
03/26/03    mvl    removed authentication featurization
03/17/03    js     Added the prefix PS_PPP_EVENT_ to ppp_event_state_type.
03/07/03    js     Added PPP events reporting. 
02/13/03    jd     Removed calls to ppp_log_frames() - no longer used
                   Fixed featurization and parameters to ppp_log_frames_ext
02/13/03    usb    Handling ret code SNOOP_CB_FORWARD_PKT from snoop cbacks.
02/06/03    mvl    Freeing packet when slhc_remember() fails and freeing meta
                   info in default TX function.
02/02/03    usb    Removed the use of variable cumulative_pkt_size from 
                   pppi_hdlc_hw_unframe() - using dsm_pullup_tail() instead 
                   to remove CRC from the end of the packet, processing new
                   enum return values from snoop_proto_msg_detect().
01/27/03    usb    PPP mode is always set in PS context, added 
                   PPP_SET_MODE_CMD, added hw framer cleanup to 
                   ppp_hdlc_unframe_init()
01/27/03    jd     Moved chap processing into process_chap_pkt()
12/10/02    usb    Moved setting PPP mode to NONE from ppp_abort to 
                   pppi_abort (PS context), checking if iface is NULL or not 
                   before processing an incoming PPP packet.
12/02/02    vr     Fixed potential HW framer buffer overflow. Removed setting
                   of unframe_mode (7E, SOP, etc.)when using hardware 
                   framer/deframer.
11/26/02    usb    Setting app field either to NULL or to a valid 
                   meta_info_ptr before calling tx_f_ptr().
11/25/02    js     Nullify pkt_tail_ptr and orig_tail_ptr whenever their
                   corresponding pkt_head_ptr and orig_head_ptr are released. 
11/19/02    jd     Fixed typos for FEATURE_DS_PPP_LOGGING
11/19/02    mvl    Fixed include for PPP logging.
11/14/02    jd     Fixed check for non-MIP packet forwarding during rereg
11/12/02    mvl    checking if setup timer exists before starting it.  Added
                   JCDMA support for auth failures.
11/08/02    usb    Reverted tx_callback() to pass meta info as a single 
                   pointer because storage of the outer pointer is on stack 
                   and hence cann't be passed through the context switch.
10/25/02    usb    Fixed tx_callback() to pass metainfo as double pointer 
                   rather than single pointer. This was causing illegal 
                   dereferences. Also added proper freeing up of memory 
                   during errors in pppi)outgoing.
10/11/02    usb    Modified HDLC hw feature to FEATURE_HDLC_HW_ACCEL
10/08/02    vr     Add 7E at the beginning of the PPP packet in hw framing
09/25/02    mvl    Modified signal usage.
09/24/02    usb    Added user_data to ppp tx and rx function callbacks
09/19/02    mvl    Removed feature super_esc as it is not useful anymore.
09/18/02    mvl    Added user_data to ppp event callbacks.  Setting mode in
                   start() and abort() calls, not internally.
09/12/02    mvl    Added the timer handling for the setup timeout function.
                   Removed netmodel featurization.
09/10/02    jd     Set tail_full to true when hw_unframe runs out of memory
                   so it will get a new dsm item rather than trying to insert
                   data in null ptr.  Also fixed unframe_mode in hw_unframe
                   to correspond to flow of ppp_asy_in so switching deframing
                   modes will not cause data aborts.
09/06/02  igt/mvl  added calls to funciton ppp_log_frames_ext() under 
                   FEATURE_DEBUG_PPP_LOG_EXT to support extended ppp logging
                   capabilities. 
09/06/02    mvl    fixed assert()s.
09/05/02    rwh    Lowered the low memory threshold in ppp_hdlc_hw_unframe()
                   from LESS_THAN_FEW to DONT_EXCEED for both pools.
09/02/02    mvl    Setting the PPP rx signal handler here, defined the RX
                   signal handler, and the PS Iface TX function.
08/30/02    dwp    Fixed test for making sure buffer passed to hdlc_read_buf
                   is 4-byte aligned. 0x3 ensures optimal packing.
08/30/02    vr     Fixed initialization bug in tail_full flag for the ifaces
                   and ensured that buffer passed to hdlc_read_buf will be
                   4-byte aligned
08/20/02    vr     Free existing chap_challenge_ptr if we get a new challenge
08/13/02    mvl    Added session handle support.
08/06/02  mvl/dwp  Updates to ppp_hdlc_hw_unframe() to pack DSM pkt chains.
08/06/02    mvl    Reworked authentication.
07/30/02    mvl    Put TASKLOCK() around invokation of event callbacks.
                   Resetting the FSM state to fsmINITIAL when PPP done.
                   Removed invalid HW framer macro.
07/29/02    mvl    Moved the IP reassembly code to pppi_abort() and also
                   setting the bridge_dev to invalid there. Fixed TASKLOCK()s
07/25/02    mvl    Moved input_params into ppp control block. Updates for PPP
                   renaming.
08/01/02    vr     Fixed chap_challenge_length exceecding DSM item size
07/30/02    igt    Removed calls to macros HDLC_FRAMER_CLEAR_ERR and 
                   HDLC_DEFRAMER_CLEAR_ERR.
07/24/02    mvl    Moved ppp mode check to the RX callback.  Variables now
                   called device rather than iface.
07/19/02    rc/dwp Fix typo in ppp_outgoing. outgoing_pkt_ptr to 
                   outgoing_head_ptr.
07/16/02    aku    Fixed bug in pppi_stop when SOCKETS is enabled (wrong
                   variable name used)
07/11/02    mvl    Updates for new PPP architecture.
07/01/02    mvl    using bridge device rather than SWAP_IFACE() macro.
05/22/02    mvl    Renamed file and moved all of the truely external code to
                   the new ppp.c file.
05/20/02    vr     Added support for hardware HDLC framer/deframer
05/03/02    mvl    File cleanup and added meta_info to ppp_outgoing()
03/22/02    ss/vr  Fixed ppp_asy_in() code to allocate multiple DSM items
                   when copying bytes of a new PPP frame.
03/18/02    vas    Added code to call registered callback func when CHAP
                   succeeds / fails on the HDR AN iface.
03/06/02    vsk    Fixed an invalid mem access problem in pppi_skipped()
03/01/02    vr     Added support for PPP passive mode in ppp_powerup_init().
03/01/02    vas    Modified to support authentication (PAP/CHAP) variables
                   per interface.
02/12/02    ss     Modified ppp_asy_in() to make allocation of DSM items
                   from the memory pools more intelligent.
02/12/02    vsk    Defined internal ppp_ip_pkt_hdlr_f_ptr_type to make it
                   build for all targets
02/11/02    ss     Modified pppi_outgoing() to make allocation of DSM items
                   from the memory pools more intelligent.
02/07/02    vsk    Added support to call IP pkt handler function if installed
                   to process IP packet instead of the default ip_route()
02/05/02    vr     Added code to not do IPCP if configured accordingly
01/24/02    mvl    Fixed an init problem for the case when ASSERT()s are
                   off.  Also did some cleanup.
01/17/02    vsk    Fixed a NULL ptr access in pppi_outgoing()
01/14/02    vsk    Fixed mem leak problem in pppi_outgoing()
12/04/01    vr     Initialize all interface to not require auth. by default
12/01/01    vas    Changed function prototypes to take the ppp iface as a
                   parameter instead of being hardcoded. Made ppp_do_auth
                   part of ppp ctl blk. lcp_chap_info maintained per iface.
11/26/01    vr     Check for laptop call before forwarding packets during MIP
                   re-registration.
11/21/01    ss     Added call to macro MIP_DATA_TRANSFER_ACTION when there is
                   some data transfer on Um.
11/17/01    aku    Changed ppp_initiate_resync() to enqueue a
                   PS_START_PPP_RESYNC_CMD to the PS task.
11/12/01    dwp    Add "|| FEATURE_DATA_PS" to whole module. Use FEATURE_DATA
                   for old/new DS Mgr dependencies. Use of the hdlc_enabled
                   flag in ppp_incoming/pppi_outgoing/ppp_powerup_init for ETSI
                   mode PPP (no HDLC over Uu). Make ppp_incoming external.
11/07/01    vr     added FEATURE_DS_MOBILE_IP wrapper
11/05/01    vr     During MIP re-reg, forward non-reg pkts to laptop
10/08/01    na     Fixed potential NULL pointer reference in ppp_asy_in().
09/20/01    na     CR18411: Fixed merge problem by removing unecessary
                   ASSERT in ppp_asy_in().
09/15/01    na     CR18328: Low throughput problem for network model calls
                   fixed in ppp_asy_in. Removed case for optimizing of
                   copy before 7E in FINDING_EOP mode.
09/14/01    jd     Added iface mode parameter to ppp_initiate_resync to
                   optionally allow Um interface to be put in internal mode
                   during resync.
09/10/01    na     CR18256, 18270 - ASSERT on tracer values in ppp_asy_in_init
                   was being triggered by incorrectly maintained tail_ptr.
09/07/01    na     ppp_asy_in() was asserting when it received a malformed
                   packet. Now it handles such a packet properly
09/03/01    aku    Putting iface in internal mode before in ppp_start() and
                   ppp_close()
08/31/01    mvl    Now checking if internal mode rather than if we are
                   resyncing to determine if packet needs processing after
                   beening snooped.
08/29/01    mvl    Removed ppp_init() as it was empty.
08/19/01    sjy    Added local PPP ctrl block ptr to pppi_outgoing(),
                   ppp_asy_in(), ppp_initiate(), and ppp_close() for when
                   netmodel feature is not defined.
                   Featurized ppp_initiate_resync() under
                   netmodel feature.
                   Featurized reference to iface_cb_array in
                   ppp_powerup_init() under netmodel feature.
08/12/01    sjy    Featurized reference to do_mip variable in PPP control
                   block
08/02/01    mvl    Removed the dependency on ppp_cb_ptr, renamed
                   ppp_skipped() to pppi_skipped() as it is internal, added
                   iface as an argument to pppi_skipped() and ppp_discard() ,
                   ppp_incoming() always takes iface as an argument, and
                   iface is now the first argument.
07/31/01    na     Added ppp_initiate_resync() function
07/30/01    mvl    Modified ppp_start() so that it actually works with
                   options that are passed in.  Added get_iface_opts().
07/27/01    mvl    Modified to use PSTimers, some code cleanup.
07/26/01    na     Fixed PPP_ASY_IN, so that the MS doesn't add an extra 0x7e
                   at the beginning of packets
07/24/01    mvl    Added initialization code so both interfaces are setup
                   correctly, ppp_initiate() and pp_close() take iface as an
                   arg and general code cleanup
07/23/01    pjb    Change location/prefix of command names. dsm_pushdown
                   return type changes. Change free count api.
                   dsm_offset_new_buffer now called to reserver header space.
06/21/01    mvl    Changed resync check in ppp_incoming() to be explicit
                   (checks PPP meta state machine state rather than checking
                   if in internal mode) - reinserted ASSERT()s Nish removed
                   (see below) but fixed them.
06/20/01    na     Removed 2 ASSERTs from ppp_asy_in_init() which were not
                   needed.
06/14/01    mvl    Updated a comment.
05/18/01    mvl    Changed OUT_IFACE() macro call to SWAP_IFACE().
02/27/01    ak     Removed pppi_outgoing from IRAM.
10/10/00    jd     Merged PPG r1.15 changes - account for dsm small item data
                   offset
08/25/00    rc     Changed dsm_pushdown() to take take pool id as fourth
                   parameter.Made changes to handle dsm_pushdown()
                   returning FALSE.
                   Changed dsm_new_buffer() calls to take pool id as input
                   parameter. So, if pool size is to be passed in as
                   parameter, a new macro DSM_DS_POOL_SIZE is used to
                   return pool id based on pool size. Added Macros that
                   check if there are enough items or if we are running out
                   of items.
                   Changed FEATURE_DS_MEM_CHK to FEATURE_DSM_MEM_CHK.
07/31/00    ak     Removed an extraneous #ifndef MSM5000_IRAM_FWD.
07/24/00    snn    Cancelled the timer if MS receives a NAK from IWF in PAP.
06/28/00    kjn    Added void argument to chap_proc and pap_proc.
06/20/00    ak     Added MSM5000_IRAM_FWD to ppp_incoming.
06/13/00    ak     Added #ifndefs for MSM5000_IRAM_FWD.
05/20/00    snn    Added support for the IP Reassembly.
05/04/00    mvl    changed interface to network model version of
                   pppi_outgoing() to fix bug where item was freed in this
                   function then freed again lower down.
04/24/00    snn    Updated a comment in PAP code for a FAQ.
03/10/00    snn    Added Support for CHAP. These have been added under the
                   DS_CHAP feature. Removed the USRX hack code.
01/25/00    ak     In ppp_asy_in, check to see if dsm_item ptr is null after
                   call to dsm_pull8.
11/16/99    rc     Added fix in pppi_outgoing() to dump packets if the
                   dsm item free count is less than the minimum required.
11/05/99    na     Fixed asserts in ppp_asy_in_init().
11/05/99    mvl    Fixed asserts in ppp_asy_in_init() so that only trip when
                   tail item is not properly freed.
11/04/99    hcg    Fixed incorrect packet chain insertion in ppp_asy_in().
10/28/99    mvl    Added code review changes
10/22/99    mvl    Added support for network model related statistics.
09/25/99    snn    Removed the parameter being passed to pap_proc(), because
                   it is not needed.
                   Added comments and fixed the commenting styles.
                   If Auth_Nak is recieved code will not terminate the link.
08/31/99    mvl    Fixed ppp_asy_in() so that we send items out in pseudo
                   network mode rather than buffering up an entire packet.
08/26/99    snn    Added code to terminate the PPP link if mobile recieves
                   an auth-nak from the base station.
08/25/99    mvl    Added ppp_asy_in_init() to reinitialize the ppp_asy_in()
                   variables to fix netmodel memory leak.  Also added tracer
                   values to help track down leak.
08/20/99    mvl    Changed MSG_MED()s in ppp_asy_in() to MSG_LOW()s
08/17/99    smp    Added include files msg.h & err.h.
08/10/99    snn    Added support for PAP timeouts and cleaned up comments.
08/09/99    ak     Added test points for sockets testing, where we can
                   simulate dropping PPP packets.
08/09/99    mvl    Added network model changes.
07/08/99    snn    Added support for DS_PAP feature.
04/08/99    na     Changed PPP_LCP_PROTOCOL in pppi_outgoing() to
                   DSM_LCP_PROTOCOL.
10/27/98    ldg    ARM porting changes: ROM to ROM const.
07/26/98    jjn    Use more efficient DSM.
04/14/98    na     Increased RAMP_MAX_SIZE to avoid potential memory blowup
                   problems. Added buffer based flow control.
01/27/98    ldg    Removed the DS_SLIM switch.
01/19/98    na     Sped up incoming and outgoing PPP processing, by removing
                   the overhead of dsm_pullup in each while loop up.
12/12/97    ldg    if_cb.InErrors now excludes checksum errors.
10/02/97    na     Added support for escaping all PPP outgoing bytes with
                   upper nibble set to F. Under FEATURE_DS_SUPER_ESC.
07/31/97    ldg    Added accessor function for UI to see some PPP vars.
07/30/97    na     Changed ACCM processing for a USRX test build. PPP doesn't
                   drop characters below 20h at all. Under DS_PAP feature.
07/22/97    na     Added PAP support - under DS_PAP feature.
06/17/97    fkm    FEATURE_xxx Updates (via Automatic Scripts)
06/12/97    ldg    Added new FEATURE_DS_PSTATS
06/05/97    na     Allowed a string of large items for incoming ppp data.
03/13/97    jjn    Included target.h and customer.h
02/19/97    na     UP fixes in ppp_asy_in.
02/13/97    na     Jim fixed a bug in ppp_asy_in that was causing memory
                   scribbles. CTIA show build.
01/23/97    na     Added a ramping procedure to guess whether next incoming
                   pkt. will need a small or a large item.
05/24/96    jjw    Gemini & ISS2 Data Services release.
11/09/95    jjw    IS-99 Compliant Release
 *  Dec 92  Bill_Simpson@um.cc.umich.edu
 *    added IPXCP.
 *  Jun 92  Bill_Simpson@um.cc.umich.edu
 *    revised buffering and tracing.
 *    added CHAP and LQR.
 *  Mar 92  Bill_Simpson@um.cc.umich.edu
 *    combined send, output, and raw using new send and queue.
 *  Jan 92  Bill_Simpson@um.cc.umich.edu
 *    Conforms to newest draft RFCs.
 *  Jul 91  Glenn McGregor & Bill Simpson
 *    Improve PAP user interface and fix related bugs.
 *    Remove pwaits and "phase machine".
 *  May 91  Bill Simpson & Glenn McGregor
 *    Update to newest LCP and IPCP draft RFCs.
 *    Add quick installation features.
 *    Add support for echo and discard message sending.
 *  Feb 91  Glenn McGregor      (ghm@merit.edu)
 *    Testing and suggestions.
 *  Jan 91  Bill_Simpson@um.cc.umich.edu
 *    Computer Systems Consulting Services
 *  09-90  -- Katie Stevens    (dkstevens@ucdavis.edu)
 *       UC Davis, Computing Services
 *  Based (in part) upon previous implementations by:
 *  1989  -- Drew Daniel Perkins    (ddp@andrew.cmu.edu)
 *       Carnegie Mellon University
===========================================================================*/


/*===========================================================================

                          INCLUDE FILES FOR MODULE

===========================================================================*/
#include "customer.h"
#include "comdef.h"

#ifdef FEATURE_DATA

#include "crc.h"
#include "dsm.h"
#include "ps_ppp_defs.h"
#include "ps_ppp_fsm.h"
#include "ps_pppi.h"
#include "ps_ppp_auth.h"
#include "ps_ppp_lcp.h"
#include "ps_ppp_ipcp.h"
#include "ps_ppp_task.h"
#include "ps_ppp_snoop.h"
#ifndef FEATURE_DATA_STRIP_ATCOP
#include "ps_ppp_netmodel.h"
#endif
#include "ps_meta_info.h"
#include "ps_ip.h"
#include "ps_svc.h"
#include "pstimer.h"
#include "dsbyte.h"
#include "assert.h"
#include "msg.h"
#include "err.h"

// this needs to be removed
#include "iface.h"
#include "ps_ipfrg.h"

#ifdef FEATURE_DS_MOBILE_IP
#ifndef FEATURE_DATA_STRIP_ATCOP
#include "ds707_rmsm.h"   // remove this dependency
#endif
#include "dsmip_regsm.h"
#include "dsmip_metasm.h"
#endif /* FEATURE_DS_MOBILE_IP */

#ifdef FEATURE_MSMHW_HUNT_SUPPORTED
#include "hdrdsm.h"                  /*       For hardware 7E detector     */
#endif /* FEATURE_MSMHW_HUNT_SUPPORTED */

#ifdef FEATURE_HDLC_HW_ACCEL
#include "hdlc_drv.h"
#endif /* FEATURE_HDLC_HW_ACCEL */

#ifdef FEATURE_DATA_PS_PPP_LOGGING
#include "ps_ppp_logging.h"
#endif /* FEATURE_DATA_PS_PPP_LOGGING */

#include "ps_ppp_events.h"

/*===========================================================================

                LOCAL DEFINITIONS AND DECLARATIONS FOR MODULE

===========================================================================*/
#define MODE_TOSS    0x01
#define MODE_ESCAPED 0x02

/*--------------------------------------------------------------------------
  If the size of the last ppp pkt < RAMP_MAX_SIZE then give a small item
  otherwise assign a large item.
---------------------------------------------------------------------------*/
#define RAMP_MAX_SIZE (DSM_DS_SMALL_ITEM_SIZ * 3)


/*---------------------------------------------------------------------------
  PPP Info structure - used for debugging - contains pointers to all of the
  PPP data structures
---------------------------------------------------------------------------*/
struct
{
  ppp_type *ppp_cb;
  lcp_value_type  *lcp_local_want_vals;
  lcp_value_type  *lcp_local_work_vals;
  lcp_value_type  *lcp_remote_want_vals;
  lcp_value_type  *lcp_remote_work_vals;
  ipcp_value_type *ipcp_local_want_vals;
  ipcp_value_type *ipcp_local_work_vals;
  ipcp_value_type *ipcp_remote_want_vals;
  ipcp_value_type *ipcp_remote_work_vals;
} ppp_info[PPP_MAX_DEV];

#ifdef FEATURE_DS_PSTATS
#error code not present
#endif /* FEATURE_DS_PSTATS */

/*---------------------------------------------------------------------------
  Pointer variable that holds the head of the current in-process PPP packet.
---------------------------------------------------------------------------*/
LOCAL dsm_item_type *ppp_pkt_head_ptr = NULL;

LOCAL struct iface if_cb;

#define pppfcs(fcs, c) ((fcs >> 8) ^ crc_16_l_table[(fcs ^ c) & 0x00ff])

#define SP_CHAR     0x20

#ifdef FEATURE_HDR_AN_AUTH
#error code not present
#endif /* FEATURE_HDR_AN_AUTH */


/*---------------------------------------------------------------------------
  Externs to support the ppp_info type
---------------------------------------------------------------------------*/
extern lcp_value_type lcp_local_want_cb[PPP_MAX_DEV];
extern lcp_value_type lcp_local_work_cb[PPP_MAX_DEV];
extern lcp_value_type lcp_remote_want_cb[PPP_MAX_DEV];
extern lcp_value_type lcp_remote_work_cb[PPP_MAX_DEV];
extern ipcp_value_type ipcp_local_want_cb[PPP_MAX_DEV];
extern ipcp_value_type ipcp_local_work_cb[PPP_MAX_DEV];
extern ipcp_value_type ipcp_remote_want_cb[PPP_MAX_DEV];
extern ipcp_value_type ipcp_remote_work_cb[PPP_MAX_DEV];

/*---------------------------------------------------------------------------
  Array that maps the PPP devices to the PPP signals for each instance

  NOTE: It is CRITICAL that the signals here are in the order of the ppp
        devices that they correspond to.
---------------------------------------------------------------------------*/
const ppp_sig_type ppp_dev_to_default_sig_array[PPP_MAX_DEV] =
{
  PPP_UM_1_RX_SIGNAL,
  PPP_RM_RX_SIGNAL,
#ifdef FEATURE_HDR_AN_AUTH
#error code not present
#endif /* FEATURE_HDR_AN_AUTH */
};


/*===========================================================================
                            FORWARD DECLARATIONS
===========================================================================*/
static void pppi_skipped
(
  ppp_dev_enum_type iface,
  dsm_item_type **item_head_ptr
);

static boolean pppi_send_prot_rej
(
 ppp_dev_enum_type iface,           /* iface (Rm or Um) pkt arrived on   */
 uint16 protocol,                   /* Bad protocol for which to PROT_REJ*/
 dsm_item_type *item_ptr            /* Pointer to the packet             */
);

boolean pppi_get_data_cback
(
  ps_sig_enum_type sig,                    /* Signal to be processed       */
  void *dev_instance                       /* user data, indicates the ppp */
);

void pppi_i_setup_timout_cb
(
  void *arg_ptr
);


/*===========================================================================

                        PUBLIC FUNCTION DECLARATIONS

===========================================================================*/
/*===========================================================================
FUNCTION PPPI_POWERUP_INIT()

DESCRIPTION
  This function does the power up initialization for PPP

RETURN VALUE
  None

DEPENDENCIES
  None

SIDE EFFECTS
  None
===========================================================================*/
void pppi_powerup_init(void)
{
  int init_check;
  uint8 loop;
  /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  for(loop = PPP_MIN_DEV; loop < PPP_MAX_DEV; loop += 1)
  {
    /*----------------------------------------------------------------------
      memset() the ppp control blocks
    ----------------------------------------------------------------------*/
    memset(ppp_cb_array + loop, 0, sizeof(ppp_type));

    /*-----------------------------------------------------------------------
      Initialize LCP and IPCP
    -----------------------------------------------------------------------*/
    init_check = lcp_powerup_init((ppp_dev_enum_type)loop);
    ASSERT(init_check == 0);
    init_check = ipcp_powerup_init((ppp_dev_enum_type)loop);
    ASSERT(init_check == 0);

    /*-----------------------------------------------------------------------
      Allocate a timer for total setup time
    -----------------------------------------------------------------------*/
    ppp_cb_array[loop].setup_timer = ps_timer_alloc(pppi_i_setup_timout_cb,
                                                    (void*)loop);
    ASSERT(ppp_cb_array[loop].setup_timer != PS_TIMER_FAILURE);

    /*-----------------------------------------------------------------------
      set the PPP phase to dead and assign the default r/tx functions.
    -----------------------------------------------------------------------*/
    ppp_cb_array[loop].phase        = pppDEAD;
    ppp_cb_array[loop].rx_f_ptr     = pppi_default_rx;
    ppp_cb_array[loop].tx_f_ptr     = pppi_default_tx;
    ppp_cb_array[loop].bridge_dev   = PPP_INVALID_DEV;
    ppp_cb_array[loop].mode         = PPP_NONE_MODE;
    ppp_cb_array[loop].hdlc_mode    = HDLC_INVALID_MODE;
    ppp_cb_array[loop].auth.mode    = PPP_NO_AUTH;
    ppp_cb_array[loop].input_params.tail_full = TRUE;

    /*-----------------------------------------------------------------------
      Initialize the ppp_info data structure
    -----------------------------------------------------------------------*/
    ppp_info[loop].ppp_cb = &(ppp_cb_array[loop]);
    ppp_info[loop].lcp_local_want_vals   = &(lcp_local_want_cb[loop]);
    ppp_info[loop].lcp_local_work_vals   = &(lcp_local_work_cb[loop]);
    ppp_info[loop].lcp_remote_want_vals  = &(lcp_remote_want_cb[loop]);
    ppp_info[loop].lcp_remote_work_vals  = &(lcp_remote_work_cb[loop]);
    ppp_info[loop].ipcp_local_want_vals  = &(ipcp_local_want_cb[loop]);
    ppp_info[loop].ipcp_local_work_vals  = &(ipcp_local_work_cb[loop]);
    ppp_info[loop].ipcp_remote_want_vals = &(ipcp_remote_want_cb[loop]);
    ppp_info[loop].ipcp_remote_work_vals = &(ipcp_remote_work_cb[loop]);

  } /* for(all PPP devices) */

  /*-------------------------------------------------------------------------
    Reset the Hardware framer if it is available
  -------------------------------------------------------------------------*/
#ifdef FEATURE_HDLC_HW_ACCEL
  hdlc_deframer_cmd(HDLC_DEFRAMER_RESET);  
#endif /* FEATURE_HDLC_HW_ACCEL */

} /* pppi_powerup_init() */



/*=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=

                              CONTROL FUNCTIONS

=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=*/
/*===========================================================================
FUNCTION PPPI_START()

DESCRIPTION
  This function will actively start PPP (though potentially in passive
  mode).

PARAMETERS
  start_info: parameters to use with start

RETURN VALUE
  None

DEPENDENCIES
  None

SIDE EFFECTS
  None
===========================================================================*/
void pppi_start
(
  pppi_start_info_type start_info
)
{
  ppp_type *ppp_cb_ptr;
  boolean   start = TRUE;

  /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/
  if(start_info.device >= PPP_MAX_DEV)
  {
    ERR_FATAL("Invalid device %d!", start_info.device, 0, 0);
  }
  ppp_cb_ptr = ppp_cb_array + start_info.device;

  TASKLOCK();
  if(ppp_cb_ptr->session_handle == 0)
  {
    MSG_MED("starti() for free dev %d", start_info.device, 0, 0);
    start = FALSE;
  }

  /*-------------------------------------------------------------------------
    Everything looks good - so fire things up
  -------------------------------------------------------------------------*/
  else
  {
    /*-----------------------------------------------------------------------
      Make the pending information active and change to internal mode
    -----------------------------------------------------------------------*/
    ppp_cb_ptr->mode = PPP_INTERNAL_MODE;
    ppp_cb_ptr->tx_f_ptr = ppp_cb_ptr->pend_tx_f_ptr;
    ppp_cb_ptr->rx_f_ptr = ppp_cb_ptr->pend_rx_f_ptr;
    ppp_cb_ptr->tx_user_data = ppp_cb_ptr->pend_tx_user_data;
    ppp_cb_ptr->rx_user_data = ppp_cb_ptr->pend_rx_user_data;

    /*-----------------------------------------------------------------------
      Initialize and open the LCP state machine but only initialize IPCP if 
      it is not disabled  
    -----------------------------------------------------------------------*/
    ppp_cb_ptr->fsm[LCP].state = fsmINITIAL;
    fsm_open(&(ppp_cb_ptr->fsm[LCP]));
    
    if(ppp_cb_ptr->fsm[IPCP].mode != PPP_CP_DISABLED)
    {
      ppp_cb_ptr->fsm[IPCP].state = fsmINITIAL;
      fsm_open(ppp_cb_ptr->fsm + IPCP);
    }
    /*-----------------------------------------------------------------------
      register the ppp_task_get_data_cback() function to handle of the PPP RX
      signal callbacks.
    -----------------------------------------------------------------------*/
    PPP_SET_SIG_HANDLER(ppp_cb_ptr->rx_signal,
                        pppi_get_data_cback,
                        (void*)(start_info.device));
    PPP_ENABLE_SIG(ppp_cb_ptr->rx_signal);
  }
  TASKFREE();

  /*-------------------------------------------------------------------------
    Start the LCP state machine if it is active (i.e. not pasive in mode)
  -------------------------------------------------------------------------*/
  if(start == TRUE &&
     ppp_cb_ptr->fsm[LCP].mode == PPP_CP_ACTIVE)
  {
    fsm_up(ppp_cb_ptr->fsm + LCP);

    /*-----------------------------------------------------------------------
      Only start the setup timer if the setup time is non-zero 
    -----------------------------------------------------------------------*/
    if(ppp_cb_ptr->setup_timer != PS_TIMER_FAILURE &&
       ppp_cb_ptr->setup_timeout > 0)
    {
      MSG_MED("Starting setup timer for %dms",
                ppp_cb_ptr->setup_timeout,
                0,
                0);
      ps_timer_start(ppp_cb_ptr->setup_timer, ppp_cb_ptr->setup_timeout);
    }
  } /* if(starting) */

} /* pppi_start() */



/*===========================================================================
FUNCTION PPPI_STOP()

DESCRIPTION
  This function will initiate the closing of a PPP session.

DEPENDENCIES
  None

RETURN VALUE
  None

SIDE EFFECTS
  None
===========================================================================*/
void pppi_stop
(
  ppp_dev_enum_type device       /* The interface (Rm or Um)            */
)
{
  ppp_type *ppp_cb_ptr;
  boolean   stop = TRUE;
  /* - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - */
  if(device >= PPP_MAX_DEV)
  {
    ERR_FATAL("Invalid device %d!", device, 0, 0);
  }

  ppp_cb_ptr = ppp_cb_array + device;

  TASKLOCK();
  /*-------------------------------------------------------------------------
    If the device is invalid then don't do anything
  -------------------------------------------------------------------------*/
  if(ppp_cb_ptr->session_handle == 0)
  {
    MSG_MED("stopi() for free dev %d", device, 0, 0);
    stop = FALSE;
  }
  else
  {
    /*-----------------------------------------------------------------------
      Put device in internal mode before initiating ppp closure.
    -----------------------------------------------------------------------*/
    ppp_cb_ptr->mode = PPP_INTERNAL_MODE;
  }
  TASKFREE();

  /*-------------------------------------------------------------------------
    If we are to stop - close the LCP FSM
  -------------------------------------------------------------------------*/
  if(stop == TRUE)
  {
    fsm_close(&ppp_cb_ptr->fsm[LCP]);
  }

} /* pppi_stop() */




/*===========================================================================
FUNCTION PPPI_ABORT()

DESCRIPTION
  This function will clean up after PPP is finished.

DEPENDENCIES
  It should only ever be called from the command (i.e. via task command
  interface) and not directly by any other piece of code).

RETURN VALUE
  None

SIDE EFFECTS
  None
===========================================================================*/
void pppi_abort
(
  ppp_dev_enum_type device       /* The interface (Rm or Um)            */
)
{
  ppp_type               *ppp_cb_ptr;
#ifdef FEATURE_DS_SOCKETS
  struct ip_reasm_type   *rp;
  int                    loop;
#endif /* FEATURE_DS_SOCKETS */

  ppp_protocol_e_type    protocol = PPP_PROTOCOL_MIN;
  ppp_event_payload_type ppp_event_payload;

  /* - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - */
  if(device >= PPP_MAX_DEV)
  {
    ERR_FATAL("Invalid device %d!", device, 0, 0);
  }

  ppp_cb_ptr = ppp_cb_array + device;

  if(ppp_cb_ptr->phase == pppESTABLISH)
  {
    protocol = PPP_PROTOCOL_LCP;
  }
  else if(ppp_cb_ptr->phase == pppNETWORK)
  {
    protocol = PPP_PROTOCOL_IPCP;
  }

  MSG_MED("PPP Phase %d Protocol %d", ppp_cb_ptr->phase, protocol, 0 );

  /*-------------------------------------------------------------------------
    Disable the RX signal, make sure all of the timers are cancelled - and
    reinitialize the sw deframer
  -------------------------------------------------------------------------*/
  PPP_DISABLE_SIG(ppp_cb_ptr->rx_signal);
  ps_timer_cancel(ppp_cb_ptr->fsm[LCP].timer);
  ps_timer_cancel(ppp_cb_ptr->fsm[IPCP].timer);
  ps_timer_cancel(ppp_cb_ptr->setup_timer);
  pppi_hdlc_unframe_init(device);
  ppp_cb_ptr->rx_iface_ptr = NULL;
  ppp_cb_ptr->phase = pppDEAD;
  ppp_cb_ptr->mode = PPP_NONE_MODE;
  ppp_cb_ptr->hdlc_mode = HDLC_INVALID_MODE;
  ppp_cb_ptr->bridge_dev = PPP_INVALID_DEV;
  ppp_cb_ptr->fsm[LCP].state  = fsmINITIAL;
  ppp_cb_ptr->fsm[IPCP].state = fsmINITIAL;

  TASKLOCK();
  if(ppp_cb_ptr->event_info[PPP_DOWN_EV].cback != NULL)
  {
    ppp_cb_ptr->event_info[PPP_DOWN_EV].cback(
      device,
      protocol,
      PPP_DOWN_EV,
      ppp_cb_ptr->event_info[PPP_DOWN_EV].user_data,
      ppp_cb_ptr->session_handle,
      ppp_cb_ptr->fsm[protocol].fail_reason);
  }
  TASKFREE();

  /*-------------------------------------------------------------------------
    This ifdef is not needed for the final build
  -------------------------------------------------------------------------*/
#ifdef FEATURE_DS_SOCKETS
  /*-------------------------------------------------------------------------
    Free the IP reassembly data structures after every call.

    // MVL: this should prbably not be here - but in the power up stuff for
    // the stack?
  -------------------------------------------------------------------------*/
  for (loop = 0; loop < NUM_DATA_FRAG; loop++)
  {
    rp = ((struct ip_reasm_type*)(ip_reasm_array + loop));
    ip_free_reasm(rp);
  }
#endif /* FEATURE_DS_SOCKETS */

  ppp_cb_ptr->fsm[protocol].fail_reason = PPP_FAIL_REASON_NONE;

  /*-------------------------------------------------------------------------
    Report PS_PPP_EVENT_CLOSED event for both IPCP and LCP
  -------------------------------------------------------------------------*/
  ppp_event_payload.ppp_event_device = device;
  ppp_event_payload.ppp_event_protocol = PPP_EV_LCP;
  ppp_event_payload.ppp_event_prev_state = PS_PPP_EVENT_UNKNOWN;
  ppp_event_payload.ppp_event_state = PS_PPP_EVENT_CLOSED;
  event_report_payload(EVENT_PPP, 
                       sizeof (ppp_event_payload),
                       &ppp_event_payload);

  ppp_event_payload.ppp_event_protocol = PPP_EV_IPCP;
  event_report_payload(EVENT_PPP, 
                       sizeof (ppp_event_payload),
                       &ppp_event_payload);


} /* pppi_abort() */



/*===========================================================================
FUNCTION PPPI_SET_MODE()

DESCRIPTION
  This function will change the PPP mode as specified. It assumes that the
  mode being set is correct and validation on the mode has been performed
  before posting the cmd to call this function.
  
PARAMETERS
  mode_info: parameters passed through the cmd required to set mode

RETURN VALUE
  None

DEPENDENCIES
  None

SIDE EFFECTS
  None
===========================================================================*/
void pppi_set_mode
(
  pppi_mode_info_type mode_info
)
{
  ppp_type *ppp_cb_ptr;

  /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/
  if(mode_info.device >= PPP_MAX_DEV)
  {
    ERR("Invalid device %d!", mode_info.device, 0, 0);
    return;
  }
  ppp_cb_ptr = ppp_cb_array + mode_info.device;

  ppp_cb_ptr->mode = (ppp_mode_enum_type)mode_info.mode;
}



/*===========================================================================
                                OUTBOUND DATA
===========================================================================*/
/*===========================================================================
FUNCTION PPPI_IFACE_TX_FUNCTION()

DESCRIPTION
  This function is registered with the associated PS Iface when a PPP
  instance is up (this is done in ipcp_openeing()).  It is a ps_iface tx
  command. 

PARAMETERS
  this_iface_ptr: pointer to the interface which is doing the TX
  pkt_chain_ptr: reference to a dsm item pointer: this is payload for PPP
  meta_info_ref_ptr: reference to a meta info item.
  tx_cmd_info: in the case of PPP this should point to the PPP instance this
               should be transmitted on.

RETURN VALUE
  Same as pppi_outgoing
   0 on success
  -1 on failure

DEPENDENCIES
  None

SIDE EFFECTS
  None
===========================================================================*/
int pppi_iface_tx_function
(
  ps_iface_type      *this_iface_ptr,
  dsm_item_type     **item_ref_ptr,
  ps_meta_info_type **meta_info_ref_ptr,
  void               *tx_cmd_info
)
{
  int device;
  ps_iface_type *in_iface_ptr;
/*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/
  ASSERT(this_iface_ptr != NULL);

  /*-------------------------------------------------------------------------
    Get the device instance from the tx_cmd_info, and make sure that the
    interface that is calling this function is the same as the one the passed
    in device is associated with.
  -------------------------------------------------------------------------*/
  device = (int)tx_cmd_info;
  in_iface_ptr = ppp_cb_array[device].rx_iface_ptr;
  
  if(in_iface_ptr == NULL || ppp_cb_array[device].mode == PPP_NONE_MODE)
  {              
    MSG_HIGH("PPP %d not configured discading pkt", device, 0, 0);
    dsm_free_packet(item_ref_ptr);
    PS_META_INFO_FREE(meta_info_ref_ptr);
    return -1;
  }

  ASSERT(this_iface_ptr == in_iface_ptr);

  return pppi_outgoing((ppp_dev_enum_type)device, 0, item_ref_ptr, meta_info_ref_ptr);

} /* pppi_iface_tx_function() */


/*===========================================================================
FUNCTION PPPI_OUTGOING()

DESCRIPTION
  This function will process the passed datagram into a PPP packet for TX.

  Input Conditions:

  1. device specifies the device( Rm or Um) where the PPP pkt. is to be sent.

  2. The 'kind' field of the incoming_pkt_ptr specifies the protocol
     of this packet. This field can be:
     DSM_PS_IP_COMPRESS: For IP datagrams. This will cause the packets
     to go through the VJ compressor.
     PPP_IP_PROTOCOL: For IP datagrams. IP packets will not go through
     the VJ compressor.
     PPP_LCP_PROTOCOL: For LCP proto.
     PPP_IPCP_PROTOCOL: For IPCP
     PPP_PAP_PROTOCOL: For PAP.
     PPP_VJCH_PROTOCOL: For VJ Compressed pkts.
     PPP_VJUCH_PROTOCOL: For VJ Uncompressed pkts.

     If it is not a known field, then the protocol parameter is used.

  3. For all packets the address and control
     fields(FF03) and the protocol fields should be stripped.

   Output Conditions:
     This function takes care to free the passed packet chain even if there
     is a error. If there is no error, it passes the packet chain to the
     lower layer.

     Depending on the hdlc_mode for the interface, it would either use the
     hardware HDLC framer or do the framing in software.

DEPENDENCIES
  None

RETURN VALUE
  -1 on error.
   0 on success.

SIDE EFFECTS
  None
===========================================================================*/
int pppi_outgoing
(
  ppp_dev_enum_type     device,                 /* device to be sent on    */
  uint16                protocol,               /* protocol of packet      */
  dsm_item_type       **incoming_pkt_ptr,       /* ptr to PPP packet chain */
  ps_meta_info_type   **meta_info_ref_ptr       /* ptr to meta info        */
)
{
  ppp_type       *ppp_cb_ptr = NULL;            /* PPP ctrl block ptr      */
  dsm_item_type  *tx_item_ptr = NULL;
  register uint8 *cp;
  int             len      = PPP_HDR_LEN;
  uint32          accm     = LCP_ACCM_DEFAULT;
  negotiate_t     negotiated;           /* negotiated remote device opts  */
  boolean         doing_lcp;            /* Indication that LCP is happening*/
  boolean         no_acfc = TRUE;       /* no address & control field cmpr */
  boolean         no_pfc;               /* no protocol field compression   */
  /* - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - */

  if(device >= PPP_MAX_DEV ||
     (*incoming_pkt_ptr)->used > (*incoming_pkt_ptr)->size)
  {
    ASSERT(0);
    dsm_free_packet(incoming_pkt_ptr);
    PS_META_INFO_FREE(meta_info_ref_ptr);
    return -1;
  }

  ppp_cb_ptr = ppp_cb_array + device;

  if(ppp_cb_ptr->rx_iface_ptr == NULL || ppp_cb_ptr->mode == PPP_NONE_MODE)
  {
    MSG_HIGH("PPP %d not configured discading pkt", device, 0, 0);
    dsm_free_packet(incoming_pkt_ptr);
    PS_META_INFO_FREE(meta_info_ref_ptr);
    return -1;    
  }

  /*-------------------------------------------------------------------------
    Assign interface specific variables.
  -------------------------------------------------------------------------*/
  negotiated  = ppp_cb_ptr->fsm[Lcp].remote.work;

  /*-------------------------------------------------------------------------
    Only increment this counter when we are in internal mode - if we are in
    some other mode this packet will be counted elsewhere
  -------------------------------------------------------------------------*/
  if(ppp_cb_ptr->mode == PPP_INTERNAL_MODE)
  {
    if_cb.OutUnicast++;
  }

  switch((*incoming_pkt_ptr)->kind)
  {
    case DSM_PS_IP_COMPRESS:
      /*---------------------------------------------------------------------
        Got compressed packet
      ---------------------------------------------------------------------*/
      if(ppp_cb_ptr->fsm[IPcp].state != fsmOPENED)
      {
        MSG_HIGH( "dev %d Not Open for IP Traffic", device, 0, 0);
        if_cb.OutDiscards++;
        dsm_free_packet(incoming_pkt_ptr);
        PS_META_INFO_FREE(meta_info_ref_ptr);
        return -1;
      }
      protocol = PPP_IP_PROTOCOL;

      if(ppp_cb_ptr->fsm[IPcp].remote.work & IPCP_N_COMPRESS)
      {
        register ipcp_pdv_type *ipcp_p =
          (ipcp_pdv_type*)ppp_cb_ptr->fsm[IPcp].pdv;
        register ipcp_value_type *vp   =
          (ipcp_value_type*)ppp_cb_ptr->fsm[IPcp].remote.work_pdv;

        /*-------------------------------------------------------------------
          Attempt TCP/IP header compression
        --------------------------------------------------------------------*/
        switch(slhc_compress(ipcp_p->slhcp,
                             incoming_pkt_ptr,
                             vp->slot_compress))
        {
          case SL_TYPE_IP: /* IP packet */
            protocol = PPP_IP_PROTOCOL;
            break;

          case SL_TYPE_COMPRESSED_TCP: /* compressed packet */
            protocol = PPP_VJCH_PROTOCOL;
            break;

          case SL_TYPE_UNCOMPRESSED_TCP: /* incompressed packet */
            protocol = PPP_VJUCH_PROTOCOL;
            break;

          default:
            /*---------------------------------------------------------------
              slhc_compress releases the incoming_pkt_ptr in case of an error
              Update counters and return here.
            ---------------------------------------------------------------*/
            MSG_HIGH( "Bad IP Packet", 0, 0, 0);
            if_cb.OutErrors++;
            dsm_free_packet(incoming_pkt_ptr);
            PS_META_INFO_FREE(meta_info_ref_ptr);
            return -1;
        }
      }
      break;

    case DSM_PS_IPCP_PROTOCOL:
      /*---------------------------------------------------------------------
        Got IPCP packet
      ---------------------------------------------------------------------*/
      protocol = PPP_IPCP_PROTOCOL;
      break;

    case DSM_PS_LCP_PROTOCOL:
      /*---------------------------------------------------------------------
        Got LCP packet
      ---------------------------------------------------------------------*/
      protocol = PPP_LCP_PROTOCOL;
      break;

    case DSM_PS_PAP_PROTOCOL:
      /*---------------------------------------------------------------------
        Got PAP packet
      ---------------------------------------------------------------------*/
      protocol = PPP_PAP_PROTOCOL;
      break;

    case DSM_PS_CHAP_PROTOCOL:
      /*---------------------------------------------------------------------
        Got CHAP packet
      ---------------------------------------------------------------------*/
      protocol = PPP_CHAP_PROTOCOL;
      break;

    default:
      MSG_MED("Got packet with kind: %d", (*incoming_pkt_ptr)->kind,0,0);
      break;
  } /* switch( protocol) */

  /*-------------------------------------------------------------------------
    If ACCM was negotiated, store the new value.
  -------------------------------------------------------------------------*/
  if(( doing_lcp = (protocol == PPP_LCP_PROTOCOL)) == 0 &&
     negotiated & LCP_N_ACCM)
  {
    accm = ((lcp_value_type*)ppp_cb_ptr->fsm[Lcp].remote.work_pdv)->accm;
  }

  /*-------------------------------------------------------------------------
    Compress protocol field if allowed.  Can only compress protocols whose
    MSB is 0, thus their value must be less than 255, hense the >= 0x00ff
  -------------------------------------------------------------------------*/
  if(( no_pfc = ( doing_lcp ||  !(negotiated & LCP_N_PFC) ||
                  protocol >= 0x00ff)) == FALSE)
  {
    --len;
  }

  /*-------------------------------------------------------------------------
    Discard HDLC address and control fields if possible - these fields are
    one byte each, hense the 2 below

    If hdlc_mode == NONE, no HDLC framing is done on this pkt.
  -------------------------------------------------------------------------*/
  if( (ppp_cb_ptr->hdlc_mode == HDLC_OFF) ||
      ((no_acfc = ( doing_lcp ||  !(negotiated & LCP_N_ACFC))) == FALSE) )
  {
    len -= 2;
  }

  /*-------------------------------------------------------------------------
    Load header with proper values
  -------------------------------------------------------------------------*/
  if (dsm_pushdown( incoming_pkt_ptr, NULL, len,
                                    DSM_DS_SMALL_ITEM_POOL) < len)
  {
     ERR(" Running out of memory: dumping PPP frames", 0, 0, 0);
     dsm_free_packet(incoming_pkt_ptr);
     PS_META_INFO_FREE(meta_info_ref_ptr);
     return -1;
  }
  cp = (*incoming_pkt_ptr)->data_ptr;

  /*-------------------------------------------------------------------------
    Do we do full address and control fields?
    If hdlc_mode == HW or SW, add HDLC framing to pkt.
  -------------------------------------------------------------------------*/
  if(ppp_cb_ptr->hdlc_mode != HDLC_OFF &&
     no_acfc)
  {
    *cp++ = HDLC_ALL_ADDR;
    *cp++ = HDLC_UI;
  }

  /*-------------------------------------------------------------------------
    Do we do put in full protocol field.  If so place MSB in packet,
    otherwise only put LSB in packet.
  -------------------------------------------------------------------------*/
  if(no_pfc)
  {
    *cp++ = (protocol >> 8);
  }
  *cp++ = (protocol & 0x00ff);

#ifdef FEATURE_DS_MOBILE_IP
  /*-------------------------------------------------------------------------
    Let MIP code know that data is being transferred over Um interface
  -------------------------------------------------------------------------*/
  if ( device == PPP_UM_SN_DEV )
  {
    MIP_DATA_TRANSFER_ACTION;
  }
#endif

  /*-------------------------------------------------------------------------
    Call the appropriate framing function and send it out
  -------------------------------------------------------------------------*/
  switch(ppp_cb_ptr->hdlc_mode)
  {
#ifdef FEATURE_HDLC_HW_ACCEL
  case HDLC_HW:
     tx_item_ptr = pppi_hdlc_hw_frame(device,
                                      accm,
                                      incoming_pkt_ptr);
    break;
#endif /* FEATURE_HDLC_HW_ACCEL */

  case HDLC_SW:
    tx_item_ptr = pppi_hdlc_sw_frame(device,
                                     accm,
                                     incoming_pkt_ptr,
                                     doing_lcp);
    break;

  case HDLC_OFF:
    tx_item_ptr = *incoming_pkt_ptr;
    break;

  default:
    ASSERT(0);
  } /* switch(ppp_cb_ptr->hdlc_mode) */

  /*-------------------------------------------------------------------------
    point to the meta_info in the pkt app_field, send the packet and return
    success.
  -------------------------------------------------------------------------*/
  if(tx_item_ptr == NULL)
  {
    PS_META_INFO_FREE(meta_info_ref_ptr);
    return -1;
  }
  else
  {
    if (meta_info_ref_ptr != NULL)
    {
      tx_item_ptr->app_field = (uint32) (*meta_info_ref_ptr);
    }
    else
    {
      tx_item_ptr->app_field = NULL;
    }

    ppp_cb_ptr->tx_f_ptr(&tx_item_ptr, ppp_cb_ptr->tx_user_data);
    return 0;
  }
} /* pppi_outgoing() */



/*===========================================================================
FUNCTION PPPI_HDLC_SW_FRAME()

DESCRIPTION
  This function will frame the PPP packet based on the specified accm,
  append the CRC and trailing 7E flag and send it on the specified interface.
  It will do the entire process in software.

  The input packet pointer should be the complete PPP packet to be framed
  including HDLC header and PPP protocol field (if we desire to send them),
  but without the CRC.

DEPENDENCIES
  This function should only be called from pppi_outgoing.

RETURN VALUE
  NULL on error
  ptr to framed packet on sucess

SIDE EFFECTS
  None
===========================================================================*/
dsm_item_type *pppi_hdlc_sw_frame
(
  ppp_dev_enum_type     device,             /* outgoing device             */
  uint32                accm,               /* accm to use for this device */
  dsm_item_type       **incoming_pkt_ptr,   /* pkt to send                 */
  boolean               doing_lcp           /* LCP happening?              */
)
{
  ppp_type       *ppp_cb_ptr = NULL;            /* PPP ctrl block ptr      */
  dsm_item_type  *outgoing_head_ptr = NULL;     /* first buffer            */
  dsm_item_type  *outgoing_tail_ptr = NULL;     /* current buffer          */
  dsm_item_type  *test_item_ptr;
  register uint8 *cp;
  uint8          *incoming_data_ptr;            /* Ptr to incoming data    */
  int             c7bits;
  register int    c;
  uint16          calc_fcs = HDLC_FCS_START;
  uint16          cnt, size, octets, i;
  dsm_mempool_id_enum_type pool_id;
  boolean         mem_avail = TRUE;
/* - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - */

  if(device >= PPP_MAX_DEV                                 ||
     (*incoming_pkt_ptr)->used > (*incoming_pkt_ptr)->size ||
     (*incoming_pkt_ptr)->used == 0)
  {
    ASSERT(0);
    return NULL;
  }

#ifdef FEATURE_DATA_PS_PPP_LOGGING
  ppp_log_frames_ext (*incoming_pkt_ptr, PPP_LOG_UNFRAMED_TX, device);
#endif /* FEATURE_DATA_PS_PPP_LOGGING */

  /*-------------------------------------------------------------------------
    Assign interface specific variables.
  -------------------------------------------------------------------------*/
  ppp_cb_ptr = &(ppp_cb_array[device]);
  ASSERT(ppp_cb_ptr->hdlc_mode != HDLC_OFF);

  /*-------------------------------------------------------------------------
    Count FCS and ending Flag bytes then build a series of buffers for the
    raw packet.  2 bytes for FCS + 1 byte for flag = 3 bytes

    If hdlc_mode == HW or SW, update accounting statistics.
  -------------------------------------------------------------------------*/
  octets = dsm_length_packet(*incoming_pkt_ptr) + 3;

  /*-------------------------------------------------------------------------
    Determine which pool to use to store the outgoing packet. If the
    determined pool is running low on memory, use the other pool.
  -------------------------------------------------------------------------*/
  pool_id = DSM_DS_POOL_SIZE(octets);

  if (DSM_DONT_EXCEED_ITEMS(pool_id))
  {
    pool_id = DSM_DS_OTHER_POOL_ID(pool_id);
    if (DSM_DONT_EXCEED_ITEMS(pool_id))
    {
      mem_avail = FALSE;
    }
  }

  /*-------------------------------------------------------------------------
    If both the pools are running low on memory or if DSM item allocation
    fails, we start dumping PPP frames.
  -------------------------------------------------------------------------*/
  if (mem_avail == FALSE ||
      (outgoing_tail_ptr = dsm_new_buffer(pool_id)) == NULL)
  {
    ERR("out of memory: small=%d large=%d",
        DSM_POOL_FREE_CNT(DSM_DS_SMALL_ITEM_POOL),
        DSM_POOL_FREE_CNT(DSM_DS_LARGE_ITEM_POOL),
        0);
    if_cb.OutDiscards++;
    dsm_free_packet(incoming_pkt_ptr);
    return NULL;
  }

  cnt  = 0;
  cp   = outgoing_tail_ptr->data_ptr;
  size = outgoing_tail_ptr->size;

  /*-------------------------------------------------------------------------
    Flush out any line garbage.
    If hdlc_mode == HW or SW, HDLC framing is added to pkt.
  -------------------------------------------------------------------------*/
  *cp++ = HDLC_FLAG;
  cnt++;
  ppp_cb_ptr->OutOpenFlag++;

  /*-------------------------------------------------------------------------
    Now build outgoing packet packet by pulling each byte from incoming
    packet, performing necessary FCS and stuffing along the way. NOTE:
    'dsm_pull8' will discard incoming packet.
  -------------------------------------------------------------------------*/
  while ( *incoming_pkt_ptr != NULL)
  {
    incoming_data_ptr = (*incoming_pkt_ptr)->data_ptr;

    for (i=0; i <(*incoming_pkt_ptr)->used; i++)
    {
      c = incoming_data_ptr[i];
      if( cnt + 7 > size)
      {
        /*-------------------------------------------------------------------
          Not enough room for escaped byte, plus frame end. Set tail size,
          append to outgoing packet then get another buffer item of
          sufficient size. Break if no memory, else make room in front of
          tail for escaped bytes and FCS.
        -------------------------------------------------------------------*/
        MSG_LOW("Appending to PPP Pkt", 0, 0, 0);
        outgoing_tail_ptr->used = cnt;
        dsm_append( &outgoing_head_ptr, &outgoing_tail_ptr);
        if(DSM_DONT_EXCEED_ITEMS(pool_id))
        {
          pool_id = DSM_DS_OTHER_POOL_ID(pool_id);
          if (DSM_DONT_EXCEED_ITEMS(pool_id))
          {
            mem_avail = FALSE;
          }
        }
        if(mem_avail == FALSE ||
           (outgoing_tail_ptr = dsm_new_buffer( pool_id)) == NULL)
        {
          /*-----------------------------------------------------------------
            No memory available, free packet and return
          -----------------------------------------------------------------*/
          dsm_free_packet(&outgoing_head_ptr);
          dsm_free_packet(incoming_pkt_ptr);
          ERR("out of memory: small=%d large=%d",
              DSM_POOL_FREE_CNT(DSM_DS_SMALL_ITEM_POOL),
              DSM_POOL_FREE_CNT(DSM_DS_LARGE_ITEM_POOL),
              0);
          if_cb.OutDiscards++;
          return NULL;
        }
        cnt  = 0;
        cp   = outgoing_tail_ptr->data_ptr;
        size = outgoing_tail_ptr->size;
      } /* if not enough room in outgoing_ptr */

      /*---------------------------------------------------------------------
          Fold char value into FCS calculation then do character escaping.
      ---------------------------------------------------------------------*/
      calc_fcs = pppfcs(calc_fcs, c);
      c7bits = c & 0x7f;
      if((( c7bits < SP_CHAR) && (accm & (1L << c7bits))) ||
         (( c == ccDEL)  &&  doing_lcp) ||  (c == HDLC_ESC_ASYNC) ||
          ( c == HDLC_FLAG))
      {
        *cp++  = HDLC_ESC_ASYNC;
        *cp++  = (c ^ HDLC_ESC_COMPL);
        cnt   += 2;
      }
      else /* no escaping needed, just copy byte over */
      {
        *cp++ = c;
        cnt++;
      }
    } /* for */

    test_item_ptr = *incoming_pkt_ptr;
    *incoming_pkt_ptr = (*incoming_pkt_ptr)->pkt_ptr;
    dsm_free_buffer( test_item_ptr);
  } /* while pkt chain is not empty */

  calc_fcs ^= 0xffff;                  /* complement                       */
  c         = (calc_fcs & 0x00ff);     /* Least significant byte first     */
  c7bits    = c & 0x7f;

  if(((c7bits < SP_CHAR) && (accm & (1L << c7bits))) ||
     ((c == ccDEL)  && doing_lcp)                    ||
     (c == HDLC_ESC_ASYNC)                           ||
     (c == HDLC_FLAG))
    {
      *cp++ = HDLC_ESC_ASYNC;
      *cp++ = (c ^ HDLC_ESC_COMPL);
      cnt += 2;
    }
    else /* no escaping needed */
    {
      *cp++ = c;
      cnt++;
    }
    c = (calc_fcs >> 8);    /* Most significant byte next */
    c7bits = c &0x7f;
    if(((c7bits < SP_CHAR) && (accm & (1L << c7bits))) ||
       ((c == ccDEL)  && doing_lcp) ||  (c == HDLC_ESC_ASYNC) ||
       (c == HDLC_FLAG))
    {
      *cp++  = HDLC_ESC_ASYNC;
      *cp++  = (c ^ HDLC_ESC_COMPL);
      cnt   += 2;
    }
    else
    {
      *cp++ = c;
      cnt++;
    }

    *cp++         = HDLC_FLAG;
    outgoing_tail_ptr->used = cnt + 1;
  /*-------------------------------------------------------------------------
    Tie off the packet
  -------------------------------------------------------------------------*/
  dsm_append( &outgoing_head_ptr, &outgoing_tail_ptr);

#ifdef FEATURE_DS_PSTATS
#error code not present
#endif

  /*-------------------------------------------------------------------------
    Now enqueue built packet to the RLP layer for TX over-the-air
  -------------------------------------------------------------------------*/
  outgoing_head_ptr->kind = DSM_PS_RLP;
#ifdef FEATURE_DSM_MEM_CHK
  /*-------------------------------------------------------------------------
    Set tracer to indicate where the item is being stored
  -------------------------------------------------------------------------*/
  outgoing_head_ptr->tracer = DSM_PS_TO_RLP_WM;
#endif

#ifdef FEATURE_DATA_PS_PPP_LOGGING
  ppp_log_frames_ext (outgoing_head_ptr, PPP_LOG_FRAMED_TX, device);
#endif /* FEATURE_DATA_PS_PPP_LOGGING */

  return outgoing_head_ptr;

} /* pppi_hdlc_sw_frame() */


#ifdef FEATURE_HDLC_HW_ACCEL
/*===========================================================================
FUNCTION PPPI_HDLC_HW_FRAME()

DESCRIPTION
  This function will use the hardware HDLC framer to frame the
  PPP packet based on the specified accm, append the CRC and trailing
  7E flag and send it on the specified interface.

  The input packet pointer should be the complete PPP packet to be framed
  including HDLC header and PPP protocol field (if we desire to send them),
  but without the CRC.

DEPENDENCIES
  This function should only be called from pppi_outgoing.

RETURN VALUE
  NULL on error
  ptr to framed packet on sucess

SIDE EFFECTS
  None
===========================================================================*/
dsm_item_type *pppi_hdlc_hw_frame
(
  ppp_dev_enum_type device,              /* outgoing device              */
  uint32                accm,               /* accm to use for this device  */
  dsm_item_type       **incoming_pkt_ptr    /* pkt to send                 */
)
{
  ppp_type       *ppp_cb_ptr = NULL;            /* PPP ctrl block ptr      */
  dsm_item_type  *outgoing_head_ptr = NULL;     /* first buffer            */
  dsm_item_type  *outgoing_tail_ptr = NULL;     /* current buffer          */
  dsm_item_type  *temp_item_ptr;
  uint8          *incoming_data_ptr;            /* Ptr to incoming data    */
  uint16          octets, bytes_left;
  dsm_mempool_id_enum_type pool_id;
  boolean         mem_avail = TRUE;
  uint16          framer_max_input_len, framer_input_len;
  uint16          buffer_offset;
  int             frame_size;
  boolean         start_of_pkt = TRUE;        /* To insert 7E at the start */
  /* - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - */

  if(device >= PPP_MAX_DEV                                 ||
     (*incoming_pkt_ptr)->used > (*incoming_pkt_ptr)->size ||
     (*incoming_pkt_ptr)->used == 0)
  {
    ASSERT(0);
    return NULL;
  }

#ifdef FEATURE_DATA_PS_PPP_LOGGING
  ppp_log_frames_ext (*incoming_pkt_ptr, PPP_LOG_UNFRAMED_TX, device);
#endif /* FEATURE_DATA_PS_PPP_LOGGING */

  /*-------------------------------------------------------------------------
    Initialize the framer
  -------------------------------------------------------------------------*/
  hdlc_framer_cmd(HDLC_FRAMER_RESET);

  /*-------------------------------------------------------------------------
    Assign interface specific variables.
  -------------------------------------------------------------------------*/
  HDLC_FRAMER_SET_ACCM(accm);
  ppp_cb_ptr = &(ppp_cb_array[device]);
  ASSERT(ppp_cb_ptr->hdlc_mode != HDLC_OFF);

  /*-------------------------------------------------------------------------
    Calculate the number of bytes in the packet. If hdlc is enabled, we need
    to send an additional 3 bytes (2 for FCS and 1 for 7E) - these are
    added to octets when they are inserted after the do loop below. So, don't
    add them to octets now.

    If hdlc_enabled == TRUE, update accounting statistics.
  -------------------------------------------------------------------------*/
  octets = dsm_length_packet(*incoming_pkt_ptr);
  bytes_left = octets;

  /*-------------------------------------------------------------------------
    Initializations before the loop
  -------------------------------------------------------------------------*/
  buffer_offset = 0;

  /*-------------------------------------------------------------------------
    Frame the entire PPP packet. Since the hardware buffer size is limited,
    we may need to do this in parts
  -------------------------------------------------------------------------*/
  do
  {
    /*-----------------------------------------------------------------------
      Determine which pool to use to store the outgoing packet. If the
      determined pool is running low on memory, use the other pool.

      Small item has N bytes, but we may need to shift the data_ptr by
      upto 3 bytes if it is not aligned at the 4-byte boundary. So, we have
      a max of N-3 bytes. In the worst case, every byte in the original PPP
      packet may need to be escaped which gives a max input size of (N-3)/2
      if we use small items.

      Also, 128 is the max input size for the framer hardware buffer.
    -----------------------------------------------------------------------*/
    if(bytes_left <= HDLC_MAX_FRAMER_BYTES_SMALL_POOL)
    {
      pool_id = DSM_DS_SMALL_ITEM_POOL;
    }
    else
    {
      pool_id = DSM_DS_LARGE_ITEM_POOL;
    }

    if (DSM_DONT_EXCEED_ITEMS(pool_id))
    {
      pool_id = DSM_DS_OTHER_POOL_ID(pool_id);
      if (DSM_DONT_EXCEED_ITEMS(pool_id))
      {
        mem_avail = FALSE;
      }
    }

    /*-----------------------------------------------------------------------
      If both the pools are running low on memory or if DSM item allocation
      fails, we start dumping PPP frames.
    -----------------------------------------------------------------------*/
    if (mem_avail == FALSE ||
        (outgoing_tail_ptr = dsm_new_buffer(pool_id)) == NULL)
    {
      ERR("out of memory: small=%d large=%d",
          DSM_POOL_FREE_CNT(DSM_DS_SMALL_ITEM_POOL),
          DSM_POOL_FREE_CNT(DSM_DS_LARGE_ITEM_POOL),
          0);
      if_cb.OutDiscards++;
      dsm_free_packet(&outgoing_head_ptr);
      dsm_free_packet(incoming_pkt_ptr);
      return NULL;
    }

#ifdef FEATURE_DSM_MEM_CHK
    outgoing_tail_ptr->tracer = DSM_CREATED_IN_PPP_HW_FRAMER;
#endif

    /*-----------------------------------------------------------------------
      Adjust outgoing_tail_ptr so that it's data_ptr is aligned at a
      4-byte boundary
    -----------------------------------------------------------------------*/
    DSM_FOUR_BYTE_ALIGN(outgoing_tail_ptr);

    /*-----------------------------------------------------------------------
      Calculate how many bytes can be framed in this iteration and write
      those many bytes to the hardware.
    -----------------------------------------------------------------------*/
    framer_max_input_len = (pool_id == DSM_DS_SMALL_ITEM_POOL)?
                            HDLC_MAX_FRAMER_BYTES_SMALL_POOL:
                            HDLC_MAX_FRAMER_INPUT_SIZE;

    /*-----------------------------------------------------------------------
      Sanity check just in case the macro HDLC_MAX_FRAMER_BYTES_SMALL_POOL
      evaluates to be greater than half the hardware buffer size.
    -----------------------------------------------------------------------*/
    framer_max_input_len = 
                       (framer_max_input_len > HDLC_MAX_FRAMER_INPUT_SIZE)?
                        HDLC_MAX_FRAMER_INPUT_SIZE:framer_max_input_len;

    framer_input_len = (bytes_left > framer_max_input_len)?
                        framer_max_input_len:bytes_left;

    bytes_left -= framer_input_len;

    /*----------------------------------------------------------------------
      Insert 7E at the beginning of the PPP packet
    ----------------------------------------------------------------------*/
    if(start_of_pkt == TRUE)
    {
      hdlc_framer_cmd(HDLC_FRAMER_INSERT_FLAG);
      start_of_pkt = FALSE;
    }

    do
    {
      incoming_data_ptr = (*incoming_pkt_ptr)->data_ptr;
      if(((*incoming_pkt_ptr)->used - buffer_offset) > framer_input_len)
      {
        hdlc_write_buf((byte *) ((byte *) incoming_data_ptr + buffer_offset),
                       HDLC_FRAMER_DATA_BUF, framer_input_len);
        buffer_offset += framer_input_len;
        framer_input_len = 0;
      }
      else
      {
        hdlc_write_buf((byte *) ((byte *) incoming_data_ptr + buffer_offset),
                       HDLC_FRAMER_DATA_BUF,
                       ((*incoming_pkt_ptr)->used - buffer_offset));
        framer_input_len -= ((*incoming_pkt_ptr)->used - buffer_offset);
        buffer_offset = 0;
        temp_item_ptr = *incoming_pkt_ptr;
        *incoming_pkt_ptr = (*incoming_pkt_ptr)->pkt_ptr;
        dsm_free_buffer( temp_item_ptr);
      }
    } while((framer_input_len > 0) && (*incoming_pkt_ptr != NULL));

    hdlc_framer_cmd(HDLC_FRAMER_DRAIN);
    hdlc_framer_cmd(HDLC_FRAMER_WRITE_DONE);

    /*-----------------------------------------------------------------------
      If there is a hardware buffer overflow while doing the framing,
      discard this packet
    -----------------------------------------------------------------------*/
    if(!HDLC_FRAMER_NO_ERR())
    {
      ERR("Framer hw error, discarding pkt!",0,0,0);
      if_cb.OutDiscards++;
      dsm_free_packet(&outgoing_head_ptr);
      dsm_free_packet(incoming_pkt_ptr);
      return NULL;
    }

    /*-----------------------------------------------------------------------
      Now read the partially framed PPP pkt into outgoing_tail_ptr
    -----------------------------------------------------------------------*/
    frame_size = HDLC_FRAMER_GET_BYTE_CNT();
    if( frame_size > 0)
    {
      hdlc_read_buf((byte *) outgoing_tail_ptr->data_ptr,
                     HDLC_FRAMER_DATA_BUF, frame_size);
      outgoing_tail_ptr->used = frame_size;
      ASSERT(outgoing_tail_ptr->used <= outgoing_tail_ptr->size );
      dsm_append( &outgoing_head_ptr, &outgoing_tail_ptr);
    }
    else
    {
      ERR("frame size is 0!",0,0,0);
      if_cb.OutDiscards++;
      dsm_free_buffer(outgoing_tail_ptr);
      dsm_free_packet(&outgoing_head_ptr);
      dsm_free_packet(incoming_pkt_ptr);
      return NULL;
    }

  } while((bytes_left > 0) && (*incoming_pkt_ptr != NULL));

  /*-------------------------------------------------------------------------
    Now add the CRC and 7E. This is kind of inefficient memory usage
    since we are using one small DSM item for only 3 bytes. We could
    more intelligently combine this with outgoing_tail_ptr above, but
    this will do for a first cut.
  -------------------------------------------------------------------------*/
  octets += 4;
  hdlc_framer_cmd(HDLC_FRAMER_INSERT_CRC);
  hdlc_framer_cmd(HDLC_FRAMER_INSERT_FLAG);
  hdlc_framer_cmd(HDLC_FRAMER_DRAIN);
  hdlc_framer_cmd(HDLC_FRAMER_WRITE_DONE);

  /*-------------------------------------------------------------------------
    If there is a hardware buffer overflow while doing the framing,
    discard this packet
  -------------------------------------------------------------------------*/
  if(!HDLC_FRAMER_NO_ERR())
    {
      ERR("Framer hw error, discarding pkt!",0,0,0);
      if_cb.OutDiscards++;
      dsm_free_packet(&outgoing_head_ptr);
      dsm_free_packet(incoming_pkt_ptr);
      return NULL;
    }

  frame_size = HDLC_FRAMER_GET_BYTE_CNT();
  if(frame_size > 0)
    {
      pool_id = DSM_DS_SMALL_ITEM_POOL;
      if (DSM_DONT_EXCEED_ITEMS(pool_id))
      {
        pool_id = DSM_DS_OTHER_POOL_ID(pool_id);
        if (DSM_DONT_EXCEED_ITEMS(pool_id))
        {
          mem_avail = FALSE;
        }
      }

      /*---------------------------------------------------------------------
        If both the pools are running low on memory or if DSM item allocation
        fails, we start dumping PPP frames.
      ---------------------------------------------------------------------*/
      if (mem_avail == FALSE ||
          (outgoing_tail_ptr = dsm_new_buffer(pool_id)) == NULL)
      {
        ERR("out of memory: small=%d large=%d",
            DSM_POOL_FREE_CNT(DSM_DS_SMALL_ITEM_POOL),
            DSM_POOL_FREE_CNT(DSM_DS_LARGE_ITEM_POOL),
            0);
        dsm_free_packet(&outgoing_head_ptr);
        dsm_free_packet(incoming_pkt_ptr);
        return NULL;
      }

#ifdef FEATURE_DSM_MEM_CHK
      outgoing_tail_ptr->tracer = DSM_CREATED_IN_PPP_HW_FRAMER;
#endif

      /*--------------------------------------------------------------------
        Adjust outgoing_tail_ptr so that it's data_ptr is aligned at a
        4-byte boundary
      ---------------------------------------------------------------------*/
      DSM_FOUR_BYTE_ALIGN(outgoing_tail_ptr);

      hdlc_read_buf((byte *) outgoing_tail_ptr->data_ptr,
                     HDLC_FRAMER_DATA_BUF, frame_size);
      outgoing_tail_ptr->used = frame_size;
      ASSERT(outgoing_tail_ptr->used <= outgoing_tail_ptr->size );
      dsm_append( &outgoing_head_ptr, &outgoing_tail_ptr);
    }
    else
    {
      ERR("frame size is 0!",0,0,0);
      if_cb.OutDiscards++;
      dsm_free_buffer(outgoing_tail_ptr);
      dsm_free_packet(&outgoing_head_ptr);
      dsm_free_packet(incoming_pkt_ptr);
      return NULL;
    } /* if frame_size > 0 */

#ifdef FEATURE_DS_PSTATS
#error code not present
#endif

  /*-------------------------------------------------------------------------
    Now enqueue built packet to the RLP layer for TX over-the-air
  -------------------------------------------------------------------------*/
  outgoing_head_ptr->kind = DSM_PS_RLP;
#ifdef FEATURE_DSM_MEM_CHK
  /*-------------------------------------------------------------------------
    Set tracer to indicate where the item is being stored
  -------------------------------------------------------------------------*/
  outgoing_head_ptr->tracer = DSM_PS_TO_RLP_WM;
#endif

#ifdef FEATURE_DATA_PS_PPP_LOGGING
  ppp_log_frames_ext (outgoing_head_ptr, PPP_LOG_FRAMED_TX, device);
#endif /* FEATURE_DATA_PS_PPP_LOGGING */

  return outgoing_head_ptr;

} /* pppi_hdlc_hw_frame() */
#endif /* FEATURE_HDLC_HW_ACCEL */


/*===========================================================================

                                INBOUND PATH

===========================================================================*/
/*===========================================================================
FUNCTION PPPI_HDLC_UNFRAME_INIT() (was PPP_ASY_IN_INIT())

DESCRIPTION
  This function will free any packets that pppi_hdlc_sw_unframe() had hanging
  around from the previous call.  Also clear out the various other params.

DEPENDENCIES
  Should only be called when entering Autodetect mode!

RETURN VALUE
  None

SIDE EFFECTS
  None
===========================================================================*/
void pppi_hdlc_unframe_init
(
   ppp_dev_enum_type device           /* ppp device pkt arrived on        */
)
{
  ppp_type *ppp_cb_ptr;
  /* - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - */
  if(device >= PPP_MAX_DEV)
  {
    ASSERT(0);
    return;
  }

  ppp_cb_ptr = ppp_cb_array + device;

  /*-------------------------------------------------------------------------
    Free any items that are hanging around, then clear out the device control
    block.
  -------------------------------------------------------------------------*/
  dsm_free_packet(&(ppp_cb_ptr->input_params.pkt_head_ptr));
  dsm_free_packet(&(ppp_cb_ptr->input_params.orig_head_ptr));

#ifdef FEATURE_HDLC_HW_ACCEL
  if(ppp_cb_ptr->hdlc_mode == HDLC_HW)
  {
    hdlc_deframer_cmd(HDLC_DEFRAMER_RESET);  
  }
#endif /* FEATURE_HDLC_HW_ACCEL */

  memset(&(ppp_cb_ptr->input_params), 0, sizeof(hdlc_unframe_var_type));
  ppp_cb_ptr->input_params.tail_full = TRUE;
} /* pppi_hdlc_unframe_init() */


#ifndef MSM5000_IRAM_FWD
/*===========================================================================
FUNCTION PPPI_HDLC_SW_UNFRAME() (was PPP_ASY_IN())

DESCRIPTION
  This function will Packetize PPP input from asynchronous device, and does
  network model processing.  It uses a state machine to accomplish this
  network model processing.  The state machine is illustrated below:

                              -------------
                             |             |
            -----------------|  UNFRAMING  |<------------------
           /                 |             |                   \
  Dispatched packet to        -------------                     \
    pppi_incoming() or               ^                      Found matching
     found FCS error                |                         protocol
       /                   Found start of packet                   \
       |                    (Full Network Mode)                     \
       v                            |                                |
  ------------                -------------                     ------------
 |            |              |             |  Found start of   |            |
 | FINDING_7E |-- Found 7E ->| FINDING_SOP |- packet (Pseudo ->|  SNOOPING  |
 |            |              |             |  Network  Mode)   |            |
  ------------                -------------                     ------------
       ^                        /      ^                            |
       |                        |   Found 7E                       /
        \                        \----/                           /
   Dispatched packet                                        No matching
   to out_iface_tx()          -------------               protocol found
           \                 |             |                   /
            \----------------| FINDING_EOP |<-----------------/
                             |             |
                              -------------

DEPENDENCIES
  None

RETURN VALUE
  None

SIDE EFFECTS
  None
===========================================================================*/
void pppi_hdlc_sw_unframe
(
  ppp_dev_enum_type  device,          /* PPP device pkt arrived on         */
  dsm_item_type    **item_head_ptr    /* address of ptr to a dsm_item_type */
)
{
  dsm_item_type *incoming_ptr  = NULL; /* Ptr to passed data item          */
  dsm_item_type *temp_item_ptr = NULL; /* Ptr to hold temp buffer          */
  hdlc_unframe_var_type *param_ptr;    /* Ptr to static vars forunframing  */
  dsm_item_type *temp_tail_ptr = NULL; /* Ptr to hold temp tail buffer     */
  ppp_type *ppp_cb_ptr;                 /* ppp CB for this dev   */
  ppp_type *ppp_bridge_cb_ptr;         /* ppp CB for the bridged dev       */
  dsm_item_type *prev_temp_item_ptr = NULL;
  uint16 cur_byte_loc = 0;             /* cur location in incoming_data_ptr*/
  uint16 total_bytes_copied = 0;
  uint16 num_bytes_to_copy = 0;
  dsm_mempool_id_enum_type pool_id;
  snoop_proto_ret_val_enum_type snoop_res;     /* Result of initial snoop  */
  byte ch_off;                                 /* current char at offset   */
  boolean mem_avail = TRUE;

  /* - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - */
  if(device >= PPP_MAX_DEV   ||
     (item_head_ptr == NULL) ||
     ((*item_head_ptr) == NULL))
  {
    ASSERT(0);
    return;
  }

  ppp_cb_ptr = &(ppp_cb_array[device]);

#ifdef FEATURE_DATA_PS_PPP_LOGGING
  ppp_log_frames_ext (*item_head_ptr, PPP_LOG_FRAMED_RX, device);
#endif /* FEATURE_DATA_PS_PPP_LOGGING */

  /*-------------------------------------------------------------------------
    Initialize variables. Incoming_ptr points to the current item in the
    incoming packet chain; orig_tail_ptr is the pointer to the last item
    in the unchanged packet chain; cur_byte_loc is the location of the byte
    being processed in the incoming_ptr->data_ptr.
  -------------------------------------------------------------------------*/
  incoming_ptr = *item_head_ptr;
  MSG_LOW("hdlc_sw_unframe() on device %d", device, 0, 0);

  param_ptr = &(ppp_cb_array[device].input_params);
  cur_byte_loc =0;

#ifdef FEATURE_DS_MOBILE_IP
  /*-------------------------------------------------------------------------
    Let MIP code know that data is being transferred over Um interace
  -------------------------------------------------------------------------*/
  if ( device == PPP_UM_SN_DEV )
  {
    MIP_DATA_TRANSFER_ACTION;
  }
#endif /* FEATURE_DS_MOBILE_IP */

  /*lint -esym(794, incoming_ptr)
   * Have lint ignore error 794 (use of NULL ptr) for the incoming_ptr as
   * within the loop it cannot be NULL
   */
  /*-------------------------------------------------------------------------
    Process each item in the incoming packet chain according to the unframe
    mode.
  -------------------------------------------------------------------------*/
  while (incoming_ptr != NULL)
  {
    MSG_LOW("Processing %d bytes for device %d",
            incoming_ptr->used, device, 0);

    if (incoming_ptr->used > 0)
    {
      switch(param_ptr->unframe_mode)
      {
        case FINDING_7E:
          /*-----------------------------------------------------------------
            If unframe mode is FINDING_7E, 
 
            Get a new item for the PPP packet that is being built.
            The address of this item will be stored in pkt_head_ptr.
            
            Change the mode to FINDING_SOP mode.
          -----------------------------------------------------------------*/
          MSG_LOW("In FINDING_7E mode", 0, 0, 0);
          cur_byte_loc = 0;

          /*-----------------------------------------------------------------
            Found the 7E in incoming byte stream. Set variables and change
            mode to FINDING_SOP.
          -----------------------------------------------------------------*/
          if (incoming_ptr->used > 0)
          {
            /*---------------------------------------------------------------
              Use heuristic to determine what size item to grab.  If the last
              packet was bigger than RAMP_MAX_SIZE grab a large item
              otherwise grab a small one. If large items are not available
              but small ones are, we go ahead and start using small items.
            ---------------------------------------------------------------*/
            if (param_ptr->prev_pkt_size > RAMP_MAX_SIZE &&
                DSM_DONT_EXCEED_ITEMS(DSM_DS_LARGE_ITEM_POOL) == FALSE)
            {
              param_ptr->pkt_head_ptr =
                dsm_new_buffer(DSM_DS_LARGE_ITEM_POOL);
              MSG_LOW(" Free Large = %d; param_ptr->pkt_head_ptr assigned",
                      DSM_POOL_FREE_CNT(DSM_DS_LARGE_ITEM_POOL), 0, 0);
            }
            else if (DSM_DONT_EXCEED_ITEMS(DSM_DS_SMALL_ITEM_POOL) == FALSE)
            {
              param_ptr->pkt_head_ptr =
                dsm_new_buffer(DSM_DS_SMALL_ITEM_POOL);
              MSG_LOW(" Free Small = %d; param_ptr->pkt_head_ptr assigned",
                      DSM_POOL_FREE_CNT(DSM_DS_SMALL_ITEM_POOL),0, 0);
            }

            /*---------------------------------------------------------------
              If memory allocation fails, free the incoming packet and return
              from function.
            ---------------------------------------------------------------*/
            if (param_ptr->pkt_head_ptr == NULL)
            {
              ERR("out of memory: small=%d large=%d",
                   DSM_POOL_FREE_CNT(DSM_DS_SMALL_ITEM_POOL),
                   DSM_POOL_FREE_CNT(DSM_DS_LARGE_ITEM_POOL),
                   0);
              dsm_free_packet(&incoming_ptr);
              return;
            }
#ifdef FEATURE_DSM_MEM_CHK
            param_ptr->pkt_head_ptr->tracer = DSM_CREATED_IN_ASY_IN;
#endif

            /*---------------------------------------------------------------
              Setup params for future unframing, change to FINDING_SOP mode
            ---------------------------------------------------------------*/
            param_ptr->rx_pkt_size       = 0;
            param_ptr->pkt_tail_ptr      = param_ptr->pkt_head_ptr;
            param_ptr->calc_fcs          = HDLC_FCS_START;
            param_ptr->orig_head_ptr     = NULL;
            param_ptr->unframe_mode      = FINDING_SOP;
          }

        case FINDING_SOP:
          /*-----------------------------------------------------------------
            If unframing mode is FINDING_SOP, the first byte after the HDLC
            flag is sought, i.e. skip all the 7E flags at the begining of the
            packet. If the interface is in PSEUDO_NET mode, then the new
            unframing mode will be SNOOPING, otherwise it will be UNFRAMING.
          -----------------------------------------------------------------*/
          MSG_LOW("In FINDING_SOP mode", 0, 0, 0);

          /*-----------------------------------------------------------------
            Go through the incoming packet to find first byte that's no 7E.
          -----------------------------------------------------------------*/
          while ((incoming_ptr->used > cur_byte_loc) &&
                 (param_ptr->unframe_mode == FINDING_SOP))
          {
            if (incoming_ptr->data_ptr[cur_byte_loc] == HDLC_FLAG)
            {
              cur_byte_loc++;
            }
            else
            {
              /*-------------------------------------------------------------
                If input interface is in PPP_PSEUDO_NET_MODE, start snooping
                for protocol and message type, otherwise unframe the rest of
                the packet.
              -------------------------------------------------------------*/
              switch(ppp_cb_ptr->mode)
              {
                /*-----------------------------------------------------------
                  Pseudo Network mode
                -----------------------------------------------------------*/
                case PPP_PSEUDO_NET_MODE:
                  param_ptr->unframe_mode = SNOOPING;
                  break;

                  /*---------------------------------------------------------
                    Full Network or Internal mode or internal snoop mode
                  ---------------------------------------------------------*/
                case PPP_FULL_NET_MODE:
                case PPP_INTERNAL_SNOOP_MODE:
                case PPP_INTERNAL_MODE:
                  param_ptr->unframe_mode = UNFRAMING;
                  dsm_free_packet(&(param_ptr->orig_head_ptr));
                  param_ptr->orig_tail_ptr = NULL;
                  break;

                default:
                  /*lint -e506 -e774 */
                  ERR("ppp.c: Interface %d is in bogus mode %d",
                       device,
                      ppp_cb_ptr->mode, 0);
                  ASSERT(0);
                  /* lint +e506 +e774 */

              } /* switch(device mode) */

            } /* if(not 7E) */

          }  /* while(FINDING_SOP) */
          break;

        case SNOOPING:
          /*-----------------------------------------------------------------
            If the unframe mode is SNOOPING, the first 2 or 4 bytes of the
            incoming byte stream are unframed and checked to see if they
            contain a protcol and message that is being snooped on. The
            unframed packet is stored in pkt_head_ptr and the original
            unchanged packet is stored in orig_head_ptr pkt chain.
          -----------------------------------------------------------------*/
          MSG_LOW("In SNOOPING mode", 0, 0, 0);
          ch_off = incoming_ptr->data_ptr[cur_byte_loc];

          /*-----------------------------------------------------------------
            Go through each byte in incoming_ptr and store in
            param_ptr->pkt_head_ptr and snoop on it.
          -----------------------------------------------------------------*/
          while ((param_ptr->unframe_mode == SNOOPING) &&
                 (cur_byte_loc < incoming_ptr->used))
          {
            /*---------------------------------------------------------------
              Somehow the mobile is receiving a 0x7e, while it is SNOOPING.
              This can happen if there is data loss or if the infrastructure
              sends a malformed PPP packet.
              To handle this - cleanup and return to FINDING_7E state.
            ---------------------------------------------------------------*/
            if( ch_off == HDLC_FLAG)
            {
              MSG_HIGH("if%d found 7E in SNOOPING loc=%d",
                       device,
                       cur_byte_loc,
                       0);
              dsm_pullup( &incoming_ptr, NULL, cur_byte_loc);
              ASSERT( incoming_ptr != NULL);
              cur_byte_loc = 0;
              dsm_free_packet( &(param_ptr->orig_head_ptr));
              param_ptr->orig_tail_ptr = NULL;
              dsm_free_packet( &(param_ptr->pkt_head_ptr));
              param_ptr->pkt_tail_ptr = NULL;
              param_ptr->unframe_mode = FINDING_7E;
              break;
            }

            /*---------------------------------------------------------------
              If the last byte of the last incoming_ptr were 7D, unescape
              this byte.
            ---------------------------------------------------------------*/
            if (param_ptr->escape == TRUE)
            {
              ch_off ^= HDLC_ESC_COMPL;
              param_ptr->escape = FALSE;
            }

            /*---------------------------------------------------------------
              Found 7D. Unscape the next byte.
            ---------------------------------------------------------------*/
            else if (ch_off == HDLC_ESC_ASYNC)
            {
              ch_off = incoming_ptr->data_ptr[++cur_byte_loc];

              ASSERT(cur_byte_loc <= incoming_ptr->used);
              if(cur_byte_loc == incoming_ptr->used)
              {
                param_ptr->escape = TRUE;
                break;
              }
              else
              {
                ch_off ^= HDLC_ESC_COMPL;
              }
            } /* if ch_off == 7D */

            /*lint -e734-----------------------------------------------------
              Copy escaped byte into pkt_head_ptr. Note, we assume that
              pkt_head_ptr has atleast 4 bytes. (Small item size should not
              be reduced to less than 4 bytes).
            ---------------------------------------------------------------*/
            param_ptr->pkt_head_ptr->
              data_ptr[(param_ptr->pkt_head_ptr)->used++] = ch_off;
            param_ptr->calc_fcs = pppfcs(param_ptr->calc_fcs, ch_off);

            /*lint +e734 */
            /*---------------------------------------------------------------
              Check if we have unframed 2 or 4 bytes, if so call the function
              for early protocol detection.  Take the appropriate action
              based on the result.
            ---------------------------------------------------------------*/
            if (((param_ptr->pkt_head_ptr)->used == PPP_PROTO_SIZ) ||
                ((param_ptr->pkt_head_ptr)->used == PPP_ACF_PROTO_SIZ))
            {
              snoop_res = snoop_early_proto_detect(device,
                                                   param_ptr->pkt_head_ptr);
              switch (snoop_res)
              {
                case SNOOP_IGNORE_PKT:
                  MSG_LOW(" %d bytes snooped - not interested in pkt",
                          (param_ptr->pkt_head_ptr)->used, 0, 0);
                  dsm_free_packet(&(param_ptr->pkt_head_ptr));
                  param_ptr->unframe_mode = FINDING_EOP;
                  param_ptr->pkt_tail_ptr = NULL;
                  break;

                case SNOOP_DO_UNFRAMING:
                  MSG_LOW(" %d bytes snooped - might have protocol",
                          (param_ptr->pkt_head_ptr)->used, 0, 0);
                  dsm_free_packet(&(param_ptr->orig_head_ptr));
                  param_ptr->unframe_mode = UNFRAMING;
        				  param_ptr->orig_tail_ptr = NULL;
                  break;

                case SNOOP_NEXT_TWO_BYTES:
                  MSG_LOW(" %d bytes snooped - need to unframe more",
                          (param_ptr->pkt_head_ptr)->used, 0, 0);
                  param_ptr->unframe_mode  = SNOOPING;
                  break;

                default:
                  ASSERT(0);
              } /* switch on snoop_res */

            } /* if 2 or 4 bytes unframed */

            ch_off = incoming_ptr->data_ptr[++cur_byte_loc];
          } /* while */
          break;

        case FINDING_EOP:
          /*-----------------------------------------------------------------
            If the unframing mode is FINDING_EOP (End of Packet) the byte
            stream is parsed to find the 7E.  The packet is not unframed, the
            end is being saught so that it can be forwarded on.
          -----------------------------------------------------------------*/
          MSG_LOW("In FINDING_EOP mode", 0, 0, 0);
          ppp_bridge_cb_ptr = &ppp_cb_array[ppp_cb_ptr->bridge_dev];

          while(param_ptr->orig_head_ptr != NULL)
          {
            /*---------------------------------------------------------------
              As we are not keeping items until we have a full ppp packet
              dispatch anything that is still hanging around from before.
              As the transmit functions don't like packet chains, pull each
              item of the chain and sent it individually.
            ---------------------------------------------------------------*/
            MSG_LOW("dispatching orig_head_ptr", 0, 0, 0);
            temp_item_ptr = param_ptr->orig_head_ptr;
            param_ptr->orig_head_ptr = param_ptr->orig_head_ptr->pkt_ptr;
            temp_item_ptr->pkt_ptr = NULL;
#ifdef FEATURE_DSM_MEM_CHK
            temp_item_ptr->tracer = DSM_OUT_OF_ASY_IN_EOP;
#endif
            temp_item_ptr->app_field = NULL;

#ifdef FEATURE_DATA_PS_PPP_LOGGING
            ppp_log_frames_ext (*item_head_ptr, 
                                PPP_LOG_FRAMED_TX, 
                                ppp_cb_ptr->bridge_dev); 
#endif /*FEATURE_DATA_PS_PPP_LOGGING*/

            ppp_bridge_cb_ptr->tx_f_ptr(&temp_item_ptr, 
                                        ppp_bridge_cb_ptr->tx_user_data);
          } /* while(items before incoming_ptr) */

          if( param_ptr->orig_head_ptr == NULL)
          {
            param_ptr->orig_tail_ptr = NULL;
          }

#ifdef FEATURE_MSMHW_HUNT_SUPPORTED
          /*-----------------------------------------------------------------
            If the 7E detector in hardware is active and we have set the
            hunt bit, we can bypass this DSM item if it does not have any
            7Es. The hunt bit is only valid for packets coming in on the
            Um interface.
          -----------------------------------------------------------------*/
          if(device == PPP_UM_SN_DEV &&
             !HDRDSM_GET_HUNT_BIT(incoming_ptr))
          {
            cur_byte_loc = incoming_ptr->used;
          }
#endif

          while (cur_byte_loc < incoming_ptr->used)
          {
            ch_off = incoming_ptr->data_ptr[cur_byte_loc++];

            /*---------------------------------------------------------------
              If 7E is found,
            ---------------------------------------------------------------*/
            if (ch_off == HDLC_FLAG)
            {
              MSG_LOW("Found 7E at offset %d", cur_byte_loc - 1, 0, 0);

              /*-------------------------------------------------------------
                This packet was not unframed so update the statistics
              -------------------------------------------------------------*/
              if_cb.FramedPkts[device] += 1;
              if_cb.TotalPkts[device]  += 1;

              /*-------------------------------------------------------------
                We need to keep a copy of the bytes that follow the 7E and a
                copy of the 7E itself to construct and examine the next
                packet.  Copy the least number of bytes: if there are less
                preceeding the 7E copy those, otherwise copy those that
                follow.
              -------------------------------------------------------------*/
              if (incoming_ptr->used == cur_byte_loc)
              {
                MSG_LOW("No bytes to copy after 7E", 0, 0, 0);
              }
              else
              {
                total_bytes_copied = 0;
                prev_temp_item_ptr = incoming_ptr;
                /*-----------------------------------------------------------
                  Iterate through the bytes in the incoming packet until we
                  have copied the required number of bytes. In each
                  iteration of the loop, we copy minimum of newly allocated
                  DSM item size and the remaining bytes to copy.
                -----------------------------------------------------------*/
                while (total_bytes_copied <
                       (incoming_ptr->used - cur_byte_loc))
                {
                  pool_id =
                    DSM_DS_POOL_SIZE(incoming_ptr->used-cur_byte_loc);
                  if (DSM_DONT_EXCEED_ITEMS(pool_id))
                  {
                    pool_id = DSM_DS_OTHER_POOL_ID(pool_id);
                    if (DSM_DONT_EXCEED_ITEMS(pool_id))
                    {
                      mem_avail = FALSE;
                    }
                  }
                  /*---------------------------------------------------------
                    If memory allocation fails - recover.
                  ---------------------------------------------------------*/
                  if ( mem_avail == FALSE ||
                       (temp_item_ptr = dsm_new_buffer(pool_id)) == NULL )
                  {
                    ERR("out of memory: small=%d large=%d",
                        DSM_POOL_FREE_CNT(DSM_DS_SMALL_ITEM_POOL),
                        DSM_POOL_FREE_CNT(DSM_DS_LARGE_ITEM_POOL),
                        0);
                    param_ptr->unframe_mode = FINDING_7E;
                    dsm_free_packet(&incoming_ptr);
                    dsm_free_packet(&(param_ptr->orig_head_ptr));
                    param_ptr->orig_tail_ptr = NULL;
                    return;
                  }
#ifdef FEATURE_DSM_MEM_CHK
                  temp_item_ptr->tracer = DSM_CREATED_IN_ASY_IN;
#endif
                  /*---------------------------------------------------------
                    Note that we may allocated a small dsm item above (if
                    running low on large items) although the total number
                    of bytes to copy require a large item.
                  ---------------------------------------------------------*/

                  num_bytes_to_copy =
                    MIN(
                      (incoming_ptr->used -cur_byte_loc -total_bytes_copied),
                      temp_item_ptr->size);

                  /*---------------------------------------------------------
                    Copy the portion of the incoming DSM item into another
                    DSM item.
                  ---------------------------------------------------------*/

                  (void)memcpy(
                    temp_item_ptr->data_ptr,
                    &(incoming_ptr->data_ptr[cur_byte_loc+total_bytes_copied]),
                    num_bytes_to_copy);

                  total_bytes_copied += num_bytes_to_copy;
                  temp_item_ptr->used = num_bytes_to_copy;

                  ASSERT( temp_item_ptr->used <= temp_item_ptr->size);

                  MSG_LOW("%d bytes copied after 7E in FINDING_EOP",
                          num_bytes_to_copy, 0, 0);

                  /*---------------------------------------------------------
                    Insert copied bytes (start of next packet) AFTER the
                    current head.
                  ---------------------------------------------------------*/
                  temp_item_ptr->pkt_ptr = prev_temp_item_ptr->pkt_ptr;
                  prev_temp_item_ptr->pkt_ptr = temp_item_ptr;
                  prev_temp_item_ptr = temp_item_ptr;
                }
                incoming_ptr->used = cur_byte_loc;
              }


              /*-------------------------------------------------------------
                As we have found the end of the packet, start looking for the
                next 7E at the beginning of the next item.
              -------------------------------------------------------------*/
              MSG_LOW("Changing to FINDING_7E state", 0, 0, 0);
              param_ptr->unframe_mode = FINDING_7E;
              break;

            } /* if 7E found */

          } /* while(still bytes in this item) */

          ASSERT( incoming_ptr != NULL);

          /*-----------------------------------------------------------------
            Transmit the first item on the incoming_ptr packet chain, and
            loop up to examine the next item on the chain.  The cur_byte_loc
            is reset as we will be examining a new item, and should start
            from the beginning of the item.
          -----------------------------------------------------------------*/
          MSG_LOW("Dispatching head item in chain", 0, 0, 0);
          temp_item_ptr = incoming_ptr;
          incoming_ptr = incoming_ptr->pkt_ptr;
          temp_item_ptr->pkt_ptr = NULL;
#ifdef FEATURE_DSM_MEM_CHK
          temp_item_ptr->tracer = DSM_OUT_OF_ASY_IN_EOP_HEAD;
#endif
          temp_item_ptr->app_field = NULL;

#ifdef FEATURE_DATA_PS_PPP_LOGGING
            ppp_log_frames_ext (*item_head_ptr, 
                                PPP_LOG_FRAMED_TX, 
                                ppp_cb_ptr->bridge_dev); 
#endif /*FEATURE_DATA_PS_PPP_LOGGING*/

            ppp_bridge_cb_ptr->tx_f_ptr(&temp_item_ptr, 
                                      ppp_bridge_cb_ptr->tx_user_data);

          cur_byte_loc = 0;

          continue; /* packet chain while */

        case UNFRAMING:
          /*-----------------------------------------------------------------
            If the unframing mode is UNFRAMING, the PPP packet will be
            extracted from the incoming byte stream (by unescaping and
            unframing it).  The extracted PPP packet will be sent to the
            upper layer.
          -----------------------------------------------------------------*/
          MSG_LOW("In UNFRAMING mode", 0, 0, 0);
          ch_off = incoming_ptr->data_ptr[cur_byte_loc];

          /*-----------------------------------------------------------------
            If last byte was a 7d then escape the next byte.
          -----------------------------------------------------------------*/
          if (param_ptr->escape == TRUE)
          {
            ch_off ^= HDLC_ESC_COMPL;
            (param_ptr->escape) = FALSE;

            /*---------------------------------------------------------------
              If the item is full then get a new item, and add the byte into
              the item.
            ---------------------------------------------------------------*/
            ASSERT((param_ptr->pkt_tail_ptr)->used <=
                   (param_ptr->pkt_tail_ptr)->size);
            if ((param_ptr->pkt_tail_ptr)->used ==
                (param_ptr->pkt_tail_ptr)->size)
            {
              param_ptr->rx_pkt_size += (param_ptr->pkt_tail_ptr)->used;
              temp_tail_ptr           = param_ptr->pkt_tail_ptr;

              param_ptr->pkt_tail_ptr =
                dsm_new_buffer(DSM_DS_SMALL_ITEM_POOL);
              /*-------------------------------------------------------------
                If memory allocation fails, discard the PPP packet chain.
              -------------------------------------------------------------*/
              if (param_ptr->pkt_tail_ptr == NULL)
              {
                MSG_HIGH(" Small item allocation failed",0 ,0, 0);
                pppi_skipped(device, &(param_ptr->pkt_head_ptr));
                param_ptr->pkt_tail_ptr = NULL;
                param_ptr->unframe_mode = FINDING_7E;
                dsm_free_packet(&incoming_ptr);
                dsm_free_packet(&(param_ptr->orig_head_ptr));
                param_ptr->orig_tail_ptr = NULL;
                return;
              }
#ifdef FEATURE_DSM_MEM_CHK
              param_ptr->pkt_tail_ptr->tracer = DSM_CREATED_IN_ASY_IN;
#endif

              /*-------------------------------------------------------------
                Adding the newly created item to the packet chain, which
                contains the PPP payload to be passed to the upper layer.
              -------------------------------------------------------------*/
              temp_tail_ptr->pkt_ptr = (param_ptr->pkt_tail_ptr);
            }

            (param_ptr->pkt_tail_ptr)->data_ptr
              [(param_ptr->pkt_tail_ptr)->used++] = ch_off;

            /*lint -e734 */
            param_ptr->calc_fcs = pppfcs(param_ptr->calc_fcs, ch_off);
            /*lint +e734 */

            ch_off = incoming_ptr->data_ptr[++cur_byte_loc];
          }

          while ((param_ptr->unframe_mode == UNFRAMING) &&
                 (cur_byte_loc < incoming_ptr->used))
          {
            /*---------------------------------------------------------------
              Found 7E. Check CRC and pass the PPP packet to the upper layer
              and set the unframe mode to NULL.
            ---------------------------------------------------------------*/
            if(ch_off == HDLC_FLAG)
            {
              /*-------------------------------------------------------------
                Increment cur_byte_loc to ensure that the 0x7e is not
                included in the next packet.
              -------------------------------------------------------------*/
              cur_byte_loc++;
              param_ptr->rx_pkt_size += (param_ptr->pkt_tail_ptr)->used;

              /*-------------------------------------------------------------
                Check the FCS. If the FCS fails or if the packet is too short
                (we need at least 3 bytes - 2 for FCS+1 for protocol field),
                skip this PPP packet otherwise pass it to the upper layer. 
              -------------------------------------------------------------*/
              if(param_ptr->calc_fcs != HDLC_FCS_FINAL)
              {
                ERR("Dev(%d) Incoming PPP Checksum Error", device, 0, 0);
                pppi_skipped(device, &(param_ptr->pkt_head_ptr));
                param_ptr->pkt_tail_ptr = NULL;
                ppp_cb_ptr->InChecksum++;
                (void)dsm_pullup(&incoming_ptr, NULL, cur_byte_loc);
                cur_byte_loc = 0;
                param_ptr->unframe_mode = FINDING_7E;
                break;
              }
              else if (param_ptr->rx_pkt_size < 3)
              {
                ERR("Dev(%d) Incoming pkt (len=%d), too short", 
                    device, param_ptr->rx_pkt_size, 0); 
                pppi_skipped(device, &(param_ptr->pkt_head_ptr));
                param_ptr->pkt_tail_ptr = NULL;
                (void)dsm_pullup(&incoming_ptr, NULL, cur_byte_loc);
                cur_byte_loc = 0;
                param_ptr->unframe_mode = FINDING_7E;
                break;                
              }
              else
              {
                /*-----------------------------------------------------------
                  Trim off the FCS bytes from the PPP packet.
                -----------------------------------------------------------*/
                dsm_trim_packet(&(param_ptr->pkt_head_ptr),
                                param_ptr->rx_pkt_size - 2);
                param_ptr->rx_pkt_size -= 2;
                MSG_LOW("PPP Pkt size: %d",param_ptr->rx_pkt_size, 0, 0);
#ifdef FEATURE_DSM_MEM_CHK
                /*-----------------------------------------------------------
                  Set tracer to indicate where the item is being stored
                -----------------------------------------------------------*/
                (param_ptr->pkt_head_ptr)->tracer = DSM_PPP_INCOMING;
#endif
                /*-----------------------------------------------------------
                  Here the ppp_prev_pkt_size variable is set to the last rcvd
                  pkt size - to be used in assigning lg/sm item for next pkt
                -----------------------------------------------------------*/
                param_ptr->prev_pkt_size = param_ptr->rx_pkt_size;

#ifdef FEATURE_DATA_PS_PPP_LOGGING
                ppp_log_frames_ext (param_ptr->pkt_head_ptr,
                                    PPP_LOG_UNFRAMED_RX,
                                    device);
#endif /* FEATURE_DATA_PS_PPP_LOGGING */

                /*-----------------------------------------------------------
                  Pkt. is all built up, pass it on to upper protocols.
                -----------------------------------------------------------*/
                pppi_incoming(device, &(param_ptr->pkt_head_ptr));
                param_ptr->pkt_head_ptr = NULL;
                param_ptr->pkt_tail_ptr = NULL;
                (void)dsm_pullup(&incoming_ptr, NULL, cur_byte_loc);
                cur_byte_loc = 0;
                param_ptr->unframe_mode = FINDING_7E;
                break;
              } /* if CRC passed */

            }   /* if 7E found */

            /*---------------------------------------------------------------
              Found 7D. Escape the next byte.
            ---------------------------------------------------------------*/
            if(ch_off == HDLC_ESC_ASYNC)
            {
              ch_off = incoming_ptr->data_ptr[++cur_byte_loc];

              ASSERT(cur_byte_loc <= incoming_ptr->used);
              if(cur_byte_loc == incoming_ptr->used)
              {
                (param_ptr->escape) = TRUE;
                break;
              }
              else
              {
                ch_off ^= HDLC_ESC_COMPL;
              }
            }

            /*---------------------------------------------------------------
              If the item is full then get a new item, and add the byte into
              the item.
            ---------------------------------------------------------------*/
            ASSERT((param_ptr->pkt_tail_ptr)->used <=
                   (param_ptr->pkt_tail_ptr)->size);
            if ((param_ptr->pkt_tail_ptr)->used ==
                (param_ptr->pkt_tail_ptr)->size)
            {
              param_ptr->rx_pkt_size += (param_ptr->pkt_tail_ptr)->used;
              temp_item_ptr = (param_ptr->pkt_tail_ptr);
              param_ptr->pkt_tail_ptr =
                dsm_new_buffer(DSM_DS_SMALL_ITEM_POOL);
              /*-------------------------------------------------------------
                If memory allocation fails, discard the PPP packet chain.
              -------------------------------------------------------------*/
              if ((param_ptr->pkt_tail_ptr) == NULL)
              {
                MSG_HIGH(" Small item allocation failed",0 ,0, 0);
                pppi_skipped(device, &(param_ptr->pkt_head_ptr));
                param_ptr->pkt_tail_ptr = NULL;
                param_ptr->unframe_mode = FINDING_7E;
                dsm_free_packet(&incoming_ptr);
                return;
              }
#ifdef FEATURE_DSM_MEM_CHK
              param_ptr->pkt_tail_ptr->tracer = DSM_CREATED_IN_ASY_IN;
#endif
              temp_item_ptr->pkt_ptr = (param_ptr->pkt_tail_ptr);
            }

            (param_ptr->pkt_tail_ptr)->data_ptr
              [(param_ptr->pkt_tail_ptr)->used++] = ch_off;
            /*lint -e734 */
            param_ptr->calc_fcs = pppfcs(param_ptr->calc_fcs, ch_off);
            /*lint +e734 */

            ch_off = incoming_ptr->data_ptr[++cur_byte_loc];
          } /* while */
          break;

        default:
          ASSERT(0);  /* Sanity check for param_ptr->unframe_mode */

      } /* switch(unframing mode) */

    } /* if incoming_ptr->used > 0 */

    if( incoming_ptr != NULL)
    {

     ASSERT(cur_byte_loc <= incoming_ptr->used);

     /*----------------------------------------------------------------------
       If the entire incoming_ptr has been looked at, free the item and go
       to the next item in the packet chain.
     ----------------------------------------------------------------------*/
     if (cur_byte_loc == incoming_ptr->used)
     {
      /*---------------------------------------------------------------------
        If we are in the UNFRAMING mode, and the current packet is empty,
        free it and move onto the next packet in the chain.
      ---------------------------------------------------------------------*/
      if((param_ptr->unframe_mode == UNFRAMING) || (incoming_ptr->used == 0))
      {
        incoming_ptr = dsm_free_buffer(incoming_ptr);
      }
      else
      {
        /*-------------------------------------------------------------------
          If there is nothing in the original packet then make this item the
          original, otherwise append it to he end of the existing chain and
          change the tail pointer accordingly
        -------------------------------------------------------------------*/
        if ((param_ptr->orig_head_ptr) == NULL)
        {
          param_ptr->orig_head_ptr= param_ptr->orig_tail_ptr = incoming_ptr;
        }
        else
        {
          ASSERT( (param_ptr->orig_tail_ptr) != NULL);
          (param_ptr->orig_tail_ptr)->pkt_ptr = incoming_ptr;
          param_ptr->orig_tail_ptr = incoming_ptr;
        }
        /*-------------------------------------------------------------------
          Make sure the orig_tail_ptr->pkt_ptr is set to NULL, otherwise
          the end of the orig_head_ptr might keep pointing to the rest of
          the incoming_ptr chain.
        -------------------------------------------------------------------*/
        incoming_ptr = incoming_ptr->pkt_ptr;
        param_ptr->orig_tail_ptr->pkt_ptr = NULL;
      } /* if not UNFRAMING */

      cur_byte_loc = 0;
     } /* if incoming_ptr traversed completely */

    } /* if incoming_ptr is not NULL */
  } /* main while loop */
  /*lint +e794 */

} /* pppi_hdlc_sw_unframe() */



#endif /* MSM5000_IRAM_FWD */

#ifdef FEATURE_HDLC_HW_ACCEL
/*===========================================================================
FUNCTION PPPI_HDLC_HW_UNFRAME()

DESCRIPTION
  This function will Packetize PPP input from asynchronous device, and
  unframe the packets using the hardware deframer block. Currently, this
  should ONLY be called for sockets and full network model calls. It will
  pass the deframed packet up to pppi_incoming for processing.

DEPENDENCIES
  None

RETURN VALUE
  None

SIDE EFFECTS
  None
===========================================================================*/
void pppi_hdlc_hw_unframe
(
  ppp_dev_enum_type  device,        /* device (Rm or Um) pkt arrived on   */
  dsm_item_type    **item_head_ptr /* address of ptr to a dsm_item_type */
)
{
  hdlc_unframe_var_type *param_ptr;    /* Ptr to static vars for unframing */
  dsm_item_type *incoming_ptr  = NULL;         /* Ptr to passed data item  */
  dsm_item_type *temp_item_ptr = NULL;         /* Ptr to hold temp buffer  */
  uint8         *incoming_data_ptr;             /* Ptr to incoming data    */
  int            deframed_size;
  int            deframed_num_hdrs;
  int            i;
  int            ppp_pkt_size;
  uint32         ppp_hdr;
  uint16         bytes_to_read;
  uint16         bytes_left;
  uint16         bytes_left_in_tail;
  uint16         cur_byte_loc = 0;  /* current offset in incoming_data_ptr */
  dsm_mempool_id_enum_type pool_id;
  boolean        mem_avail = TRUE;

  /* - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - */
  if(device >= PPP_MAX_DEV   ||
     (item_head_ptr == NULL) ||
     ((*item_head_ptr) == NULL))
  {
    ASSERT( 0);
    return;
  }

#ifdef FEATURE_DATA_PS_PPP_LOGGING
  ppp_log_frames_ext (*item_head_ptr, PPP_LOG_FRAMED_RX, device);
#endif /* FEATURE_DATA_PS_PPP_LOGGING */

  /*-------------------------------------------------------------------------
    Initialize variables. Incoming_ptr points to the current item in the
    incoming packet chain; orig_tail_ptr is the pointer to the last item
    in the unchanged packet chain; cur_byte_loc is the location of the byte
    being processed in the incoming_ptr->data_ptr.
  -------------------------------------------------------------------------*/
  incoming_ptr = *item_head_ptr;
  MSG_LOW("pppi_hdlc_hw_unframe() called for device %d", device, 0, 0);

  param_ptr = &(ppp_cb_array[device].input_params);
  cur_byte_loc =0;

#ifdef FEATURE_DS_MOBILE_IP
  /*-------------------------------------------------------------------------
    Let MIP code know that data is being transferred over Um interace
  -------------------------------------------------------------------------*/
  if ( device == PPP_UM_SN_DEV )
  {
    MIP_DATA_TRANSFER_ACTION;
  }
#endif

  /*-------------------------------------------------------------------------
    Process each item in the incoming packet chain according to the unframe
    mode.
  -------------------------------------------------------------------------*/
  while (incoming_ptr != NULL)
  {
    MSG_LOW("Processing %d bytes for device %d",
            incoming_ptr->used, device, 0);

    /*-----------------------------------------------------------------------
      Unframe the contents of this min(this DSM buffer, deframer input len)
      only. deframer input len is set so that the deframer output buffer
      should not overflow
    -----------------------------------------------------------------------*/
    incoming_data_ptr = incoming_ptr->data_ptr;
    if((incoming_ptr->used - cur_byte_loc) > HDLC_MAX_DEFRAMER_INPUT_SIZE)
    {
      hdlc_write_buf((byte *) ((byte *) incoming_data_ptr + cur_byte_loc),
                     HDLC_DEFRAMER_DATA_BUF, 
                     HDLC_MAX_DEFRAMER_INPUT_SIZE);
      cur_byte_loc += HDLC_MAX_DEFRAMER_INPUT_SIZE;
    }
    else
    {
      hdlc_write_buf((byte *) ((byte *) incoming_data_ptr + cur_byte_loc),
                     HDLC_DEFRAMER_DATA_BUF,
                     (incoming_ptr->used - cur_byte_loc));
      cur_byte_loc = incoming_ptr->used;
    }
    hdlc_deframer_cmd(HDLC_DEFRAMER_WRITE_DONE);

    /*-----------------------------------------------------------------------
      If we get an overflow in the deframer output buffer (hopefully it
      should never happen), ignore it, reset the deframer and proceed. We
      will lose some PPP packets (the deframer will indicate bad CRC), but
      it will recover after the end of the current PPP packet.
    -----------------------------------------------------------------------*/
    if(!HDLC_DEFRAMER_NO_ERR())
    {
      ERR("HW deframer buffer overflow!",0,0,0);
      dsm_free_packet(&(param_ptr->pkt_head_ptr));
      param_ptr->pkt_head_ptr = param_ptr->pkt_tail_ptr = NULL;
      hdlc_deframer_cmd(HDLC_DEFRAMER_RESET);
    }

    /*-----------------------------------------------------------------------
      Read all the PPP packets present in this RLP block
      The output of the deframer will have a header for each PPP packet
      indicating whether the PPP packet has ended (ie: 7E is found),
      whether the CRC passed and the number of bytes in the output PPP packet

      Also, there is a count of number of headers present in the deframer
      output. Note that sometimes the deframer gives a header with zero
      bytes in the PPP packet. Check for the packet size when doing the
      processing below.
    -----------------------------------------------------------------------*/
    deframed_size = HDLC_DEFRAMER_GET_BYTE_CNT();
    deframed_num_hdrs = HDLC_DEFRAMER_GET_HEADER_CNT();

    for(i = 0; i < deframed_num_hdrs; i++)
    {
      HDLC_DEFRAMER_READ_HEADER(ppp_hdr);
      ppp_pkt_size = HDLC_DEFRAMER_GET_PPP_LENGTH(ppp_hdr);

      if(ppp_pkt_size > 0)
      {
        /*-------------------------------------------------------------------
          Read the contents of this PPP (maybe partial) frame into DSM
          item(s). We need a loop to handle multiple DSM items
        -------------------------------------------------------------------*/
        bytes_left = ppp_pkt_size;

        do
        {
          /*-----------------------------------------------------------------
            If we are still building a pkt, don't get another DSM item until
            we have filled the current tail item.
          -----------------------------------------------------------------*/
          if(param_ptr->tail_full == TRUE)
          {
            pool_id = DSM_DS_SMALL_ITEM_POOL; 
            if(DSM_DONT_EXCEED_ITEMS(pool_id))
            {
              pool_id = DSM_DS_OTHER_POOL_ID(pool_id);
              if(DSM_DONT_EXCEED_ITEMS(pool_id))
              {
                mem_avail = FALSE;
              }
            }

            if(mem_avail == FALSE ||
               (temp_item_ptr = dsm_new_buffer(pool_id)) == NULL )
            {
              ERR("out of memory: small=%d large=%d",
                  DSM_POOL_FREE_CNT(DSM_DS_SMALL_ITEM_POOL),
                  DSM_POOL_FREE_CNT(DSM_DS_LARGE_ITEM_POOL),
                  0);
              param_ptr->tail_full = TRUE;  
              dsm_free_packet(&incoming_ptr);
              dsm_free_packet(&(param_ptr->pkt_head_ptr));
              param_ptr->pkt_head_ptr = param_ptr->pkt_tail_ptr = NULL;
              hdlc_deframer_cmd(HDLC_DEFRAMER_RESET);

              return;
            }

#ifdef FEATURE_DSM_MEM_CHK
            temp_item_ptr->tracer = DSM_CREATED_IN_ASY_IN;
#endif
            DSM_FOUR_BYTE_ALIGN(temp_item_ptr);

            /*-----------------------------------------------------------------
              Append this (partial) PPP frame to the previous partially 
              unframed packet, if any.
            -----------------------------------------------------------------*/
            if(param_ptr->pkt_head_ptr == NULL)
            {
              param_ptr->pkt_head_ptr = temp_item_ptr;
              param_ptr->pkt_tail_ptr = param_ptr->pkt_head_ptr;
            }
            else
            {
              param_ptr->pkt_tail_ptr->pkt_ptr = temp_item_ptr;
              param_ptr->pkt_tail_ptr = temp_item_ptr;
            }

            bytes_to_read = (bytes_left > temp_item_ptr->size)?
                            (temp_item_ptr->size):(bytes_left);

          }
          else
          {
            /*-----------------------------------------------------------------
              There is still room in the tail item, determine how much to
              read into it.
            -----------------------------------------------------------------*/
            bytes_left_in_tail =
              param_ptr->pkt_tail_ptr->size - param_ptr->pkt_tail_ptr->used;
            bytes_to_read =
              (bytes_left > bytes_left_in_tail) ? 
               bytes_left_in_tail : bytes_left;
          }

          bytes_left -= bytes_to_read;

          hdlc_read_buf(
           (param_ptr->pkt_tail_ptr->data_ptr + param_ptr->pkt_tail_ptr->used),
           HDLC_DEFRAMER_DATA_BUF, bytes_to_read);

          param_ptr->pkt_tail_ptr->used += bytes_to_read;

          /*-----------------------------------------------------------------
            Update the tail_full flag as to whether we need to get another
            DSM item for this packet, or there is still room in the current
            tail item.

            If the number of bytes in the current tail item is not a multiple
            of 4, grab a new DSM item, since the buffer passed to 
            hdlc_read_buf must be aligned to a 4-byte boundary.
          -----------------------------------------------------------------*/
          if((param_ptr->pkt_tail_ptr->used < param_ptr->pkt_tail_ptr->size)
             && ((param_ptr->pkt_tail_ptr->used & 0x3) == 0)
            )
          {
            param_ptr->tail_full = FALSE;
          }
          else
          {
            param_ptr->tail_full = TRUE;
          }

          ASSERT(param_ptr->pkt_tail_ptr->used <=
                 param_ptr->pkt_tail_ptr->size);
          ASSERT(param_ptr->pkt_tail_ptr->size > 0);
        } while( bytes_left > 0);

#ifdef FEATURE_DATA_PS_PPP_LOGGING
        ppp_log_frames_ext (param_ptr->pkt_head_ptr,
                            PPP_LOG_UNFRAMED_RX,
                            device);
#endif /* FEATURE_DATA_PS_PPP_LOGGING */
      } /*  if(ppp_pkt_size > 0) */

      /*---------------------------------------------------------------------
        If we have reached the end of the PPP packet, send it to
        pppi_incoming if CRC passes.
      ---------------------------------------------------------------------*/
      if(HDLC_DEFRAMER_PPP_ENDS(ppp_hdr))
      {
        if(param_ptr->pkt_head_ptr != NULL)
        {
          if(!HDLC_DEFRAMER_CRC_IS_VALID(ppp_hdr))
          {
            ERR("Dev(%d) Incoming PPP Checksum Error", device, 0, 0);
            pppi_skipped(device, &(param_ptr->pkt_head_ptr));
          }
          else
          {
#ifdef FEATURE_DSM_MEM_CHK
            /*---------------------------------------------------------------
              Set tracer to indicate where the item is being stored
            ---------------------------------------------------------------*/
            (param_ptr->pkt_head_ptr)->tracer = DSM_PPP_INCOMING;
#endif
            /*---------------------------------------------------------------
              Remove the CRC. If the packet is too short (we need at least 3 
              bytes - 2 for CRC+1 for protocol field), we will be left with
              a NULL pointer after removing 2 CRC bytes. Discard the packet
              if that's the case otherwise pass it to the upper layer. 
            ---------------------------------------------------------------*/
            dsm_pullup_tail(&(param_ptr->pkt_head_ptr), NULL, 2);
            
            if(param_ptr->pkt_head_ptr == NULL)
            {
              ERR("Dev(%d) Incoming pkt too short", device, 0, 0);
              pppi_skipped(device, &(param_ptr->pkt_head_ptr));
            }
            else
            {
              pppi_incoming(device, &param_ptr->pkt_head_ptr);
            }
          }
        }
        else
        {
          ERR("deframer returned PPP hdr without data!",0,0,0);
        }
        param_ptr->pkt_head_ptr = NULL;
        param_ptr->pkt_tail_ptr = NULL;
        param_ptr->tail_full    = TRUE;
      } /* if(HDLC_DEFRAMER_PPP_ENDS(ppp_hdr)) */

      deframed_size -= (ppp_pkt_size + 4); /* 4 bytes for the header */
    } /* for(i = 0; i < deframed_num_hdrs; i++) */

    ASSERT(deframed_size >= 0); /* Sanity check */

    /*-----------------------------------------------------------------------
      Move to the next DSM item in the incoming packet, if this item has
      been processed completely.
    -----------------------------------------------------------------------*/
    if(cur_byte_loc == incoming_ptr->used)
    {
      incoming_ptr = dsm_free_buffer(incoming_ptr);
      cur_byte_loc = 0;
    }
  } /* while (incoming_ptr != NULL) */
} /* pppi_hdlc_hw_unframe() */
#endif /* FEATURE_HDLC_HW_ACCEL */



/*===========================================================================
FUNCTION PPPI_INCOMING()

DESCRIPTION
  This function will process the PPP packet passed by the underlying device.
  Makes sure that the PFC and ACFC compression are not on when they shouldn't
  be. For modes other than INTERNAL, calls the snooping callback if one is 
  registered and passes pkt to the correct destination based upon the return 
  value from the callback. When in INTERNAL mode, it passes pkt up the stack 
  for protocol procesing.

DEPENDENCIES
  None

RETURN VALUE
  None

SIDE EFFECTS
  None
===========================================================================*/
void pppi_incoming
(
   ppp_dev_enum_type device,           /* ppp device pkt arrived on        */
   dsm_item_type **item_ref_ptr        /* Pointer to ref to pkt            */
)
{
  ppp_type *ppp_cb_ptr;
  uint16      protocol;               /* protocol of packet                */
  negotiate_t negotiated = FALSE;     /* what's been negotiated            */
  snoop_cb_ret_val_enum_type snoop_cb_res;  /* return value from snoop cb  */

  /* - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - */
  if(device >= PPP_MAX_DEV ||
     item_ref_ptr == NULL  ||
     (*item_ref_ptr) == NULL)
  {
    ASSERT(0);
    return;
  }
  if((*item_ref_ptr)->used == 0)
  {
    dsm_free_packet(item_ref_ptr);
    ASSERT(0);
    return;
  }

  ppp_cb_ptr = &(ppp_cb_array[device]);

  /*-------------------------------------------------------------------------
    Use negotiated values if LCP finished
  -------------------------------------------------------------------------*/
  if(ppp_cb_ptr->fsm[LCP].state == fsmOPENED)
  {
    negotiated = ppp_cb_ptr->fsm[LCP].local.work;
  }

  /*-------------------------------------------------------------------------
    If hdlc_mode == HW or SW, then the incoming pkt was HDLC framed, we only
    care about this if we are in internal mode.
  -------------------------------------------------------------------------*/
  if(ppp_cb_ptr->hdlc_mode != HDLC_OFF)
  {
    /*-----------------------------------------------------------------------
      HDLC address and control fields may be compressed out.  IFF the first
      byte is not the HDLC_ALL_ADDR byte (FF) then it is compressed out.
      Check if compression is enabled.  Error checking should only be done
      when PPP is in INTERNAL mode.
    -----------------------------------------------------------------------*/
    if((uint8)(*item_ref_ptr)->data_ptr[0] != HDLC_ALL_ADDR)
    {
      if(ppp_cb_ptr->mode == PPP_INTERNAL_MODE &&
         !(negotiated & LCP_N_ACFC))
      {
        MSG_HIGH( "Missing ALL Address", 0, 0, 0);
        pppi_skipped(device,  item_ref_ptr);
        if_cb.InErrors++;
        return;
      }
    } /* if(first byte != FF) */

    /*-----------------------------------------------------------------------
      The first byte was FF: if the second byte is HDLC_UI (03) - if not then
      Check if compression is enabled.  Error checking should only be done
      when PPP is in INTERNAL mode.
    -----------------------------------------------------------------------*/
    else if((uint8)(*item_ref_ptr)->data_ptr[1] != HDLC_UI)
    {
      if(ppp_cb_ptr->mode == PPP_INTERNAL_MODE &&
         (!(negotiated & LCP_N_ACFC) ||
          !(negotiated & LCP_N_PFC)))
      {
        MSG_HIGH( "Missing UI", 0, 0, 0);
        pppi_skipped(device,  item_ref_ptr);
        if_cb.InErrors++;
        return;
      }
    }
    /*-----------------------------------------------------------------------
      the first two bytes are FF03, strip them out
    -----------------------------------------------------------------------*/
    else
    {
      dsm_pull16(item_ref_ptr);     /* skip address/control fields */
    }
  } /* if(ppp_cb_ptr->hdlc_mode != HDLC_OFF) */

  /*-------------------------------------------------------------------------
    Initialize the expected header
  -------------------------------------------------------------------------*/
  protocol = dsm_pull8(item_ref_ptr);

  /*-------------------------------------------------------------------------
    First byte of PPP protocol field may be compressed out.  If the LS bit is
    1 then this indicates that the upper protocol byte is compressed out, as
    only the lower byte should be odd (i.e. upper should be even).  Check if
    compression is enabled.  In the case of network model builds the error
    checking should only be done when PPP is in INTERNAL mode.
  -------------------------------------------------------------------------*/
  if( protocol & 0x01)
  {
    if(ppp_cb_ptr->mode == PPP_INTERNAL_MODE &&
      !(negotiated & LCP_N_PFC))
    {
      MSG_HIGH( "Missing Upper Protocol Byte", 0, 0, 0);
      pppi_skipped(device,  item_ref_ptr);
      if_cb.InErrors++;
      return;
    }
  }

  /*-------------------------------------------------------------------------
    Upper protocol byte is NOT compressed out check that lower is correct
  -------------------------------------------------------------------------*/
  else
  {
    protocol = (protocol << 8) | dsm_pull8(item_ref_ptr);

    /*-----------------------------------------------------------------------
      Second byte of PPP protocol field must be odd
    -----------------------------------------------------------------------*/
    if( !(protocol & 0x01))
    {
      MSG_HIGH( "Missing Lower Protocol Byte", 0, 0, 0);
      pppi_skipped(device,  item_ref_ptr);
      if_cb.InErrors++;
      return;
    }
  }

  /*-------------------------------------------------------------------------
    If the input interface is not in PPP_INTERNAL_MODE, call the function
    to detect the PPP protocol and message and call the callbacks.
    In PPP_INTERNAL_MODE, the PPP packets are processed internally.
  -------------------------------------------------------------------------*/
  if(ppp_cb_ptr->mode != PPP_INTERNAL_MODE)
  {
    do
    {
      snoop_cb_res = snoop_proto_msg_detect(device, item_ref_ptr, protocol);
    } while( snoop_cb_res == SNOOP_CB_SNOOP_AGAIN);

    /*-----------------------------------------------------------------------
      If we are asked to forward the packet, send it to the bridged PPP dev.
      If we are asked to ignore the pakcket, it means the packet has been 
      completely processed by the callback and we are done in this function. 
      Otherwise we will pass the packet up the stack for further processing.
    -----------------------------------------------------------------------*/
    if(snoop_cb_res == SNOOP_CB_FORWARD_PKT)
    {
      if(ppp_cb_ptr->bridge_dev >= PPP_MAX_DEV)
      {
        ERR("Dev %d cann't forward pkt to invalid dev %d",
            device, ppp_cb_ptr->bridge_dev, 0);
        dsm_free_packet(item_ref_ptr);
      }

#ifdef FEATURE_DSM_MEM_CHK
    	(*item_ref_ptr)->tracer = DSM_PPP_TX_WM;
#endif //FEATURE_DSM_MEM_CHK
      
      pppi_outgoing(ppp_cb_ptr->bridge_dev, protocol, item_ref_ptr, NULL);
      return;
    }
    else if(snoop_cb_res == SNOOP_CB_IGNORE_PKT)
    {
      /*---------------------------------------------------------------------
        This packet has been unframed so update statistics
      ---------------------------------------------------------------------*/
      if_cb.UnframedPkts[device] += 1;
      if_cb.TotalPkts[device]    += 1;

      return;
    }
  }

#ifdef FEATURE_DS_MOBILE_IP
  /*-------------------------------------------------------------------------
    If we are in a MIP laptop call, check if we need to forward the packet
    to the laptop or push it up the stack.
    
    NOTE: This is not a very nice way to check if this is a laptop or 
    sockets call, should be done in a more generic/mode independent fashion.
    More appropriate way to check this condition would be to see if the UM 
    interface (can be obtained from PPP device on UM) is in ROUTEABLE state.
  -------------------------------------------------------------------------*/
  if ((device == PPP_UM_SN_DEV) && (ds707_rmsm_is_packet_call()))
  {
    /*----------------------------------------------------------------------
      If Mobile IP is active and this packet is not related to Mobile IP or
      PPP then forward it to the laptop.
    ----------------------------------------------------------------------*/
    if( mip_meta_sm_open() &&
        !snoop_is_registration_pkt(item_ref_ptr, protocol) )
    {
      /*--------------------------------------------------------------------
        Forward non-registration packets to the laptop 
        (Rather than flow controlling.  This will give better throughput.)
      --------------------------------------------------------------------*/
      MSG_MED("Sending pkt to laptop",0,0,0);
      (void)pppi_outgoing(PPP_RM_DEV, protocol, item_ref_ptr, NULL);
      return;
    }
    else
    {
      MSG_MED("Not fwd'ing pkt to laptop. Going up the stack!",0,0,0);
    }
  }
#endif /* FEATURE_DS_MOBILE_IP */

  /*-------------------------------------------------------------------------
    If we reach this far, then the packet belongs to the stack in the 
    phone.  Pass the packet up to perform protocol specific processing.
  -------------------------------------------------------------------------*/
  pppi_receive(device, protocol, item_ref_ptr);
} /* pppi_incoming() */


/*===========================================================================
FUNCTION PPPI_RECEIVE()

DESCRIPTION
  This is the PPP packet receiving function in the stack. It processes each 
  PPP packet based upon the protocol type and pass it up to the appropriate 
  protocol engine in the stack.

DEPENDENCIES
  None

RETURN VALUE
  None

SIDE EFFECTS
  None
===========================================================================*/
void pppi_receive
(
  ppp_dev_enum_type device,           /* ppp device pkt arrived on         */
  uint16            protocol,               /* protocol of packet          */
  dsm_item_type     **item_ref_ptr        /* Pointer to ref to pkt         */
)
{
  ppp_type *ppp_cb_ptr;
  struct      ipcp_pdv_s *ipcp_p;     /* Pointer to IPCP control block     */
  uint16      size;
  /* - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - */

  if( item_ref_ptr == NULL || *item_ref_ptr == NULL )
  {
    MSG_ERROR("Invalid args", 0, 0, 0);
    return;
  }

  if( device >= PPP_MAX_DEV )
  {
    MSG_ERROR("Invalid dev", 0, 0, 0);
    dsm_free_packet(item_ref_ptr);
    return;
  }

  ppp_cb_ptr = &(ppp_cb_array[device]);
  if( ppp_cb_ptr->session_handle == 0 ||
      ppp_cb_ptr->mode == PPP_NONE_MODE || ppp_cb_ptr->rx_iface_ptr == NULL)
  {
    MSG_HIGH("PPP %d not configured discarding pkt", device, 0, 0);
    dsm_free_packet(item_ref_ptr);
    return;
  }

  /*-------------------------------------------------------------------------
    count FCS and final flag (FSC 2 bytes + Flag 1 byte = 3 bytes
    What about Address, control and protocol field??
  -------------------------------------------------------------------------*/
  size = dsm_length_packet(*item_ref_ptr) + 3;

  ppp_cb_ptr->InGoodOctets += size;

  switch(protocol)
  {

    case PPP_PAP_PROTOCOL:
      /*---------------------------------------------------------------------
        Make sure we are in the auth phase, as that is the only place for PAP
      ---------------------------------------------------------------------*/
      if(ppp_cb_ptr->phase != pppAUTHENTICATE)
      {
        MSG_HIGH( "dev %d in %d not auth(%d) phase!  ",
                  device,
                  ppp_cb_ptr->phase,
                  pppAUTHENTICATE);
        if_cb.InDiscards++;
        pppi_skipped(device,  item_ref_ptr);
        break;
      }

      ppp_cb_ptr->InGoodOctets += size;
      pap_proc(device, item_ref_ptr);
      break;

    case PPP_CHAP_PROTOCOL:
      /*---------------------------------------------------------------------
        Chap can happen at any time - so don't check the phase.
      ---------------------------------------------------------------------*/
      ppp_cb_ptr->InGoodOctets += size;
      chap_proc(device, item_ref_ptr);
    break;

    case PPP_IPCP_PROTOCOL:
      /*---------------------------------------------------------------------
        if we receive and IPCP packet but still in the authenticate phase, we
        need to determine if it is OK to move on to the network phase.
      ---------------------------------------------------------------------*/
      if(ppp_cb_ptr->phase == pppAUTHENTICATE)
      {
        /*-------------------------------------------------------------------
          If we were authenticating, and the success flag is NOT set then we
          can't move onto the next stage, so free the packet and break out of
          this processing.  NOTE: This will only happen if we don't get the
          authentication information from the peer.
        -------------------------------------------------------------------*/
        if((ppp_cb_ptr->auth.flags & PPP_AP_REMOTE) &&
           !(ppp_cb_ptr->auth.flags & PPP_AP_SUCCESS))
        {
          dsm_free_packet(item_ref_ptr);
          break;
        }

        /*-------------------------------------------------------------------
          It is OK to Move on, so complete auth processing for appropriate
          protocol.
        ---------------------------------------------------------------------*/
        else if(ppp_cb_ptr->auth.mode == PPP_DO_PAP)
        {
          ppp_auth_complete(ppp_cb_ptr, PPP_PAP_PROTOCOL);
        }

        else if(ppp_cb_ptr->auth.mode == PPP_DO_CHAP)
        {
          ppp_auth_complete(ppp_cb_ptr, PPP_CHAP_PROTOCOL);
        }
      } /* if(in auth phase) */

      /*---------------------------------------------------------------------
        If device is configured to not do IPCP, send a protocol reject,
        else process the packet
      ----------------------------------------------------------------------*/
      if(ppp_cb_ptr->fsm[IPCP].mode == PPP_CP_DISABLED)
      {
        pppi_send_prot_rej(device, protocol, *item_ref_ptr);
      }
      else
      {
        if(ppp_cb_ptr->phase != pppNETWORK)
        {
          MSG_HIGH( "Not Ready for IPCP Traffic", 0, 0, 0);
          if_cb.InDiscards++;
          pppi_skipped(device, item_ref_ptr);
          break;
        }
        if_cb.InUnicast++;
        ppp_cb_ptr->InNCP[IPcp]++;
        fsm_proc(&(ppp_cb_ptr->fsm[IPcp]), *item_ref_ptr);
      } /* else(IPCP is available) */
      break;

    case PPP_IP_PROTOCOL:
      /*---------------------------------------------------------------------
        Got Regular IP, make sure we are ready for it, then update stats and
        process it.
        If device is configured to not do IPCP, send a protocol reject,
      ---------------------------------------------------------------------*/
      if(ppp_cb_ptr->fsm[IPCP].mode == PPP_CP_DISABLED)
      {
        pppi_send_prot_rej(device, protocol, *item_ref_ptr);
      }
      else
      {
        if( ppp_cb_ptr->fsm[IPcp].state != fsmOPENED)
        {
          MSG_HIGH( "Not Open For IP Traffic", 0, 0, 0);
          if_cb.InDiscards++;
          pppi_skipped(device,  item_ref_ptr);
          break;
        }
        if_cb.InUnicast++;
        (*item_ref_ptr)->kind = DSM_PS_IP;
        ps_iface_input(ppp_cb_ptr->rx_iface_ptr, item_ref_ptr, NULL);
      }
      break;

    case PPP_VJCH_PROTOCOL:
      /*---------------------------------------------------------------------
        Got Van Jacobson Compressed TCP/IP, make sure that we are ready for
        it, support it, and have state for it.  Then update stats and process.

        If device is configured to not do IPCP, send a protocol reject,
      ---------------------------------------------------------------------*/
      if(ppp_cb_ptr->fsm[IPCP].mode == PPP_CP_DISABLED)
      {
        pppi_send_prot_rej(device, protocol, *item_ref_ptr);
      }
      else
      {
        if( ppp_cb_ptr->fsm[IPcp].state != fsmOPENED)
        {
          MSG_HIGH( "Not Open for Compressed TCP/IP", 0, 0, 0);
          pppi_skipped(device,  item_ref_ptr);
          if_cb.InDiscards++;
          break;
        }

        if(!(ppp_cb_ptr->fsm[IPcp].local.work & IPCP_N_COMPRESS))
        {
          MSG_HIGH( "Compressed TCP/IP Not Enabled", 0, 0, 0);
          pppi_skipped(device,  item_ref_ptr);
          if_cb.InDiscards++;
          break;
        }

        ipcp_p = (ipcp_pdv_type*)(ppp_cb_ptr->fsm[IPcp].pdv);
        if( slhc_uncompress(ipcp_p->slhcp, item_ref_ptr) <= 0)
        {
          MSG_HIGH( "Compressed TCP/IP Packet Err", 0, 0, 0);
          pppi_skipped(device,  item_ref_ptr);
          if_cb.InErrors++;
          break;
        }
        if_cb.InUnicast++;
        (*item_ref_ptr)->kind = DSM_PS_IP;
        ps_iface_input(ppp_cb_ptr->rx_iface_ptr, item_ref_ptr, NULL);
      }
      break;

    case PPP_VJUCH_PROTOCOL:
      /*---------------------------------------------------------------------
        Get Van Jacobson Uncompressed TCP/IP, make sure that we are ready for
        it, support it, and can store state for it.  Then update stats and
        process.

        If device is configured to not do IPCP, send a protocol reject,
      ---------------------------------------------------------------------*/
      if(ppp_cb_ptr->fsm[IPCP].mode == PPP_CP_DISABLED)
      {
        pppi_send_prot_rej(device, protocol, *item_ref_ptr);
      }
      else
      {
        if( ppp_cb_ptr->fsm[IPcp].state != fsmOPENED)
        {
          MSG_HIGH( "Not Open for Uncompressed TCP/IP", 0, 0, 0);
          pppi_skipped(device,  item_ref_ptr);
          if_cb.InDiscards++;
          break;
        }

        if(!(ppp_cb_ptr->fsm[IPcp].local.work & IPCP_N_COMPRESS))
        {
          MSG_HIGH( "Uncompressed TCP/IP Not Enabled", 0, 0, 0);
          pppi_skipped(device, item_ref_ptr);
          if_cb.InDiscards++;
          break;
        }

        ipcp_p = (ipcp_pdv_type*)(ppp_cb_ptr->fsm[IPcp].pdv);
        if( slhc_remember(ipcp_p->slhcp, item_ref_ptr) <= 0)
        {
          MSG_HIGH( "Uncompressed TCP/IP Packet Err", 0, 0, 0);
          dsm_free_packet(item_ref_ptr);
          if_cb.InErrors++;
          break;
        }
        if_cb.InUnicast++;
        (*item_ref_ptr)->kind = DSM_PS_IP;
        ps_iface_input(ppp_cb_ptr->rx_iface_ptr, item_ref_ptr, NULL);
      }
      break;

    case PPP_LCP_PROTOCOL:
      /*---------------------------------------------------------------------
        Got LCP packet, update stats and process it
      ---------------------------------------------------------------------*/
      if_cb.InUnicast++;
      ppp_cb_ptr->InNCP[LCP]++;
      fsm_proc(&(ppp_cb_ptr->fsm[LCP]),*item_ref_ptr);
      break;

    default:
      /*---------------------------------------------------------------------
        Unknown protocol, update stats and generate protocol reject.
      ---------------------------------------------------------------------*/
      ppp_cb_ptr->InGoodOctets -= size;
      pppi_send_prot_rej(device, protocol, *item_ref_ptr);
      break;

  } /* switch(protocol) */

} /* pppi_receive() */



/*===========================================================================
                           PPP PROTOCOL PROCESSING
===========================================================================*/
/*===========================================================================
FUNCTION PPPI_DISCARD()

DESCRIPTION
  This function will queue a PPP Discard packet.

DEPENDENCIES
  None

RETURN VALUE
  None

SIDE EFFECTS
  None
===========================================================================*/
int pppi_discard
(
  ppp_dev_enum_type device,
  dsm_item_type    *item_ptr
)
{
  /* - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - */

  if( device >= PPP_MAX_DEV )
  {
    dsm_free_packet(&item_ptr);
    return -1;
  }
  return
    fsm_send(&(ppp_cb_array[device].fsm[LCP]), DISCARD_REQ, 0, item_ptr);

} /* pppi_discard() */



/*=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=

                                MISCILANEOUS

=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=*/
/*===========================================================================
FUNCTION PPPI_PS_IFACE_GET_PEER_ADDR_CBACK()

DESCRIPTION
  This function is intended to be registered with ps_iface to allow the
  determination of the PPP peer address.

  NOTE: This should only ever be called through the callback pointer in PS
        device.

PARAMETERS
  this_iface_ptr: ptr to interface control block on which to operate on.
  ip_addr_ptr:    value return - the address will be
  link_info:      void pointer that will contain the PPP device in question

RETURN VALUE
  None

DEPENDENCIES
  None

SIDE EFFECTS
  None
===========================================================================*/
void pppi_ps_iface_get_peer_addr_cback
(
  ps_iface_type *this_iface_ptr,
  ip_addr_type  *ip_addr_ptr,
  void          *link_info
)
{
  ppp_type        *ppp_cb_ptr  = (ppp_type*)link_info;
  ipcp_value_type *ipcp_value_ptr;
/*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/
  ASSERT(this_iface_ptr != NULL);
  ASSERT(ip_addr_ptr    != NULL);
  ASSERT(link_info      != NULL);
  ASSERT(ppp_cb_ptr     != NULL);

  /*-------------------------------------------------------------------------
    Make sure that the interface this was called with corresponds with the
    device the PPP device this is associated with.
  -------------------------------------------------------------------------*/
  if(this_iface_ptr != ppp_cb_ptr->rx_iface_ptr)
  {
    MSG_ERROR("ps_iface(%d:%d) ppp(%d) mismatch!",
              this_iface_ptr->name,
              this_iface_ptr->instance,
              ppp_cb_ptr - ppp_cb_array);
    return;
  }

  /*-------------------------------------------------------------------------
    Get the address that was requested and populate the IP address struct.
  -------------------------------------------------------------------------*/
  ipcp_value_ptr = (ipcp_value_type*)ppp_cb_ptr->fsm[IPCP].remote.work_pdv;
  switch(ip_addr_ptr->type)
  {
  case IP_ANY_ADDR:
    /*-----------------------------------------------------------------------
      If the caller didn't specify an address type, the v4 address will be
      returned if set otherwise the v6 address will be returned if it is set
    -----------------------------------------------------------------------*/
    if(ipcp_value_ptr->address != 0)
    {
      ip_addr_ptr->type = IPV4_ADDR;
      ip_addr_ptr->addr.v4 = ipcp_value_ptr->address;
      break;
    }
    /* fall through */

  case IPV6_ADDR:
    /*-----------------------------------------------------------------------
      return the invalid address, as we don't currently support v6
    -----------------------------------------------------------------------*/
    ip_addr_ptr->type = IP_ADDR_INVALID;
    break;

  case IPV4_ADDR:
    /*-----------------------------------------------------------------------
      return the v4 address
    -----------------------------------------------------------------------*/
    ip_addr_ptr->addr.v4 = ipcp_value_ptr->address;
    break;

  default:
    ASSERT(0);

  } /* switch(requested address type) */

} /* pppi_ps_iface_get_peer_addr_cback() */


/*===========================================================================
FUNCTION PPPI_DEFAULT_RX()

DESCRIPTION
  This function will simply return NULL indicating to the caller that there
  was no data to receive.

PARAMETERS
  None

RETURN VALUE
  NULL

DEPENDENCIES
  None

SIDE EFFECTS
  None
===========================================================================*/
dsm_item_type *pppi_default_rx
(
  void *user_data
)
{
/*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/
  MSG_MED("pppi_default_rx()!", 0, 0, 0);
  return NULL;

} /* pppi_default_rx() */



/*===========================================================================
FUNCTION PPPI_DEFAULT_TX()

DESCRIPTION
  This function will free any packet that is passed in.

PARAMETERS
  item_ref_ptr: pointer to a reference to a dsm item chain.

RETURN VALUE
  None

DEPENDENCIES
  None

SIDE EFFECTS
  None
===========================================================================*/
void pppi_default_tx
(
  dsm_item_type **item_ref_ptr,
  void *user_data
)
{
/*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/
  MSG_MED("pppi_default_tx() freeing pkt!", 0, 0, 0);
  /*-------------------------------------------------------------------------
    if the App field is set then meta_info is present, so free it
  -------------------------------------------------------------------------*/
  if((*item_ref_ptr)->app_field  != NULL)
  {
    PS_META_INFO_FREE((ps_meta_info_type**)&((*item_ref_ptr)->app_field));
  }
  dsm_free_packet(item_ref_ptr);

} /* pppi_default_tx() */



/*===========================================================================
FUNCTION PPPI_GET_DATA_CBACK()

DESCRIPTION
  This function is intended to be registered with the task that is providing
  the execution context for PPP.  This will call asy_in() if HDLC is enabled,
  otherwise we call pppi_incoming() directly.

PARAMETERS
  ppp_intance: the value of the PPP instance is expected to be stopred in the
  void*, that is this should be used as memory NOT as a pointer.

RETURN VALUE
  true: 

DEPENDENCIES
  None

SIDE EFFECTS
  None
===========================================================================*/
boolean pppi_get_data_cback
(
  ps_sig_enum_type sig,                    /* Signal to be processed       */
  void *dev_instance                       /* user data, indicates the ppp */
)
{
  ppp_dev_enum_type device;
  ppp_type  *ppp_cb_ptr;
  dsm_item_type *ppp_data_ptr;
/*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  /*-------------------------------------------------------------------------
    Get the PPP instance we are supposed to be working with (the dev_instance
    is the offset into the ppp_cb_array.
  -------------------------------------------------------------------------*/
  device = ((ppp_dev_enum_type)((uint32)dev_instance));
  if(device >= PPP_MAX_DEV)
  {
    ASSERT(0);
    return FALSE;
  }

  ppp_cb_ptr = ppp_cb_array + device;

  /*-------------------------------------------------------------------------
    Get the next data item by calling the registered rx function: if there is 
    no more data available then return TRUE to indicate the bit can be 
    removed from the processing signal mask.
  -------------------------------------------------------------------------*/
  ppp_data_ptr = ppp_cb_ptr->rx_f_ptr(ppp_cb_ptr->rx_user_data);
  if(ppp_data_ptr == NULL)
  {
    /*-----------------------------------------------------------------------
      There is no data - so we can have the bit removed from the mask
    -----------------------------------------------------------------------*/
    return TRUE;
  }

  /*-------------------------------------------------------------------------
    If the mode of the PPP device is NONE then discard the packet right here
  -------------------------------------------------------------------------*/
  if(ppp_cb_ptr->mode == PPP_NONE_MODE ||
     ppp_cb_ptr->rx_iface_ptr == NULL)
  {
    MSG_HIGH("PPP %d not configured discading pkt", device, 0, 0);
    dsm_free_packet(&ppp_data_ptr);
    return FALSE;
  }

  /*-------------------------------------------------------------------------
    If the mode of the PPP device is in RELAY then fail, as this is not a
    valid configuration.
  -------------------------------------------------------------------------*/
  if(ppp_cb_ptr->mode == PPP_RELAY_MODE)
  {
    ASSERT(0);
    MSG_HIGH("PPP %d in RELAY mode discading pkt", device, 0, 0);
    dsm_free_packet(&ppp_data_ptr);
    return FALSE;
  }

#ifdef FEATURE_DSM_MEM_CHK
  /*-------------------------------------------------------------------------
    Set tracer field to indicate where the data item is being stored
  -------------------------------------------------------------------------*/
  // fix this: should probably be in the hdlc unframe function.
  ppp_data_ptr->tracer = DSM_UM_PPP_ASY_IN;
#endif

  /*-------------------------------------------------------------------------
    If HDLC is off then send the packet to pppi_incoming() otherwise send it
    to pppi_hdlc_unframe()
  -------------------------------------------------------------------------*/
  switch(ppp_cb_ptr->hdlc_mode)
  {
  case HDLC_OFF:
    pppi_incoming(device, &ppp_data_ptr);
    break;

#ifdef FEATURE_HDLC_HW_ACCEL
  case HDLC_HW:
    if(ppp_cb_ptr->mode == PPP_INTERNAL_MODE ||
       ppp_cb_ptr->mode == PPP_FULL_NET_MODE)
    {
      pppi_hdlc_hw_unframe(device, &ppp_data_ptr);
      break;
    }
    /* else - fall through */
#endif /* FEATURE_HDLC_HW_ACCEL */

  case HDLC_SW:
    pppi_hdlc_sw_unframe(device, &ppp_data_ptr);
    break;

  default:
    ASSERT(0);
  }

  /*-------------------------------------------------------------------------
    return FALSE indicating there may still be processing required - don't
    check if there is anything required, as that will introduce overhead in
    the loaded case.
  -------------------------------------------------------------------------*/
  return FALSE;

} /* pppi_get_data_cback() */



/*===========================================================================
FUNCTION PPPI_I_SETUP_TIMEOUT_CB()

DESCRIPTION
  This function is called when the setup timer expires

PARAMETERS
  arg_ptr: void pointer that was registered: the device instance

RETURN VALUE
  None

DEPENDENCIES
  None

SIDE EFFECTS
  will kill the PPP instance it is called for.
===========================================================================*/
void pppi_i_setup_timout_cb
(
  void *arg_ptr
)
{
  ppp_dev_enum_type device;

/*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  device = (ppp_dev_enum_type)(uint32)arg_ptr;
  if(device >= PPP_MAX_DEV)
  {
    ASSERT(0);
    return;
  }
  MSG_HIGH("PPP setup timer expired, stopping PPP %d", device, 0, 0);
  ppp_stop(device);

} /* pppi_i_setup_timout_cb() */



#ifdef FEATURE_DS_PSTATS
#error code not present
#endif /* FEATURE_DS_PSTATS */



#ifdef FEATURE_DS_PSTATS
#error code not present
#endif /* FEATURE_DS_PSTATS */

#ifdef FEATURE_DS_DEBUG_UI
#error code not present
#endif /* FEATURE_DS_DEBUG_UI */


/*=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=

                         LOCAL FUNCTION DEFINITIONS

=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=*/
/*===========================================================================
FUNCTION PPPI_SEND_PROT_REJ()

DESCRIPTION
  This function will send a protocol reject for an unsupported protocol.

  NOTE: this function is only used internally to this file.

DEPENDENCIES
  None

RETURN VALUE
  TRUE if PROT_REJ successfully sent, FALSE otherwise

SIDE EFFECTS
  None
===========================================================================*/
boolean pppi_send_prot_rej
(
 ppp_dev_enum_type device,             /* device (Rm or Um) pkt arrived on   */
 uint16 protocol,                     /* Bad protocol for which to PROT_REJ*/
 dsm_item_type *item_ptr              /* Pointer to the packet             */
)
{
  ppp_type *ppp_cb_ptr;

  if( item_ptr == NULL )
  {
    MSG_ERROR("Invalid DSM", 0, 0, 0);
    return FALSE;
  }

  if( device >= PPP_MAX_DEV )
  {
    MSG_ERROR("Invalid dev", 0, 0, 0);
    dsm_free_packet(&item_ptr);
    return FALSE;
  }

  ppp_cb_ptr = &(ppp_cb_array[device]);
/*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  ppp_cb_ptr->InUnknown++;
  if_cb.InErrors++;
  MSG_HIGH( "Unknown PPP protocol: %x", protocol, 0, 0);
  if (dsm_pushdown(&item_ptr,
                   NULL,
                   sizeof(protocol),
                   DSM_DS_SMALL_ITEM_POOL) < sizeof(protocol))
  {
    ERR(" Running out of memory: dumping PPP frames", 0, 0, 0);
    dsm_free_packet(&item_ptr);
    return FALSE;
  }
  put16(item_ptr->data_ptr,protocol);
  fsm_send( &(ppp_cb_ptr->fsm[LCP]), PROT_REJ, 0, item_ptr);

  return TRUE;

} /* pppi_send_prot_rej() */



/*===========================================================================
FUNCTION PPPI_SKIPPED()

DESCRIPTION
  This function will Skip Incoming Packet routine

  NOTE: this funtion is only used internally to this file.

DEPENDENCIES
  None

RETURN VALUE
  None

SIDE EFFECTS
  None
===========================================================================*/
static void pppi_skipped
(
  ppp_dev_enum_type device,
  dsm_item_type **item_head_ptr
)
{
  ipcp_pdv_type *ipcp_ptr;
  /* - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - */

  if( device >= PPP_MAX_DEV )
  {
    dsm_free_packet(item_head_ptr);
    return;
  }

  ipcp_ptr = (ipcp_pdv_type*) (ppp_cb_array[device].fsm[IPcp].pdv);
  if(ipcp_ptr != NULL)
  {
    slhc_toss( ipcp_ptr->slhcp);
  }
  dsm_free_packet(item_head_ptr);

} /* pppi_skipped() */


#endif /* FEATURE_DATA */
