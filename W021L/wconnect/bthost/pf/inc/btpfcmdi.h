#ifndef _BTPFCMDI_H
#define _BTPFCMDI_H

#ifdef FEATURE_BT
#ifdef FEATURE_BT_EXTPF 

/*====*====*====*====*====*====*====*====*====*====*====*====*====*====*====*

   B L U E T O O T H    E X T E R N A L    P R O F I L E    C O M M A N D
  
                        I N T E R N A L    H E A D E R

DESCRIPTION
              Copyright (c) 2004-2008 QUALCOMM Incorporated.               
                      All Rights Reserved.                                  
              Qualcomm Confidential and Proprietary 
*====*====*====*====*====*====*====*====*====*====*====*====*====*====*====*/

/*===========================================================================

                        EDIT HISTORY FOR MODULE

  This section contains comments describing changes made to the module.
  Notice that changes are listed in reverse chronological order.

$Header: //source/qcom/qct/wconnect/bthost/pf/rel/00.00.26/inc/btpfcmdi.h#1 $ 

 when        who  what, where, why
 ----------  ---  -----------------------------------------------------------
 2008-06-17   hs  Added the support for AVRCP 1.3 meta data feature
                  by adding AVRCP metadata command, cmd stuctures
                  declaration
 2008-06-16   pr  Added cmd to tear down connection from printer
                  to BPP server (object channel).
 2008-05-06   gs  Fixed typo in bt_pf_cmd_pbap_cli_abort_type struct.

*
*   #43         03 Mar 2008           GS
* Added support for Get/SetReport HID profile functions.
*
*   #42         07 Nov 2007           GS
* Added HID profile host role support.
*
*   #41         31 May 2007           DSN
* Featurization to allow remoting of apis for SAP.
*
*   #40         27 Mar 2007           JTL
* Added function to put queue item back on the free queue.
*
*   #39         28 Feb 2007           MH
* Added server disconnect response functionality.
*
*    #38        22 Feb 2007           GS
* Fixed critical lint errors in AVDTP and AVRCP.
*
*    #37        05 Feb 2007           GS
* Mainlined fix for mis-aligned access issue on ARM11 target.
*
*    #36        01 Dec 2006           MH
* Rearranged bt_pf_cmd_goep_srv_confirm_set_path_type packed structure members.
*
*    #35        26 Oct 2006           MH
* Modified bt_pf_cmd_goep_srv_confirm_set_path_type to support all headers
*
*    #34        26 Jul 2006           DM
* Changes to avdtp send data to add frame handle
* Changes to a2dp send data to add last_frame
*
*    #31-32     10 Jul 2006           JH
* Removed extra linefeeds introduced in previous checkin.
*
*    #30        23 Jun 2006           JH
* Modified GOEP command structures to support JSR82.
*
*    #29        30 May 2005           GS
* Added support for PBAP feature.
*
*    #28        16 May 2005           JTL
* Added FEATURE_BT_TIME_DEBUG to determine delays for A2DP command 
* processing. Not defined by default.
*
*    #27        16 Dec 2005           JTL
* Adding new command to immediately send (flush) buffers stored in A2DP.
*
*    #26        15 Nov 2005           JTL
* Adding new command to A2DP to allow AVS to register app ID with A2DP.
*
*    #25        08 Aug 2005           JTL
* Added status to AVRCP relpies.
*
*    #24        18 Jul 2005           GS
* Added support for BIP server force disconnect.
*
*    #23        05 Jul 2005           JTL
* Updated copyright date.
* Fixed type defn for AVDTP command.
*
*    #22        02 Jun 2005           JTL
* Change data type to one now defined by AVS.
*
*    #21        27 May 2005           JTL
* Adding command for fine grained control of A2DP.
*
*    #20        12 Apr 2005           JH
* Converted FTP file operation done and FTP client abort functions into queued 
*  commands.
*
*    #17-19     17 Mar 2005           JTL
* Added support for AV profiles.
*
*    #16        13 Feb 2005            PN
* SAP command codes must start from BT_CMD_EV_PF_SAP_BASE.
*
*    #15        22 Jan 2005            PN
* Added support for SIM Access profile.
*
*    #14        7 Jan 2005            JH
* Modified to avoid possible misaligned memory accesses.
*
*    #13       19 Oct 2004            JH
* Added num_returned_handles to bt_pf_cmd_bip_srv_get_list_rsp_type
*
*    #12       30 Aug 2004            JH
* Changed element service_uuid_str to srv_name_str in command structures for 
*  SyncML registration commands
*
*    #11        9 Aug 2004            JH
* Renamed bt_pf_cmd_sml_auth_srv_rsp_type and bt_pf_cmd_sml_auth_cli_rsp_type to
*  bt_pf_cmd_sml_auth_req_type and bt_pf_cmd_sml_auth_rsp_type respectively
*
*    #8        21 Jul 2004            JTL
* Added CTP/ICP interfaces
*
*    #7        24 May 2004            JH
* Added BIP server start print response to facilitate APIT testing.
*
*    #6        14 May 2004            JH
* Further command struct changes
*
*    #5        30 Apr 2004            JH
* Added SML support
*
*    #4        23 Apr 2004            JH
* Moved command ID enum from btpfi.h to here
* Server register and deregister commands are now queued, need corresponding
*  command structs
*
*    #3        14 Apr 2004            JH
* Added BIP and BPP support
* Added APIT command structures
* Modified command structures to contain character arrays instead of pointers

===========================================================================*/

#include "customer.h"
#include "comdef.h"
#include "dsm.h"
#include "queue.h"
#include "bt.h"
#include "btpf.h"
#include "btpfi.h"

#ifdef FEATURE_BT_EXTPF_AV
#include "audiosbcenc.h"
#endif /* FEATURE_BT_EXTPF_AV */

#ifdef FEATURE_BT_EXTPF_GOEP 
#include "oi_obexauth.h"
#include "oi_obexcli.h"
#include "oi_obexcommon.h"
#include "oi_obexspec.h"
#include "oi_obexsrv.h" 
#endif /* FEATURE_BT_EXTPF_GOEP */

#ifdef FEATURE_BT_TIME_DEBUG
#include "time_svc.h"
#endif /* FEATURE_BT_TIME_DEBUG */

