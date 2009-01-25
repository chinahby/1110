/*====*====*====*====*====*====*====*====*====*====*====*====*====*====*====*

        B L U E T O O T H    H O S T   C O N T R O L L E R
                  I N T E R F A C E   D R I V E R

GENERAL DESCRIPTION
  This module contains routines for the HCI (Host-Controller Interface) driver
  for the Bluetooth host.

EXTERNALIZED FUNCTIONS

  bt_hc_drv_soc_init()
    Initializes the SOC Driver

  bt_hc_drv_set_hci_buf_sizes()
    Updates the parameters returned from Read Buffer Size command to the HCI
    driver structures.

  bt_hc_drv_process_hci_rx()
    Processes bytes received from Host controller SOC to Host via SIO.

  bt_hc_drv_process_cmd()
    Processes HCI commands and ACL data to be sent to Host Controller.

INITIALIZATION AND SEQUENCING REQUIREMENTS

  bt_hc_drv_soc_init() needs to be called prior to any other function.

        Copyright (c) 2006-2011 QUALCOMM Incorporated.
               All Rights Reserved.
        Qualcomm Confidential and Proprietary

*====*====*====*====*====*====*====*====*====*====*====*====*====*====*====*

============================================================================

EDIT HISTORY FOR MODULE

This section contains comments describing changes made to the module.
Notice that changes are listed in reverse chronological order.

 $Header: //source/qcom/qct/wconnect/bthost/core/rel/00.00.26/hci/src/bthcdrv.c#4 $
 $DateTime: 2011/02/07 10:01:56 $$Author: roystonr $
 when        who  what, where, why
 ----------  ---  -----------------------------------------------------------
 2009-04-14   pn  Watermark queues get initialized by dsm_queue_init().
 2008-12-30   rb  Fix packet processing after re-initialization of the driver
 2008-12-23   pg  Added missing re-init of bt_hc_drv.host_info.hci_rx struct
                  members during disconnection complete.
 2008-12-12   vk  Sending enhance flush command only if controller have
                  outstanding flushable packets.
 2008-11-05   pg  Generic handling for HCI Vendor Specific cmd group.
 2008-10-31   SSK Fixing Klockwork Error:- Possible uninitialized Table index
 2008-09-24   SSK Fixing improper "bit-wise ANDing" introduced in previous Fix.
 2008-09-23   SSK Fixing Compiler Warning:-'unsigned short' forbidden as bit
                  field type'
 2008-09-23   SSK Fixing KlockWork error:- 'Buffer Overflow - Array Index
              Out of Bounds' 
 2008-07-28   SSK Fixing KlockWork error:- 'Uninitialzed value'
 2008-07-28   SSK Merging from the 2007.11 Depot branch, the fix for 'Index out
                  of bounds/Uninitialzed value' Klockwork error.
 2008-05-12   VK  Addressing High\Low Water mark calculations in reduced memory 
                  usage case and adjusted DNE to accommodate max protocol chunk.
 2008-05-06   RS  Added BT_HC_DRV_DSM_ALIGN_BUFFER to memory align DSM buffers
                  sent to SIO
 2008-03-08   NM  Fixes BT 2.1 Host sending HCI ACL pkt with wrong PB flag 
                  to 2.0 controller
 2008-03-27   NM  Fix for wrongly formatted HCI Num Completed Packet event sent
                  to tester
 2008-03-24   vk  Added support for HCI Read/write Default Erroneous Data
                  Reporting mode commands.
 2008-03-21   rs  Increased the BT_HC_DRV_MAX_ACL_FRAMES to cater to 3
                  OBEX connection pumping data to the remote device at the
                  same time.


* #68        29 Feb 2008            AG
* Renamed BT_CMD_HC_IO_CAPABILITY_RESPONSE to BT_CMD_HC_IO_CAPABILITY_RQST_REPLY
* and BT_HCI_IO_CAPABILITY_RESPONSE to BT_HCI_IO_CAPABILITY_RQST_REPLY
* for consistency with spec.
*
* #67       29 Feb 2008            SSK
* Featurize Inband Sleep call in in HCI driver.
*
* #66       28 Feb 2008            SSK
* -Optimise Inband SM to enter TX BREAK after waiting for TX path to go idle.
* -Remove unneeded checks for SM being in the middle of a Inband TX Break from
*  HCI driver.The TX BRK condition is non-premptible in BT task context.
*
* #65       19 Feb 2008            SSK
* Fix for critical lint error.
*
* #64       11 Feb 2008            VK
* Featurizing Debug message for RX ACL data packet count.
*
* #63       08 Feb 2008            SSK
* Fix FCTL bug where a Back-toback DSM Count LOW Callback  followed immediately
* by a DSM count HIGH CB causes HCI driver to get into a incorrect state.
*
* #62       29 Jan 2008            VK
* Queueing HCI commands if large event is pending  with controller.
*
* #61       22 Jan 2008            DM
* When in HCI mode donot automatically update the AFH Channel Map.
*
* #60       16 Jan 2008            DSN
* Fixing defintions for vendor-specific command groups in comamnd table.
*
* #59       15 Jan 2008            DSN
* Add definitions for more vendor-specific command groups.
*
* #58       10 Jan 2008            SSK
* Implement a cleaner interface to Inband Sleep State Machine throught new APIs.
*
* #57       07 Jan 2008            PG
* Fixed incorrect update to bt_soc1_cmdmgr_data.transmit_state that could
* result in H4 data getting treated as H4DS data in certain corner cases.
*
* #56       07 Dec 2007             AG
* Reverted last change for wr eir hci cmd. The last change fixed a BITE
* test case, but broke the normal mode.
*
* #55       05 Dec 2007             AG
* Fix length and type(fixed) for wr eir hci cmd.
**
* #53       29 Nov 2007             SSK
* Implemented Inband Sleep mechanism under FEATURE_BT_QSOC_INBAND_SLEEP
*
* #52        12 Nov 2007            DM
* Dup the original dsm pointer for HCI commands that did not have any fixed
* parameters, so that the dsm has valid reference if the command gets
* retried. (Addition to Fix in #46)
*
* #51        01 Nov 2007            PG
* Fixed acl data not getting sent due to "race condition" in entry to park
* state.
*
*  #50       31 Oct 2007            VK
* Added support for Inquiry result event with RSSI.
*
*  #49       19 Oct 2007            DSN\SSK
* Add support for Controller to Host flow control.
*
*  #48       16 Oct 2007            DSN
* Added support for Rd Loc Supported commands and Rd local extended
* features command.
*
*  #47       08 Oct 2007            DM
* Changed the time delta variable to signed int so as to ignore any negative
* time difference when flushing packets for AV-Sync.
*
*  #46       01 Oct 2007            DM
* Dup the original dsm pointer in a variable length hci command and
* donot free the original dsm when the command is returned back for re-trying
*
*  #45       11 Sept 2007           BK
* Implement QSOC Driver State state machine.  Send AFH Channel Map command if
* transitioning back to ACTIVE from DISABLING since full startup sequence is not executed.
*
*  #44       04 Sept 2007           BH
* Added support for the QSoC vender specific encryption key length command.
*
*  #43       20 Aug 2007            PG
* Seperated HCI generic vendor specific event handling from Qualcomm
* proprietary implementation used with embedded controller.
*
*  #42       06 AUG 2007            PG
* Validated stream id and logged BT error before calling sio_transmit to
* prevent ASSERT in SIO code.
*
*  #41       09 JUL 2007            PG
* HCI Remote name req cancel and connection cancel support.
*
*  #40       03 Jul 2007            SSK
* Fix HCI_RESET bug in HCI APIT Mode on BTS targets :- Change to exclude
* Power-down or Power-on of the SOC during Radio-On/Off done due to HCI RST.
*
*  #39       27 Jun 2007            DM
* Changed featurization for AV-Sync in SOC based targets
*
*  #38       26 Jun 2007            DM
* Added support for broadcast data transmission while in HCI mode.
* also to allow for all Num_Complete Events to be sent to the Host while
* in HCI mode.
*
*  #37       14 Jun 2007            PG
* Added check before enqueing to acl_data_wm and log a BT ERR if wm would
* overflow.
*
*  #36       07 May 2007            PG
* Support for sending SOC1 vendor specific reset while driver is enabled
* in H4DS mode.
*
*  #35       26 Apr 2007            BH
* Fixed a bug where the MSM would send bytes out to the SoC while
* the BTS was still asleep.
*
*  #34       19 Apr 2007            PG
* Featurized out code related to Qualcomm Vendor specific commands for
* SOC targets.
*
*  #33       13 Apr 2007            JN/DSN
* Moved bt_hc_drv_powerup_init to bt_hc_drv_soc_init(). Fixed formating.
* Added some cleanup for SoC shutdown.
*
*  #32       04 Apr 2007            BH
* Following modifications:
* - Added event processing for QSoC sleep state machine.
* - sio_transmit is now INT_LOCK'ed to ensure the QSoC sleep
    state machine can track when transmits are truely complete.
*
*  #31       22 Mar 2007            DSN
* Sends bytes received from sio to HCI parser only if H4DS state is ready for
* receiving H4 messages.
*
*  #30       15 Mar 2007            DSN
* Fix for an infinite looping issue when in multiple connections.
*
*  #29       14 Mar 2007            JN
* Following modifications:
* - Added BTS as one of the controller types.
* - Added NULL check before using a function ptr.
*
*  #28       07 Mar 2007            JN
* Added support for SoC abstraction interface and BTS. Featurized flush
* timer under SOC1.
*
*  #27       07 Mar 2007            DSN
* Fix for critical lint errors.
*
*  #26       01 Mar 2007            DSN
* Use HCI Max ACL data length from HCI Read Buffer Size command response.
*
*  #25       22 Feb 2007            DSN
*  Fix for compiler warnings.
*
*  #24       20 Feb 2007            DSN
* Control the number of HCI frames received per SIO WM processing.
* Prevent Num Complete Packets event from going to RM for efficiency.
*
*  #23       20 Feb 2007            ACG
* Once yms is received, either a flush command can be issued to sio or a
* timer is used to flush any data in the SIO tx buffer. The switch can
* be made by changing just one variable (bt_enable_flush_tx) in btsoc1.c.
*
*  #22       07 Feb 2007            ACG
* Enable Tx Flush IOCTL command and remove flush timer for SOC based solutions.
*
*  #21       02 Feb 2006            SSK
* Fix a bug in bt_hc_drv_cmd() to release BCCMD channel lock conditionally.
*
*  #20       31 Jan 2007            ACG
* Added a delay between receipt of yms and voting to sleep.
*
*  #18       19 Jan 2007            DSN
* Misaligned access, typecasting fixes for ads1.2.
*
*  #17       08 Jan 2007            ACG
* Following features were added for the SOC solution:
* - vote for SIO sleep/wakeup based on BT activity
* - enable/disable driver
* - radio on/off
*
*   #16       05 Jan 2007            SSK
* H4DS optimisation: Remove usage of signals for interaction with
* the H4DS library. This reduces the task context switch overhead.
*
* Fix bug in handing off bytes to H4DS engine that resulted in high
* water mark being asserted.
*
* Fix bug in which acl_data transmit requests were not aware of the present
* deep sleep state of the H4DS Engine.
*
*   #15       22 Dec 2006            DSN
* Change ASSERT() to ASSERT_DEBUG().
*
*   #14       05 Dec 2006            SSK
* Add support for feature BT_SWDEV_H4DS_ON to turn on\off H4DS in external SOC.
*
*   #13       22 Nov 2006            DSN
* Fix for malformed dsm items transmitted to sio.
*
*   #12       22 Nov 2006            DM
* Added Flush mechanism for AV-Sync support on SoC.
*
*   #11       16 Nov 2006            DM
* Added Tx Complete Mechanism for AV-Sync Support on SoC.
*
*   #10       08 Nov 2006            SSK
* Incorporated changes in  SOC Driver APIs:- Change SOC driver API naming.
* Fix a bug in bt_hc_drv_cmd() for releasing BCCMD channel lock.
*
*   #9        06 Nov 2006            DSN
* Indicate Acl flow to L2CAP based on BD address, Fix HCI table entry to
* change local name.
*
*   #8        02 Nov 2006            SSK
* Re-checking Change : Rectified logging in bt_hc_drv_sio_transmit() and added
* SOC driver enabled check before sending HCI messages.
* Please disregard Version 7 of this file.
*
*   #7        31 Oct 2006            SSK
* Rectified logging in bt_hc_drv_sio_transmit() and added SOC
* driver enabled check before sending HCI messages.
*
*   #6        26 Oct 2006            DSN
* Added support for 3 ACL connections.
*
*   #5        20 Oct  2006           SSK
* External SOC suppport and Merge Command Flow control code of DM
*
*   #4        09 Oct  2006           GS
* Fixing compilation issues in enabling APIT and BT 1.2 feature
* on 7K target.
*
*   #3        20 Sep 2006            DSN
* Support for separate command, event and ACL log packets.
*
*   #2        22 Aug 2006            DSN
* HCI logging support. Special handling of event filt command for RF test mode
* support.
*
*   #1        01 May 2006            DSN
* Initial revision.  Added  Controller flow control for a single connection.

==========================================================================*/


/*==========================================================================

                          INCLUDE FILES FOR MODULE

==========================================================================*/

#include "customer.h"
#include "comdef.h"

#ifdef FEATURE_BT
#ifdef FEATURE_BT_SOC

/* Listed in alphebetical order of file names */
#include "bt.h"
#include "btcmdi.h"
#include "btdebug.h"
#include "bthc.h"
#include "bthci.h"
#include "bthcdrv.h"
#include "bthcpe.h"
#include "bti.h"
#include "btl2.h"
#include "btlogi.h"
#include "btmsg.h"
#include "btrc.h"
#include "btrm.h"
#include "btsocetc.h"
#include "btsoc.h"
#ifdef FEATURE_BT_SOC1
#error code not present
#endif /* FEATURE_BT_SOC1 */

#ifdef FEATURE_BT_HCI_HOST_FCTL
#include "btqsocnvmplatform.h"
#endif /* FEATURE_BT_HCI_HOST_FCTL */

#ifdef FEATURE_BT_QSOC_SLEEP
#include "btqsoc.h"
#include "btqsocsleep.h"
#endif /* FEATURE_BT_QSOC_SLEEP */

#ifdef FEATURE_BT_AVSYNC_SOC
#include "clk.h"
#endif /* FEATURE_BT_AVSYNC_SOC */

/* Optional Host HCI Tx and RX Logging */
/* #define BT_HC_DRV_TX_TRACE_ENABLE */
/* #define BT_HC_DRV_TRACE_ENABLE */
#define BT_SWDEV_CMD_FLOW_WORKAROUND

#ifdef BT_HC_DRV_TRACE_ENABLE

#include "timetick.h"

#define BT_HC_DRV_TX_TRACE_BUF_SIZE 30000
#define BT_HC_DRV_RX_TRACE_BUF_SIZE 30000

typedef struct
{
  byte   val;
  uint32 ts;
} bt_hc_drv_tx_buf_type;

bt_hc_drv_tx_buf_type  bt_hc_drv_tx_buf[BT_HC_DRV_TX_TRACE_BUF_SIZE];
uint32               bt_hc_drv_tx_trace_cntr = 0;

typedef struct
{
  byte   val;
  uint32 ts;
} bt_hc_drv_rx_buf_type;

bt_hc_drv_rx_buf_type  bt_hc_drv_rx_buf[BT_HC_DRV_TX_TRACE_BUF_SIZE];
uint32               bt_hc_drv_rx_trace_cntr = 0;

void bt_hc_drv_tx_trace
(
  dsm_item_type* dsm_ptr
);

void bt_hc_drv_rx_trace
(
  dsm_item_type* dsm_ptr
);
#define BT_HC_DRV_TX_TRACE(current_tx_ptr) bt_hc_drv_tx_trace(current_tx_ptr)
#define BT_HC_DRV_RX_TRACE(current_tx_ptr) bt_hc_drv_rx_trace(current_tx_ptr)

#else

#define BT_HC_DRV_TX_TRACE(current_tx_ptr)
#define BT_HC_DRV_RX_TRACE(current_tx_ptr)

#endif /* BT_HC_DRV_TRACE_ENABLE */

/*=====================================================================
               DEFINITIONS AND DECLARATIONS FOR MODULE
======================================================================*/

/*=====================================================================
                               Typedefs
======================================================================*/

/* Enum type to specify whether a command is of fixed length (non-arrayed
   parameters or variable length (arrayed parameters) */
typedef enum
{
  BT_CMD_FIXED_LEN = 0,
  BT_CMD_VAR_LENGTH
} bt_cmd_len_enum_type;


/* Enum type to specify whether an event is of fixed length (non-arrayed
   parameters) or variable length (arrayed parameters) */
typedef enum
{
  BT_EV_FIXED_LENGTH = 0,
  BT_EV_VAR_LENGTH
} bt_ev_len_enum_type;

/* State of the engine that sends/receives HCI packets from Controller */
typedef enum
{
  BT_HC_DRV_INIT_STATE,      /* HCI init state */
  BT_HC_DRV_NEW_PKT_STATE,   /* Processing new packet state */
  BT_HC_DRV_PKT_PAYLD_STATE, /* Processing packet payload state */
  BT_HC_DRV_PKT_PAYLD_CONT_STATE, /* Processing payload continuation state*/
  BT_HC_DRV_IDLE_STATE      /* Idle state */
} bt_hc_drv_state_enum_type;

/* State of the engine that handles HCI events from Controller */
typedef enum
{
  BT_HCI_EV_EVENT_CODE_STATE,
  BT_HCI_EV_PARAM_TOT_LEN_STATE,
  BT_HCI_EV_PARAMS_STATE
} bt_hc_drv_event_state_enum_type;

/* State of the engine that handles HCI ACL data received from external Host
   controller */
typedef enum
{
  BT_HCI_ACL_RX_HDR_STATE,
  BT_HCI_ACL_RX_DATA_STATE
} bt_hc_drv_acl_rx_state_enum_type;

/* Enum type for watermark callback types */
typedef enum
{
  BT_HC_DRV_ACL_HI_WM,
  BT_HC_DRV_ACL_LO_WM
} bt_hc_drv_acl_wm_cb_enum_type;

/* Stucture to record Tx/Rx bytes at the Host HCI layer. */
typedef struct
{
  uint32 hci_tx_cmd_cnt;  /* Transmitted bytes count */
  uint32 hci_tx_acl_cnt;  /* Transmitted bytes count */
  uint32 hci_rx_ev_cnt;   /* Received bytes count */
  uint32 hci_rx_acl_cnt;  /* Received bytes count */
} bt_hc_drv_txrx_cnt_type;

/* Structure to record the maximum size of the data portion of HCI ACL and
   Synchronous data packets sent from the Host to the Controller. */
typedef struct
{
  uint16 max_acl_data_pkt_len;   /* Max ACL data packet length in bytes*/
  uint8  max_sync_data_pkt_len;  /* Max Sync data packet length in bytes*/
  uint16 tot_num_acl_data_pkts;  /* Total number of ACL data packets */
  uint16 tot_num_sync_data_pkts; /* Total number of Synchronous data packets */
} bt_hc_drv_ctrlr_max_buf_type;

typedef struct
{
  uint16  conn_hndl;          /* Connection handle */
  uint16  num_buf_used;       /* Number of buffers used for connection */
  uint16  num_buf_req;        /* Number of buffers requested */
  uint16  num_alloc_buf;      /* Number of buffers allocated */
  boolean is_buf_guaranteed;  /* Flag to guarantee HCI buffers */
} bt_hc_drv_buf_type;

#ifdef FEATURE_BT_AVSYNC_SOC
typedef struct
{
  q_link_type      link;
  boolean          tx_status;     /* Indicates packet was sent to the SOC */
  boolean          start_of_l2cap;/* Indicates it is a start of an l2cap */
  boolean          need_to_flush; /* Indicates if the ACL data packet has
                                     exceeded its time and is marked to be
                                     flushed */
  boolean          flushable_pkt; /* Indicates if this is a flushable pkt */
  uint16           local_cid;     /* Local L2CAP Cid of the l2cap connection */
  void*            up_handle_ptr; /* Reference pointer from the host stack */
  time_type        rx_time_stamp; /* Timestamp when the packet was received by
                                     the HC Driver */
} bt_hc_drv_acl_frame_handle_type;
#endif /* FEATURE_BT_AVSYNC_SOC */

/***************************************************************************/
/*         Structures to store information about what is known             */
/*                     about the Controller                                */
/***************************************************************************/

/* Structure to record the current size of the data portion of HCI ACL and
   Synchronous data packets sent from the Host to the Controller. */
typedef struct
{
  uint16              acl_data_pkt_len;       /* Current ACL data packet length
                                                 in bytes*/
  uint8               sco_data_pkt_len;       /* Current Sync data packet length
                                                 in bytes*/
  uint16              tot_free_sco_pkts;      /* Total freed sco packets in the
                                                 Controller */
  uint16              tot_free_data_pkts;     /* Total freed data packet records
                                                 in the Controller */
  bt_hc_drv_buf_type  entries[BT_MAX_BT_DEV]; /* Controller data buffer
                                                   parameters */
  uint8               num_conn;               /* Number of connections that are
                                                 using HCI buffer space */
} bt_hc_drv_ctrlr_cur_buf_type;

/* Enum type for the type of controller used. */
typedef enum
{
  QC_BT_EMBEDDED,
  CSR_BC4_SOC,
  QC_BTS_SOC
} bt_hc_drv_ctrlr_enum_type;

/* Structure to record information about the Controller */
typedef struct
{
  bt_hc_drv_ctrlr_enum_type    ctrlr_type;    /* Controller name */
  bt_hc_drv_ctrlr_max_buf_type max_buf_info;  /* Maximum ACL buffer size */
  bt_hc_drv_ctrlr_cur_buf_type cur_buf;       /* Current ACL buffer size */
#ifdef FEATURE_BT_HCI_HOST_FCTL
  boolean                      hc2h_flow_st;  /* Host flow control supported
                                                 status */
#endif /* FEATURE_BT_HCI_HOST_FCTL */
} bt_hc_drv_ctrlr_struct_type;

/***************************************************************************/
/*                    Host Transmit and Receive Structures                 */
/***************************************************************************/

/* Structure to record information about HCI ACL Connections */
typedef struct
{
  q_type                    acl_data_tx_q;  /* queue for transmit ACL data */
  dsm_watermark_type        acl_data_tx_wm; /* watermark for transmit ACL
                                               data */
  uint16                    conn_hndl;      /* Conenction handle */
  bt_bd_addr_type           bd_addr;        /* BD address */
  boolean                   flow_on;        /* Flow indicator */
#ifdef FEATURE_BT_AVSYNC_SOC
  q_type                    acl_data_tx_frame_handle_q;
                                            /* Queue for Tx ACL Data Headers */
  uint16                    num_unacked_pkts;
                                            /* Total Number of packets that is
                                             * currently outstanding or waiting
                                             * for num completes from the
                                             * controller. */
  uint16                    num_non_flushable_pkts;
                                            /* Number of non-flushable packets
                                               that is outstanding with the
                                               Controller */
  boolean                   buffer_flushed;
                                            /* Indication that the buffer was
                                               flushed and is waiting for a
                                               buffer flushed event */
  uint16                    flush_to;
                                            /* The flush timeout being used for
                                               flushable packets in this ACL
                                               link */
#endif /* FEATURE_BT_AVSYNC_SOC */
} bt_hc_drv_acl_tx_struct_type;

/* Structure to store information regarding HCI Command flow control
 * to the Controller
 */
typedef struct
{
  uint8                num_hci_cmd_pkts;  /* Number of HCI comands that the
                                           * controller can currently accept
                                           */
#ifdef BT_SWDEV_CMD_FLOW_WORKAROUND
  boolean              expecting_large_evt;
#endif
  q_type               hci_cmd_q;         /* Queue for HCI commands */
  dsm_watermark_type   hci_cmd_tx_wm;
} bt_hc_drv_cmd_tx_struct_type;

/* Structure to record information about Host HCI transmit operations
   to Controller */
typedef struct
{
  /* HCI Transmit state */
  bt_hc_drv_state_enum_type      hci_tx_state;

  /* Information about ACL data transmitted */
  bt_hc_drv_acl_tx_struct_type   acl_tx[BT_MAX_BT_DEV];

  /* Information about Commands transmitted */
  bt_hc_drv_cmd_tx_struct_type   cmd_tx;
} bt_hc_drv_tx_struct_type;

typedef union
{
  bt_hc_drv_event_state_enum_type  rx_ev_pkt_state;
  bt_hc_drv_acl_rx_state_enum_type rx_acl_pkt_state;
} bt_hc_drv_rx_pkt_st_union_type;

/* Structure to record information about Host HCI receptions from Controller */
typedef struct
{
  bt_hc_drv_state_enum_type      hci_rx_state; /* Rx state of the HCI */
  bt_hci_pkt_type                cur_pkt_type; /* Current HCI packet */
  bt_hc_drv_rx_pkt_st_union_type rx_pkt_state; /* State for building HCI Rx
                                                  packet */
  q_type                         acl_data_rx_q[BT_MAX_BT_DEV];
                                               /* queue for ACL Rx data */
  uint8                          hc_rx_cnt;    /* Count of events and ACL
                                                  received per processing of
                                                  SIO RX watermark*/
  uint16                         hc_rx_acl_cnt[BT_MAX_BT_DEV];
                                               /* Count of ACL packets
                                                  received per processing of
                                                  SIO RX watermark*/
#ifdef FEATURE_BT_HCI_HOST_FCTL
  boolean                        rx_acl_flow[BT_MAX_BT_DEV];
#endif /* FEATURE_BT_HCI_HOST_FCTL */
} bt_hc_drv_acl_rx_struct_type;

/* Structure to record information about the Host */
typedef struct
{
  bt_hc_drv_tx_struct_type       hci_tx;
  bt_hc_drv_acl_rx_struct_type   hci_rx;
  bt_hc_drv_txrx_cnt_type        bt_hc_drv_txrx_cnt;
#ifdef FEATURE_BT_HCI_HOST_FCTL
  boolean                        is_dsm_cnt_low;
  boolean                        is_small_dsm_cnt_low;
  boolean                        is_large_dsm_cnt_low;
#endif /* FEATURE_BT_HCI_HOST_FCTL */
} bt_hc_drv_host_struct_type;

/***********************************************************/
/* Structure to record Host and Controller HCI information */
/*                  in the Host HCI driver                 */
/***********************************************************/
typedef struct
{
  bt_hc_drv_ctrlr_struct_type    ctrlr_info;     /* Controller details */
  bt_hc_drv_host_struct_type     host_info;      /* Host details */
  boolean                        hci_sync_state; /* HCI synchronization flag */
} bt_hc_drv_struct_type;



/***********************************************************/
/*                    HCI packet stuctures                 */
/***********************************************************/
/* HCI command header */
typedef BT_PACKED struct
{
  uint8                         pkt_type;
  uint16                        opcode;
  uint8                         param_len;
} bt_hc_cmd_hdr_type;


/* HCI event structure */
typedef struct
{
  bt_event_type       bt_ev;       /* BT HCI event enum value  */
  bt_hci_ev_type      hci_ev;      /* HCI event value according to BT spec */
  uint8               hci_len;     /* Size of HCI event param.0 if variable */
  bt_ev_len_enum_type ev_len_mode; /* Fixed/variable length mode */
} bt_hc_drv_ev_struct_type;


/* HCI command packet */
typedef BT_PACKED struct
{
  bt_hc_cmd_hdr_type    bt_hc_cmd_hdr;
  bt_cmd_params_union   *bt_hc_cmd_params;
} bt_hc_cmd_type;


/* HCI ACL data header */
typedef BT_PACKED struct
{
  uint8                     pkt_type;
  uint16                    acl_conn_hdr;/* HCI ACL data connection header */
  uint16                    data_len;
} bt_hc_acl_data_hdr_type;


/* HCI ACL data packet structure */
typedef struct
{
  bt_hc_acl_data_hdr_type       acl_data_hdr;
  dsm_item_type*                acl_data_dsm_ptr;
} bt_hc_acl_data_type;


/* HCI command table structure */
typedef struct
{
  word                 opcode;           /* Command opcode */
  word                 param_len;        /* Command parameter length in byets */
  bt_cmd_type          bt_cmd;           /* The equivalent local bt cmd */
  boolean              cmd_cmplt_ev_expected;
                                         /* Flag to indicate if command
                                            complete event is expected */
  bt_cmd_len_enum_type cmd_len_mode;     /* Fixed/variable length mode */
} bt_hc_drv_cmd_tbl_struct_type;


/*=====================================================================
                           Constants and Macros
======================================================================*/
#define BT_MSG_LAYER  BT_MSG_HH   /* Necessary for btmsg.h */

#ifdef FEATURE_BT_HCI_HOST_FCTL

/* MAX FTP PKT size (with FEATURE_BT_REDUCE_MEMORY_USAGE enabled).
   TBD : This value needs to be linked with the MAX OBEX PKT size in btpf.c
   There is no such constant yet in btpf.c.  */
#ifdef FEATURE_BT_REDUCE_MEMORY_USAGE
#define BT_HC_DRV_FCTL_MAX_PROT_PKT_SIZE          16384    /*16k*/
#else
#define BT_HC_DRV_FCTL_MAX_PROT_PKT_SIZE          32768    /*32k*/
#endif

/* This macro returns the Upper limit to detect if a ACL connection is backed
   up with DSM items. The # of HCI ACL frames per connection queued at HCI TX
   WM shouldn't exceed upper limit of p*tot_num_acl_bufs.Where 'p' should
   be such as to allow atleast one complete max OBEX FTP packet to be queued */
#define BT_HC_DRV_ACL_TX_HIGH_FR_CNT(tot_num_acl_bufs,p) (p*tot_num_acl_bufs)

/* This macro returns the Lower limit to detect if HCI Tx WM for a connection
   is flowing on again. The # of outstanding HCI ACL frames  at HCI TX
   WaterMark should be less than or equal to (p-1)*tot_num_acl_bufs */
#define BT_HC_DRV_ACL_TX_LOW_FR_CNT(tot_num_acl_bufs,p) ((p-1)*tot_num_acl_bufs)
#endif /*FEATURE_BT_HCI_HOST_FCTL*/

/* Maximum values for ACL data packet, SCO data packet, total number of
   ACL, SCO data packets in Controller buffer */
#define BT_HC_DRV_MAX_ACL_DATA_PKT_LEN            384
#define BT_HC_DRV_MAX_SCO_DATA_PKT_LEN            255
#define BT_HC_DRV_TOT_NUM_ACL_DATA_PKTS           8
#define BT_HC_DRV_TOT_NUM_SCO_DATA_PKTS           8

/* Length of HCI command with bo parameters */
#define BT_HCI_CMD_NO_PARAM_LEN                    0

/* HCI ACL Tx watermark parameters */
#define BT_HC_DRV_ACL_TX_WM_LO_VAL                600
#define BT_HC_DRV_ACL_TX_WM_HI_VAL                3500

#ifdef FEATURE_BT_HCI_HOST_FCTL
/* The space between high water mark and "Don't exceed count" should be good 
   enough to accomodate max chunk from protocol layer ( RFCOMM\A2DP etc ).
   below macro takes care for two chunk of protocol data and 5 HCI commands 
   packets.
*/
#define BT_HC_DRV_FCTL_OVERFLOW_CNT    ( ( (2 * BT_MAX_PROT_CHUNK_BYTES ) / \
                   ( BT_MTP_MAX_HC_LEN + sizeof( bt_hc_cmd_hdr_type ) ) ) + 5 )
                   
#define BT_HC_DRV_FCTL_OVERFLOW_BYTES          BT_HC_DRV_FCTL_OVERFLOW_CNT * \
           ( BT_MTP_MAX_HC_LEN + sizeof( bt_hc_cmd_hdr_type ) )

#define BT_HC_DRV_ACL_TX_WM_DONT_EX_CNT        \
          BT_HC_DRV_FCTL_MAX_PROT_PKT_SIZE + BT_HC_DRV_FCTL_OVERFLOW_BYTES
#else
#define BT_HC_DRV_ACL_TX_WM_DONT_EX_CNT           10000
#endif /* FEATURE_BT_HCI_HOST_FCTL */

/* HCI Command Watermark parameters */
#ifdef FEATURE_BT_2_1
#define BT_HC_DRV_MAX_HCI_CMD_Q_SIZE              1000
#else
#define BT_HC_DRV_MAX_HCI_CMD_Q_SIZE              300
#endif /* FEATURE_BT_2_1 */

/* Initial value of connection handles before conenction is set-up */
#define BT_HC_DRV_ACL_TX_INIT_CHNDL               0xFF

#ifdef FEATURE_BT_AVSYNC_SOC
#define BT_HC_DRV_MAX_ACL_FRAMES                  200
#endif /* FEATURE_BT_AVSYNC_SOC */

/* Max number of frames (events and ACL) that is processed in one round of
   sio->BT Rx watermark processing and sent to RM. A value of 20 is used to
   account for the events that RM may receive from modules other than HCI */
#define BT_HC_DRV_MAX_NUM_FRAMES                  ( BT_RM_NUM_EVENTS - 20 )

/* Checks if the given event is a Number of Completed Packets Command. */
#define IS_HOST_NUM_CMPLT_PKTS_CMD( cmd )      \
          (( cmd == BT_CMD_HC_HOST_NUM_CMPLT_PACKETS) ? TRUE : FALSE )

/* Checks if the given event is a Number of Completed Packets Event. */
#define IS_NUM_CMPLT_PKTS_EV( ev )      \
          (( ev == BT_EV_HC_NUM_CMPLT_PKTS) ? TRUE : FALSE )

#ifdef FEATURE_BT_AVSYNC_SOC
/* Checks if the given event is a Flush Occurred Event */
#define IS_FLUSH_OCCURRED_EV( ev )      \
          (( ev == BT_EV_HC_FLUSH_OCCURRED) ? TRUE : FALSE )

#ifdef BT_SWDEV_2_1_PB
/* Checks if the given event is a Enhanced Flush Complete Event */
#define IS_ENH_FLUSH_COMPLETE_EV( ev )      \
          (( ev == BT_EV_HC_ENH_FLUSH_COMPLETE) ? TRUE : FALSE )
#endif /* BT_SWDEV_2_1_PB */
#endif /* FEATURE_BT_AVSYNC_SOC */

/* Checks if the given event is a Connection Complete Event. */
#define IS_CONN_COMPLETE_EV( ev )      \
          (( ev == BT_EV_HC_CONNECTION_COMPLETE) ? TRUE : FALSE )

/* Checks if the given event is a Completed Complete Event. */
#define IS_CMD_CMPLT_EV( ev )      \
                              (( ev == BT_EV_HC_COMMAND_COMPLETE) ? TRUE : FALSE )

/* Checks if the given event is a Disconnection Complete Event. */
#define IS_DISCONN_COMPLETE_EV( ev )      \
          (( ev == BT_EV_HC_DISCONNECTION_COMPLETE) ? TRUE : FALSE )

/* Checks if the given event is a Completed Status Event. */
#define IS_CMD_STATUS_EV( ev )      \
                              (( ev == BT_EV_HC_COMMAND_STATUS) ? TRUE : FALSE )

/* Number of entries in the BT HCI commands table */
#define BT_HC_DRV_CMD_TABLE_NUM_ENTRIES  \
        (sizeof( bt_hc_drv_cmd_table )/sizeof( bt_hc_drv_cmd_tbl_struct_type ))

/* Number of entries in the BT HCI events table */
#define BT_HC_DRV_EV_TABLE_NUM_ENTRIES  \
              (sizeof( bt_hc_drv_ev_table )/sizeof( bt_hc_drv_ev_struct_type ))


