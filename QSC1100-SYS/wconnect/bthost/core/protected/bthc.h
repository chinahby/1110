#ifndef _BTHC_H
#define _BTHC_H

#ifdef FEATURE_BT

/*====*====*====*====*====*====*====*====*====*====*====*====*====*====*====*

            B L U E T O O T H    H C I    H E A D E R

DESCRIPTION
  This contains the highest level header for the Bluetooth HCI/LMP/BB layer.

              Copyright (c) 2000-2008 QUALCOMM Incorporated.               
                      All Rights Reserved.                                  
              Qualcomm Confidential and Proprietary                        

*====*====*====*====*====*====*====*====*====*====*====*====*====*====*====*/

/*===========================================================================

                        EDIT HISTORY FOR MODULE

  This section contains comments describing changes made to the module.
  Notice that changes are listed in reverse chronological order.

$PVCSHeader:   O:/src/asw/COMMON/vcs/bthc.h_v   1.12   13 Sep 2001 16:31:28   waynelee
$Header: //source/qcom/qct/wconnect/bthost/core/rel/00.00.26/protected/bthc.h#1 $ $DateTime:

$Log:   O:/src/asw/COMMON/vcs/bthc.h_v  $

  when        who  what, where, why
  ----------  ---  -----------------------------------------------------------
  2008-03-24   vk  Added macros for HCI Read/write Defalut Erroneous Data 
                   Reporting mode commands.

 *
 *   #21        29 Feb 2008            AG
 * Renamed BT_HCI_IO_CAPABILITY_RESPONSE to BT_HCI_IO_CAPABILITY_RQST_REPLY
 * for consistency with spec.
 * Updated default event mask for 2.1.
 * 
 *   #16.4        16 Jan 2008           DSN 
 * Fixing defintions for vendor-specific command groups.
 *
 *   #19        15 Jan 2008           DSN 
 * Add definitions for more vendor-specific command groups. 
 *
 *   #18        10 Jan 2008            SSK
 * Introduced a new BT_HCI_ZERO_PKT type in bt_hci_pkt_type.
 *
 *   #17        01 Nov 2007            AG
 * Merged Lisbon(BT2.1) changes into the mainline
 *
 *   #16        31 Oct 2007            VK
 * Added macros to support Inquiry result event with RSSI.
 *
 *   #15        16 Oct 2007            DSN
 * Added support for Rd Loc Supported commands and Rd local extended 
 * features command.
 *
 *   #14        04 Sept  2007          BH
 *   Added support for QSoC vender specific encryption key length command. 
 *
 *   #13        20 AUG  2007           PG
 *   Seperated HCI generic vendor specific event handling from Qualcomm 
 *   proprietary implementation used with embedded controller.
 *
 *   #12        09 JUL 2007            PG
 * HCI Remote name req cancel and connection cancel support.
 *
 *   #11         19 Apr 2007            PG
 * Featurized out code related to Qualcomm Vendor specific commands for 
 * SOC targets.
 *
 *    #10        02 Mar  2007           JN
 * Added macros for BTS specifc, vendor specific OGF/OCF.
 *
 *    #9         20 Oct 2006            SSK
 * External SOC suppport: Featurize Vendor Specific event\command definitions.
 *
 *    #8         15 Jun 2006            MH
 * Added new define preferred sniff interval.
 *
 *    #7         19 Oct 2005            JH
 * Added support for the HCI read remote extended features command and read
 *  remote extended features complete event.
 * 
 *    #6         27 Jun 2005            JH
 * Added new BT 1.2 commands: Read_Inquiry_Scan_Type, Write_Inquiry_Scan_Mode,
 *  Read_Inquiry_Mode, and Write_Inquiry_Mode
 * Replaced BT 1.1 commands: Read_Page_Scan_Mode and Write_Page_Scan_Mode with
 *  BT 1.2 commands: Read_Page_Scan_Type and Write_Page_Scan_Type.
 * Added BT 1.2 events: Synchronous Connection Complete and 
 *  Synchronous Connection Changed.
 *
 *    #5         06 Apr 2005            JH
 * Adding support for disabling BT radio, featurized using BT_SWDEV_RADIO_OFF.
 *
 *    #4         12 Aug 2004            JH
 * Added TX continuous frequency HCI command
 *
 *    #3         7  Jul 2004            JH
 * Added support for BT 1.2 AFH and eSCO HCI commands
 * 
 *    Rev 1.12   13 Sep 2001 16:31:28   waynelee
 * Added peek/poke functionality via QBTS.
 * 
 *    Rev 1.11   29 Aug 2001 11:45:26   propach
 * Merged certification tip onto main tip.
 * 
 *    Rev 1.10.1.2   14 Aug 2001 19:21:50   waynelee
 * Renamed CMD_HC_FIX_PKT_HDR to CMD_HC_CERT_CTRL.  Removed
 * obsolete qdsp image ctrl cmds.
 * 
 *    Rev 1.10.1.1   30 Jul 2001 17:08:22   waynelee
 * Added new hci cmds: rd_raw_rssi, test_set_lm_params,
 * test_send_lmp_msg.
 * 
 *    Rev 1.10.1.0   29 Jun 2001 15:22:46   waynelee
 * No change.
 * 
 *    Rev 1.10   29 Jun 2001 11:31:32   waynelee
 * Added HCI mechanism to not automatically unload QDSP image.
 * 
 *    Rev 1.9   27 Jun 2001 11:23:44   waynelee
 * Added cmds for telec and bb certification.
 * 
 *    Rev 1.8   08 Jun 2001 16:13:18   waynelee
 * Added protos and extern for real HCI_Reset and 
 * HCI_Set_Tx_Power_Level support.
 * 
 *    Rev 1.7   21 Mar 2001 16:47:28   waynelee
 * Added support for these commands: Set_QDSP_Image,
 * Set_Incoming_Hold_Params, Set_Incoming_Sniff_Params,
 * Set_Incoming_Park_Params.
 * 
 *    Rev 1.6   02 Mar 2001 00:32:08   ryuen
 * Renamed bt_xx_offline to bt_xx_qdsp_reset_shutdown
 * 
 *    Rev 1.5   23 Feb 2001 13:06:10   ryuen
 * Renamed bt_hc_initialize and bt_hc_online to bt_hc_powerup_init and
 * bt_hc_qdsp_ready_init respectively
 * 
 *    Rev 1.4   16 Feb 2001 16:10:54   propach
 * Completed EC routing of events by BT application ID.
 * 
 *    Rev 1.3   07 Feb 2001 16:44:38   waynelee
 * Made HCI 1.1 compliant.
 * 
 *    Rev 1.2   07 Feb 2001 10:17:10   propach
 * Reworked internal BT application IDs and made event control
 * route based on these.
 * 
 *    Rev 1.1   02 Feb 2001 17:24:22   waynelee
 * Added HCI #define
 * 
 *    Rev 1.0   29 Jan 2001 17:06:30   propach
 * Initial revision.
 * 
 *    Rev 1.9   22 Nov 2000 12:17:26   waynelee
 * Added support for enable_loopback HCI command
 * 
 *    Rev 1.8   27 Oct 2000 17:57:48   waynelee
 * Added new #defines for new QCOM HCI cmds
 * 
 *    Rev 1.7   04 Oct 2000 15:10:48   propach
 * RF test mode changes per WAL (corrected).
 * 
 *    Rev 1.5   05 Sep 2000 16:43:50   waynelee
 * Added online, offline support.  Added debug MSGs.  Added Robustness.
 * 
 *    Rev 1.4   31 Aug 2000 10:45:06   propach
 * Featurized APIT code.  Improved APIT watermark proxy handling.
 *
 *    Rev 1.3   03 Aug 2000 08:41:22   waynelee
 * Adding Low Power Mode Support.  Code for UPF-3
 *
 *    Rev 1.2   02 Aug 2000 09:39:12   propach
 * Added QBTS support for BT.  Added to API.
 *
 *    Rev 1.1   06 Jul 2000 13:21:44   waynelee
 * Added V/I features: change to AUX packet types, set new bd addr.
 * Writes to QDSP CRC table for 0 length packets after connection.
 * Got HCI mode working on both standalone and aarvark targets. verified
 * AG/HS connection on AARVARK target.
 *
 *    Rev 1.0   04 Jul 2000 19:19:12   ryuen
 * Initial revision.

===========================================================================*/

