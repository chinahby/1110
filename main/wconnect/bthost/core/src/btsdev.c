/*====*====*====*====*====*====*====*====*====*====*====*====*====*====*====*

            B L U E T O O T H    S D P    M O D U L E

GENERAL DESCRIPTION
  This contains the highest level code for the Bluetooth SDP layer.

Copyright (c) 2000-2008 by QUALCOMM Incorporated. 
All Rights Reserved.
Qualcomm Confidential and Proprietary
*====*====*====*====*====*====*====*====*====*====*====*====*====*====*====*/

/*===========================================================================

                        EDIT HISTORY FOR MODULE

  This section contains comments describing changes made to the module.
  Notice that changes are listed in reverse chronological order.

  $Header: //source/qcom/qct/wconnect/bthost/core/rel/00.00.26/src/btsdev.c#1 $ 
  $DateTime: 2009/01/07 18:14:54 $
  $Author: deepikas $

  when        who  what, where, why
  ----------  ---  -----------------------------------------------------------
  2008-08-08   gs  Updated bt_sd_ev_l2_connection_failed() to ignore invalid 
                   l2cap connection failed event.
  2008-07-28   hm  Fixed potential use of uninitialized variable
  2008-07-21   gs  Fixed inconsistent error code usage in bt_sd_send_error_resp_event()
                   Fixed issue in 16/32-bit UUID parsing.
  2008-06-27   gs  Fixed potential OOB array access
  2008-03-19   hm  Added error checking

 *    #58        26 Feb  2008           JTL
 * Properly handle boundary checking in search requests. No longer
 * reject searches for having a 128-bit UUID followed by other UUIDs.
 *
 *    #57        19 Feb  2008           RS
 * Local pending request shall only be cleared or a reconnect shall be initiated
 * only if the locally initiated connection was brought down
 * 
 *    #56        14 Dec  2007           HM
 * Renamed function get_attr_type to bt_sd_get_attr_type
 *
 *    #55        03 Dec  2007           PN
 * Replaced call to bt_sd_create_xml_service_record() with 
 * call to bt_sd_create_xml_service_db().
 *
 *    #54        29 Nov  2007           HM
 * Added SD command to return service search results in XML format
 *
 *    #53        07 Nov  2007           GS
 * Added support for parsing additional HID SDP attributes.
 * Added support for canceling SD idle timer.
 *
 *    #52        26 Sep  2007           SSK
 * Added support for resending 'SDP Search Command' when the initial SDP search
 * Cmd fails due to L2CAP disconnect.
 *
 *    #51        18 Jun  2007           RY
 * Updated bt_sd_ev_l2_disconnected to transition to the proper connection
 * state when an active CID remains.
 *
 *    #50        15 May  2007           SSK
 * Fixed compiler warnings by eliminating 'return statements' in switch cases
 * and changing usage of error field in struct 'bt_ev_sd_error_resp_type'
 *
 *    #49        16 Feb  2007           PN
 * Fixed critical lint errors.
 *
 *    #48        21 Dec  2006           VK
 * SD module limits numbers of hops for SDP requests if "ContinuationState" 
 * in SDP responses do not change.
 *
 *    #47        15 Dec  2006           RY
 * Updated bt_sd_parse_uuid to preserve the 128-bit UUID if needed
 *
 *    #46        14 Nov  2006           RY
 * Added SD support for custom 128-bit UUID
 *
 *    #45        23 Oct 2006            JTL
 * Take into account L2CAP MTU when creating search responses.
 *
 *    #44        20 Oct 2006            RY
 * Fixed double freeing DSM from previous checkin.
 *
 *    #43        22 Sep 2006            RY
 * 1. Fixed DSM leaks when processing garbage data.
 * 2. Updated bt_sd_ev_l2_connection_failed to initialize the local CID to NULL.
 *
 *    #42        14 Sep 2006            RY
 * Updated to use CID instead of the req_pending flag to determine if the
 * L2_CONNECTED event is generated from a local or remote initiated connection.
 *
 *    #41        31 Aug 2006            RY
 * Updated to keep track of L2CAP connection state instead of just using CIDs.
 *
 *    #40        28 Jul 2006            DM
 * Changed bt_cmd_l2_write to pass NULL frame_handle.
 *
 *    #39        21 Jul 2006            RY
 * 1. Updated bt_sd_parse_proto_desc_list to initialize param32 to 0 and
 *    to better handle custom 128-bit UUID.
 * 2. Updated bt_sd_process_service_attribute_resp and
 *    bt_sd_process_service_search_attribute_resp to better handle badly
 *    formatted AttributeList(s).
 *
 *    #38        19 June 2006           c_prasad
 * Updated bt_sd_process_service_attribute_resp to send an error response in
 * case of Null pointer.
 *
 *    #37        11 May 2006            RY
 * Updated bt_sd_parse_proto_desc_list to accept 32-bit profile version
 * number to accommodate Nokia 6780 SDP implementation.
 *
 *    #36        08 Mar 2006            RY
 * Improved message logging when parsing service search pattern.
 *
 *    #35        01 Dec 2005            DSN
 * Fixed RVCT compiler error for MSM7500 bringup. 
 *
 *    #34        18 Oct 2005            RY
 * Updated bt_sd_parse_service_search_pattern, bt_sd_parse_attribute_id_list,
 * and bt_sd_ev_l2_rx_data to fix various array out-of-bound and null pointer
 * dereference problems.
 *
 *    #33        12 Sep 2005            JTL
 * Improving DSM allocation.
 *
 *    #32        07 Sep 2005            RY
 * Added string length to the HID class descriptor definition and handling.
 *
 *    #31        02 Sep 2005            RY
 * Added support for the additional protocol descriptor list and HID descriptor
 * list service attributes.
 *
 *    #30        24 Jun 2005            PN
 * Handles failed Command Done for Device Discovery and Get Name functions.
 *
 *    #29        15 Jun 2005            RY
 * Updated bt_sd_ev_rm_inquiry_result to process the Positioning service class.
 *
 *    #28        8  Apr 2005            DP
 * Make use of BT_BDA debug message macro.
 *
 *    #27        31 Jan 2005            JH
 * Modified bt_sd_parse_proto_desc_list() to allow protocol descriptors which
 *  are not in the form of an element sequence (only contains a single UUID).
 * Modified bt_sd_parse_attribute_id_list() to ignore attribute IDs over the 
 *  limit, instead of declaring a parse error.
 * Added debug prints for errors.
 *
 *    #26        11 Jan 2005            JH
 * Modified bt_sd_parse_uuid() and bt_sd_parse_uuid_list() to ignore 
 *  unrecognized 128-bit UUIDs, instead of treating them as parse error.
 *
 *    #25        30 Dec 2004            JH
 * Added support for parsing 128-bit unsigned primitive and BPP printer specific
 *  attributes.
 *
 *    #24        29 Dec 2004            JH
 * Added support for parsing the additional protocol descriptor lists attribute
 *  and extracting the SCN of the secondary connection from it.
 *
 *    #23        01 Nov 2004            JH
 * Modified bt_sd_parse_proto_desc_uint_list_param so that the uint list is no
 *  longer stored in the service search result database.
 *
 *    #22        27 Sep 2004            JH
 * 1. Fixed incorrect while condition in bt_sd_parse_uuid_list introduced in 
 *    version #20.
 * 2. Added bt_sd_parse_proto_desc_uint_list_param and modified 
 *    bt_sd_parse_proto_desc_list to handle protocol descriptor parameters which 
 *    are in the form of a data element sequence, e.g. protocol descriptor of 
 *    BNEP.
 *
 *    #21        24 Sep 2004            DP
 * Reworked AMS compilation controls.
 *
 *    #20        30 Aug 2004            JH
 * Modified bt_sd_parse_service_search_pattern, bt_sd_parse_uuid, 
 *  bt_sd_parse_uuid_list, bt_sd_parse_proto_desc_list, 
 *  bt_sd_parse_attribute_value, and bt_sd_add_attribute_to_dsm_buffer to 
 *  support 128-bit UUIDs.
 *
 *    #18-19     2  Jun 2004            DP
 * Implemented idle timeout disconnect for SDP connections initiated
 * by remote devices.
 *
 *    #17        30 Apr PDT             RY
 * Added the support of 128-bit UUID for SyncML.
 *
 *    #16        14 Apr 2004            RY
 * Added bt_sd_convert_16_to_128_bit_uuid.
 *
 *    #15        30 Mar 2004            DP
 * Added EMS support.
 *
 *    #14        27 Jan 2004            RY
 * Added support for BT_SD_ATTR_TYPE_UUID.
 *
 *    #13        14 Jan 2004            DP
 * Added support for limited discoverable scanning.
 *
 *    #12        19 Dec 2003            DP
 * Improved error handling when building/parsing SDP database records.
 *
 *    #11        3  Dec 2003            PN
 * Added code to handle BT_EV_RM_DEVICE_UPDATED.
 *
 *    #10        26 Sep 2003            DP
 * Made service name character string.
 *
 *    #9         9  Sep 2003            RY
 * 1. Updated bt_sd_ev_l2_connected to check for remote-initiated L2CAP
 *    connection.
 * 2. Updated bt_sd_ev_l_disconnected to check for 'cid2' disconnection.
 *
 *    #8         4  Sep 2003            RY
 * 1. Removed ASSERT from bt_sd_parse_service_search_pattern.
 * 2. Updated bt_sd_parse_attribute_id_list to extract bytes only for the
 *    valid data sizes.
 * 3. Updated bt_sd_parse_text_string to check for byte array oveflow.
 * 4. Updated bt_sd_ev_l2_disconnected to generate a SDP Error Response
 *    event if there is a pending SDP request.
 * 
 *    #7         03 Jul 2003            RY
 * Added the support for uint64 and unsigned integer list.
 *
 *    Rev 1.20   12 Aug 2002 18:51:28   ryuen
 * Updated bt_sd_parse_uuid_list to support 32 and 128-bit UUID
 * 
 *    Rev 1.19   21 Feb 2002 19:16:44   ryuen
 * Fixed compiler warning
 * 
 *    Rev 1.18   21 Feb 2002 17:20:42   ryuen
 * 1. Updated bt_sd_send_sa_or_ssa_resp and
 *    bt_sd_process_service_search_attribute_req to dynamically decide how
 *    many bytes is to be used for the data size for AttributeLists
 * 2. Updated bt_sd_process_*_resp to check for the return value from
 *    bt_sd_alloc_service_record
 * 3. Updated bt_sd_parse_attribute_list to check for the return value from
 *    bt_sd_service_record_add_attribute
 * 
 *    Rev 1.17   20 Nov 2001 16:29:38   ryuen
 * 1. Replaced all memcpy with bt_sd_dsm_push*.  This fixes the data abort
 *    problem (CR18740)
 * 2. Updated bt_sd_process_*_req to check for invalid PDU length before
 *    parsing (CR18185)
 * 
 *    Rev 1.16   26 Sep 2001 12:23:58   waynelee
 * Completed BT_MSG_API_[PYLD] msgs for SD.
 * 
 *    Rev 1.15   14 Sep 2001 01:29:36   ryuen
 * 1. Updated to use the new error code enumerations
 * 2. Updated bt_sd_process_*_resp to check for invalid PDU length before
 *    parsing
 * 3. Fixed memory corruption problem when processing SA and SSA response
 * 
 *    Rev 1.14   07 Sep 2001 16:06:48   propach
 * Optimized event handling.
 * 
 *    Rev 1.13   29 Aug 2001 11:57:52   propach
 * Merged certification tip onto main tip.
 * 
 *    Rev 1.12.1.5   15 Aug 2001 20:00:36   ryuen
 * Updated bt_sd_ev_l2_connection_failed to not report an error even if
 * the connection entry is not found.  This is because SDP can receive a
 * L2CAP Connection Failed event prior to receiving a L2CAP Connected
 * event since SDP registers with L2CAP upon powerup
 * 
 *    Rev 1.12.1.4   13 Aug 2001 14:59:06   ryuen
 * Fixed compilation warning
 * 
 *    Rev 1.12.1.3   13 Aug 2001 14:37:50   ryuen
 * 1. Updated all bt_sd_process_*_resp to check for pending request and clear
 *    the request timeout timer
 * 2. Updated all bt_sd_process_*_resp but bt_sd_process_error_resp to set
 *    the idle timer and clear the pending request
 * 3. Added bt_sd_ev_sd_service_search_resp to handle the SD_SERVICE_SEARCH_RESP
 *    event
 * 4. Added bt_sd_clear_pending_request to properly clean up the DSM
 * 5. Updated bt_sd_send_error_resp_event to clear the pending request and
 *    set the idle timer
 * 6. Added bt_sd_send_timeout_resp_event
 * 7. Updated bt_sd_process_event to handle the SD_SERVICE_SEARCH, SD_ERROR,
 *    and SD_TIMEOUT events
 * 
 *    Rev 1.12.1.2   19 Jul 2001 00:12:38   ryuen
 * Updated to log the SDP Error Response to QXDM
 * 
 *    Rev 1.12.1.1   15 Jul 2001 14:18:54   ryuen
 * Updated bt_sd_send_service_search_resp and bt_sd_send_sa_or_ssa_resp
 * to log the SDP resposne to QXDM
 * 
 *    Rev 1.12.1.0   29 Jun 2001 15:25:44   waynelee
 * No change.
 * 
 *    Rev 1.12   22 May 2001 17:49:44   ryuen
 * Updated bt_sd_ev_rm_remote_name_res to set the reason field of the
 * SD_DEVICE_NAME_RESP event
 * 
 *    Rev 1.11   22 May 2001 17:22:28   ryuen
 * 1. Updated bt_sd_sig_rx_logging_function to call log_alloc with just the
 *    right amount of bytes to log, instead of with the size of the entire
 *    log buffer
 * 2. Updated bt_sd_ev_rm_remote_name_res to set the status field of the
 *    SD_DEVICE_NAME_RESP event
 * 
 *    Rev 1.10   15 May 2001 10:42:20   ryuen
 * Fixed bt_sd_convert_128_to_16_bit_uuid
 * 
 *    Rev 1.9   26 Apr 2001 23:34:36   ryuen
 * 1. Updated to comply with the 1.1 clarification that the attribute
 *    data contained in each response PDU does not exceed the
 *    MaximumAttributeByteCount parameter in the request messages
 * 2. Added BT_MSG where appropriate
 * 
 *    Rev 1.8   29 Mar 2001 22:26:56   ryuen
 * Properly cleaned up the SDP connection entry after the receipt of
 * SDP response with no continuation.  This allows for sending multiple
 * SDP requests without having to reset the phone
 * 
 *    Rev 1.7   28 Mar 2001 22:47:40   ryuen
 * Added the support for 128-bit UUID
 * 
 *    Rev 1.6   21 Mar 2001 23:32:10   ryuen
 * Simplified handling of continuation responses by concatenating them into
 * one response, which only get parsed upon the receipt of all responses
 * 
 *    Rev 1.5   16 Mar 2001 20:59:28   ryuen
 * Added bt_sd_find_discovered_device()
 * 
 *    Rev 1.4   05 Mar 2001 17:04:14   ryuen
 * Added bt_sd_ev_l2_connection_failed to handle the L2CAP Connection
 * Failed event
 * 
 *    Rev 1.3   02 Mar 2001 01:16:12   ryuen
 * Updated bt_sd_process_event to process the CMD_DONE event
 * 
 *    Rev 1.2   23 Feb 2001 15:32:48   ryuen
 * Fixed bt_sd_send_service_attribute_resp to insert the continuation state
 * parameter even for an empty attribute list
 * 
 *    Rev 1.1   07 Feb 2001 10:18:38   propach
 * Reworked internal BT application IDs and made event control
 * route based on these.
 * 
 *    Rev 1.0   29 Jan 2001 17:10:18   propach
 * Initial revision.
 * 
 *    Rev 1.24   08 Jan 2001 14:57:58   ryuen
 * Updated bt_sd_ev_rm_inquiry_result to process an inquiry result with
 * service class set to 0
 * 
 *    Rev 1.23   21 Dec 2000 17:48:18   waynelee
 * Removed T_CORE_EMUL.
 * 
 *    Rev 1.22   11 Dec 2000 18:52:18   amitj
 * minor format changes
 * 
 *    Rev 1.21   11 Dec 2000 17:51:56   amitj
 * Removed while loop in logging_function(); Replaced hardcoded values with
 * literals in logging_function()
 * 
 *    Rev 1.20   30 Nov 2000 22:08:10   ryuen
 * Added bt_sd_ev_rm_remote_name_res to process BT_EV_RM_REMOTE_NAME_RES
 * 
 *    Rev 1.19   16 Nov 2000 17:55:40   amitj
 * Made a fix to break if log_alloc returns NULL for QXDM logging
 * 
 *    Rev 1.18   14 Nov 2000 19:08:02   amitj
 * Made some corrections to QXDM logging function
 * 
 *    Rev 1.17   13 Nov 2000 17:01:18   amitj
 * Moved the bt_sd_sig_rx_logging_function() from bottom to the top of the file
 * 
 *    Rev 1.16   13 Nov 2000 15:17:42   ryuen
 * 1. Updated bt_sd_ev_rm_inquiry_result to extract the correct service
 *    class bits
 * 2. Added the processing of the Radio Manager Inquiry Complete event
 * 
 *    Rev 1.15   08 Nov 2000 13:16:52   amitj
 * Defined bt_sd_sig_rx_logging_function() and invoked it in rx_data()
 * 
 *    Rev 1.14   03 Nov 2000 11:46:26   ryuen
 * 1. Added support for L2CAP data that spans multiple DSM items, i.e. a chained
 *    DSM packet.  This is done by passing reference to DSM pointer instead of
 *    just the DSM pointer itself wherever appropriate
 * 2. Updated bt_sd_parse_proto_desc_list to use the right data size for
 *    adding protocol-specific parameter
 * 3. Updated bt_sd_parse_text_string to null-terminate the text string
 * 4. Implemented bt_sd_process_error_resp
 * 5. Updated bt_sd_process_service_search_attribute_resp to correctly parse
 *    continuation response
 * 
 *    Rev 1.13   30 Oct 2000 19:28:36   ryuen
 * Added declaration for bt_sd_dev_discv_db
 * 
 *    Rev 1.12   30 Oct 2000 18:14:30   ryuen
 * 1. Implemented bt_sd_process_service_search_resp and
 *    bt_sd_process_service_attribute_resp
 * 2. Created utility routine bt_sd_parse_attribute_value
 * 3. Added bt_sd_ev_rm_inquiry_result
 * 
 *    Rev 1.11   12 Sep 2000 14:26:40   ryuen
 * Added debug MSGs
 * 
 *    Rev 1.10   30 Aug 2000 09:42:56   waynelee
 * Made changes for new BT_MSG macros
 * 
 *    Rev 1.9   29 Aug 2000 08:26:28   propach
 * Moved comdef.h includes above FEATURE_BT to remove compiler
 * warning when feature is disabled.
 *
 *    Rev 1.8   17 Aug 2000 13:47:26   propach
 * Added Event Control (EC).  Fixed minor problems in RFCOMM.
 *
 *    Rev 1.7   14 Aug 2000 18:03:00   ryuen
 * Fixed typo - 'ev_sd_service_seach_resp' to 'ev_sd_service_search_resp'
 *
 *    Rev 1.6   09 Aug 2000 13:33:14   ryuen
 * Fixed bt_sd_send_service_search_attribute_resp() to report the correct
 * PDU parameter length
 *
 *    Rev 1.5   08 Aug 2000 10:43:28   ryuen
 * 1. Fixed a bug in bt_sd_send_service_attribute_resp where we're reporting
 *    1 byte more than we should
 * 2. Updated bt_sd_send_service_search_attribute_resp to use 32-bit service
 *    record handle
 * 3. Fixed bt_sd_parse_unsigned_primitive() to set param_ptr correctly
 *
 *    Rev 1.4   03 Aug 2000 14:57:34   ryuen
 * Minor bug fixes
 *
 *    Rev 1.3   02 Aug 2000 13:04:32   ryuen
 * Added support for sending SDP Service Search Attribute Request
 *
 *    Rev 1.2   02 Aug 2000 11:51:36   propach
 * Made standalone BT compile.
 *
 *    Rev 1.1   31 Jul 2000 22:19:38   ryuen
 * Bug fixes for getting the continuation mechanism to work in
 * SDP Service Search Attribute Request
 *    Rev 1.0   31 Jul 2000 11:55:16   ryuen
 * Initial revision.

===========================================================================*/

/*===========================================================================

                     INCLUDE FILES FOR MODULE

===========================================================================*/

#include "customer.h"
#include "comdef.h"
#ifdef FEATURE_BT
#include <string.h>
#include "queue.h"
#include "dsm.h"
#include "bt.h"
#include "bti.h"
#include "btcmdi.h"
#include "btmsg.h"
#include "btsd.h"
#include "btsdxml.h"

#if defined (BT_SWDEV_QXDM_LOG_MSG) || defined (BT_SWDEV_QXDM_LOG_PER)
#include "btlogi.h"
#include "log.h"
#endif

#include "assert.h"
#include "msg.h"


#define BT_MSG_LAYER  BT_MSG_SD   /* necessary for btmsg.h */

/*===========================================================================

                DEFINITIONS AND DECLARATIONS FOR MODULE

This section contains definitions for constants, macros, types, variables
and other items needed by this module.

===========================================================================*/

/*-------------------------------------------------------------------------*/
/*                                                                         */
/*                         Data Declarations                               */
/*                                                                         */
/*-------------------------------------------------------------------------*/

typedef struct
{
  bt_sd_data_element_enum_type   type_desc;
  uint32                         data_size;
  uint16                         byte_parsed;
} bt_sd_data_element_header_parse_result_type;


/*-------------------------------------------------------------------------*/
/*                                                                         */
/*                         Data Definitions                                */
/*                                                                         */
/*-------------------------------------------------------------------------*/

bt_sd_conn_entry_type bt_sd_conn_table[ BT_SD_MAX_NUM_OF_L2CAP_CONNECTION ];
bt_sd_device_discovery_db_type bt_sd_dev_discv_db;
bt_sd_data_type bt_sd_data;
LOCAL dsm_item_type* attr_list_dsm_ptr;
LOCAL uint8 bt_sd_base_uuid[16] = { 0x00, 0x00, 0x00, 0x00,
                                    0x00, 0x00, 0x10, 0x00,
                                    0x80, 0x00, 0x00, 0x80,
                                    0x5F, 0x9B, 0x34, 0xFB };

bt_sd_hid_host_desc_list_type bt_sd_host_desc_list[BT_SD_MAX_HID_HOST_DESC_ATTR];
uint16 hid_desc_index = 0;
/*-------------------------------------------------------------------------*/
/*                                                                         */
/*                         Static Functions                                */
/*                                                                         */
/*-------------------------------------------------------------------------*/

#ifdef BT_SWDEV_QXDM_LOG_MSG
/*===========================================================================

FUNCTION
  bt_sd_sig_rx_logging_function

DESCRIPTION
  Log all the bytes in the SDP signalling packet received.

RETURN VALUE
  None.

===========================================================================*/
void bt_sd_sig_rx_logging_function
(
  dsm_item_type   *dsm_ptr,
  word            cid
)
{
  log_bt_sd_sig_rx_type   *log_ptr;
  uint16                  data_length;

  data_length = dsm_length_packet( dsm_ptr );
  data_length = MIN( data_length, BT_SD_RX_LOG_BUFFER_SIZE );

  log_ptr = (log_bt_sd_sig_rx_type  *)log_alloc( log_bt_sd_sig_rx, 
                       sizeof( log_bt_sd_sig_rx_type ) -
                       BT_SD_RX_LOG_BUFFER_SIZE + data_length );

  if ( log_ptr != NULL )
  {
    log_ptr->length = data_length;
    log_ptr->local_cid = (uint16) cid;

    dsm_extract( dsm_ptr, BT_FIRST_BYTE_OFFSET, (void *) log_ptr->buffer, 
                 data_length );

    log_commit( log_ptr );
  }
}
#endif /* BT_SWDEV_QXDM_LOG_MSG */

/*===========================================================================

FUNCTION
  bt_sd_dsm_pullup

DESCRIPTION
  Do a regular dsm_pullup and make sure we get what we ask for

===========================================================================*/

boolean bt_sd_dsm_pullup
(
  dsm_item_type  **bph,            /* Address of ptr to head of packet     */
  void           *buf,             /* dest. ptr, can be NULL to avoid copy */
  word           cnt               /* the byte count to pull from pkt head */
)
{
  if ( dsm_pullup( bph, buf, cnt ) != cnt )
  {
    return FALSE;
  }

  return TRUE;
}


/*===========================================================================

FUNCTION
  bt_sd_dsm_pushdown_tail

DESCRIPTION
  Do a dsm_pushdown_tail and make sure we get what we ask for

===========================================================================*/

boolean bt_sd_dsm_pushdown_tail
(
  dsm_item_type  **bph,            /* Address of ptr to head of packet     */
  uint8          *buf,             /* dest. ptr, can be NULL to avoid copy */
  word           cnt,              /* the byte count to pull from pkt head */
  dsm_mempool_id_enum_type  dsm_pool_id
)
{
  if ( bt_dsm_pushdown_tail( bph, buf, cnt, dsm_pool_id ) != cnt )
  {
    return FALSE;
  }

  return TRUE;
}


/*===========================================================================

FUNCTION
  bt_sd_dsm_push8

DESCRIPTION
  Do a dsm_pushdown_tail of 1 byte

===========================================================================*/

boolean bt_sd_dsm_push8
(
  dsm_item_type  **bph,            /* Address of ptr to head of packet     */
  uint8          *buf,             /* dest. ptr, can be NULL to avoid copy */
  dsm_mempool_id_enum_type  dsm_pool_id
)
{
  if ( bt_dsm_pushdown_tail( bph, buf, 1, dsm_pool_id ) != 1 )
  {
    return FALSE;
  }

  return TRUE;
}


/*===========================================================================

FUNCTION
  bt_sd_dsm_push16

DESCRIPTION
  Do a dsm_pushdown_tail of 2 bytes

===========================================================================*/

boolean bt_sd_dsm_push16
(
  dsm_item_type  **bph,            /* Address of ptr to head of packet     */
  uint16         *buf,             /* dest. ptr, can be NULL to avoid copy */
  dsm_mempool_id_enum_type  dsm_pool_id
)
{
  if ( bt_dsm_pushdown_tail( bph, buf, 2, dsm_pool_id ) != 2 )
  {
    return FALSE;
  }

  return TRUE;
}


/*===========================================================================

FUNCTION
  bt_sd_dsm_push32

DESCRIPTION
  Do a dsm_pushdown_tail of 4 bytes

===========================================================================*/

boolean bt_sd_dsm_push32
(
  dsm_item_type  **bph,            /* Address of ptr to head of packet     */
  uint32         *buf,             /* dest. ptr, can be NULL to avoid copy */
  dsm_mempool_id_enum_type  dsm_pool_id
)
{
  if ( bt_dsm_pushdown_tail( bph, buf, 4, dsm_pool_id ) != 4 )
  {
    return FALSE;
  }

  return TRUE;
}


/*===========================================================================

FUNCTION
  bt_sd_dsm_push64

DESCRIPTION
  Do a dsm_pushdown_tail of 8 bytes

===========================================================================*/

boolean bt_sd_dsm_push64
(
  dsm_item_type  **bph,            /* Address of ptr to head of packet     */
  uint64         *buf,             /* dest. ptr, can be NULL to avoid copy */
  dsm_mempool_id_enum_type  dsm_pool_id
)
{
  if ( bt_dsm_pushdown_tail( bph, buf, 8, dsm_pool_id ) != 8 )
  {
    return FALSE;
  }

  return TRUE;
}


