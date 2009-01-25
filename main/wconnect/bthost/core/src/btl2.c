/*====*====*====*====*====*====*====*====*====*====*====*====*====*====*====*

            B L U E T O O T H    L 2    M O D U L E

GENERAL DESCRIPTION
  This contains the highest level code for the Bluetooth L2CAP layer.

              Copyright (c) 2000-2010 QUALCOMM Incorporated.               
                      All Rights Reserved.                                  
              Qualcomm Confidential and Proprietary
*====*====*====*====*====*====*====*====*====*====*====*====*====*====*====*/

/*===========================================================================

                        EDIT HISTORY FOR MODULE

  This section contains comments describing changes made to the module.
  Notice that changes are listed in reverse chronological order.

$Header: //source/qcom/qct/wconnect/bthost/core/rel/00.00.26/src/btl2.c#7 $ $DateTime:
 when        who  what, where, why
 ----------  ---  -----------------------------------------------------------
 2010-04-28  gbm  sending authentication failed information to RM in case of the
                  connection rejection because of security block
 2009-07-15  jtl  Set the idle timeout timer after the last remote-initiated L2CAP
                  connection has been disconnected. Also, better dectection of when
                  to disconnect ACLs when there are no L2CAP channels.
 2009-02-21   gs  Added support for cancelling l2cap connection if l2cap
                  disconnect API is called before the connection is up.
 2009-02-03   VK  Sending Dealyed HCI Disconnect in case of security failure. To allow 
                  L2CAP signalling get through at BB level.
 2009-01-15   VK  Fixed compiler warning.
 2009-01-13   VK  Disconnecting ACL connection if security fails.
 2008-10-09   JTL Removing usage of depricated functions.
 2008-09-09   GS  Negotiate MTU even if app specifies BT_L2_DEFAULT_MTU in L2CAP
                  ConfigReq
 2008-08-14   rb  Added handling to keep the background timer tick during
                  power collapse
 2008-08-08   GS  Fixed mis-use of bt_l2_conn_db entry in a few places.
 2008-07-02   GS  Fixed DSM leak in L2CAP info response packet processing.
                  Fixed data length issue in Command Reject handling in L2CAP.
                  Fixed out_flush_to usage in bt_l2_l2ca_config_req()
 2008-06-26   GS  Fixed incorrect identifier(=0) issue in L2CAP signalling packet
 2008-05-12   VK  Printing error log when out of DUP DSMs.

$Log:   O:/src/asw/COMMON/vcs/btl2.c_v  $
 *
 *  #79        01 Nov 2007            AG
 * Merged Lisbon(BT2.1) changes into the mainline
 *
 *  #78        15 May 2007            AP
 * While closing an L2CAP channel, if ACL link is still connected clear 
 * outstanding signaling packets and corresponding timers.
 *
 *  #77        23 Apr 2007            SSK
 * Fix compiler warnings.
 *
 *  #76        17 Apr 2007            AP
 * In function bt_l2_get_config_options,
 * - Added check for Invalid MTU/Flush/QoS length
 *
 *  #75        10 Apr 2007            JTL
 * Added a new state when outgoing connections are waiting for Mode 2 security.
 * This prevents the connection table entry from being re-used for incoming
 * connections before Mode 2 security has been authorized (or rejected).
 *
 *  #74        06 Apr 2007            AP
 * In function bt_l2_get_config_options,
 * - Updated DSM manipulation to prevent stack corruption 
 * - Removed name masking of qos_ptr 
 * 
 *  #73        02 Apr 2007            JN
 * Drop packet when payload size is greater than MTU.
 *
 *  #72        15 Mar 2007            DSN
 * Prevent MAX MTU config response from being sent out on SOC solutions when
 * remote device config req was for a MTU smaller than MAX MTU.
 *
 *  #71        08 Mar 2007            DM
 * Removed unwanted error print when l2_conn_rsp is received.
 *
 *  #70        01 Mar 2007            DSN
 * Use HCI ACL MTP size based on value read from Controller for SoC solutions.
 *
 *  #69        22 Feb 2007            JTL
 * Critical lint fixes.
 *
 *  #68        20 Feb 2007            DSN
 * Increasing the maximum number of events in L2 layer for SOC solutions.
 *
 *  #67        11 Jan 2007            JTL
 * Only clear the proper outstanding request when the RTX timer expires, rather
 * than clearing all of the outstanding requests for that connection.
 *
 *  #66        20 Dec 2006            DSN
 * Throughput optimization for BT2.0+ EDR based SOC solutions.
 *
 *   #65        16 Nov 2006            DM
 * Removed unused function bt_l2_process_tx_data_cnf()
 *
 *   #64        08 Nov 2006            DM
 * Changed name of ACL TX flow interface
 *
 *   #63        03 Nov 2006            DM
 * Changed Update ACL Tx flow interface from conn_hndl to bd_addr
 *
 *   #62        31 Oct  2006           VK
 * Fixing compilation conflict of bt_rm_atzrq_type enum.
 *
 *   #61        26 Oct 2006            VK
 * Passing connection direction to bt_cmd_rm_enforce_security_l2cap_psm 
 * function call.
 *
 *   #60        19 Oct 2006            DSN
 * Update ACL TX flow indication from BB/HC per connection handle.
 *
 *    #59       13 Sep 2006            MP
 * Changes are made to route the L2CAP ack packets properly to
 * corresponding registered Apps
 * 
 *    #58       23 Aug 2006            DSN
 * Fixed compilation errors on SOC solution.
 *
 *
 *    #57       11 Aug 2006            GB
 * Modified to support APIT mode in ASIA
 *  
 *    #56       26 Jul 2006            DM
 * Added for Av-Sync, mechanism to determine flushable and non-flushable packets
 * Added functions to handle transmit completes for l2cap data
 *
 *    #55       18 Jul 2006            AP
 * For outgoing connections, Addd support to enforce mode 2 security before 
 * sending a connection request.
 *
 *    #54       14 Jul 2006            JTL
 * Fixing NULL pointer dereference when rxing echo response when no request
 * was sent.
 *
 *    #53       27 Jun 2006            DSN
 * Added support for HostController SoC solution.
 *
 *    #52       25 Jun 2006            DSN
 * Removed btbnep.h file inclusion since it is no longer used.
 *
 *    #51       31 May 2006            JTL
 * Update RM Mode 2 security to contain CID info.
 *
 *    #50       28 Apr 2006            JTL
 * Fixing minor bugs with debug messages.
 *
 *    #49       26 Apr 2006            JTL
 * Fix #44 to send the disconnect response to remote.
 *
 *    #48       12 Apr 2006            AP/PG
 * Fix for lingering L2CAP connections setup for L2PING
 *
 *    #47       02 Apr 2006            JTL
 * Send pending connect response while awaiting RM authorization.
 * Augmenting ascii messages.
 *
 *    #46       28 Mar 2006            JTL
 * Fixing possible null pointer dereference.
 *
 *    #45       15 Mar 2006            JTL
 * Don't initialize NULL registration entries.
 *
 *    #44       02 Mar 2006            JTL
 * Send CONNECT FAILED rather than DISCONNECTED event in case remote sends
 * disconnect message before we've sent the CONNECTED event.
 * Fix null pointer dereference when initiating a connection for echo.
 *
 *    #43        22 Feb 2006           JTL
 * Added more signalling packets, and don't error fatal if we run out.
 * Try to recover from no signalling packets by disconnecting link.
 * Clear out CDB table entry as soon as possible to free resources.
 *
 *    #42        24 Jan 2006           JTL
 * Always send the MTU we'll use in the config response if it's not the 
 * default value -- not that we currently don't use anything greater than
 * the default MTU.
 *
 *    #41        20 Jan 2006           JTL 
 * Added new disconnect command which doesn't wait for the ACL timeout.
 *
 *    #40        23 Dec 2005           JTL
 * More log info & better error checking in bt_l2_process_conn_rsp_pkt. In 
 * particular, handle the case where the remote sets the CID value to 0 for 
 * our side.
 *
 *    #39        21 Dec 2005           JTL
 * Send out signalling messages if we have an ACL connection to the remote 
 * device, rather than if the particular L2 connection looked up for the 
 * signalling message is valid.
 * Revamp echo requests, and allow echo requests to bring up new ACL 
 * connections.
 *
 *    #38        06 Dec 2005           DSN
 * Fixed RVCT compile errors.
 *
 *    #37        30 Nov 2005           JTL
 * Adding command to retrieve the number of open connections to a particular
 * BD addr.
 * Better options processing.
 * Doesn't clear remaining signalling data after parsing a signalling message.
 *
 *    #36        24 Oct 2005            GS
 * Increasing ACL disconnect timer, BT_L2_ACL_LINK_DISCONNECT_TIME to 3 seconds
 * so at ACL link is not torn down too soon after the last L2 connection is
 * closed.  Improves interoperability with Belkin dongle during OBEX file
 * transfer.
 * Also, replacing tabs with spaces in indentation.
 *
 *    #35        11 Oct 2005           JTL
 * Fixing bug when bringing up multiple L2 connections to a device at once.
 *
 *    #34        06 Sep 2005           JTL
 * Better DSM and memory handling.
 * Better options processing -- but still not great.
 * Fix bug by not copying so much data from old CDBs when bringing up
 * new connections.
 *
 *    #33        11 Aug 2005            DP
 * Improved L2 debug file written upon FRP errors.  Minor code cleanup.
 *
 *    #32        05 Aug 2005            JH
 * Set BT_L2_IDLE_TIMEOUT_MS to 0 to disable idle timer when FEATURE_BT_TEST_API
 * is defined.
 *
 *    #31        13 Jul 2005            DP
 * Fixed RX ACL data corner case.
 *
 *    #30        13 Jul 2005           JTL
 * Fix #28.
 *
 *    #29        12 Jul 2005            JH
 * Disable idle timer when FEATURE_BT_TEST_API is defined.
 *
 *    #28        07 Jul 2005           JTL
 * Fix bug in bt_l2_get_conn_entry_from_cid, where it may return a conn entry
 * which is not in use.
 * Fix bug in bt_l2_ev_rm_l2cap_psm_sec_result, where it would match up
 * a conn entry with the wrong connection.
 *
 *    #27        06 Jul 2005           JTL
 * Fixing minor bugs. Rearranging some code to make code checker happy.
 *
 *    #26        05 Jul 2005           JTL
 * Fix bug in idle timeout: clear timeout to prevent it firing multiple times.
 * Increase idle timeout value to 90s make it bigger than the RTX timer.
 * Added debug code to dump bad frames to EFS. Disabled by default.
 * Fix bug if we receive only L2 header with no data.
 * Changing dsm_pushdown to dsm_pushdown_packed
 * Better freeing of pkt_ptrs.
 *
 *    #25        16 Jun 2005            DP
 * Rewrote data receive routine.  Improved high level signalling message
 * processing.  Improved debug messages.
 *
 *    #24        02 May 2005           JTL
 * Add command to disable timeout.
 * Fixed issues with timeout operation.
 *
 *    #23        22 Apr 2005           JTL
 * Put in an idle timeout (currently #defined to 15 seconds). Whenever in a
 * "W4" state, or in the OPEN state when no data has passed, if this timeout
 * passes, the L2 link will be disconnected, and eventually the ACL link if
 * there are no other connections.
 *
 *    #22        12 Apr 2005            GS
 * Don't cancel L2CAP timers when BT QDSP image is being unloaded.
 *
 *    #21        8  Apr 2005            DP
 * Make use of BT_BDA debug message macro.
 *
 *    #19        07 Apr 2005            JH
 * Fixed CID wrap around problem.
 *
 *    #18        16 Mar 2005            JTL
 * Fix event fields in connect failed event to include the proper CID,
 * rather than uninitialized value.
 *
 *    #17        24 Sep 2004            DP
 * Reworked AMS compilation controls.
 *
 *    #15-16     08 Sep 2004            JTL
 * Fix CR 48516. Incorretly copying cdb's around -- including
 * rex_timer_types. Clear out needlessly copied fields.
 * Removing unused fields from bt_l2_conn_db_type.
 * Rename misspelled "souce_cid" to source_cid
 *
 *    #14        29 Jul 2004            JTL
 * Added bt_cmd_l2_connect_immediate_cid -- immediately return the CID
 * L2 will use for the connection, in addition to returning it in the
 * connect event.
 *
 *    #11-13     15 Jun 2004            DP
 * Added PSM based Mode 2 security.
 *
 *    #10        25 May 2004            JTL
 * Fixed CR 20452:  BT: L2 echo croaks when rx many bytes
 * Changed bt_l2_process_echo_rqst_pkt so it doesn't copy more than 4 bytes
 * from the echo packet.
 *
 *    #8,#9      20 May 2004            JTL
 * Fixed CR 42160: ASSERT in bt_l2_process_conn_rqst_pkt() for NULL cdb_ptr.
 * Changed bt_l2_ev_rm_rx_data_acl() so that it doesn't blow away the cdb_ptr
 * when it's cleaning up an old connection with unassembled partial packets.
 * In addition, cleaned up a couple other places where
 * bt_l2_get_conn_entry_from_cid() could have been returning NULL, but it
 * wasn't checked by the code.
 *
 *    #7         30 Mar 2004            DP
 * Added EMS support.
 *
 *    #6         3  Oct 2003            DP
 * Converted RM timer to background timer for entire driver.
 *
 *    Rev 1.34   16 Oct 2002 15:42:10   waynelee
 * Fixed issue with l2cap rx when first l2cap fragment contained only
 * the l2cap header.
 *
 *    Rev 1.33   12 Apr 2002 11:17:04   propach
 * Packed L2CAP signaling packet structures.  Made sending of signalling
 * packets use dsm_pushdown_tail instead of memcpy.
 *
 *    Rev 1.32   08 Apr 2002 15:51:14   waynelee
 * Fixed corner case where L2 would disconnect the ACL after 1
 * second even if another L2CAP channel came up during that 1
 * second.  Added debug MSGs.
 *

===========================================================================*/

/*===========================================================================

                              INCLUDE FILES

===========================================================================*/

#include "customer.h"
#include "comdef.h"
#ifdef FEATURE_BT
#include <string.h>
#include "assert.h"
#include "dsm.h"
#include "task.h"

#include "bt.h"
#include "bti.h"
#include "btcmdi.h"

#ifndef FEATURE_BT_SOC 
#include "btbb.h"
#include "bthcetc.h"
#endif /* FEATURE_BT_SOC */
#include "btdebug.h"
#include "btl2.h"
#include "btmsg.h"
#ifdef FEATURE_BT_SOC 
#include "btsocetc.h"
#include "bthcdrv.h"
#else
#include "btqddata.h"
#include "btqdspq.h"
#endif /* FEATURE_BT_SOC */
#include "btsd.h"
#include "btutils.h"

#if defined (BT_SWDEV_QXDM_LOG_MSG) || defined (BT_SWDEV_QXDM_LOG_PER)
#include "btlogi.h"
#include "log.h"
#endif

#ifdef BT_TEST_PYLD_FAST_RX
#include "btrcev.h"
#endif

#ifdef BT_TEST_PYLD_FAST_TX
#include "btrm.h"
#endif

#undef BT_L2_DUMP_DATA_ON_ERROR

#ifdef BT_L2_DUMP_DATA_ON_ERROR
#include "fs.h"
#include "AEEStdLib.h"
#endif

#define BT_MSG_LAYER  BT_MSG_L2   /* necessary for btmsg.h */


/*-------------------------------------------------------------------------*/
/*                                                                         */
/*                  Module Definitions and Declarations                    */
/*                                                                         */
/*-------------------------------------------------------------------------*/

#define BT_MAX_L2CAP_PDU 339

#define BT_L2_MIN_FLUSH_TIMEOUT 0x0001

/* supports up to 3 L2CAP connects per device */
#define BT_L2_MAX_CONN ( 3 * BT_MAX_BT_DEV )

// CIDs
#define BT_L2_SIGNALLING_CID         0x0001
#define BT_L2_CONNLESS_RECEPTION_CID 0x0002

// MTU
#define BT_L2_MIN_MTU                48
#ifdef FEATURE_BT_SOC
  #define BT_L2_MAX_MTU              2036
#else
#define BT_L2_MAX_MTU                BT_L2_DEFAULT_MTU
#endif /* FEATURE_BT_SOC */
// service types
#define BT_L2_SERVICE_NO_TRAFFIC     0x00
#define BT_L2_SERVICE_BEST_EFFORT    0x01
#define BT_L2_SERVICE_GUARANTEED     0x02

#define BT_L2_CONFIG_OPTION_MTU           0x01
#define BT_L2_CONFIG_OPTION_FLUSH_TIMEOUT 0x02
#define BT_L2_CONFIG_OPTION_QOS           0x03

#define BT_L2_CONN_REFUSED(result) \
   ((result == 0x0002) || (result == 0x0003) || (result == 0x0004))

// states
typedef enum
{
  BT_L2_CLOSED = 0,
  BT_L2_W4_ACL_LINK,
  BT_L2_ACL_LINK_CONNECTED,
  BT_L2_W4_L2CAP_CONNECT_RQST,
  BT_L2_W4_RM_AUTH,
  BT_L2_W4_L2CAP_CONNECT_RSP,
  BT_L2_W4_L2CA_CONNECT_RSP,
  BT_L2_CONFIG,
  BT_L2_OPEN,
  BT_L2_RECONFIG,
  BT_L2_REOPEN,
  BT_L2_W4_L2CAP_DISCONNECT_RSP,
  BT_L2_W4_L2CA_DISCONNECT_RSP
} bt_l2_channel_state_type;

// events to upper layer
typedef enum
{
  BT_L2_EC_CONNECT_IND,
  BT_L2_EC_CONNECT_CFM,
  BT_L2_EC_CONNECT_PND,
  BT_L2_EC_CONFIG_IND,
  BT_L2_EC_CONFIG_CFM,
  BT_L2_EC_DISCONNECT_IND,
  BT_L2_EC_DISCONNECT_CFM,
  BT_L2_EC_TIMEOUT_IND,
  BT_L2_EC_QOS_VIOLATION_IND,
  BT_L2_EC_DATA_WRITE_IND,   // ever used?
  BT_L2_EC_DATA_WRITE_CFM,
  BT_L2_EC_DATA_READ_IND,
  BT_L2_EC_DATA_READ_CFM
} bt_l2_l2ca_event_code_type;

// signalling
typedef enum
{
  BT_L2_SIG_CMD_REJECT            = 0x01,
  BT_L2_SIG_CONNECTION_RQST,     // 0x02
  BT_L2_SIG_CONNECTION_RSP,      // 0x03
  BT_L2_SIG_CONFIGURE_RQST,      // 0x04
  BT_L2_SIG_CONFIGURE_RSP,       // 0x05
  BT_L2_SIG_DISCONNECTION_RQST,  // 0x06
  BT_L2_SIG_DISCONNECTION_RSP,   // 0x07
  BT_L2_SIG_ECHO_RQST,           // 0x08
  BT_L2_SIG_ECHO_RSP,            // 0x09
  BT_L2_SIG_INFO_RQST,           // 0x0A
  BT_L2_SIG_INFO_RSP             // 0x0B
} bt_l2_sig_cmd_type;


// configuration options
typedef enum
{
  BT_L2_CO_MTU                = 0x01,
  BT_L2_CO_FLUSH_TIMEOUT,
  BT_L2_CO_QOS
} bt_l2_config_options_type;

// configuration states
/* Device A                                       Device B
 * --------                                       --------
 * config_req_state = START                       config_req_state = START
 * config_rsp_state = START                       config_rsp_state = START
 *
 *    --------------- L2 Config Request ------------->
 * config_req_state = RQST_SENT                   config_rsp_state = RQST_RCVD
 *
 *    <-------------- L2 Config Response -------------
 * config_req_state = END                         config_rsp_state = END
 *
 *    <-------------- L2 Config Request --------------
 * config_rsp_state = RQST_RCVD                   config_req_state = RQST_SENT
 *
 *    --------------- L2 Config Response ------------>
 * config_rsp_state = END                         config_req_state = END
 * state = OPEN                                   state = OPEN
 * config_req_state = START                       config_req_state = START
 * config_rsp_state = START                       config_rsp_state = START
 */
typedef enum
{
  BT_L2_CONFIG_START = 0,
  BT_L2_RQST_SENT,
  BT_L2_RQST_RCVD,
  BT_L2_CONFIG_END
} bt_l2_config_state_type;


// result code
typedef enum
{
  BT_L2_REASON_CMD_NOT_UNDERSTOOD,
  BT_L2_REASON_MTU_EXCEEDED,
  BT_L2_REASON_INVALID_CID
} bt_l2_cmd_reject_reason_type;

typedef enum
{
  BT_L2_CONN_REQ_SUCCESS                = 0x0000,
  BT_L2_CONN_PENDING                    = 0x0001,
  BT_L2_CONN_REFUSED_PSM_UNSUPPORTED    = 0x0002,
  BT_L2_CONN_REFUSED_SECURITY_BLOCK     = 0x0003,
  BT_L2_CONN_REFUSED_NO_RESOURCE        = 0x0004
} bt_l2_conn_req_result_type;

typedef enum
{
  BT_L2_CONN_REQ_NO_INFO                = 0x0000,
  BT_L2_CONN_REQ_AUTHENTICATION_PENDING = 0x0001,
  BT_L2_CONN_REQ_AUTHORIZATION_PENDING  = 0x0002
} bt_l2_conn_req_status;

typedef enum
{
  BT_L2_CONN_RSP_SUCCESS,
  BT_L2_CONN_RSP_FAILED
} bt_l2_conn_rsp_result_type;

typedef enum
{
  BT_L2_CONFIG_REQ_SUCCESS,
  BT_L2_CONFIG_FAILED_INVALID_CID,
  BT_L2_CONFIG_FAILED_UNACCEPTABLE_PARAMS,
  BT_L2_CONFIG_FAILED_MTU_EXCEEDED,
  BT_L2_CONFIG_FAILED_UNKNOWN_OPTION,
  BT_L2_CONFIG_FAILED_OTHER_REASON,   // NOTE: not in specs, but I need it
  BT_L2_CONFIG_TIMEOUT = 0xEEEE
} bt_l2_config_req_result_type;

typedef enum
{
  BT_L2_CONFIG_RSP_SUCCESS                  = 0x0000,
  BT_L2_CONFIG_REJECTED_UNACCEPTABLE_PARAMS = 0x0001,
  BT_L2_CONFIG_REJECTED_NO_REASON           = 0x0002,
  BT_L2_CONFIG_REJECTED_UNKNOWN_OPTION      = 0x0003,
  BT_L2_CONFIG_REJECTED_INVALID_CID         = 0x0004
} bt_l2_config_rsp_result_type;

typedef enum
{
  BT_L2_WRITE_DATA_SUCCESS,
  BT_L2_WRITE_DATA_FLUSH_TO,
  BT_L2_WRITE_DATA_LINK_TERMINATED,
  BT_L2_WRITE_DATA_INVALID_CID
} bt_l2_write_data_result_type;


typedef enum
{
  BT_L2_READ_DATA_SUCCESS,
  BT_L2_READ_DATA_LINK_TERMINATED,
  BT_L2_READ_DATA_INVALID_CID
} bt_l2_read_data_result_type;

/* ----------------------------------------------------------
Command header

------------------------------------
|  Code  |  Matching ID  |  Length |  Data
------------------------------------

---------------------------------------------------------- */
typedef BT_PACKED struct
{
  byte command_code;
  byte matching_id;    // to match a reply to a request
  word length;
} bt_l2_cmd_hdr_type;

typedef BT_PACKED struct
{
  word reason;
  byte data[ 4 ];
} bt_l2_cmd_reject_cmd_type;

typedef BT_PACKED struct
{
  word psm;
  word source_cid;  // CID on the device sending this request pkt
} bt_l2_conn_req_cmd_type;

typedef BT_PACKED struct
{
  word dest_cid;    // CID on the device sending this response pkt
  word source_cid;   // CID on the device receiving this response pkt
  word result;
  word status;
} bt_l2_conn_rsp_cmd_type;

typedef struct
{
  byte type;
  // Length is 2 (for MTU and Flush_TO) or 22 (for QoS)
  // or 9 (for rTX and flow ctl)
  byte length;
  byte option[ sizeof( bt_l2_qos_type ) + 19 ];
} bt_l2_config_option_type;

typedef BT_PACKED struct
{
  word dest_cid;    // CID on the device receiving this request pkt
  word flags;
  byte data[ 32 ];  // 4 for MTU, 4 for Flush_TO, 24 for QoS
} bt_l2_config_req_cmd_type;

typedef BT_PACKED struct
{
  word source_cid;  // CID on the device receiving this response pkt
  word flags;
  word result;
  byte data[ 32 ]; // 4 for MTU, 4 for Flush_TO, 24 for QoS

} bt_l2_config_rsp_cmd_type;

typedef BT_PACKED struct
{
  word dest_cid;    // CID on the device receiving this request
  word source_cid;  // CID on the device sending this request
} bt_l2_disconn_req_cmd_type;

typedef BT_PACKED struct
{
  word dest_cid;     // CID on the device sending this response pkt
  word source_cid;   // CID on the device receiving this response pkt
} bt_l2_disconn_rsp_cmd_type;

typedef BT_PACKED struct
{
  byte data[ BT_L2_MAX_ECHO_DATA_LEN ];
} bt_l2_echo_req_cmd_type;

typedef BT_PACKED struct
{
  byte data[ BT_L2_MAX_ECHO_DATA_LEN ];
} bt_l2_echo_rsp_cmd_type;

typedef BT_PACKED struct
{
  word type;
} bt_l2_info_req_cmd_type;

typedef BT_PACKED struct
{
  word type;
  word result;
  byte data[ 2 ];  // TBD
} bt_l2_info_rsp_cmd_type;

typedef struct
{
  bt_l2_hdr_type                pkt_hdr;       // 4 bytes
  bt_l2_cmd_hdr_type            cmd_hdr;       // 4 bytes

  union
  {
    bt_l2_cmd_reject_cmd_type   cmd_reject;    // 2-6 bytes
    bt_l2_conn_req_cmd_type     conn_req;      // 4 bytes
    bt_l2_conn_rsp_cmd_type     conn_rsp;      // 8 bytes
    bt_l2_config_req_cmd_type   config_req;    // 4-? bytes
    bt_l2_config_rsp_cmd_type   config_rsp;    // 6-? bytes
    bt_l2_disconn_req_cmd_type  disconn_req;   // 4 bytes
    bt_l2_disconn_rsp_cmd_type  disconn_rsp;   // 4 bytes
    bt_l2_echo_req_cmd_type     echo_req;      // 0-4 bytes
    bt_l2_echo_rsp_cmd_type     echo_rsp;      // 0-4 bytes
    bt_l2_info_req_cmd_type     info_req;      // 2 bytes
    bt_l2_info_rsp_cmd_type     info_rsp;      // 4-6 bytes

  } cmd;
  rex_timer_type                expected_resp_timer;
  boolean                       rtx_timer_in_use;
  uint32                        rtx_timer_value;
  uint32                        ertx_timer_value;
  uint32                        total_rtx_wait_time;

} bt_l2_sig_pkt_type;

typedef struct
{
  word length;
  word remote_cid;
  byte payload[ 1 ];
} bt_l2_data_pkt_type;

#define BT_L2_MAX_PENDING_RQSTS 3
#define BT_L2_DEFAULT_INITIAL_RTX_TIMER_VALUE   60000
#define BT_L2_MAX_INITIAL_RTX_TIMER_VALUE       60000
#define BT_L2_MAX_TOTAL_RTX_WAIT_TIME           60000
#define BT_L2_DEFAULT_INITIAL_ERTX_TIMER_VALUE  60000
#define BT_L2_MAX_INITIAL_ERTX_TIMER_VALUE     300000
#define BT_L2_MAX_CONFIG_REQ_WAIT_TIME           1000
#define BT_L2_MAX_TOTAL_CONFIG_PROCESS_TIME     60000
#define BT_L2_ACL_LINK_DISCONNECT_TIME           3000
#define BT_L2_ACL_LINK_SEC_FAIL_DIS_TIME           50
#ifdef FEATURE_BT_TEST_API
#error code not present
#else
#define BT_L2_IDLE_TIMEOUT_MS                   90000
#endif /* FEATURE_BT_TEST_API */

typedef struct
{
  bt_l2_channel_state_type     state;
  bt_l2_channel_state_type     last_state_before_open;
  bt_l2_config_state_type      config_req_state;
  bt_l2_config_state_type      config_rsp_state;
  bt_lm_conn_state_type        conn_status;
  bt_bd_addr_type              bd_addr;

  uint16                       conn_hndl;

  word                         local_cid;
  word                         remote_cid;
  word                         in_flush_to;
  word                         out_flush_to;
  word                         link_to;
  word                         in_mtu;  // this channel can accept
  word                         out_mtu; // this channel will send
  bt_l2_qos_type               out_qos;
  bt_l2_qos_type               in_qos;

  bt_l2_sig_pkt_type*          outstanding_rqst_ptr[ BT_L2_MAX_PENDING_RQSTS ];
  word                         result;
  word                         status;

  boolean                      crp_hdr_valid;  /*  Current receive        */
  bt_l2_hdr_type               crp_l2_hdr;     /*    packed information.  */
  dsm_item_type*               crp_dsm_ptr;

  bt_l2_sig_cmd_type           pending_sig_cmd;
  boolean                      initiated_conn;
  boolean                      initiated_disconn;
  boolean                      received_reconfig;
  bt_app_id_type               app_id;
  byte                         last_conn_rqst_rcvd_matching_id;
  byte                         last_config_rqst_rcvd_matching_id;
  byte                         last_conn_rqst_sent_matching_id;
  byte                         last_config_rqst_sent_matching_id;
  byte                         last_disconn_rqst_sent_matching_id;
  byte                         last_echo_rqst_sent_matching_id;
  boolean                      last_config_rqst_rcvd_c_flag_set;
  uint16                       bad_pkts_rxd;
  bt_role_type                 baseband_conn_role;
  rex_timer_type               config_req_wait_timer;
  rex_timer_type               total_config_process_timer;
  rex_timer_type               acl_link_disconnect_timer;
  rex_timer_type               acl_link_sec_fail_dis_timer;
  bt_event_reason_type         disconn_reason;
  boolean                      disconn_acl_immediate;

  boolean                      serv_sec_local_initiated;
  uint16                       serv_sec_psm;
  byte                         serv_sec_matching_id;
  word                         serv_sec_source_cid;
  word                         serv_sec_result;
  word                         serv_sec_status;
  bt_error_code_type           serv_sec_error_code;

  bt_app_id_type               echo_app_id;
  bt_l2_echo_req_cmd_type      echo_data;
  int32                        idle_timeout;
  boolean                      acl_tx_flow_on;
} bt_l2_conn_db_type;

LOCAL  uint16                   bt_l2_outgoing_conn_count=0;
LOCAL  uint16                   bt_l2_bad_sig_pkts_rxd=0;
static uint16                   bt_l2_max_hci_acl_len=BT_MTP_MAX_HC_LEN; 

/*===========================================================================

MACRO
  BT_L2_IS_PSM_VALID

DESCRIPTION
  Checks the validity of a PSM as follows:
    1. The least significant bit of the least significant octet must
       be '1', i.e. the PSM value must be odd
    2. The least significant bit of the most significant octet must
       be '0'

===========================================================================*/
#define BT_L2_IS_PSM_VALID( psm ) \
   ( ( ( (psm) & 0x0001 ) == 1 ) && ( ( ( (psm) >> 8 ) & 0x0001 ) == 0 ) )

#define BT_L2_IS_C_FLAG_SET( flag ) ( ( (flag) & 0x01 ) == 1 )

/*-------------------------------------------------------------------------*/
/*                                                                         */
/*                         Data Definitions                                */
/*                                                                         */
/*-------------------------------------------------------------------------*/

bt_app_id_type bt_l2_app_id = BT_APP_ID_NULL;

/*-------------------------------------------------------------------------*/
/*  Information and queue for events destined for the L2CAP layer.         */
/*-------------------------------------------------------------------------*/
#ifdef FEATURE_BT_SOC
  #define BT_L2_NUM_EVENTS    100
#else
  #define BT_L2_NUM_EVENTS    30
#endif /* FEATURE_BT_SOC */
LOCAL bt_event_q_info_type  bt_l2_ev_q_info;
LOCAL q_type                bt_l2_ev_q;


// pool of signalling packets
#define BT_L2_MAX_SIGNALLING_PKTS   20
LOCAL bt_l2_sig_pkt_type  bt_l2_sig_pkts[ BT_L2_MAX_SIGNALLING_PKTS ];

#define BT_L2_CID_FIRST 0x0040
#define BT_L2_CID_LAST  0xFFFF
LOCAL word bt_l2_next_local_cid;
LOCAL byte bt_l2_next_matching_id;
LOCAL bt_l2_conn_db_type bt_l2_conn_db[ BT_L2_MAX_CONN ];

LOCAL bt_l2_qos_type bt_l2_default_qos =
{
  0,
  BT_L2_DEFAULT_SERVICE_TYPE,
  BT_L2_DEFAULT_TOKEN_RATE,
  BT_L2_DEFAULT_TOKEN_BUCKET_SIZE,
  BT_L2_DEFAULT_PEAK_BANDWIDTH,
  BT_L2_DEFAULT_LATENCY,
  BT_L2_DEFAULT_DELAY_VARIATION
};

void bt_rm_auth_failed (bt_bd_addr_type* bd_addr);


/*-------------------------------------------------------------------------*/
/*  L2CAP registration table definitions and declaration.                  */
/*-------------------------------------------------------------------------*/

typedef struct
{
  uint16                psm;
  bt_app_id_type        bt_app_id;
  bt_l2_psm_param_type  psm_params;
  uint16                incoming_conn_count;
  boolean               registered;
} bt_l2_reg_table_type;

LOCAL uint16                bt_l2_cur_regd_count;
LOCAL bt_l2_reg_table_type  bt_l2_reg_table[ BT_L2_MAX_PSM_COUNT ];
LOCAL void bt_l2_free_signalling_pkt( bt_l2_sig_pkt_type*  pkt_ptr );
LOCAL bt_l2_reg_table_type* bt_l2_find_reg_table_entry_app_id(
                                      bt_app_id_type   app_id);

/*===========================================================================

                         FUNCTION DEFINITIONS

===========================================================================*/

#ifdef BT_SWDEV_QXDM_LOG_MSG
/*===========================================================================

FUNCTION
  bt_l2_send_tx_signaling_log

DESCRIPTION
  Logs all the bytes in a specified L2CAP transmit signaling packet
  to the QXDM.

===========================================================================*/
LOCAL void bt_l2_send_tx_signaling_log
(
  dsm_item_type*  dsm_ptr,
  word            cid
)
{
  log_bt_l2_sig_tx_type*  log_ptr;
  uint16                  data_length;

  data_length = dsm_length_packet( dsm_ptr );
  data_length = MIN( data_length, BT_L2_LOG_BUFFER_SIZE );

  log_ptr = (log_bt_l2_sig_tx_type *)log_alloc( log_bt_l2_sig_tx,
                       sizeof( log_bt_l2_sig_tx_type ) -
                       BT_L2_LOG_BUFFER_SIZE + data_length );

  if ( log_ptr != NULL )
  {
    uint16 bytes_copied;
    log_ptr->length    = data_length;
    log_ptr->local_cid = (uint16) cid;

    bytes_copied = dsm_extract( dsm_ptr, BT_FIRST_BYTE_OFFSET,
                                (void *)log_ptr->buffer,
                                data_length );
    if( bytes_copied != data_length )
    {
      BT_ERR("BT L2: Error sending data to QXDM",0,0,0);
    }

    log_commit( log_ptr );
  }
}

/*===========================================================================

FUNCTION
  bt_l2_send_rx_signaling_log

DESCRIPTION
  Logs all the bytes in a specified L2CAP transmit signalling packet
  to the QXDM.

===========================================================================*/
LOCAL void bt_l2_send_rx_signaling_log
(
  dsm_item_type*  dsm_ptr,
  word            conn_hndl
)
{
  log_bt_l2_sig_rx_type*  log_ptr;
  uint16                  data_length;

  data_length = dsm_length_packet( dsm_ptr );
  data_length = MIN( data_length, BT_L2_LOG_BUFFER_SIZE );

  log_ptr = ( log_bt_l2_sig_rx_type *)log_alloc( log_bt_l2_sig_rx,
                       sizeof( log_bt_l2_sig_rx_type ) -
                       BT_L2_LOG_BUFFER_SIZE + data_length );

  if ( log_ptr != NULL )
  {
    uint16 bytes_copied;
    log_ptr->length    = data_length;
    log_ptr->local_cid = (uint16)(conn_hndl);

    bytes_copied = dsm_extract( dsm_ptr, BT_FIRST_BYTE_OFFSET,
                                (void *)(log_ptr->buffer),
                                data_length );
    if( bytes_copied != data_length )
    {
      BT_ERR("BT L2: Error sending data to QXDM",0,0,0);
    }

    log_commit( log_ptr );
  }
}


/*===========================================================================

FUNCTION
  bt_l2_send_payload_tx_log

DESCRIPTION
  Logs the first and last bytes in a specified transmit L2CAP
  payload packet to the QXDM.

===========================================================================*/
LOCAL void bt_l2_send_payload_tx_log
(
  dsm_item_type*  dsm_ptr,
  word            cid
)
{
  log_bt_l2_pyld_tx_type*  log_ptr;
  uint16                   data_length;

  data_length = dsm_length_packet( dsm_ptr );
  data_length = MIN( data_length, BT_PYLD_BUFFER_SIZE );

  log_ptr = (log_bt_l2_pyld_tx_type *)log_alloc( log_bt_l2_pyld_tx,
                       sizeof( log_bt_l2_pyld_tx_type ) -
                       BT_PYLD_BUFFER_SIZE + data_length );

  if ( log_ptr != NULL )
  {
    log_ptr->length    = data_length;
    log_ptr->local_cid = (uint16)(cid);

    (void) dsm_extract( dsm_ptr, BT_FIRST_BYTE_OFFSET,
                        (void *)(log_ptr->buffer),
                        MIN( data_length, BT_FIRST_TEN_PYLD_BYTES ) );

    if ( data_length >=
           (BT_FIRST_TEN_PYLD_BYTES + BT_LAST_TEN_PYLD_BYTES) )
    {
      (void)dsm_extract( dsm_ptr, (data_length - BT_LAST_TEN_PYLD_BYTES),
                         (void *)(log_ptr->buffer + BT_FIRST_TEN_PYLD_BYTES),
                         BT_LAST_TEN_PYLD_BYTES );
    }
    else if ( data_length > BT_FIRST_TEN_PYLD_BYTES )
    {
      (void)dsm_extract( dsm_ptr, BT_FIRST_TEN_PYLD_BYTES,
                         (void *)(log_ptr->buffer + BT_FIRST_TEN_PYLD_BYTES),
                         (data_length - BT_FIRST_TEN_PYLD_BYTES) );
    }
    log_commit( log_ptr );
  }
}


