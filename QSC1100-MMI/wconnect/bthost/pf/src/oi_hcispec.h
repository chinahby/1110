#ifndef _OI_HCISPEC_H
#define _OI_HCISPEC_H
/*===========================================================================

                        EDIT HISTORY FOR MODULE

  This section contains comments describing changes made to the module.
  Notice that changes are listed in reverse chronological order.

$Header: //source/qcom/qct/wconnect/bthost/pf/rel/00.00.26/src/oi_hcispec.h#1 $ 

$Log: $

===========================================================================*/
/**
@file  
 
 This file contains definitions for parameters specified in the HCI functional specification
 (section H:1 of the Bluetooth version 1.1 specification).

 Documentation for this file consists largely of references to the relevant section
 of the Bluetooth specification.
*/

/**********************************************************************************
  $Revision: #1 $
  Copyright 2002 - 2003 Open Interface North America, Inc. All rights reserved.
***********************************************************************************/

/** This definition sets the byte order for the bytestream module. HCI data is all little-endian. */
#define OI_HCI_BYTE_ORDER  OI_LITTLE_ENDIAN_BYTE_ORDER

/** This definition sets the length of the HCI command header to 3 bytes for a 2-byte opcode plus a 1-byte 
field indicating the length of the HCI command parameters to follow (in bytes). */
#define OI_HCI_CMD_HEADER_LEN          3

/** This definition sets the length of the HCI event header to 2 bytes for a 1-byte event code plus
a 1-byte field indicating the length of the parameters to follow (in bytes) */
#define OI_HCI_EVENT_HEADER_LEN        2

/** This definition sets the length of the HCI ACL header to 4 bytes for a 2-byte field for the connection
handle and flags plus a 2-byte field indicating the length of the ACL data field */
#define OI_HCI_ACL_DATA_HEADER_LEN     4

/** This definition sets the length of the HCI SCO header to 3 bytes for a 2-byte field for the connection
handle and flags plus a 1-byte field indicating the length of the SCO data field */
#define OI_HCI_SCO_DATA_HEADER_LEN     3

 
#define OI_HCI_MAX_EVENT_SIZE    (OI_HCI_EVENT_HEADER_LEN + 255)       /**< This definition sets the maximum size for an HCI event packet,
                                                                      (i.e., the size of the packet header plus the maximum size allowed by the length field). */
#define OI_HCI_MAX_COMMAND_SIZE  (OI_HCI_CMD_HEADER_LEN + 255)         /**< This definition sets the maximum size for an HCI command packet,
                                                                      (i.e., the size of the packet header plus the maximum size allowed by the length field). */
#define OI_HCI_MAX_ACL_PACKET_SIZE   (OI_HCI_ACL_DATA_HEADER_LEN + 0xFFFF) /**< This definition sets the maximum size for an HCI ACL packet,
                                                                      (i.e., the size of the packet header plus the maximum size allowed by the length field).
                                                                      In practice, ACL packets will be much smaller */
#define OI_HCI_MAX_SCO_PACKET_SIZE   (OI_HCI_SCO_DATA_HEADER_LEN + 255)    /**< This definition sets the maximum size for an HCI SCO packet,
                                                                      (i.e., the size of the packet header plus the maximum size allowed by the length field). */

/*
 * HCI packet types
 */
#define OI_HCI_COMMAND_PACKET      0x01     /**< HCI command packet type, characterizing packet types over the 
                                              UART and RS232 transports */
#define OI_HCI_ACL_DATA_PACKET     0x02     /**< HCI ACL data packet type, characterizing packet types over the 
                                              UART and RS232 transports */
#define OI_HCI_SCO_DATA_PACKET     0x03     /**< HCI SCO data packet type, characterizing packet types over the 
                                              UART and RS232 transports */
#define OI_HCI_EVENT_PACKET        0x04     /**< HCI event packet type, characterizing packet types over the 
                                              UART and RS232 transports */

/*
 * packet types used in the HCI 3-wire transport
 */
#define OI_HCI_ACK_PACKET             0x00     /**< packet type used in the H5 UART transport  */
#define OI_HCI_VENDOR_SPECIFIC_PACKET 0x0E     /**< packet type used in the H5 UART transport  */
#define OI_HCI_LINK_CONTROL_PACKET    0x0F     /**< packet type used in the H5 UART transport  */

/*
 *  LAP codes
 */
#define OI_LAP_GIAC    0x9E8B33            /**< LAP (lower address portion) for GIAC (general inquiry access code),
                                             as defined in section B.13.3 of the Bluetooth specification */
#define OI_LAP_LIAC    0x9E8B00            /**< LAP (lower address portion) for LIAC (limited inquiry access code) */

/*----------------------------------------------------------*/
/*                                                          */
/*  HCI command/events parameter definitions                */
/*                                                          */
/*----------------------------------------------------------*/

/*  ACL Packet_Type */
#define OI_DHCI_PKT_TYPE_DM1           (0x0008)                /**< ACL packet type, data medium, up to one time slot (B.4.4.3.1) */
#define OI_DHCI_PKT_TYPE_DH1           (0x0010)                /**< ACL packet type, data high, up to one time slot (B.4.4.3.2) */
#define OI_DHCI_PKT_TYPE_DM3           (0x0400)                /**< ACL packet type, data medium, up to three time slots (B.4.4.3.3) */
#define OI_DHCI_PKT_TYPE_DH3           (0x0800)                /**< ACL packet type, data high, up to three time slots (B.4.4.3.4) */
#define OI_DHCI_PKT_TYPE_DM5           (0x4000)                /**< ACL packet type, data medium, up to five time slots (B.4.4.3.5) */
#define OI_DHCI_PKT_TYPE_DH5           (0x8000)                /**< ACL packet type, data high, up to five time slots (B.4.4.3.6) */
#define OI_DHCI_ALL_PKT_TYPES                      \
                      ( OI_DHCI_PKT_TYPE_DM1 |     \
                        OI_DHCI_PKT_TYPE_DH1 |     \
                        OI_DHCI_PKT_TYPE_DM3 |     \
                        OI_DHCI_PKT_TYPE_DH3 |     \
                        OI_DHCI_PKT_TYPE_DM5 |     \
                        OI_DHCI_PKT_TYPE_DH5 )                 /**< all ACL packet type values ORed together */
/*  SCO Packet_Type */
#define OI_DHCI_PKT_TYPE_HV1           (0x0020)                /**< SCO HV1 packet type (B.4.4.2.1) */
#define OI_DHCI_PKT_TYPE_HV2           (0x0040)                /**< SCO HV2 packet type (B.4.4.2.2) */
#define OI_DHCI_PKT_TYPE_HV3           (0x0080)                /**< SCO HV3 packet type (B.4.4.2.3) */
#define OI_DHCI_ALL_SCO_PKT_TYPES                  \
                      ( OI_DHCI_PKT_TYPE_HV1 |     \
                        OI_DHCI_PKT_TYPE_HV2 |     \
                        OI_DHCI_PKT_TYPE_HV3 )                 /**< all SCO packet type values ORed together */

/*  Page_Scan_Repetition_Mode */
#define OI_DHCI_PAGESCAN_REPETITION_MODE_R0                (0x00)      /**< page scan repetition mode R0  (H:1.4.5.5) */
#define OI_DHCI_PAGESCAN_REPETITION_MODE_R1                (0x01)      /**< page scan repetition mode R1  (H:1.4.5.5) */
#define OI_DHCI_PAGESCAN_REPETITION_MODE_R2                (0x02)      /**< page scan repetition mode R2  (H:1.4.5.5) */

/*  Page_Scan_Mode */
#define OI_DHCI_PAGESCAN_MODE_MANDATORY                    (0x00)       /**< mandatory page scan mode  (H:1.4.5.5) */
#define OI_DHCI_PAGESCAN_MODE_OPT_MODE_1                   (0x01)       /**< optional page scan mode I  (H:1.4.5.5) */
#define OI_DHCI_PAGESCAN_MODE_OPT_MODE_2                   (0x02)       /**< optional page scan mode II  (H:1.4.5.5) */
#define OI_DHCI_PAGESCAN_MODE_OPT_MODE_3                   (0x03)       /**< optional page scan mode III  (H:1.4.5.5) */

/*  Allow_Role_Switch */
#define OI_DHCI_ALLOW_ROLE_SWITCH_NOTACCEPT                (0x00)       /**< role switch not accepted  (H:1.4.5.5) */
#define OI_DHCI_ALLOW_ROLE_SWITCH_ACCEPT                   (0x01)       /**< role switch not accepted  (H:1.4.5.5) */

/*  Role, Current_Role, New_Role */
#define OI_DHCI_ROLE_MASTER                                (0x00)       /**< master */
#define OI_DHCI_ROLE_SLAVE                                 (0x01)       /**< slave */

/*  Encryption_Enable */
#define OI_DHCI_ENCRYPTION_ENABLE_OFF                      (0x00)       /**< link-level encryption off  (H:1.4.5.16) */
#define OI_DHCI_ENCRYPTION_ENABLE_ON                       (0x01)       /**< link-level encryption on  (H:1.4.5.16) */

/*  Key_Flag */
#define OI_DHCI_KEY_FLAG_SEMI_PERMANENT                    (0x00)       /**< Use semi-permanent link keys  (H:1.4.5.18) */
#define OI_DHCI_KEY_FLAG_TEMPORARY                         (0x01)       /**< Use temporary link keys  (H:1.4.5.18) */

/*  Service_Type */
#define OI_DHCI_SERVICE_TYPE_NO_TRAFFIC                    (0x00)       /**< quality of service level  (D.6.3) */
#define OI_DHCI_SERVICE_TYPE_BEST_EFFORT                   (0x01)       /**< quality of service level  (D.6.3) */
#define OI_DHCI_SERVICE_TYPE_GUARANTEED                    (0x02)       /**< quality of service level  (D.6.3) */