/* Table of HCI commands to be sent from Host to Controller */
const bt_hc_drv_cmd_tbl_struct_type bt_hc_drv_cmd_table[] =
{
  { NULL,                              sizeof( bt_cmd_hc_tx_acl_data_type ),
    BT_CMD_HC_TX_ACL_DATA,             FALSE,  BT_CMD_VAR_LENGTH },

  { NULL,                              sizeof( bt_cmd_hc_tx_sco_data_type ),
    BT_CMD_HC_TX_SCO_DATA,             FALSE,  BT_CMD_VAR_LENGTH },

  /* Link Control Commands */
  { BT_HCI_INQUIRY,                    sizeof( bt_cmd_hc_inquiry_type ),
    BT_CMD_HC_INQUIRY,                 FALSE,  BT_CMD_FIXED_LEN },

  { BT_HCI_INQUIRY_CANCEL,             BT_HCI_CMD_NO_PARAM_LEN,
    BT_CMD_HC_INQUIRY_CANCEL,          TRUE, BT_CMD_FIXED_LEN },

  { BT_HCI_PERIODIC_INQUIRY_MODE,      sizeof( bt_cmd_hc_periodic_inq_type ),
    BT_CMD_HC_PERIODIC_INQUIRY,        TRUE, BT_CMD_FIXED_LEN },

  { BT_HCI_EXIT_PERIODIC_INQUIRY_MODE, BT_HCI_CMD_NO_PARAM_LEN,
    BT_CMD_HC_EXIT_PERIODIC_INQUIRY,   TRUE, BT_CMD_FIXED_LEN },

  { BT_HCI_CREATE_CONNECTION,          sizeof( bt_cmd_hc_create_conn_type ),
    BT_CMD_HC_CREATE_CONNECTION,       FALSE, BT_CMD_FIXED_LEN },

  { BT_HCI_DISCONNECT,                 sizeof( bt_cmd_hc_disconn_type ),
    BT_CMD_HC_DISCONNECT,              FALSE, BT_CMD_FIXED_LEN },

  { BT_HCI_ADD_SCO_CONNECTION,         sizeof( bt_cmd_hc_add_sco_conn_type ),
    BT_CMD_HC_ADD_SCO_CONN,            FALSE, BT_CMD_FIXED_LEN },

#ifdef FEATURE_BT_1_2
  { BT_HCI_CREATE_CONNECTION_CANCEL,   sizeof( bt_cmd_hc_create_conn_cancel_type ),
    BT_CMD_HC_CREATE_CONNECTION_CANCEL, FALSE, BT_CMD_FIXED_LEN },
#endif

  { BT_HCI_ACCEPT_CONNECTION_RQST,     sizeof( bt_cmd_hc_accept_conn_type ),
    BT_CMD_HC_ACCEPT_CONN,             FALSE, BT_CMD_FIXED_LEN },

  { BT_HCI_REJECT_CONNECTION_RQST,     sizeof( bt_cmd_hc_reject_conn_type ),
    BT_CMD_HC_REJECT_CONN,             FALSE, BT_CMD_FIXED_LEN },

  { BT_HCI_LINK_KEY_RQST_REPLY,        sizeof( bt_cmd_hc_link_key_type ),
    BT_CMD_HC_LINK_KEY,                TRUE, BT_CMD_FIXED_LEN },

  { BT_HCI_LINK_KEY_RQST_NEG_REPLY,    sizeof( bt_cmd_hc_bd_addr_type ),
    BT_CMD_HC_LINK_KEY_NEG,            TRUE, BT_CMD_FIXED_LEN },

  { BT_HCI_PIN_CODE_RQST_REPLY,        sizeof( bt_cmd_hc_pin_code_type ),
    BT_CMD_HC_PIN_CODE,                TRUE, BT_CMD_FIXED_LEN },

  { BT_HCI_PIN_CODE_RQST_NEG_REPLY,    sizeof( bt_cmd_hc_bd_addr_type ),
    BT_CMD_HC_PIN_CODE_NEG,            TRUE, BT_CMD_FIXED_LEN },

  { BT_HCI_CHANGE_CONNECTION_PKT_TYPE, sizeof( bt_cmd_hc_change_pkt_type_type ),
    BT_CMD_HC_CHANGE_PKT_TYPE,         FALSE, BT_CMD_FIXED_LEN },

  { BT_HCI_AUTHENTICATION_REQUESTED,   sizeof( bt_cmd_hc_conn_hndl_type ),
    BT_CMD_HC_AUTH_REQ,                FALSE, BT_CMD_FIXED_LEN },

  { BT_HCI_SET_CONNECTION_ENCRYPTION, sizeof( bt_cmd_hc_set_conn_encrypt_type ),
    BT_CMD_HC_SET_CONN_ENCRYPT,       FALSE, BT_CMD_FIXED_LEN },

  { BT_HCI_CHANGE_CONNECTION_LINK_KEY, sizeof( bt_cmd_hc_conn_hndl_type ),
    BT_CMD_HC_CHANGE_LINK_KEY,        FALSE, BT_CMD_FIXED_LEN },

  { BT_HCI_MASTER_LINK_KEY,           sizeof( bt_cmd_hc_master_link_key_type ),
    BT_CMD_HC_MASTER_LINK_KEY,        FALSE, BT_CMD_FIXED_LEN },

  { BT_HCI_REMOTE_NAME_RQST,          sizeof( bt_cmd_hc_remote_name_req_type ),
    BT_CMD_HC_REMOTE_NAME_REQ,        FALSE, BT_CMD_FIXED_LEN },

#ifdef FEATURE_BT_1_2
  { BT_HCI_REMOTE_NAME_RQST_CANCEL,   sizeof( bt_cmd_hc_remote_name_req_cancel_type ),
    BT_CMD_HC_REMOTE_NAME_REQ_CANCEL, FALSE, BT_CMD_FIXED_LEN },
#endif

  { BT_HCI_RD_REMOTE_SUPPORTED_FEAT,  sizeof( bt_cmd_hc_conn_hndl_type ),
    BT_CMD_HC_RD_REMOTE_FEATURES,     FALSE, BT_CMD_FIXED_LEN },

#ifdef FEATURE_BT_1_2
  { BT_HCI_RD_REMOTE_EXTENDED_FEAT,   sizeof( bt_cmd_hc_rd_rmt_ext_feat_type ),
    BT_CMD_HC_RD_REMOTE_EXT_FEATURES, FALSE, BT_CMD_FIXED_LEN },
#endif

  { BT_HCI_RD_REMOTE_VERSION_INFO,    sizeof( bt_cmd_hc_conn_hndl_type ),
    BT_CMD_HC_RD_REMOTE_VERSION,      FALSE, BT_CMD_FIXED_LEN },

  { BT_HCI_RD_CLOCK_OFFSET,           sizeof( bt_cmd_hc_conn_hndl_type ),
    BT_CMD_HC_RD_CLK_OFFSET,          FALSE, BT_CMD_FIXED_LEN },

#ifdef BT_SWDEV_2_1_SSP
  { BT_HCI_IO_CAPABILITY_RQST_REPLY,  sizeof( bt_cmd_hc_io_cap_response_type ),
    BT_CMD_HC_IO_CAPABILITY_RQST_REPLY, TRUE, BT_CMD_FIXED_LEN },

  { BT_HCI_USER_CFMTN_RQST_REPLY,
    sizeof( bt_cmd_hc_user_cfmtn_rqst_rep_type ),
    BT_CMD_HC_USER_CFMTN_RQST_REPLY,  TRUE, BT_CMD_FIXED_LEN },

  { BT_HCI_USER_CFMTN_RQST_NEG_REPLY,
    sizeof( bt_cmd_hc_user_cfmtn_rqst_neg_rep_type ),
    BT_CMD_HC_USER_CFMTN_RQST_NEG_REPLY, TRUE, BT_CMD_FIXED_LEN },

  { BT_HCI_USER_PASSKEY_RQST_REPLY,
    sizeof( bt_cmd_hc_user_passkey_rqst_rep_type ),
    BT_CMD_HC_USER_PASSKEY_RQST_REPLY, TRUE, BT_CMD_FIXED_LEN },

  { BT_HCI_USER_PASSKEY_RQST_NEG_REPLY,
    sizeof( bt_cmd_hc_user_passkey_rqst_neg_rep_type ),
    BT_CMD_HC_USER_PASSKEY_RQST_NEG_REPLY, TRUE, BT_CMD_FIXED_LEN },

  { BT_HCI_REMOTE_OOB_DATA_RQST_REPLY,
    sizeof( bt_cmd_hc_remote_oob_data_rqst_rep_type ),
    BT_CMD_HC_REMOTE_OOB_DATA_RQST_REPLY, TRUE, BT_CMD_FIXED_LEN },

  { BT_HCI_REMOTE_OOB_DATA_RQST_NEG_REPLY,
    sizeof( bt_cmd_hc_remote_oob_data_rqst_neg_rep_type ),
    BT_CMD_HC_REMOTE_OOB_DATA_RQST_NEG_REPLY, TRUE, BT_CMD_FIXED_LEN },

  { BT_HCI_IO_CAPABILITY_RQST_NEG_REPLY,
    sizeof( bt_cmd_hc_io_cap_rqst_neg_rep_type ),
    BT_CMD_HC_IO_CAPABILITY_RQST_NEG_REPLY, TRUE, BT_CMD_FIXED_LEN },
#endif /* BT_SWDEV_2_1_SSP */

  /* Link Policy Commands */
  { BT_HCI_HOLD_MODE,                 sizeof( bt_cmd_hc_hold_mode_type ),
    BT_CMD_HC_HOLD_MODE,              FALSE, BT_CMD_FIXED_LEN },

  { BT_HCI_SNIFF_MODE,                sizeof( bt_cmd_hc_sniff_mode_type ),
    BT_CMD_HC_SNIFF_MODE,             FALSE, BT_CMD_FIXED_LEN },

  { BT_HCI_EXIT_SNIFF_MODE,           sizeof( bt_cmd_hc_exit_sniff_mode_type ),
    BT_CMD_HC_EXIT_SNIFF_MODE,        FALSE, BT_CMD_FIXED_LEN },

  { BT_HCI_PARK_MODE,                 sizeof( bt_cmd_hc_park_mode_type ),
    BT_CMD_HC_PARK_MODE,              FALSE, BT_CMD_FIXED_LEN },

  { BT_HCI_EXIT_PARK_MODE,            sizeof( bt_cmd_hc_exit_park_mode_type ),
    BT_CMD_HC_EXIT_PARK_MODE,         FALSE, BT_CMD_FIXED_LEN },

  { BT_HCI_QOS_SETUP,                 sizeof( bt_cmd_hc_qos_setup_type ),
    BT_CMD_HC_QOS_SETUP,              FALSE, BT_CMD_FIXED_LEN },

  { BT_HCI_ROLE_DISCOVERY,            sizeof( bt_cmd_hc_conn_hndl_type ),
    BT_CMD_HC_ROLE_DISCOVERY,         TRUE, BT_CMD_FIXED_LEN },

  { BT_HCI_SWITCH_ROLE,               sizeof( bt_cmd_hc_switch_role_type ),
    BT_CMD_HC_SWITCH_ROLE,            FALSE, BT_CMD_FIXED_LEN },

  { BT_HCI_RD_LINK_POLICY_SETTINGS,   sizeof( bt_cmd_hc_conn_hndl_type ),
    BT_CMD_HC_RD_LINK_POLICY,         TRUE, BT_CMD_FIXED_LEN },

  { BT_HCI_WR_LINK_POLICY_SETTINGS,   sizeof( bt_cmd_hc_wr_link_policy_type ),
    BT_CMD_HC_WR_LINK_POLICY,         TRUE, BT_CMD_FIXED_LEN },

#ifdef BT_SWDEV_2_1_SSR
  { BT_HCI_SNIFF_SUBRATING,           sizeof( bt_cmd_hc_sniff_subrating_type ),
    BT_CMD_HC_SNIFF_SUBRATING,        TRUE, BT_CMD_FIXED_LEN },
#endif /* BT_SWDEV_2_1_SSR */

  /* Host Control & Baseband Commands */
  { BT_HCI_SET_EVENT_MASK,            sizeof( bt_cmd_hc_set_event_mask_type ),
    BT_CMD_HC_SET_EVENT_MASK,         TRUE, BT_CMD_FIXED_LEN },

  { BT_HCI_RESET,                     BT_HCI_CMD_NO_PARAM_LEN,
    BT_CMD_HC_RESET,                  TRUE, BT_CMD_FIXED_LEN },

  { BT_HCI_SET_EVENT_FILTER,          sizeof( bt_cmd_hc_set_event_filter_type),
    BT_CMD_HC_SET_EVENT_FILTER,       TRUE, BT_CMD_FIXED_LEN },

  { BT_HCI_FLUSH,                     sizeof( bt_cmd_hc_conn_hndl_type ),
    BT_CMD_HC_FLUSH,                  TRUE, BT_CMD_FIXED_LEN },

  { BT_HCI_RD_PIN_TYPE,               BT_HCI_CMD_NO_PARAM_LEN,
    BT_CMD_HC_RD_PIN_TYPE,            TRUE, BT_CMD_FIXED_LEN },

  { BT_HCI_WR_PIN_TYPE,               sizeof( bt_cmd_hc_wr_pin_type ),
    BT_CMD_HC_WR_PIN_TYPE,            TRUE, BT_CMD_FIXED_LEN },

  { BT_HCI_CREATE_NEW_UNIT_KEY,       BT_HCI_CMD_NO_PARAM_LEN,
    BT_CMD_HC_CREATE_NEW_UNIT_KEY,    TRUE, BT_CMD_FIXED_LEN },

  { BT_HCI_RD_STORED_LINK_KEY,        sizeof( bt_cmd_hc_rd_link_key_type ),
    BT_CMD_HC_RD_STORED_LINK_KEY,     TRUE, BT_CMD_FIXED_LEN },

  { BT_HCI_WR_STORED_LINK_KEY,        sizeof(bt_cmd_hc_wr_link_key_type),
    BT_CMD_HC_WR_STORED_LINK_KEY,     TRUE, BT_CMD_VAR_LENGTH },

  { BT_HCI_DELETE_STORED_LINK_KEY,    sizeof( bt_cmd_hc_del_link_key_type ),
    BT_CMD_HC_DEL_STORED_LINK_KEY,    TRUE, BT_CMD_FIXED_LEN },

  { BT_HCI_CHANGE_LOCAL_NAME,        sizeof( bt_cmd_hc_change_local_name_type ),
    BT_CMD_HC_CHANGE_LOCAL_NAME,      TRUE, BT_CMD_VAR_LENGTH },

  { BT_HCI_RD_CONN_ACCEPT_TIMEOUT,    BT_HCI_CMD_NO_PARAM_LEN,
    BT_CMD_HC_RD_CONN_ACCEPT_TO,      TRUE, BT_CMD_FIXED_LEN },

  { BT_HCI_WR_CONN_ACCEPT_TIMEOUT,   sizeof( bt_cmd_hc_wr_conn_accept_to_type ),
    BT_CMD_HC_WR_CONN_ACCEPT_TO,      TRUE, BT_CMD_FIXED_LEN },

  { BT_HCI_RD_PAGE_TIMEOUT,           BT_HCI_CMD_NO_PARAM_LEN,
    BT_CMD_HC_RD_PAGE_TO,             TRUE, BT_CMD_FIXED_LEN },

  { BT_HCI_WR_PAGE_TIMEOUT,           sizeof( bt_cmd_hc_wr_page_to_type ),
    BT_CMD_HC_WR_PAGE_TO,             TRUE, BT_CMD_FIXED_LEN },

  { BT_HCI_RD_SCAN_ENABLE,            BT_HCI_CMD_NO_PARAM_LEN,
    BT_CMD_HC_RD_SCAN_ENABLE,         TRUE, BT_CMD_FIXED_LEN },

  { BT_HCI_WR_SCAN_ENABLE,            sizeof( bt_cmd_hc_wr_scan_enable_type ),
    BT_CMD_HC_WR_SCAN_ENABLE,         TRUE, BT_CMD_FIXED_LEN },

  { BT_HCI_RD_PAGESCAN_ACTIVITY,      BT_HCI_CMD_NO_PARAM_LEN,
    BT_CMD_HC_RD_PAGE_SCAN_ACTIVITY,  TRUE, BT_CMD_FIXED_LEN },

  { BT_HCI_WR_PAGESCAN_ACTIVITY,      sizeof( bt_cmd_hc_wr_page_scan_act_type ),
    BT_CMD_HC_WR_PAGE_SCAN_ACTIVITY,  TRUE, BT_CMD_FIXED_LEN },

  { BT_HCI_RD_INQUIRYSCAN_ACTIVITY,   BT_HCI_CMD_NO_PARAM_LEN,
    BT_CMD_HC_RD_INQ_SCAN_ACTIVITY,   TRUE, BT_CMD_FIXED_LEN },

  { BT_HCI_WR_INQUIRYSCAN_ACTIVITY,   sizeof( bt_cmd_hc_wr_inq_scan_act_type ),
    BT_CMD_HC_WR_INQ_SCAN_ACTIVITY,   TRUE, BT_CMD_FIXED_LEN },

  { BT_HCI_RD_AUTHENTICATION_ENABLE,  BT_HCI_CMD_NO_PARAM_LEN,
    BT_CMD_HC_RD_AUTH_ENABLE,         TRUE, BT_CMD_FIXED_LEN },

  { BT_HCI_WR_AUTHENTICATION_ENABLE,  sizeof( bt_cmd_hc_wr_auth_enable_type ),
    BT_CMD_HC_WR_AUTH_ENABLE,         TRUE, BT_CMD_FIXED_LEN },

  { BT_HCI_RD_ENCRYPTION_MODE,        BT_HCI_CMD_NO_PARAM_LEN,
    BT_CMD_HC_RD_ENCRYPT_MODE,        TRUE, BT_CMD_FIXED_LEN },

  { BT_HCI_WR_ENCRYPTION_MODE,        sizeof( bt_cmd_hc_wr_encrypt_mode_type ),
    BT_CMD_HC_WR_ENCRYPT_MODE,        TRUE, BT_CMD_FIXED_LEN },

  { BT_HCI_RD_CLASS_OF_DEVICE,        BT_HCI_CMD_NO_PARAM_LEN,
    BT_CMD_HC_RD_CLASS_OF_DEVICE,     TRUE, BT_CMD_FIXED_LEN },

  { BT_HCI_WR_CLASS_OF_DEVICE,        sizeof( bt_cmd_hc_wr_class_of_dev_type ),
    BT_CMD_HC_WR_CLASS_OF_DEVICE,     TRUE, BT_CMD_FIXED_LEN },

  { BT_HCI_RD_VOICE_SETTING,          BT_HCI_CMD_NO_PARAM_LEN,
    BT_CMD_HC_RD_VOICE_SETTING,       TRUE, BT_CMD_FIXED_LEN },

  { BT_HCI_WR_VOICE_SETTING,          sizeof( bt_cmd_hc_wr_voice_setting_type ),
    BT_CMD_HC_WR_VOICE_SETTING,       TRUE, BT_CMD_FIXED_LEN },

  { BT_HCI_RD_AUTO_FLUSH_TIMEOUT,     sizeof( bt_cmd_hc_conn_hndl_type ),
    BT_CMD_HC_RD_AUTO_FLUSH_TO,       TRUE, BT_CMD_FIXED_LEN },

  { BT_HCI_WR_AUTO_FLUSH_TIMEOUT,     sizeof( bt_cmd_hc_wr_auto_flush_to_type ),
    BT_CMD_HC_WR_AUTO_FLUSH_TO,       TRUE, BT_CMD_FIXED_LEN },

  { BT_HCI_RD_NUM_BC_RETRANSMISSIONS, BT_HCI_CMD_NO_PARAM_LEN,
    BT_CMD_HC_RD_BROAD_RETRANS,       TRUE, BT_CMD_FIXED_LEN },

  { BT_HCI_WR_NUM_BC_RETRANSMISSIONS, sizeof( bt_cmd_hc_wr_broad_retrans_type ),
    BT_CMD_HC_WR_BROAD_RETRANS,       TRUE, BT_CMD_FIXED_LEN },

  { BT_HCI_RD_HOLD_MODE_ACTIVITY,     BT_HCI_CMD_NO_PARAM_LEN,
    BT_CMD_HC_RD_HOLD_MODE_ACTIVITY,  TRUE, BT_CMD_FIXED_LEN },

  { BT_HCI_WR_HOLD_MODE_ACTIVITY,     sizeof( bt_cmd_hc_wr_hold_mode_act_type ),
    BT_CMD_HC_WR_HOLD_MODE_ACTIVITY,  TRUE, BT_CMD_FIXED_LEN },

  { BT_HCI_RD_TRANSMIT_POWER_LEVEL,   sizeof( bt_cmd_hc_rd_tx_power_type ),
    BT_CMD_HC_RD_TX_POWER_LEVEL,      TRUE, BT_CMD_FIXED_LEN },

  { BT_HCI_RD_SCO_FLOW_CONTROL_ENABLE, BT_HCI_CMD_NO_PARAM_LEN,
    BT_CMD_HC_RD_SCO_FLOW_CTRL,       TRUE, BT_CMD_FIXED_LEN },

  { BT_HCI_WR_SCO_FLOW_CONTROL_ENABLE,
                                      sizeof( bt_cmd_hc_wr_sco_flow_ctrl_type ),
    BT_CMD_HC_WR_SCO_FLOW_CTRL,         TRUE, BT_CMD_FIXED_LEN },

  { BT_HCI_SET_HC_TO_HOST_FLOW_CONTROL,
                                     sizeof( bt_cmd_hc_set_hc_to_host_fc_type ),
    BT_CMD_HC_SET_HC_TO_HOST_FC,      TRUE, BT_CMD_FIXED_LEN },

  { BT_HCI_HOST_BUFFER_SIZE,          sizeof( bt_cmd_hc_host_buf_size_type ),
    BT_CMD_HC_HOST_BUFFER_SIZE,       TRUE, BT_CMD_FIXED_LEN },

  { BT_HCI_HOST_NUM_COMPLETED_PACKETS,
                                    sizeof( bt_cmd_hc_host_num_cmplt_pkt_type ),
    BT_CMD_HC_HOST_NUM_CMPLT_PACKETS, TRUE, BT_CMD_VAR_LENGTH },

  { BT_HCI_RD_LINK_SUPERVISION_TIMEOUT,
                                             sizeof( bt_cmd_hc_conn_hndl_type ),
    BT_CMD_HC_RD_LINK_SUPERVISION_TO, TRUE, BT_CMD_VAR_LENGTH },

  { BT_HCI_WR_LINK_SUPERVISION_TIMEOUT,
                                      sizeof( bt_cmd_hc_wr_link_super_to_type ),
    BT_CMD_HC_WR_LINK_SUPERVISION_TO, TRUE, BT_CMD_FIXED_LEN },

  { BT_HCI_RD_NUMBER_OF_SUPPORTED_IAC,BT_HCI_CMD_NO_PARAM_LEN,
    BT_CMD_HC_RD_NUM_SUPPORTED_IAC,   TRUE, BT_CMD_FIXED_LEN },

  { BT_HCI_RD_CURRENT_IAC_LAP,        BT_HCI_CMD_NO_PARAM_LEN,
    BT_CMD_HC_RD_CURRENT_IAC_LAP,     TRUE, BT_CMD_FIXED_LEN },

  { BT_HCI_WR_CURRENT_IAC_LAP,        sizeof( bt_cmd_hc_wr_curr_iac_lap_type),
    BT_CMD_HC_WR_CURRENT_IAC_LAP,     TRUE, BT_CMD_VAR_LENGTH },

  { BT_HCI_RD_PAGE_SCAN_PERIOD_MODE,  BT_HCI_CMD_NO_PARAM_LEN,
    BT_CMD_HC_RD_PAGE_SCAN_PER_MODE,  TRUE, BT_CMD_FIXED_LEN },

  { BT_HCI_WR_PAGE_SCAN_PERIOD_MODE,  sizeof( bt_cmd_hc_wr_ps_per_mode_type ),
    BT_CMD_HC_WR_PAGE_SCAN_PER_MODE,  TRUE, BT_CMD_FIXED_LEN },

#ifndef FEATURE_BT_1_2 /* BT 1.1 */
  { BT_HCI_RD_PAGE_SCAN_MODE,         BT_HCI_CMD_NO_PARAM_LEN,
    BT_CMD_HC_RD_PAGE_SCAN_MODE,      TRUE, BT_CMD_FIXED_LEN },

  { BT_HCI_WR_PAGE_SCAN_MODE,         sizeof( bt_cmd_hc_wr_ps_mode_type ),
    BT_CMD_HC_WR_PAGE_SCAN_MODE,      TRUE, BT_CMD_FIXED_LEN },
#endif

#ifdef BT_SWDEV_2_1_SSP
  { BT_HCI_REFRESH_ENCRYPTION_KEY,
    sizeof( bt_cmd_hc_refresh_encription_key_type ),
    BT_CMD_HC_REFRESH_ENCRYPTION_KEY, FALSE, BT_CMD_FIXED_LEN },

  { BT_HCI_RD_SIMPLE_PAIRING_MODE,    BT_HCI_CMD_NO_PARAM_LEN,
    BT_CMD_HC_RD_SIMPLE_PAIRING_MODE, TRUE, BT_CMD_FIXED_LEN },

  { BT_HCI_WR_SIMPLE_PAIRING_MODE,    sizeof( bt_cmd_hc_wr_sp_mode_type ),
    BT_CMD_HC_WR_SIMPLE_PAIRING_MODE, TRUE, BT_CMD_FIXED_LEN },
#endif /* BT_SWDEV_2_1_SSP */

#ifdef FEATURE_BT_2_1
  { BT_HCI_RD_DEFAULT_ERR_DATA_REPORTING,    BT_HCI_CMD_NO_PARAM_LEN,
      BT_CMD_HC_RD_DEFAULT_ERR_DATA_REPORTING_MODE, TRUE, BT_CMD_FIXED_LEN },

  { BT_HCI_WR_DEFAULT_ERR_DATA_REPORTING,
                            sizeof( bt_cmd_hc_wr_err_data_reporting_type ),
      BT_CMD_HC_WR_DEFAULT_ERR_DATA_REPORTING_MODE, TRUE, BT_CMD_FIXED_LEN },
#endif /* FEATURE_BT_2_1 */

#ifdef BT_SWDEV_2_1_SSP
  { BT_HCI_RD_LOCAL_OOB_DATA,         BT_HCI_CMD_NO_PARAM_LEN,
    BT_CMD_HC_RD_LOCAL_OOB_DATA,      TRUE, BT_CMD_FIXED_LEN },

  { BT_HCI_SEND_KEYPRESS_NOTIFICATION,
    sizeof( bt_cmd_hc_send_keypress_ntfctn_type_type ),
    BT_CMD_HC_SEND_KEYPRESS_NOTIFICATION, TRUE, BT_CMD_FIXED_LEN },
#endif /* BT_SWDEV_2_1_SSP */

  /* Information Parameters */
  { BT_HCI_RD_LOCAL_VERSION_INFO,     BT_HCI_CMD_NO_PARAM_LEN,
    BT_CMD_HC_RD_LOCAL_VERSION_INFO,  TRUE, BT_CMD_FIXED_LEN },

  { BT_HCI_RD_LOCAL_SUPPORTED_CMDS,   BT_HCI_CMD_NO_PARAM_LEN,
    BT_CMD_HC_RD_LOCAL_CMDS,          TRUE, BT_CMD_FIXED_LEN },

  { BT_HCI_RD_LOCAL_SUPPORTED_FEAT,   BT_HCI_CMD_NO_PARAM_LEN,
    BT_CMD_HC_RD_LOCAL_FEATURES,      TRUE, BT_CMD_FIXED_LEN },

  { BT_HCI_RD_LOCAL_EXTENDED_FEATURES,   sizeof( bt_cmd_hc_rd_local_ext_feat_type ),
    BT_CMD_HC_RD_LOCAL_EXT_FEAT,         TRUE, BT_CMD_FIXED_LEN },

  { BT_HCI_RD_BUFFER_SIZE,            BT_HCI_CMD_NO_PARAM_LEN,
    BT_CMD_HC_RD_BUFFER_SIZE,         TRUE, BT_CMD_FIXED_LEN },

  { BT_HCI_RD_COUNTRY_CODE,           BT_HCI_CMD_NO_PARAM_LEN,
    BT_CMD_HC_RD_COUNTRY_CODE,        TRUE, BT_CMD_FIXED_LEN },

  { BT_HCI_RD_BD_ADDR,                BT_HCI_CMD_NO_PARAM_LEN,
    BT_CMD_HC_RD_BD_ADDR,             TRUE, BT_CMD_FIXED_LEN },

  /* Status Parameters */
  { BT_HCI_RD_FAILED_CONTACT_COUNTER,       sizeof( bt_cmd_hc_conn_hndl_type ),
    BT_CMD_HC_RD_FAILED_CONTACT_COUNTER,    TRUE, BT_CMD_FIXED_LEN },

  { BT_HCI_RESET_FAILED_CONTACT_COUNTER,    sizeof( bt_cmd_hc_conn_hndl_type ),
    BT_CMD_HC_RESET_FAILED_CONTACT_COUNTER, TRUE, BT_CMD_FIXED_LEN },

  { BT_HCI_GET_LINK_QUALITY,                sizeof( bt_cmd_hc_conn_hndl_type ),
    BT_CMD_HC_GET_LINK_QUALITY,             TRUE, BT_CMD_FIXED_LEN },

  { BT_HCI_RD_RSSI,                         sizeof( bt_cmd_hc_conn_hndl_type ),
    BT_CMD_HC_RD_RSSI,                      TRUE, BT_CMD_FIXED_LEN},

  /* Test Commands */
  { BT_HCI_RD_LOOPBACK_MODE,          BT_HCI_CMD_NO_PARAM_LEN,
    BT_CMD_HC_RD_LOOPBACK_MODE,       TRUE, BT_CMD_FIXED_LEN },

  { BT_HCI_WR_LOOPBACK_MODE,          sizeof( bt_cmd_hc_wr_loopback_mode_type ),
    BT_CMD_HC_WR_LOOPBACK_MODE,       TRUE, BT_CMD_FIXED_LEN },

  { BT_HCI_ENABLE_DUT_MODE,           BT_HCI_CMD_NO_PARAM_LEN,
    BT_CMD_HC_ENABLE_DUT_MODE,        TRUE, BT_CMD_FIXED_LEN }

#ifdef BT_SWDEV_2_1_SSP
 ,{ BT_HCI_WR_SIMPLE_PAIRING_DEBUG_MODE,
    sizeof( bt_cmd_hc_wr_sp_debug_mode_type ),
    BT_CMD_HC_WR_SIMPLE_PAIRING_DEBUG_MODE, TRUE, BT_CMD_FIXED_LEN }
#endif /* BT_SWDEV_2_1_SSP */

#ifndef FEATURE_BT_SOC
  /* Qualcomm specific commands */
#ifndef FEATURE_BT_TEST_MODE_ONLY
 ,{ BT_HCI_FIX_INIT_WHITE,            sizeof( bt_cmd_hc_fix_init_white_type ),
    BT_CMD_HC_FIX_INIT_WHITE,         TRUE, BT_CMD_FIXED_LEN },

  { BT_HCI_FIX_INIT_ENC,              sizeof( bt_cmd_hc_fix_init_enc_type ),
    BT_CMD_HC_FIX_INIT_ENC,           TRUE, BT_CMD_FIXED_LEN },

  { BT_HCI_READ_RX_STATUS_ENABLE,     sizeof( bt_cmd_hc_rd_rx_status_en_type ),
    BT_CMD_HC_RD_RX_STATUS_EN,        TRUE, BT_CMD_FIXED_LEN },

  { BT_HCI_CONFIG_SEARCHER,           sizeof( bt_cmd_hc_config_srch_type ),
    BT_CMD_HC_CONFIG_SCHR,            TRUE, BT_CMD_FIXED_LEN },

  { BT_HCI_READ_TX_BUFFER_ENABLE,     sizeof( bt_cmd_hc_rd_tx_buffer_en_type ),
    BT_CMD_HC_RD_TX_BUFFER_EN,        TRUE, BT_CMD_FIXED_LEN },

  { BT_HCI_READ_RX_BUFFER_ENABLE,     sizeof( bt_cmd_hc_rd_rx_buffer_en_type ),
    BT_CMD_HC_RD_RX_BUFFER_EN,        TRUE, BT_CMD_FIXED_LEN },

  { BT_HCI_SEND_RAW_MODE_PACKET,      sizeof(bt_cmd_hc_send_raw_mode_pkt_type),
    BT_CMD_HC_SEND_RAW_MODE_PKT,      TRUE, BT_CMD_VAR_LENGTH }

#endif /* !defined( FEATURE_BT_TEST_MODE_ONLY ) */
#ifdef FEATURE_BT_TEST_MODE_TESTER
 ,{ BT_HCI_TESTER_ACTIVATE,           sizeof( bt_cmd_hc_conn_hndl_type ),
    BT_CMD_HC_TESTER_ACTIVATE,        FALSE, BT_CMD_FIXED_LEN },

  { BT_HCI_TESTER_CONTROL,            sizeof(bt_cmd_hc_tester_control_type),
    BT_CMD_HC_TESTER_CONTROL,         FALSE, BT_CMD_VAR_LENGTH },

  { BT_HCI_TESTER_START,              sizeof( bt_cmd_hc_conn_hndl_type ),
    BT_CMD_HC_TESTER_START,           TRUE,  BT_CMD_FIXED_LEN }

#endif /* FEATURE_BT_TEST_MODE_TESTER */
 ,{ BT_HCI_SET_NEW_BD_ADDR,           sizeof( bt_cmd_hc_bd_addr_type ),
    BT_CMD_HC_SET_NEW_BD_ADDR,        TRUE,  BT_CMD_FIXED_LEN },

  { BT_HCI_LMP_TIME_ACCURACY_REQ,     BT_HCI_CMD_NO_PARAM_LEN,
    BT_CMD_HC_TIME_ACCURACY_REQ,      FALSE, BT_CMD_FIXED_LEN },

  { BT_HCI_MODIFY_BEACON,             sizeof( bt_cmd_hc_modify_beacon_type ),
    BT_CMD_HC_MODIFY_BEACON,          TRUE, BT_CMD_FIXED_LEN },

  { BT_HCI_RD_CLK,                    BT_HCI_CMD_NO_PARAM_LEN,
    BT_CMD_HC_RD_CLK,                 TRUE, BT_CMD_FIXED_LEN },

  { BT_HCI_DIAC_LAP_EN,               sizeof( bt_cmd_hc_diac_lap_en_type ),
    BT_CMD_HC_DIAC_LAP_EN,            TRUE, BT_CMD_FIXED_LEN },

  { BT_HCI_RD_ENC_KEY_LENGTH,         BT_HCI_CMD_NO_PARAM_LEN,
    BT_CMD_HC_RD_ENC_KEY_LENGTH,      FALSE, BT_CMD_FIXED_LEN },

  { BT_HCI_WR_ENC_KEY_LENGTH,
                                     sizeof( bt_cmd_hc_wr_enc_key_length_type ),
    BT_CMD_HC_WR_ENC_KEY_LENGTH,      TRUE, BT_CMD_FIXED_LEN },

  { BT_HCI_CHANGE_REMOTE_PKT_TYPE,
                                sizeof( bt_cmd_hc_change_remote_pkt_type_type ),
    BT_CMD_HC_CHANGE_REMOTE_PKT_TYPE, TRUE, BT_CMD_FIXED_LEN },

  { BT_HCI_STATUS_VIEW_EN,            sizeof( bt_cmd_hc_status_view_en_type ),
    BT_CMD_HC_STATUS_VIEW_EN,         TRUE, BT_CMD_FIXED_LEN },

  { BT_HCI_RD_SBI_REG,                sizeof( bt_cmd_hc_rd_sbi_reg_type ),
    BT_CMD_HC_RD_SBI_REG,             FALSE, BT_CMD_FIXED_LEN },

  { BT_HCI_WR_SBI_REG,                sizeof( bt_cmd_hc_wr_sbi_reg_type ),
    BT_CMD_HC_WR_SBI_REG,             TRUE, BT_CMD_FIXED_LEN },

  { BT_HCI_ENABLE_LOOPBACK,           sizeof( bt_cmd_hc_enable_loopback_type ),
    BT_CMD_HC_ENABLE_LOOPBACK,        TRUE, BT_CMD_FIXED_LEN },

  { BT_HCI_SET_INCOMING_HOLD_PARAMS,
                              sizeof( bt_cmd_hc_set_incoming_hold_params_type ),
    BT_CMD_HC_SET_INCOMING_HOLD_PARAMS,TRUE, BT_CMD_FIXED_LEN },

  { BT_HCI_SET_INCOMING_SNIFF_PARAMS,
                             sizeof( bt_cmd_hc_set_incoming_sniff_params_type ),
    BT_CMD_HC_SET_INCOMING_SNIFF_PARAMS, TRUE, BT_CMD_FIXED_LEN },

  { BT_HCI_SET_INCOMING_PARK_PARAMS,
                              sizeof( bt_cmd_hc_set_incoming_park_params_type ),
    BT_CMD_HC_SET_INCOMING_PARK_PARAMS,  TRUE, BT_CMD_FIXED_LEN },

  { BT_HCI_SET_TX_POWER_LEVEL,
                                    sizeof( bt_cmd_hc_set_tx_power_level_type ),
    BT_CMD_HC_SET_TX_POWER_LEVEL,  TRUE, BT_CMD_FIXED_LEN },

  { BT_HCI_CERT_CTRL,                 sizeof( bt_cmd_hc_cert_ctrl_type ),
    BT_CMD_HC_CERT_CTRL,              TRUE, BT_CMD_FIXED_LEN },

  { BT_HCI_TX_CONTINUOUS,             sizeof( bt_cmd_hc_tx_continuous_type ),
    BT_CMD_HC_TX_CONTINUOUS,          TRUE, BT_CMD_FIXED_LEN },

  { BT_HCI_TX_CONTINUOUS_FREQ,
    sizeof( bt_cmd_hc_tx_continuous_freq_type ),
    BT_CMD_HC_TX_CONTINUOUS_FREQ,     TRUE, BT_CMD_FIXED_LEN },

  { BT_HCI_RD_RAW_RSSI,               sizeof( bt_cmd_hc_rd_raw_rssi_type ),
    BT_CMD_HC_RD_RAW_RSSI,            TRUE, BT_CMD_FIXED_LEN },

  { BT_HCI_TEST_SET_LM_PARAMS,
    sizeof( bt_cmd_hc_test_set_lm_params_type ),
    BT_CMD_HC_TEST_SET_LM_PARAMS,     TRUE, BT_CMD_FIXED_LEN },

  { BT_HCI_TEST_SEND_LMP_MSG,
    sizeof( bt_cmd_hc_test_send_lmp_msg_type ),
    BT_CMD_HC_TEST_SEND_LMP_MSG,      TRUE, BT_CMD_FIXED_LEN },

  { BT_HCI_WR_RAM,                    sizeof( bt_cmd_hc_wr_ram_type ),
    BT_CMD_HC_WR_RAM,                 TRUE, BT_CMD_FIXED_LEN },

  { BT_HCI_RD_RAM,                    sizeof( bt_cmd_hc_rd_ram_type ),
    BT_CMD_HC_RD_RAM,                 TRUE, BT_CMD_FIXED_LEN}

#ifdef FEATURE_BT_1_2
 ,{ BT_HCI_CONTROL_AFH,               sizeof( bt_cmd_hc_control_afh_type ),
    BT_CMD_HC_CONTROL_AFH,            TRUE, BT_CMD_FIXED_LEN },

  { BT_HCI_SET_AFH_CHAN_GOOD,
                                     sizeof( bt_cmd_hc_set_afh_chan_good_type ),
    BT_CMD_HC_SET_AFH_CHAN_GOOD,      TRUE, BT_CMD_FIXED_LEN },

  { BT_HCI_SET_ESCO_PKT_LEN,          sizeof( bt_cmd_hc_set_esco_pkt_len_type ),
    BT_CMD_HC_SET_ESCO_PKT_LEN,       TRUE, BT_CMD_FIXED_LEN }
#endif /* FEATURE_BT_1_2 */
#endif /* !FEATURE_BT_SOC */

#ifdef FEATURE_BT_1_2
  /* BT 1.2 AFH and eSCO related commands */
 ,{ BT_HCI_SET_AFH_HOST_CHAN_CLASS,
                               sizeof( bt_cmd_hc_set_afh_host_chan_class_type ),
   BT_CMD_HC_SET_AFH_HOST_CHAN_CLASS, TRUE, BT_CMD_FIXED_LEN },

  { BT_HCI_READ_AFH_CHAN_ASSESS_MODE,    BT_HCI_CMD_NO_PARAM_LEN,
    BT_CMD_HC_READ_AFH_CHAN_ASSESS_MODE, TRUE, BT_CMD_FIXED_LEN },

  { BT_HCI_SET_AFH_CHAN_ASSESS_MODE,
                              sizeof( bt_cmd_hc_set_afh_chan_assess_mode_type ),
    BT_CMD_HC_SET_AFH_CHAN_ASSESS_MODE,  TRUE, BT_CMD_FIXED_LEN },

  { BT_HCI_READ_AFH_CHAN_MAP,        sizeof( bt_cmd_hc_read_afh_chan_map_type ),
    BT_CMD_HC_READ_AFH_CHAN_MAP,     TRUE, BT_CMD_FIXED_LEN },

  { BT_HCI_SETUP_SYNC_CONNECTION,
                                 sizeof( bt_cmd_hc_setup_sync_connection_type ),
    BT_CMD_HC_SETUP_SYNC_CONNECTION, FALSE, BT_CMD_FIXED_LEN },

  { BT_HCI_ACCEPT_SYNC_CONN_REQ,
                                  sizeof( bt_cmd_hc_accept_sync_conn_req_type ),
    BT_CMD_HC_ACCEPT_SYNC_CONN_REQ,   FALSE, BT_CMD_FIXED_LEN },

  { BT_HCI_REJECT_SYNC_CONN_REQ,
                                  sizeof( bt_cmd_hc_reject_sync_conn_req_type ),
    BT_CMD_HC_REJECT_SYNC_CONN_REQ,   FALSE, BT_CMD_FIXED_LEN },

  { BT_HCI_RD_INQUIRY_SCAN_TYPE,      BT_HCI_CMD_NO_PARAM_LEN,
    BT_CMD_HC_RD_INQUIRY_SCAN_TYPE,   TRUE, BT_CMD_FIXED_LEN },

  { BT_HCI_WR_INQUIRY_SCAN_TYPE,      sizeof( bt_cmd_hc_wr_is_type_type ),
    BT_CMD_HC_WR_INQUIRY_SCAN_TYPE,   TRUE, BT_CMD_FIXED_LEN },

  { BT_HCI_RD_INQUIRY_MODE,           BT_HCI_CMD_NO_PARAM_LEN,
    BT_CMD_HC_RD_INQUIRY_MODE,        TRUE, BT_CMD_FIXED_LEN },

  { BT_HCI_WR_INQUIRY_MODE,           sizeof( bt_cmd_hc_wr_inq_mode_type ),
    BT_CMD_HC_WR_INQUIRY_MODE,        TRUE, BT_CMD_FIXED_LEN },

  { BT_HCI_RD_PAGE_SCAN_TYPE,         BT_HCI_CMD_NO_PARAM_LEN,
    BT_CMD_HC_RD_PAGE_SCAN_TYPE,      TRUE, BT_CMD_FIXED_LEN },

  { BT_HCI_WR_PAGE_SCAN_TYPE,         sizeof( bt_cmd_hc_wr_ps_type_type ),
    BT_CMD_HC_WR_PAGE_SCAN_TYPE,      TRUE, BT_CMD_FIXED_LEN }
#endif /* FEATURE_BT_1_2 */

#ifdef BT_SWDEV_2_1_PB
  ,{ BT_HCI_ENH_FLUSH,                 sizeof( bt_cmd_hc_enh_flush_type ),
    BT_CMD_HC_ENH_FLUSH,              TRUE, BT_CMD_FIXED_LEN }
#endif /* BT_SWDEV_2_1_PB */

#ifdef FEATURE_BT_SOC1
#error code not present
#endif /* FEATURE_BT_SOC1 */

/* opcode is a don't care for this entry. Special handling in cmd          */
/* processing logic picks the opcode from the opcode param in              */
/* cmd_hc_vs_any structure                                                 */
/* Size field setting is such that only the first item in the cmd which is */
/* dsm_ptr, will be copied to HCI payload.                                 */
  ,{ BT_HCI_VS_GROUP,                 sizeof(dsm_item_type*),
     BT_CMD_HC_VS_ANY,                TRUE, BT_CMD_VAR_LENGTH}

#ifdef BT_SWDEV_2_1_EIR
   ,{ BT_HCI_RD_EXT_INQ_RSP,         BT_HCI_CMD_NO_PARAM_LEN,
     BT_CMD_HC_RD_EXT_INQ_RSP,       TRUE, BT_CMD_VAR_LENGTH },

   { BT_HCI_WR_EXT_INQ_RSP,          sizeof(bt_cmd_hc_wr_eir_type),
     BT_CMD_HC_WR_EXT_INQ_RSP,       TRUE, BT_CMD_VAR_LENGTH },

   { BT_HCI_RD_INQ_RSP_TX_PWR,       BT_HCI_CMD_NO_PARAM_LEN,
     BT_CMD_HC_RD_INQ_RSP_TX_PWR,    TRUE, BT_CMD_FIXED_LEN },

   { BT_HCI_WR_INQ_TX_PWR,           sizeof(bt_cmd_hc_wr_inq_tx_pwr_type),
     BT_CMD_HC_WR_INQ_TX_PWR,        TRUE, BT_CMD_FIXED_LEN }
#endif /* BT_SWDEV_2_1_EIR */
};