/*===========================================================================

FUNCTION
  bt_l2_send_payload_rx_log

DESCRIPTION
  Logs the first and last bytes in a specified received L2CAP
  payload packet to the QXDM.

===========================================================================*/
LOCAL void bt_l2_send_payload_rx_log
(
  dsm_item_type*  dsm_ptr,
  word            cid
)
{

  log_bt_l2_pyld_rx_type*  log_ptr;
  uint16                   data_length;

  data_length = dsm_length_packet( dsm_ptr );
  data_length = MIN( data_length, BT_PYLD_BUFFER_SIZE );

  log_ptr = ( log_bt_l2_pyld_rx_type *)log_alloc( log_bt_l2_pyld_rx,
                       sizeof( log_bt_l2_pyld_rx_type ) -
                       BT_PYLD_BUFFER_SIZE + data_length );

  if ( log_ptr != NULL )
  {
    log_ptr->length    = data_length;
    log_ptr->local_cid = (uint16)(cid);

    (void)dsm_extract( dsm_ptr, BT_FIRST_BYTE_OFFSET,
                       (void *)(log_ptr->buffer),
                       MIN( data_length, BT_FIRST_TEN_PYLD_BYTES ) );

    if ( data_length >=
           (BT_FIRST_TEN_PYLD_BYTES + BT_LAST_TEN_PYLD_BYTES) )
    {
      (void)dsm_extract( dsm_ptr, (data_length - BT_LAST_TEN_PYLD_BYTES),
                         (void *)(log_ptr->buffer + BT_FIRST_TEN_PYLD_BYTES),
                         BT_LAST_TEN_PYLD_BYTES );
    }
    else if ( data_length > BT_FIRST_TEN_PYLD_BYTES )
    {
      (void)dsm_extract( dsm_ptr, BT_FIRST_TEN_PYLD_BYTES,
                         (void *)(log_ptr->buffer + BT_FIRST_TEN_PYLD_BYTES),
                         (data_length - BT_FIRST_TEN_PYLD_BYTES) );
    }
    log_commit( log_ptr );
  }

}
#endif /* BT_SWDEV_QXDM_LOG_MSG */


/*===========================================================================

FUNCTION
  bt_l2_store_bt_event

DESCRIPTION
  Stores a specified BT event on the L2CAP BT event queue.

===========================================================================*/
void bt_l2_store_bt_event
(
  bt_ev_msg_type*  bt_ev_ptr
)
{

  bt_store_bt_event( bt_ev_ptr, &bt_l2_ev_q_info );

}


/*===========================================================================

FUNCTION
  bt_l2_convert_disconn_reason_to_rm

DESCRIPTION
  Converts an L2 disconnect reason to an RM one.

===========================================================================*/
bt_rm_disc_reason_type bt_l2_convert_disconn_reason_to_rm
(
  bt_event_reason_type disconn_reason
)
{
  bt_rm_disc_reason_type rm_reason;

  switch( disconn_reason )
  {
    case BT_EVR_L2_NORMAL_DISCONNECT:
    case BT_EVR_L2_LOCAL_REJECT_CONNECTION:
    case BT_EVR_L2_REMOTE_REJECT_CONNECTION:
    case BT_EVR_L2_LINK_LOST:
      rm_reason = BT_RMDR_USER_ENDED;
      break;
    case BT_EVR_L2_AUTHENTICATION_FAILED:
        rm_reason = BT_RMDR_AUTH_FAILED;
        break; 
    default:
      BT_ERR("BT L2: Disconnect reason invalid %x", disconn_reason, 0, 0);
      rm_reason = BT_RMDR_USER_ENDED;
  }

  return rm_reason;
}

/*===========================================================================

FUNCTION
  bt_l2_set_default_qos

DESCRIPTION

===========================================================================*/
LOCAL void bt_l2_set_default_qos
(
  bt_l2_qos_type*  qos_ptr
)
{

  qos_ptr->flags             = 0;
  qos_ptr->service_type      = BT_L2_DEFAULT_SERVICE_TYPE;
  qos_ptr->token_rate        = BT_L2_DEFAULT_TOKEN_RATE;
  qos_ptr->token_bucket_size = BT_L2_DEFAULT_TOKEN_BUCKET_SIZE;
  qos_ptr->peak_bandwidth    = BT_L2_DEFAULT_PEAK_BANDWIDTH;
  qos_ptr->latency           = BT_L2_DEFAULT_LATENCY;
  qos_ptr->delay_variation   = BT_L2_DEFAULT_DELAY_VARIATION;

}


/*===========================================================================

FUNCTION
  bt_l2_init_service_security_info

DESCRIPTION
  Initializes service level security information within a specified
  L2CAP connection table entry.

===========================================================================*/
LOCAL void bt_l2_init_service_security_info
(
  bt_l2_conn_db_type*  cdb_ptr
)
{

  cdb_ptr->serv_sec_local_initiated = FALSE;
  cdb_ptr->serv_sec_psm             = BT_L2_PSM_INVALID;
  cdb_ptr->serv_sec_matching_id     = 0;
  cdb_ptr->serv_sec_source_cid      = 0;
  cdb_ptr->serv_sec_result          = 0;
  cdb_ptr->serv_sec_status          = 0;
  cdb_ptr->serv_sec_error_code      = BT_BE_SUCCESS;

}


/*===========================================================================

FUNCTION
  bt_l2_init_conn_cur_rx_packet

DESCRIPTION
  Initialize the current receive packet of a specified connection.

===========================================================================*/
LOCAL void bt_l2_init_conn_cur_rx_packet
(
  bt_l2_conn_db_type*  cdb_ptr
)
{

  uint32 dsm_length = dsm_length_packet( cdb_ptr->crp_dsm_ptr );

  if ( cdb_ptr->crp_dsm_ptr != NULL )
  {
    if ( cdb_ptr->crp_hdr_valid != FALSE )
    {
      BT_ERR( "BT L2 RX: CRP init; Drop L %x R %x CID %x",
              cdb_ptr->crp_l2_hdr.length,
              dsm_length,
              cdb_ptr->crp_l2_hdr.channel_id );
    }
    else
    {
      BT_ERR( "BT L2 RX: CRP init; Drop No hdr R %x CID %x",
              dsm_length,
              cdb_ptr->local_cid, 0 );
    }
    dsm_free_packet( &cdb_ptr->crp_dsm_ptr );
  }

  cdb_ptr->crp_hdr_valid         = FALSE;
  cdb_ptr->crp_l2_hdr.length     = 0;
  cdb_ptr->crp_l2_hdr.channel_id = BT_L2_NULL_CID;

}


/*===========================================================================

FUNCTION
  bt_l2_init_conn_db_entry

DESCRIPTION
  Initializes a specified entry in the L2CAP connection table.

===========================================================================*/
LOCAL void bt_l2_init_conn_db_entry
(
  bt_l2_conn_db_type*  cdb_ptr
)
{

  byte                 i;
  bt_l2_sig_pkt_type*  pkt_ptr;

  bt_l2_set_default_qos( &cdb_ptr->in_qos );
  bt_l2_set_default_qos( &cdb_ptr->out_qos );
  cdb_ptr->conn_hndl         = BT_HC_NULL_CONN_HANDLE;
  cdb_ptr->state             = BT_L2_CLOSED;
  cdb_ptr->config_req_state  = BT_L2_CONFIG_START;
  cdb_ptr->config_rsp_state  = BT_L2_CONFIG_START;
  cdb_ptr->conn_status       = BT_LM_CONN_DOWN;
  cdb_ptr->local_cid         = BT_L2_NULL_CID;
  cdb_ptr->remote_cid        = BT_L2_NULL_CID;
  cdb_ptr->in_mtu            = BT_L2_DEFAULT_MTU;
  cdb_ptr->out_mtu           = BT_L2_DEFAULT_MTU;
  cdb_ptr->in_flush_to       = BT_L2_DEFAULT_FLUSH_TIMEOUT;
  cdb_ptr->out_flush_to      = BT_L2_DEFAULT_FLUSH_TIMEOUT;
  cdb_ptr->link_to           = 0;
  cdb_ptr->result            = 0;
  cdb_ptr->status            = 0;
  cdb_ptr->pending_sig_cmd   = BT_L2_SIG_CMD_REJECT;
  cdb_ptr->initiated_conn    = TRUE;
  cdb_ptr->initiated_disconn = FALSE;
  cdb_ptr->received_reconfig = FALSE;
  cdb_ptr->app_id            = BT_APP_ID_NULL;
  cdb_ptr->echo_app_id       = BT_APP_ID_NULL;
  cdb_ptr->idle_timeout      = 0;
  cdb_ptr->acl_tx_flow_on    = TRUE;
  cdb_ptr->disconn_acl_immediate              = FALSE;
  cdb_ptr->last_conn_rqst_rcvd_matching_id    = 0;
  cdb_ptr->last_config_rqst_rcvd_matching_id  = 0;
  cdb_ptr->last_conn_rqst_sent_matching_id    = 0;
  cdb_ptr->last_config_rqst_sent_matching_id  = 0;
  cdb_ptr->last_disconn_rqst_sent_matching_id = 0;
  cdb_ptr->last_echo_rqst_sent_matching_id    = 0;
  cdb_ptr->last_config_rqst_rcvd_c_flag_set   = FALSE;
  memset( (byte*)(&cdb_ptr->bd_addr), 0, sizeof( bt_bd_addr_type ) );

  bt_l2_init_conn_cur_rx_packet( cdb_ptr );

  for (i=0; i<BT_L2_MAX_PENDING_RQSTS; i++)
  {
    if ( cdb_ptr->outstanding_rqst_ptr[i] != NULL )
    {
      pkt_ptr = cdb_ptr->outstanding_rqst_ptr[i];
      bt_l2_free_signalling_pkt( pkt_ptr );
      cdb_ptr->outstanding_rqst_ptr[i] = NULL;
    }
  }
  cdb_ptr->bad_pkts_rxd = 0;
  cdb_ptr->baseband_conn_role = BT_ROLE_SLAVE;
  (void)rex_clr_timer( &cdb_ptr->config_req_wait_timer );
  (void)rex_clr_timer( &cdb_ptr->total_config_process_timer );
  (void)rex_clr_timer( &cdb_ptr->acl_link_disconnect_timer );
  (void)rex_clr_timer( &cdb_ptr->acl_link_sec_fail_dis_timer );
  

  bt_l2_init_service_security_info( cdb_ptr );

}

/*===========================================================================

FUNCTION
  bt_l2_init_reg_table_entry

DESCRIPTION
  Initializes a specified entry in the L2CAP registration table.

===========================================================================*/
LOCAL void bt_l2_init_reg_table_entry
(
  bt_l2_reg_table_type*  reg_ptr
)
{

  bt_l2_set_default_qos( &reg_ptr->psm_params.out_qos );
  reg_ptr->psm                     = BT_L2_PSM_INVALID;
  reg_ptr->psm_params.out_flush_to = BT_L2_DEFAULT_FLUSH_TIMEOUT;
  reg_ptr->psm_params.in_mtu       = BT_L2_DEFAULT_MTU;
  reg_ptr->bt_app_id               = BT_APP_ID_NULL;
  reg_ptr->incoming_conn_count     = 0;
  reg_ptr->registered              = FALSE;

}


/*===========================================================================

FUNCTION
  bt_l2_conn_closed_init_reg_table_entry_if_ok

DESCRIPTION
  Initializes the entry in the L2CAP registration table corresponding
  to the connection being closed if it is unregistered and has no other
  connection up.

===========================================================================*/
LOCAL void bt_l2_conn_closed_init_reg_table_entry_if_ok
(
  uint8   ci
)
{
  uint8                 i;
  bt_l2_reg_table_type  *reg_ptr=NULL;

  for ( i=0; i< BT_L2_MAX_CONN; ++i )
  {
    if ( ( bt_l2_conn_db[i].app_id == bt_l2_conn_db[ci].app_id ) &&
         ( bt_l2_conn_db[i].state != BT_L2_CLOSED ) &&
         ( i != ci ) )
    {
      break;
    }
  }
  if ( i == BT_L2_MAX_CONN )
  {
    reg_ptr = bt_l2_find_reg_table_entry_app_id( bt_l2_conn_db[ci].app_id );
    if ( (reg_ptr != NULL ) && ( reg_ptr->registered == FALSE ) )
    {
      bt_l2_init_reg_table_entry( reg_ptr );
    }
  }
}

/*===========================================================================

FUNCTION
  bt_l2_is_acl_link_in_use

DESCRIPTION
  Returns TRUE if the ACL link, specified by the ci (L2CAP connection table
  index), is used for another L2CAP connection.

===========================================================================*/
LOCAL boolean bt_l2_is_acl_link_in_use
(
  uint8 ci
)
{
  uint8    i;
  boolean  another_conn_up = FALSE;

  /* Search conn table for another l2cap connection. */
  for ( i = 0; i < BT_L2_MAX_CONN; i++ )
  {
    if ( ( i != ci ) &&
         ( bt_l2_conn_db[ i ].state != BT_L2_CLOSED ) &&
         ( BT_BD_ADDRS_EQUAL( &( bt_l2_conn_db[ ci ].bd_addr ),
                              &( bt_l2_conn_db[ i ].bd_addr ) ) ) )
    {
      another_conn_up = TRUE;
    }
  }

  return another_conn_up;
}

/*===========================================================================

FUNCTION
  bt_l2_conn_closed_disconn_acl_link_if_ok

DESCRIPTION
  Disconnects the ACL link corresponding to the connection being closed if
  all other L2 channels on that ACL link have their state marked as CLOSED.

===========================================================================*/
LOCAL void bt_l2_conn_closed_disconn_acl_link_if_ok
(
  uint8   ci,
  bt_event_reason_type  disconn_reason
)
{
  /* Is another L2CAP connection using this ACL link? */
  if ( bt_l2_conn_db[ ci ].echo_app_id != BT_APP_ID_NULL )
  {
    /* The ACL link is in use if we're waiting for an echo response */
    bt_l2_conn_db[ ci ].state          = BT_L2_ACL_LINK_CONNECTED;    
  }
  else if ( bt_l2_is_acl_link_in_use( ci ) == FALSE )
  {
    if ( bt_l2_conn_db[ ci ].state == BT_L2_W4_ACL_LINK )
    {
      /* This is the only requested connection to this device, and the ACL
       * isn't up yet. Cancel the page */
      BT_BDA( MSG_DEBUG, "BT L2 CMD TX: Connect Cancel Addr",
              &bt_l2_conn_db[ ci ].bd_addr );
      bt_cmd_rm_connect_cancel( bt_l2_app_id,
                                &bt_l2_conn_db[ ci ].bd_addr );
    }
    else if ( bt_l2_conn_db[ ci ].disconn_acl_immediate == TRUE )
    {
      /* We've been instructed not to wait for the ACL disconnect
       * timer. Tear down the acl link now */
      bt_rm_disc_reason_type rm_disconn_reason;
      rm_disconn_reason = 
        bt_l2_convert_disconn_reason_to_rm( bt_l2_conn_db[ ci ].disconn_reason );
      bt_l2_conn_db[ ci ].state                 = BT_L2_CLOSED;
      bt_l2_conn_db[ ci ].disconn_acl_immediate = FALSE;
      bt_l2_conn_db[ ci ].idle_timeout          = 0;
      
      BT_MSG_API( "BT L2 CMD TX: RM Disc Conn, h %x r %x",
                  bt_l2_conn_db[ ci ].conn_hndl,
                  rm_disconn_reason,
                  0 );
      bt_cmd_rm_disconnect_acl( bt_l2_app_id,
                                bt_l2_conn_db[ ci ].conn_hndl,
                                rm_disconn_reason );
      bt_l2_conn_closed_init_reg_table_entry_if_ok(ci);
      
    }
    else
    {
      bt_l2_conn_db[ ci ].state          = BT_L2_ACL_LINK_CONNECTED;
      /* Tear down the ACL only if we initiated this L2CAP link, or if
         we initiated the disconnection. Do not tear down the link for
         remote initiated L2CAP links which the remote closes */
      if( bt_l2_conn_db[ ci ].initiated_conn == TRUE ||
          bt_l2_conn_db[ ci ].initiated_disconn == TRUE )
      {
        /* Set timer to disconnect ACL */        
        bt_l2_conn_db[ ci ].disconn_reason = disconn_reason;
        (void)rex_set_timer( &bt_l2_conn_db[ ci ].acl_link_disconnect_timer,
                             BT_L2_ACL_LINK_DISCONNECT_TIME );
      }
      else
      {
        /* Start the idle timer to disconnect the link if the remote device
         * doesn't do so */
        bt_l2_conn_db[ ci ].idle_timeout = BT_L2_IDLE_TIMEOUT_MS;
      }
    }
  }
}

/*===========================================================================

FUNCTION
  bt_l2_close_l2_connection

DESCRIPTION
  Marks the L2 channel as closed. Then frees up L2CAP registration entry if
  necessary. After that the ACL link is disconnected if ok.

===========================================================================*/
LOCAL void bt_l2_close_l2_connection
(
  bt_l2_conn_db_type  *cdb_ptr,
  bt_event_reason_type  disconn_reason
)
{
  uint8                 i,j;
  bt_l2_sig_pkt_type*  pkt_ptr;

  cdb_ptr->state = BT_L2_CLOSED;
  cdb_ptr->idle_timeout = 0;
  for ( i=0; i< BT_L2_MAX_CONN; ++i )
  {
    if ( &bt_l2_conn_db[i] == cdb_ptr )
      break;
  }

  if ( i < BT_L2_MAX_CONN )
  {
    bt_l2_conn_closed_disconn_acl_link_if_ok(i, disconn_reason);
    bt_l2_conn_closed_init_reg_table_entry_if_ok(i);

    if( bt_l2_conn_db[i].state == BT_L2_CLOSED )
    {
      bt_l2_init_conn_db_entry( &bt_l2_conn_db[i] );
    }
    /* If ACL link is still connected, clear outstanding signaling pkts and 
       corresponding timers */
    else 
    {
      for (j=0; j<BT_L2_MAX_PENDING_RQSTS; j++)
      {
        if ( cdb_ptr->outstanding_rqst_ptr[j] != NULL )
        {
          pkt_ptr = cdb_ptr->outstanding_rqst_ptr[j];
          bt_l2_free_signalling_pkt( pkt_ptr );
          cdb_ptr->outstanding_rqst_ptr[j] = NULL;
        }
      }
      cdb_ptr->idle_timeout = 0;
      (void)rex_clr_timer( &cdb_ptr->config_req_wait_timer );
      (void)rex_clr_timer( &cdb_ptr->total_config_process_timer );
    }
  }
}


/*===========================================================================

FUNCTION
  bt_l2_find_matching_rqst

DESCRIPTION

===========================================================================*/
LOCAL byte bt_l2_find_matching_rqst
(
  byte  conn_idx,
  byte  matching_id
)
{

  byte  i;

  for ( i=0; i < BT_L2_MAX_PENDING_RQSTS; i++ )
  {
    bt_l2_sig_pkt_type *pkt_ptr =
      bt_l2_conn_db[ conn_idx ].outstanding_rqst_ptr[ i ];

    if ( (pkt_ptr != NULL) && ( pkt_ptr->cmd_hdr.matching_id == matching_id ) )
    {
      break;
    }
  }

  return( i );

}


/*===========================================================================

FUNCTION
  bt_l2_find_free_reg_table_entry

DESCRIPTION
  Finds the first free entry in the L2CAP registration table and
  sets it up for the specified PSM and BT application ID.

RETURN VALUE
  Returns a pointer to the entry if found, otherwise NULL.

===========================================================================*/
LOCAL bt_l2_reg_table_type* bt_l2_find_free_reg_table_entry
(
  bt_app_id_type  bt_app_id,
  uint16          psm
)
{

  uint16                 i = 0;
  bt_l2_reg_table_type*  reg_ptr = NULL;

  while ( (reg_ptr == NULL) && (i < BT_L2_MAX_PSM_COUNT) )
  {
    if ( bt_l2_reg_table[ i ].psm == BT_L2_PSM_INVALID )
    {
      reg_ptr            = &bt_l2_reg_table[ i ];
      reg_ptr->psm       = psm;
      reg_ptr->bt_app_id = bt_app_id;
    }
    i++;
  }

  return( reg_ptr );

}



/*===========================================================================

FUNCTION
  bt_l2_find_reg_table_entry_app_id

DESCRIPTION
  Searches for the L2CAP registration table for a specified App id.

RETURN VALUE
  Returns a pointer to the entry if found, otherwise NULL.

===========================================================================*/
LOCAL bt_l2_reg_table_type* bt_l2_find_reg_table_entry_app_id
(
  bt_app_id_type  app_id
)
{

  uint16                 i = 0;
  bt_l2_reg_table_type*  reg_ptr = NULL;

  while ( (reg_ptr == NULL) && (i < BT_L2_MAX_PSM_COUNT) )
  {
    if ( bt_l2_reg_table[ i ].bt_app_id == app_id )
    {
      reg_ptr = &bt_l2_reg_table[ i ];
    }
    i++;
  }

  return( reg_ptr );

}

/*===========================================================================

FUNCTION
  bt_l2_find_reg_table_entry_psm

DESCRIPTION
  Searches for the L2CAP registration table for a specified PSM.

RETURN VALUE
  Returns a pointer to the entry if found, otherwise NULL.

===========================================================================*/
LOCAL bt_l2_reg_table_type* bt_l2_find_reg_table_entry_psm
(
  uint16  psm
)
{

  uint16                 i = 0;
  bt_l2_reg_table_type*  reg_ptr = NULL;

  while ( (reg_ptr == NULL) && (i < BT_L2_MAX_PSM_COUNT) )
  {
    if ( bt_l2_reg_table[ i ].psm == psm )
    {
      reg_ptr = &bt_l2_reg_table[ i ];
    }
    i++;
  }

  return( reg_ptr );

}

/*===========================================================================

FUNCTION
  bt_l2_is_psm_registered

DESCRIPTION
  Returns TRUE if the specified PSM has an entry in registration table and
  is registered, FALSE otherwise.

RETURN VALUE
  Returns TRUE or FALSE.

===========================================================================*/
LOCAL boolean bt_l2_is_psm_registered
(
  uint16  psm
)
{

  uint16                 i = 0;

  for ( i=0; i < BT_L2_MAX_PSM_COUNT; ++i )
  {
    if ( bt_l2_reg_table[ i ].psm == psm &&
         bt_l2_reg_table[ i ].registered == TRUE )
    {
      return( TRUE );
    }
  }

  return( FALSE );

}

/*===========================================================================

FUNCTION
  bt_l2_get_psm_from_reg_app_id

DESCRIPTION
  Returns psm of the app if it has entry in the registration table.

RETURN VALUE
  Returns psm.

===========================================================================*/
LOCAL uint16 bt_l2_get_psm_from_reg_app_id
(
  uint16  app_id
)
{

  uint16                 i = 0;

  for ( i=0; i < BT_L2_MAX_PSM_COUNT; ++i )
  {
    if ( bt_l2_reg_table[ i ].psm != BT_L2_PSM_INVALID &&
         bt_l2_reg_table[ i ].bt_app_id == app_id )
    {
      return( bt_l2_reg_table[i].psm );
    }
  }

  return( BT_L2_PSM_INVALID );
}

/*===========================================================================

FUNCTION
  bt_l2_get_free_conn_entry

DESCRIPTION

===========================================================================*/
LOCAL bt_l2_conn_db_type* bt_l2_get_free_conn_entry( void )
{

  uint16  ci;

  for ( ci = 0; ci < BT_L2_MAX_CONN; ci++ )
  {
    if ( bt_l2_conn_db[ ci ].state == BT_L2_CLOSED )
    {
      bt_l2_init_conn_db_entry( &bt_l2_conn_db[ ci ] );
      return( &bt_l2_conn_db[ ci ] );
    }
  }

  return( NULL );

}

/*===========================================================================

FUNCTION
  bt_l2_get_conn_entry_from_cid

DESCRIPTION
  Finds a connection db entry for a specified CID.

RETURN VALUE
  Returns a pointer to the connection db entry if found, otherwise NULL.

===========================================================================*/
LOCAL bt_l2_conn_db_type* bt_l2_get_conn_entry_from_cid
(
  uint16  local_cid
)
{

  uint16  ci;

  for ( ci = 0; ci < BT_L2_MAX_CONN; ci++ )
  {
    if ( (bt_l2_conn_db[ ci ].local_cid == local_cid) &&
         (bt_l2_conn_db[ ci ].state > BT_L2_ACL_LINK_CONNECTED) )
    {
      return( &bt_l2_conn_db[ ci ] );
    }
  }

  return( NULL );

}


/*===========================================================================

FUNCTION
  bt_l2_get_conn_entry_from_conn_hndl

DESCRIPTION
  Finds a connection db entry for a specified conn_hndl.

RETURN VALUE
  Returns a pointer to the connection db entry if found, otherwise NULL.

===========================================================================*/
LOCAL bt_l2_conn_db_type* bt_l2_get_conn_entry_from_conn_hndl
(
  uint16  conn_hndl
)
{

  uint16  ci;

  for ( ci = 0; ci < BT_L2_MAX_CONN; ci++ )
  {
    if ( ( bt_l2_conn_db[ ci ].conn_hndl == conn_hndl ) &&
         ( bt_l2_conn_db[ ci ].state != BT_L2_CLOSED ) )
    {
      return( &bt_l2_conn_db[ ci ] );
    }
  }

  return( NULL );

}

/*===========================================================================

FUNCTION
  bt_l2_get_free_signalling_pkt

DESCRIPTION

===========================================================================*/
bt_l2_sig_pkt_type* bt_l2_get_free_signalling_pkt( void )
{

  bt_l2_sig_pkt_type*  pkt_ptr = NULL;
  byte                 i;

  for (i=0; i<BT_L2_MAX_SIGNALLING_PKTS; i++)
  {
    if (bt_l2_sig_pkts[i].pkt_hdr.length == 0)
    {
      pkt_ptr = &bt_l2_sig_pkts[i];
      break;
    }
  }

  if (i == BT_L2_MAX_SIGNALLING_PKTS )
  {
    BT_ERR("BTL2: Out of signalling packets!",0,0,0);
  }
  return( pkt_ptr );

}

/*===========================================================================

FUNCTION
  bt_l2_send_l2cap_pkt

DESCRIPTION

===========================================================================*/
LOCAL void bt_l2_send_l2cap_pkt
(
  dsm_item_type*       dsm_ptr,
  bt_l2_conn_db_type*  cdb_ptr,
  void*                frame_handle,
  bt_hc_pkt_boundary_flag_type     l2cap_pkt_start
)
{

#ifdef BT_TEST_PYLD_FAST_TX
  bt_cmd_msg_type  bt_command;
#endif

  // Send command to send data
  BT_MSG_API_PYLD( "BT L2 CMD TX: RM TXDA hndl %x S %x Len %x",
                   cdb_ptr->conn_hndl, l2cap_pkt_start,
                   dsm_length_packet( dsm_ptr ) );
  cdb_ptr->idle_timeout = 0;

#ifdef BT_TEST_PYLD_FAST_TX
  bt_command.cmd_hdr.bt_app_id = bt_l2_app_id;
  bt_command.cmd_hdr.cmd_type  = BT_CMD_RM_TX_DATA_ACL;

  bt_command.cmd_msg.cmd_rm_txda.handle          = cdb_ptr->conn_hndl;
  bt_command.cmd_msg.cmd_rm_txda.local_cid       = cdb_ptr->local_cid;
  bt_command.cmd_msg.cmd_rm_txda.l2cap_pkt_start = l2cap_pkt_start;
  bt_command.cmd_msg.cmd_rm_txda.flush_timeout   = cdb_ptr->out_flush_to;
  bt_command.cmd_msg.cmd_rm_txda.dsm_ptr         = dsm_ptr;
  bt_command.cmd_msg.cmd_rm_txda.frame_handle    = frame_handle;
  bt_rm_cmd_tx_data_acl( &bt_command );
#else
  bt_cmd_rm_tx_data_acl( bt_l2_app_id,
                         cdb_ptr->conn_hndl,
                         l2cap_pkt_start,
                         cdb_ptr->out_flush_to,
                         dsm_ptr );
#endif

}

/*===========================================================================

FUNCTION
  bt_l2_add_pending_rqst

DESCRIPTION
  This function adds the given packet pointer to the given connection
  in the connection database.

===========================================================================*/
LOCAL void bt_l2_add_pending_rqst
(
  bt_l2_sig_pkt_type*  pkt_ptr,
  bt_l2_conn_db_type*  cdb_ptr
)
{

  byte  i;

  for (i=0; i<BT_L2_MAX_PENDING_RQSTS; i++)
  {
    if (cdb_ptr->outstanding_rqst_ptr[i] == NULL)
    {
      cdb_ptr->outstanding_rqst_ptr[i] = pkt_ptr;
      pkt_ptr->rtx_timer_in_use = TRUE;
      (void)rex_set_timer( &pkt_ptr->expected_resp_timer,
                           pkt_ptr->rtx_timer_value );
      break;

    }  // end if an unused outstanding_rqst_ptr found
  }  // end for loop

  if ( i == BT_L2_MAX_PENDING_RQSTS )
  {
    BT_ERR( "BT L2: pending request q full", 0, 0, 0 );
    bt_l2_free_signalling_pkt(pkt_ptr);
  }

}

/*===========================================================================

FUNCTION
  bt_l2_free_signalling_pkt

DESCRIPTION

===========================================================================*/
LOCAL void bt_l2_free_signalling_pkt( bt_l2_sig_pkt_type*  pkt_ptr )
{
  pkt_ptr->pkt_hdr.length = 0;
  pkt_ptr->rtx_timer_in_use = FALSE;
  pkt_ptr->rtx_timer_value = BT_L2_DEFAULT_INITIAL_RTX_TIMER_VALUE;
  pkt_ptr->ertx_timer_value = BT_L2_DEFAULT_INITIAL_ERTX_TIMER_VALUE;
  pkt_ptr->total_rtx_wait_time = 0;
  (void)rex_clr_timer( &pkt_ptr->expected_resp_timer );
}

/*===========================================================================

FUNCTION
  bt_l2_get_next_matching_id

DESCRIPTION
  Returns next L2CAP signalling identifier (range: 0x01 to 0xFF)

===========================================================================*/
LOCAL byte bt_l2_get_next_matching_id( )
{
  bt_l2_next_matching_id++;
  if ( bt_l2_next_matching_id == 0 ) 
  {
    bt_l2_next_matching_id++;
  }

  return bt_l2_next_matching_id;
}

/*===========================================================================

FUNCTION
  bt_l2_send_signalling_pkt

DESCRIPTION
  Given a command code and index into bt_l2_conn_db table, this function
  builds an L2CAP signalling packet using the information from
  bt_l2_conn_db and sends

===========================================================================*/
LOCAL void bt_l2_send_signalling_pkt
(
  bt_l2_sig_cmd_type   cmd,
  bt_l2_sig_pkt_type*  pkt_ptr,
  bt_l2_conn_db_type*  cdb_ptr,
  boolean              msg_resend
)
{

  dsm_item_type*  dsm_ptr;
  uint16          length;
  uint16          bytes_pushed;

  switch ( cmd )
  {
    case BT_L2_SIG_CMD_REJECT:
      BT_MSG_SIG( "BT L2 TX: L2 Cmd Reject, hndl|id %08x lcid %x rsn %x",
                  (cdb_ptr->conn_hndl << 16) | (pkt_ptr->cmd_hdr.matching_id),
                  cdb_ptr->local_cid,
                  pkt_ptr->cmd.cmd_reject.reason );
      // nothing to do
      break;

    case BT_L2_SIG_CONNECTION_RQST:
      if ( msg_resend == TRUE )
      {
        pkt_ptr->cmd_hdr.matching_id  = cdb_ptr->last_conn_rqst_sent_matching_id;
      }
      else
      {
        pkt_ptr->cmd_hdr.matching_id  = bt_l2_get_next_matching_id( );
      }
      cdb_ptr->last_conn_rqst_sent_matching_id = pkt_ptr->cmd_hdr.matching_id;
      pkt_ptr->cmd_hdr.length       = sizeof( bt_l2_conn_req_cmd_type );

      BT_MSG_SIG( "BT L2 TX: L2 Conn Req hndl|id %08x PSM %x lcid %x",
                  (cdb_ptr->conn_hndl << 16) | (pkt_ptr->cmd_hdr.matching_id),
                  pkt_ptr->cmd.conn_req.psm,
                  cdb_ptr->local_cid );
      break;

    case BT_L2_SIG_CONNECTION_RSP:
      BT_MSG_SIG( "BT L2 TX: L2 Conn Rsp hndl|id %08x lcid|rcid:%08x rslt|status %08x",
                  (cdb_ptr->conn_hndl << 16) | (pkt_ptr->cmd_hdr.matching_id),
                  (cdb_ptr->local_cid << 16) | (cdb_ptr->remote_cid),
                  (pkt_ptr->cmd.conn_rsp.result << 16) | (pkt_ptr->cmd.conn_rsp.status) );

      // build response packet to send to peer L2CAP
      pkt_ptr->cmd_hdr.length       = sizeof( bt_l2_conn_rsp_cmd_type );
      break;

    case BT_L2_SIG_CONFIGURE_RQST:
      if ( msg_resend == TRUE )
      {
        pkt_ptr->cmd_hdr.matching_id  = cdb_ptr->last_config_rqst_sent_matching_id;
      }
      else
      {
        pkt_ptr->cmd_hdr.matching_id  = bt_l2_get_next_matching_id( );
      }
      cdb_ptr->last_config_rqst_sent_matching_id = pkt_ptr->cmd_hdr.matching_id;

      BT_MSG_SIG( "BT L2 TX: L2 Cfg Req hndl|id %08x lcid|rcid:%08x flags %x",
                  (cdb_ptr->conn_hndl << 16) | (pkt_ptr->cmd_hdr.matching_id),
                  (cdb_ptr->local_cid<<16)|cdb_ptr->remote_cid,
                  pkt_ptr->cmd.config_req.flags );
      break;

    case BT_L2_SIG_CONFIGURE_RSP:
      BT_MSG_SIG( "BT L2 TX: L2 Cfg Rsp hndl|id %08x lcid|rcid:%08x st|rslt %08x",
                  (cdb_ptr->conn_hndl << 16) | (pkt_ptr->cmd_hdr.matching_id),
                  (cdb_ptr->local_cid << 16) | (cdb_ptr->remote_cid),
                  (cdb_ptr->state << 16)     | (pkt_ptr->cmd.config_rsp.result) );

      // nothing to do
      break;

    case BT_L2_SIG_DISCONNECTION_RQST:
      if ( cdb_ptr->state < BT_L2_OPEN || cdb_ptr->state == BT_L2_RECONFIG )
      {
        (void)rex_clr_timer( &cdb_ptr->config_req_wait_timer );
        (void)rex_clr_timer( &cdb_ptr->total_config_process_timer );
      }
      if ( msg_resend == TRUE )
      {
        pkt_ptr->cmd_hdr.matching_id  = cdb_ptr->last_disconn_rqst_sent_matching_id;
      }
      else
      {
        pkt_ptr->cmd_hdr.matching_id  = bt_l2_get_next_matching_id( );
      }
      cdb_ptr->last_disconn_rqst_sent_matching_id = pkt_ptr->cmd_hdr.matching_id;
      pkt_ptr->cmd_hdr.length       = sizeof( bt_l2_disconn_req_cmd_type );

      BT_MSG_SIG( "BT L2 TX: L2 Disc Req hndl|id %08x lcid %x rcid %x",
                  (cdb_ptr->conn_hndl << 16) | (pkt_ptr->cmd_hdr.matching_id),
                  cdb_ptr->local_cid,
                  cdb_ptr->remote_cid );
      break;

    case BT_L2_SIG_DISCONNECTION_RSP:
      BT_MSG_SIG( "BT L2 TX: L2 Disc Rsp hndl|id %08x lcid|rcid:%x st:%x",
                  (cdb_ptr->conn_hndl << 16) | (pkt_ptr->cmd_hdr.matching_id),
                  (cdb_ptr->local_cid << 16) | (cdb_ptr->remote_cid),
                  cdb_ptr->state );

      pkt_ptr->cmd_hdr.length       = sizeof( bt_l2_disconn_rsp_cmd_type );
      break;

    case BT_L2_SIG_ECHO_RQST:
      if ( msg_resend == TRUE )
      {
        pkt_ptr->cmd_hdr.matching_id  = cdb_ptr->last_echo_rqst_sent_matching_id;
      }
      else
      {
        pkt_ptr->cmd_hdr.matching_id  = bt_l2_get_next_matching_id( );
      }
      cdb_ptr->last_echo_rqst_sent_matching_id = pkt_ptr->cmd_hdr.matching_id;

      BT_MSG_SIG( "BT L2 TX: L2 Echo Req hndl|id %08x lcid:%x rcid:%x",
                  (cdb_ptr->conn_hndl << 16) | (pkt_ptr->cmd_hdr.matching_id),
                  cdb_ptr->local_cid,
                  cdb_ptr->remote_cid );
      break;

    case BT_L2_SIG_ECHO_RSP:
      BT_MSG_SIG( "BT L2 TX: L2 Echo Rsp hndl|id %08x lcid:%x rcid:%x",
                  (cdb_ptr->conn_hndl << 16) | (pkt_ptr->cmd_hdr.matching_id),
                  cdb_ptr->local_cid,
                  cdb_ptr->remote_cid );
      // nothing to do
      break;

    case BT_L2_SIG_INFO_RSP:
      BT_MSG_SIG( "BT L2 TX: L2 Info Rsp hndl|id %08x lcid:%x rcid:%x",
                  (cdb_ptr->conn_hndl << 16) | (pkt_ptr->cmd_hdr.matching_id),
                  cdb_ptr->local_cid,
                  cdb_ptr->remote_cid );
      // nothing to do
      break;

    default:
      BT_ERR( "BT L2: bad sig hndl|id %08x cmd|id:%08x lcid|st:%x",
              ((cdb_ptr->conn_hndl << 16) | (pkt_ptr->cmd_hdr.matching_id)),
              ((cmd<<16)|(pkt_ptr->cmd_hdr.matching_id)),
              ((cdb_ptr->local_cid << 16) | (cdb_ptr->state)) );
      break;
  }

  pkt_ptr->cmd_hdr.command_code = (byte)cmd;
  pkt_ptr->pkt_hdr.channel_id   = BT_L2_SIGNALLING_CID;
  pkt_ptr->pkt_hdr.length       = pkt_ptr->cmd_hdr.length +
                                    sizeof( bt_l2_cmd_hdr_type );

  length = pkt_ptr->pkt_hdr.length + sizeof( bt_l2_hdr_type );
  dsm_ptr = bt_get_free_dsm_ptr( BT_TL_HCI_BB, length );

  BT_SET_PKT_TRACER( dsm_ptr, DSM_BT_TX_L2_SIG );

  bytes_pushed = dsm_pushdown_tail( &dsm_ptr, (void*)(&pkt_ptr->pkt_hdr),
                                    length, DSM_DS_SMALL_ITEM_POOL );
  if( bytes_pushed != length )
  {
    BT_ERR("BT L2: can't pushdown. req:%x actual:%x", length, bytes_pushed, 0);
  }

#ifdef BT_SWDEV_QXDM_LOG_MSG
  bt_l2_send_tx_signaling_log( dsm_ptr, pkt_ptr->pkt_hdr.channel_id );
#endif

  if ( cdb_ptr->state != BT_L2_CLOSED ||
       bt_l2_is_acl_link_in_use( ((cdb_ptr - bt_l2_conn_db)
                                  / sizeof(bt_l2_conn_db_type)) ) )
  {
    bt_l2_send_l2cap_pkt( dsm_ptr, cdb_ptr, NULL, BT_HC_PKT_BOUNDARY_START );
  }
  else
  {
    BT_ERR( "BT L2 bad state %x",
            cdb_ptr->state, 0, 0 ); // BT_TBD
    dsm_free_packet( &dsm_ptr );
  }

  if ( ( cmd == BT_L2_SIG_CONNECTION_RQST ) ||
       ( cmd == BT_L2_SIG_CONFIGURE_RQST ) ||
       ( cmd == BT_L2_SIG_DISCONNECTION_RQST ) ||
       ( cmd == BT_L2_SIG_ECHO_RQST ) )
  {
    /* Save pending signalling command if this is a request */
    bt_l2_add_pending_rqst( pkt_ptr, cdb_ptr );
  }
  else
  {
    bt_l2_free_signalling_pkt( pkt_ptr );
  }

}

