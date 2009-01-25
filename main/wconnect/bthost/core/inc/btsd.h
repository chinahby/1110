#ifndef _BTSD_H
#define _BTSD_H

#ifdef FEATURE_BT

/*====*====*====*====*====*====*====*====*====*====*====*====*====*====*====*

            B L U E T O O T H    S D P    M O D U L E

DESCRIPTION
  This file contains declarations external to the Bluetooth SDP module.

Copyright (c) 2000 - 2009 by QUALCOMM Incorporated. All Rights Reserved.
*====*====*====*====*====*====*====*====*====*====*====*====*====*====*====*/

/*===========================================================================

                        EDIT HISTORY FOR MODULE

  This section contains comments describing changes made to the module.
  Notice that changes are listed in reverse chronological order.

$PVCSHeader:   O:/src/asw/COMMON/vcs/btsd.h_v   1.16   21 Feb 2002 19:16:36   ryuen
$Header: //source/qcom/qct/wconnect/bthost/core/rel/00.00.26/inc/btsd.h#2 $ $DateTime: 2009/08/12 10:38:18 $

$Log:   O:/src/asw/COMMON/vcs/btsd.h_v  $

  when        who  what, where, why
  ----------  ---  -----------------------------------------------------------
  2009-08-12   gs  Added BT_SD_SERVICE_CLASS_PBAP UUID definition

 *
 *    #55         25 Feb 2008         RS
 * Increased the BT_SD_MAX_HID_CLASS_DESC_STRING_LEN so as to match the space
 * that gets operated upon after creation in OEMBTExtSD.c to a value of
 * AEEBT_SD_MAX_HID_CLASS_DESC_STRING_LEN.
 *
 *    #54         28 JAN 2008         RS
 * Increased the BT_SD_MAX_SAME_CONTINUATION_HOPS_ALLOWED as the Winmobile 
 * sends lots of search response data on the same continuation state.
 *
 *    #53         14 Dec 2007         HM
 * Externed function bt_sd_get_attr_type. Created definition for 
 * bt_sd_lang_base_rec_type
 *
 *    #52         03 Dec 2007         PN
 * Removed extern declaration of bt_sd_create_xml_service_record().
 *
 *    #51         29 Nov 2007         HM
 * Added SD command to return service search results in XML format
 * Added a parameter to specify duration of a device search.
 * Added a direct function call to get the discovery mode.
 *
 *    #50          07 Nov 2007         GS
 * Added support for HID profile host role.
 * Added API for canceling SD timer.
 *
 *    #49       01 Nov 2007            AG
 * Merged Lisbon(BT2.1) changes into the mainline
 *
 *    #48        26 Sep  2007           SSK
 * Added 'num_retries' field to 'bt_sd_conn_entry_type'. This is used for
 * supporting 'SDP Search Retry' when SDP search fails due to L2CAP disconnect.
 *
 *    #47        21 Dec  2006           VK
 * SD module limits numbers of hops for SDP requests if "ContinuationState" 
 * in SDP responses do not change.
 *
 *    #46        14 Nov  2006           RY
 * Added SD support for custom 128-bit UUID
 *
 *    #45        23 Oct 2006            JTL
 * Added L2CAP MTU to connection entry type.
 *
 *    #44        31 Aug 2006            RY
 * Added bt_sd_conn_state_type and added state variable to bt_sd_conn_entry_type
 * to keep track of L2CAP connection state.
 *
 *    #43        21 Jul 2006            RY
 * Added bt_sd_validate_service_record.
 *
 *    #42        30 May 2006            GS
 * Added support for PBAP feature.
 *
 *    #41        10 Nov 2005            DP
 * Added connectable controls to SD service registration.
 *
 *    #39        07 Sep 2005            RY
 * Added string length to the HID class descriptor definition and handling.
 *
 *    #38        02 Sep 2005            RY
 * Added support for the HID descriptor list service attribute.
 *
 *    #37        24 Jun 2005            PN
 * Added remote_name_bd_addr to structure bt_sd_device_discovery_db_type.
 *
 *    #36        26 Jun 2005            JTL
 * Added more consts from the Assigned Numbers document for protocols
 * and UUIDs.
 *
 *    #35        15 Jun 2005            RY
 * Added bt_sd_service_record_init_attribute
 *
 *    #34        16 May 2005            RY
 * Added BT_SD_MAJOR_SERVICE_CLASS_POSITIONING.
 *
 *    #33        22 Jan 2005            PN
 * Added support for SIM Access profile.
 *
 *    #32        30 Dec 2004            JH
 * Added support for parsing BPP printer specific attributes.
 *
 *    #31        29 Dec 2004            JH
 * Added support for parsing the additional protocol descriptor lists attribute.
 *
 *    #30        23 Nov 2004            JH
 * Updated assigned UUIDs for BT service class according to the latest BT 
 *  assigned numbers.
 * Added definition of bt_sd_major_service_class_enum_type
 *
 *    #29        01 Nov 2004            JH
 * Reduced BT_SD_MAX_NUM_OF_PROTO_DESC from 8 to 4.
 *
 *    #28        29 Sep 2004            JH
 * Added data element sequence of uint to protocol descriptor structure.
 *
 *    #27        24 Sep 2004            DP
 * Reworked AMS compilation controls.
 *
 *    #25        30 Aug 2004            JH
 * Added support for 128-bit UUID.
 *
 *    #24        2  Jun 2004            DP
 * Implemented idle timeout disconnect for SDP connections initiated
 * by remote devices.
 *
 *    #23        30 Apr PDT             RY
 * Added the support of 128-bit UUID for SyncML.
 *
 *    #22        14 Apr 2004            RY
 * 1. Added BIP and BPP UUIDs.
 * 2. Added externs for bt_sd_convert_128_to_16_bit_uuid() and
 *    bt_sd_convert_16_to_128_bit_uuid().
 *
 *    #21        30 Mar 2004            DP
 * Added EMS support.
 *
 *    #20        27 Feb 2004            RY
 * Updated the SDP idle timeout to 10 seconds.  This is needed for P2MP.
 *
 *    #19        27 Feb 2004            RY
 * Updated the SDP request timeout to 10 seconds.  This is needed for P2MP.
 *
 *    #18        26 Feb 2004            DP
 * Increased SD request timeout to 2 seconds.
 *
 *    #17        25 Feb 2004            JH
 * Defeaturize macros for manipulating object transfer bit in class of service 
 * field to avoid unnecessary dependency
 *
 *    #16        23 Feb 2004            JH
 * Added macro for clearing object transfer bit in class of service field
 *
 *    #15        23 Feb 2004            JH
 * Added macro for setting object transfer bit in class of service field
 *
 *    #14        27 Jan 2004            RY
 * Added BT_SD_ATTR_TYPE_UUID.
 *
 *    #13        14 Jan 2004            DP
 * Added support for limited discoverable scanning.
 * Registering service only affects page scanning now.
 *
 *    #12        19 Dec 2003            DP
 * Increased maximum number of protocol descriptors in list.
 *
 *    #11        02 Oct 2003            PN
 * Added utility functions bt_sd_extract_srvc_cls(), bt_sd_extract_scn(),
 * bt_sd_extract_profile_version(), bt_sd_parse_class_of_device(), and
 * bt_sd_build_class_of_device().
 *
 *    #10        26 Sep 2003            DP
 * Moved constant to bt.h
 *
 *    #9         9  Sep 2003            RY
 * Added 'cid2' to bt_sd_conn_entry_type.
 *
 *    #8         03 Jul 2003            RY
 * Added the support for service attributes SupportedCapabilities,
 * SupportedFunctions, and TotalImagingDataCapacity for BIP.
 *
 *    Rev 1.16   21 Feb 2002 19:16:36   ryuen
 * Removed featurization on FEATURE_BT_SIO_API_ENHANCEMENT
 *
 *    Rev 1.15   08 Feb 2002 17:41:06   ryuen
 * Added bt_sd_free_service_record
 *
 *    Rev 1.14   14 Sep 2001 01:26:48   ryuen
 * Moved bt_sd_error_code_enum_type and bt_sd_ev_error_code_enum_type to bt.h
 *
 *    Rev 1.13   07 Sep 2001 16:06:46   propach
 * Optimized event handling.
 *
 *    Rev 1.12   29 Aug 2001 11:57:30   propach
 * Merged certification tip onto main tip.
 *
 *    Rev 1.11.1.2   13 Aug 2001 14:25:56   ryuen
 * 1. Added BT_SD_EV_ERROR_SERVICE_NOT_FOUND to bt_sd_ev_error_code_enum_type
 * 2. Added timer of rex_timer_type to bt_sd_conn_entry_type
 * 3. Added bt_sd_data_type
 * 4. Added extern definitions for bt_sd_clear_pending_request and
 *    bt_sd_send_timeout_resp_event
 * 5. Added extern definitions for bt_sd_process_timer_expiration
 *
 *    Rev 1.11.1.1   15 Jul 2001 14:17:20   ryuen
 * Added extern declaration for bt_sd_sig_tx_logging_function and
 * bt_sd_sig_rx_logging_function
 *
 *    Rev 1.11.1.0   29 Jun 2001 15:25:36   waynelee
 * No change.
 *
 *    Rev 1.11   25 May 2001 15:41:04   ryuen
 * Renamed FEATURE_BT_MOBILE_MAP to FEATURE_BT_LPOS
 *
 *    Rev 1.10   25 May 2001 10:12:50   ryuen
 * Added support for FEATURE_BT_MOBILE_MAP
 *
 *    Rev 1.9   15 May 2001 10:41:28   ryuen
 * Added BT_SD_INVALID_UUID
 *
 *    Rev 1.8   26 Apr 2001 23:27:40   ryuen
 * 1. Updated bt_sd_add_attribute_to_dsm_buffer with new DSM_POOL_ID parameter
 * 2. Removed ATTR_BYTE_COUNT_PTR parameter from bt_sd_match_service_record
 *
 *    Rev 1.7   16 Apr 2001 14:18:38   ryuen
 * Increased BT_SD_MAX_NUM_OF_DISCOVERED_DEVICE to 30
 *
 *    Rev 1.6   29 Mar 2001 22:24:46   ryuen
 * Added BT_SD_EV_ERROR_CONNECTION_FAILED
 *
 *    Rev 1.5   21 Mar 2001 23:27:44   ryuen
 * Removed unnecessary continution state information from bt_sd_conn_entry_type
 *
 *    Rev 1.4   16 Mar 2001 20:57:52   ryuen
 * Added bt_sd_device_discovery_rec_type and bt_sd_find_discovered_device()
 *
 *    Rev 1.3   02 Mar 2001 01:15:14   ryuen
 * Renamed bt_xx_offline to bt_xx_qdsp_reset_shutdown
 *
 *    Rev 1.2   23 Feb 2001 15:32:06   ryuen
 * Renamed bt_sd_initialize and bt_sd_online to bt_sd_powerup_init and
 * bt_sd_qdsp_ready_init respectively
 *
 *    Rev 1.1   07 Feb 2001 10:18:28   propach
 * Reworked internal BT application IDs and made event control
 * route based on these.
 *
 *    Rev 1.0   29 Jan 2001 17:10:08   propach
 * Initial revision.
 *
 *    Rev 1.14   18 Jan 2001 16:14:28   ryuen
 * Added extern definition for bt_sd_initialize
 *
 *    Rev 1.13   08 Jan 2001 14:57:20   ryuen
 * Added max_discvd_dev to bt_sd_device_discovery_db_type
 *
 *    Rev 1.12   30 Nov 2000 22:06:54   ryuen
 * Updated bt_sd_device_discovery_db_type with new field
 * 'remote_name_req_in_progress'
 *
 *    Rev 1.11   13 Nov 2000 16:32:38   amitj
 * Removed the prototypes of bt_rc_sig_tx() and bt_sd_sig_rx_logging_function()
 *
 *    Rev 1.10   09 Nov 2000 11:35:10   amitj
 * Declared prototype for bt_sd_snapshot_logging_function()
 *
 *    Rev 1.9   08 Nov 2000 13:14:28   amitj
 * Declared 2 new functions: bt_sd_sig_rx_logging_function() and
 * bt_sd_sig_tx()
 *
 *    Rev 1.8   08 Nov 2000 13:12:08   amitj
 *
 *    Rev 1.7   03 Nov 2000 11:56:58   ryuen
 * Increased BT_SD_MAX_TEXT_STRING_LEN from 16 to 32
 *
 *    Rev 1.6   30 Oct 2000 18:10:34   ryuen
 * Added the support for device discovery
 *
 *    Rev 1.5   29 Sep 2000 17:39:16   waynelee
 * externed bt_sd_ev_l2_rx_data()
 *
 *    Rev 1.0   29 Sep 2000 17:32:36   waynelee
 * Initial revision.
 *
 *    Rev 1.4   12 Sep 2000 14:37:28   ryuen
 * Added bt_sd_online() and bt_sd_offline()
 *
 *    Rev 1.3   17 Aug 2000 13:47:16   propach
 * Added Event Control (EC).  Fixed minor problems in RFCOMM.
 *
 *    Rev 1.2   08 Aug 2000 10:50:48   ryuen
 * 1. Updated to 32-bit service record handle
 * 2. Added ev_cb_ptr to bt_sd_conn_entry_type
 *
 *    Rev 1.1   02 Aug 2000 13:04:18   ryuen
 * Added support for sending SDP Service Search Attribute Request
 *
 *    Rev 1.0   31 Jul 2000 11:55:02   ryuen
 * Initial revision.

===========================================================================*/

