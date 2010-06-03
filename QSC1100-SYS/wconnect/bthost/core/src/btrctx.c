/*====*====*====*====*====*====*====*====*====*====*====*====*====*====*====*

          B L U E T O O T H    R F C O M M    T R A N S M I T

GENERAL DESCRIPTION
  This file contains the Bluetooth code that transmits messages for RFCOMM.

Copyright (c) 2000-2008 QUALCOMM Incorporated.
All Rights Reserved.
Qualcomm Confidential and Proprietary
*====*====*====*====*====*====*====*====*====*====*====*====*====*====*====*/

/*===========================================================================

                        EDIT HISTORY FOR MODULE

  This section contains comments describing changes made to the module.
  Notice that changes are listed in reverse chronological order.

  $Header: //source/qcom/qct/wconnect/bthost/core/rel/00.00.26/src/btrctx.c#1 $ 
  $DateTime: 2009/01/07 18:14:54 $
  $Author: deepikas $

  when        who  what, where, why
  ----------  ---  -----------------------------------------------------------
  2008-04-03   NM  Fix the issue of wrong FCS sent to remote in RPN Command,
                   MSC Command and MSC response to remote device
  2008-03-27   pn  Pacified Klocwork.

 *
 *    #18        27 Mar 2007            JN
 * Fix revision history for version #17.
 *
 *    #17        27 Mar 2007            JN
 * Fixed misaligned memory access in bt_rc_tx_mcc_test_response 
 * when source pointer is NULL. 
 *
 *    #16        21 Feb 2007            PN
 * Fixed critical lint errors.
 *
 *    #15        28 Jul 2006            DM
 * Changed bt_cmd_l2_write interface to pass NULL frame_handle
 *
 *    #14        5  Apr 2006            DP
 * Fix the data incrementing feature of the RC Ping Command.
 *
 *    #13        05 Dec 2005            DSN
 * Fixed RVCT compile error. 
 * 
 *    #12        1  Dec 2005            DP
 * Changed CBFC to support cases where there is no RX watermark by only
 * allowing one outstanding transmit credit at a time to the remote side.
 * This is known to apply for Circuit Switched (CS) data calls.
 *
 *    #10-11     4  Nov 2005            DP
 * Removed critical Lint errors.
 *
 *    #9         12 Sep 2005            JTL
 * Improving DSM allocation.
 *
 *    #8         11 May 2005            DP
 * Removed critical Lint error.
 *
 *    #7         8  Apr 2005            DP
 * Make use of BT_BDA debug message macro.
 *
 *    #6         7  Apr 2005            DP
 * Don't allow CBFC to stall flow when RX watermark don't exceed count
 * is too small.
 *
 *    #5         16 Mar 2005            DP
 * Made initial CBFC credits in PN message dynamic.
 *
 *    #4         8  Mar 2005            DP
 * Improved CBFC so it now uses actual space available in receive watermark
 * to determine number of transmit credits to give remote device.
 * 
 *    Rev 1.11   20 Feb 2002 09:33:26   waynelee
 * 
 *    Rev 1.10   23 Oct 2001 16:52:12   propach
 * Added support for RFCOMM Credit Based Flow Control (CBFC).
 * 
 *    Rev 1.9   29 Aug 2001 11:56:54   propach
 * Merged certification tip onto main tip.
 * 
 *    Rev 1.8.1.3   18 Jul 2001 16:52:18   propach
 * Fixed responses to RPN requests.
 * 
 *    Rev 1.8.1.2   17 Jul 2001 16:10:40   propach
 * Fixed RC TX signaling logging.
 * 
 *    Rev 1.8.1.1   12 Jul 2001 16:06:02   waynelee
 * Use dsm_pushdown_tail() to append the FCS for UIH frames.  This
 * function will properly add the FCS byte for dup dsms.  (Fixes 17569)
 * 
 *    Rev 1.8   08 Jun 2001 15:05:34   propach
 * Added RFCOMM link negotiations and modem status lines for SPP certification.
 * 
 *    Rev 1.7   23 May 2001 19:37:34   amitj
 * Made a small fix in bt_rc_send_payload_tx_log()
 * 
 *    Rev 1.6   23 May 2001 18:52:18   amitj
 * Now allocating only the required amount of memory for QXDM sig and
 * data logs in logging functions. Makes interpreting easier.
 * 
 *    Rev 1.5   26 Mar 2001 18:16:14   propach
 * Added RC commands and events to support serial port emulation settings
 * and modem status reading/writing.
 * 
 *    Rev 1.4   16 Mar 2001 17:28:42   propach
 * Added infrastructure for SPP including initial support of
 * RPN and RLS and reworking PN signaling messages.
 * 
 *    Rev 1.3   02 Mar 2001 12:03:40   propach
 * Removed unused init/online/offline routines.  Added TX NSC response.
 * Updated TX PN response for 1.1.
 * 
 *    Rev 1.2   26 Feb 2001 12:39:32   propach
 * Updated RC flow control.  Updated MSC and test cmd/resp.  Improved
 * QXDM message outputs.
 * 
 *    Rev 1.1   16 Feb 2001 16:11:48   propach
 * Completed EC routing of events by BT application ID.
 * 
 *    Rev 1.0   29 Jan 2001 17:09:40   propach
 * Initial revision.
 * 
 *    Rev 1.31   21 Dec 2000 17:47:28   waynelee
 * Use BT_SET_PKT_TRACER().  Change semantic of 
 * BT_TEST_PYLD_FAST to what the name says.
 * 
 *    Rev 1.30   13 Dec 2000 17:00:44   amitj
 * Replaced bt_rc_sig_tx() with bt_rc_sig_tx_logging_function() calls
 * 
 *    Rev 1.29   11 Dec 2000 18:59:54   amitj
 * minor format changes
 * 
 *    Rev 1.28   11 Dec 2000 17:19:58   amitj
 * Replaced hardcoded values with literals in logging_function()s; Renamed
 * the logging_function()s; Removed while loop and bt_cmd_l2_write() in
 * bt_rc_sig_tx_logging_function()
 * 
 *    Rev 1.27   07 Dec 2000 12:23:24   amitj
 * Made a fix to log RC PYLD TX data whether or not BT_TEST_PYLD_FAST is defined
 * 
 *    Rev 1.26   07 Dec 2000 09:58:16   waynelee
 * Removed compile warnings
 * 
 *    Rev 1.25   01 Dec 2000 17:32:38   propach
 * UPF-4 fix:  MCC messages set the CR bit in the UIH frame correctly.
 * This setting is only based on whether we are the session initiator
 * or not, irrespective of whether we are sending a MCC command or response.
 * 
 *    Rev 1.24   22 Nov 2000 11:45:24   sramacha
 * 
 *    Rev 1.23   16 Nov 2000 18:27:22   amitj
 * Made a fix to break if log_alloc returns NULL for QXDM logging
 * 
 *    Rev 1.22   15 Nov 2000 15:21:18   amitj
 * Made some corrections to QXDM logging functions and function calls
 * 
 *    Rev 1.21   13 Nov 2000 17:56:24   amitj
 * Moved bt_rc_sig_tx() and bt_rc_pyld_tx() to the top of the file
 * 
 *    Rev 1.20   13 Nov 2000 17:54:00   amitj
 * Fixed some compilation errors
 * 
 *    Rev 1.19   09 Nov 2000 15:53:18   propach
 * Fix transmission of large (>255 byte) UIH data frames.
 *
 *    Rev 1.18   08 Nov 2000 11:34:06   amitj
 * Defined 2 new functions: bt_rc_sig_tx() and bt_rc_pyld_tx();
 * Modified the code to call the new functions if BT_SWDEV_QXDM_LOG_MSG is defed
 *
 *    Rev 1.17   30 Oct 2000 15:37:36   balas
 * Added DSM tracer values.
 *
 *    Rev 1.16   27 Oct 2000 11:19:50   ryuen
 * Removed obsolete code with respect to the new fast FCS check
 *
 *    Rev 1.15   24 Oct 2000 09:44:08   propach
 * Added fast FCS treatments for payload carrying UIH frames.
 *
 *    Rev 1.14   28 Sep 2000 14:46:52   propach
 * Fix use of dsm_append() - root cause of "48 byte" problem
 *
 *    Rev 1.13   25 Sep 2000 21:24:34   propach
 * NA flow control fixes.  Add MC offline & stop signal handling.
 *
 *    Rev 1.12   16 Sep 2000 15:12:22   propach
 * Add flow control between top of RFCOMM and BB for NA.  Also avoid
 * queueing of payload data.
 *
 *    Rev 1.11   12 Sep 2000 15:47:00   propach
 * New DSM changes.
 *
 *    Rev 1.10   06 Sep 2000 17:22:52   propach
 * Fixed MSG macro usage.
 *
 *    Rev 1.9   05 Sep 2000 17:56:54   propach
 * Added QDSP services changes.
 *
 *    Rev 1.8   02 Sep 2000 19:35:32   propach
 * Added BT MSGs.
 *
 *    Rev 1.7   30 Aug 2000 09:42:34   waynelee
 * Made changes for new BT_MSG macros
 *
 *    Rev 1.6   29 Aug 2000 08:26:10   propach
 * Moved comdef.h includes above FEATURE_BT to remove compiler
 * warning when feature is disabled.
 *
 *    Rev 1.5   28 Aug 2000 16:13:44   propach
 * Cleanup RFCOMM.  Add use of msg/err macros.  Add driver fault
 * recording and statistics gathering.
 *
 *    Rev 1.4   24 Aug 2000 13:47:32   propach
 * RFCOMM optimizations. Baseband fixes.
 *
 *    Rev 1.3   17 Aug 2000 13:46:20   propach
 * Added Event Control (EC).  Fixed minor problems in RFCOMM.
 *
 *    Rev 1.2   02 Aug 2000 09:40:58   propach
 * Added QBTS support for BT.  Added to API.
 *
 *    Rev 1.1   19 Jul 2000 17:27:06   propach
 * Update BT API and featurization with cleanup.
 *
 *    Rev 1.0   04 Jul 2000 19:22:32   ryuen
 * Initial revision.

===========================================================================*/

