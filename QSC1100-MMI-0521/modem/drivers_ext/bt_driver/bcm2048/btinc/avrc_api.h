/*****************************************************************************
**
**  Name:       avrc_api.h
**
**  Description:Interface to AVRCP Application Programming Interface
**
**  Copyright (c) 2006, Broadcom Corp., All Rights Reserved.
*****************************************************************************/
#ifndef AVRC_API_H
#define AVRC_API_H

#include "avct_api.h"
#include "sdp_api.h"
#include "avrc_defs.h"

/*****************************************************************************
**  constants
*****************************************************************************/

/* API function return value result codes. */
#define AVRC_SUCCESS        AVCT_SUCCESS        /* 0 Function successful */
#define AVRC_NO_RESOURCES   AVCT_NO_RESOURCES   /* 1 Not enough resources */
#define AVRC_BAD_HANDLE     AVCT_BAD_HANDLE     /* 2 Bad handle */
#define AVRC_PID_IN_USE     AVCT_PID_IN_USE     /* 3 PID already in use */
#define AVRC_NOT_OPEN       AVCT_NOT_OPEN       /* 4 Connection not open */
#define AVRC_FAIL           0x10                /* 0x10 generic failure */
#define AVRC_BAD_PARAM      0x11                /* 0x11 bad parameter   */

/* Control role. */
#define AVRC_CT_TARGET      1                     /* target  */
#define AVRC_CT_CONTROL     2                     /* controller  */

/* AVRC CTRL events */
/* AVRC_OPEN_IND_EVT event is sent when the connection is successfully opened.
 * This eventis sent in response to an AVRC_Open(). */
#define AVRC_OPEN_IND_EVT       0

/* AVRC_CLOSE_IND_EVT event is sent when a connection is closed.
 * This event can result from a call to AVRC_Close() or when the peer closes
 * the connection.  It is also sent when a connection attempted through
 * AVRC_Open() fails. */
#define AVRC_CLOSE_IND_EVT      1   

/* AVRC_CONG_IND_EVT event indicates that AVCTP is congested and cannot send
 * any more messages. */
#define AVRC_CONG_IND_EVT       2

/* AVRC_UNCONG_IND_EVT event indicates that AVCTP is uncongested and ready to
 * send messages. */
#define AVRC_UNCONG_IND_EVT     3 

/* Categories in the SDP record
#define AVRC_CATEGORY_PLAYER_RECORDER   (1 << 0)    
#define AVRC_CATEGORY_MONITOR_AMPLIFIER (1 << 1)
#define AVRC_CATEGORY_TUNER             (1 << 2)
#define AVRC_CATEGORY_MENU              (1 << 3)
#define AVRC_EXT_PLAYER_APP_SETTINGS    (1 << 4)
#define AVRC_EXT_GROUP_NAVIGATION       (1 << 5)
*/

/* Supported categories */
#define AVRC_SUPF_CT_CAT1               0x0001      /* Category 1 */
#define AVRC_SUPF_CT_CAT2               0x0002      /* Category 2 */
#define AVRC_SUPF_CT_CAT3               0x0004      /* Category 3 */
#define AVRC_SUPF_CT_CAT4               0x0008      /* Category 4 */
#define AVRC_CATEGORY_PLAYER_RECORDER   AVRC_SUPF_CT_CAT1
#define AVRC_CATEGORY_MONITOR_AMPLIFIER AVRC_SUPF_CT_CAT2
#define AVRC_CATEGORY_TUNER             AVRC_SUPF_CT_CAT3
#define AVRC_CATEGORY_MENU              AVRC_SUPF_CT_CAT4
#define AVRC_EXT_PLAYER_APP_SETTINGS    0x0010
#define AVRC_EXT_GROUP_NAVIGATION       0x0020

#define AVRC_SUPF_TG_CAT1               0x0001      /* Category 1 */
#define AVRC_SUPF_TG_CAT2               0x0002      /* Category 2 */
#define AVRC_SUPF_TG_CAT3               0x0004      /* Category 3 */
#define AVRC_SUPF_TG_CAT4               0x0008      /* Category 4 */