/* Table of HCI events to be received by Host from Controller */
const bt_hc_drv_ev_struct_type  bt_hc_drv_ev_table[] =
{
  { BT_EV_HC_RX_ACL_DATA,                 BT_HCI_MAX_EV,
    sizeof( bt_ev_hc_rx_acl_data_type ),  BT_EV_VAR_LENGTH},

  { BT_EV_HC_INQUIRY_COMPLETE,            BT_HCI_INQUIRY_COMPLETE_EV,
    sizeof( bt_ev_hc_inquiry_cmplt_type ),BT_EV_FIXED_LENGTH },

  { BT_EV_HC_INQUIRY_RESULT,              BT_HCI_INQUIRY_RESULT_EV,
    sizeof( bt_ev_hc_inquiry_result_type ), BT_EV_VAR_LENGTH },

#ifdef BT_SWDEV_2_1_EIR
  { BT_EV_HC_INQUIRY_RESULT_EXTENDED,     BT_HCI_INQUIRY_RESULT_EXTENDED_EV,
    sizeof( bt_ev_hc_inquiry_result_extended_type ), BT_EV_VAR_LENGTH },
#endif /* BT_SWDEV_2_1_EIR */

  { BT_EV_HC_CONNECTION_COMPLETE,         BT_HCI_CONNECTION_COMPLETE_EV,
    sizeof( bt_ev_hc_conn_cmplt_type ),   BT_EV_FIXED_LENGTH },

  { BT_EV_HC_CONNECTION_REQUEST,          BT_HCI_CONNECTION_REQUEST_EV,
    sizeof( bt_ev_hc_conn_request_type ), BT_EV_FIXED_LENGTH },

  { BT_EV_HC_DISCONNECTION_COMPLETE,      BT_HCI_DISCONNECTION_COMPLETE_EV,
    sizeof( bt_ev_hc_disc_cmplt_type ),   BT_EV_FIXED_LENGTH },

  { BT_EV_HC_AUTH_COMPLETE,               BT_HCI_AUTHENTICATION_COMPLETE_EV,
    sizeof( bt_ev_hc_auth_cmplt_type ),   BT_EV_FIXED_LENGTH },

  { BT_EV_HC_REMOTE_NAME_REQ_COMPLETE,    BT_HCI_REMOTE_NAME_RQST_COMPLETE_EV,
    sizeof( bt_ev_hc_remote_name_req_cmplt_type ), BT_EV_VAR_LENGTH },

  { BT_EV_HC_ENCRYPTION_CHANGE,           BT_HCI_ENCRYPTION_CHANGE_EV,
    sizeof( bt_ev_hc_encrypt_change_type),BT_EV_FIXED_LENGTH },

  { BT_EV_HC_CHANGE_CONN_KEY_COMPLETE,
                                  BT_HCI_CHANGE_CONNECTION_LINK_KEY_COMPLETE_EV,
    sizeof( bt_ev_hc_chng_conn_key_cmplt_type ), BT_EV_FIXED_LENGTH },

  { BT_EV_HC_MASTER_LINK_KEY_COMPLETE,    BT_HCI_MASTER_LINK_KEY_COMPLETE_EV,
    sizeof( bt_ev_hc_master_key_cmplt_type ), BT_EV_FIXED_LENGTH },

  { BT_EV_HC_RD_REMOTE_FEATURES_COMPLETE,
                                  BT_HCI_READ_REMOTE_SUPPORTED_FEAT_COMPLETE_EV,
    sizeof( bt_ev_hc_rd_remote_feat_cmplt_type ), BT_EV_FIXED_LENGTH },

  { BT_EV_HC_RD_REMOTE_VERSION_COMPLETE,
                                    BT_HCI_READ_REMOTE_VERSION_INFO_COMPLETE_EV,
    sizeof( bt_ev_hc_rd_remote_ver_cmplt_type ), BT_EV_FIXED_LENGTH },

  { BT_EV_HC_QOS_SETUP_COMPLETE,          BT_HCI_QOS_SETUP_COMPLETE_EV,
    sizeof( bt_ev_hc_qos_setup_cmplt_type ), BT_EV_FIXED_LENGTH },

  { BT_EV_HC_COMMAND_COMPLETE,            BT_HCI_COMMAND_COMPLETE_EV,
    sizeof( bt_ev_hc_cmd_cmplt_type ),    BT_EV_FIXED_LENGTH },

  { BT_EV_HC_COMMAND_STATUS,              BT_HCI_COMMAND_STATUS_EV,
     sizeof( bt_ev_hc_cmd_status_type ),  BT_EV_FIXED_LENGTH },

  { BT_EV_HC_HARDWARE_ERROR,              BT_HCI_HW_ERROR_EV,
    sizeof( bt_ev_hc_hw_error_type ),     BT_EV_FIXED_LENGTH },

  { BT_EV_HC_FLUSH_OCCURRED,              BT_HCI_FLUSH_OCCURRED_EV,
    sizeof( bt_ev_hc_flush_occurred_type),BT_EV_FIXED_LENGTH },

  { BT_EV_HC_ROLE_CHANGE,                 BT_HCI_ROLE_CHANGE_EV,
    sizeof( bt_ev_hc_role_change_type ),  BT_EV_FIXED_LENGTH },

  { BT_EV_HC_NUM_CMPLT_PKTS,              BT_HCI_NUM_COMPLETED_PACKETS_EV,
    sizeof(bt_ev_hc_num_cmplt_pkts_type), BT_EV_VAR_LENGTH },

  { BT_EV_HC_MODE_CHANGE,                 BT_HCI_MODE_CHANGE_EV,
    sizeof( bt_ev_hc_mode_change_type ),  BT_EV_FIXED_LENGTH },

  { BT_EV_HC_RETURN_LINK_KEYS,            BT_HCI_RETURN_LINK_KEYS_EV,
    sizeof( bt_ev_hc_ret_key_type ),      BT_EV_VAR_LENGTH },

  { BT_EV_HC_PIN_CODE_REQUEST,            BT_HCI_PIN_CODE_REQUEST_EV,
    sizeof( bt_ev_hc_pin_request_type ),  BT_EV_FIXED_LENGTH },

  { BT_EV_HC_LINK_KEY_REQUEST,            BT_HCI_LINK_KEY_REQUEST_EV,
    sizeof( bt_ev_hc_key_request_type ),  BT_EV_FIXED_LENGTH },

  { BT_EV_HC_LINK_KEY_NOTIFICATION,       BT_HCI_LINK_KEY_NOTIFICATION_EV,
    sizeof( bt_ev_hc_key_notif_type ),    BT_EV_FIXED_LENGTH },

  { BT_EV_HC_LOOPBACK_COMMAND,            BT_HCI_LOOPBACK_COMMAND_EV,
    sizeof( bt_ev_hc_loopback_cmd_type ), BT_EV_VAR_LENGTH },

  { BT_EV_HC_DATA_BUFFER_OVERFLOW,        BT_HCI_DATA_BUFFER_OVERFLOW_EV,
    sizeof( bt_ev_hc_data_overflow_type ),BT_EV_FIXED_LENGTH },

  { BT_EV_HC_MAX_SLOTS_CHANGE,            BT_HCI_MAX_SLOTS_CHANGE_EV,
    sizeof( bt_ev_hc_max_slots_change_type ), BT_EV_FIXED_LENGTH },

  { BT_EV_HC_RD_CLK_OFFSET_COMPLETE,      BT_HCI_READ_CLOCK_OFFSET_COMPLETE_EV,
    sizeof( bt_ev_hc_rd_clk_offset_cmplt_type ), BT_EV_FIXED_LENGTH },

  { BT_EV_HC_CONN_PKT_TYPE_CHANGED,       BT_HCI_CONNECTION_PKT_TYPE_CHANGED_EV,
    sizeof( bt_ev_hc_conn_pkt_type_chng_type ), BT_EV_FIXED_LENGTH },

  { BT_EV_HC_QOS_VIOLATION,               BT_HCI_QOS_VIOLATION_EV,
    sizeof( bt_ev_hc_qos_violation_type ),BT_EV_FIXED_LENGTH },

#ifndef FEATURE_BT_1_2 /* BT 1.1 */
  { BT_EV_HC_PAGE_SCAN_MODE_CHANGE,       BT_HCI_PAGE_SCAN_MODE_CHANGE_EV,
    sizeof( bt_ev_hc_ps_mode_chng_type ), BT_EV_FIXED_LENGTH },
#endif

  { BT_EV_HC_PAGE_SCAN_REP_MODE_CHANGE,   BT_HCI_PAGE_SCAN_REP_MODE_CHANGE_EV,
    sizeof( bt_ev_hc_ps_rep_mode_chng_type ), BT_EV_FIXED_LENGTH }

#ifdef FEATURE_BT_SOC1
#error code not present
#elif (defined FEATURE_BT_QSOC)
  ,{ BT_EV_HC_VENDOR_SPECIFIC,             BT_HCI_VENDOR_SPECIFIC,
    sizeof( bt_ev_hc_vs_var_type ),        BT_EV_VAR_LENGTH }
#else
  ,{ BT_EV_HC_VENDOR_SPECIFIC,             BT_HCI_VENDOR_SPECIFIC,
    sizeof( bt_ev_hc_qc_rd_rx_status_result_type ), BT_EV_VAR_LENGTH }
#endif /* FEATURE_BT_SOC1 */


#ifdef FEATURE_BT_1_2
  ,{ BT_EV_HC_INQUIRY_RESULT_WITH_RSSI,
                                         BT_HCI_INQUIRY_RESULT_WITH_RSSI_EV,
    sizeof( bt_ev_hc_inquiry_result_with_rssi_type ), BT_EV_VAR_LENGTH },

   { BT_EV_HC_RD_REMOTE_EXT_FEATURES_COMPLETE,
                                        BT_HCI_READ_REMOTE_EXT_FEAT_COMPLETE_EV,
   sizeof( bt_ev_hc_rd_remote_ext_feat_cmplt_type ), BT_EV_FIXED_LENGTH },

   { BT_EV_HC_SYNC_CONN_COMPLETE,
                                            BT_HCI_SYNC_CONNNECTION_COMPLETE_EV,
    sizeof( bt_ev_hc_sync_conn_cmplt_type ), BT_EV_FIXED_LENGTH },

   { BT_EV_HC_SYNC_CONN_CHANGE,                BT_HCI_SYNC_CONNECTION_CHANGE_EV,
    sizeof( bt_ev_hc_sync_conn_change_type ), BT_EV_FIXED_LENGTH },
#endif /* FEATURE_BT_1_2 */
#ifdef BT_SWDEV_2_1_PB
  { BT_EV_HC_ENH_FLUSH_COMPLETE,              BT_HCI_ENH_FLUSH_COMPLETE_EV,
    sizeof( bt_ev_hc_enh_flush_cmplt_type ), BT_EV_FIXED_LENGTH }
#endif /* BT_SWDEV_2_1_PB */

#ifdef BT_SWDEV_2_1_SSP
 ,{ BT_EV_HC_ENCRYPTION_KEY_REFRESH_COMPLETE,
                                     BT_HCI_ENCRYPTION_KEY_REFRESH_COMPLETE_EV,
   sizeof( bt_ev_hc_enc_key_ref_complt_type ), BT_EV_FIXED_LENGTH },

  { BT_EV_HC_IO_CAPABILITY_REQUEST,         BT_HCI_IO_CAPABILITY_REQUEST_EV,
    sizeof( bt_ev_hc_io_cap_rqst_type ), BT_EV_FIXED_LENGTH },

  { BT_EV_HC_IO_CAPABILITY_RESPONSE,        BT_HCI_IO_CAPABILITY_RESPONSE_EV,
    sizeof( bt_ev_hc_io_cap_res_type ), BT_EV_FIXED_LENGTH },

  { BT_EV_HC_USER_CONFIRMATION_REQUEST,
                                           BT_HCI_USER_CONFIRMATION_REQUEST_EV,
    sizeof( bt_ev_hc_user_cfmtn_rqst_type ), BT_EV_FIXED_LENGTH },

  { BT_EV_HC_USER_PASSKEY_REQUEST,          BT_HCI_USER_PASSKEY_REQUEST_EV,
    sizeof( bt_ev_hc_user_passkey_rqst_type ), BT_EV_FIXED_LENGTH },

  { BT_EV_HC_REMOTE_OOB_DATA_REQUEST,       BT_HCI_REMOTE_OOB_DATA_REQUEST_EV,
    sizeof( bt_ev_hc_remote_oob_data_rqst_type ), BT_EV_FIXED_LENGTH },

  { BT_EV_HC_SIMPLE_PAIRING_COMPLETE,       BT_HCI_SIMPLE_PAIRING_COMPLETE_EV,
    sizeof( bt_ev_hc_sp_complt_type ), BT_EV_FIXED_LENGTH },

  { BT_EV_HC_USER_PASSKEY_NOTIFICATION,
                                           BT_HCI_USER_PASSKEY_NOTIFICATION_EV,
    sizeof( bt_ev_hc_user_passkey_ntfctn_type ), BT_EV_FIXED_LENGTH },

  {  BT_EV_HC_KEYPRESS_NOTIFICATION,        BT_HCI_KEYPRESS_NOTIFICATION_EV,
    sizeof( bt_ev_hc_keypress_ntfctn_type ), BT_EV_FIXED_LENGTH },

  { BT_EV_HC_REMOTE_HOST_SUPPORTED_FEAT_NTFCTN,
                                   BT_HCI_REMOTE_HOST_SUPPORTED_FEAT_NTFCTN_EV,
    sizeof( bt_ev_hc_rem_host_sup_feat_ntfctn_type ), BT_EV_FIXED_LENGTH }
#endif /* BT_SWDEV_2_1_SSP */

#ifdef BT_SWDEV_2_1_SSR
 ,{ BT_EV_HC_SNIFF_SUBRATING,               BT_HCI_SNIFF_SUBRATING_EV,
    sizeof( bt_ev_hc_sniff_subrating_type ), BT_EV_FIXED_LENGTH }
#endif /* BT_SWDEV_2_1_SSR */
#ifdef FEATURE_BT_2_1
 ,{ BT_EV_HC_LINK_SUPERVISION_TIMEOUT_CHANGED,
                                  BT_HCI_LINK_SUPERVISION_TIMEOUT_CHANGED_EV,
   sizeof( bt_ev_hc_lk_sup_to_changed_type ), BT_EV_FIXED_LENGTH },
#endif /* FEATURE_BT_2_1 */
   { BT_EV_HC_RX_SCO_DATA,                 BT_HCI_MAX_EV,
     sizeof( bt_ev_hc_rx_sco_data_type ),  BT_EV_VAR_LENGTH},

   { BT_EV_HC_MAX,                         BT_HCI_MAX_EV,
      0,                                   BT_EV_VAR_LENGTH}
};

/*=====================================================================
                                  Variables
======================================================================*/

/* Structure containing all BT Host HCI information */
static bt_hc_drv_struct_type        bt_hc_drv;

/* Watermark identifying data*/
void*  bt_hc_drv_acl_wm_data[BT_MAX_BT_DEV];

#ifdef FEATURE_BT_AVSYNC_SOC
bt_hc_drv_acl_frame_handle_type  bt_hc_drv_acl_frame_handle[
                                              BT_HC_DRV_MAX_ACL_FRAMES];
q_type                           bt_hc_drv_acl_free_frame_handle_q;
#endif /* FEATURE_BT_AVSYNC_SOC */

#ifdef FEATURE_BT_HCI_HOST_FCTL
/* FCTL factor used to determine if the framecount in HCI TX Q is High or Low*/
static uint8 fctl_limiter = 0;
#endif /* FEATURE_BT_HCI_HOST_FCTL */

/*=====================================================================
                         FUNCTION DECLARATIONS FOR MODULE
======================================================================*/

void bt_hc_drv_acl_tx_hi_wm_cb
(
  struct dsm_watermark_type_s *tx_acl_wm_ptr,
  void*  conn_idx_ptr
);
void bt_hc_drv_acl_tx_lo_wm_cb
(
  struct dsm_watermark_type_s *tx_acl_wm_ptr,
  void*  conn_idx_ptr
);

#ifdef FEATURE_BT_HCI_HOST_FCTL
void bt_hc_drv_fctl_small_dsm_cb
(
  dsm_mempool_id_type     dsm_pool_id,
  dsm_mem_level_enum_type dsm_mem_lvl,
  dsm_mem_op_enum_type    dsm_op
);

void bt_hc_drv_fctl_large_dsm_cb
(
  dsm_mempool_id_type     dsm_pool_id,
  dsm_mem_level_enum_type dsm_mem_lvl,
  dsm_mem_op_enum_type    dsm_op
);
#endif /* FEATURE_BT_HCI_HOST_FCTL */

#ifdef BT_HC_DRV_TX_TRACE_ENABLE
void bt_hc_drv_tx_trace
(
  dsm_item_type* dsm_ptr
)
{
  uint16         cnt;
  uint8          byte_val;
  timetick_type  ts;

/*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  ts = timetick_get();

  /* Dump bytes to the debug buffer and update timestamp */
  for  ( cnt = 0; cnt < dsm_length_packet(dsm_ptr); cnt++)
  {
    dsm_peek_byte( dsm_ptr, cnt, &byte_val );

    bt_hc_drv_tx_buf[bt_hc_drv_tx_trace_cntr].val  = byte_val;
    bt_hc_drv_tx_buf[bt_hc_drv_tx_trace_cntr++].ts = ts;

    if ( bt_hc_drv_tx_trace_cntr == BT_HC_DRV_TX_TRACE_BUF_SIZE )
    {
      bt_hc_drv_tx_trace_cntr = 0;
    }
  }

} /* bt_hc_drv_tx_trace */

void bt_hc_drv_rx_trace
(
  dsm_item_type* dsm_ptr
)
{
  uint16         cnt;
  uint8          byte_val;
  timetick_type  ts;

/*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  ts = timetick_get();

  /* Dump bytes to the debug buffer and update timestamp */
  for  ( cnt = 0; cnt < dsm_length_packet(dsm_ptr); cnt++)
  {
    dsm_peek_byte( dsm_ptr, cnt, &byte_val );

    bt_hc_drv_rx_buf[bt_hc_drv_rx_trace_cntr].val  = byte_val;
    bt_hc_drv_rx_buf[bt_hc_drv_rx_trace_cntr++].ts = ts;


    if ( bt_hc_drv_rx_trace_cntr == BT_HC_DRV_RX_TRACE_BUF_SIZE )
    {
      bt_hc_drv_rx_trace_cntr = 0;
    }

   }

} /* bt_hc_drv_rx_trace */


#endif /* BT_HC_DRV_TX_TRACE_ENABLE */


/*=====================================================================
                         Init/Cleanup Functions
======================================================================*/


/*==========================================================================

FUNCTION
  BT_HC_DRV_POWERUP_INIT

DESCRIPTION
  Performs Host HCI layer initializations.

DEPENDENCIES
  Needs to be called only once at BT task start-up.

INPUTS
  None.

RETURN VALUE
  None.

SIDE EFFECTS
  None.

==========================================================================*/
static void bt_hc_drv_powerup_init( void )
{
  /* Convenience pointer to Tx watermark */
  dsm_watermark_type*             tx_wm_ptr;

  /* Convenience pointer to entries in buffer */
  bt_hc_drv_buf_type*             buf_entry_ptr;

  /* Convenience pointer to ACL Tx Struct in bt_hc_drv */
  bt_hc_drv_acl_tx_struct_type*   acl_tx_ptr;

  /* Loop counter for connections */
  uint8                           c_hndl = 0;

  /* Watermark dont_exceed count */
  uint32                          dont_exceed_cnt = BT_HC_DRV_ACL_TX_WM_DONT_EX_CNT;

#ifdef FEATURE_BT_HCI_HOST_FCTL
  /* Floored int val of the FCTL limiting factor for framecount in the TX Q*/
  uint16                          fact_floor;
  /* Total buffer length available for TX in the SOC buffers*/
  uint16                          tot_hc_tx_len;
#endif /* FEATURE_BT_HCI_HOST_FCTL */

#ifdef FEATURE_BT_AVSYNC_SOC
  /* Loop Counter for initializing ACL frame header Queue */
  uint8                           idx;
#endif /* FEATURE_BT_AVSYNC_SOC */
/*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/
  /* Controller name */
#ifdef FEATURE_BT_SOC1
#error code not present
#elif (defined FEATURE_BT_QSOC)
  bt_hc_drv.ctrlr_info.ctrlr_type = QC_BTS_SOC;
#endif /* FEATURE_BT_SOC1 */

  /* HCI states */
  bt_hc_drv.host_info.hci_tx.hci_tx_state = BT_HC_DRV_INIT_STATE;
  bt_hc_drv.host_info.hci_rx.hci_rx_state = BT_HC_DRV_INIT_STATE;
  bt_hc_drv.host_info.hci_rx.hc_rx_cnt    = 0;
  bt_hc_drv.hci_sync_state                = TRUE;
  bt_hc_drv.host_info.hci_rx.rx_pkt_state.rx_ev_pkt_state =
    BT_HCI_EV_EVENT_CODE_STATE;

  /* Needs to be updated later with values from Read_Buffer_Size command */
  bt_hc_drv.ctrlr_info.cur_buf.acl_data_pkt_len   =
    BT_HC_DRV_MAX_ACL_DATA_PKT_LEN;

  bt_hc_drv.ctrlr_info.cur_buf.sco_data_pkt_len   =
    BT_HC_DRV_MAX_SCO_DATA_PKT_LEN;

  bt_hc_drv.ctrlr_info.cur_buf.tot_free_data_pkts =
    BT_HC_DRV_TOT_NUM_ACL_DATA_PKTS;

  /* Initalize the HCI command Queue and HCI Tx Watermark */
  dsm_queue_init( &bt_hc_drv.host_info.hci_tx.cmd_tx.hci_cmd_tx_wm, 
                  BT_HC_DRV_MAX_HCI_CMD_Q_SIZE, 
                  &bt_hc_drv.host_info.hci_tx.cmd_tx.hci_cmd_q );

  tx_wm_ptr = &bt_hc_drv.host_info.hci_tx.cmd_tx.hci_cmd_tx_wm;
  tx_wm_ptr->lo_watermark           = BT_HC_DRV_MAX_HCI_CMD_Q_SIZE;
  tx_wm_ptr->hi_watermark           = BT_HC_DRV_MAX_HCI_CMD_Q_SIZE;

  /* Initially only one command can be sent to the Controller */
  bt_hc_drv.host_info.hci_tx.cmd_tx.num_hci_cmd_pkts = 1;
#ifdef BT_SWDEV_CMD_FLOW_WORKAROUND
  /* Initially set expecting large evt FALSE */
  bt_hc_drv.host_info.hci_tx.cmd_tx.expecting_large_evt = FALSE;
#endif

#ifdef FEATURE_BT_AVSYNC_SOC
  /* Initialize the free frame handle queue used for AV-Sync */
  ( void ) q_init(&bt_hc_drv_acl_free_frame_handle_q);
  for ( idx = 0; idx < BT_HC_DRV_MAX_ACL_FRAMES; idx++ )
  {
    q_link_type*     link_item;
    link_item = q_link( &bt_hc_drv_acl_frame_handle[idx],
                        &bt_hc_drv_acl_frame_handle[idx].link );
    q_put (&bt_hc_drv_acl_free_frame_handle_q, link_item);
  }
#endif /* FEATURE_BT_AVSYNC_SOC */

#ifdef FEATURE_BT_HCI_HOST_FCTL
  /* ceil(val): Rounding off the calculated factor for the FCTL limit upwards
     to an integer value
     fctl_limiter = ceil( BT_HC_DRV_FCTL_MAX_PROT_PKT_SIZE/
                         (BT_QSOC_MAX_HC_NUM_TX_BUF*BT_QSOC_MAX_HC_TX_LEN) )*/

  tot_hc_tx_len = bt_soc_tot_hc_tx_len();

  fact_floor  = BT_HC_DRV_FCTL_MAX_PROT_PKT_SIZE/tot_hc_tx_len;

  fctl_limiter = fact_floor +
    ( ( BT_HC_DRV_FCTL_MAX_PROT_PKT_SIZE - (fact_floor*tot_hc_tx_len)  )?1:0 );

  /* Space between high water mark and "Don't exceed count" should be good 
     enough to accomodate max chunck from protocol layer ( RFCOMM\A2DP etc ).
     The number 32 is derived from the fact that RFCOMM can pump at the max 5104
     bytes (around 15 HCI packets). */

  dont_exceed_cnt = 
   ( ( BT_HC_DRV_ACL_TX_HIGH_FR_CNT(BT_QSOC_MAX_HC_NUM_TX_BUF, fctl_limiter) +
     BT_HC_DRV_FCTL_OVERFLOW_CNT ) * 
     ( BT_MTP_MAX_HC_LEN + sizeof(bt_hc_cmd_hdr_type)) );


  /* Register callbacks that will be invoked when the DSM pool goes High/Low */
  dsm_reg_mem_event_cb( DSM_DS_SMALL_ITEM_POOL, DSM_MEM_LEVEL_MANY,
    DSM_MEM_OP_FREE, bt_hc_drv_fctl_small_dsm_cb );

  dsm_reg_mem_event_cb( DSM_DS_SMALL_ITEM_POOL, DSM_MEM_LEVEL_FEW,
    DSM_MEM_OP_NEW, bt_hc_drv_fctl_small_dsm_cb );

  dsm_reg_mem_event_cb( DSM_DS_LARGE_ITEM_POOL, DSM_MEM_LEVEL_MANY,
    DSM_MEM_OP_FREE, bt_hc_drv_fctl_large_dsm_cb );

  dsm_reg_mem_event_cb( DSM_DS_LARGE_ITEM_POOL, DSM_MEM_LEVEL_FEW,
    DSM_MEM_OP_NEW, bt_hc_drv_fctl_large_dsm_cb );
#endif /* FEATURE_BT_HCI_HOST_FCTL */


  /* Initialize Tx data watermarks and queues for each connection handle */
  for ( c_hndl = 0; c_hndl < BT_MAX_BT_DEV; c_hndl++ )
  {
#ifdef FEATURE_BT_HCI_HOST_FCTL
    bt_hc_drv.host_info.hci_rx.hc_rx_acl_cnt[ c_hndl ] = 0;
    bt_hc_drv.host_info.hci_rx.rx_acl_flow[ c_hndl ]   = TRUE;
#endif /* FEATURE_BT_HCI_HOST_FCTL */

    acl_tx_ptr = &bt_hc_drv.host_info.hci_tx.acl_tx[ c_hndl ];
    dsm_queue_init( &acl_tx_ptr->acl_data_tx_wm, 
                    dont_exceed_cnt,
                    &acl_tx_ptr->acl_data_tx_q );

    tx_wm_ptr                         = &acl_tx_ptr->acl_data_tx_wm;
    tx_wm_ptr->lo_watermark           = BT_HC_DRV_ACL_TX_WM_LO_VAL;
    tx_wm_ptr->hi_watermark           = BT_HC_DRV_ACL_TX_WM_HI_VAL;

#ifndef FEATURE_BT_HCI_HOST_FCTL
    bt_hc_drv_acl_wm_data[c_hndl]     = (void *) c_hndl;
    tx_wm_ptr->hiwater_func_ptr       = (wm_cb_type)bt_hc_drv_acl_tx_hi_wm_cb;
    tx_wm_ptr->hiwater_func_data      = (void *)&bt_hc_drv_acl_wm_data[c_hndl];
    tx_wm_ptr->lowater_func_ptr       = (wm_cb_type)bt_hc_drv_acl_tx_lo_wm_cb;
    tx_wm_ptr->lowater_func_data      = (void *)&bt_hc_drv_acl_wm_data[c_hndl];
#endif /* FEATURE_BT_HCI_HOST_FCTL */

    /* Convenience pointer to ACL buffer entry for given connection handle */
    buf_entry_ptr    = &bt_hc_drv.ctrlr_info.cur_buf.entries[c_hndl];

    /* Initialze Connection handles */
    buf_entry_ptr->conn_hndl          = BT_HC_DRV_ACL_TX_INIT_CHNDL;

    /* Initialize number of free ACL data buffer entries to requested,
       allocated, used */
    buf_entry_ptr->num_buf_used       = 0;
    buf_entry_ptr->num_buf_req        = 0;
    buf_entry_ptr->num_alloc_buf      = 0;
    bt_hc_drv.host_info.hci_tx.acl_tx[c_hndl].flow_on = TRUE;

#ifdef FEATURE_BT_AVSYNC_SOC
    /* Initialize the frame handle queues used for AV-Sync */
    ( void ) q_init( &acl_tx_ptr->acl_data_tx_frame_handle_q );
     acl_tx_ptr->num_non_flushable_pkts = 0;
     acl_tx_ptr->num_unacked_pkts       = 0;
     acl_tx_ptr->buffer_flushed         = FALSE;
     acl_tx_ptr->flush_to               = BT_L2FLUSH_MAX_TIME;
#endif /* FEATURE_BT_AVSYNC_SOC */

     BT_MSG_HIGH( "BT HC DRV: dow: %x hiw: %x low: %x", 
                  tx_wm_ptr->dont_exceed_cnt, tx_wm_ptr->lo_watermark, 
                  tx_wm_ptr->hi_watermark);
  } /* for c_hndl */

} /* bt_hc_drv_powerup_init */


/*===========================================================================

FUNCTION
  BT_HC_DRV_BACKGROUND_TICK

DESCRIPTION
  Performs background monitoring for HCI.

DEPENDENCIES
  None.

INPUTS
  None.

RETURN VALUE
  None.

SIDE EFFECTS
  None.

===========================================================================*/
void bt_hc_drv_background_tick( void )
{

/*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  /* Process the SOC timeout event */
#ifdef FEATURE_BT_SOC1
#error code not present
#endif /* FEATURE_BT_SOC1 */


} /* bt_hc_drv_background_tick */


/*===========================================================================

FUNCTION
  BT_HC_DRV_SOC_INIT

DESCRIPTION
  Initializes the SOC Driver

DEPENDENCIES
  None.

INPUTS
  None.

RETURN VALUE
  None.

SIDE EFFECTS
  None.

===========================================================================*/
void bt_hc_drv_soc_init( void )
{

/*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  /* Initialize Host HCI driver layer. */
  bt_hc_drv_powerup_init();

  /* Initialize timers */
  bt_soc_ready_init();

  /*  Initialize the SOC unless there is a ongoing HCI RST in APIT Test mode */
  if ( ( bt_soc_ifc.bt_soc_init_func_ptr != NULL ) &&
       ( bt_hc_pe_is_hci_rst_started() == FALSE  ) )
  {
    bt_soc_ifc.bt_soc_init_func_ptr();
  }

} /* bt_hc_drv_soc_init */


/*===========================================================================

FUNCTION
  BT_HC_DRV_LOG_HOST_RX_EV

DESCRIPTION
  This function logs an HCI event packet received from Host Controller to diag.

DEPENDENCIES
  None.

INPUTS
  hci_pkt_hdr    - HCI Event packet header
  ev_code        - Event OpCode
  ev_par_len     - Length of event parameter fields
  ev_par_dsm_ptr - Pointer to the dsm item holding the HCI event paramaters

RETURN VALUE
 None.

SIDE EFFECTS
  None.

===========================================================================*/
static void bt_hc_drv_log_host_rx_ev
(
  /* HCI Event packet header for UART transport */
  bt_hci_pkt_type hci_pkt_hdr,

  /* Event Opcode */
  uint8           ev_code,

  /* Event parameter length */
  uint8           ev_par_len,

  /* Pointer to the dsm item holding the HCI event paramaters */
  dsm_item_type*  ev_par_dsm_ptr

)
{
  /* Pointer to the log buffer to be sent to diag */
  LOG_BT_HCI_EV_C_type*  log_ptr;

/*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/
  if ( log_status( LOG_BT_HCI_EV_C ))
  {
    /* Log code enabled, fill and send the packet to diag */

    /* Log memory allocation */
    log_ptr = (LOG_BT_HCI_EV_C_type *)log_alloc( LOG_BT_HCI_EV_C,
                 BT_LOG_ALLOC_VAR_SIZ( LOG_BT_HCI_EV_C_type, uint8,
                                       ev_par_len ));

    if ( log_ptr != NULL )
    {
      /* Update event packet header */
      log_ptr->hci_pkt_hdr    = hci_pkt_hdr;

      /* Update HCI event Code */
      log_ptr->hci_event_code = ev_code;

      /* Update HCI event parameter length */
      log_ptr->hci_ev_par_len = ev_par_len;

      /* Extract event parameters from dsm and copy it to the log packet */
      (void) dsm_extract( ev_par_dsm_ptr, BT_FIRST_BYTE_OFFSET,
                          (void *)(log_ptr->hci_ev_par), ev_par_len );

      /* Submit log packet to diag */
      log_commit( log_ptr );

    } /* if ( log_ptr != NULL ) */
  } /* if log_status( LOG_BT_HOST_HCI_RX_C ) */

} /* bt_hc_drv_log_host_rx_ev */