/*===========================================================================

                     INCLUDE FILES FOR MODULE

===========================================================================*/

#include "customer.h"
#include "comdef.h"
#ifdef FEATURE_BT
#include <string.h>
#include "dsm.h"
#include "bt.h"
#include "bti.h"
#include "btcmdi.h"
#include "btmsg.h"
#include "btrci.h"
#include "btrcmsgi.h"
#include "btrctx.h"
#include "btutils.h"

#ifdef BT_TEST_PYLD_FAST_TX
#include "btl2.h"
#endif

#define BT_MSG_LAYER  BT_MSG_RC   /* necessary for btmsg.h */


/*===========================================================================

                         FUNCTION DEFINITIONS

===========================================================================*/

#ifdef BT_SWDEV_QXDM_LOG_MSG
/*===========================================================================

FUNCTION
  bt_rc_send_tx_signaling_log

DESCRIPTION
  Logs all the bytes in a specified RFCOMM transmit signaling packet
  to the QXDM.

===========================================================================*/
static void bt_rc_send_tx_signaling_log
(
  uint8           dlci,
  uint16          cid,
  dsm_item_type*  dsm_ptr
)
{

  log_bt_rc_sig_tx_type*  log_ptr;
  uint32                  data_length;

  data_length = dsm_length_packet( dsm_ptr );
  data_length = MIN( data_length, BT_RC_LOG_BUFFER_SIZE );

  log_ptr = (log_bt_rc_sig_tx_type *)log_alloc( log_bt_rc_sig_tx,
                       sizeof( log_bt_rc_sig_tx_type ) -
                       BT_RC_LOG_BUFFER_SIZE + data_length );

  if ( log_ptr != NULL )
  {
    log_ptr->length    = data_length;
    log_ptr->dlci      = dlci;
    log_ptr->local_cid = cid;

    dsm_extract( dsm_ptr, BT_FIRST_BYTE_OFFSET,
                 (void*)(log_ptr->buffer),
                 data_length );

    log_commit( log_ptr );
  }

}


/*===========================================================================

FUNCTION
  bt_rc_send_payload_tx_log

DESCRIPTION
  Logs the first and last bytes in a specified transmit RFCOMM
  payload packet to the QXDM.
  
===========================================================================*/
LOCAL void bt_rc_send_payload_tx_log
(
  uint8           dlci,
  uint16          cid,
  dsm_item_type*  dsm_ptr
)
{

  log_bt_rc_pyld_tx_type*  log_ptr;
  uint32                   data_length;

  data_length = dsm_length_packet( dsm_ptr );
  data_length = MIN( data_length, BT_PYLD_BUFFER_SIZE );

  log_ptr = (log_bt_rc_pyld_tx_type *)log_alloc( log_bt_rc_pyld_tx,
                       sizeof( log_bt_rc_pyld_tx_type ) -
                       BT_PYLD_BUFFER_SIZE + data_length );

  if ( log_ptr != NULL )
  {
    log_ptr->length    = data_length;
    log_ptr->dlci      = dlci;
    log_ptr->local_cid = cid;

    dsm_extract( dsm_ptr, BT_FIRST_BYTE_OFFSET,
                 (void*)(log_ptr->buffer), 
                 MIN( data_length, BT_FIRST_TEN_PYLD_BYTES ) );

    if ( data_length >=
           (BT_FIRST_TEN_PYLD_BYTES + BT_LAST_TEN_PYLD_BYTES) )
    {
      dsm_extract( dsm_ptr,
                   (data_length - BT_LAST_TEN_PYLD_BYTES), 
                   (void*)(log_ptr->buffer + BT_FIRST_TEN_PYLD_BYTES),
                   BT_LAST_TEN_PYLD_BYTES );
    }
    else if ( data_length > BT_FIRST_TEN_PYLD_BYTES )
    {
      dsm_extract( dsm_ptr, BT_FIRST_TEN_PYLD_BYTES, 
                   (void*)(log_ptr->buffer + BT_FIRST_TEN_PYLD_BYTES),
                   (data_length - BT_FIRST_TEN_PYLD_BYTES) );
    }
    log_commit( log_ptr );
  }

}

#endif /* BT_SWDEV_QXDM_LOG_MSG */


/*===========================================================================

FUNCTION
  bt_rc_tx_signaling_on_l2cap

DESCRIPTION
  Transmits specified RFCOMM signaling data on L2CAP via a BT L2CAP
  Write Command and logs the data to the QXDM as needed.

===========================================================================*/
void bt_rc_tx_signaling_on_l2cap
(
  bt_rc_session_type*  ssn_ptr,
  bt_rc_dlc_type*      dlc_ptr,
  dsm_item_type*       dsm_ptr
)
{

  BT_MSG_API( "BT RC CMD TX: L2 Write PID %x AID %x CID %x",
              dlc_ptr->pid, bt_rc_app_id, ssn_ptr->cid );

#ifdef BT_SWDEV_QXDM_LOG_MSG
  bt_rc_send_tx_signaling_log( dlc_ptr->dlci, ssn_ptr->cid, dsm_ptr );
#endif
  
  bt_cmd_l2_write( bt_rc_app_id, ssn_ptr->cid, dsm_ptr, NULL );
  ssn_ptr->l2_write_cmds++;

}


/*===========================================================================

FUNCTION
  bt_rc_tx_sabm_frame

DESCRIPTION
  Transmits an SABM frame on a specified DLC.

===========================================================================*/
void bt_rc_tx_sabm_frame
(
  bt_rc_session_type*  ssn_ptr,
  bt_rc_dlc_type*      dlc_ptr
)
{

  dsm_item_type*             dsm_ptr;
  bt_rc_non_uih_frame_type   nuf;

  BT_MSG_SIG( "BT RC: TX SABM PID %x DLCI %x CID %x",
              dlc_ptr->pid, dlc_ptr->dlci, ssn_ptr->cid );

  dsm_ptr = bt_get_free_dsm_ptr( BT_TL_L2CAP,
                                 sizeof( bt_rc_non_uih_frame_type ) );

  nuf.rcf_hdr.address = RCFH_DLCI_2_ADDR( dlc_ptr->dlci );
  if ( ssn_ptr->initiator != FALSE )
  {
    nuf.rcf_hdr.address |= RC_CR_CMD_B;
  }
  nuf.rcf_hdr.control = (RCFH_CTRL_PF_B | RCFH_CTRL_FT_SABM_V);
  nuf.rcf_hdr.length = RCFH_VAL1_2_LEN1( 0 );
  nuf.rcf_fcs = bt_rc_calculate_fcs(
                       (uint8*)(&(nuf.rcf_hdr.address)),
                       BT_RC_FCS_BYTES_NON_UIH );

  bt_dsm_pushdown_tail( &dsm_ptr, (void*)&nuf, sizeof( bt_rc_non_uih_frame_type ),
                        DSM_DS_SMALL_ITEM_POOL );

  bt_rc_tx_signaling_on_l2cap( ssn_ptr, dlc_ptr, dsm_ptr );

  dlc_ptr->expected_rx_mask |= BT_ERXM_UA_OR_DM_B;

  dlc_ptr->tx_sabm_frames++;

}


/*===========================================================================

FUNCTION
  bt_rc_tx_ua_frame

DESCRIPTION
  Transmits a UA frame on a specified DLC.

===========================================================================*/
void bt_rc_tx_ua_frame
(
  bt_rc_session_type*  ssn_ptr,
  bt_rc_dlc_type*      dlc_ptr
)
{

  dsm_item_type*             dsm_ptr;
  bt_rc_non_uih_frame_type   nuf;

  BT_MSG_SIG( "BT RC: TX UA PID %x DLCI %x CID %x",
              dlc_ptr->pid, dlc_ptr->dlci, ssn_ptr->cid );

  dsm_ptr = bt_get_free_dsm_ptr( BT_TL_L2CAP,
                                 sizeof( bt_rc_non_uih_frame_type ) );

  nuf.rcf_hdr.address = RCFH_DLCI_2_ADDR( dlc_ptr->dlci );
  if ( ssn_ptr->initiator == FALSE )
  {
    nuf.rcf_hdr.address |= RC_CR_CMD_B;
  }
  nuf.rcf_hdr.control = (RCFH_CTRL_PF_B | RCFH_CTRL_FT_UA_V);
  nuf.rcf_hdr.length = RCFH_VAL1_2_LEN1( 0 );
  nuf.rcf_fcs = bt_rc_calculate_fcs(
                       (uint8*)(&(nuf.rcf_hdr.address)),
                       BT_RC_FCS_BYTES_NON_UIH );

  bt_dsm_pushdown_tail( &dsm_ptr, (void*)&nuf, sizeof( bt_rc_non_uih_frame_type ),
                        DSM_DS_SMALL_ITEM_POOL );

  bt_rc_tx_signaling_on_l2cap( ssn_ptr, dlc_ptr, dsm_ptr );

  dlc_ptr->tx_ua_frames++;

}