/*===========================================================================

FUNCTION
  bt_sd_get_attr_type

DESCRIPTION
  Lookup the attribute type given a particula attribute ID

===========================================================================*/
bt_sd_srv_attr_enum_type bt_sd_get_attr_type( uint16 attr_id )
{

 bt_sd_srv_attr_enum_type bt_sd_att_type;
 
  switch( attr_id )
  {
    case BT_SD_ATTR_ID_SERVICE_AVAILABILITY:
    case BT_SD_ATTR_ID_SUPPORTED_CAPABILITIES:
    case BT_SD_ATTR_ID_HID_COUNTRY_CODE:
    case BT_SD_ATTR_ID_HID_DEV_SUB_CLASS:
      bt_sd_att_type = BT_SD_ATTR_TYPE_UINT8;
      break;
    case BT_SD_ATTR_ID_SUPPORTED_FEATURES:
    case BT_SD_ATTR_ID_MAX_MEDIA_WIDTH:
    case BT_SD_ATTR_ID_MAX_MEDIA_LENGTH:
    case BT_SD_ATTR_ID_HID_PARSER_VER :
    case BT_SD_ATTR_ID_HID_PROFILE_VERSION :
      bt_sd_att_type = BT_SD_ATTR_TYPE_UINT16;
      break;
    case BT_SD_ATTR_ID_SERVICE_RECORD_HANDLE:
    case BT_SD_ATTR_ID_SERVICE_RECORD_STATE:
    case BT_SD_ATTR_ID_SERVICE_INFO_TIME_TO_LIVE:
    case BT_SD_ATTR_ID_SUPPORTED_FUNCTIONS:
      bt_sd_att_type = BT_SD_ATTR_TYPE_UINT32;
      break;
    case BT_SD_ATTR_ID_TOTAL_IMAGING_DATA_CAPACITY:
      bt_sd_att_type = BT_SD_ATTR_TYPE_UINT64;
      break;
    case BT_SD_ATTR_ID_SERVICE_SERVICE_ID:
      bt_sd_att_type = BT_SD_ATTR_TYPE_UUID128;
      break;
    case BT_SD_ATTR_ID_SERVICE_CLASS_ID_LIST:
    case BT_SD_ATTR_ID_BROWSE_GROUP_LIST:
      bt_sd_att_type = BT_SD_ATTR_TYPE_UUID_LIST;
      break;
    case BT_SD_ATTR_ID_PROTOCOL_DESCRIPTOR_LIST:
    case BT_SD_ATTR_ID_BLUETOOTH_PROFILE_DESCRIPTOR_LIST:
      bt_sd_att_type = BT_SD_ATTR_TYPE_PROTO_DESC_LIST;
      break;
    case BT_SD_ATTR_ID_LANGUAGE_BASE_ATTRIBUTE_ID_LIST:
      bt_sd_att_type = BT_SD_ATTR_TYPE_LANG_BASE_ATTR_ID_LIST;
      break;
    case BT_SD_ATTR_ID_DOCUMENTATION_URL:
    case BT_SD_ATTR_ID_CLIENT_EXECUTABLE_URL:
    case BT_SD_ATTR_ID_ICON_URL:
    case BT_SD_ATTR_ID_SERVICE_NAME:
    case BT_SD_ATTR_ID_SERVICE_DESCRIPTION:
    case BT_SD_ATTR_ID_PROVIDER_NAME:
    case BT_SD_ATTR_ID_DOCUMENT_FORMATS_SUPPORTED:
    case BT_SD_ATTR_ID_XHTML_PRINT_IMAGE_FORMATS_SUPPORTED:
    case BT_SD_ATTR_ID_1284ID:
    case BT_SD_ATTR_ID_PRINTER_NAME:
    case BT_SD_ATTR_ID_PRINTER_LOCATION:
    case BT_SD_ATTR_ID_MEDIA_TYPES_SUPPORTED:
    case BT_SD_ATTR_ID_DEVICE_NAME:
      bt_sd_att_type = BT_SD_ATTR_TYPE_STRING;
      break;
    case BT_SD_ATTR_ID_SUPPORTED_FORMATS_LIST:
      bt_sd_att_type = BT_SD_ATTR_TYPE_UINT_LIST;
      break;
    case BT_SD_ATTR_ID_COLOR_SUPPORTED:
    case BT_SD_ATTR_ID_DUPLEX_SUPPORTED:
    case BT_SD_ATTR_ID_ENHANCED_LAYOUT_SUPPORTED:
    case BT_SD_ATTR_ID_HID_VIRTUAL_CABLE:
    case BT_SD_ATTR_ID_HID_RECONNECT_INITIATE:
    case BT_SD_ATTR_ID_HID_SDP_DISABLE :
    case BT_SD_ATTR_ID_HID_BAT_POWER :
    case BT_SD_ATTR_ID_HID_REMOTE_WAKE :
    case BT_SD_ATTR_ID_HID_NORM_CONNECT:
    case BT_SD_ATTR_ID_HID_BOOT_DEVICE :
      bt_sd_att_type = BT_SD_ATTR_TYPE_BOOL;
      break;
    case BT_SD_ATTR_ID_ADDITIONAL_PROTOCOL_DESCRIPTOR_LISTS:
      bt_sd_att_type = BT_SD_ATTR_TYPE_ADD_PROTO_DESC_LISTS;
      break;
    case BT_SD_ATTR_ID_HID_DESCRIPTOR_LIST:
      bt_sd_att_type = BT_SD_ATTR_TYPE_HID_DESC_LIST;
      break;
    case BT_SD_ATTR_ID_CHARACTER_REPERTOIRES_SUPPORTED:
      bt_sd_att_type = BT_SD_ATTR_TYPE_UINT128;
      break;
    default:
      bt_sd_att_type = BT_SD_ATTR_TYPE_UNKNOWN;
      break;
  }
  
  return bt_sd_att_type;
}


/*===========================================================================

FUNCTION
  bt_sd_parse_data_element_header

DESCRIPTION
  Extract the type descriptor and data size from a data element header

===========================================================================*/

boolean bt_sd_parse_data_element_header(
  dsm_item_type**                                dsm_ptr_ptr,
  bt_sd_data_element_header_parse_result_type*   parse_result_ptr
)
{
  uint8     header;   /* Data element header */
  uint8     size_index;
  uint8     uint8_data_size;
  uint16    uint16_data_size;
  uint32    uint32_data_size;
  boolean   status       = TRUE;
  boolean   stop_parsing = FALSE;

  /* Get the first 8 bits */
  if ( ! bt_sd_dsm_pullup( dsm_ptr_ptr, (void *) &header, 1 ) )
  {
    BT_ERR( "BT SD: bt_sd_dsm_pullup failed", 0, 0, 0 );
    return FALSE;
  }
  parse_result_ptr->byte_parsed = 1;

  /* The upper 5 bits are the type descriptor */
  parse_result_ptr->type_desc = ( bt_sd_data_element_enum_type )
                                                     ( ( header >> 3 ) & 0x1F );
  /* The lower 3 bits are the size index */
  size_index = header & 0x07;

  switch( parse_result_ptr->type_desc )
  {
    case BT_SD_DATA_ELEMENT_NULL:
    case BT_SD_DATA_ELEMENT_BOOL:
      if ( size_index != 0 )
      {
        BT_MSG_HIGH( "BT SD: Invalid size index %d for type %d",
                     size_index, parse_result_ptr->type_desc, 0 );
        status = FALSE;
        break;
      }
      parse_result_ptr->data_size = 0; /* Don't care about the size field */
      break;
    case BT_SD_DATA_ELEMENT_UNSIGNED_INT:
    case BT_SD_DATA_ELEMENT_SIGNED_INT:
      if ( size_index > 4 )
      {
        BT_MSG_HIGH( "BT SD: Invalid size index %d for type %d",
                     size_index, parse_result_ptr->type_desc, 0 );
        status = FALSE;
        break;
      }
      parse_result_ptr->data_size = bt_sd_data_element_size_desc[ size_index ];
      break;
    case BT_SD_DATA_ELEMENT_UUID:
      if ( ( size_index != 1 ) && ( size_index != 2 ) && ( size_index != 4 ) )
      {
        BT_MSG_HIGH( "BT SD: Invalid size index %d for type %d",
                     size_index, parse_result_ptr->type_desc, 0 );
        status = FALSE;
        break;
      }
      parse_result_ptr->data_size = bt_sd_data_element_size_desc[ size_index ];
      break;
    case BT_SD_DATA_ELEMENT_TEXT_STRING:
    case BT_SD_DATA_ELEMENT_SEQUENCE:
    case BT_SD_DATA_ELEMENT_ALTERNATIVE:
    case BT_SD_DATA_ELEMENT_URL:
      if ( size_index < 5 )
      {
        BT_MSG_HIGH( "BT SD: Invalid size index %d for type %d",
                     size_index, parse_result_ptr->type_desc, 0 );
        status       = FALSE;
        stop_parsing = TRUE;
        break;
      }
      /* Fix endian-ness */
      switch( bt_sd_data_element_size_desc[ size_index ] )
      {
        case 1: /* 1 byte */
          if ( ! bt_sd_dsm_pullup( dsm_ptr_ptr, (uint8*) &uint8_data_size, 1 ) )
          {
            BT_ERR( "BT SD: bt_sd_dsm_pullup failed", 0, 0, 0 );
            status       = FALSE;
            stop_parsing = TRUE;
          }
          else
          {
            parse_result_ptr->data_size = uint8_data_size;
          }
          break;
        case 2: /* 2 bytes */
          if ( ! bt_sd_dsm_pullup( dsm_ptr_ptr, (uint8*) &uint16_data_size, 2 ) )
          {
            BT_ERR( "BT SD: bt_sd_dsm_pullup failed", 0, 0, 0 );
            status       = FALSE;
            stop_parsing = TRUE;
          }
          else
          {
            parse_result_ptr->data_size = SWAB16( uint16_data_size );
          }
          break;
        case 4: /* 4 bytes */
          if ( ! bt_sd_dsm_pullup( dsm_ptr_ptr, (uint8*) &uint32_data_size, 4 ) )
          {
            BT_ERR( "BT SD: bt_sd_dsm_pullup failed", 0, 0, 0 );
            status       = FALSE;
            stop_parsing = TRUE;
          }
          else
          {
            parse_result_ptr->data_size = SWAB32( uint32_data_size );
          }
          break;
        default:
          BT_ERR( "BT SD: Bad elem size %x I %x",
                  bt_sd_data_element_size_desc[ size_index ],
                  size_index, 0 );
          status       = FALSE;
          stop_parsing = TRUE;
          break;
      }
      if ( stop_parsing == FALSE )
      {
        parse_result_ptr->byte_parsed +=
          bt_sd_data_element_size_desc[ size_index ];
      }
      break;
    default:
      BT_MSG_HIGH( "BT SD: Invalid type descriptor %d",
                   parse_result_ptr->type_desc, 0, 0 );
      status = FALSE;
      break;
  }

  return status;
}


/*===========================================================================

FUNCTION
  bt_sd_convert_128_to_16_bit_uuid

DESCRIPTION
  Convert a 128-bit to 16-bit UUID

===========================================================================*/

bt_sd_uuid_type bt_sd_convert_128_to_16_bit_uuid
(
  const bt_sd_uuid128_type*  uuid128_ptr
)
{
  uint16              i;
  bt_sd_uuid_type     uuid = 0;
  bt_sd_uuid128_type  uuid128 = *uuid128_ptr;
  
  /* Subtract Base UUID */
  for ( i = 0; i < 16; i++ )
  {
    uuid128.uuid_byte[i] -= bt_sd_base_uuid[i];

    if ( ( i > 3 ) && ( uuid128.uuid_byte[i] != 0 ) )
    {
      uuid = BT_SD_INVALID_UUID;
      break;
    }
  }

  if ( uuid != BT_SD_INVALID_UUID )
  {
    uuid = ( uuid128.uuid_byte[2] << 8 ) | uuid128.uuid_byte[3];
  }

  return uuid;
}


/*===========================================================================

FUNCTION
  bt_sd_convert_16_to_128_bit_uuid

DESCRIPTION
  Convert a 16-bit to 128-bit UUID

===========================================================================*/

void bt_sd_convert_16_to_128_bit_uuid( uint8* uuid128, bt_sd_uuid_type uuid16 )
{
  uint16 i;

  *uuid128 = *(uuid128 + 1) = 0;
  *(uuid128 + 2) = (uint8)((uuid16 >> 8) & 0x00FF);
  *(uuid128 + 3) = (uint8)(uuid16 & 0x00FF);

  /* Add Base UUID */
  for ( i = 4; i < 16; i++ )
  {
    *(uuid128 + i) = bt_sd_base_uuid[i];
  }

  return;
}

/*===========================================================================

FUNCTION
  bt_sd_convert_32_to_128_bit_uuid

DESCRIPTION
  Convert a 32-bit to 128-bit UUID

===========================================================================*/

LOCAL void bt_sd_convert_32_to_128_bit_uuid( uint8* uuid128, uint32 uuid32 )
{
  uint16 i;

  *uuid128 = *(uuid128 + 1) = 0;
  *(uuid128 + 2) = (uint8)((uuid32 >> 24) & 0x000000FF);
  *(uuid128 + 3) = (uint8)((uuid32 >> 16) & 0x000000FF);
  *(uuid128 + 4) = (uint8)((uuid32 >> 8)  & 0x000000FF);
  *(uuid128 + 5) = (uint8)(uuid32 & 0x000000FF);

  /* Add Base UUID */
  for ( i = 6; i < 16; i++ )
  {
    *(uuid128 + i) = bt_sd_base_uuid[i];
  }

  return;
}

/*===========================================================================

FUNCTION
  bt_sd_parse_service_search_pattern

DESCRIPTION
  Parse the ServiceSearchPattern field of a SDP Service Search or
  SDP Service Search Attribute Request

===========================================================================*/

boolean bt_sd_parse_service_search_pattern(
  dsm_item_type**            dsm_ptr_ptr,
  bt_sd_srv_srch_pat_type*   srch_pat_ptr,
  uint16*                    total_byte_parsed_ptr
)
{
  uint16                                        byte_parsed = 0;
  uint32                                        data_seq_size;
  bt_sd_uuid_type                               uuid16;
  uint32                                        uuid32;
  bt_sd_uuid128_type                            uuid128;
  bt_sd_data_element_header_parse_result_type   parse_result;

  *total_byte_parsed_ptr = 0;

  if ( ! bt_sd_parse_data_element_header( dsm_ptr_ptr, &parse_result ) )
  {
    return FALSE;
  }

  if ( parse_result.type_desc != BT_SD_DATA_ELEMENT_SEQUENCE )
  {
    BT_MSG_HIGH( "BT SD: Unexpected type descriptor %d",
                 parse_result.type_desc, 0, 0 );
    return FALSE;
  }

  *total_byte_parsed_ptr += parse_result.byte_parsed;
  data_seq_size = parse_result.data_size;

  /* Initialize the local service search pattern structure */
  srch_pat_ptr->num_uuid    = 0;
  srch_pat_ptr->num_uuid128 = 0;
  srch_pat_ptr->syncml_flag = BT_SD_DB_NOT_SYNCML_V;

  while ( byte_parsed < data_seq_size )
  {
    if ( ! bt_sd_parse_data_element_header( dsm_ptr_ptr, &parse_result ) )
    {
      return FALSE;
    }

    if ( parse_result.type_desc != BT_SD_DATA_ELEMENT_UUID )
    {
      BT_MSG_HIGH( "BT SD: Unexpected type descriptor %d",
                   parse_result.type_desc, 0, 0 );
      return FALSE;
    }

    if( (parse_result.data_size == 16) &&
        (srch_pat_ptr->num_uuid128 >= BT_SD_MAX_NUM_OF_SRV_SRCH_PAT_UUID128) )
    {
      BT_MSG_HIGH( "BT SD: Exceeded max # of UUID128 %d",
                   srch_pat_ptr->num_uuid128, 0, 0 );
      return FALSE;
    }
    else if ( srch_pat_ptr->num_uuid >= BT_SD_MAX_NUM_OF_SRV_SRCH_PAT_UUID )
    {
      BT_MSG_HIGH( "BT SD: Exceeded max # of UUID %d",
                   srch_pat_ptr->num_uuid, 0, 0 );
      return FALSE;
    }

    byte_parsed += parse_result.byte_parsed;

    /* Fix endian-ness */
    switch( parse_result.data_size )
    {
      case 2:
        if ( bt_sd_dsm_pullup( dsm_ptr_ptr, &uuid16, 2 ) )
        {
          srch_pat_ptr->uuid[ srch_pat_ptr->num_uuid++ ] = SWAB16( uuid16 );
        }
        else
        {
          return FALSE;
        }
        break;
      case 4:
        if ( bt_sd_dsm_pullup( dsm_ptr_ptr, &uuid32, 4 ) )
        {
          uuid32 = SWAB32( uuid32 );
          srch_pat_ptr->uuid[ srch_pat_ptr->num_uuid++ ] = ( bt_sd_uuid_type ) uuid32;
        }
        else
        {
          return FALSE;
        }
        break;
      case 16:
        if ( bt_sd_dsm_pullup( dsm_ptr_ptr,
                               (uint8*) &uuid128,
                               sizeof( bt_sd_uuid128_type ) ) )
        {
          uuid16 = bt_sd_convert_128_to_16_bit_uuid( &uuid128 );
          if ( uuid16 != BT_SD_INVALID_UUID )
          {
            BT_MSG_DEBUG( "BT SD: parse SSP UUID16 %x", uuid16, 0, 0 );
            srch_pat_ptr->uuid[ srch_pat_ptr->num_uuid++ ] = uuid16;
          }
          else
          {
            BT_MSG_UUID128( MSG_DEBUG, "BT SD: parse SSP", &uuid128 );
            srch_pat_ptr->uuid128[ srch_pat_ptr->num_uuid128++ ] = uuid128;

            if ( BT_SD_UUID128_EQUAL( &uuid128,
                                      &bt_sd_syncml_server_uuid ) )
            {
              srch_pat_ptr->syncml_flag |= BT_SD_DB_SYNCML_SERVER_B;
            }
            else if ( BT_SD_UUID128_EQUAL( &uuid128,
                                           &bt_sd_syncml_client_uuid ) )
            {
              srch_pat_ptr->syncml_flag |= BT_SD_DB_SYNCML_CLIENT_B;
            }
          }
        }
        else
        {
          return FALSE;
        }
        break;
      default:
        BT_MSG_HIGH( "BT SD: Invalid data size for UUID %d",
                     parse_result.data_size, 0, 0 );
        return FALSE;
    }

    byte_parsed += parse_result.data_size;
  }

  BT_MSG_DEBUG( "BT SD: parse UUID16 %d UUID128 %d",
                srch_pat_ptr->num_uuid,
                srch_pat_ptr->num_uuid128, 0 );

  if ( byte_parsed != data_seq_size )
  {
    BT_MSG_HIGH( "BT SD: Data size mismatch %d %d",
                 byte_parsed, data_seq_size, 0 );
    return FALSE;
  }

  *total_byte_parsed_ptr += byte_parsed;

  return TRUE;
}


/*===========================================================================

FUNCTION
  bt_sd_parse_attribute_id_list

DESCRIPTION
  Parse the AttributeIDList field of a SDP Service Attribute Request

===========================================================================*/

boolean bt_sd_parse_attribute_id_list(
  dsm_item_type**            dsm_ptr_ptr,
  bt_sd_srv_srch_pat_type*   srch_pat_ptr,
  uint16*                    total_byte_parsed_ptr
)
{
  uint16                                        byte_parsed;
  uint32                                        data_seq_size;
  uint32                                        attr_id;
  bt_sd_data_element_header_parse_result_type   parse_result;

  *total_byte_parsed_ptr = 0;

  if ( ! bt_sd_parse_data_element_header( dsm_ptr_ptr, &parse_result ) )
  {
    BT_ERR( "BT SD: parse AIDL: wrong header T %x, S %x, P %x", 
            parse_result.type_desc, parse_result.data_size, 
            parse_result.byte_parsed );
    return FALSE;
  }

  if ( parse_result.type_desc != BT_SD_DATA_ELEMENT_SEQUENCE )
  {
    BT_MSG_HIGH( "BT SD: parse AIDL: unexpected type desc %d",
                 parse_result.type_desc, 0, 0 );
    return FALSE;
  }

  *total_byte_parsed_ptr += parse_result.byte_parsed;
  data_seq_size = parse_result.data_size;

  for ( srch_pat_ptr->num_attr_id = 0, byte_parsed = 0;
        byte_parsed < data_seq_size;
        srch_pat_ptr->num_attr_id++ )
  {
    /*
     * The spec does not impose a limit on the maximum number of attribute IDs
     * that can be searched, but to simplify implementation, a maximum (12)
     * is being imposed anyway. We will ignore attributes above this limit.
     */
    if ( srch_pat_ptr->num_attr_id >= BT_SD_MAX_NUM_OF_SRV_SRCH_PAT_ATTR_ID )
    {
      BT_MSG_HIGH( "BT SD: parse AIDL: exceeded max # of AID %d, dropping",
                   srch_pat_ptr->num_attr_id, 0, 0 );
    }

    if ( ! bt_sd_parse_data_element_header( dsm_ptr_ptr, &parse_result ) )
    {
      return FALSE;
    }

    if ( parse_result.type_desc != BT_SD_DATA_ELEMENT_UNSIGNED_INT )
    {
      BT_MSG_HIGH( "BT SD: parse AIDL: unexpected type desc %d",
                   parse_result.type_desc, 0, 0 );
      return FALSE;
    }

    byte_parsed += parse_result.byte_parsed;

    /* Fix endian-ness */
    switch( parse_result.data_size )
    {
      case 2:
        if ( ( bt_sd_dsm_pullup( dsm_ptr_ptr, &attr_id, 2 ) ) &&
             ( srch_pat_ptr->num_attr_id < 
               BT_SD_MAX_NUM_OF_SRV_SRCH_PAT_ATTR_ID ) )
        { 
          srch_pat_ptr->attr_id[ srch_pat_ptr->num_attr_id ].value =
            SWAB16( attr_id );
          srch_pat_ptr->attr_id[ srch_pat_ptr->num_attr_id ].is_range = FALSE;
        }
        else
        {
          return FALSE;
        }
        break;
      case 4:
        if ( ( bt_sd_dsm_pullup( dsm_ptr_ptr, &attr_id, 4 ) ) &&
             ( srch_pat_ptr->num_attr_id < 
               BT_SD_MAX_NUM_OF_SRV_SRCH_PAT_ATTR_ID ) )
        {
          srch_pat_ptr->attr_id[ srch_pat_ptr->num_attr_id ].value =
            SWAB32( attr_id );
          srch_pat_ptr->attr_id[ srch_pat_ptr->num_attr_id ].is_range = TRUE;
        }
        else
        {
          return FALSE;
        }
        break;
      default:
        BT_MSG_HIGH( "BT SD: parse AIDL: unexpected data size %d",
                     parse_result.data_size, 0, 0 );
        return FALSE;
    }

    byte_parsed += parse_result.data_size;

    BT_MSG_DEBUG( "BT SD: parse AID: 0x%x", 
                  srch_pat_ptr->attr_id[ srch_pat_ptr->num_attr_id ].value,
                  0, 0 );
  }

  if ( byte_parsed != data_seq_size )
  {
    BT_MSG_HIGH( "BT SD: parse AIDL: data size mismatch %d %d",
                 byte_parsed, data_seq_size, 0 );
    return FALSE;
  }

  /* FIXME (ryuen)
   * Verify AttributeIDList to make sure:
   *   1. Attribute IDs are listed in ascending order
   *   2. There is no duplication of attribute ID
   */

  if ( srch_pat_ptr->num_attr_id > BT_SD_MAX_NUM_OF_SRV_SRCH_PAT_ATTR_ID )
  {
    /* We received more than 12 attriibutes, and have ignored the rest. */
    srch_pat_ptr->num_attr_id = BT_SD_MAX_NUM_OF_SRV_SRCH_PAT_ATTR_ID;
  }
  *total_byte_parsed_ptr += byte_parsed;

  return TRUE;
}


/*===========================================================================

FUNCTION
  bt_sd_parse_unsigned_primitive

DESCRIPTION
  Parse an unsigned primitive - uint8, uint16, uint32, uint64, or uint128

===========================================================================*/

uint16 bt_sd_parse_unsigned_primitive(
  dsm_item_type**           dsm_ptr_ptr,
  void*                     param_ptr,
  uint8                     size
)
{
  uint16                                        tot_byte_parsed = 0;
  uint8                                         uint8_val;
  uint8*                                        uint8_ptr;
  uint16                                        uint16_val;
  uint16*                                       uint16_ptr;
  uint32                                        uint32_val;
  uint32*                                       uint32_ptr;
  uint64                                        uint64_val;
  uint64*                                       uint64_ptr;
  byte                                          uint128_val[ 16 ];
  int8                                          i;
  bt_sd_data_element_header_parse_result_type   parse_result;
  boolean                                       continue_parsing = TRUE;

  /* Parse data element header for parameter (e.g. 0x8 0x? or 0x9 0x? 0x?) */
  if ( ( ! bt_sd_parse_data_element_header( dsm_ptr_ptr, &parse_result ) ) ||
       ( parse_result.type_desc != BT_SD_DATA_ELEMENT_UNSIGNED_INT ) )
  {
    BT_ERR( "BT SD: parse UP: wrong header T %x, S %x, P %x", 
            parse_result.type_desc, parse_result.data_size, 
            parse_result.byte_parsed );
    return 0;
  }
  tot_byte_parsed += parse_result.byte_parsed;

  if ( parse_result.data_size > size )
  {
    return 0;
  }

  switch( parse_result.data_size )
  {
    case 1:
      if ( ! bt_sd_dsm_pullup( dsm_ptr_ptr, (void*) &uint8_val, 1 ) )
      {
        continue_parsing = FALSE;
      }
      else
      {
        uint8_ptr = (uint8*) param_ptr;
        *uint8_ptr = uint8_val;
      }
      break;
    case 2:
      if ( ! bt_sd_dsm_pullup( dsm_ptr_ptr, (void*) &uint16_val, 2 ) )
      {
        continue_parsing = FALSE;
      }
      else
      {
        uint16_val = SWAB16( uint16_val );
        uint16_ptr = (uint16*) param_ptr;
        *uint16_ptr = uint16_val;
      }
      break;
    case 4:
      if ( ! bt_sd_dsm_pullup( dsm_ptr_ptr, (void*) &uint32_val, 4 ) )
      {
        continue_parsing = FALSE;
      }
      else
      {
        uint32_val = SWAB32( uint32_val );
        uint32_ptr = (uint32*) param_ptr;
        *uint32_ptr = uint32_val;
      }
      break;
    case 8:
      if ( ! bt_sd_dsm_pullup( dsm_ptr_ptr, (void*) &uint64_val, 8 ) )
      {
        continue_parsing = FALSE;
      }
      else
      {
        uint64_val = SWAB64( uint64_val );
        uint64_ptr = (uint64*) param_ptr;
        *uint64_ptr = uint64_val;
      }
      break;
    case 16:
      for ( i = 15; i >= 0; i-- )
      {                     
        if ( ! bt_sd_dsm_pullup( dsm_ptr_ptr, (void*) &uint128_val[i], 1 ) )
        {           
          continue_parsing = FALSE;
          break;
        }
      }
      
      if ( continue_parsing != FALSE )
      {
        memcpy( (void*) param_ptr, (void*) uint128_val, 16 );
      }
      break;
    default:
      continue_parsing = FALSE;
      break;
  }

  if ( continue_parsing == FALSE )
  {
    return 0;
  }
  else
  {
    tot_byte_parsed += parse_result.data_size;
    return tot_byte_parsed;
  }
}


/*===========================================================================

FUNCTION
  bt_sd_parse_boolean_primitive

DESCRIPTION
  Parse an boolean primitive

===========================================================================*/
uint16 bt_sd_parse_boolean_primitive(
  dsm_item_type**           dsm_ptr_ptr,
  void*                     param_ptr,
  uint8                     size
)
{
  uint16                                        tot_byte_parsed = 0;
  uint8                                         uint8_val;
  uint8*                                        uint8_ptr;
  bt_sd_data_element_header_parse_result_type   parse_result;
  boolean                                       continue_parsing = TRUE;

  /* Parse data element header for parameter (e.g. 0x8 0x? or 0x9 0x? 0x?) */
  if ( ( ! bt_sd_parse_data_element_header( dsm_ptr_ptr, &parse_result ) ) ||
       ( parse_result.type_desc != BT_SD_DATA_ELEMENT_BOOL ) )
  {
    BT_ERR( "BT SD: parse UP: wrong header T %x, S %x, P %x", 
            parse_result.type_desc, parse_result.data_size, 
            parse_result.byte_parsed );
    return 0;
  }
  tot_byte_parsed += parse_result.byte_parsed;

  if ( parse_result.data_size > size )
  {
    return 0;
  }

  switch( parse_result.data_size )
  {
    case 0:
      if ( ! bt_sd_dsm_pullup( dsm_ptr_ptr, (void*) &uint8_val, 1 ) )
      {
        continue_parsing = FALSE;
      }
      else
      {
        uint8_ptr = (uint8*) param_ptr;
        *uint8_ptr = uint8_val;
      }
      break;
    
    default:
      continue_parsing = FALSE;
      break;
  }

  if ( continue_parsing == FALSE )
  {
    return 0;
  }
  else
  {
    /* 1 byte. Exception: if the data element type is nil, the data size is 0
        bytes. - Since data element type is 5 (Boolean) data size is to be increased
        by 1 */
    tot_byte_parsed += 1;
    return tot_byte_parsed;
  }
}

/*===========================================================================

FUNCTION
  bt_sd_parse_uuid

DESCRIPTION
  Parse an UUID

===========================================================================*/

