/*====*====*====*====*====*====*====*====*====*====*====*====*====*====*====*
            
            B L U E T O O T H    R F C O M M    M O D U L E

GENERAL DESCRIPTION
  This contains the highest level code for the Bluetooth RFCOMM layer.

              Copyright (c) 2000-2009 QUALCOMM Incorporated.               
                      All Rights Reserved.                                  
              Qualcomm Confidential and Proprietary
*====*====*====*====*====*====*====*====*====*====*====*====*====*====*====*/

/*===========================================================================

                        EDIT HISTORY FOR MODULE

  This section contains comments describing changes made to the module.
  Notice that changes are listed in reverse chronological order.

 $Header: //source/qcom/qct/wconnect/bthost/core/rel/00.00.26/src/btrc.c#3 $ $DateTime:
  when        who  what, where, why
  ----------  ---  ----------------------------------------------------------
  2009-02-21   gs  Updated connect cancel logic to properly handle the case 
                   when DLCs are not up
  2008-09-30   rb  Fixed a missing bt_rc_reg_table initialization in 
                   bt_rc_cmd_register caused due to merge
  2008-08-14   rb  Added handling to keep the background timer tick during
                   power collapse
  2008-06-26   gs  Fixed bt_rc_reg_table initialization issue in 
                   bt_rc_cmd_register()
  2008-05-16   vk  Moved BT_RC_MAX_TX_BYTES_PER_TICK to bti.h.

 *
 *    #38        29 May 2007            PR
 * Cleanup when p2p timer expires
 *
 *    #37        02 May 2007            SSK
 * Remove unused variables to fix compiler warnings.
 * 
 *    #36        09 Mar 2007            JTL
 * Set L2CAP parameters for outgoing connections.
 * Change MFS calc so it doesn't double-count overhead.
 * Always call MFS calc for client connections.
 * Flow on from lower layers now restarts TX flow.
 * When TX flow is restarted, send data immediately, rather than
 *  waiting for TX timeout.
 *
 *    #35        22 Feb 2007            JTL
 * Critical lint fixes.
 *
 *    #34        20 Feb 2007            DSN
 * Increasing the maximum number of events in RC layer for SOC solutions.
 *
 *    #33        20 Dec 2006            DSN
 * Throughput optimization for BT2.0+ EDR based SOC solutions.
 *
 *    #32        04 Dec 2006            JTL
 * Cleanup MCC better when P2P timer expires.
 * Change Cancel Connect so it does not work on links already disconnecting.
 *
 *    #31        03 Dec 2006            JTL
 * Added bt_rc_cmd_cancel_connect() to cancel outgoing connections before they've
 * been fully established.
 * Free DSM pointer in TX queue when the link is closed.
 *
 *    #30        19 Oct 2006            DSN
 * Rename BB TX flow to ACL TX flow since HCI may provide this indication for
 * SOC solution.
 *
 *    #29        14 Aug 2006            JTL
 * Shorten disconnect timeouts. If a new connection request occurs when
 * already disconnecting, disconnect sooner and re-queue the connection
 * attempt.
 *
 *    #28        14 Aug 2006            JTL
 * Use a different L2 connect command to get the CID for outgoing
 * connections immediately.
 *
 *    #27        07 Aug 2006            RP
 * Moved a debug message inside "if" condition to avoid accessing
 * un-initialized pointer.  
 *
 *    #26        27 Jun 2006            DSN
 * Removed superfluous header file inclusion.
 *
 *    #25        18 May 2006            JTL
 * Calculate optimal MFS given a small watermark DNE. Tries to limit
 * MFS to a baseband-sized chunk smaller than the DNE.
 *
 *    #24        13 Mar 2006            JTL
 * Handle collision when Unregister command is received while DLC is 
 * coming up. Added Unregister event.
 *
 *    #23        06  Dec 2005           DSN
 * Fixed RVCT compiler errors.
 *
 *    #22        1  Dec 2005            DP
 * Changed CBFC to support cases where there is no RX watermark by only
 * allowing one outstanding transmit credit at a time to the remote side.
 * This is known to apply for Circuit Switched (CS) data calls.  Removed
 * erroneous error message upon updates to baseband transmit flow control.
 *
 *    #21        16 Nov 2005            GS
 * Added support for BD address based TX flow control in RFCOMM. 
 *
 *    #20        28 Jul 2005            DP
 * Adjust system monitoring timeouts to expire in order from lowest
 * to highest layers.
 *
 *    #19        16 Jun 2005            DP
 * Rearranged code for CBFC corner case fix.
 *
 *    #18        5  May 2005            GS
 * Corrected corner case in CBFC wherein we are sending N1 instead of N1-1 info 
 * bytes when credit field is present.
 *
 *    #17       12  Apr 2005            GS
 * Restart TX flow after BT QDSP image is swapped in.
 *
 *    #16        8  Apr 2005            DP
 * Make use of BT_BDA debug message macro.
 *
 *    #15        7  Apr 2005            DP
 * Don't allow CBFC to stall flow when RX watermark don't exceed count
 * is too small.
 *
 *    #14        16 Mar 2005            DP
 * Increased maximum number of BT SIO ports available.  Fixed Mode 2
 * security for client connections.
 *
 *    #13        8  Mar 2005            DP
 * Improved CBFC so it now uses actual space available in receive watermark
 * to determine number of transmit credits to give remote device.
 *
 *    #12        29 Jun 2004            DP
 * Increased MCC only timer to 30 seconds for Mode 2 Security support.
 *
 *    #11        8  Jun 2004            DP
 * Added client side Mode 2 security.
 *
 *    #10        9  Apr 2004            DP
 * Fixed MCC only connected RC background timer check.
 *
 *    #9         19 Dec 2003            DP
 * Improved background monitoring for MCCs.
 *
 *    #8         21 Nov 2003            DP
 * Servers generate disconnect event when they initiate disconnection.
 *
 *    #7         14 Nov 2003            DP
 * Only output disconnected event to servers that have been connected.
 *
 *    #5         4  Nov 2003            DP
 * General cleanup.
 *
 *    #4         3  Oct 2003            DP
 * Added RFCOMM peer to peer communications monitoring.
 * 
 *    Rev 1.26   25 Sep 2002 11:29:20   waynelee
 * Added routine to update watermarks.
 * 
 *    Rev 1.25   05 Aug 2002 17:59:08   ryuen
 * Merged changes from MSM_BT.01.00.11.01.02 (MSM5100 2.1 Patch 6)
 * 
 *    Rev 1.24   07 Jun 2002 17:07:12   waynelee
 * Send logs for multiple RCOMM connections.
 * 
 *    Rev 1.23   03 Apr 2002 14:05:46   propach
 * Enhanced modem status changed events.
 * 
 *    Rev 1.22   13 Mar 2002 12:02:50   waynelee
 * Re-enable CBFC.  Inadventently disabled in previous checkin.
 * 
 *    Rev 1.21   13 Mar 2002 12:00:52   waynelee
 * No change.
 * 
 *    Rev 1.20   13 Mar 2002 11:59:44   waynelee
 * Re-enable CBFC.  Inadventently disabled in previous checkin.
 * 
 *    Rev 1.19   20 Feb 2002 09:35:58   waynelee
 * Merged in Dave's consolidated MTU code with fix to not set the
 * signal when TX flow is turned off.
 * 
 *    Rev 1.18.1.0   24 Jul 2002 12:11:24   propach
 * Fixed sending connection failed events.  Merged modem status change
 * improvements onto branch.
 * 
 *    Rev 1.18   30 Nov 2001 10:08:14   propach
 * Tuned ideal packet size to 3 DH3 packets.
 * 
 *    Rev 1.17   23 Oct 2001 16:54:14   propach
 * Added support for RFCOMM Credit Based Flow Control (CBFC).
 * Transmit frames are now build to a fixed size (MFS for now).
 * 
 *    Rev 1.16   07 Sep 2001 16:09:32   propach
 * Optimized event handling.  Improved support for SPP.
 * 
 *    Rev 1.15   29 Aug 2001 11:54:42   propach
 * Merged certification tip onto main tip.
 * 
 *    Rev 1.14.1.2   17 Jul 2001 16:08:30   propach
 * Fixed priority checks.
 * 
 *    Rev 1.14.1.1   12 Jul 2001 16:05:40   waynelee
 * Handle the transmission of dsms larger than MFS by duplicating them
 * and sending them individually.  (Fixes 17569)
 * 
 *    Rev 1.14   11 Jun 2001 10:13:46   waynelee
 * Always get and reg app_id during powerup_init().
 * 
 *    Rev 1.13   08 Jun 2001 15:01:14   propach
 * Added RFCOMM link negotiations and modem status lines for SPP certification.
 * 
 *    Rev 1.12   18 May 2001 11:23:02   amitj
 * Added BT_L2_NULL_ID parameter to bt_cmd_l2_set_params()
 * 
 *    Rev 1.11   29 Mar 2001 16:19:04   propach
 * Fixed direction setting when bringing up second DLCI.
 * 
 *    Rev 1.10   26 Mar 2001 18:15:10   propach
 * Added RC commands and events to support serial port emulation settings
 * and modem status reading/writing.
 * 
 *    Rev 1.9   19 Mar 2001 11:11:50   propach
 * Disable MFS checks by default (required for Toshiba) and
 * enabled RF peer-to-peer flow control by default.
 * 
 *    Rev 1.8   16 Mar 2001 17:27:02   propach
 * Added infrastructure for SPP including initial support of
 * RPN and RLS and reworking PN signaling messages.
 * 
 *    Rev 1.6   02 Mar 2001 11:46:28   propach
 * Made server registrations persistent across connections.  Started
 * credit based flow control support for 1.1 (not implemented yet).
 * Register for L2CAP only when necessary based on RC registrations.
 * Removed unused online/offline/init for RC sub-modules.
 * 
 *    Rev 1.5   26 Feb 2001 12:36:58   propach
 * Updated RC flow control.  Updated MSC and test cmd/resp.  Improved
 * QXDM message outputs.
 * 
 *    Rev 1.4   23 Feb 2001 17:57:48   ryuen
 * Renamed bt_rc_initialize and bt_rc_online to bt_rc_powerup_init and
 * bt_rc_qdsp_ready_init respectively
 * 
 *    Rev 1.3   16 Feb 2001 16:40:16   propach
 * Increased number of event buffers.
 * 
 *    Rev 1.2   16 Feb 2001 16:11:16   propach
 * Completed EC routing of events by BT application ID.
 * 
 *    Rev 1.1   07 Feb 2001 10:17:44   propach
 * Reworked internal BT application IDs and made event control
 * route based on these.
 * 
 *    Rev 1.0   29 Jan 2001 17:08:58   propach
 * Initial revision.
 * 
 *    Rev 1.31   29 Jan 2001 15:36:54   propach
 * Improved flow debug messages.
 * 
 *    Rev 1.30   21 Dec 2000 17:45:20   waynelee
 * Removed T_CORE_EMUL.  Removed usage of bt_record_fault().
 * Use BT_SET_PKT_TRACER().  Added flow ctrl debug msgs.
 * Merged in the removal of RFCOMM flow control (l2cap only).
 * Cleaned up msgs.
 * 
 *    Rev 1.29   06 Dec 2000 10:24:14   propach
 * Improved Q depth statistics gathering.
 * 
 *    Rev 1.28   29 Nov 2000 16:15:48   amitj
 * A small correction to bt_rc_snapshot_logging_function()
 * 
 *    Rev 1.27   29 Nov 2000 11:08:18   amitj
 * allocating log_bt_rc_dlci_type in snapshot_logging_function
 * 
 *    Rev 1.26   22 Nov 2000 11:45:08   sramacha
 * 
 *    Rev 1.25   22 Nov 2000 10:38:10   propach
 * Removed NA commands.
 *
 *    Rev 1.24   09 Nov 2000 11:24:14   amitj
 * Added the header for bt_rc_snapshot_logging_function()
 *
 *    Rev 1.23   03 Nov 2000 15:10:28   ryuen
 * Disabled Maximum Frame Size checking.  This is featurized under
 * FEATURE_SWDEV_DISABLE_MFS_CHECK and the feature by default is NOT defined
 *
 *    Rev 1.22   03 Nov 2000 09:05:00   waynelee
 * Added periodic logging of state/status/statistics for QXDM
 *
 *    Rev 1.21   30 Oct 2000 18:15:54   propach
 * Re-enabled RFCOMM peer-to-peer flow control.
 *
 *    Rev 1.20   24 Oct 2000 09:43:20   propach
 * Added fast FCS treatments for payload carrying UIH frames.
 *
 *    Rev 1.19   16 Oct 2000 15:31:12   propach
 * Fixed Q depth checks.
 *
 *    Rev 1.18   10 Oct 2000 13:29:46   propach
 * Fix timer based TX and APIT RX.
 *
 *    Rev 1.17   09 Oct 2000 11:31:38   propach
 * Optimized loops in/out of DSP.  Made RC TX timer driven.  Made QDSP
 * events be interrupt driven.
 *
 *    Rev 1.16   25 Sep 2000 21:24:22   propach
 * NA flow control fixes.  Add MC offline & stop signal handling.
 *
 *    Rev 1.15   16 Sep 2000 15:12:02   propach
 * Add flow control between top of RFCOMM and BB for NA.  Also avoid
 * queueing of payload data.
 *
 *    Rev 1.14   06 Sep 2000 17:22:30   propach
 * Fixed MSG macro usage.
 *
 *    Rev 1.13   05 Sep 2000 17:54:36   propach
 * Added QDSP services changes.
 *
 *    Rev 1.12   02 Sep 2000 19:35:12   propach
 * Added BT MSGs.
 *
 *    Rev 1.11   31 Aug 2000 14:18:24   waynelee
 * Changes to support new BT debug MSG macros
 *
 *    Rev 1.10   31 Aug 2000 10:45:12   propach
 * Featurized APIT code.  Improved APIT watermark proxy handling.
 *
 *    Rev 1.9   30 Aug 2000 09:42:22   waynelee
 * Made changes for new BT_MSG macros
 *
 *    Rev 1.8   29 Aug 2000 10:28:30   propach
 * Expanded command Q, fixed q depth counters.
 *
 *    Rev 1.7   29 Aug 2000 08:25:54   propach
 * Moved comdef.h includes above FEATURE_BT to remove compiler
 * warning when feature is disabled.
 *
 *    Rev 1.6   28 Aug 2000 16:13:00   propach
 * Cleanup RFCOMM.  Add use of msg/err macros.  Add driver fault
 * recording and statistics gathering.
 *
 *    Rev 1.5   25 Aug 2000 16:45:50   propach
 * Memory leak fixes and some RFCOMM cleanup.
 *
 *    Rev 1.4   24 Aug 2000 13:47:22   propach
 * RFCOMM optimizations. Baseband fixes.
 *
 *    Rev 1.3   17 Aug 2000 13:44:44   propach
 * Added Event Control (EC).  Fixed minor problems in RFCOMM.
 *
 *    Rev 1.2   02 Aug 2000 09:40:28   propach
 * Added QBTS support for BT.  Added to API.
 *
 *    Rev 1.1   19 Jul 2000 17:26:36   propach
 * Update BT API and featurization with cleanup.
 *
 *    Rev 1.0   04 Jul 2000 19:21:38   ryuen
 * Initial revision.

===========================================================================*/

/*===========================================================================

                     INCLUDE FILES FOR MODULE

===========================================================================*/

#include "customer.h"
#include "comdef.h"
#ifdef FEATURE_BT
#include <string.h>
#include "assert.h"
#include "dsm.h"
#include "queue.h"
#include "rex.h"
#include "task.h"
#include "bt.h"
#include "bti.h"
#include "btcmdi.h"
#include "btdebug.h"
#include "btl2.h"
#include "btmsg.h"
#include "btrc.h"
#include "btrcev.h"
#include "btrci.h"
#include "btrcmsgi.h"
#include "btrcrx.h"
#include "btrctx.h"
#include "btutils.h"

#ifdef BT_SWDEV_QXDM_LOG_PER
#include "btlogi.h"
#include "log.h"
#endif




/*-------------------------------------------------------------------------*/
/*                                                                         */
/*                         Data Definitions                                */
/*                                                                         */
/*-------------------------------------------------------------------------*/

#define BT_MSG_LAYER BT_MSG_RC

#define RC_PEER_TO_PEER_TIMEOUT_MS (70 * 1000)
#define RC_MCC_ONLY_TIMEOUT_MS     (70 * 1000)
#define RC_QUICK_TIMEOUT_MS        (1000)
#define RC_MEDIUM_TIMEOUT_MS       (5000)

#define RC_UNREGISTER_TIMEOUT_MS   RC_QUICK_TIMEOUT_MS
#define RC_DISC_TIMEOUT_MS         RC_MEDIUM_TIMEOUT_MS
#define RC_CONN_TIMEOUT_MS         RC_PEER_TO_PEER_TIMEOUT_MS

#ifdef FEATURE_BT_SOC
#  define RC_MAX_MTU               BT_L2_MAX_MTU
#else
#  define RC_MAX_MTU               BT_L2_DEFAULT_MTU
#endif /* FEATURE_BT_SOC */


LOCAL bt_app_id_type  bt_rc_app_id;

#define BT_RC_TX_TIMER_MS 2

LOCAL rex_timer_type  bt_rc_tx_timer;
LOCAL boolean         bt_rc_tx_timer_running;


uint16  bt_rc_ideal_info_len = BT_MTP_RC_INFO_LEN;


/*-------------------------------------------------------------------------*/
/*  RFCOMM session table (and DLC tables too).                             */
/*-------------------------------------------------------------------------*/

LOCAL bt_rc_session_type  bt_rc_session[ BT_RC_MAX_SESSIONS ];


/*-------------------------------------------------------------------------*/
/*  RFCOMM registration table definitions and declaration.                 */
/*-------------------------------------------------------------------------*/

LOCAL uint8                 bt_rc_reg_count;
LOCAL bt_rc_reg_table_type  bt_rc_reg_table[ BT_RC_MAX_PORTS ];


/*-------------------------------------------------------------------------*/
/*  RFCOMM negotiation table definitions and declaration.                  */
/*-------------------------------------------------------------------------*/

uint8                   bt_rc_negot_count;
bt_rc_negot_table_type  bt_rc_negot_table[ BT_RC_MAX_PORTS ];


/*-------------------------------------------------------------------------*/
/*  Information and queue for events destined for the RFCOMM layer.        */
/*-------------------------------------------------------------------------*/
#ifdef FEATURE_BT_SOC
  #define BT_RC_NUM_EVENTS    100
#else
  #define BT_RC_NUM_EVENTS    30
#endif /* FEATURE_BT_SOC */
LOCAL bt_event_q_info_type  bt_rc_ev_q_info;
LOCAL q_type                bt_rc_ev_q;


/*===========================================================================

                         FUNCTION DEFINITIONS

===========================================================================*/

/*===========================================================================

FUNCTION
  bt_rc_initialize_fast_uih_fcs

DESCRIPTION
  Initializes a specified fast UIH FCS structure.

===========================================================================*/
LOCAL void bt_rc_initialize_fast_uih_fcs
(
  fast_uih_fcs_type*  fast_uih_fcs_ptr
)
{

  fast_uih_fcs_ptr->hdr_addr = 0;
  fast_uih_fcs_ptr->hdr_ctrl = 0;
  fast_uih_fcs_ptr->fcs = 0;

}


/*===========================================================================

FUNCTION
  bt_rc_setup_fast_uih_fcs

DESCRIPTION
  Setups up the set of fast UIH FCS structures for a specified DLC.
  These are used for UIH frames carrying user data only (not MCC frames).

===========================================================================*/
LOCAL void bt_rc_setup_fast_uih_fcs
(
  bt_rc_session_type*  ssn_ptr,
  bt_rc_dlc_type*      dlc_ptr
)
{

  fast_uih_fcs_type*  t0_ptr = &dlc_ptr->tx_fast_uih_pf0;
  fast_uih_fcs_type*  t1_ptr = &dlc_ptr->tx_fast_uih_pf1;
  fast_uih_fcs_type*  r0_ptr = &dlc_ptr->rx_fast_uih_pf0;
  fast_uih_fcs_type*  r1_ptr = &dlc_ptr->rx_fast_uih_pf1;

  t0_ptr->hdr_addr = RCFH_DLCI_2_ADDR( dlc_ptr->dlci );
  t1_ptr->hdr_addr = t0_ptr->hdr_addr;
  r0_ptr->hdr_addr = t0_ptr->hdr_addr;
  r1_ptr->hdr_addr = t0_ptr->hdr_addr;
  if ( ssn_ptr->initiator != FALSE )
  {
    t0_ptr->hdr_addr |= RC_CR_CMD_B;
    t1_ptr->hdr_addr |= RC_CR_CMD_B;
  }
  else
  {
    r0_ptr->hdr_addr |= RC_CR_CMD_B;
    r1_ptr->hdr_addr |= RC_CR_CMD_B;
  }

  t0_ptr->hdr_ctrl = RCFH_CTRL_FT_UIH_V;
  t1_ptr->hdr_ctrl = (t0_ptr->hdr_ctrl | RCFH_CTRL_PF_B);
  r0_ptr->hdr_ctrl = t0_ptr->hdr_ctrl;
  r1_ptr->hdr_ctrl = t1_ptr->hdr_ctrl;

  t0_ptr->fcs = bt_rc_calculate_fcs(
                  (uint8*)t0_ptr, BT_RC_FCS_BYTES_UIH );

  t1_ptr->fcs = bt_rc_calculate_fcs(
                  (uint8*)t1_ptr, BT_RC_FCS_BYTES_UIH );

  r0_ptr->fcs = bt_rc_calculate_fcs(
                  (uint8*)r0_ptr, BT_RC_FCS_BYTES_UIH );

  r1_ptr->fcs = bt_rc_calculate_fcs(
                  (uint8*)r1_ptr, BT_RC_FCS_BYTES_UIH );

}


/*===========================================================================

FUNCTION
  bt_rc_get_default_priority_for_dlci

DESCRIPTION
  This returns the default priority for a given DLCI value as specified
  in GSM 07.10 section 5.6.

RETURN VALUE
  Default priority for DLCI value.

===========================================================================*/
uint8 bt_rc_get_default_priority_for_dlci
(
  uint8  dlci
)
{

  uint8  default_priority = 0;

  if ( dlci == 0 )
  {
    default_priority = 0;
  }
  else if ( (dlci >= 1) && (dlci <= 7) )
  {
    default_priority = 7;
  }
  else if ( (dlci >= 8) && (dlci <= 15) )
  {
    default_priority = 15;
  }
  else if ( (dlci >= 16) && (dlci <= 23) )
  {
    default_priority = 23;
  }
  else if ( (dlci >= 24) && (dlci <= 31) )
  {
    default_priority = 31;
  }
  else if ( (dlci >= 32) && (dlci <= 39) )
  {
    default_priority = 39;
  }
  else if ( (dlci >= 40) && (dlci <= 47) )
  {
    default_priority = 47;
  }
  else if ( (dlci >= 48) && (dlci <= 55) )
  {
    default_priority = 55;
  }
  else if ( (dlci >= 56) && (dlci <= 61) )
  {
    default_priority = 61;
  }
  else
  {
    /*  Illegal DLCI value when setting priority.  */
    BT_ERR( "BT RC: SetPri bad DLCI %x", dlci, 0, 0 );
  }

  return ( default_priority );
}