/*===========================================================================

FUNCTION
  bt_rc_tx_uih_data_frame

DESCRIPTION
  Transmits a UIH frame with specified user data on a specified DLC.

===========================================================================*/
void bt_rc_tx_uih_data_frame
(
  bt_rc_session_type*  ssn_ptr,
  bt_rc_dlc_type*      dlc_ptr,
  dsm_item_type*       dsm_ptr
)
{

  word                     rcfh_length;
  uint16                   pkt_length;
  bt_rc_frame_hdre_type    rcfe_hdr;
  uint8                    first_tx_data_byte;
  fast_uih_fcs_type*       tx_ff_ptr;
  bt_rc_cbfc_credits_type  cbfc_credits = 0;
#ifdef BT_TEST_PYLD_FAST_TX
  bt_cmd_msg_type          l2_write_cmd;
#endif

  dsm_peek_byte( dsm_ptr, 0, &first_tx_data_byte );

  if ( dlc_ptr->config.cbfc_enabled != FALSE )
  {
    if ( bt_rc_far_tx_credits_needed( ssn_ptr, dlc_ptr ) != FALSE )
    {
      tx_ff_ptr = &dlc_ptr->tx_fast_uih_pf1;
      cbfc_credits = bt_rc_max_far_tx_credits( ssn_ptr, dlc_ptr );
      
#ifdef BT_FLOW_DEBUG
      if ( dlc_ptr->config.cbfc_far_credits == 0 )
      {
        BT_MSG_HIGH( "BT RC: CBFC Far TX ON PID %x",
                     dlc_ptr->pid, 0, 0 );
      }
#endif

      dlc_ptr->config.cbfc_far_credits += cbfc_credits;
    }
    else
    {
      tx_ff_ptr = &dlc_ptr->tx_fast_uih_pf0;
    }
  }
  else
  {
    tx_ff_ptr = &dlc_ptr->tx_fast_uih_pf0;
  }
  rcfe_hdr.address = tx_ff_ptr->hdr_addr;
  rcfe_hdr.control = tx_ff_ptr->hdr_ctrl;

  pkt_length = dsm_length_packet( dsm_ptr );
  if ( pkt_length <= RCFH_LEN_MAX_NON_EXT )
  {
    rcfe_hdr.length1 = RCFH_VAL1_2_LEN1( pkt_length );
    rcfe_hdr.length2 = 0;
    rcfh_length = sizeof( bt_rc_frame_hdr_type );
  }
  else
  {
    rcfe_hdr.length1 = ((pkt_length << 1) & RCFH_LEN_LSB_M);
    rcfe_hdr.length2 = ((pkt_length >> 7) & RCFH_LEN_MSB_M);
    rcfh_length = sizeof( bt_rc_frame_hdre_type );
  }

  if ( cbfc_credits > 0 )
  {
#ifdef BT_FLOW_DEBUG
    BT_MSG_HIGH( "BT RC: CBFC TX FCr Tot/Cur %04x Len %x PID %x",
                 (uint16)((dlc_ptr->config.cbfc_far_credits << 8) |
                             cbfc_credits), pkt_length, dlc_ptr->pid );
#endif
    dsm_pushdown( &dsm_ptr, (void*)(&cbfc_credits),
                  sizeof( bt_rc_cbfc_credits_type ),
                  DSM_DS_SMALL_ITEM_POOL );
  }
  
  dsm_pushdown( &dsm_ptr, (void*)(&rcfe_hdr), rcfh_length,
                DSM_DS_SMALL_ITEM_POOL );

  dsm_pushdown_tail( &dsm_ptr, (void*)(&tx_ff_ptr->fcs),
                     sizeof( bt_rc_fcs_type ),
                     DSM_DS_SMALL_ITEM_POOL );

  dlc_ptr->tx_uih_frames++;
  dlc_ptr->tx_data_bytes += pkt_length;
  
  /*  Update local transmit credits   */
  /*  for Credit Based Flow Control.  */
  if ( (dlc_ptr->config.cbfc_enabled != FALSE) && (pkt_length > 0) )
  {
    if ( dlc_ptr->config.cbfc_local_credits > 0 )
    {
      dlc_ptr->config.cbfc_local_credits--;
    }
    else
    {
      BT_ERR( "BT RC: TX CBFC Lcr %x PID %x",
              dlc_ptr->config.cbfc_local_credits,
              dlc_ptr->pid, 0 );
    }
  }


  BT_MSG_PYLD( "BT RC TXD: DLCI %x Cnt %x d1 %x",
               dlc_ptr->dlci, pkt_length, first_tx_data_byte );
  BT_MSG_API_PYLD( "BT RC CMD TXD: L2 Write Cnt %x AID %x CID %x",
                   pkt_length, dlc_ptr->bt_app_id, ssn_ptr->cid );

  BT_SET_PKT_TRACER( dsm_ptr, DSM_BT_TX_RC );

#ifdef BT_SWDEV_QXDM_LOG_MSG
  bt_rc_send_payload_tx_log( dlc_ptr->dlci, ssn_ptr->cid, dsm_ptr );
#endif

#ifdef BT_TEST_PYLD_FAST_TX
  l2_write_cmd.cmd_hdr.cmd_type             = BT_CMD_L2_WRITE;
  l2_write_cmd.cmd_hdr.bt_app_id            = bt_rc_app_id;
  l2_write_cmd.cmd_msg.cmd_l2_write.cid     = ssn_ptr->cid;
  l2_write_cmd.cmd_msg.cmd_l2_write.dsm_ptr = dsm_ptr;
  l2_write_cmd.cmd_msg.cmd_l2_write.frame_handle  = NULL;
  bt_l2_cmd_write( &l2_write_cmd );
#else
  bt_cmd_l2_write( bt_rc_app_id, ssn_ptr->cid, dsm_ptr, NULL );
#endif

  ssn_ptr->l2_write_cmds++;

}


/*===========================================================================

FUNCTION
  bt_rc_tx_disc_frame

DESCRIPTION
  Transmits a DISC (disconnect) frame on a specified DLC.

===========================================================================*/
void bt_rc_tx_disc_frame
(
  bt_rc_session_type*  ssn_ptr,
  bt_rc_dlc_type*      dlc_ptr
)
{

  dsm_item_type*             dsm_ptr;
  bt_rc_non_uih_frame_type   nuf;

  BT_MSG_SIG( "BT RC: TX DISC PID %x DLCI %x CID %x",
              dlc_ptr->pid, dlc_ptr->dlci, ssn_ptr->cid );

  dsm_ptr = bt_get_free_dsm_ptr( BT_TL_L2CAP,
                                 sizeof( bt_rc_non_uih_frame_type ) );

  nuf.rcf_hdr.address = RCFH_DLCI_2_ADDR( dlc_ptr->dlci );
  if ( ssn_ptr->initiator != FALSE )
  {
    nuf.rcf_hdr.address |= RC_CR_CMD_B;
  }
  nuf.rcf_hdr.control = (RCFH_CTRL_PF_B | RCFH_CTRL_FT_DISC_V);
  nuf.rcf_hdr.length = RCFH_VAL1_2_LEN1( 0 );
  nuf.rcf_fcs = bt_rc_calculate_fcs(
                       (uint8*)(&(nuf.rcf_hdr.address)),
                       BT_RC_FCS_BYTES_NON_UIH );

  bt_dsm_pushdown_tail( &dsm_ptr, (void*)&nuf, sizeof( bt_rc_non_uih_frame_type ),
                        DSM_DS_SMALL_ITEM_POOL );

  bt_rc_tx_signaling_on_l2cap( ssn_ptr, dlc_ptr, dsm_ptr );

  dlc_ptr->expected_rx_mask |= BT_ERXM_UA_OR_DM_B;
  
  dlc_ptr->tx_disc_frames++;

}


/*===========================================================================

FUNCTION
  bt_rc_tx_dm_frame

DESCRIPTION
  Transmits a DM frame on a specified session for a specified DLCI.

===========================================================================*/
void bt_rc_tx_dm_frame
(
  bt_rc_session_type*  ssn_ptr,
  uint8                dlci
)
{

  dsm_item_type*             dsm_ptr;
  bt_rc_non_uih_frame_type   nuf;

  BT_MSG_SIG( "BT RC: TX DM CID %x DLCI %x",
              ssn_ptr->cid, dlci, 0 );

  dsm_ptr = bt_get_free_dsm_ptr( BT_TL_L2CAP,
                                 sizeof( bt_rc_non_uih_frame_type ) );

  nuf.rcf_hdr.address = RCFH_DLCI_2_ADDR( dlci );
  if ( ssn_ptr->initiator == FALSE )
  {
    nuf.rcf_hdr.address |= RC_CR_CMD_B;
  }
  nuf.rcf_hdr.control = (RCFH_CTRL_PF_B | RCFH_CTRL_FT_DM_V);
  nuf.rcf_hdr.length = RCFH_VAL1_2_LEN1( 0 );
  nuf.rcf_fcs = bt_rc_calculate_fcs(
                       (uint8*)(&(nuf.rcf_hdr.address)),
                       BT_RC_FCS_BYTES_NON_UIH );

  bt_dsm_pushdown_tail( &dsm_ptr, (void*)&nuf, sizeof( bt_rc_non_uih_frame_type ),
                        DSM_DS_SMALL_ITEM_POOL );

  bt_rc_tx_signaling_on_l2cap( ssn_ptr, &ssn_ptr->mcc, dsm_ptr );

  ssn_ptr->mcc.tx_dm_frames++;

}


