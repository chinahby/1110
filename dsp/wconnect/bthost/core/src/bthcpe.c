/*====*====*====*====*====*====*====*====*====*====*====*====*====*====*====*

                 B L U E T O O T H    H C I    P A R S E R

GENERAL DESCRIPTION
  This module contains the Bluetooth HCI Parser.

              Copyright (c) 2000-2008 QUALCOMM Incorporated.               
                      All Rights Reserved.                                  
              Qualcomm Confidential and Proprietary                        

*====*====*====*====*====*====*====*====*====*====*====*====*====*====*====*/

/*===========================================================================

                        EDIT HISTORY FOR MODULE

  This section contains comments describing changes made to the module.
  Notice that changes are listed in reverse chronological order.

$PVCSHeader:   O:/src/asw/COMMON/vcs/bthcpe.c_v   1.18   12 Jul 2002 16:00:12   waynelee
$Header: //source/qcom/qct/wconnect/bthost/core/rel/00.00.26/src/bthcpe.c#1 $ $DateTime:

$Log:   O:/src/asw/COMMON/vcs/bthcpe.c_v  $
  when        who  what, where, why
  ----------  ---  -----------------------------------------------------------
  2008-11-05   pg  Generic handling for HCI Vendor Specific cmd group.
  2008-03-24   vk  Handling HCI Read/write Defalut Erroneous Data Reporting 
                   mode commands and command complete event for the same.

 *  #43         11 Mar 2008            DM
 * Fixed wrong formating of the HCI write EIR command when received from the 
 * tester.
 *
 *   #42        29 Feb 2008            AG
 * Renamed BT_HCI_IO_CAPABILITY_RESPONSE to BT_HCI_IO_CAPABILITY_RQST_REPLY
 * for consistency with spec.
 * 
 *   #41        21 Jan 2008            AG
 * Fixed PB flag of received ACL data
 *
 *   #40        16 Jan 2008           DSN 
 * Fixing handling for vendor-specific command groups.
 *
 *   #39        15 Jan 2008            DSN
 * Add support to allow vendor-specific commands and events in HCI test mode.
 *
 *   #38        07 Dec 2007            AG
 * Fixed parameter handling for 2.1 command BT_CMD_HC_WR_EXT_INQ_RSP.
 * 
 *   #37        05 Dec 2007            AG
 * Added support for 2.1.
 *
 *   #36        31 Oct 2007            VK
 * Added support for Inquiry result event with RSSI.
 *
 *   #35        16 Oct 2007            DSN
 * Added support for Rd Loc Supported commands and Rd local extended 
 * features command.
 *
 *   #34        11 Sep 2007            BK
 * Ignore command complete events from QSOC if QSOC is initializing.    
 *
 *   #33        20 Aug 2007            PG
 * Seperated HCI generic vendor specific event handling from Qualcomm 
 * proprietary implementation used with embedded controller.
 *
 *   #32         09 JUL 2007            PG
 * HCI Remote name req cancel and connection cancel support.
 *
 *   #31         03 Jul 2007            SSK
 * Fix HCI Reset bug in HCI APIT Mode on BTS targets where the SOC does not power
 * on back after HCI_RST cmd is issued.
 *
 *   #30         26 Jun 2007            DM
 * Added support to fill in the BC flag in the received data indication to the 
 * host, while in HCI mode.
 *
 *   #29         04 May 2007            PG
 * Added support for SOC1 vendor specific cmd/event handling.
 *
 *   #28         19 Apr 2007            PG
 * Featurized out code related to Qualcomm Vendor Specific commands for 
 * SOC targets.
 *
 *   #27         03 Apr 2007            AP
 * During BTS4020 initialization, re-direct vendor specific and cmd complete 
 * for HCI_RESET events to BTS4020 code.
 *
 *   #26         20 Feb 2007            DSN
 * Increasing the maximum number of events in HCI layer for SOC solutions.
 *
 *   #25         16 Nov 2006            DM
 * Modified the ACL Data Tx interface to comply with the changed interface 
 * for AV-Sync
 *
 *   #24         11 Aug 2006            GB
 * Modified to support APIT mode in ASIA
 * 
 *   #23         27 Jun 2006            DSN
 * Added featurization to support HostController SoC solution.
 *
 *   #22         04 May 2006            DM
 * Preventing QC internal Event "BT_EV_HC_QC_CUR_CONN_ENCRYPT" from being
 * sent to the Host in HCI Mode.
 *
 *    #21        30 Jan 2006            PN
 * Added code to handle BT_EV_HC_QC_RADIO_ACTIVITY_CHANGED event.
 *
 *    #20        03 Jan 2006            PG
 * Fixed bug found by Klocwork.
 *
 *    #19        19 Oct 2005            JH
 * Added support for the HCI read remote extended features command, read
 *  remote extended features complete event, synchronous connection complete
 *  event, and synchronous connection changed event.
 *
 *    #18        19 Sep 2005            JH
 * Added BT_HCI_SET_AFH_CHAN_GOOD in bt_hc_pe_process_ev_cmd_cmplt().
 *
 *    #16-17     12 Sep 2005            JTL
 * Improving DSM allocation.
 *
 *    #14        02 Aug 2005            GS
 * Append HCI data dsm to HCI header dsm item in bt_hc_pe_send_data() so that
 * both header and data are sent in a single Diag packet.
 *
 *    #13        25 Jul 2005            GS
 * Added support for HCI over DIAG (ASIA).
 *
 *    #12        27 Jun 2005            JH
 * Added new BT 1.2 commands: Read_Inquiry_Scan_Type, Write_Inquiry_Scan_Mode,
 *  Read_Inquiry_Mode, and Write_Inquiry_Mode
 * Replaced BT 1.1 commands: Read_Page_Scan_Mode and Write_Page_Scan_Mode with
 *  BT 1.2 commands: Read_Page_Scan_Type and Write_Page_Scan_Type.
 * 
 *    #11        06 Apr 2005            JH
 * Adding support for disabling BT radio, featurized using BT_SWDEV_RADIO_OFF.
 *
 *    #10        12 Aug 2004            JH
 * Added TX continuous frequency HCI command
 *
 *    #9         7  Jul 2004            JH
 * Added BT 1.2 AFH and eSCO support
 *
 *    #8         7  Jul 2004            DP
 * Prevent QC HCI Remote Auth Or Pair Failed event from going out UART.
 *
 *    #7         12 Sep 2003            RY
 * Updated bt_hc_pe_send_ev_pkt_to_host to support HCI in FTM.
 * 
 *    #6         29 Jul 2003            RY
 * Updated the length field of the BT_CMD_HC_TEST_SET_LM_PARAMS command.
 * 
 *    Rev 1.18   12 Jul 2002 16:00:12   waynelee
 * Added new param to enable cert lpm testing.
 * 
 *    Rev 1.17   03 Apr 2002 13:29:12   propach
 * Made use of dsm_pushdown_tail().
 * 
 *    Rev 1.16   12 Feb 2002 14:16:56   waynelee
 * Fixed sending 0 length l2cap packets.  Fixed data aborts
 * for tester mode.
 * 
 *    Rev 1.15   21 Sep 2001 13:32:32   waynelee
 * Fixed problem where hci_change_local_name via HCI over the UART was not
 * null-terminating the name string.
 * 
 *    Rev 1.14   13 Sep 2001 16:33:36   waynelee
 * Added peek/poke functionality via QBTS.
 * 
 *    Rev 1.13   07 Sep 2001 16:06:22   propach
 * Optimized event handling.
 * 
 *    Rev 1.12   29 Aug 2001 11:49:10   propach
 * Merged certification tip onto main tip.
 * 
 *    Rev 1.11.1.4   14 Aug 2001 19:27:02   waynelee
 * Renamed CMD_HC_FIX_PKT_HDR to CMD_HC_CERT_CTRL.  Removed
 * obsolete qdsp image ctrl cmds.
 * 
 *    Rev 1.11.1.3   30 Jul 2001 17:10:12   waynelee
 * Added support for new hc cmds/ev: rd_raw_rssi, 
 * test_set_lm_params, test_send_lmp_msg and
 * rd_raw_rssi ev.
 * 
 *    Rev 1.11.1.2   20 Jul 2001 18:19:48   waynelee
 * Fixed lengths of cmd_cmplt events through HCI.
 * 
 *    Rev 1.11.1.1   02 Jul 2001 16:26:38   waynelee
 * Properly handle the BT_EV_HC_NUM_CMPLT_PKTS.  This was causing DSMs to
 * run out after ~2600 packets sent in HCI mode. (17470)
 * 
 *    Rev 1.11.1.0   29 Jun 2001 15:23:26   waynelee
 * No change.
 * 
 *    Rev 1.11   29 Jun 2001 11:31:58   waynelee
 * Added HCI mechanism to not automatically unload QDSP image.
 * 
 *    Rev 1.10   28 Jun 2001 11:16:00   waynelee
 * Fixed number of params for modify beacon hci cmd
 * 
 *    Rev 1.9   27 Jun 2001 11:25:36   waynelee
 * Added cmds for telec and bb certification.
 * 
 *    Rev 1.8   08 Jun 2001 16:15:04   waynelee
 * Added support for HCI_Set_Tx_Power_Level
 * 
 *    Rev 1.7   02 Jun 2001 16:32:30   waynelee
 * Fixes test mode tester code to be able to send data greater 
 * than 243 bytes.
 * 
 *    Rev 1.6   23 Apr 2001 18:09:44   amitj
 * Fixed a typo in BT_HCI_PE_TYPE_CHECK_STATE
 * 
 *    Rev 1.5   20 Apr 2001 16:25:00   waynelee
 * Changed symbols from bt_hci_ to bt_hc_
 * 
 *    Rev 1.4   21 Mar 2001 16:48:20   waynelee
 * Added support for these commands: Set_QDSP_Image,
 * Set_Incoming_Hold_Params, Set_Incoming_Sniff_Params,
 * Set_Incoming_Park_Params.  Fixed improper conversion
 * of conn_hnld-- prevents data from being sent to a conn_hndl
 * other than 1.  Process the command_done event  (removes
 * some QXDM errors).
 * 
 *    Rev 1.3   02 Mar 2001 00:32:12   ryuen
 * Renamed bt_xx_offline to bt_xx_qdsp_reset_shutdown
 * 
 *    Rev 1.2   23 Feb 2001 13:14:56   ryuen
 * Renamed bt_hc_pe_init to bt_hci_pe_init and removed bt_hc_pe_online
 * 
 *    Rev 1.1   07 Feb 2001 16:45:46   waynelee
 * Made HCI 1.1 compliant.
 * 
 *    Rev 1.0   29 Jan 2001 17:07:26   propach
 * Initial revision.
 * 
 *    Rev 1.27   18 Jan 2001 11:37:00   waynelee
 * allowing the test mode only build to have tester functionality
 * 
 *    Rev 1.26   21 Dec 2000 17:41:22   waynelee
 * Removed T_CORE_EMUL.  Removed usage of ASSERT(), bt_record_fault(),
 * and ERR().
 * 
 *    Rev 1.25   06 Dec 2000 10:23:40   propach
 * Improved Q depth statistics gathering.
 * 
 *    Rev 1.24   22 Nov 2000 18:06:10   waynelee
 * Allow test mode only image all of hci except v/i stuff
 * 
 *    Rev 1.23   22 Nov 2000 12:18:54   waynelee
 * Added support for enable_loopback HCI command
 * 
 *    Rev 1.22   10 Nov 2000 12:25:28   ryuen
 * Fixed problem introduced in previous checkin (r1.21)
 * 
 *    Rev 1.21   07 Nov 2000 12:15:44   ryuen
 * Allow access to all HCI commands only if FEATURE_BT_TEST_MODE_ONLY is
 * undefined; otherwise, only RD_SBI_REG and WR_SBI_REG are accessible
 * 
 *    Rev 1.20   01 Nov 2000 16:55:40   waynelee
 * Changed the size of encrypt key length HCI message
 * 
 *    Rev 1.19   27 Oct 2000 18:02:46   waynelee
 * Added support for new HCI commands and events
 * 
 *    Rev 1.18   25 Oct 2000 09:58:24   waynelee
 * Fixed from problem with chained dsm l2cap data.
 * 
 *    Rev 1.17   16 Oct 2000 15:31:02   propach
 * Fixed Q depth checks.
 *
 *    Rev 1.16   04 Oct 2000 15:10:58   propach
 * RF test mode changes per WAL (corrected).
 *
 *    Rev 1.14   29 Sep 2000 17:40:04   waynelee
 * externed bt_sd_ev_l2_rx_data()
 *
 *    Rev 1.0   29 Sep 2000 17:32:18   waynelee
 * Initial revision.
 *
 *    Rev 1.13   21 Sep 2000 12:06:44   waynelee
 * fixed auth/encrypt; fixed inquiry; added scan modes; other hs fixes
 *
 *    Rev 1.12   06 Sep 2000 10:35:28   propach
 * Moved SIO setup to DC.  Merged AG/HS.
 *
 *    Rev 1.11   05 Sep 2000 16:44:28   waynelee
 * Added online, offline support.  Added debug MSGs.  Added Robustness.
 *
 *    Rev 1.10   31 Aug 2000 10:44:46   propach
 * Featurized APIT code.  Improved APIT watermark proxy handling.
 *
 *    Rev 1.9   30 Aug 2000 09:41:48   waynelee
 * Made changes for new BT_MSG macros
 *
 *    Rev 1.8   29 Aug 2000 10:28:34   propach
 * Expanded command Q, fixed q depth counters.
 *
 *    Rev 1.7   29 Aug 2000 08:25:14   propach
 * Moved comdef.h includes above FEATURE_BT to remove compiler
 * warning when feature is disabled.
 *
 *    Rev 1.6   28 Aug 2000 16:12:26   propach
 * Cleanup RFCOMM.  Add use of msg/err macros.  Add driver fault
 * recording and statistics gathering.
 *
 *    Rev 1.5   03 Aug 2000 08:41:50   waynelee
 * Adding Low Power Mode Support.  Code for UPF-3
 *
 *    Rev 1.4   02 Aug 2000 09:40:04   propach
 * Added QBTS support for BT.  Added to API.
 *
 *    Rev 1.3   19 Jul 2000 17:25:48   propach
 * Update BT API and featurization with cleanup.
 *
 *    Rev 1.2   07 Jul 2000 18:28:34   waynelee
 * Fixing basic HCI events
 *
 *    Rev 1.1   06 Jul 2000 13:22:08   waynelee
 * Added V/I features: change to AUX packet types, set new bd addr.
 * Writes to QDSP CRC table for 0 length packets after connection.
 * Got HCI mode working on both standalone and aarvark targets. verified
 * AG/HS connection on AARVARK target.
 *
 *    Rev 1.0   04 Jul 2000 19:20:00   ryuen
 * Initial revision.

===========================================================================*/

/*===========================================================================

                     INCLUDE FILES FOR MODULE

===========================================================================*/

#include "customer.h"
#include "comdef.h"
#ifdef FEATURE_BT
#include <string.h>
#include "assert.h"
#include "dsm.h"
#include "sio.h"
#include "task.h"
#include "bt.h"
#include "bti.h"
#include "btcmdi.h"
#include "bthc.h"
#include "bthci.h"
#ifdef FEATURE_BT_TEST_API
#error code not present
#endif

#ifdef FEATURE_BT_SOC 
#include "btsoc.h"
#ifdef FEATURE_BT_QSOC
#include "btqsoc.h"
#endif /* FEATURE_BT_QSOC */
#else
#include "btlm.h"
#include "btqdspq.h"
#endif /* FEATURE_BT_SOC */

#include "btrm.h"
#include "btutils.h"
#include "btmsg.h"
#ifdef FEATURE_BT_SOC1
#error code not present
#endif

#define BT_MSG_LAYER  BT_MSG_HC   /* necessary for btmsg.h */


/*===========================================================================

                DEFINITIONS AND DECLARATIONS FOR MODULE

This section contains definitions for constants, macros, types, variables
and other items needed by this module.

===========================================================================*/

#define BT_HC_MAX_PARAM_LEN   255
#define BT_HC_MAX_DATA_LEN    360

// parse engine's states
typedef enum
{
  BT_HC_PE_TYPE_CHECK_STATE,
  BT_HC_PE_PROCESS_COMMAND_STATE,
  BT_HC_PE_PROCESS_DATA_STATE
#ifdef FEATURE_BT_TEST_API
#error code not present
#endif
} bt_hc_parser_state_type;

typedef enum
{
  BT_HC_PE_CMD_OPCODE_STATE,
  BT_HC_PE_CMD_PARAMS_LEN_STATE,
  BT_HC_PE_CMD_PARAMS_STATE
} bt_hc_pe_cmd_pkt_sub_state_type;

