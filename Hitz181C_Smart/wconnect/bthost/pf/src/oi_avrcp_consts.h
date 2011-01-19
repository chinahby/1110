#ifdef FEATURE_BT_EXTPF_AV
#ifndef _OI_AVRCP_CONSTS_H
#define _OI_AVRCP_CONSTS_H

/**
 * @file
 * This file contains types and constants used by the Audio/Video Remote Control Profile (AVRCP).
 */

/**********************************************************************************
  $Revision: #1 $
  Copyright 2002 - 2007 Open Interface North America, Inc. All rights reserved.
***********************************************************************************/

#include "oi_stddefs.h"

/** \addtogroup AVRC */
/**@{*/

#ifdef __cplusplus
extern "C" {
#endif

#define OI_AVRCP_MAX_TRANSACTIONS 16
#define OI_AVRCP_MAX_FRAME_LENGTH 512      /**< Maximum number of bytes in an individual AVRCP frame */

/* Spec-defined timeout values: differ by command type */
#define OI_AVRCP_TRCP OI_MSECONDS(100)
#define OI_AVRCP_TMTC OI_MSECONDS(200)
#define OI_AVRCP_TMTP OI_MSECONDS(1000)

enum {
    OI_AVRCP_CTYPE_CONTROL           = 0x0,
    OI_AVRCP_CTYPE_STATUS            = 0x1,
    OI_AVRCP_CTYPE_SPECIFIC_INQUIRY  = 0x2,
    OI_AVRCP_CTYPE_NOTIFY            = 0x3,
    OI_AVRCP_CTYPE_GENERAL_INQUIRY   = 0x4,
    /* Reserved */
    OI_AVRCP_RESPONSE_NOT_IMPLEMENTED    = 0x8,
    OI_AVRCP_RESPONSE_ACCEPTED           = 0x9,
    OI_AVRCP_RESPONSE_REJECTED           = 0xa,
    OI_AVRCP_RESPONSE_IN_TRANSITION      = 0xb,
    OI_AVRCP_RESPONSE_IMPLEMENTED        = 0xc,
    OI_AVRCP_RESPONSE_STABLE             = 0xc,
    OI_AVRCP_RESPONSE_CHANGED            = 0xd,
    /* Reserved */
    OI_AVRCP_RESPONSE_INTERIM            = 0xf,
    OI_AVRCP_CMD_RESPONSE_MAX            = 0xf
};

#define OI_AVRCP_IS_COMMAND(x) (((x) & 0x8) == 0)
#define OI_AVRCP_IS_RESPONSE(x) (((x) & 0x8) == 0x8)

enum {
    OI_AVRCP_OPCODE_UNIT_INFO         = 0x30,
    OI_AVRCP_OPCODE_SUBUNIT_INFO      = 0x31,
    OI_AVRCP_OPCODE_VENDOR_DEPENDENT  = 0x00,
    OI_AVRCP_OPCODE_PASS_THROUGH      = 0x7c
};

enum {
    OI_AVRCP_SUBUNIT_TYPE_MONITOR        = 0x00,
    OI_AVRCP_SUBUNIT_TYPE_AUDIO          = 0x01,
    OI_AVRCP_SUBUNIT_TYPE_PRINTER        = 0x02,
    OI_AVRCP_SUBUNIT_TYPE_DISC           = 0x03,
    OI_AVRCP_SUBUNIT_TYPE_TAPE_PLAYER    = 0x04,
    OI_AVRCP_SUBUNIT_TYPE_TUNER          = 0x05,
    OI_AVRCP_SUBUNIT_TYPE_CA             = 0x06,
    OI_AVRCP_SUBUNIT_TYPE_CAMERA         = 0x07,
    /* Reserved */
    OI_AVRCP_SUBUNIT_TYPE_PANEL          = 0x09,
    OI_AVRCP_SUBUNIT_TYPE_BULLETIN_BOARD = 0xa,
    OI_AVRCP_SUBUNIT_TYPE_CAMERA_STORAGE = 0xb,
    /* Reserved */
    OI_AVRCP_SUBUNIT_TYPE_VENDOR_UNIQUE  = 0x1c,
    /* Reserved */
    OI_AVRCP_SUBUNIT_TYPE_EXTENDED       = 0x1e,
    OI_AVRCP_SUBUNIT_TYPE_UNIT           = 0x1f, /**< Target entire unit; subunit id must be #OI_AVRCP_SUBUNIT_ID_IGNORE */
    OI_AVRCP_SUBUNIT_TYPE_MAX            = 0x1f
};

enum {
    OI_AVRCP_SUBUNIT_ID_0            = 0x0,
    OI_AVRCP_SUBUNIT_ID_1            = 0x1,
    OI_AVRCP_SUBUNIT_ID_2            = 0x2,
    OI_AVRCP_SUBUNIT_ID_3            = 0x3,
    OI_AVRCP_SUBUNIT_ID_4            = 0x4,
    OI_AVRCP_SUBUNIT_ID_EXTENDED     = 0x5,
    /* Reserved */
    OI_AVRCP_SUBUNIT_ID_IGNORE       = 0x7,  /**< Target entire unit; subunit type must be #OI_AVRCP_SUBUNIT_TYPE_UNIT */
    OI_AVRCP_SUBUNIT_ID_MAX          = 0x7
};

#define OI_AVRCP_SUBUNIT_INFO_PAGE_MAX 7
#define OI_AVRCP_SUBUNIT_INFO_PAGE_NUMBER(x) (((x & 0x3) << 4) | 0x7)   /**< IEEE 1394 AV/C  General Spec 4.0, section 11.3 */
#define OI_AVRCP_INFO_PAGE_DATA(unit_type, unit) (((unit_type & 0x1f) << 3) | (unit & 0x3))
enum {
    OI_AVRCP_OP_SELECT             = 0x00,
    OI_AVRCP_OP_UP                 = 0x01,
    OI_AVRCP_OP_DOWN               = 0x02,
    OI_AVRCP_OP_LEFT               = 0x03,
    OI_AVRCP_OP_RIGHT              = 0x04,
    OI_AVRCP_OP_RIGHT_UP           = 0x05,
    OI_AVRCP_OP_RIGHT_DOWN         = 0x06,
    OI_AVRCP_OP_LEFT_UP            = 0x07,
    OI_AVRCP_OP_LEFT_DOWN          = 0x08,
    OI_AVRCP_OP_ROOT_MENU          = 0x09,
    OI_AVRCP_OP_SETUP_MENU         = 0x0a,
    OI_AVRCP_OP_CONTENTS_MENU      = 0x0b,
    OI_AVRCP_OP_FAVORITE_MENU      = 0x0c,
    OI_AVRCP_OP_EXIT               = 0x0d,
    OI_AVRCP_OP_0                  = 0x20,
    OI_AVRCP_OP_1                  = 0x21,
    OI_AVRCP_OP_2                  = 0x22,
    OI_AVRCP_OP_3                  = 0x23,
    OI_AVRCP_OP_4                  = 0x24,
    OI_AVRCP_OP_5                  = 0x25,
    OI_AVRCP_OP_6                  = 0x26,
    OI_AVRCP_OP_7                  = 0x27,
    OI_AVRCP_OP_8                  = 0x28,
    OI_AVRCP_OP_9                  = 0x29,
    OI_AVRCP_OP_DOT                = 0x2a,
    OI_AVRCP_OP_ENTER              = 0x2b,
    OI_AVRCP_OP_CLEAR              = 0x2c,
    OI_AVRCP_OP_CHANNEL_UP         = 0x30,
    OI_AVRCP_OP_CHANNEL_DOWN       = 0x31,
    OI_AVRCP_OP_PREVIOUS_CHANNEL   = 0x32,
    OI_AVRCP_OP_SOUND_SELECT       = 0x33,
    OI_AVRCP_OP_INPUT_SELECT       = 0x34,
    OI_AVRCP_OP_DISPLAY_INFORMATION= 0x35,
    OI_AVRCP_OP_HELP               = 0x36,
    OI_AVRCP_OP_PAGE_UP            = 0x37,
    OI_AVRCP_OP_PAGE_DOWN          = 0x38,
    OI_AVRCP_OP_POWER              = 0x40,
    OI_AVRCP_OP_VOLUME_UP          = 0x41,
    OI_AVRCP_OP_VOLUME_DOWN        = 0x42,
    OI_AVRCP_OP_MUTE               = 0x43,
    OI_AVRCP_OP_PLAY               = 0x44,
    OI_AVRCP_OP_STOP               = 0x45,
    OI_AVRCP_OP_PAUSE              = 0x46,
    OI_AVRCP_OP_RECORD             = 0x47,
    OI_AVRCP_OP_REWIND             = 0x48,
    OI_AVRCP_OP_FAST_FORWARD       = 0x49,
    OI_AVRCP_OP_EJECT              = 0x4a,
    OI_AVRCP_OP_FORWARD            = 0x4b,
    OI_AVRCP_OP_BACKWARD           = 0x4c,
    OI_AVRCP_OP_ANGLE              = 0x50,
    OI_AVRCP_OP_SUBPICTURE         = 0x51,
    OI_AVRCP_OP_F1                 = 0x71,
    OI_AVRCP_OP_F2                 = 0x72,
    OI_AVRCP_OP_F3                 = 0x73,
    OI_AVRCP_OP_F4                 = 0x74,
    OI_AVRCP_OP_F5                 = 0x75,
    OI_AVRCP_OP_VENDOR_UNIQUE      = 0x7e,

    OI_AVRCP_OP_STATUS_MASK        = BIT7,
    OI_AVRCP_OP_CODE_MASK          = ~BIT7,
    OI_AVRCP_OP_PRESS              = 0,
    OI_AVRCP_OP_RELEASE            = BIT7
};

#define OI_AVRCP_OP_IS_PRESS(op) (((op) & OI_AVRCP_OP_STATUS_MASK) == OI_AVRCP_OP_PRESS)
#define OI_AVRCP_OP_IS_RELEASE(op) (((op) & OI_AVRCP_OP_STATUS_MASK) == OI_AVRCP_OP_RELEASE)
#define OI_AVRCP_OP_CODE(op) ((op) & OI_AVRCP_OP_CODE_MASK)

enum {
    OI_AVRCP_SDP_CATEGORY1  = BIT0,
    OI_AVRCP_SDP_PLAYER     = OI_AVRCP_SDP_CATEGORY1,
    OI_AVRCP_SDP_RECORDER   = OI_AVRCP_SDP_CATEGORY1,
    OI_AVRCP_SDP_CATEGORY2  = BIT1,
    OI_AVRCP_SDP_MONITOR    = OI_AVRCP_SDP_CATEGORY2,
    OI_AVRCP_SDP_AMPLIFIER  = OI_AVRCP_SDP_CATEGORY2,
    OI_AVRCP_SDP_CATEGORY3  = BIT2,
    OI_AVRCP_SDP_TUNER      = OI_AVRCP_SDP_CATEGORY3,
    OI_AVRCP_SDP_CATEGORY4  = BIT3,
    OI_AVRCP_SDP_MENU       = OI_AVRCP_SDP_CATEGORY4
};

/*
 * EXTENDED SUPPORT FOR AVRCP METADATA IN 1.3 profile
 */

#define OI_AVRCP_METADATA_ID 0x001958      /**< per BT AVRCP spec v1.3 */
#define OI_AVRCP_MINIMUM_SUPPORTED_EVENTS 2

/** This type is used to represent an AVRCP attribute for extended types:
 * a pointer to a pair of attribute ID (1 byte) and value (1 byte) **/
typedef OI_UINT8 *OI_AVRCP_ATTRIBUTE_PAIR;

enum OI_AVRCP_EVENT_ID {
    OI_AVRCP_EVENT_PLAYBACK_STATUS_CHANGED = 0x01,
    OI_AVRCP_EVENT_TRACK_CHANGED = 0x02,
    OI_AVRCP_EVENT_TRACK_REACHED_END = 0x03,
    OI_AVRCP_EVENT_TRACK_REACHED_START = 0x04,
    OI_AVRCP_EVENT_PLAYBACK_POS_CHANGED = 0x05,
    OI_AVRCP_EVENT_BATT_STATUS_CHANGED = 0x06,
    OI_AVRCP_EVENT_SYSTEM_STATUS_CHANGED = 0x07,
    OI_AVRCP_EVENT_PLAYER_APPLICATION_SETTING_CHANGED = 0x08
};

enum OI_AVRCP_PLAYER_APPLICATION_SETTINGS {
    OI_AVRCP_SETTING_ILLEGAL             = 0x0,
    OI_AVRCP_SETTING_EQUALIZER_STATUS    = 0x1,
    OI_AVRCP_SETTING_REPEAT_MODE_STATUS  = 0x2,
    OI_AVRCP_SETTING_SHUFFLE_STATUS      = 0x3,
    OI_AVRCP_SETTING_SCAN_STATUS         = 0x4
    /* 0x05 - 0x7F Reserved for future use */
    /* 0x80 - 0xFF Provided for TG driven static media player menu extension by CT */
};

enum OI_AVRCP_EQUALIZER_STATUS_VALUES {
    OI_AVRCP_EQUALIZER_STATUS_OFF = 0x1,
    OI_AVRCP_EQUALIZER_STATUS_ON  = 0x2
    /* 0x03 - 0xfF Reserved for future use */
};

enum OI_AVRCP_REPEAT_MODE_STATUS_VALUES {
    OI_AVRCP_REPEAT_MODE_OFF          = 0x1,
    OI_AVRCP_REPEAT_MODE_SINGLE_TRACK = 0x2,
    OI_AVRCP_REPEAT_MODE_ALL_TRACKS   = 0x3,
    OI_AVRCP_REPEAT_MODE_GROUP        = 0x4
    /* 0x05 - 0xFF Reserved for future use */
};

enum OI_AVRCP_SHUFFLE_MODE_STATUS_VALUES {
    OI_AVRCP_SHUFFLE_MODE_OFF        = 0x1,
    OI_AVRCP_SHUFFLE_MODE_ALL_TRACKS = 0x2,
    OI_AVRCP_SHUFFLE_MODE_GROUP      = 0x3
    /* 0x04 - 0xFF Reserved for future use */
};

enum OI_AVRCP_SCAN_MODE_STATUS_VALUES {
    OI_AVRCP_SCAN_MODE_OFF        = 0x1,
    OI_AVRCP_SCAN_MODE_ALL_TRACKS = 0x2,
    OI_AVRCP_SCAN_MODE_GROUP      = 0x3
    /* 0x04 - 0xFF Reserved for future use */
};

enum OI_AVRCP_PACKET_TYPE {
    OI_AVRCP_PDU_COMPLETE  = 0,
    OI_AVRCP_PDU_FIRST     = 1,
    OI_AVRCP_PDU_CONTINUE  = 2,
    OI_AVRCP_PDU_END       = 3
};

enum OI_AVRCP_CAPABILITY_ID {
    OI_AVRCP_CAPABILITY_COMPANY_ID       = 0x2,
    OI_AVRCP_CAPABILITY_EVENTS_SUPPORTED = 0x3
};

enum OI_AVRCP_PDU_ID {
    /*
     * Configuration PDUs
     */
    OI_AVRCP_GET_CAPABILITIES                           = 0x10,
    /*
     * Device setting PDUs
     */
    OI_AVRCP_LIST_PLAYER_APPLICATION_SETTING_ATTRIBUTES    = 0x11,
    OI_AVRCP_LIST_PLAYER_APPLICATION_SETTING_VALUES        = 0x12,
    OI_AVRCP_GET_CURRENT_PLAYER_APPLICATION_SETTING_VALUE  = 0x13,
    OI_AVRCP_SET_PLAYER_APPLICATION_SETTING_VALUE          = 0x14,
    OI_AVRCP_GET_PLAYER_APPLICATION_SETTING_ATTRIBUTE_TEXT = 0x15,
    OI_AVRCP_GET_PLAYER_APPLICATION_SETTING_VALUE_TEXT     = 0x16,
    OI_AVRCP_INFORM_DISPLAYABLE_CHARACTER_SET              = 0x17,
    OI_AVRCP_INFORM_BATTERY_STATUS_OF_CT                   = 0x18,
    /*
     * Media information PDUs
     */
    OI_AVRCP_GET_ELEMENT_ATTRIBUTES                     = 0x20,
    /*
     * Notification PDUs
     */
    OI_AVRCP_GET_PLAY_STATUS                            = 0x30,
    OI_AVRCP_REGISTER_NOTIFICATION                      = 0x31,
    /*
     * Continuing PDUs
     */
    OI_AVRCP_REQUEST_CONTINUING_RESPONSE                = 0x40,
    OI_AVRCP_ABORT_CONTINUING_RESPONSE                  = 0x41
};

/** List of error status codes sent with REJECTED response */
enum OI_AVRCP_RESPONSE_ERROR_CODE {
    OI_AVRCP_RESPONSE_ERROR_INVALID_PDU         = 0x00,
    OI_AVRCP_RESPONSE_ERROR_INVALID_PARAMETER   = 0x01,
    OI_AVRCP_RESPONSE_ERROR_PARAMETER_NOT_FOUND = 0x02,
    OI_AVRCP_RESPONSE_ERROR_INTERNAL_ERROR      = 0x03
};

enum OI_AVRCP_PLAYBACK_STATE {
    OI_AVRCP_PLAY_STOPPED = 0x00,
    OI_AVRCP_PLAY_PLAYING = 0x01,
    OI_AVRCP_PLAY_PAUSED = 0x02,
    OI_AVRCP_PLAY_FWD_SEEK = 0x03,
    OI_AVRCP_PLAY_REV_SEEK = 0x04,
    OI_AVRCP_PLAY_ERROR = 0xff
};

enum OI_AVRCP_BATTERY_STATUS {
    OI_AVRCP_BATTERY_STATUS_NORMAL = 0x00,
    OI_AVRCP_BATTERY_STATUS_WARNING = 0x01,
    OI_AVRCP_BATTERY_STATUS_CRITICAL = 0x02,
    OI_AVRCP_BATTERY_STATUS_EXTERNAL = 0x03,
    OI_AVRCP_BATTERY_STATUS_FULL_CHARGE = 0x04
};

enum OI_AVRCP_SYSTEM_STATUS {
    OI_AVRCP_SYSTEM_STATUS_POWER_ON  = 0x00,
    OI_AVRCP_SYSTEM_STATUS_POWER_OFF = 0x01,
    OI_AVRCP_SYSTEM_STATUS_UNPLUGGED = 0x02
};

enum OI_AVRCP_MEDIA_ATTRIBUTES {
    OI_AVRCP_MEDIA_ATTR_ILLEGAL = 0x0, /**< Illegal, should not be used */
    OI_AVRCP_MEDIA_ATTR_TITLE   = 0x1, /**< Title of the media, any text encoded in specified character set */
    OI_AVRCP_MEDIA_ATTR_ARTIST  = 0x2, /**< Name of the artist, any text encoded in specified character set */
    OI_AVRCP_MEDIA_ATTR_ALBUM   = 0x3, /*<* Name of the album, any text encoded in specified character set */
    OI_AVRCP_MEDIA_ATTR_TRACK_NUMBER = 0x4, /**< Number of the media (track number of the CD), numeric ASCII text with zero suppresses */
    OI_AVRCP_MEDIA_ATTR_TOTAL_TRACK_NUMBER  = 0x5, /**< Total number of the media (total track count of the CD), numeric ASCII text with zero suppresses */
    OI_AVRCP_MEDIA_ATTR_GENRE   = 0x6, /**< Genre, any text encoded in specified character set */
    OI_AVRCP_MEDIA_ATTR_PLAYING_TIME = 0x7 /**< Playing time in milliseconds, numeric ASCII text with zero suppresses (for example, 150000, meaning 2min30sec)*/

    /* 0x8-0xFFFFFFFF Reserved for future use  */
};

enum OI_AVRCP_GROUP_NAVIGATION {
    OI_AVRCP_NEXT_GROUP = 0x00,
    OI_AVRCP_PREV_GROUP = 0x01
};

#ifdef __cplusplus
}
#endif

/**@}*/

#endif
#endif /* FEATURE_BT_EXTPF_AV */