/*===========================================================================

FUNCTION
  bt_l2_get_config_options

DESCRIPTION

===========================================================================*/
LOCAL bt_l2_config_rsp_result_type bt_l2_get_config_options
(
 bt_l2_config_option_type**  co_ptr_ptr,
 int32*                      total_option_len_ptr,
 word*                       mtu_ptr,
 word*                       flush_to_ptr,
 bt_l2_qos_type*             qos_ptr,
 byte*                       num_options_ptr,
 boolean*                    mtu_option_present_ptr,
 boolean*                    fto_option_present_ptr,
 boolean*                    qos_option_present_ptr
 )
{
  dsm_item_type*             rsp_ptr;
  bt_l2_config_rsp_result_type
  result = BT_L2_CONFIG_RSP_SUCCESS;  /* assume success */
  bt_l2_config_option_type*  local_co_ptr_ptr = *co_ptr_ptr;
  /* Local copy for DSM manipulation */

  rsp_ptr = NULL;
  *num_options_ptr = 0;
  
  while (*total_option_len_ptr >= 2)
  {
    switch ((local_co_ptr_ptr)->type & 0x7F)
    {
      case BT_L2_CONFIG_OPTION_MTU:
        /* In case remote side sends invalid length, send reject with no reason 
           with already processed parameters, so as to prevent stack corruption 
           in further processing
        */
        if( ((local_co_ptr_ptr)->length) != 2) 
        {
          goto ErrorInvalidPacket;
        }
        else if( (*total_option_len_ptr - 4) < 0 )
        {
          if( result != BT_L2_CONFIG_REJECTED_UNKNOWN_OPTION )
          {
            dsm_free_packet( &rsp_ptr );
            *num_options_ptr = 0;
            result = BT_L2_CONFIG_REJECTED_UNKNOWN_OPTION;
          }
          (*num_options_ptr)++;
          /* Build parsed data in rsp_ptr; Advance local_co_ptr_ptr to next item */
          bt_dsm_pushdown_tail( &rsp_ptr,
                                local_co_ptr_ptr,
                                MIN((local_co_ptr_ptr)->length+2, *total_option_len_ptr),
                                DSM_DS_SMALL_ITEM_POOL );
        }
        else if( result != BT_L2_CONFIG_REJECTED_UNKNOWN_OPTION  )
        {
          if( *((word*)((local_co_ptr_ptr)->option)) < BT_L2_MIN_MTU )
          {
            *((word*)((local_co_ptr_ptr)->option)) = BT_L2_DEFAULT_MTU;
            if ( result != BT_L2_CONFIG_REJECTED_UNACCEPTABLE_PARAMS )
            {
              dsm_free_packet( &rsp_ptr );
              *num_options_ptr = 0;
              result = BT_L2_CONFIG_REJECTED_UNACCEPTABLE_PARAMS;
            }
            (*num_options_ptr)++;
            /* Build parsed data in rsp_ptr; Advance local_co_ptr_ptr to next item */
            bt_dsm_pushdown_tail( &rsp_ptr,
                                  local_co_ptr_ptr,
                                  MIN((local_co_ptr_ptr)->length+2, *total_option_len_ptr),
                                  DSM_DS_SMALL_ITEM_POOL );
          }
          else if ( result != BT_L2_CONFIG_REJECTED_UNACCEPTABLE_PARAMS )
          {
            if( *((word*)((local_co_ptr_ptr)->option)) > BT_L2_MAX_MTU )
            {
              *((word*)((local_co_ptr_ptr)->option)) = BT_L2_MAX_MTU;            
            }
            *mtu_ptr = *((word*)((local_co_ptr_ptr)->option));
            *mtu_option_present_ptr = TRUE;
            /* In this case result = SUCCESS, hence no need for DSM pushdown */
          }
        }
        break;
      case BT_L2_CONFIG_OPTION_FLUSH_TIMEOUT:
        if( ((local_co_ptr_ptr)->length) != 2) /* Invalid length */
        {
          goto ErrorInvalidPacket;
        }
        else if( (*total_option_len_ptr - 4) < 0 )
        {
          if( result != BT_L2_CONFIG_REJECTED_UNKNOWN_OPTION )
          {
            dsm_free_packet( &rsp_ptr );
            *num_options_ptr = 0;
          }
          result = BT_L2_CONFIG_REJECTED_UNKNOWN_OPTION;
          (*num_options_ptr)++;
         /* Build parsed data in rsp_ptr; Advance local_co_ptr_ptr to next item */
          bt_dsm_pushdown_tail( &rsp_ptr,
                                local_co_ptr_ptr,
                                MIN((local_co_ptr_ptr)->length+2, *total_option_len_ptr),
                                DSM_DS_SMALL_ITEM_POOL );
        }
        else if( result != BT_L2_CONFIG_REJECTED_UNKNOWN_OPTION )
        {
          if( *((word*)((local_co_ptr_ptr)->option)) == 0 )
          {
            // The flushTO cannot be 0
            *((word*)((local_co_ptr_ptr)->option)) = BT_L2_DEFAULT_FLUSH_TIMEOUT;
            if( result != BT_L2_CONFIG_REJECTED_UNACCEPTABLE_PARAMS )
            {
              dsm_free_packet( &rsp_ptr );
              *num_options_ptr = 0;
              result = BT_L2_CONFIG_REJECTED_UNACCEPTABLE_PARAMS;
            }
            (*num_options_ptr)++;
            /* Build parsed data in rsp_ptr; Advance local_co_ptr_ptr to next item */
            bt_dsm_pushdown_tail( &rsp_ptr,
                                  local_co_ptr_ptr,
                                  MIN((local_co_ptr_ptr)->length+2, *total_option_len_ptr),
                                  DSM_DS_SMALL_ITEM_POOL );
          }
          else
          {
            *flush_to_ptr = *((word*)((local_co_ptr_ptr)->option));
            *fto_option_present_ptr = TRUE;
          }
        }

        break;
      case BT_L2_CONFIG_OPTION_QOS:
        if( ((local_co_ptr_ptr)->length) != sizeof(bt_l2_qos_type)) /* Invalid length */
        {
          goto ErrorInvalidPacket;
        }
        else if( (*total_option_len_ptr - (int32)sizeof(bt_l2_qos_type) - 2) < 0 )
        {
          if( result != BT_L2_CONFIG_REJECTED_UNKNOWN_OPTION )
          {
            dsm_free_packet( &rsp_ptr );
            *num_options_ptr = 0;
          }
          result = BT_L2_CONFIG_REJECTED_UNKNOWN_OPTION;
          (*num_options_ptr)++;
         /* Build parsed data in rsp_ptr; Advance local_co_ptr_ptr to next item */
          bt_dsm_pushdown_tail( &rsp_ptr,
                                local_co_ptr_ptr,
                                MIN((local_co_ptr_ptr)->length+2, *total_option_len_ptr),
                                DSM_DS_SMALL_ITEM_POOL );
        }
        else if( result != BT_L2_CONFIG_REJECTED_UNKNOWN_OPTION )
        {
          qos_ptr = (bt_l2_qos_type*)((local_co_ptr_ptr)->option);
          if( qos_ptr->service_type != BT_QOS_BEST_EFFORT )
          {
            qos_ptr->service_type      = BT_QOS_BEST_EFFORT;
            qos_ptr->token_rate        = 0;
            qos_ptr->token_bucket_size = 0;
            qos_ptr->peak_bandwidth    = 0;
            qos_ptr->latency           = 0xFFFFFFFF;
            qos_ptr->delay_variation   = 0xFFFFFFFF;
            if ( (((local_co_ptr_ptr)->type & 0x80) == 0)
                 && (result != BT_L2_CONFIG_REJECTED_UNACCEPTABLE_PARAMS) )
            {
              dsm_free_packet( &rsp_ptr );
              *num_options_ptr = 0;
              result = BT_L2_CONFIG_REJECTED_UNACCEPTABLE_PARAMS;
            }
            (*num_options_ptr)++;
            /* Build parsed data in rsp_ptr; Advance local_co_ptr_ptr to next item */
            bt_dsm_pushdown_tail( &rsp_ptr,
                                  local_co_ptr_ptr,
                                  MIN((local_co_ptr_ptr)->length+2, *total_option_len_ptr),
                                  DSM_DS_SMALL_ITEM_POOL );
          }
          else
          {
            *qos_ptr = *((bt_l2_qos_type*)((local_co_ptr_ptr)->option));
            *qos_option_present_ptr = TRUE;
          }
        }
        break;
      default: // unknown type
        if (((local_co_ptr_ptr)->type & 0x80) == 0)
        {
          // the MSB of type being 0 dictates
          // "option must be recognized;
          // refuse the configuration request"
          if( result != BT_L2_CONFIG_REJECTED_UNKNOWN_OPTION )
          {
            dsm_free_packet( &rsp_ptr );
            *num_options_ptr = 0;
          }
          result = BT_L2_CONFIG_REJECTED_UNKNOWN_OPTION;
          (*num_options_ptr)++;
          /* Build parsed data in rsp_ptr; Advance local_co_ptr_ptr to next item */
          bt_dsm_pushdown_tail( &rsp_ptr,
                                local_co_ptr_ptr,
                                MIN((local_co_ptr_ptr)->length+2, *total_option_len_ptr),
                                DSM_DS_SMALL_ITEM_POOL );
        }
        break;

    }  // end switch

    *total_option_len_ptr -= ((local_co_ptr_ptr)->length + 2);
    local_co_ptr_ptr = (bt_l2_config_option_type*)
      ((byte*) local_co_ptr_ptr + (local_co_ptr_ptr)->length + 2);
  }  // end while

  // make sure we've processed all of the options exactly
  if ( *total_option_len_ptr != 0 )
  {
    BT_MSG_HIGH( "BT L2: not all options processed. %x bytes remain",
                 *total_option_len_ptr, 0, 0 );
    goto ErrorInvalidPacket;
  }

  *total_option_len_ptr = dsm_length_packet( rsp_ptr );
  /* Copy back parsed data from rsp_ptr to *co_ptr_ptr */
  dsm_pullup( &rsp_ptr, *co_ptr_ptr, *total_option_len_ptr );

  return( result );

/* In case we receive invalid packet respond with NO_REASON with 
   already parsed parameters in response message */
ErrorInvalidPacket:

  result = BT_L2_CONFIG_REJECTED_NO_REASON;
  dsm_free_packet( &rsp_ptr );
  *num_options_ptr = 0;
  return( result );

}

/*===========================================================================

FUNCTION
  bt_l2_build_config_req_pkt

DESCRIPTION
  This function builds a configuration request packet based on
  information found in the given entry in the connection database.

===========================================================================*/
LOCAL byte bt_l2_build_config_req_pkt
(
  bt_l2_config_req_cmd_type*  crc_ptr,
  bt_l2_conn_db_type*         cdb_ptr,
  boolean                     no_options,
  word                        in_mtu,
  word                        out_flush_to,
  bt_l2_qos_type              out_qos
)
{

  byte                       total_option_len = 0;
  bt_l2_config_option_type*  co_ptr =
                               (bt_l2_config_option_type *)crc_ptr->data;

  crc_ptr->dest_cid = cdb_ptr->remote_cid;
  crc_ptr->flags    = 0;

  if ( no_options == TRUE )
  {
    return( total_option_len + 4 );   // 4 for dest_cid and flags
  }

  if ((cdb_ptr->state == BT_L2_CONFIG)||
      (in_mtu != cdb_ptr->in_mtu && cdb_ptr->state == BT_L2_RECONFIG))
  {
    co_ptr->type = BT_L2_CONFIG_OPTION_MTU;
    co_ptr->length = 2;
    *((word*)(co_ptr->option)) = in_mtu;

    total_option_len += 4;
    co_ptr = (bt_l2_config_option_type *) (crc_ptr->data + total_option_len);
  }
  cdb_ptr->in_mtu = in_mtu;

  if ((out_flush_to != BT_L2_DEFAULT_FLUSH_TIMEOUT &&
                                              cdb_ptr->state == BT_L2_CONFIG)||
      (out_flush_to != cdb_ptr->out_flush_to && cdb_ptr->state == BT_L2_RECONFIG))
  {
    co_ptr->type = BT_L2_CONFIG_OPTION_FLUSH_TIMEOUT;
    co_ptr->length = 2;
    *((word*)(co_ptr->option)) = out_flush_to;

    total_option_len += 4;
    co_ptr = (bt_l2_config_option_type *) (crc_ptr->data + total_option_len);
  }
  cdb_ptr->out_flush_to = out_flush_to;


  if (((memcmp((byte *)&out_qos, (byte *)&bt_l2_default_qos,
        sizeof(bt_l2_qos_type))!=0) && cdb_ptr->state == BT_L2_CONFIG) ||
      ((memcmp((byte *)&out_qos, (byte *)&cdb_ptr->out_qos,
        sizeof(bt_l2_qos_type))!=0) && cdb_ptr->state == BT_L2_RECONFIG))
  {
    co_ptr->type = BT_L2_CONFIG_OPTION_QOS;
    co_ptr->length = 22;
    memcpy( co_ptr->option, (byte*)&out_qos,
            sizeof (bt_l2_qos_type) );

    total_option_len += 24;
  }
  memcpy((byte *)&cdb_ptr->out_qos, (byte *)&out_qos,
         sizeof(bt_l2_default_qos));

  return( total_option_len + 4 );   // 4 for dest_cid and flags

}

/*===========================================================================

FUNCTION
  bt_l2_l2ca_config_rsp_internal

DESCRIPTION
  This function builds a L2CAP_ConfigRsp packet and sends it across
  the channel.

===========================================================================*/
LOCAL word bt_l2_l2ca_config_rsp_internal
(
  byte                          matching_id,
  word                          local_cid,
  bt_l2_config_rsp_result_type  resp,
  byte                          num_options,
  bt_l2_config_option_type*     in_co_ptr,
  word                          flags
)
{

  word                 result = (word)BT_L2_CONFIG_REJECTED_INVALID_CID;
  byte                 total_option_len = 0;
  bt_l2_conn_db_type*  cdb_ptr = NULL;
  byte                 ci;

  for (ci=0; ci<BT_L2_MAX_CONN; ci++)
  {
    if ((bt_l2_conn_db[ci].local_cid == local_cid) &&
        ((bt_l2_conn_db[ci].state == BT_L2_CONFIG) ||
         (bt_l2_conn_db[ci].state == BT_L2_RECONFIG)))
    {
      cdb_ptr = &bt_l2_conn_db[ci];
      break;

    }
  }

  if (cdb_ptr != NULL)
  {
    bt_l2_sig_pkt_type *pkt_ptr = bt_l2_get_free_signalling_pkt();

    if ( pkt_ptr != NULL )
    {
      bt_l2_config_option_type *out_co_ptr =
        (bt_l2_config_option_type*) pkt_ptr->cmd.config_rsp.data;

      pkt_ptr->cmd.config_rsp.source_cid = cdb_ptr->remote_cid; // is this right? TBD
      pkt_ptr->cmd.config_rsp.flags = flags;
      pkt_ptr->cmd.config_rsp.result = (word)resp;

      while ( (num_options-- > 0) && (total_option_len < sizeof(pkt_ptr->cmd)) )
      {
        out_co_ptr->type = in_co_ptr->type;
        out_co_ptr->length = MIN(in_co_ptr->length,sizeof(pkt_ptr->cmd)-total_option_len);
        memcpy (out_co_ptr->option, in_co_ptr->option,
                out_co_ptr->length );
        /* Update the length to be processed */
        total_option_len += out_co_ptr->length + 2; // 1 for type, 1 for length
        out_co_ptr = (bt_l2_config_option_type*) ((byte*)out_co_ptr +
                                              out_co_ptr->length + 2);
        in_co_ptr = (bt_l2_config_option_type*) ((byte*)in_co_ptr +
                                             in_co_ptr->length + 2);
      }

      pkt_ptr->cmd_hdr.matching_id = matching_id;
      pkt_ptr->cmd_hdr.length = total_option_len + 6; // 2 for Source CID,
                                                      // 2 for flags,
                                                      // 2 for Result

      bt_l2_send_signalling_pkt (BT_L2_SIG_CONFIGURE_RSP, pkt_ptr, cdb_ptr,
                                 FALSE);
    }
    else
    {
      // No free signalling packets. Disconnect link
      bt_l2_close_l2_connection( cdb_ptr, BT_EVR_L2_UNSPECIFIED_ERROR );
    }

    if ( resp == BT_L2_CONFIG_RSP_SUCCESS )
    {
      result = (word)BT_L2_CONFIG_RSP_SUCCESS;
      if ( !BT_L2_IS_C_FLAG_SET(flags) )
      {
        cdb_ptr->config_rsp_state = BT_L2_CONFIG_END;
      }
    }
    else
    {
      cdb_ptr->config_rsp_state = BT_L2_CONFIG_START;
    }
  }
  else
  {
    // Can't find entry in CDB!
    BT_ERR( "BT L2: cfg rsp, can't find entry for lcid %x",
            local_cid, 0, 0 );
  }

  return( result );

}

/*===========================================================================

FUNCTION
  bt_l2_send_l2ca_config_cfm

DESCRIPTION
  | code=BT_L2_EC_CONFIG_CFM | length | mtu | flush timeout | qos |

===========================================================================*/
LOCAL boolean bt_l2_send_l2ca_config_cfm
(
  word             lcid,
  word             result,
  word             in_mtu,
  word             flush_to,
  bt_l2_qos_type*  qos_ptr
)
{

  boolean                   success = FALSE;
  bt_l2_conn_db_type*       cdb_ptr;
  bt_ev_msg_type            ev_l2_conn;
  bt_l2_reg_table_type      *reg_ptr = NULL;

  cdb_ptr = bt_l2_get_conn_entry_from_cid( lcid );

  if ( cdb_ptr != NULL )
  {
    ev_l2_conn.ev_hdr.bt_app_id = cdb_ptr->app_id;
    if ( result == (word)BT_L2_CONFIG_RSP_SUCCESS )
    {
      success = TRUE;

      if ( ( cdb_ptr->config_req_state == BT_L2_CONFIG_END ) &&
           ( cdb_ptr->config_rsp_state == BT_L2_CONFIG_END ) )
      {
        // This finishes our configuration stage, let's notify upper layer.
        switch( cdb_ptr->state )
        {
          case BT_L2_CONFIG:
            cdb_ptr->last_state_before_open = BT_L2_CONFIG;
            ev_l2_conn.ev_hdr.ev_type = BT_EV_L2_CONNECTED;
            memcpy( (byte*)(&ev_l2_conn.ev_msg.ev_l2_conn.bd_addr),
                    (byte*)(&cdb_ptr->bd_addr),
                    sizeof(bt_bd_addr_type) );
            ev_l2_conn.ev_msg.ev_l2_conn.cid = lcid;
            ev_l2_conn.ev_msg.ev_l2_conn.mtu = cdb_ptr->out_mtu;

            BT_MSG_API( "BT L2 EV TX: Conn lcid %x rcid %x mtu %x",
                         lcid, cdb_ptr->remote_cid, cdb_ptr->out_mtu );
            bt_ec_send_event( &ev_l2_conn );
            if ( cdb_ptr->initiated_conn == TRUE )
            {
              bt_l2_outgoing_conn_count++;
            }
            else
            {
              reg_ptr = bt_l2_find_reg_table_entry_app_id( cdb_ptr->app_id );
              if ( reg_ptr != NULL )
              {
                reg_ptr->incoming_conn_count++;
              }
            }
            break;
          case BT_L2_RECONFIG:
            cdb_ptr->last_state_before_open = BT_L2_RECONFIG;
            /* Send L2_RECONFIGURED event */
            ev_l2_conn.ev_hdr.ev_type = BT_EV_L2_RECONFIGURED;
            ev_l2_conn.ev_msg.ev_l2_reconfig.cid = lcid;
            ev_l2_conn.ev_msg.ev_l2_reconfig.reason = BT_EVR_GN_SUCCESS;
            ev_l2_conn.ev_msg.ev_l2_reconfig.out_mtu = cdb_ptr->out_mtu;
            ev_l2_conn.ev_msg.ev_l2_reconfig.in_flush_to = cdb_ptr->in_flush_to;
            ev_l2_conn.ev_msg.ev_l2_reconfig.in_qos = cdb_ptr->in_qos;

            BT_MSG_API( "BT L2 EV TX: Recfged lcid %x rcid %x mtu %x",
                         lcid, cdb_ptr->remote_cid, cdb_ptr->out_mtu );
            bt_ec_send_event( &ev_l2_conn );
            break;
          default:
            BT_ERR( "BT L2: lcid:%x Wrong state %x", lcid, cdb_ptr->state, 0 );
            break;
        }

        // done with configuration; reset states
        cdb_ptr->state = BT_L2_OPEN;
        cdb_ptr->idle_timeout     = BT_L2_IDLE_TIMEOUT_MS;
        cdb_ptr->config_req_state = BT_L2_CONFIG_START;
        cdb_ptr->config_rsp_state = BT_L2_CONFIG_START;
        (void)rex_clr_timer( &cdb_ptr->total_config_process_timer );
#ifdef BT_SWDEV_2_1_PB
        if( bt_soc_etc_data.lmp_version >= BT_SOC_LMP_VERSION_2_1 )
        {
          if( cdb_ptr->out_flush_to != BT_L2_DEFAULT_FLUSH_TIMEOUT )
          {
            bt_cmd_rm_set_auto_flush_to(cdb_ptr->out_flush_to, 
                                        cdb_ptr->conn_hndl);
          }
        }
#endif /* BT_SWDEV_2_1_PB */
      }
    }
    else
    {
      /* Send L2CAP Connection Failed event */
      ev_l2_conn.ev_hdr.ev_type   = BT_EV_L2_CONNECTION_FAILED;
      ev_l2_conn.ev_msg.ev_l2_conn_failed.bd_addr = cdb_ptr->bd_addr;
      ev_l2_conn.ev_msg.ev_l2_conn_failed.cid     = cdb_ptr->local_cid;
      ev_l2_conn.ev_msg.ev_l2_conn_failed.reason =
        BT_EVR_L2_CONFIG_PARAMS_NOT_AGREEABLE;

      BT_MSG_API( "BT L2 EV TX: Conn Failed lcid %x rcid %x reason %x",
                                lcid, cdb_ptr->remote_cid,
                                BT_EVR_L2_CONFIG_PARAMS_NOT_AGREEABLE );
      bt_ec_send_event( &ev_l2_conn );

      // Clear the connection entry
      // bt_l2_init_conn_db_entry( cdb_ptr );
      bt_l2_close_l2_connection( cdb_ptr, BT_EVR_L2_NORMAL_DISCONNECT );
    }
  }
  else
  {
    BT_ERR( "BT L2: lcid(%x) not found", lcid, 0, 0 );
  }

  if ( success == FALSE )
  {
    BT_ERR( "BT L2: lcid:%x rslt:%x", lcid, result, 0 );
  }

  return( success );

}

/*===========================================================================

FUNCTION
  bt_l2_l2ca_connect_rsp

DESCRIPTION
  The upper protocol will call this function after receiving
  L2CA_ConnectInd.

===========================================================================*/
LOCAL word bt_l2_l2ca_connect_rsp
(
  bt_bd_addr_type*  bd_addr_ptr,
  byte              matching_id,
  word              local_cid,
  word              resp,
  word              status
)
{

  word                      result = (word)BT_L2_CONN_RSP_FAILED;  // assume failure
  bt_l2_conn_db_type*       cdb_ptr = NULL;
  bt_l2_sig_pkt_type*       rsp_pkt_ptr = NULL;
  byte                      ci;


  for (ci=0; ci<BT_L2_MAX_CONN; ci++)
  {
    if ((bt_l2_conn_db[ci].local_cid == local_cid) &&
        (bt_l2_conn_db[ci].state == BT_L2_W4_L2CA_CONNECT_RSP))
    {
      cdb_ptr = &bt_l2_conn_db[ci];
      rsp_pkt_ptr = bt_l2_get_free_signalling_pkt();
      break;
    }  // end if entry found
  }  // end for loop

  if (rsp_pkt_ptr != NULL && cdb_ptr != NULL)
  {
    rsp_pkt_ptr->cmd.conn_rsp.dest_cid   = cdb_ptr->local_cid;
    rsp_pkt_ptr->cmd.conn_rsp.source_cid = cdb_ptr->remote_cid;
    rsp_pkt_ptr->cmd.conn_rsp.result     = resp;
    rsp_pkt_ptr->cmd.conn_rsp.status     = status;
    rsp_pkt_ptr->cmd_hdr.matching_id     = matching_id;

    bt_l2_send_signalling_pkt (BT_L2_SIG_CONNECTION_RSP, rsp_pkt_ptr, cdb_ptr,
                               FALSE);

    result = (word)BT_L2_CONN_RSP_SUCCESS;

    if (resp == (word)BT_L2_CONN_REQ_SUCCESS) // connection successful
    {
      cdb_ptr->state = BT_L2_CONFIG;   // enter config state
      cdb_ptr->config_req_state = BT_L2_CONFIG_START;
      cdb_ptr->config_rsp_state = BT_L2_CONFIG_START;
      cdb_ptr->idle_timeout = 0;
      (void)rex_set_timer( &cdb_ptr->total_config_process_timer,
                           BT_L2_MAX_TOTAL_CONFIG_PROCESS_TIME );
    }

    else if (BT_L2_CONN_REFUSED(resp))
    {
#if 0
      ev_l2_conn_failed.ev_hdr.bt_app_id = cdb_ptr->app_id;
      /* Send L2CAP Connection Failed event */
      /* Do we really need this event? The connection was not initiated by us */
      ev_l2_conn_failed.ev_hdr.ev_type   = BT_EV_L2_CONNECTION_FAILED;
      ev_l2_conn_failed.ev_msg.ev_l2_conn_failed.bd_addr = cdb_ptr->bd_addr;
      ev_l2_conn_failed.ev_msg.ev_l2_conn_failed.cid     = cdb_ptr->local_cid;
      ev_l2_conn_failed.ev_msg.ev_l2_conn_failed.reason =
        BT_EVR_L2_LOCAL_REJECT_CONNECTION;

      BT_MSG_API( "BT L2 EV TX: L2 Conn Failed lcid %x rcid %x reason %x",
                  cdb_ptr->local_cid, cdb_ptr->remote_cid,
                  BT_EVR_L2_LOCAL_REJECT_CONNECTION );
      bt_ec_send_event( &ev_l2_conn_failed );
#endif
      // upper layer refused connection request, clear the entry
      // bt_l2_init_conn_db_entry(cdb_ptr);
      bt_l2_close_l2_connection( cdb_ptr, BT_EVR_L2_LOCAL_REJECT_CONNECTION );
    }
  }  // end if entry found
  else
  {
    // No free signalling packets. Disconnect link
    bt_l2_close_l2_connection( cdb_ptr, BT_EVR_L2_UNSPECIFIED_ERROR );
  }

  return( result );
}

/*===========================================================================

FUNCTION
  bt_l2_l2ca_config_req

DESCRIPTION
  Called by upper protocol to request the (re-)configuration of a channel
  to a new set of parameters.

===========================================================================*/
LOCAL void bt_l2_l2ca_config_req
(
  word            local_cid,
  word            in_mtu,
  bt_l2_qos_type  out_flow,
  word            out_flush_to,
  word            link_to
)
{

  word                 result  = (word)BT_L2_CONFIG_FAILED_INVALID_CID;
  bt_l2_conn_db_type*  cdb_ptr = NULL;
  bt_l2_sig_pkt_type*  pkt_ptr = NULL;

  cdb_ptr = bt_l2_get_conn_entry_from_cid( local_cid );

  if (cdb_ptr != NULL) // entry found?
  {
    switch ( cdb_ptr->state )
    {
      case BT_L2_OPEN:
        // suspend data transmission at a convenient point,
        // but how? TBD

        cdb_ptr->state = BT_L2_RECONFIG;

        // then fall through to ...

      case BT_L2_CONFIG:
      case BT_L2_RECONFIG:
        // send peer ConfigReq

        if (in_mtu < BT_L2_MIN_MTU)
        {
          result = (word)BT_L2_CONFIG_FAILED_UNACCEPTABLE_PARAMS;
        }
        else {
#if 0
          memcpy( (byte*)&cdb_ptr->out_qos, (byte*)&out_flow,
                  sizeof (bt_l2_qos_type) );
          cdb_ptr->in_mtu   = in_mtu;
          cdb_ptr->link_to  = link_to;
#endif

          result = (word)BT_L2_CONFIG_FAILED_OTHER_REASON;
          pkt_ptr = bt_l2_get_free_signalling_pkt();
          if( pkt_ptr != NULL )
          {
            // the input out_flush_to == 0 means: request to use the existing
            // flush timeout value if one exists, otherwise the default
            // value will be used (page 299, BT Spec 1.0 A)
            if (out_flush_to == 0)
            {
              out_flush_to = cdb_ptr->out_flush_to;
            }

            pkt_ptr->cmd_hdr.length =
              bt_l2_build_config_req_pkt( &pkt_ptr->cmd.config_req, cdb_ptr,
                                          FALSE, in_mtu, out_flush_to, out_flow );

            // now, send config request packet to peer
            bt_l2_send_signalling_pkt( BT_L2_SIG_CONFIGURE_RQST,
                                       pkt_ptr, cdb_ptr, FALSE );

            result = (word)BT_L2_CONFIG_REQ_SUCCESS;
            cdb_ptr->config_req_state = BT_L2_RQST_SENT;
          }
          else
          {
            // No free signalling packets. Disconnect link
            bt_l2_close_l2_connection( cdb_ptr, BT_EVR_L2_UNSPECIFIED_ERROR );
          }

        }
        break;

      default:
        BT_ERR("BT L2 ca_config_req: invalid state %x", cdb_ptr->state,0,0);
        result = (word)BT_L2_CONFIG_FAILED_OTHER_REASON;
        break;
    }  // end switch
  }  // end if entry found

  if (result != (word)BT_L2_CONFIG_REQ_SUCCESS)
  {
    // send back config confirm negative event
    // not much we can do about an error result here:
    (void)bt_l2_send_l2ca_config_cfm( local_cid, result, in_mtu, out_flush_to,
                                      &out_flow );
  }

}

/*===========================================================================

FUNCTION
  bt_l2_l2ca_config_rsp

DESCRIPTION
 Called by upper protocol to respond to a configuration request event
 indication.

===========================================================================*/
LOCAL word bt_l2_l2ca_config_rsp
(
  byte            matching_id,
  word            local_cid,
  word            out_mtu,
  bt_l2_qos_type  in_flow,
  word            in_flush_to,
  word            flags
)
{

  word                       result = (word)BT_L2_CONFIG_RSP_SUCCESS;
  byte                       num_options = 0;
  byte                       config_option[sizeof(bt_l2_config_option_type)];
  bt_l2_config_option_type*  co_ptr =
                               (bt_l2_config_option_type*) config_option;
  bt_l2_conn_db_type*        cdb_ptr =
                               bt_l2_get_conn_entry_from_cid(local_cid);
  boolean                    mtu_valid = TRUE;
  boolean                    qos_valid = TRUE;

  if( cdb_ptr == NULL )
  {
    BT_ERR( "BT L2 ERR: local_cid(%x) not in DB", local_cid, 0, 0 );
    result = (word)BT_L2_CONFIG_REJECTED_INVALID_CID;
  }
  else
  {
    if ( in_flow.service_type == BT_QOS_GUARANTEED )
    {
      qos_valid = FALSE;
      result = (word)BT_L2_CONFIG_REJECTED_UNACCEPTABLE_PARAMS;
      in_flow = bt_l2_default_qos;
    }
    if ( out_mtu < BT_L2_MIN_MTU )
    {
      mtu_valid = FALSE;
      result = (word)BT_L2_CONFIG_REJECTED_UNACCEPTABLE_PARAMS;
      out_mtu = BT_L2_MIN_MTU;
    }

    if ( mtu_valid == TRUE )
    {
      cdb_ptr->out_mtu = out_mtu;
    }
    cdb_ptr->in_flush_to = in_flush_to;
    if ( qos_valid == TRUE )
    {
      memcpy( (byte*)&cdb_ptr->in_qos, (byte *)&in_flow, sizeof (bt_l2_qos_type) );
    }

    if ( result == (word)BT_L2_CONFIG_RSP_SUCCESS )
    {
      if ( BT_L2_IS_C_FLAG_SET( flags ) )
      {
        // This is a continuation response. Don't fill in anything here
        num_options = 0;
      }
      else
      {
        // Fill in the MTU in the response:
        if( out_mtu != BT_L2_DEFAULT_MTU )
        {
          co_ptr = (bt_l2_config_option_type*) config_option;
          co_ptr->type = BT_L2_CO_MTU;
          co_ptr->length = 2;
          *((word*)(co_ptr->option)) = out_mtu;
          num_options++;
          co_ptr = (bt_l2_config_option_type *) ((byte*)co_ptr + co_ptr->length + 2);
        }
      }
      BT_MSG_DEBUG( "BT L2: L2 internal Cfg Rsp lcid %x n_opts %x flags %x",
                    local_cid, num_options, flags );
      bt_l2_l2ca_config_rsp_internal (matching_id, local_cid,
                                      (bt_l2_config_rsp_result_type)result, num_options,
                                      (bt_l2_config_option_type *) config_option, flags);
    }
    else
    {
      num_options = 0;
      co_ptr = (bt_l2_config_option_type*) config_option;
      if ( mtu_valid == FALSE )
      {
        co_ptr->type = BT_L2_CO_MTU;
        co_ptr->length = 2;
        *((word*)(co_ptr->option)) = out_mtu;
        num_options++;
        co_ptr = (bt_l2_config_option_type *) ((byte*)co_ptr + co_ptr->length + 2);
      }
      if ( qos_valid == FALSE )
      {
        co_ptr->type = BT_L2_CO_QOS;
        co_ptr->length = sizeof (bt_l2_qos_type);
        memcpy (co_ptr->option, (byte*)&in_flow, sizeof (bt_l2_qos_type));
        num_options++;
      }
      bt_l2_l2ca_config_rsp_internal (matching_id, local_cid,
                                      (bt_l2_config_rsp_result_type)result, num_options, 
                                      (bt_l2_config_option_type *) config_option, flags);
    }
  }
  return( result );

}


/*===========================================================================

FUNCTION
  bt_l2_l2ca_disconnect_rsp

DESCRIPTION
  Called by upper protocol to request the disconnection of the given
  channel.

===========================================================================*/
LOCAL word bt_l2_l2ca_disconnect_rsp
(
  word  local_cid,
  byte  matching_id
)
{

  word                 result = 1;  // assume failure
  bt_l2_conn_db_type*  cdb_ptr = NULL;
  bt_l2_sig_pkt_type*  rsp_pkt_ptr = NULL;
  byte                 ci;

  for (ci=0; ci<BT_L2_MAX_CONN; ci++)
  {
    if ((bt_l2_conn_db[ci].local_cid == local_cid) &&
        (bt_l2_conn_db[ci].state == BT_L2_W4_L2CA_DISCONNECT_RSP))
    {
      cdb_ptr = &bt_l2_conn_db[ci];
      rsp_pkt_ptr = bt_l2_get_free_signalling_pkt();
      break;

    }
  }

  if (rsp_pkt_ptr != NULL && cdb_ptr != NULL)
  {
    bt_l2_disconn_rsp_cmd_type *disconn_rsp_ptr = &rsp_pkt_ptr->cmd.disconn_rsp;

    disconn_rsp_ptr->dest_cid        = cdb_ptr->local_cid;
    disconn_rsp_ptr->source_cid      = cdb_ptr->remote_cid;
    rsp_pkt_ptr->cmd_hdr.matching_id = matching_id;

    BT_MSG_SIG( "BT L2 TX: L2 Disconn Rsp lcid %x rcid %x",
                local_cid, cdb_ptr->remote_cid, 0 );

    bt_l2_send_signalling_pkt ( BT_L2_SIG_DISCONNECTION_RSP, rsp_pkt_ptr,
                                cdb_ptr, FALSE);

    result = 0; // success

  }
  else if ( cdb_ptr != NULL )
  {
    // No free signalling packets. Disconnect link
    bt_l2_close_l2_connection( cdb_ptr, BT_EVR_L2_UNSPECIFIED_ERROR );
  }
  else
  {
    BT_ERR ("BT L2: Unexp error in L2 Disconn Rsp! lcid %x mid %x", 
            local_cid, matching_id, 0);
  }

  return( result );

}