/*===========================================================================

                     INCLUDE FILES FOR MODULE

===========================================================================*/

#include "bt.h"
#include "btcmdi.h"
#include "dsm.h"


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

/* Bluetooth assigned UUIDs for Protocols */
#define BT_SD_PROTOCOL_SDP                                0x0001
#define BT_SD_PROTOCOL_UDP                                0x0002
#define BT_SD_PROTOCOL_RFCOMM                             0x0003
#define BT_SD_PROTOCOL_TCP                                0x0004
#define BT_SD_PROTOCOL_TCS_BIN                            0x0005
#define BT_SD_PROTOCOL_TCS_AT                             0x0006
#define BT_SD_PROTOCOL_OBEX                               0x0008
#define BT_SD_PROTOCOL_IP                                 0x0009
#define BT_SD_PROTOCOL_FTP                                0x000A
#define BT_SD_PROTOCOL_HTTP                               0x000C
#define BT_SD_PROTOCOL_WSP                                0x000E
#define BT_SD_PROTOCOL_BNEP                               0x000F
#define BT_SD_PROTOCOL_UPNP                               0x0010
#define BT_SD_PROTOCOL_HIDP                               0x0011
#define BT_SD_PROTOCOL_HARDCOPY_CONTROL_CHANNEL           0x0012
#define BT_SD_PROTOCOL_HARDCOPY_DATA_CHANNEL              0x0014
#define BT_SD_PROTOCOL_HARDCOPY_NOTIFICATION              0x0016
#define BT_SD_PROTOCOL_AVCTP                              0x0017
#define BT_SD_PROTOCOL_AVDTP                              0x0019
#define BT_SD_PROTOCOL_CMTP                               0x001B
#define BT_SD_PROTOCOL_UDI_C_PLANE                        0x001D
#define BT_SD_PROTOCOL_L2CAP                              0x0100

