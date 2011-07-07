/*====*====*====*====*====*====*====*====*====*====*====*====*====*====*====*

        B L U E T O O T H     E X T E R N A L     S O C
                M I S C     M O D U L E

GENERAL DESCRIPTION
  This module contain miscellaneous APIs for Bluetooth initialization.

EXTERNALIZED FUNCTIONS

  bt_soc_etc_init
   Sends miscellaneous HCI commands to the external SOC during
   SOC Enable driver.

  bt_soc_etc_reset_driver
    Resets the HCI layer of the driver.

  bt_soc_etc_get_info
    Gets external SOC LMP attributes.

  bt_soc_etc_proc_local_feat_ev
   Processes the response to the HCI Get Local Features command.

  bt_soc_etc_proc_local_ver_ev
   Processes the response to the HCI Get Local Version command.

  bt_soc_etc_get_rssi
    Returns RSSI value (in dBm) of the connection with the
    remote device given  its BD address.

INITIALIZATION AND SEQUENCING REQUIREMENTS

Copyright (c) 2006-2008 by QUALCOMM Incorporated. All Rights Reserved.
*====*====*====*====*====*====*====*====*====*====*====*====*====*====*====*/

/*===========================================================================

                        EDIT HISTORY FOR MODULE
 This section contains comments describing changes made to the module.
 Notice that changes are listed in reverse chronological order.

 $Header: //source/qcom/qct/wconnect/bthost/core/rel/00.00.26/hci/src/btsocetc.c#4 $
 $DateTime: 2011/02/15 00:51:03 $$Author: bneti $

  when        who  what, where, why
  ----------  ---  -----------------------------------------------------------
  2011-15-02   bn  Removed the main line fix for RSSI ,addressing locally on rel/26.
  2011-02-03   bn  Defined one variable dint came in as a part of inegration
  2011-01-13   ss  Added support for sending Inquiry events with RSSI for BT_SOC_LMP_VERSION_2_0
  2008-10-27   pg  Fixed BT_EV_RM_RADIO_ACTIVITY event logic for SOC based BT.
  2008-09-15   pg  Fixed Compiler warning.

 *
 * #23        28 Jan 2008            AP
 * Removed Lint compiler warning in bt_soc_etc_powerup_init
 *
 * #22        02 Nov 2007            BK
 * Added check for HCI mode in check for driver idle to avoid disabling
 * the driver while in HCI mode.
 *
 * #21        01 Nov 2007            AG
 * Merged Lisbon(BT2.1) changes into the mainline
 *
 * #20         19 Oct 2007            DSN\SSK
 * Add support for Controller to Host flow control.
 *
 * #19         02 Oct  2007           BK
 * Don't disable driver if test mode is enabled.  Also consolidated FEATURE_BT_SOC1_ROM
 * and FEATURE_BT_QSOC featurization in bt_soc_etc_check_driver_idle_timeout
 * and removed FEATURE_BT_SOC1_DIS_DRV_DISABLED (no required anymore).
 *
 * #18         11 Sept 2007           BK
 * Implement QSoC Driver State state machine.
 *
 * #17         23 Aug 2007            PG
 * Removed bt_soc_etc_get_rssi() stub function which is not needed
 * anymore.
 *
 * #16         20 Aug 2007           PG
 * Seperated HCI generic vendor specific event handling from Qualcomm
 * proprietary implementation used with embedded controller.
 *
 * #15         06 Jun 2007          DSN
 * Send the Enc key Length read from QSOC using vendor-specific event,
 * Write the initial class-of-device to SOC via HCI command.
 *
 * #14         12 Apr  2007         BH
 * Added FEATURE_BT_WLAN_COEXISTENCE for BT/WLAN coexistence support.
 *
 * #13        12 Apr 2007           SSK
 * Moved HCI command implementation to btcmd.c and removed warnings.
 *
 * #12        14 Mar 2007           JN
 * SoC feature based header file inclusion.
 *
 * #11        01 Mar 2007           DSN
 * Send HCI Read Buffer Size command after SOC initialization.
 *
 * #10       31 Jan 2007            ACG
 * Rename function bt_rm_check_driver_idle_timeout to
 * bt_soc_etc_check_driver_idle_timeout.
 *
 *  #9        30 Jan 2007           PG
 *  Commented out F3 log for "disable driver disabled for SURF" until
 *  BT_SLEEP_DEBUG is truned off for mainline builds.
 *
 *  #8        29 Jan 2007           PG
 *  Featurized a "disable driver" F3 log under BT_SLEEP_DEBUG.
 *
 *  #7        29 Jan 2007           DSN
 *  Additional feature to turn off driver enable/disable
 *
 *  #6        17 Jan 2007           ACG
 *  Change featurization - Replace T_FFA by FEATURE_BT_SOC1_ROM
 *
 *  #5        9 Jan 2007            ACG
 *  Make sure disable driver routine doesn't get called for BC4 based
 *  SURFs.
 *
 *  #4        8 Jan 2007            ACG
 *  Following features were added for the SOC solution:
 *  - vote for SIO sleep/wakeup based on BT activity
 *  - enable/disable driver
 *  - radio on/off
 *
 *   #3         05 Jan 2007          SSK
 *   Changed Debug message level in bt_soc_etc_get_rssi()
 *
 *   #2         14 Dec 2006           DSN
 * Update radio activity mode for SOC solution.
 *
 *   #1         14 Dec 2006          SSK
 *   Created New file for BT External SOC integration.

===========================================================================*/
/*===========================================================================

                     INCLUDE FILES FOR MODULE

===========================================================================*/

