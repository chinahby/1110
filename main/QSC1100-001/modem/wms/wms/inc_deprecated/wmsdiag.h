#ifndef WMSDIAG_H
#define WMSDIAG_H

/*===========================================================================


            W I R E L E S S    M E S S A G I N G   S E R V I C E S
            -- Diagnostic Services


General Description

  This file contains the packet definitions and exported functions for the
  WMSDIAG interface.

Copyright (c) 2001, 2002, 2003, 2004, 2005, 2006, 2007, 2008 by QUALCOMM, Inc.  
All Rights Reserved.

Export of this technology or software is regulated by the U.S. Government.
Diversion contrary to U.S. law prohibited.
===========================================================================*/

/*===========================================================================

                             Edit History


when       who     what, where, why
--------   ---     ----------------------------------------------------------
06/04/08   PMD     Removed support for Auto / Transparent SMS 
01/31/08   PMD     Updated Copyright Information to include 2008
01/17/07   HQ      Updated copyright info to include 2007.
05/11/06   PMD     Added Support for New Diag Commands and modified 
                   existing Diag Commands for Delayed Response
05/06/06   HQ      Fixed lint warnings by changing data size and
                   adding data size checking.
01/25/06   VK      Updated Copyright Info
10/08/05   VK      Updated copyright info
06/13/05   Rex     Added new wmsappdiag command: wmsappdiag_cb_delete_all
01/15/05   VK      Added support for 36 digit string
10/12/04   Rex     Featurize IWMSDIAG appropriately
06/24/04   Rex     Changed cb_data length to 88 bytes
05/27/04   Rex     Added new wms diag commands:
                   wmsappdiag_cb_save,
                   wmsappdiag_cb_read,
                   wmsdiag_broadcast_add_delete_serviceid
03/10/04   Rex     Fixed 4-byte alignment problem in
                   wmsdiag_set_default_auto_cdma_params_req_type
02/17/04   Rex     Added new #define for checking max length values
12/11/03   Rex     Changed wmsdiag_set_default_auto_cdma_params by removing
                   dcs and relative validity structure fields
10/30/03   Rex     Added new wms diag commands:
                   wmsdiag_send_auto_message,
                   wmsdiag_set_default_auto_cdma_params,
                   wmsdiag_set_default_auto_gw_params
09/15/03   Rex     Added support for WMS_SET_ROUTES log packet
07/29/03   AP      Added support for WMS_AUTO_DISCONNECT_CMD.
06/03/03   AP      Added tag for status report.
05/28/03   AP      Added command to originate and end a CDMA SMS call.
01/16/03   HQ      Fixed wrong name for transfer_status_report.
11/21/02   AP      Added prototype for wmsdiag_init and included needed
                   headers.
11/14/02   sm      Created.
===========================================================================*/


/*============================================================================
   WMS Subsystem Command Codes
============================================================================*/

#ifdef FEATURE_WMS_DIAG
#if defined(FEATURE_CDSMS) || defined(FEATURE_GWSMS)

#include "diagpkt.h"
#include "log.h"

#define WMSDIAG_SEND_WRITE_CMD                     0x0000
#define WMSDIAG_CLEAR_CMD                          0x0001
#define WMSDIAG_CLEAR_ALL_CMD                      0x0002
#define WMSDIAG_GET_MESSAGE_LIST_CMD               0x0003
#define WMSDIAG_READ_CMD                           0x0004
#define WMSDIAG_SET_MESSAGE_TAG_CMD                0x0005
#define WMSDIAG_SET_ROUTES_CMD                     0x0006
#define WMSDIAG_ORIG_END_SMS_CALL_CMD              0x0007
#define WMSDIAG_AUTO_DISCONNECT_CMD                0x0008
#define WMSDIAG_BROADCAST_ADD_DELETE_SERVICEID_CMD 0x000C
#define WMSAPPDIAG_CB_SAVE_CMD                     0x000D
#define WMSAPPDIAG_CB_READ_CMD                     0x000E
#define WMSDIAG_BROADCAST_DELETE_ALL_SERVICES_CMD  0x000F
#define WMSAPPDIAG_CB_DELETE_MSG_CMD               0x0010
#define WMSAPPDIAG_CB_DELETE_ALL_MSGS_CMD          0x0011
#define WMSDIAG_GET_BEARER_PREF_CMD                0x0012 
#define WMSDIAG_SET_BEARER_PREF_CMD                0x0013  