/*===========================================================================

FUNCTION
  bt_rc_tx_nsc_response

DESCRIPTION
  Transmits a Not Supported Command (NSC) response with a specified
  received MCC header type field on a specified session.

===========================================================================*/
void bt_rc_tx_nsc_response
(
  bt_rc_session_type*  ssn_ptr,
  uint8                rx_mcc_hdr_type
)
{

  dsm_item_type*             dsm_ptr;
  bt_rc_mcc_nsc_frame_type   nsc;

  BT_MSG_SIG( "BT RC: TX NSC Resp Type %x CID %x",
              rx_mcc_hdr_type, ssn_ptr->cid, 0 );
  
  dsm_ptr = bt_get_free_dsm_ptr(
              BT_TL_L2CAP, sizeof( bt_rc_mcc_nsc_frame_type ) );
  
  nsc.rcf_hdr.address = RCFH_DLCI_2_ADDR( RC_DLCI_MCC_V );
  if ( ssn_ptr->initiator != FALSE )
  {
    nsc.rcf_hdr.address |= RC_CR_CMD_B;
  }
  nsc.rcf_hdr.control = RCFH_CTRL_FT_UIH_V;
  nsc.rcf_hdr.length =
    RCFH_VAL1_2_LEN1( sizeof( bt_rc_mcc_nsc_msg_type ) );
  
  nsc.nsc_msg.mcc_hdr.type =
    RCMCCH_MT_TO_TYPE( RCMCCH_TYPE_MT_NSC_V );
  nsc.nsc_msg.mcc_hdr.length =
    RCMCCH_VALLSB_2_LENLSB( sizeof( bt_rc_mcc_nsc_type ) );

  nsc.nsc_msg.nsc_body.cmd_type = rx_mcc_hdr_type;

  nsc.rcf_fcs = bt_rc_calculate_fcs(
                       (uint8*)(&(nsc.rcf_hdr.address)),
                       BT_RC_FCS_BYTES_UIH );

  bt_dsm_pushdown_tail( &dsm_ptr, (void*)&nsc, sizeof( bt_rc_mcc_nsc_frame_type ),
                        DSM_DS_SMALL_ITEM_POOL );

  bt_rc_tx_signaling_on_l2cap( ssn_ptr, &ssn_ptr->mcc, dsm_ptr );

  ssn_ptr->mcc.tx_uih_frames++;

}


/*===========================================================================

FUNCTION
  bt_rc_tx_msc_command

DESCRIPTION
  Transmits an MSC command message for a specified DLC.

===========================================================================*/
void bt_rc_tx_msc_command
(
  bt_rc_session_type*  ssn_ptr,
  bt_rc_dlc_type*      dlc_ptr
)
{

  dsm_item_type*              dsm_ptr;
  bt_rc_mcc_msc_frame_type    mscf;
  bt_rc_mcc_msce_frame_type   mscef;
  boolean                     v24_changed = FALSE;

  if ( dlc_ptr->config.cbfc_enabled != FALSE )
  {
    if ( (dlc_ptr->local_v24_signals &
            ~RCMCCM_MSC_V24_FC_FLOW_OFF_B) !=
          dlc_ptr->last_msc_tx_v24_signals )
    {
      v24_changed = TRUE;
    }
  }
  else
  {
    if ( dlc_ptr->local_v24_signals !=
           dlc_ptr->last_msc_tx_v24_signals )
    {
      v24_changed = TRUE;
    }
  }

  if ( (!(dlc_ptr->expected_rx_mask & BT_ERXM_MSC_RESP_B)) &&
       ((v24_changed != FALSE) ||
        (dlc_ptr->local_breaks != dlc_ptr->last_msc_tx_breaks)) )
  {
    BT_MSG_SIG( "BT RC: TX MSC Cmd PID %x DLCI %x CID %x",
                dlc_ptr->pid, dlc_ptr->dlci, ssn_ptr->cid );
  
    dsm_ptr = bt_get_free_dsm_ptr(
                BT_TL_L2CAP, sizeof( bt_rc_mcc_msce_frame_type ) );
  
    mscf.rcf_hdr.address = RCFH_DLCI_2_ADDR( RC_DLCI_MCC_V );
    if ( ssn_ptr->initiator != FALSE )
    {
      mscf.rcf_hdr.address |= RC_CR_CMD_B;
    }
    mscf.rcf_hdr.control = RCFH_CTRL_FT_UIH_V;
    
    mscf.msc_msg.mcc_hdr.type =
      RCMCCH_MT_TO_TYPE( RCMCCH_TYPE_MT_MSC_V );
    mscf.msc_msg.mcc_hdr.type |= RC_CR_CMD_B;
  
    mscf.msc_msg.msc_body.dlci =
      RCMCCM_DLCI_2_MDLCI( dlc_ptr->dlci );
    
    mscf.msc_msg.msc_body.v24_signals =
      dlc_ptr->local_v24_signals;

    if ( dlc_ptr->config.cbfc_enabled != FALSE )
    {
      /*  Force FC bit to 0 if CBFC is enabled.  */
      mscf.msc_msg.msc_body.v24_signals &=
        ~RCMCCM_MSC_V24_FC_FLOW_OFF_B;
    }
  
    dlc_ptr->last_msc_tx_v24_signals =
      mscf.msc_msg.msc_body.v24_signals;

    mscf.rcf_fcs = bt_rc_calculate_fcs(
                     (uint8*)(&(mscf.rcf_hdr.address)),
                     BT_RC_FCS_BYTES_UIH );

    if ( dlc_ptr->local_breaks != dlc_ptr->last_msc_tx_breaks )
    {
      memcpy( (void*)&mscef, (void*)&mscf,
              sizeof(bt_rc_mcc_msc_frame_type) );
      mscef.rcf_hdr.length =
        RCFH_VAL1_2_LEN1( sizeof( bt_rc_mcc_msce_msg_type ) );
      mscef.msce_msg.mcc_hdr.length =
        RCMCCH_VALLSB_2_LENLSB( sizeof( bt_rc_mcc_msce_type ) );
      mscef.msce_msg.msce_body.break_signals =
        RCMCCM_MSCE_LEN_2_BKS( dlc_ptr->local_last_break_length );
      mscef.msce_msg.msce_body.break_signals |=
        RCMCCM_MSCE_BRK_SIG_B;
      mscef.rcf_fcs = mscf.rcf_fcs;
      dlc_ptr->last_msc_tx_breaks++;
      dlc_ptr->last_msc_tx_break_signals =
        mscef.msce_msg.msce_body.break_signals;
  
      bt_dsm_pushdown_tail( &dsm_ptr, (void*)&mscef,
                            sizeof( bt_rc_mcc_msce_frame_type ),
                            DSM_DS_SMALL_ITEM_POOL );
    }
    else
    {
      mscf.rcf_hdr.length =
        RCFH_VAL1_2_LEN1( sizeof( bt_rc_mcc_msc_msg_type ) );
      mscf.msc_msg.mcc_hdr.length =
        RCMCCH_VALLSB_2_LENLSB( sizeof( bt_rc_mcc_msc_type ) );
  
      dlc_ptr->last_msc_tx_break_signals = 0x0;

      bt_dsm_pushdown_tail( &dsm_ptr, (void*)&mscf,
                            sizeof( bt_rc_mcc_msc_frame_type ),
                            DSM_DS_SMALL_ITEM_POOL );
    }

    bt_rc_tx_signaling_on_l2cap( ssn_ptr, dlc_ptr, dsm_ptr );
  
    dlc_ptr->expected_rx_mask |= BT_ERXM_MSC_RESP_B;
  
    dlc_ptr->tx_uih_frames++;
  }

}