/*===========================================================================

FUNCTION
  BT_HC_DRV_LOG_HOST_RX_ACL_DATA

DESCRIPTION
  This function logs an HCI ACL data packet received from Host Controller to
  diag.

DEPENDENCIES
  None.

INPUTS
  acl_hdr           - Structure that contains the ACL header information
  acl_data_dsm_ptr  - Dsm pointer to ACL data payload

RETURN VALUE
 None.

SIDE EFFECTS
  None.

===========================================================================*/
static void bt_hc_drv_log_host_rx_acl_data
(
  /* ACL packet header */
  bt_hc_acl_data_hdr_type     acl_hdr,

  /* Pointer to the dsm item containing ACL data payload */
  dsm_item_type*              acl_data_dsm_ptr
)
{
  /* Pointer to the log buffer to be sent to diag */
  LOG_BT_HCI_RX_ACL_C_type*  log_ptr;

/*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/
  if ( log_status( LOG_BT_HCI_RX_ACL_C ))
  {
    /* Log code enabled, fill and send the packet to diag */

    /* Log memory allocation */
    log_ptr = (LOG_BT_HCI_RX_ACL_C_type *)log_alloc( LOG_BT_HCI_RX_ACL_C,
                 BT_LOG_ALLOC_VAR_SIZ( LOG_BT_HCI_RX_ACL_C_type, uint8,
                                       acl_hdr.data_len ));

    if ( log_ptr != NULL )
    {
      /* Update HCI packet type as an ACL data packet */
      log_ptr->acl_hdr.pkt_type      = acl_hdr.pkt_type;

      /* Update ACL Conn. Handle, Pkt boundary flag  & Broadcast flag bits */
      log_ptr->acl_hdr.acl_conn_hdr  = acl_hdr.acl_conn_hdr;
     
      /* Update ACL data length */
      log_ptr->acl_hdr.data_len      = acl_hdr.data_len;

      /* Extract event parameters from dsm and copy it to the log buffer */
      (void) dsm_extract( acl_data_dsm_ptr, BT_FIRST_BYTE_OFFSET,
                          (void *)(log_ptr->data_buf),
                          acl_hdr.data_len );

      /* Submit log packet to diag */
      log_commit( log_ptr );
    } /* if ( log_ptr != NULL ) */
  } /* if log_status( LOG_BT_HOST_HCI_RX_C ) */
} /* bt_hc_drv_log_host_rx_acl_data */


/*===========================================================================

FUNCTION
  BT_HC_DRV_LOG_CMD

DESCRIPTION
  This function logs an HCI command packet that is transmitted to
  Host Controller.

DEPENDENCIES
  None.

INPUTS
  cmd_dsm_ptr - Dsm pointer to the HCI command packet
  cmd_pkt_len - Length of the command packet

RETURN VALUE
 None.

SIDE EFFECTS
  None.

===========================================================================*/
static void bt_hc_drv_log_cmd
(
  /* Pointer to the dsm item holding the HCI command packet */
  dsm_item_type*  cmd_dsm_ptr,

  /* Length of the command packet */
  uint8           cmd_pkt_len
)
{
  /* Pointer to the log buffer to be sent to diag */
  LOG_BT_HCI_CMD_C_type*  log_ptr;

/*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  if ( log_status( LOG_BT_HCI_CMD_C))
  {
    /* Log code enabled, fill and send the packet to diag */

    /* Log memory allocation */
    log_ptr = (LOG_BT_HCI_CMD_C_type *)log_alloc( LOG_BT_HCI_CMD_C,
                 BT_LOG_ALLOC_VAR_SIZ( LOG_BT_HCI_CMD_C_type, uint8,
                                       cmd_pkt_len ));

    if ( log_ptr != NULL )
    {
      /* Extract the command packet from dsm and copy it to the log packet */
      (void) dsm_extract( cmd_dsm_ptr, BT_FIRST_BYTE_OFFSET,
                          (void *)(log_ptr->cmd_buffer),
                          cmd_pkt_len );

      /* Submit log packet to diag */
      log_commit( log_ptr );

    } /* if (log_ptr != NULL) */
  } /* if log_status */

} /* bt_hc_drv_log_cmd */


/*===========================================================================

FUNCTION
  BT_HC_DRV_LOG_HOST_TX_DATA

DESCRIPTION
  This function logs an HCI packet transmitted to Host Controller.

DEPENDENCIES
  None.

INPUTS
  acl_dsm_ptr - Pointer to the dsm item holding the entire ACL packet
  acl_pkt_len - Length of the acl packet
RETURN VALUE
 None.

SIDE EFFECTS
  None.

===========================================================================*/
static void bt_hc_drv_log_host_tx_data
(
  /* Pointer to the dsm item holding the HCI command packet */
  dsm_item_type*  acl_dsm_ptr,

  /* Length of the acl packet */
  uint16           acl_pkt_len
)
{
  /* Pointer to the log buffer to be sent to diag */
  LOG_BT_HCI_TX_ACL_C_type*  log_ptr;

/*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  if ( log_status( LOG_BT_HCI_TX_ACL_C))
  {
    /* Log code enabled, fill and send the packet to diag */

    /* Log memory allocation */
    log_ptr = (LOG_BT_HCI_TX_ACL_C_type *)log_alloc( LOG_BT_HCI_TX_ACL_C,
                 BT_LOG_ALLOC_VAR_SIZ( LOG_BT_HCI_TX_ACL_C_type, uint8,
                                       acl_pkt_len ));

    if ( log_ptr != NULL )
    {
      /* Extract the acl packet from dsm and copy it to the log packet */
      (void) dsm_extract( acl_dsm_ptr, BT_FIRST_BYTE_OFFSET,
                          (void *)( log_ptr->acl_buf ),
                          acl_pkt_len );

      /* Submit log packet to diag */
      log_commit( log_ptr );
     } /* if (log_ptr != NULL) */
  } /* if log_status */
} /* bt_hc_drv_log_host_tx_data */


/*==========================================================================

FUNCTION
  BT_HC_DRV_DSM_PULLUP

DESCRIPTION
  This function does a dsm pullup for BT Host HCI module.

DEPENDENCIES
  None.

INPUTS
  **pkt_head_ptr - pointer to dsm pointer of the packet
  *buf           - Pointer to buffer to copy the pulled bytes
  cnt            - Number of bytes to pullup

RETURN VALUE
 Number of bytes read.

SIDE EFFECTS
  None.

==========================================================================*/
uint16 bt_hc_drv_dsm_pullup
(
  /* pointer to dsm pointer of the packet */
  dsm_item_type **pkt_head_ptr,

  /* Pointer to buffer to copy the pulled bytes*/
  void *buf,

  /* Number of bytes to pullup */
  uint16 cnt
)
{
  uint16 num_bytes_read;

/*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/
  /* Try to pullup specified number of bytes */
  num_bytes_read = dsm_pullup( pkt_head_ptr, buf, cnt);

  if (num_bytes_read != cnt )
  {
    BT_MSG_DEBUG( "BT HC DRV: Pulled %d; Req %d bytes",
                  num_bytes_read, cnt, 0 );
  } /* if */

  return( num_bytes_read );
} /* bt_hc_drv_dsm_pullup */


#ifdef FEATURE_BT_HCI_HOST_FCTL
/*==========================================================================

                       BT HOST FLOW CONTROL  -
Functions involved in implementing ACL flow control from controller to host

==========================================================================*/

/*===========================================================================

FUNCTION
  BT_HC_DRV_SEND_HOST_NUM_CMPLT_PKTS_CMD

DESCRIPTION
  Sends the Host_Number_Of_Completed_Packets command to Controller.

DEPENDENCIES
  None.

INPUTS
  None.

RETURN VALUE
  None.

SIDE EFFECTS
  None.

===========================================================================*/
void bt_hc_drv_send_host_num_cmplt_pkts_cmd( void )
{
  uint8 c_hndl;             /* Counter for ACL connections */

  /* Number of connection handles informed in the command */
  uint8 num_conn_hndl = 0;

  /* Pointer to dsm item for host num complt packets command */
  dsm_item_type*          temp_dsm_ptr = NULL;

/*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  if( ( bt_hc_drv.ctrlr_info.cur_buf.num_conn != 0 )
#ifdef FEATURE_BT_HCI_HOST_FCTL
      && ( bt_hc_drv.host_info.is_dsm_cnt_low == FALSE )
#endif /* FEATURE_BT_HCI_HOST_FCTL */
    )
  {
    /* Sufficient dsm items remaining in system, send NumCompltPkts if reqd */

    for( c_hndl = 0; c_hndl < BT_MAX_BT_DEV; c_hndl++ )
    {
      if( (bt_hc_drv.host_info.hci_rx.hc_rx_acl_cnt[ c_hndl ] != 0 ) &&
          (bt_hc_drv.host_info.hci_rx.rx_acl_flow[ c_hndl ] == TRUE ))
      {
        /* NumCmpltPkts needs to be sent for this ConnectionHandle. Also the
           connection has not been flowed off on Tx side */

        if( temp_dsm_ptr == NULL )
        {
          temp_dsm_ptr = bt_get_free_dsm_ptr( BT_TL_HCI_BB,
                                              BT_CMD_HC_HOST_NUM_CMPLT_PKTS_MAX_SIZE);
        } /* if temp_dsm_ptr == NULL */

        (void)bt_dsm_pushdown_tail( &temp_dsm_ptr,
                                    &bt_hc_drv.ctrlr_info.cur_buf.entries[c_hndl].conn_hndl,
                                    sizeof( uint16 ),
                                    DSM_DS_SMALL_ITEM_POOL );

        (void)bt_dsm_pushdown_tail( &temp_dsm_ptr,
                                    &bt_hc_drv.host_info.hci_rx.hc_rx_acl_cnt[ c_hndl ],
                                    sizeof( uint16 ),
                                    DSM_DS_SMALL_ITEM_POOL );

        ++num_conn_hndl;
        bt_hc_drv.host_info.hci_rx.hc_rx_acl_cnt[ c_hndl ] = 0;
        ASSERT( bt_hc_drv.host_info.hci_rx.hc_rx_acl_cnt[ c_hndl ] <=
                BT_HC_HOST_TOT_NUM_ACL_DATA_PKTS );
      } /* if hc_rx_acl_cnt !=0 && flow_on == TRUE */
    } /* for c_hndl */

    if( temp_dsm_ptr != NULL )
    {
      bt_cmd_hc_host_num_cmplt_pkt( num_conn_hndl, temp_dsm_ptr );
    } /* if temp_dsm_ptr == NULL */
  } /* num_conn != 0 && is_dsm_cnt_low == FALSE */

} /* bt_hc_drv_send_host_num_cmplt_pkts_cmd */


/*==========================================================================

FUNCTION
  BT_HC_DRV_HC2H_FLOW_SUPPORT_STATUS

DESCRIPTION
  This functions updates whether the Controller supports the Host Controller
  to Host flow control.

DEPENDENCIES
  None.

INPUTS
  HC2H flow control supported status

RETURN VALUE
  None.

SIDE EFFECTS
  None.

==========================================================================*/

void bt_hc_drv_hc2h_flow_support_status
(
  /* HC2H flow control supported status */
  uint8  status
)
{

/*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/
  if( status == 0 )
  {
    bt_hc_drv.ctrlr_info.hc2h_flow_st = TRUE;
  }
  else
  {
    bt_hc_drv.ctrlr_info.hc2h_flow_st = FALSE;
    BT_ERR( "BT HC DRV: HC2H Flow Control not supported by controller, st %d",
            status, 0, 0 );
  }

} /* bt_hc_drv_hc2h_flow_support_status */

#endif /* FEATURE_BT_HCI_HOST_FCTL */


/*==========================================================================

                       BT CONTROLLER FLOW CONTROL  -
    Functions involved in implementing flow control between host and
    Controller.

==========================================================================*/
/*==========================================================================

FUNCTION
  BT_HC_DRV_SET_HCI_BUF_SIZES

DESCRIPTION
  Updates the parameters returned from Read Buffer Size command to the
  HCI driver structures.

DEPENDENCIES
  None.

INPUTS
  acl_data_pkt_len       - Max length of ACL data packet in bytes
  sco_data_pkt_len       - Max length of SCO over HCI packet in bytes
  tot_num_acl_data_pkts  - Total number of ACL data packets
  tot_num_sco_data_pkts  - Total number of SCO data packets

RETURN VALUE
  None.

SIDE EFFECTS
  None.

==========================================================================*/
void bt_hc_drv_set_hci_buf_sizes
(
  /* Max length of ACL data packet in bytes */
  uint16 acl_data_pkt_len,

  /* Max length of SCO over HCI packet in bytes */
  uint8  sco_data_pkt_len,

  /* Total number of HCI ACL Data Packets */
  uint16 tot_num_acl_data_pkts,

  /* Total number of HCI SCO Data Packets */
  uint16 tot_num_sco_data_pkts
)
{
/*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/
  /* Update the HCI driver with the Controller buffer size information */
  bt_hc_drv.ctrlr_info.cur_buf.acl_data_pkt_len   = acl_data_pkt_len;
  bt_hc_drv.ctrlr_info.cur_buf.sco_data_pkt_len   = sco_data_pkt_len;
  bt_hc_drv.ctrlr_info.cur_buf.tot_free_data_pkts = tot_num_acl_data_pkts;
  bt_hc_drv.ctrlr_info.cur_buf.tot_free_sco_pkts  = tot_num_sco_data_pkts;

  bt_hc_drv.ctrlr_info.max_buf_info.max_acl_data_pkt_len   = acl_data_pkt_len;
  bt_hc_drv.ctrlr_info.max_buf_info.max_sync_data_pkt_len  = sco_data_pkt_len;
  bt_hc_drv.ctrlr_info.max_buf_info.tot_num_acl_data_pkts  =
    tot_num_acl_data_pkts;
  bt_hc_drv.ctrlr_info.max_buf_info.tot_num_sync_data_pkts =
    tot_num_sco_data_pkts;

  bt_l2_set_max_hci_acl_data_len( acl_data_pkt_len );

} /* bt_hc_drv_set_hci_buf_sizes */


#ifdef FEATURE_BT_HCI_HOST_FCTL

/*==========================================================================

FUNCTION
  BT_HC_DRV_UPDATE_ACL_RX_FLOW

DESCRIPTION
  This function updates the RX flow indicator with the flow On/Off indication
  for each connection handle.

DEPENDENCIES
  None.

INPUTS
  conn_idx   - Index to connection handle table
  is_flow_on - Flow On,Off indicator


RETURN VALUE
  None.

SIDE EFFECTS
  NumComplete Packets command will be stopped if flow is off for the connection
  and will be started if flow is turned on for a connection.

==========================================================================*/

static void bt_hc_drv_update_acl_rx_flow
(
  /* Index to connection handle table */
  uint8   conn_idx,

  /* Flow On,Off indicator*/
  boolean is_flow_on
)
{
/*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/
  if( bt_hc_drv.host_info.hci_rx.rx_acl_flow[ conn_idx ] != is_flow_on)
  {
    /* Update the new RX flow status for this connection */
    bt_hc_drv.host_info.hci_rx.rx_acl_flow[ conn_idx ] = is_flow_on;

    if( bt_hc_drv.host_info.hci_rx.rx_acl_flow[ conn_idx ] == TRUE )
    {
      /* Rx Flow has been turned ON, send NumCompletePkts command if needed */
      bt_hc_drv_send_host_num_cmplt_pkts_cmd();
    } /* if rx_acl_flow[ conn_idx ] == TRUE  */
  } /* if rx_acl_flow[ conn_idx ] != is_flow_on */

} /* bt_hc_drv_update_acl_rx_flow */
#endif /* FEATURE_BT_HCI_HOST_FCTL */


/*==========================================================================

FUNCTION
  BT_HC_DRV_UPDATE_ACL_TX_FLOW

DESCRIPTION
  This function updates the TX flow indicator with the flow On/Off indication
  for each connection handle.

DEPENDENCIES
  None.

INPUTS
  conn_idx   - Index to connection handle table
  is_flow_on - Flow On,Off indicator


RETURN VALUE
  None.

SIDE EFFECTS
  Indications will be tranmitted to RFCOMM and L2CAP for suitable actions
  in upper layers.

==========================================================================*/

static void bt_hc_drv_update_acl_tx_flow
(
  /* Index to connection handle table */
  uint8   conn_idx,

  /* Flow On,Off indicator*/
  boolean is_flow_on
)
{
  /* Convenience pointer */
  bt_hc_drv_tx_struct_type* hc_tx = &bt_hc_drv.host_info.hci_tx;

#ifdef FEATURE_BT_HCI_HOST_FCTL
  int                       hc_tx_q_cnt; /* Count of acl frames in HCI Tx Q */
  uint16                    buf_cnt; /* Number of ACL buffers in Ctrlr */
#endif /* FEATURE_BT_HCI_HOST_FCTL */

/*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/
  /* Update the HCI, RFCOMM and L2CAP about the flow status for this
     connection*/
  if( hc_tx->acl_tx[ conn_idx ].flow_on != is_flow_on)
  {
#ifdef FEATURE_BT_HCI_HOST_FCTL
    hc_tx_q_cnt = q_cnt( &hc_tx->acl_tx[ conn_idx ].acl_data_tx_q );
    buf_cnt     = BT_QSOC_MAX_HC_NUM_TX_BUF;

    /* Check for conditions for flowing off/on - if frame count in HCI TX Q is
       low and dsm count is not low, flow on. If frame count in HCI TX Q is high
       or dsm count is low, flow off */
    if((( is_flow_on == TRUE ) &&
        ( hc_tx_q_cnt <= BT_HC_DRV_ACL_TX_LOW_FR_CNT(buf_cnt,fctl_limiter) ) &&
        ( bt_hc_drv.host_info.is_dsm_cnt_low == FALSE )) ||
       (( is_flow_on == FALSE ) &&
        (( hc_tx_q_cnt > BT_HC_DRV_ACL_TX_HIGH_FR_CNT(buf_cnt,fctl_limiter) )||
         ( bt_hc_drv.host_info.is_dsm_cnt_low == TRUE ))))
    {
      /* Update Rx flow based on TX flow */
      bt_hc_drv_update_acl_rx_flow( conn_idx, is_flow_on );
#endif /* FEATURE_BT_HCI_HOST_FCTL */

     /* Update the HCI, RFCOMM and L2CAP about the flow status for this
        connection */
      hc_tx->acl_tx[ conn_idx ].flow_on = is_flow_on;

      bt_rc_update_acl_tx_flow_on(
        &hc_tx->acl_tx[conn_idx].bd_addr, is_flow_on );

      bt_l2_update_acl_tx_flow_on(
        &hc_tx->acl_tx[conn_idx].bd_addr, is_flow_on );

#ifdef FEATURE_BT_HCI_HOST_FCTL

      BT_MSG_HIGH( "BT HC DRV: Tx flow update idx %x, TxQcnt %x isDsmCntLow %x",
                   conn_idx,hc_tx_q_cnt,bt_hc_drv.host_info.is_dsm_cnt_low );
    }
#endif /* FEATURE_BT_HCI_HOST_FCTL */

#ifdef BT_FLOW_DEBUG
    BT_MSG_HIGH( "BT HC DRV: Tx flow %x, idx %x, WMcnt %x",
                 hc_tx->acl_tx[ conn_idx ].flow_on,
                 conn_idx,
                 hc_tx->acl_tx[ conn_idx ].acl_data_tx_wm.current_cnt               );
#endif /*  BT_FLOW_DEBUG */

  } /* if hc_tx->acl_tx[ conn_idx ].flow_on */

} /* bt_hc_drv_update_acl_tx_flow */


/*==========================================================================

FUNCTION
  BT_HC_DRV_ACL_TX_HI_WM_CB

DESCRIPTION
  This is the ACL Tx High watermark callback function for connection handle 0.

DEPENDENCIES
  None.

INPUTS
  tx_acl_wm_ptr - Pointer to the watermark that has reached High watermark level
  conn_idx_ptr  - Pointer to the connection index

RETURN VALUE
  None.

SIDE EFFECTS
  None.

==========================================================================*/

void bt_hc_drv_acl_tx_hi_wm_cb
(
  /* Pointer to the watermark that has reached High watermark level */
  struct dsm_watermark_type_s *tx_acl_wm_ptr,

  /* Pointer to the connection index */
  void*  conn_idx_ptr
)
{
  /* Index to the connection handle table */
  uint8 conn_idx;
/*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/
  conn_idx = *(uint8 *)conn_idx_ptr;

  /* Pass the flow OFF indication to the connection */
  bt_hc_drv_update_acl_tx_flow( conn_idx, FALSE );
} /* bt_hc_drv_acl_tx_hi_wm_cb */


/*==========================================================================

FUNCTION
  BT_HC_DRV_ACL_TX_LO_WM_CB

DESCRIPTION
  This is the ACL Tx Low watermark callback function for connection handle 0.

DEPENDENCIES
  None.

INPUTS
  tx_acl_wm_ptr - Pointer to the watermark that has reached High watermark level
  conn_idx_ptr  - Pointer to the connection index

RETURN VALUE
  None.

SIDE EFFECTS
  None.

==========================================================================*/

void bt_hc_drv_acl_tx_lo_wm_cb
(
  /* Pointer to the watermark that has reached High watermark level */
  struct dsm_watermark_type_s *tx_acl_wm_ptr,

  /* Pointer to the connection index */
  void*  conn_idx_ptr
)
{
  /* Index to the connection handle table */
  uint8 conn_idx;
/*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/
  conn_idx = *(uint8 *)conn_idx_ptr;

  /* Pass the flow ON indication to the connection */
  bt_hc_drv_update_acl_tx_flow( conn_idx, TRUE );
} /* bt_hc_drv_acl_tx_hi_wm_cb */


/*==========================================================================

FUNCTION
  BT_HC_DRV_RX_UPDATE_H2C_FLOW_PARAMS

DESCRIPTION
  Update the Host to Controller flow control parameters when we receive a Num
  Complete Packets Event.

DEPENDENCIES
  None.

INPUTS
  conn_hndl_idx  - Index to connection handle table
  num_cmplt_pkts - Number of completed acl packets

RETURN VALUE
  None.

SIDE EFFECTS
  None.

==========================================================================*/

static void bt_hc_drv_rx_update_h2c_flow_params
(
  /* Index to connection handle table */
  uint8                          conn_hndl_idx,

  /* Number of completed acl packets from comamnd complete event */
  uint8                          num_cmplt_pkts
)
{
  /* Convenience pointer */
  bt_hc_drv_ctrlr_cur_buf_type *hci_buf_ptr;

/*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/
  hci_buf_ptr = &bt_hc_drv.ctrlr_info.cur_buf;

  /* Sanity Check for out of bounds array index */
  if( conn_hndl_idx < BT_MAX_BT_DEV )
  {
    /* Update the number of ACL buffer entries that have been freed up now */
    hci_buf_ptr->entries[ conn_hndl_idx ].num_buf_used -= num_cmplt_pkts;
    hci_buf_ptr->tot_free_data_pkts += num_cmplt_pkts;

    BT_MSG_API_PYLD( "BT HC DRV RX: H2C NumCmpltPkts, ConnH %d,Used %d,Free %d",
                     hci_buf_ptr->entries[ conn_hndl_idx ].conn_hndl,
                     hci_buf_ptr->entries[ conn_hndl_idx ].num_buf_used,
                     hci_buf_ptr->tot_free_data_pkts  );
  }
  else
  {
    BT_ERR("BT HC DRV Failure: Connection index out of bounds %d",conn_hndl_idx, 0, 0 ); 
  } /*  if conn_hndl_idx < BT_MAX_BT_DEV  */                   

} /* bt_hc_drv_rx_update_h2c_flow_params */


/*==========================================================================

FUNCTION
  BT_HC_DRV_REMOVE_CONN

DESCRIPTION
  This funtion removes the entry for a given connection handle from the host
  table and updates the H2C flow variables

DEPENDENCIES
  None.

INPUTS
  conn_hndl - Connection Handle.

RETURN VALUE
  Index into Host table for the connection handle.

SIDE EFFECTS
  None.

==========================================================================*/

static uint8 bt_hc_drv_remove_conn
(
  /* HCI connection handle */
  uint16 conn_hndl
)
{
  /* Loop counter for connection handles */
  uint8   chndl;

/*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  for( chndl = 0; chndl < BT_MAX_BT_DEV; chndl++ )
  {
    if( bt_hc_drv.ctrlr_info.cur_buf.entries[chndl].conn_hndl == conn_hndl )
    {
      /* Match found - reset this entry */
#ifdef FEATURE_BT_AVSYNC_SOC
      bt_hc_drv_empty_avsync_header_q(chndl, conn_hndl);
#endif /* FEATURE_BT_AVSYNC_SOC */

      if( bt_hc_drv.ctrlr_info.cur_buf.entries[chndl].is_buf_guaranteed == TRUE )
      {
        /* If buffers were allocated for guaranteed QOS, free them. */
        bt_hc_drv.ctrlr_info.cur_buf.tot_free_data_pkts +=
          bt_hc_drv.ctrlr_info.cur_buf.entries[chndl].num_alloc_buf;

        bt_hc_drv.ctrlr_info.cur_buf.entries[chndl].num_alloc_buf = 0;

      } /* if is_buf_guaranteed */
      else
      {
        /* Return the used buffers to the free pool. Controller will not
           send explicit NUMCmpltPkts Ev for these packets */
        bt_hc_drv.ctrlr_info.cur_buf.tot_free_data_pkts +=
          bt_hc_drv.ctrlr_info.cur_buf.entries[chndl].num_buf_used;

      } /* if !is_buf_guaranteed*/

      bt_hc_drv.ctrlr_info.cur_buf.entries[chndl].conn_hndl =
        BT_HC_DRV_ACL_TX_INIT_CHNDL;

      bt_hc_drv.host_info.hci_tx.acl_tx[chndl].conn_hndl =
        BT_HC_DRV_ACL_TX_INIT_CHNDL;

      /* Connection brought down - update this info */
      --bt_hc_drv.ctrlr_info.cur_buf.num_conn;

      /* Clean-up the parameters for this conenction handle */
      bt_hc_drv.ctrlr_info.cur_buf.entries[chndl].num_buf_used = 0;

      /* Empty the DSM watermark for this connection handle */
      dsm_empty_queue( &bt_hc_drv.host_info.hci_tx.acl_tx[chndl].acl_data_tx_wm );

      /* Reinit rx ACL cnt */
      bt_hc_drv.host_info.hci_rx.hc_rx_acl_cnt[ chndl ] = 0;
#ifdef FEATURE_BT_HCI_HOST_FCTL
      /* Reinit rx ACL flow */
      bt_hc_drv.host_info.hci_rx.rx_acl_flow[ chndl ] = TRUE;
#endif /* FEATURE_BT_HCI_HOST_FCTL */

      BT_MSG_API( "BT HC DRV : ConnHnIdx %d buffer freed. Res %d, Used %d",
                  chndl,
                  bt_hc_drv.ctrlr_info.cur_buf.entries[chndl].num_alloc_buf,
                  bt_hc_drv.ctrlr_info.cur_buf.entries[chndl].num_buf_used );

      break;
    } /* if bt_hc_drv.ctrlr_info.cur_buf.entries[chndl].conn_hndl==conn_hndl */
  } /* for chndl */

  return( chndl );

} /* bt_hc_drv_remove_conn */


/*==========================================================================

FUNCTION
  BT_HC_DRV_ALLOC_BUF

DESCRIPTION
  This function allocates buffers for a given connection.

DEPENDENCIES
  None.

INPUTS
  conn_hndl_idx - Index into the Connection Handle table

RETURN VALUE
  Flag to indicate whether the number of buffers specified were allocated or
  not.

SIDE EFFECTS
  None.

==========================================================================*/

static boolean bt_hc_drv_alloc_buf
(
  /* Index into the Connection Handle table */
  uint8    conn_hndl_idx
)
{
  boolean  buf_allocated = FALSE;

/*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/
  /* Sanity Check for out of bounds array index */
  if( conn_hndl_idx < BT_MAX_BT_DEV )
  {
    if( bt_hc_drv.ctrlr_info.cur_buf.entries[conn_hndl_idx].conn_hndl !=
      BT_HC_DRV_ACL_TX_INIT_CHNDL )
    {
      if ( bt_hc_drv.ctrlr_info.cur_buf.entries[conn_hndl_idx].is_buf_guaranteed
          == TRUE )
      {
        /* Guaranteed QOS - buffers need to be reserved */

        if( bt_hc_drv.ctrlr_info.cur_buf.tot_free_data_pkts >=
          bt_hc_drv.ctrlr_info.cur_buf.entries[conn_hndl_idx].num_buf_req )
        {
          /* Number of buffers requested is available in the free pool.
             Reserve them */
          bt_hc_drv.ctrlr_info.cur_buf.entries[conn_hndl_idx].num_alloc_buf =
            bt_hc_drv.ctrlr_info.cur_buf.entries[conn_hndl_idx].num_buf_req;

          /* Update that buffers have been taken away from the free pool */
          bt_hc_drv.ctrlr_info.cur_buf.tot_free_data_pkts -=
            bt_hc_drv.ctrlr_info.cur_buf.entries[conn_hndl_idx].num_alloc_buf;

          bt_hc_drv.ctrlr_info.cur_buf.entries[conn_hndl_idx].num_buf_used = 0;
          buf_allocated = TRUE;

          BT_MSG_API( "BT HC DRV : ConnIdx %d, buf alloc. Alloc %d, Free %d",
                       conn_hndl_idx,
                       bt_hc_drv.ctrlr_info.cur_buf.entries[conn_hndl_idx].num_buf_used,
                       bt_hc_drv.ctrlr_info.cur_buf.tot_free_data_pkts );
        }
        else
        {
          /* Number of buffers requested is more than what is in the free pool.
             Cannot reserve now */
          BT_MSG_API( "BT HC DRV : ConnIdx %d, Can't alloc buf. Alloc %d, Free %d",
                       conn_hndl_idx,
                       bt_hc_drv.ctrlr_info.cur_buf.entries[conn_hndl_idx].num_buf_used,
                       bt_hc_drv.ctrlr_info.cur_buf.tot_free_data_pkts );
        } /* else bt_hc_drv.ctrlr_info.cur_buf.tot_free_data_pkts < num_buf */
      } /* if (bt_hc_drv.ctrlr_info.cur_buf.entries...) */
    }    
  } /* conn_hndl != BT_HC_DRV_ACL_TX_INIT_CHNDL */
  else
  {
    BT_ERR("BT HC DRV Failure: Connection index out of bounds %d",
            conn_hndl_idx, 0, 0 ); 
  } /* if conn_hndl_idx < BT_MAX_BT_DEV  .. */   
  
  return( buf_allocated );
} /* bt_hc_drv_alloc_buf */


/*==========================================================================

FUNCTION
  BT_HC_DRV_UPDATE_NEW_CONN

DESCRIPTION
  This funtion updates the host table buffer entry with the given connection
  handle if it is not already present.

DEPENDENCIES
  None.

INPUTS
  conn_cp_msg - Connection Complete event

RETURN VALUE
  Index into Host table for the connection handle.

SIDE EFFECTS
  None.

==========================================================================*/

static uint8 bt_hc_drv_update_new_conn
(
  /* Connection complete event information */
  bt_ev_hc_conn_cmplt_type conn_cp_msg
)
{
  uint8                            chndl;
    /* Loop counter for connection handles */

/*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  for( chndl = 0; chndl < BT_MAX_BT_DEV; chndl++ )
  {
    if( bt_hc_drv.ctrlr_info.cur_buf.entries[chndl].conn_hndl ==
        BT_HC_DRV_ACL_TX_INIT_CHNDL )
    {
      /* Space available for a new connection - update it now */
      bt_hc_drv.ctrlr_info.cur_buf.entries[chndl].conn_hndl =
        conn_cp_msg.conn_hndl;

      /* Update the connection handle and BD address in the Host table */
      bt_hc_drv.host_info.hci_tx.acl_tx[chndl].conn_hndl =
        conn_cp_msg.conn_hndl;
      bt_hc_drv.host_info.hci_tx.acl_tx[chndl].bd_addr = conn_cp_msg.bd_addr;

      /* New connection handle added - update the number of connections. */
      bt_hc_drv.ctrlr_info.cur_buf.num_conn++;

      /* Allocate buffers if needed for guaranteed QOS */
      //buf_allocated = bt_hc_drv_alloc_buf( chndl );

      break;
    } /* if */
  }  /* for chndl */

  return( chndl );

} /* bt_hc_drv_update_new_conn */


/*==========================================================================

FUNCTION
  BT_HC_DRV_GET_CONN_HNDL_IDX

DESCRIPTION
  This funtion looks up the given connection handle in the Host HCI table and
  returns the index.

DEPENDENCIES
 None.

INPUTS
  Connection Handle.

RETURN VALUE
  Index into Host table for the connection handle.

SIDE EFFECTS
  None.

==========================================================================*/

static uint8 bt_hc_drv_get_conn_hndl_idx
(
  uint16 conn_hndl
)
{
  uint8                            chndl;
    /* Loop counter for connection handles */

  uint8                            chndl_idx = BT_HC_DRV_ACL_TX_INIT_CHNDL;
    /* Index corresponding to the connection handle */

/*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  for( chndl = 0; chndl < BT_MAX_BT_DEV; chndl++ )
  {
    if( bt_hc_drv.ctrlr_info.cur_buf.entries[chndl].conn_hndl == conn_hndl )
    {
      chndl_idx = chndl;
      break;
    } /* if */
  }  /* for chndl */

  return( chndl_idx );

} /* bt_hc_drv_get_conn_hndl_idx */


/*==========================================================================

FUNCTION
  BT_HC_DRV_PROCESS_DISCONN_COMPLETE_EV

DESCRIPTION
  This funtion processes the Disconnection Complete event from the
  controller.

DEPENDENCIES
  None.

INPUTS
  Pointer to the Disconnection Complete event structure.

RETURN VALUE
  None.

SIDE EFFECTS
  If the disconnection was successful, then the HCI buffer entry for this
  connection is destroyed and H2C flow control for this link stops.

==========================================================================*/

static void bt_hc_drv_process_disconn_complete_ev
(
  bt_ev_msg_type*  ev_msg_ptr    /* pointer to BT HCI event */
)
{
/*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  if ( ev_msg_ptr->ev_msg.ev_hc_disc.status == 0)
  {
    /* Disconnection successfully done by Ctrlr. Bring down the H2C flow
       control buffer entry */
    bt_hc_drv_remove_conn( ev_msg_ptr->ev_msg.ev_hc_disc.conn_hndl );

    BT_MSG_API( "BT HC DRV : DisconnComplete Hndl %d, New Num_conn %d",
                 ev_msg_ptr->ev_msg.ev_hc_disc.conn_hndl,
                 bt_hc_drv.ctrlr_info.cur_buf.num_conn, 0);

  } /* if bt_ev_msg_type->ev_hc_concp.status */

} /* bt_hc_drv_process_disconn_complete_ev */


/*==========================================================================

FUNCTION
  BT_HC_DRV_PROCESS_CONN_COMPLETE_EV

DESCRIPTION
  This funtion processes the Connection Complete event from the
  controller.

DEPENDENCIES
  None.

INPUTS
  Pointer to the NumCmpltPkts event structure.

RETURN VALUE
  None.

SIDE EFFECTS
  If the conection was successful, then a new HCI buffer entry for this
  connection is created and H2C flow control for this link starts.

==========================================================================*/

static void bt_hc_drv_process_conn_complete_ev
(
  bt_ev_msg_type*  ev_msg_ptr    /* pointer to BT HCI event */
)
{
 uint8  conn_hndl_idx;

/*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  if ( ev_msg_ptr->ev_msg.ev_hc_concp.status == 0)
  {
    /* New connection set-up successfully by Ctrlr. Create a new H2C flow
       control buffer entry */
    conn_hndl_idx = bt_hc_drv_update_new_conn(
                      ev_msg_ptr->ev_msg.ev_hc_concp );

    if ( conn_hndl_idx == BT_HC_DRV_ACL_TX_INIT_CHNDL )
    {
      /* Invalid HCI connection handle index. Max connections reached */
      BT_ERR( "BT HC DRV : > %d connections attempted - not supported",
               BT_MAX_BT_DEV, 0, 0);
    }
    else
    {
      /* New HCI entry added for Connection handle */
      BT_MSG_API( "BT HC DRV : ConnComplete Hndl %d, Idx %d Num_conn %d",
                   ev_msg_ptr->ev_msg.ev_hc_concp.conn_hndl,
                   conn_hndl_idx,
                   bt_hc_drv.ctrlr_info.cur_buf.num_conn);

      /* Check if buffers need to be reserved for guaranteed QOS */
      bt_hc_drv_alloc_buf( conn_hndl_idx );
    } /* if ! (conn_hndl_idx == BT_HC_DRV_ACL_TX_INIT_CHNDL) */
  } /* if bt_ev_msg_type->ev_hc_concp.status */

} /* bt_hc_drv_process_conn_complete_ev */


#ifdef BT_SWDEV_CMD_FLOW_WORKAROUND
/*==========================================================================

FUNCTION
  BT_HC_UPDATE_LRG_EVT_STS_WITH_CMD

DESCRIPTION
  This funtion checks if given a command will generate a large evt and
  depending upon result sets/clear  flags.

DEPENDENCIES
  None.

INPUTS
  Command pointer.

RETURN VALUE
  None.

SIDE EFFECTS

==========================================================================*/

static void bt_hc_update_lrg_evt_sts_with_cmd
(
  uint16 cmdOpcode
)
{
    if ( bt_hc_drv.host_info.hci_tx.cmd_tx.expecting_large_evt == FALSE )
    {
        /* Set large evt flag when commands which needs large evt are sent
         * to controller e.g. remote name commands */
        switch ( cmdOpcode )
        {
        case BT_HCI_REMOTE_NAME_RQST:
            BT_MSG_HIGH("BT HC DRV : Setting Large Evt Sts",0,0,0);
            bt_hc_drv.host_info.hci_tx.cmd_tx.expecting_large_evt = TRUE;
            break;
        default:
            break;
        }
    }
    else
    {
        /* Reset large evt flag when cancel commands needs to be send
         * to controller e.g. remote name cancel commands */
        switch ( cmdOpcode )
        {
        case BT_HCI_REMOTE_NAME_RQST_CANCEL:
            BT_MSG_HIGH("BT HC DRV : Resetting Large Evt Sts",0,0,0);
            bt_hc_drv.host_info.hci_tx.cmd_tx.expecting_large_evt = FALSE;
            /* Process Pending COmmands */
            bt_hc_drv_process_cmd_from_wm();
            break;
        default:
            break;
        }
    }

} /* bt_hc_update_lrg_evt_sts_with_cmd */


/*==========================================================================

FUNCTION
  BT_HC_UPDATE_LRG_EVT_STS_WITH_EVT

DESCRIPTION
  This funtion chears large evt sts, when expected events are recieved.


DEPENDENCIES
  None.

INPUTS
  Command pointer.

RETURN VALUE
  None.

SIDE EFFECTS

==========================================================================*/

