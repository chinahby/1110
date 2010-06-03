/*****************************************************************************
***
***  TITLE
***  MT OBEX API Header
***
***
***  DESCRIPTION
***  This contains the Interface to a Multi Transport AMSS OBEX Implementation.
***
***
***  Copyright (c) 2004-2008 QUALCOMM Incorporated.
***  All Rights Reserved.
***  Qualcomm Confidential and Proprietary
***
***  Export of this technology or software is regulated by the U.S.
***  Government. Diversion contrary to U.S. law prohibited.
***
*****************************************************************************/


/*****************************************************************************
***
***                        EDIT HISTORY FOR MODULE
***
***  This section contains comments describing changes made to the module.
***  Notice that changes are listed in reverse chronological order.
***
***  $Header: //source/qcom/qct/wconnect/bthost/pf/rel/00.00.26/inc/mt_obex.h#1 $
***
***  when       who     what, where, why
***  --------   ---     ----------------------------------------------------------
***  08-09-15   pr      Fix compiler warnings
***  2008-05-27 rb      Fix compiler warnings

***  21/03/07   MH      Realm string type added to realm structure.
***  16/02/07   MH      Added support for disconnect response functionality.
***  29/12/07   MH      Added support for 128-bit UUID OBEX server.
***  26/10/06   MH      Modified server confirm set path to support all headers
***  25/09/06   MH      Added new function to get client BD address from 
***                     server handle.
***  19/09/06   MH      Client response Call back event and Server Auth function added.
***  25/08/06   BK      Fixed comple issues 
***  29/07/06   BK      Modified to support OBEX client initiated authentication 
***  10/07/06   JH      Removed extra linefeeds introduced in previous checkin.
***  23/06/06   JH      Modified to support updated GOEP API for JSR82.
***  24/11/04   ABu     Added Interface
***  25/01/05   ABu     Added FEATURE_MT_OBEX flag around all mt_obex code
***  14/04/05   ABu     Added mt_obex_srv_register_secondary_target()
***
*****************************************************************************/

#ifndef _MT_OBEX_H
#define _MT_OBEX_H

#ifdef FEATURE_BT
#ifdef FEATURE_BT_EXTPF_GOEP
#ifdef FEATURE_MT_OBEX

/*****************************************************************************
***
***     Included Files
***
*****************************************************************************/

#include "customer.h"
#include "comdef.h"
#include "bt.h"

/*****************************************************************************
***
***     Public Defines & Constants
***
*****************************************************************************/

/* these are renamed here so as to keep callers of this API generic and not have to
   deal with any Bluetooth specific parameters */
#define app_id                              bt_app_id
#define mt_obex_app_id_type                 bt_app_id_type
#define mt_obex_cli_conn_id_type            bt_pf_goep_cli_conn_id_type
#define mt_obex_srv_conn_id_type            bt_pf_goep_srv_conn_id_type
#define mt_obex_byte_seq_type               bt_pf_byte_seq_type
#define mt_obex_unicode_str_type            bt_pf_unicode_str_type
#define mt_obex_hdr_type                    bt_pf_goep_obex_hdr_type
#define mt_obex_hdr_list_type               bt_pf_goep_obex_hdr_list_type
#define PACKED                              __packed
//#define mt_obex_status_type                 bt_cmd_status_type
#define mt_obex_sd_uuid_type                bt_sd_uuid_type
#define MT_OBEX_NO_CONN_ID                  0xFFFF
#define MT_OBEX_MAX_REALM_LEN               255

/* event types re-named for the purposes of IOBEX in BREW */
#define mt_obex_ev_cli_con_cfm_type         bt_pf_ev_goep_cli_con_cfm_type
#define mt_obex_ev_cli_dcn_ind_type         bt_pf_ev_goep_cli_dcn_ind_type
#define mt_obex_ev_cli_put_cfm_type         bt_pf_ev_goep_cli_put_cfm_type
#define mt_obex_ev_cli_get_rcv_data_type    bt_pf_ev_goep_cli_get_rcv_data_type
#define mt_obex_ev_cli_set_path_cfm_type    bt_pf_ev_goep_cli_set_path_cfm_type
#define mt_obex_ev_cli_auth_ind_type        bt_pf_ev_goep_cli_auth_ind_type
#define mt_obex_ev_cli_auth_res_ind_type    bt_pf_ev_goep_cli_auth_res_ind_type
#define mt_obex_ev_srv_con_ind_type         bt_pf_ev_goep_srv_con_ind_type
#define mt_obex_ev_srv_dcn_ind_type         bt_pf_ev_goep_srv_dcn_ind_type
#define mt_obex_ev_srv_get_ind_type         bt_pf_ev_goep_srv_get_ind_type
#define mt_obex_ev_srv_put_ind_type         bt_pf_ev_goep_srv_put_ind_type
#define mt_obex_ev_srv_set_path_ind_type    bt_pf_ev_goep_srv_set_path_ind_type

#define ev_mt_obex_cli_con_cfm              ev_goep_cli_con_cfm
#define ev_mt_obex_cli_dcn_ind              ev_goep_cli_dcn_ind
#define ev_mt_obex_cli_put_cfm              ev_goep_cli_put_cfm
#define ev_mt_obex_cli_get_rcv_data         ev_goep_cli_get_rcv_data
#define ev_mt_obex_cli_set_path_cfm         ev_goep_cli_set_path_cfm
#define ev_mt_obex_cli_auth_ind             ev_goep_cli_auth_ind

#define ev_mt_obex_srv_con_ind              ev_goep_srv_con_ind
#define ev_mt_obex_srv_dcn_ind              ev_goep_srv_dcn_ind
#define ev_mt_obex_srv_get_ind              ev_goep_srv_get_ind
#define ev_mt_obex_srv_put_ind              ev_goep_srv_put_ind
#define ev_mt_obex_srv_set_path_ind         ev_goep_srv_set_path_ind