#if defined(FEATURE_BT_EXTPF_GOEP) || defined(FEATURE_BT_EXTPF_AV)
/*-------------------------------------------------------------------------*/
/*  Bluetooth external profile command type enumeration.                   */
/*-------------------------------------------------------------------------*/
typedef enum
{
  BT_PF_CMD_NULL,
#ifdef FEATURE_BT_EXTPF_GOEP
  BT_PF_CMD_GOEP_CLI_AUTH_RSP = BT_CMD_EV_PF_GOEP_BASE,
  BT_PF_CMD_GOEP_CLI_CONNECT,
  BT_PF_CMD_GOEP_CLI_DISCONNECT,
  BT_PF_CMD_GOEP_CLI_PUT,
  BT_PF_CMD_GOEP_CLI_GET,
  BT_PF_CMD_GOEP_CLI_SET_PATH,  

  BT_PF_CMD_GOEP_SRV_ACCEPT,
  BT_PF_CMD_GOEP_SRV_CFM_SET_PATH,
  BT_PF_CMD_GOEP_SRV_AUTH_RSP,     
  BT_PF_CMD_GOEP_SRV_GET_RSP,
  BT_PF_CMD_GOEP_SRV_PUT_RSP,
  BT_PF_CMD_GOEP_SRV_DCN_RSP,
  BT_PF_CMD_GOEP_SRV_FORCE_DCN,
#endif /* FEATURE_BT_EXTPF_GOEP */

#ifdef FEATURE_BT_EXTPF_OPP
  BT_PF_CMD_OPP_CLI_CONNECT = BT_CMD_EV_PF_OPP_BASE,
  BT_PF_CMD_OPP_CLI_DISCONNECT,
  BT_PF_CMD_OPP_CLI_PUSH,
  BT_PF_CMD_OPP_CLI_PULL,

  BT_PF_CMD_OPP_SRV_REGISTER,
  BT_PF_CMD_OPP_SRV_DEREGISTER,
  BT_PF_CMD_OPP_SRV_ACCEPT,
  BT_PF_CMD_OPP_SRV_FORCE_DCN,

  BT_PF_CMD_OPP_CLI_OPEN_READ_DONE,
  BT_PF_CMD_OPP_CLI_OPEN_WRITE_DONE,
  BT_PF_CMD_OPP_CLI_READ_DONE,
  BT_PF_CMD_OPP_CLI_WRITE_DONE,
  BT_PF_CMD_OPP_CLI_ABORT,
                               
  BT_PF_CMD_OPP_SRV_OPEN_READ_DONE,
  BT_PF_CMD_OPP_SRV_OPEN_WRITE_DONE,
  BT_PF_CMD_OPP_SRV_READ_DONE,
  BT_PF_CMD_OPP_SRV_WRITE_DONE,

#ifdef FEATURE_BT_TEST_API
#error code not present
#endif /* FEATURE_BT_TEST_API */
#endif /* FEATURE_BT_EXTPF_OPP */                                                                 

#ifdef FEATURE_BT_EXTPF_FTP
  BT_PF_CMD_FTP_CLI_CONNECT = BT_CMD_EV_PF_FTP_BASE,
  BT_PF_CMD_FTP_CLI_DISCONNECT,
  BT_PF_CMD_FTP_CLI_AUTH_RSP,
  BT_PF_CMD_FTP_CLI_PUT,
  BT_PF_CMD_FTP_CLI_DELETE,
  BT_PF_CMD_FTP_CLI_GET,
  BT_PF_CMD_FTP_CLI_SET_PATH,                     
  BT_PF_CMD_FTP_CLI_NEW_SUBFOLDER,
  BT_PF_CMD_FTP_CLI_GET_FOLDER_LISTING,
  BT_PF_CMD_FTP_CLI_TREE_PUT,
  BT_PF_CMD_FTP_CLI_TREE_GET,

  BT_PF_CMD_FTP_SRV_REGISTER,
  BT_PF_CMD_FTP_SRV_DEREGISTER,
  BT_PF_CMD_FTP_SRV_AUTH_RSP,
  BT_PF_CMD_FTP_SRV_ACCEPT,
  BT_PF_CMD_FTP_SRV_FORCE_DCN,
                          
  BT_PF_CMD_FTP_CLI_BROWSE_DONE,
  BT_PF_CMD_FTP_CLI_OPEN_DONE,
  BT_PF_CMD_FTP_CLI_READ_DONE,
  BT_PF_CMD_FTP_CLI_WRITE_DONE,
  BT_PF_CMD_FTP_CLI_SET_FOLDER_DONE,
  BT_PF_CMD_FTP_CLI_ABORT,

  BT_PF_CMD_FTP_SRV_BROWSE_DONE,
  BT_PF_CMD_FTP_SRV_OPEN_DONE,
  BT_PF_CMD_FTP_SRV_READ_DONE,
  BT_PF_CMD_FTP_SRV_WRITE_DONE,
  BT_PF_CMD_FTP_SRV_SET_FOLDER_DONE,
  BT_PF_CMD_FTP_SRV_DELETE_DONE,

#ifdef FEATURE_BT_TEST_API
#error code not present
#endif /* FEATURE_BT_TEST_API */
#endif /* FEATURE_BT_EXTPF_FTP */

#ifdef FEATURE_BT_EXTPF_BIP
  BT_PF_CMD_BIP_CLI_CONNECT = BT_CMD_EV_PF_BIP_BASE,
  BT_PF_CMD_BIP_CLI_DISCONNECT,
  BT_PF_CMD_BIP_CLI_AUTH_RSP,
  BT_PF_CMD_BIP_CLI_GET_CAPABILITIES,
  BT_PF_CMD_BIP_CLI_PUT_IMAGE,
  BT_PF_CMD_BIP_CLI_PUT_LINKED_THUMBNAIL,
  BT_PF_CMD_BIP_CLI_PUT_LINKED_ATTACHMENT,
  BT_PF_CMD_BIP_CLI_GET_IMAGES_LIST,
  BT_PF_CMD_BIP_CLI_GET_IMAGE_PROPERTIES,
  BT_PF_CMD_BIP_CLI_GET_IMAGE,
  BT_PF_CMD_BIP_CLI_GET_LINKED_THUMBNAIL,
  BT_PF_CMD_BIP_CLI_GET_LINKED_ATTACHMENT,
  BT_PF_CMD_BIP_CLI_DELETE_IMAGE,
  BT_PF_CMD_BIP_CLI_GET_STATUS,
  BT_PF_CMD_BIP_CLI_GET_PARTIAL_IMAGE,
  BT_PF_CMD_BIP_CLI_START_PRINT,
  BT_PF_CMD_BIP_CLI_START_ARCHIVE,
  BT_PF_CMD_BIP_CLI_REMOTE_DISPLAY,
  BT_PF_CMD_BIP_CLI_GET_MONITORING_IMAGE,
                           
  BT_PF_CMD_BIP_SRV_REG_IMAGING_RERSPONDER,
  BT_PF_CMD_BIP_SRV_REG_REFERENCED_OBJECTS,
  BT_PF_CMD_BIP_SRV_REG_ARCHIVED_OBJECTS,
  BT_PF_CMD_BIP_SRV_DEREG_IMAGING_RERSPONDER,
  BT_PF_CMD_BIP_SRV_DEREG_REFERENCED_OBJECTS,
  BT_PF_CMD_BIP_SRV_DEREG_ARCHIVED_OBJECTS,
  BT_PF_CMD_BIP_SRV_AUTH_RSP,
  BT_PF_CMD_BIP_SRV_FORCE_DCN,
  BT_PF_CMD_BIP_SRV_GET_CAPABILITIES_RSP,
  BT_PF_CMD_BIP_SRV_PUT_IMAGE_RSP,
  BT_PF_CMD_BIP_SRV_PUT_LINKED_THUMBNAIL_RSP,
  BT_PF_CMD_BIP_SRV_PUT_LINKED_ATTACHMENT,
  BT_PF_CMD_BIP_SRV_GET_IMAGES_LIST_RSP,
  BT_PF_CMD_BIP_SRV_GET_IMAGE_PROPERTIES_RSP,
  BT_PF_CMD_BIP_SRV_GET_IMAGE_RSP,
  BT_PF_CMD_BIP_SRV_GET_LINKED_THUMBNAIL_RSP,
  BT_PF_CMD_BIP_SRV_GET_LINKED_ATTACHMENT_RSP,
  BT_PF_CMD_BIP_SRV_DELETE_IMAGE_RSP,
  BT_PF_CMD_BIP_SRV_GET_STATUS_RSP,
  BT_PF_CMD_BIP_SRV_GET_PARTIAL_IMAGE_RSP,
  BT_PF_CMD_BIP_SRV_START_ARCHIVE_RSP,
  BT_PF_CMD_BIP_SRV_REMOTE_DISPLAY_RSP,
  BT_PF_CMD_BIP_SRV_GET_MONITORING_IMAGE_RSP,

#ifdef FEATURE_BT_TEST_API
#error code not present
#endif /* FEATURE_BT_TEST_API */
#endif /* FEATURE_BT_EXTPF_BIP */

#ifdef FEATURE_BT_EXTPF_BPP
  BT_PF_CMD_BPP_SNDR_CONNECT = BT_CMD_EV_PF_BPP_BASE,
  BT_PF_CMD_BPP_SNDR_DISCONNECT,
  BT_PF_CMD_BPP_SNDR_AUTH_RSP,
  BT_PF_CMD_BPP_SNDR_SEND_FILE,
  BT_PF_CMD_BPP_SNDR_SEND_REFERENCE,
  BT_PF_CMD_BPP_SNDR_SOAP_REQ,
  BT_PF_CMD_BPP_SNDR_REGISTER,
  BT_PF_CMD_BPP_SNDR_DEREGISTER,
  BT_PF_CMD_BPP_SNDR_GET_REF_OBJ_RSP,
  BT_PF_CMD_BPP_SNDR_FORCE_DISCONNECT,

#ifdef FEATURE_BT_TEST_API      
#error code not present
#endif /* FEATURE_BT_TEST_API */
#endif /* FEATURE_BT_EXTPF_BPP */

#ifdef FEATURE_BT_EXTPF_SML
  BT_PF_CMD_SML_REGISTER_CLI = BT_CMD_EV_PF_SML_BASE,
  BT_PF_CMD_SML_DEREGISTER_CLI,
  BT_PF_CMD_SML_REGISTER_SRV,
  BT_PF_CMD_SML_DEREGISTER_SRV,
  BT_PF_CMD_SML_AUTH_REQ,
  BT_PF_CMD_SML_ACCEPT,
  BT_PF_CMD_SML_AUTH_RSP,
  BT_PF_CMD_SML_CONNECT,
  BT_PF_CMD_SML_DISCONNECT,
  BT_PF_CMD_SML_GET,
  BT_PF_CMD_SML_PUT,
  BT_PF_CMD_SML_GET_RSP,
  BT_PF_CMD_SML_PUT_RSP,
#ifdef FEATURE_BT_TEST_API
#error code not present
#endif /* FEATURE_BT_TEST_API */
#endif /* FEATURE_BT_EXTPF_SML */

#ifdef FEATURE_BT_EXTPF_CTP
  /* CTP commands */
  BT_PF_CMD_CTP_CONNECT = BT_CMD_EV_PF_CTP_BASE,
  BT_PF_CMD_CTP_ACCEPT_CONNECTION,
  BT_PF_CMD_CTP_DISCONNECT,
  BT_PF_CMD_CTP_CLEAR_CALL,
  BT_PF_CMD_CTP_ESTABLISH_CALL,
  BT_PF_CMD_CTP_INFO,
  BT_PF_CMD_CTP_INCOMING_STATUS,
  BT_PF_CMD_CTP_ACCEPT_CALL,
  BT_PF_CMD_CTP_START_DTMF,
  BT_PF_CMD_CTP_STOP_DTMF,
  BT_PF_CMD_CTP_REGISTER,
  BT_PF_CMD_CTP_DEREGISTER,
  BT_PF_CMD_CTP_REGISTER_RECALL,
  /* ICP commands */
  BT_PF_CMD_ICP_CONNECT,
  BT_PF_CMD_ICP_ACCEPT_CONNECTION,
  BT_PF_CMD_ICP_DISCONNECT,
  BT_PF_CMD_ICP_CLEAR_CALL,
  BT_PF_CMD_ICP_ESTABLISH_CALL,
  BT_PF_CMD_ICP_INCOMING_STATUS,
  BT_PF_CMD_ICP_ACCEPT_CALL,
  BT_PF_CMD_ICP_REGISTER,
  BT_PF_CMD_ICP_DEREGISTER,
#ifdef FEATURE_BT_TEST_API
#error code not present
#endif /* FEATURE_BT_TEST_API */
#endif /* FEATURE_BT_EXTPF_CTP */

#if defined( FEATURE_BT_EXTPF_SAP ) || defined( FEATURE_EXPORT_BTPF )
  BT_PF_CMD_SAP_REGISTER = BT_CMD_EV_PF_SAP_BASE,
  BT_PF_CMD_SAP_DEREGISTER,
  BT_PF_CMD_SAP_CONNECT,
  BT_PF_CMD_SAP_DISCONNECT,
  BT_PF_CMD_SAP_ACCEPT_CONNECTION,
  BT_PF_CMD_SAP_TX_APDU_REQ,
  BT_PF_CMD_SAP_TX_APDU_RSP,
  BT_PF_CMD_SAP_TX_ATR_REQ,
  BT_PF_CMD_SAP_TX_ATR_RSP,
  BT_PF_CMD_SAP_POWER_SIM_OFF,
  BT_PF_CMD_SAP_POWER_SIM_OFF_RSP,
  BT_PF_CMD_SAP_POWER_SIM_ON,
  BT_PF_CMD_SAP_POWER_SIM_ON_RSP,
  BT_PF_CMD_SAP_RESET_SIM,
  BT_PF_CMD_SAP_RESET_SIM_RSP,
  BT_PF_CMD_SAP_GET_CARD_READER_STATUS,
  BT_PF_CMD_SAP_TX_CARD_READER_STATUS,
  BT_PF_CMD_SAP_TX_SIM_STATUS,
#endif /* FEATURE_BT_EXTPF_SAP || FEATURE_EXPORT_BTPF */

#ifdef FEATURE_BT_EXTPF_AV
  /* A2DP */
  BT_PF_CMD_A2DP_ENABLE = BT_CMD_EV_PF_A2DP_BASE,
  BT_PF_CMD_A2DP_DISABLE,
  BT_PF_CMD_A2DP_SET_DEVICE,
  BT_PF_CMD_A2DP_CONNECT,
  BT_PF_CMD_A2DP_DISCONNECT,
  BT_PF_CMD_A2DP_CTL,
  /* The following four should only be used by AVS: */
  BT_PF_CMD_A2DP_START,
  BT_PF_CMD_A2DP_SUSPEND,
  BT_PF_CMD_A2DP_SEND_DATA,
  BT_PF_CMD_A2DP_FLUSH,

  /* AVRCP */
  BT_PF_CMD_AVRCP_ENABLE = BT_CMD_EV_PF_AVRCP_BASE,
  BT_PF_CMD_AVRCP_SET_DEVICE,
  BT_PF_CMD_AVRCP_CONNECT,
  BT_PF_CMD_AVRCP_DISCONNECT,
  BT_PF_CMD_AVRCP_DISABLE,
  BT_PF_CMD_AVRCP_SEND_REPLY,
  BT_PF_CMD_AVRCP_SEND_GENERIC_REPLY,

#ifdef FEATURE_BT_AVRCP_13
  /* AVRCP meta data commands */
  BT_PF_CMD_AVRCP_METADATA_ENABLE,
  BT_PF_CMD_AVRCP_EVENTID_REPLY,
  BT_PF_CMD_AVRCP_COMPID_REPLY,
  BT_PF_CMD_AVRCP_LIST_ATTRID_REPLY,
  BT_PF_CMD_AVRCP_LIST_ATTRVALUE_REPLY,
  BT_PF_CMD_AVRCP_GET_ATTRVALUE_REPLY,
  BT_PF_CMD_AVRCP_SET_ATTRVALUE_REPLY,
  BT_PF_CMD_AVRCP_GET_ATTR_SETTING_TEXT_REPLY,
  BT_PF_CMD_AVRCP_GET_ATTR_SETTING_VALUE_TEXT_REPLY,
  BT_PF_CMD_AVRCP_PLAY_STATUS_REPLY,
  BT_PF_CMD_AVRCP_GET_ELEMENTS_ATTR_REPLY,
  BT_PF_CMD_AVRCP_REG_NOTIFICATION_REPLY,
  BT_PF_CMD_AVRCP_FRAGMENT_REPLY,
  BT_PF_CMD_AVRCP_GROUP_NAV_REPLY,
  BT_PF_CMD_AVRCP_SIMPLE_META_REPLY,
#endif /* FEATURE_BT_AVRCP_13 */

  /* AVDTP */
  BT_PF_CMD_AVDTP_REGISTER = BT_CMD_EV_PF_AVDTP_BASE,
  BT_PF_CMD_AVDTP_DEREGISTER,
  BT_PF_CMD_AVDTP_REGISTER_ENDPOINT,
  BT_PF_CMD_AVDTP_DEREGISTER_ENDPOINT,
  BT_PF_CMD_AVDTP_CONNECT,
  BT_PF_CMD_AVDTP_ACCEPT,
  BT_PF_CMD_AVDTP_DISCONNECT,
  BT_PF_CMD_AVDTP_DISCOVER,
  BT_PF_CMD_AVDTP_GET_CAP,
  BT_PF_CMD_AVDTP_SET_CONFIG,
  BT_PF_CMD_AVDTP_GET_CONFIG,
  BT_PF_CMD_AVDTP_SET_CONFIG_RSP,
  BT_PF_CMD_AVDTP_OPEN,
  BT_PF_CMD_AVDTP_START,
  BT_PF_CMD_AVDTP_CLOSE,
  BT_PF_CMD_AVDTP_ABORT,
  BT_PF_CMD_AVDTP_SUSPEND,
  BT_PF_CMD_AVDTP_RECONFIGURE,
  BT_PF_CMD_AVDTP_RECONFIGURE_RSP,
  BT_PF_CMD_AVDTP_SEND_DATA,
  BT_PF_CMD_AVDTP_SECURITY_CONTROL,
  BT_PF_CMD_AVDTP_SECURITY_CONTROL_RSP,
#endif /* FEATURE_BT_EXTPF_AV */

#ifdef FEATURE_BT_EXTPF_PBAP
  BT_PF_CMD_PBAP_CLI_CONNECT = BT_CMD_EV_PF_PBAP_BASE,
  BT_PF_CMD_PBAP_CLI_DISCONNECT,
  BT_PF_CMD_PBAP_CLI_AUTH_RSP,
  BT_PF_CMD_PBAP_CLI_REGISTER,
  BT_PF_CMD_PBAP_CLI_DEREGISTER,
  BT_PF_CMD_PBAP_CLI_PULL_PHONE_BOOK,
  BT_PF_CMD_PBAP_CLI_SET_PHONE_BOOK,
  BT_PF_CMD_PBAP_CLI_PULL_VCARD_LISTING,
  BT_PF_CMD_PBAP_CLI_PULL_VCARD_ENTRY,
  BT_PF_CMD_PBAP_CLI_ABORT,

  BT_PF_CMD_PBAP_SRV_REGISTER,
  BT_PF_CMD_PBAP_SRV_DEREGISTER,
  BT_PF_CMD_PBAP_SRV_AUTH_RSP,
  BT_PF_CMD_PBAP_SRV_ACCEPT,
  BT_PF_CMD_PBAP_SRV_FORCE_DCN,

  BT_PF_CMD_PBAP_SRV_PULL_PHONE_BOOK_RSP,
  BT_PF_CMD_PBAP_SRV_SET_PHONE_BOOK_RSP,
  BT_PF_CMD_PBAP_SRV_PULL_VCARD_LISTING_RSP,
  BT_PF_CMD_PBAP_SRV_PULL_VCARD_ENTRY_RSP,
#endif /* FEATURE_BT_EXTPF_PBAP */          

#ifdef FEATURE_BT_EXTPF_HID_HOST
#error code not present
#endif /* FEATURE_BT_EXTPF_HID_HOST */

  BT_PF_CMD_LAST
} bt_pf_cmd_type;

#endif /* FEATURE_BT_EXTPF_GOEP || FEATURE_BT_EXTPF_AV */

#ifdef FEATURE_BT_EXTPF_GOEP
/*-------------------------------------------------------------------------*/
/*      Generic Object Exchange Profile (GOEP) Command Messages            */
/*-------------------------------------------------------------------------*/
typedef BT_PACKED struct
{
  bt_pf_goep_cli_conn_id_type          conn_id;
  byte                                 user_id[BT_PF_OBEX_MAX_USERID_LEN]; 
  uint8                                user_id_len;
  char                                 password_str[BT_PF_OBEX_MAX_PASSWORD_LEN + 1];
} bt_pf_cmd_goep_cli_auth_response_type;