/*  Link_Policy_Settings */
#define OI_DHCI_LINKPOLICY_DISABLE                         (0x0000)       /**< default: disables link policy settings (H:1.4.6.10) */
#define OI_DHCI_LINKPOLICY_ENABLE_SWITCH                   (0x0001)       /**< enables role switch (H:1.4.6.10) */
#define OI_DHCI_LINKPOLICY_ENABLE_HOLD                     (0x0002)       /**< enables hold mode (H:1.4.6.10) */
#define OI_DHCI_LINKPOLICY_ENABLE_SNIFF                    (0x0004)       /**< enables sniff mode (H:1.4.6.10) */
#define OI_DHCI_LINKPOLICY_ENABLE_PARK                     (0x0008)       /**< enables park mode (H:1.4.6.10) */

#define OI_DHCI_LINKPOLICY_POWER_SAVE_MASK                 (OI_DHCI_LINKPOLICY_ENABLE_HOLD  |   \
                                                            OI_DHCI_LINKPOLICY_ENABLE_SNIFF |   \
                                                            OI_DHCI_LINKPOLICY_ENABLE_PARK)

/*  Current_Mode */
#define OI_DHCI_CURRENT_MODE_ACTIVE                        (0x00)       /**< device currently active */
#define OI_DHCI_CURRENT_MODE_HOLD                          (0x01)       /**< device currently in hold mode */
#define OI_DHCI_CURRENT_MODE_SNIFF                         (0x02)       /**< device currently in sniff mode */
#define OI_DHCI_CURRENT_MODE_PARK                          (0x03)       /**< device currently in park mode */

/*  Event_Mask */
#define OI_DHCI_MASK_NO_EVTS                               (0x0000000000000000)       /**< All events are masked out. No events are enabled. */
#define OI_DHCI_MASK_INQUIRY_CMP                           (0x0000000000000001)       /**< masks out inquiry complete event */
#define OI_DHCI_MASK_INQUIRY_RESULT                        (0x0000000000000002)       /**< masks out inquiry result event */
#define OI_DHCI_MASK_CNCT_CMP                              (0x0000000000000004)       /**< masks out connection complete event */
#define OI_DHCI_MASK_CNCT_REQ                              (0x0000000000000008)       /**< masks out connection request event */
#define OI_DHCI_MASK_DISC_CMP                              (0x0000000000000010)       /**< masks out discovery complete event */
#define OI_DHCI_MASK_AUTHENT_CMP                           (0x0000000000000020)       /**< masks out authentication complete event */
#define OI_DHCI_MASK_REMOTE_NAME_REQ_CMP                   (0x0000000000000040)       /**< masks out remote name request complete event */
#define OI_DHCI_MASK_ENCRYPTION_CHG                        (0x0000000000000080)       /**< masks out encryption change event */
#define OI_DHCI_MASK_CHG_CNCT_LINK_KEY_CMP                 (0x0000000000000100)       /**< masks out change connection link key complete event */
#define OI_DHCI_MASK_MASTER_LINK_KEY_CMP                   (0x0000000000000200)       /**< masks out master link key complete event */
#define OI_DHCI_MASK_READ_RMT_FEATURES_CMP                 (0x0000000000000400)       /**< masks out read remote features complete event */
#define OI_DHCI_MASK_READ_RMT_VER_INFO_CMP                 (0x0000000000000800)       /**< masks out read remote version information complete event */
#define OI_DHCI_MASK_QOS_SETUP_CMP                         (0x0000000000001000)       /**< masks out quality of service setup complete event */
#define OI_DHCI_MASK_CMND_CMP                              (0x0000000000002000)       /**< masks out command complete event */
#define OI_DHCI_MASK_CMND_STATUS                           (0x0000000000004000)       /**< masks out command status event */
#define OI_DHCI_MASK_HARDWARE_ERROR                        (0x0000000000008000)       /**< masks out hardware error event */
#define OI_DHCI_MASK_FLUSH_OCCURRED                        (0x0000000000010000)       /**< masks out flush occurred event */
#define OI_DHCI_MASK_ROLE_CHG                              (0x0000000000020000)       /**< masks out role change event */
#define OI_DHCI_MASK_NUMBER_OF_CMP_PKTS                    (0x0000000000040000)       /**< masks out number of completed packets event */
#define OI_DHCI_MASK_MODE_CHG                              (0x0000000000080000)       /**< masks out mode change event */
#define OI_DHCI_MASK_RETURN_LINK_KEY                       (0x0000000000100000)       /**< masks out return link key event */
#define OI_DHCI_MASK_PIN_CODE_REQ                          (0x0000000000200000)       /**< masks out PIN code request event */
#define OI_DHCI_MASK_LINK_KEY_REQ                          (0x0000000000400000)       /**< masks out link key request event */
#define OI_DHCI_MASK_LINK_KEY_NOTIFICATION                 (0x0000000000800000)       /**< masks out link key notification event */
#define OI_DHCI_MASK_LOOPBACK_CMND                         (0x0000000001000000)       /**< masks out loopback command event */
#define OI_DHCI_MASK_DATA_BUFFER_OVERFLOW                  (0x0000000002000000)       /**< masks out data buffer overflow event */
#define OI_DHCI_MASK_MAX_SLOTS_CHG                         (0x0000000004000000)       /**< masks out maximum number of slots change event */
#define OI_DHCI_MASK_READ_CLOCK_OFSET_CMP                  (0x0000000008000000)       /**< masks out read clock offset complete event */
#define OI_DHCI_MASK_CNCT_PKT_TYPE_CHGD                    (0x0000000010000000)       /**< masks out connection packet type changed event */
#define OI_DHCI_MASK_QOS_VIOLATION                         (0x0000000020000000)       /**< masks out quality of service violation event */
#define OI_DHCI_MASK_PAGESCAN_MODE_CHG                     (0x0000000040000000)       /**< masks out page scan mode change event */
#define OI_DHCI_MASK_PAGESCAN_REP_MODE_CHG                 (0x0000000080000000)       /**< masks out page scan repetition mode change event */
#define OI_DHCI_MASK_ALL_EVTS_ENABLED                      (0x00000000ffffffff)       /**< default: no events masked out */
                                                        
/*  Auto-Accept_Flag */
#define OI_DHCI_AUTO_ACCEPT_FLAG_NOT                       (0x01)                     /**< disables automatic acceptance of connections */
#define OI_DHCI_AUTO_ACCEPT_FLAG_NO_SWITCH                 (0x02)                     /**< enables automatic acceptance of connections with role switching disabled or enabled */
#define OI_DHCI_AUTO_ACCEPT_FLAG_SWITCH                    (0x03)                     /**< enables automatic acceptance of connections with role switching enabled */
/*  PIN_Type */
#define OI_DHCI_PIN_TYPE_VARIABLE                          (0x00)                     /**< variable PIN */
#define OI_DHCI_PIN_TYPE_FIXED                             (0x01)                     /**< fixed PIN */
/*  Read/Delete_All_Flag */
#define OI_DHCI_ALL_FLAG_LINK_KEY_SPECIFY                  (0x00)                     /**< delete only specified link key */
#define OI_DHCI_ALL_FLAG_LINK_KEY_ALL                      (0x01)                     /**< delete all link keys */
/*  Scan_Enable */
#define OI_DHCI_SCAN_ENABLE_NO_SCAN                        (0x00)                     /**< inquiry scans and page scans disabled (H:1.4.7.17) */
#define OI_DHCI_SCAN_ENABLE_INQUIRY                        (0x01)                     /**< inquiry scans enabled and page scans disabled (H:1.4.7.17) */
#define OI_DHCI_SCAN_ENABLE_PAGE                           (0x02)                     /**< inquiry scans disabled and page scans enabled (H:1.4.7.17) */
#define OI_DHCI_SCAN_ENABLE_INQUIRY_PAGE                   (0x03)                     /**< inquiry scans and page scans enabled (H:1.4.7.17) */
/*  Authentication_Enable */
#define OI_DHCI_AUTHENT_DISABLE                            (0x00)                     /**< authentication disabled (H:1.4.7.23) */
#define OI_DHCI_AUTHENT_ENABLE                             (0x01)                     /**< authentication enabled (H:1.4.7.23) */
/*  Encryption_Mode */
#define OI_DHCI_ENCRYPTION_MODE_DISABLED                   (0x00)                     /**< encryption disabled */
#define OI_DHCI_ENCRYPTION_MODE_P2P                        (0x01)                     /**< encryption enabled for point-to-point packets */
#define OI_DHCI_ENCRYPTION_MODE_P2P_BC                     (0x02)                     /**< encryption enabled for point-to-point packets and broadcast packets */
/*  Voice_Channel_Setting */
#define OI_DHCI_VOICE_CHANNEL_CODING_LNAR               (0x0000)   /**<  voice channel setting 00XXXXXXXX  */
#define OI_DHCI_VOICE_CHANNEL_CODING_ULAW               (0x0100)   /**<  voice channel setting 01XXXXXXXX  */
#define OI_DHCI_VOICE_CHANNEL_CODING_ALAW               (0x0200)   /**<  voice channel setting 10XXXXXXXX  */
#define OI_DHCI_VOICE_CHANNEL_FORMAT_1CMP               (0x0000)   /**<  voice channel setting XX00XXXXXX  */
#define OI_DHCI_VOICE_CHANNEL_FORMAT_2CMP               (0x0040)   /**<  voice channel setting XX01XXXXXX  */
#define OI_DHCI_VOICE_CHANNEL_FORMAT_SM                 (0x0080)   /**<  voice channel setting XX10XXXXXX  */
#define OI_DHCI_VOICE_CHANNEL_SIZE_8BIT                 (0x0000)   /**<  voice channel setting XXXX0XXXXX  */
#define OI_DHCI_VOICE_CHANNEL_SIZE_16BIT                (0x0020)   /**<  voice channel setting XXXX1XXXXX  */
#define OI_DHCI_VOICE_CHANNEL_BIT_POS                   (0x001c)   /**<  voice channel setting XXXXXnnnXX  */
#define OI_DHCI_VOICE_CHANNEL_AIR_CSVD                  (0x0000)   /**<  voice channel setting XXXXXXXX00  */
#define OI_DHCI_VOICE_CHANNEL_AIR_ULAW                  (0x0001)   /**<  voice channel setting XXXXXXXX01  */
#define OI_DHCI_VOICE_CHANNEL_AIR_ALAW                  (0x0002)   /**<  voice channel setting XXXXXXXX10  */
#define OI_DHCI_VOICE_CHANNEL_DEFAULT                   (0x0060)   /**<  voice channel setting 0001100000  */
/*  Hold_Mode_Activity */
#define OI_DHCI_HOLD_MODE_ACT_DEFAULT                      (0x00)  /**< default: maintains current power state (H:1.4.7.36) */
#define OI_DHCI_HOLD_MODE_ACT_PAGE_SCAN                    (0x01)  /**< page scans suspended while in hold mode (H:1.4.7.36) */
#define OI_DHCI_HOLD_MODE_ACT_INQ_SCAN                     (0x02)  /**< inquiry scans suspended while in hold mode (H:1.4.7.36) */
#define OI_DHCI_HOLD_MODE_ACT_PER_INQ                      (0x04)  /**< periodic inquiry scans suspended while in hold mode (H:1.4.7.36) */
/*  Transmit power level type */
#define OI_DHCI_READ_CUR_TX_POWER_LEVEL                    (0x00)  /**< current transmit power level (H:1.4.7.37) */
#define OI_DHCI_READ_MAX_TX_POWER_LEVEL                    (0x01)  /**< maximum transmit power level (H:1.4.7.37) */
/*  Flow_Control_Enable */
#define OI_DHCI_FLOW_CONTROL_ENABLE_OFF                    (0x00)  /**< default: SCO flow control disabled (H:1.4.7.38) */
#define OI_DHCI_FLOW_CONTROL_ENABLE_ON                     (0x01)  /**< SCO flow control enabled (H:1.4.7.38) */
/*  Page_Scan_Period_Mode */
#define OI_DHCI_PAGESCAN_PRD_MODE_P0                       (0x00)  /**< default: page scan period mode P0 (H:1.4.7.48) */
#define OI_DHCI_PAGESCAN_PRD_MODE_P1                       (0x01)  /**< page scan period mode P1 (H:1.4.7.48) */
#define OI_DHCI_PAGESCAN_PRD_MODE_P2                       (0x02)  /**< page scan period mode P2 (H:1.4.7.48) */
/*  Loopback_Mode */
#define OI_DHCI_LOOPBACK_MODE_NO                           (0x00)  /**< default: no loopback mode enabled (H:1.4.10.1)  */
#define OI_DHCI_LOOPBACK_MODE_LOCAL                        (0x01)  /**< local loopback mode enabled (H:1.4.10.1) */
#define OI_DHCI_LOOPBACK_MODE_REMOTE                       (0x02)  /**< remote loopback mode enabled (H:1.4.10.1) */
/*  Link_Type */
#define OI_DHCI_LINK_TYPE_SCO_CNCT                         (0x00)  /**< link type: SCO connection */
#define OI_DHCI_LINK_TYPE_ACL_CNCT                         (0x01)  /**< link type: ACL connection */