static void bt_hc_update_lrg_evt_sts_with_evt
(
  bt_ev_msg_type*  ev_msg_ptr    /* pointer to BT HCI evt */
)
{
    bt_ev_hc_cmd_status_type*        cmd_status_ev_ptr;

    if ( bt_hc_drv.host_info.hci_tx.cmd_tx.expecting_large_evt == TRUE )
    {
        switch (ev_msg_ptr->ev_hdr.ev_type)
        {
            case BT_EV_HC_COMMAND_STATUS:
                cmd_status_ev_ptr = &ev_msg_ptr->ev_msg.ev_hc_cmdst;
                if( cmd_status_ev_ptr->status != BT_BE_SUCCESS )
                {
                   bt_hc_drv.host_info.hci_tx.cmd_tx.expecting_large_evt = FALSE;
                   BT_MSG_HIGH("BT HC DRV : Command sts Lrg Evt Sts %d opcode %x",
                               bt_hc_drv.host_info.hci_tx.cmd_tx.expecting_large_evt,
                               cmd_status_ev_ptr->cmd_opcode,0);
                   /* Process Pending COmmands */
                   bt_hc_drv_process_cmd_from_wm();
                }
                break;
            case BT_EV_HC_REMOTE_NAME_REQ_COMPLETE:
                bt_hc_drv.host_info.hci_tx.cmd_tx.expecting_large_evt = FALSE;
                BT_MSG_HIGH("BT HC DRV : Remote name compt Lrg Evt Sts %d evt %x",
                            bt_hc_drv.host_info.hci_tx.cmd_tx.expecting_large_evt,
                            ev_msg_ptr->ev_hdr.ev_type,0);
                /* Process Pending COmmands */
                bt_hc_drv_process_cmd_from_wm();
                break;
            default :
                break;
        }
    }
} /* bt_hc_update_lrg_evt_sts_with_evt */

#endif


/*==========================================================================

FUNCTION
  BT_HC_DRV_TX_UPDATE_H2C_FLOW_PARAMS

DESCRIPTION
  Update the Host to Controller flow control parameters.

DEPENDENCIES
  None.

INPUTS
  Index to connection handle table.

RETURN VALUE
  None.

SIDE EFFECTS
  None.

==========================================================================*/

static void bt_hc_drv_tx_update_h2c_flow_params
(
  /* Index to connection handle table */
  uint8                          conn_hndl_idx
)
{
  /* Convenience pointer */
  bt_hc_drv_ctrlr_cur_buf_type *hci_buf_ptr;

/*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/
  hci_buf_ptr = &bt_hc_drv.ctrlr_info.cur_buf;

  /* Update that one free ACL buffer entry in HCtrlr will be used up now */
  hci_buf_ptr->tot_free_data_pkts--;
  hci_buf_ptr->entries[ conn_hndl_idx ].num_buf_used++;

  BT_MSG_API_PYLD( "BT HC DRV TX: H2C ACL pkt sent ConnH %d, Used %d, Free %d",
                   hci_buf_ptr->entries[ conn_hndl_idx ].conn_hndl,
                   hci_buf_ptr->entries[ conn_hndl_idx ].num_buf_used,
                   hci_buf_ptr->tot_free_data_pkts );

#ifdef FEATURE_BT_QSOC_SLEEP
  if (hci_buf_ptr->tot_free_data_pkts == 0)
  {
      /* ACL Flowed off */
      bt_qsoc_sleep_sm_eval_event(BT_QSOC_SLEEP_EVENT_HCI_ACL_FLOWED_OFF, TRUE);
  }
#endif /* FEATURE_BT_QSOC_SLEEP */


} /* bt_hc_drv_tx_update_h2c_flow_params */

/*==========================================================================

FUNCTION
  BT_HC_DRV_SIO_WAKEUP_AND_TRANSMIT

DESCRIPTION
  Before sending data to SIO ensure that it is ready. If it is powered down,
  it needs to be brought up and SIO sleep vote needs to change.

DEPENDENCIES
  None.

INPUTS
  The data to be sent, and the stream over which it will be transmitted.

RETURN VALUE
  None.

SIDE EFFECTS
  The SIO driver is awakened if it is sleeping.  Additionally, when
  FEATURE_BT_QSOC_SLEEP is defined, the Bluetooth thread will be
  suspended until the SoC is awake.

==========================================================================*/
void bt_hc_drv_sio_wakeup_and_transmit
(
  sio_stream_id_type stream_id,           /* Stream ID                     */
  dsm_item_type     *tx_ptr               /* Packet for transmission       */
)
{
#ifdef FEATURE_BT_QSOC_INBAND_SLEEP
  sio_ioctl_param_type  ioctl_param;
#endif /* FEATURE_BT_QSOC_INBAND_SLEEP */

  if ( stream_id == SIO_NO_STREAM_ID )
  {
    /* Log error and bail out */
    dsm_free_packet( &tx_ptr );
    BT_ERR( "BT HC DRV : Attempt to tx with invalid sio stream id !!",
             0,0,0);
    return;
  }

#ifdef FEATURE_BT_ALIGN_SIO_DATA
   if ((bt_dsm_align_buffer(&tx_ptr)) == FALSE)
   {
      /* Error. Data alignment failed */
      dsm_free_packet(&tx_ptr);

      BT_ERR( "BT HC DRV : Attempt to align tx data failed!!",
               0,0,0);

      return;
   }
#endif /*FEATURE FEATURE_BT_ALIGN_SIO_DATA*/  

#ifdef BT_SWDEV_BT_DISABLED_NV
  if ( bt_dc.is_bt_disabled == FALSE )
  {
#endif /* BT_SWDEV_BT_DISABLED_NV */

#ifdef FEATURE_BT_SOC1
#error code not present
#endif /* FEATURE_BT_SOC1 */

#ifdef FEATURE_BT_QSOC_SLEEP

  {
    uint32 isave;

    INTLOCK_SAV( isave );

#ifdef FEATURE_BT_QSOC_INBAND_SLEEP
    if ( BT_QSOC_SLEEP_INBAND_TX_IN_SLEEP() == TRUE )
    {
      /* At this stage we would have locked out any RX high WAKUP ISRs from SOC
         to avoid race conditions. The SoC may be asleep or it might have just
         wokeup by sending TX UNBREAK.  We avoid this race condition by disabling
         ISRs temporarily. Once the TX path is awake, it will in any case
         awaken the RX path too.    */


      /*  Because the SIO driver may not obey HW flow control for commands
          4 bytes and less in size, ensure the SoC is awake before passing
          the command to the SIO driver. */
      if( BT_QSOC_IS_READY() == TRUE )
      {
        /* Since the MSM's TX path is down, set TX UNBREAK to wakeup SOC */
        BT_QSOC_SLEEP_INBAND_SET_WAIT_UNBRK_ACK();

        BT_LOG_INBAND_STATE( "BT HC DRV :MSM Sending TX UNBRK to SOC ");

        /* Register for the RX high callback function  before restoring the
           lockedout interrupts any RX wakeup can occur only at this stage. */
        ioctl_param.rx_high_event_func_ptr = bt_qsoc_inband_rx_unbreak;
        bt_dc_sio_ioctl( bt_dc.sio_stream_id_ext_soc,
                         SIO_IOCTL_RX_HIGH_EVENT,
                         &ioctl_param );

        bt_dc_sio_ioctl( bt_dc.sio_stream_id_ext_soc,
                         SIO_IOCTL_TX_END_BREAK,
                         NULL );
        INTFREE_SAV( isave );

        bt_qsoc_sleep_wake_soc_inband();

        /* RST the FLUSH timer as we anyway start TX FLUSH again after this TX*/
        BT_QSOC_INBAND_CLR_TX_FLUSH_TIMER();

        INTLOCK_SAV( isave );
       /* Indicate to the sleep state machine that MSM is about to TX. We need
          to do this just before sio_transmit. But Race condition may occur
          inspite of this, Hence we lock ISRs as MSM goes to sleep only after
          the FLUSH callback fires.*/
        bt_qsoc_sleep_sm_eval_event(BT_QSOC_SLEEP_EVENT_SIO_TX_BUSY, TRUE);
        sio_transmit( stream_id, tx_ptr );
        INTFREE_SAV( isave );
      }
      else
      {
        /* Restore the lockedout interrupts */
        INTFREE_SAV( isave );

        /* No need to use INBAND Sleep signalling as the Driver is not yet enabled,
           TX Path can transmit without inband sleep functionality */
        BT_LOG_INBAND_STATE( "BT HC DRV: TX Req before SOC is initialized ");

        /* RST the FLush timer as we are anyway going to start TX FLUSH again after this TX*/
        BT_QSOC_INBAND_CLR_TX_FLUSH_TIMER();

        /* The TX Line is already High.. It will be high until
          bt_qsoc_inband_init() is called. */
        bt_qsoc_sleep_sm_eval_event(BT_QSOC_SLEEP_EVENT_SIO_TX_BUSY, TRUE);
        sio_transmit( stream_id, tx_ptr );
      }

    }
    else if ( BT_QSOC_SLEEP_INBAND_TX_NOT_READY() == TRUE )
    {
      uint16 bt_qsoc_inband_rx_state = bt_qsoc_sleep_inband_get_rx_state();
      uint16 bt_qsoc_inband_tx_state = bt_qsoc_sleep_inband_get_tx_state();
      uint16 bt_qsoc_sleep_msm_state = bt_qsoc_sleep_sm_get_msm_state();

      /* No TX should have been attempted when the Inband TX path is in
         'BREAK\Wait for UNBRK ACK' state. Log a error*/
      BT_ERR( "BT HC DRV: Invalid TX Attempt by Inband SM Rx_st:%d Tx_st:%d msm_st:%d",
               bt_qsoc_inband_rx_state,
               bt_qsoc_inband_tx_state,
               bt_qsoc_sleep_msm_state );
    }
#else
    if (bt_qsoc_sleep_sm_get_soc_state() == BT_QSOC_SLEEP_SOC_STATE_ASLEEP)
    {
      /* The SoC is already asleep, no need to continue to lock ISRs */
      INTFREE_SAV( isave );

      /* Indicate to the sleep state machine that MSM is about to TX */
      bt_qsoc_sleep_sm_eval_event(BT_QSOC_SLEEP_EVENT_SIO_TX_BUSY, TRUE);

      /*
        Because the SIO driver may not obey HW flow control for commands
        4 bytes and less in size, ensure the SoC is awake before passing
        the command to the SIO driver.
      */
      bt_qsoc_sleep_wake_soc();

      sio_transmit( stream_id, tx_ptr );

    }
#endif /* FEATURE_BT_QSOC_INBAND_SLEEP*/
    else
    {
      /*
        The SoC is already awake, ensure it stays awake until the command
        is passed to the SIO driver.
      */
      BT_LOG_INBAND_STATE( "BT HC DRV Inband: SOC Already Awake");

#ifdef FEATURE_BT_QSOC_INBAND_SLEEP
      /* For Inband sleep we reset the Flush timer as we are anyway going
         to start TX FLUSH again after this TX*/
      BT_QSOC_INBAND_CLR_TX_FLUSH_TIMER();
#endif /* FEATURE_BT_QSOC_INBAND_SLEEP*/

      bt_qsoc_sleep_sm_eval_event(BT_QSOC_SLEEP_EVENT_SIO_TX_BUSY, TRUE);

      sio_transmit( stream_id, tx_ptr );

      INTFREE_SAV( isave );
    }

    bt_qsoc_sleep_issue_tx_flush(bt_dc.sio_stream_id_ext_soc);
  }
#else

  if ( bt_dc.sio_state != BT_SIO_AWAKE )
  {
#ifdef BT_SLEEP_DEBUG
    BT_MSG_HIGH( "BT HC DRV: Calling update_sio_state - tx activity",0,0,0);
#endif
      bt_dc_update_sio_state( BT_SIO_AWAKE, TRUE );
  }

  sio_transmit( stream_id, tx_ptr );

#endif /* FEATURE_BT_QSOC_SLEEP */

#ifdef BT_SWDEV_BT_DISABLED_NV
  }
  else
  {
    BT_MSG_DEBUG("BT disabled. Cannot transmit.", 0, 0, 0);
  } /* bt_dc.is_bt_disabled */
#endif /* BT_SWDEV_BT_DISABLED_NV */
} /* bt_hc_drv_sio_wakeup_and_transmit */


/*===========================================================================

FUNCTION
  BT_HC_DRV_RESET_ACL_FR_CNT

DESCRIPTION
  Reset the number of ACL frames received in last round of SIO Rx watermark
  processing.

DEPENDENCIES
  None.

INPUTS
  None.

RETURN VALUE
  None.

SIDE EFFECTS
  None.

===========================================================================*/
void bt_hc_drv_reset_acl_fr_cnt( void )
{
  uint8 c_hndl;             /* Counter for ACL connections */

/*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  for( c_hndl = 0; c_hndl < BT_MAX_BT_DEV; c_hndl++ )
  {
    bt_hc_drv.host_info.hci_rx.hc_rx_acl_cnt[ c_hndl ] = 0;
  } /* for c_hndl */

} /* bt_hc_drv_reset_acl_fr_cnt */


#ifdef FEATURE_BT_HCI_HOST_FCTL
/*==========================================================================

FUNCTION
  BT_HC_DRV_UPDATE_DSM_CNT

DESCRIPTION
 Updates the DSM COUNT Level

DEPENDENCIES
  None.

INPUTS
  None.

RETURN VALUE
  None.

SIDE EFFECTS
  None.

==========================================================================*/
void bt_hc_drv_update_dsm_cnt
(
  void
)
{

/*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  bt_hc_drv.host_info.is_dsm_cnt_low =
    ( bt_hc_drv.host_info.is_small_dsm_cnt_low
      || bt_hc_drv.host_info.is_large_dsm_cnt_low);
}


/*===========================================================================

FUNCTION
  BT_HC_DRV_FCTL_SMALL_DSM_CB

DESCRIPTION
  Callback funtion that is called when the Large DSM item free count
  becomes either 'more than many' or 'less than few'.

DEPENDENCIES
  None.

INPUTS
  dsm_pool_id - DSM small pool ID,
  dsm_mem_lvl - memory levl for which this callback is called,
  dsm_op      - dsm operation.

RETURN VALUE
  None.

SIDE EFFECTS
  None.

===========================================================================*/
void bt_hc_drv_fctl_small_dsm_cb
(
  dsm_mempool_id_type     dsm_pool_id,
  dsm_mem_level_enum_type dsm_mem_lvl,
  dsm_mem_op_enum_type    dsm_op
)
{
  bt_cmd_msg_type*    bt_cmd_ptr;

/*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  BT_MSG_HIGH( "BT HC DRV FCTL: DSM item count callback lvl :%d ",dsm_mem_lvl,0,0 );

  /*This Callback can be called from Other task's context. We need to preserve
    the seq. of calls and make the call reentrant*/

  switch ( dsm_mem_lvl )
  {
    case DSM_MEM_LEVEL_MANY:
      if( ( bt_cmd_ptr = bt_get_free_cmd() ) != NULL )
      {
        bt_initialize_cmd_buffer( bt_cmd_ptr );
        bt_cmd_ptr->cmd_hdr.cmd_type  = BT_CMD_FCTL_SMALL_DSM_HIGH_CNT;
        bt_cmd_ptr->cmd_hdr.bt_app_id = BT_APP_ID_NULL;
        /* Send FCTL CMD */
        bt_cmd( bt_cmd_ptr );
      }
      break;

    case DSM_MEM_LEVEL_FEW:
      if( ( bt_cmd_ptr = bt_get_free_cmd() ) != NULL )
      {
        bt_hc_drv.host_info.is_small_dsm_cnt_low = TRUE;
        bt_hc_drv_update_dsm_cnt();

        bt_initialize_cmd_buffer( bt_cmd_ptr );
        bt_cmd_ptr->cmd_hdr.cmd_type  = BT_CMD_FCTL_SMALL_DSM_LOW_CNT;
        bt_cmd_ptr->cmd_hdr.bt_app_id = BT_APP_ID_NULL;
        /* Send FCTL CMD */
        bt_cmd( bt_cmd_ptr );
      }
      break;

    default:
      BT_ERR( "BT HC DRV FCTL: Invalid DSM memory level value ",0,0,0 );
      break;
  }

} /* bt_hc_drv_fctl_small_dsm_cb */


/*===========================================================================

FUNCTION
  BT_HC_DRV_FCTL_LARGE_DSM_CB

DESCRIPTION
  Callback funtion that is called when the Large DSM item free count
  becomes either 'more than many' or 'less than few'.

DEPENDENCIES
  None.

INPUTS
  dsm_pool_id - DSM small pool ID,
  dsm_mem_lvl - memory levl for which this callback is called,
  dsm_op      - dsm operation.

RETURN VALUE
  None.

SIDE EFFECTS
  None.

===========================================================================*/
void bt_hc_drv_fctl_large_dsm_cb
(
  dsm_mempool_id_type     dsm_pool_id,
  dsm_mem_level_enum_type dsm_mem_lvl,
  dsm_mem_op_enum_type    dsm_op
)
{
  bt_cmd_msg_type*    bt_cmd_ptr;

/*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  BT_MSG_HIGH( "BT HC DRV FCTL: DSM item count callback lvl :%d ",dsm_mem_lvl,0,0 );

  /*This Callback can be called from Other task's context. We need to preserve
    the seq. of calls and make the call reentrant*/

  switch ( dsm_mem_lvl )
  {
    case DSM_MEM_LEVEL_MANY:
      if( ( bt_cmd_ptr = bt_get_free_cmd() ) != NULL )
      {
        bt_initialize_cmd_buffer( bt_cmd_ptr );
        bt_cmd_ptr->cmd_hdr.cmd_type  = BT_CMD_FCTL_LARGE_DSM_HIGH_CNT;
        bt_cmd_ptr->cmd_hdr.bt_app_id  = BT_APP_ID_NULL;
        /* Send FCTL CMD */
        bt_cmd( bt_cmd_ptr );
      }
      break;

    case DSM_MEM_LEVEL_FEW:
      if( ( bt_cmd_ptr = bt_get_free_cmd() ) != NULL )
      {
        bt_hc_drv.host_info.is_large_dsm_cnt_low = TRUE;
        bt_hc_drv_update_dsm_cnt();

        bt_initialize_cmd_buffer( bt_cmd_ptr );
        bt_cmd_ptr->cmd_hdr.cmd_type  = BT_CMD_FCTL_LARGE_DSM_LOW_CNT;
        bt_cmd_ptr->cmd_hdr.bt_app_id  = BT_APP_ID_NULL;
        /* Send FCTL CMD */
        bt_cmd( bt_cmd_ptr );
      }
      break;

    default:
      BT_ERR( "BT HC DRV FCTL: Invalid DSM memory level value ",0,0,0 );
      break;
  }

} /* bt_hc_drv_fctl_large_dsm_cb */


/*==========================================================================

FUNCTION
  BT_HC_DRV_UPDATE_ALL_ACL_TX_FLOW

DESCRIPTION
  This function updates the TX flow indicator with the flow On/Off indication
  for each connection handle.

DEPENDENCIES
  None.

INPUTS
  conn_idx   - Index to connection handle table
  is_flow_on - Flow On,Off indicator


RETURN VALUE
  None.

SIDE EFFECTS
  Indications will be tranmitted to RFCOMM and L2CAP for suitable actions
  in upper layers.

==========================================================================*/
void bt_hc_drv_update_all_acl_tx_flow
(
  /* Flow On,Off indicator*/
  boolean is_flow_on
)
{
  uint8 conn_idx;

/*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  /* Pass the flow ON indication to all connections */
  for( conn_idx = 0; conn_idx < BT_MAX_BT_DEV; conn_idx++ )
  {
    bt_hc_drv_update_acl_tx_flow( conn_idx, is_flow_on );
  } /* for conn_idx */

} /* bt_hc_drv_update_acl_tx_flow */

/*===========================================================================

FUNCTION
  BT_HC_DRV_FCTL_HIGH_DSM

DESCRIPTION
  Turns On Flow to all ACL connections when the DSM small item count becomes
  normal.

DEPENDENCIES
  None.

INPUTS
  None.

RETURN VALUE
  None.

SIDE EFFECTS
  None.

===========================================================================*/
void bt_hc_drv_fctl_small_dsm_high( void )
{


/*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  BT_MSG_HIGH( "BT HC DRV FCTL: Update ACL TX FLOW to ON",0,0,0);


  bt_hc_drv.host_info.is_small_dsm_cnt_low = FALSE;
  bt_hc_drv_update_dsm_cnt();

  if ( bt_hc_drv.host_info.is_dsm_cnt_low  == FALSE )
  {
    /* Pass the flow ON indication to all connections */
    bt_hc_drv_update_all_acl_tx_flow( TRUE );
  }

} /* bt_hc_drv_fctl_small_dsm_high */


/*===========================================================================

FUNCTION
  BT_HC_DRV_FCTL_SMALL_DSM_LOW

DESCRIPTION
  Turns off Flow to all ACL connections when the DSM small item count goes
  low.

DEPENDENCIES
  None.

INPUTS
  None.

RETURN VALUE
  None.

SIDE EFFECTS
  None.

===========================================================================*/
void bt_hc_drv_fctl_small_dsm_low( void )
{
/*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  BT_MSG_HIGH( "BT HC DRV FCTL: Update ACL TX FLOW to OFF",0,0,0);

  /* Pass the flow OFF indication to all connections */
  if ( bt_hc_drv.host_info.is_dsm_cnt_low == TRUE )
  {
    bt_hc_drv_update_all_acl_tx_flow( FALSE );
  }

} /* bt_hc_drv_fctl_small_dsm_low */


/*===========================================================================

FUNCTION
  BT_HC_DRV_FCTL_LARGE_DSM_HIGH

DESCRIPTION
  Turns On Flow to all ACL connections when the DSM small item count becomes
  normal.

DEPENDENCIES
  None.

INPUTS
  None.

RETURN VALUE
  None.

SIDE EFFECTS
  None.

===========================================================================*/
void bt_hc_drv_fctl_large_dsm_high( void )
{

/*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  BT_MSG_HIGH( "BT HC DRV FCTL: Update ACL TX FLOW to ON",0,0,0);

  bt_hc_drv.host_info.is_large_dsm_cnt_low = FALSE;
  bt_hc_drv_update_dsm_cnt();

  if ( bt_hc_drv.host_info.is_dsm_cnt_low == FALSE )
  {
    /* Pass the flow ON indication to all connections */
    bt_hc_drv_update_all_acl_tx_flow( TRUE );
  }

} /* bt_hc_drv_fctl_large_dsm_high */


/*===========================================================================

FUNCTION
  BT_HC_DRV_FCTL_LARGE_DSM_LOW

DESCRIPTION
  Turns off Flow to all ACL connections when the DSM small item count goes
  low.

DEPENDENCIES
  None.

INPUTS
  None.

RETURN VALUE
  None.

SIDE EFFECTS
  None.

===========================================================================*/
void bt_hc_drv_fctl_large_dsm_low( void )
{

/*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  BT_MSG_HIGH( "BT HC DRV FCTL: Update ACL TX FLOW to OFF",0,0,0);

  /* Pass the flow OFF indication to all connections */
  if ( bt_hc_drv.host_info.is_dsm_cnt_low == TRUE )
  {
    bt_hc_drv_update_all_acl_tx_flow( FALSE );
  }
} /* bt_hc_drv_fctl_large_dsm_low */
#endif /* FEATURE_BT_HCI_HOST_FCTL */


/*==========================================================================

FUNCTION
  BT_HC_DRV_SEND_ACL_DATA

DESCRIPTION
  Send ACL data to host controller.

DEPENDENCIES
  None.

INPUTS
  Dsm pointer to serial HCI ACL packet,
  Index to the connection handle table

RETURN VALUE
  None.

SIDE EFFECTS
  None.

==========================================================================*/

static void bt_hc_drv_send_acl_data
(
  /* DSM pointer to HCI ACL data packet */
  dsm_item_type*  hci_acl_dsm_ptr,

  /* Index to the connection handle table */
  uint8           conn_hndl_idx
)
{
  /* Length of ACL data packet */
  uint32 data_len;

  /* Packet boundary flag */
  uint8  pb_flag;

/*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  data_len = dsm_length_packet( hci_acl_dsm_ptr );
  dsm_peek_byte( hci_acl_dsm_ptr, 2, &pb_flag );
  BT_MSG_API_PYLD( "BT HC DRV TX: ACL pkt sent, len %d, PB %d TxAcl# %d",
                    data_len,
                    ((pb_flag >> 4) & 0x3),
                    bt_hc_drv.host_info.bt_hc_drv_txrx_cnt.hci_tx_acl_cnt++ );

  /* Log the packet to diag */
  bt_hc_drv_log_host_tx_data( hci_acl_dsm_ptr, data_len );
  BT_HC_DRV_TX_TRACE( hci_acl_dsm_ptr );

#ifdef FEATURE_BT_AVSYNC_SOC
  /* Update the Transmit status of the ACL packet being sent to the SoC */
  bt_hc_drv_update_tx_status(conn_hndl_idx);
#endif /* FEATURE_BT_AVSYNC_SOC */

  /* Send serial data containing ACL packet to sio to be sent to HCtrlr */
  bt_hc_drv_sio_wakeup_and_transmit( bt_dc.sio_stream_id_ext_soc, hci_acl_dsm_ptr );

  /* Update the H2C flow control parameters to indicate that one
     HCI buffer for this connection handle is used up in Controller */
  bt_hc_drv_tx_update_h2c_flow_params( conn_hndl_idx );

} /* bt_hc_drv_send_acl_data */


/*==========================================================================

FUNCTION
  BT_HC_DRV_TRANSMIT_ACL_DATA_FROM_WM

DESCRIPTION
  This funtion processes the Number of completed packets event from the
  controller. This tracks the availability of the controller buffer for ACL data packets.

DEPENDENCIES
  None.

INPUTS
  Pointer to the NumCmpltPkts event structure.

RETURN VALUE
  None.

SIDE EFFECTS
  None.

==========================================================================*/

static void bt_hc_drv_transmit_acl_data_from_wm
(
  uint8            conn_hndl_idx
)
{
  /* Convenience pointer */
  dsm_item_type*                   acl_data_dsm_ptr = NULL ;

/*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/
 if ( bt_hc_drv.host_info.hci_tx.acl_tx[conn_hndl_idx].acl_data_tx_wm.current_cnt != 0 )
 {
   /* There is data on the watermark - do Q checking */
   acl_data_dsm_ptr = (dsm_item_type*) q_check(
     bt_hc_drv.host_info.hci_tx.acl_tx[conn_hndl_idx].acl_data_tx_wm.q_ptr );
   ASSERT_DEBUG( acl_data_dsm_ptr != NULL );

   /* Dequeue one HCI ACL packet frm the queue */
   acl_data_dsm_ptr = dsm_dequeue(
     &bt_hc_drv.host_info.hci_tx.acl_tx[conn_hndl_idx].acl_data_tx_wm );

#ifdef FEATURE_BT_HCI_HOST_FCTL
   bt_hc_drv_update_acl_tx_flow( conn_hndl_idx, TRUE );
#endif /* FEATURE_BT_HCI_HOST_FCTL */

   /* Send the ACL packet to Sio*/
   bt_hc_drv_send_acl_data( acl_data_dsm_ptr, conn_hndl_idx );
 }

} /* bt_hc_drv_transmit_acl_data_from_wm */


/*==========================================================================

FUNCTION
  BT_HC_DRV_PROCESS_CMD_FROM_WM

DESCRIPTION
  This funtion processes HCI cmds stored on to the WM when the HCI
  driver is ready to send out anothher HCI cmd.

DEPENDENCIES
  None.

INPUTS
  None.

RETURN VALUE
  None.

SIDE EFFECTS
  None.

==========================================================================*/
void bt_hc_drv_process_cmd_from_wm( void )
{
  dsm_item_type*                   hci_cmd_dsm_ptr;
#ifdef BT_SWDEV_CMD_FLOW_WORKAROUND
  uint16                           hci_opcode;
  uint8                            byte_val;
#endif

  /* If more commands can be sent to the controller and commands
   * are queued in the command water mark, send them out.
   */

  if((bt_hc_drv.host_info.hci_tx.cmd_tx.num_hci_cmd_pkts!= 0) &&
#ifdef BT_SWDEV_CMD_FLOW_WORKAROUND
     (bt_hc_drv.host_info.hci_tx.cmd_tx.expecting_large_evt == FALSE) &&
#endif
     (bt_hc_drv.host_info.hci_tx.cmd_tx.hci_cmd_tx_wm.current_cnt != 0)
     )
  {
    hci_cmd_dsm_ptr =
            dsm_dequeue ( &bt_hc_drv.host_info.hci_tx.cmd_tx.hci_cmd_tx_wm );
    bt_hc_drv.host_info.hci_tx.cmd_tx.num_hci_cmd_pkts --;
    BT_MSG_DEBUG("BT HC Drv: Process Cmd flow send dsm = 0x%x num_cmds = 0x%x",
                  hci_cmd_dsm_ptr,
                  bt_hc_drv.host_info.hci_tx.cmd_tx.num_hci_cmd_pkts, 0);
    bt_hc_drv_log_cmd( hci_cmd_dsm_ptr, dsm_length_packet( hci_cmd_dsm_ptr ));
    BT_HC_DRV_TX_TRACE( hci_cmd_dsm_ptr );

#ifdef BT_SWDEV_CMD_FLOW_WORKAROUND
    /* Byte 1 and 2 contains Hci command opcode, peek the same and
     * Update large event pending status */
    dsm_peek_byte( hci_cmd_dsm_ptr, 2, &byte_val );
    hci_opcode = (byte_val << 8);
    dsm_peek_byte( hci_cmd_dsm_ptr, 1, &byte_val );
    hci_opcode |= byte_val;
    BT_MSG_DEBUG("BT HC Drv: hci_opcode %x",hci_opcode,0,0);
    bt_hc_update_lrg_evt_sts_with_cmd ( hci_opcode );
#endif

    bt_hc_drv_sio_wakeup_and_transmit( bt_dc.sio_stream_id_ext_soc, hci_cmd_dsm_ptr);

#ifdef FEATURE_BT_QSOC_SLEEP
    if ( bt_hc_drv.host_info.hci_tx.cmd_tx.num_hci_cmd_pkts != 0)
    {
      /* HCI Command buffer returned, but nothing to send. */
      bt_qsoc_sleep_sm_eval_event(BT_QSOC_SLEEP_EVENT_HCI_CMD_FLOWED_ON, TRUE);
    }

    if (bt_hc_drv.host_info.hci_tx.cmd_tx.hci_cmd_tx_wm.current_cnt == 0)
    {
      bt_qsoc_sleep_sm_eval_event(BT_QSOC_SLEEP_EVENT_CMD_Q_EMPTY, TRUE);
    }
#endif /* FEATURE_BT_QSOC_SLEEP */

  }
} /* bt_hc_drv_process_cmd_from_wm */


/*==========================================================================

FUNCTION
  BT_HC_DRV_PROCESS_CMD_FLOW

DESCRIPTION
  This funtion processes the events that effect the HCI Command flow,
  Command Complete and Commmand Status Events.
  This tracks the availability of the controller command buffers.

DEPENDENCIES
  None.

INPUTS
  Pointer to the event structure.

RETURN VALUE
  None.

SIDE EFFECTS
  None.

==========================================================================*/
static void bt_hc_drv_process_cmd_flow
(
  bt_ev_msg_type* ev_msg_ptr
)
{
  uint8 num_cmd_pkts;
  /* Convenience pointers*/
  bt_ev_hc_cmd_cmplt_type*         cmd_cmplt_ev_ptr;
  bt_ev_hc_cmd_status_type*        cmd_status_ev_ptr;

  switch (ev_msg_ptr->ev_hdr.ev_type)
  {
    case BT_EV_HC_COMMAND_COMPLETE:
         cmd_cmplt_ev_ptr = &ev_msg_ptr->ev_msg.ev_hc_cmd_cmplt;
         num_cmd_pkts = cmd_cmplt_ev_ptr->num_hci_cmd_pkts;
         break;

    case BT_EV_HC_COMMAND_STATUS:
         cmd_status_ev_ptr = &ev_msg_ptr->ev_msg.ev_hc_cmdst;
         num_cmd_pkts = cmd_status_ev_ptr->num_hci_cmd_pkts;
         break;

    default :
         BT_ERR("BT HC Drv: Invalid Event in cmd_flow_ctrl ev_type = 0x%x",
                 ev_msg_ptr->ev_hdr.ev_type, 0, 0);
         return;
  }
  /* Update the Number of HCI commands that can be sent to the controller */
  bt_hc_drv.host_info.hci_tx.cmd_tx.num_hci_cmd_pkts = num_cmd_pkts;
  BT_MSG_DEBUG("BT HC Drv: Process Cmd Flow current num_cmds = 0x%x",
                num_cmd_pkts, 0, 0);

  bt_hc_drv_process_cmd_from_wm();
} /* bt_hc_drv_process_cmd_flow */


/*==========================================================================

FUNCTION
  BT_HC_DRV_PROCESS_NUM_CMPLT_PKTS_EV

DESCRIPTION
  This funtion processes the Number of completed packets event from the
  controller. This tracks the availability of the controller buffer for ACL data packets.

DEPENDENCIES
  None.

INPUTS
  Pointer to the NumCmpltPkts event structure.

RETURN VALUE
  None.

SIDE EFFECTS
  None.

==========================================================================*/

static void bt_hc_drv_process_num_cmplt_pkts_ev
(
  bt_ev_msg_type*  ev_msg_ptr    /* pointer to BT HCI event */
)
{
  /* Store for parsed Conenction handle from num complete Pkts event. */
  uint16                           conn_hndl;

  /* Store for parsed Number of complete packets from num complt Pkts event. */
  uint16                           num_cmplt_pkts;

   /* Loop counter for connection handles */
  uint8                            chndl;

  /* Index to host connection handle table */
  uint8                            host_chndl_idx;

   /* Convenience pointer */
  bt_ev_hc_num_cmplt_pkts_type*    ev_ptr;

  /* Convenience pointer to event message dsm pointer */
  dsm_item_type*                   ev_msg_dsm_ptr;
/*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/
  /* Initialize convenience pointer to the event message */
  ev_ptr = &ev_msg_ptr->ev_msg.ev_hc_numpk;


  BT_MSG_API_PYLD( "BT HC DRV: HC Ev NumCmpltPk Rx %x",
                   ev_ptr->num_handles, 0, 0 );

  /* bt_ev_hc_num_cmplt_pkts_type is a packed structure and is not double
     word aligned at its dsm_ptr field. So assign to a local dsm pointer
     to avoid misaligned accesses inside dsm utility functions */
  ev_msg_dsm_ptr = ev_ptr->dsm_ptr;

  for( chndl = 0; chndl < ev_ptr->num_handles; chndl++ )
  {
    /* Parse the connection handle from the dsm item */
    bt_hc_drv_dsm_pullup( &ev_msg_dsm_ptr,
                          &conn_hndl,
                          sizeof( conn_hndl ));

    /* Parse the NumCmpltPkts for this connection handle from the dsm item */
    bt_hc_drv_dsm_pullup( &ev_msg_dsm_ptr,
                          &num_cmplt_pkts,
                          sizeof( num_cmplt_pkts ));

    host_chndl_idx = bt_hc_drv_get_conn_hndl_idx( conn_hndl );
    if ( host_chndl_idx != BT_HC_DRV_ACL_TX_INIT_CHNDL )
    {
      /* Update the number of data packets space freed up in Controller */
      bt_hc_drv_rx_update_h2c_flow_params( host_chndl_idx, num_cmplt_pkts );
#ifdef FEATURE_BT_AVSYNC_SOC
      /* Update the ACL Tx frame handle Qs and generate required Num Cmplt
         Events to the Host */
      bt_hc_drv_process_avsync_num_cmplt( host_chndl_idx, conn_hndl,
                                          num_cmplt_pkts );
#endif /* FEATURE_BT_AVSYNC_SOC */
    }

  } /* for chndl */

  /* Set the dsm ptr explicitly to NULL to remove its reference */
  ev_ptr->dsm_ptr = NULL;

  bt_hc_drv_process_acl_from_wm();
} /* bt_hc_drv_process_num_cmplt_pkts_ev */


/*==========================================================================

FUNCTION
  BT_HC_DRV_PROCESS_ACL_FROM_WM

DESCRIPTION
  This function processes ACL data stored on to the WM when the HCI
  driver is ready to send out more ACL data.

DEPENDENCIES
  None.

INPUTS
  None.

RETURN VALUE
  None.

SIDE EFFECTS
  None.

==========================================================================*/
void bt_hc_drv_process_acl_from_wm
(
  void
)
{
  /* Next connection that will be serviced for transmitting data */
  static uint8                     next_conn_idx = 0;

/*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/


  /* HCI buffer space available - try to send any enqueued packets */
  while(( bt_hc_drv.host_info.hci_tx.acl_tx[0].acl_data_tx_wm.current_cnt !=0 ) ||
        ( bt_hc_drv.host_info.hci_tx.acl_tx[1].acl_data_tx_wm.current_cnt !=0 ) ||
        ( bt_hc_drv.host_info.hci_tx.acl_tx[2].acl_data_tx_wm.current_cnt !=0 ) )
  {
    BT_MSG_API_PYLD( "BT HC DRV: WM1 %d, WM2 %d, WM3 %d",
             bt_hc_drv.host_info.hci_tx.acl_tx[0].acl_data_tx_wm.current_cnt,
             bt_hc_drv.host_info.hci_tx.acl_tx[1].acl_data_tx_wm.current_cnt,
             bt_hc_drv.host_info.hci_tx.acl_tx[2].acl_data_tx_wm.current_cnt );

    if( bt_hc_drv.ctrlr_info.cur_buf.tot_free_data_pkts == 0)
    {
      BT_MSG_API_PYLD( "BT HC DRV: No free buffers currently try later %d",
                       bt_hc_drv.ctrlr_info.cur_buf.tot_free_data_pkts,
                       0, 0 );
      break;
    }
    else
    {
        /* Queued items present - send one packet now */
        BT_MSG_API_PYLD( "BT HC DRV: ConnIdx %d,WM current cnt %d",
                         next_conn_idx,
                         bt_hc_drv.host_info.hci_tx.acl_tx[next_conn_idx].acl_data_tx_wm.current_cnt, 0 );

        /* Dequeue from Watermark and send to Ctrlr */
        bt_hc_drv_transmit_acl_data_from_wm( next_conn_idx );

        /* Move to the next connection now */
        next_conn_idx = (next_conn_idx + 1)%BT_MAX_BT_DEV;

    } /* if bt_hc_drv.ctrlr_info.cur_buf.tot_free_data_pkts != 0 */
  } /* while */

#ifdef FEATURE_BT_QSOC_SLEEP
  /* Did the MSM acquire free ACL buffers? */
  if( bt_hc_drv.ctrlr_info.cur_buf.tot_free_data_pkts != 0)
  {
    bt_qsoc_sleep_sm_eval_event(BT_QSOC_SLEEP_EVENT_HCI_ACL_FLOWED_ON, TRUE);
  }

  /* Did the ACL queue go empty? */
  if(( bt_hc_drv.host_info.hci_tx.acl_tx[0].acl_data_tx_wm.current_cnt ==0 ) &&
        ( bt_hc_drv.host_info.hci_tx.acl_tx[1].acl_data_tx_wm.current_cnt ==0 ) &&
        ( bt_hc_drv.host_info.hci_tx.acl_tx[2].acl_data_tx_wm.current_cnt ==0 )  )
  {
      bt_qsoc_sleep_sm_eval_event(BT_QSOC_SLEEP_EVENT_ACL_Q_EMPTY, TRUE);
  }
#endif /* FEATURE_BT_QSOC_SLEEP */

} /* bt_hc_drv_process_acl_from_wm */