/* event Names re-named for the purposes of IOBEX in BREW */
#define MT_OBEX_EV_CLI_CON_CFM              BT_EV_PF_GOEP_CLI_CON_CFM
#define MT_OBEX_EV_CLI_DCN_IND              BT_EV_PF_GOEP_CLI_DCN_IND
#define MT_OBEX_EV_CLI_PUT_CFM              BT_EV_PF_GOEP_CLI_PUT_CFM
#define MT_OBEX_EV_CLI_GET_RCV_DATA         BT_EV_PF_GOEP_CLI_GET_RCV_DATA
#define MT_OBEX_EV_CLI_SET_PATH_CFM         BT_EV_PF_GOEP_CLI_SET_PATH_CFM
#define MT_OBEX_EV_CLI_AUTH_IND             BT_EV_PF_GOEP_CLI_AUTH_IND
#define MT_OBEX_EV_CLI_AUTH_RESP_IND        BT_EV_PF_GOEP_CLI_AUTH_RESP_IND
#define MT_OBEX_EV_SRV_CON_IND              BT_EV_PF_GOEP_SRV_CON_IND
#define MT_OBEX_EV_SRV_AUTH_IND             BT_EV_PF_GOEP_SRV_AUTH_IND
#define MT_OBEX_EV_SRV_DCN_IND              BT_EV_PF_GOEP_SRV_DCN_IND
#define MT_OBEX_EV_SRV_GET_IND              BT_EV_PF_GOEP_SRV_GET_IND
#define MT_OBEX_EV_SRV_PUT_IND              BT_EV_PF_GOEP_SRV_PUT_IND
#define MT_OBEX_EV_SRV_SET_PATH_IND         BT_EV_PF_GOEP_SRV_SET_PATH_IND

#define MT_OBEX_OK                          MT_OBEX_SUCCESS
#define MT_STATUS_OUT_OF_MEMORY             OI_STATUS_OUT_OF_MEMORY


