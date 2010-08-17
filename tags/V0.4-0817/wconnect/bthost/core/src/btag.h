#ifndef _BTAG_H
#define _BTAG_H

#ifdef FEATURE_BT

/*====*====*====*====*====*====*====*====*====*====*====*====*====*====*====*

            B L U E T O O T H    R F C O M M    M O D U L E

DESCRIPTION
  This module contains the audio gateway application (per the headset
  profile).

Copyright (c) 2000-2008 QUALCOMM Incorporated.
All Rights Reserved.
Qualcomm Confidential and Proprietary
*====*====*====*====*====*====*====*====*====*====*====*====*====*====*====*/

/*===========================================================================

                        EDIT HISTORY FOR MODULE

  This section contains comments describing changes made to the module.
  Notice that changes are listed in reverse chronological order.

  $Header: //source/qcom/qct/wconnect/bthost/core/rel/00.00.26/src/btag.h#1 $
  $DateTime: 2009/01/07 18:14:54 $
  $Author: deepikas $

  when        who  what, where, why
  ----------  ---  -----------------------------------------------------------
  2008-03-18   co  Removed inclusion of btagi.h from now on.

 *
 *    #5         8  Apr 2004            PN
 * Added bt_ag_background_tick().
 *
 *    Rev 1.6   30 Aug 2001 14:24:14   balas
 * Removed interface to directly set/get Headset SCN.
 *
 *    Rev 1.5   13 Jun 2001 11:04:48   balas
 * Added comments and new header file.
 *
 *    Rev 1.4   28 Mar 2001 19:18:44   balas
 * Support for HS originated ACL connection.
 *
 *    Rev 1.3   02 Mar 2001 17:33:06   balas
 * Added function prototypes to get/set headset server channel.
 *
 *    Rev 1.2   02 Mar 2001 00:31:38   ryuen
 * Renamed bt_xx_offline to bt_xx_qdsp_reset_shutdown
 *
 *    Rev 1.1   23 Feb 2001 12:48:08   ryuen
 * Renamed bt_ag_init and bt_ag_online to bt_ag_powerup_init and
 * bt_ag_qdsp_ready_init respectively
 *
 *    Rev 1.0   29 Jan 2001 17:05:34   propach
 * Initial revision.
 *
 *    Rev 1.4   29 Nov 2000 15:51:44   amitj
 * Declared bt_ag_snapshot_logging_function()
 *
 *    Rev 1.3   14 Sep 2000 19:01:24   ryuen
 * Added bt_ag_online() and bt_ag_offline()
 *
 *    Rev 1.2   06 Sep 2000 10:34:58   propach
 * Moved SIO setup to DC.  Merged AG/HS.
 *
 *    Rev 1.1   19 Jul 2000 17:25:00   propach
 * Update BT API and featurization with cleanup.
 *
 *    Rev 1.0   04 Jul 2000 19:18:14   ryuen
 * Initial revision.

===========================================================================*/

/*===========================================================================

                     INCLUDE FILES FOR MODULE

===========================================================================*/

#include "bt.h"

/*===========================================================================

                DEFINITIONS AND DECLARATIONS FOR MODULE

This section contains definitions for constants, macros, types, variables
and other items needed by this module.

===========================================================================*/

/*-------------------------------------------------------------------------*/
/*                                                                         */
/*                         Data Type Definitions                           */
/*                                                                         */
/*-------------------------------------------------------------------------*/


/*-------------------------------------------------------------------------*/
/*                                                                         */
/*                       Global Data Declarations                          */
/*                                                                         */
/*-------------------------------------------------------------------------*/

/*-------------------------------------------------------------------------*/
/*                                                                         */
/*                           External Macros                               */
/*                                                                         */
/*-------------------------------------------------------------------------*/

/*-------------------------------------------------------------------------*/
/*                                                                         */
/*                    External Functions Prototypes                        */
/*                                                                         */
/*-------------------------------------------------------------------------*/

/*===========================================================================

FUNCTION
  bt_ag_powerup_init

DESCRIPTION
  Initialize the Audio Gateway layer upon powerup

===========================================================================*/
extern void bt_ag_powerup_init( void );


/*===========================================================================

FUNCTION
  bt_ag_qdsp_ready_init

DESCRIPTION
  Initialize the Audio Gateway layer when the Bluetooth QDSP image is loaded

===========================================================================*/
extern void bt_ag_qdsp_ready_init( void );


/*===========================================================================

FUNCTION
  bt_ag_qdsp_reset_shutdown

DESCRIPTION
  De-initialize the Audio Gateway layer when the Bluetooth QDSP image is
  unloaded

===========================================================================*/
extern void bt_ag_qdsp_reset_shutdown( void );


/*===========================================================================

FUNCTION
  bt_ag_process_command

DESCRIPTION
  Processes all commands intended for Audio Gateway.

===========================================================================*/
extern void bt_ag_process_command( bt_cmd_msg_type*  ag_cmd_ptr );


/*===========================================================================

FUNCTION
  bt_ag_process_ev_queue

DESCRIPTION
  Dequeues events on the Audio Gateway event queue and processes them.

===========================================================================*/
extern void bt_ag_process_ev_queue( void );


/*===========================================================================

FUNCTION
  bt_ag_timer_expired

DESCRIPTION
  Called everytime AG timer expired.

===========================================================================*/
extern void bt_ag_timer_expired( void );

/*===========================================================================

FUNCTION
  bt_ag_background_tick

DESCRIPTION
  Performs background monitoring for AG.

===========================================================================*/
extern void bt_ag_background_tick( void );

/*===========================================================================

FUNCTION
  bt_ag_check_rx_wm_data

DESCRIPTION
  Processes data on the AG RX watermark.

===========================================================================*/
extern void bt_ag_check_rx_wm_data( void );

#ifdef BT_SWDEV_QXDM_LOG_PER
/*===========================================================================

FUNCTION
  bt_ag_snapshot_logging_function

DESCRIPTION
  Logs all the Audio Gateway specific statistics through QXDM.

===========================================================================*/
extern void bt_ag_snapshot_logging_function( void );
#endif

#endif /* FEATURE_BT */

#endif  /*  _BTAG_H  */