typedef enum
{
  BT_HC_PE_DATA_CONN_HNDL_STATE,
  BT_HC_PE_DATA_LEN_STATE,
  BT_HC_PE_DATA_STATE
} bt_hc_data_pkt_sub_state_type;

// Parser engine data
typedef struct
{
  bt_hc_parser_state_type       state;
  boolean                       sync_val_read;
  bt_hci_pkt_type               pkt_type;
  word                          index;   // idx into bt_hc_pe_func_info_table
  word                          opcode;
  bt_cmd_type                   bt_cmd;

  word                          data_params_len;
  word                          data_param_bytes_received;
  byte                          cmd_params[ BT_HC_MAX_PARAM_LEN ];
  byte                         *cmd_param_ptr;
  dsm_item_type                *dsm_data_param_ptr;

  bt_hc_pkt_boundary_flag_type  pkt_boundary_flag;
  bt_hc_broadcast_flag_type     broadcast_flag;
  word                          connection_hndl;
} bt_hc_parser_info_type;

typedef struct
{
  word         opcode;            // combination of command and group subfield
  word         parameter_length;  // # of bytes of all the params for this function
  bt_cmd_type  bt_cmd;            // the equivalent bt cmd
  boolean      cmd_cmplt_ev_expected;
} bt_hc_cmd_func_info_type;

typedef struct
{
  byte type_indicator;  // always BT_HCI_EVENT_PKT
  byte event_code;
  byte param_len;
  byte params[255];
} bt_hc_pe_ev_pkt_type;

typedef BT_PACKED struct
{
  uint8   pkt_type;  // always BT_HCI_DATA_PKT
  uint16  conn_hndl;
  uint16  len_field;
} bt_hc_pe_data_hdr_type;

#define BT_HC_PE_DATA_HDR_LEN  ( sizeof( bt_hc_pe_data_hdr_type ) )

typedef struct
{
  bt_event_type   bt_ev;    /* BT HCI event enum value                     */
  bt_hci_ev_type  hci_ev;   /* HCI event enum value according to BT spec   */
  uint8           hci_len;  /* size of the HCI event param.  0 if variable */
} bt_hc_ev_data_type;

/*-------------------------------------------------------------------------*/
/*                                                                         */
/*                         Data Definitions                                */
/*                                                                         */
/*-------------------------------------------------------------------------*/

/*-------------------------------------------------------------------------*/
/*  Information and queue for events destined for the HCI parser module.   */
/*-------------------------------------------------------------------------*/
#ifdef FEATURE_BT_SOC
  #define BT_HC_NUM_EVENTS   100
#else  
  #define BT_HC_NUM_EVENTS   30
#endif /* FEATURE_BT_SOC */
LOCAL bt_event_q_info_type bt_hc_ev_q_info;
LOCAL q_type               bt_hc_ev_q;

LOCAL uint32 bt_hc_pe_sync_errors;

/* Indicate the HCI_RST state during HCI test mode */
LOCAL boolean bt_hc_pe_hci_rst_state = FALSE;