/*===========================================================================

                     INCLUDE FILES FOR MODULE

===========================================================================*/

#include "bt.h"
#include "btcmdi.h"

/*===========================================================================

                DEFINITIONS AND DECLARATIONS FOR MODULE

This section contains definitions for constants, macros, types, variables
and other items needed by this module.

===========================================================================*/

/*-------------------------------------------------------------------------*/
/*                                                                         */
/*                         Data Definitions                                */
/*                                                                         */
/*-------------------------------------------------------------------------*/

//
// Version Number
//
#define BT_HCI_VERSION_1_0B              0x00
#define BT_HCI_VERSION_1_1               0x01
#define BT_HCI_VERSION_1_2               0x02
#define BT_HCI_VERSION_2_1               0x03

//
// Sync value
//
#define BT_HCI_SYNC_VALUE                0xFE

//
// HCI packet indicator types
//
typedef enum
{
  BT_HCI_ZERO_PKT        = 0x00,
  BT_HCI_CMD_PKT         = 0x01,
  BT_HCI_ACL_DATA_PKT    = 0x02,
  BT_HCI_SCO_DATA_PKT    = 0x03,
  BT_HCI_EVENT_PKT       = 0x04
#ifdef FEATURE_BT_TEST_API
#error code not present
#endif
} bt_hci_pkt_type;

//
// HCI command groups
//
#define BT_HCI_LC_CMD_GROUP              0x0400
#define BT_HCI_LP_CMD_GROUP              0x0800
#define BT_HCI_HCBB_CMD_GROUP            0x0C00
#define BT_HCI_INFO_PRM_GROUP            0x1000
#define BT_HCI_STAT_PRM_GROUP            0x1400
#define BT_HCI_TEST_CMD_GROUP            0x1800

#define BT_HCI_NONE                      0x0000
#define BT_HCI_VS_GROUP                  0xFC00

#ifndef FEATURE_BT_SOC
#define BT_HCI_QUALCOMM_GROUP           BT_HCI_VS_GROUP
#define BT_EV_HC_QUALCOMM               BT_EV_HC_VENDOR_SPECIFIC
#define BT_HCI_QUALCOMM                 BT_HCI_VENDOR_SPECIFIC
#endif /* FEATURE_BT_SOC */

#ifdef FEATURE_BT_QSOC
/* Groups (OGF/OCF) corresponding to BTS*/
#define BT_HCI_QSOC_VS_GROUP             0xFC00
#define BT_HCI_QSOC_VS_EDL_GROUP         0xFC00
#define BT_HCI_QSOC_VS_TEST_GROUP        (BT_HCI_QSOC_VS_GROUP | 0x0003)
#define BT_HCI_QSOC_VS_TEST_GROUP1       (BT_HCI_QSOC_VS_GROUP | 0x0003)
#define BT_HCI_QSOC_VS_TEST_GROUP2       (BT_HCI_QSOC_VS_GROUP | 0x0004)
#define BT_HCI_QSOC_VS_TEST_GROUP3       (BT_HCI_QSOC_VS_GROUP | 0x000C)
#define BT_HCI_QSOC_VS_TEST_GROUP4       (BT_HCI_QSOC_VS_GROUP | 0x000D)
#define BT_HCI_QSOC_VS_TEST_GROUP5       (BT_HCI_QSOC_VS_GROUP | 0x000F)
#define BT_HCI_QSOC_VS_NVM_GROUP         (BT_HCI_QSOC_VS_GROUP | 0x000B)
#define BT_HCI_QSOC_VS_CMD_GROUP         0xFFFF
#endif /* FEATURE_BT_QSOC */