uint16 bt_sd_parse_uuid(
  dsm_item_type**            dsm_ptr_ptr,
  bt_sd_srv_attr_rec_type*   srv_attr_ptr,
  uint8*                     syncml_flag_ptr
)
{
  boolean                                       is_uuid128 = FALSE;
  uint16                                        byte_parsed = 0;
  bt_sd_uuid_type                               uuid16;
  uint32                                        uuid32;
  bt_sd_uuid128_type                            uuid128;
  bt_sd_data_element_header_parse_result_type   parse_result;

  *syncml_flag_ptr = BT_SD_DB_NOT_SYNCML_V;
  
  /* Parse data element header for UUID (e.g. 0x19 for uuid16) */
  if ( ( ! bt_sd_parse_data_element_header( dsm_ptr_ptr, &parse_result ) ) ||
       ( parse_result.type_desc != BT_SD_DATA_ELEMENT_UUID ) )
  {
    /* Send BT_EV_SD_ERROR_RESP */
    BT_ERR( "BT SD: parse UID: wrong UID header T %x, S %x, P %x", 
            parse_result.type_desc, parse_result.data_size, 
            parse_result.byte_parsed );
    return 0;
  }
  byte_parsed += parse_result.byte_parsed;

  /* Fix endian-ness */
  switch( parse_result.data_size )
  {
    case 2:
      if ( bt_sd_dsm_pullup( dsm_ptr_ptr, &uuid16, 2 ) )
      {
        uuid16 = SWAB16( uuid16 );
      }
      else
      {
        /* Send BT_EV_SD_ERROR_RESP */
        BT_ERR( "BT SD: parse UID: DSM P2 fail", 0, 0, 0 );
        return 0;
      }
      if (srv_attr_ptr->attr_type == BT_SD_ATTR_TYPE_UUID128)
      {
        bt_sd_convert_16_to_128_bit_uuid( (uint8*) &uuid128, uuid16 );
        is_uuid128 = TRUE;
      }
      break;
    case 4:
      if ( bt_sd_dsm_pullup( dsm_ptr_ptr, &uuid32, 4 ) )
      {
        uuid16 = ( bt_sd_uuid_type ) SWAB32( uuid32 );
      }
      else
      {
        /* Send BT_EV_SD_ERROR_RESP */
        BT_ERR( "BT SD: parse UID: DSM P4 fail", 0, 0, 0 );
        return 0;
      }
      if (srv_attr_ptr->attr_type == BT_SD_ATTR_TYPE_UUID128)
      {
        bt_sd_convert_32_to_128_bit_uuid( (uint8*) &uuid128, uuid32 );
        is_uuid128 = TRUE;
      }
      break;
    case 16:
      if ( bt_sd_dsm_pullup( dsm_ptr_ptr,
                             (uint8*) &uuid128,
                             sizeof( bt_sd_uuid128_type ) ) )
      {
        /* Check to see if the 128-bit UUID needs to be converted to 16-bit */
        if (  srv_attr_ptr->attr_type == BT_SD_ATTR_TYPE_UUID128 )
        {
          is_uuid128 = TRUE;
        }
        else
        {
          uuid16 = bt_sd_convert_128_to_16_bit_uuid( &uuid128 );
          if ( uuid16 == BT_SD_INVALID_UUID )
          {
            /* Custom 128-bit UUID */
            is_uuid128 = TRUE;
          }
        }
      }
      else
      {
        /* Send BT_EV_SD_ERROR_RESP */
        BT_ERR( "BT SD: parse UID: DSM P16 fail", 0, 0, 0 );
        return 0;
      }
      break;
    default:
      /* Send BT_EV_SD_ERROR_RESP */
      BT_ERR( "BT SD: parse UID: invalid UID", 0, 0, 0 );
      return 0;
  }

  byte_parsed += parse_result.data_size;

  if ( is_uuid128 )
  {
    BT_MSG_UUID128( MSG_DEBUG, "BT SD: parse UID", &uuid128 );

    if ( srv_attr_ptr->attr_type == BT_SD_ATTR_TYPE_UUID_LIST )
    {
      if ( BT_SD_UUID128_EQUAL( &uuid128, &bt_sd_syncml_server_uuid ) )
      {
        *syncml_flag_ptr = BT_SD_DB_SYNCML_SERVER_B;
      }
      else if ( BT_SD_UUID128_EQUAL( &uuid128, 
                                     &bt_sd_syncml_client_uuid ) )
      {
        *syncml_flag_ptr = BT_SD_DB_SYNCML_CLIENT_B;
      }
    }
    bt_sd_service_attribute_set_uuid128( srv_attr_ptr, &uuid128 );
  }
  else
  { 
    BT_MSG_DEBUG( "BT SD: parse UID UUID16", uuid16, 0, 0 );

    bt_sd_service_attribute_add_uuid( srv_attr_ptr, uuid16 );
  }

  return byte_parsed;
}


/*===========================================================================

FUNCTION
  bt_sd_parse_uuid_list

DESCRIPTION
  Parse an UUID list

===========================================================================*/

uint16 bt_sd_parse_uuid_list(
  dsm_item_type**            dsm_ptr_ptr,
  bt_sd_srv_attr_rec_type*   srv_attr_ptr,
  uint8*                     syncml_flag_ptr
)
{
  uint16                                        tot_byte_parsed = 0;
  uint16                                        byte_parsed = 0;
  uint16                                        data_size;
  bt_sd_data_element_header_parse_result_type   parse_result;
  uint8                                         syncml_flag;

  *syncml_flag_ptr = BT_SD_DB_NOT_SYNCML_V;

  /* Parse data element header for UUID List (e.g. 0x35 0x??) */
  if ( ( ! bt_sd_parse_data_element_header( dsm_ptr_ptr, &parse_result ) ) ||
       ( parse_result.type_desc != BT_SD_DATA_ELEMENT_SEQUENCE ) )
  {
    BT_ERR( "BT SD: parse UL: wrong UL header T %x, S %x, P %x", 
            parse_result.type_desc, parse_result.data_size, 
            parse_result.byte_parsed );
    return 0;
  }
  tot_byte_parsed += parse_result.byte_parsed;
  data_size = parse_result.data_size; /* Save data size */

  while ( tot_byte_parsed < ( data_size + parse_result.byte_parsed ) )
  {
    byte_parsed = bt_sd_parse_uuid( dsm_ptr_ptr, srv_attr_ptr, &syncml_flag );

    if ( byte_parsed == 0 )
    {
      return 0;
    }
    else if ( ( syncml_flag != BT_SD_DB_NOT_SYNCML_V ) && 
              ( *syncml_flag_ptr != BT_SD_DB_NOT_SYNCML_V ) )
    {
      /* 
       * The SyncML conditions require that only a single 128-bit SyncML
       * UUID can be present in the service class ID list of the service 
       * record. Only the last occurrence is recorded.
       */
      BT_MSG_HIGH( "BT SD: parse UL: mult SML UID, overwritten", 0, 0, 0 );
    }                              

    tot_byte_parsed += byte_parsed;
    *syncml_flag_ptr = syncml_flag;
  } 

  return tot_byte_parsed;
}


/*===========================================================================

FUNCTION
  bt_sd_parse_proto_desc_uint_list_param

DESCRIPTION
  Parse an unsigned integer list which is a parameter within a protocol 
  descriptor.
  
NOTE
  Due to memory limitations, the parsed result is NOT stored in the 
  service search result database. Only the BNEP protocol descriptor
  contains an element sequence of uint16, not storing the result will not
  affect the proper SD functioning.

===========================================================================*/

uint16 bt_sd_parse_proto_desc_uint_list_param(
  dsm_item_type**            dsm_ptr_ptr,
  bt_sd_proto_desc_type*     proto_desc_ptr
)
{
  uint16                                        tot_byte_parsed = 0;
  uint16                                        byte_parsed_l1;
  uint16                                        byte_parsed_l2;
  uint16                                        data_size;
  uint64                                        val;
  bt_sd_data_element_header_parse_result_type   parse_result;

  /* Parse data element header
   * (e.g. 0x35 0x??) */
  if ( ( ! bt_sd_parse_data_element_header( dsm_ptr_ptr, &parse_result ) ) ||
       ( parse_result.type_desc != BT_SD_DATA_ELEMENT_SEQUENCE ) )
  {
    BT_ERR( "BT SD: parse PDULP: wrong header T %x, S %x, P %x", 
            parse_result.type_desc, parse_result.data_size, 
            parse_result.byte_parsed );
    return 0;
  }
  tot_byte_parsed += parse_result.byte_parsed;
  data_size = parse_result.data_size; /* Save data size */

  byte_parsed_l1 = 0;
  while ( byte_parsed_l1 < data_size )
  {
    val = 0;
    /* Parse data element header for primitive (e.g. 0x08 for uint8) */
    if ( ( byte_parsed_l2 =
             bt_sd_parse_unsigned_primitive(
               dsm_ptr_ptr, (void*) &val, sizeof( uint64 ) ) ) == 0 )
    {
      /* Send BT_EV_SD_ERROR_RESP */
      return 0;
    }
    byte_parsed_l1 += byte_parsed_l2;

    /* 
     * The element sequence of uint16s within the protocol descriptor is
     * NOT stored in the service search result database due to memory 
     * availability limitation.
     */
    BT_MSG_DEBUG( "BT SD: parse PDULP: element 0x%x", val, 0, 0 );
  }

  tot_byte_parsed += byte_parsed_l1;

  return tot_byte_parsed;
}


/*===========================================================================

FUNCTION
  bt_sd_parse_proto_desc_list

DESCRIPTION
  Parse a protocol descriptor list

===========================================================================*/

uint16 bt_sd_parse_proto_desc_list(
  dsm_item_type**            dsm_ptr_ptr,
  bt_sd_srv_attr_rec_type*   srv_attr_ptr
)
{
  uint16                                        tot_byte_parsed = 0;
  uint16                                        byte_parsed_l1;
  uint16                                        byte_parsed_l2;
  uint16                                        byte_parsed_l3;
  uint16                                        data_size1;
  uint16                                        data_size2;
  uint32                                        param32 = 0;
  bt_sd_uuid_type                               uuid16;
  uint32                                        uuid32;
  bt_sd_uuid128_type                            uuid128;
  bt_sd_uuid128_type*                           uuid128_ptr = NULL;
  bt_sd_proto_desc_type*                        proto_desc_ptr;
  bt_sd_data_element_header_parse_result_type   parse_result;
  uint8                                         element_header;
  bt_sd_data_element_enum_type                  element_type;

  /* Parse data element header for Protocol Descriptor List (e.g. 0x35 0x??) */
  if ( ( ! bt_sd_parse_data_element_header( dsm_ptr_ptr, &parse_result ) ) ||
       ( parse_result.type_desc != BT_SD_DATA_ELEMENT_SEQUENCE ) )
  {
    BT_ERR( "BT SD: parse PDL: wrong PDL header T %x, S %x, P %x", 
            parse_result.type_desc, parse_result.data_size, 
            parse_result.byte_parsed );;
    return 0;
  }
  tot_byte_parsed += parse_result.byte_parsed;
  data_size1 = parse_result.data_size; /* Save data size */

  byte_parsed_l1 = 0;
  while ( byte_parsed_l1 < data_size1 )
  {
    /* Parse data element header for Protocol Descriptor (e.g. 0x35 0x??) */
    if ( ! bt_sd_parse_data_element_header( dsm_ptr_ptr, &parse_result ) )
    {
      BT_ERR( "BT SD: parse UL: wrong PD header T %x, S %x, P %x", 
            parse_result.type_desc, parse_result.data_size, 
            parse_result.byte_parsed );;
      return 0;
    }
    
    if ( parse_result.type_desc == BT_SD_DATA_ELEMENT_SEQUENCE )
    {                                     
      byte_parsed_l1 += parse_result.byte_parsed;
      data_size2 = parse_result.data_size;
      byte_parsed_l2 = 0;
  
      /* Parse data element header for UUID (e.g. 0x9 0x?? 0x??) */
      if ( ( ! bt_sd_parse_data_element_header( dsm_ptr_ptr, &parse_result ) ) ||
           ( parse_result.type_desc != BT_SD_DATA_ELEMENT_UUID ) )
      {
        BT_ERR( "BT SD: parse PDLL: wrong UID header T %x, S %x, P %x", 
                parse_result.type_desc, parse_result.data_size, 
                parse_result.byte_parsed );;
        return 0;
      }
      byte_parsed_l2 += parse_result.byte_parsed;
    }
    else if ( parse_result.type_desc == BT_SD_DATA_ELEMENT_UUID )
    {
      /* 
        Some devices only include a stand-alone UUID as the contents of a 
        Bluetooth profile descriptor, rather than a sequence containing 
        one UUID (profile version number may be optional). To accomodate for 
        these devices, keep going but do not allow any other elements in 
        the descriptor.
       */
      BT_MSG_HIGH( "BT SD: parse PDL: PD not a seq", 0, 0, 0 );
      data_size2 = data_size1;
      byte_parsed_l2 = parse_result.byte_parsed;
    }
    else
    {
      return 0;
    }

    switch( parse_result.data_size )
    {
      case 2:
        if ( ! bt_sd_dsm_pullup( dsm_ptr_ptr, (void*) &uuid16, 2 ) )
        {
          BT_ERR( "BT SD: parse PDL: DSM P2 fail", 0, 0, 0 );
          return 0;
        }
        uuid16 = SWAB16( uuid16 );
        break;
      case 4:
        if ( ! bt_sd_dsm_pullup( dsm_ptr_ptr, (void*) &uuid32, 4 ) )
        {
          BT_ERR( "BT SD: parse PDL: DSM P4 fail", 0, 0, 0 );
          return 0;
        }
        uuid32 = SWAB32( uuid32 );
        uuid16 = (uint16) uuid32;
        break;
      case 16:
        if ( bt_sd_dsm_pullup( dsm_ptr_ptr, (void*) &uuid128,
                               sizeof( bt_sd_uuid128_type ) ) )
        {
          uuid16 = bt_sd_convert_128_to_16_bit_uuid( &uuid128 );
          if ( uuid16 == BT_SD_INVALID_UUID )
          {
            /* Custom 128-bit UUID */
            BT_MSG_UUID128( MSG_DEBUG, "BT SD: parse PDL", &uuid128 );
            uuid128_ptr = &uuid128;
          }
          else
          {
            BT_MSG_DEBUG( "BT SD: parse PDL UUID16", uuid16, 0, 0 );
          }
        }
        else
        {
          /* Send BT_EV_SD_ERROR_RESP */
          BT_ERR( "BT SD: parse PDL: DSM P16 fail", 0, 0, 0 );
          return 0;
        }
        break;
      default:
        BT_ERR( "BT SD: parse PDL: invalid data size", 0, 0, 0 );
        return 0;
    }

    byte_parsed_l2 += parse_result.data_size;
    proto_desc_ptr =
      bt_sd_service_attribute_add_proto_desc( srv_attr_ptr,
                                              uuid16,
                                              uuid128_ptr );

    if ( proto_desc_ptr != NULL )
    { 
      while ( byte_parsed_l2 < data_size2 )
      {
        /* take a peek at the data element header */
        if ( dsm_peek_byte( *dsm_ptr_ptr, 0, &element_header ) != TRUE )
        {
          BT_ERR( "BT SD: parse PDL: DSM peek fail", 0, 0, 0 );
          return 0;
        }
        else
        {
          element_type = (bt_sd_data_element_enum_type)( ( element_header >> 3 ) 
                                                         & 0x1F );
          if ( element_type == BT_SD_DATA_ELEMENT_UNSIGNED_INT )
          {
            if ( ( byte_parsed_l3 =
                   bt_sd_parse_unsigned_primitive(
                     dsm_ptr_ptr, (void*) &param32, 4 ) ) == 0 )
            {
              BT_ERR( "BT SD: parse PDL: parse UP fail", 0, 0, 0 );
              return 0;
            }
            byte_parsed_l2 += byte_parsed_l3;
            bt_sd_service_attribute_proto_desc_add_param( 
              proto_desc_ptr,
              (uint16) param32,
              ( byte_parsed_l3 - 1 ) );
          }
          else if ( element_type == BT_SD_DATA_ELEMENT_SEQUENCE )
          {
            if ( ( byte_parsed_l3 = 
                   bt_sd_parse_proto_desc_uint_list_param( 
                     dsm_ptr_ptr, proto_desc_ptr ) ) == 0 )
            {
              BT_ERR( "BT SD: parse PDL: parse UL fail", 0, 0, 0 );
              return 0;
            }
            /* the list is not saved in the database */
            byte_parsed_l2 += byte_parsed_l3;
          }
          else 
          {
            BT_ERR( "BT SD: parse PDL: Unexpected Elem Type", 0, 0, 0 );
            return 0;
          }
        }
      } /* end while */

      byte_parsed_l1 += byte_parsed_l2;
    }
    else
    {
      BT_ERR( "BT SD: parse PDL: add PD fail", 0, 0, 0 );
      return 0;
    }
  }

  tot_byte_parsed += byte_parsed_l1;

  return tot_byte_parsed;
}


/*===========================================================================

FUNCTION
  bt_sd_parse_add_proto_desc_lists

DESCRIPTION
  Parse the additional protocol descriptor lists attribute

===========================================================================*/

uint16 bt_sd_parse_add_proto_desc_lists(
  dsm_item_type**            dsm_ptr_ptr,
  bt_sd_srv_attr_rec_type*   srv_attr_ptr
)
{

  uint16                                        tot_byte_parsed = 0;
  uint16                                        byte_parsed = 0;
  uint16                                        data_size;
  bt_sd_data_element_header_parse_result_type   parse_result;
  bt_sd_srv_attr_rec_type                       temp_attr;
  uint16                                        pdl_cnt = 0;
  uint16                                        i;

  /* Parse data element header for additional protocol descriptor lists
    (e.g. 0x35 0x??) */
  if ( ( ! bt_sd_parse_data_element_header( dsm_ptr_ptr, &parse_result ) ) ||
       ( parse_result.type_desc != BT_SD_DATA_ELEMENT_SEQUENCE ) )
  {
    BT_ERR( "BT SD: parse APDL: wrong header T %x, S %x, P %x", 
            parse_result.type_desc, parse_result.data_size, 
            parse_result.byte_parsed );
    return 0;
  }
  tot_byte_parsed += parse_result.byte_parsed;
  data_size = parse_result.data_size; /* Save data size */
  
  temp_attr.header.type = BT_SD_DATA_ELEMENT_SEQUENCE;
  temp_attr.header.size_index = 5;
  temp_attr.header.hdr_len = 2;
  temp_attr.attr_id = BT_SD_ATTR_ID_PROTOCOL_DESCRIPTOR_LIST;
  temp_attr.attr_type = BT_SD_ATTR_TYPE_PROTO_DESC_LIST;  
  temp_attr.matched_flag = FALSE;

  while ( tot_byte_parsed < ( data_size + parse_result.byte_parsed ) )
  { 
    temp_attr.header.attr_value_len = 0;
    temp_attr.attr_value.proto_desc_list.num_proto_desc = 0;

    if ( ( byte_parsed = 
           bt_sd_parse_proto_desc_list( dsm_ptr_ptr, &temp_attr ) ) > 0 )
    {

      tot_byte_parsed += byte_parsed;

      if ( pdl_cnt < BT_SD_MAX_NUM_OF_ADD_PROTO_DESC_LISTS )
      {
        /* copy the parsed info stored in temp_attr to the actual attribute */
        memcpy( (void*)(&srv_attr_ptr->attr_value.add_proto_desc_lists[ pdl_cnt ]),
                (void*)(&temp_attr.attr_value.proto_desc_list),
                sizeof( bt_sd_proto_desc_list_type ) );

        for ( i = 0; 
              i < srv_attr_ptr->
              attr_value.add_proto_desc_lists[ pdl_cnt ].num_proto_desc; 
              i++ )
        {
          /* the srv_attr_ptr is not used from this point onwards, set it here
             for consistency */
          srv_attr_ptr->attr_value.add_proto_desc_lists[ pdl_cnt ].
          proto_desc[ i ].srv_attr_ptr = (void*)srv_attr_ptr;
        }

        srv_attr_ptr->header.attr_value_len += temp_attr.header.attr_value_len;
        pdl_cnt++;
      }
      else
      {
        BT_ERR( "BT SD: parse APDL: pdl_cnt out of range! %x", pdl_cnt, 0, 0 );
      }
    }
    else
    {
      BT_ERR( "BT SD: parse APDL: Bad PDL", 0,0,0 );
      return 0;
    }

  } 

  return tot_byte_parsed;

}

/*===========================================================================

FUNCTION
  bt_sd_parse_lang_base_attr_id_list

DESCRIPTION
  Parse a language-based attribute ID list

===========================================================================*/

uint16 bt_sd_parse_lang_base_attr_id_list(
  dsm_item_type**            dsm_ptr_ptr,
  bt_sd_srv_attr_rec_type*   srv_attr_ptr
)
{
  uint16                                        tot_byte_parsed = 0;
  uint16                                        byte_parsed_l1;
  uint16                                        byte_parsed_l2;
  uint16                                        data_size;
  uint16                                        lang_id;
  uint16                                        char_enc_id;
  uint16                                        base_attr_id;
  bt_sd_data_element_header_parse_result_type   parse_result;

  /* Parse data element header for Language Base Attribute ID List
   * (e.g. 0x35 0x??) */
  if ( ( ! bt_sd_parse_data_element_header( dsm_ptr_ptr, &parse_result ) ) ||
       ( parse_result.type_desc != BT_SD_DATA_ELEMENT_SEQUENCE ) )
  {
    BT_ERR( "BT SD: parse LBAIL: wrong header T %x, S %x, P %x", 
            parse_result.type_desc, parse_result.data_size, 
            parse_result.byte_parsed );
    return 0;
  }
  tot_byte_parsed += parse_result.byte_parsed;
  data_size = parse_result.data_size; /* Save data size */

  byte_parsed_l1 = 0;
  while ( byte_parsed_l1 < data_size )
  {
    /* Parse data element header for Language ID (e.g. 0x09 for uint16) */
    if ( ( byte_parsed_l2 =
             bt_sd_parse_unsigned_primitive(
               dsm_ptr_ptr, (void*) &lang_id, 2 ) ) != 3 )
    {
      /* Send BT_EV_SD_ERROR_RESP */
      BT_ERR( "BT SD: parse LBAIL: parse LID fail", 0, 0, 0 );
      return 0;
    }
    byte_parsed_l1 += byte_parsed_l2;

    /* Parse data element header for Character Encoding ID
       (e.g. 0x09 for uint16) */
    if ( ( byte_parsed_l2 =
             bt_sd_parse_unsigned_primitive(
               dsm_ptr_ptr, (void*) &char_enc_id, 2 ) ) != 3 )
    {
      /* Send BT_EV_SD_ERROR_RESP */
      BT_ERR( "BT SD: parse LBAIL: parse CEID fail", 0, 0, 0 );
      return 0;
    }
    byte_parsed_l1 += byte_parsed_l2;

    /* Parse data element header for Base Attribute ID
       (e.g. 0x09 for uint16) */
    if ( ( byte_parsed_l2 =
             bt_sd_parse_unsigned_primitive(
               dsm_ptr_ptr, (void*) &base_attr_id, 2 ) ) != 3 )
    {
      /* Send BT_EV_SD_ERROR_RESP */
      BT_ERR( "BT SD: parse LBAIL: parse BAID fail", 0, 0, 0 );
      return 0;
    }
    byte_parsed_l1 += byte_parsed_l2;

    bt_sd_service_attribute_add_lang_base_attr_id( srv_attr_ptr, lang_id,
                                                   char_enc_id, base_attr_id );
  }

  tot_byte_parsed += byte_parsed_l1;

  return tot_byte_parsed;
}


/*===========================================================================

FUNCTION
  bt_sd_parse_hid_desc_lists

DESCRIPTION
  Parse a HID Descriptor list

===========================================================================*/

uint16 bt_sd_parse_hid_desc_lists(
  dsm_item_type**            dsm_ptr_ptr,
  bt_sd_srv_attr_rec_type*   srv_attr_ptr
)
{
  uint8                                         ch;
  uint16                                        tot_byte_parsed = 0;
  uint16                                        byte_parsed_l1;
  uint16                                        byte_parsed_l2;
  uint16                                        data_size;
  uint8                                         class_desc_type;
  uint16                                        desc_size;
  bt_sd_data_element_header_parse_result_type   parse_result;
  uint16                                        i ;

  /* Parse data element header for Language Base Attribute ID List
   * (e.g. 0x35 0x??) */
  if ( ( ! bt_sd_parse_data_element_header( dsm_ptr_ptr, &parse_result ) ) ||
       ( parse_result.type_desc != BT_SD_DATA_ELEMENT_SEQUENCE ) )
  {
    BT_ERR( "BT SD: parse LBAIL: wrong header T %x, S %x, P %x", 
            parse_result.type_desc, parse_result.data_size, 
            parse_result.byte_parsed );
    return 0;
  }
  tot_byte_parsed += parse_result.byte_parsed;
  data_size = parse_result.data_size; /* Save data size */

  /* Parse data element header for Language Base Attribute ID List
   * (e.g. 0x35 0x??) */
  if ( ( ! bt_sd_parse_data_element_header( dsm_ptr_ptr, &parse_result ) ) ||
       ( parse_result.type_desc != BT_SD_DATA_ELEMENT_SEQUENCE ) )
  {
    BT_ERR( "BT SD: parse LBAIL: wrong header T %x, S %x, P %x", 
            parse_result.type_desc, parse_result.data_size, 
            parse_result.byte_parsed );
    return 0;
  }
  tot_byte_parsed += parse_result.byte_parsed;
  data_size = parse_result.data_size; /* Save data size */

  byte_parsed_l1 = 0;
  while ( byte_parsed_l1 < data_size )
  {
    /* Parse data element header for report (e.g. 0x21 for report) */
    if ( ( byte_parsed_l2 =
             bt_sd_parse_unsigned_primitive(
               dsm_ptr_ptr, (void*) &class_desc_type, 1 ) ) != 2 )
    {
      /* Send BT_EV_SD_ERROR_RESP */
      BT_ERR( "BT SD: parse LBAIL: parse LID fail", 0, 0, 0 );
      return 0;
    }
    BT_MSG_HIGH("BT SD: HID class desc type : %x",class_desc_type,0,0);

    byte_parsed_l1 += byte_parsed_l2;
 
    /* Parse data element header for String (e.g. 0x25 0x??) */
    if ( ( ! bt_sd_parse_data_element_header( dsm_ptr_ptr, &parse_result ) ) ||
         ( parse_result.type_desc != BT_SD_DATA_ELEMENT_TEXT_STRING ) )
    {
      BT_ERR( "BT SD: parse TS: wrong header T %x, S %x, P %x", 
              parse_result.type_desc, parse_result.data_size, 
              parse_result.byte_parsed );;
      return 0;
    }
    byte_parsed_l1 += parse_result.byte_parsed;
    desc_size = parse_result.data_size;

      /* Check for byte array overflow */
    if ( desc_size > BT_SD_MAX_HID_CLASS_HOST_DESC_STRING_LEN )
    {
      BT_MSG_HIGH("BT SD:HID Discr size set to max size RecdDescSize: %x",
                                                           desc_size,0,0);
      desc_size = BT_SD_MAX_HID_CLASS_HOST_DESC_STRING_LEN;
    }

   BT_MSG_HIGH("BT SD: HID Descriptor size : %x",desc_size,0,0);
   if( ! bt_sd_dsm_pullup( dsm_ptr_ptr, 
                     (void*) &bt_sd_host_desc_list[hid_desc_index].str[0], 
                     desc_size ))
   {
     BT_ERR( "BT SD: parse TS: DSM PU fail", 0, 0, 0 );
     return 0;
   }
   bt_sd_host_desc_list[hid_desc_index].len = desc_size ;
  
   srv_attr_ptr->attr_value.hid_host_desc_list.valid = TRUE ;
   srv_attr_ptr->attr_value.hid_host_desc_list.index = hid_desc_index ;

   hid_desc_index++;
   if(hid_desc_index == BT_SD_MAX_HID_HOST_DESC_ATTR)
   {
     BT_MSG_HIGH("BT SD: Resetting HID descriptor count",0,0,0);
     hid_desc_index = 0;
   }

   byte_parsed_l1 += desc_size ;
   if ( desc_size < parse_result.data_size )
   {
     /* Retrieve the rest of the bytes */
     for ( i = 0; i < ( parse_result.data_size - desc_size ); i++ )
     {
       if ( bt_sd_dsm_pullup( dsm_ptr_ptr, (void*) &ch, 1 ) )
       {
         byte_parsed_l1++;
       }
       else
       {
         BT_ERR( "BT SD: parse TS: DSM PU 2 fail", 0, 0, 0 );
         return 0;
       }
     }
   }
   BT_MSG_HIGH("BT SD: HID attribute Parsing ByteParsed:%x DataSize:%x",
                                            byte_parsed_l1,data_size,0);
  }
  tot_byte_parsed += byte_parsed_l1;
  BT_MSG_HIGH("BT SD: Total bytes parsed : %x",tot_byte_parsed,0,0);

  return tot_byte_parsed;
}