typedef enum
{
    MT_OBEX_PENDING = BT_CS_GN_PENDING,
    MT_OBEX_SUCCESS = BT_CS_GN_SUCCESS,
    MT_OBEX_BAD_CMD_STATE = BT_CS_GN_BAD_CMD_STATE,
    MT_OBEX_CMD_Q_FULL = BT_CS_GN_CMD_Q_FULL,
    MT_OBEX_BAD_APP_ID = BT_CS_GN_BAD_APP_ID,
    MT_OBEX_UNRECOGNIZED_CMD = BT_CS_GN_UNRECOGNIZED_CMD,
    MT_OBEX_MAX_CMD_RETRIES = BT_CS_GN_MAX_CMD_RETRIES,
    MT_OBEX_RETRY_CMD_LATER = BT_CS_GN_RETRY_CMD_LATER,
    MT_OBEX_UNSUPPORTED_TP  = (MT_OBEX_RETRY_CMD_LATER+1),

#ifdef FEATURE_BT_EXTPF
    MT_OBEX_CONTINUE = BT_CS_PF_OBEX_CONTINUE,
    MT_OBEX_COMMAND_ERROR = BT_CS_PF_OBEX_COMMAND_ERROR,
    MT_OBEX_CONNECTION_TIMEOUT = BT_CS_PF_OBEX_CONNECTION_TIMEOUT,
    MT_OBEX_CONNECT_FAILED = BT_CS_PF_OBEX_CONNECT_FAILED,
    MT_OBEX_DISCONNECT_FAILED = BT_CS_PF_OBEX_DISCONNECT_FAILED,
    MT_OBEX_ERROR = BT_CS_PF_OBEX_ERROR,
    MT_OBEX_INCOMPLETE_PACKET = BT_CS_PF_OBEX_INCOMPLETE_PACKET,
    MT_OBEX_LENGTH_REQUIRED = BT_CS_PF_OBEX_LENGTH_REQUIRED,
    MT_OBEX_NOT_CONNECTED = BT_CS_PF_OBEX_NOT_CONNECTED,
    MT_OBEX_NO_MORE_CONNECTIONS = BT_CS_PF_OBEX_NO_MORE_CONNECTIONS,
    MT_OBEX_OPERATION_IN_PROGRESS = BT_CS_PF_OBEX_OPERATION_IN_PROGRESS,
    MT_OBEX_PUT_RESPONSE_ERROR = BT_CS_PF_OBEX_PUT_RESPONSE_ERROR,
    MT_OBEX_GET_RESPONSE_ERROR = BT_CS_PF_OBEX_GET_RESPONSE_ERROR,
    MT_OBEX_REQUIRED_HEADER_NOT_FOUND = BT_CS_PF_OBEX_REQUIRED_HEADER_NOT_FOUND,
    MT_OBEX_SERVICE_UNAVAILABLE = BT_CS_PF_OBEX_SERVICE_UNAVAILABLE,
    MT_OBEX_TOO_MANY_HEADER_BYTES = BT_CS_PF_OBEX_TOO_MANY_HEADER_BYTES,
    MT_OBEX_UNKNOWN_COMMAND = BT_CS_PF_OBEX_UNKNOWN_COMMAND,
    MT_OBEX_UNSUPPORTED_VERSION = BT_CS_PF_OBEX_UNSUPPORTED_VERSION,
    MT_OBEX_CLIENT_ABORTED_COMMAND = BT_CS_PF_OBEX_CLIENT_ABORTED_COMMAND,
    MT_OBEX_BAD_PACKET = BT_CS_PF_OBEX_BAD_PACKET,
    MT_OBEX_BAD_REQUEST = BT_CS_PF_OBEX_BAD_REQUEST,
    MT_OBEX_OBJECT_OVERFLOW = BT_CS_PF_OBEX_OBJECT_OVERFLOW,
    MT_OBEX_NOT_FOUND = BT_CS_PF_OBEX_NOT_FOUND,
    MT_OBEX_ACCESS_DENIED = BT_CS_PF_OBEX_ACCESS_DENIED,
    MT_OBEX_VALUE_NOT_ACCEPTABLE = BT_CS_PF_OBEX_VALUE_NOT_ACCEPTABLE,
    MT_OBEX_PACKET_OVERFLOW = BT_CS_PF_OBEX_PACKET_OVERFLOW,
    MT_OBEX_NO_SUCH_FOLDER = BT_CS_PF_OBEX_NO_SUCH_FOLDER,
    MT_OBEX_NAME_REQUIRED = BT_CS_PF_OBEX_NAME_REQUIRED,
    MT_OBEX_PASSWORD_TOO_LONG = BT_CS_PF_OBEX_PASSWORD_TOO_LONG,
    MT_OBEX_PRECONDITION_FAILED = BT_CS_PF_OBEX_PRECONDITION_FAILED,
    MT_OBEX_UNAUTHORIZED = BT_CS_PF_OBEX_UNAUTHORIZED,
    MT_OBEX_NOT_IMPLEMENTED = BT_CS_PF_OBEX_NOT_IMPLEMENTED,
    MT_OBEX_INVALID_AUTH_DIGEST = BT_CS_PF_OBEX_INVALID_AUTH_DIGEST,
    MT_OBEX_INVALID_OPERATION = BT_CS_PF_OBEX_INVALID_OPERATION,
    MT_OBEX_DATABASE_FULL = BT_CS_PF_OBEX_DATABASE_FULL,
    MT_OBEX_DATABASE_LOCKED = BT_CS_PF_OBEX_DATABASE_LOCKED,
    MT_OBEX_INTERNAL_SERVER_ERROR = BT_CS_PF_OBEX_INTERNAL_SERVER_ERROR,
    MT_OBEX_UNSUPPORTED_MEDIA_TYPE = BT_CS_PF_OBEX_UNSUPPORTED_MEDIA_TYPE,
    MT_OBEX_PARTIAL_CONTENT = BT_CS_PF_OBEX_PARTIAL_CONTENT,
    MT_OBEX_METHOD_NOT_ALLOWED = BT_CS_PF_OBEX_METHOD_NOT_ALLOWED,
    MT_OBEX_OBEXSRV_INCOMPLETE_GET = BT_CS_PF_OBEXSRV_INCOMPLETE_GET,
    MT_OBEX_FOLDER_BROWSING_NOT_ALLOWED = BT_CS_PF_OBEX_FOLDER_BROWSING_NOT_ALLOWED,
    MT_OBEX_SERVER_FORCED_DISCONNECT = BT_CS_PF_OBEX_SERVER_FORCED_DISCONNECT,
    MT_OBEX_NOT_MODIFIED = BT_CS_PF_OBEX_NOT_MODIFIED,
    MT_OBEX_GONE = BT_CS_PF_OBEX_GONE,
    MT_OBEX_REQ_TIMEOUT = BT_CS_PF_OBEX_REQ_TIMEOUT,
    MT_OBEX_CONNECTION_NOT_FOUND = BT_CS_PF_OBEX_CONNECTION_NOT_FOUND,
    MT_OBEX_OUT_OF_MEMORY = BT_CS_PF_OUT_OF_MEMORY,
    MT_OBEX_INVALID_PARAMETERS = BT_CS_PF_INVALID_PARAMETERS,
#endif /* FEATURE_BT_EXTPF */
    MT_OBEX_LAST
} mt_obex_status_type;

/* Header Types */
#define MT_OBEX_HDR_COUNT                       OI_OBEX_HDR_COUNT /**< number of objects (used by Connect) */
#define MT_OBEX_HDR_NAME                        OI_OBEX_HDR_NAME  /**< name of the object (often a file name) */
#define MT_OBEX_HDR_TYPE                        OI_OBEX_HDR_TYPE  /**< type of object (e.g., text, HTML, binary, manufacturer-specific) */
#define MT_OBEX_HDR_LENGTH                      OI_OBEX_HDR_LENGTH /**< the length of the object in bytes */
#define MT_OBEX_HDR_TIME                        OI_OBEX_HDR_TIME   /**< date/time stamp (ISO 8601) */
#define MT_OBEX_HDR_DESCRIPTION                 OI_OBEX_HDR_DESCRIPTION /**< text description of the object */
#define MT_OBEX_HDR_TARGET                      OI_OBEX_HDR_TARGET  /**< name of service to which operation is targeted */
#define MT_OBEX_HDR_HTTP                        OI_OBEX_HDR_HTTP    /**< HTTP 1.x header */
#define MT_OBEX_HDR_BODY                        OI_OBEX_HDR_BODY    /**< a chunk of the object body. */
#define MT_OBEX_HDR_END_OF_BODY                 OI_OBEX_HDR_END_OF_BODY /**< the final chunk of the object body */
#define MT_OBEX_HDR_WHO                         OI_OBEX_HDR_WHO           /**< identifies the OBEX application; used to tell if talking to a peer */
#define MT_OBEX_HDR_CONNECTION_ID               OI_OBEX_HDR_CONNECTION_ID /**< an identifier used for OBEX connection multiplexing */
#define MT_OBEX_HDR_APPLICATION_PARAMS          OI_OBEX_HDR_APPLICATION_PARAMS        /**< extended application request and response information */
#define MT_OBEX_HDR_AUTHENTICATION_CHALLENGE    OI_OBEX_HDR_AUTHENTICATION_CHALLENGE  /**< authentication digest-challenge */
#define MT_OBEX_HDR_AUTHENTICATION_RESPONSE     OI_OBEX_HDR_AUTHENTICATION_RESPONSE   /**< authentication digest-response */
#define MT_OBEX_HDR_CREATOR_ID                  OI_OBEX_HDR_CREATOR_ID                /**< identifies the creator of object */
#define MT_OBEX_HDR_OBJECT_CLASS                OI_OBEX_HDR_OBJECT_CLASS              /**< OBEX object class of object */

