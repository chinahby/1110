/*====*====*====*====*====*====*====*====*====*====*====*====*====*====*====*

B L U E T O O T H   A D V A N C E D   A U D I O   D I S T R I B U T I O N
P R O F I L E

GENERAL DESCRIPTION
This module contains the Bluetooth A2DP interface (commands and events)
implementation.

This implementation is designed to talk to only ONE device, and only
one stream end point which supports an SBC sink.

        Copyright (c) 2005-2009 QUALCOMM Incorporated.
               All Rights Reserved.
        Qualcomm Confidential and Proprietary
*====*====*====*====*====*====*====*====*====*====*====*====*====*====*====*/
/*===========================================================================

EDIT HISTORY FOR MODULE

This section contains comments describing changes made to the module.
Notice that changes are listed in reverse chronological order.

$Header: //source/qcom/qct/wconnect/bthost/pf/rel/00.00.26/src/btpfa2dp.c#3 $
$DateTime: 2009/06/08 17:25:38 $

 when        who  what, where, why
 ----------  ---  -----------------------------------------------------------
 2009-06-08  jtl  Updating slow UART support.
 2009-04-03   jn  For slow UARTs, restrict max A2DP bit-rates to 230kbps.
 2008-12-16   hs  Updated the discon reason across if stream is opened after
                  closing it. 
 2008-12-12   hs  Updated the disconnect reason in case stream is opened after 
                  closing it. In case connection is yanked due to any reason 
				  apart from Normal disconnect and link loss, target would 
				  initiate the reconnection.  
 2008-11-07   hs  Added the handling to disconnect the connection if close is 
                  rejected by the remote device due to any reason  
 2008-05-27   rb  Fix compiler warnings
 2008-03-21   rs  Increased the MAX_A2DP_FRAMES to cater the streaming of 
                  512kbps. The bitpool generally tested was around 50 and it 
                  has now increased close to 85 for the test. Also it is on
                  the safer side for a higher bitpool as well.

*
*   #74      05 Mar 2008          HS 
* Added the handling to restrict the continous attempts to abort the
* stream.   
*
*   #73      05 Feb 2008          JTL
* Handle new app ctl commands properly when in the ACP state.
*
*   #72      01 Feb 2008          HS  
* Fixed a bug in the state machine while disconnection.   
*
*   #71      16 Jan 2008          HS
* Added the handling if stream close command doesn't succeed.
*
*   #70      19 Dec 2007          JTL
* Fixed bugs in state machine.
* 
*   #69      19 Dec 2007          JTL & PN
* Added support for applications to start A2DP streaming rather than leaving
* it in AVS's control.
*
*   #68      27 Nov 2007          PR
* Added BD Address to bit rate change event
*
*   #67      21 Nov 2007          PR
* Added BD Address to all A2DP events
*
*   #66      21 May 2007          SSK
* Fix compiler warnings by typecasting enum types.
*
*   #65      10 Apr 2007          JTL
* Fix more holes left over from change #61 -- always send suspend
* event after receiving suspend command.
* Fix #63 -- remember disconnect command even if cmd_suspend is
* received.
*
*   #63-64   22 Feb 2007          JTL
* Adding ATTEMPT_TO_DISCONNECT and ATTEMPT_TO_DISABLE substates to allow
* A2DP to disconnect/disable regardless of the current state.
*
*   #62      22 Feb 2007          GS
* Fixed critical lint error.
*
*   #61      01 Feb 2007          JTL
* Always send a suspend event when receiving a suspend command.
*
*   #60      06 Dec 2006          MP
* Fixed a2dp driver to send Suspend_evt to App when driver sends
* Close command to SNK during streaming state.
*
*   #59      16 Nov 2006          DM
* Added Debug print to track AVS-Acknowledgement Requests for Av-Sync.
*
*   #58      08 Nov 2006          DM
* Fixed Compiler error - Removing reference to bt_bb_tx_flow_on when AV_SYNC 
* is not defined .
*
*   #57      03 Nov 2006          DM
* Changes for A2DP integration with SOC
*  Increased BT_PF_A2DP_PENDING_DATA_TIMEOUT_MS to 30 ms
*  Removed bt_bb_tx_flow_on and using acl_tx_flow interface for A2DP flow control
*  Temporary for SOC - Moved snd_sbc_init() from powerup init to A2DP Enable.
*
*   #56      23 Oct 2006          JTL
* Extend ACP timeout as INT continues connection sequence.
* If the remote INT finishes the connection/streaming sequence
* after the ACP timeout has expired (but before we initiate streaming
* ourselves) be sure to keep track of the correct state.
*
*   #55      12 Sep 2006          JTL
* Close the media stream if there is an error with suspend. Previous
* behavior was to ignore errors suspending the stream.
*
*   #54      07 Sep 2006          DM
* Reduced a2dp frame handle Q size (Memory optimization) 
* Now queing only frames that require acknowlegments in the a2dp Q as
* part of performance optimization.
*
*   #53      14 Aug 2006          DM
* Minor changes to debug prints for performance optimization
*
*   #52      10 Aug 2006          JTL
* Keeping track of correct PCM timestamp in case the bitrate or
* SBC frame sizes change in mid-stream.
*
*   #51      26 Jul 2006          DM
* Added a2dp frame handle Qs.
* Changes related to AV-Sync, handling of transmit cfm.
*
*   #50      26 Jul 2006          RP
* Updated the code, to pass various disconnection reasons to APP.
*
*   #48-49   06 Jul 2006          JTL
* Fixing more cases of incoming connections.
* Fixing some race conditions with incoming connections.
* Inform apps immediately about remote starting the stream, even if the
* connect event has not yet been sent.
* Fix memory leak if rxing data (this shouldn't happen).
* 
*   #47      16 Jun 2006          DSN
* Added featurization for SoC solution.
*
*   #46      19 May 2006          JTL
* Attempting to honor remote configuration of stream. Allowing remote
* to reconfigure SBC stream as well.
* More tweaking to the ACP timeout. Attempt to be spec compliant and
* still interoperable by not sending AVDTP_START if the remote opened
* the stream.
* Don't send a connected event more than once per connection.
*
*   #45      15 May 2005          JTL
* Updating debug messages.
*
* Fixing connect command so it doesn't overwrite BDaddr if already in a
* connection.
*
* Updating reconfig so multiple start commands can be received while
* reconfiguring.
*
* Adding debug messages to support determining command delays. Useful for
* determining delays introduced by tasks higher priority than BT, but lower
* than AVS.
*
*   #42-43   26 Apr 2006          JTL
* Enhance & Fix #41.
*
*   #42      24 Apr 2006          PN
* Replaced some BT_MSG_DEBUG() with BT_MSG_API() for received commands.
*
*   #41      05 Apr 2006          JTL
* Handle back-to-back suspend and start commands.
*
*   #40      04 Apr 2006          JTL
* Don't clear out info about remote SEID until we're done using it.
*
*   #39      08 Mar 2006          JTL
* Cover all cases to auto-suspend stream when remote opens it.
*
*   #38      13 Feb 2006          JTL
* Don't send empty AVDTP frames.
* Don't re-initialize data in bt_pf_a2dp_cmd_enable if it's already
* initialized.
*
*   #37      13 Feb 2006          JTL
* Better handling of remote closing audio stream, and receiving a Suspend
* command while not in the streaming state.
*
*   #36      20 Jan 2006          JTL
* Allow Start commands to be processed while in the Suspending state.
*
*   #35      14 Dec 2005          JTL
* Fixing possible double-free of DSM items. Don't send DSM items to L2
* and also free them.
*
*   #34      30 Nov 2005          JTL
* Now taking A2DP header size into account when keeping frames within MTU
* limitations.
*
*   #33      15 Nov 2005          JTL
* Keep track of a separate app ID for AVS, and send connect/start events
* directly.
*
*   #32       9 Nov 2005          DP
* Now use new SD service registration command soas to not
* be connectable solely for the SD service registration.
*
*   #31      08 Nov 2005          JTL
* Send old data immediately, rather than queuing
*
*   #30      27 Oct 2005          JTL
* Keeping track of SEIDs separately from endpoint index. Fixes bug where we
* could send the wrong SEID in set config.
*
*   #29      25 Oct 2005          JTL
* Changing default reconfig option back to CLOSE to work with the myriad of
* broken headsets.
* Changing SBC data callback function to not send data on the last frame, but
* set a timer instead. Should lead to better baseband utilization, and not
* depend on AVS setting the 'last_frame' bit.
*
*   #28      14 Oct 2005          JTL
* Disconnect from remote device when commanded to do so.
*
*   #27      11 Oct 2005          JTL
* Fix bug when remote device connects to us. Introduced when removing
* the start delay timer.
*
*   #26      10 Oct 2005          JTL
* Verifying handle IDs for events from AVDTP.
* Do nothing when remote closes transport channel.
* Change default reconfig option to use the reconfigure command.
* Fix reconfig command to only send media caps, and not transport channel cap.
* Added Joint Stereo to list of supported caps.
* Removed start delay.
*
*   #25      14 Sep 2005          JTL
* Reordering state checks in ...cmd_start() so that it correctly handles
* getting a start command while waiting to send the start event.
* Change behavior when remote devices connect to us. Leave the stream state
* open, and request capabilities in the streaming state.
* Add timer to streaming state. If we don't send data for long enough, we
* will suspend the stream.
* Fix aborting and closing, so it expects to get ind followed by cfm.
* Removed automatic reconnections.
*
*   #24      13 Sep 2005          JTL
* Added CONNECTING state. A2DP will no longer attempt a new connection while
* in this state.
* Change DSM allocation to use BT functions.
*
*   #23      02 Sep 2005          JTL
* Fix getting into the wrong state with delayed suspend responses.
*
*   #21-22   29 Aug 2005          JTL
* Fix case where A2DP leaks DSM items if the AVS send data callback is called
* when A2DP isn't in the streaming state.
*
*   #20      17 Aug 2005          JTL
* Set state to CONFIGURING when we RX a close_ind. This allows A2DP
* to set up the stream again later.
*
*   #18      04 Aug 2005          JTL
* Better handling of DSMs in the send data callback.
*
*   #17      21 Jul 2005          JTL
* Added SUSPENDING stream state. Better/more consistent use of states for
* CLOSING. Better checking of current state when processing commands and
* events.
*
*   #16      18 Jul 2005          JTL
* Don't send 2 get-capabilities back to back if we fail to get a capability.
*
*   #14      13 Jul 2005          JTL
* Change the bitrate after sending start event, rather than before.
*
*   #13      13 Jul 2005          JTL
* Added data to start/suspend events to say if they were generated due to
* commands or something else.
* Allow cmd_start to be called with NULL SBC data -- it'll try to use the
* previous data in this case.
* Added lowest bitrate option to the control command.
* Fix bug in non-variable bitrate sound callback so it doesn't send data
* in bigger than L2 MTU chunks.
*
*   #12      05 Jul 2005          JTL
* Use the MIN of the default MTU or remote's MTU.
* Handle acceptor/initiator roles better; timout of acp role if the
* remote doesn't do anything.
* Better handling of remote endpoints and capabilities.
* Various bug fixes.
* Better handling of remote devices connecting to us.
* Better recovery when receiving commands when the last ones haven't
* been processed yet; in particular better error recovery using command
* status events.
* Spec compliant responses to capabilities response; eventually needs
* to be handled by AVS.
* Incrementing AVDTP/RTP frame sequence number by one for each frame.
* Fix bug detecting when to reconfigure streams for new SBC parameters.
* Changed bt_pf_a2dp_cmd_send_data to assume all data fits in one MTU,
* rather than try to split it up. Currently no longer handles message
* fragmentation if one SBC frame does not fit in an MTU.
* Experimental code to handle variable bitrate support; disabled by default.
*
*
*   #12      14 Jul 2005          JH
* Moved bt_pf_avdtp_cmd_send_data() declaration here to avoid compiler warning.
*
*   #11      01 Jun 2005          JTL
* Changes to integrate with released AVS code.
* Fixing issues where we are ACP rather than INT -- this may cause
*   problems with improper INTs, and we may want a timeout to disconnect.
* Fixing minor bugs.
*
*   #9-10    27 May 2005          JTL
* Adding BT command to control some internal A2DP options.
*
*   #8       04 May 2005          JTL
* Update service registration to reflect A2DP.
* Initial implementation of AVS SBC callback.
* Try adding a delay before sending a start event for better
* interop with some headsets.
*
*   #7       20 Apr 2005          JTL
* Fixes for re-opening stream: save the pending SBC info.
* Ignore "in_use" field for endpoints. This should be changed later.
* Other minor changes.
*
*   #6       13 Apr 2005          JTL
* Initialize endpoint data structures.
* Another alternative method for switching streams.
*
*   #1-5     31 Mar 2005          JTL
* Initial revision.
*
===========================================================================*/

/*===========================================================================

INCLUDE FILES FOR MODULE

===========================================================================*/
#include "customer.h"
#include "comdef.h"

#ifdef FEATURE_BT
#ifdef FEATURE_BT_EXTPF_AV

#include "bt.h"
#include "btpfi.h"
#include "btpf.h"
#include "btpfcmdi.h"
#include "bti.h"
#include "btmsg.h"
#include "btsd.h"
#ifndef FEATURE_BT_SOC 
#include "btbb.h"
#endif /* FEATURE_BT_SOC */
#include "snd.h"
#include "dsm.h"
#include "audiosbc.h"
#include "audiosbcenc.h"
#include "snd.h"

#include "oi_codec_sbc.h"

#ifdef FEATURE_BT_TIME_DEBUG
#include "time_svc.h"
#endif /* FEATURE_BT_TIME_DEBUG */
#ifdef FEATURE_BT_AVSYNC_DEBUG
#include "time_svc.h"
#include "clk.h"
#include "qw.h"
#endif
/*===========================================================================

DEFINITIONS AND DECLARATIONS FOR MODULE

This section contains definitions for constants, macros, types, variables
and other items needed by this module.

===========================================================================*/

#define BT_MSG_LAYER                       BT_MSG_GN

/*-------------------------------------------------------------------------*/
/* A2DP Version number used in service registration                        */
/*-------------------------------------------------------------------------*/
#define BT_PF_A2DP_VERSION_NUMBER          0x0100
#define BT_PF_AVDTP_PSM                    0x0019

/*-------------------------------------------------------------------------*/
/* Overhead of A2DP, not including AVDTP                                   */
/*-------------------------------------------------------------------------*/
#define BT_PF_A2DP_HEADER_BYTES            1

/*-------------------------------------------------------------------------*/
/* Maximum MTU that will be used                                           */
/*-------------------------------------------------------------------------*/
#define BT_PF_A2DP_MAX_MTU                 BT_L2_DEFAULT_MTU

/*-------------------------------------------------------------------------*/
/*  This is for an A2DP header                                             */
/*-------------------------------------------------------------------------*/
#define BT_PF_A2DP_HDR_FRAGMENT_OFFSET     0x7
#define BT_PF_A2DP_HDR_START_OFFSET        0x6
#define BT_PF_A2DP_HDR_LAST_OFFSET         0x5
#define BT_PF_A2DP_HDR_RFA_OFFSET          0x4
#define BT_PF_A2DP_HDR_NUM_FRAMES_OFFSET   0x0

/*-------------------------------------------------------------------------*/
/*  RTP payload type to use in the header                                  */
/*-------------------------------------------------------------------------*/
#define BT_PF_A2DP_PAYLOAD_TYPE            0x60

/*-------------------------------------------------------------------------*/
/* Maximum amount of time we'll spend in the ACP state before doing        */
/* something.                                                              */
/*-------------------------------------------------------------------------*/
#define BT_PF_A2DP_ACP_TIMEOUT_MS          3000

/*-------------------------------------------------------------------------*/
/* Maximum amount of time we'll spend in the STREAMING state with no data  */
/* actually being streamed. This is cleared once any data has been         */
/* streamed.                                                               */
/*-------------------------------------------------------------------------*/
#define BT_PF_A2DP_STREAMING_TIMEOUT_MS    2000

/*-------------------------------------------------------------------------*/
/* Maximum amount of time we'll wait for the next SBC frame before sending */
/* out the data already accumulated.                                       */
/*-------------------------------------------------------------------------*/
#define BT_PF_A2DP_PENDING_DATA_TIMEOUT_MS 30

/*-------------------------------------------------------------------------*/
/* Maximum retry attempts of the stream after which A2DP connection will   */
/* be disconnected.                                                        */
/*-------------------------------------------------------------------------*/
#define BT_PF_A2DP_MAX_ABORT_RETRY_ATTEMPTS  3

/*-------------------------------------------------------------------------*/
/* Macro to see if SBC data is valid (not all zeros)                       */
/* Assumes that parameters passed in are word aligned                      */
/*-------------------------------------------------------------------------*/
#define BT_PF_A2DP_SBC_DATA_IS_VALID(x) \
  ( memcmp( (void*)&(x),\
            (void*)&bt_pf_a2dp_null_sbc_data,\
            sizeof( audiosbcenc_data_type ) ) )

/*-------------------------------------------------------------------------*/
/* Feature to enable variable bitrate support.                             */
/*-------------------------------------------------------------------------*/
#undef FEATURE_BT_EXTPF_A2DP_VARIABLE_BITRATE

/*-------------------------------------------------------------------------*/
/* Maximum bit rate for slow uarts                                         */
/*-------------------------------------------------------------------------*/
#define BT_PF_A2DP_MAX_SLOW_UART_BITRATE 230000

/*-------------------------------------------------------------------------*/
/*  Information and queue for events destined for the Profile L2 layer.    */
/*-------------------------------------------------------------------------*/
#define BT_PF_A2DP_NUM_EVENTS  30
LOCAL bt_pf_event_q_info_type  bt_pf_a2dp_ev_q_info;
LOCAL q_type                   bt_pf_a2dp_ev_q;

/*-------------------------------------------------------------------------*/
/* Supported capability                                                    */
/*-------------------------------------------------------------------------*/
LOCAL bt_pf_avdtp_cap_type bt_pf_a2dp_local_cap[2];
LOCAL audiosbcenc_data_type bt_pf_a2dp_local_sbc_data;

/*-------------------------------------------------------------------------*/
/* Null bluetooth address                                                  */
/*-------------------------------------------------------------------------*/
LOCAL const bt_bd_addr_type bt_pf_a2dp_null_bd_addr = BT_PF_BD_ADDR_NULL;

/*-------------------------------------------------------------------------*/
/* Null SBC data                                                           */
/*-------------------------------------------------------------------------*/
const audiosbcenc_data_type bt_pf_a2dp_null_sbc_data =
  { { 0 }, { 0 }, 0, 0 };

/*-------------------------------------------------------------------------*/
/*  Keep track of stream states                                            */
/*-------------------------------------------------------------------------*/
/* Note: These states do not exactly correspond to those in the AVDTP spec */

typedef enum {
  BT_PF_A2DP_SS_IDLE,
  BT_PF_A2DP_SS_CONNECTING,
  BT_PF_A2DP_SS_CONFIGURING,
  BT_PF_A2DP_SS_CONFIG_SENT,
  BT_PF_A2DP_SS_CONFIGURED,
  BT_PF_A2DP_SS_OPEN,
  BT_PF_A2DP_SS_RECONFIG_SENT,
  BT_PF_A2DP_SS_RECONFIGURING,
  BT_PF_A2DP_SS_START_SENT,
  BT_PF_A2DP_SS_STREAMING,
  BT_PF_A2DP_SS_SUSPENDING,
  BT_PF_A2DP_SS_CLOSE_FRM_OPEN,
  BT_PF_A2DP_SS_CLOSE_FRM_STREAMING,
  BT_PF_A2DP_SS_ABORTING
} bt_pf_a2dp_stream_state_type;

typedef enum {
  /* No substate: */
  BT_PF_A2DP_SSS_NONE,

  /* Received a Start from AVS, and are attempting to stream */
  BT_PF_A2DP_SSS_ATTEMPT_TO_STREAM,

  /* Trying to reconfigure the stream with different SBC parameters */
  BT_PF_A2DP_SSS_RECONFIGURING,

  /* Discovering remote endpoints in states other than CONFIGURING */
  BT_PF_A2DP_SSS_DISCOVERING,

  /* Discovering remote endpoints in states other than CONFIGURING,
   * followed by ATTEMPT_TO_STREAM                                  */
  BT_PF_A2DP_SSS_DISC_THEN_STREAM,

  /* Remote initiated close, waiting to tear down L2 connections  */
  BT_PF_A2DP_SSS_REMOTE_CLOSING,

  /* Received a Suspend before the Start command has been acked */
  BT_PF_A2DP_SSS_ATTEMPT_TO_SUSPEND,

  /* Disconnecting */
  BT_PF_A2DP_SSS_ATTEMPT_TO_DISCONNECT,

  /* Disabling */
  BT_PF_A2DP_SSS_ATTEMPT_TO_DISABLE
} bt_pf_a2dp_stream_substate_type;

/*-------------------------------------------------------------------------*/
/*  A2DP Reconfig option                                                   */
/*  Controls behavor of how streams are reconfigured                       */
/*-------------------------------------------------------------------------*/
typedef enum {
  BT_PF_A2DP_RECONFIGURE,
  BT_PF_A2DP_REOPEN_STREAM,
  BT_PF_A2DP_REPAGE_DEVICE
} bt_pf_a2dp_recfg_option_type;

/*-------------------------------------------------------------------------*/
/*  A2DP Role                                                              */
/*-------------------------------------------------------------------------*/
typedef enum {
  BT_PF_A2DP_ROLE_NONE,
  BT_PF_A2DP_ROLE_INT,
  BT_PF_A2DP_ROLE_ACP
} bt_pf_a2dp_role_type;
#ifdef FEATURE_BT_AVSYNC
#define MAX_A2DP_FRAMES           22
bt_pf_a2dp_frame_handle_type      a2dp_free_frame_handle[MAX_A2DP_FRAMES];
q_type                            a2dp_frame_handle_q;
q_type                            a2dp_free_frame_handle_q;
#endif /* FEATURE_BT_AVSYNC */

/*-------------------------------------------------------------------------*/
/*  Keep track of local and remote stream end points                       */
/*-------------------------------------------------------------------------*/

/* This is currently designed to ONLY talk to ONE stream end point, and
 * keep track of only ONE capability within the stream end point. */

typedef struct {
  uint8                           local_seid;
  uint16                          handle;
  bt_pf_avdtp_discover_rsp_info   endpoints[BT_PF_AVDTP_MAX_REMOTE_RECORDS];
  /* current_endpoint: Used only when getting caps */
  int8                            current_endpoint;
  /* current_seid: tracks which SEID we're working with */
  int8                            current_seid;
  uint8                           num_endpoints;
  bt_pf_avdtp_cap_type            cap;
  bt_pf_a2dp_stream_state_type    state;
  bt_pf_a2dp_stream_substate_type substate;
  bt_bd_addr_type                 bd_addr;
  bt_app_id_type                  ext_id;
  bt_app_id_type                  avs_id;
  bt_app_id_type                  int_id;
  audiosbcenc_data_type           sbc_config;
  audiosbcenc_data_type           pending_sbc_config;
  audiosbcenc_data_type           remote_sbc_config;
  uint16                          out_mtu;
  rex_timer_type                  acp_timer;
  rex_timer_type                  streaming_timer;
  rex_timer_type                  pending_data_timer;
  bt_pf_a2dp_role_type            role;
  uint32                          seq_num;
  boolean                         con_ev_sent;

  /* The following parameters are used by the sbc
   * data callback */
  rex_crit_sect_type              audio_crit_sect;
  uint16                          last_frame_len;
  uint32                          last_ts_per_frame;
  uint32                          last_seq_num;
  dsm_item_type                  *dsm_ptr;
  int32                           num_frames;
  uint32                          pcm_timestamp;

  /* The following parameters can be set using
   * bt_cmd_pf_a2dp_ctl to change the behavior of A2DP */
  boolean                         suspend_on_end;
  boolean                         honor_in_use;
  bt_pf_a2dp_recfg_option_type    recfg_option;
  boolean                         auto_bitrate;
  bt_pf_a2dp_bitrate_type         bitrate;
  bt_pf_a2dp_bitrate_type         lowest_bitrate;
  uint32                          custom_bitrate;
  boolean                         app_ctl_start;
  /* This holds the reason for Disconnection */
  uint32                          discon_reason;
  uint8                           a2dp_abort_stream_retries;

} bt_pf_a2dp_unit_type;