/*===========================================================================

FUNCTION
  bt_sd_parse_uint_list

DESCRIPTION
  Parse an unsigned integer list

===========================================================================*/

uint16 bt_sd_parse_uint_list(
  dsm_item_type**            dsm_ptr_ptr,
  bt_sd_srv_attr_rec_type*   srv_attr_ptr
)
{
  uint16                                        tot_byte_parsed = 0;
  uint16                                        byte_parsed_l1;
  uint16                                        byte_parsed_l2;
  uint16                                        data_size;
  uint64                                        val;
  bt_sd_data_element_header_parse_result_type   parse_result;

  /* Parse data element header
   * (e.g. 0x35 0x??) */
  if ( ( ! bt_sd_parse_data_element_header( dsm_ptr_ptr, &parse_result ) ) ||
       ( parse_result.type_desc != BT_SD_DATA_ELEMENT_SEQUENCE ) )
  {
    BT_ERR( "BT SD: parse UintL: wrong header T %x, S %x, P %x", 
            parse_result.type_desc, parse_result.data_size, 
            parse_result.byte_parsed );
    return 0;
  }
  tot_byte_parsed += parse_result.byte_parsed;
  data_size = parse_result.data_size; /* Save data size */

  byte_parsed_l1 = 0;
  while ( byte_parsed_l1 < data_size )
  {
    val = 0;
    /* Parse data element header for primitive (e.g. 0x08 for uint8) */
    if ( ( byte_parsed_l2 =
             bt_sd_parse_unsigned_primitive(
               dsm_ptr_ptr, (void*) &val, sizeof( uint64 ) ) ) == 0 )
    {
      /* Send BT_EV_SD_ERROR_RESP */
      return 0;
    }
    byte_parsed_l1 += byte_parsed_l2;

    bt_sd_service_attribute_uint_list_add_val(
      srv_attr_ptr,
      byte_parsed_l2 - 1, /* Subtract 1 to account for the DE header */
      val
    );
  }

  tot_byte_parsed += byte_parsed_l1;

  return tot_byte_parsed;
}


/*===========================================================================

FUNCTION
  bt_sd_parse_text_string

DESCRIPTION
  Parse a text string

===========================================================================*/

uint16 bt_sd_parse_text_string(
  dsm_item_type**            dsm_ptr_ptr,
  bt_sd_srv_attr_rec_type*   srv_attr_ptr
)
{
  uint8                                       ch;
  uint16                                      tot_byte_parsed = 0;
  uint16                                      i, len;
  #ifndef CUST_EDITION
  char                                        str[BT_SD_MAX_TEXT_STRING_LEN+1];
  #else
  char                                        str[BT_SD_MAX_TEXT_STRING_LEN+1] = {0};
  #endif
  bt_sd_data_element_header_parse_result_type parse_result;

  /* Parse data element header for String (e.g. 0x25 0x??) */
  if ( ( ! bt_sd_parse_data_element_header( dsm_ptr_ptr, &parse_result ) ) ||
       ( parse_result.type_desc != BT_SD_DATA_ELEMENT_TEXT_STRING ) )
  {
    BT_ERR( "BT SD: parse TS: wrong header T %x, S %x, P %x", 
            parse_result.type_desc, parse_result.data_size, 
            parse_result.byte_parsed );;
    return 0;
  }
  tot_byte_parsed += parse_result.byte_parsed;
  len = parse_result.data_size;

  /* Check for byte array overflow */
  if ( len > BT_SD_MAX_TEXT_STRING_LEN )
  {
    len = BT_SD_MAX_TEXT_STRING_LEN;
  }

  if ( ! bt_sd_dsm_pullup( dsm_ptr_ptr, (void*) str, len ) )
  {
    BT_ERR( "BT SD: parse TS: DSM PU 1 fail", 0, 0, 0 );
    return 0;
  }
  str[ len ] = '\0'; /* Null-terminate the string */
  tot_byte_parsed += len;

  bt_sd_service_attribute_set_str( srv_attr_ptr, str );

  if ( len < parse_result.data_size )
  {
    /* Retrieve the rest of the bytes */
    for ( i = 0; i < ( parse_result.data_size - len ); i++ )
    {
      if ( bt_sd_dsm_pullup( dsm_ptr_ptr, (void*) &ch, 1 ) )
      {
        tot_byte_parsed++;
      }
      else
      {
        BT_ERR( "BT SD: parse TS: DSM PU 2 fail", 0, 0, 0 );
        return 0;
      }
    }
  }

  return tot_byte_parsed;
}

#ifdef CUST_EDITION
uint16 bt_sd_parse_unknown_type_attr_value( dsm_item_type** dsm_ptr_ptr, bt_sd_srv_attr_rec_type*	srv_attr_ptr )
{
	boolean status = TRUE;
	uint8 Header = 0, size_index = 0;
	uint8 uint8_data_size = 0;
	uint16 uint16_data_size = 0;
	uint32 uint32_data_size = 0;
	bt_sd_data_element_header_parse_result_type parse_result = { 0 };

	/* Parse Data Element Header */
	if ( !bt_sd_dsm_pullup( dsm_ptr_ptr, ( void * )&Header, sizeof( uint8 ) ) )
    {
      	 BT_ERR( "BT SD: Peek Unknown Type Attr Header Failed", 0, 0, 0 );
      	 return 0;
    }

	parse_result.byte_parsed += sizeof( uint8 );

	/* The upper 5 bits are the type descriptor */
	parse_result.type_desc = ( bt_sd_data_element_enum_type )( ( Header >> 3 ) & 0x1F );
	
	/* The lower 3 bits are the size index */
	size_index = Header & 0x07;

	switch ( parse_result.type_desc )
	{
	case BT_SD_DATA_ELEMENT_NULL:
	case BT_SD_DATA_ELEMENT_BOOL:
		 {
			if ( size_index != 0 )
			{
				 BT_MSG_HIGH( "BT SD: Invalid size index %d for type %d", size_index, parse_result.type_desc, 0 );
				 status = FALSE;
			}
			else
			{
				 parse_result.data_size = 0; /* Don't care about the size field */
			}
		 }
		 break;
		 
	case BT_SD_DATA_ELEMENT_UNSIGNED_INT:
	case BT_SD_DATA_ELEMENT_SIGNED_INT:
		 {
		    if ( size_index > 4 )
		    {
		   		 BT_MSG_HIGH( "BT SD: Invalid size index %d for type %d", size_index, parse_result.type_desc, 0 );
			     status = FALSE;
		    }
			else
			{
		  		 parse_result.data_size = bt_sd_data_element_size_desc[ size_index ];
			}
		 }
	  	 break;
		 
	case BT_SD_DATA_ELEMENT_UUID:
		 {
		    if ( ( size_index != 1 ) && ( size_index != 2 ) && ( size_index != 4 ) )
		    {
		    	 BT_MSG_HIGH( "BT SD: Invalid size index %d for type %d", size_index, parse_result.type_desc, 0 );
		    	 status = FALSE;
		  	}
			else
			{
		  		 parse_result.data_size = bt_sd_data_element_size_desc[ size_index ];
			}
		 }
	  	 break;
		 
	case BT_SD_DATA_ELEMENT_TEXT_STRING:
	case BT_SD_DATA_ELEMENT_SEQUENCE:
	case BT_SD_DATA_ELEMENT_ALTERNATIVE:
	case BT_SD_DATA_ELEMENT_URL:
		 {
		    if ( size_index < 5 )
		    {
		    	 BT_MSG_HIGH( "BT SD: Invalid size index %d for type %d", size_index, parse_result.type_desc, 0 );
			     status = FALSE;
			     break;
		    }
			
		    /* Fix endian-ness */
		    switch ( bt_sd_data_element_size_desc[ size_index ] )
		    {
		    case 1: /* 1 byte */
				 {
					if ( !bt_sd_dsm_pullup( dsm_ptr_ptr, ( void * )&uint8_data_size, sizeof( uint8 ) ) )
			      	{
			        	 BT_ERR( "BT SD: dsm_extract failed", 0, 0, 0 );
			        	 status = FALSE;
			      	}
					else
					{
			      		 parse_result.data_size = uint8_data_size;
					}
		    	 }
		      	 break;
				 
		    case 2: /* 2 bytes */
				 {
					if ( !bt_sd_dsm_pullup( dsm_ptr_ptr, ( void * )&uint16_data_size, sizeof( uint16 ) ) )
			      	{
			        	 BT_ERR( "BT SD: dsm_extract failed", 0, 0, 0 );
			        	 status = FALSE;
			      	}
					else
					{
			      		 parse_result.data_size = SWAB16( uint16_data_size );
					}
		    	 }
		      	 break;
				 
		    case 4: /* 4 bytes */
				 {
			      	if ( !bt_sd_dsm_pullup( dsm_ptr_ptr, ( void * )&uint32_data_size, sizeof( uint32 ) ) )
			      	{
			        	 BT_ERR( "BT SD: dsm_extract failed", 0, 0, 0 );
			        	 status = FALSE;
			      	}
					else
					{
			      		 parse_result.data_size = SWAB32( uint32_data_size );
					}
		    	 }
		      	 break;
				 
		    default:
		    	 {
			      	BT_ERR( "BT SD: Bad elem size %x I %x", bt_sd_data_element_size_desc[ size_index ], size_index, 0 );
			      	status = FALSE;
		    	 }
		         break;
		  	}

			if ( status == TRUE )
			{
		  		 parse_result.byte_parsed += bt_sd_data_element_size_desc[ size_index ];
			}
		 }
	  	 break;
		 
	default:
		 {
		  	BT_MSG_HIGH( "BT SD: Invalid type descriptor %d", parse_result.type_desc, 0, 0 );
		  	status = FALSE;
		 }
	  	 break;
	}

	if ( status == FALSE )
		 return 0;
	
	if ( ( parse_result.byte_parsed + parse_result.data_size ) > BT_SD_MAX_CUSTOM_ATTR_VALUE_LENGTH )
	{
		 BT_ERR( "BT SD: Unknown Type Attr Length Exceed: HeaderSize %d, DataSize %d, TotalSize %d", 
				 parse_result.byte_parsed, parse_result.data_size, ( parse_result.byte_parsed + parse_result.data_size ) );
		 return 0;
	}

	memset( ( void * )&srv_attr_ptr->attr_value.custom_attr_value, 0, sizeof( bt_sd_custom_attribute_value ) );

	srv_attr_ptr->attr_value.custom_attr_value.Data[ 0 ] = Header;

	if ( parse_result.byte_parsed == 2 )
	{
		 memcpy( ( void * )( srv_attr_ptr->attr_value.custom_attr_value.Data + 1 ), ( void * )&uint8_data_size, sizeof( uint8 ) );
	}
	else if ( parse_result.byte_parsed == 3 )
	{
		 memcpy( ( void * )( srv_attr_ptr->attr_value.custom_attr_value.Data + 1 ), ( void * )&uint16_data_size, sizeof( uint16 ) );
	}
	else if ( parse_result.byte_parsed == 5 )
	{
		 memcpy( ( void * )( srv_attr_ptr->attr_value.custom_attr_value.Data + 1 ), ( void * )&uint32_data_size, sizeof( uint32 ) );
	}

	if ( !bt_sd_dsm_pullup( dsm_ptr_ptr, ( void * )( srv_attr_ptr->attr_value.custom_attr_value.Data + parse_result.byte_parsed ), parse_result.data_size ) )
	{
		 BT_ERR( "BT SD: parse TS: DSM PU 1 fail", 0, 0, 0 );
		 return 0;
	}

	srv_attr_ptr->header.type = parse_result.type_desc;
	srv_attr_ptr->header.size_index = size_index;
	srv_attr_ptr->header.hdr_len = ( uint8 )parse_result.byte_parsed;
	srv_attr_ptr->header.attr_value_len = parse_result.data_size;

	return ( parse_result.byte_parsed + parse_result.data_size );
}
#endif


/*===========================================================================

FUNCTION
  bt_sd_parse_attribute_value

DESCRIPTION
  Parse the AttributeValue field of a SDP Service Attribute Response and
  Service Search Attribute Response

===========================================================================*/

uint16 bt_sd_parse_attribute_value
(
  dsm_item_type**           dsm_ptr_ptr,
  bt_sd_srv_attr_enum_type  attr_type,
  bt_sd_srv_attr_rec_type*  srv_attr_ptr,
  uint8*                    syncml_flag_ptr
)
{
  uint16  byte_parsed = 0;
  uint64  uint64_val;
  uint8   bool_val = 0;
  byte    uint128_val[ 16 ];

  *syncml_flag_ptr = BT_SD_DB_NOT_SYNCML_V;

  switch( attr_type )
  {
    case BT_SD_ATTR_TYPE_UUID_LIST:
      byte_parsed = bt_sd_parse_uuid_list( dsm_ptr_ptr, srv_attr_ptr, 
                                           syncml_flag_ptr );
      break;
    case BT_SD_ATTR_TYPE_PROTO_DESC_LIST:
      byte_parsed = bt_sd_parse_proto_desc_list( dsm_ptr_ptr, srv_attr_ptr );
      break;
    case BT_SD_ATTR_TYPE_LANG_BASE_ATTR_ID_LIST:
      byte_parsed = bt_sd_parse_lang_base_attr_id_list( dsm_ptr_ptr, 
                                                        srv_attr_ptr );
      break;
    case BT_SD_ATTR_TYPE_UINT_LIST:
      byte_parsed = bt_sd_parse_uint_list( dsm_ptr_ptr, srv_attr_ptr );
      break;
    case BT_SD_ATTR_TYPE_STRING:
      byte_parsed = bt_sd_parse_text_string( dsm_ptr_ptr, srv_attr_ptr );
      break;
    case BT_SD_ATTR_TYPE_UINT8:
    case BT_SD_ATTR_TYPE_UINT16:
    case BT_SD_ATTR_TYPE_UINT32:
    case BT_SD_ATTR_TYPE_UINT64:
      byte_parsed = bt_sd_parse_unsigned_primitive( dsm_ptr_ptr, 
                                                    (void*) &uint64_val, 8 );
      if ( byte_parsed > 0 )
      {
        bt_sd_service_attribute_set_value( srv_attr_ptr, uint64_val );
      }
      break;
    case BT_SD_ATTR_TYPE_UINT128:
      byte_parsed = bt_sd_parse_unsigned_primitive( dsm_ptr_ptr, 
                                                    (void*) uint128_val, 16 );
      if ( byte_parsed > 0 )
      {
        bt_sd_service_attribute_set_uint128( srv_attr_ptr, uint128_val ); 
      }
      break;
    case BT_SD_ATTR_TYPE_BOOL:
      byte_parsed = bt_sd_parse_boolean_primitive( dsm_ptr_ptr, 
                                                    (void*) &bool_val, 1 );
      if ( byte_parsed > 0 )
      {
        if ( bool_val == 0 )
        {
          bt_sd_service_attribute_set_bool( srv_attr_ptr, FALSE );
        }
        else
        {
          bt_sd_service_attribute_set_bool( srv_attr_ptr, TRUE );
        }
      }
      break;
    case BT_SD_ATTR_TYPE_UUID:
    case BT_SD_ATTR_TYPE_UUID128:
      /* *syncml_flag_ptr should always return 0 for this case */
      byte_parsed = bt_sd_parse_uuid( dsm_ptr_ptr, srv_attr_ptr, 
                                      syncml_flag_ptr );
      break;
    case BT_SD_ATTR_TYPE_ADD_PROTO_DESC_LISTS:
      byte_parsed = bt_sd_parse_add_proto_desc_lists( dsm_ptr_ptr, 
                                                      srv_attr_ptr );
      break;

    case BT_SD_ATTR_TYPE_HID_DESC_LIST:
      byte_parsed = bt_sd_parse_hid_desc_lists( dsm_ptr_ptr, 
                                                 srv_attr_ptr );
      break;
	#ifdef CUST_EDITION
	case BT_SD_ATTR_TYPE_UNKNOWN:
		 {
			 byte_parsed = bt_sd_parse_unknown_type_attr_value( dsm_ptr_ptr, srv_attr_ptr );
		 }
      	 break;
	#endif
  }

  return byte_parsed;
}


/*===========================================================================

FUNCTION
  bt_sd_send_error_resp

DESCRIPTION
  Send a SDP Error Response

===========================================================================*/

void bt_sd_send_error_resp(
  uint16                      cid,
  uint16                      tid,
  bt_sd_error_code_enum_type  ec
)
{
  dsm_item_type*            dsm_ptr;
  bt_sd_pdu_hdr_type        pdu_hdr;
  uint16                    uint16_to_send;
  dsm_mempool_id_enum_type  dsm_pool_id;

  BT_MSG_SIG( "BT SD: TX Err Resp TID %x EC %x", tid, ec, 0 );

  if ( ( dsm_ptr = bt_get_free_dsm_ptr( BT_TL_L2CAP,
                                        BT_SD_MAX_PDU_LEN ) ) == NULL )
  {
    BT_MSG_HIGH( "BT SD: Unable to get DSM buffer", 0, 0, 0 );
    return;
  }

  dsm_pool_id = DSM_DS_POOL_SIZE( BT_SD_MAX_PDU_LEN );

  /* Insert the PDU header */
  pdu_hdr.pid = BT_SD_PDU_ERROR_RESP;
  pdu_hdr.tid = SWAB16( tid );
  pdu_hdr.length = SWAB16( 0x0002 );    /* ErrorCode */

  bt_dsm_pushdown_tail( &dsm_ptr, (void*)&pdu_hdr, sizeof(pdu_hdr),
                        DSM_DS_SMALL_ITEM_POOL );

  /* Insert ErrorCode */
  uint16_to_send = SWAB16( ec );
  bt_sd_dsm_push16( &dsm_ptr, &uint16_to_send, dsm_pool_id );

#ifdef BT_SWDEV_QXDM_LOG_MSG
  bt_sd_sig_tx_logging_function( bt_sd_app_id, cid, dsm_ptr );
#endif /* BT_SWDEV_QXDM_LOG_MSG */

  bt_cmd_l2_write( bt_sd_app_id, cid, dsm_ptr, NULL );
}


/*===========================================================================

FUNCTION
  bt_sd_send_service_search_resp

DESCRIPTION
  Send a SDP Service Search Response

===========================================================================*/

void bt_sd_send_service_search_resp(
  uint16   cid,
  uint16   tid,
  uint16   srv_rec_count
)
{
  dsm_item_type*            dsm_ptr;
  bt_sd_pdu_hdr_type        pdu_hdr;
  bt_sd_srv_rec_type*       srv_rec_ptr;
  uint32                    uint32_to_send;
  uint16                    uint16_to_send;
  uint8                     info_len = 0;
  dsm_mempool_id_enum_type  dsm_pool_id;

  BT_MSG_SIG( "BT SD: TX SS Resp TID %x Cnt %x", tid, srv_rec_count, 0 );

  if ( ( dsm_ptr = bt_get_free_dsm_ptr( BT_TL_L2CAP,
                                        BT_SD_MAX_PDU_LEN ) ) == NULL )
  {
    BT_MSG_HIGH( "BT SD: Unable to get DSM buffer", 0, 0, 0 );
    return;
  }

  dsm_pool_id = DSM_DS_POOL_SIZE( BT_SD_MAX_PDU_LEN );

  /* Insert the PDU header */
  pdu_hdr.pid = BT_SD_PDU_SERVICE_SEARCH_RESP;
  pdu_hdr.tid = SWAB16( tid );
  pdu_hdr.length = 2                 /* TotalServiceRecordCount   */
                      + 2                 /* CurrentServiceRecordCount */
                      + 4 * srv_rec_count /* ServiceRecordHandleList   */
                      + 1;                /* Continuation State        */
  pdu_hdr.length = SWAB16( pdu_hdr.length );
  bt_dsm_pushdown_tail( &dsm_ptr, (void*)&pdu_hdr, sizeof(pdu_hdr),
                        DSM_DS_SMALL_ITEM_POOL );

  /* Insert TotalServiceRecordCount */
  uint16_to_send = SWAB16( srv_rec_count );
  bt_sd_dsm_push16( &dsm_ptr, &uint16_to_send, dsm_pool_id );

  /* Insert CurrentServiceRecordCount */
  uint16_to_send = SWAB16( srv_rec_count );
  bt_sd_dsm_push16( &dsm_ptr, &uint16_to_send, dsm_pool_id );

  /* Insert ServiceRecordHandleList */
  bt_sd_srv_rec_iter.init( BT_SD_DB_TYPE_LOCAL_DB, BT_SD_ITER_MATCH_ONLY );
  for ( srv_rec_ptr = ( bt_sd_srv_rec_type* ) bt_sd_srv_rec_iter.first();
                                       /* Explicitly cast away the constness */
        srv_rec_ptr != 0;
        srv_rec_ptr = ( bt_sd_srv_rec_type* ) bt_sd_srv_rec_iter.next() )
                                      /* Explicitly cast away the constness */
  {
    uint32_to_send = SWAB32( srv_rec_ptr->srv_rec_id );
    bt_sd_dsm_push32( &dsm_ptr, &uint32_to_send, dsm_pool_id );
  }

  /* Insert ContinuationState */
  bt_sd_dsm_push8( &dsm_ptr, &info_len, dsm_pool_id );

#ifdef BT_SWDEV_QXDM_LOG_MSG
  bt_sd_sig_tx_logging_function( bt_sd_app_id, cid, dsm_ptr );
#endif /* BT_SWDEV_QXDM_LOG_MSG */

  bt_cmd_l2_write( bt_sd_app_id, cid, dsm_ptr, NULL );
}


/*===========================================================================

FUNCTION
  bt_sd_send_sa_or_ssa_resp

DESCRIPTION
  Send a SDP Service Attriubte or SDP Service Search Attribute Response

===========================================================================*/

void bt_sd_send_sa_or_ssa_resp(
  bt_sd_pdu_type            pdu_type,
  uint16                    cid,
  uint16                    tid,
  uint16                    max_attr_byte_count,
  uint8*                    cont_state_info,
  uint8                     info_len
)
{
  dsm_item_type*                       dsm_ptr;
  bt_sd_pdu_hdr_type                   pdu_hdr;
  uint16                               pdu_base_len;
  uint16                               tot_attr_byte_count;
  uint16                               attr_list_byte_count;
  uint16                               attr_list_de_hdr_len = 0;
  uint16                               cont_state_tid;
  uint16                               cont_state_attr_byte_count;
  uint16                               uint16_to_send;
  uint8                                cont_state_info_len = 0;
  uint8                                de_hdr;
  dsm_mempool_id_enum_type             dsm_pool_id;
  bt_sd_conn_entry_type*               conn_entry_ptr;
  uint16                               max_pdu_len;

  tot_attr_byte_count = dsm_length_packet( attr_list_dsm_ptr );

  BT_MSG_SIG( "BT SD: TX SA/SSA Resp Type %x TID %x Count %x",
              pdu_type, tid, tot_attr_byte_count );

  /* Find the connection entry */
  conn_entry_ptr = bt_sd_find_conn_entry_by_cid( cid );
  if ( conn_entry_ptr == NULL )
  {
    BT_ERR( "BT SD: CID %x not found in conn table", cid, 0, 0 );
    dsm_free_packet( &attr_list_dsm_ptr );
    bt_sd_send_error_resp( cid, tid, BT_SD_EV_ERROR_INSUFFICIENT_RESOURCE );
    return;
  }

  max_pdu_len = MIN( conn_entry_ptr->out_mtu, BT_SD_MAX_PDU_LEN );

  if ( info_len == 0 )
  {
    /* Remember the current transaction ID.  This is needed when continuation
     * state information is to be sent
     */
    cont_state_tid = tid;

    /* Decide how many bytes is to be used for the data size in the data element
       header of AttributeLists */
    if ( tot_attr_byte_count <= 0xFF )
    {
      attr_list_de_hdr_len = 2;
    }
    else
    {
      attr_list_de_hdr_len = 3;
    }
  }
  else
  {
    /* Extract from cont_state_info the transaction ID and make sure it
     * matches the one that originates the search
     * *** FIXME (ryuen) ***
     */
    cont_state_tid = ( cont_state_info[0] << 8 ) | cont_state_info[1];

    cont_state_attr_byte_count =
      ( cont_state_info[2] << 8 ) | cont_state_info[3];

    if ( cont_state_attr_byte_count != tot_attr_byte_count )
    {
      bt_sd_send_error_resp( cid, tid,
                             BT_SD_EV_ERROR_INVALID_CONTINUATION_STATE );
      return;
    }
  }

  if ( ( dsm_ptr = bt_get_free_dsm_ptr( BT_TL_L2CAP,
                                        max_pdu_len ) ) == NULL )
  {
    BT_ERR( "BT SD: Unable to get DSM buffer", 0, 0, 0 );
    bt_sd_send_error_resp( cid, tid, BT_SD_EV_ERROR_INSUFFICIENT_RESOURCE );
    return;
  }

  dsm_pool_id = DSM_DS_POOL_SIZE( max_pdu_len );

  /* Insert the PDU header */
  pdu_hdr.pid = pdu_type;
  pdu_hdr.tid = SWAB16( tid );
  pdu_hdr.length = 2   /* AttributeListByteCount      */
                      + attr_list_de_hdr_len
                      + 1;  /* Continuation State Info Len */

  /* Make sure AttributeListByteCount <= MaximumAttributeByteCount */
  attr_list_byte_count = tot_attr_byte_count;
  if ( attr_list_byte_count > max_attr_byte_count )
  {
    attr_list_byte_count = max_attr_byte_count;
    cont_state_info_len = 4;
  }

  /* Make sure AttributeListByteCount fits within the SDP PDU */
  pdu_base_len = sizeof( bt_sd_pdu_hdr_type ) + pdu_hdr.length;
  if ( ( pdu_base_len + attr_list_byte_count ) > max_pdu_len )
  {
    attr_list_byte_count = max_pdu_len
                   - pdu_base_len
                   - 4;      /* Continuation State Info */
    cont_state_info_len = 4;
  }

  /* Update the PDU length */
  pdu_hdr.length += attr_list_byte_count + cont_state_info_len;
  pdu_hdr.length = SWAB16( pdu_hdr.length );

  bt_dsm_pushdown_tail( &dsm_ptr, (void*)&pdu_hdr, sizeof(pdu_hdr),
                        DSM_DS_SMALL_ITEM_POOL );

  /* Insert AttributeListByteCount */
  uint16_to_send = attr_list_byte_count + attr_list_de_hdr_len;
  uint16_to_send = SWAB16( uint16_to_send );
  bt_sd_dsm_push16( &dsm_ptr, &uint16_to_send, dsm_pool_id );

  switch( attr_list_de_hdr_len )
  {
    case 2:
      /* Insert data element sequence header for AttributeList */
      de_hdr = ( BT_SD_DATA_ELEMENT_SEQUENCE << 3 ) | 5;
      bt_sd_dsm_push8( &dsm_ptr, &de_hdr, dsm_pool_id );

      /* Insert data element sequence length for AttributeList */
      bt_sd_dsm_push8( &dsm_ptr, (uint8*) &tot_attr_byte_count, dsm_pool_id );
      break;
    case 3:
      /* Insert data element sequence header for AttributeList */
      de_hdr = ( BT_SD_DATA_ELEMENT_SEQUENCE << 3 ) | 6;
      bt_sd_dsm_push8( &dsm_ptr, &de_hdr, dsm_pool_id );

      /* Insert data element sequence length for AttributeList */
      uint16_to_send = SWAB16( tot_attr_byte_count );
      bt_sd_dsm_push16( &dsm_ptr, &uint16_to_send, dsm_pool_id );
  }

  if ( attr_list_byte_count > 0 )
  {
    dsm_item_type *addr_list_dup_ptr;

    if ( dsm_dup_packet( &addr_list_dup_ptr, attr_list_dsm_ptr, 0,
                         attr_list_byte_count )
         != attr_list_byte_count )
    {
      dsm_free_packet( &dsm_ptr );
      dsm_free_packet( &addr_list_dup_ptr );
      bt_sd_send_error_resp( cid, tid, BT_SD_EV_ERROR_INSUFFICIENT_RESOURCE );
      return;
    }
    dsm_append( &dsm_ptr, &addr_list_dup_ptr );
    dsm_pullup( &attr_list_dsm_ptr, NULL, attr_list_byte_count );
  }

  /* Insert ContinuationState */
  bt_sd_dsm_push8( &dsm_ptr, &cont_state_info_len, dsm_pool_id );

  if ( cont_state_info_len > 0 )
  {
    /* Insert TID */
    uint16_to_send = SWAB16( cont_state_tid );
    bt_sd_dsm_push16( &dsm_ptr, &uint16_to_send, dsm_pool_id );

    /* Insert attribute byte count */
    cont_state_attr_byte_count = dsm_length_packet( attr_list_dsm_ptr );
    uint16_to_send = SWAB16( cont_state_attr_byte_count );
    bt_sd_dsm_push16( &dsm_ptr, &uint16_to_send, dsm_pool_id );
  }

#ifdef BT_SWDEV_QXDM_LOG_MSG
  bt_sd_sig_tx_logging_function( bt_sd_app_id, cid, dsm_ptr );
#endif /* BT_SWDEV_QXDM_LOG_MSG */

  bt_cmd_l2_write( bt_sd_app_id, cid, dsm_ptr, NULL );
}