//
// Link Control Command opcodes
//
#define BT_HCI_INQUIRY                         (BT_HCI_LC_CMD_GROUP | 0x0001)
#define BT_HCI_INQUIRY_CANCEL                  (BT_HCI_LC_CMD_GROUP | 0x0002)
#define BT_HCI_PERIODIC_INQUIRY_MODE           (BT_HCI_LC_CMD_GROUP | 0x0003)
#define BT_HCI_EXIT_PERIODIC_INQUIRY_MODE      (BT_HCI_LC_CMD_GROUP | 0x0004)
#define BT_HCI_CREATE_CONNECTION               (BT_HCI_LC_CMD_GROUP | 0x0005)
#define BT_HCI_DISCONNECT                      (BT_HCI_LC_CMD_GROUP | 0x0006)
#define BT_HCI_ADD_SCO_CONNECTION              (BT_HCI_LC_CMD_GROUP | 0x0007)
#define BT_HCI_CREATE_CONNECTION_CANCEL        (BT_HCI_LC_CMD_GROUP | 0x0008)
#define BT_HCI_ACCEPT_CONNECTION_RQST          (BT_HCI_LC_CMD_GROUP | 0x0009)
#define BT_HCI_REJECT_CONNECTION_RQST          (BT_HCI_LC_CMD_GROUP | 0x000A)
#define BT_HCI_LINK_KEY_RQST_REPLY             (BT_HCI_LC_CMD_GROUP | 0x000B)
#define BT_HCI_LINK_KEY_RQST_NEG_REPLY         (BT_HCI_LC_CMD_GROUP | 0x000C)
#define BT_HCI_PIN_CODE_RQST_REPLY             (BT_HCI_LC_CMD_GROUP | 0x000D)
#define BT_HCI_PIN_CODE_RQST_NEG_REPLY         (BT_HCI_LC_CMD_GROUP | 0x000E)
#define BT_HCI_CHANGE_CONNECTION_PKT_TYPE      (BT_HCI_LC_CMD_GROUP | 0x000F)
#define BT_HCI_AUTHENTICATION_REQUESTED        (BT_HCI_LC_CMD_GROUP | 0x0011)
#define BT_HCI_SET_CONNECTION_ENCRYPTION       (BT_HCI_LC_CMD_GROUP | 0x0013)
#define BT_HCI_CHANGE_CONNECTION_LINK_KEY      (BT_HCI_LC_CMD_GROUP | 0x0015)
#define BT_HCI_MASTER_LINK_KEY                 (BT_HCI_LC_CMD_GROUP | 0x0017)
#define BT_HCI_REMOTE_NAME_RQST                (BT_HCI_LC_CMD_GROUP | 0x0019)
#define BT_HCI_REMOTE_NAME_RQST_CANCEL         (BT_HCI_LC_CMD_GROUP | 0x001A)
#define BT_HCI_RD_REMOTE_SUPPORTED_FEAT        (BT_HCI_LC_CMD_GROUP | 0x001B)
#ifdef FEATURE_BT_1_2
#define BT_HCI_RD_REMOTE_EXTENDED_FEAT         (BT_HCI_LC_CMD_GROUP | 0x001C)
#endif
#define BT_HCI_RD_REMOTE_VERSION_INFO          (BT_HCI_LC_CMD_GROUP | 0x001D)
#define BT_HCI_RD_CLOCK_OFFSET                 (BT_HCI_LC_CMD_GROUP | 0x001F)

#define BT_HCI_SETUP_SYNC_CONNECTION           (BT_HCI_LC_CMD_GROUP | 0x0028)
#define BT_HCI_ACCEPT_SYNC_CONN_REQ            (BT_HCI_LC_CMD_GROUP | 0x0029)
#define BT_HCI_REJECT_SYNC_CONN_REQ            (BT_HCI_LC_CMD_GROUP | 0x002A)
#ifdef BT_SWDEV_2_1_SSP
#define BT_HCI_IO_CAPABILITY_RQST_REPLY        (BT_HCI_LC_CMD_GROUP | 0x002B)
#define BT_HCI_USER_CFMTN_RQST_REPLY           (BT_HCI_LC_CMD_GROUP | 0x002C)
#define BT_HCI_USER_CFMTN_RQST_NEG_REPLY       (BT_HCI_LC_CMD_GROUP | 0x002D)
#define BT_HCI_USER_PASSKEY_RQST_REPLY         (BT_HCI_LC_CMD_GROUP | 0x002E)
#define BT_HCI_USER_PASSKEY_RQST_NEG_REPLY     (BT_HCI_LC_CMD_GROUP | 0x002F)
#define BT_HCI_REMOTE_OOB_DATA_RQST_REPLY      (BT_HCI_LC_CMD_GROUP | 0x0030)
#define BT_HCI_REMOTE_OOB_DATA_RQST_NEG_REPLY  (BT_HCI_LC_CMD_GROUP | 0x0033)
#define BT_HCI_IO_CAPABILITY_RQST_NEG_REPLY    (BT_HCI_LC_CMD_GROUP | 0x0034)
#endif /* BT_SWDEV_2_1_SSP */

//
// Link Policy Commands
//
#define BT_HCI_HOLD_MODE                       (BT_HCI_LP_CMD_GROUP | 0x0001)
#define BT_HCI_SNIFF_MODE                      (BT_HCI_LP_CMD_GROUP | 0x0003)
#define BT_HCI_EXIT_SNIFF_MODE                 (BT_HCI_LP_CMD_GROUP | 0x0004)
#define BT_HCI_PARK_MODE                       (BT_HCI_LP_CMD_GROUP | 0x0005)
#define BT_HCI_EXIT_PARK_MODE                  (BT_HCI_LP_CMD_GROUP | 0x0006)
#define BT_HCI_QOS_SETUP                       (BT_HCI_LP_CMD_GROUP | 0x0007)
#define BT_HCI_ROLE_DISCOVERY                  (BT_HCI_LP_CMD_GROUP | 0x0009)
#define BT_HCI_SWITCH_ROLE                     (BT_HCI_LP_CMD_GROUP | 0x000B)
#define BT_HCI_RD_LINK_POLICY_SETTINGS         (BT_HCI_LP_CMD_GROUP | 0x000C)
#define BT_HCI_WR_LINK_POLICY_SETTINGS         (BT_HCI_LP_CMD_GROUP | 0x000D)
#ifdef BT_SWDEV_2_1_SSR
#define BT_HCI_SNIFF_SUBRATING                 (BT_HCI_LP_CMD_GROUP | 0x0011)
#endif /* BT_SWDEV_2_1_SSR */

