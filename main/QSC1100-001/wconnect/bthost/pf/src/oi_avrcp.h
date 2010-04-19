#ifdef FEATURE_BT_EXTPF_AV
#ifndef _PROFILES_AVRCP_H
#define _PROFILES_AVRCP_H

/**
 * @file
 * This file provides the interface for the Audio/Video Remote Control Profile (AVRCP).
 */

/**********************************************************************************
  $Revision: #1 $
  Copyright 2002 - 2004 Open Interface North America, Inc. All rights reserved.
***********************************************************************************/

#include "oi_common.h"
#include "oi_l2cap.h"
#include "oi_avctp.h"
#include "oi_avrcp_consts.h"
#include "oi_sdpdb.h"
#include "oi_unicode.h"

/** \addtogroup AVRC */
/**@{*/

#ifdef __cplusplus
extern "C" {
#endif


/****************************************************************************
 *
 * AVRCP types and macros
 *
 ****************************************************************************/


/** A structure of this type is used to represent an AVRCP frame.
 *  Extended subunit types and IDs are not supported.
 */
typedef struct {
    OI_UINT8 cmd;
    OI_UINT8 subunitType;
    OI_UINT8 subunitID;
    OI_UINT8 opcode;
    OI_BYTE *operand;
    OI_UINT16 operandCount;
} OI_AVRCP_FRAME;


/** A structure of this type is used to represent status of the currently playing media.
 */
typedef struct {
    OI_UINT32 songLength;   /**< The total length of the song in milliseconds.
                               If TG does not support SongLength then TG shall return 0xFFFFFFFF. */
    OI_UINT32 songPosition; /**< The current position of the song in milliseconds elapsed.
                               If TG does not support SongPosition then TG shall return 0xFFFFFFFF. */
    OI_UINT8  status;       /**< Current play status of song, an #OI_AVRCP_PLAYBACK_STATE constant. */
} OI_AVRCP_PLAY_STATUS;


/** A structure of this type is used to represent a player event and its current status.
 */
typedef struct {
    OI_UINT8 id;                    /**< An #OI_AVRCP_EVENT_ID constant. */
    union {
        OI_UINT8 playStatus;        /**< An #OI_AVRCP_PLAYBACK_STATE constant. */
        OI_UINT8 trackIndex[8];     /**< Must be 8 bytes long. 0xFFFFFFFFFF if no track selected. */
        OI_UINT32 playbackPosition; /**< The current position of the song in milliseconds elapsed.
                                         TG shall return 0xFFFFFFFF if not supported. */
        OI_UINT8 batteryStatus;     /**< An #OI_AVRCP_BATTERY_STATUS constant. */
        OI_UINT8 systemStatus;      /**< An #OI_AVRCP_SYSTEM_STATUS constant. */
        struct {
            /** Pairs of #OI_AVRCP_PLAYER_APPLICATION_SETTINGS and their values. */
            OI_AVRCP_ATTRIBUTE_PAIR attributes;
            OI_UINT8 numAttr;
        } deviceSetting;
    } u;
} OI_AVRCP_EVENT;

typedef struct {
    OI_UINT32 attrID;   /**< Media Attribute ID, a #OI_AVRCP_MEDIA_ATTRIBUTES constant. */
    OI_UINT16 charSet;  /**< Character set code. See OI_LANG_ENCODING constants in oi_bt_assigned_nos.h. */
    OI_PSTR value;      /**< Media Attribute Value */
} OI_AVRCP_ELEMENT_ATTRIBUTE;

/** A structure of this type represents a text string associated with a particular Player Application Setting
    Attribute ID or Value ID. */
typedef struct {
    OI_UINT8  id;       /**< An #OI_AVRCP_PLAYER_APPLICATION_SETTINGS or OI_AVRCP_*_STATUS_VALUES constant. */
    OI_UINT16 charSet;  /**< Character set code. See OI_LANG_ENCODING constants in oi_bt_assigned_nos.h. */
    OI_PSTR value;      /**< Text attribute value */
} OI_AVRCP_ATTRIBUTE_TEXT;

/* The following structures are used to represent Target responses.
 */

/** Response to PDU 0x10. */
typedef struct {
    OI_UINT8 capabilityID;
    OI_UINT8 capabilityCount;
    union {
        OI_UINT32 *companyID;
        OI_UINT8   *eventID;
    } capabilities;
} OI_AVRCP_CAPABILITY;

/** Response to PDUs 0x11-0x14. */
typedef struct {
    OI_UINT8 numAttrs;
    union {
        OI_UINT8 attrID;
        OI_UINT8 *ids;
        OI_UINT8 *attrValues;
        OI_AVRCP_ATTRIBUTE_PAIR attrs;
    }attrSetting;
} OI_AVRCP_PLAYER_APPLICATION_ATTRIBUTES;

typedef struct {
    OI_UINT8 numAttr;
    OI_AVRCP_ELEMENT_ATTRIBUTE *attrs;
} OI_AVRCP_ELEMENT_ATTRIBUTE_RESPONSE;

typedef struct {
    OI_UINT8 numAttrs;
    OI_AVRCP_ATTRIBUTE_TEXT *attrs;
} OI_AVRCP_PLAYER_ATTRIBUTE_TEXT_RESPONSE;

typedef struct {
    OI_UINT8 pduID;
    union {

        /** #OI_AVRCP_GET_CAPABILITIES response. PDU 0x10 */
        OI_AVRCP_CAPABILITY getCapabilities;

        /** #OI_AVRCP_LIST_PLAYER_APPLICATION_SETTING_ATTRIBUTES response. PDU 0x11-0x14 */
        OI_AVRCP_PLAYER_APPLICATION_ATTRIBUTES playerAttributes;

        /** #OI_AVRCP_LIST_PLAYER_APPLICATION_SETTING_ATTRIBUTES response. PDUs 0x15, 0x16 */
        OI_AVRCP_PLAYER_ATTRIBUTE_TEXT_RESPONSE playerAttributesText;

        /** PDU 0x20 */
        OI_AVRCP_ELEMENT_ATTRIBUTE_RESPONSE elementAttributes;

        /** PDU 0x30 */
        OI_AVRCP_PLAY_STATUS playStatus;

        /** PDU 0x31 */
        OI_AVRCP_EVENT *event;
    } u;
} OI_AVRCP_RESPONSE;

/** The structure of this type is used to represent a data buffer (supplied by the controller application)
 *  into which the response frame data can be optionally unmarshalled. */
typedef struct {
    OI_UINT8 data[OI_AVRCP_MAX_FRAME_LENGTH];
    OI_BOOL overflow; /**< This flag indicates whether the data[] buffer was not sufficient to accomodate the unmarshalled response.
                           If it is set to TRUE, the value in data[] is valid, but incomplete. */
} OI_AVRCP_RESPONSE_DATA;

/** This structure is used for handling AVRCP message fragmentation. */
typedef struct {
    /** Pointer to a current attribute/element/value indicated by
     *  @a currAttrIndex of #OI_AVRCP_TARGET_CONTINUE_RESPONSE_COMMAND_IND.
     *  This value may point to an array of one of the following structures, depending
     *  on response PDU:
     *   - For #OI_AVRCP_GET_ELEMENT_ATTRIBUTES, an array of #OI_AVRCP_ELEMENT_ATTRIBUTE
     *   - For #OI_AVRCP_GET_PLAYER_APPLICATION_SETTING_ATTRIBUTE_TEXT, an array of
     *     #OI_AVRCP_ATTRIBUTE_TEXT
     *   - For #OI_AVRCP_GET_PLAYER_APPLICATION_SETTING_VALUE_TEXT, an array of
     *     #OI_AVRCP_ATTRIBUTE_TEXT
     *   - For #OI_AVRCP_GET_CURRENT_PLAYER_APPLICATION_SETTING_VALUE, an array of #OI_AVRCP_ATTRIBUTE_PAIR
     *   - For #OI_AVRCP_REGISTER_NOTIFICATION, an array of #OI_AVRCP_ATTRIBUTE_PAIR
     *   - For #OI_AVRCP_LIST_PLAYER_APPLICATION_SETTING_ATTRIBUTES, an array of #OI_UINT8, each an
     *     application setting ID
     *   - For #OI_AVRCP_LIST_PLAYER_APPLICATION_SETTING_VALUES, an array of #OI_UINT8, each an
     *     application setting value
     *   - For #OI_AVRCP_GET_CAPABILITIES with a capability ID of #OI_AVRCP_CAPABILITY_COMPANY_ID, an array of #OI_UINT32,
     *     each a company ID
     *   - For #OI_AVRCP_GET_CAPABILITIES with a capability ID of #OI_AVRCP_CAPABILITY_EVENTS_SUPPORTED, an array of #OI_UINT8,
     *     each an event ID
     */
    void *attr;
    /** Byte pointer to text data associated with the current attribute/element/value indicated by
     *  @a currDataPos of #OI_AVRCP_TARGET_CONTINUE_RESPONSE_COMMAND_IND. This field is only relevant
     *  when responding to these PDUs:
     *   - #OI_AVRCP_GET_PLAYER_APPLICATION_SETTING_ATTRIBUTE_TEXT
     *   - #OI_AVRCP_GET_PLAYER_APPLICATION_SETTING_VALUE_TEXT
     *   - #OI_AVRCP_GET_ELEMENT_ATTRIBUTES
     */
    OI_UINT8 *data;
} OI_AVRCP_FRAGMENT_DATA;


/****************************************************************************
 *
 * AVRCP Connection Callbacks
 *
 ****************************************************************************/

/**
 * A callback function of this type indicates an incoming connection.
 *
 * @param  addr  the Bluetooth device address of the remote device
 */
typedef void (*OI_AVRCP_CONNECT_IND)(OI_BD_ADDR *addr);

/**
 * A callback function of this type confirms the establishment of an AVRCP connection.
 *
 * @param  addr   the Bluetooth device address of the remote device
 * @param  result the result
 */
typedef void (*OI_AVRCP_CONNECT_CFM)(OI_BD_ADDR *addr,
                                     OI_STATUS result);


/**
 * A callback function of this type indicates that a disconnect initiated by the local or remote
 * side has completed.
 *
 * @param  addr     the Bluetooth device address of the remote device
 * @param  reason   the reason for the disconnect, such as #OI_L2CAP_DISCONNECT_REMOTE_REQUEST
 *                  (See the L2CAP documentation for more details.)
 */
typedef void (*OI_AVRCP_DISCONNECT_IND)(OI_BD_ADDR *addr,
                                        OI_STATUS reason);

/**
 * A structure of this type is used to group callback functions that are common to both
 * AVRCP target and controller functionality.
 */
typedef struct {
    OI_AVRCP_CONNECT_IND connectInd;
    OI_AVRCP_DISCONNECT_IND disconnectInd;
    OI_AVRCP_CONNECT_CFM connectCfm;
} OI_AVRCP_CALLBACKS;

/**
 * This function initializes the AVRCP profile.
 */
OI_STATUS OI_AVRCP_Register(const OI_AVRCP_CALLBACKS *cb);

/**
 * This function terminates the AVRCP profile.
 */
OI_STATUS OI_AVRCP_Deregister(void);

/****************************************************************************
 *
 * AVRCP Connection Functions
 *
 ****************************************************************************/

/** This function requests a connection to the specified remote device.
 *
 * @param addr  the Bluetooth device address of the remote device
 * @param l2cap This parameter is a pointer to the L2CAP connection parameters
 *              to be used for the underlying AVCTP (Audio/Video Control Transport Protocol)
 *              connection. If an AVCTP connection already exists, this parameter will not be used.
 *              Passing a NULL value results in the default L2CAP parameters being used.
 */
OI_STATUS OI_AVRCP_Connect(OI_BD_ADDR *addr,
                           OI_L2CAP_CONNECT_PARAMS *l2cap);

/**
 * This function indicates whether to accept or reject an incoming connection.
 * It should be called in response to an #OI_AVRCP_CONNECT_IND callback.
 *
 * @param addr   the Bluetooth device address of the remote device
 * @param accept TRUE to accept, FALSE to reject
 */
OI_STATUS OI_AVRCP_Accept(OI_BD_ADDR *addr,
                          OI_BOOL accept);

/**
 * This function disconnects from the specified device. This function always succeeds.
 *
 * @param addr  the Bluetooth device address of the remote device
 */
OI_STATUS OI_AVRCP_Disconnect(OI_BD_ADDR *addr);



/****************************************************************************
 *
 * AVRCP controller commands
 *
 ****************************************************************************/

/**
 * This function sends an AVRCP "PASS THROUGH" command to a remote target.
 *
 * @param addr          This parameter indicates the Bluetooth device address
 *                      of the target device. An AVCTP connection to this device must
 *                      already exist.
 * @param transaction   This parameter is a pointer to an OI_UINT8 buffer to receive
 *                      the AVCTP transaction number assigned to this command. This
 *                      must be non-NULL.
 * @param op            This parameter indicates the AVRCP operation to send.
 *                      See avctp_consts.h for a list. VENDOR_UNIQUE operations requiring
 *                      operation data should use #OI_AVRCPController_SendGeneric instead.
 */
OI_STATUS OI_AVRCPController_SendCommand(OI_BD_ADDR *addr,
                                         OI_UINT8 *transaction,
                                         OI_UINT8 op);

/**
 * This function sends an AVRCP "UNIT INFO" query to a remote target.
 *
 * @param addr          This parameter indicates the Bluetooth device address
 *                      of the target device. An AVCTP connection to this device must
 *                      already exist.
 * @param transaction   This parameter is a pointer to an OI_UINT8 buffer to receive
 *                      the AVCTP transaction number assigned to this command. This
 *                      must be non-NULL.
 */
OI_STATUS OI_AVRCPController_GetUnitInfo(OI_BD_ADDR *addr,
                                         OI_UINT8 *transaction);


/**
 * This function sends an AVRCP "SUBUNIT INFO" query to a remote target.
 *
 * @param addr          This parameter indicates the Bluetooth device address
 *                      of the target device. An AVCTP connection to this device must
 *                      already exist.
 * @param transaction   This parameter is a pointer to an OI_UINT8 buffer to receive
 *                      the AVCTP transaction number assigned to this command. This
 *                      must be non-NULL.
 * @param page          the number, from 0 to 7 inclusive, indicating which page of subunit
 *                      information to request
 */
OI_STATUS OI_AVRCPController_GetSubunitInfo(OI_BD_ADDR *addr,
                                            OI_UINT8 *transaction,
                                            OI_UINT8 page);

/**
 * This function sends an arbitrary AVRCP command to a remote target.
 *
 * @param addr          This parameter indicates the Bluetooth device address
 *                      of the target device. An AVCTP connection to this device must
 *                      already exist.
 * @param transaction   This parameter is a pointer to an OI_UINT8 buffer to receive
 *                      the AVCTP transaction number assigned to this command. This
 *                      must be non-NULL.
 * @param frame         pointer to the #OI_AVRCP_FRAME to send
 */
OI_STATUS OI_AVRCPController_SendGeneric(OI_BD_ADDR *addr,
                                         OI_UINT8 *transaction,
                                         OI_AVRCP_FRAME *frame);

/**
 * A callback function of this type indicates either receipt of a reply from a remote target or a
 * transmit failure.
 *
 * @param addr          Bluetooth device address of the target device associated with this reply
 * @param transaction   the AVCTP transaction number associated with this reply or failure notification.
 * @param reply         pointer to the reply frame if @a status is OI_OK, NULL otherwise
 * @param status        OI_OK indicates receipt of a target response; other values
 *                      indicate a failure during transmission of a previous command.
 */
typedef void (*OI_AVRCP_CONTROLLER_REPLY_IND)(OI_BD_ADDR *addr,
                                              OI_UINT8 transaction,
                                              OI_AVRCP_FRAME *reply,
                                              OI_STATUS status);


/*
 * EXTENDED SUPPORT FOR AVRCP METADATA IN 1.3 profile
 */

/****************************************************************************
 *
 * Enhanced AVRCP controller commands
 *
 ****************************************************************************/

/*
 * CAPABILITY COMMANDS
 */

/**
 * This function sends an AVRCP "GET CAPABILITY" query with 0x2 command value to a remote target.
 *
 * @param addr          This parameter indicates the Bluetooth device address
 *                      of the target device. An AVCTP connection to this device must
 *                      already exist.
 * @param transaction   This parameter is a pointer to an OI_UINT8 buffer to receive
 *                      the AVCTP transaction number assigned to this command. This
 *                      must be non-NULL.
 */
OI_STATUS OI_AVRCPController_GetCompanyIDs(OI_BD_ADDR *addr,
                                           OI_UINT8 *transaction);


/**
 * This function sends an AVRCP "GET CAPABILITY" query with 0x3 command value to a remote target.
 *
 * @param addr          This parameter indicates the Bluetooth device address
 *                      of the target device. An AVCTP connection to this device must
 *                      already exist.
 * @param transaction   This parameter is a pointer to an OI_UINT8 buffer to receive
 *                      the AVCTP transaction number assigned to this command. This
 *                      must be non-NULL.
 */
OI_STATUS OI_AVRCPController_GetSupportedEvents(OI_BD_ADDR *addr,
                                                OI_UINT8 *transaction);

/**
 * This function sends an AVRCP "LIST PLAYER APPLICATION SETTING ATTRIBUTES" query to a remote target.
 *
 * @param addr          This parameter indicates the Bluetooth device address
 *                      of the target device. An AVCTP connection to this device must
 *                      already exist.
 * @param transaction   This parameter is a pointer to an OI_UINT8 buffer to receive
 *                      the AVCTP transaction number assigned to this command. This
 *                      must be non-NULL.
 */
OI_STATUS OI_AVRCPController_ListPlayerAppSettingAttributes(OI_BD_ADDR *addr,
                                                            OI_UINT8 *transaction);

/**
 * This function sends an AVRCP "LIST PLAYER APPLICATION SETTING VALUES" query to a remote target.
 *
 * @param addr          This parameter indicates the Bluetooth device address
 *                      of the target device. An AVCTP connection to this device must
 *                      already exist.
 * @param transaction   This parameter is a pointer to an OI_UINT8 buffer to receive
 *                      the AVCTP transaction number assigned to this command. This
 *                      must be non-NULL.
 * @param attrID        Indicates the attribute for which the range of values is requested.
 *                      Attribute is an #OI_AVRCP_PLAYER_APPLICATION_SETTINGS constant.
 */
OI_STATUS OI_AVRCPController_ListPlayerAppSettingValues(OI_BD_ADDR *addr,
                                                        OI_UINT8 *transaction,
                                                        OI_UINT8 attrID);

/**
 * This function sends an AVRCP "GET CURRENT PLAYER APPLICATION SETTING VALUE" command to a remote target.
 *
 * @param addr          This parameter indicates the Bluetooth device address
 *                      of the target device. An AVCTP connection to this device must
 *                      already exist.
 * @param transaction   This parameter is a pointer to an OI_UINT8 buffer to receive
 *                      the AVCTP transaction number assigned to this command. This
 *                      must be non-NULL.
 * @param count         Number of attributes for which the current values are requested.
 * @param attrIDs       Pointer to a list of @a count attributes for which the current values are requested.
 *                      Attributes are #OI_AVRCP_PLAYER_APPLICATION_SETTINGS constants.
 */
OI_STATUS OI_AVRCPController_GetCurrentPlayerAppSettingValue(OI_BD_ADDR *addr,
                                                             OI_UINT8 *transaction,
                                                             OI_UINT8 count,
                                                             OI_UINT8 *attrIDs);

/**
 * This function sends an AVRCP "SET PLAYER APPLICATION SETTING VALUE" query to a remote target.
 *
 * @param addr          This parameter indicates the Bluetooth device address
 *                      of the target device. An AVCTP connection to this device must
 *                      already exist.
 * @param transaction   This parameter is a pointer to an OI_UINT8 buffer to receive
 *                      the AVCTP transaction number assigned to this command. This
 *                      must be non-NULL.
 * @param count         Number of attributes for which the values to be set.
 * @param attrs         Pointer to a list of @a count pairs of attribute IDs and corresponding values.
 *                      Attributes are #OI_AVRCP_PLAYER_APPLICATION_SETTINGS constants. Values are
 *                      OI_AVRCP_*_STATUS_VALUES constants.
 */
OI_STATUS OI_AVRCPController_SetPlayerAppSettingValue(OI_BD_ADDR *addr,
                                                      OI_UINT8 *transaction,
                                                      OI_UINT8 count,
                                                      OI_AVRCP_ATTRIBUTE_PAIR attrs);

/**
 * This function sends an AVRCP "GET CURRENT PLAYER APPLICATION SETTING ATTRIBUTE TEXT" query to a remote target.
 * This query requests the target device to provide supported player application setting attribute displayable text
 * for the provided PlayerApplicationSettingAttributeIDs. This command is expected to be used only for
 * extended attributes for menu navigation. It is assumed that all <attribute, value> pairs used for
 * menu extensions are statically defined by TG.
 *
 * @param addr          This parameter indicates the Bluetooth device address
 *                      of the target device. An AVCTP connection to this device must
 *                      already exist.
 * @param transaction   This parameter is a pointer to an OI_UINT8 buffer to receive
 *                      the AVCTP transaction number assigned to this command. This
 *                      must be non-NULL.
 * @param count         Number of attributes for which the current values are requested.
 * @param attrIDs       Pointer to a list of @a count attribute IDs for which the text settings are requested.
 *                      Attribute IDs are valid PlayerApplicationSettingAttributeID values received from the target,
 *                      or defined attributeIDs listed under #OI_AVRCP_PLAYER_APPLICATION_SETTINGS in oi_avrcp_consts.h.
 */
OI_STATUS OI_AVRCPController_GetPlayerAppSettingAttributeText(OI_BD_ADDR *addr,
                                                              OI_UINT8 *transaction,
                                                              OI_UINT8 count,
                                                              OI_UINT8 *attrIDs);

/**
 * This function sends an AVRCP "GET PLAYER APPLICATION SETTING VALUE TEXT" query to a remote target.
 * This query requests the target device to provide target supported player application setting value
 * displayable text for the provided player application setting attribute values. This command
 * is expected to be used only for extended attributes for menu navigation. It is assumed that
 * all <attribute, value> pairs used for menu extensions are statically defined by TG.
 *
 * @param addr          This parameter indicates the Bluetooth device address
 *                      of the target device. An AVCTP connection to this device must
 *                      already exist.
 * @param transaction   This parameter is a pointer to an OI_UINT8 buffer to receive
 *                      the AVCTP transaction number assigned to this command. This
 *                      must be non-NULL.
 * @param attrID        Player application setting attribute ID as defined by #OI_AVRCP_PLAYER_APPLICATION_SETTINGS
 *                      in oi_avrcp_consts.h or received from the target.
 * @param count         Number of player application setting values for which corresponding strings are needed.
 * @param valueIDs      Pointer to a list of @a count player application setting value IDs for which
 *                      corresponding strings are needed.
 */
OI_STATUS OI_AVRCPController_GetPlayerAppSettingValueText(OI_BD_ADDR *addr,
                                                          OI_UINT8 *transaction,
                                                          OI_UINT8 attrID,
                                                          OI_UINT8 count,
                                                          OI_UINT8 *valueIDs);

/**
 * This function sends an AVRCP "INFORM DISPLAYABLE CHARACTER SET" command to a remote target.
 *
 * @param addr          This parameter indicates the Bluetooth device address
 *                      of the target device. An AVCTP connection to this device must
 *                      already exist.
 * @param transaction   This parameter is a pointer to an OI_UINT8 buffer to receive
 *                      the AVCTP transaction number assigned to this command. This
 *                      must be non-NULL.
 * @param count         Number of displayable character sets (1-255).
 * @param charSetIDs    List of character set IDs that can be displayed on CT. See the
 *                      OI_LANG_ENCODING constants defined in oi_bt_assigned_nos.h.
 */
OI_STATUS OI_AVRCPController_InformCharacterSet(OI_BD_ADDR *addr,
                                                OI_UINT8 *transaction,
                                                OI_UINT8 count,
                                                OI_UINT16 *charSetIDs);

/**
 * This function sends an AVRCP "INFORM BATTERY STATUS OF CT" command to a remote target.
 *
 * @param addr          This parameter indicates the Bluetooth device address
 *                      of the target device. An AVCTP connection to this device must
 *                      already exist.
 * @param transaction   This parameter is a pointer to an OI_UINT8 buffer to receive
 *                      the AVCTP transaction number assigned to this command. This
 *                      must be non-NULL.
 * @param batteryStatus Battery status of controller. For list of available values see
 *                      #OI_AVRCP_BATTERY_STATUS in oi_avrcp_consts.h
 */
OI_STATUS OI_AVRCPController_InformBatteryStatusOfCT(OI_BD_ADDR *addr,
                                                     OI_UINT8 *transaction,
                                                     OI_UINT8 batteryStatus);

/**
 * This function sends an AVRCP "GET PLAY STATUS" query to a remote target.
 *
 * @param addr          This parameter indicates the Bluetooth device address
 *                      of the target device. An AVCTP connection to this device must
 *                      already exist.
 * @param transaction   This parameter is a pointer to an OI_UINT8 buffer to receive
 *                      the AVCTP transaction number assigned to this command. This
 *                      must be non-NULL.
 */
OI_STATUS OI_AVRCPController_GetPlayStatus(OI_BD_ADDR *addr,
                                           OI_UINT8 *transaction);

/**
 * This function sends an AVRCP "GET ELEMENT ATTRIBUTE" query to a remote target.
 *
 * @param addr          This parameter indicates the Bluetooth device address
 *                      of the target device. An AVCTP connection to this device must
 *                      already exist.
 * @param transaction   This parameter is a pointer to an OI_UINT8 buffer to receive
 *                      the AVCTP transaction number assigned to this command. This
 *                      must be non-NULL.
 * @param id            Pointer to an 8-byte identifier specifying a media element on Target.
 *                      The only valid identifier is @c PLAYING (0x0000000000000000).
 * @param count         Number of element attributes requested.
 * @param attributes    Pointer to a list of attribute IDs for the attributes to be retrieved.
 *                      Attributes are #OI_AVRCP_MEDIA_ATTRIBUTES constants.
 */
OI_STATUS OI_AVRCPController_GetElementAttributes(OI_BD_ADDR *addr,
                                                  OI_UINT8 *transaction,
                                                  OI_UINT8 *id,
                                                  OI_UINT8 count,
                                                  OI_UINT32 *attributes);

/**
 * This function sends an AVRCP "REGISTER NOTIFICATION" command to a remote target.
 * This command registers with the TG to receive notifications asynchronously based
 * on specific events occurring. The TG sends one notification immediately and another
 * when the event occurs. The CT will receive these notifications via the
 * #OI_AVRCP_CONTROLLER_METADATA_REPLY_IND with a @a pduID of #OI_AVRCP_REGISTER_NOTIFICATION.
 *
 * @param addr             This parameter indicates the Bluetooth device address
 *                         of the target device. An AVCTP connection to this device must
 *                         already exist.
 * @param transaction      This parameter is a pointer to an OI_UINT8 buffer to receive
 *                         the AVCTP transaction number assigned to this command. This
 *                         must be non-NULL.
 * @param eventID          This parameter specifies an event for which the CT requires
 *                         notifications. An #OI_AVRCP_EVENT_ID constant.
 * @param playbackInterval This parameter specifies the playback position interval (in seconds)
 *                         at which the change in playback position will be notified. It is applicable
 *                         only for EventID EVENT_PLAYBACK_POS_CHANGED. For other events,
 *                         value of this parameter is ignored.
 */
OI_STATUS OI_AVRCPController_RegisterNotification(OI_BD_ADDR *addr,
                                                  OI_UINT8 *transaction,
                                                  OI_UINT8 eventID,
                                                  OI_UINT32 playbackInterval);

/**
 * This function sends an AVRCP "REQUEST CONTINUING RESPONSE" command to a remote target.
 * This command requests the remainder of an incomplete response to a sent PDU command.
 * The CT may call this function if the #OI_AVRCP_CONTROLLER_METADATA_REPLY_IND callback
 * has a @a packetType argument of #OI_AVRCP_PDU_FIRST or #OI_AVRCP_PDU_CONTINUE.
 *
 * @param addr          This parameter indicates the Bluetooth device address
 *                      of the target device. An AVCTP connection to this device must
 *                      already exist.
 * @param transaction   This parameter is a pointer to an OI_UINT8 buffer to receive
 *                      the AVCTP transaction number assigned to this command. This
 *                      must be non-NULL.
 * @param pduID         The PDU ID of the incomplete response; the @a pduID argument to
 *                      #OI_AVRCP_CONTROLLER_METADATA_REPLY_IND.
 */
OI_STATUS OI_AVRCPController_ContinueResponse(OI_BD_ADDR *addr,
                                              OI_UINT8 *transaction,
                                              OI_UINT8 pduID);

/**
 * This function sends an AVRCP "ABORT CONTINUING RESPONSE" command to a remote target
 * to abort a continuing response. The CT may call this function if the
 * #OI_AVRCP_CONTROLLER_METADATA_REPLY_IND callback has a @a packetType argument of
 * #OI_AVRCP_PDU_FIRST or #OI_AVRCP_PDU_CONTINUE.
 *
 * @param addr          This parameter indicates the Bluetooth device address
 *                      of the target device. An AVCTP connection to this device must
 *                      already exist.
 * @param transaction   This parameter is a pointer to an OI_UINT8 buffer to receive
 *                      the AVCTP transaction number assigned to this command. This
 *                      must be non-NULL.
 * @param pduID         The PDU ID of the incomplete response; the @a pduID argument to
 *                      #OI_AVRCP_CONTROLLER_METADATA_REPLY_IND.
 */
OI_STATUS OI_AVRCPController_AbortResponse(OI_BD_ADDR *addr,
                                           OI_UINT8 *transaction,
                                           OI_UINT8 pduID);


/**
 * This function sends an AVRCP PASS THROUGH Command (vendor unique) for Group Navigation.
 * @param addr            This parameter indicates the Bluetooth device address
 *                        of the target device. An AVCTP connection to this device must
 *                        already exist.
 * @param transaction     This parameter is a pointer to an OI_UINT8 buffer to receive
 *                        the AVCTP transaction number assigned to this command. This
 *                        must be non-NULL.
 * @param vendorUniqueID  Indicates Next or Previous Group (see #OI_AVRCP_GROUP_NAVIGATION in oi_avrcp_consts.h).
 * @param buttonPress     Indicates whether button is pushed or released (TRUE for pushed, FALSE for released).
*/
OI_STATUS OI_AVRCPController_GroupNavigationCommand(OI_BD_ADDR *addr,
                                                    OI_UINT8 *transaction,
                                                    OI_UINT16 vendorUniqueID,
                                                    OI_BOOL buttonPress);


/**
 * This is a helper function that can be used to unmarshal the response frame received from TG into
 * an #OI_AVRCP_RESPONSE representation. This function can only process non-fragmented responses where
 * the #OI_AVRCP_CONTROLLER_METADATA_REPLY_IND @a packetType argument is #OI_AVRCP_PDU_COMPLETE.
 *
 * @param frame          pointer to the reply frame.
 * @param pduID          PDU_ID of the received response; the @a pduID argument to #OI_AVRCP_CONTROLLER_METADATA_REPLY_IND.
 * @param responseBuffer A pointer to a buffer, into which the frame will be unmarshalled.
 * @param dataBuffer     A pointer to a structure that contains an unmarshalling buffer and an overflow flag.
 *                       The response's variable length data (ex. attributes and their values) will be unmarshalled
 *                       into the buffer. The corresponding pointers in @a responseBuffer will point to data in this buffer.
 *                       This structure also contains an overflow flag, which when set to TRUE indicates that the provided
 *                       buffer was insufficient to unmarshal the entire packet and the packet was only partially processed.
 */
OI_STATUS OI_AVRCPController_UnmarshalMetaResponse(OI_AVRCP_FRAME *frame,
                                                   OI_UINT8 pduID,
                                                   OI_AVRCP_RESPONSE *responseBuffer,
                                                   OI_AVRCP_RESPONSE_DATA *dataBuffer);

/****************************************************************************
 *
 * Enhanced AVRCP Controller Callbacks
 *
 ****************************************************************************/

/**
 * A callback function of this type indicates receipt of VENDOR DEPENDENT reply from a remote target.
 * This function must be defined if metadata callback functions are going to be used.
 *
 * @param addr          Bluetooth device address of the target device associated with this reply
 * @param transaction   AVCTP transaction number associated with this reply or failure notification.
 * @param frame         pointer to the reply frame if @a status is OI_OK.
 * @param pduID         PDU ID of a command that was sent to generate this response. An #OI_AVRCP_PDU_ID
 *                      constant.
 * @param packetType    indicates the recieved packet as either start (Packet
 *                      Type=0x1), continue (Packet Type=0x2), or end packet (Packet Type=0x3). In the case of a
 *                      non-fragmented message, Packet Type=0x0. An #OI_AVRCP_PACKET_TYPE constant.
 * @param status        indicates the status of received response.
 */
typedef void (*OI_AVRCP_CONTROLLER_METADATA_REPLY_IND)(OI_BD_ADDR *addr,
                                                       OI_UINT8 transaction,
                                                       OI_AVRCP_FRAME *frame,
                                                       OI_UINT8 pduID,
                                                       OI_UINT8 packetType,
                                                       OI_STATUS status);

typedef struct {
    OI_AVRCP_CONTROLLER_REPLY_IND replyInd;
    OI_AVRCP_CONTROLLER_METADATA_REPLY_IND metadataReplyInd;
} OI_AVRCP_CONTROLLER_CALLBACKS;

/** This function registers an AVRCP controller with the AVRCP profile. This should be called
 *  after calling OI_AVRCP_Register() to register the endpoint.
 *
 * @param cb          pointer to controller-specific callbacks
 * @param categories  This parameter represents SDP constants declaring which remote control
 *                    categories are supported. If multiple categories are supported, their
 *                    values should be ORed together. (See avrcp_consts.h.)
 * @param strings     strings to register in service record
 * @return            OI_OK if registration was successful, an error code otherwise
 */
OI_STATUS OI_AVRCP_RegisterController(const OI_AVRCP_CONTROLLER_CALLBACKS *cb,
                                      OI_UINT16 categories,
                                      const OI_SDP_STRINGS *strings);

/**
 * Get the service record handle associated with this service.
 * This can be used with e.g. #OI_SDPDB_SetAttributeList to add
 * vendor-specific SDP attributes to the profile.
 *
 * @param handle  return the service record's handle
 */
OI_STATUS OI_AVRCPController_GetServiceRecord(OI_UINT32 *handle);

/** This function deregisters the AVRCP Controller from the AVRCP profile. This should be called
 * prior to deregistering the profile.
 */
OI_STATUS OI_AVRCP_DeregisterController( void );


/******************************
 Target interface
*******************************/

/** A callback function of this type indicates that an AVRCP command has been sent. The device should
 *  respond immediately with OI_AVRCPTarget_SendReply().
 *
 * @param addr          Bluetooth device address of the host sending the command
 * @param transaction   transaction ID of the command, which is passed to OI_AVRCPTarget_SendReply()
 * @param op            the operation sent (See avrcp_consts.h.)
 */
typedef void (*OI_AVRCP_TARGET_COMMAND_IND)(OI_BD_ADDR *addr,
                                            OI_UINT8 transaction,
                                            OI_UINT8 op);

/** A callback function of this type indicates that a vendor-dependent AVRCP command has been sent.
 *  If this callback is defined, the device should respond with OI_AVRCPTarget_SendGenericReply().
 *  If this callback is specified as NULL, the protocol stack will respond automatically
 *  with the status code OI_AVRCP_RESPONSE_NOT_IMPLEMENTED.
 *
 * @param addr          Address of the host sending the command.
 * @param transaction   Transaction ID of the command. This is passed to
 *                      OI_AVRCPTarget_SendGenericReply().
 * @param frame         Pointer to an #OI_AVRCP_FRAME structure describing the command.
 */
typedef void (*OI_AVRCP_TARGET_GENERIC_IND)(OI_BD_ADDR *addr,
                                            OI_UINT8 transaction,
                                            OI_AVRCP_FRAME *frame);


/** Sends a reply to an AVRCP PASS_THROUGH command.
 * @param addr          Bluetooth device address of the host being replied to
 * @param transaction   transaction ID of the command being replied to
 * @param op            the operation being replied to (should be the same as
 *                      the @a op argument to #OI_AVRCP_TARGET_COMMAND_IND).
 * @param response      the AVRCP response code (See avrcp_consts.h.)
 * @return              OI_OK if the reply could be sent, an error code otherwise
 *
 */
OI_STATUS OI_AVRCPTarget_SendReply(OI_BD_ADDR *addr,
                                   OI_UINT8 transaction,
                                   OI_UINT8 op,
                                   OI_UINT8 response);

/** Sends a reply to an arbitrary AVRCP command.
 * @param addr          Bluetooth device address of the host being replied to
 * @param transaction   transaction ID of the command being replied to
 * @param frame         the frame to return to the controller. It is permissable
 *                      to modify in-place the frame received by #OI_AVRCP_TARGET_GENERIC_IND
 * @return              OI_OK if the reply could be sent, an error code otherwise
 *
 */
OI_STATUS OI_AVRCPTarget_SendGenericReply(OI_BD_ADDR *addr,
                                          OI_UINT8 transaction,
                                          OI_AVRCP_FRAME *frame);



/**
 * Get the service record handle associated with this service.
 * This can be used with e.g. #OI_SDPDB_SetAttributeList to add
 * vendor-specific SDP attributes to the profile.
 *
 * @param handle  return the service record's handle
 */
OI_STATUS OI_AVRCPTarget_GetServiceRecord(OI_UINT32 *handle);

/** This function deregisters the AVRCP target from the AVRCP profile. This should be called
 * prior to deregistering the profile.
 */
OI_STATUS OI_AVRCP_DeregisterTarget( void );



/****************************************************************************
 *
 * Enhanced AVRCP target interface
 *
 ****************************************************************************/

/**
 * This function sends response to the following CT commands, where the response does not include
 * any parameters:
 *     - SET CURRENT PLAYER APPLICATION SETTING VALUE
 *     - INFORM DISPLAYABLE CHARACTER SET
 *     - INFORM BATTERY STATUS
 *     - ABORT CONTINUING RESPONSE
 * Alternatively, it sends a REJECTED or NOT_IMPLEMENTED response to CT,
 * based on the value of the @a status parameter.
 *
 * @param addr          This parameter indicates the Bluetooth device address
 *                      of the CT device. An AVCTP connection to this device must
 *                      already exist.
 * @param transaction   Transaction ID of the command being replied to.
 * @param pduID         PDU ID of a command that was sent to generate this response.
 *                      An #OI_AVRCP_PDU_ID constant.
 * @param status        OI_OK or an error status code specifying a reason for the REJECTED/NOT_IMPLEMENTED response.
 *                      Allowed values:
 *                            - OI_OK
 *                            - OI_AVRCP_NOT_IMPLEMENTED
 *                            - OI_AVRCP_RESPONSE_INVALID_PDU
 *                            - OI_AVRCP_RESPONSE_INVALID_PARAMETER
 *                            - OI_AVRCP_RESPONSE_PARAMETER_NOT_FOUND
 *                            - OI_AVRCP_RESPONSE_INTERNAL_ERROR
 *                            .
 *                      Any other value of status parameter will result in REJECTED response sent with INTERNAL_ERROR code.
 *
 * @return              OI_OK if the reply could be sent, an error code otherwise
*/
OI_STATUS OI_AVRCPTarget_SendSimpleMetaReply(OI_BD_ADDR *addr,
                                             OI_UINT8 transaction,
                                             OI_UINT8 pduID,
                                             OI_STATUS status);
/**
 * This function sends a reply to a GET CAPABILITIES command with a list of
 * Company IDs.
 *
 * @param addr          This parameter indicates the Bluetooth device address
 *                      of the CT device. An AVCTP connection to this device must
 *                      already exist.
 * @param transaction   Transaction ID of the command being replied to.
 * @param count         Number of company IDs returned by a remote target.
 * @param companyIDs    Pointer to a list of @a count company IDs.
 * @return              OI_OK if the reply could be sent, an error code otherwise.
 *                      If error code is OI_AVRCP_RESPONSE_PACKET_OVERFLOW, the response message is too big
 *                      to send in one packet and has to be fragmented. The FIRST packet has been sent.
 */
OI_STATUS OI_AVRCPTarget_SendCompanyIDReply(OI_BD_ADDR *addr,
                                            OI_UINT8 transaction,
                                            OI_UINT8 count,
                                            OI_UINT32 *companyIDs);

/**
 * This function sends a reply to a GET CAPABILITIES command with a list of
 * Company IDs.
 *
 * @param addr          This parameter indicates the Bluetooth device address
 *                      of the CT device. An AVCTP connection to this device must
 *                      already exist.
 * @param transaction   Transaction ID of the command being replied to.
 * @param count         Number of events supported by a remote target.
 * @param eventIDs      Pointer to a list of @a count event IDs.
 * @return              OI_OK if the reply could be sent, an error code otherwise.
 *                      If error code is OI_AVRCP_RESPONSE_PACKET_OVERFLOW, the response message is too big
 *                      to send in one packet and has to be fragmented. The FIRST packet has been sent.
 */
OI_STATUS OI_AVRCPTarget_SendEventIDReply(OI_BD_ADDR *addr,
                                          OI_UINT8 transaction,
                                          OI_UINT8 count,
                                          OI_UINT8 *eventIDs);

/**
 * This function sends a reply to a LIST_PLAYER_APPLICATION_SETTING_ATTRIBUTES command with a list of
 * supported player setting attribute IDs.
 *
 * @param addr          This parameter indicates the Bluetooth device address
 *                      of the CT device. An AVCTP connection to this device must
 *                      already exist.
 * @param transaction   Transaction ID of the command being replied to.
 * @param count         Number of player setting attributes supported by a remote target.
 * @param attrIDs       Pointer to a list of @a count attribute IDs. Attribute IDs are
 *                      #OI_AVRCP_PLAYER_APPLICATION_SETTINGS constants.
 * @return              OI_OK if the reply could be sent, an error code otherwise.
 */
OI_STATUS OI_AVRCPTarget_ListAttrIDReply(OI_BD_ADDR *addr,
                                         OI_UINT8 transaction,
                                         OI_UINT8 count,
                                         OI_UINT8 *attrIDs);

/**
 * This function sends a reply to a LIST_PLAYER_APPLICATION_SETTING_VALUES command with a list of
 * supported settings for specified attribute ID.
 *
 * @param addr          This parameter indicates the Bluetooth device address
 *                      of the CT device. An AVCTP connection to this device must
 *                      already exist.
 * @param transaction   Transaction ID of the command being replied to.
 * @param attrID        ID of an attribute for which the settings values are requested;
                        an #OI_AVRCP_PLAYER_APPLICATION_SETTINGS constant.
 * @param count         Number of player setting attributes supported by the target.
 * @param attrSettingVals Pointer to a list of @a count values that can be used to initialize the attribute.
 *                        These are OI_AVRCP_*_STATUS_VALUES constants.
 * @return              OI_OK if the reply could be sent, an error code otherwise.
 */
OI_STATUS OI_AVRCPTarget_ListAttrValuesReply(OI_BD_ADDR *addr,
                                             OI_UINT8 transaction,
                                             OI_UINT8 attrID,
                                             OI_UINT8 count,
                                             OI_UINT8 *attrSettingVals);

/**
 * This function sends a reply to a SET_CURRENT_PLAYER_APPLICATION_SETTING_VALUE command.
 *
 * @param addr          This parameter indicates the Bluetooth device address
 *                      of the CT device. An AVCTP connection to this device must
 *                      already exist.
 * @param transaction   Transaction ID of the command being replied to.
 * @param count         Number of attributes for which the current settings are provided.
 * @param attrs         Pointer to a list of @a count pairs of attribute IDs and corresponding
                        values. Attribute IDs are #OI_AVRCP_PLAYER_APPLICATION_SETTINGS constants
                        and values are OI_AVRCP_*_STATUS_VALUES constants.
 * @return              OI_OK if the reply could be sent, an error code otherwise.
 *                      If error code is OI_AVRCP_RESPONSE_PACKET_OVERFLOW, the response message is too big
 *                      to send in one packet and has to be fragmented. The FIRST packet has been sent.
 */
OI_STATUS OI_AVRCPTarget_GetAttrValueReply(OI_BD_ADDR *addr,
                                           OI_UINT8 transaction,
                                           OI_UINT8 count,
                                           OI_AVRCP_ATTRIBUTE_PAIR attrs);

/**
 * This function sends a reply to a GET PLAYER APPLICATION SETTING TEXT command.
 *
 * @param addr          This parameter indicates the Bluetooth device address
 *                      of the CT device. An AVCTP connection to this device must
 *                      already exist.
 * @param transaction   Transaction ID of the command being replied to.
 * @param count         Number of attributes for which text settings are provided.
 * @param attrs         List of @a count attributes and corresponding text.
 * @return              OI_OK if the reply could be sent, an error code otherwise.
 *                      If error code is OI_AVRCP_RESPONSE_PACKET_OVERFLOW, the response message is too big
 *                      to send in one packet and has to be fragmented. The FIRST packet has been sent.
 */
OI_STATUS OI_AVRCPTarget_GetAttrSettingTextReply(OI_BD_ADDR *addr,
                                                 OI_UINT8 transaction,
                                                 OI_UINT8 count,
                                                 OI_AVRCP_ATTRIBUTE_TEXT *attrs);

/**
 * This function sends a reply to a GET PLAYER APPLICATION SETTING VALUE TEXT command.
 *
 * @param addr          This parameter indicates the Bluetooth device address
 *                      of the CT device. An AVCTP connection to this device must
 *                      already exist.
 * @param transaction   Transaction ID of the command being replied to.
 * @param count         Number of attribute values for which text settings are provided.
 * @param settingValues List of @a count attribute values and corresponding text.
 * @return              OI_OK if the reply could be sent, an error code otherwise.
 *                      If error code is OI_AVRCP_RESPONSE_PACKET_OVERFLOW, the response message is too big
 *                      to send in one packet and has to be fragmented. The FIRST packet has been sent.
*/
OI_STATUS OI_AVRCPTarget_GetAttrSettingValueTextReply(OI_BD_ADDR *addr,
                                                      OI_UINT8 transaction,
                                                      OI_UINT8 count,
                                                      OI_AVRCP_ATTRIBUTE_TEXT *settingValues);
/**
 * This function sends a reply to a GET PLAY STATUS command.
 *
 * @param addr          This parameter indicates the Bluetooth device address
 *                      of the CT device. An AVCTP connection to this device must
 *                      already exist.
 * @param transaction   Transaction ID of the command being replied to.
 * @param playStatus    Pointer to current status of playing media.
 * @return              OI_OK if the reply could be sent, an error code otherwise.
*/
OI_STATUS OI_AVRCPTarget_PlayStatusReply(OI_BD_ADDR *addr,
                                         OI_UINT8 transaction,
                                         OI_AVRCP_PLAY_STATUS *playStatus);

/**
 * This function sends a reply to a REGISTER command. Upon receiving a
 * #OI_AVRCP_TARGET_EVENT_NOTIFICATION_COMMAND_IND callback, this function should be
 * called with @a isFinal set to FALSE to return the current state. When the requested
 * event occurs, this function should be called again with @a isFinal set to TRUE to
 * send the changed state.
 *
 * @param addr          This parameter indicates the Bluetooth device address
 *                      of the CT device. An AVCTP connection to this device must
 *                      already exist.
 * @param transaction   Transaction ID of the command being replied to.
 * @param event         Pointer to the requested event and its current or changed state.
 * @param isFinal       Indicates whether this is an initial INTERIM or final CHANGED response.
 * @return              OI_OK if the reply could be sent, an error code otherwise.
 *                      If error code is OI_AVRCP_RESPONSE_PACKET_OVERFLOW, the response message is too big
 *                      to send in one packet and has to be fragmented. The FIRST packet has been sent.
 */
OI_STATUS OI_AVRCPTarget_RegisterNotificationReply(OI_BD_ADDR *addr,
                                                   OI_UINT8 transaction,
                                                   OI_AVRCP_EVENT *event,
                                                   OI_BOOL isFinal);
/**
 * This function sends a reply to a GET_ELEMENT_ATTRIBUTE command.
 *
 * @param addr          This parameter indicates the Bluetooth device address
 *                      of the CT device. An AVCTP connection to this device must
 *                      already exist.
 * @param transaction   Transaction ID of the command being replied to.
 * @param count         Number of media attributes provided.
 * @param element       Pointer to a list of @a count media attributes. Media attributes
 *                      are #OI_AVRCP_MEDIA_ATTRIBUTES constants.
 * @return              OI_OK if the reply could be sent, an error code otherwise.
 *                      If error code is OI_AVRCP_RESPONSE_PACKET_OVERFLOW, the response message is too big
 *                      to send in one packet and has to be fragmented. The FIRST packet has been sent.
 */
OI_STATUS OI_AVRCPTarget_GetElementAttributesReply(OI_BD_ADDR *addr,
                                                   OI_UINT8 transaction,
                                                   OI_UINT8 count,
                                                   OI_AVRCP_ELEMENT_ATTRIBUTE *element);

/**
 * This function sends a reply to an AVRCP PASS THROUGH Command (vendor unique) for Group Navigation.
 *
 * @param addr            This parameter indicates the Bluetooth device address
 *                        of the CT device. An AVCTP connection to this device must
 *                        already exist.
 * @param transaction     Transaction ID of the command being replied to.
 * @param vendorUniqueID  Indicates Next or Previous Group (see #OI_AVRCP_GROUP_NAVIGATION in oi_avrcp_consts.h).
 * @param buttonPress     Indicates whether button is pushed or released (TRUE for pushed, FALSE for released).
 * @param responseCode    Can take values: OI_AVRCP_RESPONSE_NOT_IMPLEMENTED    = 0x8,
 *                                         OI_AVRCP_RESPONSE_ACCEPTED           = 0x9,
 *                                         OI_AVRCP_RESPONSE_REJECTED           = 0xa
 * @return                OI_OK if the reply could be sent, an error code otherwise.
*/
OI_STATUS OI_AVRCPTarget_GroupNavigationReply(OI_BD_ADDR *addr,
                                              OI_UINT8 transaction,
                                              OI_UINT16 vendorUniqueID,
                                              OI_BOOL buttonPress,
                                              OI_UINT8 responseCode);

/**
 * This function sends an additional fragment of a reply. It must be called in response
 * to a CONTINUE_RESPONSE command.
 *
 * @param addr          This parameter indicates the Bluetooth device address
 *                      of the CT device. An AVCTP connection to this device must
 *                      already exist.
 * @param transaction   Transaction ID of the command being replied to.
 * @param pduID         Target PDU_ID for continue response. The @a pduID argument of the
 *                      #OI_AVRCP_TARGET_CONTINUE_RESPONSE_COMMAND_IND callback.
 * @param fragmentMsg   Attribute, element, or ID information and data for next
 *                      fragment. If address and ID information are
 *                      set to NULL, the next fragment will be begin where the last
 *                      fragment left off; this requires the data buffers used for the first
 *                      fragment to be valid.
 *                      Otherwise, the application may specify a new valid data buffer
 *                      containing data for the given ID. This ID and data will be sent
 *                      in the next fragment. See @ref avrcp_fragmentation.
 * @retval OI_OK  the complete reply could be sent; the sent packet has the LAST packet type
 * @retval OI_AVRCP_RESPONSE_PACKET_OVERFLOW  the reply could be sent, but the response
 *                message is too big to fit in one packet; the sent packet has the CONTINUE
 *                packet type
 * @retval OI_AVRCP_NOT_IMPLEMENTED  the response to the command indicated by @a pduID should
 *                not be fragmented; it is not possible for the the response message to exceed
 *                512 bytes based on the spec definition
 */
OI_STATUS OI_AVRCPTarget_FragmentReply (OI_BD_ADDR *addr,
                                        OI_UINT8 transaction,
                                        OI_UINT8 pduID,
                                        OI_AVRCP_FRAGMENT_DATA *fragmentMsg);

/****************************************************************************
 *
 * Enhanced AVRCP target callbacks
 *
 ****************************************************************************/
/**
 * A callback function of this type indicates that an enhanced AVRCP command GET_CAPABILITIES
 * has been received. The device should respond immediately with
 * OI_AVRCPTarget_SendCompanyIDReply(), OI_AVRCPTarget_SendEventIDReply(), or
 * OI_AVRCPTarget_SendSimpleMetaReply().
 *
 * @param addr          Bluetooth device address of the CT sending the command.
 * @param transaction   Transaction ID of the received command.
 * @param capability    Indicates what type of capability is requested; an #OI_AVRCP_CAPABILITY_ID constant.
 */
typedef void (*OI_AVRCP_TARGET_CAPABILITY_COMMAND_IND)(OI_BD_ADDR *addr,
                                                       OI_UINT8 transaction,
                                                       OI_UINT8 capability);

/**
 * A callback function of this type indicates that an enhanced AVRCP command LIST_PLAYER_APPLICATION_SETTING_VALUES
 * has been received. The device should respond immediately with OI_AVRCPTarget_ListAttrValuesReply()
 * or OI_AVRCPTarget_SendSimpleMetaReply().
 *
 * @param addr          Bluetooth device address of the CT sending the command.
 * @param transaction   Transaction ID of the received command.
 * @param attrID        Attribute for which the range of values is requested; an #OI_AVRCP_PLAYER_APPLICATION_SETTINGS
 *                      constant.
 */
typedef void (*OI_AVRCP_TARGET_LIST_ATTRIBUTE_VALUES_COMMAND_IND)(OI_BD_ADDR *addr,
                                                                  OI_UINT8 transaction,
                                                                  OI_UINT8 attrID);

/**
 * A callback function of this type indicates that an enhanced AVRCP command LIST_PLAYER_APPLICATION_SETTING_ATTRIBUTES
 * has been received. The device should respond immediately with OI_AVRCPTarget_ListAttrIDReply()
 * or OI_AVRCPTarget_SendSimpleMetaReply().
 *
 * @param addr          Bluetooth device address of the CT sending the command.
 * @param transaction   Transaction ID of the received command.
 */
typedef void (*OI_AVRCP_TARGET_LIST_ATTRIBUTES_COMMAND_IND)(OI_BD_ADDR *addr,
                                                            OI_UINT8 transaction);

/**
 * A callback function of this type indicates that an enhanced AVRCP command GET CURRENT PLAYER APPLICATION SETTING VALUE
 * has been received. The device should respond immediately with OI_AVRCPTarget_GetAttrValueReply()
 * or OI_AVRCPTarget_SendSimpleMetaReply().
 *
 * @param addr          Bluetooth device address of the CT sending the command.
 * @param transaction   Transaction ID of the received command.
 * @param count         Number of attributes for which the current values are requested.
 * @param attrIDs       Pointer to a list of @a count attributes for which the current values are requested.
 *                      Attributes are #OI_AVRCP_PLAYER_APPLICATION_SETTINGS constants.
 */
typedef void (*OI_AVRCP_TARGET_GET_ATTRIBUTE_VALUES_COMMAND_IND)(OI_BD_ADDR *addr,
                                                                 OI_UINT8 transaction,
                                                                 OI_UINT8 count,
                                                                 OI_UINT8 *attrIDs);

/**
 * A callback function of this type indicates that an enhanced AVRCP command SET PLAYER APPLICATION SETTING VALUE
 * has been received. The device should respond immediately with OI_AVRCPTarget_SendSimpleMetaReply().
 *
 * @param addr          Bluetooth device address of the CT sending the command.
 * @param transaction   Transaction ID of the received command.
 * @param count         Number of attributes for which the current values are being set.
 * @param attrs         Pointer to a list of @a count pairs of attribute IDs and corresponding values.
 *                      Attribute IDs are #OI_AVRCP_PLAYER_APPLICATION_SETTINGS constants; values are
 *                      OI_AVRCP_*_STATUS_VALUES constants.
 */
typedef void (*OI_AVRCP_TARGET_SET_ATTRIBUTE_VALUES_COMMAND_IND)(OI_BD_ADDR *addr,
                                                                 OI_UINT8 transaction,
                                                                 OI_UINT8 count,
                                                                 OI_AVRCP_ATTRIBUTE_PAIR attrs);


/**
 * A callback function of this type indicates that an enhanced AVRCP command GET PLAYER APPLICATION SETTING ATTRIBUTE TEXT
 * has been received. The device should respond immediately with OI_AVRCPTarget_GetAttrSettingTextReply() or
 * OI_AVRCPTarget_SendSimpleMetaReply().
 *
 * @param addr          Bluetooth device address of the CT sending the command.
 * @param transaction   Transaction ID of the received command.
 * @param count         Number of attributes for which the text settings are requested.
 * @param attrIDs       Pointer to a list of @a count attributes for which text is requested.
 *                      Attributes are #OI_AVRCP_PLAYER_APPLICATION_SETTINGS constants.
 */
typedef void (*OI_AVRCP_TARGET_GET_ATTRIBUTE_SETTING_TEXT_COMMAND_IND)(OI_BD_ADDR *addr,
                                                                       OI_UINT8 transaction,
                                                                       OI_UINT8 count,
                                                                       OI_UINT8 *attrIDs);

/**
 * A callback function of this type indicates that an enhanced AVRCP command
 * GET PLAYER APPLICATION SETTING ATTRIBUTE VALUE TEXT has been received. The device should respond immediately
 * with OI_AVRCPTarget_GetAttrSettingValueTextReply() or OI_AVRCPTarget_SendSimpleMetaReply().
 *
 * @param addr          Bluetooth device address of the CT sending the command.
 * @param transaction   Transaction ID of the received command.
 * @param attrID        Player application setting attribute ID as defined by #OI_AVRCP_PLAYER_APPLICATION_SETTINGS
 *                      in oi_avrcp_consts.h or received from the target.
 * @param count         Number of player application setting values for which corresponding strings are needed.
 * @param valueIDs      Pointer to a list of @a count player application setting value IDs for which
 *                      corresponding strings are needed. Value IDs are OI_AVRCP_*_STATUS_VALUES constants.
 */
typedef void (*OI_AVRCP_TARGET_GET_ATTRIBUTE_SETTING_VALUE_TEXT_COMMAND_IND)(OI_BD_ADDR *addr,
                                                                             OI_UINT8 transaction,
                                                                             OI_UINT8 attrID,
                                                                             OI_UINT8 count,
                                                                             OI_UINT8 *valueIDs);

/**
 * A callback function of this type indicates that an enhanced AVRCP command "INFORM DISPLAYABLE CHARACTER SET"
 * command has been received. The device should respond immediately with OI_AVRCPTarget_SendSimpleMetaReply().
 *
 * @param addr          This parameter indicates the Bluetooth device address
 *                      of the target device. An AVCTP connection to this device must
 *                      already exist.
 * @param transaction   Transaction ID of the received command.
 * @param count         Number of character sets displayable by the CT (1-255).
 * @param charSetIDs    List of character set IDs displayable by the CT. Character set IDs are OI_LANG_ENCODING
 *                      constants listed in oi_bt_assigned_nos.h.
 */
typedef void (*OI_AVRCP_TARGET_INFORM_CHARACTER_SET_COMMAND_IND)(OI_BD_ADDR *addr,
                                                                 OI_UINT8 transaction,
                                                                 OI_UINT8 count,
                                                                 OI_UINT16 *charSetIDs);

 /**
 * A callback function of this type indicates that an enhanced AVRCP command "INFORM BATTERY STATUS OF CT"
 * command has been received. The device should respond immediately with OI_AVRCPTarget_SendSimpleMetaReply().
 *
 * @param addr          This parameter indicates the Bluetooth device address
 *                      of the target device. An AVCTP connection to this device must
 *                      already exist.
 * @param transaction   Transaction ID of the received command.
 * @param batteryStatus Battery status of controller; an #OI_AVRCP_BATTERY_STATUS constant.
 */
typedef void (*OI_AVRCP_TARGET_INFORM_BATTERY_STATUS_OF_CT_COMMAND_IND)(OI_BD_ADDR *addr,
                                                                        OI_UINT8 transaction,
                                                                        OI_UINT8 batteryStatus);

 /**
 * A callback function of this type indicates that an enhanced AVRCP command GET_PLAY_STATUS
 * has been received. The device should respond immediately with OI_AVRCPTarget_PlayStatusReply() or
 * OI_AVRCPTarget_SendSimpleMetaReply().
 *
 * @param addr          Bluetooth device address of the CT sending the command.
 * @param transaction   Transaction ID of the received command.
 */
typedef void (*OI_AVRCP_TARGET_GET_PLAY_STATUS_COMMAND_IND)(OI_BD_ADDR *addr,
                                                            OI_UINT8 transaction);


/**
 * A callback function of this type indicates that an enhanced AVRCP command GET_ELEMENT_ATTRIBUTE
 * has been received. The device should respond immediately with OI_AVRCPTarget_GetElementAttributesReply() or
 * OI_AVRCPTarget_SendSimpleMetaReply().
 *
 * @param addr          Bluetooth device address of the CT sending the command.
 * @param transaction   Transaction ID of the received command.
 * @param id            Pointer to an 8-byte identifier specifying a media element on Target.
 *                      The only valid identifier is @c PLAYING (0x0000000000000000).
 * @param count         Number of element attributes requested.
 * @param attributes    Pointer to a list of attribute IDs for the attributes to be retrieved.
 *                      These IDs are #OI_AVRCP_MEDIA_ATTRIBUTES constants.
 */
typedef void (*OI_AVRCP_TARGET_GET_ELEMENT_ATTRIBUTE_COMMAND_IND)(OI_BD_ADDR *addr,
                                                                  OI_UINT8 transaction,
                                                                  OI_UINT8 *id,
                                                                  OI_UINT8 count,
                                                                  OI_UINT32 *attributes);

/**
 * A callback function of this type indicates that an enhanced AVRCP command REGISTER NOTIFICATION
 * has been received. The device should respond immediately with OI_AVRCPTarget_RegisterNotificationReply()
 * with INTERIM response type. When the status of the requested event changes, the device should send
 * OI_AVRCPTarget_RegisterNotificationReply() with CHANGED response type.
 *
 * @param addr             Bluetooth device address of the CT sending the command.
 * @param transaction      Transaction ID of the received command.
 * @param eventID          This parameter specifies an event for which the CT requires
 *                         notifications. An #OI_AVRCP_EVENT_ID constant.
 * @param playbackInterval This parameter specifies the playback time (in seconds) at which
 *                         the change in playback position should be notified. It is applicable
 *                         only for EventID EVENT_PLAYBACK_POS_CHANGED. For other events,
 *                         value of this parameter is ignored.
 */
typedef void (*OI_AVRCP_TARGET_EVENT_NOTIFICATION_COMMAND_IND)(OI_BD_ADDR *addr,
                                                               OI_UINT8 transaction,
                                                               OI_UINT8 eventID,
                                                               OI_UINT32 playbackInterval);

/**
 * A callback function of this type indicates that an enhanced AVRCP command REQUEST CONTINUING RESPONSE
 * has been received. The device should respond with OI_AVRCPTarget_FragmentReply().
 *
 * @param addr          Bluetooth device address of the CT sending the command.
 * @param transaction   Transaction ID of the received command.
 * @param pduID         Target PDU_ID for continue command; an #OI_AVRCP_PDU_ID constant.
 * @param currAttrIndex Index of the current attribute/element/value that was not sent completely in the previous response.
 * @param currDataPos   Index of the next byte of the current attribute/element/value data that was not sent correctly in
 *                      the previous response.
 *
 * @note @a currAttrIndex and @a currDataPos values can be used by Target application to calculate how much data was already sent in
 * previous responses and to update the data pointer sent in OI_AVRCPTarget_FragmentReply() accordingly.
 */
typedef void (*OI_AVRCP_TARGET_CONTINUE_RESPONSE_COMMAND_IND) (OI_BD_ADDR *addr,
                                                               OI_UINT8 transaction,
                                                               OI_UINT8 pduID,
                                                               OI_UINT16 currAttrIndex,
                                                               OI_UINT16 currDataPos);

/**
 * A callback function of this type indicates that an enhanced AVRCP command ABORT CONTINUING RESPONSE
 * has been received. The device should respond with OI_AVRCPTarget_SendSimpleMetaReply().
 *
 * @param addr          Bluetooth device address of the CT sending the command.
 * @param transaction   Transaction ID of the received command.
 * @param pduID         Target PDU_ID for continue command; an #OI_AVRCP_PDU_ID constant.
 */
typedef void (*OI_AVRCP_TARGET_ABORT_RESPONSE_COMMAND_IND) (OI_BD_ADDR *addr,
                                                            OI_UINT8 transaction,
                                                            OI_UINT8 pduID);

/**
 * A callback function of this type indicates that an enhanced AVRCP PASS THROUGH Command (vendor unique)
 * for Group Navigation has been received. The device should respond with OI_AVRCPTarget_GroupNavigationReply().
 *
 * @param addr           Bluetooth device address of the CT sending the command.
 * @param transaction    Transaction ID of the received command.
 * @param vendorUniqueID Indicates Next or Previous Group (see #OI_AVRCP_GROUP_NAVIGATION in oi_avrcp_consts.h).
 * @param buttonPress    Indicates whether button is pushed or released (TRUE for pushed, FALSE for released).
 */
typedef void (*OI_AVRCP_TARGET_GROUP_NAVIGATION_COMMAND_IND) (OI_BD_ADDR *addr,
                                                              OI_UINT8 transaction,
                                                              OI_UINT16 vendorUniqueID,
                                                              OI_BOOL buttonPress);

/**
 * A structure of this type is used to group target callback functions
 * that would handle AVRCP metadata commands.
 */
typedef struct {
    OI_AVRCP_TARGET_CAPABILITY_COMMAND_IND capabilityCommandInd;
    OI_AVRCP_TARGET_LIST_ATTRIBUTES_COMMAND_IND listAttrCommandInd;
    OI_AVRCP_TARGET_LIST_ATTRIBUTE_VALUES_COMMAND_IND listAttrValuesCommandInd;
    OI_AVRCP_TARGET_GET_ATTRIBUTE_VALUES_COMMAND_IND getAttrValuesCommandInd;
    OI_AVRCP_TARGET_SET_ATTRIBUTE_VALUES_COMMAND_IND setAttrValuesCommandInd;
    OI_AVRCP_TARGET_GET_ATTRIBUTE_SETTING_TEXT_COMMAND_IND getAttrSettingTextCommandInd;
    OI_AVRCP_TARGET_GET_ATTRIBUTE_SETTING_VALUE_TEXT_COMMAND_IND getAttrSettingValueTextCommandInd;
    OI_AVRCP_TARGET_INFORM_CHARACTER_SET_COMMAND_IND informCharacterSetCommandInd;
    OI_AVRCP_TARGET_INFORM_BATTERY_STATUS_OF_CT_COMMAND_IND informBatteryStatusOfCTCommandInd;
    OI_AVRCP_TARGET_GET_ELEMENT_ATTRIBUTE_COMMAND_IND getElementAttrCommandInd;
    OI_AVRCP_TARGET_GET_PLAY_STATUS_COMMAND_IND getPlayStatusCommandInd;
    OI_AVRCP_TARGET_EVENT_NOTIFICATION_COMMAND_IND eventNotificationCommandInd;
    OI_AVRCP_TARGET_CONTINUE_RESPONSE_COMMAND_IND continueResponseCommandInd;
    OI_AVRCP_TARGET_ABORT_RESPONSE_COMMAND_IND abortResponseCommandInd;
    OI_AVRCP_TARGET_GROUP_NAVIGATION_COMMAND_IND groupNavigationCommandInd;
} OI_AVRCP_TARGET_METADATA_CALLBACKS;

typedef struct {
    OI_AVRCP_TARGET_COMMAND_IND commandInd;
    OI_AVRCP_TARGET_GENERIC_IND genericInd;
    OI_AVRCP_TARGET_METADATA_CALLBACKS *metadataCb;
} OI_AVRCP_TARGET_CALLBACKS;

/** This function registers an AVRCP target with the AVRCP profile. This should be called
 *  after calling OI_AVRCP_Register() to register the endpoint.
 * @param cb          pointer to target-specific callbacks
 * @param categories  This parameter represents SDP constants declaring which remote control
 *                    categories are supported. If multiple categories are supported, their
 *                    values should be ORed together. See oi_avrcp_consts.h.
 * @param companyID   points to a 3-byte array containing an IEEE company
 *                    identifier. If this is NULL, the AVC generic identifier 0xFFFFFF will be used
 *                    instead. This identifier is included in UNIT INFO responses.
 * @param strings     strings to register in service record
 * @return            OI_OK if registration was successful, an error code otherwise
 */
OI_STATUS OI_AVRCP_RegisterTarget(const OI_AVRCP_TARGET_CALLBACKS *cb,
                                  OI_UINT16 categories,
                                  OI_BYTE *companyID,
                                  const OI_SDP_STRINGS *strings);


/**
 * Get the service record handle associated with this service.  
 * This can be used with e.g. OI_SDPDB_SetAttributeList to add
 * vendor-specific SDP attributes to the profile.
 *
 * @param handle  return the service record's handle
 */
OI_STATUS OI_AVRCPTarget_GetServiceRecord(OI_UINT32 *handle);

/** This function deregisters the AVRCP target from the AVRCP profile. This should be called
 * prior to deregistering the profile.
 */
OI_STATUS OI_AVRCP_DeregisterTarget( void );

#ifdef __cplusplus
}
#endif

/**@}*/

#endif
#endif /* FEATURE_BT_EXTPF_AV */
