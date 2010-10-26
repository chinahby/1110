/*====*====*====*====*====*====*====*====*====*====*====*====*====*====*====*

            B L U E T O O T H    S D P    M O D U L E

GENERAL DESCRIPTION
  This contains the highest level code for the Bluetooth SDP layer.

Copyright (c) 2000 - 2008 by QUALCOMM Incorporated. 
All Rights Reserved.
Qualcomm Confidential and Proprietary.
*====*====*====*====*====*====*====*====*====*====*====*====*====*====*====*/

/*===========================================================================

                        EDIT HISTORY FOR MODULE

  This section contains comments describing changes made to the module.
  Notice that changes are listed in reverse chronological order.

$PVCSHeader:   O:/src/asw/COMMON/vcs/btsd.c_v   1.18   21 Feb 2002 19:16:00   ryuen
$Header: //source/qcom/qct/wconnect/bthost/core/rel/00.00.26/src/btsd.c#1 $ $DateTime:

$Log:   O:/src/asw/COMMON/vcs/btsd.c_v  $

  when        who  what, where, why
  ----------  ---  -----------------------------------------------------------
  2008-08-14   rb  Added handling to keep the background timer tick during
                   power collapse

 *
 *   #57        29 Feb 2008            AG
 * Updated comments and cleaned up code.
 * 
 *    #56        23 Jan 2008            HM
 * Returning an error message in search service when command 
 * retry count reaches max value
 *
 *    #55        12 Dec 2007            HM
 * Added SD extended command to register custom services 
 *
 *    #54        29 Nov 2007            HM
 * Added SD command to return service search results in XML format
 * Added command to cancel service search. 
 * Added a parameter to specify duration of a device search. 
 * Added a direct function call to get the discovery mode.
 *
 *    #53        07 Nov 2007            GS
 * Added interface to cancel SD Idle timer and disconnect L2 channels
 *
 *    #52        01 Nov 2007            AG
 * Merged Lisbon(BT2.1) changes into the mainline
 *
 *    #51        25 Oct 2007            MH
 * Modified SD Discover devices command to copy BD address only
 * when service class type is unknown
 *
 *    #50        17 Jul 2007            PR
 * Remote name request cancel support
 *
 *    #49        15 May 2007            SSK
 * Fixed compiler warnings by removing unused variables and by changing
 * error field type in struct 'bt_ev_sd_error_resp_type'.
 *
 *    #48        11 Apr 2007            RY
 * Updated bt_sd_search_service to 1.) requeue service search request if one
 * is already pending; 2.) save the client app ID only when its service
 * search request is ready to be processed
 *
 *    #47        16 Feb 2007            PN
 * Fixed critical lint errors.
 *
 *    #46        29 Jan 2007            MH
 * Added support to update OBEX Profile service record from 128-bit UUID.
 *
 *    #45        21 Dec  2006           VK
 * SD module limits numbers of hops for SDP requests if "ContinuationState" 
 * in SDP responses do not change.
 *
 *    #44        16 Nov 2006            RY
 * Initialize num_uuid128 to 0 prior to calls to bt_cmd_sd_search_service. 
 *
 *    #43        14 Nov  2006           RY
 * Added SD support for custom 128-bit UUID
 *
 *    #42        29 Oct 2006            RY
 * Updated bt_sd_search_service to:
 * 1. requeue the command only when there is a local-initated connection to
 *    be disconnected.
 * 2. check for non-NULL local CID before sending a service search request.
 *
 *    #41        14 Sep 2006            RY
 * Updated to use bt_cmd_l2_connect_immediate_cid to correctly track the
 * local and remote initiated CIDs.
 *
 *    #40        31 Aug 2006            RY
 * Updated bt_sd_search_service to tear down all locally initiated idle L2CAP
 * connections before starting a service search to a new device.  This allows
 * back-to-back service searches to more than 3 devices.
 *
 *    #39        28 Jul 2006            DM
 * Changed bt_cmd_l2_write to pass NULL frame_handle.
 *
 *    #37        27 Jun 2006            DSN
 * Fixed typecasting for RVCT compiler.
 *
 *    #36        23 Jun 2006            JH
 * OBEX protocol descriptor is now added in 
 *  bt_sd_update_obex_profile_service_record() instead of 
 *  bt_sd_create_service_record() in btsddb.c.
 *
 *    #35        21 Jun 2006            JH
 * Clear capturing and rendering class of device bits when deregistering A2DP
 *  audio source and sink respectively.
 *
 *    #34        30 May 2006            PG
 * Initialized remote_name_req_in_progress to false on power up.
 *
 *    #33        30 May 2006            GS
 * Added support for PBAP feature.
 * 
 *    #32        09 Dec 2005            DSN
 * Fixed RVCT compiler errors.
 *
 *    #31        10 Nov 2005            DP
 * Fixed previous changes for connectable controls on SD service
 * registrations by making use of new connectable flag instead
 * of using the enable flag.
 *
 *    #30        9  Nov 2005            DP
 * Renamed and extended SD service registration command to
 * allow SD service registrations to specify whether or not
 * the device should be connectable based on the existence
 * of the SD service registration.
 *
 *    #28        12 Sep 2005            JTL
 * Improving DSM allocation.
 *
 *    #27        24 Jun 2005            PN
 * bt_sd_get_device_name() remembers BD address of remote device.
 *
 *    #26        23 Jun 2005            JTL
 * Add Rendering bit to COD when registering and audio sink, and capturing bit
 * for audio source.
 *
 *    #25        16 May 2004            RY
 * Updated bt_sd_parse_class_of_device to handle the positioning service class.
 *
 *    #24        10 Apr 2004            RY
 * Adding support for SD Get Discoverable Mode.
 *
 *    #23        18 Dec 2004            JH
 * Modified bt_sd_register_service() to allow services with the same UUID to be
 *  registered as long as they have different SCN or PSM.
 * Added support for the following new commands:
 *  BT_CMD_SD_GET_SECONDARY_SERVER_CHANNEL_NUMBER, 
 *  BT_CMD_SD_UPDATE_SRV_REC_BY_SCN, BT_CMD_SD_UPDATE_SRV_REC_BY_PSM,
 *  BT_CMD_SD_UPDATE_SRV_NAME, BT_CMD_SD_REGISTER_SRV, 
 *  BT_CMD_SD_UNREGISTER_SRV_BY_SCN, BT_CMD_SD_UNREGISTER_SRV_BY_PSM
 *
 *    #22         1 Dec 2004            JH
 * Removed the #ifdef FEATURE_BT_EXTPF around bt_sd_set_cod_srv_class_bit() and 
 *  bt_sd_clear_cod_srv_class_bit()
 *
 *    #21        23 Nov 2004            JH
 * Replaced bt_sd_set/clear_cod_obj_xfer_bit() functions with the more generic
 *   bt_sd_set/clear_cod_srv_class_bit() functions.
 * Modified bt_sd_register_service()/bt_sd_unregister_service() to set/clear
 *  the networking bit of the class of service field when networking services
 *  are registered/deregistered.
 * Use bt_sd_major_service_class_enum_type enumerations instead of hardcoding
 *  numbers.
 *
 *    #20        24 Sep 2004            DP
 * Reworked AMS compilation controls.
 * 
 *    #19        30 Aug 2004            JH
 * Modified bt_sd_search_service to support searching based on 128-bit UUID.
 * Added bt_sd_update_obex_profile_service_record to centralize OBEX service
 *  record creation.
 * Added bt_sd_register_syncml_service and bt_sd_unregister_syncml_service.
 * Moved functions for setting object transfer bit in class of device field here
 *  from btpf.c.
 * Modified bt_sd_register_service and bt_sd_unregister_service to manipulate
 *  object transfer bit in class of device field. 
 *
 *    #17-18      2  Jun 2004           DP
 * Implemented idle timeout disconnect for SDP connections initiated
 * by remote devices.
 *
 *    #16        30 Mar 2004            DP
 * Added EMS support.
 *
 *    #15        26 Feb 2004            PN
 * Added debug messages to bt_sd_extract*().
 *
 *    #14        30 Jan 2004            DP
 * Removed compiler warning.
 *
 *    #13        14 Jan 2004            DP
 * Added support for limited discoverable scanning.
 * Registering service only affects page scanning now (not inquiry scanning).
 * Setting discoverable mode only affects inquiry scanning now.
 * NA no longer registered as a service by default.
 *
 *    #12        19 Dec 2003            DP
 * SD set service discoverable now keeps track by application ID.
 * Improved error handling when building/parsing SDP database records.
 *
 *    #11        26 Nov 2003            DP
 * Improved command status reporting for failures in some commands.
 *
 *    #10        02 Oct 2003            PN
 * Added utility functions bt_sd_extract_srvc_cls(), bt_sd_extract_scn(),
 * bt_sd_extract_profile_version(), bt_sd_parse_class_of_device(), and
 * bt_sd_build_class_of_device().
 *
 *    #9         26 Sep 2003            DP
 * Made service name character string.
 *
 *    #9         26 Sep 2003            DP
 * Made service name character string.
 *
 *    #8         9  Sep 2003            RY
 * Updated to use 'cid1' for self-initiated L2CAP connection.
 * 
 *    #7         03 Jul 2003            RY
 * 1. Added the ImagingResponder service record.
 * 2. Removed the LANAccessPPP service record.
 * 
 *    Rev 1.18   21 Feb 2002 19:16:00   ryuen
 * Removed featurization on FEATURE_BT_SIO_API_ENHANCEMENT
 * 
 *    Rev 1.17   08 Feb 2002 17:40:46   ryuen
 * Added bt_cmd_sd_register_service and bt_cmd_sd_unregister_service
 * 
 *    Rev 1.16   13 Nov 2001 13:48:32   ryuen
 * Added bt_sd_update_service_record
 * 
 *    Rev 1.15   26 Sep 2001 12:23:20   waynelee
 * Completed BT_MSG_API_[PYLD] msgs for SD.
 * 
 *    Rev 1.14   14 Sep 2001 01:25:02   ryuen
 * Updated to use the new error code enumerations
 * 
 *    Rev 1.13   07 Sep 2001 16:06:42   propach
 * Optimized event handling.
 * 
 *    Rev 1.12   29 Aug 2001 11:57:20   propach
 * Merged certification tip onto main tip.
 * 
 *    Rev 1.11.1.2   13 Aug 2001 14:22:40   ryuen
 * 1. Added bt_sd_get_server_channel_number
 * 2. Updated bt_sd_powerup_init to initialize the timers (one per connection)
 * 3. Updated bt_sd_send_pending_request to clear the idle timer and
 *    set the request timeout timer
 * 4. Added bt_sd_process_timer_expiration
 * 
 *    Rev 1.11.1.1   15 Jul 2001 14:13:00   ryuen
 * Made bt_sd_sig_tx_logging_function non-static
 * 
 *    Rev 1.11.1.0   29 Jun 2001 15:25:34   waynelee
 * No change.
 * 
 *    Rev 1.11   25 May 2001 15:40:56   ryuen
 * Renamed FEATURE_BT_MOBILE_MAP to FEATURE_BT_LPOS
 * 
 *    Rev 1.10   25 May 2001 10:12:46   ryuen
 * Added support for FEATURE_BT_MOBILE_MAP
 * 
 *    Rev 1.9   22 May 2001 17:19:40   ryuen
 * Updated bt_sd_sig_tx_logging_function to call log_alloc with just the
 * right amount of bytes to log, instead of with the size of the entire log
 * buffer
 * 
 *    Rev 1.8   17 May 2001 17:16:10   amitj
 * Modified bt_cmd_l2_set_params to include NULL cid value
 * 
 *    Rev 1.7   26 Apr 2001 23:26:16   ryuen
 * 1. Updated bt_sd_send_pending_request to increment the transaction ID
 *    only for new request, i.e. not a continuation
 * 2. Added BT_MSG where appropriate
 * 
 *    Rev 1.6   21 Mar 2001 23:28:20   ryuen
 * Removed all references to deleted continuation state information in
 * bt_sd_conn_entry_type
 * 
 *    Rev 1.5   02 Mar 2001 01:13:28   ryuen
 * 1. Added bt_sd_set_service_discoverable_mode
 * 2. Updated to use the new EC get_app_id_and_register API
 * 3. Renamed bt_sd_offline to bt_sd_qdsp_reset_shutdown
 * 
 *    Rev 1.4   23 Feb 2001 15:28:48   ryuen
 * 1. Renamed bt_sd_initialize and bt_sd_online to bt_sd_powerup_init and
 *    bt_sd_qdsp_ready_init respectively
 * 2. Updated bt_sd_search_service to correctly initialize the number of
 *    response received to 0.  This fixes CR 16196
 * 
 *    Rev 1.3   16 Feb 2001 16:11:52   propach
 * Completed EC routing of events by BT application ID.
 * 
 *    Rev 1.2   12 Feb 2001 17:42:06   waynelee
 * Added BT_USE_RC2SIO_SCN_FOR_NA_SDP feature.  Registers
 * the RC2SIO server channel for the NA SDP entry for used in
 * conjunction with the RC2SIO RFCOMM Server app.
 * 
 *    Rev 1.1   07 Feb 2001 10:18:16   propach
 * Reworked internal BT application IDs and made event control
 * route based on these.
 * 
 *    Rev 1.0   29 Jan 2001 17:10:04   propach
 * Initial revision.
 * 
 *    Rev 1.24   22 Jan 2001 14:19:34   propach
 * Fixed multiple ASSERT problem when going online from offline (for ryuen).
 * 
 *    Rev 1.23   08 Jan 2001 14:55:32   ryuen
 * Updated bt_sd_start_device_discovery to save the max_num_response
 * parameter; also, instead of passing it to bt_cmd_rm_start_inquiry,
 * always pass 0 such that RM does not check for maximum number of responses
 * 
 *    Rev 1.22   21 Dec 2000 17:48:02   waynelee
 * Removed T_CORE_EMUL.
 * 
 *    Rev 1.21   11 Dec 2000 17:46:24   amitj
 * Renamed the bt_sd_sig_tx() function; Removed bt_cmd_l2_write() from this
 * function; Replaced hardcoded values with literals in logging_function()
 * 
 *    Rev 1.20   30 Nov 2000 22:05:46   ryuen
 * Implemented bt_sd_get_device_name
 * 
 *    Rev 1.19   16 Nov 2000 17:46:36   amitj
 * Made a fix to break if log_alloc returns NULL for QXDM logging
 * 
 *    Rev 1.18   14 Nov 2000 19:01:28   amitj
 * Made some corrections to QXDM logging function
 * 
 *    Rev 1.17   13 Nov 2000 17:19:02   amitj
 * Moved  bt_sd_sig_tx() from the bottom to the top of the file
 * 
 *    Rev 1.16   08 Nov 2000 13:45:58   amitj
 * Defined bt_sd_sig_tx() and invoked it in bt_sd_send_pending_request()
 * 
 *    Rev 1.15   06 Nov 2000 12:54:50   ryuen
 * Added QXDM periodic logging
 * 
 *    Rev 1.14   03 Nov 2000 11:08:50   ryuen
 * 1. Updated bt_sd_search_service to correctly handle search pattern that
 *    specifies attribute ID range
 * 2. Updated bt_sd_send_pending_request to insert continuation state info
 * 
 *    Rev 1.13   30 Oct 2000 18:06:40   ryuen
 * 1. Implemented bt_sd_start_device_discovery and bt_sd_stop_device_discovery
 * 2. Updated bt_sd_search_service to also send SDP_ServiceSearchReq and
 *    SDP_ServiceAttributeReq
 * 3. Updated bt_sd_initialize to register for RM events
 * 4. Removed obsolete feature FEATURE_BT_START_PAGE_SCAN
 * 
 * 
 *    Rev 1.12   29 Sep 2000 17:36:20   waynelee
 * Fixed L2CAP so SDP works; fixed L2CAP for non-default MTU; added SDP entries;
 * fixed RD_BD_ADDR cmd_cmplt ev
 * 
 *    Rev 1.0   29 Sep 2000 17:32:32   waynelee
 * Initial revision.
 * 
 *    Rev 1.11   12 Sep 2000 14:08:48   ryuen
 * 1. Added bt_sd_online() and bt_sd_offline()
 * 2. Added debug MSGs
 * 
 *    Rev 1.10   05 Sep 2000 18:57:32   propach
 * Updated featurization.
 *
 *    Rev 1.9   31 Aug 2000 13:31:12   waynelee
 * Adding intermin auth params support.  Adding RM inquiry support.
 *
 *    Rev 1.8   30 Aug 2000 09:42:46   waynelee
 * Made changes for new BT_MSG macros
 *
 *    Rev 1.7   29 Aug 2000 08:26:20   propach
 * Moved comdef.h includes above FEATURE_BT to remove compiler
 * warning when feature is disabled.
 *
 *    Rev 1.6   17 Aug 2000 13:46:54   propach
 * Added Event Control (EC).  Fixed minor problems in RFCOMM.
 *
 *    Rev 1.5   14 Aug 2000 18:03:40   ryuen
 * Updated bt_sd_process_command() to return BT_CS_GN_UNRECOGNIZED_CMD
 *
 *    Rev 1.4   08 Aug 2000 10:59:30   ryuen
 * Added the support for sending SDP service search attribute request
 *
 *    Rev 1.3   03 Aug 2000 14:57:20   ryuen
 * Minor bug fixes
 *
 *    Rev 1.2   02 Aug 2000 13:04:06   ryuen
 * Added support for sending SDP Service Search Attribute Request
 *
 *    Rev 1.1   02 Aug 2000 11:31:32   propach
 * Made standalone BT compile.
 *
 *    Rev 1.0   31 Jul 2000 11:55:00   ryuen
 * Initial revision.

===========================================================================*/

/*===========================================================================

                     INCLUDE FILES FOR MODULE

===========================================================================*/

#include "customer.h"
#include "comdef.h"
#ifdef FEATURE_BT
#include <string.h>
#include "dsm.h"
#include "task.h"
#include "queue.h"
#include "bt.h"
#include "bti.h"
#include "btcmdi.h"
#include "btdebug.h"
#include "btl2.h"
#include "btmsg.h"
#include "btsd.h"

