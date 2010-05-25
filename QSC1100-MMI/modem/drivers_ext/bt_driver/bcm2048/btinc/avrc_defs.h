/*****************************************************************************
**
**  Name:       avrc_defs.h
**
**  Description:    AVRCP definitions
**
**  Copyright (c) 2006, Broadcom Corp., All Rights Reserved.
*****************************************************************************/
#ifndef _AVRC_DEFS_H
#define _AVRC_DEFS_H

/*****************************************************************************
**  constants
*****************************************************************************/
#define AVRC_PACKET_LEN         512 /* Per the spec, you must support 512 byte RC packets */

#define AVRC_META_PDU_OFFSET    4
#define AVRC_SUB_TYPE_LEN       4

/* command type codes */
#define AVRC_CMD_CTRL       0   /* Instruct a target to perform an operation */
#define AVRC_CMD_STATUS     1   /* Check a device’s current status */
#define AVRC_CMD_SPEC_INQ   2   /* Check whether a target supports a particular
                                   control command; all operands are included */
#define AVRC_CMD_NOTIF      3   /* Used for receiving notification of a change in a device’s state */
#define AVRC_CMD_GEN_INQ    4   /* Check whether a target supports a particular
                                   control command; operands are not included */

/* response type codes */
#define AVRC_RSP_NOT_IMPL   8   /* The target does not implement the command specified
                                   by the opcode and operand,
                                   or doesn’t implement the specified subunit */
#define AVRC_RSP_ACCEPT     9   /* The target executed or is executing the command */
#define AVRC_RSP_REJ        10  /* The target implements the command specified by the
                                   opcode but cannot respond because the current state
                                   of the target doesn’t allow it */
#define AVRC_RSP_IN_TRANS   11  /* The target implements the status command but it is
                                   in a state of transition; the status command may
                                   be retried at a future time */
#define AVRC_RSP_IMPL_STBL  12  /* For specific inquiry or general inquiy commands,
                                   the target implements the command; for status
                                   commands, the target returns stable and includes
                                   the status results */
#define AVRC_RSP_CHANGED    13  /* The response frame contains a notification that the
                                   target device’s state has changed */
#define AVRC_RSP_INTERIM    15  /* For control commands, the target has accepted the
                                   request but cannot return information within 100
                                   milliseconds; for notify commands, the target accepted
                                   the command, and will notify the controller of a change
                                   of target state at a future time */

/* subunit type */
#define AVRC_SUB_MONITOR    0x00    /* Monitor */
#define AVRC_SUB_AUDIO      0x01    /* Audio */
#define AVRC_SUB_PRINTER    0x02    /* Printer */
#define AVRC_SUB_DISC       0x03    /* Disc */
#define AVRC_SUB_TAPE       0x04    /* Tape recorder/player */
#define AVRC_SUB_TUNER      0x05    /* Tuner */
#define AVRC_SUB_CA         0x06    /* CA */
#define AVRC_SUB_CAMERA     0x07    /* Camera */
#define AVRC_SUB_PANEL      0x09    /* Panel */
#define AVRC_SUB_BB         0x0A    /* Bulletin Board */
#define AVRC_SUB_CAM_STOR   0x0B    /* Camera Storage */
#define AVRC_SUB_VENDOR     0x1C    /* Vendor unique */
#define AVRC_SUB_EXT        0x1E    /* Subunit type extended to next byte */
#define AVRC_SUB_UNIT       0x1F    /* Unit */

/* opcodes */
#define AVRC_OP_UNIT_INFO   0x30    /* Report unit information */
#define AVRC_OP_SUB_INFO    0x31    /* Report subunit information */
#define AVRC_OP_VENDOR      0x00    /* Vendor-dependent commands */
#define AVRC_OP_PASS_THRU   0x7C    /* panel subunit opcode */

/* Company ID's
*/
#define AVRC_CO_BLUETOOTH_SIG   0x00FFFFFF
#define AVRC_CO_WIDCOMM         0x00000361
#define AVRC_CO_BROADCOM        0x00001018
#define AVRC_CO_METADATA        0x00001958  /* Unique COMPANY ID for Metadata messages */

/* State flag for Passthrough commands
*/
#define AVRC_STATE_PRESS    0
#define AVRC_STATE_RELEASE  1