#define AVRC_META_SUCCESS               AVRC_SUCCESS
#define AVRC_META_FAIL                  AVRC_FAIL
#define AVRC_METADATA_CMD               0x0000
#define AVRC_METADATA_RESP              0x0001



/*****************************************************************************
**  data type definitions
*****************************************************************************/

/* This data type is used in AVRC_FindService() to initialize the SDP database
 * to hold the result service search. */
typedef struct
{
    UINT32              db_len;  /* Length, in bytes, of the discovery database */
    tSDP_DISCOVERY_DB  *p_db;    /* Pointer to the discovery database */
    UINT16              num_attr;/* The number of attributes in p_attrs */
    UINT16             *p_attrs; /* The attributes filter. If NULL, AVRCP API sets the attribute filter
                                  * to be ATTR_ID_SERVICE_CLASS_ID_LIST, ATTR_ID_BT_PROFILE_DESC_LIST,
                                  * ATTR_ID_SUPPORTED_FEATURES, ATTR_ID_SERVICE_NAME and ATTR_ID_PROVIDER_NAME.
                                  * If not NULL, the input is taken as the filter. */
} tAVRC_SDP_DB_PARAMS;

/* This callback function returns service discovery information to the
 * application after the AVRC_FindService() API function is called.  The
 * implementation of this callback function must copy the p_service_name
 * and p_provider_name parameters passed to it as they are not guaranteed
 * to remain after the callback function exits. */
typedef void (tAVRC_FIND_CBACK) (BOOLEAN found, char* p_service_name,
             UINT16 service_name_len, char* p_provider_name,
             UINT16 provider_name_len, UINT16 categories);

 
/* This is the control callback function.  This function passes events
 * listed in Table 20 to the application. */
typedef void (tAVRC_CTRL_CBACK) (UINT8 handle, UINT8 event, UINT16 result,
             BD_ADDR peer_addr);


/* This is the message callback function.  It is executed when AVCTP has
 * a message packet ready for the application.  The implementation of this
 * callback function must copy the tAVRC_MSG structure passed to it as it
 * is not guaranteed to remain after the callback function exits. */
typedef void (tAVRC_MSG_CBACK) (UINT8 handle, UINT8 label, UINT8 opcode,
             tAVRC_MSG *p_msg);

typedef struct
{
    tAVRC_CTRL_CBACK    *p_ctrl_cback;  /* pointer to application control callback */
    tAVRC_MSG_CBACK     *p_msg_cback;   /* pointer to application message callback */
    UINT32              company_id;     /* the company ID  */
    UINT8               conn;           /* Connection role (Initiator/acceptor) */
    UINT8               control;        /* Control role (Control/Target) */
} tAVRC_CONN_CB;

/*****************************************************************************
**  Metadata transfer Encoding/Decoding definitions
*****************************************************************************/
typedef struct {
    UINT8   msg_type;                       /* Command or Response message */
    UINT8   pdu_id;                         /* PDU_ID */
    UINT8   pkt_type;                       /* packet type */
} tAVRC_META_HEADER;

typedef struct {
    tAVRC_META_HEADER   header;
    UINT8               capability;         /* Capability ID */
    UINT8               count;
    UINT8               *p_company_list; 
    UINT8               *p_event_list;
} tAVRC_META_CAPABILITY;

/* Player Application Settings and Vaue*/
typedef struct {
    UINT8   attrib_id;        
    UINT8   id_text_len;
    UINT8   *p_attrib_id_str;              /* Attribute ID text */   
    UINT8   attrib_value;
    UINT8   val_text_len;
    UINT8   *p_attrib_val_str;             /* Attribute value text */
} tAVRC_META_APPSETTING;

typedef struct {
    tAVRC_META_HEADER       header;
    UINT16                  char_set;       /* CharacterSet ID */
    UINT8                   count;          /* NumPlayerApplicationSettingAttributes (N) */
    tAVRC_META_APPSETTING   *p_player_setting; /* PlayerApplicationSettingAttributeID */
} tAVRC_META_PLAYERAPPSETTING;

typedef struct {
    tAVRC_META_HEADER   header;
    UINT8               count;              /* 1-255 */
    UINT8               *p_str;                  
} tAVRC_META_CHARACTERSET;