#if defined (BT_SWDEV_QXDM_LOG_MSG) || defined (BT_SWDEV_QXDM_LOG_PER)
#include "btlogi.h"
#include "log.h"
#endif


#define BT_MSG_LAYER  BT_MSG_SD   /* necessary for btmsg.h */

/*===========================================================================

                DEFINITIONS AND DECLARATIONS FOR MODULE

This section contains definitions for constants, macros, types, variables
and other items needed by this module.

===========================================================================*/

LOCAL bt_app_id_type  bt_sd_app_id;

LOCAL boolean bt_sd_connectable;

/*  Discoverable application table.  */
typedef struct
{
  bt_app_id_type                        bt_app_id;
  bt_sd_service_discoverable_mode_type  discoverable_mode;
} bt_sd_da_type;

#define BT_SD_MAX_DISCOVERABLE_APPS 20
LOCAL bt_sd_da_type  bt_da[ BT_SD_MAX_DISCOVERABLE_APPS ];

/* networking server reference count */
LOCAL uint16 bt_sd_networking_srv_count = 0; 
#ifdef FEATURE_BT_EXTPF
/* OBEX server reference count */
LOCAL uint16 bt_sd_obex_srv_count = 0; 
#endif /* FEATURE_BT_EXTPF */

#ifdef FEATURE_BT_EXTPF_AV
/* AV profile server reference count */
LOCAL uint16 bt_sd_av_srv_count = 0; 
#endif /* FEATURE_BT_EXTPF_AV */

/*-------------------------------------------------------------------------*/
/*  Information and queue for events destined for the SDP layer.           */
/*-------------------------------------------------------------------------*/

#define BT_SD_NUM_EVENTS    10
LOCAL bt_event_q_info_type  bt_sd_ev_q_info;
LOCAL q_type                bt_sd_ev_q;


/*-------------------------------------------------------------------------*/
/*                                                                         */
/*                         Static Functions                                */
/*                                                                         */
/*-------------------------------------------------------------------------*/

#ifdef BT_SWDEV_QXDM_LOG_MSG
/*===========================================================================

FUNCTION
  bt_sd_sig_tx_logging_function

DESCRIPTION
  Log all the bytes in the SDP signalling packet transmitted.

RETURN VALUE
  None.

===========================================================================*/
void bt_sd_sig_tx_logging_function
(
  bt_app_id_type  bt_app_id,
  uint16          cid,
  dsm_item_type   *dsm_ptr
)
{
  log_bt_sd_sig_tx_type   *log_ptr;
  uint16                  data_length;

  data_length = dsm_length_packet( dsm_ptr );
  data_length = MIN( data_length, BT_SD_TX_LOG_BUFFER_SIZE );

  log_ptr = (log_bt_sd_sig_tx_type   *)log_alloc( log_bt_sd_sig_tx,
                       sizeof(log_bt_sd_sig_tx_type) -
                       BT_SD_TX_LOG_BUFFER_SIZE + data_length );

  if ( log_ptr != NULL )
  {
    log_ptr->length = data_length;
    log_ptr->local_cid = cid;

    dsm_extract( dsm_ptr, BT_FIRST_BYTE_OFFSET, (void *)log_ptr->buffer, 
                 data_length );
    log_commit( log_ptr );
  }
}
#endif /* BT_SWDEV_QXDM_LOG_MSG */

/*===========================================================================

FUNCTION
  bt_sd_set_cod_srv_class_bit

DESCRIPTION
  Increment service reference count and sets the service class bit in 
  the class of device field.

RETURN VALUE
  None.

===========================================================================*/
LOCAL void bt_sd_set_cod_srv_class_bit( 
  bt_sd_major_service_class_enum_type bit_mask, 
  uint16*                             reference_cnt_ptr )
{
  bt_cod_type cod;

  (*reference_cnt_ptr)++;
  /* set class of service bit */
  (void)bt_cmd_rm_get_local_info(  /* only interested in class of device */
    bt_sd_app_id, NULL, NULL, NULL, &cod, NULL, NULL );
  cod.cod_bytes[2] |= bit_mask;
  (void)bt_cmd_rm_set_local_info( bt_sd_app_id, &cod, NULL );
}

/*===========================================================================

FUNCTION
  bt_sd_clear_cod_srv_class_bit

DESCRIPTION
  Decrement service reference count and if zero, clears the service class
  bit in the class of device field.

RETURN VALUE
  None.

===========================================================================*/
LOCAL void bt_sd_clear_cod_srv_class_bit( 
  bt_sd_major_service_class_enum_type bit_mask, 
  uint16*                             reference_cnt_ptr )
{
  bt_cod_type cod;

  (*reference_cnt_ptr)--;
  if ( *reference_cnt_ptr == 0 )
  {    
    /* clear class of service bit only if no more server is registered */ 
    (void)bt_cmd_rm_get_local_info(  /* only interested in class of device */
      bt_sd_app_id, NULL, NULL, NULL, &cod, NULL, NULL );
    cod.cod_bytes[2] &= ~bit_mask;
    (void)bt_cmd_rm_set_local_info( bt_sd_app_id, &cod, NULL );
  }
}

/*===========================================================================

FUNCTION
  bt_sd_store_bt_event

DESCRIPTION
  Stores a specified BT event on the SDP BT event queue.
  
============================================================================*/
LOCAL void bt_sd_store_bt_event
(
  bt_ev_msg_type*  bt_ev_ptr
)
{
  
  bt_store_bt_event( bt_ev_ptr, &bt_sd_ev_q_info );

}


/*===========================================================================

FUNCTION
  bt_sd_init_discoverable_app

DESCRIPTION
  Initializes a specified discoverable application table entry.

============================================================================*/
LOCAL void bt_sd_init_discoverable_app
(
  bt_sd_da_type*  da_ptr
)
{

  da_ptr->bt_app_id         = BT_APP_ID_NULL;
  da_ptr->discoverable_mode = BT_SD_SERVICE_DISCOVERABLE_MODE_NONE;

}


/*===========================================================================

FUNCTION
  bt_sd_get_discoverable_app

DESCRIPTION
  Finds the discoverable application table entry corresponding to a
  specified ID.  When necessary it sets up a new application table
  entry for the application.  A pointer to the entry is provided if
  available.

RETURN VALUES
  BT_CS_GN_SUCCESS if application pointer is valid; otherwise an
    appropriate command status failure value is returned and the
    application pointer is invalid.
  Pointer to application table entry passed by reference is updated.

===========================================================================*/
LOCAL bt_cmd_status_type bt_sd_get_discoverable_app
(
  boolean          can_be_new,
  bt_app_id_type   bt_app_id,
  bt_sd_da_type**  da_ptr_ptr
)
{

  bt_cmd_status_type  status = BT_CS_GN_SUCCESS;
  bt_sd_da_type*      da_ptr = NULL;
  bt_sd_da_type*      free_da_ptr = NULL;
  uint16              i;

  for ( i = 0; i < BT_SD_MAX_DISCOVERABLE_APPS; i++ )
  {
    if ( bt_da[ i ].bt_app_id == bt_app_id )
    {
      da_ptr = &bt_da[ i ];
    }
    else if ( (free_da_ptr == NULL) &&
              (bt_da[ i ].bt_app_id == BT_APP_ID_NULL) )
    {
      free_da_ptr = &bt_da[ i ];
    }
  }

  if ( da_ptr == NULL )
  {
    if ( can_be_new != FALSE )
    {
      if ( free_da_ptr != NULL )
      {
        da_ptr = free_da_ptr;
        da_ptr->bt_app_id = bt_app_id;
      }
      else
      {
        status = BT_CS_SD_MAX_DISCOVERABLE_APPLICATIONS;
      }
    }
    else
    {
      status = BT_CS_GN_BAD_APP_ID;
    }
  }

  *da_ptr_ptr = da_ptr;
  
  return ( status );

}


/*===========================================================================

FUNCTION
  bt_sd_set_discoverable_mode

DESCRIPTION
  Sets the appropriate discoverable mode based on the settings
  for all discoverable applications.

===========================================================================*/
LOCAL bt_cmd_status_type bt_sd_set_discoverable_mode
(
  void
)
{

  uint16                                i;
  bt_sd_service_discoverable_mode_type  cur_sd_mode;
  bt_rm_discoverable_type               rm_discoverable;
  bt_cmd_status_type                    cmd_status;

  cur_sd_mode = BT_SD_SERVICE_DISCOVERABLE_MODE_NONE;

  for ( i = 0; i < BT_SD_MAX_DISCOVERABLE_APPS; i++ )
  {
    if ( (bt_da[ i ].bt_app_id != BT_APP_ID_NULL) &&
         (bt_da[ i ].discoverable_mode > cur_sd_mode ) )
    {
      cur_sd_mode = bt_da[ i ].discoverable_mode;
    }
  }

  switch ( cur_sd_mode )
  {
    case BT_SD_SERVICE_DISCOVERABLE_MODE_NONE:
      {
        rm_discoverable = BT_RM_DISCOVERABLE_NOT;
        break;
      }
    
    case BT_SD_SERVICE_DISCOVERABLE_MODE_GENERAL:
      {
        rm_discoverable = BT_RM_DISCOVERABLE_GENERAL;
        break;
      }

    case BT_SD_SERVICE_DISCOVERABLE_MODE_LIMITED:
      {
        rm_discoverable = BT_RM_DISCOVERABLE_LIMITED;
        break;
      }

    default:
      {
        BT_ERR( "BT SD:  Bad SD Discv Mode %x", cur_sd_mode, 0, 0 );
        rm_discoverable = BT_RM_DISCOVERABLE_NOT;
        break;
      }
  }

  BT_MSG_API( "BT SD CMD TX: RM SetDiscv %x SD DM %x AID %x",
              rm_discoverable, cur_sd_mode, bt_sd_app_id );
  if ( (cmd_status = bt_cmd_rm_set_discoverable(
                       bt_sd_app_id, rm_discoverable,
                       BT_RM_AVP_AUTOMATIC )) == BT_CS_GN_PENDING )
  {
    cmd_status = BT_CS_GN_SUCCESS;
  }

  return ( cmd_status );

}


/*===========================================================================

FUNCTION
  bt_sd_set_service_discoverable_mode

DESCRIPTION
  Process a SD Set Service Discoverable Mode command.
  
============================================================================*/
LOCAL void bt_sd_set_service_discoverable_mode
(
  bt_cmd_msg_type*  ssdm_ptr
)
{
  
  bt_sd_da_type*                        da_ptr;
  bt_sd_service_discoverable_mode_type  req_sd_mode;

  req_sd_mode = ssdm_ptr->cmd_msg.cmd_sd_set_srv_discv_mode.srv_discv_mode;
  
  if ( (req_sd_mode == BT_SD_SERVICE_DISCOVERABLE_MODE_NONE) ||
       (req_sd_mode == BT_SD_SERVICE_DISCOVERABLE_MODE_GENERAL) ||
       (req_sd_mode == BT_SD_SERVICE_DISCOVERABLE_MODE_LIMITED) )
  {
    if ( (ssdm_ptr->cmd_hdr.cmd_status =
            bt_sd_get_discoverable_app(
              TRUE,  /*  Can be a new application.  */
              ssdm_ptr->cmd_hdr.bt_app_id,
              &da_ptr )) == BT_CS_GN_SUCCESS )
    {
      if ( req_sd_mode != BT_SD_SERVICE_DISCOVERABLE_MODE_NONE )
      {
        da_ptr->discoverable_mode = req_sd_mode;
      }
      else
      {
        bt_sd_init_discoverable_app( da_ptr );
      }

      ssdm_ptr->cmd_hdr.cmd_status = bt_sd_set_discoverable_mode();
    }
  }
  else
  {
    ssdm_ptr->cmd_hdr.cmd_status = BT_CS_SD_BAD_PARAM;
  }

}


/*===========================================================================

FUNCTION
  bt_sd_get_service_discoverable_mode

DESCRIPTION
  Process a SD Get Discoverable Mode command. 
  This is a direct function call and is not a command
  
============================================================================*/
bt_sd_service_discoverable_mode_type bt_sd_get_service_discoverable_mode
(
  void
)
{
  uint16                                i;
  bt_sd_service_discoverable_mode_type  cur_sd_mode = 
     BT_SD_SERVICE_DISCOVERABLE_MODE_NONE;

  TASKLOCK();
  for ( i = 0; i < BT_SD_MAX_DISCOVERABLE_APPS; i++ )
  {
    if ( (bt_da[ i ].bt_app_id != BT_APP_ID_NULL) &&
         (bt_da[ i ].discoverable_mode > cur_sd_mode ) )
    {
      cur_sd_mode = bt_da[ i ].discoverable_mode;
    }
  }
  TASKFREE();

  return ( cur_sd_mode );
}


/*===========================================================================

FUNCTION
  bt_sd_get_discoverable_mode

DESCRIPTION
  Process a SD Get Discoverable Mode command.
  
============================================================================*/
LOCAL void bt_sd_get_discoverable_mode
(
  bt_cmd_msg_type*  gdm_ptr
)
{
  bt_ev_msg_type                        ev_discv_mode_resp;

  gdm_ptr->cmd_hdr.cmd_status = BT_CS_GN_SUCCESS;

  ev_discv_mode_resp.ev_hdr.ev_type = BT_EV_SD_DISCOVERABLE_MODE_RESP;
  ev_discv_mode_resp.ev_hdr.bt_app_id = gdm_ptr->cmd_hdr.bt_app_id;
  ev_discv_mode_resp.ev_msg.ev_sd_discv_mode_resp.discv_mode = 
    bt_sd_get_service_discoverable_mode();

  bt_ec_send_event( &ev_discv_mode_resp );

}


/*===========================================================================

FUNCTION
  bt_sd_start_device_discovery

DESCRIPTION
  Process the command BT_CMD_SD_START_DEVICE_DISCOVERY
============================================================================*/

LOCAL void bt_sd_start_device_discovery(
  bt_cmd_msg_type*   cmd_msg_ptr
)
{
  uint16       max_num_resp;
  bt_iac_type  iac;
  uint8        duration_secs;

  /* Make sure we are not already doing device discovery */
  if ( bt_sd_dev_discv_db.dev_discv_in_progress )
  {
    BT_MSG_HIGH( "BT SD: Device discovery already in progress", 0, 0, 0 );

    cmd_msg_ptr->cmd_hdr.cmd_status =
      BT_CS_SD_DEVICE_DISCOVERY_ALREADY_IN_PROGRESS;

    return;
  }

  /* Okay to proceed */
  cmd_msg_ptr->cmd_hdr.cmd_status = BT_CS_GN_SUCCESS;

  /* Mark device discovery in progress */
  bt_sd_dev_discv_db.dev_discv_in_progress = TRUE;

  /* Save max_num_response */
  max_num_resp = cmd_msg_ptr->cmd_msg.cmd_sd_start_dev_discv.max_num_response;
  bt_sd_dev_discv_db.max_discvd_dev =
    ( max_num_resp > BT_SD_MAX_NUM_OF_DISCOVERED_DEVICE ) ?
        BT_SD_MAX_NUM_OF_DISCOVERED_DEVICE : max_num_resp;
  bt_sd_dev_discv_db.num_discvd_dev = 0;

  /* Save app ID */
  bt_sd_dev_discv_db.app_id = cmd_msg_ptr->cmd_hdr.bt_app_id;

  if (cmd_msg_ptr->cmd_hdr.cmd_type == BT_CMD_SD_DISCOVER_DEVICES)
  {
    /* Save BD address by which to filter; for now we only filter by the LSB */
    if ( cmd_msg_ptr->cmd_msg.cmd_sd_start_dev_discv.service_class_bitmap == 
          BT_SERVICE_CLASS_UNKNOWN ) 
    {
      bt_sd_dev_discv_db.bd_addr_2_filter = 
        cmd_msg_ptr->cmd_msg.cmd_sd_discv_dev.bd_addr;
    }
  }

  /* Save service class bitmap */
  bt_sd_dev_discv_db.service_class_bitmap =
    cmd_msg_ptr->cmd_msg.cmd_sd_start_dev_discv.service_class_bitmap;

  /* Make sure it is in the range 1-30 */
  duration_secs = cmd_msg_ptr->cmd_msg.cmd_sd_start_dev_discv.duration_secs;
  if( duration_secs > BT_SD_MAX_SEARCH_DURATION || 
      duration_secs < BT_SD_MIN_SEARCH_DURATION )
  {
    duration_secs = BT_SD_DEFAULT_SEARCH_DURATION;
    BT_MSG_DEBUG( "BT SD: Dev Disc duration: %u", duration_secs, 0, 0 );
  }

  /*  Instruct RM to start inquiry on GIAC.  */
  iac.iac_bytes[ 0 ] = BT_GIAC_0;
  iac.iac_bytes[ 1 ] = BT_IAC_1;
  iac.iac_bytes[ 2 ] = BT_IAC_2;
  bt_cmd_rm_inquire( bt_sd_app_id, iac, duration_secs, 0, 0 );
}


/*===========================================================================

FUNCTION
  bt_sd_stop_service_search

DESCRIPTION
  Process the command BT_CMD_SD_STOP_SERVICE_SEARCH
============================================================================*/

LOCAL void bt_sd_stop_service_search(
  bt_cmd_msg_type*   cmd_msg_ptr
)
{
  bt_sd_conn_entry_type*         conn_entry_ptr;

  cmd_msg_ptr->cmd_hdr.cmd_status = BT_CS_SD_BAD_PARAM;

  conn_entry_ptr = bt_sd_find_conn_entry_by_bd_addr(
                     &( cmd_msg_ptr->cmd_msg.cmd_sd_stop_srv_srch.bd_addr ) );

  if ( conn_entry_ptr == 0 )
  {
    BT_MSG_DEBUG( "BT SD: Stop Srv Srch: No Conn", 0, 0, 0 );
  }
  else
  {
    cmd_msg_ptr->cmd_hdr.cmd_status = BT_CS_GN_SUCCESS;

    bt_sd_clear_pending_request( conn_entry_ptr );
    bt_cmd_l2_disconnect( bt_sd_app_id,
                          conn_entry_ptr->cid_loc_initd );
  }
}


