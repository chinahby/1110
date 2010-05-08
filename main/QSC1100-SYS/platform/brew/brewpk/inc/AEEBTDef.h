#ifndef AEEBTDEF_H
#define AEEBTDEF_H
/*=============================================================================
FILE:         AEEBTDef.h

SERVICES:     IBT common definitions

DESCRIPTION:  This header file contains all definitions shared by all IBT 
              interfaces.

===============================================================================
        Copyright © 2007 QUALCOMM Incorporated.
               All Rights Reserved.
            QUALCOMM Proprietary/GTDR
=============================================================================*/

#include "AEEStdDef.h"

//**********************************************************************
//
// Definitions, Macros
//
//**********************************************************************

// Base Defines
#define AEEBT_MAX_LEN_PIN_CODE      16   // Maximum length of pincode
#define AEEBT_MAX_LEN_NICK_NAME     16   // Maximum length of nick name
#define AEEBT_MAX_LEN_DEVICE_NAME   30   // Maximum length of device name
#define AEEBT_MAX_LEN_SERVICE_NAME  30   // Maximum length of service name

/*=============================================================================
 BT Events
=============================================================================*/
#define AEEBT_EV_UNSUPPORTED     0    
#define AEEBT_EV_PIN_REQ         1           // PIN is needed for bonding
#define AEEBT_EV_DEV_UPDATE      2           // the device DB has been updated
#define AEEBT_EV_CONN_UP         3           // a new connection is established
#define AEEBT_EV_CONN_DOWN       4           // a connection is torn down
#define AEEBT_EV_RADIO_STATE     5           // a change in radio state

// Define a data type to refer BT Event
typedef uint32 BTEvent;

/*=============================================================================
 Bluetooth Result Codes
=============================================================================*/
#define AEEBT_RESULT_BASE          0x0000
#define AEEBT_RESULT_OBEX_BASE     0x1000
#define AEEBT_RESULT_OBEX_RSP_BASE (AEEBT_RESULT_OBEX_BASE | 0x0100)

#define AEEBT_RESULT_SUCCESS                     AEEBT_RESULT_BASE
#define AEEBT_RESULT_BAD_PARAM                   (AEEBT_RESULT_BASE + 1)
#define AEEBT_RESULT_NO_RESOURCE                 (AEEBT_RESULT_BASE + 2)
#define AEEBT_RESULT_DB_FULL                     (AEEBT_RESULT_BASE + 3)
#define AEEBT_RESULT_ENTRY_EXISTS                (AEEBT_RESULT_BASE + 4)
#define AEEBT_RESULT_ENTRY_NOT_FOUND             (AEEBT_RESULT_BASE + 5)
#define AEEBT_RESULT_UNKNOWN_ERR                 (AEEBT_RESULT_BASE + 6)
#define AEEBT_RESULT_INVALID_HANDLE              (AEEBT_RESULT_BASE + 7)
#define AEEBT_RESULT_OUT_OF_MEMORY               (AEEBT_RESULT_BASE + 8)
#define AEEBT_RESULT_ALREADY_REGISTERED          (AEEBT_RESULT_BASE + 9)
#define AEEBT_RESULT_NOT_REGISTERED              (AEEBT_RESULT_BASE + 10)
#define AEEBT_RESULT_ALREADY_CONNECTED           (AEEBT_RESULT_BASE + 11)
#define AEEBT_RESULT_NOT_CONNECTED               (AEEBT_RESULT_BASE + 12)
#define AEEBT_RESULT_CHANNEL_NOT_FOUND           (AEEBT_RESULT_BASE + 13)
#define AEEBT_RESULT_NOT_INITIALIZED             (AEEBT_RESULT_BASE + 14)
#define AEEBT_RESULT_INITIALIZATION_FAILED       (AEEBT_RESULT_BASE + 15)
#define AEEBT_RESULT_INVALID_OPERATION           (AEEBT_RESULT_BASE + 16)
#define AEEBT_RESULT_OPERATION_IN_PROGRESS       (AEEBT_RESULT_BASE + 17)
#define AEEBT_RESULT_NEGOTIATION_FAILURE         (AEEBT_RESULT_BASE + 18)
#define AEEBT_RESULT_REMOTE_TIMEOUT              (AEEBT_RESULT_BASE + 19)
#define AEEBT_RESULT_CONTINUE                    (AEEBT_RESULT_BASE + 20)
#define AEEBT_RESULT_FINAL                       (AEEBT_RESULT_BASE + 21)
#define AEEBT_RESULT_DEV_NOT_BONDED              (AEEBT_RESULT_BASE + 22)
#define AEEBT_RESULT_CONNECTION_REJECTED         (AEEBT_RESULT_BASE + 23)
#define AEEBT_RESULT_SERVER_CHANNEL_IN_USE       (AEEBT_RESULT_BASE + 24)
#define AEEBT_RESULT_NORMAL_DISCONNECT           (AEEBT_RESULT_BASE + 25) 
#define AEEBT_RESULT_REMOTE_DISCONNECT           (AEEBT_RESULT_BASE + 26) 
#define AEEBT_RESULT_CMD_NOT_ALLOWED             (AEEBT_RESULT_BASE + 27) 

// these errors are triggered by the response from the remote device
#define AEEBT_RESULT_OBEX_DISCONNECT_FAILED      AEEBT_RESULT_OBEX_BASE
#define AEEBT_RESULT_OBEX_PUT_RSP                (AEEBT_RESULT_OBEX_BASE + 1)
#define AEEBT_RESULT_OBEX_GET_RSP                (AEEBT_RESULT_OBEX_BASE + 2)
#define AEEBT_RESULT_OBEX_CONNECT_FAILED         (AEEBT_RESULT_OBEX_BASE + 3)
#define AEEBT_RESULT_OBEX_UNKNOWN                (AEEBT_RESULT_OBEX_BASE + 4)
#define AEEBT_RESULT_OBEX_FOLDER_NOT_FOUND       (AEEBT_RESULT_OBEX_BASE + 5)
#define AEEBT_RESULT_OBEX_BROWSING_NOT_ALLOWED   (AEEBT_RESULT_OBEX_BASE + 6)

// errors map directly to OBEX response codes received from remote device
#define AEEBT_RESULT_OBEX_PARTIAL_CONTENT        AEEBT_RESULT_OBEX_RSP_BASE
#define AEEBT_RESULT_OBEX_BAD_REQUEST            (AEEBT_RESULT_OBEX_RSP_BASE + 1)
#define AEEBT_RESULT_OBEX_UNAUTHORIZED           (AEEBT_RESULT_OBEX_RSP_BASE + 2)
#define AEEBT_RESULT_OBEX_FORBIDDEN              (AEEBT_RESULT_OBEX_RSP_BASE + 3)
#define AEEBT_RESULT_OBEX_NOT_FOUND              (AEEBT_RESULT_OBEX_RSP_BASE + 4)
#define AEEBT_RESULT_OBEX_METHOD_NOT_ALLOWED     (AEEBT_RESULT_OBEX_RSP_BASE + 5)
#define AEEBT_RESULT_OBEX_NOT_ACCEPTABLE         (AEEBT_RESULT_OBEX_RSP_BASE + 6)
#define AEEBT_RESULT_OBEX_LENGTH_REQUIRED        (AEEBT_RESULT_OBEX_RSP_BASE + 7)
#define AEEBT_RESULT_OBEX_PRECONDITION_FAILED    (AEEBT_RESULT_OBEX_RSP_BASE + 8)
#define AEEBT_RESULT_OBEX_UNSUPPORTED_MEDIA_TYPE (AEEBT_RESULT_OBEX_RSP_BASE + 9)
#define AEEBT_RESULT_OBEX_INTERNAL_SERVER_ERROR  (AEEBT_RESULT_OBEX_RSP_BASE + 10)
#define AEEBT_RESULT_OBEX_NOT_IMPLEMENTED        (AEEBT_RESULT_OBEX_RSP_BASE + 11)
#define AEEBT_RESULT_OBEX_SERVICE_UNAVAILABLE    (AEEBT_RESULT_OBEX_RSP_BASE + 12)
#define AEEBT_RESULT_OBEX_DATABASE_FULL          (AEEBT_RESULT_OBEX_RSP_BASE + 13)
#define AEEBT_RESULT_OBEX_DATABASE_LOCKED        (AEEBT_RESULT_OBEX_RSP_BASE + 14)
#define AEEBT_RESULT_OBEX_CLIENT_ABORTED_COMMAND (AEEBT_RESULT_OBEX_RSP_BASE + 15)