typedef BT_PACKED struct
{
  bt_bd_addr_type                      bd_addr;
  uint8                                channel; 
  bt_pf_goep_obex_hdr_type*            headers_ptr;
  uint8                                headers_cnt;
  bt_sd_uuid_type                      uuid;
  bt_security_type                     security; 
} bt_pf_cmd_goep_cli_connect_type;

typedef BT_PACKED struct
{                                           
  bt_pf_goep_cli_conn_id_type          conn_id; 
  bt_pf_goep_obex_hdr_type*            headers_ptr;
  uint8                                headers_cnt;
} bt_pf_cmd_goep_cli_disconnect_type;

typedef BT_PACKED struct
{                                           
  bt_pf_goep_cli_conn_id_type          conn_id; 
  bt_pf_goep_obex_hdr_type*            headers_ptr;
  uint8                                headers_cnt;
  bt_cmd_status_type                   status; 
} bt_pf_cmd_goep_cli_put_type;

typedef BT_PACKED struct
{                                           
  bt_pf_goep_cli_conn_id_type          conn_id; 
  bt_pf_goep_obex_hdr_type*            headers_ptr;
  uint8                                headers_cnt;
  boolean                              final; 
} bt_pf_cmd_goep_cli_get_type;

typedef BT_PACKED struct
{                                           
  bt_pf_goep_cli_conn_id_type          conn_id;
  uint16                               folder_str[BT_PF_MAX_FILENAME_LEN + 1];
  uint8                                folder_len;
  boolean                              up_level;
  boolean                              dont_create; 
  bt_pf_goep_obex_hdr_type*            headers_ptr;
  uint8                                headers_cnt;
} bt_pf_cmd_goep_cli_set_path_type;


typedef BT_PACKED struct
{                                           
  bt_pf_goep_srv_conn_id_type          conn_id;
  boolean                              accept; 
  bt_pf_goep_obex_hdr_type*            headers_ptr;
  uint8                                headers_cnt;
  bt_cmd_status_type                   status;
} bt_pf_cmd_goep_srv_accept_type;
 
typedef BT_PACKED struct
{                                               
  bt_pf_goep_srv_conn_id_type          conn_id; 
  bt_pf_goep_obex_hdr_type*            headers_ptr;
  uint8                                headers_cnt; 
  bt_cmd_status_type                   status;
} bt_pf_cmd_goep_srv_confirm_set_path_type;

typedef BT_PACKED struct
{                                               
  bt_pf_goep_srv_conn_id_type          conn_id;
  char                                 password[BT_PF_OBEX_MAX_PASSWORD_LEN + 1];                               
  boolean                              read_only;
} bt_pf_cmd_goep_srv_auth_response_type;

typedef BT_PACKED struct
{                                               
  bt_pf_goep_srv_conn_id_type          conn_id; 
  bt_pf_goep_obex_hdr_type*            headers_ptr;
  uint8                                headers_cnt; 
  bt_cmd_status_type                   status;
} bt_pf_cmd_goep_srv_get_response_type;
                                 
typedef BT_PACKED struct
{                                               
  bt_pf_goep_srv_conn_id_type          conn_id; 
  bt_pf_goep_obex_hdr_type*            headers_ptr;
  uint8                                headers_cnt; 
  bt_cmd_status_type                   status;
} bt_pf_cmd_goep_srv_put_response_type;

typedef BT_PACKED struct
{                                               
  bt_pf_goep_srv_conn_id_type          conn_id; 
  bt_pf_goep_obex_hdr_type*            headers_ptr;
  uint8                                headers_cnt; 
  bt_cmd_status_type                   status;
} bt_pf_cmd_goep_srv_dcn_response_type;

typedef BT_PACKED struct
{                                               
  bt_pf_goep_srv_conn_id_type          conn_id;
} bt_pf_cmd_goep_srv_force_disconnect_type;

#endif /* FEATURE_BT_EXTPF_GOEP */

#ifdef FEATURE_BT_EXTPF_OPP
/*-------------------------------------------------------------------------*/
/*           Object Push Profile (OPP) Command Messages                    */
/*-------------------------------------------------------------------------*/
typedef BT_PACKED struct
{
  bt_bd_addr_type                      bd_addr;
  uint8                                channel;    
} bt_pf_cmd_opp_cli_connect_type;

typedef BT_PACKED struct
{
  bt_pf_opp_cli_conn_id_type           conn_id;
} bt_pf_cmd_opp_cli_disconnect_type;

typedef BT_PACKED struct
{
  bt_pf_opp_cli_conn_id_type           conn_id;                              
  uint16                               name_str[BT_PF_MAX_FILENAME_LEN + 1];
  uint8                                name_len;
  char                                 type_str[BT_PF_MAX_MIME_TYPE_LEN + 1];                         
} bt_pf_cmd_opp_cli_push_type;

typedef BT_PACKED struct
{
  bt_pf_opp_cli_conn_id_type           conn_id;
} bt_pf_cmd_opp_cli_pull_type;

typedef BT_PACKED struct
{
  bt_pf_opp_cli_conn_id_type           conn_id;
} bt_pf_cmd_opp_cli_abort_type;

typedef BT_PACKED struct
{
  bt_pf_opp_cli_conn_id_type           conn_id;
  bt_pf_opp_handle_type                handle;
  bt_cmd_status_type                   status;
  uint32                               size;
  uint16                               name_str[BT_PF_MAX_FILENAME_LEN + 1];
  uint8                                name_len;
  char                                 type_str[BT_PF_MAX_MIME_TYPE_LEN + 1]; 
} bt_pf_cmd_opp_cli_open_read_done_type;

typedef BT_PACKED struct
{
  bt_pf_opp_cli_conn_id_type           conn_id;
  bt_pf_opp_handle_type                handle;
  bt_cmd_status_type                   status;
} bt_pf_cmd_opp_cli_open_write_done_type; 

typedef BT_PACKED struct
{
  bt_pf_opp_cli_conn_id_type           conn_id;
  bt_pf_opp_handle_type                handle;
  bt_cmd_status_type                   status;
  uint16                               data_len;
  byte*                                data_ptr;
} bt_pf_cmd_opp_cli_read_done_type;

typedef BT_PACKED struct
{
  bt_pf_opp_cli_conn_id_type           conn_id;
  bt_pf_opp_handle_type                handle;
  bt_cmd_status_type                   status;
} bt_pf_cmd_opp_cli_write_done_type;

typedef BT_PACKED struct
{
  uint32                               supported_formats;
  char                                 srv_name_str[BT_PF_MAX_FILENAME_LEN + 1];
} bt_pf_cmd_opp_srv_register_type;

typedef BT_PACKED struct
{
  bt_pf_opp_srv_conn_id_type           conn_id;
  boolean                              allow_push;
  boolean                              allow_pull;
} bt_pf_cmd_opp_srv_accept_type;

typedef BT_PACKED struct
{
  bt_pf_opp_srv_conn_id_type           conn_id;
} bt_pf_cmd_opp_srv_force_disconnect_type;

typedef BT_PACKED struct
{
  bt_pf_opp_srv_conn_id_type           conn_id;
  bt_pf_opp_handle_type                handle;
  bt_cmd_status_type                   status;
  uint32                               size;
  uint16                               name_str[BT_PF_MAX_FILENAME_LEN + 1];
  uint8                                name_len;
  char                                 type_str[BT_PF_MAX_MIME_TYPE_LEN + 1]; 
} bt_pf_cmd_opp_srv_open_read_done_type;

typedef BT_PACKED struct
{
  bt_pf_opp_srv_conn_id_type           conn_id;
  bt_pf_opp_handle_type                handle;
  bt_cmd_status_type                   status;
} bt_pf_cmd_opp_srv_open_write_done_type;

typedef BT_PACKED struct
{
  bt_pf_opp_srv_conn_id_type           conn_id;
  bt_pf_opp_handle_type                handle;
  bt_cmd_status_type                   status;
} bt_pf_cmd_opp_srv_write_done_type; 

typedef BT_PACKED struct
{
  bt_pf_opp_srv_conn_id_type           conn_id;
  bt_pf_opp_handle_type                handle;
  bt_cmd_status_type                   status;
  uint16                               data_len;
  byte*                                data_ptr;
} bt_pf_cmd_opp_srv_read_done_type;

#ifdef FEATURE_BT_TEST_API
#error code not present
#endif /* FEATURE_BT_TEST_API */

#endif /* FEATURE_BT_EXTPF_OPP */

#ifdef FEATURE_BT_EXTPF_FTP
/*-------------------------------------------------------------------------*/
/*            File Transfer Profile (FTP) Command Messages                 */
/*-------------------------------------------------------------------------*/
typedef BT_PACKED struct
{ 
  bt_bd_addr_type                      bd_addr;
  uint8                                channel;    
} bt_pf_cmd_ftp_cli_connect_type;

typedef BT_PACKED struct
{
  bt_pf_ftp_cli_conn_id_type           conn_id;
} bt_pf_cmd_ftp_cli_disconnect_type;

typedef BT_PACKED struct
{
  bt_pf_ftp_cli_conn_id_type           conn_id;
  byte                                 user_id[BT_PF_OBEX_MAX_USERID_LEN];
  uint8                                user_id_len;
  char                                 password_str[BT_PF_OBEX_MAX_PASSWORD_LEN + 1];
} bt_pf_cmd_ftp_cli_auth_rsp_type;

typedef BT_PACKED struct
{ 
  bt_pf_ftp_cli_conn_id_type           conn_id;
  uint16                               local_name_str[BT_PF_MAX_FILENAME_LEN + 1];
  uint16                               remote_name_str[BT_PF_MAX_FILENAME_LEN + 1];
  uint8                                local_name_len;
  uint8                                remote_name_len;
} bt_pf_cmd_ftp_cli_put_type;

typedef BT_PACKED struct
{ 
  bt_pf_ftp_cli_conn_id_type           conn_id;
  uint16                               name_str[BT_PF_MAX_FILENAME_LEN + 1];
  uint8                                name_len;
} bt_pf_cmd_ftp_cli_delete_type;

typedef BT_PACKED struct
{ 
  bt_pf_ftp_cli_conn_id_type           conn_id;
  uint16                               local_name_str[BT_PF_MAX_FILENAME_LEN + 1];
  uint16                               remote_name_str[BT_PF_MAX_FILENAME_LEN + 1];
  uint8                                local_name_len;
  uint8                                remote_name_len;
} bt_pf_cmd_ftp_cli_get_type;

typedef BT_PACKED struct
{  
  bt_pf_ftp_cli_conn_id_type           conn_id;      
  uint16                               folder_str[BT_PF_MAX_FILENAME_LEN + 1];
  uint8                                folder_len;
  bt_pf_ftp_cli_set_path_type          set_path_ctrl;  
} bt_pf_cmd_ftp_cli_set_path_type;

typedef BT_PACKED struct
{                                     
  bt_pf_ftp_cli_conn_id_type           conn_id;
  uint16                               folder_str[BT_PF_MAX_FILENAME_LEN + 1];
  uint8                                folder_len;
} bt_pf_cmd_ftp_cli_new_subfolder_type;

typedef BT_PACKED struct
{
  bt_pf_ftp_cli_conn_id_type           conn_id;
  uint16                               folder_str[BT_PF_MAX_FILENAME_LEN + 1];
  uint8                                folder_len;
} bt_pf_cmd_ftp_cli_get_folder_listing_type;

typedef BT_PACKED struct
{
  bt_pf_ftp_cli_conn_id_type           conn_id;
  uint16                               local_name_str[BT_PF_MAX_FILENAME_LEN + 1];
  uint16                               remote_name_str[BT_PF_MAX_FILENAME_LEN + 1];
  uint8                                local_name_len;
  uint8                                remote_name_len;
} bt_pf_cmd_ftp_cli_tree_put_type;

typedef BT_PACKED struct
{
  bt_pf_ftp_cli_conn_id_type           conn_id;
  uint16                               local_name_str[BT_PF_MAX_FILENAME_LEN + 1];
  uint16                               remote_name_str[BT_PF_MAX_FILENAME_LEN + 1];
  uint8                                local_name_len;
  uint8                                remote_name_len;
} bt_pf_cmd_ftp_cli_tree_get_type;

typedef BT_PACKED struct
{
  bt_pf_ftp_cli_conn_id_type           conn_id;
} bt_pf_cmd_ftp_cli_abort_type;

typedef BT_PACKED struct
{
  bt_pf_ftp_cli_conn_id_type           conn_id;
  bt_cmd_status_type                   status;
  bt_pf_ftp_handle_type                handle;
  uint32                               size;
} bt_pf_cmd_ftp_cli_browse_done_type;

typedef BT_PACKED struct
{
  bt_pf_ftp_cli_conn_id_type           conn_id;
  bt_cmd_status_type                   status;
  bt_pf_ftp_handle_type                handle;
  uint32                               size;
} bt_pf_cmd_ftp_cli_open_done_type;

typedef BT_PACKED struct
{
  bt_pf_ftp_cli_conn_id_type           conn_id;
  bt_cmd_status_type                   status;
  bt_pf_ftp_handle_type                handle;
  uint16                               len;
  byte*                                data_ptr;
} bt_pf_cmd_ftp_cli_read_done_type;

typedef BT_PACKED struct
{
  bt_pf_ftp_cli_conn_id_type           conn_id;
  bt_cmd_status_type                   status;
  bt_pf_ftp_handle_type                handle;
} bt_pf_cmd_ftp_cli_write_done_type;