#include "customer.h"
#include "comdef.h"

#ifdef FEATURE_BT
#ifdef FEATURE_BT_SOC
#include <string.h>

#include "dsm.h"
#include "bt.h"
#include "btcmdi.h"
#include "btdebug.h"
#include "bthc.h"
#include "bthci.h"
#include "bthcpe.h"
#include "bthcdrv.h"
#include "bti.h"
#include "btlogi.h"
#include "btmsg.h"
#include "btsocetc.h"
#include "btrm.h"

#ifdef FEATURE_BT_SOC1
#error code not present
#endif /* FEATURE_BT_SOC1 */

#define BT_MSG_LAYER BT_MSG_GN   /* necessary for btmsg.h */

/*===========================================================================

             DEFINITIONS AND DECLARATIONS FOR MODULE

This section contains definitions for constants, macros, types, variables
and other items needed by this module.

===========================================================================*/

rex_timer_type                           bt_soc_driver_idle_timer;

/* Data structure for the BTSOCETC module's state information*/
bt_soc_etc_data_type                bt_soc_etc_data;

#ifdef BT_SWDEV_2_1_EIR
bt_event_mask_type                  bt_soc_event_mask;
#endif /* BT_SWDEV_2_1_EIR */


/*===========================================================================

                         FUNCTION DEFINITIONS

===========================================================================*/


/*===========================================================================

FUNCTION
  bt_soc_etc_check_driver_idle_timeout

DESCRIPTION
  This routine checks for driver activity. It disables the driver when
  insufficient activity is detected.

===========================================================================*/
void bt_soc_etc_check_driver_idle_timeout
(
  void
)
{
#if (defined( FEATURE_BT_SOC1_ROM ) || defined(FEATURE_BT_QSOC))
  // Check if any baseband activity is ongoing
  if ( ( bt_dc.driver_state == BT_DS_SOC_ACTIVE ) &&
       !bt_dc.inq_active &&
       !bt_dc.page_active &&
       !bt_dc.scan_active &&
       !bt_rm_any_conn_active() &&
       !bt_rm_test_mode_enabled() &&
       !BT_IN_HCI_MODE() )
  {
      BT_DC_UPDATE_DRIVER_STATE(BT_DS_SOC_DISABLING);

      BT_MSG_HIGH( "BT SOC ETC: Starting disable driver routine ", 0,0,0 );
      // Wait until timer expires. If no activity is detected in the meanwhile
      // it is ok to disable driver.
      rex_set_timer( &bt_soc_driver_idle_timer, BT_SOC_DRIVER_IDLE_TIMER );
  }
#elif defined(FEATURE_BT_SOC1_FLASH)
#error code not present
#else
  // For other SOCs
  // Don't trigger disable driver to begin with. Once bringup is successful
  // this can be added back.
#endif /* FEATURE_BT_SOC1_ROM || FEATURE_BT_QSOC */
}


/*==========================================================================

  FUNCTION       BT_HC_SOC_PROC_LOCAL_FEAT_EV

  DESCRIPTION    Processes the response to the HCI Get Local Features command

  DEPENDENCIES   None.

  PARAMETERS     ev_msg_ptr  : Pointer to the event structure.

  RETURN VALUE   None

  ==========================================================================*/