/* masks for first two bytes of data packets */
#define OI_DHCI_DATA_PKT_HANDLE_MASK           (0x0fff)        /**< Bits 0-11 are the connection handle. A connection handle is a 12-bit value
                                                                 and therefore only consume the first half of the second byte used to express it.
                                                                 ACL data packets use the second half of that byte but SCO data packets leave 
                                                                 those four bits as reserved. This mask is applied to the 16-bit field at the beginning
                                                                 of a data packet. (H:1.4.4.3) */
#define OI_DHCI_DATA_PKT_PB_FLAG_MASK          (0x3000)        /**< Bits 12-13 are the packet boundary flag. This mask is applied to the 16-bit field at start
                                                                 of the data packet. (H:1.4.4.3) */
#define OI_DHCI_DATA_PKT_BC_FLAG_MASK          (0xC000)        /**< Bits 14-15 are broadcast flag. This mask is applied to the 16-bit field at start
                                                                 of the data packet. (H:1.4.4.3) */

/*  Packet_Boundary_Flag */
#define OI_DHCI_PKT_BOUNDARY_FLAG_CONTINUE     (0x1000)        /**< boundary flag indicating continuation of a higher-level message (H:1.4.4.3) */
#define OI_DHCI_PKT_BOUNDARY_FLAG_FIRST        (0x2000)        /**< boundary flag indicating that this is the first packet of a higher-level message (H:1.4.4.3) */

/*  Broadcast_Flag */
#define OI_DHCI_PKT_BROADCAST_FLAG_P2P         (0x0000)        /**< packets only sent point-to-point (H:1.4.4.3) */
#define OI_DHCI_PKT_BROADCAST_FLAG_ACTIVE      (0x4000)        /**< packets broadcast to all active slaves (H:1.4.4.3) */
#define OI_DHCI_PKT_BROADCAST_FLAG_PICONET     (0x8000)        /**< packets broadcast to all slaves in park beacon slot, thereby reaching slaves in park mode as well (H:1.4.4.3) */

/*  Link Key_Type (link key notification event) */
#define OI_DHCI_LINKKEY_COMBINATION_TYPE       (0x00)          /**< used as argument for link key notification event: New link key is combination key. */
#define OI_DHCI_LINKKEY_LOCAL_UNIT_TYPE        (0x01)          /**< used as argument for link key notification event: New link key is local unit key. */
#define OI_DHCI_LINKKEY_REMOTE_UNIT_TYPE       (0x02)          /**< used as argument for link key notification event: New link key is remote unit key. */

/***************************************************************************

    HCI status/error codes

***************************************************************************/