/*===========================================================================

FUNCTION
  bt_l2_send_l2ca_connect_ind

DESCRIPTION
  | code=BT_L2_EC_CONNECT_IND | length | BD_ADDR | CID | PSM | matching ID |

===========================================================================*/
LOCAL boolean bt_l2_send_l2ca_connect_ind
(
  bt_bd_addr_type*  bd_addr_ptr,
  word              lcid,
  word              psm,
  byte              matching_id
)
{

  boolean                   success = TRUE;
  bt_l2_conn_db_type*       cdb_ptr;
  bt_l2_reg_table_type*  reg_ptr;

  reg_ptr = bt_l2_find_reg_table_entry_psm( psm );
  if ( ( reg_ptr != NULL ) && ( bt_l2_is_psm_registered( psm ) == TRUE ) )
  {
    cdb_ptr = bt_l2_get_conn_entry_from_cid( lcid );

    if ( cdb_ptr == NULL )
    {
      BT_ERR( "BT L2 ERR: psm(%x) not in DB", psm, 0, 0 );
      success = FALSE;
    }
    else
    {
      cdb_ptr->in_mtu       = reg_ptr->psm_params.in_mtu;
      cdb_ptr->out_qos      = reg_ptr->psm_params.out_qos;
      cdb_ptr->out_flush_to = reg_ptr->psm_params.out_flush_to;
      cdb_ptr->app_id       = reg_ptr->bt_app_id;

      bt_l2_l2ca_connect_rsp( bd_addr_ptr,
                              matching_id,
                              lcid,
                              BT_L2_CONN_REQ_SUCCESS,
                              BT_L2_CONN_REQ_NO_INFO );
    }
  }
  else
  {
    bt_l2_l2ca_connect_rsp( bd_addr_ptr,
                            matching_id,
                            lcid,
                            BT_L2_CONN_REFUSED_PSM_UNSUPPORTED,
                            BT_L2_CONN_REQ_NO_INFO );
  }

  return( success );

}

/*===========================================================================

FUNCTION
  bt_l2_send_l2ca_connect_cfm

DESCRIPTION
  | code=BT_L2_EC_CONNECT_CFM | length | CID | result | status |

===========================================================================*/
LOCAL boolean bt_l2_send_l2ca_connect_cfm
(
  word     lcid,
  word     result,
  word     status
)
{

  boolean                    success = FALSE;
  bt_l2_conn_db_type*        cdb_ptr;
  bt_ev_msg_type             ev_l2_conn_failed;

  cdb_ptr = bt_l2_get_conn_entry_from_cid( lcid );

  if ( cdb_ptr != NULL )
  {
    if ( result == BT_L2_CONN_RSP_SUCCESS )
    {
      if ( cdb_ptr->initiated_conn == TRUE )
      {
        bt_l2_l2ca_config_req( cdb_ptr->local_cid,
                               cdb_ptr->in_mtu,
                               cdb_ptr->out_qos,
                               cdb_ptr->out_flush_to,
                               cdb_ptr->link_to );
        success = TRUE;
        (void)rex_set_timer( &cdb_ptr->total_config_process_timer,
                             BT_L2_MAX_TOTAL_CONFIG_PROCESS_TIME );
      }
      else
      {
        BT_ERR( "BT L2: Local dev should be initiator:%x %x", lcid, result, 0 );
      }
    }
    else
    {
      /* Send L2CAP Connection Failed event */
      ev_l2_conn_failed.ev_hdr.ev_type   = BT_EV_L2_CONNECTION_FAILED;
      ev_l2_conn_failed.ev_hdr.bt_app_id = cdb_ptr->app_id;
      ev_l2_conn_failed.ev_msg.ev_l2_conn_failed.bd_addr = cdb_ptr->bd_addr;
      ev_l2_conn_failed.ev_msg.ev_l2_conn_failed.cid     = cdb_ptr->local_cid;

      ev_l2_conn_failed.ev_msg.ev_l2_conn_failed.reason =
        BT_EVR_L2_REMOTE_REJECT_CONNECTION;
      cdb_ptr->serv_sec_error_code = BT_BE_UNSPECIFIED_ERROR;

      BT_MSG_API( "BT L2 EV TX: Conn Failed lcid %x rslt %x SE %x",
                  lcid, result, 0 );
      bt_ec_send_event( &ev_l2_conn_failed );
    }
  }
  else
  {
    BT_ERR( "BT L2: cid(%x) should be in DB", lcid, 0, 0 );
  }

  return( success );

}

/*===========================================================================

FUNCTION
  bt_l2_send_l2ca_config_ind

DESCRIPTION
  | code=BT_L2_EC_CONFIG_IND | length | CID | matching id | mtu | qos |
  | flush timeout |

===========================================================================*/
LOCAL boolean bt_l2_send_l2ca_config_ind
(
  word             lcid,
  byte             matching_id,
  word             mtu,
  word             flush_to,
  bt_l2_qos_type*  qos_ptr,
  word             flags
)
{

  boolean                   success = FALSE;
  bt_l2_conn_db_type*       cdb_ptr;
  bt_l2_reg_table_type*  reg_ptr;
  bt_ev_msg_type            ev_l2_conn;
  word                      rqst_process_result;

  cdb_ptr = bt_l2_get_conn_entry_from_cid( lcid );

  if ( cdb_ptr != NULL )
  {
    // Send the config response
    rqst_process_result =
    bt_l2_l2ca_config_rsp( matching_id,
                           lcid,
                           mtu,
                           *qos_ptr,
                           flush_to,
                           flags );

    if ( ( cdb_ptr->config_req_state == BT_L2_CONFIG_END ) &&
         ( cdb_ptr->config_rsp_state == BT_L2_CONFIG_END ) )
    {
      // done with configuration; reset the states
      if ( cdb_ptr->state == BT_L2_CONFIG )
      {
        cdb_ptr->last_state_before_open = BT_L2_CONFIG;
        cdb_ptr->state = BT_L2_OPEN;
#ifdef BT_SWDEV_2_1_PB
        if( bt_soc_etc_data.lmp_version >= BT_SOC_LMP_VERSION_2_1 )
        {
          if( cdb_ptr->out_flush_to != BT_L2_DEFAULT_FLUSH_TIMEOUT )
          {
            bt_cmd_rm_set_auto_flush_to(cdb_ptr->out_flush_to,
                                        cdb_ptr->conn_hndl);
          }
        }
#endif /* BT_SWDEV_2_1_PB */
      }
      else
      {
        cdb_ptr->last_state_before_open = BT_L2_RECONFIG;
        cdb_ptr->state = BT_L2_REOPEN;
      }
      cdb_ptr->config_req_state = BT_L2_CONFIG_START;
      cdb_ptr->config_rsp_state = BT_L2_CONFIG_START;
      (void)rex_clr_timer( &cdb_ptr->total_config_process_timer );
    }
    else if ( ( ( cdb_ptr->state == BT_L2_CONFIG &&
                           cdb_ptr->initiated_conn == FALSE ) ||
                ( cdb_ptr->state == BT_L2_RECONFIG &&
                           cdb_ptr->received_reconfig == TRUE ) ) &&
              ( !BT_L2_IS_C_FLAG_SET(flags) ) &&
              ( rqst_process_result == BT_L2_CONFIG_RSP_SUCCESS ) )
    {
      bt_l2_l2ca_config_req( cdb_ptr->local_cid,
                             cdb_ptr->in_mtu,
                             cdb_ptr->out_qos,
                             cdb_ptr->out_flush_to,
                             cdb_ptr->link_to );
      cdb_ptr->received_reconfig = FALSE;
    }
    if ( cdb_ptr->state == BT_L2_OPEN || cdb_ptr->state == BT_L2_REOPEN )
    {
      ev_l2_conn.ev_hdr.bt_app_id = cdb_ptr->app_id;
      cdb_ptr->idle_timeout       = BT_L2_IDLE_TIMEOUT_MS;
      if ( cdb_ptr->state == BT_L2_OPEN )
      {
        ev_l2_conn.ev_hdr.ev_type   = BT_EV_L2_CONNECTED;
        // This finishes our configuration stage, let's notify upper layer.
        if ( cdb_ptr->initiated_conn == TRUE )
        {
          bt_l2_outgoing_conn_count++;
        }
        else
        {
          reg_ptr = bt_l2_find_reg_table_entry_app_id( cdb_ptr->app_id );
          if ( reg_ptr != NULL )
          {
            reg_ptr->incoming_conn_count++;
          }
        }
        memcpy( (byte*)(&ev_l2_conn.ev_msg.ev_l2_conn.bd_addr),
                (byte*)(&cdb_ptr->bd_addr),
                sizeof(bt_bd_addr_type) );
        ev_l2_conn.ev_msg.ev_l2_conn.cid = lcid;
        ev_l2_conn.ev_msg.ev_l2_conn.mtu = cdb_ptr->out_mtu;
        BT_MSG_API("BT L2 EV TX: L2 Conn lcid %x mtu %x", lcid, cdb_ptr->out_mtu, 0);
      }
      else
      {
        cdb_ptr->state = BT_L2_OPEN;
        ev_l2_conn.ev_hdr.ev_type   = BT_EV_L2_RECONFIGURED;
        ev_l2_conn.ev_msg.ev_l2_reconfig.cid = lcid;
        ev_l2_conn.ev_msg.ev_l2_reconfig.reason = BT_EVR_L2_CONFIG_SUCCESS;
        ev_l2_conn.ev_msg.ev_l2_reconfig.out_mtu = cdb_ptr->out_mtu;
        ev_l2_conn.ev_msg.ev_l2_reconfig.in_flush_to = cdb_ptr->in_flush_to;
        ev_l2_conn.ev_msg.ev_l2_reconfig.in_qos = cdb_ptr->in_qos;
        BT_MSG_API("BT L2 EV TX: L2 Recfged lcid %x mtu %x", lcid, cdb_ptr->out_mtu, 0);
#ifdef BT_SWDEV_2_1_PB
        if( bt_soc_etc_data.lmp_version >= BT_SOC_LMP_VERSION_2_1 )
        {
          if( cdb_ptr->out_flush_to != BT_L2_DEFAULT_FLUSH_TIMEOUT )
          {
            bt_cmd_rm_set_auto_flush_to(cdb_ptr->out_flush_to,
                                        cdb_ptr->conn_hndl);
          }
        }
#endif /* BT_SWDEV_2_1_PB */
      }

      bt_ec_send_event( &ev_l2_conn );
    }
    success = TRUE;
  }

  if ( success == FALSE )
  {
    // TBD - send error event here?
    BT_ERR( "BT L2 ERR: lcid(%x) not found", lcid, 0, 0 );
  }

  return( success );

}

/*===========================================================================

FUNCTION bt_l2_send_l2ca_disconnect_ind

DESCRIPTION
  | code=BT_L2_EC_DISCONNECT_IND | CID | matching ID |

===========================================================================*/
boolean bt_l2_send_l2ca_disconnect_ind
(
  word  lcid,
  byte  matching_id
)
{

  boolean                   success = FALSE;
  bt_l2_conn_db_type*       cdb_ptr;
  bt_ev_msg_type            ev_l2_disc;

  cdb_ptr = bt_l2_get_conn_entry_from_cid( lcid );

  if ( cdb_ptr != NULL )
  {
    // Send the disconnection confirmation
    bt_l2_l2ca_disconnect_rsp( lcid, matching_id );

    if( cdb_ptr->state < BT_L2_OPEN )
    {
      /* If the connection wasn't open yet, sent a connect
       * failed rather than a disconnection event */
      bt_l2_send_l2ca_connect_cfm( cdb_ptr->local_cid,
                                   BT_L2_CONN_RSP_FAILED,
                                   0); /* unused */
                          
    }
    else
    {
      /*  Let upper layer know the remote has disconnected.  */
      ev_l2_disc.ev_hdr.bt_app_id = cdb_ptr->app_id;
      ev_l2_disc.ev_hdr.ev_type   = BT_EV_L2_DISCONNECTED;
      ev_l2_disc.ev_msg.ev_l2_disc.cid = lcid;
      ev_l2_disc.ev_msg.ev_l2_disc.reason = BT_EVR_L2_NORMAL_DISCONNECT;
      
      BT_MSG_API( "BT L2 EV TX: L2 Discon AID %x lcid|id %08x rsn %x",
                  ev_l2_disc.ev_hdr.bt_app_id,
                  (lcid<<16) | (matching_id),
                  ev_l2_disc.ev_msg.ev_l2_disc.reason );
      if ( ev_l2_disc.ev_hdr.bt_app_id != BT_APP_ID_NULL )
      {
        bt_ec_send_event( &ev_l2_disc );
      }
    }

    success = TRUE;

    bt_l2_close_l2_connection( cdb_ptr, BT_EVR_L2_NORMAL_DISCONNECT );
  }

  return( success );

}

/*===========================================================================

FUNCTION
  bt_l2_send_l2ca_disconnect_cfm

DESCRIPTION
  | code=BT_L2_EC_DISCONNECT_CFM | len | CID | result

===========================================================================*/
LOCAL boolean bt_l2_send_l2ca_disconnect_cfm
(
  word  lcid
)
{

  boolean                   success = TRUE;
  bt_l2_conn_db_type*       cdb_ptr;
  bt_ev_msg_type            ev_l2_disc;


  // WAL - do all that is necessary to tear the connection

  cdb_ptr = bt_l2_get_conn_entry_from_cid( lcid );

  if ( cdb_ptr != NULL )
  {
    success = TRUE;
    ev_l2_disc.ev_hdr.bt_app_id = cdb_ptr->app_id;

    if ( cdb_ptr->state == BT_L2_W4_L2CAP_DISCONNECT_RSP ||
         cdb_ptr->state == BT_L2_RECONFIG )
    {
      /*  Let upper layer know the remote has disconnected.  */
      ev_l2_disc.ev_hdr.ev_type = BT_EV_L2_DISCONNECTED;
      ev_l2_disc.ev_msg.ev_l2_disc.cid = lcid;
      if ( cdb_ptr->state == BT_L2_RECONFIG )
      {
        ev_l2_disc.ev_msg.ev_l2_disc.reason =
                                     BT_EVR_L2_CONFIG_PARAMS_NOT_AGREEABLE;
      }
      else
      {
        ev_l2_disc.ev_msg.ev_l2_disc.reason = BT_EVR_L2_NORMAL_DISCONNECT;
      }

      BT_MSG_API( "BT L2 EV TX: L2 Discon aid %x lcid %x st|rsn %08x",
                  ev_l2_disc.ev_hdr.bt_app_id,
                  lcid,
                  (cdb_ptr->state<<16) | ev_l2_disc.ev_msg.ev_l2_disc.reason );
      if ( ev_l2_disc.ev_hdr.bt_app_id != BT_APP_ID_NULL )
      {
        bt_ec_send_event( &ev_l2_disc );
      }
    }
    else if ( cdb_ptr->state == BT_L2_CONFIG )
    {
      /* Send L2CAP Connection Failed event */
      ev_l2_disc.ev_hdr.ev_type   = BT_EV_L2_CONNECTION_FAILED;
      ev_l2_disc.ev_msg.ev_l2_conn_failed.bd_addr = cdb_ptr->bd_addr;
      ev_l2_disc.ev_msg.ev_l2_conn_failed.cid     = cdb_ptr->local_cid;
      ev_l2_disc.ev_msg.ev_l2_conn_failed.reason =
          BT_EVR_L2_REMOTE_REJECT_CONNECTION;

      BT_MSG_API( "BT L2 EV TX: L2 Conn Failed AID %x lcid %x st|rsn %x",
                  ev_l2_disc.ev_hdr.bt_app_id,
                  lcid,
                  (cdb_ptr->state<<16) | (ev_l2_disc.ev_msg.ev_l2_conn_failed.reason) );
      if ( ev_l2_disc.ev_hdr.bt_app_id != BT_APP_ID_NULL )
      {
        bt_ec_send_event( &ev_l2_disc );
      }
    }
  }

  return ( success );

}

/*===========================================================================

FUNCTION
  bt_l2_process_cmd_reject_pkt

DESCRIPTION
  Processes CmdReject command.

===========================================================================*/
LOCAL boolean bt_l2_process_cmd_reject_pkt
(
  dsm_item_type**      in_dsm_ptr_ptr,
  bt_l2_cmd_hdr_type*  cmd_hdr_ptr,
  bt_l2_conn_db_type*  cdb_ptr
)
{

  boolean                   success = FALSE;
  bt_l2_cmd_reject_cmd_type reject_cmd;
  bt_l2_sig_pkt_type        *pkt_ptr;
  bt_l2_sig_pkt_type        *disconn_pkt_ptr;
  uint8                     ci;
  bt_ev_msg_type            ev_l2_disconn;
  uint16                    num_bytes;
  uint16                    num_read = 0;
  
  num_bytes = dsm_pullup( in_dsm_ptr_ptr, (void*)&reject_cmd.reason, sizeof( word ) );
  
  if ( num_bytes == sizeof( word ) )
  {
    switch (reject_cmd.reason)
    {
    case BT_L2_REASON_CMD_NOT_UNDERSTOOD:
      num_read   = sizeof( word ) + 0;
      break;
    case BT_L2_REASON_MTU_EXCEEDED:
      num_read   = sizeof( word ) + 2;
      num_bytes += dsm_pullup( in_dsm_ptr_ptr, (void*)&reject_cmd.data[0], 2 );
      break;
    case BT_L2_REASON_INVALID_CID:
      num_read   = sizeof( word ) + 4;
      num_bytes += dsm_pullup( in_dsm_ptr_ptr, (void*)&reject_cmd.data[0], 4 );
      break;
    default:
      BT_ERR ("BT L2 RX: L2 Cmd Reject, unknown reason %x", 
              reject_cmd.reason, 0 , 0);
      num_bytes += dsm_pullup( in_dsm_ptr_ptr, NULL, cmd_hdr_ptr->length - num_bytes);
      break;
    }
  }

  /* Don't check num_bytes against sizeof( bt_l2_cmd_reject_cmd_type ), since the
   * type contains optional bytes which need not be included */
  if ( ( cdb_ptr == NULL ) || ( num_read == 0 ) || ( num_read != num_bytes ) )
  {
    BT_ERR("BT L2 RX: L2 Cmd Reject error. bytes %x, cdb_ptr %x, id %x",
           num_bytes, cdb_ptr, cmd_hdr_ptr->matching_id);
  }
  else
  {
    BT_MSG_SIG( "BT L2 RX: L2 Cmd Reject len %x data %x id %x",
                num_bytes,
                ( (reject_cmd.data[0] << 24) | (reject_cmd.data[1] << 16) |
                  (reject_cmd.data[2] << 8) | (reject_cmd.data[1]) ),
                cmd_hdr_ptr->matching_id );

    for (ci=0; ci<BT_L2_MAX_CONN; ci++)
    {
      byte rqst_idx = bt_l2_find_matching_rqst( ci, cmd_hdr_ptr->matching_id );

      if ((rqst_idx < BT_L2_MAX_PENDING_RQSTS) &&  // response matches request?
          (bt_l2_conn_db[ci].state != BT_L2_CLOSED))
        // not in the closed state?
      {
        cdb_ptr = &bt_l2_conn_db[ci];
        pkt_ptr = cdb_ptr->outstanding_rqst_ptr[ rqst_idx ];
        cdb_ptr->outstanding_rqst_ptr[ rqst_idx ] = NULL;
        success = TRUE;

        switch ( (bt_l2_sig_cmd_type)pkt_ptr->cmd_hdr.command_code )
        {
          case BT_L2_SIG_ECHO_RQST:
          case BT_L2_SIG_INFO_RQST:
            bt_l2_free_signalling_pkt( pkt_ptr );
            return (success);
        }
        if ( (bt_l2_sig_cmd_type)pkt_ptr->cmd_hdr.command_code
             != BT_L2_SIG_DISCONNECTION_RQST )
        {
          BT_MSG_DEBUG( "BT L2: lcid:%x, %x cmd rejected; disconn",
                        bt_l2_conn_db[ci].local_cid,
                        (bt_l2_sig_cmd_type) pkt_ptr->cmd_hdr.command_code,
                        0 );
          if ( bt_l2_conn_db[ci].state < BT_L2_OPEN )
          {
            (void)rex_clr_timer( &cdb_ptr->config_req_wait_timer );
            (void)rex_clr_timer( &cdb_ptr->total_config_process_timer );

            disconn_pkt_ptr = bt_l2_get_free_signalling_pkt();
            if ( disconn_pkt_ptr != NULL )
            {
              disconn_pkt_ptr->cmd.disconn_req.dest_cid   = bt_l2_conn_db[ci].remote_cid;
              disconn_pkt_ptr->cmd.disconn_req.source_cid = bt_l2_conn_db[ci].local_cid;

              bt_l2_send_signalling_pkt( BT_L2_SIG_DISCONNECTION_RQST,
                                         disconn_pkt_ptr,
                                         &bt_l2_conn_db[ci], FALSE );
            }
            else
            {
              // No free signalling packets. Disconnect link
              bt_l2_close_l2_connection( cdb_ptr, BT_EVR_L2_UNSPECIFIED_ERROR );
            }
          }
          else if ( bt_l2_conn_db[ci].state < BT_L2_W4_L2CAP_DISCONNECT_RSP )
          {
            bt_cmd_l2_disconnect( bt_l2_app_id, bt_l2_conn_db[ci].local_cid );
          }
        }
        else
        {
          /* Send L2 DISCONNECTED event to the upper layer */
          /*  Let upper layer know the remote has disconnected.  */
          ev_l2_disconn.ev_hdr.bt_app_id = bt_l2_conn_db[ci].app_id;
          if ( bt_l2_conn_db[ci].state > BT_L2_CONFIG )
          {
            ev_l2_disconn.ev_hdr.ev_type   = BT_EV_L2_DISCONNECTED;
            ev_l2_disconn.ev_msg.ev_l2_disc.cid = bt_l2_conn_db[ci].local_cid;
            ev_l2_disconn.ev_msg.ev_l2_disc.reason = BT_EVR_L2_LINK_LOST;
          }
          else
          {
            ev_l2_disconn.ev_hdr.ev_type   = BT_EV_L2_CONNECTION_FAILED;
            ev_l2_disconn.ev_msg.ev_l2_conn_failed.bd_addr = bt_l2_conn_db[ci].bd_addr;
            ev_l2_disconn.ev_msg.ev_l2_conn_failed.cid     = bt_l2_conn_db[ci].local_cid;
            ev_l2_disconn.ev_msg.ev_l2_conn_failed.reason = BT_EVR_L2_LINK_LOST;
          }
          BT_MSG_API( "BT L2 EV TX: L2 closed, AID %x lcid %x, disconn rejected",
                      ev_l2_disconn.ev_hdr.bt_app_id,
                      bt_l2_conn_db[ci].local_cid, 0);
          if ( ev_l2_disconn.ev_hdr.bt_app_id != BT_APP_ID_NULL )
          {
            bt_ec_send_event( &ev_l2_disconn );
          }
          bt_l2_close_l2_connection( &bt_l2_conn_db[ci],
                                     BT_EVR_L2_PAGE_FAILED );
        }
        bt_l2_free_signalling_pkt( pkt_ptr );
        break;
      }
    }
  }
  return (success);
}


/*===========================================================================

FUNCTION
  bt_l2_process_conn_rqst_pkt

DESCRIPTION
  Processes ConnectReq command.

===========================================================================*/
LOCAL boolean bt_l2_process_conn_rqst_pkt
(
  bt_l2_conn_db_type*  cdb_ptr,
  bt_l2_cmd_hdr_type*  cmd_hdr_ptr
)
{

  boolean                   success = FALSE;
  bt_l2_conn_db_type*       new_cdb_ptr;
  bt_l2_conn_req_cmd_type   req_cmd;
  bt_l2_conn_req_cmd_type*  req_cmd_ptr;
  bt_l2_sig_pkt_type*       rsp_pkt_ptr = NULL;
  uint8                     i;
  uint16                    num_bytes;

  num_bytes = dsm_pullup( &cdb_ptr->crp_dsm_ptr, (void*)&req_cmd,
                          sizeof( bt_l2_conn_req_cmd_type ) );

  if( num_bytes != sizeof( bt_l2_conn_req_cmd_type ) )
  {
    BT_ERR("BT L2: process_conn_rqst_pkt unrecoverable error. bytes %x",
           num_bytes, 0, 0);
    return FALSE;
  }

  req_cmd_ptr = (bt_l2_conn_req_cmd_type *) &req_cmd;

  BT_MSG_SIG( "BT L2 RX: L2 Conn Req id %x PSM %x rcid %x", 
              cmd_hdr_ptr->matching_id,
              req_cmd.psm,
              req_cmd.source_cid );
  BT_BDA( MSG_SIG, "BT L2 RX: L2 Conn Req",
          &cdb_ptr->bd_addr );

  for ( i=0; i < BT_L2_MAX_CONN; ++i )
  {
    if ( bt_l2_conn_db[i].remote_cid == req_cmd_ptr->source_cid &&
         bt_l2_conn_db[i].conn_hndl == cdb_ptr->conn_hndl &&
         bt_l2_conn_db[i].last_conn_rqst_rcvd_matching_id ==
         cmd_hdr_ptr->matching_id &&
         bt_l2_conn_db[i].state != BT_L2_CLOSED )
    {
      //entry already exits.
      // just set the state to right value and send the ind.
      break;
    }
  }

  if ( i == BT_L2_MAX_CONN )
  {
    /* Entry does not exist. See if the connection is already open
     * and if the connection entry is already used, get a new entry */
    if ( cdb_ptr->state == BT_L2_ACL_LINK_CONNECTED )
    {
      (void)rex_clr_timer( &cdb_ptr->acl_link_disconnect_timer );
    }
    else if ( cdb_ptr->state > BT_L2_W4_L2CAP_CONNECT_RQST )
    {
      new_cdb_ptr = bt_l2_get_free_conn_entry();
      if ( new_cdb_ptr != NULL )
      {
        // Copy the data:
        new_cdb_ptr->conn_status        = cdb_ptr->conn_status;
        new_cdb_ptr->bd_addr            = cdb_ptr->bd_addr;
        new_cdb_ptr->conn_hndl          = cdb_ptr->conn_hndl;
        new_cdb_ptr->app_id             = cdb_ptr->app_id;
        new_cdb_ptr->baseband_conn_role = cdb_ptr->baseband_conn_role;

        cdb_ptr = new_cdb_ptr;
        cdb_ptr->initiated_conn  = FALSE;
        cdb_ptr->initiated_disconn = FALSE;
      }
      else
      {
        /* Need to send peer a L2CAP Connection Response with reject reason (TBD) */
        rsp_pkt_ptr = bt_l2_get_free_signalling_pkt();
        if( rsp_pkt_ptr != NULL )
        {
          rsp_pkt_ptr->cmd.conn_rsp.dest_cid   = BT_L2_NULL_CID;
          rsp_pkt_ptr->cmd.conn_rsp.source_cid = req_cmd_ptr->source_cid;
          rsp_pkt_ptr->cmd.conn_rsp.result     = (word)BT_L2_CONN_REFUSED_NO_RESOURCE;
          rsp_pkt_ptr->cmd.conn_rsp.status     = (word)BT_L2_CONN_REQ_NO_INFO;
          rsp_pkt_ptr->cmd_hdr.matching_id     = cmd_hdr_ptr->matching_id;

          bt_l2_send_signalling_pkt (BT_L2_SIG_CONNECTION_RSP, rsp_pkt_ptr,
                                     cdb_ptr, FALSE);
        }
        else
        {
          // No free signalling packets. Disconnect link
          bt_l2_close_l2_connection( cdb_ptr, BT_EVR_L2_UNSPECIFIED_ERROR );
        }

        return FALSE;
      }
    }

    if ( bt_l2_next_local_cid == BT_L2_CID_LAST )
    {
      bt_l2_next_local_cid = BT_L2_CID_FIRST;
    }
    else
    {
      bt_l2_next_local_cid++;
    }
    cdb_ptr->local_cid      = bt_l2_next_local_cid;
    cdb_ptr->remote_cid     = req_cmd_ptr->source_cid;
    cdb_ptr->state          = BT_L2_W4_L2CA_CONNECT_RSP;
    cdb_ptr->initiated_conn = FALSE;
    cdb_ptr->initiated_disconn = FALSE;
    cdb_ptr->idle_timeout   = BT_L2_IDLE_TIMEOUT_MS;
    cdb_ptr->last_conn_rqst_rcvd_matching_id = cmd_hdr_ptr->matching_id;

    rsp_pkt_ptr = bt_l2_get_free_signalling_pkt();
    if( rsp_pkt_ptr != NULL )
    {
      rsp_pkt_ptr->cmd.conn_rsp.dest_cid   = cdb_ptr->local_cid;
      rsp_pkt_ptr->cmd.conn_rsp.source_cid = cdb_ptr->remote_cid;
      rsp_pkt_ptr->cmd.conn_rsp.result     = BT_L2_CONN_PENDING;
      rsp_pkt_ptr->cmd.conn_rsp.status     = BT_L2_CONN_REQ_AUTHORIZATION_PENDING;
      rsp_pkt_ptr->cmd_hdr.matching_id     = cmd_hdr_ptr->matching_id;
      bt_l2_send_signalling_pkt( BT_L2_SIG_CONNECTION_RSP, rsp_pkt_ptr,
                                 cdb_ptr, FALSE );
    }
    else
    {
      BT_ERR("BT L2: no sig packet for pending conn rsp.", 0, 0, 0);
    }

    cdb_ptr->serv_sec_local_initiated = FALSE;
    cdb_ptr->serv_sec_psm             = req_cmd_ptr->psm;
    cdb_ptr->serv_sec_matching_id     = cmd_hdr_ptr->matching_id;

    BT_MSG_API( "BT L2 CMD TX: RM Enf Sec PSM %x MI %x lcid %x",
                cdb_ptr->serv_sec_psm, cdb_ptr->serv_sec_matching_id,
                cdb_ptr->local_cid );
    BT_BDA( MSG_API, "BT L2 CMD TX: RM Enf Sec PSM", &cdb_ptr->bd_addr );
    bt_cmd_rm_enforce_security_l2cap_psm(
      bt_l2_app_id, cdb_ptr->serv_sec_psm, &cdb_ptr->bd_addr,
      cdb_ptr->local_cid, BT_RM_ATZRQ_INCOMING ); 

    success = TRUE;
  }
  else
  {
    // i < BT_L2_MAX_CONN
    if ( ( bt_l2_conn_db[i].state == BT_L2_CONFIG ) &&
         ( bt_l2_conn_db[i].config_req_state == BT_L2_CONFIG_START ) &&
         ( bt_l2_conn_db[i].config_rsp_state == BT_L2_CONFIG_START ) )
    {
      bt_l2_conn_db[i].state = BT_L2_W4_L2CA_CONNECT_RSP;
      bt_l2_conn_db[i].idle_timeout  = BT_L2_IDLE_TIMEOUT_MS;

      rsp_pkt_ptr = bt_l2_get_free_signalling_pkt();
      if( rsp_pkt_ptr != NULL )
      {
        rsp_pkt_ptr->cmd.conn_rsp.dest_cid   = bt_l2_conn_db[i].local_cid;
        rsp_pkt_ptr->cmd.conn_rsp.source_cid = bt_l2_conn_db[i].remote_cid;
        rsp_pkt_ptr->cmd.conn_rsp.result     = BT_L2_CONN_PENDING;
        rsp_pkt_ptr->cmd.conn_rsp.status     = BT_L2_CONN_REQ_AUTHORIZATION_PENDING;
        rsp_pkt_ptr->cmd_hdr.matching_id     = cmd_hdr_ptr->matching_id;
        bt_l2_send_signalling_pkt( BT_L2_SIG_CONNECTION_RSP, rsp_pkt_ptr,
                                   cdb_ptr, FALSE );
      }
      else
      {
        BT_ERR("BT L2: no sig packet for pending conn rsp.", 0, 0, 0);
      }

      bt_l2_conn_db[i].serv_sec_local_initiated = FALSE;
      bt_l2_conn_db[i].serv_sec_psm             = req_cmd_ptr->psm;
      bt_l2_conn_db[i].serv_sec_matching_id     = cmd_hdr_ptr->matching_id;
      bt_l2_conn_db[i].initiated_conn           = FALSE;
      bt_l2_conn_db[i].initiated_disconn        = FALSE;

      BT_MSG_API( "BT L2 CMD TX: RM Enf Sec PSM %x MI %x lcid %x",
                  bt_l2_conn_db[i].serv_sec_psm,
                  bt_l2_conn_db[i].serv_sec_matching_id,
                  bt_l2_conn_db[i].local_cid );
      BT_BDA( MSG_API, "BT L2 CMD TX: RM Enf Sec PSM",
              &bt_l2_conn_db[i].bd_addr );
      bt_cmd_rm_enforce_security_l2cap_psm(
        bt_l2_app_id, bt_l2_conn_db[i].serv_sec_psm,
        &bt_l2_conn_db[i].bd_addr,
        bt_l2_conn_db[i].local_cid,BT_RM_ATZRQ_INCOMING); 

      success = TRUE;
    }
    else
    {
      // reject the command
      bt_l2_sig_pkt_type *pkt_ptr      = bt_l2_get_free_signalling_pkt();
      if( pkt_ptr != NULL )
      {
        pkt_ptr->cmd.cmd_reject.reason   = (word)BT_L2_REASON_INVALID_CID;
        *((uint16 *)pkt_ptr->cmd.cmd_reject.data)   = bt_l2_conn_db[i].local_cid;
        *((uint16 *)(pkt_ptr->cmd.cmd_reject.data+2))  = bt_l2_conn_db[i].remote_cid;

        pkt_ptr->cmd_hdr.matching_id = cmd_hdr_ptr->matching_id;
        pkt_ptr->cmd_hdr.length      = 6;

        bt_l2_send_signalling_pkt( BT_L2_SIG_CMD_REJECT,
                                   pkt_ptr,
                                   &bt_l2_conn_db[i], FALSE );
      }
      else
      {
        // No free signalling packets. Disconnect link
        bt_l2_close_l2_connection( &bt_l2_conn_db[i], BT_EVR_L2_UNSPECIFIED_ERROR );
      }
      success = TRUE;
    }
  }

  return( success );

}


/*===========================================================================

FUNCTION
  bt_l2_process_conn_rsp_pkt

DESCRIPTION
  Processes ConnectRsp command.

===========================================================================*/
LOCAL boolean bt_l2_process_conn_rsp_pkt
(
  dsm_item_type**      in_dsm_ptr_ptr,
  bt_l2_cmd_hdr_type*  cmd_hdr_ptr
)
{

  boolean                   success = FALSE;
  bt_l2_conn_db_type*       cdb_ptr = NULL;
  byte                      ci;
  bt_l2_conn_rsp_cmd_type   conn_rsp_cmd;
  bt_l2_conn_rsp_cmd_type*  conn_rsp_cmd_ptr;
  bt_error_code_type        ec = BT_BE_SUCCESS;
  uint16                    num_bytes;
  byte                      rqst_idx;
  boolean                   connect_result;

  num_bytes = dsm_pullup( in_dsm_ptr_ptr, (void*)&conn_rsp_cmd,
                          sizeof( bt_l2_conn_rsp_cmd_type ) );
  conn_rsp_cmd_ptr = (bt_l2_conn_rsp_cmd_type *) &conn_rsp_cmd;

  if( num_bytes != sizeof( bt_l2_conn_rsp_cmd_type ) )
  {
    BT_ERR("BT L2: process_conn_resp_pkt unrecoverable error. bytes %x",
           num_bytes, 0, 0);
    return FALSE;
  }

  BT_MSG_SIG( "BT L2 RX: L2 Conn Rsp lcid|rcid %08x rslt|status %x id %x", 
              (conn_rsp_cmd.source_cid << 16) | conn_rsp_cmd.dest_cid,
              (conn_rsp_cmd.result << 16)     | conn_rsp_cmd.status,
              cmd_hdr_ptr->matching_id );


  for (ci=0; ci<BT_L2_MAX_CONN; ci++)
  {
    rqst_idx = bt_l2_find_matching_rqst( ci, cmd_hdr_ptr->matching_id );

    if ((rqst_idx < BT_L2_MAX_PENDING_RQSTS) &&  // response matches request?
        (bt_l2_conn_db[ci].state == (word)BT_L2_W4_L2CAP_CONNECT_RSP))
                                               // in the right state?
    {
      cdb_ptr = &bt_l2_conn_db[ci];
      break;

    }
  }

  if ( (cdb_ptr != NULL) && (rqst_idx < BT_L2_MAX_PENDING_RQSTS) )
  {
    success = TRUE;   // found it!
    if ( conn_rsp_cmd_ptr->result == BT_L2_CONN_PENDING )
    {
      cdb_ptr->remote_cid  = conn_rsp_cmd_ptr->dest_cid;
      (void)rex_clr_timer(&(cdb_ptr->outstanding_rqst_ptr[ rqst_idx ])
                          -> expected_resp_timer );
      (cdb_ptr->outstanding_rqst_ptr[ rqst_idx ])->rtx_timer_in_use = FALSE;
      (void)rex_set_timer(&(cdb_ptr->outstanding_rqst_ptr[ rqst_idx ])
                           -> expected_resp_timer,
                          (cdb_ptr->outstanding_rqst_ptr[ rqst_idx ])
                          -> ertx_timer_value );
      return ( TRUE );
    }

    if ( conn_rsp_cmd_ptr->source_cid != cdb_ptr->local_cid )
    {
      /* The remote really shouldn't do this. The source_cid in the
       * connection response should just mirror the value we sent
       * to them. But we know what they meant, since the request ID
       * matches. Rather than reject the message, just fix it here.
       */
      BT_ERR("BT L2: conn rsp; remote sent lcid==%x, changing to %x",
             conn_rsp_cmd_ptr->source_cid, cdb_ptr->local_cid,0);
      conn_rsp_cmd_ptr->source_cid = cdb_ptr->local_cid;
    }

    // remove connection request from outstanding request list
    bt_l2_free_signalling_pkt( cdb_ptr->outstanding_rqst_ptr[ rqst_idx ] );
    cdb_ptr->outstanding_rqst_ptr[ rqst_idx ] = NULL;

    // stop the timer (that was started when ConnectReq was sent), TBD

    // update connection DB
    switch ( conn_rsp_cmd_ptr->result)
    {
      case BT_L2_CONN_RSP_SUCCESS:
        // update connection DB
        cdb_ptr->remote_cid  = conn_rsp_cmd_ptr->dest_cid;
        cdb_ptr->conn_status = BT_LM_CONN_UP;
        cdb_ptr->state       = BT_L2_CONFIG;
        break;
      case BT_L2_CONN_REFUSED_PSM_UNSUPPORTED:
        ec = BT_BE_UNSPECIFIED_ERROR;
        break;
      case BT_L2_CONN_REFUSED_SECURITY_BLOCK:
        ec = BT_BE_CONN_REJECTED_SECURITY_FAILURE;
        // Pass this information to RM as RM is unaware of this failure
        bt_rm_auth_failed (&cdb_ptr->bd_addr);
        break;
      case BT_L2_CONN_REFUSED_NO_RESOURCE:
        ec = BT_BE_CONN_REJECTED_NO_RESOURCE;
        break;
      default:
        // TBD
        ec = BT_BE_UNSPECIFIED_ERROR;
        break;
    }

    /* Send connect cfm event with result/status */
    connect_result = bt_l2_send_l2ca_connect_cfm(
                         conn_rsp_cmd_ptr->source_cid,
                         conn_rsp_cmd_ptr->result,
                         conn_rsp_cmd_ptr->status ); 

    if ( (ec != BT_BE_SUCCESS) || (connect_result == FALSE) )
    {
      BT_ERR("BT L2: ci: %x, secure'nuff:%x",
             ci,
             0,
             0);
      /*  Connection refused;  tear down the ACL link if necessary.  */
      bt_l2_close_l2_connection(
        cdb_ptr, BT_EVR_L2_REMOTE_REJECT_CONNECTION );
     }

  }
  else
  {
    /* No matching cdb found */
    BT_MSG_DEBUG("BT L2 RX: *INVALID* conn rsp. no matching id %x",
                 cmd_hdr_ptr->matching_id, 0, 0 );

  }

  return( success );
}