/*===========================================================================

FUNCTION
  bt_rc_write_sp_config

DESCRIPTION
  Updates the fields corresponding to an MCC RPN command of a
  specified configuration element.

RETURN VALUE
  TRUE if any field is changed, otherwise FALSE.

===========================================================================*/
boolean bt_rc_write_sp_config
(
  uint8                 dlci,
  bt_rc_config_type*    cfg_ptr,
  bt_rc_mcc_rpne_type*  rpne_msg_ptr
)
{

  boolean  rpn_cfg_changed = FALSE;
  uint16   param_mask;
  uint8    baudrate;
  uint8    format;
  uint8    flow_ctrl;
  uint8    xon_char;
  uint8    xoff_char;

  param_mask = rpne_msg_ptr->param_mask;
  baudrate   = rpne_msg_ptr->baudrate;
  format     = rpne_msg_ptr->format;
  flow_ctrl  = rpne_msg_ptr->flow_ctrl;
  xon_char   = rpne_msg_ptr->xon_char;
  xoff_char  = rpne_msg_ptr->xoff_char;
  
  /*  Accept baud rate change if present.  */
  if ( (param_mask & RCMCCM_RPN_PM_BR_B) &&
       (baudrate != cfg_ptr->baudrate) )
  {
    BT_MSG_SIG( "BT RC: RPN New BR %x was %x DLCI %x",
                baudrate, cfg_ptr->baudrate, dlci );
    cfg_ptr->baudrate = baudrate;
    rpn_cfg_changed = TRUE;
  }

  /*  Accept data bits change if present.  */
  if ( (param_mask & RCMCCM_RPN_PM_D_B) &&
       ((format & RCMCCM_RPN_FMT_D_M) !=
        (cfg_ptr->format & RCMCCM_RPN_FMT_D_M)) )
  {
    BT_MSG_SIG( "BT RC: RPN New DB %x was %x DLCI %x",
                (format & RCMCCM_RPN_FMT_D_M),
                (cfg_ptr->format & RCMCCM_RPN_FMT_D_M), dlci );

    cfg_ptr->format &= ~RCMCCM_RPN_FMT_D_M;
    cfg_ptr->format |= (format & RCMCCM_RPN_FMT_D_M);
    rpn_cfg_changed = TRUE;
  }

  /*  Accept stop bits change if present.  */
  if ( (param_mask & RCMCCM_RPN_PM_S_B) &&
       ((format & RCMCCM_RPN_FMT_S_1P5_B) !=
        (cfg_ptr->format & RCMCCM_RPN_FMT_S_1P5_B)) )
  {
    BT_MSG_SIG( "BT RC: RPN New SB %x was %x DLCI %x",
                (format & RCMCCM_RPN_FMT_S_1P5_B),
                (cfg_ptr->format & RCMCCM_RPN_FMT_S_1P5_B), dlci );

    cfg_ptr->format &= ~RCMCCM_RPN_FMT_S_1P5_B;
    cfg_ptr->format |= (format & RCMCCM_RPN_FMT_S_1P5_B);
    rpn_cfg_changed = TRUE;
  }

  /*  Accept parity change if present.  */
  if ( (param_mask & RCMCCM_RPN_PM_P_B) &&
       ((format & RCMCCM_RPN_FMT_P_B) !=
        (cfg_ptr->format & RCMCCM_RPN_FMT_P_B)) )
  {
    BT_MSG_SIG( "BT RC: RPN New Parity %x was %x DLCI %x",
                (format & RCMCCM_RPN_FMT_P_B),
                (cfg_ptr->format & RCMCCM_RPN_FMT_P_B), dlci );

    cfg_ptr->format &= ~RCMCCM_RPN_FMT_P_B;
    cfg_ptr->format |= (format & RCMCCM_RPN_FMT_P_B);
    rpn_cfg_changed = TRUE;
  }

  /*  Accept parity type change if present.  */
  if ( (param_mask & RCMCCM_RPN_PM_PT_B) &&
       ((format & RCMCCM_RPN_FMT_PT_M) !=
        (cfg_ptr->format & RCMCCM_RPN_FMT_PT_M)) )
  {
    BT_MSG_SIG( "BT RC: RPN New PT %x was %x DLCI %x",
                (format & RCMCCM_RPN_FMT_PT_M),
                (cfg_ptr->format & RCMCCM_RPN_FMT_PT_M), dlci );

    cfg_ptr->format &= ~RCMCCM_RPN_FMT_PT_M;
    cfg_ptr->format |= (format & RCMCCM_RPN_FMT_PT_M);
    rpn_cfg_changed = TRUE;
  }

  /*  BT_TBD:  All of the flow control settings below  */
  /*           need to be implemented or not allowed.  */

  /*  Accept XON character change if present.  */
  if ( (param_mask & RCMCCM_RPN_PM_XON_B) &&
       (xon_char != cfg_ptr->xon_char) )
  {
    BT_MSG_SIG( "BT RC: RPN New XON %x was %x DLCI %x",
                xon_char, cfg_ptr->xon_char, dlci );

    cfg_ptr->xon_char = xon_char;
    rpn_cfg_changed = TRUE;
  }

  /*  Accept XOFF character change if present.  */
  if ( (param_mask & RCMCCM_RPN_PM_XOFF_B) &&
       (xoff_char != cfg_ptr->xoff_char) )
  {
    BT_MSG_SIG( "BT RC: RPN New XOFF %x was %x DLCI %x",
                xoff_char, cfg_ptr->xoff_char, dlci );

    cfg_ptr->xoff_char = xoff_char;
    rpn_cfg_changed = TRUE;
  }

  /*  Accept XON/OFF on input change if present.  */
  if ( (param_mask & RCMCCM_RPN_PM_XO_IN_B) &&
       ((flow_ctrl & RCMCCM_RPN_FC_XO_IN_B) !=
        (cfg_ptr->flow_ctrl & RCMCCM_RPN_FC_XO_IN_B)) )
  {
    BT_MSG_SIG( "BT RC: RPN New XO IN %x was %x DLCI %x",
                (flow_ctrl & RCMCCM_RPN_FC_XO_IN_B),
                (cfg_ptr->flow_ctrl & RCMCCM_RPN_FC_XO_IN_B), dlci );

    cfg_ptr->flow_ctrl &= ~RCMCCM_RPN_FC_XO_IN_B;
    cfg_ptr->flow_ctrl |= (flow_ctrl & RCMCCM_RPN_FC_XO_IN_B);
    rpn_cfg_changed = TRUE;
  }

  /*  Accept XON/OFF on output change if present.  */
  if ( (param_mask & RCMCCM_RPN_PM_XO_OUT_B) &&
       ((flow_ctrl & RCMCCM_RPN_FC_XO_OUT_B) !=
        (cfg_ptr->flow_ctrl & RCMCCM_RPN_FC_XO_OUT_B)) )
  {
    BT_MSG_SIG( "BT RC: RPN New XO OUT %x was %x DLCI %x",
                (flow_ctrl & RCMCCM_RPN_FC_XO_OUT_B),
                (cfg_ptr->flow_ctrl & RCMCCM_RPN_FC_XO_OUT_B), dlci );

    cfg_ptr->flow_ctrl &= ~RCMCCM_RPN_FC_XO_OUT_B;
    cfg_ptr->flow_ctrl |= (flow_ctrl & RCMCCM_RPN_FC_XO_OUT_B);
    rpn_cfg_changed = TRUE;
  }

  /*  Accept RTR on input change if present.  */
  if ( (param_mask & RCMCCM_RPN_PM_RTR_IN_B) &&
       ((flow_ctrl & RCMCCM_RPN_FC_RTR_IN_B) !=
        (cfg_ptr->flow_ctrl & RCMCCM_RPN_FC_RTR_IN_B)) )
  {
    BT_MSG_SIG( "BT RC: RPN New RTR IN %x was %x DLCI %x",
                (flow_ctrl & RCMCCM_RPN_FC_RTR_IN_B),
                (cfg_ptr->flow_ctrl & RCMCCM_RPN_FC_RTR_IN_B), dlci );

    cfg_ptr->flow_ctrl &= ~RCMCCM_RPN_FC_RTR_IN_B;
    cfg_ptr->flow_ctrl |= (flow_ctrl & RCMCCM_RPN_FC_RTR_IN_B);
    rpn_cfg_changed = TRUE;
  }

  /*  Accept RTR on output change if present.  */
  if ( (param_mask & RCMCCM_RPN_PM_RTR_OUT_B) &&
       ((flow_ctrl & RCMCCM_RPN_FC_RTR_OUT_B) !=
        (cfg_ptr->flow_ctrl & RCMCCM_RPN_FC_RTR_OUT_B)) )
  {
    BT_MSG_SIG( "BT RC: RPN New RTR OUT %x was %x DLCI %x",
                (flow_ctrl & RCMCCM_RPN_FC_RTR_OUT_B),
                (cfg_ptr->flow_ctrl & RCMCCM_RPN_FC_RTR_OUT_B), dlci );

    cfg_ptr->flow_ctrl &= ~RCMCCM_RPN_FC_RTR_OUT_B;
    cfg_ptr->flow_ctrl |= (flow_ctrl & RCMCCM_RPN_FC_RTR_OUT_B);
    rpn_cfg_changed = TRUE;
  }

  /*  Accept RTC on input change if present.  */
  if ( (param_mask & RCMCCM_RPN_PM_RTC_IN_B) &&
       ((flow_ctrl & RCMCCM_RPN_FC_RTC_IN_B) !=
        (cfg_ptr->flow_ctrl & RCMCCM_RPN_FC_RTC_IN_B)) )
  {
    BT_MSG_SIG( "BT RC: RPN New RTC IN %x was %x DLCI %x",
                (flow_ctrl & RCMCCM_RPN_FC_RTC_IN_B),
                (cfg_ptr->flow_ctrl & RCMCCM_RPN_FC_RTC_IN_B), dlci );

    cfg_ptr->flow_ctrl &= ~RCMCCM_RPN_FC_RTC_IN_B;
    cfg_ptr->flow_ctrl |= (flow_ctrl & RCMCCM_RPN_FC_RTC_IN_B);
    rpn_cfg_changed = TRUE;
  }

  /*  Accept RTC on output change if present.  */
  if ( (param_mask & RCMCCM_RPN_PM_RTC_OUT_B) &&
       ((flow_ctrl & RCMCCM_RPN_FC_RTC_OUT_B) !=
        (cfg_ptr->flow_ctrl & RCMCCM_RPN_FC_RTC_OUT_B)) )
  {
    BT_MSG_SIG( "BT RC: RPN New RTC OUT %x was %x DLCI %x",
                (flow_ctrl & RCMCCM_RPN_FC_RTC_OUT_B),
                (cfg_ptr->flow_ctrl & RCMCCM_RPN_FC_RTC_OUT_B), dlci );

    cfg_ptr->flow_ctrl &= ~RCMCCM_RPN_FC_RTC_OUT_B;
    cfg_ptr->flow_ctrl |= (flow_ctrl & RCMCCM_RPN_FC_RTC_OUT_B);
    rpn_cfg_changed = TRUE;
  }

  if ( cfg_ptr->flow_ctrl != 0 )
  {
    BT_MSG_HIGH( "BT RC: RPN FC not None %x DLCI %x",
                 cfg_ptr->flow_ctrl, dlci, 0 );
  }

  return ( rpn_cfg_changed );

}


/*===========================================================================

FUNCTION
  bt_rc_need_remote_port_negotiation
  
DESCRIPTION
  Checks whether a specified configuration element requires an RFCOMM
  Remote Port Negotiation.  This is necessary the configuration fields
  corresponding to an MCC RPN command are not the default values and
  the negotiation has not yet occurred.
  
RETURN VALUE
  TRUE if RPN must occur, otherwise FALSE.

===========================================================================*/
LOCAL boolean bt_rc_need_remote_port_negotiation
(
  bt_rc_dlc_type*  dlc_ptr
)
{

  boolean  rpn_needed = FALSE;

  if ( dlc_ptr->config.rpn_negot_done == FALSE )
  {
    if ( dlc_ptr->config.baudrate != RCMCCM_RPN_BR_DEFAULT_V )
    {
      rpn_needed = TRUE;
    }

    if ( dlc_ptr->config.format != RCMCCM_RPN_FMT_DEFAULT_V )
    {
      rpn_needed = TRUE;
    }

    if ( dlc_ptr->config.flow_ctrl != RCMCCM_RPN_FC_DEFAULT_V )
    {
      rpn_needed = TRUE;
    }

    if ( dlc_ptr->config.xon_char != RCMCCM_RPN_XON_DEFAULT_V )
    {
      rpn_needed = TRUE;
    }

    if ( dlc_ptr->config.xoff_char != RCMCCM_RPN_XOFF_DEFAULT_V )
    {
      rpn_needed = TRUE;
    }
  }
  
  return ( rpn_needed );

}


/*===========================================================================

FUNCTION
  bt_rc_initialize_sp_config
  
DESCRIPTION
  Initializes the serial port part of a specified configuration element.

===========================================================================*/
LOCAL void bt_rc_initialize_sp_config
(
  bt_rc_config_type*  cfg_ptr
)
{

  cfg_ptr->baudrate  = RCMCCM_RPN_BR_DEFAULT_V;
  cfg_ptr->format    = RCMCCM_RPN_FMT_DEFAULT_V;
  cfg_ptr->flow_ctrl = RCMCCM_RPN_FC_DEFAULT_V;
  cfg_ptr->xon_char  = RCMCCM_RPN_XON_DEFAULT_V;
  cfg_ptr->xoff_char = RCMCCM_RPN_XOFF_DEFAULT_V;

}


/*===========================================================================

FUNCTION
  bt_rc_local_rx_flow_on

DESCRIPTION
  Determines whether the receive flow for a specified DLC is on.

RETURN VALUE
  TRUE  if the receive flow in on
  FALSE otherwise
  
===========================================================================*/
LOCAL boolean bt_rc_local_rx_flow_on
(
  bt_rc_session_type*  ssn_ptr,
  bt_rc_dlc_type*      dlc_ptr
)
{

  boolean  rx_flow_on = FALSE;

  /*  RX Flow ON?  */
  if ( ((ssn_ptr->mcc.local_v24_signals | dlc_ptr->local_v24_signals) &
           RCMCCM_MSC_V24_FC_FLOW_OFF_B) == 0 )
  {
    rx_flow_on = TRUE;
  }

  return ( rx_flow_on );

}


/*===========================================================================

FUNCTION
  bt_rc_max_far_tx_credits

DESCRIPTION
  Determines the maximum number of transmit credits the remote side can
  currently be given without overflowing the receive watermark on a
  specified DLC.

RETURN VALUE
  Maximum number of transmit credits the far side can currently be given.
  
===========================================================================*/
bt_rc_cbfc_credits_type bt_rc_max_far_tx_credits
(
  bt_rc_session_type*  ssn_ptr,
  bt_rc_dlc_type*      dlc_ptr
)
{

  bt_rc_cbfc_credits_type  max_far_tx_credits = 0;

  int32                    rx_bytes_committed;
  int32                    rx_wm_free_bytes;

  
  if ( dlc_ptr->rx_wm_ptr != NULL )
  {
    /*  Calcultate the number of transmit credits the   */
    /*  remote side can currently be given.  It is the  */
    /*  number of free bytes on the receive watermark   */
    /*  less the bytes committed already (which is the  */
    /*  outstanding credits times the Maximum Frame     */
    /*  Size (MFS)) divided by the MFS.                 */

    rx_bytes_committed = dlc_ptr->config.cbfc_far_credits *
                           dlc_ptr->config.max_frame_size;

    rx_wm_free_bytes = dlc_ptr->rx_wm_ptr->dont_exceed_cnt -
                         dlc_ptr->rx_wm_ptr->current_cnt -
                           rx_bytes_committed;

    /*  Manual divide by MFS.  */
    while ( rx_wm_free_bytes > dlc_ptr->config.max_frame_size )
    {
      max_far_tx_credits++;
      rx_wm_free_bytes -= dlc_ptr->config.max_frame_size;
    }

    if ( (dlc_ptr->rx_wm_ptr->current_cnt == 0) &&
         /* JTL: */
         /* (dlc_ptr->rx_wm_ptr->current_cnt < (dlc_ptr->rx_wm_ptr->dont_exceed_cnt / 2)) &&
            (dlc_ptr->rx_wm_ptr->dont_exceed_cnt < dlc_ptr->config.max_frame_size) && */
         (dlc_ptr->config.cbfc_far_credits == 0) &&
         (bt_rc_local_rx_flow_on( ssn_ptr, dlc_ptr ) != FALSE) &&
         (max_far_tx_credits == 0) )
    {
      /*  There are no transmit credits being given to the far side    */
      /*  even though flow is enabled and the receive watermark is     */
      /*  empty.  This can only be caused by the watermark Don't       */
      /*  Exceed Count (DEC) being too small relative to the MFS on    */
      /*  RFCOMM.  Issue one transmit credit just to make sure flow    */
      /*  continues.  Data will be received correctly if the far side  */
      /*  sends RFCOMM frames smaller than the DEC.  Any frames sent   */
      /*  that are larger than the DEC will be dropped.  It's better   */
      /*  to drop some data than to stop flow forever.  This is an     */
      /*  application mis-use of BT and must be corrected for things   */
      /*  to work reliably.  Output an error message to flag it.       */

      max_far_tx_credits = 1;

      BT_ERR( "BT RC: Bad WM DEC %x CNT %x MFS %x",
              dlc_ptr->rx_wm_ptr->dont_exceed_cnt,
              dlc_ptr->rx_wm_ptr->current_cnt,
              dlc_ptr->config.max_frame_size );
    }
  }
  else
  {
    /*  There is no RX watermark of which to measure   */
    /*  the depth.  Only allow 1 outstanding transmit  */
    /*  credit at a time when RX flow is enabled.      */

    if ( (dlc_ptr->config.cbfc_far_credits == 0) &&
         (bt_rc_local_rx_flow_on( ssn_ptr, dlc_ptr ) != FALSE) )
    {
      max_far_tx_credits = 1;
    }
  }

  return ( max_far_tx_credits );

}


/*===========================================================================

FUNCTION
  bt_rc_far_tx_credits_needed

DESCRIPTION
  Determines whether the remote device of a specified DLC needs
  transmit credits sent to it.

RETURN VALUE
  TRUE if credits need to be sent to the remote device.
  FALSE when no credits need to be sent currently.
  
===========================================================================*/
boolean bt_rc_far_tx_credits_needed
(
  bt_rc_session_type*  ssn_ptr,
  bt_rc_dlc_type*      dlc_ptr
)
{

  boolean  far_tx_credits_needed = FALSE;

  if ( (bt_rc_local_rx_flow_on( ssn_ptr, dlc_ptr ) != FALSE) &&
       (bt_rc_max_far_tx_credits( ssn_ptr, dlc_ptr ) > 0) )
  {
    far_tx_credits_needed = TRUE;
  }

  return ( far_tx_credits_needed );

}


/*===========================================================================

FUNCTIONS
  bt_rc_rx_wm_gone_empty_cb*

DESCRIPTION
  The following functions are the RX "Gone Empty" call back functions
  for each of the DLCs supported by the BT SIO module.

===========================================================================*/
LOCAL void bt_rc_rx_wm_gone_empty_cb
(
  uint8  ssn_idx,
  uint8  dlc_idx
)
{

  if ( bt_rc_far_tx_credits_needed( &SSN( ssn_idx ),
                                    &(DLC( ssn_idx , dlc_idx )) ) != FALSE )
  {
    /*  Force a UIH frame transmit if none  */
    /*  pending to make sure we give some   */
    /*  transmit credits to the far side.   */
    bt_rc_restart_tx_flow();
#ifdef BT_FLOW_DEBUG
    BT_MSG_HIGH( "BT RC: CBFC RX GE ReTX FCr %x PID %x",
                 DLC( ssn_idx , dlc_idx ).config.cbfc_far_credits,
                 DLC( ssn_idx , dlc_idx ).pid, 0 );
#endif
  }

  /*  Call application's callback as necessary.  */
  if ( DLC( ssn_idx , dlc_idx ).orig_rx_wm_ge_fptr != NULL )
  {
    (DLC( ssn_idx , dlc_idx ).orig_rx_wm_ge_fptr)();
  }

}

/*  There must be one bt_rc_rx_wm_gone_empty_cb_*_* per  */
/*  [ BT_RC_MAX_SESSIONS, BT_RC_MAX_PORTS_PER_SESSION ]  */
/*  referenced below.                                    */

LOCAL void bt_rc_rx_wm_gone_empty_cb_0_0( void )
{
  bt_rc_rx_wm_gone_empty_cb( 0, 0 );
}

LOCAL void bt_rc_rx_wm_gone_empty_cb_0_1( void )
{
  bt_rc_rx_wm_gone_empty_cb( 0, 1 );
}

LOCAL void bt_rc_rx_wm_gone_empty_cb_0_2( void )
{
  bt_rc_rx_wm_gone_empty_cb( 0, 2 );
}

LOCAL void bt_rc_rx_wm_gone_empty_cb_0_3( void )
{
  bt_rc_rx_wm_gone_empty_cb( 0, 3 );
}

LOCAL void bt_rc_rx_wm_gone_empty_cb_0_4( void )
{
  bt_rc_rx_wm_gone_empty_cb( 0, 4 );
}

LOCAL void bt_rc_rx_wm_gone_empty_cb_0_5( void )
{
  bt_rc_rx_wm_gone_empty_cb( 0, 5 );
}

LOCAL void bt_rc_rx_wm_gone_empty_cb_0_6( void )
{
  bt_rc_rx_wm_gone_empty_cb( 0, 6 );
}

LOCAL void bt_rc_rx_wm_gone_empty_cb_0_7( void )
{
  bt_rc_rx_wm_gone_empty_cb( 0, 7 );
}

LOCAL void bt_rc_rx_wm_gone_empty_cb_0_8( void )
{
  bt_rc_rx_wm_gone_empty_cb( 0, 8 );
}

LOCAL void bt_rc_rx_wm_gone_empty_cb_0_9( void )
{
  bt_rc_rx_wm_gone_empty_cb( 0, 9 );
}

LOCAL void bt_rc_rx_wm_gone_empty_cb_1_0( void )
{
  bt_rc_rx_wm_gone_empty_cb( 1, 0 );
}

LOCAL void bt_rc_rx_wm_gone_empty_cb_1_1( void )
{
  bt_rc_rx_wm_gone_empty_cb( 1, 1 );
}

LOCAL void bt_rc_rx_wm_gone_empty_cb_1_2( void )
{
  bt_rc_rx_wm_gone_empty_cb( 1, 2 );
}

LOCAL void bt_rc_rx_wm_gone_empty_cb_1_3( void )
{
  bt_rc_rx_wm_gone_empty_cb( 1, 3 );
}

LOCAL void bt_rc_rx_wm_gone_empty_cb_1_4( void )
{
  bt_rc_rx_wm_gone_empty_cb( 1, 4 );
}

LOCAL void bt_rc_rx_wm_gone_empty_cb_1_5( void )
{
  bt_rc_rx_wm_gone_empty_cb( 1, 5 );
}

LOCAL void bt_rc_rx_wm_gone_empty_cb_1_6( void )
{
  bt_rc_rx_wm_gone_empty_cb( 1, 6 );
}

LOCAL void bt_rc_rx_wm_gone_empty_cb_1_7( void )
{
  bt_rc_rx_wm_gone_empty_cb( 1, 7 );
}

LOCAL void bt_rc_rx_wm_gone_empty_cb_1_8( void )
{
  bt_rc_rx_wm_gone_empty_cb( 1, 8 );
}

LOCAL void bt_rc_rx_wm_gone_empty_cb_1_9( void )
{
  bt_rc_rx_wm_gone_empty_cb( 1, 9 );
}

LOCAL void bt_rc_rx_wm_gone_empty_cb_2_0( void )
{
  bt_rc_rx_wm_gone_empty_cb( 2, 0 );
}

LOCAL void bt_rc_rx_wm_gone_empty_cb_2_1( void )
{
  bt_rc_rx_wm_gone_empty_cb( 2, 1 );
}

LOCAL void bt_rc_rx_wm_gone_empty_cb_2_2( void )
{
  bt_rc_rx_wm_gone_empty_cb( 2, 2 );
}

LOCAL void bt_rc_rx_wm_gone_empty_cb_2_3( void )
{
  bt_rc_rx_wm_gone_empty_cb( 2, 3 );
}

LOCAL void bt_rc_rx_wm_gone_empty_cb_2_4( void )
{
  bt_rc_rx_wm_gone_empty_cb( 2, 4 );
}

LOCAL void bt_rc_rx_wm_gone_empty_cb_2_5( void )
{
  bt_rc_rx_wm_gone_empty_cb( 2, 5 );
}

LOCAL void bt_rc_rx_wm_gone_empty_cb_2_6( void )
{
  bt_rc_rx_wm_gone_empty_cb( 2, 6 );
}

LOCAL void bt_rc_rx_wm_gone_empty_cb_2_7( void )
{
  bt_rc_rx_wm_gone_empty_cb( 2, 7 );
}

LOCAL void bt_rc_rx_wm_gone_empty_cb_2_8( void )
{
  bt_rc_rx_wm_gone_empty_cb( 2, 8 );
}

LOCAL void bt_rc_rx_wm_gone_empty_cb_2_9( void )
{
  bt_rc_rx_wm_gone_empty_cb( 2, 9 );
}

