/*====*====*====*====*====*====*====*====*====*====*====*====*====*====*====*

         B L U E T O O T H    E X T E R N A L    P R O F I L E S


GENERAL DESCRIPTION
  This module contains the BT external profile task and the code for
  mapping to selected Open Interface APIs.

Copyright (c) 2004-2009 by QUALCOMM Incorporated. 
All Rights Reserved.
Qualcomm Confidential and Proprietary
*====*====*====*====*====*====*====*====*====*====*====*====*====*====*====*/

/*===========================================================================

                        EDIT HISTORY FOR MODULE

  This section contains comments describing changes made to the module.
  Notice that changes are listed in reverse chronological order. Please
  use ISO format for dates.

  $Header: //source/qcom/qct/wconnect/bthost/pf/rel/00.00.26/src/btpf.c#4 $
  $DateTime: 2009/07/07 16:10:58 $
  $Author: ganeshs $

  when        who  what, where, why
  ----------  ---  -----------------------------------------------------------
  2009-07-07   gs  Changed OI_RFCOMM_RegisterServer() to allow registration to 
                   go through without suspending calling task (in rex_timed_wait())
  2009-04-14   pn  Watermark queues get initialized by dsm_queue_init().
  2009-02-21   gs  Allow SIO close and corresponding indication to upper 
                   layer to include all SIO CLI states
  2008-10-29  gs   Fixed some typos
  2008-10-28  dsn  Add OFFLINE and STOP SIGNALS to the BT PF wait mask.

*    #70        24 Jan 2008           VK
* Intializing pointer after freeing memory in _OI_Free.
*
*    #69        07 Nov 2007           GS
* Added HID profile host role support.  
* Added support for re-queuing BT PF commands.
*
*    #68        23 Aug 2007           DM
* Freeing the SIO RX and TX watermarks in state Open Error.
*
*    #66        15 Jun 2007           PN
* Updated bt_pf_err_oi() to use AEEVA macros for compatibility with safe strings
* APIs.
* 
*    #65        21 May 2007           SSK
* Correct initialization values in bt_pf_init_cmd_buffer() and
* remove compiler warnings.
*
*    #64        27 Mar 2007           JTL
* Better handling of command queue.
*
*    #63        16 Feb 2007           PN
* Corrected edit history from revisions #53 to #62.
*
*    #62        16 Feb 2007           PN
* Made use of safe string functions. Fixed critical lint errors.
*
*    #61        25 Jan 2007           DSN
* Increasing the size of BT PF SIO RX watermark for EDR SOC solutions 
*
*    #60        17 Jan 2007           GS
* Modified bt_pf_write_completed() so that BT PF SIO server state is
* updated only when previous state is SENDING.
*
*    #59        20 Dec 2006           DSN
* Throughput optimization for BT2.0+ EDR based SOC solutions.
*
*    #58        20 Nov 2006           PR
* Added utility APIs while merging OI Code v3.0.14
*
*    #57        14 Nov 2006           GS
* Updated RFCOMM_WriteMBUF() to return error if MBUF dsm item is larger 
* than BT PF SIO TX wm don't exceed size.
*
*    #56        13 Nov 2006           PN
* Fixed a typo in debug message logging in bt_pf_sio_status_change_cb().
*
*    #55        10 Nov 2006           MH
* Initialize client realm info table during BTPF Task init
*
*    #54        18 Oct 2006           PN
* Added more debug messages.
*
*    #53        07 Jun 2006           PR
* Turning on A2DP and AVRCP without turning on other profiles
*
*    #52        30 May 2006           GS
* Added support for PBAP feature.
*
*    #51        24 May 2006           JH
* Removed MT OBEX featurization around added callbacks to support up to 8
* SIO ports.
*
*    #50        23 May 2006           PN
* Added error messages.
*
*    #49        28 Mar 2006           JH
* Added call to initialize GOEP client app-connection table during task init.
* Added featurization around code for initializing GOEP tables.
*
*    #48        16 Nov 2005           GS
* Swapping the order in which WRT_CFM and RCV_IND signals are processed in
* the main event processing loop.  This will prevent BT PF from potentially 
* dropping OBEX packet if there is a race condition.
*
*    #47        11 Nov 2005           SL
* Clear SIO stream ID on switching from OBEX to AT for PCAA.
*
*    #46        27 Oct 2005           PN
* Removed redundant connection state check for data received on SAP connection.
*
*    #45         22 Oct 2005          JH
* Replaced FEATURE_PCAA with FEATURE_PCAA_SIRIUS.
*
*    #44         07 Oct 2005          JH
* Added calls to bt_pf_pcaa_set_active_port() and bt_pf_pcaa_set_active_stream()
*  in bt_pf_pcaa_sio_open().
*
*    #43         04 Oct 2005          GS
* Modified bt_pf_sio_rx_data to allow receiving data in BT_PF_SIO_CLI_SENDING
* state.
*
*    #42         19 Sep 2005          JH
* Added function bt_pf_pcaa_change_mode().
* Modified bt_pf_pcaa_sio_open() to enable DTR callback for USB.
*
*    #41         16 Sep 2005          JTL
* Adding more error codes to the PF event map for Radio Off.
*
*    #40         12 Sep 2005          ABu
* Modified bt_pf_sio_irda_status_change_cb() to add state BT_PF_SIO_SRV_CONNECTED
* so that a server can force a disconnect.
*
*    #39         17 Aug 2005          ABu
* Modified bt_pf_sio_irda_status_change_cb() to get the reason for a transport
* disconnect so that the application knows why the connection failed.
*
*    #38         27 Jul 2005          SL
* Made changes to support PCAA over SPP.
*
*    #37         26 Jul 2005          JH
* Increased profile heap size from 14 to 16 kBytes.
*
*    #36         12 Jul 2005          JH
* Added BT_PF_TMO_SIG to signal mask when calling rex_timed_wait() which was 
*  previously misused.
*
*    #35         06 Jul 2005          JTL
* Fixing pointer and array indexing bugs.
*
*    #34         31 May 2005           RY
* Merged in Sirius changes.
*
*    #33         16 May 2005           PG
* Passed 32 bit variable to INTLOCK_SAV INTFREE_SAV macros.
*
*    #32         4 May 2005           JH
* Increased profile heap size by 2k bytes.
*
*    #31        20 Mar 2005           JTL
* Fix compile error when building without A2DP profiles enabled.
*
*    #30        17 Mar 2005           JH
* Modified OI_RFCOMM_Disconnect() to allow disconnect in sending states.
*
*    #29        17 Mar 2005           JTL
* Fixing error mapping to map all HCI error codes.
*
*    #28        15 Mar 2005           JH
* Increase BT profile heap size from 6 to 12 kB.
*
*    #27        08 Mar 2005           JTL
* Added AV profile code. More helper functions.
*
*    #26        04 Mar 2005           JTL
* Added bytestreams. Augmented MBUF implementation. Added debug printfs.
*
*    #25        18 Feb 2005            PN
* Updated SAP support based on test result from UPF.
*
*    #24        13 Feb 2005            PN
* Added support for SIM Access profile.
*
*    #23        22 Jan 2005            PN
* Added OI_RFCOMM_WriteSegmented() and OI_SECMGR_ResolvePolicies() to support
* SIM Access profile.
*
*    #22       19 Jan 2005        JH
* Removed path "ext\" from source file includes due to compile issues on Linux.
*
*    #21       11 Nov 2004        JH
* Increased profile heap size from 4K to 6K to support increased OBEX client and
*  server max receive buffer size (1K to 2K).
*
*    #20       28 Oct 2004        JH
* Use BT_PF_TMO_SIG for BIP and BPP timers instead of the FTP, BIP and BPP
*  signals.
*
*    #19       08 Oct 2004        JH
* Cap max_frame_size to BT_MTP_RC_INFO_LEN in OI_RFCOMM_Connect and
*  OI_RFCOMM_RegisterServer.
*
*    #18       30 Aug 2004        JH
* Removed bt_pf_set_cod_obj_xfer_bit and bt_pf_clear_cod_obj_xfer_bit. These
*  operations are now performed by btsd.c during service registration.
*
*    #17       26 Aug 2004        JH
* In OI_Rfcomm_WriteMbuf, set dsm_state to BT_PF_MBUF_DSM_SENT before
* sio_transmit is called to avoid situations where DSM is freed multiple times.
*
*    #16       10 Aug 2004        JTL
* Fixing bt_pf_map_event_code.
*
*    #15       06 Aug 2004        JTL
* Added L2 and RM glue code event signals and processing code.
* Init L2 and RM glue code on powerup.
* Created PF equivalent for storing BT events.
*
*    #14       01 Aug 2004        JTL
* Removed DEVMGR stubs, and placed implementation in another file.
* Added bt_pf_map_event_code.
*
*    #13       29 Jul 2004        JTL
* Removed L2 stubs, and placed implementation in another file.
*
*    #12       26 Jul 2004
* Revamped profile signal and timer scheme.
*
*    #10       15 Jun 2004
* Modified bt_pf_sio_rx_data to allow receiving data in BT_PF_SIO_SRV_SENDING
*  state. This is a temporary fix for BIP.
*
*    #9        06 Jun 2004
* Changed profile heap size from 2kbytes to 4kbytes.
*
*    #8        14 May 2004
* Added bt_pf_ev_cb for processing profile events
* Added bt_pf_set_cod_obj_xfer_bit & bt_pf_clear_cod_obj_xfer_bit for
*  manipulating object transfer class of service bit.
*
*    #7        30 Apr 2004            JH
* Added SML support
*
*    #6        23 Apr 2004            JH
* Make changes related to the replacement of profile event control with
*  general BT event control
*
*    #5        14 Apr 2004            JH
* Added BIP and BPP support
* Added tprofile timeout timer and corresponding signal
*
*    #3        25 Feb 2004            JH
* Renamed btpfopp.h btpfoppi.h, btpfftp.h btpfftpi.h, & btpfgoep.h btpfgoepi.h
*
*    #2        24 Feb 2004            JH
* Added code for watchdog report; Updated code for starting task;
* Added support for task stop and task offline from TMC
*

===========================================================================*/

/*===========================================================================

                     INCLUDE FILES FOR MODULE

===========================================================================*/
#include "customer.h"
#include "comdef.h"
#ifdef FEATURE_BT
#ifdef FEATURE_BT_EXTPF
#define BT_MSG_LAYER  BT_MSG_GN
#include "bt.h"
#include "btpfi.h"
#include "oi_status.h"
#include "oi_mbuf.h"
#include "oi_rfcomm.h"
#include "oi_rfcomm_internal.h"
#include "oi_dispatch.h"
#include "oi_memmgr.h"
#include "oi_std_utils.h"
#include "oi_assert.h"
#include "oi_obexspec.h"
#include "oi_config_table.h"
#include "oi_init_flags.h"
#include "oi_devmgr.h"
#include "oi_osinterface.h"
#include "oi_l2cap.h"
#include "oi_l2cap_group.h"
#include "oi_l2cap_qos.h"
#include "memheap.h"
#include "dog.h"
#include "rex.h"
#include "task.h"
#include "tmc.h"
#include "clk.h"
#include "btmsg.h"
#include "bti.h"
#include "btpfcs.h"
#include "btcmdi.h"
#include "btrci.h"
#include "btpfcmdi.h"
#include "btpfgoepi.h"
#include "btsd.h"
#include "btdebug.h"
#include "btutils.h"

#if defined(FEATURE_MT_OBEX) && defined(FEATURE_PCAA_SIRIUS)
#error code not present
#endif /* defined(FEATURE_MT_OBEX) && defined(FEATURE_PCAA_SIRIUS) */

#ifdef FEATURE_BT_EXTPF_HID_HOST
#error code not present
#endif /* FEATURE_BT_EXTPF_HID_HOST */

/*===========================================================================

                            INCLUDE SOURCE FILES

===========================================================================*/
#include "oi_init_flags.c"
#include "oi_bt_stack_config_default.c"
#include "oi_bt_profile_config_default.c"
#include "oi_config_table.c"
#undef __OI_MODULE__
#include "oi_bytestream.c"
#if defined(FEATURE_MT_OBEX) && defined(FEATURE_IRDA)
#error code not present
#endif /* defined(FEATURE_MT_OBEX) && defined(FEATURE_IRDA) */

/*===========================================================================

                DEFINITIONS AND DECLARATIONS FOR MODULE

This section contains definitions for constants, macros, types, variables
and other items needed by this module.

===========================================================================*/

#define BT_PF_REQUEUED_CMD_Q_TIMER_MS 500
#define BT_PF_CMD_MAX_RETRIES         10

/*-------------------------------------------------------------------------*/
/*                                                                         */
/*                         Data Definitions                                */
/*                                                                         */
/*-------------------------------------------------------------------------*/

bt_app_id_type bt_pf_app_id;

/*-------------------------------------------------------------------------*/
/* BT external profile task command queue                                  */
/*-------------------------------------------------------------------------*/
#define BT_PF_NUM_CMDS    20
LOCAL bt_pf_cmd_msg_type bt_pf_cmds[ BT_PF_NUM_CMDS ];
LOCAL q_type             bt_pf_cmd_q;
LOCAL q_type             bt_pf_cmd_re_q;
LOCAL q_type             bt_pf_cmd_free_q;

LOCAL rex_tcb_type*      bt_pf_app_tcb_ptr;

/*-------------------------------------------------------------------------*/
/* BT external profile task master timer                                   */
/*-------------------------------------------------------------------------*/
rex_timer_type           bt_pf_timer;
rex_timer_type           bt_pf_tmo_timer;
LOCAL rex_timer_type     bt_pf_rpt_timer;

/* Requeued command q timeout */
LOCAL rex_timer_type     bt_pf_requeued_cmd_q_timer;

#ifdef FEATURE_BT_EXTPF_FTP
rex_timer_type           bt_pf_ftp_tmo_timer;
#endif /* FEATURE_BT_EXTPF_FTP */

#ifdef FEATURE_BT_EXTPF_BIP
rex_timer_type           bt_pf_bip_cli_tmo_timer;
rex_timer_type           bt_pf_bip_srv_tmo_timer;
#endif /* FEATURE_BT_EXTPF_BIP */

#ifdef FEATURE_BT_EXTPF_BPP
rex_timer_type           bt_pf_bpp_cli_tmo_timer;
rex_timer_type           bt_pf_bpp_srv_tmo_timer;
#endif /* FEATURE_BT_EXTPF_BPP */

#ifdef FEATURE_BT_EXTPF_HID_HOST
#error code not present
#endif /* FEATURE_BT_EXTPF_HID_HOST */

/* BT external profile task master timer tick interval */
#define BT_PF_TIMER_INTVL_100MS 5 /* in multiples of 100 ms */
#define BT_PF_TIMER_INTVL_MS    BT_PF_TIMER_INTVL_100MS * 100

/*-------------------------------------------------------------------------*/
/* Structure for mapping OI_MBUF to DSM                                    */
/*-------------------------------------------------------------------------*/
#define BT_PF_NUM_MBUF 20
LOCAL OI_MBUF bt_pf_mbuf[BT_PF_NUM_MBUF];

/*-------------------------------------------------------------------------*/
/* Heap for dynamic memory allocation                                      */
/*-------------------------------------------------------------------------*/
mem_heap_type   bt_pf_heap;

#ifdef FEATURE_BT_EXTPF_GOEP
#ifdef FEATURE_BT_SOC
#ifdef FEATURE_BT_REDUCE_MEMORY_USAGE
#define BT_PF_HEAP_SIZE (1024 * (64 / 2) * 4)
#else
#define BT_PF_HEAP_SIZE (1024 * 64 * 4)
#endif
#else
#define BT_PF_HEAP_SIZE (1024 * 16)
#endif /* FEATURE_BT_SOC */
#else
#ifdef FEATURE_BT_EXTPF_AV
#define BT_PF_HEAP_SIZE 1024
#endif
#endif

#ifdef FEATURE_BT_SOC
#ifdef FEATURE_BT_REDUCE_MEMORY_USAGE
#define BT_PF_WM_HI   (30000 / 2)
#define BT_PF_WM_MAX  (35000 / 2)
#else
#define BT_PF_WM_HI   (30000)
#define BT_PF_WM_MAX  (35000)
#endif
#else
#define BT_PF_WM_HI   (4000)
#define BT_PF_WM_MAX  (5000)
#endif /* FEATURE_BT_SOC */

LOCAL char bt_pf_heap_buf[BT_PF_HEAP_SIZE];

/*-------------------------------------------------------------------------*/
/* External profile task signals                                           */
/*-------------------------------------------------------------------------*/
/* Signals to wait on in task main loop */
LOCAL const rex_sigs_type bt_pf_wait_sig_mask = (
  BT_PF_TASK_STOP_SIG    |
  BT_PF_TASK_OFFLINE_SIG |
  BT_PF_CMD_Q_SIG        |
  BT_PF_TIMER_SIG        |
  BT_PF_CON_CFM_SIG      |
  BT_PF_CON_IND_SIG      |
  BT_PF_DCN_IND_SIG      |
  BT_PF_RCV_IND_SIG      |
  BT_PF_WRT_CFM_SIG      |
  BT_PF_RLS_IND_SIG      |
  BT_PF_MSC_IND_SIG      |
  BT_PF_RPN_IND_SIG      |
  BT_PF_RPT_TIMER_SIG    |
  BT_PF_L2IF_EV_SIG      |
  BT_PF_RMIF_EV_SIG
#ifdef FEATURE_BT_EXTPF_AV
  | BT_PF_A2DP_EV_SIG 
#endif /* FEATURE_BT_EXTPF_AV */
#if defined(FEATURE_MT_OBEX) && defined(FEATURE_PCAA_SIRIUS)
#error code not present
#endif /* (FEATURE_MT_OBEX) && defined(FEATURE_PCAA_SIRIUS) */
#ifdef FEATURE_BT_EXTPF_HID_HOST
#error code not present
#endif /* FEATURE_BT_EXTPF_HID_HOST */
  );

/* Channel masks for each signal */
/* For each channel mask defined, a 1 in the bit location indicates that  */
/* the the signal is active for the corresponding SIO connection channel. */
/* e.g. a channel mask of 0x05 indicates that the signal is active for    */
/* channels 0 and 2. */
typedef struct
{
  uint32 con_cfm;
  uint32 con_ind;
  uint32 dcn_ind;
  uint32 wrt_cfm;
  uint32 rcv_ind;
  uint32 rls_ind;
  uint32 msc_ind;
  uint32 rpn_ind;
} bt_pf_ch_masks_type;

LOCAL bt_pf_ch_masks_type bt_pf_ch_masks;

/*-------------------------------------------------------------------------*/
/* Data for implementing dispatcher                                        */
/*-------------------------------------------------------------------------*/
#define BT_PF_MAX_DISPATCH 8
#define BT_PF_DISPATCH_TABLE_FULL -1
#define BT_PF_MAX_INLINE_ARG_SIZE 8

/* A callback handle is a 32 bit value that encodes the callback entry */
/* index along with a unique salt. */
/* Top 16 bits are the entry index */
#define CB_INDEX(h)  ((uint16) ((h) >> 16))
/* Bottom 16 bits are the salt */
#define CB_SALT(h)   ((uint32) (((uint32) (h)) & 0xFFFF))

/* Generate a callback handle from an entry index. */
#define CB_HANDLE(i, s) ( (DISPATCH_CB_HANDLE) ((((uint32) (i)) << 16) | \
                          CB_SALT(s)) )

/* Dispatcher table entry */
typedef struct
{
  DISPATCH_CB_FUNC   cb_ptr;
  byte*              arg_ptr;
  byte               inline_arg[BT_PF_MAX_INLINE_ARG_SIZE];
  uint16             arg_size;
  rex_timer_cnt_type tmo;
  uint16             salt; /* for generating unique dispatcher cb handle */
} bt_pf_dispatch_item_type;

/* Dispatcher table */
LOCAL bt_pf_dispatch_item_type bt_pf_dispatcher[BT_PF_MAX_DISPATCH];

/*-------------------------------------------------------------------------*/
/* Data for mapping OI_RFCOMM calls to SIO calls                           */
/*-------------------------------------------------------------------------*/