/*===========================================================================

FUNCTION
  bt_sd_process_error_resp

DESCRIPTION
  Process a SDP Error Response

===========================================================================*/

boolean bt_sd_process_error_resp(
  word                 cid,
  dsm_item_type**      dsm_ptr_ptr,
  bt_sd_pdu_hdr_type*  pdu_hdr_ptr
)
{
  bt_sd_conn_entry_type*  conn_entry_ptr;
  uint16                  error_code;

  BT_MSG_SIG( "BT SD: RX Err Resp TID %x", pdu_hdr_ptr->tid, 0, 0 );

  conn_entry_ptr = bt_sd_find_conn_entry_by_cid( cid );

  if ( ( conn_entry_ptr == 0 ) || ( ! conn_entry_ptr->req_pending ) )
  {
    BT_MSG_HIGH( "BT SD: Unexpected response", 0, 0, 0 );
    return FALSE;
  }

  /* Clear Request Timeout Timer */
  rex_clr_timer( &conn_entry_ptr->tmr_loc_initd );

  /* Extract the error code */
  if ( ! bt_sd_dsm_pullup( dsm_ptr_ptr, (void *) &error_code, 2 ) )
  {
    bt_sd_send_error_resp_event( conn_entry_ptr, BT_SD_EV_ERROR_UNKNOWN );
    return FALSE;
  }

  error_code = SWAB16( error_code );

  /* FIXME (ryuen)
   * Might need to free the dsm buffer manually since we are not extracting
   * the error information (N bytes)
   */

  /* Send BT_EV_SD_ERROR_RESP */
  bt_sd_send_error_resp_event( conn_entry_ptr, error_code );

  /* NOTE: Setting of the Idle Timer and clearing of the pending request
   *       are being take care of by bt_sd_send_error_resp_event
   */

  return TRUE;
}


/*===========================================================================

FUNCTION
  bt_sd_process_service_search_req

DESCRIPTION
  Process a SDP Service Search Request

===========================================================================*/

boolean bt_sd_process_service_search_req(
  word                 cid,
  dsm_item_type**      dsm_ptr_ptr,
  bt_sd_pdu_hdr_type*  pdu_hdr_ptr
)
{
  uint8                     info_len;
  uint16                    srv_rec_count;
  uint16                    matched_byte_count;
  uint16                    byte_parsed, total_byte_parsed = 0;
  bt_sd_srv_srch_pat_type   srch_pat;

  BT_MSG_SIG( "BT SD: RX SS Req TID %x Len %x",
              pdu_hdr_ptr->tid, pdu_hdr_ptr->length, 0 );

  /* Check for invalid PDU length */
  if ( pdu_hdr_ptr->length != dsm_length_packet( *dsm_ptr_ptr ) )
  {
    bt_sd_send_error_resp( cid,
                           pdu_hdr_ptr->tid,
                           BT_SD_EV_ERROR_INVALID_PDU_SIZE );
    return FALSE;
  }

  /*
   * Initialize the search pattern
   */
  srch_pat.max_srv_rec_count = 0;
  srch_pat.max_attr_byte_count = 0;
  srch_pat.num_uuid = 0;
  srch_pat.num_uuid128 = 0;
  srch_pat.num_attr_id = 0;
  srch_pat.syncml_flag = BT_SD_DB_NOT_SYNCML_V;
  srch_pat.uuid128[0] = bt_sd_null_uuid128;

  /*
   * Parse the ServiceSearchPattern field
   */
  if ( ! bt_sd_parse_service_search_pattern( dsm_ptr_ptr,
                                             &srch_pat, &byte_parsed ) )
  {
    bt_sd_send_error_resp( cid, pdu_hdr_ptr->tid,
                           BT_SD_EV_ERROR_INVALID_REQUEST_SYNTAX );
    return FALSE;
  }
  total_byte_parsed += byte_parsed;

  /*
   * Parse the MaximumServiceRecordCount field
   */
  if ( ! bt_sd_dsm_pullup( dsm_ptr_ptr, (void *) &(srch_pat.max_srv_rec_count), 2 ) )
  {
    bt_sd_send_error_resp( cid, pdu_hdr_ptr->tid,
                           BT_SD_EV_ERROR_INVALID_REQUEST_SYNTAX );
    return FALSE;
  }
  srch_pat.max_srv_rec_count = SWAB16( srch_pat.max_srv_rec_count );
  total_byte_parsed += 2;

  /*
   * Parse the ContinuationState field
   */
  if ( ! bt_sd_dsm_pullup( dsm_ptr_ptr, (void *) &info_len, 1 ) )
  {
    bt_sd_send_error_resp( cid, pdu_hdr_ptr->tid,
                           BT_SD_EV_ERROR_INVALID_REQUEST_SYNTAX );
    return FALSE;
  }
  total_byte_parsed++;

  /* For now, there is no need to implement continuation state for service
   * search request, because a response can contain up to a maximum of 13
   * service record handles, and that should be plenty */
  if ( info_len != 0 )
  {
    BT_MSG_HIGH( "BT SD: Unexpected cont state info len %d", info_len, 0, 0 );
    bt_sd_send_error_resp( cid, pdu_hdr_ptr->tid,
                           BT_SD_EV_ERROR_INVALID_CONTINUATION_STATE );
    return FALSE;
  }

  if ( total_byte_parsed != pdu_hdr_ptr->length )
  {
    BT_MSG_HIGH( "BT SD: Parameter length mismatch %d %d",
                 total_byte_parsed, pdu_hdr_ptr->length, 0 );
    bt_sd_send_error_resp( cid, pdu_hdr_ptr->tid,
                           BT_SD_EV_ERROR_INVALID_PDU_SIZE );
    return FALSE;
  }

  matched_byte_count = bt_sd_match_service_record( &srch_pat );
  srv_rec_count = matched_byte_count / 4;

  /* Send response */
  bt_sd_send_service_search_resp( cid, pdu_hdr_ptr->tid, srv_rec_count );

  return TRUE;
}


/*===========================================================================

FUNCTION
  bt_sd_process_service_search_resp

DESCRIPTION
  Process a SDP Service Search Response

===========================================================================*/

boolean bt_sd_process_service_search_resp(
  word                 cid,
  dsm_item_type**      dsm_ptr_ptr,
  bt_sd_pdu_hdr_type*  pdu_hdr_ptr
)
{
  uint8                   info_len;
  uint16                  i;
  uint16                  tot_srv_rec_count;
  uint16                  cur_srv_rec_count;
  uint16                  tot_byte_parsed = 0;
  uint32                  srv_rec_handle;
  bt_sd_srv_rec_type*     srv_rec_ptr;
  bt_sd_conn_entry_type*  conn_entry_ptr;
  bt_ev_msg_type          ev_srv_srch_resp;
  uint8  cont_state_info[ BT_SD_MAX_CONTINUATION_STATE_INFO_LEN ];

  BT_MSG_SIG( "BT SD: RX SS Resp TID %x Len %x",
              pdu_hdr_ptr->tid, pdu_hdr_ptr->length, 0 );

  conn_entry_ptr = bt_sd_find_conn_entry_by_cid( cid );

  if ( ( conn_entry_ptr == 0 ) || ( ! conn_entry_ptr->req_pending ) )
  {
    BT_MSG_HIGH( "BT SD: Unexpected response", 0, 0, 0 );
    return FALSE;
  }

  /* Clear Request Timeout Timer */
  rex_clr_timer( &conn_entry_ptr->tmr_loc_initd );

  /* Check for invalid PDU length */
  if ( pdu_hdr_ptr->length != dsm_length_packet( *dsm_ptr_ptr ) )
  {
    BT_ERR("BT SD: invalid PDU length %d", pdu_hdr_ptr->length, 0, 0 );
    bt_sd_send_error_resp_event(
      conn_entry_ptr,
      BT_SD_EV_ERROR_INVALID_PDU_SIZE
    );
    return FALSE;
  }

  /* Check for continuation */
  if ( conn_entry_ptr->num_resp_recv == 0 )
  {
    /* This is the very first response */
    bt_sd_initialize_database( BT_SD_DB_TYPE_SRV_SRCH_RESULT_DB );
  }

  /* FIXME (ryuen)
   * Use the cid to find the corresponding memory location where the
   * result will be stored.  For now, since only one connection is allowed,
   * save result to known location
   */

  /* Parse TotalServiceRecordCount */
  if ( ! bt_sd_dsm_pullup( dsm_ptr_ptr, (void *) &tot_srv_rec_count, 2 ) )
  {
    BT_ERR("BT SD: bad tot_srv_rec_count", 0, 0, 0 );

    /* Send BT_EV_SD_ERROR_RESP */
    bt_sd_send_error_resp_event(
      conn_entry_ptr,
      BT_SD_EV_ERROR_INVALID_RESPONSE_SYNTAX
    );
    return FALSE;
  }
  tot_srv_rec_count = SWAB16( tot_srv_rec_count );
  tot_byte_parsed += 2;

  /* Parse CurrentServiceRecordCount */
  if ( ! bt_sd_dsm_pullup( dsm_ptr_ptr, (void *) &cur_srv_rec_count, 2 ) )
  {
    BT_ERR("BT SD: bad cur_srv_rec_count", 0, 0, 0 );

    /* Send BT_EV_SD_ERROR_RESP */
    bt_sd_send_error_resp_event(
      conn_entry_ptr,
      BT_SD_EV_ERROR_INVALID_RESPONSE_SYNTAX
    );
    return FALSE;
  }
  cur_srv_rec_count = SWAB16( cur_srv_rec_count );
  tot_byte_parsed += 2;

  /* Parse ServiceRecordHandleList */
  for ( i = 0; i < cur_srv_rec_count; i++ )
  {
    if ( ! bt_sd_dsm_pullup( dsm_ptr_ptr, (void *) &srv_rec_handle, 4 ) )
    {
      BT_ERR("BT SD: bad srv_rec_handle", 0, 0, 0 );

      /* Send BT_EV_SD_ERROR_RESP */
      bt_sd_send_error_resp_event(
        conn_entry_ptr,
        BT_SD_EV_ERROR_INVALID_RESPONSE_SYNTAX
      );
      return FALSE;
    }
    srv_rec_handle = SWAB32( srv_rec_handle );
    srv_rec_ptr =
      bt_sd_alloc_service_record( BT_SD_DB_TYPE_SRV_SRCH_RESULT_DB );

    if ( srv_rec_ptr )
    {
      srv_rec_ptr->srv_rec_id = srv_rec_handle;
    }
    tot_byte_parsed += 4;
  }

  /* Parse ContinuationState */
  if ( ! bt_sd_dsm_pullup( dsm_ptr_ptr, (void *) &info_len, 1 ) )
  {
    BT_ERR("BT SD: bad info_len", 0, 0, 0 );
    /* Send BT_EV_SD_ERROR_RESP */
    bt_sd_send_error_resp_event(
      conn_entry_ptr,
      BT_SD_EV_ERROR_INVALID_RESPONSE_SYNTAX
    );
    return FALSE;
  }
  tot_byte_parsed++;

  if ( info_len > 0 )
  {
    if ( ( info_len > BT_SD_MAX_CONTINUATION_STATE_INFO_LEN ) ||
         ( ! bt_sd_dsm_pullup( dsm_ptr_ptr,
                               (void *) cont_state_info, info_len ) ) )
    {
      BT_ERR("BT SD: bad cont_state_info", 0, 0, 0 );

      /* Send BT_EV_SD_ERROR_RESP */
      bt_sd_send_error_resp_event(
        conn_entry_ptr,
        BT_SD_EV_ERROR_INVALID_RESPONSE_SYNTAX
      );
      return FALSE;
    }

    tot_byte_parsed += info_len;
  }

  if ( tot_byte_parsed != pdu_hdr_ptr->length )
  {
    BT_ERR("BT SD: tot_byte_parsed != PDU len", 0, 0, 0 );

    bt_sd_send_error_resp_event(
      conn_entry_ptr,
      BT_SD_EV_ERROR_INVALID_RESPONSE_SYNTAX
    );
    return FALSE;
  }
  /* compare old cont info with new one */
  if(memcmp(conn_entry_ptr->cont_state_info,cont_state_info,
                                        BT_SD_MAX_CONTINUATION_STATE_INFO_LEN) !=0)
  {
      memcpy(conn_entry_ptr->cont_state_info,cont_state_info,
                                        BT_SD_MAX_CONTINUATION_STATE_INFO_LEN);      
  }
  else
  {
      BT_MSG_SIG( "BT SD: Same Cont state",0,0,0 );
      conn_entry_ptr->same_cont_state_count++;
  }


  if (( info_len == 0 ) || (conn_entry_ptr->same_cont_state_count >
                             BT_SD_MAX_SAME_CONTINUATION_HOPS_ALLOWED))
  {
    /* Make sure the number of service record handles we got back
     * matches TotalServiceRecordCount */
    if ( bt_sd_get_num_service_record(
           BT_SD_DB_TYPE_SRV_SRCH_RESULT_DB ) != tot_srv_rec_count )
    {
      BT_ERR("BT SD: tot_srv_rec_count != # hndls rxd", 0, 0, 0 );

      bt_sd_send_error_resp_event(
        conn_entry_ptr,
        BT_SD_EV_ERROR_INVALID_RESPONSE_SYNTAX
      );
      return FALSE;
    }

    BT_MSG_API( "BT SD EV TX: SD SS Resp Cnt1 %x Cnt2 %x AID %x",
                tot_srv_rec_count, 0, conn_entry_ptr->bt_app_id );

    /* Send BT_EV_SD_SRV_SRCH_RESP */
    ev_srv_srch_resp.ev_hdr.ev_type = BT_EV_SD_SERVICE_SEARCH_RESP;
    ev_srv_srch_resp.ev_hdr.bt_app_id = conn_entry_ptr->bt_app_id;
    ev_srv_srch_resp.ev_msg.ev_sd_service_search_resp.bd_addr =
      conn_entry_ptr->bd_addr;
    ev_srv_srch_resp.ev_msg.ev_sd_service_search_resp.srv_rec_count =
      tot_srv_rec_count;
    ev_srv_srch_resp.ev_msg.ev_sd_service_search_resp.attr_byte_count =
      0;

    /* Insert the servce record handle */
    bt_sd_srv_rec_iter.init( BT_SD_DB_TYPE_SRV_SRCH_RESULT_DB,
                             BT_SD_ITER_ALL );
    for ( srv_rec_ptr = (bt_sd_srv_rec_type*) bt_sd_srv_rec_iter.first(), i = 0;
          srv_rec_ptr != 0;
          srv_rec_ptr = (bt_sd_srv_rec_type*) bt_sd_srv_rec_iter.next(), i++ )
    {
      ev_srv_srch_resp.ev_msg.ev_sd_service_search_resp.
        srv_rec_handle_list[i] = srv_rec_ptr->srv_rec_id;
    }
    for ( ; i < BT_SD_MAX_NUM_OF_SRV_SRCH_PAT_UUID; i++ )
    {
      ev_srv_srch_resp.ev_msg.ev_sd_service_search_resp.
        srv_rec_handle_list[i] = 0;
    }

    bt_ec_send_event( &ev_srv_srch_resp );

    /* Set Idle Timer */
    rex_set_timer( &conn_entry_ptr->tmr_loc_initd, BT_SD_IDLE_TIMEOUT_MS );

    /* Done */
    bt_sd_clear_pending_request( conn_entry_ptr );
  }
  else /* Need to send more requests */
  {
    conn_entry_ptr->num_resp_recv++;

    /* Send another request with continuation information */
    bt_sd_send_pending_request( conn_entry_ptr, info_len, cont_state_info );
  }

  return TRUE;
}


/*===========================================================================

FUNCTION
  bt_sd_process_service_attribute_req

DESCRIPTION
  Process a SDP Service Attribute Request

===========================================================================*/

boolean bt_sd_process_service_attribute_req(
  word                 cid,
  dsm_item_type**      dsm_ptr_ptr,
  bt_sd_pdu_hdr_type*  pdu_hdr_ptr
)
{
  uint8                     info_len;
  uint32                    srv_rec_handle;
  uint32                    dsm_pkt_len;
  uint16                    matched_attr_byte_count = 0;
  uint16                    byte_parsed, total_byte_parsed = 0;
  bt_sd_srv_rec_type*       srv_rec_ptr;
  bt_sd_srv_attr_rec_type*  srv_attr_ptr;
  bt_sd_srv_srch_pat_type   srch_pat;
  dsm_mempool_id_enum_type  dsm_pool_id;
  uint8   cont_state_info[BT_SD_MAX_CONTINUATION_STATE_INFO_LEN];

  BT_MSG_SIG( "BT SD: RX SA Req TID %x Len %x",
              pdu_hdr_ptr->tid, pdu_hdr_ptr->length, 0 );

  /* Check for invalid PDU length */
  if ( pdu_hdr_ptr->length != dsm_length_packet( *dsm_ptr_ptr ) )
  {
    bt_sd_send_error_resp( cid,
                           pdu_hdr_ptr->tid,
                           BT_SD_EV_ERROR_INVALID_PDU_SIZE );
    return FALSE;
  }

  /*
   * Initialize the search pattern
   */
  srch_pat.max_srv_rec_count = 0;
  srch_pat.max_attr_byte_count = 0;
  srch_pat.num_uuid = 0;
  srch_pat.num_uuid128 = 0;
  srch_pat.num_attr_id = 0;
  srch_pat.syncml_flag = BT_SD_DB_NOT_SYNCML_V;
  srch_pat.uuid128[0] = bt_sd_null_uuid128;

  /*
   * Parse the ServiceRecordHandle field
   */
  if ( ! bt_sd_dsm_pullup( dsm_ptr_ptr, (void *) &srv_rec_handle, 4 ) )
  {
    bt_sd_send_error_resp( cid, pdu_hdr_ptr->tid,
                           BT_SD_EV_ERROR_INVALID_REQUEST_SYNTAX );
    return FALSE;
  }
  srv_rec_handle = SWAB32( srv_rec_handle );
  total_byte_parsed += 4;

  /*
   * Parse the MaximumAttributeByteCount field
   */
  if ( ! bt_sd_dsm_pullup( dsm_ptr_ptr, (void*) &(srch_pat.max_attr_byte_count), 2 ) )
  {
    bt_sd_send_error_resp( cid, pdu_hdr_ptr->tid,
                           BT_SD_EV_ERROR_INVALID_REQUEST_SYNTAX );
    return FALSE;
  }
  srch_pat.max_attr_byte_count = SWAB16( srch_pat.max_attr_byte_count );
  total_byte_parsed += 2;

  /*
   * Parse the AttributeIDList field
   */
  if ( ! bt_sd_parse_attribute_id_list( dsm_ptr_ptr, &srch_pat, &byte_parsed ) )
  {
    bt_sd_send_error_resp( cid, pdu_hdr_ptr->tid,
                           BT_SD_EV_ERROR_INVALID_REQUEST_SYNTAX );
    return FALSE;
  }
  total_byte_parsed += byte_parsed;

  /*
   * Parse the ContinuationState field
   */
  if ( ! bt_sd_dsm_pullup( dsm_ptr_ptr, (void *) &info_len, 1 ) )
  {
    bt_sd_send_error_resp( cid, pdu_hdr_ptr->tid,
                           BT_SD_EV_ERROR_INVALID_REQUEST_SYNTAX );
    return FALSE;
  }
  total_byte_parsed++;

  if ( info_len > 0 )
  {
    if ( ( info_len > BT_SD_MAX_CONTINUATION_STATE_INFO_LEN ) ||
         ( info_len != 4 ) )
    {
      BT_MSG_HIGH( "BT SD: Unexpected cont state info len %d", info_len, 0, 0 );
      bt_sd_send_error_resp( cid, pdu_hdr_ptr->tid,
                             BT_SD_EV_ERROR_INVALID_CONTINUATION_STATE );
      return FALSE;
    }

    if ( ! bt_sd_dsm_pullup( dsm_ptr_ptr, (void *) cont_state_info, info_len ) )
    {
      bt_sd_send_error_resp( cid, pdu_hdr_ptr->tid,
                             BT_SD_EV_ERROR_INVALID_REQUEST_SYNTAX );
      return FALSE;
    }
    total_byte_parsed += info_len;
  }

  if ( total_byte_parsed != pdu_hdr_ptr->length )
  {
    BT_MSG_HIGH( "BT SD: Parameter length mismatch %d %d",
                 total_byte_parsed, pdu_hdr_ptr->length, 0 );
    bt_sd_send_error_resp( cid, pdu_hdr_ptr->tid,
                           BT_SD_EV_ERROR_INVALID_PDU_SIZE );
    return FALSE;
  }

  srv_rec_ptr = bt_sd_find_service_record_by_handle( srv_rec_handle );

  if ( ( srv_rec_ptr == 0 ) || ( ! srv_rec_ptr->enabled_flag ) )
  {
    BT_MSG_HIGH( "BT SD: Unable to find service record %ld",
                 srv_rec_handle, 0, 0 );
    bt_sd_send_error_resp( cid, pdu_hdr_ptr->tid,
                           BT_SD_EV_ERROR_INVALID_SERVICE_RECORD_HANDLE );
    return FALSE;
  }

  /* Start a new search if this is not the continuation of a former request */
  if ( info_len == 0 )
  {
    matched_attr_byte_count =
      bt_sd_match_service_attribute( srv_rec_ptr, &srch_pat );

    if ( dsm_length_packet( attr_list_dsm_ptr ) > 0 )
    {
      dsm_free_packet( &attr_list_dsm_ptr );
    }
    attr_list_dsm_ptr = NULL;

    if ( matched_attr_byte_count > 0 )
    {
      /* Push all matched attributes onto a DSM */
      if ( ( attr_list_dsm_ptr =
               bt_get_free_dsm_ptr( BT_TL_L2CAP,
                                    matched_attr_byte_count ) ) == NULL )
      {
        BT_ERR( "BT SD: Unable to get DSM buffer", 0, 0, 0 );
        bt_sd_send_error_resp( cid, pdu_hdr_ptr->tid,
                               BT_SD_EV_ERROR_INSUFFICIENT_RESOURCE );
        return FALSE;
      }

      dsm_pool_id = DSM_DS_POOL_SIZE( matched_attr_byte_count );

      bt_sd_srv_attr_iter.init( srv_rec_ptr, BT_SD_ITER_MATCH_ONLY );
      for ( srv_attr_ptr =
              ( bt_sd_srv_attr_rec_type* ) bt_sd_srv_attr_iter.first();
            srv_attr_ptr != 0;
            srv_attr_ptr =
              ( bt_sd_srv_attr_rec_type* ) bt_sd_srv_attr_iter.next() )
      {
        bt_sd_add_attribute_to_dsm_buffer( srv_attr_ptr,
                                           attr_list_dsm_ptr,
                                           dsm_pool_id );
      }

      dsm_pkt_len = dsm_length_packet( attr_list_dsm_ptr );
      if ( dsm_pkt_len != matched_attr_byte_count )
      {
        BT_ERR( "BT SD: DSM length mismatch %d %d",
                dsm_pkt_len,
                matched_attr_byte_count, 0 );
        dsm_free_packet( &attr_list_dsm_ptr );
        bt_sd_send_error_resp( cid, pdu_hdr_ptr->tid,
                               BT_SD_EV_ERROR_INSUFFICIENT_RESOURCE );
        return FALSE;
      }
    }
  }

  /* Send response */
  bt_sd_send_sa_or_ssa_resp( BT_SD_PDU_SERVICE_ATTRIBUTE_RESP,
                             cid, pdu_hdr_ptr->tid,
                             srch_pat.max_attr_byte_count,
                             cont_state_info, info_len );

  return TRUE;
}


/*===========================================================================

FUNCTION
  bt_sd_parse_attribute_list

DESCRIPTION
  Parse the AttributeList parameter of a SDP Service Attribute Response
  or a SDP Service Attribute Search Response

===========================================================================*/

uint16 bt_sd_parse_attribute_list
(
  dsm_item_type**      dsm_ptr_ptr,
  bt_sd_srv_rec_type*  srv_rec_ptr
)
{
  uint8                                         uint8_val;
  uint16                                        tot_byte_parsed = 0;
  uint16                                        cur_byte_parsed = 0;
  uint16                                        byte_parsed;
  uint16                                        matched_attr_byte_count;
  uint16                                        attr_id;
  bt_sd_srv_attr_enum_type                      attr_type;
  bt_sd_srv_attr_rec_type*                      srv_attr_ptr;
  bt_sd_data_element_header_parse_result_type   parse_result;
  uint8                                         syncml_flag;

  /* Parse data element sequence header for AttributeList */
  if ( ( ! bt_sd_parse_data_element_header( dsm_ptr_ptr, &parse_result ) ) ||
       ( parse_result.type_desc != BT_SD_DATA_ELEMENT_SEQUENCE ) )
  {
    BT_ERR( "BT SD: parse AL: wrong AL header T %x, S %x, P %x", 
            parse_result.type_desc, parse_result.data_size, 
            parse_result.byte_parsed );
    return 0;
  }

  tot_byte_parsed += parse_result.byte_parsed;

  matched_attr_byte_count = parse_result.data_size;

  while ( cur_byte_parsed < matched_attr_byte_count )
  {
    /* Parse data element header for attribute ID */
    if ( ( ! bt_sd_parse_data_element_header( dsm_ptr_ptr, &parse_result ) ) ||
         ( parse_result.type_desc != BT_SD_DATA_ELEMENT_UNSIGNED_INT )   ||
         ( parse_result.data_size != 2 ) )
    {
      BT_ERR( "BT SD: parse AL: wrong AID header T %x, S %x, P %x", 
            parse_result.type_desc, parse_result.data_size, 
            parse_result.byte_parsed );
      return 0;
    }
    cur_byte_parsed += parse_result.byte_parsed;

    /* Parse AttributeID */
    if ( ! bt_sd_dsm_pullup( dsm_ptr_ptr, (void*) &attr_id, 2 ) )
    {
      BT_ERR( "BT SD: parse AL: DSM PU 1 fail", 0, 0, 0 );
      return 0;
    }
    attr_id = SWAB16( attr_id );
    cur_byte_parsed += 2;

    if ( ( attr_type = bt_sd_get_attr_type( attr_id ) ) == BT_SD_ATTR_TYPE_UNKNOWN )
    {
      BT_ERR( "BT SD: parse AL: unknown attr type: attr_id=%x attr_type:%x",
              attr_id, attr_type, 0 );
      return 0;
    }

    srv_attr_ptr = bt_sd_service_record_add_attribute( srv_rec_ptr,
                                                       attr_id,
                                                       attr_type );

    if ( srv_attr_ptr )
    {
      /* Parse attribute value */
      if ( ( byte_parsed =
               bt_sd_parse_attribute_value(
                 dsm_ptr_ptr, attr_type, srv_attr_ptr, &syncml_flag ) ) > 0 )
      {
        cur_byte_parsed += byte_parsed;
        srv_rec_ptr->matched_attr_byte_count = cur_byte_parsed;
        srv_rec_ptr->syncml_flag = syncml_flag;
      }
      else
      {
        BT_ERR( "BT SD: parse AL: parse attr val fail", 0, 0, 0 );
        return 0;
      }
    }
    else
    {
      BT_ERR( "BT SD: Unable to add service attribute", 0, 0, 0 );

      /* Skip over the rest of the service attribute(s) */
      while ( cur_byte_parsed < matched_attr_byte_count )
      {
        if ( bt_sd_dsm_pullup( dsm_ptr_ptr, (void *) &uint8_val, 1 ) )
        {
          cur_byte_parsed++;
        }
        else
        {
          BT_ERR( "BT SD: parse AL: DSM PU 2 fail", 0, 0, 0 );
          return 0;
        }
      }
      break;
    }
  }

  tot_byte_parsed += cur_byte_parsed;

  return tot_byte_parsed;
}