#if ( BT_RC_MAX_SESSIONS > 3 )
#error "Need more RX GE cbs for BT_RC_MAX_SESSIONS > 3"
#endif

#if ( BT_RC_MAX_PORTS_PER_SESSION > 10 )
#error "Need more RX GE cbs for BT_RC_MAX_PORTS_PER_SESSION > 10"
#endif


/*===========================================================================

FUNCTION
  bt_rc_link_in_rx_gone_empty_fptr

DESCRIPTION
  This function links in the RFCOMM "Gone Empty" function into
  the receive watermark of specified DLC as necessary.

===========================================================================*/
LOCAL void bt_rc_link_in_rx_gone_empty_fptr
(
  bt_rc_dlc_type*  dlc_ptr
)
{

  void  (*ge_fptr)( void );

  if ( dlc_ptr->rx_wm_ptr != NULL )
  {
    ge_fptr = dlc_ptr->rx_wm_ptr->gone_empty_func_ptr;

    if ( (dlc_ptr->my_rx_wm_ge_fptr != NULL) &&
         (ge_fptr != dlc_ptr->my_rx_wm_ge_fptr) )
    {
      if ( (ge_fptr != bt_rc_rx_wm_gone_empty_cb_0_0) &&
           (ge_fptr != bt_rc_rx_wm_gone_empty_cb_0_1) &&
           (ge_fptr != bt_rc_rx_wm_gone_empty_cb_0_2) &&
           (ge_fptr != bt_rc_rx_wm_gone_empty_cb_0_3) &&
           (ge_fptr != bt_rc_rx_wm_gone_empty_cb_0_4) &&
           (ge_fptr != bt_rc_rx_wm_gone_empty_cb_0_5) &&
           (ge_fptr != bt_rc_rx_wm_gone_empty_cb_0_6) &&
           (ge_fptr != bt_rc_rx_wm_gone_empty_cb_0_7) &&
           (ge_fptr != bt_rc_rx_wm_gone_empty_cb_0_8) &&
           (ge_fptr != bt_rc_rx_wm_gone_empty_cb_0_9) )
      {
        if ( (ge_fptr != bt_rc_rx_wm_gone_empty_cb_1_0) &&
             (ge_fptr != bt_rc_rx_wm_gone_empty_cb_1_1) &&
             (ge_fptr != bt_rc_rx_wm_gone_empty_cb_1_2) &&
             (ge_fptr != bt_rc_rx_wm_gone_empty_cb_1_3) &&
             (ge_fptr != bt_rc_rx_wm_gone_empty_cb_1_4) &&
             (ge_fptr != bt_rc_rx_wm_gone_empty_cb_1_5) &&
             (ge_fptr != bt_rc_rx_wm_gone_empty_cb_1_6) &&
             (ge_fptr != bt_rc_rx_wm_gone_empty_cb_1_7) &&
             (ge_fptr != bt_rc_rx_wm_gone_empty_cb_1_8) &&
             (ge_fptr != bt_rc_rx_wm_gone_empty_cb_1_9) )
        {
          if ( (ge_fptr != bt_rc_rx_wm_gone_empty_cb_2_0) &&
               (ge_fptr != bt_rc_rx_wm_gone_empty_cb_2_1) &&
               (ge_fptr != bt_rc_rx_wm_gone_empty_cb_2_2) &&
               (ge_fptr != bt_rc_rx_wm_gone_empty_cb_2_3) &&
               (ge_fptr != bt_rc_rx_wm_gone_empty_cb_2_4) &&
               (ge_fptr != bt_rc_rx_wm_gone_empty_cb_2_5) &&
               (ge_fptr != bt_rc_rx_wm_gone_empty_cb_2_6) &&
               (ge_fptr != bt_rc_rx_wm_gone_empty_cb_2_7) &&
               (ge_fptr != bt_rc_rx_wm_gone_empty_cb_2_8) &&
               (ge_fptr != bt_rc_rx_wm_gone_empty_cb_2_9) )
          {
            INTLOCK();
            dlc_ptr->orig_rx_wm_ge_fptr = ge_fptr;
            dlc_ptr->rx_wm_ptr->gone_empty_func_ptr =
              dlc_ptr->my_rx_wm_ge_fptr;
            INTFREE();
          }
        }
      }
    }
  }

}


/*===========================================================================

FUNCTION
  bt_rc_calc_optimal_mfs

DESCRIPTION
  Determines optimal MFS size given a max size and a watermark.

===========================================================================*/
uint32 bt_rc_calc_optimal_mfs
(
 uint32              mfs,
 dsm_watermark_type* wm
)
{
#define BT_RC_NUM_BB_PKTS(len,pyld) ( ( (len)                           \
                                        + BT_L2_MAX_OVERHEAD_BYTES      \
                                        + BT_RC_MAX_OVERHEAD_BYTES)  /  \
                                      ( (pyld) ) )

#define BT_RC_MFS_CALC(pyld, num_bb_pkts) (( (pyld) * (num_bb_pkts) )   \
                                            - BT_RC_MAX_OVERHEAD_BYTES  \
                                            - BT_L2_MAX_OVERHEAD_BYTES )

  int32  dne = ( wm->dont_exceed_cnt - 50 );
  /* Subtract some "fudge factor" bytes from the WM.
   * The "fudge" bytes are necessary to allow for SIO applications which may
   * leave some bytes on a small wm. If the application leaves more than
   * "fudge" bytes on their WM, the remote device will run out of credits
   * and RC may not provide more until the WM is emptied */
  if ( dne < 0 )
  {
    dne = 0;
  }

  if( mfs > dne )
  {
    /* The DNE is too small. Lower the MFS and try to make it fit in sane 
     * BB packets */
    /* TODO: This should be updated to only check 3DHx and 2DHx in the case of
     * both sides supporting EDR */
    if( BT_RC_NUM_BB_PKTS( dne, BT_3DH5_PAYLOAD_MAX_LEN ) > 0 )
    {
      mfs = BT_RC_MFS_CALC( BT_3DH5_PAYLOAD_MAX_LEN,
                            BT_RC_NUM_BB_PKTS( dne, BT_3DH5_PAYLOAD_MAX_LEN ) );
    }
    else if( BT_RC_NUM_BB_PKTS( dne, BT_2DH5_PAYLOAD_MAX_LEN ) > 0 )
    {
      mfs = BT_RC_MFS_CALC( BT_2DH5_PAYLOAD_MAX_LEN,
                            BT_RC_NUM_BB_PKTS( dne, BT_2DH5_PAYLOAD_MAX_LEN ) );
    }
    else if( BT_RC_NUM_BB_PKTS( dne, BT_3DH3_PAYLOAD_MAX_LEN ) > 0 )
    {
      mfs = BT_RC_MFS_CALC( BT_3DH3_PAYLOAD_MAX_LEN,
                            BT_RC_NUM_BB_PKTS( dne, BT_3DH3_PAYLOAD_MAX_LEN ) );
    }
    else if( BT_RC_NUM_BB_PKTS( dne, BT_2DH3_PAYLOAD_MAX_LEN ) > 0 )
    {
      mfs = BT_RC_MFS_CALC( BT_2DH3_PAYLOAD_MAX_LEN,
                            BT_RC_NUM_BB_PKTS( dne, BT_2DH3_PAYLOAD_MAX_LEN ) );
    }
    else if( BT_RC_NUM_BB_PKTS( dne, BT_DH5_PAYLOAD_MAX_LEN ) > 0 )
    {
      mfs = BT_RC_MFS_CALC( BT_DH5_PAYLOAD_MAX_LEN,
                            BT_RC_NUM_BB_PKTS( dne, BT_DH5_PAYLOAD_MAX_LEN ) );
    }
    else if ( BT_RC_NUM_BB_PKTS( dne, BT_DM5_PAYLOAD_MAX_LEN ) > 0)
    {
      mfs = BT_RC_MFS_CALC( BT_DM5_PAYLOAD_MAX_LEN,
                            BT_RC_NUM_BB_PKTS( dne, BT_DM5_PAYLOAD_MAX_LEN ) );

    }
    else if ( BT_RC_NUM_BB_PKTS( dne, BT_DH3_PAYLOAD_MAX_LEN ) > 0)
    {
      mfs = BT_RC_MFS_CALC( BT_DH3_PAYLOAD_MAX_LEN,
                            BT_RC_NUM_BB_PKTS( dne, BT_DH3_PAYLOAD_MAX_LEN ) );
    }
    else if ( BT_RC_NUM_BB_PKTS( dne, BT_DM3_PAYLOAD_MAX_LEN ) > 0)
    {
      mfs = BT_RC_MFS_CALC( BT_DM3_PAYLOAD_MAX_LEN,
                            BT_RC_NUM_BB_PKTS( dne, BT_DM3_PAYLOAD_MAX_LEN ) );
    }
    else
    {
      mfs = BT_RC_MFS_CALC( BT_DH1_PAYLOAD_MAX_LEN,
                            BT_RC_NUM_BB_PKTS( dne, BT_DH1_PAYLOAD_MAX_LEN ) );
    }

    if ( mfs < BT_RC_MFS_MIN )
    {
      mfs = BT_RC_MFS_MIN;
      BT_ERR("BT RC: Ridiculously Small MFS calc! %x",
             mfs,0,0);
    }
    else if ( mfs < BT_RC_MFS_CALC( BT_DH5_PAYLOAD_MAX_LEN, 1 ) )
    {
      BT_ERR("BT RC: Small MFS calc! %x",
             mfs,0,0);
    }
    BT_MSG_DEBUG("BT RC: Limit max MFS %x due to WM DNE %x: %x",
                 mfs,
                 wm->dont_exceed_cnt,
                 mfs);
  }
  return mfs;
}

/*===========================================================================

FUNCTION
  bt_rc_need_port_negotiation
  
DESCRIPTION
  Checks whether a specified DLC element requires an RFCOMM Port
  Negotiation.  This is necessary if the configuration fields
  corresponding to an MCC PN command are not the default values and
  the negotiation has not yet occurred.
  
RETURN VALUE
  TRUE if PN must occur, otherwise FALSE.

===========================================================================*/
LOCAL boolean bt_rc_need_port_negotiation
(
  bt_rc_dlc_type*  dlc_ptr
)
{

  boolean  pn_needed = FALSE;

  if ( dlc_ptr->config.pn_negot_done == FALSE )
  {
    /*  Always do PN negotiation to be sure  */
    /*  Credit Based Flow Control is setup   */
    /*  consistently on both sides.          */
    pn_needed = TRUE;

#if 0
    if ( dlc_ptr->config.cbfc_enabled != FALSE )
    {
      pn_needed = TRUE;
    }

    if ( dlc_ptr->config.max_frame_size != BT_RC_MFS_DEFAULT )
    {
      pn_needed = TRUE;
    }

    if ( dlc_ptr->config.priority !=
           bt_rc_get_default_priority_for_dlci( dlc_ptr->dlci ) )
    {
      pn_needed = TRUE;
    }
#endif
  
  }

  return ( pn_needed );

}


/*===========================================================================

FUNCTION
  bt_rc_initialize_config
  
DESCRIPTION
  Initializes a specified configuration element.

===========================================================================*/
LOCAL void bt_rc_initialize_config
(
  bt_rc_config_type*  cfg_ptr
)
{

  cfg_ptr->pn_negot_done      = FALSE;
  cfg_ptr->rpn_negot_done     = FALSE;
  cfg_ptr->max_frame_size     = BT_MTP_RC_INFO_LEN;
  cfg_ptr->cbfc_enabled       = TRUE;
  cfg_ptr->cbfc_local_credits = 0;
  cfg_ptr->cbfc_far_credits   = BT_RC_CBFC_CREDITS_DEFAULT;
  cfg_ptr->priority           = 0xFF;
  bt_rc_initialize_sp_config( cfg_ptr );

}


/*===========================================================================

FUNCTION
  bt_rc_init_reg_table_entry
  
DESCRIPTION
  Initializes a specified registration table entry.

===========================================================================*/
LOCAL void bt_rc_init_reg_table_entry
(
  bt_rc_reg_table_type*  rte_ptr
)
{

  rte_ptr->bt_app_id      = BT_APP_ID_NULL;
  rte_ptr->server_channel = BT_RC_SC_NULL;
  rte_ptr->tx_wm_ptr      = NULL;
  rte_ptr->rx_wm_ptr      = NULL;
  rte_ptr->connected      = FALSE;
  rte_ptr->unreg_pending  = FALSE;

}


/*===========================================================================

FUNCTION
  bt_rc_init_negot_table_entry
  
DESCRIPTION
  Initializes a specified negotiation table entry.

===========================================================================*/
LOCAL void bt_rc_init_negot_table_entry
(
  bt_rc_negot_table_type*  nte_ptr
)
{

  nte_ptr->bt_app_id        = BT_APP_ID_NULL;
  nte_ptr->dlci             = 0xFF;
  nte_ptr->server_channel   = BT_RC_SC_NULL;
  memset( (byte*)(&(nte_ptr->bd_addr)), 0, sizeof( bt_bd_addr_type ) );
  nte_ptr->expected_rx_mask = 0x0000;
  bt_rc_initialize_config( &nte_ptr->config );

}


/*===========================================================================

FUNCTION
  bt_rc_initialize_dlc_entry

DESCRIPTION
  Initializes a specified DLC table entry.

===========================================================================*/
LOCAL void bt_rc_initialize_dlc_entry
(
  bt_rc_dlc_type*  dlc_ptr
)
{

  dlc_ptr->state = BT_RCDS_DISCONNECTED;
  dlc_ptr->expected_rx_mask = 0x0000;
  dlc_ptr->peer_to_peer_timer_ms_left = 0;
  dlc_ptr->dlci = 0xFF;
  dlc_ptr->direction = 0xF0;
  dlc_ptr->server_channel = BT_RC_SC_NULL;
  dlc_ptr->bt_app_id = BT_APP_ID_NULL;
  bt_rc_initialize_config( &dlc_ptr->config );
  dlc_ptr->tx_wm_ptr = NULL;
  dlc_ptr->rx_wm_ptr = NULL;
  dlc_ptr->orig_rx_wm_ge_fptr = NULL;
  dlc_ptr->tx_pkt_ptr = NULL;
  bt_rc_initialize_fast_uih_fcs( &dlc_ptr->tx_fast_uih_pf0 );
  bt_rc_initialize_fast_uih_fcs( &dlc_ptr->tx_fast_uih_pf1 );
  bt_rc_initialize_fast_uih_fcs( &dlc_ptr->rx_fast_uih_pf0 );
  bt_rc_initialize_fast_uih_fcs( &dlc_ptr->rx_fast_uih_pf1 );
  dlc_ptr->ack_timeout = 6000;   /*  60 seconds.  */
  dlc_ptr->mcc_timeout = 6000;   /*  60 seconds.  */
  dlc_ptr->local_v24_signals = RCMCCM_MSC_V24_LOC_DEFAULT_V;
  dlc_ptr->local_breaks = 0;
  dlc_ptr->local_last_break_length = 0;
  dlc_ptr->last_msc_tx_v24_signals = ~dlc_ptr->local_v24_signals;
  dlc_ptr->last_msc_tx_breaks = 0;
  dlc_ptr->last_msc_tx_break_signals = 0x0;
  dlc_ptr->local_overruns = 0;
  dlc_ptr->local_parity_errors = 0;
  dlc_ptr->local_framing_errors = 0;
  dlc_ptr->last_rls_tx_overruns = 0;
  dlc_ptr->last_rls_tx_parity_errors = 0;
  dlc_ptr->last_rls_tx_framing_errors = 0;
  dlc_ptr->last_rls_tx_value1 = 0x0;
  dlc_ptr->far_v24_signals = RCMCCM_MSC_V24_FAR_DEFAULT_V;
  dlc_ptr->far_breaks = 0;
  dlc_ptr->far_last_break_length = 0;
  dlc_ptr->far_overruns = 0;
  dlc_ptr->far_parity_errors = 0;
  dlc_ptr->far_framing_errors = 0;
  dlc_ptr->tx_errors = 0;
  dlc_ptr->rx_errors = 0;
  dlc_ptr->tx_data_bytes = 0;
  dlc_ptr->rx_data_bytes = 0;
  dlc_ptr->tx_uih_frames = 0;
  dlc_ptr->rx_uih_frames = 0;
  dlc_ptr->tx_sabm_frames = 0;
  dlc_ptr->rx_sabm_frames = 0;
  dlc_ptr->tx_ua_frames = 0;
  dlc_ptr->rx_ua_frames = 0;
  dlc_ptr->tx_dm_frames = 0;
  dlc_ptr->rx_dm_frames = 0;
  dlc_ptr->tx_disc_frames = 0;
  dlc_ptr->rx_disc_frames = 0;

}


/*===========================================================================

FUNCTION
  bt_rc_initialize_session_entry

DESCRIPTION
  Initializes a specified session table entry.

===========================================================================*/
void bt_rc_initialize_session_entry
(
  uint8  ssn_idx
)
{

  uint8 i;

  SSN( ssn_idx ).initiator = FALSE;
  memset( (byte*)(&(SSN( ssn_idx ).bd_addr)), 0,
                  sizeof( bt_bd_addr_type ) );
  SSN( ssn_idx ).acl_tx_flow_on = TRUE;
  SSN( ssn_idx ).cid = 0;
  SSN( ssn_idx ).l2_write_cmds = 0;
  SSN( ssn_idx ).l2_rx_data_evs = 0;

  bt_rc_initialize_dlc_entry( &(SSN( ssn_idx ).mcc) );
  SSN( ssn_idx ).mcc.dlci = RC_DLCI_MCC_V;
  SSN( ssn_idx ).mcc.config.priority =
    bt_rc_get_default_priority_for_dlci( SSN( ssn_idx ).mcc.dlci );

  SSN( ssn_idx ).dlc_count = 0;

  for ( i = 0; i < BT_RC_MAX_PORTS_PER_SESSION; i++ )
  {
    bt_rc_initialize_dlc_entry( &(DLC( ssn_idx , i )) );
  }
  
  SSN( ssn_idx ).test_cmds_txd            = 0;
  SSN( ssn_idx ).test_cmds_rxd            = 0;
  SSN( ssn_idx ).test_resps_txd           = 0;
  SSN( ssn_idx ).test_resps_rxd           = 0;
  SSN( ssn_idx ).test_errors              = 0;
  SSN( ssn_idx ).last_test_tx_bt_app_id   = BT_APP_ID_NULL;
  SSN( ssn_idx ).last_test_tx_pid         = BT_RC_NULL_PID;
  SSN( ssn_idx ).last_test_tx_byte_count  = 0;
  SSN( ssn_idx ).last_test_tx_start_value = 0;
  SSN( ssn_idx ).last_test_tx_increment   = 0;
  SSN( ssn_idx ).l2_disc_wait             = FALSE;

}


/*===========================================================================

FUNCTION
  bt_rc_store_bt_event

DESCRIPTION
  Stores a specified BT event on the RFCOMM BT event queue.

===========================================================================*/
LOCAL void bt_rc_store_bt_event
(
  bt_ev_msg_type*  bt_ev_ptr
)
{

  bt_store_bt_event( bt_ev_ptr, &bt_rc_ev_q_info );

}


/*===========================================================================

FUNCTION
  bt_rc_process_ev_queue

DESCRIPTION
  Dequeues events on the RFCOMM event queue and processes them.

===========================================================================*/
void bt_rc_process_ev_queue( void )
{

  bt_ev_msg_type*  bt_ev_ptr;
  int              q_count = q_cnt( &bt_rc_ev_q );

  BT_VSTAT_MAX( q_count, RC_MAX_EV_Q_DEPTH );

  BT_MSG_API_PYLD( "RC ev Q - cur_cnt = %x, max_cnt = %x",
                   q_count,
                   bt_vital_stats[BT_VSAI_RC_MAX_EV_Q_DEPTH], 0);

  while ( (bt_ev_ptr = (bt_ev_msg_type *)q_get( &bt_rc_ev_q )) != NULL )
  {
    bt_rc_process_event( bt_ev_ptr );
    q_put( &bt_event_free_q, &bt_ev_ptr->ev_hdr.link );
  }

}


/*===========================================================================

FUNCTION
  bt_rc_powerup_init

DESCRIPTION
  Initializes entire RFCOMM layer upon powerup.

===========================================================================*/
void bt_rc_powerup_init( void )
{

   uint8  i, j;

   bt_rc_app_id = bt_cmd_ec_get_app_id_and_register(
                    bt_rc_store_bt_event );
   ASSERT_DEBUG( bt_rc_app_id != BT_APP_ID_NULL );
   BT_MSG_API( "BT RC TRX: EC Get AID %x", bt_rc_app_id, 0, 0 );
   BT_MSG_HIGH( "BT RC AID is %x", bt_rc_app_id, 0, 0 );
   
   timer_def( &bt_rc_tx_timer, 
              &bt_always_on_timer_group, &bt_tcb, 
              BT_RC_TX_TIMER_SIG, NULL, 0 );

   bt_rc_tx_timer_running = FALSE;

   (void) q_init( &bt_rc_ev_q );
   bt_rc_ev_q_info.event_q_ptr      = &bt_rc_ev_q;
   bt_rc_ev_q_info.max_q_depth      = BT_RC_NUM_EVENTS;
   bt_rc_ev_q_info.event_q_bit_mask = BT_EVQ_RC_B;
   bt_rc_ev_q_info.max_event_bytes  = bt_rc_get_max_event_bytes();

   bt_rc_reg_count = 0;
   for ( i = 0; i < BT_RC_MAX_PORTS; i++ )
   {
     bt_rc_init_reg_table_entry( &bt_rc_reg_table[ i ] );
   }

   bt_rc_negot_count = 0;
   for ( i = 0; i < BT_RC_MAX_PORTS; i++ )
   {
     bt_rc_init_negot_table_entry( &bt_rc_negot_table[ i ] );
   }
   
   for ( i = 0; i < BT_RC_MAX_SESSIONS; i++ )
   {
     SSN( i ).ssn_idx = i;
     bt_rc_initialize_session_entry( i );

     SSN( i ).mcc.pid = SIDX_DIDX_2_PID( i, 0xCC );  /*  Never used!  */
     SSN( i ).mcc.my_rx_wm_ge_fptr = NULL;

     for ( j = 0; j < BT_RC_MAX_PORTS_PER_SESSION; j++ )
     {
       DLC( i, j ).pid = SIDX_DIDX_2_PID( i, j );
     }

   }

   /*  There must be one bt_rc_rx_wm_gone_empty_cb_*_* per  */
   /*  [ BT_RC_MAX_SESSIONS, BT_RC_MAX_PORTS_PER_SESSION ]  */
   /*  referenced below.                                    */

   DLC( 0, 0 ).my_rx_wm_ge_fptr = bt_rc_rx_wm_gone_empty_cb_0_0;
   DLC( 0, 1 ).my_rx_wm_ge_fptr = bt_rc_rx_wm_gone_empty_cb_0_1;
   DLC( 0, 2 ).my_rx_wm_ge_fptr = bt_rc_rx_wm_gone_empty_cb_0_2;
   DLC( 0, 3 ).my_rx_wm_ge_fptr = bt_rc_rx_wm_gone_empty_cb_0_3;
   DLC( 0, 4 ).my_rx_wm_ge_fptr = bt_rc_rx_wm_gone_empty_cb_0_4;
   DLC( 0, 5 ).my_rx_wm_ge_fptr = bt_rc_rx_wm_gone_empty_cb_0_5;
   DLC( 0, 6 ).my_rx_wm_ge_fptr = bt_rc_rx_wm_gone_empty_cb_0_6;
   DLC( 0, 7 ).my_rx_wm_ge_fptr = bt_rc_rx_wm_gone_empty_cb_0_7;
   DLC( 0, 8 ).my_rx_wm_ge_fptr = bt_rc_rx_wm_gone_empty_cb_0_8;
   DLC( 0, 9 ).my_rx_wm_ge_fptr = bt_rc_rx_wm_gone_empty_cb_0_9;

   DLC( 1, 0 ).my_rx_wm_ge_fptr = bt_rc_rx_wm_gone_empty_cb_1_0;
   DLC( 1, 1 ).my_rx_wm_ge_fptr = bt_rc_rx_wm_gone_empty_cb_1_1;
   DLC( 1, 2 ).my_rx_wm_ge_fptr = bt_rc_rx_wm_gone_empty_cb_1_2;
   DLC( 1, 3 ).my_rx_wm_ge_fptr = bt_rc_rx_wm_gone_empty_cb_1_3;
   DLC( 1, 4 ).my_rx_wm_ge_fptr = bt_rc_rx_wm_gone_empty_cb_1_4;
   DLC( 1, 5 ).my_rx_wm_ge_fptr = bt_rc_rx_wm_gone_empty_cb_1_5;
   DLC( 1, 6 ).my_rx_wm_ge_fptr = bt_rc_rx_wm_gone_empty_cb_1_6;
   DLC( 1, 7 ).my_rx_wm_ge_fptr = bt_rc_rx_wm_gone_empty_cb_1_7;
   DLC( 1, 8 ).my_rx_wm_ge_fptr = bt_rc_rx_wm_gone_empty_cb_1_8;
   DLC( 1, 9 ).my_rx_wm_ge_fptr = bt_rc_rx_wm_gone_empty_cb_1_9;
   
   DLC( 2, 0 ).my_rx_wm_ge_fptr = bt_rc_rx_wm_gone_empty_cb_2_0;
   DLC( 2, 1 ).my_rx_wm_ge_fptr = bt_rc_rx_wm_gone_empty_cb_2_1;
   DLC( 2, 2 ).my_rx_wm_ge_fptr = bt_rc_rx_wm_gone_empty_cb_2_2;
   DLC( 2, 3 ).my_rx_wm_ge_fptr = bt_rc_rx_wm_gone_empty_cb_2_3;
   DLC( 2, 4 ).my_rx_wm_ge_fptr = bt_rc_rx_wm_gone_empty_cb_2_4;
   DLC( 2, 5 ).my_rx_wm_ge_fptr = bt_rc_rx_wm_gone_empty_cb_2_5;
   DLC( 2, 6 ).my_rx_wm_ge_fptr = bt_rc_rx_wm_gone_empty_cb_2_6;
   DLC( 2, 7 ).my_rx_wm_ge_fptr = bt_rc_rx_wm_gone_empty_cb_2_7;
   DLC( 2, 8 ).my_rx_wm_ge_fptr = bt_rc_rx_wm_gone_empty_cb_2_8;
   DLC( 2, 9 ).my_rx_wm_ge_fptr = bt_rc_rx_wm_gone_empty_cb_2_9;
   
   bt_rc_ideal_info_len = BT_MTP_RC_INFO_LEN;

}