LOCAL bt_pf_a2dp_unit_type bt_pf_a2dp_unit;

/*===========================================================================

FUNCTION DECLARATIONS

===========================================================================*/
LOCAL void bt_pf_a2dp_cmd_start( bt_pf_cmd_msg_type *cmd_ptr );
LOCAL void bt_pf_a2dp_cmd_send_data( bt_pf_cmd_msg_type *cmd_ptr );
#ifdef FEATURE_BT_SOC
LOCAL void bt_pf_a2dp_sbc_data_cb( uint8*     data,
                                   uint32     seq_num,
                                   uint32     timestamps_per_frame,
                                   boolean    last_frame,
                                   uint16     frame_len );

LOCAL void bt_pf_a2dp_sbc_init_cb( const void *data, snd_status_type status );
#endif
#ifdef BT_TEST_PYLD_FAST_TX
/* Direct call to AVDTP to send data if FAST_TX is defined */
extern void bt_pf_avdtp_cmd_send_data( struct bt_pf_cmd_msg_struct* );
#endif

/*===========================================================================

INTERNAL FUNCTION DEFINITIONS

===========================================================================*/

/*===========================================================================

FUNCTION
bt_pf_a2dp_close_stream

DESCRIPTION
Called when the A2dp stream needs to be closed and reopened

===========================================================================*/
LOCAL void bt_pf_a2dp_close_stream( bt_pf_a2dp_stream_state_type  state, uint32  reason )
{
  bt_pf_a2dp_unit.discon_reason = reason;
  bt_pf_a2dp_unit.state         = state;
  bt_cmd_pf_avdtp_close( bt_pf_a2dp_unit.int_id,
                         bt_pf_a2dp_unit.handle,
                         NULL );
}
/*===========================================================================

FUNCTION
bt_pf_a2dp_set_substate

DESCRIPTION
Called when the A2dp stream substate is to be updated with new substate 

===========================================================================*/
LOCAL void bt_pf_a2dp_set_substate(bt_pf_a2dp_stream_substate_type  new_substate)
{
  if( new_substate == BT_PF_A2DP_SSS_NONE ) 
  {
    bt_pf_a2dp_unit.substate = BT_PF_A2DP_SSS_NONE;
    bt_pf_a2dp_unit.a2dp_abort_stream_retries = 0;
  }
  else
  {
    bt_pf_a2dp_unit.substate = new_substate; 
  }
}

/*===========================================================================

FUNCTION
bt_pf_a2dp_abort_stream

DESCRIPTION
Called when the A2dp stream needs to aborted

===========================================================================*/
LOCAL void bt_pf_a2dp_abort_stream( void )
{
  BT_MSG_DEBUG( "BT PF A2DP CMD TX: Abort", 0, 0, 0);
  bt_pf_a2dp_unit.state = BT_PF_A2DP_SS_ABORTING;
  /* Incrementing the abort attempt count */
  bt_pf_a2dp_unit.a2dp_abort_stream_retries++ ;
  bt_cmd_pf_avdtp_abort( bt_pf_a2dp_unit.int_id,
                         bt_pf_a2dp_unit.handle,
                         NULL );
}

/*===========================================================================

FUNCTION
bt_pf_a2dp_verify_sbc_data

DESCRIPTION
Called when the remote side attempts to configure us. Check to see if
we support the requested SBC configuration.

===========================================================================*/
LOCAL boolean bt_pf_a2dp_verify_sbc_data( audiosbcenc_data_type *sbc_data )
{
  /* TODO: implement. Verify that the SBC encoder can support this
   * requested configuration.
   * Right now, we accept all configurations, and reconfig the stream
   * when we actually want to start.
   */
  return TRUE;
}

/*===========================================================================

FUNCTION
bt_pf_a2dp_change_bitrate

DESCRIPTION
Called when we want to change the bitrate. Causes a bitrate event to
be sent with the value defined in bt_pf_a2dp_unit.bitrate.

===========================================================================*/
LOCAL void bt_pf_a2dp_change_bitrate( void )
{
  bt_ev_msg_type event;

#ifdef FEATURE_BT_SOC_BITRATE_460800
  int32 lowest_bitrate;

  switch( bt_pf_a2dp_unit.lowest_bitrate )
  {
    case BT_PF_A2DP_BITRATE_LOWEST:
      lowest_bitrate = AUDIOSBC_BITRATE_LOWEST * 2;
      break;
    case BT_PF_A2DP_BITRATE_LOW:
      lowest_bitrate = AUDIOSBC_BITRATE_LOW * 2;
      break;
    case BT_PF_A2DP_BITRATE_MEDIUM:
      lowest_bitrate = AUDIOSBC_BITRATE_MEDIUM * 2;
      break;
    case BT_PF_A2DP_BITRATE_HIGH:
      lowest_bitrate = AUDIOSBC_BITRATE_HIGH * 2;
      break;
    case BT_PF_A2DP_BITRATE_HIGHEST:
      lowest_bitrate = AUDIOSBC_BITRATE_MAX * 2;
      break;
    default:
      lowest_bitrate = AUDIOSBC_BITRATE_LOWEST * 2;
      break;
  }

  if( bt_pf_a2dp_unit.auto_bitrate != FALSE )
  {
    /* For slow uarts, insure we use a slow bitrate if A2DP is allowed to
     * pick bitrates. */
    bt_pf_a2dp_unit.bitrate = BT_PF_A2DP_BITRATE_CUSTOM;
    bt_pf_a2dp_unit.custom_bitrate = 
      MAX( BT_PF_A2DP_MAX_SLOW_UART_BITRATE, lowest_bitrate );
  }
#endif /* FEATURE_BT_SOC_BITRATE_460800 */

  event.ev_hdr.bt_app_id = bt_pf_a2dp_unit.ext_id;
  event.ev_hdr.ev_type   = BT_EV_PF_A2DP_BITRATE;
  event.ev_msg.ev_a2dp_bitrate.bitrate_hint   = bt_pf_a2dp_unit.bitrate;
  event.ev_msg.ev_a2dp_bitrate.custom_bitrate = bt_pf_a2dp_unit.custom_bitrate;
  event.ev_msg.ev_a2dp_bitrate.bd_addr = bt_pf_a2dp_unit.bd_addr;

  BT_MSG_DEBUG("BT PF A2DP TX EV: bitrate. AppID: %x, Rate %x, Custom Rate %x",
               bt_pf_a2dp_unit.ext_id,
               bt_pf_a2dp_unit.bitrate,
               bt_pf_a2dp_unit.custom_bitrate);
  bt_ec_send_event( &event );

  // Also call the sound interface directly
  snd_sbc_set_bitrate( (snd_sbc_bitrate_type)bt_pf_a2dp_unit.bitrate,
                       bt_pf_a2dp_unit.custom_bitrate,
                       NULL, NULL );
}

/*===========================================================================

FUNCTION
bt_pf_a2dp_end_streaming

DESCRIPTION
Called when we may no be longer streaming, ie, when we might leave
BT_PF_A2DP_SS_STREAMING

===========================================================================*/
LOCAL void bt_pf_a2dp_end_streaming( bt_pf_a2dp_event_type origin )
{
  rex_clr_timer( &bt_pf_a2dp_unit.streaming_timer );
  rex_clr_timer( &bt_pf_a2dp_unit.pending_data_timer );

  if( bt_pf_a2dp_unit.state    == BT_PF_A2DP_SS_STREAMING           ||
      bt_pf_a2dp_unit.state    == BT_PF_A2DP_SS_SUSPENDING          ||
      bt_pf_a2dp_unit.state    == BT_PF_A2DP_SS_CLOSE_FRM_STREAMING ||
      bt_pf_a2dp_unit.substate == BT_PF_A2DP_SSS_ATTEMPT_TO_SUSPEND ||
      bt_pf_a2dp_unit.substate == BT_PF_A2DP_SSS_ATTEMPT_TO_DISABLE ||
      bt_pf_a2dp_unit.substate == BT_PF_A2DP_SSS_ATTEMPT_TO_DISCONNECT )
  {
    bt_ev_msg_type event;

    // Clear out data from previous audio
    rex_enter_crit_sect( &bt_pf_a2dp_unit.audio_crit_sect );
    dsm_free_packet( &bt_pf_a2dp_unit.dsm_ptr );
    bt_pf_a2dp_unit.num_frames     = 0;
    bt_pf_a2dp_unit.last_frame_len = 0;
    bt_pf_a2dp_unit.pcm_timestamp  = 0;
    rex_leave_crit_sect( &bt_pf_a2dp_unit.audio_crit_sect );

    // Send a suspend event
    event.ev_hdr.bt_app_id               = bt_pf_a2dp_unit.ext_id;
    event.ev_hdr.ev_type                 = BT_EV_PF_A2DP_SUSPEND;
    event.ev_msg.ev_a2dp_suspend.origin  = origin;
    event.ev_msg.ev_a2dp_suspend.bd_addr = bt_pf_a2dp_unit.bd_addr;

    BT_MSG_DEBUG("BT PF A2DP TX EV: suspend. AppID: %x, origin: %x",
                 bt_pf_a2dp_unit.ext_id, origin, 0);
    bt_ec_send_event( &event );
  }
  else
  {
    BT_MSG_DEBUG("BT PF A2DP: not sending suspend event. state: %x substate: %x",
                 bt_pf_a2dp_unit.state,
                 bt_pf_a2dp_unit.substate,
                 0);
  }

  if( bt_pf_a2dp_unit.substate == BT_PF_A2DP_SSS_ATTEMPT_TO_SUSPEND )
  {
    bt_pf_a2dp_set_substate ( BT_PF_A2DP_SSS_NONE );
  }

}

/*===========================================================================

FUNCTION
bt_pf_a2dp_start_streaming

DESCRIPTION
Called when we start streaming, ie, when we can enter
BT_PF_A2DP_SS_STREAMING

===========================================================================*/
LOCAL void bt_pf_a2dp_start_streaming( bt_pf_a2dp_event_type origin )
{
  bt_ev_msg_type event;

  if( bt_pf_a2dp_unit.state == BT_PF_A2DP_SS_OPEN       ||
      bt_pf_a2dp_unit.state == BT_PF_A2DP_SS_START_SENT ||
      bt_pf_a2dp_unit.state == BT_PF_A2DP_SS_STREAMING )
  {
    // Set the state to streaming
    bt_pf_a2dp_unit.state = BT_PF_A2DP_SS_STREAMING;
    if( bt_pf_a2dp_unit.substate == BT_PF_A2DP_SSS_ATTEMPT_TO_STREAM )
    {
      bt_pf_a2dp_set_substate( BT_PF_A2DP_SSS_NONE );
    }

    // Send the Start event to the avs_id and the ext_id
    event.ev_hdr.ev_type   = BT_EV_PF_A2DP_START;
    event.ev_msg.ev_a2dp_start.origin = origin;
    event.ev_hdr.bt_app_id = bt_pf_a2dp_unit.ext_id;
    event.ev_msg.ev_a2dp_start.bd_addr = bt_pf_a2dp_unit.bd_addr;
    BT_MSG_DEBUG("BT PF A2DP TX EV: start. AppID: %x origin %x",
                 event.ev_hdr.bt_app_id, origin, 0);
    bt_ec_send_event( &event );
    if( bt_pf_a2dp_unit.avs_id != BT_APP_ID_NULL )
    {
      event.ev_hdr.bt_app_id = bt_pf_a2dp_unit.avs_id;
      BT_MSG_DEBUG("BT PF A2DP TX EV: start. AppID: %x origin %x",
                   event.ev_hdr.bt_app_id, origin, 0);
      bt_ec_send_event( &event );
    }

    // Set the bitrate
    bt_pf_a2dp_change_bitrate();
  }
  else
  {
    BT_ERR("BT PF A2DP: Attempt to start streaming when not open",0,0,0);
  }
}

/*===========================================================================

FUNCTION
bt_pf_a2dp_send_close_ev

DESCRIPTION
Send the close event

===========================================================================*/
void bt_pf_a2dp_send_close_ev( bt_pf_a2dp_event_type origin )
{
  bt_ev_msg_type event;

  event.ev_hdr.bt_app_id = bt_pf_a2dp_unit.ext_id;
  event.ev_hdr.ev_type   = BT_EV_PF_A2DP_CLOSE;
  event.ev_msg.ev_a2dp_close.origin = origin;
  event.ev_msg.ev_a2dp_close.bd_addr = bt_pf_a2dp_unit.bd_addr;
  
  BT_MSG_DEBUG("BT PF A2DP TX EV: close. AppID: %x, Init %x",
               bt_pf_a2dp_unit.ext_id,
               event.ev_msg.ev_a2dp_close.origin,0);
  bt_ec_send_event( &event );
}

/*===========================================================================

FUNCTION
bt_pf_a2dp_streaming_timeout_cb

DESCRIPTION
Called if we remain in the streaming state without streaming any data
for BT_PF_A2DP_STREAMING_TIMEOUT_MS

===========================================================================*/
LOCAL void bt_pf_a2dp_streaming_timeout_cb( unsigned long unused )
{
  if( bt_pf_a2dp_unit.state == BT_PF_A2DP_SS_STREAMING )
  {
    BT_MSG_DEBUG("BT PF A2DP: No data in streaming state. Suspending",0,0,0);
    bt_pf_a2dp_unit.state = BT_PF_A2DP_SS_SUSPENDING;
    bt_cmd_pf_avdtp_suspend( bt_pf_a2dp_unit.int_id,
                             bt_pf_a2dp_unit.handle,
                             NULL );
  }
  else
  {
    BT_ERR("BT PF A2DP: streaming timeout cb called in wrong state %x",
           bt_pf_a2dp_unit.state, 0, 0);
  }
}

/*===========================================================================

FUNCTION
bt_pf_a2dp_acp_timeout_cb

DESCRIPTION
Called if we remain in the acceptor state for BT_PF_A2DP_ACP_TIMEOUT_MS.

===========================================================================*/
LOCAL void bt_pf_a2dp_acp_timeout_cb( unsigned long unused )
{
  BT_MSG_HIGH("BT PF A2DP: ACP Timeout expired",0,0,0);
  if( bt_pf_a2dp_unit.role == BT_PF_A2DP_ROLE_ACP )
  {
    /* If we're in the open state, just go ahead with discovery, */
    if( bt_pf_a2dp_unit.state == BT_PF_A2DP_SS_OPEN )
    {
      BT_MSG_DEBUG("BT PF A2DP CMD TX: discover",0,0,0);
      if( bt_pf_a2dp_unit.substate == BT_PF_A2DP_SSS_ATTEMPT_TO_STREAM )
      {
        bt_pf_a2dp_set_substate( BT_PF_A2DP_SSS_DISCOVERING );
      }
      else
      {
        bt_pf_a2dp_set_substate( BT_PF_A2DP_SSS_DISC_THEN_STREAM );
      }
      bt_cmd_pf_avdtp_discover( bt_pf_a2dp_unit.int_id,
                                &bt_pf_a2dp_unit.bd_addr, 
                                NULL );
    }
    else /* We haven't even made it to open yet. Abort */
    {
      bt_pf_a2dp_abort_stream();
    }
  }
  else
  {
    BT_ERR("BT PF A2DP: ACP timeout when not ACP",0,0,0);
  }
}

/*===========================================================================

FUNCTION
bt_pf_a2dp_pending_data_timeout_cb

DESCRIPTION
Called if we retain SBC data for BT_PF_A2DP_STREAMING_TIMEOUT_MS without
receiving new SBC frames from AVS.
Note: this runs in the task context of the AVS task sending SBC frames.

===========================================================================*/
LOCAL void bt_pf_a2dp_pending_data_timeout_cb( unsigned long called_reason )
{
  bt_cmd_pf_a2dp_flush( bt_pf_a2dp_unit.int_id );
}

/*===========================================================================

FUNCTION
bt_pf_a2dp_try_next_endpoint

DESCRIPTION
Gets capabilities for the next remote endpoint. Disconnects if no
more endpoints available.

===========================================================================*/
void bt_pf_a2dp_try_next_endpoint( void )
{
  boolean found_record = FALSE;
  int i;

  for( i = bt_pf_a2dp_unit.current_endpoint+1;
       (i < bt_pf_a2dp_unit.num_endpoints) && (!found_record);
       i++ )
  {
    if( bt_pf_a2dp_unit.endpoints[i].media_type == BT_PF_AVDTP_MEDIA_TYPE_AUDIO &&
        bt_pf_a2dp_unit.endpoints[i].endpoint_type == BT_PF_AVDTP_SNK           &&
        ( bt_pf_a2dp_unit.endpoints[i].in_use == FALSE   ||
          bt_pf_a2dp_unit.honor_in_use == FALSE          ||
          bt_pf_a2dp_unit.state != BT_PF_A2DP_SS_CONFIGURING ) )
    {
      if( found_record == FALSE )
      {
        found_record = TRUE;
        bt_pf_a2dp_unit.current_endpoint = i;
        bt_pf_a2dp_unit.current_seid     = bt_pf_a2dp_unit.endpoints[i].seid;
        BT_MSG_DEBUG("BT PF A2DP CMD TX: Get cap seid %x",
                     bt_pf_a2dp_unit.endpoints[i].seid, 0, 0);
        bt_cmd_pf_avdtp_get_cap( bt_pf_a2dp_unit.int_id,
                                 &bt_pf_a2dp_unit.bd_addr,
                                 bt_pf_a2dp_unit.endpoints[i].seid,
                                 NULL );
      }
    }
  }
  if( found_record == FALSE )
  {
    BT_MSG_HIGH("BT PF A2DP: no usable remote end points; disconnecting",
                0,0,0);
    // There were no compatible remote endpoints
    bt_pf_a2dp_unit.discon_reason = BT_CS_PF_A2DP_ERR_INCOMPATIABLE_DEVICES;
    bt_pf_a2dp_set_substate( BT_PF_A2DP_SSS_NONE );
    bt_cmd_pf_a2dp_disconnect( bt_pf_a2dp_unit.int_id );
  }
}

/*===========================================================================

AVDTP EVENT HANDLERS
These functions handle AVDTP events

===========================================================================*/
/*===========================================================================

FUNCTION
bt_pf_a2dp_ev_pf_avdtp_con_ind

DESCRIPTION
Handles event from AVDTP.

===========================================================================*/
LOCAL void bt_pf_a2dp_ev_pf_avdtp_con_ind( bt_ev_pf_avdtp_con_ind_type *ev )
{
  BT_MSG_DEBUG("BT PF A2DP EV RX: con cfm ss|sss %08x",
               (bt_pf_a2dp_unit.state << 16) | bt_pf_a2dp_unit.substate,
               0,0);

  if( bt_pf_a2dp_unit.state == BT_PF_A2DP_SS_IDLE )
  {
    // If we're idle, accept this connection.
    bt_pf_a2dp_unit.bd_addr = ev->bd_addr;

    //BT_BDA( MSG_HIGH, "BT PF A2DP: Incoming conneciton",
     //       &bt_pf_a2dp_unit.bd_addr );

    bt_pf_a2dp_unit.state = BT_PF_A2DP_SS_CONNECTING;
    bt_cmd_pf_avdtp_accept( bt_pf_a2dp_unit.int_id,
                            &ev->bd_addr, TRUE );

    bt_pf_a2dp_unit.role = BT_PF_A2DP_ROLE_ACP;
    rex_set_timer( &bt_pf_a2dp_unit.acp_timer,
                   BT_PF_A2DP_ACP_TIMEOUT_MS );

  }
  else
  {
    // If we're already connected to someone, don't accept.
    BT_MSG_DEBUG("BT PF A2DP: already connected. Rejecting",0,0,0);
    bt_cmd_pf_avdtp_accept( bt_pf_a2dp_unit.int_id,
                            &ev->bd_addr, FALSE );
  }
}

/*===========================================================================

FUNCTION
bt_pf_a2dp_ev_pf_avdtp_con_cfm

DESCRIPTION
Handles event from AVDTP.

===========================================================================*/
LOCAL void bt_pf_a2dp_ev_pf_avdtp_con_cfm( bt_ev_pf_avdtp_con_cfm_type *ev )
{
  bt_ev_msg_type event;

  BT_MSG_DEBUG("BT PF A2DP EV RX: con cfm ss|sss %08x",
               (bt_pf_a2dp_unit.state << 16) | bt_pf_a2dp_unit.substate,
               0,0);

  if( ev->result == BT_CS_GN_SUCCESS )
  {
    bt_pf_a2dp_unit.bd_addr = ev->bd_addr;

    // Each time we connect to a new device, reset the bitrate
    // selection -- assuming we have a good link.
    bt_pf_a2dp_unit.bitrate = BT_PF_A2DP_BITRATE_HIGH;
    bt_pf_a2dp_unit.auto_bitrate = TRUE;

    if( bt_pf_a2dp_unit.substate == BT_PF_A2DP_SSS_ATTEMPT_TO_DISABLE ||
        bt_pf_a2dp_unit.substate == BT_PF_A2DP_SSS_ATTEMPT_TO_DISCONNECT )
    {
      bt_cmd_pf_a2dp_disconnect( bt_pf_a2dp_unit.int_id );
    }
    if( bt_pf_a2dp_unit.role != BT_PF_A2DP_ROLE_ACP )
    {
      // Start the configuring process
      bt_pf_a2dp_unit.state = BT_PF_A2DP_SS_CONFIGURING;
      bt_cmd_pf_avdtp_discover( bt_pf_a2dp_unit.int_id,
                                &ev->bd_addr, NULL );
    }
  }
  else
  {
    // There was some connection problem. Let the upper layer know.
    bt_pf_a2dp_unit.state    = BT_PF_A2DP_SS_IDLE;
    bt_pf_a2dp_set_substate( BT_PF_A2DP_SSS_NONE );
    event.ev_hdr.bt_app_id = bt_pf_a2dp_unit.ext_id;
    event.ev_hdr.ev_type = BT_EV_PF_A2DP_CON_FAILED;
    event.ev_msg.ev_a2dp_con_failed.bd_addr = ev->bd_addr;
    event.ev_msg.ev_a2dp_con_failed.reason  = ev->result;
    
    BT_MSG_DEBUG("BT PF A2DP TX EV: con failed. AppID: %x",
                  bt_pf_a2dp_unit.ext_id,0,0);

    bt_ec_send_event( &event );
  }
}