/*===========================================================================

FUNCTION
  bt_rc_tx_msc_response

DESCRIPTION
  Transmits a specified MSC response message on a specified DLCI on
  which a MSC command was received.

===========================================================================*/
void bt_rc_tx_msc_response
(
  bt_rc_rx_info_type*   rxi_ptr,
  bt_rc_mcc_msce_type*  msce_ptr
)
{

  dsm_item_type*              tx_dsm_ptr;
  bt_rc_mcc_msc_frame_type    tx_mscf;
  bt_rc_mcc_msce_frame_type   tx_mscef;

  BT_MSG_SIG( "BT RC: TX MSC Resp DLCI %x CID %x",
              RCMCCM_MDLCI_2_DLCI( msce_ptr->msc_body.dlci ),
              rxi_ptr->ssn_ptr->cid, 0 );

  tx_dsm_ptr = bt_get_free_dsm_ptr(
                 BT_TL_L2CAP, sizeof( bt_rc_mcc_msce_frame_type ) );

  tx_mscf.rcf_hdr.address = RCFH_DLCI_2_ADDR( RC_DLCI_MCC_V );
  if ( rxi_ptr->ssn_ptr->initiator != FALSE )
  {
    tx_mscf.rcf_hdr.address |= RC_CR_CMD_B;
  }
  tx_mscf.rcf_hdr.control = RCFH_CTRL_FT_UIH_V;

  tx_mscf.msc_msg.mcc_hdr.type =
    RCMCCH_MT_TO_TYPE( RCMCCH_TYPE_MT_MSC_V );
  memcpy( (byte*)(&tx_mscf.msc_msg.msc_body),
          (byte*)(msce_ptr), sizeof( bt_rc_mcc_msc_type ) );

  tx_mscf.rcf_fcs = bt_rc_calculate_fcs(
                      (uint8*)(&(tx_mscf.rcf_hdr.address)),
                      BT_RC_FCS_BYTES_UIH );

  if ( rxi_ptr->mcch_length == sizeof( bt_rc_mcc_msce_type ) )
  {
    memcpy( (void*)&tx_mscef, (void*)&tx_mscf,
            sizeof( bt_rc_mcc_msc_frame_type ) );
    tx_mscef.msce_msg.msce_body.break_signals =
      msce_ptr->break_signals;
    tx_mscef.rcf_hdr.length =
      RCFH_VAL1_2_LEN1( sizeof( bt_rc_mcc_msce_msg_type ) );
    tx_mscef.msce_msg.mcc_hdr.length =
      RCMCCH_VALLSB_2_LENLSB( sizeof( bt_rc_mcc_msce_type ) );
    tx_mscef.rcf_fcs = tx_mscf.rcf_fcs;
    bt_dsm_pushdown_tail( &tx_dsm_ptr, (void*)&tx_mscef,
                          sizeof( bt_rc_mcc_msce_frame_type ),
                          DSM_DS_SMALL_ITEM_POOL );
  }
  else
  {
    tx_mscf.rcf_hdr.length =
       RCFH_VAL1_2_LEN1( sizeof( bt_rc_mcc_msc_msg_type ) );
    tx_mscf.msc_msg.mcc_hdr.length =
       RCMCCH_VALLSB_2_LENLSB( sizeof( bt_rc_mcc_msc_type ) );
    bt_dsm_pushdown_tail( &tx_dsm_ptr, (void*)&tx_mscf,
                          sizeof( bt_rc_mcc_msc_frame_type ),
                          DSM_DS_SMALL_ITEM_POOL );
  }

  bt_rc_tx_signaling_on_l2cap( rxi_ptr->ssn_ptr,
                               rxi_ptr->dlc_ptr,
                               tx_dsm_ptr );

  rxi_ptr->dlc_ptr->tx_uih_frames++;

}


/*===========================================================================

FUNCTION
  bt_rc_tx_pn_command

DESCRIPTION
  Transmits an PN command message for a specified DLC.

===========================================================================*/
void bt_rc_tx_pn_command
(
  bt_rc_session_type*  ssn_ptr,
  bt_rc_dlc_type*      dlc_ptr
)
{

  dsm_item_type*            dsm_ptr;
  bt_rc_mcc_pn_frame_type   pnf;

  BT_MSG_SIG( "BT RC: TX PN Cmd PID %x DLCI %x CID %x",
              dlc_ptr->pid, dlc_ptr->dlci, ssn_ptr->cid );

  dsm_ptr = bt_get_free_dsm_ptr(
              BT_TL_L2CAP, sizeof( bt_rc_mcc_pn_frame_type ) );

  pnf.rcf_hdr.address = RCFH_DLCI_2_ADDR( RC_DLCI_MCC_V );
  if ( ssn_ptr->initiator != FALSE )
  {
    pnf.rcf_hdr.address |= RC_CR_CMD_B;
  }
  pnf.rcf_hdr.control = RCFH_CTRL_FT_UIH_V;
  
  pnf.pn_msg.mcc_hdr.type =
    RCMCCH_MT_TO_TYPE( RCMCCH_TYPE_MT_PN_V );
  pnf.pn_msg.mcc_hdr.type |= RC_CR_CMD_B;
  pnf.rcf_hdr.length =
    RCFH_VAL1_2_LEN1( sizeof( bt_rc_mcc_pn_msg_type ) );
  pnf.pn_msg.mcc_hdr.length =
    RCMCCH_VALLSB_2_LENLSB( sizeof( bt_rc_mcc_pn_type ) );

  pnf.pn_msg.pn_body.dlci = dlc_ptr->dlci & RCMCCM_PN_DLCI_M;

  if ( (dlc_ptr->config.pn_negot_done == FALSE) &&
       (dlc_ptr->config.cbfc_enabled != FALSE) )
  {
    pnf.pn_msg.pn_body.cl_field =
      RCMCCM_PN_CLVAL_2_CLF( RCMCCM_PN_CLVAL_CBFC_REQ_V );
    
    dlc_ptr->config.cbfc_far_credits = 
      bt_rc_max_far_tx_credits( ssn_ptr, dlc_ptr );
    dlc_ptr->config.cbfc_far_credits = MIN( BT_RC_CBFC_CREDITS_MAX_PN,
                                            dlc_ptr->config.cbfc_far_credits );
    
    if ( dlc_ptr->config.cbfc_far_credits < BT_RC_CBFC_CREDITS_MIN_PN )
    {
      if ( dlc_ptr->rx_wm_ptr != NULL )
      {
        BT_ERR( "BT RC: Bad WM DEC %x FCr %x PID %x",
                dlc_ptr->rx_wm_ptr->dont_exceed_cnt,
                dlc_ptr->config.cbfc_far_credits, dlc_ptr->pid );
      }

      dlc_ptr->config.cbfc_far_credits = BT_RC_CBFC_CREDITS_MIN_PN;
    }
    
    pnf.pn_msg.pn_body.k_field =
      RCMCCM_PN_KVAL_2_KF( dlc_ptr->config.cbfc_far_credits );
  }
  else
  {
    pnf.pn_msg.pn_body.cl_field =
      RCMCCM_PN_CLVAL_2_CLF( RCMCCM_PN_CLVAL_CBFC_NO_V );
    pnf.pn_msg.pn_body.k_field = RCMCCM_PN_KVAL_2_KF( 0 );
  }

  pnf.pn_msg.pn_body.priority =
    dlc_ptr->config.priority & RCMCCM_PN_PRIORITY_M;

  pnf.pn_msg.pn_body.max_frame_size =
    dlc_ptr->config.max_frame_size;
  
  pnf.pn_msg.pn_body.unused1 = 0;
  pnf.pn_msg.pn_body.unused2 = 0;

  pnf.rcf_fcs = bt_rc_calculate_fcs(
                   (uint8*)(&(pnf.rcf_hdr.address)),
                   BT_RC_FCS_BYTES_UIH );

  bt_dsm_pushdown_tail( &dsm_ptr, (void*)&pnf, sizeof( bt_rc_mcc_pn_frame_type ),
                        DSM_DS_SMALL_ITEM_POOL );

  bt_rc_tx_signaling_on_l2cap( ssn_ptr, dlc_ptr, dsm_ptr );

  dlc_ptr->expected_rx_mask |= BT_ERXM_PN_RESP_B;

  dlc_ptr->tx_uih_frames++;

}


/*===========================================================================

FUNCTION
  bt_rc_tx_pn_response

DESCRIPTION
  Transmits a specified PN response message on a specified DLCI on
  which a PN command was received.

===========================================================================*/
void bt_rc_tx_pn_response
(
  bt_rc_rx_info_type*  rxi_ptr,
  bt_rc_mcc_pn_type*   pn_ptr
)
{

  dsm_item_type*            tx_dsm_ptr;
  bt_rc_mcc_pn_frame_type   tx_pn;

  BT_MSG_SIG( "BT RC: TX PN Resp DLCI %x CID %x",
              (pn_ptr->dlci & RCMCCM_PN_DLCI_M),
              rxi_ptr->ssn_ptr->cid, 0 );

  tx_dsm_ptr = bt_get_free_dsm_ptr( BT_TL_L2CAP,
                                    sizeof( bt_rc_mcc_pn_frame_type ) );

  tx_pn.rcf_hdr.address = RCFH_DLCI_2_ADDR( RC_DLCI_MCC_V );
  if ( rxi_ptr->ssn_ptr->initiator != FALSE )
  {
    tx_pn.rcf_hdr.address |= RC_CR_CMD_B;
  }
  tx_pn.rcf_hdr.control = RCFH_CTRL_FT_UIH_V;
  tx_pn.rcf_hdr.length =
    RCFH_VAL1_2_LEN1( sizeof( bt_rc_mcc_pn_msg_type ) );
  
  tx_pn.pn_msg.mcc_hdr.type =
    RCMCCH_MT_TO_TYPE( RCMCCH_TYPE_MT_PN_V );
  tx_pn.pn_msg.mcc_hdr.length =
    RCMCCH_VALLSB_2_LENLSB( sizeof( bt_rc_mcc_pn_type ) );

  memcpy( (byte*)(&tx_pn.pn_msg.pn_body),
          (byte*)(pn_ptr), sizeof( bt_rc_mcc_pn_type ) );
  
  tx_pn.rcf_fcs = bt_rc_calculate_fcs(
                     (uint8*)(&(tx_pn.rcf_hdr.address)),
                     BT_RC_FCS_BYTES_UIH );

  bt_dsm_pushdown_tail( &tx_dsm_ptr, (void*)&tx_pn, sizeof( bt_rc_mcc_pn_frame_type ),
                        DSM_DS_SMALL_ITEM_POOL );

  bt_rc_tx_signaling_on_l2cap( rxi_ptr->ssn_ptr,
                               rxi_ptr->dlc_ptr,
                               tx_dsm_ptr );

  rxi_ptr->dlc_ptr->tx_uih_frames++;

}