/*  Should be set to WMS_MAX_LEN from wms.h (can't access here since an enum) */
#define WMSDIAG_MESSAGE_LIST_MAX 255

#define WMSDIAG_MAX_LEN 255
#define WMSDIAG_ADDRESS_MAX 36
#define WMSDIAG_SUBADDRESS_MAX 36

/* Default max size of data buffer of diag req, resp and log packets */
#define WMSDIAG_DEFAULT_DATA_BUF_MAX \
        ( WMSDIAG_ADDRESS_MAX + WMSDIAG_SUBADDRESS_MAX + WMSDIAG_MAX_LEN )

#define WMSAPPDIAG_MAX_STORAGE_TYPE 1
#define WMSAPPDIAG_MAX_CB_MSG_RAM 50
#define WMSAPPDIAG_MAX_BC_MSG_NV 99
#define WMSAPPDIAG_MAX_CB_DATA_SIZE 88


/*============================================================================

    Enumerated Types Used to Interface With WMSDIAG Commands

============================================================================*/
/* Enumerated Type For WMSEDIAG Memory Storage Types */
typedef enum
{
  WMSDIAG_MEM_STORE_NONE =0,
  WMSDIAG_MEM_STORE_RAM_GW,
  WMSDIAG_MEM_STORE_SIM,
  WMSDIAG_MEM_STORE_NV_GW,
  WMSDIAG_MEM_STORE_RAM_CDMA,
  WMSDIAG_MEM_STORE_RUIM,
  WMSDIAG_MEM_STORE_NV_CDMA,
  WMSDIAG_MEM_STORE_MAX,
  WMSDIAG_MEM_STORE_INVALID,
  WMSDIAG_MEM_STORE_FOUR_BYTES = 0x10000000
} WMSDIAG_MemoryStoreType;

/* Enumerated Type For WMSEDIAG Message Tags */

typedef enum
{
  WMSDIAG_TAG_NONE =0,
  WMSDIAG_TAG_MT_READ,
  WMSDIAG_TAG_MT_NOT_READ,
  WMSDIAG_TAG_MO_SENT,
  WMSDIAG_TAG_MO_NOT_SENT,
  WMSDIAG_TAG_MO_SENT_ST_NOT_RECEIVED,
  WMSDIAG_TAG_MO_SENT_ST_NOT_STORED,
  WMSDIAG_TAG_MO_SENT_ST_STORED,
  WMSDIAG_TAG_MO_TEMPLATE,
  WMSDIAG_TAG_STATUS_RPT,
  WMSDIAG_TAG_MAX,
  WMSDIAG_TAG_INVALID,
  WMSDIAG_TAG_FOUR_BYTES = 0x10000000
} WMSDIAG_MessageTagType;

typedef enum
{
  WMSDIAG_SEND_MODE_MEM_STORE = 0,
  WMSDIAG_SEND_MODE_CLIENT,
  WMSDIAG_SEND_MODE_MAX,
  WMSDIAG_SEND_MODE_INVALID
} WMSDIAG_SendModeType;

typedef enum
{
  WMSDIAG_WRITE_MODE_INSERT = 0,
  WMSDIAG_WRITE_MODE_REPLACE,
  WMSDIAG_WRITE_MODE_MAX,
  WMSDIAG_WRITE_MODE_INVALID
} WMSDIAG_WriteModeType;


typedef enum
{
  WMSDIAG_MSG_MODE_CDMA = 0,
  WMSDIAG_MSG_MODE_GW,
  WMSDIAG_MSG_MODE_AUTO,
  WMSDIAG_MSG_MODE_MAX,
  WMSDIAG_MSG_MODE_INVALID
} WMSDIAG_MessageModeType;

typedef enum
{
  WMSDIAG_ADDR_DIGIT_MODE_4_BIT = 0,
  WMSDIAG_ADDR_DIGIT_MODE_8_BIT,
  WMSDIAG_ADDR_DIGIT_MODE_MAX,
  WMSDIAG_ADDR_DIGIT_MODE_INVALID
} WMSDIAG_AddressDigitModeType;