/*===========================================================================

FUNCTION
bt_pf_a2dp_ev_pf_avdtp_discon_ind

DESCRIPTION
Handles event from AVDTP.

===========================================================================*/
LOCAL void bt_pf_a2dp_ev_pf_avdtp_discon_ind( bt_ev_pf_avdtp_discon_ind_type *ev )
{
  bt_ev_msg_type event;
#ifdef FEATURE_BT_AVSYNC
  bt_pf_a2dp_frame_handle_type *a2dp_frame_handle;
#endif /* FEATURE_BT_AVSYNC */
  BT_MSG_DEBUG("BT PF A2DP EV RX: discon ind ss|sss %08x",
               (bt_pf_a2dp_unit.state << 16) | bt_pf_a2dp_unit.substate,
               0,0);

  if( !BT_BD_ADDRS_EQUAL( &ev->bd_addr, &bt_pf_a2dp_unit.bd_addr ) )
  {
    // Disconnected from another device
    return;
  }

  bt_pf_a2dp_end_streaming( BT_PF_A2DP_EVENT_REMOTE_INIT );

  // Keep track of the correct states, and clear out old information:
  memset( (void*)&bt_pf_a2dp_unit.endpoints, 0,
          ( sizeof( bt_pf_avdtp_discover_rsp_info )
            * BT_PF_AVDTP_MAX_REMOTE_RECORDS ) );
  memset( (void*)&bt_pf_a2dp_unit.cap, 0,
          sizeof( bt_pf_avdtp_cap_type ) );
  memset( (void*)&bt_pf_a2dp_unit.pending_sbc_config, 0,
          sizeof( audiosbcenc_data_type ) );
  memset( (void*)&bt_pf_a2dp_unit.sbc_config, 0,
          sizeof( audiosbcenc_data_type ) );
  bt_pf_a2dp_unit.state            = BT_PF_A2DP_SS_IDLE;
  bt_pf_a2dp_unit.handle           = 0;
  bt_pf_a2dp_unit.current_endpoint = -1;
  bt_pf_a2dp_unit.current_seid     = -1;
  bt_pf_a2dp_unit.num_endpoints    = 0;
  bt_pf_a2dp_unit.substate         = BT_PF_A2DP_SSS_NONE;
  bt_pf_a2dp_unit.out_mtu          = 0;
  bt_pf_a2dp_unit.role             = BT_PF_A2DP_ROLE_NONE;
  bt_pf_a2dp_unit.seq_num          = 0;
  bt_pf_a2dp_unit.con_ev_sent      = FALSE;
  bt_pf_a2dp_unit.a2dp_abort_stream_retries  = 0;
  rex_clr_timer( &bt_pf_a2dp_unit.acp_timer );
  rex_clr_timer( &bt_pf_a2dp_unit.streaming_timer );

#ifdef FEATURE_BT_AVSYNC
  /* Free up the queued a2dp frames */
  while((a2dp_frame_handle = q_get(&a2dp_frame_handle_q)) != NULL)
  {
    q_put(&a2dp_free_frame_handle_q, &a2dp_frame_handle->link);
  }
#endif /* FEATURE_BT_AVSYNC */
  // Let the upper layer know.
  event.ev_hdr.bt_app_id = bt_pf_a2dp_unit.ext_id;
  event.ev_hdr.ev_type = BT_EV_PF_A2DP_DISCON;
  event.ev_msg.ev_a2dp_discon.bd_addr = ev->bd_addr;

  if(bt_pf_a2dp_unit.discon_reason != BT_CS_PF_A2DP_ERR_A2DP_NONE)
  {
   event.ev_msg.ev_a2dp_discon.reason  
    = (bt_cmd_status_type)bt_pf_a2dp_unit.discon_reason;
   bt_pf_a2dp_unit.discon_reason       = BT_CS_PF_A2DP_ERR_A2DP_NONE;
  }
  else
  {
   event.ev_msg.ev_a2dp_discon.reason  = ev->reason;
  }

  BT_MSG_DEBUG("BT PF A2DP TX EV: discon. AppID: %x reason: %x",
               bt_pf_a2dp_unit.ext_id,
               event.ev_msg.ev_a2dp_discon.reason,
               0);

  bt_ec_send_event( &event );

  if( bt_pf_a2dp_unit.substate == BT_PF_A2DP_SSS_ATTEMPT_TO_DISABLE )
  {
    bt_cmd_pf_a2dp_disable( bt_pf_a2dp_unit.int_id );
  }
}

/*===========================================================================

FUNCTION
bt_pf_a2dp_ev_pf_avdtp_disc_cfm

DESCRIPTION
Handles event from AVDTP.

===========================================================================*/
LOCAL void bt_pf_a2dp_ev_pf_avdtp_disc_cfm( bt_ev_pf_avdtp_disc_cfm_type *ev )
{
  int i;

  BT_MSG_DEBUG("BT PF A2DP EV RX: discover cfm ss|sss %08x",
               (bt_pf_a2dp_unit.state << 16) | bt_pf_a2dp_unit.substate,
               0,0);
  if( ev->error == BT_CS_GN_SUCCESS )
  {
    if( bt_pf_a2dp_unit.state    == BT_PF_A2DP_SS_CONFIGURING  ||
        bt_pf_a2dp_unit.substate == BT_PF_A2DP_SSS_DISCOVERING ||
        bt_pf_a2dp_unit.substate == BT_PF_A2DP_SSS_DISC_THEN_STREAM )
    {
      /* Copy the discovery records for later use */
      for( i = 0;
           i < ev->num_records * sizeof(bt_pf_avdtp_discover_rsp_info);
           i++ )
      {
        ((uint8*)bt_pf_a2dp_unit.endpoints)[i] = ((uint8*)ev->records)[i];
      }
      /* Start looking at endpoints: */
      bt_pf_a2dp_unit.num_endpoints = ev->num_records;
      bt_pf_a2dp_unit.current_endpoint = -1;
      bt_pf_a2dp_unit.current_seid     = -1;
      bt_pf_a2dp_try_next_endpoint();
    }
    else
    {
      BT_MSG_HIGH("BT PF A2DP: disc cfm in unexpected state %x",
                  bt_pf_a2dp_unit.state, 0, 0);
    }
  }
  else
  {
    BT_ERR("BT PF A2DP: Error %x with discovery. Disconnecting",
           ev->error, 0, 0);
    bt_pf_a2dp_unit.discon_reason = BT_CS_PF_A2DP_ERR_ENDPOINT_DISCOVERY;
    bt_pf_a2dp_set_substate( BT_PF_A2DP_SSS_NONE );
    bt_cmd_pf_a2dp_disconnect( bt_pf_a2dp_unit.int_id );
  }
}

/*===========================================================================

FUNCTION
bt_pf_a2dp_ev_pf_avdtp_get_cap_cfm

DESCRIPTION
Handles event from AVDTP.

===========================================================================*/
LOCAL void bt_pf_a2dp_ev_pf_avdtp_get_cap_cfm( bt_ev_pf_avdtp_get_cap_cfm_type *ev )
{
  int i;
  int found_cap = -1;

  BT_MSG_DEBUG("BT PF A2DP EV RX: get cap cfm ss|sss %08x",
               (bt_pf_a2dp_unit.state << 16) | bt_pf_a2dp_unit.substate,
               0,0);

  if( ev->error != BT_CS_GN_SUCCESS )
  {
    BT_ERR("BT PF A2DP: Error %x getting remote caps. Trying next endpoint",
           ev->error, 0, 0);
  }
  else if( bt_pf_a2dp_unit.state    != BT_PF_A2DP_SS_CONFIGURING  &&
           bt_pf_a2dp_unit.substate != BT_PF_A2DP_SSS_DISCOVERING &&
           bt_pf_a2dp_unit.substate != BT_PF_A2DP_SSS_DISC_THEN_STREAM )
  {
    BT_MSG_HIGH("BT PF A2DP: get cap cfm in unexpected state %x",
                bt_pf_a2dp_unit.state, 0, 0);
  }
  else
  {
    for( i = 0; i < ev->num_caps; i++ )
    {
      if( ev->caps[i].type == BT_PF_AVDTP_CAP_CODEC                              &&
          ev->caps[i].data.codec_data.media_type == BT_PF_AVDTP_MEDIA_TYPE_AUDIO &&
          ev->caps[i].data.codec_data.codec_type == BT_PF_AVDTP_CODEC_SBC )
      {
        if( found_cap == -1 )
        {
          found_cap = i;
          bt_pf_a2dp_unit.cap = ev->caps[i];
        }
        else
        {
          BT_MSG_DEBUG("BT PF A2DP: too many avalable caps, using first",
                       0,0,0);
        }
      }
    }
  }

  if( found_cap != -1 &&
      ( bt_pf_a2dp_unit.state    == BT_PF_A2DP_SS_CONFIGURING       ||
        bt_pf_a2dp_unit.substate == BT_PF_A2DP_SSS_DISCOVERING      ||
        bt_pf_a2dp_unit.substate == BT_PF_A2DP_SSS_DISC_THEN_STREAM ) )
  {
    bt_ev_msg_type event;

    if( bt_pf_a2dp_unit.con_ev_sent == FALSE )
    {
      // Send a connected event with the sbc data to both the ext_id and avs_id
      event.ev_hdr.bt_app_id = bt_pf_a2dp_unit.ext_id;
      event.ev_hdr.ev_type   = BT_EV_PF_A2DP_CON;
      event.ev_msg.ev_a2dp_con.bd_addr    = ev->bd_addr;
      event.ev_msg.ev_a2dp_con.sbc_config =
        ev->caps[found_cap].data.codec_data.data.sbc_data;
      bt_pf_a2dp_unit.remote_sbc_config =
        ev->caps[found_cap].data.codec_data.data.sbc_data;
      BT_MSG_DEBUG("BT PF A2DP TX EV: con. AppID: %x",
                   bt_pf_a2dp_unit.ext_id,0,0);
      bt_ec_send_event( &event );
      bt_pf_a2dp_unit.con_ev_sent = TRUE;
      if( bt_pf_a2dp_unit.avs_id != BT_APP_ID_NULL )
      {
        event.ev_hdr.bt_app_id = bt_pf_a2dp_unit.avs_id;
        BT_MSG_DEBUG("BT PF A2DP TX EV: con. AppID: %x",
                     bt_pf_a2dp_unit.ext_id,0,0);
        bt_ec_send_event( &event );
      }
    }

    if( bt_pf_a2dp_unit.substate == BT_PF_A2DP_SSS_ATTEMPT_TO_STREAM ||
        bt_pf_a2dp_unit.substate == BT_PF_A2DP_SSS_DISC_THEN_STREAM )
    {
      // Trying to open an endpoint. Use the pending config
      // here to configure the endpoint.
      bt_pf_cmd_msg_type cmd_msg;
      bt_cmd_pf_a2dp_start_type *cmd_ptr = &cmd_msg.cmd_msg.cmd_a2dp_start;

      bt_pf_a2dp_set_substate( BT_PF_A2DP_SSS_NONE );
      cmd_msg.cmd_hdr.app_id = bt_pf_a2dp_unit.int_id;
      cmd_ptr->sbc_data = bt_pf_a2dp_null_sbc_data;
      bt_pf_a2dp_cmd_start( &cmd_msg );
    }
    else if ( bt_pf_a2dp_unit.substate == BT_PF_A2DP_SSS_ATTEMPT_TO_DISABLE ||
              bt_pf_a2dp_unit.substate == BT_PF_A2DP_SSS_ATTEMPT_TO_DISCONNECT )
    {
      bt_cmd_pf_a2dp_disconnect( bt_pf_a2dp_unit.int_id );
    }
    else
    {
      bt_pf_a2dp_set_substate( BT_PF_A2DP_SSS_NONE );
    }
  }
  else if ( bt_pf_a2dp_unit.state    == BT_PF_A2DP_SS_CONFIGURING       ||
            bt_pf_a2dp_unit.substate == BT_PF_A2DP_SSS_DISCOVERING      ||
            bt_pf_a2dp_unit.substate == BT_PF_A2DP_SSS_DISC_THEN_STREAM )
  {
    bt_pf_a2dp_try_next_endpoint();
  }
}

/*===========================================================================

FUNCTION
bt_pf_a2dp_ev_pf_avdtp_set_cfg_ind

DESCRIPTION
Handles event from AVDTP.

===========================================================================*/
LOCAL void bt_pf_a2dp_ev_pf_avdtp_set_cfg_ind( bt_ev_pf_avdtp_set_cfg_ind_type *ev )
{
  BT_MSG_DEBUG("BT PF A2DP EV RX: set cfg ind hndl %x ss|sss %08x",
               ev->handle,
               (bt_pf_a2dp_unit.state << 16) | bt_pf_a2dp_unit.substate,
               0);

  if( !BT_BD_ADDRS_EQUAL( &ev->bd_addr, &bt_pf_a2dp_unit.bd_addr ) )
  {
    // Set config from another device
    //BT_BDA( ERR, "BT PF A2DP: set cfg wrong bda:", &ev->bd_addr );
    return;
  }

  if( bt_pf_a2dp_unit.state != BT_PF_A2DP_SS_CONNECTING  &&
      bt_pf_a2dp_unit.state != BT_PF_A2DP_SS_CONFIGURING &&
      bt_pf_a2dp_unit.state != BT_PF_A2DP_SS_CONFIGURED )
  {
    // This is an invalid state to rx a set configuration.
    BT_ERR("BT PF A2DP: set cfg in bad state %x",
           bt_pf_a2dp_unit.state, 0, 0);
    // Respond to the config
    bt_cmd_pf_avdtp_set_config_rsp( bt_pf_a2dp_unit.int_id,
                                    ev->handle,
                                    ev->txn,
                                    BT_PF_AVDTP_CAP_CODEC,
                                    BT_CS_PF_AVDTP_ERR_STATE );
  }
  else if( ev->local_seid == bt_pf_a2dp_unit.local_seid                           &&
           ev->num_caps == 2                                                      &&
           ev->caps[0].type == BT_PF_AVDTP_CAP_MEDIA_TRANSPORT                    &&
           ev->caps[1].type == BT_PF_AVDTP_CAP_CODEC                              &&
           ev->caps[1].data.codec_data.media_type == BT_PF_AVDTP_MEDIA_TYPE_AUDIO &&
           ev->caps[1].data.codec_data.codec_type == BT_PF_AVDTP_CODEC_SBC        &&
           bt_pf_a2dp_verify_sbc_data( &ev->caps[1].data.codec_data.data.sbc_data ) )
  {
    // Set the state to configured
    bt_pf_a2dp_unit.state = BT_PF_A2DP_SS_CONFIGURED;

    BT_MSG_HIGH( "BT PF A2DP: Remote configured media stream",
                 0, 0, 0);

    if( bt_pf_a2dp_unit.role == BT_PF_A2DP_ROLE_ACP )
    {
      rex_set_timer( &bt_pf_a2dp_unit.acp_timer,
                     BT_PF_A2DP_ACP_TIMEOUT_MS );
    }

    // Save the info
    bt_pf_a2dp_unit.handle           = ev->handle;
    bt_pf_a2dp_unit.current_seid     = ev->remote_seid;
    bt_pf_a2dp_unit.sbc_config       = ev->caps[1].data.codec_data.data.sbc_data;

    // Tell AVS the current config
    audiosbc_set_enc_params( (audiosbcenc_data_type*)&bt_pf_a2dp_unit.sbc_config );

    // Respond to the config
    bt_cmd_pf_avdtp_set_config_rsp( bt_pf_a2dp_unit.int_id,
                                    ev->handle,
                                    ev->txn,
                                    0,
                                    BT_CS_GN_SUCCESS );

    if( bt_pf_a2dp_unit.substate == BT_PF_A2DP_SSS_ATTEMPT_TO_SUSPEND )
    {
      bt_pf_a2dp_end_streaming( BT_PF_A2DP_EVENT_LOCAL_INIT );
    }
  }
  else
  {
    BT_ERR("BT PF A2DP: bad config. num caps: %x, local_seid: %x, message_seid: %x",
           ev->num_caps,
           bt_pf_a2dp_unit.local_seid,
           ev->local_seid );
    BT_ERR("BT PF A2DP: type %x, media_type %x, codec_type %x",
           ev->caps[1].type,
           ev->caps[1].data.codec_data.media_type,
           ev->caps[1].data.codec_data.codec_type );
    // Respond to the config
    bt_cmd_pf_avdtp_set_config_rsp( bt_pf_a2dp_unit.int_id,
                                    ev->handle,
                                    ev->txn,
                                    BT_PF_AVDTP_CAP_CODEC,
                                    BT_CS_PF_AVDTP_ERR_UNSUPPORTED_CONFIG );
  }
}

/*===========================================================================

FUNCTION
bt_pf_a2dp_ev_pf_avdtp_set_cfg_cfm

DESCRIPTION
Handles event from AVDTP.

===========================================================================*/
LOCAL void bt_pf_a2dp_ev_pf_avdtp_set_cfg_cfm( bt_ev_pf_avdtp_set_cfg_cfm_type *ev )
{
  BT_MSG_DEBUG("BT PF A2DP EV RX: set cfg cfm hndl %x err %x ss|sss %08x",
               ev->handle, ev->error,
               (bt_pf_a2dp_unit.state << 16) | bt_pf_a2dp_unit.substate );

  if( (bt_pf_a2dp_unit.state != BT_PF_A2DP_SS_CONFIG_SENT) &&
      (bt_pf_a2dp_unit.state != BT_PF_A2DP_SS_CONFIGURING) )
  {
    BT_MSG_HIGH("BT PF A2DP: set cfg cfm in unexpected state %x",
                bt_pf_a2dp_unit.state, 0, 0);
  }
  else if( ev->error == BT_CS_GN_SUCCESS )
  {
    if( bt_pf_a2dp_unit.state == BT_PF_A2DP_SS_CONFIG_SENT )
    {
      // Record the current sbc config
      bt_pf_a2dp_unit.sbc_config = bt_pf_a2dp_unit.pending_sbc_config;
    }

    bt_pf_a2dp_unit.state       = BT_PF_A2DP_SS_CONFIGURED;
    bt_pf_a2dp_unit.handle      = ev->handle;
    bt_cmd_pf_avdtp_open( bt_pf_a2dp_unit.int_id,
                          ev->handle,
                          NULL );
  }
  else
  {
    bt_pf_a2dp_unit.discon_reason = BT_CS_PF_A2DP_ERR_SET_CONFIG_FAILED;
    BT_ERR("BT PF A2DP: set cfg err %x; disconnecting", ev->error,0,0);
    bt_pf_a2dp_set_substate( BT_PF_A2DP_SSS_NONE );
    bt_cmd_pf_a2dp_disconnect( bt_pf_a2dp_unit.int_id );
  }
}

/*===========================================================================

FUNCTION
bt_pf_a2dp_ev_pf_avdtp_get_cfg_cfm

DESCRIPTION
Handles event from AVDTP.

===========================================================================*/
LOCAL void bt_pf_a2dp_ev_pf_avdtp_get_cfg_cfm( bt_ev_pf_avdtp_get_cfg_cfm_type *ev )
{
  BT_ERR("BT PF A2DP EV RX get cfg cfm: Not Handled",0,0,0);
}

/*===========================================================================

FUNCTION
bt_pf_a2dp_ev_pf_avdtp_open_cfm

DESCRIPTION
Handles event from AVDTP.

===========================================================================*/
LOCAL void bt_pf_a2dp_ev_pf_avdtp_open_cfm( bt_ev_pf_avdtp_open_cfm_type *ev )
{
  bt_ev_msg_type event;

  BT_MSG_DEBUG("BT PF A2DP EV RX: open cfm hndl %x err %x ss|sss %08x",
               ev->handle, ev->error,
               (bt_pf_a2dp_unit.state << 16) | bt_pf_a2dp_unit.substate );

  if( bt_pf_a2dp_unit.state != BT_PF_A2DP_SS_CONFIGURED )
  {
    BT_MSG_HIGH("BT PF A2DP: open cfm in unexpected state %x",
                bt_pf_a2dp_unit.state, 0, 0);
  }
  if( ev->error == BT_CS_GN_SUCCESS &&
      ev->handle == bt_pf_a2dp_unit.handle )
  {
    bt_pf_a2dp_unit.state  = BT_PF_A2DP_SS_OPEN;
    bt_pf_a2dp_unit.role   = BT_PF_A2DP_ROLE_NONE;
    rex_clr_timer( &bt_pf_a2dp_unit.acp_timer );

    BT_MSG_HIGH( "BT PF A2DP: Media Channel Open. MTU %x",
                 ev->out_mtu, 0, 0 );

    if(bt_pf_a2dp_unit.discon_reason == 
         BT_CS_PF_A2DP_ERR_ATTEMPTED_TO_CLOSE_AND_OPEN_STREAM)
    {
      bt_pf_a2dp_unit.discon_reason = BT_CS_PF_A2DP_ERR_A2DP_NONE;
    }

    if( ev->out_mtu > BT_PF_A2DP_MAX_MTU )
    {
      ev->out_mtu = BT_PF_A2DP_MAX_MTU;
    }
    bt_pf_a2dp_unit.out_mtu = ev->out_mtu;

    // Send the open event
    event.ev_hdr.bt_app_id = bt_pf_a2dp_unit.ext_id;
    event.ev_hdr.ev_type   = BT_EV_PF_A2DP_OPEN;
    event.ev_msg.ev_a2dp_open.origin = BT_PF_A2DP_EVENT_LOCAL_INIT;
    event.ev_msg.ev_a2dp_open.bd_addr = bt_pf_a2dp_unit.bd_addr;

    BT_MSG_DEBUG("BT PF A2DP TX EV: open. aid: %x, Init %x",
                 bt_pf_a2dp_unit.ext_id,
                 event.ev_msg.ev_a2dp_open.origin,0);
    bt_ec_send_event( &event );


    if( bt_pf_a2dp_unit.substate == BT_PF_A2DP_SSS_ATTEMPT_TO_SUSPEND )
    {
      bt_pf_a2dp_end_streaming( BT_PF_A2DP_EVENT_LOCAL_INIT );
    }
    else if( bt_pf_a2dp_unit.substate == BT_PF_A2DP_SSS_ATTEMPT_TO_STREAM &&
             bt_pf_a2dp_unit.state != BT_PF_A2DP_SS_STREAMING )
    {
      bt_pf_cmd_msg_type cmd_msg;
      bt_cmd_pf_a2dp_start_type *cmd_ptr = &cmd_msg.cmd_msg.cmd_a2dp_start;

      cmd_msg.cmd_hdr.app_id = bt_pf_a2dp_unit.int_id;
      cmd_ptr->sbc_data = bt_pf_a2dp_null_sbc_data;
      bt_pf_a2dp_cmd_start( &cmd_msg );      
    }
    else if( bt_pf_a2dp_unit.substate == BT_PF_A2DP_SSS_ATTEMPT_TO_DISCONNECT ||
             bt_pf_a2dp_unit.substate == BT_PF_A2DP_SSS_ATTEMPT_TO_DISABLE )
    {
      bt_cmd_pf_a2dp_disconnect( bt_pf_a2dp_unit.int_id );
    }
  }
  else
  {
    BT_ERR( "BT PF A2DP: Error opening stream %x, ev hndl %x, int hndl %x",
            ev->error,
            ev->handle,
            bt_pf_a2dp_unit.handle );
    if( bt_pf_a2dp_unit.discon_reason !=
        BT_CS_PF_A2DP_ERR_ATTEMPTED_TO_CLOSE_AND_OPEN_STREAM )
    {
      bt_pf_a2dp_unit.discon_reason = BT_CS_PF_A2DP_ERR_FAILED_TO_START_STREAM;
    }
    bt_cmd_pf_a2dp_disconnect( bt_pf_a2dp_unit.int_id );
  }
}