//
// Host Control & Baseband Commands
//
#define BT_HCI_SET_EVENT_MASK                  (BT_HCI_HCBB_CMD_GROUP | 0x0001)
#define BT_HCI_RESET                           (BT_HCI_HCBB_CMD_GROUP | 0x0003)
#define BT_HCI_SET_EVENT_FILTER                (BT_HCI_HCBB_CMD_GROUP | 0x0005)
#define BT_HCI_FLUSH                           (BT_HCI_HCBB_CMD_GROUP | 0x0008)
#define BT_HCI_RD_PIN_TYPE                     (BT_HCI_HCBB_CMD_GROUP | 0x0009)
#define BT_HCI_WR_PIN_TYPE                     (BT_HCI_HCBB_CMD_GROUP | 0x000A)
#define BT_HCI_CREATE_NEW_UNIT_KEY             (BT_HCI_HCBB_CMD_GROUP | 0x000B)
#define BT_HCI_RD_STORED_LINK_KEY              (BT_HCI_HCBB_CMD_GROUP | 0x000D)
#define BT_HCI_WR_STORED_LINK_KEY              (BT_HCI_HCBB_CMD_GROUP | 0x0011)
#define BT_HCI_DELETE_STORED_LINK_KEY          (BT_HCI_HCBB_CMD_GROUP | 0x0012)
#define BT_HCI_CHANGE_LOCAL_NAME               (BT_HCI_HCBB_CMD_GROUP | 0x0013)
#define BT_HCI_RD_LOCAL_NAME                   (BT_HCI_HCBB_CMD_GROUP | 0x0014)
#define BT_HCI_RD_CONN_ACCEPT_TIMEOUT          (BT_HCI_HCBB_CMD_GROUP | 0x0015)
#define BT_HCI_WR_CONN_ACCEPT_TIMEOUT          (BT_HCI_HCBB_CMD_GROUP | 0x0016)
#define BT_HCI_RD_PAGE_TIMEOUT                 (BT_HCI_HCBB_CMD_GROUP | 0x0017)
#define BT_HCI_WR_PAGE_TIMEOUT                 (BT_HCI_HCBB_CMD_GROUP | 0x0018)
#define BT_HCI_RD_SCAN_ENABLE                  (BT_HCI_HCBB_CMD_GROUP | 0x0019)
#define BT_HCI_WR_SCAN_ENABLE                  (BT_HCI_HCBB_CMD_GROUP | 0x001A)
#define BT_HCI_RD_PAGESCAN_ACTIVITY            (BT_HCI_HCBB_CMD_GROUP | 0x001B)
#define BT_HCI_WR_PAGESCAN_ACTIVITY            (BT_HCI_HCBB_CMD_GROUP | 0x001C)
#define BT_HCI_RD_INQUIRYSCAN_ACTIVITY         (BT_HCI_HCBB_CMD_GROUP | 0x001D)
#define BT_HCI_WR_INQUIRYSCAN_ACTIVITY         (BT_HCI_HCBB_CMD_GROUP | 0x001E)
#define BT_HCI_RD_AUTHENTICATION_ENABLE        (BT_HCI_HCBB_CMD_GROUP | 0x001F)
#define BT_HCI_WR_AUTHENTICATION_ENABLE        (BT_HCI_HCBB_CMD_GROUP | 0x0020)
#define BT_HCI_RD_ENCRYPTION_MODE              (BT_HCI_HCBB_CMD_GROUP | 0x0021)
#define BT_HCI_WR_ENCRYPTION_MODE              (BT_HCI_HCBB_CMD_GROUP | 0x0022)
#define BT_HCI_RD_CLASS_OF_DEVICE              (BT_HCI_HCBB_CMD_GROUP | 0x0023)
#define BT_HCI_WR_CLASS_OF_DEVICE              (BT_HCI_HCBB_CMD_GROUP | 0x0024)
#define BT_HCI_RD_VOICE_SETTING                (BT_HCI_HCBB_CMD_GROUP | 0x0025)
#define BT_HCI_WR_VOICE_SETTING                (BT_HCI_HCBB_CMD_GROUP | 0x0026)
#define BT_HCI_RD_AUTO_FLUSH_TIMEOUT           (BT_HCI_HCBB_CMD_GROUP | 0x0027)
#define BT_HCI_WR_AUTO_FLUSH_TIMEOUT           (BT_HCI_HCBB_CMD_GROUP | 0x0028)
#define BT_HCI_RD_NUM_BC_RETRANSMISSIONS       (BT_HCI_HCBB_CMD_GROUP | 0x0029)
#define BT_HCI_WR_NUM_BC_RETRANSMISSIONS       (BT_HCI_HCBB_CMD_GROUP | 0x002A)
#define BT_HCI_RD_HOLD_MODE_ACTIVITY           (BT_HCI_HCBB_CMD_GROUP | 0x002B)
#define BT_HCI_WR_HOLD_MODE_ACTIVITY           (BT_HCI_HCBB_CMD_GROUP | 0x002C)
#define BT_HCI_RD_TRANSMIT_POWER_LEVEL         (BT_HCI_HCBB_CMD_GROUP | 0x002D)
#define BT_HCI_RD_SCO_FLOW_CONTROL_ENABLE      (BT_HCI_HCBB_CMD_GROUP | 0x002E)
#define BT_HCI_WR_SCO_FLOW_CONTROL_ENABLE      (BT_HCI_HCBB_CMD_GROUP | 0x002F)
#define BT_HCI_SET_HC_TO_HOST_FLOW_CONTROL     (BT_HCI_HCBB_CMD_GROUP | 0x0031)
#define BT_HCI_HOST_BUFFER_SIZE                (BT_HCI_HCBB_CMD_GROUP | 0x0033)
#define BT_HCI_HOST_NUM_COMPLETED_PACKETS      (BT_HCI_HCBB_CMD_GROUP | 0x0035)
#define BT_HCI_RD_LINK_SUPERVISION_TIMEOUT     (BT_HCI_HCBB_CMD_GROUP | 0x0036)
#define BT_HCI_WR_LINK_SUPERVISION_TIMEOUT     (BT_HCI_HCBB_CMD_GROUP | 0x0037)
#define BT_HCI_RD_NUMBER_OF_SUPPORTED_IAC      (BT_HCI_HCBB_CMD_GROUP | 0x0038)
#define BT_HCI_RD_CURRENT_IAC_LAP              (BT_HCI_HCBB_CMD_GROUP | 0x0039)
#define BT_HCI_WR_CURRENT_IAC_LAP              (BT_HCI_HCBB_CMD_GROUP | 0x003A)
#define BT_HCI_RD_PAGE_SCAN_PERIOD_MODE        (BT_HCI_HCBB_CMD_GROUP | 0x003B)
#define BT_HCI_WR_PAGE_SCAN_PERIOD_MODE        (BT_HCI_HCBB_CMD_GROUP | 0x003C)
#ifndef FEATURE_BT_1_2 /* BT 1.1 */
#define BT_HCI_RD_PAGE_SCAN_MODE               (BT_HCI_HCBB_CMD_GROUP | 0x003D)
#define BT_HCI_WR_PAGE_SCAN_MODE               (BT_HCI_HCBB_CMD_GROUP | 0x003E)
#else /* BT 1.2 */
#define BT_HCI_SET_AFH_HOST_CHAN_CLASS         (BT_HCI_HCBB_CMD_GROUP | 0x003F)
#define BT_HCI_RD_INQUIRY_SCAN_TYPE            (BT_HCI_HCBB_CMD_GROUP | 0x0042)
#define BT_HCI_WR_INQUIRY_SCAN_TYPE            (BT_HCI_HCBB_CMD_GROUP | 0x0043)
#define BT_HCI_RD_INQUIRY_MODE                 (BT_HCI_HCBB_CMD_GROUP | 0x0044)
#define BT_HCI_WR_INQUIRY_MODE                 (BT_HCI_HCBB_CMD_GROUP | 0x0045)
#define BT_HCI_RD_PAGE_SCAN_TYPE               (BT_HCI_HCBB_CMD_GROUP | 0x0046)
#define BT_HCI_WR_PAGE_SCAN_TYPE               (BT_HCI_HCBB_CMD_GROUP | 0x0047)
#define BT_HCI_READ_AFH_CHAN_ASSESS_MODE       (BT_HCI_HCBB_CMD_GROUP | 0x0048)
#define BT_HCI_SET_AFH_CHAN_ASSESS_MODE        (BT_HCI_HCBB_CMD_GROUP | 0x0049)
#endif /* FEATURE_BT_1_2 */
#ifdef BT_SWDEV_2_1_PB
#define BT_HCI_ENH_FLUSH                       (BT_HCI_HCBB_CMD_GROUP | 0x005F)
#endif /* BT_SWDEV_2_1_PB */
#ifdef BT_SWDEV_2_1_EIR 
#define BT_HCI_RD_EXT_INQ_RSP                 (BT_HCI_HCBB_CMD_GROUP | 0x0051)
#define BT_HCI_WR_EXT_INQ_RSP                 (BT_HCI_HCBB_CMD_GROUP | 0x0052)
#define BT_HCI_RD_INQ_RSP_TX_PWR              (BT_HCI_HCBB_CMD_GROUP | 0x0058)
#define BT_HCI_WR_INQ_TX_PWR                  (BT_HCI_HCBB_CMD_GROUP | 0x0059)
#endif /* BT_SWDEV_2_1_EIR */