/* Bluetooth assigned UUIDs for Service Classes */
#define BT_SD_SERVICE_CLASS_SERVICE_DISCOVERY_SERVER                  0x1000
#define BT_SD_SERVICE_CLASS_BROWSE_GROUP_DESCRIPTOR                   0x1001
#define BT_SD_SERVICE_CLASS_PUBLIC_BROWSE_GROUP                       0x1002
#define BT_SD_SERVICE_CLASS_SERIAL_PORT                               0x1101
#define BT_SD_SERVICE_CLASS_LAN_ACCESS_PPP                            0x1102
#define BT_SD_SERVICE_CLASS_DIALUP_NETWORKING                         0x1103
#define BT_SD_SERVICE_CLASS_IR_MC_SYNC                                0x1104
#define BT_SD_SERVICE_CLASS_OBEX_OBJECT_PUSH                          0x1105
#define BT_SD_SERVICE_CLASS_OBEX_FILE_TRANSFER                        0x1106
#define BT_SD_SERVICE_CLASS_IR_MC_SYNC_COMMAND                        0x1107
#define BT_SD_SERVICE_CLASS_HEADSET                                   0x1108
#define BT_SD_SERVICE_CLASS_CORDLESS_TELEPHONY                        0x1109
#define BT_SD_SERVICE_CLASS_AUDIO_SOURCE                              0x110A
#define BT_SD_SERVICE_CLASS_AUDIO_SINK                                0x110B
#define BT_SD_SERVICE_CLASS_AV_REMOTE_CONTROL_TARGET                  0x110C
#define BT_SD_SERVICE_CLASS_ADVANCED_AUDIO_DISTRIBUTION               0x110D
#define BT_SD_SERVICE_CLASS_AV_REMOTE_CONTROL                         0x110E
#define BT_SD_SERVICE_CLASS_VIDEO_CONFERENCING                        0x110F
#define BT_SD_SERVICE_CLASS_INTERCOM                                  0x1110
#define BT_SD_SERVICE_CLASS_FAX                                       0x1111
#define BT_SD_SERVICE_CLASS_HEADSET_AUDIO_GATEWAY                     0x1112
#ifdef FEATURE_BT_LPOS
#error code not present
#else
#define BT_SD_SERVICE_CLASS_WAP                                       0x1113
#endif /* FEATURE_BT_LPOS */
#define BT_SD_SERVICE_CLASS_WAP_CLIENT                                0x1114
#define BT_SD_SERVICE_CLASS_PANU                                      0x1115
#define BT_SD_SERVICE_CLASS_NAP                                       0x1116
#define BT_SD_SERVICE_CLASS_GN                                        0x1117
#define BT_SD_SERVICE_CLASS_DIRECT_PRINTING                           0x1118
#define BT_SD_SERVICE_CLASS_REFERENCE_PRINTING                        0x1119
#define BT_SD_SERVICE_CLASS_IMAGING                                   0x111A
#define BT_SD_SERVICE_CLASS_IMAGING_RESPONDER                         0x111B
#define BT_SD_SERVICE_CLASS_IMAGING_AUTOMATIC_ARCHIVE                 0x111C
#define BT_SD_SERVICE_CLASS_IMAGING_REFERENCED_OBJECTS                0x111D
#define BT_SD_SERVICE_CLASS_HANDSFREE                                 0x111E
#define BT_SD_SERVICE_CLASS_HANDSFREE_AUDIO_GATEWAY                   0x111F
#define BT_SD_SERVICE_CLASS_DIRECT_PRINTING_REFERENCE_OBJECTS_SERVICE 0x1120
#define BT_SD_SERVICE_CLASS_REFLECTED_UI                              0x1121
#define BT_SD_SERVICE_CLASS_BASIC_PRINTING                            0x1122
#define BT_SD_SERVICE_CLASS_PRINTING_STATUS                           0x1123
#define BT_SD_SERVICE_CLASS_HUMAN_INTERFACE_DEVICE_SERVICE            0x1124
#define BT_SD_SERVICE_CLASS_HARDCOPY_CABLE_REPLACEMENT                0x1125
#define BT_SD_SERVICE_CLASS_HCR_PRINT                                 0x1126
#define BT_SD_SERVICE_CLASS_HCR_SCAN                                  0x1127
#define BT_SD_SERVICE_CLASS_COMMON_ISDN_ACCESS                        0x1128
#define BT_SD_SERVICE_CLASS_VIDEO_CONFERENCING_GW                     0x1129
#define BT_SD_SERVICE_CLASS_UDI_MT                                    0x112A
#define BT_SD_SERVICE_CLASS_UDI_TA                                    0x112B
#define BT_SD_SERVICE_CLASS_AUDIO_VIDEO                               0x112C
#define BT_SD_SERVICE_CLASS_SIM_ACCESS                                0x112D
#define BT_SD_SERVICE_CLASS_PBAP_PCE                                  0x112E
#define BT_SD_SERVICE_CLASS_PBAP_PSE                                  0x112F
#define BT_SD_SERVICE_CLASS_PBAP                                      0x1130
#define BT_SD_SERVICE_CLASS_PNP_INFORMATION                           0x1200
#define BT_SD_SERVICE_CLASS_GENERIC_NETWORKING                        0x1201
#define BT_SD_SERVICE_CLASS_GENERIC_FILE_TRANSFER                     0x1202
#define BT_SD_SERVICE_CLASS_GENERIC_AUDIO                             0x1203
#define BT_SD_SERVICE_CLASS_GENERIC_TELEPHONY                         0x1204
#define BT_SD_SERVICE_CLASS_UPNP_SERVICE                              0x1205
#define BT_SD_SERVICE_CLASS_UPNP_IP_SERVICE                           0x1206
#define BT_SD_SERVICE_CLASS_ESDP_UPNP_IP_PAN                          0x1300
#define BT_SD_SERVICE_CLASS_ESDP_UPNP_IP_LAP                          0x1301
#define BT_SD_SERVICE_CLASS_ESDP_UPNP_L2CAP                           0x1302
#define BT_SD_SERVICE_CLASS_VIDEO_SOURCE                              0x1303
#define BT_SD_SERVICE_CLASS_VIDEO_SINK                                0x1304
#define BT_SD_SERVICE_CLASS_VIDEO_DISTRIBUTION                        0x1305

#define BT_SD_NULL_UUID128                { \
                                0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0 }
#define BT_SD_SERVICE_CLASS_SYNCML_SERVER { \
              0, 0, 0, 0x01, 0, 0, 0x10, 0, 0x80, 0, 0, 0x02, 0xEE, 0, 0, 0x02 }
#define BT_SD_SERVICE_CLASS_SYNCML_CLIENT { \
              0, 0, 0, 0x02, 0, 0, 0x10, 0, 0x80, 0, 0, 0x02, 0xEE, 0, 0, 0x02 }
#define BT_SD_DB_NOT_SYNCML_V     0x00
#define BT_SD_DB_SYNCML_M         0x03
#define BT_SD_DB_SYNCML_CLIENT_B  0x01 
#define BT_SD_DB_SYNCML_SERVER_B  0x02

/* Bluetooth assigned numbers for Attribute ID */
#define BT_SD_ATTR_ID_SERVICE_RECORD_HANDLE                0x0000
#define BT_SD_ATTR_ID_SERVICE_CLASS_ID_LIST                0x0001
#define BT_SD_ATTR_ID_SERVICE_RECORD_STATE                 0x0002
#define BT_SD_ATTR_ID_SERVICE_SERVICE_ID                   0x0003
#define BT_SD_ATTR_ID_PROTOCOL_DESCRIPTOR_LIST             0x0004
#define BT_SD_ATTR_ID_BROWSE_GROUP_LIST                    0x0005
#define BT_SD_ATTR_ID_LANGUAGE_BASE_ATTRIBUTE_ID_LIST      0x0006
#define BT_SD_ATTR_ID_SERVICE_INFO_TIME_TO_LIVE            0x0007
#define BT_SD_ATTR_ID_SERVICE_AVAILABILITY                 0x0008
#define BT_SD_ATTR_ID_BLUETOOTH_PROFILE_DESCRIPTOR_LIST    0x0009
#define BT_SD_ATTR_ID_DOCUMENTATION_URL                    0x000A
#define BT_SD_ATTR_ID_CLIENT_EXECUTABLE_URL                0x000B
#define BT_SD_ATTR_ID_ICON_URL                             0x000C
#define BT_SD_ATTR_ID_ADDITIONAL_PROTOCOL_DESCRIPTOR_LISTS 0x000D
#define BT_SD_ATTR_ID_SERVICE_NAME                         0x0100
#define BT_SD_ATTR_ID_SERVICE_DESCRIPTION                  0x0101
#define BT_SD_ATTR_ID_PROVIDER_NAME                        0x0102
#define BT_SD_ATTR_ID_VERSION_NUMBER_LIST                  0x0200
#define BT_SD_ATTR_ID_GROUP_ID                             0x0200
#define BT_SD_ATTR_ID_SERVICE_DATABASE_STATE               0x0201
/* HID */
#define BT_SD_ATTR_ID_HID_DEV_REL_NUM                      0x0200
#define BT_SD_ATTR_ID_HID_PARSER_VER                       0x0201
#define BT_SD_ATTR_ID_HID_DEV_SUB_CLASS                    0x0202
#define BT_SD_ATTR_ID_HID_COUNTRY_CODE                     0x0203
#define BT_SD_ATTR_ID_HID_VIRTUAL_CABLE                    0x0204
#define BT_SD_ATTR_ID_HID_RECONNECT_INITIATE               0x0205
#define BT_SD_ATTR_ID_HID_DESCRIPTOR_LIST                  0x0206
#define BT_SD_ATTR_ID_HID_SDP_DISABLE                      0x0208
#define BT_SD_ATTR_ID_HID_BAT_POWER                        0x0209
#define BT_SD_ATTR_ID_HID_REMOTE_WAKE                      0x020A
#define BT_SD_ATTR_ID_HID_PROFILE_VERSION                  0x020B
#define BT_SD_ATTR_ID_HID_SUP_TIMEOUT                      0x020C
#define BT_SD_ATTR_ID_HID_NORM_CONNECT                     0x020D
#define BT_SD_ATTR_ID_HID_BOOT_DEVICE                      0x020E