/*===========================================================================

FUNCTION
bt_pf_a2dp_ev_pf_avdtp_open_ind

DESCRIPTION
Handles event from AVDTP.

===========================================================================*/
LOCAL void bt_pf_a2dp_ev_pf_avdtp_open_ind( bt_ev_pf_avdtp_open_ind_type *ev )
{
  bt_ev_msg_type event;

  BT_MSG_DEBUG("BT PF A2DP EV RX: open ind hndl %x ss|sss %08x",
               ev->handle,
               (bt_pf_a2dp_unit.state << 16) | bt_pf_a2dp_unit.substate,
               0);

  if( bt_pf_a2dp_unit.state != BT_PF_A2DP_SS_CONFIGURED )
  {
    BT_MSG_HIGH("BT PF A2DP: open ind in unexpected state %x",
                bt_pf_a2dp_unit.state, 0, 0);
  }
  else if( ev->handle == bt_pf_a2dp_unit.handle )
  {
    // Set the state to open
    bt_pf_a2dp_unit.state  = BT_PF_A2DP_SS_OPEN;

    // While we're theoretically no longer the acceptor, don't clear
    // this info until we get a Start from the remote. If we don't get
    // a start, we will Abort and start over anyway.

    if( bt_pf_a2dp_unit.role == BT_PF_A2DP_ROLE_ACP )
    {
      rex_set_timer( &bt_pf_a2dp_unit.acp_timer,
                     BT_PF_A2DP_ACP_TIMEOUT_MS );
    }

    BT_MSG_HIGH( "BT PF A2DP: Media Channel Open. MTU %x",
                 ev->out_mtu, 0, 0 );

    if( ev->out_mtu > BT_PF_A2DP_MAX_MTU )
    {
      ev->out_mtu = BT_PF_A2DP_MAX_MTU;
    }
    bt_pf_a2dp_unit.out_mtu = ev->out_mtu;

    // Send the open event
    event.ev_hdr.bt_app_id = bt_pf_a2dp_unit.ext_id;
    event.ev_hdr.ev_type   = BT_EV_PF_A2DP_OPEN;
    event.ev_msg.ev_a2dp_open.origin = BT_PF_A2DP_EVENT_REMOTE_INIT;
    event.ev_msg.ev_a2dp_open.bd_addr = bt_pf_a2dp_unit.bd_addr;
    BT_MSG_DEBUG("BT PF A2DP TX EV: open. AppID: %x, Init %x",
                 bt_pf_a2dp_unit.ext_id,
                 event.ev_msg.ev_a2dp_open.origin,0);
    bt_ec_send_event( &event );
  }
}

/*===========================================================================

FUNCTION
bt_pf_a2dp_ev_pf_avdtp_close_ind

DESCRIPTION
Handles event from AVDTP.

===========================================================================*/
LOCAL void bt_pf_a2dp_ev_pf_avdtp_close_ind( bt_ev_pf_avdtp_close_ind_type *ev )
{
  BT_MSG_DEBUG("BT PF A2DP EV RX: close ind hndl %x ss|sss %08x",
               ev->handle,
               (bt_pf_a2dp_unit.state << 16) | bt_pf_a2dp_unit.substate,
               0);

  if( ev->handle == bt_pf_a2dp_unit.handle )
  {
    bt_pf_a2dp_end_streaming( BT_PF_A2DP_EVENT_REMOTE_INIT );
    if( bt_pf_a2dp_unit.state == BT_PF_A2DP_SS_STREAMING ||
          bt_pf_a2dp_unit.state == BT_PF_A2DP_SS_SUSPENDING )
    {
      bt_pf_a2dp_unit.state = BT_PF_A2DP_SS_CLOSE_FRM_STREAMING;
    }
    else
    {
      bt_pf_a2dp_unit.state = BT_PF_A2DP_SS_CLOSE_FRM_OPEN;
    }
    if( bt_pf_a2dp_unit.substate != BT_PF_A2DP_SSS_ATTEMPT_TO_DISCONNECT &&
        bt_pf_a2dp_unit.substate != BT_PF_A2DP_SSS_ATTEMPT_TO_DISABLE )
    {
      bt_pf_a2dp_set_substate( BT_PF_A2DP_SSS_REMOTE_CLOSING );
    }
  }
  else
  {
    BT_ERR( "BT PF A2DP: Wrong handle in close ind %x, should be %x",
            ev->handle,
            bt_pf_a2dp_unit.handle,
            0 );
  }
}

/*===========================================================================

FUNCTION
bt_pf_a2dp_ev_pf_avdtp_close_cfm

DESCRIPTION
Handles event from AVDTP.

===========================================================================*/
LOCAL void bt_pf_a2dp_ev_pf_avdtp_close_cfm( bt_ev_pf_avdtp_close_cfm_type *ev )
{
  BT_MSG_DEBUG("BT PF A2DP EV RX: close cfm hndl %x err %x ss|sss %08x",
               ev->handle, ev->error,
               (bt_pf_a2dp_unit.state << 16) | bt_pf_a2dp_unit.substate );

  if( bt_pf_a2dp_unit.state != BT_PF_A2DP_SS_CLOSE_FRM_OPEN && 
      bt_pf_a2dp_unit.state != BT_PF_A2DP_SS_CLOSE_FRM_STREAMING )
  {
    BT_MSG_HIGH("BT PF A2DP: close cfm in unexpected state %x",
                 bt_pf_a2dp_unit.state, 0, 0);
  }
  else if( ev->handle != bt_pf_a2dp_unit.handle )
  {
    BT_ERR( "BT PF A2DP: Wrong handle in close cfm %x, should be %x",
            ev->handle,
            bt_pf_a2dp_unit.handle,
            0 );
  } 
  else if ( ev->error != BT_CS_GN_SUCCESS )
  {
    /* Abort the stream */ 
    BT_ERR("BT PF A2DP: Can't close stream err %x; aborting",ev->error,0,0);
    bt_pf_a2dp_abort_stream();
  }
  else if( bt_pf_a2dp_unit.substate == BT_PF_A2DP_SSS_ATTEMPT_TO_SUSPEND )
  {
    bt_pf_a2dp_end_streaming( BT_PF_A2DP_EVENT_LOCAL_INIT );
    bt_pf_a2dp_unit.handle   = 0;
    bt_pf_a2dp_unit.state    = BT_PF_A2DP_SS_CONFIGURING;
    bt_pf_a2dp_set_substate( BT_PF_A2DP_SSS_NONE );
  
  }
  else if( bt_pf_a2dp_unit.substate == BT_PF_A2DP_SSS_ATTEMPT_TO_STREAM ||
           bt_pf_a2dp_unit.substate == BT_PF_A2DP_SSS_DISCOVERING       ||
           bt_pf_a2dp_unit.substate == BT_PF_A2DP_SSS_DISC_THEN_STREAM  ||
           bt_pf_a2dp_unit.substate == BT_PF_A2DP_SSS_RECONFIGURING )
  {
    bt_pf_a2dp_end_streaming( BT_PF_A2DP_EVENT_LOCAL_INIT );
    bt_pf_a2dp_send_close_ev( BT_PF_A2DP_EVENT_LOCAL_INIT );

    bt_pf_a2dp_unit.handle   = 0;
    bt_pf_a2dp_unit.state    = BT_PF_A2DP_SS_CONFIGURING;
    bt_pf_a2dp_set_substate( BT_PF_A2DP_SSS_ATTEMPT_TO_STREAM );

    if( bt_pf_a2dp_unit.recfg_option == BT_PF_A2DP_REPAGE_DEVICE )
    {
      bt_pf_a2dp_unit.discon_reason = BT_CS_PF_A2DP_NORMAL_DISCONNECT;
      BT_MSG_DEBUG("BT PF A2DP CMD TX: disconnect -- attempt to reconnect",0,0,0);
      /* Use AVDTP disconnect directly here */
      bt_cmd_pf_avdtp_disconnect( bt_pf_a2dp_unit.int_id,
                                  &bt_pf_a2dp_unit.bd_addr );
    }
    else
    {
      bt_pf_cmd_msg_type cmd_msg;
      bt_cmd_pf_a2dp_start_type *cmd_ptr = &cmd_msg.cmd_msg.cmd_a2dp_start;
      // If we're trying to reconfigure the stream.
      // Reopen it with the correct parameters
      bt_pf_a2dp_unit.state = BT_PF_A2DP_SS_CONFIGURING;

      cmd_msg.cmd_hdr.app_id = bt_pf_a2dp_unit.int_id;
      cmd_ptr->sbc_data = bt_pf_a2dp_null_sbc_data;
      bt_pf_a2dp_cmd_start( &cmd_msg );
    }
  }
  else if( bt_pf_a2dp_unit.substate == BT_PF_A2DP_SSS_REMOTE_CLOSING )
  {
    bt_pf_a2dp_send_close_ev( BT_PF_A2DP_EVENT_REMOTE_INIT );

    bt_pf_a2dp_unit.handle       = 0;
    // Setting the state to Configuring here will skip the discover
    // and get caps step. This assumes that none of this information
    // needs to be refreshed.
    bt_pf_a2dp_unit.state        = BT_PF_A2DP_SS_CONFIGURING;
  }
  else if( bt_pf_a2dp_unit.substate == BT_PF_A2DP_SSS_ATTEMPT_TO_DISCONNECT ||
           bt_pf_a2dp_unit.substate == BT_PF_A2DP_SSS_ATTEMPT_TO_DISABLE )
  {
    bt_pf_a2dp_end_streaming( BT_PF_A2DP_EVENT_LOCAL_INIT );
    bt_pf_a2dp_send_close_ev( BT_PF_A2DP_EVENT_LOCAL_INIT );

    bt_pf_a2dp_unit.state    = BT_PF_A2DP_SS_CONFIGURING;

    bt_cmd_pf_a2dp_disconnect( bt_pf_a2dp_unit.int_id );
  }
  else if( bt_pf_a2dp_unit.suspend_on_end == FALSE )
  {
    bt_pf_a2dp_end_streaming( BT_PF_A2DP_EVENT_LOCAL_INIT );
    bt_pf_a2dp_send_close_ev( BT_PF_A2DP_EVENT_LOCAL_INIT );

    // We're closing the stream rather than suspending, so
    // start the configuring process
    bt_pf_a2dp_unit.handle           = 0;
    bt_pf_a2dp_unit.state            = BT_PF_A2DP_SS_CONFIGURING;
    bt_pf_a2dp_unit.substate         = BT_PF_A2DP_SSS_NONE;
    bt_pf_a2dp_unit.role             = BT_PF_A2DP_ROLE_INT;
  }
  else
  {
    bt_pf_a2dp_end_streaming( BT_PF_A2DP_EVENT_LOCAL_INIT );
    bt_pf_a2dp_send_close_ev( BT_PF_A2DP_EVENT_LOCAL_INIT );
    
    bt_pf_a2dp_unit.discon_reason = BT_CS_PF_A2DP_NORMAL_DISCONNECT;
    bt_pf_a2dp_unit.state         = BT_PF_A2DP_SS_CONFIGURING;
    BT_MSG_DEBUG("BT PF A2DP CMD TX: disconn. ss: %x sss: %x rsn: %x",
                 bt_pf_a2dp_unit.state,
                 bt_pf_a2dp_unit.substate,
                 bt_pf_a2dp_unit.discon_reason );
    bt_cmd_pf_a2dp_disconnect( bt_pf_a2dp_unit.int_id );
  }
}

/*===========================================================================

FUNCTION
bt_pf_a2dp_ev_pf_avdtp_start_ind

DESCRIPTION
Handles event from AVDTP.

===========================================================================*/
LOCAL void bt_pf_a2dp_ev_pf_avdtp_start_ind( bt_ev_pf_avdtp_start_ind_type *ev )
{
  BT_MSG_DEBUG("BT PF A2DP: rx start ind hndl %x ss|sss %08x",
               ev->handle,
               (bt_pf_a2dp_unit.state << 16) | bt_pf_a2dp_unit.substate,
               0 );

  if( ev->handle != bt_pf_a2dp_unit.handle )
  {
    BT_ERR( "BT PF A2DP: wrong handle in start ind %x, should be %x",
            ev->handle,
            bt_pf_a2dp_unit.handle,
            0 );
  }
  else if( bt_pf_a2dp_unit.substate == BT_PF_A2DP_SSS_ATTEMPT_TO_DISCONNECT ||
           bt_pf_a2dp_unit.substate == BT_PF_A2DP_SSS_ATTEMPT_TO_DISABLE )
  {
    /* Got a start indication after receiving a disconnect/disable command */
    /* Just ignore this, and hope all gets disconnected, since we should have
     * already started closing the stream */
    BT_MSG_DEBUG("BT PF A2DP: ignoring start ind", 0, 0, 0);
  }
  else if( bt_pf_a2dp_unit.con_ev_sent == FALSE )
  {
    rex_clr_timer( &bt_pf_a2dp_unit.acp_timer );
    rex_set_timer( &bt_pf_a2dp_unit.streaming_timer,
                   BT_PF_A2DP_STREAMING_TIMEOUT_MS );

    /* Inform upper layers we're now streaming */
    bt_pf_a2dp_start_streaming( BT_PF_A2DP_EVENT_REMOTE_INIT );

    if( bt_pf_a2dp_unit.substate != BT_PF_A2DP_SSS_DISCOVERING && 
        bt_pf_a2dp_unit.substate != BT_PF_A2DP_SSS_DISC_THEN_STREAM )
    {
      /* We were the acceptor and the ACP timer has not expired; 
       * get info about endpoints and capabilities now.
       */
      BT_MSG_DEBUG("BT PF A2DP CMD TX: discover",0,0,0);
      bt_pf_a2dp_unit.role     = BT_PF_A2DP_ROLE_NONE;
      bt_pf_a2dp_set_substate( BT_PF_A2DP_SSS_DISCOVERING );
      bt_cmd_pf_avdtp_discover( bt_pf_a2dp_unit.int_id,
                                &bt_pf_a2dp_unit.bd_addr, 
                                NULL );
    }
    else if( bt_pf_a2dp_unit.substate == BT_PF_A2DP_SSS_DISC_THEN_STREAM )
    {
      /* We're now streaming. Don't try to start streaming again when we're
       * done with the discovery process.
       */
      bt_pf_a2dp_set_substate( BT_PF_A2DP_SSS_DISCOVERING );
    }
  }
  else
  {
    if( bt_pf_a2dp_unit.substate == BT_PF_A2DP_SSS_ATTEMPT_TO_SUSPEND )
    {
      // We've received a suspend command while trying to start the stream.
      // Suspend it now.

      // Should we inform upper layers that the remot started streaming?
      bt_pf_a2dp_start_streaming( BT_PF_A2DP_EVENT_REMOTE_INIT );
      bt_cmd_pf_a2dp_suspend( bt_pf_a2dp_unit.int_id );
    }
    else
    {
      rex_set_timer( &bt_pf_a2dp_unit.streaming_timer,
                     BT_PF_A2DP_STREAMING_TIMEOUT_MS );
      bt_pf_a2dp_start_streaming( BT_PF_A2DP_EVENT_REMOTE_INIT );
    }
  }
}

/*===========================================================================

FUNCTION
bt_pf_a2dp_ev_pf_avdtp_start_cfm

DESCRIPTION
Handles event from AVDTP.

===========================================================================*/
LOCAL void bt_pf_a2dp_ev_pf_avdtp_start_cfm( bt_ev_pf_avdtp_start_cfm_type *ev )
{
  BT_MSG_DEBUG("BT PF A2DP EV RX: start cfm hndl %x err %x ss|sss %08x",
               ev->handle, ev->error,
               (bt_pf_a2dp_unit.state << 16) | bt_pf_a2dp_unit.substate );

  if( ev->error == BT_CS_GN_SUCCESS &&
      ev->handle == bt_pf_a2dp_unit.handle )
  {
    if( bt_pf_a2dp_unit.substate == BT_PF_A2DP_SSS_ATTEMPT_TO_SUSPEND ||
        bt_pf_a2dp_unit.substate == BT_PF_A2DP_SSS_ATTEMPT_TO_DISABLE ||
        bt_pf_a2dp_unit.substate == BT_PF_A2DP_SSS_ATTEMPT_TO_DISCONNECT )
    {
      // We've received a suspend or disconnect command while trying to start the
      // stream. Suspend it now.

      // Should we inform upper layers that the previous start command
      // was successful?
      bt_pf_a2dp_start_streaming( BT_PF_A2DP_EVENT_LOCAL_INIT );
      bt_cmd_pf_a2dp_suspend( bt_pf_a2dp_unit.int_id );
    }
    else
    {
      // Check the pending SBC config. If it's not the same as the current
      // config, must reconfig the stream:
      if( memcmp( (byte*)&bt_pf_a2dp_unit.sbc_config,
                  (byte*)&bt_pf_a2dp_unit.pending_sbc_config,
                  sizeof( audiosbcenc_data_type ) )
          == 0 )
      {
        bt_pf_a2dp_start_streaming( BT_PF_A2DP_EVENT_LOCAL_INIT );
      }
      else
      {
        bt_pf_a2dp_set_substate( BT_PF_A2DP_SSS_RECONFIGURING );
        if( bt_pf_a2dp_unit.recfg_option == BT_PF_A2DP_RECONFIGURE )
        {
          bt_pf_a2dp_unit.state = BT_PF_A2DP_SS_SUSPENDING;
          bt_cmd_pf_avdtp_suspend( bt_pf_a2dp_unit.int_id,
                                   bt_pf_a2dp_unit.handle,
                                   NULL );
        }
        else
        {
          /* Must close the stream to reconfigure */
		  bt_pf_a2dp_close_stream( BT_PF_A2DP_SS_CLOSE_FRM_STREAMING, 
			                       BT_CS_PF_A2DP_ERR_ATTEMPTED_TO_CLOSE_AND_OPEN_STREAM );
        }
      }
    }
  }
  else if( ev->handle == bt_pf_a2dp_unit.handle &&
           bt_pf_a2dp_unit.state != BT_PF_A2DP_SS_STREAMING )
  {
    bt_pf_a2dp_unit.discon_reason = BT_CS_PF_A2DP_ERR_FAILED_TO_START_STREAM;
    BT_ERR("BT PF A2DP: Can't start streaming err %x; aborting",ev->error,0,0);
    BT_MSG_DEBUG("BT PF A2DP CMD TX: Abort",0,0,0);

    bt_pf_a2dp_unit.state    = BT_PF_A2DP_SS_ABORTING;
    if( bt_pf_a2dp_unit.substate != BT_PF_A2DP_SSS_ATTEMPT_TO_SUSPEND &&
        bt_pf_a2dp_unit.substate != BT_PF_A2DP_SSS_ATTEMPT_TO_DISABLE &&
        bt_pf_a2dp_unit.substate != BT_PF_A2DP_SSS_ATTEMPT_TO_DISCONNECT )
    {
      bt_pf_a2dp_set_substate( BT_PF_A2DP_SSS_ATTEMPT_TO_STREAM );
    }
    bt_pf_a2dp_abort_stream();
  }
}

/*===========================================================================

FUNCTION
bt_pf_a2dp_ev_pf_avdtp_suspend_ind

DESCRIPTION
Handles event from AVDTP.

===========================================================================*/
LOCAL void bt_pf_a2dp_ev_pf_avdtp_suspend_ind( bt_ev_pf_avdtp_suspend_ind_type *ev )
{
  BT_MSG_DEBUG("BT PF A2DP EV RX: suspend ind hndl %x ss|sss %08x",
               ev->handle,
               (bt_pf_a2dp_unit.state << 16) | bt_pf_a2dp_unit.substate, 0);
  if( ev->handle == bt_pf_a2dp_unit.handle )
  {
    if( ( bt_pf_a2dp_unit.state == BT_PF_A2DP_SS_STREAMING )           ||
        ( bt_pf_a2dp_unit.state == BT_PF_A2DP_SS_SUSPENDING )          ||
        ( bt_pf_a2dp_unit.state == BT_PF_A2DP_SS_CLOSE_FRM_STREAMING ) ||
        ( bt_pf_a2dp_unit.state == BT_PF_A2DP_SS_ABORTING ) ) 
    {
      bt_pf_a2dp_end_streaming( BT_PF_A2DP_EVENT_REMOTE_INIT );
    }

    switch(bt_pf_a2dp_unit.state)
    {
      case BT_PF_A2DP_SS_STREAMING:
      case BT_PF_A2DP_SS_SUSPENDING:
        bt_pf_a2dp_unit.state = BT_PF_A2DP_SS_OPEN;
         break;
      case BT_PF_A2DP_SS_CLOSE_FRM_STREAMING:
        bt_pf_a2dp_unit.state = BT_PF_A2DP_SS_CLOSE_FRM_OPEN;
        break;
      case BT_PF_A2DP_SS_ABORTING:
        /* Ignore the event as abort is already underway */
       break;
      default:
       BT_ERR("BT PF A2DP: Unexpected Event ", 0, 0, 0);
    }
  }
  else
  {
    BT_ERR( "BT PF A2DP: wrong handle in suspend ind %x, should be %x",
            ev->handle, bt_pf_a2dp_unit.handle, 0 );
  }
}

/*===========================================================================

FUNCTION
bt_pf_a2dp_ev_pf_avdtp_suspend_cfm

DESCRIPTION
Handles event from AVDTP.

===========================================================================*/
LOCAL void bt_pf_a2dp_ev_pf_avdtp_suspend_cfm( bt_ev_pf_avdtp_suspend_cfm_type *ev )
{
  BT_MSG_DEBUG("BT PF A2DP EV RX: suspend cfm hndl %x err %x ss|sss %08x",
               ev->handle, ev->error,
               (bt_pf_a2dp_unit.state << 16) | bt_pf_a2dp_unit.substate );

  if( ev->error == BT_CS_GN_SUCCESS &&
      bt_pf_a2dp_unit.state == BT_PF_A2DP_SS_SUSPENDING &&
      ev->handle == bt_pf_a2dp_unit.handle )
  {
    bt_pf_a2dp_end_streaming( BT_PF_A2DP_EVENT_LOCAL_INIT );
    // Set the state to OPEN
    bt_pf_a2dp_unit.state = BT_PF_A2DP_SS_OPEN;
    if( (bt_pf_a2dp_unit.substate == BT_PF_A2DP_SSS_ATTEMPT_TO_STREAM) ||
        (bt_pf_a2dp_unit.substate == BT_PF_A2DP_SSS_RECONFIGURING) )
    {
      bt_pf_cmd_msg_type cmd_msg;
      bt_cmd_pf_a2dp_start_type *cmd_ptr = &cmd_msg.cmd_msg.cmd_a2dp_start;

      cmd_msg.cmd_hdr.app_id = bt_pf_a2dp_unit.int_id;
      cmd_ptr->sbc_data = bt_pf_a2dp_null_sbc_data;
      bt_pf_a2dp_cmd_start( &cmd_msg );
    }
    else if( bt_pf_a2dp_unit.substate == BT_PF_A2DP_SSS_ATTEMPT_TO_DISABLE ||
             bt_pf_a2dp_unit.substate == BT_PF_A2DP_SSS_ATTEMPT_TO_DISCONNECT )
    {
      bt_cmd_pf_a2dp_disconnect( bt_pf_a2dp_unit.int_id );
    }
    
  }
  else if( ev->handle == bt_pf_a2dp_unit.handle )
  {
    BT_ERR("BT PF A2DP: Failed to suspend %x state %x.",
           ev->error,
           bt_pf_a2dp_unit.state, 0);
    if( bt_pf_a2dp_unit.state == BT_PF_A2DP_SS_SUSPENDING )
    {
      bt_pf_a2dp_close_stream( BT_PF_A2DP_SS_CLOSE_FRM_STREAMING, 
			                   BT_CS_PF_A2DP_ERR_ATTEMPTED_TO_CLOSE_AND_OPEN_STREAM );
    }
    else if ( ev->error == BT_CS_GN_SUCCESS )
    {
      /* We've received a successful suspend confirm from the remote,
       * but we're not expecting one. There's a mismatch somewhere.
       * Lets start over */
      bt_pf_a2dp_abort_stream();
    }
  }
}