#ifdef BT_SWDEV_2_1_SSP
#define BT_HCI_REFRESH_ENCRYPTION_KEY          (BT_HCI_HCBB_CMD_GROUP | 0x0053)

#define BT_HCI_RD_SIMPLE_PAIRING_MODE          (BT_HCI_HCBB_CMD_GROUP | 0x0055)
#define BT_HCI_WR_SIMPLE_PAIRING_MODE          (BT_HCI_HCBB_CMD_GROUP | 0x0056)
#define BT_HCI_RD_LOCAL_OOB_DATA               (BT_HCI_HCBB_CMD_GROUP | 0x0057)
#endif /* BT_SWDEV_2_1_SSP */

#ifdef FEATURE_BT_2_1
#define BT_HCI_RD_DEFAULT_ERR_DATA_REPORTING   (BT_HCI_HCBB_CMD_GROUP | 0x005A)
#define BT_HCI_WR_DEFAULT_ERR_DATA_REPORTING   (BT_HCI_HCBB_CMD_GROUP | 0x005B)
#endif /* FEATURE_BT_2_1 */

#ifdef BT_SWDEV_2_1_SSP
#define BT_HCI_SEND_KEYPRESS_NOTIFICATION      (BT_HCI_HCBB_CMD_GROUP | 0x0060)
#endif /* BT_SWDEV_2_1_SSP */


//
// Information Parameters
//
#define BT_HCI_RD_LOCAL_VERSION_INFO           (BT_HCI_INFO_PRM_GROUP | 0x0001)
#define BT_HCI_RD_LOCAL_SUPPORTED_CMDS         (BT_HCI_INFO_PRM_GROUP | 0x0002)
#define BT_HCI_RD_LOCAL_SUPPORTED_FEAT         (BT_HCI_INFO_PRM_GROUP | 0x0003)
#define BT_HCI_RD_LOCAL_EXTENDED_FEATURES      (BT_HCI_INFO_PRM_GROUP | 0x0004)
#define BT_HCI_RD_BUFFER_SIZE                  (BT_HCI_INFO_PRM_GROUP | 0x0005)
#define BT_HCI_RD_COUNTRY_CODE                 (BT_HCI_INFO_PRM_GROUP | 0x0007)
#define BT_HCI_RD_BD_ADDR                      (BT_HCI_INFO_PRM_GROUP | 0x0009)

//
// Status Parameters
//
#define BT_HCI_RD_FAILED_CONTACT_COUNTER       (BT_HCI_STAT_PRM_GROUP | 0x0001)
#define BT_HCI_RESET_FAILED_CONTACT_COUNTER    (BT_HCI_STAT_PRM_GROUP | 0x0002)
#define BT_HCI_GET_LINK_QUALITY                (BT_HCI_STAT_PRM_GROUP | 0x0003)
#define BT_HCI_RD_RSSI                         (BT_HCI_STAT_PRM_GROUP | 0x0005)
#define BT_HCI_READ_AFH_CHAN_MAP               (BT_HCI_STAT_PRM_GROUP | 0x0006)

//
// Test Commands
//
#define BT_HCI_RD_LOOPBACK_MODE                (BT_HCI_TEST_CMD_GROUP | 0x0001)
#define BT_HCI_WR_LOOPBACK_MODE                (BT_HCI_TEST_CMD_GROUP | 0x0002)
#define BT_HCI_ENABLE_DUT_MODE                 (BT_HCI_TEST_CMD_GROUP | 0x0003)
#ifdef BT_SWDEV_2_1_SSP
#define BT_HCI_WR_SIMPLE_PAIRING_DEBUG_MODE    (BT_HCI_TEST_CMD_GROUP | 0x0004)
#endif /* BT_SWDEV_2_1_SSP */

