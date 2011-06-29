/*====*====*====*====*====*====*====*====*====*====*====*====*====*====*====*


               B L U E T O O T H    D R I V E R    Q U E U E D

                     C O M M A N D    I N T E R F A C E

GENERAL DESCRIPTION
  This module contains the routines to queue up commands the
  Bluetooth driver.

              Copyright (c) 2000-2009 QUALCOMM Incorporated.               
                      All Rights Reserved.                                  
              Qualcomm Confidential and Proprietary                        
*====*====*====*====*====*====*====*====*====*====*====*====*====*====*====*/


/*===========================================================================

                        EDIT HISTORY FOR MODULE

  This section contains comments describing changes made to the module.
  Notice that changes are listed in reverse chronological order.

$PVCSHeader:   O:/src/asw/COMMON/vcs/btcmd.c_v   1.31   05 Aug 2002 17:59:02   ryuen
$Header: //source/qcom/qct/wconnect/bthost/core/rel/00.00.26/src/btcmd.c#3 $ $DateTime:

$Log:   O:/src/asw/COMMON/vcs/btcmd.c_v  $

  when        who  what, where, why
  ----------  ---  -----------------------------------------------------------
  2009-05-12   pg  Added bt_cmd_rm_register_link_status_ext() API.  
  2009-02-18   hm  Created an API for Bond Cancel
  2008-11-05   pg  Generic handling for HCI Vendor Specific cmd group.
  2008-09-15   pg  Fixed Compiler warning.
  2008-08-30   pg  Added HCI cmd to read clock offset
  2008-08-14   rb  Added support for FEATURE_BT_DCVS
  2008-05-21   ag  Added support for setting device security for JSR82 when 2.1
                   is enabled.
  2008-03-24   vk  Added support for HCI Read/Write Defalut Erroneous Data 
                   Reporting mode command.
                   
 *
 *   #92        06 Mar 2008            AG
 * Added error check for passkey in bt_cmd_rm_passkey_reply
 *
 *   #91        29 Feb 2008            AG
 * Renamed BT_CMD_HC_IO_CAPABILITY_RESPONSE to BT_CMD_HC_IO_CAPABILITY_RQST_REPLY
 * for consistency with spec.
 * Added new status code for deprecated functions.
 * Placed stdlib inclusion outside featurization.
 * 
 *   #90        07 Feb 2008            RS
 * The UUID for 128 is now included in the search pattern for SS requests 
 * thereby getting initialized to the value sent by the application.
 *
 *   #89        14 Dec 2007            VK
 * Defined bt_cmd_hc_wr_link_supervision_timeout function.
 *
 *   #88        12 Dec 2007            HM
 * Added SD extended command to register custom services 
 *
 *   #87        29 Nov 2007            HM
 * Added SD command to return service search results in XML format
 * Added command to cancel service search. 
 * Added a parameter to specify duration of a device search. 
 *
 *   #86        23 Nov 2007            PN
 * Used correct command status code for set_device_black_list() and 
 * set_device_white_list().
 *
 *   #85        07 Nov 2007            GS
 * Added bt_cmd_sd_cancel_idle_timer().
 *
 *   #84        01 Nov 2007            AG
 * Merged Lisbon(BT2.1) changes into the mainline
 *
 *   #83        25 Oct 2007            MH
 * Modified SD discover devices to validate BD address and service type 
 *
 *   #82        19 OCT 2007            DSN\SSK
 * Add HCI commands for Controller to Host flow control.
 *
 *   #81        23 Aug 2007            PG
 * Added HCI read rssi cmd.
 *
 *   #80        17 JUL 2007            PR
 * Remote name request cancel support in SD
 *
 *   #79        09 JUL 2007            PG
 * HCI Remote name req cancel and connection cancel support.
 *
 *   #78        01 JUN 2007            VK
 * Added HCI Command to read Remote Version.
 *
 *   #77        15 MAY 2007            VK
 * Added HCI Command to read Remote Supported Feature.
 *
 *   #76        23 Apr 2007            SSK
 * Change parameter type in bt_cmd_rm_tx_data_acl()  from type 'boolean'
 * to type 'bt_hc_pkt_boundary_flag_type' for stronger type checking. 
 *
 *   #75        12 Apr 2007            SSK
 * Fix bt_cmd_hc_rd_local_ver() and bt_cmd_hc_rd_local_feat() to return
 * correct command status.
 *
 *   #74        12 Apr 2007            SSK
 * Moved newly added HCI command implementation from btsocetc.c
 *
 *   #73        05 Apr 2007            PG
 * Added HCI commands for writing and reading scan type.
 *
 *   #72        05 Apr 2007            CO
 * Fixed that a couple AG commands never get sent.
 * Fixed BT command queue resource leak.
 *
 *   #71        27 Mar 2007            DSN
 * Typecasting fixes for rvct2.1 compiler.
 *
 *   #70        12 Mar 2007            GS
 * Replaced memcpy with safe string copy macros, whereever applicable.
 *
 *   #69        01 Mar 2007            DSN
 * Add support for HCI Read Buffer Size command.
 *
 *   #68        16 Feb 2007            PN
 * Made use of safe string functions.
 *
 *   #67        29 Jan  2007           MH
 * Added support to update OBEX Profile service record with 128-bit UUID.
 *
 *   #66        03 Dec 2006            JTL
 * Added bt_cmd_rc_cancel_connect().
 *
 *   #65        20 Nov 2006            MH
 * Provided NULL pointer access fix in 
 * bt_cmd_sd_update_obex_profile_service_record
 * 
 *   #64        16 Nov 2006            RY
 * Added SD support for custom 128-bit UUID
 *
 *    #63        16 Nov 2005            DM
 * Changed bt_cmd_hc_tx_acl_data() to add local_cid and frame_handle,
 * Required for AV-Sync.
 *
 *    #62        31 Oct 2006            VK
 * Fixing compilation conflict of bt_rm_atzrq_type enum.
 *
 *    #61        26 Oct 2006            VK
 * Added one parameter in function bt_cmd_rm_enforce_security_l2cap_psm and 
 * Passing the same in command to RM.
 *
 *    #60        26 Jul 2006            DM
 * Modified bt_cmd_l2_write to take in a new param - upper layer frame handle
 *
 *    #59        27 Jun 2006            DSN
 * Changes to support HostController SoC solution. Added a fix for improper
 * compiler optimization of double-word accesses on msm7500 ARM11
 *
 *    #58        25 Jun 2006            DSN
 * Removed Bonding App(BA) related references that are no longer used.
 *
 *    #57        01 Jun 2006            JTL
 * Add CID to L2Cap Mode2 security.
 *
 *    #56        05 Apr 2006            DP
 * Added RM remove all devices command.  Added HC change link key command
 * (not currently used).  Added HC write encryption key length command to
 * support Extended Service Security.
 *
 *    #56        14 Mar 2006            JH
 * Modified bt_cmd_rm_connect_sco(), adding a new parameter to indicate if eSCO
 *  should be attempted.
 *
 *    #55        30 Jan 2006            PN
 * Added bt_cmd_rm_register_radio_activity() and 
 * bt_cmd_rm_unregister_radio_activity().
 *
 *    #54        20 Jan 2006            JTL
 * Added L2 command to disconnect ACL link without waiting for timeout.
 *
 *    #53        15 Dec 2005            JH
 * Brought back bt_cmd_l2_get_num_conn() which was previously commented out. 
 *
 *    #52        02 Dec 2005            DSN
 * Changes for MSM7500 BT bringup. 
 *
 *    #51        02 Dec 2005            DSN
 * Added featurization for MSM7500 BT bringup. 
 *
 *    #50        02 Dec 2005            GS
 * Fixing potential problem of dereferencing NULL ptr in bt_cmd_rm_inquire()
 * Temporarily undoing changes made in #48.
 *
 *    #48        01 Dec 2005            JTL
 * Adding L2 command to get number of L2 connections for a particular
 * BD Addr.
 *
 *    #47        01 Dec 2005            PG
 * Cosmetic code changes to keep lint happy (no critical lint errors).
 *
 *    #46        23 Nov 2005            DP
 * Added RM commands for White List and Extended Service Security.
 *
 *    #45        9  Nov 2005            DP
 * Renamed and extended SD service registration command.
 *
 *    #44        13 Sep 2005            JH
 * Fixed cut-and-paste error in bt_cmd_hc_setup_sync_conn().
 *
 *    #43        29 Aug 2005            PN
 * Fixed a typo.
 *
 *    #42        26 Aug 2005            PN
 * Mainlined support for Response & Hold feature of HFP1.5.
 *
 *    #41        19 Aug 2005            GS
 * Update COD ARM copy in bt_cmd_rm_set_local_info() so as to make COD get/set
 * operation look atomic.
 *
 *    #40        29 Jul 2005            PN
 * Mainlined support of battery charge and RSSI indicators.
 *
 *    #39        27 Jun 2005           JH
 * Added bt_cmd_hc_setup_sync_conn().
 *
 *    #38        02 May 2005           JTL
 * Adding command to disable the L2 inital idle timeout.
 *
 *    #37        10 Apr 2005            RY
 * Adding support for SD Get Discoverable Mode.
 *
 *    #36        06 Apr 2005            JH
 * Adding support for disabling BT radio.
 *
 *    #35        16 Mar 2005            DP
 * Fix Mode 2 security for client connections.
 *
 *    #34        05 Mar 2005            JH
 * Added AFH related RM and HCI commands.
 *
 *    #33        11 Feb 2005            PN
 * Unused fields in bt_cmd_sd_unregister_service_type are now initialized.
 *
 *    #32        29 Dec 2004            JH
 * Added RM commands bt_cmd_rm_device_services_update() and 
 *  bt_cmd_rm_auto_service_search_disable().
 *
 *    #31        29 Dec 2004            JH
 * Added the following SD commands: 
 *  bt_cmd_sd_get_secondary_server_channel_number(), 
 *  bt_cmd_sd_update_service_rec_by_scn(), bt_cmd_sd_update_service_rec_by_psm(),
 *  bt_cmd_sd_update_srv_name(), bt_cmd_sd_register_srv(), 
 *  bt_cmd_sd_unregister_srv_by_scn(), bt_cmd_sd_unregister_srv_by_psm()
 * Modified the following SD commands to ensure backward compatibility:
 *  bt_cmd_sd_update_service_record(), bt_cmd_sd_register_service()
 *
 *    #30        17 Dec 2004            JTL
 * Added bt_sio commands for all of the other IOCTLs and transmit.
 *
 *    #29        13 Dec 2004            PN
 * Added code to bt_cmd_sio_in_flow_on() and bt_cmd_sio_in_flow_off() to set
 * the command code.
 *    #28        07 Dec 2004            JTL
 * Added SIO commands flow on/off.
 *
 *    #27        30 Aug 2004            JH
 * Added the following 4 commands:
 *  bt_cmd_sd_search_service_uuid128
 *  bt_cmd_sd_update_obex_profile_service_record
 *  bt_cmd_sd_register_syncml_service
 *  bt_cmd_sd_unregister_syncml_service
 *
 *    #26        25 Aug 2004            DP
 * Added battery charge and signal strength indicator setting commands
 * to AG.  Removed featurization of BT HF.
 *
 *    #25        12 Aug 2004            JH
 * Added TX continuous frequency HCI command
 *
 *    #23        7  Jul 2004            JH
 * Added HCI_accept_synchronous_connection_request and 
 *  HCI_reject_synchronous_connection_request commands for BT 1.2 eSCO support
 *
 *    #22        1  Jun 2004            DP
 * Implemented Mode 2 security and AG External I/O features.
 *
 *    #21        12 Apr 2004            PN
 * - Added bt_cmd_ag_send_pb_entries_info()
 * - Updated bt_cmd_ag_send_pb_read_result() to support AT+CPBR and AT+CPBF
 *
 *    #20        8  Apr 2004            PN
 * Added input parameter 'event' to bt_cmd_ag_send_pb_read_result().
 *
 *    #19        2  Apr 2004            DP
 * Added EMS support.
 *
 *    #18        25 Mar 2004            PN
 * - Added bt_cmd_ag_send_pb_select_result(), bt_cmd_ag_send_pb_write_result()
 * - Removed critical lint errors
 *
 *    #0         12 Jan 2004            PN
 * Added bt_cmd_ag_update_vr_state().
 *
 *    #16        12 Jan 2004            DP
 * Fix bt_cmd_rm_set_conn_role_sw_req_ok().
 *
 *    #15        19 Dec 2003            DP
 * Added RM command to allow role switch requests from remote devices.
 *
 *    #14        26 Nov 2003            DP
 * Provide extra check in SD service search command.
 *
 *    #13        12 Nov 2003            DP
 * Improved error checking for SDP commands.
 *
 *    #11        4  Nov 2003            DP
 * Added security APIs to RM.  General cleanup.
 *
 *    #10        2  Oct 2003            RY
 * Updated bt_cmd_ag_enable to handle NULL bd_addr_ptr.
 *
 *    #9         26 Sep 2003            DP
 * Updated RM Device Manager API, AG connect/disconnect and VR capable APIs,
 * and made service/device names character strings.
 * 
 *    #8         06 Aug 2003            PN
 * - Modified bt_cmd_rm_device_add() and bt_cmd_rm_device_update().
 * - Added bt_cmd_hc_wr_class_of_device().
 * 
 *    #7         12 Jun 2003            PN
 * - Added bt_cmd_ag_set_inband_ring().
 * - Added support for BT_AG_HF_EXTENSION.
 *
 *    Rev 1.31   05 Aug 2002 17:59:02   ryuen
 * Merged changes from MSM_BT.01.00.11.01.02 (MSM5100 2.1 Patch 6)
 * 

===========================================================================*/

/*===========================================================================

                     INCLUDE FILES FOR MODULE

===========================================================================*/
#include "customer.h"
#include "comdef.h"
#ifdef FEATURE_BT
#include <string.h>
#include "task.h"
#include "rex.h"
#include "dsm.h"
#include "bt.h"
#include "bti.h"
#include "btcmdi.h"
#ifndef FEATURE_BT_SOC 
#include "btlm.h"
#endif /* FEATURE_BT_SOC */
#include "btmsg.h"
#include "btsd.h"
#include "btutils.h"
#include <stdlib.h>

/*===========================================================================

             DATA DEFINITIONS AND DECLARATIONS FOR MODULE

This section contains definitions for constants, macros, types, variables
and other items needed by this module.

===========================================================================*/

#define BT_MSG_LAYER BT_MSG_GN


/*  "Manually" extern'd because it should only be used here.  */
extern struct bt_cmd_msg_struct* bt_get_free_cmd( void );


/*===========================================================================

                         FUNCTION DEFINITIONS

===========================================================================*/

/*-------------------------------------------------------------------------*/
/*                     Network Access (NA) Commands                        */
/*-------------------------------------------------------------------------*/

#ifdef FEATURE_BT_NA

/*===========================================================================

FUNCTION
  bt_cmd_na_enable

DESCRIPTION
  This function sends an NA enable command to the Bluetooth driver.

===========================================================================*/
bt_cmd_status_type bt_cmd_na_enable
(
  bt_app_id_type  bt_app_id
)
{

  bt_cmd_msg_type*    cmd_ptr;
  bt_cmd_status_type  cmd_status = BT_CS_GN_CMD_Q_FULL;

  if ( (cmd_ptr = bt_get_free_cmd()) != NULL )
  {
    cmd_ptr->cmd_hdr.cmd_type  = BT_CMD_NA_ENABLE;
    cmd_ptr->cmd_hdr.bt_app_id = bt_app_id;

    cmd_status = bt_cmd( cmd_ptr );
  }

  return( cmd_status );

}


/*===========================================================================

FUNCTION
  bt_cmd_na_disable

DESCRIPTION
  This function sends an NA disable command to the Bluetooth driver.

===========================================================================*/
bt_cmd_status_type bt_cmd_na_disable
(
  bt_app_id_type  bt_app_id
)
{

  bt_cmd_msg_type*    cmd_ptr;
  bt_cmd_status_type  cmd_status = BT_CS_GN_CMD_Q_FULL;

  if ( (cmd_ptr = bt_get_free_cmd()) != NULL )
  {
    cmd_ptr->cmd_hdr.cmd_type  = BT_CMD_NA_DISABLE;
    cmd_ptr->cmd_hdr.bt_app_id = bt_app_id;

    cmd_status = bt_cmd( cmd_ptr );
  }

  return( cmd_status );

}

#endif /* FEATURE_BT_NA */

/*-------------------------------------------------------------------------*/
/*                      Audio Gateway (AG) Commands                        */
/*-------------------------------------------------------------------------*/

#ifdef FEATURE_BT_AG
/*===========================================================================

FUNCTION
  bt_cmd_ag_enable

DESCRIPTION
  This function sends an AG enable command to the Bluetooth driver.

===========================================================================*/
bt_cmd_status_type bt_cmd_ag_enable
(
  bt_app_id_type        bt_app_id,
  bt_bd_addr_type*      bd_addr_ptr,
  bt_audio_device_type  audio_device
)
{

  bt_cmd_msg_type*    cmd_ptr;
  bt_cmd_status_type  cmd_status = BT_CS_GN_CMD_Q_FULL;

  if ( (cmd_ptr = bt_get_free_cmd()) != NULL )
  {
    cmd_ptr->cmd_hdr.cmd_type                   = BT_CMD_AG_ENABLE;
    cmd_ptr->cmd_hdr.bt_app_id                  = bt_app_id;
    if ( bd_addr_ptr != NULL )
    {
      cmd_ptr->cmd_msg.cmd_ag_enable.bd_addr      = *bd_addr_ptr;
    }
    else
    {
      memset( ( void* )( &cmd_ptr->cmd_msg.cmd_ag_enable.bd_addr ), 0,
              sizeof( bt_bd_addr_type) );
    }
    cmd_ptr->cmd_msg.cmd_ag_enable.audio_device = audio_device;
  
    cmd_status = bt_cmd( cmd_ptr );
  }

  return( cmd_status );

}


/*===========================================================================

FUNCTION
  bt_cmd_ag_connect

DESCRIPTION
  This function sends an AG connect command to the Bluetooth driver.

===========================================================================*/
bt_cmd_status_type bt_cmd_ag_connect
(
  bt_app_id_type        bt_app_id,
  bt_bd_addr_type*      bd_addr_ptr,
  bt_audio_device_type  audio_device
)
{

  bt_cmd_msg_type*    cmd_ptr;
  bt_cmd_status_type  cmd_status = BT_CS_GN_CMD_Q_FULL;

  if ( (cmd_ptr = bt_get_free_cmd()) != NULL )
  {
    cmd_ptr->cmd_hdr.cmd_type                    = BT_CMD_AG_CONNECT;
    cmd_ptr->cmd_hdr.bt_app_id                   = bt_app_id;
    cmd_ptr->cmd_msg.cmd_ag_connect.bd_addr      = *bd_addr_ptr;
    cmd_ptr->cmd_msg.cmd_ag_connect.audio_device = audio_device;
  
    cmd_status = bt_cmd( cmd_ptr );
  }

  return( cmd_status );

}


/*===========================================================================

FUNCTION
  bt_cmd_ag_config

DESCRIPTION
  This function sends an AG config command to the Bluetooth driver.

===========================================================================*/
bt_cmd_status_type bt_cmd_ag_config
(
  bt_app_id_type        bt_app_id,
  bt_ag_idle_mode_type  idle_mode,
  uint16                idle_timeout
)
{

  bt_cmd_msg_type*    cmd_ptr;
  bt_cmd_status_type  cmd_status = BT_CS_GN_CMD_Q_FULL;

  if ( (cmd_ptr = bt_get_free_cmd()) != NULL )
  {
    cmd_ptr->cmd_hdr.cmd_type                   = BT_CMD_AG_CONFIG;
    cmd_ptr->cmd_hdr.bt_app_id                  = bt_app_id;
    cmd_ptr->cmd_msg.cmd_ag_config.idle_mode    = idle_mode;
    cmd_ptr->cmd_msg.cmd_ag_config.idle_timeout = idle_timeout;
  
    cmd_status = bt_cmd( cmd_ptr );
  }
  
  return( cmd_status );

}


/*===========================================================================

FUNCTION
  bt_cmd_ag_set_vr_capable

DESCRIPTION
  This function sends an AG set VR capable command to the Bluetooth driver.

===========================================================================*/
bt_cmd_status_type bt_cmd_ag_set_vr_capable
(
  bt_app_id_type  bt_app_id,
  boolean         vr_capable
)
{

  bt_cmd_msg_type*    cmd_ptr;
  bt_cmd_status_type  cmd_status = BT_CS_GN_CMD_Q_FULL;

  if ( (cmd_ptr = bt_get_free_cmd()) != NULL )
  {
    cmd_ptr->cmd_hdr.cmd_type              = BT_CMD_AG_SET_VR_CAPABLE;
    cmd_ptr->cmd_hdr.bt_app_id             = bt_app_id;
    cmd_ptr->cmd_msg.cmd_ag_set_vr.capable = vr_capable;
  
    cmd_status = bt_cmd( cmd_ptr );
  }
  
  return( cmd_status );

}


/*===========================================================================

FUNCTION
  bt_cmd_ag_ring_audio_device

DESCRIPTION
  This function sends an AG ring command to the Bluetooth driver.

===========================================================================*/
bt_cmd_status_type bt_cmd_ag_ring_audio_device
(
  bt_app_id_type  bt_app_id,
  uint16          max_rings,
  uint16          ring_period
)
{

  bt_cmd_msg_type*    cmd_ptr;
  bt_cmd_status_type  cmd_status = BT_CS_GN_CMD_Q_FULL;

  if ( (cmd_ptr = bt_get_free_cmd()) != NULL )
  {
    cmd_ptr->cmd_hdr.cmd_type  = BT_CMD_AG_RING_AUDIO_DEVICE;
    cmd_ptr->cmd_hdr.bt_app_id = bt_app_id;
    cmd_ptr->cmd_msg.cmd_ag_ring_audio_device.max_rings = max_rings;
    cmd_ptr->cmd_msg.cmd_ag_ring_audio_device.ring_period = ring_period;
  
    cmd_status = bt_cmd( cmd_ptr );
  }

  return( cmd_status );

}


/*===========================================================================

FUNCTION
  bt_cmd_ag_audio_connect

DESCRIPTION
  This function sends an AG audio connect command to the Bluetooth driver.

===========================================================================*/
bt_cmd_status_type bt_cmd_ag_audio_connect
(
  bt_app_id_type  bt_app_id,
  uint16          max_time
)
{

  bt_cmd_msg_type*    cmd_ptr;
  bt_cmd_status_type  cmd_status = BT_CS_GN_CMD_Q_FULL;

  if ( (cmd_ptr = bt_get_free_cmd()) != NULL )
  {
    cmd_ptr->cmd_hdr.cmd_type  = BT_CMD_AG_AUDIO_CONNECT;
    cmd_ptr->cmd_hdr.bt_app_id = bt_app_id;
    cmd_ptr->cmd_msg.cmd_ag_audio_conn.max_time = max_time;
    
    cmd_status = bt_cmd( cmd_ptr );
  }

  return( cmd_status );

}


/*===========================================================================

FUNCTION
  bt_cmd_ag_audio_disconnect

DESCRIPTION
  This function sends an AG audio disconnect command to the
  Bluetooth driver.

===========================================================================*/
bt_cmd_status_type bt_cmd_ag_audio_disconnect
(
  bt_app_id_type  bt_app_id
)
{

  bt_cmd_msg_type*    cmd_ptr;
  bt_cmd_status_type  cmd_status = BT_CS_GN_CMD_Q_FULL;

  if ( (cmd_ptr = bt_get_free_cmd()) != NULL )
  {
    cmd_ptr->cmd_hdr.cmd_type  = BT_CMD_AG_AUDIO_DISCONNECT;
    cmd_ptr->cmd_hdr.bt_app_id = bt_app_id;
  
    cmd_status = bt_cmd( cmd_ptr );
  }

  return( cmd_status );

}


/*===========================================================================

FUNCTION
  bt_cmd_ag_set_ad_spkr_gain

DESCRIPTION
  This function sends an AG set audio device speaker gain command to the
  Bluetooth driver.

===========================================================================*/
bt_cmd_status_type bt_cmd_ag_set_ad_spkr_gain
(
  bt_app_id_type  bt_app_id,
  uint8           ad_spkr_gain
)
{

  bt_cmd_msg_type*    cmd_ptr;
  bt_cmd_status_type  cmd_status = BT_CS_GN_CMD_Q_FULL;

  if ( (cmd_ptr = bt_get_free_cmd()) != NULL )
  {
    cmd_ptr->cmd_hdr.cmd_type    = BT_CMD_AG_SET_AD_SPKR_GAIN;
    cmd_ptr->cmd_hdr.bt_app_id   = bt_app_id;
    cmd_ptr->cmd_msg.cmd_ag_spkr.ad_speaker_gain = ad_spkr_gain;
  
    cmd_status = bt_cmd( cmd_ptr );
  }

  return( cmd_status );

}


/*===========================================================================

FUNCTION
  bt_cmd_ag_set_ad_mic_gain

DESCRIPTION
  This function sends an AG set audio device microphone gain command to the
  Bluetooth driver.

===========================================================================*/
bt_cmd_status_type bt_cmd_ag_set_ad_mic_gain
(
  bt_app_id_type  bt_app_id,
  uint8           ad_mic_gain
)
{

  bt_cmd_msg_type*    cmd_ptr;
  bt_cmd_status_type  cmd_status = BT_CS_GN_CMD_Q_FULL;

  if ( (cmd_ptr = bt_get_free_cmd()) != NULL )
  {
    cmd_ptr->cmd_hdr.cmd_type    = BT_CMD_AG_SET_AD_MIC_GAIN;
    cmd_ptr->cmd_hdr.bt_app_id   = bt_app_id;
    cmd_ptr->cmd_msg.cmd_ag_mic.ad_microphone_gain = ad_mic_gain;
  
    cmd_status = bt_cmd( cmd_ptr );
  }

  return( cmd_status );

}


/*===========================================================================

FUNCTION
  bt_cmd_ag_disconnect

DESCRIPTION
  This function sends an AG disconnect command to the Bluetooth driver.

===========================================================================*/
bt_cmd_status_type bt_cmd_ag_disconnect
(
  bt_app_id_type  bt_app_id
)
{

  bt_cmd_msg_type*    cmd_ptr;
  bt_cmd_status_type  cmd_status = BT_CS_GN_CMD_Q_FULL;

  if ( (cmd_ptr = bt_get_free_cmd()) != NULL )
  {
    cmd_ptr->cmd_hdr.cmd_type  = BT_CMD_AG_DISCONNECT;
    cmd_ptr->cmd_hdr.bt_app_id = bt_app_id;
  
    cmd_status = bt_cmd( cmd_ptr );
  }

  return( cmd_status );

}


/*===========================================================================

FUNCTION
  bt_cmd_ag_disable

DESCRIPTION
  This function sends an AG disable command to the Bluetooth driver.

===========================================================================*/
bt_cmd_status_type bt_cmd_ag_disable
(
  bt_app_id_type  bt_app_id
)
{

  bt_cmd_msg_type*    cmd_ptr;
  bt_cmd_status_type  cmd_status = BT_CS_GN_CMD_Q_FULL;

  if ( (cmd_ptr = bt_get_free_cmd()) != NULL )
  {
    cmd_ptr->cmd_hdr.cmd_type  = BT_CMD_AG_DISABLE;
    cmd_ptr->cmd_hdr.bt_app_id = bt_app_id;
  
    cmd_status = bt_cmd( cmd_ptr );
  }
  
  return( cmd_status );

}


/*===========================================================================

FUNCTION
  bt_cmd_ag_update_vr_state

DESCRIPTION
  This function sends an AG update VR state command to the Bluetooth driver.

===========================================================================*/
bt_cmd_status_type bt_cmd_ag_update_vr_state( 
  bt_app_id_type  bt_app_id,
  boolean vr_enabled 
)
{
  bt_cmd_msg_type*    cmd_ptr;
  bt_cmd_status_type  cmd_status = BT_CS_GN_CMD_Q_FULL;

  if ( (cmd_ptr = bt_get_free_cmd()) != NULL )
  {
    cmd_ptr->cmd_hdr.cmd_type  = BT_CMD_AG_UPDATE_VR_STATE;
    cmd_ptr->cmd_hdr.bt_app_id = bt_app_id;
    cmd_ptr->cmd_msg.cmd_ag_update_vr_state.vr_enabled = vr_enabled;
  
    cmd_status = bt_cmd( cmd_ptr );
  }
  
  return( cmd_status );
}


/*===========================================================================

FUNCTION
  bt_cmd_ag_external_io_config

DESCRIPTION
  This function configures the external AG command I/O.

===========================================================================*/
bt_cmd_status_type bt_cmd_ag_external_io_config
(
  bt_app_id_type       bt_app_id,
  boolean              enable_external_io,
  dsm_watermark_type*  rx_wm_ptr,
  dsm_watermark_type*  tx_wm_ptr,
  bt_ag_int_cmd_type   int_cmd_off_mask
)
{
  bt_cmd_msg_type*    cmd_ptr;
  bt_cmd_status_type  cmd_status = BT_CS_GN_CMD_Q_FULL;

  if ( (cmd_ptr = bt_get_free_cmd()) != NULL )
  {
    cmd_ptr->cmd_hdr.cmd_type  = BT_CMD_AG_EXT_IO_CONFIG;
    cmd_ptr->cmd_hdr.bt_app_id = bt_app_id;

    cmd_ptr->cmd_msg.cmd_ag_extio.enable_external_io = enable_external_io;
    cmd_ptr->cmd_msg.cmd_ag_extio.rx_wm_ptr          = rx_wm_ptr;
    cmd_ptr->cmd_msg.cmd_ag_extio.tx_wm_ptr          = tx_wm_ptr;
    cmd_ptr->cmd_msg.cmd_ag_extio.int_cmd_off_mask   = int_cmd_off_mask;
    
    cmd_status = bt_cmd( cmd_ptr );
  }
  
  return( cmd_status );

}


/*===========================================================================

FUNCTION
  bt_cmd_ag_set_inband_ring

DESCRIPTION
  This function sends an AG command to the Bluetooth driver to enable/disable 
  inband rining in AG.

===========================================================================*/
bt_cmd_status_type bt_cmd_ag_set_inband_ring(
  bt_app_id_type bt_app_id,
  boolean        enabled 
)
{
  bt_cmd_msg_type*    cmd_ptr;
  bt_cmd_status_type  cmd_status = BT_CS_GN_CMD_Q_FULL;

  if ( (cmd_ptr = bt_get_free_cmd()) != NULL )
  {
    cmd_ptr->cmd_hdr.cmd_type  = BT_CMD_AG_SET_INBAND_RING;
    cmd_ptr->cmd_hdr.bt_app_id = bt_app_id;
    cmd_ptr->cmd_msg.cmd_ag_set_inband_ring.inband_enabled = enabled;
  
    cmd_status = bt_cmd( cmd_ptr );
  }
  
  return( cmd_status );
}


/*===========================================================================

FUNCTION
  bt_cmd_ag_set_battchg_ind

DESCRIPTION
  This function sets the Battery Charge Indication value within
  the Bluetooth AG driver.  If different than the last value set
  then the information is sent to the car-kit if currently connected.

===========================================================================*/
bt_cmd_status_type bt_cmd_ag_set_battchg_ind(
  bt_app_id_type           bt_app_id,
  bt_ag_battchg_level_type battchg
)
{
  bt_cmd_msg_type*    cmd_ptr;
  bt_cmd_status_type  cmd_status = BT_CS_GN_CMD_Q_FULL;

  if ( (cmd_ptr = bt_get_free_cmd()) != NULL )
  {
    cmd_ptr->cmd_hdr.cmd_type             = BT_CMD_AG_SET_BATTCHG_IND;
    cmd_ptr->cmd_hdr.bt_app_id            = bt_app_id;
    cmd_ptr->cmd_msg.cmd_ag_setbc.battchg = battchg;
    cmd_status = bt_cmd( cmd_ptr );
  }
  
  return( cmd_status );
}


/*===========================================================================

FUNCTION
  bt_cmd_ag_set_rssi

DESCRIPTION
  This function sets the Receive Signal Strength Indication value within
  the Bluetooth AG driver.  If different than the last value set
  then the information is sent to the car-kit if currently connected.

===========================================================================*/
bt_cmd_status_type bt_cmd_ag_set_rssi(
  bt_app_id_type         bt_app_id,
  bt_ag_rssi_level_type  rssi
)
{
  bt_cmd_msg_type*    cmd_ptr;
  bt_cmd_status_type  cmd_status = BT_CS_GN_CMD_Q_FULL;

  if ( (cmd_ptr = bt_get_free_cmd()) != NULL )
  {
    cmd_ptr->cmd_hdr.cmd_type          = BT_CMD_AG_SET_RSSI;
    cmd_ptr->cmd_hdr.bt_app_id         = bt_app_id;
    cmd_ptr->cmd_msg.cmd_ag_setrs.rssi = rssi;
    cmd_status = bt_cmd( cmd_ptr );
  }
  
  return( cmd_status );
}


/*===========================================================================

FUNCTION
  bt_cmd_ag_send_supported_rh

DESCRIPTION
  This function commands the Bluetooth driver to send a list of supported 
  respond and hold states to the car-kit.  This is in response to 
  BT_EV_AG_BTRH_RCVD(TEST).

===========================================================================*/
bt_cmd_status_type bt_cmd_ag_send_supported_rh(
  bt_app_id_type  bt_app_id,
  boolean         *supported_rh
)
{
  bt_cmd_msg_type*    cmd_ptr;
  bt_cmd_status_type  cmd_status = BT_CS_GN_CMD_Q_FULL;

  if ( (cmd_ptr = bt_get_free_cmd()) != NULL )
  {
    cmd_ptr->cmd_hdr.cmd_type  = BT_CMD_AG_SEND_SUPPORTED_RH;
    cmd_ptr->cmd_hdr.bt_app_id = bt_app_id;

    memcpy( (void*) cmd_ptr->cmd_msg.cmd_ag_send_supported_rh.supported_rh,
            (void*) supported_rh, (sizeof(boolean)*BT_AG_RH_MAX) );
    cmd_status = bt_cmd( cmd_ptr );
  }
  
  return( cmd_status );
}


/*===========================================================================

FUNCTION
  bt_cmd_ag_send_current_rh

DESCRIPTION
  This function commands the Bluetooth driver to send the current
  respond and hold state to the car-kit.  This is in response to 
  BT_EV_AG_BTRH_RCVD(SET) or BT_EV_AG_BTRH_RCVD(READ).

===========================================================================*/
bt_cmd_status_type bt_cmd_ag_send_current_rh(
  bt_app_id_type        bt_app_id,
  boolean               is_in_rh,
  bt_ag_rh_state_type   current_rh
)
{
  bt_cmd_msg_type*    cmd_ptr;
  bt_cmd_status_type  cmd_status = BT_CS_GN_CMD_Q_FULL;

  if ( (cmd_ptr = bt_get_free_cmd()) != NULL )
  {
    cmd_ptr->cmd_hdr.cmd_type  = BT_CMD_AG_SEND_CURRENT_RH;
    cmd_ptr->cmd_hdr.bt_app_id = bt_app_id;

    cmd_ptr->cmd_msg.cmd_ag_send_current_rh.is_in_rh   = is_in_rh;
    cmd_ptr->cmd_msg.cmd_ag_send_current_rh.current_rh = current_rh;

    cmd_status = bt_cmd( cmd_ptr );
  }
  
  return( cmd_status );
}

#ifdef BT_AG_HF_EXTENSION
/*===========================================================================

FUNCTION
  bt_cmd_ag_send_pb_list

DESCRIPTION
  This function commands the Bluetooth driver to send a list of supported 
  phonebooks to the car-kit.  This is in response to BT_EV_AG_PAP_CPBS(TEST).

===========================================================================*/
bt_cmd_status_type bt_cmd_ag_send_pb_list(
  bt_app_id_type      bt_app_id,
  uint8               num_supported_phonebooks,
  bt_pap_device_type  *supported_phonebooks 
)
{
  bt_cmd_msg_type*    cmd_ptr;
  bt_cmd_status_type  cmd_status = BT_CS_GN_CMD_Q_FULL;
  uint8               i;

  if ( (cmd_ptr = bt_get_free_cmd()) != NULL )
  {
    cmd_ptr->cmd_hdr.cmd_type  = BT_CMD_AG_SEND_PB_LIST;
    cmd_ptr->cmd_hdr.bt_app_id = bt_app_id;

    if ( num_supported_phonebooks > BT_PAP_DEV_MAX )
    {
      num_supported_phonebooks = BT_PAP_DEV_MAX;
    }

    cmd_ptr->cmd_msg.cmd_ag_send_pb_list.num_supported_phonebooks =
      num_supported_phonebooks;
    for (i=0; i<num_supported_phonebooks; i++)
    {
      cmd_ptr->cmd_msg.cmd_ag_send_pb_list.supported_phonebooks[i] =
        supported_phonebooks[i];
    }
  
    cmd_status = bt_cmd( cmd_ptr );
  }
  
  return( cmd_status );
}


/*===========================================================================

FUNCTION
  bt_cmd_ag_send_pb_select_result

DESCRIPTION
  This function commands the Bluetooth driver to send to the car-kit result
  of phonebook selection.  This is in response to BT_EV_AG_PAP_CPBS(SET).

===========================================================================*/
bt_cmd_status_type bt_cmd_ag_send_pb_select_result(
  bt_app_id_type      bt_app_id,
  bt_pap_device_type  phonebook,
  bt_pap_return_type  result
)
{
  bt_cmd_msg_type*    cmd_ptr;
  bt_cmd_status_type  cmd_status = BT_CS_GN_CMD_Q_FULL;

  if ( (cmd_ptr = bt_get_free_cmd()) != NULL )
  {
    cmd_ptr->cmd_hdr.cmd_type  = BT_CMD_AG_SEND_PB_SELECT_RESULT;
    cmd_ptr->cmd_hdr.bt_app_id = bt_app_id;

    cmd_ptr->cmd_msg.cmd_ag_send_pb_select_result.pb_id  = phonebook;
    cmd_ptr->cmd_msg.cmd_ag_send_pb_select_result.result = result;
  
    cmd_status = bt_cmd( cmd_ptr );
  }
  
  return( cmd_status );
}


/*===========================================================================

FUNCTION
  bt_cmd_ag_send_pb_info

DESCRIPTION
  This function commands the Bluetooth driver to send info on the selected 
  phonebook to the car-kit.  This is in response to BT_EV_AG_PAP_CPBS(READ).

===========================================================================*/
bt_cmd_status_type bt_cmd_ag_send_pb_info(
  bt_app_id_type      bt_app_id,
  bt_pap_device_type  phonebook,
  uint16              used_recs,
  uint16              total_recs 
)
{
  bt_cmd_msg_type*    cmd_ptr;
  bt_cmd_status_type  cmd_status = BT_CS_GN_CMD_Q_FULL;

  if ( (cmd_ptr = bt_get_free_cmd()) != NULL )
  {
    cmd_ptr->cmd_hdr.cmd_type  = BT_CMD_AG_SEND_PB_INFO;
    cmd_ptr->cmd_hdr.bt_app_id = bt_app_id;

    cmd_ptr->cmd_msg.cmd_ag_send_pb_info.pb_id       = phonebook;
    cmd_ptr->cmd_msg.cmd_ag_send_pb_info.used_recs   = used_recs;
    cmd_ptr->cmd_msg.cmd_ag_send_pb_info.total_recs  = total_recs;
  
    cmd_status = bt_cmd( cmd_ptr );
  }
  
  return( cmd_status );
}


/*===========================================================================

FUNCTION
  bt_cmd_ag_send_pb_entries_info

DESCRIPTION
  This function commands the Bluetooth driver to send entries information
  of the selected phonebook to the car-kit.  This is in response to
  BT_EV_AG_PAP_CPBR( TEST ).

===========================================================================*/
bt_cmd_status_type bt_cmd_ag_send_pb_entries_info(
  bt_app_id_type      bt_app_id,
  bt_pap_device_type  selected_phonebook,
  bt_pap_return_type  result,
  uint16              start,
  uint16              end,
  uint16              max_num_len,
  uint16              max_text_len
)
{
  bt_cmd_msg_type*    cmd_ptr;
  bt_cmd_status_type  cmd_status = BT_CS_GN_CMD_Q_FULL;

  if ( (cmd_ptr = bt_get_free_cmd()) != NULL )
  {
    cmd_ptr->cmd_hdr.cmd_type  = BT_CMD_AG_SEND_PB_READ_RESULT;
    cmd_ptr->cmd_hdr.bt_app_id = bt_app_id;

    cmd_ptr->cmd_msg.cmd_ag_send_pb_entries_info.pb_id  = selected_phonebook;
    cmd_ptr->cmd_msg.cmd_ag_send_pb_entries_info.result = result;
    cmd_ptr->cmd_msg.cmd_ag_send_pb_entries_info.start_idx = start;
    cmd_ptr->cmd_msg.cmd_ag_send_pb_entries_info.end_idx = end;
    cmd_ptr->cmd_msg.cmd_ag_send_pb_entries_info.max_num_len = max_num_len;
    cmd_ptr->cmd_msg.cmd_ag_send_pb_entries_info.max_text_len = max_text_len;
    cmd_status = bt_cmd( cmd_ptr );
  }
  
  return( cmd_status );
}


/*===========================================================================

FUNCTION
  bt_cmd_ag_send_pb_read_result

DESCRIPTION
  This function commands the Bluetooth driver to send result of reading
  a given entry of the selected phonebook to the car-kit.  This is in response 
  to either BT_EV_AG_PAP_CPBR( SET ) or BT_EV_AG_PAP_CPBF.

===========================================================================*/
bt_cmd_status_type bt_cmd_ag_send_pb_read_result(
  bt_app_id_type      bt_app_id,
  bt_event_type       event,
  bt_pap_device_type  pb,
  bt_pap_return_type  result,
  bt_pap_record_type  *rec_ptr
)
{
  bt_cmd_msg_type*    cmd_ptr;
  bt_cmd_status_type  cmd_status = BT_CS_GN_CMD_Q_FULL;

  if ( (result != BT_PAP_SUCCESS) || (rec_ptr == NULL) )
  {
    cmd_status = BT_CS_AG_INVALID_PARAMETERS;
  }
  else if ( (cmd_ptr = bt_get_free_cmd()) != NULL )
  {
    cmd_ptr->cmd_hdr.cmd_type  = BT_CMD_AG_SEND_PB_READ_RESULT;
    cmd_ptr->cmd_hdr.bt_app_id = bt_app_id;

    cmd_ptr->cmd_msg.cmd_ag_send_pb_read_result.event  = event;
    cmd_ptr->cmd_msg.cmd_ag_send_pb_read_result.pb_id  = pb;
    cmd_ptr->cmd_msg.cmd_ag_send_pb_read_result.result = result;

    cmd_ptr->cmd_msg.cmd_ag_send_pb_read_result.record = *rec_ptr;
    cmd_status = bt_cmd( cmd_ptr );
  }
  
  return( cmd_status );
}


/*===========================================================================

FUNCTION
  bt_cmd_ag_send_pb_write_result

DESCRIPTION
  This function commands the Bluetooth driver to send to the car-kit result 
  of editing the selected phonebook.  This is in response to BT_EV_AG_PAP_CPBW.

===========================================================================*/
bt_cmd_status_type bt_cmd_ag_send_pb_write_result(
  bt_app_id_type      bt_app_id,
  bt_pap_device_type  pb,
  bt_pap_return_type  result
)
{
  bt_cmd_msg_type*    cmd_ptr;
  bt_cmd_status_type  cmd_status = BT_CS_GN_CMD_Q_FULL;

  if ( (cmd_ptr = bt_get_free_cmd()) != NULL )
  {
    cmd_ptr->cmd_hdr.cmd_type  = BT_CMD_AG_SEND_PB_WRITE_RESULT;
    cmd_ptr->cmd_hdr.bt_app_id = bt_app_id;

    cmd_ptr->cmd_msg.cmd_ag_send_pb_write_result.pb_id  = pb;
    cmd_ptr->cmd_msg.cmd_ag_send_pb_write_result.result = result;

    cmd_status = bt_cmd( cmd_ptr );
  }
  
  return( cmd_status );
}


/*===========================================================================

FUNCTION
  bt_cmd_ag_send_subscriber_numbers

DESCRIPTION
  This function commands the Bluetooth driver to send subscriber number info
  to the car-kit.  This is in response to BT_EV_AG_PAP_CNUM.
  
  Up to 3 subscriber numbers can be sent to car-kit: voice number, fax number,
  and data number.

===========================================================================*/
bt_cmd_status_type bt_cmd_ag_send_subscriber_numbers(
  bt_app_id_type              bt_app_id,
  uint8                       subs_num_cnt,
  bt_pap_subsc_num_info_type  *sni 
)
{
  bt_cmd_msg_type*    cmd_ptr;
  bt_cmd_status_type  cmd_status = BT_CS_GN_CMD_Q_FULL;
  uint8               i;

  if ( (cmd_ptr = bt_get_free_cmd()) != NULL )
  {
    cmd_ptr->cmd_hdr.cmd_type             = BT_CMD_AG_SEND_SUBS_NUM_INFO;
    cmd_ptr->cmd_hdr.bt_app_id            = bt_app_id;

    for ( i=0; i<subs_num_cnt && i<3; i++)
    {
      cmd_ptr->cmd_msg.cmd_ag_send_subsc_num_info.sni[i]  = sni[i];
    }

    while ( i<3 )
    {
      cmd_ptr->cmd_msg.cmd_ag_send_subsc_num_info.sni[i++].number[0] = 0;
    }

    cmd_status = bt_cmd( cmd_ptr );
  }

  return( cmd_status );
}
#endif /* BT_AG_HF_EXTENSION */
#endif /* FEATURE_BT_AG */


/*-------------------------------------------------------------------------*/
/*               Service Discovery Protocol (SDP) Commands                 */
/*-------------------------------------------------------------------------*/

/*===========================================================================

FUNCTION
  bt_cmd_sd_set_service_discoverable_mode

DESCRIPTION

===========================================================================*/
bt_cmd_status_type bt_cmd_sd_set_service_discoverable_mode
(
  bt_app_id_type                        bt_app_id,
  bt_sd_service_discoverable_mode_type  srv_discv_mode
)
{

  bt_cmd_msg_type*    cmd_ptr;
  bt_cmd_status_type  cmd_status = BT_CS_GN_CMD_Q_FULL;

  if ( (cmd_ptr = bt_get_free_cmd()) != NULL )
  {
    cmd_ptr->cmd_hdr.cmd_type  = BT_CMD_SD_SET_SERVICE_DISCOVERABLE_MODE;
    cmd_ptr->cmd_hdr.bt_app_id = bt_app_id;
    cmd_ptr->cmd_msg.cmd_sd_set_srv_discv_mode.srv_discv_mode =
                                 srv_discv_mode;
  
    cmd_status = bt_cmd( cmd_ptr );
  }

  return( cmd_status );

}

/*===========================================================================

FUNCTION
  bt_cmd_sd_get_discoverable_mode

DESCRIPTION

===========================================================================*/
bt_cmd_status_type bt_cmd_sd_get_discoverable_mode
(
  bt_app_id_type  bt_app_id
)
{

  bt_cmd_msg_type*    cmd_ptr;
  bt_cmd_status_type  cmd_status = BT_CS_GN_CMD_Q_FULL;

  if ( (cmd_ptr = bt_get_free_cmd()) != NULL )
  {
    cmd_ptr->cmd_hdr.cmd_type  = BT_CMD_SD_GET_DISCOVERABLE_MODE;
    cmd_ptr->cmd_hdr.bt_app_id = bt_app_id;
  
    cmd_status = bt_cmd( cmd_ptr );
  }

  return( cmd_status );

}

/*===========================================================================

FUNCTION
  bt_cmd_sd_discover_devices_ext

DESCRIPTION

===========================================================================*/
bt_cmd_status_type bt_cmd_sd_discover_devices_ext
(
  bt_app_id_type    bt_app_id,
  uint16            service_class_bitmap,
  bt_bd_addr_type*  bd_addr_ptr,
  uint16            max_num_response,
  uint8             duration_secs
)
{

  bt_cmd_msg_type*    cmd_ptr;
  bt_cmd_status_type  cmd_status = BT_CS_GN_CMD_Q_FULL;

  /*Service class unknown requires BD address*/
  if((service_class_bitmap == BT_SERVICE_CLASS_UNKNOWN) && 
              (bd_addr_ptr == NULL))
  {
    return BT_CS_SD_BAD_PARAM ;
  }

  if ( (cmd_ptr = bt_get_free_cmd()) != NULL )
  {
    cmd_ptr->cmd_hdr.cmd_type  = BT_CMD_SD_DISCOVER_DEVICES_EXT;
    cmd_ptr->cmd_hdr.bt_app_id = bt_app_id;
    if(bd_addr_ptr != NULL)
    {
      cmd_ptr->cmd_msg.cmd_sd_discv_dev.bd_addr = *bd_addr_ptr;
    }
                                 
    cmd_ptr->cmd_msg.cmd_sd_discv_dev.dev_discv.service_class_bitmap =
                                 service_class_bitmap;
    cmd_ptr->cmd_msg.cmd_sd_discv_dev.dev_discv.max_num_response =
                                 max_num_response;
    cmd_ptr->cmd_msg.cmd_sd_discv_dev.dev_discv.duration_secs =
                                 duration_secs;

    cmd_status = bt_cmd( cmd_ptr );
  }

  return( cmd_status );

}

/*===========================================================================

FUNCTION
  bt_cmd_sd_discover_devices

DESCRIPTION

===========================================================================*/
bt_cmd_status_type bt_cmd_sd_discover_devices
(
  bt_app_id_type    bt_app_id,
  uint16            service_class_bitmap,
  bt_bd_addr_type*  bd_addr_ptr,
  uint16            max_num_response
)
{

  bt_cmd_msg_type*    cmd_ptr;
  bt_cmd_status_type  cmd_status = BT_CS_GN_CMD_Q_FULL;

  /*Service class unknown requires BD address*/
  if((service_class_bitmap == BT_SERVICE_CLASS_UNKNOWN) && 
              (bd_addr_ptr == NULL))
  {
    return BT_CS_SD_BAD_PARAM ;
  }

  if ( (cmd_ptr = bt_get_free_cmd()) != NULL )
  {
    cmd_ptr->cmd_hdr.cmd_type  = BT_CMD_SD_DISCOVER_DEVICES;
    cmd_ptr->cmd_hdr.bt_app_id = bt_app_id;
    if(bd_addr_ptr != NULL)
    {
      cmd_ptr->cmd_msg.cmd_sd_discv_dev.bd_addr = *bd_addr_ptr;
    }
    cmd_ptr->cmd_msg.cmd_sd_discv_dev.dev_discv.service_class_bitmap =
                                 service_class_bitmap;
    cmd_ptr->cmd_msg.cmd_sd_discv_dev.dev_discv.max_num_response =
                                 max_num_response;
    cmd_ptr->cmd_msg.cmd_sd_discv_dev.dev_discv.duration_secs = 
                                 BT_SD_DEFAULT_SEARCH_DURATION;

    cmd_status = bt_cmd( cmd_ptr );
  }

  return( cmd_status );

}



/*===========================================================================

FUNCTION
  bt_cmd_sd_start_device_discovery

DESCRIPTION

===========================================================================*/
bt_cmd_status_type bt_cmd_sd_start_device_discovery
(
  bt_app_id_type    bt_app_id,
  uint16            service_class_bitmap,
  uint16            max_num_response
)
{

  bt_cmd_msg_type*    cmd_ptr;
  bt_cmd_status_type  cmd_status = BT_CS_GN_CMD_Q_FULL;

  if ( (cmd_ptr = bt_get_free_cmd()) != NULL )
  {
    cmd_ptr->cmd_hdr.cmd_type  = BT_CMD_SD_START_DEVICE_DISCOVERY;
    cmd_ptr->cmd_hdr.bt_app_id = bt_app_id;
    cmd_ptr->cmd_msg.cmd_sd_start_dev_discv.service_class_bitmap =
                                 service_class_bitmap;
    cmd_ptr->cmd_msg.cmd_sd_start_dev_discv.max_num_response =
                                 max_num_response;
    cmd_ptr->cmd_msg.cmd_sd_start_dev_discv.duration_secs = 
                                 BT_SD_DEFAULT_SEARCH_DURATION;

    cmd_status = bt_cmd( cmd_ptr );
  }

  return( cmd_status );

}


/*===========================================================================

FUNCTION
  bt_cmd_sd_stop_device_discovery

DESCRIPTION

===========================================================================*/
bt_cmd_status_type bt_cmd_sd_stop_device_discovery
(
  bt_app_id_type bt_app_id
)
{

  bt_cmd_msg_type*    cmd_ptr;
  bt_cmd_status_type  cmd_status = BT_CS_GN_CMD_Q_FULL;

  if ( (cmd_ptr = bt_get_free_cmd()) != NULL )
  {
    cmd_ptr->cmd_hdr.cmd_type  = BT_CMD_SD_STOP_DEVICE_DISCOVERY;
    cmd_ptr->cmd_hdr.bt_app_id = bt_app_id;
  
    cmd_status = bt_cmd( cmd_ptr );
  }

  return( cmd_status );

}


/*===========================================================================

FUNCTION
  bt_cmd_sd_get_device_name

DESCRIPTION

===========================================================================*/
bt_cmd_status_type bt_cmd_sd_get_device_name
(
  bt_app_id_type    bt_app_id,
  bt_bd_addr_type*  bd_addr_ptr
)
{

  bt_cmd_msg_type*    cmd_ptr;
  bt_cmd_status_type  cmd_status = BT_CS_GN_CMD_Q_FULL;

  if ( (cmd_ptr = bt_get_free_cmd()) != NULL )
  {
    cmd_ptr->cmd_hdr.cmd_type       = BT_CMD_SD_GET_DEVICE_NAME;
    cmd_ptr->cmd_hdr.bt_app_id                   = bt_app_id;
    cmd_ptr->cmd_msg.cmd_sd_get_dev_name.bd_addr = *bd_addr_ptr;
  
    cmd_status = bt_cmd( cmd_ptr );
  }

  return( cmd_status );

}

/*===========================================================================

FUNCTION
  bt_cmd_sd_get_device_name_cancel

DESCRIPTION

===========================================================================*/
bt_cmd_status_type bt_cmd_sd_get_device_name_cancel
(
  bt_app_id_type    bt_app_id,
  bt_bd_addr_type*  bd_addr_ptr
)
{

  bt_cmd_msg_type*    cmd_ptr;
  bt_cmd_status_type  cmd_status = BT_CS_GN_CMD_Q_FULL;

  if ( (cmd_ptr = bt_get_free_cmd()) != NULL )
  {
    cmd_ptr->cmd_hdr.cmd_type       = BT_CMD_SD_GET_DEVICE_NAME_CANCEL;
    cmd_ptr->cmd_hdr.bt_app_id      = bt_app_id;
    cmd_ptr->cmd_msg.cmd_sd_get_dev_name_cancel.bd_addr = *bd_addr_ptr;
  
    cmd_status = bt_cmd( cmd_ptr );
  }

  return( cmd_status );

}

/*===========================================================================

FUNCTION
  bt_cmd_sd_send_service_search_req

DESCRIPTION

===========================================================================*/
bt_cmd_status_type bt_cmd_sd_send_service_search_req
(
  bt_app_id_type                  bt_app_id,
  bt_bd_addr_type*                bd_addr_ptr,
  bt_sd_srch_pat_uuid_list_type*  uuid_list,
  uint16                          max_srv_rec_count
)
{

  uint16                          i;
  bt_cmd_msg_type*                cmd_ptr;
  bt_cmd_sd_search_service_type*  sss_ptr;
  bt_cmd_status_type              cmd_status = BT_CS_SD_BAD_PARAM;


  if ( (uuid_list->num_uuid128 <= BT_SD_MAX_NUM_OF_SRV_SRCH_PAT_UUID128) &&
       (uuid_list->num_uuid <= BT_SD_MAX_NUM_OF_SRV_SRCH_PAT_UUID) )
  {
    cmd_status = BT_CS_GN_CMD_Q_FULL;

    if ( (cmd_ptr = bt_get_free_cmd()) != NULL )
    {
      cmd_ptr->cmd_hdr.cmd_type      = BT_CMD_SD_SEARCH_SERVICE;
      cmd_ptr->cmd_hdr.bt_app_id     = bt_app_id;

      sss_ptr = &(cmd_ptr->cmd_msg.cmd_sd_srv_srch);
      sss_ptr->bd_addr               = *bd_addr_ptr;
      sss_ptr->generate_xml          = FALSE;
      sss_ptr->srv_srch_pat.pdu_type = BT_SD_PDU_SERVICE_SEARCH_REQ;
      sss_ptr->srv_srch_pat.max_srv_rec_count =
                                       max_srv_rec_count;

      sss_ptr->srv_srch_pat.num_uuid = uuid_list->num_uuid;
      for ( i = 0; i < uuid_list->num_uuid; i++ )
      {
        sss_ptr->srv_srch_pat.uuid[i] = uuid_list->uuid[i];
      }

      sss_ptr->srv_srch_pat.num_uuid128 = uuid_list->num_uuid128;
      for ( i = 0; i < uuid_list->num_uuid128; i++ )
      {
        sss_ptr->srv_srch_pat.uuid128[i] = uuid_list->uuid128[i];
      }

      cmd_status = bt_cmd( cmd_ptr );
    }
  }

  return( cmd_status );

}


/*===========================================================================

FUNCTION
  bt_cmd_sd_send_service_attribute_req

DESCRIPTION

===========================================================================*/
bt_cmd_status_type bt_cmd_sd_send_service_attribute_req
(
  bt_app_id_type                     bt_app_id,
  bt_bd_addr_type*                   bd_addr_ptr,
  uint32                             srv_rec_handle,
  bt_sd_srch_pat_attr_id_list_type*  attr_id_list,
  uint16                             max_attr_byte_count
)
{

  uint16                          i;
  bt_cmd_msg_type*                cmd_ptr;
  bt_cmd_sd_search_service_type*  sss_ptr;
  bt_cmd_status_type              cmd_status = BT_CS_SD_BAD_PARAM;

  if ( attr_id_list->num_attr_id <=
         BT_SD_MAX_NUM_OF_SRV_SRCH_PAT_ATTR_ID )
  {
    cmd_status = BT_CS_GN_CMD_Q_FULL;

    if ( (cmd_ptr = bt_get_free_cmd()) != NULL )
    {
      cmd_ptr->cmd_hdr.cmd_type      = BT_CMD_SD_SEARCH_SERVICE;
      cmd_ptr->cmd_hdr.bt_app_id     = bt_app_id;
    
      sss_ptr = &(cmd_ptr->cmd_msg.cmd_sd_srv_srch);
      sss_ptr->bd_addr               = *bd_addr_ptr;
      sss_ptr->generate_xml          = FALSE;
      sss_ptr->srv_srch_pat.pdu_type = BT_SD_PDU_SERVICE_ATTRIBUTE_REQ;
      sss_ptr->srv_srch_pat.max_attr_byte_count =
                                       max_attr_byte_count;
    
      sss_ptr->srv_srch_pat.srv_rec_handle = srv_rec_handle;
    
      sss_ptr->srv_srch_pat.num_attr_id = attr_id_list->num_attr_id;
      for ( i = 0; i < attr_id_list->num_attr_id; i++ )
      {
        sss_ptr->srv_srch_pat.attr_id[i].is_range =
          attr_id_list->attr_id[i].is_range;
        sss_ptr->srv_srch_pat.attr_id[i].value =
          attr_id_list->attr_id[i].value;
      }
    
      cmd_status = bt_cmd( cmd_ptr );
    }
  }

  return( cmd_status );

}


/*===========================================================================

FUNCTION
  bt_cmd_sd_search_service_ext

DESCRIPTION

===========================================================================*/
bt_cmd_status_type bt_cmd_sd_search_service_ext
(
  bt_app_id_type                     bt_app_id,
  bt_bd_addr_type*                   bd_addr_ptr,
  bt_sd_srch_pat_uuid_list_type*     uuid_list,
  bt_sd_srch_pat_attr_id_list_type*  attr_id_list,
  uint16                             max_attr_byte_count,
  boolean                            generate_xml
)
{
  
  uint16                          i;
  bt_cmd_msg_type*                cmd_ptr;
  bt_cmd_sd_search_service_type*  sss_ptr;
  bt_cmd_status_type              cmd_status = BT_CS_SD_BAD_PARAM;

  if ( (uuid_list->num_uuid128 <=
          BT_SD_MAX_NUM_OF_SRV_SRCH_PAT_UUID128) &&
       (uuid_list->num_uuid <=
          BT_SD_MAX_NUM_OF_SRV_SRCH_PAT_UUID) &&
       (attr_id_list->num_attr_id <=
          BT_SD_MAX_NUM_OF_SRV_SRCH_PAT_ATTR_ID) )
  {
    cmd_status = BT_CS_GN_CMD_Q_FULL;

    if ( (cmd_ptr = bt_get_free_cmd()) != NULL )
    {
      cmd_ptr->cmd_hdr.cmd_type      = BT_CMD_SD_SEARCH_SERVICE_EXT;
      cmd_ptr->cmd_hdr.bt_app_id     = bt_app_id;
    
      sss_ptr = &(cmd_ptr->cmd_msg.cmd_sd_srv_srch);
      sss_ptr->bd_addr               = *bd_addr_ptr;
      sss_ptr->generate_xml          = generate_xml;
      sss_ptr->srv_srch_pat.pdu_type = BT_SD_PDU_SERVICE_SEARCH_ATTRIBUTE_REQ;
      sss_ptr->srv_srch_pat.max_attr_byte_count = max_attr_byte_count;
    
      sss_ptr->srv_srch_pat.num_uuid = uuid_list->num_uuid;
      for ( i = 0; i < uuid_list->num_uuid; i++ )
      {
        sss_ptr->srv_srch_pat.uuid[i] = uuid_list->uuid[i];
      }
      
      sss_ptr->srv_srch_pat.num_uuid128 = uuid_list->num_uuid128;
      for ( i = 0; i < uuid_list->num_uuid128; i++ )
      {
        sss_ptr->srv_srch_pat.uuid128[i] = uuid_list->uuid128[i];
      }
    
      sss_ptr->srv_srch_pat.num_attr_id = attr_id_list->num_attr_id;
      for ( i = 0; i < attr_id_list->num_attr_id; i++ )
      {
        sss_ptr->srv_srch_pat.attr_id[i].is_range =
          attr_id_list->attr_id[i].is_range;
        sss_ptr->srv_srch_pat.attr_id[i].value =
          attr_id_list->attr_id[i].value;
      }
    
      cmd_status = bt_cmd( cmd_ptr );
    }
  }

  return( cmd_status );

}


/*===========================================================================

FUNCTION
  bt_cmd_sd_search_service

DESCRIPTION

===========================================================================*/
bt_cmd_status_type bt_cmd_sd_search_service
(
  bt_app_id_type                     bt_app_id,
  bt_bd_addr_type*                   bd_addr_ptr,
  bt_sd_srch_pat_uuid_list_type*     uuid_list,
  bt_sd_srch_pat_attr_id_list_type*  attr_id_list,
  uint16                             max_attr_byte_count
)
{
  
  uint16                          i;
  bt_cmd_msg_type*                cmd_ptr;
  bt_cmd_sd_search_service_type*  sss_ptr;
  bt_cmd_status_type              cmd_status = BT_CS_SD_BAD_PARAM;

  if ( (uuid_list->num_uuid128 <=
          BT_SD_MAX_NUM_OF_SRV_SRCH_PAT_UUID128) &&
       (uuid_list->num_uuid <=
          BT_SD_MAX_NUM_OF_SRV_SRCH_PAT_UUID) &&
       (attr_id_list->num_attr_id <=
          BT_SD_MAX_NUM_OF_SRV_SRCH_PAT_ATTR_ID) )
  {
    cmd_status = BT_CS_GN_CMD_Q_FULL;

    if ( (cmd_ptr = bt_get_free_cmd()) != NULL )
    {
      cmd_ptr->cmd_hdr.cmd_type      = BT_CMD_SD_SEARCH_SERVICE;
      cmd_ptr->cmd_hdr.bt_app_id     = bt_app_id;
    
      sss_ptr = &(cmd_ptr->cmd_msg.cmd_sd_srv_srch);
      sss_ptr->bd_addr               = *bd_addr_ptr;
      sss_ptr->generate_xml          = FALSE;
      sss_ptr->srv_srch_pat.pdu_type = BT_SD_PDU_SERVICE_SEARCH_ATTRIBUTE_REQ;
      sss_ptr->srv_srch_pat.max_attr_byte_count = max_attr_byte_count;
    
      sss_ptr->srv_srch_pat.num_uuid = uuid_list->num_uuid;
      for ( i = 0; i < uuid_list->num_uuid; i++ )
      {
        sss_ptr->srv_srch_pat.uuid[i] = uuid_list->uuid[i];
      }
      
      sss_ptr->srv_srch_pat.num_uuid128 = uuid_list->num_uuid128;
      for ( i = 0; i < uuid_list->num_uuid128; i++ )
      {
        sss_ptr->srv_srch_pat.uuid128[i] = uuid_list->uuid128[i];
      }
    
      sss_ptr->srv_srch_pat.num_attr_id = attr_id_list->num_attr_id;
      for ( i = 0; i < attr_id_list->num_attr_id; i++ )
      {
        sss_ptr->srv_srch_pat.attr_id[i].is_range =
          attr_id_list->attr_id[i].is_range;
        sss_ptr->srv_srch_pat.attr_id[i].value =
          attr_id_list->attr_id[i].value;
      }
    
      cmd_status = bt_cmd( cmd_ptr );
    }
  }

  return( cmd_status );

}


/*===========================================================================

FUNCTION
  bt_cmd_sd_stop_service_search

DESCRIPTION

===========================================================================*/
bt_cmd_status_type bt_cmd_sd_stop_service_search
(
  bt_app_id_type        bt_app_id,
  bt_bd_addr_type*      bd_addr_ptr
)
{

  bt_cmd_msg_type*    cmd_ptr;
  bt_cmd_status_type  cmd_status = BT_CS_GN_CMD_Q_FULL;

  if ( (cmd_ptr = bt_get_free_cmd()) != NULL )
  {
    cmd_ptr->cmd_hdr.cmd_type  = BT_CMD_SD_STOP_SERVICE_SEARCH;
    cmd_ptr->cmd_hdr.bt_app_id = bt_app_id;
  
    cmd_ptr->cmd_msg.cmd_sd_stop_srv_srch.bd_addr = *bd_addr_ptr;

    cmd_status = bt_cmd( cmd_ptr );
  }

  return( cmd_status );

}


/*===========================================================================

FUNCTION
  bt_cmd_sd_cancel_idle_timer

DESCRIPTION

===========================================================================*/
bt_cmd_status_type bt_cmd_sd_cancel_idle_timer
(
  bt_app_id_type                     bt_app_id,
  bt_bd_addr_type*                   bd_addr_ptr
)
{
  
  bt_cmd_msg_type*                cmd_ptr;
  bt_cmd_status_type              cmd_status;

  if(bd_addr_ptr == NULL)
  {
    return BT_CS_SD_BAD_PARAM ;
  }
  
  if ( (cmd_ptr = bt_get_free_cmd()) != NULL )
  {
    cmd_ptr->cmd_hdr.cmd_type      = BT_CMD_SD_CANCEL_IDLE_TIMER;
    cmd_ptr->cmd_hdr.bt_app_id     = bt_app_id;
    
    cmd_ptr->cmd_msg.cmd_sd_cancel_idle_timer.bd_addr = 
      *bd_addr_ptr;
    
    cmd_status = bt_cmd( cmd_ptr );
  }
  else
  {
    cmd_status = BT_CS_GN_CMD_Q_FULL;
  }

  return( cmd_status );

}

/*===========================================================================

FUNCTION
  bt_cmd_sd_get_server_channel_number

DESCRIPTION

===========================================================================*/
bt_cmd_status_type bt_cmd_sd_get_server_channel_number
(
  bt_app_id_type    bt_app_id,
  bt_bd_addr_type*  bd_addr_ptr,
  uint16            service_class
)
{

  bt_cmd_msg_type*    cmd_ptr;
  bt_cmd_status_type  cmd_status = BT_CS_GN_CMD_Q_FULL;
  bt_cmd_sd_get_server_channel_number_type*  cmd_get_scn_ptr;

  if ( (cmd_ptr = bt_get_free_cmd()) != NULL )
  {
    cmd_ptr->cmd_hdr.cmd_type  = BT_CMD_SD_GET_SERVER_CHANNEL_NUMBER;
    cmd_ptr->cmd_hdr.bt_app_id = bt_app_id;
  
    cmd_get_scn_ptr            = &(cmd_ptr->cmd_msg.cmd_sd_get_scn);
    cmd_get_scn_ptr->bd_addr   = *bd_addr_ptr;
    cmd_get_scn_ptr->service_class = service_class;
  
    cmd_status = bt_cmd( cmd_ptr );
  }

  return( cmd_status );

}


/*===========================================================================

FUNCTION
  bt_cmd_sd_get_secondary_server_channel_number

DESCRIPTION

===========================================================================*/
bt_cmd_status_type bt_cmd_sd_get_secondary_server_channel_number
(
  bt_app_id_type    bt_app_id,
  bt_bd_addr_type*  bd_addr_ptr,
  uint16            service_class
)
{

  bt_cmd_msg_type*    cmd_ptr;
  bt_cmd_status_type  cmd_status = BT_CS_GN_CMD_Q_FULL;
  bt_cmd_sd_get_server_channel_number_type*  cmd_get_scn_ptr;

  if ( (cmd_ptr = bt_get_free_cmd()) != NULL )
  {
    cmd_ptr->cmd_hdr.cmd_type  = BT_CMD_SD_GET_SECONDARY_SERVER_CHANNEL_NUMBER;
    cmd_ptr->cmd_hdr.bt_app_id = bt_app_id;
  
    cmd_get_scn_ptr            = &(cmd_ptr->cmd_msg.cmd_sd_get_scn);
    cmd_get_scn_ptr->bd_addr   = *bd_addr_ptr;
    cmd_get_scn_ptr->service_class = service_class;
  
    cmd_status = bt_cmd( cmd_ptr );
  }

  return( cmd_status );

}


/*===========================================================================

FUNCTION
  bt_cmd_sd_search_service_uuid128

DESCRIPTION
  Perform a service search using a 128-bit UUID
  
NOTE
  This command only supports searching for a single attribute using a single
  128-bit UUID, unlike bt_cmd_sd_search_service which allows searching for
  multiple attributes using multiple UUIDs.

===========================================================================*/
bt_cmd_status_type bt_cmd_sd_search_service_uuid128
(
  bt_app_id_type                     bt_app_id,
  bt_bd_addr_type*                   bd_addr_ptr,
  bt_sd_uuid128_type*                uuid_ptr,
  uint16                             attr_id
)
{
  
  bt_cmd_msg_type*                cmd_ptr;
  bt_cmd_status_type              cmd_status = BT_CS_GN_CMD_Q_FULL;
  bt_cmd_sd_search_service_type*  sss_ptr;

  if ( (cmd_ptr = bt_get_free_cmd()) != NULL )
  { 
    cmd_ptr->cmd_hdr.cmd_type      = BT_CMD_SD_SEARCH_SERVICE_UUID128;
    cmd_ptr->cmd_hdr.bt_app_id     = bt_app_id;

    sss_ptr = &(cmd_ptr->cmd_msg.cmd_sd_srv_srch);
    sss_ptr->bd_addr               = *bd_addr_ptr;
    sss_ptr->generate_xml          = FALSE;
    sss_ptr->srv_srch_pat.pdu_type = BT_SD_PDU_SERVICE_SEARCH_ATTRIBUTE_REQ;
    sss_ptr->srv_srch_pat.max_attr_byte_count =
                                     BT_SD_DEFAULT_MAX_ATTR_BYTE_COUNT;

    sss_ptr->srv_srch_pat.num_uuid = 0;
    sss_ptr->srv_srch_pat.num_uuid128 = 1;
    sss_ptr->srv_srch_pat.uuid128[0] = *uuid_ptr;

    sss_ptr->srv_srch_pat.num_attr_id = 1;
    sss_ptr->srv_srch_pat.attr_id[0].is_range = FALSE;
    sss_ptr->srv_srch_pat.attr_id[0].value = attr_id;

    cmd_status = bt_cmd( cmd_ptr );
  }

  return( cmd_status );
}


/*===========================================================================

FUNCTION
  bt_cmd_sd_update_service_record

DESCRIPTION

===========================================================================*/
bt_cmd_status_type bt_cmd_sd_update_service_record
(
  bt_app_id_type   bt_app_id,
  bt_sd_uuid_type  uuid,
  uint8            scn,
  char*            service_name_str_ptr
)
{

  bt_cmd_msg_type*    cmd_ptr;
  bt_cmd_status_type  cmd_status = BT_CS_GN_CMD_Q_FULL;
  bt_cmd_sd_update_service_record_type*  cmd_update_srv_rec_ptr;

  if ( (cmd_ptr = bt_get_free_cmd()) != NULL )
  {
    cmd_ptr->cmd_hdr.cmd_type    = BT_CMD_SD_UPDATE_SERVICE_RECORD;
    cmd_ptr->cmd_hdr.bt_app_id   = bt_app_id;
  
    cmd_update_srv_rec_ptr = &(cmd_ptr->cmd_msg.cmd_sd_update_srv_rec);
    cmd_update_srv_rec_ptr->uuid    = uuid;
    cmd_update_srv_rec_ptr->scn     = BT_RC_SCN_NOT_SPECIFIED;
    cmd_update_srv_rec_ptr->new_scn = scn; 
    cmd_update_srv_rec_ptr->psm     = BT_L2_PSM_INVALID;
    cmd_update_srv_rec_ptr->new_psm = BT_L2_PSM_INVALID;

    BT_STRCPY( (char *)cmd_update_srv_rec_ptr->service_name_str,
               service_name_str_ptr,
               sizeof (cmd_update_srv_rec_ptr->service_name_str) );

    cmd_status = bt_cmd( cmd_ptr );
  }

  return( cmd_status );

}


/*===========================================================================

FUNCTION
  bt_cmd_sd_update_service_rec_by_scn

DESCRIPTION

===========================================================================*/
bt_cmd_status_type bt_cmd_sd_update_srv_rec_by_scn
(
  bt_app_id_type   bt_app_id,
  bt_sd_uuid_type  uuid,
  uint8            scn,
  uint8            new_scn,
  char*            service_name_str_ptr
)
{

  bt_cmd_msg_type*    cmd_ptr;
  bt_cmd_status_type  cmd_status = BT_CS_GN_CMD_Q_FULL;
  bt_cmd_sd_update_service_record_type*  cmd_update_srv_rec_ptr;

  if ( (cmd_ptr = bt_get_free_cmd()) != NULL )
  {
    cmd_ptr->cmd_hdr.cmd_type    = BT_CMD_SD_UPDATE_SRV_REC_BY_SCN;
    cmd_ptr->cmd_hdr.bt_app_id   = bt_app_id;
  
    cmd_update_srv_rec_ptr = &(cmd_ptr->cmd_msg.cmd_sd_update_srv_rec);
    cmd_update_srv_rec_ptr->uuid    = uuid;
    cmd_update_srv_rec_ptr->scn     = scn;
    cmd_update_srv_rec_ptr->new_scn = new_scn;
    BT_STRCPY( (char *)cmd_update_srv_rec_ptr->service_name_str,
               service_name_str_ptr,
               sizeof (cmd_update_srv_rec_ptr->service_name_str) );
  
    cmd_status = bt_cmd( cmd_ptr );
  }

  return( cmd_status );

}


/*===========================================================================

FUNCTION
  bt_cmd_sd_update_service_rec_by_psm

DESCRIPTION

===========================================================================*/
bt_cmd_status_type bt_cmd_sd_update_srv_rec_by_psm
(
  bt_app_id_type   bt_app_id,
  bt_sd_uuid_type  uuid,
  uint16           psm,
  uint16           new_psm,
  char*            service_name_str_ptr
)
{

  bt_cmd_msg_type*    cmd_ptr;
  bt_cmd_status_type  cmd_status = BT_CS_GN_CMD_Q_FULL;
  bt_cmd_sd_update_service_record_type*  cmd_update_srv_rec_ptr;

  if ( (cmd_ptr = bt_get_free_cmd()) != NULL )
  {
    cmd_ptr->cmd_hdr.cmd_type    = BT_CMD_SD_UPDATE_SRV_REC_BY_PSM;
    cmd_ptr->cmd_hdr.bt_app_id   = bt_app_id;
  
    cmd_update_srv_rec_ptr = &(cmd_ptr->cmd_msg.cmd_sd_update_srv_rec);
    cmd_update_srv_rec_ptr->uuid    = uuid;
    cmd_update_srv_rec_ptr->scn     = BT_RC_SCN_NOT_SPECIFIED;
    cmd_update_srv_rec_ptr->new_scn = BT_RC_SCN_NOT_SPECIFIED;
    cmd_update_srv_rec_ptr->psm     = psm;
    cmd_update_srv_rec_ptr->new_psm = new_psm;
    BT_STRCPY( (char *)cmd_update_srv_rec_ptr->service_name_str,
               service_name_str_ptr,
               sizeof (cmd_update_srv_rec_ptr->service_name_str) );

    cmd_status = bt_cmd( cmd_ptr );
  }

  return( cmd_status );

}


/*===========================================================================

FUNCTION
  bt_cmd_sd_update_srv_name

DESCRIPTION

===========================================================================*/
bt_cmd_status_type bt_cmd_sd_update_srv_name
(
  bt_app_id_type   bt_app_id,
  bt_sd_uuid_type  uuid,
  char*            service_name_str_ptr
)
{

  bt_cmd_msg_type*    cmd_ptr;
  bt_cmd_status_type  cmd_status = BT_CS_GN_CMD_Q_FULL;
  bt_cmd_sd_update_service_record_type*  cmd_update_srv_rec_ptr;

  if ( (cmd_ptr = bt_get_free_cmd()) != NULL )
  {
    cmd_ptr->cmd_hdr.cmd_type    = BT_CMD_SD_UPDATE_SRV_NAME;
    cmd_ptr->cmd_hdr.bt_app_id   = bt_app_id;
  
    cmd_update_srv_rec_ptr = &(cmd_ptr->cmd_msg.cmd_sd_update_srv_rec);
    cmd_update_srv_rec_ptr->uuid    = uuid;
    BT_STRCPY( (char *)cmd_update_srv_rec_ptr->service_name_str,
               service_name_str_ptr,
               sizeof (cmd_update_srv_rec_ptr->service_name_str) );

    cmd_status = bt_cmd( cmd_ptr );
  }

  return( cmd_status );
}

#ifdef FEATURE_BT_EXTPF

/*===========================================================================

FUNCTION
  bt_cmd_sd_update_obex_profile_service_record

DESCRIPTION

===========================================================================*/
bt_cmd_status_type bt_cmd_sd_update_obex_profile_service_record
(
  bt_app_id_type         bt_app_id,
  bt_sd_uuid_type        uuid,
  uint8                  scn,
  char*                  service_name_str_ptr,
  bt_sd_uuid128_type*    service_id_ptr,
  bt_sd_uint8_list_type* supported_formats_list_ptr,
  uint8                  supported_capabilities,
  uint16                 supported_features,
  uint32                 supported_functions,
  uint64                 total_image_capacity
)
{

  bt_cmd_msg_type*    cmd_ptr;
  bt_cmd_status_type  cmd_status = BT_CS_GN_CMD_Q_FULL;
  bt_cmd_sd_update_obex_profile_service_rec_type*  cmd_update_srv_rec_ptr;

  if ( (cmd_ptr = bt_get_free_cmd()) != NULL )
  {
    cmd_ptr->cmd_hdr.cmd_type    = BT_CMD_SD_UPDATE_OBEX_PROFILE_SERVICE_RECORD;
    cmd_ptr->cmd_hdr.bt_app_id   = bt_app_id;
  
    cmd_update_srv_rec_ptr = &(cmd_ptr->cmd_msg.cmd_sd_update_obex_srv_rec);
    cmd_update_srv_rec_ptr->uuid = uuid;
    cmd_update_srv_rec_ptr->scn  = scn;
    if ( service_id_ptr != NULL )
    {
      cmd_update_srv_rec_ptr->service_id = *service_id_ptr;
    }
    else
    {
      cmd_update_srv_rec_ptr->service_id = bt_sd_null_uuid128;
    }
    if ( supported_formats_list_ptr != NULL )
    {                                       
      cmd_update_srv_rec_ptr->supported_formats_list = 
        *supported_formats_list_ptr;
    }
    else
    {
      cmd_update_srv_rec_ptr->supported_formats_list.num_val = 0;
    }
    cmd_update_srv_rec_ptr->supported_capabilities = supported_capabilities;
    cmd_update_srv_rec_ptr->supported_features = supported_features;
    cmd_update_srv_rec_ptr->supported_functions = supported_functions;
    cmd_update_srv_rec_ptr->total_image_capacity = total_image_capacity;

    if ( service_name_str_ptr != NULL )
    {
      BT_STRCPY( (char *)cmd_update_srv_rec_ptr->service_name_str,
                 service_name_str_ptr,
                 sizeof (cmd_update_srv_rec_ptr->service_name_str) );
    }
    else
    {
      cmd_update_srv_rec_ptr->service_name_str[0] = 0 ;
    }

    cmd_status = bt_cmd( cmd_ptr );
  }

  return( cmd_status );
}

/*===========================================================================

FUNCTION
  bt_cmd_sd_update_obex_profile_service_record

DESCRIPTION
    This command updates 128-bit UUID SD service records. 
===========================================================================*/
bt_cmd_status_type bt_cmd_sd_custom_update_obex_profile_service_record
(
  bt_app_id_type         bt_app_id,
  bt_sd_uuid128_type*    uuid128_ptr,
  uint8                  scn,
  char*                  service_name_str_ptr,
  bt_sd_uuid128_type*    service_id_ptr,
  bt_sd_uint8_list_type* supported_formats_list_ptr,
  uint8                  supported_capabilities,
  uint16                 supported_features,
  uint32                 supported_functions,
  uint64                 total_image_capacity
)
{

  bt_cmd_msg_type*    cmd_ptr;
  bt_cmd_status_type  cmd_status = BT_CS_GN_CMD_Q_FULL;
  bt_cmd_sd_update_obex_profile_service_rec_type*  cmd_update_srv_rec_ptr;

  if ( (cmd_ptr = bt_get_free_cmd()) != NULL )
  {
    cmd_ptr->cmd_hdr.cmd_type    = BT_CMD_SD_CUSTOM_UPDATE_OBEX_PROFILE_SERVICE_RECORD;
    cmd_ptr->cmd_hdr.bt_app_id   = bt_app_id;

    
    cmd_update_srv_rec_ptr = &(cmd_ptr->cmd_msg.cmd_sd_update_obex_srv_rec);
    cmd_update_srv_rec_ptr->scn  = scn;
    
    if ( uuid128_ptr != NULL )
    {
      cmd_update_srv_rec_ptr->uuid128 = *uuid128_ptr;
    }
    else
    {
      cmd_update_srv_rec_ptr->uuid128 = bt_sd_null_uuid128;
    }

    if ( service_id_ptr != NULL )
    {
      cmd_update_srv_rec_ptr->service_id = *service_id_ptr;
    }
    else
    {
      cmd_update_srv_rec_ptr->service_id = bt_sd_null_uuid128;
    }
    if ( supported_formats_list_ptr != NULL )
    {                                       
      cmd_update_srv_rec_ptr->supported_formats_list = 
                            *supported_formats_list_ptr;
    }
    else
    {
      cmd_update_srv_rec_ptr->supported_formats_list.num_val = 0;
    }
    cmd_update_srv_rec_ptr->supported_capabilities = supported_capabilities;
    cmd_update_srv_rec_ptr->supported_features = supported_features;
    cmd_update_srv_rec_ptr->supported_functions = supported_functions;
    cmd_update_srv_rec_ptr->total_image_capacity = total_image_capacity;

    if ( service_name_str_ptr != NULL )
    {
      BT_STRCPY( (char *)cmd_update_srv_rec_ptr->service_name_str,
                 service_name_str_ptr,
                 sizeof (cmd_update_srv_rec_ptr->service_name_str) );
    }
    else
    {
      cmd_update_srv_rec_ptr->service_name_str[0] = 0 ;
    }

    cmd_status = bt_cmd( cmd_ptr );
  }

  return( cmd_status );
}

#endif /* FEATURE_BT_EXTPF */



/*===========================================================================

FUNCTION
  bt_cmd_sd_register_serv_ext

DESCRIPTION
  This function sends a SD Register Service Extended command to the
  Bluetooth driver.

===========================================================================*/
bt_cmd_status_type bt_cmd_sd_register_serv_ext
(
  bt_app_id_type   bt_app_id,
  bt_sd_uuid_type  uuid,
  uint16           ver,
  uint8            scn,
  uint16           psm,
  boolean          connectable,
  char*            service_name_str_ptr
)
{

  bt_cmd_msg_type*    cmd_ptr;
  bt_cmd_status_type  cmd_status = BT_CS_GN_CMD_Q_FULL;
  bt_cmd_sd_register_service_type*  cmd_reg_srv_ptr;

  if ( ( scn != BT_RC_SCN_NOT_SPECIFIED ) &&  /* either SCN or PSM can */
       ( psm != BT_L2_PSM_INVALID       ) )   /* be used but not both  */
  {
    return BT_CS_SD_BAD_PARAM;
  }        

  if ( (cmd_ptr = bt_get_free_cmd()) != NULL )
  {
    cmd_ptr->cmd_hdr.cmd_type  = BT_CMD_SD_REGISTER_SERV_EXT;
    cmd_ptr->cmd_hdr.bt_app_id = bt_app_id;
  
    cmd_reg_srv_ptr = &(cmd_ptr->cmd_msg.cmd_sd_reg_srv);
    cmd_reg_srv_ptr->uuid            = uuid;
    cmd_reg_srv_ptr->scn             = scn;
    cmd_reg_srv_ptr->psm             = psm;
    cmd_reg_srv_ptr->profile_version = ver;
    cmd_reg_srv_ptr->connectable     = connectable;

    BT_STRCPY( (char *)cmd_reg_srv_ptr->service_name_str,
               service_name_str_ptr,
               sizeof(cmd_reg_srv_ptr->service_name_str) );
  
    cmd_reg_srv_ptr->
      service_name_str[ BT_SD_MAX_SERVICE_NAME_LEN ] = 0;
    
    cmd_status = bt_cmd( cmd_ptr );
  }

  return( cmd_status );

}


/*===========================================================================

FUNCTION
  bt_cmd_sd_register_service

DESCRIPTION
  This function sends a SD Register Service command to the
  Bluetooth driver.

===========================================================================*/
bt_cmd_status_type bt_cmd_sd_register_service
(
  bt_app_id_type   bt_app_id,
  bt_sd_uuid_type  uuid,
  uint16           ver,
  uint8            scn,
  char*            service_name_str_ptr
)
{

  bt_cmd_msg_type*    cmd_ptr;
  bt_cmd_status_type  cmd_status = BT_CS_GN_CMD_Q_FULL;
  bt_cmd_sd_register_service_type*  cmd_reg_srv_ptr;

  if ( (cmd_ptr = bt_get_free_cmd()) != NULL )
  {
    cmd_ptr->cmd_hdr.cmd_type  = BT_CMD_SD_REGISTER_SERVICE;
    cmd_ptr->cmd_hdr.bt_app_id = bt_app_id;
  
    cmd_reg_srv_ptr = &(cmd_ptr->cmd_msg.cmd_sd_reg_srv);
    cmd_reg_srv_ptr->uuid            = uuid;
    cmd_reg_srv_ptr->scn             = scn;
    cmd_reg_srv_ptr->psm             = BT_L2_PSM_INVALID;
    cmd_reg_srv_ptr->profile_version = ver;
    cmd_reg_srv_ptr->connectable     = TRUE;
    
    BT_STRCPY( (char *)cmd_reg_srv_ptr->service_name_str,
               service_name_str_ptr,
               sizeof (cmd_reg_srv_ptr->service_name_str) );

    cmd_status = bt_cmd( cmd_ptr );
  }

  return( cmd_status );

}  


/*===========================================================================

FUNCTION
  bt_cmd_sd_internal_register_custom_service

DESCRIPTION
  This function sends a SD Register Custom Service extended command to the
  Bluetooth driver.

===========================================================================*/
LOCAL bt_cmd_status_type bt_cmd_sd_internal_register_custom_service
(
  bt_app_id_type               bt_app_id,
  const bt_sd_uuid_list_type*  uuid_list_ptr,
  uint16                       ver,
  uint8                        scn,
  uint16                       psm,
  boolean                      connectable,
  char*                        service_name_str_ptr,
  bt_cmd_type                  cmd_type
)
{

  bt_cmd_msg_type*                         cmd_ptr;
  bt_cmd_status_type                       cmd_status = BT_CS_GN_CMD_Q_FULL;
  bt_cmd_sd_register_custom_service_type*  cmd_reg_custom_srv_ptr;
  uint8                                    i;
  
  if ( ( ( scn != BT_RC_SCN_NOT_SPECIFIED ) &&  /* either SCN or PSM can */
         ( psm != BT_L2_PSM_INVALID ) ) ||      /* be used but not both  */
       ( uuid_list_ptr->num_uuid128 > BT_SD_MAX_NUM_OF_UUID128 ) ||
       ( uuid_list_ptr->num_uuid > BT_SD_MAX_NUM_OF_UUID ) )
  {
    return BT_CS_SD_BAD_PARAM;
  }        

  if ( (cmd_ptr = bt_get_free_cmd()) != NULL )
  {
    cmd_ptr->cmd_hdr.cmd_type  = cmd_type;
    cmd_ptr->cmd_hdr.bt_app_id = bt_app_id;

    cmd_reg_custom_srv_ptr = &(cmd_ptr->cmd_msg.cmd_sd_reg_custom_srv);

    for ( i = 0;
          i < uuid_list_ptr->num_uuid128;
          i++)
    {
      cmd_reg_custom_srv_ptr->uuid_list.uuid128[i] =
        uuid_list_ptr->uuid128[i];
    }
    cmd_reg_custom_srv_ptr->uuid_list.num_uuid128 = i;

    for ( i = 0;
          i < uuid_list_ptr->num_uuid;
          i++)
    {
      cmd_reg_custom_srv_ptr->uuid_list.uuid[i] = uuid_list_ptr->uuid[i];
    }
    cmd_reg_custom_srv_ptr->uuid_list.num_uuid = i;
    
    cmd_reg_custom_srv_ptr->scn                = scn;
    cmd_reg_custom_srv_ptr->psm                = psm;
    cmd_reg_custom_srv_ptr->profile_version    = ver;
    cmd_reg_custom_srv_ptr->connectable        = connectable;

    BT_STRCPY( (char *)cmd_reg_custom_srv_ptr->service_name_str,
               service_name_str_ptr,
               sizeof (cmd_reg_custom_srv_ptr->service_name_str) );

    cmd_status = bt_cmd( cmd_ptr );
  }

  return( cmd_status );

}  


/*===========================================================================

FUNCTION
  bt_cmd_sd_register_custom_service_ext

DESCRIPTION
  This function sends a SD Register Custom Service extended command to the
  Bluetooth driver.

===========================================================================*/
bt_cmd_status_type bt_cmd_sd_register_custom_service_ext
(
  bt_app_id_type               bt_app_id,
  const bt_sd_uuid_list_type*  uuid_list_ptr,
  uint16                       ver,
  uint8                        scn,
  uint16                       psm,
  boolean                      connectable,
  char*                        service_name_str_ptr
)
{
  bt_cmd_status_type    cmd_status = BT_CS_GN_CMD_Q_FULL;
  bt_cmd_type           cmd_type =  BT_CMD_SD_REGISTER_CUSTOM_SERVICE_EXT;

  cmd_status = bt_cmd_sd_internal_register_custom_service( bt_app_id, uuid_list_ptr, 
                                                           ver, scn, psm, connectable,
                                                           service_name_str_ptr,
                                                           cmd_type );
  return( cmd_status );

}  


/*===========================================================================

FUNCTION
  bt_cmd_sd_register_custom_service

DESCRIPTION
  This function sends a SD Register Custom Service command to the
  Bluetooth driver.

===========================================================================*/
bt_cmd_status_type bt_cmd_sd_register_custom_service
(
  bt_app_id_type               bt_app_id,
  const bt_sd_uuid_list_type*  uuid_list_ptr,
  uint16                       ver,
  uint8                        scn,
  uint16                       psm,
  char*                        service_name_str_ptr
)
{

  bt_cmd_status_type    cmd_status = BT_CS_GN_CMD_Q_FULL;
  bt_cmd_type           cmd_type =  BT_CMD_SD_REGISTER_CUSTOM_SERVICE;
  
  cmd_status = bt_cmd_sd_internal_register_custom_service( bt_app_id, uuid_list_ptr, 
                                                           ver, scn, psm, TRUE,
                                                           service_name_str_ptr, 
                                                           cmd_type );

  return( cmd_status );
}  


/*===========================================================================

FUNCTION
  bt_cmd_sd_unregister_service

DESCRIPTION

===========================================================================*/
bt_cmd_status_type bt_cmd_sd_unregister_service
(
  bt_app_id_type   bt_app_id,
  bt_sd_uuid_type  uuid
)
{

  bt_cmd_msg_type*    cmd_ptr;
  bt_cmd_status_type  cmd_status = BT_CS_GN_CMD_Q_FULL;
  bt_cmd_sd_unregister_service_type*  cmd_unreg_srv_ptr;

  if ( (cmd_ptr = bt_get_free_cmd()) != NULL )
  {
    cmd_ptr->cmd_hdr.cmd_type    = BT_CMD_SD_UNREGISTER_SERVICE;
    cmd_ptr->cmd_hdr.bt_app_id   = bt_app_id;
  
    cmd_unreg_srv_ptr = &(cmd_ptr->cmd_msg.cmd_sd_unreg_srv);
    cmd_unreg_srv_ptr->uuid = uuid;
    cmd_unreg_srv_ptr->scn  = BT_RC_SCN_NOT_SPECIFIED;
    cmd_unreg_srv_ptr->psm  = BT_L2_PSM_INVALID;
  
    cmd_status = bt_cmd( cmd_ptr );
  }

  return( cmd_status );

}


/*===========================================================================

FUNCTION
  bt_cmd_sd_unregister_custom_service

DESCRIPTION

===========================================================================*/
bt_cmd_status_type bt_cmd_sd_unregister_custom_service
(
  bt_app_id_type             bt_app_id,
  const bt_sd_uuid128_type*  uuid128_ptr
)
{

  bt_cmd_msg_type*    cmd_ptr;
  bt_cmd_status_type  cmd_status = BT_CS_GN_CMD_Q_FULL;
  bt_cmd_sd_unregister_custom_service_type*  cmd_unreg_custom_srv_ptr;

  if ( (cmd_ptr = bt_get_free_cmd()) != NULL )
  {
    cmd_ptr->cmd_hdr.cmd_type    = BT_CMD_SD_UNREGISTER_CUSTOM_SERVICE;
    cmd_ptr->cmd_hdr.bt_app_id   = bt_app_id;
  
    cmd_unreg_custom_srv_ptr = &(cmd_ptr->cmd_msg.cmd_sd_unreg_custom_srv);
    cmd_unreg_custom_srv_ptr->uuid128 = *uuid128_ptr;
  
    cmd_status = bt_cmd( cmd_ptr );
  }

  return( cmd_status );

}


/*===========================================================================

FUNCTION
  bt_cmd_sd_unregister_srv_by_scn

DESCRIPTION

===========================================================================*/
bt_cmd_status_type bt_cmd_sd_unregister_srv_by_scn
(
  bt_app_id_type   bt_app_id,
  bt_sd_uuid_type  uuid,
  uint8            scn
)
{

  bt_cmd_msg_type*    cmd_ptr;
  bt_cmd_status_type  cmd_status = BT_CS_GN_CMD_Q_FULL;
  bt_cmd_sd_unregister_service_type*  cmd_unreg_srv_ptr;

  if ( (cmd_ptr = bt_get_free_cmd()) != NULL )
  {
    cmd_ptr->cmd_hdr.cmd_type    = BT_CMD_SD_UNREGISTER_SRV_BY_SCN;
    cmd_ptr->cmd_hdr.bt_app_id   = bt_app_id;
  
    cmd_unreg_srv_ptr = &(cmd_ptr->cmd_msg.cmd_sd_unreg_srv);
    cmd_unreg_srv_ptr->uuid = uuid;
    cmd_unreg_srv_ptr->scn  = scn;
    cmd_unreg_srv_ptr->psm  = BT_L2_PSM_INVALID;
  
    cmd_status = bt_cmd( cmd_ptr );
  }

  return( cmd_status );

}
            

/*===========================================================================

FUNCTION
  bt_cmd_sd_unregister_srv_by_psm

DESCRIPTION

===========================================================================*/
bt_cmd_status_type bt_cmd_sd_unregister_srv_by_psm
(
  bt_app_id_type   bt_app_id,
  bt_sd_uuid_type  uuid,
  uint16           psm
)
{

  bt_cmd_msg_type*    cmd_ptr;
  bt_cmd_status_type  cmd_status = BT_CS_GN_CMD_Q_FULL;
  bt_cmd_sd_unregister_service_type*  cmd_unreg_srv_ptr;

  if ( (cmd_ptr = bt_get_free_cmd()) != NULL )
  {
    cmd_ptr->cmd_hdr.cmd_type    = BT_CMD_SD_UNREGISTER_SRV_BY_PSM;
    cmd_ptr->cmd_hdr.bt_app_id   = bt_app_id;
  
    cmd_unreg_srv_ptr = &(cmd_ptr->cmd_msg.cmd_sd_unreg_srv);
    cmd_unreg_srv_ptr->uuid = uuid;
    cmd_unreg_srv_ptr->psm  = psm;
    cmd_unreg_srv_ptr->scn  = BT_RC_SCN_NOT_SPECIFIED;
  
    cmd_status = bt_cmd( cmd_ptr );
  }

  return( cmd_status );

}

#ifdef FEATURE_BT_EXTPF_SML  

/*===========================================================================

FUNCTION
  bt_cmd_sd_register_syncml_service

DESCRIPTION

===========================================================================*/
bt_cmd_status_type bt_cmd_sd_register_syncml_service
(
  bt_app_id_type   bt_app_id,
  boolean          client,  
  uint8            scn, 
  char*            service_name_str_ptr     
)
{

  bt_cmd_msg_type*    cmd_ptr;
  bt_cmd_status_type  cmd_status = BT_CS_GN_CMD_Q_FULL;
  bt_cmd_sd_register_syncml_service_type*  cmd_reg_sml_ptr;

  if ( (cmd_ptr = bt_get_free_cmd()) != NULL )
  {
    cmd_ptr->cmd_hdr.cmd_type    = BT_CMD_SD_REGISTER_SYNCML_SERVICE;
    cmd_ptr->cmd_hdr.bt_app_id   = bt_app_id;
  
    cmd_reg_sml_ptr = &(cmd_ptr->cmd_msg.cmd_sd_reg_syncml_srv);
    cmd_reg_sml_ptr->client = client;
    cmd_reg_sml_ptr->scn  = scn;
    BT_STRCPY( (char *)cmd_reg_sml_ptr->service_name_str,
               service_name_str_ptr,
               sizeof (cmd_reg_sml_ptr->service_name_str) );

    cmd_status = bt_cmd( cmd_ptr );
  }

  return( cmd_status );
}


/*===========================================================================

FUNCTION
  bt_cmd_sd_unregister_syncml_service

DESCRIPTION

===========================================================================*/
bt_cmd_status_type bt_cmd_sd_unregister_syncml_service
(
  bt_app_id_type   bt_app_id,
  boolean          client  
)
{

  bt_cmd_msg_type*    cmd_ptr;
  bt_cmd_status_type  cmd_status = BT_CS_GN_CMD_Q_FULL;

  if ( (cmd_ptr = bt_get_free_cmd()) != NULL )
  {
    cmd_ptr->cmd_hdr.cmd_type    = BT_CMD_SD_UNREGISTER_SYNCML_SERVICE;
    cmd_ptr->cmd_hdr.bt_app_id   = bt_app_id;
  
    cmd_ptr->cmd_msg.cmd_sd_unreg_syncml_srv.client = client;

    cmd_status = bt_cmd( cmd_ptr );
  }

  return( cmd_status );
}

#endif /* FEATURE_BT_EXTPF_SML */

/*-------------------------------------------------------------------------*/
/*                            SIO Commands                                 */
/*-------------------------------------------------------------------------*/

/*===========================================================================

FUNCTION
  bt_cmd_sio_transmit

DESCRIPTION

===========================================================================*/
bt_cmd_status_type bt_cmd_sio_transmit
(
  bt_app_id_type         bt_app_id,
  sio_stream_id_type     stream_id,
  dsm_item_type*         dsm_ptr
)
{
  bt_cmd_msg_type*    cmd_ptr;
  bt_cmd_status_type  cmd_status = BT_CS_GN_CMD_Q_FULL;

  if ( (cmd_ptr = bt_get_free_cmd()) != NULL )
  {
    cmd_ptr->cmd_hdr.cmd_type    = BT_CMD_SIO_TRANSMIT;
    cmd_ptr->cmd_hdr.bt_app_id   = bt_app_id;
  
    cmd_ptr->cmd_msg.cmd_sio_transmit.stream_id = stream_id;
    cmd_ptr->cmd_msg.cmd_sio_transmit.dsm_ptr   = dsm_ptr;
    
    cmd_status = bt_cmd( cmd_ptr );
  }

  return( cmd_status );

}

/*===========================================================================

FUNCTION
  bt_cmd_sio_in_flow_on

DESCRIPTION

===========================================================================*/
bt_cmd_status_type bt_cmd_sio_in_flow_on
(
  bt_app_id_type         bt_app_id,
  sio_stream_id_type     stream_id
)
{
  bt_cmd_msg_type*    cmd_ptr;
  bt_cmd_status_type  cmd_status = BT_CS_GN_CMD_Q_FULL;

  if ( (cmd_ptr = bt_get_free_cmd()) != NULL )
  {
    cmd_ptr->cmd_hdr.cmd_type    = BT_CMD_SIO_IN_FLOW_ON;
    cmd_ptr->cmd_hdr.bt_app_id   = bt_app_id;
  
    cmd_ptr->cmd_msg.cmd_sio_in_flow.stream_id = stream_id;
    cmd_ptr->cmd_msg.cmd_sio_in_flow.cmd       = SIO_IOCTL_INBOUND_FLOW_ENABLE;
    
    cmd_status = bt_cmd( cmd_ptr );
  }

  return( cmd_status );
}

/*===========================================================================

FUNCTION
  bt_cmd_sio_in_flow_off

DESCRIPTION

===========================================================================*/
bt_cmd_status_type bt_cmd_sio_in_flow_off
(
  bt_app_id_type         bt_app_id,
  sio_stream_id_type     stream_id
)
{
  bt_cmd_msg_type*    cmd_ptr;
  bt_cmd_status_type  cmd_status = BT_CS_GN_CMD_Q_FULL;

  if ( (cmd_ptr = bt_get_free_cmd()) != NULL )
  {
    cmd_ptr->cmd_hdr.cmd_type    = BT_CMD_SIO_IN_FLOW_OFF;
    cmd_ptr->cmd_hdr.bt_app_id   = bt_app_id;
  
    cmd_ptr->cmd_msg.cmd_sio_in_flow.stream_id = stream_id;
    cmd_ptr->cmd_msg.cmd_sio_in_flow.cmd       = SIO_IOCTL_INBOUND_FLOW_DISABLE;
    
    cmd_status = bt_cmd( cmd_ptr );
  }

  return( cmd_status );
}                                                                         

/*===========================================================================

FUNCTION
  bt_cmd_sio_

DESCRIPTION

===========================================================================*/
bt_cmd_status_type bt_cmd_sio_change_mode
(
  bt_app_id_type         bt_app_id,
  sio_stream_id_type     stream_id,
  sio_stream_mode_type   stream_mode,
  dsm_watermark_type    *tx_queue,
  dsm_watermark_type    *rx_queue,
  sio_rx_func_ptr_type   rx_func_ptr

)
{
  bt_cmd_msg_type*    cmd_ptr;
  bt_cmd_status_type  cmd_status = BT_CS_GN_CMD_Q_FULL;

  if ( (cmd_ptr = bt_get_free_cmd()) != NULL )
  {
    cmd_ptr->cmd_hdr.cmd_type    = BT_CMD_SIO_CHANGE_MODE;
    cmd_ptr->cmd_hdr.bt_app_id   = bt_app_id;
  
    cmd_ptr->cmd_msg.cmd_sio_change_mode.stream_id   = stream_id;
    cmd_ptr->cmd_msg.cmd_sio_change_mode.stream_mode = stream_mode;
    cmd_ptr->cmd_msg.cmd_sio_change_mode.tx_queue    = tx_queue;
    cmd_ptr->cmd_msg.cmd_sio_change_mode.rx_queue    = rx_queue;
    cmd_ptr->cmd_msg.cmd_sio_change_mode.rx_func_ptr = rx_func_ptr;
    
    cmd_status = bt_cmd( cmd_ptr );
  }

  return( cmd_status );
}


/*===========================================================================

FUNCTION
  bt_cmd_sio_enable_autodetect

DESCRIPTION

===========================================================================*/
bt_cmd_status_type bt_cmd_sio_enable_autodetect
(
  bt_app_id_type         bt_app_id,
  sio_stream_id_type     stream_id
)
{
  bt_cmd_msg_type*    cmd_ptr;
  bt_cmd_status_type  cmd_status = BT_CS_GN_CMD_Q_FULL;

  if ( (cmd_ptr = bt_get_free_cmd()) != NULL )
  {
    cmd_ptr->cmd_hdr.cmd_type    = BT_CMD_SIO_ENABLE_AUTODETECT;
    cmd_ptr->cmd_hdr.bt_app_id   = bt_app_id;
  
    cmd_ptr->cmd_msg.cmd_sio_enable_autodetect.stream_id = stream_id;
    
    cmd_status = bt_cmd( cmd_ptr );
  }

  return( cmd_status );
}


/*===========================================================================

FUNCTION
  bt_cmd_sio_set_flow_ctl

DESCRIPTION

===========================================================================*/
bt_cmd_status_type bt_cmd_sio_set_flow_ctl
(
  bt_app_id_type         bt_app_id,
  sio_stream_id_type     stream_id,
  sio_flow_ctl_type      tx_flow,
  sio_flow_ctl_type      rx_flow
)
{
  bt_cmd_msg_type*    cmd_ptr;
  bt_cmd_status_type  cmd_status = BT_CS_GN_CMD_Q_FULL;

  if ( (cmd_ptr = bt_get_free_cmd()) != NULL )
  {
    cmd_ptr->cmd_hdr.cmd_type    = BT_CMD_SIO_SET_FLOW_CTL;
    cmd_ptr->cmd_hdr.bt_app_id   = bt_app_id;
  
    cmd_ptr->cmd_msg.cmd_sio_set_flow_ctl.stream_id = stream_id;
    cmd_ptr->cmd_msg.cmd_sio_set_flow_ctl.tx_flow   = tx_flow;
    cmd_ptr->cmd_msg.cmd_sio_set_flow_ctl.rx_flow   = rx_flow;
    
    cmd_status = bt_cmd( cmd_ptr );
  }

  return( cmd_status );
}


/*===========================================================================

FUNCTION
  bt_cmd_sio_enable_dtr_event

DESCRIPTION

===========================================================================*/
bt_cmd_status_type bt_cmd_sio_enable_dtr_event
(
  bt_app_id_type         bt_app_id,
  sio_stream_id_type     stream_id,
  sio_vv_func_ptr_type   enable_dte_ready_event
)
{
  bt_cmd_msg_type*    cmd_ptr;
  bt_cmd_status_type  cmd_status = BT_CS_GN_CMD_Q_FULL;

  if ( (cmd_ptr = bt_get_free_cmd()) != NULL )
  {
    cmd_ptr->cmd_hdr.cmd_type    = BT_CMD_SIO_ENABLE_DTR_EVENT;
    cmd_ptr->cmd_hdr.bt_app_id   = bt_app_id;
  
    cmd_ptr->cmd_msg.cmd_sio_enable_dtr_event.stream_id = stream_id;
    cmd_ptr->cmd_msg.cmd_sio_enable_dtr_event.enable_dte_ready_event 
      = enable_dte_ready_event;
    
    cmd_status = bt_cmd( cmd_ptr );
  }

  return( cmd_status );
}

/*===========================================================================

FUNCTION
  bt_cmd_sio_disable_dtr_event

DESCRIPTION

===========================================================================*/
bt_cmd_status_type bt_cmd_sio_disable_dtr_event
(
  bt_app_id_type         bt_app_id,
  sio_stream_id_type     stream_id
)
{
  bt_cmd_msg_type*    cmd_ptr;
  bt_cmd_status_type  cmd_status = BT_CS_GN_CMD_Q_FULL;

  if ( (cmd_ptr = bt_get_free_cmd()) != NULL )
  {
    cmd_ptr->cmd_hdr.cmd_type    = BT_CMD_SIO_DISABLE_DTR_EVENT;
    cmd_ptr->cmd_hdr.bt_app_id   = bt_app_id;
  
    cmd_ptr->cmd_msg.cmd_sio_in_flow.stream_id = stream_id;
    
    cmd_status = bt_cmd( cmd_ptr );
  }

  return( cmd_status );
}

/*===========================================================================

FUNCTION
  bt_cmd_sio_cd_assert

DESCRIPTION

===========================================================================*/
bt_cmd_status_type bt_cmd_sio_cd_assert
(
  bt_app_id_type         bt_app_id,
  sio_stream_id_type     stream_id
)
{
  bt_cmd_msg_type*    cmd_ptr;
  bt_cmd_status_type  cmd_status = BT_CS_GN_CMD_Q_FULL;

  if ( (cmd_ptr = bt_get_free_cmd()) != NULL )
  {
    cmd_ptr->cmd_hdr.cmd_type    = BT_CMD_SIO_CD_ASSERT;
    cmd_ptr->cmd_hdr.bt_app_id   = bt_app_id;
  
    cmd_ptr->cmd_msg.cmd_sio_cd_assert.stream_id = stream_id;
    
    cmd_status = bt_cmd( cmd_ptr );
  }

  return( cmd_status );
}

/*===========================================================================

FUNCTION
  bt_cmd_sio_cd_deassert

DESCRIPTION

===========================================================================*/
bt_cmd_status_type bt_cmd_sio_cd_deassert
(
  bt_app_id_type         bt_app_id,
  sio_stream_id_type     stream_id
)
{
  bt_cmd_msg_type*    cmd_ptr;
  bt_cmd_status_type  cmd_status = BT_CS_GN_CMD_Q_FULL;

  if ( (cmd_ptr = bt_get_free_cmd()) != NULL )
  {
    cmd_ptr->cmd_hdr.cmd_type    = BT_CMD_SIO_CD_DEASSERT;
    cmd_ptr->cmd_hdr.bt_app_id   = bt_app_id;
  
    cmd_ptr->cmd_msg.cmd_sio_cd_deassert.stream_id = stream_id;
    
    cmd_status = bt_cmd( cmd_ptr );
  }

  return( cmd_status );
}

/*===========================================================================

FUNCTION
  bt_cmd_sio_ri_assert

DESCRIPTION

===========================================================================*/
bt_cmd_status_type bt_cmd_sio_ri_assert
(
  bt_app_id_type         bt_app_id,
  sio_stream_id_type     stream_id
)
{
  bt_cmd_msg_type*    cmd_ptr;
  bt_cmd_status_type  cmd_status = BT_CS_GN_CMD_Q_FULL;

  if ( (cmd_ptr = bt_get_free_cmd()) != NULL )
  {
    cmd_ptr->cmd_hdr.cmd_type    = BT_CMD_SIO_RI_ASSERT;
    cmd_ptr->cmd_hdr.bt_app_id   = bt_app_id;
  
    cmd_ptr->cmd_msg.cmd_sio_ri_assert.stream_id = stream_id;
    
    cmd_status = bt_cmd( cmd_ptr );
  }

  return( cmd_status );
}

/*===========================================================================

FUNCTION
  bt_cmd_sio_ri_deassert

DESCRIPTION

===========================================================================*/
bt_cmd_status_type bt_cmd_sio_ri_deassert
(
  bt_app_id_type         bt_app_id,
  sio_stream_id_type     stream_id
)
{
  bt_cmd_msg_type*    cmd_ptr;
  bt_cmd_status_type  cmd_status = BT_CS_GN_CMD_Q_FULL;

  if ( (cmd_ptr = bt_get_free_cmd()) != NULL )
  {
    cmd_ptr->cmd_hdr.cmd_type    = BT_CMD_SIO_RI_DEASSERT;
    cmd_ptr->cmd_hdr.bt_app_id   = bt_app_id;
  
    cmd_ptr->cmd_msg.cmd_sio_ri_deassert.stream_id = stream_id;
    
    cmd_status = bt_cmd( cmd_ptr );
  }

  return( cmd_status );
}

/*===========================================================================

FUNCTION
  bt_cmd_sio_dsr_assert

DESCRIPTION

===========================================================================*/
bt_cmd_status_type bt_cmd_sio_dsr_assert
(
  bt_app_id_type         bt_app_id,
  sio_stream_id_type     stream_id
)
{
  bt_cmd_msg_type*    cmd_ptr;
  bt_cmd_status_type  cmd_status = BT_CS_GN_CMD_Q_FULL;

  if ( (cmd_ptr = bt_get_free_cmd()) != NULL )
  {
    cmd_ptr->cmd_hdr.cmd_type    = BT_CMD_SIO_DSR_ASSERT;
    cmd_ptr->cmd_hdr.bt_app_id   = bt_app_id;
  
    cmd_ptr->cmd_msg.cmd_sio_dsr_assert.stream_id = stream_id;
    
    cmd_status = bt_cmd( cmd_ptr );
  }

  return( cmd_status );
}

/*===========================================================================

FUNCTION
  bt_cmd_sio_dsr_deassert

DESCRIPTION

===========================================================================*/
bt_cmd_status_type bt_cmd_sio_dsr_deassert
(
  bt_app_id_type         bt_app_id,
  sio_stream_id_type     stream_id
)
{
  bt_cmd_msg_type*    cmd_ptr;
  bt_cmd_status_type  cmd_status = BT_CS_GN_CMD_Q_FULL;

  if ( (cmd_ptr = bt_get_free_cmd()) != NULL )
  {
    cmd_ptr->cmd_hdr.cmd_type    = BT_CMD_SIO_DSR_DEASSERT;
    cmd_ptr->cmd_hdr.bt_app_id   = bt_app_id;
  
    cmd_ptr->cmd_msg.cmd_sio_dsr_deassert.stream_id = stream_id;
    
    cmd_status = bt_cmd( cmd_ptr );
  }

  return( cmd_status );
}

/*===========================================================================

FUNCTION
  bt_cmd_sio_record_pkt_func_ptr

DESCRIPTION

===========================================================================*/
bt_cmd_status_type bt_cmd_sio_record_pkt_func_ptr
(
  bt_app_id_type         bt_app_id,
  sio_stream_id_type     stream_id,
  sio_vv_func_ptr_type   func_ptr
)
{
  bt_cmd_msg_type*    cmd_ptr;
  bt_cmd_status_type  cmd_status = BT_CS_GN_CMD_Q_FULL;

  if ( (cmd_ptr = bt_get_free_cmd()) != NULL )
  {
    cmd_ptr->cmd_hdr.cmd_type    = BT_CMD_SIO_RECORD_PKT_FUNC_PTR;
    cmd_ptr->cmd_hdr.bt_app_id   = bt_app_id;
  
    cmd_ptr->cmd_msg.cmd_sio_record_pkt_func_ptr.stream_id = stream_id;
    cmd_ptr->cmd_msg.cmd_sio_record_pkt_func_ptr.func_ptr  = func_ptr;
    
    cmd_status = bt_cmd( cmd_ptr );
  }

  return( cmd_status );
}

/*===========================================================================

FUNCTION
  bt_cmd_sio_record_esc_func_ptr

DESCRIPTION

===========================================================================*/
bt_cmd_status_type bt_cmd_sio_record_esc_func_ptr
(
  bt_app_id_type         bt_app_id,
  sio_stream_id_type     stream_id,
  sio_vv_func_ptr_type   func_ptr
)
{
  bt_cmd_msg_type*    cmd_ptr;
  bt_cmd_status_type  cmd_status = BT_CS_GN_CMD_Q_FULL;

  if ( (cmd_ptr = bt_get_free_cmd()) != NULL )
  {
    cmd_ptr->cmd_hdr.cmd_type    = BT_CMD_SIO_RECORD_ESC_FUNC_PTR;
    cmd_ptr->cmd_hdr.bt_app_id   = bt_app_id;
  
    cmd_ptr->cmd_msg.cmd_sio_record_esc_func_ptr.stream_id = stream_id;
    cmd_ptr->cmd_msg.cmd_sio_record_esc_func_ptr.func_ptr  = func_ptr;
    
    cmd_status = bt_cmd( cmd_ptr );
  }

  return( cmd_status );
}

/*===========================================================================

FUNCTION
  bt_cmd_sio_bt_configure

DESCRIPTION

===========================================================================*/
bt_cmd_status_type bt_cmd_sio_bt_configure
(
  bt_app_id_type         bt_app_id,
  sio_stream_id_type     stream_id,
  uint8                  baudrate,
  uint8                  format,
  uint8                  flow_ctrl,
  uint8                  xon_char,
  uint8                  xoff_char
)
{
  bt_cmd_msg_type*    cmd_ptr;
  bt_cmd_status_type  cmd_status = BT_CS_GN_CMD_Q_FULL;

  if ( (cmd_ptr = bt_get_free_cmd()) != NULL )
  {
    cmd_ptr->cmd_hdr.cmd_type    = BT_CMD_SIO_BT_CONFIGURE;
    cmd_ptr->cmd_hdr.bt_app_id   = bt_app_id;
  
    cmd_ptr->cmd_msg.cmd_sio_bt_configure.stream_id = stream_id;
    cmd_ptr->cmd_msg.cmd_sio_bt_configure.baudrate  = baudrate;
    cmd_ptr->cmd_msg.cmd_sio_bt_configure.format    = format;
    cmd_ptr->cmd_msg.cmd_sio_bt_configure.flow_ctrl = flow_ctrl;
    cmd_ptr->cmd_msg.cmd_sio_bt_configure.xon_char  = xon_char;
    cmd_ptr->cmd_msg.cmd_sio_bt_configure.xoff_char = xoff_char;
    
    cmd_status = bt_cmd( cmd_ptr );
  }

  return( cmd_status );
}

/*===========================================================================

FUNCTION
  bt_cmd_sio_bt_disconnect

DESCRIPTION

===========================================================================*/
bt_cmd_status_type bt_cmd_sio_bt_disconnect
(
  bt_app_id_type         bt_app_id,
  sio_stream_id_type     stream_id
)
{
  bt_cmd_msg_type*    cmd_ptr;
  bt_cmd_status_type  cmd_status = BT_CS_GN_CMD_Q_FULL;

  if ( (cmd_ptr = bt_get_free_cmd()) != NULL )
  {
    cmd_ptr->cmd_hdr.cmd_type    = BT_CMD_SIO_BT_DISCONNECT;
    cmd_ptr->cmd_hdr.bt_app_id   = bt_app_id;
  
    cmd_ptr->cmd_msg.cmd_sio_bt_disconnect.stream_id = stream_id;
    
    cmd_status = bt_cmd( cmd_ptr );
  }

  return( cmd_status );
}

/*===========================================================================

FUNCTION
  bt_cmd_sio_bt_flush_tx

DESCRIPTION

===========================================================================*/
bt_cmd_status_type bt_cmd_sio_bt_flush_tx
(
  bt_app_id_type         bt_app_id,
  sio_stream_id_type     stream_id,
  sio_vv_func_ptr_type   func_ptr
)
{
  bt_cmd_msg_type*    cmd_ptr;
  bt_cmd_status_type  cmd_status = BT_CS_GN_CMD_Q_FULL;

  if ( (cmd_ptr = bt_get_free_cmd()) != NULL )
  {
    cmd_ptr->cmd_hdr.cmd_type    = BT_CMD_SIO_BT_FLUSH_TX;
    cmd_ptr->cmd_hdr.bt_app_id   = bt_app_id;
  
    cmd_ptr->cmd_msg.cmd_sio_bt_flush_tx.stream_id = stream_id;
    cmd_ptr->cmd_msg.cmd_sio_bt_flush_tx.func_ptr  = func_ptr;
    
    cmd_status = bt_cmd( cmd_ptr );
  }

  return( cmd_status );
}

/*-------------------------------------------------------------------------*/
/*                            RFCOMM Commands                              */
/*-------------------------------------------------------------------------*/

/*===========================================================================

FUNCTION
  bt_cmd_rc_register

DESCRIPTION
  This function sends an RFCOMM register command to the Bluetooth driver.

===========================================================================*/
bt_cmd_status_type bt_cmd_rc_register
(
  bt_app_id_type       bt_app_id,
  uint8                server_channel,
  dsm_watermark_type*  tx_wm_ptr,
  dsm_watermark_type*  rx_wm_ptr
)
{

  bt_cmd_msg_type*    cmd_ptr;
  bt_cmd_status_type  cmd_status = BT_CS_GN_CMD_Q_FULL;

  if ( (cmd_ptr = bt_get_free_cmd()) != NULL )
  {
    cmd_ptr->cmd_hdr.cmd_type                  = BT_CMD_RC_REGISTER;
    cmd_ptr->cmd_hdr.bt_app_id                 = bt_app_id;
    cmd_ptr->cmd_msg.cmd_rc_reg.server_channel = server_channel;
    cmd_ptr->cmd_msg.cmd_rc_reg.tx_wm_ptr      = tx_wm_ptr;
    cmd_ptr->cmd_msg.cmd_rc_reg.rx_wm_ptr      = rx_wm_ptr;
  
    cmd_status = bt_cmd( cmd_ptr );
  }

  return( cmd_status );

}


/*===========================================================================

FUNCTION
  bt_cmd_rc_connect

DESCRIPTION
  This function sends an RFCOMM connect command to the Bluetooth driver.

===========================================================================*/
bt_cmd_status_type bt_cmd_rc_connect
(
  bt_app_id_type       bt_app_id,
  uint8                server_channel,
  bt_bd_addr_type*     bd_addr_ptr,
  dsm_watermark_type*  tx_wm_ptr,
  dsm_watermark_type*  rx_wm_ptr
)
{

  bt_cmd_msg_type*    cmd_ptr;
  bt_cmd_status_type  cmd_status = BT_CS_GN_CMD_Q_FULL;

  if ( (cmd_ptr = bt_get_free_cmd()) != NULL )
  {
    cmd_ptr->cmd_hdr.cmd_type                   = BT_CMD_RC_CONNECT;
    cmd_ptr->cmd_hdr.bt_app_id                  = bt_app_id;
    cmd_ptr->cmd_msg.cmd_rc_conn.server_channel = server_channel;
    cmd_ptr->cmd_msg.cmd_rc_conn.bd_addr        = *bd_addr_ptr;
    cmd_ptr->cmd_msg.cmd_rc_conn.tx_wm_ptr      = tx_wm_ptr;
    cmd_ptr->cmd_msg.cmd_rc_conn.rx_wm_ptr      = rx_wm_ptr;
  
    cmd_status = bt_cmd( cmd_ptr );
  }

  return( cmd_status );

}


/*===========================================================================

FUNCTION
  bt_cmd_rc_connect_custom_cfg

DESCRIPTION
  This function sends an RFCOMM connect with custom configuration
  command to the Bluetooth driver.

===========================================================================*/
bt_cmd_status_type bt_cmd_rc_connect_custom_cfg
(
  bt_app_id_type       bt_app_id,
  uint8                server_channel,
  bt_bd_addr_type*     bd_addr_ptr,
  dsm_watermark_type*  tx_wm_ptr,
  dsm_watermark_type*  rx_wm_ptr,
  boolean              pn_negot_done,
  boolean              rpn_negot_done,
  uint16               max_frame_size,
  uint8                priority,
  boolean              cb_flow_ctrl,
  uint8                baudrate,
  uint8                format,
  uint8                flow_ctrl,
  uint8                xon_char,
  uint8                xoff_char
)
{

  bt_cmd_msg_type*    cmd_ptr;
  bt_cmd_status_type  cmd_status = BT_CS_GN_CMD_Q_FULL;

  if ( (cmd_ptr = bt_get_free_cmd()) != NULL )
  {
    cmd_ptr->cmd_hdr.cmd_type              = BT_CMD_RC_CONN_CUST_CFG;
    cmd_ptr->cmd_hdr.bt_app_id                      = bt_app_id;
    cmd_ptr->cmd_msg.cmd_rc_conn_cfg.server_channel = server_channel;
    cmd_ptr->cmd_msg.cmd_rc_conn_cfg.bd_addr        = *bd_addr_ptr;
    cmd_ptr->cmd_msg.cmd_rc_conn_cfg.tx_wm_ptr      = tx_wm_ptr;
    cmd_ptr->cmd_msg.cmd_rc_conn_cfg.rx_wm_ptr      = rx_wm_ptr;
    cmd_ptr->cmd_msg.cmd_rc_conn_cfg.pn_negot_done  = pn_negot_done;
    cmd_ptr->cmd_msg.cmd_rc_conn_cfg.rpn_negot_done = rpn_negot_done;
    cmd_ptr->cmd_msg.cmd_rc_conn_cfg.max_frame_size = max_frame_size;
    cmd_ptr->cmd_msg.cmd_rc_conn_cfg.priority       = priority;
    cmd_ptr->cmd_msg.cmd_rc_conn_cfg.cb_flow_ctrl   = cb_flow_ctrl;
    cmd_ptr->cmd_msg.cmd_rc_conn_cfg.baudrate       = baudrate;
    cmd_ptr->cmd_msg.cmd_rc_conn_cfg.format         = format;
    cmd_ptr->cmd_msg.cmd_rc_conn_cfg.flow_ctrl      = flow_ctrl;
    cmd_ptr->cmd_msg.cmd_rc_conn_cfg.xon_char       = xon_char;
    cmd_ptr->cmd_msg.cmd_rc_conn_cfg.xoff_char      = xoff_char;
  
    cmd_status = bt_cmd( cmd_ptr );
  }

  return( cmd_status );

}


/*===========================================================================

FUNCTION
  bt_cmd_rc_tx_data

DESCRIPTION
  This function sends an RFCOMM transmit data command to the Bluetooth
  driver.

===========================================================================*/
bt_cmd_status_type bt_cmd_rc_tx_data
(
  bt_app_id_type  bt_app_id,
  uint16          port_id
)
{

  bt_cmd_msg_type*    cmd_ptr;
  bt_cmd_status_type  cmd_status = BT_CS_GN_CMD_Q_FULL;

  if ( (cmd_ptr = bt_get_free_cmd()) != NULL )
  {
    cmd_ptr->cmd_hdr.cmd_type       = BT_CMD_RC_TX_DATA;
    cmd_ptr->cmd_hdr.bt_app_id      = bt_app_id;
    cmd_ptr->cmd_msg.cmd_rc_txd.pid = port_id;
  
    cmd_status = bt_cmd( cmd_ptr );
  }

  return( cmd_status );

}


/*===========================================================================

FUNCTION
  bt_cmd_rc_set_port_rx_fc

DESCRIPTION
  This function sends an RFCOMM set port receive flow control command
  to the Bluetooth driver.

===========================================================================*/
bt_cmd_status_type bt_cmd_rc_set_port_rx_fc
(
  bt_app_id_type  bt_app_id,
  uint16          port_id,
  boolean         enable_flow
)
{

  bt_cmd_msg_type*    cmd_ptr;
  bt_cmd_status_type  cmd_status = BT_CS_GN_CMD_Q_FULL;

  if ( (cmd_ptr = bt_get_free_cmd()) != NULL )
  {
    cmd_ptr->cmd_hdr.cmd_type     = BT_CMD_RC_SET_PORT_RX_FC;
    cmd_ptr->cmd_hdr.bt_app_id                 = bt_app_id;
    cmd_ptr->cmd_msg.cmd_rc_sprxfc.pid         = port_id;
    cmd_ptr->cmd_msg.cmd_rc_sprxfc.enable_flow = enable_flow;
  
    cmd_status = bt_cmd( cmd_ptr );
  }

  return( cmd_status );

}


/*===========================================================================

FUNCTION
  bt_cmd_rc_set_session_rx_fc

DESCRIPTION
  This function sends an RFCOMM set session receive flow control command
  to the Bluetooth driver.

===========================================================================*/
bt_cmd_status_type bt_cmd_rc_set_session_rx_fc
(
  bt_app_id_type  bt_app_id,
  uint16          port_id,
  boolean         enable_flow
)
{

  bt_cmd_msg_type*    cmd_ptr;
  bt_cmd_status_type  cmd_status = BT_CS_GN_CMD_Q_FULL;

  if ( (cmd_ptr = bt_get_free_cmd()) != NULL )
  {
    cmd_ptr->cmd_hdr.cmd_type = BT_CMD_RC_SET_SESSION_RX_FC;
    cmd_ptr->cmd_hdr.bt_app_id                 = bt_app_id;
    cmd_ptr->cmd_msg.cmd_rc_ssrxfc.pid         = port_id;
    cmd_ptr->cmd_msg.cmd_rc_ssrxfc.enable_flow = enable_flow;
  
    cmd_status = bt_cmd( cmd_ptr );
  }

  return( cmd_status );

}


/*===========================================================================

FUNCTION
  bt_cmd_rc_write_modem_status

DESCRIPTION
  This function sends an RFCOMM write modem status command to the
  Bluetooth driver.

===========================================================================*/
bt_cmd_status_type bt_cmd_rc_write_modem_status
(
  bt_app_id_type  bt_app_id,
  uint16          port_id,
  uint8           ms_valid_mask,
  uint8           modem_status,
  boolean         break_present,
  uint8           break_length    /*  Units of 200ms, max 15  */
)
{

  bt_cmd_msg_type*    cmd_ptr;
  bt_cmd_status_type  cmd_status = BT_CS_GN_CMD_Q_FULL;

  if ( (cmd_ptr = bt_get_free_cmd()) != NULL )
  {
    cmd_ptr->cmd_hdr.cmd_type                 = BT_CMD_RC_WRITE_MS;
    cmd_ptr->cmd_hdr.bt_app_id                = bt_app_id;
    cmd_ptr->cmd_msg.cmd_rc_wms.pid           = port_id;
    cmd_ptr->cmd_msg.cmd_rc_wms.ms_valid_mask = ms_valid_mask;
    cmd_ptr->cmd_msg.cmd_rc_wms.modem_status  = modem_status;
    cmd_ptr->cmd_msg.cmd_rc_wms.break_present = break_present;
    cmd_ptr->cmd_msg.cmd_rc_wms.break_length  = break_length;
  
    cmd_status = bt_cmd( cmd_ptr );
  }

  return( cmd_status );

}


/*===========================================================================

FUNCTION
  bt_cmd_rc_write_sp_config
  
DESCRIPTION
  This function sends an RFCOMM Write Serial Port Configuration command
  to the Bluetooth driver.

===========================================================================*/
bt_cmd_status_type bt_cmd_rc_write_sp_config
(
  bt_app_id_type  bt_app_id,
  uint16          port_id,
  uint8           baudrate,
  uint8           format,     /*  Corresponds to value3 octet in RPN.  */
  uint8           flow_ctrl,
  uint8           xon_char,
  uint8           xoff_char
)
{

  bt_cmd_msg_type*    cmd_ptr;
  bt_cmd_status_type  cmd_status = BT_CS_GN_CMD_Q_FULL;

  if ( (cmd_ptr = bt_get_free_cmd()) != NULL )
  {
    cmd_ptr->cmd_hdr.cmd_type              = BT_CMD_RC_WRITE_SP_CFG;
    cmd_ptr->cmd_hdr.bt_app_id             = bt_app_id;
    cmd_ptr->cmd_msg.cmd_rc_wspc.pid       = port_id;
    cmd_ptr->cmd_msg.cmd_rc_wspc.baudrate  = baudrate;
    cmd_ptr->cmd_msg.cmd_rc_wspc.format    = format;
    cmd_ptr->cmd_msg.cmd_rc_wspc.flow_ctrl = flow_ctrl;
    cmd_ptr->cmd_msg.cmd_rc_wspc.xon_char  = xon_char;
    cmd_ptr->cmd_msg.cmd_rc_wspc.xoff_char = xoff_char;
  
    cmd_status = bt_cmd( cmd_ptr );
  }
  
  return( cmd_status );

}


/*===========================================================================

FUNCTION
  bt_cmd_rc_line_error

DESCRIPTION
  This function sends an RFCOMM line error command to the Bluetooth
  driver.

===========================================================================*/
bt_cmd_status_type bt_cmd_rc_line_error
(
  bt_app_id_type  bt_app_id,
  uint16          port_id,
  boolean         overrun_error,
  boolean         parity_error,
  boolean         framing_error
)
{

  bt_cmd_msg_type*    cmd_ptr;
  bt_cmd_status_type  cmd_status = BT_CS_GN_CMD_Q_FULL;

  if ( (cmd_ptr = bt_get_free_cmd()) != NULL )
  {
    cmd_ptr->cmd_hdr.cmd_type                  = BT_CMD_RC_LINE_ERROR;
    cmd_ptr->cmd_hdr.bt_app_id                 = bt_app_id;
    cmd_ptr->cmd_msg.cmd_rc_lerr.pid           = port_id;
    cmd_ptr->cmd_msg.cmd_rc_lerr.overrun_error = overrun_error;
    cmd_ptr->cmd_msg.cmd_rc_lerr.parity_error  = parity_error;
    cmd_ptr->cmd_msg.cmd_rc_lerr.framing_error = framing_error;
  
    cmd_status = bt_cmd( cmd_ptr );
  }

  return( cmd_status );

}


/*===========================================================================

FUNCTION
  bt_cmd_rc_ping

DESCRIPTION
  This function sends an RFCOMM ping command to the Bluetooth driver.

===========================================================================*/
bt_cmd_status_type bt_cmd_rc_ping
(
  bt_app_id_type  bt_app_id,
  uint16          pid,
  uint16          byte_count,
  uint8           start_value,
  uint8           increment
)
{

  bt_cmd_msg_type*    cmd_ptr;
  bt_cmd_status_type  cmd_status = BT_CS_GN_CMD_Q_FULL;

  if ( (cmd_ptr = bt_get_free_cmd()) != NULL )
  {
    cmd_ptr->cmd_hdr.cmd_type                = BT_CMD_RC_PING;
    cmd_ptr->cmd_hdr.bt_app_id               = bt_app_id;
    cmd_ptr->cmd_msg.cmd_rc_ping.pid         = pid;
    cmd_ptr->cmd_msg.cmd_rc_ping.byte_count  = byte_count;
    cmd_ptr->cmd_msg.cmd_rc_ping.start_value = start_value;
    cmd_ptr->cmd_msg.cmd_rc_ping.increment   = increment;
  
    cmd_status = bt_cmd( cmd_ptr );
  }

  return( cmd_status );

}


/*===========================================================================

FUNCTION
  bt_cmd_rc_disconnect

DESCRIPTION
  This function sends an RFCOMM disconnect command to the Bluetooth
  driver.

===========================================================================*/
bt_cmd_status_type bt_cmd_rc_disconnect
(
  bt_app_id_type  bt_app_id,
  uint16          port_id
)
{

  bt_cmd_msg_type*    cmd_ptr;
  bt_cmd_status_type  cmd_status = BT_CS_GN_CMD_Q_FULL;

  if ( (cmd_ptr = bt_get_free_cmd()) != NULL )
  {
    cmd_ptr->cmd_hdr.cmd_type        = BT_CMD_RC_DISCONNECT;
    cmd_ptr->cmd_hdr.bt_app_id       = bt_app_id;
    cmd_ptr->cmd_msg.cmd_rc_disc.pid = port_id;
  
    cmd_status = bt_cmd( cmd_ptr );
  }

  return( cmd_status );

}


/*===========================================================================

FUNCTION
  bt_cmd_rc_unregister

DESCRIPTION
  This function sends an RFCOMM unregister command to the Bluetooth
  driver.

===========================================================================*/
bt_cmd_status_type bt_cmd_rc_unregister
(
  bt_app_id_type  bt_app_id,
  uint8           server_channel
)
{

  bt_cmd_msg_type*    cmd_ptr;
  bt_cmd_status_type  cmd_status = BT_CS_GN_CMD_Q_FULL;

  if ( (cmd_ptr = bt_get_free_cmd()) != NULL )
  {
    cmd_ptr->cmd_hdr.cmd_type  = BT_CMD_RC_UNREGISTER;
    cmd_ptr->cmd_hdr.bt_app_id = bt_app_id;
    cmd_ptr->cmd_msg.cmd_rc_unreg.server_channel = server_channel;
  
    cmd_status = bt_cmd( cmd_ptr );
  }

  return( cmd_status );

}

/*===========================================================================

FUNCTION
  bt_cmd_rc_cancel_connect

DESCRIPTION
  This function cancels an RFCOMM connection in progress. It will also
  disconnect the session if already connected.

===========================================================================*/
bt_cmd_status_type bt_cmd_rc_cancel_connect
(
  bt_app_id_type       bt_app_id,
  uint8                server_channel,
  bt_bd_addr_type*     bd_addr_ptr
)
{

  bt_cmd_msg_type*    cmd_ptr;
  bt_cmd_status_type  cmd_status = BT_CS_GN_CMD_Q_FULL;

  if ( (cmd_ptr = bt_get_free_cmd()) != NULL )
  {
    cmd_ptr->cmd_hdr.cmd_type              = BT_CMD_RC_CANCEL_CONNECT;
    cmd_ptr->cmd_hdr.bt_app_id                       = bt_app_id;
    cmd_ptr->cmd_msg.cmd_rc_cncl_conn.server_channel = server_channel;
    cmd_ptr->cmd_msg.cmd_rc_cncl_conn.bd_addr        = *bd_addr_ptr;
  
    cmd_status = bt_cmd( cmd_ptr );
  }

  return( cmd_status );

}


/*-------------------------------------------------------------------------*/
/*                             L2CAP Commands                              */
/*-------------------------------------------------------------------------*/

/*===========================================================================

FUNCTION
  bt_cmd_l2_set_params

DESCRIPTION
  This function sends an L2CAP set parameters command to the Bluetooth
  driver.

===========================================================================*/
bt_cmd_status_type bt_cmd_l2_set_params
(
  bt_app_id_type       bt_app_id,
  uint16               cid,
  uint16               protocol_id,
  uint16               out_flush_to,
  uint16               in_mtu,
  bt_qos_service_type  service_type,
  dword                token_rate,
  dword                token_bucket_size,
  dword                peak_bandwidth,
  dword                latency,
  dword                delay_variation
)
{

  bt_cmd_msg_type*    cmd_ptr;
  bt_cmd_status_type  cmd_status = BT_CS_GN_CMD_Q_FULL;
  bt_l2_psm_param_type      *l2_psm_params;

  if ( (cmd_ptr = bt_get_free_cmd()) != NULL )
  {
    cmd_ptr->cmd_hdr.cmd_type          = BT_CMD_L2_SET_PARAMS;
    cmd_ptr->cmd_hdr.bt_app_id         = bt_app_id;
    cmd_ptr->cmd_msg.cmd_l2_params.cid = cid;
    cmd_ptr->cmd_msg.cmd_l2_params.psm = protocol_id;

    l2_psm_params = &cmd_ptr->cmd_msg.cmd_l2_params.psm_params;

    l2_psm_params->out_qos.token_bucket_size = token_bucket_size;

    l2_psm_params->out_flush_to              = out_flush_to;
    l2_psm_params->out_qos.latency           = latency;
    l2_psm_params->in_mtu                    = in_mtu;
    l2_psm_params->out_qos.flags             = 0;
    l2_psm_params->out_qos.peak_bandwidth    = peak_bandwidth;
    l2_psm_params->out_qos.service_type      = service_type;
    l2_psm_params->out_qos.token_rate        = token_rate;
    l2_psm_params->out_qos.delay_variation   = delay_variation;


    cmd_status = bt_cmd( cmd_ptr );
  }

  return( cmd_status );

}


/*===========================================================================

FUNCTION
  bt_cmd_l2_register

DESCRIPTION
  This function sends an L2CAP register command to the Bluetooth driver.

===========================================================================*/
bt_cmd_status_type bt_cmd_l2_register
(
  bt_app_id_type  bt_app_id,
  uint16          protocol_id
)
{

  bt_cmd_msg_type*    cmd_ptr;
  bt_cmd_status_type  cmd_status = BT_CS_GN_CMD_Q_FULL;

  if ( (cmd_ptr = bt_get_free_cmd()) != NULL )
  {
    cmd_ptr->cmd_hdr.cmd_type       = BT_CMD_L2_REGISTER;
    cmd_ptr->cmd_hdr.bt_app_id      = bt_app_id;
    cmd_ptr->cmd_msg.cmd_l2_reg.psm = protocol_id;
  
    cmd_status = bt_cmd( cmd_ptr );
  }

  return( cmd_status );

}


/*===========================================================================

FUNCTION
  bt_cmd_l2_connect

DESCRIPTION
  This function sends an L2CAP connect command to the Bluetooth driver.

===========================================================================*/
bt_cmd_status_type bt_cmd_l2_connect
(
  bt_app_id_type    bt_app_id,
  uint16            protocol_id,
  bt_bd_addr_type*  bd_addr_ptr
)
{

  bt_cmd_msg_type*    cmd_ptr;
  bt_cmd_status_type  cmd_status = BT_CS_GN_CMD_Q_FULL;

  if ( (cmd_ptr = bt_get_free_cmd()) != NULL )
  {
    cmd_ptr->cmd_hdr.cmd_type            = BT_CMD_L2_CONNECT;
    cmd_ptr->cmd_hdr.bt_app_id           = bt_app_id;
    cmd_ptr->cmd_msg.cmd_l2_conn.psm     = protocol_id;
    cmd_ptr->cmd_msg.cmd_l2_conn.bd_addr = *bd_addr_ptr;
  
    cmd_status = bt_cmd( cmd_ptr );
  }

  return( cmd_status );

}


/*===========================================================================

FUNCTION
  bt_cmd_l2_write

DESCRIPTION
  This function sends an L2CAP write command to the Bluetooth driver.

===========================================================================*/
bt_cmd_status_type bt_cmd_l2_write
(
  bt_app_id_type  bt_app_id,
  uint16          channel_id,
  dsm_item_type*  dsm_ptr,
  void*           frame_handle
)
{

  bt_cmd_msg_type*    cmd_ptr;
  bt_cmd_status_type  cmd_status = BT_CS_GN_CMD_Q_FULL;

  if ( (cmd_ptr = bt_get_free_cmd()) != NULL )
  {
    cmd_ptr->cmd_hdr.cmd_type             = BT_CMD_L2_WRITE;
    cmd_ptr->cmd_hdr.bt_app_id            = bt_app_id;
    cmd_ptr->cmd_msg.cmd_l2_write.cid     = channel_id;
    cmd_ptr->cmd_msg.cmd_l2_write.dsm_ptr = dsm_ptr;
    cmd_ptr->cmd_msg.cmd_l2_write.frame_handle = frame_handle;
    cmd_status = bt_cmd( cmd_ptr );
  }

  return( cmd_status );

}


/*===========================================================================

FUNCTION
  bt_cmd_l2_ping

DESCRIPTION
  This function sends an L2CAP Ping command to the Bluetooth driver.

===========================================================================*/
bt_cmd_status_type bt_cmd_l2_ping
(
  bt_app_id_type    bt_app_id,
  bt_bd_addr_type*  bd_addr_ptr,
  uint16            length,
  uint8*            data
)
{

  bt_cmd_msg_type*    cmd_ptr;
  bt_cmd_status_type  cmd_status = BT_CS_GN_CMD_Q_FULL;
  uint16              len = MIN( length, BT_L2_MAX_ECHO_DATA_LEN );

  if ( (cmd_ptr = bt_get_free_cmd()) != NULL )
  {
    cmd_ptr->cmd_hdr.cmd_type            = BT_CMD_L2_PING;
    cmd_ptr->cmd_hdr.bt_app_id           = bt_app_id;
    cmd_ptr->cmd_msg.cmd_l2_ping.bd_addr = *bd_addr_ptr;
    cmd_ptr->cmd_msg.cmd_l2_ping.length  = len;
    memcpy( (uint8*)(cmd_ptr->cmd_msg.cmd_l2_ping.data), data, len );
  
    cmd_status = bt_cmd( cmd_ptr );
  }

  return( cmd_status );

}


/*===========================================================================

FUNCTION
  bt_cmd_l2_disconnect

DESCRIPTION
  This function sends an L2CAP disconnect command to the Bluetooth driver.

===========================================================================*/
bt_cmd_status_type bt_cmd_l2_disconnect
(
  bt_app_id_type  bt_app_id,
  uint16          channel_id
)
{

  bt_cmd_msg_type*    cmd_ptr;
  bt_cmd_status_type  cmd_status = BT_CS_GN_CMD_Q_FULL;

  if ( (cmd_ptr = bt_get_free_cmd()) != NULL )
  {
    cmd_ptr->cmd_hdr.cmd_type        = BT_CMD_L2_DISCONNECT;
    cmd_ptr->cmd_hdr.bt_app_id       = bt_app_id;
    cmd_ptr->cmd_msg.cmd_l2_disc.cid = channel_id;
  
    cmd_status = bt_cmd( cmd_ptr );
  }

  return( cmd_status );

}


/*===========================================================================

FUNCTION
  bt_cmd_l2_disconn_immediate

DESCRIPTION
  This function sends an L2CAP disconnect command to the Bluetooth driver.
  If this is the last L2 connection, this will disconnect the ACL link
  immediately rather than waiting for a timeout.

===========================================================================*/
bt_cmd_status_type bt_cmd_l2_disconn_immediate
(
  bt_app_id_type  bt_app_id,
  uint16          channel_id
)
{

  bt_cmd_msg_type*    cmd_ptr;
  bt_cmd_status_type  cmd_status = BT_CS_GN_CMD_Q_FULL;

  if ( (cmd_ptr = bt_get_free_cmd()) != NULL )
  {
    cmd_ptr->cmd_hdr.cmd_type        = BT_CMD_L2_DISCONN_IMMEDIATE;
    cmd_ptr->cmd_hdr.bt_app_id       = bt_app_id;
    cmd_ptr->cmd_msg.cmd_l2_disc.cid = channel_id;
  
    cmd_status = bt_cmd( cmd_ptr );
  }

  return( cmd_status );

}


/*===========================================================================

FUNCTION
  bt_cmd_l2_unregister

DESCRIPTION
  This function sends an L2CAP unregister command to the Bluetooth driver.

===========================================================================*/
bt_cmd_status_type bt_cmd_l2_unregister
(
  bt_app_id_type  bt_app_id,
  uint16          protocol_id
)
{

  bt_cmd_msg_type*    cmd_ptr;
  bt_cmd_status_type  cmd_status = BT_CS_GN_CMD_Q_FULL;

  if ( (cmd_ptr = bt_get_free_cmd()) != NULL )
  {
    cmd_ptr->cmd_hdr.cmd_type         = BT_CMD_L2_UNREGISTER;
    cmd_ptr->cmd_hdr.bt_app_id        = bt_app_id;
    cmd_ptr->cmd_msg.cmd_l2_unreg.psm = protocol_id;
  
    cmd_status = bt_cmd( cmd_ptr );
  }
  
  return( cmd_status );

}

/*===========================================================================

FUNCTION
  bt_cmd_l2_connect_immediate_cid

DESCRIPTION
  This function sends an L2CAP connect command to the Bluetooth driver, and
  immediately returns the CID value which will be used by L2 for this
  connection

===========================================================================*/
bt_cmd_status_type bt_cmd_l2_connect_immediate_cid
(
  bt_app_id_type    bt_app_id,
  uint16            protocol_id,
  bt_bd_addr_type*  bd_addr_ptr,
  uint16*           cid
)
{

  bt_cmd_msg_type*    cmd_ptr;
  bt_cmd_status_type  cmd_status = BT_CS_GN_CMD_Q_FULL;

  if ( (cmd_ptr = bt_get_free_cmd()) != NULL )
  {
    *cid = bt_l2_get_free_cid();

    cmd_ptr->cmd_hdr.cmd_type                    = BT_CMD_L2_CONNECT_IMMEDIATE_CID;
    cmd_ptr->cmd_hdr.bt_app_id                   = bt_app_id;
    cmd_ptr->cmd_msg.cmd_l2_conn_imd_cid.psm     = protocol_id;
    cmd_ptr->cmd_msg.cmd_l2_conn_imd_cid.bd_addr = *bd_addr_ptr;
    cmd_ptr->cmd_msg.cmd_l2_conn_imd_cid.cid     = *cid;
  
    cmd_status = bt_cmd( cmd_ptr );
  }

  return( cmd_status );

}

/*===========================================================================

FUNCTION
  bt_cmd_l2_disable_timeout

DESCRIPTION
  This function commands L2 to not automatically disconnect a device after
  a timeout period if no data has been sent since the beginning of the
  connection.

===========================================================================*/
bt_cmd_status_type bt_cmd_l2_disable_timeout
(
  bt_app_id_type    bt_app_id,
  uint16            cid
)
{

  bt_cmd_msg_type*    cmd_ptr;
  bt_cmd_status_type  cmd_status = BT_CS_GN_CMD_Q_FULL;

  if ( (cmd_ptr = bt_get_free_cmd()) != NULL )
  {
    cmd_ptr->cmd_hdr.cmd_type                    = BT_CMD_L2_DISABLE_TIMEOUT;
    cmd_ptr->cmd_hdr.bt_app_id                   = bt_app_id;
    cmd_ptr->cmd_msg.cmd_l2_disable_timeout.cid  = cid;
  
    cmd_status = bt_cmd( cmd_ptr );
  }

  return( cmd_status );

}

/*===========================================================================

FUNCTION
  bt_cmd_l2_get_num_conn

DESCRIPTION
  This function returns the number of L2 connections to a particular BD Addr.
  This will return 0 if the ACL is connected, but there are no L2 channels 
  open. Note that this does not count connections comming up or going down, 
  only the active connections.
  The return value is immediate, and is stored in the pointer to the 
  input variable num_conn.

===========================================================================*/
bt_cmd_status_type bt_cmd_l2_get_num_conn
(
  bt_app_id_type    bt_app_id,
  bt_bd_addr_type*  bd_addr_ptr,
  uint16*           num_conn
)
{

  *num_conn = bt_l2_get_num_conn( bd_addr_ptr );

  return( BT_CS_GN_SUCCESS );

}

/*-------------------------------------------------------------------------*/
/*                       Radio Manager (RM) Commands                       */
/*-------------------------------------------------------------------------*/

/*===========================================================================

FUNCTION
  bt_cmd_rm_update_bt_module_state

DESCRIPTION
  This function processes the RM update BT module state cmd.

===========================================================================*/
bt_cmd_status_type bt_cmd_rm_update_bt_module_state
(
  bt_app_id_type        bt_app_id,
  uint32                module,
  bt_module_state_type  state
)
{

  bt_cmd_msg_type*    cmd_ptr;
  bt_cmd_status_type  cmd_status = BT_CS_GN_CMD_Q_FULL;

  if ( (cmd_ptr = bt_get_free_cmd()) != NULL )
  {
    cmd_ptr->cmd_hdr.cmd_type  = BT_CMD_RM_UPDATE_BT_MODULE_ST;
    cmd_ptr->cmd_hdr.bt_app_id = bt_app_id;
    
    cmd_ptr->cmd_msg.cmd_rm_upd_bms.module = module;
    cmd_ptr->cmd_msg.cmd_rm_upd_bms.state = state;

    cmd_status = bt_cmd( cmd_ptr );
  }

  return( cmd_status );

}

/*===========================================================================

FUNCTION
  bt_cmd_rm_set_local_info

DESCRIPTION
  This function sends a RM Set Local Information command to the
  Bluetooth driver.

===========================================================================*/
bt_cmd_status_type bt_cmd_rm_set_local_info
(
  bt_app_id_type  bt_app_id,
  bt_cod_type*    class_of_device_ptr,
  char*           bt_name_str_ptr
)
{

  bt_cmd_msg_type*    cmd_ptr;
  bt_cmd_status_type  cmd_status = BT_CS_GN_CMD_Q_FULL;

  if ( (cmd_ptr = bt_get_free_cmd()) != NULL )
  {
    cmd_ptr->cmd_hdr.cmd_type  = BT_CMD_RM_SET_LOCAL_INFO;
    cmd_ptr->cmd_hdr.bt_app_id = bt_app_id;

    if ( class_of_device_ptr != NULL )
    {
      cmd_ptr->cmd_msg.cmd_rm_setif.cod_valid = TRUE;
      cmd_ptr->cmd_msg.cmd_rm_setif.class_of_device =
        *class_of_device_ptr;
    }
    else
    {
      cmd_ptr->cmd_msg.cmd_rm_setif.cod_valid = FALSE;
    }
    
    if ( bt_name_str_ptr != NULL )
    {
      cmd_ptr->cmd_msg.cmd_rm_setif.bt_name_valid = TRUE;
      BT_STRCPY( (char *)cmd_ptr->cmd_msg.cmd_rm_setif.bt_name_str,
                 bt_name_str_ptr, 
                 sizeof (cmd_ptr->cmd_msg.cmd_rm_setif.bt_name_str) );
    }
    else
    {
      cmd_ptr->cmd_msg.cmd_rm_setif.bt_name_valid = FALSE;
    }

    cmd_status = bt_cmd( cmd_ptr );
  }

  /*  Update COD shadow copy in RAM right now to   */
  /*  make COD get and set operation look atomic.  */
  if ( (class_of_device_ptr != NULL) && (cmd_status == BT_CS_GN_PENDING) )
  {
#ifndef FEATURE_BT_SOC
    bt_lm_data.attributes.class_of_device = *class_of_device_ptr;
#else
    bt_class_of_device = *class_of_device_ptr;
#endif /* FEATURE_BT_SOC */
  }

  return( cmd_status );

}


#ifdef BT_SWDEV_2_1_EIR
/*===========================================================================

FUNCTION
  bt_cmd_rm_set_local_info_ext

DESCRIPTION
  This function sends a RM Set Local Information(extended) command to the
  Bluetooth driver. Extends Set Local information to include short name
  (required for Lisbon - EIR)

===========================================================================*/
bt_cmd_status_type bt_cmd_rm_set_local_info_ext
(
  bt_app_id_type  bt_app_id,
  bt_cod_type*    class_of_device_ptr,
  char*           bt_name_str_ptr,
  char*           bt_short_name_str_ptr
)
{
  bt_cmd_msg_type*    cmd_ptr;
  bt_cmd_status_type  cmd_status = BT_CS_GN_CMD_Q_FULL;

  MSG_FATAL("***zzg bt_cmd_rm_set_local_info_ext***", 0, 0, 0);

  if ( (cmd_ptr = bt_get_free_cmd()) != NULL )
  {
    cmd_ptr->cmd_hdr.cmd_type  = BT_CMD_RM_SET_LOCAL_INFO_EXT;
    cmd_ptr->cmd_hdr.bt_app_id = bt_app_id;

    /* Class of Device */
    if ( class_of_device_ptr != NULL )
    {
      cmd_ptr->cmd_msg.cmd_rm_setif_ext.cod_valid = TRUE;
      cmd_ptr->cmd_msg.cmd_rm_setif_ext.class_of_device =
        *class_of_device_ptr;
    }
    else
    {
      cmd_ptr->cmd_msg.cmd_rm_setif_ext.cod_valid = FALSE;
    }

    /* BT Device Name */
    if ( bt_name_str_ptr != NULL )
    {
      cmd_ptr->cmd_msg.cmd_rm_setif_ext.bt_name_valid = TRUE;
      BT_STRCPY( (char *)cmd_ptr->cmd_msg.cmd_rm_setif_ext.bt_name_str,
                 bt_name_str_ptr, 
                 sizeof (cmd_ptr->cmd_msg.cmd_rm_setif_ext.bt_name_str) );
    }
    else
    {
      cmd_ptr->cmd_msg.cmd_rm_setif_ext.bt_name_valid = FALSE;
    }

    /* Short Name */

    if ( bt_short_name_str_ptr != NULL )
    {
      cmd_ptr->cmd_msg.cmd_rm_setif_ext.bt_short_name_valid = TRUE;
      BT_STRCPY( (char *)cmd_ptr->cmd_msg.cmd_rm_setif_ext.bt_short_name_str,
                 bt_short_name_str_ptr, 
                 sizeof (cmd_ptr->cmd_msg.cmd_rm_setif_ext.bt_short_name_str) );
    }
    else
    {
      cmd_ptr->cmd_msg.cmd_rm_setif_ext.bt_short_name_valid = FALSE;
    }
	

	

    cmd_status = bt_cmd( cmd_ptr );
  }

  /*  Update COD shadow copy in RAM right now to   */
  /*  make COD get and set operation look atomic.  */
  if ( (class_of_device_ptr != NULL) && (cmd_status == BT_CS_GN_PENDING) )
  {
#ifndef FEATURE_BT_SOC
    bt_lm_data.attributes.class_of_device = *class_of_device_ptr;
#else
    bt_class_of_device = *class_of_device_ptr;
#endif /* FEATURE_BT_SOC */
  }

  return( cmd_status );

}
#endif /* BT_SWDEV_2_1_EIR */


#ifdef BT_SWDEV_2_1_EIR
/*===========================================================================

FUNCTION
  bt_cmd_rm_wr_eir_data

DESCRIPTION
  This function sends a RM Write EIR data command to the Bluetooth driver

===========================================================================*/
bt_cmd_status_type bt_cmd_rm_wr_eir_data
(
  bt_app_id_type  bt_app_id,
  uint8           eir_data_type,
  uint8           eir_data_size,
  const uint8*    eir_data_ptr
)
{
  bt_cmd_msg_type*    cmd_ptr;
  bt_cmd_status_type  cmd_status = BT_CS_RM_BAD_PARAM;

  if ( eir_data_ptr != NULL &&
       eir_data_size <= BT_MAX_EIR_DATA_SIZE )
  {
    cmd_status = BT_CS_GN_CMD_Q_FULL;
    if ( (cmd_ptr = bt_get_free_cmd()) != NULL )
    {
      cmd_ptr->cmd_hdr.cmd_type  = BT_CMD_RM_WR_EIR_DATA;
      cmd_ptr->cmd_hdr.bt_app_id = bt_app_id;

      cmd_ptr->cmd_msg.cmd_rm_wr_eir.eir_data_type = eir_data_type;
      cmd_ptr->cmd_msg.cmd_rm_wr_eir.eir_data_size = eir_data_size;
      memcpy( (void *)cmd_ptr->cmd_msg.cmd_rm_wr_eir.eir_data,
              (const void *)eir_data_ptr,
              eir_data_size);

      cmd_status = bt_cmd( cmd_ptr );
    }
  }

  return( cmd_status );
}
#endif /* BT_SWDEV_2_1_EIR */


#ifdef BT_SWDEV_2_1_EIR
/*===========================================================================

FUNCTION
  bt_cmd_rm_wr_eir_uuid

DESCRIPTION
  This function sends a RM Write EIR UUID command to the Bluetooth driver
  (Required for Lisbon - EIR)

===========================================================================*/
bt_cmd_status_type bt_cmd_rm_wr_eir_uuid
(
  bt_app_id_type  bt_app_id,
  bt_uuid_type    uuid_type, 
  boolean         uuid_list_complete,
  uint8           bt_uuid_data_size,
  uint8*          bt_uuid_data_ptr
)
{
  bt_cmd_msg_type*    cmd_ptr;
  bt_cmd_status_type  cmd_status = BT_CS_RM_BAD_PARAM;

  if ( bt_uuid_data_ptr != NULL &&
       bt_uuid_data_size <= BT_MAX_EIR_UUID_DATA_SIZE ) 
  {
    cmd_status = BT_CS_GN_CMD_Q_FULL;
    if ( (cmd_ptr = bt_get_free_cmd()) != NULL )
    {
      cmd_ptr->cmd_hdr.cmd_type  = BT_CMD_RM_WR_EIR_UUID;
      cmd_ptr->cmd_hdr.bt_app_id = bt_app_id;

      cmd_ptr->cmd_msg.cmd_rm_wr_uuid.uuid_type          = uuid_type;
      cmd_ptr->cmd_msg.cmd_rm_wr_uuid.uuid_list_complete = uuid_list_complete;
      cmd_ptr->cmd_msg.cmd_rm_wr_uuid.uuid_data_size     = bt_uuid_data_size;
      memcpy( (void *)cmd_ptr->cmd_msg.cmd_rm_wr_uuid.uuid_data,
              (void *)bt_uuid_data_ptr,
              bt_uuid_data_size);

      cmd_status = bt_cmd( cmd_ptr );
    }
  }

  return( cmd_status );
}
#endif /* BT_SWDEV_2_1_EIR */


/*===========================================================================

FUNCTION
  bt_cmd_rm_config_application

DESCRIPTION
  This function sends a RM Configure Application command to the Bluetooth
  driver.

===========================================================================*/
bt_cmd_status_type bt_cmd_rm_config_application
(
  bt_app_id_type         bt_app_id,
  boolean                remove_app_config,
  bt_rm_act_vs_pwr_type  page_scan_act_vs_pwr,
  bt_rm_act_vs_pwr_type  inq_scan_act_vs_pwr,
  boolean                deliver_dev_updates
)
{

  bt_cmd_msg_type*    cmd_ptr;
  bt_cmd_status_type  cmd_status = BT_CS_GN_CMD_Q_FULL;

  if ( (cmd_ptr = bt_get_free_cmd()) != NULL )
  {
    cmd_ptr->cmd_hdr.cmd_type  = BT_CMD_RM_CONFIG_APPLICATION;
    cmd_ptr->cmd_hdr.bt_app_id = bt_app_id;
    
    cmd_ptr->cmd_msg.cmd_rm_apcfg.remove_app_config =
      remove_app_config;
    cmd_ptr->cmd_msg.cmd_rm_apcfg.page_scan_act_vs_pwr =
      page_scan_act_vs_pwr;
    cmd_ptr->cmd_msg.cmd_rm_apcfg.inq_scan_act_vs_pwr =
      inq_scan_act_vs_pwr;
    cmd_ptr->cmd_msg.cmd_rm_apcfg.deliver_dev_updates =
      deliver_dev_updates;
  
    cmd_status = bt_cmd( cmd_ptr );
  }

  return( cmd_status );

}


/*===========================================================================

FUNCTION
  bt_cmd_rm_auto_service_search_disable

DESCRIPTION
  This function enables/disables automatic service search upon bonding to a
  remote device.

===========================================================================*/
bt_cmd_status_type bt_cmd_rm_auto_service_search_disable
(
  bt_app_id_type  bt_app_id,
  boolean         disable
)
{

  bt_cmd_msg_type*    cmd_ptr;
  bt_cmd_status_type  cmd_status = BT_CS_GN_CMD_Q_FULL;

  if ( (cmd_ptr = bt_get_free_cmd()) != NULL )
  {
    cmd_ptr->cmd_hdr.cmd_type             = 
      BT_CMD_RM_AUTO_SERVICE_SEARCH_DISABLE;
    cmd_ptr->cmd_hdr.bt_app_id            = bt_app_id;
    cmd_ptr->cmd_msg.cmd_rm_ssdis.disable = disable;
  
    cmd_status = bt_cmd( cmd_ptr );
  }

  return( cmd_status );

}

/*===========================================================================

FUNCTION
  bt_cmd_rm_register_link_status_ext

DESCRIPTION
  This function sends a RM Register Link Status command to the
  Bluetooth driver.

===========================================================================*/
bt_cmd_status_type bt_cmd_rm_register_link_status_ext
(
  bt_app_id_type    bt_app_id,
  bt_bd_addr_type*  bd_addr_ptr,
  boolean           security_changes,
  boolean           mode_changes,
  boolean           role_switches,
  boolean           new_acl_pkts,
  boolean           new_sco_pkts,
  uint16            rssi_delta,
  boolean           sync_param_updates
)
{
  
  bt_cmd_msg_type*    cmd_ptr;
  bt_cmd_status_type  cmd_status = BT_CS_GN_CMD_Q_FULL;

  if ( (cmd_ptr = bt_get_free_cmd()) != NULL )
  {
    cmd_ptr->cmd_hdr.cmd_type  = BT_CMD_RM_REG_LINK_STATUS;
    cmd_ptr->cmd_hdr.bt_app_id = bt_app_id;
    cmd_ptr->cmd_msg.cmd_rm_regls.bd_addr          = *bd_addr_ptr;
    cmd_ptr->cmd_msg.cmd_rm_regls.security_changes = security_changes;
    cmd_ptr->cmd_msg.cmd_rm_regls.mode_changes     = mode_changes;
    cmd_ptr->cmd_msg.cmd_rm_regls.role_switches    = role_switches;
    cmd_ptr->cmd_msg.cmd_rm_regls.new_acl_pkts     = new_acl_pkts;
    cmd_ptr->cmd_msg.cmd_rm_regls.new_sco_pkts     = new_sco_pkts;
    cmd_ptr->cmd_msg.cmd_rm_regls.rssi_delta       = rssi_delta;
    cmd_ptr->cmd_msg.cmd_rm_regls.sync_param_updates       = sync_param_updates; 

    cmd_status = bt_cmd( cmd_ptr );
  }

  return( cmd_status );

}

/*===========================================================================

FUNCTION
  bt_cmd_rm_register_link_status

DESCRIPTION
  This function sends a RM Register Link Status command to the
  Bluetooth driver. For backward compatability, 
  bt_cmd_rm_register_link_status_ext() is called with new sync_param_updates 
  param set to FALSE.

===========================================================================*/
bt_cmd_status_type bt_cmd_rm_register_link_status
(
  bt_app_id_type    bt_app_id,
  bt_bd_addr_type*  bd_addr_ptr,
  boolean           security_changes,
  boolean           mode_changes,
  boolean           role_switches,
  boolean           new_acl_pkts,
  boolean           new_sco_pkts,
  uint16            rssi_delta
)
{
  return( bt_cmd_rm_register_link_status_ext (
                                               bt_app_id,
                                               bd_addr_ptr,
                                               security_changes,
                                               mode_changes,
                                               role_switches,
                                               new_acl_pkts,
                                               new_sco_pkts,
                                               rssi_delta,
                                               FALSE )
         );
}


/*===========================================================================

FUNCTION
  bt_cmd_rm_unregister_link_status

DESCRIPTION
  This function sends a RM Unregister Link Status command to the
  Bluetooth driver.

===========================================================================*/
bt_cmd_status_type bt_cmd_rm_unregister_link_status
(
  bt_app_id_type    bt_app_id,
  bt_bd_addr_type*  bd_addr_ptr
)
{
  
  bt_cmd_msg_type*    cmd_ptr;
  bt_cmd_status_type  cmd_status = BT_CS_GN_CMD_Q_FULL;

  if ( (cmd_ptr = bt_get_free_cmd()) != NULL )
  {
    cmd_ptr->cmd_hdr.cmd_type             = BT_CMD_RM_UNREG_LINK_STATUS;
    cmd_ptr->cmd_hdr.bt_app_id            = bt_app_id;
    cmd_ptr->cmd_msg.cmd_rm_urgls.bd_addr = *bd_addr_ptr;
  
    cmd_status = bt_cmd( cmd_ptr );
  }

  return( cmd_status );

}


/*===========================================================================

FUNCTION
  bt_cmd_rm_register_radio_activity

===========================================================================*/
bt_cmd_status_type bt_cmd_rm_register_radio_activity
(
  bt_app_id_type    bt_app_id
)
{
  
  bt_cmd_msg_type*    cmd_ptr;
  bt_cmd_status_type  cmd_status = BT_CS_GN_CMD_Q_FULL;

  if ( (cmd_ptr = bt_get_free_cmd()) != NULL )
  {
    cmd_ptr->cmd_hdr.cmd_type  = BT_CMD_RM_REG_RADIO_ACTIVITY;
    cmd_ptr->cmd_hdr.bt_app_id = bt_app_id;
  
    cmd_status = bt_cmd( cmd_ptr );
  }

  return( cmd_status );

}


/*===========================================================================

FUNCTION
  bt_cmd_rm_unregister_radio_activity

===========================================================================*/
bt_cmd_status_type bt_cmd_rm_unregister_radio_activity
(
  bt_app_id_type    bt_app_id
)
{
  
  bt_cmd_msg_type*    cmd_ptr;
  bt_cmd_status_type  cmd_status = BT_CS_GN_CMD_Q_FULL;

  if ( (cmd_ptr = bt_get_free_cmd()) != NULL )
  {
    cmd_ptr->cmd_hdr.cmd_type  = BT_CMD_RM_UNREG_RADIO_ACTIVITY;
    cmd_ptr->cmd_hdr.bt_app_id = bt_app_id;
  
    cmd_status = bt_cmd( cmd_ptr );
  }

  return( cmd_status );

}


/*===========================================================================

FUNCTION
  bt_cmd_rm_low_power_config

DESCRIPTION
  This function sends an RM Low Power Configuration command to the
  Bluetooth driver.

===========================================================================*/
bt_cmd_status_type bt_cmd_rm_low_power_config
(
  bt_app_id_type          bt_app_id,     
  bt_bd_addr_type*        bd_addr_ptr,   
  bt_low_power_ctrl_type  low_power_ctrl,
  uint16                  max_interval_ms,
  uint16                  idle_timeout_ms
)
{

  bt_cmd_msg_type*    cmd_ptr;
  bt_cmd_status_type  cmd_status = BT_CS_GN_CMD_Q_FULL;

  if ( (cmd_ptr = bt_get_free_cmd()) != NULL )
  {
    cmd_ptr->cmd_hdr.cmd_type  = BT_CMD_RM_LOW_POWER_CONFIG;
    cmd_ptr->cmd_hdr.bt_app_id = bt_app_id;
    
    cmd_ptr->cmd_msg.cmd_rm_lpcfg.bd_addr         = *bd_addr_ptr;
    cmd_ptr->cmd_msg.cmd_rm_lpcfg.low_power_ctrl  = low_power_ctrl;
    cmd_ptr->cmd_msg.cmd_rm_lpcfg.max_interval_ms = max_interval_ms;
    cmd_ptr->cmd_msg.cmd_rm_lpcfg.idle_timeout_ms = idle_timeout_ms;
  
    cmd_status = bt_cmd( cmd_ptr );
  }

  return( cmd_status );

}


#ifdef BT_SWDEV_2_1_SSR
/*===========================================================================

FUNCTION
  bt_cmd_rm_low_power_config_ext

DESCRIPTION
  This function sends an RM Low Power Configuration Extension command to the
  Bluetooth driver.

===========================================================================*/
bt_cmd_status_type bt_cmd_rm_low_power_config_ext
(
  bt_app_id_type           bt_app_id,     
  bt_bd_addr_type*         bd_addr_ptr,   
  bt_low_power_ctrl_type   low_power_ctrl,
  uint16                   max_interval_ms,
  uint16                   idle_timeout_ms,
  bt_sniff_ssr_ctrl_type*  sniff_ssr_ctrl_ptr
)
{

  bt_cmd_msg_type*    cmd_ptr;
  bt_cmd_status_type  cmd_status = BT_CS_GN_CMD_Q_FULL;

  if ( (cmd_ptr = bt_get_free_cmd()) != NULL )
  {
    cmd_ptr->cmd_hdr.cmd_type  = BT_CMD_RM_LOW_POWER_CONFIG_EXT;
    cmd_ptr->cmd_hdr.bt_app_id = bt_app_id;
    
    cmd_ptr->cmd_msg.cmd_rm_lpcfg_ext.bd_addr            = *bd_addr_ptr;
    cmd_ptr->cmd_msg.cmd_rm_lpcfg_ext.low_power_ctrl     = low_power_ctrl;
    cmd_ptr->cmd_msg.cmd_rm_lpcfg_ext.max_interval_ms    = max_interval_ms;
    cmd_ptr->cmd_msg.cmd_rm_lpcfg_ext.idle_timeout_ms    = idle_timeout_ms;
    cmd_ptr->cmd_msg.cmd_rm_lpcfg_ext.max_latency_ms     =
      sniff_ssr_ctrl_ptr->max_latency_ms;
    cmd_ptr->cmd_msg.cmd_rm_lpcfg_ext.local_cid          =
      sniff_ssr_ctrl_ptr->local_cid;
    cmd_ptr->cmd_msg.cmd_rm_lpcfg_ext.server_channel     =
      sniff_ssr_ctrl_ptr->server_channel;
    cmd_ptr->cmd_msg.cmd_rm_lpcfg_ext.stick_to_app_param =
      sniff_ssr_ctrl_ptr->stick_to_app_param;
    cmd_ptr->cmd_msg.cmd_rm_lpcfg_ext.sniff_initiator_oriented =
      sniff_ssr_ctrl_ptr->sniff_initiator_oriented;
  
    cmd_status = bt_cmd( cmd_ptr );
  }

  return( cmd_status );

}
#endif /* BT_SWDEV_2_1_SSR */


/*===========================================================================

FUNCTION
  bt_cmd_rm_set_connection_role

DESCRIPTION
  This function sends a RM Set Connection Role command to the
  Bluetooth driver.

===========================================================================*/
bt_cmd_status_type bt_cmd_rm_set_connection_role
(
  bt_app_id_type    bt_app_id,
  bt_bd_addr_type*  bd_addr_ptr,
  bt_role_type      role
)
{

  bt_cmd_msg_type*    cmd_ptr;
  bt_cmd_status_type  cmd_status = BT_CS_GN_CMD_Q_FULL;

  if ( (cmd_ptr = bt_get_free_cmd()) != NULL )
  {
    cmd_ptr->cmd_hdr.cmd_type  = BT_CMD_RM_SET_CONN_ROLE;
    cmd_ptr->cmd_hdr.bt_app_id = bt_app_id;
    
    if ( bd_addr_ptr != NULL )
    {
      cmd_ptr->cmd_msg.cmd_rm_setrl.bd_addr_valid = TRUE;
      cmd_ptr->cmd_msg.cmd_rm_setrl.bd_addr       = *bd_addr_ptr;
    }
    else
    {
      cmd_ptr->cmd_msg.cmd_rm_setrl.bd_addr_valid = FALSE;
    }
    
    cmd_ptr->cmd_msg.cmd_rm_setrl.role = role;
  
    cmd_status = bt_cmd( cmd_ptr );
  }

  return( cmd_status );

}


/*===========================================================================

FUNCTION
  bt_cmd_rm_set_conn_role_sw_req_ok

DESCRIPTION
  This function sends a RM Set Connection Role Switch Request
  OK command to the Bluetooth driver.

===========================================================================*/
bt_cmd_status_type bt_cmd_rm_set_conn_role_sw_req_ok
(
  bt_app_id_type    bt_app_id,
  bt_bd_addr_type*  bd_addr_ptr,
  boolean           role_sw_req_ok
)
{

  bt_cmd_msg_type*    cmd_ptr;
  bt_cmd_status_type  cmd_status = BT_CS_GN_CMD_Q_FULL;

  if ( (cmd_ptr = bt_get_free_cmd()) != NULL )
  {
    cmd_ptr->cmd_hdr.cmd_type  = BT_CMD_RM_SET_CONN_ROLE_SW_REQ_OK;
    cmd_ptr->cmd_hdr.bt_app_id = bt_app_id;
    
    if ( bd_addr_ptr != NULL )
    {
      cmd_ptr->cmd_msg.cmd_rm_scrsr.bd_addr_valid = TRUE;
      cmd_ptr->cmd_msg.cmd_rm_scrsr.bd_addr       = *bd_addr_ptr;
    }
    else
    {
      cmd_ptr->cmd_msg.cmd_rm_scrsr.bd_addr_valid = FALSE;
    }
    
    cmd_ptr->cmd_msg.cmd_rm_scrsr.role_sw_req_ok = role_sw_req_ok;
  
    cmd_status = bt_cmd( cmd_ptr );
  }

  return( cmd_status );

}


/*===========================================================================

FUNCTION
  bt_cmd_rm_set_connection_security

DESCRIPTION
  This function sends a RM Set Connection Security command to the
  Bluetooth driver.

===========================================================================*/
bt_cmd_status_type bt_cmd_rm_set_connection_security
(
  bt_app_id_type    bt_app_id,
  bt_bd_addr_type*  bd_addr_ptr,
  bt_security_type  security
)
{

  bt_cmd_status_type  cmd_status = BT_CS_GN_CMD_Q_FULL;

#ifdef BT_SWDEV_2_1_SSP
  cmd_status = BT_CS_GN_DEPRECATED_CMD;
#else
  bt_cmd_msg_type*    cmd_ptr;

  if ( (cmd_ptr = bt_get_free_cmd()) != NULL )
  {
    cmd_ptr->cmd_hdr.cmd_type  = BT_CMD_RM_SET_CONN_SECURITY;
    cmd_ptr->cmd_hdr.bt_app_id = bt_app_id;
    
    cmd_ptr->cmd_msg.cmd_rm_setcs.bd_addr  = *bd_addr_ptr;
    cmd_ptr->cmd_msg.cmd_rm_setcs.security = security;
  
    cmd_status = bt_cmd( cmd_ptr );
  }
#endif /* BT_SWDEV_2_1_SSP */

  return( cmd_status );

}


/*===========================================================================

FUNCTION
  bt_cmd_rm_set_device_security

DESCRIPTION
  This function sends a RM Set Device Security command to the Bluetooth
  driver.

===========================================================================*/
bt_cmd_status_type bt_cmd_rm_set_device_security
(
  bt_app_id_type    bt_app_id,
  bt_bd_addr_type*  bd_addr_ptr,
  bt_security_type  security
)
{

  bt_cmd_status_type  cmd_status = BT_CS_GN_CMD_Q_FULL;
  bt_cmd_msg_type*    cmd_ptr;

  if ( (cmd_ptr = bt_get_free_cmd()) != NULL )
  {
    cmd_ptr->cmd_hdr.cmd_type  = BT_CMD_RM_SET_DEVICE_SECURITY;
    cmd_ptr->cmd_hdr.bt_app_id = bt_app_id;
    
    if ( bd_addr_ptr != NULL )
    {
      cmd_ptr->cmd_msg.cmd_rm_setds.bd_addr_valid = TRUE;
      cmd_ptr->cmd_msg.cmd_rm_setds.bd_addr       = *bd_addr_ptr;
    }
    else
    {
      cmd_ptr->cmd_msg.cmd_rm_setds.bd_addr_valid = FALSE;
    }

    cmd_ptr->cmd_msg.cmd_rm_setds.security = security;
  
    cmd_status = bt_cmd( cmd_ptr );
  }

  return( cmd_status );

}


/*===========================================================================

FUNCTION
  bt_cmd_rm_set_service_security

DESCRIPTION
  This function sends a RM Set Service Security command to the Bluetooth
  driver.

===========================================================================*/
bt_cmd_status_type bt_cmd_rm_set_service_security
(
  bt_app_id_type       bt_app_id,
  bt_service_id_type*  service_id_ptr,
  bt_security_type     security,
  boolean              authorize,
  boolean              authorize_first
)
{

  bt_cmd_status_type  cmd_status = BT_CS_GN_CMD_Q_FULL;

#ifdef BT_SWDEV_2_1_SSP
  cmd_status = BT_CS_GN_DEPRECATED_CMD;
#else
  bt_cmd_msg_type*    cmd_ptr;

  if ( (cmd_ptr = bt_get_free_cmd()) != NULL )
  {
    cmd_ptr->cmd_hdr.cmd_type  = BT_CMD_RM_SET_SERVICE_SECURITY;
    cmd_ptr->cmd_hdr.bt_app_id = bt_app_id;
    
    cmd_ptr->cmd_msg.cmd_rm_setss.service_id      = *service_id_ptr;
    cmd_ptr->cmd_msg.cmd_rm_setss.security        = security;
    cmd_ptr->cmd_msg.cmd_rm_setss.authorize       = authorize;
    cmd_ptr->cmd_msg.cmd_rm_setss.authorize_first = authorize_first;
  
    cmd_status = bt_cmd( cmd_ptr );
  }
#endif /* BT_SWDEV_2_1_SSP */
  return( cmd_status );

}


/*===========================================================================

FUNCTION
  bt_cmd_rm_set_service_sec_ext

DESCRIPTION
  This function sends a RM Set Service Security Extended command to
  the Bluetooth driver.

===========================================================================*/
bt_cmd_status_type bt_cmd_rm_set_service_sec_ext
(
  bt_app_id_type       bt_app_id,
  bt_service_id_type*  service_id_ptr,
  uint8                min_pin_len_bytes,
  boolean              comb_link_key_only,
  uint8                min_enc_key_len_bytes
)
{

  bt_cmd_msg_type*    cmd_ptr;
  bt_cmd_status_type  cmd_status = BT_CS_GN_CMD_Q_FULL;

  if ( (cmd_ptr = bt_get_free_cmd()) != NULL )
  {
    cmd_ptr->cmd_hdr.cmd_type  = BT_CMD_RM_SET_SERVICE_SEC_EXT;
    cmd_ptr->cmd_hdr.bt_app_id = bt_app_id;
    
    cmd_ptr->cmd_msg.cmd_rm_setse.service_id         = *service_id_ptr;
    cmd_ptr->cmd_msg.cmd_rm_setse.min_pin_len_bytes  = min_pin_len_bytes;
    cmd_ptr->cmd_msg.cmd_rm_setse.comb_link_key_only = comb_link_key_only;
    cmd_ptr->cmd_msg.cmd_rm_setse.min_enc_key_len_bytes =
      min_enc_key_len_bytes;
  
    cmd_status = bt_cmd( cmd_ptr );
  }

  return( cmd_status );

}


/*===========================================================================

FUNCTION
  bt_cmd_rm_authorize_reply

DESCRIPTION
  This function sends a RM Authorize Reply command to the Bluetooth
  driver.

===========================================================================*/
bt_cmd_status_type bt_cmd_rm_authorize_reply
(
  bt_app_id_type       bt_app_id,
  bt_service_id_type*  service_id_ptr,
  bt_bd_addr_type*     bd_addr_ptr,
  boolean              authorized
)
{

  bt_cmd_msg_type*    cmd_ptr;
  bt_cmd_status_type  cmd_status = BT_CS_GN_CMD_Q_FULL;

  if ( (cmd_ptr = bt_get_free_cmd()) != NULL )
  {
    cmd_ptr->cmd_hdr.cmd_type  = BT_CMD_RM_AUTHORIZE_REPLY;
    cmd_ptr->cmd_hdr.bt_app_id = bt_app_id;
    
    cmd_ptr->cmd_msg.cmd_rm_atzrp.service_id = *service_id_ptr;
    cmd_ptr->cmd_msg.cmd_rm_atzrp.bd_addr    = *bd_addr_ptr;
    cmd_ptr->cmd_msg.cmd_rm_atzrp.authorized = authorized;
  
    cmd_status = bt_cmd( cmd_ptr );
  }

  return( cmd_status );

}


/*===========================================================================

FUNCTION
  bt_cmd_rm_enforce_security_rfcomm_scn

DESCRIPTION
  This function sends a RM Enforce Security RFCOMM SCN command to the
  Bluetooth driver.

===========================================================================*/
bt_cmd_status_type bt_cmd_rm_enforce_security_rfcomm_scn
(
  bt_app_id_type    bt_app_id,
  uint8             rfcomm_scn,
  bt_bd_addr_type*  bd_addr_ptr,
  boolean           local_client
)
{

  bt_cmd_msg_type*    cmd_ptr;
  bt_cmd_status_type  cmd_status = BT_CS_GN_CMD_Q_FULL;

  if ( (cmd_ptr = bt_get_free_cmd()) != NULL )
  {
    cmd_ptr->cmd_hdr.cmd_type  = BT_CMD_RM_ENFORCE_SEC_RFCOMM_SCN;
    cmd_ptr->cmd_hdr.bt_app_id = bt_app_id;
    
    cmd_ptr->cmd_msg.cmd_rm_esscn.rfcomm_scn   = rfcomm_scn;
    cmd_ptr->cmd_msg.cmd_rm_esscn.bd_addr      = *bd_addr_ptr;
    cmd_ptr->cmd_msg.cmd_rm_esscn.local_client = local_client;
  
    cmd_status = bt_cmd( cmd_ptr );
  }

  return( cmd_status );

}


/*===========================================================================

FUNCTION
  bt_cmd_rm_enforce_security_l2cap_psm

DESCRIPTION
  This function sends a RM Enforce Security L2CAP PSM command to the
  Bluetooth driver.

===========================================================================*/
bt_cmd_status_type bt_cmd_rm_enforce_security_l2cap_psm
(
  bt_app_id_type    bt_app_id,
  uint16            l2cap_psm,
  bt_bd_addr_type*  bd_addr_ptr,
  uint16            l2cap_cid,
  bt_rm_atzrq_type  conn_dir
)
{

  bt_cmd_msg_type*    cmd_ptr;
  bt_cmd_status_type  cmd_status = BT_CS_GN_CMD_Q_FULL;

  if ( (cmd_ptr = bt_get_free_cmd()) != NULL )
  {
    cmd_ptr->cmd_hdr.cmd_type  = BT_CMD_RM_ENFORCE_SEC_L2CAP_PSM;
    cmd_ptr->cmd_hdr.bt_app_id = bt_app_id;
    
    cmd_ptr->cmd_msg.cmd_rm_espsm.l2cap_psm = l2cap_psm;
    cmd_ptr->cmd_msg.cmd_rm_espsm.bd_addr   = *bd_addr_ptr;
    cmd_ptr->cmd_msg.cmd_rm_espsm.l2cap_cid = l2cap_cid;
    cmd_ptr->cmd_msg.cmd_rm_espsm.conn_dir  = conn_dir;
  
    cmd_status = bt_cmd( cmd_ptr );
  }

  return( cmd_status );

}


/*===========================================================================

FUNCTION
  bt_cmd_rm_bond

DESCRIPTION
  This function sends a RM Bond command to the Bluetooth driver.

===========================================================================*/
bt_cmd_status_type bt_cmd_rm_bond
(
  bt_app_id_type     bt_app_id,
  bt_bd_addr_type*   bd_addr_ptr,
  bt_pin_code_type*  pin_ptr
)
{

  bt_cmd_status_type  cmd_status = BT_CS_GN_CMD_Q_FULL;

#ifdef BT_SWDEV_2_1_SSP
  cmd_status = BT_CS_GN_DEPRECATED_CMD;
#else
  bt_cmd_msg_type*    cmd_ptr;

  if ( (cmd_ptr = bt_get_free_cmd()) != NULL )
  {
    cmd_ptr->cmd_hdr.cmd_type            = BT_CMD_RM_BOND;
    cmd_ptr->cmd_hdr.bt_app_id           = bt_app_id;
    cmd_ptr->cmd_msg.cmd_rm_bond.bd_addr = *bd_addr_ptr;
    cmd_ptr->cmd_msg.cmd_rm_bond.pin     = *pin_ptr;
  
    cmd_status = bt_cmd( cmd_ptr );
  }
#endif /* BT_SWDEV_2_1_SSP */

  return( cmd_status );

}


/*===========================================================================

FUNCTION
  bt_cmd_rm_bond_cancel

DESCRIPTION
  This function sends a RM Bond cancel command to the Bluetooth driver.

===========================================================================*/
bt_cmd_status_type bt_cmd_rm_bond_cancel
(
  bt_app_id_type     bt_app_id,
  bt_bd_addr_type*   bd_addr_ptr
)
{

  bt_cmd_status_type  cmd_status = BT_CS_GN_CMD_Q_FULL;

  bt_cmd_msg_type*    cmd_ptr;

  if ( (cmd_ptr = bt_get_free_cmd()) != NULL )
  {
    cmd_ptr->cmd_hdr.cmd_type            = BT_CMD_RM_BOND_CANCEL;
    cmd_ptr->cmd_hdr.bt_app_id           = bt_app_id;
    cmd_ptr->cmd_msg.cmd_rm_bondc.bd_addr = *bd_addr_ptr;
  
    cmd_status = bt_cmd( cmd_ptr );
  }

  return( cmd_status );

}


/*===========================================================================

FUNCTION
  bt_cmd_rm_unbond

DESCRIPTION
  This function sends a RM Unbond command to the Bluetooth driver.

===========================================================================*/
bt_cmd_status_type bt_cmd_rm_unbond
(
  bt_app_id_type     bt_app_id,
  bt_bd_addr_type*   bd_addr_ptr
)
{

  bt_cmd_msg_type*    cmd_ptr;
  bt_cmd_status_type  cmd_status = BT_CS_GN_CMD_Q_FULL;

  if ( (cmd_ptr = bt_get_free_cmd()) != NULL )
  {
    cmd_ptr->cmd_hdr.cmd_type             = BT_CMD_RM_UNBOND;
    cmd_ptr->cmd_hdr.bt_app_id            = bt_app_id;
    cmd_ptr->cmd_msg.cmd_rm_unbnd.bd_addr = *bd_addr_ptr;
  
    cmd_status = bt_cmd( cmd_ptr );
  }

  return( cmd_status );

}


/*===========================================================================

FUNCTION
  bt_cmd_rm_set_bondable

DESCRIPTION
  This function sends a RM Set Bondable command to the
  Bluetooth driver.

===========================================================================*/
bt_cmd_status_type bt_cmd_rm_set_bondable
(
  bt_app_id_type  bt_app_id,
  boolean         bondable
)
{

  bt_cmd_msg_type*    cmd_ptr;
  bt_cmd_status_type  cmd_status = BT_CS_GN_CMD_Q_FULL;

  if ( (cmd_ptr = bt_get_free_cmd()) != NULL )
  {
    cmd_ptr->cmd_hdr.cmd_type              = BT_CMD_RM_SET_BONDABLE;
    cmd_ptr->cmd_hdr.bt_app_id             = bt_app_id;
    cmd_ptr->cmd_msg.cmd_rm_setbd.bondable = bondable;
  
    cmd_status = bt_cmd( cmd_ptr );
  }

  return( cmd_status );

}


/*===========================================================================

FUNCTION
  bt_cmd_rm_pin_reply

DESCRIPTION
  This function sends a RM Pin Reply command to the Bluetooth driver.

===========================================================================*/
bt_cmd_status_type bt_cmd_rm_pin_reply
(
  bt_app_id_type     bt_app_id,
  bt_bd_addr_type*   bd_addr_ptr,
  boolean            pin_valid,
  bt_pin_code_type*  pin_ptr
)
{
  
  bt_cmd_msg_type*    cmd_ptr;
  bt_cmd_status_type  cmd_status = BT_CS_GN_CMD_Q_FULL;

  if ( (cmd_ptr = bt_get_free_cmd()) != NULL )
  {
    cmd_ptr->cmd_hdr.cmd_type             = BT_CMD_RM_PIN_REPLY;
    cmd_ptr->cmd_hdr.bt_app_id            = bt_app_id;
    cmd_ptr->cmd_msg.cmd_rm_pin.bd_addr   = *bd_addr_ptr;
    cmd_ptr->cmd_msg.cmd_rm_pin.pin_valid = pin_valid;
    cmd_ptr->cmd_msg.cmd_rm_pin.pin       = *pin_ptr;
  
    cmd_status = bt_cmd( cmd_ptr );
  }

  return( cmd_status );

}


/*===========================================================================

FUNCTION
  bt_cmd_rm_device_add

DESCRIPTION
  This function sends a RM Device Add command to the Bluetooth driver.

===========================================================================*/
bt_cmd_status_type bt_cmd_rm_device_add
(
  bt_app_id_type         bt_app_id,
  const bt_device_type*  device_ptr
)
{
  
  bt_cmd_msg_type*    cmd_ptr;
  bt_cmd_status_type  cmd_status = BT_CS_GN_CMD_Q_FULL;

  if ( (cmd_ptr = bt_get_free_cmd()) != NULL )
  {
    cmd_ptr->cmd_hdr.cmd_type            = BT_CMD_RM_DEVICE_ADD;
    cmd_ptr->cmd_hdr.bt_app_id           = bt_app_id;
    cmd_ptr->cmd_msg.cmd_rm_dvadd.device = *device_ptr;
  
    cmd_status = bt_cmd( cmd_ptr );
  }

  return( cmd_status );

}


/*===========================================================================

FUNCTION
  bt_cmd_rm_device_update

DESCRIPTION
  This function sends a RM Device Update command to the Bluetooth driver.

===========================================================================*/
bt_cmd_status_type bt_cmd_rm_device_update
(
  bt_app_id_type              bt_app_id,
  bt_rm_dev_update_ctrl_type  update_control,
  const bt_device_type*       device_ptr
)
{
  
  bt_cmd_msg_type*    cmd_ptr;
  bt_cmd_status_type  cmd_status = BT_CS_GN_CMD_Q_FULL;

  if ( (cmd_ptr = bt_get_free_cmd()) != NULL )
  {
    cmd_ptr->cmd_hdr.cmd_type                    = BT_CMD_RM_DEVICE_UPDATE;
    cmd_ptr->cmd_hdr.bt_app_id                   = bt_app_id;
    cmd_ptr->cmd_msg.cmd_rm_dvupd.update_control = update_control;
    cmd_ptr->cmd_msg.cmd_rm_dvupd.device         = *device_ptr;
  
    cmd_status = bt_cmd( cmd_ptr );
  }

  return( cmd_status );

}


/*===========================================================================

FUNCTION
  bt_cmd_rm_device_services_update

DESCRIPTION
  This function sends a RM Device Services Update command to the Bluetooth 
  driver.

===========================================================================*/
bt_cmd_status_type bt_cmd_rm_device_services_update
(
  bt_app_id_type          bt_app_id,
  bt_bd_addr_type*        bd_addr_ptr,
  uint16                  num_services,
  const bt_service_type*  services_ptr
)
{
  
  bt_cmd_msg_type*    cmd_ptr;
  bt_cmd_status_type  cmd_status = BT_CS_GN_CMD_Q_FULL;

  if ( (cmd_ptr = bt_get_free_cmd()) != NULL )
  {
    cmd_ptr->cmd_hdr.cmd_type             = BT_CMD_RM_DEVICE_SERVICES_UPDATE;
    cmd_ptr->cmd_hdr.bt_app_id            = bt_app_id;
    cmd_ptr->cmd_msg.cmd_rm_dsupd.bd_addr = *bd_addr_ptr;
    cmd_ptr->cmd_msg.cmd_rm_dsupd.num_services = num_services;

    memcpy( (void*)cmd_ptr->cmd_msg.cmd_rm_dsupd.services,
            (void*)services_ptr,
            ( ( num_services > BT_SD_MAX_NUM_OF_SRV_REC ) ? 
              BT_SD_MAX_NUM_OF_SRV_REC : num_services ) * 
              sizeof( bt_service_type ) );
  
    cmd_status = bt_cmd( cmd_ptr );
  }

  return( cmd_status );

}


/*===========================================================================

FUNCTION
  bt_cmd_rm_device_remove

DESCRIPTION
  This function sends a RM Device Update command to the Bluetooth driver.

===========================================================================*/
bt_cmd_status_type bt_cmd_rm_device_remove
(
  bt_app_id_type    bt_app_id,
  bt_bd_addr_type*  bd_addr_ptr
)
{
  
  bt_cmd_msg_type*    cmd_ptr;
  bt_cmd_status_type  cmd_status = BT_CS_GN_CMD_Q_FULL;

  if ( (cmd_ptr = bt_get_free_cmd()) != NULL )
  {
    cmd_ptr->cmd_hdr.cmd_type             = BT_CMD_RM_DEVICE_REMOVE;
    cmd_ptr->cmd_hdr.bt_app_id            = bt_app_id;
    cmd_ptr->cmd_msg.cmd_rm_dvrmv.bd_addr = *bd_addr_ptr;
  
    cmd_status = bt_cmd( cmd_ptr );
  }

  return( cmd_status );

}


/*===========================================================================

FUNCTION
  bt_cmd_rm_device_remove_all

DESCRIPTION
  This function sends a RM Device Remove All command to the Bluetooth
  driver.

===========================================================================*/
bt_cmd_status_type bt_cmd_rm_device_remove_all
(
  bt_app_id_type    bt_app_id
)
{
  
  bt_cmd_msg_type*    cmd_ptr;
  bt_cmd_status_type  cmd_status = BT_CS_GN_CMD_Q_FULL;

  if ( (cmd_ptr = bt_get_free_cmd()) != NULL )
  {
    cmd_ptr->cmd_hdr.cmd_type  = BT_CMD_RM_DEVICE_REMOVE_ALL;
    cmd_ptr->cmd_hdr.bt_app_id = bt_app_id;
  
    cmd_status = bt_cmd( cmd_ptr );
  }

  return( cmd_status );

}


/*===========================================================================

FUNCTION
  bt_cmd_rm_test_mode_enable

DESCRIPTION
  This function sends a RM Test Mode Enable command to Bluetooth
  Driver.

===========================================================================*/
bt_cmd_status_type bt_cmd_rm_test_mode_enable
(
  bt_app_id_type  bt_app_id,
  boolean         enable
)
{

  bt_cmd_msg_type*    cmd_ptr;
  bt_cmd_status_type  cmd_status = BT_CS_GN_CMD_Q_FULL;

  if ( (cmd_ptr = bt_get_free_cmd()) != NULL )
  {
    cmd_ptr->cmd_hdr.cmd_type            = BT_CMD_RM_TEST_MODE_ENABLE;
    cmd_ptr->cmd_hdr.bt_app_id           = bt_app_id;
    cmd_ptr->cmd_msg.cmd_rm_testm.enable = enable;
  
    cmd_status = bt_cmd( cmd_ptr );
  }

  return( cmd_status );

}


/*===========================================================================

FUNCTION
  bt_cmd_rm_set_connectable

DESCRIPTION
  This function sends a RM Set Connectable command to the Bluetooth
  driver.

===========================================================================*/
bt_cmd_status_type bt_cmd_rm_set_connectable
(
  bt_app_id_type         bt_app_id,
  boolean                connectable,
  bt_rm_act_vs_pwr_type  act_vs_pwr
)
{

  bt_cmd_msg_type*    cmd_ptr;
  bt_cmd_status_type  cmd_status = BT_CS_GN_CMD_Q_FULL;

  if ( (cmd_ptr = bt_get_free_cmd()) != NULL )
  {
    cmd_ptr->cmd_hdr.cmd_type  = BT_CMD_RM_SET_CONNECTABLE;
    cmd_ptr->cmd_hdr.bt_app_id = bt_app_id;
    cmd_ptr->cmd_msg.cmd_rm_setc.connectable = connectable;
    cmd_ptr->cmd_msg.cmd_rm_setc.act_vs_pwr  = act_vs_pwr;
  
    cmd_status = bt_cmd( cmd_ptr );
  }

  return( cmd_status );

}


/*===========================================================================

FUNCTION
  bt_cmd_rm_set_discoverable

DESCRIPTION
  This function sends a RM Set Discoverable command to the Bluetooth
  driver.

===========================================================================*/
bt_cmd_status_type bt_cmd_rm_set_discoverable
(
  bt_app_id_type           bt_app_id,
  bt_rm_discoverable_type  discoverable,
  bt_rm_act_vs_pwr_type    act_vs_pwr
)
{

  bt_cmd_msg_type*    cmd_ptr;
  bt_cmd_status_type  cmd_status = BT_CS_GN_CMD_Q_FULL;

  if ( (cmd_ptr = bt_get_free_cmd()) != NULL )
  {
    cmd_ptr->cmd_hdr.cmd_type  = BT_CMD_RM_SET_DISCOVERABLE;
    cmd_ptr->cmd_hdr.bt_app_id = bt_app_id;
    cmd_ptr->cmd_msg.cmd_rm_setd.discoverable = discoverable;
    cmd_ptr->cmd_msg.cmd_rm_setd.act_vs_pwr   = act_vs_pwr;
  
    cmd_status = bt_cmd( cmd_ptr );
  }

  return( cmd_status );

}


/*===========================================================================

FUNCTION
  bt_cmd_rm_inquire

DESCRIPTION
  This function sends a RM Inquire command to the Bluetooth driver.

===========================================================================*/
bt_cmd_status_type bt_cmd_rm_inquire
(
  bt_app_id_type  bt_app_id,
  bt_iac_type     iac,
  uint8           duration_secs,
  bt_cod_type*    class_of_device_filter_ptr,
  uint8           max_responses
)
{

  bt_cmd_msg_type*    cmd_ptr;
  bt_cmd_status_type  cmd_status = BT_CS_GN_CMD_Q_FULL;

  if ( (cmd_ptr = bt_get_free_cmd()) != NULL )
  {
    cmd_ptr->cmd_hdr.cmd_type                 = BT_CMD_RM_INQUIRE;
    cmd_ptr->cmd_hdr.bt_app_id                = bt_app_id;
    cmd_ptr->cmd_msg.cmd_rm_inq.iac           = iac;
    cmd_ptr->cmd_msg.cmd_rm_inq.duration_secs = duration_secs;

    if ( class_of_device_filter_ptr ) 
    {
      cmd_ptr->cmd_msg.cmd_rm_inq.class_of_device_filter =
        *class_of_device_filter_ptr;
    }
    else
    {
      cmd_ptr->cmd_msg.cmd_rm_inq.class_of_device_filter.cod_bytes[0] = 0;
      cmd_ptr->cmd_msg.cmd_rm_inq.class_of_device_filter.cod_bytes[1] = 0;
      cmd_ptr->cmd_msg.cmd_rm_inq.class_of_device_filter.cod_bytes[2] = 0;
    }

    cmd_ptr->cmd_msg.cmd_rm_inq.max_responses = max_responses;
  
    cmd_status = bt_cmd( cmd_ptr );
  }

  return( cmd_status );

}


/*===========================================================================

FUNCTION
  bt_cmd_rm_stop_inquiry

DESCRIPTION
  This function sends a RM Stop Inquiry command to the Bluetooth driver.

===========================================================================*/
bt_cmd_status_type bt_cmd_rm_stop_inquiry
(
  bt_app_id_type  bt_app_id
)
{

  bt_cmd_msg_type*    cmd_ptr;
  bt_cmd_status_type  cmd_status = BT_CS_GN_CMD_Q_FULL;

  if ( (cmd_ptr = bt_get_free_cmd()) != NULL )
  {
    cmd_ptr->cmd_hdr.cmd_type  = BT_CMD_RM_STOP_INQUIRY;
    cmd_ptr->cmd_hdr.bt_app_id = bt_app_id;
  
    cmd_status = bt_cmd( cmd_ptr );
  }

  return( cmd_status );

}


/*===========================================================================

FUNCTION
  bt_cmd_rm_remote_name_req

DESCRIPTION
  This function sends a RM Remote Name Request command to the
  Bluetooth driver.

===========================================================================*/
bt_cmd_status_type bt_cmd_rm_remote_name_req
(
  bt_app_id_type    bt_app_id,
  bt_bd_addr_type*  bd_addr_ptr
)
{
  
  bt_cmd_msg_type*    cmd_ptr;
  bt_cmd_status_type  cmd_status = BT_CS_GN_CMD_Q_FULL;

  if ( (cmd_ptr = bt_get_free_cmd()) != NULL )
  {
    cmd_ptr->cmd_hdr.cmd_type             = BT_CMD_RM_REMOTE_NAME_REQ;
    cmd_ptr->cmd_hdr.bt_app_id            = bt_app_id;
    cmd_ptr->cmd_msg.cmd_rm_rname.bd_addr = *bd_addr_ptr;
  
    cmd_status = bt_cmd( cmd_ptr );
  }

  return( cmd_status );

}

/*===========================================================================

FUNCTION
  bt_cmd_rm_remote_name_req_cancel

DESCRIPTION
  This function sends a RM Remote Name Request cancel command to the
  Bluetooth driver.

===========================================================================*/
bt_cmd_status_type bt_cmd_rm_remote_name_req_cancel
(
  bt_app_id_type    bt_app_id,
  bt_bd_addr_type*  bd_addr_ptr
)
{
  
  bt_cmd_msg_type*    cmd_ptr;
  bt_cmd_status_type  cmd_status = BT_CS_GN_CMD_Q_FULL;

  if ( (cmd_ptr = bt_get_free_cmd()) != NULL )
  {
    cmd_ptr->cmd_hdr.cmd_type             = BT_CMD_RM_REMOTE_NAME_REQ_CANCEL;
    cmd_ptr->cmd_hdr.bt_app_id            = bt_app_id;
    cmd_ptr->cmd_msg.cmd_rm_rname_cancel.bd_addr = *bd_addr_ptr;
  
    cmd_status = bt_cmd( cmd_ptr );
  }

  return( cmd_status );

}

/*===========================================================================

FUNCTION
  bt_cmd_rm_connect_acl

DESCRIPTION
  This function sends a RM Connect ACL command to the Bluetooth driver.

===========================================================================*/
bt_cmd_status_type bt_cmd_rm_connect_acl
(
  bt_app_id_type    bt_app_id,
  bt_bd_addr_type*  bd_addr_ptr
)
{

  bt_cmd_msg_type*    cmd_ptr;
  bt_cmd_status_type  cmd_status = BT_CS_GN_CMD_Q_FULL;

  if ( (cmd_ptr = bt_get_free_cmd()) != NULL )
  {
    cmd_ptr->cmd_hdr.cmd_type             = BT_CMD_RM_CONNECT_ACL;
    cmd_ptr->cmd_hdr.bt_app_id            = bt_app_id;
    cmd_ptr->cmd_msg.cmd_rm_conna.bd_addr = *bd_addr_ptr;
  
    cmd_status = bt_cmd( cmd_ptr );
  }

  return( cmd_status );

}

/*===========================================================================

FUNCTION
  bt_cmd_rm_connect_cancel

DESCRIPTION
  This function sends a RM Connect Cancel command to the Bluetooth driver.

===========================================================================*/
bt_cmd_status_type bt_cmd_rm_connect_cancel
(
  bt_app_id_type    bt_app_id,
  bt_bd_addr_type*  bd_addr_ptr
)
{

  bt_cmd_msg_type*    cmd_ptr;
  bt_cmd_status_type  cmd_status = BT_CS_GN_CMD_Q_FULL;

  if ( (cmd_ptr = bt_get_free_cmd()) != NULL )
  {
    cmd_ptr->cmd_hdr.cmd_type             = BT_CMD_RM_CONNECT_CANCEL;
    cmd_ptr->cmd_hdr.bt_app_id            = bt_app_id;
    cmd_ptr->cmd_msg.cmd_rm_conn_cancel.bd_addr = *bd_addr_ptr;
  
    cmd_status = bt_cmd( cmd_ptr );
  }

  return( cmd_status );

}

/*===========================================================================

FUNCTION
  bt_cmd_rm_register_acl

DESCRIPTION
  This function sends a RM Register ACL command to the Bluetooth driver.

===========================================================================*/
bt_cmd_status_type bt_cmd_rm_register_acl
(
  bt_app_id_type  bt_app_id
)
{

  bt_cmd_msg_type*    cmd_ptr;
  bt_cmd_status_type  cmd_status = BT_CS_GN_CMD_Q_FULL;

  if ( (cmd_ptr = bt_get_free_cmd()) != NULL )
  {
    cmd_ptr->cmd_hdr.cmd_type  = BT_CMD_RM_REGISTER_ACL;
    cmd_ptr->cmd_hdr.bt_app_id = bt_app_id;
  
    cmd_status = bt_cmd( cmd_ptr );
  }

  return( cmd_status );

}


/*===========================================================================

FUNCTION
  bt_cmd_rm_accept_acl

DESCRIPTION
  This function sends a RM Accept ACL command to the Bluetooth driver.

===========================================================================*/
bt_cmd_status_type bt_cmd_rm_accept_acl
(
  bt_app_id_type    bt_app_id,
  bt_bd_addr_type*  bd_addr_ptr
)
{

  bt_cmd_msg_type*    cmd_ptr;
  bt_cmd_status_type  cmd_status = BT_CS_GN_CMD_Q_FULL;

  if ( (cmd_ptr = bt_get_free_cmd()) != NULL )
  {
    cmd_ptr->cmd_hdr.cmd_type             = BT_CMD_RM_ACCEPT_ACL;
    cmd_ptr->cmd_hdr.bt_app_id            = bt_app_id;
    cmd_ptr->cmd_msg.cmd_rm_acpta.bd_addr = *bd_addr_ptr;
  
    cmd_status = bt_cmd( cmd_ptr );
  }

  return( cmd_status );

}


/*===========================================================================

FUNCTION
  bt_cmd_rm_reject_acl

DESCRIPTION
  This function sends a RM Reject ACL command to the Bluetooth driver.

===========================================================================*/
bt_cmd_status_type bt_cmd_rm_reject_acl
(
  bt_app_id_type         bt_app_id,
  bt_bd_addr_type*       bd_addr_ptr,
  bt_rm_rej_reason_type  reason
)
{
  bt_cmd_msg_type*    cmd_ptr;
  bt_cmd_status_type  cmd_status = BT_CS_GN_CMD_Q_FULL;

  if ( (cmd_ptr = bt_get_free_cmd()) != NULL )
  {
    cmd_ptr->cmd_hdr.cmd_type            = BT_CMD_RM_REJECT_ACL;
    cmd_ptr->cmd_hdr.bt_app_id           = bt_app_id;
    cmd_ptr->cmd_msg.cmd_rm_reja.bd_addr = *bd_addr_ptr;
    cmd_ptr->cmd_msg.cmd_rm_reja.reason  = reason;
  
    cmd_status = bt_cmd( cmd_ptr );
  }

  return( cmd_status );

}


/*===========================================================================

FUNCTION
  bt_cmd_rm_unregister_acl

DESCRIPTION
  This function sends a RM Unregister ACL command to the Bluetooth driver.

===========================================================================*/
bt_cmd_status_type bt_cmd_rm_unregister_acl
(
  bt_app_id_type  bt_app_id
)
{

  bt_cmd_msg_type*    cmd_ptr;
  bt_cmd_status_type  cmd_status = BT_CS_GN_CMD_Q_FULL;

  if ( (cmd_ptr = bt_get_free_cmd()) != NULL )
  {
    cmd_ptr->cmd_hdr.cmd_type  = BT_CMD_RM_UNREGISTER_ACL;
    cmd_ptr->cmd_hdr.bt_app_id = bt_app_id;
  
    cmd_status = bt_cmd( cmd_ptr );
  }

  return( cmd_status );

}


/*===========================================================================

FUNCTION
  bt_cmd_rm_tx_data_acl

DESCRIPTION
  This function sends a RM Transmit Data ACL command to the Bluetooth
  driver.

===========================================================================*/
bt_cmd_status_type bt_cmd_rm_tx_data_acl
(
  bt_app_id_type               bt_app_id,
  bt_rm_handle_type            handle,
  bt_hc_pkt_boundary_flag_type l2cap_pkt_start,
  word                         out_flush_to,
  dsm_item_type*               dsm_ptr
)
{

  bt_cmd_msg_type*    cmd_ptr;
  bt_cmd_status_type  cmd_status = BT_CS_GN_CMD_Q_FULL;

  if ( (cmd_ptr = bt_get_free_cmd()) != NULL )
  {
    cmd_ptr->cmd_hdr.cmd_type                    = BT_CMD_RM_TX_DATA_ACL;
    cmd_ptr->cmd_hdr.bt_app_id                   = bt_app_id;
    cmd_ptr->cmd_msg.cmd_rm_txda.handle          = handle;
    cmd_ptr->cmd_msg.cmd_rm_txda.l2cap_pkt_start = l2cap_pkt_start;
    cmd_ptr->cmd_msg.cmd_rm_txda.flush_timeout   = out_flush_to;
    cmd_ptr->cmd_msg.cmd_rm_txda.dsm_ptr         = dsm_ptr;
  
    cmd_status = bt_cmd( cmd_ptr );
  }

  return( cmd_status );

}


/*===========================================================================

FUNCTION
  bt_cmd_rm_disconnect_acl

DESCRIPTION
  This function sends a RM Disconnect ACL command to the Bluetooth driver.

===========================================================================*/
bt_cmd_status_type bt_cmd_rm_disconnect_acl
(
  bt_app_id_type          bt_app_id,
  bt_rm_handle_type       handle,
  bt_rm_disc_reason_type  reason
)
{

  bt_cmd_msg_type*    cmd_ptr;
  bt_cmd_status_type  cmd_status = BT_CS_GN_CMD_Q_FULL;

  if ( (cmd_ptr = bt_get_free_cmd()) != NULL )
  {
    cmd_ptr->cmd_hdr.cmd_type            = BT_CMD_RM_DISCONNECT_ACL;
    cmd_ptr->cmd_hdr.bt_app_id           = bt_app_id;
    cmd_ptr->cmd_msg.cmd_rm_disca.handle = handle;
    cmd_ptr->cmd_msg.cmd_rm_disca.reason = reason;
  
    cmd_status = bt_cmd( cmd_ptr );
  }

  return( cmd_status );

}


/*===========================================================================

FUNCTION
  bt_cmd_rm_connect_sco

DESCRIPTION
  This function sends a RM Connect SCO command to the Bluetooth driver.

===========================================================================*/
bt_cmd_status_type bt_cmd_rm_connect_sco
(
  bt_app_id_type    bt_app_id,
  bt_bd_addr_type*  bd_addr_ptr,
  boolean           attempt_esco
)
{

  bt_cmd_msg_type*    cmd_ptr;
  bt_cmd_status_type  cmd_status = BT_CS_GN_CMD_Q_FULL;

  if ( (cmd_ptr = bt_get_free_cmd()) != NULL )
  {
    cmd_ptr->cmd_hdr.cmd_type                  = BT_CMD_RM_CONNECT_SCO;
    cmd_ptr->cmd_hdr.bt_app_id                 = bt_app_id;
    cmd_ptr->cmd_msg.cmd_rm_conns.bd_addr      = *bd_addr_ptr;
    cmd_ptr->cmd_msg.cmd_rm_conns.attempt_esco = attempt_esco;
  
    cmd_status = bt_cmd( cmd_ptr );
  }

  return( cmd_status );

}


/*===========================================================================

FUNCTION
  bt_cmd_rm_register_sco

DESCRIPTION
  This function sends a RM Register SCO command to the Bluetooth driver.

===========================================================================*/
bt_cmd_status_type bt_cmd_rm_register_sco
(
  bt_app_id_type  bt_app_id
)
{

  bt_cmd_msg_type*    cmd_ptr;
  bt_cmd_status_type  cmd_status = BT_CS_GN_CMD_Q_FULL;

  if ( (cmd_ptr = bt_get_free_cmd()) != NULL )
  {
    cmd_ptr->cmd_hdr.cmd_type  = BT_CMD_RM_REGISTER_SCO;
    cmd_ptr->cmd_hdr.bt_app_id = bt_app_id;
  
    cmd_status = bt_cmd( cmd_ptr );
  }

  return( cmd_status );

}


/*===========================================================================

FUNCTION
  bt_cmd_rm_accept_sco

DESCRIPTION
  This function sends a RM Accept SCO command to the Bluetooth driver.

===========================================================================*/
bt_cmd_status_type bt_cmd_rm_accept_sco
(
  bt_app_id_type    bt_app_id,
  bt_bd_addr_type*  bd_addr_ptr
)
{

  bt_cmd_msg_type*    cmd_ptr;
  bt_cmd_status_type  cmd_status = BT_CS_GN_CMD_Q_FULL;

  if ( (cmd_ptr = bt_get_free_cmd()) != NULL )
  {
    cmd_ptr->cmd_hdr.cmd_type             = BT_CMD_RM_ACCEPT_SCO;
    cmd_ptr->cmd_hdr.bt_app_id            = bt_app_id;
    cmd_ptr->cmd_msg.cmd_rm_acpts.bd_addr = *bd_addr_ptr;
  
    cmd_status = bt_cmd( cmd_ptr );
  }

  return( cmd_status );

}


/*===========================================================================

FUNCTION
  bt_cmd_rm_reject_sco

DESCRIPTION
  This function sends a RM Reject SCO command to the Bluetooth driver.

===========================================================================*/
bt_cmd_status_type bt_cmd_rm_reject_sco
(
  bt_app_id_type         bt_app_id,
  bt_bd_addr_type*       bd_addr_ptr,
  bt_rm_rej_reason_type  reason
)
{
  bt_cmd_msg_type*    cmd_ptr;
  bt_cmd_status_type  cmd_status = BT_CS_GN_CMD_Q_FULL;

  if ( (cmd_ptr = bt_get_free_cmd()) != NULL )
  {
    cmd_ptr->cmd_hdr.cmd_type            = BT_CMD_RM_REJECT_SCO;
    cmd_ptr->cmd_hdr.bt_app_id           = bt_app_id;
    cmd_ptr->cmd_msg.cmd_rm_rejs.bd_addr = *bd_addr_ptr;
    cmd_ptr->cmd_msg.cmd_rm_rejs.reason  = reason;
  
    cmd_status = bt_cmd( cmd_ptr );
  }

  return( cmd_status );

}


/*===========================================================================

FUNCTION
  bt_cmd_rm_unregister_sco

DESCRIPTION
  This function sends a RM Unregister SCO command to the Bluetooth
  driver.

===========================================================================*/
bt_cmd_status_type bt_cmd_rm_unregister_sco
(
  bt_app_id_type  bt_app_id
)
{

  bt_cmd_msg_type*    cmd_ptr;
  bt_cmd_status_type  cmd_status = BT_CS_GN_CMD_Q_FULL;

  if ( (cmd_ptr = bt_get_free_cmd()) != NULL )
  {
    cmd_ptr->cmd_hdr.cmd_type  = BT_CMD_RM_UNREGISTER_SCO;
    cmd_ptr->cmd_hdr.bt_app_id = bt_app_id;
  
    cmd_status = bt_cmd( cmd_ptr );
  }

  return( cmd_status );

}


/*===========================================================================

FUNCTION
  bt_cmd_rm_tx_data_sco

DESCRIPTION
  This function sends a RM Transmit Data SCO command to the Bluetooth
  driver.

===========================================================================*/
bt_cmd_status_type bt_cmd_rm_tx_data_sco
(
  bt_app_id_type     bt_app_id,
  bt_rm_handle_type  handle,
  dsm_item_type*     dsm_ptr
)
{

  bt_cmd_msg_type*    cmd_ptr;
  bt_cmd_status_type  cmd_status = BT_CS_GN_CMD_Q_FULL;

  if ( (cmd_ptr = bt_get_free_cmd()) != NULL )
  {
    cmd_ptr->cmd_hdr.cmd_type            = BT_CMD_RM_TX_DATA_SCO;
    cmd_ptr->cmd_hdr.bt_app_id           = bt_app_id;
    cmd_ptr->cmd_msg.cmd_rm_txds.handle  = handle;
    cmd_ptr->cmd_msg.cmd_rm_txds.dsm_ptr = dsm_ptr;
  
    cmd_status = bt_cmd( cmd_ptr );
  }

  return( cmd_status );

}


/*===========================================================================

FUNCTION
  bt_cmd_rm_disconnect_sco

DESCRIPTION
  This function sends a RM Disconnect SCO command to the Bluetooth driver.

===========================================================================*/
bt_cmd_status_type bt_cmd_rm_disconnect_sco
(
  bt_app_id_type          bt_app_id,
  bt_rm_handle_type       handle,
  bt_rm_disc_reason_type  reason
)
{

  bt_cmd_msg_type*    cmd_ptr;
  bt_cmd_status_type  cmd_status = BT_CS_GN_CMD_Q_FULL;

  if ( (cmd_ptr = bt_get_free_cmd()) != NULL )
  {
    cmd_ptr->cmd_hdr.cmd_type            = BT_CMD_RM_DISCONNECT_SCO;
    cmd_ptr->cmd_hdr.bt_app_id           = bt_app_id;
    cmd_ptr->cmd_msg.cmd_rm_discs.handle = handle;
    cmd_ptr->cmd_msg.cmd_rm_discs.reason = reason;
  
    cmd_status = bt_cmd( cmd_ptr );
  }

  return( cmd_status );

}


#ifdef FEATURE_BT_TELEC
/*===========================================================================

FUNCTION
  bt_cmd_rm_start_telec_test

DESCRIPTION
  This function sends a RM Start TELEC Test command to the
  Bluetooth driver.

===========================================================================*/
bt_cmd_status_type bt_cmd_rm_start_telec_test
(
  bt_app_id_type  bt_app_id,
  boolean         transmit,
  boolean         hopping
)
{

  bt_cmd_msg_type*    cmd_ptr;
  bt_cmd_status_type  cmd_status = BT_CS_GN_CMD_Q_FULL;

  if ( (cmd_ptr = bt_get_free_cmd()) != NULL )
  {
    cmd_ptr->cmd_hdr.cmd_type              = BT_CMD_RM_START_TELEC_TEST;
    cmd_ptr->cmd_hdr.bt_app_id             = bt_app_id;
    cmd_ptr->cmd_msg.cmd_rm_telec.transmit = transmit;
    cmd_ptr->cmd_msg.cmd_rm_telec.hopping  = hopping;
  
    cmd_status = bt_cmd( cmd_ptr );
  }

  return( cmd_status );

}
#endif


/*===========================================================================

FUNCTION
  bt_cmd_rm_disable_radio

DESCRIPTION
  This function sends a RM Disable Radio command to the Bluetooth Driver.

===========================================================================*/
bt_cmd_status_type bt_cmd_rm_disable_radio
(
  bt_app_id_type  bt_app_id,
  boolean         disable
)
{

  bt_cmd_status_type  cmd_status = BT_CS_GN_CMD_Q_FULL;

#ifdef BT_SWDEV_RADIO_OFF

  bt_cmd_msg_type*    cmd_ptr;

  if ( (cmd_ptr = bt_get_free_cmd()) != NULL )
  {
    cmd_ptr->cmd_hdr.cmd_type             = BT_CMD_RM_DISABLE_RADIO;
    cmd_ptr->cmd_hdr.bt_app_id            = bt_app_id;
    cmd_ptr->cmd_msg.cmd_rm_dsrad.disable = disable;
  
    cmd_status = bt_cmd( cmd_ptr );
  }

#else

  cmd_status = BT_CS_RM_UNRECOGNIZED_CMD;

#endif /* BT_SWDEV_RADIO_OFF */

  return( cmd_status );


}


/*===========================================================================

FUNCTION
  bt_cmd_rm_disable_discoverability

DESCRIPTION
  This function sends a RM Disable Discoverability command to the
  Bluetooth Driver.

===========================================================================*/
bt_cmd_status_type bt_cmd_rm_disable_discoverability
(
  bt_app_id_type  bt_app_id,
  boolean         disable
)
{

  bt_cmd_status_type  cmd_status = BT_CS_GN_CMD_Q_FULL;

#ifdef BT_SWDEV_RADIO_OFF

  bt_cmd_msg_type*    cmd_ptr;

  if ( (cmd_ptr = bt_get_free_cmd()) != NULL )
  {
    cmd_ptr->cmd_hdr.cmd_type  = BT_CMD_RM_DISABLE_DISCOVERABILITY;
    cmd_ptr->cmd_hdr.bt_app_id = bt_app_id;

    cmd_ptr->cmd_msg.cmd_rm_dsdcv.disable = disable;
  
    cmd_status = bt_cmd( cmd_ptr );
  }

#else

  cmd_status = BT_CS_RM_UNRECOGNIZED_CMD;

#endif /* BT_SWDEV_RADIO_OFF */

  return( cmd_status );

}


/*===========================================================================

FUNCTION
  bt_cmd_rm_set_device_black_list

DESCRIPTION
  This function sends a RM Set Device Black List command to the
  Bluetooth Driver.

===========================================================================*/
bt_cmd_status_type bt_cmd_rm_set_device_black_list
(
  bt_app_id_type   bt_app_id,
  uint16           dev_black_list_count,
  bt_bd_addr_type  dev_black_list[]
)
{

  bt_cmd_status_type  cmd_status = BT_CS_RM_BAD_PARAM;

#ifdef BT_SWDEV_RADIO_OFF

  bt_cmd_msg_type*    cmd_ptr;
  uint16              i;

  if ( dev_black_list_count <= BT_RM_MAX_DEV_BLACK_LIST_COUNT )
  {
    cmd_status = BT_CS_GN_CMD_Q_FULL;

    if ( (cmd_ptr = bt_get_free_cmd()) != NULL )
    {
      cmd_ptr->cmd_hdr.cmd_type  = BT_CMD_RM_SET_DEVICE_BLACK_LIST;
      cmd_ptr->cmd_hdr.bt_app_id = bt_app_id;

      for ( i = 0; i < dev_black_list_count; i++ )
      {
        cmd_ptr->cmd_msg.cmd_rm_setbl.
          dev_black_list[ i ] = dev_black_list[ i ];
      }

      cmd_ptr->cmd_msg.cmd_rm_setbl.dev_black_list_count =
        dev_black_list_count;

      cmd_status = bt_cmd( cmd_ptr );
    }
  }

#else

  cmd_status = BT_CS_RM_UNRECOGNIZED_CMD;

#endif /* BT_SWDEV_RADIO_OFF */

  return( cmd_status );

}


/*===========================================================================

FUNCTION
  bt_cmd_rm_set_device_white_list

DESCRIPTION
  This function sends a RM Set Device White List command to the
  Bluetooth Driver.

===========================================================================*/
bt_cmd_status_type bt_cmd_rm_set_device_white_list
(
  bt_app_id_type   bt_app_id,
  uint16           dev_white_list_count,
  bt_bd_addr_type  dev_white_list[]
)
{

  bt_cmd_status_type  cmd_status = BT_CS_RM_BAD_PARAM;

#ifdef BT_SWDEV_RADIO_OFF

  bt_cmd_msg_type*    cmd_ptr;
  uint16              i;

  if ( dev_white_list_count <= BT_RM_MAX_DEV_WHITE_LIST_COUNT )
  {
    cmd_status = BT_CS_GN_CMD_Q_FULL;

    if ( (cmd_ptr = bt_get_free_cmd()) != NULL )
    {
      cmd_ptr->cmd_hdr.cmd_type  = BT_CMD_RM_SET_DEVICE_WHITE_LIST;
      cmd_ptr->cmd_hdr.bt_app_id = bt_app_id;

      for ( i = 0; i < dev_white_list_count; i++ )
      {
        cmd_ptr->cmd_msg.cmd_rm_setwl.
          dev_white_list[ i ] = dev_white_list[ i ];
      }

      cmd_ptr->cmd_msg.cmd_rm_setwl.dev_white_list_count =
        dev_white_list_count;

      cmd_status = bt_cmd( cmd_ptr );
    }
  }

#else

  cmd_status = BT_CS_RM_UNRECOGNIZED_CMD;

#endif /* BT_SWDEV_RADIO_OFF */
  
  return( cmd_status );

}


#ifdef FEATURE_BT_1_2
/*===========================================================================

FUNCTION
  bt_cmd_rm_config_afh

DESCRIPTION
  This function sends a RM Configure AFH command to the Bluetooth driver.

===========================================================================*/
bt_cmd_status_type bt_cmd_rm_config_afh
(
  bt_app_id_type bt_app_id,
  uint16         afh_period,
  uint16         retx_threshold,
  uint16         crc_err_threshold
)
{ 

  bt_cmd_msg_type*    cmd_ptr;
  bt_cmd_status_type  cmd_status = BT_CS_GN_CMD_Q_FULL;

  if ( (cmd_ptr = bt_get_free_cmd()) != NULL )
  {
    cmd_ptr->cmd_hdr.cmd_type                            = BT_CMD_RM_CONFIG_AFH;
    cmd_ptr->cmd_hdr.bt_app_id                           = bt_app_id;
    cmd_ptr->cmd_msg.cmd_rm_config_afh.afh_period        = afh_period;
    cmd_ptr->cmd_msg.cmd_rm_config_afh.retx_threshold    = retx_threshold;
    cmd_ptr->cmd_msg.cmd_rm_config_afh.crc_err_threshold = crc_err_threshold;
  
    cmd_status = bt_cmd( cmd_ptr );
  }

  return( cmd_status );
}


/*===========================================================================

FUNCTION
  bt_cmd_rm_enable_afh

DESCRIPTION
  This function sends a RM Enable AFH command to the Bluetooth driver.

===========================================================================*/
bt_cmd_status_type bt_cmd_rm_enable_afh
(
  bt_app_id_type           bt_app_id,
  boolean                  enable,
  bt_rm_afh_threshold_type level
)
{

  bt_cmd_msg_type*    cmd_ptr;
  bt_cmd_status_type  cmd_status = BT_CS_GN_CMD_Q_FULL;

  if ( (cmd_ptr = bt_get_free_cmd()) != NULL )
  {
    cmd_ptr->cmd_hdr.cmd_type                 = BT_CMD_RM_ENABLE_AFH;
    cmd_ptr->cmd_hdr.bt_app_id                = bt_app_id;
    cmd_ptr->cmd_msg.cmd_rm_enable_afh.enable = enable;
    cmd_ptr->cmd_msg.cmd_rm_enable_afh.level  = level;
  
    cmd_status = bt_cmd( cmd_ptr );
  }

  return( cmd_status );
}


/*===========================================================================

FUNCTION
  bt_cmd_rm_set_afh_ch_map

DESCRIPTION
  This function sends a RM Set AFH Channel Map command to the Bluetooth driver.

===========================================================================*/
bt_cmd_status_type bt_cmd_rm_set_afh_ch_map
(
  bt_app_id_type      bt_app_id,
  bt_afh_ch_map_type* ch_map_ptr
)
{

  bt_cmd_msg_type*    cmd_ptr;
  bt_cmd_status_type  cmd_status = BT_CS_GN_CMD_Q_FULL;

  if ( (cmd_ptr = bt_get_free_cmd()) != NULL )
  {
    cmd_ptr->cmd_hdr.cmd_type                     = BT_CMD_RM_SET_AFH_CH_MAP;
    cmd_ptr->cmd_hdr.bt_app_id                    = bt_app_id;
    cmd_ptr->cmd_msg.cmd_rm_set_afh_ch_map.ch_map = *ch_map_ptr;
  
    cmd_status = bt_cmd( cmd_ptr );
  }

  return( cmd_status );
}


/*===========================================================================

FUNCTION
  bt_cmd_rm_get_afh_ch_map

DESCRIPTION
  This function sends a RM Get AFH Channel Map command to the Bluetooth driver.

===========================================================================*/
bt_cmd_status_type bt_cmd_rm_get_afh_ch_map
(
  bt_app_id_type      bt_app_id 
)
{

  bt_cmd_msg_type*    cmd_ptr;
  bt_cmd_status_type  cmd_status = BT_CS_GN_CMD_Q_FULL;

  if ( (cmd_ptr = bt_get_free_cmd()) != NULL )
  {
    cmd_ptr->cmd_hdr.cmd_type  = BT_CMD_RM_GET_AFH_CH_MAP;
    cmd_ptr->cmd_hdr.bt_app_id = bt_app_id;
  
    cmd_status = bt_cmd( cmd_ptr );
  }

  return( cmd_status );
}

#endif /* FEATURE_BT_1_2 */

#ifdef BT_SWDEV_2_1_PB
/*===========================================================================

FUNCTION
  bt_cmd_rm_set_auto_flush_to

DESCRIPTION
  This function sends the Automatic Flush time out value to the RM.

===========================================================================*/
bt_cmd_status_type bt_cmd_rm_set_auto_flush_to
(
  uint32             flush_to,
  bt_rm_handle_type  handle
)
{
  bt_cmd_msg_type*    cmd_ptr;
  bt_cmd_status_type  cmd_status = BT_CS_GN_CMD_Q_FULL;

  if ( (cmd_ptr = bt_get_free_cmd()) != NULL )
  {
    cmd_ptr->cmd_hdr.cmd_type  = BT_CMD_RM_SET_AUTO_FLUSH_TO;
    cmd_ptr->cmd_hdr.bt_app_id = bt_dc_last_hci_ev_reg_app_id;
    cmd_ptr->cmd_msg.cmd_rm_set_auto_flush_to.flush_to  = flush_to;
    cmd_ptr->cmd_msg.cmd_rm_set_auto_flush_to.rm_handle = handle;
  
    cmd_status = bt_cmd( cmd_ptr );
  }

  return( cmd_status );
}

#endif /* BT_SWDEV_2_1_PB */

#ifdef BT_SWDEV_2_1_SSP
/*===========================================================================

FUNCTION
  bt_cmd_rm_set_io_cap

DESCRIPTION
  This function sends a RM Set IO Capability command to the Bluetooth driver.

===========================================================================*/
bt_cmd_status_type bt_cmd_rm_set_io_cap
(
  bt_app_id_type    bt_app_id,
  bt_rm_io_cap_type io_cap 
)
{
  bt_cmd_msg_type*    cmd_ptr;
  bt_cmd_status_type  cmd_status = BT_CS_GN_CMD_Q_FULL;

  if ( (cmd_ptr = bt_get_free_cmd()) != NULL )
  {
    cmd_ptr->cmd_hdr.cmd_type             = BT_CMD_RM_SET_IO_CAP;
    cmd_ptr->cmd_hdr.bt_app_id            = bt_app_id;
    cmd_ptr->cmd_msg.cmd_rm_siocap.io_cap = io_cap;

    cmd_status = bt_cmd( cmd_ptr );
  }

  return( cmd_status );

}

/*===========================================================================

FUNCTION
  bt_cmd_rm_set_sm4

DESCRIPTION
  This function sends a RM Set SM4 command to the Bluetooth driver.

===========================================================================*/
bt_cmd_status_type bt_cmd_rm_set_sm4
(
  bt_app_id_type       bt_app_id,
  bt_service_id_type*  service_id_ptr,
  bt_sec_level_type    min_security,
  boolean              authorize,
  boolean              authorize_first 
)
{
  bt_cmd_msg_type*    cmd_ptr;
  bt_cmd_status_type  cmd_status = BT_CS_GN_CMD_Q_FULL;

  if ( (cmd_ptr = bt_get_free_cmd()) != NULL )
  {
    cmd_ptr->cmd_hdr.cmd_type            = BT_CMD_RM_SET_SM4;
    cmd_ptr->cmd_hdr.bt_app_id           = bt_app_id;
    cmd_ptr->cmd_msg.cmd_rm_setsssm4.service_id      = *service_id_ptr;
    cmd_ptr->cmd_msg.cmd_rm_setsssm4.authorize       = authorize;
    cmd_ptr->cmd_msg.cmd_rm_setsssm4.authorize_first = authorize_first;
    cmd_ptr->cmd_msg.cmd_rm_setsssm4.sec_level       = min_security;

    cmd_status = bt_cmd( cmd_ptr );
  }

  return( cmd_status );
}


/*===========================================================================

FUNCTION
  bt_cmd_rm_wr_remote_oob_data

DESCRIPTION
  This function sends a RM Wr Remote OOB command to the Bluetooth driver.

===========================================================================*/
bt_cmd_status_type bt_cmd_rm_wr_remote_oob_data
(
  bt_app_id_type     bt_app_id,
  uint8              size_oob_data,
  const void*        ptr_oob_data
)
{
  bt_cmd_msg_type*    cmd_ptr;
  bt_cmd_status_type  cmd_status = BT_CS_RM_BAD_PARAM;

  /* Writes remote side OOB data to driver */
  if ( ptr_oob_data != NULL &&
       size_oob_data <= BT_MAX_OOB_DATA_SIZE )
  {
    cmd_status = BT_CS_GN_CMD_Q_FULL;

    if ( (cmd_ptr = bt_get_free_cmd()) != NULL )
    {
      cmd_ptr->cmd_hdr.cmd_type   = BT_CMD_RM_WR_REMOTE_OOB_DATA;
      cmd_ptr->cmd_hdr.bt_app_id  = bt_app_id;

      memcpy ( (void *)cmd_ptr->cmd_msg.cmd_rm_wro.oob_data,
               (void *)ptr_oob_data, size_oob_data );

      cmd_status = bt_cmd( cmd_ptr );
    }
  }

  return( cmd_status );
}


/*===========================================================================

FUNCTION
  bt_cmd_rm_create_local_oob_data

DESCRIPTION
  This function sends a RM Create OOB Data command to the Bluetooth driver.

===========================================================================*/
bt_cmd_status_type bt_cmd_rm_create_local_oob_data
(
  bt_app_id_type     bt_app_id
)
{
  bt_cmd_msg_type*    cmd_ptr;
  bt_cmd_status_type  cmd_status = BT_CS_GN_CMD_Q_FULL;

  if ( (cmd_ptr = bt_get_free_cmd()) != NULL )
  {
    cmd_ptr->cmd_hdr.cmd_type   = BT_CMD_RM_CREATE_LOCAL_OOB_DATA;
    cmd_ptr->cmd_hdr.bt_app_id  = bt_app_id;

    cmd_status = bt_cmd( cmd_ptr );
  }

  return( cmd_status );
}


/*===========================================================================

FUNCTION
  bt_cmd_rm_user_confirmation_reply

DESCRIPTION
  This function sends a RM User Confirmation Reply command to the 
  Bluetooth driver.

===========================================================================*/
bt_cmd_status_type bt_cmd_rm_user_confirmation_reply
(
  bt_app_id_type     bt_app_id,
  bt_bd_addr_type*   bd_addr_ptr,
  boolean            answer 
)
{
  bt_cmd_msg_type*    cmd_ptr;
  bt_cmd_status_type  cmd_status = BT_CS_GN_CMD_Q_FULL;

  if ( (cmd_ptr = bt_get_free_cmd()) != NULL )
  {
    cmd_ptr->cmd_hdr.cmd_type                = BT_CMD_RM_USER_CONFIRMATION_REPLY;
    cmd_ptr->cmd_hdr.bt_app_id               = bt_app_id;
    cmd_ptr->cmd_msg.cmd_rm_uconfrep.answer  = answer;
    cmd_ptr->cmd_msg.cmd_rm_uconfrep.bd_addr = *bd_addr_ptr;

    cmd_status = bt_cmd( cmd_ptr );
  }

  return( cmd_status );
}

/*===========================================================================

FUNCTION
  bt_cmd_rm_passkey_reply

DESCRIPTION
  This function sends a RM Passkey Reply command to the Bluetooth driver.

===========================================================================*/
bt_cmd_status_type bt_cmd_rm_passkey_reply
(
  bt_app_id_type     bt_app_id,
  bt_bd_addr_type*   bd_addr_ptr,
  const char         passkey[ BT_RM_SSP_PASSKEY_LEN ],
  boolean            valid 
)
{
  bt_cmd_msg_type*    cmd_ptr;
  bt_cmd_status_type  cmd_status = BT_CS_GN_CMD_Q_FULL;

  if ( (cmd_ptr = bt_get_free_cmd()) != NULL )
  {
    cmd_ptr->cmd_hdr.cmd_type   = BT_CMD_RM_PASSKEY_REPLY;
    cmd_ptr->cmd_hdr.bt_app_id  = bt_app_id;
    cmd_ptr->cmd_msg.cmd_rm_pkreply.bd_addr = *bd_addr_ptr;
    if ( valid != FALSE )
    {
      cmd_ptr->cmd_msg.cmd_rm_pkreply.passkey = (uint32)atol( (const char * )passkey );
    }
    cmd_ptr->cmd_msg.cmd_rm_pkreply.valid = valid;

    cmd_status = bt_cmd( cmd_ptr );
  }

  return( cmd_status );
}


/*===========================================================================

FUNCTION
  bt_cmd_rm_keypress_notification

DESCRIPTION
  This function sends a RM Keypress Notification command to the Bluetooth 
  driver.

===========================================================================*/
bt_cmd_status_type bt_cmd_rm_keypress_notification
(
  bt_app_id_type                   bt_app_id,
  bt_bd_addr_type*                 bd_addr_ptr,
  bt_rm_keypress_notification_type key 
)
{
  bt_cmd_msg_type*    cmd_ptr;
  bt_cmd_status_type  cmd_status = BT_CS_GN_CMD_Q_FULL;

  if ( (cmd_ptr = bt_get_free_cmd()) != NULL )
  {
    cmd_ptr->cmd_hdr.cmd_type   = BT_CMD_RM_KEYPRESS_NOTIFICATION;
    cmd_ptr->cmd_hdr.bt_app_id  = bt_app_id;
    cmd_ptr->cmd_msg.cmd_rm_kpn.bd_addr = *bd_addr_ptr;
    cmd_ptr->cmd_msg.cmd_rm_kpn.key = key;

    cmd_status = bt_cmd( cmd_ptr );
  }

  return( cmd_status );
}


/*===========================================================================

FUNCTION
  bt_cmd_rm_bond_ext

DESCRIPTION
  This function sends a RM Bond Extended command to the Bluetooth driver.

===========================================================================*/
bt_cmd_status_type bt_cmd_rm_bond_ext
(
  bt_app_id_type     bt_app_id,
  bt_bd_addr_type*   bd_addr_ptr,
  bt_pin_code_type*  pin_code_ptr,
  boolean            mitm_protection_required
)
{
  bt_cmd_msg_type*    cmd_ptr;
  bt_cmd_status_type  cmd_status = BT_CS_GN_CMD_Q_FULL;

  if ( (cmd_ptr = bt_get_free_cmd()) != NULL )
  {
    cmd_ptr->cmd_hdr.cmd_type            = BT_CMD_RM_BOND_EXT;
    cmd_ptr->cmd_hdr.bt_app_id           = bt_app_id;
    cmd_ptr->cmd_msg.cmd_rm_bondext.bd_addr = *bd_addr_ptr;
    cmd_ptr->cmd_msg.cmd_rm_bondext.pin     = *pin_code_ptr;
    cmd_ptr->cmd_msg.cmd_rm_bondext.mitm_protection_reqd = mitm_protection_required;

    cmd_status = bt_cmd( cmd_ptr );
  }

  return( cmd_status );

}

/*===========================================================================

FUNCTION
  bt_cmd_rm_authorize_rebond

DESCRIPTION
  This function sends a RM Authorize Rebond command to the Bluetooth driver.

===========================================================================*/
bt_cmd_status_type bt_cmd_rm_authorize_rebond
(
  bt_app_id_type     bt_app_id,
  bt_bd_addr_type*   bd_addr_ptr,
  boolean            rebond,
  bt_rm_authorize_bond_type bonding_requirements
)
{
  bt_cmd_msg_type*    cmd_ptr;
  bt_cmd_status_type  cmd_status = BT_CS_GN_CMD_Q_FULL;

  if ( (cmd_ptr = bt_get_free_cmd()) != NULL )
  {
    cmd_ptr->cmd_hdr.cmd_type            = BT_CMD_RM_AUTHORIZE_REBOND;
    cmd_ptr->cmd_hdr.bt_app_id           = bt_app_id;
    cmd_ptr->cmd_msg.cmd_rm_areb.bd_addr = *bd_addr_ptr;
    cmd_ptr->cmd_msg.cmd_rm_areb.bond_allowed = rebond;
    cmd_ptr->cmd_msg.cmd_rm_areb.bond_req     = bonding_requirements;

    cmd_status = bt_cmd( cmd_ptr );
  }

  return( cmd_status );
}

/*===========================================================================

FUNCTION
  bt_cmd_rm_set_sm4_debug_mode

DESCRIPTION
  This function sends a RM Set SM4 Debug Mode command to the Bluetooth driver.

===========================================================================*/
bt_cmd_status_type bt_cmd_rm_set_sm4_debug_mode
(
  bt_app_id_type     bt_app_id,
  boolean            enable_debug_mode
)
{
  bt_cmd_msg_type*    cmd_ptr;
  bt_cmd_status_type  cmd_status = BT_CS_GN_CMD_Q_FULL;

  if ( (cmd_ptr = bt_get_free_cmd()) != NULL )
  {
    cmd_ptr->cmd_hdr.cmd_type            = BT_CMD_RM_SET_SM4_DEBUG_MODE;
    cmd_ptr->cmd_hdr.bt_app_id           = bt_app_id;
    cmd_ptr->cmd_msg.cmd_rm_sm4debug.enable_debug_mode = enable_debug_mode;
  
    cmd_status = bt_cmd( cmd_ptr );
  }

  return( cmd_status );
}

#endif /* BT_SWDEV_2_1_SSP */

/*-------------------------------------------------------------------------*/
/*                Host Controller Interface (HCI) Commands                 */
/*-------------------------------------------------------------------------*/

/*===========================================================================

FUNCTION
  bt_cmd_hc_inquiry

DESCRIPTION
  This function sends an HCI Inquiry command to the Bluetooth driver.

===========================================================================*/
bt_cmd_status_type bt_cmd_hc_inquiry
(
  bt_lap_type  lap,
  uint8        inquiry_len,
  uint8        num_responses
)
{

  bt_cmd_msg_type*    cmd_ptr;
  bt_cmd_status_type  cmd_status = BT_CS_GN_CMD_Q_FULL;

  if ( (cmd_ptr = bt_get_free_cmd()) != NULL )
  {
    cmd_ptr->cmd_hdr.cmd_type  = BT_CMD_HC_INQUIRY;
    cmd_ptr->cmd_hdr.bt_app_id = bt_dc_last_hci_ev_reg_app_id;

    memcpy( (void*) cmd_ptr->cmd_msg.cmd_hc_inquiry.lap,
            lap, sizeof( bt_lap_type ) );
    cmd_ptr->cmd_msg.cmd_hc_inquiry.inquiry_len   = inquiry_len;
    cmd_ptr->cmd_msg.cmd_hc_inquiry.num_responses = num_responses;
  
    cmd_status = bt_cmd( cmd_ptr );
  }

  return( cmd_status );

}


/*===========================================================================

FUNCTION
  bt_cmd_hc_inquiry_cancel

DESCRIPTION
  This function sends an HCI Inquiry_Cancel command to the Bluetooth
  driver.

===========================================================================*/
bt_cmd_status_type bt_cmd_hc_inquiry_cancel
(
  void
)
{

  bt_cmd_msg_type*    cmd_ptr;
  bt_cmd_status_type  cmd_status = BT_CS_GN_CMD_Q_FULL;

  if ( (cmd_ptr = bt_get_free_cmd()) != NULL )
  {
    cmd_ptr->cmd_hdr.cmd_type  = BT_CMD_HC_INQUIRY_CANCEL;
    cmd_ptr->cmd_hdr.bt_app_id = bt_dc_last_hci_ev_reg_app_id;
  
    cmd_status = bt_cmd( cmd_ptr );
  }

  return( cmd_status );

}

#ifdef BT_SWDEV_2_1_EIR
/*===========================================================================

FUNCTION
  bt_cmd_hc_set_event_mask

DESCRIPTION
  This function sets the event mask in the controller

===========================================================================*/
bt_cmd_status_type bt_cmd_hc_set_event_mask
(
  bt_event_mask_type   event_mask
)
{
  uint8               i;
  bt_cmd_msg_type*    cmd_ptr;
  bt_cmd_status_type  cmd_status = BT_CS_GN_CMD_Q_FULL;

  if ( (cmd_ptr = bt_get_free_cmd()) != NULL )
  {
    cmd_ptr->cmd_hdr.cmd_type  = BT_CMD_HC_SET_EVENT_MASK;
    cmd_ptr->cmd_hdr.bt_app_id = bt_dc_last_hci_ev_reg_app_id;

    for(i=0; i<BT_EVENT_MASK_LEN; i++)
    {
      cmd_ptr->cmd_msg.cmd_hc_set_event_mask.event_mask[ i ] = event_mask.bytes[ i ];
    }
    cmd_status = bt_cmd( cmd_ptr );
  }

  return( cmd_status );
}
#endif /* BT_SWDEV_2_1_EIR */


#ifdef BT_SWDEV_2_1_EIR
/*===========================================================================

FUNCTION
  bt_cmd_hc_wr_inq_mode

DESCRIPTION
  This function writes the inquiry mode to the controller

===========================================================================*/
bt_cmd_status_type bt_cmd_hc_wr_inq_mode
(
  bt_inq_mode_type    inquiry_mode
)
{
  bt_cmd_msg_type*    cmd_ptr;
  bt_cmd_status_type  cmd_status = BT_CS_GN_CMD_Q_FULL;

  if ( (cmd_ptr = bt_get_free_cmd()) != NULL )
  {
    cmd_ptr->cmd_hdr.cmd_type  = BT_CMD_HC_WR_INQUIRY_MODE;
    cmd_ptr->cmd_hdr.bt_app_id = bt_dc_last_hci_ev_reg_app_id;

    cmd_ptr->cmd_msg.cmd_hc_wr_inq_mode.inq_mode = inquiry_mode;
    cmd_status = bt_cmd( cmd_ptr );
  }

  return( cmd_status );
}
#endif /* BT_SWDEV_2_1_EIR */


#ifdef BT_SWDEV_2_1_EIR
/*===========================================================================

FUNCTION
  bt_cmd_hc_wr_ext_inq_rsp

DESCRIPTION
  This function writes the EIR to be sent

===========================================================================*/
bt_cmd_status_type bt_cmd_hc_wr_ext_inq_rsp
(
  dsm_item_type*                dsm_ptr, 
  uint8                         fec_required
)
{
  bt_cmd_msg_type*    cmd_ptr;
  bt_cmd_status_type  cmd_status = BT_CS_GN_CMD_Q_FULL;

  if ( (cmd_ptr = bt_get_free_cmd()) != NULL )
  {
    cmd_ptr->cmd_hdr.cmd_type  = BT_CMD_HC_WR_EXT_INQ_RSP;
    cmd_ptr->cmd_hdr.bt_app_id = bt_dc_last_hci_ev_reg_app_id;

    cmd_ptr->cmd_msg.cmd_hc_wr_eir.fec_required = fec_required;
    cmd_ptr->cmd_msg.cmd_hc_wr_eir.dsm_ptr      = dsm_ptr;
  
    cmd_status = bt_cmd( cmd_ptr );
  }

  return( cmd_status );
}
#endif /* BT_SWDEV_2_1_EIR */


#ifdef BT_SWDEV_2_1_EIR
/*===========================================================================

FUNCTION
  bt_cmd_hc_rd_ext_inq_rsp

DESCRIPTION
  This function reads the EIR to be sent

===========================================================================*/
bt_cmd_status_type bt_cmd_hc_rd_ext_inq_rsp
(
  void
)
{
  bt_cmd_msg_type*    cmd_ptr;
  bt_cmd_status_type  cmd_status = BT_CS_GN_CMD_Q_FULL;

  if ( (cmd_ptr = bt_get_free_cmd()) != NULL )
  {
    cmd_ptr->cmd_hdr.cmd_type  = BT_CMD_HC_RD_EXT_INQ_RSP;
    cmd_ptr->cmd_hdr.bt_app_id = bt_dc_last_hci_ev_reg_app_id;
  
    cmd_status = bt_cmd( cmd_ptr );
  }

  return( cmd_status );
}
#endif /* BT_SWDEV_2_1_EIR */


#ifdef BT_SWDEV_2_1_EIR
/*===========================================================================

FUNCTION
  bt_cmd_hc_wr_inq_tx_pwr

DESCRIPTION
  This function writes the tx power level to transmit the inquiry(ID) packet

===========================================================================*/
bt_cmd_status_type bt_cmd_hc_wr_inq_tx_pwr
(
  uint8                         tx_power_level
)
{
  bt_cmd_msg_type*    cmd_ptr;
  bt_cmd_status_type  cmd_status = BT_CS_GN_CMD_Q_FULL;

  if ( (cmd_ptr = bt_get_free_cmd()) != NULL )
  {
    cmd_ptr->cmd_hdr.cmd_type  = BT_CMD_HC_WR_INQ_TX_PWR;
    cmd_ptr->cmd_hdr.bt_app_id = bt_dc_last_hci_ev_reg_app_id;

    cmd_ptr->cmd_msg.cmd_hc_wr_inq_tx_pwr.tx_power_level = tx_power_level;
  
    cmd_status = bt_cmd( cmd_ptr );
  }

  return( cmd_status );
}
#endif /* BT_SWDEV_2_1_EIR */


#ifdef BT_SWDEV_2_1_EIR
/*===========================================================================

FUNCTION
  bt_cmd_hc_rd_inq_rsp_tx_pwr

DESCRIPTION
  This function reads the inquiry response (FHS) tx power level

===========================================================================*/
bt_cmd_status_type bt_cmd_hc_rd_inq_rsp_tx_pwr
(
  void
)
{
  bt_cmd_msg_type*    cmd_ptr;
  bt_cmd_status_type  cmd_status = BT_CS_GN_CMD_Q_FULL;

  if ( (cmd_ptr = bt_get_free_cmd()) != NULL )
  {
    cmd_ptr->cmd_hdr.cmd_type  = BT_CMD_HC_RD_INQ_RSP_TX_PWR;
    cmd_ptr->cmd_hdr.bt_app_id = bt_dc_last_hci_ev_reg_app_id;

  
    cmd_status = bt_cmd( cmd_ptr );
  }

  return( cmd_status );
}
#endif /* BT_SWDEV_2_1_EIR */

/*===========================================================================

FUNCTION
  bt_cmd_hc_create_connection

DESCRIPTION
  This function sends an HCI create connection command to the Bluetooth
  driver.

===========================================================================*/
bt_cmd_status_type bt_cmd_hc_create_connection
(
  bt_bd_addr_type*            bd_addr_ptr,
  bt_bb_pkt_type              pkt_type,
  bt_page_scan_rep_mode_type  ps_rep_mode,
  bt_page_scan_mode_type      ps_mode,
  uint16                      clk_offset,
  bt_role_switch_type         role_switch
)
{

  bt_cmd_msg_type*    cmd_ptr;
  bt_cmd_status_type  cmd_status = BT_CS_GN_CMD_Q_FULL;

  if ( (cmd_ptr = bt_get_free_cmd()) != NULL )
  {
    cmd_ptr->cmd_hdr.cmd_type  = BT_CMD_HC_CREATE_CONNECTION;
    cmd_ptr->cmd_hdr.bt_app_id = bt_dc_last_hci_ev_reg_app_id;
    
    cmd_ptr->cmd_msg.cmd_hc_create_conn.bd_addr     = *bd_addr_ptr;
    cmd_ptr->cmd_msg.cmd_hc_create_conn.pkt_type    = pkt_type;
    cmd_ptr->cmd_msg.cmd_hc_create_conn.ps_rep_mode = ps_rep_mode;
    cmd_ptr->cmd_msg.cmd_hc_create_conn.ps_mode     = ps_mode;
    cmd_ptr->cmd_msg.cmd_hc_create_conn.clk_offset  = clk_offset;
    cmd_ptr->cmd_msg.cmd_hc_create_conn.role_switch = role_switch;
  
    cmd_status = bt_cmd( cmd_ptr );
  }

  return( cmd_status );

}


/*===========================================================================

FUNCTION
  bt_cmd_hc_rd_remote_feature

DESCRIPTION
  This function sends an HCI Read Remote Feature command to the Bluetooth
  driver.

===========================================================================*/
bt_cmd_status_type bt_cmd_hc_rd_remote_feature
(
  uint16  conn_hndl  /*  the connection handle  */
)
{

  bt_cmd_msg_type*    cmd_ptr;
  bt_cmd_status_type  cmd_status = BT_CS_GN_CMD_Q_FULL;

  if ( (cmd_ptr = bt_get_free_cmd()) != NULL )
  {
    cmd_ptr->cmd_hdr.cmd_type  = BT_CMD_HC_RD_REMOTE_FEATURES;
    cmd_ptr->cmd_hdr.bt_app_id = bt_dc_last_hci_ev_reg_app_id;

    cmd_ptr->cmd_msg.cmd_hc_conn_hndl.conn_hndl = conn_hndl;
  
    cmd_status = bt_cmd( cmd_ptr );
  }
  
  return( cmd_status );

}

#ifdef FEATURE_BT_2_1
/*===========================================================================

FUNCTION
  bt_cmd_hc_rd_remote_feature_ext

DESCRIPTION
  This function sends an HCI Read Remote Feature Ext command to the Bluetooth
  driver.

===========================================================================*/
bt_cmd_status_type bt_cmd_hc_rd_remote_feature_ext
(
  uint16  conn_hndl,  /*  the connection handle  */
  uint8   page_number
)
{

  bt_cmd_msg_type*    cmd_ptr;
  bt_cmd_status_type  cmd_status = BT_CS_GN_CMD_Q_FULL;

  if ( (cmd_ptr = bt_get_free_cmd()) != NULL )
  {
    cmd_ptr->cmd_hdr.cmd_type  = BT_CMD_HC_RD_REMOTE_EXT_FEATURES;
    cmd_ptr->cmd_hdr.bt_app_id = bt_dc_last_hci_ev_reg_app_id;

    cmd_ptr->cmd_msg.cmd_hc_rd_rmt_ext_feat.conn_hndl = conn_hndl;
    cmd_ptr->cmd_msg.cmd_hc_rd_rmt_ext_feat.page_number = page_number;
  
    cmd_status = bt_cmd( cmd_ptr );
  }
  
  return( cmd_status );

}
#endif /* FEATURE_BT_2_1 */

/*===========================================================================

FUNCTION
  bt_cmd_hc_rd_remote_version

DESCRIPTION
  This function sends an HCI Read Remote Version command to the Bluetooth
  driver.

===========================================================================*/
bt_cmd_status_type bt_cmd_hc_rd_remote_version
(
  uint16  conn_hndl  /*  the connection handle  */
)
{

  bt_cmd_msg_type*    cmd_ptr;
  bt_cmd_status_type  cmd_status = BT_CS_GN_CMD_Q_FULL;

  if ( (cmd_ptr = bt_get_free_cmd()) != NULL )
  {
    cmd_ptr->cmd_hdr.cmd_type  = BT_CMD_HC_RD_REMOTE_VERSION;
    cmd_ptr->cmd_hdr.bt_app_id = bt_dc_last_hci_ev_reg_app_id;

    cmd_ptr->cmd_msg.cmd_hc_conn_hndl.conn_hndl = conn_hndl;
  
    cmd_status = bt_cmd( cmd_ptr );
  }
  
  return( cmd_status );

}

/*===========================================================================

FUNCTION
  bt_cmd_hc_rd_clk_offset

DESCRIPTION
  This function sends a HCI Read Clock Offset command to the Bluetooth
  driver.

===========================================================================*/
bt_cmd_status_type bt_cmd_hc_rd_clk_offset
(
  uint16  conn_hndl  /*  the connection handle  */
)
{

  bt_cmd_msg_type*    cmd_ptr;
  bt_cmd_status_type  cmd_status = BT_CS_GN_CMD_Q_FULL;

  if ( (cmd_ptr = bt_get_free_cmd()) != NULL )
  {
    cmd_ptr->cmd_hdr.cmd_type  = BT_CMD_HC_RD_CLK_OFFSET;
    cmd_ptr->cmd_hdr.bt_app_id = bt_dc_last_hci_ev_reg_app_id;

    cmd_ptr->cmd_msg.cmd_hc_conn_hndl.conn_hndl = conn_hndl;
  
    cmd_status = bt_cmd( cmd_ptr );
  }
  
  return( cmd_status );

}



/*===========================================================================

FUNCTION
  bt_cmd_hc_accept_connection

DESCRIPTION
  This function sends an HCI accept connection command to the Bluetooth
  driver.

===========================================================================*/
bt_cmd_status_type bt_cmd_hc_accept_connection
(
  bt_bd_addr_type*  bd_addr_ptr,
  bt_role_type      role
)
{

  bt_cmd_msg_type*    cmd_ptr;
  bt_cmd_status_type  cmd_status = BT_CS_GN_CMD_Q_FULL;

  if ( (cmd_ptr = bt_get_free_cmd()) != NULL )
  {
    cmd_ptr->cmd_hdr.cmd_type  = BT_CMD_HC_ACCEPT_CONN;
    cmd_ptr->cmd_hdr.bt_app_id = bt_dc_last_hci_ev_reg_app_id;

    cmd_ptr->cmd_msg.cmd_hc_accept_conn.bd_addr = *bd_addr_ptr;
    cmd_ptr->cmd_msg.cmd_hc_accept_conn.role    = role;
  
    cmd_status = bt_cmd( cmd_ptr );
  }

  return( cmd_status );

}


/*===========================================================================

FUNCTION
  bt_cmd_hc_reject_connection

DESCRIPTION
  This function sends an HCI reject connection command to the Bluetooth
  driver.

===========================================================================*/
bt_cmd_status_type bt_cmd_hc_reject_connection
(
  bt_bd_addr_type*  bd_addr_ptr,
  uint8             reason
)
{

  bt_cmd_msg_type*    cmd_ptr;
  bt_cmd_status_type  cmd_status = BT_CS_GN_CMD_Q_FULL;

  if ( (cmd_ptr = bt_get_free_cmd()) != NULL )
  {
    cmd_ptr->cmd_hdr.cmd_type  = BT_CMD_HC_REJECT_CONN;
    cmd_ptr->cmd_hdr.bt_app_id = bt_dc_last_hci_ev_reg_app_id;
    
    cmd_ptr->cmd_msg.cmd_hc_reject_conn.bd_addr = *bd_addr_ptr;
    cmd_ptr->cmd_msg.cmd_hc_reject_conn.reason  = reason;
  
    cmd_status = bt_cmd( cmd_ptr );
  }

  return( cmd_status );

}


/*===========================================================================

FUNCTION
  bt_cmd_hc_tx_acl_data

DESCRIPTION
  This function sends an HCI TX ACL data command to the Bluetooth driver.

===========================================================================*/
bt_cmd_status_type bt_cmd_hc_tx_acl_data
(
  uint16                        conn_hndl,
  bt_hc_pkt_boundary_flag_type  pkt_boundary_flag,
  bt_hc_broadcast_flag_type     broadcast_flag,
  dsm_item_type*                dsm_ptr,
  uint16                        local_cid,
  void*                         frame_handle
)
{

  bt_cmd_msg_type*    cmd_ptr;
  bt_cmd_status_type  cmd_status = BT_CS_GN_CMD_Q_FULL;

  if ( (cmd_ptr = bt_get_free_cmd()) != NULL )
  {
    cmd_ptr->cmd_hdr.cmd_type  = BT_CMD_HC_TX_ACL_DATA;
    cmd_ptr->cmd_hdr.bt_app_id = bt_dc_last_hci_ev_reg_app_id;
  
    cmd_ptr->cmd_msg.cmd_hc_tx_acl_data.conn_hndl = conn_hndl;
    cmd_ptr->cmd_msg.cmd_hc_tx_acl_data.
                                pkt_boundary_flag = pkt_boundary_flag;
    cmd_ptr->cmd_msg.cmd_hc_tx_acl_data.
                                broadcast_flag    = broadcast_flag;
    cmd_ptr->cmd_msg.cmd_hc_tx_acl_data.dsm_ptr   = dsm_ptr;
    cmd_ptr->cmd_msg.cmd_hc_tx_acl_data.local_cid = local_cid;
    cmd_ptr->cmd_msg.cmd_hc_tx_acl_data.
                                   frame_handle   = frame_handle;
  
    cmd_status = bt_cmd( cmd_ptr );
  }

  return( cmd_status );

}

/*===========================================================================

FUNCTION
  bt_cmd_hc_tx_sco_data

DESCRIPTION
  This function sends an HCI TX SCO data command to the Bluetooth driver.

===========================================================================*/
bt_cmd_status_type bt_cmd_hc_tx_sco_data
(
  uint16          conn_hndl,
  dsm_item_type*  dsm_ptr
)
{

  bt_cmd_msg_type*    cmd_ptr;
  bt_cmd_status_type  cmd_status = BT_CS_GN_CMD_Q_FULL;

  if ( (cmd_ptr = bt_get_free_cmd()) != NULL )
  {
    cmd_ptr->cmd_hdr.cmd_type  = BT_CMD_HC_TX_SCO_DATA;
    cmd_ptr->cmd_hdr.bt_app_id = bt_dc_last_hci_ev_reg_app_id;

    cmd_ptr->cmd_msg.cmd_hc_tx_sco_data.conn_hndl = conn_hndl;
    cmd_ptr->cmd_msg.cmd_hc_tx_sco_data.dsm_ptr   = dsm_ptr;
  
    cmd_status = bt_cmd( cmd_ptr );
  }

  return( cmd_status );

}


/*===========================================================================

FUNCTION
  bt_cmd_hc_add_sco_connection

DESCRIPTION
  This function sends an HCI add SCO connection command to the Bluetooth
  driver.

===========================================================================*/
bt_cmd_status_type bt_cmd_hc_add_sco_connection
(
  uint16          conn_hndl,
  bt_bb_pkt_type  pkt_type
)
{

  bt_cmd_msg_type*    cmd_ptr;
  bt_cmd_status_type  cmd_status = BT_CS_GN_CMD_Q_FULL;

  if ( (cmd_ptr = bt_get_free_cmd()) != NULL )
  {
    cmd_ptr->cmd_hdr.cmd_type  = BT_CMD_HC_ADD_SCO_CONN;
    cmd_ptr->cmd_hdr.bt_app_id = bt_dc_last_hci_ev_reg_app_id;

    cmd_ptr->cmd_msg.cmd_hc_add_sco.conn_hndl = conn_hndl;
    cmd_ptr->cmd_msg.cmd_hc_add_sco.pkt_type  = pkt_type;
  
    cmd_status = bt_cmd( cmd_ptr );
  }

  return( cmd_status );

}


/*===========================================================================

FUNCTION
  bt_cmd_hc_disconnect

DESCRIPTION
  This function sends an HCI disconnect command to the Bluetooth driver.

===========================================================================*/
bt_cmd_status_type bt_cmd_hc_disconnect
(
  uint16  conn_hndl,
  uint8   reason
)
{

  bt_cmd_msg_type*    cmd_ptr;
  bt_cmd_status_type  cmd_status = BT_CS_GN_CMD_Q_FULL;

  if ( (cmd_ptr = bt_get_free_cmd()) != NULL )
  {
    cmd_ptr->cmd_hdr.cmd_type  = BT_CMD_HC_DISCONNECT;
    cmd_ptr->cmd_hdr.bt_app_id = bt_dc_last_hci_ev_reg_app_id;
  
    cmd_ptr->cmd_msg.cmd_hc_disconn.conn_hndl = conn_hndl;
    cmd_ptr->cmd_msg.cmd_hc_disconn.reason    = reason;
  
    cmd_status = bt_cmd( cmd_ptr );
  }

  return( cmd_status );

}


/*===========================================================================

FUNCTION
  bt_cmd_hc_sniff_mode

DESCRIPTION
  This function sends an HCI sniff mode command to the Bluetooth driver.

===========================================================================*/
bt_cmd_status_type bt_cmd_hc_sniff_mode
(
  uint16  conn_hndl,
  uint16  min_interval,
  uint16  max_interval,
  uint16  sniff_attempt,
  uint16  sniff_timeout
)
{

  bt_cmd_msg_type*    cmd_ptr;
  bt_cmd_status_type  cmd_status = BT_CS_GN_CMD_Q_FULL;

  if ( (cmd_ptr = bt_get_free_cmd()) != NULL )
  {
    cmd_ptr->cmd_hdr.cmd_type  = BT_CMD_HC_SNIFF_MODE;
    cmd_ptr->cmd_hdr.bt_app_id = bt_dc_last_hci_ev_reg_app_id;
  
    cmd_ptr->cmd_msg.cmd_hc_sniff_mode.conn_hndl = conn_hndl;
    cmd_ptr->cmd_msg.cmd_hc_sniff_mode.
                              sniff_min_interval = min_interval;
    cmd_ptr->cmd_msg.cmd_hc_sniff_mode.
                              sniff_max_interval = max_interval;
    cmd_ptr->cmd_msg.cmd_hc_sniff_mode.
                              sniff_attempt      = sniff_attempt;
    cmd_ptr->cmd_msg.cmd_hc_sniff_mode.
                              sniff_timeout      = sniff_timeout;
  
    cmd_status = bt_cmd( cmd_ptr );
  }

  return( cmd_status );

}


/*===========================================================================

FUNCTION
  bt_cmd_hc_exit_sniff_mode

DESCRIPTION
  This function sends an HCI exit sniff mode command to the Bluetooth
  driver.

===========================================================================*/
bt_cmd_status_type bt_cmd_hc_exit_sniff_mode
(
  uint16  conn_hndl
)
{

  bt_cmd_msg_type*    cmd_ptr;
  bt_cmd_status_type  cmd_status = BT_CS_GN_CMD_Q_FULL;

  if ( (cmd_ptr = bt_get_free_cmd()) != NULL )
  {
    cmd_ptr->cmd_hdr.cmd_type  = BT_CMD_HC_EXIT_SNIFF_MODE;
    cmd_ptr->cmd_hdr.bt_app_id = bt_dc_last_hci_ev_reg_app_id;

    cmd_ptr->cmd_msg.cmd_hc_exit_sniff_mode.conn_hndl = conn_hndl;
    
    cmd_status = bt_cmd( cmd_ptr );
  }

  return( cmd_status );

}


/*===========================================================================

FUNCTION
  bt_cmd_hc_exit_park_mode

DESCRIPTION
  This function sends an HCI exit park mode command to the Bluetooth
  driver.

===========================================================================*/
bt_cmd_status_type bt_cmd_hc_exit_park_mode
(
  uint16  conn_hndl
)
{

  bt_cmd_msg_type*    cmd_ptr;
  bt_cmd_status_type  cmd_status = BT_CS_GN_CMD_Q_FULL;

  if ( (cmd_ptr = bt_get_free_cmd()) != NULL )
  {
    cmd_ptr->cmd_hdr.cmd_type  = BT_CMD_HC_EXIT_PARK_MODE;
    cmd_ptr->cmd_hdr.bt_app_id = bt_dc_last_hci_ev_reg_app_id;

    cmd_ptr->cmd_msg.cmd_hc_exit_park_mode.conn_hndl = conn_hndl;

    cmd_status = bt_cmd( cmd_ptr );
  }

  return( cmd_status );

}


/*===========================================================================

FUNCTION
  bt_cmd_hc_link_key_req_reply_neg

DESCRIPTION
  This function sends an HCI Link Key Request Reply Negative command to
  the Bluetooth driver.

===========================================================================*/
bt_cmd_status_type bt_cmd_hc_link_key_req_reply_neg
(
  bt_bd_addr_type*  bd_addr_ptr
)
{

  bt_cmd_msg_type*    cmd_ptr;
  bt_cmd_status_type  cmd_status = BT_CS_GN_CMD_Q_FULL;

  if ( (cmd_ptr = bt_get_free_cmd()) != NULL )
  {
    cmd_ptr->cmd_hdr.cmd_type  = BT_CMD_HC_LINK_KEY_NEG;
    cmd_ptr->cmd_hdr.bt_app_id = bt_dc_last_hci_ev_reg_app_id;

    cmd_ptr->cmd_msg.cmd_hc_bd_addr.bd_addr = *bd_addr_ptr;
  
    cmd_status = bt_cmd( cmd_ptr );
  }

  return( cmd_status );

}


/*===========================================================================

FUNCTION
  bt_cmd_hc_link_key_req_reply

DESCRIPTION
  This function sends an HCI Link Key Request Reply command to
  the Bluetooth driver.

===========================================================================*/
bt_cmd_status_type bt_cmd_hc_link_key_req_reply
(
  bt_bd_addr_type*  bd_addr_ptr,
  bt_link_key_type  link_key
)
{

  bt_cmd_msg_type*    cmd_ptr;
  bt_cmd_status_type  cmd_status = BT_CS_GN_CMD_Q_FULL;

  if ( (cmd_ptr = bt_get_free_cmd()) != NULL )
  {
    cmd_ptr->cmd_hdr.cmd_type  = BT_CMD_HC_LINK_KEY;
    cmd_ptr->cmd_hdr.bt_app_id = bt_dc_last_hci_ev_reg_app_id;

    cmd_ptr->cmd_msg.cmd_hc_key.bd_addr = *bd_addr_ptr;
    memcpy( (byte*)(&cmd_ptr->cmd_msg.cmd_hc_key.link_key),
            (byte*)(&link_key), sizeof( bt_link_key_type ) );
  
    cmd_status = bt_cmd( cmd_ptr );
  }

  return( cmd_status );

}


/*===========================================================================

FUNCTION
  bt_cmd_hc_pin_code_req_reply

DESCRIPTION
  This function sends an HCI PIN code Request Reply command to
  the Bluetooth driver.

===========================================================================*/
bt_cmd_status_type bt_cmd_hc_pin_code_req_reply
(
  bt_bd_addr_type*  bd_addr_ptr,
  uint8             pin_code_len,
  uint8             pin_code[]
)
{

  bt_cmd_msg_type*    cmd_ptr;
  bt_cmd_status_type  cmd_status = BT_CS_GN_CMD_Q_FULL;

  if ( (cmd_ptr = bt_get_free_cmd()) != NULL )
  {
    cmd_ptr->cmd_hdr.cmd_type  = BT_CMD_HC_PIN_CODE;
    cmd_ptr->cmd_hdr.bt_app_id = bt_dc_last_hci_ev_reg_app_id;

    cmd_ptr->cmd_msg.cmd_hc_pin.bd_addr      = *bd_addr_ptr;
    cmd_ptr->cmd_msg.cmd_hc_pin.pin_code_len = pin_code_len;
    memcpy( (void*) cmd_ptr->cmd_msg.cmd_hc_pin.pin_code,
            pin_code, BT_MAX_PIN_CODE_LEN );
  
    cmd_status = bt_cmd( cmd_ptr );
  }

  return( cmd_status );

}


/*===========================================================================

FUNCTION
  bt_cmd_hc_pin_code_req_reply_neg

DESCRIPTION
  This function sends an HCI PIN code Request Reply Negative command to
  the Bluetooth driver.

===========================================================================*/
bt_cmd_status_type bt_cmd_hc_pin_code_req_reply_neg
(
  bt_bd_addr_type*  bd_addr_ptr
)
{

  bt_cmd_msg_type*    cmd_ptr;
  bt_cmd_status_type  cmd_status = BT_CS_GN_CMD_Q_FULL;

  if ( (cmd_ptr = bt_get_free_cmd()) != NULL )
  {
    cmd_ptr->cmd_hdr.cmd_type  = BT_CMD_HC_PIN_CODE_NEG;
    cmd_ptr->cmd_hdr.bt_app_id = bt_dc_last_hci_ev_reg_app_id;

    cmd_ptr->cmd_msg.cmd_hc_bd_addr.bd_addr = *bd_addr_ptr;
  
    cmd_status = bt_cmd( cmd_ptr );
  }

  return( cmd_status );

}


/*===========================================================================

FUNCTION
  bt_cmd_hc_authentication_requested

DESCRIPTION
  This function sends an HCI Authentication Request command to
  the Bluetooth driver.

===========================================================================*/
bt_cmd_status_type bt_cmd_hc_authentication_requested
(
  uint16  conn_hndl
)
{

  bt_cmd_msg_type*    cmd_ptr;
  bt_cmd_status_type  cmd_status = BT_CS_GN_CMD_Q_FULL;

  if ( (cmd_ptr = bt_get_free_cmd()) != NULL )
  {
    cmd_ptr->cmd_hdr.cmd_type  = BT_CMD_HC_AUTH_REQ;
    cmd_ptr->cmd_hdr.bt_app_id = bt_dc_last_hci_ev_reg_app_id;

    cmd_ptr->cmd_msg.cmd_hc_conn_hndl.conn_hndl = conn_hndl;
  
    cmd_status = bt_cmd( cmd_ptr );
  }

  return( cmd_status );

}


/*===========================================================================

FUNCTION
  bt_cmd_hc_remote_name_req

DESCRIPTION
  This function sends an HCI Remote Name Request command to
  the Bluetooth driver.

===========================================================================*/
bt_cmd_status_type bt_cmd_hc_remote_name_req
(
  bt_bd_addr_type             bd_addr,
  bt_page_scan_rep_mode_type  ps_rep_mode,
  bt_page_scan_mode_type      ps_mode,
  uint16                      clk_offset
)
{

  bt_cmd_msg_type*    cmd_ptr;
  bt_cmd_status_type  cmd_status = BT_CS_GN_CMD_Q_FULL;

  if ( (cmd_ptr = bt_get_free_cmd()) != NULL )
  {
    cmd_ptr->cmd_hdr.cmd_type  = BT_CMD_HC_REMOTE_NAME_REQ;
    cmd_ptr->cmd_hdr.bt_app_id = bt_dc_last_hci_ev_reg_app_id;

    cmd_ptr->cmd_msg.cmd_hc_remote_name_req.bd_addr     = bd_addr;
    cmd_ptr->cmd_msg.cmd_hc_remote_name_req.ps_rep_mode = ps_rep_mode;
    cmd_ptr->cmd_msg.cmd_hc_remote_name_req.ps_mode     = ps_mode;
    cmd_ptr->cmd_msg.cmd_hc_remote_name_req.clk_offset  = clk_offset;

    cmd_status = bt_cmd( cmd_ptr );
  }

  return( cmd_status );

}

/*===========================================================================

FUNCTION
  bt_cmd_hc_wr_link_supervision_timeout

DESCRIPTION
  This function sends an HCI Write Link Super vision timeout command to
  the Bluetooth driver.

===========================================================================*/
bt_cmd_status_type bt_cmd_hc_wr_link_supervision_timeout
(
  uint16                      conn_hndl,
  uint16                      link_supervision_to
)
{

  bt_cmd_msg_type*    cmd_ptr;
  bt_cmd_status_type  cmd_status = BT_CS_GN_CMD_Q_FULL;

  if ( (cmd_ptr = bt_get_free_cmd()) != NULL )
  {
    cmd_ptr->cmd_hdr.cmd_type  = BT_CMD_HC_WR_LINK_SUPERVISION_TO;
    cmd_ptr->cmd_hdr.bt_app_id = bt_dc_last_hci_ev_reg_app_id;

    cmd_ptr->cmd_msg.cmd_hc_wr_link_super_to.conn_hndl     = conn_hndl;
    cmd_ptr->cmd_msg.cmd_hc_wr_link_super_to.link_supervision_to = 
                                                        link_supervision_to;
    
    cmd_status = bt_cmd( cmd_ptr );
  }

  return( cmd_status );

}

/*===========================================================================

FUNCTION
  bt_cmd_hc_wr_scan_enable

DESCRIPTION
  This function sends an HCI write scan enable command to the Bluetooth
  driver.

===========================================================================*/
bt_cmd_status_type bt_cmd_hc_wr_scan_enable
(
  bt_hc_scan_enable_type  scan_enable
)
{

  bt_cmd_msg_type*    cmd_ptr;
  bt_cmd_status_type  cmd_status = BT_CS_GN_CMD_Q_FULL;

  if ( (cmd_ptr = bt_get_free_cmd()) != NULL )
  {
    cmd_ptr->cmd_hdr.cmd_type  = BT_CMD_HC_WR_SCAN_ENABLE;
    cmd_ptr->cmd_hdr.bt_app_id = bt_dc_last_hci_ev_reg_app_id;

    cmd_ptr->cmd_msg.cmd_hc_wr_scan_enable.scan_enable = scan_enable;
  
    cmd_status = bt_cmd( cmd_ptr );
  }

  return( cmd_status );

}


/*===========================================================================

FUNCTION
  bt_cmd_hc_wr_auth_enable

DESCRIPTION
  This function sends an HCI Authentication enable command to the
  Bluetooth driver.

===========================================================================*/
bt_cmd_status_type bt_cmd_hc_wr_auth_enable
(
  bt_hc_auth_enable_type  auth_enable
)
{

  bt_cmd_msg_type*    cmd_ptr;
  bt_cmd_status_type  cmd_status = BT_CS_GN_CMD_Q_FULL;

  if ( (cmd_ptr = bt_get_free_cmd()) != NULL )
  {
    cmd_ptr->cmd_hdr.cmd_type  = BT_CMD_HC_WR_AUTH_ENABLE;
    cmd_ptr->cmd_hdr.bt_app_id = bt_dc_last_hci_ev_reg_app_id;
  
    cmd_ptr->cmd_msg.cmd_hc_wr_auth_enable.auth_enable = auth_enable;
  
    cmd_status = bt_cmd( cmd_ptr );
  }
  
  return( cmd_status );

}


/*===========================================================================

FUNCTION
  bt_cmd_hc_wr_encrypt_mode

DESCRIPTION
  This function sends an HCI Encryption Mode command to the Bluetooth
  driver.

===========================================================================*/
bt_cmd_status_type bt_cmd_hc_wr_encrypt_mode
(
  bt_encrypt_mode_type  encrypt_mode
)
{

  bt_cmd_msg_type*    cmd_ptr;
  bt_cmd_status_type  cmd_status = BT_CS_GN_CMD_Q_FULL;

  if ( (cmd_ptr = bt_get_free_cmd()) != NULL )
  {
    cmd_ptr->cmd_hdr.cmd_type  = BT_CMD_HC_WR_ENCRYPT_MODE;
    cmd_ptr->cmd_hdr.bt_app_id = bt_dc_last_hci_ev_reg_app_id;
    
    cmd_ptr->cmd_msg.cmd_hc_wr_encrypt_mode.encrypt_mode = encrypt_mode;
  
    cmd_status = bt_cmd( cmd_ptr );
  }

  return( cmd_status );

}


/*===========================================================================

FUNCTION
  bt_cmd_hc_wr_class_of_device

DESCRIPTION
  This function sends an HCI write class of device command to the
  Bluetooth driver.

===========================================================================*/
bt_cmd_status_type bt_cmd_hc_wr_class_of_device
(
  bt_cod_type cod
)
{

  bt_cmd_msg_type*    cmd_ptr;
  bt_cmd_status_type  cmd_status = BT_CS_GN_CMD_Q_FULL;

  if ( (cmd_ptr = bt_get_free_cmd()) != NULL )
  {
    cmd_ptr->cmd_hdr.cmd_type  = BT_CMD_HC_WR_CLASS_OF_DEVICE;
    cmd_ptr->cmd_hdr.bt_app_id = bt_dc_last_hci_ev_reg_app_id;
    
    cmd_ptr->cmd_msg.cmd_hc_wr_class_of_dev.class_of_device = cod;
  
    cmd_status = bt_cmd( cmd_ptr );
  }

  return( cmd_status );

}


/*===========================================================================

FUNCTION
  bt_cmd_hc_wr_stored_link_key

DESCRIPTION
  This function sends an HCI write stored link key command to the
  Bluetooth driver.

===========================================================================*/
bt_cmd_status_type bt_cmd_hc_wr_stored_link_key
(
  uint8           num_keys_to_write,
  dsm_item_type*  dsm_ptr
    /*  dsm_ptr the equivalent of the following data:       */
    /*    bt_bd_addr_type   bd_addr[ num_keys_to_write ];   */
    /*    bt_link_key_type  link_key[ num_keys_to_write ];  */
)
{

  bt_cmd_msg_type*    cmd_ptr;
  bt_cmd_status_type  cmd_status = BT_CS_GN_CMD_Q_FULL;

  if ( (cmd_ptr = bt_get_free_cmd()) != NULL )
  {
    cmd_ptr->cmd_hdr.cmd_type  = BT_CMD_HC_WR_STORED_LINK_KEY;
    cmd_ptr->cmd_hdr.bt_app_id = bt_dc_last_hci_ev_reg_app_id;
    
    cmd_ptr->cmd_msg.cmd_hc_wr_link_key.
                              num_keys_to_write = num_keys_to_write;
    cmd_ptr->cmd_msg.cmd_hc_wr_link_key.dsm_ptr = dsm_ptr;
  
    cmd_status = bt_cmd( cmd_ptr );
  }

  return( cmd_status );

}


/*===========================================================================

FUNCTION
  bt_cmd_hc_change_local_name

DESCRIPTION
  This function sends an HCI change local name command to the
  Bluetooth driver.

===========================================================================*/
bt_cmd_status_type bt_cmd_hc_change_local_name
(
  dsm_item_type*  dsm_ptr  /*  dsm_ptr contains up to 248 bytes of UTF-8  */
                           /*  encoded characters; null terminated.       */
)
{

  bt_cmd_msg_type*    cmd_ptr;
  bt_cmd_status_type  cmd_status = BT_CS_GN_CMD_Q_FULL;

  if ( (cmd_ptr = bt_get_free_cmd()) != NULL )
  {
    cmd_ptr->cmd_hdr.cmd_type  = BT_CMD_HC_CHANGE_LOCAL_NAME;
    cmd_ptr->cmd_hdr.bt_app_id = bt_dc_last_hci_ev_reg_app_id;

    cmd_ptr->cmd_msg.cmd_hc_change_local_name.dsm_ptr = dsm_ptr;
  
    cmd_status = bt_cmd( cmd_ptr );
  }

  return( cmd_status );

}


/*===========================================================================

FUNCTION
  bt_cmd_hc_host_num_cmplt_pkt

DESCRIPTION
  This function sends an HCI host number of completed packets command
  to the Bluetooth driver.

===========================================================================*/
bt_cmd_status_type bt_cmd_hc_host_num_cmplt_pkt
(
  uint8           number_of_handles,
  dsm_item_type*  dsm_ptr
    /*  dsm_ptr the equivalent of the following data:        */
    /*    uint16  conn_hndl[ number_of_handles ];            */
    /*    uint16  host_num_cmplt_pkts[ number_of_handles ];  */
)
{

  bt_cmd_msg_type*    cmd_ptr;
  bt_cmd_status_type  cmd_status = BT_CS_GN_CMD_Q_FULL;

  if ( (cmd_ptr = bt_get_free_cmd()) != NULL )
  {
    cmd_ptr->cmd_hdr.cmd_type  = BT_CMD_HC_HOST_NUM_CMPLT_PACKETS;
    cmd_ptr->cmd_hdr.bt_app_id = bt_dc_last_hci_ev_reg_app_id;

    cmd_ptr->cmd_msg.cmd_hc_host_num_cmplt_pkt.
             number_of_handles = number_of_handles;
    cmd_ptr->cmd_msg.cmd_hc_host_num_cmplt_pkt.
                       dsm_ptr = dsm_ptr;
  
    cmd_status = bt_cmd( cmd_ptr );
  }

  return( cmd_status );

}


/*===========================================================================

FUNCTION
  bt_cmd_hc_wr_cur_iac_lap

DESCRIPTION
  This function sends an HCI write current IAC LAP command to the
  Bluetooth driver.

===========================================================================*/
bt_cmd_status_type bt_cmd_hc_wr_cur_iac_lap
(
  uint8           num_current_iac,
  dsm_item_type*  dsm_ptr
    /*  dsm_ptr the equivalent of the following data:  */
    /*    bt_lap_type  iac_lap[ num_current_iac ];     */
)
{

  bt_cmd_msg_type*    cmd_ptr;
  bt_cmd_status_type  cmd_status = BT_CS_GN_CMD_Q_FULL;

  if ( (cmd_ptr = bt_get_free_cmd()) != NULL )
  {
    cmd_ptr->cmd_hdr.cmd_type  = BT_CMD_HC_WR_CURRENT_IAC_LAP;
    cmd_ptr->cmd_hdr.bt_app_id = bt_dc_last_hci_ev_reg_app_id;

    cmd_ptr->cmd_msg.cmd_hc_wr_curr_iac_lap.
               num_current_iac = num_current_iac;
    cmd_ptr->cmd_msg.cmd_hc_wr_curr_iac_lap.
                       dsm_ptr = dsm_ptr;
  
    cmd_status = bt_cmd( cmd_ptr );
  }

  return( cmd_status );

}


/*===========================================================================

FUNCTION
  bt_cmd_hc_wr_page_to

DESCRIPTION
  This function sends an HCI_Wr_Page_Timeout command to the Bluetooth
  driver.

===========================================================================*/
bt_cmd_status_type bt_cmd_hc_wr_page_to
(
  uint16  page_timeout
)
{

  bt_cmd_msg_type*    cmd_ptr;
  bt_cmd_status_type  cmd_status = BT_CS_GN_CMD_Q_FULL;

  if ( (cmd_ptr = bt_get_free_cmd()) != NULL )
  {
    cmd_ptr->cmd_hdr.cmd_type  = BT_CMD_HC_WR_PAGE_TO;
    cmd_ptr->cmd_hdr.bt_app_id = bt_dc_last_hci_ev_reg_app_id;
    
    cmd_ptr->cmd_msg.cmd_hc_wr_page_to.page_timeout = page_timeout;
  
    cmd_status = bt_cmd( cmd_ptr );
  }

  return( cmd_status );

}


/*===========================================================================

FUNCTION
  bt_cmd_hc_wr_page_scan_act

DESCRIPTION
  This function sends an HCI_Write_Page_Scan_Activity command to the
  Bluetooth driver.

===========================================================================*/
bt_cmd_status_type bt_cmd_hc_wr_page_scan_act
(
  uint16  page_scan_interval,  /*  in .625 ms units  */
  uint16  page_scan_window     /*  in .625 ms units  */
)
{

  bt_cmd_msg_type*    cmd_ptr;
  bt_cmd_status_type  cmd_status = BT_CS_GN_CMD_Q_FULL;

  if ( (cmd_ptr = bt_get_free_cmd()) != NULL )
  {
    cmd_ptr->cmd_hdr.cmd_type  = BT_CMD_HC_WR_PAGE_SCAN_ACTIVITY;
    cmd_ptr->cmd_hdr.bt_app_id = bt_dc_last_hci_ev_reg_app_id;

    cmd_ptr->cmd_msg.cmd_hc_wr_page_scan_act.
            page_scan_interval = page_scan_interval;
    cmd_ptr->cmd_msg.cmd_hc_wr_page_scan_act.
              page_scan_window = page_scan_window;

    cmd_status = bt_cmd( cmd_ptr );
  }

  return( cmd_status );

}


/*===========================================================================

FUNCTION
  bt_cmd_hc_wr_inq_scan_act

DESCRIPTION
  This function sends an HCI_Write_Page_Scan_Activity command to the
  Bluetooth driver.

===========================================================================*/
bt_cmd_status_type bt_cmd_hc_wr_inq_scan_act
(
  uint16  inq_scan_interval,  /*  in .625 ms units  */
  uint16  inq_scan_window     /*  in .625 ms units  */
)
{

  bt_cmd_msg_type*    cmd_ptr;
  bt_cmd_status_type  cmd_status = BT_CS_GN_CMD_Q_FULL;

  if ( (cmd_ptr = bt_get_free_cmd()) != NULL )
  {
    cmd_ptr->cmd_hdr.cmd_type  = BT_CMD_HC_WR_INQ_SCAN_ACTIVITY;
    cmd_ptr->cmd_hdr.bt_app_id = bt_dc_last_hci_ev_reg_app_id;

    cmd_ptr->cmd_msg.cmd_hc_wr_inq_scan_act.
         inquiry_scan_interval = inq_scan_interval;
    cmd_ptr->cmd_msg.cmd_hc_wr_inq_scan_act.
         inquiry_scan_window = inq_scan_window;

    cmd_status = bt_cmd( cmd_ptr );
  }

  return( cmd_status );

}


/*===========================================================================

FUNCTION
  bt_cmd_hc_enable_dut_mode

DESCRIPTION
  This function sends an HCI_Enable_DUT_Mode command to the
  Bluetooth driver.

===========================================================================*/
bt_cmd_status_type bt_cmd_hc_enable_dut_mode
(
  void
)
{

  bt_cmd_msg_type*    cmd_ptr;
  bt_cmd_status_type  cmd_status = BT_CS_GN_CMD_Q_FULL;

  if ( (cmd_ptr = bt_get_free_cmd()) != NULL )
  {
    cmd_ptr->cmd_hdr.cmd_type  = BT_CMD_HC_ENABLE_DUT_MODE;
    cmd_ptr->cmd_hdr.bt_app_id = bt_dc_last_hci_ev_reg_app_id;
  
    cmd_status = bt_cmd( cmd_ptr );
  }

  return( cmd_status );

}


/*===========================================================================

FUNCTION
  bt_cmd_hc_send_raw_mode_pkt

DESCRIPTION
  This function sends an HCI send raw mode packet command to the
  Bluetooth driver.

===========================================================================*/
bt_cmd_status_type bt_cmd_hc_send_raw_mode_pkt
(
  uint16          conn_hndl,
  uint8           start,          /*  Start of data to send  */
  uint8           end,            /*  Last of data to send   */
  uint8           num_slots,
  uint16          data_byte_len,  /*  Not to exceed 360--    */
                                  /*    size of a HV5        */
  dsm_item_type*  dsm_ptr         /*  The data to transmit   */
)
{

  bt_cmd_msg_type*    cmd_ptr;
  bt_cmd_status_type  cmd_status = BT_CS_GN_CMD_Q_FULL;

  if ( (cmd_ptr = bt_get_free_cmd()) != NULL )
  {
    cmd_ptr->cmd_hdr.cmd_type  = BT_CMD_HC_SEND_RAW_MODE_PKT;
    cmd_ptr->cmd_hdr.bt_app_id = bt_dc_last_hci_ev_reg_app_id;

    cmd_ptr->cmd_msg.cmd_hc_send_raw_mode_pkt.conn_hndl = conn_hndl;
    cmd_ptr->cmd_msg.cmd_hc_send_raw_mode_pkt.start     = start;
    cmd_ptr->cmd_msg.cmd_hc_send_raw_mode_pkt.end       = end;
    cmd_ptr->cmd_msg.cmd_hc_send_raw_mode_pkt.num_slots = num_slots;
    cmd_ptr->cmd_msg.cmd_hc_send_raw_mode_pkt.data_byte_len =
                                                      data_byte_len;
    cmd_ptr->cmd_msg.cmd_hc_send_raw_mode_pkt.dsm_ptr   = dsm_ptr;
  
    cmd_status = bt_cmd( cmd_ptr );
  }

  return( cmd_status );

}


#ifdef FEATURE_BT_TEST_MODE_TESTER
/*===========================================================================

FUNCTION
  bt_cmd_hc_tester_control

DESCRIPTION
  This function sends an HCI tester control command to the Bluetooth
  driver.

===========================================================================*/
bt_cmd_status_type bt_cmd_hc_tester_control
(
  uint16          conn_hndl,
  uint8           test_senario,
  uint8           hopping_mode,
  uint8           dut_tx_freq_k,
  uint8           dut_rx_freq_k,
  uint8           power_control_mode,
  uint8           poll_period,  /*  In units of 1.25 ms  */
  uint8           packet_type,
  uint16          payload_len,
  uint8           start_end,    /*  start = 0; end = 1  */
  dsm_item_type*  dsm_ptr
)
{

  bt_cmd_msg_type*    cmd_ptr;
  bt_cmd_status_type  cmd_status = BT_CS_GN_CMD_Q_FULL;

  if ( (cmd_ptr = bt_get_free_cmd()) != NULL )
  {
    cmd_ptr->cmd_hdr.cmd_type  = BT_CMD_HC_TESTER_CONTROL;
    cmd_ptr->cmd_hdr.bt_app_id = bt_dc_last_hci_ev_reg_app_id;

    cmd_ptr->cmd_msg.cmd_hc_tester_ctrl.conn_hndl     = conn_hndl;
    cmd_ptr->cmd_msg.cmd_hc_tester_ctrl.test_senario  = test_senario;
    cmd_ptr->cmd_msg.cmd_hc_tester_ctrl.hopping_mode  = hopping_mode;
    cmd_ptr->cmd_msg.cmd_hc_tester_ctrl.dut_tx_freq_k = dut_tx_freq_k;
    cmd_ptr->cmd_msg.cmd_hc_tester_ctrl.dut_rx_freq_k = dut_rx_freq_k;
    cmd_ptr->cmd_msg.cmd_hc_tester_ctrl.power_control_mode =
                                                   power_control_mode;
    cmd_ptr->cmd_msg.cmd_hc_tester_ctrl.poll_period   = poll_period;
    cmd_ptr->cmd_msg.cmd_hc_tester_ctrl.packet_type   = packet_type;
    cmd_ptr->cmd_msg.cmd_hc_tester_ctrl.payload_len   = payload_len;
    cmd_ptr->cmd_msg.cmd_hc_tester_ctrl.start_end     = start_end;
    cmd_ptr->cmd_msg.cmd_hc_tester_ctrl.dsm_ptr       = dsm_ptr;
  
    cmd_status = bt_cmd( cmd_ptr );
  }

  return( cmd_status );

}
#endif /* FEATURE_BT_TEST_MODE_TESTER */

/*===========================================================================

FUNCTION
  bt_cmd_hc_vs_any

DESCRIPTION
  This function sends a HCI Vendor specific command to the Bluetooth driver.

===========================================================================*/
bt_cmd_status_type bt_cmd_hc_vs_any
(
  dsm_item_type*  dsm_ptr,
  uint16          opcode
)
{

  bt_cmd_msg_type*    cmd_ptr;
  bt_cmd_status_type  cmd_status = BT_CS_GN_CMD_Q_FULL;

  if ( (cmd_ptr = bt_get_free_cmd()) != NULL )
  {
    cmd_ptr->cmd_hdr.cmd_type  = BT_CMD_HC_VS_ANY;
    cmd_ptr->cmd_hdr.bt_app_id = bt_dc_last_hci_ev_reg_app_id;

    cmd_ptr->cmd_msg.cmd_hc_vs_any.dsm_ptr    = dsm_ptr;
    cmd_ptr->cmd_msg.cmd_hc_vs_any.opcode     = opcode;
  
    cmd_status = bt_cmd( cmd_ptr );
  }
  
  return( cmd_status );

}

/*===========================================================================

FUNCTION
  bt_cmd_hc_send_raw_hc_cmd

DESCRIPTION
  This function sends an HCI command specified via a byte buffer to
  the Bluetooth driver.

===========================================================================*/
bt_cmd_status_type bt_cmd_hc_send_raw_hc_cmd
(
  bt_cmd_type  cmd_type,
  uint8*       hc_cmd_ptr,
  uint16       byte_count
)
{

  bt_cmd_msg_type*    cmd_ptr;
  bt_cmd_status_type  cmd_status = BT_CS_GN_CMD_Q_FULL;

  if ( (cmd_ptr = bt_get_free_cmd()) != NULL )
  {
    cmd_ptr->cmd_hdr.cmd_type  = cmd_type;
    cmd_ptr->cmd_hdr.bt_app_id = bt_dc_last_hci_ev_reg_app_id;
  
    memcpy( (uint8*) &cmd_ptr->cmd_msg, hc_cmd_ptr, byte_count );
  
    cmd_status = bt_cmd( cmd_ptr );
  }
  
  return( cmd_status );

}

/*===========================================================================

FUNCTION
  bt_cmd_hc_wr_link_policy

DESCRIPTION
  This function sends an HCI command to set the link policy for
  enabling link sniff, hold, park modes and master-slave switch.

===========================================================================*/
bt_cmd_status_type bt_cmd_hc_wr_link_policy
(
  uint16  conn_hndl,
  uint16  link_policy_settings
)
{

  bt_cmd_msg_type*    cmd_ptr;
  bt_cmd_status_type  cmd_status = BT_CS_GN_CMD_Q_FULL;

  if ( (cmd_ptr = bt_get_free_cmd()) != NULL )
  {
    cmd_ptr->cmd_hdr.cmd_type  = BT_CMD_HC_WR_LINK_POLICY;
    cmd_ptr->cmd_hdr.bt_app_id = bt_dc_last_hci_ev_reg_app_id;

    cmd_ptr->cmd_msg.cmd_hc_wr_link_policy.
                     conn_hndl = conn_hndl;
    cmd_ptr->cmd_msg.cmd_hc_wr_link_policy.
          link_policy_settings = link_policy_settings;
  
    cmd_status = bt_cmd( cmd_ptr );
  }
  
  return( cmd_status );

}


/*===========================================================================

FUNCTION
  bt_cmd_hc_set_event_filter_clear_all

DESCRIPTION
  This function sends an HCI Set Event Filter command to the Bluetooth 
  driver to clear all filters.

===========================================================================*/
bt_cmd_status_type bt_cmd_hc_set_event_filter_clear_all
(
  void
)
{

  bt_cmd_msg_type*    cmd_ptr;
  bt_cmd_status_type  cmd_status = BT_CS_GN_CMD_Q_FULL;

  if ( (cmd_ptr = bt_get_free_cmd()) != NULL )
  {
    cmd_ptr->cmd_hdr.cmd_type  = BT_CMD_HC_SET_EVENT_FILTER;
    cmd_ptr->cmd_hdr.bt_app_id = bt_dc_last_hci_ev_reg_app_id;
    
    cmd_ptr->cmd_msg.cmd_hc_set_event_filter.filter_type =
                                               BT_FILTER_CLEAR_ALL;

    cmd_status = bt_cmd( cmd_ptr );
  }
  
  return( cmd_status );

}


/*===========================================================================

FUNCTION
  bt_cmd_hc_set_event_filter_conn_all_dev

DESCRIPTION
  This function sends an HCI Set Event (Connection All Device) Filter 
  command to the Bluetooth driver.

===========================================================================*/
bt_cmd_status_type bt_cmd_hc_set_event_filter_conn_all_dev
(
  bt_auto_accept_type  auto_accept_flag
)
{

  bt_cmd_msg_type*    cmd_ptr;
  bt_cmd_status_type  cmd_status = BT_CS_GN_CMD_Q_FULL;

  if ( (cmd_ptr = bt_get_free_cmd()) != NULL )
  {
    cmd_ptr->cmd_hdr.cmd_type  = BT_CMD_HC_SET_EVENT_FILTER;
    cmd_ptr->cmd_hdr.bt_app_id = bt_dc_last_hci_ev_reg_app_id;
    
    cmd_ptr->cmd_msg.cmd_hc_set_event_filter.
      filter_type                       = BT_FILTER_CONN_SETUP;
    cmd_ptr->cmd_msg.cmd_hc_set_event_filter.
      filter_cond_type.conn_filter_cond = BT_CONN_FILTER_ALL_DEVICES;
    cmd_ptr->cmd_msg.cmd_hc_set_event_filter.
      condition.auto_accept_flag        = auto_accept_flag;

    cmd_status = bt_cmd( cmd_ptr );
  }

  return( cmd_status );

}


/*===========================================================================

FUNCTION
  bt_cmd_hc_wr_voice_setting

DESCRIPTION
  This function sends a HCI write voice settings command to the
  Bluetooth driver.

===========================================================================*/
bt_cmd_status_type bt_cmd_hc_wr_voice_setting
(
  uint16  voice_encoding  /*  use BT_HC_VS_**_M #defines  */
)
{

  bt_cmd_msg_type*    cmd_ptr;
  bt_cmd_status_type  cmd_status = BT_CS_GN_CMD_Q_FULL;

  if ( (cmd_ptr = bt_get_free_cmd()) != NULL )
  {
    cmd_ptr->cmd_hdr.cmd_type  = BT_CMD_HC_WR_VOICE_SETTING;
    cmd_ptr->cmd_hdr.bt_app_id = bt_dc_last_hci_ev_reg_app_id;

    cmd_ptr->cmd_msg.cmd_hc_wr_voice_setting.
                 voice_setting = voice_encoding;
  
    cmd_status = bt_cmd( cmd_ptr );
  }
  
  return( cmd_status );

}


/*===========================================================================

FUNCTION
  bt_cmd_hc_set_incoming_hold_params

DESCRIPTION
  This function sends an HCI_Set_Incoming_Hold_Params command to the
  Bluetooth driver.

===========================================================================*/
bt_cmd_status_type bt_cmd_hc_set_incoming_hold_params
(
  uint16  conn_hndl,     /*  the connection handle  */
  uint16  min_interval,  /*  in .625 ms units       */
  uint16  max_interval   /*  in .625 ms units       */
)
{

  bt_cmd_msg_type*    cmd_ptr;
  bt_cmd_status_type  cmd_status = BT_CS_GN_CMD_Q_FULL;

  if ( (cmd_ptr = bt_get_free_cmd()) != NULL )
  {
    cmd_ptr->cmd_hdr.cmd_type  = BT_CMD_HC_SET_INCOMING_HOLD_PARAMS;
    cmd_ptr->cmd_hdr.bt_app_id = bt_dc_last_hci_ev_reg_app_id;

    cmd_ptr->cmd_msg.cmd_hc_set_inc_hold_params.
                     conn_hndl = conn_hndl;
    cmd_ptr->cmd_msg.cmd_hc_set_inc_hold_params.
                  min_interval = min_interval;
    cmd_ptr->cmd_msg.cmd_hc_set_inc_hold_params.
                  max_interval = max_interval;
  
    cmd_status = bt_cmd( cmd_ptr );
  }

  return( cmd_status );

}


/*===========================================================================

FUNCTION
  bt_cmd_hc_set_incoming_sniff_params

DESCRIPTION
  This function sends an HCI_Set_Incoming_Sniff_Params command to the
  Bluetooth driver.

===========================================================================*/
bt_cmd_status_type bt_cmd_hc_set_incoming_sniff_params
(
  uint16  conn_hndl,     /*  the connection handle  */
  uint16  min_interval,  /*  in .625 ms units       */
  uint16  max_interval   /*  in .625 ms units       */
)
{

  bt_cmd_msg_type*    cmd_ptr;
  bt_cmd_status_type  cmd_status = BT_CS_GN_CMD_Q_FULL;

  if ( (cmd_ptr = bt_get_free_cmd()) != NULL )
  {
    cmd_ptr->cmd_hdr.cmd_type  = BT_CMD_HC_SET_INCOMING_SNIFF_PARAMS;
    cmd_ptr->cmd_hdr.bt_app_id = bt_dc_last_hci_ev_reg_app_id;

    cmd_ptr->cmd_msg.cmd_hc_set_inc_sniff_params.
                     conn_hndl = conn_hndl;
    cmd_ptr->cmd_msg.cmd_hc_set_inc_sniff_params.
                  min_interval = min_interval;
    cmd_ptr->cmd_msg.cmd_hc_set_inc_sniff_params.
                  max_interval = max_interval;
  
    cmd_status = bt_cmd( cmd_ptr );
  }
  
  return( cmd_status );

}


/*===========================================================================

FUNCTION
  bt_cmd_hc_set_incoming_park_params

DESCRIPTION
  This function sends an HCI_Set_Incoming_Park_Params command to the
  Bluetooth driver.

===========================================================================*/
bt_cmd_status_type bt_cmd_hc_set_incoming_park_params
(
  uint16  conn_hndl,     /*  the connection handle  */
  uint16  min_interval,  /*  in .625 ms units       */
  uint16  max_interval   /*  in .625 ms units       */
)
{

  bt_cmd_msg_type*    cmd_ptr;
  bt_cmd_status_type  cmd_status = BT_CS_GN_CMD_Q_FULL;

  if ( (cmd_ptr = bt_get_free_cmd()) != NULL )
  {
    cmd_ptr->cmd_hdr.cmd_type  = BT_CMD_HC_SET_INCOMING_PARK_PARAMS;
    cmd_ptr->cmd_hdr.bt_app_id = bt_dc_last_hci_ev_reg_app_id;

    cmd_ptr->cmd_msg.cmd_hc_set_inc_park_params.
                     conn_hndl = conn_hndl;
    cmd_ptr->cmd_msg.cmd_hc_set_inc_park_params.
                  min_interval = min_interval;
    cmd_ptr->cmd_msg.cmd_hc_set_inc_park_params.
                  max_interval = max_interval;
  
    cmd_status = bt_cmd( cmd_ptr );
  }
  
  return( cmd_status );

}


/*===========================================================================

FUNCTION
  bt_cmd_hc_wr_auto_flush_timeout

DESCRIPTION
  This function sends a write automatic flush timeout command to the
  Bluetooth driver.

===========================================================================*/
bt_cmd_status_type bt_cmd_hc_wr_auto_flush_timeout
(
  uint16  conn_hndl,     /*  the connection handle  */
  uint16  flush_timeout  /*  in .625 ms units       */
)
{

  bt_cmd_msg_type*    cmd_ptr;
  bt_cmd_status_type  cmd_status = BT_CS_GN_CMD_Q_FULL;

  if ( (cmd_ptr = bt_get_free_cmd()) != NULL )
  {
    cmd_ptr->cmd_hdr.cmd_type  = BT_CMD_HC_WR_AUTO_FLUSH_TO;
    cmd_ptr->cmd_hdr.bt_app_id = bt_dc_last_hci_ev_reg_app_id;

    cmd_ptr->cmd_msg.cmd_hc_wr_auto_flush_to.
                     conn_hndl = conn_hndl;
    cmd_ptr->cmd_msg.cmd_hc_wr_auto_flush_to.
                 flush_timeout = flush_timeout;
  
    cmd_status = bt_cmd( cmd_ptr );
  }
  
  return( cmd_status );

}


/*===========================================================================

FUNCTION
  bt_cmd_hc_change_pkt_type

DESCRIPTION
  This function sends a change packet type command to the
  Bluetooth driver.

===========================================================================*/
bt_cmd_status_type bt_cmd_hc_change_pkt_type
(
  uint16  conn_hndl,  /*  the connection handle  */
  uint16  pkt_type    /*  defined in bti.h       */
)
{

  bt_cmd_msg_type*    cmd_ptr;
  bt_cmd_status_type  cmd_status = BT_CS_GN_CMD_Q_FULL;

  if ( (cmd_ptr = bt_get_free_cmd()) != NULL )
  {
    cmd_ptr->cmd_hdr.cmd_type  = BT_CMD_HC_CHANGE_PKT_TYPE;
    cmd_ptr->cmd_hdr.bt_app_id = bt_dc_last_hci_ev_reg_app_id;

    cmd_ptr->cmd_msg.cmd_hc_change_pkt.conn_hndl = conn_hndl;
    cmd_ptr->cmd_msg.cmd_hc_change_pkt.pkt_type  = pkt_type;
  
  
    cmd_status = bt_cmd( cmd_ptr );
  }
  
  return( cmd_status );

}


/*===========================================================================

FUNCTION
  bt_cmd_hc_set_conn_encrypt

DESCRIPTION
  This function sends a set connection encryption type command to the
  Bluetooth driver.

===========================================================================*/
bt_cmd_status_type bt_cmd_hc_set_conn_encrypt
(
  uint16                  conn_hndl,   /*  the connection handle  */
  bt_encrypt_enable_type  encrypt_en   /*  defined in btevi.h     */
)
{

  bt_cmd_msg_type*    cmd_ptr;
  bt_cmd_status_type  cmd_status = BT_CS_GN_CMD_Q_FULL;

  if ( (cmd_ptr = bt_get_free_cmd()) != NULL )
  {
    cmd_ptr->cmd_hdr.cmd_type  = BT_CMD_HC_SET_CONN_ENCRYPT;
    cmd_ptr->cmd_hdr.bt_app_id = bt_dc_last_hci_ev_reg_app_id;

    cmd_ptr->cmd_msg.cmd_hc_set_conn_encrypt.conn_hndl  = conn_hndl;
    cmd_ptr->cmd_msg.cmd_hc_set_conn_encrypt.encrypt_en = encrypt_en;
  
    cmd_status = bt_cmd( cmd_ptr );
  }
  
  return( cmd_status );

}


/*===========================================================================

FUNCTION
  bt_cmd_hc_change_link_key

DESCRIPTION
  This function sends an HC change link key command to the Bluetooth
  driver.

===========================================================================*/
bt_cmd_status_type bt_cmd_hc_change_link_key
(
  uint16  conn_hndl
)
{

  bt_cmd_msg_type*    cmd_ptr;
  bt_cmd_status_type  cmd_status = BT_CS_GN_CMD_Q_FULL;

  if ( (cmd_ptr = bt_get_free_cmd()) != NULL )
  {
    cmd_ptr->cmd_hdr.cmd_type  = BT_CMD_HC_CHANGE_LINK_KEY;
    cmd_ptr->cmd_hdr.bt_app_id = bt_dc_last_hci_ev_reg_app_id;

    cmd_ptr->cmd_msg.cmd_hc_conn_hndl.conn_hndl = conn_hndl;
  
    cmd_status = bt_cmd( cmd_ptr );
  }
  
  return( cmd_status );

}


/*===========================================================================

FUNCTION
  bt_cmd_hc_wr_enc_key_len

DESCRIPTION
  This function sends an HC write encryption key length command to the
  Bluetooth driver.

===========================================================================*/
bt_cmd_status_type bt_cmd_hc_wr_enc_key_len
(
  uint8  min_enc_key_size,
  uint8  max_enc_key_size
)
{

  bt_cmd_msg_type*    cmd_ptr;
  bt_cmd_status_type  cmd_status = BT_CS_GN_CMD_Q_FULL;

  if ( (cmd_ptr = bt_get_free_cmd()) != NULL )
  {
    cmd_ptr->cmd_hdr.cmd_type  = BT_CMD_HC_WR_ENC_KEY_LENGTH;
    cmd_ptr->cmd_hdr.bt_app_id = bt_dc_last_hci_ev_reg_app_id;

    cmd_ptr->cmd_msg.cmd_hc_wr_enc_key_length.min_enc_key_size =
      min_enc_key_size;
    cmd_ptr->cmd_msg.cmd_hc_wr_enc_key_length.max_enc_key_size =
      max_enc_key_size;
  
    cmd_status = bt_cmd( cmd_ptr );
  }
  
  return( cmd_status );

}


/*===========================================================================

FUNCTION
  bt_cmd_hc_tx_continuous

DESCRIPTION
  This function sends a tx continous command to the Bluetooth driver.

===========================================================================*/
bt_cmd_status_type bt_cmd_hc_tx_continuous
(
  boolean  hopping
)
{

  bt_cmd_msg_type*    cmd_ptr;
  bt_cmd_status_type  cmd_status = BT_CS_GN_CMD_Q_FULL;

  if ( (cmd_ptr = bt_get_free_cmd()) != NULL )
  {
    cmd_ptr->cmd_hdr.cmd_type  = BT_CMD_HC_TX_CONTINUOUS;
    cmd_ptr->cmd_hdr.bt_app_id = bt_dc_last_hci_ev_reg_app_id;
    
    cmd_ptr->cmd_msg.cmd_hc_tx_continuous.hopping = (uint8)(hopping);
  
    cmd_status = bt_cmd( cmd_ptr );
  }
  
  return( cmd_status );

}

/*===========================================================================

FUNCTION
  bt_cmd_hc_tx_continuous_freq

DESCRIPTION
  This function sends a tx continous command to the Bluetooth driver
  specifying a fix transmit frequency without hopping.

===========================================================================*/
bt_cmd_status_type bt_cmd_hc_tx_continuous_freq
(
  uint8  channel_number
)
{

  bt_cmd_msg_type*    cmd_ptr;
  bt_cmd_status_type  cmd_status = BT_CS_GN_CMD_Q_FULL;

  if ( (cmd_ptr = bt_get_free_cmd()) != NULL )
  {
    cmd_ptr->cmd_hdr.cmd_type  = BT_CMD_HC_TX_CONTINUOUS_FREQ;
    cmd_ptr->cmd_hdr.bt_app_id = bt_dc_last_hci_ev_reg_app_id;
    
    cmd_ptr->cmd_msg.cmd_hc_tx_continuous_freq.channel_number = channel_number;
  
    cmd_status = bt_cmd( cmd_ptr );
  }
  
  return( cmd_status );

}

/*===========================================================================

FUNCTION
  bt_cmd_hc_switch_role

DESCRIPTION
  This function sends a switch role command to the Bluetooth driver.

===========================================================================*/
bt_cmd_status_type bt_cmd_hc_switch_role
(
  bt_bd_addr_type*     bd_addr_ptr,
  bt_role_type         role_type
)
{

  bt_cmd_msg_type*    cmd_ptr;
  bt_cmd_status_type  cmd_status = BT_CS_GN_CMD_Q_FULL;

  if ( (cmd_ptr = bt_get_free_cmd()) != NULL )
  {
    cmd_ptr->cmd_hdr.cmd_type  = BT_CMD_HC_SWITCH_ROLE;
    cmd_ptr->cmd_hdr.bt_app_id = bt_dc_last_hci_ev_reg_app_id;
    
    cmd_ptr->cmd_msg.cmd_hc_switch_role.bd_addr = *bd_addr_ptr;
    cmd_ptr->cmd_msg.cmd_hc_switch_role.role    = role_type;
  
    cmd_status = bt_cmd( cmd_ptr );
  }
  
  return( cmd_status );

}

/*==========================================================================

FUNCTION
  bt_cmd_hc_rd_local_feat

DESCRIPTION
  This function Sends the HCI Get Local Features command to Bluetooth driver.

==========================================================================*/
bt_cmd_status_type bt_cmd_hc_rd_local_feat( void )
{
  bt_cmd_msg_type*   bt_cmd_ptr;
  bt_cmd_status_type  cmd_status = BT_CS_GN_CMD_Q_FULL;

  bt_cmd_ptr = bt_get_free_cmd();

  if ( bt_cmd_ptr != NULL )
  {
    bt_initialize_cmd_buffer( bt_cmd_ptr );

    bt_cmd_ptr->cmd_hdr.cmd_type    = BT_CMD_HC_RD_LOCAL_FEATURES;
    bt_cmd_ptr->cmd_hdr.bt_app_id   = bt_dc_last_hci_ev_reg_app_id;

    cmd_status = bt_cmd( bt_cmd_ptr );
  }

  return( cmd_status );

} /* bt_cmd_hc_rd_local_feat */


/*==========================================================================

FUNCTION
  bt_cmd_hc_rd_local_ver

DESCRIPTION
  This function Sends the HCI Get Local Version command to Bluetooth driver.

==========================================================================*/
bt_cmd_status_type bt_cmd_hc_rd_local_ver( void )
{
  bt_cmd_msg_type*   bt_cmd_ptr;
  bt_cmd_status_type  cmd_status = BT_CS_GN_CMD_Q_FULL;

  bt_cmd_ptr = bt_get_free_cmd();

  if ( bt_cmd_ptr  != NULL )
  {
    bt_initialize_cmd_buffer( bt_cmd_ptr );

    bt_cmd_ptr->cmd_hdr.cmd_type    = BT_CMD_HC_RD_LOCAL_VERSION_INFO;
    bt_cmd_ptr->cmd_hdr.bt_app_id   = bt_dc_last_hci_ev_reg_app_id;

    cmd_status = bt_cmd( bt_cmd_ptr );
  }

  return( cmd_status );

} /* bt_cmd_hc_rd_local_ver */

/*===========================================================================

FUNCTION
  bt_cmd_hc_rd_rssi

DESCRIPTION
  This function sends an HC Read RSSI cmd to the Bluetooth driver.

===========================================================================*/
bt_cmd_status_type bt_cmd_hc_rd_rssi
(
  uint16  conn_hndl
)
{

  bt_cmd_msg_type*    cmd_ptr;
  bt_cmd_status_type  cmd_status = BT_CS_GN_CMD_Q_FULL;

  if ( (cmd_ptr = bt_get_free_cmd()) != NULL )
  {
    cmd_ptr->cmd_hdr.cmd_type  = BT_CMD_HC_RD_RSSI;
    cmd_ptr->cmd_hdr.bt_app_id = bt_dc_last_hci_ev_reg_app_id;

    cmd_ptr->cmd_msg.cmd_hc_conn_hndl.conn_hndl = conn_hndl;
  
    cmd_status = bt_cmd( cmd_ptr );
  }
  
  return( cmd_status );

} /* bt_cmd_hc_rd_rssi */

#ifdef FEATURE_BT_1_2

/*===========================================================================

FUNCTION
  bt_cmd_hc_setup_sync_conn

DESCRIPTION
  This function sends an HCI setup synchronous connection command to the 
  Bluetooth driver.

===========================================================================*/
bt_cmd_status_type bt_cmd_hc_setup_sync_conn
(
  uint16           conn_hndl,
  uint32           tx_bandwidth,
  uint32           rx_bandwidth,
  uint16           max_latency,
  uint16           voice_setting,
  uint8            retransmission_effort,
  uint16           pkt_type
)
{

  bt_cmd_msg_type*    cmd_ptr;
  bt_cmd_status_type  cmd_status = BT_CS_GN_CMD_Q_FULL;

  if ( (cmd_ptr = bt_get_free_cmd()) != NULL )
  {
    cmd_ptr->cmd_hdr.cmd_type  = BT_CMD_HC_SETUP_SYNC_CONNECTION;
    cmd_ptr->cmd_hdr.bt_app_id = bt_dc_last_hci_ev_reg_app_id;

    cmd_ptr->cmd_msg.cmd_hc_setup_sync_connection.conn_hndl     = conn_hndl;
    cmd_ptr->cmd_msg.cmd_hc_setup_sync_connection.tx_bandwidth  = tx_bandwidth;
    cmd_ptr->cmd_msg.cmd_hc_setup_sync_connection.rx_bandwidth  = rx_bandwidth;
    cmd_ptr->cmd_msg.cmd_hc_setup_sync_connection.max_latency   = max_latency;
    cmd_ptr->cmd_msg.cmd_hc_setup_sync_connection.voice_setting = voice_setting;      
    cmd_ptr->cmd_msg.cmd_hc_setup_sync_connection.retransmission_effort = 
      retransmission_effort;
    cmd_ptr->cmd_msg.cmd_hc_setup_sync_connection.pkt_type     = pkt_type;
  
    cmd_status = bt_cmd( cmd_ptr );
  }

  return( cmd_status );

}


/*===========================================================================

FUNCTION
  bt_cmd_hc_accept_sync_conn_req

DESCRIPTION
  This function sends an HCI accept synchronous connection command to the 
  Bluetooth driver.

===========================================================================*/
bt_cmd_status_type bt_cmd_hc_accept_sync_conn_req
(
  bt_bd_addr_type*  bd_addr_ptr,
  uint32            tx_bandwidth,
  uint32            rx_bandwidth,
  uint16            max_latency,
  uint16            content_format,
  uint8             retransmission_effort,
  uint16            pkt_type
)
{

  bt_cmd_msg_type*    cmd_ptr;
  bt_cmd_status_type  cmd_status = BT_CS_GN_CMD_Q_FULL;

  if ( (cmd_ptr = bt_get_free_cmd()) != NULL )
  {
    cmd_ptr->cmd_hdr.cmd_type  = BT_CMD_HC_ACCEPT_SYNC_CONN_REQ;
    cmd_ptr->cmd_hdr.bt_app_id = bt_dc_last_hci_ev_reg_app_id;

    cmd_ptr->cmd_msg.cmd_hc_accept_sync_conn_req.bd_addr      = *bd_addr_ptr;
    cmd_ptr->cmd_msg.cmd_hc_accept_sync_conn_req.tx_bandwidth = tx_bandwidth;
    cmd_ptr->cmd_msg.cmd_hc_accept_sync_conn_req.rx_bandwidth = rx_bandwidth;
    cmd_ptr->cmd_msg.cmd_hc_accept_sync_conn_req.max_latency  = max_latency;
    cmd_ptr->cmd_msg.cmd_hc_accept_sync_conn_req.content_format = 
      content_format;      
    cmd_ptr->cmd_msg.cmd_hc_accept_sync_conn_req.retransmission_effort = 
      retransmission_effort;
    cmd_ptr->cmd_msg.cmd_hc_accept_sync_conn_req.pkt_type     = pkt_type;
  
    cmd_status = bt_cmd( cmd_ptr );
  }

  return( cmd_status );

}


/*===========================================================================

FUNCTION
  bt_cmd_hc_reject_sync_conn_req

DESCRIPTION
  This function sends an HCI reject synchronous connection command to the 
  Bluetooth driver.

===========================================================================*/
bt_cmd_status_type bt_cmd_hc_reject_sync_conn_req
(
  bt_bd_addr_type*  bd_addr_ptr,
  uint8             reason
)
{

  bt_cmd_msg_type*    cmd_ptr;
  bt_cmd_status_type  cmd_status = BT_CS_GN_CMD_Q_FULL;

  if ( (cmd_ptr = bt_get_free_cmd()) != NULL )
  {
    cmd_ptr->cmd_hdr.cmd_type  = BT_CMD_HC_REJECT_SYNC_CONN_REQ;
    cmd_ptr->cmd_hdr.bt_app_id = bt_dc_last_hci_ev_reg_app_id;
    
    cmd_ptr->cmd_msg.cmd_hc_reject_sync_conn_req.bd_addr = *bd_addr_ptr;
    cmd_ptr->cmd_msg.cmd_hc_reject_sync_conn_req.reason  = reason;
  
    cmd_status = bt_cmd( cmd_ptr );
  }

  return( cmd_status );
}


/*===========================================================================

FUNCTION
  bt_cmd_hc_afh_host_chan_class

DESCRIPTION
  This function sends an HCI set AFH host channel classification command to the 
  Bluetooth driver.

===========================================================================*/
bt_cmd_status_type bt_cmd_hc_afh_host_chan_class
(
  bt_afh_ch_map_type ch_class
)
{
  bt_cmd_msg_type*    cmd_ptr;
  bt_cmd_status_type  cmd_status = BT_CS_GN_CMD_Q_FULL;
  uint16              i;

  if ( (cmd_ptr = bt_get_free_cmd()) != NULL )
  {
    cmd_ptr->cmd_hdr.cmd_type  = BT_CMD_HC_SET_AFH_HOST_CHAN_CLASS;
    cmd_ptr->cmd_hdr.bt_app_id = bt_dc_last_hci_ev_reg_app_id;
    for ( i = 0; i < 10; i++ )
    {                        
      cmd_ptr->cmd_msg.cmd_hc_set_afh_host_chan_class.afh_chan_class[ i ] = 
        ch_class.bytes[ i ];
    }
  
    cmd_status = bt_cmd( cmd_ptr );
  }

  return( cmd_status );
}


/*===========================================================================

FUNCTION
  bt_cmd_hc_read_afh_chan_assess_mode

DESCRIPTION
  This function sends an HCI read AFH channel assessment mode command to the 
  Bluetooth driver.

===========================================================================*/
bt_cmd_status_type bt_cmd_hc_read_afh_chan_assess_mode
(
  void
)
{
  bt_cmd_msg_type*    cmd_ptr;
  bt_cmd_status_type  cmd_status = BT_CS_GN_CMD_Q_FULL;

  if ( (cmd_ptr = bt_get_free_cmd()) != NULL )
  {
    cmd_ptr->cmd_hdr.cmd_type  = BT_CMD_HC_READ_AFH_CHAN_ASSESS_MODE;
    cmd_ptr->cmd_hdr.bt_app_id = bt_dc_last_hci_ev_reg_app_id;
  
    cmd_status = bt_cmd( cmd_ptr );
  }

  return( cmd_status );
}


/*===========================================================================

FUNCTION
  bt_cmd_hc_set_afh_chan_assess_mode

DESCRIPTION
  This function sends an HCI set AFH channel assessment mode command to the 
  Bluetooth driver.

===========================================================================*/
bt_cmd_status_type bt_cmd_hc_set_afh_chan_assess_mode
(
  uint8 afh_chan_asses_mode
)
{
  bt_cmd_msg_type*    cmd_ptr;
  bt_cmd_status_type  cmd_status = BT_CS_GN_CMD_Q_FULL;

  if ( (cmd_ptr = bt_get_free_cmd()) != NULL )
  {
    cmd_ptr->cmd_hdr.cmd_type  = BT_CMD_HC_SET_AFH_CHAN_ASSESS_MODE;
    cmd_ptr->cmd_hdr.bt_app_id = bt_dc_last_hci_ev_reg_app_id;
    cmd_ptr->cmd_msg.cmd_hc_set_afh_chan_assess_mode.afh_chan_asses_mode =
      afh_chan_asses_mode;

    cmd_status = bt_cmd( cmd_ptr );
  }

  return( cmd_status );
}


/*===========================================================================

FUNCTION
  bt_cmd_hc_read_afh_chan_map

DESCRIPTION
  This function sends an HCI read AFH channel map command to the 
  Bluetooth driver.

===========================================================================*/
bt_cmd_status_type bt_cmd_hc_read_afh_chan_map
(
  uint16 conn_hndl
)
{
  bt_cmd_msg_type*    cmd_ptr;
  bt_cmd_status_type  cmd_status = BT_CS_GN_CMD_Q_FULL;

  if ( (cmd_ptr = bt_get_free_cmd()) != NULL )
  {
   cmd_ptr->cmd_hdr.cmd_type  = BT_CMD_HC_READ_AFH_CHAN_MAP;
    cmd_ptr->cmd_hdr.bt_app_id = bt_dc_last_hci_ev_reg_app_id;
    cmd_ptr->cmd_msg.cmd_hc_read_afh_chan_map.conn_hndl = conn_hndl;

    cmd_status = bt_cmd( cmd_ptr );
  }

  return( cmd_status );
}


/*==========================================================================

FUNCTION     
    bt_cmd_hc_rd_buf_size

DESCRIPTION  
  Sends the HCI Read Buffer Size command to SOC. Note that this command must 
  be issued to Controller before the first ACL data packet is sent to 
  Controller. 

SIDE EFFECTS 
  For BTS4020 Controller, this initializes the HCI buffers for ACL.

==========================================================================*/
bt_cmd_status_type bt_cmd_hc_rd_buf_size( void )
{
  bt_cmd_msg_type*   bt_cmd_ptr;
  bt_cmd_status_type  cmd_status = BT_CS_GN_CMD_Q_FULL;

/*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  bt_cmd_ptr = bt_get_free_cmd();

  if ( bt_cmd_ptr  != NULL )
  {
    bt_initialize_cmd_buffer( bt_cmd_ptr );

    bt_cmd_ptr->cmd_hdr.cmd_type    = BT_CMD_HC_RD_BUFFER_SIZE;
    bt_cmd_ptr->cmd_hdr.bt_app_id   = bt_dc_last_hci_ev_reg_app_id;

    bt_cmd( bt_cmd_ptr );
  }

  return( cmd_status );

}/* bt_cmd_hc_rd_buf_size */


/*==========================================================================

FUNCTION     
  bt_cmd_hc_host_buf_size

DESCRIPTION  
  This function sends the Host_Buffer_Size command that is used by the Host to
  notify the Controller about the number and maximum size of the data portion 
  of HCI ACL and SCO Packets that can be sent from the Controller to the Host.

SIDE EFFECTS 
  None

==========================================================================*/
bt_cmd_status_type bt_cmd_hc_host_buf_size( void )
{
  bt_cmd_msg_type*    cmd_ptr;
  bt_cmd_status_type  cmd_status = BT_CS_GN_CMD_Q_FULL;
  uint16 small_dsm_cnt = DSM_POOL_FREE_CNT(DSM_DS_SMALL_ITEM_POOL);
  uint16 large_dsm_cnt = DSM_POOL_FREE_CNT(DSM_DS_LARGE_ITEM_POOL);      

/*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  cmd_ptr = bt_get_free_cmd();

  if ( cmd_ptr  != NULL )
  {
    bt_initialize_cmd_buffer( cmd_ptr );

    cmd_ptr->cmd_hdr.cmd_type    = BT_CMD_HC_HOST_BUFFER_SIZE;
    cmd_ptr->cmd_hdr.bt_app_id   = bt_dc_last_hci_ev_reg_app_id;
    cmd_ptr->cmd_msg.cmd_hc_host_buf_size.acl_data_pkt_len = 
      BT_HC_HOST_ACL_DATA_PKT_LEN;
    cmd_ptr->cmd_msg.cmd_hc_host_buf_size.sco_data_pkt_len = 
      BT_HC_HOST_SCO_DATA_PKT_LEN;
    cmd_ptr->cmd_msg.cmd_hc_host_buf_size.tot_sco_data_pkts = 
      BT_HC_HOST_TOT_NUM_SCO_DATA_PKTS;

   /* Ensure BT_HC_HOST_TOT_NUM_ACL_DATA_PKTS is <= min(small_dsm_cnt,large_dsm_cnt). 
      If not set 'tot_acl_data_pkts' to the min value if the DSM counts.
      This is not very essential because current value of 
      BT_HC_HOST_TOT_NUM_ACL_DATA_PKTS is a pretty small number. We would
      expect dsm counts to be higher but nevertheless  his initialization is
      done only once during SOC power-up so it should be okay to check for it.
     */      
    if ( BT_HC_HOST_TOT_NUM_ACL_DATA_PKTS  <= MIN(small_dsm_cnt,large_dsm_cnt))
    {
    cmd_ptr->cmd_msg.cmd_hc_host_buf_size.tot_acl_data_pkts = 
      BT_HC_HOST_TOT_NUM_ACL_DATA_PKTS;
    }
    else
    {
      cmd_ptr->cmd_msg.cmd_hc_host_buf_size.tot_acl_data_pkts = 
        MIN(small_dsm_cnt,large_dsm_cnt);
      BT_MSG_HIGH( "BT CMD: Reduced Tot Acl Data Pkts because of Low DSM Cnt",
                    0, 0, 0 );        
    }

    cmd_status = bt_cmd( cmd_ptr );
  }

  return( cmd_status );

} /* bt_cmd_hc_host_buf_size */


/*==========================================================================

FUNCTION     
  bt_cmd_hc_set_hc_to_host_fc

DESCRIPTION  
  This function sends the HCI_Set_Controller_To_Host_Flow_Control command that 
  is used by the Host to turn flow control on or off for data and/or voice sent
  in the direction from the Controller to the Host.

SIDE EFFECTS 
  None

==========================================================================*/
bt_cmd_status_type bt_cmd_hc_set_hc_to_host_fc( void )
{
  bt_cmd_msg_type*    cmd_ptr;
  bt_cmd_status_type  cmd_status = BT_CS_GN_CMD_Q_FULL;

/*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  cmd_ptr = bt_get_free_cmd();

  if ( cmd_ptr  != NULL )
  {
    bt_initialize_cmd_buffer( cmd_ptr );

    cmd_ptr->cmd_hdr.cmd_type    = BT_CMD_HC_SET_HC_TO_HOST_FC;
    cmd_ptr->cmd_hdr.bt_app_id   = bt_dc_last_hci_ev_reg_app_id;
    cmd_ptr->cmd_msg.cmd_hc_set_hc_to_host_fc.flow_control_enable = 
      BT_HC_FLOW_TO_HOST_ACL_M;

    cmd_status = bt_cmd( cmd_ptr );
  }

  return( cmd_status );

} /* bt_cmd_hc_set_hc_to_host_fc */


/*===========================================================================

FUNCTION
  bt_cmd_hc_control_afh

DESCRIPTION
  This proproetary HCI command enables/disables AFH in the Bluetooth Driver.

===========================================================================*/
bt_cmd_status_type bt_cmd_hc_control_afh
(
  uint8 enable
)
{
  bt_cmd_msg_type*    cmd_ptr;
  bt_cmd_status_type  cmd_status = BT_CS_GN_CMD_Q_FULL;

  if ( (cmd_ptr = bt_get_free_cmd()) != NULL )
  {
    cmd_ptr->cmd_hdr.cmd_type  = BT_CMD_HC_CONTROL_AFH;
    cmd_ptr->cmd_hdr.bt_app_id = bt_dc_last_hci_ev_reg_app_id;
    cmd_ptr->cmd_msg.cmd_hc_control_afh.enable = enable;

    cmd_status = bt_cmd( cmd_ptr );
  }

  return( cmd_status );
}


#ifdef FEATURE_BT_TEST_API
#error code not present
#endif /* FEATURE_BT_TEST_API */

/*===========================================================================

FUNCTION
  bt_cmd_hc_wr_inq_scan_type

DESCRIPTION
  This function sends an HCI command to write inqyiry scan type

===========================================================================*/
bt_cmd_status_type bt_cmd_hc_wr_inq_scan_type
(
  bt_inq_scan_mode_type scan_type 
)
{

  bt_cmd_msg_type*    cmd_ptr;
  bt_cmd_status_type  cmd_status = BT_CS_GN_CMD_Q_FULL;

  if ( (cmd_ptr = bt_get_free_cmd()) != NULL )
  {
    cmd_ptr->cmd_hdr.cmd_type  = BT_CMD_HC_WR_INQUIRY_SCAN_TYPE;
    cmd_ptr->cmd_hdr.bt_app_id = bt_dc_last_hci_ev_reg_app_id;

    cmd_ptr->cmd_msg.cmd_hc_wr_is_type.inq_scan_type = scan_type;
  
    cmd_status = bt_cmd( cmd_ptr );
  }

  return( cmd_status );

}

/*===========================================================================

FUNCTION
  bt_cmd_hc_wr_page_scan_type

DESCRIPTION
  This function sends an HCI command to write page scan type

===========================================================================*/
bt_cmd_status_type bt_cmd_hc_wr_page_scan_type
(
  bt_page_scan_mode_type scan_type 
)
{

  bt_cmd_msg_type*    cmd_ptr;
  bt_cmd_status_type  cmd_status = BT_CS_GN_CMD_Q_FULL;

  if ( (cmd_ptr = bt_get_free_cmd()) != NULL )
  {
    cmd_ptr->cmd_hdr.cmd_type  = BT_CMD_HC_WR_PAGE_SCAN_TYPE;
    cmd_ptr->cmd_hdr.bt_app_id = bt_dc_last_hci_ev_reg_app_id;

    cmd_ptr->cmd_msg.cmd_hc_wr_ps_type.page_scan_type = scan_type;
  
    cmd_status = bt_cmd( cmd_ptr );
  }

  return( cmd_status );

}

/*===========================================================================

FUNCTION
  bt_cmd_hc_rd_inq_scan_type

DESCRIPTION
  This function sends an HCI command to read inqyiry scan type

===========================================================================*/
bt_cmd_status_type bt_cmd_hc_rd_inq_scan_type
(
  void 
)
{

  bt_cmd_msg_type*    cmd_ptr;
  bt_cmd_status_type  cmd_status = BT_CS_GN_CMD_Q_FULL;

  if ( (cmd_ptr = bt_get_free_cmd()) != NULL )
  {
    cmd_ptr->cmd_hdr.cmd_type  = BT_CMD_HC_RD_INQUIRY_SCAN_TYPE;
    cmd_ptr->cmd_hdr.bt_app_id = bt_dc_last_hci_ev_reg_app_id;
  
    cmd_status = bt_cmd( cmd_ptr );
  }

  return( cmd_status );

}

/*===========================================================================

FUNCTION
  bt_cmd_hc_rd_page_scan_type

DESCRIPTION
  This function sends an HCI command to read page scan type

===========================================================================*/
bt_cmd_status_type bt_cmd_hc_rd_page_scan_type
(
  void 
)
{

  bt_cmd_msg_type*    cmd_ptr;
  bt_cmd_status_type  cmd_status = BT_CS_GN_CMD_Q_FULL;

  if ( (cmd_ptr = bt_get_free_cmd()) != NULL )
  {
    cmd_ptr->cmd_hdr.cmd_type  = BT_CMD_HC_RD_PAGE_SCAN_TYPE;
    cmd_ptr->cmd_hdr.bt_app_id = bt_dc_last_hci_ev_reg_app_id;
  
    cmd_status = bt_cmd( cmd_ptr );
  }

  return( cmd_status );

}

/*===========================================================================

FUNCTION
  bt_cmd_hc_create_connection_cancel

DESCRIPTION
  This function sends an HCI command to cancel create connection

===========================================================================*/
bt_cmd_status_type bt_cmd_hc_create_connection_cancel
(
  bt_bd_addr_type*  bd_addr_ptr
)
{

  bt_cmd_msg_type*    cmd_ptr;
  bt_cmd_status_type  cmd_status = BT_CS_GN_CMD_Q_FULL;

  if ( (cmd_ptr = bt_get_free_cmd()) != NULL )
  {
    cmd_ptr->cmd_hdr.cmd_type  = BT_CMD_HC_CREATE_CONNECTION_CANCEL;
    cmd_ptr->cmd_hdr.bt_app_id = bt_dc_last_hci_ev_reg_app_id;

    cmd_ptr->cmd_msg.cmd_hc_create_conn_cancel.bd_addr     = *bd_addr_ptr;
  
    cmd_status = bt_cmd( cmd_ptr );
  }

  return( cmd_status );

}


/*===========================================================================

FUNCTION
  bt_cmd_hc_remote_name_req_cancel

DESCRIPTION
  This function sends an HCI command to cancel remote name request

===========================================================================*/
bt_cmd_status_type bt_cmd_hc_remote_name_req_cancel
(
  bt_bd_addr_type*  bd_addr_ptr 
)
{

  bt_cmd_msg_type*    cmd_ptr;
  bt_cmd_status_type  cmd_status = BT_CS_GN_CMD_Q_FULL;

  if ( (cmd_ptr = bt_get_free_cmd()) != NULL )
  {
    cmd_ptr->cmd_hdr.cmd_type  = BT_CMD_HC_REMOTE_NAME_REQ_CANCEL;
    cmd_ptr->cmd_hdr.bt_app_id = bt_dc_last_hci_ev_reg_app_id;

    cmd_ptr->cmd_msg.cmd_hc_remote_name_req_cancel.bd_addr     = *bd_addr_ptr;
  
    cmd_status = bt_cmd( cmd_ptr );
  }

  return( cmd_status );

}

#endif /* FEATURE_BT_1_2 */

#ifdef BT_SWDEV_2_1_SSP

/*===========================================================================

FUNCTION
  bt_cmd_hc_io_capability_response

DESCRIPTION
  This function sends the current I/O capabilities of the host replying
  to an IO Capability Request event from the controller.

===========================================================================*/
bt_cmd_status_type bt_cmd_hc_io_capability_response
(
  bt_bd_addr_type*   bd_addr_ptr,
  uint8              io_capability,
  uint8              oob_data_present,
  uint8              auth_req
)
{

  bt_cmd_msg_type*    cmd_ptr;
  bt_cmd_status_type  cmd_status = BT_CS_GN_CMD_Q_FULL;

  if ( (cmd_ptr = bt_get_free_cmd()) != NULL )
  {
    cmd_ptr->cmd_hdr.cmd_type  = BT_CMD_HC_IO_CAPABILITY_RQST_REPLY;
    cmd_ptr->cmd_hdr.bt_app_id = bt_dc_last_hci_ev_reg_app_id;

    cmd_ptr->cmd_msg.cmd_hc_io_cap_res.bd_addr           = *bd_addr_ptr;
    cmd_ptr->cmd_msg.cmd_hc_io_cap_res.io_capability     = io_capability;
    cmd_ptr->cmd_msg.cmd_hc_io_cap_res.oob_data_present  = oob_data_present;
    cmd_ptr->cmd_msg.cmd_hc_io_cap_res.auth_req          = auth_req;
  
    cmd_status = bt_cmd( cmd_ptr );
  }

  return( cmd_status );

}

/*===========================================================================

FUNCTION
  bt_cmd_hc_user_cfmtn_rqst_reply

DESCRIPTION
  This function is used to reply to a User Confirmation Request event and
  indicates that the user selected "yes".

===========================================================================*/
bt_cmd_status_type bt_cmd_hc_user_cfmtn_rqst_reply
(
  bt_bd_addr_type*   bd_addr_ptr
)
{
  
  bt_cmd_msg_type*    cmd_ptr;
  bt_cmd_status_type  cmd_status = BT_CS_GN_CMD_Q_FULL;

  if ( (cmd_ptr = bt_get_free_cmd()) != NULL )
  {
    cmd_ptr->cmd_hdr.cmd_type  = BT_CMD_HC_USER_CFMTN_RQST_REPLY;
    cmd_ptr->cmd_hdr.bt_app_id = bt_dc_last_hci_ev_reg_app_id;

    cmd_ptr->cmd_msg.cmd_hc_user_cfm_rqst_rep.bd_addr = *bd_addr_ptr;
  
    cmd_status = bt_cmd( cmd_ptr );
  }

  return( cmd_status );

}

/*===========================================================================

FUNCTION
  bt_cmd_hc_user_cfmtn_rqst_neg_reply

DESCRIPTION
  This function is used to reply to a User Confirmation Request event and
  indicates that the user selected "no".

===========================================================================*/
bt_cmd_status_type bt_cmd_hc_user_cfmtn_rqst_neg_reply
(
  bt_bd_addr_type*   bd_addr_ptr
)
{

  bt_cmd_msg_type*    cmd_ptr;
  bt_cmd_status_type  cmd_status = BT_CS_GN_CMD_Q_FULL;

  if ( (cmd_ptr = bt_get_free_cmd()) != NULL )
  {
    cmd_ptr->cmd_hdr.cmd_type  = BT_CMD_HC_USER_CFMTN_RQST_NEG_REPLY;
    cmd_ptr->cmd_hdr.bt_app_id = bt_dc_last_hci_ev_reg_app_id;

    cmd_ptr->cmd_msg.cmd_hc_user_cfm_rqst_neg_rep.bd_addr = *bd_addr_ptr;
  
    cmd_status = bt_cmd( cmd_ptr );
  }

  return( cmd_status );

}

/*===========================================================================

FUNCTION
  bt_cmd_hc_user_passkey_rqst_reply

DESCRIPTION
  This function is used to reply to a User Passkey Request event and
  specifies the Numeric_Value (passkey) entered by the user.

===========================================================================*/
bt_cmd_status_type bt_cmd_hc_user_passkey_rqst_reply
(
  bt_bd_addr_type*   bd_addr_ptr,
  uint32             num_val
)
{

  bt_cmd_msg_type*    cmd_ptr;
  bt_cmd_status_type  cmd_status = BT_CS_GN_CMD_Q_FULL;

  if ( (cmd_ptr = bt_get_free_cmd()) != NULL )
  {
    cmd_ptr->cmd_hdr.cmd_type  = BT_CMD_HC_USER_PASSKEY_RQST_REPLY;
    cmd_ptr->cmd_hdr.bt_app_id = bt_dc_last_hci_ev_reg_app_id;

    cmd_ptr->cmd_msg.cmd_hc_user_passkey_rqst_rep.bd_addr = *bd_addr_ptr;
    cmd_ptr->cmd_msg.cmd_hc_user_passkey_rqst_rep.num_val = num_val;
  
    cmd_status = bt_cmd( cmd_ptr );
  }

  return( cmd_status );

}

/*===========================================================================

FUNCTION
  bt_cmd_hc_user_passkey_rqst_neg_reply

DESCRIPTION
  This function is used to reply to a User Passkey Request event and
  indicates the host could not provide a passkey.

===========================================================================*/
bt_cmd_status_type bt_cmd_hc_user_passkey_rqst_neg_reply
(
  bt_bd_addr_type*   bd_addr_ptr
)
{

  bt_cmd_msg_type*    cmd_ptr;
  bt_cmd_status_type  cmd_status = BT_CS_GN_CMD_Q_FULL;

  if ( (cmd_ptr = bt_get_free_cmd()) != NULL )
  {
    cmd_ptr->cmd_hdr.cmd_type  = BT_CMD_HC_USER_PASSKEY_RQST_NEG_REPLY;
    cmd_ptr->cmd_hdr.bt_app_id = bt_dc_last_hci_ev_reg_app_id;

    cmd_ptr->cmd_msg.cmd_hc_user_passkey_rqst_neg_rep.bd_addr = *bd_addr_ptr;
  
    cmd_status = bt_cmd( cmd_ptr );
  }

  return( cmd_status );

}

/*===========================================================================

FUNCTION
  bt_cmd_hc_remote_oob_data_rqst_reply

DESCRIPTION
  This function is used to reply to a Remote OOB Data Request event with
  the C and R values received via an OOB transfer from a remote device.

===========================================================================*/
bt_cmd_status_type bt_cmd_hc_remote_oob_data_rqst_reply
(
  bt_bd_addr_type*   bd_addr_ptr,
  bt_sp_hash_type*   sp_hash_ptr,
  bt_sp_rand_type*   sp_rand_ptr
)
{

  bt_cmd_msg_type*    cmd_ptr;
  bt_cmd_status_type  cmd_status = BT_CS_GN_CMD_Q_FULL;

  if ( (cmd_ptr = bt_get_free_cmd()) != NULL )
  {
    cmd_ptr->cmd_hdr.cmd_type  = BT_CMD_HC_REMOTE_OOB_DATA_RQST_REPLY;
    cmd_ptr->cmd_hdr.bt_app_id = bt_dc_last_hci_ev_reg_app_id;

    cmd_ptr->cmd_msg.cmd_hc_remote_oob_data_rqst_rep.bd_addr = *bd_addr_ptr;
    cmd_ptr->cmd_msg.cmd_hc_remote_oob_data_rqst_rep.sp_hash = *sp_hash_ptr;
    cmd_ptr->cmd_msg.cmd_hc_remote_oob_data_rqst_rep.sp_rand = *sp_rand_ptr;
  
    cmd_status = bt_cmd( cmd_ptr );
  }

  return( cmd_status );

}

/*===========================================================================

FUNCTION
  bt_cmd_hc_remote_oob_data_rqst_neg_reply

DESCRIPTION
  This functionis used to reply to a Remote OOB Data Request event that
  the Host does not have the C and R values.

===========================================================================*/
bt_cmd_status_type bt_cmd_hc_remote_oob_data_rqst_neg_reply
(
  bt_bd_addr_type*   bd_addr_ptr
)
{

  bt_cmd_msg_type*    cmd_ptr;
  bt_cmd_status_type  cmd_status = BT_CS_GN_CMD_Q_FULL;

  if ( (cmd_ptr = bt_get_free_cmd()) != NULL )
  {
    cmd_ptr->cmd_hdr.cmd_type  = BT_CMD_HC_REMOTE_OOB_DATA_RQST_NEG_REPLY;
    cmd_ptr->cmd_hdr.bt_app_id = bt_dc_last_hci_ev_reg_app_id;

    cmd_ptr->cmd_msg.cmd_hc_remote_oob_data_rqst_neg_rep.bd_addr =
      *bd_addr_ptr;
  
    cmd_status = bt_cmd( cmd_ptr );
  }

  return( cmd_status );

}

/*===========================================================================

FUNCTION
  bt_cmd_hc_io_capability_rqst_neg_reply

DESCRIPTION
  This function is used to reject a pairing attempt after an HCI IO
  Capability Request event has been received by the Host.

===========================================================================*/
bt_cmd_status_type bt_cmd_hc_io_capability_rqst_neg_reply
(
  bt_bd_addr_type*   bd_addr_ptr,
  uint8              reason
)
{

  bt_cmd_msg_type*    cmd_ptr;
  bt_cmd_status_type  cmd_status = BT_CS_GN_CMD_Q_FULL;

  if ( (cmd_ptr = bt_get_free_cmd()) != NULL )
  {
    cmd_ptr->cmd_hdr.cmd_type  = BT_CMD_HC_IO_CAPABILITY_RQST_NEG_REPLY;
    cmd_ptr->cmd_hdr.bt_app_id = bt_dc_last_hci_ev_reg_app_id;

    cmd_ptr->cmd_msg.cmd_hc_io_cap_rqst_neg_rep.bd_addr = *bd_addr_ptr;
    cmd_ptr->cmd_msg.cmd_hc_io_cap_rqst_neg_rep.reason  = reason;
  
    cmd_status = bt_cmd( cmd_ptr );
  }

  return( cmd_status );

}

/*===========================================================================

FUNCTION
  bt_cmd_hc_refresh_encryption_key

DESCRIPTION
  This function is used by the Host to cause the Controller to refresh the
  encryption key by pausing and resuming encryption.

===========================================================================*/
bt_cmd_status_type bt_cmd_hc_refresh_encryption_key
(
  uint16   conn_hndl
)
{

  bt_cmd_msg_type*    cmd_ptr;
  bt_cmd_status_type  cmd_status = BT_CS_GN_CMD_Q_FULL;

  if ( (cmd_ptr = bt_get_free_cmd()) != NULL )
  {
    cmd_ptr->cmd_hdr.cmd_type  = BT_CMD_HC_REFRESH_ENCRYPTION_KEY;
    cmd_ptr->cmd_hdr.bt_app_id = bt_dc_last_hci_ev_reg_app_id;

    cmd_ptr->cmd_msg.cmd_hc_refresh_enc_key.conn_hndl = conn_hndl;
  
    cmd_status = bt_cmd( cmd_ptr );
  }

  return( cmd_status );

}

/*===========================================================================

FUNCTION
  bt_cmd_hc_rd_simple_pairing_mode

DESCRIPTION
  This function reads the Simple Pairing mode setting in the Controller.

===========================================================================*/
bt_cmd_status_type bt_cmd_hc_rd_simple_pairing_mode
(
  void
)
{

  bt_cmd_msg_type*    cmd_ptr;
  bt_cmd_status_type  cmd_status = BT_CS_GN_CMD_Q_FULL;

  if ( (cmd_ptr = bt_get_free_cmd()) != NULL )
  {
    cmd_ptr->cmd_hdr.cmd_type  = BT_CMD_HC_RD_SIMPLE_PAIRING_MODE;
    cmd_ptr->cmd_hdr.bt_app_id = bt_dc_last_hci_ev_reg_app_id;
  
    cmd_status = bt_cmd( cmd_ptr );
  }

  return( cmd_status );

}

/*===========================================================================

FUNCTION
  bt_cmd_hc_wr_simple_pairing_mode

DESCRIPTION
  This function enables Simple Pairing mode in the Controller.

===========================================================================*/
bt_cmd_status_type bt_cmd_hc_wr_simple_pairing_mode
(
  uint8   sp_mode
)
{

  bt_cmd_msg_type*    cmd_ptr;
  bt_cmd_status_type  cmd_status = BT_CS_GN_CMD_Q_FULL;

  if ( (cmd_ptr = bt_get_free_cmd()) != NULL )
  {
    cmd_ptr->cmd_hdr.cmd_type  = BT_CMD_HC_WR_SIMPLE_PAIRING_MODE;
    cmd_ptr->cmd_hdr.bt_app_id = bt_dc_last_hci_ev_reg_app_id;

    cmd_ptr->cmd_msg.cmd_hc_wr_sp_mode.sp_mode = sp_mode;
  
    cmd_status = bt_cmd( cmd_ptr );
  }

  return( cmd_status );

}

#ifdef FEATURE_BT_2_1
/*===========================================================================

FUNCTION
  bt_cmd_hc_rd_default_err_data_reporting_mode

DESCRIPTION
  This function reads the Default Erroneous Data Reporting mode in the
  Controller.

===========================================================================*/
bt_cmd_status_type bt_cmd_hc_rd_default_err_data_reporting_mode
(
  void
)
{

  bt_cmd_msg_type*    cmd_ptr;
  bt_cmd_status_type  cmd_status = BT_CS_GN_CMD_Q_FULL;

  if ( (cmd_ptr = bt_get_free_cmd()) != NULL )
  {
    cmd_ptr->cmd_hdr.cmd_type  = BT_CMD_HC_RD_DEFAULT_ERR_DATA_REPORTING_MODE;
    cmd_ptr->cmd_hdr.bt_app_id = bt_dc_last_hci_ev_reg_app_id;
  
    cmd_status = bt_cmd( cmd_ptr );
  }

  return( cmd_status );

}

/*===========================================================================

FUNCTION
  bt_cmd_hc_wr_default_err_data_reporting_mode

DESCRIPTION
  This function sends "Write Default Erroneous Data Reporting  mode in the 
  Controller.

===========================================================================*/
bt_cmd_status_type bt_cmd_hc_wr_default_err_data_reporting_mode
(
  uint8   err_data_reporting_mode
)
{

  bt_cmd_msg_type*    cmd_ptr;
  bt_cmd_status_type  cmd_status = BT_CS_GN_CMD_Q_FULL;

  if ( (cmd_ptr = bt_get_free_cmd()) != NULL )
  {
    cmd_ptr->cmd_hdr.cmd_type  = BT_CMD_HC_WR_DEFAULT_ERR_DATA_REPORTING_MODE;
    cmd_ptr->cmd_hdr.bt_app_id = bt_dc_last_hci_ev_reg_app_id;

    cmd_ptr->cmd_msg.cmd_hc_wr_default_err_data_reporting.default_err_reporting_mode
         = err_data_reporting_mode;
  
    cmd_status = bt_cmd( cmd_ptr );
  }

  return( cmd_status );

}

#endif /* FEATURE_BT_2_1 */

/*===========================================================================

FUNCTION
  bt_cmd_hc_rd_local_oob_data

DESCRIPTION
  This function is used to obtain a Simple Pairing Hash C and Simple Pairing
  Randomizer R to be transferred to a remote device using OOB.

===========================================================================*/
bt_cmd_status_type bt_cmd_hc_rd_local_oob_data
(
  void
)
{

  bt_cmd_msg_type*    cmd_ptr;
  bt_cmd_status_type  cmd_status = BT_CS_GN_CMD_Q_FULL;

  if ( (cmd_ptr = bt_get_free_cmd()) != NULL )
  {
    cmd_ptr->cmd_hdr.cmd_type  = BT_CMD_HC_RD_LOCAL_OOB_DATA;
    cmd_ptr->cmd_hdr.bt_app_id = bt_dc_last_hci_ev_reg_app_id;
  
    cmd_status = bt_cmd( cmd_ptr );
  }

  return( cmd_status );

}

/*===========================================================================

FUNCTION
  bt_cmd_hc_send_keypress_notification

DESCRIPTION
  This function informs the remote device when keys have been entered
  or erased.

===========================================================================*/
bt_cmd_status_type bt_cmd_hc_send_keypress_notification
(
  bt_bd_addr_type*   bd_addr_ptr,
  uint8              notification_type
)
{

  bt_cmd_msg_type*    cmd_ptr;
  bt_cmd_status_type  cmd_status = BT_CS_GN_CMD_Q_FULL;

  if ( (cmd_ptr = bt_get_free_cmd()) != NULL )
  {
    cmd_ptr->cmd_hdr.cmd_type  = BT_CMD_HC_SEND_KEYPRESS_NOTIFICATION;
    cmd_ptr->cmd_hdr.bt_app_id = bt_dc_last_hci_ev_reg_app_id;

    cmd_ptr->cmd_msg.cmd_hc_send_keypress_ntfctn_type.bd_addr           =
      *bd_addr_ptr;
    cmd_ptr->cmd_msg.cmd_hc_send_keypress_ntfctn_type.notification_type =
      notification_type;
  
    cmd_status = bt_cmd( cmd_ptr );
  }

  return( cmd_status );

}

/*===========================================================================

FUNCTION
  bt_cmd_hc_wr_simple_pairing_debug_mode

DESCRIPTION
  This function configures the Controller to use a predefined Diffie Hellman
  private key for Simple Pairing.

===========================================================================*/
bt_cmd_status_type bt_cmd_hc_wr_simple_pairing_debug_mode
(
  uint8   sp_debug_mode
)
{

  bt_cmd_msg_type*    cmd_ptr;
  bt_cmd_status_type  cmd_status = BT_CS_GN_CMD_Q_FULL;

  if ( (cmd_ptr = bt_get_free_cmd()) != NULL )
  {
    cmd_ptr->cmd_hdr.cmd_type  = BT_CMD_HC_WR_SIMPLE_PAIRING_DEBUG_MODE;
    cmd_ptr->cmd_hdr.bt_app_id = bt_dc_last_hci_ev_reg_app_id;

    cmd_ptr->cmd_msg.cmd_hc_wr_sp_debug_mode.sp_debug_mode = sp_debug_mode;
  
    cmd_status = bt_cmd( cmd_ptr );
  }

  return( cmd_status );

}

#endif /* BT_SWDEV_2_1_SSP */


#ifdef BT_SWDEV_2_1_SSR
/*===========================================================================

FUNCTION
  bt_cmd_hc_sniff_subrating

DESCRIPTION
  This function specifies the parameters for sniff subrating for a given link.

===========================================================================*/
bt_cmd_status_type bt_cmd_hc_sniff_subrating
(
  uint16            conn_hndl,
  uint16            max_latency,
  uint16            min_remote_to,
  uint16            min_local_to
)
{
  bt_cmd_msg_type*    cmd_ptr;
  bt_cmd_status_type  cmd_status = BT_CS_GN_CMD_Q_FULL;

  if ( (cmd_ptr = bt_get_free_cmd()) != NULL )
  {
    cmd_ptr->cmd_hdr.cmd_type            = BT_CMD_HC_SNIFF_SUBRATING;
    cmd_ptr->cmd_hdr.bt_app_id           = bt_dc_last_hci_ev_reg_app_id;

    cmd_ptr->cmd_msg.cmd_hc_ssr.conn_hndl     = conn_hndl;
    cmd_ptr->cmd_msg.cmd_hc_ssr.max_latency   = max_latency;
    cmd_ptr->cmd_msg.cmd_hc_ssr.min_remote_to = min_remote_to;
    cmd_ptr->cmd_msg.cmd_hc_ssr.min_local_to  = min_local_to;
  
    cmd_status = bt_cmd( cmd_ptr );
  }

  return( cmd_status );
}
#endif /* BT_SWDEV_2_1_SSR */

#endif /* FEATURE_BT */