/*===========================================================================

FUNCTION
  bt_l2_process_config_rqst_pkt

DESCRIPTION
  Processes ConfigReq command

===========================================================================*/
LOCAL boolean bt_l2_process_config_rqst_pkt
(
  dsm_item_type**      in_dsm_ptr_ptr,
  bt_l2_cmd_hdr_type*  cmd_hdr_ptr
)
{

  byte                       ci;
  word                       dest_cid;
  word                       flags;
  bt_l2_config_option_type   co;
  bt_l2_config_option_type  *co_ptr;
  int32                      option_len;
  int32                      extra_len;
  bt_l2_sig_pkt_type        *pkt_ptr;

  option_len = MIN( cmd_hdr_ptr->length-4, sizeof(bt_l2_config_option_type) );
  extra_len  = cmd_hdr_ptr->length-4 - option_len;

  if( (dsm_pullup( in_dsm_ptr_ptr, &dest_cid, 2 ) != 2) ||
      (dsm_pullup( in_dsm_ptr_ptr, &flags, 2 ) != 2) ||
      (dsm_pullup( in_dsm_ptr_ptr, &co, option_len ) != option_len))
  {
    BT_ERR("BT L2: process_config_rqst_pkt pullup error. len %x, opt_len %x",
           cmd_hdr_ptr->length, option_len, 0);
    return FALSE;
  }

  if( extra_len > 0 )
  {
    BT_ERR( "BT L2: cfg rqst pkt too big(%x)",
            cmd_hdr_ptr->length, 0, 0);
    dsm_pullup( in_dsm_ptr_ptr, NULL, extra_len );
  }

  BT_MSG_SIG( "BT L2 RX: L2 Cfg Rqst flags %x lcid %x id %x",
              flags, dest_cid, cmd_hdr_ptr->matching_id );

   // search table for the entry matching input connection handle
  for (ci=0; ci<BT_L2_MAX_CONN; ci++)
  {
    if ( ( bt_l2_conn_db[ ci ].local_cid == dest_cid ) &&
         ( ( bt_l2_conn_db[ ci ].state == BT_L2_CONFIG ) ||
           ( bt_l2_conn_db[ ci ].state == BT_L2_RECONFIG ) ||
           ( bt_l2_conn_db[ ci ].state == BT_L2_OPEN ) ) )
    {
      break;
    }  // end if entry found
  }  // end for loop

  co_ptr = &co;

  if( ci < BT_L2_MAX_CONN )   // entry found?
  {
    bt_l2_config_rsp_result_type
      result = BT_L2_CONFIG_RSP_SUCCESS;  /* assume valid config options */
    byte                     num_options = 0;
    bt_l2_conn_db_type       *cdb_ptr = &bt_l2_conn_db[ci];
    word                     out_mtu = cdb_ptr->out_mtu;
    word                     in_fto = cdb_ptr->in_flush_to;
    bt_l2_qos_type           in_qos = cdb_ptr->in_qos;
    boolean                  mtu_option_present = FALSE;
    boolean                  fto_option_present = FALSE;
    boolean                  qos_option_present = FALSE;

    // extract config options from input L2CAP packet
    result = bt_l2_get_config_options( &co_ptr, &option_len,
                                       &out_mtu, &in_fto,
                                       &in_qos, &num_options,
                                       &mtu_option_present,
                                       &fto_option_present,
                                       &qos_option_present );

    if ( ( result == BT_L2_CONFIG_REJECTED_UNKNOWN_OPTION ) ||
         ( result == BT_L2_CONFIG_REJECTED_UNACCEPTABLE_PARAMS ) ||
         ( result == BT_L2_CONFIG_REJECTED_NO_REASON ) )
    {
      BT_MSG_HIGH( "BT L2: Rejecting cfg rqst lcid %x n_opts %x",
                    cdb_ptr->local_cid, num_options, 0 );
      bt_l2_l2ca_config_rsp_internal( cmd_hdr_ptr->matching_id,
                                      cdb_ptr->local_cid, result, num_options,
                                      co_ptr, flags );
    }
    else
    {
      switch ( cdb_ptr->state )
      {
        case BT_L2_CLOSED:
          // Reject the config
          BT_MSG_HIGH("BT L2: Cfg req in closed state. lcid %x rcid %x",
                      bt_l2_conn_db[ci].local_cid,
                      bt_l2_conn_db[ci].remote_cid,0);
          pkt_ptr = bt_l2_get_free_signalling_pkt();
          if( pkt_ptr != NULL )
          {
            pkt_ptr->cmd.cmd_reject.reason                = (word)BT_L2_REASON_INVALID_CID;
            *((uint16 *)pkt_ptr->cmd.cmd_reject.data)     = bt_l2_conn_db[ci].local_cid;
            *((uint16 *)(pkt_ptr->cmd.cmd_reject.data+2)) = bt_l2_conn_db[ci].remote_cid;

            pkt_ptr->cmd_hdr.matching_id = cmd_hdr_ptr->matching_id;
            pkt_ptr->cmd_hdr.length      = 6;

            bt_l2_send_signalling_pkt( BT_L2_SIG_CMD_REJECT,
                                       pkt_ptr,
                                       &bt_l2_conn_db[ci], FALSE );
          }
          break;

        case BT_L2_OPEN:
          // If a new matching is sent, it is a fresh reconfiguration
          // request.
          if ( cdb_ptr->last_config_rqst_rcvd_matching_id !=
                                              cmd_hdr_ptr->matching_id )
          {
            cdb_ptr->received_reconfig = TRUE;
            cdb_ptr->state = BT_L2_RECONFIG;
          }
          else
          {
            // If last matching id is resent, revert back to the old
            // state just before the channel was made open.
            cdb_ptr->state = cdb_ptr->last_state_before_open;
            cdb_ptr->config_req_state = BT_L2_CONFIG_END;
          }
          // then fall through to ...

        case BT_L2_CONFIG:
        case BT_L2_RECONFIG:
          (void)rex_clr_timer( &cdb_ptr->config_req_wait_timer );
          if ( ( cdb_ptr->state == BT_L2_RECONFIG ) &&
               ( cdb_ptr->config_req_state == BT_L2_RQST_SENT ) &&
               ( ( cdb_ptr->config_rsp_state == BT_L2_CONFIG_END ) ||
                 ( cdb_ptr->config_rsp_state == BT_L2_RQST_RCVD &&
                   cdb_ptr->last_config_rqst_rcvd_c_flag_set == TRUE ) ) &&
               ( cdb_ptr->last_config_rqst_rcvd_matching_id ==
                                              cmd_hdr_ptr->matching_id ) )
          {
            cdb_ptr->received_reconfig = TRUE;
          }
          cdb_ptr->config_rsp_state = BT_L2_RQST_RCVD;

          // send upper protocol ConfigInd
          bt_l2_send_l2ca_config_ind(
                 cdb_ptr->local_cid,
                 cmd_hdr_ptr->matching_id,
                 out_mtu, in_fto, &in_qos, flags );
          break;

      }  // end switch
    }
    cdb_ptr->last_config_rqst_rcvd_matching_id = cmd_hdr_ptr->matching_id;
    if ( BT_L2_IS_C_FLAG_SET(flags) )
    {
      cdb_ptr->last_config_rqst_rcvd_c_flag_set = TRUE;
    }
    else
    {
      cdb_ptr->last_config_rqst_rcvd_c_flag_set = FALSE;
    }
  }  // end if entry found
  return( ci < BT_L2_MAX_CONN );
}

/*===========================================================================

FUNCTION
  bt_l2_process_config_rsp_pkt

DESCRIPTION
  Processes ConfigRsp command

===========================================================================*/
LOCAL boolean bt_l2_process_config_rsp_pkt
(
  dsm_item_type**      in_dsm_ptr_ptr,
  bt_l2_cmd_hdr_type*  cmd_hdr_ptr
)
{

  boolean                    success = FALSE;
  bt_l2_conn_db_type*        cdb_ptr = NULL;
  byte                       ci;
  word                       src_cid;
  word                       flags;
  word                       result;
  byte                       num_options = 0;
  bt_l2_config_option_type   co;
  bt_l2_config_option_type*  co_ptr = &co;
  bt_l2_sig_pkt_type*        pkt_ptr;
  boolean                    mtu_option_present = FALSE;
  boolean                    fto_option_present = FALSE;
  boolean                    qos_option_present = FALSE;
  word                       in_mtu;
  word                       out_flush_to;
  bt_l2_qos_type             out_qos;
  byte                       rqst_idx;
  int32                      option_bytes;
  int32                      extra_bytes;
  int32                      total_option_len;

  option_bytes = MIN( cmd_hdr_ptr->length - 6,
                      sizeof(bt_l2_config_option_type) );
  extra_bytes = cmd_hdr_ptr->length - 6 - option_bytes;

  if( (dsm_pullup( in_dsm_ptr_ptr, &src_cid, 2 ) != 2) ||
      (dsm_pullup( in_dsm_ptr_ptr, &flags, 2 ) != 2) ||
      (dsm_pullup( in_dsm_ptr_ptr, &result, 2 ) != 2) ||
      (dsm_pullup( in_dsm_ptr_ptr, &co, option_bytes ) !=  option_bytes ) )
  {
    BT_ERR("BT L2: process_config_rsp_pkt pullup error",0,0,0);
    return FALSE;
  }

  if( extra_bytes > 0 )
  {
    BT_ERR("BT L2: cfg rsp pkt too big(%x)",
           cmd_hdr_ptr->length, 0, 0);
    dsm_pullup( in_dsm_ptr_ptr, NULL, extra_bytes );
  }

  BT_MSG_SIG( "BT L2 RX: Cfg Rsp result %x, rcid %x, len|id %08x",
              result, src_cid, 
              (cmd_hdr_ptr->length<<16) | (cmd_hdr_ptr->matching_id) );

  for (ci=0; ci<BT_L2_MAX_CONN; ci++)
  {
    rqst_idx = bt_l2_find_matching_rqst (ci, cmd_hdr_ptr->matching_id);

    if ((rqst_idx < BT_L2_MAX_PENDING_RQSTS) &&  // response matches request?
        ((bt_l2_conn_db[ci].state == BT_L2_CONFIG) ||
         (bt_l2_conn_db[ci].state == BT_L2_RECONFIG)) &&   // in the right state?
        (bt_l2_conn_db[ci].config_req_state == BT_L2_RQST_SENT))
    {
      cdb_ptr = &bt_l2_conn_db[ci];
      break;
    }
  }  // end for loop

  if ( (cdb_ptr != NULL) && (rqst_idx < BT_L2_MAX_PENDING_RQSTS) )
  {
    success = TRUE;   // found it!

    // remove config request from outstanding request list
    bt_l2_free_signalling_pkt( cdb_ptr->outstanding_rqst_ptr[ rqst_idx ] );
    cdb_ptr->outstanding_rqst_ptr[ rqst_idx ] = NULL;

    if ( result == BT_L2_CONFIG_RSP_SUCCESS )
    {
      /* Check configuration options */
      /* TBD Right now, we just look at these options but don't do anything
       * about them.  Should be fixed */
      total_option_len = cmd_hdr_ptr->length - 6;
      result = bt_l2_get_config_options( &co_ptr, &total_option_len,
                                         &cdb_ptr->in_mtu,
                                         &cdb_ptr->out_flush_to,
                                         &cdb_ptr->out_qos,
                                         &num_options,
                                         &mtu_option_present,
                                         &fto_option_present,
                                         &qos_option_present );

      if ( ( ( cdb_ptr->initiated_conn == TRUE && cdb_ptr->state == BT_L2_CONFIG ) ||
             ( cdb_ptr->received_reconfig == FALSE && cdb_ptr->state == BT_L2_RECONFIG ) ) &&
           ( cdb_ptr->config_rsp_state == BT_L2_CONFIG_START ) )
      {
        (void)rex_set_timer( &cdb_ptr->config_req_wait_timer,
                             BT_L2_MAX_CONFIG_REQ_WAIT_TIME );
      }
    }
    else if ( result == BT_L2_CONFIG_REJECTED_UNACCEPTABLE_PARAMS )
    {
      in_mtu = cdb_ptr->in_mtu;
      out_flush_to = cdb_ptr->out_flush_to;
      out_qos = cdb_ptr->out_qos;

      // Accept the parameters suggested by the remote device
      total_option_len = cmd_hdr_ptr->length - 6;
      result = bt_l2_get_config_options( &co_ptr, &total_option_len,
                                         &in_mtu,
                                         &out_flush_to,
                                         &out_qos,
                                         &num_options,
                                         &mtu_option_present,
                                         &fto_option_present,
                                         &qos_option_present );

      BT_MSG_DEBUG( "BT L2: L2 Cfg Rsp unacc param result %x", result,0,0 );

      if ( result != BT_L2_CONFIG_RSP_SUCCESS )
      {
        result = BT_L2_CONFIG_REJECTED_UNACCEPTABLE_PARAMS;
      }
      else if ( ( mtu_option_present == TRUE ) &&
           ( ( in_mtu > cdb_ptr->in_mtu ) || ( in_mtu < BT_L2_MIN_MTU ) ) )
      {
        result = BT_L2_CONFIG_REJECTED_UNACCEPTABLE_PARAMS;
      }
      else if ( ( qos_option_present == TRUE ) &&
                ( out_qos.service_type == BT_QOS_GUARANTEED ) )
      {
        result = BT_L2_CONFIG_REJECTED_UNACCEPTABLE_PARAMS;
      }
      else if ( ( ( mtu_option_present == TRUE ) && ( in_mtu <= cdb_ptr->in_mtu ) ) ||
                ( out_flush_to != cdb_ptr->out_flush_to ) ||
                ( memcmp((void *)&out_qos, (void *)&cdb_ptr->out_qos, sizeof(bt_l2_qos_type))!=0 ) )
      {
        // Send a new request with the acceptable params value
        bt_l2_l2ca_config_req( cdb_ptr->local_cid,
                               in_mtu,
                               out_qos,
                               out_flush_to,
                               cdb_ptr->link_to );
        return ( success );
      }
      else
      {
        result = BT_L2_CONFIG_REJECTED_UNACCEPTABLE_PARAMS;
      }
    }
    else if ( result == BT_L2_CONFIG_REJECTED_UNKNOWN_OPTION )
    {
      total_option_len = cmd_hdr_ptr->length - 6;
      result = bt_l2_get_config_options( &co_ptr, &total_option_len,
                                         &in_mtu,
                                         &out_flush_to,
                                         &out_qos,
                                         &num_options,
                                         &mtu_option_present,
                                         &fto_option_present,
                                         &qos_option_present );

      BT_MSG_DEBUG( "BT L2: L2 Cfg Rsp unk option rslt %x", result,0,0 );

      if ( result != BT_L2_CONFIG_RSP_SUCCESS )
      {
        result = BT_L2_CONFIG_REJECTED_UNKNOWN_OPTION;
      }
      else if ( mtu_option_present == FALSE )
      {
        if ( fto_option_present == TRUE || qos_option_present == TRUE )
        {
          if ( fto_option_present == TRUE )
          {
            if ( cdb_ptr->out_flush_to != BT_L2_DEFAULT_FLUSH_TIMEOUT )
            {
              out_flush_to = BT_L2_DEFAULT_FLUSH_TIMEOUT;
            }
            else
            {
              result = BT_L2_CONFIG_REJECTED_UNKNOWN_OPTION;
            }
          }
          if ( qos_option_present == TRUE &&
               result != BT_L2_CONFIG_REJECTED_UNKNOWN_OPTION )
          {
            if ( memcmp((byte *)&cdb_ptr->out_qos, (byte *)&bt_l2_default_qos,
                        sizeof(bt_l2_qos_type))!=0 )
            {
              out_qos = bt_l2_default_qos;
            }
            else
            {
              result = BT_L2_CONFIG_REJECTED_UNKNOWN_OPTION;
            }
          }
          if ( result != BT_L2_CONFIG_REJECTED_UNKNOWN_OPTION )
          {
            // Send a new request with the acceptable params value
            bt_l2_l2ca_config_req( cdb_ptr->local_cid,
                                   cdb_ptr->in_mtu,
                                   out_qos,
                                   out_flush_to,
                                   cdb_ptr->link_to );
            return ( success );
          }
        }
        else
        {
          result = BT_L2_CONFIG_REJECTED_UNKNOWN_OPTION;
        }
      }
      else
      {
        result = BT_L2_CONFIG_REJECTED_UNKNOWN_OPTION;
      }
    }

    if ( (result == BT_L2_CONFIG_RSP_SUCCESS) && BT_L2_IS_C_FLAG_SET(flags) )
    {
      pkt_ptr = bt_l2_get_free_signalling_pkt();
      if( pkt_ptr != NULL )
      {
        pkt_ptr->cmd_hdr.length =
          bt_l2_build_config_req_pkt( &pkt_ptr->cmd.config_req, cdb_ptr, TRUE,
                                      in_mtu, out_flush_to, out_qos );

        // now, send config request packet to peer
        bt_l2_send_signalling_pkt( BT_L2_SIG_CONFIGURE_RQST,
                                   pkt_ptr, cdb_ptr, FALSE );
        
        result = BT_L2_CONFIG_REQ_SUCCESS;
        cdb_ptr->config_req_state = BT_L2_RQST_SENT;
      }
      else
      {
        result = BT_L2_CONFIG_FAILED_OTHER_REASON;
        bt_l2_close_l2_connection( cdb_ptr, BT_EVR_L2_UNSPECIFIED_ERROR );
      }
    }
    else if ( result == BT_L2_CONFIG_RSP_SUCCESS )
    {
      cdb_ptr->config_req_state = BT_L2_CONFIG_END;
      // inform upper protocol
      bt_l2_send_l2ca_config_cfm( src_cid, result,
                                  cdb_ptr->in_mtu,
                                  cdb_ptr->out_flush_to,
                                  &cdb_ptr->out_qos );
    }
    else
    {
      BT_MSG_DEBUG( "BT L2: Discon Rqst res %x lcid %x rcid %x",
                     result, cdb_ptr->local_cid, cdb_ptr->remote_cid );

      if ( cdb_ptr->state < BT_L2_OPEN )
      {
        /* Send L2CAP Disconnection Request */
        pkt_ptr = bt_l2_get_free_signalling_pkt();

        (void)rex_clr_timer( &cdb_ptr->config_req_wait_timer );
        (void)rex_clr_timer( &cdb_ptr->total_config_process_timer );

        if ( pkt_ptr != NULL )
        {
          pkt_ptr->cmd.disconn_req.dest_cid   = cdb_ptr->remote_cid;
          pkt_ptr->cmd.disconn_req.source_cid = cdb_ptr->local_cid;

          bt_l2_send_signalling_pkt( BT_L2_SIG_DISCONNECTION_RQST,
                                     pkt_ptr,
                                     cdb_ptr, FALSE );
        }
        else
        {
          // No free signalling packets. Disconnect link
          bt_l2_close_l2_connection( cdb_ptr, BT_EVR_L2_UNSPECIFIED_ERROR );
        }
      }
      else if ( cdb_ptr->state < BT_L2_W4_L2CAP_DISCONNECT_RSP )
      {
        bt_cmd_l2_disconnect( bt_l2_app_id, cdb_ptr->local_cid );
      }
    }
  }  // end if entry found

  return( success );
}


/*===========================================================================

FUNCTION
  bt_l2_process_disconn_rqst_pkt

DESCRIPTION

===========================================================================*/
LOCAL boolean bt_l2_process_disconn_rqst_pkt
(
  dsm_item_type**      in_dsm_ptr_ptr,
  bt_l2_cmd_hdr_type*  cmd_hdr_ptr
)
{
  boolean                      success = FALSE;
  byte                         ci = BT_L2_MAX_CONN;
  bt_l2_disconn_req_cmd_type   disconn_req;
  bt_l2_disconn_req_cmd_type*  disconn_req_ptr;
  bt_l2_sig_pkt_type*          rsp_pkt_ptr  = NULL;
  bt_l2_reg_table_type*        reg_ptr=NULL;

  if( (dsm_pullup( in_dsm_ptr_ptr,
                   (void*)&disconn_req,
                   sizeof( bt_l2_disconn_req_cmd_type ) ))
      != sizeof( bt_l2_disconn_req_cmd_type ) )
  {
    BT_ERR("BT L2 RX: L2 Disc Req pullup error id %x",
           cmd_hdr_ptr->matching_id,0,0);
    return FALSE;
  }

  disconn_req_ptr = (bt_l2_disconn_req_cmd_type *) &disconn_req;

  BT_MSG_SIG( "BT L2 RX: L2 Disc Req id %x lcid %x rcid %x", 
              cmd_hdr_ptr->matching_id,
              disconn_req.dest_cid,
              disconn_req.source_cid );

  for (ci=0; ci<BT_L2_MAX_CONN; ci++)
  {
    if ((bt_l2_conn_db[ci].local_cid == disconn_req_ptr->dest_cid) &&
        (bt_l2_conn_db[ci].state != BT_L2_CLOSED))
    {
      if (bt_l2_conn_db[ci].remote_cid == disconn_req_ptr->source_cid)
      {
        if ( ( bt_l2_conn_db[ci].state == BT_L2_OPEN ) ||
             ( bt_l2_conn_db[ci].state == BT_L2_RECONFIG ) )
        {
          if ( bt_l2_conn_db[ci].initiated_conn == TRUE )
          {
            bt_l2_outgoing_conn_count--;
          }
          else
          {
            reg_ptr = bt_l2_find_reg_table_entry_app_id(
                                           bt_l2_conn_db[ci].app_id );
            if ( reg_ptr != NULL )
            {
              reg_ptr->incoming_conn_count--;
            }
          }
        }
        bt_l2_conn_db[ci].state        = BT_L2_W4_L2CA_DISCONNECT_RSP;
        bt_l2_conn_db[ci].idle_timeout = BT_L2_IDLE_TIMEOUT_MS;
        bt_l2_send_l2ca_disconnect_ind( bt_l2_conn_db[ci].local_cid,
                                        cmd_hdr_ptr->matching_id );
        success = TRUE;
      }
      break;
    }  // end if local CID matched
  }  // end for loop

  if ( ( success == FALSE )  && // entry not found or remote CID not matched.
       ( ci < BT_L2_MAX_CONN ) &&
       ( ( rsp_pkt_ptr = bt_l2_get_free_signalling_pkt() ) != NULL ) )
  {
    bt_l2_cmd_reject_cmd_type* reject_pkt_ptr = &rsp_pkt_ptr->cmd.cmd_reject;
    word *cid_ptr = (word*) reject_pkt_ptr->data;

    reject_pkt_ptr->reason = BT_L2_REASON_INVALID_CID;
    cid_ptr[0] = disconn_req_ptr->dest_cid;
    cid_ptr[1] = disconn_req_ptr->source_cid;

    rsp_pkt_ptr->cmd_hdr.matching_id = cmd_hdr_ptr->matching_id;
    rsp_pkt_ptr->cmd_hdr.length      = 6;

    bt_l2_send_signalling_pkt( BT_L2_SIG_CMD_REJECT, rsp_pkt_ptr,
                               &bt_l2_conn_db[ ci], FALSE );
    success = TRUE;
  }

  return( success );

}


/*===========================================================================

FUNCTION
  bt_l2_process_disconn_rsp_pkt

DESCRIPTION

===========================================================================*/
LOCAL boolean bt_l2_process_disconn_rsp_pkt
(
  dsm_item_type**      in_dsm_ptr_ptr,
  bt_l2_cmd_hdr_type*  cmd_hdr_ptr
)
{

  boolean                      success = FALSE;
  bt_l2_conn_db_type*          cdb_ptr = NULL;
  byte                         ci;
  bt_l2_disconn_rsp_cmd_type   disconn_rsp;
  bt_l2_disconn_rsp_cmd_type*  disconn_rsp_ptr;

  if( dsm_pullup( in_dsm_ptr_ptr, (void*)&disconn_rsp,
                  sizeof( bt_l2_disconn_rsp_cmd_type ) )
      != sizeof( bt_l2_disconn_rsp_cmd_type ) )
  {
    BT_ERR("BT L2: process_disconn_rsp_pkt pullup error",0,0,0);
    return FALSE;
  }
  disconn_rsp_ptr = (bt_l2_disconn_rsp_cmd_type *) &disconn_rsp;

  BT_MSG_SIG( "BT L2 RX: L2 Disc Rsp id %x lcid %x rcid %x",
              cmd_hdr_ptr->matching_id,
              disconn_rsp.dest_cid,
              disconn_rsp.source_cid );

  for (ci=0; ci<BT_L2_MAX_CONN; ci++)
  {
    byte rqst_idx = bt_l2_find_matching_rqst (ci, cmd_hdr_ptr->matching_id);

    if ((rqst_idx < BT_L2_MAX_PENDING_RQSTS) &&
        ((bt_l2_conn_db[ci].state == BT_L2_W4_L2CAP_DISCONNECT_RSP) ||
         (bt_l2_conn_db[ci].state == BT_L2_CONFIG) ||
         (bt_l2_conn_db[ci].state == BT_L2_RECONFIG)))
    {
      cdb_ptr = &bt_l2_conn_db[ci];
      success = TRUE;

      bt_l2_send_l2ca_disconnect_cfm( disconn_rsp_ptr->source_cid );

      // remove config request from outstanding request list
      bt_l2_free_signalling_pkt( cdb_ptr->outstanding_rqst_ptr[ rqst_idx ] );
      cdb_ptr->outstanding_rqst_ptr[ rqst_idx ] = NULL;

      break;
    }
  }

  /* Tear down ACL link */
  if ( success == TRUE )
  {
    bt_l2_close_l2_connection( &bt_l2_conn_db[ci], BT_EVR_L2_NORMAL_DISCONNECT );
  }

  return( success );
}


/*===========================================================================

FUNCTION
  bt_l2_process_echo_rqst_pkt

DESCRIPTION

===========================================================================*/
LOCAL boolean bt_l2_process_echo_rqst_pkt
(
  dsm_item_type**      in_dsm_ptr_ptr,
  bt_l2_cmd_hdr_type*  cmd_hdr_ptr,
  bt_l2_conn_db_type*  cdb_ptr
)
{
  bt_l2_echo_req_cmd_type  echo_req;
  bt_l2_sig_pkt_type*      rsp_pkt_ptr = 0;
  int32                    data_length;
  int32                    extra_length;

  data_length = MIN(cmd_hdr_ptr->length,BT_L2_MAX_ECHO_DATA_LEN);
  extra_length = cmd_hdr_ptr->length - data_length;
  if( extra_length > 0 )
  {
    dsm_pullup( in_dsm_ptr_ptr, (void*)&echo_req,
                data_length );
    dsm_pullup( in_dsm_ptr_ptr, NULL, extra_length );
  }
  else
  {
    dsm_pullup( in_dsm_ptr_ptr, (void*)&echo_req,
                cmd_hdr_ptr->length );
  }

  BT_MSG_SIG( "BT L2 RX: L2 Echo Req id %x tot data len %x", 
              cmd_hdr_ptr->matching_id,
              cmd_hdr_ptr->length, 0 );

  rsp_pkt_ptr = bt_l2_get_free_signalling_pkt();

  if ( rsp_pkt_ptr != NULL )
  {
    rsp_pkt_ptr->cmd_hdr.matching_id = cmd_hdr_ptr->matching_id;
    rsp_pkt_ptr->cmd_hdr.length = data_length;
    memcpy( (byte*)rsp_pkt_ptr->cmd.echo_rsp.data,
            (byte*)echo_req.data,
            data_length );

    bt_l2_send_signalling_pkt( BT_L2_SIG_ECHO_RSP, rsp_pkt_ptr, cdb_ptr,
                               FALSE );
  }
  else
  {
    // No free signalling packets. Disconnect link
    bt_l2_close_l2_connection( cdb_ptr, BT_EVR_L2_UNSPECIFIED_ERROR );
 
    return FALSE;
  }

  return TRUE;
}


/*===========================================================================

FUNCTION
  bt_l2_process_echo_rsp_pkt

DESCRIPTION

===========================================================================*/
LOCAL boolean bt_l2_process_echo_rsp_pkt
(
  dsm_item_type**      in_dsm_ptr_ptr,
  bt_l2_cmd_hdr_type*  cmd_hdr_ptr
)
{
  bt_l2_echo_rsp_cmd_type  echo_rsp;
  uint16                   ci;
  bt_l2_conn_db_type*      cdb_ptr = NULL;
  bt_ev_msg_type           ev_l2_ping;
  int32                    data_length;
  int32                    extra_length;

  data_length = MIN(cmd_hdr_ptr->length,BT_L2_MAX_ECHO_DATA_LEN);
  extra_length = cmd_hdr_ptr->length - data_length;

  BT_MSG_SIG( "BT L2 RX: L2 Echo Rsp id %x tot data len %x",
              cmd_hdr_ptr->matching_id,
              cmd_hdr_ptr->length, 0 );

  if( extra_length > 0 )
  {
    dsm_pullup( in_dsm_ptr_ptr, (void*)&echo_rsp,
                data_length );
    dsm_pullup( in_dsm_ptr_ptr, NULL, extra_length );
  }
  else
  {
    dsm_pullup( in_dsm_ptr_ptr, (void*)&echo_rsp,
                cmd_hdr_ptr->length );
  }

  for ( ci = 0; ci < BT_L2_MAX_CONN; ci++ )
  {
    byte rqst_idx = bt_l2_find_matching_rqst( ci, cmd_hdr_ptr->matching_id );

    if ( ( rqst_idx < BT_L2_MAX_PENDING_RQSTS ) &&
         ( bt_l2_conn_db[ ci ].state != BT_L2_CLOSED ) )
    {
      cdb_ptr = &bt_l2_conn_db[ ci ];

      // remove echo request from outstanding request list
      bt_l2_free_signalling_pkt( cdb_ptr->outstanding_rqst_ptr[ rqst_idx ] );
      cdb_ptr->outstanding_rqst_ptr[ rqst_idx ] = NULL;

      // Disconnect this link if it's only up to wait for the echo rsp
      if ( cdb_ptr->state == BT_L2_ACL_LINK_CONNECTED )
      {
        bt_l2_close_l2_connection( cdb_ptr, BT_EVR_L2_NORMAL_DISCONNECT );
      }
      break;
    }
  }

  if ( ( cdb_ptr != NULL ) && ( cdb_ptr->echo_app_id != BT_APP_ID_NULL ) )
  {
    ev_l2_ping.ev_hdr.ev_type            = BT_EV_L2_PING;
    ev_l2_ping.ev_hdr.bt_app_id          = cdb_ptr->echo_app_id;
    ev_l2_ping.ev_msg.ev_l2_ping.bd_addr = cdb_ptr->bd_addr;
    ev_l2_ping.ev_msg.ev_l2_ping.length  = MIN(cmd_hdr_ptr->length,BT_L2_MAX_ECHO_DATA_LEN);
    memcpy( ( byte* ) ev_l2_ping.ev_msg.ev_l2_ping.data,
            (byte *)&echo_rsp,
            MIN(cmd_hdr_ptr->length,BT_L2_MAX_ECHO_DATA_LEN) );
    ev_l2_ping.ev_msg.ev_l2_ping.reason  = BT_EVR_L2_PING_SUCCESS;

    BT_MSG_API( "BT L2 EV TX: L2 Ping success, AID %x lcid %x st %x",
                ev_l2_ping.ev_hdr.bt_app_id,
                cdb_ptr->local_cid, cdb_ptr->state );
    bt_ec_send_event( &ev_l2_ping );
    cdb_ptr->echo_app_id = BT_APP_ID_NULL;
  }
  else
  {
    BT_ERR( "BT L2: unexpected echo response", 0, 0, 0 );
  }

  return (cdb_ptr != NULL);
}


/*===========================================================================

FUNCTION
  bt_l2_process_info_rqst_pkt

DESCRIPTION

===========================================================================*/
LOCAL boolean bt_l2_process_info_rqst_pkt
(
  dsm_item_type**      in_dsm_ptr_ptr,
  bt_l2_cmd_hdr_type*  cmd_hdr_ptr,
  bt_l2_conn_db_type*  cdb_ptr
)
{
  bt_l2_info_req_cmd_type  info_req;
  bt_l2_sig_pkt_type*      rsp_pkt_ptr = 0;

  if( dsm_pullup( in_dsm_ptr_ptr, (void*)&info_req,
                  sizeof( bt_l2_info_req_cmd_type ) )
      != sizeof( bt_l2_info_req_cmd_type ) )
  {
    BT_ERR("BT L2: process_info_rqst_pkt pullup error",0,0,0);
    return FALSE;
  }

  BT_MSG_SIG( "BT L2 RX: L2 Info Req id %x type %x",
              cmd_hdr_ptr->matching_id,
              info_req.type, 0 );

  rsp_pkt_ptr = bt_l2_get_free_signalling_pkt();

  if ( rsp_pkt_ptr != NULL )
  {
    rsp_pkt_ptr->cmd_hdr.matching_id = cmd_hdr_ptr->matching_id;
    rsp_pkt_ptr->cmd_hdr.length = 4; /* 2 for InfoType; 2 for Result */
    rsp_pkt_ptr->cmd.info_rsp.type = info_req.type;
    rsp_pkt_ptr->cmd.info_rsp.result = 0x0001; /* Not supported */

    bt_l2_send_signalling_pkt( BT_L2_SIG_INFO_RSP, rsp_pkt_ptr, cdb_ptr,
                               FALSE );
  }
  else
  {
    // No free signalling packets. Disconnect link
    bt_l2_close_l2_connection( cdb_ptr, BT_EVR_L2_UNSPECIFIED_ERROR );
    return FALSE;
  }

  return TRUE;
}


/*===========================================================================

FUNCTION
  bt_l2_process_info_rsp_pkt

DESCRIPTION

===========================================================================*/
LOCAL boolean bt_l2_process_info_rsp_pkt
(
  dsm_item_type**      in_dsm_ptr_ptr,
  bt_l2_cmd_hdr_type*  cmd_hdr_ptr
)
{
  BT_MSG_SIG( "BT L2 RX: L2 Info Rsp id %x Len %x", 
              cmd_hdr_ptr->matching_id, cmd_hdr_ptr->length, 0 );

  dsm_pullup( in_dsm_ptr_ptr, NULL, cmd_hdr_ptr->length );

  return TRUE;
}


/*===========================================================================

FUNCTION
  bt_l2_process_unknown_pkt

DESCRIPTION

===========================================================================*/
LOCAL boolean bt_l2_process_unknown_pkt
(
  dsm_item_type**      in_dsm_ptr_ptr,
  bt_l2_cmd_hdr_type*  cmd_hdr_ptr,
  bt_l2_conn_db_type*  cdb_ptr
)
{

  bt_l2_sig_pkt_type*      rsp_pkt_ptr = 0;

  BT_ERR( "BT L2 RX: Bad SIG Pkt %x id %x len %x",
          cmd_hdr_ptr->command_code,
          cmd_hdr_ptr->matching_id,
          cmd_hdr_ptr->length );

  dsm_pullup( in_dsm_ptr_ptr, NULL, cmd_hdr_ptr->length );

  rsp_pkt_ptr = bt_l2_get_free_signalling_pkt();

  if ( rsp_pkt_ptr != NULL )
  {
    rsp_pkt_ptr->cmd_hdr.matching_id = cmd_hdr_ptr->matching_id;
    rsp_pkt_ptr->cmd_hdr.length = 2; /* 2 for Reason */
    rsp_pkt_ptr->cmd.cmd_reject.reason = BT_L2_REASON_CMD_NOT_UNDERSTOOD;

    bt_l2_send_signalling_pkt( BT_L2_SIG_CMD_REJECT, rsp_pkt_ptr, cdb_ptr,
                               FALSE );
  }
  else
  {
    // No free signalling packets. Disconnect link
    bt_l2_close_l2_connection( cdb_ptr, BT_EVR_L2_UNSPECIFIED_ERROR );
  }
  return TRUE;
}