/* MIME types used by OBEX */
#define MT_OBEX_FOLDER_LISTING_TYPE             OI_OBEX_FOLDER_LISTING_TYPE /* "x-obex/folder-listing" */
#define MT_OBEX_ICALENDAR_TYPE                  OI_OBEX_ICALENDAR_TYPE      /* "text/calendar */
#define MT_OBEX_VCALENDAR_TYPE                  OI_OBEX_VCALENDAR_TYPE      /* text/x-vcalendar */
#define MT_OBEX_VCARD_TYPE                      OI_OBEX_VCARD_TYPE          /* text/x-vcard */
#define MT_OBEX_VNOTE_TYPE                      OI_OBEX_VNOTE_TYPE          /* text/x-vnote */
#define MT_OBEX_VMESSAGE_TYPE                   OI_OBEX_VMESSAGE_TYPE       /* text/x-vmsg */
#define MT_OBEX_UPF_TYPE                        OI_OBEX_UPF_TYPE            /* image/x-UPF */
#define MT_OBEX_JPEG_TYPE                       OI_OBEX_JPEG_TYPE           /* image/jpeg */
#define MT_OBEX_TEXT_TYPE                       OI_OBEX_TEXT_TYPE           /* text/plain */
#define MT_OBEX_ICALENDAR_SUFFIX                OI_OBEX_ICALENDAR_SUFFIX    /* .ics */
#define MT_OBEX_VCALENDAR_SUFFIX                OI_OBEX_VCALENDAR_SUFFIX    /* .vcs */
#define MT_OBEX_VCARD_SUFFIX                    OI_OBEX_VCARD_SUFFIX        /* .vcf */
#define MT_OBEX_VNOTE_SUFFIX                    OI_OBEX_VNOTE_SUFFIX        /* .vnt */
#define MT_OBEX_VMESSAGESUFFIX                  OI_OBEX_VMESSAGESUFFIX      /* .vmg */

/* need for applications to define their target UUID's */
#define MT_OBEX_UUID_SIZE                   OI_OBEX_UUID_SIZE
#define MT_OBEX_ARRAYSIZE(a)                (sizeof(a)/sizeof(a[0]))

/* multi transport server bit definitions, used to setup and configure OBEX
   server transports */
#define MT_OBEX_BLUETOOTH_TP 0x01
#define MT_OBEX_IRDA_TP      0x02
#define MT_OBEX_USB_TP       0x04
#define MT_OBEX_RS232_TP     0x08

/*****************************************************************************
***
***     Public Type Definitions
***
*****************************************************************************/

/* multi transport types needed */
typedef uint8 mt_obex_tp_support_type;
typedef uint8 mt_obex_tp_config_type;
typedef void* mt_obex_obj_handle_type;

typedef enum {
    MT_OBEX_FIRST_PACKET_STATE,
    MT_OBEX_CONTINUE_STATE
} mt_obex_multi_segment_state_type;

typedef enum
{
    MT_OBEX_SRV_AUTH_NONE = BT_PF_GOEP_SRV_AUTH_NONE,
    MT_OBEX_SRV_AUTH_PASSWORD = BT_PF_GOEP_SRV_AUTH_PASSWORD,
    MT_OBEX_SRV_AUTH_USERID_AND_PASSWORD = BT_PF_GOEP_SRV_AUTH_USERID_AND_PASSWORD
} mt_obex_srv_auth_type;

typedef enum
{
  MT_OBEX_SEC_NONE = BT_SEC_NONE,
  MT_OBEX_SEC_AUTHENTICATE = BT_SEC_AUTHENTICATE,
  MT_OBEX_AUTHENTICATE_AND_ENCRYPT = BT_SEC_AUTHENTICATE_AND_ENCRYPT,
  MT_OBEX_SEC_MAX = BT_SEC_MAX
} mt_obex_security_type;

/* below is for clients where an application can only have an OBEX client to a
   single transport */
typedef enum
{
    MT_OBEX_BLUETOOTH_TRANSPORT,
    MT_OBEX_IRDA_TRANSPORT,
    MT_OBEX_USB_TRANSPORT,
    MT_OBEX_RS232_TRANSPORT
} mt_obex_transport_type;

/* For Client Connection Purposes
   connection structs that can pass in connection parameters specific to particular
   transports
 */
typedef struct
{
    bt_bd_addr_type*        bd_addr_ptr;
    uint8                   channel;
    mt_obex_sd_uuid_type    uuid;
    mt_obex_security_type   security;
    /* etc */
} mt_obex_bt_conn_struct_type;

typedef struct
{
    uint32 irda_device_addr;
    const uint8 *ias_query;
    uint8 ias_query_len;
    /* etc */
} mt_obex_irda_conn_struct_type;


/* For Server Registration Purposes
   configuration structs that can tell the transports to prepare by adding
   service discovery or security settings to their databases etc
*/
typedef struct
{
    mt_obex_sd_uuid_type    uuid;
    mt_obex_security_type   security;
    /* This is needed to indicate that the APP needs a different event type
     * for AUTH_REQ. The same should not be combined with
     * the connection indication.This is a modification for the Java APPs. */
    boolean                 authRequired;    /* etc */                           
} mt_obex_bt_config_struct_type;

