#ifndef _BTRC_H
#define _BTRC_H

#ifdef FEATURE_BT

/*====*====*====*====*====*====*====*====*====*====*====*====*====*====*====*

            B L U E T O O T H    R F C O M M    M O D U L E

DESCRIPTION
  This file contains declarations external to the Bluetooth RFCOMM module.

Copyright (c) 2000-2006 by QUALCOMM Incorporated. All Rights Reserved.
*====*====*====*====*====*====*====*====*====*====*====*====*====*====*====*/

/*===========================================================================

                        EDIT HISTORY FOR MODULE

  This section contains comments describing changes made to the module.
  Notice that changes are listed in reverse chronological order.

$PVCSHeader:   O:/src/asw/COMMON/vcs/btrc.h_v   1.4   25 Sep 2002 11:29:28   waynelee
$Header: //source/qcom/qct/wconnect/bthost/core/rel/00.00.26/protected/btrc.h#1 $ $DateTime:

$Log:   O:/src/asw/COMMON/vcs/btrc.h_v  $
 *    #6         19 Oct 2006            DSN
 * Rename BB TX flow to ACL TX flow since HCI may provide this indication for
 * SOC solution.
 *
 *    #5         16 Nov 2005            GS
 * Added support for BD address based TX flow control in RFCOMM.
 *
 *    #4         3  Oct 2003            DP
 * Added RFCOMM peer to peer communications monitoring.
 *
 *    #3         26 Sep 2003            DP
 * Cleaned up.
 * 
 *    Rev 1.4   25 Sep 2002 11:29:28   waynelee
 * Added routine to update watermarks.
 * 

===========================================================================*/

/*===========================================================================

                     INCLUDE FILES FOR MODULE

===========================================================================*/

#include "dsm.h"

#include "bt.h"
#include "btcmdi.h"
#include "btrci.h"


/*-------------------------------------------------------------------------*/
/*                                                                         */
/*                     External Data Declarations                          */
/*                                                                         */
/*-------------------------------------------------------------------------*/

extern rex_timer_type bt_rc_tx_timer;
extern boolean        bt_rc_tx_timer_running;


/*-------------------------------------------------------------------------*/
/*                                                                         */
/*                    External Functions Prototypes                        */
/*                                                                         */
/*-------------------------------------------------------------------------*/

/*===========================================================================

FUNCTION
  bt_rc_powerup_init

DESCRIPTION
  Initialize the RFCOMM layer upon powerup

===========================================================================*/
extern void bt_rc_powerup_init( void );


/*===========================================================================

FUNCTION
  bt_rc_qdsp_ready_init

DESCRIPTION
  Initialize the RFCOMM layer when the Bluetooth QDSP image is loaded

===========================================================================*/
extern void bt_rc_qdsp_ready_init( void );


/*===========================================================================

FUNCTION
  bt_rc_qdsp_reset_shutdown

DESCRIPTION
  De-initialize the RFCOMM layer when the Bluetooth QDSP image is unloaded

===========================================================================*/
extern void bt_rc_qdsp_reset_shutdown( void );


/*===========================================================================

FUNCTION
  bt_rc_process_command

DESCRIPTION
  Processes all commands intended for RFCOMM.

===========================================================================*/
extern void bt_rc_process_command( bt_cmd_msg_type* rc_cmd_msg_ptr );


/*===========================================================================

FUNCTION
  bt_rc_tx_timer_expired

DESCRIPTION
  Sends all pending RFCOMM transmit data since the last RC TX timer
  expiration.

===========================================================================*/
extern void bt_rc_tx_timer_expired( void );


/*===========================================================================

FUNCTION
  bt_rc_background_tick

DESCRIPTION
  Performs background monitoring for RFCOMM.

===========================================================================*/
extern void bt_rc_background_tick
(
  void
);


/*===========================================================================

FUNCTION
  bt_rc_process_ev_queue

DESCRIPTION
  Dequeues events on the RFCOMM event queue and processes them.

===========================================================================*/
extern void bt_rc_process_ev_queue( void );


/*===========================================================================

FUNCTION
  bt_rc_restart_tx_flow

DESCRIPTION
  Primes all RFCOMM transmits again by sending a RFCOMM Transmit
  Data command for each active PID that has flow enabled.

===========================================================================*/
extern void bt_rc_restart_tx_flow( void );

/*===========================================================================

FUNCTION
  bt_rc_update_bb_tx_flow_on

DESCRIPTION
  Sets the bb_tx_flow_on flag for a specified BD address to a
  specified value.

===========================================================================*/
extern void bt_rc_update_acl_tx_flow_on(
  bt_bd_addr_type*  bd_addr_ptr,
  boolean           bb_tx_flow_on
);

/*===========================================================================

FUNCTION
  bt_rc_update_wm_ptrs

DESCRIPTION
  Updates a specified PID's watermark pointers.

===========================================================================*/
extern void bt_rc_update_wm_ptrs
(
  uint16               rc_pid,
  dsm_watermark_type*  tx_wm_ptr,
  dsm_watermark_type*  rx_wm_ptr
);


#ifdef BT_SWDEV_QXDM_LOG_PER
/*===========================================================================

FUNCTION
  bt_rc_send_log

DESCRIPTION
  Logs all the RFCOMM specific statistics through QXDM.

===========================================================================*/
extern void bt_rc_send_log( void );
#endif /* BT_SWDEV_QXDM_LOG_PER */


#endif /* FEATURE_BT */

#endif /* _BTRC_H */