/* Operation ID list for Passthrough commands
*/
#define AVRC_ID_SELECT      0x00    /* select */
#define AVRC_ID_UP          0x01    /* up */
#define AVRC_ID_DOWN        0x02    /* down */
#define AVRC_ID_LEFT        0x03    /* left */
#define AVRC_ID_RIGHT       0x04    /* right */
#define AVRC_ID_RIGHT_UP    0x05    /* right-up */
#define AVRC_ID_RIGHT_DOWN  0x06    /* right-down */
#define AVRC_ID_LEFT_UP     0x07    /* left-up */
#define AVRC_ID_LEFT_DOWN   0x08    /* left-down */
#define AVRC_ID_ROOT_MENU   0x09    /* root menu */
#define AVRC_ID_SETUP_MENU  0x0A    /* setup menu */
#define AVRC_ID_CONT_MENU   0x0B    /* contents menu */
#define AVRC_ID_FAV_MENU    0x0C    /* favorite menu */
#define AVRC_ID_EXIT        0x0D    /* exit */
#define AVRC_ID_0           0x20    /* 0 */
#define AVRC_ID_1           0x21    /* 1 */
#define AVRC_ID_2           0x22    /* 2 */
#define AVRC_ID_3           0x23    /* 3 */
#define AVRC_ID_4           0x24    /* 4 */
#define AVRC_ID_5           0x25    /* 5 */
#define AVRC_ID_6           0x26    /* 6 */
#define AVRC_ID_7           0x27    /* 7 */
#define AVRC_ID_8           0x28    /* 8 */
#define AVRC_ID_9           0x29    /* 9 */
#define AVRC_ID_DOT         0x2A    /* dot */
#define AVRC_ID_ENTER       0x2B    /* enter */
#define AVRC_ID_CLEAR       0x2C    /* clear */
#define AVRC_ID_CHAN_UP     0x30    /* channel up */
#define AVRC_ID_CHAN_DOWN   0x31    /* channel down */
#define AVRC_ID_PREV_CHAN   0x32    /* previous channel */
#define AVRC_ID_SOUND_SEL   0x33    /* sound select */
#define AVRC_ID_INPUT_SEL   0x34    /* input select */
#define AVRC_ID_DISP_INFO   0x35    /* display information */
#define AVRC_ID_HELP        0x36    /* help */
#define AVRC_ID_PAGE_UP     0x37    /* page up */
#define AVRC_ID_PAGE_DOWN   0x38    /* page down */
#define AVRC_ID_POWER       0x40    /* power */
#define AVRC_ID_VOL_UP      0x41    /* volume up */
#define AVRC_ID_VOL_DOWN    0x42    /* volume down */
#define AVRC_ID_MUTE        0x43    /* mute */
#define AVRC_ID_PLAY        0x44    /* play */
#define AVRC_ID_STOP        0x45    /* stop */
#define AVRC_ID_PAUSE       0x46    /* pause */
#define AVRC_ID_RECORD      0x47    /* record */
#define AVRC_ID_REWIND      0x48    /* rewind */
#define AVRC_ID_FAST_FOR    0x49    /* fast forward */
#define AVRC_ID_EJECT       0x4A    /* eject */
#define AVRC_ID_FORWARD     0x4B    /* forward */
#define AVRC_ID_BACKWARD    0x4C    /* backward */
#define AVRC_ID_ANGLE       0x50    /* angle */
#define AVRC_ID_SUBPICT     0x51    /* subpicture */
#define AVRC_ID_F1          0x71    /* F1 */
#define AVRC_ID_F2          0x72    /* F2 */
#define AVRC_ID_F3          0x73    /* F3 */
#define AVRC_ID_F4          0x74    /* F4 */
#define AVRC_ID_F5          0x75    /* F5 */
#define AVRC_ID_VENDOR      0x7E    /* vendor unique */

/*****************************************************************************
**  Metadata transfer definitions
*****************************************************************************/

/* Define the Metadata Packet types 
*/
#define AVRC_META_PKT_SINGLE      0
#define AVRC_META_PKT_START       1
#define AVRC_META_PKT_CONTINUE    2
#define AVRC_META_PKT_END         3

/* Define the PDUs carried in the vendor dependant data
*/
#define AVRC_META_PDU_GET_CAPABILITIES          0x10
#define AVRC_META_PDU_LIST_PLAYER_APP_ATTR      0x11
#define AVRC_META_PDU_LIST_PLAYER_APP_VALUES    0x12
#define AVRC_META_PDU_GET_CUR_PLAYER_APP_VALUE  0x13
#define AVRC_META_PDU_SET_PLAYER_APP_VALUE      0x14
#define AVRC_META_PDU_GET_PLAYER_APP_ATTR_TEXT  0x15
#define AVRC_META_PDU_GET_PLAYER_APP_VALUE_TEXT 0x16
#define AVRC_META_PDU_INFORM_DISPLAY_CHAR_SET   0x17
#define AVRC_META_PDU_INFORM_BATTERY_STAT_OF_CT 0x18
#define AVRC_META_PDU_GET_ELEMENT_ATTR          0x20
#define AVRC_META_PDU_GET_PLAY_STATUS           0x30
#define AVRC_META_PDU_REGISTER_NOTIFICATION     0x31
#define AVRC_META_PDU_REQUEST_CONTINUATION_RSP  0x40
#define AVRC_META_PDU_ABORT_CONTINUATION_RSP    0x41