/*===========================================================================

FUNCTION
  bt_rc_qdsp_ready_init

DESCRIPTION
  Initialize the RFCOMM layer when the Bluetooth QDSP image is loaded.

===========================================================================*/
void bt_rc_qdsp_ready_init( void )
{
  bt_rc_restart_tx_flow();
}


/*===========================================================================

FUNCTION
  bt_rc_qdsp_reset_shutdown

DESCRIPTION
  De-initialize the RFCOMM layer when the Bluetooth QDSP image is unloaded.

===========================================================================*/
void bt_rc_qdsp_reset_shutdown( void )
{

  rex_clr_timer( &bt_rc_tx_timer );
  rex_clr_sigs( &bt_tcb, BT_RC_TX_TIMER_SIG );
  bt_rc_tx_timer_running = FALSE;

}

/*===========================================================================

FUNCTION
  bt_rc_soc_reset_shutdown

DESCRIPTION
  De-initialize the RFCOMM layer when the Bluetooth SOC is powered down.

===========================================================================*/
void bt_rc_soc_reset_shutdown( void )
{
  rex_clr_timer( &bt_rc_tx_timer );
  rex_clr_sigs( &bt_tcb, BT_RC_TX_TIMER_SIG );
  bt_rc_tx_timer_running = FALSE;
}

/*===========================================================================

FUNCTION
  bt_rc_configuration_is_valid

DESCRIPTION
  Determines whether specified RFCOMM configuration values are valid.

RETURN VALUE
  TRUE if values are valid, otherwise FALSE.

===========================================================================*/
LOCAL boolean bt_rc_configuration_is_valid
(
  uint16  max_frame_size,
  uint8   priority,
  uint8   baudrate,
  uint8   format,
  uint8   flow_ctrl
)
{

  boolean cfg_is_valid = TRUE;

  if ( (max_frame_size < BT_RC_MFS_MIN) ||
       (max_frame_size > BT_RC_MFS_MAX) )
  {
    BT_ERR( "BT RC: Bad cfg MFS %x", max_frame_size, 0, 0 );
    cfg_is_valid = FALSE;
  }

  if ( priority > BT_RC_PRI_MAX )
  {
    BT_ERR( "BT RC: Bad cfg Pri %x", priority, 0, 0 );
    cfg_is_valid = FALSE;
  }
  
  if ( /*  (baudrate < RCMCCM_RPN_BR_MIN_V) ||  */
       /*  Compiler warns on the above due to   */
       /*  unsigned compared with 0...          */
       (baudrate > RCMCCM_RPN_BR_MAX_V) )
  {
    BT_ERR( "BT RC: Bad cfg BR %x", baudrate, 0, 0 );
    cfg_is_valid = FALSE;
  }

  if ( (format & ~RCMCCM_RPN_FMT_M) != 0 )
  {
    BT_ERR( "BT RC: Bad cfg Format %x", format, 0, 0 );
    cfg_is_valid = FALSE;
  }

  if ( (flow_ctrl & ~RCMCCM_RPN_FC_M) != 0 )
  {
    BT_ERR( "BT RC: Bad cfg FC %x", flow_ctrl, 0, 0 );
    cfg_is_valid = FALSE;
  }

  return ( cfg_is_valid );

}

  
/*===========================================================================

FUNCTION
  bt_rc_find_local_server_pid

DESCRIPTION
  Finds a specified local server's PID.

RETURN VALUE
  PID if found; otherwise BT_RC_NULL_PID.

===========================================================================*/
LOCAL uint16 bt_rc_find_local_server_pid
(
  uint8  server_channel
)
{

  uint8   i, j;
  uint16  return_pid = BT_RC_NULL_PID;

  for ( i = 0; i < BT_RC_MAX_SESSIONS; i++ )
  {
    if ( SSN( i ).mcc.state != BT_RCDS_DISCONNECTED )
    {
      /*  For local servers, the direction of the DLC is  */
      /*  equal to the multiplexer's initiator setting.   */

      for ( j = 0; j < BT_RC_MAX_PORTS_PER_SESSION; j++ )
      {
        if ( DLC( i, j ).state != BT_RCDS_DISCONNECTED )
        {
          if ( (DLC( i, j ).server_channel == server_channel) &&
               (DLC( i, j ).direction == SSN( i ).initiator) )
          {
            return_pid = SIDX_DIDX_2_PID( i, j );
            break;
          }
        }
      }
    }
  }

  return ( return_pid );

}


/*===========================================================================

FUNCTION
  bt_rc_find_remote_server_dlc_entry

DESCRIPTION
  Finds a specified remote server's DLC table entry within a specified
  session table entry.

RETURN VALUE
  Pointer to DLC table entry, if found; otherwise NULL.

===========================================================================*/
LOCAL bt_rc_dlc_type* bt_rc_find_remote_server_dlc_entry
(
  bt_rc_session_type*  ssn_ptr,
  uint8                server_channel
)
{

  uint8            i;
  bt_rc_dlc_type*  dlc_ptr = NULL;

  /*  For remote servers, the direction of the DLC is the     */
  /*  opposite of the local multiplexer's initiator setting.  */

  for ( i = 0; i < BT_RC_MAX_PORTS_PER_SESSION; i++ )
  {
    if ( ssn_ptr->dlc[ i ].state != BT_RCDS_DISCONNECTED )
    {
      if ( (ssn_ptr->dlc[ i ].direction != ssn_ptr->initiator) &&
           (ssn_ptr->dlc[ i ].server_channel == server_channel) )
      {
        dlc_ptr = &ssn_ptr->dlc[ i ];
        break;
      }
    }
  }

  return ( dlc_ptr );

}


/*===========================================================================

FUNCTION
  bt_rc_restart_tx_flow

DESCRIPTION
  Primes all RFCOMM transmits again by sending an RFCOMM Transmit
  Data command for each active PID.

===========================================================================*/
void bt_rc_restart_tx_flow
(
  void
)
{

#ifdef BT_FLOW_DEBUG
  BT_MSG_HIGH( "BT RC: Restart TX flow", 0, 0, 0 );
#endif

  if ( bt_rc_tx_timer_running != FALSE )
  {
    rex_clr_timer( &bt_rc_tx_timer );
    bt_rc_tx_timer_running = FALSE;
  }
  rex_set_sigs( &bt_tcb, BT_RC_TX_TIMER_SIG );

}


/*===========================================================================

FUNCTION
  bt_rc_update_acl_tx_flow_on

DESCRIPTION
  Sets the acl_tx_flow_on flag for a specified BD address to a
  specified value.

===========================================================================*/
void bt_rc_update_acl_tx_flow_on
(
  bt_bd_addr_type*  bd_addr_ptr,
  boolean           acl_tx_flow_on
)
{

  bt_rc_session_type*  ssn_ptr;

  if ( (ssn_ptr = bt_rc_find_session_bd_addr( bd_addr_ptr )) != NULL )
  {
#ifdef BT_FLOW_DEBUG
    BT_MSG_HIGH( "BT RC: Update BB/HC TX flow %x CID %x",
                 acl_tx_flow_on, ssn_ptr->cid, 0 );
#endif

    if( (ssn_ptr->acl_tx_flow_on == FALSE) &&
        (acl_tx_flow_on != FALSE) )
    {
      bt_rc_restart_tx_flow();
    }
    ssn_ptr->acl_tx_flow_on = acl_tx_flow_on;
  }

} /* bt_rc_update_acl_tx_flow_on */


/*===========================================================================

FUNCTION
  bt_rc_get_rx_flow_on

DESCRIPTION
  Checks the receive flow state of all active PIDs and returns the
  appropriate value for the ARM to QDSP receive flow on flag.

RETURN VALUE
  FALSE:  if any active PID's flow is OFF
  TRUE:   iff all active PID's flows are ON

===========================================================================*/
LOCAL boolean bt_rc_get_rx_flow_on
(
  void
)
{

  uint8    i, j;
  boolean  rx_flow_on = TRUE;

  for ( i = 0; i < BT_RC_MAX_SESSIONS; i++ )
  {
    if ( SSN( i ).mcc.state == BT_RCDS_CONNECTED )
    {
      for ( j = 0; j < BT_RC_MAX_PORTS_PER_SESSION; j++ )
      {
        if ( DLC( i, j ).state == BT_RCDS_CONNECTED )
        {
          if ( DLC( i, j ).local_v24_signals &
                 RCMCCM_MSC_V24_FC_FLOW_OFF_B )
          {
            rx_flow_on = FALSE;
          }
        }
      }
    }
  }

  return ( rx_flow_on );

}


/*===========================================================================

FUNCTION
  bt_rc_pid_to_ssn_dlc_ptrs

DESCRIPTION
  Converts a specified PID to the corresponding session and DLC pointers.

RETURN VALUES
  TRUE:   PID converted to valid and active session and DLC pointers;
          *ssn_ptr points to a valid session table entry
          *dlc_ptr points to a valid DLC table entry

  FALSE:  PID did not convert to valid session and DLC pointers;
          *ssn_ptr is NULL
          *dlc_ptr is NULL

===========================================================================*/
LOCAL boolean bt_rc_pid_to_ssn_dlc_ptrs
(
  uint16                pid,
  bt_rc_session_type**  ssn_ptr_ptr,
  bt_rc_dlc_type**      dlc_ptr_ptr
)
{

  uint8    ssn_idx, dlc_idx;
  boolean  ptrs_ok = FALSE;

  *ssn_ptr_ptr = NULL;
  *dlc_ptr_ptr = NULL;

  ssn_idx = ((pid >> 8) & 0x00FF);
  dlc_idx = (pid & 0x00FF);

  if ( (ssn_idx < BT_RC_MAX_SESSIONS) &&
       (dlc_idx < BT_RC_MAX_PORTS_PER_SESSION) )
  {
    if ( (bt_rc_session[ ssn_idx ].ssn_idx == ssn_idx) &&
         (bt_rc_session[ ssn_idx ].dlc[ dlc_idx ].pid == pid) )
    {
      if ( (bt_rc_session[ ssn_idx ].mcc.state !=
              BT_RCDS_DISCONNECTED) &&
           (bt_rc_session[ ssn_idx ].dlc[ dlc_idx ].state !=
              BT_RCDS_DISCONNECTED) )
      {
        *ssn_ptr_ptr = &bt_rc_session[ ssn_idx ];
        *dlc_ptr_ptr = &bt_rc_session[ ssn_idx ].dlc[ dlc_idx ];
        ptrs_ok = TRUE;
      }
    }
  }

  return ( ptrs_ok );

}


/*===========================================================================

FUNCTION
  bt_rc_pid_to_ssn_ptr

DESCRIPTION
  Converts a specified PID to the corresponding session pointer.

RETURN VALUES
  TRUE:   PID converted to valid and active session pointer;
          *ssn_ptr points to a valid session table entry

  FALSE:  PID did not convert to valid session pointer;
          *ssn_ptr is NULL

===========================================================================*/
LOCAL boolean bt_rc_pid_to_ssn_ptr
(
  uint16                pid,
  bt_rc_session_type**  ssn_ptr_ptr
)
{

  uint8    ssn_idx;
  boolean  ptr_ok = FALSE;

  *ssn_ptr_ptr = NULL;

  ssn_idx = ((pid >> 8) & 0x00FF);

  if ( ssn_idx < BT_RC_MAX_SESSIONS )
  {
    if ( bt_rc_session[ ssn_idx ].ssn_idx == ssn_idx )
    {
      if ( bt_rc_session[ ssn_idx ].mcc.state != BT_RCDS_DISCONNECTED )
      {
        *ssn_ptr_ptr = &bt_rc_session[ ssn_idx ];
        ptr_ok = TRUE;
      }
    }
  }

  return ( ptr_ok );

}


/*===========================================================================

FUNCTION
  bt_rc_update_wm_ptrs

DESCRIPTION
  Updates a specified PID's watermark pointers.

===========================================================================*/
void bt_rc_update_wm_ptrs
(
  uint16               rc_pid,
  dsm_watermark_type*  tx_wm_ptr,
  dsm_watermark_type*  rx_wm_ptr
)
{

  bt_rc_session_type*  ssn_ptr;
  bt_rc_dlc_type*      dlc_ptr;

  INTLOCK();

  if ( bt_rc_pid_to_ssn_dlc_ptrs( rc_pid, &ssn_ptr, &dlc_ptr ) != FALSE )
  {
    dlc_ptr->rx_wm_ptr = rx_wm_ptr;
    dlc_ptr->tx_wm_ptr = tx_wm_ptr;
  }
  else
  {
    BT_ERR( "BT RC: Adj WM bad PID %x", rc_pid, 0, 0 );
  }

  INTFREE();

}


/*===========================================================================

FUNCTION
  bt_rc_find_regd_server_channel

DESCRIPTION
  Finds a specified server channel's entry in the RFCOMM
  registration table.

RETURN VALUE
  Pointer to RFCOMM registration table entry, if found; otherwise NULL.

===========================================================================*/
bt_rc_reg_table_type* bt_rc_find_regd_server_channel
(
  uint8  server_channel
)
{

  uint8                  i = 0;
  bt_rc_reg_table_type*  reg_ptr = NULL;

  while ( (i < bt_rc_reg_count) && (reg_ptr == NULL) )
  {
    if ( bt_rc_reg_table[ i ].server_channel == server_channel )
    {
      reg_ptr = &bt_rc_reg_table[ i ];
    }
    i++;
  }

  return ( reg_ptr );

}


/*===========================================================================

FUNCTION
  bt_rc_find_free_negot_table_entry

DESCRIPTION
  Returns a pointer to a free negotiation table entry.  This will
  either be a currently unused entry or the oldest entry in the
  table will be re-initialized and used.

RETURN VALUE
  Pointer to a free negotiation table entry

===========================================================================*/
bt_rc_negot_table_type* bt_rc_find_free_negot_table_entry
(
  void
)
{

  bt_rc_negot_table_type*  nte_ptr = NULL;

  if ( bt_rc_negot_count < BT_RC_MAX_PORTS )
  {
    nte_ptr = &bt_rc_negot_table[ bt_rc_negot_count ];
    bt_rc_negot_count++;
  }
  else
  {
    /*  Overwrite oldest entry is the best we can do. */
    nte_ptr = &bt_rc_negot_table[ 0 ];
  }
  bt_rc_init_negot_table_entry( nte_ptr );

  return ( nte_ptr );

}


/*===========================================================================

FUNCTION
  bt_rc_find_negot_table_entry

DESCRIPTION
  Finds the negotiation table entry with a specified BD address and DLCI.
  If not found, this routine will create a new one if specified.  This
  will either be a currently unused entry or the oldest entry in the
  table will be re-initialized and used.  When creating an entry, the
  DLCI and BD Address is filled in by this routine.

RETURN VALUE
  Pointer to negotiation table entry if found; otherwise NULL.

===========================================================================*/
bt_rc_negot_table_type* bt_rc_find_negot_table_entry
(
  uint8             dlci,
  bt_bd_addr_type*  bd_addr_ptr,
  boolean           create_entry
)
{

  uint8                    i = 0;
  bt_rc_negot_table_type*  nte_ptr = NULL;

  while ( (i < bt_rc_negot_count) && (nte_ptr == NULL) )
  {
    if ( (bt_rc_negot_table[ i ].bt_app_id == BT_APP_ID_NULL) &&
         (BT_BD_ADDRS_EQUAL( &bt_rc_negot_table[ i ].bd_addr,
                             bd_addr_ptr )) &&
         (bt_rc_negot_table[ i ].dlci == dlci) )
    {
      nte_ptr = &bt_rc_negot_table[ i ];
    }
    i++;
  }

  if ( (nte_ptr == NULL) && (create_entry != FALSE) )
  {
    nte_ptr = bt_rc_find_free_negot_table_entry();
    nte_ptr->bd_addr = *bd_addr_ptr;
    nte_ptr->dlci    = dlci;
    nte_ptr->config.priority =
      bt_rc_get_default_priority_for_dlci( dlci );
  }

  return ( nte_ptr );

}


/*===========================================================================

FUNCTION
  bt_rc_remove_negot_table_entry

DESCRIPTION
  Removes a specified negotiation table entry.

===========================================================================*/
LOCAL void bt_rc_remove_negot_table_entry
(
  bt_rc_negot_table_type*  nte_ptr
)
{

  uint8  i = 0, idx = BT_RC_MAX_PORTS;

  while ( (i < bt_rc_negot_count) && (idx == BT_RC_MAX_PORTS) )
  {
    if ( nte_ptr == &bt_rc_negot_table[ i ] )
    {
      idx = i;
    }
    i++;
  }

  /*  Remove entry from negotiation table.  */

  if ( idx != BT_RC_MAX_PORTS )
  {
    if ( idx < (bt_rc_negot_count - 1) )
    {
      bt_rc_negot_table[ idx ] =
        bt_rc_negot_table[ (bt_rc_negot_count - 1) ];
    }
    bt_rc_negot_count--;
  }
  else
  {
    BT_ERR( "BT RC: NTE not found %x Cnt %x",
            nte_ptr, bt_rc_negot_count, 0 );
  }

}


/*===========================================================================

FUNCTION
  bt_rc_cmd_register

DESCRIPTION
  Register a resident server application with RFCOMM.

===========================================================================*/
LOCAL void bt_rc_cmd_register
(
  bt_cmd_msg_type*  rc_reg_ptr
)
{

  uint8               i, j = 0;
  bt_cmd_status_type  cmd_status;

  rc_reg_ptr->cmd_hdr.cmd_status = BT_CS_GN_SUCCESS;

  BT_MSG_API( "BT RC CMD RX: Reg SC %x AID %x",
              rc_reg_ptr->cmd_msg.cmd_rc_reg.server_channel,
              rc_reg_ptr->cmd_hdr.bt_app_id, 0 );

  if ( (rc_reg_ptr->cmd_msg.cmd_rc_reg.server_channel >=
          RC_SERV_CHAN_MIN_V) && (rc_reg_ptr->cmd_msg.cmd_rc_reg.
          server_channel <= RC_SERV_CHAN_MAX_V) )
  {
    for ( i = 0; i < BT_RC_MAX_SESSIONS; i++ )
    {
      if ( SSN( i ).mcc.state != BT_RCDS_DISCONNECTED )
      {
        j += SSN( i ).dlc_count;
      }
    }
    if ( (j + bt_rc_reg_count) < BT_RC_MAX_PORTS )
    {
      /*  Register server.  */
      bt_rc_reg_table[ bt_rc_reg_count ].server_channel =
              rc_reg_ptr->cmd_msg.cmd_rc_reg.server_channel;
      bt_rc_reg_table[ bt_rc_reg_count ].bt_app_id =
              rc_reg_ptr->cmd_hdr.bt_app_id;
      bt_rc_reg_table[ bt_rc_reg_count ].tx_wm_ptr =
              rc_reg_ptr->cmd_msg.cmd_rc_reg.tx_wm_ptr;
      bt_rc_reg_table[ bt_rc_reg_count ].rx_wm_ptr =
              rc_reg_ptr->cmd_msg.cmd_rc_reg.rx_wm_ptr;
      bt_rc_reg_table[ bt_rc_reg_count ].connected = FALSE;
      bt_rc_reg_table[ bt_rc_reg_count ].unreg_pending = FALSE;

      bt_rc_reg_count++;
      
      BT_MSG_HIGH( "BT RC: Reg SC %x AID %x",
                   rc_reg_ptr->cmd_msg.cmd_rc_reg.server_channel,
                   rc_reg_ptr->cmd_hdr.bt_app_id, 0 );

      if ( bt_rc_reg_count == 1 )
      {
        BT_MSG_HIGH( "BT RC: Register with L2CAP", 0, 0, 0 );

        BT_MSG_API( "BT RC CMD TX: L2 StPrm PSM %x AID %x",
                    BT_L2_PSM_RFCOMM, bt_rc_app_id, 0 );
        cmd_status = bt_cmd_l2_set_params(
                       bt_rc_app_id,
                       BT_L2_NULL_CID,
                       BT_L2_PSM_RFCOMM,
                       BT_L2_DEFAULT_FLUSH_TIMEOUT,
                       RC_MAX_MTU,
                       BT_QOS_BEST_EFFORT,
                       BT_L2_DEFAULT_TOKEN_RATE,
                       BT_L2_DEFAULT_TOKEN_BUCKET_SIZE,
                       BT_L2_DEFAULT_PEAK_BANDWIDTH,
                       BT_L2_DEFAULT_LATENCY,
                       BT_L2_DEFAULT_DELAY_VARIATION );
        ASSERT_DEBUG( cmd_status == BT_CS_GN_PENDING );
      
        BT_MSG_API( "BT RC CMD TX: L2 Reg PSM %x AID %x",
                    BT_L2_PSM_RFCOMM, bt_rc_app_id, 0 );
        cmd_status = bt_cmd_l2_register( bt_rc_app_id,
                                         BT_L2_PSM_RFCOMM );
        ASSERT_DEBUG( cmd_status == BT_CS_GN_PENDING );
      }
    } else {
      /*  Max number of apps already active  */
      /*    and/or registered.               */
      rc_reg_ptr->cmd_hdr.cmd_status = BT_CS_RC_REG_FULL;
    }
  } else {
    /*  Server channel value out of range.  */
    rc_reg_ptr->cmd_hdr.cmd_status = BT_CS_RC_BAD_SC;
  }

}