/*===========================================================================

FUNCTION
  bt_sd_stop_device_discovery

DESCRIPTION
  Process the command BT_CMD_SD_STOP_DEVICE_DISCOVERY
============================================================================*/

LOCAL void bt_sd_stop_device_discovery(
  bt_cmd_msg_type*   cmd_msg_ptr
)
{
  /* Make sure we are doing device discovery */
  if ( ! bt_sd_dev_discv_db.dev_discv_in_progress )
  {
    BT_MSG_HIGH( "BT SD: Device discovery not started", 0, 0, 0 );

    cmd_msg_ptr->cmd_hdr.cmd_status =
      BT_CS_SD_DEVICE_DISCOVERY_NOT_STARTED;

    return;
  }

  /* Okay to proceed */
  cmd_msg_ptr->cmd_hdr.cmd_status = BT_CS_GN_SUCCESS;

  /* Mark device discovery stopped */
  bt_sd_dev_discv_db.dev_discv_in_progress = FALSE;

  /* Instruct RM to stop inquiry */
  bt_cmd_rm_stop_inquiry( bt_sd_app_id );
}


/*===========================================================================

FUNCTION
  bt_sd_get_device_name

DESCRIPTION
  Process the command BT_CMD_SD_GET_DEVICE_NAME
============================================================================*/

LOCAL void bt_sd_get_device_name(
  bt_cmd_msg_type*   cmd_msg_ptr
)
{
  /* Make sure we are not already requesting for remote device name */
  if ( bt_sd_dev_discv_db.remote_name_req_in_progress )
  {
    BT_MSG_HIGH( "BT SD: Remote name request already in progress", 0, 0, 0 );

    cmd_msg_ptr->cmd_hdr.cmd_status =
      BT_CS_SD_REMOTE_NAME_REQ_ALREADY_IN_PROGRESS;

    return;
  }

  /* Okay to proceed */
  cmd_msg_ptr->cmd_hdr.cmd_status = BT_CS_GN_SUCCESS;

  /* Mark remote name request in progress */
  bt_sd_dev_discv_db.remote_name_req_in_progress = TRUE;

  /* Save app ID */
  bt_sd_dev_discv_db.app_id = cmd_msg_ptr->cmd_hdr.bt_app_id;

  /* Save remote device BD address */
  bt_sd_dev_discv_db.remote_name_bd_addr = 
    cmd_msg_ptr->cmd_msg.cmd_sd_get_dev_name.bd_addr;

  /* Instruct RM to perform remote name request */
  bt_cmd_rm_remote_name_req(
    bt_sd_app_id,
    &( cmd_msg_ptr->cmd_msg.cmd_sd_get_dev_name.bd_addr )
  );
}

/*===========================================================================

FUNCTION
  bt_sd_get_device_name_cancel

DESCRIPTION
  Process the command BT_CMD_SD_GET_DEVICE_NAME_CANCEL
============================================================================*/

LOCAL void bt_sd_get_device_name_cancel(
  bt_cmd_msg_type*   cmd_msg_ptr
)
{
  /* Make sure we are requesting for remote device name */
  if ( !bt_sd_dev_discv_db.remote_name_req_in_progress )
  {
    BT_MSG_HIGH( "BT SD: Remote name request not in progress", 0, 0, 0 );

    cmd_msg_ptr->cmd_hdr.cmd_status =
      BT_CS_SD_REMOTE_NAME_REQ_NOT_IN_PROGRESS;

    return;
  }

  /* Okay to proceed */
  cmd_msg_ptr->cmd_hdr.cmd_status = BT_CS_GN_SUCCESS;

  /* Instruct RM to cancel remote name request */
  bt_cmd_rm_remote_name_req_cancel(
    bt_sd_app_id,
    &( cmd_msg_ptr->cmd_msg.cmd_sd_get_dev_name_cancel.bd_addr )
  );
}

/*===========================================================================

FUNCTION
  bt_sd_search_service

DESCRIPTION
  Process the command BT_CMD_SD_SEARCH_SERVICE
============================================================================*/

LOCAL void bt_sd_search_service(
  bt_cmd_msg_type*   cmd_msg_ptr
)
{
  dsm_item_type*                 dsm_ptr;
  dsm_item_type*                 uuid_dsm_ptr;
  bt_sd_pdu_hdr_type             pdu_hdr;
  bt_sd_srv_srch_pat_type*       srch_pat_ptr;
  int16                          i;
  uint16                         len, uint16_to_send;
  uint32                         uint32_to_send;
  bt_sd_conn_entry_type*         conn_entry_ptr;
  bt_ev_msg_type                 ev_err_resp;
  boolean                        retry_cmd = FALSE;
  uint8                          temp;

  cmd_msg_ptr->cmd_hdr.cmd_status = BT_CS_GN_SUCCESS;

  conn_entry_ptr = bt_sd_find_conn_entry_by_bd_addr(
                     &( cmd_msg_ptr->cmd_msg.cmd_sd_srv_srch.bd_addr ) );

  /* New connection */
  if ( conn_entry_ptr == 0 )
  {
    /* Tear down all locally initiated idle connections */
    for ( i = 0; i < BT_SD_MAX_NUM_OF_L2CAP_CONNECTION; i++ )
    {
      if ( ( bt_sd_conn_table[i].conn_state == BT_SD_CONN_STATE_CONNECTED ) &&
           ( bt_sd_conn_table[i].req_pending != TRUE ) && /* Idle */
           ( bt_sd_conn_table[i].cid_loc_initd != BT_L2_NULL_CID ) )
      {
        retry_cmd = TRUE;

        /* Clear the Idle Timer */
        rex_clr_timer( &bt_sd_conn_table[i].tmr_loc_initd );

        bt_sd_conn_table[i].conn_state = BT_SD_CONN_STATE_DISCONNECTING;

        bt_cmd_l2_disconn_immediate( bt_sd_app_id,
                                     bt_sd_conn_table[i].cid_loc_initd );
      }
    }

    if ( retry_cmd )
    {
      if ( BT_CHECK_RETRY_EXPIRED( cmd_msg_ptr->cmd_hdr.cmd_retries ) ) 
      {
        BT_MSG_HIGH("BT SD EV TX: Max Retry exceeded", 0, 0, 0);
        ev_err_resp.ev_hdr.ev_type = BT_EV_SD_ERROR_RESP;
        ev_err_resp.ev_hdr.bt_app_id = cmd_msg_ptr->cmd_hdr.bt_app_id;
        ev_err_resp.ev_msg.ev_sd_error_resp.bd_addr =
          cmd_msg_ptr->cmd_msg.cmd_sd_srv_srch.bd_addr;
        ev_err_resp.ev_msg.ev_sd_error_resp.error_code =
          BT_SD_EV_ERROR_INSUFFICIENT_RESOURCE;

        bt_ec_send_event( &ev_err_resp );
      }
      else
      {
        BT_MSG_HIGH("BT SD EV TX: SD Serv Search Cmd ReQ'd", 0, 0, 0);
        cmd_msg_ptr->cmd_hdr.cmd_status = BT_CS_GN_RETRY_CMD_LATER;
      }

      return;
    }
    else
    {
      if ( ( conn_entry_ptr =
             bt_sd_add_conn_entry(
               &( cmd_msg_ptr->cmd_msg.cmd_sd_srv_srch.bd_addr ) ) ) == 0 )
      {
        BT_MSG_SIG( "BT SD EV TX: SD Err Resp EC %x AID %x",
                    BT_SD_EV_ERROR_MAX_SDP_CONN_EXCEEDED,
                    cmd_msg_ptr->cmd_hdr.bt_app_id,
                    0 );

        ev_err_resp.ev_hdr.ev_type = BT_EV_SD_ERROR_RESP;
        ev_err_resp.ev_hdr.bt_app_id = cmd_msg_ptr->cmd_hdr.bt_app_id;
        ev_err_resp.ev_msg.ev_sd_error_resp.bd_addr =
          cmd_msg_ptr->cmd_msg.cmd_sd_srv_srch.bd_addr;
        ev_err_resp.ev_msg.ev_sd_error_resp.error_code =
          BT_SD_EV_ERROR_MAX_SDP_CONN_EXCEEDED;

        bt_ec_send_event( &ev_err_resp );

        return;
      }
    }
  }
  else /* Existing connection */
  {
    /* Check connection state */
    if ( ( conn_entry_ptr->conn_state == BT_SD_CONN_STATE_CONNECTING ) ||
         ( conn_entry_ptr->conn_state == BT_SD_CONN_STATE_DISCONNECTING ) ||
         ( conn_entry_ptr->req_pending ) )
    {
      if ( BT_CHECK_RETRY_EXPIRED( cmd_msg_ptr->cmd_hdr.cmd_retries ) ) 
      {
        BT_MSG_HIGH("BT SD EV TX: Max Retry exceeded", 0, 0, 0);
        ev_err_resp.ev_hdr.ev_type = BT_EV_SD_ERROR_RESP;
        ev_err_resp.ev_hdr.bt_app_id = cmd_msg_ptr->cmd_hdr.bt_app_id;
        ev_err_resp.ev_msg.ev_sd_error_resp.bd_addr =
          cmd_msg_ptr->cmd_msg.cmd_sd_srv_srch.bd_addr;
        ev_err_resp.ev_msg.ev_sd_error_resp.error_code =
          BT_SD_EV_ERROR_INSUFFICIENT_RESOURCE;

        bt_ec_send_event( &ev_err_resp );
      }
      else
      {
        cmd_msg_ptr->cmd_hdr.cmd_status = BT_CS_GN_RETRY_CMD_LATER;
      }
      return;
    }
  }

  if ( ( dsm_ptr = bt_get_free_dsm_ptr( BT_TL_L2CAP,
                                        BT_SD_MAX_PDU_LEN ) ) == NULL )
  {
    BT_MSG_HIGH( "BT SD: Unable to get DSM buffer", 0, 0, 0 );
    bt_sd_send_error_resp_event(
      conn_entry_ptr,
      BT_SD_EV_ERROR_INSUFFICIENT_RESOURCE
    );
    return;
  }

  conn_entry_ptr->bt_app_id = cmd_msg_ptr->cmd_hdr.bt_app_id;

  /* Generate XML flag */
  conn_entry_ptr->generate_xml = cmd_msg_ptr->cmd_msg.cmd_sd_srv_srch.generate_xml;

  srch_pat_ptr = &( cmd_msg_ptr->cmd_msg.cmd_sd_srv_srch.srv_srch_pat );

  /* Save space for the PDU header */
  pdu_hdr.pid = srch_pat_ptr->pdu_type;
  pdu_hdr.tid = SWAB16( conn_entry_ptr->tid );
  bt_dsm_pushdown_tail( &dsm_ptr, NULL, sizeof(pdu_hdr),
                        DSM_DS_SMALL_ITEM_POOL );

  if ( ( srch_pat_ptr->pdu_type == BT_SD_PDU_SERVICE_SEARCH_REQ ) ||
       ( srch_pat_ptr->pdu_type == BT_SD_PDU_SERVICE_SEARCH_ATTRIBUTE_REQ ) )
  {
    /* Insert ServiceSearchPattern */
    temp = ( BT_SD_DATA_ELEMENT_SEQUENCE << 3 ) | 5;
    bt_dsm_pushdown_tail( &dsm_ptr, &temp, sizeof(temp), 
                          DSM_DS_SMALL_ITEM_POOL );

    len =
      ( srch_pat_ptr->num_uuid128 * ( sizeof( bt_sd_uuid128_type ) + 1 ) ) +
                                        /* add 1 for data element descriptor */
      ( srch_pat_ptr->num_uuid * 3 );
    bt_dsm_pushdown_tail( &dsm_ptr, ((uint8*)&len), 1, 
                          DSM_DS_SMALL_ITEM_POOL );

    for ( i = 0; i < srch_pat_ptr->num_uuid128; i++ )
    {
      temp = ( BT_SD_DATA_ELEMENT_UUID << 3 ) | 4; /* size = 4 => 16 bytes */
      bt_dsm_pushdown_tail(
        &dsm_ptr,
        &temp,
        sizeof(temp),
        DSM_DS_SMALL_ITEM_POOL );
      bt_dsm_pushdown_tail(
        &dsm_ptr,
        ( uint8* ) &srch_pat_ptr->uuid128[i],
        sizeof( bt_sd_uuid128_type ),
        DSM_DS_SMALL_ITEM_POOL );
    }
    
    for ( i = 0; i < srch_pat_ptr->num_uuid; i++ )
    {
      temp = ( BT_SD_DATA_ELEMENT_UUID << 3 ) | 1;
      bt_dsm_pushdown_tail( &dsm_ptr, &temp, sizeof(temp), DSM_DS_SMALL_ITEM_POOL );
      uint16_to_send = SWAB16( srch_pat_ptr->uuid[i] );
      bt_dsm_pushdown_tail( &dsm_ptr, &uint16_to_send, 
                            sizeof(uint16_to_send),
                            DSM_DS_SMALL_ITEM_POOL );
    }

    if ( srch_pat_ptr->pdu_type == BT_SD_PDU_SERVICE_SEARCH_REQ )
    {
      /* Insert MaxServiceRecordCount */
      uint16_to_send = SWAB16( srch_pat_ptr->max_srv_rec_count );
      bt_dsm_pushdown_tail( &dsm_ptr, &uint16_to_send, 
                            sizeof(uint16_to_send),
                            DSM_DS_SMALL_ITEM_POOL );
    }
  }

  if ( ( srch_pat_ptr->pdu_type == BT_SD_PDU_SERVICE_ATTRIBUTE_REQ ) ||
       ( srch_pat_ptr->pdu_type == BT_SD_PDU_SERVICE_SEARCH_ATTRIBUTE_REQ ) )
  {
    if ( srch_pat_ptr->pdu_type == BT_SD_PDU_SERVICE_ATTRIBUTE_REQ )
    {
      /* Insert ServiceRecordHandle */
      uint32_to_send = SWAB32( srch_pat_ptr->srv_rec_handle );
      bt_dsm_pushdown_tail( &dsm_ptr, &uint32_to_send, 
                            sizeof(uint32_to_send),
                            DSM_DS_SMALL_ITEM_POOL );
    }

    /* Insert MaximumAttributeByteCount */
    uint16_to_send = SWAB16( srch_pat_ptr->max_attr_byte_count );
    bt_dsm_pushdown_tail( &dsm_ptr, &uint16_to_send, 
                          sizeof(uint16_to_send),
                          DSM_DS_SMALL_ITEM_POOL );

    /* Insert AttributeIDList */
    temp = ( BT_SD_DATA_ELEMENT_SEQUENCE << 3 ) | 5;
    bt_dsm_pushdown_tail( &dsm_ptr, &temp, sizeof(temp), 
                          DSM_DS_SMALL_ITEM_POOL );
  
    uuid_dsm_ptr = bt_get_free_dsm_ptr( BT_TL_HCI_BB, 30 );

    len = 0;
    for ( i = 0; i < srch_pat_ptr->num_attr_id; i++ )
    {
      if ( srch_pat_ptr->attr_id[i].is_range )
      {
        /* 4 bytes */
        temp = ( BT_SD_DATA_ELEMENT_UNSIGNED_INT << 3 ) | 2;
        bt_dsm_pushdown_tail( &uuid_dsm_ptr, &temp, sizeof(temp), 
                              DSM_DS_SMALL_ITEM_POOL );
        uint32_to_send = SWAB32( srch_pat_ptr->attr_id[i].value );
        bt_dsm_pushdown_tail( &uuid_dsm_ptr, &uint32_to_send, 
                              sizeof(uint32_to_send),
                              DSM_DS_SMALL_ITEM_POOL );
        len += 5;
      }
      else
      {
        /* 2 bytes */
        temp = ( BT_SD_DATA_ELEMENT_UNSIGNED_INT << 3 ) | 1;
        bt_dsm_pushdown_tail( &uuid_dsm_ptr, &temp, sizeof(temp), 
                              DSM_DS_SMALL_ITEM_POOL );
        uint16_to_send = SWAB16( srch_pat_ptr->attr_id[i].value );
        bt_dsm_pushdown_tail( &uuid_dsm_ptr, &uint16_to_send, 
                              sizeof(uint16_to_send),
                              DSM_DS_SMALL_ITEM_POOL );
        len += 3;
      }
    }
    bt_dsm_pushdown_tail( &dsm_ptr, ((uint8*)&len), 1,
                          DSM_DS_SMALL_ITEM_POOL );
    dsm_append( &dsm_ptr, &uuid_dsm_ptr );
  }
  
  /* Pull up the space saved for the header earlier */
  dsm_pullup( &dsm_ptr, NULL, sizeof(pdu_hdr) );
  
  pdu_hdr.length = dsm_length_packet( dsm_ptr );
  pdu_hdr.length = SWAB16( pdu_hdr.length );
  bt_dsm_pushdown_packed( &dsm_ptr, (void*)&pdu_hdr, sizeof(pdu_hdr),
                          DSM_DS_SMALL_ITEM_POOL );

  conn_entry_ptr->req_pending = TRUE;
  conn_entry_ptr->pending_req_dsm_ptr = dsm_ptr;
  conn_entry_ptr->num_resp_recv = 0;
  conn_entry_ptr->same_cont_state_count = 0;
  memset(conn_entry_ptr->cont_state_info,0,BT_SD_MAX_CONTINUATION_STATE_INFO_LEN);


  /* Duplicate the request just in case it needs to be sent again
   * upon receipt of response with continuation
   */
  dsm_dup_packet( &conn_entry_ptr->dup_req_dsm_ptr, 
                  conn_entry_ptr->pending_req_dsm_ptr, 
                  0,
                  dsm_length_packet( conn_entry_ptr->pending_req_dsm_ptr ) );

  if ( ( conn_entry_ptr->conn_state == BT_SD_CONN_STATE_CONNECTED ) &&
       ( conn_entry_ptr->cid_loc_initd != BT_L2_NULL_CID ) )
  {
    bt_sd_send_pending_request( conn_entry_ptr, 0, 0 );
  }
  else
  {
    conn_entry_ptr->conn_state = BT_SD_CONN_STATE_CONNECTING;

    /* Establish L2CAP connection first */
    bt_cmd_l2_connect_immediate_cid(
      bt_sd_app_id,
      BT_L2_PSM_SDP,
      &( conn_entry_ptr->bd_addr ),
      &( conn_entry_ptr->cid_loc_initd ) );

    BT_MSG_API( "BT SD CMD TX: L2 Conn- AID %x CID %x",
                bt_sd_app_id, conn_entry_ptr->cid_loc_initd, 0 );
  }
}