#define OI_DHCI_STATUS_SUCCESS                             (0x00)        /**< HCI success code, which is mapped by BLUEmagic 3.0 HCI software to OI_OK status code */
#define OI_DHCI_ERR_UNKNOWN_HCI_CMND                       (0x01)        /**< HCI error code, which is mapped by BLUEmagic 3.0 HCI software to corresponding OI_HCIERR_ error code */
#define OI_DHCI_ERR_NO_CNCT                                (0x02)        /**< HCI error code, which is mapped by BLUEmagic 3.0 HCI software to corresponding OI_HCIERR_ error code */
#define OI_DHCI_ERR_HARDWARE_FAILURE                       (0x03)        /**< HCI error code, which is mapped by BLUEmagic 3.0 HCI software to corresponding OI_HCIERR_ error code */
#define OI_DHCI_ERR_PAGE_TIMEOUT                           (0x04)        /**< HCI error code, which is mapped by BLUEmagic 3.0 HCI software to corresponding OI_HCIERR_ error code */
#define OI_DHCI_ERR_AUTHENT_FAILURE                        (0x05)        /**< HCI error code, which is mapped by BLUEmagic 3.0 HCI software to corresponding OI_HCIERR_ error code */
#define OI_DHCI_ERR_KEY_MISSING                            (0x06)        /**< HCI error code, which is mapped by BLUEmagic 3.0 HCI software to corresponding OI_HCIERR_ error code */
#define OI_DHCI_ERR_MEMORY_FULL                            (0x07)        /**< HCI error code, which is mapped by BLUEmagic 3.0 HCI software to corresponding OI_HCIERR_ error code */
#define OI_DHCI_ERR_CNCT_TIMEOUT                           (0x08)        /**< HCI error code, which is mapped by BLUEmagic 3.0 HCI software to corresponding OI_HCIERR_ error code */
#define OI_DHCI_ERR_MAX_NUM_CNCTS                          (0x09)        /**< HCI error code, which is mapped by BLUEmagic 3.0 HCI software to corresponding OI_HCIERR_ error code */
#define OI_DHCI_ERR_MAX_NUM_SCO_CNCTS                      (0x0a)        /**< HCI error code, which is mapped by BLUEmagic 3.0 HCI software to corresponding OI_HCIERR_ error code */
#define OI_DHCI_ERR_MAX_NUM_ACL_CNCTS                      (0x0b)        /**< HCI error code, which is mapped by BLUEmagic 3.0 HCI software to corresponding OI_HCIERR_ error code */
#define OI_DHCI_ERR_CMND_DISALLOWED                        (0x0c)        /**< HCI error code, which is mapped by BLUEmagic 3.0 HCI software to corresponding OI_HCIERR_ error code */
#define OI_DHCI_ERR_HOST_REJ_RESOURCES                     (0x0d)        /**< HCI error code, which is mapped by BLUEmagic 3.0 HCI software to corresponding OI_HCIERR_ error code */
#define OI_DHCI_ERR_HOST_REJ_SECURITY                      (0x0e)        /**< HCI error code, which is mapped by BLUEmagic 3.0 HCI software to corresponding OI_HCIERR_ error code */
#define OI_DHCI_ERR_HOST_REJ_PERSONAL_DEV                  (0x0f)        /**< HCI error code, which is mapped by BLUEmagic 3.0 HCI software to corresponding OI_HCIERR_ error code */
#define OI_DHCI_ERR_HOST_TIMEOUT                           (0x10)        /**< HCI error code, which is mapped by BLUEmagic 3.0 HCI software to corresponding OI_HCIERR_ error code */
#define OI_DHCI_ERR_UNSUPPORT_FEAT_PARAM                   (0x11)        /**< HCI error code, which is mapped by BLUEmagic 3.0 HCI software to corresponding OI_HCIERR_ error code */
#define OI_DHCI_ERR_INVALID_HCI_CMND_PARAM                 (0x12)        /**< HCI error code, which is mapped by BLUEmagic 3.0 HCI software to corresponding OI_HCIERR_ error code */
#define OI_DHCI_ERR_TERM_CNCT_USER_ENDED                   (0x13)        /**< HCI error code, which is mapped by BLUEmagic 3.0 HCI software to corresponding OI_HCIERR_ error code */
#define OI_DHCI_ERR_TERM_CNCT_LOW_RESOURCE                 (0x14)        /**< HCI error code, which is mapped by BLUEmagic 3.0 HCI software to corresponding OI_HCIERR_ error code */
#define OI_DHCI_ERR_TERM_CNCT_POWER_OFF                    (0x15)        /**< HCI error code, which is mapped by BLUEmagic 3.0 HCI software to corresponding OI_HCIERR_ error code */
#define OI_DHCI_ERR_TERM_BY_LOCAL_HOST                     (0x16)        /**< HCI error code, which is mapped by BLUEmagic 3.0 HCI software to corresponding OI_HCIERR_ error code */
#define OI_DHCI_ERR_REPEATED_ATTEMPTS                      (0x17)        /**< HCI error code, which is mapped by BLUEmagic 3.0 HCI software to corresponding OI_HCIERR_ error code */
#define OI_DHCI_ERR_PAIRING_NOT_ALLOWED                    (0x18)        /**< HCI error code, which is mapped by BLUEmagic 3.0 HCI software to corresponding OI_HCIERR_ error code */
#define OI_DHCI_ERR_UNKNOWN_LMP_PDU                        (0x19)        /**< HCI error code, which is mapped by BLUEmagic 3.0 HCI software to corresponding OI_HCIERR_ error code */
#define OI_DHCI_ERR_UNSUPPORTED_REMOTE_FEAT                (0x1a)        /**< HCI error code, which is mapped by BLUEmagic 3.0 HCI software to corresponding OI_HCIERR_ error code */
#define OI_DHCI_ERR_SCO_REJ                                (0x1b)        /**< HCI error code, which is mapped by BLUEmagic 3.0 HCI software to corresponding OI_HCIERR_ error code */
#define OI_DHCI_ERR_SCO_INTERVAL_REJ                       (0x1c)        /**< HCI error code, which is mapped by BLUEmagic 3.0 HCI software to corresponding OI_HCIERR_ error code */
#define OI_DHCI_ERR_SCO_AIR_MODE_REJ                       (0x1d)        /**< HCI error code, which is mapped by BLUEmagic 3.0 HCI software to corresponding OI_HCIERR_ error code */
#define OI_DHCI_ERR_INVALID_LMP_PARAMETER                  (0x1e)        /**< HCI error code, which is mapped by BLUEmagic 3.0 HCI software to corresponding OI_HCIERR_ error code */
#define OI_DHCI_ERR_UNSPECIFIED_ERROR                      (0x1f)        /**< HCI error code, which is mapped by BLUEmagic 3.0 HCI software to corresponding OI_HCIERR_ error code */
#define OI_DHCI_ERR_UNSUPPORTED_LMP_PARAM                  (0x20)        /**< HCI error code, which is mapped by BLUEmagic 3.0 HCI software to corresponding OI_HCIERR_ error code */
#define OI_DHCI_ERR_ROLE_CHANGE_NOT_ALLOWED                (0x21)        /**< HCI error code, which is mapped by BLUEmagic 3.0 HCI software to corresponding OI_HCIERR_ error code */
#define OI_DHCI_ERR_LMP_RESPONSE_TIMEOUT                   (0x22)        /**< HCI error code, which is mapped by BLUEmagic 3.0 HCI software to corresponding OI_HCIERR_ error code */
#define OI_DHCI_ERR_LMP_ERROR_TRANS_COLLISION              (0x23)        /**< HCI error code, which is mapped by BLUEmagic 3.0 HCI software to corresponding OI_HCIERR_ error code */
#define OI_DHCI_ERR_LMP_PDU_NOT_ALLOWED                    (0x24)        /**< HCI error code, which is mapped by BLUEmagic 3.0 HCI software to corresponding OI_HCIERR_ error code */
#define OI_DHCI_ERR_ENCRYPTION_MODE_NOT_ACCEPTABLE         (0x25)        /**< HCI error code, which is mapped by BLUEmagic 3.0 HCI software to corresponding OI_HCIERR_ error code */
#define OI_DHCI_ERR_UNIT_KEY_USED                          (0x26)        /**< HCI error code, which is mapped by BLUEmagic 3.0 HCI software to corresponding OI_HCIERR_ error code */
#define OI_DHCI_ERR_QOS_IS_NOT_SUPPORTED                   (0x27)        /**< HCI error code, which is mapped by BLUEmagic 3.0 HCI software to corresponding OI_HCIERR_ error code */
#define OI_DHCI_ERR_INSTANT_PASSED                         (0x28)        /**< HCI error code, which is mapped by BLUEmagic 3.0 HCI software to corresponding OI_HCIERR_ error code */
#define OI_DHCI_ERR_UNIT_KEY_PAIRING_UNSUPPORTED           (0x29)        /**< HCI error code, which is mapped by BLUEmagic 3.0 HCI software to corresponding OI_HCIERR_ error code */

/*-------------------------------------------------------------------------------------*/
/*  End of HCI commands/event parameter definitions                                    */
/*-------------------------------------------------------------------------------------*/

/*-------------------------------------------------------------------------------------*/
/*                                                                                     */
/*  HCI command definitions (opcodes, parameter lengths, and return parameter lengths) */
/*                                                                                     */
/*-------------------------------------------------------------------------------------*/


#define OI_DHCI_NOP_OPCODE                            0          /**< NOP is used by the host controller 
                                                                   to notify the host when the number of
                                                                   HCI command packets that can be outstanding
                                                                   has changed. */

/*  
H:1.4.5 Link Control Commands 
*/
#define OI_DHCI_INQUIRY_OPCODE                     (0x0401)     /**< H:1.4.5.1 Inquiry */
#define OI_DHCI_INQUIRY_PARM_LEN                        (5)     /**< H:1.4.5.1 Inquiry */

#define OI_DHCI_INQUIRY_CANCEL_OPCODE              (0x0402)     /**< H:1.4.5.2 Inquiry_Cancel */
#define OI_DHCI_INQUIRY_CANCEL_PARM_LEN                 (0)     /**< H:1.4.5.2 Inquiry_Cancel */

#define OI_DHCI_PERIODIC_INQ_MODE_OPCODE           (0x0403)     /**< H:1.4.5.3 Periodic_Inquiry_Mode */
#define OI_DHCI_PERIODIC_INQ_MODE_PARM_LEN              (9)     /**< H:1.4.5.3 Periodic_Inquiry_Mode */
#define OI_DHCI_PERIODIC_INQ_MODE_RETURN_LEN            (1)     /**< H:1.4.5.3 Periodic_Inquiry_Mode */

#define OI_DHCI_EXIT_PERIODIC_INQ_MODE_OPCODE      (0x0404)     /**< H:1.4.5.4 Exit_Periodic_Inquiry_Mode */
#define OI_DHCI_EXIT_PERIODIC_INQ_MODE_PARM_LEN         (0)     /**< H:1.4.5.4 Exit_Periodic_Inquiry_Mode */
#define OI_DHCI_EXIT_PERIODIC_INQ_MODE_RETURN_LEN       (1)     /**< H:1.4.5.4 Exit_Periodic_Inquiry_Mode */

#define OI_DHCI_CREATE_CNCT_OPCODE                 (0x0405)     /**< H:1.4.5.5 Create_Connection */
#define OI_DHCI_CREATE_CNCT_PARM_LEN                   (13)     /**< H:1.4.5.5 Create_Connection */

#define OI_DHCI_DISC_OPCODE                        (0x0406)     /**< H:1.4.5.6 Disconnect */
#define OI_DHCI_DISC_PARM_LEN                           (3)     /**< H:1.4.5.6 Disconnect */

#define OI_DHCI_ADD_SCO_CNCT_OPCODE                (0x0407)     /**< H:1.4.5.7 Add_SCO_Connection */
#define OI_DHCI_ADD_SCO_CNCT_PARM_LEN                   (4)     /**< H:1.4.5.7 Add_SCO_Connection */

#define OI_DHCI_ACCEPT_CNCT_REQ_OPCODE             (0x0409)     /**< H:1.4.5.8 Accept_Connection_Request */
#define OI_DHCI_ACCEPT_CNCT_REQ_PARM_LEN                (7)     /**< H:1.4.5.8 Accept_Connection_Request */

#define OI_DHCI_REJ_CNCT_REQ_OPCODE                (0x040a)     /**< H:1.4.5.9 Reject_Connection_Request */
#define OI_DHCI_REJ_CNCT_REQ_PARM_LEN                   (7)     /**< H:1.4.5.9 Reject_Connection_Request */

#define OI_DHCI_LINK_KEY_REQ_REPLY_OPCODE          (0x040b)     /**< H:1.4.5.10 Link_Key_Request_Reply */
#define OI_DHCI_LINK_KEY_REQ_REPLY_PARM_LEN            (22)     /**< H:1.4.5.10 Link_Key_Request_Reply */
#define OI_DHCI_LINK_KEY_REQ_REPLY_RETURN_LEN           (7)     /**< H:1.4.5.10 Link_Key_Request_Reply */

#define OI_DHCI_LINK_KEY_REQ_NEG_REPLY_OPCODE      (0x040c)     /**< H:1.4.5.11 Link_Key_Request_Negative_Reply */
#define OI_DHCI_LINK_KEY_REQ_NEG_REPLY_PARM_LEN         (6)     /**< H:1.4.5.11 Link_Key_Request_Negative_Reply */
#define OI_DHCI_LINK_KEY_REQ_NEG_REPLY_RETURN_LEN       (7)     /**< H:1.4.5.11 Link_Key_Request_Negative_Reply */

#define OI_DHCI_PIN_CODE_REQ_REPLY_OPCODE          (0x040d)     /**< H:1.4.5.12 PIN_Code_Request_Reply */
#define OI_DHCI_PIN_CODE_REQ_REPLY_PARM_LEN            (23)     /**< H:1.4.5.12 PIN_Code_Request_Reply */
#define OI_DHCI_PIN_CODE_REQ_REPLY_RETURN_LEN           (7)     /**< H:1.4.5.12 PIN_Code_Request_Reply */