// Define a data type to refer BT Result
typedef uint32 BTResult;

/*=============================================================================
 Bluetooth Error returned by APIs
=============================================================================*/
#define AEEBT_EBASE     0x00009dfa
#define AEEBT_ENMASK    (AEEBT_EBASE + 1)
#define AEEBT_EPORT     (AEEBT_EBASE + 2)
#define AEEBT_EDBFULL   (AEEBT_EBASE + 3)
#define AEEBT_ERECFULL  (AEEBT_EBASE + 4)
#define AEEBT_ENOREC    (AEEBT_EBASE + 5)
#define AEEBT_ENOATTR   (AEEBT_EBASE + 6)
#define AEEBT_EENDOFREC (AEEBT_EBASE + 7) 

/*=============================================================================
 Class of Service
=============================================================================*/
#define AEEBT_COS_UNKNOWN                   0x0000
#define AEEBT_COS_LIMITED_DISCOVERABLE_MODE 0x0001
#define AEEBT_COS_NETWORKING                0x0002
#define AEEBT_COS_RENDERING                 0x0004
#define AEEBT_COS_CAPTURING                 0x0008
#define AEEBT_COS_OBJECT_TRANSFER           0x0010
#define AEEBT_COS_AUDIO                     0x0020
#define AEEBT_COS_TELEPHONY                 0x0040
#define AEEBT_COS_INFORMATION               0x0080
#define AEEBT_COS_POSITIONING               0x0100
#define AEEBT_COS_ALL                       0xFFFF

// Define a data type to refer Class of Service
typedef uint16    BTClassOfService;

/*=============================================================================
Device class
=============================================================================*/
#define AEEBT_DC_MISC                       0x0000
#define AEEBT_DC_COMPUTER                   0x0100
#define AEEBT_DC_PHONE                      0x0200
#define AEEBT_DC_LAN_ACCESS_POINT           0x0300
#define AEEBT_DC_AUDIO                      0x0400
#define AEEBT_DC_PERIPHERAL                 0x0500
#define AEEBT_DC_IMAGING                    0x0600
#define AEEBT_DC_UNCLASSIFIED               0x1F00

// Define a data type to refer Device Class
typedef uint16    BTDeviceClass;

/*=============================================================================
 Sub Device Class
=============================================================================*/
#define AEEBT_SDC_COMPUTER_DESKTOP_WORKSTATION        1
#define AEEBT_SDC_COMPUTER_SERVER_CLASS_COMPUTER      2
#define AEEBT_SDC_COMPUTER_LAPTOP                     3
#define AEEBT_SDC_COMPUTER_HANDHELD_PC_OR_PDA         4
#define AEEBT_SDC_COMPUTER_PALM_SIZED_PC_OR_PDA       5
#define AEEBT_SDC_COMPUTER_WEARABLE_COMPUTER          6
                                                      
#define AEEBT_SDC_PHONE_CELLULAR                      1
#define AEEBT_SDC_PHONE_CORDLESS                      2
#define AEEBT_SDC_PHONE_SMART_PHONE                   3
#define AEEBT_SDC_PHONE_WIRED_MODEM_OR_VOICE_GATEWAY  4
#define AEEBT_SDC_PHONE_COMMON_ISDN_ACCESS            5

#define AEEBT_SDC_AUDIO_HEADSET                       1
#define AEEBT_SDC_AUDIO_HANDSFREE                     2
#define AEEBT_SDC_AUDIO_MICROPHONE                    4
#define AEEBT_SDC_AUDIO_SPEAKER                       5
#define AEEBT_SDC_AUDIO_HEADPHONES                    6
#define AEEBT_SDC_AUDIO_PORTABLE                      7
#define AEEBT_SDC_AUDIO_CAR                           8
#define AEEBT_SDC_AUDIO_SETTOP_BOX                    9
#define AEEBT_SDC_AUDIO_HIFI                         10
#define AEEBT_SDC_AUDIO_VCR                          11
#define AEEBT_SDC_AUDIO_VIDEOCAM                     12
#define AEEBT_SDC_AUDIO_CAMCORDER                    13
#define AEEBT_SDC_AUDIO_VIDEO_MONITOR                14
#define AEEBT_SDC_AUDIO_VIDEO_N_SPEAKER              15
#define AEEBT_SDC_AUDIO_VIDEO_CONF                   16
#define AEEBT_SDC_AUDIO_GAMING                       18

#define AEEBT_SDC_PERIPHERAL_JOYSTICK                 1
#define AEEBT_SDC_PERIPHERAL_GAMEPAD                  2
#define AEEBT_SDC_PERIPHERAL_REMOTE_CONTROL           3
#define AEEBT_SDC_PERIPHERAL_SENSING_DEVICE           4
#define AEEBT_SDC_PERIPHERAL_DIGITIZER_TABLET         5
#define AEEBT_SDC_PERIPHERAL_SIM_CARD_READER          6
                                                      
#define AEEBT_SDC_LAN_FULLY_AVAILABLE                 0
#define AEEBT_SDC_LAN_1_TO_17_PERCENT_UTILIZED        1
#define AEEBT_SDC_LAN_17_TO_33_PERCENT_UTILIZED       2
#define AEEBT_SDC_LAN_33_TO_50_PERCENT_UTILIZED       3
#define AEEBT_SDC_LAN_50_to_67_PERCENT_UTILIZED       4
#define AEEBT_SDC_LAN_67_TO_83_PERCENT_UTILIZED       5
#define AEEBT_SDC_LAN_83_TO_99_PERCENT_UTILIZED       6
#define AEEBT_SDC_LAN_NO_SERVICE_AVAILABLE            7

// Define a data type to refer Sub Device Class
typedef uint16    BTSubDeviceClass;