/*===========================================================================

FUNCTION
bt_pf_a2dp_ev_pf_avdtp_recfg_ind

DESCRIPTION
Handles event from AVDTP.

===========================================================================*/
LOCAL void bt_pf_a2dp_ev_pf_avdtp_recfg_ind( bt_ev_pf_avdtp_recfg_ind_type *ev )
{
  BT_MSG_DEBUG("BT PF A2DP EV RX: recfg ind hndl %x num_caps %x ss|sss %08x",
               ev->handle, ev->num_caps,
               (bt_pf_a2dp_unit.state << 16) | bt_pf_a2dp_unit.substate );

  if( bt_pf_a2dp_unit.state == BT_PF_A2DP_SS_OPEN                            &&
      ev->handle == bt_pf_a2dp_unit.handle                                   &&
      ev->num_caps == 1                                                      &&
      ev->caps[0].type                       == BT_PF_AVDTP_CAP_CODEC        &&
      ev->caps[0].data.codec_data.media_type == BT_PF_AVDTP_MEDIA_TYPE_AUDIO &&
      ev->caps[0].data.codec_data.codec_type == BT_PF_AVDTP_CODEC_SBC        &&
      bt_pf_a2dp_verify_sbc_data( &ev->caps[1].data.codec_data.data.sbc_data ) )
  {
    bt_pf_a2dp_unit.sbc_config       = ev->caps[0].data.codec_data.data.sbc_data;

    // Tell AVS the current config
    audiosbc_set_enc_params( (audiosbcenc_data_type*)&bt_pf_a2dp_unit.sbc_config );

    bt_cmd_pf_avdtp_reconfigure_rsp( bt_pf_a2dp_unit.int_id,
                                     ev->handle,
                                     ev->txn,
                                     BT_PF_AVDTP_CAP_CODEC,
                                     BT_CS_GN_SUCCESS );
  }
  else
  {
    bt_cmd_status_type error = BT_CS_PF_AVDTP_ERR_UNSUPPORTED_CONFIG;

    if(  bt_pf_a2dp_unit.state != BT_PF_A2DP_SS_OPEN )
    {
      error = BT_CS_PF_AVDTP_ERR_STATE;
    }
    else if( ev->handle != bt_pf_a2dp_unit.handle )
    {
      error = BT_CS_PF_AVDTP_ERR_BAD_SERVICE;
    }
    else if ( ev->num_caps != 1                                                      ||
              ev->caps[0].type                       != BT_PF_AVDTP_CAP_CODEC        ||
              ev->caps[0].data.codec_data.media_type == BT_PF_AVDTP_MEDIA_TYPE_AUDIO ||
              ev->caps[0].data.codec_data.codec_type == BT_PF_AVDTP_CODEC_SBC        ||
              bt_pf_a2dp_verify_sbc_data( &ev->caps[1].data.codec_data.data.sbc_data )
              == FALSE )
    {
      error = BT_CS_PF_AVDTP_ERR_INVALID_CAPABILITIES;
    }

    bt_cmd_pf_avdtp_reconfigure_rsp( bt_pf_a2dp_unit.int_id,
                                     ev->handle,
                                     ev->txn,
                                     BT_PF_AVDTP_CAP_CODEC,
                                     error );
  }
}

/*===========================================================================

FUNCTION
bt_pf_a2dp_ev_pf_avdtp_recfg_cfm

DESCRIPTION
Handles event from AVDTP.

===========================================================================*/
LOCAL void bt_pf_a2dp_ev_pf_avdtp_recfg_cfm( bt_ev_pf_avdtp_recfg_cfm_type *ev )
{
  BT_MSG_DEBUG("BT PF A2DP EV RX: recfg cfm hndl %x err %x ss|sss %08x",
               ev->handle, ev->error,
               (bt_pf_a2dp_unit.state << 16) | bt_pf_a2dp_unit.substate );

  if( bt_pf_a2dp_unit.state != BT_PF_A2DP_SS_RECONFIG_SENT &&
      bt_pf_a2dp_unit.state != BT_PF_A2DP_SS_RECONFIGURING )
  {
    BT_MSG_HIGH("BT PF A2DP: recfg cfm in unexpected state %x",
                bt_pf_a2dp_unit.state, 0, 0);
  }
  else if( ev->handle != bt_pf_a2dp_unit.handle )
  {
    BT_ERR( "BT PF A2DP: bad handle in recfg cfm %x, should be %x",
            ev->handle, bt_pf_a2dp_unit.handle, 0 );
  }
  else if( ev->error != BT_CS_GN_SUCCESS )
  {
    BT_ERR("BT A2DP: can't reconfigure stream. Attempting to close & reopen",0,0,0);
	bt_pf_a2dp_close_stream( BT_PF_A2DP_SS_CLOSE_FRM_OPEN, 
			                 BT_CS_PF_A2DP_ERR_ATTEMPTED_TO_CLOSE_AND_OPEN_STREAM );
    /* make sure we try again */
    if( bt_pf_a2dp_unit.substate != BT_PF_A2DP_SSS_ATTEMPT_TO_SUSPEND &&
        bt_pf_a2dp_unit.substate != BT_PF_A2DP_SSS_ATTEMPT_TO_DISABLE &&
        bt_pf_a2dp_unit.substate != BT_PF_A2DP_SSS_ATTEMPT_TO_DISCONNECT )
    {
      bt_pf_a2dp_set_substate( BT_PF_A2DP_SSS_ATTEMPT_TO_STREAM );
    }
  }
  else 
  {
    bt_pf_cmd_msg_type cmd_msg;
    bt_cmd_pf_a2dp_start_type *cmd_ptr = &cmd_msg.cmd_msg.cmd_a2dp_start;

    if( bt_pf_a2dp_unit.state == BT_PF_A2DP_SS_RECONFIG_SENT )
    {
      // save the new configuration
      bt_pf_a2dp_unit.sbc_config = bt_pf_a2dp_unit.pending_sbc_config;
      bt_pf_a2dp_unit.state      = BT_PF_A2DP_SS_OPEN;
    }
    // else bt_pf_a2dp_unit.state == BT_PF_A2DP_SS_CONFIGURING

    cmd_msg.cmd_hdr.app_id = bt_pf_a2dp_unit.int_id;
    cmd_ptr->sbc_data = bt_pf_a2dp_null_sbc_data;
    bt_pf_a2dp_cmd_start( &cmd_msg );
  }
}

/*===========================================================================

FUNCTION
bt_pf_a2dp_ev_pf_avdtp_abort_ind

DESCRIPTION
Handles event from AVDTP.

===========================================================================*/
LOCAL void bt_pf_a2dp_ev_pf_avdtp_abort_ind( bt_ev_pf_avdtp_abort_ind_type *ev )
{
  BT_MSG_HIGH("BT PF A2DP EV RX: abort ind handle %x",
              ev->handle, 0, 0);

  if( ev->handle == bt_pf_a2dp_unit.handle )
  {
    bt_pf_a2dp_unit.role = BT_PF_A2DP_ROLE_NONE;
    rex_clr_timer( &bt_pf_a2dp_unit.acp_timer );

    bt_pf_a2dp_end_streaming( BT_PF_A2DP_EVENT_REMOTE_INIT );

    // Restart connection process
    bt_pf_a2dp_unit.state = BT_PF_A2DP_SS_CONFIGURING;
    bt_cmd_pf_avdtp_discover( bt_pf_a2dp_unit.int_id,
                              &bt_pf_a2dp_unit.bd_addr, NULL );
  }
}

/*===========================================================================

FUNCTION
bt_pf_a2dp_ev_pf_avdtp_abort_cfm

DESCRIPTION
Handles event from AVDTP.

===========================================================================*/
LOCAL void bt_pf_a2dp_ev_pf_avdtp_abort_cfm( bt_ev_pf_avdtp_abort_cfm_type *ev )
{
  BT_MSG_HIGH("BT PF A2DP EV RX: abort cfm. hndl %x err %x",
              ev->handle, ev->error, 0);

  if( ev->handle == bt_pf_a2dp_unit.handle )
  {
    bt_pf_a2dp_unit.role = BT_PF_A2DP_ROLE_NONE;
    rex_clr_timer( &bt_pf_a2dp_unit.acp_timer );

    bt_pf_a2dp_end_streaming( BT_PF_A2DP_EVENT_REMOTE_INIT );

    if( ev->error == BT_CS_GN_SUCCESS )
    {
      if( bt_pf_a2dp_unit.a2dp_abort_stream_retries == BT_PF_A2DP_MAX_ABORT_RETRY_ATTEMPTS )
      {
        bt_pf_a2dp_unit.discon_reason = BT_CS_PF_A2DP_ERR_A2DP_NONE;
        /* Abort attempts reached so disconnect */
        bt_cmd_pf_a2dp_disconnect( bt_pf_a2dp_unit.int_id );
        bt_pf_a2dp_unit.a2dp_abort_stream_retries = 0;
      }
      else 
      {
        /* Restart connection process */
        bt_pf_a2dp_unit.state = BT_PF_A2DP_SS_CONFIGURING;
        bt_cmd_pf_avdtp_discover( bt_pf_a2dp_unit.int_id,
                                  &bt_pf_a2dp_unit.bd_addr, 
                                  NULL );
      }
      
    }
    else
    {
      bt_pf_a2dp_unit.discon_reason = BT_CS_PF_A2DP_ERR_ABORT_FAILED;
      // Abort failed. Disconnect
      bt_cmd_pf_a2dp_disconnect( bt_pf_a2dp_unit.int_id );
    }
  }
}

/*===========================================================================

FUNCTION
bt_pf_a2dp_ev_pf_avdtp_sec_ind

DESCRIPTION
Handles event from AVDTP.

===========================================================================*/
LOCAL void bt_pf_a2dp_ev_pf_avdtp_sec_ind( bt_ev_pf_avdtp_sec_ind_type *ev )
{
  BT_ERR("BT PF A2DP EV RX sec ind: unhandled",0,0,0);
}

/*===========================================================================

FUNCTION
bt_pf_a2dp_ev_pf_avdtp_sec_cfm

DESCRIPTION
Handles event from AVDTP.

===========================================================================*/
LOCAL void bt_pf_a2dp_ev_pf_avdtp_sec_cfm( bt_ev_pf_avdtp_sec_cfm_type *ev )
{
  BT_ERR("BT PF A2DP EV RX sec cfm: unhandled",0,0,0);
}

/*===========================================================================

FUNCTION
bt_pf_a2dp_ev_pf_avdtp_recv_data

DESCRIPTION
Handles event from AVDTP.

===========================================================================*/
LOCAL void bt_pf_a2dp_ev_pf_avdtp_recv_data( bt_ev_pf_avdtp_recv_data_type *ev )
{
  dsm_item_type *free_me_ptr = ev->dsm_ptr;
  BT_ERR("BT PF A2DP EV RX recv data: unhandled",0,0,0);
  dsm_free_packet( &free_me_ptr );
}

/*===========================================================================

FUNCTION
bt_pf_a2dp_ev_pf_avdtp_send_cfm

DESCRIPTION
Handles event from AVDTP.

===========================================================================*/
LOCAL void bt_pf_a2dp_ev_pf_avdtp_send_cfm( bt_ev_pf_avdtp_send_cfm_type *ev )
{
#ifdef FEATURE_BT_AVSYNC
  bt_pf_a2dp_frame_handle_type *a2dp_frame_handle;
#ifdef FEATURE_BT_AVSYNC_DEBUG
  time_type     time_stamp;
  uint32        sec,delta_ms;
  uint16        msec;
#endif

#ifdef FEATURE_BT_AVSYNC_DEBUG
  if(ev->result != OI_OK)
  {
    BT_MSG_HIGH("BT L2FLUSH: Send Ack to AVS for Failed Pkt",0, 0, 0);
  }
#endif
  /* De-queue and call the AVS Ack callback if required */
  a2dp_frame_handle = q_get(&a2dp_frame_handle_q);
  if(a2dp_frame_handle == NULL)
  {
    BT_ERR("BT PF A2DP: No A2DP frames queued", 0, 0, 0);
    return;
  }
  if(a2dp_frame_handle->last_frame)
  {
#ifdef FEATURE_BT_AVSYNC_DEBUG
    BT_MSG_API("BT PF A2DP: SND tx cfm to AVS frame = 0x%x",
                  a2dp_frame_handle->a2dp_last_seq, 0, 0);
#endif
    snd_sbc_avsync(QW_CVT_N2Q(&a2dp_frame_handle->a2dp_last_seq));
  }
  
#ifdef FEATURE_BT_AVSYNC_DEBUG
  clk_read_ms(time_stamp);
  sec = (uint32)(QW_CVT_Q2N( time_stamp ) / 1000);
  msec = (uint16)(QW_CVT_Q2N( time_stamp ) % 1000);
  BT_MSG_HIGH("A2DP TX Completed : Time Stamp = %d s %3d ms",sec, msec, 0);
  sec = (uint32)(QW_CVT_Q2N( a2dp_frame_handle->sent_timestamp ) / 1000);
  msec = (uint16)(QW_CVT_Q2N( a2dp_frame_handle->sent_timestamp ) % 1000);
  BT_MSG_HIGH("A2DP Sent         : Time Stamp = %d s %3d ms",sec, msec, 0);
  delta_ms = (uint32)(QW_CVT_Q2N( time_stamp ) - 
             QW_CVT_Q2N( a2dp_frame_handle->sent_timestamp ));
  BT_MSG_HIGH(" Delta = %d ms", delta_ms,0,0); 
#endif
  q_put(&a2dp_free_frame_handle_q, &a2dp_frame_handle->link);
#endif /* FEATURE_BT_AVSYNC */
}

/*===========================================================================

FUNCTION
bt_pf_a2dp_get_max_event_bytes

DESCRIPTION
Returns the number of bytes in the largest event expected to be
processed by the A2DP layer.

===========================================================================*/
LOCAL uint16 bt_pf_a2dp_get_max_event_bytes
(
 void
 )
{
  uint16  max_eb;

  max_eb = sizeof( bt_ev_pf_avdtp_con_ind_type );
  max_eb = MAX( max_eb, sizeof(bt_ev_pf_avdtp_con_cfm_type) );
  max_eb = MAX( max_eb, sizeof(bt_ev_pf_avdtp_discon_ind_type) );
  max_eb = MAX( max_eb, sizeof(bt_ev_pf_avdtp_disc_cfm_type) );
  max_eb = MAX( max_eb, sizeof(bt_ev_pf_avdtp_get_cap_cfm_type) );
  max_eb = MAX( max_eb, sizeof(bt_ev_pf_avdtp_set_cfg_ind_type) );
  max_eb = MAX( max_eb, sizeof(bt_ev_pf_avdtp_set_cfg_cfm_type) );
  max_eb = MAX( max_eb, sizeof(bt_ev_pf_avdtp_get_cfg_cfm_type) );
  max_eb = MAX( max_eb, sizeof(bt_ev_pf_avdtp_open_cfm_type) );
  max_eb = MAX( max_eb, sizeof(bt_ev_pf_avdtp_open_ind_type) );
  max_eb = MAX( max_eb, sizeof(bt_ev_pf_avdtp_close_ind_type) );
  max_eb = MAX( max_eb, sizeof(bt_ev_pf_avdtp_close_cfm_type) );
  max_eb = MAX( max_eb, sizeof(bt_ev_pf_avdtp_start_ind_type) );
  max_eb = MAX( max_eb, sizeof(bt_ev_pf_avdtp_start_cfm_type) );
  max_eb = MAX( max_eb, sizeof(bt_ev_pf_avdtp_suspend_ind_type) );
  max_eb = MAX( max_eb, sizeof(bt_ev_pf_avdtp_suspend_cfm_type) );
  max_eb = MAX( max_eb, sizeof(bt_ev_pf_avdtp_recfg_ind_type) );
  max_eb = MAX( max_eb, sizeof(bt_ev_pf_avdtp_recfg_cfm_type) );
  max_eb = MAX( max_eb, sizeof(bt_ev_pf_avdtp_abort_ind_type) );
  max_eb = MAX( max_eb, sizeof(bt_ev_pf_avdtp_abort_cfm_type) );
  max_eb = MAX( max_eb, sizeof(bt_ev_pf_avdtp_sec_ind_type) );
  max_eb = MAX( max_eb, sizeof(bt_ev_pf_avdtp_sec_cfm_type) );
  max_eb = MAX( max_eb, sizeof(bt_ev_pf_avdtp_recv_data_type) );
  max_eb = MAX( max_eb, sizeof(bt_ev_pf_avdtp_send_cfm_type) );

  max_eb += sizeof( bt_ev_hdr_type );

  return( max_eb );
}

/*===========================================================================

FUNCTION
bt_pf_a2dp_process_event

DESCRIPTION
Handles all BT driver events coming to the A2DP layer.

===========================================================================*/
LOCAL void bt_pf_a2dp_process_event( bt_ev_msg_type*  ev_msg_ptr )
{
  switch ( ev_msg_ptr->ev_hdr.ev_type )
  {
    case BT_EV_GN_CMD_DONE:
      {
        if ( ev_msg_ptr->ev_msg.ev_gn_cmd_done.cmd_status !=
             BT_CS_GN_SUCCESS )
        {
          BT_ERR( "BT A2DP: Bad CMD Done Stat %x Typ %x AID %x",
                  ev_msg_ptr->ev_msg.ev_gn_cmd_done.cmd_status,
                  ev_msg_ptr->ev_msg.ev_gn_cmd_done.cmd_type,
                  ev_msg_ptr->ev_hdr.bt_app_id );
          if( ev_msg_ptr->ev_msg.ev_gn_cmd_done.cmd_status
              == BT_CS_PF_NOT_CONNECTED )
          {
            // Some error. Let everyone know we're disconnected
            bt_ev_pf_avdtp_discon_ind_type ev;
            ev.bd_addr = bt_pf_a2dp_unit.bd_addr;
            ev.reason = BT_CS_PF_NOT_CONNECTED;
            bt_pf_a2dp_set_substate( BT_PF_A2DP_SSS_NONE );
            bt_pf_a2dp_unit.discon_reason = 
				           BT_CS_PF_A2DP_ERR_FAILED_TO_PROCESS_CMD;

            bt_pf_a2dp_ev_pf_avdtp_discon_ind( &ev );
          }
          else if( ev_msg_ptr->ev_msg.ev_gn_cmd_done.cmd_status
                   == BT_CS_PF_ALREADY_CONNECTED                ||
                   ev_msg_ptr->ev_msg.ev_gn_cmd_done.cmd_status
                   == BT_CS_PF_CONNECTION_EXISTS )
          {
            // Apparently, we were sent a connect command when already
            // connected.
            bt_ev_pf_avdtp_con_cfm_type ev;
            ev.bd_addr = bt_pf_a2dp_unit.bd_addr;
            ev.result  = BT_CS_GN_SUCCESS;
            BT_MSG_DEBUG("BT PF A2DP: Already connected. Generating con cfm event",
                         0,0,0);
            bt_pf_a2dp_unit.state = BT_PF_A2DP_SS_CONNECTING;
            bt_pf_a2dp_ev_pf_avdtp_con_cfm(&ev);
          }
          else if( ev_msg_ptr->ev_msg.ev_gn_cmd_done.cmd_type
                   == BT_PF_CMD_AVDTP_ABORT )
          {
            // The abort didn't work. As a last ditch effort, lets say it succeeded
            // and try to connect anyway.
            bt_ev_pf_avdtp_abort_cfm_type ev;
            BT_MSG_DEBUG("BT PF A2DP: Abort failed. Try to connect anyway",
                         0,0,0);
            ev.txn = 0;
            ev.handle = 0;
            ev.error = BT_CS_GN_SUCCESS;
            bt_pf_a2dp_ev_pf_avdtp_abort_cfm(&ev);
          }
        }
        break;
      }
    case BT_EV_PF_AVDTP_CON_IND:
      bt_pf_a2dp_ev_pf_avdtp_con_ind(&ev_msg_ptr->ev_msg.ev_avdtp_con_ind);
      break;
    case BT_EV_PF_AVDTP_CON_CFM:
      bt_pf_a2dp_ev_pf_avdtp_con_cfm(&ev_msg_ptr->ev_msg.ev_avdtp_con_cfm);
      break;
    case BT_EV_PF_AVDTP_DISCON_IND:
      bt_pf_a2dp_ev_pf_avdtp_discon_ind(&ev_msg_ptr->ev_msg.ev_avdtp_discon_ind);
      break;
    case BT_EV_PF_AVDTP_DISC_CFM:
      bt_pf_a2dp_ev_pf_avdtp_disc_cfm(&ev_msg_ptr->ev_msg.ev_avdtp_disc_cfm);
      break;
    case BT_EV_PF_AVDTP_GET_CAP_CFM:
      bt_pf_a2dp_ev_pf_avdtp_get_cap_cfm(&ev_msg_ptr->ev_msg.ev_avdtp_get_cap_cfm);
      break;
    case BT_EV_PF_AVDTP_SET_CFG_IND:
      bt_pf_a2dp_ev_pf_avdtp_set_cfg_ind(&ev_msg_ptr->ev_msg.ev_avdtp_set_cfg_ind);
      break;
    case BT_EV_PF_AVDTP_SET_CFG_CFM:
      bt_pf_a2dp_ev_pf_avdtp_set_cfg_cfm(&ev_msg_ptr->ev_msg.ev_avdtp_set_cfg_cfm);
      break;
    case BT_EV_PF_AVDTP_GET_CFG_CFM:
      bt_pf_a2dp_ev_pf_avdtp_get_cfg_cfm(&ev_msg_ptr->ev_msg.ev_avdtp_get_cfg_cfm);
      break;
    case BT_EV_PF_AVDTP_OPEN_CFM:
      bt_pf_a2dp_ev_pf_avdtp_open_cfm(&ev_msg_ptr->ev_msg.ev_avdtp_open_cfm);
      break;
    case BT_EV_PF_AVDTP_OPEN_IND:
      bt_pf_a2dp_ev_pf_avdtp_open_ind(&ev_msg_ptr->ev_msg.ev_avdtp_open_ind);
      break;
    case BT_EV_PF_AVDTP_CLOSE_IND:
      bt_pf_a2dp_ev_pf_avdtp_close_ind(&ev_msg_ptr->ev_msg.ev_avdtp_close_ind);
      break;
    case BT_EV_PF_AVDTP_CLOSE_CFM:
      bt_pf_a2dp_ev_pf_avdtp_close_cfm(&ev_msg_ptr->ev_msg.ev_avdtp_close_cfm);
      break;
    case BT_EV_PF_AVDTP_START_IND:
      bt_pf_a2dp_ev_pf_avdtp_start_ind(&ev_msg_ptr->ev_msg.ev_avdtp_start_ind);
      break;
    case BT_EV_PF_AVDTP_START_CFM:
      bt_pf_a2dp_ev_pf_avdtp_start_cfm(&ev_msg_ptr->ev_msg.ev_avdtp_start_cfm);
      break;
    case BT_EV_PF_AVDTP_SUSPEND_IND:
      bt_pf_a2dp_ev_pf_avdtp_suspend_ind(&ev_msg_ptr->ev_msg.ev_avdtp_suspend_ind);
      break;
    case BT_EV_PF_AVDTP_SUSPEND_CFM:
      bt_pf_a2dp_ev_pf_avdtp_suspend_cfm(&ev_msg_ptr->ev_msg.ev_avdtp_suspend_cfm);
      break;
    case BT_EV_PF_AVDTP_RECFG_IND:
      bt_pf_a2dp_ev_pf_avdtp_recfg_ind(&ev_msg_ptr->ev_msg.ev_avdtp_recfg_ind);
      break;
    case BT_EV_PF_AVDTP_RECFG_CFM:
      bt_pf_a2dp_ev_pf_avdtp_recfg_cfm(&ev_msg_ptr->ev_msg.ev_avdtp_recfg_cfm);
      break;
    case BT_EV_PF_AVDTP_ABORT_IND:
      bt_pf_a2dp_ev_pf_avdtp_abort_ind(&ev_msg_ptr->ev_msg.ev_avdtp_abort_ind);
      break;
    case BT_EV_PF_AVDTP_ABORT_CFM:
      bt_pf_a2dp_ev_pf_avdtp_abort_cfm(&ev_msg_ptr->ev_msg.ev_avdtp_abort_cfm);
      break;
    case BT_EV_PF_AVDTP_SEC_IND:
      bt_pf_a2dp_ev_pf_avdtp_sec_ind(&ev_msg_ptr->ev_msg.ev_avdtp_sec_ind);
      break;
    case BT_EV_PF_AVDTP_SEC_CFM:
      bt_pf_a2dp_ev_pf_avdtp_sec_cfm(&ev_msg_ptr->ev_msg.ev_avdtp_sec_cfm);
      break;
    case BT_EV_PF_AVDTP_RECV_DATA:
      bt_pf_a2dp_ev_pf_avdtp_recv_data(&ev_msg_ptr->ev_msg.ev_avdtp_recv_data);
      break;
    case BT_EV_PF_AVDTP_SEND_CFM:
      bt_pf_a2dp_ev_pf_avdtp_send_cfm(&ev_msg_ptr->ev_msg.ev_avdtp_send_cfm);
      break;
  }
}