#define OI_DHCI_PIN_CODE_REQ_NEG_REPLY_OPCODE      (0x040e)     /**< H:1.4.5.13 PIN_Code_Request_Negative_Reply */
#define OI_DHCI_PIN_CODE_REQ_NEG_REPLY_PARM_LEN         (6)     /**< H:1.4.5.13 PIN_Code_Request_Negative_Reply */
#define OI_DHCI_PIN_CODE_REQ_NEG_REPLY_RETURN_LEN       (6)     /**< H:1.4.5.13 PIN_Code_Request_Negative_Reply */

#define OI_DHCI_CHANGE_CNCT_PKT_TYPE_OPCODE        (0x040f)     /**< H:1.4.5.14 Change_Connection_Packet_Type */
#define OI_DHCI_CHANGE_CNCT_PKT_TYPE_PARM_LEN           (4)     /**< H:1.4.5.14 Change_Connection_Packet_Type */

#define OI_DHCI_AUTHENT_REQ_OPCODE                 (0x0411)     /**< H:1.4.5.15 Authentication_Requested */
#define OI_DHCI_AUTHENT_REQ_PARM_LEN                    (2)     /**< H:1.4.5.15 Authentication_Requested */

#define OI_DHCI_SET_CNCT_ENCRYPTION_OPCODE         (0x0413)     /**< H:1.4.5.16 Set_Connection_Encryption */
#define OI_DHCI_SET_CNCT_ENCRYPTION_PARM_LEN            (3)     /**< H:1.4.5.16 Set_Connection_Encryption */

#define OI_DHCI_CHANGE_CNCT_LINK_KEY_OPCODE        (0x0415)     /**< H:1.4.5.17 Change_Connection_Link_Key */
#define OI_DHCI_CHANGE_CNCT_LINK_KEY_PARM_LEN           (2)     /**< H:1.4.5.17 Change_Connection_Link_Key */

#define OI_DHCI_MASTER_LINK_KEY_OPCODE             (0x0417)     /**< H:1.4.5.18 Master_Link_Key */
#define OI_DHCI_MASTER_LINK_KEY_PARM_LEN                (1)     /**< H:1.4.5.18 Master_Link_Key */

#define OI_DHCI_REMOTE_NAME_REQ_OPCODE             (0x0419)     /**< H:1.4.5.19 Remote_Name_Request */
#define OI_DHCI_REMOTE_NAME_REQ_PARM_LEN               (10)     /**< H:1.4.5.19 Remote_Name_Request */

#define OI_DHCI_READ_REMOTE_FEATURE_OPCODE         (0x041b)     /**< H:1.4.5.20 Read_Remote_Supported_Features */
#define OI_DHCI_READ_REMOTE_FEATURE_PARM_LEN            (2)     /**< H:1.4.5.20 Read_Remote_Supported_Features */

#define OI_DHCI_READ_REMOTE_VER_INFO_OPCODE        (0x041d)     /**< H:1.4.5.21 Read_Remote_Version_Information */
#define OI_DHCI_READ_REMOTE_VER_INFO_PARM_LEN           (2)     /**< H:1.4.5.21 Read_Remote_Version_Information */

#define OI_DHCI_READ_CLOCK_OPCODE                  (0x041f)     /**< H:1.4.5.22 Read_Clock_Offset */
#define OI_DHCI_READ_CLOCK_PARM_LEN                     (2)     /**< H:1.4.5.22 Read_Clock_Offset */

/*  
H:1.4.6 Link Policy Commands 
*/
#define OI_DHCI_HOLD_MODE_OPCODE                   (0x0801)     /**< H:1.4.6.1 Hold_Mode */
#define OI_DHCI_HOLD_MODE_PARM_LEN                      (6)     /**< H:1.4.6.1 Hold_Mode */

#define OI_DHCI_SNIFF_MODE_OPCODE                  (0x0803)     /**< H:1.4.6.2 Sniff_Mode */
#define OI_DHCI_SNIFF_MODE_PARM_LEN                    (10)     /**< H:1.4.6.2 Sniff_Mode */

#define OI_DHCI_EXIT_SNIFF_MODE_OPCODE             (0x0804)     /**< H:1.4.6.3 Exit_Sniff_Mode */
#define OI_DHCI_EXIT_SNIFF_MODE_PARM_LEN                (2)     /**< H:1.4.6.3 Exit_Sniff_Mode */

#define OI_DHCI_PARK_MODE_OPCODE                   (0x0805)     /**< H:1.4.6.4 Park_Mode */
#define OI_DHCI_PARK_MODE_PARM_LEN                      (6)     /**< H:1.4.6.4 Park_Mode */

#define OI_DHCI_EXIT_PARK_MODE_OPCODE              (0x0806)     /**< H:1.4.6.5 Exit_Park_Mode */
#define OI_DHCI_EXIT_PARK_MODE_PARM_LEN                 (2)     /**< H:1.4.6.5 Exit_Park_Mode */

#define OI_DHCI_QOS_SETUP_OPCODE                   (0x0807)     /**< H:1.4.6.6 QoS_Setup (quality of service) */
#define OI_DHCI_QOS_SETUP_PARM_LEN                     (20)     /**< H:1.4.6.6 QoS_Setup (quality of service) */

#define OI_DHCI_ROLE_DISCOVERY_OPCODE              (0x0809)     /**< H:1.4.6.7 Role_Discovery */
#define OI_DHCI_ROLE_DISCOVERY_PARM_LEN                 (2)     /**< H:1.4.6.7 Role_Discovery */
#define OI_DHCI_ROLE_DISCOVERY_RETURN_LEN               (4)     /**< H:1.4.6.7 Role_Discovery */

#define OI_DHCI_SWITCH_ROLE_OPCODE                 (0x080b)     /**< H:1.4.6.8 Switch_Role */
#define OI_DHCI_SWITCH_ROLE_PARM_LEN                    (7)     /**< H:1.4.6.8 Switch_Role */

#define OI_DHCI_READ_LINKPOLICY_OPCODE             (0x080c)     /**< H:1.4.6.9 Read_Link_Policy_Settings */
#define OI_DHCI_READ_LINKPOLICY_PARM_LEN                (2)     /**< H:1.4.6.9 Read_Link_Policy_Settings */
#define OI_DHCI_READ_LINKPOLICY_RETURN_LEN              (5)     /**< H:1.4.6.9 Read_Link_Policy_Settings */

#define OI_DHCI_WRITE_LINKPOLICY_OPCODE            (0x080d)     /**< H:1.4.6.9 Write_Link_Policy_Settings */
#define OI_DHCI_WRITE_LINKPOLICY_PARM_LEN               (4)     /**< H:1.4.6.9 Write_Link_Policy_Settings */

/*  
H:1.4.7 Host Controller and Baseband Commands         
*/
#define OI_DHCI_SET_EVT_MASK_OPCODE                (0x0c01)     /**< H:1.4.7.1 Set_Event_Mask */
#define OI_DHCI_SET_EVT_MASK_PARM_LEN                   (8)     /**< H:1.4.7.1 Set_Event_Mask */
#define OI_DHCI_SET_EVT_MASK_RETURN_LEN                 (1)     /**< H:1.4.7.1 Set_Event_Mask */

#define OI_DHCI_RESET_OPCODE                       (0x0c03)     /**< H:1.4.7.2 Reset */
#define OI_DHCI_RESET_PARM_LEN                          (0)     /**< H:1.4.7.2 Reset */
#define OI_DHCI_RESET_RETURN_LEN                        (1)     /**< H:1.4.7.2 Reset */

#define OI_DHCI_SET_EVT_FILTER_OPCODE              (0x0c05)     /**< H:1.4.7.3 Set_Event_Filter */
#define OI_DHCI_SET_EVT_FILTER_INQ_00_PARM_LEN        (2+0)     /**< H:1.4.7.3 Set_Event_Filter */
#define OI_DHCI_SET_EVT_FILTER_INQ_01_PARM_LEN        (2+6)     /**< H:1.4.7.3 Set_Event_Filter */
#define OI_DHCI_SET_EVT_FILTER_INQ_02_PARM_LEN        (2+6)     /**< H:1.4.7.3 Set_Event_Filter */
#define OI_DHCI_SET_EVT_FILTER_CNCT_00_PARM_LEN       (2+1)     /**< H:1.4.7.3 Set_Event_Filter */
#define OI_DHCI_SET_EVT_FILTER_CNCT_01_PARM_LEN       (2+7)     /**< H:1.4.7.3 Set_Event_Filter */
#define OI_DHCI_SET_EVT_FILTER_CNCT_02_PARM_LEN       (2+7)     /**< H:1.4.7.3 Set_Event_Filter */
#define OI_DHCI_SET_EVT_FILTER_MAXIMUM_PARM_LEN       (2+7)     /**< for buffer sizing, H:1.4.7.3 Set_Event_Filter */
#define OI_DHCI_SET_EVT_FILTER_RETURN_LEN               (1)     /**< H:1.4.7.3 Set_Event_Filter */

#define OI_DHCI_FLUSH_OPCODE                       (0x0c08)     /**< H:1.4.7.4 Flush */
#define OI_DHCI_FLUSH_PARM_LEN                          (2)     /**< H:1.4.7.4 Flush */
#define OI_DHCI_FLUSH_RETURN_LEN                        (3)     /**< H:1.4.7.4 Flush */

#define OI_DHCI_READ_PIN_TYPE_OPCODE               (0x0c09)     /**< H:1.4.7.5 Read_PIN_Type */
#define OI_DHCI_READ_PIN_TYPE_PARM_LEN                  (0)     /**< H:1.4.7.5 Read_PIN_Type */
#define OI_DHCI_READ_PIN_TYPE_RETURN_LEN                (2)     /**< H:1.4.7.5 Read_PIN_Type */