/*===========================================================================

FUNCTION
  bt_sd_process_service_attribute_resp

DESCRIPTION
  Process a SDP Service Attribute Response

===========================================================================*/

boolean bt_sd_process_service_attribute_resp(
  word                 cid,
  dsm_item_type**      dsm_ptr_ptr,
  bt_sd_pdu_hdr_type*  pdu_hdr_ptr
)
{
  uint8                                         info_len;
  uint16                                        i;
  uint16                                        srv_rec_count;
  uint16                                        attr_list_byte_count;
  dsm_item_type**                               local_dsm_ptr_ptr;
  bt_sd_srv_rec_type*                           srv_rec_ptr;
  bt_sd_conn_entry_type*                        conn_entry_ptr;
  bt_ev_msg_type                                ev_srv_srch_resp;
  uint8  cont_state_info[BT_SD_MAX_CONTINUATION_STATE_INFO_LEN];

  BT_MSG_SIG( "BT SD: RX SA Resp TID %x Len %x",
              pdu_hdr_ptr->tid, pdu_hdr_ptr->length, 0 );

  conn_entry_ptr = bt_sd_find_conn_entry_by_cid( cid );

  if ( ( conn_entry_ptr == 0 ) || ( ! conn_entry_ptr->req_pending ) )
  {
    BT_MSG_HIGH( "BT SD: Unexpected response", 0, 0, 0 );
    return FALSE;
  }

  /* Clear Request Timeout Timer */
  rex_clr_timer( &conn_entry_ptr->tmr_loc_initd );

  /* Check for invalid PDU length */
  if ( pdu_hdr_ptr->length != dsm_length_packet( *dsm_ptr_ptr ) )
  {
    bt_sd_send_error_resp_event(
      conn_entry_ptr,
      BT_SD_EV_ERROR_INVALID_PDU_SIZE
    );
    return FALSE;
  }

  /* FIXME (ryuen)
   * Use the cid to find the corresponding memory location where the
   * result will be stored.  For now, since only one connection is allowed,
   * save result to known location
   */

  /* Parse AttributeListByteCount */
  if ( ! bt_sd_dsm_pullup( dsm_ptr_ptr, (void *) &attr_list_byte_count, 2 ) )
  {
    /* Send BT_EV_SD_ERROR_RESP */
    bt_sd_send_error_resp_event(
      conn_entry_ptr,
      BT_SD_EV_ERROR_INVALID_RESPONSE_SYNTAX
    );
    return FALSE;
  }
  attr_list_byte_count = SWAB16( attr_list_byte_count );

  /* Take a peek at the InfoLen field of ContinuationState */
  if ( dsm_extract( *dsm_ptr_ptr, attr_list_byte_count, &info_len, 1 ) != 1 )
  {
    /* Send BT_EV_SD_ERROR_RESP */
    bt_sd_send_error_resp_event(
      conn_entry_ptr,
      BT_SD_EV_ERROR_INVALID_RESPONSE_SYNTAX
    );
    return FALSE;
  }

  if ( info_len > 0 )
  {
    if ( info_len > BT_SD_MAX_CONTINUATION_STATE_INFO_LEN )
    {
      /* Send BT_EV_SD_ERROR_RESP */
      bt_sd_send_error_resp_event(
        conn_entry_ptr,
        BT_SD_EV_ERROR_INVALID_RESPONSE_SYNTAX
      );
      return FALSE;
    }

    /* Get the rest of the ContinuationState */
    if ( dsm_pullup_tail( dsm_ptr_ptr,
                          (void *) cont_state_info, info_len ) != info_len )
    {
      /* Send BT_EV_SD_ERROR_RESP */
      bt_sd_send_error_resp_event(
        conn_entry_ptr,
        BT_SD_EV_ERROR_INVALID_RESPONSE_SYNTAX
      );
      return FALSE;
    }
  }

  /* Don't forget the InfoLen field */
  if ( dsm_pullup_tail( dsm_ptr_ptr, 0, 1 ) != 1 )
  {
    /* Send BT_EV_SD_ERROR_RESP */
    bt_sd_send_error_resp_event(
      conn_entry_ptr,
      BT_SD_EV_ERROR_INVALID_RESPONSE_SYNTAX
    );
    return FALSE;
  }

  /* Save the DSM for further appending */
  dsm_append( &conn_entry_ptr->resp_dsm_ptr, dsm_ptr_ptr );

  /* compare old cont info with new one */
  if(memcmp(conn_entry_ptr->cont_state_info,cont_state_info,
                                        BT_SD_MAX_CONTINUATION_STATE_INFO_LEN) !=0)
  {
      memcpy(conn_entry_ptr->cont_state_info,cont_state_info,
                                        BT_SD_MAX_CONTINUATION_STATE_INFO_LEN);      
  }
  else
  {
      BT_MSG_SIG( "BT SD: Same Cont state",0,0,0 );
      conn_entry_ptr->same_cont_state_count++;
  }


  if ( ( info_len == 0 ) || (conn_entry_ptr->same_cont_state_count >
                             BT_SD_MAX_SAME_CONTINUATION_HOPS_ALLOWED))
  {
    local_dsm_ptr_ptr = &conn_entry_ptr->resp_dsm_ptr;

    bt_sd_initialize_database( BT_SD_DB_TYPE_SRV_SRCH_RESULT_DB );
    srv_rec_ptr =
      bt_sd_alloc_service_record( BT_SD_DB_TYPE_SRV_SRCH_RESULT_DB );

    if ( srv_rec_ptr )
    {
      if ( bt_sd_parse_attribute_list( local_dsm_ptr_ptr, srv_rec_ptr ) == 0 )
      {
        BT_ERR("BT SD: bad attrib list", 0, 0, 0 );
      }

      bt_sd_validate_service_record( srv_rec_ptr );
    }
    else
    {
      BT_ERR( "BT SD: Unable to allocate service record", 0, 0, 0 );
      dsm_free_packet( local_dsm_ptr_ptr );

        bt_sd_send_error_resp_event(
          conn_entry_ptr,
          BT_SD_EV_ERROR_INSUFFICIENT_RESOURCE
        );      
        return FALSE;      
    }

    srv_rec_count = 
      bt_sd_get_num_service_record( BT_SD_DB_TYPE_SRV_SRCH_RESULT_DB );

    BT_MSG_API( "BT SD EV TX: SD SS Resp Cnt1 %x Cnt2 %x AID %x",
                srv_rec_count,
                srv_rec_ptr->matched_attr_byte_count,
                conn_entry_ptr->bt_app_id );

    /* Send BT_EV_SD_SRV_SRCH_RESP */
    ev_srv_srch_resp.ev_hdr.ev_type = BT_EV_SD_SERVICE_SEARCH_RESP;
    ev_srv_srch_resp.ev_hdr.bt_app_id = conn_entry_ptr->bt_app_id;
    ev_srv_srch_resp.ev_msg.ev_sd_service_search_resp.bd_addr =
      conn_entry_ptr->bd_addr;
    for ( i = 0; i < BT_SD_MAX_NUM_OF_SRV_SRCH_PAT_UUID; i++ )
    {
      ev_srv_srch_resp.ev_msg.ev_sd_service_search_resp.
        srv_rec_handle_list[i] = 0;
    }
    ev_srv_srch_resp.ev_msg.ev_sd_service_search_resp.srv_rec_count =
      srv_rec_count;
    ev_srv_srch_resp.ev_msg.ev_sd_service_search_resp.attr_byte_count =
      srv_rec_ptr->matched_attr_byte_count;
    bt_ec_send_event( &ev_srv_srch_resp );

    /* Set Idle Timer */
    rex_set_timer( &conn_entry_ptr->tmr_loc_initd, BT_SD_IDLE_TIMEOUT_MS );

    /* Done */
    bt_sd_clear_pending_request( conn_entry_ptr );
  }
  else /* Need to send more requests */
  {
    conn_entry_ptr->num_resp_recv++;

    /* Send another request with continuation information */
    bt_sd_send_pending_request( conn_entry_ptr, info_len, cont_state_info );
  }

  return TRUE;
}


/*===========================================================================

FUNCTION
  bt_sd_process_service_search_attribute_req

DESCRIPTION
  Process a SDP Service Search Attribute Request

===========================================================================*/

boolean bt_sd_process_service_search_attribute_req(
  word                 cid,
  dsm_item_type**      dsm_ptr_ptr,
  bt_sd_pdu_hdr_type*  pdu_hdr_ptr
)
{
  uint8                     info_len;
  uint8                     de_hdr;
  uint16                    matched_attr_byte_count = 0;
  uint16                    byte_parsed, total_byte_parsed = 0;
  uint16                    uint16_val;
  uint32                    dsm_pkt_len;
  bt_sd_srv_srch_pat_type   srch_pat;
  bt_sd_srv_rec_type*       srv_rec_ptr;
  bt_sd_srv_attr_rec_type*  srv_attr_ptr;
  dsm_mempool_id_enum_type  dsm_pool_id;
  uint8   cont_state_info[BT_SD_MAX_CONTINUATION_STATE_INFO_LEN];

  BT_MSG_SIG( "BT SD: RX SSA Req TID %x Len %x",
              pdu_hdr_ptr->tid, pdu_hdr_ptr->length, 0 );

  /* Check for invalid PDU length */
  if ( pdu_hdr_ptr->length != dsm_length_packet( *dsm_ptr_ptr ) )
  {
    bt_sd_send_error_resp( cid,
                           pdu_hdr_ptr->tid,
                           BT_SD_EV_ERROR_INVALID_PDU_SIZE );
    return FALSE;
  }

  /*
   * Initialize the search pattern
   */
  srch_pat.max_srv_rec_count = 0;
  srch_pat.max_attr_byte_count = 0;
  srch_pat.num_uuid = 0;
  srch_pat.num_uuid128 = 0;
  srch_pat.num_attr_id = 0;
  srch_pat.syncml_flag = BT_SD_DB_NOT_SYNCML_V;
  srch_pat.uuid128[0] = bt_sd_null_uuid128;

  /*
   * Parse the ServiceSearchPattern field
   */
  if ( ! bt_sd_parse_service_search_pattern( dsm_ptr_ptr,
                                             &srch_pat, &byte_parsed ) )
  {
    bt_sd_send_error_resp( cid, pdu_hdr_ptr->tid,
                           BT_SD_EV_ERROR_INVALID_REQUEST_SYNTAX );
    return FALSE;
  }
  total_byte_parsed += byte_parsed;

  /*
   * Parse the MaximumAttributeByteCount field
   */
  if ( ! bt_sd_dsm_pullup( dsm_ptr_ptr,
                           (void *) &( srch_pat.max_attr_byte_count ), 2 ) )
  {
    bt_sd_send_error_resp( cid, pdu_hdr_ptr->tid,
                           BT_SD_EV_ERROR_INVALID_REQUEST_SYNTAX );
    return FALSE;
  }
  srch_pat.max_attr_byte_count = SWAB16( srch_pat.max_attr_byte_count );
  total_byte_parsed += 2;

  /*
   * Parse the AttributeIDList field
   */
  if ( ! bt_sd_parse_attribute_id_list( dsm_ptr_ptr, &srch_pat, &byte_parsed ) )
  {
    bt_sd_send_error_resp( cid, pdu_hdr_ptr->tid,
                           BT_SD_EV_ERROR_INVALID_REQUEST_SYNTAX );
    return FALSE;
  }
  total_byte_parsed += byte_parsed;

  /*
   * Parse the ContinuationState field
   */
  if ( ! bt_sd_dsm_pullup( dsm_ptr_ptr, (void *) &info_len, 1 ) )
  {
    bt_sd_send_error_resp( cid, pdu_hdr_ptr->tid,
                           BT_SD_EV_ERROR_INVALID_REQUEST_SYNTAX );
    return FALSE;
  }
  total_byte_parsed++;

  if ( info_len > 0 )
  {
    if ( ( info_len > BT_SD_MAX_CONTINUATION_STATE_INFO_LEN ) ||
         ( info_len != 4 ) )
    {
      BT_MSG_HIGH( "BT SD: Unexpected cont state info len %d", info_len, 0, 0 );
      bt_sd_send_error_resp( cid, pdu_hdr_ptr->tid,
                             BT_SD_EV_ERROR_INVALID_CONTINUATION_STATE );
      return FALSE;
    }

    if ( ! bt_sd_dsm_pullup( dsm_ptr_ptr, (void *) cont_state_info, info_len ) )
    {
      bt_sd_send_error_resp( cid, pdu_hdr_ptr->tid,
                             BT_SD_EV_ERROR_INVALID_REQUEST_SYNTAX );
      return FALSE;
    }
    total_byte_parsed += info_len;
  }

  if ( total_byte_parsed != pdu_hdr_ptr->length )
  {
    BT_MSG_HIGH( "BT SD: Parameter length mismatch %d %d",
                 total_byte_parsed, pdu_hdr_ptr->length, 0 );
    bt_sd_send_error_resp( cid, pdu_hdr_ptr->tid,
                           BT_SD_EV_ERROR_INVALID_PDU_SIZE );
    return FALSE;
  }

  /* Start a new search if this is not the continuation of a former request */
  if ( info_len == 0 )
  {
    matched_attr_byte_count = bt_sd_match_service_record( &srch_pat );

    /* Free data from the previous request, if there is any */
    if ( dsm_length_packet( attr_list_dsm_ptr ) > 0 )
    {
      dsm_free_packet( &attr_list_dsm_ptr );
    }
    attr_list_dsm_ptr = NULL;

    if ( matched_attr_byte_count > 0 )
    {
      /* Push all matched attributes onto a DSM */
      if ( ( attr_list_dsm_ptr =
               bt_get_free_dsm_ptr( BT_TL_L2CAP,
                                    matched_attr_byte_count ) ) == NULL )
      {
        BT_ERR( "BT SD: Unable to get DSM buffer", 0, 0, 0 );
        bt_sd_send_error_resp( cid, pdu_hdr_ptr->tid,
                               BT_SD_EV_ERROR_INSUFFICIENT_RESOURCE );
        return FALSE;
      }

      dsm_pool_id = DSM_DS_POOL_SIZE( matched_attr_byte_count );

      bt_sd_srv_rec_iter.init( BT_SD_DB_TYPE_LOCAL_DB, BT_SD_ITER_MATCH_ONLY );
      for ( srv_rec_ptr =
              ( bt_sd_srv_rec_type* ) bt_sd_srv_rec_iter.first();
            srv_rec_ptr != 0;
            srv_rec_ptr =
              ( bt_sd_srv_rec_type* ) bt_sd_srv_rec_iter.next() )
      {
        if ( srv_rec_ptr->matched_attr_byte_count <= 0xFF )
        {
          de_hdr = ( BT_SD_DATA_ELEMENT_SEQUENCE << 3 ) | 5;
          bt_sd_dsm_push8( &attr_list_dsm_ptr, &de_hdr, dsm_pool_id );
          bt_sd_dsm_push8( &attr_list_dsm_ptr,
                           ( uint8* ) &srv_rec_ptr->matched_attr_byte_count,
                           dsm_pool_id );
        }
        else
        {
          de_hdr = ( BT_SD_DATA_ELEMENT_SEQUENCE << 3 ) | 6;
          bt_sd_dsm_push8( &attr_list_dsm_ptr, &de_hdr, dsm_pool_id );

          uint16_val = SWAB16( srv_rec_ptr->matched_attr_byte_count );
          bt_sd_dsm_push16( &attr_list_dsm_ptr, &uint16_val, dsm_pool_id );
        }

        bt_sd_srv_attr_iter.init( srv_rec_ptr, BT_SD_ITER_MATCH_ONLY );
        for ( srv_attr_ptr =
                ( bt_sd_srv_attr_rec_type* ) bt_sd_srv_attr_iter.first();
              srv_attr_ptr != 0;
              srv_attr_ptr =
                ( bt_sd_srv_attr_rec_type* ) bt_sd_srv_attr_iter.next() )
        {
          bt_sd_add_attribute_to_dsm_buffer( srv_attr_ptr,
                                             attr_list_dsm_ptr,
                                             dsm_pool_id );
        }
      }

      dsm_pkt_len = dsm_length_packet( attr_list_dsm_ptr );
      if ( dsm_pkt_len != matched_attr_byte_count )
      {
        BT_ERR( "BT SD: DSM length mismatch %d %d",
                dsm_pkt_len,
                matched_attr_byte_count, 0 );
        dsm_free_packet( &attr_list_dsm_ptr );
        bt_sd_send_error_resp( cid, pdu_hdr_ptr->tid,
                               BT_SD_EV_ERROR_INSUFFICIENT_RESOURCE );
        return FALSE;
      }
    }
  }

  /* Send response */
  bt_sd_send_sa_or_ssa_resp( BT_SD_PDU_SERVICE_SEARCH_ATTRIBUTE_RESP,
                             cid, pdu_hdr_ptr->tid,
                             srch_pat.max_attr_byte_count,
                             cont_state_info, info_len );

  return TRUE;
}


/*===========================================================================

FUNCTION
  bt_sd_process_service_search_attribute_resp

DESCRIPTION
  Process a SDP Service Search Attribute Response

===========================================================================*/

boolean bt_sd_process_service_search_attribute_resp(
  word                 cid,
  dsm_item_type**      dsm_ptr_ptr,
  bt_sd_pdu_hdr_type*  pdu_hdr_ptr
)
{
  uint16                                        attr_lists_byte_count;
  uint8                                         info_len;
  uint16                                        tot_attr_byte_count;
  uint16                                        tot_byte_parsed = 0;
  uint16                                        i, byte_parsed;
  uint16                                        srv_rec_count;
  dsm_item_type**                               local_dsm_ptr_ptr;
  bt_sd_srv_rec_type*                           srv_rec_ptr;
  bt_sd_conn_entry_type*                        conn_entry_ptr;
  bt_ev_msg_type                                ev_srv_srch_resp;
  dsm_item_type*                                dsm_ptr;
  bt_sd_data_element_header_parse_result_type   parse_result;
  uint8   cont_state_info[BT_SD_MAX_CONTINUATION_STATE_INFO_LEN];

  BT_MSG_SIG( "BT SD: RX SSA Resp TID %x Len %x",
              pdu_hdr_ptr->tid, pdu_hdr_ptr->length, 0 );

  conn_entry_ptr = bt_sd_find_conn_entry_by_cid( cid );

  if ( ( conn_entry_ptr == 0 ) || ( ! conn_entry_ptr->req_pending ) )
  {
    BT_MSG_HIGH( "BT SD: Unexpected response", 0, 0, 0 );
    return FALSE;
  }

  /* Clear Request Timeout Timer */
  rex_clr_timer( &conn_entry_ptr->tmr_loc_initd );

  /* Check for invalid PDU length */
  if ( pdu_hdr_ptr->length != dsm_length_packet( *dsm_ptr_ptr ) )
  {
    BT_ERR("BT SD: invalid PDU length %d", pdu_hdr_ptr->length, 0, 0 );
    bt_sd_send_error_resp_event(
      conn_entry_ptr,
      BT_SD_EV_ERROR_INVALID_PDU_SIZE
    );
    return FALSE;
  }

  /* FIXME (ryuen)
   * Use the cid to find the corresponding memory location where the
   * result will be stored.  For now, since only one connection is allowed,
   * save result to known location
   */

  /* Parse AttributeListsByteCount */
  if ( ! bt_sd_dsm_pullup( dsm_ptr_ptr, (void *) &attr_lists_byte_count, 2 ) )
  {
    BT_ERR("BT SD: bad attr_lists_byte_count", 0, 0, 0 );

    /* Send BT_EV_SD_ERROR_RESP */
    bt_sd_send_error_resp_event(
      conn_entry_ptr,
      BT_SD_EV_ERROR_INVALID_RESPONSE_SYNTAX
    );
    return FALSE;
  }
  attr_lists_byte_count = SWAB16( attr_lists_byte_count );

  /* Take a peek at the InfoLen field of ContinuationState */
  if ( dsm_extract( *dsm_ptr_ptr, attr_lists_byte_count, &info_len, 1 )  != 1 )
  {
    BT_ERR("BT SD: bad info_len", 0, 0, 0 );

    /* Send BT_EV_SD_ERROR_RESP */
    bt_sd_send_error_resp_event(
      conn_entry_ptr,
      BT_SD_EV_ERROR_INVALID_RESPONSE_SYNTAX
    );
    return FALSE;
  }

  if ( info_len > 0 )
  {
    if ( info_len > BT_SD_MAX_CONTINUATION_STATE_INFO_LEN )
    {
      BT_ERR("BT SD: bad info_len", 0, 0, 0 );

      /* Send BT_EV_SD_ERROR_RESP */
      bt_sd_send_error_resp_event(
        conn_entry_ptr,
        BT_SD_EV_ERROR_INVALID_RESPONSE_SYNTAX
      );
      return FALSE;
    }

    /* Get the rest of the ContinuationState */
    if ( dsm_pullup_tail( dsm_ptr_ptr,
                          (void *) cont_state_info, info_len ) != info_len )
    {
      BT_ERR("BT SD: bad cont_state_info", 0, 0, 0 );
      bt_sd_send_error_resp_event(
        conn_entry_ptr,
        BT_SD_EV_ERROR_INVALID_RESPONSE_SYNTAX
      );
      return FALSE;
    }
  }

  /* Don't forget the InfoLen field */
  if ( dsm_pullup_tail( dsm_ptr_ptr, 0, 1 ) != 1 )
  {
    BT_ERR("BT SD: bad info_len", 0, 0, 0 );

    /* Send BT_EV_SD_ERROR_RESP */
    bt_sd_send_error_resp_event(
      conn_entry_ptr,
      BT_SD_EV_ERROR_INVALID_RESPONSE_SYNTAX
    );
    return FALSE;
  }

  /* Save the DSM for further appending */
  dsm_append( &conn_entry_ptr->resp_dsm_ptr, dsm_ptr_ptr );

  /* compare old cont info with new one */
  if(memcmp(conn_entry_ptr->cont_state_info,cont_state_info,
                                        BT_SD_MAX_CONTINUATION_STATE_INFO_LEN) !=0)
  {
      memcpy(conn_entry_ptr->cont_state_info,cont_state_info,
                                        BT_SD_MAX_CONTINUATION_STATE_INFO_LEN);      
  }
  else
  {
      BT_MSG_SIG( "BT SD: Same Cont state",0,0,0 );
      conn_entry_ptr->same_cont_state_count++;
  }

  if ( ( info_len == 0 ) || (conn_entry_ptr->same_cont_state_count >
                             BT_SD_MAX_SAME_CONTINUATION_HOPS_ALLOWED))
  {
    local_dsm_ptr_ptr = &conn_entry_ptr->resp_dsm_ptr;

    /* Parse data element sequence header for AttributeLists */
    if ( ( ! bt_sd_parse_data_element_header( local_dsm_ptr_ptr,
                                              &parse_result ) ) ||
         ( parse_result.type_desc != BT_SD_DATA_ELEMENT_SEQUENCE ) )
    {
      BT_ERR("BT SD: bad type_desc", 0, 0, 0 );

      /* Send BT_EV_SD_ERROR_RESP */
      bt_sd_send_error_resp_event(
        conn_entry_ptr,
        BT_SD_EV_ERROR_INVALID_RESPONSE_SYNTAX
      );
      return FALSE;
    }

    tot_attr_byte_count = parse_result.data_size;

    bt_sd_initialize_database( BT_SD_DB_TYPE_SRV_SRCH_RESULT_DB );

    while ( tot_byte_parsed < tot_attr_byte_count )
    {
      /* Ignore empty AttributeLists regardless of the DES length */
      if ( dsm_length_packet( *local_dsm_ptr_ptr ) == 0 )
      {
        break;
      }

      srv_rec_ptr =
        bt_sd_alloc_service_record( BT_SD_DB_TYPE_SRV_SRCH_RESULT_DB );

      if ( srv_rec_ptr )
      {
        if ( ( byte_parsed =
                 bt_sd_parse_attribute_list(
                   local_dsm_ptr_ptr, srv_rec_ptr ) ) == 0 )
        {
          BT_ERR("BT SD: bad attrib list", 0, 0, 0 );
        }

        tot_byte_parsed += byte_parsed;

        bt_sd_validate_service_record( srv_rec_ptr );

        /*
         * As soon as we encounter a parsing error, stop and return whatever
         * we've already parsed
         */
        if ( byte_parsed == 0 )
        {
          break;
        }
      }
      else
      {
        BT_ERR( "BT SD: Unable to allocate service record", 0, 0, 0 );
        dsm_free_packet( local_dsm_ptr_ptr );
        break;
      }
    }

    srv_rec_count =
      bt_sd_get_num_service_record( BT_SD_DB_TYPE_SRV_SRCH_RESULT_DB );

    BT_MSG_API( "BT SD EV TX: SD SS Resp Cnt1 %x Cnt2 %x AID %x",
                srv_rec_count, tot_attr_byte_count, conn_entry_ptr->bt_app_id );

    if( conn_entry_ptr->generate_xml == TRUE )
    {
      /* Send BT_EV_SD_SERVICE_SEARCH_XML_RESP */
      dsm_ptr = bt_get_free_dsm_ptr( BT_TL_HCI_BB,  tot_attr_byte_count );
      bt_sd_create_xml_service_db( BT_SD_DB_TYPE_SRV_SRCH_RESULT_DB, &dsm_ptr );

      ev_srv_srch_resp.ev_hdr.ev_type = BT_EV_SD_SERVICE_SEARCH_XML_RESP;
      ev_srv_srch_resp.ev_hdr.bt_app_id = conn_entry_ptr->bt_app_id;
      ev_srv_srch_resp.ev_msg.ev_sd_service_search_xml_resp.bd_addr =
        conn_entry_ptr->bd_addr;

      ev_srv_srch_resp.ev_msg.ev_sd_service_search_xml_resp.dsm_ptr = 
        dsm_ptr;
    }
    else
    {
      /* Send BT_EV_SD_SRV_SRCH_RESP */
      ev_srv_srch_resp.ev_hdr.ev_type = BT_EV_SD_SERVICE_SEARCH_RESP;
      ev_srv_srch_resp.ev_hdr.bt_app_id = conn_entry_ptr->bt_app_id;
      ev_srv_srch_resp.ev_msg.ev_sd_service_search_resp.bd_addr =
        conn_entry_ptr->bd_addr;
      for ( i = 0; i < BT_SD_MAX_NUM_OF_SRV_SRCH_PAT_UUID; i++ )
      {
        ev_srv_srch_resp.ev_msg.ev_sd_service_search_resp.
          srv_rec_handle_list[i] = 0;
      }
      ev_srv_srch_resp.ev_msg.ev_sd_service_search_resp.srv_rec_count =
        srv_rec_count;
      ev_srv_srch_resp.ev_msg.ev_sd_service_search_resp.attr_byte_count =
        tot_attr_byte_count;
    }
    bt_ec_send_event( &ev_srv_srch_resp );
  
    /* Set Idle Timer */
    rex_set_timer( &conn_entry_ptr->tmr_loc_initd, BT_SD_IDLE_TIMEOUT_MS );

    /* Done */
    bt_sd_clear_pending_request( conn_entry_ptr );
  }
  else /* Need to send more requests */
  {
    conn_entry_ptr->num_resp_recv++;

    /* Send another request with continuation information */
    bt_sd_send_pending_request( conn_entry_ptr, info_len, cont_state_info );
  }

  return TRUE;
}


/*===========================================================================

FUNCTION
  bt_sd_ev_l2_connected

DESCRIPTION
  Handles an L2CAP connected event.

===========================================================================*/

boolean bt_sd_ev_l2_connected(
  bt_ev_l2_connected_type*  l2_conn_ptr
)
{
  bt_sd_conn_entry_type* conn_entry_ptr;

  BT_MSG_API( "BT SD EV RX: L2 Connected CID %x",
              l2_conn_ptr->cid, 0, 0 );

  conn_entry_ptr = bt_sd_find_conn_entry_by_bd_addr( &l2_conn_ptr->bd_addr );

  /* New connection */
  if ( conn_entry_ptr == 0 )
  {
    if ( ( conn_entry_ptr =
             bt_sd_add_conn_entry( &( l2_conn_ptr->bd_addr ) ) ) == 0 )
    {
      /* Can't add new connection */
      return FALSE;
    }
  }

  conn_entry_ptr->conn_state = BT_SD_CONN_STATE_CONNECTED;
  conn_entry_ptr->out_mtu    = l2_conn_ptr->mtu;

  if ( conn_entry_ptr->cid_loc_initd == l2_conn_ptr->cid )
  {
    /*  This L2CAP connection was initiated by the local device.  */
    
    BT_MSG_HIGH( "BT SD: Local Init'd CID %x",
                 conn_entry_ptr->cid_loc_initd, 0, 0 );

    bt_sd_send_pending_request( conn_entry_ptr, 0, 0 );
  }
  else
  {
    /*  This L2CAP connection was initiated by the remote device.  */

    conn_entry_ptr->cid_rem_initd = l2_conn_ptr->cid;
    
    BT_MSG_HIGH( "BT SD: Remote Init'd CID %x",
                 conn_entry_ptr->cid_rem_initd, 0, 0 );
    
    /*  Start Idle Timer for remote initiated L2CAP connection.  */
    rex_set_timer( &conn_entry_ptr->tmr_rem_initd,
                   BT_SD_IDLE_TIMEOUT_MS );
  }

  bt_sd_check_connectable_setting();
  
  return TRUE;
}