/* SIO connection states */
typedef enum
{
  BT_PF_SIO_NO_STATE,
  BT_PF_SIO_CLI_FIRST_STATE,
  BT_PF_SIO_CLI_NO_CONNECT = BT_PF_SIO_CLI_FIRST_STATE,
  BT_PF_SIO_CLI_PENDING_CFG,
  BT_PF_SIO_CLI_CONNECTED,
  BT_PF_SIO_CLI_SENDING,
  BT_PF_SIO_CLI_LAST_STATE = BT_PF_SIO_CLI_SENDING,
  BT_PF_SIO_SRV_FIRST_STATE,
  BT_PF_SIO_SRV_NO_CONNECT = BT_PF_SIO_SRV_FIRST_STATE,
  BT_PF_SIO_SRV_PENDING_CFG,
  BT_PF_SIO_SRV_REGISTERED,
  BT_PF_SIO_SRV_PENDING_ACCEPT,
  BT_PF_SIO_SRV_CONNECTED,
  BT_PF_SIO_SRV_SENDING,
  BT_PF_SIO_SRV_LAST_STATE = BT_PF_SIO_SRV_SENDING
} bt_pf_sio_state_type;

LOCAL boolean bt_pf_rx_before_accept = FALSE;

/* Context of the MBUF written when OI_RFCOMM_Write is called */
typedef struct
{
  OI_RFCOMM_WRITE_CFM   writeCfm;
  OI_BYTE               *buf;
  OI_UINT16             bufLen;
  OI_RFCOMM_LINK_HANDLE linkHandle;
} BT_PF_WRITE_MBUF_CONTEXT;

/* Struct storing info related to connect confirmation callback */
typedef struct
{
  OI_RFCOMM_CONNECT_CFM      fptr;
  OI_STATUS                  result;
} bt_pf_con_cfm_info_type;

/* Struct storing info related to connect indication callback */
typedef struct
{
  OI_RFCOMM_CONNECT_IND      fptr;
  bt_bd_addr_type            remote_bd_addr;
} bt_pf_con_ind_info_type;

/* Struct storing info related to disconnect indication callback */
typedef struct
{
  OI_RFCOMM_DISCONNECT_IND   fptr;
  OI_STATUS                  reason;
} bt_pf_dcn_ind_info_type;

/* Struct storing info related to write confirmation callback */
typedef struct
{
  OI_RFCOMM_WRITE_MBUF_CFM   fptr;
  OI_STATUS                  result;
} bt_pf_wrt_cfm_info_type;

/* Struct storing info related to receive indication callback */
typedef struct
{
  OI_RFCOMM_RECV_DATA_IND    fptr;
} bt_pf_rcv_ind_info_type;

/* Struct storing info related to RLS indication callback */
typedef struct
{
  OI_RFCOMM_LINE_STATUS_IND  fptr;
  uint8                      status;
} bt_pf_rls_ind_info_type;

/* Struct storing info related to MSC indication callback */
typedef struct
{
  OI_RFCOMM_LINE_CONTROL_IND fptr;
  uint8                      line_ctrl;
  uint8                      break_ctrl;
} bt_pf_msc_ind_info_type;

/* Struct storing info related to RPN indication callback */
typedef struct
{
  OI_RFCOMM_PORT_PARAM_IND   fptr;
  OI_RFCOMM_PORTPARAM        port_param;
} bt_pf_rpn_ind_info_type;

/* Struct storing info related to a SIO connection */
typedef struct
{
  sio_stream_id_type          stream_id;
  bt_pf_sio_state_type        state;
  uint8                       scn;
  uint16                      uuid;
  uint16                      max_frame_size;
  OI_MBUF*                    mbuf_ptr;
  void*                       link_context;
  void*                       server_context;
  dsm_watermark_type          rx_wm;
  dsm_watermark_type          tx_wm;
  q_type                      to_sio_q;
  q_type                      from_sio_q;

  /* SIO callbacks */
  bt_spp_status_fptr_type     spp_stat_fptr;
  bt_spp_cfg_rpt_fptr_type    spp_cfg_fptr;
  bt_spp_ms_fptr_type         spp_ms_fptr;
  bt_spp_le_fptr_type         spp_le_fptr;

  /* Open Interface defined RFCOMM callbacks */
  bt_pf_con_cfm_info_type     con_cfm;
  bt_pf_con_ind_info_type     con_ind;
  bt_pf_dcn_ind_info_type     dcn_ind;
  bt_pf_wrt_cfm_info_type     wrt_cfm;
  bt_pf_rcv_ind_info_type     rcv_ind;
  bt_pf_rls_ind_info_type     rls_ind;
  bt_pf_msc_ind_info_type     msc_ind;
  bt_pf_rpn_ind_info_type     rpn_ind;
} bt_pf_sio_conn_type;

#define SIO_STREAM_RESERVED -2
#define BT_PF_SIO_ENTRY_NOT_FOUND -1

#define RLS_NO_ERROR      ( 0x00 )
#define RLS_OVERRUN_ERROR ( RCMCCM_RLS_V1_LS_ERR_B | \
                            RCMCCM_RLS_V1_LSET_OVERRUN_V )
#define RLS_PARITY_ERROR  ( RCMCCM_RLS_V1_LS_ERR_B | \
                            RCMCCM_RLS_V1_LSET_PARITY_V )
#define RLS_FRAMING_ERROR ( RCMCCM_RLS_V1_LS_ERR_B | \
                            RCMCCM_RLS_V1_LSET_FRAMING_V )

/* SIO connection table */
LOCAL bt_pf_sio_conn_type bt_pf_sio_table[BT_PF_SIO_MAX_PORTS];

LOCAL uint32 bt_pf_ch_bit[BT_PF_SIO_MAX_PORTS];

/** The zero address.  Used to test for an uninitialized address. */
const OI_BD_ADDR OI_ZeroAddr = { {0,0,0,0,0,0} };

/*-------------------------------------------------------------------------*/
/*   Map QCom BT Event codes to OI status                                  */
/*-------------------------------------------------------------------------*/
typedef struct {
  bt_event_reason_type reason;
  bt_cmd_status_type   status;
} bt_pf_event_map_type;

LOCAL const bt_pf_event_map_type bt_pf_event_map[] =
  {
    /* L2 reasons */
    { BT_EVR_L2_CONFIG_PARAMS_NOT_AGREEABLE, OI_L2CAP_CONFIG_FAIL_INVALID_PARAMETERS },
    { BT_EVR_L2_LOCAL_REJECT_CONNECTION,     OI_L2CAP_CONNECT_REFUSED_NO_RESOURCES },
    { BT_EVR_L2_REMOTE_REJECT_CONNECTION,    OI_L2CAP_CONNECT_REFUSED_NO_RESOURCES },
    { BT_EVR_L2_PAGE_FAILED,                 OI_L2CAP_CONNECT_TIMEOUT },
    { BT_EVR_L2_LINK_LOST,                   OI_L2CAP_DISCONNECT_LOWER_LAYER },
    { BT_EVR_L2_AUTHENTICATION_FAILED,       OI_L2CAP_CONNECT_REFUSED_SECURITY },
    { BT_EVR_L2_UNSPECIFIED_ERROR,           OI_FAIL },
    { BT_EVR_L2_NORMAL_DISCONNECT,           OI_L2CAP_DISCONNECT_REMOTE_REQUEST },
    { BT_EVR_L2_PING_SUCCESS,                OI_OK },
    { BT_EVR_L2_PING_FAILED,                 OI_L2CAP_PING_TIMEOUT },
    { BT_EVR_L2_CONFIG_SUCCESS,              OI_OK },
    /* RM Reasons */
    { BT_EVR_RM_NO_CONNECTION,               OI_STATUS_NOT_CONNECTED },
    { BT_EVR_RM_NO_RESOURCES,                OI_STATUS_NO_RESOURCES },
    { BT_EVR_RM_SECURITY_NOT_MET,            OI_L2CAP_CONNECT_REFUSED_SECURITY },
    { BT_EVR_RM_RADIO_OFF,                   OI_STATUS_RESET_IN_PROGRESS },
    /* Must be last: */
    { BT_EVR_GN_SUCCESS,                     OI_OK }
  };


#ifdef FEATURE_MT_OBEX
#ifdef FEATURE_PCAA_SIRIUS
#error code not present
#endif /* FEATURE_PCAA_SIRIUS */

#ifdef FEATURE_IRDA
#error code not present
#endif /* FEATURE_IRDA */

#endif /* FEATURE_MT_OBEX */
/*===========================================================================

       FUNCTIONS FOR MAPPING OI RFCOMM CALLBACKS TO SIO CALLBACKS

===========================================================================*/

/*===========================================================================

FUNCTION
  bt_pf_find_table_entry_sid

DESCRIPTION
  Locates the index of the SIO connection table entry specified by the
  stream ID. When stream_id is SIO_NO_STREAM_ID, this function reserves
  the next free entry for use by the calling function.

===========================================================================*/
int16 bt_pf_find_table_entry_sid(sio_stream_id_type stream_id)
{
  int16 i, entry = BT_PF_SIO_ENTRY_NOT_FOUND;

  INTLOCK();
  for (i = 0; i < BT_PF_SIO_MAX_PORTS; i++)
  {
    if (bt_pf_sio_table[i].stream_id == stream_id)
    {
      if (stream_id == SIO_NO_STREAM_ID)
      {
        bt_pf_sio_table[i].stream_id = SIO_STREAM_RESERVED;
      }
      entry = i;
      break;
    }
  }
  INTFREE();
  return(entry);
}

/*===========================================================================

FUNCTION
  bt_pf_find_table_entry_scn

DESCRIPTION
  Locates the index of the SIO connection table entry specified by the server
  channel number.

===========================================================================*/
int16 bt_pf_find_table_entry_scn(uint8 server_channel)
{
  int16 i;

  for (i = 0; i < BT_PF_SIO_MAX_PORTS; i++)
  {
    if (bt_pf_sio_table[i].scn == server_channel)
    {
      return(i);
    }
  }
  return(BT_PF_SIO_ENTRY_NOT_FOUND);
}

/*===========================================================================

FUNCTION
  bt_pf_free_table_entry

DESCRIPTION
  Frees a previously assigned entry in the SIO connection table.

===========================================================================*/
void bt_pf_free_table_entry(uint16 i)
{
  bt_pf_sio_table[i].stream_id = SIO_NO_STREAM_ID;
  bt_pf_sio_table[i].state = BT_PF_SIO_NO_STATE;
  bt_pf_sio_table[i].scn = BT_RC_SC_NULL;
  bt_pf_sio_table[i].con_cfm.fptr = NULL;
  bt_pf_sio_table[i].wrt_cfm.fptr = NULL;
  bt_pf_sio_table[i].con_ind.fptr = NULL;
  bt_pf_sio_table[i].dcn_ind.fptr = NULL;
  bt_pf_sio_table[i].rcv_ind.fptr = NULL;
  bt_pf_sio_table[i].msc_ind.fptr = NULL;
  bt_pf_sio_table[i].rls_ind.fptr = NULL;
  bt_pf_sio_table[i].rpn_ind.fptr = NULL;
  bt_pf_sio_table[i].mbuf_ptr = NULL;

  memset( (byte*)(&bt_pf_sio_table[i].con_ind.remote_bd_addr), 0,
          sizeof( bt_bd_addr_type ) );

  /* clear ch mask bit in case it is set */
  bt_pf_ch_masks.con_cfm &= ~bt_pf_ch_bit[i];
  bt_pf_ch_masks.con_ind &= ~bt_pf_ch_bit[i];
  bt_pf_ch_masks.dcn_ind &= ~bt_pf_ch_bit[i];
  bt_pf_ch_masks.msc_ind &= ~bt_pf_ch_bit[i];
  bt_pf_ch_masks.rcv_ind &= ~bt_pf_ch_bit[i];
  bt_pf_ch_masks.rls_ind &= ~bt_pf_ch_bit[i];
  bt_pf_ch_masks.rpn_ind &= ~bt_pf_ch_bit[i];
  bt_pf_ch_masks.wrt_cfm &= ~bt_pf_ch_bit[i];

  return;
}

/*===========================================================================

FUNCTION
  bt_pf_sio_status_change_cb

DESCRIPTION
  This is the SIO status change callback function. It responds to
  status changes of the SIO stream by signaling the appropriate OI RFCOMM
  callbacks to be invoked.

===========================================================================*/
void bt_pf_sio_status_change_cb(uint16 i, bt_spp_status_type* status_ptr)
{
  BT_MSG_DEBUG( "BT PF: SPP St Chg, i|sid=0x%04x uuid=%x sppSt|st=0x%04x",
                ((uint16)(i << 8) | status_ptr->stream_id), 
                bt_pf_sio_table[i].uuid, 
                ((uint16)(status_ptr->spp_state << 8) | 
                bt_pf_sio_table[i].state) );
  switch (status_ptr->spp_state)
  {
    case BT_SPP_ST_CONNECTED:
      if (bt_pf_sio_table[i].state == BT_PF_SIO_CLI_NO_CONNECT)
      {
        /* connected to server, need to wait for config change cb */
        bt_pf_sio_table[i].state = BT_PF_SIO_CLI_PENDING_CFG;
        bt_pf_sio_table[i].stream_id = status_ptr->stream_id;
      }
      else if (bt_pf_sio_table[i].state == BT_PF_SIO_SRV_REGISTERED)
      {
        /* connected to client, need to wait for config change cb */
        bt_pf_sio_table[i].state = BT_PF_SIO_SRV_PENDING_CFG;
        bt_pf_sio_table[i].con_ind.remote_bd_addr = status_ptr->bd_addr;
      }
      break;
    case BT_SPP_ST_DISCONNECTED: /* only applicable to client */
      if ( (bt_pf_sio_table[i].state >= BT_PF_SIO_CLI_NO_CONNECT) &&
           (bt_pf_sio_table[i].state <= BT_PF_SIO_CLI_SENDING) )
      {
        /* connection with server terminated or cancelled */
        bt_pf_sio_table[i].dcn_ind.reason = OI_RFCOMM_SESSION_SHUTDOWN;
        bt_pf_ch_masks.dcn_ind |= bt_pf_ch_bit[i];
        rex_set_sigs( &bt_pf_tcb, BT_PF_DCN_IND_SIG );
      }

      break;
    case BT_SPP_ST_OPENING:
    case BT_SPP_ST_OPEN: /* only applicable to server */
      if ( (bt_pf_sio_table[i].state == BT_PF_SIO_SRV_CONNECTED) ||
           (bt_pf_sio_table[i].state == BT_PF_SIO_SRV_SENDING) )
      {
        /* connection with client terminated */
        bt_pf_sio_table[i].dcn_ind.reason = OI_RFCOMM_SESSION_SHUTDOWN;
        bt_pf_ch_masks.dcn_ind |= bt_pf_ch_bit[i];
        rex_set_sigs( &bt_pf_tcb, BT_PF_DCN_IND_SIG );
      }
      else if (
            (bt_pf_sio_table[i].state == BT_PF_SIO_SRV_PENDING_CFG) ||
            (bt_pf_sio_table[i].state == BT_PF_SIO_SRV_PENDING_ACCEPT) )
      {
        /* connection with server terminated before config change cb is
           received or before connection is accepted by application */
        bt_pf_sio_table[i].state = BT_PF_SIO_SRV_REGISTERED;
        memset( (byte*)(&bt_pf_sio_table[i].con_ind.remote_bd_addr), 0,
                sizeof( bt_bd_addr_type ) );
        dsm_empty_queue( &(bt_pf_sio_table[i].tx_wm) );
        dsm_empty_queue( &(bt_pf_sio_table[i].rx_wm) );
      }
      else if (bt_pf_sio_table[i].state == BT_PF_SIO_SRV_NO_CONNECT)
      {
        /* server registration completed */
        bt_pf_sio_table[i].state = BT_PF_SIO_SRV_REGISTERED;
        bt_pf_sio_table[i].stream_id = status_ptr->stream_id;
        bt_pf_sio_table[i].scn = status_ptr->rc_server_channel;
      }
      break;
    case BT_SPP_ST_OPEN_ERROR:
      /* In case of Open Errors clear out any queued data in the 
       * SIO watermarks */
      dsm_empty_queue( &(bt_pf_sio_table[i].tx_wm) );
      dsm_empty_queue( &(bt_pf_sio_table[i].rx_wm) );

      if (bt_pf_sio_table[i].state == BT_PF_SIO_CLI_NO_CONNECT)
      {
        /* connection to server failed */
        bt_pf_sio_table[i].con_cfm.result = OI_RFCOMM_ESTABLISH_FAILURE;
        bt_pf_sio_table[i].max_frame_size = 0;
        bt_pf_ch_masks.con_cfm |= bt_pf_ch_bit[i];
        rex_set_sigs( &bt_pf_tcb, BT_PF_CON_CFM_SIG );
      }
      else if (bt_pf_sio_table[i].state == BT_PF_SIO_SRV_NO_CONNECT)
      {
        /* service registration failed */
        BT_ERR("BT PF: error in server registration!, uuid=0x%x, stream_id=%d",
               bt_pf_sio_table[i].uuid, bt_pf_sio_table[i].stream_id, 0);

        /* clean up */
        sio_close(bt_pf_sio_table[i].stream_id, NULL);
        bt_pf_free_table_entry(i);
      }
      break;
  }
}

#ifdef FEATURE_MT_OBEX

#ifdef FEATURE_IRDA
#error code not present
#endif /* FEATURE_IRDA */

#if defined(FEATURE_MT_OBEX) && defined(FEATURE_PCAA_SIRIUS)
#error code not present
#endif /* defined(FEATURE_MT_OBEX) && defined(FEATURE_PCAA_SIRIUS) */

#endif /* FEATURE_MT_OBEX */

/*===========================================================================

FUNCTION
  bt_pf_sio_config_change_cb

DESCRIPTION
  This is the SIO configuration change callback function. It responds to
  configuration changes of the SIO stream by signaling OI RFCOMM callbacks
  to be invoked.

===========================================================================*/
void bt_pf_sio_config_change_cb(uint16 i,
                                   bt_spp_cfg_rpt_type* config_rpt_ptr)
{
  /* handle RPN */
  if (bt_pf_sio_table[i].rpn_ind.fptr != NULL)
  {
    bt_pf_sio_table[i].rpn_ind.port_param.mask = OI_RFCOMM_MASK_ALL;
    bt_pf_sio_table[i].rpn_ind.port_param.baudRate =
                  config_rpt_ptr->spp_config.baudrate;
    bt_pf_sio_table[i].rpn_ind.port_param.dataBits =
                  config_rpt_ptr->spp_config.format & RCMCCM_RPN_FMT_D_M;
    bt_pf_sio_table[i].rpn_ind.port_param.stopBits =
                  config_rpt_ptr->spp_config.format & RCMCCM_RPN_FMT_S_1P5_B;
    bt_pf_sio_table[i].rpn_ind.port_param.parity =
                  config_rpt_ptr->spp_config.format & RCMCCM_RPN_FMT_P_B;
    bt_pf_sio_table[i].rpn_ind.port_param.parityType =
                  config_rpt_ptr->spp_config.format & RCMCCM_RPN_FMT_PT_M;
    bt_pf_sio_table[i].rpn_ind.port_param.flowControlType =
                  config_rpt_ptr->spp_config.flow_ctrl;
    bt_pf_sio_table[i].rpn_ind.port_param.XON_Char =
                  config_rpt_ptr->spp_config.xon_char;
    bt_pf_sio_table[i].rpn_ind.port_param.XOFF_Char =
                  config_rpt_ptr->spp_config.xoff_char;
  }

  if (bt_pf_sio_table[i].state == BT_PF_SIO_CLI_PENDING_CFG)
  {
    /* config change cb received, connection to server complete */
    bt_pf_sio_table[i].state = BT_PF_SIO_CLI_CONNECTED;
    bt_pf_sio_table[i].con_cfm.result = OI_OK;
    bt_pf_sio_table[i].max_frame_size =
                  config_rpt_ptr->spp_config.max_frame_size;
    bt_pf_ch_masks.con_cfm |= bt_pf_ch_bit[i];
    rex_set_sigs( &bt_pf_tcb, BT_PF_CON_CFM_SIG );
    /* signals RPN, which has a lower priority than connect confirm */
    if (bt_pf_sio_table[i].rpn_ind.fptr != NULL)
    {
      bt_pf_ch_masks.rpn_ind |= bt_pf_ch_bit[i];
      rex_set_sigs( &bt_pf_tcb, BT_PF_RPN_IND_SIG );
    }
  }
  else if (bt_pf_sio_table[i].state == BT_PF_SIO_SRV_PENDING_CFG)
  {
    /* config change cb received, connection with client complete */
    bt_pf_sio_table[i].state = BT_PF_SIO_SRV_PENDING_ACCEPT;
    bt_pf_sio_table[i].max_frame_size =
                  config_rpt_ptr->spp_config.max_frame_size;
    bt_pf_ch_masks.con_ind |= bt_pf_ch_bit[i];
    rex_set_sigs( &bt_pf_tcb, BT_PF_CON_IND_SIG );
    /* not signaling RPN here, connection has to be accepted first */
    /* so RPN is signaled in OI_RFCOMM_Accept */
  }
  /* handle RPN */
  else if ( (bt_pf_sio_table[i].state == BT_PF_SIO_CLI_CONNECTED) ||
            (bt_pf_sio_table[i].state == BT_PF_SIO_SRV_CONNECTED) )
  {
    if (bt_pf_sio_table[i].rpn_ind.fptr != NULL)
    {
      bt_pf_ch_masks.rpn_ind |= bt_pf_ch_bit[i];
      rex_set_sigs( &bt_pf_tcb, BT_PF_RPN_IND_SIG );
    }
  }
}