/*===========================================================================

FUNCTION
  bt_sd_cancel_idle_timer

DESCRIPTION
  Cancels local initiated idle timer and disconnects L2cap channels

===========================================================================*/
LOCAL void bt_sd_cancel_idle_timer( 
  bt_cmd_msg_type*   cmd_msg_ptr
)
{
  bt_sd_conn_entry_type*  conn_entry_ptr;
  
  cmd_msg_ptr->cmd_hdr.cmd_status = BT_CS_GN_SUCCESS;

  conn_entry_ptr = bt_sd_find_conn_entry_by_bd_addr( 
    &( cmd_msg_ptr->cmd_msg.cmd_sd_cancel_idle_timer.bd_addr ) );

  if(conn_entry_ptr == NULL)
  {
    cmd_msg_ptr->cmd_hdr.cmd_status = BT_CS_SD_SERVICE_RECORD_NOT_FOUND;
    return;
  }
  
  if ( conn_entry_ptr->used &&
       (conn_entry_ptr->conn_state == BT_SD_CONN_STATE_CONNECTED )&&
       ( conn_entry_ptr->cid_loc_initd != BT_L2_NULL_CID ))
  {
    conn_entry_ptr->conn_state = BT_SD_CONN_STATE_DISCONNECTING;

    BT_MSG_DEBUG( "BT SD: Clear Idle Timer Local CID %x",
                    conn_entry_ptr->cid_loc_initd, 0, 0 );

    /* Clear the Idle Timer */
    rex_clr_timer( &conn_entry_ptr->tmr_loc_initd );

    BT_MSG_API( "BT SD CMD TX: L2 Disc CID %x",
                  conn_entry_ptr->cid_loc_initd, 0, 0 );
    bt_cmd_l2_disconnect( bt_sd_app_id,
                          conn_entry_ptr->cid_loc_initd );
  }
}

/*===========================================================================

FUNCTION
  bt_sd_get_server_channel_number

DESCRIPTION
  Process the command BT_CMD_SD_GET_SERVER_CHANNEL_NUMBER
============================================================================*/

LOCAL void bt_sd_get_server_channel_number(
  bt_cmd_msg_type*   cmd_msg_ptr
)
{
  bt_sd_srch_pat_uuid_list_type     uuid_list;
  bt_sd_srch_pat_attr_id_list_type  attr_id_list;
  bt_ev_msg_type                    ev_err_resp;

  cmd_msg_ptr->cmd_hdr.cmd_status = BT_CS_GN_SUCCESS;

  /* Make sure there is no pending request */
  if ( bt_sd_data.app_id_user == BT_APP_ID_NULL )
  {
    /* Save application ID */
    bt_sd_data.app_id_user = cmd_msg_ptr->cmd_hdr.bt_app_id;

    /* Save the application-level command that generates the SDP request */
    bt_sd_data.app_cmd = cmd_msg_ptr->cmd_hdr.cmd_type;

    /* Save the service class */
    bt_sd_data.service_class =
      cmd_msg_ptr->cmd_msg.cmd_sd_get_scn.service_class;

    /* Do SDP Service Search */
    uuid_list.num_uuid128 = 0;
    uuid_list.num_uuid = 1;
    uuid_list.uuid[0] = cmd_msg_ptr->cmd_msg.cmd_sd_get_scn.service_class;
    attr_id_list.num_attr_id = 1;
    attr_id_list.attr_id[0].is_range = FALSE;
    if ( bt_sd_data.app_cmd == BT_CMD_SD_GET_SECONDARY_SERVER_CHANNEL_NUMBER )
    {                                                             
      attr_id_list.attr_id[0].value = 
        BT_SD_ATTR_ID_ADDITIONAL_PROTOCOL_DESCRIPTOR_LISTS;
    }
    else
    {
      attr_id_list.attr_id[0].value = BT_SD_ATTR_ID_PROTOCOL_DESCRIPTOR_LIST;
    }

    BT_MSG_API( "BT SD CMD TX: SD Search Service - AID %x",
                bt_sd_app_id, 0, 0 );

    bt_cmd_sd_search_service(
      bt_sd_app_id,
      &( cmd_msg_ptr->cmd_msg.cmd_sd_get_scn.bd_addr ),
      &uuid_list,
      &attr_id_list,
      BT_SD_DEFAULT_MAX_ATTR_BYTE_COUNT );
  }
  else
  {
    ev_err_resp.ev_hdr.ev_type = BT_EV_SD_ERROR_RESP;
    ev_err_resp.ev_hdr.bt_app_id = cmd_msg_ptr->cmd_hdr.bt_app_id;
    ev_err_resp.ev_msg.ev_sd_error_resp.bd_addr =
      cmd_msg_ptr->cmd_msg.cmd_sd_get_scn.bd_addr;
    ev_err_resp.ev_msg.ev_sd_error_resp.error_code =
      BT_SD_EV_ERROR_REQUEST_PENDING;

    BT_MSG_API( "BT SD EV TX: SD Err Resp EC %x AID %x",
                ev_err_resp.ev_msg.ev_sd_error_resp.error_code,
                ev_err_resp.ev_hdr.bt_app_id,
                0 );

    bt_ec_send_event( &ev_err_resp );
  }
}

/*===========================================================================

FUNCTION
  bt_sd_update_service_record

DESCRIPTION
  Process the command BT_CMD_SD_UPDATE_SERVICE_RECORD,
  BT_CMD_SD_UPDATE_SRV_REC_BY_SCN, and BT_CMD_SD_UPDATE_SRV_REC_BY_PSM
============================================================================*/

LOCAL void bt_sd_update_service_record(
  bt_cmd_msg_type*   cmd_msg_ptr
)
{
  bt_sd_srv_rec_type*          srv_rec_ptr;
  bt_sd_srv_attr_rec_type*     srv_attr_ptr;
  bt_sd_proto_desc_list_type*  proto_desc_list_ptr;
  uint16                       i; 

  if ( ( cmd_msg_ptr->cmd_msg.cmd_sd_update_srv_rec.scn != 
         BT_RC_SCN_NOT_SPECIFIED ) &&
       ( cmd_msg_ptr->cmd_msg.cmd_sd_update_srv_rec.psm != 
         BT_L2_PSM_INVALID ) )
  {
    /* either SCN or PSM can be used but not both */
    cmd_msg_ptr->cmd_hdr.cmd_status = BT_CS_SD_BAD_PARAM;
    return;
  }

  cmd_msg_ptr->cmd_hdr.cmd_status = BT_CS_GN_SUCCESS;

  /* SCN and PSM not specified when using this command */
  srv_rec_ptr = bt_sd_find_service_record_by_uuid(
                  BT_SD_DB_TYPE_LOCAL_DB,
                  cmd_msg_ptr->cmd_msg.cmd_sd_update_srv_rec.uuid,
                  cmd_msg_ptr->cmd_msg.cmd_sd_update_srv_rec.scn,
                  cmd_msg_ptr->cmd_msg.cmd_sd_update_srv_rec.psm );

  if ( srv_rec_ptr != 0 )
  {
    /* Update Protocol Descriptor List */
    srv_attr_ptr = bt_sd_find_service_attribute(
                     srv_rec_ptr,
                     BT_SD_ATTR_ID_PROTOCOL_DESCRIPTOR_LIST );

    if ( srv_attr_ptr != 0 )
    {
      proto_desc_list_ptr = &( srv_attr_ptr->attr_value.proto_desc_list );
      for ( i = 0; i < proto_desc_list_ptr->num_proto_desc; i++ )
      {
        if ( cmd_msg_ptr->cmd_msg.cmd_sd_update_srv_rec.psm != 
                  BT_L2_PSM_INVALID )
        {
          if ( ( proto_desc_list_ptr->proto_desc[i].uuid == 
                 BT_SD_PROTOCOL_L2CAP ) &&
               ( cmd_msg_ptr->cmd_msg.cmd_sd_update_srv_rec.new_psm != 
                 BT_L2_PSM_INVALID ) )
          {
            BT_MSG_DEBUG( "BT SD: PSM updated %x",
                          cmd_msg_ptr->cmd_msg.cmd_sd_update_srv_rec.new_psm,
                          0, 0 );
            proto_desc_list_ptr->proto_desc[i].param[0].value =
              cmd_msg_ptr->cmd_msg.cmd_sd_update_srv_rec.new_psm;
            break;
          }
        }
        else
        {
          if ( ( proto_desc_list_ptr->proto_desc[i].uuid == 
                 BT_SD_PROTOCOL_RFCOMM ) && 
               ( cmd_msg_ptr->cmd_msg.cmd_sd_update_srv_rec.new_scn != 
                 BT_RC_SCN_NOT_SPECIFIED ) )
          { 
            BT_MSG_DEBUG( "BT SD: SCN updated %x",
                          cmd_msg_ptr->cmd_msg.cmd_sd_update_srv_rec.new_scn,
                          0, 0 );
            proto_desc_list_ptr->proto_desc[i].param[0].value =
              cmd_msg_ptr->cmd_msg.cmd_sd_update_srv_rec.new_scn;
            break;
          }
        }
      }
    }
  
    /* Update Service Name */
    srv_attr_ptr = bt_sd_find_service_attribute(
                     srv_rec_ptr,
                     BT_SD_ATTR_ID_SERVICE_NAME );

    if ( srv_attr_ptr != 0 )
    {
      memset( srv_attr_ptr->attr_value.str, 0, BT_SD_MAX_TEXT_STRING_LEN );
      bt_sd_service_attribute_set_str(
        srv_attr_ptr,
        (char *)cmd_msg_ptr->cmd_msg.cmd_sd_update_srv_rec.service_name_str );
    }
  }
  else
  {
    cmd_msg_ptr->cmd_hdr.cmd_status = BT_CS_SD_SERVICE_RECORD_NOT_FOUND;
  }
} 



/*===========================================================================

FUNCTION
  bt_sd_update_service_name

DESCRIPTION
  Process the command BT_CMD_SD_UPDATE_SRV_NAME
============================================================================*/

LOCAL void bt_sd_update_service_name(
  bt_cmd_msg_type*   cmd_msg_ptr
)
{
  bt_sd_srv_rec_type*          srv_rec_ptr;
  bt_sd_srv_attr_rec_type*     srv_attr_ptr;

  cmd_msg_ptr->cmd_hdr.cmd_status = BT_CS_GN_SUCCESS;

  /* SCN and PSM not specified when using this command */
  srv_rec_ptr = bt_sd_find_service_record_by_uuid(
                  BT_SD_DB_TYPE_LOCAL_DB,
                  cmd_msg_ptr->cmd_msg.cmd_sd_update_srv_rec.uuid,
                  BT_RC_SCN_NOT_SPECIFIED,
                  BT_L2_PSM_INVALID );

  if ( srv_rec_ptr != 0 )
  {
    /* Update Service Name */
    srv_attr_ptr = bt_sd_find_service_attribute(
                     srv_rec_ptr,
                     BT_SD_ATTR_ID_SERVICE_NAME );

    if ( srv_attr_ptr != 0 )
    {
      memset( srv_attr_ptr->attr_value.str, 0, BT_SD_MAX_TEXT_STRING_LEN );
      bt_sd_service_attribute_set_str(
        srv_attr_ptr,
        (char *)cmd_msg_ptr->cmd_msg.cmd_sd_update_srv_rec.service_name_str );
    }
  }
  else
  {
    cmd_msg_ptr->cmd_hdr.cmd_status = BT_CS_SD_SERVICE_RECORD_NOT_FOUND;
  }
} 

#ifdef FEATURE_BT_EXTPF
/*===========================================================================

FUNCTION
  bt_sd_update_obex_profile_service_record

DESCRIPTION
  Updates a service record for a OBEX-based profile server.

===========================================================================*/
LOCAL void bt_sd_update_obex_profile_service_record( 
  bt_cmd_msg_type*   cmd_msg_ptr
)
{
  bt_sd_srv_rec_type*         srv_rec_ptr;
  bt_sd_srv_attr_rec_type*    srv_attr_ptr;
  uint16                      i;
  bt_cmd_sd_update_obex_profile_service_rec_type* cmd_update_ptr;

  cmd_update_ptr = &cmd_msg_ptr->cmd_msg.cmd_sd_update_obex_srv_rec;

  cmd_msg_ptr->cmd_hdr.cmd_status = BT_CS_GN_SUCCESS;

  srv_rec_ptr = bt_sd_find_service_record_by_uuid( 
                  BT_SD_DB_TYPE_LOCAL_DB, 
                  cmd_update_ptr->uuid,
                  cmd_update_ptr->scn,
                  BT_L2_PSM_INVALID );

  if (srv_rec_ptr == NULL)
  {
    BT_ERR( "BT SD: obex srv rec not found, Uuid %x", 
            cmd_update_ptr->uuid, 0, 0 );
    cmd_msg_ptr->cmd_hdr.cmd_status = BT_CS_SD_SERVICE_RECORD_NOT_FOUND;
    return;
  }

  /* Add OBEX to protocol descriptor list */

  srv_attr_ptr = bt_sd_find_service_attribute(
                   srv_rec_ptr,
                   BT_SD_ATTR_ID_PROTOCOL_DESCRIPTOR_LIST );

  if (srv_attr_ptr == NULL)
  {
    BT_ERR( "BT SD: protocol desc list attr not found, Uuid %x", 
            cmd_update_ptr->uuid, 0, 0 );
    cmd_msg_ptr->cmd_hdr.cmd_status = BT_CS_SD_SERVICE_RECORD_NOT_FOUND;
    return;
  }

  bt_sd_service_attribute_add_proto_desc(
    srv_attr_ptr, BT_SD_PROTOCOL_OBEX, NULL );

  /* Add service name attribute */

  if ( cmd_update_ptr->service_name_str != NULL )
  {
    srv_attr_ptr = bt_sd_find_service_attribute(
                     srv_rec_ptr,
                     BT_SD_ATTR_ID_SERVICE_NAME );

    if (srv_attr_ptr == NULL)
    {
      BT_ERR( "BT SD: obex srv name attr not found, Uuid %x", 
              cmd_update_ptr->uuid, 0, 0 );
      cmd_msg_ptr->cmd_hdr.cmd_status = BT_CS_SD_SERVICE_RECORD_NOT_FOUND;
      return;
    }
  
    bt_sd_service_attribute_set_str( srv_attr_ptr, 
                                     (char *)cmd_update_ptr->service_name_str );
  }

  /* Add Service ID attribute */

  if ( !BT_SD_UUID128_EQUAL( &cmd_update_ptr->service_id, 
                             &bt_sd_null_uuid128 ) )
  {                           
    srv_attr_ptr = bt_sd_service_record_add_attribute(
                     srv_rec_ptr, BT_SD_ATTR_ID_SERVICE_SERVICE_ID,
                     BT_SD_ATTR_TYPE_UUID128 );
  
    if (srv_attr_ptr == NULL)
    {
      BT_ERR( "BT SD: obex srv add attr fail, Uuid %x", 
              cmd_update_ptr->uuid, 0, 0 );
      cmd_msg_ptr->cmd_hdr.cmd_status = BT_CS_SD_LOCAL_SERVICE_DB_FULL;
      return;
    }
    
    bt_sd_service_attribute_set_uuid128( srv_attr_ptr, 
                                         &cmd_update_ptr->service_id );
  }

  /* Add the Supported Formats List service attribute */

  if ( cmd_update_ptr->supported_formats_list.num_val != 0 )
  {                                       
    srv_attr_ptr = bt_sd_service_record_add_attribute(
                     srv_rec_ptr,
                     BT_SD_ATTR_ID_SUPPORTED_FORMATS_LIST,
                     BT_SD_ATTR_TYPE_UINT_LIST );
  
    if (srv_attr_ptr == NULL)
    {
      BT_ERR( "BT SD: obex srv add attr fail, Uuid %x", 
              cmd_update_ptr->uuid, 0, 0 );
      cmd_msg_ptr->cmd_hdr.cmd_status = BT_CS_SD_LOCAL_SERVICE_DB_FULL;
      return;
    }
    
    for (i = 0; i < cmd_update_ptr->supported_formats_list.num_val; i++)
    {
      bt_sd_service_attribute_uint_list_add_val(
        srv_attr_ptr,
        1, /* size = 1 byte */
        cmd_update_ptr->supported_formats_list.val[i] );
    }
  }

  /* Add the Supported Capabilities service attribute */
  /* For PBAP PSE, it corresponds to Supported Repositories */

  if ( cmd_update_ptr->supported_capabilities != 0 )
  {                     
    if ( cmd_update_ptr->uuid == BT_SD_SERVICE_CLASS_PBAP_PSE ) 
    {
      srv_attr_ptr = bt_sd_service_record_add_attribute(
                       srv_rec_ptr, 
                       BT_SD_ATTR_ID_SUPPORTED_REPOSITORIES,
                       BT_SD_ATTR_TYPE_UINT8 );
    }
    else
    {
      srv_attr_ptr = bt_sd_service_record_add_attribute(
                       srv_rec_ptr, 
                       BT_SD_ATTR_ID_SUPPORTED_CAPABILITIES,
                       BT_SD_ATTR_TYPE_UINT8 );
    }
  
    if (srv_attr_ptr == NULL)
    {
      BT_ERR( "BT SD: obex srv add attr fail, Uuid %x", 
              cmd_update_ptr->uuid, 0, 0 );
      cmd_msg_ptr->cmd_hdr.cmd_status = BT_CS_SD_LOCAL_SERVICE_DB_FULL;
      return;
    }

    bt_sd_service_attribute_set_value( srv_attr_ptr, 
                                       cmd_update_ptr->supported_capabilities );
  

  }
  
  /* Add the Supported Features service attribute */

  if ( cmd_update_ptr->supported_features != 0 )
  {                            
    srv_attr_ptr = bt_sd_service_record_add_attribute(
                     srv_rec_ptr, 
                     BT_SD_ATTR_ID_SUPPORTED_FEATURES, 
                     BT_SD_ATTR_TYPE_UINT16 );
  
    if (srv_attr_ptr == NULL)
    {
      BT_ERR( "BT SD: obex srv add attr fail, Uuid %x", 
              cmd_update_ptr->uuid, 0, 0 );
      cmd_msg_ptr->cmd_hdr.cmd_status = BT_CS_SD_LOCAL_SERVICE_DB_FULL;
      return;
    }

    bt_sd_service_attribute_set_value( srv_attr_ptr, 
                                       cmd_update_ptr->supported_features );
  }
  
  /* Add the Supported Functions service attribute */

  if ( cmd_update_ptr->supported_functions != 0 )
  {                             
    srv_attr_ptr = bt_sd_service_record_add_attribute( 
                    srv_rec_ptr, 
                    BT_SD_ATTR_ID_SUPPORTED_FUNCTIONS,
                    BT_SD_ATTR_TYPE_UINT32 );
  
    if (srv_attr_ptr == NULL)
    {
      BT_ERR( "BT SD: obex srv add attr fail, Uuid %x", 
              cmd_update_ptr->uuid, 0, 0 );
      cmd_msg_ptr->cmd_hdr.cmd_status = BT_CS_SD_LOCAL_SERVICE_DB_FULL;
      return;
    }
  
    bt_sd_service_attribute_set_value( srv_attr_ptr, 
                                       cmd_update_ptr->supported_functions );
  }
  
  /* Add the Total Imaging Data Capacity service attribute */

  if ( cmd_update_ptr->total_image_capacity != 0 )
  {
    srv_attr_ptr = bt_sd_service_record_add_attribute(
                     srv_rec_ptr, 
                     BT_SD_ATTR_ID_TOTAL_IMAGING_DATA_CAPACITY,
                     BT_SD_ATTR_TYPE_UINT64 );
  
    if (srv_attr_ptr == NULL)
    {
      BT_ERR( "BT SD: obex srv add attr fail, Uuid %x", 
              cmd_update_ptr->uuid, 0, 0 );
      cmd_msg_ptr->cmd_hdr.cmd_status = BT_CS_SD_LOCAL_SERVICE_DB_FULL;
      return;
    }
  
    bt_sd_service_attribute_set_value( srv_attr_ptr, 
                                       cmd_update_ptr->total_image_capacity );
  }
} 