typedef struct {
    tAVRC_META_HEADER   header;
    UINT8               bat_stat;     
} tAVRC_META_BATSTAT;

typedef struct {
    UINT32              attrib_id;
    UINT16              str_len;
    UINT8               *p_str;
} tAVRC_META_STRING;

typedef struct {
    tAVRC_META_STRING   song_title;         /* Title */ 
    tAVRC_META_STRING   artist_name;        /* Artist */
    tAVRC_META_STRING   album_name;         /* Album  */
    tAVRC_META_STRING   num_track;          /* Current track number */
    tAVRC_META_STRING   total_track;        /* Total number of track */
    tAVRC_META_STRING   genre;              /* Genre */
    tAVRC_META_STRING   playing_time;       /* Total playing time in miliseconds */
} tAVRC_META_MEDIA_INFO; 

typedef struct {
    tAVRC_META_HEADER       header;
    UINT8                   count;
    UINT8                   Identifier;
    UINT16                  char_set;
    tAVRC_META_MEDIA_INFO   *p_media_info;
} tAVRC_META_ELEMENT_ATTRIBUTES; 
  
typedef struct {
    tAVRC_META_HEADER header;
    UINT32  song_length;
    UINT32  song_pos;
    UINT8   play_status;
} tAVRC_META_PLAYSTATUS;

typedef struct {
    tAVRC_META_HEADER           header;
    UINT8                       event_id;
    UINT32                      playback_interval;
    UINT8                       play_status;
    UINT8                       Identifier;
    UINT32                      playback_pos;
    UINT8                       bat_stat;
    UINT8                       sys_status;
    tAVRC_META_PLAYERAPPSETTING app_setting;
} tAVRC_META_NOTIFICATION;

typedef struct {
    tAVRC_META_HEADER   header;
    UINT8               pdu_id;
} tAVRC_META_CONTINUATION;

typedef struct {
    tAVRC_META_HEADER   header;
    UINT32              company_id;
    UINT16              direction;
} tAVRC_META_GROUP_NAVIGATION; 

typedef struct {
    tAVRC_META_HEADER   header;
    UINT8               error_code;
} tAVRC_META_ERRORSTATUS;

typedef union
{
    tAVRC_META_HEADER               head;
    tAVRC_META_CAPABILITY           capa;           /* 0x10 - GetCapabilities */
    tAVRC_META_PLAYERAPPSETTING     list_attr;      /* 0x11 - ListPlayerApplicationSettingAttributes */
    tAVRC_META_PLAYERAPPSETTING     list_val;       /* 0x12 - ListPlayerApplicationSettingValues */
    tAVRC_META_PLAYERAPPSETTING     get_cur_val;    /* 0x13 - GetCurrentPlayerApplicationSettingValues */
    tAVRC_META_PLAYERAPPSETTING     set_val;        /* 0x14 - SetPlayerApplicationSettingValue */
    tAVRC_META_PLAYERAPPSETTING     get_attr_text;  /* 0x15 - GetPlayerApplicatonSettingAttributeText */
    tAVRC_META_PLAYERAPPSETTING     get_val_text;   /* 0x16 - GetPlayerApplicatonSettingValueText */
    tAVRC_META_CHARACTERSET         char_set;       /* 0x17 - InformDisplayableCharacterSet */   
    tAVRC_META_BATSTAT              bat_stat;       /* 0x18 - InformBatteryStatusOfCT */
    tAVRC_META_ELEMENT_ATTRIBUTES   elem_attr;      /* 0x20 - GetElementAttributes */
    tAVRC_META_PLAYSTATUS           play_stat;      /* 0x30 - GetPlayStatus */
    tAVRC_META_NOTIFICATION         regi_notif;     /* 0x31 - RegisterNotification */
    tAVRC_META_CONTINUATION         req_resp;       /* 0x40 - RequestContinuingResponse */
    tAVRC_META_CONTINUATION         abort_resp;     /* 0x41 - AbortContinuingResponse */
    tAVRC_META_GROUP_NAVIGATION     navi_dir;       /* Navigational direction */
    tAVRC_META_ERRORSTATUS          error_code;     /* Error Codes  */
} tAVRC_META_PDU;


