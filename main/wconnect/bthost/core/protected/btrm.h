#ifndef _BTRM_H
#define _BTRM_H

#ifdef FEATURE_BT

/*====*====*====*====*====*====*====*====*====*====*====*====*====*====*====*

          B L U E T O O T H    R A D I O    M A N A G E R

DESCRIPTION
  This file contains declarations for the Bluetooth Radio Manager.

Copyright (c) 2000 - 2007 by QUALCOMM Incorporated. All Rights Reserved.
*====*====*====*====*====*====*====*====*====*====*====*====*====*====*====*/

/*===========================================================================

                        EDIT HISTORY FOR MODULE

  This section contains comments describing changes made to the module.
  Notice that changes are listed in reverse chronological order.

$PVCSHeader:   O:/src/asw/COMMON/vcs/btrm.h_v   1.9   03 Apr 2002 14:25:48   propach
$Header: //source/qcom/qct/wconnect/bthost/core/rel/00.00.26/protected/btrm.h#1 $ $DateTime:

$Log:   O:/src/asw/COMMON/vcs/btrm.h_v  $
 *
 *    #16       01 Nov 2007            AG
 * Merged Lisbon(BT2.1) changes into the mainline
 *
 *    #15        01 Nov 2007            PG
 * Added bt_rm_pending_acl_tx_lpm_check() function prototype.
 *
 *    #14        02 Oct  2007           BK
 * Added bt_rm_test_mode_enabled() function prototype.
 * 
 *    #13        03 Jul 2007            SSK
 * Moving the bt_rm_disable_radio() API out of Feature T_MSM6250 to make
 * the API available to other targets.
 *
 *    #12        20 Feb 2007            DSN
 * Increasing the maximum number of events in RM layer for SOC solutions.
  *
 *    #11        8 Jan 2007            ACG
 * Following features were added for the SOC solution:
 * - vote for SIO sleep/wakeup based on BT activity
 * - enable/disable driver
 * - radio on/off
 * 
 *    #10        03 Aug 2005            GS
 * Mainlined mDSP reset handling functionality for UMTS targets.  
 *
 *    #9         06 May 2005            GS
 * Added extern function bt_rm_disable_radio()
 *
 *    #8         06 Apr 2005            JH
 * Adding support for disabling BT radio, featurized using BT_SWDEV_RADIO_OFF.
 *
 *    #7         29 Jan 2004            DP
 * Added remote name request to connection setup sequence.  Also added
 * this comment, thus 2 revisions.
 *
 *    #5         4  Nov 2003            DP
 * Added bt_rm_efs_init().
 *
 *    #4         3  Oct 2003            DP
 * Converted RM timer to background timer.
 * 
 *    Rev 1.9   03 Apr 2002 14:25:48   propach
 * Added event handler.
 * 
===========================================================================*/


#ifdef FEATURE_BT_SOC
  #define BT_RM_NUM_EVENTS   100
#else
  #define BT_RM_NUM_EVENTS   40
#endif /* FEATURE_BT_SOC */

extern bt_app_id_type  bt_rm_app_id;

/*===========================================================================

FUNCTION
  bt_rm_pending_acl_tx_lpm_check

DESCRIPTION
  This function checks low power mode state of the link identified by 
  hc_handle and initiates exit from specific low power modes. This function
  is called when num_complete ack has not been received from the controller
  for a "timeout" period for acl data that has been sent to the controller.
  Current implementation exits park state only. 

===========================================================================*/

extern void bt_rm_pending_acl_tx_lpm_check 
( 
  uint16 hc_handle
);

/*===========================================================================

FUNCTION
  bt_rm_any_conn_active

DESCRIPTION
  Checks for active connections. Returns TRUE if any ACL links are up.

===========================================================================*/
extern boolean bt_rm_any_conn_active ( void );

/*===========================================================================

FUNCTION
  bt_rm_process_command

DESCRIPTION
  Processes a Radio Manager command.

===========================================================================*/
extern void bt_rm_process_command
(
  bt_cmd_msg_type*  rm_cmd_ptr
);


/*===========================================================================

FUNCTION
  bt_rm_process_ev_queue

DESCRIPTION
  Dequeues events on the BT RM event queue and processes them.

===========================================================================*/
extern void bt_rm_process_ev_queue( void );