/*===========================================================================

FUNCTION
  bt_sd_custom_update_obex_profile_service_record

DESCRIPTION
  Updates a service record for a OBEX-based profile 128-bit UUID server

===========================================================================*/
LOCAL void bt_sd_custom_update_obex_profile_service_record( 
     bt_cmd_msg_type*   cmd_msg_ptr
)
{
  bt_sd_srv_rec_type*         srv_rec_ptr;
  bt_sd_srv_attr_rec_type*    srv_attr_ptr;
  uint16                      i;
  bt_cmd_sd_update_obex_profile_service_rec_type* cmd_update_ptr;

  cmd_update_ptr = &cmd_msg_ptr->cmd_msg.cmd_sd_update_obex_srv_rec;

  cmd_msg_ptr->cmd_hdr.cmd_status = BT_CS_GN_SUCCESS;

  srv_rec_ptr = bt_sd_find_service_record_by_uuid128( 
                  BT_SD_DB_TYPE_LOCAL_DB, 
                  &cmd_update_ptr->uuid128,
                  cmd_update_ptr->scn,
                  BT_L2_PSM_INVALID );

  if (srv_rec_ptr == NULL)
  {
    BT_ERR( "BT SD: 128-bit UUID obex srv rec not found", 0, 0, 0 );
    cmd_msg_ptr->cmd_hdr.cmd_status = BT_CS_SD_SERVICE_RECORD_NOT_FOUND;
    return;
  }

  /* Add OBEX to protocol descriptor list */

  srv_attr_ptr = bt_sd_find_service_attribute(
                   srv_rec_ptr,
                   BT_SD_ATTR_ID_PROTOCOL_DESCRIPTOR_LIST );

  if (srv_attr_ptr == NULL)
  {
    BT_ERR( "BT SD: protocol desc list attr not found, 128-BitUuid",0,0,0);
    cmd_msg_ptr->cmd_hdr.cmd_status = BT_CS_SD_SERVICE_RECORD_NOT_FOUND;
    return;
  }

  bt_sd_service_attribute_add_proto_desc(
    srv_attr_ptr, BT_SD_PROTOCOL_OBEX, NULL );

  /* Add service name attribute */

  if ( cmd_update_ptr->service_name_str != NULL )
  {
    srv_attr_ptr = bt_sd_find_service_attribute(
                     srv_rec_ptr,
                     BT_SD_ATTR_ID_SERVICE_NAME );

    if (srv_attr_ptr == NULL)
    {
      BT_ERR( "BT SD: obex srv name attr not found, 128-Bit Uuid ",0,0,0 );
      cmd_msg_ptr->cmd_hdr.cmd_status = BT_CS_SD_SERVICE_RECORD_NOT_FOUND;
      return;
    }
  
    bt_sd_service_attribute_set_str( srv_attr_ptr, 
                                     (char *)cmd_update_ptr->service_name_str );
  }

  /* Add Service ID attribute */

  if ( !BT_SD_UUID128_EQUAL( &cmd_update_ptr->service_id, 
                             &bt_sd_null_uuid128 ) )
  {                           
    srv_attr_ptr = bt_sd_service_record_add_attribute(
                     srv_rec_ptr, BT_SD_ATTR_ID_SERVICE_SERVICE_ID,
                     BT_SD_ATTR_TYPE_UUID128 );
  
    if (srv_attr_ptr == NULL)
    {
      BT_ERR( "BT SD: obex srv add attr fail, 128-bit Uuid",0, 0, 0 );
      cmd_msg_ptr->cmd_hdr.cmd_status = BT_CS_SD_LOCAL_SERVICE_DB_FULL;
      return;
    }
    
    bt_sd_service_attribute_set_uuid128( srv_attr_ptr, 
                                         &cmd_update_ptr->service_id );
  }

  /* Add the Supported Formats List service attribute */

  if ( cmd_update_ptr->supported_formats_list.num_val != 0 )
  {                                       
    srv_attr_ptr = bt_sd_service_record_add_attribute(
                     srv_rec_ptr,
                     BT_SD_ATTR_ID_SUPPORTED_FORMATS_LIST,
                     BT_SD_ATTR_TYPE_UINT_LIST );
  
    if (srv_attr_ptr == NULL)
    {
      BT_ERR( "BT SD: obex srv add attr fail, 128-bit Uuid %x",0,0,0);
      cmd_msg_ptr->cmd_hdr.cmd_status = BT_CS_SD_LOCAL_SERVICE_DB_FULL;
      return;
    }
    
    for (i = 0; i < cmd_update_ptr->supported_formats_list.num_val; i++)
    {
      bt_sd_service_attribute_uint_list_add_val(
        srv_attr_ptr,
        1, /* size = 1 byte */
        cmd_update_ptr->supported_formats_list.val[i] );
    }
  }

  /* Add the Supported Capabilities service attribute */
  /* For PBAP PSE, it corresponds to Supported Repositories */

  if ( cmd_update_ptr->supported_capabilities != 0 )
  {                     
    if ( cmd_update_ptr->uuid == BT_SD_SERVICE_CLASS_PBAP_PSE ) 
    {
      srv_attr_ptr = bt_sd_service_record_add_attribute(
                       srv_rec_ptr, 
                       BT_SD_ATTR_ID_SUPPORTED_REPOSITORIES,
                       BT_SD_ATTR_TYPE_UINT8 );
    }
    else
    {
      srv_attr_ptr = bt_sd_service_record_add_attribute(
                       srv_rec_ptr, 
                       BT_SD_ATTR_ID_SUPPORTED_CAPABILITIES,
                       BT_SD_ATTR_TYPE_UINT8 );
    }
  
    if (srv_attr_ptr == NULL)
    {
      BT_ERR( "BT SD: obex srv add attr fail,128-bit UUID",0,0, 0 );
      cmd_msg_ptr->cmd_hdr.cmd_status = BT_CS_SD_LOCAL_SERVICE_DB_FULL;
      return;
    }

    bt_sd_service_attribute_set_value( srv_attr_ptr, 
                                       cmd_update_ptr->supported_capabilities );
  

  }
  
  /* Add the Supported Features service attribute */

  if ( cmd_update_ptr->supported_features != 0 )
  {                            
    srv_attr_ptr = bt_sd_service_record_add_attribute(
                     srv_rec_ptr, 
                     BT_SD_ATTR_ID_SUPPORTED_FEATURES, 
                     BT_SD_ATTR_TYPE_UINT16 );
  
    if (srv_attr_ptr == NULL)
    {
      BT_ERR( "BT SD: obex srv add attr fail,128-bit Uuid",0, 0, 0 );
      cmd_msg_ptr->cmd_hdr.cmd_status = BT_CS_SD_LOCAL_SERVICE_DB_FULL;
      return;
    }

    bt_sd_service_attribute_set_value( srv_attr_ptr, 
                                       cmd_update_ptr->supported_features );
  }
  
  /* Add the Supported Functions service attribute */

  if ( cmd_update_ptr->supported_functions != 0 )
  {                             
    srv_attr_ptr = bt_sd_service_record_add_attribute( 
                    srv_rec_ptr, 
                    BT_SD_ATTR_ID_SUPPORTED_FUNCTIONS,
                    BT_SD_ATTR_TYPE_UINT32 );
  
    if (srv_attr_ptr == NULL)
    {
      BT_ERR( "BT SD: obex srv add attr fail,128-bit Uuid",0,0,0);
      cmd_msg_ptr->cmd_hdr.cmd_status = BT_CS_SD_LOCAL_SERVICE_DB_FULL;
      return;
    }
  
    bt_sd_service_attribute_set_value( srv_attr_ptr, 
                                       cmd_update_ptr->supported_functions );
  }
  
  /* Add the Total Imaging Data Capacity service attribute */

  if ( cmd_update_ptr->total_image_capacity != 0 )
  {
    srv_attr_ptr = bt_sd_service_record_add_attribute(
                     srv_rec_ptr, 
                     BT_SD_ATTR_ID_TOTAL_IMAGING_DATA_CAPACITY,
                     BT_SD_ATTR_TYPE_UINT64 );
  
    if (srv_attr_ptr == NULL)
    {
      BT_ERR( "BT SD: obex srv add attr fail, 128-bit Uuid",0,0,0 );
      cmd_msg_ptr->cmd_hdr.cmd_status = BT_CS_SD_LOCAL_SERVICE_DB_FULL;
      return;
    }
  
    bt_sd_service_attribute_set_value( srv_attr_ptr, 
                                       cmd_update_ptr->total_image_capacity );
  }
} 

#endif /* FEATURE_BT_EXTPF */

/*===========================================================================

FUNCTION
  bt_sd_check_connectable_setting

DESCRIPTION
  This routine updates the connectable setting as necessary.
  
============================================================================*/
bt_cmd_status_type bt_sd_check_connectable_setting
(
  void
)
{

  boolean             cur_connectable = FALSE;
  bt_cmd_status_type  cmd_status = BT_CS_GN_SUCCESS;
  
  if ( (bt_sd_any_local_services_connectable() != FALSE) &&
       (bt_sd_any_connections_active() == FALSE) )
  {
    cur_connectable = TRUE;
  }

  if ( bt_sd_connectable != cur_connectable )
  {
    bt_sd_connectable = cur_connectable;

    BT_MSG_HIGH( "BT SD: Connectable %x", bt_sd_connectable, 0, 0 );
    BT_MSG_API( "BT SD CMD TX: RM SetConn %x AID %x",
                bt_sd_connectable, bt_sd_app_id, 0 );
    if ( (cmd_status =
            bt_cmd_rm_set_connectable(
              bt_sd_app_id, bt_sd_connectable,
              BT_RM_AVP_AUTOMATIC )) == BT_CS_GN_PENDING )
    {
      cmd_status = BT_CS_GN_SUCCESS;
    }
  }

  return( cmd_status );

}

            
/*===========================================================================

FUNCTION
  bt_sd_register_service

DESCRIPTION
  Process the command BT_CMD_SD_REGISTER_SERVICE
============================================================================*/
LOCAL void bt_sd_register_service
(
  bt_cmd_msg_type*  rs_ptr
)
{

  bt_sd_srv_rec_type*  sr_ptr;
  boolean              prev_enabled_services;

  rs_ptr->cmd_hdr.cmd_status = BT_CS_GN_SUCCESS;

  if ( (sr_ptr = bt_sd_find_service_record_by_uuid(
                   BT_SD_DB_TYPE_LOCAL_DB,
                   rs_ptr->cmd_msg.cmd_sd_reg_srv.uuid,
                   rs_ptr->cmd_msg.cmd_sd_reg_srv.scn,
                   rs_ptr->cmd_msg.cmd_sd_reg_srv.psm )) == 0 )
  { 
    if ( (sr_ptr =
            bt_sd_create_service_record(
              rs_ptr->cmd_msg.cmd_sd_reg_srv.uuid,
              rs_ptr->cmd_msg.cmd_sd_reg_srv.scn,
              rs_ptr->cmd_msg.cmd_sd_reg_srv.psm,
              rs_ptr->cmd_msg.cmd_sd_reg_srv.profile_version,
              (char *)rs_ptr->cmd_msg.cmd_sd_reg_srv.service_name_str )) != NULL )
    {
      prev_enabled_services = bt_sd_any_local_services_enabled();

      bt_sd_enable_service_record( sr_ptr, TRUE );

      bt_sd_update_service_connectable(
        sr_ptr, rs_ptr->cmd_msg.cmd_sd_reg_srv.connectable );

      switch ( rs_ptr->cmd_msg.cmd_sd_reg_srv.uuid )
      {
        case BT_SD_SERVICE_CLASS_DIALUP_NETWORKING:
        case BT_SD_SERVICE_CLASS_PANU:
        case BT_SD_SERVICE_CLASS_NAP:
        case BT_SD_SERVICE_CLASS_GN:
          bt_sd_set_cod_srv_class_bit( BT_SD_MAJOR_SERVICE_CLASS_NETWORKING, 
                                       &bt_sd_networking_srv_count );
          break;

#ifdef FEATURE_BT_EXTPF
        case BT_SD_SERVICE_CLASS_OBEX_OBJECT_PUSH:
        case BT_SD_SERVICE_CLASS_OBEX_FILE_TRANSFER:
        case BT_SD_SERVICE_CLASS_IMAGING_RESPONDER:
        case BT_SD_SERVICE_CLASS_IMAGING_AUTOMATIC_ARCHIVE:
        case BT_SD_SERVICE_CLASS_IMAGING_REFERENCED_OBJECTS:
        case BT_SD_SERVICE_CLASS_DIRECT_PRINTING_REFERENCE_OBJECTS_SERVICE:
        case BT_SD_SERVICE_CLASS_PBAP_PCE:
        case BT_SD_SERVICE_CLASS_PBAP_PSE:
          bt_sd_set_cod_srv_class_bit( BT_SD_MAJOR_SERVICE_CLASS_OBJECT_TRANSFER, 
                                       &bt_sd_obex_srv_count );
          break;
#endif /* FEATURE_BT_EXTPF */

#ifdef FEATURE_BT_EXTPF_AV
        case BT_SD_SERVICE_CLASS_AUDIO_SOURCE:
          bt_sd_set_cod_srv_class_bit( BT_SD_MAJOR_SERVICE_CLASS_CAPTURING,
                                       &bt_sd_av_srv_count );
          break;
        case BT_SD_SERVICE_CLASS_AUDIO_SINK:
          /* could use a different ref count for completeness */
          bt_sd_set_cod_srv_class_bit( BT_SD_MAJOR_SERVICE_CLASS_RENDERING,
                                       &bt_sd_av_srv_count );
          break;
#endif /* FEATURE_BT_EXTPF_AV */
        default:
          break;
      }

      if ( prev_enabled_services == FALSE )
      {
        BT_MSG_API( "BT SD CMD TX: L2 StPrm AID %x PSM %x",
                    bt_sd_app_id, BT_L2_PSM_SDP, 0 );
        if ( (rs_ptr->cmd_hdr.cmd_status =
                bt_cmd_l2_set_params(
                  bt_sd_app_id, BT_L2_NULL_CID, BT_L2_PSM_SDP,
                  BT_L2_DEFAULT_FLUSH_TIMEOUT, BT_L2_DEFAULT_MTU,
                  BT_L2_DEFAULT_SERVICE_TYPE, BT_L2_DEFAULT_TOKEN_RATE,
                  BT_L2_DEFAULT_TOKEN_BUCKET_SIZE,
                  BT_L2_DEFAULT_PEAK_BANDWIDTH,
                  BT_L2_DEFAULT_LATENCY,
                  BT_L2_DEFAULT_DELAY_VARIATION )) == BT_CS_GN_PENDING )
        {
          BT_MSG_API( "BT SD CMD TX: L2 Reg AID %x PSM %x",
                      bt_sd_app_id, BT_L2_PSM_SDP, 0 );
          if ( (rs_ptr->cmd_hdr.cmd_status =
                  bt_cmd_l2_register(
                    bt_sd_app_id, BT_L2_PSM_SDP )) == BT_CS_GN_PENDING )
          {
            rs_ptr->cmd_hdr.cmd_status = bt_sd_check_connectable_setting();
          }
        }
      }
#ifdef BT_SWDEV_2_1_EIR
      /* Send all 16-bit UUIDs to RM */
      bt_sd_eir_update_rm_uuid( BT_UUID_16BIT_TYPE );
#endif /* BT_SWDEV_2_1_EIR */
    }
    else
    {
      rs_ptr->cmd_hdr.cmd_status = BT_CS_SD_LOCAL_SERVICE_DB_FULL;
    }
  }
  else
  {
    rs_ptr->cmd_hdr.cmd_status = BT_CS_SD_SERVICE_RECORD_EXISTS;
  }

}