/*===========================================================================

FUNCTION
bt_pf_a2dp_store_bt_event

DESCRIPTION
Stores a specified BT event on the A2DP BT event queue.

===========================================================================*/
LOCAL void bt_pf_a2dp_store_bt_event( bt_ev_msg_type*  bt_ev_ptr )
{
  bt_pf_store_bt_event( bt_ev_ptr, &bt_pf_a2dp_ev_q_info );
}

/*===========================================================================

FUNCTION
bt_pf_a2dp_process_ev_queue

DESCRIPTION
Dequeues events on the BT A2DP event queue and processes them.

===========================================================================*/
void bt_pf_a2dp_process_ev_queue( void )
{
  bt_ev_msg_type*  bt_ev_ptr;
  int              q_count = q_cnt( &bt_pf_a2dp_ev_q );

  BT_VSTAT_MAX( q_count, PF_A2DP_MAX_EV_Q_DEPTH );

  while ( (bt_ev_ptr = (bt_ev_msg_type *) q_get( &bt_pf_a2dp_ev_q )) != NULL )
  {
    bt_pf_a2dp_process_event( bt_ev_ptr );
    q_put( &bt_event_free_q, &bt_ev_ptr->ev_hdr.link );
  }
}

/*===========================================================================

CMD PROCESSING FUNCTION DEFINITIONS

===========================================================================*/

/*===========================================================================

FUNCTION
bt_pf_a2dp_cmd_enable

DESCRIPTION
Enables the A2DP layer. Registers for the L2CAP PSM, and initializes a
SRC endpoint.

===========================================================================*/
LOCAL void bt_pf_a2dp_cmd_enable( bt_pf_cmd_msg_type *cmd_ptr )
{
  bt_app_id_type old_avs_id;

  if( bt_pf_a2dp_unit.int_id != BT_APP_ID_NULL )
  {
    BT_ERR("BT PF A2DP: already enabled",0,0,0);
    cmd_ptr->cmd_hdr.cmd_status = BT_CS_PF_ALREADY_INITIALIZED;
    return;
  }


  BT_MSG_API("BT PF A2DP CMD RX: enableaid %x ss|sss %08x",
             cmd_ptr->cmd_hdr.app_id,
             (bt_pf_a2dp_unit.state << 16) | bt_pf_a2dp_unit.substate,0);
#ifdef FEATURE_BT_SOC
  snd_sbc_init( bt_pf_a2dp_sbc_data_cb, bt_pf_a2dp_sbc_init_cb, NULL );
#endif
  // Configure local SBC parameters.
  // Should really get this data from AVS.
  bt_pf_a2dp_local_sbc_data.octet0.sampling_frequency =
    AUDIOSBCENC_SAMPLING_FREQUENCY_48000 | AUDIOSBCENC_SAMPLING_FREQUENCY_44100;
  bt_pf_a2dp_local_sbc_data.octet0.channel_mode |=
    AUDIOSBCENC_CHANNEL_MODE_STEREO | AUDIOSBCENC_CHANNEL_MODE_MONO |
    AUDIOSBCENC_CHANNEL_MODE_JOINT_STEREO | AUDIOSBCENC_CHANNEL_MODE_DUAL_CHANNEL;
  bt_pf_a2dp_local_sbc_data.octet1.block_length =
    AUDIOSBCENC_BLOCK_LENGTH_16 | AUDIOSBCENC_BLOCK_LENGTH_8 |
    AUDIOSBCENC_BLOCK_LENGTH_12 | AUDIOSBCENC_BLOCK_LENGTH_4;
  bt_pf_a2dp_local_sbc_data.octet1.allocation_method |=
    AUDIOSBCENC_ALLOCATION_METHOD_SNR | AUDIOSBCENC_ALLOCATION_METHOD_LOUDNESS;
  bt_pf_a2dp_local_sbc_data.octet1.subbands |=
    AUDIOSBCENC_SUBBANDS_8 
#ifndef FEATURE_SBC_DSP_CODEC
    | AUDIOSBCENC_SUBBANDS_4
#endif /* FEATURE_SBC_DSP_CODEC */
    ;
  bt_pf_a2dp_local_sbc_data.min_bitpool = 2;
  bt_pf_a2dp_local_sbc_data.max_bitpool = 250;

  // Configure Caps
  bt_pf_a2dp_local_cap[0].type = BT_PF_AVDTP_CAP_MEDIA_TRANSPORT;
  bt_pf_a2dp_local_cap[0].data.generic_data.data        = NULL;
  bt_pf_a2dp_local_cap[0].data.generic_data.data_len    = 0;

  bt_pf_a2dp_local_cap[1].type = BT_PF_AVDTP_CAP_CODEC;
  bt_pf_a2dp_local_cap[1].data.codec_data.media_type = BT_PF_AVDTP_MEDIA_TYPE_AUDIO;
  bt_pf_a2dp_local_cap[1].data.codec_data.codec_type = BT_PF_AVDTP_CODEC_SBC;
  bt_pf_a2dp_local_cap[1].data.codec_data.data.sbc_data = bt_pf_a2dp_local_sbc_data;

  // Clear any old settings
  if( bt_pf_a2dp_unit.int_id != BT_APP_ID_NULL )
  {
    bt_cmd_ec_free_application_id( bt_pf_a2dp_unit.int_id );
  }
  rex_clr_timer( &bt_pf_a2dp_unit.acp_timer );
  rex_clr_timer( &bt_pf_a2dp_unit.streaming_timer );
  rex_clr_timer( &bt_pf_a2dp_unit.pending_data_timer );
  dsm_free_packet( &bt_pf_a2dp_unit.dsm_ptr );
  // rex doesn't have a way to properly remove critical sections.
  rex_init_crit_sect( &bt_pf_a2dp_unit.audio_crit_sect );

  old_avs_id = bt_pf_a2dp_unit.avs_id;

  memset( &bt_pf_a2dp_unit, 0, sizeof( bt_pf_a2dp_unit_type ) );

  bt_pf_a2dp_unit.avs_id = old_avs_id;

  rex_def_timer_ex( &bt_pf_a2dp_unit.acp_timer,
                    bt_pf_a2dp_acp_timeout_cb,
                    BT_PF_A2DP_ACP_TIMEOUT_MS );

  rex_def_timer_ex( &bt_pf_a2dp_unit.streaming_timer,
                    bt_pf_a2dp_streaming_timeout_cb,
                    BT_PF_A2DP_STREAMING_TIMEOUT_MS );

  rex_def_timer_ex( &bt_pf_a2dp_unit.pending_data_timer,
                    bt_pf_a2dp_pending_data_timeout_cb,
                    BT_PF_A2DP_PENDING_DATA_TIMEOUT_MS );

  rex_init_crit_sect( &bt_pf_a2dp_unit.audio_crit_sect );

  bt_pf_a2dp_unit.int_id =
    bt_cmd_ec_get_app_id_and_register( bt_pf_a2dp_store_bt_event );
  bt_pf_a2dp_unit.ext_id = cmd_ptr->cmd_hdr.app_id;

  bt_pf_a2dp_unit.suspend_on_end = TRUE;
  bt_pf_a2dp_unit.honor_in_use   = TRUE;
  bt_pf_a2dp_unit.recfg_option   = BT_PF_A2DP_REOPEN_STREAM;
  bt_pf_a2dp_unit.auto_bitrate   = TRUE;
  bt_pf_a2dp_unit.bitrate        = BT_PF_A2DP_BITRATE_HIGH;
  bt_pf_a2dp_unit.custom_bitrate = 0;
  bt_pf_a2dp_unit.app_ctl_start  = FALSE;
  bt_pf_a2dp_unit.a2dp_abort_stream_retries = 0;

  // Register with AVDTP
  bt_cmd_pf_avdtp_register( bt_pf_a2dp_unit.int_id );

  bt_cmd_pf_avdtp_register_endpoint( bt_pf_a2dp_unit.int_id,
                                     BT_PF_AVDTP_MEDIA_TYPE_AUDIO,
                                     BT_PF_AVDTP_SRC,
                                     bt_pf_a2dp_local_cap,
                                     2,
                                     &bt_pf_a2dp_unit.local_seid );

  bt_cmd_sd_register_serv_ext( bt_pf_a2dp_unit.int_id,
                               BT_SD_SERVICE_CLASS_AUDIO_SOURCE,
                               BT_PF_A2DP_VERSION_NUMBER,
                               0,
                               BT_PF_AVDTP_PSM,
                               FALSE,  /*  Do not page scan for this.  */
                               "A2DP Source" );


  cmd_ptr->cmd_hdr.cmd_status = BT_CS_GN_SUCCESS;
}

/*===========================================================================

FUNCTION
bt_pf_a2dp_cmd_disable

DESCRIPTION
Disables A2DP. Deregisters L2CAP.

===========================================================================*/
LOCAL void bt_pf_a2dp_cmd_disable( bt_pf_cmd_msg_type *cmd_ptr )
{
  BT_MSG_API("BT PF A2DP CMD RX: Disable aid %x ss|sss %08x",
             cmd_ptr->cmd_hdr.app_id,
             (bt_pf_a2dp_unit.state << 16) | bt_pf_a2dp_unit.substate, 0);

  bt_cmd_sd_unregister_srv_by_psm( bt_pf_a2dp_unit.int_id,
                                   BT_SD_SERVICE_CLASS_AUDIO_SOURCE,
                                   BT_PF_AVDTP_PSM );

  bt_cmd_pf_avdtp_deregister_endpoint( bt_pf_a2dp_unit.int_id,
                                       bt_pf_a2dp_unit.local_seid );
  bt_cmd_pf_avdtp_deregister( bt_pf_a2dp_unit.int_id );
  bt_cmd_ec_free_application_id( bt_pf_a2dp_unit.int_id );

  bt_pf_a2dp_unit.int_id          = BT_APP_ID_NULL;
  bt_pf_a2dp_unit.ext_id          = BT_APP_ID_NULL;
  bt_pf_a2dp_unit.a2dp_abort_stream_retries = 0;
  cmd_ptr->cmd_hdr.cmd_status = BT_CS_GN_SUCCESS;
}

/*===========================================================================

FUNCTION
bt_pf_a2dp_cmd_set_device

DESCRIPTION
Connects to a remote A2DP device and begins A2DP setup.

===========================================================================*/
LOCAL void bt_pf_a2dp_cmd_set_device( bt_pf_cmd_msg_type *cmd_ptr )
{
  BT_MSG_API("BT PF A2DP CMD RX: set device aid %x",
             cmd_ptr->cmd_hdr.app_id,0,0);
  if( bt_pf_a2dp_unit.state != BT_PF_A2DP_SS_IDLE )
  {
    cmd_ptr->cmd_hdr.cmd_status = BT_CS_PF_INVALID_STATE;
  }
  else
  {
    bt_pf_a2dp_unit.bd_addr = cmd_ptr->cmd_msg.cmd_a2dp_set_device.bd_addr;
    cmd_ptr->cmd_hdr.cmd_status = BT_CS_GN_SUCCESS;
  }
}

/*===========================================================================

FUNCTION
bt_pf_a2dp_cmd_connect

DESCRIPTION
Connects to a remote A2DP device and begins A2DP setup.

===========================================================================*/
LOCAL void bt_pf_a2dp_cmd_connect( bt_pf_cmd_msg_type *cmd_ptr )
{
  BT_MSG_API("BT PF A2DP CMD RX: connect aid %x ss|sss %08x",
             cmd_ptr->cmd_hdr.app_id,
             (bt_pf_a2dp_unit.state << 16) | bt_pf_a2dp_unit.substate, 0);
  cmd_ptr->cmd_hdr.cmd_status = BT_CS_PF_INVALID_PARAMETERS;

  if ( bt_pf_a2dp_unit.state != BT_PF_A2DP_SS_IDLE )
  {
    cmd_ptr->cmd_hdr.cmd_status = BT_CS_PF_ALREADY_CONNECTED;
  }
  else if ( BT_BD_ADDRS_EQUAL( &cmd_ptr->cmd_msg.cmd_a2dp_connect.bd_addr,
                               &bt_pf_a2dp_null_bd_addr )
            == FALSE )
  {
    // if the connect address is not null, set the internal address and connect
    bt_pf_a2dp_unit.bd_addr = cmd_ptr->cmd_msg.cmd_a2dp_connect.bd_addr;

    bt_pf_a2dp_unit.discon_reason = BT_CS_PF_A2DP_ERR_A2DP_NONE;
    bt_pf_a2dp_unit.state = BT_PF_A2DP_SS_CONNECTING;
    bt_pf_a2dp_unit.role = BT_PF_A2DP_ROLE_INT;
    rex_clr_timer( &bt_pf_a2dp_unit.acp_timer );
    bt_cmd_pf_avdtp_connect( bt_pf_a2dp_unit.int_id,
                             &bt_pf_a2dp_unit.bd_addr );
    cmd_ptr->cmd_hdr.cmd_status = BT_CS_GN_SUCCESS;
  }

}

/*===========================================================================

FUNCTION
bt_pf_a2dp_cmd_disconnect

DESCRIPTION
Disconnects from a connected A2DP device.

===========================================================================*/
LOCAL void bt_pf_a2dp_cmd_disconnect( bt_pf_cmd_msg_type *cmd_ptr )
{
  BT_MSG_API("BT PF A2DP CMD RX: disconnect aid %x ss|sss %08x",
             cmd_ptr->cmd_hdr.app_id,
             (bt_pf_a2dp_unit.state << 16) | bt_pf_a2dp_unit.substate, 0);

  bt_pf_a2dp_set_substate( BT_PF_A2DP_SSS_ATTEMPT_TO_DISCONNECT );

  if( bt_pf_a2dp_unit.state == BT_PF_A2DP_SS_OPEN ||
      bt_pf_a2dp_unit.state == BT_PF_A2DP_SS_STREAMING )
  {
    // Need to close the stream first
    if( bt_pf_a2dp_unit.state == BT_PF_A2DP_SS_STREAMING )
    {
      bt_pf_a2dp_unit.state = BT_PF_A2DP_SS_CLOSE_FRM_STREAMING;
    }
    else
    {
      bt_pf_a2dp_unit.state = BT_PF_A2DP_SS_CLOSE_FRM_OPEN;
    } 
    BT_MSG_DEBUG("BT PF A2DP CMD TX: close",0,0,0);
    bt_cmd_pf_avdtp_close( bt_pf_a2dp_unit.int_id,
                           bt_pf_a2dp_unit.handle,
                           NULL );
  }
  else if( bt_pf_a2dp_unit.state == BT_PF_A2DP_SS_RECONFIG_SENT  ||
           bt_pf_a2dp_unit.state == BT_PF_A2DP_SS_RECONFIGURING  ||
           bt_pf_a2dp_unit.state == BT_PF_A2DP_SS_START_SENT     ||
           bt_pf_a2dp_unit.state == BT_PF_A2DP_SS_SUSPENDING     ||
           bt_pf_a2dp_unit.state == BT_PF_A2DP_SS_CLOSE_FRM_OPEN ||
           bt_pf_a2dp_unit.state == BT_PF_A2DP_SS_ABORTING       ||
           bt_pf_a2dp_unit.state == BT_PF_A2DP_SS_CLOSE_FRM_STREAMING )
  {
    /* Don't do anything here */
    BT_MSG_DEBUG("BT PF A2DP: Discon pending state transition; ss %x sss %x",
                 bt_pf_a2dp_unit.state,
                 bt_pf_a2dp_unit.substate, 0);
  }
  else
  {
    // Just disconnect
    BT_MSG_DEBUG("BT PF A2DP CMD TX: disconnect",0,0,0);
    if( bt_pf_a2dp_unit.discon_reason == BT_CS_PF_A2DP_ERR_A2DP_NONE )
    {
      bt_pf_a2dp_unit.discon_reason = BT_CS_PF_A2DP_NORMAL_DISCONNECT;
    }
    bt_cmd_pf_avdtp_disconnect( bt_pf_a2dp_unit.int_id,
                                &bt_pf_a2dp_unit.bd_addr );
  }

  cmd_ptr->cmd_hdr.cmd_status = BT_CS_GN_SUCCESS;
}