#define OI_DHCI_WRITE_PIN_TYPE_OPCODE              (0x0c0a)     /**< H:1.4.7.6 Write_PIN_Type */
#define OI_DHCI_WRITE_PIN_TYPE_PARM_LEN                 (1)     /**< H:1.4.7.6 Write_PIN_Type */
#define OI_DHCI_WRITE_PIN_TYPE_RETURN_LEN               (1)     /**< H:1.4.7.6 Write_PIN_Type */

#define OI_DHCI_CREATE_NEW_UNIT_KEY_OPCODE         (0x0c0b)     /**< H:1.4.7.7 Create_New_Unit_Key */
#define OI_DHCI_CREATE_NEW_UNIT_KEY_PARM_LEN            (0)     /**< H:1.4.7.7 Create_New_Unit_Key */
#define OI_DHCI_CREATE_NEW_UNIT_KEY_RETURN_LEN          (0)     /**< H:1.4.7.7 Create_New_Unit_Key */

#define OI_DHCI_READ_STORED_LINK_KEY_OPCODE        (0x0c0d)     /**< H:1.4.7.8 Read_Stored_Link_Key */
#define OI_DHCI_READ_STORED_LINK_KEY_PARM_LEN           (7)     /**< H:1.4.7.8 Read_Stored_Link_Key */
#define OI_DHCI_READ_STORED_LINK_KEY_RETURN_LEN         (5)     /**< H:1.4.7.8 Read_Stored_Link_Key */

#define OI_DHCI_WRITE_STORED_LINK_KEY_OPCODE       (0x0c11)     /**< H:1.4.7.9 Write_Stored_Link_Key */
#define OI_DHCI_WRITE_STORED_LINK_KEY_PARM_LEN         (23)     /**< H:1.4.7.9 Write_Stored_Link_Key
                                                                  The HCI specification allows for storing 
                                                                  multiple keys with a single command, this
                                                                  functionality is not supported by BLUEmagic 3.0
                                                                  software; this implementation only supports 
                                                                  storing one key at a time, which is reflected in
                                                                  this parameter length. */
#define OI_DHCI_WRITE_STORED_LINK_KEY_RETURN_LEN        (2)     /**< H:1.4.7.9 Write_Stored_Link_Key */

#define OI_DHCI_DELETE_STORED_LINK_KEY_OPCODE      (0x0c12)     /**< H:1.4.7.10 Delete_Stored_Link_Key */
#define OI_DHCI_DELETE_STORED_LINK_KEY_PARM_LEN         (7)     /**< H:1.4.7.10 Delete_Stored_Link_Key */
#define OI_DHCI_DELETE_STORED_LINK_KEY_RETURN_LEN       (3)     /**< H:1.4.7.10 Delete_Stored_Link_Key */

#define OI_DHCI_CHANGE_LOCAL_NAME_OPCODE           (0x0c13)     /**< H:1.4.7.11 Change_Local_Name */
#define OI_DHCI_CHANGE_LOCAL_NAME_PARM_LEN            (248)     /**< H:1.4.7.11 Change_Local_Name */
#define OI_DHCI_CHANGE_LOCAL_NAME_RETURN_LEN            (1)     /**< H:1.4.7.11 Change_Local_Name */

#define OI_DHCI_READ_LOCAL_NAME_OPCODE             (0x0c14)     /**< H:1.4.7.12 Read_Local_Name */
#define OI_DHCI_READ_LOCAL_NAME_PARM_LEN                (0)     /**< H:1.4.7.12 Read_Local_Name */
#define OI_DHCI_READ_LOCAL_NAME_RETURN_LEN            (249)     /**< H:1.4.7.12 Read_Local_Name */

#define OI_DHCI_READ_CNCT_ACCEPT_TO_OPCODE         (0x0c15)     /**< H:1.4.7.13 Read_Connection_Accept_Timeout */
#define OI_DHCI_READ_CNCT_ACCEPT_TO_PARM_LEN            (0)     /**< H:1.4.7.13 Read_Connection_Accept_Timeout */
#define OI_DHCI_READ_CNCT_ACCEPT_TO_RETURN_LEN          (3)     /**< H:1.4.7.13 Read_Connection_Accept_Timeout */

#define OI_DHCI_WRITE_CNCT_ACCEPT_TO_OPCODE        (0x0c16)     /**< H:1.4.7.14 Write_Connection_Accept_Timeout */
#define OI_DHCI_WRITE_CNCT_ACCEPT_TO_PARM_LEN           (2)     /**< H:1.4.7.14 Write_Connection_Accept_Timeout */
#define OI_DHCI_WRITE_CNCT_ACCEPT_TO_RETURN_LEN         (1)     /**< H:1.4.7.14 Write_Connection_Accept_Timeout */

#define OI_DHCI_READ_PAGE_TIMEOUT_OPCODE           (0x0c17)     /**< H:1.4.7.15 Read_Page_Timeout */
#define OI_DHCI_READ_PAGE_TIMEOUT_PARM_LEN              (0)     /**< H:1.4.7.15 Read_Page_Timeout */
#define OI_DHCI_READ_PAGE_TIMEOUT_RETURN_LEN            (3)     /**< H:1.4.7.15 Read_Page_Timeout */

#define OI_DHCI_WRITE_PAGE_TIMEOUT_OPCODE          (0x0c18)     /**< H:1.4.7.16 Write_Page_Timeout */
#define OI_DHCI_WRITE_PAGE_TIMEOUT_PARM_LEN             (2)     /**< H:1.4.7.16 Write_Page_Timeout */
#define OI_DHCI_WRITE_PAGE_TIMEOUT_RETURN_LEN           (1)     /**< H:1.4.7.16 Write_Page_Timeout */

#define OI_DHCI_READ_SCAN_ENABLE_OPCODE            (0x0c19)     /**< H:1.4.7.17 Read_Scan_Enable */
#define OI_DHCI_READ_SCAN_ENABLE_PARM_LEN               (0)     /**< H:1.4.7.17 Read_Scan_Enable */

#define OI_DHCI_WRITE_SCAN_ENABLE_OPCODE           (0x0c1a)     /**< H:1.4.7.18 Write_Scan_Enable */
#define OI_DHCI_WRITE_SCAN_ENABLE_PARM_LEN              (1)     /**< H:1.4.7.18 Write_Scan_Enable */

#define OI_DHCI_READ_PAGESCAN_ACTIVITY_OPCODE      (0x0c1b)     /**< H:1.4.7.19 Read_Page_Scan_Activity */
#define OI_DHCI_READ_PAGESCAN_ACTIVITY_PARM_LEN         (0)     /**< H:1.4.7.19 Read_Page_Scan_Activity */

#define OI_DHCI_WRITE_PAGESCAN_ACT_OPCODE          (0x0c1c)     /**< H:1.4.7.20 Write_Page_Scan_Activity */
#define OI_DHCI_WRITE_PAGESCAN_ACT_PARM_LEN             (4)     /**< H:1.4.7.20 Write_Page_Scan_Activity */

#define OI_DHCI_READ_INQUIRYSCAN_ACT_OPCODE        (0x0c1d)     /**< H:1.4.7.21 Read_Inquiry_Scan_Activity */
#define OI_DHCI_READ_INQUIRYSCAN_ACT_PARM_LEN           (0)     /**< H:1.4.7.21 Read_Inquiry_Scan_Activity */

#define OI_DHCI_WRITE_INQUIRYSCAN_ACT_OPCODE       (0x0c1e)     /**< H:1.4.7.22 Write_Inquiry_Scan_Activity */
#define OI_DHCI_WRITE_INQUIRYSCAN_ACT_PARM_LEN          (4)     /**< H:1.4.7.22 Write_Inquiry_Scan_Activity */

#define OI_DHCI_READ_AUTHENT_ENABLE_OPCODE         (0x0c1f)     /**< H:1.4.7.23 Read_Authentication_Enable */
#define OI_DHCI_READ_AUTHENT_ENABLE_PARM_LEN            (0)     /**< H:1.4.7.23 Read_Authentication_Enable */

#define OI_DHCI_WRITE_AUTHENT_ENABLE_OPCODE        (0x0c20)     /**< H:1.4.7.24 Write_Authentication_Enable */
#define OI_DHCI_WRITE_AUTHENT_ENABLE_PARM_LEN           (1)     /**< H:1.4.7.24 Write_Authentication_Enable */

#define OI_DHCI_READ_ENCRYPTION_MODE_OPCODE        (0x0c21)     /**< H:1.4.7.25 Read_Encryption_Mode */
#define OI_DHCI_READ_ENCRYPTION_MODE_PARM_LEN           (0)     /**< H:1.4.7.25 Read_Encryption_Mode */

#define OI_DHCI_WRITE_ENCRYPTION_MODE_OPCODE       (0x0c22)     /**< H:1.4.7.26 Read_Encryption_Mode */
#define OI_DHCI_WRITE_ENCRYPTION_MODE_PARM_LEN          (1)     /**< H:1.4.7.26 Read_Encryption_Mode */

#define OI_DHCI_READ_CLASS_OF_DEVICE_OPCODE        (0x0c23)     /**< H:1.4.7.27 Read_Class_of_Device */
#define OI_DHCI_READ_CLASS_OF_DEVICE_PARM_LEN           (0)     /**< H:1.4.7.27 Read_Class_of_Device */
#define OI_DHCI_READ_CLASS_OF_DEVICE_RETURN_LEN         (4)     /**< H:1.4.7.27 Read_Class_of_Device */

#define OI_DHCI_WRITE_CLASS_OF_DEVICE_OPCODE       (0x0c24)     /**< H:1.4.7.28 Write_Class_of_Device */
#define OI_DHCI_WRITE_CLASS_OF_DEVICE_PARM_LEN          (3)     /**< H:1.4.7.28 Write_Class_of_Device */
#define OI_DHCI_WRITE_CLASS_OF_DEVICE_RETURN_LEN        (1)     /**< H:1.4.7.28 Write_Class_of_Device */