#ifdef FEATURE_BT_AVSYNC_SOC
/*==========================================================================

FUNCTION
  BT_HC_DRV_PROCESS_FLUSH_OCCURRED_EV

DESCRIPTION
  This funtion processes the Flush Occurred Event from the Controller, This Event will
  reset the state so that the Host can further flush packets

DEPENDENCIES
  None.

INPUTS
  Pointer to the FlushOccurred event structure.

RETURN VALUE
  None.

SIDE EFFECTS
  None.

==========================================================================*/

static void bt_hc_drv_process_flush_occurred_ev
(
  bt_ev_msg_type*  ev_msg_ptr    /* pointer to BT HCI event */
)
{
  uint8          conn_hndl_idx;

/*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  conn_hndl_idx = bt_hc_drv_get_conn_hndl_idx(
                    ev_msg_ptr->ev_msg.ev_hc_flush.conn_hndl);

  /* Sanity Check for out of bounds array index */
  if( conn_hndl_idx < BT_MAX_BT_DEV )  
  {  
  bt_hc_drv.host_info.hci_tx.acl_tx[conn_hndl_idx].buffer_flushed = FALSE;
}
}
#endif /* FEATURE_BT_AVSYNC_SOC */

/*==========================================================================

                       BT HOST TRANSMIT  FUNCTIONS  -
    Functions involved in transmitting data to the sio interface
    to be sent to the Controller .

    Host hci driver(bthcdrv) ==> sio driver -> physical bus ->  controller

==========================================================================*/
/*==========================================================================

FUNCTION
  BT_HC_DRV_BUILD_HCI_ACL_DATA_PKT

DESCRIPTION
  Builds a HCI ACL data packet.

DEPENDENCIES
  None.

RETURN VALUE
  DSM pointer to HCI ACL data packet.

SIDE EFFECTS
  None.

==========================================================================*/

static dsm_item_type* bt_hc_drv_build_hci_acl_data_pkt
(
  bt_cmd_msg_type*  bt_cmd_ptr
)
{
  /* Length of the header of ACL packet */
  uint8                   acl_pkt_hdr_len;

  /* Pointer to HCI ACL data dsm item to be transmitted */
  dsm_item_type*          hci_acl_dsm_ptr = NULL;

  /* HCI ACL data packet */
  bt_hc_acl_data_type     acl_tx_pkt;

  boolean                 got_dsm;

/*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  /* Build the HCI header for ACL data packet to be transmitted. */
    acl_tx_pkt.acl_data_hdr.pkt_type = BT_HCI_ACL_DATA_PKT;

  /* First, fill in the Connection handle bits */
  acl_tx_pkt.acl_data_hdr.acl_conn_hdr =
    bt_cmd_ptr->cmd_msg.cmd_hc_tx_acl_data.conn_hndl;

  /* Fill in Packet boundary flag */
  if( bt_cmd_ptr->cmd_msg.cmd_hc_tx_acl_data.pkt_boundary_flag ==
                                          BT_HC_PKT_BOUNDARY_START )
  {
    /* Update ACL packet boundary flag bits 13:12 */ 
#ifdef BT_SWDEV_2_1_PB
    if( bt_soc_etc_data.lmp_version < BT_SOC_LMP_VERSION_2_1 )
    {      
      acl_tx_pkt.acl_data_hdr.acl_conn_hdr |=
                                (uint16)( ( (uint16)BT_HC_PKT_BOUNDARY_START_FLUSHABLE) << 0xC );
    }
    else
    {
      acl_tx_pkt.acl_data_hdr.acl_conn_hdr |=
                                (uint16)( ((uint16)BT_HC_PKT_BOUNDARY_START) << 0xC );
    }
#else
    acl_tx_pkt.acl_data_hdr.acl_conn_hdr |=
                                (uint16)( ((uint16)BT_HC_PKT_BOUNDARY_START_FLUSHABLE) << 0xC );
#endif /* BT_SWDEV_2_1_PB */
  }else if ( bt_cmd_ptr->cmd_msg.cmd_hc_tx_acl_data.pkt_boundary_flag ==
                                BT_HC_PKT_BOUNDARY_START_FLUSHABLE )
  {
    acl_tx_pkt.acl_data_hdr.acl_conn_hdr |=
                                (uint16)( ((uint16)BT_HC_PKT_BOUNDARY_START_FLUSHABLE) << 0xC );
  }
  else
  {
    acl_tx_pkt.acl_data_hdr.acl_conn_hdr |=
                                (uint16)( ((uint16)BT_HC_PKT_BOUNDARY_FRAGMENT) << 0xC );
  }

  /* Update ACL broadcast flag bits 15:14 */
  acl_tx_pkt.acl_data_hdr.acl_conn_hdr |=
    (uint16)((uint16)bt_cmd_ptr->cmd_msg.cmd_hc_tx_acl_data.broadcast_flag << 0xE);

  /* Fill in length of ACL data in the packet */
  acl_tx_pkt.acl_data_hdr.data_len =
    dsm_length_packet( bt_cmd_ptr->cmd_msg.cmd_hc_tx_acl_data.dsm_ptr );

  acl_tx_pkt.acl_data_dsm_ptr = bt_cmd_ptr->cmd_msg.cmd_hc_tx_acl_data.dsm_ptr;

  /* Get the size of the ACL data packet header */
  acl_pkt_hdr_len = sizeof( bt_hc_acl_data_hdr_type );

  /* Get a new dsm item to serialize the ACL data packet. */
  hci_acl_dsm_ptr = bt_get_free_dsm_ptr( BT_TL_HCI_BB, acl_pkt_hdr_len);

  /* Push the ACL packet header into the dsm item */
  got_dsm = bt_dsm_pushdown_tail( &hci_acl_dsm_ptr,
                                  (void *)  &acl_tx_pkt.acl_data_hdr,
                                  acl_pkt_hdr_len,
                                  DSM_DS_SMALL_ITEM_POOL);

  if ( (got_dsm == FALSE) || (hci_acl_dsm_ptr == NULL))
  {
    /* ACL header serialization failed */
    BT_ERR( "BT HC DRV : ACL pushdown failed", 0,0,0);
    dsm_free_packet( &hci_acl_dsm_ptr);
  }
  else
  {
    /* ACL header serialized successfuly. Append the data portion to the
       dsm item */
    if ( bt_cmd_ptr->cmd_msg.cmd_hc_tx_acl_data.dsm_ptr != NULL )
    {
      dsm_append( &hci_acl_dsm_ptr,
                  (dsm_item_type **)&acl_tx_pkt.acl_data_dsm_ptr );
    }
    else
    {
      BT_ERR( "BT HC DRV : ACL dsm append failed; Null dsm data ptr", 0,0,0);
      dsm_free_packet( &hci_acl_dsm_ptr);

    } /* bt_cmd_ptr */
  } /* if got_dsm */

  return( hci_acl_dsm_ptr );
} /* bt_hc_drv_build_hci_acl_data_pkt */


/*==========================================================================

  FUNCTION       BT_HC_DRV_Q_TX_ACL_DATA

  DESCRIPTION    Put the data on the HCI Tx ACL data watermark until a buffer
                 is freed up in the Controller. By design, wm is expected to
                 have space. If not, BT_ERR is logged and data dropped.

  DEPENDENCIES   None.

  RETURN VALUE   None.

  SIDE EFFECTS   None.

==========================================================================*/

static void bt_hc_drv_q_tx_acl_data
(
  /* DSM pointer to the ACL data */
  dsm_item_type*  hci_acl_data_dsm_ptr,

  /* Index to connection handle table */
  uint8           conn_hndl_idx
)
{
  /* Length of ACL data packet */
  uint32 data_len;

/*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  data_len = dsm_length_packet( hci_acl_data_dsm_ptr );

  /* Sanity Check for out of bounds array index */
  if( conn_hndl_idx < BT_MAX_BT_DEV )
  {
    /* If the WM has space */
    if((bt_hc_drv.host_info.hci_tx.acl_tx[ conn_hndl_idx ].acl_data_tx_wm.current_cnt + data_len) <=
        bt_hc_drv.host_info.hci_tx.acl_tx[ conn_hndl_idx ].acl_data_tx_wm.dont_exceed_cnt)
    {
      dsm_enqueue( &bt_hc_drv.host_info.hci_tx.acl_tx[ conn_hndl_idx ].acl_data_tx_wm,
                   &hci_acl_data_dsm_ptr );

#ifdef FEATURE_BT_HCI_HOST_FCTL
      bt_hc_drv_update_acl_tx_flow( conn_hndl_idx, FALSE );
#endif /* FEATURE_BT_HCI_HOST_FCTL */
    }
    else
    {
      /* Free the generated dsm item since it cannot be processed now */
      dsm_free_packet( &hci_acl_data_dsm_ptr );

      BT_ERR(" BT HC DRV TX: acl_data_tx WM Full. Data dropped !!", 0, 0, 0);
    }
  }
  else
  {
    BT_ERR("BT HC DRV Failure: Connection index out of bounds %d",
            conn_hndl_idx, 0, 0 ); 
  }  /*  if conn_hndl_idx < BT_MAX_BT_DEV ..*/  
} /* bt_hc_drv_q_tx_acl_data */


/*==========================================================================

FUNCTION
  BT_HC_DRV_TX_IS_HCI_BUF_SPACE_FREE

DESCRIPTION
  Checks if there are any free HCI buffers in Controller by looking up the
  locally maintained structure.

DEPENDENCIES
  None.

INPUTS
  None.

RETURN VALUE
  None.

SIDE EFFECTS
  None.

==========================================================================*/

static boolean bt_hc_drv_tx_is_hci_buf_space_free
(
  /* Index to connection handle table */
  uint8                          conn_hndl_idx
)
{
  /* Buffer free reaturn value */
  boolean is_buf_free = FALSE;

/*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

 /* Sanity Check for out of bounds array index */
  if( conn_hndl_idx < BT_MAX_BT_DEV )
  {
    if( bt_hc_drv.ctrlr_info.cur_buf.entries[conn_hndl_idx].is_buf_guaranteed == TRUE )
    {
      /* Buffers have been guaranteed for guaranteed QOS */
      if(( bt_hc_drv.ctrlr_info.cur_buf.entries[conn_hndl_idx].num_alloc_buf -
           bt_hc_drv.ctrlr_info.cur_buf.entries[conn_hndl_idx].num_buf_used ) > 0 )
      {
        /* Buffer available in the reserved pool */
        is_buf_free = TRUE;
      }
    } /* if is_buf_guaranteed */
    else
    {
      /* Buffers haven't been guaranteed */
      if( bt_hc_drv.ctrlr_info.cur_buf.tot_free_data_pkts !=0 )
      {
        /* Buffer available in the free pool */
        is_buf_free = TRUE;
      }
    }
  }
  else
  {
    BT_ERR("BT HC DRV Failure: Connection index out of bounds %d",
            conn_hndl_idx, 0, 0 ); 
  } /*  if conn_hndl_idx < BT_MAX_BT_DEV  */
  
  return( is_buf_free );
} /* bt_hc_drv_tx_is_hci_buf_space_free */


/*==========================================================================

FUNCTION
  BT_HC_DRV_PROCESS_TX_ACL_DATA

DESCRIPTION
  Processes ACL data to be transmitted to Controller. This checks to see if a
  Controller ACL data buffer is ready to store this packet. If yes, this sends
  it off to the Controller through the sio driver. If not, it is put on the
  HCI Tx ACL data watermark until a buffer is freed up in the Controller.

DEPENDENCIES
  None.

INPUTS
  None.

RETURN VALUE
  None.

SIDE EFFECTS
  None.

==========================================================================*/

static void bt_hc_drv_process_tx_acl_data
(
  bt_cmd_msg_type*  bt_cmd_ptr
)
{
  /* Convenience pointer */
  dsm_item_type*  hci_acl_data_dsm_ptr;

  /* Index to connection handle table */
  uint8          conn_hndl_idx;

  /* Connection complete event information */
  bt_ev_hc_conn_cmplt_type   conn_cp_msg;
/*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  /* Serialize the ACL data onto a dsm item */
  hci_acl_data_dsm_ptr = bt_hc_drv_build_hci_acl_data_pkt( bt_cmd_ptr );

  /* Get the index corresponding to this conenction handle */
  conn_hndl_idx = bt_hc_drv_get_conn_hndl_idx(
                    bt_cmd_ptr->cmd_msg.cmd_hc_tx_acl_data.conn_hndl );

  /* Handle Broadcast Data.
   * Connection Handle management has to be done at the Host,
   * add the assign Connection Handle entry in the HC drivers Controller Info.
   * To Do : When Broadcast is supprted in the Host
   *   - Broadcast Connection Handle Management
   *   - Connection Handle Conflict resolution.
   */
  if ( bt_cmd_ptr->cmd_msg.cmd_hc_tx_acl_data.broadcast_flag
                                      != BT_HC_BROADCAST_NONE )
  {
    BT_MSG_DEBUG("Broadcast Data : CH = 0x%x, PB = 0x%x",
                  bt_cmd_ptr->cmd_msg.cmd_hc_tx_acl_data.conn_hndl,
                  bt_cmd_ptr->cmd_msg.cmd_hc_tx_acl_data.broadcast_flag, 0);
    if ( conn_hndl_idx == BT_HC_DRV_ACL_TX_INIT_CHNDL )
    {
      /* Add an entry for this kind of boradcast */
      conn_cp_msg.conn_hndl = bt_cmd_ptr->cmd_msg.cmd_hc_tx_acl_data.conn_hndl;
      memset( (void *)&conn_cp_msg.bd_addr, 0, sizeof (bt_bd_addr_type) ) ;
      BT_MSG_DEBUG("Add CH 0x%x entry for Broadcast",
                    bt_cmd_ptr->cmd_msg.cmd_hc_tx_acl_data.conn_hndl, 0, 0);
      conn_hndl_idx = bt_hc_drv_update_new_conn( conn_cp_msg );
    }
  }

  if ( conn_hndl_idx == BT_HC_DRV_ACL_TX_INIT_CHNDL )
  {
    BT_ERR( "BT HC DRV: New ACL not supported; 3 ACL already up", 0, 0, 0 );
  } /* if conn_hndl_idx == BT_HC_DRV_ACL_TX_INIT_CHNDL */
  else if ( conn_hndl_idx >= BT_MAX_BT_DEV )
  {
    BT_ERR("BT HC DRV Failure: Connection index out of bounds %d",
            conn_hndl_idx, 0, 0 );
  } /* if  conn_hndl_idx >= BT_MAX_BT_DEV  */    
  else
  {
#ifdef FEATURE_BT_AVSYNC_SOC
    /* Queue the Packet handle so as to match in the transmit complete */
     bt_hc_drv_enque_acl_data(bt_cmd_ptr, conn_hndl_idx);
#endif /* FEATURE_BT_AVSYNC_SOC */
    if (( bt_hc_drv_tx_is_hci_buf_space_free( conn_hndl_idx ) == TRUE )      &&
        ( bt_hc_drv.host_info.hci_tx.acl_tx[conn_hndl_idx].acl_data_tx_wm.current_cnt
          == 0 ))
    {
      /* HostCtrl has an ACL buffer free, transmit this ACL packet to sio */
      bt_hc_drv_send_acl_data( hci_acl_data_dsm_ptr, conn_hndl_idx );
    }
    else
    {
      /* Enqueue the ACL data packet to be sent later */
      bt_hc_drv_q_tx_acl_data( hci_acl_data_dsm_ptr, conn_hndl_idx );

#ifdef FEATURE_BT_QSOC_SLEEP
      bt_qsoc_sleep_sm_eval_event(BT_QSOC_SLEEP_EVENT_ACL_Q_NONEMPTY, TRUE);
#endif /* FEATURE_BT_QSOC_SLEEP */
    }
  } /* if conn_hndl_idx != BT_HC_DRV_ACL_TX_INIT_CHNDL */
} /* bt_hc_drv_process_tx_acl_data */


/*==========================================================================

FUNCTION
  BT_HC_DRV_GET_CMD_INDEX

DESCRIPTION
  Finds the index to the given command in the command table.

DEPENDENCIES
  None.


INPUTS
  None.

RETURN VALUE
  Command Index.

SIDE EFFECTS
  None.

==========================================================================*/

static uint8 bt_hc_drv_get_cmd_index
(
  bt_cmd_type         cmd_type
)
{
  uint8 cmd_table_idx = 0; /* Index into the comamnd table */
  uint8 loop_idx      = 0; /* Loop index */

/*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/
  /* Commands are ordered from BT_CMD_EV_HC_BASE */
  cmd_table_idx = cmd_type - BT_CMD_EV_HC_BASE;

  /* Sanity Check for out of bounds array index */
  if ( ( cmd_table_idx < BT_HC_DRV_CMD_TABLE_NUM_ENTRIES ) && 
       ( cmd_type >= BT_CMD_EV_HC_BASE ) )
  {
    /* Check if the table entry matches with the given command, otherwise
     use loop through to find the right entry. */
    if ( bt_hc_drv_cmd_table[cmd_table_idx].bt_cmd != cmd_type)
    {
      /* Table entry mismatch - loop to find correct entry for the command.*/
      for( loop_idx = 0; loop_idx < BT_HC_DRV_CMD_TABLE_NUM_ENTRIES; loop_idx++ )
      {
        if( bt_hc_drv_cmd_table[loop_idx].bt_cmd == cmd_type )
        {
          /* Match found - return the command index in the table. */
          cmd_table_idx = loop_idx;
          break;
        }
      }
    } /* if bt_hc_drv_cmd_table[cmd_table_idx].bt_cmd != cmd_type */
  }
  else
  {
    /* Table entry mismatch - loop to find correct entry for the command.*/
    for( loop_idx = 0; loop_idx < BT_HC_DRV_CMD_TABLE_NUM_ENTRIES; loop_idx++ )
    {
      if( bt_hc_drv_cmd_table[loop_idx].bt_cmd == cmd_type )
      {
        break;
      }
    }
    /* Search finished - return the command index in the table. */
    cmd_table_idx = loop_idx;
  } /* if  cmd_table_idx < BT_HC_DRV_CMD_TABLE_NUM_ENTRIES */

  return ( cmd_table_idx );

} /* bt_hc_drv_get_cmd_index */


/*==========================================================================

FUNCTION
  BT_HC_DRV_GET_CMD_PARAM_LEN

DESCRIPTION
  Get the parameter length of the HCI_set_event_filter command. This command has
  a nested union and therefore has to be handled specially.

DEPENDENCIES
  None.

INPUTS
  None.

RETURN VALUE
  cmd_param_len - HCI_set_event_filter parameter length in bytes.

SIDE EFFECTS
  None.

==========================================================================*/

static uint8 bt_hc_drv_get_ev_filt_cmd_param_len
(
  /* Pointer to the Bluetooth command */
  bt_cmd_msg_type*  bt_cmd_ptr
)
{
  /* Command parameter length */
  uint8                           cmd_param_len = 0;

  /* Local refernece of event filter command */
  bt_cmd_hc_set_event_filter_type ev_filt;

/*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/
  /* Convenience initialization to the event_filter command */
  ev_filt = bt_cmd_ptr->cmd_msg.cmd_hc_set_event_filter;

  switch( ev_filt.filter_type )
  {
    case BT_FILTER_CLEAR_ALL:
      /* Clear all filters filter type. No filter conditions and condition
         parameters needed */
      cmd_param_len = sizeof( bt_filter_type );
      break;

    case BT_FILTER_INQUIRY_RESULT:
      /* Inquiry result filter type. Different filter conditions are handled
         below */
      if ( ev_filt.filter_cond_type.inq_filter_cond == BT_INQUIRY_NEW_DEVICE )
      {
        /* Return all devices from inquiry filter condition */
        cmd_param_len = sizeof( bt_filter_type ) +
          sizeof( bt_inquiry_filter_cond_type );
      }
      else if( ev_filt.filter_cond_type.inq_filter_cond ==
               BT_INQUIRY_FILTER_CLASS_OF_DEVICE )
      {
        /* Device with specific class of device inquiry filter condition */
        cmd_param_len = sizeof( bt_filter_type ) +
          sizeof( bt_inquiry_filter_cond_type ) +
          sizeof( bt_inquiry_filter_class_of_device_cond_type );
      }
      else if ( ev_filt.filter_cond_type.inq_filter_cond ==
               BT_INQUIRY_FILTER_BD_ADDR )
      {
        /* Device with specific BD_ADDR inquiry filter condition */
        cmd_param_len = sizeof( bt_filter_type ) +
            sizeof( bt_inquiry_filter_cond_type ) +
            sizeof( bt_bd_addr_type );
      }
      else
      {
        /* reserved for future use - do nothing */
      }
      break;

    case BT_FILTER_CONN_SETUP:
      /* Connection set-up filter type Different filter conditions are handled
         below */

      if ( ev_filt.filter_cond_type.conn_filter_cond ==
           BT_CONN_FILTER_ALL_DEVICES )
      {
        /* Allow connections from all devices */
        cmd_param_len = 3;
      }
      else if ( ev_filt.filter_cond_type.conn_filter_cond ==
                BT_CONN_FILTER_CLASS_OF_DEVICE )
      {
        /* Allow connections from a device with specific class of device */
        cmd_param_len = sizeof( bt_filter_type ) +
            sizeof( bt_conn_filter_cond_type ) +
            sizeof( bt_conn_filter_class_of_device_cond_type );
      }
      else if( ev_filt.filter_cond_type.conn_filter_cond ==
               BT_CONN_FILTER_BD_ADDR )
      {
        /* Allow connections from a device with specific BD_ADDR */
        cmd_param_len = sizeof( bt_filter_type ) +
          sizeof( bt_conn_filter_cond_type ) +
          sizeof( bt_conn_filter_bd_addr_cond_type );
      }
      else
      {
        /* reserved for future use - do nothing */
      }
      break;
    default:
      break;
  } /* switch( filter_type )*/

  return( cmd_param_len );

} /* bt_hc_drv_get_ev_filt_cmd_param_len */


/*==========================================================================

FUNCTION
  BT_HC_DRV_GET_CMD_PARAM_LEN

DESCRIPTION
  Get the parameter length of the command. This is used to get the parameter
  length of commands which do NOT use dsm items. For most comamnds it is a direct
  function of the struture size. Some commands are handled separately since
  the parameter length is not equal to the structure size - an example is
  HCI_set_event_filter comamnd which uses a nested union.

DEPENDENCIES
  None.

INPUTS
  None.

RETURN VALUE
  cmd_param_len - Command parameter size in bytes.

SIDE EFFECTS
  None.

==========================================================================*/

static uint8 bt_hc_drv_get_cmd_param_len
(
  /* Pointer to the Bluetooth command */
  bt_cmd_msg_type*  bt_cmd_ptr
)
{
  /* Index of a command in the HCI command table. */
  uint8                           cmd_idx;

  /* Command parameter length in bytes. */
  uint8                           cmd_param_len = 0;

/*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  switch( bt_cmd_ptr->cmd_hdr.cmd_type )
  {
    /* Handle commands with special command length settings */

    case BT_CMD_HC_SET_EVENT_FILTER :
      cmd_param_len = bt_hc_drv_get_ev_filt_cmd_param_len( bt_cmd_ptr );
      break;

    default:
      /* Get the index to the command table for the current command */
      cmd_idx = bt_hc_drv_get_cmd_index( bt_cmd_ptr->cmd_hdr.cmd_type );

      /* Sanity Check for out of bounds array index */
      if ( cmd_idx < BT_HC_DRV_CMD_TABLE_NUM_ENTRIES )
      {
        /* Get the command parameter length from the command table */
        cmd_param_len = bt_hc_drv_cmd_table[cmd_idx].param_len;
      }
      else
      {
        BT_ERR("BT HC DRV Failure: Command Table index out of bounds ind:%d type:%d",
                cmd_idx, bt_cmd_ptr->cmd_hdr.cmd_type, 0 ); 
      } /* if  cmd_idx < BT_HC_DRV_CMD_TABLE_NUM_ENTRIES  */      
      break;
  } /* switch( bt_cmd_ptr->cmd_hdr.cmd_type ) */

  return( cmd_param_len );

} /* bt_hc_drv_get_cmd_param_len */


/*==========================================================================

FUNCTION
  BT_HC_DRV_SEND_CMD

DESCRIPTION
  Send the HCI comamnd to host controller.

DEPENDENCIES
  None.

INPUTS
  None.

RETURN VALUE
  None.

SIDE EFFECTS
  None.

==========================================================================*/

void bt_hc_drv_send_cmd
(
  bt_cmd_msg_type*  bt_cmd_ptr
)
{
  uint8                                cmd_idx;
  const bt_hc_drv_cmd_tbl_struct_type *cmd_ptr;
  dsm_item_type*                       dsm_temp_ptr   = NULL;
  dsm_item_type*                       dsm_temp_ptr1  = NULL;
  dsm_item_type*                       dsm_ptr_params = NULL;
  dsm_item_type*                       dsm_ptr_hdr    = NULL;
  bt_hc_cmd_type                       bt_hc_cmd;
  boolean                              got_dsm;
  boolean                              packet_duped   = FALSE;
  uint16                               read_bytes;
  uint16                               data_len = 0;

/*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  /* Get the index to the command table for the current command */
  cmd_idx = bt_hc_drv_get_cmd_index( bt_cmd_ptr->cmd_hdr.cmd_type );

   /* Sanity Check for out of bounds array index */
  if ( cmd_idx < BT_HC_DRV_CMD_TABLE_NUM_ENTRIES )
  {
    /* Initialize convenience pointer */
    cmd_ptr = &bt_hc_drv_cmd_table[cmd_idx];

    /* Update the HCI command opcode and HCI packet type */
    bt_hc_cmd.bt_hc_cmd_hdr.pkt_type  = BT_HCI_CMD_PKT;

    /* Special handling for BT_CMD_HC_VS_ANY. See bt_hc_cmd_hdr data           */
    /* structure defined in btcmdi.h. BT_CMD_HC_VS_ANY actually represents all */
    /* possible VS copcode combination (0xFC00 | <10 bit OCF>).                */
    /* bt_hc_drv_cmd_table cannot be used to get the opcode to be sent in the  */
    /* HCI cmd (unlike all other HCI cmds handled in this module). Therefore   */
    /* the 16 bit HCI opcode is passed as a field in the command structure.    */

    if ( bt_cmd_ptr->cmd_hdr.cmd_type == BT_CMD_HC_VS_ANY )
    {
      bt_hc_cmd.bt_hc_cmd_hdr.opcode = bt_cmd_ptr->cmd_msg.cmd_hc_vs_any.opcode;
    }
    else
    {
      bt_hc_cmd.bt_hc_cmd_hdr.opcode = cmd_ptr->opcode;
    }

    /* Get a dsm to fill in command parameters later */

    if ( cmd_ptr->cmd_len_mode == BT_CMD_VAR_LENGTH )
    {
      /***************************************
       Build a variable length command packet
       ****************************************/

      /* get the length of the fixed length portion of command */
      bt_hc_cmd.bt_hc_cmd_hdr.param_len =  cmd_ptr->param_len -
                                                         sizeof(dsm_item_type*);

      /* Temporary dsm pointer to manipulate dsms in command structure */
      dsm_temp_ptr1 = bt_get_free_dsm_ptr( BT_TL_HCI_BB, cmd_ptr->param_len );

      /* Copy the fixed length portion and address of dsm containing
         the variable length parameters into the temp dsm item. */
      got_dsm       = bt_dsm_pushdown_tail( &dsm_temp_ptr1,
                                          (void *)&bt_cmd_ptr->cmd_msg,
                                          cmd_ptr->param_len,
                                          DSM_DS_SMALL_ITEM_POOL );

      /* If pushdown was successful continue. */
      ASSERT_DEBUG( got_dsm == TRUE );

      /* Throw away the fixed length header and retain the address to dsm */
      read_bytes = bt_hc_drv_dsm_pullup( &dsm_temp_ptr1,
                                         NULL,
                                         bt_hc_cmd.bt_hc_cmd_hdr.param_len );
      ASSERT_DEBUG( read_bytes == bt_hc_cmd.bt_hc_cmd_hdr.param_len );

      /* Assign the variable length region to another temp dsm*/

      read_bytes = bt_hc_drv_dsm_pullup( &dsm_temp_ptr1,
                                         &dsm_temp_ptr,
                                         sizeof(dsm_item_type*));

      /* Error checking for the above operations */
      ASSERT_DEBUG( read_bytes    == sizeof(dsm_item_type*));

      dsm_temp_ptr1 = NULL;
      ASSERT_DEBUG( dsm_temp_ptr  != NULL );

      if (bt_hc_cmd.bt_hc_cmd_hdr.param_len != 0)
      {
        dsm_ptr_params = bt_get_free_dsm_ptr( BT_TL_HCI_BB, cmd_ptr->param_len);
        got_dsm = bt_dsm_pushdown_tail( &dsm_ptr_params,
                                        (void *)&bt_cmd_ptr->cmd_msg,
                                        bt_hc_cmd.bt_hc_cmd_hdr.param_len,
                                        DSM_DS_SMALL_ITEM_POOL );
        ASSERT_DEBUG( got_dsm == TRUE );

        dsm_dup_packet(&dsm_temp_ptr1, dsm_temp_ptr, 0,
                       dsm_length_packet(dsm_temp_ptr));
        packet_duped = TRUE;
        /* Append the variable length parameter to the fixed length param dsm */
        dsm_append( &dsm_ptr_params, &dsm_temp_ptr1 );
        ASSERT_DEBUG( dsm_temp_ptr1   == NULL );
        ASSERT_DEBUG( dsm_ptr_params != NULL );
  
      }
      else
      {
        dsm_dup_packet(&dsm_temp_ptr1, dsm_temp_ptr, 0,
                       dsm_length_packet(dsm_temp_ptr));
        packet_duped = TRUE;
        dsm_ptr_params = dsm_temp_ptr1;
      }
 
      bt_hc_cmd.bt_hc_cmd_hdr.param_len =  dsm_length_packet( dsm_ptr_params );

    } /* if ( cmd_ptr->cmd_len_mode == BT_CMD_VAR_LENGTH ) */
    else
    {
      /* Fixed length command */
      dsm_ptr_params = bt_get_free_dsm_ptr( BT_TL_HCI_BB, cmd_ptr->param_len);
  
      bt_hc_cmd.bt_hc_cmd_hdr.param_len =
      bt_hc_drv_get_cmd_param_len( bt_cmd_ptr );

      got_dsm = bt_dsm_pushdown_tail( &dsm_ptr_params,
                                      (void *)&bt_cmd_ptr->cmd_msg,
                                      bt_hc_cmd.bt_hc_cmd_hdr.param_len,
                                      DSM_DS_SMALL_ITEM_POOL );

      ASSERT_DEBUG( got_dsm == TRUE );

    } /* else ( cmd_ptr->cmd_len_mode == BT_CMD_FIXED_LEN )*/

    /* Get dsm for command header information. */
    dsm_ptr_hdr    = bt_get_free_dsm_ptr( BT_TL_HCI_BB,
                                          sizeof(bt_hc_cmd.bt_hc_cmd_hdr));

    bt_dsm_pushdown_tail( &dsm_ptr_hdr,
                         (void *)&(bt_hc_cmd.bt_hc_cmd_hdr),
                          sizeof(bt_hc_cmd.bt_hc_cmd_hdr),
                          DSM_DS_SMALL_ITEM_POOL );


    if ( dsm_ptr_params != NULL )
    {
      /* Don't append if there are no bytes in the dsm */
      if ( ( dsm_length_packet( dsm_ptr_params ) !=0 ) )
      {
        dsm_append( &dsm_ptr_hdr, &dsm_ptr_params );
        ASSERT_DEBUG( dsm_ptr_params == NULL );
      }
      else
      {
        dsm_free_packet( &dsm_ptr_params );
      }
    }
    else
    {
      BT_ERR( "BT HC DRV: Command params NULL", 0, 0, 0);
    }

#if (!(defined(FEATURE_BT_SOC1) || defined(FEATURE_BT_QSOC)))
    if ( ( bt_hc_cmd.bt_hc_cmd_hdr.opcode & BT_HCI_VS_GROUP ) == BT_HCI_VS_GROUP)
    {
      dsm_free_packet( &dsm_ptr_hdr );
      BT_MSG_DEBUG( "BT HC DRV: QC HCI cmd supressed,OP 0x%X,len %d",
                     bt_hc_cmd.bt_hc_cmd_hdr.opcode,
                     bt_hc_cmd.bt_hc_cmd_hdr.param_len,
                     0 );
    }
    else
#endif /* !(defined(FEATURE_BT_SOC1) || defined(FEATURE_BT_QSOC)) */
    {
      data_len = dsm_length_packet( dsm_ptr_hdr );
#ifdef FEATURE_BT_SOC1
#error code not present
#endif /* FEATURE_BT_SOC1 */
      {
        BT_MSG_HIGH( "BT HC DRV TX: HCI Cmd, Op 0x%X,ParLen %d,TotLen %d",
                      bt_hc_cmd.bt_hc_cmd_hdr.opcode,
                      bt_hc_cmd.bt_hc_cmd_hdr.param_len,
                      data_len );
      }

      bt_hc_drv.host_info.bt_hc_drv_txrx_cnt.hci_tx_cmd_cnt++;

#ifdef BT_SWDEV_CMD_FLOW_WORKAROUND
      /* check if large event expecting flag need to be cleared */
      if ( bt_hc_drv.host_info.hci_tx.cmd_tx.expecting_large_evt == TRUE )
      {
        bt_hc_update_lrg_evt_sts_with_cmd( bt_hc_cmd.bt_hc_cmd_hdr.opcode );
      }
#endif

      if((bt_hc_drv.host_info.hci_tx.cmd_tx.num_hci_cmd_pkts != 0) &&
#ifdef BT_SWDEV_CMD_FLOW_WORKAROUND
         (bt_hc_drv.host_info.hci_tx.cmd_tx.expecting_large_evt == FALSE) &&
#endif
         (bt_hc_drv.host_info.hci_tx.cmd_tx.hci_cmd_tx_wm.current_cnt == 0 )
#ifdef FEATURE_BT_HCI_HOST_FCTL
         ||
         ( IS_HOST_NUM_CMPLT_PKTS_CMD( bt_cmd_ptr->cmd_hdr.cmd_type ) == TRUE )
#endif /* FEATURE_BT_HCI_HOST_FCTL */
#ifdef FEATURE_BT_SOC1
#error code not present
#endif /* FEATURE_BT_SOC1 */
        )
      {
        bt_hc_drv_log_cmd( dsm_ptr_hdr, data_len );
        BT_HC_DRV_TX_TRACE( dsm_ptr_hdr );

#ifdef BT_SWDEV_CMD_FLOW_WORKAROUND
        /* update large event expecting flag */
        bt_hc_update_lrg_evt_sts_with_cmd( bt_hc_cmd.bt_hc_cmd_hdr.opcode );
#endif

#ifdef FEATURE_BT_SOC1
#error code not present
#endif /* FEATURE_BT_SOC1 */
        {
#ifdef FEATURE_BT_HCI_HOST_FCTL
          /* Host Num completed pkts does not affect HCI Cmd flow control */
          if( IS_HOST_NUM_CMPLT_PKTS_CMD( bt_cmd_ptr->cmd_hdr.cmd_type ) ==
              FALSE )
#endif /* FEATURE_BT_HCI_HOST_FCTL */
          {
            bt_hc_drv.host_info.hci_tx.cmd_tx.num_hci_cmd_pkts --;

            BT_MSG_DEBUG( "BT HC DRV TX: Cmd Tx to SIO num_cmds = 0x%x,OP 0x%x "
                           "Txcmd# %d",
                           bt_hc_drv.host_info.hci_tx.cmd_tx.num_hci_cmd_pkts,
                           bt_hc_cmd.bt_hc_cmd_hdr.opcode,
                           bt_hc_drv.host_info.bt_hc_drv_txrx_cnt.hci_tx_cmd_cnt
                        );
          } /* if( IS_HOST_NUM_CMPLT_PKTS_CMD == FALSE ) */
        }

        if( packet_duped == TRUE )
        {
          /* Free the duped dsm */
          dsm_free_packet( &dsm_temp_ptr );
        }
  
        /* HostController has Command buffers available */
        bt_hc_drv_sio_wakeup_and_transmit( bt_dc.sio_stream_id_ext_soc, dsm_ptr_hdr );


#ifdef FEATURE_BT_QSOC_SLEEP
        if ( bt_hc_drv.host_info.hci_tx.cmd_tx.num_hci_cmd_pkts == 0)
        {
           /* Cmd Flowed off */
          bt_qsoc_sleep_sm_eval_event(BT_QSOC_SLEEP_EVENT_HCI_CMD_FLOWED_OFF, TRUE);
        }
#endif /* FEATURE_BT_QSOC_SLEEP */
      }
      else
      {
        /* If the WM has space */
        if((bt_hc_drv.host_info.hci_tx.cmd_tx.hci_cmd_tx_wm.current_cnt + data_len) <=
            bt_hc_drv.host_info.hci_tx.cmd_tx.hci_cmd_tx_wm.dont_exceed_cnt)
        {
          BT_MSG_DEBUG( "BT HC DRV TX: Enque in HCI CMD Wm cmd_dsm_ptr = 0x%x",
                         dsm_ptr_hdr, 0, 0);
        /* Enqueue the CMD in the Watermark to be sent later */
          dsm_enqueue( &bt_hc_drv.host_info.hci_tx.cmd_tx.hci_cmd_tx_wm,
                       &dsm_ptr_hdr);

          if( packet_duped == TRUE )
          {
            /* Free the duped dsm */
            dsm_free_packet( &dsm_temp_ptr );
          }
#ifdef FEATURE_BT_QSOC_SLEEP
          bt_qsoc_sleep_sm_eval_event(BT_QSOC_SLEEP_EVENT_CMD_Q_NONEMPTY, TRUE);
#endif /* FEATURE_BT_QSOC_SLEEP */
        }
        else
        {
           /* Free the generated dsm item since it cannot be processed now */
          dsm_free_packet( &dsm_ptr_hdr );

          BT_MSG_DEBUG(" BT HC DRV TX: WM Full Retry Later", 0, 0, 0);
          /* WM is full, return back status for the host to retry later */
          bt_cmd_ptr->cmd_hdr.cmd_status = BT_CS_GN_RETRY_CMD_LATER;
        }
      }
    }
  }
  else
  {
    BT_ERR("BT HC DRV : Cmd Send Failed, Cmd Tbl Index out of bounds ind:%d type:%d",
            cmd_idx, bt_cmd_ptr->cmd_hdr.cmd_type , 0 );
    bt_cmd_ptr->cmd_hdr.cmd_status = BT_CS_GN_RETRY_CMD_LATER;
  } /* if  cmd_idx < BT_HC_DRV_CMD_TABLE_NUM_ENTRIES ... */      
} /* bt_hc_drv_send_cmd */