typedef enum
{
  WMSDIAG_ADDR_NUM_MODE_NOT_DATA_NTWK = 0,
  WMSDIAG_ADDR_NUM_MODE_DATA_NTWK,
  WMSDIAG_ADDR_NUM_MODE_MAX,
  WMSDIAG_ADDR_NUM_MODE_INVALID
} WMSDIAG_AddressNumberModeType;

typedef enum
{
  WMSDIAG_DATA_FORMAT_CDMA,              /* IS-95 */
  WMSDIAG_DATA_FORMAT_ANALOG_CLI,        /* IS-91 */
  WMSDIAG_DATA_FORMAT_ANALOG_VOICE_MAIL, /* IS-91 */
  WMSDIAG_DATA_FORMAT_ANALOG_SMS,        /* IS-91 */
  WMSDIAG_DATA_FORMAT_ANALOG_AWISMS,     /* IS-95 Alert With Information SMS */
  WMSDIAG_DATA_FORMAT_MWI,               /* Message Waiting Indication as voice mail */
  WMSDIAG_DATA_FORMAT_GW_PP,             /* GW Point-to-Point SMS */
  WMSDIAG_DATA_FORMAT_GW_CB,              /* GW CB SMS */
  WMSDIAG_DATA_FORMAT_MAX,
  WMSDIAG_DATA_FORMAT_INVALID
} WMSDIAG_DataFormatType;

//#ifdef FEATURE_GWSMS

  typedef enum
  {
    WMSDIAG_TPDU_DELIVER = 0,
    WMSDIAG_TPDU_DELIVER_REPORT_ACK,
    WMSDIAG_TPDU_DELIVER_REPORT_ERROR,
    WMSDIAG_TPDU_SUBMIT,
    WMSDIAG_TPDU_SUBMIT_REPORT_ACK,
    WMSDIAG_TPDU_SUBMIT_REPORT_ERROR,
    WMSDIAG_TPDU_STATUS_REPORT,
    WMSDIAG_TPDU_COMMAND,
    WMSDIAG_TPDU_NONE,
    WMSDIAG_TPDU_MAX,
    WMSDIAG_TPDU_INVALID
  } WMSDIAG_QWTPDUType;

//#endif /* FEATURE_GWSMS */

typedef enum
{
  WMSDIAG_ROUTE_DISCARD = 0,      /* Discard message */
  WMSDIAG_ROUTE_STORE_AND_NOTIFY, /* Store message and notify clients */
  WMSDIAG_ROUTE_TRANSFER_ONLY,    /* Don't store msg but send it to clients */
  WMSDIAG_ROUTE_TRANSFER_AND_ACK, /* Send the message to clients and ack it */
  WMSDIAG_ROUTE_NO_CHANGE,        /* Don't change the current setting */
  WMSDIAG_ROUTE_MAX,
  WMSDIAG_ROUTE_INVALID,
  WMSDIAG_ROUTE_FOUR_BYTES = 0x10000000
} WMSDIAG_RouteType;


typedef struct
{
  uint32 index;
  uint32 tag_type;

} WMSDIAG_MessageListEntryType;

/*============================================================================
    PACKET DEFINITIONS
============================================================================*/

/*============================================================================

PACKET   WMS_SEND_WRITE_CMD

PURPOSE  Sent from the DM to the DMSS to send a message.

RESPONSE Sends back a wmsdiag_send_rsp_type packet to indicate that the request
         has been made with the WMS interface.

============================================================================*/

typedef struct
{
  diagpkt_subsys_header_type header;

  uint8 transaction_ID;
  uint8 reserved_byte1;
  uint8 reserved_byte2;
  uint8 reserved_byte3;

  uint8 write_message;
  uint8 write_mode;
  uint8 send_mode;
  uint8 msg_mode;

  uint32  tag_type;
  uint32 storage_type;
  uint32 index;
  uint32 teleservice_type;

  uint8 addr_digit_mode;
  uint8 addr_num_mode;
  uint8 addr_num_type;
  uint8 addr_num_plan_type;

  uint8 subaddr_type;
  uint8 data_format;
  uint8 tpdu_type;
  uint8 is_broadcast;

  uint8 subaddr_odd_or_even;
  uint8 tl_ack_needed;
  uint8 service_present;
  uint8 is_MO;

  uint32 service_type;

  uint16 addr_num_digits;
  uint16 subaddr_num_digits;

  uint32 user_data_length;

  /*  This is actually of variable length equal to addr_num_digits + subaddr_num_digits + data_length */
  uint8 data [WMSDIAG_DEFAULT_DATA_BUF_MAX];

} wmsdiag_send_write_req_type;