/* Define the vendor unique id carried in the pass through data
*/
#define AVRC_META_PDU_NEXT_GROUP                0x00
#define AVRC_META_PDU_PREV_GROUP                0x01

/* Define the Capability IDs
*/
#define AVRC_META_CAP_COMPANY_ID                0x02
#define AVRC_META_CAP_EVENTS_SUPPORTED          0x03
#define AVRC_COMPANY_ID_LEN                     3
#define AVRC_META_CAPABILITY_OFFSET             2

/* Define the Media Attribute IDs
*/
#define AVRC_META_ATTR_ID_TITLE                 0x01
#define AVRC_META_ATTR_ID_ARTIST                0x02
#define AVRC_META_ATTR_ID_ALBUM                 0x03
#define AVRC_META_ATTR_ID_TRACK_NUM             0x04
#define AVRC_META_ATTR_ID_NUM_TRACKS            0x05
#define AVRC_META_ATTR_ID_GENRE                 0x06
#define AVRC_META_ATTR_ID_PLAYING_TIME          0x07

/* Define the Player Application Settings IDs
*/
#define AVRC_META_PLAYER_SETTING_EQUALIZER      0x01
#define AVRC_META_PLAYER_SETTING_REPEAT         0x02
#define AVRC_META_PLAYER_SETTING_SHUFFLE        0x03
#define AVRC_META_PLAYER_SETTING_SCAN           0x04
#define AVRC_META_PLAYER_SETTING_LOW_MENU_EXT   0x80
#define AVRC_META_PLAYER_SETTING_HIGH_MENU_EXT  0xff

/* Define the possible values of the Player Application Settings
*/
#define AVRC_META_PLAYER_VAL_OFF                0x01
#define AVRC_META_PLAYER_VAL_ON                 0x02
#define AVRC_META_PLAYER_VAL_SINGLE_REPEAT      0x02
#define AVRC_META_PLAYER_VAL_ALL_REPEAT         0x03
#define AVRC_META_PLAYER_VAL_GROUP_REPEAT       0x04
#define AVRC_META_PLAYER_VAL_ALL_SHUFFLE        0x02
#define AVRC_META_PLAYER_VAL_GROUP_SHUFFLE      0x03
#define AVRC_META_PLAYER_VAL_ALL_SCAN           0x02
#define AVRC_META_PLAYER_VAL_GROUP_SCAN         0x03

/* Define the possible values of Battery Status PDU
*/
#define AVRC_META_BATTERY_STATUS_NORMAL         0x00
#define AVRC_META_BATTERY_STATUS_WARNING        0x01
#define AVRC_META_BATTERY_STATUS_CRITICAL       0x02
#define AVRC_META_BATTERY_STATUS_EXTERNAL       0x03
#define AVRC_META_BATTERY_STATUS_FULL_CHARGE    0x04

/* Define character set */
#define AVRC_META_CHAR_SET_SIZE                 2

/* Define the Media Attributes
*/
#define AVRC_META_MEDIA_ATTRIB_TITLE            0x01
#define AVRC_META_MEDIA_ATTRIB_ARTIST           0x02
#define AVRC_META_MEDIA_ATTRIB_ALBUM            0x03
#define AVRC_META_MEDIA_ATTRIB_TRACK_NUM        0x04
#define AVRC_META_MEDIA_ATTRIB_TOTAL_TRACK_NUM  0x05
#define AVRC_META_MEDIA_ATTRIB_GENRE            0x06
#define AVRC_META_MEDIA_ATTRIB_PLAYING_TIME     0x07        /* in miliseconds */

/* Define the possible values of play state
*/
#define AVRC_META_PLAYSTATE_RESP_MSG_SIZE       9
#define AVRC_META_PLAYSTATE_STOPPED             0x00
#define AVRC_META_PLAYSTATE_PLAYING             0x01
#define AVRC_META_PLAYSTATE_PAUSED              0x02
#define AVRC_META_PLAYSTATE_FWD_SEEK            0x03
#define AVRC_META_PLAYSTATE_REV_SEEK            0x04
#define AVRC_META_PLAYSTATE_ERROR               0xFF