/*===========================================================================

FUNCTION
  bt_l2_process_signalling_pkt

DESCRIPTION
  This function processes signalling packets.

===========================================================================*/
LOCAL void bt_l2_process_signalling_pkt
(
  bt_l2_conn_db_type*  cdb_ptr
)
{

  bt_l2_cmd_hdr_type   cmd_hdr;
  bt_l2_cmd_hdr_type*  cmd_hdr_ptr;
  boolean              success;
  uint16               bytes_pulled;
  uint32               dsm_len;

#ifdef BT_SWDEV_QXDM_LOG_MSG
  bt_l2_send_rx_signaling_log( cdb_ptr->crp_dsm_ptr, cdb_ptr->conn_hndl );
#endif

  while ( ( dsm_len = dsm_length_packet( cdb_ptr->crp_dsm_ptr ) ) > 0 )
  {
    /* loop around and process all signalling commands in this signalling
    ** command packet.
    */

    bytes_pulled = dsm_pullup( &cdb_ptr->crp_dsm_ptr, (void*)&cmd_hdr,
                               sizeof( bt_l2_cmd_hdr_type ) );
    if ( bytes_pulled == sizeof( bt_l2_cmd_hdr_type ) )
    {
      /* process this command */
      cmd_hdr_ptr = (bt_l2_cmd_hdr_type *) &cmd_hdr;

      switch ((bt_l2_sig_cmd_type)cmd_hdr_ptr->command_code)
      {
        case BT_L2_SIG_CMD_REJECT:
          success = bt_l2_process_cmd_reject_pkt( &cdb_ptr->crp_dsm_ptr,
                                                  cmd_hdr_ptr, cdb_ptr );
          break;

        case BT_L2_SIG_CONNECTION_RQST:
          success = bt_l2_process_conn_rqst_pkt( cdb_ptr, cmd_hdr_ptr );
          break;

        case BT_L2_SIG_CONNECTION_RSP:
          success = bt_l2_process_conn_rsp_pkt( &cdb_ptr->crp_dsm_ptr,
                                                cmd_hdr_ptr );
          break;

        case BT_L2_SIG_CONFIGURE_RQST:
          success = bt_l2_process_config_rqst_pkt( &cdb_ptr->crp_dsm_ptr,
                                                   cmd_hdr_ptr );
          break;

        case BT_L2_SIG_CONFIGURE_RSP:
          success = bt_l2_process_config_rsp_pkt( &cdb_ptr->crp_dsm_ptr,
                                                  cmd_hdr_ptr );
          break;

        case BT_L2_SIG_DISCONNECTION_RQST:
          success = bt_l2_process_disconn_rqst_pkt( &cdb_ptr->crp_dsm_ptr,
                                                    cmd_hdr_ptr );
          break;

        case BT_L2_SIG_DISCONNECTION_RSP:
          success = bt_l2_process_disconn_rsp_pkt( &cdb_ptr->crp_dsm_ptr,
                                                   cmd_hdr_ptr );
          break;

        case BT_L2_SIG_ECHO_RQST:
          success = bt_l2_process_echo_rqst_pkt( &cdb_ptr->crp_dsm_ptr,
                                                 cmd_hdr_ptr, cdb_ptr );
          break;

        case BT_L2_SIG_ECHO_RSP:
          success = bt_l2_process_echo_rsp_pkt( &cdb_ptr->crp_dsm_ptr,
                                                cmd_hdr_ptr );
          break;

        case BT_L2_SIG_INFO_RQST:
          success = bt_l2_process_info_rqst_pkt( &cdb_ptr->crp_dsm_ptr,
                                                 cmd_hdr_ptr, cdb_ptr );
          break;

        case BT_L2_SIG_INFO_RSP:
          success = bt_l2_process_info_rsp_pkt( &cdb_ptr->crp_dsm_ptr,
                                                cmd_hdr_ptr );
          break;

        default:
          success = bt_l2_process_unknown_pkt( &cdb_ptr->crp_dsm_ptr,
                                               cmd_hdr_ptr, cdb_ptr );
      }

      if ( success == FALSE )
      {
        BT_ERR( "BT L2 RX: Bad Sig pkt Cmd Code %x L %x",
                cmd_hdr_ptr->command_code, dsm_len, 0 );
      }
    }
    else
    {
      BT_ERR( "BT L2 RX: Bad Sig pkt pullup C %x L %x",
              bytes_pulled, dsm_len, 0 );
    }
  }

  if ( cdb_ptr->crp_dsm_ptr != NULL )
  {
    BT_ERR( "BT L2 RX: Bad Sig pkt processing L %x",
            dsm_len, 0, 0 );
  }

}


/*===========================================================================

FUNCTION
  bt_l2_process_data_pkt

DESCRIPTION
  Sends data received on a CID to the appropriate upper layer.

===========================================================================*/
LOCAL void bt_l2_process_data_pkt
(
  bt_l2_conn_db_type*  cdb_ptr
)
{

  bt_ev_msg_type  ev_l2_rxd;
  uint32          dsm_len;

#ifdef BT_SWDEV_QXDM_LOG_MSG
  bt_l2_send_payload_rx_log( cdb_ptr->crp_dsm_ptr, cdb_ptr->local_cid );
#endif

  dsm_len = dsm_length_packet( cdb_ptr->crp_dsm_ptr );

  if ( cdb_ptr->state == BT_L2_OPEN )
  {
    cdb_ptr->idle_timeout = 0;

    /*  Send received data to upper layer.  */

    ev_l2_rxd.ev_hdr.bt_app_id         = cdb_ptr->app_id;
    ev_l2_rxd.ev_hdr.ev_type           = BT_EV_L2_RX_DATA;
    ev_l2_rxd.ev_msg.ev_l2_rxd.cid     = cdb_ptr->local_cid;
    ev_l2_rxd.ev_msg.ev_l2_rxd.dsm_ptr = cdb_ptr->crp_dsm_ptr;


    BT_MSG_API_PYLD( "BT L2 EV TX: RXD AID %x lcid %x Len %x",
                     cdb_ptr->app_id, cdb_ptr->local_cid,
                     dsm_len );

    BT_SET_PKT_TRACER( cdb_ptr->crp_dsm_ptr, DSM_BT_RX_L2 );

#ifdef BT_TEST_PYLD_FAST_RX
#ifdef FEATURE_BT_TEST_API
#error code not present
#endif /* FEATURE_BT_TEST_API */
    {
      /*  BT_TBD... code below not complete.  */
      switch( bt_l2_get_psm_from_reg_app_id( cdb_ptr->app_id ) )
      {
        case BT_L2_PSM_SDP:
          bt_sd_ev_l2_rx_data( &(ev_l2_rxd.ev_msg.ev_l2_rxd) );
          break;
        case BT_L2_PSM_RFCOMM:
          bt_rc_ev_l2_rx_data( &ev_l2_rxd );
          break;
        default:
          /* Route data for unsupported PSM to Event Control */
          bt_ec_send_event( &ev_l2_rxd );
          break;
      }
    }
#else  /* BT_TEST_PYLD_FAST_RX */

    bt_ec_send_event( &ev_l2_rxd );

#endif /* BT_TEST_PYLD_FAST_RX */

    cdb_ptr->crp_dsm_ptr = NULL;
  }
  else
  {
    BT_ERR( "BT L2 RX: Bad Conn St %x Drop L %x",
            cdb_ptr->state, dsm_len, 0 );

    dsm_free_packet( &cdb_ptr->crp_dsm_ptr );
  }

  if ( cdb_ptr->crp_dsm_ptr != NULL )
  {
    BT_ERR( "BT L2 RX: Bad Data pkt processing L %x",
            dsm_len, 0, 0 );

    dsm_free_packet( &cdb_ptr->crp_dsm_ptr );
  }

}

/*-------------------------------------------------------------------------*/
/*                                                                         */
/*                      Event Processing Functions                         */
/*                                                                         */
/*-------------------------------------------------------------------------*/

/*===========================================================================

FUNCTION
  bt_l2_ev_rm_rx_data_acl

DESCRIPTION
  Processes a RM Receive Data ACL event.

===========================================================================*/
LOCAL void bt_l2_ev_rm_rx_data_acl
(
  bt_ev_msg_type*  ev_ptr
)
{

  bt_ev_rm_rx_data_acl_type*  rxda_ptr;
  dsm_item_type*              dsm_ptr;
  uint32                      dsm_length;
  uint32                      temp_dsm_len;
  uint16                      i;
  bt_l2_conn_db_type*         dest_cdb_ptr = NULL;
  bt_l2_conn_db_type*         cdb_ptr = NULL;
  uint16                      bytes_pulled_up;
  uint16                      conns_with_crp = 0;

  rxda_ptr = &ev_ptr->ev_msg.ev_rm_rxda;
  dsm_ptr  = rxda_ptr->dsm_ptr;

  BT_SET_PKT_TRACER( dsm_ptr, DSM_BT_RX_REACH_L2 );

  dsm_length = dsm_length_packet( dsm_ptr );

  BT_MSG_API_PYLD( "BT L2 EV RX: RM RXDA H %x S %x Len %x",
                   rxda_ptr->handle, rxda_ptr->l2cap_pkt_start,
                   dsm_length );

  /*  Get connection table entry pointer.  */
  for ( i = 0; i < BT_L2_MAX_CONN; i++ )
  {
    if ( bt_l2_conn_db[ i ].conn_hndl == rxda_ptr->handle )
    {
      if ( (bt_l2_conn_db[ i ].crp_dsm_ptr != NULL) ||
           (bt_l2_conn_db[ i ].crp_hdr_valid != FALSE) )
      {
        conns_with_crp++;
        cdb_ptr = &bt_l2_conn_db[ i ];
      }

      if ( cdb_ptr == NULL )
      {
        cdb_ptr = &bt_l2_conn_db[ i ];
      }
    }
  }

  /*  Sanity check for fragmented packets and connection table state.  */
  if ( ((rxda_ptr->l2cap_pkt_start != FALSE) && (conns_with_crp != 0)) ||
       ((rxda_ptr->l2cap_pkt_start == FALSE) && (conns_with_crp != 1)) )
  {
    BT_ERR( "BT L2 RX: Inconsistent FRP S %x C %x H %x",
            rxda_ptr->l2cap_pkt_start, conns_with_crp, rxda_ptr->handle );
  }

  if ( cdb_ptr != NULL )
  {
    /*  Throw away DSM if it is empty or if a L2CAP Start of   */
    /*  packet was not received before a continuation packet.  */
    if ( (dsm_length > 0) &&
         (!((rxda_ptr->l2cap_pkt_start == FALSE) &&
            ((cdb_ptr->crp_dsm_ptr == NULL) &&
             (cdb_ptr->crp_hdr_valid == FALSE)))) )
    {
      if ( rxda_ptr->l2cap_pkt_start != FALSE )
      {
        /*  This is a start of L2CAP packet.  */
        BT_SET_PKT_TRACER( dsm_ptr, DSM_BT_RX_L2_HOLD );

        if ( (cdb_ptr->crp_dsm_ptr != NULL) ||
             (cdb_ptr->crp_hdr_valid != FALSE) )
        {
          /*  This is a start of L2CAP packet but there is an  */
          /*  incomplete packet currently in the connection.   */
          /*  Throw the incomplete packet away at this point.  */

          if ( cdb_ptr->crp_hdr_valid != FALSE )
          {
            temp_dsm_len = dsm_length_packet( cdb_ptr->crp_dsm_ptr );
            BT_ERR( "BT L2 RX: Unexp Start; Drop L Exp %x R %x CID %x",
                    cdb_ptr->crp_l2_hdr.length,
                    temp_dsm_len,
                    cdb_ptr->crp_l2_hdr.channel_id );

            if ( cdb_ptr->crp_l2_hdr.channel_id == BT_L2_SIGNALLING_CID )
            {
              bt_l2_bad_sig_pkts_rxd++;
            }
            else
            {
              cdb_ptr->bad_pkts_rxd++;
            }
          }
          else
          {
            temp_dsm_len = dsm_length_packet( cdb_ptr->crp_dsm_ptr );
            BT_ERR( "BT L2 RX: Unexp Start; Drop No hdr R %x LCID %x",
                    temp_dsm_len, cdb_ptr->local_cid, 0 );

            cdb_ptr->bad_pkts_rxd++;
          }

          bt_l2_init_conn_cur_rx_packet( cdb_ptr );
        }

        cdb_ptr->crp_dsm_ptr = dsm_ptr;  /*  Save 1st fragment.  */
        dsm_ptr = NULL;

        BT_MSG_PYLD( "BT L2 RX: Start Pkt C %4x lcid %x",
                     dsm_length, cdb_ptr->local_cid, 0 );
      }
      else
      {
        /*  This is a continuation L2CAP packet.  */
        BT_SET_PKT_TRACER( dsm_ptr, DSM_BT_RX_L2_APPEND );

        if ( cdb_ptr->crp_dsm_ptr != NULL )
        {
          dsm_append( &cdb_ptr->crp_dsm_ptr, &dsm_ptr );
        }
        else
        {
          /*  Previously must have received  */
          /*  just the L2CAP header bytes.   */
          cdb_ptr->crp_dsm_ptr = dsm_ptr;
        }

        BT_MSG_PYLD( "BT L2 RX: Cont Pkt  C %4x LCID %x",
                     dsm_length, cdb_ptr->local_cid, 0 );
      }

      dsm_length = dsm_length_packet( cdb_ptr->crp_dsm_ptr );

      /*  Parse L2CAP header as needed.  */
      if ( (cdb_ptr->crp_hdr_valid == FALSE) &&
           (dsm_length >= sizeof( bt_l2_hdr_type )) )
      {
        if ( (bytes_pulled_up =
                dsm_pullup( &cdb_ptr->crp_dsm_ptr,
                            (void*)&cdb_ptr->crp_l2_hdr,
                            sizeof( bt_l2_hdr_type ))) ==
                  sizeof( bt_l2_hdr_type ) )
        {
          dsm_length -= sizeof( bt_l2_hdr_type );

          cdb_ptr->crp_hdr_valid = TRUE;

          BT_MSG_PYLD( "BT L2 RX: Hdr Cmplt CID %x L %x R %x",
                       cdb_ptr->crp_l2_hdr.channel_id,
                       cdb_ptr->crp_l2_hdr.length, dsm_length );

          /*  If this is normal payload data for a     */
          /*  specific CID swap the CRP data into the  */
          /*  appropriate connection table entry.      */
          if ( (cdb_ptr->crp_l2_hdr.channel_id != BT_L2_NULL_CID) &&
               (cdb_ptr->crp_l2_hdr.channel_id != BT_L2_SIGNALLING_CID) &&
               (cdb_ptr->crp_l2_hdr.channel_id !=
                  BT_L2_CONNLESS_RECEPTION_CID) )
          {
            if ( cdb_ptr->local_cid != cdb_ptr->crp_l2_hdr.channel_id )
            {
              if ( (dest_cdb_ptr =
                      bt_l2_get_conn_entry_from_cid(
                        cdb_ptr->crp_l2_hdr.channel_id )) != NULL )
              {
                bt_l2_init_conn_cur_rx_packet( dest_cdb_ptr );

                dest_cdb_ptr->crp_dsm_ptr   = cdb_ptr->crp_dsm_ptr;
                dest_cdb_ptr->crp_hdr_valid = cdb_ptr->crp_hdr_valid;
                dest_cdb_ptr->crp_l2_hdr    = cdb_ptr->crp_l2_hdr;

                cdb_ptr->crp_dsm_ptr = NULL;
                bt_l2_init_conn_cur_rx_packet( cdb_ptr );

                cdb_ptr = dest_cdb_ptr;
              }
              else
              {
                BT_ERR( "BT L2 RX: No Conn HCID %x L %x H %x",
                        cdb_ptr->crp_l2_hdr.channel_id, dsm_length,
                        rxda_ptr->handle );

                bt_l2_init_conn_cur_rx_packet( cdb_ptr );
              }
            }

            /* Header is valid only for payload less than MTU.*/
            if(cdb_ptr->crp_l2_hdr.length > cdb_ptr->in_mtu)
            {
              BT_ERR( "BT L2 RX: PDU size (0x%04x) greater than MTU (0x%04x)",
                       cdb_ptr->crp_l2_hdr.length, BT_L2_MAX_MTU, 0 );

              if ( cdb_ptr->crp_l2_hdr.channel_id == BT_L2_SIGNALLING_CID )
              {
                bt_l2_bad_sig_pkts_rxd++;
              }
              else
              {
                cdb_ptr->bad_pkts_rxd++;
              }
             
              /* This will ensure dropping rest of the data for this L2CAP PDU */
              bt_l2_init_conn_cur_rx_packet( cdb_ptr );
            }
          }
        }
        else
        {
          BT_ERR( "BT L2 RX: Bad DSM L2 Hdr PU C %x L %x H %x",
                  bytes_pulled_up, dsm_length, rxda_ptr->handle );

          cdb_ptr->bad_pkts_rxd++;

          bt_l2_init_conn_cur_rx_packet( cdb_ptr );

          dsm_free_packet( &dsm_ptr );
        }
      }

      if ( cdb_ptr->crp_hdr_valid != FALSE )
      {
        if ( dsm_length == cdb_ptr->crp_l2_hdr.length )
        {
          BT_MSG_PYLD( "BT L2 RX: Pkt Cmplt R %x HCID %x LCID %x",
                       dsm_length, cdb_ptr->crp_l2_hdr.channel_id,
                       cdb_ptr->local_cid );

          if ( (cdb_ptr->crp_l2_hdr.channel_id != BT_L2_NULL_CID) &&
               (cdb_ptr->crp_l2_hdr.channel_id !=
                  BT_L2_CONNLESS_RECEPTION_CID) )
          {
            if ( cdb_ptr->crp_l2_hdr.channel_id == BT_L2_SIGNALLING_CID )
            {
              bt_l2_process_signalling_pkt( cdb_ptr );
            }
            else
            {
              bt_l2_process_data_pkt( cdb_ptr );
            }
          }
          else
          {
            /*  NULL CID or Connectionless CID.  */
            BT_ERR( "BT L2 RX: Unexp CID %x L %x H %x",
                    cdb_ptr->crp_l2_hdr.channel_id,
                    dsm_length, rxda_ptr->handle );

          }
          bt_l2_init_conn_cur_rx_packet( cdb_ptr );
        }
        else if ( dsm_length > cdb_ptr->crp_l2_hdr.length )
        {
          /*  Received more bytes in packet than header indicated.  */
          BT_ERR( "BT L2 RX: Bad Pkt Len %x R %x HCID %x",
                  cdb_ptr->crp_l2_hdr.length, dsm_length,
                  cdb_ptr->crp_l2_hdr.channel_id );

          BT_ERR( "BT L2 RX: Bad Pkt Len LCID %x",
                  cdb_ptr->local_cid, 0, 0 );

          if ( cdb_ptr->crp_l2_hdr.channel_id == BT_L2_SIGNALLING_CID )
          {
            bt_l2_bad_sig_pkts_rxd++;
          }
          else
          {
            cdb_ptr->bad_pkts_rxd++;
          }

          bt_l2_init_conn_cur_rx_packet( cdb_ptr );
        }
      }
    }
    else
    {
      /*  Either the DSM is empty or a L2CAP Start of packet  */
      /*  was not received before this continuation packet.   */
      BT_ERR( "BT L2 RX: Bad Pkt Drop L %x S %x H %x",
              dsm_length, rxda_ptr->l2cap_pkt_start, rxda_ptr->handle );
      BT_ERR( "BT L2 RX: Bad Pkt Drop LCID %x", cdb_ptr->local_cid, 0, 0 );
      dsm_free_packet( &dsm_ptr );
    }
  }
  else
  {
    /*  Could not find consistent connection table entry for handle.  */
    BT_ERR( "BT L2 RX: No Conn H %x S %x L %x",
            rxda_ptr->handle, rxda_ptr->l2cap_pkt_start, dsm_length );

    dsm_free_packet( &dsm_ptr );
  }

}

#ifdef FEATURE_BT_AVSYNC
/*===========================================================================

FUNCTION
  bt_l2_ev_rm_tx_data_cfm

DESCRIPTION
  Processes a RM Tx Cfm event.

===========================================================================*/
void bt_l2_ev_rm_tx_data_cfm(bt_ev_msg_type*  ev_msg_ptr)
{
  bt_ev_msg_type              ev_l2_tx_data_cfm;
  bt_ev_rm_tx_data_cfm_type*  tx_data_cfm_ptr;
  bt_l2_conn_db_type*         cdb_ptr = NULL;
  uint8                       i;

  tx_data_cfm_ptr = &ev_msg_ptr->ev_msg.ev_rm_tx_cfm;
  for ( i = 0; i < BT_L2_MAX_CONN ; i++ )
  {
    if ( bt_l2_conn_db[ i ].conn_hndl == tx_data_cfm_ptr->handle &&
       ( bt_l2_conn_db[ i ].local_cid == tx_data_cfm_ptr->local_cid ) )
    {
      cdb_ptr = &bt_l2_conn_db[ i ];
      break;
    }
  }
  if(NULL == cdb_ptr)
  {
    BT_ERR("BT L2: NULL CDB_PTR...",0,0,0);
    return;
  }
  ev_l2_tx_data_cfm.ev_hdr.bt_app_id = cdb_ptr->app_id;
  ev_l2_tx_data_cfm.ev_hdr.ev_type = BT_EV_L2_TX_DATA_CFM;
  ev_l2_tx_data_cfm.ev_msg.ev_l2_tx_data_cfm.up_handle_ptr = 
                                   tx_data_cfm_ptr->up_handle_ptr;
  bt_ec_send_event(&ev_l2_tx_data_cfm);
}
#endif /* FEATURE_BT_AVSYNC */

/*===========================================================================

FUNCTION
  bt_l2_ev_rm_connected_acl

DESCRIPTION
  Processes a RM Connected ACL event.

===========================================================================*/
LOCAL void bt_l2_ev_rm_connected_acl
(
  bt_ev_msg_type*  ev_msg_ptr
)
{

  bt_ev_rm_connected_acl_type*  conn_msg_ptr =
                                  &ev_msg_ptr->ev_msg.ev_rm_conna;
  uint16                      conn_hndl    = conn_msg_ptr->handle;
  bt_bd_addr_type*            bd_addr_ptr  = &conn_msg_ptr->bd_addr;
  bt_l2_conn_db_type*         cdb_ptr;
  uint16                      ci;

  BT_MSG_API( "BT L2 EV RX: RM ConnA hndl %x",
              conn_msg_ptr->handle, 0, 0 );
  BT_BDA( MSG_API, "BT L2 EV RX: RM ConnA",
          &conn_msg_ptr->bd_addr );

  for ( ci = 0; ci < BT_L2_MAX_CONN; ci++ )
  {
    cdb_ptr = &bt_l2_conn_db[ ci ];

    if ( ( cdb_ptr->state != BT_L2_CLOSED ) &&
         ( cdb_ptr->conn_status == BT_LM_CONN_GOING_UP ) &&
         ( BT_BD_ADDRS_EQUAL( &cdb_ptr->bd_addr, bd_addr_ptr ) == TRUE ) )
    {
      cdb_ptr->conn_hndl    = conn_hndl;
      cdb_ptr->conn_status  = BT_LM_CONN_UP;
      cdb_ptr->idle_timeout = BT_L2_IDLE_TIMEOUT_MS;

      if ( cdb_ptr->pending_sig_cmd == BT_L2_SIG_CONNECTION_RQST )
      {
        /* We initiated this ACL link, let's continue with l2cap connection */
        cdb_ptr->state = BT_L2_W4_RM_AUTH;

        /* For outgoing connections enforce mode 2 security before 
         * sending connect request */
        BT_MSG_API( "BT L2 CMD TX: RM Enf Sec PSM %x AID %x LCID %x",
                    cdb_ptr->serv_sec_psm, bt_l2_app_id, 
                    cdb_ptr->local_cid );
        BT_BDA( MSG_API, "BT L2 CMD TX: RM Enf Sec PSM", &cdb_ptr->bd_addr );
        bt_cmd_rm_enforce_security_l2cap_psm( bt_l2_app_id,
                                              cdb_ptr->serv_sec_psm,
                                              &cdb_ptr->bd_addr,
                                              cdb_ptr->local_cid,
                                              BT_RM_ATZRQ_OUTGOING );

      }
      else
      {
        /* We did not initiate this connection */
        /* Update db and just hang tight */
        cdb_ptr->state     = BT_L2_W4_L2CAP_CONNECT_RQST;
      }

      if ( cdb_ptr->echo_app_id != BT_APP_ID_NULL )
      {
        /* We initiated this ACL link just for the echo request */
        bt_l2_sig_pkt_type *pkt_ptr      = bt_l2_get_free_signalling_pkt();
        if( pkt_ptr != NULL )
        {
          pkt_ptr->cmd_hdr.length = 4;
          memcpy( (byte*)pkt_ptr->cmd.echo_req.data,
                  (byte*)cdb_ptr->echo_data.data,
                  4 );
          
          cdb_ptr->state        = BT_L2_ACL_LINK_CONNECTED;
          
          bt_l2_send_signalling_pkt( BT_L2_SIG_ECHO_RQST, pkt_ptr, cdb_ptr,
                                     FALSE );
        }
        else
        {
          // No free signalling packets. Disconnect link
          bt_l2_close_l2_connection( cdb_ptr, BT_EVR_L2_UNSPECIFIED_ERROR );
        }
      }
    }
  }
}


/*===========================================================================

FUNCTION
  bt_l2_ev_rm_l2cap_psm_sec_result

DESCRIPTION
  Processes a RM L2CAP PSM Security Result event.

===========================================================================*/
LOCAL void bt_l2_ev_rm_l2cap_psm_sec_result
(
  bt_ev_msg_type*  rm_psr_ptr
)
{

  uint16               ci;
  bt_l2_conn_db_type*  cdb_ptr = NULL;
  bt_l2_sig_pkt_type*  pkt_ptr;
  bt_ev_msg_type       ev_l2_conn;

  BT_MSG_API( "BT L2 EV RX: RM PSM SecRes %x PSM %x",
              rm_psr_ptr->ev_msg.ev_rm_srpsm.secure_enough,
              rm_psr_ptr->ev_msg.ev_rm_srpsm.l2cap_psm, 0 );
  BT_BDA( MSG_API, "BT L2 EV RX: RM PSM SecRes",
          &rm_psr_ptr->ev_msg.ev_rm_srpsm.bd_addr );

  for ( ci = 0; ci < BT_L2_MAX_CONN; ci++ )
  {
    if ( ( (bt_l2_conn_db[ ci ].state == BT_L2_W4_RM_AUTH)  ||
           (bt_l2_conn_db[ ci ].state == BT_L2_W4_L2CA_CONNECT_RSP) ) &&
         (bt_l2_conn_db[ ci ].serv_sec_psm != BT_L2_PSM_INVALID)      &&
         (bt_l2_conn_db[ ci ].local_cid == 
          rm_psr_ptr->ev_msg.ev_rm_srpsm.l2cap_cid)                   &&
         ((BT_BD_ADDRS_EQUAL(
             &bt_l2_conn_db[ ci ].bd_addr,
             &rm_psr_ptr->ev_msg.ev_rm_srpsm.bd_addr )) != FALSE)     &&
         (bt_l2_conn_db[ ci ].serv_sec_psm ==
            rm_psr_ptr->ev_msg.ev_rm_srpsm.l2cap_psm) )
    {
      cdb_ptr = &bt_l2_conn_db[ ci ];

      /*  Continue with L2CAP connection setup  */
      /*  including use of security result.     */

      if ( rm_psr_ptr->ev_msg.ev_rm_srpsm.secure_enough == FALSE )
      {
        BT_MSG_HIGH( "BT L2: ServSec DISC lcid %x PSM %x",
                     cdb_ptr->local_cid,
                     rm_psr_ptr->ev_msg.ev_rm_srpsm.l2cap_psm, 0 );
        BT_BDA( MSG_HIGH, "BT L2: ServSec DISC", &cdb_ptr->bd_addr );
      }

      if ( cdb_ptr->serv_sec_local_initiated == FALSE )
      {
        /*  Remote device initiated this connection.  */
        if ( rm_psr_ptr->ev_msg.ev_rm_srpsm.secure_enough != FALSE )
        {
          /*  Security level is met.                 */
          /*  Continue with L2CAP connection setup.  */

          /*  Inform upper layer.                    */
          (void) bt_l2_send_l2ca_connect_ind(
                   &cdb_ptr->bd_addr, cdb_ptr->local_cid,
                   cdb_ptr->serv_sec_psm,
                   cdb_ptr->serv_sec_matching_id );
        }
        else
        {
          /*  Security level was not met.            */
          /*  Send appropriate connection repsonse.  */
          bt_rm_link_status_type   linkStat;

          pkt_ptr = bt_l2_get_free_signalling_pkt();
          if( pkt_ptr != NULL )
          {
            pkt_ptr->cmd.conn_rsp.dest_cid  = cdb_ptr->local_cid;
            pkt_ptr->cmd.conn_rsp.source_cid= cdb_ptr->remote_cid;
            pkt_ptr->cmd.conn_rsp.result    = BT_L2_CONN_REFUSED_SECURITY_BLOCK;
            pkt_ptr->cmd.conn_rsp.status    = BT_L2_CONN_REQ_NO_INFO;
            pkt_ptr->cmd_hdr.matching_id    =
              cdb_ptr->last_conn_rqst_rcvd_matching_id;
            
            bt_l2_send_signalling_pkt( BT_L2_SIG_CONNECTION_RSP,
                                       pkt_ptr, cdb_ptr, FALSE );
          }

          bt_l2_conn_db[ci ].disconn_reason = BT_EVR_L2_AUTHENTICATION_FAILED ;
          
          /* Get ACL link status */
          linkStat.bd_addr = bt_l2_conn_db[ ci ].bd_addr;
          bt_cmd_rm_get_link_status( bt_l2_app_id, &linkStat );

          /* ACL Link is not encrypted issue disconnect */
          if ( linkStat.sec_level == BT_RM_SL_0_NONE )
          {
                bt_l2_conn_db[ ci ].state          = BT_L2_ACL_LINK_CONNECTED;
                /* Set timer to disconnect ACL */
                bt_l2_conn_db[ ci ].disconn_reason = BT_EVR_L2_AUTHENTICATION_FAILED;
                (void)rex_set_timer( &bt_l2_conn_db[ ci ].acl_link_sec_fail_dis_timer,
                                        BT_L2_ACL_LINK_SEC_FAIL_DIS_TIME );
          }
          else
          {
              /* Security Failed but Encryption is enabled, go through normal way 
                 of disconnect */
              bt_l2_close_l2_connection(
                &bt_l2_conn_db[ci], BT_EVR_L2_AUTHENTICATION_FAILED );
          }
        }
      }
      else
      {
        /*  Local device initiated this connection.  */
        /*  Process user response for mode 2 security
         *  and update "serv_sec_*" parameters accordingly */

        if( (rm_psr_ptr->ev_msg.ev_rm_srpsm.secure_enough == FALSE))
        {
          BT_ERR("BT L2: ci: %x, secure'nuff:%x",
                 ci,
                 rm_psr_ptr->ev_msg.ev_rm_srpsm.secure_enough,
                 0);

          cdb_ptr->serv_sec_source_cid = cdb_ptr->local_cid;
          cdb_ptr->serv_sec_result     = BT_L2_CONN_REFUSED_SECURITY_BLOCK;
          cdb_ptr->serv_sec_status     = BT_L2_CONN_REQ_NO_INFO;
          cdb_ptr->serv_sec_error_code = BT_BE_CONN_REJECTED_SECURITY_FAILURE;

          /* Send L2CAP CONNECTION_FAILED event since authorization is rejected*/
          ev_l2_conn.ev_hdr.bt_app_id = cdb_ptr->app_id;
          ev_l2_conn.ev_hdr.ev_type   = BT_EV_L2_CONNECTION_FAILED;
          ev_l2_conn.ev_msg.ev_l2_conn_failed.bd_addr = cdb_ptr->bd_addr;
          ev_l2_conn.ev_msg.ev_l2_conn_failed.cid     = cdb_ptr->local_cid;
          ev_l2_conn.ev_msg.ev_l2_conn_failed.reason =
          BT_EVR_L2_AUTHENTICATION_FAILED;

          BT_MSG_API( "BT L2 EV TX: Conn Failed lcid:%x rcid:%x %x",
                                  cdb_ptr->local_cid, cdb_ptr->remote_cid,
                                  BT_EV_L2_CONNECTION_FAILED );
          bt_ec_send_event( &ev_l2_conn );

          /* Connection refused;  tear down the ACL link if necessary.*/
          bt_l2_close_l2_connection(
            cdb_ptr, BT_EVR_L2_AUTHENTICATION_FAILED );
        }
        else
        {
          cdb_ptr->serv_sec_source_cid = cdb_ptr->local_cid;
          cdb_ptr->serv_sec_result     = BT_L2_CONN_PENDING;
          cdb_ptr->serv_sec_status     = BT_L2_CONN_RSP_SUCCESS;
          cdb_ptr->serv_sec_error_code = BT_BE_SUCCESS;

          // send L2CAP_ConnectReq packet
          pkt_ptr      = bt_l2_get_free_signalling_pkt();
          if( pkt_ptr != NULL )
          {
            pkt_ptr->cmd.conn_req.source_cid = cdb_ptr->local_cid;
            cdb_ptr->state                   = BT_L2_W4_L2CAP_CONNECT_RSP;
            pkt_ptr->cmd.conn_req.psm        = 
            bt_l2_get_psm_from_reg_app_id( cdb_ptr->app_id );
          
            bt_l2_send_signalling_pkt( BT_L2_SIG_CONNECTION_RQST,
                                       pkt_ptr,
                                       cdb_ptr, FALSE );
          }
          else
          {
            // No free signalling packets. Disconnect link
            bt_l2_close_l2_connection( cdb_ptr, BT_EVR_L2_UNSPECIFIED_ERROR );
          }
        }
      }

      break;  /*  For each L2CAP connection.  */
    }
  }

  if ( cdb_ptr == NULL )
  {
    BT_ERR( "BT L2: Conn not found PSM %x CID %x",
            rm_psr_ptr->ev_msg.ev_rm_srpsm.l2cap_psm,
            rm_psr_ptr->ev_msg.ev_rm_srpsm.l2cap_cid, 0 );
    BT_BDA( ERR, "BT L2: Conn not found",
            &rm_psr_ptr->ev_msg.ev_rm_srpsm.bd_addr );
  }

}


/*===========================================================================

FUNCTION
  bt_l2_ev_rm_connect_req_acl

DESCRIPTION
  Processes a RM Connection Request ACL event.

===========================================================================*/
LOCAL void bt_l2_ev_rm_connect_req_acl
(
  bt_ev_msg_type*  ev_msg_ptr
)
{
  bt_ev_rm_connect_req_acl_type*  conn_msg_ptr =
                                    &ev_msg_ptr->ev_msg.ev_rm_creqa;
  bt_l2_conn_db_type*             cdb_ptr;

  BT_MSG_API( "BT L2 EV RX: RM ConnReqA COD %06x",
              (conn_msg_ptr->class_of_device.cod_bytes[2]<<16) |
              (conn_msg_ptr->class_of_device.cod_bytes[1]<<8)  | 
              (conn_msg_ptr->class_of_device.cod_bytes[0]),
              0, 0 );
  BT_BDA( MSG_API, "BT L2 EV RX: RM ConnReqA",
          &conn_msg_ptr->bd_addr );

  cdb_ptr = bt_l2_get_free_conn_entry();

  if ( cdb_ptr )
  {
    /* Remote is initiating a connection.  Update db and accept */
    memcpy( (uint8*) &cdb_ptr->bd_addr,
            (uint8*) &conn_msg_ptr->bd_addr,
            sizeof( bt_bd_addr_type ) );

    cdb_ptr->conn_status    = BT_LM_CONN_GOING_UP;
    cdb_ptr->initiated_conn = FALSE;
    cdb_ptr->initiated_disconn = FALSE;
    cdb_ptr->state          = BT_L2_W4_ACL_LINK;
    cdb_ptr->baseband_conn_role = BT_ROLE_SLAVE;

    /* Send the HCI Accept command */
    BT_MSG_API( "BT L2 CMD TX: RM Accept Conn", 0, 0, 0 );
    bt_cmd_rm_accept_acl( bt_l2_app_id, &conn_msg_ptr->bd_addr );
  }
  else
  {
    /* Should never occur.  L2CAP conn db should accommodate all
    ** acl links that the hw can connect to.  There is as problem somewhere
    */
    bt_cmd_rm_reject_acl( bt_l2_app_id, &conn_msg_ptr->bd_addr,
                          BT_RMRJ_NO_RESOURCE );
    BT_ERR( "BT L2: has no more conn entries", 0, 0, 0 );
  }

}