typedef struct
{
  diagpkt_subsys_header_type header;
} wmsdiag_send_write_rsp_type;

/*============================================================================

PACKET   WMS_CLEAR_CMD

PURPOSE  Sent from the DM to the DMSS to delete the entry with the given index
         from the memory type specified.

RESPONSE The DM sends back a wmsdiag_clear_rsp_type packet to indicate that
         it has attempted to clear the message.

============================================================================*/
typedef struct
{
  diagpkt_subsys_header_type header;

  uint8 transaction_ID;
  uint8 reserved_byte_1;
  uint8 reserved_byte_2;
  uint8 reserved_byte_3;

  uint32 index;
  uint32 storage_type;

} wmsdiag_clear_req_type;

typedef struct
{
  diagpkt_subsys_header_type header;
} wmsdiag_clear_rsp_type;

/*============================================================================

PACKET   WMS_CLEAR_ALL_CMD

PURPOSE  Sent from the DM to the DMSS to delete all the messages in a given
         memory storage type.

RESPONSE The DM sends back a wmsdiag_clear_all_rsp_type packet to indicate that
         it has attempted to clear the messages.

============================================================================*/
typedef struct
{
  diagpkt_subsys_header_type header;

  uint8 transaction_ID;
  uint8 reserved_byte_1;
  uint8 reserved_byte_2;
  uint8 reserved_byte_3;

  uint32 storage_type;

} wmsdiag_clear_all_req_type;

typedef struct
{
  diagpkt_subsys_header_type header;
} wmsdiag_clear_all_rsp_type;

/*============================================================================

PACKET   WMS_GET_MESSAGE_LIST_CMD

PURPOSE  Sent from the DM to the DMSS to return a list of all messages in all
         memory stores.

RESPONSE The DM sends back a wmsdiag_get_message_list_rsp_type packet that
         contains a count of how many messages there were and an index, tag and
         memory store type for each available message.

============================================================================*/
typedef struct
{
  diagpkt_subsys_header_type header;

  uint8 transaction_ID;
  uint8 reserved_byte_1;
  uint8 reserved_byte_2;
  uint8 reserved_byte_3;

  uint32 storage_type;

} wmsdiag_get_message_list_req_type;

typedef struct
{
  diagpkt_subsys_header_type header;

  uint32 num_entries;
  uint32 storage_type;
  uint32 voice_mail_index;
  uint32 voice_mail_active;
  uint32 status;

  WMSDIAG_MessageListEntryType messages [ WMSDIAG_MESSAGE_LIST_MAX ];

} wmsdiag_get_message_list_rsp_type;


/*============================================================================

PACKET   WMS_READ_CMD

PURPOSE  Sent from the DM to the DMSS to return a specific message.

RESPONSE The DM sends back a wmsdiag_read_rsp_type packet

============================================================================*/
typedef struct
{
  diagpkt_subsys_header_type header;

  uint8 transaction_ID;
  uint8 reserved_byte_1;
  uint8 reserved_byte_2;
  uint8 reserved_byte_3;

  uint32 storage_type;
  uint32 index;

} wmsdiag_read_req_type;

typedef struct
{
  diagpkt_subsys_header_type header;

  uint32 status;

} wmsdiag_read_rsp_type;

/*  This struct is used by the WMS interface to construct logs for messages */