/*===========================================================================

FUNCTION
  bt_rc_tx_rpn_command

DESCRIPTION
  Transmits an RPN command message for a specified DLC.

===========================================================================*/
void bt_rc_tx_rpn_command
(
  bt_rc_session_type*  ssn_ptr,
  bt_rc_dlc_type*      dlc_ptr,
  boolean              request_only
)
{

  dsm_item_type*              dsm_ptr;
  bt_rc_mcc_rpn_frame_type    rpnf;
  bt_rc_mcc_rpne_frame_type   rpnef;

  if ( request_only != FALSE )
  {
    BT_MSG_SIG( "BT RC: TX RPN Req DLCI %x CID %x",
                dlc_ptr->dlci, ssn_ptr->cid, 0 );
  }
  else
  {
    BT_MSG_SIG( "BT RC: TX RPN Cmd DLCI %x CID %x",
                dlc_ptr->dlci, ssn_ptr->cid, 0 );
  }

  dsm_ptr = bt_get_free_dsm_ptr(
              BT_TL_L2CAP, sizeof( bt_rc_mcc_rpne_frame_type ) );

  rpnf.rcf_hdr.address = RCFH_DLCI_2_ADDR( RC_DLCI_MCC_V );
  if ( ssn_ptr->initiator != FALSE )
  {
    rpnf.rcf_hdr.address |= RC_CR_CMD_B;
  }
  rpnf.rcf_hdr.control = RCFH_CTRL_FT_UIH_V;
  
  rpnf.rpn_msg.mcc_hdr.type =
    RCMCCH_MT_TO_TYPE( RCMCCH_TYPE_MT_RPN_V );
  rpnf.rpn_msg.mcc_hdr.type |= RC_CR_CMD_B;

  rpnf.rpn_msg.rpn_body.dlci =
    RCMCCM_DLCI_2_MDLCI( dlc_ptr->dlci );
  
  rpnf.rcf_fcs = bt_rc_calculate_fcs(
                   (uint8*)(&(rpnf.rcf_hdr.address)),
                   BT_RC_FCS_BYTES_UIH );

  if ( request_only == FALSE )
  {
    memcpy( (void*)&rpnef, (void*)&rpnf,
            sizeof( bt_rc_mcc_rpn_frame_type ) );
    rpnef.rcf_hdr.length =
      RCFH_VAL1_2_LEN1( sizeof( bt_rc_mcc_rpne_msg_type ) );
    rpnef.rpne_msg.mcc_hdr.length =
      RCMCCH_VALLSB_2_LENLSB( sizeof( bt_rc_mcc_rpne_type ) );
    rpnef.rpne_msg.rpne_body.baudrate   = dlc_ptr->config.baudrate;
    rpnef.rpne_msg.rpne_body.format     = dlc_ptr->config.format;
    rpnef.rpne_msg.rpne_body.flow_ctrl  = dlc_ptr->config.flow_ctrl;
    rpnef.rpne_msg.rpne_body.xon_char   = dlc_ptr->config.xon_char;
    rpnef.rpne_msg.rpne_body.xoff_char  = dlc_ptr->config.xoff_char;
    rpnef.rpne_msg.rpne_body.param_mask = RCMCCM_RPN_PM_DEFAULT_V;
    rpnef.rcf_fcs = rpnf.rcf_fcs;
    bt_dsm_pushdown_tail( &dsm_ptr, (void*)&rpnef, sizeof( rpnef ),
                          DSM_DS_SMALL_ITEM_POOL );
  }
  else
  {
    rpnf.rcf_hdr.length =
      RCFH_VAL1_2_LEN1( sizeof( bt_rc_mcc_rpn_msg_type ) );
    rpnf.rpn_msg.mcc_hdr.length =
      RCMCCH_VALLSB_2_LENLSB( sizeof( bt_rc_mcc_rpn_type ) );
    bt_dsm_pushdown_tail( &dsm_ptr, (void*)&rpnf, sizeof( rpnf ),
                          DSM_DS_SMALL_ITEM_POOL );
  }

  bt_rc_tx_signaling_on_l2cap( ssn_ptr, dlc_ptr, dsm_ptr );

  dlc_ptr->expected_rx_mask |= BT_ERXM_RPN_RESP_B;

  dlc_ptr->tx_uih_frames++;

}


/*===========================================================================

FUNCTION
  bt_rc_tx_rpn_response

DESCRIPTION
  Transmits a specified RPN response message on a specified DLCI on
  which a RPN command was received.

===========================================================================*/
void bt_rc_tx_rpn_response
(
  bt_rc_rx_info_type*   rxi_ptr,
  bt_rc_mcc_rpne_type*  rpne_ptr
)
{

  dsm_item_type*              tx_dsm_ptr;
  bt_rc_mcc_rpne_frame_type   tx_rpne;

  BT_MSG_SIG( "BT RC: TX RPN Resp DLCI %x CID %x",
              RCMCCM_MDLCI_2_DLCI( rpne_ptr->rpn_body.dlci ),
              rxi_ptr->ssn_ptr->cid, 0 );

  tx_dsm_ptr = bt_get_free_dsm_ptr( BT_TL_L2CAP,
                 sizeof( bt_rc_mcc_rpne_frame_type ) );

  tx_rpne.rcf_hdr.address = RCFH_DLCI_2_ADDR( RC_DLCI_MCC_V );
  if ( rxi_ptr->ssn_ptr->initiator != FALSE )
  {
    tx_rpne.rcf_hdr.address |= RC_CR_CMD_B;
  }
  tx_rpne.rcf_hdr.control = RCFH_CTRL_FT_UIH_V;
  tx_rpne.rcf_hdr.length =
    RCFH_VAL1_2_LEN1( sizeof( bt_rc_mcc_rpne_msg_type ) );
  
  tx_rpne.rpne_msg.mcc_hdr.type =
    RCMCCH_MT_TO_TYPE( RCMCCH_TYPE_MT_RPN_V );
  tx_rpne.rpne_msg.mcc_hdr.length =
    RCMCCH_VALLSB_2_LENLSB( sizeof( bt_rc_mcc_rpne_type ) );

  memcpy( (byte*)(&tx_rpne.rpne_msg.rpne_body),
          (byte*)(rpne_ptr), sizeof( bt_rc_mcc_rpne_type ) );
  
  tx_rpne.rcf_fcs = bt_rc_calculate_fcs(
                       (uint8*)(&(tx_rpne.rcf_hdr.address)),
                       BT_RC_FCS_BYTES_UIH );

  bt_dsm_pushdown_tail( &tx_dsm_ptr, (void*)&tx_rpne, sizeof( tx_rpne ),
                        DSM_DS_SMALL_ITEM_POOL );

  bt_rc_tx_signaling_on_l2cap( rxi_ptr->ssn_ptr,
                               rxi_ptr->dlc_ptr,
                               tx_dsm_ptr );

  rxi_ptr->dlc_ptr->tx_uih_frames++;

}


/*===========================================================================

FUNCTION
  bt_rc_tx_rls_command

DESCRIPTION
  Transmits an RLS command message for a specified DLC.

===========================================================================*/
void bt_rc_tx_rls_command
(
  bt_rc_session_type*  ssn_ptr,
  bt_rc_dlc_type*      dlc_ptr
)
{

  dsm_item_type*             dsm_ptr;
  bt_rc_mcc_rls_frame_type   rlsf;
  uint8                      rls_value1 = 0x0;

  if ( (!(dlc_ptr->expected_rx_mask & BT_ERXM_MSC_RESP_B)) )
  {
    if ( dlc_ptr->local_overruns !=
           dlc_ptr->last_rls_tx_overruns )
    {
      rls_value1 = RCMCCM_RLS_LEV_2_V1( RCMCCM_RLS_V1_LSET_OVERRUN_V );
      dlc_ptr->last_rls_tx_overruns++;
    }
    else if ( dlc_ptr->local_parity_errors !=
                dlc_ptr->last_rls_tx_parity_errors )
    {
      rls_value1 = RCMCCM_RLS_LEV_2_V1( RCMCCM_RLS_V1_LSET_PARITY_V );
      dlc_ptr->last_rls_tx_parity_errors++;
    }
    else if ( dlc_ptr->local_framing_errors !=
                dlc_ptr->last_rls_tx_framing_errors )
    {
      rls_value1 = RCMCCM_RLS_LEV_2_V1( RCMCCM_RLS_V1_LSET_FRAMING_V );
      dlc_ptr->last_rls_tx_framing_errors++;
    }
  }

  if ( rls_value1 != 0x0 )
  {
    BT_MSG_SIG( "BT RC: TX RLS Cmd V1 %x PID %x DLCI %x",
                rls_value1, dlc_ptr->pid, dlc_ptr->dlci );

    dsm_ptr = bt_get_free_dsm_ptr(
                BT_TL_L2CAP, sizeof( bt_rc_mcc_rls_frame_type ) );

    rlsf.rcf_hdr.address = RCFH_DLCI_2_ADDR( RC_DLCI_MCC_V );
    if ( ssn_ptr->initiator != FALSE )
    {
      rlsf.rcf_hdr.address |= RC_CR_CMD_B;
    }
    rlsf.rcf_hdr.control = RCFH_CTRL_FT_UIH_V;
  
    rlsf.rls_msg.mcc_hdr.type =
      RCMCCH_MT_TO_TYPE( RCMCCH_TYPE_MT_RLS_V );
    rlsf.rls_msg.mcc_hdr.type |= RC_CR_CMD_B;

    rlsf.rls_msg.rls_body.dlci =
      RCMCCM_DLCI_2_MDLCI( dlc_ptr->dlci );
  
    rlsf.rls_msg.rls_body.value1 = rls_value1;
    
    dlc_ptr->last_rls_tx_value1 = rls_value1;

    rlsf.rcf_hdr.length =
      RCFH_VAL1_2_LEN1( sizeof( bt_rc_mcc_rls_msg_type ) );
    rlsf.rls_msg.mcc_hdr.length =
      RCMCCH_VALLSB_2_LENLSB( sizeof( bt_rc_mcc_rls_type ) );

    rlsf.rcf_fcs = bt_rc_calculate_fcs(
                      (uint8*)(&(rlsf.rcf_hdr.address)),
                      BT_RC_FCS_BYTES_UIH );

    bt_dsm_pushdown_tail( &dsm_ptr, (void*)&rlsf, sizeof( rlsf ),
                          DSM_DS_SMALL_ITEM_POOL );

    bt_rc_tx_signaling_on_l2cap( ssn_ptr, dlc_ptr, dsm_ptr );

    dlc_ptr->expected_rx_mask |= BT_ERXM_RLS_RESP_B;
 
    dlc_ptr->tx_uih_frames++;
  }

}