/*===========================================================================

FUNCTION
  bt_sd_register_custom_service

DESCRIPTION
  Process the command BT_CMD_SD_REGISTER_CUSTOM_SERVICE
============================================================================*/
LOCAL void bt_sd_register_custom_service
(
  bt_cmd_msg_type*  rs_ptr
)
{

  bt_sd_srv_rec_type* sr_ptr;
  boolean             prev_enabled_services;

  rs_ptr->cmd_hdr.cmd_status = BT_CS_GN_SUCCESS;

  if ( (sr_ptr = bt_sd_find_service_record_by_uuid128(
                   BT_SD_DB_TYPE_LOCAL_DB,
                   &rs_ptr->cmd_msg.cmd_sd_reg_custom_srv.uuid_list.uuid128[0],
                   rs_ptr->cmd_msg.cmd_sd_reg_srv.scn,
                   rs_ptr->cmd_msg.cmd_sd_reg_srv.psm )) == NULL )
  { 
    if ( (sr_ptr =
            bt_sd_create_custom_service_record(
              &rs_ptr->cmd_msg.cmd_sd_reg_custom_srv.uuid_list,
              rs_ptr->cmd_msg.cmd_sd_reg_custom_srv.scn,
              rs_ptr->cmd_msg.cmd_sd_reg_custom_srv.psm,
              rs_ptr->cmd_msg.cmd_sd_reg_custom_srv.profile_version,
              (char *)rs_ptr->cmd_msg.cmd_sd_reg_custom_srv.service_name_str )) != NULL )
    {
      prev_enabled_services = bt_sd_any_local_services_enabled();

      bt_sd_enable_service_record( sr_ptr, TRUE );

      bt_sd_update_service_connectable(
        sr_ptr, rs_ptr->cmd_msg.cmd_sd_reg_custom_srv.connectable );

      if ( prev_enabled_services == FALSE )
      {
        BT_MSG_API( "BT SD CMD TX: L2 StPrm AID %x PSM %x",
                    bt_sd_app_id, BT_L2_PSM_SDP, 0 );
        if ( (rs_ptr->cmd_hdr.cmd_status =
                bt_cmd_l2_set_params(
                  bt_sd_app_id, BT_L2_NULL_CID, BT_L2_PSM_SDP,
                  BT_L2_DEFAULT_FLUSH_TIMEOUT, BT_L2_DEFAULT_MTU,
                  BT_L2_DEFAULT_SERVICE_TYPE, BT_L2_DEFAULT_TOKEN_RATE,
                  BT_L2_DEFAULT_TOKEN_BUCKET_SIZE,
                  BT_L2_DEFAULT_PEAK_BANDWIDTH,
                  BT_L2_DEFAULT_LATENCY,
                  BT_L2_DEFAULT_DELAY_VARIATION )) == BT_CS_GN_PENDING )
        {
          BT_MSG_API( "BT SD CMD TX: L2 Reg AID %x PSM %x",
                      bt_sd_app_id, BT_L2_PSM_SDP, 0 );
          if ( (rs_ptr->cmd_hdr.cmd_status =
                  bt_cmd_l2_register(
                    bt_sd_app_id, BT_L2_PSM_SDP )) == BT_CS_GN_PENDING )
          {
            rs_ptr->cmd_hdr.cmd_status = bt_sd_check_connectable_setting();
          }
        }
      }
#ifdef BT_SWDEV_2_1_EIR
      /* Send all 128 bit UUIDs to RM */
      bt_sd_eir_update_rm_uuid( BT_UUID_128BIT_TYPE );
#endif /* BT_SWDEV_2_1_EIR */
    }
    else
    {
      rs_ptr->cmd_hdr.cmd_status = BT_CS_SD_LOCAL_SERVICE_DB_FULL;
    }
  }
  else
  {
    rs_ptr->cmd_hdr.cmd_status = BT_CS_SD_SERVICE_RECORD_EXISTS;
  }

}


/*===========================================================================

FUNCTION
  bt_sd_perform_service_unreg

DESCRIPTION
  This routine removes a service record and performs the associated cleanup.
  
============================================================================*/
bt_cmd_status_type bt_sd_perform_service_unreg
( 
  bt_sd_srv_rec_type* sr_ptr 
)
{
  boolean             prev_enabled_services;
  bt_cmd_status_type  status = BT_CS_GN_SUCCESS;
  
  prev_enabled_services = bt_sd_any_local_services_enabled();

  bt_sd_free_service_record( sr_ptr );

  if ( ! sr_ptr->is_custom_srv )
  {
    switch ( sr_ptr->srv_uuid )
    {
      case BT_SD_SERVICE_CLASS_DIALUP_NETWORKING:
      case BT_SD_SERVICE_CLASS_PANU:
      case BT_SD_SERVICE_CLASS_NAP:
      case BT_SD_SERVICE_CLASS_GN:
        bt_sd_clear_cod_srv_class_bit( BT_SD_MAJOR_SERVICE_CLASS_NETWORKING, 
                                       &bt_sd_networking_srv_count );
        break;
#ifdef FEATURE_BT_EXTPF
      case BT_SD_SERVICE_CLASS_OBEX_OBJECT_PUSH:
      case BT_SD_SERVICE_CLASS_OBEX_FILE_TRANSFER:
      case BT_SD_SERVICE_CLASS_IMAGING_RESPONDER:
      case BT_SD_SERVICE_CLASS_IMAGING_AUTOMATIC_ARCHIVE:
      case BT_SD_SERVICE_CLASS_IMAGING_REFERENCED_OBJECTS:
      case BT_SD_SERVICE_CLASS_DIRECT_PRINTING_REFERENCE_OBJECTS_SERVICE:
        bt_sd_clear_cod_srv_class_bit( BT_SD_MAJOR_SERVICE_CLASS_OBJECT_TRANSFER, 
                                       &bt_sd_obex_srv_count );
        break;
#endif /* FEATURE_BT_EXTPF */
#ifdef FEATURE_BT_EXTPF_AV 
      case BT_SD_SERVICE_CLASS_AUDIO_SOURCE: 
        bt_sd_clear_cod_srv_class_bit( BT_SD_MAJOR_SERVICE_CLASS_CAPTURING, 
                                       &bt_sd_av_srv_count ); 
        break; 
      case BT_SD_SERVICE_CLASS_AUDIO_SINK: 
        /* could use a different ref count for completeness */
        bt_sd_clear_cod_srv_class_bit( BT_SD_MAJOR_SERVICE_CLASS_RENDERING, 
                                       &bt_sd_av_srv_count ); 
        break; 
#endif /* FEATURE_BT_EXTPF_AV */ 
      default:
        break;
    }
  }

  if ( (prev_enabled_services != FALSE) &&
       (bt_sd_any_local_services_enabled() == FALSE) )
  {
    BT_MSG_API( "BT SD CMD TX: L2 Unreg AID %x PSM %x",
                bt_sd_app_id, BT_L2_PSM_SDP, 0 );
    if ( (status =
            bt_cmd_l2_unregister(
              bt_sd_app_id, BT_L2_PSM_SDP )) == BT_CS_GN_PENDING )
    {
      status = bt_sd_check_connectable_setting();
    }
  }
  return( status );
}

/*===========================================================================

FUNCTION
  bt_sd_unregister_service

DESCRIPTION
  Process the command BT_CMD_SD_UNREGISTER_SRV_BY_SCN and
  BT_CMD_SD_UNREGISTER_SRV_BY_PSM.
============================================================================*/
LOCAL void bt_sd_unregister_service
(
  bt_cmd_msg_type*  urs_ptr
)
{

  bt_sd_srv_rec_type*  sr_ptr;

  if ( ( urs_ptr->cmd_msg.cmd_sd_unreg_srv.scn != BT_RC_SCN_NOT_SPECIFIED ) &&
       ( urs_ptr->cmd_msg.cmd_sd_unreg_srv.psm != BT_L2_PSM_INVALID ) )
  {
    /* either SCN or PSM can be used but not both */
    urs_ptr->cmd_hdr.cmd_status = BT_CS_SD_BAD_PARAM;
    return;
  }        

  urs_ptr->cmd_hdr.cmd_status = BT_CS_GN_SUCCESS;

  if ( (sr_ptr = bt_sd_find_service_record_by_uuid(
                   BT_SD_DB_TYPE_LOCAL_DB,
                   urs_ptr->cmd_msg.cmd_sd_unreg_srv.uuid,
                   urs_ptr->cmd_msg.cmd_sd_unreg_srv.scn,
                   urs_ptr->cmd_msg.cmd_sd_unreg_srv.psm )) != 0 )
  {
    urs_ptr->cmd_hdr.cmd_status = bt_sd_perform_service_unreg( sr_ptr );
#ifdef BT_SWDEV_2_1_EIR
      /* Send all 16-bit UUIDs to RM */
      bt_sd_eir_update_rm_uuid( BT_UUID_16BIT_TYPE );
#endif /* BT_SWDEV_2_1_EIR */
  }
  else
  {
    urs_ptr->cmd_hdr.cmd_status = BT_CS_SD_SERVICE_RECORD_NOT_FOUND;
  }

}


/*===========================================================================

FUNCTION
  bt_sd_unregister_custom_service

DESCRIPTION
  Process the command BT_CMD_SD_UNREGISTER_CUSTOM_SERVICE.
============================================================================*/
LOCAL void bt_sd_unregister_custom_service
(
  bt_cmd_msg_type*  urcs_ptr
)
{

  bt_sd_srv_rec_type*  sr_ptr;

  urcs_ptr->cmd_hdr.cmd_status = BT_CS_GN_SUCCESS;

  if ( (sr_ptr = bt_sd_find_service_record_by_uuid128(
                   BT_SD_DB_TYPE_LOCAL_DB,
                   &urcs_ptr->cmd_msg.cmd_sd_unreg_custom_srv.uuid128,
                   BT_RC_SCN_NOT_SPECIFIED,
                   BT_L2_PSM_INVALID)) != 0 )
  {
    urcs_ptr->cmd_hdr.cmd_status = bt_sd_perform_service_unreg( sr_ptr );
#ifdef BT_SWDEV_2_1_EIR
      /* Send all 128-bit UUIDs to RM */
      bt_sd_eir_update_rm_uuid( BT_UUID_128BIT_TYPE );
#endif /* BT_SWDEV_2_1_EIR */
  }
  else
  {
    urcs_ptr->cmd_hdr.cmd_status = BT_CS_SD_SERVICE_RECORD_NOT_FOUND;
  }

}


/*===========================================================================

FUNCTION
  bt_sd_unregister_all_srv_with_uuid

DESCRIPTION
  Process the command BT_CMD_SD_UNREGISTER_SERVICE
============================================================================*/
LOCAL void bt_sd_unregister_all_srv_with_uuid
(
  bt_cmd_msg_type*  urs_ptr
)
{

  bt_sd_srv_rec_type*  sr_ptr;
  bt_cmd_status_type   status;
  boolean              match = FALSE;

  urs_ptr->cmd_hdr.cmd_status = BT_CS_GN_SUCCESS;

  bt_sd_srv_rec_iter.init( BT_SD_DB_TYPE_LOCAL_DB, BT_SD_ITER_ALL );
  for ( sr_ptr = ( bt_sd_srv_rec_type* ) bt_sd_srv_rec_iter.first();
                                       /* Explicitly cast away the constness */
        sr_ptr != 0;
        sr_ptr = ( bt_sd_srv_rec_type* ) bt_sd_srv_rec_iter.next() )
                                      /* Explicitly cast away the constness */
  { 
    if ( sr_ptr->srv_uuid == urs_ptr->cmd_msg.cmd_sd_unreg_srv.uuid )
    {
      match = TRUE;
      status = bt_sd_perform_service_unreg( sr_ptr );

      if ( status != BT_CS_GN_SUCCESS )
      {
        urs_ptr->cmd_hdr.cmd_status = status;
      }
#ifdef BT_SWDEV_2_1_EIR
      /* Send all 16-bit UUIDs to RM */
      bt_sd_eir_update_rm_uuid( BT_UUID_16BIT_TYPE );
#endif /* BT_SWDEV_2_1_EIR */
    }
  } /* end for each service record */

  if ( !match )
  {
    urs_ptr->cmd_hdr.cmd_status = BT_CS_SD_SERVICE_RECORD_NOT_FOUND;
  }

}

#ifdef FEATURE_BT_EXTPF_SML

/*===========================================================================

FUNCTION
  bt_sd_register_syncml_service

DESCRIPTION
  Process the command BT_CMD_SD_REGISTER_SYNCML_SERVICE
============================================================================*/
LOCAL void bt_sd_register_syncml_service
(
  bt_cmd_msg_type*  rss_ptr
)
{

  bt_sd_srv_rec_type*  sr_ptr;
  boolean              prev_enabled_services;

  rss_ptr->cmd_hdr.cmd_status = BT_CS_GN_SUCCESS;

  if ( (sr_ptr = bt_sd_find_syncml_service_record(
                   BT_SD_DB_TYPE_LOCAL_DB,
                   rss_ptr->cmd_msg.cmd_sd_reg_syncml_srv.client )) == 0 )
  {
    if ( (sr_ptr =
            bt_sd_create_syncml_service_record(
              rss_ptr->cmd_msg.cmd_sd_reg_syncml_srv.client,
              rss_ptr->cmd_msg.cmd_sd_reg_syncml_srv.scn,
             (char *)rss_ptr->cmd_msg.cmd_sd_reg_syncml_srv.service_name_str )) != NULL )
   {
      prev_enabled_services = bt_sd_any_local_services_enabled();

      bt_sd_enable_service_record( sr_ptr, TRUE );

      bt_sd_set_cod_srv_class_bit( BT_SD_MAJOR_SERVICE_CLASS_OBJECT_TRANSFER, 
                                   &bt_sd_obex_srv_count );

      if ( prev_enabled_services == FALSE )
      {
        BT_MSG_API( "BT SD CMD TX: L2 StPrm AID %x PSM %x",
                    bt_sd_app_id, BT_L2_PSM_SDP, 0 );
        if ( (rss_ptr->cmd_hdr.cmd_status =
                bt_cmd_l2_set_params(
                  bt_sd_app_id, BT_L2_NULL_CID, BT_L2_PSM_SDP,
                  BT_L2_DEFAULT_FLUSH_TIMEOUT, BT_L2_DEFAULT_MTU,
                  BT_L2_DEFAULT_SERVICE_TYPE, BT_L2_DEFAULT_TOKEN_RATE,
                  BT_L2_DEFAULT_TOKEN_BUCKET_SIZE,
                  BT_L2_DEFAULT_PEAK_BANDWIDTH,
                  BT_L2_DEFAULT_LATENCY,
                  BT_L2_DEFAULT_DELAY_VARIATION )) == BT_CS_GN_PENDING )
        {
          BT_MSG_API( "BT SD CMD TX: L2 Reg AID %x PSM %x",
                      bt_sd_app_id, BT_L2_PSM_SDP, 0 );
          if ( (rss_ptr->cmd_hdr.cmd_status =
                  bt_cmd_l2_register(
                    bt_sd_app_id, BT_L2_PSM_SDP )) == BT_CS_GN_PENDING )
          {
            rss_ptr->cmd_hdr.cmd_status = bt_sd_check_connectable_setting();
          }
        }
      }
    }
    else
    {
      rss_ptr->cmd_hdr.cmd_status = BT_CS_SD_LOCAL_SERVICE_DB_FULL;
    }
  }
  else
  {
    rss_ptr->cmd_hdr.cmd_status = BT_CS_SD_SERVICE_RECORD_EXISTS;
  }

}

/*===========================================================================

FUNCTION
  bt_sd_unregister_syncml_service

DESCRIPTION
  Process the command BT_CMD_SD_UNREGISTER_SYNCML_SERVICE
============================================================================*/
LOCAL void bt_sd_unregister_syncml_service
(
  bt_cmd_msg_type*  urss_ptr
)
{

  bt_sd_srv_rec_type*  sr_ptr;
  boolean              prev_enabled_services;

  urss_ptr->cmd_hdr.cmd_status = BT_CS_GN_SUCCESS;

  if ( (sr_ptr = bt_sd_find_syncml_service_record(
                   BT_SD_DB_TYPE_LOCAL_DB,
                   urss_ptr->cmd_msg.cmd_sd_unreg_syncml_srv.client )) != 0 )
  {
    prev_enabled_services = bt_sd_any_local_services_enabled();

    bt_sd_free_service_record( sr_ptr );

    bt_sd_clear_cod_srv_class_bit( BT_SD_MAJOR_SERVICE_CLASS_OBJECT_TRANSFER, 
                                   &bt_sd_obex_srv_count );

    if ( (prev_enabled_services != FALSE) &&
         (bt_sd_any_local_services_enabled() == FALSE) )
    {
      BT_MSG_API( "BT SD CMD TX: L2 Unreg AID %x PSM %x",
                  bt_sd_app_id, BT_L2_PSM_SDP, 0 );
      if ( (urss_ptr->cmd_hdr.cmd_status =
              bt_cmd_l2_unregister(
                bt_sd_app_id, BT_L2_PSM_SDP )) == BT_CS_GN_PENDING )
      {
        urss_ptr->cmd_hdr.cmd_status = bt_sd_check_connectable_setting();
      }
    }
  }
  else
  {
    urss_ptr->cmd_hdr.cmd_status = BT_CS_SD_SERVICE_RECORD_NOT_FOUND;
  }

}
#endif /* FEATURE_BT_EXTPF_SML */


/*-------------------------------------------------------------------------*/
/*                                                                         */
/*                        External Functions                               */
/*                                                                         */
/*-------------------------------------------------------------------------*/

/*===========================================================================

FUNCTION
  bt_sd_powerup_init

DESCRIPTION
  Initialize the SDP layer upon powerup

===========================================================================*/