typedef BT_PACKED struct
{
  bt_pf_ftp_cli_conn_id_type           conn_id;
  bt_cmd_status_type                   status;
} bt_pf_cmd_ftp_cli_set_folder_done_type;

typedef BT_PACKED struct
{
  bt_pf_goep_srv_auth_type             auth;
  boolean                              read_only;
  boolean                              allow_browse;
  char                                 srv_name_str[BT_PF_MAX_FILENAME_LEN + 1];
} bt_pf_cmd_ftp_srv_register_type;

typedef BT_PACKED struct
{   
  bt_pf_ftp_srv_conn_id_type           conn_id;
  boolean                              read_only;
  boolean                              allow_browse;
  char                                 password_str[BT_PF_OBEX_MAX_PASSWORD_LEN + 1];
} bt_pf_cmd_ftp_srv_auth_rsp_type;

typedef BT_PACKED struct
{   
  bt_pf_ftp_srv_conn_id_type           conn_id;
  uint16                               root_folder_str[BT_PF_MAX_FILENAME_LEN + 1];
  uint8                                root_folder_len;
  boolean                              accept;
} bt_pf_cmd_ftp_srv_accept_type;

typedef BT_PACKED struct
{   
  bt_pf_ftp_srv_conn_id_type           conn_id;
} bt_pf_cmd_ftp_srv_force_disconnect_type;

typedef BT_PACKED struct
{
  bt_pf_ftp_srv_conn_id_type           conn_id;
  bt_cmd_status_type                   status;
  bt_pf_ftp_handle_type                handle;
  uint32                               size;  
} bt_pf_cmd_ftp_srv_browse_done_type;

typedef BT_PACKED struct
{
  bt_pf_ftp_srv_conn_id_type           conn_id;
  bt_cmd_status_type                   status;
  bt_pf_ftp_handle_type                handle;
  uint32                               size;  
} bt_pf_cmd_ftp_srv_open_done_type;

typedef BT_PACKED struct
{
  bt_pf_ftp_srv_conn_id_type           conn_id;
  bt_cmd_status_type                   status;
  bt_pf_ftp_handle_type                handle;
  uint16                               len;
  byte*                                data_ptr;   
} bt_pf_cmd_ftp_srv_read_done_type;

typedef BT_PACKED struct
{
  bt_pf_ftp_srv_conn_id_type           conn_id;
  bt_cmd_status_type                   status;
  bt_pf_ftp_handle_type                handle;
} bt_pf_cmd_ftp_srv_write_done_type;

typedef BT_PACKED struct
{
  bt_pf_ftp_srv_conn_id_type           conn_id;
  bt_cmd_status_type                   status;
} bt_pf_cmd_ftp_srv_delete_done_type;

typedef BT_PACKED struct
{
  bt_pf_ftp_srv_conn_id_type           conn_id;
  bt_cmd_status_type                   status;
} bt_pf_cmd_ftp_srv_set_folder_done_type;


#ifdef FEATURE_BT_TEST_API
#error code not present
#endif /* FEATURE_BT_TEST_API */

#endif /* FEATURE_BT_EXTPF_FTP */

#ifdef FEATURE_BT_EXTPF_BIP
/*-------------------------------------------------------------------------*/
/*            Basic Imaging Profile (BIP) Command Messages                 */
/*-------------------------------------------------------------------------*/

typedef BT_PACKED struct
{                                              
  bt_bd_addr_type                      bd_addr;
  uint8                                channel;
  byte                                 target[BT_PF_UUID128_SIZE];
} bt_pf_cmd_bip_cli_connect_type;

typedef BT_PACKED struct
{   
  bt_pf_bip_cli_conn_id_type           conn_id;
} bt_pf_cmd_bip_cli_disconnect_type;

typedef BT_PACKED struct
{   
  bt_pf_bip_cli_conn_id_type           conn_id;
  char                                 pin_str[BT_PF_OBEX_MAX_PASSWORD_LEN + 1];
} bt_pf_cmd_bip_cli_auth_rsp_type;

typedef BT_PACKED struct
{   
  bt_pf_bip_cli_conn_id_type           conn_id;
} bt_pf_cmd_bip_cli_get_capabilities_type;

typedef BT_PACKED struct
{   
  bt_pf_bip_cli_conn_id_type           conn_id;
  uint16                               name_str[BT_PF_MAX_FILENAME_LEN + 1];
  uint16                               name_len;
  bt_pf_byte_seq_type*                 descriptior_ptr;
  bt_pf_byte_seq_type*                 data_ptr;
  boolean                              final;
} bt_pf_cmd_bip_cli_put_image_type;

typedef BT_PACKED struct
{   
  bt_pf_bip_cli_conn_id_type           conn_id;
  uint16                               image_handle_str[BT_PF_BIP_IMAGE_HANDLE_LEN + 1];
  bt_pf_byte_seq_type*                 data_ptr;
  boolean                              final;
} bt_pf_cmd_bip_cli_put_thumbnail_type;

typedef BT_PACKED struct
{   
  bt_pf_bip_cli_conn_id_type           conn_id;
  uint16                               image_handle_str[BT_PF_BIP_IMAGE_HANDLE_LEN + 1];
  bt_pf_byte_seq_type*                 descriptior_ptr;
  bt_pf_byte_seq_type*                 data_ptr;
  boolean                              final;
} bt_pf_cmd_bip_cli_put_attachment_type;

typedef BT_PACKED struct
{   
  bt_pf_bip_cli_conn_id_type           conn_id;
  uint16                               handle_count;
  uint16                               handle_offset;
  uint8                                latest;
  bt_pf_byte_seq_type*                 image_handles_descriptor_ptr;
} bt_pf_cmd_bip_cli_get_list_type;

typedef BT_PACKED struct
{   
  bt_pf_bip_cli_conn_id_type           conn_id;
  uint16                               image_handle_str[BT_PF_BIP_IMAGE_HANDLE_LEN + 1];
} bt_pf_cmd_bip_cli_get_properties_type;

typedef BT_PACKED struct
{   
  bt_pf_bip_cli_conn_id_type           conn_id;
  uint16                               image_handle_str[BT_PF_BIP_IMAGE_HANDLE_LEN + 1];
  bt_pf_byte_seq_type*                 descriptior_ptr;
} bt_pf_cmd_bip_cli_get_image_type;

typedef BT_PACKED struct
{   
  bt_pf_bip_cli_conn_id_type           conn_id;
  uint16                               image_handle_str[BT_PF_BIP_IMAGE_HANDLE_LEN + 1];
} bt_pf_cmd_bip_cli_get_thumbnail_type;

typedef BT_PACKED struct
{   
  bt_pf_bip_cli_conn_id_type           conn_id;
  uint16                               image_handle_str[BT_PF_BIP_IMAGE_HANDLE_LEN + 1];
  uint16                               name_str[BT_PF_MAX_FILENAME_LEN + 1];
  uint8                                name_len;
} bt_pf_cmd_bip_cli_get_attachment_type;

typedef BT_PACKED struct
{   
  bt_pf_bip_cli_conn_id_type           conn_id; 
  uint16                               image_handle_str[BT_PF_BIP_IMAGE_HANDLE_LEN + 1];
} bt_pf_cmd_bip_cli_delete_image_type;

typedef BT_PACKED struct
{   
  bt_pf_bip_cli_conn_id_type           conn_id;
} bt_pf_cmd_bip_cli_get_status_type;

typedef BT_PACKED struct
{   
  bt_pf_bip_cli_conn_id_type           conn_id;
  uint16                               name_str[BT_PF_MAX_FILENAME_LEN + 1];
  uint8                                name_len;
  uint32                               partial_length;
  uint32                               partial_offset;
} bt_pf_cmd_bip_cli_get_partial_image_type;

typedef BT_PACKED struct
{   
  bt_pf_bip_cli_conn_id_type           conn_id;
  bt_sd_uuid128_type                   ref_obj_srv_service_id;
  bt_pf_byte_seq_type*                 data_ptr;
  boolean                              final;
} bt_pf_cmd_bip_cli_start_print_type;

typedef BT_PACKED struct
{   
  bt_pf_bip_cli_conn_id_type           conn_id;
  bt_sd_uuid128_type                   arch_obj_srv_service_id;
} bt_pf_cmd_bip_cli_start_archive_type;

typedef BT_PACKED struct
{   
  bt_pf_bip_cli_conn_id_type           conn_id;
  uint16                               image_handle_str[BT_PF_BIP_IMAGE_HANDLE_LEN + 1];
  uint8                                display_command;
} bt_pf_cmd_bip_cli_remote_display_type;

typedef BT_PACKED struct
{   
  bt_pf_bip_cli_conn_id_type           conn_id;
  boolean                              store_image;
} bt_pf_cmd_bip_cli_get_monitor_image_type;

typedef BT_PACKED struct
{                                           
  char                                 srv_name_str[BT_PF_MAX_FILENAME_LEN + 1];
  uint8                                supported_capabilities;
  uint16                               supported_features;
  uint32                               supported_functions;
  uint64                               imaging_capacity;
  boolean                              auth;
} bt_pf_cmd_bip_srv_reg_img_responder_type;

typedef BT_PACKED struct
{                                           
  char                                 srv_name_str[BT_PF_MAX_FILENAME_LEN + 1];
  bt_sd_uuid128_type                   service_id;
  uint32                               supported_functions;
  boolean                              auth;
} bt_pf_cmd_bip_srv_reg_ref_objects_type;

typedef BT_PACKED struct
{                                           
  char                                 srv_name_str[BT_PF_MAX_FILENAME_LEN + 1];
  bt_sd_uuid128_type                   service_id;
  uint32                               supported_functions;
  boolean                              auth;
} bt_pf_cmd_bip_srv_reg_arch_objects_type;

typedef BT_PACKED struct
{   
  bt_pf_bip_srv_conn_id_type           conn_id;
  char                                 pin_str[BT_PF_OBEX_MAX_PASSWORD_LEN + 1];
} bt_pf_cmd_bip_srv_auth_rsp_type;

typedef BT_PACKED struct
{   
  bt_pf_bip_srv_conn_id_type           conn_id;
} bt_pf_cmd_bip_srv_force_disconnect_type;

typedef BT_PACKED struct
{   
  bt_pf_bip_srv_conn_id_type           conn_id;
  bt_pf_byte_seq_type*                 data_ptr;
  boolean                              final;
  bt_cmd_status_type                   status;
} bt_pf_cmd_bip_srv_get_capabilities_rsp_type;

typedef BT_PACKED struct
{   
  bt_pf_bip_srv_conn_id_type           conn_id;
  uint16                               image_handle_str[BT_PF_BIP_IMAGE_HANDLE_LEN + 1];
  bt_cmd_status_type                   status;
} bt_pf_cmd_bip_srv_put_image_rsp_type;

typedef BT_PACKED struct
{   
  bt_pf_bip_srv_conn_id_type           conn_id;
  bt_cmd_status_type                   status;
} bt_pf_cmd_bip_srv_put_thumbnail_rsp_type;

typedef BT_PACKED struct
{   
  bt_pf_bip_srv_conn_id_type           conn_id;
  bt_cmd_status_type                   status;
} bt_pf_cmd_bip_srv_put_attachment_rsp_type;

typedef BT_PACKED struct
{   
  bt_pf_bip_srv_conn_id_type           conn_id;
  bt_cmd_status_type                   status;
  uint16                               num_returned_handles;
  bt_pf_byte_seq_type*                 image_handles_descriptor_ptr;
  bt_pf_byte_seq_type*                 data_ptr;
  boolean                              final;
} bt_pf_cmd_bip_srv_get_list_rsp_type;

typedef BT_PACKED struct
{   
  bt_pf_bip_srv_conn_id_type           conn_id;
  bt_cmd_status_type                   status;
  bt_pf_byte_seq_type*                 data_ptr;
  boolean                              final;
} bt_pf_cmd_bip_srv_get_properties_rsp_type;

typedef BT_PACKED struct
{   
  bt_pf_bip_srv_conn_id_type           conn_id;
  bt_cmd_status_type                   status;
  uint32                               file_size;
  bt_pf_byte_seq_type*                 data_ptr;
  boolean                              final;
} bt_pf_cmd_bip_srv_get_image_rsp_type;

typedef BT_PACKED struct
{   
  bt_pf_bip_srv_conn_id_type           conn_id;
  bt_cmd_status_type                   status;
  bt_pf_byte_seq_type*                 data_ptr;
  boolean                              final;
} bt_pf_cmd_bip_srv_get_thumbnail_rsp_type;

typedef BT_PACKED struct
{   
  bt_pf_bip_srv_conn_id_type           conn_id;
  bt_cmd_status_type                   status;
  bt_pf_byte_seq_type*                 data_ptr;
  boolean                              final;
} bt_pf_cmd_bip_srv_get_attachment_rsp_type;

typedef BT_PACKED struct
{   
  bt_pf_bip_srv_conn_id_type           conn_id;
  bt_cmd_status_type                   status;
} bt_pf_cmd_bip_srv_delete_image_rsp_type;

typedef BT_PACKED struct
{   
  bt_pf_bip_srv_conn_id_type           conn_id;
  bt_cmd_status_type                   status;
} bt_pf_cmd_bip_srv_get_status_rsp_type;

typedef BT_PACKED struct
{   
  bt_pf_bip_srv_conn_id_type           conn_id;
  uint32                               total_file_size;
  boolean                              end_flag;
  bt_pf_byte_seq_type*                 data_ptr;
  boolean                              final;
  bt_cmd_status_type                   status;
} bt_pf_cmd_bip_srv_get_partial_image_rsp_type;