#define OI_DHCI_READ_VOICE_SETTING_OPCODE          (0x0c25)     /**< H:1.4.7.29 Read_Voice_Setting */
#define OI_DHCI_READ_VOICE_SETTING_PARM_LEN             (0)     /**< H:1.4.7.29 Read_Voice_Setting */

#define OI_DHCI_WRITE_VOICE_SETTING_OPCODE         (0x0c26)     /**< H:1.4.7.30 Write_Voice_Setting */
#define OI_DHCI_WRITE_VOICE_SETTING_PARM_LEN            (2)     /**< H:1.4.7.30 Write_Voice_Setting */

#define OI_DHCI_READ_AUTO_FLUSH_TO_OPCODE          (0x0c27)     /**< H:1.4.7.31 Read_Automatic_Flush_Timeout */
#define OI_DHCI_READ_AUTO_FLUSH_TO_PARM_LEN             (2)     /**< H:1.4.7.31 Read_Automatic_Flush_Timeout */

#define OI_DHCI_WRITE_AUTO_FLUSH_TO_OPCODE         (0x0c28)     /**< H:1.4.7.32 Write_Automatic_Flush_Timeout */
#define OI_DHCI_WRITE_AUTO_FLUSH_TO_PARM_LEN            (4)     /**< H:1.4.7.32 Write_Automatic_Flush_Timeout */

#define OI_DHCI_READ_NUM_BC_RETRANS_OPCODE         (0x0c29)     /**< H:1.4.7.33 Read_Num_Broadcast_Retransmissions */
#define OI_DHCI_READ_NUM_BC_RETRANS_PARM_LEN            (0)     /**< H:1.4.7.33 Read_Num_Broadcast_Retransmissions */

#define OI_DHCI_WRITE_NUM_BC_RETRANS_OPCODE        (0x0c2a)     /**< H:1.4.7.34 Write_Num_Broadcast_Retransmissions */
#define OI_DHCI_WRITE_NUM_BC_RETRANS_PARM_LEN           (1)     /**< H:1.4.7.34 Write_Num_Broadcast_Retransmissions */

#define OI_DHCI_READ_HOLD_MODE_ACT_OPCODE          (0x0c2b)     /**< H:1.4.7.35 Read_Hold_Mode_Activity */
#define OI_DHCI_READ_HOLD_MODE_ACT_PARM_LEN             (0)     /**< H:1.4.7.35 Read_Hold_Mode_Activity */

#define OI_DHCI_WRITE_HOLD_MODE_ACT_OPCODE         (0x0c2c)     /**< H:1.4.7.36 Write_Hold_Mode_Activity */
#define OI_DHCI_WRITE_HOLD_MODE_ACT_PARM_LEN            (1)     /**< H:1.4.7.36 Write_Hold_Mode_Activity */

#define OI_DHCI_READ_TX_POWER_LEVEL_OPCODE         (0x0c2d)     /**< H:1.4.7.37 Read_Transmit_Power_Level */
#define OI_DHCI_READ_TX_POWER_LEVEL_PARM_LEN            (3)     /**< H:1.4.7.37 Read_Transmit_Power_Level */

#define OI_DHCI_READ_SCO_FLOW_CTL_ENA_OPCODE       (0x0c2e)     /**< H:1.4.7.38 Read_SCO_Flow_Control_Enable */
#define OI_DHCI_READ_SCO_FLOW_CTL_ENA_PARM_LEN          (0)     /**< H:1.4.7.38 Read_SCO_Flow_Control_Enable */

#define OI_DHCI_WRITE_SCO_FLOW_CTL_ENA_OPCODE      (0x0c2f)     /**< H:1.4.7.39 Read_SCO_Flow_Control_Enable */
#define OI_DHCI_WRITE_SCO_FLOW_CTL_ENA_PARM_LEN         (1)     /**< H:1.4.7.39 Read_SCO_Flow_Control_Enable */

#define OI_DHCI_SET_CNTR_TO_HOST_FLOW_OPCODE       (0x0c31)     /**< H:1.4.7.40 Set_Host_Controller_To_Host_Flow_Control */
#define OI_DHCI_SET_CNTR_TO_HOST_FLOW_PARM_LEN          (1)     /**< H:1.4.7.40 Set_Host_Controller_To_Host_Flow_Control */

#define OI_DHCI_HOST_BUFFER_SIZE_OPCODE            (0x0c33)     /**< H:1.4.7.41 Host_Buffer_Size */
#define OI_DHCI_HOST_BUFFER_SIZE_PARM_LEN               (7)     /**< H:1.4.7.41 Host_Buffer_Size */
#define OI_DHCI_HOST_BUFFER_SIZE_RETURN_LEN             (1)     /**< H:1.4.7.41 Host_Buffer_Size */

#define OI_DHCI_HOST_NUM_COMPLETED_PKTS_OPCODE     (0x0c35)     /**< H:1.4.7.42 Host_Number_Of_Completed_Packets;
                                                                  parameter length is variable */
/* #define   DHCI_HOST_NUM_COMPLETED_PKTS_LEN(x) */

#define OI_DHCI_READ_LINK_SVISION_TO_OPCODE        (0x0c36)     /**< H:1.4.7.43 Read_Link_Supervision_Timeout */
#define OI_DHCI_READ_LINK_SVISION_TO_PARM_LEN           (2)     /**< H:1.4.7.43 Read_Link_Supervision_Timeout */

#define OI_DHCI_WRITE_LINK_SVISION_TO_OPCODE       (0x0c37)     /**< H:1.4.7.44 Write_Link_Supervision_Timeout */
#define OI_DHCI_WRITE_LINK_SVISION_TO_PARM_LEN          (4)     /**< H:1.4.7.44 Write_Link_Supervision_Timeout */

#define OI_DHCI_READ_NUM_SUPPORTED_IAC_OPCODE      (0x0c38)     /**< H:1.4.7.45 Read_Number_Of_Supported_IAC */
#define OI_DHCI_READ_NUM_SUPPORTED_IAC_PARM_LEN         (0)     /**< H:1.4.7.45 Read_Number_Of_Supported_IAC */

#define OI_DHCI_READ_CURRENT_IAC_LAP_OPCODE        (0x0c39)     /**< H:1.4.7.46 Read_Current_IAC_LAP; 
                                                                  returned parameter length is variable */
#define OI_DHCI_READ_CURRENT_IAC_LAP_PARM_LEN           (0)     /**< H:1.4.7.46 Read_Current_IAC_LAP */

#define OI_DHCI_WRITE_CURRENT_IAC_LAP_OPCODE       (0x0c3a)     /**< H:1.4.7.47 Write_Current_IAC_LAP;
                                                                  parameter length is variable (1+3*Num_Current_IAC)  */

#define OI_DHCI_READ_PAGESCAN_PRD_MODE_OPCODE      (0x0c3b)     /**< H:1.4.7.48 Read_Page_Scan_Period_Mode */
#define OI_DHCI_READ_PAGESCAN_PRD_MODE_PARM_LEN         (0)     /**< H:1.4.7.48 Read_Page_Scan_Period_Mode */

#define OI_DHCI_WRITE_PAGESCN_PRD_MODE_OPCODE      (0x0c3c)     /**< H:1.4.7.49 Write_Page_Scan_Period_Mode */
#define OI_DHCI_WRITE_PAGESCN_PRD_MODE_PARM_LEN         (1)     /**< H:1.4.7.49 Write_Page_Scan_Period_Mode */

#define OI_DHCI_READ_PAGESCAN_MODE_OPCODE          (0x0c3d)     /**< H:1.4.7.50 Read_Page_Scan_Mode */
#define OI_DHCI_READ_PAGESCAN_MODE_PARM_LEN             (0)     /**< H:1.4.7.50 Read_Page_Scan_Mode */

#define OI_DHCI_WRITE_PAGESCAN_MODE_OPCODE         (0x0c3e)     /**< H:1.4.7.51 Write_Page_Scan_Mode */
#define OI_DHCI_WRITE_PAGESCAN_MODE_PARM_LEN            (1)     /**< H:1.4.7.51 Write_Page_Scan_Mode */

/* 
 H:1.4.8 Informational Parameters                    
*/
#define OI_DHCI_READ_LOCAL_VER_INFO_OPCODE         (0x1001)     /**< H:1.4.8.1 Read_Local_Version_Information */
#define OI_DHCI_READ_LOCAL_VER_INFO_PARM_LEN            (0)     /**< H:1.4.8.1 Read_Local_Version_Information */
#define OI_DHCI_READ_LOCAL_VER_INFO_RETURN_LEN          (9)     /**< H:1.4.8.1 Read_Local_Version_Information */

#define OI_DHCI_READ_LOCAL_FEATURES_OPCODE         (0x1003)     /**< H:1.4.8.2 Read_Local_Supported_Features */
#define OI_DHCI_READ_LOCAL_FEATURES_PARM_LEN            (0)     /**< H:1.4.8.2 Read_Local_Supported_Features */
#define OI_DHCI_READ_LOCAL_FEATURES_RETURN_LEN          (9)     /**< H:1.4.8.2 Read_Local_Supported_Features */

#define OI_DHCI_READ_BUFFER_SIZE_OPCODE            (0x1005)     /**< H:1.4.8.3 Read_Buffer_Size */
#define OI_DHCI_READ_BUFFER_SIZE_PARM_LEN               (0)     /**< H:1.4.8.3 Read_Buffer_Size */
#define OI_DHCI_READ_BUFFER_SIZE_RETURN_LEN             (8)     /**< H:1.4.8.3 Read_Buffer_Size */

#define OI_DHCI_READ_COUNTRY_CODE_OPCODE           (0x1007)     /**< H:1.4.8.4 Read_Country_Code */
#define OI_DHCI_READ_COUNTRY_CODE_PARM_LEN              (0)     /**< H:1.4.8.4 Read_Country_Code */