#ifndef FEATURE_BT_SOC
//
// Qualcomm vendor specific Commands
//
#define BT_HCI_FIX_INIT_WHITE                  (BT_HCI_QUALCOMM_GROUP | 0x0001)
#define BT_HCI_FIX_INIT_ENC                    (BT_HCI_QUALCOMM_GROUP | 0x0002)
#define BT_HCI_READ_RX_STATUS_ENABLE           (BT_HCI_QUALCOMM_GROUP | 0x0003)
#define BT_HCI_CONFIG_SEARCHER                 (BT_HCI_QUALCOMM_GROUP | 0x0004)
#define BT_HCI_READ_TX_BUFFER_ENABLE           (BT_HCI_QUALCOMM_GROUP | 0x0005)
#define BT_HCI_READ_RX_BUFFER_ENABLE           (BT_HCI_QUALCOMM_GROUP | 0x0006)
#define BT_HCI_SEND_RAW_MODE_PACKET            (BT_HCI_QUALCOMM_GROUP | 0x0007)
#ifdef FEATURE_BT_TEST_MODE_TESTER
  #define BT_HCI_TESTER_ACTIVATE               (BT_HCI_QUALCOMM_GROUP | 0x0008)
  #define BT_HCI_TESTER_CONTROL                (BT_HCI_QUALCOMM_GROUP | 0x0009)
  #define BT_HCI_TESTER_START                  (BT_HCI_QUALCOMM_GROUP | 0x000A)
#endif /* FEATURE_BT_TEST_MODE_TESTER */
#define BT_HCI_SET_NEW_BD_ADDR                 (BT_HCI_QUALCOMM_GROUP | 0x000B)
#define BT_HCI_LMP_TIME_ACCURACY_REQ           (BT_HCI_QUALCOMM_GROUP | 0x000C)
#define BT_HCI_MODIFY_BEACON                   (BT_HCI_QUALCOMM_GROUP | 0x000D)
#define BT_HCI_RD_CLK                          (BT_HCI_QUALCOMM_GROUP | 0x000E)
#define BT_HCI_RD_SBI_REG                      (BT_HCI_QUALCOMM_GROUP | 0x000F)
#define BT_HCI_WR_SBI_REG                      (BT_HCI_QUALCOMM_GROUP | 0x0010)
#define BT_HCI_DIAC_LAP_EN                     (BT_HCI_QUALCOMM_GROUP | 0x0011)
#define BT_HCI_RD_ENC_KEY_LENGTH               (BT_HCI_QUALCOMM_GROUP | 0x0012)
#define BT_HCI_WR_ENC_KEY_LENGTH               (BT_HCI_QUALCOMM_GROUP | 0x0013)
#define BT_HCI_CHANGE_REMOTE_PKT_TYPE          (BT_HCI_QUALCOMM_GROUP | 0x0014)
#define BT_HCI_STATUS_VIEW_EN                  (BT_HCI_QUALCOMM_GROUP | 0x0015)
#define BT_HCI_ENABLE_LOOPBACK                 (BT_HCI_QUALCOMM_GROUP | 0x0016)
#define BT_HCI_SET_INCOMING_HOLD_PARAMS        (BT_HCI_QUALCOMM_GROUP | 0x0018)
#define BT_HCI_SET_INCOMING_SNIFF_PARAMS       (BT_HCI_QUALCOMM_GROUP | 0x0019)
#define BT_HCI_SET_INCOMING_PARK_PARAMS        (BT_HCI_QUALCOMM_GROUP | 0x001A)
#define BT_HCI_SET_TX_POWER_LEVEL              (BT_HCI_QUALCOMM_GROUP | 0x001B)
#define BT_HCI_CERT_CTRL                       (BT_HCI_QUALCOMM_GROUP | 0x001C)
#define BT_HCI_TX_CONTINUOUS                   (BT_HCI_QUALCOMM_GROUP | 0x001D)
#define BT_HCI_TX_CONTINUOUS_FREQ              (BT_HCI_QUALCOMM_GROUP | 0x001E)

#define BT_HCI_RD_RAW_RSSI                     (BT_HCI_QUALCOMM_GROUP | 0x0020)
#define BT_HCI_TEST_SET_LM_PARAMS              (BT_HCI_QUALCOMM_GROUP | 0x0021)
#define BT_HCI_TEST_SEND_LMP_MSG               (BT_HCI_QUALCOMM_GROUP | 0x0022)
#define BT_HCI_WR_RAM                          (BT_HCI_QUALCOMM_GROUP | 0x0023)
#define BT_HCI_RD_RAM                          (BT_HCI_QUALCOMM_GROUP | 0x0024)
#define BT_HCI_CONTROL_AFH                     (BT_HCI_QUALCOMM_GROUP | 0x0025)
#define BT_HCI_SET_AFH_CHAN_GOOD               (BT_HCI_QUALCOMM_GROUP | 0x0026)
#define BT_HCI_SET_ESCO_PKT_LEN                (BT_HCI_QUALCOMM_GROUP | 0x0027)
#endif /* !FEATURE_BT_SOC */