#define BT_SD_ATTR_ID_SERVICE_VERSION                      0x0300
#define BT_SD_ATTR_ID_NETWORK                              0x0301

/* Cordless Telephony Profile */
#define BT_SD_ATTR_ID_EXTERNAL_NETWORK                     0x0301

/* Synchronization Profile */
#define BT_SD_ATTR_ID_SUPPORTED_DATA_STORES_LIST           0x0301

/* Generic Access Profile */
#define BT_SD_ATTR_ID_REMOTE_AUDIO_VOLUME_CONTROL          0x0302

/* Object Push Profile */
#define BT_SD_ATTR_ID_SUPPORTED_FORMATS_LIST               0x0303

/* Fax Profile */
#define BT_SD_ATTR_ID_FAX_CLASS_1_SUPPORT                  0x0302
#define BT_SD_ATTR_ID_FAX_CLASS_2_0_SUPPORT                0x0303
#define BT_SD_ATTR_ID_FAX_CLASS_2_SUPPORT                  0x0304
#define BT_SD_ATTR_ID_AUDIO_FEEDBACK_SUPPORT               0x0305

/* BIP */
#define BT_SD_ATTR_ID_SUPPORTED_CAPABILITIES               0x0310

/* BIP and HFP and AVRCP and A2DP */
#define BT_SD_ATTR_ID_SUPPORTED_FEATURES                   0x0311

/* BIP */
#define BT_SD_ATTR_ID_SUPPORTED_FUNCTIONS                  0x0312
#define BT_SD_ATTR_ID_TOTAL_IMAGING_DATA_CAPACITY          0x0313

/* PBAP */
#define BT_SD_ATTR_ID_SUPPORTED_REPOSITORIES               0x0314

/* BPP printer specific attributes */
#define BT_SD_ATTR_ID_DOCUMENT_FORMATS_SUPPORTED           0x0350
#define BT_SD_ATTR_ID_CHARACTER_REPERTOIRES_SUPPORTED      0x0352
#define BT_SD_ATTR_ID_XHTML_PRINT_IMAGE_FORMATS_SUPPORTED  0x0354
#define BT_SD_ATTR_ID_COLOR_SUPPORTED                      0x0356
#define BT_SD_ATTR_ID_1284ID                               0x0358
#define BT_SD_ATTR_ID_PRINTER_NAME                         0x035A
#define BT_SD_ATTR_ID_PRINTER_LOCATION                     0x035C
#define BT_SD_ATTR_ID_DUPLEX_SUPPORTED                     0x035E
#define BT_SD_ATTR_ID_MEDIA_TYPES_SUPPORTED                0x0360
#define BT_SD_ATTR_ID_MAX_MEDIA_WIDTH                      0x0362
#define BT_SD_ATTR_ID_MAX_MEDIA_LENGTH                     0x0364
#define BT_SD_ATTR_ID_ENHANCED_LAYOUT_SUPPORTED            0x0366
#define BT_SD_ATTR_ID_RUI_FORMATS_SUPPORTED                0x0368
#define BT_SD_ATTR_ID_REFERENCE_PRINTING_RUI_SUPPORTED     0x0370
#define BT_SD_ATTR_ID_DIRECT_PRINTING_RUI_SUPPORTED        0x0372
#define BT_SD_ATTR_ID_REFERENCE_PRINTING_TOP_URL           0x0374
#define BT_SD_ATTR_ID_DIRECT_PRINTING_TOP_URL              0x0376
#define BT_SD_ATTR_ID_DEVICE_NAME                          0x037A

#ifdef FEATURE_BT_LPOS
#error code not present
#endif /* FEATURE_BT_LPOS */

/* Arbitrary-chosen numbers to limit the scope of SDP */
#define BT_SD_MAX_NUM_OF_UUID                                  4
#define BT_SD_MAX_NUM_OF_PROTO_DESC                            4
#define BT_SD_MAX_NUM_OF_ADD_PROTO_DESC_LISTS                  1
#define BT_SD_MAX_NUM_OF_PROTO_PARAM                           1
#define BT_SD_MAX_NUM_OF_LANG_BASE_REC                         1
#define BT_SD_MAX_NUM_OF_SRV_ATTR                             12
#define BT_SD_MAX_NUM_OF_L2CAP_CONNECTION  BT_MAX_DEVICES_CONNECTED
#define BT_SD_MAX_TEXT_STRING_LEN                             32
#define BT_SD_MAX_PDU_LEN                                     64
#define BT_SD_MAX_CONTINUATION_STATE_INFO_LEN                 16
#define BT_SD_MAX_NUM_OF_DISCOVERED_DEVICE                    30
#define BT_SD_MAX_NUM_OF_HID_CLASS_DESC                        1
#define BT_SD_MAX_HID_CLASS_DESC_STRING_LEN                   55
#define BT_SD_MAX_HID_CLASS_HOST_DESC_STRING_LEN              500
#define BT_SD_MAX_HID_HOST_DESC_ATTR                          2

#define BT_SD_REQ_TIMEOUT_MS                               10000 /* 10 secs */
#define BT_SD_IDLE_TIMEOUT_MS                              10000 /* 10 secs */

#define BT_SD_DEFAULT_MAX_ATTR_BYTE_COUNT                   0x30

#define BT_SD_MAX_SAME_CONTINUATION_HOPS_ALLOWED            25

#define BT_SD_MAX_SEARCH_RETRIES                            2

/* Inquiry length (the max and min are spec defined) - Vol2 PartE 7.1.1 */
#define BT_SD_MAX_SEARCH_DURATION         61
#define BT_SD_MIN_SEARCH_DURATION         1
#define BT_SD_DEFAULT_SEARCH_DURATION     10

typedef enum
{
  BT_SD_MAJOR_DEVICE_CLASS_MISC                 = 0x0000,
  BT_SD_MAJOR_DEVICE_CLASS_COMPUTER             = 0x0100,
  BT_SD_MAJOR_DEVICE_CLASS_PHONE                = 0x0200,
  BT_SD_MAJOR_DEVICE_CLASS_LAN_ACCESS_POINT     = 0x0300,
  BT_SD_MAJOR_DEVICE_CLASS_AUDIO                = 0x0400,
  BT_SD_MAJOR_DEVICE_CLASS_PERIPHERAL           = 0x0500,
  BT_SD_MAJOR_DEVICE_CLASS_IMAGING              = 0x0600,
  BT_SD_MAJOR_DEVICE_CLASS_UNCLASSIFIED         = 0x1F00
} bt_sd_major_device_class_enum_type;