/*=============================================================================
 Service Class UUIDs
=============================================================================*/
#define AEEBT_SCU_SERVICE_DISCOVERY_SERVER                  0x1000
#define AEEBT_SCU_BROWSE_GROUP_DESCRIPTOR                   0x1001
#define AEEBT_SCU_PUBLIC_BROWSE_GROUP                       0x1002
#define AEEBT_SCU_SERIAL_PORT                               0x1101
#define AEEBT_SCU_LAN_ACCESS_PPP                            0x1102
#define AEEBT_SCU_DIALUP_NETWORKING                         0x1103
#define AEEBT_SCU_IR_MC_SYNC                                0x1104
#define AEEBT_SCU_OBEX_OBJECT_PUSH                          0x1105
#define AEEBT_SCU_OBEX_FILE_TRANSFER                        0x1106
#define AEEBT_SCU_IR_MC_SYNC_COMMAND                        0x1107
#define AEEBT_SCU_HEADSET                                   0x1108
#define AEEBT_SCU_CORDLESS_TELEPHONY                        0x1109
#define AEEBT_SCU_AUDIO_SOURCE                              0x110A
#define AEEBT_SCU_AUDIO_SINK                                0x110B
#define AEEBT_SCU_AV_REMOTE_CONTROL_TARGET                  0x110C
#define AEEBT_SCU_ADVANCED_AUDIO_DISTRIBUTION               0x110D
#define AEEBT_SCU_AV_REMOTE_CONTROL                         0x110E
#define AEEBT_SCU_VIDEO_CONFERENCING                        0x110F
#define AEEBT_SCU_INTERCOM                                  0x1110
#define AEEBT_SCU_FAX                                       0x1111
#define AEEBT_SCU_HEADSET_AUDIO_GATEWAY                     0x1112
#define AEEBT_SCU_WAP                                       0x1113
#define AEEBT_SCU_WAP_CLIENT                                0x1114
#define AEEBT_SCU_PANU                                      0x1115
#define AEEBT_SCU_NAP                                       0x1116
#define AEEBT_SCU_GN                                        0x1117
#define AEEBT_SCU_DIRECT_PRINTING                           0x1118
#define AEEBT_SCU_REFERENCE_PRINTING                        0x1119
#define AEEBT_SCU_IMAGING                                   0x111A
#define AEEBT_SCU_IMAGING_RESPONDER                         0x111B
#define AEEBT_SCU_IMAGING_AUTOMATIC_ARCHIVE                 0x111C
#define AEEBT_SCU_IMAGING_REFERENCED_OBJECTS                0x111D
#define AEEBT_SCU_HANDSFREE                                 0x111E
#define AEEBT_SCU_HANDSFREE_AUDIO_GATEWAY                   0x111F
#define AEEBT_SCU_DIRECT_PRINTING_REFERENCE_OBJECTS_SERVICE 0x1120
#define AEEBT_SCU_REFLECTED_UI                              0x1121
#define AEEBT_SCU_BASIC_PRINTING                            0x1122
#define AEEBT_SCU_PRINTING_STATUS                           0x1123
#define AEEBT_SCU_HUMAN_INTERFACE_DEVICE_SERVICE            0x1124
#define AEEBT_SCU_HARDCOPY_CABLE_REPLACEMENT                0x1125
#define AEEBT_SCU_HCR_PRINT                                 0x1126
#define AEEBT_SCU_HCR_SCAN                                  0x1127
#define AEEBT_SCU_COMMON_ISDN_ACCESS                        0x1128
#define AEEBT_SCU_VIDEO_CONFERENCING_GW                     0x1129
#define AEEBT_SCU_UDI_MT                                    0x112A
#define AEEBT_SCU_UDI_TA                                    0x112B
#define AEEBT_SCU_AUDIO_VIDEO                               0x112C
#define AEEBT_SCU_SIM_ACCESS                                0x112D
#define AEEBT_SCU_PBAP_PCE                                  0x112E
#define AEEBT_SCU_PBAP_PSE                                  0x112F
#define AEEBT_SCU_PNP_INFORMATION                           0x1200
#define AEEBT_SCU_GENERIC_NETWORKING                        0x1201
#define AEEBT_SCU_GENERIC_FILE_TRANSFER                     0x1202
#define AEEBT_SCU_GENERIC_AUDIO                             0x1203
#define AEEBT_SCU_GENERIC_TELEPHONY                         0x1204
#define AEEBT_SCU_UPNP_SERVICE                              0x1205
#define AEEBT_SCU_UPNP_IP_SERVICE                           0x1206
#define AEEBT_SCU_ESDP_UPNP_IP_PAN                          0x1300
#define AEEBT_SCU_ESDP_UPNP_IP_LAP                          0x1301
#define AEEBT_SCU_ESDP_UPNP_L2CAP                           0x1302
#define AEEBT_SCU_VIDEO_SOURCE                              0x1303
#define AEEBT_SCU_VIDEO_SINK                                0x1304
#define AEEBT_SCU_VIDEO_DISTRIBUTION                        0x1305
#define AEEBT_SCU_INVALID                                   0xFFFF

// Define a data type to refer Service Class UUID
typedef uint16  BTServiceClassUUID;

/*=============================================================================
 LMP (Link Manager Protocol) Features Bit position in the 8-byte bitmask
=============================================================================*/
#define AEEBT_LMP_FEAT_3_SLOT_PKTS_B         0
#define AEEBT_LMP_FEAT_5_SLOT_PKTS_B         1
#define AEEBT_LMP_FEAT_ENCRYPT_B             2
#define AEEBT_LMP_FEAT_SLOT_OFFSET_B         3
#define AEEBT_LMP_FEAT_TIMING_ACCURACY_B     4
#define AEEBT_LMP_FEAT_SWITCH_B              5
#define AEEBT_LMP_FEAT_HOLD_MODE_B           6
#define AEEBT_LMP_FEAT_SNIFF_MODE_B          7
#define AEEBT_LMP_FEAT_PARK_MODE_B           8
#define AEEBT_LMP_FEAT_RSSI_B                9
#define AEEBT_LMP_FEAT_AUTO_DATA_RATE_B      10
#define AEEBT_LMP_FEAT_SCO_LINK_B            11
#define AEEBT_LMP_FEAT_HV2_PKTS_B            12
#define AEEBT_LMP_FEAT_HV3_PKTS_B            13
#define AEEBT_LMP_FEAT_U_LAW_LOG_B           14
#define AEEBT_LMP_FEAT_A_LAW_LOG_B           15
#define AEEBT_LMP_FEAT_CVSD_B                16
#define AEEBT_LMP_FEAT_PAGING_SCHEME_B       17
#define AEEBT_LMP_FEAT_POWER_CTRL_B          18
#define AEEBT_LMP_FEAT_TRANS_SCO_DATA_B      19
#define AEEBT_LMP_FEAT_ESCO_LINK_B           31
#define AEEBT_LMP_FEAT_AFH_CAPABLE_SLAVE_B   35
#define AEEBT_LMP_FEAT_AFH_CLASS_SLAVE_B     36
#define AEEBT_LMP_FEAT_AFH_CAPABLE_MASTER_B  43
#define AEEBT_LMP_FEAT_AFH_CLASS_MASTER_B    44