/*===========================================================================

FUNCTION
  bt_pf_sio_modem_status_cb

DESCRIPTION
  This is the SIO modem status change callback function. It responds to
  modem status changes of the SIO stream by signaling the OI RFCOMM
  line control indication callback to be invoked.

===========================================================================*/
void bt_pf_sio_modem_status_cb(uint16 i, bt_spp_ms_type* modem_status_ptr)
{
  if (bt_pf_sio_table[i].msc_ind.fptr != NULL)
  {
    bt_pf_sio_table[i].msc_ind.line_ctrl =
                  (uint8)modem_status_ptr->modem_status;
    if (modem_status_ptr->break_present)
    {
      bt_pf_sio_table[i].msc_ind.break_ctrl =
                    modem_status_ptr->break_length;
    }
    else
    {
      bt_pf_sio_table[i].msc_ind.break_ctrl = 0;
    }
    bt_pf_ch_masks.msc_ind |= bt_pf_ch_bit[i];
    rex_set_sigs( &bt_pf_tcb, BT_PF_MSC_IND_SIG );
  }
}

/*===========================================================================

FUNCTION
  bt_pf_sio_line_error_cb

DESCRIPTION
  This is the SIO line error callback function. It responds to
  line error in the SIO stream by signaling the OI RFCOMM line status
  indication callback to be invoked.

===========================================================================*/
void bt_pf_sio_line_error_cb(uint16 i, bt_spp_le_type* line_error_ptr)
{
  if (bt_pf_sio_table[i].rls_ind.fptr != NULL)
  {
    switch(line_error_ptr->cur_line_error)
    {
      case BT_SPP_LE_OVERRUN_ERROR:
        bt_pf_sio_table[i].rls_ind.status = RLS_OVERRUN_ERROR;
        break;
      case BT_SPP_LE_PARITY_ERROR:
        bt_pf_sio_table[i].rls_ind.status = RLS_PARITY_ERROR;
        break;
      case BT_SPP_LE_FRAMING_ERROR:
        bt_pf_sio_table[i].rls_ind.status = RLS_FRAMING_ERROR;
        break;
      default:
        bt_pf_sio_table[i].rls_ind.status = RLS_NO_ERROR;
    }
    bt_pf_ch_masks.rls_ind |= bt_pf_ch_bit[i];
    rex_set_sigs( &bt_pf_tcb, BT_PF_RLS_IND_SIG );
  }
}

/*===========================================================================

FUNCTION
  bt_pf_sio_tx_done

DESCRIPTION
  This is the SIO TX watermark gone empty callback function. It responds to
  transmission completion in the SIO stream by signaling the OI RFCOMM write
  confirmation callback to be invoked.

===========================================================================*/
void bt_pf_sio_tx_done(uint16 i)
{
  if ( (bt_pf_sio_table[i].state == BT_PF_SIO_CLI_SENDING) ||
       (bt_pf_sio_table[i].state == BT_PF_SIO_SRV_SENDING) )
  {
    bt_pf_sio_table[i].wrt_cfm.result = OI_OK;
    bt_pf_ch_masks.wrt_cfm |= bt_pf_ch_bit[i];
    rex_set_sigs( &bt_pf_tcb, BT_PF_WRT_CFM_SIG );
  }
  else
  {
    BT_ERR( "BT PF: SID %x TX done in unexpected state %x",
            bt_pf_sio_table[i].stream_id, bt_pf_sio_table[i].state, 0 );
  }
}

/*===========================================================================

FUNCTION
  bt_pf_sio_rx_data

DESCRIPTION
  This is the SIO RX watermark non-empty callback function. It responds to
  data reception in the SIO stream by signaling the OI RFCOMM receive
  indication callback to be invoked.

===========================================================================*/
void bt_pf_sio_rx_data(uint16 i)
{
  if ( (bt_pf_sio_table[i].state == BT_PF_SIO_CLI_CONNECTED) ||
       (bt_pf_sio_table[i].state == BT_PF_SIO_SRV_CONNECTED) ||
       (bt_pf_sio_table[i].state == BT_PF_SIO_CLI_SENDING)   || 
       (bt_pf_sio_table[i].state == BT_PF_SIO_SRV_SENDING) )     /* DEBUG */
  {
    bt_pf_ch_masks.rcv_ind |= bt_pf_ch_bit[i];
    rex_set_sigs( &bt_pf_tcb, BT_PF_RCV_IND_SIG );
  }
  else if ( (bt_pf_sio_table[i].state == BT_PF_SIO_SRV_PENDING_ACCEPT) ||
            (bt_pf_sio_table[i].state == BT_PF_SIO_SRV_REGISTERED) )
  {
    BT_MSG_DEBUG( "BT PF: SID %x RX data before accepted, State: %x",
            bt_pf_sio_table[i].stream_id, bt_pf_sio_table[i].state, 0 );
    bt_pf_rx_before_accept = TRUE;
  }
  else
  {
    BT_ERR( "BT PF: SID %x RX data in unexpected state: %x",
            bt_pf_sio_table[i].stream_id, bt_pf_sio_table[i].state, 0 );
  }
}

/*===========================================================================

 The following functions maps the hard-coded callback for each channel
 to the corresponding general routine defined above.

===========================================================================*/
void bt_pf_sio_status_change_cb_0(bt_spp_status_type* status_ptr)
{
  bt_pf_sio_status_change_cb( 0, status_ptr );
}

void bt_pf_sio_status_change_cb_1(bt_spp_status_type* status_ptr)
{
  bt_pf_sio_status_change_cb( 1, status_ptr );
}

void bt_pf_sio_status_change_cb_2(bt_spp_status_type* status_ptr)
{
  bt_pf_sio_status_change_cb( 2, status_ptr );
}

void bt_pf_sio_status_change_cb_3(bt_spp_status_type* status_ptr)
{
  bt_pf_sio_status_change_cb( 3, status_ptr );
}

void bt_pf_sio_status_change_cb_4(bt_spp_status_type* status_ptr)
{
  bt_pf_sio_status_change_cb( 4, status_ptr );
}

void bt_pf_sio_status_change_cb_5(bt_spp_status_type* status_ptr)
{
  bt_pf_sio_status_change_cb( 5, status_ptr );
}

void bt_pf_sio_status_change_cb_6(bt_spp_status_type* status_ptr)
{
  bt_pf_sio_status_change_cb( 6, status_ptr );
}

void bt_pf_sio_status_change_cb_7(bt_spp_status_type* status_ptr)
{
  bt_pf_sio_status_change_cb( 7, status_ptr );
}

void bt_pf_sio_config_change_cb_0(bt_spp_cfg_rpt_type* config_rpt_ptr)
{
  bt_pf_sio_config_change_cb( 0, config_rpt_ptr );
}

void bt_pf_sio_config_change_cb_1(bt_spp_cfg_rpt_type* config_rpt_ptr)
{
  bt_pf_sio_config_change_cb( 1, config_rpt_ptr );
}

void bt_pf_sio_config_change_cb_2(bt_spp_cfg_rpt_type* config_rpt_ptr)
{
  bt_pf_sio_config_change_cb( 2, config_rpt_ptr );
}

void bt_pf_sio_config_change_cb_3(bt_spp_cfg_rpt_type* config_rpt_ptr)
{
  bt_pf_sio_config_change_cb( 3, config_rpt_ptr );
}

void bt_pf_sio_config_change_cb_4(bt_spp_cfg_rpt_type* config_rpt_ptr)
{
  bt_pf_sio_config_change_cb( 4, config_rpt_ptr );
}

void bt_pf_sio_config_change_cb_5(bt_spp_cfg_rpt_type* config_rpt_ptr)
{
  bt_pf_sio_config_change_cb( 5, config_rpt_ptr );
}

void bt_pf_sio_config_change_cb_6(bt_spp_cfg_rpt_type* config_rpt_ptr)
{
  bt_pf_sio_config_change_cb( 6, config_rpt_ptr );
}

void bt_pf_sio_config_change_cb_7(bt_spp_cfg_rpt_type* config_rpt_ptr)
{
  bt_pf_sio_config_change_cb( 7, config_rpt_ptr );
}

void bt_pf_sio_modem_status_cb_0(bt_spp_ms_type* modem_status_ptr)
{
  bt_pf_sio_modem_status_cb( 0, modem_status_ptr );
}

void bt_pf_sio_modem_status_cb_1(bt_spp_ms_type* modem_status_ptr)
{
  bt_pf_sio_modem_status_cb( 1, modem_status_ptr );
}

void bt_pf_sio_modem_status_cb_2(bt_spp_ms_type* modem_status_ptr)
{
  bt_pf_sio_modem_status_cb( 2, modem_status_ptr );
}

void bt_pf_sio_modem_status_cb_3(bt_spp_ms_type* modem_status_ptr)
{
  bt_pf_sio_modem_status_cb( 3, modem_status_ptr );
}

void bt_pf_sio_modem_status_cb_4(bt_spp_ms_type* modem_status_ptr)
{
  bt_pf_sio_modem_status_cb( 4, modem_status_ptr );
}

void bt_pf_sio_modem_status_cb_5(bt_spp_ms_type* modem_status_ptr)
{
  bt_pf_sio_modem_status_cb( 5, modem_status_ptr );
}

void bt_pf_sio_modem_status_cb_6(bt_spp_ms_type* modem_status_ptr)
{
  bt_pf_sio_modem_status_cb( 6, modem_status_ptr );
}

void bt_pf_sio_modem_status_cb_7(bt_spp_ms_type* modem_status_ptr)
{
  bt_pf_sio_modem_status_cb( 7, modem_status_ptr );
}

void bt_pf_sio_line_error_cb_0(bt_spp_le_type* line_error_ptr)
{
  bt_pf_sio_line_error_cb( 0, line_error_ptr );
}

void bt_pf_sio_line_error_cb_1(bt_spp_le_type* line_error_ptr)
{
  bt_pf_sio_line_error_cb( 1, line_error_ptr );
}

void bt_pf_sio_line_error_cb_2(bt_spp_le_type* line_error_ptr)
{
  bt_pf_sio_line_error_cb( 2, line_error_ptr );
}

void bt_pf_sio_line_error_cb_3(bt_spp_le_type* line_error_ptr)
{
  bt_pf_sio_line_error_cb( 3, line_error_ptr );
}

void bt_pf_sio_line_error_cb_4(bt_spp_le_type* line_error_ptr)
{
  bt_pf_sio_line_error_cb( 4, line_error_ptr );
}

void bt_pf_sio_line_error_cb_5(bt_spp_le_type* line_error_ptr)
{
  bt_pf_sio_line_error_cb( 5, line_error_ptr );
}

void bt_pf_sio_line_error_cb_6(bt_spp_le_type* line_error_ptr)
{
  bt_pf_sio_line_error_cb( 6, line_error_ptr );
}

void bt_pf_sio_line_error_cb_7(bt_spp_le_type* line_error_ptr)
{
  bt_pf_sio_line_error_cb( 7, line_error_ptr );
}

void bt_pf_sio_tx_done_cb_0()
{
  bt_pf_sio_tx_done( 0 );
}

void bt_pf_sio_rx_data_cb_0()
{
  bt_pf_sio_rx_data( 0 );
}

void bt_pf_sio_tx_done_cb_1()
{
  bt_pf_sio_tx_done( 1 );
}

void bt_pf_sio_rx_data_cb_1()
{
  bt_pf_sio_rx_data( 1 );
}

void bt_pf_sio_tx_done_cb_2()
{
  bt_pf_sio_tx_done( 2 );
}

void bt_pf_sio_rx_data_cb_2()
{
  bt_pf_sio_rx_data( 2 );
}

void bt_pf_sio_tx_done_cb_3()
{
  bt_pf_sio_tx_done( 3 );
}

void bt_pf_sio_rx_data_cb_3()
{
  bt_pf_sio_rx_data( 3 );
}

void bt_pf_sio_tx_done_cb_4()
{
  bt_pf_sio_tx_done( 4 );
}

void bt_pf_sio_rx_data_cb_4()
{
  bt_pf_sio_rx_data( 4 );
}

void bt_pf_sio_tx_done_cb_5()
{
  bt_pf_sio_tx_done( 5 );
}

void bt_pf_sio_rx_data_cb_5()
{
  bt_pf_sio_rx_data( 5 );
}

void bt_pf_sio_tx_done_cb_6()
{
  bt_pf_sio_tx_done( 6 );
}

void bt_pf_sio_rx_data_cb_6()
{
  bt_pf_sio_rx_data( 6 );
}

void bt_pf_sio_tx_done_cb_7()
{
  bt_pf_sio_tx_done( 7 );
}

void bt_pf_sio_rx_data_cb_7()
{
  bt_pf_sio_rx_data( 7 );
}

/*===========================================================================

                   FUNCTIONS FOR MAPPING MBUF APIs

===========================================================================*/

/*===========================================================================

FUNCTION
  OI_MBUF_Alloc

DESCRIPTION
  Allocate an entry in the MBUF table.

===========================================================================*/
OI_MBUF* OI_MBUF_Alloc(OI_UINT8 numCells)
{
  uint16 i;
  OI_MBUF* free_mbuf_ptr = NULL;

  /* allocate free entry from the MBUF table */
  for (i = 0; i < BT_PF_NUM_MBUF; i++)
  {
    if (bt_pf_mbuf[i].free)
    {
      free_mbuf_ptr = &bt_pf_mbuf[i];
      bt_pf_mbuf[i].free = FALSE;
      bt_pf_mbuf[i].IsPristine = TRUE;
      break;
    }
  }
  return free_mbuf_ptr;
}

/*===========================================================================

FUNCTION
  OI_MBUF_Free

DESCRIPTION
  Frees an MBUF in use.

===========================================================================*/
void OI_MBUF_Free(OI_MBUF *mbuf_ptr)
{
  if (mbuf_ptr != NULL)
  {
    if (mbuf_ptr->dsm_state == BT_PF_MBUF_OK_TO_FREE)
    {
      /* free the underlying DSM item */
      dsm_free_packet(&(mbuf_ptr->dsm_ptr));
    }
    else
    {
      /* DSM is being transmitted and will be freed by the stack */
      mbuf_ptr->dsm_state = BT_PF_MBUF_OK_TO_FREE; /* reset state */
    }

    /* free the MBUF table entry */
    mbuf_ptr->dsm_ptr = NULL;
    mbuf_ptr->context_ptr = NULL;
    mbuf_ptr->free = TRUE;
  }
  return;
}

/*===========================================================================

FUNCTION
  OI_MBUF_Prepend

DESCRIPTION
  Adds data to the front of an MBUF.

===========================================================================*/
OI_STATUS OI_MBUF_Prepend(OI_MBUF* mbuf_ptr,
                          const OI_BYTE* buf_ptr,
                          OI_UINT16 len,
                          OI_MBUF_DISPOSITION disposition)
{
  uint16 bytes_pushed;
  boolean dsm_allocated = FALSE;

  if (mbuf_ptr->dsm_ptr == NULL)
  {
    /* if MBUF is empty, grab a DSM item */
    mbuf_ptr->dsm_ptr = bt_get_free_dsm_ptr( BT_TL_OBEX, len );
    if (mbuf_ptr->dsm_ptr == NULL)
    {
      return OI_STATUS_OUT_OF_MEMORY;
    }
    dsm_allocated = TRUE;
  }

  bytes_pushed = dsm_pushdown_packed( &mbuf_ptr->dsm_ptr, (void*)buf_ptr,
                                      len, DSM_DS_POOL_SIZE(len) );
  if ((bytes_pushed != len))
  {
    if (dsm_allocated)
    {
      dsm_free_packet(&(mbuf_ptr->dsm_ptr));
    }
    return OI_STATUS_OUT_OF_MEMORY;
  }

  /* since data is actually copied into the DSM item, */
  /* we can safely free the data */
  if (disposition == MBUF_FREE)
  {
    mem_free( &bt_pf_heap, (void*)buf_ptr );
  }

  return OI_STATUS_SUCCESS;
}

/*===========================================================================

FUNCTION
  OI_MBUF_Append

DESCRIPTION
  Adds data to the end of an MBUF.

===========================================================================*/
OI_STATUS OI_MBUF_Append(OI_MBUF* mbuf_ptr,
                         const OI_BYTE* buf_ptr,
                         OI_UINT16 len,
                         OI_MBUF_DISPOSITION disposition)
{
  uint16 bytes_pushed;
  boolean dsm_allocated = FALSE;


  if (mbuf_ptr->dsm_ptr == NULL)
  {
    /* if MBUF is empty, grab a DSM item */
    mbuf_ptr->dsm_ptr = bt_get_free_dsm_ptr( BT_TL_OBEX, len );
	
    if (mbuf_ptr->dsm_ptr == NULL)
    {
      return OI_STATUS_OUT_OF_MEMORY;
    }
    dsm_allocated = TRUE;
  }

  bytes_pushed = dsm_pushdown_tail( &mbuf_ptr->dsm_ptr, (void*)buf_ptr,len, DSM_DS_POOL_SIZE(len) );

  
  if ((bytes_pushed != len))
  {
    if (dsm_allocated)
    {
      dsm_free_packet(&(mbuf_ptr->dsm_ptr));
    }

    return OI_STATUS_OUT_OF_MEMORY;
  }

  /* since data is actually copied into the DSM item, */
  /* we can safely free the data */
  if (disposition == MBUF_FREE)
  {
    mem_free( &bt_pf_heap, (void*)buf_ptr );
  }

  return OI_STATUS_SUCCESS;
}

/*===========================================================================

FUNCTION
  OI_MBUF_Wrap

DESCRIPTION
  Wraps the data supplied in a MBUF structure.

===========================================================================*/
OI_MBUF* OI_MBUF_Wrap(const OI_BYTE *data_ptr,
                      OI_UINT16 len,
                      OI_MBUF_DISPOSITION disposition)
{
  OI_MBUF* new_mbuf_ptr = NULL;
  OI_STATUS status;

  new_mbuf_ptr = OI_MBUF_Alloc( 0 );
  if (new_mbuf_ptr != NULL)
  {
    status = OI_MBUF_Append(new_mbuf_ptr, data_ptr, len, disposition);

    if (status != OI_STATUS_SUCCESS)
    {
      OI_MBUF_Free( new_mbuf_ptr );
      new_mbuf_ptr = NULL;
    }
  }
  return(new_mbuf_ptr);
}

/*===========================================================================

FUNCTION
  OI_MBUF_SetWindow

DESCRIPTION
  This API is originally intended for setting the data window in an MBUF.
  The data window defines the portion of MBUF data which is to be written
  when OI_RFCOMM_WriteMBUF is called. This function in conjunction with
  OI_MBUF_AdvanceWindow allow the profile code to send an MBUF chunk by
  chunk if the MBUF is too large to fit in a single RFCOMM packet.
  Since SIO will handle the underlying segmentation, this API is no longer
  needed.

===========================================================================*/
OI_UINT16 OI_MBUF_SetWindow(OI_MBUF *mbuf_ptr,
                            OI_UINT16 winSize)
{
  /* do nothing, just returns number of bytes in DSM */
  return(mbuf_ptr->dsm_ptr->used);
}


