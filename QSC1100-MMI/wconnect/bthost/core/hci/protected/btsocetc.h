#ifndef _BTSOCETC_H
#define _BTSOCETC_H

#ifdef FEATURE_BT
#ifdef FEATURE_BT_SOC


/*====*====*====*====*====*====*====*====*====*====*====*====*====*====*====*

    B L U E T O O T H   BC4   D R I V E R   M O D U L E

GENERAL DESCRIPTION
  This module contains the Bluetooth BC4 CSR driver data and code.

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

 $Header: //source/qcom/qct/wconnect/bthost/core/rel/00.00.26/hci/protected/btsocetc.h#1 $
 $DateTime: 2009/01/07 18:14:54 $$Author: deepikas $
  when        who  what, where, why
  ----------  ---  -----------------------------------------------------------
  2008-10-27   pg  Fixed BT_EV_RM_RADIO_ACTIVITY event logic for SOC based BT.
  2008-08-21   pg  Moved BT_LMP_FEATURE_SUPPORTED macro defintion to bt.h. This
                   macro is helpful to users of the new bt_cmd_rm for read remote 
                   features.

 *   #10      01 Nov 2007            AG
 *   Merged Lisbon(BT2.1) changes into the mainline
 *
 *   #9       23 Aug 2007            PG
 *   Removed bt_soc_etc_get_rssi() stub function which is not needed
 *   anymore.
 *
 *   #8       06 Jun 2007          DSN
 *   Send the Enc key Length read from QSOC using vendor-specific event.
 *
 *   #7       12 Apr 2007            BH
 *   Added FEATURE_BT_WLAN_COEXISTENCE for BT/WLAN coexistence 
 *   support.   
 *
 *   #6       12 Feb 2007            SSK
 *   Declare prototype in header file for  externalized function
 *   bt_soc_etc_cmd_hc_wr_enc_key_len().
 *
 *   #5       07 Feb 2007            ACG
 *   Increase driver idle timeout to 5 minutes.
 *
 *   #4       31 Jan 2007            ACG
 *   Rename function bt_rm_check_driver_idle_timeout to 
 *   bt_soc_etc_check_driver_idle_timeout.
 *
 *   #3        8 Jan 2007            ACG
 *   Following features were added for the SOC solution:
 *   - vote for SIO sleep/wakeup based on BT activity
 *   - enable/disable driver
 *   - radio on/off
 *
 *   #2         14 Dec 2006          DSN
 *   Update radio activity mode for SOC solution.
 *
 *   #1         14 Dec 2006          SSK
 *   Created New file for BT External SOC integration

===========================================================================*/
/*===========================================================================

                     INCLUDE FILES FOR MODULE

===========================================================================*/



/*===========================================================================

             DEFINITIONS AND DECLARATIONS FOR MODULE

This section contains definitions for constants, macros, types, variables
and other items needed by this module.

===========================================================================*/

/*=========================================================================*/
/*                                   Constants                             */
/*=========================================================================*/
#define BT_SOC_DRIVER_IDLE_TIMER 300000 //ms

/*=========================================================================*/
/*                                   Typedefs                              */
/*=========================================================================*/

typedef struct
{
  uint8                hci_version;
  uint16               hci_revision;
  uint8                lmp_version;
  uint16               manufacturer_name;
  uint16               lmp_subversion;
  bt_lmp_features_type lmp_features;
#ifdef  FEATURE_BT_WLAN_COEXISTENCE
  bt_afh_ch_map_type   host_ch_class;
#endif /* FEATURE_BT_WLAN_COEXISTENCE */
} bt_soc_etc_data_type;

extern rex_timer_type                           bt_soc_driver_idle_timer;
extern bt_soc_etc_data_type                     bt_soc_etc_data;


/*===========================================================================

                         FUNCTION DEFINITIONS

===========================================================================*/


/*-------------------------------------------------------------------------*/
/*                                                                         */
/*             Extern Function Declarations for BTSOCETC module            */
/*                                                                         */
/*-------------------------------------------------------------------------*/

/*===========================================================================

FUNCTION
  bt_soc_etc_check_driver_idle_timeout

DESCRIPTION
  This routine checks for driver activity. It disables the driver when
  insufficient activity is detected.

===========================================================================*/
extern void bt_soc_etc_check_driver_idle_timeout ( void );

/*==========================================================================

  FUNCTION      BT_HC_SOC_INIT

  DESCRIPTION   Sends miscellaneous HCI commands to the external SOC.

  DEPENDENCIES  None.

  PARAMETERS    None.

  RETURN VALUE  None.

  SIDE EFFECTS  This function should be called only after the SOC DRIVER has
                been enabled.
==========================================================================*/
extern void bt_soc_etc_init( void );


/*===========================================================================

  FUNCTION        BT_HC_SOC_RESET_DRIVER

  DEPENDENCIES    The SOC DRIVER should be enabled for this command
                  to execute successfully.

  PARAMETERS      None.

  RETURN VALUE    None.

  SIDE EFFECTS    None.

  DESCRIPTION     Resets the HCI layer of the driver.

===========================================================================*/
extern void bt_soc_etc_reset_driver( void );


/*===========================================================================

  FUNCTION     BT_HC_SOC_GET_INFO

  DESCRIPTION  Send

  DEPENDENCIES None.

  PARAMETERS   None.

  RETURN VALUE None.

  SIDE EFFECTS This function should be called only after btsoc1_init() has
               finished.

===========================================================================*/
extern bt_cmd_status_type bt_soc_etc_get_info
(
  bt_version_type*       version_ptr,
  bt_lmp_features_type*  lmp_features_ptr
);


/*==========================================================================

  FUNCTION       BT_HC_SOC_PROC_LOCAL_FEAT_EV

  DESCRIPTION    Process the get feature response event

  DEPENDENCIES   None.

  PARAMETERS     e0v_msg_ptr  : Pointer to the event structure.

  RETURN VALUE   None

 ==========================================================================*/
extern void bt_soc_etc_proc_local_feat_ev
(
  const bt_ev_msg_type*  ev_msg_ptr
);


/*==========================================================================

  FUNCTION       BT_HC_SOC_PROC_LOCAL_VER_EV

  DESCRIPTION    Processes the response to the HCI Get Local Features command

  DEPENDENCIES   None.

  PARAMETERS     ev_msg_ptr  : Pointer to the event structure.

  RETURN VALUE   None

  ==========================================================================*/
extern void bt_soc_etc_proc_local_ver_ev
(
  const bt_ev_msg_type*  ev_msg_ptr
);

/*==========================================================================

  FUNCTION      BT_HC_SOC_POWERUP_INIT

  DESCRIPTION   Initializes SOC specific data.

  DEPENDENCIES  None.

  PARAMETERS    None.

  RETURN VALUE  None.

  SIDE EFFECTS  None.

==========================================================================*/
extern void bt_soc_etc_powerup_init( void );


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
extern bt_cmd_status_type bt_soc_etc_cmd_hc_wr_enc_key_len
(
  uint8  min_enc_key_size,
  uint8  max_enc_key_size
);

/*===========================================================================

FUNCTION
  bt_soc_etc_send_cur_conn_enc_ev

DESCRIPTION
  Sends an HC QC Current Connection Encryption event.

===========================================================================*/
extern void bt_soc_etc_send_cur_conn_enc_ev
(
  uint8   enc_key_len,
  uint16  conn_hndl
);


#endif /* FEATURE_BT_SOC */
#endif /* FEATURE_BT */
#endif /* _BTSOCETC_H */