/*=============================================================================
 LMP Company IDs
 "All companies that create a unique implementation of the Link Manager
 shall have their own CompId."
=============================================================================*/
#define AEEBT_COMPID_ERICSSON     0
#define AEEBT_COMPID_NOKIA        1
#define AEEBT_COMPID_INTEL        2
#define AEEBT_COMPID_IBM          3
#define AEEBT_COMPID_TOSHIBA      4
#define AEEBT_COMPID_3COM         5
#define AEEBT_COMPID_MS           6
#define AEEBT_COMPID_LUCENT       7
#define AEEBT_COMPID_MOTOROLA     8
#define AEEBT_COMPID_INFINEON     9
#define AEEBT_COMPID_CSR          10
#define AEEBT_COMPID_SILICONWAVE  11
#define AEEBT_COMPID_DIGIANSWER   12
#define AEEBT_COMPID_TI           13
#define AEEBT_COMPID_PARTHUS      14
#define AEEBT_COMPID_BROADCOM     15
#define AEEBT_COMPID_MITEL        16
#define AEEBT_COMPID_WIDCOMM      17
#define AEEBT_COMPID_ZEEVO        18
#define AEEBT_COMPID_ATMEL        19
#define AEEBT_COMPID_MITSUBISHI   20
#define AEEBT_COMPID_RTX          21
#define AEEBT_COMPID_KCTECH       22
#define AEEBT_COMPID_NEWLOGIC     23
#define AEEBT_COMPID_TRANSILICA   24
#define AEEBT_COMPID_ROHDESCHWARZ 25
#define AEEBT_COMPID_TTPCOM       26
#define AEEBT_COMPID_SIGNIA       27
#define AEEBT_COMPID_CONEXANT     28
#define AEEBT_COMPID_QUALCOMM     29
#define AEEBT_COMPID_INVENTEL     30
#define AEEBT_COMPID_AVMBERLIN    31
#define AEEBT_COMPID_BANDSPEED    32
#define AEEBT_COMPID_MANSELLA     33
#define AEEBT_COMPID_NEC          34
#define AEEBT_COMPID_WAVEPLUS     35
#define AEEBT_COMPID_ALCATEL      36
#define AEEBT_COMPID_PHILIPS      37
#define AEEBT_COMPID_CTECH        38
#define AEEBT_COMPID_RFMICRO      40
#define AEEBT_COMPID_HITACHI      41
#define AEEBT_COMPID_SYMBOLTECH   42
#define AEEBT_COMPID_MACRONIX     44
#define AEEBT_COMPID_GCT          45
#define AEEBT_COMPID_MEWTEL       47
#define AEEBT_COMPID_STMICROELEC  48
#define AEEBT_COMPID_SYNOPSYS     49
#define AEEBT_COMPID_COMMIL       51
#define AEEBT_COMPID_COMPACCESS   52
#define AEEBT_COMPID_ECLIPSE      53
#define AEEBT_COMPID_RENESAS      54
#define AEEBT_COMPID_MOBILIAN     55
#define AEEBT_COMPID_TERAX        56
#define AEEBT_COMPID_ISS          57
#define AEEBT_COMPID_MATSUSHITA   58
#define AEEBT_COMPID_GENNUM       59
#define AEEBT_COMPID_IPEXTREME    61
#define AEEBT_COMPID_SYSNCHIPS    62
#define AEEBT_COMPID_SEIKOEPSON   64

// Define a data type to refer LMP Company ID
typedef uint32  BTCompanyID;

/*=============================================================================
 Security setting for individual remote device
=============================================================================*/
#define AEEBT_SEC_NONE                     0 /* no security                  */
#define AEEBT_SEC_AUTHENTICATE             1 /* authentication only          */
#define AEEBT_SEC_AUTHENTICATE_AND_ENCRYPT 2 /* authentication & encryption  */

// Define a data type to refer BT Security Level
typedef uint32     BTSecurityLevel;

/*=============================================================================
 Connection state
=============================================================================*/
#define AEEBT_CONN_STATE_DISCONNECTED   0
#define AEEBT_CONN_STATE_CONNECTING     1
#define AEEBT_CONN_STATE_CONNECTED      2
#define AEEBT_CONN_STATE_DISCONNECTING  3
typedef uint32    BTConnectionState;

/*=============================================================================
 Connection mode
=============================================================================*/
#define AEEBT_CONN_MODE_ACTIVE           0
#define AEEBT_CONN_MODE_HOLD             1
#define AEEBT_CONN_MODE_SNIFF            2
#define AEEBT_CONN_MODE_PARK             3
typedef uint32     BTConnectionMode;

/*=============================================================================
 Modifiable fields in Device DB entries
=============================================================================*/
#define AEEBT_MDF_VALID_B       0x0001  // added or removed
#define AEEBT_MDF_BONDED_B      0x0002  // bonded or unbonded
#define AEEBT_MDF_SEC_B         0x0004  // security level changed
#define AEEBT_MDF_COD_B         0x0008  // class of device changed
#define AEEBT_MDF_NAME_B        0x0010  // BT name changed
#define AEEBT_MDF_NICKNAME_B    0x0020  // nickname changed
#define AEEBT_MDF_VAL1_B        0x0040  // user defined value1 changed
#define AEEBT_MDF_VAL2_B        0x0080  // user defined value2 changed
#define AEEBT_MDF_VAL3_B        0x0100  // user defined value3 changed
#define AEEBT_MDF_VAL4_B        0x0200  // user defined value4 changed
#define AEEBT_MDF_SVCS_B        0x0400  // services changed

// Define a data type to refer Modifiable Device Field
typedef uint32  BTModDevField;

/*=============================================================================
 Modifiable fields in Connection Status Info
=============================================================================*/
#define AEEBT_MCSF_MODE_B       0x0001  // Connection Mode changed
#define AEEBT_MCSF_SEC_B        0x0002  // Security level changed
#define AEEBT_MCSF_ROLE_B       0x0004  // Role changed
#define AEEBT_MCSF_APT_B        0x0010  // ACL Packet Types changed
#define AEEBT_MCSF_SPT_B        0x0020  // SCO Packet Types changed
#define AEEBT_MCSF_RSSI_B       0x0040  // RSSI changed

// Define a data type to refer Modifiable Connection Status Field
typedef uint32  BTModConnStatField;

/*=============================================================================
 OBEX data formats
=============================================================================*/
#define AEEBT_OBEX_FORMAT_VCARD_2_1  0x01
#define AEEBT_OBEX_FORMAT_VCARD_3_0  0x02
#define AEEBT_OBEX_FORMAT_VCAL_1_0   0x04
#define AEEBT_OBEX_FORMAT_ICAL_2_0   0x08
#define AEEBT_OBEX_FORMAT_VNOTE      0x10
#define AEEBT_OBEX_FORMAT_VMESSAGE   0x20
#define AEEBT_OBEX_FORMAT_ANY        0xFFFFFFFF

// Define a data type to refer OBEX data formats
typedef uint32  BTOBEXFormat;

/*=============================================================================
 BT Protocols
 See Bluetooth Assigned Numbers on SIG website
=============================================================================*/
#define AEEBT_PROT_SDP            0x0001
#define AEEBT_PROT_RFCOMM         0x0003
#define AEEBT_PROT_OBEX           0x0008
#define AEEBT_PROT_HIDP           0x0011
#define AEEBT_PROT_AVCTP          0x0017
#define AEEBT_PROT_AVDTP          0x0019
#define AEEBT_PROT_L2CAP          0x0100
#define AEEBT_PROT_INVALID        0xFFFF
typedef uint32 BTProtocol;

/*=============================================================================
 BT Radio States
=============================================================================*/
#define AEEBT_RADIO_OFF           0x01 // powered off
#define AEEBT_RADIO_IDLE          0x02 // powered on but doing nothing
#define AEEBT_RADIO_INQUIRING     0x04 // searching for other devices
#define AEEBT_RADIO_PAGING        0x08 // trying to connect to another device
#define AEEBT_RADIO_INQ_SCANNING  0x10 // detectable by other devices
#define AEEBT_RADIO_PAGE_SCANNING 0x20 // connectable
typedef uint32 BTRadioState;

/*=============================================================================
 UUID types
=============================================================================*/
#define AEEBT_UUID16              1
#define AEEBT_UUID32              2
#define AEEBT_UUID128             3
typedef uint32 BTUUIDType;


/*=============================================================================
 Bluetooth Device Address
=============================================================================*/
typedef struct
{
   uint8 uAddr[6];
}  BDAddress;

/*=============================================================================
 Class of Device (COD)
=============================================================================*/
typedef struct
{
   BTClassOfService   COS;
   BTDeviceClass      DevCls;
   BTSubDeviceClass   SubDevCls;

}  BTClassOfDevice;