LOCAL bt_hc_cmd_func_info_type bt_hc_pe_func_info_table[] =
{
  // Link Control Commands
  { BT_HCI_INQUIRY,                          5,      BT_CMD_HC_INQUIRY,                FALSE},
  { BT_HCI_INQUIRY_CANCEL,                   0,      BT_CMD_HC_INQUIRY_CANCEL,         TRUE},
  { BT_HCI_PERIODIC_INQUIRY_MODE,            9,      BT_CMD_HC_PERIODIC_INQUIRY,       TRUE},
  { BT_HCI_EXIT_PERIODIC_INQUIRY_MODE,       0,      BT_CMD_HC_EXIT_PERIODIC_INQUIRY,  TRUE},
  { BT_HCI_CREATE_CONNECTION,                13,     BT_CMD_HC_CREATE_CONNECTION,      FALSE},
  { BT_HCI_DISCONNECT,                       3,      BT_CMD_HC_DISCONNECT,             FALSE},
  { BT_HCI_ADD_SCO_CONNECTION,               4,      BT_CMD_HC_ADD_SCO_CONN,           FALSE},
  { BT_HCI_ACCEPT_CONNECTION_RQST,           7,      BT_CMD_HC_ACCEPT_CONN,            FALSE},
  { BT_HCI_REJECT_CONNECTION_RQST,           7,      BT_CMD_HC_REJECT_CONN,            FALSE},
  { BT_HCI_LINK_KEY_RQST_REPLY,              22,     BT_CMD_HC_LINK_KEY,               TRUE},
  { BT_HCI_LINK_KEY_RQST_NEG_REPLY,          6,      BT_CMD_HC_LINK_KEY_NEG,           TRUE},
  { BT_HCI_PIN_CODE_RQST_REPLY,              23,     BT_CMD_HC_PIN_CODE,               TRUE},
  { BT_HCI_PIN_CODE_RQST_NEG_REPLY,          6,      BT_CMD_HC_PIN_CODE_NEG,           TRUE},
  { BT_HCI_CHANGE_CONNECTION_PKT_TYPE,       4,      BT_CMD_HC_CHANGE_PKT_TYPE,        FALSE},
  { BT_HCI_AUTHENTICATION_REQUESTED,         2,      BT_CMD_HC_AUTH_REQ,               FALSE},
  { BT_HCI_SET_CONNECTION_ENCRYPTION,        3,      BT_CMD_HC_SET_CONN_ENCRYPT,       FALSE},
  { BT_HCI_CHANGE_CONNECTION_LINK_KEY,       2,      BT_CMD_HC_CHANGE_LINK_KEY,        FALSE},
  { BT_HCI_MASTER_LINK_KEY,                  1,      BT_CMD_HC_MASTER_LINK_KEY,        FALSE},
  { BT_HCI_REMOTE_NAME_RQST,                 10,     BT_CMD_HC_REMOTE_NAME_REQ,        FALSE},
  { BT_HCI_RD_REMOTE_SUPPORTED_FEAT,         2,      BT_CMD_HC_RD_REMOTE_FEATURES,     FALSE},
#ifdef FEATURE_BT_1_2
  { BT_HCI_RD_REMOTE_EXTENDED_FEAT,          3,      BT_CMD_HC_RD_REMOTE_EXT_FEATURES, FALSE},
  { BT_HCI_CREATE_CONNECTION_CANCEL,         6,      BT_CMD_HC_CREATE_CONNECTION_CANCEL, TRUE},
  { BT_HCI_REMOTE_NAME_RQST_CANCEL,          6,      BT_CMD_HC_REMOTE_NAME_REQ_CANCEL, TRUE},
#endif
  { BT_HCI_RD_REMOTE_VERSION_INFO,           2,      BT_CMD_HC_RD_REMOTE_VERSION,      FALSE},
  { BT_HCI_RD_CLOCK_OFFSET,                  2,      BT_CMD_HC_RD_CLK_OFFSET,          FALSE},
#ifdef FEATURE_BT_2_1
  { BT_HCI_IO_CAPABILITY_RQST_REPLY,       9, BT_CMD_HC_IO_CAPABILITY_RQST_REPLY,      TRUE},
  { BT_HCI_USER_CFMTN_RQST_REPLY,          6, BT_CMD_HC_USER_CFMTN_RQST_REPLY,         TRUE},
  { BT_HCI_USER_CFMTN_RQST_NEG_REPLY,      6, BT_CMD_HC_USER_CFMTN_RQST_NEG_REPLY,     TRUE},
  { BT_HCI_USER_PASSKEY_RQST_REPLY,       10, BT_CMD_HC_USER_PASSKEY_RQST_REPLY,       TRUE},
  { BT_HCI_USER_PASSKEY_RQST_NEG_REPLY,    6, BT_CMD_HC_USER_PASSKEY_RQST_NEG_REPLY,   TRUE},
  { BT_HCI_REMOTE_OOB_DATA_RQST_REPLY,    38, BT_CMD_HC_REMOTE_OOB_DATA_RQST_REPLY,    TRUE},
  { BT_HCI_REMOTE_OOB_DATA_RQST_NEG_REPLY, 6, BT_CMD_HC_REMOTE_OOB_DATA_RQST_NEG_REPLY,TRUE},
  { BT_HCI_IO_CAPABILITY_RQST_NEG_REPLY,   7, BT_CMD_HC_IO_CAPABILITY_RQST_NEG_REPLY,  TRUE},
#endif /* FEATURE_BT_2_1 */

  // Link Policy Commands
  { BT_HCI_HOLD_MODE,                        6,      BT_CMD_HC_HOLD_MODE,              FALSE},
  { BT_HCI_SNIFF_MODE,                       10,     BT_CMD_HC_SNIFF_MODE,             FALSE},
  { BT_HCI_EXIT_SNIFF_MODE,                  2,      BT_CMD_HC_EXIT_SNIFF_MODE,        FALSE},
  { BT_HCI_PARK_MODE,                        6,      BT_CMD_HC_PARK_MODE,              FALSE},
  { BT_HCI_EXIT_PARK_MODE,                   2,      BT_CMD_HC_EXIT_PARK_MODE,         FALSE},
  { BT_HCI_QOS_SETUP,                        20,     BT_CMD_HC_QOS_SETUP,              FALSE},
  { BT_HCI_ROLE_DISCOVERY,                   2,      BT_CMD_HC_ROLE_DISCOVERY,         TRUE},
  { BT_HCI_SWITCH_ROLE,                      7,      BT_CMD_HC_SWITCH_ROLE,            FALSE},
  { BT_HCI_RD_LINK_POLICY_SETTINGS,          2,      BT_CMD_HC_RD_LINK_POLICY,         TRUE},
  { BT_HCI_WR_LINK_POLICY_SETTINGS,          4,      BT_CMD_HC_WR_LINK_POLICY,         TRUE},
#ifdef FEATURE_BT_2_1
  { BT_HCI_SNIFF_SUBRATING,                  8,      BT_CMD_HC_SNIFF_SUBRATING,        TRUE},
#endif /* FEATURE_BT_2_1 */

  // Host Control & Baseband Commands
  { BT_HCI_SET_EVENT_MASK,                   8,      BT_CMD_HC_SET_EVENT_MASK,         TRUE},
  { BT_HCI_RESET,                            0,      BT_CMD_HC_RESET,                  TRUE},
  { BT_HCI_SET_EVENT_FILTER,                 0xffff, BT_CMD_HC_SET_EVENT_FILTER,       TRUE},
  { BT_HCI_FLUSH,                            2,      BT_CMD_HC_FLUSH,                  TRUE},
  { BT_HCI_RD_PIN_TYPE,                      0,      BT_CMD_HC_RD_PIN_TYPE,            TRUE},
  { BT_HCI_WR_PIN_TYPE,                      1,      BT_CMD_HC_WR_PIN_TYPE,            TRUE},
  { BT_HCI_CREATE_NEW_UNIT_KEY,              0,      BT_CMD_HC_CREATE_NEW_UNIT_KEY,    TRUE},
  { BT_HCI_RD_STORED_LINK_KEY,               7,      BT_CMD_HC_RD_STORED_LINK_KEY,     TRUE},
  { BT_HCI_WR_STORED_LINK_KEY,               0xffff, BT_CMD_HC_WR_STORED_LINK_KEY,     TRUE},
  { BT_HCI_DELETE_STORED_LINK_KEY,           7,      BT_CMD_HC_DEL_STORED_LINK_KEY,    TRUE},
  { BT_HCI_CHANGE_LOCAL_NAME,                248,    BT_CMD_HC_CHANGE_LOCAL_NAME,      TRUE},
  { BT_HCI_RD_CONN_ACCEPT_TIMEOUT,           0,      BT_CMD_HC_RD_CONN_ACCEPT_TO,      TRUE},
  { BT_HCI_WR_CONN_ACCEPT_TIMEOUT,           2,      BT_CMD_HC_WR_CONN_ACCEPT_TO,      TRUE},
  { BT_HCI_RD_PAGE_TIMEOUT,                  0,      BT_CMD_HC_RD_PAGE_TO,             TRUE},
  { BT_HCI_WR_PAGE_TIMEOUT,                  2,      BT_CMD_HC_WR_PAGE_TO,             TRUE},
  { BT_HCI_RD_SCAN_ENABLE,                   0,      BT_CMD_HC_RD_SCAN_ENABLE,         TRUE},
  { BT_HCI_WR_SCAN_ENABLE,                   1,      BT_CMD_HC_WR_SCAN_ENABLE,         TRUE},

  { BT_HCI_RD_PAGESCAN_ACTIVITY,             0,      BT_CMD_HC_RD_PAGE_SCAN_ACTIVITY,  TRUE},
  { BT_HCI_WR_PAGESCAN_ACTIVITY,             4,      BT_CMD_HC_WR_PAGE_SCAN_ACTIVITY,  TRUE},
  { BT_HCI_RD_INQUIRYSCAN_ACTIVITY,          0,      BT_CMD_HC_RD_INQ_SCAN_ACTIVITY,   TRUE},
  { BT_HCI_WR_INQUIRYSCAN_ACTIVITY,          4,      BT_CMD_HC_WR_INQ_SCAN_ACTIVITY,   TRUE},
  { BT_HCI_RD_AUTHENTICATION_ENABLE,         0,      BT_CMD_HC_RD_AUTH_ENABLE,         TRUE},
  { BT_HCI_WR_AUTHENTICATION_ENABLE,         1,      BT_CMD_HC_WR_AUTH_ENABLE,         TRUE},
  { BT_HCI_RD_ENCRYPTION_MODE,               0,      BT_CMD_HC_RD_ENCRYPT_MODE,        TRUE},
  { BT_HCI_WR_ENCRYPTION_MODE,               1,      BT_CMD_HC_WR_ENCRYPT_MODE,        TRUE},
  { BT_HCI_RD_CLASS_OF_DEVICE,               0,      BT_CMD_HC_RD_CLASS_OF_DEVICE,     TRUE},
  { BT_HCI_WR_CLASS_OF_DEVICE,               3,      BT_CMD_HC_WR_CLASS_OF_DEVICE,     TRUE},
  { BT_HCI_RD_VOICE_SETTING,                 0,      BT_CMD_HC_RD_VOICE_SETTING,       TRUE},
  { BT_HCI_WR_VOICE_SETTING,                 2,      BT_CMD_HC_WR_VOICE_SETTING,       TRUE},
  { BT_HCI_RD_AUTO_FLUSH_TIMEOUT,            2,      BT_CMD_HC_RD_AUTO_FLUSH_TO,       TRUE},
  { BT_HCI_WR_AUTO_FLUSH_TIMEOUT,            4,      BT_CMD_HC_WR_AUTO_FLUSH_TO,       TRUE},
  { BT_HCI_RD_NUM_BC_RETRANSMISSIONS,        0,      BT_CMD_HC_RD_BROAD_RETRANS,       TRUE},
  { BT_HCI_WR_NUM_BC_RETRANSMISSIONS,        1,      BT_CMD_HC_WR_BROAD_RETRANS,       TRUE},
  { BT_HCI_RD_HOLD_MODE_ACTIVITY,            0,      BT_CMD_HC_RD_HOLD_MODE_ACTIVITY,  TRUE},
  { BT_HCI_WR_HOLD_MODE_ACTIVITY,            1,      BT_CMD_HC_WR_HOLD_MODE_ACTIVITY,  TRUE},
  { BT_HCI_RD_TRANSMIT_POWER_LEVEL,          3,      BT_CMD_HC_RD_TX_POWER_LEVEL,      TRUE},
  { BT_HCI_RD_SCO_FLOW_CONTROL_ENABLE,       0,      BT_CMD_HC_RD_SCO_FLOW_CTRL,       TRUE},
  { BT_HCI_WR_SCO_FLOW_CONTROL_ENABLE,       1,      BT_CMD_HC_WR_SCO_FLOW_CTRL,       TRUE},
  { BT_HCI_SET_HC_TO_HOST_FLOW_CONTROL,      1,      BT_CMD_HC_SET_HC_TO_HOST_FC,      TRUE},
  { BT_HCI_HOST_BUFFER_SIZE,                 7,      BT_CMD_HC_HOST_BUFFER_SIZE,       TRUE},
  { BT_HCI_HOST_NUM_COMPLETED_PACKETS,       0xffff, BT_CMD_HC_HOST_NUM_CMPLT_PACKETS, TRUE},
  { BT_HCI_RD_LINK_SUPERVISION_TIMEOUT,      2,      BT_CMD_HC_RD_LINK_SUPERVISION_TO, TRUE},
  { BT_HCI_WR_LINK_SUPERVISION_TIMEOUT,      4,      BT_CMD_HC_WR_LINK_SUPERVISION_TO, TRUE},
  { BT_HCI_RD_NUMBER_OF_SUPPORTED_IAC,       0,      BT_CMD_HC_RD_NUM_SUPPORTED_IAC,   TRUE},
  { BT_HCI_RD_CURRENT_IAC_LAP,               0,      BT_CMD_HC_RD_CURRENT_IAC_LAP,     TRUE},
  { BT_HCI_WR_CURRENT_IAC_LAP,               0xffff, BT_CMD_HC_WR_CURRENT_IAC_LAP,     TRUE},
  { BT_HCI_RD_PAGE_SCAN_PERIOD_MODE,         0,      BT_CMD_HC_RD_PAGE_SCAN_PER_MODE,  TRUE},
  { BT_HCI_WR_PAGE_SCAN_PERIOD_MODE,         1,      BT_CMD_HC_WR_PAGE_SCAN_PER_MODE,  TRUE},
#ifndef FEATURE_BT_1_2 /* BT 1.1 */
  { BT_HCI_RD_PAGE_SCAN_MODE,                0,      BT_CMD_HC_RD_PAGE_SCAN_MODE,      TRUE},
  { BT_HCI_WR_PAGE_SCAN_MODE,                1,      BT_CMD_HC_WR_PAGE_SCAN_MODE,      TRUE},
#endif

#ifdef FEATURE_BT_1_2
  // BT 1.2 AFH and eSCO related commands
  { BT_HCI_SET_AFH_HOST_CHAN_CLASS,     10,  BT_CMD_HC_SET_AFH_HOST_CHAN_CLASS,   TRUE },
  { BT_HCI_READ_AFH_CHAN_ASSESS_MODE,    0,  BT_CMD_HC_READ_AFH_CHAN_ASSESS_MODE, TRUE },
  { BT_HCI_SET_AFH_CHAN_ASSESS_MODE,     1,  BT_CMD_HC_SET_AFH_CHAN_ASSESS_MODE,  TRUE },
  { BT_HCI_READ_AFH_CHAN_MAP,            2,  BT_CMD_HC_READ_AFH_CHAN_MAP,         TRUE },
  { BT_HCI_SETUP_SYNC_CONNECTION,       17,  BT_CMD_HC_SETUP_SYNC_CONNECTION,    FALSE },
  { BT_HCI_ACCEPT_SYNC_CONN_REQ,        21,  BT_CMD_HC_ACCEPT_SYNC_CONN_REQ,     FALSE },
  { BT_HCI_REJECT_SYNC_CONN_REQ,         7,  BT_CMD_HC_REJECT_SYNC_CONN_REQ,     FALSE },
  { BT_HCI_RD_INQUIRY_SCAN_TYPE,         0,  BT_CMD_HC_RD_INQUIRY_SCAN_TYPE,      TRUE },
  { BT_HCI_WR_INQUIRY_SCAN_TYPE,         1,  BT_CMD_HC_WR_INQUIRY_SCAN_TYPE,      TRUE },
  { BT_HCI_RD_INQUIRY_MODE,              0,  BT_CMD_HC_RD_INQUIRY_MODE,           TRUE },
  { BT_HCI_WR_INQUIRY_MODE,              1,  BT_CMD_HC_WR_INQUIRY_MODE,           TRUE },
  { BT_HCI_RD_PAGE_SCAN_TYPE,            0,  BT_CMD_HC_RD_PAGE_SCAN_TYPE,         TRUE },
  { BT_HCI_WR_PAGE_SCAN_TYPE,            1,  BT_CMD_HC_WR_PAGE_SCAN_TYPE,         TRUE },
#endif /* FEATURE_BT_1_2 */
#ifdef FEATURE_BT_2_1
  { BT_HCI_ENH_FLUSH,                    3,  BT_CMD_HC_ENH_FLUSH,                 TRUE},
  { BT_HCI_RD_EXT_INQ_RSP,               0,  BT_CMD_HC_RD_EXT_INQ_RSP,            TRUE},
  { BT_HCI_WR_EXT_INQ_RSP,          0xffff,  BT_CMD_HC_WR_EXT_INQ_RSP,            TRUE},
  { BT_HCI_RD_INQ_RSP_TX_PWR,            0,  BT_CMD_HC_RD_INQ_RSP_TX_PWR,         TRUE},
  { BT_HCI_WR_INQ_TX_PWR,                1,  BT_CMD_HC_WR_INQ_TX_PWR,             TRUE},
  { BT_HCI_REFRESH_ENCRYPTION_KEY,       2,  BT_CMD_HC_REFRESH_ENCRYPTION_KEY,    FALSE},
  { BT_HCI_RD_SIMPLE_PAIRING_MODE,       0,  BT_CMD_HC_RD_SIMPLE_PAIRING_MODE,    TRUE},
  { BT_HCI_WR_SIMPLE_PAIRING_MODE,       1,  BT_CMD_HC_WR_SIMPLE_PAIRING_MODE,    TRUE},
  { BT_HCI_RD_LOCAL_OOB_DATA,            0,  BT_CMD_HC_RD_LOCAL_OOB_DATA,         TRUE},
  { BT_HCI_RD_DEFAULT_ERR_DATA_REPORTING, 0,  
                                   BT_CMD_HC_RD_DEFAULT_ERR_DATA_REPORTING_MODE,  TRUE},
  { BT_HCI_WR_DEFAULT_ERR_DATA_REPORTING, 1,  
                                   BT_CMD_HC_WR_DEFAULT_ERR_DATA_REPORTING_MODE,  TRUE},
  { BT_HCI_SEND_KEYPRESS_NOTIFICATION,   7,  BT_CMD_HC_SEND_KEYPRESS_NOTIFICATION,TRUE},
#endif /* FEATURE_BT_2_1 */

  // Information Parameters
  { BT_HCI_RD_LOCAL_VERSION_INFO,            0,      BT_CMD_HC_RD_LOCAL_VERSION_INFO,  TRUE},
  { BT_HCI_RD_LOCAL_SUPPORTED_CMDS,          0,      BT_CMD_HC_RD_LOCAL_CMDS,          TRUE},
  { BT_HCI_RD_LOCAL_SUPPORTED_FEAT,          0,      BT_CMD_HC_RD_LOCAL_FEATURES,      TRUE},

  { BT_HCI_RD_LOCAL_EXTENDED_FEATURES,       1,      BT_CMD_HC_RD_LOCAL_EXT_FEAT,      TRUE},

  { BT_HCI_RD_BUFFER_SIZE,                   0,      BT_CMD_HC_RD_BUFFER_SIZE,         TRUE},
  { BT_HCI_RD_COUNTRY_CODE,                  0,      BT_CMD_HC_RD_COUNTRY_CODE,        TRUE},
  { BT_HCI_RD_BD_ADDR,                       0,      BT_CMD_HC_RD_BD_ADDR,             TRUE},

  // Status Parameters
  { BT_HCI_RD_FAILED_CONTACT_COUNTER,        2,      BT_CMD_HC_RD_FAILED_CONTACT_COUNTER,    TRUE},
  { BT_HCI_RESET_FAILED_CONTACT_COUNTER,     2,      BT_CMD_HC_RESET_FAILED_CONTACT_COUNTER, TRUE},
  { BT_HCI_GET_LINK_QUALITY,                 2,      BT_CMD_HC_GET_LINK_QUALITY,             TRUE},
  { BT_HCI_RD_RSSI,                          2,      BT_CMD_HC_RD_RSSI,                      TRUE},

  // Test Commands
  { BT_HCI_RD_LOOPBACK_MODE,                 0,      BT_CMD_HC_RD_LOOPBACK_MODE,       TRUE},
  { BT_HCI_WR_LOOPBACK_MODE,                 1,      BT_CMD_HC_WR_LOOPBACK_MODE,       TRUE},
  { BT_HCI_ENABLE_DUT_MODE,                  0,      BT_CMD_HC_ENABLE_DUT_MODE,        TRUE}
#ifdef FEATURE_BT_2_1
 ,{ BT_HCI_WR_SIMPLE_PAIRING_DEBUG_MODE,     1,      BT_CMD_HC_WR_SIMPLE_PAIRING_DEBUG_MODE, TRUE}
#endif /* FEATURE_BT_2_1 */

#ifndef FEATURE_BT_SOC
  // Qualcomm specific commands
#ifndef FEATURE_BT_TEST_MODE_ONLY
 ,{ BT_HCI_FIX_INIT_WHITE,               4,  BT_CMD_HC_FIX_INIT_WHITE,    TRUE },
  { BT_HCI_FIX_INIT_ENC,                29,  BT_CMD_HC_FIX_INIT_ENC,      TRUE },
  { BT_HCI_READ_RX_STATUS_ENABLE,        1,  BT_CMD_HC_RD_RX_STATUS_EN,   TRUE },
  { BT_HCI_CONFIG_SEARCHER,             13,  BT_CMD_HC_CONFIG_SCHR,       TRUE },
  { BT_HCI_READ_TX_BUFFER_ENABLE,        1,  BT_CMD_HC_RD_TX_BUFFER_EN,   TRUE },
  { BT_HCI_READ_RX_BUFFER_ENABLE,        1,  BT_CMD_HC_RD_RX_BUFFER_EN,   TRUE },
  { BT_HCI_SEND_RAW_MODE_PACKET,    0xFFFF,  BT_CMD_HC_SEND_RAW_MODE_PKT, TRUE }
#endif /* !defined( FEATURE_BT_TEST_MODE_ONLY ) */
#ifdef FEATURE_BT_TEST_MODE_TESTER
 ,{ BT_HCI_TESTER_ACTIVATE,              2,  BT_CMD_HC_TESTER_ACTIVATE,  FALSE },
  { BT_HCI_TESTER_CONTROL,          0xFFFF,  BT_CMD_HC_TESTER_CONTROL,   FALSE },
  { BT_HCI_TESTER_START,                 2,  BT_CMD_HC_TESTER_START,      TRUE }
#endif /* FEATURE_BT_TEST_MODE_TESTER */
 ,{ BT_HCI_SET_NEW_BD_ADDR,              6,  BT_CMD_HC_SET_NEW_BD_ADDR,   TRUE },

  { BT_HCI_LMP_TIME_ACCURACY_REQ,        0,  BT_CMD_HC_TIME_ACCURACY_REQ,FALSE },
  { BT_HCI_MODIFY_BEACON,               16,  BT_CMD_HC_MODIFY_BEACON,     TRUE },
  { BT_HCI_RD_CLK,                       0,  BT_CMD_HC_RD_CLK,            TRUE },
  { BT_HCI_DIAC_LAP_EN,                  2,  BT_CMD_HC_DIAC_LAP_EN,       TRUE },
  { BT_HCI_RD_ENC_KEY_LENGTH,            0,  BT_CMD_HC_RD_ENC_KEY_LENGTH,FALSE },
  { BT_HCI_WR_ENC_KEY_LENGTH,            2,  BT_CMD_HC_WR_ENC_KEY_LENGTH, TRUE },
  { BT_HCI_CHANGE_REMOTE_PKT_TYPE,       4,  BT_CMD_HC_CHANGE_REMOTE_PKT_TYPE, TRUE },
  { BT_HCI_STATUS_VIEW_EN,               1,  BT_CMD_HC_STATUS_VIEW_EN,    TRUE },

  { BT_HCI_RD_SBI_REG,                   1,  BT_CMD_HC_RD_SBI_REG,       FALSE },
  { BT_HCI_WR_SBI_REG,                   2,  BT_CMD_HC_WR_SBI_REG,        TRUE },
  { BT_HCI_ENABLE_LOOPBACK,              4,  BT_CMD_HC_ENABLE_LOOPBACK,   TRUE },
  { BT_HCI_SET_INCOMING_HOLD_PARAMS,     6,  BT_CMD_HC_SET_INCOMING_HOLD_PARAMS,  TRUE },
  { BT_HCI_SET_INCOMING_SNIFF_PARAMS,    6,  BT_CMD_HC_SET_INCOMING_SNIFF_PARAMS, TRUE },
  { BT_HCI_SET_INCOMING_PARK_PARAMS,     6,  BT_CMD_HC_SET_INCOMING_PARK_PARAMS,  TRUE },
  { BT_HCI_SET_TX_POWER_LEVEL,           3,  BT_CMD_HC_SET_TX_POWER_LEVEL,  TRUE },
  { BT_HCI_CERT_CTRL,                    8,  BT_CMD_HC_CERT_CTRL,  TRUE },
  { BT_HCI_TX_CONTINUOUS,                1,  BT_CMD_HC_TX_CONTINUOUS, TRUE },
  { BT_HCI_TX_CONTINUOUS_FREQ,           1,  BT_CMD_HC_TX_CONTINUOUS_FREQ, TRUE },

  { BT_HCI_RD_RAW_RSSI,                  2,  BT_CMD_HC_RD_RAW_RSSI, TRUE },
  { BT_HCI_TEST_SET_LM_PARAMS,           5,  BT_CMD_HC_TEST_SET_LM_PARAMS, TRUE },
  { BT_HCI_TEST_SEND_LMP_MSG,           10,  BT_CMD_HC_TEST_SEND_LMP_MSG, TRUE },
  { BT_HCI_WR_RAM,                       9,  BT_CMD_HC_WR_RAM, TRUE },
  { BT_HCI_RD_RAM,                       5,  BT_CMD_HC_RD_RAM, TRUE }       
#ifdef FEATURE_BT_1_2
 ,{ BT_HCI_CONTROL_AFH,                  1,  BT_CMD_HC_CONTROL_AFH,               TRUE },
  { BT_HCI_SET_AFH_CHAN_GOOD,           10,  BT_CMD_HC_SET_AFH_CHAN_GOOD,         TRUE },
  { BT_HCI_SET_ESCO_PKT_LEN,             6,  BT_CMD_HC_SET_ESCO_PKT_LEN,          TRUE }
#endif /* FEATURE_BT_1_2 */
#else
#ifdef FEATURE_BT_SOC1
#error code not present
#endif /* FEATURE_BT_SOC1 */

/* opcode (BT_HCI_VS_GROUP) is just a place holder for this entry.  */
/* Special handling in bt_hc_pe_process_opcode masks of the OCF     */
/* bits before using the opcode to index into the table to get the  */
/* corresponding bt_cmd                                             */
 ,{ BT_HCI_VS_GROUP,             0xffff,  BT_CMD_HC_VS_ANY,               TRUE}

#endif /* !FEATURE_BT_SOC */
};