typedef BT_PACKED struct
{   
  bt_pf_bip_srv_conn_id_type           conn_id;
  bt_cmd_status_type                   status;
} bt_pf_cmd_bip_srv_start_archive_rsp_type;

typedef BT_PACKED struct
{   
  bt_pf_bip_srv_conn_id_type           conn_id;
  uint16                               image_handle_str[BT_PF_BIP_IMAGE_HANDLE_LEN + 1];
  bt_cmd_status_type                   status;
} bt_pf_cmd_bip_srv_remote_display_rsp_type;

typedef BT_PACKED struct
{   
  bt_pf_bip_srv_conn_id_type           conn_id;
  uint16                               image_handle_str[BT_PF_BIP_IMAGE_HANDLE_LEN + 1];
  bt_pf_byte_seq_type*                 data_ptr;
  boolean                              final;
  bt_cmd_status_type                   status;
} bt_pf_cmd_bip_srv_get_monitor_image_rsp_type;

#ifdef FEATURE_BT_TEST_API
#error code not present
#endif /* FEATURE_BT_TEST_API */

#endif /* FEATURE_BT_EXTPF_BIP */

#ifdef FEATURE_BT_EXTPF_BPP
/*-------------------------------------------------------------------------*/
/*            Basic Printing Profile (BPP) Command Messages                */
/*-------------------------------------------------------------------------*/

typedef BT_PACKED struct
{                                              
  bt_bd_addr_type                      bd_addr;
  uint8                                channel;
  bt_pf_bpp_target_type                target;
} bt_pf_cmd_bpp_sndr_connect_type;

typedef BT_PACKED struct
{                                              
  bt_pf_bpp_sndr_conn_id_type          conn_id;
} bt_pf_cmd_bpp_sndr_disconnect_type;

typedef BT_PACKED struct      
{
  bt_pf_bpp_target_type                target;
  byte                                 user_id[BT_PF_OBEX_MAX_USERID_LEN]; 
  uint8                                user_id_len;
  char                                 password_str[BT_PF_OBEX_MAX_PASSWORD_LEN + 1];
} bt_pf_cmd_bpp_sndr_auth_rsp_type;

typedef BT_PACKED struct
{                                              
  bt_pf_bpp_sndr_conn_id_type          conn_id;      
  uint16                               name_str[BT_PF_MAX_FILENAME_LEN + 1];        
  uint8                                name_len;
  char                                 type_str[BT_PF_MAX_MIME_TYPE_LEN + 1];
  uint16*                              description_ptr;       
  bt_pf_bpp_job_id_type                job_id;
  bt_pf_byte_seq_type*                 data_ptr;
  boolean                              final;          
} bt_pf_cmd_bpp_sndr_send_file_type;

typedef BT_PACKED struct
{                                              
  bt_pf_bpp_sndr_conn_id_type          conn_id;
  bt_pf_bpp_reference_type             type;
  bt_pf_byte_seq_type*                 http_headers_ptr;
  bt_pf_bpp_job_id_type                job_id;
  bt_pf_byte_seq_type*                 data_ptr;
  boolean                              final;
} bt_pf_cmd_bpp_sndr_send_reference_type;

typedef BT_PACKED struct
{                                              
  bt_pf_bpp_sndr_conn_id_type          conn_id;
  bt_pf_byte_seq_type*                 body_ptr;
  boolean                              final;
} bt_pf_cmd_bpp_sndr_soap_req_type;

typedef BT_PACKED struct
{                                              
  bt_pf_bpp_sndr_conn_id_type          conn_id;
  bt_pf_byte_seq_type*                 data_ptr;
  int32                                file_size;
  bt_cmd_status_type                   status;
} bt_pf_cmd_bpp_sndr_get_ref_obj_rsp_type;

typedef BT_PACKED struct
{
  char                                 srv_name_str[BT_PF_MAX_FILENAME_LEN + 1];
} bt_pf_cmd_bpp_sndr_register_type;

typedef BT_PACKED struct
{
  bt_pf_bpp_sndr_conn_id_type           conn_id;
} bt_pf_cmd_bpp_sndr_force_disconnect_type;

#ifdef FEATURE_BT_TEST_API
#error code not present
#endif /* FEATURE_BT_TEST_API */

#endif /* FEATURE_BT_EXTPF_BPP */

#ifdef FEATURE_BT_EXTPF_SML
/*-------------------------------------------------------------------------*/
/*                    SML Command Messages                                 */
/*-------------------------------------------------------------------------*/

typedef BT_PACKED struct
{ 
  char                                 srv_name_str[BT_PF_MAX_FILENAME_LEN + 1];   
  boolean                              auth;
} bt_pf_cmd_sml_register_cli_type;

typedef BT_PACKED struct
{
  char                                 srv_name_str[BT_PF_MAX_FILENAME_LEN + 1];   
  boolean                              auth;
} bt_pf_cmd_sml_register_srv_type;

typedef BT_PACKED struct
{         
  bt_pf_sml_conn_id_type               conn_id;
  char                                 password_str[BT_PF_OBEX_MAX_PASSWORD_LEN + 1];
  boolean                              read_only;
} bt_pf_cmd_sml_auth_req_type;

typedef BT_PACKED struct
{                                              
  bt_pf_sml_conn_id_type               conn_id;
  boolean                              accept;
  bt_cmd_status_type                   status;
} bt_pf_cmd_sml_accept_type;


typedef BT_PACKED struct
{         
  bt_pf_sml_conn_id_type               conn_id;                                      
  byte                                 user_id[BT_PF_OBEX_MAX_USERID_LEN];
  uint8                                user_id_len;
  char                                 password_str[BT_PF_OBEX_MAX_PASSWORD_LEN + 1];
} bt_pf_cmd_sml_auth_rsp_type;

typedef BT_PACKED struct
{
  bt_bd_addr_type                      bd_addr;
  uint8                                channel;
  char                                 target_uuid_str[BT_PF_UUID128_SIZE + 1];
} bt_pf_cmd_sml_connect_type;

typedef BT_PACKED struct
{
  bt_pf_sml_conn_id_type               conn_id;
} bt_pf_cmd_sml_disconnect_type;

typedef BT_PACKED struct
{
  bt_pf_sml_conn_id_type               conn_id;
  char                                 type_str[BT_PF_MAX_MIME_TYPE_LEN + 1];
} bt_pf_cmd_sml_get_type;

typedef BT_PACKED struct
{
  bt_pf_sml_conn_id_type               conn_id;
  char                                 type_str[BT_PF_MAX_MIME_TYPE_LEN + 1];
  bt_pf_byte_seq_type*                 data_ptr;
  bt_cmd_status_type                   status;
} bt_pf_cmd_sml_put_type;

typedef BT_PACKED struct
{
  bt_pf_sml_conn_id_type               conn_id;                              
  bt_pf_byte_seq_type*                 data_ptr;
  bt_cmd_status_type                   status;
} bt_pf_cmd_sml_get_rsp_type;

typedef BT_PACKED struct
{
  bt_pf_sml_conn_id_type               conn_id;
  bt_cmd_status_type                   status;
} bt_pf_cmd_sml_put_rsp_type;

#ifdef FEATURE_BT_TEST_API
#error code not present
#endif /* FEATURE_BT_TEST_API */
typedef BT_PACKED struct
{
  uint16                               len;
  byte                                 data_bytes[1];
} bt_pf_cmd_apit_sml_send_data_type;
#endif /* FEATURE_BT_EXTPF_SML */

#ifdef FEATURE_BT_EXTPF_CTP
/*-------------------------------------------------------------------------*/
/*         Cordless Telephony Profile (CTP) Command Messages               */
/*-------------------------------------------------------------------------*/
typedef BT_PACKED struct
{
  bt_bd_addr_type                     bd_addr;
} bt_pf_cmd_ctp_connect_type;

typedef BT_PACKED struct
{
  bt_bd_addr_type                     bd_addr;
  boolean                             accept;
} bt_pf_cmd_ctp_accept_connection_type;

typedef BT_PACKED struct
{
  bt_bd_addr_type                     bd_addr;
} bt_pf_cmd_ctp_disconnect_type;

typedef BT_PACKED struct
{
  bt_bd_addr_type                     bd_addr;
  bt_pf_ctp_cause_type                cause;
} bt_pf_cmd_ctp_clear_call_type;

typedef BT_PACKED struct
{
  bt_bd_addr_type                     bd_addr;
  bt_pf_ctp_number_type               calling_number;
  bt_pf_ctp_number_type               called_number;
} bt_pf_cmd_ctp_establish_call_type;

typedef BT_PACKED struct
{
  bt_bd_addr_type                     bd_addr;
  boolean                             sending_complete;
  char                                keypad_facility;
  bt_pf_ctp_number_type               calling_number;
  bt_pf_ctp_number_type               called_number;
  bt_pf_ctp_audio_control_type        audio_control;
} bt_pf_cmd_ctp_info_type;

typedef BT_PACKED struct
{
  bt_bd_addr_type                     bd_addr;
  bt_pf_ctp_call_status_type          status;
} bt_pf_cmd_ctp_incoming_status_type;

typedef BT_PACKED struct
{
  bt_bd_addr_type                     bd_addr;
} bt_pf_cmd_ctp_accept_call_type;

typedef BT_PACKED struct
{
  bt_bd_addr_type                     bd_addr;
  char                                tone;
} bt_pf_cmd_ctp_start_dtmf_type;

typedef BT_PACKED struct
{
  bt_bd_addr_type                     bd_addr;
} bt_pf_cmd_ctp_stop_dtmf_type;

typedef BT_PACKED struct
{
  bt_bd_addr_type                     bd_addr;
} bt_pf_cmd_ctp_register_recall_type;

/*-------------------------------------------------------------------------*/
/*              Intercomm Profile (ICP) Command Messages                   */
/*-------------------------------------------------------------------------*/
typedef BT_PACKED struct
{
  bt_bd_addr_type                     bd_addr;
} bt_pf_cmd_icp_connect_type;

typedef BT_PACKED struct
{
  bt_bd_addr_type                     bd_addr;
  boolean                             accept;
} bt_pf_cmd_icp_accept_connection_type;

typedef BT_PACKED struct
{
  bt_bd_addr_type                     bd_addr;
} bt_pf_cmd_icp_disconnect_type;

typedef BT_PACKED struct
{
  bt_bd_addr_type                     bd_addr;
  bt_pf_icp_cause_type                cause;
} bt_pf_cmd_icp_clear_call_type;

typedef BT_PACKED struct
{
  bt_bd_addr_type                     bd_addr;
  bt_pf_icp_number_type               calling_number;
  bt_pf_icp_number_type               called_number;
} bt_pf_cmd_icp_establish_call_type;

typedef BT_PACKED struct
{
  bt_bd_addr_type                     bd_addr;
  bt_pf_icp_call_status_type          status;
} bt_pf_cmd_icp_incoming_status_type;

typedef BT_PACKED struct
{
  bt_bd_addr_type                     bd_addr;
} bt_pf_cmd_icp_accept_call_type;
#endif /* FEATURE_BT_EXTPF_CTP */

#if defined( FEATURE_BT_EXTPF_SAP ) || defined( FEATURE_EXPORT_BT )
/*-------------------------------------------------------------------------*/
/*               SIM Access Profile (SAP) Command Messages                 */
/*-------------------------------------------------------------------------*/
typedef BT_PACKED struct
{
  uint16                        min_frame_size;
  uint16                        max_frame_size;
  uint8*                        req_dst_ptr;
  uint16*                       req_len_ptr;
  char                          srv_name_str[BT_PF_MAX_FILENAME_LEN + 1];
} bt_pf_cmd_sap_register_type;

// bt_pf_cmd_sap_deregister_type has no body

typedef BT_PACKED struct
{
  bt_bd_addr_type               bd_addr;
  uint8                         channel;
  uint16                        min_frame_size;
  uint16                        max_frame_size;
  uint8*                        rsp_dst_ptr;
  uint16*                       rsp_len_ptr;
} bt_pf_cmd_sap_connect_type;

typedef BT_PACKED struct
{
  boolean                       disc_immediately; // ignored if client
} bt_pf_cmd_sap_disconnect_type;

typedef BT_PACKED struct
{
  boolean                       accept;
} bt_pf_cmd_sap_accept_conn_type;

typedef BT_PACKED struct
{
  const uint8*                  req_ptr;
  uint16                        req_len;
} bt_pf_cmd_sap_tx_apdu_req_type;

typedef BT_PACKED struct
{
  bt_pf_sap_result_code_type    result;
  const uint8*                  rsp_ptr;
  uint16                        rsp_len;
} bt_pf_cmd_sap_tx_apdu_rsp_type;

// bt_pf_cmd_sap_tx_atr_req_type has no body

typedef BT_PACKED struct
{
  bt_pf_sap_result_code_type    result;
  const uint8*                  rsp_ptr;
  uint16                        rsp_len;
} bt_pf_cmd_sap_tx_atr_rsp_type;

// bt_pf_cmd_sap_power_sim_off_type has no body

typedef BT_PACKED struct
{
  bt_pf_sap_result_code_type    result;
} bt_pf_cmd_sap_power_sim_off_rsp_type;

// bt_pf_cmd_sap_power_sim_on_type has no body

typedef BT_PACKED struct
{
  bt_pf_sap_result_code_type    result;
} bt_pf_cmd_sap_power_sim_on_rsp_type;

// bt_pf_cmd_sap_reset_sim_type has no body

typedef BT_PACKED struct
{
  bt_pf_sap_result_code_type    result;
} bt_pf_cmd_sap_reset_sim_rsp_type;

// bt_pf_cmd_sap_get_card_reader_stat_type has no body