#ifdef FEATURE_BT_SOC1
#error code not present
#endif /*FEATURE_BT_SOC1*/


/*==========================================================================

FUNCTION
  BT_HC_DRV_PROCESS_CMD_PREAMBLE

DESCRIPTION
  Before messages are sent to the host controller, make sure
  SOC is up. If it is not, call bt_dc_enable_driver to bring
  it up first.

DEPENDENCIES
  None.

INPUTS
  None.

RETURN VALUE
  None.

SIDE EFFECTS
  None.

==========================================================================*/
void bt_hc_drv_process_cmd_preamble(void)
{

  BT_MSG_HIGH( "BT HC DRV: Started Driver Bringup ", 0, 0, 0 );
  rex_clr_timer( &bt_soc_driver_idle_timer );
  rex_clr_sigs( &bt_tcb, BT_DRIVER_IDLE_TIMER_SIG );

  if ( bt_dc.driver_state != BT_DS_SOC_DISABLING)
  {
    // driver state is disabled or idle
    bt_dc_enable_driver();
  }
  else
  {
    // The disable driver process was initiated, but not completed.
    // This means SOC has not been shutdown, only SIO maybe sleeping.
    bt_dc_update_sio_state( BT_SIO_AWAKE, TRUE );

    BT_DC_UPDATE_DRIVER_STATE(BT_DS_SOC_ACTIVE);

#ifdef FEATURE_BT_WLAN_COEXISTENCE
    /* Send AFH channel map now since any updates were not sent while
       driver state was not ACTIVE. */
    if( bt_dc.cur_hci_mode == BT_HCIM_OFF )
    {
      (void) bt_cmd_hc_afh_host_chan_class ( bt_soc_etc_data.host_ch_class );
    }
#endif /* FEATURE_BT_WLAN_COEXISTENCE */

    BT_MSG_HIGH( "BT HC DRV: Driver Enabled ", 0,0,0 );
  }
} /* bt_hc_drv_process_cmd_preamble */


/*==========================================================================

FUNCTION
  BT_HC_DRV_PROCESS_CMD

DESCRIPTION
  Processes HCI commands and ACL data to be sent to Host Controller.
  Adds HCI header and sends HCI Command and ACL  packets to host controller.
  Note SCO over HCI is not handled now.

DEPENDENCIES
  SIO port to communicate with Host controller must be open.

INPUTS
  None.


RETURN VALUE
  None.

SIDE EFFECTS
  None.

==========================================================================*/
void bt_hc_drv_process_cmd
(
  bt_cmd_msg_type*  bt_cmd_ptr
)
{

/*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  /* Command status. Default to SUCCESS */
  bt_cmd_ptr->cmd_hdr.cmd_status = BT_CS_GN_SUCCESS;

#ifdef FEATURE_BT_HCI_HOST_FCTL
    /* Check if either the Large or SMALL DSM count is already LOW, If so, don't
       process any HCI commands ahead of the LOW DSM count FCTL command in the queue */
    if ( bt_hc_drv.host_info.is_dsm_cnt_low == TRUE )
    {
      bt_cmd_ptr->cmd_hdr.cmd_status = BT_CS_GN_RETRY_CMD_LATER;
    }
    else
#endif /* FEATURE_BT_HCI_HOST_FCTL */
    {
      /* Process HCI Commands, ACL & SCO data to be transmitted to Controller */
      switch ( bt_cmd_ptr->cmd_hdr.cmd_type )
      {
        case BT_CMD_HC_TX_ACL_DATA:
          /* Process HCI ACL data for transmission. If ACL buffer is unavailable
             in Controller, packets may be queued for later transmission. */
          bt_hc_drv_process_tx_acl_data( bt_cmd_ptr );
          break;

        case BT_CMD_HC_TX_SCO_DATA:
          /* Nothing here since SCO over HCI is not supported */
          break;

        default:
          /* Process and transmit HCI commands */
          bt_hc_drv_send_cmd( bt_cmd_ptr );
          break;

      } /* switch */
    }
} /* bt_hc_drv_process_cmd */


#ifdef FEATURE_BT_HCI_HOST_FCTL
/*==========================================================================

FUNCTION
  BT_HC_DRV_FCTL_PROCESS_CMD

DESCRIPTION
  Processes HCI Flow control commands that take appropriate flow control
  action based on DSM Item pools levels.

DEPENDENCIES
  None.

INPUTS
  None.

RETURN VALUE
  None.

SIDE EFFECTS
  None.

==========================================================================*/
void bt_hc_drv_fctl_process_cmd
(
  bt_cmd_msg_type*  bt_cmd_ptr
)
{

/*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  /* Process HCI FCTL Commands*/
  switch ( bt_cmd_ptr->cmd_hdr.cmd_type )
  {
    case BT_CMD_FCTL_SMALL_DSM_HIGH_CNT:
      bt_hc_drv_fctl_small_dsm_high();
      bt_cmd_ptr->cmd_hdr.cmd_status = BT_CS_GN_SUCCESS;
      break;

    case BT_CMD_FCTL_SMALL_DSM_LOW_CNT:
      bt_hc_drv_fctl_small_dsm_low();
      bt_cmd_ptr->cmd_hdr.cmd_status = BT_CS_GN_SUCCESS;
      break;

    case BT_CMD_FCTL_LARGE_DSM_HIGH_CNT:
      bt_hc_drv_fctl_large_dsm_high();
      bt_cmd_ptr->cmd_hdr.cmd_status = BT_CS_GN_SUCCESS;
      break;

    case BT_CMD_FCTL_LARGE_DSM_LOW_CNT:
      bt_hc_drv_fctl_large_dsm_low();
      bt_cmd_ptr->cmd_hdr.cmd_status = BT_CS_GN_SUCCESS;
      break;

    default:
      bt_cmd_ptr->cmd_hdr.cmd_status = BT_CS_GN_UNRECOGNIZED_CMD;
      BT_ERR( "BT HC DRV FCTL: Invalid Cmd type: %d",bt_cmd_ptr->cmd_hdr.cmd_type,0 ,0 );
      break;

  } /* switch */

} /* bt_hc_drv_fctl_process_cmd */
#endif /* FEATURE_BT_HCI_HOST_FCTL */


/*==========================================================================

                       BT HOST TRANSMIT RECEIVE FUNCTIONS
    Functions involved in receiving data which was sent by the Controller.
    Data is fetched from the sio interface.

    host hci driver <== sio driver <- physical bus <-  controller

==========================================================================*/
 /*===========================================================================

FUNCTION
  BT_HC_DRV_TX_STATE_CHANGE

DESCRIPTION
  Changes the processing state of HCI driver.

DEPENDENCIES
  None.

INPUTS
  New HCI Tx state.

RETURN VALUE
  None.

SIDE EFFECTS
  None.

===========================================================================*/
void bt_hc_drv_tx_state_change
(
  bt_hc_drv_state_enum_type  hci_tx_state
    /* Host HCI tx state */
)
{
/*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/
  /* Update the Host HCI tx state */
  bt_hc_drv.host_info.hci_tx.hci_tx_state = hci_tx_state;

} /* bt_hc_drv_tx_state_change */


/*===========================================================================

FUNCTION
  BT_HC_DRV_GET_HCI_RX_STATE

DESCRIPTION
  Get the Rx processing state of HCI driver.

DEPENDENCIES
  None.

INPUTS
  None.

RETURN VALUE
  Rx processing state of HCI driver.

SIDE EFFECTS
  None.

===========================================================================*/
bt_hc_drv_state_enum_type bt_hc_drv_get_hci_rx_state( void )
{
/*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  return( bt_hc_drv.host_info.hci_rx.hci_rx_state );

} /* bt_hc_drv_get_hci_rx_state */


/*===========================================================================

FUNCTION
  BT_HC_DRV_RX_STATE_CHANGE

DESCRIPTION
  Changes the Rx processing state of HCI driver.

DEPENDENCIES
  None.

INPUTS
  New HCI Rx state.

RETURN VALUE
  None.

SIDE EFFECTS
  None.

===========================================================================*/
void bt_hc_drv_rx_state_change
(
  bt_hc_drv_state_enum_type  hci_rx_state
    /* Host HCI Rx state */
)
{
/*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/
  /* Update the Host HCI Rx state */
  bt_hc_drv.host_info.hci_rx.hci_rx_state = hci_rx_state;

} /* bt_hc_drv_rx_state_change */

/*===========================================================================

FUNCTION
  BT_HC_DRV_RX_ACL_PKT_STATE_CHANGE

DESCRIPTION
  Changes the state of the engine that is building the received HCI ACL packet.

DEPENDENCIES
  None.

INPUTS
  New HCI Rx ACL packet state.

RETURN VALUE
  None.

SIDE EFFECTS
  None.

===========================================================================*/
void bt_hc_drv_rx_acl_pkt_state_change
(
  bt_hc_drv_acl_rx_state_enum_type  rx_hci_acl_pkt_state
    /* Host HCI Rx ACL packet state */
)
{
/*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/
  /* Update the Host HCI Rx ACL packet state */
  bt_hc_drv.host_info.hci_rx.rx_pkt_state.rx_acl_pkt_state =
    rx_hci_acl_pkt_state;

} /* bt_hc_drv_rx_acl_pkt_state_change */


/*===========================================================================

FUNCTION
  BT_HC_DRV_GET_RX_ACL_PKT_STATE

DESCRIPTION
  Returns the state of the engine that is building the received HCI ACL
  packet.

DEPENDENCIES
  None.

INPUTS
  None.

RETURN VALUE
  New HCI Rx ACL packet state.

SIDE EFFECTS
  None.

===========================================================================*/
bt_hc_drv_acl_rx_state_enum_type bt_hc_drv_get_rx_acl_pkt_state( void )
{
/*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/
  /* Return the Host HCI Rx ACL packet state */
  return( bt_hc_drv.host_info.hci_rx.rx_pkt_state.rx_acl_pkt_state );

} /* bt_hc_drv_get_rx_ev_pkt_state */


/*===========================================================================

FUNCTION
  BT_HC_DRV_RX_EV_PKT_STATE_CHANGE

DESCRIPTION
  Changes the state of the engine that is building the received HCI event
  packet.

DEPENDENCIES
  None.

INPUTS
  New HCI Rx Event packet state.

RETURN VALUE
  None.

SIDE EFFECTS
  None.

===========================================================================*/
void bt_hc_drv_rx_ev_pkt_state_change
(
  bt_hc_drv_event_state_enum_type  rx_hci_ev_pkt_state
    /* Host HCI Rx Event packet state */
)
{
/*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/
  /* Update the Host HCI Rx event packet state */
  bt_hc_drv.host_info.hci_rx.rx_pkt_state.rx_ev_pkt_state =
                                                            rx_hci_ev_pkt_state;

} /* bt_hc_drv_rx_ev_pkt_state_change */


/*===========================================================================

FUNCTION
  BT_HC_DRV_GET_RX_EV_PKT_STATE

DESCRIPTION
  Returns the state of the engine that is building the received HCI event
  packet.

DEPENDENCIES
  None.

INPUTS
  None.

RETURN VALUE
  New HCI Rx event packet state.

SIDE EFFECTS
  None.

===========================================================================*/
bt_hc_drv_event_state_enum_type bt_hc_drv_get_rx_ev_pkt_state( void )
{
/*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/
  /* Return the Host HCI Rx packet state */
  return( bt_hc_drv.host_info.hci_rx.rx_pkt_state.rx_ev_pkt_state );

} /* bt_hc_drv_get_rx_ev_pkt_state */


/*===========================================================================

FUNCTION
  BT_HC_DRV_SET_HCI_SYNC_STATUS

DESCRIPTION
  Update if HCI of Host and Controller are in sync.

DEPENDENCIES
  None.

INPUTS
  Synchronization status flag.

RETURN VALUE
  Updated synchronization status flag.

SIDE EFFECTS
  None.

===========================================================================*/
boolean bt_hc_drv_set_hci_sync_status
(
  boolean is_hci_synced
    /* Host-HostController HCI Synchronized status flag */
)
{
/*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/
  /* Update the Host HCI structure with the sync status */
  bt_hc_drv.hci_sync_state = is_hci_synced;

  return( bt_hc_drv.hci_sync_state );

} /* bt_hc_drv_set_hci_sync_status */


/*===========================================================================

FUNCTION
  BT_HC_DRV_GET_HCI_SYNC_STATUS

DESCRIPTION
  Get the status on Host-HostController synchronization.

DEPENDENCIES
  None.

INPUTS
  None.

RETURN VALUE
  Synchronization status flag.

SIDE EFFECTS
  None.

===========================================================================*/
boolean bt_hc_drv_get_hci_sync_status( void )
{
/*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  return( bt_hc_drv.hci_sync_state );

} /* bt_hc_drv_get_hci_sync_status */


/*===========================================================================

FUNCTION
  BT_HC_DRV_VERIFY_HCI_SYNC_STATUS

DESCRIPTION
  Verify the status of Host-HostController synchronization. This function takes
  suitable action if sync has been lost.

DEPENDENCIES
  None.

INPUTS
  None.

RETURN VALUE
  Synchronization status flag.

SIDE EFFECTS
  None.

===========================================================================*/
boolean bt_hc_drv_verify_hci_sync_status( void )
{
/*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  if( bt_hc_drv_get_hci_sync_status() == FALSE)
  {
    /* TBD - Single point where SoC is to be reset if needed */

  } /* if */

  return( bt_hc_drv.hci_sync_state );

} /* bt_hc_drv_verify_hci_sync_status */


/*==========================================================================

FUNCTION
  BT_HC_DRV_GET_EVENT_INDEX

DESCRIPTION
  Finds the index to the given command in the command table.

DEPENDENCIES
  None.

INPUTS
  None.

RETURN VALUE
  Event Index.

SIDE EFFECTS
  None.

==========================================================================*/
static uint8 bt_hc_drv_get_event_index
(
  uint8   ev_type
)
{

  uint8 ev_table_idx = 0; /* Index into the event table */
  uint8 loop_idx     = 0; /* Loop index */

/*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/
  /* HCI events are ordered from BT_CMD_EV_HC_BASE. */
  ev_table_idx = ev_type - BT_CMD_EV_HC_BASE;

  /* Check if the table entry matches with the given event, otherwise
     use loop through to find the right entry */
  if ( bt_hc_drv_ev_table[ev_table_idx].hci_ev != ev_table_idx )
  {
    /* Table entry mismatch - loop to find correct entry for the event */
    for ( loop_idx = 0; loop_idx < BT_HC_DRV_EV_TABLE_NUM_ENTRIES; loop_idx++ )
    {
      if ( bt_hc_drv_ev_table[loop_idx].hci_ev == ev_type )
      {
        /* Match found - return the event index in the table */
        ev_table_idx = loop_idx;
        break;
      }
    }
    if ( loop_idx == (sizeof( bt_hc_drv_ev_table ) / sizeof ( bt_hc_drv_ev_struct_type ) ) )
    {
        /* Return last entry */
        ev_table_idx = loop_idx - 1 ;
    }
  } /* if bt_hc_drv_ev_table[loop_idx].hci_ev != ev_type */

  return ( ev_table_idx );

} /* bt_hc_drv_get_event_index */


/*===========================================================================

FUNCTION
  BT_HC_DRV_BUILD_RX_ACL_DATA_PKT

DESCRIPTION
  Builds an ACL data packet from the sio stream coming srom sio.

DEPENDENCIES
  None.

INPUTS
  Pointer to the dsm item containing received HCI ACL packet.

RETURN VALUE
  None.

SIDE EFFECTS
  None.

===========================================================================*/
void bt_hc_drv_build_rx_hci_acl_data_pkt
(
  dsm_item_type** hci_acl_rx_dsm_ptr
)
{
  /* Store for received ACL data packet as it is being parsed */
  static bt_hc_acl_data_type  acl_rx_data;

  /* Dsm item to build received ACL data packet */
  static dsm_item_type*       acl_rx_dsm_ptr = NULL;

  /* Number of bytes pending to be read */
  static uint16               bytes_pending = 0;

  /* ACL data packet to be sent to upper layers */
  bt_ev_msg_type              acl_rx_data_i = {0};

  /* Number of bytes read */
  uint16                      read_bytes;

  /* Length of the ACL header */
  uint16                      acl_hdr_len = sizeof ( bt_hc_acl_data_hdr_type ) -
                                              sizeof( uint8 );
  /* Temporary dsm item */
  dsm_item_type*              temp_dsm_ptr = NULL;

  /* Index into the HCI ACL connection handle table */
  uint8                       conn_idx;

/*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  acl_rx_data_i.ev_hdr.ev_type = BT_EV_HC_RX_ACL_DATA;

  switch ( bt_hc_drv_get_rx_acl_pkt_state() )
  {
    case BT_HCI_ACL_RX_HDR_STATE :

      if ( bt_hc_drv_get_hci_rx_state() == BT_HC_DRV_PKT_PAYLD_STATE )
      {
        bytes_pending    = acl_hdr_len;
      }

      /* Try to parse the pending bytes into another dsm item */
      read_bytes = dsm_dup_packet( &temp_dsm_ptr, *hci_acl_rx_dsm_ptr,
                                   0, bytes_pending );

      /* Pullup bytes read from the HCI RX dsm item */
      bt_hc_drv_dsm_pullup( hci_acl_rx_dsm_ptr, NULL, read_bytes );

      /* Update the number of bytes pending to be parsed yet */
      bytes_pending = bytes_pending - read_bytes;

      /* Keep building ACL data in acl_rx_dsm_ptr. temp_dsm_ptr becomes
         NULL */
      dsm_append( &acl_rx_dsm_ptr, &temp_dsm_ptr );


      if ( bytes_pending == 0 )
      {
        /* ACL header should now be in acl_rx_dsm_ptr. Pull it up. */
        read_bytes = bt_hc_drv_dsm_pullup(
                       &acl_rx_dsm_ptr,
                       (void *)&acl_rx_data.acl_data_hdr.acl_conn_hdr,
                       acl_hdr_len);

        ASSERT_DEBUG( acl_rx_dsm_ptr == NULL );

        /* ACL header pulled sucessfully. Now go to the ACL data state */
        bt_hc_drv_rx_acl_pkt_state_change( BT_HCI_ACL_RX_DATA_STATE );
        bytes_pending = acl_rx_data.acl_data_hdr.data_len;
      }
      else
      {
        BT_MSG_API_PYLD( "BT HC DRV: ACL hdr pullup remaining; Len %d, Rem %d",
                 acl_hdr_len, bytes_pending, 0 );

        bt_hc_drv_rx_state_change( BT_HC_DRV_PKT_PAYLD_CONT_STATE );

        break;
      }  /* else param_bytes_pending != 0*/

      /* Fallthrough */

    case BT_HCI_ACL_RX_DATA_STATE :

      /* Try to read the pending bytes into another dsm */
      read_bytes = dsm_dup_packet( &temp_dsm_ptr, *hci_acl_rx_dsm_ptr,
                                   0, bytes_pending );

      /* Pullup number of bytes read from the Rx dsm item */
      bt_hc_drv_dsm_pullup( hci_acl_rx_dsm_ptr, NULL,
                                          read_bytes );

      /* Update the number of bytes pending to be parsed yet */
      bytes_pending = bytes_pending - read_bytes;

      /* Keep building ACL data in acl_rx_dsm_ptr. temp_dsm_ptr becomes
         NULL */
      dsm_append( &acl_rx_dsm_ptr, &temp_dsm_ptr );

      if ( bytes_pending == 0 )
      {
        acl_rx_data.acl_data_hdr.pkt_type = BT_HCI_ACL_DATA_PKT;

        /* Log the rx ACL packets */
        bt_hc_drv_log_host_rx_acl_data( acl_rx_data.acl_data_hdr,
                                        acl_rx_dsm_ptr );

        /* ACL data should now be in acl_rx_dsm_ptr. Pull it up. */
        acl_rx_data_i.ev_msg.ev_hc_rxda.dsm_ptr = acl_rx_dsm_ptr;

        /* Fill in Connection Handle */
        acl_rx_data_i.ev_msg.ev_hc_rxda.conn_hndl =
         (uint16)( ((uint16)acl_rx_data.acl_data_hdr.acl_conn_hdr)  & 0x0FFF );

        /* Fill in Packet Boundary flag  */
        acl_rx_data_i.ev_msg.ev_hc_rxda.pkt_boundary_flag =
        (bt_hc_pkt_boundary_flag_type)
        ( (((uint16)acl_rx_data.acl_data_hdr.acl_conn_hdr) & 0x3000) >> 0xC );

        /* Fill in Broadcast flag */
        acl_rx_data_i.ev_msg.ev_hc_rxda.broadcast_flag =
        (bt_hc_broadcast_flag_type)
        ( (( (uint16)acl_rx_data.acl_data_hdr.acl_conn_hdr) & 0xC000) >> 0xE );

        /* Fill in length of ACL data */
        acl_rx_data_i.ev_msg.ev_hc_rxda.data_length =
          acl_rx_data.acl_data_hdr.data_len;

        bt_hc_drv.host_info.bt_hc_drv_txrx_cnt.hci_rx_acl_cnt++;
        BT_MSG_API_PYLD( "BT HC DRV RX: AclDatPkt ConnH %d, DL %d RxAcl# %d",
                         acl_rx_data_i.ev_msg.ev_hc_rxda.conn_hndl ,
                         acl_rx_data_i.ev_msg.ev_hc_rxda.data_length,
                         bt_hc_drv.host_info.bt_hc_drv_txrx_cnt.hci_rx_acl_cnt );

       /* Increment the HCI RX frame count in this round of sio processing */
        conn_idx = bt_hc_drv_get_conn_hndl_idx(
                 ( ((uint16)acl_rx_data.acl_data_hdr.acl_conn_hdr) & 0x0FFF) );
        (bt_hc_drv.host_info.hci_rx.hc_rx_acl_cnt[conn_idx])++;

        /* Send the ACL data packet to upper layers */
        bt_hc_send_event( &acl_rx_data_i );

        acl_rx_dsm_ptr = NULL;

        /* Update states to start parsing a new HCI ACL data packet next time */
        bt_hc_drv_rx_acl_pkt_state_change( BT_HCI_ACL_RX_HDR_STATE );
        bt_hc_drv_rx_state_change( BT_HC_DRV_NEW_PKT_STATE );
      } /* else param_bytes_pending != 0*/
      else
      {
        BT_MSG_API_PYLD( "BT HC DRV: ACL data params - Len %d, Rem %d",
                          acl_rx_data.acl_data_hdr.data_len,
                          bytes_pending,
                          0 );

        bt_hc_drv_rx_state_change( BT_HC_DRV_PKT_PAYLD_CONT_STATE );
      }  /* else param_bytes_pending != 0 */

      break;
  } /* switch */

} /* bt_hc_drv_build_rx_hci_acl_data_pkt */


/*===========================================================================

FUNCTION
  BT_HC_DRV_PARSE_EV_PARAMS_FROM_DSM

DESCRIPTION
  Processes the input dsm containing HCI event parameters and puts it into the
  event message structure.

DEPENDENCIES
  None.

INPUTS
  ev_params_dsm_ptr - Pointer to the dsm item containing event parameters
  event_idx    - Index to event table

RETURN VALUE
  Pointer to the built event message.

SIDE EFFECTS
  None.

===========================================================================*/
bt_ev_msg_type bt_hc_drv_parse_ev_params_from_dsm
(
  dsm_item_type** ev_params_dsm_ptr,
  uint8           hci_event_code,
  uint8           param_tot_len
)
{
  /* Index to the event table */
  uint8               event_idx;

  /* HCI event packet to be built */
  bt_ev_msg_type      hci_ev;

  /* Number of bytes read in dsm_pullup */
  uint16              read_bytes = 0;

  /* Length of the non-arrayed portion of an event */
  uint8               ev_non_arr_par_len;

  /* Duplicate dsm pointer to store parts of the event */
  dsm_item_type*      hci_ev_dup_dsm_ptr = NULL;

/*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/
  /* Update the type of event by looking up the event table */
  event_idx             = bt_hc_drv_get_event_index( hci_event_code );
  hci_ev.ev_hdr.ev_type = bt_hc_drv_ev_table[event_idx].bt_ev;

  /* Initialize the Headers values to pacify Klockwork */
  q_link(&hci_ev.ev_hdr,&hci_ev.ev_hdr.link);
  hci_ev.ev_hdr.ev_cb_ptr     = NULL;
  hci_ev.ev_hdr.bt_app_id     = 0;

  /* Build event parameters. Events with fixed and variable
     length parameters are handled differently */
  if ( bt_hc_drv_ev_table[event_idx].ev_len_mode == BT_EV_FIXED_LENGTH )
  {
    /* Fixed length events have only non arrayed paramters - simply pull
       the parameter length bytes from dsm */
    read_bytes = bt_hc_drv_dsm_pullup( ev_params_dsm_ptr,
                                       (void *)&hci_ev.ev_msg,
                                       param_tot_len);

  } /* BT_EV_FIXED_LENGTH */
  else if ( bt_hc_drv_ev_table[event_idx].ev_len_mode == BT_EV_VAR_LENGTH )
  {
    /* Variable length events have non arrayed paramters and arrayed
       parameters which are locally stored as dsm */

    /* get the length of non-arrayed portion */
    ev_non_arr_par_len = bt_hc_drv_ev_table[event_idx].hci_len -
                           sizeof( dsm_item_type* );

    /* Read the non-arrayed portion into ev_msg */
    read_bytes = bt_hc_drv_dsm_pullup( ev_params_dsm_ptr,
                                       (void *)&hci_ev.ev_msg,
                                       ev_non_arr_par_len);

    ASSERT_DEBUG( read_bytes == ev_non_arr_par_len );

    /**** Now get the arrayed parameters ****/

    hci_ev_dup_dsm_ptr = bt_get_free_dsm_ptr( BT_TL_HCI_BB,
                                     BT_EV_HC_INQUIRY_RESULT_ELEM_SIZE );

    /* Push the non-arrayed portion from ev_msg back into the new
       allocated dsm */
    bt_dsm_pushdown_packed( &hci_ev_dup_dsm_ptr,
                            (void *)&hci_ev.ev_msg,
                            ev_non_arr_par_len,
                            DSM_DS_POOL_SIZE(
                            BT_EV_HC_INQUIRY_RESULT_ELEM_SIZE ));

    /* Push down the pointer to the dsm containing the arrayed params
       into the allocated dsm */
    bt_dsm_pushdown_tail( &hci_ev_dup_dsm_ptr,
                          ev_params_dsm_ptr,
                          sizeof(dsm_item_type*),
                          DSM_DS_POOL_SIZE(
                          BT_EV_HC_INQUIRY_RESULT_ELEM_SIZE ));

    /* Now we have duplicated the ev_msg structure in hci_ev_dup_dsm_ptr.
       Pull it up */
    bt_hc_drv_dsm_pullup( &hci_ev_dup_dsm_ptr,
                          (void *) &hci_ev.ev_msg,
                          bt_hc_drv_ev_table[event_idx].hci_len);
    /* hci_ev_dup_dsm_ptr freed now */

    ASSERT_DEBUG( hci_ev_dup_dsm_ptr == NULL );
    *ev_params_dsm_ptr = NULL;

  } /* BT_EV_VAR_LENGTH */

  return( hci_ev );
} /* bt_hc_drv_parse_ev_params_from_dsm */


/*===========================================================================

FUNCTION
  BT_HC_DRV_BUILD_RX_HCI_EVENT_PKT

DESCRIPTION
  Processes the bytes coming from Host controller SOC to Host via SIO
  and parses it to build an HCI event packet.

DEPENDENCIES
  None.

INPUTS
  Dsm pointer to the bytes from sio.

RETURN VALUE
  None.

SIDE EFFECTS
  None.

===========================================================================*/
void bt_hc_drv_build_rx_hci_event_pkt
(
  dsm_item_type** hci_ev_dsm_ptr
)
{
  static bt_ev_msg_type               ev_from_soc;
  static dsm_item_type*               ev_par_dsm_ptr = NULL;
  static uint8                        hci_event_code;
  static uint8                        param_tot_len;
  static uint8                        param_bytes_pending = 0;
  /* State of the event packet being built */
  bt_hc_drv_event_state_enum_type     hci_ev_state;
  dsm_item_type*                      hci_ev_dup_dsm_ptr = NULL;
  dsm_item_type*                      hci_num_pkt_ev_dup_dsm_ptr = NULL;
  uint16                              read_bytes;

/*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  hci_ev_state = bt_hc_drv_get_rx_ev_pkt_state();

  switch ( hci_ev_state )
  {
    case   BT_HCI_EV_EVENT_CODE_STATE    :
    {
      /* Try to read the event code */
      read_bytes = bt_hc_drv_dsm_pullup( hci_ev_dsm_ptr,
                                          &hci_event_code,
                                          sizeof(uint8) );

      if ( read_bytes != sizeof(uint8) )
      {
        /* If unable to read the event code, declare HCI sync loss */
        bt_hc_drv_set_hci_sync_status( FALSE );

        BT_ERR( "BT HC DRV: Get Ev idx failed. HCI Lost Sync", 0, 0, 0);

        break;
      } /* if */
      else
      {
       /* Got event code successfully. Continue to next state */
        bt_hc_drv_rx_ev_pkt_state_change( BT_HCI_EV_PARAM_TOT_LEN_STATE );

      } /* if read_bytes != sizeof(uint8) */
    }
    /* Fallthrough */
    case   BT_HCI_EV_PARAM_TOT_LEN_STATE :
    {
      /* Try to read the total parameter length */
      read_bytes = bt_hc_drv_dsm_pullup( hci_ev_dsm_ptr,
                                          &param_tot_len,
                                          sizeof(uint8) );

      if ( read_bytes != sizeof(uint8) )
      {
        /* Unable to read the event parameter length. Wait for more bytes
          from sio */
        BT_MSG_DEBUG( "BT HC DRV: DQ to Get Ev %d ParLen",
                      hci_event_code, 0, 0);

        bt_hc_drv_rx_state_change( BT_HC_DRV_PKT_PAYLD_CONT_STATE );

        break;
      } /* if */
      else
      {
        /* Got event parameter length successfully. Continue to next state */
        bt_hc_drv_rx_ev_pkt_state_change( BT_HCI_EV_PARAMS_STATE );

        param_bytes_pending = param_tot_len;
      } /* if read_bytes != sizeof(uint8) */
    }
    /* Fallthrough */

    case  BT_HCI_EV_PARAMS_STATE         :
    {
       read_bytes = dsm_dup_packet( &hci_ev_dup_dsm_ptr,
                                    *hci_ev_dsm_ptr,
                                    0,
                                    param_bytes_pending );       
       param_bytes_pending = param_bytes_pending - read_bytes;
       read_bytes = bt_hc_drv_dsm_pullup( hci_ev_dsm_ptr, NULL, read_bytes);

       /* Keep building whole event in ev_par_dsm_ptr. hci_ev_dup_dsm_ptr
          becomes NULL */
       dsm_append( &ev_par_dsm_ptr, &hci_ev_dup_dsm_ptr );

       if ( param_bytes_pending == 0 )
       {
         if ( (bt_dc.cur_hci_mode != BT_HCIM_OFF) &&
              (hci_event_code == BT_HCI_NUM_COMPLETED_PACKETS_EV) )
         {
           /* hci mode ; num cmplt pkt ev recieved */
           read_bytes = dsm_dup_packet( &hci_num_pkt_ev_dup_dsm_ptr,
                                        ev_par_dsm_ptr,
                                        0,
                                        param_tot_len );
           ASSERT_DEBUG( read_bytes == param_tot_len );
         }
         /* Entire event data should now be in ev_par_dsm_ptr */
         ASSERT_DEBUG( dsm_length_packet(ev_par_dsm_ptr) == param_tot_len );

         /* Log the rx event packet */
         bt_hc_drv_log_host_rx_ev( bt_hc_drv.host_info.hci_rx.cur_pkt_type,
                                   hci_event_code, param_tot_len,
                                   ev_par_dsm_ptr );

         /* Parse the dsm to get the event parameters */
         ev_from_soc = bt_hc_drv_parse_ev_params_from_dsm(
                         &ev_par_dsm_ptr,
                         hci_event_code,
                         param_tot_len);

         bt_hc_drv.host_info.bt_hc_drv_txrx_cnt.hci_rx_ev_cnt++;

         /* Event packet completely built. Send it to upper layers */
         BT_MSG_API_PYLD( "BT HC DRV RX: HCI Ec, Op 0x%X, len %d, RxEv# %d",
                         hci_event_code,
                         param_tot_len,
                         bt_hc_drv.host_info.bt_hc_drv_txrx_cnt.hci_rx_ev_cnt);

#ifdef BT_SWDEV_CMD_FLOW_WORKAROUND
         /* update large event pending status */
         bt_hc_update_lrg_evt_sts_with_evt ( &ev_from_soc );
#endif

         /* If the received event is a Command Complete or a
          * Command Status then process command flow control.
          */
         if((IS_CMD_CMPLT_EV( ev_from_soc.ev_hdr.ev_type ) != FALSE) ||
            (IS_CMD_STATUS_EV( ev_from_soc.ev_hdr.ev_type ) != FALSE))
         {
           bt_hc_drv_process_cmd_flow( &ev_from_soc);
         }
         else if ( IS_NUM_CMPLT_PKTS_EV( ev_from_soc.ev_hdr.ev_type ) == TRUE )
         {
           /* NumCmpltPktsEvent received. Update flow control values. */
           bt_hc_drv_process_num_cmplt_pkts_ev( &ev_from_soc );
           if ( bt_dc.cur_hci_mode != BT_HCIM_OFF )
           {
             /* hci mode ; Parse the dup dsm to get the event parameters for
                num cmplt pkts event */
              ev_from_soc = bt_hc_drv_parse_ev_params_from_dsm(
                &hci_num_pkt_ev_dup_dsm_ptr,
                BT_HCI_NUM_COMPLETED_PACKETS_EV,
                param_tot_len);
           }
         }
         else if ( IS_CONN_COMPLETE_EV( ev_from_soc.ev_hdr.ev_type ) == TRUE )
         {
           /* Connection Complete event received. Check if new H2C flow buffer
              needs a new entry */
           bt_hc_drv_process_conn_complete_ev( &ev_from_soc );
         }
         else if( IS_DISCONN_COMPLETE_EV( ev_from_soc.ev_hdr.ev_type ) == TRUE )
         {
           /* Disconnection Complete event received. Check if H2C flow buffer
              needs to be torn down */
           bt_hc_drv_process_disconn_complete_ev( &ev_from_soc );
         }
#ifdef FEATURE_BT_AVSYNC_SOC
         else if ( IS_FLUSH_OCCURRED_EV ( ev_from_soc.ev_hdr.ev_type ) == TRUE )
         {
           bt_hc_drv_process_flush_occurred_ev ( &ev_from_soc );
         }
#ifdef BT_SWDEV_2_1_PB
         else if ( IS_ENH_FLUSH_COMPLETE_EV ( ev_from_soc.ev_hdr.ev_type )
                                                                      == TRUE )
         {
           bt_hc_drv_process_flush_occurred_ev ( &ev_from_soc );
         }
#endif /* BT_SWDEV_2_1_PB */
#endif /* FEATURE_BT_AVSYNC_SOC */

         /* Increment the RX EV frame count in this round of sio processing */
         bt_hc_drv.host_info.hci_rx.hc_rx_cnt++;


#ifdef FEATURE_BT_SOC1
#error code not present
#endif /* FEATURE_BT_SOC1 */
           /* If in HCI mode send all evets to Host */
           if ( bt_dc.cur_hci_mode != BT_HCIM_OFF )
           {
             bt_hc_send_event( &ev_from_soc );
           }
           /* Send all but NumCmpltPkts Event up the stack if not in HCI Mode */
           else if ( IS_NUM_CMPLT_PKTS_EV( ev_from_soc.ev_hdr.ev_type ) == FALSE )
           {
             bt_hc_send_event( &ev_from_soc );
           }
#ifdef FEATURE_BT_SOC1
#error code not present
#endif /* FEATURE_BT_SOC1 */

         /* Reinitialize for starting a new event packet next time*/
         bt_hc_drv_rx_state_change( BT_HC_DRV_NEW_PKT_STATE );
         bt_hc_drv_rx_ev_pkt_state_change( BT_HCI_EV_EVENT_CODE_STATE );

       } /* if param_bytes_pending == 0 */
       else
       {
         BT_MSG_DEBUG( "BT HC DRV: pullup remaining; EV %d,ParLen %d,Rem %d",
                 hci_event_code, param_tot_len, param_bytes_pending );

         bt_hc_drv_rx_state_change( BT_HC_DRV_PKT_PAYLD_CONT_STATE );
       }  /* else param_bytes_pending != 0*/
       break;
    }

  } /* switch */

} /* bt_hc_drv_build_rx_hci_event_pkt */


