#ifndef _BTHCPE_H
#define _BTHCPE_H

#ifdef FEATURE_BT

/*====*====*====*====*====*====*====*====*====*====*====*====*====*====*====*

          B L U E T O O T H    H C I    P A R S E R    H E A D E R

DESCRIPTION
  This file contains declarations external declarations of the HCI Parse
  Engine Module.

Copyright (c) 2000 by QUALCOMM Incorporated. All Rights Reserved.
*====*====*====*====*====*====*====*====*====*====*====*====*====*====*====*/

/*===========================================================================

                        EDIT HISTORY FOR MODULE

  This section contains comments describing changes made to the module.
  Notice that changes are listed in reverse chronological order.

$PVCSHeader:   O:/src/asw/COMMON/vcs/bthcpe.h_v   1.3   20 Apr 2001 16:25:08   waynelee
$Header: //source/qcom/qct/wconnect/bthost/core/rel/00.00.26/protected/bthcpe.h#1 $ $DateTime:

$Log:   O:/src/asw/COMMON/vcs/bthcpe.h_v  $
 * 
 *    #4        03 Jul 2007            SSK
 * Adding APIs to expose HCI RESET state in HCI APIT test mode.
 *
 *    #3        06 Apr 2005            JH
 * Adding support for disabling BT radio, featurized using BT_SWDEV_RADIO_OFF.
 *
 *    Rev 1.3   20 Apr 2001 16:25:08   waynelee
 * Changed symbols from bt_hci_ to bt_hc_
 * 
 *    Rev 1.2   02 Mar 2001 00:32:18   ryuen
 * Renamed bt_xx_offline to bt_xx_qdsp_reset_shutdown
 * 
 *    Rev 1.1   23 Feb 2001 12:49:18   ryuen
 * Renamed bt_hc_pe_init to bt_hci_pe_init
 * 
 *    Rev 1.0   29 Jan 2001 17:07:32   propach
 * Initial revision.
 * 
 *    Rev 1.4   05 Sep 2000 16:44:40   waynelee
 * Added online, offline support.  Added debug MSGs.  Added Robustness.
 * 
 *    Rev 1.3   28 Aug 2000 16:12:32   propach
 * Cleanup RFCOMM.  Add use of msg/err macros.  Add driver fault
 * recording and statistics gathering.
 *
 *    Rev 1.2   03 Aug 2000 08:41:56   waynelee
 * Adding Low Power Mode Support.  Code for UPF-3
 *
 *    Rev 1.1   02 Aug 2000 09:40:12   propach
 * Added QBTS support for BT.  Added to API.
 *
 *    Rev 1.0   04 Jul 2000 19:20:06   ryuen
 * Initial revision.

===========================================================================*/

/*===========================================================================

                     INCLUDE FILES FOR MODULE

===========================================================================*/

/*===========================================================================

          FUNCTION DEFINITIONS AND DECLARATIONS FOR MODULE

This section contains definitions for functions, local and global.

===========================================================================*/

/*-------------------------------------------------------------------------*/
/*                                                                         */
/*                        External Functions                               */
/*                                                                         */
/*-------------------------------------------------------------------------*/

/*===========================================================================

FUNCTION
  bt_hc_pe_init

DESCRIPTION
  Initializes the HCI parser module.

===========================================================================*/
extern void bt_hc_pe_init( void );


/*===========================================================================

FUNCTION
  bt_hc_pe_qdsp_reset_shutdown

DESCRIPTION
  De-initialize the HCI parser module

===========================================================================*/
extern void bt_hc_pe_qdsp_reset_shutdown( void );


/*===========================================================================

FUNCTION
  bt_hc_process_ev_queue

DESCRIPTION
  Processes the HCI event queue.

===========================================================================*/
extern void bt_hc_process_ev_queue( void );

#ifdef BT_SWDEV_RADIO_OFF

/*===========================================================================

FUNCTION
  bt_hc_empty_ev_queue

DESCRIPTION
  Empties the HCI event queue.

===========================================================================*/
extern void bt_hc_empty_ev_queue( void );

#endif /* BT_SWDEV_RADIO_OFF */

/*===========================================================================

FUNCTION
  bt_hc_pe_parse_host_msg

DESCRIPTION
  Parses host messages.

===========================================================================*/
extern void bt_hc_pe_parse_host_msg( void );

/*===========================================================================

FUNCTION
  bt_hc_store_bt_event

DESCRIPTION
  Stores a specified BT event on the HCI BT event queue.

===========================================================================*/
extern void bt_hc_store_bt_event
(
  bt_ev_msg_type*  bt_ev_ptr  /*  The BT event to store.  */
);

/*===========================================================================

FUNCTION
  bt_hc_pe_is_hci_rst_started

DESCRIPTION
  Returns TRUE if HCI Reset has started through a HCI RST comand issued
  by a test host in HCI APIT Test mode, else returns False.

===========================================================================*/
extern  boolean bt_hc_pe_is_hci_rst_started( void );

/*===========================================================================

FUNCTION
  bt_hc_pe_hci_rst_done

DESCRIPTION
  Resets the HCI RST state flag to indicate that HCI RST procedure has finished
  in HCI APIT Test mode.

===========================================================================*/
extern void bt_hc_pe_hci_rst_done( void );

#endif /* FEATURE_BT */

#endif /* _BTHCPE_H */