typedef BT_PACKED struct
{
  bt_pf_sap_result_code_type    result;
  uint8                         status;
} bt_pf_cmd_sap_tx_card_reader_stat_type;

typedef BT_PACKED struct
{
  uint8                         status;
} bt_pf_cmd_sap_tx_sim_stat_type;

#endif /* FEATURE_BT_EXTPF_SAP || FEATURE_EXPORT_BT */

#ifdef FEATURE_BT_EXTPF_AV
/*-------------------------------------------------------------------------*/
/*                     A2DP Command Messages                               */
/*-------------------------------------------------------------------------*/

typedef BT_PACKED struct {
  bt_bd_addr_type      bd_addr;
} bt_cmd_pf_a2dp_connect_type;

typedef bt_cmd_pf_a2dp_connect_type bt_cmd_pf_a2dp_set_device_type;

typedef BT_PACKED struct {
  audiosbcenc_data_type sbc_data;
} bt_cmd_pf_a2dp_start_type;

typedef BT_PACKED struct {
  uint32               timestamp;
  dsm_item_type*       data;
  uint32               seq_num;
  uint32               timestamps_per_frame;
  uint8                num_frames;
  uint16               frame_len;
  boolean              last_frame;
} bt_cmd_pf_a2dp_send_data_type;

typedef BT_PACKED struct {
  bt_pf_a2dp_ctl_type  ctl;
  // The interpretation of the data field depends on the ctl value sent.
  uint32               data1;
} bt_cmd_pf_a2dp_ctl_type;

/*-------------------------------------------------------------------------*/
/*                     AVRCP Command Messages                              */
/*-------------------------------------------------------------------------*/
typedef BT_PACKED struct {
  uint8                supported_categories;
} bt_cmd_pf_avrcp_enable_type;

typedef BT_PACKED struct {
  bt_bd_addr_type      bd_addr;
} bt_cmd_pf_avrcp_connect_type;

typedef BT_PACKED struct {
  bt_bd_addr_type            bd_addr;
  uint8                      txn;
  bt_pf_avrcp_op_status_type status;
  bt_event_type              opcode;
  bt_pf_avrcp_rsp_type       response;
} bt_cmd_pf_avrcp_send_reply_type;

typedef BT_PACKED struct {
  bt_bd_addr_type             bd_addr;
  uint8                       txn;
  bt_pf_avrcp_generic_op_type frame;
} bt_cmd_pf_avrcp_send_generic_reply_type;

#ifdef FEATURE_BT_AVRCP_13

/* AVRCP meta data command structures definition */
typedef BT_PACKED struct {
  bt_bd_addr_type             bd_addr;
  uint8                       txn;
  uint8                       event_count;
  uint8*                      event_ids;
} bt_cmd_pf_avrcp_send_eventid_reply_type;

typedef BT_PACKED struct {
  bt_bd_addr_type             bd_addr;
  uint8                       txn;
  uint8                       comp_count;
  uint32*                     comp_ids;
} bt_cmd_pf_avrcp_send_compid_reply_type;

typedef BT_PACKED struct {
  bt_bd_addr_type             bd_addr;
  uint8                       txn;
  uint8                       attr_count;
  uint8*                      attr_ids;
} bt_cmd_pf_avrcp_send_list_attrid_reply_type;

typedef BT_PACKED struct {
  bt_bd_addr_type             bd_addr;
  uint8                       txn;
  uint8                       attr_id;
  uint8                       attr_val_count;
  uint8*                      attr_settings_vals;
} bt_cmd_pf_avrcp_send_list_attrvalue_reply_type;

typedef BT_PACKED struct {
  bt_bd_addr_type             bd_addr;
  uint8                       txn;
  uint8                       attr_id_count;
  bt_pf_avrcp_attr_pair_type  attrs;  
} bt_cmd_pf_avrcp_send_get_attr_value_reply_type;

typedef BT_PACKED struct {
  bt_bd_addr_type               bd_addr;
  uint8                         txn;
  uint8                         attr_id_count;
  bt_pf_avrcp_attr_text_type*   attrs_text;
} bt_cmd_pf_avrcp_send_get_attr_setting_text_reply_type;

typedef BT_PACKED struct {
  bt_bd_addr_type               bd_addr;
  uint8                         txn;
  uint8                         attr_val_count;
  bt_pf_avrcp_attr_text_type*   attrs_val_text;
} bt_cmd_pf_avrcp_send_get_attr_setting_value_text_reply_type;  

typedef BT_PACKED struct {
  bt_bd_addr_type                bd_addr;
  uint8                          txn;
  bt_pf_avrcp_play_status_type   play_status;      
} bt_cmd_pf_avrcp_send_play_status_reply_type;

typedef BT_PACKED struct {
  bt_bd_addr_type                             bd_addr;
  uint8                                       txn;
  bt_pf_avrcp_notif_event_type                ev_metadata;
  boolean                                     is_final;
} bt_cmd_pf_avrcp_send_register_notification_reply_type;

typedef BT_PACKED struct {
  bt_bd_addr_type                             bd_addr;
  uint8                                       txn;
  uint8                                       media_attr_count;
  bt_pf_avrcp_element_attr_type*              media_element_attr;
} bt_cmd_pf_avrcp_send_get_element_attributes_reply_type;

typedef BT_PACKED struct {
  bt_bd_addr_type             bd_addr;
  uint8                       txn;
  uint16                      vendor_unique_id;
  boolean                     button_press;
  bt_pf_avrcp_rsp_type        resp_code;  
} bt_cmd_pf_avrcp_send_group_navigation_reply_type;

typedef BT_PACKED struct {
  bt_bd_addr_type             bd_addr;
  uint8                       txn;
  uint8                       pdu_id; 
  bt_cmd_status_type          status;
} bt_cmd_pf_avrcp_cmd_send_simple_meta_reply_type;


#endif /* FEATURE_BT_AVRCP_13 */

/*-------------------------------------------------------------------------*/
/*                     AVDTP Command Messages                              */
/*-------------------------------------------------------------------------*/

/* These commands are empty:
   bt_cmd_pf_avdtp_register_type;
   bt_cmd_pf_avdtp_deregister_type;
*/

typedef BT_PACKED struct
{
  uint8                media_type;
  uint8                endpoint_type;
  bt_pf_avdtp_cap_type caps[BT_PF_AVDTP_MAX_CAPS];
  uint8                num_caps;
  uint8*               seid; /* Output value: To be filled in by profile */
} bt_cmd_pf_avdtp_register_endpoint_type;

typedef BT_PACKED struct
{
  uint8                seid;
} bt_cmd_pf_avdtp_deregister_endpoint_type;

typedef BT_PACKED struct
{
  bt_bd_addr_type      bd_addr;
} bt_cmd_pf_avdtp_connect_type;

typedef BT_PACKED struct
{
  bt_bd_addr_type      bd_addr;
  boolean              accept;
} bt_cmd_pf_avdtp_accept_type;

typedef BT_PACKED struct
{
  bt_bd_addr_type      bd_addr;
} bt_cmd_pf_avdtp_disconnect_type;

typedef BT_PACKED struct
{
  bt_bd_addr_type      bd_addr;
  uint8*               txn; /* Output value */
} bt_cmd_pf_avdtp_discover_type;

typedef BT_PACKED struct
{
  bt_bd_addr_type      bd_addr;
  uint8                remote_seid;
  uint8*               txn; /* Output value */
} bt_cmd_pf_avdtp_get_cap_type;

typedef BT_PACKED struct
{
  bt_bd_addr_type      bd_addr;
  uint8                remote_seid;
  uint8                local_seid;
  bt_pf_avdtp_cap_type caps[BT_PF_AVDTP_MAX_CAPS];
  uint8                num_caps;
  uint8*               txn;    /* Output value */
  uint16*              handle; /* Output value */
} bt_cmd_pf_avdtp_set_config_type;

typedef BT_PACKED struct
{
  uint16               handle;
  uint8*               txn; /* Output value */
} bt_cmd_pf_avdtp_get_config_type;

typedef BT_PACKED struct
{
  uint16               handle;
  uint8                txn;
  uint8                category;
  bt_cmd_status_type   error;
} bt_cmd_pf_avdtp_set_config_rsp_type;

typedef BT_PACKED struct
{
  uint16               handle;
  uint8*               txn; /* Output value */
} bt_cmd_pf_avdtp_open_type;

typedef BT_PACKED struct
{
  uint16               handle;
  uint8*               txn; /* Output value */
} bt_cmd_pf_avdtp_start_type;

typedef BT_PACKED struct
{
  uint16               handle;
  uint8*               txn; /* Output value */
} bt_cmd_pf_avdtp_close_type;

typedef BT_PACKED struct
{
  uint16               handle;
  uint8*               txn; /* Output value */
} bt_cmd_pf_avdtp_abort_type;

typedef BT_PACKED struct
{
  uint16               handle;
  uint8*               txn; /* Output value */
} bt_cmd_pf_avdtp_suspend_type;

typedef BT_PACKED struct
{
  uint16               handle;
  bt_pf_avdtp_cap_type caps[BT_PF_AVDTP_MAX_CAPS];
  uint8                num_caps;
  uint8*               txn; /* Output value */
} bt_cmd_pf_avdtp_reconfigure_type;

typedef BT_PACKED struct
{
  uint16               handle;
  uint8                txn;
  uint8                category;
  bt_cmd_status_type   error;
} bt_cmd_pf_avdtp_reconfigure_rsp_type;

typedef BT_PACKED struct
{
  uint16               handle;
  uint16               seq_num;
  uint32               timestamp;
  uint8                payload_type;
  uint16               marker;
  dsm_item_type*       data;
  void*                frame_handle;
} bt_cmd_pf_avdtp_send_data_type;

typedef BT_PACKED struct
{
  uint16               handle;
  dsm_item_type*       data;
  uint8*               txn; /* Output value */  
} bt_cmd_pf_avdtp_security_control_type;

typedef BT_PACKED struct
{
  uint16               handle;
  uint8                txn;
  dsm_item_type*       data;
  bt_cmd_status_type   error;
} bt_cmd_pf_avdtp_security_control_rsp_type;
#endif /* FEATURE_BT_EXTPF_AV */

#ifdef FEATURE_BT_EXTPF_PBAP
/*-------------------------------------------------------------------------*/
/*            Phone Book Access Profile (PBAP) Command Messages            */
/*-------------------------------------------------------------------------*/
typedef BT_PACKED struct
{
  char                                 cli_name_str[BT_PF_MAX_FILENAME_LEN + 1];
} bt_pf_cmd_pbap_cli_register_type;

typedef BT_PACKED struct
{ 
  bt_bd_addr_type                      bd_addr;
  uint8                                channel;    
} bt_pf_cmd_pbap_cli_connect_type;

typedef BT_PACKED struct
{
  bt_pf_pbap_cli_conn_id_type          conn_id;
} bt_pf_cmd_pbap_cli_disconnect_type;

typedef BT_PACKED struct
{
  bt_pf_pbap_cli_conn_id_type          conn_id;
} bt_pf_cmd_pbap_cli_abort_type;

typedef BT_PACKED struct
{
  bt_pf_pbap_cli_conn_id_type          conn_id;
  byte                                 user_id[BT_PF_OBEX_MAX_USERID_LEN];
  uint8                                user_id_len;
  char                                 pin_str[BT_PF_OBEX_MAX_PASSWORD_LEN + 1];
} bt_pf_cmd_pbap_cli_auth_rsp_type;

typedef BT_PACKED struct
{ 
  bt_pf_pbap_cli_conn_id_type          conn_id;
  uint16                               name_str[BT_PF_MAX_FILENAME_LEN + 1];
  uint8                                name_len;
  uint64*                              attribute_mask_ptr;
  uint8*                               format_ptr;
  uint16*                              max_list_count_ptr;
  uint16*                              list_start_offset_ptr;
} bt_pf_cmd_pbap_cli_pull_phone_book_type;

typedef BT_PACKED struct
{ 
  bt_pf_pbap_cli_conn_id_type          conn_id;
  uint16                               folder_str[BT_PF_MAX_FILENAME_LEN + 1];
  uint8                                folder_len;
  bt_pf_pbap_cli_set_path_type         set_path_ctrl;
} bt_pf_cmd_pbap_cli_set_phone_book_type;

typedef BT_PACKED struct
{ 
  bt_pf_pbap_cli_conn_id_type          conn_id;
  uint16                               name_str[BT_PF_MAX_FILENAME_LEN + 1];
  uint8                                name_len;
  uint8*                               order_ptr;
  char                                 value_str[BT_PF_MAX_MIME_TYPE_LEN];
  uint8                                value_len;
  uint8*                               search_attrib_ptr;
  uint16*                              max_list_count_ptr;
  uint16*                              list_start_offset_ptr;
} bt_pf_cmd_pbap_cli_pull_vcard_list_type;

typedef BT_PACKED struct
{  
  bt_pf_pbap_cli_conn_id_type          conn_id;      
  uint16                               name_str[BT_PF_MAX_FILENAME_LEN + 1];
  uint8                                name_len;
  uint64*                              attribute_mask_ptr;
  uint8*                               format_ptr;
} bt_pf_cmd_pbap_cli_pull_vcard_entry_type;

typedef BT_PACKED struct
{
  char                                 srv_name_str[BT_PF_MAX_FILENAME_LEN + 1];
  uint8                                supported_repositories;
  bt_pf_goep_srv_auth_type             auth;
} bt_pf_cmd_pbap_srv_register_type;

typedef BT_PACKED struct
{
  bt_pf_pbap_srv_conn_id_type          conn_id;
  boolean                              accept;
} bt_pf_cmd_pbap_srv_accept_type;