/*===========================================================================

FUNCTION
  bt_rm_store_bt_event

DESCRIPTION
  Stores a specified BT event on the RM BT event queue.

===========================================================================*/
extern void bt_rm_store_bt_event
(
  bt_ev_msg_type*  bt_ev_ptr
);


/*===========================================================================

FUNCTION
  bt_rm_powerup_init

DESCRIPTION
  Initialize the Radio Manager Module upon powerup

===========================================================================*/
extern void bt_rm_powerup_init( void );


/*===========================================================================

FUNCTION
  bt_rm_efs_init

DESCRIPTION
  Initialize device and service information in RAM using data stored
  in flash file system, if present, or it will initialize those
  files in EFS as necessary.

===========================================================================*/
extern void bt_rm_efs_init( void );


#ifdef BT_SWDEV_2_1_EIR
#ifdef BT_SWDEV_2_1_NV
/*===========================================================================

FUNCTION
  bt_rm_nv_init

DESCRIPTION

===========================================================================*/
extern void bt_rm_nv_init( void );
#endif /* BT_SWDEV_2_1_NV */
#endif /* BT_SWDEV_2_1_EIR */


//Add By zzg 2011_02_25
extern boolean bt_save_efs_params(void);
extern void bt_rm_refresh_efs_param(void);
//Add End

/*===========================================================================

FUNCTION
  bt_rm_qdsp_ready_init

DESCRIPTION
  Initialize the Radio Manager Module when the Bluetooth QDSP image is
  loaded.

===========================================================================*/
extern void bt_rm_qdsp_ready_init( void );


/*===========================================================================

FUNCTION
  bt_rm_qdsp_reset_shutdown

DESCRIPTION
  De-initialize the Radio Manager Module when the Bluetooth QDSP image is
  unloaded.

===========================================================================*/
extern void bt_rm_qdsp_reset_shutdown( void );


/*===========================================================================

FUNCTION
  bt_rm_background_tick

DESCRIPTION
  Performs background monitoring for RM.

===========================================================================*/
extern void bt_rm_background_tick
(
  void
);


/*===========================================================================

FUNCTION
  bt_rm_deliver_remote_device_name

DESCRIPTION
  Provides the device name of a remote device to the RM.

===========================================================================*/
extern void bt_rm_deliver_remote_device_name
(
  bt_bd_addr_type*  bd_addr_ptr,
  char*             name_str
);


#ifdef BT_TEST_PYLD_FAST_RX
/*===========================================================================

FUNCTION
  bt_rm_ev_hc_rx_acl_data

DESCRIPTION
  Processes a HC Receive ACL Data event.

===========================================================================*/
extern void bt_rm_ev_hc_rx_acl_data
(
  bt_ev_msg_type*  ev_msg_ptr
);
#endif /* BT_TEST_PYLD_FAST_RX */


#ifdef BT_TEST_PYLD_FAST_TX
/*===========================================================================

FUNCTION
  bt_rm_cmd_tx_acl_data

DESCRIPTION
  Processes a RM Transmit ACL Data command.

===========================================================================*/
extern void bt_rm_cmd_tx_data_acl
(
  bt_cmd_msg_type*  rm_tx_acl_ptr
);
#endif /* BT_TEST_PYLD_FAST_TX */

#ifdef BT_SWDEV_RADIO_OFF
/*===========================================================================

FUNCTION
  bt_rm_radio_disabled

DESCRIPTION
  Performs post-processing after BT radio has been turned off.

===========================================================================*/
extern void bt_rm_radio_disabled
(
  void
);
#endif /* BT_SWDEV_RADIO_OFF */

/*===========================================================================

FUNCTION
  bt_rm_disable_radio

DESCRIPTION
  Processes a RM Disable Radio command.  This API also exposes the routine 
  for mDSP image swap handling on UMTS targets.

===========================================================================*/
extern void bt_rm_disable_radio
(
  bt_cmd_msg_type*  rm_dr_ptr
);

/*===========================================================================

FUNCTION
  bt_rm_test_mode_enabled

DESCRIPTION
  Returns the state of test mode.

RETURN VALUES
  TRUE if test mode is enabled, FALSE if not in test mode.

===========================================================================*/
boolean bt_rm_test_mode_enabled
(
  void
);

#endif /* FEATURE_BT */

#endif /* _BTRM_H */