/*===========================================================================

FUNCTION
  OI_MBUF_AdvanceWindow

DESCRIPTION
  This API is originally intended for advancing the data window in an MBUF.
  The data window defines the portion of MBUF data which is to be written
  when OI_RFCOMM_WriteMBUF is called. This function in conjunction with
  OI_MBUF_SetWindow allow the profile code to send an MBUF chunk by
  chunk if the MBUF is too large to fit in a single RFCOMM packet.
  Since SIO will handle the underlying segmentation, this API is no longer
  needed.

===========================================================================*/
OI_UINT16 OI_MBUF_AdvanceWindow(OI_MBUF *mbuf_ptr,
                                OI_UINT16 winSize)
{
  /* always returns 0 because the entire DSM will have been */
  /* transmitted by SIO when write confirm callback is called */
  /* Returning 0 notifyies the calling function that the entire */
  /* MBUF has been written */
  return(0);
}

OI_UINT16 _OI_MBUF_NumBytes(OI_MBUF *mbuf_ptr)
{
  uint16 num_bytes = 0;
  dsm_item_type* dsm_ptr;

  if (mbuf_ptr == NULL)
  {
    return 0;
  }
  else
  {
    dsm_ptr = mbuf_ptr->dsm_ptr;
    while (dsm_ptr != NULL)
    {
      num_bytes += dsm_ptr->used;
      dsm_ptr = dsm_ptr->pkt_ptr;
    }
  }
  return(num_bytes);
}

/*===========================================================================

FUNCTION
  OI_MBUF_PullMBuf

DESCRIPTION
  Duplicates some amount of data from one MBUF into another MBUF, then
  removes that data from the source MBUF.

===========================================================================*/
OI_UINT16 OI_MBUF_PullMBuf(OI_MBUF *dest,
                           OI_MBUF *src,
                           OI_UINT16 numBytes)
{
  OI_UINT16 bytes_pulled;

  dsm_free_packet(&(dest->dsm_ptr));
  bytes_pulled = dsm_dup_packet( &(dest->dsm_ptr),
                                 src->dsm_ptr,
                                 0, numBytes );
  (void) dsm_pullup( &(src->dsm_ptr),
                     NULL,
                     bytes_pulled );
  src->IsPristine = FALSE;

  return bytes_pulled;
}

/*===========================================================================

FUNCTION
  OI_MBUF_PullBytes

DESCRIPTION
  Copies bytes from an MBUF into a buffer.

===========================================================================*/
OI_UINT16 OI_MBUF_PullBytes(OI_BYTE *dest,
                            OI_MBUF *src,
                            OI_UINT16 numBytes)
{
  OI_UINT16 bytes_pulled;

  bytes_pulled = dsm_pullup( &(src->dsm_ptr), (void*)dest, numBytes );
  src->IsPristine = FALSE;

  return bytes_pulled;
}

/*===========================================================================

              FUNCTIONS FOR MAPPING MEMORY MANAGER APIs

===========================================================================*/

#ifdef FEATURE_BT_AVRCP_13

/*===========================================================================

FUNCTION
  _AV_Free

  DESCRIPTION
  Frees the memory block previously allocated on the heap by _AV_Malloc.

===========================================================================*/
void _AV_Free( void *block_ptr )
{
  if (block_ptr != NULL)
  {
    mem_free( &bt_pf_heap, block_ptr );
    block_ptr = NULL;
  }
  else
  {
    BT_ERR( "BT PF: _OI_FREE: unexpected NULL pointer", 0, 0, 0 );
  }
  return;
}
#endif /* FEATURE_BT_AVRCP_13 */
/*===========================================================================

FUNCTION
  _OI_Free

DESCRIPTION
  Frees the memory block previously allocated on the heap by _OI_Malloc.

===========================================================================*/
void _OI_Free(void *block_ptr)
{
  if (block_ptr != NULL)
  {
    mem_free( &bt_pf_heap, block_ptr );
    block_ptr = NULL;
  }
  else
  {
    BT_ERR( "BT PF: _OI_FREE: unexpected NULL pointer", 0, 0, 0 );
  }
  return;
}

/*===========================================================================

FUNCTION
  _OI_FreeIf

DESCRIPTION
  If the reference to a pointer to a dynamic memory block passed in is not
  NULL, frees the memory and sets the referenced value to NULL.

===========================================================================*/
void _OI_FreeIf(OI_BYTE **blockRef_ptr)
{
  if ((blockRef_ptr != NULL) && (*blockRef_ptr != NULL))
  {
    mem_free( &bt_pf_heap, (void*)*blockRef_ptr );
    *blockRef_ptr = NULL;
  }
  else
  {
    if (blockRef_ptr == NULL)
    {
      BT_ERR( "BT PF: _OI_FreeIf: unexpected NULL pointer", 0, 0, 0 );
    }
  }
  return;
}

/*===========================================================================

FUNCTION
  _OI_Malloc

DESCRIPTION
  Allocates a block of memory form the heap.

===========================================================================*/
void* _OI_Malloc(OI_INT32 size)
{
  void* buf_ptr = NULL;

  buf_ptr = mem_malloc( &bt_pf_heap, (size_t)size );
  return(buf_ptr);
}

#ifdef FEATURE_BT_AVRCP_13
/*===========================================================================

FUNCTION
  _AV_Malloc

DESCRIPTION
  Allocates a block of memory form the heap.

===========================================================================*/
void* _AV_Malloc(size_t size)
{
  void* buf_ptr = NULL;

  buf_ptr = mem_malloc( &bt_pf_heap, (size_t)size );
  return(buf_ptr);
}
#endif /* FEATURE_BT_AVRCP_13 */
/*===========================================================================

FUNCTION
  _OI_Calloc

DESCRIPTION
  Allocates a block of memory form the heap and set the contents to zero.

===========================================================================*/
void* _OI_Calloc(OI_INT32 size)
{
  void* buf_ptr = NULL;

  buf_ptr = mem_calloc( &bt_pf_heap, (size_t)size, sizeof(OI_BYTE) );
  return(buf_ptr);
}

/*===========================================================================

FUNCTION
  _OI_StaticMalloc

DESCRIPTION
  Not doing static memory allocation anymore.

===========================================================================*/
void* _OI_StaticMalloc(OI_INT32 size)
{
  return( _OI_Malloc(size) );
}

/*===========================================================================

FUNCTION
  OI_MemSet

DESCRIPTION
  Memory set.

===========================================================================*/
void OI_MemSet(void *Block, OI_UINT8 Val, OI_UINT32 Size)
{
  if (Block != NULL)
  {
    memset( Block, Val, (size_t)Size );
  }
  else
  {
    BT_ERR( "BT PF: _OI_StaticMalloc: unexpected NULL pointer", 0, 0, 0 );
  }

  return;
}

/*===========================================================================

FUNCTION
  OI_MemCopy

DESCRIPTION
  Memory copy.

===========================================================================*/
void OI_MemCopy(void *To, void const *From, OI_UINT32 Size)
{
  if ((To != NULL) && (From != NULL))
  {
    memcpy( To, From, (size_t)Size );
  }
  else
  {
    BT_ERR( "BT PF: OI_MemCopy: unexpected NULL pointer", 0, 0, 0 );
  }
  return;
}

/*===========================================================================

FUNCTION
  OI_MemZero

DESCRIPTION
  Zero out the memory block.

===========================================================================*/
void OI_MemZero(void *Block, OI_UINT32 Size)
{
  if (Block != NULL)
  {
    memset( Block, 0, (size_t)Size );
  }
  else
  {
    BT_ERR( "BT PF: OI_MemZero: unexpected NULL pointer", 0, 0, 0 );
  }

  return;
}

/*===========================================================================

FUNCTION
  OI_MemCmp

DESCRIPTION
  Compares the value stored in the two memory blocks.

===========================================================================*/
OI_INT OI_MemCmp(void const *s1, void const *s2, OI_UINT32 n)
{
  if ((s1 != NULL) && (s2 != NULL))
  {
    return(memcmp( s1, s2, (size_t)n ));
  }
  else
  {
    BT_ERR( "BT PF: OI_MemCmp: unexpected NULL pointer", 0, 0, 0 );
    return(0);
  }
}


/*===========================================================================

                 FUNCTIONS FOR MAPPING DISPATCHER APIs

===========================================================================*/
/*===========================================================================

FUNCTION
  bt_pf_find_dispatcher_entry

DESCRIPTION
  Initializes dispatcher table.

===========================================================================*/
LOCAL void bt_pf_init_dispatcher_table()
{
  int16 i;

  for (i = 0; i < BT_PF_MAX_DISPATCH; i++)
  {
    bt_pf_dispatcher[i].arg_ptr = NULL;
    bt_pf_dispatcher[i].arg_size = 0;
    bt_pf_dispatcher[i].cb_ptr = NULL;
    bt_pf_dispatcher[i].tmo = 0;
    bt_pf_dispatcher[i].salt = 0;
  }
}
/*===========================================================================

FUNCTION
  bt_pf_find_dispatcher_entry

DESCRIPTION
  Locates a free entry in the dispatcher table.

===========================================================================*/
LOCAL int16 bt_pf_find_dispatcher_entry()
{
  int16 i;

  for (i = 0; i < BT_PF_MAX_DISPATCH; i++)
  {
    if (bt_pf_dispatcher[i].cb_ptr == NULL)
    {
      return i;
    }
  }
  return(BT_PF_DISPATCH_TABLE_FULL);
}

/*===========================================================================

FUNCTION
  OI_Dispatch_RegisterTimedFunc

DESCRIPTION
  Registers a timed function in the dispatcher to be invoked after the
  specified timeout expired. The registered function will be
  invoked ON OR AFTER the moment when the timeout expires.
  timeout interval is specified in tenths of a second.

===========================================================================*/
OI_STATUS OI_Dispatch_RegisterTimedFunc(DISPATCH_CB_FUNC    cb_fptr,
                                        DISPATCH_ARG*       arg_ptr,
                                        OI_INTERVAL         timeout,
                                        DISPATCH_CB_HANDLE* handle_ptr)
{
  void* arg_copy_ptr;
  int16 index;
  bt_pf_dispatch_item_type* item_ptr;
  OI_STATUS ret = OI_STATUS_SUCCESS;

  INTLOCK();
  /* locate free entry in dispatcher table */
  index = bt_pf_find_dispatcher_entry();
  if (index == BT_PF_DISPATCH_TABLE_FULL)
  {
    ret = OI_DISPATCH_TABLE_OVERFLOW;
  }
  else
  {
    item_ptr = &bt_pf_dispatcher[index];

    if (arg_ptr == NULL) /* callback has no arguments */
    {
      item_ptr->arg_ptr = NULL;
      item_ptr->arg_size = 0;
    }
    else if (arg_ptr->size <= BT_PF_MAX_INLINE_ARG_SIZE)
    /* callback arguments can fit in the pre-defined array */
    {
      OI_MemCopy( item_ptr->inline_arg, arg_ptr->data, arg_ptr->size );
      item_ptr->arg_ptr = item_ptr->inline_arg;
      item_ptr->arg_size = arg_ptr->size;
    }
    else /* cb arguments too big, need to allocate memory to store them */
    {
      arg_copy_ptr = OI_Malloc(arg_ptr->size);
      if (arg_copy_ptr == NULL)
      {
        ret = OI_STATUS_OUT_OF_MEMORY;
      }
      else
      {
        OI_MemCopy( arg_copy_ptr, arg_ptr->data, arg_ptr->size );
        item_ptr->arg_ptr = arg_copy_ptr;
        item_ptr->arg_size = arg_ptr->size;
      }
    }

    if (ret == OI_STATUS_SUCCESS)
    {
      item_ptr->cb_ptr = cb_fptr;
      if (handle_ptr != NULL)
      {
        /* generate unique callback handle */
        *handle_ptr = CB_HANDLE( index, ++item_ptr->salt );
      }
      item_ptr->tmo = (rex_timer_cnt_type)(timeout /
                                           BT_PF_TIMER_INTVL_100MS);
    }
  }
  INTFREE();
  return(ret);
}

/*===========================================================================

FUNCTION
  OI_Dispatch_RegisterFunc

DESCRIPTION
  Registers a function in the dispatcher to be invoked at the next
  master timer tick. The master timer tick occurs in 500ms intervals.

===========================================================================*/
OI_STATUS OI_Dispatch_RegisterFunc(DISPATCH_CB_FUNC    cb_fptr,
                                   DISPATCH_ARG*       arg_ptr,
                                   DISPATCH_CB_HANDLE* handle_ptr)
{
  /* equivalent to registering a timed function with zero timeout */
  return(OI_Dispatch_RegisterTimedFunc(cb_fptr, arg_ptr, 0, handle_ptr) );
}

/*===========================================================================

FUNCTION
  OI_Dispatch_IsValidHandle

DESCRIPTION
  Checks if the supplied callback handle is valid.

===========================================================================*/
OI_BOOL OI_Dispatch_IsValidHandle(DISPATCH_CB_HANDLE handle)
{
  uint16 index = CB_INDEX(handle);

  if ( (index >= BT_PF_MAX_DISPATCH) ||
       (bt_pf_dispatcher[index].cb_ptr == NULL) )
  {
    return FALSE;
  }
  /* Check the salt in the CBHandle is what we expect */
  return(CB_SALT( bt_pf_dispatcher[index].salt ) == CB_SALT( handle ));
}

/*===========================================================================

FUNCTION
  OI_Dispatch_SetFuncTimeout

DESCRIPTION
  Set or change the timeout of a previously registered function.
  If a callback function was previously registered without a timeout, calling
  this function will make it a timed callback function.

===========================================================================*/
OI_STATUS OI_Dispatch_SetFuncTimeout(DISPATCH_CB_HANDLE handle,
                                     OI_INTERVAL        timeout)
{
  uint16 index = CB_INDEX(handle);

  if (!OI_Dispatch_IsValidHandle( handle ))
  {
    return OI_DISPATCH_INVALID_CB_HANDLE;
  }
  /* update the timeout value of the dispatcher table entry */
  INTLOCK();
  bt_pf_dispatcher[index].tmo = (rex_timer_cnt_type)(timeout /
                                                 BT_PF_TIMER_INTVL_100MS);
  INTFREE();

  return OI_STATUS_SUCCESS;
}

/*===========================================================================

FUNCTION
  OI_Dispatch_CallFunc

DESCRIPTION
  Removes a registered callback function from the dispatch queue and calls
  it immediately.

===========================================================================*/
OI_STATUS OI_Dispatch_CallFunc(DISPATCH_CB_HANDLE handle)
{
  uint16 index = CB_INDEX(handle);
  DISPATCH_ARG arg = { BT_PF_MAX_INLINE_ARG_SIZE+1, NULL };

  if (!OI_Dispatch_IsValidHandle( handle ))
  {
    return OI_DISPATCH_INVALID_CB_HANDLE;
  }

  /* setting timeout to max value prevents the callback function from being
     invoked again when timer expires while callback is being executed */
  bt_pf_dispatcher[index].tmo = OI_MAX_INTERVAL;
  if (bt_pf_dispatcher[index].arg_ptr == NULL)
  {
    (bt_pf_dispatcher[index].cb_ptr)( NULL );
  }
  else
  {
    /* retrieve callback function arguments */
    arg.size = bt_pf_dispatcher[index].arg_size;
    arg.data = (void*)(bt_pf_dispatcher[index].arg_ptr);
    (bt_pf_dispatcher[index].cb_ptr)( &arg );
  }

  INTLOCK();
  /* release entry in dispatcher table */
  if ( (arg.size > BT_PF_MAX_INLINE_ARG_SIZE) && (arg.data != NULL) )
  {
    /* discard callback function arguments*/
    OI_Free( bt_pf_dispatcher[index].arg_ptr );
    bt_pf_dispatcher[index].arg_ptr = NULL;
  }
  bt_pf_dispatcher[index].cb_ptr = NULL;
  bt_pf_dispatcher[index].tmo = 0;
  INTFREE();

  return OI_OK;
}

/*===========================================================================

FUNCTION
  OI_Dispatch_CancelFunc

DESCRIPTION
  Cancels a previously registered callback function.

===========================================================================*/
OI_STATUS OI_Dispatch_CancelFunc(DISPATCH_CB_HANDLE handle)
{
  uint16 index = CB_INDEX(handle);

  if (!OI_Dispatch_IsValidHandle( handle ))
  {
    return OI_DISPATCH_INVALID_CB_HANDLE;
  }

  INTLOCK();
  if ( (bt_pf_dispatcher[index].arg_size > BT_PF_MAX_INLINE_ARG_SIZE) &&
       (bt_pf_dispatcher[index].arg_ptr != NULL) )
  {
    OI_Free( bt_pf_dispatcher[index].arg_ptr );
    bt_pf_dispatcher[index].arg_ptr = NULL;
  }
  bt_pf_dispatcher[index].cb_ptr = NULL;
  bt_pf_dispatcher[index].tmo = 0;
  INTFREE();

  return OI_STATUS_SUCCESS;
}


/*===========================================================================

                    FUNCTIONS FOR MAPPING RFCOMM APIs

===========================================================================*/

/*===========================================================================

FUNCTION
  OI_RFCOMM_Connect

DESCRIPTION
  Calls sio_open() to initiate a connection to a server.

NOTE
  The bufSize input is intended for specifying size of buffer to use for
  flow control on this link. This buffering will be used for storing
  received data when incoming flow is disabled. Since OBEX is a
  command/response protocol and we assemble complete packets before calling
  up the the application we do not need RFCOMM to do any buffering for flow
  control.

===========================================================================*/
OI_STATUS OI_RFCOMM_Connect(OI_RFCOMM_CONNECT_CFM callback,
                            const OI_RFCOMM_LINK_INDICATIONS *indications,
                            OI_BD_ADDR *addr,
                            OI_UINT8 serverChannel,
                            OI_UINT16 maxFrameSize,
                            OI_UINT16 bufSize,
                            OI_RFCOMM_LINK_HANDLE *handle,
                            const OI_CONNECT_POLICY *policy)
{
  sio_stream_id_type  stream_id;
  sio_open_type       so;
  bt_spp_open_type    bso;
  int16               index;

#if defined(FEATURE_MT_OBEX) && defined(FEATURE_IRDA)
#error code not present
#endif /* defined(FEATURE_MT_OBEX) && defined(FEATURE_IRDA) */

  BT_MSG_DEBUG( "OI RC Connect - scn=%d uuid=0x%x mfs=%d", 
                serverChannel, policy->serviceUuid32, maxFrameSize );

  /* assign open entry in SIO connection table */
  index = bt_pf_find_table_entry_sid(SIO_NO_STREAM_ID);
  if (index == BT_PF_SIO_ENTRY_NOT_FOUND)
  {
    BT_ERR( "OI RC Connect - out of entry in PF SIO table", 0, 0, 0 );
    return(OI_STATUS_NO_RESOURCES);
  }

  so.rx_flow        = SIO_FCTL_BEST;
  so.tx_flow        = SIO_FCTL_BEST;
  so.stream_mode    = SIO_GENERIC_MODE;
  so.tx_queue       = &(bt_pf_sio_table[index].tx_wm);
  so.rx_queue       = &(bt_pf_sio_table[index].rx_wm);
  so.tail_char_used = FALSE;
  so.rx_func_ptr    = NULL;
  so.bt_open_ptr    = &bso;

#if defined(FEATURE_MT_OBEX) && defined(FEATURE_IRDA)
#error code not present
#endif /* defined(FEATURE_MT_OBEX) && defined(FEATURE_IRDA) */

  so.port_id        = SIO_PORT_BT_SPP;
  bso.status_change_fptr    = bt_pf_sio_table[index].spp_stat_fptr;
  bso.config_change_fptr    = bt_pf_sio_table[index].spp_cfg_fptr;
  bso.modem_status_fptr     = bt_pf_sio_table[index].spp_ms_fptr;
  bso.line_error_fptr       = bt_pf_sio_table[index].spp_le_fptr;
  if ( maxFrameSize >= BT_MTP_RC_INFO_LEN )
  {
    /* BT_MTP_RC_INFO_LEN is the optimal frame size which is drived from the
       size of a DH5 packet plus RC & L2 headers and FEC. This is the max
       frame size that the underlying BT SIO (RFCOMM) will transmit in one
       shot, passing bigger values down will have no effect. */
    bso.max_frame_size      = BT_MTP_RC_INFO_LEN;
  }
  else
  {
    bso.max_frame_size      = maxFrameSize;
  }
  bso.service_version       = 0x0100;
  bso.service_name_str_ptr  = NULL;
  bso.client_app            = TRUE;
  bso.bd_addr_ptr           = (bt_bd_addr_type*)addr;
  if (serverChannel != BT_SPP_SCN_UNSPECIFIED)
  {
    bso.service_uuid          = BT_SD_INVALID_UUID;
  }
  else
  {
    bso.service_uuid          = (bt_sd_uuid_type)policy->serviceUuid32;
  }
  bso.rc_server_channel     = (bt_spp_scn_enum_type)serverChannel;
#if defined(FEATURE_MT_OBEX) && defined(FEATURE_IRDA)
#error code not present
#endif /* defined(FEATURE_MT_OBEX) && defined(FEATURE_IRDA) */

  bt_pf_sio_table[index].uuid         = (bt_sd_uuid_type)policy->serviceUuid32;
  bt_pf_sio_table[index].con_cfm.fptr = callback;
  bt_pf_sio_table[index].dcn_ind.fptr = indications->disconnectInd;
  bt_pf_sio_table[index].msc_ind.fptr = indications->lineControlInd;
  bt_pf_sio_table[index].rls_ind.fptr = indications->lineStatusInd;
  bt_pf_sio_table[index].rpn_ind.fptr = indications->portParamInd;
  bt_pf_sio_table[index].rcv_ind.fptr = indications->recvDataInd;
                                           /* not used */
  bt_pf_sio_table[index].state = BT_PF_SIO_CLI_NO_CONNECT;
  memcpy( (byte*)&bt_pf_sio_table[index].con_ind.remote_bd_addr,
          (byte*)addr,
          sizeof( bt_bd_addr_type ) );

  /* There is a possibility that the status change callback is called */
  /* before sio_open returns. For the case where OPEN_ERROR results,  */
  /* the table entry will be freed by the callback. */
  stream_id = sio_open( &so );

  if (stream_id == SIO_NO_STREAM_ID)
  {
    BT_ERR( "OI RC Connect - sio_open() failed", 0, 0, 0 );
    bt_pf_free_table_entry(index);
    return(OI_STATUS_NO_RESOURCES);
  }
  bt_pf_sio_table[index].stream_id = stream_id;
  *handle = (OI_RFCOMM_LINK_HANDLE)stream_id;

  return OI_STATUS_SUCCESS;
}

