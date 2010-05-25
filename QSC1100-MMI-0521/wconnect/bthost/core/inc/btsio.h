#ifndef BTSIO_H
#define BTSIO_H
/*===========================================================================

         B L U E T O O T H   S E R I A L    I / O    M O D U L E

DESCRIPTION
  This file contains declarations external to the Bluetooth SIO module.

Copyright (c) 2000 by QUALCOMM, Incorporated.  All Rights Reserved.

===========================================================================*/


/*===========================================================================

                      EDIT HISTORY FOR FILE

$PVCSHeader:   O:/src/asw/COMMON/vcs/btsio.h_v   1.4   24 Jul 2002 12:05:36   propach
$Header: //source/qcom/qct/wconnect/bthost/core/rel/00.00.26/inc/btsio.h#1 $ $DateTime:

$Log:   O:/src/asw/COMMON/vcs/btsio.h_v  $
 *
 *    #8        22 Oct 2005            JH
 * Replaced FEATURE_PCAA with FEATURE_PCAA_SIRIUS.
 *
 *    #7        27 Jul 2005            SL
 * Made changes to support PCAA over SPP.
 *
 *    #6        31 May 2005            RY
 * Merged in Sirius changes.
 *
 *    #5        09 Dec 2004            JH
 * Added prototype of bt_sio_process_command() 
 *
 *    #4        12 Dec 2003            DP
 * Add background tick for BT SIO.
 * 
 *    Rev 1.4   24 Jul 2002 12:05:36   propach
 * Improved SIO API for SPP.
 * 
 *    Rev 1.3   08 Feb 2002 17:41:58   ryuen
 * Added bt_sio_process_rc_rx_data
 * 
 *    Rev 1.2   02 Mar 2001 01:25:58   ryuen
 * Renamed initialize/online/offline to
 * powerup_init/qdsp_ready_init/qdsp_reset_shutdown
 * 
 *    Rev 1.1   23 Feb 2001 12:56:54   ryuen
 * Removed obsolete function declaration bt_sio_cmd_initialize
 * 
 *    Rev 1.0   29 Jan 2001 17:10:38   propach
 * Initial revision.
 * 
 *    Rev 1.3   08 Dec 2000 10:25:42   propach
 * Added BTSIO event queue.
 * 
 *    Rev 1.2   29 Nov 2000 13:57:10   propach
 * Added NA enable/disable command support and removed FEATURE_BT_NA.
 * 
 *    Rev 1.1   22 Nov 2000 10:40:22   propach
 * Redid entire module to generalize solution for SIO over BT.

===========================================================================*/

/*===========================================================================

                        INCLUDE FILES FOR MODULE

===========================================================================*/

#include "comdef.h"
#include "queue.h"
#include "dsm.h"
#include "bt.h"
#include "bti.h"
#include "btcmdi.h"


/*===========================================================================

                           FUNCTION PROTOTYPES

===========================================================================*/

/*===========================================================================

FUNCTION
  bt_sio_powerup_init

DESCRIPTION
  Initialize the SIO module upon powerup

===========================================================================*/
extern void bt_sio_powerup_init( void );


/*===========================================================================

FUNCTION
  bt_sio_qdsp_ready_init

DESCRIPTION
  Initialize the SIO module when the Bluetooth QDSP image is loaded

===========================================================================*/
extern void bt_sio_qdsp_ready_init( void );


/*===========================================================================

FUNCTION
  bt_sio_qdsp_reset_shutdown

DESCRIPTION
  De-initialize the SIO module when the Bluetooth QDSP image is unloaded

===========================================================================*/
extern void bt_sio_qdsp_reset_shutdown( void );


/*===========================================================================

FUNCTION
  bt_sio_background_tick

DESCRIPTION
  Performs background monitoring for the BT SIO module.

===========================================================================*/
extern void bt_sio_background_tick
(
  void
);


/*===========================================================================

FUNCTION
  bt_na_process_command

DESCRIPTION
  Processes all commands intended for NA.

===========================================================================*/
extern void bt_na_process_command
(
  bt_cmd_msg_type*  na_cmd_ptr
);


/*===========================================================================

FUNCTION
  bt_sio_process_rc_rx_data

DESCRIPTION
  Processes data received by RFCOMM on a specified PID as necessary
  when it is intended for either the Network Access port or an SPP port.
  
RETURN VALUE
  dsm_ptr is set to NULL if the data was consumed by BTSIO.

===========================================================================*/
extern void bt_sio_process_rc_rx_data
(
  uint16           pid,
  dsm_item_type**  dsm_ptr_ptr
);

  
/*===========================================================================

FUNCTION
  bt_sio_process_ev_queue

DESCRIPTION
  Dequeues events on the BT SIO event queue and processes them.

===========================================================================*/
extern void bt_sio_process_ev_queue( void );


/*===========================================================================

FUNCTION
  bt_sio_process_command

DESCRIPTION
  Processes an SIO command.

===========================================================================*/
extern void bt_sio_process_command
(
  bt_cmd_msg_type*  sio_cmd_ptr
);

#if defined(FEATURE_MT_OBEX) && defined(FEATURE_PCAA_SIRIUS)
#error code not present
#endif /* defined(FEATURE_MT_OBEX) && defined(FEATURE_PCAA_SIRIUS) */

#endif /* BTSIO_H */