/*===========================================================================

FUNCTION
  bt_l2_ev_rm_disconnected_acl

DESCRIPTION
  Processes a RM Disconnected ACL event.

===========================================================================*/
LOCAL void bt_l2_ev_rm_disconnected_acl
(
  bt_ev_msg_type*  ev_msg_ptr
)
{
  uint16                           ci;
  bt_l2_conn_db_type*              cdb_ptr;
  bt_ev_rm_disconnected_acl_type*  disc_msg_ptr =
                               &ev_msg_ptr->ev_msg.ev_rm_disca;
  bt_ev_msg_type                   ev_l2_disc;
  bt_ev_msg_type                   ev_l2_conn_failed;
  bt_l2_reg_table_type             *reg_ptr = NULL;

  BT_MSG_API( "BT L2 EV RX: RM Disc Cmplt hndl %x rsn %x",
              disc_msg_ptr->handle,
              disc_msg_ptr->reason, 0 );

  for ( ci = 0; ci < BT_L2_MAX_CONN; ci++ )
  {
    cdb_ptr = &( bt_l2_conn_db[ ci ] );

    if ( cdb_ptr->conn_hndl == disc_msg_ptr->handle &&
         cdb_ptr->state != BT_L2_CLOSED )
    {
      if ( cdb_ptr->state > BT_L2_CONFIG )
      {
        // Connection was open, generate L2_DISCONNECTED
        ev_l2_disc.ev_hdr.bt_app_id = cdb_ptr->app_id;
        ev_l2_disc.ev_hdr.ev_type = BT_EV_L2_DISCONNECTED;
        ev_l2_disc.ev_msg.ev_l2_disc.cid = cdb_ptr->local_cid;
        ev_l2_disc.ev_msg.ev_l2_disc.reason = BT_EVR_L2_LINK_LOST;

        BT_MSG_API( "BT L2 EV TX: L2 Discon, AID %x lcid %x, ACL lost",
                    ev_l2_disc.ev_hdr.bt_app_id,
                    cdb_ptr->local_cid, 0 );
        if ( ev_l2_disc.ev_hdr.bt_app_id != BT_APP_ID_NULL )
        {
          bt_ec_send_event( &ev_l2_disc );
        }
        if ( cdb_ptr->initiated_conn == TRUE )
        {
          bt_l2_outgoing_conn_count--;
        }
        else
        {
          reg_ptr = bt_l2_find_reg_table_entry_app_id( cdb_ptr->app_id );
          if ( reg_ptr != NULL )
          {
            reg_ptr->incoming_conn_count--;
          }
        }
      }
      else if ( cdb_ptr->state == BT_L2_ACL_LINK_CONNECTED )
      {
        cdb_ptr->state        = BT_L2_CLOSED;
      }
      else if ( cdb_ptr->initiated_conn == TRUE )
      {
        // Connection was being set up, generate CONNECTION_FAILED
        ev_l2_conn_failed.ev_hdr.bt_app_id = cdb_ptr->app_id;
        ev_l2_conn_failed.ev_hdr.ev_type   = BT_EV_L2_CONNECTION_FAILED;
        ev_l2_conn_failed.ev_msg.ev_l2_conn_failed.bd_addr = cdb_ptr->bd_addr;
        ev_l2_conn_failed.ev_msg.ev_l2_conn_failed.cid     = cdb_ptr->local_cid;
        ev_l2_conn_failed.ev_msg.ev_l2_conn_failed.reason  = BT_EVR_L2_LINK_LOST;

        BT_MSG_API( "BT L2 EV TX: L2 Conn Failed, AID %x lcid %x link lost",
                    cdb_ptr->app_id, cdb_ptr->local_cid, 0 );
        if ( ev_l2_conn_failed.ev_hdr.bt_app_id != BT_APP_ID_NULL )
        {
          bt_ec_send_event( &ev_l2_conn_failed );
        }
      }
      cdb_ptr->state        = BT_L2_CLOSED;
      cdb_ptr->idle_timeout = 0;
      bt_l2_conn_closed_init_reg_table_entry_if_ok(ci);
    }
  }
}

/*===========================================================================

FUNCTION
  bt_l2_ev_rm_connection_failed_acl

DESCRIPTION
  Processes a RM Connection Failed ACL event.

===========================================================================*/
LOCAL void bt_l2_ev_rm_connection_failed_acl
(
  bt_ev_msg_type*  ev_msg_ptr
)
{
  uint16                     ci;
  bt_l2_conn_db_type*        cdb_ptr;
  bt_ev_rm_connection_failed_acl_type*  failed_msg_ptr =
                               &ev_msg_ptr->ev_msg.ev_rm_confa;
  bt_ev_msg_type             ev_l2_conn_failed;

  BT_MSG_API( "BT L2 EV RX: RM ConnFA rsn %x",
              failed_msg_ptr->reason, 0, 0 );
  BT_BDA( MSG_API, "BT L2 EV RX: RM ConnFA",
          &failed_msg_ptr->bd_addr );

  for ( ci = 0; ci < BT_L2_MAX_CONN; ci++ )
  {
    cdb_ptr = &( bt_l2_conn_db[ ci ] );

    if ( BT_BD_ADDRS_EQUAL( &cdb_ptr->bd_addr,
                            &failed_msg_ptr->bd_addr ) == TRUE )
    {
      if ( cdb_ptr->state == BT_L2_W4_ACL_LINK )
      {
        if ( cdb_ptr->initiated_conn == TRUE )
        {
          /* Send BT_EV_L2_CONNECTION_FAILED */
          ev_l2_conn_failed.ev_hdr.bt_app_id = cdb_ptr->app_id;
          ev_l2_conn_failed.ev_hdr.ev_type   = BT_EV_L2_CONNECTION_FAILED;
          ev_l2_conn_failed.ev_msg.ev_l2_conn_failed.bd_addr = cdb_ptr->bd_addr;
          ev_l2_conn_failed.ev_msg.ev_l2_conn_failed.cid     = cdb_ptr->local_cid;
          ev_l2_conn_failed.ev_msg.ev_l2_conn_failed.reason =
            failed_msg_ptr->reason;

          BT_MSG_API( "BT L2 EV TX: L2 Conn Failed, AID %x lcid %x ACL failed",
                      cdb_ptr->app_id,
                      cdb_ptr->local_cid, 0 );
          bt_ec_send_event( &ev_l2_conn_failed );
        }

        // Clear the connection entry
        // bt_l2_init_conn_db_entry( cdb_ptr );
        cdb_ptr->state        = BT_L2_CLOSED;
        cdb_ptr->idle_timeout = 0;
        bt_l2_conn_closed_init_reg_table_entry_if_ok(ci);
      }
    }
  }
}


/*===========================================================================

FUNCTION
  bt_l2_get_max_event_bytes

DESCRIPTION
  Returns the number of bytes in the largest event expected to be
  processed by L2CAP.

===========================================================================*/
LOCAL uint16 bt_l2_get_max_event_bytes
(
  void
)
{

  uint16  max_eb;

  max_eb = sizeof( bt_ev_gn_cmd_done_type );
  max_eb = MAX( max_eb, sizeof( bt_ev_rm_connect_req_acl_type ) );
  max_eb = MAX( max_eb, sizeof( bt_ev_rm_connected_acl_type ) );
  max_eb = MAX( max_eb, sizeof( bt_ev_rm_connection_failed_acl_type ) );
  max_eb = MAX( max_eb, sizeof( bt_ev_rm_l2cap_psm_sec_result_type ) );
  max_eb = MAX( max_eb, sizeof( bt_ev_rm_rx_data_acl_type ) );
  max_eb = MAX( max_eb, sizeof( bt_ev_rm_disconnected_acl_type ) );

  max_eb += sizeof( bt_ev_hdr_type );

  return( max_eb );

}


/*===========================================================================

FUNCTION
  bt_l2_process_event

DESCRIPTION
  Handles all BT driver events coming to the L2CAP layer.

===========================================================================*/
LOCAL void bt_l2_process_event
(
  bt_ev_msg_type*  ev_msg_ptr
)
{

  switch ( ev_msg_ptr->ev_hdr.ev_type )
  {
    case BT_EV_GN_CMD_DONE:
      {
        if ( ev_msg_ptr->ev_msg.ev_gn_cmd_done.cmd_status !=
             BT_CS_GN_SUCCESS )
        {
          BT_ERR( "BT L2: Bad CMD Done Stat %x Typ %x AID %x",
                  ev_msg_ptr->ev_msg.ev_gn_cmd_done.cmd_status,
                  ev_msg_ptr->ev_msg.ev_gn_cmd_done.cmd_type,
                  ev_msg_ptr->ev_hdr.bt_app_id );
        }
        break;
      }

    case BT_EV_RM_CONNECT_REQ_ACL:
      {
        bt_l2_ev_rm_connect_req_acl( ev_msg_ptr );
        break;
      }

    case BT_EV_RM_CONNECTED_ACL:
      {
        bt_l2_ev_rm_connected_acl( ev_msg_ptr );
        break;
      }

    case BT_EV_RM_CONNECTION_FAILED_ACL:
      {
        bt_l2_ev_rm_connection_failed_acl( ev_msg_ptr );
        break;
      }

    case BT_EV_RM_L2CAP_PSM_SEC_RESULT:
      {
        bt_l2_ev_rm_l2cap_psm_sec_result( ev_msg_ptr );
        break;
      }

    case BT_EV_RM_RX_DATA_ACL:
      {
        bt_l2_ev_rm_rx_data_acl( ev_msg_ptr );
        break;
      }

#ifdef FEATURE_BT_AVSYNC
  case BT_EV_RM_TX_DATA_CFM:
      {
        bt_l2_ev_rm_tx_data_cfm( ev_msg_ptr );
        break;
      }
#endif /* FEATURE_BT_AVSYNC */
    case BT_EV_RM_DISCONNECTED_ACL:
      {
        bt_l2_ev_rm_disconnected_acl( ev_msg_ptr );
        break;
      }

    default:
      {
        BT_ERR( "BT L2: Bad RX EV Type %x AID %x",
                ev_msg_ptr->ev_hdr.ev_type,
                ev_msg_ptr->ev_hdr.bt_app_id, 0 );
        break;
      }
  }

}

/*===========================================================================

FUNCTION
  bt_l2_cmd_set_params

DESCRIPTION

===========================================================================*/
LOCAL void bt_l2_cmd_set_params
(
  bt_cmd_msg_type*  l2_sp_ptr
)
{

  bt_l2_reg_table_type*  reg_ptr;
  byte                   ci = 0;
  bt_l2_conn_db_type     *cdb_ptr=NULL;

  BT_MSG_API( "BT L2 CMD RX: Set Params AID %x lcid %x PSM %x",
              l2_sp_ptr->cmd_hdr.bt_app_id,
              l2_sp_ptr->cmd_msg.cmd_l2_params.cid,
              l2_sp_ptr->cmd_msg.cmd_l2_params.psm );

  l2_sp_ptr->cmd_hdr.cmd_status = BT_CS_GN_SUCCESS;

  if ( (reg_ptr = bt_l2_find_reg_table_entry_psm(
                   l2_sp_ptr->cmd_msg.cmd_l2_params.psm )) != NULL )
  {
    if ( reg_ptr->bt_app_id != l2_sp_ptr->cmd_hdr.bt_app_id )
    {
      l2_sp_ptr->cmd_hdr.cmd_status = BT_CS_L2_INVALID_PSM;
    }
  }
  else if ( (reg_ptr = bt_l2_find_free_reg_table_entry(
               l2_sp_ptr->cmd_hdr.bt_app_id,
               l2_sp_ptr->cmd_msg.cmd_l2_params.psm )) == NULL )
  {
    l2_sp_ptr->cmd_hdr.cmd_status = BT_CS_L2_SETPRM_REG_FULL;
  }

  if ( l2_sp_ptr->cmd_hdr.cmd_status == BT_CS_GN_SUCCESS )
  {
    if ( l2_sp_ptr->cmd_msg.cmd_l2_params.cid == BT_L2_NULL_CID )
    {
      reg_ptr->psm_params = l2_sp_ptr->cmd_msg.cmd_l2_params.psm_params;
    }
    else
    {
      for ( ci=0; ci < BT_L2_MAX_CONN; ci++ )
      {
        if ( ( bt_l2_conn_db[ ci ].app_id == reg_ptr->bt_app_id ) &&
             ( bt_l2_conn_db[ ci ].conn_hndl != BT_HC_NULL_CONN_HANDLE ) &&
             ( bt_l2_conn_db[ ci ].local_cid ==
                                 l2_sp_ptr->cmd_msg.cmd_l2_params.cid ) )
        {
          // If conn is not open, do not process this command.
          if ( bt_l2_conn_db[ ci ].state == BT_L2_OPEN )
          {
            cdb_ptr = &bt_l2_conn_db[ ci ];
            cdb_ptr->state = BT_L2_RECONFIG;

            BT_MSG_DEBUG("BT L2: Start Recfg, lcid:%x",
                          bt_l2_conn_db[ci].local_cid, 0, 0 );

            bt_l2_l2ca_config_req( cdb_ptr->local_cid,
                     l2_sp_ptr->cmd_msg.cmd_l2_params.psm_params.in_mtu,
                     l2_sp_ptr->cmd_msg.cmd_l2_params.psm_params.out_qos,
                     l2_sp_ptr->cmd_msg.cmd_l2_params.psm_params.out_flush_to,
                                   cdb_ptr->link_to );
          }
          // If conn is not open and the command was just for this conn
          // return an error.
          else if ( bt_l2_conn_db[ ci ].state != BT_L2_OPEN )
          {
            l2_sp_ptr->cmd_hdr.cmd_status = BT_CS_L2_CONN_NOT_UP;
          }
          break;
        }
      }
    }
  }

}


/*===========================================================================

FUNCTION
  bt_l2_cmd_register

DESCRIPTION
  Register a protocol with L2CAP.

===========================================================================*/
LOCAL void bt_l2_cmd_register
(
  bt_cmd_msg_type*  l2_reg_ptr
)
{

  bt_l2_reg_table_type*  reg_ptr;

  l2_reg_ptr->cmd_hdr.cmd_status = BT_CS_GN_SUCCESS;

  BT_MSG_API( "BT L2 CMD RX: Reg AID %x PSM %x",
              l2_reg_ptr->cmd_hdr.bt_app_id,
              l2_reg_ptr->cmd_msg.cmd_l2_reg.psm, 0);

  if ( BT_L2_IS_PSM_VALID( l2_reg_ptr->cmd_msg.cmd_l2_reg.psm ) )
  {
    if ( (reg_ptr = bt_l2_find_reg_table_entry_psm(
                      l2_reg_ptr->cmd_msg.cmd_l2_reg.psm )) != NULL )
    {
      if ( reg_ptr->bt_app_id == l2_reg_ptr->cmd_hdr.bt_app_id )
      {
        if ( reg_ptr->registered == FALSE )
        {
          reg_ptr->registered = TRUE;
        }
        else
        {
          l2_reg_ptr->cmd_hdr.cmd_status = BT_CS_L2_REG_DUPLICATE;
        }
      }
      else
      {
        l2_reg_ptr->cmd_hdr.cmd_status = BT_CS_L2_INVALID_PSM;
      }
    }
    else
    {
      /*  Create a new registration table entry for PSM.  */
      if ( (reg_ptr = bt_l2_find_free_reg_table_entry(
              l2_reg_ptr->cmd_hdr.bt_app_id,
              l2_reg_ptr->cmd_msg.cmd_l2_reg.psm )) != NULL )
      {
        reg_ptr->registered = TRUE;
      }
      else
      {
        l2_reg_ptr->cmd_hdr.cmd_status = BT_CS_L2_REG_FULL;
      }
    }
  }
  else
  {
    l2_reg_ptr->cmd_hdr.cmd_status = BT_CS_L2_INVALID_PSM;
  }

  if ( l2_reg_ptr->cmd_hdr.cmd_status == BT_CS_GN_SUCCESS )
  {
    bt_l2_cur_regd_count++;
    if ( bt_l2_cur_regd_count == 1 )
    {
      BT_MSG_API( "BT L2 CMD TX: RM RegA AID %x",
                  bt_l2_app_id, 0, 0 );
      bt_cmd_rm_register_acl( bt_l2_app_id  );
    }
  }

}


/*===========================================================================

FUNCTION
  bt_l2_cmd_connect

DESCRIPTION

===========================================================================*/
LOCAL void bt_l2_cmd_connect
(
 bt_cmd_msg_type*  l2_conn_ptr,
 boolean cid_provided
)
{

  bt_lm_conn_state_type         acl_conn_state = BT_LM_CONN_DOWN;
  bt_l2_conn_db_type*           cdb_ptr = NULL;
  byte                          ci = 0;
  bt_l2_reg_table_type*         reg_ptr;
  uint16                        conn_hndl = BT_HC_NULL_CONN_HANDLE;
  bt_ev_msg_type                ev_l2_conn_failed;

  BT_MSG_API( "BT L2 CMD RX: Conn AID %x PSM %x",
              l2_conn_ptr->cmd_hdr.bt_app_id,
              l2_conn_ptr->cmd_msg.cmd_l2_conn.psm, 0);

  BT_BDA( MSG_API, "BT L2 CMD RX: Conn",
          &l2_conn_ptr->cmd_msg.cmd_l2_conn.bd_addr );

  l2_conn_ptr->cmd_hdr.cmd_status = BT_CS_GN_SUCCESS;

  // Search L2CAP connection table for matching BD address
  for ( ci = 0; ci < BT_L2_MAX_CONN; ci++ )
  {
    // Found matching BD address.  Let's see if an ACL Link is up
    if ( BT_BD_ADDRS_EQUAL( &bt_l2_conn_db[ ci ].bd_addr,
                            &l2_conn_ptr->cmd_msg.cmd_l2_conn.bd_addr ) )
    {
      if ( bt_l2_conn_db[ ci ].state > BT_L2_W4_ACL_LINK )
      {
        // no need to create a connection
        acl_conn_state = bt_l2_conn_db[ ci ].conn_status;
        conn_hndl = bt_l2_conn_db[ ci ].conn_hndl;
        if ( bt_l2_conn_db[ ci ].state == BT_L2_ACL_LINK_CONNECTED )
        {
          cdb_ptr = &bt_l2_conn_db[ ci ];
          (void)rex_clr_timer( &bt_l2_conn_db[ci].acl_link_disconnect_timer );
        }
        break;
      }
    }
  }

  if ( cdb_ptr == NULL )
  {
    // get a free l2cap db entry
    cdb_ptr = bt_l2_get_free_conn_entry();
  }

  if ( cdb_ptr != NULL )
  {
    // let's use this l2cap db entry
    if( cid_provided )
    {
      cdb_ptr->local_cid     = l2_conn_ptr->cmd_msg.cmd_l2_conn_imd_cid.cid;
    }
    else
    {
      if ( bt_l2_next_local_cid == BT_L2_CID_LAST )
      {
        bt_l2_next_local_cid = BT_L2_CID_FIRST;
      }
      else
      {
        bt_l2_next_local_cid++;
      }
      cdb_ptr->local_cid     = bt_l2_next_local_cid;
    }
    cdb_ptr->conn_status     = acl_conn_state;
    cdb_ptr->initiated_conn  = TRUE;
    cdb_ptr->initiated_disconn = FALSE;
    cdb_ptr->app_id          = l2_conn_ptr->cmd_hdr.bt_app_id;
    memcpy( (byte*) &cdb_ptr->bd_addr,
            (byte*) &l2_conn_ptr->cmd_msg.cmd_l2_conn.bd_addr,
            sizeof( bt_bd_addr_type ) );

    cdb_ptr->serv_sec_local_initiated = TRUE;
    cdb_ptr->serv_sec_psm             = l2_conn_ptr->cmd_msg.cmd_l2_conn.psm;

    if ( ci < BT_L2_MAX_CONN )
    {
      cdb_ptr->baseband_conn_role = bt_l2_conn_db[ci].baseband_conn_role;
    }

    if ( (reg_ptr = bt_l2_find_reg_table_entry_psm(
                    l2_conn_ptr->cmd_msg.cmd_l2_conn.psm )) != NULL )
    {
      cdb_ptr->in_mtu       = reg_ptr->psm_params.in_mtu;
      cdb_ptr->out_qos      = reg_ptr->psm_params.out_qos;
      cdb_ptr->out_flush_to = reg_ptr->psm_params.out_flush_to;
    }
    else
    {
      bt_l2_find_free_reg_table_entry( l2_conn_ptr->cmd_hdr.bt_app_id,
                                       l2_conn_ptr->cmd_msg.cmd_l2_reg.psm );
    }

    // connection is up.  send the authorization request.
    if ( acl_conn_state == BT_LM_CONN_UP )
    {
      /* Enforce mode 2 security check before sending L2CAP connect 
         request */
      cdb_ptr->state        = BT_L2_W4_RM_AUTH;
      cdb_ptr->idle_timeout = BT_L2_IDLE_TIMEOUT_MS;
      cdb_ptr->conn_hndl    = conn_hndl;

      BT_MSG_API( "BT L2 CMD TX: RM Enf Sec PSM %x AID %x LCID %x",
                  cdb_ptr->serv_sec_psm, bt_l2_app_id, 
                  cdb_ptr->local_cid );
      BT_BDA( MSG_API, "BT L2 CMD TX: RM Enf Sec PSM", &cdb_ptr->bd_addr );
      bt_cmd_rm_enforce_security_l2cap_psm( bt_l2_app_id,
                                            cdb_ptr->serv_sec_psm,
                                            &cdb_ptr->bd_addr,
                                            cdb_ptr->local_cid,
                                            BT_RM_ATZRQ_OUTGOING);
    }

    // connection is going up
    else if ( acl_conn_state >= BT_LM_CONN_GOING_UP )
    {
      // Send the l2cap connect command after the connetion is established.
      cdb_ptr->state           = BT_L2_W4_ACL_LINK;
      cdb_ptr->pending_sig_cmd = BT_L2_SIG_CONNECTION_RQST;
    }

    // no connection; let's establish one first
    else
    {
      /* Send the l2cap connect command after we connect with the device. */

      cdb_ptr->conn_status        = BT_LM_CONN_GOING_UP;
      cdb_ptr->state              = BT_L2_W4_ACL_LINK;
      cdb_ptr->pending_sig_cmd    = BT_L2_SIG_CONNECTION_RQST;
      cdb_ptr->baseband_conn_role = BT_ROLE_MASTER;

      bt_cmd_rm_connect_acl( bt_l2_app_id,
                             &l2_conn_ptr->cmd_msg.cmd_l2_conn.bd_addr );
    }
  }  // Conn DB entry found
  else
  {
    /* Send L2CAP Connection Failed event */
    ev_l2_conn_failed.ev_hdr.ev_type   = BT_EV_L2_CONNECTION_FAILED;
    ev_l2_conn_failed.ev_hdr.bt_app_id = l2_conn_ptr->cmd_hdr.bt_app_id;
    ev_l2_conn_failed.ev_msg.ev_l2_conn_failed.bd_addr =
      l2_conn_ptr->cmd_msg.cmd_l2_conn.bd_addr;
    if( cid_provided )
    {
      ev_l2_conn_failed.ev_msg.ev_l2_conn_failed.cid =
        l2_conn_ptr->cmd_msg.cmd_l2_conn_imd_cid.cid;
    }
    else
    {
      // This isn't quite right, but will most likely get the correct cid:
      if ( bt_l2_next_local_cid == BT_L2_CID_FIRST )
      {
        ev_l2_conn_failed.ev_msg.ev_l2_conn_failed.cid = BT_L2_CID_LAST;
      }
      else
      {
        ev_l2_conn_failed.ev_msg.ev_l2_conn_failed.cid = bt_l2_next_local_cid-1;
      }
    }
    ev_l2_conn_failed.ev_msg.ev_l2_conn_failed.reason =
      BT_EVR_L2_LOCAL_REJECT_CONNECTION;

    BT_MSG_API( "BT L2 EV TX: L2 Conn Failed, AID %x no entries",
                ev_l2_conn_failed.ev_hdr.bt_app_id, 0, 0 );
    bt_ec_send_event( &ev_l2_conn_failed );
  }

}


/*===========================================================================

FUNCTION
  bt_l2_cmd_write

DESCRIPTION
  Upper protocol calls this function to request the transfer of data
  across the channel.  If the length of the data exceeds the OutMTU,
  only the first OutMTU bytes are sent.

===========================================================================*/
void bt_l2_cmd_write
(
  bt_cmd_msg_type*  l2_write_ptr
)
{

  bt_l2_hdr_type       l2_hdr;
  uint32               length;
  dsm_item_type*       dsm_ptr;
  uint16               total_bytes_to_dup, offset, bytes_duplicated;
  uint16               bytes_to_copy;
  dsm_item_type*       dup_dsm_ptr = NULL;
  bt_hc_pkt_boundary_flag_type    l2cap_pkt_start;
  bt_l2_conn_db_type*  cdb_ptr;

  l2_write_ptr->cmd_hdr.cmd_status = BT_CS_GN_SUCCESS;
  dsm_ptr = l2_write_ptr->cmd_msg.cmd_l2_write.dsm_ptr;

  length = dsm_length_packet( dsm_ptr );

  BT_MSG_API_PYLD( "BT L2 CMD RX: Write AID %x CID %x L %x",
                   l2_write_ptr->cmd_hdr.bt_app_id,
                   l2_write_ptr->cmd_msg.cmd_l2_write.cid,
                   length);

  if ( ((cdb_ptr = bt_l2_get_conn_entry_from_cid(
           l2_write_ptr->cmd_msg.cmd_l2_write.cid)) != NULL) &&
       (cdb_ptr->state == BT_L2_OPEN) )
  {
    if ( length <= cdb_ptr->out_mtu )
    {
#ifdef BT_SWDEV_QXDM_LOG_MSG
      bt_l2_send_payload_tx_log( dsm_ptr, cdb_ptr->local_cid );
#endif
      /*  Add L2CAP header.  */
      l2_hdr.length     = length;
      l2_hdr.channel_id = cdb_ptr->remote_cid;
      dsm_pushdown_packed( &dsm_ptr, &l2_hdr, sizeof( bt_l2_hdr_type ),
                           DSM_DS_SMALL_ITEM_POOL );

      length += sizeof( bt_l2_hdr_type );
      if(l2_write_ptr->cmd_msg.cmd_l2_write.frame_handle)
      {
         BT_MSG_API_PYLD("BT L2: Start of l2cap flushable packet", 0, 0, 0);
         l2cap_pkt_start = BT_HC_PKT_BOUNDARY_START_FLUSHABLE;
      }
      else
      {
         l2cap_pkt_start = BT_HC_PKT_BOUNDARY_START;
      }
      if ( length <= bt_l2_max_hci_acl_len )
      {
        BT_SET_PKT_TRACER( dsm_ptr, DSM_BT_TX_L2 );

        BT_MSG_PYLD( "BT L2 TX: Not Frag L %x CID %x",
                     length, cdb_ptr->local_cid, 0 );

        bt_l2_send_l2cap_pkt( dsm_ptr, cdb_ptr,  
                              l2_write_ptr->cmd_msg.cmd_l2_write.frame_handle ,
                              l2cap_pkt_start );
      }
      else
      {

        /*  Packet too big for L2CAP. */

        /* Fragment into smaller (MTU sized) packets by duplicating
        ** the entire DSM.  Send all packets except the last dup'd
        ** DSM, which will be sent after the while loop.  Free the
        ** original DSM since all data has been dup'd.
        */
        total_bytes_to_dup = length;
        offset             = 0;
        BT_MSG_PYLD( "BT L2 TX: Frags L %x CID %x",
                     length, cdb_ptr->local_cid, 0 );
        do
        {
          if ( total_bytes_to_dup > bt_l2_max_hci_acl_len )
          {
            bytes_to_copy = bt_l2_max_hci_acl_len;
          }
          else
          {
            bytes_to_copy = total_bytes_to_dup;
          }
          bytes_duplicated = dsm_dup_packet( &dup_dsm_ptr,
                                             dsm_ptr,
                                             offset,
                                             bytes_to_copy );
          if ( dup_dsm_ptr == NULL )
          {
            BT_ERR("BT L2 TX: Out of DUP DSM'S", 0,0,0);
          }

          total_bytes_to_dup -= bytes_duplicated;
          offset += bytes_duplicated;

          BT_SET_PKT_TRACER( dup_dsm_ptr, DSM_BT_TX_L2 );

          BT_MSG_PYLD( "BT L2 TX: Seg L to copy %x dup'd %x",
                       bytes_to_copy, bytes_duplicated, 0 );
          BT_MSG_PYLD( "BT L2 TX: Seg L to dup  %x Offset %x",
                       total_bytes_to_dup, offset, 0 );

          if(total_bytes_to_dup == 0)
          {
            bt_l2_send_l2cap_pkt( dup_dsm_ptr, cdb_ptr, 
                              l2_write_ptr->cmd_msg.cmd_l2_write.frame_handle,
                              l2cap_pkt_start);
          }
          else
          {
            bt_l2_send_l2cap_pkt( dup_dsm_ptr, cdb_ptr, NULL, l2cap_pkt_start);
          }
          if( l2cap_pkt_start ==  BT_HC_PKT_BOUNDARY_START_FLUSHABLE)
          {
             l2cap_pkt_start = BT_HC_PKT_BOUNDARY_CONT_FLUSHABLE;
          }
          else if( l2cap_pkt_start ==  BT_HC_PKT_BOUNDARY_START) 
          {
             l2cap_pkt_start = BT_HC_PKT_BOUNDARY_FRAGMENT;
          }
        } while ( total_bytes_to_dup != 0 );

        dsm_free_packet( &dsm_ptr );
      }
    }
    else
    {
      BT_MSG_DEBUG( "BT L2: Drop L2 data MTU %x cid %x len %x",
                    cdb_ptr->out_mtu,
                    l2_write_ptr->cmd_msg.cmd_l2_write.cid,
                    length );

      l2_write_ptr->cmd_hdr.cmd_status = BT_CS_L2_WRITE_BIG_PKT_SIZE;
      dsm_free_packet( &dsm_ptr );
    }
  }
  else
  {
    BT_MSG_DEBUG( "BT L2: Drop L2 data bad CID %x len %x",
      l2_write_ptr->cmd_msg.cmd_l2_write.cid, length, 0 );

    l2_write_ptr->cmd_hdr.cmd_status = BT_CS_L2_WRITE_CID_NOT_FOUND;
    dsm_free_packet( &dsm_ptr );
  }

  // BT_TBD: To maintain spec L2CAP API, consider...
  //          bt_l2_send_l2ca_data_cfm(
  //            BT_L2_EC_DATA_WRITE_CFM, result, local_cid, len );

}

/*===========================================================================

FUNCTION
  bt_l2_cmd_disconnect

DESCRIPTION
  Request to disconnect a specified channel.

===========================================================================*/
LOCAL void bt_l2_cmd_disconnect
(
  bt_cmd_msg_type*  l2_disc_ptr
)
{

  bt_l2_sig_pkt_type*       pkt_ptr = NULL;
  bt_l2_conn_db_type*       cdb_ptr = NULL;
  bt_l2_reg_table_type*     reg_ptr = NULL;
  int32                     ci;
  bt_ev_msg_type            ev_l2_conn_failed;

  BT_MSG_API( "BT L2 CMD RX: Disconn AID %x lcid %x",
              l2_disc_ptr->cmd_hdr.bt_app_id,
              l2_disc_ptr->cmd_msg.cmd_l2_disc.cid, 0 );

  l2_disc_ptr->cmd_hdr.cmd_status = BT_CS_L2_DISC_CID_NOT_FOUND;

  for ( ci = 0; ci < BT_L2_MAX_CONN; ci++ )
  {
    if ( (bt_l2_conn_db[ ci ].local_cid == l2_disc_ptr->cmd_msg.cmd_l2_disc.cid) &&
         (bt_l2_conn_db[ ci ].state > BT_L2_W4_ACL_LINK) )
    {
      cdb_ptr = &bt_l2_conn_db[ ci ];
    }
  }

  if ( ( cdb_ptr != NULL ) )
  {
    if( cdb_ptr->state == BT_L2_W4_ACL_LINK ||
        cdb_ptr->state == BT_L2_ACL_LINK_CONNECTED ||
        (cdb_ptr->state == BT_L2_W4_RM_AUTH && cdb_ptr->initiated_conn == TRUE))
    {
      /* We haven't sent a connect request yet. Send the event first before cleanup. */
      ev_l2_conn_failed.ev_hdr.ev_type   = BT_EV_L2_CONNECTION_FAILED;
      ev_l2_conn_failed.ev_hdr.bt_app_id = cdb_ptr->app_id;
      ev_l2_conn_failed.ev_msg.ev_l2_conn_failed.bd_addr = cdb_ptr->bd_addr;
      ev_l2_conn_failed.ev_msg.ev_l2_conn_failed.cid     = cdb_ptr->local_cid;

      ev_l2_conn_failed.ev_msg.ev_l2_conn_failed.reason =
        BT_EVR_L2_LOCAL_REJECT_CONNECTION;
      cdb_ptr->serv_sec_error_code = BT_BE_UNSPECIFIED_ERROR;

      BT_MSG_API( "BT L2 EV TX: Conn Failed lcid %x rslt %x SE %x",
                  cdb_ptr->local_cid, BT_EVR_L2_LOCAL_REJECT_CONNECTION, 0 );
      bt_ec_send_event( &ev_l2_conn_failed );


      cdb_ptr->initiated_disconn = TRUE;
      bt_l2_close_l2_connection( cdb_ptr, BT_EVR_L2_LOCAL_REJECT_CONNECTION ); 
    }
    else if( cdb_ptr->state < BT_L2_W4_L2CAP_DISCONNECT_RSP )
    {
      /* For all other cases, we've already sent (or received) a connection
       * request. So send a discon request here */
      cdb_ptr->initiated_disconn = TRUE;
      pkt_ptr = bt_l2_get_free_signalling_pkt();
      if( pkt_ptr != NULL )
      {
        pkt_ptr->cmd.disconn_req.dest_cid   = cdb_ptr->remote_cid;
        pkt_ptr->cmd.disconn_req.source_cid = cdb_ptr->local_cid;

        bt_l2_send_signalling_pkt( BT_L2_SIG_DISCONNECTION_RQST,
                                   pkt_ptr,
                                   cdb_ptr, FALSE );

        l2_disc_ptr->cmd_hdr.cmd_status = BT_CS_GN_SUCCESS;
        cdb_ptr->state = BT_L2_W4_L2CAP_DISCONNECT_RSP;
        cdb_ptr->idle_timeout = BT_L2_IDLE_TIMEOUT_MS;

        if ( cdb_ptr->initiated_conn == FALSE )
        {
          reg_ptr = bt_l2_find_reg_table_entry_app_id( cdb_ptr->app_id );
          if ( ( reg_ptr == NULL ) || ( reg_ptr->incoming_conn_count < 1 ) )
          {
            l2_disc_ptr->cmd_hdr.cmd_status = BT_CS_L2_DISC_ERROR;
          }
          else
          {
            reg_ptr->incoming_conn_count--;
          }
        }
        else if ( bt_l2_outgoing_conn_count < 1 )
        {
          l2_disc_ptr->cmd_hdr.cmd_status = BT_CS_L2_DISC_ERROR;
        }
        else
        {
          bt_l2_outgoing_conn_count--;
        }
      }
      else
      {
        BT_ERR("BT L2: Out of signalling packets",0,0,0);
        // No free signalling packets. Disconnect link
        bt_l2_close_l2_connection( cdb_ptr, BT_EVR_L2_UNSPECIFIED_ERROR );
      }
    }
    else
    {
      BT_ERR("BT L2: disconnect invalid state %x. CID %x", 
             cdb_ptr->state, l2_disc_ptr->cmd_msg.cmd_l2_disc.cid, 0);
    }
  }
  else
  {
    BT_ERR("BT L2: disconnect invalid CID %x", 
           l2_disc_ptr->cmd_msg.cmd_l2_disc.cid, 0, 0);
  }
}

/*===========================================================================

FUNCTION
  bt_l2_cmd_disconnect_immediate

DESCRIPTION
  Request to disconnect a specified channel.

===========================================================================*/
LOCAL void bt_l2_cmd_disconn_immediate
(
  bt_cmd_msg_type*  l2_disc_ptr
)
{

  bt_l2_conn_db_type*   cdb_ptr = NULL;
  int32                 ci;

  for ( ci = 0; ci < BT_L2_MAX_CONN; ci++ )
  {
    if ( (bt_l2_conn_db[ ci ].local_cid == l2_disc_ptr->cmd_msg.cmd_l2_disc.cid) &&
         (bt_l2_conn_db[ ci ].state > BT_L2_W4_ACL_LINK) )
    {
      cdb_ptr = &bt_l2_conn_db[ ci ];
    }
  }

  if ( cdb_ptr != NULL )
  {
    cdb_ptr->disconn_acl_immediate = TRUE;
    bt_l2_cmd_disconnect( l2_disc_ptr );
  }

}

/*===========================================================================

FUNCTION
  bt_l2_cmd_unregister

DESCRIPTION

===========================================================================*/
LOCAL void bt_l2_cmd_unregister
(
  bt_cmd_msg_type*  l2_unreg_ptr
)
{

  bt_l2_reg_table_type*  reg_ptr;
  uint8                  i;

  BT_MSG_API( "BT L2 CMD RX: Unreg AID %x PSM %x",
              l2_unreg_ptr->cmd_hdr.bt_app_id,
              l2_unreg_ptr->cmd_msg.cmd_l2_unreg.psm, 0 );

  l2_unreg_ptr->cmd_hdr.cmd_status = BT_CS_GN_SUCCESS;

  if ( ((reg_ptr = bt_l2_find_reg_table_entry_psm(
          l2_unreg_ptr->cmd_msg.cmd_l2_unreg.psm )) != NULL) &&
       (bt_l2_is_psm_registered(l2_unreg_ptr->cmd_msg.cmd_l2_unreg.psm) == TRUE) )
  {
    if ( reg_ptr->bt_app_id != l2_unreg_ptr->cmd_hdr.bt_app_id )
    {
      l2_unreg_ptr->cmd_hdr.cmd_status = BT_CS_L2_INVALID_PSM;
    }
  }
  else
  {
    l2_unreg_ptr->cmd_hdr.cmd_status = BT_CS_L2_UNREG_NOT_FOUND;
  }

  if ( l2_unreg_ptr->cmd_hdr.cmd_status == BT_CS_GN_SUCCESS )
  {
    reg_ptr->registered = FALSE;
    bt_l2_cur_regd_count--;
    if ( bt_l2_cur_regd_count == 0 )
    {
      BT_MSG_API( "BT L2 CMD TX: RM UnregA AID %x",
                  bt_l2_app_id, 0, 0 );
      bt_cmd_rm_unregister_acl( bt_l2_app_id  );
    }
    for ( i=0; i< BT_L2_MAX_CONN; ++i )
    {
      if ( ( bt_l2_conn_db[i].app_id == reg_ptr->bt_app_id ) &&
           ( bt_l2_conn_db[i].state != BT_L2_CLOSED ) )
      {
        break;
      }
    }
    if ( i == BT_L2_MAX_CONN )
    {
      bt_l2_init_reg_table_entry( reg_ptr );
    }
  }

}