/*===========================================================================

FUNCTION
  bt_rc_cmd_connect

DESCRIPTION
  Connect a resident client application to a remote server.

===========================================================================*/
LOCAL void bt_rc_cmd_connect
(
  bt_cmd_msg_type*  rc_conn_ptr
)
{
  bt_rc_dlc_type*      dlc_ptr;
  bt_rc_session_type*  ssn_ptr;
  bt_cmd_status_type   cmd_status;

  rc_conn_ptr->cmd_hdr.cmd_status = BT_CS_GN_SUCCESS;

  BT_MSG_API( "BT RC CMD RX: Conn SC %x AID %x",
              rc_conn_ptr->cmd_msg.cmd_rc_conn.server_channel,
              rc_conn_ptr->cmd_hdr.bt_app_id, 0 );
  BT_BDA( MSG_API, "BT RC CMD RX: Conn",
          &rc_conn_ptr->cmd_msg.cmd_rc_conn.bd_addr );

  /*  Is a session already active with remote device?  */

  if ( (ssn_ptr = bt_rc_find_session_bd_addr( &rc_conn_ptr->
          cmd_msg.cmd_rc_conn.bd_addr )) == NULL )
  {
    /*  No... start RFCOMM session with remote device.  */

    if ( (ssn_ptr = bt_rc_find_free_session_entry()) != NULL )
    {
      ssn_ptr->initiator = TRUE;
      ssn_ptr->bd_addr = rc_conn_ptr->cmd_msg.cmd_rc_conn.bd_addr;
      ssn_ptr->mcc.server_channel = rc_conn_ptr->cmd_msg.
        cmd_rc_conn.server_channel;

      /*  For local clients, the direction of the DLC is the  */
      /*  opposite of the multiplexer's initiator setting.    */

      ssn_ptr->mcc.direction = FALSE;

      ssn_ptr->mcc.bt_app_id = rc_conn_ptr->cmd_hdr.bt_app_id;
      ssn_ptr->mcc.tx_wm_ptr = rc_conn_ptr->cmd_msg.cmd_rc_conn.tx_wm_ptr;
      ssn_ptr->mcc.rx_wm_ptr = rc_conn_ptr->cmd_msg.cmd_rc_conn.rx_wm_ptr;

      ssn_ptr->mcc.state     = BT_RCDS_CONNECTING_L2CAP;

      BT_MSG_HIGH( "BT RC: New SSN AID %x", ssn_ptr->mcc.bt_app_id, 0, 0 );
      BT_BDA( MSG_HIGH, "BT RC: New SSN", &ssn_ptr->bd_addr );

      BT_MSG_API( "BT RC CMD TX: L2 StPrm PSM %x AID %x",
                  BT_L2_PSM_RFCOMM, bt_rc_app_id, 0 );
      cmd_status = bt_cmd_l2_set_params( bt_rc_app_id,
                                         BT_L2_NULL_CID,
                                         BT_L2_PSM_RFCOMM,
                                         BT_L2_DEFAULT_FLUSH_TIMEOUT,
                                         RC_MAX_MTU,
                                         BT_QOS_BEST_EFFORT,
                                         BT_L2_DEFAULT_TOKEN_RATE,
                                         BT_L2_DEFAULT_TOKEN_BUCKET_SIZE,
                                         BT_L2_DEFAULT_PEAK_BANDWIDTH,
                                         BT_L2_DEFAULT_LATENCY,
                                         BT_L2_DEFAULT_DELAY_VARIATION );
      ASSERT_DEBUG( cmd_status == BT_CS_GN_PENDING );

      BT_MSG_API( "BT RC CMD TX: L2 Conn AID %x PSM %x",
                  bt_rc_app_id, BT_L2_PSM_RFCOMM, 0 );
      BT_BDA( MSG_API, "BT RC CMD TX: L2 Conn", &ssn_ptr->bd_addr );
      cmd_status = bt_cmd_l2_connect_immediate_cid( bt_rc_app_id,
                                                    BT_L2_PSM_RFCOMM,
                                                    &ssn_ptr->bd_addr,
                                                    &ssn_ptr->cid );
      ASSERT_DEBUG( cmd_status == BT_CS_GN_PENDING );
    }
    else
    {
      rc_conn_ptr->cmd_hdr.cmd_status = BT_CS_RC_CONN_MAX_SESSIONS;
    }
  }
  else
  {
    if ( ssn_ptr->mcc.state == BT_RCDS_CONNECTED )
    {
      /*  Are we already connected to this remote server?  */

      if ( (dlc_ptr = bt_rc_find_remote_server_dlc_entry( ssn_ptr,
             rc_conn_ptr->cmd_msg.cmd_rc_conn.server_channel )) == NULL )
      {
        /*  No... open new port on active RFCOMM session.  */

        if ( (dlc_ptr = bt_rc_find_free_dlc_entry( ssn_ptr )) != NULL )
        {
          /*  For local clients, the direction of the DLC is the  */
          /*  opposite of the multiplexer's initiator setting.    */

          if ( ssn_ptr->initiator == FALSE )
          {
            dlc_ptr->direction = TRUE;
          }
          else
          {
            dlc_ptr->direction = FALSE;
          }

          dlc_ptr->server_channel = rc_conn_ptr->cmd_msg.cmd_rc_conn.
                                      server_channel;
          dlc_ptr->dlci = BT_DLC_SC_2_DLCI( dlc_ptr->server_channel );
          if ( dlc_ptr->direction != FALSE )
          {
            dlc_ptr->dlci |= BT_DLC_DLCI_DIR_B;
          }
          dlc_ptr->bt_app_id = rc_conn_ptr->cmd_hdr.bt_app_id;
          dlc_ptr->tx_wm_ptr = rc_conn_ptr->cmd_msg.cmd_rc_conn.tx_wm_ptr;
          dlc_ptr->rx_wm_ptr = rc_conn_ptr->cmd_msg.cmd_rc_conn.rx_wm_ptr;
          bt_rc_link_in_rx_gone_empty_fptr( dlc_ptr );

          bt_rc_set_local_client_dlc_config( ssn_ptr, dlc_ptr );

          BT_MSG_HIGH( "BT RC: New DLCI %x SC %x AID %x",
                       dlc_ptr->dlci, dlc_ptr->server_channel,
                       dlc_ptr->bt_app_id );

          dlc_ptr->state = BT_RCDS_CONNECTING_DLC;

          ssn_ptr->dlc_count++;

          bt_rc_bring_up_local_client_dlc( ssn_ptr, dlc_ptr );
        }
        else
        {
          /*  No room for new DLC.  */
          rc_conn_ptr->cmd_hdr.cmd_status = BT_CS_RC_CONN_MAX_DLCS;
        }
      }
      else
      {
        if ( dlc_ptr->state != BT_RCDS_CONNECTED )
        {
          /* This is a higher layer issue. A connect command was issued */
          /* for this SCN/BDA while it was already in the process of    */
          /* disconnecting or connecting. It will be treated the same   */
          /* as receiving a connect command while already connected.    */
          BT_ERR("BT RC: RX connect command in bad state. SC %x, ST %x, AID %x",
                 rc_conn_ptr->cmd_msg.cmd_rc_conn.server_channel,
                 dlc_ptr->state, rc_conn_ptr->cmd_hdr.bt_app_id );
          BT_BDA( ERR, "BT RC: RX connect",
                  &rc_conn_ptr->cmd_msg.cmd_rc_conn.bd_addr );
        }
        rc_conn_ptr->cmd_hdr.cmd_status = BT_CS_RC_CONN_ACTIVE;
      }
    }
    else if ( (ssn_ptr->mcc.state == BT_RCDS_DISCONNECTING_DLC) ||
              (ssn_ptr->mcc.state == BT_RCDS_DISCONNECTING_L2CAP) )
    {
      /* This link is in the process of disconnecting. We should 
       * speed up that process by shortening the P2P timeout */
      if( (ssn_ptr->mcc.peer_to_peer_timer_ms_left == 0 ) ||
          (ssn_ptr->mcc.peer_to_peer_timer_ms_left > 
           RC_QUICK_TIMEOUT_MS) )
      {
        ssn_ptr->mcc.peer_to_peer_timer_ms_left = RC_QUICK_TIMEOUT_MS;
      }
      
      rc_conn_ptr->cmd_hdr.cmd_status = BT_CS_GN_RETRY_CMD_LATER;
    }
    else
    {
      rc_conn_ptr->cmd_hdr.cmd_status = BT_CS_GN_RETRY_CMD_LATER;
    }
  }

}


/*===========================================================================

FUNCTION
  bt_rc_cmd_connect_custom_cfg

DESCRIPTION
  Connect a resident client application to a remote server using a
  specified RFCOMM link configuration.

===========================================================================*/
LOCAL void bt_rc_cmd_connect_custom_cfg
(
  bt_cmd_msg_type*  rc_ccc_ptr
)
{

  bt_rc_negot_table_type*  nte_ptr;
  bt_cmd_status_type       cmd_status;

  rc_ccc_ptr->cmd_hdr.cmd_status = BT_CS_GN_SUCCESS;
  
  if ( bt_rc_configuration_is_valid(
         rc_ccc_ptr->cmd_msg.cmd_rc_conn_cfg.max_frame_size,
         rc_ccc_ptr->cmd_msg.cmd_rc_conn_cfg.priority,
         rc_ccc_ptr->cmd_msg.cmd_rc_conn_cfg.baudrate,
         rc_ccc_ptr->cmd_msg.cmd_rc_conn_cfg.format,
         rc_ccc_ptr->cmd_msg.cmd_rc_conn_cfg.flow_ctrl ) != FALSE )
  {
    nte_ptr = bt_rc_find_free_negot_table_entry();

    nte_ptr->bt_app_id =
      rc_ccc_ptr->cmd_hdr.bt_app_id;
    nte_ptr->bd_addr =
      rc_ccc_ptr->cmd_msg.cmd_rc_conn_cfg.bd_addr;
    nte_ptr->server_channel =
      rc_ccc_ptr->cmd_msg.cmd_rc_conn_cfg.server_channel;
    nte_ptr->config.max_frame_size =
      rc_ccc_ptr->cmd_msg.cmd_rc_conn_cfg.max_frame_size;
    nte_ptr->config.priority =
      rc_ccc_ptr->cmd_msg.cmd_rc_conn_cfg.priority;
    nte_ptr->config.cbfc_enabled =
      rc_ccc_ptr->cmd_msg.cmd_rc_conn_cfg.cb_flow_ctrl;
    if ( nte_ptr->config.cbfc_enabled == FALSE )
    {
      nte_ptr->config.cbfc_far_credits = 0;
    }
    nte_ptr->config.baudrate =
      rc_ccc_ptr->cmd_msg.cmd_rc_conn_cfg.baudrate;
    nte_ptr->config.format =
      rc_ccc_ptr->cmd_msg.cmd_rc_conn_cfg.format;
    nte_ptr->config.flow_ctrl =
      rc_ccc_ptr->cmd_msg.cmd_rc_conn_cfg.flow_ctrl;
    nte_ptr->config.xon_char =
      rc_ccc_ptr->cmd_msg.cmd_rc_conn_cfg.xon_char;
    nte_ptr->config.xoff_char =
      rc_ccc_ptr->cmd_msg.cmd_rc_conn_cfg.xoff_char;

    if ( (cmd_status = bt_cmd_rc_connect(
            rc_ccc_ptr->cmd_hdr.bt_app_id,
            rc_ccc_ptr->cmd_msg.cmd_rc_conn_cfg.server_channel,
            &rc_ccc_ptr->cmd_msg.cmd_rc_conn_cfg.bd_addr,
            rc_ccc_ptr->cmd_msg.cmd_rc_conn_cfg.tx_wm_ptr,
            rc_ccc_ptr->cmd_msg.cmd_rc_conn_cfg.rx_wm_ptr )) !=
          BT_CS_GN_PENDING )
    {
      BT_ERR( "BT RC: Conn Cust Cfg bad st %x",
              cmd_status, 0, 0 );
      rc_ccc_ptr->cmd_hdr.cmd_status = BT_CS_GN_CMD_Q_FULL;
    }
  }
  else
  {
    rc_ccc_ptr->cmd_hdr.cmd_status = BT_CS_RC_BAD_CONFIG;
  }

}


/*===========================================================================

FUNCTION
  bt_rc_split_packet
  
DESCRIPTION
  Splits a specified packet into two as specified using DSM duplication.
  Orignal packet is freed iff split is successful.
  
RETURN VALUES
  if split is successful then
    TRUE is returned and
    *first_pkt_ptr_ptr points to the first packet and
    *last_pkt_ptr_ptr points to the last packet
    *orig_pkt_ptr_ptr is NULL (and associated packet was freed)
  otherwise
    FALSE is returned and
    *first_pkt_ptr_ptr is NULL
    *last_pkt_ptr_ptr is NULL
    *orig_pkt_ptr_ptr is not changed (and associated packet was NOT freed)

===========================================================================*/
LOCAL boolean bt_rc_split_packet
(
  dsm_item_type**  orig_pkt_ptr_ptr,
  uint16           orig_pkt_len,
  uint16           split_offset,
  dsm_item_type**  first_pkt_ptr_ptr,
  dsm_item_type**  last_pkt_ptr_ptr
)
{
  
  boolean  success = FALSE;
  uint16   first_pkt_len;
  uint16   last_pkt_len;
  uint16   duped_len;

  first_pkt_len = split_offset;
  last_pkt_len = orig_pkt_len - split_offset;

  *first_pkt_ptr_ptr = NULL;
  *last_pkt_ptr_ptr = NULL;

  duped_len = dsm_dup_packet( first_pkt_ptr_ptr,
                              *orig_pkt_ptr_ptr,
                              0, first_pkt_len );

  if ( (*first_pkt_ptr_ptr != NULL) &&
       (duped_len == first_pkt_len) &&
       (dsm_length_packet( *first_pkt_ptr_ptr ) == first_pkt_len) )
  {
    duped_len = dsm_dup_packet( last_pkt_ptr_ptr,
                                *orig_pkt_ptr_ptr,
                                split_offset,
                                last_pkt_len );

    if ( (*last_pkt_ptr_ptr != NULL) &&
         (duped_len == last_pkt_len) &&
         (dsm_length_packet( *last_pkt_ptr_ptr ) == last_pkt_len) )
    {
      success = TRUE;
    }
    else
    {
      BT_ERR( "BT RC: TX Bad LSplit %x %x %x",
              last_pkt_len, duped_len, split_offset );
      dsm_free_packet( last_pkt_ptr_ptr );
      dsm_free_packet( first_pkt_ptr_ptr );
    }
  }
  else
  {
    BT_ERR( "BT RC: TX Bad FSplit %x %x %x",
            first_pkt_len, duped_len, split_offset );
    dsm_free_packet( first_pkt_ptr_ptr );
  }
  
  if ( success != FALSE )
  {
    dsm_free_packet( orig_pkt_ptr_ptr );
  }

  return ( success );

}


/*===========================================================================

FUNCTION
  bt_rc_add_to_pending_tx
  
DESCRIPTION
  Adds a specified packet to the pending transmit packet of a
  specified DLC.
  
RETURN VALUES
  Length of resulting pending transmit packet
  *pkt_ptr_ptr becomes NULL
  
===========================================================================*/
LOCAL uint16 bt_rc_add_to_pending_tx
(
  bt_rc_dlc_type*  dlc_ptr,
  dsm_item_type**  pkt_ptr_ptr,
  uint16           pkt_tracer
)
{

  if ( dlc_ptr->tx_pkt_ptr == NULL )
  {
    dlc_ptr->tx_pkt_ptr = *pkt_ptr_ptr;
  }
  else
  {
    dsm_append( &dlc_ptr->tx_pkt_ptr, pkt_ptr_ptr );
    BT_SET_PKT_TRACER( dlc_ptr->tx_pkt_ptr, pkt_tracer );
  }
  *pkt_ptr_ptr = NULL;
  
  return ( dsm_length_packet( dlc_ptr->tx_pkt_ptr ) );

}


/*===========================================================================

FUNCTION
  bt_rc_tx_one_data_frame
  
DESCRIPTION
  Builds and sends one RFCOMM UIH data frame on a specified DLC.

RETURN VALUE
  Count of total data bytes transmitted.
  
===========================================================================*/
LOCAL uint16 bt_rc_tx_one_data_frame
(
  bt_rc_session_type*  ssn_ptr,
  bt_rc_dlc_type*      dlc_ptr,
  uint16               best_info_len
)
{

  boolean         done = FALSE;
  uint16          tx_pkt_len;
  dsm_item_type*  next_pkt_ptr = NULL;
  uint16          next_pkt_len;
  dsm_item_type*  first_pkt_ptr;
  dsm_item_type*  last_pkt_ptr;
  
  /*  The pending transmit packet is at dlc_ptr->tx_pkt_ptr.  */
  /*  There may also be more pending transmit data on the     */
  /*  watermark at dlc_ptr->tx_wm_ptr.  Our goal is to end    */
  /*  up with the best RFCOMM transmit frame possible at      */
  /*  dlc_ptr->tx_pkt_ptr and any other pending transmit      */
  /*  data left over (either from what was already pending    */
  /*  or taken off the watermark) at next_pkt_ptr.   We will  */
  /*  then transmit a UIH frame with dlc_ptr->tx_pkt_ptr      */
  /*  and put next_pkt_ptr into dlc_ptr->tx_pkt_ptr for the   */
  /*  next iteration.                                         */

  /*  There are three cases to handle:                        */
  /*    1)  The pending transmit packet is larger than an     */
  /*        ideal frame -->  split it to form an ideal frame  */
  /*        and make what is left the next_pkt_ptr            */
  /*    2)  The pending transmit packet is equal to an ideal  */
  /*        frame -->  take the next data off the WM make it  */
  /*        the next_pkt_ptr                                  */
  /*    3)  The pending transmit packet is smaller than an    */
  /*        ideal frame --> take more data off the WM until   */
  /*        we have more than an ideal frame                  */

  tx_pkt_len = dsm_length_packet( dlc_ptr->tx_pkt_ptr );

  if ( tx_pkt_len > best_info_len )         /*  case 1  */
  {
    if ( bt_rc_split_packet( &dlc_ptr->tx_pkt_ptr, tx_pkt_len,
                             best_info_len, &first_pkt_ptr,
                             &next_pkt_ptr ) != FALSE )
    {
      dlc_ptr->tx_pkt_ptr = first_pkt_ptr;
    }
    tx_pkt_len = dsm_length_packet( dlc_ptr->tx_pkt_ptr );
  }
  else if ( tx_pkt_len == best_info_len )   /*  case 2  */
  {
    next_pkt_ptr = dsm_dequeue( dlc_ptr->tx_wm_ptr );
    BT_SET_PKT_TRACER( next_pkt_ptr, DSM_BT_TX_RC_DEQUEUED );
  }
  else                                            /*  case 3  */
  {
    while ( done == FALSE )
    {
      if ( (next_pkt_ptr = dsm_dequeue( dlc_ptr->tx_wm_ptr )) != NULL )
      {
        next_pkt_len = dsm_length_packet( next_pkt_ptr );
        BT_SET_PKT_TRACER( next_pkt_ptr, DSM_BT_TX_RC_DEQUEUED );

        if ( next_pkt_len > 0 )
        {
          if ( (tx_pkt_len + next_pkt_len) <= best_info_len )
          {
            tx_pkt_len = bt_rc_add_to_pending_tx(
                           dlc_ptr, &next_pkt_ptr,
                           DSM_BT_TX_RC_APPEND );

            if ( tx_pkt_len == best_info_len )
            {
              next_pkt_ptr = dsm_dequeue( dlc_ptr->tx_wm_ptr );
              BT_SET_PKT_TRACER( next_pkt_ptr, DSM_BT_TX_RC_DEQUEUED );
              done = TRUE;
            }
          }
          else
          {
            if ( bt_rc_split_packet(
                   &next_pkt_ptr, next_pkt_len,
                   (best_info_len - tx_pkt_len),
                   &first_pkt_ptr, &last_pkt_ptr ) != FALSE )
            {
              tx_pkt_len = bt_rc_add_to_pending_tx(
                             dlc_ptr, &first_pkt_ptr,
                             DSM_BT_TX_RC_DUP );
              
              next_pkt_ptr = last_pkt_ptr;
              done = TRUE;
            }
          }
        }
        else
        {
          BT_ERR( "BT RC: TX Bad Pkt Len %x PID %x",
                  next_pkt_len, dlc_ptr->pid, 0 );
          dsm_free_packet( &next_pkt_ptr );
        }
      }
      else
      {
        /*  No more transmit data pending.  */
        done = TRUE;
      }
    }
  }
  
  if ( dlc_ptr->tx_pkt_ptr != NULL )
  {
    if ( tx_pkt_len == best_info_len )
    {
      BT_SET_PKT_TRACER( dlc_ptr->tx_pkt_ptr,
                         DSM_BT_TX_RC_FULL );
    }
    else
    {
      BT_SET_PKT_TRACER( dlc_ptr->tx_pkt_ptr,
                         DSM_BT_TX_RC_LAST );
    }
    BT_MSG_API_PYLD( "BT RC CMD RX: TXD Len %x ILen %x PID %x",
                     tx_pkt_len, best_info_len, dlc_ptr->pid );

    bt_rc_tx_uih_data_frame( ssn_ptr, dlc_ptr,
                             dlc_ptr->tx_pkt_ptr );
  }
  dlc_ptr->tx_pkt_ptr = next_pkt_ptr;

  return( tx_pkt_len );

}


/*===========================================================================

FUNCTION
  bt_rc_tx_data_with_cbfc

DESCRIPTION
  Sends pending RFCOMM transmit data if present on a specified DLC
  that uses Credit Based Flow Control.  If no transmit data is
  pending this routine may send a UIH frame with no data to send
  transmit credits to the far side.

===========================================================================*/
LOCAL void bt_rc_tx_data_with_cbfc
(
  bt_rc_session_type*  ssn_ptr,
  bt_rc_dlc_type*      dlc_ptr
)
{

  uint16          tx_bytes = 1;
  uint16          total_tx_bytes = 0;
  dsm_item_type*  dsm_ptr;
  uint16          best_info_len =
                    MIN( bt_rc_ideal_info_len,
                         dlc_ptr->config.max_frame_size );

  if ( (best_info_len == dlc_ptr->config.max_frame_size) &&
       (bt_rc_far_tx_credits_needed( ssn_ptr, dlc_ptr ) != FALSE) )
  {
    best_info_len--;
  }

  while ( (ssn_ptr->acl_tx_flow_on != FALSE) && (tx_bytes > 0) &&
          (dlc_ptr->config.cbfc_local_credits > 0 ) &&
          (total_tx_bytes < BT_RC_MAX_TX_BYTES_PER_TICK) )
  {
    tx_bytes = bt_rc_tx_one_data_frame( ssn_ptr, dlc_ptr, best_info_len );
    total_tx_bytes += tx_bytes;
  }

#ifdef BT_FLOW_DEBUG
  if ( dlc_ptr->config.cbfc_local_credits == 0 )
  {
    BT_MSG_HIGH( "BT RC: CBFC Local TX OFF PID %x",
                 dlc_ptr->pid, 0, 0 );
  }
#endif

  /*  Send transmit credits to far side if   */
  /*  needed via a 0 length UIH data frame.  */
  if ( (total_tx_bytes == 0) &&
       (bt_rc_far_tx_credits_needed( ssn_ptr, dlc_ptr ) != FALSE) )
  {
    dsm_ptr = bt_get_free_dsm_ptr( BT_TL_RFCOMM, 1 );
    bt_rc_tx_uih_data_frame( ssn_ptr, dlc_ptr, dsm_ptr );
  }

}


/*===========================================================================

FUNCTION
  bt_rc_tx_data_without_cbfc

DESCRIPTION
  Sends pending RFCOMM transmit data if present on a specified DLC
  that doesn not use Credit Based Flow Control.
  
===========================================================================*/
LOCAL void bt_rc_tx_data_without_cbfc
(
  bt_rc_session_type*  ssn_ptr,
  bt_rc_dlc_type*      dlc_ptr
)
{

  uint16  tx_bytes = 1;
  uint16  total_tx_bytes = 0;
  uint16  best_info_len =
            MIN( bt_rc_ideal_info_len,
                 dlc_ptr->config.max_frame_size );

  if ( ((ssn_ptr->mcc.far_v24_signals | dlc_ptr->far_v24_signals) &
           RCMCCM_MSC_V24_FC_FLOW_OFF_B) == 0 )
  {
    while ( (ssn_ptr->acl_tx_flow_on != FALSE) && (tx_bytes > 0) &&
            (total_tx_bytes < BT_RC_MAX_TX_BYTES_PER_TICK) )
    {
      tx_bytes = bt_rc_tx_one_data_frame( ssn_ptr, dlc_ptr, best_info_len );
      total_tx_bytes += tx_bytes;
    }
  }

}