void bt_soc_etc_proc_local_feat_ev
(
  const bt_ev_msg_type*  ev_msg_ptr
)
{

/*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  BT_MSG_HIGH( "BT HC SOC ETC: Got RD_REMOTE_SUPPORTED_FEAT response",
                0, 0, 0 );

  if ( ev_msg_ptr->ev_msg.ev_hc_cmd_cmplt.ret_params.rd_local_feat.status ==
       BT_BE_SUCCESS )
  {
    bt_soc_etc_data.lmp_features =
      ev_msg_ptr->ev_msg.ev_hc_cmd_cmplt.ret_params.rd_local_feat.lmp_features;
  }
  else
  {
    /* Failure to get Local Features */
    BT_ERR( "BT HC SOC ETC: HCI local feature request failed error: %lX",
             ev_msg_ptr->ev_msg.ev_hc_cmd_cmplt.ret_params.status.status,
             0,
             0 );
  }
} /* bt_soc_etc_proc_local_feat_ev */


#ifdef FEATURE_BT_2_1
/*==========================================================================

  FUNCTION      BT_SOC_ETC_VERSION_INIT

  DESCRIPTION   Initializes the SOC based on the LMP version it supports

  DEPENDENCIES  None.

  PARAMETERS    None.

  RETURN VALUE  None.

  SIDE EFFECTS  This function should be called only after the SOC version
                has been read

==========================================================================*/
void bt_soc_etc_version_init( void )
{
    if( bt_soc_etc_data.lmp_version >= BT_SOC_LMP_VERSION_2_1 )
    {
      /* Set Controller to send us extended inquiry responses */
      bt_cmd_hc_wr_inq_mode( BT_INQ_RESULT_EXTENDED );

      (void) bt_cmd_hc_wr_simple_pairing_mode( TRUE );

      bt_cmd_hc_rd_inq_rsp_tx_pwr();
    }
    else
    {
		bt_cmd_hc_wr_inq_mode(BT_INQ_RESULT_WITH_RSSI);
	}
}
#endif /* FEATURE_BT_2_1 */


/*==========================================================================

  FUNCTION       BT_HC_SOC_PROC_LOCAL_VER_EV

  DESCRIPTION    Processes the response to the HCI Get Local Version command.

  DEPENDENCIES   None.

  PARAMETERS     ev_msg_ptr  : Pointer to the event structure.

  RETURN VALUE   None

  ==========================================================================*/
void bt_soc_etc_proc_local_ver_ev
(
  const bt_ev_msg_type*  ev_msg_ptr
)
{

/*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  BT_MSG_HIGH( "BT HC SOC ETC: Got RD_REMOTE_SUPPORTED_FEAT response",
                0, 0, 0 );

  if ( ev_msg_ptr->ev_msg.ev_hc_cmd_cmplt.ret_params.rd_local_ver.status ==
       BT_BE_SUCCESS )
  {
    bt_soc_etc_data.lmp_version =
      ev_msg_ptr->ev_msg.ev_hc_cmd_cmplt.ret_params.rd_local_ver.lmp_version;

    bt_soc_etc_data.lmp_subversion =
      ev_msg_ptr->ev_msg.ev_hc_cmd_cmplt.ret_params.rd_local_ver.lmp_subversion;

    bt_soc_etc_data.manufacturer_name =
      ev_msg_ptr->ev_msg.ev_hc_cmd_cmplt.ret_params.rd_local_ver.manufacturer_name;

    bt_soc_etc_data.hci_version =
      ev_msg_ptr->ev_msg.ev_hc_cmd_cmplt.ret_params.rd_local_ver.hci_version;

    bt_soc_etc_data.hci_revision =
      ev_msg_ptr->ev_msg.ev_hc_cmd_cmplt.ret_params.rd_local_ver.hci_revision;

#ifdef FEATURE_BT_2_1
    bt_soc_etc_version_init();
#endif /* FEATURE_BT_2_1 */
  }
  else
  {
    /* Failure to get Local Version */
    BT_ERR( "BT HC SOC ETC: HCI local feature request failed error: %lX",
             ev_msg_ptr->ev_msg.ev_hc_cmd_cmplt.ret_params.status.status,
             0,
             0 );
  }
} /* bt_soc_etc_proc_local_ver_ev */


