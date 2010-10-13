#ifndef _BTL2_H
#define _BTL2_H

#ifdef FEATURE_BT

/*====*====*====*====*====*====*====*====*====*====*====*====*====*====*====*

   B L U E T O O T H    L O G I C A L    L I N K    C O N T R O L    A N D

                   A D A P T A T I O N    P R O T O C O L

DESCRIPTION
  This module contains data and function definitions relevent to the
  Bluetooth L2CAP layer.

Copyright (c) 2000-2007 by QUALCOMM Incorporated. All Rights Reserved.
*====*====*====*====*====*====*====*====*====*====*====*====*====*====*====*/

/*===========================================================================

                        EDIT HISTORY FOR MODULE

  This section contains comments describing changes made to the module.
  Notice that changes are listed in reverse chronological order.

$PVCSHeader:   O:/src/asw/COMMON/vcs/btl2.h_v   1.6   20 Feb 2002 09:32:54   waynelee
$Header: //source/qcom/qct/wconnect/bthost/core/rel/00.00.26/protected/btl2.h#1 $ $DateTime:

$Log:   O:/src/asw/COMMON/vcs/btl2.h_v  $
 *    #12      14 Mar 2007            JN
 * Remove compiler warning.
 *
 *    #11      16 Nov 2006            DM
 * Removed unused function bt_l2_process_tx_data_cnf()
 *
 *    #10      08 Nov 2006            DM
 * Moved ACL Tx Flow definition from here to bti.h
 *
 *    #9       03 Nov 2006            DM
 * Changed Update ACL Tx flow interface from conn_hndl to bd_addr
 *
 *    #8       19 Oct 2006            DSN
 * Update ACL TX flow indication from BB/HC per connection handle.
 *
 *    #7       26 Jul 2006            DM
 * Added bt_l2_process_tx_data_cnf
 *
 *   #6        27 Jun 2006            DSN
 * Removed superfluous header file inclusion.
 *
 *    #5       12 Apr 2006            AP/PG
 * Fix for lingering L2CAP connections setup for L2PING
 *
 *    #4         3  Oct 2003            DP
 * Converted RM timer to background timer for entire driver.
 * 
 *    Rev 1.6   20 Feb 2002 09:32:54   waynelee
 * Fixed prototype name.
 * 
 *    Rev 1.5   17 May 2001 17:57:30   amitj
 * Declared bt_l2_expected_resp_timer_expired()
 * 
 *    Rev 1.4   02 Mar 2001 12:20:58   propach
 * Moved L2CAP message format and internal table declarations into
 * btl2cap.c.  Renamed online/offline routines.
 * 
 *    Rev 1.3   23 Feb 2001 13:18:28   ryuen
 * Renamed bt_l2_initialize and bt_l2_online to bt_l2_powerup_init and
 * bt_l2_qdsp_ready_init respectively
 * 
 *    Rev 1.2   16 Feb 2001 16:11:12   propach
 * Completed EC routing of events by BT application ID.
 * 
 *    Rev 1.1   02 Feb 2001 11:01:16   amitj
 * Declared bt_l2_app_id
 * 
 *    Rev 1.0   29 Jan 2001 17:08:02   propach
 * Initial revision.
 * 
 *    Rev 1.9   30 Nov 2000 22:03:30   ryuen
 * Added actual values to enum literals per Bluetooth specification
 * 
 *    Rev 1.8   13 Nov 2000 18:02:24   amitj
 * Removed the prototypes of rx/tx logging functions
 * 
 *    Rev 1.7   08 Nov 2000 18:46:28   amitj
 * Declared 5 new functions:  bt_l2_sig_tx_logging_function(), 
 * bt_l2_sig_rx_logging_function(), bt_l2_pyld_tx_logging_function(),
 * bt_l2_pyld_rx_logging_function() and bt_l2_snapshot_logging_function()
 * 
 *    Rev 1.6   16 Sep 2000 15:11:58   propach
 * Add flow control between top of RFCOMM and BB for NA.  Also avoid
 * queueing of payload data.
 *
 *    Rev 1.5   05 Sep 2000 16:45:08   waynelee
 * Added online, offline support.  Added debug MSGs.  Added Robustness.
 *
 *    Rev 1.4   28 Aug 2000 19:43:54   waynelee
 * Fixed L2CAP Disconnect.  Added Flow Ctrl support for BB.
 *
 *    Rev 1.3   21 Aug 2000 17:54:58   waynelee
 * UPF-3 fixes.  Properly chain continuation packets.  Process signalling
 * packets w/ multiple cmds.  Process conn rsp pending cmd.
 *
 *    Rev 1.2   17 Aug 2000 13:44:36   propach
 * Added Event Control (EC).  Fixed minor problems in RFCOMM.
 *
 *    Rev 1.1   19 Jul 2000 17:26:04   propach
 * Update BT API and featurization with cleanup.
 *
 *    Rev 1.0   04 Jul 2000 19:20:40   ryuen
 * Initial revision.

===========================================================================*/