typedef enum
{
  BT_SD_MINOR_DEVICE_CLASS_UNCLASSIFIED         = 0x0000,

  BT_SD_MINOR_DEVICE_CLASS_DESKTOP_WORKSTATION  = BT_SD_MAJOR_DEVICE_CLASS_COMPUTER + 1,
  BT_SD_MINOR_DEVICE_CLASS_SERVER_CLASS_COMPUTER,
  BT_SD_MINOR_DEVICE_CLASS_LAPTOP,
  BT_SD_MINOR_DEVICE_CLASS_HANDHELD_PC_OR_PDA,
  BT_SD_MINOR_DEVICE_CLASS_PALM_SIZED_PC_OR_PDA,
  BT_SD_MINOR_DEVICE_CLASS_WEARABLE_COMPUTER,

  BT_SD_MINOR_DEVICE_CLASS_CELLULAR             = BT_SD_MAJOR_DEVICE_CLASS_PHONE + 1,
  BT_SD_MINOR_DEVICE_CLASS_CORDLESS,
  BT_SD_MINOR_DEVICE_CLASS_SMART_PHONE,
  BT_SD_MINOR_DEVICE_CLASS_WIRED_MODEM_OR_VOICE_GATEWAY,
  BT_SD_MINOR_DEVICE_CLASS_COMMON_ISDN_ACCESS,

  BT_SD_MINOR_DEVICE_CLASS_FULLY_AVAILABLE      = BT_SD_MAJOR_DEVICE_CLASS_LAN_ACCESS_POINT,
  BT_SD_MINOR_DEVICE_CLASS_1_TO_17_PERCENT_UTILIZED,
  BT_SD_MINOR_DEVICE_CLASS_17_TO_33_PERCENT_UTILIZED,
  BT_SD_MINOR_DEVICE_CLASS_33_TO_50_PERCENT_UTILIZED,
  BT_SD_MINOR_DEVICE_CLASS_50_to_67_PERCENT_UTILIZED,
  BT_SD_MINOR_DEVICE_CLASS_67_TO_83_PERCENT_UTILIZED,
  BT_SD_MINOR_DEVICE_CLASS_83_TO_99_PERCENT_UTILIZED,
  BT_SD_MINOR_DEVICE_CLASS_NO_SERVICE_AVAILABLE,

  BT_SD_MINOR_DEVICE_CLASS_HEADSET_PROFILE      = BT_SD_MAJOR_DEVICE_CLASS_AUDIO + 1,
  BT_SD_MINOR_DEVICE_CLASS_HANDSFREE,

  BT_SD_MINOR_DEVICE_CLASS_JOYSTICK             = BT_SD_MAJOR_DEVICE_CLASS_PERIPHERAL + 1,
  BT_SD_MINOR_DEVICE_CLASS_GAMEPAD,
  BT_SD_MINOR_DEVICE_CLASS_REMOTE_CONTROL,
  BT_SD_MINOR_DEVICE_CLASS_SENSING_DEVICE,
  BT_SD_MINOR_DEVICE_CLASS_DIGITIZER_TABLET,
  BT_SD_MINOR_DEVICE_CLASS_SIM_CARD_READER

} bt_sd_minor_device_class_enum_type;

typedef enum
{
  /* 
   * these bit masks should ONLY be used when working on byte 2 of the class of 
   * device field, namely ...class_of_device.cod_bytes[2]
   */
  BT_SD_MAJOR_SERVICE_CLASS_POSITIONING     = 0x01,
  BT_SD_MAJOR_SERVICE_CLASS_NETWORKING      = 0x02,
  BT_SD_MAJOR_SERVICE_CLASS_RENDERING       = 0x04,
  BT_SD_MAJOR_SERVICE_CLASS_CAPTURING       = 0x08,
  BT_SD_MAJOR_SERVICE_CLASS_OBJECT_TRANSFER = 0x10,
  BT_SD_MAJOR_SERVICE_CLASS_AUDIO           = 0x20,
  BT_SD_MAJOR_SERVICE_CLASS_TELEPHONY       = 0x40,
  BT_SD_MAJOR_SERVICE_CLASS_INFORMATION     = 0x80 
} bt_sd_major_service_class_enum_type;

#define BT_SD_COD_2_LIM_DISCV( class_of_device ) (   \
          (class_of_device.cod_bytes[ 1 ] & 0x20) ?  \
             TRUE : FALSE)

#define BT_SD_WR_COD_LIM_DISCV_ON( class_of_device ) (  \
          (class_of_device.cod_bytes[ 1 ] |= 0x20) ) 

#define BT_SD_WR_COD_LIM_DISCV_OFF( class_of_device ) (  \
          (class_of_device.cod_bytes[ 1 ] &= ~0x20) )
                  
#define BT_SD_UUID128_EQUAL( u1, u2 ) (      \
          (memcmp( (byte*)(u1), (byte*)(u2), \
                    sizeof( bt_sd_uuid128_type) ) == 0) ? TRUE : FALSE )
                    
#ifdef FEATURE_BT_EXTPF
#define BT_SD_IS_OBEX_SERVICE( uuid ) ( \
          ( uuid == BT_SD_SERVICE_CLASS_OBEX_OBJECT_PUSH ) || \
          ( uuid == BT_SD_SERVICE_CLASS_OBEX_FILE_TRANSFER ) || \
          ( uuid == BT_SD_SERVICE_CLASS_IMAGING_RESPONDER ) || \
          ( uuid == BT_SD_SERVICE_CLASS_IMAGING_AUTOMATIC_ARCHIVE ) || \
          ( uuid == BT_SD_SERVICE_CLASS_IMAGING_REFERENCED_OBJECTS ) || \
          ( uuid == BT_SD_SERVICE_CLASS_DIRECT_PRINTING_REFERENCE_OBJECTS_SERVICE ) )
#endif /* FEATURE_BT_EXTPF */

/* Enumeration of all the SDP OPP supported format types */
typedef enum
{
  BT_SD_OPP_SUPPORTED_FORMAT_VCARD_2_1 = 0x01,
  BT_SD_OPP_SUPPORTED_FORMAT_VCARD_3_0 = 0x02,
  BT_SD_OPP_SUPPORTED_FORMAT_VCAL_1_0  = 0x03,
  BT_SD_OPP_SUPPORTED_FORMAT_ICAL_2_0  = 0x04,
  BT_SD_OPP_SUPPORTED_FORMAT_VNOTE     = 0x05,
  BT_SD_OPP_SUPPORTED_FORMAT_VMESSAGE  = 0x06,
  BT_SD_OPP_SUPPORTED_FORMAT_ANY       = 0xFF
} bt_opp_supported_format_type;

/* Enumeration for all the SDP PDU types */
typedef enum
{
  BT_SD_PDU_ERROR_RESP                      = 0x01,
  BT_SD_PDU_SERVICE_SEARCH_REQ              = 0x02,
  BT_SD_PDU_SERVICE_SEARCH_RESP             = 0x03,
  BT_SD_PDU_SERVICE_ATTRIBUTE_REQ           = 0x04,
  BT_SD_PDU_SERVICE_ATTRIBUTE_RESP          = 0x05,
  BT_SD_PDU_SERVICE_SEARCH_ATTRIBUTE_REQ    = 0x06,
  BT_SD_PDU_SERVICE_SEARCH_ATTRIBUTE_RESP   = 0x07
} bt_sd_pdu_type;

/* Enumeration for all the data element type descriptors */
typedef enum
{
  BT_SD_DATA_ELEMENT_NULL         = 0,     /* Nil                          */
  BT_SD_DATA_ELEMENT_UNSIGNED_INT = 1,     /* Unsigned integer             */
  BT_SD_DATA_ELEMENT_SIGNED_INT   = 2,     /* Signed 2s-complement integer */
  BT_SD_DATA_ELEMENT_UUID         = 3,     /* UUID                         */
  BT_SD_DATA_ELEMENT_TEXT_STRING  = 4,     /* Text string                  */
  BT_SD_DATA_ELEMENT_BOOL         = 5,     /* Boolean                      */
  BT_SD_DATA_ELEMENT_SEQUENCE     = 6,     /* Data element sequence        */
  BT_SD_DATA_ELEMENT_ALTERNATIVE  = 7,     /* Data element alternative     */
  BT_SD_DATA_ELEMENT_URL          = 8      /* Uniform resource locator     */
} bt_sd_data_element_enum_type;