void bt_sd_powerup_init( void )
{
  uint16               i;
  #if defined( FEATURE_BT_LPOS ) || defined( BT_TEST_SDP )  
  bt_sd_srv_rec_type*  sr_ptr = NULL;
  #endif

#ifdef BT_SWDEV_2_1_SSP
  bt_service_id_type  svc_id = 
    { BT_SIM_SDP_UUID, BT_SD_PROTOCOL_SDP, 0, 0 };
  bt_cmd_status_type  status;
#endif /* BT_SWDEV_2_1_SSP */

  bt_sd_app_id = bt_cmd_ec_get_app_id_and_register( bt_sd_store_bt_event );
  ASSERT_DEBUG( bt_sd_app_id != BT_APP_ID_NULL );
  BT_MSG_API( "BT SD TRX: EC Get AID %x", bt_sd_app_id, 0, 0 );
  BT_MSG_HIGH( "BT SD AID is %x", bt_sd_app_id, 0, 0 );

  (void) q_init( &bt_sd_ev_q );
  bt_sd_ev_q_info.event_q_ptr      = &bt_sd_ev_q;
  bt_sd_ev_q_info.max_q_depth      = BT_SD_NUM_EVENTS;
  bt_sd_ev_q_info.event_q_bit_mask = BT_EVQ_SD_B;
  bt_sd_ev_q_info.max_event_bytes  = bt_sd_get_max_event_bytes();

  /* Populate the SDP database */
  bt_sd_initialize_database( BT_SD_DB_TYPE_LOCAL_DB );

#ifdef FEATURE_BT_LPOS
#error code not present
#endif /* FEATURE_BT_LPOS */

#ifdef BT_TEST_SDP
  if ( (sr_ptr = bt_sd_create_service_record(
                   BT_SD_SERVICE_CLASS_FAX,
                   0x02,
                   BT_L2_PSM_INVALID,
                   0x0100,
                   "Fax" )) != NULL )
  {
    bt_sd_enable_service_record( sr_ptr, TRUE );
  }

  if ( (sr_ptr = bt_sd_create_service_record(
                   BT_SD_SERVICE_CLASS_OBEX_OBJECT_PUSH,
                   0x20,
                   BT_L2_PSM_INVALID,
                   0x0100,
                   "OBEX Object Push" )) != NULL )
  {
    bt_sd_enable_service_record( sr_ptr, TRUE );
  }

  if ( (sr_ptr = bt_sd_create_service_record(
                   BT_SD_SERVICE_CLASS_IMAGING_RESPONDER,
                   0x21,
                   BT_L2_PSM_INVALID,
                   0x0100,
                   "Imaging" )) != NULL )
  {
    bt_sd_enable_service_record( sr_ptr, TRUE );
  }
#endif /* BT_TEST_SDP */

  /* Initialize the Device Discovery DB */
  bt_sd_dev_discv_db.dev_discv_in_progress = FALSE;
  bt_sd_dev_discv_db.num_discvd_dev = 0;
  bt_sd_dev_discv_db.remote_name_req_in_progress = FALSE;

  /* Initialize the L2CAP connection table */
  for ( i = 0; i < BT_SD_MAX_NUM_OF_L2CAP_CONNECTION; i++ )
  {
    bt_sd_conn_table[i].used = FALSE;
    bt_sd_conn_table[i].cid_loc_initd = BT_L2_NULL_CID;

    timer_def( &bt_sd_conn_table[i].tmr_loc_initd, 
               &bt_always_on_timer_group, &bt_tcb, 
               BT_SD_TIMER_SIG, NULL, 0 );
    bt_sd_conn_table[i].cid_rem_initd = BT_L2_NULL_CID;

    timer_def( &bt_sd_conn_table[i].tmr_rem_initd, 
               &bt_always_on_timer_group, &bt_tcb, 
               BT_SD_TIMER_SIG, NULL, 0 );

    bt_sd_conn_table[i].tid = 0;
    bt_sd_conn_table[i].pending_req_dsm_ptr = 0;
    bt_sd_conn_table[i].conn_state = BT_SD_CONN_STATE_DISCONNECTED;
  }

  /*  Initialize the Discoverable Applications table.  */
  for ( i = 0; i < BT_SD_MAX_DISCOVERABLE_APPS; i++ )
  {
    bt_sd_init_discoverable_app( &bt_da[ i ] );
  }

  bt_sd_data.app_id_user = BT_APP_ID_NULL;

  bt_sd_connectable = FALSE;
#ifdef BT_SWDEV_2_1_SSP
  /* Note: Don't need to do SM4 registration by UUID, since our concept of UUID
   * is broken (and always has been) and only applies to RFCOMM services.
   * 
   * This next will be both for incoming & outgoing SDP.
   */
  svc_id.service_id_method = BT_SIM_L2CAP_PSM;
  svc_id.l2cap_psm = BT_L2_PSM_SDP;
  if ( (status = 
        bt_cmd_rm_set_sm4( bt_sd_app_id, 
                           &svc_id, 
                           BT_RM_SL_0_NONE, 
                           FALSE, 
                           FALSE )) != BT_CS_GN_PENDING )
  {
    BT_ERR( "BT SD: Failed to set sm4, stat=%x", status, 0, 0 );
  }
#endif /* BT_SWDEV_2_1_SSP */

}


/*===========================================================================

FUNCTION
  bt_sd_qdsp_ready_init

DESCRIPTION
  Initialize the SDP layer when the Bluetooth QDSP image is loaded

===========================================================================*/

void bt_sd_qdsp_ready_init( void )
{
}


/*===========================================================================

FUNCTION
  bt_sd_qdsp_reset_shutdown

DESCRIPTION
  De-initialize the SDP layer when the Bluetooth QDSP image is unloaded

===========================================================================*/

void bt_sd_qdsp_reset_shutdown( void )
{
}


/*===========================================================================

FUNCTION
  bt_sd_soc_reset_shutdown

DESCRIPTION
  De-initialize the SDP layer when the Bluetooth SOC is powered down.

===========================================================================*/

void bt_sd_soc_reset_shutdown( void )
{
  uint16               i;
  for ( i = 0; i < BT_SD_MAX_NUM_OF_L2CAP_CONNECTION; i++ )
  {
    (void)rex_clr_timer( &bt_sd_conn_table[i].tmr_loc_initd );

    (void)rex_clr_timer( &bt_sd_conn_table[i].tmr_rem_initd );
  }
  (void)rex_clr_sigs( &bt_tcb, BT_SD_TIMER_SIG );
}

/*===========================================================================

FUNCTION
  bt_sd_process_command

DESCRIPTION
  Process SDP commands.

===========================================================================*/

void bt_sd_process_command(
  bt_cmd_msg_type*   cmd_msg_ptr
)
{
  switch( cmd_msg_ptr->cmd_hdr.cmd_type )
  {
    case BT_CMD_SD_SET_SERVICE_DISCOVERABLE_MODE:
    {
      BT_MSG_API( "BT SD CMD RX: Set Discoverable", 0, 0, 0 );
      bt_sd_set_service_discoverable_mode( cmd_msg_ptr );
      break;
    }

    case BT_CMD_SD_GET_DISCOVERABLE_MODE:
    {
      BT_MSG_API( "BT SD CMD RX: Get Discoverable Mode", 0, 0, 0 );
      bt_sd_get_discoverable_mode( cmd_msg_ptr );
      break;
    }

    case BT_CMD_SD_DISCOVER_DEVICES:
    case BT_CMD_SD_DISCOVER_DEVICES_EXT:
    case BT_CMD_SD_START_DEVICE_DISCOVERY:
    {
      BT_MSG_API( "BT SD CMD RX: Start Discovery", 0, 0, 0 );
      bt_sd_start_device_discovery( cmd_msg_ptr );
      break;
    }

    case BT_CMD_SD_STOP_DEVICE_DISCOVERY:
    {
      BT_MSG_API( "BT SD CMD RX: Stop Discovery", 0, 0, 0 );
      bt_sd_stop_device_discovery( cmd_msg_ptr );
      break;
    }

    case BT_CMD_SD_GET_DEVICE_NAME:
    {
      //BT_BDA( MSG_API, "BT SD CMD RX: Get Dev Name",
      //        &( cmd_msg_ptr->cmd_msg.cmd_sd_get_dev_name.bd_addr ) );
      bt_sd_get_device_name( cmd_msg_ptr );
      break;
    }

    case BT_CMD_SD_GET_DEVICE_NAME_CANCEL:
    {
      //BT_BDA( MSG_API, "BT SD CMD RX: Get Dev Name Cancel",
        //      &( cmd_msg_ptr->cmd_msg.cmd_sd_get_dev_name_cancel.bd_addr ) );
      bt_sd_get_device_name_cancel( cmd_msg_ptr );
      break;
    }

    case BT_CMD_SD_STOP_SERVICE_SEARCH:
    {
      //BT_BDA( MSG_API, "BT SD CMD RX: Stop Srv Srch",
       //       &( cmd_msg_ptr->cmd_msg.cmd_sd_stop_srv_srch.bd_addr ) );
      bt_sd_stop_service_search( cmd_msg_ptr );
      break;
    }

    case BT_CMD_SD_SEARCH_SERVICE:
    case BT_CMD_SD_SEARCH_SERVICE_EXT:
    {
      //BT_BDA( MSG_API, "BT SD CMD RX: Search Service",
      //        &( cmd_msg_ptr->cmd_msg.cmd_sd_srv_srch.bd_addr ) );
      BT_MSG_API( "BT SD CMD RX: Srch Srv Gen XML %x", 
                  cmd_msg_ptr->cmd_msg.cmd_sd_srv_srch.generate_xml, 0, 0 );
      bt_sd_search_service( cmd_msg_ptr );
      break;
    }

    case BT_CMD_SD_CANCEL_IDLE_TIMER:
    {
      //BT_BDA( MSG_API, "BT SD CMD RX: Cancel Idle Timer",
       //       &( cmd_msg_ptr->cmd_msg.cmd_sd_cancel_idle_timer.bd_addr ) );
      bt_sd_cancel_idle_timer( cmd_msg_ptr );
      break;
    }

    case BT_CMD_SD_GET_SERVER_CHANNEL_NUMBER:
    {
      //BT_BDA( MSG_API, "BT SD CMD RX: Get Srv Channel #",
        //      &( cmd_msg_ptr->cmd_msg.cmd_sd_get_scn.bd_addr ) );
      bt_sd_get_server_channel_number( cmd_msg_ptr );
      break;
    }

    case BT_CMD_SD_GET_SECONDARY_SERVER_CHANNEL_NUMBER:
    {
      //BT_BDA( MSG_API, "BT SD CMD RX: Get Sec Srv Channel #",
//              &( cmd_msg_ptr->cmd_msg.cmd_sd_get_scn.bd_addr ) );
      bt_sd_get_server_channel_number( cmd_msg_ptr );
      break;
    }

    case BT_CMD_SD_SEARCH_SERVICE_UUID128:
    {
      //BT_BDA( MSG_API, "BT SD CMD RX: Search Srv UUID 128",
            //  &( cmd_msg_ptr->cmd_msg.cmd_sd_srv_srch.bd_addr ) );
      bt_sd_search_service( cmd_msg_ptr );
      break;
    }

    case BT_CMD_SD_UPDATE_SERVICE_RECORD:
    case BT_CMD_SD_UPDATE_SRV_REC_BY_SCN:
    case BT_CMD_SD_UPDATE_SRV_REC_BY_PSM:
    {
      BT_MSG_API( "BT SD CMD RX: Update Srv Rec UUID %x SCN %x PSM %x", 
                  cmd_msg_ptr->cmd_msg.cmd_sd_update_srv_rec.uuid, 
                  cmd_msg_ptr->cmd_msg.cmd_sd_update_srv_rec.scn, 
                  cmd_msg_ptr->cmd_msg.cmd_sd_update_srv_rec.psm );
      bt_sd_update_service_record( cmd_msg_ptr );
      break;
    }

    case BT_CMD_SD_UPDATE_SRV_NAME:
    {
      BT_MSG_API( "BT SD CMD RX: Update Srv Name UUID %x", 
                  cmd_msg_ptr->cmd_msg.cmd_sd_update_srv_rec.uuid, 0, 0 );
      bt_sd_update_service_name( cmd_msg_ptr );
      break;
    }

#ifdef FEATURE_BT_EXTPF
    case BT_CMD_SD_UPDATE_OBEX_PROFILE_SERVICE_RECORD:
    {
      BT_MSG_API( "BT SD CMD RX: Update Obex Srv Rec", 0, 0, 0 );
      bt_sd_update_obex_profile_service_record( cmd_msg_ptr );
      break;
    }

    case BT_CMD_SD_CUSTOM_UPDATE_OBEX_PROFILE_SERVICE_RECORD:
    {
      BT_MSG_API( "BT SD CMD RX: Custom Update Obex Srv Rec", 0, 0, 0 );
      bt_sd_custom_update_obex_profile_service_record( cmd_msg_ptr );
      break;
    }
#endif /* FEATURE_BT_EXTPF */

    case BT_CMD_SD_REGISTER_SERVICE:
    case BT_CMD_SD_REGISTER_SERV_EXT:
    {
      BT_MSG_API( "BT SD CMD RX: Reg Srv UUID %x SCN %x PSM %x",
                  cmd_msg_ptr->cmd_msg.cmd_sd_reg_srv.uuid,
                  cmd_msg_ptr->cmd_msg.cmd_sd_reg_srv.scn, 
                  cmd_msg_ptr->cmd_msg.cmd_sd_reg_srv.psm );
      BT_MSG_API( "BT SD CMD RX: Reg Srv PfVer %x C'able %x",
                 cmd_msg_ptr->cmd_msg.cmd_sd_reg_srv.profile_version,
                 cmd_msg_ptr->cmd_msg.cmd_sd_reg_srv.connectable, 0 );
      bt_sd_register_service( cmd_msg_ptr );
      break;
    }

    case BT_CMD_SD_REGISTER_CUSTOM_SERVICE:
    case BT_CMD_SD_REGISTER_CUSTOM_SERVICE_EXT:
    {
      BT_MSG_API( "BT SD CMD RX: Reg Custom Srv #UUID128 %x #UUID16 %x",
                  cmd_msg_ptr->cmd_msg.cmd_sd_reg_custom_srv.uuid_list.num_uuid128,
                  cmd_msg_ptr->cmd_msg.cmd_sd_reg_custom_srv.uuid_list.num_uuid, 0 );
      //BT_MSG_UUID128( MSG_API, "BT SD CMD RX: Reg Custom Srv",
       //               &cmd_msg_ptr->cmd_msg.cmd_sd_reg_custom_srv.uuid_list.uuid128[0] );
      BT_MSG_API( "BT SD CMD RX: Reg Custom Srv SCN %x PSM %x PfVer %x",
                  cmd_msg_ptr->cmd_msg.cmd_sd_reg_custom_srv.scn,
                  cmd_msg_ptr->cmd_msg.cmd_sd_reg_custom_srv.psm,
                  cmd_msg_ptr->cmd_msg.cmd_sd_reg_custom_srv.profile_version );
      bt_sd_register_custom_service( cmd_msg_ptr );
      break;
    }

    case BT_CMD_SD_UNREGISTER_SERVICE:
    {
      BT_MSG_API( "BT SD CMD RX: Unreg Srv UUID %x",
                  cmd_msg_ptr->cmd_msg.cmd_sd_unreg_srv.uuid, 0, 0 );
      bt_sd_unregister_all_srv_with_uuid( cmd_msg_ptr );
      break;
    }

    case BT_CMD_SD_UNREGISTER_SRV_BY_SCN:
    case BT_CMD_SD_UNREGISTER_SRV_BY_PSM:
    {
      BT_MSG_API( "BT SD CMD RX: Unreg Srv UUID %x SCN %x PSM %x",
                  cmd_msg_ptr->cmd_msg.cmd_sd_unreg_srv.uuid, 
                  cmd_msg_ptr->cmd_msg.cmd_sd_unreg_srv.scn, 
                  cmd_msg_ptr->cmd_msg.cmd_sd_unreg_srv.psm );
      bt_sd_unregister_service( cmd_msg_ptr );
      break;
    }

    case BT_CMD_SD_UNREGISTER_CUSTOM_SERVICE:
    {
      //BT_MSG_UUID128( MSG_API, "BT SD CMD RX: Unreg Custom Srv",
         //             &cmd_msg_ptr->cmd_msg.cmd_sd_unreg_custom_srv.uuid128 );
      bt_sd_unregister_custom_service( cmd_msg_ptr );
      break;
    }

#ifdef FEATURE_BT_EXTPF_SML
    case BT_CMD_SD_REGISTER_SYNCML_SERVICE:
    {
      BT_MSG_API( "BT SD CMD RX: Unreg SyncML", 0, 0, 0 );
      bt_sd_register_syncml_service( cmd_msg_ptr );
      break;
    }

    case BT_CMD_SD_UNREGISTER_SYNCML_SERVICE:
    {
      BT_MSG_API( "BT SD CMD RX: Unreg SyncML", 0, 0, 0 );
      bt_sd_unregister_syncml_service( cmd_msg_ptr );
      break;
    }
#endif /* FEATURE_BT_EXTPF_SML */

    default:
    {
      BT_ERR( "BT SD CMD RX: Unknown: %x", 
              cmd_msg_ptr->cmd_hdr.cmd_type, 0, 0 );
      cmd_msg_ptr->cmd_hdr.cmd_status = BT_CS_GN_UNRECOGNIZED_CMD;
      break;
    }
  }
}


/*===========================================================================

FUNCTION
  bt_sd_process_ev_queue

DESCRIPTION
  Dequeues events on the SDP event queue and processes them.

===========================================================================*/

void bt_sd_process_ev_queue( void )
{
  bt_ev_msg_type*  bt_ev_ptr;
  int              q_count = q_cnt( &bt_sd_ev_q );

  BT_VSTAT_MAX( q_count, SD_MAX_EV_Q_DEPTH );

  while ( ( bt_ev_ptr = (bt_ev_msg_type *) q_get( &bt_sd_ev_q ) ) != NULL )
  {
    bt_sd_process_event( bt_ev_ptr );
    q_put( &bt_event_free_q, &bt_ev_ptr->ev_hdr.link );
  }
}

/*===========================================================================

FUNCTION
  bt_sd_send_pending_request

DESCRIPTION
  Send the pending request

===========================================================================*/