/*===========================================================================

                     INCLUDE FILES FOR MODULE

===========================================================================*/

#include "dsm.h"
#include "bt.h"
#include "bti.h"


/*===========================================================================

             DATA DEFINITIONS AND DECLARATIONS FOR MODULE

This section contains definitions for constants, macros, types, variables
and other items needed by this module.

===========================================================================*/

/*===========================================================================

FUNCTION
  bt_l2_powerup_init

DESCRIPTION
  Initialize the L2CAP layer upon powerup.

===========================================================================*/
extern void bt_l2_powerup_init( void );

/*===========================================================================

FUNCTION
  bt_l2_qdsp_ready_init

DESCRIPTION
  Initialize the L2CAP layer when the Bluetooth QDSP image is loaded.

===========================================================================*/
extern void bt_l2_qdsp_ready_init( void );


/*===========================================================================

FUNCTION
  bt_l2_qdsp_reset_shutdown

DESCRIPTION
  De-initialize the L2CAP layer when the Bluetooth QDSP image is unloaded.

===========================================================================*/
extern void bt_l2_qdsp_reset_shutdown( void );


/*===========================================================================

FUNCTION
  bt_l2_process_command

DESCRIPTION
  Processes a specified L2CAP command.

===========================================================================*/
extern void bt_l2_process_command( bt_cmd_msg_type* l2_cmd_ptr );


/*===========================================================================

FUNCTION
  bt_l2_process_ev_queue

DESCRIPTION
  Dequeues events on the L2CAP event queue and processes them.

===========================================================================*/
extern void bt_l2_process_ev_queue( void );


/*===========================================================================

FUNCTION
  bt_l2_expected_resp_timer_expired

DESCRIPTION
  Callback function for the signalling message response timer.

===========================================================================*/
extern void bt_l2_expected_resp_timer_expired
(
  void
);


/*===========================================================================

FUNCTION
  bt_l2_background_tick

DESCRIPTION
  Performs background monitoring for L2CAP.

===========================================================================*/
extern void bt_l2_background_tick
(
  void
);

/*===========================================================================

FUNCTION
  bt_l2_disconnect_acl_if_unused

DESCRIPTION
  disconnect acl if link is unused.

===========================================================================*/
extern void bt_l2_disconnect_acl_if_unused
(
  void
);


#ifdef BT_TEST_PYLD_FAST_RX
/*===========================================================================

FUNCTION
  bt_l2_process_rm_rx_acl_data

DESCRIPTION
  Processes the BT_EV_RM_RX_ACL_DATA event - only to be used internally
  by the BT driver.

===========================================================================*/
extern void bt_l2_process_rm_rx_acl_data( bt_ev_msg_type* ev_msg_ptr );
#endif /* BT_TEST_PYLD_FAST_RX */


#ifdef BT_TEST_PYLD_FAST_TX
/*===========================================================================

FUNCTION
  bt_l2_cmd_write

DESCRIPTION
  Transmits L2CAP data - only to be used internally by the BT driver.

===========================================================================*/
extern void bt_l2_cmd_write( bt_cmd_msg_type* l2_write_ptr );
#endif /* BT_TEST_PYLD_FAST_TX */

/*===========================================================================

FUNCTION
  bt_l2_update_acl_tx_flow_on

DESCRIPTION
  Sets the acl_tx_flow_on flag for a specified BD ADDR to a specified value.

===========================================================================*/
extern void bt_l2_update_acl_tx_flow_on
(
  const bt_bd_addr_type *bd_addr_ptr,  
                               /* Pointer to BD ADDR of the ACL connection */
  boolean               acl_tx_flow_on 
                               /* ACL Tx Flow indication from lower layers */
);


/*===========================================================================

FUNCTION
  bt_l2_set_max_hci_acl_data_len

DESCRIPTION
  Sets the the Max HCI ACL data size value in bytes.

===========================================================================*/
extern void bt_l2_set_max_hci_acl_data_len
( 
  uint16 max_acl_len 
);


#ifdef BT_SWDEV_QXDM_LOG_PER
/*===========================================================================

FUNCTION
  bt_l2_send_log

DESCRIPTION
  Logs all the L2CAP specific statistics through QXDM.

===========================================================================*/
void bt_l2_send_log( void );
#endif /* BT_SWDEV_QXDM_LOG_PER */

#endif /* FEATURE_BT */

#endif /* _BTL2_H */