// WAL - in the future, should use sizeof the specific struct
bt_hc_ev_data_type  bt_hc_ev_data_table[] =
{
  { BT_EV_HC_RX_ACL_DATA,                 BT_HCI_MAX_EV,                                  0 },
  { BT_EV_HC_RX_SCO_DATA,                 BT_HCI_MAX_EV,                                  0 },
  { BT_EV_HC_INQUIRY_COMPLETE,            BT_HCI_INQUIRY_COMPLETE_EV,                     1 },
  { BT_EV_HC_INQUIRY_RESULT,              BT_HCI_INQUIRY_RESULT_EV,                       0 },
  { BT_EV_HC_CONNECTION_COMPLETE,         BT_HCI_CONNECTION_COMPLETE_EV,                 11 },
  { BT_EV_HC_CONNECTION_REQUEST,          BT_HCI_CONNECTION_REQUEST_EV,                  10 },
  { BT_EV_HC_DISCONNECTION_COMPLETE,      BT_HCI_DISCONNECTION_COMPLETE_EV,               4 },
  { BT_EV_HC_AUTH_COMPLETE,               BT_HCI_AUTHENTICATION_COMPLETE_EV,              3 },
  { BT_EV_HC_REMOTE_NAME_REQ_COMPLETE,    BT_HCI_REMOTE_NAME_RQST_COMPLETE_EV,            0 },
  { BT_EV_HC_ENCRYPTION_CHANGE,           BT_HCI_ENCRYPTION_CHANGE_EV,                    4 },
  { BT_EV_HC_CHANGE_CONN_KEY_COMPLETE,    BT_HCI_CHANGE_CONNECTION_LINK_KEY_COMPLETE_EV,  3 },
  { BT_EV_HC_MASTER_LINK_KEY_COMPLETE,    BT_HCI_MASTER_LINK_KEY_COMPLETE_EV,             4 },
  { BT_EV_HC_RD_REMOTE_FEATURES_COMPLETE, BT_HCI_READ_REMOTE_SUPPORTED_FEAT_COMPLETE_EV, 11 },
  { BT_EV_HC_RD_REMOTE_VERSION_COMPLETE,  BT_HCI_READ_REMOTE_VERSION_INFO_COMPLETE_EV,    8 },
  { BT_EV_HC_QOS_SETUP_COMPLETE,          BT_HCI_QOS_SETUP_COMPLETE_EV,                  21 },
  { BT_EV_HC_COMMAND_COMPLETE,            BT_HCI_COMMAND_COMPLETE_EV,                     0 },
  { BT_EV_HC_COMMAND_STATUS,              BT_HCI_COMMAND_STATUS_EV,                       4 },
  { BT_EV_HC_HARDWARE_ERROR,              BT_HCI_HW_ERROR_EV,                             1 },
  { BT_EV_HC_FLUSH_OCCURRED,              BT_HCI_FLUSH_OCCURRED_EV,                       2 },
  { BT_EV_HC_ROLE_CHANGE,                 BT_HCI_ROLE_CHANGE_EV,                          8 },
  { BT_EV_HC_NUM_CMPLT_PKTS,              BT_HCI_NUM_COMPLETED_PACKETS_EV,                0 },
  { BT_EV_HC_MODE_CHANGE,                 BT_HCI_MODE_CHANGE_EV,                          6 },
  { BT_EV_HC_RETURN_LINK_KEYS,            BT_HCI_RETURN_LINK_KEYS_EV,                     0 },
  { BT_EV_HC_PIN_CODE_REQUEST,            BT_HCI_PIN_CODE_REQUEST_EV,                     6 },
  { BT_EV_HC_LINK_KEY_REQUEST,            BT_HCI_LINK_KEY_REQUEST_EV,                     6 },
  { BT_EV_HC_LINK_KEY_NOTIFICATION,       BT_HCI_LINK_KEY_NOTIFICATION_EV,               23 },
  { BT_EV_HC_LOOPBACK_COMMAND,            BT_HCI_LOOPBACK_COMMAND_EV,                     0 },
  { BT_EV_HC_DATA_BUFFER_OVERFLOW,        BT_HCI_DATA_BUFFER_OVERFLOW_EV,                 1 },
  { BT_EV_HC_MAX_SLOTS_CHANGE,            BT_HCI_MAX_SLOTS_CHANGE_EV,                     3 },
  { BT_EV_HC_RD_CLK_OFFSET_COMPLETE,      BT_HCI_READ_CLOCK_OFFSET_COMPLETE_EV,           5 },
  { BT_EV_HC_CONN_PKT_TYPE_CHANGED,       BT_HCI_CONNECTION_PKT_TYPE_CHANGED_EV,          5 },
  { BT_EV_HC_QOS_VIOLATION,               BT_HCI_QOS_VIOLATION_EV,                        2 },
  { BT_EV_HC_PAGE_SCAN_MODE_CHANGE,       BT_HCI_PAGE_SCAN_MODE_CHANGE_EV,                7 },
  { BT_EV_HC_PAGE_SCAN_REP_MODE_CHANGE,   BT_HCI_PAGE_SCAN_REP_MODE_CHANGE_EV,            7 },
  { BT_EV_HC_VENDOR_SPECIFIC,             BT_HCI_VENDOR_SPECIFIC,                         0 }
#ifdef FEATURE_BT_1_2
 ,{ BT_EV_HC_INQUIRY_RESULT_WITH_RSSI,    BT_HCI_INQUIRY_RESULT_WITH_RSSI_EV,             0 },
  { BT_EV_HC_RD_REMOTE_EXT_FEATURES_COMPLETE, BT_HCI_READ_REMOTE_EXT_FEAT_COMPLETE_EV,   13 },
  { BT_EV_HC_SYNC_CONN_COMPLETE,              BT_HCI_SYNC_CONNNECTION_COMPLETE_EV,       17 },
  { BT_EV_HC_SYNC_CONN_CHANGE,                BT_HCI_SYNC_CONNECTION_CHANGE_EV,           9 }
#endif /* FEATURE_BT_1_2 */
#ifdef FEATURE_BT_2_1
 ,{ BT_EV_HC_SNIFF_SUBRATING,                 BT_HCI_SNIFF_SUBRATING_EV,                 11 },
  { BT_EV_HC_INQUIRY_RESULT_EXTENDED,         BT_HCI_INQUIRY_RESULT_EXTENDED_EV,          0 },
  { BT_EV_HC_ENCRYPTION_KEY_REFRESH_COMPLETE, BT_HCI_ENCRYPTION_KEY_REFRESH_COMPLETE_EV,  3 },
  { BT_EV_HC_IO_CAPABILITY_REQUEST,             BT_HCI_IO_CAPABILITY_REQUEST_EV,          6 },
  { BT_EV_HC_IO_CAPABILITY_RESPONSE,            BT_HCI_IO_CAPABILITY_RESPONSE_EV,         9 },
  { BT_EV_HC_USER_CONFIRMATION_REQUEST,         BT_HCI_USER_CONFIRMATION_REQUEST_EV,     10 },
  { BT_EV_HC_USER_PASSKEY_REQUEST,              BT_HCI_USER_PASSKEY_REQUEST_EV,           6 },
  { BT_EV_HC_REMOTE_OOB_DATA_REQUEST,           BT_HCI_REMOTE_OOB_DATA_REQUEST_EV,        6 },
  { BT_EV_HC_SIMPLE_PAIRING_COMPLETE,           BT_HCI_SIMPLE_PAIRING_COMPLETE_EV,        7 },
  { BT_EV_HC_LINK_SUPERVISION_TIMEOUT_CHANGED,BT_HCI_LINK_SUPERVISION_TIMEOUT_CHANGED_EV, 4 },
  { BT_EV_HC_ENH_FLUSH_COMPLETE,                BT_HCI_ENH_FLUSH_COMPLETE_EV,             2 },
  { BT_EV_HC_USER_PASSKEY_NOTIFICATION,         BT_HCI_USER_PASSKEY_NOTIFICATION_EV,     10 },
  { BT_EV_HC_KEYPRESS_NOTIFICATION,             BT_HCI_KEYPRESS_NOTIFICATION_EV,          7 },
  { BT_EV_HC_REMOTE_HOST_SUPPORTED_FEAT_NTFCTN,BT_HCI_REMOTE_HOST_SUPPORTED_FEAT_NTFCTN_EV, 14 }
#endif /* FEATURE_BT_2_1 */

};


// Parse engine's variables
LOCAL bt_hc_parser_info_type bt_hc_pe_info;

// Event Msg to send
LOCAL bt_hc_pe_ev_pkt_type  bt_hc_pe_ev_pkt;

// Data header to send
LOCAL bt_hc_pe_data_hdr_type  bt_hc_pe_data_hdr;

/*-------------------------------------------------------------------------*/
/*                                                                         */
/*                     Static Function Prototypes                          */
/*                                                                         */
/*-------------------------------------------------------------------------*/

LOCAL boolean bt_hc_pe_process_pkt_type (byte);
LOCAL boolean bt_hc_pe_process_cmd_pkt (byte);
LOCAL boolean bt_hc_pe_process_data_pkt (byte);

LOCAL boolean bt_hc_pe_process_opcode (byte, bt_hc_pe_cmd_pkt_sub_state_type*);
LOCAL boolean bt_hc_pe_process_params_len (byte, bt_hc_pe_cmd_pkt_sub_state_type*);
LOCAL boolean bt_hc_pe_process_conn_hndl (byte, bt_hc_data_pkt_sub_state_type*);
LOCAL boolean bt_hc_pe_process_data_len (byte, bt_hc_data_pkt_sub_state_type*);
LOCAL void bt_hc_pe_queue_cmd( void );

/*===========================================================================

                          FUNCTION DEFINITIONS

===========================================================================*/