/*===========================================================================

FUNCTION
  bt_rc_start_tx_timer

DESCRIPTION
  Starts the TX timer if it is not already running.

===========================================================================*/
LOCAL void bt_rc_start_tx_timer
(
  void
)
{
  
  if ( bt_rc_tx_timer_running == FALSE )
  {
    rex_set_timer( &bt_rc_tx_timer, BT_RC_TX_TIMER_MS );
    bt_rc_tx_timer_running = TRUE;
  }

}

  
/*===========================================================================

FUNCTION
  bt_rc_tx_timer_expired

DESCRIPTION
  Sends pending RFCOMM transmit data if present.

===========================================================================*/
void bt_rc_tx_timer_expired
(
  void
)
{

  uint8  i, j;

  for ( i = 0; i < BT_RC_MAX_SESSIONS; i++ )
  {
    if ( SSN( i ).mcc.state != BT_RCDS_DISCONNECTED )
    {
      for ( j = 0; j < BT_RC_MAX_PORTS_PER_SESSION; j++ )
      {
        if ( (DLC( i, j ).state == BT_RCDS_CONNECTED) &&
             (DLC( i, j ).tx_wm_ptr != NULL) )
        {
          if ( DLC( i, j ).config.cbfc_enabled != FALSE )
          {
            bt_rc_tx_data_with_cbfc( &SSN( i ), &DLC( i, j ) );
          }
          else
          {
            bt_rc_tx_data_without_cbfc( &SSN( i ), &DLC( i, j ) );
          }
          if ( DLC( i, j ).tx_pkt_ptr != NULL &&
               ( DLC( i, j ).config.cbfc_enabled == FALSE ||
                 DLC( i, j ).config.cbfc_local_credits != 0 ) )
          {
            bt_rc_start_tx_timer();
          }
        }
        else if ( (DLC( i, j ).state != BT_RCDS_DISCONNECTED) &&
                  (DLC( i, j ).tx_wm_ptr == NULL) )
        {
          BT_ERR("BT RC: Tx tmr but wm ptr NULL",0,0,0);
        }
      }
    }
  }

}


/*===========================================================================

FUNCTION
  bt_rc_cmd_tx_data

DESCRIPTION
  Send data over an existing RFCOMM link via the watermark.

===========================================================================*/
LOCAL void bt_rc_cmd_tx_data
(
  bt_cmd_msg_type*  rc_txd_ptr
)
{

  rc_txd_ptr->cmd_hdr.cmd_status = BT_CS_GN_SUCCESS;

  BT_MSG_API_PYLD( "BT RC CMD RX: TXD AID %x PID %x",
                   rc_txd_ptr->cmd_hdr.bt_app_id,
                   rc_txd_ptr->cmd_msg.cmd_rc_txd.pid, 0 );

  bt_rc_start_tx_timer();

}


/*===========================================================================

FUNCTION
  bt_rc_cmd_set_port_rx_fc

DESCRIPTION
  Set the receive flow control on a specified port as indicated.

===========================================================================*/
LOCAL void bt_rc_cmd_set_port_rx_fc
(
  bt_cmd_msg_type*  rc_sprxfc_ptr
)
{

  bt_rc_dlc_type*      dlc_ptr;
  bt_rc_session_type*  ssn_ptr;

  rc_sprxfc_ptr->cmd_hdr.cmd_status = BT_CS_GN_SUCCESS;

  BT_MSG_API( "BT RC CMD RX: Port FC %x PID %x AID %x",
              rc_sprxfc_ptr->cmd_msg.cmd_rc_sprxfc.enable_flow,
              rc_sprxfc_ptr->cmd_msg.cmd_rc_sprxfc.pid,
              rc_sprxfc_ptr->cmd_hdr.bt_app_id );
  
  if ( bt_rc_pid_to_ssn_dlc_ptrs(
         rc_sprxfc_ptr->cmd_msg.cmd_rc_sprxfc.pid,
         &ssn_ptr, &dlc_ptr ) != FALSE )
  {
    if ( (rc_sprxfc_ptr->cmd_msg.cmd_rc_sprxfc.enable_flow == FALSE) &&
          (!(dlc_ptr->local_v24_signals &
               RCMCCM_MSC_V24_FC_FLOW_OFF_B)) )
    {
      dlc_ptr->local_v24_signals |= RCMCCM_MSC_V24_FC_FLOW_OFF_B;
#ifdef BT_FLOW_DEBUG
      BT_MSG_HIGH( "BT FLOW: RC DLC RX flow OFF PID %x",
                   rc_sprxfc_ptr->cmd_msg.cmd_rc_sprxfc.pid, 0, 0 );
#endif
      if ( ssn_ptr->mcc.config.cbfc_enabled == FALSE )
      {
        bt_rc_tx_msc_command( ssn_ptr, dlc_ptr );
      }
    }
    else if ( (rc_sprxfc_ptr->cmd_msg.cmd_rc_sprxfc.enable_flow !=
                 FALSE) &&
              (dlc_ptr->local_v24_signals &
                 RCMCCM_MSC_V24_FC_FLOW_OFF_B) )
    {
      dlc_ptr->local_v24_signals &= ~RCMCCM_MSC_V24_FC_FLOW_OFF_B;
#ifdef BT_FLOW_DEBUG
      BT_MSG_HIGH( "BT FLOW: RC DLC RX flow ON PID %x",
                   rc_sprxfc_ptr->cmd_msg.cmd_rc_sprxfc.pid, 0, 0 );
#endif
      if ( ssn_ptr->mcc.config.cbfc_enabled == FALSE )
      {
        bt_rc_tx_msc_command( ssn_ptr, dlc_ptr );
      }
      else
      {
        bt_rc_restart_tx_flow();
      }
    }
  }
  else
  {
    rc_sprxfc_ptr->cmd_hdr.cmd_status = BT_CS_RC_BAD_PID;
    BT_ERR( "BT RC: PFC cmd Bad PID %x AID %x FC %x",
            rc_sprxfc_ptr->cmd_msg.cmd_rc_sprxfc.pid,
            rc_sprxfc_ptr->cmd_hdr.bt_app_id,
            rc_sprxfc_ptr->cmd_msg.cmd_rc_sprxfc.enable_flow );
  }

}


/*===========================================================================

FUNCTION
  bt_rc_cmd_set_session_rx_fc

DESCRIPTION
  Set the receive flow control on a specified session as indicated.

===========================================================================*/
LOCAL void bt_rc_cmd_set_session_rx_fc
(
  bt_cmd_msg_type*  rc_ssrxfc_ptr
)
{

  bt_rc_session_type*  ssn_ptr;

  rc_ssrxfc_ptr->cmd_hdr.cmd_status = BT_CS_GN_SUCCESS;

  if ( bt_rc_pid_to_ssn_ptr(
         rc_ssrxfc_ptr->cmd_msg.cmd_rc_ssrxfc.pid,
         &ssn_ptr ) != FALSE )
  {
    BT_MSG_API( "BT RC CMD RX: SSN FC %x AID %x Sidx %x",
              rc_ssrxfc_ptr->cmd_msg.cmd_rc_ssrxfc.enable_flow,
              rc_ssrxfc_ptr->cmd_hdr.bt_app_id,
              ssn_ptr->ssn_idx );

    if ( (rc_ssrxfc_ptr->cmd_msg.cmd_rc_ssrxfc.enable_flow == FALSE) &&
         (!(ssn_ptr->mcc.local_v24_signals &
              RCMCCM_MSC_V24_FC_FLOW_OFF_B)) )
    {
      ssn_ptr->mcc.local_v24_signals |= RCMCCM_MSC_V24_FC_FLOW_OFF_B;
#ifdef BT_FLOW_DEBUG
      BT_MSG_HIGH( "BT FLOW: RC SSN RX flow OFF AID %x Sidx %x",
                   rc_ssrxfc_ptr->cmd_hdr.bt_app_id,
                   ssn_ptr->ssn_idx, 0 );
#endif
      if ( ssn_ptr->mcc.config.cbfc_enabled == FALSE )
      {
        bt_rc_tx_fc_command( ssn_ptr );
      }
    }
    else if ( (rc_ssrxfc_ptr->cmd_msg.cmd_rc_ssrxfc.enable_flow !=
                FALSE) &&
              (ssn_ptr->mcc.local_v24_signals &
                 RCMCCM_MSC_V24_FC_FLOW_OFF_B) )
    {
      ssn_ptr->mcc.local_v24_signals &= ~RCMCCM_MSC_V24_FC_FLOW_OFF_B;
#ifdef BT_FLOW_DEBUG
      BT_MSG_HIGH( "BT FLOW: SSN RX flow ON AID %x Sidx %x",
                   rc_ssrxfc_ptr->cmd_hdr.bt_app_id,
                   ssn_ptr->ssn_idx, 0 );
#endif
      if ( ssn_ptr->mcc.config.cbfc_enabled == FALSE )
      {
        bt_rc_tx_fc_command( ssn_ptr );
      }
      else
      {
        bt_rc_restart_tx_flow();
      }
    }
  }
  else
  {
    rc_ssrxfc_ptr->cmd_hdr.cmd_status = BT_CS_RC_BAD_PID;
    BT_ERR( "BT RC: SSN FC cmd Bad PID %x FC %x AID %x",
            rc_ssrxfc_ptr->cmd_msg.cmd_rc_ssrxfc.pid,
            rc_ssrxfc_ptr->cmd_msg.cmd_rc_ssrxfc.enable_flow,
            rc_ssrxfc_ptr->cmd_hdr.bt_app_id );
  }

}


/*===========================================================================

FUNCTION
  bt_rc_update_disconnected_dlc

DESCRIPTION
  Updates the session table entry and DLC table entry for a specified
  DLC that is now disconnected.

RETURN VALUE
  FALSE if tables are consistent with this disconnection, otherwise TRUE.

===========================================================================*/
boolean bt_rc_update_disconnected_dlc
(
  bt_rc_session_type*   ssn_ptr,
  bt_rc_dlc_type*       dlc_ptr,
  boolean               conn_failed,
  bt_event_reason_type  reason
)
{

  boolean                error = FALSE;
  bt_rc_reg_table_type*  reg_ptr;

  if ( ssn_ptr->dlc_count > 0 )
  {
    ssn_ptr->dlc_count--;
    
    if ( conn_failed == FALSE )
    {
      if ( (dlc_ptr->direction != ssn_ptr->initiator) ||
           ((dlc_ptr->direction == ssn_ptr->initiator) &&
            ((dlc_ptr->state == BT_RCDS_CONNECTED) ||
             (dlc_ptr->state == BT_RCDS_DISCONNECTING_DLC)) ) )
      {
        bt_rc_ev_send_disconnected( dlc_ptr->bt_app_id,
                                    dlc_ptr->pid, reason );
      }
    }
    else
    {
      bt_rc_ev_send_connection_failed( dlc_ptr->bt_app_id,
                                       &(ssn_ptr->bd_addr),
                                       reason );
    }

    /*  Make the registration table entry  */
    /*  for local servers disconnected.    */
  
    /*  For local servers, the direction of the DLC is  */
    /*  equal to the multiplexer's initiator setting.   */
    if ( dlc_ptr->direction == ssn_ptr->initiator )
    {
      if ( (reg_ptr = bt_rc_find_regd_server_channel(
                        dlc_ptr->server_channel )) != NULL )
      {
        reg_ptr->connected = FALSE;
      }
      else
      {
        error = TRUE;
        BT_ERR( "BT RC: Serv NOT regd SC %x Sidx %x",
                dlc_ptr->server_channel, ssn_ptr->ssn_idx, 0 );
      }
    }
    
    if ( error == FALSE )
    {
      if ( dlc_ptr->rx_wm_ptr != NULL )
      {
        INTLOCK();
        if ( dlc_ptr->rx_wm_ptr->gone_empty_func_ptr ==
               dlc_ptr->my_rx_wm_ge_fptr )
        {
          dlc_ptr->rx_wm_ptr->gone_empty_func_ptr =
            dlc_ptr->orig_rx_wm_ge_fptr;
        }
        INTFREE();
        dlc_ptr->orig_rx_wm_ge_fptr = NULL;
      }
      dsm_free_packet( &dlc_ptr->tx_pkt_ptr );
      bt_rc_initialize_dlc_entry( dlc_ptr );
    }
  }
  else
  {
    /*  Bad DLC count in session entry when disconnecting DLC.  */
    error = TRUE;
    BT_ERR( "BT RC: Bad DLC Cnt %x SC %x PID %x",
            ssn_ptr->dlc_count, dlc_ptr->server_channel,
            dlc_ptr->pid );
  }

  return ( error );

}


/*===========================================================================

FUNCTION
  bt_rc_update_disconnected_mcc

DESCRIPTION
  Updates the session table entry for a specified MCC that is now
  disconnected.

RETURN VALUE
  FALSE if tables are consistent with this disconnection, otherwise TRUE.

===========================================================================*/
boolean bt_rc_update_disconnected_mcc
(
  bt_rc_session_type*  ssn_ptr
)
{

  boolean  error = FALSE;

  if ( ssn_ptr->dlc_count == 0 )
  {
    if ( (ssn_ptr->mcc.state == BT_RCDS_CONNECTED) ||
         (ssn_ptr->mcc.state == BT_RCDS_DISCONNECTING_DLC) )
    {
      ssn_ptr->mcc.state = BT_RCDS_DISCONNECTING_L2CAP;
    }
    else
    {
      /*  Bad MCC state for DLC disconnect.  */
      error = TRUE;
      BT_ERR( "BT RC: Bad MCC St %x Sidx %x",
              ssn_ptr->mcc.state, ssn_ptr->ssn_idx, 0 );
    }
  }
  else
  {
    /*  Disconnected MCC with active DLCs.  */
    error = TRUE;
    BT_ERR( "BT RC: Bad MCC DLC Cnt %x St %x Sidx %x",
            ssn_ptr->dlc_count, ssn_ptr->mcc.state,
            ssn_ptr->ssn_idx );
  }

  return ( error );

}


/*===========================================================================

FUNCTION
  bt_rc_bring_down_dlc

DESCRIPTION
  Performs the next action necessary to bring down a specified DLC.

RETURN VALUE
  FALSE if tables are consistent with this disconnection, otherwise TRUE.

===========================================================================*/
boolean bt_rc_bring_down_dlc
(
  bt_rc_session_type*   ssn_ptr,
  bt_rc_dlc_type*       dlc_ptr,
  boolean               conn_failed,
  bt_event_reason_type  reason
)
{

  boolean  error = FALSE;

  if ( dlc_ptr != &ssn_ptr->mcc )
  {
    if ( (error = bt_rc_update_disconnected_dlc(
                    ssn_ptr, dlc_ptr,
                    conn_failed, reason )) == FALSE )
    {
      if ( ssn_ptr->dlc_count == 0 )
      {
        /*  No more DLCs active - disconnect session.  */
        bt_rc_tx_disc_frame( ssn_ptr, &(ssn_ptr->mcc) );

        ssn_ptr->mcc.state = BT_RCDS_DISCONNECTING_DLC;
      }
    }
  }
  else
  {
    if ( (ssn_ptr->mcc.state == BT_RCDS_DISCONNECTING_DLC) &&
         (ssn_ptr->dlc_count == 0) )
    {
      BT_MSG_API( "BT RC CMD TX: L2 Disc AID %x CID %x",
                  bt_rc_app_id, ssn_ptr->cid, 0 );

      bt_cmd_l2_disconnect( bt_rc_app_id, ssn_ptr->cid );
      
      ssn_ptr->mcc.state = BT_RCDS_DISCONNECTING_L2CAP;
    }
    else
    {
      /*  Inconsistent MCC state.  */
      error = TRUE;
      BT_ERR( "BT RC: Bad MCC DLC St %x Cnt %x Sidx %x",
              ssn_ptr->mcc.state, ssn_ptr->dlc_count,
              ssn_ptr->ssn_idx );
    }
  }

  return ( error );

}


/*===========================================================================

FUNCTION
  bt_rc_dlc_peer_to_peer_tick

DESCRIPTION
  Performs Peer to Peer communications monitoring for a specified DLC.

===========================================================================*/
LOCAL void bt_rc_dlc_peer_to_peer_tick
(
  bt_rc_session_type*  ssn_ptr,
  bt_rc_dlc_type*      dlc_ptr
)
{

  boolean  connection_failure = FALSE;

  if ( dlc_ptr->peer_to_peer_timer_ms_left <= BT_BACKGROUND_TIMER_MS )
  {
    /*  Peer to peer monitor has expired.  */
    dlc_ptr->peer_to_peer_timer_ms_left = 0;

    if ( (dlc_ptr->state == BT_RCDS_DISCONNECTED) ||
         (dlc_ptr->state == BT_RCDS_CONNECTED) )
    {
      /*  Peer to peer message exchanges to setup and/or    */
      /*  teardown the RFCOMM link finished up properly...  */
      /*  JUST IN TIME... nothing needs to be done here.    */
      BT_MSG_HIGH( "BT RC: P2P JIT St %x PID %x",
                   dlc_ptr->state, dlc_ptr->pid, 0 );
    }
    else if ( ((dlc_ptr->state == BT_RCDS_CONNECTING_L2CAP) ||
               (dlc_ptr->state == BT_RCDS_DISCONNECTING_L2CAP)) ||
              ((dlc_ptr == &ssn_ptr->mcc) &&
               ((dlc_ptr->state == BT_RCDS_CONNECTING_DLC) ||
                (dlc_ptr->state == BT_RCDS_DISCONNECTING_DLC))) )
    {
      /*  When trying to setup/teardown the MCC either L2CAP did  */
      /*  not respond to RFCOMM's connect/disconnect command or   */
      /*  the peer message exchanges at RFCOMM did not finish up  */
      /*  properly.  Clean up the connection information and      */
      /*  notify the application as appropriate.                  */

      BT_MSG_HIGH( "BT RC: P2P MCC Exp St %x PID %x",
                   dlc_ptr->state, dlc_ptr->pid, 0 );
      
      if ( (dlc_ptr->state == BT_RCDS_CONNECTING_L2CAP) ||
           (dlc_ptr->state == BT_RCDS_CONNECTING_DLC)   ||
           (dlc_ptr->state == BT_RCDS_DISCONNECTING_DLC) )
      {
        /*  If it was connecting, inform application of failure.  */
        if ( dlc_ptr->state != BT_RCDS_DISCONNECTING_DLC )
        {
          bt_rc_ev_send_connection_failed(
            ssn_ptr->mcc.bt_app_id, &(ssn_ptr->bd_addr),
            BT_EVR_RC_REMOTE_SIGNALING_ERROR );
        }

        dlc_ptr->state = BT_RCDS_DISCONNECTING_DLC;

        bt_rc_bring_down_dlc( ssn_ptr, dlc_ptr, connection_failure,
                              BT_EVR_RC_REMOTE_SIGNALING_ERROR );      
      }
      else if ( dlc_ptr->state == BT_RCDS_DISCONNECTING_L2CAP )
      {
        if ( ssn_ptr->l2_disc_wait != FALSE )
        {
           BT_MSG_API( "BT RC CMD TX: L2 Disc AID %x CID %x",
                       bt_rc_app_id, ssn_ptr->cid, 0 );

           ssn_ptr->mcc.peer_to_peer_timer_ms_left = RC_DISC_TIMEOUT_MS;
           ssn_ptr->l2_disc_wait = FALSE;
           bt_cmd_l2_disconnect( bt_rc_app_id, ssn_ptr->cid );
        }
        else
        {
          /* L2 has not responded to the disconnect command in a */
          /* reasonable amount of time.                          */
          BT_ERR( "BT RC: P2P MCC while disc L2.", 0, 0, 0);
          bt_rc_initialize_session_entry( ssn_ptr->ssn_idx );
        }
      }
    }
    else if ( (dlc_ptr->state == BT_RCDS_CONNECTING_DLC) ||
              (dlc_ptr->state == BT_RCDS_DISCONNECTING_DLC) )
    {
      /*  Peer to peer message exchanges at RFCOMM to setup  */
      /*  and/or teardown the RFCOMM DLC did not finish up   */
      /*  properly.  Clean up the connection information     */
      /*  and notify the application as appropriate.         */
      BT_MSG_HIGH( "BT RC: P2P DLC Exp St %x PID %x",
                   dlc_ptr->state, dlc_ptr->pid, 0 );

      if ( dlc_ptr->state == BT_RCDS_CONNECTING_DLC )
      {
        connection_failure = TRUE;
      }

      bt_rc_bring_down_dlc( ssn_ptr, dlc_ptr, connection_failure,
                            BT_EVR_RC_REMOTE_SIGNALING_ERROR );
    }
    else
    {
      BT_ERR( "BT RC: P2P Bad St %x PID %x",
              dlc_ptr->state, dlc_ptr->pid, 0 );
    }
  }
  else
  {
    dlc_ptr->peer_to_peer_timer_ms_left -= BT_BACKGROUND_TIMER_MS;
  }

}


/*===========================================================================

FUNCTION
  bt_rc_monitor_dlc

DESCRIPTION
  Performs background monitoring for RFCOMM.

===========================================================================*/
void bt_rc_monitor_dlc
(
  bt_rc_session_type*  ssn_ptr,
  bt_rc_dlc_type*      dlc_ptr
)
{

  /*  It has already been verified that the     */
  /*  state is NOT disconnected at this point.  */

  if ( dlc_ptr->state != BT_RCDS_CONNECTED )
  {
    if ( dlc_ptr->peer_to_peer_timer_ms_left != 0 )
    {
      /*  Monitor Peer to Peer communications.  */
      bt_rc_dlc_peer_to_peer_tick( ssn_ptr, dlc_ptr );
    }
    else
    {
      /*  Start Peer to Peer communications monitoring.  */
      if ( (dlc_ptr->state == BT_RCDS_DISCONNECTING_DLC) ||
           (dlc_ptr->state == BT_RCDS_DISCONNECTING_L2CAP) )
      {
        dlc_ptr->peer_to_peer_timer_ms_left = RC_DISC_TIMEOUT_MS;
      }
      else // State is Connecting DLC ar L2CAP
      {
        dlc_ptr->peer_to_peer_timer_ms_left = RC_CONN_TIMEOUT_MS;
      }
    }
  }
  else
  {
    /*  Peer to peer message exchanges to setup the  */
    /*  RFCOMM link finished up properly.  No need   */
    /*  to monitor further.                          */

    dlc_ptr->peer_to_peer_timer_ms_left = 0;
  }

}


/*===========================================================================

FUNCTION
  bt_rc_mcc_connected_only_tick

DESCRIPTION
  Makes sure an MCC is not left connected with no DLCs associated
  with it.

===========================================================================*/
LOCAL void bt_rc_mcc_connected_only_tick
(
  bt_rc_session_type*  ssn_ptr
)
{

  if ( ssn_ptr->mcc.peer_to_peer_timer_ms_left != 0 )
  {
    if ( ssn_ptr->mcc.peer_to_peer_timer_ms_left <= BT_BACKGROUND_TIMER_MS )
    {
      /*  MCC monitor has expired.  */
      ssn_ptr->mcc.peer_to_peer_timer_ms_left = 0;

      BT_MSG_HIGH( "BT RC: MCC Conn Only Exp CID %x I %x D %x",
                   ssn_ptr->cid, ssn_ptr->initiator,
                   ssn_ptr->mcc.direction );
      
      /*  Initiate MCC (session) disconnect.  */
      bt_rc_tx_disc_frame( ssn_ptr, &(ssn_ptr->mcc) );

      ssn_ptr->mcc.state = BT_RCDS_DISCONNECTING_DLC;
    }
    else
    {
      ssn_ptr->mcc.peer_to_peer_timer_ms_left -= BT_BACKGROUND_TIMER_MS;
    }
  }
  else
  {
    /*  Start MCC monitoring.  */
    ssn_ptr->mcc.peer_to_peer_timer_ms_left = RC_MCC_ONLY_TIMEOUT_MS;
  }

}


/*===========================================================================

FUNCTION
  bt_rc_background_tick

DESCRIPTION
  Performs background monitoring for RFCOMM.

===========================================================================*/
void bt_rc_background_tick
(
  void
)
{

  uint8    i, j;
  boolean  dlc_active;

  for ( i = 0; i < BT_RC_MAX_SESSIONS; i++ )
  {
    if ( SSN( i ).mcc.state == BT_RCDS_CONNECTED )
    {
      dlc_active = FALSE;

      for ( j = 0; j < BT_RC_MAX_PORTS_PER_SESSION; j++ )
      {
        if ( DLC( i, j ).state != BT_RCDS_DISCONNECTED )
        {
          dlc_active = TRUE;

          bt_rc_monitor_dlc( &SSN( i ), &DLC( i, j ) );
        }
      }

      if ( dlc_active == FALSE )
      {
        /*  MCC is connected but no DLCs are active.  */
        /*  Perhaps remote disconnected last DLC but  */
        /*  did not disconnect MCC.  Make sure it     */
        /*  is disconnected eventually.               */

        bt_rc_mcc_connected_only_tick( &SSN( i ) );
      }
      else
      {
        /*  No need to monitor MCC.  */
        SSN( i ).mcc.peer_to_peer_timer_ms_left = 0;
      }
    }
    else if ( SSN( i ).mcc.state != BT_RCDS_DISCONNECTED )
    {
      bt_rc_monitor_dlc( &SSN( i ), &SSN( i ).mcc );
    }
  }

}