/*===========================================================================

FUNCTION
  bt_rc_tx_rls_response

DESCRIPTION
  Transmits a specified RLS response message on a specified DLCI on
  which a RLS command was received.

===========================================================================*/
void bt_rc_tx_rls_response
(
  bt_rc_rx_info_type*  rxi_ptr,
  bt_rc_mcc_rls_type*  rls_ptr
)
{

  dsm_item_type*             tx_dsm_ptr;
  bt_rc_mcc_rls_frame_type   tx_rls;

  BT_MSG_SIG( "BT RC: TX RLS Resp V1 %x DLCI %x CID %x",
              rls_ptr->value1,
              RCMCCM_MDLCI_2_DLCI( rls_ptr->dlci ),
              rxi_ptr->ssn_ptr->cid );

  tx_dsm_ptr = bt_get_free_dsm_ptr( BT_TL_L2CAP,
                 sizeof( bt_rc_mcc_rls_frame_type ) );


  tx_rls.rcf_hdr.address = RCFH_DLCI_2_ADDR( RC_DLCI_MCC_V );
  if ( rxi_ptr->ssn_ptr->initiator != FALSE )
  {
    tx_rls.rcf_hdr.address |= RC_CR_CMD_B;
  }
  tx_rls.rcf_hdr.control = RCFH_CTRL_FT_UIH_V;
  tx_rls.rcf_hdr.length =
    RCFH_VAL1_2_LEN1( sizeof( bt_rc_mcc_rls_msg_type ) );
  
  tx_rls.rls_msg.mcc_hdr.type =
    RCMCCH_MT_TO_TYPE( RCMCCH_TYPE_MT_RLS_V );
  
  tx_rls.rls_msg.mcc_hdr.length =
    RCMCCH_VALLSB_2_LENLSB( rxi_ptr->mcch_length );

  memcpy( (byte*)(&tx_rls.rls_msg.rls_body),
          (byte*)(rls_ptr), sizeof( bt_rc_mcc_rls_type ) );
  
  tx_rls.rcf_fcs = bt_rc_calculate_fcs(
                      (uint8*)(&(tx_rls.rcf_hdr.address)),
                      BT_RC_FCS_BYTES_UIH );

  bt_dsm_pushdown_tail( &tx_dsm_ptr, (void*)&tx_rls, sizeof( tx_rls ),
                        DSM_DS_SMALL_ITEM_POOL );

  bt_rc_tx_signaling_on_l2cap( rxi_ptr->ssn_ptr,
                               rxi_ptr->dlc_ptr,
                               tx_dsm_ptr );

  rxi_ptr->dlc_ptr->tx_uih_frames++;

}


/*===========================================================================

FUNCTION
  bt_rc_tx_fc_command

DESCRIPTION
  Transmits either an FCon or FCoff command message (as appropriate)
  for a specified session.

===========================================================================*/
void bt_rc_tx_fc_command
(
  bt_rc_session_type*  ssn_ptr
)
{

  dsm_item_type*            dsm_ptr;
  bt_rc_mcc_fc_frame_type   fc;

  dsm_ptr = bt_get_free_dsm_ptr( BT_TL_L2CAP,
                                 sizeof( bt_rc_mcc_fc_frame_type ) );

  fc.rcf_hdr.address = RCFH_DLCI_2_ADDR( RC_DLCI_MCC_V );
  if ( ssn_ptr->initiator != FALSE )
  {
    fc.rcf_hdr.address |= RC_CR_CMD_B;
  }
  fc.rcf_hdr.control = RCFH_CTRL_FT_UIH_V;
  fc.rcf_hdr.length =
    RCFH_VAL1_2_LEN1( sizeof( bt_rc_mcc_hdr_type ) );

  if ( ssn_ptr->mcc.local_v24_signals & RCMCCM_MSC_V24_FC_FLOW_OFF_B )
  {
    fc.mcc_hdr.type = RCMCCH_MT_TO_TYPE( RCMCCH_TYPE_MT_FCOFF_V );
    ssn_ptr->mcc.expected_rx_mask |= BT_ERXM_FCOFF_RESP_B;
    BT_MSG_SIG( "BT RC: TX FCOff Cmd CID %x LV24 %x",
                ssn_ptr->cid, ssn_ptr->mcc.local_v24_signals, 0 );
  }
  else
  {
    fc.mcc_hdr.type = RCMCCH_MT_TO_TYPE( RCMCCH_TYPE_MT_FCON_V );
    ssn_ptr->mcc.expected_rx_mask |= BT_ERXM_FCON_RESP_B;
    BT_MSG_SIG( "BT RC: TX FCOn Cmd CID %x LV24 %x",
                ssn_ptr->cid, ssn_ptr->mcc.local_v24_signals, 0 );
  }
  fc.mcc_hdr.type |= RC_CR_CMD_B;
  fc.mcc_hdr.length = RCMCCH_VALLSB_2_LENLSB( 0 );

  fc.rcf_fcs = bt_rc_calculate_fcs(
                      (uint8*)(&(fc.rcf_hdr.address)),
                      BT_RC_FCS_BYTES_UIH );

  bt_dsm_pushdown_tail( &dsm_ptr, (void*)&fc, sizeof( fc ),
                        DSM_DS_SMALL_ITEM_POOL );

  bt_rc_tx_signaling_on_l2cap( ssn_ptr, &ssn_ptr->mcc, dsm_ptr );

  ssn_ptr->mcc.tx_uih_frames++;

}


/*===========================================================================

FUNCTION
  bt_rc_tx_fc_response

DESCRIPTION
  Transmits either an FCon or FCoff command response message (as
  appropriate) for a specified session.

===========================================================================*/
void bt_rc_tx_fc_response
(
  bt_rc_session_type*  ssn_ptr
)
{

  dsm_item_type*            dsm_ptr;
  bt_rc_mcc_fc_frame_type   fc;

  dsm_ptr = bt_get_free_dsm_ptr( BT_TL_L2CAP,
                                 sizeof( bt_rc_mcc_fc_frame_type ) );
  
  fc.rcf_hdr.address = RCFH_DLCI_2_ADDR( RC_DLCI_MCC_V );
  if ( ssn_ptr->initiator != FALSE )
  {
    fc.rcf_hdr.address |= RC_CR_CMD_B;
  }
  fc.rcf_hdr.control = RCFH_CTRL_FT_UIH_V;
  fc.rcf_hdr.length =
    RCFH_VAL1_2_LEN1( sizeof( bt_rc_mcc_hdr_type ) );
  
  if ( ssn_ptr->mcc.far_v24_signals & RCMCCM_MSC_V24_FC_FLOW_OFF_B )
  {
    fc.mcc_hdr.type = RCMCCH_MT_TO_TYPE( RCMCCH_TYPE_MT_FCOFF_V );
    BT_MSG_SIG( "BT RC: TX FCOff Resp CID %x FV24 %x",
                ssn_ptr->cid, ssn_ptr->mcc.far_v24_signals, 0 );
  }
  else
  {
    fc.mcc_hdr.type = RCMCCH_MT_TO_TYPE( RCMCCH_TYPE_MT_FCON_V );
    BT_MSG_SIG( "BT RC: TX FCOn Resp CID %x FV24 %x",
                ssn_ptr->cid, ssn_ptr->mcc.far_v24_signals, 0 );
  }
  fc.mcc_hdr.length = RCMCCH_VALLSB_2_LENLSB( 0 );

  fc.rcf_fcs = bt_rc_calculate_fcs(
                      (uint8*)(&(fc.rcf_hdr.address)),
                      BT_RC_FCS_BYTES_UIH );

  bt_dsm_pushdown_tail( &dsm_ptr, (void*)&fc, sizeof( fc ),
                        DSM_DS_SMALL_ITEM_POOL );

  bt_rc_tx_signaling_on_l2cap( ssn_ptr, &ssn_ptr->mcc, dsm_ptr );

  ssn_ptr->mcc.tx_uih_frames++;

}