typedef struct
{

  log_hdr_type header;

  uint8 msg_mode;
  uint8 transaction_ID;
  uint8 status;
  uint8 reserved_byte_1;

  uint32 tag_type;
  uint32 storage_type;
  uint32 index;
  uint32 teleservice_type;

  uint8 addr_digit_mode;
  uint8 addr_num_mode;
  uint8 addr_num_type;
  uint8 addr_num_plan_type;

  uint8 subaddr_type;
  uint8 data_format;
  uint8 tpdu_type;
  uint8 is_broadcast;

  uint8 subaddr_odd_or_even;
  uint8 tl_ack_needed;
  uint8 service_present;
  uint8 is_MO;

  uint32 service_type;

  uint16 addr_num_digits;
  uint16 subaddr_num_digits;

  uint32 user_data_length;

  /*  This is actually of variable length equal to addr_num_digits + subaddr_num_digits + data_length */
  uint8 data [WMSDIAG_DEFAULT_DATA_BUF_MAX];

} wmsdiag_read_log_type;

/*============================================================================

PACKET   WMS_SET_MESSAGE_TAG_CMD

PURPOSE  Sent from the DM to the DMSS to set the tag of a specific message

RESPONSE The DM sends back a wmsdiag_set_message_tag_rsp_type packet to
         show that it attempted to set the message tag.

============================================================================*/
typedef struct
{
  diagpkt_subsys_header_type header;

  uint8 transaction_ID;
  uint8 reserved_byte_1;
  uint8 reserved_byte_2;
  uint8 reserved_byte_3;

  uint32 storage_type;
  uint32 index;
  uint32 tag_type;

} wmsdiag_set_message_tag_req_type;

typedef struct
{
  diagpkt_subsys_header_type header;
} wmsdiag_set_message_tag_rsp_type;

/*============================================================================

PACKET   WMS_SET_ROUTES_CMD

PURPOSE  Sent from the DM to the DMSS to set the routes for message class

RESPONSE The DM sends back a wmsdiag_set_routes_rsp_type packet to
         show that it attempted to set the message routes.

============================================================================*/
typedef struct
{
  diagpkt_subsys_header_type header;

  uint8 transaction_ID;
  uint8 transfer_status_report;
  uint8 reserved_byte_2;
  uint8 reserved_byte_3;

  uint32 wmsdiag_msg_class0_route_pp;
  uint32 wmsdiag_msg_class0_storage_pp;

  uint32 wmsdiag_msg_class1_route_pp;
  uint32 wmsdiag_msg_class1_storage_pp;

  uint32 wmsdiag_msg_class2_route_pp;
  uint32 wmsdiag_msg_class2_storage_pp;

  uint32 wmsdiag_msg_class3_route_pp;
  uint32 wmsdiag_msg_class3_storage_pp;

  uint32 wmsdiag_msg_class_none_route_pp;
  uint32 wmsdiag_msg_class_none_storage_pp;

  uint32 wmsdiag_msg_class_cdma_route_pp;
  uint32 wmsdiag_msg_class_cdma_storage_pp;

  uint32 wmsdiag_msg_class0_route_bc;
  uint32 wmsdiag_msg_class0_storage_bc;

  uint32 wmsdiag_msg_class1_route_bc;
  uint32 wmsdiag_msg_class1_storage_bc;

  uint32 wmsdiag_msg_class2_route_bc;
  uint32 wmsdiag_msg_class2_storage_bc;

  uint32 wmsdiag_msg_class3_route_bc;
  uint32 wmsdiag_msg_class3_storage_bc;

  uint32 wmsdiag_msg_class_none_route_bc;
  uint32 wmsdiag_msg_class_none_storage_bc;

  uint32 wmsdiag_msg_class_cdma_route_bc;
  uint32 wmsdiag_msg_class_cdma_storage_bc;

} wmsdiag_set_routes_req_type;

typedef struct
{
  diagpkt_subsys_header_type header;

  uint32 status;

} wmsdiag_set_routes_rsp_type;

/*  This struct is used by the WMS interface to construct logs for messages */

typedef struct
{

  log_hdr_type header;

  uint8 transaction_ID;
  uint8 status;
  uint8 reserved_byte_1;
  uint8 reserved_byte_2;


} wmsdiag_set_routes_log_type;


/*============================================================================

PACKET   WMS_ORIG_END_SMS_CALL_CMD

PURPOSE  Sent from the DM to the DMSS to originate or end a SMS call.  For
         origination the service option may be specified.

RESPONSE The DM sends back a wmsdiag_sms_call_rsp_type packet to indicate that
         it has attempted to originate or end a sms call.

============================================================================*/
typedef struct
{
  diagpkt_subsys_header_type header;

  uint8 transaction_ID;
  uint8 reserved_byte_1;
  uint8 reserved_byte_2;
  uint8 reserved_byte_3;

  uint32  is_orig;
  uint32  so;
} wmsdiag_orig_end_sms_call_req_type;