typedef union
{
    mt_obex_bt_conn_struct_type    bt_conn;
    mt_obex_irda_conn_struct_type  irda_conn;
} mt_obex_transport_conn_union_type;

typedef struct
{
    mt_obex_tp_config_type         transports;
    mt_obex_bt_config_struct_type  bt_config;
} mt_obex_tp_config_struct_type;

typedef struct
{
    mt_obex_transport_type transport;
    mt_obex_transport_conn_union_type transport_conn;
} mt_obex_tp_conn_struct_type;

typedef struct {
    uint8 realmStr[MT_OBEX_MAX_REALM_LEN];
    uint8 len;
    uint8 realmStrType;
}mt_realm_info;

/* Definition for a 128-bit UUID */
typedef struct
{
  uint8     uuid_byte[ 16 ];
} mt_obex_uuid128_type;

/*****************************************************************************
***
***     Public Data References
***
*****************************************************************************/

/*****************************************************************************
***
***     Public Macros
***
*****************************************************************************/

/*****************************************************************************
***
***     Public Functions
***
*****************************************************************************/

/************************ Generic Functions *********************************/

/*===========================================================================

FUNCTION mt_obex_get_transports_supported

DESCRIPTION
  This function can be used by an application to find out what transports are
  supported for OBEX to run on.

DEPENDENCIES
  None

RETURN VALUE
  None

SIDE EFFECTS
  None

===========================================================================*/
mt_obex_tp_support_type mt_obex_get_transports_supported(void);


/************************ client functions *********************************/
/*===========================================================================

FUNCTION mt_obex_client_init_data

DESCRIPTION
  Initializes the client authentication data.

DEPENDENCIES
  None

RETURN VALUE
  General Status of the command that has been issued to OBEX.

SIDE EFFECTS
  None

===========================================================================*/

void mt_obex_client_init_data (uint16 loc);


/*===========================================================================

FUNCTION mt_obex_client_authenticate

DESCRIPTION
  This command indicates OBEX Client to initiate the authentication procedure.

DEPENDENCIES
  None

RETURN VALUE
  General Status of the command that has been issued to OBEX.

SIDE EFFECTS
  None

===========================================================================*/

mt_obex_status_type mt_obex_client_authenticate(
    mt_obex_tp_conn_struct_type         *transport_params,
    mt_realm_info                       *realm_info_ptr,
    boolean                             user_id,
    boolean                             full_access);



/*===========================================================================

FUNCTION mt_obex_server_authenticate

DESCRIPTION
  This command indicates OBEX to authenticate the client, the OBEX authenticates 
  the incoming connection if this command is been issued.  
  
DEPENDENCIES
  None

RETURN VALUE
  General Status of the command that has been issued to OBEX.

SIDE EFFECTS
  None

===========================================================================*/

mt_obex_status_type mt_obex_server_authenticate(
    mt_obex_tp_conn_struct_type         *transport_params,
    uint16                              conn_id,
    mt_realm_info                       *realm_info_ptr,
    boolean                             user_id,
    boolean                             full_access);


/*===========================================================================

FUNCTION mt_obex_cli_auth_response

DESCRIPTION
  Responds to an authentication indication. This command responds to an
  authentication indication, which was previously indicated to the application
  by the MT_OBEX_EV_CLI_AUTH_IND event. A NULL password indicates that the
  application is rejecting the authentication challenge.

DEPENDENCIES
  None

RETURN VALUE
  General Status of the command that has been issued to OBEX.

SIDE EFFECTS
  None

===========================================================================*/

mt_obex_status_type mt_obex_cli_auth_response(
    mt_obex_app_id_type                  app_id,
    mt_obex_cli_conn_id_type             conn_id,
    byte*                                user_id_ptr,
    uint8                                user_id_len,
    char*                                password_ptr );

/*===========================================================================

FUNCTION mt_obex_cli_connect

DESCRIPTION
  This command connects a client to an OBEX server and thus begins an OBEX session.
  This command establishes a connection to an OBEX server. For Bluetooth this
  includes setting up an RFCOMM connection to the OBEX server TBD. For IrDA
  this request potentially initiates device discovery and service discovery
  and establishes all necessary layers of connections (i.e. IrLAP/IrLMP/TinyTP).
  A MT_OBEX_EV_CLI_CON_CFM event will be generated to indicate if the connection
  is successful.

DEPENDENCIES
  None

RETURN VALUE
  General Status of the command that has been issued to OBEX.

SIDE EFFECTS
  None

===========================================================================*/
mt_obex_status_type mt_obex_cli_connect(
    mt_obex_tp_conn_struct_type          *transport_params,
    mt_obex_app_id_type                  app_id,
    mt_obex_hdr_list_type*               headers_ptr );

/*===========================================================================

FUNCTION mt_obex_cli_disconnect

DESCRIPTION
  This command disconnects a client from an OBEX server and thus ends an OBEX
  session. This command attempts to tear down the specified connection with
  a remote OBEX server.  For IrDA this command will also tear down the underlying
  transport connection if this transport connection was established during the
  mt_obex_cli_connect() call otherwise just the OBEX session will be
  disconnected/terminated. A MT_OBEX_EV_CLI_DCN_IND event will be generated to
  indicate completion of the disconnection.

DEPENDENCIES
  None

RETURN VALUE
  General Status of the command that has been issued to OBEX.

SIDE EFFECTS
  None

===========================================================================*/
mt_obex_status_type mt_obex_cli_disconnect(
    mt_obex_app_id_type                  app_id,
    mt_obex_cli_conn_id_type             conn_id,
    mt_obex_hdr_list_type*               headers_ptr );