/*===========================================================================

FUNCTION
  bt_hc_pe_get_max_event_bytes

DESCRIPTION
  Returns the number of bytes in the largest event expected to be
  processed by the HCI parser module.

===========================================================================*/
LOCAL uint16 bt_hc_pe_get_max_event_bytes
(
  void
)
{

  uint16  max_eb;

#if 0  // BT_TBD:  Could be optimized as below?
  uint16  i;

  max_eb = sizeof( bt_ev_gn_cmd_done_type );
  for ( i = 0; i < ARR_SIZE( bt_hc_ev_data_table ); i++ )
  {
    max_eb = MAX( max_eb, bt_hc_ev_data_table[ i ].hci_len;
  }
  
  max_eb += sizeof( bt_ev_hdr_type );
#else
  max_eb = sizeof( bt_ev_msg_type );
#endif
  
  return( max_eb ); // BT_TBD

}


/*===========================================================================

FUNCTION
  bt_hc_pe_init()

DESCRIPTION
  Initializes the HCI parser module.
===========================================================================*/
void bt_hc_pe_init( void )
{

  /* Initialize the HCI Parser event queue */
  (void) q_init( &bt_hc_ev_q );
  bt_hc_ev_q_info.event_q_ptr      = &bt_hc_ev_q;
  bt_hc_ev_q_info.max_q_depth      = BT_HC_NUM_EVENTS;
  bt_hc_ev_q_info.event_q_bit_mask = BT_EVQ_HC_B;
  bt_hc_ev_q_info.max_event_bytes  = bt_hc_pe_get_max_event_bytes();

  bt_hc_pe_info.state = BT_HC_PE_TYPE_CHECK_STATE;

  /* Initialize the Event msg data struct */
  bt_hc_pe_ev_pkt.type_indicator = BT_HCI_EVENT_PKT;

  bt_hc_pe_sync_errors = 0;

#ifdef FEATURE_BT_TEST_API
#error code not present
#endif

}


/*===========================================================================

FUNCTION
  bt_hc_pe_qdsp_reset_shutdown

DESCRIPTION
  De-initialize the HCI Parser when the Bluetooth QDSP image is unloaded

===========================================================================*/
void bt_hc_pe_qdsp_reset_shutdown( void )
{
  // TBD - IMAGE_SWAP
}

/*===========================================================================

FUNCTION
  bt_hc_pe_get_byte

DESCRIPTION
  Gets a byte from the UART or the SIO driver, depending on platform.

===========================================================================*/
LOCAL boolean           /* returns true if a byte was read */
bt_hc_pe_get_byte
(
  uint8*  rcvd_byte     /* the received byte */
)
{
  static   dsm_item_type*  dsm_ptr = NULL;
  uint8    bytes_read;

  if ( dsm_ptr == NULL )
  {
    /* current dsm_item has no more bytes.  read in the next queued
    ** dsm_item from the watermark queue.
    */
    dsm_ptr = dsm_dequeue( &bt_from_sio_wm );
  }

  /* read in one byte.  the dsm func takes care of deallocating dsm_items
  ** and if dsm_item passed in is NULL.
  */
  bytes_read = dsm_pullup( &dsm_ptr, rcvd_byte, 1 );

  return ( ( bytes_read == 1 ) ? TRUE : FALSE );
}

/*===========================================================================

FUNCTION
  bt_hc_pe_parse_host_msg

DESCRIPTION
  This function is called to read and parse data received from the UART.
  It does so repeatedly until there's no data available from the UART.

===========================================================================*/
void bt_hc_pe_parse_host_msg( void )
{
  byte byte_read = 0;
  boolean success;

  while ( bt_hc_pe_get_byte( &byte_read ) )
  {
    switch (bt_hc_pe_info.state)
    {
      case BT_HC_PE_TYPE_CHECK_STATE:  // no packet type received yet
        {
          success = bt_hc_pe_process_pkt_type( byte_read );
          if ( !success ) bt_hc_pe_sync_errors++;
          break;
        }

      case BT_HC_PE_PROCESS_COMMAND_STATE:
        {
          bt_hc_pe_process_cmd_pkt( byte_read );
          break;
        }

      case BT_HC_PE_PROCESS_DATA_STATE:
        {
          bt_hc_pe_process_data_pkt( byte_read );
          break;
        }

#ifdef FEATURE_BT_TEST_API
#error code not present
#endif
      default:
        {
          BT_ERR( "BT HC PE: Bad state %x",
                  bt_hc_pe_info.state, 0, 0 );
          break;
        }
    }
  }
}

/*===========================================================================

FUNCTION
  bt_hc_pe_process_pkt_type

DESCRIPTION
  This function validates the packet type and changes the state
  accordingly

===========================================================================*/
LOCAL boolean bt_hc_pe_process_pkt_type
(
  byte  packet_type
)
{

  boolean  valid_packet_type = TRUE;

  bt_hc_pe_info.pkt_type = (bt_hci_pkt_type)(packet_type);

  switch ( packet_type )
  {
    case BT_HCI_CMD_PKT:
      {
        bt_hc_pe_info.opcode = 0;          // re-init opcode
        bt_hc_pe_info.data_params_len = 0; // re-init length
        bt_hc_pe_info.state = BT_HC_PE_PROCESS_COMMAND_STATE;
        break;
      }

    case BT_HCI_ACL_DATA_PKT:
    case BT_HCI_SCO_DATA_PKT:
      {
        bt_hc_pe_info.data_params_len = 0; // re-init length
        bt_hc_pe_info.state = BT_HC_PE_PROCESS_DATA_STATE;
        break;
      }

#ifdef FEATURE_BT_TEST_API
#error code not present
#endif

    default:
      {
        // invalid packet type received; discard this byte and stay in this
        // state until a valid packet type is received
        valid_packet_type = FALSE;  // indicate failure
        break;
      }
  }

  return( valid_packet_type );

}


/*===========================================================================

FUNCTION
  bt_hc_pe_process_cmd_pkt

DESCRIPTION
  This function handles the received bytes and build a command out of them.

===========================================================================*/
LOCAL boolean bt_hc_pe_process_cmd_pkt (byte byte_read)
{
  boolean success = TRUE; // assume success
  static bt_hc_pe_cmd_pkt_sub_state_type sub_state = BT_HC_PE_CMD_OPCODE_STATE;

  switch (sub_state)
  {
    case BT_HC_PE_CMD_OPCODE_STATE:
      success = bt_hc_pe_process_opcode (byte_read, &sub_state);
      break;

    case BT_HC_PE_CMD_PARAMS_LEN_STATE:
      success = bt_hc_pe_process_params_len (byte_read, &sub_state);
      break;

    case BT_HC_PE_CMD_PARAMS_STATE:
      *bt_hc_pe_info.cmd_param_ptr = byte_read;
      bt_hc_pe_info.cmd_param_ptr++; // point to next byte in buffer

      if ( ++bt_hc_pe_info.data_param_bytes_received ==
           bt_hc_pe_info.data_params_len )
      {
        bt_hc_pe_queue_cmd();

        // done with this packet
        bt_hc_pe_info.state = BT_HC_PE_TYPE_CHECK_STATE;
        sub_state = BT_HC_PE_CMD_OPCODE_STATE;
      }
      break;
  }

  if (!success)
  {
    bt_hc_pe_info.state = BT_HC_PE_TYPE_CHECK_STATE;  // re-sync
    sub_state = BT_HC_PE_CMD_OPCODE_STATE;
  }

  return(success);

}  // end of bt_hc_pe_process_cmd_pkt

/*===========================================================================

FUNCTION
  bt_hc_pe_process_data_pkt

DESCRIPTION

===========================================================================*/
LOCAL boolean bt_hc_pe_process_data_pkt( byte byte_read )
{
  static bt_hc_data_pkt_sub_state_type
           sub_state = BT_HC_PE_DATA_CONN_HNDL_STATE;

  boolean success = TRUE; // assume success

  switch ( sub_state )
  {
    case BT_HC_PE_DATA_CONN_HNDL_STATE:
      success = bt_hc_pe_process_conn_hndl( byte_read, &sub_state );
      break;

    case BT_HC_PE_DATA_LEN_STATE:
      success = bt_hc_pe_process_data_len( byte_read, &sub_state );
      break;

    case BT_HC_PE_DATA_STATE:
      dsm_pushdown_tail( &bt_hc_pe_info.dsm_data_param_ptr,
                         ((void*)(&byte_read)), sizeof( byte ),
                         DSM_DS_SMALL_ITEM_POOL );
      bt_hc_pe_info.data_param_bytes_received++;
      break;
  }

  if ( success )
  {
    if ( ( sub_state == BT_HC_PE_DATA_STATE ) &&
         ( bt_hc_pe_info.data_param_bytes_received ==
           bt_hc_pe_info.data_params_len ) )
    {
      /* send BT HCI TX data command */
      if ( bt_hc_pe_info.pkt_type == BT_HCI_ACL_DATA_PKT )
      {
        bt_cmd_hc_tx_acl_data( bt_hc_pe_info.connection_hndl,
                               bt_hc_pe_info.pkt_boundary_flag,
                               bt_hc_pe_info.broadcast_flag,
                               bt_hc_pe_info.dsm_data_param_ptr,
                               BT_L2_NULL_CID,
                               NULL);
      }
      else
      {
        bt_cmd_hc_tx_sco_data( bt_hc_pe_info.connection_hndl,
                               bt_hc_pe_info.dsm_data_param_ptr );
      }

      // done with this packet
      bt_hc_pe_info.state = BT_HC_PE_TYPE_CHECK_STATE;
      sub_state = BT_HC_PE_DATA_CONN_HNDL_STATE;
    }
  }
  else
  {
    bt_hc_pe_info.state = BT_HC_PE_TYPE_CHECK_STATE;  // re-sync
    sub_state = BT_HC_PE_DATA_CONN_HNDL_STATE;
  }

  return( success );

}  // end of bt_hc_pe_process_data_pkt

/*===========================================================================

FUNCTION
  bt_hc_pe_process_opcode

DESCRIPTION
  This function builds the opcode, validates it, and changes the parse
  engine's state accordingly.

===========================================================================*/
LOCAL boolean
bt_hc_pe_process_opcode
(
  byte opcode_byte,
  bt_hc_pe_cmd_pkt_sub_state_type *sub_state_ptr
)
{
  boolean opcode_valid = TRUE;  // assume success

  static byte num_opcode_bytes_read = 0;
  short num_entries = sizeof (bt_hc_pe_func_info_table) / sizeof ( bt_hc_cmd_func_info_type);

  // The lower-order byte comes before the higher-order byte.
  // It's necessary to shift the 2nd byte 8 bit to the left
  // and OR it with the 1st byte to build the 2-byte opcode.
  bt_hc_pe_info.opcode |= ((word) opcode_byte) << (8 * num_opcode_bytes_read);

  if (++num_opcode_bytes_read == 2) // both bytes of the opcode received?
  {
    word opcode = bt_hc_pe_info.opcode;
    num_opcode_bytes_read = 0;  // re-init counter

    bt_hc_pe_info.index = 0;  // initialize table index

    if ( (opcode & BT_HCI_VS_GROUP) == BT_HCI_VS_GROUP ) 
    {
      opcode = BT_HCI_VS_GROUP;
    }
    // search the table for the given opcode
    while (bt_hc_pe_info.index < num_entries)
    {
      if (bt_hc_pe_func_info_table[bt_hc_pe_info.index].opcode == opcode )
      {
        // found it!  remember the parameter length so I can compare it with
        // the length field which is the next to be received
        bt_hc_pe_info.data_params_len =
          bt_hc_pe_func_info_table[bt_hc_pe_info.index].parameter_length;

        bt_hc_pe_info.bt_cmd =
          bt_hc_pe_func_info_table[ bt_hc_pe_info.index ].bt_cmd;

        if (bt_hc_pe_info.data_params_len != 0)
        {
          bt_hc_pe_info.cmd_param_ptr = bt_hc_pe_info.cmd_params;
        }
        *sub_state_ptr = BT_HC_PE_CMD_PARAMS_LEN_STATE;

        break;
      }
      bt_hc_pe_info.index++;  // go to next entry
    } // end while

    if (bt_hc_pe_info.index == num_entries) // opcode not found in table?
    {
      bt_ev_msg_type  ev_hcs_msg;

      opcode_valid = FALSE; // indicate failure

      /* Send Command Status Events */
      ev_hcs_msg.ev_hdr.ev_type = BT_EV_HC_COMMAND_STATUS;
      ev_hcs_msg.ev_msg.ev_hc_cmdst.status = BT_BE_UNKNOWN_HCI_CMD;
#ifndef FEATURE_BT_SOC
      ev_hcs_msg.ev_msg.ev_hc_cmdst.num_hci_cmd_pkts =
                                  bt_lm_data.additional_cmd_pkts;
#endif /* FEATURE_BT_SOC */
      ev_hcs_msg.ev_msg.ev_hc_cmdst.cmd_opcode =
                                  bt_hc_pe_info.opcode;
      bt_hc_send_event( &ev_hcs_msg );
    }

  } // end if (++num_opcode_bytes_read == 2)

  return(opcode_valid);

}

/*===========================================================================

FUNCTION
  bt_hc_pe_process_params_len

DESCRIPTION
  The input parameter indicates the number of parameter bytes yet to be
  received.

===========================================================================*/
LOCAL boolean
bt_hc_pe_process_params_len
(
  byte len,
  bt_hc_pe_cmd_pkt_sub_state_type *sub_state_ptr
)
{
  boolean len_matched = TRUE;  // assume success

  bt_hc_pe_info.data_param_bytes_received = 0;

  if (bt_hc_pe_info.data_params_len == (word) len)  // as expected?
  {
    if (len == 0) // no parameters?
    {
      bt_hc_pe_queue_cmd();
      bt_hc_pe_info.state = BT_HC_PE_TYPE_CHECK_STATE; // done with this packet
      *sub_state_ptr = BT_HC_PE_CMD_OPCODE_STATE;
    }
    else
    {  // there're parameters to be received
      *sub_state_ptr = BT_HC_PE_CMD_PARAMS_STATE;
    }
  }
  else if (bt_hc_pe_info.data_params_len == 0xffff)  // special case?
  {
    bt_hc_pe_info.data_params_len = (word)len; // save the actually length

    // the bt_hc_set_event_filter command has between 1 and 9 parameter bytes
    if ((bt_hc_pe_info.opcode == BT_HCI_SET_EVENT_FILTER) &&
        ((len >= 1) && (len <= 9)))
    {
      *sub_state_ptr = BT_HC_PE_CMD_PARAMS_STATE;
    }

    // the bt_hc_write_stored_link_key has 1+multiple of 22 bytes of parameters
    else if ((bt_hc_pe_info.opcode == BT_HCI_WR_STORED_LINK_KEY) &&
             ((len >= 23) && ((len-1)% 22 == 0)))
    {
      *sub_state_ptr = BT_HC_PE_CMD_PARAMS_STATE;
    }
    // this packet has 1 + multiple of 4 bytes of parameters
    else if ( ( bt_hc_pe_info.opcode == BT_HCI_HOST_NUM_COMPLETED_PACKETS ) &&
              ( ( len >= 5 ) && ( ( len - 1 ) % 4 == 0 ) ) )
    {
      *sub_state_ptr = BT_HC_PE_CMD_PARAMS_STATE;
    }

    // this packets has 1 + multiple of 3 bytes of parameters
    else if ( ( bt_hc_pe_info.opcode == BT_HCI_WR_CURRENT_IAC_LAP ) &&
              ( ( len >= 4 ) && ( ( len - 1 ) % 3 == 0 ) ) )
    {
      *sub_state_ptr = BT_HC_PE_CMD_PARAMS_STATE;
    }

#ifndef FEATURE_BT_SOC
    else if ( ( bt_hc_pe_info.opcode == BT_HCI_SEND_RAW_MODE_PACKET ) &&
              ( len > 8 ) )
    {
      *sub_state_ptr = BT_HC_PE_CMD_PARAMS_STATE;
    }

#ifdef FEATURE_BT_TEST_MODE_TESTER
    else if ( ( bt_hc_pe_info.opcode == BT_HCI_TESTER_CONTROL ) &&
              ( len > 12 ) )
    {
      *sub_state_ptr = BT_HC_PE_CMD_PARAMS_STATE;
    }
#endif /* FEATURE_BT_TEST_MODE_TESTER */
#endif /* !FEATURE_BT_SOC */
#ifdef FEATURE_BT_SOC1
#error code not present
#endif

#ifdef FEATURE_BT_QSOC
    else if ( (bt_hc_pe_info.opcode & 0xFC00 ) == BT_HCI_QSOC_VS_GROUP)
    {
      *sub_state_ptr = BT_HC_PE_CMD_PARAMS_STATE;
    }
#endif /* FEATURE_BT_QSOC */

#ifdef FEATURE_BT_2_1
    else if (bt_hc_pe_info.opcode == BT_HCI_WR_EXT_INQ_RSP)
    {
      MSG_HIGH( "BT HC PE: Received WR EXT INQ RSP", 0, 0, 0 );
      *sub_state_ptr = BT_HC_PE_CMD_PARAMS_STATE;
    }
#endif /* FEATURE_BT_2_1 */

    else
    {  // all special cases are handled above; so, this must be an error
      len_matched = FALSE;            // indicate error
      BT_ERR( "BT HC PE: Bad HCI cmd: %x",
              bt_hc_pe_info.opcode, 0, 0 );
    }
  }
  else
  {  // obviously an error
    len_matched = FALSE;              // indicate error
  }

  if ( !len_matched )
  {
    byte status = BT_BE_INVALID_HCI_CMD_PARAMS;

    // generate appropriate event
    if (bt_hc_pe_func_info_table[bt_hc_pe_info.index].cmd_cmplt_ev_expected)
    {
      bt_ev_msg_type  ev_hcc_msg;

      /* Send Command Complete Event */
      ev_hcc_msg.ev_hdr.ev_type = BT_EV_HC_COMMAND_COMPLETE;
#ifndef FEATURE_BT_SOC
      ev_hcc_msg.ev_msg.ev_hc_cmd_cmplt.num_hci_cmd_pkts =
                                    bt_lm_data.additional_cmd_pkts;
#endif /* FEATURE_BT_SOC */
      ev_hcc_msg.ev_msg.ev_hc_cmd_cmplt.cmd_opcode =
                                    bt_hc_pe_info.opcode;
      ev_hcc_msg.ev_msg.ev_hc_cmd_cmplt.ret_params.status.status = status;
      bt_hc_send_event( &ev_hcc_msg );
    }
    else
    {
      bt_ev_msg_type  ev_hcs_msg;

      /* Send Command Status Event */
      ev_hcs_msg.ev_hdr.ev_type = BT_EV_HC_COMMAND_STATUS;
      ev_hcs_msg.ev_msg.ev_hc_cmdst.status = status;
#ifndef FEATURE_BT_SOC
      ev_hcs_msg.ev_msg.ev_hc_cmdst.num_hci_cmd_pkts =
                                  bt_lm_data.additional_cmd_pkts;
#endif /* FEATURE_BT_SOC */
      ev_hcs_msg.ev_msg.ev_hc_cmdst.cmd_opcode =
                                  bt_hc_pe_info.opcode;
      bt_hc_send_event( &ev_hcs_msg );
    }
  }
  return(len_matched);

} // end of bt_hc_pe_process_params_len

/*===========================================================================

FUNCTION
  bt_hc_pe_process_conn_hndl

DESCRIPTION
  This function extracts the following from the input parameter byte(s):
    connection handle
    packet boundary flag for acl data packets
    broadcast flag for acl data packets

===========================================================================*/
LOCAL boolean bt_hc_pe_process_conn_hndl
(
  byte                             ch_byte,
  bt_hc_data_pkt_sub_state_type*   sub_state_ptr
)
{
  boolean success = TRUE; // assume success
  uint8   shift; 
  uint16  mask;

  static byte num_ch_bytes_read = 0;

  // The lower-order byte comes before the higher-order byte.
  // It's necessary to shift the 2nd byte 8 bit to the left and
  // OR it with the 1st byte to build the 2-byte connection handle

  shift = ( 8 * num_ch_bytes_read );
  mask  = 0x00FF << shift;

  bt_hc_pe_info.connection_hndl = 
    ( bt_hc_pe_info.connection_hndl & ~mask ) |
    ( ( (uint16) ch_byte << shift ) & mask );

  if ( ++num_ch_bytes_read == 2 ) // both bytes of connection handle received?
  {
    num_ch_bytes_read = 0;  // reset counter

    bt_hc_pe_info.connection_hndl &= 0x0fff;  // remove the 2 2-bit flags from it

    if ( bt_hc_pe_info.pkt_type == BT_HCI_ACL_DATA_PKT )
    {
      // extract the 2-bit packet boundary flag
      bt_hc_pkt_boundary_flag_type pkt_boundary_flag =
        (bt_hc_pkt_boundary_flag_type) ( ( ch_byte >> 4 ) & 0x03 );

      // extract the 2-bit broadcast flag
      bt_hc_broadcast_flag_type broadcast_flag =
        (bt_hc_broadcast_flag_type) ( ( ch_byte >> 6 ) & 0x03 );

      switch ( pkt_boundary_flag )
      {
        case BT_HC_PKT_BOUNDARY_FRAGMENT:
        case BT_HC_PKT_BOUNDARY_START:
#ifdef FEATURE_BT_2_1
        case BT_HC_PKT_BOUNDARY_START_FLUSHABLE:
#endif /* FEATURE_BT_2_1 */
          bt_hc_pe_info.pkt_boundary_flag = pkt_boundary_flag;
          break;

        default:
          success = FALSE;  // indicate error
          break;
      }

      switch ( broadcast_flag )
      {
        case BT_HC_BROADCAST_NONE:
        case BT_HC_BROADCAST_ACTIVE:
        case BT_HC_BROADCAST_PICONET:
          bt_hc_pe_info.broadcast_flag = broadcast_flag;
          break;

        default:
          success = FALSE;  // indicate error
          break;
      }
    }
  }

  if ( success && ( num_ch_bytes_read == 0 ) )
  {
    *sub_state_ptr = BT_HC_PE_DATA_LEN_STATE;
  }

  return(success);

} // end of bt_hc_pe_process_conn_hndl

/*===========================================================================

FUNCTION
  bt_hc_pe_process_data_len

DESCRIPTION
  This function builds the data length from the input parameter(s)
  and verifies that it's within range.

===========================================================================*/
LOCAL boolean
bt_hc_pe_process_data_len
(
  byte len_byte,
  bt_hc_data_pkt_sub_state_type *sub_state_ptr
)
{
  boolean success = TRUE;  // assume success
  static byte num_len_bytes_read = 0; // the length is 2-byte long for ACL

  if ( bt_hc_pe_info.pkt_type == BT_HCI_ACL_DATA_PKT )
  {
    bt_hc_pe_info.data_params_len |=
      ( (word) len_byte ) << ( 8 * num_len_bytes_read );

    if (++num_len_bytes_read == 2) // both bytes of the length field received?
    {
      num_len_bytes_read = 0; // re-init counter

      if ( bt_hc_pe_info.data_params_len > BT_HC_MAX_DATA_LEN )
      {
        success = FALSE;
      }
    }
  }
  else // SCO data
  {
    bt_hc_pe_info.data_params_len = (word) len_byte;
  }

  if ( success && ( ( bt_hc_pe_info.pkt_type == BT_HCI_SCO_DATA_PKT ) ||
                    ( num_len_bytes_read == 0 ) ) )
  {
    /* We are about to parse a HCI data packet (i.e. receive data bytes).
    ** We need to allocate a dsm buffer, and set up the pointers.
    */
    bt_hc_pe_info.dsm_data_param_ptr =
      bt_get_free_dsm_ptr( BT_TL_L2CAP,
                           bt_hc_pe_info.data_params_len );

    *sub_state_ptr = BT_HC_PE_DATA_STATE;
    bt_hc_pe_info.data_param_bytes_received = 0;
  }

  return(success);

} // end of bt_hc_pe_process_data_len

/*===========================================================================

FUNCTION
  bt_hc_pe_is_hci_rst_started

DESCRIPTION
  Returns TRUE if HCI Reset has started through a HCI RST comand issued
  by a test host in HCI APIT Test mode, else returns False.

===========================================================================*/
boolean bt_hc_pe_is_hci_rst_started( void )
{
  return bt_hc_pe_hci_rst_state;
} // end of bt_hc_pe_is_hci_rst_started

/*===========================================================================

FUNCTION
  bt_hc_pe_hci_rst_done

DESCRIPTION
  Resets the HCI RST state flag to indicate that HCI RST procedure has finished
  in HCI APIT Test mode.

===========================================================================*/
void bt_hc_pe_hci_rst_done( void )
{
  bt_hc_pe_hci_rst_state = FALSE;
} // end of bt_hc_pe_hci_rst_done

/*===========================================================================

FUNCTION
  bt_hc_pe_queue_cmd

DESCRIPTION
  Creates a BT HC command of the current HCI command we are parsing,
  and queues it to BT.  For variable sized commands, allocates the dsm_ptr
  that the BT cmd requires.

===========================================================================*/
LOCAL void bt_hc_pe_queue_cmd()
{
  dsm_item_type*    dsm_ptr;
  uint8             dsm_length;

  switch( bt_hc_pe_info.bt_cmd )
  {
    /* The special cases below need to allocate a dsm ptr and copy
    ** the HCI parameters into the dsm data struct.
    */
    case BT_CMD_HC_WR_STORED_LINK_KEY:
    {
      dsm_length = bt_hc_pe_info.data_params_len - 1;

      dsm_ptr = bt_get_free_dsm_ptr( BT_TL_HCI_BB, dsm_length );
      bt_dsm_pushdown_tail( &dsm_ptr, bt_hc_pe_info.cmd_params + 1, dsm_length,
                            DSM_DS_SMALL_ITEM_POOL );

      bt_cmd_hc_wr_stored_link_key(
        bt_hc_pe_info.cmd_params[ 0 ],  /*  number of keys to write  */
        dsm_ptr );
      break;
    }
    case BT_CMD_HC_CHANGE_LOCAL_NAME:
    {
      // get the length of the string
      for ( dsm_length = 0; dsm_length < BT_HCI_MAX_NAME_LEN; dsm_length++ )
      {
        if ( bt_hc_pe_info.cmd_params[ dsm_length ] == 0x00 )
        {
          dsm_length++; /* increment dsm_length for the null-string */
          break;
        }
      }

      // make sure name is null-terminated
      if ( dsm_length == BT_HCI_MAX_NAME_LEN )
      {
        bt_hc_pe_info.cmd_params[ BT_HCI_MAX_NAME_LEN - 1 ] = 0x00;
      }

      dsm_ptr = bt_get_free_dsm_ptr( BT_TL_HCI_BB, dsm_length );
      bt_dsm_pushdown_tail( &dsm_ptr, bt_hc_pe_info.cmd_params, dsm_length,
                            DSM_DS_SMALL_ITEM_POOL );

      bt_cmd_hc_change_local_name( dsm_ptr );
      break;
    }
    case BT_CMD_HC_HOST_NUM_CMPLT_PACKETS:
    {
      dsm_length = bt_hc_pe_info.data_params_len - 1;
      dsm_ptr = bt_get_free_dsm_ptr( BT_TL_HCI_BB, dsm_length );
      bt_dsm_pushdown_tail( &dsm_ptr, bt_hc_pe_info.cmd_params + 1, dsm_length,
                            DSM_DS_SMALL_ITEM_POOL );

      bt_cmd_hc_host_num_cmplt_pkt(
        bt_hc_pe_info.cmd_params[ 0 ],  /*  Number of handles.  */
        dsm_ptr );
      break;
    }
    case BT_CMD_HC_WR_CURRENT_IAC_LAP:
    {
      dsm_length = bt_hc_pe_info.data_params_len - 1;
      dsm_ptr = bt_get_free_dsm_ptr( BT_TL_HCI_BB, dsm_length );
      bt_dsm_pushdown_tail( &dsm_ptr, bt_hc_pe_info.cmd_params + 1, dsm_length,
                            DSM_DS_SMALL_ITEM_POOL );

      bt_cmd_hc_wr_cur_iac_lap(
        bt_hc_pe_info.cmd_params[ 0 ],  /*  Number of current IAC LAPs  */
        dsm_ptr );
      break;
    }
    case BT_CMD_HC_SEND_RAW_MODE_PKT:
    {
      uint16   data_len;

      dsm_length = bt_hc_pe_info.data_params_len -
                     BT_HC_RAW_MODE_NON_DSM_SIZE;
      dsm_ptr = bt_get_free_dsm_ptr( BT_TL_HCI_BB, dsm_length );
      bt_dsm_pushdown_tail( &dsm_ptr,
                            bt_hc_pe_info.cmd_params + BT_HC_RAW_MODE_NON_DSM_SIZE,
                            dsm_length,
                            DSM_DS_SMALL_ITEM_POOL );

      data_len = ( bt_hc_pe_info.cmd_params[ 5 ] ) |
                 ( bt_hc_pe_info.cmd_params[ 6 ] << 8 );

      bt_cmd_hc_send_raw_mode_pkt(
        *((uint16*) &bt_hc_pe_info.cmd_params[ 0 ]), /*  Connection handle. */
        (uint8)  bt_hc_pe_info.cmd_params[ 2 ],  /*  Start of data to send  */
        (uint8)  bt_hc_pe_info.cmd_params[ 3 ],  /*  Last of data to send   */
        (uint8)  bt_hc_pe_info.cmd_params[ 4 ],  /*  num_slots              */
        data_len,                                /*  data_byte_len          */
        dsm_ptr );                               /*  The data to transmit   */
      break;
    }

#ifdef FEATURE_BT_TEST_MODE_TESTER
    case BT_CMD_HC_TESTER_CONTROL:
    {
      uint16  payload_len;

      dsm_length = bt_hc_pe_info.data_params_len -
                     BT_HC_TESTER_CTRL_NON_DSM_SIZE;
      dsm_ptr = bt_get_free_dsm_ptr( BT_TL_HCI_BB, dsm_length );
      bt_dsm_pushdown_tail( &dsm_ptr,
                            bt_hc_pe_info.cmd_params + BT_HC_TESTER_CTRL_NON_DSM_SIZE,
                            dsm_length,
                            DSM_DS_SMALL_ITEM_POOL );

      payload_len = ( bt_hc_pe_info.cmd_params[ 9 ] ) |
                    ( bt_hc_pe_info.cmd_params[ 10 ] << 8 );

      bt_cmd_hc_tester_control(
        *((uint16*) &bt_hc_pe_info.cmd_params[ 0 ]), /*  Connection handle.  */
        (uint8)  bt_hc_pe_info.cmd_params[ 2 ],   /*  test_senario           */
        (uint8)  bt_hc_pe_info.cmd_params[ 3 ],   /*  hopping_mode           */
        (uint8)  bt_hc_pe_info.cmd_params[ 4 ],   /*  dut_tx_freq_k          */
        (uint8)  bt_hc_pe_info.cmd_params[ 5 ],   /*  dut_rx_freq_k          */
        (uint8)  bt_hc_pe_info.cmd_params[ 6 ],   /*  power_control_mode     */
        (uint8)  bt_hc_pe_info.cmd_params[ 7 ],   /*  poll_period (1.25 ms)  */
        (uint8)  bt_hc_pe_info.cmd_params[ 8 ],   /*  packet_type            */
        payload_len,                              /*  payload_len            */
        (uint8)  bt_hc_pe_info.cmd_params[ 11 ],  /*  start = 0; end = 1     */
        dsm_ptr );
      break;
    }
#endif /* FEATURE_BT_TEST_MODE_TESTER */

#ifdef FEATURE_BT_SOC1
#error code not present
#endif

  case BT_CMD_HC_VS_ANY:
  {
     BT_MSG_DEBUG( "BT HC PE: Sending VS cmd to HC driver OC %x", 
                    bt_hc_pe_info.opcode, 0, 0 );

    /* Copy the cmd_params portion (payload w/o opcode) into the dsm item. */
     dsm_ptr = bt_get_free_dsm_ptr( BT_TL_HCI_BB, bt_hc_pe_info.data_params_len );
     bt_dsm_pushdown_tail( &dsm_ptr,
                           bt_hc_pe_info.cmd_params,
                           bt_hc_pe_info.data_params_len,
                           DSM_DS_SMALL_ITEM_POOL );

     bt_cmd_hc_vs_any( dsm_ptr, bt_hc_pe_info.opcode );
     break;
  } 

#ifdef FEATURE_BT_2_1
    case BT_CMD_HC_WR_EXT_INQ_RSP:
    {
      MSG_HIGH( "BT HC PE: Sending command WR EXT INQ RSP to HC driver", 0, 0, 0 );
      dsm_length = bt_hc_pe_info.data_params_len - 1;

      dsm_ptr = bt_get_free_dsm_ptr( BT_TL_HCI_BB, dsm_length );
      bt_dsm_pushdown_tail( &dsm_ptr, bt_hc_pe_info.cmd_params + 1, dsm_length,
                            DSM_DS_SMALL_ITEM_POOL );

      bt_cmd_hc_wr_ext_inq_rsp(
        dsm_ptr,
        bt_hc_pe_info.cmd_params[ 0 ] ); /* FEC required */
      break;
    }
#endif /* FEATURE_BT_2_1 */

    case BT_CMD_HC_RESET:
#ifdef FEATURE_BT_QSOC
    if (  !BT_IN_HCI_MODE() )
    {
      /* In HCI APIT mode ,on BTS we handle HCI RST commands by    */
      /* doing a Radio OFF & Radio ON. The Radio ON\OFF procedure  */          
      /* sends out a  HCI RST cmd.                                 */          
      bt_cmd_msg_type      radio_on_off_cmd;        

      /* Switch off the radio */
      radio_on_off_cmd.cmd_hdr.bt_app_id = BT_APP_ID_NULL;
      radio_on_off_cmd.cmd_hdr.cmd_type  = BT_CMD_RM_DISABLE_RADIO;

      radio_on_off_cmd.cmd_msg.cmd_rm_dsrad.disable = TRUE;
      bt_rm_disable_radio( &radio_on_off_cmd );
        
      /* Flag that radio on\off procedure in HCI Mode has started */
      bt_hc_pe_hci_rst_state = TRUE;
      break;
    }
#endif /* FEATURE_BT_QSOC */      
   
    default:
      /*  Most HCI commands have parameters that are byte  */
      /*  identical to BT commands... send it directly.    */
      bt_cmd_hc_send_raw_hc_cmd( bt_hc_pe_info.bt_cmd,
                                 bt_hc_pe_info.cmd_params,
                                 bt_hc_pe_info.data_params_len );
      break;
  } // end switch
}

/*===========================================================================

FUNCTION
  bt_hc_pe_send_data()

DESCRIPTION
  Sends the current HCI data to the UART.
===========================================================================*/
LOCAL void bt_hc_pe_send_data
(
  dsm_item_type*  dsm_data_ptr,
  uint8*          conn_hndl
)
{
  dsm_item_type*  dsm_data_hdr_ptr;

  /* Fill out the data header */
  bt_hc_pe_data_hdr.pkt_type  = BT_HCI_ACL_DATA_PKT;
  bt_hc_pe_data_hdr.conn_hndl = ( conn_hndl[ 1 ] << 8 ) | conn_hndl[ 0 ];
  bt_hc_pe_data_hdr.len_field = dsm_length_packet( dsm_data_ptr );

  /* Create a dsm buffer for the header and fill it up. */
  dsm_data_hdr_ptr = bt_get_free_dsm_ptr( BT_TL_HCI_BB,
                                          BT_HC_PE_DATA_HDR_LEN );
  bt_dsm_pushdown_tail( &dsm_data_hdr_ptr,
                        (void*)&bt_hc_pe_data_hdr,
                        BT_HC_PE_DATA_HDR_LEN,
                        DSM_DS_SMALL_ITEM_POOL );

  /* Append data dsm on to event header dsm item */
  if ( dsm_data_ptr != NULL ) 
  {
    dsm_append( &dsm_data_hdr_ptr, &dsm_data_ptr );
  }

#ifdef FEATURE_FACTORY_TESTMODE
  if (( bt_dc.cur_hci_mode == BT_HCIM_STANDARD_HCI_FTM ) ||
      ( bt_dc.cur_hci_mode == BT_HCIM_STANDARD_APIT_FTM ))
  {
    /* Enqueue event header and data on the "hijacked" wm */
    dsm_enqueue( &bt_to_sio_wm, &dsm_data_hdr_ptr );
  }
  else
#endif /* FEATURE_FACTORY_TESTMODE */
  {
    sio_transmit( bt_dc.sio_stream_id, dsm_data_hdr_ptr );
  }
}

/*===========================================================================

FUNCTION
  bt_hc_pe_send_ev_pkt_to_host()

DESCRIPTION
  Sends the current HCI event message to the UART.
===========================================================================*/
#define BT_HC_PE_EV_BYTE_LEN ( bt_hc_pe_ev_pkt.param_len + 3 )

LOCAL void bt_hc_pe_send_ev_pkt_to_host( void )
{
  dsm_item_type*  dsm_ptr;

  dsm_ptr = bt_get_free_dsm_ptr( BT_TL_HCI_BB, BT_HC_PE_EV_BYTE_LEN );
  bt_dsm_pushdown_tail( &dsm_ptr,
                        &bt_hc_pe_ev_pkt,
                        BT_HC_PE_EV_BYTE_LEN,
                        DSM_DS_SMALL_ITEM_POOL );
#ifdef FEATURE_FACTORY_TESTMODE
  if (( bt_dc.cur_hci_mode == BT_HCIM_STANDARD_HCI_FTM )||
      ( bt_dc.cur_hci_mode == BT_HCIM_STANDARD_APIT_FTM))
  {
    dsm_enqueue( &bt_to_sio_wm, &dsm_ptr );
  }
  else
  {
#endif
    sio_transmit( bt_dc.sio_stream_id, dsm_ptr );
#ifdef FEATURE_FACTORY_TESTMODE
  }
#endif
}

/*===========================================================================

FUNCTION
  bt_hc_pe_process_ev_cmd_cmplt()

DESCRIPTION
  Processes the BT HCI command complete event.
===========================================================================*/
LOCAL void bt_hc_pe_process_ev_cmd_cmplt
(
  bt_ev_msg_type*  ev_msg_ptr
)
{
  bt_ev_hc_cmd_cmplt_type*  cmplt_msg_ptr =
                              &ev_msg_ptr->ev_msg.ev_hc_cmd_cmplt;
  boolean                   variable_pkt  = FALSE;

#ifdef FEATURE_BT_QSOC
  if (cmplt_msg_ptr->cmd_opcode != BT_HCI_RESET)
  {
    if (bt_dc.driver_state != BT_DS_SOC_ACTIVE)
    {
      BT_MSG_HIGH( "bt_hc_pe_process_ev_cmd_cmplt: Skip event: cmd opcode = 0x%x",
                   cmplt_msg_ptr->cmd_opcode, 0, 0 );
      /* Don't send any events if BT driver state is not active. */
      return;
    }
  }
#endif /* FEATURE_BT_QSOC */

  /* Set the size of the command complete header */
  bt_hc_pe_ev_pkt.param_len = sizeof( uint8 ) + sizeof( uint16 );

  // WAL place this data in a table later on, and search table.
  switch ( cmplt_msg_ptr->cmd_opcode )
  {
    case BT_HCI_RESET:
#ifdef FEATURE_BT_QSOC
      /* If BTS' initialization is not complete, re-direct HCI_RESET 
       * command complete event to it. This is required for successful
       * BTS init
       */
      if( BT_QSOC_IS_READY() == FALSE )
      {
        if (bt_soc_ifc.bt_soc_proc_hci_reset_func_ptr != NULL)
        {
          bt_soc_ifc.bt_soc_proc_hci_reset_func_ptr(ev_msg_ptr);

          /* Don't send reset event to host. */
          return;

        }
        break;
      }
      /* Otherwise fall through */
#endif /* FEATURE_BT_QSOC */
    case BT_HCI_INQUIRY_CANCEL:
    case BT_HCI_PERIODIC_INQUIRY_MODE:
    case BT_HCI_EXIT_PERIODIC_INQUIRY_MODE:
    case BT_HCI_SET_EVENT_MASK:
    case BT_HCI_SET_EVENT_FILTER:
    case BT_HCI_WR_PIN_TYPE:
    case BT_HCI_CREATE_NEW_UNIT_KEY:
    case BT_HCI_CHANGE_LOCAL_NAME:
    case BT_HCI_WR_CONN_ACCEPT_TIMEOUT:
    case BT_HCI_WR_PAGE_TIMEOUT:
    case BT_HCI_WR_SCAN_ENABLE:
    case BT_HCI_WR_PAGESCAN_ACTIVITY:
    case BT_HCI_WR_INQUIRYSCAN_ACTIVITY:
    case BT_HCI_WR_AUTHENTICATION_ENABLE:
    case BT_HCI_WR_ENCRYPTION_MODE:
    case BT_HCI_WR_CLASS_OF_DEVICE:
    case BT_HCI_WR_VOICE_SETTING:
    case BT_HCI_WR_NUM_BC_RETRANSMISSIONS:
    case BT_HCI_WR_HOLD_MODE_ACTIVITY:
    case BT_HCI_WR_SCO_FLOW_CONTROL_ENABLE:
    case BT_HCI_SET_HC_TO_HOST_FLOW_CONTROL:
    case BT_HCI_HOST_BUFFER_SIZE:
    case BT_HCI_WR_CURRENT_IAC_LAP:
    case BT_HCI_WR_PAGE_SCAN_PERIOD_MODE:
#ifndef FEATURE_BT_1_2 /* BT 1.1 */
    case BT_HCI_WR_PAGE_SCAN_MODE:
#endif
    case BT_HCI_WR_LOOPBACK_MODE:
    case BT_HCI_ENABLE_DUT_MODE:
#ifndef FEATURE_BT_SOC
/* Qualcomm vendor specific Commands */
    case BT_HCI_FIX_INIT_WHITE:
    case BT_HCI_FIX_INIT_ENC:
    case BT_HCI_READ_RX_STATUS_ENABLE:
    case BT_HCI_CONFIG_SEARCHER:
    case BT_HCI_READ_TX_BUFFER_ENABLE:
    case BT_HCI_READ_RX_BUFFER_ENABLE:
    case BT_HCI_SEND_RAW_MODE_PACKET:
#ifdef FEATURE_BT_TEST_MODE_TESTER
    case BT_HCI_TESTER_ACTIVATE:
    case BT_HCI_TESTER_CONTROL:
    case BT_HCI_TESTER_START:
#endif /* FEATURE_BT_TEST_MODE_TESTER */
    case BT_HCI_SET_NEW_BD_ADDR:
    case BT_HCI_MODIFY_BEACON:
    case BT_HCI_WR_SBI_REG:
    case BT_HCI_DIAC_LAP_EN:
    case BT_HCI_WR_ENC_KEY_LENGTH:
    case BT_HCI_CHANGE_REMOTE_PKT_TYPE:
    case BT_HCI_STATUS_VIEW_EN:
    case BT_HCI_ENABLE_LOOPBACK:
    case BT_HCI_SET_INCOMING_HOLD_PARAMS:
    case BT_HCI_SET_INCOMING_SNIFF_PARAMS:
    case BT_HCI_SET_INCOMING_PARK_PARAMS:
    case BT_HCI_SET_TX_POWER_LEVEL:
    case BT_HCI_CERT_CTRL:
    case BT_HCI_TX_CONTINUOUS:
    case BT_HCI_TX_CONTINUOUS_FREQ:

    case BT_HCI_TEST_SET_LM_PARAMS:
    case BT_HCI_TEST_SEND_LMP_MSG:
    case BT_HCI_WR_RAM:
#ifdef FEATURE_BT_1_2
    case BT_HCI_CONTROL_AFH:
    case BT_HCI_SET_AFH_CHAN_GOOD:
    case BT_HCI_SET_ESCO_PKT_LEN:
#endif /* FEATURE_BT_1_2 */
#endif /* !FEATURE_BT_SOC */
#ifdef FEATURE_BT_1_2
    case BT_HCI_SET_AFH_HOST_CHAN_CLASS:  
    case BT_HCI_SET_AFH_CHAN_ASSESS_MODE:
    case BT_HCI_WR_INQUIRY_SCAN_TYPE:
    case BT_HCI_WR_INQUIRY_MODE:
    case BT_HCI_WR_PAGE_SCAN_TYPE:
#endif /* FEATURE_BT_1_2 */
#ifdef FEATURE_BT_2_1
    case BT_HCI_WR_EXT_INQ_RSP:
    case BT_HCI_WR_SIMPLE_PAIRING_MODE:
    case BT_HCI_WR_DEFAULT_ERR_DATA_REPORTING:
    case BT_HCI_WR_INQ_TX_PWR:
    case BT_HCI_SEND_KEYPRESS_NOTIFICATION:
    case BT_HCI_WR_SIMPLE_PAIRING_DEBUG_MODE:
#endif /* FEATURE_BT_2_1 */
      bt_hc_pe_ev_pkt.param_len += sizeof( bt_hc_rp_status_type );
      break;
    case BT_HCI_LINK_KEY_RQST_REPLY:
    case BT_HCI_LINK_KEY_RQST_NEG_REPLY:
    case BT_HCI_PIN_CODE_RQST_REPLY:
    case BT_HCI_PIN_CODE_RQST_NEG_REPLY:
    case BT_HCI_RD_BD_ADDR:
#ifdef FEATURE_BT_2_1
    case BT_HCI_IO_CAPABILITY_RQST_REPLY:
    case BT_HCI_USER_CFMTN_RQST_REPLY:
    case BT_HCI_USER_CFMTN_RQST_NEG_REPLY:
    case BT_HCI_USER_PASSKEY_RQST_REPLY:
    case BT_HCI_USER_PASSKEY_RQST_NEG_REPLY:
    case BT_HCI_REMOTE_OOB_DATA_RQST_REPLY:
    case BT_HCI_REMOTE_OOB_DATA_RQST_NEG_REPLY:
    case BT_HCI_IO_CAPABILITY_RQST_NEG_REPLY:
#endif /* FEATURE_BT_2_1 */
      bt_hc_pe_ev_pkt.param_len += sizeof( bt_hc_rp_bd_addr_type );
      break;
    case BT_HCI_WR_LINK_POLICY_SETTINGS:
    case BT_HCI_FLUSH:
    case BT_HCI_WR_AUTO_FLUSH_TIMEOUT:
    case BT_HCI_WR_LINK_SUPERVISION_TIMEOUT:
    case BT_HCI_RESET_FAILED_CONTACT_COUNTER:
#ifdef FEATURE_BT_2_1
    case BT_HCI_SNIFF_SUBRATING:
#endif /* FEATURE_BT_2_1 */
      bt_hc_pe_ev_pkt.param_len += sizeof( bt_hc_rp_conn_hndl_type );
      break;
    case BT_HCI_ROLE_DISCOVERY:
      bt_hc_pe_ev_pkt.param_len += sizeof( bt_hc_rp_role_discovery_type );
      break;
    case BT_HCI_RD_LINK_POLICY_SETTINGS:
      bt_hc_pe_ev_pkt.param_len += sizeof( bt_hc_rp_rd_link_policy_type );
      break;
    case BT_HCI_RD_PIN_TYPE:
      bt_hc_pe_ev_pkt.param_len += sizeof( bt_hc_rp_rd_pin_type );
      break;
    case BT_HCI_RD_STORED_LINK_KEY:
      bt_hc_pe_ev_pkt.param_len += sizeof( bt_hc_rp_rd_link_key_type );
      break;
    case BT_HCI_WR_STORED_LINK_KEY:
      bt_hc_pe_ev_pkt.param_len += sizeof( bt_hc_rp_wr_link_key_type );
      break;
    case BT_HCI_DELETE_STORED_LINK_KEY:
      bt_hc_pe_ev_pkt.param_len += sizeof( bt_hc_rp_del_link_key_type );
      break;
    case BT_HCI_RD_LOCAL_NAME:
    {
      uint8                         name_len;
      dsm_item_type*                dsm_ptr;
      bt_hc_rp_rd_local_name_type*  name_msg_ptr;

      name_msg_ptr = &cmplt_msg_ptr->ret_params.rd_local_name;
      dsm_ptr      = name_msg_ptr->dsm_ptr;

      /* Copy the command complete header */
      memcpy( bt_hc_pe_ev_pkt.params,
              ( (uint8*) cmplt_msg_ptr ) + sizeof( bt_ev_hdr_type ),
              bt_hc_pe_ev_pkt.param_len );

      /* Copy the status */
      bt_hc_pe_ev_pkt.params[ bt_hc_pe_ev_pkt.param_len++ ] =
        name_msg_ptr->status;

      /* Copy the name */
      name_len = dsm_length_packet( dsm_ptr );
      dsm_pullup( &dsm_ptr,
                  &bt_hc_pe_ev_pkt.params[ bt_hc_pe_ev_pkt.param_len ],
                  name_len );
      dsm_free_packet( &dsm_ptr );

      bt_hc_pe_ev_pkt.param_len += name_len;

      variable_pkt = TRUE;  /* Mark this event as a variable msg */
      break;
    }
    case BT_HCI_RD_CONN_ACCEPT_TIMEOUT:
      bt_hc_pe_ev_pkt.param_len += sizeof( bt_hc_rp_rd_conn_accept_to_type );
      break;
    case BT_HCI_RD_PAGE_TIMEOUT:
      bt_hc_pe_ev_pkt.param_len += sizeof( bt_hc_rp_rd_page_to_type );
      break;
    case BT_HCI_RD_SCAN_ENABLE:
      bt_hc_pe_ev_pkt.param_len += sizeof( bt_hc_rp_rd_scan_enable_type );
      break;
    case BT_HCI_RD_PAGESCAN_ACTIVITY:
      bt_hc_pe_ev_pkt.param_len += sizeof( bt_hc_rp_rd_page_scan_act_type );
      break;
    case BT_HCI_RD_INQUIRYSCAN_ACTIVITY:
      bt_hc_pe_ev_pkt.param_len += sizeof( bt_hc_rp_rd_inq_scan_act_type );
      break;
    case BT_HCI_RD_AUTHENTICATION_ENABLE:
      bt_hc_pe_ev_pkt.param_len += sizeof( bt_hc_rp_rd_auth_enable_type );
      break;
    case BT_HCI_RD_ENCRYPTION_MODE:
      bt_hc_pe_ev_pkt.param_len += sizeof( bt_hc_rp_rd_encrypt_mode_type );
      break;
    case BT_HCI_RD_CLASS_OF_DEVICE:
      bt_hc_pe_ev_pkt.param_len += sizeof( bt_hc_rp_rd_class_of_dev_type );
      break;
    case BT_HCI_RD_VOICE_SETTING:
      bt_hc_pe_ev_pkt.param_len += sizeof( bt_hc_rp_rd_voice_setting_type );
      break;
    case BT_HCI_RD_AUTO_FLUSH_TIMEOUT:
      bt_hc_pe_ev_pkt.param_len += sizeof( bt_hc_rp_rd_auto_flush_to_type );
      break;
    case BT_HCI_RD_NUM_BC_RETRANSMISSIONS:
      bt_hc_pe_ev_pkt.param_len += sizeof( bt_hc_rp_rd_broad_retrans_type );
      break;
    case BT_HCI_RD_HOLD_MODE_ACTIVITY:
      bt_hc_pe_ev_pkt.param_len += sizeof( bt_hc_rp_rd_hold_mode_act_type );
      break;
    case BT_HCI_RD_TRANSMIT_POWER_LEVEL:
      bt_hc_pe_ev_pkt.param_len += sizeof( bt_hc_rp_rd_tx_power_type );
      break;
    case BT_HCI_RD_SCO_FLOW_CONTROL_ENABLE:
      bt_hc_pe_ev_pkt.param_len += sizeof( bt_hc_rp_rd_sco_flow_ctrl_type );
      break;
    case BT_HCI_RD_LINK_SUPERVISION_TIMEOUT:
      bt_hc_pe_ev_pkt.param_len += sizeof( bt_hc_rp_rd_link_super_to_type );
      break;
    case BT_HCI_RD_NUMBER_OF_SUPPORTED_IAC:
      bt_hc_pe_ev_pkt.param_len += sizeof( bt_hc_rp_rd_num_iac_type );
      break;
    case BT_HCI_RD_CURRENT_IAC_LAP:
    {
      uint8                           iac_count;
      dsm_item_type*                  dsm_ptr;
      bt_hc_rp_rd_curr_iac_lap_type*  lap_msg_ptr;

      lap_msg_ptr = &cmplt_msg_ptr->ret_params.rd_curr_iac_lap;
      dsm_ptr     = lap_msg_ptr->dsm_ptr;
      iac_count   = lap_msg_ptr->num_current_iac;

      /* Copy the command complete header */
      memcpy( bt_hc_pe_ev_pkt.params,
              ( (uint8*) cmplt_msg_ptr ) + sizeof( bt_ev_hdr_type ),
              bt_hc_pe_ev_pkt.param_len );

      /* Copy the status and num_of_iacs */
      bt_hc_pe_ev_pkt.params[ bt_hc_pe_ev_pkt.param_len++ ] =
        lap_msg_ptr->status;
      bt_hc_pe_ev_pkt.params[ bt_hc_pe_ev_pkt.param_len++ ] = iac_count;

      /* Copy the iac_lap */
      dsm_pullup( &dsm_ptr,
                  &bt_hc_pe_ev_pkt.params[ bt_hc_pe_ev_pkt.param_len ],
                  iac_count * sizeof( bt_lap_type ) );
      dsm_free_packet( &dsm_ptr );

      bt_hc_pe_ev_pkt.param_len += iac_count * sizeof( bt_lap_type );

      variable_pkt = TRUE;  /* Mark this event as a variable msg */
      break;
    }
    case BT_HCI_RD_PAGE_SCAN_PERIOD_MODE:
      bt_hc_pe_ev_pkt.param_len += sizeof( bt_hc_rp_rd_ps_per_mode_type );
      break;
#ifndef FEATURE_BT_1_2 /* BT 1.1 */
    case BT_HCI_RD_PAGE_SCAN_MODE:
      bt_hc_pe_ev_pkt.param_len += sizeof( bt_hc_rp_rd_ps_mode_type );
      break;
#endif
    case BT_HCI_RD_LOCAL_VERSION_INFO:
      bt_hc_pe_ev_pkt.param_len += sizeof( bt_hc_rp_rd_local_ver_type );
      break;
    case BT_HCI_RD_LOCAL_SUPPORTED_FEAT:
      bt_hc_pe_ev_pkt.param_len += sizeof( bt_hc_rp_rd_local_feat_type );
      break;
    case BT_HCI_RD_LOCAL_SUPPORTED_CMDS:
      bt_hc_pe_ev_pkt.param_len += sizeof( bt_hc_rp_rd_local_cmds_type );
      break;
    case BT_HCI_RD_LOCAL_EXTENDED_FEATURES:
      bt_hc_pe_ev_pkt.param_len += sizeof( bt_hc_rp_rd_rd_local_ext_feat_type );
      break;
    case BT_HCI_RD_BUFFER_SIZE:
      bt_hc_pe_ev_pkt.param_len += sizeof( bt_hc_rp_rd_buffer_size_type );
      break;
    case BT_HCI_RD_COUNTRY_CODE:
      bt_hc_pe_ev_pkt.param_len += sizeof( bt_hc_rp_rd_country_code_type );
      break;
    case BT_HCI_RD_FAILED_CONTACT_COUNTER:
      bt_hc_pe_ev_pkt.param_len += sizeof( bt_hc_rp_rd_failed_contact_type );
      break;
    case BT_HCI_GET_LINK_QUALITY:
      bt_hc_pe_ev_pkt.param_len += sizeof( bt_hc_rp_get_link_quality_type );
      break;
    case BT_HCI_RD_RSSI:
      bt_hc_pe_ev_pkt.param_len += sizeof( bt_hc_rp_rd_rssi_type );
      break;
    case BT_HCI_RD_LOOPBACK_MODE:
      bt_hc_pe_ev_pkt.param_len += sizeof( bt_hc_rp_rd_loopback_mode_type );
      break;
#ifdef FEATURE_BT_1_2
    case BT_HCI_READ_AFH_CHAN_MAP:
      bt_hc_pe_ev_pkt.param_len += sizeof( bt_hc_rp_rd_afh_chan_map_type );
      break;
    case BT_HCI_READ_AFH_CHAN_ASSESS_MODE:
      bt_hc_pe_ev_pkt.param_len += sizeof( bt_hc_rp_rd_afh_chan_assess_mode_type );
      break;
    case BT_HCI_RD_INQUIRY_SCAN_TYPE:
      bt_hc_pe_ev_pkt.param_len += sizeof( bt_hc_rp_rd_inq_scan_type_type );
      break;
    case BT_HCI_RD_INQUIRY_MODE:
      bt_hc_pe_ev_pkt.param_len += sizeof( bt_hc_rp_rd_inq_mode_type );
      break;
    case BT_HCI_RD_PAGE_SCAN_TYPE:
      bt_hc_pe_ev_pkt.param_len += sizeof( bt_hc_rp_rd_page_scan_type_type );
      break;
#endif /* FEATURE_BT_1_2 */
#ifdef FEATURE_BT_2_1
    case BT_HCI_RD_EXT_INQ_RSP:
      bt_hc_pe_ev_pkt.param_len += sizeof( bt_hc_rp_rd_ext_inq_rsp_type );
      break;
    case BT_HCI_RD_SIMPLE_PAIRING_MODE:
      bt_hc_pe_ev_pkt.param_len += sizeof( bt_hc_rp_sp_mode_type );
      break;
    case BT_HCI_RD_LOCAL_OOB_DATA:
      bt_hc_pe_ev_pkt.param_len += sizeof( bt_hc_rp_oob_data_type );
      break;
    case BT_HCI_RD_DEFAULT_ERR_DATA_REPORTING:
      bt_hc_pe_ev_pkt.param_len += 
                sizeof( bt_hc_rp_rd_default_err_data_reporting_type );
      break;
    case BT_HCI_RD_INQ_RSP_TX_PWR:
      bt_hc_pe_ev_pkt.param_len += sizeof( bt_hc_rp_rd_inq_rsp_tx_pwr );
      break;
#endif /* FEATURE_BT_2_1 */
    case BT_HCI_NONE:
        break;
    default:
      BT_ERR( "BT HC PE: Bad Cmd Cmp Ev opcode: %x",
              cmplt_msg_ptr->cmd_opcode, 0, 0 );
  }

  if ( !variable_pkt )
  {
    /* Variable packets cases have already copied all the bytes */
    memcpy( bt_hc_pe_ev_pkt.params,
            (uint8*) cmplt_msg_ptr,
            bt_hc_pe_ev_pkt.param_len );
  }

  bt_hc_pe_send_ev_pkt_to_host();
}

/*===========================================================================

FUNCTION
  bt_hc_process_event()

DESCRIPTION
  Processes the BT event sent to the HCI layer.
===========================================================================*/
LOCAL void bt_hc_process_event
(
  bt_ev_msg_type*  ev_msg_ptr
)
{
  bt_event_type   bt_ev = ev_msg_ptr->ev_hdr.ev_type;
  uint8           ev_idx;

  if ( bt_ev == BT_EV_GN_CMD_DONE )
  {
    /*  Don't send Command Done events out the UART.  */
    return;
  }
#ifndef FEATURE_BT_SOC
  else if ( (bt_ev == BT_EV_HC_QUALCOMM) &&
            ( (ev_msg_ptr->ev_msg.ev_hc_qc.qc_event == 
               BT_EV_HC_QC_REM_AUTH_OR_PAIR_FAILED) ||
              (ev_msg_ptr->ev_msg.ev_hc_qc.qc_event == 
               BT_EV_HC_QC_RADIO_ACTIVITY_CHANGED) ||
              (ev_msg_ptr->ev_msg.ev_hc_qc.qc_event == 
               BT_EV_HC_QC_CUR_CONN_ENCRYPT) ) )
  {
    /*  Don't send these events out the UART.  */
    return;
  }
#endif /* !FEATURE_BT_SOC */
  else
  {
    /* Search table for event code and length */
    for ( ev_idx = 0; ev_idx < ARR_SIZE( bt_hc_ev_data_table ); ev_idx++ )
    {
      if ( bt_hc_ev_data_table[ ev_idx ].bt_ev == bt_ev )
      {
        bt_hc_pe_ev_pkt.event_code = bt_hc_ev_data_table[ ev_idx ].hci_ev;
        bt_hc_pe_ev_pkt.param_len = bt_hc_ev_data_table[ ev_idx ].hci_len;
        break;
      }
    }
  }
  if( ev_idx == ARR_SIZE( bt_hc_ev_data_table ) )
  {
    BT_ERR( "BT HC PE: Bad HCI EV Type %x", bt_ev, 0, 0 );
    return;
  }

  switch ( bt_ev )
  {
    case BT_EV_HC_RX_ACL_DATA:
    {
      byte                        conn_hndl_buf[ 2 ];
      bt_ev_hc_rx_acl_data_type*  acl_msg_ptr =
                                    &ev_msg_ptr->ev_msg.ev_hc_rxda;

      bt_w2ba( acl_msg_ptr->conn_hndl, conn_hndl_buf );
      conn_hndl_buf[1] |= ( acl_msg_ptr->pkt_boundary_flag << 4 );
      if ( acl_msg_ptr->broadcast_flag == BT_HC_BROADCAST_ACTIVE )
      {
        conn_hndl_buf[1] |= 0x40 ;
      }
      else if ( acl_msg_ptr->broadcast_flag == BT_HC_BROADCAST_PICONET )
      {
        conn_hndl_buf[1] |= 0x80 ;
      }
      {
        dsm_item_type*  dsm_ptr = acl_msg_ptr->dsm_ptr;
        bt_hc_pe_send_data( dsm_ptr, conn_hndl_buf );
      }
      break;
    }
    case BT_EV_HC_RX_SCO_DATA:
    {
      BT_ERR( "BT HC PE: SCO data not implemented yet", 0, 0, 0 );
      break;
    }
    case BT_EV_HC_INQUIRY_RESULT:
    {
      bt_ev_hc_inquiry_result_type*  inq_msg_ptr =
        &ev_msg_ptr->ev_msg.ev_hc_inqrs;
      uint8  num_rsp = inq_msg_ptr->num_responses;
      dsm_item_type*  dsm_ptr = inq_msg_ptr->dsm_ptr;

      bt_hc_pe_ev_pkt.params[ 0 ] = num_rsp;
      bt_hc_pe_ev_pkt.param_len = 1 + ( num_rsp *
                                        BT_EV_HC_INQUIRY_RESULT_ELEM_SIZE );
      dsm_pullup( &dsm_ptr,
                  &bt_hc_pe_ev_pkt.params[ 1 ],
                  bt_hc_pe_ev_pkt.param_len - 1 );
      dsm_free_packet( &dsm_ptr );

      bt_hc_pe_send_ev_pkt_to_host();
      break;
    }
	#ifdef FEATURE_BT_2_1
    case BT_EV_HC_INQUIRY_RESULT_WITH_RSSI:
    {
		
      bt_ev_hc_inquiry_result_with_rssi_type*  inq_msg_ptr =
        &ev_msg_ptr->ev_msg.ev_hc_inqrs_wrssi;
	
      uint8  num_rsp = inq_msg_ptr->num_responses;
      dsm_item_type*  dsm_ptr = inq_msg_ptr->dsm_ptr;

      bt_hc_pe_ev_pkt.params[ 0 ] = num_rsp;
      bt_hc_pe_ev_pkt.param_len = 1 + ( num_rsp *
                                        BT_EV_HC_INQUIRY_RESULT_WITH_RSSI_ELEM_SIZE );
      dsm_pullup( &dsm_ptr,
                  &bt_hc_pe_ev_pkt.params[ 1 ],
                  bt_hc_pe_ev_pkt.param_len - 1 );
      dsm_free_packet( &dsm_ptr );

      bt_hc_pe_send_ev_pkt_to_host();
      break;
    }

    case BT_EV_HC_INQUIRY_RESULT_EXTENDED:
    {
      bt_ev_hc_inquiry_result_extended_type*  inq_msg_ptr =
        &ev_msg_ptr->ev_msg.ev_hc_inqrs_extended;
      uint8  num_rsp = inq_msg_ptr->num_responses;
      dsm_item_type*  dsm_ptr = inq_msg_ptr->dsm_ptr;

      bt_hc_pe_ev_pkt.params[ 0 ] = num_rsp;
      bt_hc_pe_ev_pkt.param_len = 1 + ( num_rsp *
                                        BT_EV_HC_INQUIRY_RESULT_EXTENDED_ELEM_SIZE );
      dsm_pullup( &dsm_ptr,
                  &bt_hc_pe_ev_pkt.params[ 1 ],
                  bt_hc_pe_ev_pkt.param_len - 1 );
      dsm_free_packet( &dsm_ptr );

      bt_hc_pe_send_ev_pkt_to_host();
      break;
    }

    case BT_EV_HC_REMOTE_NAME_REQ_COMPLETE:
    {
      dsm_item_type* dsm_ptr;
      uint8  hci_name_hdr_len = sizeof( uint8 ) + sizeof( bt_bd_addr_type );

      bt_ev_hc_remote_name_req_cmplt_type*  name_msg_ptr =
        &ev_msg_ptr->ev_msg.ev_hc_rname;

      /* Copy the status and bd_addr params */
      memcpy( bt_hc_pe_ev_pkt.params,
              (uint8*) name_msg_ptr,
              hci_name_hdr_len );

      /* copy the name params */
      dsm_ptr = name_msg_ptr->dsm_ptr;

      bt_hc_pe_ev_pkt.param_len = hci_name_hdr_len + BT_HCI_MAX_NAME_LEN;

      dsm_pullup( &dsm_ptr,
                  &bt_hc_pe_ev_pkt.params[ hci_name_hdr_len ],
                  dsm_length_packet( dsm_ptr ) );
      dsm_free_packet( &dsm_ptr );

      bt_hc_pe_send_ev_pkt_to_host();
      break;
    }
	#endif /* FEATURE_BT_2_1 */
    case BT_EV_HC_COMMAND_COMPLETE:
    {
      bt_hc_pe_process_ev_cmd_cmplt( ev_msg_ptr );
      break;
    }
	#ifdef FEATURE_BT_2_1
    case BT_EV_HC_NUM_CMPLT_PKTS:
    {
      bt_ev_hc_num_cmplt_pkts_type*  cmplt_msg_ptr =
        &ev_msg_ptr->ev_msg.ev_hc_numpk;
      dsm_item_type*  dsm_ptr = cmplt_msg_ptr->dsm_ptr;
      uint8           num_hndls = cmplt_msg_ptr->num_handles;

      bt_hc_pe_ev_pkt.params[ 0 ] = num_hndls;
      bt_hc_pe_ev_pkt.param_len = 1 + ( num_hndls *
                                        BT_EV_HC_NUM_CMPLT_PKTS_ELEM_SIZE );
      dsm_pullup( &dsm_ptr,
                  &bt_hc_pe_ev_pkt.params[ 1 ],
                  bt_hc_pe_ev_pkt.param_len - 1 );
      dsm_free_packet( &dsm_ptr );

      bt_hc_pe_send_ev_pkt_to_host();
      break;
    }
    case BT_EV_HC_RETURN_LINK_KEYS:
    {
      bt_ev_hc_ret_key_type*  key_msg_ptr =
        &ev_msg_ptr->ev_msg.ev_hc_retky;
      dsm_item_type*  dsm_ptr = key_msg_ptr->dsm_ptr;
      uint8           num_keys = key_msg_ptr->num_keys;

      bt_hc_pe_ev_pkt.params[ 0 ] = num_keys;
      bt_hc_pe_ev_pkt.param_len = 1 + ( num_keys *
                                        BT_EV_HC_RETURN_LINK_KEYS_ELEM_SIZE );
      dsm_pullup( &dsm_ptr,
                  &bt_hc_pe_ev_pkt.params[ 1 ],
                  bt_hc_pe_ev_pkt.param_len - 1 );
      dsm_free_packet( &dsm_ptr );

      bt_hc_pe_send_ev_pkt_to_host();
      break;
    }
	#endif
    case BT_EV_HC_VENDOR_SPECIFIC:
    {
#ifdef FEATURE_BT_SOC
#ifdef FEATURE_BT_QSOC

      BT_MSG_DEBUG( "BT HC PE: VS event", 0, 0, 0 );

      if( BT_QSOC_IS_READY() == FALSE )
      {
        /* Re-direct vendor specific events to SoC code */
        if(bt_soc_ifc.bt_soc_proc_vs_event_func_ptr != NULL)
        {
          bt_soc_ifc.bt_soc_proc_vs_event_func_ptr(ev_msg_ptr);
        }
        else
        {
          BT_ERR( "BT HC PE: NULL Pointer - bt_soc_proc_vs_event_func_ptr", 0, 0, 0 );
        }
      }
      else
      {
        /* Send vendor-specific events to test Host */
        dsm_item_type* dsm_ptr;

        bt_ev_hc_vs_var_type*  qsoc_vs_msg_ptr =
                                      &ev_msg_ptr->ev_msg.ev_hc_vs_var_hdr;

        /* copy vendor specific event content */
        bt_hc_pe_ev_pkt.param_len = 
                  sizeof( bt_ev_hc_vs_var_type ) - sizeof(dsm_item_type*) ;

        memcpy( bt_hc_pe_ev_pkt.params,
                (uint8*) &ev_msg_ptr->ev_msg.ev_hc_vs_var_hdr,
                bt_hc_pe_ev_pkt.param_len );

        dsm_ptr = qsoc_vs_msg_ptr->evt_data;

        bt_hc_pe_ev_pkt.param_len += dsm_pullup( &dsm_ptr,
                                       &bt_hc_pe_ev_pkt.params[ bt_hc_pe_ev_pkt.param_len ],
                                       dsm_length_packet( dsm_ptr ) );
        dsm_free_packet( &dsm_ptr );

        bt_hc_pe_send_ev_pkt_to_host();

      }
      break;
#endif /* FEATURE_BT_QSOC */
#ifdef FEATURE_BT_SOC1
#error code not present
#endif /* FEATURE_BT_SOC1 */
#else     
      /* Set the correct event length and continue normally */
      bt_hc_pe_ev_pkt.param_len = 1;
      switch ( ev_msg_ptr->ev_msg.ev_hc_qc.qc_event )
      {
        case BT_EV_HC_QC_RD_RX_STATUS_RESULT:
          bt_hc_pe_ev_pkt.param_len +=
            sizeof( bt_ev_hc_qc_rd_rx_status_result_type );
          break;
        case BT_EV_HC_QC_RD_TX_BUFFER_RESULT:
          bt_hc_pe_ev_pkt.param_len +=
            sizeof( bt_ev_hc_qc_rd_tx_buffer_result_type );
          break;
        case BT_EV_HC_QC_RD_RX_BUFFER_RESULT:
          bt_hc_pe_ev_pkt.param_len +=
            sizeof( bt_ev_hc_qc_rd_rx_buffer_result_type );
          break;
#ifdef FEATURE_BT_TEST_MODE_TESTER
        case BT_EV_HC_QC_TESTER_ACTIVATE_RESULT:
          bt_hc_pe_ev_pkt.param_len +=
            sizeof( bt_ev_hc_qc_tester_activate_result_type );
          break;
        case BT_EV_HC_QC_TESTER_CONTROL_RESULT:
          bt_hc_pe_ev_pkt.param_len +=
            sizeof( bt_ev_hc_qc_tester_control_result_type );
          break;
#endif /* FEATURE_BT_TEST_MODE_TESTER */
        case BT_EV_HC_QC_TIME_ACCURACY_RESP:
          bt_hc_pe_ev_pkt.param_len +=
            sizeof( bt_ev_hc_qc_time_accuracy_resp_type );
          break;
        case BT_EV_HC_QC_RD_CLK_RESP:
          bt_hc_pe_ev_pkt.param_len +=
            sizeof( bt_ev_hc_qc_rd_clk_resp_type );
          break;
        case BT_EV_HC_QC_RD_SBI_REG_CMPLT:
          bt_hc_pe_ev_pkt.param_len +=
            sizeof( bt_ev_hc_qc_rd_sbi_reg_cmplt_type );
          break;
        case BT_EV_HC_QC_RD_ENC_KEY_LENGTH:
          bt_hc_pe_ev_pkt.param_len +=
            sizeof( bt_ev_hc_qc_rd_enc_key_length_type );
          break;
        case BT_EV_HC_QC_RD_RAW_RSSI:
          bt_hc_pe_ev_pkt.param_len +=
            sizeof( bt_ev_hc_qc_rd_raw_rssi_type );
          break;
        case BT_EV_HC_QC_RD_RAM:
          bt_hc_pe_ev_pkt.param_len +=
            sizeof( bt_ev_hc_qc_rd_ram_type );
          break;
        default:
          BT_ERR( "BT HC PE: Bad qcom event: %x",
               ev_msg_ptr->ev_msg.ev_hc_qc.qc_event, 0, 0 );
          break;
      } /* switch */
 #endif /*  FEATURE_BT_QSOC */
    } /* case */ 
    /* fall through deliberate */

    default:
    {
      /* Normal events.  Copy params; length already filled out */
      memcpy( bt_hc_pe_ev_pkt.params,
              ( (uint8*) ev_msg_ptr ) + sizeof( bt_ev_hdr_type ),
              bt_hc_pe_ev_pkt.param_len );

      bt_hc_pe_send_ev_pkt_to_host();
      break;
    }
  }
}


/*===========================================================================

FUNCTION
  bt_hc_store_bt_event

DESCRIPTION
  Stores a specified BT event on the HCI BT event queue.

===========================================================================*/
void bt_hc_store_bt_event
(
  bt_ev_msg_type*  bt_ev_ptr
)
{

  bt_store_bt_event( bt_ev_ptr, &bt_hc_ev_q_info );

}

/*===========================================================================

FUNCTION
  bt_hc_process_ev_queue

DESCRIPTION
  Processes the HCI event queue.

===========================================================================*/
void bt_hc_process_ev_queue( void )
{

  bt_ev_msg_type*  bt_ev_ptr;
  int              q_count = q_cnt( &bt_hc_ev_q );

  BT_VSTAT_MAX( q_count, HC_MAX_EV_Q_DEPTH );

  BT_MSG_API_PYLD( "HC ev Q - cur_cnt = %x, max_cnt = %x",
                   q_count,
                   bt_vital_stats[BT_VSAI_HC_MAX_EV_Q_DEPTH], 0);

  while ( (bt_ev_ptr = (bt_ev_msg_type *) q_get( &bt_hc_ev_q )) != NULL )
  {
    bt_hc_process_event( bt_ev_ptr );
    q_put( &bt_event_free_q, &bt_ev_ptr->ev_hdr.link );
  }

}

#ifdef BT_SWDEV_RADIO_OFF

/*===========================================================================

FUNCTION
  bt_hc_empty_ev_queue

DESCRIPTION
  Empties the HCI event queue.

===========================================================================*/
void bt_hc_empty_ev_queue( void )
{

  bt_ev_msg_type*  bt_ev_ptr;

  while ( (bt_ev_ptr = (bt_ev_msg_type *) q_get( &bt_hc_ev_q )) != NULL )
  {
    q_put( &bt_event_free_q, &bt_ev_ptr->ev_hdr.link );
  }

}

#endif /* BT_SWDEV_RADIO_OFF */

#endif /* FEATURE_BT */