/* Enumeration for all the attribute types */
typedef enum
{
  BT_SD_ATTR_TYPE_UNKNOWN,
  BT_SD_ATTR_TYPE_UUID_LIST,
  BT_SD_ATTR_TYPE_PROTO_DESC_LIST,
  BT_SD_ATTR_TYPE_LANG_BASE_ATTR_ID_LIST,
  BT_SD_ATTR_TYPE_UINT_LIST,
  BT_SD_ATTR_TYPE_STRING,
  BT_SD_ATTR_TYPE_UINT8,
  BT_SD_ATTR_TYPE_UINT16,
  BT_SD_ATTR_TYPE_UINT32,
  BT_SD_ATTR_TYPE_UINT64,
  BT_SD_ATTR_TYPE_BOOL,
  BT_SD_ATTR_TYPE_UUID,
  BT_SD_ATTR_TYPE_UUID128,
  BT_SD_ATTR_TYPE_ADD_PROTO_DESC_LISTS,
  BT_SD_ATTR_TYPE_UINT128,
  BT_SD_ATTR_TYPE_HID_DESC_LIST
} bt_sd_srv_attr_enum_type;

typedef enum
{
  BT_SD_CONN_STATE_DISCONNECTED = 0,
  BT_SD_CONN_STATE_CONNECTING,
  BT_SD_CONN_STATE_CONNECTED,
  BT_SD_CONN_STATE_DISCONNECTING
} bt_sd_conn_state_type;

/* Definition for a PDU header */
typedef BT_PACKED struct
{
  uint8    pid;                                   /* Protocol Data Unit ID */
  uint16   tid;                                   /* Transaction ID        */
  uint16   length;                                /* Parameter length      */
} bt_sd_pdu_hdr_type;

/* Defintion for a data element header */
typedef struct
{
  bt_sd_data_element_enum_type   type;
  uint8                          size_index;
  uint8                          hdr_len;
  uint8                          attr_value_len;
} bt_sd_data_element_header_type;

/* Definition for a Data Element Sequence of unsigned integers
   (uint8/uint16/uint32/uint64) */
typedef struct
{
  uint8                          num_val;
  uint8                          size;
  uint64                         val[ BT_SD_MAX_NUM_OF_UINT ];
} bt_sd_uint_list_type;

/* Definition for a Protocol Descriptor */
typedef struct
{
  bt_sd_data_element_header_type header;
  bt_sd_uuid_type                uuid; /* UUID identifying the protocol */
  boolean                        is_uuid128;
  bt_sd_uuid128_type             uuid128;
  uint16                         num_param;
  struct
  {
    uint32                       value;
    uint8                        size;
  } param[ BT_SD_MAX_NUM_OF_PROTO_PARAM ];   /* Protocol-specific parameters */

  void*                          srv_attr_ptr;
                                   /* Pointer to the enclosing service
                                      attribute record */
} bt_sd_proto_desc_type;

/* Definition for ProtocolDescriptorList */
typedef struct
{
  bt_sd_data_element_header_type header;
  uint16                         num_proto_desc;
  bt_sd_proto_desc_type          proto_desc[ BT_SD_MAX_NUM_OF_PROTO_DESC ];
} bt_sd_proto_desc_list_type;

typedef struct
{
  uint16                       lang_id;
  uint16                       char_enc_id;
  uint16                       base_attr_id;
} bt_sd_lang_base_rec_type;

/* Definition for LanguageBaseAttributeIDList */
typedef struct
{
  uint16                        num_lang_base_rec;
  bt_sd_lang_base_rec_type      lang_base_rec[ BT_SD_MAX_NUM_OF_LANG_BASE_REC ];
} bt_sd_lang_base_attr_id_list_type;

/* Definition for HID Descriptor List */
typedef struct
{
  bt_sd_data_element_header_type  header;
  uint8   val;
  uint8   len;
  byte    str[ BT_SD_MAX_HID_CLASS_DESC_STRING_LEN ];
} bt_sd_hid_class_desc_type;

typedef struct
{
  uint16                     num_hid_class_desc;
  bt_sd_hid_class_desc_type  hid_class_desc[ BT_SD_MAX_NUM_OF_HID_CLASS_DESC ];
} bt_sd_hid_desc_list_type;

typedef struct
{
  uint16   len;
  byte     str[ BT_SD_MAX_HID_CLASS_HOST_DESC_STRING_LEN ];
} bt_sd_hid_host_desc_list_type;

typedef struct
{
  uint8     valid;
  uint8     index ;
} bt_sd_hid_host_desc_type;

#ifdef CUST_EDITION
#define BT_SD_MAX_CUSTOM_ATTR_VALUE_LENGTH	( 255 + 2 )

typedef struct
{
  uint8			Data[ BT_SD_MAX_CUSTOM_ATTR_VALUE_LENGTH ];
} bt_sd_custom_attribute_value;
#endif

/* Definition for a SDP Service Attribute */
typedef struct
{
  bt_sd_data_element_header_type header;

  uint16                         attr_id;
  bt_sd_srv_attr_enum_type       attr_type;
  boolean                        matched_flag;

  union
  {
    bt_sd_uuid_list_type              uuid_list;
    bt_sd_proto_desc_list_type        proto_desc_list;
    bt_sd_lang_base_attr_id_list_type lang_base_attr_id_list;
    bt_sd_uint_list_type              uint_list;
    char                              str[ BT_SD_MAX_TEXT_STRING_LEN ];
    uint64                            primitive_value;
    boolean                           bool_flag;
    bt_sd_uuid128_type                uuid128;
    bt_sd_proto_desc_list_type        add_proto_desc_lists[ 
                                        BT_SD_MAX_NUM_OF_ADD_PROTO_DESC_LISTS ];
    byte                              uint128_value[ 16 ];
    bt_sd_hid_desc_list_type          hid_desc_list;
    bt_sd_hid_host_desc_type          hid_host_desc_list;
	#ifdef CUST_EDITION
	bt_sd_custom_attribute_value	  custom_attr_value;
	#endif
  } attr_value;

} bt_sd_srv_attr_rec_type;

/* Definition for a SDP Service Record */
typedef struct
{
  uint32                         srv_rec_id;
  boolean                        is_custom_srv;
  bt_sd_uuid_type                srv_uuid;
  bt_sd_uuid128_type             srv_uuid128;

  uint8                          syncml_flag;
  boolean                        used_flag;
  boolean                        enabled_flag;
  boolean                        connectable;
  boolean                        matched_flag;
  uint16                         matched_attr_byte_count;

  uint16                         num_srv_attr;
  bt_sd_srv_attr_rec_type        srv_attr[ BT_SD_MAX_NUM_OF_SRV_ATTR ];
} bt_sd_srv_rec_type;

/*
 * Definition for the SDP Database, which can be local or used as a
 * depository for results of a SDP Service Attribute / Service
 * Search Attribute Request
 */
typedef struct bt_sd_srv_db
{
  uint16                         num_used_srv_rec;
  uint16                         num_enabled_srv_rec;
  uint16                         num_connectable_srv_rec;
  uint16                         num_matched_srv_rec;
  bt_sd_srv_rec_type             srv_rec[ BT_SD_MAX_NUM_OF_SRV_REC ];
} bt_sd_srv_db_type;

/* Enumerations for iteration - iterate over all versus matched-only items */
typedef enum
{
  BT_SD_ITER_ALL,
  BT_SD_ITER_MATCH_ONLY
} bt_sd_iter_enum_type;

/* Definition for a service record iterator */
typedef struct
{
  void                           (*init)( bt_sd_db_enum_type     db_type,
                                          bt_sd_iter_enum_type   iter_type );
  const bt_sd_srv_rec_type*      (*first)( void );
  const bt_sd_srv_rec_type*      (*next)( void );
} bt_sd_srv_rec_iter_type;

/* Definition for a service attribute iterator */
typedef struct
{
  void                           (*init)(const bt_sd_srv_rec_type* srv_rec_ptr,
                                         bt_sd_iter_enum_type iter_type );
  const bt_sd_srv_attr_rec_type* (*first)( void );
  const bt_sd_srv_attr_rec_type* (*next)( void );
} bt_sd_srv_attr_iter_type;

/* Definition for a discovered device record */
typedef struct
{
  bt_bd_addr_type              bd_addr;
  uint16                       service_class;
  uint16                       major_device_class;
  uint16                       minor_device_class;
} bt_sd_device_discovery_rec_type;