typedef BT_PACKED struct
{   
  bt_pf_pbap_srv_conn_id_type          conn_id;
  char                                 pin_str[BT_PF_OBEX_MAX_PASSWORD_LEN + 1];
} bt_pf_cmd_pbap_srv_auth_rsp_type;

typedef BT_PACKED struct
{
  bt_pf_pbap_srv_conn_id_type          conn_id;
} bt_pf_cmd_pbap_srv_force_disconnect_type;

typedef BT_PACKED struct
{   
  bt_pf_pbap_srv_conn_id_type          conn_id;
  bt_pf_byte_seq_type*                 data_ptr;
  uint16*                              phone_book_size_ptr;
  uint8*                               num_missed_calls_ptr;
  boolean                              final;
  bt_cmd_status_type                   status;
} bt_pf_cmd_pbap_srv_pull_phone_book_rsp_type;

typedef BT_PACKED struct
{
  bt_pf_pbap_srv_conn_id_type          conn_id;
  bt_cmd_status_type                   status;
} bt_pf_cmd_pbap_srv_set_phone_book_rsp_type;

typedef BT_PACKED struct
{   
  bt_pf_pbap_srv_conn_id_type          conn_id;
  bt_pf_byte_seq_type*                 data_ptr;
  uint16*                              phone_book_size_ptr;
  uint8*                               num_missed_calls_ptr;
  boolean                              final;
  bt_cmd_status_type                   status;
} bt_pf_cmd_pbap_srv_pull_vcard_list_rsp_type;

typedef BT_PACKED struct
{   
  bt_pf_pbap_srv_conn_id_type          conn_id;
  bt_pf_byte_seq_type*                 data_ptr;
  boolean                              final;
  bt_cmd_status_type                   status;
} bt_pf_cmd_pbap_srv_pull_vcard_entry_rsp_type;

#endif /* FEATURE_BT_EXTPF_PBAP */

#ifdef FEATURE_BT_EXTPF_HID_HOST
#error code not present
#endif /* FEATURE_BT_EXTPF_HID_HOST */

/*-------------------------------------------------------------------------*/
/*  Union of all BT Profile commands.                                      */
/*-------------------------------------------------------------------------*/

typedef enum
{
  BT_PF_CST_QUEUED = 0xDB01,
  BT_PF_CST_PROCESSING,
  BT_PF_CST_DONE,
  BT_PF_CST_ERROR
} bt_pf_cmd_state_type;

typedef struct
{
  q_link_type           link;
  bt_pf_cmd_type        cmd_type;
  bt_app_id_type        app_id;
  bt_cmd_status_type    cmd_status;
  bt_pf_cmd_state_type  cmd_state; 
  uint16                cmd_retries;
#ifdef FEATURE_BT_TIME_DEBUG
  time_type             cmd_start_time;
#endif /* FEATURE_BT_TIME_DEBUG */
} bt_pf_cmd_hdr_type;