/* Define the events that can be registered for notifications
*/
#define AVRC_META_EVT_PLAY_STATUS_CHANGE        0x01
#define AVRC_META_EVT_TRACK_CHANGE              0x02            
#define AVRC_META_EVT_TRACK_REACHED_END         0x03
#define AVRC_META_EVT_TRACK_REACHED_START       0x04
#define AVRC_META_EVT_PLAY_POS_CHANGED          0x05
#define AVRC_META_EVT_BATTERY_STATUS_CHANGE     0x06
#define AVRC_META_EVT_SYSTEM_STATUS_CHANGE      0x07
#define AVRC_META_EVT_APP_SETTING_CHANGE        0x08

#define AVRC_META_EVT_MSG_LEN_1                 0x01
#define AVRC_META_EVT_MSG_LEN_2                 0x02
#define AVRC_META_EVT_MSG_LEN_5                 0x05
#define AVRC_META_EVT_MSG_LEN_9                 0x09

/* Define the possible values of system status
*/
#define AVRC_META_SYSTEMSTATE_PWR_ON            0x00
#define AVRC_META_SYSTEMSTATE_PWR_OFF           0x01
#define AVRC_META_SYSTEMSTATE_PWR_UNPLUGGED     0x02

/* Define the possible error codes
*/
#define AVRC_META_ERROR_INVALID_COMMAND         0x00    /* Invalid command (PDU).*/
#define AVRC_META_ERROR_INVALID_PARAM           0x01    /* Invalid command (PDU with parameter ID) */
#define AVRC_META_ERROR_BAD_CONTENTS            0x02    /* Specified parameter not found*/
#define AVRC_META_ERROR_INTERNAL                0x03    /* Internal Error */

/*****************************************************************************
**  data type definitions
*****************************************************************************/

/*
This structure contains the header parameters of an AV/C message.
*/
typedef struct 
{
    UINT8   ctype;          /* Command type.  */
    UINT8   subunit_type;   /* Subunit type. */
    UINT8   subunit_id;     /* Subunit ID.  This value is typically ignored in AVRCP,
                             * except for VENDOR DEPENDENT messages when the value is
                             * vendor-dependent.  Value range is 0-7. */
} tAVRC_HDR;

/* This structure contains a UNIT INFO message. */
typedef struct
{
    tAVRC_HDR   hdr;        /* Message header. */
    UINT32      company_id; /* Company identifier. */
    UINT8       unit_type;  /* Unit type.  Uses the same values as subunit type. */
    UINT8       unit;       /* This value is vendor dependent and typically zero.  */
} tAVRC_MSG_UNIT;

/* This structure contains a SUBUNIT INFO message. */
typedef struct
{
    tAVRC_HDR   hdr;        /* Message header. */
    UINT8       subunit_type[AVRC_SUB_TYPE_LEN];
                            /* Array containing subunit type values.  */
    BOOLEAN     panel;      /* TRUE if the panel subunit type is in the 
                             * subunit_type array, FALSE otherwise. */
    UINT8       page;       /* Specifies which part of the subunit type table is
                             * returned.  For AVRCP it is typically zero.
                             * Value range is 0-7. */
} tAVRC_MSG_SUB;

/* This structure contains a VENDOR DEPENDENT message. */
typedef struct
{
    tAVRC_HDR   hdr;        /* Message header. */
    UINT32      company_id; /* Company identifier. */
    UINT8      *p_data;     /* Pointer to vendor dependent data. */
    UINT16       len;        /* Length in bytes of vendor dependent data. */
} tAVRC_MSG_VENDOR;

/* PASS THROUGH message structure */
typedef struct
{
    tAVRC_HDR   hdr;        /* hdr.ctype Unused.
                             * hdr.subunit_type Unused.
                             * hdr.subunit_id Unused. */
    UINT8       op_id;      /* Operation ID.  */
    UINT8       state;      /* Keypress state.  */
    UINT8      *p_data;     /* Pointer to data.  This parameter is only valid
                             * when the op_id is AVRC_ID_VENDOR.*/
    UINT8       len;        /* Length in bytes of data. This parameter is only
                             * valid when the op_id is AVRC_ID_VENDOR.*/
} tAVRC_MSG_PASS;

/* This is a union of all message type structures. */
typedef union
{
    tAVRC_HDR           hdr;    /* Message header. */
    tAVRC_MSG_UNIT      unit;   /* UNIT INFO message. */
    tAVRC_MSG_SUB       sub;    /* SUBUNIT INFO message. */
    tAVRC_MSG_VENDOR    vendor; /* VENDOR DEPENDENT message. */
    tAVRC_MSG_PASS      pass;   /* PASS THROUGH message. */
} tAVRC_MSG;

#endif