/*===========================================================================

FUNCTION
  OI_RFCOMM_RegisterServer

DESCRIPTION
  Calls sio_open() to register an RFCOMM server.

NOTE
  The bufSize input is intended for specifying size of buffer to use for
  flow control on this link. This buffering will be used for storing
  received data when incoming flow is disabled. Since OBEX is a
  command/response protocol and we assemble complete packets before calling
  up the the application we do not need RFCOMM to do any buffering for flow
  control.

===========================================================================*/
OI_STATUS OI_RFCOMM_RegisterServer(OI_RFCOMM_CONNECT_IND callback,
                               const OI_RFCOMM_LINK_INDICATIONS *indications,
                               OI_UINT16 maxFrameSize,
                               OI_UINT16 bufSize, /* not used */
                               OI_UINT8 *serverChannel,
                               const OI_CONNECT_POLICY *policy)
{
  sio_stream_id_type  stream_id;
  sio_open_type       so;
  bt_spp_open_type    bso;
  int16               index;
#if defined(FEATURE_MT_OBEX) && defined(FEATURE_IRDA)
#error code not present
#endif

  BT_MSG_DEBUG( "OI RC RegSrv - uuid=0x%x mfs=%d", 
                policy->serviceUuid32, maxFrameSize, 0 );

  if ( ((bufSize < maxFrameSize) && (bufSize != 0)) || (indications == NULL) )
  {
    return OI_STATUS_INVALID_PARAMETERS;
  }

  /* assign open entry in SIO connection table */
  index = bt_pf_find_table_entry_sid(SIO_NO_STREAM_ID);
  if (index == BT_PF_SIO_ENTRY_NOT_FOUND)
  {
    BT_ERR( "OI RC Reg Srvr - out of entry in PF SIO table", 0, 0, 0 );
    return(OI_RFCOMM_OUT_OF_SERVER_CHANNELS);
  }

  /* save pointer to TCB of calling task */
  bt_pf_app_tcb_ptr = rex_self();

  /* set port to NULL for now and then set to relevant port below */
  so.port_id        = SIO_PORT_NULL;
  so.rx_flow        = SIO_FCTL_BEST;
  so.tx_flow        = SIO_FCTL_BEST;
  so.stream_mode    = SIO_GENERIC_MODE;
  so.tx_queue       = &(bt_pf_sio_table[index].tx_wm);
  so.rx_queue       = &(bt_pf_sio_table[index].rx_wm);
  so.tail_char_used = FALSE;
  so.rx_func_ptr    = NULL;

#ifdef FEATURE_MT_OBEX
#ifdef FEATURE_IRDA
#error code not present
#endif /* FEATURE_IRDA */
#ifdef FEATURE_USB_CDC_ACM
  if( (*serverChannel) == MT_OBEX_USB_TP_ID)
  {
    so.port_id        = SIO_PORT_USB_CDC_ACM;
  }
#endif /* FEATURE_USB_CDC_ACM */
  if( (*serverChannel) == MT_OBEX_RS232_TP_ID)
  {
    so.port_id        = SIO_PORT_UART_MAIN;
  }

  /* we are attempting to setup an OBEX server over a NON-BT transport */
  if(so.port_id != SIO_PORT_NULL)
  {
    /* skip BT SIO setup */
    if (policy == NULL)
    {
      bso.service_uuid        = BT_SD_INVALID_UUID;
    }
    else
    {
      bso.service_uuid        = (bt_sd_uuid_type)policy->serviceUuid32;
    }
  }
  else /* BT OBEX server */
  {
#endif /* FEATURE_MT_OBEX */

    so.port_id        = SIO_PORT_BT_SPP;
    so.bt_open_ptr    = &bso;

    bso.status_change_fptr    = bt_pf_sio_table[index].spp_stat_fptr;
    bso.config_change_fptr    = bt_pf_sio_table[index].spp_cfg_fptr;
    bso.modem_status_fptr     = bt_pf_sio_table[index].spp_ms_fptr;
    bso.line_error_fptr       = bt_pf_sio_table[index].spp_le_fptr;
    if ( maxFrameSize >= BT_MTP_RC_INFO_LEN )
    {
      /* BT_MTP_RC_INFO_LEN is the optimal frame size which is drived from the
         size of a DH5 packet plus RC & L2 headers and FEC. This is the max
         frame size that the underlying BT SIO (RFCOMM) will transmit in one
         shot, passing bigger values down will have no effect. */
      bso.max_frame_size      = BT_MTP_RC_INFO_LEN;
    }
    else
    {
      bso.max_frame_size      = maxFrameSize;
    }
    bso.service_version       = 0x0100;
    bso.service_name_str_ptr  = NULL;
    bso.client_app            = FALSE;
    bso.bd_addr_ptr           = NULL;
    if (policy == NULL)
    {
      bso.service_uuid        = BT_SD_INVALID_UUID;
    }
    else
    {
      bso.service_uuid        = (bt_sd_uuid_type)policy->serviceUuid32;
    }
    bso.rc_server_channel     = BT_SPP_SCN_UNSPECIFIED;
#ifdef FEATURE_MT_OBEX
  }
#endif /* FEATURE_MT_OBEX */
  bt_pf_sio_table[index].uuid         = bso.service_uuid;
  bt_pf_sio_table[index].con_ind.fptr = callback;
  bt_pf_sio_table[index].dcn_ind.fptr = indications->disconnectInd;
  bt_pf_sio_table[index].msc_ind.fptr = indications->lineControlInd;
  bt_pf_sio_table[index].rls_ind.fptr = indications->lineStatusInd;
  bt_pf_sio_table[index].rpn_ind.fptr = indications->portParamInd;
  bt_pf_sio_table[index].rcv_ind.fptr = indications->recvDataInd;/* not used */
  bt_pf_sio_table[index].state = BT_PF_SIO_SRV_NO_CONNECT;

  rex_clr_sigs( rex_self(), BT_PF_WAIT_SIG );

#if defined(FEATURE_MT_OBEX) && defined(FEATURE_PCAA_SIRIUS)
#error code not present
#else /* defined(FEATURE_MT_OBEX) && defined(FEATURE_PCAA_SIRIUS) */

  stream_id = sio_open( &so );

#endif /* defined(FEATURE_MT_OBEX) && defined(FEATURE_PCAA_SIRIUS) */

  if (stream_id == SIO_NO_STREAM_ID)
  {
    bt_pf_free_table_entry(index);
    BT_ERR( "RC Reg Srvr - sio_open() failed", 0, 0, 0 );
    return(OI_RFCOMM_OUT_OF_SERVER_CHANNELS);
  }

  if (bt_pf_sio_table[index].state == BT_PF_SIO_SRV_REGISTERED)
  { 
    // Okay to let upper layer know that registration has completed since
    // we have scn information by now (from sio_open()).
    *serverChannel = bt_pf_sio_table[index].scn;
    return(OI_OK);
  }
  else
  {
    BT_ERR( "RC Reg Srvr - idx=%d state=%d", 
            index, bt_pf_sio_table[index].state, 0 );
    return(OI_RFCOMM_OUT_OF_SERVER_CHANNELS);
  }
}

#if defined(FEATURE_MT_OBEX) && defined(FEATURE_PCAA_SIRIUS)
#error code not present
#endif /* defined(FEATURE_MT_OBEX) && defined(FEATURE_PCAA_SIRIUS) */


/*===========================================================================

FUNCTION
  OI_RFCOMM_DeregisterServer

DESCRIPTION
  Calls sio_close() to deregister an RFCOMM server.
  Connections to client is assumed to have been disconnected.

===========================================================================*/
OI_STATUS OI_RFCOMM_DeregisterServer(OI_UINT8 serverChannel)
{
  int16 index;

  BT_MSG_DEBUG( "OI RC DeregSrv - scn=%d", serverChannel, 0, 0 );

  index = bt_pf_find_table_entry_scn( serverChannel );
  if (index == BT_PF_SIO_ENTRY_NOT_FOUND)
  {
    return(OI_STATUS_INVALID_PARAMETERS);
  }
  /* This function assumes all connections has already been terminated */
  if (bt_pf_sio_table[index].state == BT_PF_SIO_SRV_REGISTERED)
  {
    dsm_empty_queue( &(bt_pf_sio_table[index].tx_wm) );
    dsm_empty_queue( &(bt_pf_sio_table[index].rx_wm) );
    sio_close( bt_pf_sio_table[index].stream_id, NULL );
    bt_pf_free_table_entry(index);
    return(OI_STATUS_SUCCESS);
  }

  BT_ERR( "BT PF: SID %x DeregisterServer in unexpected state %x",
    bt_pf_sio_table[index].stream_id, bt_pf_sio_table[index].state, 0 );
  return(OI_STATUS_INTERNAL_ERROR);
}

/*===========================================================================

FUNCTION
  OI_RFCOMM_Accept

DESCRIPTION
  Provides the application an option to accept or refuse an
  incoming connection to an RFCOMM server. Since SIO/RFCOMM always
  accepts incoming connections, this function will disconnect if the
  application refuses to accept the connection.

===========================================================================*/
OI_STATUS OI_RFCOMM_Accept( OI_RFCOMM_CONNECT_CFM callback,
                            OI_RFCOMM_LINK_HANDLE link,
                            OI_BOOL accept)
{
  int16 index;

  BT_MSG_DEBUG( "OI RC Accept - sid=%d accept=%d", link, accept, 0 );

  /* locate entry in SIO connection table */
  index = bt_pf_find_table_entry_sid( link );
  if (index == BT_PF_SIO_ENTRY_NOT_FOUND)
  {
    BT_ERR( "OI RC Accept - out of entry in PF SIO table", 0, 0, 0 );
    return(OI_RFCOMM_LINK_NOT_FOUND);
  }
  if (bt_pf_sio_table[index].state == BT_PF_SIO_SRV_PENDING_ACCEPT)
  {
    BT_MSG_DEBUG( "OI RC Accept - idx=%d cb=0x%x rxb4a=%d", 
                  index, callback, bt_pf_rx_before_accept );

    if (accept)
    {
      /* RFCOMM has already accepted the incoming connection */
      /* so call callback right away */
      bt_pf_sio_table[index].state = BT_PF_SIO_SRV_CONNECTED;

      /* This is equivalent to calling OI_RFCOMM_SetLinkContext.  */
      /* It is done here because the callback function needs to   */
      /* know the link. OI_RFCOMM_SetLinkContext is called after  */
      /* this function returns (in RfcommConnectInd in obexsrv.c) */
      /* essentially setting the link again, which is OK.         */
      bt_pf_sio_table[index].link_context =
        bt_pf_sio_table[index].server_context;

      if (callback != NULL)
      {
        callback( bt_pf_sio_table[index].stream_id,
                  bt_pf_sio_table[index].max_frame_size,
                  OI_OK );
      }
      if (bt_pf_rx_before_accept)
      {
        bt_pf_ch_masks.rcv_ind |= bt_pf_ch_bit[index];
        rex_set_sigs( &bt_pf_tcb, BT_PF_RCV_IND_SIG );
        bt_pf_rx_before_accept = FALSE;
      }
      /* RPN config info has previously been received, signal RPN */
      if (bt_pf_sio_table[index].rpn_ind.fptr != NULL)
      {
        bt_pf_ch_masks.rpn_ind |= bt_pf_ch_bit[index];
        rex_set_sigs( &bt_pf_tcb, BT_PF_RPN_IND_SIG );
      }
    }
    else
    {
      /* if application is not accepting, we need to disconnect */
      sio_ioctl( bt_pf_sio_table[index].stream_id,
                 SIO_IOCTL_BT_DISCONNECT, NULL);
    }
    return(OI_STATUS_SUCCESS);
  }

  BT_ERR( "BT PF: SID %x Accept in unexpected state %x",
    bt_pf_sio_table[index].stream_id, bt_pf_sio_table[index].state, 0 );

  return(OI_STATUS_INTERNAL_ERROR);
}

/*===========================================================================

FUNCTION
  OI_RFCOMM_Disconnect

DESCRIPTION
 Terminates the specified RFCOMM link. The disconnect indication callback
 function passed to OI_RFCOMM_Accept() or OI_RFCOMM_Connect() will be called
 when the disconnection is complete.

===========================================================================*/
OI_STATUS OI_RFCOMM_Disconnect(OI_RFCOMM_LINK_HANDLE link)
{
  int16 index;

  BT_MSG_DEBUG( "OI RC Disconnect - sid=%d", link, 0, 0 );

  /* locate entry in SIO connection table */
  index = bt_pf_find_table_entry_sid( link );
  if (index == BT_PF_SIO_ENTRY_NOT_FOUND)
  {
    BT_ERR( "OI RC Disc - entry not found in PF SIO table", 0, 0, 0 );
    return(OI_RFCOMM_LINK_NOT_FOUND);
  }

#if defined(FEATURE_MT_OBEX) && defined(FEATURE_PCAA_SIRIUS)
#error code not present
#endif

  if ( (bt_pf_sio_table[index].state == BT_PF_SIO_SRV_CONNECTED) ||
       (bt_pf_sio_table[index].state == BT_PF_SIO_SRV_SENDING)   ||
       ((bt_pf_sio_table[index].state >= BT_PF_SIO_CLI_NO_CONNECT) && 
        (bt_pf_sio_table[index].state <= BT_PF_SIO_CLI_SENDING)))
  {
#if defined(FEATURE_MT_OBEX) && defined(FEATURE_IRDA)
#error code not present
#else
    sio_ioctl( bt_pf_sio_table[index].stream_id, 
               SIO_IOCTL_BT_DISCONNECT, NULL );
#endif /* defined(FEATURE_MT_OBEX) && defined(FEATURE_IRDA) */

    return(OI_STATUS_SUCCESS);
  }

  BT_ERR( "BT PF: SID %x Disconnect in unexpected state %x",
    bt_pf_sio_table[index].stream_id, bt_pf_sio_table[index].state, 0 );

  return(OI_STATUS_INTERNAL_ERROR);    
}

/*===========================================================================

FUNCTION
  OI_RFCOMM_WriteMBUF

DESCRIPTION
  Calls sio_transmit() to transmit the data encapsulate by the MBUF passed
  in. The entire contents of the MBUF will be sent.

===========================================================================*/
OI_STATUS OI_RFCOMM_WriteMBUF(OI_RFCOMM_WRITE_MBUF_CFM callback,
                              OI_RFCOMM_LINK_HANDLE link,
                              OI_MBUF *mbuf)
{
  int16 index;
  OI_STATUS ret;
  uint32 dsm_pkt_len;

  if ( (mbuf == NULL) || (callback == NULL) || (OI_MBUF_NumBytes(mbuf) == 0) )
  {
    return(OI_STATUS_INVALID_PARAMETERS);
  }

  /* locate entry in SIO connection table */
  index = bt_pf_find_table_entry_sid( link );
  if (index == BT_PF_SIO_ENTRY_NOT_FOUND)
  {
    BT_ERR( "OI RC WriteMBUF - entry not found in PF SIO table", 0, 0, 0 );
    return(OI_RFCOMM_LINK_NOT_FOUND);
  }

  dsm_pkt_len = dsm_length_packet(mbuf->dsm_ptr);
  switch (bt_pf_sio_table[index].state)
  {
    case BT_PF_SIO_SRV_CONNECTED:
      if ( dsm_length_packet(mbuf->dsm_ptr) <= 
          (bt_pf_sio_table[index].tx_wm.dont_exceed_cnt - 
           bt_pf_sio_table[index].tx_wm.current_cnt) ) 
      {
        bt_pf_sio_table[index].wrt_cfm.fptr = callback;
        bt_pf_sio_table[index].state = BT_PF_SIO_SRV_SENDING;
        /* pointer to the MBUF being sent needs to be saved because
           the callback needs it */
        bt_pf_sio_table[index].mbuf_ptr = mbuf;
        mbuf->dsm_state = BT_PF_MBUF_DSM_SENT;
        /* transmit the underlying DSM of the MBUF */
        sio_transmit( bt_pf_sio_table[index].stream_id, mbuf->dsm_ptr );
      }
      else
      {
        BT_ERR( "OI RC WriteMBUF - data too large! CC %d IL %d DNE %d", 
                bt_pf_sio_table[index].tx_wm.current_cnt,
                dsm_pkt_len,
                bt_pf_sio_table[index].tx_wm.dont_exceed_cnt );
        ret = OI_STATUS_INTERNAL_ERROR;
        break;
      }
#if defined(FEATURE_MT_OBEX) && defined(FEATURE_PCAA_SIRIUS)
#error code not present
#endif
      ret = OI_STATUS_SUCCESS;
      break;
    case BT_PF_SIO_CLI_CONNECTED:
      if ( dsm_length_packet(mbuf->dsm_ptr) <= 
          (bt_pf_sio_table[index].tx_wm.dont_exceed_cnt - 
           bt_pf_sio_table[index].tx_wm.current_cnt) )  
      {
        bt_pf_sio_table[index].wrt_cfm.fptr = callback;
        bt_pf_sio_table[index].state = BT_PF_SIO_CLI_SENDING;
        /* pointer to the MBUF being sent needs to be saved because
           the callback needs it */
        bt_pf_sio_table[index].mbuf_ptr = mbuf;
        mbuf->dsm_state = BT_PF_MBUF_DSM_SENT;

        /* transmit the underlying DSM of the MBUF */
        sio_transmit( bt_pf_sio_table[index].stream_id, mbuf->dsm_ptr );
        ret = OI_STATUS_SUCCESS;
      }
      else
      {
        BT_ERR( "OI RC WriteMBUF - data too large! CC %d IL %d DNE %d", 
                bt_pf_sio_table[index].tx_wm.current_cnt,
                dsm_pkt_len,
                bt_pf_sio_table[index].tx_wm.dont_exceed_cnt );
        ret = OI_STATUS_INTERNAL_ERROR;
      }
      break;
    case BT_PF_SIO_CLI_SENDING:
    case BT_PF_SIO_SRV_SENDING:
      ret = OI_RFCOMM_WRITE_IN_PROGRESS;
      break;
    default:
      BT_ERR( "BT PF: SID %x WriteMBUF in unexpected state %x",
              bt_pf_sio_table[index].stream_id,
              bt_pf_sio_table[index].state, 0  );
      ret = OI_STATUS_INTERNAL_ERROR;
  }
  return(ret);
}