/*=============================================================================
 UUID definitions
=============================================================================*/
typedef uint16   BTUUID16;
typedef uint32   BTUUID32;
typedef struct
{
   uint8          aUUIDByte[ 16 ];

}  BTUUID128;

typedef struct
{
   BTUUIDType     dType;
   uint8          uuid[1];

}  BTUUID;

/*=============================================================================
 LMP Features Bit Mask
=============================================================================*/
typedef struct
{
   uint8  bytes[ 8 ];

}  BTLMPFeatures;

/*=============================================================================
 LMP Version Information
=============================================================================*/
typedef struct
{
   uint32              VersNum;
   BTCompanyID         CompID;
   uint32              SubVersNum;

}  BTLMPVersion;

/*=============================================================================
 Device Information
=============================================================================*/
typedef struct
{
   BDAddress              bdAddr;
   BTLMPFeatures          LMPFeatures;
   BTLMPVersion           LMPVersion;
   BTClassOfDevice        COD;
   uint8                  szName[ AEEBT_MAX_LEN_DEVICE_NAME+1 ];

}  BTDeviceInfo;

/*=============================================================================
 Local Device Information
=============================================================================*/
typedef struct
{
   BTDeviceInfo         myInfo;
   BTSecurityLevel      mySecLevel;
}  BTLocalInfo;

/*=============================================================================
 Connection Status
=============================================================================*/
typedef struct
{
   BTConnectionMode     ConnMode;
   BTSecurityLevel      Security;
   boolean              bLocalIsMaster;

}  BTConnStatus;

/*=============================================================================
 BTConnEvData
=============================================================================*/
typedef struct
{
   BDAddress            bdAddr;
   BTServiceClassUUID   uuid;
}  BTConnEvData;

/*=============================================================================
 BTDevUpdateEvData
=============================================================================*/
typedef struct
{
   BDAddress            bdAddr;
   BTModDevField        modField;
}  BTDevUpdateEvData;

/*=============================================================================
 Notify Data
=============================================================================*/
typedef struct
{
   BTEvent              event;
   uint32               uTimeStamp;
}  BTNotifyInfo;


/*=============================================================================
 Helper functions
=============================================================================*/
static __inline boolean AEEBT_BD_ADDRS_EQUAL (const BDAddress* p1, 
                                              const BDAddress* p2)
{
   int i = sizeof (BDAddress);
   while (i-- > 0) { if (p1->uAddr[i] != p2->uAddr[i]) return FALSE; }
   return TRUE;
}