/*===========================================================================

FUNCTION
  bt_sd_ev_l2_connection_failed

DESCRIPTION
  Handles an L2CAP connection failed event.

===========================================================================*/

boolean bt_sd_ev_l2_connection_failed(
  bt_ev_l2_connection_failed_type*  l2_conn_failed_ptr
)
{
  bt_sd_conn_entry_type* conn_entry_ptr;

  BT_MSG_API( "BT SD EV RX: L2 Connect Failed: %x",
               l2_conn_failed_ptr->reason, 0, 0 );

  conn_entry_ptr =
    bt_sd_find_conn_entry_by_bd_addr( &l2_conn_failed_ptr->bd_addr );

  if ( ( conn_entry_ptr != NULL ) && 
       ( ( conn_entry_ptr->cid_loc_initd == l2_conn_failed_ptr->cid ) || 
         ( conn_entry_ptr->cid_rem_initd == l2_conn_failed_ptr->cid ) ) )
  {
    /* Send BT_EV_SD_ERROR_RESP */
    bt_sd_send_error_resp_event(
      conn_entry_ptr,
      BT_SD_EV_ERROR_CONNECTION_FAILED
    );

    if ( conn_entry_ptr->cid_loc_initd == l2_conn_failed_ptr->cid )
    {
      conn_entry_ptr->cid_loc_initd = BT_L2_NULL_CID;
    }

    if ( ( conn_entry_ptr->cid_loc_initd == BT_L2_NULL_CID ) &&
         ( conn_entry_ptr->cid_rem_initd == BT_L2_NULL_CID ) )
    {
      conn_entry_ptr->conn_state = BT_SD_CONN_STATE_DISCONNECTED;

      /* Free the connection entry */
      conn_entry_ptr->used = FALSE;
    }
  }
  else
  {
    BT_ERR ("BT SD: Ignoring unexp L2 Connect Failed evt on cid %x", 
            l2_conn_failed_ptr->cid, 0, 0 );
  }

  bt_sd_check_connectable_setting();
  
  return TRUE;
}


/*===========================================================================

FUNCTION
  bt_sd_ev_l2_disconnected

DESCRIPTION
  Handles an L2CAP disconnected event.

===========================================================================*/

boolean bt_sd_ev_l2_disconnected(
  bt_ev_l2_disconnected_type*  l2_disc_ptr
)
{
  bt_sd_conn_entry_type*  conn_entry_ptr;
  bt_ev_msg_type          ev_idle_timer_resp;
  uint16                  cid = l2_disc_ptr->cid;

  BT_MSG_API( "BT SD EV RX: L2 Disconnected CID %x", cid, 0, 0 );

  conn_entry_ptr = bt_sd_find_conn_entry_by_cid( cid );

  if ( conn_entry_ptr == NULL )
  {
    return FALSE;
  }
  else
  {
    if ( conn_entry_ptr->cid_loc_initd == cid )
    {
      conn_entry_ptr->cid_loc_initd = BT_L2_NULL_CID;

      rex_clr_timer( &conn_entry_ptr->tmr_loc_initd );
    }
    else if ( conn_entry_ptr->cid_rem_initd == cid )
    {
      conn_entry_ptr->cid_rem_initd = BT_L2_NULL_CID;

      rex_clr_timer( &conn_entry_ptr->tmr_rem_initd );
    }
    else
    {
      BT_ERR( "BT SD: Bad L2 DISC CID %x L %x R %x",
              cid, conn_entry_ptr->cid_loc_initd,
              conn_entry_ptr->cid_rem_initd );
      return FALSE;
    }
  }
  
  if ( ( conn_entry_ptr->req_pending ) && 
       ( conn_entry_ptr->cid_loc_initd == BT_L2_NULL_CID ) )
  {   
    if ( ( conn_entry_ptr->num_retries < BT_SD_MAX_SEARCH_RETRIES) &&
         ( conn_entry_ptr->dup_req_dsm_ptr != NULL ) )
    {
      BT_MSG_HIGH( "BT SD: Retrying pending SDP command ", 0, 0, 0 );
      conn_entry_ptr->num_retries++;
        
      /* At this point we don't know if the previous response-pending SDP Cmd
         has been sent out yet from our MSM host.If the SDP Cmd has already
         been sent out,it will be rejected by the remote peer and that is fine.
          If it has not yet been sent out, we don't know in which layer the SDP
         pkt is in the Host BT stack. But we don't need to clean it off from the
         the stack or free the SDP pkt in 'pending_req_dsm_ptr'. Once the SDP
         command goes out, it will be rejected by the peer gracefully */      

      /* Try resending the SDP command using the 'dup req dsm pointer' */
      dsm_dup_packet( &conn_entry_ptr->pending_req_dsm_ptr,
                      conn_entry_ptr->dup_req_dsm_ptr,
                      0,
                      dsm_length_packet( conn_entry_ptr->dup_req_dsm_ptr ) );

      conn_entry_ptr->conn_state = BT_SD_CONN_STATE_CONNECTING;

      /* Re-Establish L2CAP connection first */
      bt_cmd_l2_connect_immediate_cid(
        bt_sd_app_id,
        BT_L2_PSM_SDP,
        &( conn_entry_ptr->bd_addr ),
        &( conn_entry_ptr->cid_loc_initd ) );

      BT_MSG_API( "BT SD CMD TX: L2 Conn Cmd Tx - AID %x CID %x",
                  bt_sd_app_id, conn_entry_ptr->cid_loc_initd, 0 );
    }
    else /* Send Error Response and clear pending request */
    {
      bt_sd_send_error_resp_event(
        conn_entry_ptr,
        BT_SD_EV_ERROR_CONNECTION_TERMINATED
      );
    }
  }

  ev_idle_timer_resp.ev_hdr.ev_type   = BT_EV_SD_CANCEL_IDLE_TIMER_RESP;
  ev_idle_timer_resp.ev_hdr.bt_app_id = conn_entry_ptr->bt_app_id;
  
  bt_ec_send_event( &ev_idle_timer_resp );

  if ( (conn_entry_ptr->cid_loc_initd == BT_L2_NULL_CID) &&
       (conn_entry_ptr->cid_rem_initd == BT_L2_NULL_CID) )
  {
    conn_entry_ptr->conn_state = BT_SD_CONN_STATE_DISCONNECTED;

    /*  Free the entry.  */
    conn_entry_ptr->used = FALSE;
  }
  else
  {
    /* There is still an active CID - move the connection state from
       DISCONNECTING back to CONNECTED */
    conn_entry_ptr->conn_state = BT_SD_CONN_STATE_CONNECTED;
  }

  bt_sd_check_connectable_setting();
  
  return TRUE;
}


/*===========================================================================

FUNCTION
  bt_sd_ev_l2_rx_data

DESCRIPTION
  Handles an L2CAP received data event.  This means some data was
  received for SDP.

===========================================================================*/

boolean bt_sd_ev_l2_rx_data
(
  bt_ev_l2_rx_data_type*  l2_rxd_ptr
)
{
  boolean                 success = FALSE;
  bt_sd_pdu_hdr_type      pdu_hdr;
  bt_sd_pdu_hdr_type*     pdu_hdr_ptr = &pdu_hdr;
  word                    cid = l2_rxd_ptr->cid;
  dsm_item_type*          dsm_ptr = l2_rxd_ptr->dsm_ptr;
  bt_sd_conn_entry_type*  conn_entry_ptr;

  BT_MSG_API_PYLD( "BT SD EV RX: L2 Rx Data", 0, 0, 0 );

#ifdef BT_SWDEV_QXDM_LOG_MSG
  bt_sd_sig_rx_logging_function( dsm_ptr, cid );
#endif

  if ( ! bt_sd_dsm_pullup( &dsm_ptr, ( void * ) pdu_hdr_ptr,
                           sizeof( bt_sd_pdu_hdr_type ) ) )
  {
    return FALSE;
  }
  pdu_hdr_ptr->tid = SWAB16( pdu_hdr_ptr->tid );
  pdu_hdr_ptr->length = SWAB16( pdu_hdr_ptr->length );

  switch( pdu_hdr_ptr->pid )
  {
    case BT_SD_PDU_ERROR_RESP:
      success = bt_sd_process_error_resp( cid, &dsm_ptr, pdu_hdr_ptr );
      break;
    case BT_SD_PDU_SERVICE_SEARCH_REQ:
      success = bt_sd_process_service_search_req(
                                          cid, &dsm_ptr, pdu_hdr_ptr );
      break;
    case BT_SD_PDU_SERVICE_SEARCH_RESP:
      success = bt_sd_process_service_search_resp(
                                          cid, &dsm_ptr, pdu_hdr_ptr );
      break;
    case BT_SD_PDU_SERVICE_ATTRIBUTE_REQ:
      success = bt_sd_process_service_attribute_req(
                                          cid, &dsm_ptr, pdu_hdr_ptr );
      break;
    case BT_SD_PDU_SERVICE_ATTRIBUTE_RESP:
      success = bt_sd_process_service_attribute_resp(
                                          cid, &dsm_ptr, pdu_hdr_ptr );
      break;
    case BT_SD_PDU_SERVICE_SEARCH_ATTRIBUTE_REQ:
      success = bt_sd_process_service_search_attribute_req(
                                          cid, &dsm_ptr, pdu_hdr_ptr );
      break;
    case BT_SD_PDU_SERVICE_SEARCH_ATTRIBUTE_RESP:
      success = bt_sd_process_service_search_attribute_resp(
                                          cid, &dsm_ptr, pdu_hdr_ptr );
      break;
  }

  /* Free any non-processed data */
  if ( dsm_ptr != NULL )
  {
    dsm_free_packet( &dsm_ptr );
  }
  
  if ( success != FALSE )
  {
    if ( (conn_entry_ptr = bt_sd_find_conn_entry_by_cid( cid )) != 0 )
    {
      if ( conn_entry_ptr->cid_rem_initd == cid )
      {
        /*  Reset Idle Timer for remote initiated L2CAP connection.  */
        rex_set_timer( &conn_entry_ptr->tmr_rem_initd,
                       BT_SD_IDLE_TIMEOUT_MS );
      }
    }
    else
    {
      BT_ERR( "BT SD: Bad L2 RxD CID %x", cid, 0, 0 );
    }
  }

  return( success );
}


/*===========================================================================

FUNCTION
  bt_sd_ev_rm_inquiry_result

DESCRIPTION
  Handles a Radio Manager Inquiry Result Event

===========================================================================*/

boolean bt_sd_ev_rm_inquiry_result
(
  bt_ev_rm_inquiry_result_type*  rm_inq_res_ptr
)
{
  uint16          i;
  uint16          service_class      = BT_SERVICE_CLASS_UNKNOWN;
  uint16          major_device_class = BT_SD_MAJOR_DEVICE_CLASS_UNCLASSIFIED;
  uint16          minor_device_class = BT_SD_MINOR_DEVICE_CLASS_UNCLASSIFIED;
  bt_ev_msg_type  ev_ddr;

  BT_MSG_API( "BT SD EV RX: RM Inq Res COD %02x %04x",
    rm_inq_res_ptr->class_of_device.cod_bytes[ 2 ],
    (uint16)((rm_inq_res_ptr->class_of_device.cod_bytes[ 1 ] << 8) |
                rm_inq_res_ptr->class_of_device.cod_bytes[ 0 ]), 0 );
  BT_BDA( MSG_API, "BT SD EV RX: RM Inq Res", &rm_inq_res_ptr->bd_addr );

  /* Make sure we're doing device discovery */
  if ( ! bt_sd_dev_discv_db.dev_discv_in_progress )
  {
    return TRUE;
  }

  /* Make sure we haven't exceeded the maximum number of discovered devices */
  if ( bt_sd_dev_discv_db.num_discvd_dev == bt_sd_dev_discv_db.max_discvd_dev )
  {
    return TRUE;
  }

  /* Check for duplicates */
  for ( i = 0; i < bt_sd_dev_discv_db.num_discvd_dev; i++ )
  {
    if ( BT_BD_ADDRS_EQUAL(
           &( bt_sd_dev_discv_db.discvd_dev_info[i].bd_addr ),
           &( rm_inq_res_ptr->bd_addr ) ) )
    {
      /* Found duplicate */
      return TRUE;
    }
  }

  /* Extract service class */
  if ( rm_inq_res_ptr->class_of_device.cod_bytes[2] & 0x80 )
  {
    service_class |= BT_SERVICE_CLASS_INFORMATION;
  }
  if ( rm_inq_res_ptr->class_of_device.cod_bytes[2] & 0x40 )
  {
    service_class |= BT_SERVICE_CLASS_TELEPHONY;
  }
  if ( rm_inq_res_ptr->class_of_device.cod_bytes[2] & 0x20 )
  {
    service_class |= BT_SERVICE_CLASS_AUDIO;
  }
  if ( rm_inq_res_ptr->class_of_device.cod_bytes[2] & 0x10 )
  {
    service_class |= BT_SERVICE_CLASS_OBJECT_TRANSFER;
  }
  if ( rm_inq_res_ptr->class_of_device.cod_bytes[2] & 0x08 )
  {
    service_class |= BT_SERVICE_CLASS_CAPTURING;
  }
  if ( rm_inq_res_ptr->class_of_device.cod_bytes[2] & 0x04 )
  {
    service_class |= BT_SERVICE_CLASS_RENDERING;
  }
  if ( rm_inq_res_ptr->class_of_device.cod_bytes[2] & 0x02 )
  {
    service_class |= BT_SERVICE_CLASS_NETWORKING;
  }
  if ( rm_inq_res_ptr->class_of_device.cod_bytes[2] & 0x01 )
  {
    service_class |= BT_SERVICE_CLASS_POSITIONING;
  }
  if ( BT_SD_COD_2_LIM_DISCV( rm_inq_res_ptr->class_of_device ) )
  {
    service_class |= BT_SERVICE_CLASS_LIMITED_DISCOVERABLE_MODE;
  }

  /* Filter by LSB of BD address */
  if (bt_sd_dev_discv_db.service_class_bitmap == BT_SERVICE_CLASS_UNKNOWN)
  {
    if (rm_inq_res_ptr->bd_addr.bd_addr_bytes[0] != 
        bt_sd_dev_discv_db.bd_addr_2_filter.bd_addr_bytes[0] )
    {
      /* doesn't match */
      return TRUE;
    }
  }
  /* Filter by device type; we accept responses with unknown service or
     matching services only */
  else if ( (service_class != BT_SERVICE_CLASS_UNKNOWN ) &&
            ((service_class & bt_sd_dev_discv_db.service_class_bitmap) == 0) )
  {
    return TRUE;
  }

  /* Extract major and minor device class */
  major_device_class =
    (rm_inq_res_ptr->class_of_device.cod_bytes[1] & 0x1F) << 8;

  switch( major_device_class )
  {
    case BT_SD_MAJOR_DEVICE_CLASS_COMPUTER:
    case BT_SD_MAJOR_DEVICE_CLASS_PHONE:
    case BT_SD_MAJOR_DEVICE_CLASS_AUDIO:
      minor_device_class =
        ((rm_inq_res_ptr->class_of_device.cod_bytes[0] >> 2) & 0x3F) |
          major_device_class;
      break;
    case BT_SD_MAJOR_DEVICE_CLASS_LAN_ACCESS_POINT:
      minor_device_class =
        ((rm_inq_res_ptr->class_of_device.cod_bytes[0] >> 2) & 0x07) |
          major_device_class;
      break;
  }

  /* Update the Device Discovery database */
  i = bt_sd_dev_discv_db.num_discvd_dev++;
  bt_sd_dev_discv_db.discvd_dev_info[i].bd_addr = rm_inq_res_ptr->bd_addr;
  bt_sd_dev_discv_db.discvd_dev_info[i].service_class = service_class;
  bt_sd_dev_discv_db.discvd_dev_info[i].major_device_class =
    major_device_class;
  bt_sd_dev_discv_db.discvd_dev_info[i].minor_device_class =
    minor_device_class;

  /* Send BT_EV_SD_DEVICE_DISCOVERY_RESP */
  ev_ddr.ev_hdr.ev_type = BT_EV_SD_DEVICE_DISCOVERY_RESP;
  ev_ddr.ev_hdr.bt_app_id = bt_sd_dev_discv_db.app_id;
  ev_ddr.ev_msg.ev_sd_dev_discv_resp.bd_addr = rm_inq_res_ptr->bd_addr;
  ev_ddr.ev_msg.ev_sd_dev_discv_resp.service_class = service_class;
  ev_ddr.ev_msg.ev_sd_dev_discv_resp.major_device_class = major_device_class;
  ev_ddr.ev_msg.ev_sd_dev_discv_resp.minor_device_class = minor_device_class;

  BT_MSG_API( "BT SD EV TX: SD DD Resp SC %x Mj/Mn %04x AID %x",
    ev_ddr.ev_msg.ev_sd_dev_discv_resp.service_class,
    (uint16)((ev_ddr.ev_msg.ev_sd_dev_discv_resp.major_device_class << 8) |
                ev_ddr.ev_msg.ev_sd_dev_discv_resp.minor_device_class),
    ev_ddr.ev_hdr.bt_app_id );
  BT_BDA( MSG_API, "BT SD EV TX: SD DD Resp",
          &ev_ddr.ev_msg.ev_sd_dev_discv_resp.bd_addr );

  bt_ec_send_event( &ev_ddr );

  return TRUE;
}


/*===========================================================================

FUNCTION
  bt_sd_ev_rm_inquiry_complete

DESCRIPTION
  Handles a Radio Manager Inquiry Complete Event

===========================================================================*/

boolean bt_sd_ev_rm_inquiry_complete
(
  bt_event_reason_type reason
)
{
  bt_ev_msg_type  ev_dev_discv_cmplt;

  BT_MSG_API( "BT SD EV RX: RM Inq Cmplt", 0, 0, 0 );

  bt_sd_dev_discv_db.dev_discv_in_progress = FALSE;

  BT_MSG_API( "BT SD EV TX: SD Dev Discv Cmplt Cnt %x AID %x",
              bt_sd_dev_discv_db.num_discvd_dev,
              bt_sd_dev_discv_db.app_id,
              0 );

  /* Send BT_EV_SD_DEVICE_DISCOVERY_COMPLETE */
  ev_dev_discv_cmplt.ev_hdr.ev_type = BT_EV_SD_DEVICE_DISCOVERY_COMPLETE;
  ev_dev_discv_cmplt.ev_hdr.bt_app_id = bt_sd_dev_discv_db.app_id;
  ev_dev_discv_cmplt.ev_msg.ev_sd_dev_discv_cmplt.reason = reason;
  ev_dev_discv_cmplt.ev_msg.ev_sd_dev_discv_cmplt.num_discvd_dev =
    bt_sd_dev_discv_db.num_discvd_dev;

  bt_ec_send_event( &ev_dev_discv_cmplt );

  return TRUE;
}


/*===========================================================================

FUNCTION
  bt_sd_ev_rm_remote_name

DESCRIPTION
  Handles a Radio Manager Remote Name Event

===========================================================================*/

boolean bt_sd_ev_rm_remote_name
(
  bt_ev_rm_remote_name_type*  rm_rnr_ptr
)
{
  bt_ev_msg_type  ev_dev_name_resp;

  BT_MSG_API( "BT SD EV RX: RM Remote Name Resp %c%c%c", 
              rm_rnr_ptr->name_str[0], rm_rnr_ptr->name_str[1],
              rm_rnr_ptr->name_str[2] );

  bt_sd_dev_discv_db.remote_name_req_in_progress = FALSE;

  BT_MSG_API( "BT SD EV TX: SD Dev Name Resp AID %x",
              bt_sd_dev_discv_db.app_id, 0, 0 );

  /* Send BT_EV_SD_DEVICE_NAME_RESP */
  ev_dev_name_resp.ev_hdr.ev_type = BT_EV_SD_DEVICE_NAME_RESP;
  ev_dev_name_resp.ev_hdr.bt_app_id = bt_sd_dev_discv_db.app_id;
  ev_dev_name_resp.ev_msg.ev_sd_dname.reason = BT_EVR_GN_SUCCESS;
  #ifndef CUST_EDITION //modify by peng 2010-10-28, if cannot get remote name, report addr
  if ( rm_rnr_ptr->status != BT_CS_GN_SUCCESS )
  {
    ev_dev_name_resp.ev_msg.ev_sd_dname.reason =
      BT_EVR_SD_GET_DEVICE_NAME_FAILED;
  }
  #endif /*CUST_EDITION*/
  ev_dev_name_resp.ev_msg.ev_sd_dname.bd_addr = rm_rnr_ptr->bd_addr;

  #ifdef CUST_EDITION //modify by peng 2010-10-28, if cannot get remote name, report addr
  if ( rm_rnr_ptr->status != BT_CS_GN_SUCCESS )
  {
    snprintf( (char*)ev_dev_name_resp.ev_msg.ev_sd_dname.device_name_str, 
            BT_SD_MAX_DEVICE_NAME_LEN,
            "%x.%x.%x.%x.%x.%x",
            rm_rnr_ptr->bd_addr.bd_addr_bytes[0],
            rm_rnr_ptr->bd_addr.bd_addr_bytes[1],
            rm_rnr_ptr->bd_addr.bd_addr_bytes[2],
            rm_rnr_ptr->bd_addr.bd_addr_bytes[3],
            rm_rnr_ptr->bd_addr.bd_addr_bytes[4],
            rm_rnr_ptr->bd_addr.bd_addr_bytes[5]);
  }
  else
  #endif /*CUST_EDITION*/
  {
    memcpy( (void*)ev_dev_name_resp.ev_msg.ev_sd_dname.device_name_str,
            (void*)rm_rnr_ptr->name_str,
            BT_SD_MAX_DEVICE_NAME_LEN );
  }

  ev_dev_name_resp.ev_msg.ev_sd_dname.
    device_name_str[ BT_SD_MAX_DEVICE_NAME_LEN ] = 0;

  bt_ec_send_event( &ev_dev_name_resp );

  return TRUE;
}

uint8 bt_sd_extract_scn_from_protocol_descriptor_list
(
  bt_sd_proto_desc_list_type* pdl_ptr
)
{
  uint16 i;

  for ( i = 0; i < pdl_ptr->num_proto_desc; i++ )
  {
    if ( ( pdl_ptr->proto_desc[i].uuid == BT_SD_PROTOCOL_RFCOMM ) &&
         ( pdl_ptr->proto_desc[i].num_param == 1 ) )
    { 
      return( pdl_ptr->proto_desc[i].param[0].value & 0xFF );
    }
  }
  return( BT_RC_SCN_NOT_SPECIFIED );
}

/*===========================================================================

FUNCTION
  bt_sd_ev_sd_service_search_resp

DESCRIPTION
  Handles a SDP Service Search Response Event

===========================================================================*/

boolean bt_sd_ev_sd_service_search_resp
(
  bt_ev_sd_service_search_resp_type  *sd_ssr_ptr
)
{
  bt_ev_msg_type               ev;
  bt_sd_srv_rec_type*          srv_rec_ptr;
  uint8                        scn = BT_RC_SCN_NOT_SPECIFIED;

  BT_MSG_API( "BT SD EV RX: SD Service Search Resp", 0, 0, 0 );

  ev.ev_hdr.bt_app_id = bt_sd_data.app_id_user;
  bt_sd_data.app_id_user = BT_APP_ID_NULL;

  switch( bt_sd_data.app_cmd )
  {
    case BT_CMD_SD_GET_SERVER_CHANNEL_NUMBER:
    {
      bt_sd_srv_rec_iter.init(
        BT_SD_DB_TYPE_SRV_SRCH_RESULT_DB,
        BT_SD_ITER_ALL
      );
      srv_rec_ptr = ( bt_sd_srv_rec_type* ) bt_sd_srv_rec_iter.first();
      if ( ( srv_rec_ptr != 0 )               &&
           ( srv_rec_ptr->num_srv_attr == 1 ) &&
           ( srv_rec_ptr->srv_attr[0].attr_id ==
             BT_SD_ATTR_ID_PROTOCOL_DESCRIPTOR_LIST ) )
      {
        scn = bt_sd_extract_scn_from_protocol_descriptor_list(
                &( srv_rec_ptr->srv_attr[0].attr_value.proto_desc_list ) );
      }

      if ( scn != BT_RC_SCN_NOT_SPECIFIED )
      {
        ev.ev_hdr.ev_type = BT_EV_SD_SERVER_CHANNEL_NUMBER_RESP;
        ev.ev_msg.ev_sd_scn_resp.bd_addr = sd_ssr_ptr->bd_addr;
        ev.ev_msg.ev_sd_scn_resp.service_class = bt_sd_data.service_class;
        ev.ev_msg.ev_sd_scn_resp.scn = scn;

        BT_MSG_API( "BT SD EV TX: SD SCN Resp SCN %x",
                    ev.ev_msg.ev_sd_scn_resp.scn, 0, 0 );
      }
      else
      {
        ev.ev_hdr.ev_type = BT_EV_SD_ERROR_RESP;
        ev.ev_msg.ev_sd_error_resp.bd_addr = sd_ssr_ptr->bd_addr;
        ev.ev_msg.ev_sd_error_resp.error_code =
          BT_SD_EV_ERROR_SERVICE_NOT_FOUND;

        BT_MSG_API( "BT SD EV TX: SD Err Resp EC %x",
                    ev.ev_msg.ev_sd_error_resp.error_code, 0, 0 );
      }
      break;
    }

    case BT_CMD_SD_GET_SECONDARY_SERVER_CHANNEL_NUMBER:
    {
      bt_sd_srv_rec_iter.init(
        BT_SD_DB_TYPE_SRV_SRCH_RESULT_DB,
        BT_SD_ITER_ALL
      );
      srv_rec_ptr = ( bt_sd_srv_rec_type* ) bt_sd_srv_rec_iter.first();
      if ( ( srv_rec_ptr != 0 )               &&
           ( srv_rec_ptr->num_srv_attr == 1 ) &&
           ( srv_rec_ptr->srv_attr[0].attr_id ==
             BT_SD_ATTR_ID_ADDITIONAL_PROTOCOL_DESCRIPTOR_LISTS ) )
      {
        scn = bt_sd_extract_scn_from_protocol_descriptor_list( 
                &(srv_rec_ptr->srv_attr[0].attr_value.add_proto_desc_lists[0]));
      }

      if ( scn != BT_RC_SCN_NOT_SPECIFIED )
      {
        ev.ev_hdr.ev_type = BT_EV_SD_SECONDARY_SERVER_CHANNEL_NUMBER_RESP;
        ev.ev_msg.ev_sd_scn_resp.bd_addr = sd_ssr_ptr->bd_addr;
        ev.ev_msg.ev_sd_scn_resp.service_class = bt_sd_data.service_class;
        ev.ev_msg.ev_sd_scn_resp.scn = scn;

        BT_MSG_API( "BT SD EV TX: SD Sec SCN Resp SCN %x",
                    ev.ev_msg.ev_sd_scn_resp.scn, 0, 0 );
      }
      else
      {
        ev.ev_hdr.ev_type = BT_EV_SD_ERROR_RESP;
        ev.ev_msg.ev_sd_error_resp.bd_addr = sd_ssr_ptr->bd_addr;
        ev.ev_msg.ev_sd_error_resp.error_code =
          BT_SD_EV_ERROR_SERVICE_NOT_FOUND;

        BT_MSG_API( "BT SD EV TX: SD Err Resp EC %x",
                    ev.ev_msg.ev_sd_error_resp.error_code, 0, 0 );
      }
      break;
    }
  }

  bt_ec_send_event( &ev );

  return TRUE;
}


/*-------------------------------------------------------------------------*/
/*                                                                         */
/*                        External Functions                               */
/*                                                                         */
/*-------------------------------------------------------------------------*/

/*===========================================================================

FUNCTION
  bt_sd_clear_pending_request

DESCRIPTION
  This function clears a pending request by re-initializing the
  appropriate fields in the connection table

===========================================================================*/