/*===========================================================================

FUNCTION
  bt_rc_cmd_disconnect

DESCRIPTION
  Disconnect from an existing RFCOMM link.

===========================================================================*/
LOCAL void bt_rc_cmd_disconnect
(
  bt_cmd_msg_type*  rc_disc_ptr
)
{

  bt_rc_dlc_type*      dlc_ptr;
  bt_rc_session_type*  ssn_ptr;

  rc_disc_ptr->cmd_hdr.cmd_status = BT_CS_GN_SUCCESS;

  BT_MSG_API( "BT RC CMD RX: Disc PID %x AID %x",
              rc_disc_ptr->cmd_msg.cmd_rc_disc.pid,
              rc_disc_ptr->cmd_hdr.bt_app_id, 0 );

  if ( bt_rc_pid_to_ssn_dlc_ptrs( rc_disc_ptr->cmd_msg.cmd_rc_disc.pid,
                                  &ssn_ptr, &dlc_ptr ) != FALSE )
  {
    if ( (ssn_ptr->mcc.state == BT_RCDS_CONNECTED) &&
         (dlc_ptr->state == BT_RCDS_CONNECTED) )
    {
      BT_MSG_HIGH( "BT RC: Disc DLC PID %x AID %x",
                   rc_disc_ptr->cmd_msg.cmd_rc_disc.pid,
                   rc_disc_ptr->cmd_hdr.bt_app_id, 0 );

      dlc_ptr->state = BT_RCDS_DISCONNECTING_DLC;
      bt_rc_tx_disc_frame( ssn_ptr, dlc_ptr );
    }
    else
    {
      rc_disc_ptr->cmd_hdr.cmd_status = BT_CS_GN_RETRY_CMD_LATER;
    }
  }
  else
  {
    rc_disc_ptr->cmd_hdr.cmd_status = BT_CS_RC_BAD_PID;
  }

}


/*===========================================================================

FUNCTION
  bt_rc_cmd_unregister

DESCRIPTION
  Unregisters a server from RFCOMM.

===========================================================================*/
LOCAL void bt_rc_cmd_unregister
(
  bt_cmd_msg_type*  rc_unreg_ptr
)
{

  bt_rc_reg_table_type*  reg_ptr;
  bt_cmd_status_type     cmd_status;
  uint16                 connected_server_pid;
  bt_rc_dlc_type*        dlc_ptr;
  bt_rc_session_type*    ssn_ptr;
  

  rc_unreg_ptr->cmd_hdr.cmd_status = BT_CS_GN_SUCCESS;

  BT_MSG_API( "BT RC CMD RX: Unreg SC %x AID %x",
              rc_unreg_ptr->cmd_msg.cmd_rc_unreg.server_channel,
              rc_unreg_ptr->cmd_hdr.bt_app_id, 0 );

  /*  Server must not already be active and/or registered.  */

  if ( (reg_ptr = bt_rc_find_regd_server_channel( rc_unreg_ptr->
                    cmd_msg.cmd_rc_unreg.server_channel )) != NULL )
  {
    if ( reg_ptr->connected == FALSE )
    {
      if ( reg_ptr->bt_app_id == rc_unreg_ptr->cmd_hdr.bt_app_id )
      {
        bt_rc_reg_count--;
        if ( bt_rc_reg_count != 0 )
        {
          *reg_ptr = bt_rc_reg_table[ bt_rc_reg_count ];
        }
        rc_unreg_ptr->cmd_hdr.cmd_status = BT_CS_GN_SUCCESS;

        BT_MSG_HIGH( "BT RC: Unreg SC %x AID %x",
          rc_unreg_ptr->cmd_msg.cmd_rc_unreg.server_channel,
          rc_unreg_ptr->cmd_hdr.bt_app_id, 0 );

        bt_rc_ev_send_unregistered( rc_unreg_ptr->cmd_hdr.bt_app_id,
                                    rc_unreg_ptr->cmd_msg.cmd_rc_unreg.server_channel );

        if ( bt_rc_reg_count == 0 )
        {
          BT_MSG_HIGH( "BT RC: Unregister with L2CAP", 0, 0, 0 );

          BT_MSG_API( "BT RC CMD TX: L2 Unreg PSM %x AID %x",
                      BT_L2_PSM_RFCOMM, bt_rc_app_id, 0 );
          cmd_status = bt_cmd_l2_unregister( bt_rc_app_id,
                                             BT_L2_PSM_RFCOMM );
          ASSERT_DEBUG( cmd_status == BT_CS_GN_PENDING );
        }
      }
      else
      {
        rc_unreg_ptr->cmd_hdr.cmd_status = BT_CS_GN_BAD_APP_ID;
      }
    }
    else
    {
      if ( reg_ptr->unreg_pending == FALSE )
      {
        rc_unreg_ptr->cmd_hdr.cmd_status = BT_CS_RC_CONN_ACTIVE;
        connected_server_pid = 
          bt_rc_find_local_server_pid(rc_unreg_ptr->
                                      cmd_msg.cmd_rc_unreg.server_channel);

        if ( connected_server_pid != BT_RC_NULL_PID )
        {
          if ( bt_rc_pid_to_ssn_dlc_ptrs( connected_server_pid,
                                          &ssn_ptr, &dlc_ptr ) != FALSE )
          {
            BT_MSG_DEBUG( "BT RC: Server Conn Coll SC %x PID %x",
                          rc_unreg_ptr->cmd_msg.cmd_rc_unreg.server_channel,
                          connected_server_pid, 0 );
          
            BT_MSG_DEBUG( "BT RC: Server Conn Coll MCC St %x DLC St %x",
                          ssn_ptr->mcc.state, dlc_ptr->state, 0 );
          
            if ( (ssn_ptr->mcc.state == BT_RCDS_CONNECTED) &&
                 (dlc_ptr->state == BT_RCDS_CONNECTING_DLC) )
            {
              BT_MSG_API( "BT RC CMD TX: RC Disc PID %x AID %x",
                          bt_rc_app_id, connected_server_pid, 0 );
              if( (dlc_ptr->peer_to_peer_timer_ms_left == 0 ) ||
                  (dlc_ptr->peer_to_peer_timer_ms_left > 
                   RC_UNREGISTER_TIMEOUT_MS) )
              {
                /* Set the P2P timer to a short value, insuring this
                 * gets unregistered in a short amount of time. */
                dlc_ptr->peer_to_peer_timer_ms_left = RC_UNREGISTER_TIMEOUT_MS;
              }
              cmd_status = bt_cmd_rc_disconnect( bt_rc_app_id,
                                                 connected_server_pid );
              ASSERT_DEBUG( cmd_status == BT_CS_GN_PENDING );
            
              reg_ptr->unreg_pending = TRUE;

              rc_unreg_ptr->cmd_hdr.cmd_status = BT_CS_GN_RETRY_CMD_LATER;
            }
          }
          else
          {
            BT_ERR( "BT RC: Bad Conn'd Server PID %x SC %x",
                    connected_server_pid,
                    rc_unreg_ptr->cmd_msg.cmd_rc_unreg.server_channel, 0 );
          }
        }
        else
        {
          BT_ERR( "BT RC: Inconsistent Conn'd Server SC %x",
                  rc_unreg_ptr->cmd_msg.cmd_rc_unreg.server_channel, 0, 0 );
        }
      }
      else
      {
        rc_unreg_ptr->cmd_hdr.cmd_status = BT_CS_GN_RETRY_CMD_LATER;
      }
    }
  }
  else
  {
    rc_unreg_ptr->cmd_hdr.cmd_status = BT_CS_RC_BAD_SC;
  }

}


/*===========================================================================

FUNCTION
  bt_rc_cmd_cancel_connect

DESCRIPTION
  Cancels a connect operation, or disconnects if already connected.

===========================================================================*/
LOCAL void bt_rc_cmd_cancel_connect
(
  bt_cmd_msg_type*  rc_cncl_conn_ptr
)
{
  bt_rc_session_type*    ssn_ptr;
  bt_rc_dlc_type*        dlc_ptr;
  bt_bd_addr_type*       bd_addr_ptr;
  uint8                  server_channel;

  bd_addr_ptr    = &rc_cncl_conn_ptr->cmd_msg.cmd_rc_cncl_conn.bd_addr;
  server_channel = rc_cncl_conn_ptr->cmd_msg.cmd_rc_cncl_conn.server_channel;

  BT_MSG_API( "BT RC CMD RX: Cancel Conn AID %x SCN %x",
              rc_cncl_conn_ptr->cmd_hdr.bt_app_id,
              server_channel, 0 );

  BT_BDA( MSG_API, "BT RC CMD RX: Cancel Conn", bd_addr_ptr );

  if ( (ssn_ptr = bt_rc_find_session_bd_addr( bd_addr_ptr )) != NULL )
  {
    if ( (dlc_ptr = bt_rc_find_remote_server_dlc_entry( ssn_ptr, 
                                                        server_channel )) != 
         NULL )
    {
      /* Found the DLC which needs to be disconnected. */
      if ( dlc_ptr->state == BT_RCDS_CONNECTED )
      {
        /* Already connected. Just perform normal disconnect. */
        BT_MSG_API( "BT RC CMD TX: RC Disconnect AID %x PID %x SCN %x",
                    bt_rc_app_id, dlc_ptr->pid, server_channel );
        bt_cmd_rc_disconnect( bt_rc_app_id, dlc_ptr->pid );
        rc_cncl_conn_ptr->cmd_hdr.cmd_status = BT_CS_GN_SUCCESS;
      }
      else if ( dlc_ptr->state == BT_RCDS_CONNECTING_DLC )
      {
        /* Connection in progress. Force a P2P timeout to clean up */
        dlc_ptr->peer_to_peer_timer_ms_left = 0;
        bt_rc_dlc_peer_to_peer_tick( ssn_ptr, dlc_ptr );
        rc_cncl_conn_ptr->cmd_hdr.cmd_status = BT_CS_GN_SUCCESS;
      }
      else
      {
        rc_cncl_conn_ptr->cmd_hdr.cmd_status = BT_CS_GN_BAD_CMD_STATE;
      }
    }
    else
    {
      /* No remote DLC was found. There are a couple reasons for this:
       * 1) MCC is in the CONNECTING_L2CAP state, and L2CAP isn't up yet
       * 2) MCC is in the CONNECTING_DLC state, and no DLCs are up yet
       */
      if( ssn_ptr->mcc.state == BT_RCDS_CONNECTING_L2CAP ||
          ssn_ptr->mcc.state == BT_RCDS_CONNECTING_DLC )
      {
        /* Note: Don't set the MCC state here. This will guarantee RC sends a
         * connect failed event when the L2 disconnect event is received */
        BT_MSG_API( "BT RC CMD TX: L2 Disc AID %x CID %x",
                    bt_rc_app_id, ssn_ptr->cid, 0 );
        bt_cmd_l2_disconnect( bt_rc_app_id, ssn_ptr->cid );
        rc_cncl_conn_ptr->cmd_hdr.cmd_status = BT_CS_GN_SUCCESS;
      }
      else
      {
        BT_ERR( "BT RC: Wrong SCN for cancel connect %x", server_channel, 0, 0 );
        rc_cncl_conn_ptr->cmd_hdr.cmd_status = BT_CS_RC_BAD_SC;
      }
    }
  }
  else
  {
    BT_BDA( ERR, "BT RC: No connection to cancel", bd_addr_ptr );
    rc_cncl_conn_ptr->cmd_hdr.cmd_status = BT_CS_RC_BAD_BD_ADDR;
  }
}

/*===========================================================================

FUNCTION
  bt_rc_cmd_write_modem_status

DESCRIPTION
  Updates modem status for a specified port.

===========================================================================*/
LOCAL void bt_rc_cmd_write_modem_status
(
  bt_cmd_msg_type*  rc_wms_ptr
)
{

  bt_rc_session_type*  ssn_ptr;
  bt_rc_dlc_type*      dlc_ptr;

  rc_wms_ptr->cmd_hdr.cmd_status = BT_CS_GN_SUCCESS;

  BT_MSG_API( "BT RC CMD RX: Wr MS %02x %02x PID %x",
              rc_wms_ptr->cmd_msg.cmd_rc_wms.ms_valid_mask,
              rc_wms_ptr->cmd_msg.cmd_rc_wms.modem_status,
              rc_wms_ptr->cmd_msg.cmd_rc_wms.pid );

  if ( bt_rc_pid_to_ssn_dlc_ptrs( rc_wms_ptr->cmd_msg.cmd_rc_wms.pid,
                                  &ssn_ptr, &dlc_ptr ) != FALSE )
  {
    dlc_ptr->local_v24_signals &=
      ~(rc_wms_ptr->cmd_msg.cmd_rc_wms.ms_valid_mask &
          RCMCCM_MSC_V24_MS_SIGS_M);

    dlc_ptr->local_v24_signals |=
      (rc_wms_ptr->cmd_msg.cmd_rc_wms.ms_valid_mask &
         rc_wms_ptr->cmd_msg.cmd_rc_wms.modem_status &
         RCMCCM_MSC_V24_MS_SIGS_M);

    if ( rc_wms_ptr->cmd_msg.cmd_rc_wms.break_present != FALSE )
    {
      dlc_ptr->local_breaks++;
      dlc_ptr->local_last_break_length =
        rc_wms_ptr->cmd_msg.cmd_rc_wms.break_length;
    }
    bt_rc_tx_msc_command( ssn_ptr, dlc_ptr );
  }
  else
  {
    rc_wms_ptr->cmd_hdr.cmd_status = BT_CS_RC_BAD_PID;
  }

}


/*===========================================================================

FUNCTION
  bt_cmd_rc_read_modem_status

DESCRIPTION
  This function immediately returns the modem status for a specified
  RFCOMM port.  The command is NOT queued to the driver.

===========================================================================*/
bt_cmd_status_type bt_cmd_rc_read_modem_status
(
  bt_app_id_type  bt_app_id,
  uint16          pid,
  uint8*          ms_ptr,      /*  Destination for modem_status value.  */
  uint8*          breaks_ptr,  /*  Destinations for break information.  */
  uint8*          last_break_length_ptr
)
{

  bt_rc_session_type*  ssn_ptr;
  bt_rc_dlc_type*      dlc_ptr;
  bt_cmd_status_type   cmd_status = BT_CS_GN_SUCCESS;
   
  TASKLOCK();
  if ( bt_rc_pid_to_ssn_dlc_ptrs( pid, &ssn_ptr, &dlc_ptr ) != FALSE )
  {
    if ( bt_app_id == dlc_ptr->bt_app_id )
    {
      *ms_ptr = (dlc_ptr->far_v24_signals & RCMCCM_MSC_V24_MS_SIGS_M);
      *breaks_ptr            = dlc_ptr->far_breaks;
      *last_break_length_ptr = dlc_ptr->far_last_break_length;
    }
    else
    {
      cmd_status = BT_CS_GN_BAD_APP_ID;
    }
  }
  else
  {
    cmd_status = BT_CS_RC_BAD_PID;
  }
  TASKFREE();

  if ( cmd_status != BT_CS_GN_SUCCESS )
  {
    *ms_ptr                = 0x00;
    *breaks_ptr            = 0;
    *last_break_length_ptr = 0;
  }

  BT_MSG_API( "BT RC CMD RTX: Rd MS %x PID %x St %x",
              *ms_ptr, pid, cmd_status );

  return ( cmd_status );

}


/*===========================================================================

FUNCTION
  bt_rc_cmd_ping

DESCRIPTION
  Sends a RC Test command on a the specified multiplexer control
  channel.

===========================================================================*/
LOCAL void bt_rc_cmd_ping
(
  bt_cmd_msg_type*  rc_ping_ptr
)
{

  bt_rc_session_type*  ssn_ptr;
  bt_rc_dlc_type*      dlc_ptr;

  rc_ping_ptr->cmd_hdr.cmd_status = BT_CS_GN_SUCCESS;

  if ( bt_rc_pid_to_ssn_dlc_ptrs(
         rc_ping_ptr->cmd_msg.cmd_rc_ping.pid,
         &ssn_ptr, &dlc_ptr ) != FALSE )
  {
    if ( rc_ping_ptr->cmd_hdr.bt_app_id == dlc_ptr->bt_app_id )
    {
      if ( !(ssn_ptr->mcc.expected_rx_mask & BT_ERXM_TEST_RESP_B) )
      {
        if ( rc_ping_ptr->cmd_msg.cmd_rc_ping.byte_count <=
               ssn_ptr->mcc.config.max_frame_size )
        {
          BT_MSG_API( "BT RC CMD RX: Ping Len %x AID %x Sidx %x",
                      rc_ping_ptr->cmd_msg.cmd_rc_ping.byte_count,
                      rc_ping_ptr->cmd_hdr.bt_app_id,
                      ssn_ptr->ssn_idx );

          ssn_ptr->last_test_tx_bt_app_id =
            rc_ping_ptr->cmd_hdr.bt_app_id;
          ssn_ptr->last_test_tx_pid =
            rc_ping_ptr->cmd_msg.cmd_rc_ping.pid;

          bt_rc_tx_mcc_test_command( ssn_ptr,
            rc_ping_ptr->cmd_msg.cmd_rc_ping.byte_count,
            rc_ping_ptr->cmd_msg.cmd_rc_ping.start_value,
            rc_ping_ptr->cmd_msg.cmd_rc_ping.increment );
        }
        else
        {
          rc_ping_ptr->cmd_hdr.cmd_status = BT_CS_RC_BAD_DATA_LEN;
        }
      }
      else
      {
        rc_ping_ptr->cmd_hdr.cmd_status = BT_CS_RC_CONN_ACTIVE;
      }
    }
    else
    {
      rc_ping_ptr->cmd_hdr.cmd_status = BT_CS_GN_BAD_APP_ID;
    }
  }
  else
  {
    rc_ping_ptr->cmd_hdr.cmd_status = BT_CS_RC_BAD_PID;
  }

}


/*===========================================================================

FUNCTION
  bt_rc_cmd_line_error

DESCRIPTION
  Updates line status for a specified port.

===========================================================================*/
LOCAL void bt_rc_cmd_line_error
(
  bt_cmd_msg_type*  rc_le_ptr
)
{

  bt_rc_session_type*  ssn_ptr;
  bt_rc_dlc_type*      dlc_ptr;

  rc_le_ptr->cmd_hdr.cmd_status = BT_CS_GN_SUCCESS;

  BT_MSG_API( "BT RC CMD RX: Line Err %x %x PID %x",
              rc_le_ptr->cmd_msg.cmd_rc_lerr.overrun_error,
              (rc_le_ptr->cmd_msg.cmd_rc_lerr.parity_error |
                 rc_le_ptr->cmd_msg.cmd_rc_lerr.framing_error),
              rc_le_ptr->cmd_msg.cmd_rc_lerr.pid );

  if ( bt_rc_pid_to_ssn_dlc_ptrs( rc_le_ptr->cmd_msg.cmd_rc_lerr.pid,
                                  &ssn_ptr, &dlc_ptr ) != FALSE )
  {
    if ( rc_le_ptr->cmd_hdr.bt_app_id == dlc_ptr->bt_app_id )
    {
      if ( rc_le_ptr->cmd_msg.cmd_rc_lerr.overrun_error != FALSE )
      {
        dlc_ptr->local_overruns++;
      }
      if ( rc_le_ptr->cmd_msg.cmd_rc_lerr.parity_error != FALSE )
      {
        dlc_ptr->local_parity_errors++;
      }
      if ( rc_le_ptr->cmd_msg.cmd_rc_lerr.framing_error != FALSE )
      {
        dlc_ptr->local_framing_errors++;
      }
      bt_rc_tx_rls_command( ssn_ptr, dlc_ptr );
    }
    else
    {
      rc_le_ptr->cmd_hdr.cmd_status = BT_CS_GN_BAD_APP_ID;
    }
  }
  else
  {
    rc_le_ptr->cmd_hdr.cmd_status = BT_CS_RC_BAD_PID;
  }

}


/*===========================================================================

FUNCTION
  bt_cmd_rc_read_line_status

DESCRIPTION
  This function immediately returns the line error counts for a
  specified RFCOMM port.  The command is NOT queued to the driver.

===========================================================================*/
bt_cmd_status_type bt_cmd_rc_read_line_status
(
  bt_app_id_type  bt_app_id,
  uint16          pid,
  uint8*          overruns_ptr,       /*  Destinations for line  */
  uint8*          parity_errors_ptr,  /*    error counts.        */
  uint8*          framing_errors_ptr
)
{

  bt_rc_session_type*  ssn_ptr;
  bt_rc_dlc_type*      dlc_ptr;
  bt_cmd_status_type   cmd_status = BT_CS_GN_SUCCESS;
   
  TASKLOCK();
  if ( bt_rc_pid_to_ssn_dlc_ptrs( pid, &ssn_ptr, &dlc_ptr ) != FALSE )
  {
    if ( bt_app_id == dlc_ptr->bt_app_id )
    {
      *overruns_ptr       = dlc_ptr->far_overruns;
      *parity_errors_ptr  = dlc_ptr->far_parity_errors;
      *framing_errors_ptr = dlc_ptr->far_framing_errors;
    }
    else
    {
      cmd_status = BT_CS_GN_BAD_APP_ID;
    }
  }
  else
  {
    cmd_status = BT_CS_RC_BAD_PID;
  }
  TASKFREE();

  if ( cmd_status != BT_CS_GN_SUCCESS )
  {
    *overruns_ptr       = 0;
    *parity_errors_ptr  = 0;
    *framing_errors_ptr = 0;
  }
  
  BT_MSG_API( "BT RC CMD RTX: Rd LS %x PID %x St %x",
              ((*overruns_ptr) + (*parity_errors_ptr) +
               (*framing_errors_ptr)), pid, cmd_status );

  return ( cmd_status );

}


/*===========================================================================

FUNCTION
  bt_rc_cmd_write_sp_config

DESCRIPTION
  Updates the serial port settings within the configuration of the
  specified port.  The serial port settings corresponding to an MCC RPN
  command.  If this results in an actual change in these settings, a
  Remote Port Negotiations (RPN) command is sent.  If the settings are
  not changed, a RPN request command is sent instead.  Either way, an
  RFCOMM reconfigured event will be sent back to the application upon
  receipt of the corresponding RPN response.

===========================================================================*/
LOCAL void bt_rc_cmd_write_sp_config
(
  bt_cmd_msg_type*  rc_wspc_ptr
)
{

  bt_rc_dlc_type*      dlc_ptr;
  bt_rc_session_type*  ssn_ptr;
  bt_rc_mcc_rpne_type  rpne_msg;

  rc_wspc_ptr->cmd_hdr.cmd_status = BT_CS_RC_BAD_PID;

  BT_MSG_API( "BT RC CMD RX: Write SP Cfg PID %x AID %x",
              rc_wspc_ptr->cmd_msg.cmd_rc_wspc.pid,
              rc_wspc_ptr->cmd_hdr.bt_app_id, 0 );

  if ( bt_rc_pid_to_ssn_dlc_ptrs(
         rc_wspc_ptr->cmd_msg.cmd_rc_wspc.pid,
         &ssn_ptr, &dlc_ptr ) != FALSE )
  {
    if ( dlc_ptr->state == BT_RCDS_CONNECTED )
    {
      rpne_msg.baudrate   = rc_wspc_ptr->cmd_msg.cmd_rc_wspc.baudrate;
      rpne_msg.format     = rc_wspc_ptr->cmd_msg.cmd_rc_wspc.format;
      rpne_msg.flow_ctrl  = rc_wspc_ptr->cmd_msg.cmd_rc_wspc.flow_ctrl;
      rpne_msg.xon_char   = rc_wspc_ptr->cmd_msg.cmd_rc_wspc.xon_char;
      rpne_msg.xoff_char  = rc_wspc_ptr->cmd_msg.cmd_rc_wspc.xoff_char;
      rpne_msg.param_mask = RCMCCM_RPN_PM_DEFAULT_V;

      if ( bt_rc_write_sp_config( dlc_ptr->dlci,
             &dlc_ptr->config, &rpne_msg ) == FALSE )
      {
        /*  Send RPN request.  */
        bt_rc_tx_rpn_command( ssn_ptr, dlc_ptr, TRUE );
      }
      else
      {
        /*  Send full RPN command.  */
        bt_rc_tx_rpn_command( ssn_ptr, dlc_ptr, FALSE );
      }

      rc_wspc_ptr->cmd_hdr.cmd_status = BT_CS_GN_SUCCESS;
    }
  }

}