/*===========================================================================

FUNCTION
  RFCOMM_WriteMBUFCfm

DESCRIPTION
  This is the callback function passed into OI_RFCOMM_WriteMBUF in the
  implementation of OI_RFCOMM_Write.

===========================================================================*/
LOCAL void RFCOMM_WriteMBUFCfm(OI_RFCOMM_LINK_HANDLE linkHandle,
                                OI_MBUF *mbuf,
                                OI_STATUS result)
{
  BT_PF_WRITE_MBUF_CONTEXT *context;

  if (mbuf == NULL)
  {
    BT_ERR( "BT EXTP: SID %x WriteMBUFCfm with NULL MBUF",
            linkHandle, 0, 0 );
    return;
  }
  if (mbuf->context_ptr == NULL)
  {
    BT_ERR( "BT EXTP: SID %x WriteMBUFCfm with NULL MBUF context",
            linkHandle, 0, 0 );
    return;
  }
  /* retrieve the OI_RFCOMM_Write callback function pointer from
     the MBUF context */
  context = mbuf->context_ptr;
  OI_MBUF_Free(mbuf);

  context->writeCfm(linkHandle, context->buf, context->bufLen, result);
  OI_Free(context);
}

/*===========================================================================

FUNCTION
  OI_RFCOMM_Write

DESCRIPTION
  Wraps the specified data in an MBUF structure and calls
  OI_RFCOMM_WriteMBUF for transmission.

===========================================================================*/
OI_STATUS OI_RFCOMM_Write(OI_RFCOMM_WRITE_CFM callback,
                          OI_RFCOMM_LINK_HANDLE linkHandle,
                          OI_UINT8 *data,
                          OI_UINT16 dataLen)
{
  OI_MBUF *mbuf;
  BT_PF_WRITE_MBUF_CONTEXT *context;
  OI_STATUS result;

  if ( (data == NULL) || (dataLen == 0) || (callback == NULL) )
  {
    return OI_STATUS_INVALID_PARAMETERS;
  }

  /* allocate the context of the MBUF */
  context = OI_Malloc( sizeof( BT_PF_WRITE_MBUF_CONTEXT ) );
  if (context == NULL)
  {
    return OI_STATUS_OUT_OF_MEMORY;
  }

  /* wraps the data in a MBUF and use OI_RFCOMM_WriteMBUF to send data */
  mbuf = OI_MBUF_Wrap(data, dataLen, MBUF_KEEP);
  if (mbuf == NULL)
  {
    OI_Free(context);
    return OI_STATUS_OUT_OF_MEMORY;
  }

  /* saves the callback function pointer in the MBUF's context */
  context->writeCfm = callback;
  context->buf = data;
  context->bufLen = dataLen;
  context->linkHandle = linkHandle;
  mbuf->context_ptr = context;

  result = OI_RFCOMM_WriteMBUF(RFCOMM_WriteMBUFCfm, linkHandle, mbuf);
  if (!OI_SUCCESS(result))
  {
    OI_Free(context);
    OI_MBUF_Free(mbuf);
  }
  return result;
}

/*===========================================================================

FUNCTION
  OI_RFCOMM_WriteSegmented

DESCRIPTION
  In OI implementation, this function supposedly handles segmenting data
  before sending it out. In QC implementation, segmentation is handled in the
  RFCOMM layer when necessary.

===========================================================================*/
OI_STATUS OI_RFCOMM_WriteSegmented(OI_RFCOMM_WRITE_CFM callback,
                                   OI_RFCOMM_LINK_HANDLE link,
                                   OI_UINT8 *data,
                                   OI_UINT16 dataLen)
{
  return ( OI_RFCOMM_Write(callback, link, data, dataLen) );
}

/*===========================================================================

FUNCTION
  OI_RFCOMM_SetServerContext

DESCRIPTION
  Associates a caller defined context with a registered RFCOMM server channel.
  This context can then be retrieved by calling OI_RFCOMM_GetServerContext.

===========================================================================*/
OI_STATUS OI_RFCOMM_SetServerContext(OI_UINT8 scn, void *context)
{
  uint16 i;

  for (i = 0; i < BT_PF_MAX_DISPATCH; i++)
  {
    if (bt_pf_sio_table[i].scn == scn)
    {
      bt_pf_sio_table[i].server_context = context;
      return OI_OK;
    }
  }
  return OI_STATUS_NOT_FOUND;
}

/*===========================================================================

FUNCTION
  OI_RFCOMM_GetServerContext

DESCRIPTION
  Gets a caller defined context from a registered RFCOMM server channel. This
  is a value that we previously set by a call to OI_RFCOMM_SetServerContext.

===========================================================================*/
void* OI_RFCOMM_GetServerContext(OI_UINT8 scn)
{
  uint16 i;

  for (i = 0; i < BT_PF_MAX_DISPATCH; i++)
  {
    if (bt_pf_sio_table[i].scn == scn)
    {
      return(bt_pf_sio_table[i].server_context);
    }
  }
  return NULL;
}

/*===========================================================================

FUNCTION
  OI_RFCOMM_SetLinkContext

DESCRIPTION
  Associates a caller defined context with an RFCOMM link. This context can
  then be retrieved by calling OI_RFCOMM_GetLinkContext.

===========================================================================*/
OI_STATUS OI_RFCOMM_SetLinkContext(OI_RFCOMM_LINK_HANDLE handle, void *context)
{
  uint16 i;

  for (i = 0; i < BT_PF_MAX_DISPATCH; i++)
  {
    if (bt_pf_sio_table[i].stream_id == (sio_stream_id_type)handle)
    {
      bt_pf_sio_table[i].link_context = context;
      return OI_OK;
    }
  }
  return OI_STATUS_NOT_FOUND;
}

/*===========================================================================

FUNCTION
  OI_RFCOMM_GetLinkContext

DESCRIPTION
  Gets a caller defined context associate with an RFCOMM link. This is a value
  that we previously set by a call to OI_RFCOMM_SetLinkContext.

===========================================================================*/
void* OI_RFCOMM_GetLinkContext(OI_RFCOMM_LINK_HANDLE handle)
{
  uint16 i;

  for (i = 0; i < BT_PF_MAX_DISPATCH; i++)
  {
    if (bt_pf_sio_table[i].stream_id == (sio_stream_id_type)handle)
    {
      return(bt_pf_sio_table[i].link_context);
    }
  }
  return NULL;
}

#if defined(FEATURE_MT_OBEX) && defined(FEATURE_PCAA_SIRIUS)
#error code not present
#endif /* defined(FEATURE_MT_OBEX) && defined(FEATURE_PCAA_SIRIUS) */

/*===========================================================================

        FUNCTIONS FOR MAPPING SDP AND DEVICE MANAGER APIs

===========================================================================*/

/*===========================================================================

DESCRIPTION
  The following functions are not populated. These operations needs to be
  done by calling the corresponding bt_sd commands.

===========================================================================*/

OI_STATUS OI_SDPDB_AddServiceRecord(
                                OI_SDPDB_SERVICE_RECORD const *ServiceRecord,
                                OI_UINT32 *ServiceRecordHandle)
{
  return OI_OK;
}

OI_STATUS OI_SDPDB_RemoveServiceRecord(OI_UINT32 ServiceRecordHandle)
{
  return OI_OK;
}

OI_STATUS OI_SDPDB_SetAttributeList(OI_UINT32 serviceRecordHandle,
                                    OI_SDPDB_ATTRIBUTE const *attributeList,
                                    OI_UINT16 numAttributes)
{
  return OI_OK;
}

OI_STATUS OI_SDPDB_RemoveAttributeList(OI_UINT32 serviceRecordHandle,
                                       OI_SDPDB_ATTRIBUTE **attributeList,
                                       OI_UINT16 *numAttributes)
{
  return OI_OK;
}
void OI_SDPDB_FreeAttributeListElements(OI_SDPDB_ATTRIBUTE *attributeList,
                                        OI_UINT16 numAttributes)
{
  return ;
}
OI_STATUS OI_SDP_UTILS_BuildPDLAttribute(OI_UUID32 upperProtocol,
                                         OI_UINT8 channelNumber,
                                         OI_SDPDB_ATTRIBUTE *pdlAttribute)
{
  return OI_OK;
}

void OI_SDP_UTILS_FreeAttributeListElements(OI_SDPDB_ATTRIBUTE *attributeList,
                                            OI_UINT16 numAttributes)
{
  return;
}

OI_STATUS OI_DEVMGR_RegisterServiceClass(OI_UINT32 serviceClass)
{
  return OI_OK;
}

OI_STATUS OI_DEVMGR_DeregisterServiceClass(OI_UINT32 serviceClass)
{
  return OI_OK;
}

/*===========================================================================

          FUNCTIONS FOR MAPPING SECURITY DATABASE APIs

===========================================================================*/

OI_SECURITY_POLICY OI_SECURITYDB_GetPolicy(OI_UINT32 serviceUuid)
{
  return(OI_SEC_REQUIRE_NOTHING);
}

OI_SECURITY_POLICY OI_SECMGR_ResolvePolicies(
  OI_SECURITY_POLICY  policy1,
  OI_SECURITY_POLICY  policy2)
{
  return ( (policy1 > policy2) ? policy1 : policy2 );
}

/*===========================================================================

           FUNCTIONS FOR MAPPING MISCELLANEOUS UTILITY APIs

===========================================================================*/

/*===========================================================================

FUNCTION
  bt_pf_err_oi

DESCRIPTION
  Handle OI error messages.

===========================================================================*/
void bt_pf_err_oi (const char * format, ...)
{
#define BT_PF_DEBUG_STR "BT PF OI: "
#define BT_PF_DEBUG_STR_LEN 10
  static char    debug_buf[255];
  AEEVaList      args;

  AEEVA_START( args, format );
  (void)BT_VSTRFORMAT( debug_buf+BT_PF_DEBUG_STR_LEN, 
                       sizeof(debug_buf) - BT_PF_DEBUG_STR_LEN,
                       format, args );
  AEEVA_END( args );

  BT_STRCPY( debug_buf, BT_PF_DEBUG_STR, sizeof(debug_buf) );

  BT_ERR( debug_buf,0,0,0);
}

/*===========================================================================

FUNCTION
  bt_pf_copy_bd_addr_from_oi

DESCRIPTION
  Helper function used to copy OI data from one structure to another.

===========================================================================*/
void bt_pf_copy_bd_addr_from_oi( bt_bd_addr_type *dest, const OI_BD_ADDR *src )
{
  int i;
  for ( i = 0; i < sizeof(bt_bd_addr_type); i++ )
  {
    ((byte*)dest)[i] = ((byte*)src)[i];
  }
}

/*===========================================================================

FUNCTION
  bt_pf_copy_bd_addr_to_oi

DESCRIPTION
  Utility function to copy a Bluetooth driver BD addr to an OI BD Addr.

===========================================================================*/
LOCAL void bt_pf_copy_bd_addr_to_oi( OI_BD_ADDR* dest,
                                     bt_bd_addr_type* src )
{
  int i;
  for ( i = 0; i < sizeof(bt_bd_addr_type); i++ )
  {
    ((byte*)dest)[i] = ((byte*)src)[i];
  }
}

/*===========================================================================
FUNCTION
  bt_pf_map_event_code

DESCRIPTION
  Maps a BT event reason to an OI_STATUS
===========================================================================*/
LOCAL OI_STATUS bt_pf_map_event_code( bt_event_reason_type reason )
{
  int i;
  if( reason >= BT_EVR_HCI_START &&
      reason <  BT_EVR_HCI_END )
  {
    // This is an HCI status. Map directly to command status.
    return (OI_STATUS)reason;
  }

  for( i = 0; bt_pf_event_map[i].reason != BT_EVR_GN_SUCCESS; i++ )
  {
    if( bt_pf_event_map[i].reason == reason )
    {
      return bt_pf_event_map[i].status;
    }
  }
  // If we can't find a reason, return OI_OK
  BT_ERR("BT PF: Can't find a status for reason %d",reason,0,0);
  return OI_OK;
}


/*===========================================================================

FUNCTION
  OI_StrLen

DESCRIPTION
  Returns length of a null-terminated string.

===========================================================================*/
OI_INT OI_StrLen(OI_CHAR const *pStr)
{
  return(strlen( pStr ));
}

/*===========================================================================

FUNCTION
  OI_Strcmp

DESCRIPTION
  Compares the contents of two strings.

===========================================================================*/
OI_INT OI_Strcmp(OI_CHAR const *p1, OI_CHAR const *p2)
{
  return(strcmp( p1, p2 ));
}

/*===========================================================================

FUNCTION
  OI_StrcmpInsensitive

DESCRIPTION
  Compares the contents of two strings ignoring case of alphabetical
  contents.

===========================================================================*/
OI_INT OI_StrcmpInsensitive(OI_CHAR const *p1, OI_CHAR const *p2)
{
  OI_CHAR uc1;
  OI_CHAR uc2;

  OI_ASSERT(p1 != NULL);
  OI_ASSERT(p2 != NULL);

  for (;;)
  {
    uc1 = OI_toupper(*p1);
    uc2 = OI_toupper(*p2);

    if (uc1 < uc2) return -1;
    if (uc1 > uc2) return 1;
    if (uc1 == '\0') return 0;
    ++p1;
    ++p2;
  }
}
/*===========================================================================

FUNCTION
  OI_StrncmpInsensitive

DESCRIPTION
  Compares the contents of two strings ignoring case of alphabetical
  contents.

===========================================================================*/

OI_INT OI_StrncmpInsensitive(OI_CHAR const *p1,
                             OI_CHAR const *p2,
                             OI_UINT16 len)
{
    OI_CHAR uc1;
    OI_CHAR uc2

    OI_ASSERT(p1 != NULL);
    OI_ASSERT(p2 != NULL);

    while (len--) {
        uc1 = OI_toupper(*p1);
        uc2 = OI_toupper(*p2);
        if (uc1 < uc2) return -1;
        if (uc1 > uc2) return 1;
        if (uc1 == '\0') return 0;
        p1++;
        p2++;
    }
    return 0;
}

/*===========================================================================

FUNCTION
  OI_Time_Now

DESCRIPTION
  Returns a time stamp.
  (This function is used by OBEX authentication to generate a unique,
   non-repeating bit string.)

===========================================================================*/
void OI_Time_Now(OI_TIME *now)
{
  qword ms;

  clk_read_ms( ms );
  now->seconds = (OI_INT32)(QW_CVT_Q2N( ms ) / 1000);
  now->mseconds = (OI_INT16)(QW_CVT_Q2N( ms ) % 1000);

  return;
}

/*===========================================================================

FUNCTION
  OI_LogError

DESCRIPTION
  Log error handler - indicate module and line number of error

===========================================================================*/
void OI_LogError(OI_MODULE module, OI_INT lineno)
{
  /* Send error info to console */
  BT_MSG_DEBUG("BT PF: OI_LogError %d,%d", module, lineno, 0);
}


/*===========================================================================

                    FUNCTIONS FOR SIGNAL HANDLING

===========================================================================*/

#ifdef FEATURE_BT_EXTPF_SAP
#error code not present
#endif

/*===========================================================================

FUNCTION
  bt_pf_data_received

DESCRIPTION
  Handles the data received signal by invoking the upper level callback.

===========================================================================*/
void bt_pf_data_received()
{
  uint8 i;

  for (i = 0; i < BT_PF_SIO_MAX_PORTS; i++)
  {
    /* check channel mask */
    if (bt_pf_ch_bit[i] & bt_pf_ch_masks.rcv_ind)
    {
      bt_pf_ch_masks.rcv_ind &= ~bt_pf_ch_bit[i]; /* clear ch mask bit */

#ifdef FEATURE_BT_EXTPF_SAP
#error code not present
#endif
#ifdef FEATURE_BT_EXTPF_GOEP
      if (bt_pf_sio_table[i].state == BT_PF_SIO_CLI_CONNECTED)
      {
        bt_pfgoep_cli_rc_rcv_ind( bt_pf_sio_table[i].stream_id,
                                  &bt_pf_sio_table[i].rx_wm );
      }
      else if (bt_pf_sio_table[i].state == BT_PF_SIO_SRV_CONNECTED)
      {
        bt_pfgoep_srv_rc_rcv_ind( bt_pf_sio_table[i].stream_id,
                                  &bt_pf_sio_table[i].rx_wm );
      }
      else
      {
        BT_ERR("BT PF: Data received in unexp state=%d!", 
               bt_pf_sio_table[i].state, 0, 0);
      }
#endif
    }
  }
}

/*===========================================================================

FUNCTION
  bt_pf_write_completed

DESCRIPTION
  Handles the write complete signal by invoking the upper level callback.

===========================================================================*/
void bt_pf_write_completed()
{
  uint8 i;

  for (i = 0; i < BT_PF_SIO_MAX_PORTS; i++)
  {
    /* check channel mask */
    if (bt_pf_ch_bit[i] & bt_pf_ch_masks.wrt_cfm)
    {
      bt_pf_ch_masks.wrt_cfm &= ~bt_pf_ch_bit[i]; /* clear ch mask bit */

      if (bt_pf_sio_table[i].wrt_cfm.fptr != NULL)
      {
        (bt_pf_sio_table[i].wrt_cfm.fptr)(
          bt_pf_sio_table[i].stream_id,
          bt_pf_sio_table[i].mbuf_ptr,
          bt_pf_sio_table[i].wrt_cfm.result );
      }
      /* discard data related to this write */
      bt_pf_sio_table[i].mbuf_ptr = NULL;
      bt_pf_sio_table[i].wrt_cfm.fptr = NULL;

      if ( bt_pf_sio_table[i].state == BT_PF_SIO_CLI_SENDING ) 
      {
        bt_pf_sio_table[i].state = BT_PF_SIO_CLI_CONNECTED;
      }
      else if ( bt_pf_sio_table[i].state == BT_PF_SIO_SRV_SENDING ) 
      {
        bt_pf_sio_table[i].state = BT_PF_SIO_SRV_CONNECTED;
      }
      else
      {
        BT_ERR("BT PF: Write Completed unexp state! idx=%d state=%x", 
               i, bt_pf_sio_table[i].state, 0);
      }
    }
  }
}

/*===========================================================================

FUNCTION
  bt_pf_connected

DESCRIPTION
  Handles the connection complete signal by invoking the upper level callback.

===========================================================================*/
void bt_pf_connected()
{
  uint8 i;

  for (i = 0; i < BT_PF_SIO_MAX_PORTS; i++)
  {
    /* check channel mask */
    if (bt_pf_ch_bit[i] & bt_pf_ch_masks.con_cfm)
    {
      bt_pf_ch_masks.con_cfm &= ~bt_pf_ch_bit[i]; /* clear ch mask bit */

      if (bt_pf_sio_table[i].con_cfm.fptr != NULL)
      {
        (bt_pf_sio_table[i].con_cfm.fptr)( bt_pf_sio_table[i].stream_id,
                                       bt_pf_sio_table[i].max_frame_size,
                                       bt_pf_sio_table[i].con_cfm.result );
      }
      if (bt_pf_sio_table[i].con_cfm.result != OI_OK)
      {
        /* connection failed, cleanup */
        sio_close(bt_pf_sio_table[i].stream_id, NULL);
        bt_pf_free_table_entry(i);
      }
    }
  }
}

/*===========================================================================

FUNCTION
  bt_pf_connection_received

DESCRIPTION
  Handles the connection received signal by invoking the upper level callback.

===========================================================================*/
void bt_pf_connection_received()
{
  uint8 i;

  for (i = 0; i < BT_PF_SIO_MAX_PORTS; i++)
  {
    /* check channel mask */
    if (bt_pf_ch_bit[i] & bt_pf_ch_masks.con_ind)
    {
      bt_pf_ch_masks.con_ind &= ~bt_pf_ch_bit[i]; /* clear ch mask bit */
      if (bt_pf_sio_table[i].con_ind.fptr != NULL)
      {
        (bt_pf_sio_table[i].con_ind.fptr)(
          (OI_BD_ADDR*)&bt_pf_sio_table[i].con_ind.remote_bd_addr,
          bt_pf_sio_table[i].scn,
          bt_pf_sio_table[i].max_frame_size,
          (OI_RFCOMM_LINK_HANDLE)bt_pf_sio_table[i].stream_id );
      }
    }
  }
}