typedef BT_PACKED union
{
#ifdef FEATURE_BT_EXTPF_GOEP                              
  bt_pf_cmd_goep_cli_auth_response_type        cmd_goep_cli_auth_rsp;        
  bt_pf_cmd_goep_cli_connect_type              cmd_goep_cli_connect;
  bt_pf_cmd_goep_cli_disconnect_type           cmd_goep_cli_disconnect;
  bt_pf_cmd_goep_cli_put_type                  cmd_goep_cli_put;
  bt_pf_cmd_goep_cli_get_type                  cmd_goep_cli_get;
  bt_pf_cmd_goep_cli_set_path_type             cmd_goep_cli_set_path; 

  bt_pf_cmd_goep_srv_accept_type               cmd_goep_srv_accept;
  bt_pf_cmd_goep_srv_confirm_set_path_type     cmd_goep_srv_cfm_set_path; 
  bt_pf_cmd_goep_srv_auth_response_type        cmd_goep_srv_auth_rsp;
  bt_pf_cmd_goep_srv_get_response_type         cmd_goep_srv_get_rsp;
  bt_pf_cmd_goep_srv_put_response_type         cmd_goep_srv_put_rsp;
  bt_pf_cmd_goep_srv_force_disconnect_type     cmd_goep_srv_force_dcn;     
  bt_pf_cmd_goep_srv_dcn_response_type         cmd_goep_srv_dcn_rsp;
#endif /* FEATURE_BT_EXTPF_GOEP */

#ifdef FEATURE_BT_EXTPF_OPP
  bt_pf_cmd_opp_cli_connect_type               cmd_opp_cli_connect;
  bt_pf_cmd_opp_cli_disconnect_type            cmd_opp_cli_disconnect;
  bt_pf_cmd_opp_cli_push_type                  cmd_opp_cli_push;
  bt_pf_cmd_opp_cli_pull_type                  cmd_opp_cli_pull;
  bt_pf_cmd_opp_cli_open_read_done_type        cmd_opp_cli_open_read_done;
  bt_pf_cmd_opp_cli_open_write_done_type       cmd_opp_cli_open_write_done;
  bt_pf_cmd_opp_cli_read_done_type             cmd_opp_cli_read_done;
  bt_pf_cmd_opp_cli_write_done_type            cmd_opp_cli_write_done;
  bt_pf_cmd_opp_cli_abort_type                 cmd_opp_cli_abort;

  bt_pf_cmd_opp_srv_register_type              cmd_opp_srv_register;
  bt_pf_cmd_opp_srv_accept_type                cmd_opp_srv_accept;
  bt_pf_cmd_opp_srv_force_disconnect_type      cmd_opp_srv_force_dcn;
  bt_pf_cmd_opp_srv_open_read_done_type        cmd_opp_srv_open_read_done;
  bt_pf_cmd_opp_srv_open_write_done_type       cmd_opp_srv_open_write_done;
  bt_pf_cmd_opp_srv_read_done_type             cmd_opp_srv_read_done;
  bt_pf_cmd_opp_srv_write_done_type            cmd_opp_srv_write_done;
#ifdef FEATURE_BT_TEST_API
#error code not present
#endif /* FEATURE_BT_TEST_API */
#endif /* FEATURE_BT_EXTPF_OPP */
  
#ifdef FEATURE_BT_EXTPF_FTP
  bt_pf_cmd_ftp_cli_connect_type               cmd_ftp_cli_connect;
  bt_pf_cmd_ftp_cli_disconnect_type            cmd_ftp_cli_disconnect;
  bt_pf_cmd_ftp_cli_auth_rsp_type              cmd_ftp_cli_auth_rsp;
  bt_pf_cmd_ftp_cli_put_type                   cmd_ftp_cli_put;
  bt_pf_cmd_ftp_cli_delete_type                cmd_ftp_cli_delete;
  bt_pf_cmd_ftp_cli_get_type                   cmd_ftp_cli_get;
  bt_pf_cmd_ftp_cli_set_path_type              cmd_ftp_cli_set_path;
  bt_pf_cmd_ftp_cli_new_subfolder_type         cmd_ftp_cli_new_subfolder;
  bt_pf_cmd_ftp_cli_get_folder_listing_type    cmd_ftp_cli_get_folder_listing;
  bt_pf_cmd_ftp_cli_tree_put_type              cmd_ftp_cli_tree_put;
  bt_pf_cmd_ftp_cli_tree_get_type              cmd_ftp_cli_tree_get;
  bt_pf_cmd_ftp_cli_abort_type                 cmd_ftp_cli_abort;
  bt_pf_cmd_ftp_cli_browse_done_type           cmd_ftp_cli_browse_done;
  bt_pf_cmd_ftp_cli_open_done_type             cmd_ftp_cli_open_done;
  bt_pf_cmd_ftp_cli_read_done_type             cmd_ftp_cli_read_done;
  bt_pf_cmd_ftp_cli_write_done_type            cmd_ftp_cli_write_done;
  bt_pf_cmd_ftp_cli_set_folder_done_type       cmd_ftp_cli_set_folder_done;

  bt_pf_cmd_ftp_srv_register_type              cmd_ftp_srv_register;
  bt_pf_cmd_ftp_srv_auth_rsp_type              cmd_ftp_srv_auth_rsp;
  bt_pf_cmd_ftp_srv_accept_type                cmd_ftp_srv_accept;
  bt_pf_cmd_ftp_srv_force_disconnect_type      cmd_ftp_srv_force_dcn;
  bt_pf_cmd_ftp_srv_browse_done_type           cmd_ftp_srv_browse_done;
  bt_pf_cmd_ftp_srv_open_done_type             cmd_ftp_srv_open_done;
  bt_pf_cmd_ftp_srv_read_done_type             cmd_ftp_srv_read_done;
  bt_pf_cmd_ftp_srv_write_done_type            cmd_ftp_srv_write_done;
  bt_pf_cmd_ftp_srv_delete_done_type           cmd_ftp_srv_delete_done;
  bt_pf_cmd_ftp_srv_set_folder_done_type       cmd_ftp_srv_set_folder_done;
#ifdef FEATURE_BT_TEST_API
#error code not present
#endif /* FEATURE_BT_TEST_API */
#endif /* FEATURE_BT_EXTPF_FTP */

#ifdef FEATURE_BT_EXTPF_BIP
  bt_pf_cmd_bip_cli_connect_type               cmd_bip_cli_connect;
  bt_pf_cmd_bip_cli_disconnect_type            cmd_bip_cli_disconnect;
  bt_pf_cmd_bip_cli_auth_rsp_type              cmd_bip_cli_auth_rsp;
  bt_pf_cmd_bip_cli_get_capabilities_type      cmd_bip_cli_get_capabilities;
  bt_pf_cmd_bip_cli_put_image_type             cmd_bip_cli_put_image;
  bt_pf_cmd_bip_cli_put_thumbnail_type         cmd_bip_cli_put_thumbnail;
  bt_pf_cmd_bip_cli_put_attachment_type        cmd_bip_cli_put_attachment;
  bt_pf_cmd_bip_cli_get_list_type              cmd_bip_cli_get_list;
  bt_pf_cmd_bip_cli_get_properties_type        cmd_bip_cli_get_properties;
  bt_pf_cmd_bip_cli_get_image_type             cmd_bip_cli_get_image;
  bt_pf_cmd_bip_cli_get_thumbnail_type         cmd_bip_cli_get_thumbnail;
  bt_pf_cmd_bip_cli_get_attachment_type        cmd_bip_cli_get_attachment;
  bt_pf_cmd_bip_cli_delete_image_type          cmd_bip_cli_delete_image;
  bt_pf_cmd_bip_cli_get_status_type            cmd_bip_cli_get_status;
  bt_pf_cmd_bip_cli_get_partial_image_type     cmd_bip_cli_get_partial_image;
  bt_pf_cmd_bip_cli_start_print_type           cmd_bip_cli_start_print;
  bt_pf_cmd_bip_cli_start_archive_type         cmd_bip_cli_start_archive;
  bt_pf_cmd_bip_cli_remote_display_type        cmd_bip_cli_remote_display;
  bt_pf_cmd_bip_cli_get_monitor_image_type     cmd_bip_cli_get_monitor_image;

  bt_pf_cmd_bip_srv_reg_img_responder_type     cmd_bip_srv_reg_img_responder;
  bt_pf_cmd_bip_srv_reg_ref_objects_type       cmd_bip_srv_reg_ref_objects;
  bt_pf_cmd_bip_srv_reg_arch_objects_type      cmd_bip_srv_reg_arch_objects;
  bt_pf_cmd_bip_srv_auth_rsp_type              cmd_bip_srv_auth_rsp;
  bt_pf_cmd_bip_srv_force_disconnect_type      cmd_bip_srv_force_dcn;
  bt_pf_cmd_bip_srv_get_capabilities_rsp_type  cmd_bip_srv_get_capabilities_rsp;
  bt_pf_cmd_bip_srv_put_image_rsp_type         cmd_bip_srv_put_image_rsp;
  bt_pf_cmd_bip_srv_put_thumbnail_rsp_type     cmd_bip_srv_put_thumbnail_rsp;
  bt_pf_cmd_bip_srv_put_attachment_rsp_type    cmd_bip_srv_put_attachment_rsp;
  bt_pf_cmd_bip_srv_get_list_rsp_type          cmd_bip_srv_get_list_rsp;
  bt_pf_cmd_bip_srv_get_properties_rsp_type    cmd_bip_srv_get_properties_rsp;
  bt_pf_cmd_bip_srv_get_image_rsp_type         cmd_bip_srv_get_image_rsp;
  bt_pf_cmd_bip_srv_get_thumbnail_rsp_type     cmd_bip_srv_get_thumbnail_rsp;
  bt_pf_cmd_bip_srv_get_attachment_rsp_type    cmd_bip_srv_get_attachment_rsp;
  bt_pf_cmd_bip_srv_delete_image_rsp_type      cmd_bip_srv_delete_image_rsp;
  bt_pf_cmd_bip_srv_get_status_rsp_type        cmd_bip_srv_get_status_rsp;
  bt_pf_cmd_bip_srv_get_partial_image_rsp_type cmd_bip_srv_get_partial_image_rsp;
  bt_pf_cmd_bip_srv_start_archive_rsp_type     cmd_bip_srv_start_archive_rsp;
  bt_pf_cmd_bip_srv_remote_display_rsp_type    cmd_bip_srv_remote_display_rsp;
  bt_pf_cmd_bip_srv_get_monitor_image_rsp_type cmd_bip_srv_get_monitor_image_rsp;
#ifdef FEATURE_BT_TEST_API
#error code not present
#endif /* FEATURE_BT_TEST_API */
#endif /* FEATURE_BT_EXTPF_BIP */

#ifdef FEATURE_BT_EXTPF_BPP
  bt_pf_cmd_bpp_sndr_connect_type              cmd_bpp_sndr_connect;
  bt_pf_cmd_bpp_sndr_disconnect_type           cmd_bpp_sndr_disconnect;
  bt_pf_cmd_bpp_sndr_auth_rsp_type             cmd_bpp_sndr_auth_rsp;
  bt_pf_cmd_bpp_sndr_send_file_type            cmd_bpp_sndr_send_file;
  bt_pf_cmd_bpp_sndr_send_reference_type       cmd_bpp_sndr_send_reference;
  bt_pf_cmd_bpp_sndr_soap_req_type             cmd_bpp_sndr_soap_req;
  bt_pf_cmd_bpp_sndr_get_ref_obj_rsp_type      cmd_bpp_sndr_get_ref_obj_rsp;
  bt_pf_cmd_bpp_sndr_register_type             cmd_bpp_sndr_register;
  bt_pf_cmd_bpp_sndr_force_disconnect_type     cmd_bpp_sndr_force_disconnect;
#ifdef FEATURE_BT_TEST_API
#error code not present
#endif /* FEATURE_BT_TEST_API */
#endif /* FEATURE_BT_EXTPF_BPP */

#ifdef FEATURE_BT_EXTPF_SML                    
  bt_pf_cmd_sml_register_cli_type              cmd_sml_register_cli;
  bt_pf_cmd_sml_register_srv_type              cmd_sml_register_srv;
  bt_pf_cmd_sml_auth_req_type                  cmd_sml_auth_req;
  bt_pf_cmd_sml_accept_type                    cmd_sml_accept;
  bt_pf_cmd_sml_auth_rsp_type                  cmd_sml_auth_rsp;
  bt_pf_cmd_sml_connect_type                   cmd_sml_connect;
  bt_pf_cmd_sml_disconnect_type                cmd_sml_disconnect;
  bt_pf_cmd_sml_get_type                       cmd_sml_get;
  bt_pf_cmd_sml_put_type                       cmd_sml_put;
  bt_pf_cmd_sml_get_rsp_type                   cmd_sml_get_rsp;
  bt_pf_cmd_sml_put_rsp_type                   cmd_sml_put_rsp;
#ifdef FEATURE_BT_TEST_API
#error code not present
#endif /* FEATURE_BT_TEST_API */
#endif /* FEATURE_BT_EXTPF_SML */

#ifdef FEATURE_BT_EXTPF_CTP
  /* CTP: */
  bt_pf_cmd_ctp_connect_type                   cmd_ctp_connect;
  bt_pf_cmd_ctp_accept_connection_type         cmd_ctp_accept_connection;
  bt_pf_cmd_ctp_disconnect_type                cmd_ctp_disconnect;
  bt_pf_cmd_ctp_clear_call_type                cmd_ctp_clear_call;
  bt_pf_cmd_ctp_establish_call_type            cmd_ctp_establish_call;
  bt_pf_cmd_ctp_info_type                      cmd_ctp_info;
  bt_pf_cmd_ctp_incoming_status_type           cmd_ctp_incoming_status;
  bt_pf_cmd_ctp_accept_call_type               cmd_ctp_accept_call;
  bt_pf_cmd_ctp_start_dtmf_type                cmd_ctp_start_dtmf;
  bt_pf_cmd_ctp_stop_dtmf_type                 cmd_ctp_stop_dtmf;
  bt_pf_cmd_ctp_register_recall_type           cmd_ctp_register_recall;

  /* ICP: */
  bt_pf_cmd_icp_connect_type                   cmd_icp_connect;
  bt_pf_cmd_icp_accept_connection_type         cmd_icp_accept_connection;
  bt_pf_cmd_icp_disconnect_type                cmd_icp_disconnect;
  bt_pf_cmd_icp_clear_call_type                cmd_icp_clear_call;
  bt_pf_cmd_icp_establish_call_type            cmd_icp_establish_call;
  bt_pf_cmd_icp_incoming_status_type           cmd_icp_incoming_status;
  bt_pf_cmd_icp_accept_call_type               cmd_icp_accept_call;
#ifdef FEATURE_BT_TEST_API 
#endif /* FEATURE_BT_TEST_API */
#endif /* FEATURE_BT_EXTPF_CTP */

#if defined( FEATURE_BT_EXTPF_SAP ) || defined( FEATURE_EXPORT_BT )
  bt_pf_cmd_sap_register_type                  cmd_sap_register;
  bt_pf_cmd_sap_accept_conn_type               cmd_sap_accept_conn;
  bt_pf_cmd_sap_connect_type                   cmd_sap_connect;
  bt_pf_cmd_sap_disconnect_type                cmd_sap_disconnect;
  bt_pf_cmd_sap_tx_apdu_req_type               cmd_sap_tx_apdu_req;
  bt_pf_cmd_sap_tx_apdu_rsp_type               cmd_sap_tx_apdu_rsp;
  bt_pf_cmd_sap_tx_atr_rsp_type                cmd_sap_tx_atr_rsp;
  bt_pf_cmd_sap_power_sim_off_rsp_type         cmd_sap_power_sim_off_rsp;
  bt_pf_cmd_sap_power_sim_on_rsp_type          cmd_sap_power_sim_on_rsp;
  bt_pf_cmd_sap_reset_sim_rsp_type             cmd_sap_reset_sim_rsp;
  bt_pf_cmd_sap_tx_card_reader_stat_type       cmd_sap_tx_card_reader_stat;
  bt_pf_cmd_sap_tx_sim_stat_type               cmd_sap_tx_sim_stat;
#endif /* FEATURE_BT_EXTPF_SAP || FEATURE_EXPORT_BT */

#ifdef FEATURE_BT_EXTPF_AV
  bt_cmd_pf_a2dp_set_device_type               cmd_a2dp_set_device;
  bt_cmd_pf_a2dp_connect_type                  cmd_a2dp_connect;
  bt_cmd_pf_a2dp_start_type                    cmd_a2dp_start;
  bt_cmd_pf_a2dp_send_data_type                cmd_a2dp_send_data;
  bt_cmd_pf_a2dp_ctl_type                      cmd_a2dp_ctl;

  bt_cmd_pf_avrcp_enable_type                  cmd_avrcp_enable;
  bt_cmd_pf_avrcp_connect_type                 cmd_avrcp_connect;
  bt_cmd_pf_avrcp_send_reply_type              cmd_avrcp_send_reply;
  bt_cmd_pf_avrcp_send_generic_reply_type      cmd_avrcp_send_generic_reply;

#ifdef FEATURE_BT_AVRCP_13
  /* AVRCP meta data structures */
  bt_cmd_pf_avrcp_send_eventid_reply_type                         cmd_eventid_reply_type;
  bt_cmd_pf_avrcp_send_compid_reply_type                          cmd_compid_reply_type;
  bt_cmd_pf_avrcp_send_list_attrid_reply_type                     cmd_list_attrid_reply_type;
  bt_cmd_pf_avrcp_send_list_attrvalue_reply_type                  cmd_list_attrvalue_reply_type;
  bt_cmd_pf_avrcp_send_get_attr_value_reply_type                  cmd_get_attr_value_reply_type;
  bt_cmd_pf_avrcp_send_get_attr_setting_text_reply_type           cmd_get_attr_setting_text_reply_type;
  bt_cmd_pf_avrcp_send_get_attr_setting_value_text_reply_type     cmd_get_attr_setting_value_text_reply_type;
  bt_cmd_pf_avrcp_send_play_status_reply_type                     cmd_play_status_reply_type;
  bt_cmd_pf_avrcp_send_get_element_attributes_reply_type          cmd_get_element_attributes_reply_type;
  bt_cmd_pf_avrcp_send_register_notification_reply_type           cmd_register_notification_reply_type;
  bt_cmd_pf_avrcp_send_group_navigation_reply_type                cmd_group_navigation_reply_type;
  bt_cmd_pf_avrcp_cmd_send_simple_meta_reply_type                 cmd_send_simple_meta_reply_type;
#endif /* FEATURE_BT_AVRCP_13 */

  bt_cmd_pf_avdtp_register_endpoint_type       cmd_avdtp_register_endpoint;
  bt_cmd_pf_avdtp_deregister_endpoint_type     cmd_avdtp_deregister_endpoint;
  bt_cmd_pf_avdtp_connect_type                 cmd_avdtp_connect;
  bt_cmd_pf_avdtp_accept_type                  cmd_avdtp_accept;
  bt_cmd_pf_avdtp_disconnect_type              cmd_avdtp_disconnect;
  bt_cmd_pf_avdtp_discover_type                cmd_avdtp_discover;
  bt_cmd_pf_avdtp_get_cap_type                 cmd_avdtp_get_cap;
  bt_cmd_pf_avdtp_set_config_type              cmd_avdtp_set_config;
  bt_cmd_pf_avdtp_get_config_type              cmd_avdtp_get_config;
  bt_cmd_pf_avdtp_set_config_rsp_type          cmd_avdtp_set_config_rsp;
  bt_cmd_pf_avdtp_open_type                    cmd_avdtp_open;
  bt_cmd_pf_avdtp_start_type                   cmd_avdtp_start;
  bt_cmd_pf_avdtp_close_type                   cmd_avdtp_close;
  bt_cmd_pf_avdtp_abort_type                   cmd_avdtp_abort;
  bt_cmd_pf_avdtp_suspend_type                 cmd_avdtp_suspend;
  bt_cmd_pf_avdtp_reconfigure_type             cmd_avdtp_reconfigure;
  bt_cmd_pf_avdtp_reconfigure_rsp_type         cmd_avdtp_reconfigure_rsp;
  bt_cmd_pf_avdtp_send_data_type               cmd_avdtp_send_data;
  bt_cmd_pf_avdtp_security_control_type        cmd_avdtp_security_control;
  bt_cmd_pf_avdtp_security_control_rsp_type    cmd_avdtp_security_control_rsp;
#endif /* FEATURE_BT_EXTPF_AV */

#ifdef FEATURE_BT_EXTPF_PBAP
  bt_pf_cmd_pbap_cli_register_type             cmd_pbap_cli_register;
  bt_pf_cmd_pbap_cli_connect_type              cmd_pbap_cli_connect;
  bt_pf_cmd_pbap_cli_disconnect_type           cmd_pbap_cli_disconnect;
  bt_pf_cmd_pbap_cli_abort_type                cmd_pbap_cli_abort;
  bt_pf_cmd_pbap_cli_auth_rsp_type             cmd_pbap_cli_auth_rsp;
  bt_pf_cmd_pbap_cli_pull_phone_book_type      cmd_pbap_cli_pull_phone_book;
  bt_pf_cmd_pbap_cli_set_phone_book_type       cmd_pbap_cli_set_phone_book;
  bt_pf_cmd_pbap_cli_pull_vcard_list_type      cmd_pbap_cli_pull_vcard_list;
  bt_pf_cmd_pbap_cli_pull_vcard_entry_type     cmd_pbap_cli_pull_vcard_entry;

  bt_pf_cmd_pbap_srv_register_type             cmd_pbap_srv_register;
  bt_pf_cmd_pbap_srv_accept_type               cmd_pbap_srv_accept;
  bt_pf_cmd_pbap_srv_auth_rsp_type             cmd_pbap_srv_auth_rsp;
  bt_pf_cmd_pbap_srv_force_disconnect_type     cmd_pbap_srv_force_dcn;
  bt_pf_cmd_pbap_srv_pull_phone_book_rsp_type  cmd_pbap_srv_pull_phone_book_rsp;
  bt_pf_cmd_pbap_srv_set_phone_book_rsp_type   cmd_pbap_srv_set_phone_book_rsp;
  bt_pf_cmd_pbap_srv_pull_vcard_list_rsp_type  cmd_pbap_srv_pull_vcard_list_rsp;
  bt_pf_cmd_pbap_srv_pull_vcard_entry_rsp_type cmd_pbap_srv_pull_vcard_entry_rsp;
#endif /* FEATURE_BT_EXTPF_PBAP */

#ifdef FEATURE_BT_EXTPF_HID_HOST 
#error code not present
#endif /* FEATURE_BT_EXTPF_HID_HOST */

} bt_pf_cmd_params_union;

typedef struct bt_pf_cmd_msg_struct
{
  bt_pf_cmd_hdr_type      cmd_hdr;
  bt_pf_cmd_params_union  cmd_msg;
} bt_pf_cmd_msg_type;


extern bt_pf_cmd_msg_type* bt_pf_get_free_cmd( void );
extern void bt_pf_free_cmd( bt_pf_cmd_msg_type* cmd_ptr );

extern bt_cmd_status_type bt_pf_cmd( bt_pf_cmd_msg_type* );

extern bt_cmd_status_type bt_pf_copy_unicode_str( 
  BT_PACKED uint16*       to_ptr, 
  BT_PACKED uint16*       from_ptr,
  uint8                   max_len,
  uint8*                  actual_len );

extern bt_cmd_status_type bt_pf_copy_ascii_str( 
  char*                   to_ptr, 
  char*                   from_ptr,
  uint8                   max_len );

extern uint8 bt_pf_process_unicode_str( 
  BT_PACKED uint16*       to_ptr,
  bt_pf_unicode_str_type* from_ptr,
  uint8                   max_len);

#ifdef FEATURE_BT_EXTPF_HID_HOST
#error code not present
#endif

#ifdef FEATURE_BT_TEST_API
#error code not present
#endif /* FEATURE_BT_TEST_API */

#endif /* FEATURE_BT_EXTPF */
#endif /* FEATURE_BT */
#endif /* _BTPFCMDI_H */