/*===========================================================================

FUNCTION mt_obex_cli_put

DESCRIPTION
  Puts data to an OBEX server. The bt_pf_goep_cli_put_cfm_cb() callback
  indicates that the server has received and acknowledged receipt of the
  data and then the MT_OBEX_EV_CLI_PUT_CFM event will be forwarded to the
  application. Headers must not be freed until acknowledged.

DEPENDENCIES
  None

RETURN VALUE
  General Status of the command that has been issued to OBEX.

SIDE EFFECTS
  None

===========================================================================*/
mt_obex_status_type mt_obex_cli_put(
    mt_obex_app_id_type                  app_id,
    mt_obex_cli_conn_id_type             conn_id,
    mt_obex_hdr_list_type*               headers_ptr,
    mt_obex_status_type                  status );

/*===========================================================================

FUNCTION mt_obex_cli_get

DESCRIPTION
  Get data from the OBEX server. This will cause one or more packets to be
  retrieved from an OBEX server. Headers must not be freed until acknowledged.
  A MT_OBEX_EV_CLI_GET_RCV_DATA event is returned to the application when an
  OBEX get request returns data.

DEPENDENCIES
  None

RETURN VALUE
  General Status of the command that has been issued to OBEX.

SIDE EFFECTS
  None

===========================================================================*/
mt_obex_status_type mt_obex_cli_get(
    mt_obex_app_id_type                  app_id,
    mt_obex_cli_conn_id_type             conn_id,
    mt_obex_hdr_list_type*               headers_ptr,
    boolean                              final );

/*===========================================================================

FUNCTION mt_obex_cli_set_path

DESCRIPTION
  This command can be used to set the current folder for getting and putting
  objects and can be used to create new folders. A MT_OBEX_EV_CLI_SET_PATH_CFM
  event will be sent to the application if this request is successful.

DEPENDENCIES
  None

RETURN VALUE
  General Status of the command that has been issued to OBEX.

SIDE EFFECTS
  None

===========================================================================*/
mt_obex_status_type mt_obex_cli_set_path(
    mt_obex_app_id_type                  app_id,
    mt_obex_cli_conn_id_type             conn_id,
    uint16*                              folder_ptr,
    boolean                              up_level,
    boolean                              dont_create,
    mt_obex_hdr_list_type*               headers_ptr );

/*===========================================================================

FUNCTION mt_obex_cli_abort_operation

DESCRIPTION
  Abort the current OBEX operation. Terminate the current put or get operation
  on this connection by replacing the next GET or PUT command by an ABORT command.

DEPENDENCIES
  None

RETURN VALUE
  General Status of the command that has been issued to OBEX.

SIDE EFFECTS
  None

===========================================================================*/
mt_obex_status_type mt_obex_cli_abort_operation(
    mt_obex_app_id_type                  app_id,
    mt_obex_cli_conn_id_type             conn_id );

/************************ server functions *********************************/

/*===========================================================================

FUNCTION mt_obex_srv_register

DESCRIPTION
  Register an OBEX server. This command registers an OBEX server to accept put
  and/or get requests from OBEX clients. This command is returned immediately
  and is not placed on the command queue.

DEPENDENCIES
  None

RETURN VALUE
  General Status of the command that has been issued to OBEX.

SIDE EFFECTS
  None

===========================================================================*/
mt_obex_status_type mt_obex_srv_register(
    mt_obex_app_id_type                  app_id,
    mt_obex_byte_seq_type*               target_ptr,
    mt_obex_srv_auth_type                auth,
    mt_obex_tp_config_struct_type        *tp_config_params,
    uint8*                               ch_num );

/*===========================================================================

FUNCTION mt_obex_srv_register_secondary_target

DESCRIPTION
  Register a second target with an already registered OBEX server. Then this
  server will handle OBEX client connects for both of these targets.

DEPENDENCIES
  None

RETURN VALUE
  General Status of the command that has been issued to OBEX.

SIDE EFFECTS
  None

===========================================================================*/
mt_obex_status_type mt_obex_srv_register_secondary_target(
    mt_obex_app_id_type                  app_id,
    mt_obex_byte_seq_type*               target_ptr,
    mt_obex_srv_auth_type                auth,
    uint8                                ch_num,
    uint8*                               target_id );

/*===========================================================================

FUNCTION mt_obex_srv_accept_connect

DESCRIPTION
  Accept or reject a connect request. Accept or reject a connection from an
  OBEX client when a MT_OBEX_EV_SRV_CON_IND event is received from OBEX.

DEPENDENCIES
  None

RETURN VALUE
  General Status of the command that has been issued to OBEX.

SIDE EFFECTS
  None

===========================================================================*/
mt_obex_status_type mt_obex_srv_accept_connect(
    mt_obex_app_id_type                  app_id,
    uint16                               conn_id,
    boolean                              accept,
    mt_obex_hdr_list_type*               headers_ptr,
    mt_obex_status_type                  status );

/*===========================================================================

FUNCTION mt_obex_srv_confirm_set_path

DESCRIPTION
  Respond to a SETPATH command. Send SETPATH response to OBEX client when a
  MT_OBEX_EV_SRV_SET_PATH_IND event is received from OBEX.

DEPENDENCIES
  None

RETURN VALUE
  General Status of the command that has been issued to OBEX.

SIDE EFFECTS
  None

===========================================================================*/
mt_obex_status_type mt_obex_srv_confirm_set_path(
    mt_obex_app_id_type                  app_id,
    uint16                               conn_id,
    mt_obex_hdr_list_type*               headers_ptr,
    mt_obex_status_type                  status );