/*===========================================================================

FUNCTION
  bt_pf_disconnected

DESCRIPTION
  Handles the disconnected signal by invoking the upper level callback.

===========================================================================*/
void bt_pf_disconnected()
{
  uint8 i;
#if defined(FEATURE_MT_OBEX) && defined(FEATURE_PCAA_SIRIUS)
#error code not present
#endif

  for (i = 0; i < BT_PF_SIO_MAX_PORTS; i++)
  {
    /* check channel mask */
    if (bt_pf_ch_bit[i] & bt_pf_ch_masks.dcn_ind)
    {
      bt_pf_ch_masks.dcn_ind &= ~bt_pf_ch_bit[i]; /* clear ch mask bit */
      if (bt_pf_sio_table[i].dcn_ind.fptr != NULL)
      {
        (bt_pf_sio_table[i].dcn_ind.fptr)( bt_pf_sio_table[i].stream_id,
                                       bt_pf_sio_table[i].dcn_ind.reason );
      }
      dsm_empty_queue( &(bt_pf_sio_table[i].tx_wm) );
      dsm_empty_queue( &(bt_pf_sio_table[i].rx_wm) );
      if (bt_pf_sio_table[i].state >= BT_PF_SIO_SRV_FIRST_STATE)
      {
        /* for server, go back to registered state */
        bt_pf_sio_table[i].state = BT_PF_SIO_SRV_REGISTERED;
        memset( (byte*)(&bt_pf_sio_table[i].con_ind.remote_bd_addr), 0,
                sizeof( bt_bd_addr_type ) );
#if defined(FEATURE_MT_OBEX) && defined(FEATURE_PCAA_SIRIUS)
#error code not present
#endif
      }
      else
      {
        /* for client, cleanup */
        sio_close( bt_pf_sio_table[i].stream_id, NULL );
        bt_pf_free_table_entry( i );
      }
    }
  }
}

/*===========================================================================

FUNCTION
  bt_pf_rls_received

DESCRIPTION
  Handles the RLS received signal by invoking the upper level callback.

===========================================================================*/
void bt_pf_rls_received()
{
  uint8 i;

  for (i = 0; i < BT_PF_SIO_MAX_PORTS; i++)
  {
    /* check channel mask */
    if (bt_pf_ch_bit[i] & bt_pf_ch_masks.rls_ind)
    {
      bt_pf_ch_masks.rls_ind &= ~bt_pf_ch_bit[i]; /* clear ch mask bit */
      if (bt_pf_sio_table[i].rls_ind.fptr != NULL)
      {
        (bt_pf_sio_table[i].rls_ind.fptr)( bt_pf_sio_table[i].stream_id,
                                       bt_pf_sio_table[i].rls_ind.status );
      }
    }
  }
}

/*===========================================================================

FUNCTION
  bt_pf_msc_received

DESCRIPTION
  Handles the MSC received signal by invoking the upper level callback.

===========================================================================*/
void bt_pf_msc_received()
{
  uint8 i;

  for (i = 0; i < BT_PF_SIO_MAX_PORTS; i++)
  {
    /* check channel mask */
    if (bt_pf_ch_bit[i] & bt_pf_ch_masks.msc_ind)
    {
      bt_pf_ch_masks.msc_ind &= ~bt_pf_ch_bit[i]; /* clear ch mask bit */
      if (bt_pf_sio_table[i].msc_ind.fptr != NULL)
      {
        (bt_pf_sio_table[i].msc_ind.fptr)( bt_pf_sio_table[i].stream_id,
                                   bt_pf_sio_table[i].msc_ind.line_ctrl,
                                   bt_pf_sio_table[i].msc_ind.break_ctrl );
      }
    }
  }
}

/*===========================================================================

FUNCTION
  bt_pf_rpn_received

DESCRIPTION
  Handles the RPN received signal by invoking the upper level callback.

===========================================================================*/
void bt_pf_rpn_received()
{
  uint8 i;

  for (i = 0; i < BT_PF_SIO_MAX_PORTS; i++)
  {
    /* check channel mask */
    if (bt_pf_ch_bit[i] & bt_pf_ch_masks.rpn_ind)
    {
      bt_pf_ch_masks.rpn_ind &= ~bt_pf_ch_bit[i]; /* clear ch mask bit */
      if (bt_pf_sio_table[i].rpn_ind.fptr != NULL)
      {
        (bt_pf_sio_table[i].rpn_ind.fptr)( bt_pf_sio_table[i].stream_id,
                                &(bt_pf_sio_table[i].rpn_ind.port_param) );
      }
    }
  }
}

/*===========================================================================

FUNCTION
  bt_pf_timer_expired

DESCRIPTION
  Handles the task master timer tick. This function is set up to be invoked
  every BT_PF_TIMER_INTVL_MS. It handles the dispatcher callbacks.

===========================================================================*/
LOCAL void bt_pf_timer_expired()
{
  DISPATCH_ARG arg;
  uint16 i;

  /* iterates through dispatcher table */
  for (i = 0; i < BT_PF_MAX_DISPATCH; i++)
  {
    if (bt_pf_dispatcher[i].cb_ptr != NULL) /* entry not assigned */
    {
      if (bt_pf_dispatcher[i].tmo == 0) /* timeout expired */
      {
        if (bt_pf_dispatcher[i].arg_ptr == NULL)
        {
          (bt_pf_dispatcher[i].cb_ptr)( NULL );
        }
        else
        {
          /* retrieve callback function arguments */
          arg.size = bt_pf_dispatcher[i].arg_size;
          arg.data = (void*)(bt_pf_dispatcher[i].arg_ptr);
          (bt_pf_dispatcher[i].cb_ptr)( &arg );
          if ( (arg.size > BT_PF_MAX_INLINE_ARG_SIZE) &&
               (arg.data != NULL) )
          {
            /* discard callback function arguments*/
            OI_Free( bt_pf_dispatcher[i].arg_ptr );
            bt_pf_dispatcher[i].arg_ptr = NULL;
          }
        }
        /* release entry in dispatcher table */
        bt_pf_dispatcher[i].cb_ptr = NULL;
      }
      else
      {
        /* decrement timeout count if not yet expired */
        bt_pf_dispatcher[i].tmo--;
      }
    }
  }
}

/*===========================================================================

               BT EXTERNAL PROFILE COMMAND QUEUES

===========================================================================*/
/*===========================================================================

FUNCTION
  bt_pf_init_cmd_buffer

DESCRIPTION
  Initializes a specified BT command buffer.

===========================================================================*/
LOCAL void bt_pf_init_cmd_buffer(bt_pf_cmd_msg_type*  bt_pf_cmd_ptr)
{

  bt_pf_cmd_ptr->cmd_hdr.cmd_type    = BT_PF_CMD_NULL;
  bt_pf_cmd_ptr->cmd_hdr.app_id      = BT_PF_APP_ID_NULL;
  bt_pf_cmd_ptr->cmd_hdr.cmd_state   = BT_PF_CST_DONE;
  bt_pf_cmd_ptr->cmd_hdr.cmd_status  = BT_CS_GN_UNRECOGNIZED_CMD;

}
/*===========================================================================

FUNCTION
  bt_pf_get_free_cmd

DESCRIPTION
  Gets an available BT external profile command buffer.

RETURN VALUE
  Pointer to free BT external profile command buffer if available; otherwise
  NULL.

===========================================================================*/
bt_pf_cmd_msg_type* bt_pf_get_free_cmd(void)
{
  bt_pf_cmd_msg_type*  free_cmd_ptr;

  free_cmd_ptr = (bt_pf_cmd_msg_type*) q_get( &bt_pf_cmd_free_q );
  if( free_cmd_ptr != NULL )
  {
    free_cmd_ptr->cmd_hdr.cmd_state = BT_PF_CST_DONE;
  }
  else
  {
    BT_ERR( "BT PF: Out of cmd buffers", 0, 0, 0 );
  }

  return( free_cmd_ptr );

}

/*===========================================================================

FUNCTION
  bt_pf_free_cmd

DESCRIPTION
  Puts a in-use command queue item back onto the free queue.

RETURN VALUE
  None.

===========================================================================*/
void bt_pf_free_cmd(bt_pf_cmd_msg_type* cmd_ptr)
{
  bt_pf_init_cmd_buffer( cmd_ptr );
  q_put( &bt_pf_cmd_free_q, &cmd_ptr->cmd_hdr.link );
}

/*===========================================================================

FUNCTION
  bt_pf_cmd

DESCRIPTION
  This function takes an already filled out bt_cmd_msg_type and places it
  on the BT external profile command queue.  It is used by all of the routines
  in btpfcmd.c

===========================================================================*/
bt_cmd_status_type bt_pf_cmd(bt_pf_cmd_msg_type*  bt_pf_cmd_ptr)
{
  boolean             free_cmd = TRUE;
  int                 bt_pf_cmd_q_depth;
  bt_cmd_status_type  cmd_status;

  if ( bt_pf_cmd_ptr->cmd_hdr.cmd_state == BT_PF_CST_DONE )
  {
    if ( (bt_pf_cmd_q_depth = q_cnt( &bt_pf_cmd_q )) < BT_PF_NUM_CMDS )
    {
      bt_pf_cmd_ptr->cmd_hdr.cmd_state  = BT_PF_CST_QUEUED;
      cmd_status = BT_CS_GN_PENDING;

      /*  Queue command and signal BT external profile task.  */
      q_put( &bt_pf_cmd_q, &bt_pf_cmd_ptr->cmd_hdr.link );
      (void) rex_set_sigs( &bt_pf_tcb, BT_PF_CMD_Q_SIG );

      free_cmd = FALSE;
    }
    else
    {
      cmd_status = BT_CS_GN_CMD_Q_FULL;
      BT_ERR( "BT PF: Cmd Q Full- Depth %x", bt_pf_cmd_q_depth, 0, 0 );
    }
  }
  else
  {
    cmd_status = BT_CS_GN_BAD_CMD_STATE;
    BT_ERR( "BT PF: Bad Cmd St %x AID %x Typ %x",
            bt_pf_cmd_ptr->cmd_hdr.cmd_state,
            bt_pf_cmd_ptr->cmd_hdr.app_id,
            bt_pf_cmd_ptr->cmd_hdr.cmd_type );
  }

  if ( free_cmd != FALSE )
  {
    bt_pf_free_cmd( bt_pf_cmd_ptr );
  }

  return cmd_status;
}

/*===========================================================================

FUNCTION
  bt_pf_process_command

DESCRIPTION
  Calls the profile specific command processing function to process
  command passed in.

===========================================================================*/
LOCAL void bt_pf_process_command(bt_pf_cmd_msg_type* cmd_ptr)
{
#ifdef FEATURE_BT_EXTPF_GOEP
  if ( BT_PF_CE_GROUP( cmd_ptr->cmd_hdr.cmd_type, GOEP ) )
  {
    bt_pf_goep_process_command( cmd_ptr );
  }
  else
#endif /* FEATURE_BT_EXTPF_GOEP */
#ifdef FEATURE_BT_EXTPF_OPP
  if ( BT_PF_CE_GROUP( cmd_ptr->cmd_hdr.cmd_type, OPP ) )
  {
    bt_pf_opp_process_command( cmd_ptr );
  }
  else
#endif /* FEATURE_BT_EXTPF_OPP */
#ifdef FEATURE_BT_EXTPF_FTP
  if ( BT_PF_CE_GROUP( cmd_ptr->cmd_hdr.cmd_type, FTP ) )
  {
    bt_pf_ftp_process_command( cmd_ptr );
  }
  else
#endif /* FEATURE_BT_EXTPF_FTP */
#ifdef FEATURE_BT_EXTPF_BIP
  if ( BT_PF_CE_GROUP( cmd_ptr->cmd_hdr.cmd_type, BIP ) )
  {
    bt_pf_bip_process_command( cmd_ptr );
  }
  else
#endif /* FEATURE_BT_EXTPF_BIP */
#ifdef FEATURE_BT_EXTPF_BPP
  if ( BT_PF_CE_GROUP( cmd_ptr->cmd_hdr.cmd_type, BPP ) )
  {
    bt_pf_bpp_process_command( cmd_ptr );
  }
  else
#endif /* FEATURE_BT_EXTPF_BPP */
#ifdef FEATURE_BT_EXTPF_SML
  if ( BT_PF_CE_GROUP( cmd_ptr->cmd_hdr.cmd_type, SML ) )
  {
    bt_pf_sml_process_command( cmd_ptr );
  }
  else
#endif /* FEATURE_BT_EXTPF_SML */
#ifdef FEATURE_BT_EXTPF_CTP
  if ( BT_PF_CE_GROUP( cmd_ptr->cmd_hdr.cmd_type, CTP ) )
  {
    bt_pf_ctp_process_command( cmd_ptr );
  }
  else
#endif /* FEATURE_BT_EXTPF_CTP */
#ifdef FEATURE_BT_EXTPF_SAP
#error code not present
#endif /* FEATURE_BT_EXTPF_SAP */
#ifdef FEATURE_BT_EXTPF_AV
  if ( BT_PF_CE_GROUP( cmd_ptr->cmd_hdr.cmd_type, AVDTP ) )
  {
    bt_pf_avdtp_process_command( cmd_ptr );
  }
  else if ( BT_PF_CE_GROUP( cmd_ptr->cmd_hdr.cmd_type, A2DP ) )
  {
    bt_pf_a2dp_process_command( cmd_ptr );
  }
  else if ( BT_PF_CE_GROUP( cmd_ptr->cmd_hdr.cmd_type, AVRCP ) )
  {
    bt_pf_avrcp_process_command( cmd_ptr );
  }
  else
#endif /* FEATURE_BT_EXTPF_AV */
#ifdef FEATURE_BT_EXTPF_PBAP
  if ( BT_PF_CE_GROUP( cmd_ptr->cmd_hdr.cmd_type, PBAP ) )
  {
    bt_pf_pbap_process_command( cmd_ptr );
  }
  else
#endif /* FEATURE_BT_EXTPF_PBAP */
#ifdef FEATURE_BT_EXTPF_HID_HOST
#error code not present
#endif /* FEATURE_BT_EXTPF_HID_HOST */
  {
    cmd_ptr->cmd_hdr.cmd_status = BT_CS_GN_UNRECOGNIZED_CMD;
  }
}

/*===========================================================================

FUNCTION
  bt_pf_process_command_queue

DESCRIPTION
  Dequeue and process command in the profile command queue.

===========================================================================*/
LOCAL void bt_pf_process_command_queue()
{
  bt_pf_cmd_msg_type* cmd_ptr;
  bt_ev_msg_type      ev_cmd_done;
  int                 q_count = q_cnt( &bt_pf_cmd_q );

  ev_cmd_done.ev_hdr.ev_type = BT_EV_GN_CMD_DONE;

  BT_VSTAT_MAX( q_count, PF_MAX_CMD_Q_DEPTH );

  while ( (cmd_ptr = ((bt_pf_cmd_msg_type*)q_get( &bt_pf_cmd_q ))) != NULL )
  {
    if ( cmd_ptr->cmd_hdr.cmd_state == BT_PF_CST_QUEUED )
    {
      cmd_ptr->cmd_hdr.cmd_state = BT_PF_CST_PROCESSING;
      bt_pf_process_command( cmd_ptr );

      if ( cmd_ptr->cmd_hdr.cmd_status == BT_CS_GN_RETRY_CMD_LATER )
      {
        cmd_ptr->cmd_hdr.cmd_status = BT_CS_GN_SUCCESS;

        cmd_ptr->cmd_hdr.cmd_state = BT_PF_CST_QUEUED;
        if ( q_cnt( &bt_pf_cmd_re_q ) == 0 )
        {
          rex_set_timer( &bt_pf_requeued_cmd_q_timer,
                         BT_PF_REQUEUED_CMD_Q_TIMER_MS );
        }
        q_put( &bt_pf_cmd_re_q, &cmd_ptr->cmd_hdr.link );

        BT_MSG_DEBUG( "BT PF: Cmd ReQd AID %x Typ %x Cnt %x",
                      cmd_ptr->cmd_hdr.app_id,
                      cmd_ptr->cmd_hdr.cmd_type,
                      cmd_ptr->cmd_hdr.cmd_retries );
      }
    }
    else
    {
      cmd_ptr->cmd_hdr.cmd_status = BT_CS_GN_BAD_CMD_STATE;
    }

    if (( cmd_ptr->cmd_hdr.cmd_status != BT_CS_GN_SUCCESS ) &&
        ( cmd_ptr->cmd_hdr.cmd_status != OI_OK ) )
    {
      BT_ERR( "BT PF: Bad Cmd Done Stat %x Typ %x AID %x",
              cmd_ptr->cmd_hdr.cmd_status,
              cmd_ptr->cmd_hdr.cmd_type,
              cmd_ptr->cmd_hdr.app_id );
    }

    /*  Send Command Done event.  */
    ev_cmd_done.ev_hdr.bt_app_id = cmd_ptr->cmd_hdr.app_id;
    ev_cmd_done.ev_msg.ev_gn_cmd_done.cmd_type = (bt_cmd_type)cmd_ptr->cmd_hdr.cmd_type;
    ev_cmd_done.ev_msg.ev_gn_cmd_done.bt_app_id = cmd_ptr->cmd_hdr.app_id;
    ev_cmd_done.ev_msg.ev_gn_cmd_done.cmd_status = cmd_ptr->cmd_hdr.cmd_status;
    bt_ec_send_event( &ev_cmd_done );

    if ( cmd_ptr->cmd_hdr.cmd_state == BT_PF_CST_PROCESSING )
    {
      bt_pf_free_cmd( cmd_ptr );
    }
  }
}

/*===========================================================================

FUNCTION
  bt_pf_process_requeued_commands

DESCRIPTION
  Dequeues commands from the BT PF command re-queue and processes them.

===========================================================================*/
LOCAL void bt_pf_process_requeued_commands
(
  boolean  rqd_cmd_q_timer_expired
)
{

  int                  cmd_count;
  uint16               i;
  bt_pf_cmd_msg_type*  cmd_ptr;

  cmd_count = q_cnt( &bt_pf_cmd_re_q );

  BT_VSTAT_MAX( cmd_count, MAX_CMDS_REQUEUED );

  for ( i = 0; i < cmd_count; i++ )
  {
    if ( (cmd_ptr = (bt_pf_cmd_msg_type*)(q_get( &bt_pf_cmd_re_q ))) != NULL )
    {
      if ( cmd_ptr->cmd_hdr.cmd_state == BT_PF_CST_QUEUED )
      {
        cmd_ptr->cmd_hdr.cmd_state = BT_PF_CST_PROCESSING;
        bt_pf_process_command( cmd_ptr );

        if ( cmd_ptr->cmd_hdr.cmd_status == BT_CS_GN_RETRY_CMD_LATER )
        {
          cmd_ptr->cmd_hdr.cmd_status = BT_CS_GN_SUCCESS;

          if ( rqd_cmd_q_timer_expired != FALSE )
          {
            cmd_ptr->cmd_hdr.cmd_retries++;
          }
          
          if ( cmd_ptr->cmd_hdr.cmd_retries < BT_PF_CMD_MAX_RETRIES )
          {
            cmd_ptr->cmd_hdr.cmd_state = BT_PF_CST_QUEUED;
            q_put( &bt_pf_cmd_re_q, &cmd_ptr->cmd_hdr.link );

            BT_MSG_DEBUG( "BT PF: Cmd ReQd Cnt %x Typ %x AID %x",
                          cmd_ptr->cmd_hdr.cmd_retries,
                          cmd_ptr->cmd_hdr.cmd_type,
                          cmd_ptr->cmd_hdr.app_id );
          }
          else
          {
            cmd_ptr->cmd_hdr.cmd_status = BT_CS_GN_MAX_CMD_RETRIES;
          }
        }
      }
      else
      {
        cmd_ptr->cmd_hdr.cmd_status = BT_CS_GN_BAD_CMD_STATE;
      }

      if ( cmd_ptr->cmd_hdr.cmd_status != BT_CS_GN_SUCCESS )
      {
        BT_ERR( "BT PF: Bad Cmd Done Stat- %x Typ %x AID %x",
                cmd_ptr->cmd_hdr.cmd_status,
                cmd_ptr->cmd_hdr.cmd_type,
                cmd_ptr->cmd_hdr.app_id );
      }

      if ( cmd_ptr->cmd_hdr.cmd_state == BT_PF_CST_PROCESSING )
      {
        BT_VSTAT_MAX( cmd_ptr->cmd_hdr.cmd_retries, MAX_CMD_RETRIES );

        bt_pf_init_cmd_buffer( cmd_ptr );
        q_put( &bt_pf_cmd_free_q, &cmd_ptr->cmd_hdr.link );
      }
    }
    else
    {
      BT_ERR( "BT PF: Bad Cmd ReQ Cnt %x %x", cmd_count, i, 0 );
    }
  }
  if ( (rqd_cmd_q_timer_expired != FALSE) && (q_cnt( &bt_pf_cmd_re_q ) != 0) )
  {
    rex_set_timer( &bt_pf_requeued_cmd_q_timer,
                   BT_PF_REQUEUED_CMD_Q_TIMER_MS );
  }

}