/* Definition for the Device Discovery database */
typedef struct
{
  boolean                          dev_discv_in_progress;
  boolean                          remote_name_req_in_progress;
  bt_app_id_type                   app_id;
  bt_bd_addr_type                  bd_addr_2_filter;
  bt_bd_addr_type                  remote_name_bd_addr;
  uint16                           service_class_bitmap;
  uint16                           max_discvd_dev;
  uint16                           num_discvd_dev;
  bt_sd_device_discovery_rec_type  discvd_dev_info[
                                     BT_SD_MAX_NUM_OF_DISCOVERED_DEVICE ];
} bt_sd_device_discovery_db_type;

/* Definition for a SDP connection entry */
typedef struct
{
  boolean          used;
  bt_bd_addr_type  bd_addr;
  uint16           cid_loc_initd;  /*  For connection initiated by  */
  rex_timer_type   tmr_loc_initd;  /*    the local device.          */
  uint16           cid_rem_initd;  /*  For connection initiated by  */
  rex_timer_type   tmr_rem_initd;  /*    the remote device.         */
  uint16           out_mtu;
  uint16           tid;
  boolean          req_pending;
  bt_app_id_type   bt_app_id;
  dsm_item_type*   pending_req_dsm_ptr;
  dsm_item_type*   dup_req_dsm_ptr;
  dsm_item_type*   resp_dsm_ptr;
  uint16           num_resp_recv;
  bt_sd_conn_state_type  conn_state;
  uint8            cont_state_info[BT_SD_MAX_CONTINUATION_STATE_INFO_LEN];
  uint8            same_cont_state_count;  
  uint8            num_retries;
  boolean          generate_xml;
} bt_sd_conn_entry_type;

typedef struct
{
  bt_app_id_type                 app_id_user;
  bt_cmd_type                    app_cmd;
  uint16                         service_class;
} bt_sd_data_type;


/*-------------------------------------------------------------------------*/
/*                                                                         */
/*                        Global Data Declarations                         */
/*                                                                         */
/*-------------------------------------------------------------------------*/

extern bt_app_id_type  bt_sd_app_id;

extern const bt_sd_srv_rec_iter_type bt_sd_srv_rec_iter;
  /* SDP Service Record Iterator */
extern const bt_sd_srv_attr_iter_type bt_sd_srv_attr_iter;
  /* SDP Service Attribute Iterator */
extern const uint8 bt_sd_data_element_size_desc[];
  /* Lookup table based on size index */
extern bt_sd_conn_entry_type  bt_sd_conn_table[];
  /* L2CAP connection table */
extern bt_sd_device_discovery_db_type bt_sd_dev_discv_db;

extern bt_sd_data_type  bt_sd_data;

extern const bt_sd_uuid128_type bt_sd_syncml_server_uuid;
extern const bt_sd_uuid128_type bt_sd_syncml_client_uuid;
extern const bt_sd_uuid128_type bt_sd_null_uuid128;
extern bt_sd_hid_host_desc_list_type 
       bt_sd_host_desc_list[BT_SD_MAX_HID_HOST_DESC_ATTR];

/*-------------------------------------------------------------------------*/
/*                                                                         */
/*                           External Macros                               */
/*                                                                         */
/*-------------------------------------------------------------------------*/

#define SWAB16(x) \
  ( (uint16) ( \
             ( ( (uint16) (x) & (uint16) 0x00FFU ) << 8 ) | \
             ( ( (uint16) (x) & (uint16) 0xFF00U ) >> 8 ) ) )

#define SWAB32(x) \
  ( (uint32) ( \
             ( ( (uint32) (x) & (uint32) 0x000000FFUL ) << 24 ) | \
             ( ( (uint32) (x) & (uint32) 0x0000FF00UL ) << 8  ) | \
             ( ( (uint32) (x) & (uint32) 0x00FF0000UL ) >> 8  ) | \
             ( ( (uint32) (x) & (uint32) 0xFF000000UL ) >> 24 ) ) )

#define SWAB64(x) \
  ( (uint64) ( \
             ( ( (uint64) (x) & (uint64) 0x00000000000000FFULL ) << 56 ) | \
             ( ( (uint64) (x) & (uint64) 0x000000000000FF00ULL ) << 40 ) | \
             ( ( (uint64) (x) & (uint64) 0x0000000000FF0000ULL ) << 24 ) | \
             ( ( (uint64) (x) & (uint64) 0x00000000FF000000ULL ) <<  8 ) | \
             ( ( (uint64) (x) & (uint64) 0x000000FF00000000ULL ) >>  8 ) | \
             ( ( (uint64) (x) & (uint64) 0x0000FF0000000000ULL ) >> 24 ) | \
             ( ( (uint64) (x) & (uint64) 0x00FF000000000000ULL ) >> 40 ) | \
             ( ( (uint64) (x) & (uint64) 0xFF00000000000000ULL ) >> 56 ) ) )

/*-------------------------------------------------------------------------*/
/*                                                                         */
/*                    External Functions Prototypes                        */
/*                                                                         */
/*-------------------------------------------------------------------------*/

extern void bt_sd_powerup_init( void );

extern void bt_sd_qdsp_ready_init( void );

extern void bt_sd_qdsp_reset_shutdown( void );

extern void bt_sd_process_ev_queue( void );

extern uint16 bt_sd_get_max_event_bytes( void );

extern void bt_sd_process_event( bt_ev_msg_type*  ev_msg_ptr );

extern void bt_sd_process_command( bt_cmd_msg_type* cmd_msg_ptr );

extern boolean bt_sd_ev_l2_rx_data( bt_ev_l2_rx_data_type* l2_rxd_ptr );

extern bt_sd_srv_attr_enum_type bt_sd_get_attr_type( uint16 attr_id );

extern void bt_sd_send_pending_request(
  bt_sd_conn_entry_type*     conn_entry_ptr,
  uint16                     info_len,
  uint8*                     cont_state_info
);

extern void bt_sd_clear_pending_request(
  bt_sd_conn_entry_type*     conn_entry_ptr
);

extern void bt_sd_send_error_resp_event(
  bt_sd_conn_entry_type*     conn_entry_ptr,
  uint16                     ec
);

extern void bt_sd_send_timeout_resp_event(
  bt_sd_conn_entry_type*     conn_entry_ptr
);

extern void bt_sd_add_attribute_to_dsm_buffer(
  const bt_sd_srv_attr_rec_type*   srv_attr_ptr,
  dsm_item_type*                   dsm_ptr,
  dsm_mempool_id_enum_type         dsm_pool_id
);

extern bt_sd_conn_entry_type* bt_sd_find_conn_entry_by_bd_addr(
  const bt_bd_addr_type*     ba_ptr
);

extern bt_sd_conn_entry_type* bt_sd_find_conn_entry_by_cid(
  uint16                     cid
);

extern bt_sd_conn_entry_type* bt_sd_add_conn_entry(
  const bt_bd_addr_type*     ba_ptr
);

extern bt_sd_srv_rec_type* bt_sd_create_service_record(
  bt_sd_uuid_type            uuid,    /* Service Class UUID                 */
  uint8                      rfc_scn, /* RFCOMM Server Channel Number       */
  uint16                     l2_psm,  /* L2CAP Protocol/Service Multiplexer */
  uint16                     pvn,     /* Protocol Version Number            */
  const char*                name     /* Service Name                       */
);

extern bt_sd_srv_rec_type* bt_sd_create_custom_service_record(
  bt_sd_uuid_list_type*      uuid_list_ptr,  /* Service Class UUID List     */
  uint8                      rfc_scn, /* RFCOMM Server Channel Number       */
  uint16                     l2_psm,  /* L2CAP Protocol/Service Multiplexer */
  uint16                     pvn,     /* Protocol Version Number            */
  const char*                name     /* Service Name                       */
);

#ifdef FEATURE_BT_EXTPF_SML
extern bt_sd_srv_rec_type* bt_sd_create_syncml_service_record
(
  boolean          client,  /* SyncML client or server      */
  uint8            rfc_scn, /* RFCOMM Server Channel Number */
  const char*      name     /* Service Name                 */
);
#endif

#ifdef BT_SWDEV_2_1_EIR
extern void bt_sd_eir_update_rm_uuid(
  bt_uuid_type   uuid_type
);
#endif /* BT_SWDEV_2_1_EIR */