void bt_sd_send_pending_request(
  bt_sd_conn_entry_type*   conn_entry_ptr,
  uint16                   info_len,
  uint8*                   cont_state_info
)
{
  dsm_item_type*       dsm_ptr = 0;
  bt_sd_pdu_hdr_type   pdu_hdr;
  uint16               len;

  /* Clear the Idle Timer */
  rex_clr_timer( &conn_entry_ptr->tmr_loc_initd );

  if ( info_len > 0 )  /* This is a continuation */
  {
    if ( conn_entry_ptr->dup_req_dsm_ptr )
    {
      dsm_free_packet( &conn_entry_ptr->pending_req_dsm_ptr );
      dsm_dup_packet( &conn_entry_ptr->pending_req_dsm_ptr, 
                      conn_entry_ptr->dup_req_dsm_ptr,
                      0,
                      dsm_length_packet( conn_entry_ptr->dup_req_dsm_ptr ) );
    }
    else /* This should never happen */
    {
      BT_ERR( "BT SD: Null duplicate request DSM pointer", 0, 0, 0 );
      return;
    }
  }

  if ( conn_entry_ptr->pending_req_dsm_ptr )
  {
    dsm_ptr = conn_entry_ptr->pending_req_dsm_ptr;
    dsm_pullup( &dsm_ptr, (void*)&pdu_hdr, sizeof(bt_sd_pdu_hdr_type) );
    len = SWAB16( pdu_hdr.length );

     /* Insert ContinuationState information */
    bt_dsm_pushdown_tail( &dsm_ptr, &info_len, 1, DSM_DS_SMALL_ITEM_POOL );
    len++;

    if ( info_len > 0 )
    {
      bt_dsm_pushdown_tail( &dsm_ptr, cont_state_info, info_len, 
                            DSM_DS_SMALL_ITEM_POOL );
      len += info_len;
    }
    else
    {
      /* Increment the transaction ID only if this is a new request,
       * i.e. not a continuation
       */
      conn_entry_ptr->tid++;
    }

    /* Update PDU length */
    pdu_hdr.length = SWAB16( len );

    bt_dsm_pushdown_packed( &dsm_ptr, (void*)&pdu_hdr, sizeof(pdu_hdr),
                            DSM_DS_SMALL_ITEM_POOL );

    BT_MSG_SIG( "BT SD: TX SDP Req Type %x TID %x Len %x",
                pdu_hdr.pid,
                SWAB16( pdu_hdr.tid ),
                len );
    
#ifdef BT_SWDEV_QXDM_LOG_MSG
    bt_sd_sig_tx_logging_function( bt_sd_app_id,
                                   conn_entry_ptr->cid_loc_initd,
                                   dsm_ptr );
#endif
    bt_cmd_l2_write( bt_sd_app_id,
                     conn_entry_ptr->cid_loc_initd,
                     dsm_ptr, NULL );

    conn_entry_ptr->pending_req_dsm_ptr = 0;

    /* Set Request Timeout Timer */
    rex_set_timer( &conn_entry_ptr->tmr_loc_initd, BT_SD_REQ_TIMEOUT_MS );
  }
}


/*===========================================================================

FUNCTION
  bt_sd_process_timer_expiration

DESCRIPTION
  Process the request/idle timeout event

===========================================================================*/

void bt_sd_process_timer_expiration( void )
{
  uint16                  i;
  bt_sd_conn_entry_type*  conn_entry_ptr;

  BT_MSG_HIGH( "BT SD: Timer expired...", 0, 0, 0 );
  
  for ( i = 0; i < BT_SD_MAX_NUM_OF_L2CAP_CONNECTION; i++ )
  {
    conn_entry_ptr = &bt_sd_conn_table[i];

    if ( conn_entry_ptr->used &&
         ( conn_entry_ptr->conn_state == BT_SD_CONN_STATE_CONNECTED ) )
    {
      /*  Check for time outs for connection initiated by local device.  */
      /*  Can either be a SD Request Timeout or an Idle Timeout.         */
      if ( conn_entry_ptr->cid_loc_initd != BT_L2_NULL_CID )
      {
        if ( rex_get_timer( &conn_entry_ptr->tmr_loc_initd ) == 0 )
        {
          if ( conn_entry_ptr->req_pending )
          {
            BT_MSG_HIGH( "BT SD: Req Timeout Local CID %x",
                         conn_entry_ptr->cid_loc_initd, 0, 0 );

            /*  SD Request Timeout. */
            bt_sd_send_timeout_resp_event( conn_entry_ptr );

            /*  Start Idle Timer.  */
            rex_set_timer( &conn_entry_ptr->tmr_loc_initd,
                           BT_SD_IDLE_TIMEOUT_MS );
          }
          else
          {
            conn_entry_ptr->conn_state = BT_SD_CONN_STATE_DISCONNECTING;

            /*  Idle Timeout.  */
            BT_MSG_HIGH( "BT SD: Idle Timeout Local CID %x",
                         conn_entry_ptr->cid_loc_initd, 0, 0 );

            BT_MSG_API( "BT SD CMD TX: L2 Disc CID %x",
                        conn_entry_ptr->cid_loc_initd, 0, 0 );
            bt_cmd_l2_disconnect( bt_sd_app_id,
                                  conn_entry_ptr->cid_loc_initd );
          }
        }
      }
      
      /*  Check for time out for connection initiated by remote device.  */
      /*  Can only be an Idle Timeout.                                   */
      if ( conn_entry_ptr->cid_rem_initd != BT_L2_NULL_CID )
      {
        if ( rex_get_timer( &conn_entry_ptr->tmr_rem_initd ) == 0 )
        {
          conn_entry_ptr->conn_state = BT_SD_CONN_STATE_DISCONNECTING;

          /*  Idle Timeout.  */
          BT_MSG_HIGH( "BT SD: Idle Timeout Remote CID %x",
                       conn_entry_ptr->cid_rem_initd, 0, 0 );

          BT_MSG_API( "BT SD CMD TX: L2 Disc CID %x",
                      conn_entry_ptr->cid_rem_initd, 0, 0 );
          bt_cmd_l2_disconnect( bt_sd_app_id,
                                conn_entry_ptr->cid_rem_initd );
        }
      }
    }
  }
}

/*===========================================================================

FUNCTION
  bt_sd_extract_srvc_cls

DESCRIPTION
  Extract the service class from the given service attribute.  

===========================================================================*/
bt_sd_uuid_type bt_sd_extract_srvc_cls( bt_sd_srv_attr_rec_type* attrib_ptr )
{
  bt_sd_uuid_type uuid = 0xFFFF;

  /* The first service class on the list is what we want */
  if ( attrib_ptr->attr_value.uuid_list.num_uuid128 > 0 )
  {
//    BT_MSG_UUID128( MSG_DEBUG, "BT SD: Service Class",
                //    &attrib_ptr->attr_value.uuid_list.uuid128[0] );
  }
  else if ( attrib_ptr->attr_value.uuid_list.num_uuid > 0 )
  {
    uuid = attrib_ptr->attr_value.uuid_list.uuid[ 0 ];
    BT_MSG_DEBUG( "BT SD: Service Class %x", uuid, 0, 0 );
  }
  return uuid;
}

/*===========================================================================

FUNCTION
  bt_sd_extract_scn

DESCRIPTION
  Extract the server channel number from the given service attribute.  

===========================================================================*/
uint8 bt_sd_extract_scn( bt_sd_srv_attr_rec_type* attrib_ptr )
{
  uint8  i;
  uint8  scn   = BT_RC_SCN_NOT_SPECIFIED;
  uint32 mask  = 0xFFFFFFFF;

  bt_sd_proto_desc_list_type* proto_desc_list_ptr = 
    &( attrib_ptr->attr_value.proto_desc_list );

  for ( i=0; i<proto_desc_list_ptr->num_proto_desc; i++ )
  {
    if ( ( proto_desc_list_ptr->proto_desc[i].uuid ==
           BT_SD_PROTOCOL_RFCOMM ) &&
         ( proto_desc_list_ptr->proto_desc[i].num_param == 1 ) )
    {
      switch( proto_desc_list_ptr->proto_desc[i].param[0].size )
      {
        case 1: /* 8-bit */
          mask = 0x000000FF;
          break;
        case 2: /* 16-bit */
          mask = 0x0000FFFF;
          break;
      }
      scn = ( proto_desc_list_ptr->proto_desc[i].param[0].value & mask );
      BT_MSG_DEBUG( "BT SD: SCN %x M %x", scn, mask, 0 );
      break;
    }
  }
  return (scn);
}


/*===========================================================================

FUNCTION
  bt_sd_extract_profile_version

DESCRIPTION
  Extract the profile version from the given service attribute.  

===========================================================================*/
uint16 bt_sd_extract_profile_version( bt_sd_srv_attr_rec_type* attrib_ptr )
{
  uint32  mask  = 0xFFFFFFFF;
  uint16  version = 0;

  bt_sd_proto_desc_list_type* proto_desc_list_ptr = 
    &( attrib_ptr->attr_value.proto_desc_list );

  if ( (proto_desc_list_ptr->num_proto_desc > 0) && 
       (proto_desc_list_ptr->proto_desc[0].num_param > 0) )
  {
    switch( proto_desc_list_ptr->proto_desc[0].param[0].size )
    {
      case 1: /* 8-bit */
        mask = 0x000000FF;
        break;
      case 2: /* 16-bit */
        mask = 0x0000FFFF;
        break;
    }
    version = ( proto_desc_list_ptr->proto_desc[0].param[0].value & mask );
    BT_MSG_DEBUG( "BT SD: Profile Ver %x M %x", version, mask, 0 );
  }
  return (version);
}

/*===========================================================================

FUNCTION
  bt_sd_parse_class_of_device

DESCRIPTION
  Extracts from the class of device its 3 components: service class, major
  device class, and minor device class.

===========================================================================*/
void bt_sd_parse_class_of_device( 
  const bt_cod_type* cod_ptr, 
  uint16*            svc_cls_ptr,
  uint16*            mjr_dev_cls_ptr,
  uint16*            mnr_dev_cls_ptr
)
{
  *svc_cls_ptr     = BT_SERVICE_CLASS_UNKNOWN;
  *mjr_dev_cls_ptr = BT_SD_MAJOR_DEVICE_CLASS_UNCLASSIFIED;
  *mnr_dev_cls_ptr = BT_SD_MINOR_DEVICE_CLASS_UNCLASSIFIED;

  /* Extract service class */
  if ( cod_ptr->cod_bytes[2] & BT_SD_MAJOR_SERVICE_CLASS_INFORMATION )
  {
    *svc_cls_ptr |= BT_SERVICE_CLASS_INFORMATION;
  }
  if ( cod_ptr->cod_bytes[2] & BT_SD_MAJOR_SERVICE_CLASS_TELEPHONY )
  {
    *svc_cls_ptr |= BT_SERVICE_CLASS_TELEPHONY;
  }
  if ( cod_ptr->cod_bytes[2] & BT_SD_MAJOR_SERVICE_CLASS_AUDIO )
  {
    *svc_cls_ptr |= BT_SERVICE_CLASS_AUDIO;
  }
  if ( cod_ptr->cod_bytes[2] & BT_SD_MAJOR_SERVICE_CLASS_OBJECT_TRANSFER )
  {
    *svc_cls_ptr |= BT_SERVICE_CLASS_OBJECT_TRANSFER;
  }
  if ( cod_ptr->cod_bytes[2] & BT_SD_MAJOR_SERVICE_CLASS_CAPTURING )
  {
    *svc_cls_ptr |= BT_SERVICE_CLASS_CAPTURING;
  }
  if ( cod_ptr->cod_bytes[2] & BT_SD_MAJOR_SERVICE_CLASS_RENDERING )
  {
    *svc_cls_ptr |= BT_SERVICE_CLASS_RENDERING;
  }
  if ( cod_ptr->cod_bytes[2] & BT_SD_MAJOR_SERVICE_CLASS_NETWORKING )
  {
    *svc_cls_ptr |= BT_SERVICE_CLASS_NETWORKING;
  }
  if ( cod_ptr->cod_bytes[2] & BT_SD_MAJOR_SERVICE_CLASS_POSITIONING )
  {
    *svc_cls_ptr |= BT_SERVICE_CLASS_POSITIONING;
  }
  if ( cod_ptr->cod_bytes[1] & 0x20 )
  {
    *svc_cls_ptr |= BT_SERVICE_CLASS_LIMITED_DISCOVERABLE_MODE;
  }

  /* Extract major and minor device class */
  *mjr_dev_cls_ptr = (cod_ptr->cod_bytes[1] & 0x1F) << 8;

  switch( *mjr_dev_cls_ptr )
  {
    case BT_SD_MAJOR_DEVICE_CLASS_COMPUTER:
    case BT_SD_MAJOR_DEVICE_CLASS_PHONE:
    case BT_SD_MAJOR_DEVICE_CLASS_AUDIO:
      *mnr_dev_cls_ptr =
        ((cod_ptr->cod_bytes[0] >> 2) & 0x3F) | *mjr_dev_cls_ptr;
      break;
    case BT_SD_MAJOR_DEVICE_CLASS_LAN_ACCESS_POINT:
      *mnr_dev_cls_ptr =
        ((cod_ptr->cod_bytes[0] >> 5) & 0x07) | *mjr_dev_cls_ptr;
      break;
    case BT_SD_MAJOR_DEVICE_CLASS_PERIPHERAL:
      break;
  }
}

/*===========================================================================

FUNCTION
  bt_sd_build_class_of_device

DESCRIPTION
  Builds the class of device from 3 components: service class, major
  device class, and minor device class.

===========================================================================*/
void bt_sd_build_class_of_device( 
  uint16       svc_cls,
  uint16       mjr_dev_cls,
  uint16       mnr_dev_cls,
  bt_cod_type* cod_ptr
)
{
  cod_ptr->cod_bytes[0] = 0;
  cod_ptr->cod_bytes[1] = 0;
  cod_ptr->cod_bytes[2] = 0;

  if ( svc_cls & BT_SERVICE_CLASS_INFORMATION )
  {
    cod_ptr->cod_bytes[2] |= BT_SD_MAJOR_SERVICE_CLASS_INFORMATION;
  }
  if ( svc_cls & BT_SERVICE_CLASS_TELEPHONY )
  {
    cod_ptr->cod_bytes[2] |= BT_SD_MAJOR_SERVICE_CLASS_TELEPHONY;
  }
  if ( svc_cls & BT_SERVICE_CLASS_AUDIO )
  {
    cod_ptr->cod_bytes[2] |= BT_SD_MAJOR_SERVICE_CLASS_AUDIO;
  }
  if ( svc_cls & BT_SERVICE_CLASS_OBJECT_TRANSFER )
  {
    cod_ptr->cod_bytes[2] |= BT_SD_MAJOR_SERVICE_CLASS_OBJECT_TRANSFER;
  }
  if ( svc_cls & BT_SERVICE_CLASS_CAPTURING )
  {
    cod_ptr->cod_bytes[2] |= BT_SD_MAJOR_SERVICE_CLASS_CAPTURING;
  }
  if ( svc_cls & BT_SERVICE_CLASS_RENDERING )
  {
    cod_ptr->cod_bytes[2] |= BT_SD_MAJOR_SERVICE_CLASS_RENDERING;
  }
  if ( svc_cls & BT_SERVICE_CLASS_NETWORKING )
  {
    cod_ptr->cod_bytes[2] |= BT_SD_MAJOR_SERVICE_CLASS_NETWORKING;
  }
  if ( svc_cls & BT_SERVICE_CLASS_POSITIONING )
  {
    cod_ptr->cod_bytes[2] |= BT_SD_MAJOR_SERVICE_CLASS_POSITIONING;
  }
  if ( svc_cls & BT_SERVICE_CLASS_LIMITED_DISCOVERABLE_MODE )
  {
    cod_ptr->cod_bytes[1] |= 0x20;
  }

  cod_ptr->cod_bytes[1] |= (mjr_dev_cls >> 8);
  cod_ptr->cod_bytes[0] |= ((mnr_dev_cls & 0xFF) << 2);

}


#ifdef BT_SWDEV_QXDM_LOG_PER
/*===========================================================================

FUNCTION
  bt_sd_snapshot_logging_function

DESCRIPTION
  Log all the SDP related statistics.

RETURN VALUE
  None.

===========================================================================*/

void bt_sd_snapshot_logging_function( void )
{
  log_bt_sdp_type        *log_ptr;
  uint8                  i;

  for (i=0; i<BT_SD_MAX_NUM_OF_L2CAP_CONNECTION; i++)
  {
    if (bt_sd_conn_table[i].used != FALSE)
    {
      log_ptr = (log_bt_sdp_type *)log_alloc(log_bt_sdp, sizeof(log_bt_sdp_type));
      if (log_ptr != NULL)
      {
        *((bt_bd_addr_type *)log_ptr->sdp_bd_addr) = 
                             bt_sd_conn_table[i].bd_addr;
        if ( bt_sd_conn_table[i].cid_loc_initd != BT_L2_NULL_CID )
        {
          log_ptr->sdp_cid = bt_sd_conn_table[i].cid_loc_initd;
        }
        else
        {
          log_ptr->sdp_cid = bt_sd_conn_table[i].cid_rem_initd;
        }
        log_ptr->sdp_tid = bt_sd_conn_table[i].tid;
        if ( bt_sd_conn_table[i].cid_loc_initd != BT_L2_NULL_CID )
        {
          log_ptr->sdp_connected = TRUE;
        }
        else
        {
          log_ptr->sdp_connected = FALSE;
        }
        log_ptr->sdp_request_pending = 
                                 (uint8) bt_sd_conn_table[i].req_pending;
        log_ptr->sdp_bt_app_id = bt_sd_conn_table[i].bt_app_id;
        log_ptr->sdp_num_resp_recv = 
                       bt_sd_conn_table[i].num_resp_recv;

        log_commit(log_ptr);
      }
    } /* if table entry in use */
  } /* for */
}
#endif /* BT_SWDEV_QXDM_LOG_PER */

#endif /* FEATURE_BT */