typedef struct
{
  diagpkt_subsys_header_type header;
} wmsdiag_orig_end_sms_call_rsp_type;

/*============================================================================

PACKET   WMS_AUTO_DISCONNECT_CMD

PURPOSE  Sent from the DM to the DMSS to enable or disable auto disconnect of
         a SMS call.  If enabling, specified time of auto disconnct is req'd.

RESPONSE The DM sends back a wmsdiag_auto_disconnect_rsp_type packet to indicate
         that it has attempted to enable or disable auto disconnect.

============================================================================*/
typedef struct
{
  diagpkt_subsys_header_type header;

  uint8 transaction_ID;
  uint8 reserved_byte_1;
  uint8 reserved_byte_2;
  uint8 reserved_byte_3;

  uint32  enable_auto_disconnect;
  uint32  timeout;
} wmsdiag_enable_disable_auto_disconnect_req_type;

typedef struct
{
  diagpkt_subsys_header_type header;
} wmsdiag_enable_disable_auto_disconnect_rsp_type;

/*============================================================================

PACKET   WMSDIAG_BROADCAST_ADD_DELETE_SERVICEID_CMD 

PURPOSE  Sent from the DM to the DMSS to Add / Delete a Broadcast Service ID.

RESPONSE The DM sends back an immediate and delayed response packets to 
         indicate success or failure.

============================================================================*/
typedef struct
{
  diagpkt_subsys_header_type header;

  uint8 transaction_ID;
  uint8 msg_mode;
  uint8 add_or_delete_serviceid;
  uint8 language;

  uint16 service_category;
  uint16 serviceid_from;

  uint16 serviceid_to;
  uint8 is_selected;
  uint8 priority;

  uint8 label_encoding;
  uint8 alert_option;
  uint8 max_messages;
  uint8 reserved_byte1;

  uint8 label[30];

} wmsdiag_broadcast_add_delete_serviceid_req_type;

typedef struct
{
  diagpkt_subsys_header_v2_type header;

  uint8 transaction_ID;
  uint8 reserved_byte1;
  uint8 reserved_byte2;
  uint8 reserved_byte3;

} wmsdiag_broadcast_add_delete_serviceid_rsp_type;

#ifdef FEATURE_IWMSDIAG

/*============================================================================

PACKET   WMSAPPDIAG_CB_SAVE_CMD

PURPOSE  Sent from the DM to the DMSS to save a Cell Broadcast Message.

RESPONSE The DM sends back an immediate and delayed response packets to 
         indicate success or failure.

============================================================================*/
typedef struct
{
  diagpkt_subsys_header_type header;

  uint8 transaction_ID;
  uint8 reserved_byte1;
  uint16 ram_index;

} wmsappdiag_cb_save_req_type;

typedef struct
{
  diagpkt_subsys_header_v2_type header;
  
  uint8 transaction_ID;
  uint8 reserved_byte1;
  uint16 efs_index;
  
} wmsappdiag_cb_save_rsp_type;

/*============================================================================

PACKET   WMSAPPDIAG_CB_SAVE_CMD

PURPOSE  Sent from the DM to the DMSS to read a Cell Broadcast Message.

RESPONSE The DM sends back an immediate and delayed response packets to 
         indicate success or failure.

============================================================================*/
typedef struct
{
  diagpkt_subsys_header_type header;

  uint32 transaction_ID;
  uint16 wmsapp_storage_type;
  uint16 index;

} wmsappdiag_cb_read_req_type;

typedef struct
{
  diagpkt_subsys_header_v2_type header;

  uint32 transaction_ID;

  uint8 time_year;
  uint8 time_month;
  uint8 time_day;
  uint8 time_hour;

  uint8 time_minute;
  uint8 time_second;
  uint8 reserved_byte1;
  uint8 reserved_byte2;

  uint8 cb_data[WMSAPPDIAG_MAX_CB_DATA_SIZE];

} wmsappdiag_cb_read_rsp_type;