/*===========================================================================

FUNCTION mt_obex_srv_auth_response

DESCRIPTION
  Response to a connect indication which reported that the connection
  was unauthorized. If the password is correct and authentication
  completes another connect indication will be issued.

DEPENDENCIES
  None

RETURN VALUE
  General Status of the command that has been issued to OBEX.

SIDE EFFECTS
  None

===========================================================================*/
mt_obex_status_type mt_obex_srv_auth_response(
    mt_obex_app_id_type                  app_id,
    uint16                               conn_id,
    const char*                          password_ptr,
    boolean                              read_only );

/*===========================================================================

FUNCTION mt_obex_srv_get_response

DESCRIPTION
  Respond to a GET command. When application receives a MT_OBEX_EV_SRV_GET_IND
  event this function will respond to a GET command by sending data to a
  connected OBEX client.


DEPENDENCIES
  None

RETURN VALUE
  General Status of the command that has been issued to OBEX.

SIDE EFFECTS
  None

===========================================================================*/
mt_obex_status_type mt_obex_srv_get_response(
    mt_obex_app_id_type                  app_id,
    uint16                               conn_id,
    mt_obex_hdr_list_type*               headers_ptr,
    mt_obex_status_type                  status );

/*===========================================================================

FUNCTION mt_obex_srv_put_response

DESCRIPTION
  Respond to a PUT command. When application receives a MT_OBEX_EV_SRV_PUT_IND
  event this function will acknowledge a PUT command, optionally sending response
  headers to a connected OBEX client.

DEPENDENCIES
  None

RETURN VALUE
  General Status of the command that has been issued to OBEX.

SIDE EFFECTS
  None

===========================================================================*/
mt_obex_status_type mt_obex_srv_put_response(
    mt_obex_app_id_type                  app_id,
    uint16                               conn_id,
    mt_obex_hdr_list_type*               headers_ptr,
    mt_obex_status_type                  status );


/*===========================================================================

FUNCTION mt_obex_srv_disconnect_response

DESCRIPTION
  Respond to a Disconnect command. When application receives a MT_OBEX_EV_SRV_DCN_IND
  event this function will acknowledge a Disconenct command, optionally sending response
  headers to a OBEX client.

DEPENDENCIES
  None

RETURN VALUE
  General Status of the command that has been issued to OBEX.

SIDE EFFECTS
  None

===========================================================================*/
mt_obex_status_type mt_obex_srv_disconnect_response(
    mt_obex_app_id_type                  app_id,
    uint16                               conn_id,
    mt_obex_hdr_list_type*               headers_ptr,
    mt_obex_status_type                  status );


/*===========================================================================

FUNCTION mt_obex_srv_force_disconnect

DESCRIPTION
  Force a disconnect from the server. This command forcibly severs the
  connection from an OBEX client to the OBEX server. An OBEX server may need
  to forcibly terminate a connection during deregistration, since
  deregistration will fail if a connection is in place.

DEPENDENCIES
  None

RETURN VALUE
  General Status of the command that has been issued to OBEX.

SIDE EFFECTS
  None

===========================================================================*/
mt_obex_status_type mt_obex_srv_force_disconnect(
    mt_obex_app_id_type                  app_id,
    uint16                               conn_id );

/*===========================================================================

FUNCTION mt_obex_srv_deregister_server

DESCRIPTION
  De-register an OBEX server. This function deregisters an OBEX server so that
  it will no longer accept put and/or get requests from OBEX clients for the
  specified target. This will also deregister any secondary targets associated
  with this server. To deregister a server that has active connections,
  terminate the connections using mt_obex_srv_force_disconnect() first, then
  deregister the server.

DEPENDENCIES
  None

RETURN VALUE
  General Status of the command that has been issued to OBEX.

SIDE EFFECTS
  None

===========================================================================*/
mt_obex_status_type mt_obex_srv_deregister_server(
    uint16                               app_id,
    uint8                                ch_num );

/*===========================================================================

FUNCTION mt_obex_srv_get_client_addr

DESCRIPTION
  This function gets the Bluetooth Device Address (BD_ADDR) associated with 
  an OBEX server connection handle.
  
DEPENDENCIES
  None

RETURN VALUE
  General Status of the command that has been issued to OBEX.

SIDE EFFECTS
  None

===========================================================================*/
mt_obex_status_type mt_obex_srv_get_client_addr(
  mt_obex_app_id_type                  app_id,
  uint16                               conn_id,
  bt_bd_addr_type*                     bd_addr_ptr );


/*===========================================================================

FUNCTION 
    mt_obex_status_type	mt_obex_sd_register_custom_service

DESCRIPTION
  This function registers custom 128bit address with SD.
  
DEPENDENCIES
  None
    
RETURN VALUE
  General Status of the command that has been issued to OBEX.

SIDE EFFECTS
  None

===========================================================================*/

mt_obex_status_type	mt_obex_sd_register_custom_service(
            mt_obex_app_id_type           app_id,
            mt_obex_uuid128_type         *uuid128,
            uint8                         scn,
            char*                         service_name_str_ptr) ;