/*===========================================================================

FUNCTION
  BT_HC_DRV_GET_RX_HCI_PKT_TYPE

DESCRIPTION
  Parses the packet type from the first byte of the received HCI packet.

DEPENDENCIES
  None.

INPUTS
  Dsm pointer to the byte stream from sio.

RETURN VALUE
  None.

SIDE EFFECTS
  None.

===========================================================================*/
void bt_hc_drv_get_rx_hci_pkt_type
(
  dsm_item_type**  hci_rx_dsm_ptr
)
{
  uint16                 read_byte;
  uint8                  pkt_type;

/*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/
  /* New HCI packet. Get the HCI packet type field from dsm */
  read_byte       = bt_hc_drv_dsm_pullup( hci_rx_dsm_ptr,
                                           &pkt_type,
                                           sizeof(uint8) );

  if ( read_byte == sizeof(uint8) )
  {
#ifdef FEATURE_BT_SOC1
#error code not present
#endif /* FEATURE_BT_SOC1 */
    {
    /* Packet type obtained successfully. HCI in sync. Proceed. */
    bt_hc_drv.host_info.hci_rx.cur_pkt_type = ( bt_hci_pkt_type )pkt_type;
    bt_hc_drv_set_hci_sync_status( TRUE );

    /* Set HCI Rx state to Continue processing the patload of this HCI packet */
    bt_hc_drv_rx_state_change( BT_HC_DRV_PKT_PAYLD_STATE );

    BT_MSG_API_PYLD( "BT HC DRV: Pkt %d received; DQrem %d, DQproc %d",
                      pkt_type,
                      dsm_length_packet( *hci_rx_dsm_ptr ),
                      read_byte );

    }
  }
  else
  {
    /* Failure to read packet type - lost sync with host controller. */
    BT_ERR( "BT HC DRV: Packet type read failure.HCI Lost sync.", 0, 0, 0);

    /* Update the HCI sync status */
    bt_hc_drv_set_hci_sync_status( FALSE );

  } /* if read_byte == sizeof(uint8) */

} /* bt_hc_drv_get_rx_hci_pkt_type */


/*===========================================================================

FUNCTION
  BT_HC_DRV_BUILD_RX_HCI_PKT

DESCRIPTION
  Builds the HCI packet received from the sio bytestream.

DEPENDENCIES
  None.

INPUTS
  Dsm pointer to the byte stream from sio.
  HCI Packet type.

RETURN VALUE
  None.

SIDE EFFECTS
  None.

===========================================================================*/
void bt_hc_drv_build_rx_hci_pkt
(
  uint8           hci_pkt_type,
  dsm_item_type**  hci_rx_dsm_ptr
)
{

/*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  /* Parse the bytes from sio and build the hci packet received from
     HostController */
   switch ( (bt_hci_pkt_type)hci_pkt_type )
   {
     case BT_HCI_EVENT_PKT :
       /* Parse and build the Event packet received from HostController */
       bt_hc_drv_build_rx_hci_event_pkt( hci_rx_dsm_ptr );

       break;

     case BT_HCI_ACL_DATA_PKT :
       /* Parse and build the ACL data packet received from HostController */
       bt_hc_drv_build_rx_hci_acl_data_pkt( hci_rx_dsm_ptr );

       break;

     case BT_HCI_SCO_DATA_PKT :
       BT_MSG_HIGH( "BT HC DRV: SCO pktfrom SOC %d. Check settings",
                    hci_pkt_type, 0, 0 );

       bt_hc_drv_rx_state_change( BT_HC_DRV_NEW_PKT_STATE );
       break;

     case BT_HCI_ZERO_PKT     :
#ifdef FEATURE_BT_QSOC_INBAND_SLEEP
       BT_LOG_INBAND_STATE("BT HC DRV:Recvd zero char for RX BRK from SOC");
#else
       BT_MSG_HIGH( "BT HC DRV: Bad pkt type from SOC %d, Sync loss",
                     hci_pkt_type, 0, 0 );
#endif /* FEATURE_BT_QSOC_INBAND_SLEEP */
      /* Set HCI Sync to FALSE until next Valid PKT is processed by HCI DRIVER */
       bt_hc_drv_set_hci_sync_status( FALSE );
       bt_hc_drv_rx_state_change( BT_HC_DRV_NEW_PKT_STATE );
       break;

     default                  :
#ifndef BT_SWDEV_H4DS_ON
       bt_hc_drv_set_hci_sync_status( FALSE );
       BT_MSG_HIGH( "BT HC DRV: Bad pkt type from SOC %d, Sync loss",
                     hci_pkt_type, 0, 0 );
#endif
       bt_hc_drv_rx_state_change( BT_HC_DRV_NEW_PKT_STATE );
       break;
   } /* switch */

} /* bt_hc_drv_build_rx_hci_pkt */


/*===========================================================================

FUNCTION
  BT_HC_DRV_DQ_BYTES_FROM_SIO

DESCRIPTION
  Dequeues bytes coming from Host Controller SOC to Host via SIO.

DEPENDENCIES
  None.

INPUTS
  None.

RETURN VALUE
  Pointer to dsm item dequeued from sio watermark.

SIDE EFFECTS
  None.

===========================================================================*/
dsm_item_type* bt_hc_drv_dq_bytes_from_sio( void )
{
  /* dsm item for bytes received from sio at HCI */
  dsm_item_type*  sio_to_hci_dsm_ptr;

/*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/
 /* Dequeue dsm items from sio watermark */
  sio_to_hci_dsm_ptr = dsm_dequeue( &bt_soc_to_msm_sio_wm );

  /* Debug trace */
  BT_HC_DRV_RX_TRACE( sio_to_hci_dsm_ptr );

  BT_MSG_API_PYLD( "BT HC DRV: DQ from_sio %d",
                   dsm_length_packet( sio_to_hci_dsm_ptr ), 0, 0 );
  return( sio_to_hci_dsm_ptr );

} /* bt_hc_drv_dq_bytes_from_sio */


/*===========================================================================

FUNCTION
  BT_HC_DRV_GET_TOT_ACL_FR_RX

DESCRIPTION
  Process the number of ACL frames received in current round of SIO Rx watermark
  processing.

DEPENDENCIES
  None.

INPUTS
  None.

RETURN VALUE
  The number of ACL frames received in current round of SIO Rx watermark
  processing.

SIDE EFFECTS
  None.

===========================================================================*/
uint8 bt_hc_drv_get_tot_acl_fr_rx( void )
{
  uint8 tot_acl_frames = 0; /* Total number of ACL frames received */
  uint8 c_hndl;             /* Counter for ACL connections */

/*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  for( c_hndl = 0; c_hndl < BT_MAX_BT_DEV; c_hndl++ )
  {
    tot_acl_frames += bt_hc_drv.host_info.hci_rx.hc_rx_acl_cnt[ c_hndl ];
  } /* for c_hndl */

  return( tot_acl_frames );

} /* bt_hc_drv_get_tot_acl_fr_rx */


/*===========================================================================

FUNCTION
  BT_HC_DRV_PROCESS_BYTES_FROM_SIO

DESCRIPTION
  Process bytes coming from Host Controller SOC to Host via SIO.

DEPENDENCIES
  None.

INPUTS
  None.

RETURN VALUE
  None.

SIDE EFFECTS
  None.

===========================================================================*/
void bt_hc_drv_process_bytes_from_sio( void )
{
  /* Dsm pointer to the received sio data */
  static dsm_item_type*  sio_rx_dsm_ptr = NULL;

  /* Debug variable */
  uint16                 debug_dsm = 0;

/*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/
  if ( sio_rx_dsm_ptr == NULL )
  {
    /* Dequeue dsm items from sio watermark */
    sio_rx_dsm_ptr = bt_hc_drv_dq_bytes_from_sio();

  } /* if ( sio_rx_dsm_ptr == NULL ) */

/*  If this is the first response from SoC after the driver has been
 *  re-initialized (E.g: After a SoC "power-off" and "power-on"), hc driver
 *  packet processing state may not be correct. For example, if this function
 *  is triggered by processing of host stack events then the state needs to be
 *  set correctly before processing the packet. So, this check has been added.
 */

  if ( (sio_rx_dsm_ptr != NULL) && ( bt_hc_drv_get_hci_rx_state() == BT_HC_DRV_INIT_STATE ) )
  {
    /* Change HCI state from idle to processing a new pakcet */
    bt_hc_drv_rx_state_change( BT_HC_DRV_NEW_PKT_STATE );
  } /* if ( bt_hc_drv_get_hci_rx_state() == BT_HC_DRV_INIT_STATE ) */

  /* Extract events and ACL data packets if sio has given us some data */
  while (( sio_rx_dsm_ptr != NULL ) &&
         (( bt_hc_drv.host_info.hci_rx.hc_rx_cnt +
            bt_hc_drv_get_tot_acl_fr_rx() ) < BT_HC_DRV_MAX_NUM_FRAMES ))
  {
    /***************************************************************************
       Get the HCI data packet type.
       This needs to be done only if it is a new packet.  If it is a packet
       which was not completed earlier, skip reading this field.
    **************************************************************************/
    if ( bt_hc_drv_get_hci_rx_state() == BT_HC_DRV_NEW_PKT_STATE )
    {
      /* New HCI packet. Get the HCI packet type field from dsm */
      bt_hc_drv_get_rx_hci_pkt_type( &sio_rx_dsm_ptr );

#ifdef FEATURE_BT_QSOC_INBAND_SLEEP
      /* Update the Inband RX state  machine */
      bt_qsoc_sleep_inband_rx_new_pkt();
#endif /* FEATURE_BT_QSOC_INBAND_SLEEP*/
    }
    else
    {
      debug_dsm = dsm_length_packet( sio_rx_dsm_ptr );

#ifdef BT_HC_DRV_INBAND_TRACE_ENABLE
      /* Debug log call for  Inband Rx activity  */
      bt_qsoc_sleep_inband_log_rx_activity();
#endif /* BT_HC_DRV_INBAND_TRACE_ENABLE */

      /* Continuing HCI packet. Get remaining portion of the packet. */
      bt_hc_drv_build_rx_hci_pkt( bt_hc_drv.host_info.hci_rx.cur_pkt_type,
                                  &sio_rx_dsm_ptr );


      if ( bt_hc_drv_get_hci_rx_state() == BT_HC_DRV_NEW_PKT_STATE )
      {
         BT_MSG_API_PYLD( "BT HC DRV: Pkttype %d built; DQrem %d, DQproc %d",
                          bt_hc_drv.host_info.hci_rx.cur_pkt_type,
                          dsm_length_packet( sio_rx_dsm_ptr),
                          debug_dsm -  dsm_length_packet( sio_rx_dsm_ptr) );
      } /* if */
      else
      {
        BT_MSG_API_PYLD( "BT HC DRV: Pkttype %d in progress; DQrem %d, DQproc %d",
                      bt_hc_drv.host_info.hci_rx.cur_pkt_type,
                      dsm_length_packet( sio_rx_dsm_ptr),
                      debug_dsm -  dsm_length_packet( sio_rx_dsm_ptr));
      } /* if bt_hc_drv_get_hci_rx_state() != BT_HC_DRV_NEW_PKT_STATE */

    } /* if ( bt_hc_drv_get_hci_rx_state() == BT_HC_DRV_NEW_PKT_STATE ) */

    /* Verify that HCI of Host and Controller are in sync */
    bt_hc_drv_verify_hci_sync_status();

    if ( sio_rx_dsm_ptr == NULL )
    {
      /* Dequeue dsm items from sio watermark */
      sio_rx_dsm_ptr = bt_hc_drv_dq_bytes_from_sio();

    } /* if ( sio_rx_dsm_ptr == NULL ) */

  } /* while sio_rx_dsm_ptr != NULL  &&
       bt_hc_drv.host_info.hci_rx.hc_rx_cnt < BT_RM_NUM_EVENTS */

    BT_MSG_API_PYLD( "Processed SIO hci_fr_cnt %d, WM_cur_cnt=%d",
                     ( bt_hc_drv.host_info.hci_rx.hc_rx_cnt +
                     bt_hc_drv_get_tot_acl_fr_rx() ),
                     bt_soc_to_msm_sio_wm.current_cnt, 0
                   );

  /* Reset  the HCI RX count since this round of sio watermark processing is
     complete */
#ifdef FEATURE_BT_HCI_HOST_FCTL
  bt_hc_drv_send_host_num_cmplt_pkts_cmd();
#else
  bt_hc_drv_reset_acl_fr_cnt();
#endif /* FEATURE_BT_HCI_HOST_FCTL */
  bt_hc_drv.host_info.hci_rx.hc_rx_cnt = 0;

  ASSERT_DEBUG( sio_rx_dsm_ptr == NULL );

#ifdef FEATURE_BT_QSOC_SLEEP
  if (bt_soc_to_msm_sio_wm.current_cnt == 0)
  {
#ifdef   FEATURE_BT_QSOC_INBAND_SLEEP
    /* Inband sleep SM doesn't depend on RX data to determine Inband sleep state
       Also Inband SM double-checks the SIO WM before UART Shutdown.Else it starts
       a RX flush timer afer which  Inband Sleep SM attempts Uart shutdown again*/
#else
    bt_qsoc_sleep_sm_eval_event (BT_QSOC_SLEEP_EVENT_SIO_RX_IDLE, TRUE);
#endif /* FEATURE_BT_QSOC_INBAND_SLEEP */
  }
#endif /* FEATURE_BT_QSOC_SLEEP */

} /* bt_hc_drv_process_bytes_from_sio */


/*===========================================================================

FUNCTION
  BT_HC_DRV_PROCESS_HCI_RX

DESCRIPTION
  Processes bytes received from Host Controller SOC to Host via SIO.

DEPENDENCIES
  None.

INPUTS
  None.

RETURN VALUE
  None.

SIDE EFFECTS
  None.

===========================================================================*/
void bt_hc_drv_process_hci_rx( void )
{
  /* Flag to check if HCI of Host and Controller are in sync */
  boolean                is_hci_in_sync = TRUE;

/*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

#ifdef FEATURE_BT_SOC1
#error code not present
#endif /* FEATURE_BT_SOC1 */

  if ( bt_hc_drv_get_hci_rx_state() == BT_HC_DRV_INIT_STATE )
  {
    /* Change HCI state from idle to processing a new pakcet */
    bt_hc_drv_rx_state_change( BT_HC_DRV_NEW_PKT_STATE );
  } /* if ( bt_hc_drv_get_hci_rx_state() == BT_HC_DRV_INIT_STATE ) */

  /* Verify that HCI of Host and Controller are in sync */
  is_hci_in_sync = bt_hc_drv_verify_hci_sync_status();

  /* Proceed only if HCI is in sync, otherwise do nothing.
     bt_hc_drv_verify_hci_sync_status will take care of resetting SoC
     if needed */
  if ( is_hci_in_sync == TRUE )
  {
    bt_hc_drv_process_bytes_from_sio();

  } /* if is_hci_in_sync */
  else
  {
    BT_ERR( "BT Host HCI: Lost sync with Controller; %d",
            is_hci_in_sync,
            0,
            0);
  } /* else is_hci_in_sync */


  BT_MSG_API_PYLD( "BT HC DRV: Sio WM processed; HciPkt %d, RxSt %d, SyncSt %d",
                    bt_hc_drv.host_info.hci_rx.cur_pkt_type,
                    bt_hc_drv_get_hci_rx_state(),
                    bt_hc_drv_get_hci_sync_status() );

} /* bt_hc_drv_process_hci_rx */


/*==========================================================================

FUNCTION
  BT_HC_DRV_RESET

DESCRIPTION
  This function resets the HCI driver by clearing any existing used queues,
  watermarks used and places it in the start-up state.

DEPENDENCIES
  None.

INPUTS
  None.

RETURN VALUE
  None.

SIDE EFFECTS
  None.

==========================================================================*/

static void bt_hc_drv_reset( void )
{
  /* Loop counter for connection handles */
  uint8   c_hndl;

/*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  /* Remove all connections and empty all TX ACL watermarks and AV-Sync Qs */
  for( c_hndl = 0; c_hndl < BT_MAX_BT_DEV; c_hndl++ )
  {
    bt_hc_drv_remove_conn(
      bt_hc_drv.ctrlr_info.cur_buf.entries[c_hndl].conn_hndl );

  } /* for( c_hndl )*/

  /* Empty the command watermark */
  dsm_empty_queue( &bt_hc_drv.host_info.hci_tx.cmd_tx.hci_cmd_tx_wm );

  /* Empty the sio-> BT and BT->sio watermarks */
  dsm_empty_queue( &bt_soc_to_msm_sio_wm );
  dsm_empty_queue( &bt_msm_to_soc_sio_wm );

  /* Reinitialize HCI packet processing states */
  bt_hc_drv_rx_state_change( BT_HC_DRV_NEW_PKT_STATE );
  bt_hc_drv_set_hci_sync_status( TRUE );

} /* bt_hc_drv_reset */


/*===========================================================================

FUNCTION
  BT_HC_DRV_SOC_SHUTDOWN

DESCRIPTION
  Shuts down the SOC driver.

DEPENDENCIES
  None.

INPUTS
  None.

RETURN VALUE
  None.

SIDE EFFECTS
  None.

===========================================================================*/
void bt_hc_drv_soc_shutdown( void )
{

/*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  BT_MSG_API( "BT HC DRV : Power-off SoC", 0, 0, 0 );

#ifdef FEATURE_BT_HCI_HOST_FCTL
/* Unregister the callbacks for  the DSM pool Free item count */
  dsm_unreg_mem_event_cb( DSM_DS_SMALL_ITEM_POOL, DSM_MEM_LEVEL_MANY,
    DSM_MEM_OP_FREE );

  dsm_unreg_mem_event_cb( DSM_DS_SMALL_ITEM_POOL, DSM_MEM_LEVEL_FEW,
    DSM_MEM_OP_NEW );

  dsm_unreg_mem_event_cb( DSM_DS_LARGE_ITEM_POOL, DSM_MEM_LEVEL_MANY,
    DSM_MEM_OP_FREE );

  dsm_unreg_mem_event_cb( DSM_DS_LARGE_ITEM_POOL, DSM_MEM_LEVEL_FEW,
    DSM_MEM_OP_NEW );
#endif /* FEATURE_BT_HCI_HOST_FCTL */

  /* Reset the Host HCI driver layer */
  bt_hc_drv_reset();

  /* Reset the HCI test mode parsing engine */
  bt_hc_reset();

  /* Clear any timers that were running for operation with SOC */
  bt_soc_shutdown_cleanup();

  /* Shutdown SoC unless there is an ongoing HCI RST in Test Mode */
  if ( ( bt_soc_ifc.bt_soc_proc_shutdown_func_ptr != NULL ) &&
       ( bt_hc_pe_is_hci_rst_started() == FALSE ) )
  {
    bt_soc_ifc.bt_soc_proc_shutdown_func_ptr();
  }
#ifdef FEATURE_BT_SOC1
#error code not present
#endif /* FEATURE_BT_SOC1 */
} /* bt_hc_drv_soc_shutdown */


#ifdef FEATURE_BT_AVSYNC_SOC
/*==========================================================================

FUNCTION
  BT_HC_DRV_ENQUE_ACL_DATA

DESCRIPTION
  Enques ACL data to be transmitted to Controller. This only enques the
  packet header information so as to match a Num Complete and send
  filer only necessary num completes to the Host Stack.

DEPENDENCIES
  None.

INPUTS
  BT Cmd message with ACL Data.
  Index to ACL Connection Handle in HC Driver

RETURN VALUE
  None.

SIDE EFFECTS
  None.

==========================================================================*/

void bt_hc_drv_enque_acl_data
(
  bt_cmd_msg_type* bt_cmd_ptr,
  uint8            c_hndl_idx
)
{
  bt_hc_drv_acl_frame_handle_type  *acl_frame_handle;
  bt_cmd_hc_tx_acl_data_type       *hc_tad_ptr;

/*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

#ifdef FEATURE_BT_AVSYNC_DEBUG
  BT_MSG_DEBUG("BT HC Drv: Av-Sync Enq c_hndl=0x%x", c_hndl_idx, 0, 0);
#endif

  /* Sanity Check for out of bounds array index */
  if( c_hndl_idx < BT_MAX_BT_DEV )
  {
    hc_tad_ptr = &bt_cmd_ptr->cmd_msg.cmd_hc_tx_acl_data;
    /* Queue the packet so as to match it with the Num Completes */
    acl_frame_handle = q_get(&bt_hc_drv_acl_free_frame_handle_q);
    if(acl_frame_handle != NULL)
    {
      acl_frame_handle->tx_status      = FALSE;
      acl_frame_handle->need_to_flush  = FALSE;
      acl_frame_handle->up_handle_ptr  = hc_tad_ptr->frame_handle;
      acl_frame_handle->local_cid      = hc_tad_ptr->local_cid;
  
      clk_read_ms(acl_frame_handle->rx_time_stamp);
      if((hc_tad_ptr->pkt_boundary_flag == BT_HC_PKT_BOUNDARY_START) ||
         (hc_tad_ptr->pkt_boundary_flag == BT_HC_PKT_BOUNDARY_START_FLUSHABLE))
      {
        acl_frame_handle->start_of_l2cap = TRUE;
      }
      else
      {
        acl_frame_handle->start_of_l2cap = FALSE;
      }
      if((hc_tad_ptr->pkt_boundary_flag == BT_HC_PKT_BOUNDARY_START_FLUSHABLE) ||
         (hc_tad_ptr->pkt_boundary_flag == BT_HC_PKT_BOUNDARY_CONT_FLUSHABLE))
      {
        acl_frame_handle->flushable_pkt = TRUE;
      }
      else
      {
        acl_frame_handle->flushable_pkt = FALSE;
      }
      q_put( &bt_hc_drv.host_info.hci_tx.acl_tx[c_hndl_idx].
              acl_data_tx_frame_handle_q, &acl_frame_handle->link );
#ifdef FEATURE_BT_AVSYNC_DEBUG
      BT_MSG_DEBUG("BT HC Drv: Av-Sync Enq Q=0x%x", acl_frame_handle, 0, 0);
      BT_MSG_DEBUG("BT HC Drv: Av-Sync Enq up=0x%x, st=0x%x, f-able=0x%x",
                    acl_frame_handle->up_handle_ptr,
                    acl_frame_handle->start_of_l2cap,
                    acl_frame_handle->flushable_pkt);
#endif
    } /* if frame_handle != NULL */
    else
    {
       BT_ERR("BT HC DRV: Av-Sync Ran out of free frame handles", 0, 0, 0);
    } /* else if frame_handle != NULL */
  }
  else
  {
    BT_ERR("BT HC DRV Failure: Connection index out of bounds %d",c_hndl_idx, 0, 0 ); 
  } /*  if c_hndl_idx < BT_MAX_BT_DEV ..*/   
} /* bt_hc_drv_enque_acl_data */


/*==========================================================================

FUNCTION
  BT_HC_DRV_UPDATE_TX_STATUS

DESCRIPTION
  Updates the Tx Status for the Header Element to True when this ACL data is
  sent to the SoC.

DEPENDENCIES
  None.

INPUTS
  Index to ACL Connection Handle in HC Driver

RETURN VALUE
  None.

SIDE EFFECTS
  None.

==========================================================================*/
void bt_hc_drv_update_tx_status
(
  uint8        c_hndl_idx
)
{
  bt_hc_drv_acl_frame_handle_type  *acl_frame_handle;

/*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

#ifdef FEATURE_BT_AVSYNC_DEBUG
  BT_MSG_DEBUG("BT HC Drv: Av-Sync Update Tx status c_hndl=0x%x",
                c_hndl_idx, 0, 0);
#endif
  /* Find the Queue Element that is not yet sent to the SoC */
  acl_frame_handle = q_check(&bt_hc_drv.host_info.hci_tx.acl_tx[c_hndl_idx].
                             acl_data_tx_frame_handle_q);
  while(NULL != acl_frame_handle)
  {
    if(acl_frame_handle->tx_status != TRUE)
    {
      acl_frame_handle->tx_status = TRUE;
      bt_hc_drv.host_info.hci_tx.acl_tx[c_hndl_idx].
                                    num_unacked_pkts ++;
      if( acl_frame_handle->flushable_pkt != TRUE)
      {
        bt_hc_drv.host_info.hci_tx.acl_tx[c_hndl_idx].
                                    num_non_flushable_pkts ++;
      }
#ifdef FEATURE_BT_AVSYNC_DEBUG
      BT_MSG_DEBUG("BT HC Drv: Av-Sync Update Sent Tx status for up=0x%x",
                    acl_frame_handle->up_handle_ptr, 0, 0);
      BT_MSG_DEBUG("BT HC Drv: Av-Sync Update Enq Q=0x%x non_f=0x%x tot=0x%x",
                    acl_frame_handle,
                    bt_hc_drv.host_info.hci_tx.acl_tx[c_hndl_idx].
                                             num_non_flushable_pkts,
                    bt_hc_drv.host_info.hci_tx.acl_tx[c_hndl_idx].
                                             num_unacked_pkts);
#endif
      break;
    } /* if tx_status != TRUE */
    acl_frame_handle =
            q_next(&bt_hc_drv.host_info.hci_tx.acl_tx[c_hndl_idx].
                   acl_data_tx_frame_handle_q, &acl_frame_handle->link);
  } /* While acl_frame_handle != NULL */
} /* bt_hc_drv_update_tx_status */


/*==========================================================================

FUNCTION
  BT_HC_DRV_PROCESS_AVSYNC_NUM_CMPLT

DESCRIPTION
  Processes the Num Completes received from the SoC, De-ques the completed ACL
  Packet headers and generates a Tx-Cfm Event to RM for ACL packets that require
  an acknowledgement to be generated.

DEPENDENCIES
  None.

INPUTS
  Index to ACL Connection Handle in HC Driver
  The ACL Connection Handle for this ACL Connection
  The number of completed packets in this ACL Link.

RETURN VALUE
  None.

SIDE EFFECTS
  None.

==========================================================================*/
void bt_hc_drv_process_avsync_num_cmplt
(
  uint8           c_hndl_idx,
  uint16          conn_hndl,
  uint16          num_cmplt_pkts
)
{
  bt_hc_drv_acl_frame_handle_type* acl_frame_handle;
  bt_ev_msg_type                   data_cfm_msg;
  uint8                            i;

/*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

#ifdef FEATURE_BT_AVSYNC_DEBUG
  BT_MSG_DEBUG("BT HC Drv: Av-Sync process NCmplt c_hndl_idx=0x%x pkts = 0x%x",
                c_hndl_idx, num_cmplt_pkts, 0);
#endif

  /* Sanity Check for out of bounds array index */
  if( c_hndl_idx < BT_MAX_BT_DEV )
  {
    /* De-queue the Completed ACL headers and generate Tx-Cfm for the
       required ones */
    for(i=0; i<num_cmplt_pkts; i++)
    {
      acl_frame_handle = q_get(&bt_hc_drv.host_info.hci_tx.acl_tx[c_hndl_idx].
                               acl_data_tx_frame_handle_q);
#ifdef FEATURE_BT_AVSYNC_DEBUG
      BT_MSG_DEBUG("BT HC Drv: Av-Sync De-Q Q=0x%x f_p=0x%x", acl_frame_handle,
                    acl_frame_handle->flushable_pkt, 0);
#endif
      if(NULL != acl_frame_handle)
      {
        if((NULL != acl_frame_handle->up_handle_ptr) &&
           (BT_INVALID_ADDRESS != acl_frame_handle->up_handle_ptr))
        {
          data_cfm_msg.ev_hdr.ev_type = BT_EV_HC_TX_DATA_CFM;
          data_cfm_msg.ev_msg.ev_hc_tx_cfm.conn_hndl = conn_hndl;
          data_cfm_msg.ev_msg.ev_hc_tx_cfm.local_cid =
                                              acl_frame_handle->local_cid;
          data_cfm_msg.ev_msg.ev_hc_tx_cfm.up_handle_ptr =
                                              acl_frame_handle->up_handle_ptr;
          bt_hc_send_event( &data_cfm_msg );
#ifdef FEATURE_BT_AVSYNC_DEBUG
          BT_MSG_DEBUG("BT HC Drv: Av-Sync Generating Tx Cfm CH=0x%x, lcid=0x%x"
                       "up=0x%x", conn_hndl, acl_frame_handle->local_cid,
                       acl_frame_handle->up_handle_ptr);
#endif
        } /* if up_handle_ptr != NULL && up_handle_ptr != BT_INVALID_ADDRESS */
        if( acl_frame_handle->flushable_pkt != TRUE )
        {
          bt_hc_drv.host_info.hci_tx.acl_tx[c_hndl_idx].
                                      num_non_flushable_pkts --;
        }
        bt_hc_drv.host_info.hci_tx.acl_tx[c_hndl_idx].
                                      num_unacked_pkts --;
        acl_frame_handle->tx_status = FALSE;
        /* Put it back into the free Q */
        q_put(&bt_hc_drv_acl_free_frame_handle_q, &acl_frame_handle->link);
      } /* if acl_frame_handle != NULL */
      else
      {
        BT_ERR("BT HC Drv: Av-Sync: No ACL hdrs queued in num_cmplt", 0, 0, 0);
      } /* else if acl_frame_handle != NULL */
    }  /* for i */
  }
  else
  {
    BT_ERR("BT HC DRV Failure: Connection index out of bounds %d",c_hndl_idx, 0, 0 ); 
  } /*  if c_hndl_idx < BT_MAX_BT_DEV .. */   
} /* bt_hc_drv_process_avsync_num_cmplt */


/*==========================================================================

FUNCTION
  BT_HC_DRV_EMPTY_AVSYNC_HEADER_Q

DESCRIPTION
  Empty the Queued Packet headers and generates a Tx-Cfm Event to RM
  for ACL packets that require an acknowledgement to be generated.

DEPENDENCIES
  None.

INPUTS
  Index to ACL Connection Handle in HC Driver
  The ACL Connection Handle for this ACL Connection

RETURN VALUE
  None.

SIDE EFFECTS
  None.

==========================================================================*/
void bt_hc_drv_empty_avsync_header_q
(
  uint8     c_hndl_idx,
  uint16    conn_hndl
)
{
  uint16       num_cmplt_pkts;

/*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  num_cmplt_pkts = q_cnt(&bt_hc_drv.host_info.hci_tx.acl_tx[c_hndl_idx].
                         acl_data_tx_frame_handle_q);
  if(num_cmplt_pkts > 0)
  {
#ifdef FEATURE_BT_AVSYNC_DEBUG
    BT_MSG_DEBUG("BT HC Drv: Av-Sync Generating Tx-Cfm for Disc"
                 "c_idx=0x%x, c_hndl=0x%x, num_cmp=0x%x",
                 c_hndl_idx, conn_hndl, num_cmplt_pkts);
#endif
    /* Generate tx Cfm for all the queued headers */
    bt_hc_drv_process_avsync_num_cmplt(c_hndl_idx, conn_hndl, num_cmplt_pkts);
  } /* if num_cmplt_pkts > 0 */
  bt_hc_drv.host_info.hci_tx.acl_tx[c_hndl_idx].num_non_flushable_pkts = 0;
  bt_hc_drv.host_info.hci_tx.acl_tx[c_hndl_idx].num_unacked_pkts       = 0;
  bt_hc_drv.host_info.hci_tx.acl_tx[c_hndl_idx].buffer_flushed         = FALSE;
} /* bt_hc_drv_empty_avsync_header_q */

/*==========================================================================

FUNCTION
  BT_HC_DRV_L2FLUSH_TIMER_TICK

DESCRIPTION
  Tick timer routine to monitor ACL packets that has stayed in the HC beyond
  a pre-configured timeout period (Flush timeout).
  Only if all the packets are flushable (Media) then a HCI Flush command
  is generated to the Controller.

DEPENDENCIES
  None.

INPUTS
  None.

RETURN VALUE
  None.

SIDE EFFECTS
  None.

==========================================================================*/
void bt_hc_drv_l2flush_timer_tick
(
  void
)
{
  uint8                             c_hndl_idx;
  bt_hc_drv_acl_frame_handle_type*  acl_frame_handle;
  time_type                         time_stamp;
  int32                             delta_ms;
  bt_cmd_msg_type*                  cmd_ptr;

/*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  clk_read_ms(time_stamp);
  for(c_hndl_idx = 0; c_hndl_idx < BT_MAX_BT_DEV; c_hndl_idx++)
  {
    /* if there are outstanding Pkts that have been written to cntrlr */
    /* call RM API to change LPM state if necessary */
    if( q_cnt(&bt_hc_drv.host_info.hci_tx.acl_tx[c_hndl_idx].
                         acl_data_tx_frame_handle_q) > 0 )
    {
      bt_rm_pending_acl_tx_lpm_check (
        bt_hc_drv.ctrlr_info.cur_buf.entries[c_hndl_idx].conn_hndl );
    }

    if( bt_hc_drv.host_info.hci_tx.acl_tx[c_hndl_idx].
                                  num_non_flushable_pkts > 0 )
    {
       BT_MSG_DEBUG("BT HC Drv: Av-Sync Flush: Skip Flush nf_pkts = 0x%x "
                    "Tot = 0x%x",
                     bt_hc_drv.host_info.hci_tx.acl_tx[c_hndl_idx].
                                   num_non_flushable_pkts,
                     bt_hc_drv.host_info.hci_tx.acl_tx[c_hndl_idx].
                                   num_unacked_pkts, 0);
#ifdef BT_SWDEV_2_1_PB
       if( ( bt_soc_etc_data.lmp_version < BT_SOC_LMP_VERSION_2_1 ) ||
           ( ( bt_hc_drv.host_info.hci_tx.acl_tx[c_hndl_idx].
                                    num_unacked_pkts -
                  bt_hc_drv.host_info.hci_tx.acl_tx[c_hndl_idx].
                                    num_non_flushable_pkts ) == 0 ) )
       {
         /* Flush command not should be issued in two cases */
         /* Case 1 : Local controller version is BT 2.0 or below */
         /* Case 2 : Controller do not have any outstanding flushable packets*/
         continue;
       }
#else
       /* Donot Flush (use HCI Flush) when there are non flushable packets
        * also that are outstanding in the controller.
        */
       continue;
#endif /* BT_SWDEV_2_1_PB */
    } /* if num_non_flushable_pkts > 0 */

    /* Check if there are outstanding Pkts that have been written to SoC */
    if( q_cnt(&bt_hc_drv.host_info.hci_tx.acl_tx[c_hndl_idx].
                         acl_data_tx_frame_handle_q) > 0 )
    {
      /* Check If the packet at the head of the Queue has exceeded its time
         and need to be flushed */
      acl_frame_handle = q_check(&bt_hc_drv.host_info.hci_tx.acl_tx[c_hndl_idx].
                                  acl_data_tx_frame_handle_q);
      delta_ms = (int32)(QW_CVT_Q2N( time_stamp ) -
                  QW_CVT_Q2N( acl_frame_handle->rx_time_stamp ));

      /* If the Packet has exceed the flush timeout , and if there are
       * out standing packets in the controller and if a flush command
       * has not been issued prior then flush it */
      if(( delta_ms > bt_hc_drv.host_info.hci_tx.acl_tx[c_hndl_idx].flush_to )
          && ( acl_frame_handle->tx_status == TRUE )
          && ( bt_hc_drv.host_info.hci_tx.acl_tx[c_hndl_idx].buffer_flushed
                                                                    != TRUE ))
      {
        BT_MSG_HIGH("BT HC Drv: Av-Sync Flush: HCI flush c_idx = 0x%x, "
                    "delta = 0x%x", c_hndl_idx, delta_ms, 0);
        if ( (cmd_ptr = bt_get_free_cmd()) != NULL )
        {
#ifdef BT_SWDEV_2_1_PB
          if( bt_soc_etc_data.lmp_version >= BT_SOC_LMP_VERSION_2_1 )
          {
            cmd_ptr->cmd_hdr.cmd_type  = BT_CMD_HC_ENH_FLUSH;
            cmd_ptr->cmd_hdr.bt_app_id = bt_dc_last_hci_ev_reg_app_id;
            cmd_ptr->cmd_msg.cmd_hc_enh_flush.conn_hndl =
              bt_hc_drv.ctrlr_info.cur_buf.entries[c_hndl_idx].conn_hndl;
            cmd_ptr->cmd_msg.cmd_hc_enh_flush.pkt_type =
                                              BT_HC_AUTO_FLUSHABLE_PKT_TYPE;
            BT_MSG_HIGH("BT HC Drv: Av-Sync Flush: HCI (ENH) Flush Cmd c_hndl=0x%x"
                       "appid=0x%x",
                        bt_hc_drv.ctrlr_info.cur_buf.entries[c_hndl_idx].conn_hndl,
                        bt_dc_last_hci_ev_reg_app_id, 0);
          }
          else
          {
            cmd_ptr->cmd_hdr.cmd_type  = BT_CMD_HC_FLUSH;
            cmd_ptr->cmd_hdr.bt_app_id = bt_dc_last_hci_ev_reg_app_id;
            cmd_ptr->cmd_msg.cmd_hc_flush.conn_hndl =
                  bt_hc_drv.ctrlr_info.cur_buf.entries[c_hndl_idx].conn_hndl;
          }
#else /* BT_SWDEV_2_1_PB */
          cmd_ptr->cmd_hdr.cmd_type  = BT_CMD_HC_FLUSH;
          cmd_ptr->cmd_hdr.bt_app_id = bt_dc_last_hci_ev_reg_app_id;
          cmd_ptr->cmd_msg.cmd_hc_flush.conn_hndl =
                bt_hc_drv.ctrlr_info.cur_buf.entries[c_hndl_idx].conn_hndl;
#endif /* !BT_SWDEV_2_1_PB */
          bt_cmd( cmd_ptr );
          bt_hc_drv.host_info.hci_tx.acl_tx[c_hndl_idx].buffer_flushed = TRUE;
        }
      } /* if delta_ms > flush_to and tx_status == TRUE */
    } /* if q_cnt > 0 */
  } /* for c_hndl_idx */
} /* bt_hc_drv_l2flush_timer_tick */

#endif /* FEATURE_BT_AVSYNC_SOC */
#endif /* FEATURE_BT_SOC */
#endif /* FEATURE_BT */
