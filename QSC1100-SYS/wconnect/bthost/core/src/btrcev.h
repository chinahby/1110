#ifndef _BTRCEV_H
#define _BTRCEV_H

#ifdef FEATURE_BT

/*====*====*====*====*====*====*====*====*====*====*====*====*====*====*====*

             B L U E T O O T H    R F C O M M    E V E N T S

DESCRIPTION
  This file contains declarations for the Bluetooth code that handles
  BT events for the RFCOMM layer.

Copyright (c) 2000 - 2006 by QUALCOMM Incorporated. All Rights Reserved.
*====*====*====*====*====*====*====*====*====*====*====*====*====*====*====*/

/*===========================================================================

                        EDIT HISTORY FOR MODULE

  This section contains comments describing changes made to the module.
  Notice that changes are listed in reverse chronological order.

$PVCSHeader:   O:/src/asw/COMMON/vcs/btrcev.h_v   1.5   03 Apr 2002 14:06:38   propach
$Header: //source/qcom/qct/wconnect/bthost/core/rel/00.00.26/src/btrcev.h#1 $ $DateTime:

$Log:   O:/src/asw/COMMON/vcs/btrcev.h_v  $
 * 
 *    #4        16 Mar 2006            JTL
 * Added Unregister event.
 *
 *    Rev 1.5   03 Apr 2002 14:06:38   propach
 * Enhanced modem status changed events.
 * 
 *    Rev 1.4   23 Oct 2001 16:52:02   propach
 * Added support for RFCOMM Credit Based Flow Control (CBFC).
 * 
 *    Rev 1.3   07 Sep 2001 16:06:28   propach
 * Optimized event handling.
 * 
 *    Rev 1.2   08 Jun 2001 15:03:30   propach
 * Added RFCOMM link negotiations and modem status lines for SPP certification.
 * 
 *    Rev 1.1   02 Mar 2001 11:57:30   propach
 * Removed unused online/offline/init routines.
 * 
 *    Rev 1.0   29 Jan 2001 17:09:20   propach
 * Initial revision.
 * 
 *    Rev 1.4   01 Dec 2000 12:26:46   sramacha
 * Added bt_rc_ev_send_connection_failed event.
 * 
 *    Rev 1.3   16 Sep 2000 15:12:20   propach
 * Add flow control between top of RFCOMM and BB for NA.  Also avoid
 * queueing of payload data.
 *
 *    Rev 1.2   05 Sep 2000 17:56:08   propach
 * Added QDSP services changes.
 *
 *    Rev 1.1   17 Aug 2000 13:45:28   propach
 * Added Event Control (EC).  Fixed minor problems in RFCOMM.
 *
 *    Rev 1.0   04 Jul 2000 19:21:52   ryuen
 * Initial revision.

===========================================================================*/

/*===========================================================================

                        INCLUDE FILES FOR MODULE

===========================================================================*/

#include "comdef.h"
#include "bt.h"


/*-------------------------------------------------------------------------*/
/*                                                                         */
/*                    External Functions Prototypes                        */
/*                                                                         */
/*-------------------------------------------------------------------------*/

/*===========================================================================

FUNCTION
  bt_rc_get_max_event_bytes

DESCRIPTION
  Returns the number of bytes in the largest event expected to be
  processed by RFCOMM.

===========================================================================*/
extern uint16 bt_rc_get_max_event_bytes( void );


/*===========================================================================

FUNCTION
  bt_rc_process_event

DESCRIPTION
  Handles all BT driver events coming to the RFCOMM layer.

===========================================================================*/
extern void bt_rc_process_event( bt_ev_msg_type* ev_msg_ptr );


/*===========================================================================

FUNCTION
  bt_rc_ev_copy_config

DESCRIPTION
  Copies specified DLC configuration information into a specified
  RFCOMM event configuration field.

===========================================================================*/
extern void bt_rc_ev_copy_config(
  bt_rc_ev_config_type*  ev_cfg_ptr,
  bt_rc_config_type*     dlc_cfg_ptr
);


/*===========================================================================

FUNCTION
  bt_rc_ev_send_connected

DESCRIPTION
  Sends an RFCOMM connected event with specified parameters.

===========================================================================*/
extern void bt_rc_ev_send_connected(
  bt_app_id_type      bt_app_id,
  bt_bd_addr_type*    bd_addr_ptr,
  uint16              pid,
  bt_rc_config_type*  config
);


/*===========================================================================

FUNCTION
  bt_rc_ev_send_ms_changed

DESCRIPTION
  Sends an RFCOMM Modem Status changed event with specified parameters.

===========================================================================*/
extern void bt_rc_ev_send_ms_changed(
  bt_app_id_type  bt_app_id,
  uint16          pid,
  uint8           ms_changed_mask,
  uint8           modem_status,
  boolean         break_present,
  uint8           break_length,
  uint8           breaks
);


/*===========================================================================

FUNCTION
  bt_rc_ev_send_connection_failed

DESCRIPTION
  Sends an RFCOMM connection failed event with specified parameters.

===========================================================================*/
extern void bt_rc_ev_send_connection_failed(
  bt_app_id_type       bt_app_id,
  bt_bd_addr_type*     bd_addr_ptr,
  bt_event_reason_type reason
);


/*===========================================================================

FUNCTION
  bt_rc_ev_send_disconnected

DESCRIPTION
  Sends an RFCOMM disconnected event with specified parameters.

===========================================================================*/
extern void bt_rc_ev_send_disconnected(
  bt_app_id_type bt_app_id,
  uint16         pid,
  bt_event_reason_type  reason
);

/*===========================================================================

FUNCTION
  bt_rc_ev_send_unregistered

DESCRIPTION
  Sends an RFCOMM unregistered event with specified parameters.

===========================================================================*/
extern void bt_rc_ev_send_unregistered(
  bt_app_id_type bt_app_id,
  uint8          server_channel
);

#ifdef BT_TEST_PYLD_FAST_RX
/*===========================================================================

FUNCTION
  bt_rc_ev_l2_rx_data

DESCRIPTION
  Handles an L2CAP received data event - only to be used internally by
  the BT driver.

===========================================================================*/
extern void bt_rc_ev_l2_rx_data( bt_ev_msg_type* l2_rxd_ptr );
#endif /* BT_TEST_PYLD_FAST_RX */


#endif /* FEATURE_BT */

#endif /* _BTRCEV_H */