//
// Event types
//
typedef enum
{
  BT_HCI_INQUIRY_COMPLETE_EV                     =  0x01,
  BT_HCI_INQUIRY_RESULT_EV                       =  0x02,
  BT_HCI_CONNECTION_COMPLETE_EV                  =  0x03,
  BT_HCI_CONNECTION_REQUEST_EV                   =  0x04,
  BT_HCI_DISCONNECTION_COMPLETE_EV               =  0x05,
  BT_HCI_AUTHENTICATION_COMPLETE_EV              =  0x06,
  BT_HCI_REMOTE_NAME_RQST_COMPLETE_EV            =  0x07,
  BT_HCI_ENCRYPTION_CHANGE_EV                    =  0x08,
  BT_HCI_CHANGE_CONNECTION_LINK_KEY_COMPLETE_EV  =  0x09,
  BT_HCI_MASTER_LINK_KEY_COMPLETE_EV             =  0x0A,
  BT_HCI_READ_REMOTE_SUPPORTED_FEAT_COMPLETE_EV  =  0x0B,
  BT_HCI_READ_REMOTE_VERSION_INFO_COMPLETE_EV    =  0x0C,
  BT_HCI_QOS_SETUP_COMPLETE_EV                   =  0x0D,
  BT_HCI_COMMAND_COMPLETE_EV                     =  0x0E,
  BT_HCI_COMMAND_STATUS_EV                       =  0x0F,
  BT_HCI_HW_ERROR_EV                             =  0x10,
  BT_HCI_FLUSH_OCCURRED_EV                       =  0x11,
  BT_HCI_ROLE_CHANGE_EV                          =  0x12,
  BT_HCI_NUM_COMPLETED_PACKETS_EV                =  0x13,
  BT_HCI_MODE_CHANGE_EV                          =  0x14,
  BT_HCI_RETURN_LINK_KEYS_EV                     =  0x15,
  BT_HCI_PIN_CODE_REQUEST_EV                     =  0x16,
  BT_HCI_LINK_KEY_REQUEST_EV                     =  0x17,
  BT_HCI_LINK_KEY_NOTIFICATION_EV                =  0x18,
  BT_HCI_LOOPBACK_COMMAND_EV                     =  0x19,
  BT_HCI_DATA_BUFFER_OVERFLOW_EV                 =  0x1A,
  BT_HCI_MAX_SLOTS_CHANGE_EV                     =  0x1B,
  BT_HCI_READ_CLOCK_OFFSET_COMPLETE_EV           =  0x1C,
  BT_HCI_CONNECTION_PKT_TYPE_CHANGED_EV          =  0x1D,
  BT_HCI_QOS_VIOLATION_EV                        =  0x1E,
  BT_HCI_PAGE_SCAN_MODE_CHANGE_EV                =  0x1F,
  BT_HCI_PAGE_SCAN_REP_MODE_CHANGE_EV            =  0x20,
#ifdef FEATURE_BT_1_2
/* This 1 event is not supported yet                            
  BT_HCI_FLOW_SPEC_COMPLETE_EV                   =  0x21,
*/  
  
  BT_HCI_INQUIRY_RESULT_WITH_RSSI_EV             =  0x22,
  BT_HCI_READ_REMOTE_EXT_FEAT_COMPLETE_EV        =  0x23,
  BT_HCI_SYNC_CONNNECTION_COMPLETE_EV            =  0x2C,
  BT_HCI_SYNC_CONNECTION_CHANGE_EV               =  0x2D,
#endif /* FEATURE_BT_1_2 */
#ifdef BT_SWDEV_2_1_SSR
  BT_HCI_SNIFF_SUBRATING_EV                      =  0x2E,
#endif /* BT_SWDEV_2_1_SSR */
#ifdef BT_SWDEV_2_1_EIR 
  BT_HCI_INQUIRY_RESULT_EXTENDED_EV              =  0x2F,
#endif /* BT_SWDEV_2_1_EIR */
#ifdef BT_SWDEV_2_1_SSP
  BT_HCI_ENCRYPTION_KEY_REFRESH_COMPLETE_EV      =  0x30,
  BT_HCI_IO_CAPABILITY_REQUEST_EV                =  0x31,
  BT_HCI_IO_CAPABILITY_RESPONSE_EV               =  0x32,
  BT_HCI_USER_CONFIRMATION_REQUEST_EV            =  0x33,
  BT_HCI_USER_PASSKEY_REQUEST_EV                 =  0x34,
  BT_HCI_REMOTE_OOB_DATA_REQUEST_EV              =  0x35,
  BT_HCI_SIMPLE_PAIRING_COMPLETE_EV              =  0x36,
  BT_HCI_LINK_SUPERVISION_TIMEOUT_CHANGED_EV     =  0x38,
#ifdef BT_SWDEV_2_1_PB
  BT_HCI_ENH_FLUSH_COMPLETE_EV                   =  0x39,
#endif /* BT_SWDEV_2_1_PB */
  BT_HCI_USER_PASSKEY_NOTIFICATION_EV            =  0x3B,
  BT_HCI_KEYPRESS_NOTIFICATION_EV                =  0x3C,
  BT_HCI_REMOTE_HOST_SUPPORTED_FEAT_NTFCTN_EV    =  0x3D,
#endif /* BT_SWDEV_2_1_SSP */

  BT_HCI_VENDOR_SPECIFIC                         =  0xFF,
  BT_HCI_MAX_EV
} bt_hci_ev_type;

//
// Event bit mask
// 
#define BT_HCI_INQUIRY_CMPLT_EV_BIT                    0x0000000000000001
#define BT_HCI_INQUIRY_RESULT_EV_BIT                   0x0000000000000002
#define BT_HCI_CONNECTION_CMPLT_EV_BIT                 0x0000000000000004
#define BT_HCI_CONNECTION_RQST_EV_BIT                  0x0000000000000008
#define BT_HCI_DISCONNECTION_CMPLT_EV_BIT              0x0000000000000010
#define BT_HCI_AUTH_COMPLT_EV_BIT                      0x0000000000000020
#define BT_HCI_REMOTE_NAME_RQST_CMPLT_EV_BIT           0x0000000000000040
#define BT_HCI_ENCRYPTION_CHG_EV_BIT                   0x0000000000000080
#define BT_HCI_CHG_CONN_LINK_KEY_CMPLT_EV_BIT          0x0000000000000100
#define BT_HCI_MASTER_LINK_KEY_CMPLT_EV_BIT            0x0000000000000200
#define BT_HCI_RD_REMOTE_SUPPORTED_FEAT_CMPLT_EV_BIT   0x0000000000000400
#define BT_HCI_RD_REMOTE_VERSION_INFO_CMPLT_EV_BIT     0x0000000000000800
#define BT_HCI_QOS_SETUP_CMPLT_EV_BIT                  0x0000000000001000
#define BT_HCI_CMD_CMPLT_EV_BIT                        0x0000000000002000
#define BT_HCI_CMD_STATUS_EV_BIT                       0x0000000000004000
#define BT_HCI_HW_ERR_EV_BIT                           0x0000000000008000
#define BT_HCI_FLUSH_OCCURRED_EV_BIT                   0x0000000000010000
#define BT_HCI_ROLE_CHG_EV_BIT                         0x0000000000020000
#define BT_HCI_NUM_CMPLTED_PKTS_BIT                    0x0000000000040000
#define BT_HCI_MODE_CHG_EV_BIT                         0x0000000000080000
#define BT_HCI_RETURN_LINK_KEYS_EV_BIT                 0x0000000000100000
#define BT_HCI_PIN_CODE_RQST_EV_BIT                    0x0000000000200000
#define BT_HCI_LINK_KEY_RQST_EV_BIT                    0x0000000000400000
#define BT_HCI_LINK_KEY_NOTIFICATION_EV_BIT            0x0000000000800000
#define BT_HCI_LOOPBACK_CMD_EV_BIT                     0x0000000001000000
#define BT_HCI_DATA_BUF_OVERFLOW_EV_BIT                0x0000000002000000
#define BT_HCI_MAX_SLOTS_CHG_EV_BIT                    0x0000000004000000
#define BT_HCI_RD_CLOCK_OFFSET_CMPLT_EV_BIT            0x0000000008000000
#define BT_HCI_CONN_PKT_TYPE_CHG_EV_BIT                0x0000000010000000
#define BT_HCI_QOS_VIOLATION_EV_BIT                    0x0000000020000000
#define BT_HCI_PG_SCAN_MODE_CHG_EV_BIT                 0x0000000040000000
#define BT_HCI_PG_SCAN_REP_MODE_CHG_EV_BIT             0x0000000080000000
#ifndef FEATURE_BT_1_2 /* BT 1.1 */
#define BT_HCI_DEFAULT_EVENT_MASK                      0x00000000FFFFFFFF
#else /* BT 1.2 */
/* These 3 events are not supported yet
#define BT_HCI_FLOW_SPEC_CMPLT_EV_BIT                  0x0000000100000000ll
#define BT_HCI_INQUIRY_RESULT_WITH_RSSI_EV_BIT         0x0000000200000000ll
*/
/* missing values are reserved */
#define BT_HCI_INQUIRY_RESULT_WITH_RSSI_EV_BIT         0x0000000200000000ll
#define BT_HCI_RD_REMOTE_EXTENDED_FEAT_CMPLT_EV_BIT    0x0000000400000000ll
#define BT_HCI_SYNC_CONN_CMPLT_EV_BIT                  0x0000080000000000ll
#define BT_HCI_SYNC_CONN_CHANGE_EV_BIT                 0x0000100000000000ll
#ifdef FEATURE_BT_2_1
#define BT_HCI_DEFAULT_EVENT_MASK                      0x1FFFFFFFFFFFFFFF
#else
#define BT_HCI_DEFAULT_EVENT_MASK                      0x00001800FFFFFFFFll
#endif
#endif /* FEATURE_BT_1_2 */
#ifdef BT_SWDEV_2_1_SSR
#define BT_HCI_SNIFF_SUBRATING_EV_BIT                  0x0000200000000000
#endif /* BT_SWDEV_2_1_SSR */
#ifdef BT_SWDEV_2_1_SSP
#define BT_HCI_ENCRYPTION_KEY_REFRESH_COMPLETE_EV_BIT  0x0000800000000000
#define BT_HCI_IO_CAPABILITY_REQUEST_EV_BIT            0x0001000000000000
#define BT_HCI_IO_CAPABILITY_RESPONSE_EV_BIT           0x0002000000000000
#define BT_HCI_USER_CONFIRMATION_REQUEST_BIT           0x0004000000000000
#define BT_HCI_USER_PASSKEY_REQUEST_EV_BIT             0x0008000000000000
#define BT_HCI_REMOTE_OOB_DATA_REQUEST_BIT             0x0010000000000000
#define BT_HCI_SIMPLE_PAIRING_COMPLETE_BIT             0x0020000000000000