/*===========================================================================

  FUNCTION        BT_HC_SOC_RESET_DRIVER

  DESCRIPTION     Resets the HCI layer of the driver.

  DEPENDENCIES    The SOC DRIVER should be enabled for this command
                  to execute successfully.

  PARAMETERS      None.

  RETURN VALUE    None.

  SIDE EFFECTS    None.

===========================================================================*/
void bt_soc_etc_reset_driver
(
  void
)
{
  bt_cmd_msg_type*   bt_cmd_ptr;

/*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  bt_hc_pe_init();

  bt_cmd_ptr = bt_get_free_cmd();

  if ( bt_cmd_ptr != NULL )
  {
    bt_initialize_cmd_buffer( bt_cmd_ptr );

    BT_MSG_HIGH( "BT HC SOC ETC: Sending HCI RESET ", 0, 0, 0 );
    bt_cmd_ptr->cmd_hdr.cmd_type    = BT_CMD_HC_RESET;
    bt_cmd_ptr->cmd_hdr.bt_app_id   = bt_dc_last_hci_ev_reg_app_id;

    /* Sending out HCI Reset command   */
    bt_cmd( bt_cmd_ptr );
  }

} /* bt_soc_etc_reset_driver */


/*===========================================================================

  FUNCTION     BT_HC_SOC_GET_INFO

  DESCRIPTION  Gets external SOC LMP attributes

  DEPENDENCIES None.

  PARAMETERS   None.

  RETURN VALUE None.

  SIDE EFFECTS This function should be called only after btsoc1_init() has
               finished.

===========================================================================*/
bt_cmd_status_type bt_soc_etc_get_info
(
  bt_version_type*       version_ptr,
  bt_lmp_features_type*  lmp_features_ptr
)
{

/*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  BT_MSG_HIGH( "BT HC SOC ETC: bt_soc_etc_get_info called ", 0, 0, 0 );

  if ( version_ptr != NULL )
  {
    version_ptr->lmp_version       = bt_soc_etc_data.lmp_version;
    version_ptr->manufacturer_name = bt_soc_etc_data.manufacturer_name;
    version_ptr->lmp_subversion    = bt_soc_etc_data.lmp_subversion;
  }

  if ( lmp_features_ptr != NULL )
  {
    *lmp_features_ptr              = bt_soc_etc_data.lmp_features;
  }

  return( BT_CS_GN_SUCCESS );
} /* bt_soc_etc_get_info */


/*=====================================================================
                 Host Controller Interface (HCI) Commands
======================================================================*/

/*===========================================================================

  FUNCTION         BT_SOC_ETC_CMD_HC_WR_ENC_KEY_LEN

  DESCRIPTION      This function sends an HC write encryption key length
                   command to the  Bluetooth driver.

  DEPENDENCIES     None.

  PARAMETERS       min_enc_key_size: Minimum encryption key length.
                   max_enc_key_size: Maximum encryption key length.

  RETURN VALUE     None.

  SIDE EFFECTS     None.

===========================================================================*/
bt_cmd_status_type bt_soc_etc_cmd_hc_wr_enc_key_len
(
  uint8  min_enc_key_size,
  uint8  max_enc_key_size
)
{
  bt_cmd_status_type  cmd_status = BT_CS_GN_CMD_Q_FULL;

/*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  BT_MSG_HIGH( "BT HC SOC ETC: Writing Max-Min enc key lengths to SOC ",
               0, 0, 0 );

  /*TBD*/

 /* Nothing to be done for BC4 SOC now as the Initial PSKEYs take care of this.
    For other SOCs relevant code need to be added here */

  return( cmd_status );

} /* bt_soc_etc_cmd_hc_wr_enc_key_len */


/*=====================================================================
                 Host Controller Interface (HCI) Events
======================================================================*/

/*===========================================================================

FUNCTION
  bt_soc_etc_send_cur_conn_enc_ev

DESCRIPTION
  Sends an HC QC Current Connection Encryption event.

===========================================================================*/
void bt_soc_etc_send_cur_conn_enc_ev
(
  uint8   enc_key_len,
  uint16  conn_hndl
)
{

  bt_ev_msg_type                      hc_ev;
  bt_ev_hc_qc_cur_conn_encrypt_type*  cce_ptr;


  BT_MSG_API( "BT SOC ETC TX: Cur Conn Enc H %x EKL %x",
              conn_hndl, enc_key_len, 0 );

  hc_ev.ev_hdr.ev_type           = BT_EV_HC_CUR_CONN_ENCRYPT;

  cce_ptr = &hc_ev.ev_msg.ev_hc_ccenc;

  cce_ptr->conn_hndl             = conn_hndl;
  cce_ptr->encrypt_enable        = BT_ENCRYPT_ENABLE_ON;
  cce_ptr->cur_enc_key_len_bytes = enc_key_len;

  bt_hc_send_event( &hc_ev );

} /* bt_soc_etc_send_cur_conn_enc_ev */

/*=====================================================================
                         Init/Cleanup Functions
======================================================================*/

/*==========================================================================

  FUNCTION      BT_HC_SOC_POWERUP_INIT

  DESCRIPTION   Initializes SOC specific data.

  DEPENDENCIES  None.

  PARAMETERS    None.

  RETURN VALUE  None.

  SIDE EFFECTS  None.

==========================================================================*/
void bt_soc_etc_powerup_init( void )
{

#if (defined FEATURE_BT_2_1 || defined FEATURE_BT_WLAN_COEXISTENCE)
  uint8              i;
#endif /* (FEATURE_BT_2_1 || FEATURE_BT_WLAN_COEXISTENCE) */

#ifdef FEATURE_BT_2_1
  for(i=0; i<BT_EVENT_MASK_LEN; i++)
  {
    bt_soc_event_mask.bytes[ i ] = 0xFF;
  }
#endif /* FEATURE_BT_2_1 */

#ifdef FEATURE_BT_WLAN_COEXISTENCE
  {

    for ( i = 0; i < 10; i++ )
    {
      /* Initialize channel map with all channels set to "unknown" */
      /* 1==> channel indicated by bit position is unknown */
      bt_soc_etc_data.host_ch_class.bytes[ i ] = 0xFF;
    }

    /* bit 79 of 80 bit mask is reserved and should be set to 0 */
    bt_soc_etc_data.host_ch_class.bytes[ 9 ] = 0x7F;
  }
#endif /* FEATURE_BT_WLAN_COEXISTENCE */
}

/*==========================================================================

  FUNCTION      BT_SOC_ETC_INIT

  DESCRIPTION   Sends miscellaneous HCI commands to the external SOC.

  DEPENDENCIES  None.

  PARAMETERS    None.

  RETURN VALUE  None.

  SIDE EFFECTS  This function should be called only after the SOC DRIVER has
                been enabled.

==========================================================================*/
void bt_soc_etc_init( void )
{
  dsm_item_type*      dsm_ptr;

/*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  BT_MSG_HIGH( "BT SOC ETC: bt_soc_etc_init called ", 0, 0, 0 );

  /* SOC Driver should be enabled now and HCI commands can be sent  */

  /* Command to read the HCI buffer sizes */
  (void) bt_cmd_hc_rd_buf_size();
  BT_MSG_HIGH( "BT SOC ETC: Read Buffer Size cmd sent", 0, 0, 0 );

#ifdef FEATURE_BT_HCI_HOST_FCTL
  (void) bt_cmd_hc_host_buf_size();
    BT_MSG_HIGH( "BT SOC ETC: Host Buffer Size cmd sent", 0, 0, 0 );

  (void) bt_cmd_hc_set_hc_to_host_fc();
    BT_MSG_HIGH( "BT SOC ETC: Set Con2Host Flow Control cmd sent", 0, 0, 0 );
#endif /* FEATURE_BT_HCI_HOST_FCTL */

  /* Command to read the device features from external SOC */
  (void) bt_cmd_hc_rd_local_feat();

    /* Command to read the Local Version info from external SOC */
  (void) bt_cmd_hc_rd_local_ver();

    /* Write the initial class of device */
  (void)bt_cmd_hc_wr_class_of_device( bt_class_of_device );

    /* Write the Device Name into the external SOC */
    BT_MSG_HIGH( "BT HC SOC ETC: HC Init Loc Name from EFS", 0, 0, 0 );

  dsm_ptr = bt_get_free_dsm_ptr( BT_TL_HCI_BB,
                                 BT_MAX_LOCAL_DEVICE_NAME_LEN );

  bt_dsm_pushdown_tail( &dsm_ptr,
                        (void *)(bt_efs_params.bt_name),
                        BT_MAX_LOCAL_DEVICE_NAME_LEN,
                        DSM_DS_SMALL_ITEM_POOL );

    /* The event response to this command need not be processed */
  (void) bt_cmd_hc_change_local_name( dsm_ptr );

#ifdef FEATURE_BT_WLAN_COEXISTENCE
  (void) bt_cmd_hc_afh_host_chan_class ( bt_soc_etc_data.host_ch_class );
#endif /* FEATURE_BT_WLAN_COEXISTENCE */

#ifdef FEATURE_BT_2_1
    /* Set the event mask */
  bt_cmd_hc_set_event_mask( bt_soc_event_mask );
#endif /* FEATURE_BT_2_1 */

} /* bt_soc_etc_init */

#endif /* FEATURE_BT_SOC */
#endif /* FEATURE_BT */