#define OI_DHCI_READ_BD_ADDR_OPCODE                (0x1009)     /**< H:1.4.8.5 Read_BD_ADDR */
#define OI_DHCI_READ_BD_ADDR_PARM_LEN                   (0)     /**< H:1.4.8.5 Read_BD_ADDR */

/*  
H:1.4.9 Status Parameters
*/
#define OI_DHCI_READ_FAILED_CON_CTR_OPCODE         (0x1401)     /**< H:1.4.9.1 Read_Failed_Contact_Counter */
#define OI_DHCI_READ_FAILED_CON_CTR_PARM_LEN            (2)     /**< H:1.4.9.1 Read_Failed_Contact_Counter */

#define OI_DHCI_RESET_FAILED_CON_CTR_OPCODE        (0x1402)     /**< H:1.4.9.2 Reset_Failed_Contact_Counter */
#define OI_DHCI_RESET_FAILED_CON_CTR_PARM_LEN           (2)     /**< H:1.4.9.2 Reset_Failed_Contact_Counter */

#define OI_DHCI_GET_LINK_QUALITY_OPCODE            (0x1403)     /**< H:1.4.9.3 Get_Link_Quality */
#define OI_DHCI_GET_LINK_QUALITY_PARM_LEN               (2)     /**< H:1.4.9.3 Get_Link_Quality */

#define OI_DHCI_READ_RSSI_OPCODE                   (0x1405)     /**< H:1.4.9.4 Read_RSSI */
#define OI_DHCI_READ_RSSI_PARM_LEN                      (2)     /**< H:1.4.9.4 Read_RSSI */

/*  
H:1.4.10  Testing Commands
*/
#define OI_DHCI_READ_LOOPBACK_MODE_OPCODE          (0x1801)     /**< H:1.4.10.1 Read_Loopback_Mode */
#define OI_DHCI_READ_LOOPBACK_MODE_PARM_LEN             (0)     /**< H:1.4.10.1 Read_Loopback_Mode */

#define OI_DHCI_WRITE_LOOPBACK_MODE_OPCODE         (0x1802)     /**< H:1.4.10.2 Write_Loopback_Mode */
#define OI_DHCI_WRITE_LOOPBACK_MODE_PARM_LEN            (1)     /**< H:1.4.10.2 Write_Loopback_Mode */

#define OI_DHCI_ENABLE_DEV_TEST_MODE_OPCODE        (0x1803)     /**< H:1.4.10.3 Enable_Device_Under_Test_Mode */
#define OI_DHCI_ENABLE_DEV_TEST_MODE_PARM_LEN           (0)     /**< H:1.4.10.3 Enable_Device_Under_Test_Mode */

/*  
H:1.4.4  Special Commands
*/
#define OI_DHCI_BLUETOOTH_LOGO_TEST_OPCODE         (0xf800)     /**< H:1.4.4.1 Bluetooth_Logo_Testing */
#define OI_DHCI_BLUETOOTH_LOGO_TEST_PARM_LEN          (255)     /**< H:1.4.4.1 Bluetooth_Logo_Testing */

#define OI_DHCI_VENDOR_OPCODE                      (0xfc00)     /**< H:1.4.4.1 Vendor_Specific_Command */
#define OI_DHCI_VENDOR_PARM_LEN                       (255)     /**< H:1.4.4.1 Vendor_Specific_Command */

/*----------------------------------------------------------*/
/*  End of HCI command definitions                          */
/*----------------------------------------------------------*/

/*----------------------------------------------------------*/
/*  Masks and Macros for OGF and OCF placement              */
/*----------------------------------------------------------*/
#define OI_DHCI_OGF_MASK                           (0xfc00)     /**< H:1.4.4.1 Vendor_Specific_Command */
#define OI_DHCI_OCF_MASK                           (0x03FF)     /**< H:1.4.4.1 Vendor_Specific_Command */

#define OI_HCI_OGF_TO_OPCODE(ogf)        (((ogf) & 0x3f) << 10)     /**< H:1.4.4.1 Vendor_Specific_Command */
#define OI_HCI_OCF_TO_OPCODE(ocf)              ((ocf) & 0x03ff)     /**< H:1.4.4.1 Vendor_Specific_Command */
#define OI_HCI_OPCODE_TO_OGF(opcode)  (((opcode) >> 10) & 0x3f)     /**< H:1.4.4.1 Vendor_Specific_Command */
#define OI_HCI_OPCODE_TO_OCF(opcode)        ((opcode) & 0x03ff)     /**< H:1.4.4.1 Vendor_Specific_Command */

/*----------------------------------------------------------*/
/*  End of Masks and Macros for OGF and OCF placement       */
/*----------------------------------------------------------*/

/*----------------------------------------------------------*/
/*                                                          */
/*  HCI event code definitions                              */
/*                                                          */
/*----------------------------------------------------------*/


#define OI_DHCI_INQUIRY_CMPLT_EVT                  (0x01)     /**< H:1.5.2.1 Inquiry Complete Event */

#define OI_DHCI_INQUIRY_RESULT_EVT                 (0x02)     /**< H:1.5.2.2 Inquiry Result Event */

#define OI_DHCI_CNCT_CMPLT_EVT                     (0x03)     /**< H:1.5.2.3 Connection Complete Event */

#define OI_DHCI_CNCT_REQ_EVT                       (0x04)     /**< H:1.5.2.4 Connection Request Event */

#define OI_DHCI_DISC_CMPLT_EVT                     (0x05)     /**< H:1.5.2.5 Disconnection Complete Event */

#define OI_DHCI_AUTHENT_CMPLT_EVT                  (0x06)     /**< H:1.5.2.6 Authentication Complete Event */

#define OI_DHCI_REMOTE_NAME_REQ_CMPLT_EVT          (0x07)     /**< H:1.5.2.7 Remote Name Request Complete Event */

#define OI_DHCI_ENCRYPTION_CHG_EVT                 (0x08)     /**< H:1.5.2.8 Encryption Change Event */

#define OI_DHCI_CHG_CNCT_LINKKEY_CMPLT_EVT         (0x09)     /**< H:1.5.2.9 Change Connection Link Key Complete Event */

#define OI_DHCI_MASTER_LINK_KEY_CMPLT_EVT          (0x0a)     /**< H:1.5.2.10 Master Link Key Complete Event */

#define OI_DHCI_READ_REMOTE_FEAT_CMPLT_EVT         (0x0b)     /**< H:1.5.2.11 Read Remote Supported Features Complete Event */

#define OI_DHCI_READ_RMT_VERINFO_CMPLT_EVT         (0x0c)     /**< H:1.5.2.12 Read Remote Version Information Complete Event */

#define OI_DHCI_QOS_SETUP_CMPLT_EVT                (0x0d)     /**< H:1.5.2.13 QoS Setup Complete Event */

#define OI_DHCI_CMND_CMPLT_EVT                     (0x0e)     /**< H:1.5.2.14 Command Complete Event */

#define OI_DHCI_CMND_STATUS_EVT                    (0x0f)     /**< H:1.5.2.15 Command Status Event */

#define OI_DHCI_HARDWARE_ERR_EVT                   (0x10)     /**< H:1.5.2.16 Hardware Error Event */

#define OI_DHCI_FLUSH_OCCURRED_EVT                 (0x11)     /**< H:1.5.2.17 Flush Occurred Event */

#define OI_DHCI_ROLE_CHANGE_EVT                    (0x12)     /**< H:1.5.2.18 Role Change Event */

#define OI_DHCI_NUM_COMPLETED_PKTS_EVT             (0x13)     /**< H:1.5.2.19 Number Of Completed Packets Event */

#define OI_DHCI_MODE_CHANGE_EVT                    (0x14)     /**< H:1.5.2.20 Mode Change Event */

#define OI_DHCI_RETURN_LINK_KEYS_EVT               (0x15)     /**< H:1.5.2.21 Return Link Keys Event */

#define OI_DHCI_PIN_CODE_REQ_EVT                   (0x16)     /**< H:1.5.2.22 PIN Code Request Event */

#define OI_DHCI_LINK_KEY_REQ_EVT                   (0x17)     /**< H:1.5.2.23 Link Key Request Event */

#define OI_DHCI_LINK_KEY_NOTIFICATION_EVT          (0x18)     /**< H:1.5.2.24 Link Key Notification Event */

#define OI_DHCI_LOOPBACK_CMND_EVT                  (0x19)     /**< H:1.5.2.25 Loopback Command Event */

#define OI_DHCI_DATA_BUFFER_OVERFLOW_EVT           (0x1a)     /**< H:1.5.2.26 Data Buffer Overflow Event */

#define OI_DHCI_MAX_SLOTS_CHANGE_EVT               (0x1b)     /**< H:1.5.2.27 Max Slots Change EventEvent */

#define OI_DHCI_READ_CLOCK_CMPLT_EVT               (0x1c)     /**< H:1.5.2.28 Read Clock Offset Complete Event */

#define OI_DHCI_CNCT_PKT_TYPE_CHANGED_EVT          (0x1d)     /**< H:1.5.2.29 Connection Packet Type Changed Event */

#define OI_DHCI_QOS_VIOLATION_EVT                  (0x1e)     /**< H:1.5.2.30 QoS Violation Event */

#define OI_DHCI_PAGESCAN_MODE_CHANGE_EVT           (0x1f)     /**< H:1.5.2.31 Page Scan Mode Change Event */

#define OI_DHCI_PAGESCAN_REP_MODE_CHG_EVT          (0x20)     /**< H:1.5.2.32 Page Scan Repetition Mode Change Event */

/*  
H:1.4.4.2  Special events
*/ 
#define OI_DHCI_BT_LOGO_TEST_EVT                   (0xFE)     /**< Bluetooth logo test event */
#define OI_DHCI_VENDOR_SPECIFIC_DEBUG_EVT          (0xFF)     /**< vendor-specific debugging event */

/*----------------------------------------------------------*/
/*  End of HCI Event defines                                */
/*----------------------------------------------------------*/
/*****************************************************************************/
#endif /* _OI_HCISPEC_H */