/*===========================================================================

                        BT EXTERNAL PROFILE TASK

===========================================================================*/
/*===========================================================================

FUNCTION
  bt_pf_ev_cb

DESCRIPTION
  Event callback function for the BT external profile task.

===========================================================================*/
LOCAL void bt_pf_ev_cb( struct bt_ev_msg_struct* ev_msg_ptr )
{
  switch (ev_msg_ptr->ev_hdr.ev_type)
  {
    case BT_EV_GN_CMD_DONE:
      BT_MSG_DEBUG( "BT PF: Cmd Done, Status: %x, App: %x, Cmd: %x",
                    ev_msg_ptr->ev_msg.ev_gn_cmd_done.cmd_status,
                    ev_msg_ptr->ev_msg.ev_gn_cmd_done.bt_app_id,
                    ev_msg_ptr->ev_msg.ev_gn_cmd_done.cmd_type );
      break;

    default:
      BT_MSG_DEBUG( "BT PF: unknown event type %x",
              ev_msg_ptr->ev_hdr.ev_type, 0, 0 );
  }
  return;
}

/*===========================================================================

FUNCTION
  bt_pf_task_init

DESCRIPTION
  Performs initialization for the BT external profile task.

===========================================================================*/
void bt_pf_task_init()
{
  uint16 i;
  uint32 mask = 1;

  bt_pf_app_id = bt_cmd_ec_get_app_id_and_register( bt_pf_ev_cb );
  ASSERT_DEBUG( bt_pf_app_id != BT_APP_ID_NULL );
  BT_MSG_HIGH( "BT PF AID is %x", bt_pf_app_id, 0, 0 );

  bt_pf_init_dispatcher_table();

  for (i = 0; i < BT_PF_NUM_MBUF; i++)
  {
    bt_pf_mbuf[i].dsm_ptr = NULL;
    bt_pf_mbuf[i].context_ptr = NULL;
    bt_pf_mbuf[i].free = TRUE;
    bt_pf_mbuf[i].dsm_state = BT_PF_MBUF_OK_TO_FREE;
  }
  mem_init_heap( &bt_pf_heap, bt_pf_heap_buf, BT_PF_HEAP_SIZE, NULL );

  /* SIO channel specific initialization */
  for (i = 0; i < BT_PF_SIO_MAX_PORTS; i++)
  {
    bt_pf_free_table_entry( i );

    dsm_queue_init( &bt_pf_sio_table[i].tx_wm, 
                    BT_PF_WM_MAX, 
                    &bt_pf_sio_table[i].to_sio_q );
    bt_pf_sio_table[i].tx_wm.lo_watermark = 100;  /* not used */
    bt_pf_sio_table[i].tx_wm.hi_watermark = BT_PF_WM_HI;

    dsm_queue_init( &bt_pf_sio_table[i].rx_wm, 
                    BT_PF_WM_MAX, 
                    &bt_pf_sio_table[i].from_sio_q );
    bt_pf_sio_table[i].rx_wm.lo_watermark = 100;  /* not used */
    bt_pf_sio_table[i].rx_wm.hi_watermark = BT_PF_WM_HI;

    bt_pf_ch_bit[i] = mask;
    mask = mask << 1;
  }

  /* setup all the hardcoded callback functions */
  bt_pf_sio_table[0].tx_wm.gone_empty_func_ptr = bt_pf_sio_tx_done_cb_0;
  bt_pf_sio_table[0].rx_wm.non_empty_func_ptr  = bt_pf_sio_rx_data_cb_0;
  bt_pf_sio_table[0].spp_stat_fptr       = bt_pf_sio_status_change_cb_0;
  bt_pf_sio_table[0].spp_cfg_fptr        = bt_pf_sio_config_change_cb_0;
  bt_pf_sio_table[0].spp_ms_fptr         = bt_pf_sio_modem_status_cb_0;
  bt_pf_sio_table[0].spp_le_fptr         = bt_pf_sio_line_error_cb_0;
  bt_pf_sio_table[1].tx_wm.gone_empty_func_ptr = bt_pf_sio_tx_done_cb_1;
  bt_pf_sio_table[1].rx_wm.non_empty_func_ptr  = bt_pf_sio_rx_data_cb_1;
  bt_pf_sio_table[1].spp_stat_fptr       = bt_pf_sio_status_change_cb_1;
  bt_pf_sio_table[1].spp_cfg_fptr        = bt_pf_sio_config_change_cb_1;
  bt_pf_sio_table[1].spp_ms_fptr         = bt_pf_sio_modem_status_cb_1;
  bt_pf_sio_table[1].spp_le_fptr         = bt_pf_sio_line_error_cb_1;
  bt_pf_sio_table[2].tx_wm.gone_empty_func_ptr = bt_pf_sio_tx_done_cb_2;
  bt_pf_sio_table[2].rx_wm.non_empty_func_ptr  = bt_pf_sio_rx_data_cb_2;
  bt_pf_sio_table[2].spp_stat_fptr       = bt_pf_sio_status_change_cb_2;
  bt_pf_sio_table[2].spp_cfg_fptr        = bt_pf_sio_config_change_cb_2;
  bt_pf_sio_table[2].spp_ms_fptr         = bt_pf_sio_modem_status_cb_2;
  bt_pf_sio_table[2].spp_le_fptr         = bt_pf_sio_line_error_cb_2;
  bt_pf_sio_table[3].tx_wm.gone_empty_func_ptr = bt_pf_sio_tx_done_cb_3;
  bt_pf_sio_table[3].rx_wm.non_empty_func_ptr  = bt_pf_sio_rx_data_cb_3;
  bt_pf_sio_table[3].spp_stat_fptr       = bt_pf_sio_status_change_cb_3;
  bt_pf_sio_table[3].spp_cfg_fptr        = bt_pf_sio_config_change_cb_3;
  bt_pf_sio_table[3].spp_ms_fptr         = bt_pf_sio_modem_status_cb_3;
  bt_pf_sio_table[3].spp_le_fptr         = bt_pf_sio_line_error_cb_3;
  bt_pf_sio_table[4].tx_wm.gone_empty_func_ptr = bt_pf_sio_tx_done_cb_4;
  bt_pf_sio_table[4].rx_wm.non_empty_func_ptr  = bt_pf_sio_rx_data_cb_4;
  bt_pf_sio_table[4].spp_stat_fptr       = bt_pf_sio_status_change_cb_4;
  bt_pf_sio_table[4].spp_cfg_fptr        = bt_pf_sio_config_change_cb_4;
  bt_pf_sio_table[4].spp_ms_fptr         = bt_pf_sio_modem_status_cb_4;
  bt_pf_sio_table[4].spp_le_fptr         = bt_pf_sio_line_error_cb_4;
  bt_pf_sio_table[5].tx_wm.gone_empty_func_ptr = bt_pf_sio_tx_done_cb_5;
  bt_pf_sio_table[5].rx_wm.non_empty_func_ptr  = bt_pf_sio_rx_data_cb_5;
  bt_pf_sio_table[5].spp_stat_fptr       = bt_pf_sio_status_change_cb_5;
  bt_pf_sio_table[5].spp_cfg_fptr        = bt_pf_sio_config_change_cb_5;
  bt_pf_sio_table[5].spp_ms_fptr         = bt_pf_sio_modem_status_cb_5;
  bt_pf_sio_table[5].spp_le_fptr         = bt_pf_sio_line_error_cb_5;
  bt_pf_sio_table[6].tx_wm.gone_empty_func_ptr = bt_pf_sio_tx_done_cb_6;
  bt_pf_sio_table[6].rx_wm.non_empty_func_ptr  = bt_pf_sio_rx_data_cb_6;
  bt_pf_sio_table[6].spp_stat_fptr       = bt_pf_sio_status_change_cb_6;
  bt_pf_sio_table[6].spp_cfg_fptr        = bt_pf_sio_config_change_cb_6;
  bt_pf_sio_table[6].spp_ms_fptr         = bt_pf_sio_modem_status_cb_6;
  bt_pf_sio_table[6].spp_le_fptr         = bt_pf_sio_line_error_cb_6;
  bt_pf_sio_table[7].tx_wm.gone_empty_func_ptr = bt_pf_sio_tx_done_cb_7;
  bt_pf_sio_table[7].rx_wm.non_empty_func_ptr  = bt_pf_sio_rx_data_cb_7;
  bt_pf_sio_table[7].spp_stat_fptr       = bt_pf_sio_status_change_cb_7;
  bt_pf_sio_table[7].spp_cfg_fptr        = bt_pf_sio_config_change_cb_7;
  bt_pf_sio_table[7].spp_ms_fptr         = bt_pf_sio_modem_status_cb_7;
  bt_pf_sio_table[7].spp_le_fptr         = bt_pf_sio_line_error_cb_7;

#ifdef FEATURE_BT_EXTPF_GOEP
  /* initialize GOEP connection table too */
  bt_pf_goep_srv_target_app_table_init();
  bt_pf_goep_cli_app_conn_table_init();
#endif /* FEATURE_BT_EXTPF_GOEP */

  /* define master timer */
  rex_def_timer( &bt_pf_timer, &bt_pf_tcb, BT_PF_TIMER_SIG );

  /* define dog report timer */
  rex_def_timer( &bt_pf_rpt_timer, &bt_pf_tcb, BT_PF_RPT_TIMER_SIG );

  /* define profile timeout timer */
  rex_def_timer( &bt_pf_tmo_timer, &bt_pf_tcb, BT_PF_TMO_SIG );

  /* define cmd re-queue timer */
  rex_def_timer( &bt_pf_requeued_cmd_q_timer, &bt_pf_tcb, BT_PF_REQUEUED_CMD_Q_SIG );

#ifdef FEATURE_BT_EXTPF_FTP
  rex_def_timer( &bt_pf_ftp_tmo_timer, &bt_pf_tcb, BT_PF_TMO_SIG );
#endif /* FEATURE_BT_EXTPF_FTP */

#ifdef FEATURE_BT_EXTPF_BIP
  rex_def_timer( &bt_pf_bip_cli_tmo_timer, &bt_pf_tcb, BT_PF_TMO_SIG );
  rex_def_timer( &bt_pf_bip_srv_tmo_timer, &bt_pf_tcb, BT_PF_TMO_SIG );
#endif /* FEATURE_BT_EXTPF_BIP */

#ifdef FEATURE_BT_EXTPF_BPP
  rex_def_timer( &bt_pf_bpp_cli_tmo_timer, &bt_pf_tcb, BT_PF_TMO_SIG );
  rex_def_timer( &bt_pf_bpp_srv_tmo_timer, &bt_pf_tcb, BT_PF_TMO_SIG );
#endif /* FEATURE_BT_EXTPF_BPP */

#ifdef FEATURE_BT_EXTPF_HID_HOST
#error code not present
#endif /* FEATURE_BT_EXTPF_HID_HOST */

  /* clear all signals */
  rex_clr_sigs( &bt_pf_tcb, bt_pf_wait_sig_mask |
                BT_PF_WAIT_SIG | BT_PF_OPP_SIG | BT_PF_FTP_SIG |
                BT_PF_BIP_CLI_SIG | BT_PF_BIP_SRV_SIG |
                BT_PF_BPP_CLI_SIG | BT_PF_BPP_SRV_SIG );

  /* initialize task queues */
  q_init( &bt_pf_cmd_q );
  q_init( &bt_pf_cmd_re_q );
  q_init( &bt_pf_cmd_free_q );

  for ( i = 0; i < BT_PF_NUM_CMDS; i++ )
  {
    bt_pf_init_cmd_buffer( &bt_pf_cmds[i] );
    q_put( &bt_pf_cmd_free_q, q_link( &bt_pf_cmds[i],
                                      &bt_pf_cmds[i].cmd_hdr.link ) );
  }

  /* set all profile configuration parameters to default values */
  OI_ConfigTable_Init();
  OI_InitFlags_ResetAllFlags();

  /* Init l2if  */
  bt_pf_l2if_init();

  /* Init rmif */
  bt_pf_rmif_init();

#ifdef FEATURE_BT_EXTPF_AV
  /* Init a2dp */
  bt_pf_a2dp_init();
#endif /* FEATURE_BT_EXTPF_AV */

  /* Init Client Auth Data */
  bt_cmd_pf_goep_cli_init_data();

#ifdef FEATURE_BT_EXTPF_HID_HOST
#error code not present
#endif

  return;
}


/*===========================================================================

FUNCTION
  bt_pf_task

DESCRIPTION
  This is the BT external profile task.

===========================================================================*/
void bt_pf_task(dword ignored) /* Parameter received from Rex - ignored */
{
  uint32                isave;
  rex_sigs_type         rex_signals;

  bt_pf_task_init();

  /*  Tell MC we are created and wait for start signal.  */
#ifdef T_MSM6100
  task_start( BT_PF_RPT_TIMER_SIG, DOG_BT_PF_RPT, &bt_pf_rpt_timer );
#else
#error code not present
#endif

  /* start master and dog timers */
  rex_set_timer( &bt_pf_timer, BT_PF_TIMER_INTVL_MS );
  rex_set_timer( &bt_pf_rpt_timer, DOG_BT_PF_RPT_TIME );

  rex_clr_sigs( &bt_pf_tcb, BT_PF_TASK_START_SIG );

  BT_MSG_DEBUG("BT PF: task started", 0, 0, 0);

  while ( 1 )  /*  Never exit this loop.  */
  {
    rex_signals = rex_wait( bt_pf_wait_sig_mask );

    if ( rex_signals & BT_PF_WRT_CFM_SIG )
    {
      rex_clr_sigs( &bt_pf_tcb, BT_PF_WRT_CFM_SIG );
      bt_pf_write_completed();
    }
    if ( rex_signals & BT_PF_REQUEUED_CMD_Q_SIG )
    {
      rex_clr_sigs( &bt_pf_tcb, BT_PF_REQUEUED_CMD_Q_SIG );
      bt_pf_process_requeued_commands( TRUE );
    }
    if ( rex_signals & BT_PF_CMD_Q_SIG )
    {
      bt_pf_process_command_queue();
      INTLOCK_SAV( isave );
      if ( q_cnt( &bt_pf_cmd_q ) == 0 )
      {
        rex_clr_sigs( &bt_pf_tcb, BT_PF_CMD_Q_SIG );
      }
      INTFREE_SAV( isave );
    }
    if ( rex_signals & BT_PF_RCV_IND_SIG )
    {
      rex_clr_sigs( &bt_pf_tcb, BT_PF_RCV_IND_SIG );
      bt_pf_data_received();
    }
    if ( rex_signals & BT_PF_CON_CFM_SIG )
    {
      rex_clr_sigs( &bt_pf_tcb, BT_PF_CON_CFM_SIG );
      bt_pf_connected();
    }
    if ( rex_signals & BT_PF_CON_IND_SIG )
    {
      rex_clr_sigs( &bt_pf_tcb, BT_PF_CON_IND_SIG );
      bt_pf_connection_received();
    }
    if ( rex_signals & BT_PF_DCN_IND_SIG )
    {
      rex_clr_sigs( &bt_pf_tcb, BT_PF_DCN_IND_SIG );
      bt_pf_disconnected();
    }
    if ( rex_signals & BT_PF_RLS_IND_SIG )
    {
      rex_clr_sigs( &bt_pf_tcb, BT_PF_RLS_IND_SIG );
      bt_pf_rls_received();
    }
    if ( rex_signals & BT_PF_MSC_IND_SIG )
    {
      rex_clr_sigs( &bt_pf_tcb, BT_PF_MSC_IND_SIG );
      bt_pf_msc_received();
    }
    if ( rex_signals & BT_PF_RPN_IND_SIG )
    {
      rex_clr_sigs( &bt_pf_tcb, BT_PF_RPN_IND_SIG );
      bt_pf_rpn_received();
    }
    if ( rex_signals & BT_PF_TIMER_SIG )
    {
      rex_clr_sigs( &bt_pf_tcb, BT_PF_TIMER_SIG );
      rex_set_timer( &bt_pf_timer, BT_PF_TIMER_INTVL_MS );
      bt_pf_timer_expired();
    }
    if ( rex_signals & BT_PF_RPT_TIMER_SIG )
    {
      dog_report( DOG_BT_PF_RPT );
      rex_set_timer( &bt_pf_rpt_timer, DOG_BT_PF_RPT_TIME );
    }
    if ( rex_signals & BT_PF_L2IF_EV_SIG )
    {
      rex_clr_sigs( &bt_pf_tcb, BT_PF_L2IF_EV_SIG );
      bt_pf_l2if_process_ev_queue();
    }
    if ( rex_signals & BT_PF_RMIF_EV_SIG )
    {
      rex_clr_sigs( &bt_pf_tcb, BT_PF_RMIF_EV_SIG );
      bt_pf_rmif_process_ev_queue();
    }
#ifdef FEATURE_BT_EXTPF_AV
    if ( rex_signals & BT_PF_A2DP_EV_SIG )
    {
      rex_clr_sigs( &bt_pf_tcb, BT_PF_A2DP_EV_SIG );
      bt_pf_a2dp_process_ev_queue();
    }
#endif /* FEATURE_BT_EXTPF_AV */

#ifdef FEATURE_BT_EXTPF_HID_HOST
#error code not present
#endif /* FEATURE_BT_EXTPF_HID_HOST */

#if defined(FEATURE_MT_OBEX) && defined(FEATURE_PCAA_SIRIUS)
#error code not present
#endif /* defined(FEATURE_MT_OBEX) && defined(FEATURE_PCAA_SIRIUS) */

    if ( rex_signals & (BT_PF_TASK_STOP_SIG | BT_PF_TASK_OFFLINE_SIG) )
    {
      rex_clr_sigs( &bt_pf_tcb, (BT_PF_TASK_STOP_SIG | BT_PF_TASK_OFFLINE_SIG) );
      if ( rex_signals & BT_PF_TASK_STOP_SIG )
      {
        MSG_MED( "BT PF: got stop sig", 0, 0, 0 );
      }
      else
      {
        MSG_MED( "BT PF: got offline sig", 0, 0, 0 );
      }
#ifdef T_MSM6100
      task_ack();
#else
#error code not present
#endif
    }
  }
}

/*===========================================================================

FUNCTION
  bt_pf_store_bt_event

DESCRIPTION
  Stores a specified BT event on a specified BT driver internal
  event queue.

===========================================================================*/
void bt_pf_store_bt_event
(
  bt_ev_msg_type*           bt_ev_ptr,
  bt_pf_event_q_info_type*  evqi_ptr
)
{

  bt_ev_msg_type*  ev_ptr;
  int              ev_q_depth;

  if ( (ev_q_depth = q_cnt( evqi_ptr->event_q_ptr )) <
          evqi_ptr->max_q_depth )
  {
    if ( (ev_ptr =
            (bt_ev_msg_type*)q_get( &bt_event_free_q )) != NULL )
    {
      memcpy( (byte*)ev_ptr, (byte*)bt_ev_ptr,
              evqi_ptr->max_event_bytes );

      q_put( evqi_ptr->event_q_ptr, &ev_ptr->ev_hdr.link );

      (void) rex_set_sigs( &bt_pf_tcb, evqi_ptr->pf_task_sig );
    }
    else
    {
      bt_free_any_dsms_in_event( bt_ev_ptr );
      BT_ERR( "BT PF EVQ: Overflow D %x M %x B %x",
              ev_q_depth, evqi_ptr->max_q_depth,
              evqi_ptr->pf_task_sig );
    }
  }
  else
  {
    bt_free_any_dsms_in_event( bt_ev_ptr );
    BT_ERR( "BT EVQ: Overflow D %x M %x B %x",
              ev_q_depth, evqi_ptr->max_q_depth,
              evqi_ptr->pf_task_sig );
  }

}


#endif /* FEATURE_BT_EXTPF */

#endif /* FEATURE_BT */