#endif /* FEATURE_IWMSDIAG */

/*============================================================================

PACKET   WMSDIAG_BROADCAST_DELETE_ALL_SERVICES_CMD

PURPOSE  Sent from the DM to the DMSS to Delete All Broadcast Services

RESPONSE The DM sends back an immediate and delayed response packets to 
         indicate success or failure.

============================================================================*/
typedef struct
{
  diagpkt_subsys_header_type header;

  uint8 transaction_ID;
  uint8 msg_mode;
  uint8 reserved_byte1;
  uint8 reserved_byte2;
  
} wmsdiag_broadcast_delete_all_services_req_type;

typedef struct
{
  diagpkt_subsys_header_v2_type header;

  uint8 transaction_ID;
  uint8 reserved_byte1;
  uint8 reserved_byte2;
  uint8 reserved_byte3;

} wmsdiag_broadcast_delete_all_services_rsp_type;


#ifdef FEATURE_IWMSDIAG

/*============================================================================

PACKET   WMSAPPDIAG_CB_DELETE_MSG_CMD

PURPOSE  Sent from the DM to the DMSS to delete a Cell Broadcast Message.

RESPONSE The DM sends back an immediate and delayed response packets to 
         indicate success or failure.

============================================================================*/
typedef struct
{
  diagpkt_subsys_header_type header;

  uint32 transaction_ID;
  uint32 wmsapp_storage_type;
  uint32 index;

} wmsappdiag_cb_delete_msg_req_type;

typedef struct
{
  diagpkt_subsys_header_v2_type header;
  
  uint32 transaction_ID;

} wmsappdiag_cb_delete_msg_rsp_type;


/*============================================================================

PACKET   WMSAPPDIAG_CB_DELETE_ALL_MSGS_CMD

PURPOSE  Sent from the DM to the DMSS to delete all the Cell Broadcast Messages.

RESPONSE The DM sends back an immediate and delayed response packets to 
         indicate success or failure.

============================================================================*/
typedef struct
{
  diagpkt_subsys_header_type header;

  uint32 transaction_ID;
  uint32 wmsapp_storage_type;

} wmsappdiag_cb_delete_all_msgs_req_type;

typedef struct
{
  diagpkt_subsys_header_v2_type header;
  
  uint32 transaction_ID;

} wmsappdiag_cb_delete_all_msgs_rsp_type;

#endif /* FEATURE_IWMSDIAG */


/*============================================================================

PACKET   WMSDIAG_GET_BEARER_PREF_CMD

PURPOSE  Sent from the DM to the DMSS to Get the SMS Bearer Preference.

RESPONSE The DM sends back an immediate and delayed response packets to 
         indicate success or failure.

============================================================================*/
typedef struct
{
  diagpkt_subsys_header_type header;

  uint32 transaction_ID;

} wmsdiag_get_bearer_pref_req_type;

typedef struct
{
  diagpkt_subsys_header_v2_type header;
  
  uint32 transaction_ID;

  uint32 sms_domain_pref;

} wmsdiag_get_bearer_pref_rsp_type;


/*============================================================================

PACKET   WMSDIAG_SET_BEARER_PREF_CMD

PURPOSE  Sent from the DM to the DMSS to Set the SMS Bearer Preference.

RESPONSE The DM sends back an immediate and delayed response packets to 
         indicate success or failure.

============================================================================*/
typedef struct
{
  diagpkt_subsys_header_type header;

  uint32 transaction_ID;

  uint32 sms_domain_pref;

} wmsdiag_set_bearer_pref_req_type;

typedef struct
{
  diagpkt_subsys_header_v2_type header;
  
  uint32 transaction_ID;

} wmsdiag_set_bearer_pref_rsp_type;


/*===========================================================================
FUNCTION wmsdiag_init

DESCRIPTION
  Initialize the WMS Diag Module

DEPENDENCIES
  none

RETURN VALUE
  None.

SIDE EFFECTS
  None.
===========================================================================*/
void wmsdiag_init(void);

#endif /* if defined(FEATURE_CDSMS) || defined(FEATURE_GWSMS) */
#endif /* FEATURE_WMS_DIAG */

#endif  /*  WMSDIAG_H */