/*** client events ***/
/*

MT_OBEX_EV_CLI_CON_CFM
This event notifies the application whether the requested connection with the remote OBEX
server has been established. The connect operation was initialized by a previous call to
the mt_obex_cli_connect() command.

Data Type associated with this event:

typedef PACKED struct
{
  mt_obex_cli_conn_id_type               conn_id;
  boolean                                read_only;
  mt_obex_status_type                    status;
} mt_obex_ev_cli_con_cfm_type;

MT_OBEX_EV_CLI_DCN_IND
This event notifies the application that the connection with the remote OBEX server has
been torn down. The disconnection was either initiated by a previous call to the
mt_obex_cli_disconnect() command or by the remote OBEX server.

Data Type associated with this event:

typedef PACKED struct
{
  mt_obex_cli_conn_id_type               conn_id;
} mt_obex_ev_cli_dcn_ind_type;

MT_OBEX_EV_CLI_PUT_CFM
This event notifies the application that the requested PUT operation has completed.
The push operation was initiated by a previous call to the mt_obex_cli_put() command.

Data Type associated with this event:

typedef PACKED struct
{
  mt_obex_cli_conn_id_type               conn_id;
  bt_pf_goep_obex_hdr_list_type*         rsp_headers_ptr;
  mt_obex_status_type                    status;
} mt_obex_ev_cli_put_cfm_type;

MT_OBEX_EV_CLI_GET_RCV_DATA
This event notifies the application that there has been a response to the requested
GET operation . The GET operation was initiated by a previous call to the mt_obex_cli_get()
command.

Data Type associated with this event:

typedef PACKED struct
{
  mt_obex_cli_conn_id_type               conn_id;
  mt_obex_hdr_list_type*                 rsp_headers_ptr;
  mt_obex_status_type                    status;
} mt_obex_ev_cli_get_rcv_data_type;


MT_OBEX_EV_CLI_SET_PATH_CFM
This event notifies the application that a set path operation has completed at the remote
OBEX server. The set path operation was initiated by a previous call to the mt_obex_cli_set_path()
command.

Data Type associated with this event:

typedef PACKED struct
{
  mt_obex_cli_conn_id_type               conn_id;
  mt_obex_status_type                    status;
} mt_obex_ev_cli_set_path_cfm_type;



MT_OBEX_EV_CLI_AUTH_IND
This event requests the application to provide a user ID and password for authentication with the
remote OBEX server. The application should respond by providing the OBEX protocol with the password
and user ID, if requested, using the mt_obex_cli_auth_response() command.

Data Type associated with this event:
typedef PACKED struct
{
  mt_obex_cli_conn_id_type               conn_id;
  boolean                                user_id_required;
} mt_obex_ev_cli_auth_ind_type;
*/

/*** server events ***/

/*
MT_OBEX_EV_SRV_CON_IND
This event notifies the application that a request to connect to a local OBEX server has been
received from a remote OBEX client. The unauthorized flag in the initial event is set to FALSE.
The application can respond by:
�   Calling the mt_obex_srv_auth_response() command to proceed with authenticating the remote OBEX client.
�   Calling the mt_obex_srv_accept_connect() command with the accept flag set to FALSE to reject
the incoming connection.
�   Calling the mt_obex_srv_accept_connect() command with the accept flag set to TRUE to accept
the incoming connection without authentication.
If the application proceeds with authentication, a second MT_OBEX_EV_SRV_CON_IND event will be
issued after the remote OBEX client responds with a password. The unauthorized flag will be set
to TRUE if authentication succeeds or FALSE if authentication fails.
If authentication succeeds, the application can accept the connection by calling the
mt_obex_srv_accept_connect() command with the accept flag set to TRUE.

If authentication fails, the application can respond by:
�   Calling the mt_obex_srv_accept_connect() command with the accept flag set to FALSE to reject
the incoming connection.
�   Calling the mt_obex_srv_auth_response() command to retry authentication.
Data Type associated with this event:

typedef PACKED struct
{
  uint8                                  ch_num;
  uint8                                  target_id;
  mt_obex_srv_conn_id_type               conn_id;
  boolean                                unauthorized;
  byte                         user_id[BT_PF_OBEX_MAX_USERID_LEN];
  uint8                                  user_id_len;
} mt_obex_ev_srv_con_ind_type;

MT_OBEX_EV_SRV_DCN_IND
This event notifies the application that a remote OBEX client is disconnecting from the local OBEX server.

Data Type associated with this event:

typedef PACKED struct
{
  uint8                                  ch_num;
  uint8                                  target_id;
  mt_obex_srv_conn_id_type               conn_id;
} mt_obex_ev_srv_dcn_ind_type;


MT_OBEX_EV_SRV_GET_IND
This event notifies the application that a GET request has been received from a remote OBEX client. The application should call the mt_obex_srv_get_response() command to return the object data.

Data Type associated with this event:

typedef PACKED struct
{
  uint8                                  ch_num;
  uint8                                  target_id;
  mt_obex_srv_conn_id_type               conn_id;
  mt_obex_hdr_list_type*                 cmd_headers_ptr;
  mt_obex_status_type                    status;
} mt_obex_ev_srv_get_ind_type;

MT_OBEX_EV_SRV_PUT_IND
This event notifies the application that a PUT request has been received from a remote OBEX client. The application should call the mt_obex_srv_put_response() command to indicate if the PUT operation is successful.

Data Type associated with this event:

typedef PACKED struct
{
  uint8                                  ch_num;
  uint8                                  target_id;
  mt_obex_srv_conn_id_type               conn_id;
  mt_obex_hdr_list_type*                 cmd_headers_ptr;
  mt_obex_status_type                    status;
} mt_obex_ev_srv_put_ind_type;


MT_OBEX_EV_SRV_SET_PATH_IND
This event requests the application to set the current folder to the specified path. The application should set the current folder to the specified path and notify OBEX when this is completed by calling the mt_obex_srv_confirm_set_path() command.

Data Type associated with this event:

typedef PACKED struct
{
  uint8                                  ch_num;
  uint8                                  target_id;
  uint16*                                folder_ptr;
  boolean                                up_level;
  boolean                                create;
} mt_obex_ev_srv_set_path_ind_type;
*/

#endif /* FEATURE_MT_OBEX */
#endif /* FEATURE_BT_EXTPF_GOEP */
#endif /* FEATURE_BT */

#endif /* _MT_OBEX_H */