/*===========================================================================

FUNCTION
  bt_rc_tx_mcc_test_command

DESCRIPTION
  Transmits a test command of specified format on a specified
  RFCOMM session.

===========================================================================*/
extern void bt_rc_tx_mcc_test_command
(
  bt_rc_session_type*  ssn_ptr,
  uint16               byte_count,
  uint8                start_value,
  uint8                increment
)
{

  uint16                 i = 0;
  uint16                 pkt_length, frame_length;
  word                   rcfh_length;
  word                   mcch_length;
  bt_rc_frame_hdre_type  rcfe_hdr;
  bt_rc_mcc_hdre_type    mcce_hdr;
  dsm_item_type*         dsm_ptr;
  bt_rc_fcs_type         fcs;
  
  BT_MSG_SIG( "BT RC: TX Test Cmd Len %x St %x CID %x",
              byte_count, start_value, ssn_ptr->cid );
  
  ssn_ptr->mcc.expected_rx_mask |= BT_ERXM_TEST_RESP_B;
  ssn_ptr->test_cmds_txd++;
  ssn_ptr->last_test_tx_byte_count  = byte_count;
  ssn_ptr->last_test_tx_start_value = start_value;
  ssn_ptr->last_test_tx_increment   = increment;
  
  mcce_hdr.type = RCMCCH_MT_TO_TYPE( RCMCCH_TYPE_MT_TEST_V );
  mcce_hdr.type |= RC_CR_CMD_B;
  
  if ( byte_count <= RCMCCH_LEN_MAX_NON_EXT )
  {
    mcce_hdr.length1 = RCMCCH_VALLSB_2_LENLSB( byte_count );
    mcce_hdr.length2 = 0;
    mcch_length = sizeof( bt_rc_mcc_hdr_type );
  }
  else
  {
    mcce_hdr.length1 = ((byte_count << 1) & RCMCCH_LEN_M);
    mcce_hdr.length2 = RCMCCH_VALLSB_2_LENLSB( (byte_count >> 7) );
    mcch_length = sizeof( bt_rc_mcc_hdre_type );
  }
  
  rcfe_hdr.address = RCFH_DLCI_2_ADDR( RC_DLCI_MCC_V );
  if ( ssn_ptr->initiator != FALSE )
  {
    rcfe_hdr.address |= RC_CR_CMD_B;
  }
  rcfe_hdr.control = RCFH_CTRL_FT_UIH_V;
  
  pkt_length = mcch_length + byte_count;
  if ( pkt_length <= RCFH_LEN_MAX_NON_EXT )
  {
    rcfe_hdr.length1 = RCFH_VAL1_2_LEN1( pkt_length );
    rcfe_hdr.length2 = 0;
    rcfh_length = sizeof( bt_rc_frame_hdr_type );
  }
  else
  {
    rcfe_hdr.length1 = ((pkt_length << 1) & RCFH_LEN_LSB_M);
    rcfe_hdr.length2 = ((pkt_length >> 7) & RCFH_LEN_MSB_M);
    rcfh_length = sizeof( bt_rc_frame_hdre_type );
  }
  
  frame_length = rcfh_length + pkt_length + sizeof( bt_rc_fcs_type );
  
  dsm_ptr = bt_get_free_dsm_ptr( BT_TL_L2CAP, frame_length );
  
  bt_dsm_pushdown_tail( &dsm_ptr, (byte*)(&rcfe_hdr), rcfh_length,
                        DSM_DS_SMALL_ITEM_POOL );

  bt_dsm_pushdown_tail( &dsm_ptr, (byte*)(&mcce_hdr), mcch_length,
                        DSM_DS_SMALL_ITEM_POOL );
  
  while ( i < byte_count )
  {
    bt_dsm_pushdown_tail( &dsm_ptr, &start_value, sizeof( uint8 ),
                          DSM_DS_SMALL_ITEM_POOL );
    start_value += increment;
    i++;
  }
  
  fcs = bt_rc_calculate_fcs( (uint8*)(&rcfe_hdr.address),
                             BT_RC_FCS_BYTES_UIH );
  
  bt_dsm_pushdown_tail( &dsm_ptr, &fcs, sizeof( fcs ),
                        DSM_DS_SMALL_ITEM_POOL );
    
  bt_rc_tx_signaling_on_l2cap( ssn_ptr, &ssn_ptr->mcc, dsm_ptr );
  
  ssn_ptr->mcc.tx_uih_frames++;

}


/*===========================================================================

FUNCTION
  bt_rc_tx_mcc_test_response

DESCRIPTION
  Transmits a Test response for a specified received Test command.

===========================================================================*/
void bt_rc_tx_mcc_test_response
(
  bt_rc_rx_info_type*  rxi_ptr
)
{

  uint16                 byte_count, pkt_length, frame_length;
  word                   rcfh_length;
  word                   mcch_length;
  bt_rc_frame_hdre_type  rcfe_hdr;
  bt_rc_mcc_hdre_type    mcce_hdr;
  dsm_item_type*         dsm_ptr;
  dsm_item_type*         dup_dsm_ptr;
  bt_rc_fcs_type         fcs;
  uint8                  peek_byte;
  
  rxi_ptr->ssn_ptr->test_resps_txd++;
  
  mcce_hdr.type = RCMCCH_MT_TO_TYPE( RCMCCH_TYPE_MT_TEST_V );
  
  byte_count = rxi_ptr->mcch_length;
  if ( byte_count != 0 )
  {
    dsm_peek_byte( rxi_ptr->dsm_ptr, 0, &peek_byte );
    BT_MSG_SIG( "BT RC: TX Test Resp Len %x St %x CID %x",
                byte_count, peek_byte,
                rxi_ptr->ssn_ptr->cid );
  }
  else
  {
    BT_MSG_SIG( "BT RC: TX Test Resp Len 0 CID %x",
                rxi_ptr->ssn_ptr->cid, 0, 0 );
  }

  if ( byte_count <= RCMCCH_LEN_MAX_NON_EXT )
  {
    mcce_hdr.length1 = RCMCCH_VALLSB_2_LENLSB( byte_count );
    mcce_hdr.length2 = 0;
    mcch_length = sizeof( bt_rc_mcc_hdr_type );
  }
  else
  {
    mcce_hdr.length1 = ((byte_count << 1) & RCMCCH_LEN_M);
    mcce_hdr.length2 = RCMCCH_VALLSB_2_LENLSB( (byte_count>>7) );
    mcch_length = sizeof( bt_rc_mcc_hdre_type );
  }
  
  rcfe_hdr.address = RCFH_DLCI_2_ADDR( RC_DLCI_MCC_V );
  if ( rxi_ptr->ssn_ptr->initiator != FALSE )
  {
    rcfe_hdr.address |= RC_CR_CMD_B;
  }
  rcfe_hdr.control = RCFH_CTRL_FT_UIH_V;
  
  pkt_length = mcch_length + byte_count;
  if ( pkt_length <= RCFH_LEN_MAX_NON_EXT )
  {
    rcfe_hdr.length1 = RCFH_VAL1_2_LEN1( pkt_length );
    rcfe_hdr.length2 = 0;
    rcfh_length = sizeof( bt_rc_frame_hdr_type );
  }
  else
  {
    rcfe_hdr.length1 = ((pkt_length << 1) & RCFH_LEN_LSB_M);
    rcfe_hdr.length2 = ((pkt_length >> 7) & RCFH_LEN_MSB_M);
    rcfh_length = sizeof( bt_rc_frame_hdre_type );
  }

  frame_length = rcfh_length + pkt_length + sizeof( bt_rc_fcs_type );

  dsm_ptr = bt_get_free_dsm_ptr( BT_TL_L2CAP, frame_length );
  
  bt_dsm_pushdown_tail( &dsm_ptr, (byte*)(&rcfe_hdr), rcfh_length,
                        DSM_DS_SMALL_ITEM_POOL );

  bt_dsm_pushdown_tail( &dsm_ptr, (byte*)(&mcce_hdr), mcch_length,
                        DSM_DS_SMALL_ITEM_POOL );

  /* The check below is required to prevent dsm operations in case of
   * NULL rxi_ptr, which may otherwise cause illegal memory accesses
   */
  if (rxi_ptr->dsm_ptr != NULL)
  {
    dsm_dup_packet( &dup_dsm_ptr, rxi_ptr->dsm_ptr, 0, byte_count );
    dsm_pullup( &rxi_ptr->dsm_ptr, NULL, byte_count );
    dsm_append( &dsm_ptr, &dup_dsm_ptr );
  }
  
  fcs = bt_rc_calculate_fcs( (uint8*)(&rcfe_hdr.address),
                             BT_RC_FCS_BYTES_UIH );

  bt_dsm_pushdown_tail( &dsm_ptr, &fcs, sizeof( fcs ),
                        DSM_DS_SMALL_ITEM_POOL );

  bt_rc_tx_signaling_on_l2cap( rxi_ptr->ssn_ptr,
                               rxi_ptr->dlc_ptr,
                               dsm_ptr );

  rxi_ptr->ssn_ptr->mcc.tx_uih_frames++;

}

#endif /* FEATURE_BT */