/*===========================================================================

FUNCTION
  bt_rc_process_command

DESCRIPTION
  Processes a specified RFCOMM command.

===========================================================================*/
void bt_rc_process_command
(
  bt_cmd_msg_type*  rc_cmd_ptr
)
{

  switch ( rc_cmd_ptr->cmd_hdr.cmd_type )
  {
    case BT_CMD_RC_REGISTER:
      {
        bt_rc_cmd_register( rc_cmd_ptr );
        break;
      }

    case BT_CMD_RC_CONNECT:
      {
        bt_rc_cmd_connect( rc_cmd_ptr );
        break;
      }

    case BT_CMD_RC_CONN_CUST_CFG:
      {
        bt_rc_cmd_connect_custom_cfg( rc_cmd_ptr );
        break;
      }

    case BT_CMD_RC_TX_DATA:
      {
        bt_rc_cmd_tx_data( rc_cmd_ptr );
        break;
      }

    case BT_CMD_RC_SET_PORT_RX_FC:
      {
        bt_rc_cmd_set_port_rx_fc( rc_cmd_ptr );
        break;
      }

    case BT_CMD_RC_SET_SESSION_RX_FC:
      {
        bt_rc_cmd_set_session_rx_fc( rc_cmd_ptr );
        break;
      }

    case BT_CMD_RC_WRITE_MS:
      {
        bt_rc_cmd_write_modem_status( rc_cmd_ptr );
        break;
      }

    case BT_CMD_RC_WRITE_SP_CFG:
      {
        bt_rc_cmd_write_sp_config( rc_cmd_ptr );
        break;
      }

    case BT_CMD_RC_LINE_ERROR:
      {
        bt_rc_cmd_line_error( rc_cmd_ptr );
        break;
      }

    case BT_CMD_RC_PING:
      {
        bt_rc_cmd_ping( rc_cmd_ptr );
        break;
      }
  
    case BT_CMD_RC_DISCONNECT:
      {
        bt_rc_cmd_disconnect( rc_cmd_ptr );
        break;
      }
  
    case BT_CMD_RC_UNREGISTER:
      {
        bt_rc_cmd_unregister( rc_cmd_ptr );
        break;
      }

    case BT_CMD_RC_CANCEL_CONNECT:
      {
        bt_rc_cmd_cancel_connect( rc_cmd_ptr );
        break;
      }

    default:
      {
        rc_cmd_ptr->cmd_hdr.cmd_status = BT_CS_RC_UNRECOGNIZED_CMD;
        BT_ERR( "BT RC: Bad RX CMD Type %x AID %x",
                rc_cmd_ptr->cmd_hdr.cmd_type,
                rc_cmd_ptr->cmd_hdr.bt_app_id, 0 );
        break;
      }
  }

}


/*===========================================================================

FUNCTION
  bt_rc_calculate_fcs

DESCRIPTION
  Calculates the Frame Check Sequence (FCS) fields for a specified
  number of bytes.  byte_count must be 2 or 3 into this routine.

RETURN VALUE
  FCS

===========================================================================*/
bt_rc_fcs_type bt_rc_calculate_fcs
(
  uint8  *src_ptr,
  uint8   byte_count
)
{

  uint16          i, j, bit_mask, sumB;
  uint16          poly = 0x01C0;
  bt_rc_fcs_type  sumA;

  sumB = (((uint16)(src_ptr[ 0 ])) << 1);
  for ( i = 1; i < byte_count; i++ )
  {
    bit_mask = 0x01;
    for ( j = 0; j < 8; j++ )
    {
      if ( sumB & 0x0001 )
      {
        sumB ^= poly;
      }
      sumB >>= 1;
      if ( src_ptr[ i ] & bit_mask )
      {
        sumB |= 0x0100;
      }
      bit_mask <<= 1;
    }
  }
  for ( j = 0; j < 9; j++ )
  {
    if ( sumB & 0x0001 )
    {
      sumB ^= poly;
    }
    sumB >>= 1;
  }

  if ( byte_count == BT_RC_FCS_BYTES_UIH )
  {
    sumA = 0xEB;
  }
  else if ( byte_count == BT_RC_FCS_BYTES_NON_UIH )
  {
    sumA = 0xD4;
  }
  else
  {
    /*  Illegal number of bytes over which to calculate FCS.  */
    BT_ERR( "BT RC: Bad FCS Byte Count %x", byte_count, 0, 0 );
    sumA = 0x0;
    sumB = 0xFF;
  }

  return( (bt_rc_fcs_type)(~(sumA ^ sumB)) );

}


/*===========================================================================

FUNCTION
  bt_rc_find_free_session_entry

DESCRIPTION
  Finds a free session table entry.

RETURN VALUE
  Pointer to free session entry, if found; otherwise NULL.

===========================================================================*/
bt_rc_session_type* bt_rc_find_free_session_entry
(
  void
)
{

  uint8                i;
  bt_rc_session_type*  ssn_ptr = NULL;

  for ( i = 0; i < BT_RC_MAX_SESSIONS; i++ )
  {
    if ( SSN( i ).mcc.state == BT_RCDS_DISCONNECTED )
    {
      ssn_ptr = &SSN( i );
      break;
    }
  }

  return ( ssn_ptr );

}


/*===========================================================================

FUNCTION
  bt_rc_find_session_cid

DESCRIPTION
  Finds the session table entry of a specified L2CAP channel ID.

RETURN VALUE
  Pointer to session table entry, if found; otherwise NULL.

===========================================================================*/
bt_rc_session_type* bt_rc_find_session_cid
(
  uint16  cid
)
{

  uint8                i;
  bt_rc_session_type*  ssn_ptr = NULL;

  for ( i = 0; i < BT_RC_MAX_SESSIONS; i++ )
  {
    if ( SSN( i ).mcc.state != BT_RCDS_DISCONNECTED )
    {
      if ( SSN( i ).cid == cid )
      {
        ssn_ptr = &SSN( i );
        break;
      }
    }
  }

  return ( ssn_ptr );

}


/*===========================================================================

FUNCTION
  bt_rc_find_session_bd_addr

DESCRIPTION
  Finds the session table entry of a specified remote BT device address.

RETURN VALUE
  Pointer to session table entry, if found; otherwise NULL.

===========================================================================*/
bt_rc_session_type* bt_rc_find_session_bd_addr
(
  bt_bd_addr_type*  bd_addr_ptr
)
{

  uint8                i;
  bt_rc_session_type*  ssn_ptr = NULL;

  for ( i = 0; i < BT_RC_MAX_SESSIONS; i++ )
  {
    if ( SSN( i ).mcc.state != BT_RCDS_DISCONNECTED )
    {
      if ( BT_BD_ADDRS_EQUAL( &(SSN( i ).bd_addr), bd_addr_ptr ) )
      {
        ssn_ptr = &SSN( i );
        break;
      }
    }
  }

  return ( ssn_ptr );

}


/*===========================================================================

FUNCTION
  bt_rc_find_free_dlc_entry

DESCRIPTION
  Finds a free DLC table entry within a specified session table entry.

RETURN VALUE
  Pointer to free DLC table entry, if found; otherwise NULL.

===========================================================================*/
bt_rc_dlc_type* bt_rc_find_free_dlc_entry
(
  bt_rc_session_type*  ssn_ptr
)
{

  uint8            i;
  bt_rc_dlc_type*  dlc_ptr = NULL;

  for ( i = 0; i < BT_RC_MAX_PORTS_PER_SESSION; i++ )
  {
    if ( ssn_ptr->dlc[ i ].state == BT_RCDS_DISCONNECTED )
    {
      dlc_ptr = &ssn_ptr->dlc[ i ];
      break;
    }
  }

  return ( dlc_ptr );

}


/*===========================================================================

FUNCTION
  bt_rc_find_dlc_entry_dlci

DESCRIPTION
  Finds the DLC entry within a specified session corresponding to a
  specified DLCI.

RETURN VALUE
  Pointer to DLC table entry, if found; otherwise NULL.

===========================================================================*/
bt_rc_dlc_type* bt_rc_find_dlc_entry_dlci
(
  bt_rc_session_type*  ssn_ptr,
  uint8                dlci
)
{

  uint8            i;
  bt_rc_dlc_type*  dlc_ptr = NULL;

  for ( i = 0; i < BT_RC_MAX_PORTS_PER_SESSION; i++ )
  {
    if ( ssn_ptr->dlc[ i ].state != BT_RCDS_DISCONNECTED )
    {
      if ( ssn_ptr->dlc[ i ].dlci == dlci )
      {
        dlc_ptr = &ssn_ptr->dlc[ i ];
        break;
      }
    }
  }

  return ( dlc_ptr );

}


/*===========================================================================

FUNCTION
  bt_rc_setup_local_server_dlc

DESCRIPTION
  Sets up a new DLC for a local server based on a received RFCOMM frame.
  Consistency checks are performed to validate this operation:
    1)  The received frame must be an SABM frame
    2)  The direction within the DLCI must be correct
    3)  The server channel must be correct and registered locally

RETURN VALUE
  dlc_ptr in rx_info:
    Updated if all is OK;  otherwise set to NULL.

===========================================================================*/
void bt_rc_setup_local_server_dlc
(
  bt_rc_rx_info_type*  rxi_ptr
)
{

  bt_rc_reg_table_type*  reg_ptr;

  rxi_ptr->dlc_ptr = NULL;

  if ( (rxi_ptr->fh_frame_type == RCFH_CTRL_FT_SABM_V ) &&
       (rxi_ptr->fh_command != FALSE) )
  {
    /*  For local servers, the direction of the DLC is  */
    /*  equal to the multiplexer's initiator setting.   */

    if ( rxi_ptr->fh_direction == rxi_ptr->ssn_ptr->initiator )
    {
      if ( (reg_ptr = bt_rc_find_regd_server_channel(
                        rxi_ptr->fh_server_channel )) != NULL )
      {
        if ( reg_ptr->connected == FALSE )
        {
          if ( (rxi_ptr->dlc_ptr = bt_rc_find_free_dlc_entry(
                  rxi_ptr->ssn_ptr )) != NULL )
          {
            rxi_ptr->dlc_ptr->dlci             = rxi_ptr->fh_dlci;
            rxi_ptr->dlc_ptr->direction        = rxi_ptr->fh_direction;
            rxi_ptr->dlc_ptr->server_channel   = rxi_ptr->fh_server_channel;
            rxi_ptr->dlc_ptr->bt_app_id        = reg_ptr->bt_app_id;
            rxi_ptr->dlc_ptr->tx_wm_ptr        = reg_ptr->tx_wm_ptr;
            rxi_ptr->dlc_ptr->rx_wm_ptr        = reg_ptr->rx_wm_ptr;
            rxi_ptr->dlc_ptr->state            = BT_RCDS_CONNECTING_DLC;
            rxi_ptr->dlc_ptr->expected_rx_mask = BT_ERXM_SABM_B;

            bt_rc_set_local_server_dlc_config( rxi_ptr->ssn_ptr,
                                               rxi_ptr->dlc_ptr );

            rxi_ptr->ssn_ptr->dlc_count++;

            reg_ptr->connected = TRUE;
          }
          else
          {
            /*  No room for new DLC;  send DM.  */
            bt_rc_tx_dm_frame( rxi_ptr->ssn_ptr, rxi_ptr->fh_dlci );
            BT_ERR( "BT RC: No DLCs avail SC %x CID %x",
                    rxi_ptr->fh_server_channel,
                    rxi_ptr->ssn_ptr->cid, 0 );
          }
        }
        else
        {
          /*  Server channel already connected.  */
          bt_rc_tx_dm_frame( rxi_ptr->ssn_ptr, rxi_ptr->fh_dlci );
          BT_ERR( "BT RC: SC %x in use %x CID %x",
                  rxi_ptr->fh_server_channel,
                  rxi_ptr->ssn_ptr->cid, 0 );
        }
      }
      else
      {
        /*  Server channel not registered; send DM.  */
        bt_rc_tx_dm_frame( rxi_ptr->ssn_ptr, rxi_ptr->fh_dlci );
        BT_ERR( "BT RC: SC %x not reg'd CID %x",
                rxi_ptr->fh_server_channel,
                rxi_ptr->ssn_ptr->cid, 0 );
      }
    }
    else
    {
      /*  Direction in SABM address is incorrect for local server.  */
      BT_ERR( "BT RC: Bad SABM Dir %x Init %x CID %x",
              rxi_ptr->fh_direction,
              rxi_ptr->ssn_ptr->initiator,
              rxi_ptr->ssn_ptr->cid );
    }
  }
  else
  {
    /*  Non-SABM on new DLCI.  */
    BT_ERR( "BT RC: Not SABM FT %x Cmd %x CID %x",
            rxi_ptr->fh_frame_type,
            rxi_ptr->fh_command,
            rxi_ptr->ssn_ptr->cid );
  }

}


/*===========================================================================

FUNCTION
  bt_rc_set_local_server_dlc_config

DESCRIPTION
  Fills in the configuration settings of a specified local server DLC.
  Defaults are used if there is no entry in the negotiation table.
  The negotiation table would have an entry if a client performed
  Port Negotiations or Remote Port Negotiations via PN and/or RPN MCC
  messages prior to sending the SABM frame for this DLC.

===========================================================================*/
void bt_rc_set_local_server_dlc_config
(
  bt_rc_session_type*  ssn_ptr,
  bt_rc_dlc_type*      dlc_ptr
)
{

  bt_rc_negot_table_type*  nte_ptr;

  nte_ptr = bt_rc_find_negot_table_entry( dlc_ptr->dlci,
                                          &ssn_ptr->bd_addr, FALSE );

  if ( nte_ptr != NULL )
  {
    dlc_ptr->config = nte_ptr->config;

    bt_rc_remove_negot_table_entry( nte_ptr );
  }
  else
  {
    bt_rc_initialize_config( &dlc_ptr->config );
    if ( dlc_ptr->config.max_frame_size >
           ssn_ptr->mcc.config.max_frame_size )
    {
      BT_MSG_HIGH( "BT RC: Made MFS %x not %x PID %x",
                   ssn_ptr->mcc.config.max_frame_size,
                   dlc_ptr->config.max_frame_size,
                   dlc_ptr->pid );
      dlc_ptr->config.max_frame_size =
        ssn_ptr->mcc.config.max_frame_size;
    }
    dlc_ptr->config.priority =
      bt_rc_get_default_priority_for_dlci( dlc_ptr->dlci );
  }

  if ( dlc_ptr->config.pn_negot_done == FALSE )
  {
    /*  If not explicitly negotiated by the client  */
    /*  we cannot use Credit Based Flow Control.    */
    dlc_ptr->config.cbfc_enabled       = FALSE;
    dlc_ptr->config.cbfc_local_credits = 0;
    dlc_ptr->config.cbfc_far_credits   = 0;
  }

  bt_rc_setup_fast_uih_fcs( ssn_ptr, dlc_ptr );

}


/*===========================================================================

FUNCTION
  bt_rc_set_local_client_dlc_config

DESCRIPTION
  Fills in the configuration settings of a specified local client DLC.
  Defaults are used if there was no configuration information previously
  set via a Connect with Custom Configuration command.

===========================================================================*/
void bt_rc_set_local_client_dlc_config
(
  bt_rc_session_type*  ssn_ptr,
  bt_rc_dlc_type*      dlc_ptr
)
{

  uint8                    i = 0;
  bt_rc_negot_table_type*  nte_ptr = NULL;
  uint32                   tmp_mfs;

  while ( (i < bt_rc_negot_count) && (nte_ptr == NULL) )
  {
    if ( (bt_rc_negot_table[ i ].bt_app_id == dlc_ptr->bt_app_id) &&
         (BT_BD_ADDRS_EQUAL( &bt_rc_negot_table[ i ].bd_addr,
                             &ssn_ptr->bd_addr )) &&
         (bt_rc_negot_table[ i ].server_channel == 
            dlc_ptr->server_channel) )
    {
      nte_ptr = &bt_rc_negot_table[ i ];
    }
    i++;
  }

  if ( nte_ptr != NULL )
  {
    dlc_ptr->config = nte_ptr->config;

    bt_rc_remove_negot_table_entry( nte_ptr );
  }
  else
  {
    bt_rc_initialize_config( &dlc_ptr->config );
    dlc_ptr->config.priority =
      bt_rc_get_default_priority_for_dlci( dlc_ptr->dlci );
  }

  tmp_mfs = bt_rc_calc_optimal_mfs( ssn_ptr->mcc.config.max_frame_size,
                                    dlc_ptr->rx_wm_ptr );

  if ( dlc_ptr->config.max_frame_size > tmp_mfs )
  {
    BT_MSG_HIGH( "BT RC: Made MFS %x not %x PID %x",
                 tmp_mfs,
                 dlc_ptr->config.max_frame_size,
                 dlc_ptr->pid );
    dlc_ptr->config.max_frame_size = tmp_mfs;
  }

  bt_rc_setup_fast_uih_fcs( ssn_ptr, dlc_ptr );

}


/*===========================================================================

FUNCTION
  bt_rc_loc_client_dlc_srv_sec_result

DESCRIPTION
  Processes service security results initiated by startup of a local
  client DLC.

===========================================================================*/
void bt_rc_loc_client_dlc_srv_sec_result
(
  bt_rc_session_type*  ssn_ptr,
  bt_rc_dlc_type*      dlc_ptr,
  boolean              secure_enough
)
{

  if ( secure_enough != FALSE )
  {
    bt_rc_tx_sabm_frame( ssn_ptr, dlc_ptr );
  }
  else
  {
    /*  Connection does not meet security requirements.  */
    /*  Inform application of connection failure.        */

    bt_rc_bring_down_dlc( ssn_ptr, dlc_ptr,
                          TRUE,  /*  Connection failed.  */
                          BT_EVR_RM_SECURITY_NOT_MET );
    
    BT_MSG_HIGH( "BT RC: SSec reqs DISC PID %x SCN %x",
                 dlc_ptr->pid, dlc_ptr->server_channel, 0 );
    BT_BDA( MSG_HIGH, "BT RC: SSec reqs DISC", &ssn_ptr->bd_addr );
  }

}


/*===========================================================================

FUNCTION
  bt_rc_bring_up_local_client_dlc

DESCRIPTION
  Steps through the appropriate transmissions of required MCC commands
  and/or RFCOMM frames to bring up a specified local client DLC.  This
  includes transmitting a Port Negotiation (PN) command, a Remote Port
  Negotiation (RPN) command, and a Set Asynchronous Balanced Mode
  (SABM) frame, in that order.

===========================================================================*/
void bt_rc_bring_up_local_client_dlc
(
  bt_rc_session_type*  ssn_ptr,
  bt_rc_dlc_type*      dlc_ptr
)
{

  if ( bt_rc_need_port_negotiation( dlc_ptr ) != FALSE )
  {
    bt_rc_tx_pn_command( ssn_ptr, dlc_ptr );
  }
  else if ( bt_rc_need_remote_port_negotiation( dlc_ptr ) != FALSE )
  {
    bt_rc_tx_rpn_command( ssn_ptr, dlc_ptr, FALSE );
  }
  else
  {
    BT_MSG_API( "BT RC CMD TX: RM Enf Sec SCN %x LC %x",
                dlc_ptr->server_channel, TRUE, 0 );
    BT_BDA( MSG_API, "BT RC CMD TX: RM Enf Sec SCN", &ssn_ptr->bd_addr );
    bt_cmd_rm_enforce_security_rfcomm_scn(
      bt_rc_app_id, dlc_ptr->server_channel,
      &ssn_ptr->bd_addr, TRUE );
  }

}


#ifdef BT_SWDEV_QXDM_LOG_PER
/*===========================================================================

FUNCTION
  bt_rc_send_log

DESCRIPTION
  Logs all the RCFCOMM specific statistics through QXDM.

===========================================================================*/
void bt_rc_send_log( void )
{

  log_bt_rc_session_type*  ls_ptr;
  log_bt_rc_dlci_type*     ld_ptr;
  bt_rc_session_type*      ssn_ptr = NULL;
  bt_rc_dlc_type*          dlci_ptr = NULL;
  int                      i, j;

  for ( i = 0; i < BT_RC_MAX_SESSIONS; i++ )
  {
    ssn_ptr = &(SSN( i ));

    if ( ( ssn_ptr->mcc.state != BT_RCDS_DISCONNECTED ) &&
         ( (ls_ptr = (log_bt_rc_session_type *)log_alloc( log_bt_rc_session,
                                sizeof( log_bt_rc_session_type))) != NULL ) )
    {
       ls_ptr->rc_ssn_idx = ssn_ptr->ssn_idx;
       ls_ptr->rc_mcc_state = (uint8)(ssn_ptr->mcc.state);
       *((bt_bd_addr_type *)ls_ptr->rc_bd_addr) = ssn_ptr->bd_addr;
       ls_ptr->rc_cid = ssn_ptr->cid;
       ls_ptr->rc_initiator = (uint8) ssn_ptr->initiator;

       log_commit( ls_ptr );
    }
   
    for ( j = 0; j < ssn_ptr->dlc_count; j++ )
    {
      dlci_ptr = &(ssn_ptr->dlc[ j ]);

      ld_ptr = ( log_bt_rc_dlci_type *)log_alloc( log_bt_rc_dlci,
                          sizeof( log_bt_rc_dlci_type ) );
      if ( ld_ptr != NULL )
      {
        ld_ptr->rc_ssn_idx               = ssn_ptr->ssn_idx;
        ld_ptr->rc_dlci                  = dlci_ptr->dlci;
        ld_ptr->rc_dlci_state            = dlci_ptr->state;
        ld_ptr->rc_dlci_pid              = dlci_ptr->pid;
        ld_ptr->rc_dlci_direction        = dlci_ptr->direction;
        ld_ptr->rc_server_channel        = dlci_ptr->server_channel;
        ld_ptr->rc_app_id                = dlci_ptr->bt_app_id;
        ld_ptr->rc_max_frame_size        = dlci_ptr->config.max_frame_size;
        ld_ptr->rc_expected_rx_mask      = dlci_ptr->expected_rx_mask;
        ld_ptr->rc_local_msc_v24_signals = dlci_ptr->local_v24_signals;
        ld_ptr->rc_far_msc_v24_signals   = dlci_ptr->far_v24_signals;
        ld_ptr->rc_tx_errors             = dlci_ptr->tx_errors;
        ld_ptr->rc_rx_errors             = dlci_ptr->rx_errors;
        ld_ptr->rc_tx_data_bytes         = dlci_ptr->tx_data_bytes;
        ld_ptr->rc_rx_data_bytes         = dlci_ptr->rx_data_bytes;
        ld_ptr->rc_tx_uih_frames         = dlci_ptr->tx_uih_frames;
        ld_ptr->rc_rx_uih_frames         = dlci_ptr->rx_uih_frames;
        ld_ptr->rc_tx_wm_max_cnt         = dlci_ptr->tx_wm_ptr->dont_exceed_cnt;
        if ( dlci_ptr->rx_wm_ptr != NULL )
        {
          ld_ptr->rc_rx_wm_max_cnt       = dlci_ptr->rx_wm_ptr->dont_exceed_cnt;
          ld_ptr->rc_rx_wm_current_cnt   = dlci_ptr->rx_wm_ptr->current_cnt;
        }
        else
        {
          ld_ptr->rc_rx_wm_max_cnt       = 0;
          ld_ptr->rc_rx_wm_current_cnt   = 0;
        }
        ld_ptr->rc_tx_wm_current_cnt     = dlci_ptr->tx_wm_ptr->current_cnt;

        log_commit( ld_ptr );
      }
    }
  }

}
#endif /* BT_SWDEV_QXDM_LOG_PER */

#endif /* FEATURE_BT */