/*===========================================================================

FUNCTION
  bt_l2_cmd_ping

DESCRIPTION

===========================================================================*/
LOCAL void bt_l2_cmd_ping
(
  bt_cmd_msg_type*  l2_ping_ptr
)
{
  uint16                ci;
  bt_ev_msg_type        ev_l2_ping;
  boolean               existing_rqst  = FALSE;
  bt_l2_conn_db_type*   cdb_ptr        = NULL;
  bt_l2_sig_pkt_type*   pkt_ptr        = NULL;
  boolean               success        = FALSE;

  BT_MSG_API( "BT L2 CMD RX: Ping AID %x len %x",
              l2_ping_ptr->cmd_hdr.bt_app_id,
              l2_ping_ptr->cmd_msg.cmd_l2_ping.length, 0 );

  BT_BDA( MSG_API, "BT L2 CMD RX: Ping",
          &l2_ping_ptr->cmd_msg.cmd_l2_ping.bd_addr );

  l2_ping_ptr->cmd_hdr.cmd_status = BT_CS_GN_SUCCESS;

  /* Find an open L2CAP channel */
  for ( ci = 0; ci < BT_L2_MAX_CONN; ci++ )
  {
    if ( bt_l2_conn_db[ ci ].state != BT_L2_CLOSED &&
         ( BT_BD_ADDRS_EQUAL( &( l2_ping_ptr->cmd_msg.cmd_l2_ping.bd_addr ),
                              &( bt_l2_conn_db[ ci ].bd_addr ) ) ) )
    {
      if ( bt_l2_conn_db[ ci ].echo_app_id != BT_APP_ID_NULL )
      {
        existing_rqst = TRUE;
        cdb_ptr = NULL;
        break;
      }
      /* Found, no need to create a connection */
      cdb_ptr = &bt_l2_conn_db[ ci ];
      if ( bt_l2_conn_db[ ci ].state == BT_L2_ACL_LINK_CONNECTED )
      {
        (void)rex_clr_timer( &bt_l2_conn_db[ci].acl_link_disconnect_timer );
      }
    }
  }

  if ( cdb_ptr != NULL )
  {
    cdb_ptr->echo_app_id = l2_ping_ptr->cmd_hdr.bt_app_id;
    /* If the link is coming up, wait until it's up to send the echo rqst */
    if ( cdb_ptr->state != BT_L2_W4_ACL_LINK )
    {
      pkt_ptr = bt_l2_get_free_signalling_pkt();
      if ( pkt_ptr != NULL )
      {
        pkt_ptr->cmd_hdr.length = MIN(l2_ping_ptr->cmd_msg.cmd_l2_ping.length,
                                      BT_L2_MAX_ECHO_DATA_LEN);
        memcpy( (byte*)pkt_ptr->cmd.echo_req.data,
                (byte*)l2_ping_ptr->cmd_msg.cmd_l2_ping.data,
                MIN(l2_ping_ptr->cmd_msg.cmd_l2_ping.length,BT_L2_MAX_ECHO_DATA_LEN) );
        bt_l2_send_signalling_pkt( BT_L2_SIG_ECHO_RQST, pkt_ptr, cdb_ptr,
                                   FALSE );
        
        success = TRUE;
      }
      else
      {
        // No free signalling packets. Disconnect link
        bt_l2_close_l2_connection( cdb_ptr, BT_EVR_L2_UNSPECIFIED_ERROR );
        
        BT_ERR( "BT L2: no free sig pkt, cid:%x, state:%x",
                cdb_ptr->local_cid, cdb_ptr->state, 0 );
      }
    }
    else
    {
      memcpy( (byte*)cdb_ptr->echo_data.data,
              (byte*)l2_ping_ptr->cmd_msg.cmd_l2_ping.data,
              MIN(l2_ping_ptr->cmd_msg.cmd_l2_ping.length,BT_L2_MAX_ECHO_DATA_LEN) );
      success = TRUE;
    }
  }
  else if ( existing_rqst == FALSE )
  {
    /* Create a connection with this device */
    cdb_ptr = bt_l2_get_free_conn_entry();
    if ( cdb_ptr != NULL )
    {
      memcpy( (byte*) &cdb_ptr->bd_addr,
              (byte*) &l2_ping_ptr->cmd_msg.cmd_l2_ping.bd_addr,
              sizeof( bt_bd_addr_type ) );

      cdb_ptr->initiated_conn     = TRUE;
      cdb_ptr->initiated_disconn  = TRUE;
      cdb_ptr->app_id             = cdb_ptr->echo_app_id;
      cdb_ptr->conn_status        = BT_LM_CONN_GOING_UP;
      cdb_ptr->state              = BT_L2_W4_ACL_LINK;
      cdb_ptr->baseband_conn_role = BT_ROLE_MASTER;
      cdb_ptr->pending_sig_cmd    = BT_L2_SIG_ECHO_RQST;
        
      bt_cmd_rm_connect_acl( bt_l2_app_id,
                             &cdb_ptr->bd_addr );
      success = TRUE;
    }
    else
    {
      BT_ERR( "BT L2: no L2 chan to remote dev and no free db entries",
              0, 0, 0 );
    }
  }
  else
  {
    BT_ERR( "BT L2: pending echo rqst", 0, 0, 0 );
  }

  if ( ! success )
  {
    ev_l2_ping.ev_hdr.ev_type            = BT_EV_L2_PING;
    ev_l2_ping.ev_hdr.bt_app_id          = l2_ping_ptr->cmd_hdr.bt_app_id;
    ev_l2_ping.ev_msg.ev_l2_ping.bd_addr =
      l2_ping_ptr->cmd_msg.cmd_l2_ping.bd_addr;
    ev_l2_ping.ev_msg.ev_l2_ping.length  = 0;
    ev_l2_ping.ev_msg.ev_l2_ping.reason  = BT_EVR_L2_PING_FAILED;

    BT_MSG_API( "BT L2 EV TX: L2 Ping failed AID %x",
                ev_l2_ping.ev_hdr.bt_app_id, 0, 0 );
    bt_ec_send_event( &ev_l2_ping );
  }
}

/*===========================================================================

FUNCTION
  bt_l2_cmd_disable_timeout

DESCRIPTION

===========================================================================*/
LOCAL void bt_l2_cmd_disable_timeout
(
  bt_cmd_msg_type*  l2_disable_timeout_ptr
)
{
  bt_l2_conn_db_type*       cdb_ptr;

  BT_MSG_API( "BT L2 CMD RX: Disable Timeout AID %x lcid %x",
              l2_disable_timeout_ptr->cmd_hdr.bt_app_id,
              l2_disable_timeout_ptr->cmd_msg.cmd_l2_disable_timeout.cid, 0 );
  cdb_ptr =
    bt_l2_get_conn_entry_from_cid( l2_disable_timeout_ptr ->
                                   cmd_msg.cmd_l2_disable_timeout.cid );

  if( cdb_ptr == NULL )
  {
    l2_disable_timeout_ptr->cmd_hdr.cmd_status = BT_CS_L2_DIS_TO_CID_NOT_FOUND;
  }
  else if( cdb_ptr->app_id != l2_disable_timeout_ptr->cmd_hdr.bt_app_id )
  {
    l2_disable_timeout_ptr->cmd_hdr.cmd_status = BT_CS_GN_BAD_APP_ID;
  }
  else if( cdb_ptr->state < BT_L2_OPEN )
  {
    l2_disable_timeout_ptr->cmd_hdr.cmd_status = BT_CS_L2_CONN_NOT_UP;
  }
  else
  {
    l2_disable_timeout_ptr->cmd_hdr.cmd_status = BT_CS_GN_SUCCESS;
    cdb_ptr->idle_timeout = 0;
  }
}

/*-------------------------------------------------------------------------*/
/*                                                                         */
/*                        External Functions                               */
/*                                                                         */
/*-------------------------------------------------------------------------*/

/*===========================================================================

FUNCTION
  bt_l2_powerup_init

DESCRIPTION
  Initialize the L2CAP layer upon powerup.

===========================================================================*/
void bt_l2_powerup_init( void )
{
  byte  i;

  bt_l2_app_id = bt_cmd_ec_get_app_id_and_register( bt_l2_store_bt_event );
  ASSERT_DEBUG( bt_l2_app_id != BT_APP_ID_NULL );
  BT_MSG_API( "BT L2 TRX: EC Get AID %x", bt_l2_app_id, 0, 0 );
  BT_MSG_HIGH( "BT L2 AID is %x", bt_l2_app_id, 0, 0 );
  
  (void) q_init( &bt_l2_ev_q );
  bt_l2_ev_q_info.event_q_ptr      = &bt_l2_ev_q;
  bt_l2_ev_q_info.max_q_depth      = BT_L2_NUM_EVENTS;
  bt_l2_ev_q_info.event_q_bit_mask = BT_EVQ_L2_B;
  bt_l2_ev_q_info.max_event_bytes  = bt_l2_get_max_event_bytes();

  for ( i = 0; i < BT_L2_MAX_CONN; i++ )
  {
    timer_def( &bt_l2_conn_db[i].config_req_wait_timer, 
               &bt_always_on_timer_group, &bt_tcb, 
               BT_L2_SIG_MSG_RESP_TIMER_SIG, NULL, 0 );

    timer_def( &bt_l2_conn_db[i].total_config_process_timer, 
               &bt_always_on_timer_group, &bt_tcb, 
               BT_L2_SIG_MSG_RESP_TIMER_SIG, NULL, 0 );

    timer_def( &bt_l2_conn_db[i].acl_link_disconnect_timer, 
               &bt_always_on_timer_group, &bt_tcb, 
               BT_L2_SIG_MSG_RESP_TIMER_SIG, NULL, 0 );

    timer_def( &bt_l2_conn_db[i].acl_link_sec_fail_dis_timer, 
               &bt_always_on_timer_group, &bt_tcb, 
               BT_L2_SIG_MSG_RESP_TIMER_SIG, NULL, 0 );

    bt_l2_conn_db[i].crp_dsm_ptr = NULL;

    bt_l2_init_conn_db_entry(&bt_l2_conn_db[i]);
  }

  /*  Initialize the L2CAP registration table.  */
  bt_l2_cur_regd_count = 0;
  for ( i = 0; i < BT_L2_MAX_PSM_COUNT; i++ )
  {
    bt_l2_init_reg_table_entry( &bt_l2_reg_table[ i ] );
  }

  // Initialize the signalling packet buffer
  for (i = 0; i < BT_L2_MAX_SIGNALLING_PKTS; i++ )
  {
    timer_def( &bt_l2_sig_pkts[i].expected_resp_timer, 
               &bt_always_on_timer_group, &bt_tcb, 
               BT_L2_SIG_MSG_RESP_TIMER_SIG, NULL, 0 );

    bt_l2_free_signalling_pkt( &bt_l2_sig_pkts[i] );
  }

  bt_l2_next_local_cid = BT_L2_CID_FIRST;
  bt_l2_next_matching_id = 0;
}


/*===========================================================================

FUNCTION
  bt_l2_qdsp_ready_init

DESCRIPTION
  Initialize the L2CAP layer when the Bluetooth QDSP image is loaded.

===========================================================================*/
void bt_l2_qdsp_ready_init( void )
{
}

/*===========================================================================

FUNCTION
  bt_l2_soc_reset_shutdown

DESCRIPTION
  De-initialize the L2CAP layer when the Bluetooth SOC is shutdown

===========================================================================*/
void bt_l2_soc_reset_shutdown( void )
{
  
  uint8    i;

  for (i = 0; i < BT_L2_MAX_SIGNALLING_PKTS; i++ )
  {
    (void)rex_clr_timer( &bt_l2_sig_pkts[i].expected_resp_timer );
  }
  for ( i = 0; i < BT_L2_MAX_CONN; i++ )
  {
    (void)rex_clr_timer( &bt_l2_conn_db[i].config_req_wait_timer );
    (void)rex_clr_timer( &bt_l2_conn_db[i].total_config_process_timer );
    (void)rex_clr_timer( &bt_l2_conn_db[i].acl_link_disconnect_timer );
    (void)rex_clr_timer( &bt_l2_conn_db[i].acl_link_sec_fail_dis_timer ); 
  }
  (void)rex_clr_sigs( &bt_tcb, BT_L2_SIG_MSG_RESP_TIMER_SIG );
 
}

/*===========================================================================

FUNCTION
  bt_l2_qdsp_reset_shutdown

DESCRIPTION
  De-initialize the L2CAP layer when the Bluetooth QDSP image is unloaded

===========================================================================*/
void bt_l2_qdsp_reset_shutdown( void )
{
  /*
  uint8    i;

  // TBD - IMAGE_SWAP
  for (i = 0; i < BT_L2_MAX_SIGNALLING_PKTS; i++ )
  {
    (void)rex_clr_timer( &bt_l2_sig_pkts[i].expected_resp_timer );
  }
  for ( i = 0; i < BT_L2_MAX_CONN; i++ )
  {
    (void)rex_clr_timer( &bt_l2_conn_db[i].config_req_wait_timer );
    (void)rex_clr_timer( &bt_l2_conn_db[i].total_config_process_timer );
    (void)rex_clr_timer( &bt_l2_conn_db[i].acl_link_disconnect_timer );
  }
  (void)rex_clr_sigs( &bt_tcb, BT_L2_SIG_MSG_RESP_TIMER_SIG );
  */
}

/*===========================================================================

FUNCTION
  bt_l2_get_free_cid

DESCRIPTION
  Gets the next available CID, and marks it as used

===========================================================================*/
uint16 bt_l2_get_free_cid( void )
{
  uint16 cid;

  TASKLOCK();
  if ( bt_l2_next_local_cid == BT_L2_CID_LAST )
  {
    bt_l2_next_local_cid = BT_L2_CID_FIRST;
  }
  else
  {
    bt_l2_next_local_cid++;
  }
  cid = bt_l2_next_local_cid;
  TASKFREE();
  return cid;
}

/*===========================================================================

FUNCTION
  bt_l2_get_num_conn

DESCRIPTION
  Returns the number of connections to a particular BD Addr. Note that this
  does not count connections comming up or going down, only the active
  connections.

===========================================================================*/
uint16 bt_l2_get_num_conn( bt_bd_addr_type *bd_addr_ptr )
{
  uint8    i;
  uint16   count;

  TASKLOCK();
  /* Search conn table for another l2cap connection. */
  for ( i = 0, count = 0; i < BT_L2_MAX_CONN; i++ )
  {
    if ( ( bt_l2_conn_db[ i ].state >= BT_L2_OPEN ) &&
         ( bt_l2_conn_db[ i ].state <= BT_L2_REOPEN ) &&
         ( BT_BD_ADDRS_EQUAL( bd_addr_ptr,
                              &( bt_l2_conn_db[ i ].bd_addr ) ) ) )
    {
      count++;
    }
  }
  TASKFREE();

  return count;
}

/*===========================================================================

FUNCTION
  bt_l2_process_command

DESCRIPTION
  Processes a specified L2CAP command.

===========================================================================*/
void bt_l2_process_command
(
  bt_cmd_msg_type*  l2_cmd_ptr
)
{

  switch ( l2_cmd_ptr->cmd_hdr.cmd_type )
  {
    case BT_CMD_L2_SET_PARAMS:
      bt_l2_cmd_set_params( l2_cmd_ptr );
      break;

    case BT_CMD_L2_REGISTER:
      bt_l2_cmd_register( l2_cmd_ptr );
      break;

    case BT_CMD_L2_CONNECT:
      bt_l2_cmd_connect( l2_cmd_ptr, FALSE );
      break;

    case BT_CMD_L2_WRITE:
      bt_l2_cmd_write( l2_cmd_ptr );
      break;

    case BT_CMD_L2_DISCONNECT:
      bt_l2_cmd_disconnect( l2_cmd_ptr );
      break;

    case BT_CMD_L2_DISCONN_IMMEDIATE:
      bt_l2_cmd_disconn_immediate( l2_cmd_ptr );
      break;

    case BT_CMD_L2_UNREGISTER:
      bt_l2_cmd_unregister( l2_cmd_ptr );
      break;

    case BT_CMD_L2_PING:
      bt_l2_cmd_ping( l2_cmd_ptr );
      break;

    case BT_CMD_L2_CONNECT_IMMEDIATE_CID:
      bt_l2_cmd_connect( l2_cmd_ptr, TRUE );
      break;

    case BT_CMD_L2_DISABLE_TIMEOUT:
      bt_l2_cmd_disable_timeout( l2_cmd_ptr );
      break;

    default:
      BT_ERR( "BT L2 CMD RX: Unknown: %x AID %x",
              l2_cmd_ptr->cmd_hdr.cmd_type,
              l2_cmd_ptr->cmd_hdr.bt_app_id, 0 );
      l2_cmd_ptr->cmd_hdr.cmd_status = BT_CS_L2_UNRECOGNIZED_CMD;
      break;
  }

}


/*===========================================================================

FUNCTION
  bt_l2_process_ev_queue

DESCRIPTION
  Dequeues events on the L2CAP event queue and processes them.

===========================================================================*/
void bt_l2_process_ev_queue( void )
{

  bt_ev_msg_type*  bt_ev_ptr;
  int              q_count = q_cnt( &bt_l2_ev_q );

  BT_VSTAT_MAX( q_count, L2_MAX_EV_Q_DEPTH );

  BT_MSG_API_PYLD( "L2 ev Q - cur_cnt = %x, max_cnt = %x",
                   q_count,
                   bt_vital_stats[BT_VSAI_L2_MAX_EV_Q_DEPTH], 0);


  while ( (bt_ev_ptr = (bt_ev_msg_type *) q_get( &bt_l2_ev_q ) ) != NULL )
  {
    bt_l2_process_event( bt_ev_ptr );
    q_put( &bt_event_free_q, &bt_ev_ptr->ev_hdr.link );
  }

}

/*===========================================================================

FUNCTION
  bt_l2_expected_resp_timer_expired

DESCRIPTION
  Call-back function for expected response timer. Called from bt.c when the
  BT_L2_EXPECTED_RESP_TIMER_SIG is set.

===========================================================================*/
void bt_l2_expected_resp_timer_expired( void )
{
  uint8               i;
  uint8               j;
  bt_l2_sig_pkt_type  *pkt_ptr;
  bt_l2_sig_pkt_type  *disconn_pkt_ptr;
  bt_ev_msg_type      ev_l2_disconn;

  for ( i=0; i < BT_L2_MAX_CONN; ++i )
  {
    if ( bt_l2_conn_db[i].state == BT_L2_CLOSED )
    {
      continue;
    }
    if ( ( ( rex_get_timer( &bt_l2_conn_db[i].
                            acl_link_disconnect_timer ) == 0 ) ||
           ( ( rex_get_timer( &bt_l2_conn_db[i].
                 acl_link_sec_fail_dis_timer ) == 0 ) &&
           ( bt_l2_conn_db[ i ].disconn_reason == 
                           BT_EVR_L2_AUTHENTICATION_FAILED ) ) ) &&
         ( bt_l2_conn_db[i].state == BT_L2_ACL_LINK_CONNECTED ) &&
         ( ( bt_l2_is_acl_link_in_use( i ) == FALSE ) ||
           ( bt_l2_conn_db[ i ].disconn_reason == 
                                            BT_EVR_L2_AUTHENTICATION_FAILED ) ) )
    {
      bt_rm_disc_reason_type rm_disconn_reason;
      rm_disconn_reason = bt_l2_convert_disconn_reason_to_rm( bt_l2_conn_db[ i ].disconn_reason );
      bt_l2_conn_db[i].state        = BT_L2_CLOSED;
      bt_l2_conn_db[i].idle_timeout = 0;

      BT_MSG_API( "BT L2 CMD TX: RM Disc Conn, h %x r %x",
                  bt_l2_conn_db[ i ].conn_hndl,
                  rm_disconn_reason,
                  0 );
      bt_cmd_rm_disconnect_acl( bt_l2_app_id,
                                bt_l2_conn_db[ i ].conn_hndl,
                                rm_disconn_reason );
      bt_l2_conn_closed_init_reg_table_entry_if_ok(i);
      bt_l2_init_conn_db_entry( &bt_l2_conn_db[i] );
      continue;
    }
    if ( ( rex_get_timer( &bt_l2_conn_db[i].config_req_wait_timer ) == 0 ||
           rex_get_timer( &bt_l2_conn_db[i].total_config_process_timer ) == 0 ) &&
         ( bt_l2_conn_db[i].state == BT_L2_CONFIG ||
           bt_l2_conn_db[i].state == BT_L2_RECONFIG ) )
    {
      if ( rex_get_timer( &bt_l2_conn_db[i].config_req_wait_timer ) != 0 )
      {
        (void)rex_clr_timer( &bt_l2_conn_db[i].config_req_wait_timer );
        BT_MSG_HIGH( "BT L2: Tot Cfg timer exprd, state:%x, cid:%x",
                      bt_l2_conn_db[i].state, bt_l2_conn_db[i].local_cid, 0 );
      }
      if ( rex_get_timer( &bt_l2_conn_db[i].total_config_process_timer ) != 0 )
      {
        (void)rex_clr_timer( &bt_l2_conn_db[i].total_config_process_timer );
        BT_MSG_HIGH( "BT L2: Cfg req wait timer exprd, state:%x, cid:%x",
                      bt_l2_conn_db[i].state, bt_l2_conn_db[i].local_cid, 0 );
      }
      for ( j=0; j < BT_L2_MAX_PENDING_RQSTS; ++j )
      {
        if ( bt_l2_conn_db[i].outstanding_rqst_ptr[j] != NULL)
        {
          pkt_ptr = bt_l2_conn_db[i].outstanding_rqst_ptr[j];
          if ( pkt_ptr->cmd_hdr.command_code ==
                                (byte)BT_L2_SIG_DISCONNECTION_RQST )
          {
            break;
          }
          bt_l2_conn_db[i].outstanding_rqst_ptr[j] = NULL;
          if ( ( rex_get_timer( &pkt_ptr->expected_resp_timer ) != 0 ) &&
               ( pkt_ptr->pkt_hdr.length != 0 ) )
          {
            (void)rex_clr_timer( &pkt_ptr->expected_resp_timer );
          }
          bt_l2_free_signalling_pkt( pkt_ptr );
        }
      }
      if ( j == BT_L2_MAX_PENDING_RQSTS )
      {
        if ( bt_l2_conn_db[i].state < BT_L2_OPEN )
        {
          disconn_pkt_ptr = bt_l2_get_free_signalling_pkt();
          if ( disconn_pkt_ptr != NULL )
          {
            disconn_pkt_ptr->cmd.disconn_req.dest_cid   = bt_l2_conn_db[i].remote_cid;
            disconn_pkt_ptr->cmd.disconn_req.source_cid = bt_l2_conn_db[i].local_cid;

            bt_l2_send_signalling_pkt( BT_L2_SIG_DISCONNECTION_RQST,
                                       disconn_pkt_ptr,
                                       &bt_l2_conn_db[i], FALSE );
          }
          else
          {
            // No free signalling packets. Disconnect link
            bt_l2_close_l2_connection( &bt_l2_conn_db[i], BT_EVR_L2_UNSPECIFIED_ERROR );
          }
        }
        else if ( bt_l2_conn_db[i].state < BT_L2_W4_L2CAP_DISCONNECT_RSP )
        {
          bt_cmd_l2_disconnect( bt_l2_app_id, bt_l2_conn_db[i].local_cid );
        }
        continue;
      }
    }
    for ( j=0; j < BT_L2_MAX_PENDING_RQSTS; ++j )
    {
      if ( bt_l2_conn_db[i].outstanding_rqst_ptr[j] != NULL)
      {
        pkt_ptr = bt_l2_conn_db[i].outstanding_rqst_ptr[j];

        if ( rex_get_timer( &pkt_ptr->expected_resp_timer ) == 0 &&
             ( pkt_ptr->pkt_hdr.length != 0 ) )
        {
          bt_l2_conn_db[i].outstanding_rqst_ptr[j] = NULL;
          if ( pkt_ptr->rtx_timer_in_use == TRUE )
          {
            pkt_ptr->total_rtx_wait_time +=
                              pkt_ptr->rtx_timer_value;
            pkt_ptr->rtx_timer_value *= 2;
            if ( pkt_ptr->total_rtx_wait_time <
                                         BT_L2_MAX_TOTAL_RTX_WAIT_TIME )
            {
              if ( ( bt_l2_conn_db[i].state < BT_L2_W4_L2CAP_DISCONNECT_RSP ) ||
                   ( (bt_l2_sig_cmd_type)pkt_ptr->cmd_hdr.command_code
                                       == BT_L2_SIG_DISCONNECTION_RQST ) )
              {
                BT_MSG_HIGH( "BT L2: RTX expired, Resend L2 cmd:%x, cid:%x",
                             (bt_l2_sig_cmd_type) pkt_ptr->cmd_hdr.command_code,
                             bt_l2_conn_db[i].local_cid,
                             0 );
                bt_l2_send_signalling_pkt(
                   ( bt_l2_sig_cmd_type ) pkt_ptr->cmd_hdr.command_code,
                   pkt_ptr,
                   &bt_l2_conn_db[i], TRUE );
              }
              else
              {
                bt_l2_free_signalling_pkt( pkt_ptr );
              }
            }
            else
            {
              if ( (bt_l2_sig_cmd_type)pkt_ptr->cmd_hdr.command_code
                                       != BT_L2_SIG_DISCONNECTION_RQST )
              {
                BT_MSG_HIGH( "BT L2: RTX expired, L2 cmd:%x; cid:%x disconn",
                             (bt_l2_sig_cmd_type) pkt_ptr->cmd_hdr.command_code,
                             bt_l2_conn_db[i].local_cid,
                             0 );
                if ( rex_get_timer( &bt_l2_conn_db[i].config_req_wait_timer ) != 0 )
                {
                  (void)rex_clr_timer( &bt_l2_conn_db[i].config_req_wait_timer );
                }
                if ( rex_get_timer( &bt_l2_conn_db[i].total_config_process_timer ) != 0 )
                {
                  (void)rex_clr_timer( &bt_l2_conn_db[i].total_config_process_timer );
                }
                if ( bt_l2_conn_db[i].state < BT_L2_OPEN )
                {
                  disconn_pkt_ptr = bt_l2_get_free_signalling_pkt();
                  if ( disconn_pkt_ptr != NULL )
                  {
                    disconn_pkt_ptr->cmd.disconn_req.dest_cid   = bt_l2_conn_db[i].remote_cid;
                    disconn_pkt_ptr->cmd.disconn_req.source_cid = bt_l2_conn_db[i].local_cid;

                    bt_l2_send_signalling_pkt( BT_L2_SIG_DISCONNECTION_RQST,
                                               disconn_pkt_ptr,
                                               &bt_l2_conn_db[i], FALSE );
                  }
                  else
                  {
                    // No free signalling packets. Disconnect link
                    bt_l2_close_l2_connection( &bt_l2_conn_db[i], BT_EVR_L2_UNSPECIFIED_ERROR );
                  }
                }
                else if ( bt_l2_conn_db[i].state < BT_L2_W4_L2CAP_DISCONNECT_RSP )
                {
                  bt_cmd_l2_disconnect( bt_l2_app_id, bt_l2_conn_db[i].local_cid );
                }
              }
              else
              {
                /* Send L2 DISCONNECTED event to the upper layer */
                /*  Let upper layer know the remote has disconnected.  */
                ev_l2_disconn.ev_hdr.bt_app_id = bt_l2_conn_db[i].app_id;
                if ( bt_l2_conn_db[i].state > BT_L2_CONFIG )
                {
                  ev_l2_disconn.ev_hdr.ev_type   = BT_EV_L2_DISCONNECTED;
                  ev_l2_disconn.ev_msg.ev_l2_disc.cid = bt_l2_conn_db[i].local_cid;
                  ev_l2_disconn.ev_msg.ev_l2_disc.reason = BT_EVR_L2_LINK_LOST;
                }
                else
                {
                  ev_l2_disconn.ev_hdr.ev_type   = BT_EV_L2_CONNECTION_FAILED;
                  ev_l2_disconn.ev_msg.ev_l2_conn_failed.bd_addr = bt_l2_conn_db[i].bd_addr;
                  ev_l2_disconn.ev_msg.ev_l2_conn_failed.cid     = bt_l2_conn_db[i].local_cid;
                  ev_l2_disconn.ev_msg.ev_l2_conn_failed.reason = BT_EVR_L2_LINK_LOST;
                }
                BT_MSG_API( "BT L2 EV TX: RTX expired, AID %x lcid %x rcid %x closed",
                            bt_l2_conn_db[i].app_id,
                            bt_l2_conn_db[i].local_cid,
                            bt_l2_conn_db[i].remote_cid );
                if ( ev_l2_disconn.ev_hdr.bt_app_id != BT_APP_ID_NULL )
                {
                  bt_ec_send_event( &ev_l2_disconn );
                }
                //bt_l2_init_conn_db_entry( &bt_l2_conn_db[i] );
                bt_l2_close_l2_connection( &bt_l2_conn_db[i], BT_EVR_L2_LINK_LOST );
              }
              bt_l2_free_signalling_pkt( pkt_ptr );
            }
          }
          else
          {
            if ( (bt_l2_sig_cmd_type)pkt_ptr->cmd_hdr.command_code
                                     == BT_L2_SIG_CONNECTION_RQST )
            {
              BT_MSG_HIGH( "BT L2: ERTX expired; disconn cid:%x",
                           bt_l2_conn_db[i].local_cid, 0, 0 );
              disconn_pkt_ptr = bt_l2_get_free_signalling_pkt();
              if ( disconn_pkt_ptr != NULL )
              {
                disconn_pkt_ptr->cmd.disconn_req.dest_cid   = bt_l2_conn_db[i].remote_cid;
                disconn_pkt_ptr->cmd.disconn_req.source_cid = bt_l2_conn_db[i].local_cid;

                bt_l2_send_signalling_pkt( BT_L2_SIG_DISCONNECTION_RQST,
                                           disconn_pkt_ptr,
                                           &bt_l2_conn_db[i], FALSE );
              }
              else
              {
                // No free signalling packets. Disconnect link
                bt_l2_close_l2_connection( &bt_l2_conn_db[i], BT_EVR_L2_UNSPECIFIED_ERROR );
              }
            }
            else
            {
              BT_ERR( "BT L2: ERTX expired, L2 cmd %x, close cid%x",
                       pkt_ptr->cmd_hdr.command_code,
                       bt_l2_conn_db[i].local_cid, 0 );
              bt_l2_close_l2_connection( &bt_l2_conn_db[i],
                                         BT_EVR_L2_LINK_LOST );
            }
            bt_l2_free_signalling_pkt( pkt_ptr );
          } // else

        } // if timer expired
      }  // if outstanding request
    } // check all requests for a conn entry
  } // check all conn entries
}


/*===========================================================================

FUNCTION
  bt_l2_background_tick

DESCRIPTION
  Performs background monitoring for L2CAP.

===========================================================================*/
void bt_l2_background_tick
(
  void
)
{
  int ci;

  /* Close L2 connections for which no data has passed after the timeout. */
  for ( ci = 0; ci < BT_L2_MAX_CONN; ci++ )
  {
    if ( bt_l2_conn_db[ ci ].idle_timeout != 0 )
    {
      bt_l2_conn_db[ ci ].idle_timeout -= BT_BACKGROUND_TIMER_MS;
      if( bt_l2_conn_db[ ci ].idle_timeout <= 0 )
      {
        bt_l2_conn_db[ ci ].idle_timeout = 0;
        BT_MSG_DEBUG("BT L2: Idle timer expired CID %x, AID %x",
                     bt_l2_conn_db[ ci ].local_cid,
                     bt_l2_conn_db[ ci ].app_id, 0);
        BT_BDA( MSG_HIGH, "BT L2: Idle timer expired", &bt_l2_conn_db[ ci ].bd_addr );
        // Timer expired; disconnect
        bt_l2_conn_db[ ci ].idle_timeout = 0;
        bt_l2_conn_db[ ci ].disconn_acl_immediate = TRUE;
                
        if( bt_l2_conn_db[ ci ].local_cid != BT_L2_NULL_CID )
        {
          // This is an established connection
          bt_cmd_msg_type bt_cmd;

          bt_cmd.cmd_msg.cmd_l2_disc.cid = bt_l2_conn_db[ ci ].local_cid;
          bt_l2_cmd_disconnect( &bt_cmd );
        }
        else
        {
          // No cid. But we must get rid of it anyway.
          bt_l2_close_l2_connection( &bt_l2_conn_db[ ci ], BT_EVR_L2_LOCAL_REJECT_CONNECTION );
        }
      }
    }
  }
}

/*===========================================================================

FUNCTION
  bt_l2_disconnect_acl_if_unused

DESCRIPTION
  Disconnect unused L2CAP connection

===========================================================================*/
void bt_l2_disconnect_acl_if_unused(void) 
{
 int i;        
 for ( i=0; i < BT_L2_MAX_CONN; ++i)
 {
   if ( ( bt_l2_conn_db[i].state == BT_L2_CLOSED) || (bt_l2_conn_db[i].state == 
        BT_L2_ACL_LINK_CONNECTED ) )
   {
     continue;
   }

   if ( bt_l2_conn_db[i].state == BT_L2_W4_L2CAP_CONNECT_RQST && 
       bt_l2_conn_db[i].initiated_conn == FALSE )
   {
     BT_MSG_DEBUG( "BT L2: Close l2 and ACL for unused connection", 0, 0, 0 );
     /* This should be disconnected immediately */
     bt_l2_conn_db[i].disconn_acl_immediate = TRUE;
     bt_l2_close_l2_connection(&bt_l2_conn_db[i], BT_EVR_L2_UNSPECIFIED_ERROR);
   }
 }
}


/*===========================================================================

FUNCTION
  bt_cmd_l2_get_acl_tx_flow_status

DESCRIPTION
  Returns the flow on/off status for the connection handle associated with
  the cid.

RETURN VALUE
  ACL TX Flow on/off status

===========================================================================*/
boolean bt_cmd_l2_get_acl_tx_flow_status
(
  uint16    cid            /* Channel Id of the L2CAP connection */
)
{
  bt_l2_conn_db_type*     l2_conn_db_ptr;
  boolean                 is_flow_on = FALSE;

  /* Obtain the L2 connection database pointer for this Cid */
  l2_conn_db_ptr = bt_l2_get_conn_entry_from_cid( cid );

  if ( l2_conn_db_ptr != NULL )
  {
    is_flow_on = l2_conn_db_ptr->acl_tx_flow_on;
  }
  else
  {
    BT_ERR( "BT L2: Get ACL Tx Flow - invalid Cid", cid, 0, 0);
  }

  return( is_flow_on );
} /* bt_cmd_l2_get_acl_tx_flow_status */


/*===========================================================================

FUNCTION
  bt_l2_update_acl_tx_flow_on

DESCRIPTION
  Sets the acl_tx_flow_on flag for a specified BD ADDR to a specified value.

===========================================================================*/
void bt_l2_update_acl_tx_flow_on
(
  const bt_bd_addr_type *bd_addr_ptr,  /* Pointer to BD ADDR of the ACL connection */
  boolean         acl_tx_flow_on /* ACL Tx Flow indication from lower layers */
)
{
  uint8                   ci;

  /* Obtain the L2 connection database pointer for this BD ADDR */
  for ( ci = 0; ci < BT_L2_MAX_CONN; ci++ )
  {
    if ( BT_BD_ADDRS_EQUAL( &bt_l2_conn_db[ ci ].bd_addr, bd_addr_ptr ) &&
       ( bt_l2_conn_db[ ci ].state != BT_L2_CLOSED ) )
    {
      bt_l2_conn_db[ ci ].acl_tx_flow_on = acl_tx_flow_on;
    }
  }

#ifdef BT_FLOW_DEBUG
    BT_MSG_HIGH( "BT L2: Update ACL TX flow Flow %x", acl_tx_flow_on, 0, 0);
    BT_BDA( MSG_HIGH, "BT L2: Update Flow ", bd_addr_ptr );
#endif

} /* bt_l2_update_acl_tx_flow_on */


#ifdef FEATURE_BT_SOC
/*===========================================================================

FUNCTION
  bt_l2_set_max_hci_acl_data_len

DESCRIPTION
  Sets the the Max HCI ACL data size value in bytes.

===========================================================================*/
void bt_l2_set_max_hci_acl_data_len
( 
  uint16 max_acl_len 
)
{
  /* Set the max hci acl data packet length to the variable used by L2 for 
     HCI fragmentation */
  bt_l2_max_hci_acl_len = max_acl_len;

} /* bt_l2_set_max_hci_acl_data_len */
#endif /* FEATURE_BT_SOC */


#ifdef BT_SWDEV_QXDM_LOG_PER
/*===========================================================================

FUNCTION
  bt_l2_send_log

DESCRIPTION
  Logs all the L2CAP specific statistics through QXDM.

===========================================================================*/
void bt_l2_send_log( void )
{

  log_bt_l2_type*      log_ptr;
  bt_l2_conn_db_type*  conn_ptr;
  uint16               ci;

  for ( ci = 0; ci < BT_L2_MAX_CONN; ci++ )
  {
    if ( bt_l2_conn_db[ ci ].state == BT_L2_CLOSED )
    {
      continue;
    }
    else
    {
      conn_ptr = &bt_l2_conn_db[ ci ];
    }

    log_ptr = ( log_bt_l2_type *)log_alloc( log_bt_l2, sizeof( log_bt_l2_type) );
    if ( log_ptr != NULL )
    {
      log_ptr->l2cap_idx = ci;
      log_ptr->l2cap_state = conn_ptr->state;
      log_ptr->l2cap_config_req_state = conn_ptr->config_req_state;
      log_ptr->l2cap_config_rsp_state = conn_ptr->config_rsp_state;
      log_ptr->l2cap_conn_status =  conn_ptr->conn_status;
      *((bt_bd_addr_type *)log_ptr->l2cap_bd_addr) =
                             conn_ptr->bd_addr;
      log_ptr->l2cap_conn_hdl = conn_ptr->conn_hndl;
      log_ptr->l2cap_psm = conn_ptr->app_id;
      log_ptr->l2cap_local_cid = conn_ptr->local_cid;
      log_ptr->l2cap_remote_cid = conn_ptr->remote_cid;
      log_ptr->l2cap_result = conn_ptr->result;
      log_ptr->l2cap_status = conn_ptr->status;

      log_commit( log_ptr );
    }
  }

}
#endif /* BT_SWDEV_QXDM_LOG_PER */

#endif /* FEATURE_BT */