void bt_sd_clear_pending_request(
  bt_sd_conn_entry_type*  conn_entry_ptr
)
{
  if ( conn_entry_ptr && conn_entry_ptr->req_pending )
  {
    conn_entry_ptr->req_pending = FALSE;
    dsm_free_packet( &( conn_entry_ptr->pending_req_dsm_ptr ) );
    dsm_free_packet( &( conn_entry_ptr->dup_req_dsm_ptr ) );
    dsm_free_packet( &conn_entry_ptr->resp_dsm_ptr );
    conn_entry_ptr->num_retries = 0;
  }
}

/*===========================================================================

FUNCTION
  bt_sd_send_error_resp_event

DESCRIPTION
  Send a BT_EV_SD_ERROR_RESP event

===========================================================================*/

void bt_sd_send_error_resp_event(
  bt_sd_conn_entry_type*  conn_entry_ptr,
  uint16                  ec
)
{
  bt_ev_msg_type   ev_err_resp;

  BT_MSG_SIG( "BT SD EV TX: SD Err Resp EC %x AID %x",
              ec, conn_entry_ptr->bt_app_id, 0 );

  ev_err_resp.ev_hdr.ev_type = BT_EV_SD_ERROR_RESP;
  ev_err_resp.ev_hdr.bt_app_id = conn_entry_ptr->bt_app_id;
  ev_err_resp.ev_msg.ev_sd_error_resp.bd_addr = conn_entry_ptr->bd_addr;
  ev_err_resp.ev_msg.ev_sd_error_resp.error_code =
    (bt_sd_error_code_enum_type)( ec );

  bt_ec_send_event( &ev_err_resp );

  bt_sd_clear_pending_request( conn_entry_ptr );

  /*  Reset Idle Timer if L2CAP is connected.  */
  if ( conn_entry_ptr->conn_state == BT_SD_CONN_STATE_CONNECTED )
  {
    rex_set_timer( &conn_entry_ptr->tmr_loc_initd,
                   BT_SD_IDLE_TIMEOUT_MS );
  }
}


/*===========================================================================

FUNCTION
  bt_sd_send_timeout_resp_event

DESCRIPTION
  Send a BT_EV_SD_TIMEOUT_RESP event

===========================================================================*/

void bt_sd_send_timeout_resp_event(
  bt_sd_conn_entry_type*  conn_entry_ptr
)
{
  bt_ev_msg_type   ev_timeout_resp;

  BT_MSG_SIG( "BT SD EV TX: SD Timeout Resp AID %x",
              conn_entry_ptr->bt_app_id, 0, 0 );

  ev_timeout_resp.ev_hdr.ev_type = BT_EV_SD_TIMEOUT_RESP;
  ev_timeout_resp.ev_hdr.bt_app_id = conn_entry_ptr->bt_app_id;
  ev_timeout_resp.ev_msg.ev_sd_timeout_resp.bd_addr = conn_entry_ptr->bd_addr;

  bt_ec_send_event( &ev_timeout_resp );

  bt_sd_clear_pending_request( conn_entry_ptr );

  /*  Reset Idle Timer if L2CAP is connected.  */
  if ( conn_entry_ptr->conn_state == BT_SD_CONN_STATE_CONNECTED )
  {
    rex_set_timer( &conn_entry_ptr->tmr_loc_initd,
                   BT_SD_IDLE_TIMEOUT_MS );
  }
}


/*===========================================================================

FUNCTION
  bt_sd_add_attribute_to_dsm_buffer

DESCRIPTION
  Add a service attribute to a DSM buffer

===========================================================================*/

void bt_sd_add_proto_desc_list_to_dsm_buffer(
  const bt_sd_proto_desc_list_type*  pdl_ptr, 
  dsm_item_type*                     dsm_ptr,
  dsm_mempool_id_enum_type           dsm_pool_id
)
{
  uint8                                      uint8_val;
  uint16                                     i, j, value;
  uint16                                     size_index = 0;
  uint16                                     uint16_val;
  const bt_sd_proto_desc_type*               proto_desc_ptr;
      
  for ( i = 0;
        i < pdl_ptr->num_proto_desc;
        i++ )
  {
    proto_desc_ptr = &( pdl_ptr->proto_desc[i] );
    uint8_val = ( BT_SD_DATA_ELEMENT_SEQUENCE << 3 ) | 5;
    bt_sd_dsm_push8( &dsm_ptr, &uint8_val, dsm_pool_id );
    bt_sd_dsm_push8( &dsm_ptr,
                     ( uint8* ) &( proto_desc_ptr->header.attr_value_len ),
                     dsm_pool_id );

    if ( proto_desc_ptr->is_uuid128 )
    {
      /* Insert 128-bit UUID */
      uint8_val = ( BT_SD_DATA_ELEMENT_UUID << 3 ) | 4; /* size=4 => 16 bytes */
      bt_sd_dsm_push8( &dsm_ptr, &uint8_val, dsm_pool_id );
      bt_sd_dsm_pushdown_tail(
        &dsm_ptr,
        ( uint8* ) &proto_desc_ptr->uuid128,
        sizeof( bt_sd_uuid128_type ),
        dsm_pool_id );
    }
    else
    {
      /* Insert 16-bit UUID */
      uint8_val = ( BT_SD_DATA_ELEMENT_UUID << 3 ) | 1;
      bt_sd_dsm_push8( &dsm_ptr, &uint8_val, dsm_pool_id );
      uint16_val = SWAB16( proto_desc_ptr->uuid );
      bt_sd_dsm_push16( &dsm_ptr, &uint16_val, dsm_pool_id );
    }

    /* Insert parameter(s) */
    for ( j = 0; j < proto_desc_ptr->num_param; j++ )
    {
      switch( proto_desc_ptr->param[j].size )
      {
        case 1: /* 1 byte */
          size_index = 0;
          value = proto_desc_ptr->param[j].value;
          break;
        case 2: /* 2 bytes */
          size_index = 1;
          value = SWAB16( proto_desc_ptr->param[j].value );
          break;
        default:
          BT_ERR( "BT SD: Bad Param size %x I %x",
                  proto_desc_ptr->param[j].size, j, 0 );
          size_index = 0;
          break;
      }

      uint8_val = ( BT_SD_DATA_ELEMENT_UNSIGNED_INT << 3 ) | size_index;
      bt_sd_dsm_push8( &dsm_ptr, &uint8_val, dsm_pool_id );
      bt_sd_dsm_pushdown_tail( &dsm_ptr, (uint8*) &value,
                               proto_desc_ptr->param[j].size,
                               dsm_pool_id );
    }
  }
}

void bt_sd_add_attribute_to_dsm_buffer(
  const bt_sd_srv_attr_rec_type*   srv_attr_ptr,
  dsm_item_type*                   dsm_ptr,
  dsm_mempool_id_enum_type         dsm_pool_id
)
{
  uint8                                      uint8_val, size;
  uint16                                     i;
  uint16                                     size_index = 0;
  uint16                                     uint16_val;
  uint32                                     uint32_val;
  uint64                                     uint64_val;
  const bt_sd_proto_desc_list_type*          proto_desc_list_ptr;
  const bt_sd_lang_base_attr_id_list_type*   lb_attr_id_list_ptr;
  const bt_sd_uint_list_type*                uint_list_ptr;

  /* Insert attribute ID */
  uint8_val = 0x09;
  bt_sd_dsm_push8( &dsm_ptr, &uint8_val, dsm_pool_id );
  uint16_val = SWAB16( srv_attr_ptr->attr_id );
  bt_sd_dsm_push16( &dsm_ptr, &uint16_val, dsm_pool_id );

  /* Insert data element header for attribute value */
  uint8_val = ( srv_attr_ptr->header.type << 3 ) |
                  srv_attr_ptr->header.size_index;
  bt_sd_dsm_push8( &dsm_ptr, &uint8_val, dsm_pool_id );
  if ( srv_attr_ptr->header.hdr_len > 1 )
  {
    bt_sd_dsm_push8( &dsm_ptr,
                     ( uint8* ) &( srv_attr_ptr->header.attr_value_len ),
                     dsm_pool_id );
  }

  switch( srv_attr_ptr->attr_type )
  {
    case BT_SD_ATTR_TYPE_UUID_LIST:
      for ( i = 0; i < srv_attr_ptr->attr_value.uuid_list.num_uuid128; i++ )
      {
        uint8_val = ( BT_SD_DATA_ELEMENT_UUID << 3 ) | 4;
        bt_sd_dsm_push8( &dsm_ptr, &uint8_val, dsm_pool_id );
        bt_sd_dsm_pushdown_tail(
          &dsm_ptr,
          ( uint8* ) &srv_attr_ptr->attr_value.uuid_list.uuid128[i],
          sizeof( bt_sd_uuid128_type ),
          dsm_pool_id );
      }
      for ( i = 0; i < srv_attr_ptr->attr_value.uuid_list.num_uuid; i++ )
      {
        uint8_val = ( BT_SD_DATA_ELEMENT_UUID << 3 ) | 1;
        bt_sd_dsm_push8( &dsm_ptr, &uint8_val, dsm_pool_id );
        uint16_val = SWAB16( srv_attr_ptr->attr_value.uuid_list.uuid[i] );
        bt_sd_dsm_push16( &dsm_ptr, &uint16_val, dsm_pool_id );
      }
      break;
    case BT_SD_ATTR_TYPE_PROTO_DESC_LIST:
      bt_sd_add_proto_desc_list_to_dsm_buffer(
        &srv_attr_ptr->attr_value.proto_desc_list, dsm_ptr, dsm_pool_id ); 
      break;
    case BT_SD_ATTR_TYPE_ADD_PROTO_DESC_LISTS:
      for ( i = 0; i < BT_SD_MAX_NUM_OF_ADD_PROTO_DESC_LISTS; i++ )
      {
        proto_desc_list_ptr = &srv_attr_ptr->attr_value.add_proto_desc_lists[i];
        
        /* Insert data element header for each protocol descriptor list */
        uint8_val = ( proto_desc_list_ptr->header.type << 3 ) |
                      proto_desc_list_ptr->header.size_index;
        bt_sd_dsm_push8( &dsm_ptr, &uint8_val, dsm_pool_id );
        bt_sd_dsm_push8(
          &dsm_ptr,
          ( uint8* ) &( proto_desc_list_ptr->header.attr_value_len ),
          dsm_pool_id );
        
        bt_sd_add_proto_desc_list_to_dsm_buffer(
          proto_desc_list_ptr, dsm_ptr, dsm_pool_id );
      }
      break;
    case BT_SD_ATTR_TYPE_LANG_BASE_ATTR_ID_LIST:
      lb_attr_id_list_ptr =
        &( srv_attr_ptr->attr_value.lang_base_attr_id_list );
      for ( i = 0; i < lb_attr_id_list_ptr->num_lang_base_rec; i++ )
      {
        /* Insert language ID */
        uint8_val = ( BT_SD_DATA_ELEMENT_UNSIGNED_INT << 3 ) | 1;
        bt_sd_dsm_push8( &dsm_ptr, &uint8_val, dsm_pool_id );
        uint16_val = SWAB16(lb_attr_id_list_ptr->lang_base_rec[i].lang_id);
        bt_sd_dsm_push16( &dsm_ptr, &uint16_val, dsm_pool_id );

        /* Insert character encoding ID */
        uint8_val = ( BT_SD_DATA_ELEMENT_UNSIGNED_INT << 3 ) | 1;
        bt_sd_dsm_push8( &dsm_ptr, &uint8_val, dsm_pool_id );
        uint16_val =
          SWAB16(lb_attr_id_list_ptr->lang_base_rec[i].char_enc_id);
        bt_sd_dsm_push16( &dsm_ptr, &uint16_val, dsm_pool_id );

        /* Insert base attribute ID */
        uint8_val = ( BT_SD_DATA_ELEMENT_UNSIGNED_INT << 3 ) | 1;
        bt_sd_dsm_push8( &dsm_ptr, &uint8_val, dsm_pool_id );
        uint16_val =
          SWAB16(lb_attr_id_list_ptr->lang_base_rec[i].base_attr_id);
        bt_sd_dsm_push16( &dsm_ptr, &uint16_val, dsm_pool_id );
      }
      break;
    case BT_SD_ATTR_TYPE_UINT_LIST:
      uint_list_ptr = &( srv_attr_ptr->attr_value.uint_list );
      size = uint_list_ptr->size;

      switch( size )
      {
        case 1: /* 1 byte */
          size_index = 0;
          break;
        case 2: /* 2 bytes */
          size_index = 1;
          break;
        case 4: /* 4 bytes */
          size_index = 2;
          break;
        case 8: /* 8 bytes */
          size_index = 3;
          break;
        default:
          BT_ERR( "BT SD: Bad size %x ", size, 0, 0 );
          size_index = 0;
          break;
      }

      for ( i = 0; i < uint_list_ptr->num_val; i++ )
      {
        uint8_val = ( BT_SD_DATA_ELEMENT_UNSIGNED_INT << 3 ) | size_index;
        bt_sd_dsm_push8( &dsm_ptr, &uint8_val, dsm_pool_id );
        switch( size )
        {
          case 1:
            uint8_val = (uint8) uint_list_ptr->val[i];
            bt_sd_dsm_push8( &dsm_ptr, &uint8_val, dsm_pool_id );
            break;
          case 2:
            uint16_val = (uint16) uint_list_ptr->val[i];
            uint16_val = SWAB16( uint16_val );
            bt_sd_dsm_push16( &dsm_ptr, &uint16_val, dsm_pool_id );
            break;
          case 4:
            uint32_val = (uint32) uint_list_ptr->val[i];
            uint32_val = SWAB32( uint32_val );
            bt_sd_dsm_push32( &dsm_ptr, &uint32_val, dsm_pool_id );
            break;
          case 8:
            uint64_val = SWAB64( uint_list_ptr->val[i] );
            bt_sd_dsm_push64( &dsm_ptr, &uint64_val, dsm_pool_id );
            break;
        }
      }
      break;
    case BT_SD_ATTR_TYPE_STRING:
      bt_sd_dsm_pushdown_tail( &dsm_ptr,
                               ( uint8* ) srv_attr_ptr->attr_value.str, 
                               srv_attr_ptr->header.attr_value_len,
                               dsm_pool_id );
      break;
    case BT_SD_ATTR_TYPE_UINT8:
      bt_sd_dsm_push8( &dsm_ptr,
                       ( uint8* ) &(srv_attr_ptr->attr_value.primitive_value),
                       dsm_pool_id );
      break;
    case BT_SD_ATTR_TYPE_UINT16:
      uint16_val = SWAB16( srv_attr_ptr->attr_value.primitive_value );
      bt_sd_dsm_push16( &dsm_ptr, &uint16_val, dsm_pool_id );
      break;
    case BT_SD_ATTR_TYPE_UINT32:
      uint32_val = SWAB32( srv_attr_ptr->attr_value.primitive_value );
      bt_sd_dsm_push32( &dsm_ptr, &uint32_val, dsm_pool_id );
      break;
    case BT_SD_ATTR_TYPE_UINT64:
      uint64_val = SWAB64( srv_attr_ptr->attr_value.primitive_value );
      bt_sd_dsm_push64( &dsm_ptr, &uint64_val, dsm_pool_id );
      break;
    case BT_SD_ATTR_TYPE_BOOL:
      bt_sd_dsm_push8( &dsm_ptr,
                       ( uint8* ) &(srv_attr_ptr->attr_value.bool_flag),
                       dsm_pool_id );
      break;
    case BT_SD_ATTR_TYPE_UUID:
      bt_sd_dsm_push16( &dsm_ptr,
                       ( uint16* ) &(srv_attr_ptr->attr_value.primitive_value),
                       dsm_pool_id );
      break;
    case BT_SD_ATTR_TYPE_UUID128:
      bt_sd_dsm_pushdown_tail( &dsm_ptr,
                               ( uint8* ) &srv_attr_ptr->attr_value.uuid128,
                               sizeof( bt_sd_uuid128_type ),
                               dsm_pool_id );
      break;
    case BT_SD_ATTR_TYPE_HID_DESC_LIST:
      for ( i = 0;
            i < srv_attr_ptr->attr_value.hid_desc_list.num_hid_class_desc;
            i++ )
      {
        const bt_sd_hid_class_desc_type* hid_class_desc_ptr =
          &( srv_attr_ptr->attr_value.hid_desc_list.hid_class_desc[i] );

        /* Insert data element sequence header */
        uint8_val = ( hid_class_desc_ptr->header.type << 3 ) |
                        hid_class_desc_ptr->header.size_index;
        bt_sd_dsm_push8( &dsm_ptr, &uint8_val, dsm_pool_id );
        bt_sd_dsm_push8( &dsm_ptr,
                         ( uint8* ) &( hid_class_desc_ptr->header.attr_value_len ),
                         dsm_pool_id );

        /* Insert uint8 */
        uint8_val = ( BT_SD_DATA_ELEMENT_UNSIGNED_INT << 3 ) | 0;
        bt_sd_dsm_push8( &dsm_ptr, &uint8_val, dsm_pool_id );
        bt_sd_dsm_push8( &dsm_ptr,
                         ( uint8* ) &hid_class_desc_ptr->val,
                         dsm_pool_id );

        /* Insert string */
        uint8_val = ( BT_SD_DATA_ELEMENT_TEXT_STRING << 3 ) | 5;
        bt_sd_dsm_push8( &dsm_ptr, &uint8_val, dsm_pool_id );
        uint8_val = hid_class_desc_ptr->len;
        bt_sd_dsm_push8( &dsm_ptr, &uint8_val, dsm_pool_id );
        bt_sd_dsm_pushdown_tail( &dsm_ptr,
                                 (uint8*) hid_class_desc_ptr->str,
                                 uint8_val,
                                 dsm_pool_id );
      }
      break;
	#ifdef CUST_EDITION
	case BT_SD_ATTR_TYPE_UNKNOWN:
		 {
			 bt_sd_dsm_pushdown_tail( &dsm_ptr,
                               		  ( uint8 * )( srv_attr_ptr->attr_value.custom_attr_value.Data + srv_attr_ptr->header.hdr_len ),
                               		  srv_attr_ptr->header.attr_value_len,
                               		  dsm_pool_id );
		 }
		 break;
	#endif
	
    default:
      BT_ERR( "BT SD: Bad Attr Type %x ",
              srv_attr_ptr->attr_type, 0, 0 );
      break;
  }
}


/*===========================================================================

FUNCTION
  bt_sd_find_conn_entry_by_bd_addr

DESCRIPTION
  Find an L2CAP connection entry by BD_ADDR

===========================================================================*/

bt_sd_conn_entry_type* bt_sd_find_conn_entry_by_bd_addr(
  const bt_bd_addr_type*   ba_ptr
)
{
  uint16 i;

  for ( i = 0; i < BT_SD_MAX_NUM_OF_L2CAP_CONNECTION; i++ )
  {
    if ( bt_sd_conn_table[i].used &&
         ( BT_BD_ADDRS_EQUAL( &( bt_sd_conn_table[i].bd_addr ), ba_ptr ) ) )
    {
      return &( bt_sd_conn_table[i] );
    }
  }

  return NULL;
}


/*===========================================================================

FUNCTION
  bt_sd_find_conn_entry_by_cid

DESCRIPTION
  Find an L2CAP connection entry by channel ID

===========================================================================*/

bt_sd_conn_entry_type* bt_sd_find_conn_entry_by_cid( uint16 cid )
{
  uint16 i;

  for ( i = 0; i < BT_SD_MAX_NUM_OF_L2CAP_CONNECTION; i++ )
  {
    if ( bt_sd_conn_table[i].used &&
         ( ( bt_sd_conn_table[i].cid_loc_initd == cid ) ||
           ( bt_sd_conn_table[i].cid_rem_initd == cid ) ) )
    {
      return &( bt_sd_conn_table[i] );
    }
  }

  return NULL;
}


/*===========================================================================

FUNCTION
  bt_sd_add_conn_entry

DESCRIPTION
  Add a new entry to the L2CAP connection table

===========================================================================*/

bt_sd_conn_entry_type* bt_sd_add_conn_entry(
  const bt_bd_addr_type*   ba_ptr
)
{
  uint16 i;

  /* Find empty entry */
  for ( i = 0;
        ( ( i < BT_SD_MAX_NUM_OF_L2CAP_CONNECTION ) &&
          ( bt_sd_conn_table[i].used ) );
        i++ );

  /* Table full */
  if ( i == BT_SD_MAX_NUM_OF_L2CAP_CONNECTION )
  {
    return NULL;
  }

  bt_sd_conn_table[i].used = TRUE;
  bt_sd_conn_table[i].bd_addr = *ba_ptr;
  bt_sd_conn_table[i].cid_loc_initd = BT_L2_NULL_CID;
  bt_sd_conn_table[i].cid_rem_initd = BT_L2_NULL_CID;
  bt_sd_conn_table[i].tid = 0;
  bt_sd_conn_table[i].req_pending = FALSE;
  bt_sd_conn_table[i].bt_app_id = BT_APP_ID_NULL;
  bt_sd_conn_table[i].pending_req_dsm_ptr = NULL;
  bt_sd_conn_table[i].dup_req_dsm_ptr = NULL;
  bt_sd_conn_table[i].resp_dsm_ptr = NULL;
  bt_sd_conn_table[i].num_resp_recv = 0;
  bt_sd_conn_table[i].conn_state = BT_SD_CONN_STATE_DISCONNECTED;
  bt_sd_conn_table[i].same_cont_state_count = 0;
  bt_sd_conn_table[i].num_retries = 0;
  memset(bt_sd_conn_table[i].cont_state_info,0,BT_SD_MAX_CONTINUATION_STATE_INFO_LEN);

  return &( bt_sd_conn_table[i] );
}


/*===========================================================================

FUNCTION
  bt_sd_any_connections_active

DESCRIPTION
  Returns whether there are any SDP connections active.

===========================================================================*/
boolean bt_sd_any_connections_active
(
  void
)
{

  boolean  any_connections_active = FALSE;

  uint16 i;

  for ( i = 0; i < BT_SD_MAX_NUM_OF_L2CAP_CONNECTION; i++ )
  {
    if ( ((bt_sd_conn_table[i].used != FALSE) &&
          (bt_sd_conn_table[i].conn_state != BT_SD_CONN_STATE_DISCONNECTED ) ) )
    {
      any_connections_active = TRUE;
      break;
    }
  }

  return ( any_connections_active );

}


/*===========================================================================

FUNCTION
  bt_sd_get_max_event_bytes

DESCRIPTION
  Returns the number of bytes in the largest event expected to be
  processed by SDP.

===========================================================================*/
uint16 bt_sd_get_max_event_bytes
(
  void
)
{

  uint16  max_eb;

  max_eb = sizeof( bt_ev_gn_cmd_done_type );
  max_eb = MAX( max_eb, sizeof( bt_ev_l2_connected_type ) );
  max_eb = MAX( max_eb, sizeof( bt_ev_l2_connection_failed_type ) );
  max_eb = MAX( max_eb, sizeof( bt_ev_l2_rx_data_type ) );
  max_eb = MAX( max_eb, sizeof( bt_ev_l2_disconnected_type ) );
  max_eb = MAX( max_eb, sizeof( bt_ev_rm_inquiry_result_type ) );
  max_eb = MAX( max_eb, sizeof( bt_ev_rm_inquiry_complete_type ) );
  max_eb = MAX( max_eb, sizeof( bt_ev_rm_remote_name_type ) );
  max_eb = MAX( max_eb, sizeof( bt_ev_sd_service_search_resp_type ) );
  max_eb = MAX( max_eb, sizeof( bt_ev_sd_error_resp_type ) );
  max_eb = MAX( max_eb, sizeof( bt_ev_sd_timeout_resp_type ) );

  max_eb += sizeof( bt_ev_hdr_type );

  return( max_eb );

}

/*===========================================================================

FUNCTION
  bt_sd_ev_bad_cmd_done

DESCRIPTION
  Handles Command Done event with status other than BT_CS_GN_SUCCESS.

===========================================================================*/
LOCAL void bt_sd_ev_bad_cmd_done( bt_ev_gn_cmd_done_type* ev_ptr )
{
  switch ( ev_ptr->cmd_type )
  {
    case BT_CMD_RM_INQUIRE:
    {
      bt_sd_ev_rm_inquiry_complete( BT_EVR_SD_DEVICE_DISCOVERY_FAILED );
      break;
    }
    case BT_CMD_RM_REMOTE_NAME_REQ:
    {
      bt_ev_rm_remote_name_type tmp_ev;
      tmp_ev.status = ev_ptr->cmd_status;
      tmp_ev.bd_addr = bt_sd_dev_discv_db.remote_name_bd_addr;
      tmp_ev.name_str[ 0 ] = NULL;
      bt_sd_ev_rm_remote_name( &tmp_ev );
      break;
    }
  }
}

/*===========================================================================

FUNCTION
  bt_sd_process_event

DESCRIPTION
  Dequeues events on the SDP event queue and processes them.

===========================================================================*/

void bt_sd_process_event
(
  bt_ev_msg_type*  ev_msg_ptr
)
{
  switch ( ev_msg_ptr->ev_hdr.ev_type )
  {
    case BT_EV_GN_CMD_DONE:
    {
      if ( ev_msg_ptr->ev_msg.ev_gn_cmd_done.cmd_status !=
           BT_CS_GN_SUCCESS )
      {
        BT_ERR( "BT SD: Bad CMD Done Stat %x Typ %x AID %x",
                ev_msg_ptr->ev_msg.ev_gn_cmd_done.cmd_status,
                ev_msg_ptr->ev_msg.ev_gn_cmd_done.cmd_type,
                ev_msg_ptr->ev_hdr.bt_app_id );
        bt_sd_ev_bad_cmd_done( &ev_msg_ptr->ev_msg.ev_gn_cmd_done );
      }
      break;
    }

    case BT_EV_L2_CONNECTED:
    {
      bt_sd_ev_l2_connected( &ev_msg_ptr->ev_msg.ev_l2_conn );
      break;
    }

    case BT_EV_L2_CONNECTION_FAILED:
    {
      bt_sd_ev_l2_connection_failed( &ev_msg_ptr->ev_msg.ev_l2_conn_failed );
      break;
    }

    case BT_EV_L2_RX_DATA:
    {
      if ( ! bt_sd_ev_l2_rx_data( &ev_msg_ptr->ev_msg.ev_l2_rxd ) )
      {
        BT_MSG_HIGH( "BT SD: Unable to process received data", 0, 0, 0 );
      }
      break;
    }

    case BT_EV_L2_DISCONNECTED:
    {
      bt_sd_ev_l2_disconnected( &ev_msg_ptr->ev_msg.ev_l2_disc );
      break;
    }

    case BT_EV_RM_INQUIRY_RESULT:
    {
      bt_sd_ev_rm_inquiry_result( &ev_msg_ptr->ev_msg.ev_rm_inqrs );
      break;
    }

    case BT_EV_RM_INQUIRY_COMPLETE:
    {
      bt_sd_ev_rm_inquiry_complete( BT_EVR_GN_SUCCESS );
      break;
    }

    case BT_EV_RM_REMOTE_NAME:
    {
      bt_sd_ev_rm_remote_name( &ev_msg_ptr->ev_msg.ev_rm_rname );
      break;
    }

    case BT_EV_RM_DEVICE_UPDATED:
    {
      //  nothing to do here
      break;
    }

    case BT_EV_SD_SERVICE_SEARCH_RESP:
    {
      bt_sd_ev_sd_service_search_resp(
        &ev_msg_ptr->ev_msg.ev_sd_service_search_resp
      );
      break;
    }

    case BT_EV_SD_ERROR_RESP:
    case BT_EV_SD_TIMEOUT_RESP:
    {
      /* Forward to application */
      ev_msg_ptr->ev_hdr.bt_app_id = bt_sd_data.app_id_user;
      bt_ec_send_event( ev_msg_ptr );

      bt_sd_data.app_id_user = BT_APP_ID_NULL;

      break;
    }

    default:
    {
      BT_ERR( "BT SD: Bad RX EV Type %x AID %x",
              ev_msg_ptr->ev_hdr.ev_type,
              ev_msg_ptr->ev_hdr.bt_app_id, 0 );
      break;
    }
  }
}


/*===========================================================================

FUNCTION
  bt_sd_find_discovered_device

DESCRIPTION
  Find a discovered device record by BD_ADDR

===========================================================================*/

bt_sd_device_discovery_rec_type* bt_sd_find_discovered_device
(
  bt_bd_addr_type*           bd_addr_ptr
)
{
  uint16                            i;
  bt_sd_device_discovery_rec_type*  dev_rec_ptr = NULL;

  for ( i = 0; i < bt_sd_dev_discv_db.num_discvd_dev; i++ )
  {
    dev_rec_ptr = &bt_sd_dev_discv_db.discvd_dev_info[ i ];

    if ( BT_BD_ADDRS_EQUAL( &dev_rec_ptr->bd_addr, bd_addr_ptr ) )
    {
      return dev_rec_ptr;
    }
  }

  return NULL;
}

#endif /* FEATURE_BT */