typedef struct {       
    tAVRC_META_PDU      meta_msg;
    UINT8               rsp_type;
    UINT8               *p_vendor_data;
    UINT8               *p_buffer;
    UINT16              buffer_len;
} tAVRC_META_MSGS;
/*****************************************************************************
**  external function declarations
*****************************************************************************/
#ifdef __cplusplus
extern "C"
{
#endif

/******************************************************************************
**
** Function         AVRC_AddRecord
**
** Description      This function is called to build an AVRCP SDP record.
**                  Prior to calling this function the application must 
**                  call SDP_CreateRecord() to create an SDP record.
**
**                  Input Parameters:
**                      service_uuid:  Indicates TG(UUID_SERVCLASS_AV_REM_CTRL_TARGET)
**                                            or CT(UUID_SERVCLASS_AV_REMOTE_CONTROL)
**
**                      p_service_name:  Pointer to a null-terminated character
**                      string containing the service name.
**                      If service name is not used set this to NULL.
**
**                      p_provider_name:  Pointer to a null-terminated character
**                      string containing the provider name.
**                      If provider name is not used set this to NULL.
**
**                      categories:  Supported categories.
**
**                      sdp_handle:  SDP handle returned by SDP_CreateRecord().
**
**                  Output Parameters:
**                      None.
**
** Returns          AVRC_SUCCESS if successful.
**                  AVRC_NO_RESOURCES if not enough resources to build the SDP record.
**
******************************************************************************/
AVRC_API extern UINT16 AVRC_AddRecord(UINT16 service_uuid, char *p_service_name,
                char *p_provider_name, UINT16 categories, UINT32 sdp_handle);

/******************************************************************************
**
** Function         AVRC_FindService
**
** Description      This function is called by the application to perform service
**                  discovery and retrieve AVRCP SDP record information from a
**                  peer device.  Information is returned for the first service
**                  record found on the server that matches the service UUID.  
**                  The callback function will be executed when service discovery
**                  is complete.  There can only be one outstanding call to
**                  AVRC_FindService() at a time; the application must wait for
**                  the callback before it makes another call to the function.
**                  The application is responsible for allocating memory for the
**                  discovery database.  It is recommended that the size of the
**                  discovery database be at least 300 bytes.  The application
**                  can deallocate the memory after the callback function has
**                  executed.
**
**                  Input Parameters:
**                      service_uuid: Indicates TG(UUID_SERVCLASS_AV_REM_CTRL_TARGET)
**                                           or CT(UUID_SERVCLASS_AV_REMOTE_CONTROL)
**
**                      bd_addr:  BD address of the peer device.
**
**                      p_db:  SDP discovery database parameters.
**                      
**                      p_cback:  Pointer to the callback function.
**
**                  Output Parameters:
**                      None.
**
** Returns          AVRC_SUCCESS if successful.
**                  AVRC_BAD_PARAMS if discovery database parameters are invalid.
**                  AVRC_NO_RESOURCES if there are not enough resources to
**                                    perform the service search.
**
******************************************************************************/
AVRC_API extern UINT16 AVRC_FindService(UINT16 service_uuid, BD_ADDR bd_addr,
                tAVRC_SDP_DB_PARAMS *p_db, tAVRC_FIND_CBACK *p_cback);

/******************************************************************************
**
** Function         AVRC_Open
**
** Description      This function is called to open a connection to AVCTP. 
**                  The connection can be either an initiator or acceptor, as
**                  determined by the p_ccb->stream parameter. 
**                  The connection can be a target, a controller or for both role,
**                  as determined by the p_ccb->control parameter. 
**                  By definition, a target connection is an acceptor connection
**                  that waits for an incoming AVCTP connection from the peer. 
**                  The connection remains available to the application until
**                  the application closes it by calling AVRC_Close().  The
**                  application does not need to reopen the connection after an
**                  AVRC_CLOSE_IND_EVT is received.
**
**                  Input Parameters:
**                      p_ccb->company_id: Company Identifier.
**
**                      p_ccb->p_ctrl_cback:  Pointer to control callback function.
**
**                      p_ccb->p_msg_cback:  Pointer to message callback function.
**                      
**                      p_ccb->conn: AVCTP connection role.  This is set to
**                      AVCTP_INT for initiator connections and AVCTP_ACP
**                      for acceptor connections.
**
**                      p_ccb->control: Control role.  This is set to
**                      AVRC_CT_TARGET for target connections, AVRC_CT_CONTROL
**                      for control connections or (AVRC_CT_TARGET|AVRC_CT_CONTROL)
**                      for connections that support both roles.
**
**                      peer_addr: BD address of peer device.  This value is
**                      only used for initiator connections; for acceptor
**                      connections it can be set to NULL.
**
**                  Output Parameters:
**                      p_handle: Pointer to handle.  This parameter is only 
**                                valid if AVRC_SUCCESS is returned.
**
** Returns          AVRC_SUCCESS if successful.
**                  AVRC_NO_RESOURCES if there are not enough resources to open
**                  the connection.
**
******************************************************************************/
AVRC_API extern UINT16 AVRC_Open(UINT8 *p_handle, tAVRC_CONN_CB *p_ccb,
                                 BD_ADDR_PTR peer_addr);

/******************************************************************************
**
** Function         AVRC_Close
**
** Description      Close a connection opened with AVRC_Open().
**                  This function is called when the
**                  application is no longer using a connection.
**
**                  Input Parameters:
**                      handle: Handle of this connection.
**
**                  Output Parameters:
**                      None.
**
** Returns          AVRC_SUCCESS if successful.
**                  AVRC_BAD_HANDLE if handle is invalid.
**
******************************************************************************/
AVRC_API extern UINT16 AVRC_Close(UINT8 handle);

/******************************************************************************
**
** Function         AVRC_UnitCmd
**
** Description      Send a UNIT INFO command to the peer device.  This
**                  function can only be called for controller role connections.
**                  Any response message from the peer is passed back through
**                  the tAVRC_MSG_CBACK callback function.
**
**                  Input Parameters:
**                      handle: Handle of this connection.
**
**                      label: Transaction label.
**
**                  Output Parameters:
**                      None.
**
** Returns          AVRC_SUCCESS if successful.
**                  AVRC_BAD_HANDLE if handle is invalid.
**
******************************************************************************/
AVRC_API extern UINT16 AVRC_UnitCmd(UINT8 handle, UINT8 label);

/******************************************************************************
**
** Function         AVRC_SubCmd
**
** Description      Send a SUBUNIT INFO command to the peer device.  This
**                  function can only be called for controller role connections.
**                  Any response message from the peer is passed back through
**                  the tAVRC_MSG_CBACK callback function.
**
**                  Input Parameters:
**                      handle: Handle of this connection.
**
**                      label: Transaction label.
**
**                      page: Specifies which part of the subunit type table
**                      is requested.  For AVRCP it is typically zero.
**                      Value range is 0-7.
**
**                  Output Parameters:
**                      None.
**
** Returns          AVRC_SUCCESS if successful.
**                  AVRC_BAD_HANDLE if handle is invalid.
**
******************************************************************************/
AVRC_API extern UINT16 AVRC_SubCmd(UINT8 handle, UINT8 label, UINT8 page);


/******************************************************************************
**
** Function         AVRC_PassCmd
**
** Description      Send a PASS THROUGH command to the peer device.  This
**                  function can only be called for controller role connections.
**                  Any response message from the peer is passed back through
**                  the tAVRC_MSG_CBACK callback function.
**
**                  Input Parameters:
**                      handle: Handle of this connection.
**
**                      label: Transaction label.
**
**                      p_msg: Pointer to PASS THROUGH message structure. 
**
**                  Output Parameters:
**                      None.
**
** Returns          AVRC_SUCCESS if successful.
**                  AVRC_BAD_HANDLE if handle is invalid.
**
******************************************************************************/
AVRC_API extern UINT16 AVRC_PassCmd(UINT8 handle, UINT8 label, tAVRC_MSG_PASS *p_msg);

/******************************************************************************
**
** Function         AVRC_PassRsp
**
** Description      Send a PASS THROUGH response to the peer device.  This
**                  function can only be called for target role connections. 
**                  This function must be called when a PASS THROUGH command
**                  message is received from the peer through the
**                  tAVRC_MSG_CBACK callback function.
**
**                  Input Parameters:
**                      handle: Handle of this connection.
**
**                      label: Transaction label.  Must be the same value as
**                      passed with the command message in the callback function.
**
**                      p_msg: Pointer to PASS THROUGH message structure.
**
**                  Output Parameters:
**                      None.
**
** Returns          AVRC_SUCCESS if successful.
**                  AVRC_BAD_HANDLE if handle is invalid.
**
******************************************************************************/
AVRC_API extern UINT16 AVRC_PassRsp(UINT8 handle, UINT8 label, tAVRC_MSG_PASS *p_msg);


/******************************************************************************
**
** Function         AVRC_VendorCmd
**
** Description      Send a VENDOR DEPENDENT command to the peer device.  This
**                  function can only be called for controller role connections.
**                  Any response message from the peer is passed back through
**                  the tAVRC_MSG_CBACK callback function.
**
**                  Input Parameters:
**                      handle: Handle of this connection.
**
**                      label: Transaction label.
**
**                      p_msg: Pointer to VENDOR DEPENDENT message structure.
**
**                  Output Parameters:
**                      None.
**
** Returns          AVRC_SUCCESS if successful.
**                  AVRC_BAD_HANDLE if handle is invalid.
**
******************************************************************************/
AVRC_API extern UINT16 AVRC_VendorCmd(UINT8  handle, UINT8  label, tAVRC_MSG_VENDOR *p_msg);


/******************************************************************************
**
** Function         AVRC_VendorRsp
**
** Description      Send a VENDOR DEPENDENT response to the peer device.  This
**                  function can only be called for target role connections.
**                  This function must be called when a VENDOR DEPENDENT
**                  command message is received from the peer through the
**                  tAVRC_MSG_CBACK callback function.
**
**                  Input Parameters:
**                      handle: Handle of this connection.
**
**                      label: Transaction label.  Must be the same value as
**                      passed with the command message in the callback function.
**
**                      p_msg: Pointer to VENDOR DEPENDENT message structure.
**
**                  Output Parameters:
**                      None.
**
** Returns          AVRC_SUCCESS if successful.
**                  AVRC_BAD_HANDLE if handle is invalid.
**
******************************************************************************/
AVRC_API extern UINT16 AVRC_VendorRsp(UINT8  handle, UINT8  label, tAVRC_MSG_VENDOR *p_msg);


/******************************************************************************
**
** Function         AVRC_SetTraceLevel
**
** Description      Sets the trace level for AVRC. If 0xff is passed, the
**                  current trace level is returned.
**
**                  Input Parameters:
**                      new_level:  The level to set the AVRC tracing to:
**                      0xff-returns the current setting.
**                      0-turns off tracing.
**                      >= 1-Errors.
**                      >= 2-Warnings.
**                      >= 3-APIs.
**                      >= 4-Events.
**                      >= 5-Debug.
**
** Returns          The new trace level or current trace level if
**                  the input parameter is 0xff.
**
******************************************************************************/
AVRC_API extern UINT8 AVRC_SetTraceLevel (UINT8 new_level);

/*******************************************************************************
**
** Function         AVRC_Init
**
** Description      This function is called at stack startup to allocate the
**                  control block (if using dynamic memory), and initializes the
**                  control block and tracing level.
**
** Returns          void
**
*******************************************************************************/
AVRC_API extern void AVRC_Init(void);

/*******************************************************************************
**
** Function         AVRC_BldMetadata
**
** Description      This function is called at stack to build the metadata binary 
**                  string.
**
** Returns          UINT16
**
*******************************************************************************/
AVRC_API extern UINT16 AVRC_BldMetadata(tAVRC_META_MSGS *p_params, UINT8 *p_data);

/*******************************************************************************
**
** Function         AVRC_ParsMetadata
**
** Description      This function is called at stack startup to parse the metadata
**                  binary string.
**
** Returns          UINT8
**
*******************************************************************************/
AVRC_API extern UINT8 AVRC_ParsMetadata(UINT8 *p_data, UINT16 len, UINT8 rsp_code, UINT8 rc_msg_type, tAVRC_META_MSGS *p_params);

#ifdef __cplusplus
}
#endif

#endif /* AVRC_API_H */