/*===========================================================================

FUNCTION
bt_pf_a2dp_cmd_start

DESCRIPTION
Setup streaming audio data with the remote device. This will either
finalize the A2DP configuration, or reconfigure the stream to conform
to the options given in the command.

===========================================================================*/
LOCAL void bt_pf_a2dp_cmd_start( bt_pf_cmd_msg_type *cmd_ptr )
{
  boolean config_changed = FALSE;
  boolean data_valid = 
    BT_PF_A2DP_SBC_DATA_IS_VALID( cmd_ptr->cmd_msg.cmd_a2dp_start.sbc_data );

  BT_MSG_API( "BT PF A2DP CMD RX: start aid %x data %d ss|sss %08x",
              cmd_ptr->cmd_hdr.app_id, data_valid,
              (bt_pf_a2dp_unit.state << 16) | bt_pf_a2dp_unit.substate );

  if( data_valid )
  {
    if( memcmp( (byte*)&bt_pf_a2dp_unit.pending_sbc_config,
                (byte*)&cmd_ptr->cmd_msg.cmd_a2dp_start.sbc_data,
                sizeof( audiosbcenc_data_type ) ) != 0 )
    {
      config_changed = TRUE;
      bt_pf_a2dp_unit.pending_sbc_config = 
        cmd_ptr->cmd_msg.cmd_a2dp_start.sbc_data;
    }
    if( (bt_pf_a2dp_unit.app_ctl_start != FALSE)           &&
        (bt_pf_a2dp_unit.state != BT_PF_A2DP_SS_STREAMING) &&
        (bt_pf_a2dp_unit.substate != BT_PF_A2DP_SSS_ATTEMPT_TO_STREAM) )
    {
      // The app is controlling start. So don't start
      // when we have a valid SBC config.
      // However, if we're currently streaming (or trying to stream)
      // process the start command anyway. This will insure that
      // a START event is sent to AVS when necessary.
      return;
    }
  }

  if( bt_pf_a2dp_unit.substate == BT_PF_A2DP_SSS_ATTEMPT_TO_DISABLE ||
      bt_pf_a2dp_unit.substate == BT_PF_A2DP_SSS_ATTEMPT_TO_DISCONNECT )
  {
    BT_MSG_HIGH("BT PF A2DP: rx start command when attempting to disconnect",
                0, 0, 0);
    cmd_ptr->cmd_hdr.cmd_status = BT_CS_PF_INVALID_STATE;
  }
  else if( bt_pf_a2dp_unit.role == BT_PF_A2DP_ROLE_ACP )
  {
    bt_pf_a2dp_set_substate( BT_PF_A2DP_SSS_ATTEMPT_TO_STREAM );

    if( BT_PF_A2DP_SBC_DATA_IS_VALID( bt_pf_a2dp_unit.pending_sbc_config ) )
    {
      rex_clr_timer( &bt_pf_a2dp_unit.acp_timer );
      bt_pf_a2dp_abort_stream();
    }
  }
  else if ( bt_pf_a2dp_unit.state == BT_PF_A2DP_SS_STREAMING )
  {
    if( bt_pf_a2dp_unit.substate == BT_PF_A2DP_SSS_RECONFIGURING )
    {
      // nothing
    } 
    else if ( data_valid == FALSE )
    {
      bt_pf_a2dp_start_streaming( BT_PF_A2DP_EVENT_LOCAL_INIT );
    }
    else
    {
      // already connected
      // Compare negotiated stream states first:
      if( memcmp( (byte*)&bt_pf_a2dp_unit.sbc_config,
                  (byte*)&cmd_ptr->cmd_msg.cmd_a2dp_start.sbc_data,
                  sizeof( audiosbcenc_data_type ) ) == 0 )
      {
        bt_pf_a2dp_start_streaming( BT_PF_A2DP_EVENT_LOCAL_INIT );
      }
      else
      {
        // Need to renegotiate stream params
        // Save the SBC info:

        if( bt_pf_a2dp_unit.substate == BT_PF_A2DP_SSS_DISCOVERING )
        {
          /* This will occur if we're discovering endpoints because the remote
           * just connected, then we received a start command.
           */
          bt_pf_a2dp_set_substate( BT_PF_A2DP_SSS_DISC_THEN_STREAM );
        }
        else if( bt_pf_a2dp_unit.substate == BT_PF_A2DP_SSS_DISC_THEN_STREAM )
        {
          // Nothing to do here.
        }
        else
        {
          bt_pf_a2dp_set_substate( BT_PF_A2DP_SSS_RECONFIGURING );
          if( bt_pf_a2dp_unit.recfg_option == BT_PF_A2DP_RECONFIGURE )
          {
            bt_pf_a2dp_unit.state = BT_PF_A2DP_SS_SUSPENDING;
            bt_cmd_pf_avdtp_suspend( bt_pf_a2dp_unit.int_id,
                                     bt_pf_a2dp_unit.handle,
                                     NULL );
          }
          else
          {
            /* Must close the stream to reconfigure */
            bt_pf_a2dp_close_stream( BT_PF_A2DP_SS_CLOSE_FRM_STREAMING, 
			                         BT_CS_PF_A2DP_ERR_ATTEMPTED_TO_CLOSE_AND_OPEN_STREAM );
          }
        }
      }
    }
  }
  else if( bt_pf_a2dp_unit.state == BT_PF_A2DP_SS_START_SENT )
  {
    // We're currently starting the stream.
    // If the stream states are the same, do nothing.
    // If the stream state is different, must reconfigure later
    bt_pf_a2dp_set_substate( BT_PF_A2DP_SSS_ATTEMPT_TO_STREAM );

  }
  else if( bt_pf_a2dp_unit.state == BT_PF_A2DP_SS_OPEN )
  {
    // Compare negotiated stream states first:
    if( memcmp( (byte*)&bt_pf_a2dp_unit.sbc_config,
                (byte*)&bt_pf_a2dp_unit.pending_sbc_config,
                sizeof( audiosbcenc_data_type ) )
        == 0 )
    {
      // Negotiated stream is currently up. Just start.

      bt_pf_a2dp_unit.state = BT_PF_A2DP_SS_START_SENT;
      bt_cmd_pf_avdtp_start( bt_pf_a2dp_unit.int_id,
                             bt_pf_a2dp_unit.handle,
                             NULL );
      bt_pf_a2dp_set_substate( BT_PF_A2DP_SSS_NONE );
    }
    else
    {
      if( bt_pf_a2dp_unit.substate == BT_PF_A2DP_SSS_DISCOVERING )
      {
        /* This will occur if we're discovering endpoints because the remote
         * just connected, then we received a start command.
         */
        bt_pf_a2dp_set_substate( BT_PF_A2DP_SSS_DISC_THEN_STREAM ); 
      }
      else if( bt_pf_a2dp_unit.substate == BT_PF_A2DP_SSS_DISC_THEN_STREAM )
      {
        // nothing to do here.
      }
      else if( bt_pf_a2dp_unit.recfg_option == BT_PF_A2DP_RECONFIGURE )
      {
        // Need to renegotiate stream parameters.
        bt_pf_avdtp_cap_type cap[1];
        
        bt_pf_a2dp_unit.role     = BT_PF_A2DP_ROLE_INT;
        bt_pf_a2dp_unit.state    = BT_PF_A2DP_SS_RECONFIG_SENT;
        bt_pf_a2dp_set_substate( BT_PF_A2DP_SSS_ATTEMPT_TO_STREAM );
        rex_clr_timer( &bt_pf_a2dp_unit.acp_timer );
        
        cap[0].type                          = BT_PF_AVDTP_CAP_CODEC;
        cap[0].data.codec_data.media_type    = BT_PF_AVDTP_MEDIA_TYPE_AUDIO;
        cap[0].data.codec_data.codec_type    = BT_PF_AVDTP_CODEC_SBC;
        cap[0].data.codec_data.data.sbc_data = bt_pf_a2dp_unit.pending_sbc_config;
        
        BT_MSG_DEBUG("BT PF A2DP CMD TX: reconfigure",0,0,0);
        bt_cmd_pf_avdtp_reconfigure( bt_pf_a2dp_unit.int_id,
                                     bt_pf_a2dp_unit.handle,
                                     cap,
                                     1,
                                     NULL );
      }
      else // reconfig option is not BT_PF_A2DP_RECONFIGURE
      {
        // Many headsets seem not to support reconfiguring.
        // Close the stream and open it again.
        BT_MSG_DEBUG("BT PF A2DP CMD TX: close -- attempt to reopen",0,0,0);
        bt_pf_a2dp_close_stream( BT_PF_A2DP_SS_CLOSE_FRM_OPEN, 
			                     BT_CS_PF_A2DP_ERR_ATTEMPTED_TO_CLOSE_AND_OPEN_STREAM );
        /* make sure we try again */
        bt_pf_a2dp_set_substate( BT_PF_A2DP_SSS_ATTEMPT_TO_STREAM );
      }
    }
  }
  else if ( (bt_pf_a2dp_unit.state == BT_PF_A2DP_SS_RECONFIG_SENT) ||
            (bt_pf_a2dp_unit.state == BT_PF_A2DP_SS_CONFIG_SENT) )
  {
    bt_pf_a2dp_set_substate( BT_PF_A2DP_SSS_ATTEMPT_TO_STREAM );

    // A set config has already been sent, make sure it's the
    // right one:
    if( config_changed != FALSE )
    {
      if(bt_pf_a2dp_unit.state == BT_PF_A2DP_SS_RECONFIG_SENT)
      {
        bt_pf_a2dp_unit.state = BT_PF_A2DP_SS_RECONFIGURING;
      }
      else
      {
        bt_pf_a2dp_unit.state = BT_PF_A2DP_SS_CONFIGURING;
      }
    }
  }
  else if ( bt_pf_a2dp_unit.state == BT_PF_A2DP_SS_CONFIGURING )
  {
    if( BT_PF_A2DP_SBC_DATA_IS_VALID(bt_pf_a2dp_unit.pending_sbc_config) )
    {
      bt_pf_avdtp_cap_type cap[2];
   
      cap[0].type                          = BT_PF_AVDTP_CAP_MEDIA_TRANSPORT;
      cap[0].data.generic_data.data        = NULL;
      cap[0].data.generic_data.data_len    = 0;
   
      cap[1].type = BT_PF_AVDTP_CAP_CODEC;
      cap[1].data.codec_data.media_type = BT_PF_AVDTP_MEDIA_TYPE_AUDIO;
      cap[1].data.codec_data.codec_type = BT_PF_AVDTP_CODEC_SBC;
      cap[1].data.codec_data.data.sbc_data = bt_pf_a2dp_unit.pending_sbc_config;
   
      BT_MSG_DEBUG( "BT PF A2DP CMD TX: set config, seid %x",
                    bt_pf_a2dp_unit.current_seid, 0, 0 );
      bt_pf_a2dp_unit.state = BT_PF_A2DP_SS_CONFIG_SENT;
      bt_cmd_pf_avdtp_set_config( bt_pf_a2dp_unit.int_id,
                                  &bt_pf_a2dp_unit.bd_addr,
                                  bt_pf_a2dp_unit.current_seid,
                                  bt_pf_a2dp_unit.local_seid,
                                  cap,
                                  2,
                                  NULL,
                                  &bt_pf_a2dp_unit.handle);
    }
    bt_pf_a2dp_set_substate( BT_PF_A2DP_SSS_ATTEMPT_TO_STREAM );
  }
  else if( bt_pf_a2dp_unit.state == BT_PF_A2DP_SS_RECONFIGURING )
  {
    // Need to renegotiate stream parameters.
    bt_pf_avdtp_cap_type cap[1];
    
    if( BT_PF_A2DP_SBC_DATA_IS_VALID(bt_pf_a2dp_unit.pending_sbc_config) )
    {
      bt_pf_a2dp_unit.role     = BT_PF_A2DP_ROLE_INT;
      bt_pf_a2dp_unit.state    = BT_PF_A2DP_SS_RECONFIG_SENT;
      bt_pf_a2dp_set_substate( BT_PF_A2DP_SSS_ATTEMPT_TO_STREAM );
      rex_clr_timer( &bt_pf_a2dp_unit.acp_timer );
      
      cap[0].type                          = BT_PF_AVDTP_CAP_CODEC;
      cap[0].data.codec_data.media_type    = BT_PF_AVDTP_MEDIA_TYPE_AUDIO;
      cap[0].data.codec_data.codec_type    = BT_PF_AVDTP_CODEC_SBC;
      cap[0].data.codec_data.data.sbc_data = bt_pf_a2dp_unit.pending_sbc_config;
      
      BT_MSG_DEBUG("BT PF A2DP CMD TX: reconfigure",0,0,0);
      bt_cmd_pf_avdtp_reconfigure( bt_pf_a2dp_unit.int_id,
                                   bt_pf_a2dp_unit.handle,
                                   cap,
                                   1,
                                   NULL );
    }
  }

  else if( bt_pf_a2dp_unit.state == BT_PF_A2DP_SS_SUSPENDING ||
           bt_pf_a2dp_unit.state == BT_PF_A2DP_SS_CONFIGURED ||
           bt_pf_a2dp_unit.state == BT_PF_A2DP_SS_CLOSE_FRM_OPEN ||
           bt_pf_a2dp_unit.state == BT_PF_A2DP_SS_CLOSE_FRM_STREAMING ||
           bt_pf_a2dp_unit.state == BT_PF_A2DP_SS_ABORTING )
  {
    bt_pf_a2dp_set_substate( BT_PF_A2DP_SSS_ATTEMPT_TO_STREAM );
  }
  else if ( BT_BD_ADDRS_EQUAL( &bt_pf_a2dp_unit.bd_addr, 
                               &bt_pf_a2dp_null_bd_addr ) == FALSE )
  {
    bt_pf_cmd_msg_type cmd;
    cmd.cmd_msg.cmd_a2dp_connect.bd_addr = bt_pf_a2dp_unit.bd_addr;
    if( BT_PF_A2DP_SBC_DATA_IS_VALID(bt_pf_a2dp_unit.pending_sbc_config) )
    {
      bt_pf_a2dp_set_substate( BT_PF_A2DP_SSS_ATTEMPT_TO_STREAM );
    }
    bt_pf_a2dp_cmd_connect( &cmd );
  }
  else
  {
    BT_MSG_HIGH("BT PF A2DP: Start cmd in invalid state, ss %x sss %x",
                bt_pf_a2dp_unit.state,
                bt_pf_a2dp_unit.substate, 0);
    cmd_ptr->cmd_hdr.cmd_status = BT_CS_PF_INVALID_STATE;
  }
}

/*===========================================================================

FUNCTION
bt_pf_a2dp_cmd_suspend

DESCRIPTION
Suspends a started audio stream.

===========================================================================*/
LOCAL void bt_pf_a2dp_cmd_suspend( bt_pf_cmd_msg_type *cmd_ptr )
{
  BT_MSG_API("BT PF A2DP CMD RX: suspend aid %x ss|sss %08x",
             cmd_ptr->cmd_hdr.app_id,
             (bt_pf_a2dp_unit.state << 16) | bt_pf_a2dp_unit.substate,0);

  if( rex_get_timer( &bt_pf_a2dp_unit.pending_data_timer ) != 0 )
  {
    rex_clr_timer( &bt_pf_a2dp_unit.pending_data_timer );
    bt_pf_a2dp_pending_data_timeout_cb( 0 );
  }

  if( bt_pf_a2dp_unit.state == BT_PF_A2DP_SS_STREAMING )
  {
    bt_pf_a2dp_set_substate( BT_PF_A2DP_SSS_NONE );
    if( bt_pf_a2dp_unit.suspend_on_end )
    {
      bt_pf_a2dp_unit.state = BT_PF_A2DP_SS_SUSPENDING;
      bt_cmd_pf_avdtp_suspend( bt_pf_a2dp_unit.int_id,
                               bt_pf_a2dp_unit.handle,
                               NULL );
    }
    else
    {
      // close on end
      bt_pf_a2dp_close_stream( BT_PF_A2DP_SS_CLOSE_FRM_STREAMING, 
			                   BT_CS_PF_A2DP_ERR_ATTEMPTED_TO_CLOSE_AND_OPEN_STREAM );
    }
  }
  else if( bt_pf_a2dp_unit.state    == BT_PF_A2DP_SS_CLOSE_FRM_OPEN      ||
           bt_pf_a2dp_unit.state    == BT_PF_A2DP_SS_CLOSE_FRM_STREAMING ||
           bt_pf_a2dp_unit.state    == BT_PF_A2DP_SS_START_SENT          ||
           bt_pf_a2dp_unit.substate == BT_PF_A2DP_SSS_ATTEMPT_TO_STREAM  ||
           bt_pf_a2dp_unit.substate == BT_PF_A2DP_SSS_RECONFIGURING      ||
           bt_pf_a2dp_unit.substate == BT_PF_A2DP_SSS_ATTEMPT_TO_SUSPEND )
  {
    if( bt_pf_a2dp_unit.substate != BT_PF_A2DP_SSS_ATTEMPT_TO_DISCONNECT &&
        bt_pf_a2dp_unit.substate != BT_PF_A2DP_SSS_ATTEMPT_TO_DISABLE )
    {
      bt_pf_a2dp_set_substate( BT_PF_A2DP_SSS_ATTEMPT_TO_SUSPEND );
    }
  }
  else
  {
    // Send a suspend event back to app which sent the suspend cmd.
    bt_ev_msg_type event;
    event.ev_hdr.bt_app_id              = cmd_ptr->cmd_hdr.app_id;
    event.ev_hdr.ev_type                = BT_EV_PF_A2DP_SUSPEND;
    event.ev_msg.ev_a2dp_suspend.origin = BT_PF_A2DP_EVENT_LOCAL_INIT;
    event.ev_msg.ev_a2dp_suspend.bd_addr = bt_pf_a2dp_unit.bd_addr;

    BT_MSG_DEBUG("BT PF A2DP TX EV: suspend. AppID: %x, origin: %x",
                 bt_pf_a2dp_unit.ext_id, BT_PF_A2DP_EVENT_LOCAL_INIT, 0);
    bt_ec_send_event( &event );
  }
}

/*===========================================================================

FUNCTION
bt_pf_a2dp_cmd_send_data

DESCRIPTION
Sends SBC data to a streaming A2DP device.

Discards messages if fragmentation needed.
Assumes that all data will fit in one L2Cap MTU.
Does not handle fragmentation if one SBC frame doesn't fit.

===========================================================================*/
LOCAL void bt_pf_a2dp_cmd_send_data( bt_pf_cmd_msg_type *cmd_ptr )
{
  bt_cmd_pf_a2dp_send_data_type *cmd = &cmd_ptr->cmd_msg.cmd_a2dp_send_data;
  dsm_item_type *orig_ptr = cmd->data;
#ifdef BT_TEST_PYLD_FAST_TX
  bt_pf_cmd_msg_type bt_cmd;
#endif /* BT_TEST_PYLD_FAST_TX */
#ifdef FEATURE_BT_AVSYNC
  bt_pf_a2dp_frame_handle_type  *a2dp_frame_handle=NULL;
#endif /* FEATURE_BT_AVSYNC */

  rex_enter_crit_sect( &bt_pf_a2dp_unit.audio_crit_sect );

  if( bt_pf_a2dp_unit.state == BT_PF_A2DP_SS_STREAMING )
  {
    unsigned char a2dp_header;

    rex_clr_timer( &bt_pf_a2dp_unit.streaming_timer );

    a2dp_header =
      cmd->num_frames << BT_PF_A2DP_HDR_NUM_FRAMES_OFFSET ;

    dsm_pushdown_packed( &orig_ptr,
                         &a2dp_header,
                         1,
                         DSM_DS_SMALL_ITEM_POOL );

#ifdef FEATURE_BT_AVSYNC
    if(cmd->last_frame)
    {
      a2dp_frame_handle = q_get(&a2dp_free_frame_handle_q);
#ifdef FEATURE_BT_AVSYNC_DEBUG
      BT_MSG_DEBUG("BT PF A2DP: AVS Requesting Ack for seq_num=0x%x",
                    cmd->seq_num + cmd->num_frames, 0, 0);
#endif
      if(a2dp_frame_handle != NULL)
      {
        a2dp_frame_handle->a2dp_last_seq = cmd->seq_num + cmd->num_frames;
        a2dp_frame_handle->last_frame = cmd->last_frame;
#ifdef FEATURE_BT_AVSYNC_DEBUG
        /* Store the time of Transmission */
        clk_read_ms(a2dp_frame_handle->sent_timestamp);
#endif
        q_put(&a2dp_frame_handle_q, &a2dp_frame_handle->link);
      }
      else
      {
        BT_ERR("BT PF A2DP: Ran out of free a2dp frame handles", 0, 0, 0);
      }
    }
    else
    {
    /* Only ask for tx cfm for the required frames that needs to be 
     * acked to AVS, for the others we still need to pass some 
     * non NULL value so that it is identified as a flushable 
     * packet
     */
      a2dp_frame_handle = BT_INVALID_ADDRESS;
    }
#endif /* FEATURE_BT_AVSYNC */

#ifdef BT_TEST_PYLD_FAST_TX
    bt_cmd.cmd_hdr.cmd_type = BT_PF_CMD_AVDTP_SEND_DATA;
    bt_cmd.cmd_hdr.app_id   = bt_pf_a2dp_unit.int_id;
    bt_cmd.cmd_msg.cmd_avdtp_send_data.handle       = bt_pf_a2dp_unit.handle;
    bt_cmd.cmd_msg.cmd_avdtp_send_data.seq_num      = ++bt_pf_a2dp_unit.seq_num;
    bt_cmd.cmd_msg.cmd_avdtp_send_data.timestamp    = bt_pf_a2dp_unit.pcm_timestamp;
    bt_cmd.cmd_msg.cmd_avdtp_send_data.payload_type = BT_PF_A2DP_PAYLOAD_TYPE;
    bt_cmd.cmd_msg.cmd_avdtp_send_data.marker       = 0;
    bt_cmd.cmd_msg.cmd_avdtp_send_data.data         = orig_ptr;
#ifdef FEATURE_BT_AVSYNC
    bt_cmd.cmd_msg.cmd_avdtp_send_data.frame_handle = a2dp_frame_handle;
#else
    bt_cmd.cmd_msg.cmd_avdtp_send_data.frame_handle = NULL;
#endif /* FEATURE_BT_AVSYNC */
    bt_pf_avdtp_cmd_send_data( &bt_cmd );
#else
    bt_cmd_pf_avdtp_send_data( bt_pf_a2dp_unit.int_id,
                               bt_pf_a2dp_unit.handle,
                               ++bt_pf_a2dp_unit.seq_num,
                               cmd->timestamps_per_frame * cmd->seq_num,
                               BT_PF_A2DP_PAYLOAD_TYPE, /* Payload Type */
                               0, /* Marker */
                               orig_ptr,
#ifdef FEATURE_BT_AVSYNC
                               (void *)a2dp_frame_handle );
#else
                               NULL);
#endif /* FEATURE_BT_AVSYNC */
#endif /* else BT_TEST_PYLD_FAST_TX */
    bt_pf_a2dp_unit.pcm_timestamp += cmd->timestamps_per_frame * cmd->num_frames;
  }
  else
  {
    dsm_free_packet( &orig_ptr );
    cmd_ptr->cmd_hdr.cmd_status = BT_CS_PF_INVALID_STATE;
  }
  rex_leave_crit_sect( &bt_pf_a2dp_unit.audio_crit_sect );
}

/*===========================================================================

FUNCTION
bt_pf_a2dp_cmd_ctl

DESCRIPTION
Controls behavior of A2DP. May be used by the app for testing or interop.

===========================================================================*/
LOCAL void bt_pf_a2dp_cmd_ctl( bt_pf_cmd_msg_type *cmd_ptr )
{
  bt_cmd_pf_a2dp_ctl_type *cmd = &cmd_ptr->cmd_msg.cmd_a2dp_ctl;

  BT_MSG_API("BT PF A2DP CMD RX: aid %x ctl %08x ss|sss %08x", 
             cmd_ptr->cmd_hdr.app_id, 
             (cmd->ctl << 16) | cmd->data1,
             (bt_pf_a2dp_unit.state << 16) | bt_pf_a2dp_unit.substate);

  switch( cmd->ctl )
  {
    case BT_PF_A2DP_CTL_SUSPEND_ON_END:
      bt_pf_a2dp_unit.suspend_on_end = TRUE;
      break;

    case BT_PF_A2DP_CTL_CLOSE_ON_END:
      bt_pf_a2dp_unit.suspend_on_end = FALSE;
      break;

    case BT_PF_A2DP_CTL_HONOR_IN_USE:
      bt_pf_a2dp_unit.honor_in_use = TRUE;
      break;

    case BT_PF_A2DP_CTL_IGNORE_IN_USE:
      bt_pf_a2dp_unit.honor_in_use = FALSE;
      break;

    case BT_PF_A2DP_CTL_RECFG_BY_RECFG:
      bt_pf_a2dp_unit.recfg_option = BT_PF_A2DP_RECONFIGURE;
      break;

    case BT_PF_A2DP_CTL_RECFG_BY_CLOSE:
      bt_pf_a2dp_unit.recfg_option = BT_PF_A2DP_REOPEN_STREAM;
      break;

    case BT_PF_A2DP_CTL_RECFG_BY_REPAGE:
      bt_pf_a2dp_unit.recfg_option = BT_PF_A2DP_REPAGE_DEVICE;
      break;

    case BT_PF_A2DP_CTL_INCR_BITRATE:
      bt_pf_a2dp_unit.auto_bitrate = FALSE;
      if( bt_pf_a2dp_unit.bitrate < BT_PF_A2DP_BITRATE_HIGHEST )
      {
        bt_pf_a2dp_unit.bitrate++;
        bt_pf_a2dp_change_bitrate();
      }
      else
      {
        cmd_ptr->cmd_hdr.cmd_status = BT_CS_PF_A2DP_ALREADY_MAX_BITRATE;
      }
      break;

    case BT_PF_A2DP_CTL_DECR_BITRATE:
      bt_pf_a2dp_unit.auto_bitrate = FALSE;
      if( bt_pf_a2dp_unit.bitrate > BT_PF_A2DP_BITRATE_LOWEST )
      {
        bt_pf_a2dp_unit.bitrate--;
        bt_pf_a2dp_change_bitrate();
      }
      else
      {
        cmd_ptr->cmd_hdr.cmd_status = BT_CS_PF_A2DP_ALREADY_MIN_BITRATE;
      }
      break;

    case BT_PF_A2DP_CTL_SET_BITRATE:
      bt_pf_a2dp_unit.auto_bitrate = FALSE;
      if( bt_pf_a2dp_unit.custom_bitrate != cmd->data1 ||
          bt_pf_a2dp_unit.bitrate != BT_PF_A2DP_BITRATE_CUSTOM )
      {
        bt_pf_a2dp_unit.bitrate = BT_PF_A2DP_BITRATE_CUSTOM;
        bt_pf_a2dp_unit.custom_bitrate = cmd->data1;
        bt_pf_a2dp_change_bitrate();
      }
      break;

    case BT_PF_A2DP_CTL_AUTO_BITRATE:
      bt_pf_a2dp_unit.auto_bitrate = TRUE;
      bt_pf_a2dp_unit.bitrate = BT_PF_A2DP_BITRATE_HIGH;
      // TODO: Recalculate bitrate here if needed
      break;

    case BT_PF_A2DP_CTL_LOWEST_BITRATE:
      bt_pf_a2dp_unit.auto_bitrate = TRUE;
      bt_pf_a2dp_unit.bitrate = BT_PF_A2DP_BITRATE_HIGH;
      bt_pf_a2dp_unit.lowest_bitrate = (bt_pf_a2dp_bitrate_type)cmd->data1;
      // TODO: Recalculate bitrate here if needed
      break;

    case BT_PF_A2DP_CTL_APP_START:
      bt_pf_a2dp_unit.app_ctl_start = TRUE;
      break;
    default:
      BT_ERR("BT PF A2DP: Unrecognized ctl value %x.", cmd->ctl, 0, 0);
      cmd_ptr->cmd_hdr.cmd_status = BT_CS_PF_INVALID_PARAMETERS;
  }
}