#define BT_HCI_USER_PASSKEY_NOTIFICATION_BIT           0x0400000000000000
#define BT_HCI_KEYPRESS_NOTIFICATION_EV_BIT            0x0800000000000000
#define BT_HCI_REMOTE_HOST_SUPPORTED_FEAT_NTF_EV_BIT   0x1000000000000000
#endif /* BT_SWDEV_2_1_SSP */
//
// Constants
//
#define BT_HCI_MAX_NAME_LEN        248

#define BT_HCI_MIN_CONN_ACCEPT_TO  0x0001
#define BT_HCI_MAX_CONN_ACCEPT_TO  0xB540

#define BT_HCI_MIN_PAGE_TO         0x0001
#define BT_HCI_MAX_PAGE_TO         0xFFFF

#define BT_HCI_EVENT_MASK_LEN   8

#define BT_HCI_PREFERRED_SNIFF_INTERVAL_MS 1280 

#define BT_HCI_MS_TO_625USEC( ms ) ((ms * 8) / 5)

#define BT_HCI_PREFERRED_SNIFF_INTERVAL  \
           (BT_HCI_MS_TO_625USEC( BT_HCI_PREFERRED_SNIFF_INTERVAL_MS ))


/* Specifies the number of packets to transmit before
** sending a HCI Num Completed Packets Event.
*/
#define BT_HCI_MIN_NUM_CMPLT_PKTS_VALUE    10

/* max packet size over HCI link */
#define BT_HCI_MAX_RX_PKT_SIZ  ( 339 + 4 )
#define BT_HCI_MAX_TX_PKT_SIZ  ( 339 + 4 )

/*-------------------------------------------------------------------------*/
/*                                                                         */
/*               External Data Declarations for BT module                  */
/*                                                                         */
/*-------------------------------------------------------------------------*/

extern bt_app_id_type    bt_hc_app_id;
extern bt_hci_mode_type  bt_hc_prev_hci_mode;

/*===========================================================================

                          FUNCTION DEFINITIONS

===========================================================================*/
/*-------------------------------------------------------------------------*/
/*                                                                         */
/*               Extern Funciton Declarations for BT module                */
/*                                                                         */
/*-------------------------------------------------------------------------*/


#ifdef BT_SWDEV_RADIO_OFF

/*===========================================================================

FUNCTION
  bt_hc_reset

DESCRIPTION
  Resets the HCI layer of the driver.

===========================================================================*/
extern void bt_hc_reset( void );

#endif /* BT_SWDEV_RADIO_OFF */

/*===========================================================================

FUNCTION
  bt_hc_powerup_init

DESCRIPTION
  Initialize the HCI Module upon powerup

===========================================================================*/
extern void bt_hc_powerup_init( void );


/*===========================================================================

FUNCTION
  bt_hc_qdsp_ready_init

DESCRIPTION
  Initialize the HCI Module when the Bluetooth QDSP image is loaded

===========================================================================*/
extern void bt_hc_qdsp_ready_init( void );


/*===========================================================================

FUNCTION
  bt_hc_qdsp_reset_shutdown

DESCRIPTION
  De-initialize the Bluetooth HCI Module when the Bluetooth QDSP image is
  unloaded

===========================================================================*/
extern void bt_hc_qdsp_reset_shutdown( void );


/*===========================================================================

FUNCTION
  bt_hc_process_cmd

DESCRIPTION
  Process the BT command sent to the HCI/LMP/BB layer.

===========================================================================*/
extern void bt_hc_process_cmd
(
  bt_cmd_msg_type*  bt_cmd_ptr
);

/*===========================================================================

FUNCTION
  bt_hc_finish_reset

DESCRIPTION
  Called after the driver has been reset.  If the reset was initiated by
  HCI_Reset, then make sure the driver is in HCI mode.  Also, send the
  command complete to indiate that reset is done.

===========================================================================*/
extern void bt_hc_finish_reset( void );

#endif /* FEATURE_BT */

#endif /* BTHC_H */