extern void bt_sd_enable_service_record(
  bt_sd_srv_rec_type*        srv_rec_ptr,
  boolean                    enabled_flag
);

extern void bt_sd_update_service_connectable(
  bt_sd_srv_rec_type*        srv_rec_ptr,
  boolean                    connectable
);

extern uint16 bt_sd_match_service_record(
  const bt_sd_srv_srch_pat_type*  srch_pat_ptr
);

extern uint32 bt_sd_match_service_attribute(
  bt_sd_srv_rec_type*        srv_rec_ptr,
  const bt_sd_srv_srch_pat_type*  srch_pat_ptr
);

extern void bt_sd_clear_service_record_match( void );

extern void bt_sd_clear_service_attribute_match(
  bt_sd_srv_rec_type*        srv_rec_ptr
);

extern bt_sd_srv_rec_type* bt_sd_find_service_record_by_handle(
  uint32                     srv_rec_handle
);

extern bt_sd_srv_rec_type* bt_sd_find_service_record_by_uuid(
  bt_sd_db_enum_type         db_type,
  bt_sd_uuid_type            uuid,
  uint8                      scn,
  uint16                     psm
);

extern bt_sd_srv_rec_type* bt_sd_find_service_record_by_uuid128(
  bt_sd_db_enum_type         db_type,
  const bt_sd_uuid128_type*  uuid128_ptr,
  uint8                      scn,
  uint16                     psm
);

#ifdef FEATURE_BT_EXTPF_SML
extern bt_sd_srv_rec_type* bt_sd_find_syncml_service_record(
  bt_sd_db_enum_type         db_type,
  boolean                    client
);
#endif

extern bt_sd_srv_attr_rec_type* bt_sd_find_service_attribute(
  bt_sd_srv_rec_type*        srv_rec_ptr,
  uint16                     srv_attr_id
);

extern uint16 bt_sd_get_num_service_record(
  bt_sd_db_enum_type         db_type
);

extern bt_sd_service_discoverable_mode_type bt_sd_get_service_discoverable_mode
( 
  void 
);

extern boolean bt_sd_any_local_services_enabled( void );

extern boolean bt_sd_any_local_services_connectable( void );

/*
 * Functions to add to the SDP database
 */
extern void bt_sd_initialize_database(
  bt_sd_db_enum_type         db_type
);

extern bt_sd_srv_rec_type* bt_sd_alloc_service_record(
  bt_sd_db_enum_type         db_type
);

extern void bt_sd_free_service_record(
  bt_sd_srv_rec_type*        srv_rec_ptr
);

extern void bt_sd_validate_service_record(
  bt_sd_srv_rec_type*        srv_rec_ptr
);

extern boolean bt_sd_service_record_init_attribute(
  bt_sd_srv_attr_rec_type*   srv_attr_ptr,
  uint16                     attr_id,
  bt_sd_srv_attr_enum_type   attr_type
);

extern bt_sd_srv_attr_rec_type* bt_sd_service_record_add_attribute(
  bt_sd_srv_rec_type*        srv_rec_ptr,
  uint16                     attr_id,
  bt_sd_srv_attr_enum_type   attr_type
);

extern void bt_sd_service_attribute_add_uuid(
  bt_sd_srv_attr_rec_type*   srv_attr_ptr,
  bt_sd_uuid_type            uuid
);

extern void bt_sd_service_attribute_set_uuid128(
  bt_sd_srv_attr_rec_type*   srv_attr_ptr,
  const bt_sd_uuid128_type*  uuid128_ptr
);

extern bt_sd_proto_desc_type* bt_sd_service_attribute_add_proto_desc(
  bt_sd_srv_attr_rec_type*   srv_attr_ptr,
  bt_sd_uuid_type            uuid,
  const bt_sd_uuid128_type*  uuid128_ptr
);

extern void bt_sd_service_attribute_proto_desc_add_param(
  bt_sd_proto_desc_type*     proto_desc_ptr,
  uint16                     param,
  uint8                      size /* in bytes */
);

extern void bt_sd_service_attribute_add_lang_base_attr_id(
  bt_sd_srv_attr_rec_type*   srv_attr_ptr,
  uint16                     lang_id,
  uint16                     char_enc_id,
  uint16                     base_attr_id
);

extern void bt_sd_service_attribute_uint_list_add_val(
  bt_sd_srv_attr_rec_type*   srv_attr_ptr,
  uint8                      size,
  uint64                     val
);

extern void bt_sd_service_attribute_set_str(
  bt_sd_srv_attr_rec_type*   srv_attr_ptr,
  const char*                str
);

extern void bt_sd_service_attribute_set_value(
  bt_sd_srv_attr_rec_type*   srv_attr_ptr,
  uint64                     value
);

extern void bt_sd_service_attribute_set_uint128(
  bt_sd_srv_attr_rec_type*   srv_attr_ptr,
  const byte*                uint128_ptr
);

extern void bt_sd_service_attribute_set_bool(
  bt_sd_srv_attr_rec_type*   srv_attr_ptr,
  boolean                    bool_flag
);

extern void bt_sd_service_attribute_add_hid_class_desc(
  bt_sd_srv_attr_rec_type*   srv_attr_ptr,
  uint8                      val,
  uint8                      len,
  const byte*                str
);

extern bt_sd_device_discovery_rec_type* bt_sd_find_discovered_device(
  bt_bd_addr_type*           bd_addr_ptr
);

extern void bt_sd_process_timer_expiration( void );

extern bt_sd_uuid_type bt_sd_convert_128_to_16_bit_uuid( 
  const bt_sd_uuid128_type*  uuid128_ptr
);

extern void bt_sd_convert_16_to_128_bit_uuid( 
  uint8*                     uuid128, 
  bt_sd_uuid_type            uuid16
);

/*===========================================================================

FUNCTION
  bt_sd_extract_srvc_cls

DESCRIPTION
  Extract the service class from the given service attribute.  

===========================================================================*/
extern bt_sd_uuid_type bt_sd_extract_srvc_cls( bt_sd_srv_attr_rec_type* );

/*===========================================================================

FUNCTION
  bt_sd_extract_scn

DESCRIPTION
  Extract the server channel number from the given service attribute.  

===========================================================================*/
extern uint8 bt_sd_extract_scn( bt_sd_srv_attr_rec_type* );

/*===========================================================================

FUNCTION
  bt_sd_extract_profile_version

DESCRIPTION
  Extract the profile version from the given service attribute.  

===========================================================================*/
extern uint16 bt_sd_extract_profile_version( bt_sd_srv_attr_rec_type* );

/*===========================================================================

FUNCTION
  bt_sd_parse_class_of_device

DESCRIPTION
  Extracts from the class of device its 3 components: service class, major
  device class, and minor device class.

===========================================================================*/
extern void bt_sd_parse_class_of_device( 
  const bt_cod_type* cod_ptr, 
  uint16*            svc_cls_ptr,
  uint16*            mjr_dev_cls_ptr,
  uint16*            mnr_dev_cls_ptr
);

/*===========================================================================

FUNCTION
  bt_sd_build_class_of_device

DESCRIPTION
  Builds the class of device from 3 components: service class, major
  device class, and minor device class.

===========================================================================*/
extern void bt_sd_build_class_of_device( 
  uint16       svc_cls,
  uint16       mjr_dev_cls,
  uint16       mnr_dev_cls,
  bt_cod_type* cod_ptr
);


bt_cmd_status_type bt_sd_check_connectable_setting( void );
boolean bt_sd_any_connections_active( void );

#ifdef BT_SWDEV_QXDM_LOG_PER
extern void bt_sd_snapshot_logging_function( void );
#endif

#ifdef BT_SWDEV_QXDM_LOG_MSG
extern void bt_sd_sig_tx_logging_function(
  bt_app_id_type  bt_app_id,
  uint16          cid,
  dsm_item_type   *dsm_ptr
);

extern void bt_sd_sig_rx_logging_function
(
  dsm_item_type   *dsm_ptr,
  word            cid
);


#endif /* BT_SWDEV_QXDM_LOG_MSG */

#endif /* FEATURE_BT */

#endif  /*  _BTSD_H  */