/*===========================================================================

FUNCTION
bt_pf_a2dp_cmd_flush

DESCRIPTION
Causes all stored data to be sent immediately.

===========================================================================*/
LOCAL void bt_pf_a2dp_cmd_flush( bt_pf_cmd_msg_type *cmd_ptr )
{
  bt_pf_cmd_msg_type cmd_msg;
  bt_cmd_pf_a2dp_send_data_type *cmd = &cmd_msg.cmd_msg.cmd_a2dp_send_data;

  rex_enter_crit_sect( &bt_pf_a2dp_unit.audio_crit_sect );
  if( bt_pf_a2dp_unit.dsm_ptr != NULL &&
      bt_pf_a2dp_unit.state == BT_PF_A2DP_SS_STREAMING &&
      bt_pf_a2dp_unit.num_frames != 0 &&
      bt_cmd_l2_get_acl_tx_flow_status( bt_pf_a2dp_unit.handle ) == TRUE )
  {
    BT_MSG_DEBUG("BT PF A2DP CMD RX: Flush aid %x, num frames %x",
                 cmd_ptr->cmd_hdr.app_id, bt_pf_a2dp_unit.num_frames, 0);

    cmd->timestamp            = 0; /* currently unused */
    cmd->data                 = bt_pf_a2dp_unit.dsm_ptr;
    cmd->seq_num              = ( bt_pf_a2dp_unit.last_seq_num
                                  - bt_pf_a2dp_unit.num_frames );
    cmd->timestamps_per_frame = bt_pf_a2dp_unit.last_ts_per_frame;
    cmd->num_frames           = bt_pf_a2dp_unit.num_frames;
    cmd->frame_len            = bt_pf_a2dp_unit.last_frame_len;
#ifdef FEATURE_BT_AVSYNC
    cmd->last_frame           = 0;
#endif /* FEATURE_BT_AVSYNC */
#ifdef FEATURE_BT_AVSYNC_DEBUG
    BT_MSG_DEBUG("BT PF A2DP : Flushing Seq no = %d frames = %d", 
                  bt_pf_a2dp_unit.last_seq_num, bt_pf_a2dp_unit.num_frames, 0);
#endif
    bt_pf_a2dp_cmd_send_data( &cmd_msg );
  }
  else
  {
    dsm_free_packet( &bt_pf_a2dp_unit.dsm_ptr );
    BT_ERR("BT PF A2DP: Flush Drp SBC",0,0,0);
  }
  bt_pf_a2dp_unit.dsm_ptr        = NULL;
  bt_pf_a2dp_unit.num_frames     = 0;
  bt_pf_a2dp_unit.last_frame_len = 0;
  rex_leave_crit_sect( &bt_pf_a2dp_unit.audio_crit_sect );
}


/*=========================================================================*/
/*                                                                         */
/*            EXTERNAL FUNCTIONS                                           */
/*                                                                         */
/*=========================================================================*/
/*===========================================================================

FUNCTION
bt_pf_a2dp_sbc_init_cb

DESCRIPTION
Callback provided to the SND task to check status of the SBC encoder
init.

===========================================================================*/
void bt_pf_a2dp_sbc_init_cb( const void *data, snd_status_type status )
{
  if( status != SND_SUCCESS )
  {
    BT_ERR("BT PF A2DP RX CB: sbc init status %x", status, 0, 0);
  }
}

/*===========================================================================

FUNCTION
bt_pf_a2dp_process_command

DESCRIPTION
Processes all incomming commands to the A2DP layer.

===========================================================================*/
void bt_pf_a2dp_process_command( bt_pf_cmd_msg_type* cmd_ptr )
{
#ifdef FEATURE_BT_TIME_DEBUG
  uint64 start_time;
  uint64 now;

  now = time_get_ms_native();
#endif /* FEATURE_BT_TIME_DEBUG */

  cmd_ptr->cmd_hdr.cmd_status = BT_CS_GN_SUCCESS;
  
  switch ( cmd_ptr->cmd_hdr.cmd_type )
  {
    case BT_PF_CMD_A2DP_ENABLE:
      bt_pf_a2dp_cmd_enable( cmd_ptr );
      break;
    case BT_PF_CMD_A2DP_DISABLE:
      bt_pf_a2dp_cmd_disable( cmd_ptr );
      break;
    case BT_PF_CMD_A2DP_SET_DEVICE:
      bt_pf_a2dp_cmd_set_device( cmd_ptr );
      break;
    case BT_PF_CMD_A2DP_CONNECT:
      bt_pf_a2dp_cmd_connect( cmd_ptr );
      break;
    case BT_PF_CMD_A2DP_DISCONNECT:
      bt_pf_a2dp_cmd_disconnect( cmd_ptr );
      break;
    case BT_PF_CMD_A2DP_START:
      bt_pf_a2dp_cmd_start( cmd_ptr );
      break;
    case BT_PF_CMD_A2DP_SUSPEND:
      bt_pf_a2dp_cmd_suspend( cmd_ptr );
      break;
    case BT_PF_CMD_A2DP_SEND_DATA:
      bt_pf_a2dp_cmd_send_data( cmd_ptr );
      break;
    case BT_PF_CMD_A2DP_CTL:
      bt_pf_a2dp_cmd_ctl( cmd_ptr );
      break;
    case BT_PF_CMD_A2DP_FLUSH:
      bt_pf_a2dp_cmd_flush( cmd_ptr );
      break;
    default:
      BT_ERR("BT PF A2DP: Unknown command %x",
             cmd_ptr->cmd_hdr.cmd_type, 0, 0);
      cmd_ptr->cmd_hdr.cmd_status = BT_CS_GN_UNRECOGNIZED_CMD;
  }

#ifdef FEATURE_BT_TIME_DEBUG
  /* Display time delay for command */
  start_time = time_to_ms_native( cmd_ptr->cmd_hdr.cmd_start_time );
  if( (uint32)(now-start_time) > 5 )
  {
    BT_MSG_HIGH("BT PF A2DP CMD Delay: %d", (uint32)(now-start_time),0,0);
  }
#endif /* FEATURE_BT_TIME_DEBUG */
}

/*===========================================================================

FUNCTION
bt_pf_a2dp_sbc_data_cb

DESCRIPTION
This is called periodically by AVS to send SBC frames to the BT stack.
The data must be copied out immediately.

Note: this runs in the task context of the AVS task sending SBC frames.

===========================================================================*/
void bt_pf_a2dp_sbc_data_cb( uint8*     data,
                             uint32     seq_num,
                             uint32     timestamps_per_frame,
                             boolean    last_frame,
                             uint16     frame_len )
{
#ifndef FEATURE_BT_EXTPF_A2DP_VARIABLE_BITRATE
  static boolean error_printed;
#ifdef FEATURE_BT_AVSYNC
  uint64 cfm_seq;
#endif /* FEATURE_BT_AVSYNC */

  if( bt_pf_a2dp_unit.state != BT_PF_A2DP_SS_STREAMING )
  {
    /* A2DP is not in a state to send this audio.
     * Free resources and return. */
    rex_enter_crit_sect( &bt_pf_a2dp_unit.audio_crit_sect );
    dsm_free_packet( &bt_pf_a2dp_unit.dsm_ptr );
    rex_leave_crit_sect( &bt_pf_a2dp_unit.audio_crit_sect );
    return;
  }

  rex_enter_crit_sect( &bt_pf_a2dp_unit.audio_crit_sect );

  if( bt_pf_a2dp_unit.dsm_ptr == NULL )
  {
    bt_pf_a2dp_unit.num_frames     = 0;
    bt_pf_a2dp_unit.last_frame_len = 0;
    error_printed                  = FALSE;
    // Get a DSM item:
    bt_pf_a2dp_unit.dsm_ptr = bt_get_free_dsm_ptr( BT_TL_A2DP,
                                                   bt_pf_a2dp_unit.out_mtu );
  }

  if( bt_pf_a2dp_unit.dsm_ptr )
  {
    if( ( frame_len != bt_pf_a2dp_unit.last_frame_len &&
          bt_pf_a2dp_unit.num_frames != 0 )                         ||
        ( (dsm_length_packet(bt_pf_a2dp_unit.dsm_ptr) + frame_len) >
          (bt_pf_a2dp_unit.out_mtu - BT_PF_A2DP_HEADER_BYTES) ) )
    {
      // The frame size has changed, or frame's too big to fit in an MTU.
      // Send off the packets now.
      if( bt_cmd_pf_a2dp_send_data( bt_pf_a2dp_unit.int_id,
                                    bt_pf_a2dp_unit.dsm_ptr,
                                    bt_pf_a2dp_unit.last_seq_num - 
                                    bt_pf_a2dp_unit.num_frames,
                                    bt_pf_a2dp_unit.last_ts_per_frame,
                                    bt_pf_a2dp_unit.num_frames,
                                    bt_pf_a2dp_unit.last_frame_len,
                                    last_frame,
                                    bt_pf_a2dp_unit.handle)
          != BT_CS_GN_PENDING )
      {
        dsm_free_packet( &bt_pf_a2dp_unit.dsm_ptr );
      }
      else
      {
        rex_clr_timer( &bt_pf_a2dp_unit.pending_data_timer );
      }
#ifdef FEATURE_BT_AVSYNC_DEBUG
      BT_MSG_DEBUG("BT PF A2DP 1: Send Seq nos %d frames = %d", 
                    bt_pf_a2dp_unit.last_seq_num,bt_pf_a2dp_unit.num_frames,0);
#endif

      bt_pf_a2dp_unit.dsm_ptr = NULL;
      bt_pf_a2dp_unit.dsm_ptr = bt_get_free_dsm_ptr( BT_TL_A2DP,
                                                     bt_pf_a2dp_unit.out_mtu );
      bt_pf_a2dp_unit.num_frames     = 0;
      bt_pf_a2dp_unit.last_frame_len = 0;
      error_printed                  = FALSE;
    }
    /* Ahead of time, make sure there's space on the BT watermark
     * and there is free space (atleast 2) in the a2dp Q for AV-Sync 
     */
#ifdef FEATURE_BT_AVSYNC
    if(( bt_cmd_l2_get_acl_tx_flow_status( bt_pf_a2dp_unit.handle ) == TRUE ) &&
       (q_cnt(&a2dp_free_frame_handle_q) >= 2))
#else
    if(( bt_cmd_l2_get_acl_tx_flow_status( bt_pf_a2dp_unit.handle ) == TRUE ))
#endif
    {
      if( bt_dsm_pushdown_tail( &bt_pf_a2dp_unit.dsm_ptr,
                                data, frame_len,
                                DSM_DS_SMALL_ITEM_POOL )
          == FALSE )
      {
        BT_ERR("BT PF A2DP: out of space in DSMs",0,0,0);
        dsm_free_packet( &bt_pf_a2dp_unit.dsm_ptr );
      }
      else
      {
        bt_pf_a2dp_unit.last_frame_len    = frame_len;
        bt_pf_a2dp_unit.last_seq_num      = seq_num;
        bt_pf_a2dp_unit.last_ts_per_frame = timestamps_per_frame;
        bt_pf_a2dp_unit.num_frames++;

        rex_set_timer( &bt_pf_a2dp_unit.pending_data_timer,
                       BT_PF_A2DP_PENDING_DATA_TIMEOUT_MS );
      }
      if(last_frame)
      {
        // If it is a last frame Send off the packets now.
        if( bt_cmd_pf_a2dp_send_data( bt_pf_a2dp_unit.int_id,
                                      bt_pf_a2dp_unit.dsm_ptr,
                                      bt_pf_a2dp_unit.last_seq_num - bt_pf_a2dp_unit.num_frames,
                                      bt_pf_a2dp_unit.last_ts_per_frame,
                                      bt_pf_a2dp_unit.num_frames,
                                      bt_pf_a2dp_unit.last_frame_len,
                                      last_frame,
                                      bt_pf_a2dp_unit.handle)
            != BT_CS_GN_PENDING )
        {
          dsm_free_packet( &bt_pf_a2dp_unit.dsm_ptr );
        }
        else
        {
          rex_clr_timer( &bt_pf_a2dp_unit.pending_data_timer );
        }
#ifdef FEATURE_BT_AVSYNC_DEBUG
        BT_MSG_DEBUG("BT PF A2DP 2: Send Seq nos %d frames = %d", 
                     bt_pf_a2dp_unit.last_seq_num, bt_pf_a2dp_unit.num_frames, 0);
#endif
        bt_pf_a2dp_unit.dsm_ptr = NULL;
      }
    }
    else
    {
      if( error_printed == FALSE )
      {
        BT_ERR("BT PF A2DP: Drp SBC",0,0,0);
        error_printed = TRUE;
      }
#ifdef FEATURE_BT_AVSYNC
      if(last_frame)
      {
         BT_MSG_HIGH(" BT PF A2DP: ACKing dropped frame to AVS for seq = %d "
                     "Free a2dp_frames = %d",
                     seq_num, q_cnt(&a2dp_free_frame_handle_q), 0);
         cfm_seq = ( uint64 )seq_num;
         snd_sbc_avsync(QW_CVT_N2Q(&cfm_seq));
      }
#endif /* FEATURE_BT_AVSYNC */
    }
  }
  rex_leave_crit_sect( &bt_pf_a2dp_unit.audio_crit_sect );

#else /* FEATURE_BT_EXTPF_A2DP_VARIABLE_BITRATE */

  /* Experimental code to try out variable bitrates.
   * Currently needs much work. Is experimental (read,
   * hackish) */

#  define BITRATE_DECRIMENT_FRAMES 15
#  define BITRATE_INCREMENT_FRAMES 500
#  define MAX_HOLD_BUF_CNT         100

  static boolean        a2dp_init = FALSE;
  static boolean        start_of_run;
  static int32          num_frames;
  static uint16         last_frame_len;
  static dsm_item_type *dsm_ptr;
  static uint32         start_seq_num;
  static uint32         decriment_bitrate_seq_num = 0;
  static uint16         bytes_buffered = 0;
  // This is hackish:
  static uint8          device_area = 3;
  static uint16         last_buf_sz = 0;
  static int32          buf_delta[ BT_PF_A2DP_BITRATE_CUSTOM ];
  static uint16         max_hold_buf_sz = 0;
  static uint32         max_hold_buf_cnt = 0;
  static uint32         bytes_sent;

  if( bt_pf_a2dp_unit.state != BT_PF_A2DP_SS_STREAMING )
  {
    a2dp_init = FALSE;
    if( dsm )
    {
      dsm_free_packet( &dsm_ptr );
      dsm = NULL;
    }
    return;
  }

  if( !a2dp_init )
  {
    last_frame_len = frame_len;
    start_seq_num  = seq_num;
    num_frames     = 0;
    dsm            = bt_get_free_dsm_ptr( BT_TL_A2DP, 600 );
    a2dp_init      = TRUE;
  }

  if( start_of_run )
  {
    bytes_sent = 0;
    start_of_run = FALSE;
    // This is hackish.
    // Assume the buffer most full is the one a2dp is running under:
    if( ( (bt_lm_remote_unit[0].qdsp_tx_buf.queued_data_wm.current_cnt >=
           bt_lm_remote_unit[1].qdsp_tx_buf.queued_data_wm.current_cnt)  )  &&
        ( (bt_lm_remote_unit[0].qdsp_tx_buf.queued_data_wm.current_cnt >=
           bt_lm_remote_unit[2].qdsp_tx_buf.queued_data_wm.current_cnt)  )  )
    {
      device_area = 0;
    }
    else if( ( (bt_lm_remote_unit[1].qdsp_tx_buf.queued_data_wm.current_cnt >=
                bt_lm_remote_unit[0].qdsp_tx_buf.queued_data_wm.current_cnt) ) &&
             ( (bt_lm_remote_unit[1].qdsp_tx_buf.queued_data_wm.current_cnt >=
                bt_lm_remote_unit[2].qdsp_tx_buf.queued_data_wm.current_cnt) ) )
    {
      device_area = 1;
    }
    else
    {
      device_area = 2;
    }
    if( bt_pf_a2dp_unit.bitrate != BT_PF_A2DP_BITRATE_CUSTOM )
    {
      buf_delta[ bt_pf_a2dp_unit.bitrate-1 ] =
        bt_lm_remote_unit[device_area].qdsp_tx_buf.queued_data_wm.current_cnt - last_buf_sz;
    }
    last_buf_sz = bt_lm_remote_unit[device_area].qdsp_tx_buf.queued_data_wm.current_cnt;
    if( last_buf_sz > max_hold_buf_sz )
    {
      max_hold_buf_sz = last_buf_sz;
      max_hold_buf_cnt = seq_num;
    }
  }

  if( ((bytes_buffered + frame_len) > (bt_pf_a2dp_unit.out_mtu - BT_PF_A2DP_HEADER_BYTES)) ||
      frame_len != last_frame_len )
  {
    // Send the buffered bytes now.
    if( bt_cmd_pf_a2dp_send_data( bt_pf_a2dp_unit.int_id,
                                  dsm,
                                  seq_num-1 - num_frames,
                                  timestamps_per_frame,
                                  num_frames,
                                  frame_len,
                                  bt_pf_a2dp_unit.handle )
        != BT_CS_GN_PENDING )
    {
      dsm_free_packet( &dsm_ptr );
    }
    dsm            = bt_get_free_dsm_ptr( BT_TL_A2DP, 600 );
    start_seq_num = seq_num;
    num_frames = 0;
    last_frame_len = 0;
    bytes_sent += bytes_buffered;
    bytes_buffered = 0;
  }
  // Ahead of time, make sure there's space on the BT watermark
  dsm_pushdown_tail( &dsm_ptr, data, frame_len, DSM_DS_LARGE_ITEM_POOL );
  bytes_buffered += frame_len;
  last_frame_len = frame_len;
  num_frames ++;

  if( last_frame )
  {
    start_of_run = TRUE;
    if( seq_num - max_hold_buf_cnt > MAX_HOLD_BUF_CNT )
    {
      max_hold_buf_sz = last_buf_sz;
    }

    if( bt_pf_a2dp_unit.auto_bitrate &&
        (start_seq_num - decriment_bitrate_seq_num > BITRATE_INCREMENT_FRAMES) &&
        (bt_pf_a2dp_unit.bitrate < BT_PF_A2DP_BITRATE_HIGHEST) &&
        (bt_lm_remote_unit[ device_area ].qdsp_tx_buf.queued_data_wm.current_cnt == 0) &&
        (buf_delta[ bt_pf_a2dp_unit.bitrate-1 ] == 0 ) &&
        (max_hold_buf_sz == 0) )
    {
      BT_MSG_DEBUG("BT PF A2DP: incr bitrate: delta %x, buf sz %d, dev area %x",
                   buf_delta[ bt_pf_a2dp_unit.bitrate-1 ],
                   bt_lm_remote_unit[ device_area ].qdsp_tx_buf.queued_data_wm.current_cnt,
                   device_area);
      bt_pf_a2dp_unit.bitrate++;
      bt_pf_a2dp_change_bitrate();
      decriment_bitrate_seq_num = start_seq_num;
    }
    else if( bt_pf_a2dp_unit.auto_bitrate != FALSE &&
             bt_pf_a2dp_unit.bitrate != BT_PF_A2DP_BITRATE_CUSTOM )
    {
      if( (int32)(buf_delta[ bt_pf_a2dp_unit.bitrate-1 ] * (int32)bt_pf_a2dp_unit.bitrate * 2) >
          (int32)
          ( bt_lm_remote_unit[ device_area ].qdsp_tx_buf.queued_data_wm.hi_watermark
            - bt_lm_remote_unit[ device_area ].qdsp_tx_buf.queued_data_wm.current_cnt
            - bytes_sent
            - bytes_buffered ) )
      {
        if( (start_seq_num - decriment_bitrate_seq_num > BITRATE_DECRIMENT_FRAMES) &&
            (bt_pf_a2dp_unit.bitrate >= bt_pf_a2dp_unit.lowest_bitrate ) )
        {
          decriment_bitrate_seq_num = start_seq_num;
          BT_MSG_DEBUG("BT PF A2DP: decr bitrate: delta %x, buf sz %d, dev area %x",
                       buf_delta[ bt_pf_a2dp_unit.bitrate-1 ],
                       bt_lm_remote_unit[ device_area ].qdsp_tx_buf.queued_data_wm.current_cnt
                       + bytes_sent + bytes_buffered,
                       device_area );
          bt_pf_a2dp_unit.bitrate--;
          bt_pf_a2dp_change_bitrate();
          max_hold_buf_sz = last_buf_sz;
          max_hold_buf_cnt = seq_num;
        }
      }
    }
  }
#endif /* FEATURE_BT_EXTPF_A2DP_VARIABLE_BITRATE */
}

/*===========================================================================

FUNCTION
bt_pf_a2dp_init

DESCRIPTION
One time initialization at boot time.

===========================================================================*/
extern void bt_pf_a2dp_init( void )
{
#ifdef FEATURE_BT_AVSYNC
  int i;
#endif /* FEATURE_BT_AVSYNC */
  (void) q_init( &bt_pf_a2dp_ev_q );
  bt_pf_a2dp_ev_q_info.event_q_ptr      = &bt_pf_a2dp_ev_q;
  bt_pf_a2dp_ev_q_info.max_q_depth      = BT_PF_A2DP_NUM_EVENTS;
  bt_pf_a2dp_ev_q_info.pf_task_sig      = BT_PF_A2DP_EV_SIG;
  bt_pf_a2dp_ev_q_info.max_event_bytes  = bt_pf_a2dp_get_max_event_bytes();

#ifdef FEATURE_BT_AVSYNC
  (void) q_init( &a2dp_free_frame_handle_q);
  (void) q_init( &a2dp_frame_handle_q );
  for (i=0; i< MAX_A2DP_FRAMES; i++)
  {
    q_link_type   *link_item;
    link_item = q_link(&a2dp_free_frame_handle[i], 
                &a2dp_free_frame_handle[i].link);
    q_put(&a2dp_free_frame_handle_q, link_item);
  }
#endif /* FEATURE_BT_AVSYNC */
  memset( &bt_pf_a2dp_unit, 0, sizeof( bt_pf_a2dp_unit_type ) );

  bt_pf_a2dp_unit.int_id = BT_APP_ID_NULL;
  bt_pf_a2dp_unit.avs_id = BT_APP_ID_NULL;
  bt_pf_a2dp_unit.discon_reason = BT_CS_PF_A2DP_ERR_A2DP_NONE;

  rex_def_timer_ex( &bt_pf_a2dp_unit.acp_timer,
                    bt_pf_a2dp_acp_timeout_cb,
                    BT_PF_A2DP_ACP_TIMEOUT_MS );

  rex_def_timer_ex( &bt_pf_a2dp_unit.streaming_timer,
                    bt_pf_a2dp_streaming_timeout_cb,
                    BT_PF_A2DP_STREAMING_TIMEOUT_MS );

  rex_def_timer_ex( &bt_pf_a2dp_unit.pending_data_timer,
                    bt_pf_a2dp_pending_data_timeout_cb,
                    BT_PF_A2DP_PENDING_DATA_TIMEOUT_MS );

  rex_init_crit_sect( &bt_pf_a2dp_unit.audio_crit_sect );
#ifndef FEATURE_BT_SOC
  snd_sbc_init( bt_pf_a2dp_sbc_data_cb, bt_pf_a2dp_sbc_init_cb, NULL );
#endif
}
#endif /* FEATURE_BT_EXTPF_AV */
#endif /* FEATURE_BT */