/*=============================================================================
DATA STRUCTURE DOCUMENTATION
===============================================================================
BTEvent

Description:
   The following definitions are possible values for BT events.  They are 
   broadcast to all applications who have registered with AEECLSID_BT_NOTIFIER
   to receive events with NMASK_BT.

   Events                  Associated data structure
   ---------------------------------------------------------
   AEEBT_EV_UNSUPPORTED  : none - app should never get this
   AEEBT_EV_PIN_REQ      : BTEventData.bdAddr
   AEEBT_EV_DEV_UPDATE   : BTEventData.devUpdateEv
   AEEBT_EV_CONN_UP      : BTEventData.bdAddr
   AEEBT_EV_CONN_DOWN    : BTEventData.bdAddr
   
===============================================================================
BTResult

Description:
   The following are the possible Bluetooth result codes

   AEEBT_RESULT_SUCCESS                       : Success 
   AEEBT_RESULT_BAD_PARAM                     : Invalid input parameters
                                                to the API
   AEEBT_RESULT_NO_RESOURCE                   : Not enough resource to complete
                                                current operation
   AEEBT_RESULT_DB_FULL                       : Device database is full 
   AEEBT_RESULT_ENTRY_EXISTS                  : An entry with same BD address 
                                              : Already exists in device database
   AEEBT_RESULT_ENTRY_NOT_FOUND               : There is no matching entry in
                                              : device database 
   AEEBT_RESULT_UNKNOWN_ERR                   : Unknown error
   AEEBT_RESULT_INVALID_HANDLE                : Invalid handle
   AEEBT_RESULT_OUT_OF_MEMORY                 : There is not enough memory  
   AEEBT_RESULT_ALREADY_REGISTERED            : Already registered 
   AEEBT_RESULT_NOT_REGISTERED                : Not registered 
   AEEBT_RESULT_ALREADY_CONNECTED             : Already connected 
   AEEBT_RESULT_NOT_CONNECTED                 : Not connected
   AEEBT_RESULT_CHANNEL_NOT_FOUND             : CID does not represent a current
                                                connection 
   AEEBT_RESULT_NOT_INITIALIZED               : Data not initialized
   AEEBT_RESULT_INITIALIZATION_FAILED         : Module initialization failed 
   AEEBT_RESULT_INVALID_OPERATION             : Operation not allowed at this time
   AEEBT_RESULT_OPERATION_IN_PROGRESS         : Another operation is still in progress
   AEEBT_RESULT_NEGOTIATION_FAILURE           : Error in negotiation
   AEEBT_RESULT_REMOTE_TIMEOUT                : Remote device timed out
   AEEBT_RESULT_CONTINUE                      : Continue processing OBEX request
   AEEBT_RESULT_FINAL                         : Indicates that an OBEX command or response 
                                                is complete 
   AEEBT_RESULT_DEV_NOT_BONDED                : Device is not bonded
   AEEBT_RESULT_CONNECTION_REJECTED           : Connection to the remote device is
                                              : rejected
   AEEBT_RESULT_SERVER_CHANNEL_IN_USE         : Specified server channel is in use
   AEEBT_RESULT_NORMAL_DISCONNECT             : Local disconnection
   AEEBT_RESULT_REMOTE_DISCONNECT             : Remote disconnection    
   
   AEEBT_RESULT_OBEX_DISCONNECT_FAILED        : A disconnect failed probably 
                                                because the connection did not
												exist              
   AEEBT_RESULT_OBEX_PUT_RSP                  : Error in response to a PUT command
   AEEBT_RESULT_OBEX_GET_RSP                  : Error in response to a GET command
   AEEBT_RESULT_OBEX_CONNECT_FAILED           : Connection request did not succeed
   AEEBT_RESULT_OBEX_UNKNOWN                  : Unrecognized OBEX command
   AEEBT_RESULT_OBEX_FOLDER_NOT_FOUND         : Indicates that folder specified for
                                                browsing is not found
   AEEBT_RESULT_OBEX_BROWSING_NOT_ALLOWED     : Indicates that an server does
                                              : not allow folder browsing
    
   AEEBT_RESULT_OBEX_PARTIAL_CONTENT          : Obex data is not complete
   AEEBT_RESULT_OBEX_BAD_REQUEST              : Bad obex request         
   AEEBT_RESULT_OBEX_UNAUTHORIZED             : Authentication was not successful 
   AEEBT_RESULT_OBEX_FORBIDDEN                : Obex forbidden
   AEEBT_RESULT_OBEX_NOT_FOUND                : Referenced resource not found 
   AEEBT_RESULT_OBEX_METHOD_NOT_ALLOWED       : function not allowed
   AEEBT_RESULT_OBEX_NOT_ACCEPTABLE           : Value in command not in range 
   AEEBT_RESULT_OBEX_LENGTH_REQUIRED          : Length header required in command  
   AEEBT_RESULT_OBEX_PRECONDITION_FAILED      : Obex recondition failed 
   AEEBT_RESULT_OBEX_UNSUPPORTED_MEDIA_TYPE   : Unsupported media type 
   AEEBT_RESULT_OBEX_INTERNAL_SERVER_ERROR    : Internal inconsistency
   AEEBT_RESULT_OBEX_NOT_IMPLEMENTED          : Unsupported feature
   AEEBT_RESULT_OBEX_SERVICE_UNAVAILABLE      : Unsupported service
   AEEBT_RESULT_OBEX_DATABASE_FULL            : Sync database full    
   AEEBT_RESULT_OBEX_DATABASE_LOCKED          : Sync database locked 
   AEEBT_RESULT_OBEX_CLIENT_ABORTED_COMMAND   : Server received abort command 

   These are Bluetooth Error codes returned by APIs
   AEEBT_ENMASK                               : Bad event notifier mask
   AEEBT_EPORT                                : Must map the port before setting
                                                HCI mode
   AEEBT_EDBFULL                              : Data base is full
   AEEBT_ERECFULL                             : Service record already full
   AEEBT_ENOREC                               : Service record does not exist
   AEEBT_ENOATTR                              : Service attribute does not exist  
   
===============================================================================
BTClassOfService

Description:
   The following are the possible values for class of service

   AEEBT_COS_UNKNOWN                  
   AEEBT_COS_LIMITED_DISCOVERABLE_MODE
   AEEBT_COS_NETWORKING               
   AEEBT_COS_RENDERING                
   AEEBT_COS_CAPTURING                
   AEEBT_COS_OBJECT_TRANSFER          
   AEEBT_COS_AUDIO                    
   AEEBT_COS_TELEPHONY                
   AEEBT_COS_INFORMATION              
   AEEBT_COS_POSITIONING              
   AEEBT_COS_ALL                      

===============================================================================
BTDeviceClass

Description:
   The following are the possible values for bluetooth device class

   AEEBT_DC_MISC             
   AEEBT_DC_COMPUTER         
   AEEBT_DC_PHONE            
   AEEBT_DC_LAN_ACCESS_POINT 
   AEEBT_DC_AUDIO            
   AEEBT_DC_PERIPHERAL       
   AEEBT_DC_IMAGING          
   AEEBT_DC_UNCLASSIFIED     
                 

===============================================================================
BTSubDeviceClass

Description:
   The following are the possible values for bluetooth sub device class

   AEEBT_SDC_COMPUTER_DESKTOP_WORKSTATION       
   AEEBT_SDC_COMPUTER_SERVER_CLASS_COMPUTER     
   AEEBT_SDC_COMPUTER_LAPTOP                    
   AEEBT_SDC_COMPUTER_HANDHELD_PC_OR_PDA        
   AEEBT_SDC_COMPUTER_PALM_SIZED_PC_OR_PDA      
   AEEBT_SDC_COMPUTER_WEARABLE_COMPUTER
   AEEBT_SDC_PHONE_CELLULAR                     
   AEEBT_SDC_PHONE_CORDLESS                     
   AEEBT_SDC_PHONE_SMART_PHONE                  
   AEEBT_SDC_PHONE_WIRED_MODEM_OR_VOICE_GATEWAY 
   AEEBT_SDC_PHONE_COMMON_ISDN_ACCESS           
   AEEBT_SDC_AUDIO_HEADSET                      
   AEEBT_SDC_AUDIO_HANDSFREE                    
   AEEBT_SDC_AUDIO_MICROPHONE                   
   AEEBT_SDC_AUDIO_SPEAKER                      
   AEEBT_SDC_AUDIO_HEADPHONES                   
   AEEBT_SDC_AUDIO_PORTABLE                     
   AEEBT_SDC_AUDIO_CAR                          
   AEEBT_SDC_AUDIO_SETTOP_BOX                   
   AEEBT_SDC_AUDIO_HIFI                         
   AEEBT_SDC_AUDIO_VCR                          
   AEEBT_SDC_AUDIO_VIDEOCAM                     
   AEEBT_SDC_AUDIO_CAMCORDER                    
   AEEBT_SDC_AUDIO_VIDEO_MONITOR                
   AEEBT_SDC_AUDIO_VIDEO_N_SPEAKER              
   AEEBT_SDC_AUDIO_VIDEO_CONF                   
   AEEBT_SDC_AUDIO_GAMING                       
   AEEBT_SDC_PERIPHERAL_JOYSTICK                
   AEEBT_SDC_PERIPHERAL_GAMEPAD                 
   AEEBT_SDC_PERIPHERAL_REMOTE_CONTROL          
   AEEBT_SDC_PERIPHERAL_SENSING_DEVICE          
   AEEBT_SDC_PERIPHERAL_DIGITIZER_TABLET        
   AEEBT_SDC_PERIPHERAL_SIM_CARD_READER         
   AEEBT_SDC_LAN_FULLY_AVAILABLE                
   AEEBT_SDC_LAN_1_TO_17_PERCENT_UTILIZED       
   AEEBT_SDC_LAN_17_TO_33_PERCENT_UTILIZED      
   AEEBT_SDC_LAN_33_TO_50_PERCENT_UTILIZED      
   AEEBT_SDC_LAN_50_to_67_PERCENT_UTILIZED      
   AEEBT_SDC_LAN_67_TO_83_PERCENT_UTILIZED      
   AEEBT_SDC_LAN_83_TO_99_PERCENT_UTILIZED      
   AEEBT_SDC_LAN_NO_SERVICE_AVAILABLE           
  
===============================================================================
BTServiceClassUUID

Description:
   The following are the possible values for service class UUID

   AEEBT_SCU_SERVICE_DISCOVERY_SERVER                  
   AEEBT_SCU_BROWSE_GROUP_DESCRIPTOR                   
   AEEBT_SCU_PUBLIC_BROWSE_GROUP                       
   AEEBT_SCU_SERIAL_PORT                               
   AEEBT_SCU_LAN_ACCESS_PPP                            
   AEEBT_SCU_DIALUP_NETWORKING                         
   AEEBT_SCU_IR_MC_SYNC                                
   AEEBT_SCU_OBEX_OBJECT_PUSH                          
   AEEBT_SCU_OBEX_FILE_TRANSFER                        
   AEEBT_SCU_IR_MC_SYNC_COMMAND                        
   AEEBT_SCU_HEADSET                                   
   AEEBT_SCU_CORDLESS_TELEPHONY                        
   AEEBT_SCU_AUDIO_SOURCE                              
   AEEBT_SCU_AUDIO_SINK                                
   AEEBT_SCU_AV_REMOTE_CONTROL_TARGET                  
   AEEBT_SCU_ADVANCED_AUDIO_DISTRIBUTION               
   AEEBT_SCU_AV_REMOTE_CONTROL                         
   AEEBT_SCU_VIDEO_CONFERENCING                        
   AEEBT_SCU_INTERCOM                                  
   AEEBT_SCU_FAX                                       
   AEEBT_SCU_HEADSET_AUDIO_GATEWAY                     
   AEEBT_SCU_WAP                                       
   AEEBT_SCU_WAP_CLIENT                                
   AEEBT_SCU_PANU                                      
   AEEBT_SCU_NAP                                       
   AEEBT_SCU_GN                                        
   AEEBT_SCU_DIRECT_PRINTING                           
   AEEBT_SCU_REFERENCE_PRINTING                        
   AEEBT_SCU_IMAGING                                   
   AEEBT_SCU_IMAGING_RESPONDER                         
   AEEBT_SCU_IMAGING_AUTOMATIC_ARCHIVE                 
   AEEBT_SCU_IMAGING_REFERENCED_OBJECTS                
   AEEBT_SCU_HANDSFREE                                 
   AEEBT_SCU_HANDSFREE_AUDIO_GATEWAY                   
   AEEBT_SCU_DIRECT_PRINTING_REFERENCE_OBJECTS_SERVICE 
   AEEBT_SCU_REFLECTED_UI                              
   AEEBT_SCU_BASIC_PRINTING                            
   AEEBT_SCU_PRINTING_STATUS                           
   AEEBT_SCU_HUMAN_INTERFACE_DEVICE_SERVICE            
   AEEBT_SCU_HARDCOPY_CABLE_REPLACEMENT                
   AEEBT_SCU_HCR_PRINT                                 
   AEEBT_SCU_HCR_SCAN                                  
   AEEBT_SCU_COMMON_ISDN_ACCESS                        
   AEEBT_SCU_VIDEO_CONFERENCING_GW                     
   AEEBT_SCU_UDI_MT                                    
   AEEBT_SCU_UDI_TA                                    
   AEEBT_SCU_AUDIO_VIDEO                               
   AEEBT_SCU_SIM_ACCESS                                
   AEEBT_SCU_PBAP_PCE                                  
   AEEBT_SCU_PBAP_PSE                                  
   AEEBT_SCU_PNP_INFORMATION                           
   AEEBT_SCU_GENERIC_NETWORKING                        
   AEEBT_SCU_GENERIC_FILE_TRANSFER                     
   AEEBT_SCU_GENERIC_AUDIO                             
   AEEBT_SCU_GENERIC_TELEPHONY                         
   AEEBT_SCU_UPNP_SERVICE                              
   AEEBT_SCU_UPNP_IP_SERVICE                           
   AEEBT_SCU_ESDP_UPNP_IP_PAN                          
   AEEBT_SCU_ESDP_UPNP_IP_LAP                          
   AEEBT_SCU_ESDP_UPNP_L2CAP                           
   AEEBT_SCU_VIDEO_SOURCE                              
   AEEBT_SCU_VIDEO_SINK                                
   AEEBT_SCU_VIDEO_DISTRIBUTION                        
  
===============================================================================
BTCompanyID

Description:
   The following are the possible values for LMP company ID which is part
   of LMP feature.All companies that create a unique implementation of 
   the Link Manager shall have their own company ID

   AEEBT_COMPID_ERICSSON     
   AEEBT_COMPID_NOKIA        
   AEEBT_COMPID_INTEL        
   AEEBT_COMPID_IBM          
   AEEBT_COMPID_TOSHIBA      
   AEEBT_COMPID_3COM         
   AEEBT_COMPID_MS           
   AEEBT_COMPID_LUCENT       
   AEEBT_COMPID_MOTOROLA     
   AEEBT_COMPID_INFINEON     
   AEEBT_COMPID_CSR          
   AEEBT_COMPID_SILICONWAVE  
   AEEBT_COMPID_DIGIANSWER   
   AEEBT_COMPID_TI           
   AEEBT_COMPID_PARTHUS      
   AEEBT_COMPID_BROADCOM     
   AEEBT_COMPID_MITEL        
   AEEBT_COMPID_WIDCOMM      
   AEEBT_COMPID_ZEEVO        
   AEEBT_COMPID_ATMEL        
   AEEBT_COMPID_MITSUBISHI   
   AEEBT_COMPID_RTX          
   AEEBT_COMPID_KCTECH       
   AEEBT_COMPID_NEWLOGIC     
   AEEBT_COMPID_TRANSILICA   
   AEEBT_COMPID_ROHDESCHWARZ 
   AEEBT_COMPID_TTPCOM       
   AEEBT_COMPID_SIGNIA       
   AEEBT_COMPID_CONEXANT     
   AEEBT_COMPID_QUALCOMM     
   AEEBT_COMPID_INVENTEL     
   AEEBT_COMPID_AVMBERLIN    
   AEEBT_COMPID_BANDSPEED    
   AEEBT_COMPID_MANSELLA     
   AEEBT_COMPID_NEC          
   AEEBT_COMPID_WAVEPLUS     
   AEEBT_COMPID_ALCATEL      
   AEEBT_COMPID_PHILIPS      
   AEEBT_COMPID_CTECH        
   AEEBT_COMPID_RFMICRO      
   AEEBT_COMPID_HITACHI      
   AEEBT_COMPID_SYMBOLTECH   
   AEEBT_COMPID_MACRONIX     
   AEEBT_COMPID_GCT          
   AEEBT_COMPID_MEWTEL       
   AEEBT_COMPID_STMICROELEC  
   AEEBT_COMPID_SYNOPSYS     
   AEEBT_COMPID_COMMIL       
   AEEBT_COMPID_COMPACCESS   
   AEEBT_COMPID_ECLIPSE      
   AEEBT_COMPID_RENESAS      
   AEEBT_COMPID_MOBILIAN     
   AEEBT_COMPID_TERAX        
   AEEBT_COMPID_ISS          
   AEEBT_COMPID_MATSUSHITA   
   AEEBT_COMPID_GENNUM       
   AEEBT_COMPID_IPEXTREME    
   AEEBT_COMPID_SYSNCHIPS    
   AEEBT_COMPID_SEIKOEPSON   

===============================================================================
BTSecurityLevel

Description:
   The following are the possible values for BT Security Level.

   AEEBT_SEC_NONE                      : No security will be enforced               
   AEEBT_SEC_AUTHENTICATE              : Requires authentication only 
   AEEBT_SEC_AUTHENTICATE_AND_ENCRYPT  : Requires both authentication and
                                         encryption

===============================================================================
BTConnectionMode

Description:
   The following are the possible values for BT connection mode.

   AEEBT_CONN_MODE_ACTIVE  : device is in active connection mode
   AEEBT_CONN_MODE_HOLD    : device is in hold mode 
   AEEBT_CONN_MODE_SNIFF   : device is in sniff mode
   AEEBT_CONN_MODE_PARK    : device is in park mode


===============================================================================
BTModDevField

Description:
   The following are modifiable fields in Device DB entries

   AEEBT_MDF_VALID_B       : added or removed  
   AEEBT_MDF_BONDED_B      : bonded or unbonded
   AEEBT_MDF_SEC_B         : security level changed    
   AEEBT_MDF_COD_B         : class of device changed     
   AEEBT_MDF_NAME_B        : BT name changed    
   AEEBT_MDF_NICKNAME_B    : nickname changed
   AEEBT_MDF_VAL1_B        : User defined value1 changed 
   AEEBT_MDF_VAL2_B        : User defined value2 changed
   AEEBT_MDF_VAL3_B        : User defined value3 changed
   AEEBT_MDF_VAL4_B        : User defined value4 changed
   AEEBT_MDF_SVCS_B        : Services changed 


===============================================================================

BTModConnStatField

Description:
   The following are modifiable Connection Status Field

   AEEBT_MCSF_MODE_B       : Connection Mode changed
   AEEBT_MCSF_SEC_B        : Security level changed
   AEEBT_MCSF_ROLE_B       : Role changed
   AEEBT_MCSF_APT_B        : ACL Packet Types changed
   AEEBT_MCSF_SPT_B        : SCO Packet Types changed
   AEEBT_MCSF_RSSI_B       : RSSI changed



===============================================================================
BTOBEXFormat

Description:
   The following are OBEX data formats

   AEEBT_OBEX_FORMAT_VCARD_2_1
   AEEBT_OBEX_FORMAT_VCARD_3_0
   AEEBT_OBEX_FORMAT_VCAL_1_0 
   AEEBT_OBEX_FORMAT_ICAL_2_0 
   AEEBT_OBEX_FORMAT_VNOTE    
   AEEBT_OBEX_FORMAT_VMESSAGE 
   AEEBT_OBEX_FORMAT_ANY      

===============================================================================
BTProtocol

Description:
   Bluetooth protocols used by various profiles.
   
   AEEBT_PROT_SDP
   AEEBT_PROT_RFCOMM
   AEEBT_PROT_OBEX
   AEEBT_PROT_HIDP
   AEEBT_PROT_AVCTP
   AEEBT_PROT_AVDTP
   AEEBT_PROT_L2CAP

===============================================================================
BTRadioState

Description:
   Various states of Bluetooth radio.
   
   AEEBT_RADIO_OFF           : radio is powered off 
   AEEBT_RADIO_IDLE          : radio is powered on but doing nothing
   AEEBT_RADIO_INQUIRING     : radio is in inquiring state; this is when 
                               local device is searching for other devices
   AEEBT_RADIO_PAGING        : radio is in paging state; this is when local
                               device is trying to connect to another device
   AEEBT_RADIO_INQ_SCANNING  : radio is in inquiry scanning state; this makes
                               local device detectable by other devices
   AEEBT_RADIO_PAGE_SCANNING : radio is in page scanning; this makes local
                               device connectable by another device

===============================================================================
BTUUIDType

Description:
   Possible types of UUID.  Bluetooth assigned UUIDs can be converted between
   the different types.  Custom UUIDs must be in 128-bit format.
   
   AEEBT_UUID16              : 16-bit UUID; most commonly used
   AEEBT_UUID32              : 32-bit UUID; least commonly used
   AEEBT_UUID128             : 128-bit UUID; must be used for custom UUIDs

===============================================================================
BDAddress

Description:
   Specifies the format of the Bluetooth Device (BD) address.

Definition:
   typedef struct
   {
      uint8 uAddr[6];
   }  BDAddress;

Members;
    uAddr      : Bluetooth device address

===============================================================================
BTLMPFeatures

Description:
   Specifies the bitmap mask representing the LMP features.

Definition:
   typedef struct
   {
      uint8  bytes[8];
   } BTLMPFeatures;

Members:
    bytes      : Each bit in the bytes represents
	             a certain Bluetooth LMP feature that is supported.
				  
===============================================================================
BTClassOfDevice

Description:
   Specifies the information contained in the Bluetooth Class of Device (COD).
   Bluetooth devices send COD when responding to Inquiry.

Definition:
   typedef struct
   {
      BTClassOfService   COS;
      BTDeviceClass      DevCls;
      BTSubDeviceClass   SubDevCls;
      
   } BTClassOfDevice;

Members:
   COS       : the class of service the device is (Telephony, Rendering,...)
   DevCls    : the device class this device belongs to (Phone, Imaging, ...)
   SubDevCls : the device sub class (Cell phone, Printer,...)

See Also:
   BTClassOfService
   BTDeviceClass
   BTSubDeviceClass
===============================================================================
BTUUIDs

Description:
   Individual UUID types, BTUUID16, BTUUID32, and BTUUID128, are used to
   represent specific UUIDs.  If a UUID type is not known ahead of time, it
   should be represented with the generic BTUUID type, which must be converted
   to the specific type if it's a 16- or 32-bit UUID.

Definitions:
   typedef uint16   BTUUID16;
   typedef uint32   BTUUID32;
   typedef struct
   {
      uint8          aUUIDByte[ 16 ];
   }  BTUUID128;
   
   typedef struct
   {
      BTUUIDType     dType;
      uint8          uuid[1];
   }  BTUUID;

Members:
   dType    : AEEBT_UUID16, AEEBT_UUID32, or AEEBT_UUID128
   uuid     : array of nSize bytes representing a UUID of type BTUUID16, 
                BTUUID32, or BTUUID128, depending on nSize
                
===============================================================================
BTLMPVersion

Description:
   LMP version information.

Definition:
   typedef struct
   {
      uint32              VersNum;
      BTCompanyID         CompID;
      uint32              SubVersNum;
   
   } BTLMPVersion;

Members:
   VersNum     : current LMP version number
   CompID      : Manufacturer ID.
   SubVersNum  : Sub version number   

See Also:
    BTCompanyID

===============================================================================
BTDeviceInfo

Description:
  Public information retrievable from any Bluetooth device.

Definition:
   typedef struct
   {
      BDAddress              bdAddr;
      BTLMPFeatures          LMPFeatures;
      BTLMPVersion           LMPVersion;
      BTClassOfDevice        COD;
      uint8                  szName[ AEEBT_MAX_LEN_DEVICE_NAME+1 ];
      int32                  numServices;
   
   } BTDeviceInfo;

Members:
   bdAddr      : the Bluetooth Device address of a device
   LMPFeatures : the LMP features supported by the device
   LMPVersion  : LMP version information including company ID
   COD         : class of device
   szName      : the Bluetoth name of the device

See Also:
   BDAddress
   BTLMPFeatures
   BTLMPVersion
   BTClassOfDevice
===============================================================================
BTLocalInfo

Description:
  Information about the local bluetooth device

Definition:
   typedef struct
   {
      BTDeviceInfo         myInfo;
      BTSecurityLevel      mySecLevel;
   }  BTLocalInfo;


Members:
   myInfo      : the BTDeviceInfo structure
   mySecLevel  : Security level. Possible values are
                 AEEBT_SEC_NONE 
                 AEEBT_SEC_AUTHENTICATE
                 AEEBT_SEC_AUTHENTICATE_AND_ENCRYPT
See Also:
   BTDeviceInfo
   BTSecurityLevel
===============================================================================
BTConnStatus

Description:
  Information about connection status of the local device

Definition:
   typedef struct
   {
      BTConnectionMode     ConnMode;
      BTSecurityLevel      Security;
      boolean              bLocalIsMaster;
   }  BTConnStatus;


Members:
   ConnMode       : Connection mode
   Security       : Security level
   bLocalIsMaster : TRUE if the role of local device is master in the

See Also:
   BTConnectionMode
   BTSecurityLevel

===============================================================================
BTConnEvData

Description:
  This event is sent to the application when a remote device is requesting 
  to connect to local device to use a particular service.

Definition:
   typedef struct
   {
      BDAddress            bdAddr;
      BTServiceClassUUID   uuid;
   }  BTConnEvData;

Members:
   bdAddr       : address of remote device
   uuid         : the service that remote device is requesting to use

See Also:
   BDAddress
   BTServiceClassUUID

==============================================================================
BTDevUpdateEvData

Description:
  Information about the modified field in the device database.
  
Definition:
   typedef struct
   {
      BDAddress            bdAddr;
      BTModDevField        modField;
   }  BTDevUpdateEvData;

Members:
   bdAddr       : Bluetooth device address of the device
   modField     : Indicates which field of device information structure is
                  modified 
See Also:
   BDAddress
   BTModDevField

==============================================================================
BTNotifyInfo

Description:
  Information to look up event.
  
Definition:
   typedef struct
   {
      BTEvent              event;
      uint32               uTimeStamp;
   }  BTNotifyInfo;

Members:
   event            : Bluetooth event
   uTimeStamp       : Time stamp of event. 

See Also:
    BTEvent

=============================================================================

=============================================================================
  MACROS  DOCUMENTATION
============================================================================
AEEBT_BD_ADDRS_EQUAL()

Description:
   This helper function returns TRUE if the 2 BD addresses are the same.
   
Prototype:
   __inline boolean AEEBT_BD_ADDRS_EQUAL (const BDAddress* pAddr1, 
                                          const BDAddress* pAddr2);
   
Parameters:
   pAddr1                : first BD address
   pAddr1                : second BD address
 
Return values:
  TRUE if contents of pAddr1 and pAddr2 are the same.

=============================================================================*/
#endif // AEEBTDEF_H
